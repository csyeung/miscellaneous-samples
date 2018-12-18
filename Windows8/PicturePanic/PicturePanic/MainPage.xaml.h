#pragma once

#include "MainPage.g.h"

using namespace Windows::UI::Xaml::Controls;

namespace GameFramework
{

ref class App;

public ref class MainPage sealed
{
public:
	MainPage();

	inline void SetApp(App^ app) { _app = app; }
	inline SwapChainBackgroundPanel^ GetSwapChainBackgroundPanel() { return DXSwapChainPanel; };

	void SetSnapped();
    void HideSnapped();

protected:

private:
		App^ _app;
};

}
