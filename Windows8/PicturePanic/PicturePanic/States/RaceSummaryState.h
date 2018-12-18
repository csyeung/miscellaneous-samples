#pragma once

#include "GameState/RenderableGameState.h"
#include "SummaryDialog.h"
#include "Sprites/SpriteRenderer.h"

using namespace GameFramework::GameState;
using namespace GameFramework::Sprites;

ref class RaceGameState;


ref class RaceSummaryState : RenderableGameState
{
internal:
	enum class SummaryMenuAction
	{
		RETRY = 0,
		NEXTPUZZLE,
		LEAVEGAME
	};

	RaceSummaryState(_In_ RaceGameState^ gamestateRace);

	virtual bool Update(_In_ float timeTotal, _In_ float timeDelta) override;
	virtual void Render(_In_ float timeTotal, _In_ float timeDelta) override;
	virtual void OnEntered() override;
	virtual void Release() override;

	virtual void OnWindowSizeChange(_In_ float screenWidth, _In_ float screenHeight) override;

	virtual bool OnPointerPressed(_In_ PointerPoint^ point) override;
	virtual bool OnPointerMoved(_In_ PointerPoint^ point) override;
	virtual bool OnPointerReleased(_In_ PointerPoint^ point) override;

	virtual void SetupExitTransition() override;

	property bool Win
	{
		void set(bool value)
		{
			_isWin = value;		
		}
	}

private:
	bool			_isWin;
	SummaryDialog^	_summaryDialog;
	SpriteRenderer* _pSpriteRenderer;

	RaceGameState^ _gamestateRace;

};