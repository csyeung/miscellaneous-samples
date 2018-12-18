#include "pch.h"

#include "SummaryDialog.h"
#include "PicturePanic.h"
#include "States/RaceSummaryState.h"
#include "Globals/GameGlobals.h"

using namespace Windows::UI::ViewManagement;

using namespace GameFramework::Globals;


SummaryDialog::SummaryDialog() : _buttonFacebook(nullptr), _buttonTwitter(nullptr), _buttonWindowsShare(nullptr),
							     _buttonMainMenu(nullptr), _buttonRetryPuzzle(nullptr), _buttonNextPuzzle(nullptr), 
								 _buttonBackground(nullptr), _buttonPuzzleComplete(nullptr), _buttonTimeUp(nullptr),
								 _exiting(false)
{
}


SummaryDialog::~SummaryDialog()
{
}

void SummaryDialog::OnWindowSizeChange(bool animate)
{
	MenuScreen::OnWindowSizeChange(animate);

	bool isSnap = (ApplicationView::Value == ApplicationViewState::Snapped);

	float animationTime = (animate) ? 1.0f : 0.0f;

	float nativeWindowWidth  = (isSnap) ? 320.0f : GameGlobals::NativeWidth;
	float nativeWindowHeight = GameGlobals::NativeHeight;

	if (!isSnap)
	{
		float nativeWindowScaledWidth = nativeWindowWidth * GameGlobals::ScreenFactorX;
		float nativeWindowScaledHeight = nativeWindowHeight * GameGlobals::ScreenFactorY;

		float renderWindow = PicturePanic::Instance->RenderWindowSize;
		float dialogWidth = (nativeWindowScaledWidth - renderWindow);
		float dialogCenterX = renderWindow + (dialogWidth / 2.0f);
		float dialogHeight = nativeWindowHeight * GameGlobals::ScreenFactorY;

		float puzzleCompleteHeight = float(_buttonPuzzleComplete->Width);
		float puzzleCompleteWidth = float(_buttonPuzzleComplete->Height);

		float timeUpWidth = float(_buttonTimeUp->Width);
		float timeUpHeight = float(_buttonTimeUp->Height);
		
		_posTitleGoal = XMFLOAT2(dialogCenterX, dialogHeight * 0.25f);
		//_buttonTimeUp->AnimateTo(dialogCenterX, dialogHeight * 0.25f, animationTime);

		// place the buttons
		float retryHeight = float(_buttonRetryPuzzle->Height);
		float nextHeight = float(_buttonNextPuzzle->Height);
		float mainMenuHeight = float(_buttonMainMenu->Height);

		float gap = 20.0f * GameGlobals::UIScaleFactor;
		float currentY = dialogHeight / 2.0f;

		// retry
		_posButtonRetryGoal = XMFLOAT2(dialogCenterX, currentY);
		//_buttonRetryPuzzle->AnimateTo(dialogCenterX, currentY, animationTime);

		currentY += retryHeight * GameGlobals::UIScaleFactor / 2.0f + gap + nextHeight / 2.0f * GameGlobals::UIScaleFactor;

		// next
		_posButtonNextPuzzleGoal = XMFLOAT2(dialogCenterX, currentY);
		//_buttonNextPuzzle->AnimateTo(dialogCenterX, currentY, animationTime);
		
		currentY += nextHeight * GameGlobals::UIScaleFactor / 2.0f + gap + mainMenuHeight / 2.0f * GameGlobals::UIScaleFactor;

		// main
		_buttonMainMenu->AnimateTo(dialogCenterX, currentY, animationTime);

		// twitter, facebook, microsoft share all along the bottom right
		float twitterWidth  = float(_buttonTwitter->Width);
		float twitterHeight = float(_buttonTwitter->Height);
		float facebookWidth = float(_buttonFacebook->Width);
		float facebookHeight = float(_buttonFacebook->Height);	
		float windowsShareWidth = float(_buttonWindowsShare->Width);
		float windowsShareHeight = float(_buttonWindowsShare->Height);	
		float mainMenuWidth  = float(_buttonMainMenu->Width);

		float uiScale = GameGlobals::UIScaleFactor;

		
		float gapIcon = 10.0f;
		float gapScaled = gapIcon * uiScale;
		float currentX = nativeWindowScaledWidth - (((twitterWidth / 2.0f) + gap + facebookWidth + gap + windowsShareWidth) * uiScale);
		
		_posButtonTwitterGoal = XMFLOAT2(currentX, nativeWindowScaledHeight - (twitterHeight * uiScale / 2.0f) - gapScaled);

		currentX += (twitterWidth * uiScale / 2.0f) + gapScaled + (facebookWidth * uiScale / 2.0f);
		
		_posButtonFacebookGoal = XMFLOAT2(currentX, nativeWindowScaledHeight - (facebookHeight * uiScale / 2.0f) - gapScaled);
		
		currentX += (facebookWidth * uiScale / 2.0f) + gapScaled + (windowsShareWidth * uiScale / 2.0f);
	
		_posButtonWindowsShareGoal = XMFLOAT2(currentX, nativeWindowScaledHeight - (windowsShareHeight * uiScale / 2.0f) - gapScaled);

		
		_posButtonMainMenuGoal = XMFLOAT2(renderWindow + gapScaled + ((mainMenuWidth * uiScale) / 2.0f), nativeWindowScaledHeight - ((mainMenuHeight * uiScale) / 2.0f));
	}
	else
	{
	}

	StartAnimationSequence(false);
}


