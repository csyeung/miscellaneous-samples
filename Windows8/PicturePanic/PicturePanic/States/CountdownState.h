#pragma once

#include "GameState/RenderableGameState.h"
#include "Resources/Texture2DResource.h"
#include "Sprites/SpriteRenderer.h"

using namespace GameFramework::GameState;
using namespace GameFramework::Resources;
using namespace GameFramework::Sprites;


ref class CountdownState : RenderableGameState
{
internal:
	CountdownState();

	virtual bool Update(float timeTotal, float timeDelta) override;
	virtual void Render(_In_ float timeTotal, _In_ float timeDelta) override;
	virtual void OnEntered() override;

	property float RemainingTime
	{
		float get()
		{
			return _countDownTime;
		}
	}

	
private:
	static const float COUNTDOWN_TIME;
	static const float GO_TIME;

	float _countDownTime;
	int	  _countDownSecond;

	float _goTime;

	Texture2DResource^	_texture3;
	Texture2DResource^	_texture2;
	Texture2DResource^	_texture1;
	Texture2DResource^	_textureGo;

	SpriteRenderer* _pSpriteRenderer;
};