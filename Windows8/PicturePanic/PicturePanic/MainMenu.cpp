#include "pch.h"

#include "MainMenu.h"
#include "PicturePanic.h"
#include "Utility/BasicLoader.h"
#include "Utility/Easing.h"
#include "Resources/ResourceFactory.h"
#include "Resources/Texture2DResource.h"
#include "States/MenuState.h"
#include "Globals/GameGlobals.h"

using namespace GameFramework::Utility;
using namespace GameFramework::Resources;
using namespace GameFramework::Globals;

const float MainMenu::CONTROL_ANIMATION_TIME = 1.0f;

MainMenu::MainMenu() : _buttonPlay(nullptr), _buttonOptions(nullptr), _firstTime(true)
{
}

MainMenu::~MainMenu()
{
}

void MainMenu::Initialize()
{
	Platform::String^ assetsPath = "Assets\\Languages\\" + PicturePanic::Instance->Language + "\\MainMenu\\";

	_imageLogo = ref new UIImage(this, assetsPath + "logo.png", assetsPath + "snap\\logo.png");

	_imageLogo->Rotation = XMConvertToRadians(20.0f);

	AddChild(_imageLogo);

	MenuScreen::Initialize();
}


void MainMenu::OnWindowSizeChange(bool animate)
{
	MenuScreen::OnWindowSizeChange(animate);

	bool isSnap = (ApplicationView::Value == ApplicationViewState::Snapped);

	float nativeWindowWidth  = (isSnap) ? 320.0f : GameGlobals::NativeWidth;
	float nativeWindowHeight = GameGlobals::NativeHeight;

	float nativeWindowHalfWidth = nativeWindowWidth / 2.0f;
	float nativeWindowQuarterWidth = nativeWindowHalfWidth / 2.0f;
	float nativeWindowHalfHeight = nativeWindowHeight / 2.0f;

	float playHalfWidth = _buttonPlay->Width / 2.0f;
	float playHalfHeight = _buttonPlay->Height / 2.0f;
	float optionsHalfWidth = _buttonOptions->Width / 2.0f;
	float optionsHalfHeight = _buttonOptions->Height / 2.0f;
	float animationTime = (animate) ? CONTROL_ANIMATION_TIME : 0.0f;
	float logoWidth = (isSnap) ? _imageLogo->SnapWidth : _imageLogo->Width;
	float logoHeight = (isSnap) ? _imageLogo->SnapHeight : _imageLogo->Height;

	if (!isSnap)
	{
		_buttonPlay->AnimateTo(nativeWindowQuarterWidth, nativeWindowHalfHeight, animationTime);
		_buttonOptions->AnimateTo(nativeWindowQuarterWidth, (nativeWindowHeight * 0.75f), animationTime);
	}
	else
	{
		_buttonPlay->AnimateTo(nativeWindowHalfWidth, nativeWindowHalfHeight, animationTime);
		
		// no options button in snap
		_buttonOptions->AnimateTo(0,  (nativeWindowHeight * 0.75f), animationTime);
	}
	
	float targetX;
	float targetY;

	if (isSnap)
	{
		targetX = (nativeWindowWidth / 2.0f) * GameGlobals::ScreenFactorX;
		targetY = logoHeight / 2.0f;
	}
	else
	{
		targetX = (nativeWindowWidth - (logoWidth / 2.0f));
		targetY = nativeWindowHeight / 2.0f;
	}

	_imageLogo->AnimateTo(targetX, targetY, animationTime, 0.0f, true);
}


void MainMenu::CreateButtons()
{
	Platform::String^ assetsPath = "Assets\\Languages\\" + PicturePanic::Instance->Language + "\\MainMenu\\buttons\\";

	// create play button
	ButtonInfo playInfo;
	ZeroMemory(&playInfo, sizeof(ButtonInfo));

	playInfo.name					  = L"PlayButton";
	playInfo.defaultButtonImagePath	  = assetsPath + "play.png";
	playInfo.highlightButtonImagePath = nullptr;  //assetsPath + "play_highlight.png";
	playInfo.actionGameState		  = static_cast<int>(MenuState::ActiveMenuState::MODEMENU);
	playInfo.uiDirection			  = 1;
	playInfo.initialScale			  = 0.75f;
	playInfo.scaleOnTouch			  = 1.5;
	playInfo.scaleToTime			  = 0.35f;

	_buttonPlay = ref new GameFramework::UI::Button(this, &playInfo);

	_buttonPlay->ModifiesUIScreenState = true;
	_buttonPlay->AvailableInSnap = true;

	AddChild(_buttonPlay);
	
	// create options button
	ButtonInfo optionsInfo;
	ZeroMemory(&optionsInfo, sizeof(ButtonInfo));
	
	optionsInfo.name					 = L"OptionsButton";
	optionsInfo.defaultButtonImagePath   = assetsPath + "options.png";
	optionsInfo.highlightButtonImagePath = nullptr;//assetsPath + L"options_highlight.png";
	optionsInfo.actionGameState			 = static_cast<int>(MenuState::ActiveMenuState::OPTIONSMENU);
	optionsInfo.uiDirection				 = 0;
	optionsInfo.initialScale			 = 0.75f;
	optionsInfo.scaleOnTouch			 = 1;
	optionsInfo.scaleToTime				 = 0.35f;

	_buttonOptions = ref new GameFramework::UI::Button(this, &optionsInfo);

	_buttonOptions->ModifiesUIScreenState = false;
	_buttonOptions->AvailableInSnap = false;

	AddChild(_buttonOptions);
}


void MainMenu::ReleaseAllActiveResources()
{
	_imageLogo->ReleaseAllActiveResources();
	_imageLogo = nullptr;

	MenuScreen::ReleaseAllActiveResources();
}



