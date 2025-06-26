#include "pch.h"
#include "CWndOptions.h"
#include "CApp.h"
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
	const auto hFont = eck::CreateDefFont(m_iDpi);
	eck::SetFontForWndAndCtrl(HWnd, hFont);
	DeleteObject(m_hFont);
	m_hFont = hFont;

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
	m_LytComm.SetRowCol(2, 2);
	m_LytComm.SetCellMode(eck::TlCellMode::Fixed);
	m_LytMain.SetRowCol(10, 4);
	m_LytMain.SetCellMode(eck::TlCellMode::Fixed);
	m_LytDt.SetRowCol(10, 4);
	m_LytDt.SetCellMode(eck::TlCellMode::Fixed);

	UpdateTableLayoutRowHeight();

	constexpr DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_TABSTOP;
	constexpr DWORD dwStyleStatic = WS_CHILD | WS_VISIBLE;

	m_Tab.Create(nullptr, WS_CHILD | WS_VISIBLE, 0,
		0, 0, 0, 400, hWnd, 0);
	m_Tab.InsertItem(L"激活水印", 0);
	m_Tab.InsertItem(L"桌面水印", 1);

	int iLine{};
	// 顶部
	{
		m_CBUia.Create(L"UIAccess", dwStyle | BS_AUTOCHECKBOX, 0,
			0, 0, 0, 0, hWnd, 0);
		m_LytComm.Add(&m_CBUia, iLine, 0, Mar, eck::LF_FILL, 0, 0);

		m_CBAutoRun.Create(L"开机自启", dwStyle | BS_AUTOCHECKBOX, 0,
			0, 0, 0, 0, hWnd, 0);
		m_LytComm.Add(&m_CBAutoRun, iLine, 1, Mar, eck::LF_FILL, 0, 0);

		++iLine;

		m_CBExcludeFromSnapshot.Create(L"防止被截图", dwStyle | BS_AUTOCHECKBOX, 0,
			0, 0, 0, 0, hWnd, 0);
		m_LytComm.Add(&m_CBExcludeFromSnapshot, 1, 0, Mar, eck::LF_FILL, 0, 0);
		iLine = 0;

		m_LytComm.CommitTableMetrics();
	}
	m_Layout.Add(&m_LytComm, {}, eck::LF_FILL_WIDTH | eck::LF_FIX_HEIGHT);
	// 第一页
	{
		m_LAPos.Create(L"显示位置：", dwStyleStatic, 0,
			0, 0, 0, 0, hWnd, 0);
		m_LAPos.SetAlign(FALSE, eck::Align::Center);
		m_LAPos.SetTransparent(TRUE);
		m_LytMain.Add(&m_LAPos, iLine, 0, Mar, eck::LF_FILL);

		m_CCBPos.Create(nullptr, dwStyle, 0,
			0, 0, 0, 0, hWnd, 0);
		m_CCBPos.GetListBox().SetItemCount(ARRAYSIZE(ItemsPos));
		m_LytMain.Add(&m_CCBPos, iLine, 1, Mar, eck::LF_FILL);

		m_CBColorFont.Create(L"启用彩色字体", dwStyle | BS_AUTOCHECKBOX, 0,
			0, 0, 0, 0, hWnd, 0);
		m_LytMain.Add(&m_CBColorFont, iLine, 2, Mar, eck::LF_FILL, 0, 1);

		++iLine;

		m_LADx.Create(L"水平边距：", dwStyleStatic, 0,
			0, 0, 0, 0, hWnd, 0);
		m_LADx.SetAlign(FALSE, eck::Align::Center);
		m_LADx.SetTransparent(TRUE);
		m_LytMain.Add(&m_LADx, iLine, 0, Mar, eck::LF_FILL);

		m_EDDx.Create(0, dwStyle, WS_EX_CLIENTEDGE,
			0, 0, 0, 0, hWnd, 0);
		m_EDDx.SetInputMode(eck::CEditExt::InputMode::Int);
		m_EDDx.GetSignal().Connect(this, &CWndOptions::EditArrowCtrl);
		m_LytMain.Add(&m_EDDx, iLine, 1, Mar, eck::LF_FILL);

		m_LADy.Create(L"垂直边距：", dwStyleStatic, 0,
			0, 0, 0, 0, hWnd, 0);
		m_LADy.SetAlign(FALSE, eck::Align::Center);
		m_LADy.SetTransparent(TRUE);
		m_LytMain.Add(&m_LADy, iLine, 2, Mar, eck::LF_FILL);

		m_EDDy.Create(0, dwStyle, WS_EX_CLIENTEDGE,
			0, 0, 0, 0, hWnd, 0);
		m_EDDy.SetInputMode(eck::CEditExt::InputMode::Int);
		m_EDDy.GetSignal().Connect(this, &CWndOptions::EditArrowCtrl);
		m_LytMain.Add(&m_EDDy, iLine, 3, Mar, eck::LF_FILL);

		++iLine;

		m_LATheme.Create(L"主题：", dwStyleStatic, 0,
			0, 0, 0, 0, hWnd, 0);
		m_LATheme.SetAlign(FALSE, eck::Align::Center);
		m_LATheme.SetTransparent(TRUE);
		m_LytMain.Add(&m_LATheme, iLine, 0, Mar, eck::LF_FILL);

		m_CCBTheme.Create(nullptr, dwStyle, 0,
			0, 0, 0, 0, hWnd, 0);
		m_CCBTheme.GetListBox().SetItemCount(ARRAYSIZE(ItemsTheme));
		m_LytMain.Add(&m_CCBTheme, iLine, 1, Mar, eck::LF_FILL);

		m_LAPadding.Create(L"行间距：", dwStyleStatic, 0,
			0, 0, 0, 0, hWnd, 0);
		m_LAPadding.SetAlign(FALSE, eck::Align::Center);
		m_LAPadding.SetTransparent(TRUE);
		m_LytMain.Add(&m_LAPadding, iLine, 2, Mar, eck::LF_FILL);

		m_EDPadding.Create(0, dwStyle, WS_EX_CLIENTEDGE,
			0, 0, 0, 0, hWnd, 0);
		m_EDPadding.SetInputMode(eck::CEditExt::InputMode::Int);
		m_EDPadding.GetSignal().Connect(this, &CWndOptions::EditArrowCtrl);
		m_LytMain.Add(&m_EDPadding, iLine, 3, Mar, eck::LF_FILL);

		++iLine;

		m_LAColor.Create(L"文本颜色：", dwStyleStatic, 0,
			0, 0, 0, 0, hWnd, 0);
		m_LAColor.SetAlign(FALSE, eck::Align::Center);
		m_LAColor.SetTransparent(TRUE);
		m_LytMain.Add(&m_LAColor, iLine, 0, Mar, eck::LF_FILL);

		m_CPKColor.Create(nullptr, dwStyle, 0,
			0, 0, 0, 0, hWnd, 0);
		m_LytMain.Add(&m_CPKColor, iLine, 1, Mar, eck::LF_FILL);

		m_EDColorAlpha.Create(0, dwStyle, WS_EX_CLIENTEDGE,
			0, 0, 0, 0, hWnd, 0);
		m_EDColorAlpha.SetInputMode(eck::CEditExt::InputMode::Byte);
		m_EDColorAlpha.GetSignal().Connect(this, &CWndOptions::EditArrowCtrl);
		m_LytMain.Add(&m_EDColorAlpha, iLine, 2, Mar, eck::LF_FILL);

		++iLine;

		m_LAFont1.Create(L"首行字体：", dwStyleStatic, 0,
			0, 0, 0, 0, hWnd, 0);
		m_LAFont1.SetAlign(FALSE, eck::Align::Center);
		m_LAFont1.SetTransparent(TRUE);
		m_LytMain.Add(&m_LAFont1, iLine, 0, Mar, eck::LF_FILL);

		m_FPKFont1.Create(nullptr, dwStyle, WS_EX_CLIENTEDGE,
			0, 0, 0, 0, hWnd, 0);
		m_LytMain.Add(&m_FPKFont1, iLine, 1, Mar, eck::LF_FILL, 0, 2);

		++iLine;

		m_LAFont2.Create(L"次行字体：", dwStyleStatic, 0,
			0, 0, 0, 0, hWnd, 0);
		m_LAFont2.SetAlign(FALSE, eck::Align::Center);
		m_LAFont2.SetTransparent(TRUE);
		m_LytMain.Add(&m_LAFont2, iLine, 0, Mar, eck::LF_FILL);

		m_FPKFont2.Create(nullptr, dwStyle, WS_EX_CLIENTEDGE,
			0, 0, 0, 0, hWnd, 0);
		m_LytMain.Add(&m_FPKFont2, iLine, 1, Mar, eck::LF_FILL, 0, 2);

		++iLine;

		m_LALine1.Create(L"首行文本：", dwStyleStatic, 0,
			0, 0, 0, 0, hWnd, 0);
		m_LALine1.SetAlign(FALSE, eck::Align::Center);
		m_LALine1.SetTransparent(TRUE);
		m_LytMain.Add(&m_LALine1, iLine, 0, Mar, eck::LF_FILL);

		m_EDLine1.Create(nullptr, dwStyle, WS_EX_CLIENTEDGE,
			0, 0, 0, 0, hWnd, 0);
		m_LytMain.Add(&m_EDLine1, iLine, 1, Mar, eck::LF_FILL, 0, 2);

		++iLine;

		m_LALine2.Create(L"次行文本：", dwStyleStatic, 0,
			0, 0, 0, 0, hWnd, 0);
		m_LALine2.SetAlign(FALSE, eck::Align::Center);
		m_LALine2.SetTransparent(TRUE);
		m_LytMain.Add(&m_LALine2, iLine, 0, Mar, eck::LF_FILL);

		m_EDLine2.Create(nullptr, dwStyle, WS_EX_CLIENTEDGE,
			0, 0, 0, 0, hWnd, 0);
		m_LytMain.Add(&m_EDLine2, iLine, 1, Mar, eck::LF_FILL, 0, 2);
		iLine = 0;
	}
	m_LytTab.Add(&m_LytMain, {}, eck::LF_FILL_WIDTH | eck::LF_FIX_HEIGHT);
	// 第二页
	{
		m_LADtPos.Create(L"显示位置：", dwStyleStatic, 0,
			0, 0, 0, 0, hWnd, 0);
		m_LADtPos.SetAlign(FALSE, eck::Align::Center);
		m_LADtPos.SetTransparent(TRUE);
		m_LytDt.Add(&m_LADtPos, iLine, 0, Mar, eck::LF_FILL);

		m_CCBDtPos.Create(nullptr, dwStyle, 0,
			0, 0, 0, 0, hWnd, 0);
		m_CCBDtPos.GetListBox().SetItemCount(ARRAYSIZE(ItemsPos));
		m_LytDt.Add(&m_CCBDtPos, iLine, 1, Mar, eck::LF_FILL);

		m_CBDtColorFont.Create(L"启用彩色字体", dwStyle | BS_AUTOCHECKBOX, 0,
			0, 0, 0, 0, hWnd, 0);
		m_LytDt.Add(&m_CBDtColorFont, iLine, 2, Mar, eck::LF_FILL, 0, 1);

		++iLine;

		m_LADtDx.Create(L"水平边距：", dwStyleStatic, 0,
			0, 0, 0, 0, hWnd, 0);
		m_LADtDx.SetAlign(FALSE, eck::Align::Center);
		m_LADtDx.SetTransparent(TRUE);
		m_LytDt.Add(&m_LADtDx, iLine, 0, Mar, eck::LF_FILL);

		m_EDDtDx.Create(0, dwStyle, WS_EX_CLIENTEDGE,
			0, 0, 0, 0, hWnd, 0);
		m_EDDtDx.SetInputMode(eck::CEditExt::InputMode::Int);
		m_EDDtDx.GetSignal().Connect(this, &CWndOptions::EditArrowCtrl);
		m_LytDt.Add(&m_EDDtDx, iLine, 1, Mar, eck::LF_FILL);

		m_LADtDy.Create(L"垂直边距：", dwStyleStatic, 0,
			0, 0, 0, 0, hWnd, 0);
		m_LADtDy.SetAlign(FALSE, eck::Align::Center);
		m_LADtDy.SetTransparent(TRUE);
		m_LytDt.Add(&m_LADtDy, iLine, 2, Mar, eck::LF_FILL);

		m_EDDtDy.Create(0, dwStyle, WS_EX_CLIENTEDGE,
			0, 0, 0, 0, hWnd, 0);
		m_EDDtDy.SetInputMode(eck::CEditExt::InputMode::Int);
		m_EDDtDy.GetSignal().Connect(this, &CWndOptions::EditArrowCtrl);
		m_LytDt.Add(&m_EDDtDy, iLine, 3, Mar, eck::LF_FILL);

		++iLine;

		m_LADtColor.Create(L"文本颜色：", dwStyleStatic, 0,
			0, 0, 0, 0, hWnd, 0);
		m_LADtColor.SetAlign(FALSE, eck::Align::Center);
		m_LADtColor.SetTransparent(TRUE);
		m_LytDt.Add(&m_LADtColor, iLine, 0, Mar, eck::LF_FILL);

		m_CPKDtColor.Create(nullptr, dwStyle, 0,
			0, 0, 0, 0, hWnd, 0);
		m_LytDt.Add(&m_CPKDtColor, iLine, 1, Mar, eck::LF_FILL);

		m_EDDtColorAlpha.Create(0, dwStyle, WS_EX_CLIENTEDGE,
			0, 0, 0, 0, hWnd, 0);
		m_EDDtColorAlpha.SetInputMode(eck::CEditExt::InputMode::Byte);
		m_EDDtColorAlpha.GetSignal().Connect(this, &CWndOptions::EditArrowCtrl);
		m_LytDt.Add(&m_EDDtColorAlpha, iLine, 2, Mar, eck::LF_FILL);

		++iLine;

		m_LADtShadowColor.Create(L"阴影颜色：", dwStyleStatic, 0,
			0, 0, 0, 0, hWnd, 0);
		m_LADtShadowColor.SetAlign(FALSE, eck::Align::Center);
		m_LADtShadowColor.SetTransparent(TRUE);
		m_LytDt.Add(&m_LADtShadowColor, iLine, 0, Mar, eck::LF_FILL);

		m_CPKDtShadowColor.Create(nullptr, dwStyle, 0,
			0, 0, 0, 0, hWnd, 0);
		m_LytDt.Add(&m_CPKDtShadowColor, iLine, 1, Mar, eck::LF_FILL);

		m_EDDtShadowColorAlpha.Create(0, dwStyle, WS_EX_CLIENTEDGE,
			0, 0, 0, 0, hWnd, 0);
		m_EDDtShadowColorAlpha.SetInputMode(eck::CEditExt::InputMode::Byte);
		m_EDDtShadowColorAlpha.GetSignal().Connect(this, &CWndOptions::EditArrowCtrl);
		m_LytDt.Add(&m_EDDtShadowColorAlpha, iLine, 2, Mar, eck::LF_FILL);

		++iLine;

		m_LADtFont.Create(L"字体：", dwStyleStatic, 0,
			0, 0, 0, 0, hWnd, 0);
		m_LADtFont.SetAlign(FALSE, eck::Align::Center);
		m_LADtFont.SetTransparent(TRUE);
		m_LytDt.Add(&m_LADtFont, iLine, 0, Mar, eck::LF_FILL);

		m_FPKDtFont.Create(nullptr, dwStyle, WS_EX_CLIENTEDGE,
			0, 0, 0, 0, hWnd, 0);
		m_LytDt.Add(&m_FPKDtFont, iLine, 1, Mar, eck::LF_FILL, 0, 2);

		++iLine;

		m_LADtShadowRadius.Create(L"阴影半径：", dwStyleStatic, 0,
			0, 0, 0, 0, hWnd, 0);
		m_LADtShadowRadius.SetAlign(FALSE, eck::Align::Center);
		m_LADtShadowRadius.SetTransparent(TRUE);
		m_LytDt.Add(&m_LADtShadowRadius, iLine, 0, Mar, eck::LF_FILL);

		m_EDDtShadowRadius.Create(0, dwStyle, WS_EX_CLIENTEDGE,
			0, 0, 0, 0, hWnd, 0);
		m_EDDtShadowRadius.SetInputMode(eck::CEditExt::InputMode::Float);
		m_LytDt.Add(&m_EDDtShadowRadius, iLine, 1, Mar, eck::LF_FILL);

		m_LADtShadowExtent.Create(L"阴影范围：", dwStyleStatic, 0,
			0, 0, 0, 0, hWnd, 0);
		m_LADtShadowExtent.SetAlign(FALSE, eck::Align::Center);
		m_LADtShadowExtent.SetTransparent(TRUE);
		m_LytDt.Add(&m_LADtShadowExtent, iLine, 2, Mar, eck::LF_FILL);

		m_EDDtShadowExtent.Create(0, dwStyle, WS_EX_CLIENTEDGE,
			0, 0, 0, 0, hWnd, 0);
		m_EDDtShadowExtent.SetInputMode(eck::CEditExt::InputMode::Float);
		m_LytDt.Add(&m_EDDtShadowExtent, iLine, 3, Mar, eck::LF_FILL);

		++iLine;

		m_LADtText.Create(L"文本：", dwStyleStatic, 0,
			0, 0, 0, 0, hWnd, 0);
		m_LADtText.SetAlign(FALSE, eck::Align::Center);
		m_LADtText.SetTransparent(TRUE);
		m_LytDt.Add(&m_LADtText, iLine, 0, Mar, eck::LF_FILL);

		m_EDDtText.SetAutoWrap(TRUE);
		m_EDDtText.SetMultiLine(TRUE);
		m_EDDtText.Create(nullptr, dwStyle | WS_VSCROLL, WS_EX_CLIENTEDGE,
			0, 0, 0, 0, hWnd, 0);
		m_EDDtText.GetSignal().Connect(this, &CWndOptions::EditMultiLineCtrl);
		m_LytDt.Add(&m_EDDtText, iLine, 1, Mar, eck::LF_FILL, 2, 2);

		iLine = 0;
	}
	m_LytTab.Add(&m_LytDt, {}, eck::LF_FILL_WIDTH | eck::LF_FIX_HEIGHT);
	m_Layout.Add(&m_Tab, {}, eck::LF_FILL, 1);
	// 底部按钮
	{
		const MARGINS MarBtn{ xMar, 0, xMar, yMar };
		const auto cxBtn = eck::DpiScale(BtnWidth, m_iDpi);
		const auto cyBtn = eck::DpiScale(BtnHeight, m_iDpi);
		m_BTGitHub.Create(L" 饭桶中心", dwStyle | BS_PUSHBUTTON, 0,
			0, 0, cxBtn, cyBtn, hWnd, 0);
		m_LytBtn.Add(&m_BTGitHub, MarBtn, eck::LF_FIX);
		m_BTReLoad.Create(L"重新加载", dwStyle | BS_PUSHBUTTON, 0,
			0, 0, cxBtn, cyBtn, hWnd, 0);
		m_LytBtn.Add(&m_BTReLoad, MarBtn, eck::LF_FIX);
		m_BTSave.Create(L"保存", dwStyle | BS_PUSHBUTTON, 0,
			0, 0, cxBtn, cyBtn, hWnd, 0);
		m_LytBtn.Add(&m_BTSave, MarBtn, eck::LF_FIX);
	}
	m_Layout.Add(&m_LytBtn, { .cyTopHeight = yMar }, eck::LF_FIX | eck::LF_ALIGN_FAR);
	m_Layout.LoInitDpi(m_iDpi);
	m_LytTab.LoInitDpi(m_iDpi);

	m_LytTab.ShowFrame(0);
	UpdateDpi();
	eck::SetFontForWndAndCtrl(hWnd, m_hFont);
	OptToUI();
}

