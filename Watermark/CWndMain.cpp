#include "pch.h"
#include "CWndMain.h"

void CWndMain::ClearRes()
{
}

LRESULT CWndMain::OnCreate(HWND hWnd, CREATESTRUCT* pcs)
{
	constexpr BOOL b{ 1 };
	DwmSetWindowAttribute(hWnd, DWMWA_EXCLUDED_FROM_PEEK, &b, sizeof(b));
	eck::GetThreadCtx()->UpdateDefColor();
	m_iDpi = eck::GetDpi(hWnd);

	RECT rcClient;
	GetClientRect(hWnd, &rcClient);
	m_cxClient = rcClient.right;
	m_cyClient = rcClient.bottom;
	ReCreateMemoryDC();
	ReCreateDWriteResources();
	UpdatePosSize();
	return 0;
}

void CWndMain::Paint()
{
	const auto pRt = m_pRenderTarget.Get();
	
	m_pBrush->SetColor(eck::ARGBToD2dColorF(App.GetOpt().GetCurrColor()));
	pRt->BeginDraw();
	pRt->Clear({});
	D2D1_POINT_2F pt{};
	pRt->DrawTextLayout(pt, m_pTextLayout1.Get(), m_pBrush.Get(),
		D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT);
	pt.y += (m_cyLine1 + m_cyPadding);
	pRt->DrawTextLayout(pt, m_pTextLayout2.Get(), m_pBrush.Get(),
		D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT);
	pRt->EndDraw();

	SIZE size{ m_cxClient, m_cyClient };
	POINT pt1{};
	BLENDFUNCTION bf{ AC_SRC_OVER,0,255,AC_SRC_ALPHA };
	UpdateLayeredWindow(HWnd, nullptr, nullptr, &size,
		m_DC.GetDC(), &pt1, 0, &bf, ULW_ALPHA);
}

void CWndMain::CalcWindowPosition(int cx, int cy, int& x, int& y)
{
	const auto hMon = MonitorFromWindow(HWnd, MONITOR_DEFAULTTONEAREST);
	MONITORINFO mi;
	mi.cbSize = sizeof(mi);
	GetMonitorInfoW(hMon, &mi);

	const auto& Opt = App.GetOpt();
	const auto dx = eck::DpiScale(Opt.dx, m_iDpi);
	const auto dy = eck::DpiScale(Opt.dy, m_iDpi);
	switch (Opt.ePos)
	{
	case PosType::TopLeft:
		x = mi.rcWork.left + dx;
		y = mi.rcWork.top + dy;
		break;
	case PosType::TopRight:
		x = mi.rcWork.right - cx - dx;
		y = mi.rcWork.top + dy;
		break;
	case PosType::BottomLeft:
		x = mi.rcWork.left + dx;
		y = mi.rcWork.bottom - cy - dy;
		break;
	default:
		x = mi.rcWork.right - cx - dx;
		y = mi.rcWork.bottom - cy - dy;
		break;
	}
}

void CWndMain::ReCreateMemoryDC()
{
	m_pRenderTarget.Clear();
	m_pBrush.Clear();
	if (m_DC.GetDC())
		m_DC.ReSize32(HWnd, m_cxClient, m_cyClient);
	else
		m_DC.Create32(HWnd, m_cxClient, m_cyClient);
	constexpr D2D1_RENDER_TARGET_PROPERTIES RtProps
	{
		.pixelFormat = { DXGI_FORMAT_B8G8R8A8_UNORM,D2D1_ALPHA_MODE_PREMULTIPLIED },
		.dpiX = 96.f,
		.dpiY = 96.f,
	};
	eck::g_pD2dFactory->CreateDCRenderTarget(&RtProps, &m_pRenderTarget);
	m_pRenderTarget->CreateSolidColorBrush({}, &m_pBrush);
	m_pRenderTarget->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);
	const RECT rcSub{ 0,0,m_cxClient, m_cyClient };
	m_pRenderTarget->BindDC(m_DC.GetDC(), &rcSub);
}

