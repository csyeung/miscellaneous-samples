#include "pch.h"
#include "FadeTransitionState.h"
#include "GameState/GameStateManager.h"
#include "Globals/GameGlobals.h"

using namespace GameFramework::Globals;

const float FadeTransitionState::DEFAULT_FADEIN_TIME = 1.0f;
const float FadeTransitionState::DEFAULT_FADEOUT_TIME = 1.0f;


FadeTransitionState::FadeTransitionState() : RenderableGameState(L"FadeTransition"), _fadeOutDelay(0.0f), _fadeInDelay(0.0f), _timeFadeOut(DEFAULT_FADEOUT_TIME), _timeFadeIn(DEFAULT_FADEIN_TIME), _currentTime(0.0f)
{
}


void FadeTransitionState::OnEntered()
{
	GameState::OnEntered();

	_state = FadeState::FADEOUT;

	_currentTime = 0.0f;

	_currentAlpha = 0.0f;

	_quad = ref new FullScreenQuad();

	_quad->Colored = true;

	_quad->Initialize(GameGlobals::D3DDevice);
}


bool FadeTransitionState::Update(float timeTotal, float timeDelta)
{
	switch (_state)
	{
	case FadeState::FADEOUT:
		if (_fadeOutDelay > 0.0f)
		{
			_fadeOutDelay -= timeDelta;

			return true;
		}

		_currentTime += timeDelta;

		_currentAlpha = _currentTime / _timeFadeOut;

		if (_currentAlpha >= 1.0f)
		{
			UpdateStatesForTransition();

			_currentAlpha = 1.0f;

			_state = FadeState::FADEIN;

			_currentTime = _timeFadeIn;
		}

		break;

	case FadeState::FADEIN:
		if (_fadeInDelay > 0.0f)
		{
			_fadeInDelay -= timeDelta;

			return true;
		}

		_currentTime -= timeDelta;

		_currentAlpha = _currentTime / _timeFadeIn;

		if (_currentAlpha <= 0.0f)
		{
			_currentAlpha = 0.0f;

			_state = FadeState::FADECOMPLETE;
		}

		break;

	case FadeState::FADECOMPLETE:
		GameStateManager::Instance->Pop();

		break;
	}

	return true;
}


void FadeTransitionState::UpdateStatesForTransition()
{
	// get the current state off of the game state manager (this state)
	GameState^ currentState = GameStateManager::Instance->Pop();

	bool transitionCompleted = false;

	RenderableGameState^ transitionFromState = dynamic_cast<RenderableGameState^>(GameStateManager::Instance->ActiveGameState);

	for (unsigned int i = 0; i < _vecGameStatesForTransition.size(); ++i)
	{
		GameState^ state = _vecGameStatesForTransition[i];

		state->Enabled = true;

		GameStateManager::Instance->Push(state, state->Modality, true);

		transitionCompleted = true;
	}

	if (!transitionCompleted)
	{
		// This type of fade transition will pop the next state on the stack
		GameState^ stateToRemove = GameStateManager::Instance->Pop();
		//stateToRemove->Release();

		RenderableGameState^ nextState = dynamic_cast<RenderableGameState^>(GameStateManager::Instance->ActiveGameState);

		nextState->Visible = true;
	}
	else
	{
		// we have transitioned, let's mark the transition from state as not active
		transitionFromState->Enabled = false;
	}

	// now add the fade transition state back to the top with an overlay modality
	GameStateManager::Instance->Push(currentState, GameStateModality::OVERLAY, false);
}


void FadeTransitionState::Render(_In_ float timeTotal, _In_ float timeDelta)
{
	_quad->ColorRender(GameGlobals::D3DContext, XMFLOAT4(0.0f, 0.0f, 0.0f, _currentAlpha));	
}