void CWndOptions::OnSize(LPARAM lParam)
{
	int cxClient, cyClient;
	ECK_GET_SIZE_LPARAM(cxClient, cyClient, lParam);

	const auto dMar = eck::DpiScale(8, m_iDpi);
	const auto dMarMin = eck::DpiScale(1, m_iDpi);

	m_LytComm.SetColWidth(0, cxClient / 2 - dMar);
	m_LytComm.SetColWidth(1, cxClient / 2 - dMar);
	m_LytComm.CommitTableMetrics();
	m_Layout.Arrange(dMar, dMar, cxClient - dMar * 2, cyClient - dMar * 2);

	RECT rc;
	GetClientRect(m_Tab.HWnd, &rc);
	MapWindowRect(m_Tab.HWnd, HWnd, &rc);
	m_Tab.AdjustRect(&rc, FALSE);
	const auto cx = rc.right - rc.left;
	const auto cy = rc.bottom - rc.top;
	const auto cxLabel = eck::DpiScale(70, m_iDpi);
	const auto cxContent = (cx - cxLabel * 2 - dMarMin * 2) / 2;

	m_LytMain.SetColWidth(0, cxLabel);
	m_LytMain.SetColWidth(1, cxContent);
	m_LytMain.SetColWidth(2, cxLabel);
	m_LytMain.SetColWidth(3, cxContent);
	m_LytMain.CommitTableMetrics();

	m_LytDt.SetColWidth(0, cxLabel);
	m_LytDt.SetColWidth(1, cxContent);
	m_LytDt.SetColWidth(2, cxLabel);
	m_LytDt.SetColWidth(3, cxContent);
	m_LytDt.CommitTableMetrics();

	m_LytTab.Arrange(rc.left + dMarMin, rc.top + dMarMin,
		cx - dMarMin * 2, cy - dMarMin * 2);
}

