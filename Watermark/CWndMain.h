#pragma once
class CWndMain : public eck::CWnd
{
private:
	eck::CEzCDC m_DC{};
	int m_cxClient{};
	int m_cyClient{};
	int m_iDpi{ USER_DEFAULT_SCREEN_DPI };

	GpFont* m_pFont1{}, * m_pFont2{};
	float m_cyLine1{}, m_cyLine2{}, m_cyPadding{};

	GpGraphics* m_pGraphics{};
	GpStringFormat* m_pStrFmt{};
	GpSolidFill* m_pSolidFill{};

	void ClearRes();

	LRESULT OnCreate(HWND hWnd, CREATESTRUCT* pcs);

	void UpdateGraphics();

	void UpdatePosSize();

	void Paint();

	void UpdateLayered();

	void CalcPos(int cx, int cy, int& x, int& y);
public:
	ECK_CWND_SINGLEOWNER(CWndMain);
	ECK_CWND_CREATE_CLS_HINST(eck::WCN_DUMMY, eck::g_hInstance);

	LRESULT OnMsg(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;

	void UpdateFont();

	void UpdatePos();
};