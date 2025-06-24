#pragma once
#include "CApp.h"


class CWndMain : public eck::CWnd
{
private:
	eck::CEzCDC m_DC{};
	ComPtr<ID2D1DCRenderTarget> m_pRenderTarget{};
	ComPtr<ID2D1SolidColorBrush> m_pBrush{};
	ComPtr<IDWriteTextLayout> m_pTextLayout1, m_pTextLayout2;
	ComPtr<IDWriteTextFormat> m_pTextFormat1, m_pTextFormat2;

	float m_cyLine1{},
		m_cyLine2{},
		m_cyPadding{};

	int m_cxClient{};
	int m_cyClient{};
	int m_iDpi{ USER_DEFAULT_SCREEN_DPI };

	void ClearRes();

	LRESULT OnCreate(HWND hWnd, CREATESTRUCT* pcs);

	void UpdatePosSize();

	void Paint();

	void UpdateLayered();

	void CalcWindowPosition(int cx, int cy, int& x, int& y);

	// 重新创建内存DC和Dw渲染目标
	// 使用m_cxClient和m_cyClient指定的尺寸
	void ReCreateMemoryDC();

	void ReCreateDWriteResources();

	void OnAppEvent(const APP_NOTIFY& n);
public:
	ECK_CWND_SINGLEOWNER_NO_DEF_CONS(CWndMain);
	ECK_CWND_CREATE_CLS_HINST(eck::WCN_DUMMY, eck::g_hInstance);

	CWndMain()
	{
		App.GetSignal().Connect(this, &CWndMain::OnAppEvent);
	}

	LRESULT OnMsg(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;

	void UpdatePos();
};