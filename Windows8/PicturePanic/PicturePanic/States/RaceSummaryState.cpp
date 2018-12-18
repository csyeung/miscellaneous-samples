#include "pch.h"
#include "RaceSummaryState.h"
#include "RaceGameState.h"
#include "GameState/GameStateManager.h"

using namespace GameFramework::Globals;


RaceSummaryState::RaceSummaryState(_In_ RaceGameState^ gamestateRace) : RenderableGameState(L"Summary"), _summaryDialog(nullptr), _pSpriteRenderer(nullptr), _isWin(false)
{
	_gamestateRace = gamestateRace;
}


void RaceSummaryState::OnEntered()
{
	GameState::OnEntered();

	if (_summaryDialog == nullptr)
	{
		_summaryDialog  = ref new SummaryDialog();

		_summaryDialog->Initialize();

		_pSpriteRenderer = new SpriteRenderer();
		_pSpriteRenderer->Initialize(GameGlobals::D3DDevice);
	}

	_summaryDialog->SetComplete(_isWin);

	OnWindowSizeChange(GameGlobals::WindowWidth, GameGlobals::WindowHeight);
}


void RaceSummaryState::OnWindowSizeChange(_In_ float screenWidth, _In_ float screenHeight)
{
	_summaryDialog->OnWindowSizeChange(true);

	_summaryDialog->StartAnimationSequence(true);
}


bool RaceSummaryState::OnPointerPressed(_In_ PointerPoint^ point)
{
	// MenuState assumes to be the top most state to accept input
	if (GameStateManager::Instance->ActiveGameState != this)
	{
		return false;
	}

	_summaryDialog->SetExclusivePointer(point->PointerId);

	return _summaryDialog->HitTest(point->PointerId, point->Position.X, point->Position.Y, PointerState::PRESSED);
}


bool RaceSummaryState::OnPointerMoved(_In_ PointerPoint^ point)
{
	// MenuState assumes to be the top most state to accept input
	if (GameStateManager::Instance->ActiveGameState != this)
	{
		return false;
	}

	return _summaryDialog->HitTest(point->PointerId, point->Position.X, point->Position.Y, PointerState::MOVE);
}


bool RaceSummaryState::OnPointerReleased(_In_ PointerPoint^ point) 
{
	// MenuState assumes to be the top most state to accept input
	if (GameStateManager::Instance->ActiveGameState != this)
	{
		return false;
	}

	if (_summaryDialog->IsExclusivePointer(point->PointerId))
	{
		int nextState = 0;
		int direction = 0;

		if (_summaryDialog->HitTest(point->PointerId, point->Position.X, point->Position.Y, PointerState::RELEASE, &nextState, &direction))
		{
			_summaryDialog->ReleaseExclusivePointer();

			switch ((SummaryMenuAction)nextState)
			{
			case SummaryMenuAction::RETRY:
				_gamestateRace->Retry();
				break;

			case SummaryMenuAction::NEXTPUZZLE:
				_gamestateRace->NextPuzzle();
				break;

			case SummaryMenuAction::LEAVEGAME:
				_gamestateRace->LeaveGame();
				break;
			}

			return true;
		}
	}

	return false;	
}


void RaceSummaryState::SetupExitTransition()
{
	_summaryDialog->StartExitSequence();
}


bool RaceSummaryState::Update(_In_ float timeTotal, _In_ float timeDelta)
{
	_summaryDialog->Update(timeTotal, timeDelta);

	return !(_summaryDialog->Exited);
}


void RaceSummaryState::Render(_In_ float timeTotal, _In_ float timeDelta)
{
	_pSpriteRenderer->Begin(GameGlobals::D3DContext);

	_summaryDialog->Render(_pSpriteRenderer);

	_pSpriteRenderer->End();
}


void RaceSummaryState::Release()
{
	delete _pSpriteRenderer;
	_pSpriteRenderer = nullptr;

	_summaryDialog->ReleaseAllActiveResources();

	_summaryDialog = nullptr;
}


