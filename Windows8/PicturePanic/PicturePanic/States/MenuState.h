#pragma once

#include "GameState/RenderableGameState.h"
#include "MainMenu.h"
#include "ModeMenu.h"
#include "DifficultyMenu.h"
#include "Sprites/SpriteRenderer.h"
#include "States/RaceGameState.h"
#include "States/FadeTransitionState.h"

using namespace GameFramework::GameState;
using namespace GameFramework::Sprites;

ref class MenuState : RenderableGameState
{
	
internal:
	enum class ActiveMenuState
	{
		MAINMENU = 0,
		MODEMENU,
		DIFFICULTYMENU,
		OPTIONSMENU,
		TO_GAME
	};

	MenuState();

	virtual void OnEntered() override;
	virtual void Release() override;

	virtual void OnWindowSizeChange(_In_ float screenWidth, _In_ float screenHeight) override;

	virtual bool OnPointerPressed(_In_ PointerPoint^ point) override;
	virtual bool OnPointerMoved(_In_ PointerPoint^ point) override;
	virtual bool OnPointerReleased(_In_ PointerPoint^ point) override;

	virtual bool Update(_In_ float timeTotal, _In_ float timeDelta) override;
	virtual void Render(_In_ float timeTotal, _In_ float timeDelta) override;

	void ReturnToDifficultyScreen();

private:
	

	void SetActiveMenuScreen(int direction);

	static const float UI_TRANSITION_TIME;
	

	MenuScreen^				_activeMenuScreen;

	MainMenu^				_mainMenu;
	ModeMenu^				_modeMenu;
	DifficultyMenu^			_difficultyMenu;

	bool					_firstTime;

	ActiveMenuState			_state;

	Platform::Agile<PointerPoint>	_capturedPointer;
	bool							_pointCaptured;
	SpriteRenderer*				    _pSpriteRenderer;

	// states from menu
	RaceGameState^		 _gamestateRace;
	FadeTransitionState^ _gamestateFadeTransition;
};