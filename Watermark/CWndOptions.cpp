#include "pch.h"
#include "CWndOptions.h"
#include "GlobalOptions.h"
#include "CWndMain.h"
#include "Res/resource.h"

constexpr static struct
{
	std::wstring_view svText;
}
ItemsPos[]
{
	{ L"左上" },
	{ L"右上" },
	{ L"左下" },
	{ L"右下" },
	{ L"居中" },
};

constexpr static struct
{
	std::wstring_view svText;
}
ItemsTheme[]
{
	{ L"浅色" },
	{ L"深色" },
	{ L"通用" },
	{ L"自动" }
};

enum
{
	BtnWidth = 100,
	BtnHeight = 30,
	BottomLineHeight = BtnHeight,
	BtnIcon = 22,
};

void CWndOptions::UpdateDpi()
{
	DeleteObject(m_hFont);
	m_hFont = eck::CreateDefFont(m_iDpi);

	m_CPKColor.SetItemHeight(0, eck::DpiScale(20, m_iDpi));
	m_CPKColor.SetItemHeight(eck::DpiScale(24, m_iDpi));

	const auto cxy = eck::DpiScale(BtnIcon, m_iDpi);
	const auto hi = (HICON)LoadImageW(NtCurrentImageBaseHInst(),
		MAKEINTRESOURCEW(IDI_ICON1), IMAGE_ICON, cxy, cxy, LR_DEFAULTSIZE);
	m_BTGitHub.SetImage(hi, IMAGE_ICON);
	DestroyIcon(m_hiGitHub);
	m_hiGitHub = hi;
}