void CWndMain::ReCreateDWriteResources()
{
	const auto& Opt = App.GetOpt();

	const auto cyFont1 = eck::DpiScaleF(Opt.iPoint1 * 96.f / 72.f, m_iDpi);
	const auto cyFont2 = eck::DpiScaleF(Opt.iPoint2 * 96.f / 72.f, m_iDpi);

	eck::g_pDwFactory->CreateTextFormat(Opt.Font1.Data(), nullptr,
		DWRITE_FONT_WEIGHT(Opt.iWeight1), DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL, cyFont1,
		App.GetUserLocale().Data(), m_pTextFormat1.AddrOfClear());
	eck::g_pDwFactory->CreateTextFormat(Opt.Font2.Data(), nullptr,
		DWRITE_FONT_WEIGHT(Opt.iWeight2), DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL, cyFont2,
		App.GetUserLocale().Data(), m_pTextFormat2.AddrOfClear());

	const auto cxMax = (float)eck::DaGetSystemMetrics(SM_CXVIRTUALSCREEN, m_iDpi);
	const auto cyMax = (float)eck::DaGetSystemMetrics(SM_CYVIRTUALSCREEN, m_iDpi);
	eck::g_pDwFactory->CreateTextLayout(Opt.rsLine1.Data(), Opt.rsLine1.Size(),
		m_pTextFormat1.Get(), cxMax, cyMax, m_pTextLayout1.AddrOfClear());
	eck::g_pDwFactory->CreateTextLayout(Opt.rsLine2.Data(), Opt.rsLine2.Size(),
		m_pTextFormat2.Get(), cxMax, cyMax, m_pTextLayout2.AddrOfClear());
}

void CWndMain::OnAppEvent(const APP_NOTIFY& n)
{
	if (n.uFlags & ANF_MA_REFRESH)
	{
		ReCreateDWriteResources();
		UpdatePosSize();
		return;
	}
	if (n.uFlags & ANF_MA_UPDATE_SIZE)
		UpdatePosSize();
	else if (n.uFlags & ANF_MA_UPDATE_POS)
		UpdatePos();
	else if (n.uFlags & ANF_MA_UPDATE_COLOR)
		Paint();
}

LRESULT CWndMain::OnMsg(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_SIZE:
	{
		ECK_GET_SIZE_LPARAM(m_cxClient, m_cyClient, lParam);
		ReCreateMemoryDC();
	}
	return 0;

	case WM_CREATE:
		return OnCreate(hWnd, (CREATESTRUCT*)lParam);

	case WM_DESTROY:
		ClearRes();
		PostQuitMessage(0);
		return 0;

	case WM_SYSCOLORCHANGE:
		eck::MsgOnSysColorChangeMainWnd(hWnd, wParam, lParam);
		break;
	case WM_SETTINGCHANGE:
		eck::MsgOnSettingChangeFixDpiAwareV2(hWnd, wParam, lParam);
		if (eck::MsgOnSettingChangeMainWnd(hWnd, wParam, lParam))
			Paint();
		break;
	case WM_DPICHANGED:
	{
		m_iDpi = HIWORD(wParam);
		ReCreateDWriteResources();
		UpdatePosSize();
	}
	break;
	}
	return __super::OnMsg(hWnd, uMsg, wParam, lParam);
}

void CWndMain::UpdatePos()
{
	int x, y;
	CalcWindowPosition(m_cxClient, m_cyClient, x, y);
	SetWindowPos(HWnd, nullptr, x, y, 0, 0,
		SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}

void CWndMain::UpdatePosSize()
{
	m_cyPadding = eck::DpiScaleF((float)App.GetOpt().cyPadding, m_iDpi);
	float cxF, cyF;
	DWRITE_TEXT_METRICS tm;
	m_pTextLayout1->GetMetrics(&tm);
	cxF = tm.width;
	cyF = tm.height;
	m_cyLine1 = tm.height;
	m_pTextLayout2->GetMetrics(&tm);
	cxF = std::max(cxF, tm.width);
	cyF += (m_cyPadding + tm.height);
	m_cyLine2 = tm.height;

	const auto cx = int(ceilf(cxF) + eck::DpiScaleF(1.f, m_iDpi));
	const auto cy = (int)ceilf(cyF);
	int x, y;
	CalcWindowPosition(cx, cy, x, y);

	SetWindowPos(HWnd, nullptr, x, y, std::max(cx, 1), std::max(cy, 1),
		SWP_NOZORDER | SWP_NOACTIVATE);
	Paint();
}