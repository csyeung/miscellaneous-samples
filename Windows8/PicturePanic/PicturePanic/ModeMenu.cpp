#include "pch.h"
#include "ModeMenu.h"
#include "UI/PointerState.h"
#include "PicturePanic.h"
#include "States/MenuState.h"
#include "Globals/GameGlobals.h"

using namespace GameFramework::UI;
using namespace GameFramework::Globals;

const float ModeMenu::CONTROL_ANIMATION_TIME = 1.0f;
const bool ModeMenu::ENABLE_DAILY			 = false;
const bool ModeMenu::ENABLE_HEADTOHEAD		 = false;
const bool ModeMenu::ENABLE_SCOREATTACK		 = false;
const float ModeMenu::MOVE_TIME				 = 0.3f;
const int ModeMenu::MAX_MODES				 = 4;
	
ModeMenu::ModeMenu() : _buttonLeftArrow(nullptr), _buttonRightArrow(nullptr), _buttonBack(nullptr), 
					   _buttonRace(nullptr), _buttonDailyChallenge(nullptr), _buttonHeadToHead(nullptr), 
					   _buttonScoreAttack(nullptr), _currentModeIndex(0), _noClickArrowDelay(-1.0f)
{
}


ModeMenu::~ModeMenu()
{
}


void ModeMenu::Initialize()
{
	Platform::String^ assetsPath = "Assets\\Languages\\" + PicturePanic::Instance->Language + L"\\ModeMenu\\";

	_imageTitle = ref new UIImage(this, assetsPath + L"modesmenu.png", nullptr);
	_imageTitle->AvailableInSnap = false;

	AddChild(_imageTitle);

	MenuScreen::Initialize();
}


void ModeMenu::OnWindowSizeChange(bool animate)
{
	MenuScreen::OnWindowSizeChange(animate);

	bool isSnap = (ApplicationView::Value == ApplicationViewState::Snapped);

	float nativeWindowWidth  = (isSnap) ? 320.0f : GameGlobals::NativeWidth;
	float nativeWindowHeight = GameGlobals::NativeHeight;

	float timeToAnimate = (animate) ? CONTROL_ANIMATION_TIME : 0.0f;

	float windowCenterX = nativeWindowWidth / 2.0f;
	float windowCenterScaledX = nativeWindowWidth * GameGlobals::ScreenFactorX / 2.0f;
	float windowCenterY = nativeWindowHeight / 2.0f;
	float windowCenterScaledY = nativeWindowHeight * GameGlobals::ScreenFactorY / 2.0f;
	float iconHalfWidth = _buttonRace->Width / 2.0f;
	float iconHalfHeight = _buttonRace->Height / 2.0f;
	float arrowIconWidth = _buttonLeftArrow->Width;
	float arrowIconHeight = _buttonLeftArrow->Height;

	float uiScale = GameGlobals::UIScaleFactor;
	
	float factor = (isSnap) ? 0.5f : 1.0f * uiScale;

	if (!isSnap)
	{
		_imageTitle->AnimateTo(windowCenterScaledX, _imageTitle->Height * uiScale, timeToAnimate, 0, false);

		_buttonLeftArrow->AnimateTo(windowCenterScaledX - (iconHalfWidth + arrowIconWidth) * uiScale, windowCenterScaledY, timeToAnimate, 0, false);
		_buttonRightArrow->AnimateTo(windowCenterScaledX + (iconHalfWidth + arrowIconWidth) * uiScale, windowCenterScaledY, timeToAnimate, 0, false);

		_buttonBack->AnimateTo(86, nativeWindowHeight - 78, timeToAnimate);

		float currentX = windowCenterScaledX - (_currentModeIndex * windowCenterScaledX);

		_buttonRace->AnimateTo(currentX, windowCenterScaledY, timeToAnimate, 0, false);
		
		currentX += windowCenterScaledX;

		_buttonDailyChallenge->AnimateTo(currentX, windowCenterScaledY, timeToAnimate, 0, false);

		currentX += windowCenterScaledX;

		_buttonHeadToHead->AnimateTo(currentX, windowCenterScaledY, timeToAnimate, 0, false);

		currentX += windowCenterScaledX;

		_buttonScoreAttack->AnimateTo(currentX, windowCenterScaledY, timeToAnimate, 0, false);
	}
	else
	{
		// small vertical placement
		float emptySpace = (nativeWindowHeight * GameGlobals::ScreenFactorY) - (iconHalfHeight + (2 * iconHalfHeight) + iconHalfHeight);

		float gap = emptySpace / 2;
		//float currentY = gap;

		float centerScreen = windowCenterY - iconHalfHeight * factor;

		// arrows at the top and bottom
		_buttonLeftArrow->AnimateTo(windowCenterScaledX, windowCenterScaledY - (iconHalfHeight * factor) - (45.0f * uiScale), timeToAnimate, 90, false);
		_buttonRightArrow->AnimateTo(windowCenterScaledX, windowCenterScaledY + (iconHalfHeight * factor) + (45.0f * uiScale), timeToAnimate, 90, false);

		float currentY = windowCenterY - (_currentModeIndex * nativeWindowHeight) / 2;

		_buttonRace->AnimateTo(windowCenterX, currentY, timeToAnimate);

		currentY += windowCenterY;

		_buttonDailyChallenge->AnimateTo(windowCenterX, currentY, timeToAnimate);

		currentY += windowCenterY;

		_buttonHeadToHead->AnimateTo(windowCenterX, currentY, timeToAnimate);

		currentY += windowCenterY;

		_buttonScoreAttack->AnimateTo(windowCenterX, currentY, timeToAnimate);

		_buttonBack->AnimateTo(43, nativeWindowHeight - 39, timeToAnimate);
	}	
}


