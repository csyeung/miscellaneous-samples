#include "pch.h"

#include "DifficultyMenu.h"
#include "PicturePanic.h"
#include "States/MenuState.h"
#include "Globals/GameGlobals.h"

using namespace GameFramework::Globals;

const int DifficultyMenu::EASY_ROWS		 = 3;
const int DifficultyMenu::EASY_COLUMNS	 = 3;
const float DifficultyMenu::EASY_TIME	 = 1 * 60;
const int DifficultyMenu::MEDIUM_ROWS	 = 4;
const int DifficultyMenu::MEDIUM_COLUMNS = 4;
const float DifficultyMenu::MEDIUM_TIME	 = 1.5f * 60;
const int DifficultyMenu::HARD_ROWS		 = 5;
const int DifficultyMenu::HARD_COLUMNS	 = 5;
const float DifficultyMenu::HARD_TIME	 = 3 * 60;

const float DifficultyMenu::CONTROL_ANIMATION_TIME = 1.0f;

DifficultyMenu::DifficultyMenu() : _buttonEasyDifficulty(nullptr), _buttonMediumDifficulty(nullptr), _buttonHardDifficulty(nullptr), _buttonBack(nullptr)
{
}


DifficultyMenu::~DifficultyMenu()
{
}


void DifficultyMenu::Initialize()
{
	Platform::String^ assetsPath = "Assets\\Languages\\" + PicturePanic::Instance->Language + "\\DifficultyMenu\\";

	_imageTitle = ref new UIImage(this, assetsPath + "difficultymenu.png", nullptr);
	_imageTitle->AvailableInSnap = false;

	AddChild(_imageTitle);

	MenuScreen::Initialize();
}


void DifficultyMenu::OnWindowSizeChange(bool animate)
{
	MenuScreen::OnWindowSizeChange(animate);

	bool isSnap = (ApplicationView::Value == ApplicationViewState::Snapped);
	float timeToAnimate = (animate) ? CONTROL_ANIMATION_TIME : 0.0f;

	float nativeWindowWidth  = (isSnap) ? 320.0f : GameGlobals::NativeWidth;
	float nativeWindowHeight = GameGlobals::NativeHeight;
	float windowCenterScaledX = nativeWindowWidth * GameGlobals::ScreenFactorX / 2.0f;
	float nativeWindowWidthHalf = nativeWindowWidth / 2.0f;

	float easyWidth = float(_buttonEasyDifficulty->Width);
	float easyHeight = float(_buttonEasyDifficulty->Height);
	float mediumWidth = float(_buttonMediumDifficulty->Width);
	float mediumHeight = float(_buttonMediumDifficulty->Height);
	float hardWidth = float(_buttonHardDifficulty->Width);
	float hardHeight = float(_buttonHardDifficulty->Height);

	if (!isSnap)
	{
		_imageTitle->AnimateTo(windowCenterScaledX, _imageTitle->Height * GameGlobals::UIScaleFactor, timeToAnimate, 0, false);

			// figure out horizontal placement
		float emptySpace = nativeWindowWidth - (easyWidth + mediumWidth + hardWidth);

		float gap = emptySpace / 4;
		float currentX = gap + easyWidth / 2.0f;

		_buttonEasyDifficulty->AnimateTo(currentX, nativeWindowHeight / 2.0f, timeToAnimate);

		currentX += mediumWidth + gap;

		_buttonMediumDifficulty->AnimateTo(currentX, nativeWindowHeight / 2.0f, timeToAnimate);
	
		currentX += hardWidth + gap;

		_buttonHardDifficulty->AnimateTo(currentX, nativeWindowHeight / 2.0f, timeToAnimate);

		_buttonBack->AnimateTo(100, GameGlobals::WindowHeight - 200, timeToAnimate);

		_buttonBack->AnimateTo(86, nativeWindowHeight - 78, timeToAnimate);
	}
	else
	{
		float factor = 0.5f;

		// small vertical placement
		float emptySpace = nativeWindowHeight - (easyHeight + mediumHeight + hardHeight) * factor;

		float gap = emptySpace / 4;
		float currentY = gap + (easyHeight / 2.0f) * factor;

		_buttonEasyDifficulty->AnimateTo(nativeWindowWidthHalf, currentY, timeToAnimate);

		currentY += (easyHeight / 2.0f) * factor + gap + (mediumHeight / 2.0f) * factor;

		_buttonMediumDifficulty->AnimateTo(nativeWindowWidthHalf, currentY, timeToAnimate);
	
		currentY += (mediumHeight / 2.0f) * factor + gap + (hardHeight / 2.0f) * factor;

		_buttonHardDifficulty->AnimateTo(nativeWindowWidthHalf, currentY, timeToAnimate);

		_buttonBack->AnimateTo(43, nativeWindowHeight - 39, timeToAnimate);
	}
}