void SummaryDialog::CreateButtons()
{
	Platform::String^ assetsLocalizedPath = L"Assets\\Languages\\" + PicturePanic::Instance->Language + L"\\SummaryDialog\\";
	Platform::String^ assetsPath = L"Assets\\UI\\SummaryDialog\\buttons\\";

	// create twitter button
	ButtonInfo twitterInfo;
	ZeroMemory(&twitterInfo, sizeof(ButtonInfo));

	twitterInfo.name					 = L"TwitterButton";
	twitterInfo.defaultButtonImagePath	 = assetsPath + L"twitter.png";
	twitterInfo.highlightButtonImagePath = assetsPath + L"twitter.png";
	twitterInfo.actionGameState			 = -1;

	_buttonTwitter = ref new GameFramework::UI::Button(this, &twitterInfo);
	
	_buttonTwitter->AvailableInSnap = true;
	_buttonTwitter->ModifiesUIScreenState = false;

	AddChild(_buttonTwitter);

	// create facebook button
	ButtonInfo facebookInfo;
	ZeroMemory(&facebookInfo, sizeof(ButtonInfo));

	facebookInfo.name					  = L"FacebookButton";
	facebookInfo.defaultButtonImagePath	  = assetsPath + L"facebook.png";
	facebookInfo.highlightButtonImagePath = assetsPath + L"facebook.png";
	facebookInfo.actionGameState		  = -1;

	_buttonFacebook = ref new GameFramework::UI::Button(this, &facebookInfo);

	_buttonFacebook->AvailableInSnap = true;
	_buttonFacebook->ModifiesUIScreenState = false;

	AddChild(_buttonFacebook);

	// create windows share charm button
	ButtonInfo windowsShareInfo;
	ZeroMemory(&windowsShareInfo, sizeof(ButtonInfo));

	windowsShareInfo.name					  = L"WindowsShareButton";
	windowsShareInfo.defaultButtonImagePath	  = assetsPath + L"windows_share.png";
	windowsShareInfo.highlightButtonImagePath = assetsPath + L"windows_share.png";
	windowsShareInfo.actionGameState		  = -1;

	_buttonWindowsShare = ref new GameFramework::UI::Button(this, &windowsShareInfo);

	_buttonWindowsShare->AvailableInSnap = true;
	_buttonWindowsShare->ModifiesUIScreenState = false;

	AddChild(_buttonWindowsShare);

	// create retry button
	ButtonInfo retryInfo;
	ZeroMemory(&retryInfo, sizeof(ButtonInfo));

	retryInfo.name					   = L"RetryPuzzleButton";
	retryInfo.defaultButtonImagePath   = assetsLocalizedPath + L"buttons\\retry.png";
	retryInfo.actionGameState		   = static_cast<int>(RaceSummaryState::SummaryMenuAction::RETRY);
	retryInfo.uiDirection			   = 1;
	retryInfo.initialScale			   = 0.75f;
	retryInfo.scaleOnTouch			   = 1.0f;
	retryInfo.scaleToTime			   = 0.35f;

	_buttonRetryPuzzle = ref new GameFramework::UI::Button(this, &retryInfo);

	_buttonRetryPuzzle->AvailableInSnap = true;
	_buttonRetryPuzzle->ModifiesUIScreenState = true;

	AddChild(_buttonRetryPuzzle);

	// create next button
	ButtonInfo nextInfo;
	ZeroMemory(&nextInfo, sizeof(ButtonInfo));

	nextInfo.name					  = L"NextPuzzleButton";
	nextInfo.defaultButtonImagePath   = assetsLocalizedPath + L"buttons\\nextgame.png";
	nextInfo.actionGameState		  = static_cast<int>(RaceSummaryState::SummaryMenuAction::NEXTPUZZLE);
	nextInfo.uiDirection			  = 1;
	nextInfo.initialScale			  = 0.75f;
	nextInfo.scaleOnTouch			  = 1.0f;
	nextInfo.scaleToTime			  = 0.35f;

	_buttonNextPuzzle = ref new GameFramework::UI::Button(this, &nextInfo);

	_buttonNextPuzzle->AvailableInSnap = true;
	_buttonNextPuzzle->ModifiesUIScreenState = true;

	AddChild(_buttonNextPuzzle);

	// create main menu button
	ButtonInfo mainInfo;
	ZeroMemory(&mainInfo, sizeof(ButtonInfo));

	mainInfo.name					  = L"MainMenuButton";
	mainInfo.defaultButtonImagePath   = L"Assets\\UI\\Buttons\\back.png";
	mainInfo.actionGameState		  = static_cast<int>(RaceSummaryState::SummaryMenuAction::LEAVEGAME);

	_buttonMainMenu = ref new GameFramework::UI::Button(this, &mainInfo);

	_buttonMainMenu->AvailableInSnap = true;
	_buttonMainMenu->ModifiesUIScreenState = true;

	AddChild(_buttonMainMenu);

	// create puzzle complete message as a button
	// this is set visible before the dialog is called to draw
	ButtonInfo puzzleCompleteInfo;
	ZeroMemory(&puzzleCompleteInfo, sizeof(ButtonInfo));

	puzzleCompleteInfo.name					    = L"PuzzleCompleteButton";
	puzzleCompleteInfo.defaultButtonImagePath	= assetsLocalizedPath + L"puzzlecomplete.png";

	_buttonPuzzleComplete = ref new GameFramework::UI::Button(this, &puzzleCompleteInfo);

	_buttonPuzzleComplete->AvailableInSnap = true;
	_buttonPuzzleComplete->ModifiesUIScreenState = false;
	_buttonPuzzleComplete->ClickEnable(false);

	AddChild(_buttonPuzzleComplete);


	// create time up message as a button
	// this is set visible before the dialog is called to draw
	ButtonInfo timeUpInfo;
	ZeroMemory(&timeUpInfo, sizeof(ButtonInfo));

	timeUpInfo.name					  = L"TimeUpButton";
	timeUpInfo.defaultButtonImagePath = assetsLocalizedPath + L"timeup.png";

	_buttonTimeUp = ref new GameFramework::UI::Button(this, &timeUpInfo);

	_buttonTimeUp->AvailableInSnap = true;
	_buttonTimeUp->ModifiesUIScreenState = false;
	_buttonTimeUp->ClickEnable(false);

	AddChild(_buttonTimeUp);

}

