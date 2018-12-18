#include "pch.h"
#include "CountdownState.h"
#include "GameState/GameStateManager.h"
#include "Resources/ResourceFactory.h"
#include "PicturePanic.h"
#include "Globals/GameGlobals.h"

using namespace GameFramework::Globals;

const float CountdownState::COUNTDOWN_TIME = 3.0f;
const float CountdownState::GO_TIME = 0.75f;


CountdownState::CountdownState() : RenderableGameState(L"Countdown"), _pSpriteRenderer(nullptr)
{
}


void CountdownState::OnEntered()
{
	GameState::OnEntered();

	if (_pSpriteRenderer == nullptr)
	{
		_pSpriteRenderer = new SpriteRenderer();
		_pSpriteRenderer->Initialize(GameGlobals::D3DDevice);

		// load 3, 2, 1 go, win, lose
		_texture3 = ResourceFactory::CreateTexture2DResource("Assets\\UI\\InGame\\3.png");
		_texture2 = ResourceFactory::CreateTexture2DResource("Assets\\UI\\InGame\\2.png");
		_texture1 = ResourceFactory::CreateTexture2DResource("Assets\\UI\\InGame\\1.png");

		Platform::String^ assetPath = "Assets\\Languages\\" + PicturePanic::Instance->Language + L"\\InGame\\";
		_textureGo = ResourceFactory::CreateTexture2DResource(assetPath + "go.png");
	}

	_countDownTime = COUNTDOWN_TIME + 1;
	_countDownSecond = static_cast<int>(COUNTDOWN_TIME);
	
}


bool CountdownState::Update(float timeTotal, float timeDelta)
{
	if (_countDownTime > 1.0f)
	{
		_countDownTime -= timeDelta;

		_countDownSecond = static_cast<int>(_countDownTime);		

		if (_countDownTime <= 1.0f)
		{
			_countDownTime = 0.0f;

			_goTime = GO_TIME;
		}
	}
	else
	{
		_goTime -= timeDelta;

		if (_goTime <= 0.0f)
		{
			// remove this state
			GameStateManager::Instance->Pop();
		}
	}

	return true;
}


void CountdownState::Render(_In_ float timeTotal, _In_ float timeDelta)
{
	ID3D11DeviceContext1* pContext = GameGlobals::D3DContext;

	Texture2DResource^ texture = nullptr;

	if (_countDownSecond > 0)
	{
		// add 1 to get full seconds from 3, 2, 1
		switch(_countDownSecond)
		{
		case 3:
			texture = _texture3;

			break;

		case 2:
			texture = _texture2;
			break;

		case 1:
			texture = _texture1;
			break;
		}
	}
	else
	{
		texture = _textureGo;
	}

	float xCountDown = PicturePanic::Instance->RenderWindowSize / 2.0f - texture->Width / 2.0f;
	float yCountDown = (GameGlobals::WindowHeight / 2.0f) - texture->Height / 2.0f;

	XMMATRIX matTranslate = XMMatrixTranslation(xCountDown, yCountDown, 0);

	_pSpriteRenderer->Begin(pContext, SpriteRenderer::FilterMode::Linear);
	_pSpriteRenderer->Render(texture->ShaderResourceView, matTranslate);
	_pSpriteRenderer->End();
}


