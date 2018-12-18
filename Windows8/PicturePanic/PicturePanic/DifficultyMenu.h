#pragma once
#include "pch.h"

#include "UI/MenuScreen.h"
#include "UI/UIImage.h"

using namespace GameFramework::UI;

ref class DifficultyInfo
{
internal:
	DifficultyInfo() {}
	
	Platform::String^ modePath;
	int width;
	int height;
	float timeForPuzzle;
};

ref class DifficultyMenu : MenuScreen
{
	internal:
		DifficultyMenu();
		~DifficultyMenu();

		virtual void Initialize() override;
		virtual void OnWindowSizeChange(bool animate) override;

		virtual property DifficultyInfo^ SelectedDifficultyInfo
		{
			DifficultyInfo^ get()
			{
				return _selectedDataInfo;
			}
		}
				
	protected private:
		virtual void CreateButtons() override;

		void DifficultySelected(Platform::Object^ data);

		static const int EASY_ROWS;
		static const int EASY_COLUMNS;
		static const float EASY_TIME;

		static const int MEDIUM_ROWS;
		static const int MEDIUM_COLUMNS;
		static const float MEDIUM_TIME;

		static const int HARD_ROWS;
		static const int HARD_COLUMNS;
		static const float HARD_TIME;

		static const float CONTROL_ANIMATION_TIME;
		
		GameFramework::UI::Button^ _buttonEasyDifficulty;
		GameFramework::UI::Button^ _buttonMediumDifficulty;
		GameFramework::UI::Button^ _buttonHardDifficulty;
		GameFramework::UI::Button^ _buttonBack;

		UIImage^ _imageTitle;

		DifficultyInfo^ _selectedDataInfo;
};