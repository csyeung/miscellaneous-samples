#pragma once

#include "GameState/RenderableGameState.h"
#include "Camera/PerspectiveCamera.h"
#include "Resources/RenderTargetResource.h"
#include "Resources/DepthStencilResource.h"
#include "PicturePanic.h"
#include "CountdownState.h"
#include "GameTimerState.h"
#include "RaceSummaryState.h"
#include "FadeTransitionState.h"
#include "ParticleSystem/ParticleSystem2D.h"
#include "PuzzlePiece.h"

using namespace GameFramework::GameState;
using namespace GameFramework::Camera;
using namespace GameFramework::Resources;
using namespace GameFramework::ParticleSystem;

ref class MenuState;


ref class RaceGameState : RenderableGameState
{
internal:
	RaceGameState(_In_ MenuState^ stateMainMenu);

	void Reset();

	virtual bool Update(_In_ float timeTotal, _In_ float timeDelta) override;
	virtual void Render(_In_ float timeTotal, _In_ float timeDelta) override;
	virtual void OnEntered() override;
	virtual void Release() override;
	virtual bool OnPointerPressed(_In_ PointerPoint^ point) override;
	virtual void OnWindowSizeChange(_In_ float screenWidth, _In_ float screenHeight) override;
	virtual void ReleaseActiveResources() override;

	void Retry();
	void NextPuzzle();
	void LeaveGame();

	property int Rows
	{
		void set(int value)
		{
			_numRows = value;
		}
	}

	property int Columns
	{
		void set(int value)
		{
			_numColumns = value;
		}
	}

	property Platform::String^ PuzzleImagePath
	{
		void set(Platform::String^ value)
		{
			_puzzleImagePath = value;
		}
	}

	property float PlayTime
	{
		void set(float value)
		{
			_playTime = value;
		}
	}
	
private:
	enum class CurrentGameState
	{
		STARTUP,
		SHOWPUZZLE,
		COUNTDOWN,
		HIDEPUZZLE,
		PLAY,
		LOSE,
		WIN,
		NEXTPUZZLE
	};

	static const float CAMERA_ASPECTRATIO;
	static const float CAMERA_FOV;
	static const float CAMERA_NEARCLIP;
	static const float CAMERA_FARCLIP;
	static const float CAMERA_POS_Z;
	static const int MAX_PUZZLES;
	static const float PUZZLE_STARTUPTIME;
	static const float PUZZLE_SHOWTIME;
	static const float PUZZLE_HIDETIME;

	void CreateRaceGameResources();
	void CreateCorrectEffect();
	void CreatePuzzlePieces();
	void SetupCelebrationEffects();

	// puzzle control function
	void SelectNextPuzzle();
	void ResetPuzzle();
	void FlipPuzzle(_In_ bool hide, _In_ float timeToFlip);
	void ScramblePuzzle();

	void UpdateStartUpState(_In_ float timeDelta);
	void UpdateShowPuzzleState(_In_ float timeDelta);
	void UpdateCountDownState(_In_ float timeDelta);
	void UpdateHidePuzzleState(_In_ float timeDelta);
	void UpdatePlayState(_In_ float timeDelta);
	void UpdateNextPuzzleState(_In_ float timeDelta);
	

	void RenderPuzzlePieces(_In_ float timeTotal);

	PicturePanic^ _game;

	PerspectiveCamera*	    _pCamera;

	RenderTargetResource^			_renderTargetPuzzle;
	DepthStencilResource^			_depthStencil;
	ComPtr<ID3D11DepthStencilState> _depthStencilState;

	SpriteRenderer*					_pSpriteRenderer;

	vector<PuzzlePiece^>			_vecPuzzlePieces;
	PuzzlePiece^			_pFirstSelectedPiece;

	// puzzle control 
	int				 _currentPuzzle;
	CurrentGameState _state;
	float			_startUpTime;
	float			_showPuzzleTime;
	float			_hidePuzzleTime;
	
	float			_renderTargetYStart;
	float			_renderTargetYTarget;
	float			_renderTargetYCurrent;
	float			_renderTargetMoveTime;

	// puzzle game mode attributes
	int				  _numRows;
	int			 	  _numColumns;
	Platform::String^ _puzzleImagePath;
	float			  _playTime;
	float			  _timeToWaitForNextPuzzle;

	// celebration effects
	vector<ParticleSystem2D^>		_vecParticleSystem2Ds;

	// effect for correct selection
	PostHaloEffect^					_effectCorrect;

	// states needed during game mode
	CountdownState^		 _gamestateCountdown;
	GameTimerState^		 _gamestateTimer;
	RaceSummaryState^    _gamestateSummary;
	FadeTransitionState^ _gamestateFadeTransition;
	MenuState^			 _stateMainMenu;

};