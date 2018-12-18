#include "pch.h"
#include "RaceGameState.h"
#include <time.h>
#include "Resources/ResourceFactory.h"
#include "GameState/GameStateManager.h"
#include "RaceSummaryState.h"
#include "MenuState.h"
#include "Utility/Easing.h"

using namespace GameFramework::GameState;
using namespace GameFramework::Utility;
using namespace GameFramework::Globals;

const float RaceGameState::CAMERA_ASPECTRATIO = 1.0f;
const float RaceGameState::CAMERA_FOV		  = 90.0f;
const float RaceGameState::CAMERA_NEARCLIP	  = 0.01f;
const float RaceGameState::CAMERA_FARCLIP	  = 100.0f;
const float RaceGameState::CAMERA_POS_Z		  = -40.0f;
const int RaceGameState::MAX_PUZZLES		  = 7;
const float RaceGameState::PUZZLE_STARTUPTIME = 1.0f;
const float RaceGameState::PUZZLE_SHOWTIME    = 2.0f;
const float RaceGameState::PUZZLE_HIDETIME    = 1.1f;


RaceGameState::RaceGameState(_In_ MenuState^ stateMainMenu) : RenderableGameState(L"RaceGameState"), _pCamera(nullptr), _currentPuzzle(0), _numRows(-1), _numColumns(-1), 
															  _gamestateCountdown(nullptr), _gamestateTimer(nullptr), _gamestateSummary(nullptr),
															  _stateMainMenu(stateMainMenu), _renderTargetYStart(0), _renderTargetMoveTime(-1)
{
	_pCamera = new PerspectiveCamera(CAMERA_ASPECTRATIO, CAMERA_FOV, CAMERA_NEARCLIP, CAMERA_FARCLIP);

	// create states for this game mode
	_gamestateCountdown  = ref new CountdownState();
	
	_gamestateTimer      = ref new GameTimerState();
	
	_gamestateSummary    = ref new RaceSummaryState(this);
	_gamestateSummary->ExitTransition = true;
	
	_gamestateFadeTransition = ref new FadeTransitionState();	

	Reset();
}


void RaceGameState::Reset()
{
	_currentPuzzle = 0;

	_startUpTime = PUZZLE_STARTUPTIME;
	_showPuzzleTime = PUZZLE_SHOWTIME;
	_hidePuzzleTime = PUZZLE_HIDETIME;

	for (unsigned int i = 0; i < _vecParticleSystem2Ds.size(); ++i)
	{
		ParticleSystem2D^ particleSystem = _vecParticleSystem2Ds[i];
		
		particleSystem->Release();
	}

	_vecParticleSystem2Ds.clear();


	for (unsigned int iPiece = 0; iPiece < _vecPuzzlePieces.size(); ++iPiece)
	{
		PuzzlePiece^ piece = _vecPuzzlePieces[iPiece];

		piece->Hide(0.0f, 0.0f);
	}
}


void RaceGameState::OnEntered()
{
	GameState::OnEntered();

	Reset();

	_game = PicturePanic::Instance;

	if (_pSpriteRenderer == nullptr)
	{
		_pSpriteRenderer = new SpriteRenderer();
		_pSpriteRenderer->Initialize(GameGlobals::D3DDevice);
	}

	_pFirstSelectedPiece = nullptr;

	// Initialize ParticleSystem
	for (int i = 0; i < _numColumns; ++i)
	{
		ParticleSystem2D^ particleSystem = ref new ParticleSystem2D();

		particleSystem->Initialize(GameGlobals::D3DDevice);

		_vecParticleSystem2Ds.push_back(particleSystem);
	}

	CreateRaceGameResources();
	SetupCelebrationEffects();

	// build the puzzle pieces
	// first set the camera to the desired position for building the puzzle
	if (_vecPuzzlePieces.size() == 0)
	{
		XMFLOAT3 vEye(0, 0, CAMERA_POS_Z);
		XMFLOAT3 vLookAt(0, 0, 0);
		XMFLOAT3 vUp(0, 1, 0);
	
		_pCamera->SetLookAt(vEye, vLookAt, vUp);

		CreatePuzzlePieces();
	}

	// adjust the camera to give a margin of space around the pieces (back it up)
	XMVECTOR vNewPos = XMVectorSet(0, 0, CAMERA_POS_Z - 2, 0);

	_pCamera->SetPosition(vNewPos);

	_state = CurrentGameState::STARTUP;

	SelectNextPuzzle();
}