void CWndOptions::ColorOptToUI()
{
	WCHAR szBuf[eck::CchI32ToStrBufNoRadix2];
	const auto argb = App.GetOpt().GetCurrColor();
	BYTE byAlpha;
	m_CPKColor.SetColor(eck::ArgbToColorref(argb, &byAlpha));
	m_CPKColor.Redraw();
	swprintf(szBuf, L"%d", (int)byAlpha);
	m_EDColorAlpha.SetText(szBuf);
}

void CWndOptions::UIToColorOpt()
{
	WCHAR szBuf[eck::CchI32ToStrBufNoRadix2];
	szBuf[0] = L'\0';
	m_EDColorAlpha.GetText(szBuf, ARRAYSIZE(szBuf));
	const auto byAlpha = (BYTE)wcstoul(szBuf, nullptr, 10);
	App.GetOpt().SetCurrColor(eck::ColorrefToArgb(m_CPKColor.GetColor(), byAlpha));
}

void CWndOptions::DtTextColorOptToUI()
{
	WCHAR szBuf[eck::CchI32ToStrBufNoRadix2];
	BYTE byAlpha;
	m_CPKDtColor.SetColor(eck::ArgbToColorref(App.GetOpt().crDtText, &byAlpha));
	m_CPKDtColor.Redraw();
	swprintf(szBuf, L"%d", (int)byAlpha);
	m_EDDtColorAlpha.SetText(szBuf);
}

