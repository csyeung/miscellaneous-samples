#pragma once
#include "pch.h"

#include "UI/MenuScreen.h"
#include "UI/UIImage.h"

#include <vector>

using namespace std;

using namespace GameFramework::UI;

enum PicturePanicGameState;

ref class ModeMenu : MenuScreen
{
	internal:
		ModeMenu();
		~ModeMenu();

		virtual void Initialize() override;
		virtual void OnWindowSizeChange(bool animate) override;
		
		virtual void Update(float timeTotal, float timeDelta) override;
		virtual void Render(SpriteRenderer* pSpriteRenderer) override;

		virtual void ReleaseAllActiveResources() override;
		
	protected private:
		virtual void CreateButtons() override;
		void CreateArrowButtons();
		void MoveModes(int direction);

		void OnLeftArrow(Platform::Object^ data);
		void OnRightArrow(Platform::Object^ data);

		static const float CONTROL_ANIMATION_TIME;
		static const bool ENABLE_DAILY;
		static const bool ENABLE_HEADTOHEAD;
		static const bool ENABLE_SCOREATTACK;
		static const float MOVE_TIME;
		static const int MAX_MODES;

		GameFramework::UI::Button^ _buttonLeftArrow;
		GameFramework::UI::Button^ _buttonRightArrow;
		GameFramework::UI::Button^ _buttonBack;
		GameFramework::UI::Button^ _buttonRace;
		GameFramework::UI::Button^ _buttonHeadToHead;
		GameFramework::UI::Button^ _buttonDailyChallenge;
		GameFramework::UI::Button^ _buttonScoreAttack;

		int _currentModeIndex;
		float _noClickArrowDelay;

		UIImage^ _imageTitle;
};