void RaceGameState::OnWindowSizeChange(_In_ float screenWidth, _In_ float screenHeight)
{
	CreateRaceGameResources();

	PuzzlePiece::OnWindowSizeChange(GameGlobals::D3DDevice, GameGlobals::D3DContext, screenWidth, screenWidth);
}


void RaceGameState::CreateRaceGameResources()
{
	CreateCorrectEffect();

	if (_renderTargetPuzzle != nullptr)
	{
		_renderTargetPuzzle->Release();
		_renderTargetPuzzle = nullptr;
	}

	if (_depthStencil != nullptr)
	{
		_depthStencil->Release();
		_depthStencil = nullptr;
	}

	if (_depthStencilState != nullptr)
	{
		_depthStencilState = nullptr;
	}


	int renderWindowSize = static_cast<int>(_game->RenderWindowSize);

	_renderTargetPuzzle = ResourceFactory::CreateRenderTargetResource(renderWindowSize, renderWindowSize);

	switch(ApplicationView::Value)
	{
		case ApplicationViewState::Filled:
			// center the puzzle render target in y
			_renderTargetYCurrent = _renderTargetYStart;

			_renderTargetYTarget = GameGlobals::WindowHeight / 2.0f - (renderWindowSize / 2.0f) - _renderTargetYStart;

			_renderTargetMoveTime = 1.2f;

			break;

		case ApplicationViewState::Snapped:
		case ApplicationViewState::FullScreenLandscape:
			if (_renderTargetYStart != 0)
			{
				_renderTargetYCurrent = _renderTargetYStart;
				_renderTargetYTarget = 0 - _renderTargetYStart;

				_renderTargetMoveTime = 1.2f;
			}

			break;
	}
		
	_depthStencil = ResourceFactory::CreateDepthStencilResource(renderWindowSize, renderWindowSize);

	if (_depthStencilState == nullptr)
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
}


void RaceGameState::CreateCorrectEffect()
{
	int minDimension = static_cast<int>(_game->RenderWindowSize);

	if (_effectCorrect && _effectCorrect->RenderTargetWidth != minDimension)
	{
		_effectCorrect->Release();
		_effectCorrect = nullptr;
	}
	
	if (_effectCorrect == nullptr)
	{
		_effectCorrect = ref new PostHaloEffect(minDimension, minDimension);

		_effectCorrect->Initialize(GameGlobals::D3DDevice);
	}
}


