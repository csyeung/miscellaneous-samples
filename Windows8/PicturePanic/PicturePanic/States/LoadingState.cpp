#include "pch.h"

#include "LoadingState.h"
#include "Resources/ResourceFactory.h"
#include "GameState/GameStateManager.h"
#include "MenuState.h"
#include "PuzzleFlipTransitionState.h"
#include "Globals/GameGlobals.h"

using namespace concurrency;

using namespace GameFramework::GameState;
using namespace GameFramework::Resources;
using namespace GameFramework::Globals;

const float LoadingState::DEFAULT_LOAD_TIME  = 2.0f;


LoadingState::LoadingState() : RenderableGameState(L"Loading"), _loadingComplete(false), _defaultLoadingTime(DEFAULT_LOAD_TIME)
{
}


void LoadingState::OnEntered()
{
	GameState::OnEntered();

	_spriteRenderer = new SpriteRenderer();
	_spriteRenderer->Initialize(GameGlobals::D3DDevice);

	CreateRenderTarget();

	_textureLogo = ResourceFactory::CreateTexture2DResource("Assets\\System\\SplashScreen.png");

	PrepareResources();
	DoAsynchronousLoading();
}


void LoadingState::OnWindowSizeChange(_In_ float screenWidth, _In_ float screenHeight)
{
	CreateRenderTarget();
}


void LoadingState::CreateRenderTarget()
{
	if (_renderTarget)
	{
		_renderTarget->Release();
		_renderTarget = nullptr;
	}

	_renderTarget = ResourceFactory::CreateRenderTargetResource(static_cast<int>(GameGlobals::WindowWidth), static_cast<int>(GameGlobals::WindowHeight));
}


void LoadingState::PrepareResources()
{
	// MainMenu
	Platform::String^ assetsPathMainMenu = L"Assets\\Languages\\" + Windows::Globalization::Language::CurrentInputMethodLanguageTag + "\\MainMenu\\";
	ResourceFactory::AddPreloadItem(PreLoadType::Texture2D, assetsPathMainMenu + "logo.png");
	ResourceFactory::AddPreloadItem(PreLoadType::Texture2D, assetsPathMainMenu + "snap\\logo.png");
	ResourceFactory::AddPreloadItem(PreLoadType::Texture2D, assetsPathMainMenu + "buttons\\play.png");
	ResourceFactory::AddPreloadItem(PreLoadType::Texture2D, assetsPathMainMenu + "buttons\\options.png");

	// ModeMenu
	Platform::String^ assetsPathModeMenu = L"Assets\\Languages\\" + Windows::Globalization::Language::CurrentInputMethodLanguageTag + "\\ModeMenu\\";
	ResourceFactory::AddPreloadItem(PreLoadType::Texture2D, assetsPathModeMenu + "modesmenu.png");
	ResourceFactory::AddPreloadItem(PreLoadType::Texture2D, assetsPathModeMenu + "buttons\\race_default.png");
	ResourceFactory::AddPreloadItem(PreLoadType::Texture2D, assetsPathModeMenu + "buttons\\race_description.png");
	ResourceFactory::AddPreloadItem(PreLoadType::Texture2D, assetsPathModeMenu + "buttons\\daily_default.png");
	ResourceFactory::AddPreloadItem(PreLoadType::Texture2D, assetsPathModeMenu + "buttons\\daily_disabled.png");
	ResourceFactory::AddPreloadItem(PreLoadType::Texture2D, assetsPathModeMenu + "buttons\\daily_description.png");
	ResourceFactory::AddPreloadItem(PreLoadType::Texture2D, assetsPathModeMenu + "buttons\\headtohead_default.png");
	ResourceFactory::AddPreloadItem(PreLoadType::Texture2D, assetsPathModeMenu + "buttons\\headtohead_disabled.png");
	ResourceFactory::AddPreloadItem(PreLoadType::Texture2D, assetsPathModeMenu + "buttons\\headtohead_description.png");
	ResourceFactory::AddPreloadItem(PreLoadType::Texture2D, assetsPathModeMenu + "buttons\\scoreattack_default.png");
	ResourceFactory::AddPreloadItem(PreLoadType::Texture2D, assetsPathModeMenu + "buttons\\scoreattack_disabled.png");
	ResourceFactory::AddPreloadItem(PreLoadType::Texture2D, assetsPathModeMenu + "buttons\\score_description.png");
	
	// back button
	ResourceFactory::AddPreloadItem(PreLoadType::Texture2D, "Assets\\UI\\Buttons\\back.png");

	// arrows
	ResourceFactory::AddPreloadItem(PreLoadType::Texture2D, "Assets\\UI\\Buttons\\arrow_left.png");
	ResourceFactory::AddPreloadItem(PreLoadType::Texture2D, "Assets\\UI\\Buttons\\arrow_left_disabled.png");
	ResourceFactory::AddPreloadItem(PreLoadType::Texture2D, "Assets\\UI\\Buttons\\arrow_right.png");
	ResourceFactory::AddPreloadItem(PreLoadType::Texture2D, "Assets\\UI\\Buttons\\arrow_right_disabled.png");

	// DifficultyMenu
	Platform::String^ assetsPathDifficultyMenu = "Assets\\Languages\\" + Windows::Globalization::Language::CurrentInputMethodLanguageTag + "\\DifficultyMenu\\";
	ResourceFactory::AddPreloadItem(PreLoadType::Texture2D, assetsPathDifficultyMenu + "difficultymenu.png");
	ResourceFactory::AddPreloadItem(PreLoadType::Texture2D, assetsPathDifficultyMenu + "buttons\\easy.png");
	ResourceFactory::AddPreloadItem(PreLoadType::Texture2D, assetsPathDifficultyMenu + "buttons\\medium.png");
	ResourceFactory::AddPreloadItem(PreLoadType::Texture2D, assetsPathDifficultyMenu + "buttons\\hard.png");

}