void CWndOptions::OnCreate(HWND hWnd)
{
	TrayAdd(0, LoadIconW(nullptr, IDI_APPLICATION), L"QksWatermark\r\n双击显示选项，右键退出");
	
	m_iDpi = eck::GetDpi(hWnd);

	const auto xMar = eck::DpiScale(2, m_iDpi);
	const auto yMar = eck::DpiScale(4, m_iDpi);
	const MARGINS Mar{ xMar, yMar, xMar, yMar };

	// 7行4列
	m_Layout.SetRowCol(10, 4);
	m_Layout.SetCellMode(eck::TlCellMode::Fixed);

	constexpr DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_TABSTOP;
	constexpr DWORD dwStyleStatic = WS_CHILD | WS_VISIBLE;

	int iLine{};

	m_CBUia.Create(L"UIAccess", dwStyle | BS_AUTOCHECKBOX, 0,
		0, 0, 0, 0, hWnd, 0);
	m_Layout.Add(&m_CBUia, iLine, 0, Mar, eck::LF_FILL, 0, 1);

	m_CBAutoRun.Create(L"开机自启", dwStyle | BS_AUTOCHECKBOX, 0,
		0, 0, 0, 0, hWnd, 0);
	m_Layout.Add(&m_CBAutoRun, iLine, 2, Mar, eck::LF_FILL, 0, 1);

	++iLine;

	m_LAPos.Create(L"显示位置：", dwStyleStatic, 0,
		0, 0, 0, 0, hWnd, 0);
	m_LAPos.SetAlign(FALSE, eck::Align::Center);
	m_Layout.Add(&m_LAPos, iLine, 0, Mar, eck::LF_FILL);

	m_CCBPos.Create(nullptr, dwStyle, 0,
		0, 0, 0, 0, hWnd, 0);
	m_CCBPos.GetListBox().SetItemCount(ARRAYSIZE(ItemsPos));
	m_Layout.Add(&m_CCBPos, iLine, 1, Mar, eck::LF_FILL);

	m_LAPadding.Create(L"行间距：", dwStyleStatic, 0,
		0, 0, 0, 0, hWnd, 0);
	m_LAPadding.SetAlign(FALSE, eck::Align::Center);
	m_Layout.Add(&m_LAPadding, iLine, 2, Mar, eck::LF_FILL);
	m_EDPadding.Create(0, dwStyle, WS_EX_CLIENTEDGE,
		0, 0, 0, 0, hWnd, 0);
	m_EDPadding.SetInputMode(eck::CEditExt::InputMode::Int);
	m_Layout.Add(&m_EDPadding, iLine, 3, Mar, eck::LF_FILL);

	++iLine;

	m_LADx.Create(L"水平边距：", dwStyleStatic, 0,
		0, 0, 0, 0, hWnd, 0);
	m_LADx.SetAlign(FALSE, eck::Align::Center);
	m_Layout.Add(&m_LADx, iLine, 0, Mar, eck::LF_FILL);
	m_EDDx.Create(0, dwStyle, WS_EX_CLIENTEDGE,
		0, 0, 0, 0, hWnd, 0);
	m_EDDx.SetInputMode(eck::CEditExt::InputMode::Int);
	m_Layout.Add(&m_EDDx, iLine, 1, Mar, eck::LF_FILL);

	m_LADy.Create(L"垂直边距：", dwStyleStatic, 0,
		0, 0, 0, 0, hWnd, 0);
	m_LADy.SetAlign(FALSE, eck::Align::Center);
	m_Layout.Add(&m_LADy, iLine, 2, Mar, eck::LF_FILL);
	m_EDDy.Create(0, dwStyle, WS_EX_CLIENTEDGE,
		0, 0, 0, 0, hWnd, 0);
	m_EDDy.SetInputMode(eck::CEditExt::InputMode::Int);
	m_Layout.Add(&m_EDDy, iLine, 3, Mar, eck::LF_FILL);

	++iLine;

	m_LATheme.Create(L"主题：", dwStyleStatic, 0,
		0, 0, 0, 0, hWnd, 0);
	m_LATheme.SetAlign(FALSE, eck::Align::Center);
	m_Layout.Add(&m_LATheme, iLine, 0, Mar, eck::LF_FILL);
	m_CCBTheme.Create(nullptr, dwStyle, 0,
		0, 0, 0, 0, hWnd, 0);
	m_CCBTheme.GetListBox().SetItemCount(ARRAYSIZE(ItemsTheme));
	m_Layout.Add(&m_CCBTheme, iLine, 1, Mar, eck::LF_FILL);

	++iLine;

	m_LAColor.Create(L"文本颜色：", dwStyleStatic, 0,
		0, 0, 0, 0, hWnd, 0);
	m_LAColor.SetAlign(FALSE, eck::Align::Center);
	m_Layout.Add(&m_LAColor, iLine, 0, Mar, eck::LF_FILL);

	m_CPKColor.Create(nullptr, dwStyle, 0,
		0, 0, 0, 0, hWnd, 0);
	m_Layout.Add(&m_CPKColor, iLine, 1, Mar, eck::LF_FILL);

	m_EDColorAlpha.Create(0, dwStyle, WS_EX_CLIENTEDGE,
		0, 0, 0, 0, hWnd, 0);
	m_EDColorAlpha.SetInputMode(eck::CEditExt::InputMode::Byte);
	m_Layout.Add(&m_EDColorAlpha, iLine, 2, Mar, eck::LF_FILL);

	++iLine;

	m_LAFont1.Create(L"首行字体：", dwStyleStatic, 0,
		0, 0, 0, 0, hWnd, 0);
	m_LAFont1.SetAlign(FALSE, eck::Align::Center);
	m_Layout.Add(&m_LAFont1, iLine, 0, Mar, eck::LF_FILL);
	m_FPKFont1.Create(nullptr, dwStyle, WS_EX_CLIENTEDGE,
		0, 0, 0, 0, hWnd, 0);
	m_Layout.Add(&m_FPKFont1, iLine, 1, Mar, eck::LF_FILL, 0, 2);

	++iLine;

	m_LAFont2.Create(L"次行字体：", dwStyleStatic, 0,
		0, 0, 0, 0, hWnd, 0);
	m_LAFont2.SetAlign(FALSE, eck::Align::Center);
	m_Layout.Add(&m_LAFont2, iLine, 0, Mar, eck::LF_FILL);
	m_FPKFont2.Create(nullptr, dwStyle, WS_EX_CLIENTEDGE,
		0, 0, 0, 0, hWnd, 0);
	m_Layout.Add(&m_FPKFont2, iLine, 1, Mar, eck::LF_FILL, 0, 2);

	++iLine;

	m_LALine1.Create(L"首行文本：", dwStyleStatic, 0,
		0, 0, 0, 0, hWnd, 0);
	m_LALine1.SetAlign(FALSE, eck::Align::Center);
	m_Layout.Add(&m_LALine1, iLine, 0, Mar, eck::LF_FILL);
	m_EDLine1.Create(nullptr, dwStyle, WS_EX_CLIENTEDGE,
		0, 0, 0, 0, hWnd, 0);
	m_Layout.Add(&m_EDLine1, iLine, 1, Mar, eck::LF_FILL, 0, 2);

	++iLine;

	m_LALine2.Create(L"次行文本：", dwStyleStatic, 0,
		0, 0, 0, 0, hWnd, 0);
	m_LALine2.SetAlign(FALSE, eck::Align::Center);
	m_Layout.Add(&m_LALine2, iLine, 0, Mar, eck::LF_FILL);
	m_EDLine2.Create(nullptr, dwStyle, WS_EX_CLIENTEDGE,
		0, 0, 0, 0, hWnd, 0);
	m_Layout.Add(&m_EDLine2, iLine, 1, Mar, eck::LF_FILL, 0, 2);

	++iLine;

	const MARGINS MarBtn{ xMar, 0, xMar, yMar };


	const auto cxBtn = eck::DpiScale(BtnWidth, m_iDpi);
	const auto cyBtn = eck::DpiScale(BtnHeight, m_iDpi);
	m_BTGitHub.Create(L"饭桶中心", dwStyle | BS_PUSHBUTTON, 0,
		0, 0, cxBtn, cyBtn, hWnd, 0);
	m_LytBtn.Add(&m_BTGitHub, MarBtn, eck::LF_FIX);
	m_BTReLoad.Create(L"重新加载", dwStyle | BS_PUSHBUTTON, 0,
		0, 0, cxBtn, cyBtn, hWnd, 0);
	m_LytBtn.Add(&m_BTReLoad, MarBtn, eck::LF_FIX);
	m_BTSave.Create(L"保存", dwStyle | BS_PUSHBUTTON, 0,
		0, 0, cxBtn, cyBtn, hWnd, 0);
	m_LytBtn.Add(&m_BTSave, MarBtn, eck::LF_FIX);
	m_Layout.Add(&m_LytBtn, iLine, 0, Mar, eck::LF_FIX | eck::LF_ALIGN_R, 0, 3);

	m_Layout.LoInitDpi(m_iDpi);

	UpdateDpi();
	eck::SetFontForWndAndCtrl(hWnd, m_hFont);

	OptToUI();
}

