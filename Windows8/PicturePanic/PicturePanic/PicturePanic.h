#pragma once

#include "pch.h"
#include "DirectXBase.h"
#include "DirectXCollision.h"

using namespace concurrency;
using namespace Microsoft::WRL;
using namespace Windows::UI::Core;
using namespace Windows::UI::Input;
using namespace DirectX;
using namespace Windows::UI::Xaml::Controls;

using namespace GameFramework;

using std::vector;


ref class MenuState;

ref class PicturePanic : public DirectXBase
{
internal:
	static property PicturePanic^ Instance
	{
		PicturePanic^ get()
		{
			if (_instance == nullptr)
			{
				_instance = ref new PicturePanic();
			}
	
			return _instance;
		}
	}

	virtual void Initialize(CoreWindow^ window, SwapChainBackgroundPanel^ swapChainPanel, float dpi, float nativeWidth, float nativeHeight) override;
	virtual void CreateDeviceIndependentResources() override;
    virtual void CreateDeviceResources() override;
    virtual void CreateWindowSizeDependentResources() override;
	virtual void UpdateForWindowSizeChange(float screenWidth, float screenHeight) override;
    virtual void Render(float timeTotal, float timeDelta) override;
    void Update(float timeTotal, float timeDelta);

	virtual void OnSuspending() override;
	virtual void OnResuming() override;


	// events
	void OnKeyDown(_In_ CoreWindow^ sender, _In_ KeyEventArgs^ args);
	
	void OnPointerPressed(_In_ CoreWindow^ sender, _In_ PointerEventArgs ^args);
	void OnPointerMoved(_In_ CoreWindow^ sender, _In_ PointerEventArgs ^args);
	void OnPointerReleased(_In_ CoreWindow^ sender, _In_ PointerEventArgs ^args);
	
	void OnDataRequested(_In_ DataTransferManager^ sender, DataRequestedEventArgs^ args);


	virtual property float RenderWindowSize
	{
		float get()
		{
			return _renderWindowSize;
		}
	}

	virtual property bool Initialized
	{
		bool get()
		{
			return _initialized;
		}
	}

	virtual property Platform::String^ Language
	{
		Platform::String^ get()
		{
			return _strCurrentLangauge; 
		}
	}

	virtual property MenuState^ MainMenu
	{
		MenuState^ get()
		{
			return _menuState;
		}
	}

private:
	PicturePanic();

	void LoadGameStates();
	void CreatePuzzleGameResources();

	static PicturePanic^ _instance;

	float				_renderWindowSize;

	ComPtr<ID3D11DepthStencilView>		_depthStencilView;
	ComPtr<ID3D11DepthStencilState>		_depthStencilState;
	
	Platform::String^		_strCurrentLangauge;


	
	Platform::Agile<PointerPoint>	_capturedPointer;
	bool							_pointCaptured;  // yuck

	bool							_initialized;

	MenuState^ _menuState;
};

