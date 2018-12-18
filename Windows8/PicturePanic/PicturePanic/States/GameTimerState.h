#pragma once

#include "GameState/RenderableGameState.h"
#include "SampleOverlay.h"

using namespace GameFramework::GameState;


ref class GameTimerState : RenderableGameState
{
internal:
	GameTimerState();

	virtual bool Update(_In_ float timeTotal, _In_ float timeDelta) override;
	virtual void Render(_In_ float timeTotal, _In_ float timeDelta) override;
	virtual void OnEntered() override;

	property float RemainingTime
	{
		float get()
		{
			return _timer;
		}
	}

	property float TotalTime
	{
		void set(float value)
		{
			_timer = value;
		}
	}

	
private:
	float		   _timer;
	SampleOverlay^ _overlay;
};