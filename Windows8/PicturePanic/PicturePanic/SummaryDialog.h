#pragma once

#include "pch.h"
#include "UI/MenuScreen.h"

using namespace GameFramework::UI;

ref class SummaryDialog : MenuScreen
{
internal:
	SummaryDialog();
	~SummaryDialog();

	virtual void OnWindowSizeChange(bool animate) override;

	virtual void Update(float timeTotal, float timeDelta) override;
	virtual void Render(SpriteRenderer* pSpriteRenderer) override;
	virtual void ReleaseAllActiveResources() override;

	void SetComplete(bool flag);

	void StartAnimationSequence(bool initialize);
	void StartExitSequence();

	property bool Exited
	{
		bool get()
		{
			return (_exiting && _timeToExit <= 0.0f);
		}
	}

protected private:
	virtual void CreateButtons() override;
	
	GameFramework::UI::Button^ _buttonBackground;
	GameFramework::UI::Button^ _buttonFacebook;
	GameFramework::UI::Button^ _buttonTwitter;
	GameFramework::UI::Button^ _buttonWindowsShare;
	GameFramework::UI::Button^ _buttonMainMenu;
	GameFramework::UI::Button^ _buttonRetryPuzzle;
	GameFramework::UI::Button^ _buttonNextPuzzle;

	GameFramework::UI::Button^ _buttonPuzzleComplete;
	GameFramework::UI::Button^ _buttonTimeUp;

	XMFLOAT2 _posTitleGoal;
	XMFLOAT2 _posButtonRetryGoal;
	XMFLOAT2 _posButtonNextPuzzleGoal;
	XMFLOAT2 _posButtonTwitterGoal;
	XMFLOAT2 _posButtonFacebookGoal;
	XMFLOAT2 _posButtonWindowsShareGoal;
	XMFLOAT2 _posButtonMainMenuGoal;

	bool _exiting;
	float _timeToExit;
};