void RaceGameState::CreatePuzzlePieces()
{
	const int NumPieces = _numRows * _numColumns;

	XMFLOAT3 eyePos = _pCamera->Position();

	XMVECTOR vCameraPos = XMLoadFloat3(&eyePos);

	float height = -XMVectorGetZ(vCameraPos) * tan (_pCamera->FieldOfView() / 2);
	float width  = height * (_pCamera->AspectRatio());

	float pieceWidth  = width / _numColumns;
	float pieceHeight = height / _numRows;

	float xCur = float(-_numColumns + 1);
	float yCur = float(_numRows - 1);

	float u = 0;
	float uStep = 1.0f / _numColumns;
	float v = 0;
	float vStep = 1.0f / _numRows;

	for (int iPiece = 0; iPiece < NumPieces; ++iPiece)
	{
		PuzzlePiece^ piece = ref new PuzzlePiece(pieceWidth, pieceHeight, 1.0f);
		piece->InitializePiece(GameGlobals::D3DDevice);

		// homogenized coords
		piece->SetLocation(xCur, yCur, 0);
		piece->SetCorrectUVs(GameGlobals::D3DContext, u, v, u + uStep, v + vStep);

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


void RaceGameState::SetupCelebrationEffects()
{
	// 1 particle system per column
	const float rate = 100.0f;
	const XMFLOAT2 direction = XMFLOAT2(0, -1);
	ParticleSystem2D::MinMax angleDeviation;

	angleDeviation.min = -25;
	angleDeviation.max = 25;

	ParticleSystem2D::Particle2DDescription desc;
	ZeroMemory(&desc, sizeof(desc));

	desc.lifeTime.min = 5.0f;
	desc.lifeTime.max = 5.0f;
	desc.scale.min = 1.0f;
	desc.scale.max = 2.0f;
	desc.velocity.min = 2300;
	desc.velocity.max = 3000;
	desc.angularVelocity.min = -30;
	desc.angularVelocity.max = 30;
	desc.useGravity = true;
	desc.drag.min = 0.98f;
	desc.drag.max = 0.99f;
	desc.fadeInPercent.min = 0.0f;
	desc.fadeInPercent.max = 0.0f;
	desc.fadeOutPercent.min = 0.2f;
	desc.fadeOutPercent.max = 0.2f;

	float xCurrent = 0.0f;
	float xStep = 1080.0f / _numColumns;

	float currentDelay = 0.0f;
	float delayStep = 0.2f;
	
	if (_numColumns == 3)
	{
		xCurrent = 100.0f;
	}

	for (int i = 0; i < _numColumns; ++i)
	{
		_vecParticleSystem2Ds[i]->Reset();

		_vecParticleSystem2Ds[i]->Position = XMFLOAT2(xCurrent, 1080);
		_vecParticleSystem2Ds[i]->Direction = direction;
		_vecParticleSystem2Ds[i]->Rate = rate;
		_vecParticleSystem2Ds[i]->AngleDeviationInDegrees = angleDeviation;
		_vecParticleSystem2Ds[i]->Lifetime = 0.5f;
		_vecParticleSystem2Ds[i]->Delay = currentDelay;


		_vecParticleSystem2Ds[i]->AddColor(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
		_vecParticleSystem2Ds[i]->AddColor(XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
		_vecParticleSystem2Ds[i]->AddColor(XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f));
		_vecParticleSystem2Ds[i]->AddColor(XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f));
		_vecParticleSystem2Ds[i]->AddColor(XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f));
		_vecParticleSystem2Ds[i]->AddColor(XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f));
		_vecParticleSystem2Ds[i]->AddColor(XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f));

		desc.textureName = "Assets\\InGame\\ParticleSystem\\star.png";
		_vecParticleSystem2Ds[i]->AddParticleType(&desc);

		desc.textureName = "Assets\\InGame\\ParticleSystem\\diamond.png";
		_vecParticleSystem2Ds[i]->AddParticleType(&desc);

		desc.textureName = "Assets\\InGame\\ParticleSystem\\circle.png";
		_vecParticleSystem2Ds[i]->AddParticleType(&desc);

		xCurrent += xStep;
		currentDelay += delayStep;
	}
}


void RaceGameState::SelectNextPuzzle()
{
	++_currentPuzzle;

	if (_currentPuzzle > MAX_PUZZLES)
	{
		_currentPuzzle = 1;
	}

	PuzzlePiece::LoadNewPuzzle(_puzzleImagePath, _currentPuzzle);
}


void RaceGameState::ResetPuzzle()
{
	int numPieces = _vecPuzzlePieces.size();

	for (int iPiece = 0; iPiece < numPieces; ++iPiece)
	{
		PuzzlePiece^ piece = _vecPuzzlePieces[iPiece];

		piece->ResetUVs(GameGlobals::D3DContext);
		piece->Reset();
	}
}


