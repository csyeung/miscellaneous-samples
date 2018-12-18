#pragma once

#include "GameState/RenderableGameState.h"
#include "UI/Background.h"
#include "Sprites/SpriteRenderer.h"

using namespace GameFramework::GameState;
using namespace GameFramework::UI;


ref class BackgroundImageState : RenderableGameState
{
internal:
	BackgroundImageState() : RenderableGameState(L"BackgroundImage"), _background(nullptr) 
	{
		_matrix = XMMatrixIdentity();
	}

	virtual void OnEntered() override;
	virtual void Release() override;

	virtual void Render(_In_ float timeTotal, _In_ float timeDelta) override;

private:
	Background^	_background;
	XMMATRIX _matrix;

	SpriteRenderer* _pSpriteRenderer;
};

