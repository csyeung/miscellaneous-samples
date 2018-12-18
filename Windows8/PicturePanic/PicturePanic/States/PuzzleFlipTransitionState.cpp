#include "pch.h"
#include "PuzzleFlipTransitionState.h"
#include "GameState/GameStateManager.h"
#include "Resources/ResourceFactory.h"
#include "Utility/Utility.h"
#include <time.h>

using namespace GameFramework::Resources;
using namespace GameFramework::Utility;
using namespace GameFramework::Globals;

const float PuzzleFlipTransitionState::CAMERA_ASPECTRATIO = 1.0f;
const float PuzzleFlipTransitionState::CAMERA_FOV		  = 90.0f;
const float PuzzleFlipTransitionState::CAMERA_NEARCLIP	  = 0.01f;
const float PuzzleFlipTransitionState::CAMERA_FARCLIP	  = 100.0f;
const float PuzzleFlipTransitionState::CAMERA_POS_Z		  = -40.0f;


PuzzleFlipTransitionState::PuzzleFlipTransitionState() : RenderableGameState(L"PuzzleFlipTransition"), 
													     _numRows(-1), _numColumns(-1), _delayTime(0)
{
	_pCamera = new PerspectiveCamera(CAMERA_ASPECTRATIO, CAMERA_FOV, CAMERA_NEARCLIP, CAMERA_FARCLIP);

	XMFLOAT3 vEye(0, 0, CAMERA_POS_Z);
	XMFLOAT3 vLookAt(0, 0, 0);
	XMFLOAT3 vUp(0, 1, 0);
	
	_pCamera->SetLookAt(vEye, vLookAt, vUp);
}


void PuzzleFlipTransitionState::OnEntered()
{
	GameState::OnEntered();

	_numRows = 5;
	_numColumns = 8;

	CreateBuffers();
	CreatePieces();

	CreateRenderTargets();

	// Set the rendertarget for all states below this one
	GameStateManager::Instance->StatesRenderTarget = _renderTarget;
	GameStateManager::Instance->StatesDepthStencil = _depthStencil;
	GameStateManager::Instance->StateRequiringRenderTarget = this;

	_textureSide = ResourceFactory::CreateTexture2DResource("Assets\\UI\\Transition\\side.png");

	PuzzlePiece::SetCustomFrontTexture(_renderTarget);
	PuzzlePiece::SetCustomBackTexture(_textureBack);
	PuzzlePiece::SetCustomSidesTexture(_textureSide);
	
	SetupReveal();
}


void PuzzleFlipTransitionState::OnWindowSizeChange(_In_ float screenWidth, _In_ float screenHeight)
{
	PuzzlePiece::OnWindowSizeChange(GameGlobals::D3DDevice, GameGlobals::D3DContext, screenWidth, screenHeight);
}


void PuzzleFlipTransitionState::CreateBuffers()
{
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	depthStencilDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	DXCall(GameGlobals::D3DDevice->CreateDepthStencilState(&depthStencilDesc, &_depthStencilState));
}


void PuzzleFlipTransitionState::CreatePieces()
{
	const int numPieces = _numRows * _numColumns;

	XMFLOAT3 eyePos = _pCamera->Position();

	XMVECTOR vCameraPos = XMLoadFloat3(&eyePos);

	
	float height = -XMVectorGetZ(vCameraPos) * tan (_pCamera->FieldOfView() / 2);
	float width = height * (_pCamera->AspectRatio());

	float pieceWidth = width / _numColumns;
	float pieceHeight = height / _numRows;

	float xCur = float(-_numColumns + 1);
	float yCur = float(_numRows - 1);

	float u = 0;
	float uStep = 1.0f / _numColumns;
	float v = 0;
	float vStep = 1.0f / _numRows;

	ID3D11Device1* pDevice = GameGlobals::D3DDevice;
	ID3D11DeviceContext1* pContext = GameGlobals::D3DContext;

	for (int iPiece = 0; iPiece < numPieces; ++iPiece)
	{
		PuzzlePiece^ piece = ref new PuzzlePiece(pieceWidth, pieceHeight, 1.0f);
		piece->InitializePiece(pDevice);

		// for transition initialize a custom back piece
		piece->InitializeCustomBackPiece(pDevice);

		// homogenized coords
		piece->SetLocation(xCur, yCur, 0);
		piece->SetCorrectUVs(pContext, u, v, u + uStep, v + vStep);

		xCur += 2;
		u += uStep;

		if (xCur > _numColumns)
		{
			// next row
			xCur = float(-_numColumns + 1);
			yCur -= 2.0f;

			u = 0;
			v += vStep;
	
		}

		_vecPuzzlePieces.push_back(piece);
	}
}


void PuzzleFlipTransitionState::CreateRenderTargets()
{
	_renderTarget = ResourceFactory::CreateRenderTargetResource(static_cast<int>(GameGlobals::WindowWidth), static_cast<int>(GameGlobals::WindowHeight));
	_depthStencil = ResourceFactory::CreateDepthStencilResource(static_cast<int>(_renderTarget->Width), static_cast<int>(_renderTarget->Height));
}


