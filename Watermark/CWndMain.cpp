#include "pch.h"
#include "CWndMain.h"
#include "GlobalOptions.h"

void CWndMain::ClearRes()
{
	m_DC.Destroy();
	if (m_pGraphics)
	{
		GdipDeleteGraphics(m_pGraphics);
		m_pGraphics = nullptr;
	}
	if (m_pFont1)
	{
		GdipDeleteFont(m_pFont1);
		m_pFont1 = nullptr;
	}
	if (m_pFont2)
	{
		GdipDeleteFont(m_pFont2);
		m_pFont2 = nullptr;
	}
	if (m_pStrFmt)
	{
		GdipDeleteStringFormat(m_pStrFmt);
		m_pStrFmt = nullptr;
	}
}

LRESULT CWndMain::OnCreate(HWND hWnd, CREATESTRUCT* pcs)
{
	BOOL b{ 1 };
	DwmSetWindowAttribute(hWnd, DWMWA_EXCLUDED_FROM_PEEK, &b, sizeof(b));
	eck::GetThreadCtx()->UpdateDefColor();
	m_iDpi = eck::GetDpi(hWnd);

	RECT rcClient;
	GetClientRect(hWnd, &rcClient);
	m_cxClient = rcClient.right;
	m_cyClient = rcClient.bottom;
	m_DC.Create32(hWnd, m_cxClient, m_cyClient);
	UpdateGraphics();

	GdipStringFormatGetGenericDefault(&m_pStrFmt);
	GdipSetStringFormatLineAlign(m_pStrFmt, Gdiplus::StringAlignmentCenter);
	Paint();
	return 0;
}

void CWndMain::UpdateGraphics()
{
	if (m_pGraphics)
		GdipDeleteGraphics(m_pGraphics);
	GdipCreateFromHDC(m_DC.GetDC(), &m_pGraphics);
	GdipSetTextRenderingHint(m_pGraphics, Gdiplus::TextRenderingHintAntiAliasGridFit);
	GdipSetSmoothingMode(m_pGraphics, Gdiplus::SmoothingModeAntiAlias);
	GdipGraphicsClear(m_pGraphics, 0);
}

void CWndMain::Paint()
{
	GdipGraphicsClear(m_pGraphics, 0);

	GpSolidFill* pSolidFill;
	GdipCreateSolidFill(g_Options.GetCurrColor(), &pSolidFill);

	GpRectF rcText{ 0.f,0.f, (float)m_cxClient, m_cyLine1 };
	GdipDrawString(m_pGraphics, g_Options.rsLine1.Data(), g_Options.rsLine1.Size(),
		m_pFont1, &rcText, m_pStrFmt, pSolidFill);

	rcText.Y += (m_cyLine1 + m_cyPadding);
	rcText.Height = m_cyLine2;
	GdipDrawString(m_pGraphics, g_Options.rsLine2.Data(), g_Options.rsLine2.Size(),
		m_pFont2, &rcText, m_pStrFmt, pSolidFill);

	GdipDeleteBrush(pSolidFill);
}

void CWndMain::UpdateLayered()
{
	SIZE size{ m_cxClient, m_cyClient };
	POINT pt{};
	BLENDFUNCTION bf{ AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
	UpdateLayeredWindow(HWnd, nullptr, nullptr, &size,
		m_DC.GetDC(), &pt, 0, &bf, ULW_ALPHA);
}

void CWndMain::CalcPos(int cx, int cy, int& x, int& y)
{
	const auto hMon = MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST);
	MONITORINFO mi;
	mi.cbSize = sizeof(mi);
	GetMonitorInfoW(hMon, &mi);
	const auto dx = eck::DpiScale(g_Options.dx, m_iDpi);
	const auto dy = eck::DpiScale(g_Options.dy, m_iDpi);
	switch (g_Options.ePos)
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

LRESULT CWndMain::OnMsg(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_SIZE:
	{
		ECK_GET_SIZE_LPARAM(m_cxClient, m_cyClient, lParam);
		m_DC.ReSize32(hWnd, m_cxClient, m_cyClient);
		UpdateGraphics();
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
		if (eck::MsgOnSettingChangeMainWnd(hWnd, wParam, lParam))
		{
			Paint();
			UpdateLayered();
		}
		break;
	case WM_DPICHANGED:
	{
		m_iDpi = HIWORD(wParam);
		eck::MsgOnDpiChanged(hWnd, lParam);
		UpdateFont();
	}
	break;
	}
	return __super::OnMsg(hWnd, uMsg, wParam, lParam);
}

void CWndMain::UpdateFont()
{
	if (m_pFont1)
	{
		GdipDeleteFont(m_pFont1);
		m_pFont1 = nullptr;
	}
	if (m_pFont2)
	{
		GdipDeleteFont(m_pFont2);
		m_pFont2 = nullptr;
	}

	const float TextHeight1 = g_Options.iPoint1 * 96.f / 72.f;
	const float TextHeight2 = g_Options.iPoint2 * 96.f / 72.f;
	m_cyLine1 = eck::DpiScaleF(TextHeight1, m_iDpi);
	m_cyLine2 = eck::DpiScaleF(TextHeight2, m_iDpi);
	m_cyPadding = eck::DpiScaleF((float)g_Options.cyPadding, m_iDpi);

	GpFontFamily* pFontFamily1, * pFontFamily2;
	GdipCreateFontFamilyFromName(g_Options.Font1.Data(), nullptr, &pFontFamily1);
	GdipCreateFont(pFontFamily1, (float)m_cyLine1,
		Gdiplus::FontStyleRegular, Gdiplus::UnitPixel, &m_pFont1);
	GdipDeleteFontFamily(pFontFamily1);

	GdipCreateFontFamilyFromName(g_Options.Font2.Data(), nullptr, &pFontFamily2);
	GdipCreateFont(pFontFamily2, (float)m_cyLine2,
		Gdiplus::FontStyleRegular, Gdiplus::UnitPixel, &m_pFont2);
	GdipDeleteFontFamily(pFontFamily2);
	UpdatePosSize();
}

void CWndMain::UpdatePos()
{
	int x, y;
	CalcPos(m_cxClient, m_cyClient, x, y);
	SetWindowPos(HWnd, nullptr, x, y, 0, 0,
		SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}

void CWndMain::UpdatePosSize()
{
	float cxF, cyF;
	GpRectF rcLayout{ 0.f,0.f, INFINITY, INFINITY };
	GpRectF rcText;
	GdipMeasureString(m_pGraphics, g_Options.rsLine1.Data(), g_Options.rsLine1.Size(),
		m_pFont1, &rcLayout, m_pStrFmt, &rcText, nullptr, nullptr);
	cxF = rcText.Width;
	cyF = rcText.Height;
	m_cyLine1 = cyF;
	GdipMeasureString(m_pGraphics, g_Options.rsLine2.Data(), g_Options.rsLine2.Size(),
		m_pFont2, &rcLayout, nullptr, &rcText, nullptr, nullptr);
	cxF = std::max(cxF, rcText.Width);
	cyF += (m_cyPadding + rcText.Height);
	m_cyLine2 = rcText.Height;
	const auto cx = int(ceilf(cxF) + eck::DpiScaleF(1.f, m_iDpi));
	const auto cy = (int)ceilf(cyF);

	int x, y;
	CalcPos(cx, cy, x, y);

	SetWindowPos(HWnd, nullptr, x, y, std::max(cx, 1), std::max(cy, 1),
		SWP_NOZORDER | SWP_NOACTIVATE);
	Paint();
	UpdateLayered();
}
