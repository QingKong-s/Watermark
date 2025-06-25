#include "pch.h"
#include "CWndDesktopText.h"

const static UINT s_uMsgTaskbarCreated = RegisterWindowMessageW(L"TaskbarCreated");

void CWndDesktopText::Paint()
{
	const auto pRt = m_pRenderTarget.Get();

	m_pBrush->SetColor(eck::ARGBToD2dColorF(App.GetOpt().crDtText));
	pRt->BeginDraw();
	pRt->Clear({});
	DWRITE_TEXT_METRICS tm;
	m_pTextLayout->GetMetrics(&tm);
	pRt->DrawTextLayout({ -tm.left,0 }, m_pTextLayout.Get(), m_pBrush.Get(),
		D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT);
	pRt->EndDraw();

	SIZE size{ m_cxClient,m_cyClient };
	POINT pt1{};
	BLENDFUNCTION bf{ AC_SRC_OVER,0,255,AC_SRC_ALPHA };
	UpdateLayeredWindow(HWnd, nullptr, nullptr, &size,
		m_DC.GetDC(), &pt1, 0, &bf, ULW_ALPHA);
}

void CWndDesktopText::ReCreateDWriteResources()
{
	const auto& Opt = App.GetOpt();
	const auto cyFont = eck::DpiScaleF(Opt.iDtPoint * 96.f / 72.f, m_iDpi);

	eck::g_pDwFactory->CreateTextFormat(Opt.rsDtFont.Data(), nullptr,
		DWRITE_FONT_WEIGHT(Opt.iDtWeight), DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL, cyFont,
		App.GetUserLocale().Data(), m_pTextFormat.AddrOfClear());
	m_pTextFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);

	eck::g_pDwFactory->CreateTextLayout(Opt.rsDtTextParsed.Data(),
		Opt.rsDtTextParsed.Size(), m_pTextFormat.Get(),
		float(m_rcMonitorWork.right - m_rcMonitorWork.left),
		float(m_rcMonitorWork.bottom - m_rcMonitorWork.top),
		m_pTextLayout.AddrOfClear());
	if (Opt.eDtPos == PosType::TopLeft || Opt.eDtPos == PosType::BottomLeft)
		m_pTextLayout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	else
		m_pTextLayout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
}

void CWndDesktopText::ReCreateMemoryDC()
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

void CWndDesktopText::CalcWindowPosition(int cx, int cy, int& x, int& y)
{
	const auto dx = eck::DpiScale(App.GetOpt().dxDt, m_iDpi);
	const auto dy = eck::DpiScale(App.GetOpt().dyDt, m_iDpi);
	const auto cxEdge = (int)ceilf(eck::DpiScaleF(1.f, m_iDpi));
	const auto cyEdge = (int)ceilf(eck::DpiScaleF(1.f, m_iDpi));

	switch (App.GetOpt().eDtPos)
	{
	case PosType::TopLeft:
		x = m_rcMonitorWork.left + dx + cxEdge;
		y = m_rcMonitorWork.top + dy + cyEdge;
		break;
	case PosType::TopRight:
		x = m_rcMonitorWork.right - dx - cx - cxEdge;
		y = m_rcMonitorWork.top + dy + cyEdge;
		break;
	case PosType::BottomLeft:
		x = m_rcMonitorWork.left + dx + cxEdge;
		y = m_rcMonitorWork.bottom - dy - cy - cyEdge;
		break;
	default:// BottomRight
		x = m_rcMonitorWork.right - dx - cx - cxEdge;
		y = m_rcMonitorWork.bottom - dy - cy - cyEdge;
		break;
	}
}

void CWndDesktopText::UpdatePosSize()
{
	DWRITE_TEXT_METRICS tm;
	m_pTextLayout->GetMetrics(&tm);
	const auto cx = (int)ceilf(tm.width);
	const auto cy = (int)ceilf(tm.height);
	int x, y;
	CalcWindowPosition(cx, cy, x, y);
	SetWindowPos(HWnd, nullptr, x, y, cx, cy,
		SWP_NOZORDER | SWP_NOACTIVATE);
	Paint();
}

void CWndDesktopText::UpdatePos()
{
	int x, y;
	CalcWindowPosition(m_cxClient, m_cyClient, x, y);
	SetWindowPos(HWnd, nullptr, x, y, 0, 0,
		SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}

void CWndDesktopText::OnAppEvent(const APP_NOTIFY& n)
{

}

void CWndDesktopText::SetZOrder()
{
	const auto hProgman = FindWindowW(L"Progman", L"Program Manager");
	if (hProgman)
	{
		SetWindowLongPtrW(HWnd, GWLP_HWNDPARENT, (LONG_PTR)hProgman);
		SetWindowPos(HWnd, hProgman, 0, 0, 0, 0,
			SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
	}
	else
		SetWindowLongPtrW(HWnd, GWLP_HWNDPARENT, 0);
}

void CWndDesktopText::UpdateMonitorInfo()
{
	const auto hMon = MonitorFromWindow(HWnd, MONITOR_DEFAULTTONEAREST);
	MONITORINFO mi;
	mi.cbSize = sizeof(mi);
	GetMonitorInfoW(hMon, &mi);
	m_rcMonitorWork = mi.rcWork;
}

LRESULT CWndDesktopText::OnMsg(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == s_uMsgTaskbarCreated)
	{
		SetZOrder();
		UpdatePos();
		return 0;
	}

	switch (uMsg)
	{
	case WM_SETTINGCHANGE:
	{
		if ((lParam && eck::TcsEqual((PCWSTR)lParam, L"TraySettings")) ||
			wParam == SPI_SETWORKAREA)
		{
			UpdateMonitorInfo();
			UpdatePos();
		}
		eck::MsgOnSettingChangeFixDpiAwareV2(hWnd, wParam, lParam);
	}
	break;

	case WM_DPICHANGED:
		UpdateMonitorInfo();
		m_iDpi = HIWORD(wParam);
		ReCreateDWriteResources();
		UpdatePosSize();
		break;

	case WM_CREATE:
	{
		UpdateMonitorInfo();
		SetZOrder();

		constexpr BOOL b{ 1 };
		DwmSetWindowAttribute(hWnd, DWMWA_EXCLUDED_FROM_PEEK, &b, sizeof(b));
		m_iDpi = eck::GetDpi(hWnd);

		RECT rcClient;
		GetClientRect(hWnd, &rcClient);
		m_cxClient = rcClient.right;
		m_cyClient = rcClient.bottom;
		ReCreateMemoryDC();
		ReCreateDWriteResources();
		UpdatePosSize();
	}
	break;

	case WM_SIZE:
	{
		ECK_GET_SIZE_LPARAM(m_cxClient, m_cyClient, lParam);
		m_cxClient = std::max(m_cxClient, 1);
		m_cyClient = std::max(m_cyClient, 1);
		ReCreateMemoryDC();
	}
	return 0;
	}
	return __super::OnMsg(hWnd, uMsg, wParam, lParam);
}