void CWndOptions::DtUIToTextColorOpt()
{
	WCHAR szBuf[eck::CchI32ToStrBufNoRadix2];
	szBuf[0] = L'\0';
	m_EDDtColorAlpha.GetText(szBuf, ARRAYSIZE(szBuf));
	const auto byAlpha = (BYTE)wcstoul(szBuf, nullptr, 10);
	App.GetOpt().crDtText = eck::ColorrefToArgb(m_CPKDtColor.GetColor(), byAlpha);
}

void CWndOptions::DtShadowColorOptToUI()
{
	WCHAR szBuf[eck::CchI32ToStrBufNoRadix2];
	BYTE byAlpha;
	m_CPKDtShadowColor.SetColor(eck::ArgbToColorref(App.GetOpt().crDtShadow, &byAlpha));
	m_CPKDtShadowColor.Redraw();
	swprintf(szBuf, L"%d", (int)byAlpha);
	m_EDDtShadowColorAlpha.SetText(szBuf);
}

void CWndOptions::DtUIToShadowColorOpt()
{
	WCHAR szBuf[eck::CchI32ToStrBufNoRadix2];
	szBuf[0] = L'\0';
	m_EDDtShadowColorAlpha.GetText(szBuf, ARRAYSIZE(szBuf));
	const auto byAlpha = (BYTE)wcstoul(szBuf, nullptr, 10);
	App.GetOpt().crDtShadow = eck::ColorrefToArgb(m_CPKDtShadowColor.GetColor(), byAlpha);
}