void RaceGameState::FlipPuzzle(_In_ bool hide, _In_ float timeToFlip)
{
	int numPieces = _vecPuzzlePieces.size();

	int currentRow = 0;
	int currentCol = 0;
	float delayAmount = 0.0f;
	float delayStep = 0.10f;

	for (int iPiece = 0; iPiece < numPieces; ++iPiece)
	{
		PuzzlePiece^ piece = _vecPuzzlePieces[iPiece];

		if (hide)
		{
			piece->Hide(delayAmount, timeToFlip);
		}
		else
		{
			piece->Show(delayAmount, timeToFlip);
		}

		++currentCol;

		delayAmount += delayStep;

		if (currentCol == _numColumns)
		{
			currentCol = 0;
			++currentRow;

			delayAmount = currentRow * delayStep;
		}
	}
}


void RaceGameState::ScramblePuzzle()
{
	int numPieces = _vecPuzzlePieces.size();

	
	//Seeding random number generator
	srand(unsigned(time(NULL)));
 
	int currentRow = 0;
	int currentCol = 0;
	float delayAmount = 0.0f;
	float delayStep = 0.10f;

	std::vector<PuzzlePiece^> vecTempShuffle(numPieces);


	for (int i = 0; i < numPieces; ++i)
	{
		vecTempShuffle[i] = _vecPuzzlePieces[i];
	}

	// Shuffle
	std::random_shuffle(vecTempShuffle.begin(), vecTempShuffle.end());

	PuzzlePiece^ originalPiece = vecTempShuffle.back();
	vecTempShuffle.pop_back();
	
	while (vecTempShuffle.size())
	{
		std::random_shuffle(vecTempShuffle.begin(), vecTempShuffle.end());
	
		PuzzlePiece^ swapPiece = vecTempShuffle.back();
		vecTempShuffle.pop_back();

		originalPiece->SwapWith(GameGlobals::D3DContext, swapPiece);

		if (vecTempShuffle.size())
		{
			originalPiece = vecTempShuffle.back();
			vecTempShuffle.pop_back();
		}
	}


	for (int iPiece = 0; iPiece < numPieces; ++iPiece)
	{
		PuzzlePiece^ piece = _vecPuzzlePieces[iPiece];

		piece->Show(delayAmount, 0.5f);

		++currentCol;

		delayAmount += delayStep;

		if (currentCol == _numColumns)
		{
			currentCol = 0;
			++currentRow;

			delayAmount = currentRow * delayStep;
		}
	}
}


bool RaceGameState::OnPointerPressed(_In_ PointerPoint^ point)
{
	// test input on puzzle board scene
	XMVECTOR vInputScreenSpace = XMVectorSet(point->Position.X, point->Position.Y - _renderTargetYStart, 0.0f, 1.0f);

	XMMATRIX matView = _pCamera->ViewMatrix();
	XMMATRIX matProjection = _pCamera->ProjectionMatrix();
	
	//XMMATRIX matWorld = XMMatrixTranslation(0, -_renderTargetYStart, 0);
	XMMATRIX matWorld = XMMatrixIdentity();

	XMVECTOR vInputObjectSpace = XMVector3Unproject(vInputScreenSpace, 0, 0, _renderTargetPuzzle->Width, _renderTargetPuzzle->Height, 0.0f, 1.0f, matProjection, matView, matWorld);

	XMFLOAT3 eyePos = _pCamera->Position();

	XMVECTOR vRayOrigin = XMLoadFloat3(&eyePos);
	XMVECTOR vRayDir = vInputObjectSpace - vRayOrigin;
	vRayDir = XMVector3Normalize(vRayDir);

	int numPieces = _vecPuzzlePieces.size();

	for (int i = 0; i < numPieces; ++i)
	{
		PuzzlePiece^ piece = _vecPuzzlePieces[i];

		if (piece->HitTest(vRayOrigin, vRayDir))
		{
			if (!piece->IsSelected())
			{
				piece->Select();

				if (_pFirstSelectedPiece == nullptr)
				{
					_pFirstSelectedPiece = piece;
				}
				else
				{
					// swap pieces
					_pFirstSelectedPiece->FlipAndSwapWithPiece(GameGlobals::D3DContext, piece);
					_pFirstSelectedPiece->UnSelect();

					_pFirstSelectedPiece = nullptr;

					piece->UnSelect();
				}
			}
			else
			{
				piece->UnSelect();

				_pFirstSelectedPiece = nullptr;
			}

			return true;
		}
	}

	return false;
}