void LoadingState::DoAsynchronousLoading()
{
	create_task([this]()
	{
		// Asynchronously load the game resources
        // By doing all this asynchronously, the game gets to its main loop more quickly 
        // and in parallel all the necessary resources are loaded on other threads.
		//
		// The resources are preloaded into the ResourceFactory for early caching.
		return ResourceFactory::Preload();
       
	}).then([this]()
	{
		ResourceFactory::FinalizePreloadedResources();

		//_game->LoadGameResources();

		PuzzlePiece::Initialize(GameGlobals::D3DDevice, GameGlobals::D3DContext, GameGlobals::D3DFeatureLevel);

		_loadingComplete = true;

	}, task_continuation_context::use_current());
}


bool LoadingState::Update(_In_ float timeTotal, _In_ float timeDelta)
{
	_defaultLoadingTime -= timeDelta;

	if  (LoadingComplete)
	{
		// pop the loading state off of the stack
		GameStateManager::Instance->Pop();

		// push the main menu state onto the state stack
		GameStateManager::Instance->Push(PicturePanic::Instance->MainMenu, GameStateModality::OVERLAY, true);

		PuzzleFlipTransitionState^ transitionState = ref new PuzzleFlipTransitionState();
		transitionState->BackTexture = _renderTarget;

		GameStateManager::Instance->Push(transitionState, GameStateModality::OVERLAY, true);

	}

	return true;
}


void LoadingState::Render(_In_ float timeTotal, _In_ float timeDelta)
{
	ID3D11DeviceContext1* pContext = GameGlobals::D3DContext;

	CD3D11_VIEWPORT viewport(0.0f, 0.0f, GameGlobals::WindowWidth, GameGlobals::WindowHeight);
	pContext->RSSetViewports(1, &viewport);


	float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

	ID3D11RenderTargetView* pPreviousRenderTarget;
	ID3D11DepthStencilView* pPreviousDepthStencilView;

	pContext->OMGetRenderTargets(1, &pPreviousRenderTarget, &pPreviousDepthStencilView);

	pContext->OMSetRenderTargets(1, _renderTarget->AddressOfRenderTarget, nullptr);
	pContext->ClearRenderTargetView(_renderTarget->RenderTargetView, ClearColor);
	//pContext->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

	_spriteRenderer->Begin(pContext, SpriteRenderer::FilterMode::Linear);

	float logoX = GameGlobals::WindowWidth / 2.0f - _textureLogo->Width / 2.0f;
	float logoY = GameGlobals::WindowHeight / 2.0f - _textureLogo->Height / 2.0f;

	_spriteRenderer->Render(_textureLogo->ShaderResourceView, XMMatrixTranslation(logoX, logoY, 0));
	_spriteRenderer->End();

	pContext->OMSetRenderTargets(1, &pPreviousRenderTarget, pPreviousDepthStencilView);

	pPreviousRenderTarget->Release();
	pPreviousDepthStencilView->Release();
	
	_spriteRenderer->Begin(pContext, SpriteRenderer::FilterMode::Linear);

	_spriteRenderer->Render(_renderTarget->ShaderResourceView, XMMatrixIdentity());
	_spriteRenderer->End();
}


void LoadingState::Release()
{
	//_game = nullptr;

	/*if (_spriteRenderer)
	{
		_spriteRenderer->Release();

		delete _spriteRenderer;
		_spriteRenderer = nullptr;
	}*/

	/*if (_renderTarget)
	{
		_renderTarget->Release();
		_renderTarget = nullptr;
	}*/
}