#pragma once
#include "CApp.h"

class CWndDesktopText : public eck::CWnd
{
private:
	eck::CEzCDC m_DC{};
	ComPtr<ID2D1DCRenderTarget> m_pRenderTarget{};
	ComPtr<ID2D1DeviceContext> m_pDeviceContext{};
	ComPtr<ID2D1SolidColorBrush> m_pBrush{};
	ComPtr<IDWriteTextLayout> m_pTextLayout{};
	ComPtr<IDWriteTextFormat> m_pTextFormat{};

	int m_cxClient{};
	int m_cyClient{};
	int m_iDpi{ USER_DEFAULT_SCREEN_DPI };
	int m_cxyShadowExtent{};

	RECT m_rcMonitorWork{};

	// cx、cy为文本尺寸，不含阴影边缘
	void CalcWindowPosition(int cx, int cy, int& x, int& y);

	void UpdatePosSize();

	void Paint();

	void UpdatePos();

	// 重新创建内存DC和Dw渲染目标
	// 使用m_cxClient和m_cyClient指定的尺寸
	void ReCreateMemoryDC();

	void ReCreateDWriteResources();

	void OnAppEvent(const APP_NOTIFY& n);

	void SetZOrder();

	void UpdateMonitorInfo();
public:
	ECK_CWND_SINGLEOWNER_NO_DEF_CONS(CWndDesktopText);
	ECK_CWND_CREATE_CLS_HINST(eck::WCN_DUMMY, eck::g_hInstance);

	CWndDesktopText()
	{
		App.GetSignal().Connect(this, &CWndDesktopText::OnAppEvent);
	}

	LRESULT OnMsg(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;
};