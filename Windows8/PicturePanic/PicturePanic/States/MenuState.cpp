#include "pch.h"
#include "MenuState.h"
#include "GameState/GameStateManager.h"
#include "FadeTransitionState.h"
#include "RaceGameState.h"

using namespace GameFramework::Globals;

const float MenuState::UI_TRANSITION_TIME = 0.35f;

MenuState::MenuState() : RenderableGameState(L"MenuState"), _firstTime(true), _state(ActiveMenuState::MAINMENU), _pointCaptured(false),
															_gamestateRace(nullptr), _gamestateFadeTransition(nullptr)
{
	_mainMenu		= ref new MainMenu();
	_modeMenu		= ref new ModeMenu();
	_difficultyMenu = ref new DifficultyMenu();
}


void MenuState::OnEntered()
{
	GameState::OnEntered();

	if (_firstTime)
	{
		_pSpriteRenderer = new SpriteRenderer();
		_pSpriteRenderer->Initialize(GameGlobals::D3DDevice);

		_mainMenu->Initialize();
		_modeMenu->Initialize();
		_difficultyMenu->Initialize();

		_activeMenuScreen = _mainMenu;

		// call OnWindowSizeChange to layout the menu screens correctly
		OnWindowSizeChange(GameGlobals::WindowWidth, GameGlobals::WindowHeight);

		_firstTime = false;
	}
}


void MenuState::OnWindowSizeChange(_In_ float screenWidth, _In_ float screenHeight)
{
	_mainMenu->OnWindowSizeChange((_firstTime) ? false : (_state == ActiveMenuState::MAINMENU));
	_modeMenu->OnWindowSizeChange((_state == ActiveMenuState::MODEMENU));
	_difficultyMenu->OnWindowSizeChange((_state == ActiveMenuState::DIFFICULTYMENU));

	if (_state != ActiveMenuState::MAINMENU)
	{
		if (!(ApplicationView::Value == ApplicationViewState::Snapped))
		{
			_mainMenu->SetPosition(0, screenHeight, 0);
		}
		else
		{
			_mainMenu->SetPosition(-screenWidth, 0, 0);
		}
	}

	if (_state != ActiveMenuState::MODEMENU)
	{
		if (!(ApplicationView::Value == ApplicationViewState::Snapped))
		{
			_modeMenu->SetPosition(0, -screenHeight, 0);
		}
		else
		{
			_modeMenu->SetPosition(screenWidth, 0, 0);
		}
	}
	
	if (_state != ActiveMenuState::DIFFICULTYMENU)
	{
		_difficultyMenu->SetPosition(0, -screenHeight, 0);
	}
}


bool MenuState::OnPointerPressed(_In_ PointerPoint^ point)
{
	// MenuState assumes to be the top most state to accept input
	if (GameStateManager::Instance->ActiveGameState != this)
	{
		return false;
	}

	if (_state == ActiveMenuState::MODEMENU)
	{
		// mode menu allows for swiping modes
		// capture the down event here
		if (!_pointCaptured)
		{
			_capturedPointer = point;

			_pointCaptured = true;
		}
	}

	_activeMenuScreen->SetExclusivePointer(point->PointerId);

	_activeMenuScreen->HitTest(point->PointerId, point->Position.X, point->Position.Y, PointerState::PRESSED);

	return true;
}


bool MenuState::OnPointerMoved(_In_ PointerPoint^ point)
{
	// MenuState assumes to be the top most state to accept input
	if (GameStateManager::Instance->ActiveGameState != this)
	{
		return false;
	}

	_activeMenuScreen->HitTest(point->PointerId, point->Position.X, point->Position.Y, PointerState::MOVE);

	return true;
}


bool MenuState::OnPointerReleased(_In_ PointerPoint^ point) 
{
	// MenuState assumes to be the top most state to accept input
	if (GameStateManager::Instance->ActiveGameState != this)
	{
		return false;
	}

	if (_activeMenuScreen->IsExclusivePointer(point->PointerId))
	{
		int nextState = 0;
		int direction = 0;

		if (_activeMenuScreen->HitTest(point->PointerId, point->Position.X, point->Position.Y, PointerState::RELEASE, &nextState, &direction))
		{
			_state = (ActiveMenuState)nextState;

			_activeMenuScreen->ReleaseExclusivePointer();

			if (_state != ActiveMenuState::TO_GAME)
			{
				SetActiveMenuScreen(direction);
			}
			else
			{
				if (_gamestateFadeTransition == nullptr)
				{
					_gamestateFadeTransition = ref new FadeTransitionState();
				}

				// create a state based on the chosen mode (race mode for now)
				if (_gamestateRace == nullptr)
				{
					_gamestateRace = ref new RaceGameState(this);

					_gamestateRace->Modality = GameStateModality::OVERLAY;

					_gamestateFadeTransition->AddStateForTransition(_gamestateRace);
				}

				DifficultyInfo^ info = _difficultyMenu->SelectedDifficultyInfo;

				_gamestateRace->Columns		    = info->width;
				_gamestateRace->Rows			= info->height;
				_gamestateRace->PlayTime		= info->timeForPuzzle;
				_gamestateRace->PuzzleImagePath = info->modePath;

				GameStateManager::Instance->Push(_gamestateFadeTransition, GameStateModality::OVERLAY, true);
			}
		}
		else
		{
			_activeMenuScreen->ReleaseExclusivePointer();

			if (_state == ActiveMenuState::MODEMENU)
			{
				if (_capturedPointer->PointerId == point->PointerId)
				{
					float xDelta = point->Position.X - _capturedPointer->Position.X;

					int direction = (xDelta > 0) ? 1 : -1;
					//UINT64 timeDelta = pt->Timestamp - _capturedPointer->Timestamp;
						
					//float timeDeltaInSeconds = timeDelta / 1000.0f;
					_capturedPointer.Release();
					_pointCaptured = false;
				}
			}
		}	
	}

	return true;
}