void CWndOptions::OnSize(LPARAM lParam)
{
	int cxClient, cyClient;
	ECK_GET_SIZE_LPARAM(cxClient, cyClient, lParam);

	const auto dMar = eck::DpiScale(8, m_iDpi);
	const auto cxLabel = eck::DpiScale(70, m_iDpi);
	const auto cyLine = eck::DpiScale(30, m_iDpi);
	const auto cxContent = (cxClient - cxLabel * 2 - dMar * 2) / 2;

	m_Layout.SetRowHeight(cyLine);
	m_Layout.SetRowHeight(9, eck::DpiScale(BottomLineHeight, m_iDpi));

	m_Layout.SetColWidth(0, cxLabel);
	m_Layout.SetColWidth(1, cxContent);
	m_Layout.SetColWidth(2, cxLabel);
	m_Layout.SetColWidth(3, cxContent);

	m_Layout.CommitTableMetrics();
	m_Layout.Arrange(dMar, dMar, cxClient - dMar * 2, cyClient - dMar * 2);
}

void CWndOptions::ColorOptToUI()
{
	WCHAR szBuf[eck::CchI32ToStrBufNoRadix2];
	const auto argb = g_Options.GetCurrColor();
	BYTE byAlpha;
	m_CPKColor.SetColor(eck::ARGBToColorref(argb, &byAlpha));
	swprintf(szBuf, L"%d", (int)byAlpha);
	m_EDColorAlpha.SetText(szBuf);
}

