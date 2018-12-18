#include "pch.h"
#include "App.xaml.h"
#include "MainPage.xaml.h"

using namespace GameFramework;

using namespace Microsoft::WRL;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Graphics::Display;
using namespace Windows::UI::Core;
using namespace Windows::UI::ViewManagement;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::ApplicationModel::Store;
using namespace concurrency;


MainPage::MainPage()
{
	InitializeComponent();

#ifdef USE_STORE_SIMULATOR
    ResetLicense->Visibility = ::Visibility::Visible;
#endif
}


void MainPage::SetSnapped()
{
    VisualStateManager::GoToState(this->DXMainPage, "SnappedAdState", false);
}


void MainPage::HideSnapped()
{
    VisualStateManager::GoToState(this->DXMainPage, "UnsnappedAdState", false);
}