void ModeMenu::CreateButtons()
{
	CreateArrowButtons();

	Platform::String^ assetsPath = "Assets\\Languages\\" + PicturePanic::Instance->Language + L"\\ModeMenu\\buttons\\";
	
	// create race button
	ButtonInfo raceInfo;
	ZeroMemory(&raceInfo, sizeof(ButtonInfo));

	raceInfo.name					  = "RaceMode";
	raceInfo.defaultButtonImagePath   = assetsPath + "race_default.png";
	raceInfo.highlightButtonImagePath = nullptr; //assetsPath + L"race_default.png";
	raceInfo.disabledButtonImagePath  = "";
	raceInfo.actionGameState		  = static_cast<int>(MenuState::ActiveMenuState::DIFFICULTYMENU);
	raceInfo.uiDirection			  = 1;
	raceInfo.initialScale			  = 0.9f;
	raceInfo.scaleOnTouch			  = 1;
	raceInfo.scaleToTime			  = 0.35f;
	
	_buttonRace = ref new GameFramework::UI::Button(this, &raceInfo);

	_buttonRace->ModifiesUIScreenState = true;
	_buttonRace->AvailableInSnap = true;

	AddChild(_buttonRace);

	UIImage^ raceDescription = ref new UIImage(_buttonRace, assetsPath + "race_description.png", nullptr);
	raceDescription->Offset = XMFLOAT2(0, 340.0f);
	raceDescription->AvailableInSnap = false;

	_buttonRace->AddChild(raceDescription);

	// create daily challenge button
	ButtonInfo dailyInfo;
	ZeroMemory(&dailyInfo, sizeof(ButtonInfo));

	dailyInfo.name					   = "DailyMode";
	dailyInfo.defaultButtonImagePath   = assetsPath + "daily_default.png";
	dailyInfo.highlightButtonImagePath = nullptr;//assetsPath + L"daily_default.png";
	dailyInfo.disabledButtonImagePath  = assetsPath + "daily_disabled.png";
	dailyInfo.actionGameState		   = static_cast<int>(MenuState::ActiveMenuState::DIFFICULTYMENU);
	dailyInfo.uiDirection			   = 1;
	dailyInfo.initialScale			   = 0.9f;
	dailyInfo.scaleOnTouch			   = 1;
	dailyInfo.scaleToTime			   = 0.35f;
	
	_buttonDailyChallenge = ref new GameFramework::UI::Button(this, &dailyInfo);

	_buttonDailyChallenge->ModifiesUIScreenState = true;
	_buttonDailyChallenge->AvailableInSnap = true;
	_buttonDailyChallenge->ClickEnable(false);
	_buttonDailyChallenge->Enable(ENABLE_DAILY);

	AddChild(_buttonDailyChallenge);

	UIImage^ dailyDescription = ref new UIImage(_buttonDailyChallenge, assetsPath + "daily_description.png", nullptr);
	dailyDescription->Offset = XMFLOAT2(0, 340.0f);
	dailyDescription->AvailableInSnap = false;

	_buttonDailyChallenge->AddChild(dailyDescription);

	
	// create challenge button
	ButtonInfo headToHeadInfo;
	ZeroMemory(&headToHeadInfo, sizeof(ButtonInfo));

	headToHeadInfo.name					    = "Head to Head";
	headToHeadInfo.defaultButtonImagePath   = assetsPath + "headtohead_default.png";
	headToHeadInfo.highlightButtonImagePath = nullptr;//assetsPath + L"headtohead_default.png";
	headToHeadInfo.disabledButtonImagePath  = assetsPath + "headtohead_disabled.png";
	headToHeadInfo.actionGameState		    = static_cast<int>(MenuState::ActiveMenuState::DIFFICULTYMENU);
	headToHeadInfo.uiDirection			    = 1;
	headToHeadInfo.initialScale			    = 0.9f;
	headToHeadInfo.scaleOnTouch			    = 1;
	headToHeadInfo.scaleToTime			    = 0.35f;

	_buttonHeadToHead = ref new GameFramework::UI::Button(this, &headToHeadInfo);

	_buttonHeadToHead->ModifiesUIScreenState = true;
	_buttonHeadToHead->AvailableInSnap = true;
	_buttonHeadToHead->ClickEnable(false);
	_buttonHeadToHead->Enable(ENABLE_HEADTOHEAD);

	AddChild(_buttonHeadToHead);

	UIImage^ headtoheadDescription = ref new UIImage(_buttonHeadToHead, assetsPath + "headtohead_description.png", nullptr);
	headtoheadDescription->Offset = XMFLOAT2(0, 340.0f);
	headtoheadDescription->AvailableInSnap = false;

	_buttonHeadToHead->AddChild(headtoheadDescription);

	// create single button
	ButtonInfo scoreAttackInfo;
	ZeroMemory(&scoreAttackInfo, sizeof(ButtonInfo));

	scoreAttackInfo.name					 = "Score Attack";
	scoreAttackInfo.defaultButtonImagePath   = assetsPath + "scoreattack_default.png";
	scoreAttackInfo.highlightButtonImagePath = nullptr;//assetsPath + L"scoreattack_default.png";
	scoreAttackInfo.disabledButtonImagePath  = assetsPath + "scoreattack_disabled.png";
	scoreAttackInfo.actionGameState			 = static_cast<int>(MenuState::ActiveMenuState::DIFFICULTYMENU);
	scoreAttackInfo.uiDirection				 = 1;
	scoreAttackInfo.initialScale			 = 0.9f;
	scoreAttackInfo.scaleOnTouch			 = 1;
	scoreAttackInfo.scaleToTime			     = 0.35f;

	_buttonScoreAttack = ref new GameFramework::UI::Button(this, &scoreAttackInfo);

	_buttonScoreAttack->ModifiesUIScreenState = true;
	_buttonScoreAttack->AvailableInSnap = true;
	_buttonScoreAttack->ClickEnable(false);
	_buttonScoreAttack->Enable(ENABLE_SCOREATTACK);

	AddChild(_buttonScoreAttack);

	UIImage^ scoreDescription = ref new UIImage(_buttonScoreAttack, assetsPath + "score_description.png", nullptr);
	scoreDescription->Offset = XMFLOAT2(0, 340.0f);
	scoreDescription->AvailableInSnap = false;

	_buttonScoreAttack->AddChild(scoreDescription);

	// create back button
	ButtonInfo backInfo;
	ZeroMemory(&backInfo, sizeof(ButtonInfo));

	backInfo.name					  = "BackButton";
	backInfo.defaultButtonImagePath	  = "Assets\\UI\\Buttons\\back.png";
	backInfo.highlightButtonImagePath = nullptr;//"Assets/UI/Buttons/back_highlight.png";
	backInfo.actionGameState		  = static_cast<int>(MenuState::ActiveMenuState::MAINMENU);
	backInfo.uiDirection			  = -1;
	backInfo.initialScale			  = 0.9f;
	backInfo.scaleOnTouch			  = 1;
	backInfo.scaleToTime			  = 0.35f;

	_buttonBack = ref new GameFramework::UI::Button(this, &backInfo);

	_buttonBack->ModifiesUIScreenState = true;
	_buttonBack->AvailableInSnap = true;

	AddChild(_buttonBack);
}

