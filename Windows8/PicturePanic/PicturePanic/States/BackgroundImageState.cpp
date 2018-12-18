#include "pch.h"
#include "BackgroundImageState.h"
#include "Globals/GameGlobals.h"

using namespace GameFramework::Globals;


void BackgroundImageState::OnEntered()
{
	GameState::OnEntered();

	if (nullptr == _background)
	{
		_pSpriteRenderer = new SpriteRenderer();
		_pSpriteRenderer->Initialize(GameGlobals::D3DDevice);

		_background = ref new Background("Assets\\UI\\Backgrounds\\bg.jpg", "Assets\\UI\\Backgrounds\\Snap\\bg_snap.jpg");
	
		_background->Initialize();
	}
}


void BackgroundImageState::Release()
{
	delete _pSpriteRenderer;
	_pSpriteRenderer = nullptr;

	_background->ReleaseAllActiveResources();
}


void BackgroundImageState::Render(_In_ float timeTotal, _In_ float timeDelta)
{
	ID3D11DeviceContext1* pContext = GameGlobals::D3DContext;

	_pSpriteRenderer->Begin(pContext, SpriteRenderer::FilterMode::Linear);

	_background->Render(_pSpriteRenderer, _matrix);

	_pSpriteRenderer->End();
}