void DifficultyMenu::CreateButtons()
{
	Platform::String^ assetsPath = "Assets\\Languages\\" + PicturePanic::Instance->Language + L"\\DifficultyMenu\\buttons\\";

	// create easy difficulty button
	DifficultyInfo^ easyGame = ref new DifficultyInfo();

	easyGame->modePath		= "\\easy\\";
	easyGame->width			= EASY_ROWS;
	easyGame->height		= EASY_COLUMNS;
	easyGame->timeForPuzzle = EASY_TIME;

	ButtonInfo difficultyEasyInfo;
	ZeroMemory(&difficultyEasyInfo, sizeof(ButtonInfo));

	difficultyEasyInfo.name						= "EasyDifficulty";
	difficultyEasyInfo.defaultButtonImagePath	= assetsPath + "easy.png";
	difficultyEasyInfo.highlightButtonImagePath = nullptr;//assetsPath + "easy.png";
	difficultyEasyInfo.actionGameState			= static_cast<int>(MenuState::ActiveMenuState::TO_GAME);
	difficultyEasyInfo.uiDirection				= 1;
	difficultyEasyInfo.initialScale				= 0.9f;
	difficultyEasyInfo.scaleOnTouch				= 1;
	difficultyEasyInfo.scaleToTime				= 0.35f;
	difficultyEasyInfo.data						= easyGame;
	
	_buttonEasyDifficulty = ref new GameFramework::UI::Button(this, &difficultyEasyInfo);

	_buttonEasyDifficulty->AvailableInSnap = true;
	_buttonEasyDifficulty->ModifiesUIScreenState = true;

	_buttonEasyDifficulty->OnButtonClicked += ref new ButtonClicked(this, &DifficultyMenu::DifficultySelected);

	AddChild(_buttonEasyDifficulty);

	// create medium difficulty button
	DifficultyInfo^ mediumGame = ref new DifficultyInfo();
	
	mediumGame->modePath	  = "\\medium\\";
	mediumGame->width		  = MEDIUM_ROWS;
	mediumGame->height		  = MEDIUM_COLUMNS;
	mediumGame->timeForPuzzle = MEDIUM_TIME;

	ButtonInfo difficultyMediumInfo;
	ZeroMemory(&difficultyMediumInfo, sizeof(ButtonInfo));

	difficultyMediumInfo.name					  = "MediumDifficulty";
	difficultyMediumInfo.defaultButtonImagePath   = assetsPath + "medium.png";
	difficultyMediumInfo.highlightButtonImagePath = nullptr;//assetsPath + "medium.png";
	difficultyMediumInfo.actionGameState		  = static_cast<int>(MenuState::ActiveMenuState::TO_GAME);
	difficultyMediumInfo.uiDirection			  = 1;
	difficultyMediumInfo.initialScale			  = 0.9f;
	difficultyMediumInfo.scaleOnTouch			  = 1;
	difficultyMediumInfo.scaleToTime			  = 0.35f;	
	difficultyMediumInfo.data					  = mediumGame;	
	
	_buttonMediumDifficulty = ref new GameFramework::UI::Button(this, &difficultyMediumInfo);

	_buttonMediumDifficulty->AvailableInSnap = true;
	_buttonMediumDifficulty->ModifiesUIScreenState = true;

	_buttonMediumDifficulty->OnButtonClicked += ref new ButtonClicked(this, &DifficultyMenu::DifficultySelected);
	
	AddChild(_buttonMediumDifficulty);

	// create hard difficulty button
	DifficultyInfo^ hardGame = ref new DifficultyInfo();

	hardGame->modePath	    = "\\hard\\";
	hardGame->width		    = HARD_ROWS;
	hardGame->height		= HARD_COLUMNS;
	hardGame->timeForPuzzle = HARD_TIME;

	ButtonInfo difficultyHardInfo;
	ZeroMemory(&difficultyHardInfo, sizeof(ButtonInfo));

	difficultyHardInfo.name						= "HardDifficulty";
	difficultyHardInfo.defaultButtonImagePath	= assetsPath + "hard.png";
	difficultyHardInfo.highlightButtonImagePath = nullptr;//assetsPath + "hard.png";
	difficultyHardInfo.actionGameState			= static_cast<int>(MenuState::ActiveMenuState::TO_GAME);
	difficultyHardInfo.uiDirection				= 1;
	difficultyHardInfo.initialScale				= 0.9f;
	difficultyHardInfo.scaleOnTouch				= 1;
	difficultyHardInfo.scaleToTime				= 0.35f;	
	difficultyHardInfo.data					    = hardGame;	

	_buttonHardDifficulty = ref new GameFramework::UI::Button(this, &difficultyHardInfo);

	_buttonHardDifficulty->AvailableInSnap = true;
	_buttonHardDifficulty->ModifiesUIScreenState = true;

	_buttonHardDifficulty->OnButtonClicked += ref new ButtonClicked(this, &DifficultyMenu::DifficultySelected);
	
	AddChild(_buttonHardDifficulty);

	// create back button
	ButtonInfo backInfo;
	ZeroMemory(&backInfo, sizeof(ButtonInfo));

	backInfo.name					  = "BackButton";
	backInfo.defaultButtonImagePath	  = "Assets\\UI\\Buttons\\back.png";
	backInfo.highlightButtonImagePath = nullptr;//"Assets\\UI\\Buttons\\back_highlight.png";
	backInfo.actionGameState		  = static_cast<int>(MenuState::ActiveMenuState::MODEMENU);
	backInfo.uiDirection			  = -1;
	
	_buttonBack = ref new GameFramework::UI::Button(this, &backInfo);

	_buttonBack->AvailableInSnap = true;
	_buttonBack->ModifiesUIScreenState = true;

	AddChild(_buttonBack);
}


void DifficultyMenu::DifficultySelected(Platform::Object^ data)
{
	_selectedDataInfo = (DifficultyInfo^) data;

	//PicturePanic::Instance()->SetupRaceMode(info->width, info->height, info->modePath, info->timeForPuzzle);

	//PicturePanic::Instance()->InitializeFade();
}

		