void ModeMenu::CreateArrowButtons()
{
	// create left arrow button
	ButtonInfo leftInfo;
	ZeroMemory(&leftInfo, sizeof(ButtonInfo));

	leftInfo.name					  = "LeftArrow";
	leftInfo.defaultButtonImagePath   = "Assets\\UI\\Buttons\\arrow_left.png";
	leftInfo.highlightButtonImagePath = nullptr;//"Assets/UI/Buttons/arrow_left_highlight.png";
	leftInfo.disabledButtonImagePath  = "Assets\\UI\\Buttons\\arrow_left_disabled.png";
	leftInfo.uiDirection			  = -1;
	
	_buttonLeftArrow = ref new GameFramework::UI::Button(this, &leftInfo);

	// left button is disabled initially
	_buttonLeftArrow->Enable(false);

	_buttonLeftArrow->OnButtonClicked += ref new ButtonClicked(this, &ModeMenu::OnLeftArrow);	

	AddChild(_buttonLeftArrow);

	// create right arrow button
	ButtonInfo rightInfo;
	ZeroMemory(&rightInfo, sizeof(ButtonInfo));

	rightInfo.name					   = "RightArrow";
	rightInfo.defaultButtonImagePath   = "Assets\\UI\\Buttons\\arrow_right.png";
	rightInfo.highlightButtonImagePath = nullptr;//"Assets/UI/Buttons/arrow_right_highlight.png";
	rightInfo.disabledButtonImagePath  = "Assets\\UI\\Buttons\\arrow_right_disabled.png";
	rightInfo.uiDirection			   = 1;
	
	_buttonRightArrow = ref new GameFramework::UI::Button(this, &rightInfo);

	_buttonRightArrow->OnButtonClicked += ref new ButtonClicked(this, &ModeMenu::OnRightArrow);	

	AddChild(_buttonRightArrow);
}

