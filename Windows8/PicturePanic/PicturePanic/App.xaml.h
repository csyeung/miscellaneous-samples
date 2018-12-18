#pragma once

#if defined(_DEBUG)
#define DISABLE_XAML_GENERATED_BREAK_ON_UNHANDLED_EXCEPTION 1
#endif

//#define USE_STORE_SIMULATOR 1

#include "MainPage.xaml.h"
#include "PicturePanic.h"
#include "Timers/Timer.h"

#include "App.g.h"

using namespace GameFramework::Timers;

namespace GameFramework
{

/// <summary>
/// An empty page that can be used on its own or navigated to within a Frame.
/// </summary>
public ref class App sealed
{
public:
	App();

	virtual void OnLaunched(Windows::ApplicationModel::Activation::LaunchActivatedEventArgs^ pArgs) override;

private:	
	~App();

    void OnSuspending(_In_ Platform::Object^ sender, _In_ Windows::ApplicationModel::SuspendingEventArgs^ args);
	void OnResuming(_In_ Platform::Object^ sender, _In_ Platform::Object^ args);
	void OnWindowActivationChanged(_In_ Platform::Object^ sender, _In_ Windows::UI::Core::WindowActivatedEventArgs^ args);
    void OnWindowSizeChanged(_In_ Windows::UI::Core::CoreWindow^ sender, _In_ Windows::UI::Core::WindowSizeChangedEventArgs^ args);
    void OnLogicalDpiChanged(_In_ Platform::Object^ sender);
    void OnVisibilityChanged(_In_ Windows::UI::Core::CoreWindow^ sender, _In_ Windows::UI::Core::VisibilityChangedEventArgs^ args);
    void OnRendering(_In_ Object^ sender, _In_ Object^ args);

    void UpdateViewState();
	void SetSnapped();
    void HideSnapped();

	bool                                        _hasFocus;
    bool                                        _visible;

	PicturePanic^								_renderer;
	MainPage^									_mainPage;
	Windows::Foundation::EventRegistrationToken _eventRegistrationTokenOnRendering;


	Timer						_timer;
};


}