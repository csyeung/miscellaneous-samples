#include "pch.h"
#include "GameTimerState.h"
#include "GameState/GameStateManager.h"

using namespace GameFramework::Globals;


GameTimerState::GameTimerState() : RenderableGameState(L"GameTimer"), _overlay(nullptr)
{
}


void GameTimerState::OnEntered()
{
	GameState::OnEntered();	

	if (_overlay == nullptr)
	{
		_overlay = ref new SampleOverlay();

		_overlay->Initialize("");

		_overlay->UpdateForWindowSizeChange(GameGlobals::WindowWidth, GameGlobals::WindowHeight);

		_overlay->ShowTimer(true);
	}

	_overlay->ShowWarning(false);
}


bool GameTimerState::Update(_In_ float timeTotal, _In_ float timeDelta)
{
	_timer -= timeDelta;

	int secondsLeft = int(_timer);

	int hours   = secondsLeft / 3600;
	secondsLeft = secondsLeft % 3600;
	int minutes = secondsLeft / 60;
	secondsLeft = secondsLeft % 60;
	int seconds = secondsLeft;

	int milliseconds = (int)((_timer - (minutes * 60) - seconds) * 100.0f);

	wchar_t tmp[100];
   
	swprintf_s(tmp, 100, L"%01d:%02d:%02d", minutes, seconds, milliseconds);

	Platform::String^ timeText = ref new Platform::String(tmp);
		
	_overlay->SetTime(timeText);

	if (_timer <= 10.0f)
	{
		if (_overlay->NeedsWarning())
		{
			_overlay->ShowWarning(true);
		}

		if (_timer <= 5.0f)
		{
			_overlay->ShowFranticWarning();
		}
	}

	_overlay->Update(timeDelta);

	return true;
}


void GameTimerState::Render(_In_ float timeTotal, _In_ float timeDelta)
{
	_overlay->Render(GameGlobals::WindowWidth, GameGlobals::WindowHeight);
}