void CWndOptions::UIToColorOpt()
{
	WCHAR szBuf[eck::CchI32ToStrBufNoRadix2];
	szBuf[0] = L'\0';
	m_EDColorAlpha.GetText(szBuf, ARRAYSIZE(szBuf));
	const auto byAlpha = (BYTE)wcstoul(szBuf, nullptr, 10);
	g_Options.SetCurrColor(eck::ColorrefToARGB(m_CPKColor.GetColor(), byAlpha));
}

void CWndOptions::OptToUI()
{
	WCHAR szBuf[eck::CchI32ToStrBufNoRadix2];
	eck::FONTPICKERINFO fpki;
	m_CBUia.SetCheckState(!!g_Options.bUia);
	m_CBAutoRun.SetCheckState(!!g_Options.bAutoRun);
	m_CCBPos.GetListBox().SetCurrSel((int)g_Options.ePos);
	swprintf(szBuf, L"%d", g_Options.cyPadding);
	m_EDPadding.SetText(szBuf);
	swprintf(szBuf, L"%d", g_Options.dx);
	m_EDDx.SetText(szBuf);
	swprintf(szBuf, L"%d", g_Options.dy);
	m_EDDy.SetText(szBuf);
	m_CCBTheme.GetListBox().SetCurrSel((int)g_Options.eTheme);
	fpki.pszFontName = g_Options.Font1.Data();
	fpki.iPointSize = g_Options.iPoint1;
	fpki.iWeight = g_Options.iWeight1;
	m_FPKFont1.FromInfo(fpki);
	fpki.pszFontName = g_Options.Font2.Data();
	fpki.iPointSize = g_Options.iPoint2;
	fpki.iWeight = g_Options.iWeight2;
	m_FPKFont2.FromInfo(fpki);
	m_EDLine1.SetText(g_Options.rsLine1.Data());
	m_EDLine2.SetText(g_Options.rsLine2.Data());
	ColorOptToUI();
}

void CWndOptions::UIToOpt()
{
	g_Options.bUia =!!m_CBUia.GetCheckState();
	g_Options.bAutoRun =!!m_CBAutoRun.GetCheckState();
	g_Options.ePos = (PosType)m_CCBPos.GetListBox().GetCurrSel();
	WCHAR szBuf[eck::CchI32ToStrBufNoRadix2];
	m_EDPadding.GetText(szBuf, ARRAYSIZE(szBuf));
	g_Options.cyPadding = (int)_wtoi(szBuf);
	m_EDDx.GetText(szBuf, ARRAYSIZE(szBuf));
	g_Options.dx = (int)_wtoi(szBuf);
	m_EDDy.GetText(szBuf, ARRAYSIZE(szBuf));
	g_Options.dy = (int)_wtoi(szBuf);
	g_Options.eTheme = (ThemeType)m_CCBTheme.GetListBox().GetCurrSel();
	eck::FONTPICKERINFO fpki;
	g_Options.Font1 = m_FPKFont1.ToInfo(fpki);
	g_Options.iPoint1 = fpki.iPointSize;
	g_Options.iWeight1 = fpki.iWeight;
	g_Options.Font2 = m_FPKFont2.ToInfo(fpki);
	g_Options.iPoint2 = fpki.iPointSize;
	g_Options.iWeight2 = fpki.iWeight;
	g_Options.rsLine1 = m_EDLine1.GetText();
	g_Options.rsLine2 = m_EDLine2.GetText();
	UIToColorOpt();

	g_Options.ToIni();
}