void RaceGameState::Retry()
{
	GameStateManager::Instance->Pop();

	_startUpTime = PUZZLE_STARTUPTIME;
	_showPuzzleTime = PUZZLE_SHOWTIME;
	_hidePuzzleTime = PUZZLE_HIDETIME * 0.75f;

	FlipPuzzle(true, 1.0f);

	_timeToWaitForNextPuzzle = 1.0f;

	_state = CurrentGameState::STARTUP;
}


void RaceGameState::NextPuzzle()
{
	GameStateManager::Instance->Pop();

	_startUpTime = PUZZLE_STARTUPTIME;
	_showPuzzleTime = PUZZLE_SHOWTIME;
	_hidePuzzleTime = PUZZLE_HIDETIME * 0.75f;

	FlipPuzzle(true, 1.0f);

	_timeToWaitForNextPuzzle = 1.0f;

	_state = CurrentGameState::NEXTPUZZLE;
}

void RaceGameState::LeaveGame()
{
	GameStateManager::Instance->Pop();

	// create a state based on the chosen mode (race mode for now)
	GameStateManager::Instance->Push(_gamestateFadeTransition, GameStateModality::OVERLAY, true);

	// Prep the main menu screen to be on the right screen
	_stateMainMenu->Visible = false;
	_stateMainMenu->ReturnToDifficultyScreen();
}


void RaceGameState::UpdateStartUpState(_In_ float timeDelta)
{
	_startUpTime -= timeDelta;

	if (_startUpTime <= 0.0f)
	{
		ResetPuzzle();

		FlipPuzzle(false, 1.0f);

		_state = CurrentGameState::SHOWPUZZLE;
	}
}


void RaceGameState::UpdateShowPuzzleState(_In_ float timeDelta)
{
	_showPuzzleTime -= timeDelta;

	if (_showPuzzleTime <= 0.0f)
	{
		_state = CurrentGameState::COUNTDOWN;
		
		// add the countdown timer state
		GameStateManager::Instance->Push(_gamestateCountdown, GameStateModality::OVERLAY, true);
	}
}


void RaceGameState::UpdateCountDownState(_In_ float timeDelta)
{
	if (_gamestateCountdown->RemainingTime < 1.0f)
	{
		FlipPuzzle(true, 0.5f);

		_state = CurrentGameState::HIDEPUZZLE;
	}
}


void RaceGameState::UpdateHidePuzzleState(_In_ float timeDelta)
{
	_hidePuzzleTime -= timeDelta;

	if (_hidePuzzleTime <= 0.0f)
	{
		ScramblePuzzle();
		
		_gamestateTimer->TotalTime = 2 * 60;

		GameStateManager::Instance->Push(_gamestateTimer, GameStateModality::OVERLAY, true);

		_state = CurrentGameState::PLAY;
	}
}


void RaceGameState::UpdatePlayState(_In_ float timeDelta)
{
	if (_gamestateTimer->RemainingTime <= 0.0f)
	{
		_gamestateSummary->Win = false;

		GameStateManager::Instance->Push(_gamestateSummary, GameStateModality::OVERLAY, true);
					
		_state = CurrentGameState::LOSE;
	}
	else
	{
		// check for correct puzzle
		int numPiecees = _vecPuzzlePieces.size();

		bool correct = true;

		for (int i = 0; i < numPiecees; ++i)
		{
			PuzzlePiece^ piece = _vecPuzzlePieces[i];

			if (!piece->IsCorrect())
			{
				correct = false;
				break;
			}
		}

		if (correct)
		{
			_gamestateSummary->Win = true;

			// pop off the timer state
			GameStateManager::Instance->Pop();

			GameStateManager::Instance->Push(_gamestateSummary, GameStateModality::OVERLAY, true);
						
			_state = CurrentGameState::WIN;
			
			for (int i = 0; i < _numColumns; ++i)
			{
				_vecParticleSystem2Ds[i]->Start();
			}
		}
	}
}


