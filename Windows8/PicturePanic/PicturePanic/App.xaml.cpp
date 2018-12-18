//
// App.xaml.cpp
// Implementation of the App class
//

#include "pch.h"
#include "App.xaml.h"

using namespace GameFramework;

using namespace concurrency;
using namespace DirectX;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Store;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Display;
using namespace Windows::Storage;
using namespace Windows::UI::Core;
using namespace Windows::UI::Input;
using namespace Windows::UI::ViewManagement;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Media;


App::App() : _hasFocus(true), _visible(true)
{
	InitializeComponent();

#if defined(_DEBUG)
    UnhandledException += ref new UnhandledExceptionEventHandler([](Object^ /* sender */, UnhandledExceptionEventArgs^ args)
    {
        Platform::String^ error = "Game_PicturePanic::App::UnhandledException: " + args->Message + "\n";
        OutputDebugStringW(error->Data());
    });
#endif
}


App::~App()
{
    CompositionTarget::Rendering::remove(_eventRegistrationTokenOnRendering);
}


void App::OnLaunched(LaunchActivatedEventArgs^ /* args */)
{
    Suspending += ref new SuspendingEventHandler(this, &App::OnSuspending);
    Resuming += ref new EventHandler<Object^>(this, &App::OnResuming);

    _mainPage = ref new MainPage();
    _mainPage->SetApp(this);

    Window::Current->Content = _mainPage;
    Window::Current->Activated += ref new WindowActivatedEventHandler(this, &App::OnWindowActivationChanged);
    Window::Current->Activate();

    auto window = Window::Current->CoreWindow;

	// set pointer visualization settings
    PointerVisualizationSettings^ visualizationSettings = PointerVisualizationSettings::GetForCurrentView();
    visualizationSettings->IsContactFeedbackEnabled = false;
    visualizationSettings->IsBarrelButtonFeedbackEnabled = false;

    window->SizeChanged += ref new TypedEventHandler<CoreWindow^, WindowSizeChangedEventArgs^>(this, &App::OnWindowSizeChanged);

    window->VisibilityChanged += ref new TypedEventHandler<CoreWindow^, VisibilityChangedEventArgs^>(this, &App::OnVisibilityChanged);

    DisplayProperties::LogicalDpiChanged += ref new DisplayPropertiesEventHandler(this, &App::OnLogicalDpiChanged);


	_renderer = PicturePanic::Instance;
	_renderer->Initialize(window, _mainPage->GetSwapChainBackgroundPanel(), DisplayProperties::LogicalDpi, 1920, 1080);
	
    _eventRegistrationTokenOnRendering = CompositionTarget::Rendering::add(ref new EventHandler<Object^>(this, &App::OnRendering));
   
	UpdateViewState();
}


void App::OnRendering(_In_ Object^ /* sender */, _In_ Object^ /* args */)
{
	_timer.Update();

	float elapsed = _timer.ElapsedSecondsF();
	float delta = _timer.DeltaSecondsF();

#ifdef _DEBUG
	delta = min(0.5f, delta);
#endif

	_renderer->Update(elapsed, delta);

    _renderer->Render(elapsed, delta);

   
    if (!_visible || !_hasFocus)
    {
        // App is in an inactive state so disable the OnRendering callback
        // This optimizes for power and allows the framework to become more queiecent
        if (_eventRegistrationTokenOnRendering.Value != 0)
        {
            CompositionTarget::Rendering::remove(_eventRegistrationTokenOnRendering);

            _eventRegistrationTokenOnRendering.Value = 0;
        }
    }

	_renderer->Present();
}


void App::OnSuspending(_In_ Platform::Object^ /* sender */, _In_ SuspendingEventArgs^ args)
{
    // Save application state.
    // If your application needs time to complete a lengthy operation, it can request a deferral.
    // The SuspendingOperation has a deadline time. Make sure all your operations are complete by that time!
    // If the app doesn't return from this handler within five seconds, it will be terminated.
    SuspendingOperation^ op = args->SuspendingOperation;
    SuspendingDeferral^ deferral = op->GetDeferral();

	_renderer->OnSuspending();

    deferral->Complete();
}


void App::OnWindowActivationChanged(_In_ Platform::Object^ /* sender */, _In_ Windows::UI::Core::WindowActivatedEventArgs^ args)
{
    if (args->WindowActivationState == CoreWindowActivationState::Deactivated)
    {
        //_hasFocus = false;
    }
    else if (args->WindowActivationState == CoreWindowActivationState::CodeActivated || args->WindowActivationState == CoreWindowActivationState::PointerActivated)
    {
        _hasFocus = true;

        // App is now "active" so set up the event handler to do game processing and rendering
        if (_eventRegistrationTokenOnRendering.Value == 0)
		{
			_eventRegistrationTokenOnRendering = CompositionTarget::Rendering::add(ref new EventHandler<Object^>(this, &App::OnRendering));
		}
    }
}


void App::OnWindowSizeChanged(_In_ CoreWindow^ window, _In_ WindowSizeChangedEventArgs^ args)
{
	UpdateViewState();

	_renderer->UpdateForWindowSizeChange(args->Size.Width, args->Size.Height);

    //_mainPage->OnWindowSizeChanged();

    //m_renderNeeded = true;

    if (_eventRegistrationTokenOnRendering.Value == 0)
    {
        // Add the OnRendering Callback to make sure that an update happens.
        _eventRegistrationTokenOnRendering = CompositionTarget::Rendering::add(ref new EventHandler<Object^>(this, &App::OnRendering));
    }
}


void App::OnLogicalDpiChanged(_In_ Object^ /* sender */)
{
    _renderer->SetDpi(DisplayProperties::LogicalDpi);
}


void App::OnVisibilityChanged(_In_ CoreWindow^ sender, _In_ VisibilityChangedEventArgs^ args)
{
    _visible = args->Visible;

    if (_visible && (_eventRegistrationTokenOnRendering.Value == 0))
    {
        // App is now visible so set up the event handler to do game processing and rendering
        _eventRegistrationTokenOnRendering = CompositionTarget::Rendering::add(ref new EventHandler<Object^>(this, &App::OnRendering));
    }
}


void App::OnResuming(_In_ Platform::Object^ /* sender */, _In_ Platform::Object^ /* args */)
{
	_renderer->OnResuming();

    if (_hasFocus)
    {
        //m_updateState = m_updateStateNext;
    }
    else
    {
        //m_updateState = UpdateEngineState::Deactivated;
    }

    //m_renderNeeded = true;
}


void App::UpdateViewState()
{
	if (ApplicationView::Value == ApplicationViewState::Snapped)
    {
        SetSnapped();
    }
    else if (ApplicationView::Value == ApplicationViewState::Filled ||
			 ApplicationView::Value == ApplicationViewState::FullScreenLandscape ||
			 ApplicationView::Value == ApplicationViewState::FullScreenPortrait)
    {
		HideSnapped();

        // App is now "active" so set up the event handler to do game processing and rendering
        if (_hasFocus && _eventRegistrationTokenOnRendering.Value == 0)
        {
            _eventRegistrationTokenOnRendering = CompositionTarget::Rendering::add(ref new EventHandler<Object^>(this, &App::OnRendering));
        }
    }
}


void App::SetSnapped()
{
    _mainPage->SetSnapped();
}


void App::HideSnapped()
{
    _mainPage->HideSnapped();
}