void CWndOptions::OptToUI()
{
	const auto& Opt = App.GetOpt();
	WCHAR szBuf[eck::CchI32ToStrBufNoRadix2];
	eck::FONTPICKERINFO fpki;
	m_CBUia.SetCheckState(!!Opt.bUia);
	m_CBAutoRun.SetCheckState(!!Opt.bAutoRun);
	m_CBExcludeFromSnapshot.SetCheckState(!!Opt.bExcludeFromSnapshot);

	m_CCBPos.GetListBox().SetCurrSel((int)Opt.ePos);
	swprintf(szBuf, L"%d", Opt.cyPadding);
	m_EDPadding.SetText(szBuf);
	swprintf(szBuf, L"%d", Opt.dx);
	m_EDDx.SetText(szBuf);
	swprintf(szBuf, L"%d", Opt.dy);
	m_EDDy.SetText(szBuf);
	m_CCBTheme.GetListBox().SetCurrSel((int)Opt.eTheme);
	fpki.pszFontName = Opt.Font1.Data();
	fpki.iPointSize = Opt.iPoint1;
	fpki.iWeight = Opt.iWeight1;
	m_FPKFont1.FromInfo(fpki);
	fpki.pszFontName = Opt.Font2.Data();
	fpki.iPointSize = Opt.iPoint2;
	fpki.iWeight = Opt.iWeight2;
	m_FPKFont2.FromInfo(fpki);
	m_EDLine1.SetText(Opt.rsLine1.Data());
	m_EDLine2.SetText(Opt.rsLine2.Data());
	m_CBColorFont.SetCheckState(!!Opt.bColorFont);
	ColorOptToUI();

	m_CBDtColorFont.SetCheckState(!!Opt.bDtColorFont);
	m_CCBDtPos.GetListBox().SetCurrSel((int)Opt.eDtPos);
	swprintf(szBuf, L"%d", Opt.dxDt);
	m_EDDtDx.SetText(szBuf);
	swprintf(szBuf, L"%d", Opt.dyDt);
	m_EDDtDy.SetText(szBuf);
	m_FPKDtFont.FromInfo({ Opt.rsDtFont.Data(), Opt.iDtPoint, Opt.iDtWeight });
	m_EDDtText.SetText(Opt.rsDtText.Data());
	swprintf(szBuf, L"%g", Opt.fDtShadowRadius);
	m_EDDtShadowRadius.SetText(szBuf);
	swprintf(szBuf, L"%g", Opt.fDtShadowExtent);
	m_EDDtShadowExtent.SetText(szBuf);
	DtTextColorOptToUI();
	DtShadowColorOptToUI();
}