void MenuState::SetActiveMenuScreen(int direction)
{
	bool isSnapView = (ApplicationView::Value == ApplicationViewState::Snapped);

	float xTarget = (isSnapView) ? GameGlobals::WindowWidth : 0;
	float yTarget = (isSnapView) ? 0 : GameGlobals::WindowHeight;

	switch(_state)
	{
	case ActiveMenuState::MAINMENU:
		if (direction == -1)
		{
			// going backward
			_modeMenu->TranslateTo(xTarget, -yTarget, 0, UI_TRANSITION_TIME);

			_mainMenu->SetPosition(-xTarget, yTarget, 0);
			_mainMenu->TranslateTo(0, 0, 0, UI_TRANSITION_TIME);
		}
				
		_activeMenuScreen = _mainMenu;

		break;

	case ActiveMenuState::MODEMENU:
		if (direction == 1)
		{
			// going forward
			_mainMenu->TranslateTo(-xTarget, yTarget, 0, UI_TRANSITION_TIME);
		}
		else
		{
			// going backward
			_difficultyMenu->TranslateTo(xTarget, -yTarget, 0, UI_TRANSITION_TIME);
		}

		_modeMenu->SetPosition(direction * xTarget, direction * -yTarget, 0);
		_modeMenu->TranslateTo(0, 0, 0, UI_TRANSITION_TIME);

		_activeMenuScreen = _modeMenu;
		break;

	case ActiveMenuState::DIFFICULTYMENU:
		if (direction == 1)
		{
			// going forward
			_modeMenu->TranslateTo(-xTarget, yTarget, 0, UI_TRANSITION_TIME);

			_difficultyMenu->SetPosition(direction * xTarget, -yTarget, 0);
			_difficultyMenu->TranslateTo(0, 0, 0, UI_TRANSITION_TIME);
		}
		
		_activeMenuScreen = _difficultyMenu;

		break;

	/*case OPTIONSMENU:
		// if the state is options menu, set the state to "MainMenu" but toggle the settings charm pane
		if (!_showingOptions)
		{

		}

		_showingOptions = !_showingOptions;

		SettingsPane::Show();
		
		_state = MAINMENU;
		
		break;*/

	default:
		_activeMenuScreen = nullptr;
		break;
	}

	if (_activeMenuScreen != nullptr)
	{
		_activeMenuScreen->SetInitialState();
	}
}


void MenuState::ReturnToDifficultyScreen()
{
	_state = ActiveMenuState::DIFFICULTYMENU;
}


bool MenuState::Update(_In_ float timeTotal, _In_ float timeDelta)
{
	_mainMenu->Update(timeTotal, timeDelta);
	_modeMenu->Update(timeTotal, timeDelta);
	_difficultyMenu->Update(timeTotal, timeDelta);

	return true;
}


void MenuState::Render(_In_ float timeTotal, _In_ float timeDelta)
{
	_pSpriteRenderer->Begin(GameGlobals::D3DContext);

	switch(_state)
	{
	case ActiveMenuState::MAINMENU:
		_activeMenuScreen->Render(_pSpriteRenderer);
		_modeMenu->Render(_pSpriteRenderer);
		break;
	case ActiveMenuState::MODEMENU:
		_mainMenu->Render(_pSpriteRenderer);
		_activeMenuScreen->Render(_pSpriteRenderer);
		_difficultyMenu->Render(_pSpriteRenderer);
		break;

	case ActiveMenuState::DIFFICULTYMENU:
		_modeMenu->Render(_pSpriteRenderer);
		_activeMenuScreen->Render(_pSpriteRenderer);
		break;
	}

	_pSpriteRenderer->End();
}


void MenuState::Release()
{
	delete _pSpriteRenderer;
	_pSpriteRenderer = nullptr;

	_activeMenuScreen = nullptr;

	_mainMenu->ReleaseAllActiveResources();
	_modeMenu->ReleaseAllActiveResources();
	_difficultyMenu->ReleaseAllActiveResources();

	_mainMenu = nullptr;
	_modeMenu = nullptr;
	_difficultyMenu = nullptr;
}