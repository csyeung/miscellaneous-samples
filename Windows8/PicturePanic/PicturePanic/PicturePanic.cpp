#include "pch.h"

#include "PicturePanic.h"
#include "Utility/BasicShapes.h"
#include "Utility/BasicLoader.h"
#include "UI/PointerState.h"
#include <time.h>
#include "Utility/Easing.h"
#include "Resources/ResourceFactory.h"
#include "Resources/Texture2DResource.h"
#include "GameState/GameState.h"
#include "GameState/GameStateManager.h"
#include "States/BackgroundImageState.h"
#include "States/LoadingState.h"
#include "States/FadeTransitionState.h"
#include "States/MenuState.h"
#include "Utility/Utility.h"
#include "Globals/GameGlobals.h"

using namespace Microsoft::WRL;
using namespace Windows::Foundation;
using namespace Windows::UI::Core;
using namespace Windows::System;
using namespace Windows::UI::ApplicationSettings;
using namespace Windows::Globalization;
using namespace Windows::UI::ViewManagement;

using namespace GameFramework::Utility;
using namespace GameFramework::UI;
using namespace GameFramework::Resources;
using namespace GameFramework::GameState;
using namespace GameFramework::Globals;

PicturePanic^ PicturePanic::_instance = nullptr;

PicturePanic::PicturePanic() : _initialized(false), _menuState(nullptr)
{
	_strCurrentLangauge = Windows::Globalization::Language::CurrentInputMethodLanguageTag;
}


void PicturePanic::LoadGameStates()
{
	// create global main menu state
	_menuState = ref new MenuState();

	// create background and loading states
	BackgroundImageState^ backgroundImageState = ref new BackgroundImageState();
	GameStateManager::Instance->Push(backgroundImageState);

	//FadeTransitionState^ fadeState = ref new FadeTransitionState();
	//GameStateManager::Instance()->Push(fadeState);

	LoadingState^ loadingState = ref new LoadingState();
	GameStateManager::Instance->Push(loadingState, GameStateModality::OVERLAY, true);
}


void PicturePanic::Initialize(CoreWindow^ window, SwapChainBackgroundPanel^ swapChainPanel, float dpi, float nativeWidth, float nativeHeight)
{
	DirectXBase::Initialize(window, swapChainPanel, dpi, nativeWidth, nativeHeight);

	window->KeyDown += ref new TypedEventHandler<CoreWindow^, KeyEventArgs^>(this, &PicturePanic::OnKeyDown);

	window->PointerPressed += ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &PicturePanic::OnPointerPressed);
	window->PointerMoved += ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &PicturePanic::OnPointerMoved);
	window->PointerReleased += ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &PicturePanic::OnPointerReleased);

	_dataTransferManager->DataRequested += ref new TypedEventHandler<DataTransferManager^, DataRequestedEventArgs^>(this, &PicturePanic::OnDataRequested);

	LoadGameStates();

	_initialized = true;
}


void PicturePanic::CreateDeviceIndependentResources()
{
    DirectXBase::CreateDeviceIndependentResources();
}


void PicturePanic::CreateDeviceResources()
{
	DirectXBase::CreateDeviceResources();
}


void PicturePanic::CreatePuzzleGameResources()
{
	bool needTextures = true;

	
	if (needTextures)
	{
		// Create a descriptor for the depth/stencil buffer.
		D3D11_TEXTURE2D_DESC depthBufferDesc;
		ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

		depthBufferDesc.Width = UINT(_renderWindowSize);
		depthBufferDesc.Height = UINT(_renderWindowSize);
		depthBufferDesc.MipLevels = 1;
		depthBufferDesc.ArraySize = 1;
		depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthBufferDesc.SampleDesc.Count = 1;
		depthBufferDesc.SampleDesc.Quality = 0;
		depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthBufferDesc.CPUAccessFlags = 0;
		depthBufferDesc.MiscFlags = 0;

		// Allocate a 2-D surface as the depth/stencil buffer.
		ComPtr<ID3D11Texture2D> depthBuffer;
		DXCall(m_d3dDevice->CreateTexture2D(&depthBufferDesc, nullptr, &depthBuffer));

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

		DXCall(m_d3dDevice->CreateDepthStencilState(&depthStencilDesc, &_depthStencilState));

		// Create a DepthStencil view on this surface to use on bind.
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
		ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

		depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;

		DXCall(m_d3dDevice->CreateDepthStencilView(depthBuffer.Get(), &depthStencilViewDesc, &_depthStencilView));
	}
}