void RaceGameState::UpdateNextPuzzleState(_In_ float timeDelta)
{
	_timeToWaitForNextPuzzle -= timeDelta;

	if (_timeToWaitForNextPuzzle <= 0.0f)
	{
		SelectNextPuzzle();

		_state = CurrentGameState::STARTUP;
	}
}


bool RaceGameState::Update(_In_ float timeTotal, _In_ float timeDelta)
{
	// update for current game state
	switch(_state)
	{
	// Puzzle is facing away
	case CurrentGameState::STARTUP:		UpdateStartUpState(timeDelta);		break;
	case CurrentGameState::SHOWPUZZLE:  UpdateShowPuzzleState(timeDelta);	break;
	case CurrentGameState::COUNTDOWN:   UpdateCountDownState(timeDelta);	break;
	case CurrentGameState::HIDEPUZZLE:	UpdateHidePuzzleState(timeDelta);	break;
	case CurrentGameState::PLAY:		UpdatePlayState(timeDelta);			break;
	case CurrentGameState::NEXTPUZZLE:  UpdateNextPuzzleState(timeDelta);	break;
	}

	// Update any active particle systems
	for (int i = 0; i < _numColumns; ++i)
	{
		ParticleSystem2D^ particleSystem = _vecParticleSystem2Ds[i];
		
		if (particleSystem->IsActive)
		{
			particleSystem->Update(timeDelta);
		}
	}

	// update the pieces
	int numPieces = _vecPuzzlePieces.size();

	for (int iPiece = 0; iPiece < numPieces; ++iPiece)
	{
		PuzzlePiece^ piece = _vecPuzzlePieces[iPiece];

		piece->Update(timeTotal, timeDelta, _pCamera);
	}

	if (_renderTargetMoveTime > -1)
	{
		if (_renderTargetMoveTime <= 1.0f)
		{
			float normalizedTime = 1.0f - _renderTargetMoveTime;

			float factor = BackEaseOut(normalizedTime, 0.7f);

			_renderTargetYStart = _renderTargetYCurrent + factor * _renderTargetYTarget;
		}

		_renderTargetMoveTime -= timeDelta;

		if (_renderTargetMoveTime <= 0)
		{
			_renderTargetMoveTime = -1;

			_renderTargetYStart = _renderTargetYCurrent + _renderTargetYTarget;
		}
	}

	return true;
}


void RaceGameState::Render(_In_ float timeTotal, _In_ float timeDelta)
{
	_effectCorrect->ViewMatrix		  = _pCamera->ViewMatrix();
	_effectCorrect->ProjectionMatrix  = _pCamera->ProjectionMatrix();

	RenderPuzzlePieces(timeTotal);

	ID3D11DeviceContext1* pContext = GameGlobals::D3DContext;
	CD3D11_VIEWPORT viewport(0.0f, 0.0f, GameGlobals::WindowWidth, GameGlobals::WindowHeight);

	pContext->RSSetViewports(1, &viewport);

	ID3D11RenderTargetView* pRenderTargetView = GameGlobals::RenderTargetView;
	pContext->OMSetRenderTargets(1, &pRenderTargetView, GameGlobals::DepthStencilView);

//	float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
//	pContext->ClearRenderTargetView(_game->RenderTargetView, ClearColor);
	
	_pSpriteRenderer->Begin(pContext, SpriteRenderer::FilterMode::Linear);

	// draw rendered puzzle
	XMMATRIX mat = XMMatrixTranslation(0, _renderTargetYStart, 0);
	_pSpriteRenderer->Render(_renderTargetPuzzle->ShaderResourceView, mat);

	_pSpriteRenderer->End();

	if (_state == CurrentGameState::WIN)
	{
		for (int i = 0; i < _numColumns; ++i)
		{
			ParticleSystem2D^ particleSystem = _vecParticleSystem2Ds[i];
		
			if (particleSystem->IsActive)
			{
				particleSystem->Render(pContext);
			}
		}
	}
}