void CWndOptions::UIToOpt()
{
	auto& Opt = App.GetOpt();
	Opt.bUia = !!m_CBUia.GetCheckState();
	Opt.bAutoRun = !!m_CBAutoRun.GetCheckState();
	Opt.bExcludeFromSnapshot = !!m_CBExcludeFromSnapshot.GetCheckState();

	Opt.ePos = (PosType)m_CCBPos.GetListBox().GetCurrSel();
	WCHAR szBuf[eck::CchI32ToStrBufNoRadix2];
	szBuf[0] = L'\0';
	m_EDPadding.GetText(szBuf, ARRAYSIZE(szBuf));
	Opt.cyPadding = (int)_wtoi(szBuf);
	szBuf[0] = L'\0';
	m_EDDx.GetText(szBuf, ARRAYSIZE(szBuf));
	Opt.dx = (int)_wtoi(szBuf);
	szBuf[0] = L'\0';
	m_EDDy.GetText(szBuf, ARRAYSIZE(szBuf));
	Opt.dy = (int)_wtoi(szBuf);
	Opt.eTheme = (ThemeType)m_CCBTheme.GetListBox().GetCurrSel();
	eck::FONTPICKERINFO fpki;
	Opt.Font1 = m_FPKFont1.ToInfo(fpki);
	Opt.iPoint1 = fpki.iPointSize;
	Opt.iWeight1 = fpki.iWeight;
	Opt.Font2 = m_FPKFont2.ToInfo(fpki);
	Opt.iPoint2 = fpki.iPointSize;
	Opt.iWeight2 = fpki.iWeight;
	Opt.rsLine1 = m_EDLine1.GetText();
	Opt.rsLine2 = m_EDLine2.GetText();
	Opt.bColorFont = !!m_CBColorFont.GetCheckState();
	UIToColorOpt();

	Opt.bDtColorFont = !!m_CBDtColorFont.GetCheckState();
	Opt.eDtPos = (PosType)m_CCBDtPos.GetListBox().GetCurrSel();
	szBuf[0] = L'\0';
	m_EDDtDx.GetText(szBuf, ARRAYSIZE(szBuf));
	Opt.dxDt = (int)_wtoi(szBuf);
	szBuf[0] = L'\0';
	m_EDDtDy.GetText(szBuf, ARRAYSIZE(szBuf));
	Opt.dyDt = (int)_wtoi(szBuf);
	Opt.rsDtFont = m_FPKDtFont.ToInfo(fpki);
	Opt.iDtPoint = fpki.iPointSize;
	Opt.iDtWeight = fpki.iWeight;
	Opt.rsDtText = m_EDDtText.GetText();
	Opt.ParseDesktopText();
	szBuf[0] = L'\0';
	m_EDDtShadowRadius.GetText(szBuf, ARRAYSIZE(szBuf));
	Opt.fDtShadowRadius = std::clamp((float)_wtof(szBuf), 0.f, ShadowMax);
	szBuf[0] = L'\0';
	m_EDDtShadowExtent.GetText(szBuf, ARRAYSIZE(szBuf));
	Opt.fDtShadowExtent = std::clamp((float)_wtof(szBuf), 0.f, ShadowMax);
	DtUIToTextColorOpt();
	DtUIToShadowColorOpt();

	Opt.ToIni();
}