void SummaryDialog::StartAnimationSequence(bool initialize)
{
	float delay = 0.0f;

	if (initialize)
	{
		float startX = _posTitleGoal.x;
		float startYTitle = -256;
		float startYMenu = PicturePanic::Instance->RenderWindowSize + 256;

		_buttonTimeUp->Position = XMFLOAT3(startX, startYTitle, 0);
		_buttonPuzzleComplete->Position = XMFLOAT3(startX, startYTitle, 0);
		_buttonRetryPuzzle->Position = XMFLOAT3(startX, startYMenu, 0);
		_buttonNextPuzzle->Position = XMFLOAT3(startX, startYMenu, 0);

		_buttonTwitter->Position = XMFLOAT3(startX, startYMenu, 0);
		_buttonFacebook->Position = XMFLOAT3(startX, startYMenu, 0);
		_buttonWindowsShare->Position = XMFLOAT3(startX, startYMenu, 0);
		_buttonMainMenu->Position = XMFLOAT3(startX, startYMenu, 0);

		_buttonRetryPuzzle->Reset();
		_buttonNextPuzzle->Reset();
		_buttonTwitter->Reset();
		_buttonFacebook->Reset();
		_buttonWindowsShare->Reset();
		_buttonMainMenu->Reset();

		delay = 0.8f;
	}

	float animationTime = 1.0f;

	_buttonTimeUp->AnimateTo(_posTitleGoal.x, -256, animationTime, 0, false);
	_buttonPuzzleComplete->AnimateTo(_posTitleGoal.x, _posTitleGoal.y, animationTime, 0, false);
	_buttonRetryPuzzle->AnimateToWithDelay(delay, _posButtonRetryGoal.x, _posButtonRetryGoal.y, animationTime, 0, false);
	_buttonNextPuzzle->AnimateToWithDelay(delay, _posButtonNextPuzzleGoal.x, _posButtonNextPuzzleGoal.y, animationTime, 0, false);

	_buttonTwitter->AnimateToWithDelay(delay, _posButtonTwitterGoal.x, _posButtonTwitterGoal.y, animationTime, 0, false);
	_buttonFacebook->AnimateToWithDelay(delay, _posButtonFacebookGoal.x, _posButtonFacebookGoal.y, animationTime, 0, false);
	_buttonWindowsShare->AnimateToWithDelay(delay, _posButtonWindowsShareGoal.x, _posButtonWindowsShareGoal.y, animationTime, 0, false);

	_buttonMainMenu->AnimateToWithDelay(delay, _posButtonMainMenuGoal.x, _posButtonMainMenuGoal.y, animationTime, 0, false);

	_exiting = false;
}


