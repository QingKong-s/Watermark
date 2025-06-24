#pragma once
class CWndMain;
class CWndOptions : public eck::CForm
{
private:
	eck::CButton m_CBUia{};
	eck::CButton m_CBAutoRun{};

	eck::CLabel m_LAPos{};
	eck::CComboBoxNew m_CCBPos{};

	eck::CLabel m_LAPadding{};
	eck::CEditExt m_EDPadding{};

	eck::CLabel m_LADx{};
	eck::CEditExt m_EDDx{};
	eck::CLabel m_LADy{};
	eck::CEditExt m_EDDy{};

	eck::CLabel m_LATheme{};
	eck::CComboBoxNew m_CCBTheme{};

	eck::CLabel m_LAColor{};
	eck::CColorPicker m_CPKColor{};
	eck::CEditExt m_EDColorAlpha{};

	eck::CLabel m_LAFont1{};
	eck::CFontPicker m_FPKFont1{};
	eck::CLabel m_LAFont2{};
	eck::CFontPicker m_FPKFont2{};

	eck::CLabel m_LALine1{};
	eck::CEditExt m_EDLine1{};
	eck::CLabel m_LALine2{};
	eck::CEditExt m_EDLine2{};

	eck::CButton m_BTReLoad{};
	eck::CButton m_BTSave{};
	eck::CButton m_BTGitHub{};

	eck::CTableLayout m_Layout{};
	eck::CLinearLayoutH m_LytBtn{};

	HFONT m_hFont{};
	int m_iDpi{ USER_DEFAULT_SCREEN_DPI };
	BOOL m_bExitMsgBox{};
	HICON m_hiGitHub{};

	void UpdateDpi();

	void OnCreate(HWND hWnd);

	void OnSize(LPARAM lParam);

	void ColorOptToUI();
	void UIToColorOpt();

	void OptToUI();
	void UIToOpt();

	void ColorUpdated();

	LRESULT EditArrowCtrl(HWND hWnd, UINT uMsg,
		WPARAM wParam, LPARAM lParam, eck::SlotCtx& Ctx);
public:
	ECK_CWND_SINGLEOWNER(CWndOptions);
	ECK_CWND_CREATE_CLS_HINST(eck::WCN_DUMMY, eck::g_hInstance);

	BOOL PreTranslateMessage(const MSG& Msg) override;

	LRESULT OnMsg(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;

	void OnTrayNotify(UINT uMsg, UINT uID, int x, int y) override;
};