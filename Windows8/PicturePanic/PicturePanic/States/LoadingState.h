#pragma once

#include "Resources/RenderTargetResource.h"
#include "GameState/RenderableGameState.h"
#include "PicturePanic.h"
#include "Sprites/SpriteRenderer.h"

using namespace GameFramework::GameState;
using namespace GameFramework::Sprites;
using namespace GameFramework::Resources;

ref class LoadingState : RenderableGameState
{
internal:
	LoadingState();

	virtual void OnEntered() override;
	virtual void OnWindowSizeChange(_In_ float screenWidth, _In_ float screenHeight) override;

	virtual void Release() override;

	virtual bool Update(_In_ float timeTotal, _In_ float timeDelta) override;
	virtual void Render(_In_ float timeTotal, _In_ float timeDelta) override;

private:
	void PrepareResources();
	void DoAsynchronousLoading();
	void CreateRenderTarget();

	property bool LoadingComplete
	{
		bool get()
		{
			return (_loadingComplete && _defaultLoadingTime <= 0.0f && PicturePanic::Instance->Initialized);
		}
	}

	static const float DEFAULT_LOAD_TIME;

	RenderTargetResource^ _renderTarget;

	bool _loadingComplete;
	float _defaultLoadingTime;

	SpriteRenderer* _spriteRenderer;

	Texture2DResource^ _textureLogo;
};