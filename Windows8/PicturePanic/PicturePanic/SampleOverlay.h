//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

#pragma once
#include "pch.h"

#include "Utility/Utility.h"

using namespace Windows::UI::Core;
using namespace Windows::Foundation;
using namespace Microsoft::WRL;
using namespace Windows::UI::ViewManagement;
using namespace Windows::Graphics::Display;
using namespace D2D1;

using namespace GameFramework::Utility;

ref class SampleOverlay
{
internal:
    SampleOverlay();

    void Initialize(_In_ Platform::String^ caption);

    void ResetDirectXResources();

    void UpdateForWindowSizeChange(float windowWidth, float windowHeight);

	void Update(float timeDelta);
    void Render(float width, float height);

    float GetTitleHeightInDips();

	void UpdateText(Platform::String^ text);
	void SetTime(Platform::String^ textTimer);
	void SetCountDownTime(Platform::String^ textCountdown);
	
	void ShowTimer(bool flag);
	void ShowCountdown(bool flag);
	void ShowLose(bool flag);
	void ShowWin(bool flag);
	void ShowWarning(bool flag);
	inline void ShowFranticWarning() { _warningSpeedScale = 0.5f; }

	void Reset();

	inline bool NeedsWarning() { return (_showWarning == false); }


private:
	ComPtr<IDWriteTextFormat>						_nameTextFormat;
	ComPtr<IDWriteTextFormat>						_timerTextFormat;
	ComPtr<IDWriteTextFormat>						_timerSnapTextFormat;
	ComPtr<IDWriteTextFormat>						_countDownTextFormat;
	ComPtr<IDWriteTextFormat>						_loseTextFormat;
	ComPtr<IDWriteTextFormat>						_winTextFormat;

    Microsoft::WRL::ComPtr<ID2D1Factory1>           m_d2dFactory;
    Microsoft::WRL::ComPtr<ID2D1Device>             m_d2dDevice;
    Microsoft::WRL::ComPtr<ID2D1DeviceContext>      m_d2dContext;
    Microsoft::WRL::ComPtr<IDWriteFactory>          m_dwriteFactory;
    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>    m_whiteBrush;
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>	_greenBrush;
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>	_redBrush;
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>	_yellowBrush;
    Microsoft::WRL::ComPtr<ID2D1DrawingStateBlock>  m_stateBlock;

    Microsoft::WRL::ComPtr<IWICImagingFactory>      m_wicFactory;
    Microsoft::WRL::ComPtr<ID2D1Bitmap>             m_logoBitmap;
    Microsoft::WRL::ComPtr<IDWriteTextLayout>       m_textLayout;
	Microsoft::WRL::ComPtr<IDWriteTextLayout>       _timerLayout;
	ComPtr<IDWriteTextLayout>						_timerSnapLayout;
	Microsoft::WRL::ComPtr<IDWriteTextLayout>       _countDownLayout;
	Microsoft::WRL::ComPtr<IDWriteTextLayout>       _loseLayout;
	Microsoft::WRL::ComPtr<IDWriteTextLayout>       _winLayout;

	
    UINT                                            m_idIncrement;
    bool                                            m_drawOverlay;
    Platform::String^                               m_sampleName;
    float                                           m_padding;
    float                                           m_textVerticalOffset;
    D2D1_SIZE_F                                     m_logoSize;
    float                                           m_overlayWidth;
	bool											_showTimer;
	bool											_showCountdown;
	bool											_showCountdownGo;
	bool											_showLose;
	bool											_showWin;

	bool											_showWarning;
	float											_warningSpeedScale;
	float										    _timeToSwitchWarningColor;
	int												_currentColorIndex;
	std::vector<ID2D1SolidColorBrush *>					_vecWarningBrushes;
	float                                           _timerWidth;
	float											_timerHeight;
};