void ModeMenu::OnRightArrow(Platform::Object^ data)
{
	_buttonRightArrow->ClickEnable(false);

	MoveModes(1);
}

void ModeMenu::OnLeftArrow(Platform::Object^ data)
{
	_buttonLeftArrow->ClickEnable(false);

	MoveModes(-1);
}


void ModeMenu::MoveModes(int direction)
{
	_currentModeIndex += direction;

	if (_currentModeIndex >= 0 && _currentModeIndex < MAX_MODES )
	{
		_buttonLeftArrow->Enable(true);
		_buttonRightArrow->Enable(true);

		_noClickArrowDelay = MOVE_TIME + 0.1f;

		float nativeWindowWidth = 1920.0f;
		float nativeWindowHeight = 1080.0f;

		float currentX = nativeWindowWidth * GameGlobals::ScreenFactorX / 2.0f - (_currentModeIndex * (nativeWindowWidth * GameGlobals::ScreenFactorX / 2.0f));
		float currentY = nativeWindowHeight * GameGlobals::ScreenFactorY / 2.0f - (_currentModeIndex * (nativeWindowHeight * GameGlobals::ScreenFactorY / 2.0f));

		// remember the current button positions (for interpolation)
		// stop at size - 1 to miss back button
		vector<GameFramework::UI::Button^> vecButtons;

		vecButtons.push_back(_buttonRace);
		vecButtons.push_back(_buttonDailyChallenge);
		vecButtons.push_back(_buttonHeadToHead);
		vecButtons.push_back(_buttonScoreAttack);

		for (unsigned int i = 0; i < vecButtons.size(); ++i)
		{
			GameFramework::UI::Button^ button = vecButtons[i];

			XMFLOAT3 pos = button->Position;

			if ((ApplicationView::Value == ApplicationViewState::Snapped))
			{
				button->AnimateTo(pos.x, currentY, MOVE_TIME, 0, false);

				currentY += (nativeWindowHeight * GameGlobals::ScreenFactorY / 2.0f);
			}
			else
			{
				button->AnimateTo(currentX, pos.y, MOVE_TIME, 0, false);

				currentX += (nativeWindowWidth * GameGlobals::ScreenFactorX / 2.0f);
			}

			button->ClickEnable((i == (_currentModeIndex)));
		}

		if (_currentModeIndex == 0)
		{
			_buttonLeftArrow->Enable(false);
		}
		else if (_currentModeIndex == MAX_MODES -1)
		{
			_buttonRightArrow->Enable(false);
		}
	}
	else
	{
		_currentModeIndex = max(0, _currentModeIndex);
		_currentModeIndex = min(_currentModeIndex, MAX_MODES - 1);
	}
}

void ModeMenu::Update(float timeTotal, float timeDelta)
{
	if (_noClickArrowDelay != 1.0f)
	{
		_noClickArrowDelay -= timeDelta;

		if (_noClickArrowDelay <= 0)
		{
			_buttonLeftArrow->ClickEnable(true);
			_buttonRightArrow->ClickEnable(true);

			_noClickArrowDelay = -1.0f;
		}
	}
	// update the custom arrow buttons
	//_buttonLeftArrow->Update(timeTotal, timeDelta);
	//_buttonRightArrow->Update(timeTotal, timeDelta);

	MenuScreen::Update(timeTotal, timeDelta);
}


void ModeMenu::Render(SpriteRenderer* pSpriteRenderer)
{
	MenuScreen::Render(pSpriteRenderer);

	// render the custom arrow buttons
	_buttonLeftArrow->Render(pSpriteRenderer);
	_buttonRightArrow->Render(pSpriteRenderer);
}


void ModeMenu::ReleaseAllActiveResources()
{
	MenuScreen::ReleaseAllActiveResources();

	if (_buttonLeftArrow != nullptr)
	{
		_buttonLeftArrow->ReleaseAllActiveResources();

		_buttonLeftArrow = nullptr;
	}

	if (_buttonRightArrow != nullptr)
	{
		_buttonRightArrow->ReleaseAllActiveResources();

		_buttonRightArrow = nullptr;
	}
}