void PicturePanic::UpdateForWindowSizeChange(float screenWidth, float screenHeight)
{
	DirectXBase::UpdateForWindowSizeChange(screenWidth, screenHeight);
	
	XMVECTOR vScreenScale = XMVectorSet(1, 1, 1, 1);
	XMVECTOR vScaled = XMVector2Transform(vScreenScale, _matRootTransform);

	float xScreenScale = XMVectorGetX(vScaled);
	//float yScreenScale = XMVectorGetY(vScaled);

	if (ApplicationView::Value == ApplicationViewState::Snapped)
	{
		_renderWindowSize = m_window->Bounds.Width;
	}
	else
	{
		_renderWindowSize = GameGlobals::NativeHeight * GameGlobals::ScreenFactorX;

		// check for ads overlap
		float adHeight = 90;
		float pixelBoarder = 0;
		float cameraCompensation = -10;

		if (_renderWindowSize >= screenHeight - (adHeight + pixelBoarder + cameraCompensation))
		{
			_renderWindowSize = screenHeight - (adHeight + pixelBoarder + cameraCompensation);
		}
	}
		
	CreatePuzzleGameResources();

	GameStateManager::Instance->UpdateForWindowSizeChange(screenWidth, screenHeight);
}


void PicturePanic::CreateWindowSizeDependentResources()
{
	// this will resize the swapchain and reset the device, rendertargets, etc
    DirectXBase::CreateWindowSizeDependentResources();
}

void PicturePanic::OnKeyDown(_In_ CoreWindow^ sender, _In_ KeyEventArgs^ args)
{
	
	switch (args->VirtualKey)
	{
		case VirtualKey::Space:
#if defined(_DEBUG)
		// If the debug layer isn't present on the system then this QI will fail.  In that case
		// the routine just exits.   This is only a debugging aid to see the active D3D objects
		// so it isn't critical to functional operation.

		ComPtr<ID3D11Debug> debugLayer;
		HRESULT hr = m_d3dDevice.As(&debugLayer);
		
		if (FAILED(hr))
		{
			return;
		}
		
		DXCall(debugLayer->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL));
#endif

			break;

	}
}



void PicturePanic::OnPointerPressed(_In_ CoreWindow^ sender, _In_ PointerEventArgs^ args)
{
	PointerPoint ^pt = args->CurrentPoint;

	GameStateManager::Instance->OnPointerPressed(pt);
}

void PicturePanic::OnPointerMoved(_In_ CoreWindow^ sender, _In_ PointerEventArgs^ args)
{
	PointerPoint ^pt = args->CurrentPoint;

	GameStateManager::Instance->OnPointerMoved(pt);
}

void PicturePanic::OnPointerReleased(_In_ CoreWindow^ sender, _In_ PointerEventArgs^ args)
{
	PointerPoint ^pt = args->CurrentPoint;

	GameStateManager::Instance->OnPointerReleased(pt);
}


void PicturePanic::OnDataRequested(_In_ DataTransferManager^ sender, DataRequestedEventArgs^ args)
{
#if 0
	// fill in the default information
	DataPackagePropertySet^ properties = args->Request->Data->Properties;

	properties->Title = "Puzzle Panic";

	Platform::String^ description;
	Platform::String^ text;

	if (_state == PicturePanicGameState::LOSEPUZZLE)
	{
		description = ref new Platform::String(L"Share Puzzle Panic Game");

		std::wstring str = L"Got Windows 8?  You need to download Puzzle Panic! - http://www.bing.com";
		
		text = ref new Platform::String(str.c_str());
	}
	else
	{
		description = ref new Platform::String(L"Share my puzzle win");		

		std::wstring str = L"I just finished Puzzle Panic Level 1 in 5 moves!  Do you think you can beat me? - http://www.bing.com";
		
		text = ref new Platform::String(str.c_str());
	}

	properties->Description = description;

	args->Request->Data->SetText(text);
#endif
}



void PicturePanic::Update(float timeTotal, float timeDelta)
{
	GameStateManager::Instance->Update(timeTotal, timeDelta);
}


void PicturePanic::OnSuspending()
{
	DirectXBase::OnSuspending();
}


void PicturePanic::OnResuming()
{
	DirectXBase::OnResuming();
}


void PicturePanic::Render(float timeTotal, float timeDelta)
{
	CD3D11_VIEWPORT viewport(0.0f, 0.0f, m_windowBounds.Width, m_windowBounds.Height);
	const float ClearColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

	m_d3dContext->RSSetViewports(1, &viewport);

	m_d3dContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());
	m_d3dContext->ClearRenderTargetView(m_renderTargetView.Get(), ClearColor);
	m_d3dContext->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

	GameStateManager::Instance->Render(m_d3dContext.Get(), timeTotal, timeDelta);
}