LRESULT CWndOptions::OnMsg(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_NOTIFY:
	{
		const auto* const pnmhdr = (NMHDR*)lParam;
		if (pnmhdr->hwndFrom == m_CCBPos.HWnd)
			switch (pnmhdr->code)
			{
			case eck::NM_LBN_GETDISPINFO:
			{
				const auto p = (eck::NMLBNGETDISPINFO*)lParam;
				p->Item.pszText = (PWSTR)ItemsPos[p->Item.idxItem].svText.data();
				p->Item.cchText = (int)ItemsPos[p->Item.idxItem].svText.size();
			}
			return TRUE;
			}
		else if (pnmhdr->hwndFrom == m_CCBTheme.HWnd)
			switch (pnmhdr->code)
			{
			case eck::NM_LBN_GETDISPINFO:
			{
				const auto p = (eck::NMLBNGETDISPINFO*)lParam;
				p->Item.pszText = (PWSTR)ItemsTheme[p->Item.idxItem].svText.data();
				p->Item.cchText = (int)ItemsTheme[p->Item.idxItem].svText.size();
			}
			return TRUE;
			}
	}
	break;
	case WM_COMMAND:
	{
		if (HIWORD(wParam) == BN_CLICKED)
			if (lParam == (LPARAM)m_BTGitHub.HWnd)
			{
				ShellExecuteW(nullptr, nullptr, 
					L"https://github.com/QingKong-s/Watermark", 
					nullptr, nullptr, SW_SHOWNORMAL);
				return 0;
			}
			else if (lParam == (LPARAM)m_BTReLoad.HWnd)
			{
				OptToUI();
				return 0;
			}
			else if (lParam == (LPARAM)m_BTSave.HWnd)
			{
				UIToOpt();
				m_pWndMain->UpdateFont();
				return 0;
			}
			else if (lParam == (LPARAM)m_CBAutoRun.HWnd)
			{
				eck::SetAutoRun(L"qk_s_watermark_cxx", m_CBAutoRun.GetCheckState());
				return 0;
			}
	}
	break;

	case WM_CLOSE:
		Show(SW_HIDE);
	return 0;

	case WM_SIZE:
		OnSize(lParam);
		break;
	case WM_DPICHANGED:
	{
		m_iDpi = LOWORD(wParam);
		m_Layout.LoOnDpiChanged(m_iDpi);
		eck::MsgOnDpiChanged(hWnd, lParam);
	}
	break;
	case WM_CREATE:
		OnCreate(hWnd);
		break;
	case WM_DESTROY:
	{
		DeleteObject(m_hFont);
		m_hFont = nullptr;
		DestroyIcon(m_hiGitHub);
		m_hiGitHub = nullptr;
	}
	break;
	}
	return __super::OnMsg(hWnd, uMsg, wParam, lParam);
}

BOOL CWndOptions::PreTranslateMessage(const MSG& Msg)
{
	if (IsDialogMessageW(HWnd, (MSG*)&Msg))
		return TRUE;
	return __super::PreTranslateMessage(Msg);
}

void CWndOptions::OnTrayNotify(UINT uMsg, UINT uID, int x, int y)
{
	if (uMsg == WM_LBUTTONDBLCLK)
		Show(SW_SHOW);
	else if (uMsg == WM_RBUTTONUP)
		if (MessageBoxW(HWnd, L"确定要退出吗？", L"退出", MB_YESNO | MB_ICONQUESTION) == IDYES)
		{
			m_pWndMain->Destroy();
			Destroy();
			PostQuitMessage(0);
		}
}