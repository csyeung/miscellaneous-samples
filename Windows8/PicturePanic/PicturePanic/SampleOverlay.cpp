//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

#include "pch.h"
#include "SampleOverlay.h"
#include "Globals/GameGlobals.h"

using namespace Windows::UI::Core;
using namespace Windows::Foundation;
using namespace Microsoft::WRL;
using namespace Windows::UI::ViewManagement;
using namespace Windows::Graphics::Display;
using namespace D2D1;
using namespace GameFramework;
using namespace GameFramework::Globals;

SampleOverlay::SampleOverlay() : m_drawOverlay(true), _showCountdown(false)
{
	Reset();
}

void SampleOverlay::Reset()
{
	_showWarning = false;
	_currentColorIndex = 0;
	_warningSpeedScale = 1.0f;

}

void SampleOverlay::Initialize(_In_ Platform::String^ caption)
{
	m_wicFactory = GameGlobals::WICFactory;
	m_dwriteFactory = GameGlobals::DWriteFactory;
    m_sampleName = caption;
	m_d2dDevice = GameGlobals::D2DDevice;
	m_d2dContext = GameGlobals::D2DContext;
    m_padding = 3.0f;
    m_textVerticalOffset = 5.0f;
    m_logoSize = D2D1::SizeF(0.0f, 0.0f);
    m_overlayWidth = 0.0f;

    ComPtr<ID2D1Factory> factory;
    m_d2dDevice->GetFactory(&factory);

    DXCall(factory.As(&m_d2dFactory));

    ResetDirectXResources();
}

void SampleOverlay::ResetDirectXResources()
{
    DXCall(m_d2dContext->CreateSolidColorBrush(ColorF(ColorF::White), &m_whiteBrush));
	DXCall(m_d2dContext->CreateSolidColorBrush(ColorF(ColorF::Red), &_redBrush));
	DXCall(m_d2dContext->CreateSolidColorBrush(ColorF(ColorF::Yellow), &_yellowBrush));
	DXCall(m_d2dContext->CreateSolidColorBrush(ColorF(ColorF::Green), &_greenBrush));

	_vecWarningBrushes.push_back(m_whiteBrush.Get());
	_vecWarningBrushes.push_back(_redBrush.Get());
	    
    DXCall( m_dwriteFactory->CreateTextFormat(L"Segoe UI",
											  nullptr,
											  DWRITE_FONT_WEIGHT_LIGHT,
											  DWRITE_FONT_STYLE_NORMAL,
											  DWRITE_FONT_STRETCH_NORMAL,
											  36.0f,
											  L"en-US",
											  &_nameTextFormat));

    DXCall(_nameTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING));

    DXCall(_nameTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR));

    UpdateText("");


	DXCall( m_dwriteFactory->CreateTextFormat(L"Segoe UI",
											  nullptr,
											  DWRITE_FONT_WEIGHT_HEAVY,
											  DWRITE_FONT_STYLE_NORMAL,
											  DWRITE_FONT_STRETCH_NORMAL,
											  150.0f,
											  L"en-US",
											  &_timerTextFormat));

    DXCall(_timerTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING));

    DXCall(_timerTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR));

	DXCall( m_dwriteFactory->CreateTextFormat(L"Segoe UI",
											  nullptr,
											  DWRITE_FONT_WEIGHT_HEAVY,
											  DWRITE_FONT_STYLE_NORMAL,
											  DWRITE_FONT_STRETCH_NORMAL,
											  75.0f,
											  L"en-US",
											  &_timerSnapTextFormat));


	DXCall( m_dwriteFactory->CreateTextFormat(L"Segoe UI",
											  nullptr,
											  DWRITE_FONT_WEIGHT_HEAVY,
											  DWRITE_FONT_STYLE_NORMAL,
											  DWRITE_FONT_STRETCH_NORMAL,
											  150.0f,
											  L"en-US",
											  &_countDownTextFormat));

    DXCall(_countDownTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING));

    DXCall(_countDownTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR));


    

    DXCall(m_d2dFactory->CreateDrawingStateBlock(&m_stateBlock));
}

void SampleOverlay::UpdateText(Platform::String^ text)
{
	DXCall(
        m_dwriteFactory->CreateTextLayout(
            text->Data(),
            text->Length(),
            _nameTextFormat.Get(),
            4096.0f,
            4096.0f,
            &m_textLayout
            )
        );

    DWRITE_TEXT_METRICS metrics = {0};
    DXCall(
        m_textLayout->GetMetrics(&metrics)
        );

	m_overlayWidth = m_padding * 3.0f + m_logoSize.width + metrics.width;
}

void SampleOverlay::ShowTimer(bool flag)
{
	_showTimer = flag;

	if (!_showTimer)
	{
		// no need for warning text
		_showWarning = false;
	}
}

void SampleOverlay::ShowCountdown(bool flag)
{
	_showCountdown = flag;

	if (!_showCountdown)
	{
		_showCountdownGo = false;
	}
}

void SampleOverlay::ShowLose(bool flag)
{
	_showLose = flag;

	if (_showLose)
	{
		Platform::String^ loseString = L"You Lose!";

		DXCall(
        m_dwriteFactory->CreateTextLayout(
            loseString->Data(),
            loseString->Length(),
            _countDownTextFormat.Get(),
            4096.0f,
            4096.0f,
            &_loseLayout
            )
        );
	}
}