void CWndOptions::UpdateTableLayoutRowHeight()
{
	const auto cyLine = eck::DpiScale(30, m_iDpi);
	m_LytComm.SetRowHeight(cyLine);
	m_LytMain.SetRowHeight(cyLine);
	m_LytDt.SetRowHeight(cyLine);
}

LRESULT CWndOptions::EditArrowCtrl(HWND hWnd, UINT uMsg,
	WPARAM wParam, LPARAM lParam, eck::SlotCtx& Ctx)
{
	switch (uMsg)
	{
	case WM_KEYDOWN:
		if (wParam == VK_UP || wParam == VK_DOWN)
		{
			Ctx.Processed();
			WCHAR szBuf[eck::CchI32ToStrBufNoRadix2];
			GetWindowTextW(hWnd, szBuf, ARRAYSIZE(szBuf));
			int iVal = _wtoi(szBuf);
			if (wParam == VK_UP)
				++iVal;
			else if (wParam == VK_DOWN)
				--iVal;
			if (hWnd == m_EDColorAlpha.HWnd ||
				hWnd == m_EDDtColorAlpha.HWnd ||
				hWnd == m_EDDtShadowColorAlpha.HWnd)
			{
				if (iVal <= 0 || iVal >= 255)
					return 0;
			}
			swprintf(szBuf, L"%d", iVal);
			SetWindowTextW(hWnd, szBuf);
			if (hWnd == m_EDPadding.HWnd)
			{
				App.GetOpt().cyPadding = iVal;
				App.GetSignal().Emit({ ANF_MA_UPDATE_PADDING });
			}
			else if (hWnd == m_EDDx.HWnd)
			{
				App.GetOpt().dx = iVal;
				App.GetSignal().Emit({ ANF_MA_UPDATE_POS });
			}
			else if (hWnd == m_EDDy.HWnd)
			{
				App.GetOpt().dy = iVal;
				App.GetSignal().Emit({ ANF_MA_UPDATE_POS });
			}
			else if (hWnd == m_EDColorAlpha.HWnd)
			{
				UIToColorOpt();
				App.GetSignal().Emit({ ANF_MA_UPDATE_COLOR });
			}
			else if (hWnd == m_EDDtDx.HWnd)
			{
				App.GetOpt().dxDt = iVal;
				App.GetSignal().Emit({ ANF_DT_UPDATE_POS });
			}
			else if (hWnd == m_EDDtDy.HWnd)
			{
				App.GetOpt().dyDt = iVal;
				App.GetSignal().Emit({ ANF_DT_UPDATE_POS });
			}
			else if (hWnd == m_EDDtColorAlpha.HWnd)
			{
				DtUIToTextColorOpt();
				App.GetSignal().Emit({ ANF_DT_UPDATE_COLOR });
			}
			else if (hWnd == m_EDDtShadowColorAlpha.HWnd)
			{
				DtUIToShadowColorOpt();
				App.GetSignal().Emit({ ANF_DT_UPDATE_COLOR });
			}
			return 0;
		}
	}
	return 0;
}