void PuzzleFlipTransitionState::SetupReveal()
{
	srand(unsigned(time(NULL)));

	int revealType = Random(0, 4);

	//revealType = 4;
	
	switch(revealType)
	{
	case 0:
		SetupLeftRevealWipe();
		break;

	case 1:
		SetupDiagonalRevealWipe();
		break;

	case 2:
		SetupRandomRevealWipe();
		break;

	case 3:
		SetupTopRevealWipe();
		break;

	case 4:
		SetupReverseDiagonalRevealWipe();
		break;
	}
}


void PuzzleFlipTransitionState::SetupLeftRevealWipe()
{
	float delayCurrent = _delayTime;
	float delayStep = 0.15f;
	float timeToShow = 0.7f;

	for (unsigned int i = 0; i < _vecPuzzlePieces.size(); ++i)
	{
		if (i % (_numColumns) == 0)
		{
			delayCurrent = _delayTime;
		}
		else
		{
			delayCurrent += delayStep;
		}

		_vecPuzzlePieces[i]->Show(delayCurrent, timeToShow);		
	}
}


void PuzzleFlipTransitionState::SetupDiagonalRevealWipe()
{
	int numPieces = _vecPuzzlePieces.size();

	int currentRow = 0;
	int currentCol = 0;
	float delayAmount = _delayTime;
	float delayStep = 0.15f;
	float timeToShow = 0.75f;

	for (unsigned int iPiece = 0; iPiece < _vecPuzzlePieces.size(); ++iPiece)
	{
		PuzzlePiece^ piece = _vecPuzzlePieces[iPiece];

		piece->Show(delayAmount, timeToShow);

		++currentCol;

		delayAmount += delayStep;

		if (currentCol == _numColumns)
		{
			currentCol = 0;
			++currentRow;

			delayAmount = _delayTime + (currentRow * delayStep);
		}
	}
}


void PuzzleFlipTransitionState::SetupRandomRevealWipe()
{
	int numPieces = _vecPuzzlePieces.size();

	float delayCurrent = _delayTime;
	float delayStep = 0.025f;
	float timeToShow = 0.7f;

	srand(unsigned(time(NULL)));
 
	std::vector<PuzzlePiece^> vecTempShuffle(numPieces);


	for (int i = 0; i < numPieces; ++i)
	{
		vecTempShuffle[i] = _vecPuzzlePieces[i];
	}

	// Shuffle
	std::random_shuffle(vecTempShuffle.begin(), vecTempShuffle.end());

	for (unsigned int i = 0; i < vecTempShuffle.size(); ++i)
	{
		vecTempShuffle[i]->Show(delayCurrent, timeToShow);		

		delayCurrent += delayStep;
	}
}


void PuzzleFlipTransitionState::SetupTopRevealWipe()
{
	float delayCurrent = _delayTime;
	float delayStep = 0.25f;
	float timeToShow = 0.7f;

	for (unsigned int i = 0; i < _vecPuzzlePieces.size(); ++i)
	{
		if (i % _numColumns == 0)
		{
			delayCurrent += delayStep;
		}

		_vecPuzzlePieces[i]->Show(delayCurrent, timeToShow);		
	}
}


void PuzzleFlipTransitionState::SetupReverseDiagonalRevealWipe()
{
	int numPieces = _vecPuzzlePieces.size();

	int currentRow = 0;
	int currentCol = 0;
	float delayAmount = _delayTime;
	float delayStep = 0.15f;
	float timeToShow = 0.75f;

	int iPiece =  _vecPuzzlePieces.size() - 1;

	for (iPiece; iPiece >= 0 ; --iPiece)
	{
		PuzzlePiece^ piece = _vecPuzzlePieces[iPiece];

		piece->Show(delayAmount, timeToShow);

		++currentCol;

		delayAmount += delayStep;

		if (currentCol == _numColumns)
		{
			currentCol = 0;
			++currentRow;

			delayAmount = _delayTime + (currentRow * delayStep);
		}
	}
}


bool PuzzleFlipTransitionState::Update(float timeTotal, float timeDelta)
{
	int numPieces = _vecPuzzlePieces.size();

	bool animated = false;

	for (int iPiece = 0; iPiece < numPieces; ++iPiece)
	{
		PuzzlePiece^ piece = _vecPuzzlePieces[iPiece];

		if (piece->IsAnimating)
		{
			animated = true;
		}

		piece->Update(timeTotal, timeDelta, _pCamera);
	}

	if (!animated)
	{
		// clean up custom render target
		GameStateManager::Instance->StateRequiringRenderTarget = nullptr;
		GameStateManager::Instance->StatesDepthStencil = nullptr;
		GameStateManager::Instance->StatesRenderTarget = nullptr;
	}

	return animated;	
}


void PuzzleFlipTransitionState::Render(_In_ float timeTotal, _In_ float timeDelta)
{
	ID3D11DeviceContext1* pContext = GameGlobals::D3DContext;

	pContext->OMSetDepthStencilState(_depthStencilState.Get(), 1);

	float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

	pContext->ClearRenderTargetView(GameGlobals::RenderTargetView, clearColor);


	int numPieces = _vecPuzzlePieces.size();

	for (int iPiece = 0; iPiece < numPieces; ++iPiece)
	{
		PuzzlePiece^ piece = _vecPuzzlePieces[iPiece];

		piece->RenderAsTransition(pContext);
	}
}


