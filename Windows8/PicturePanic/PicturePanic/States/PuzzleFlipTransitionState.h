#pragma once

#include "GameState/RenderableGameState.h"
#include "Camera/PerspectiveCamera.h"
#include "PuzzlePiece.h"
#include "Resources/Texture2DResource.h"
#include "Resources/RenderTargetResource.h"
#include "Resources/DepthStencilResource.h"
#include "MenuState.h"

using namespace std;

using namespace GameFramework::GameState;
using namespace GameFramework::Camera;
using namespace GameFramework::Resources;


ref class PuzzleFlipTransitionState : RenderableGameState
{
internal:
	PuzzleFlipTransitionState();

	virtual bool Update(float timeTotal, float timeDelta) override;
	virtual void Render(_In_ float timeTotal, _In_ float timeDelta) override;
	virtual void OnEntered() override;
	virtual void OnWindowSizeChange(_In_ float screenWidth, _In_ float screenHeight) override;

	property float Delay
	{
		void set(float value)
		{
			_delayTime = value;
		}
	}

	property Texture2DResource^ BackTexture
	{
		void set(Texture2DResource^ value)
		{
			_textureBack = value;
		}
	}

private:
	static const float CAMERA_ASPECTRATIO;
	static const float CAMERA_FOV;
	static const float CAMERA_NEARCLIP;
	static const float CAMERA_FARCLIP;
	static const float CAMERA_POS_Z;

	void CreateBuffers();
	void CreatePieces();
	void CreateRenderTargets();
	void SetupReveal();

	void SetupLeftRevealWipe();
	void SetupDiagonalRevealWipe();
	void SetupRandomRevealWipe();
	void SetupTopRevealWipe();
	void SetupReverseDiagonalRevealWipe();

	ComPtr<ID3D11DepthStencilState>		_depthStencilState;

	int				  _numRows;
	int			 	  _numColumns;

	PerspectiveCamera*		_pCamera;
	vector<PuzzlePiece^>	_vecPuzzlePieces;

	float _delayTime;

	RenderTargetResource^ _renderTarget;
	DepthStencilResource^ _depthStencil;

	Texture2DResource^ _textureBack;
	Texture2DResource^ _textureSide;
};