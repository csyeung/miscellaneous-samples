#pragma once
#include "pch.h"
#include "UI/MenuScreen.h"
#include "UI/UIImage.h"
#include "Resources/Texture2DResource.h"

using namespace GameFramework::UI;
using namespace GameFramework::Resources;

ref class MainMenu : MenuScreen
{
	internal:
		MainMenu();
		~MainMenu();

		virtual void Initialize() override;
		virtual void OnWindowSizeChange(bool animate) override;

		virtual void ReleaseAllActiveResources() override;	

	protected private:
		virtual void CreateButtons() override;

		static const float CONTROL_ANIMATION_TIME;

		GameFramework::UI::Button^ _buttonPlay;
		GameFramework::UI::Button^ _buttonOptions;

		bool _firstTime;

		UIImage^							_imageLogo;		
};