void SummaryDialog::StartExitSequence()
{
	if (!_exiting)
	{
		float animationTime = 1.0f;

		float endX = _posTitleGoal.x;
		float endYTitle = -256;
		float endYMenu = PicturePanic::Instance->RenderWindowSize + 256;

		_buttonTimeUp->AnimateTo(_posTitleGoal.x, endYTitle, animationTime, 0, false);
		_buttonPuzzleComplete->AnimateTo(_posTitleGoal.x, endYTitle, animationTime, 0, false);
		_buttonRetryPuzzle->AnimateTo(endX, endYMenu, animationTime, 0, false);
		_buttonNextPuzzle->AnimateTo(endX, endYMenu, animationTime, 0, false);

		_buttonTwitter->AnimateTo(endX, endYMenu, animationTime, 0, false);
		_buttonFacebook->AnimateTo(endX, endYMenu, animationTime, 0, false);
		_buttonWindowsShare->AnimateTo(endX, endYMenu, animationTime, 0, false);

		_buttonMainMenu->AnimateTo(endX, endYMenu, animationTime, 0, false);

		_exiting = true;
		_timeToExit = animationTime;
	}
}


void SummaryDialog::Update(float timeTotal, float timeDelta)
{
	MenuScreen::Update(timeTotal, timeDelta);

	if (_exiting)
	{
		_timeToExit -= timeDelta;

	}
}

void SummaryDialog::Render(SpriteRenderer* pSpriteRenderer)
{
	// render logo
	ID3D11ShaderResourceView* pSRV = nullptr;

	/*if ((ApplicationView::Value == ApplicationViewState::Snapped))
	{
		pSRV = _logoSnapSRV.Get();
	}
	else
	{
		pSRV = _logoSRV.Get();
	}*/

	//pSpriteRenderer->Render(pSRV, _matLogoTransform);
	
	MenuScreen::Render(pSpriteRenderer);
}

void SummaryDialog::ReleaseAllActiveResources()
{
	MenuScreen::ReleaseAllActiveResources();
}

void SummaryDialog::SetComplete(bool flag)
{
	_buttonPuzzleComplete->SetVisible(flag);
	_buttonTimeUp->SetVisible(!flag);
	
}