void RaceGameState::RenderPuzzlePieces(_In_ float timeTotal)
{
	ID3D11DeviceContext1* pContext = GameGlobals::D3DContext;

	pContext->OMSetDepthStencilState(_depthStencilState.Get(), 1);

	// set the render target to the texture
	pContext->OMSetRenderTargets(1, _renderTargetPuzzle->AddressOfRenderTarget, _depthStencil->DepthStencilView);

	CD3D11_VIEWPORT viewport(0.0f, 0.0f, _game->RenderWindowSize, _game->RenderWindowSize);
	pContext->RSSetViewports(1, &viewport);

	// clear both the render target and depth stencil to default values
    const float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	pContext->ClearRenderTargetView(_renderTargetPuzzle->RenderTargetView, ClearColor);
	pContext->ClearDepthStencilView(_depthStencil->DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	

	// for each piece update the shader constants
	int numPieces = _vecPuzzlePieces.size();

	for (int iPiece = 0; iPiece < numPieces; ++iPiece)
	{
		PuzzlePiece^ piece = _vecPuzzlePieces[iPiece];

		if (!piece->IsCorrect())
		{
			piece->Render(pContext, timeTotal);
		}
	}

	// YUCK!!! doing this for now to speed up development
	// loop through all pieces to render correct ones

	_effectCorrect->InitialClear(pContext);

	_effectCorrect->Begin(pContext);
		
	_effectCorrect->BeginObjectPass();

	bool needCorrect = false;

	for (int iPiece = 0; iPiece < numPieces; ++iPiece)
	{
		PuzzlePiece^ piece = _vecPuzzlePieces[iPiece];

		if (piece->IsCorrect())
		{
			piece->RenderCorrect(pContext, _effectCorrect);

			needCorrect = true;
		}
	}

	if (needCorrect)
	{
		_effectCorrect->Render();
	}

	_effectCorrect->End();

	for (int iPiece = 0; iPiece < numPieces; ++iPiece)
	{
		PuzzlePiece^ piece = _vecPuzzlePieces[iPiece];

		if (piece->IsCorrect())
		{
			piece->Render(pContext, timeTotal);
		}
	}

	// YUCK!!! doing this for now to speed up development
	// loop through all pieces again to render the one that is selected
	// this render will be the selection effect and the geometry

	for (int iPiece = 0; iPiece < numPieces; ++iPiece)
	{
		PuzzlePiece^ piece = _vecPuzzlePieces[iPiece];

		piece->RenderSelected(pContext);
	}
}


void RaceGameState::Release()
{
	ReleaseActiveResources();

	for (int i = 0; i < _numColumns; ++i)
	{
		ParticleSystem2D^ particleSystem = _vecParticleSystem2Ds[i];
		
		particleSystem->Release();
	}

	_vecParticleSystem2Ds.clear();

	_game = nullptr;

	delete _pCamera;
	_pCamera = nullptr;

	delete _pSpriteRenderer;
	_pSpriteRenderer = nullptr;

	_depthStencilState = nullptr;

	for (unsigned int iPiece = 0; iPiece < _vecPuzzlePieces.size(); ++iPiece)
	{
		PuzzlePiece^ piece = _vecPuzzlePieces[iPiece];

		piece->Release();
	}

	_vecPuzzlePieces.clear();
}


void RaceGameState::ReleaseActiveResources()
{
	_renderTargetPuzzle->Release();
	_renderTargetPuzzle = nullptr;

	_depthStencil->Release();
	_depthStencil = nullptr;

	_effectCorrect->Release();
	_effectCorrect = nullptr;
}