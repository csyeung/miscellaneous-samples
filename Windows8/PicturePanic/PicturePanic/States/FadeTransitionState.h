#pragma once

#include "GameState/RenderableGameState.h"
#include "Effects/FullScreenQuad.h"

using namespace std;

using namespace GameFramework::Effects;
using namespace GameFramework::GameState;

ref class FadeTransitionState : RenderableGameState
{
internal:
	FadeTransitionState();

	virtual bool Update(float timeTotal, float timeDelta) override;
	virtual void Render(_In_ float timeTotal, _In_ float timeDelta) override;
	virtual void OnEntered() override;

	void AddStateForTransition(GameState^ state) { _vecGameStatesForTransition.push_back(state); }

	property float FadeOutTime
	{
		void set(float value)
		{
			_timeFadeOut = value;
		}
	}

	property float FadeOutDelay
	{
		void set(float value)
		{
			_fadeOutDelay = value;
		}
	}

	property float FadeInTime
	{
		void set(float value)
		{
			_timeFadeIn = value;
		}
	}

	property float FadeInDelay
	{
		void set(float value)
		{
			_fadeInDelay = value;
		}
	}

private:
	enum class FadeState
	{
		FADEOUT,
		FADEIN,
		FADECOMPLETE
	};

	static const float DEFAULT_FADEIN_TIME;
	static const float DEFAULT_FADEOUT_TIME;

	void UpdateStatesForTransition();

	FullScreenQuad^ _quad;

	FadeState _state;
	
	float	  _timeFadeOut;
	float	  _timeFadeIn;
	float	  _currentTime;
	float	  _fadeOutDelay;
	float	  _fadeInDelay;

	float	  _currentAlpha;

	vector<GameState^> _vecGameStatesForTransition;
};