LRESULT CWndOptions::EditMultiLineCtrl(HWND hWnd, UINT uMsg,
	WPARAM wParam, LPARAM lParam, eck::SlotCtx& Ctx)
{
	switch (uMsg)
	{
	case WM_GETDLGCODE:
		Ctx.Processed();
		return DLGC_WANTALLKEYS | DLGC_WANTARROWS | DLGC_WANTCHARS | DLGC_HASSETSEL;
	}
	return 0;
}

LRESULT CWndOptions::OnMsg(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_NOTIFY:
	{
		const auto* const pnmhdr = (NMHDR*)lParam;
		if (pnmhdr->hwndFrom == m_CCBPos.HWnd ||
			pnmhdr->hwndFrom == m_CCBDtPos.HWnd)
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
			case eck::NM_LBN_ITEMCHANGED:
			{
				App.GetOpt().eTheme = (ThemeType)m_CCBTheme.GetListBox().GetCurrSel();
				ColorOptToUI();
				if (App.GetOpt().eTheme == ThemeType::Auto)
				{
					EnableWindow(m_CPKColor.HWnd, FALSE);
					EnableWindow(m_EDColorAlpha.HWnd, FALSE);
				}
				else
				{
					EnableWindow(m_CPKColor.HWnd, TRUE);
					EnableWindow(m_EDColorAlpha.HWnd, TRUE);
				}
			}
			return 0;
			}
		else if (pnmhdr->hwndFrom == m_CPKColor.HWnd)
			switch (pnmhdr->code)
			{
			case eck::NM_CLP_CLRCHANGED:
				UIToColorOpt();
				App.GetSignal().Emit({ ANF_MA_UPDATE_COLOR });
				return 0;
			}
		else if (pnmhdr->hwndFrom == m_CPKDtColor.HWnd)
			switch (pnmhdr->code)
			{
			case eck::NM_CLP_CLRCHANGED:
				DtUIToTextColorOpt();
				App.GetSignal().Emit({ ANF_DT_UPDATE_COLOR });
				return 0;
			}
		else if (pnmhdr->hwndFrom == m_CPKDtShadowColor.HWnd)
			switch (pnmhdr->code)
			{
			case eck::NM_CLP_CLRCHANGED:
				DtUIToShadowColorOpt();
				App.GetSignal().Emit({ ANF_DT_UPDATE_COLOR });
				return 0;
			}
		else if (pnmhdr->hwndFrom == m_Tab.HWnd)
			switch (pnmhdr->code)
			{
			case TCN_SELCHANGE:
				m_LytTab.ShowFrame(m_Tab.GetCurSel());
				return 0;
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
				App.GetSignal().Emit({ ANF_MA_REFRESH | ANF_DT_REFRESH });
				return 0;
			}
			else if (lParam == (LPARAM)m_CBAutoRun.HWnd)
			{
				eck::SetAutoRun(L"qk_s_watermark_cxx",
					m_CBAutoRun.GetCheckState(),
					eck::AutoRunType::TaskScheduler | eck::AutoRunType::RunAdmin);
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
		UpdateTableLayoutRowHeight();
		m_Layout.LoOnDpiChanged(m_iDpi);
		m_LytTab.LoOnDpiChanged(m_iDpi);
		eck::MsgOnDpiChanged(hWnd, lParam);
		UpdateDpi();
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
	{
		Show(SW_SHOW);
		SetForegroundWindow(HWnd);
	}
	else if (uMsg == WM_RBUTTONUP)
	{
		if (m_bExitMsgBox)
			return;
		m_bExitMsgBox = TRUE;
		if (MessageBoxW(HWnd, L"确定要退出吗？", L"退出", MB_YESNO | MB_ICONQUESTION) == IDYES)
		{
			App.GetSignal().Emit({ ANF_EXIT });
			Destroy();
			PostQuitMessage(0);
		}
		m_bExitMsgBox = FALSE;
	}
}