void SampleOverlay::ShowWin(bool flag)
{
	_showWin = flag;

	if (_showWin)
	{
		Platform::String^ winString = L"You Win!!";

		DXCall(
        m_dwriteFactory->CreateTextLayout(
            winString->Data(),
            winString->Length(),
            _countDownTextFormat.Get(),
            4096.0f,
            4096.0f,
            &_winLayout
            )
        );
	}
}

void SampleOverlay::SetTime(Platform::String^ textTimer)
{
	if (_showTimer)
	{
		bool isSnap = (ApplicationView::Value == ApplicationViewState::Snapped);

		IDWriteTextFormat* pTextFormat = (isSnap) ? _timerSnapTextFormat.Get() : _timerTextFormat.Get();
		IDWriteTextLayout** ppTextLayout = (isSnap) ? _timerSnapLayout.GetAddressOf() : _timerLayout.GetAddressOf();

		DXCall( m_dwriteFactory->CreateTextLayout(textTimer->Data(),
												  textTimer->Length(),
												  pTextFormat,
												  4096.0f,
												  4096.0f,
												  ppTextLayout));

		DWRITE_TEXT_METRICS metrics = {0};
    
		DXCall((*ppTextLayout)->GetMetrics(&metrics));

		_timerHeight = metrics.height;
		_timerWidth  = metrics.width;
	}
}

void SampleOverlay::SetCountDownTime(Platform::String^ textCountdown)
{
	if (_showCountdown)
	{
		if (textCountdown == L"Go!!")
		{
			_showCountdownGo = true;
		}

		DXCall( m_dwriteFactory->CreateTextLayout(textCountdown->Data(),
												  textCountdown->Length(),
												  _countDownTextFormat.Get(),
												  4096.0f,
												  4096.0f,
												  &_countDownLayout));
	}
}
	


void SampleOverlay::UpdateForWindowSizeChange(float windowWidth, float windowHeight)
{
    /*if (CoreWindow::GetForCurrentThread()->Bounds.Width < m_overlayWidth)
    {
        m_drawOverlay = false;
    }
    else
    {
        m_drawOverlay = true;
    }*/
}

void SampleOverlay::Update(float timeDelta)
{
	if (_showWarning)
	{
		_timeToSwitchWarningColor -= timeDelta;

		if (_timeToSwitchWarningColor <= 0)
		{
			_timeToSwitchWarningColor = 0.5f * _warningSpeedScale;

			++_currentColorIndex;

			if (_currentColorIndex > 1)
			{
				_currentColorIndex = 0;
			}
		}
	}
}

void SampleOverlay::ShowWarning(bool flag)
{
	_showWarning = flag;
}

void SampleOverlay::Render(float width, float height)
{
    if (m_drawOverlay)
    {
        m_d2dContext->SaveDrawingState(m_stateBlock.Get());

        m_d2dContext->BeginDraw();
        //m_d2dContext->SetTransform(D2D1::Matrix3x2F::Identity());
        //m_d2dContext->DrawBitmap(
        //    m_logoBitmap.Get(),
        //    D2D1::RectF(m_padding, 0.0f, m_logoSize.width + m_padding, m_logoSize.height)
        //    );

        m_d2dContext->DrawTextLayout(
            Point2F(m_logoSize.width + 2.0f * m_padding, m_textVerticalOffset),
            m_textLayout.Get(),
            m_whiteBrush.Get()
            );

		if (_showLose)
		{
			m_d2dContext->DrawTextLayout(Point2F(120, 350),
										 _loseLayout.Get(),
										 _redBrush.Get());
		}
		else if (_showWin)
		{
			m_d2dContext->DrawTextLayout(Point2F(110, 350),
										 _winLayout.Get(),
										 _yellowBrush.Get());
		}
		else if (_showCountdownGo)
		{
			m_d2dContext->DrawTextLayout(Point2F(300, 350),
												 _countDownLayout.Get(),
												 _greenBrush.Get());
		}
		else if (_showCountdown)
		{
			m_d2dContext->DrawTextLayout(Point2F(410, 350),
												 _countDownLayout.Get(),
												 _yellowBrush.Get());
		}
		else if (_showTimer)
		{
			ID2D1SolidColorBrush* pBrush;
			if (!_showWarning)
			{
				pBrush = m_whiteBrush.Get();
			}
			else
			{
				pBrush = _vecWarningBrushes[_currentColorIndex];
			}

			bool isSnap = (ApplicationView::Value == ApplicationViewState::Snapped);

			IDWriteTextLayout* pTextLayout = (isSnap) ? _timerSnapLayout.Get() : _timerLayout.Get();
			D2D1_POINT_2F location;
			
			if (isSnap)
			{
				location = Point2F(width / 2.0f - _timerWidth / 2.0f, height / 2.0f);
			}
			else
			{
				location = Point2F(width - _timerWidth - 5, height / 2.0f - 100);
			}

			m_d2dContext->DrawTextLayout(location, pTextLayout, pBrush);			
		}

        // We ignore the HRESULT returned as we want to application to handle the 
        // error when it uses Direct2D next.
        m_d2dContext->EndDraw();

        m_d2dContext->RestoreDrawingState(m_stateBlock.Get());
    }
}

float SampleOverlay::GetTitleHeightInDips()
{
    return m_logoSize.height;
}
