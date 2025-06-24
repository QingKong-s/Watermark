#pragma once

class CWndDesktopText : public eck::CWnd
{
private:
	ComPtr<IDWriteTextLayout> m_pTextLayout{};
	ComPtr<IDWriteTextFormat> m_pTextFormat{};
public:
	ECK_CWND_SINGLEOWNER_NO_DEF_CONS(CWndDesktopText);
	ECK_CWND_CREATE_CLS_HINST(eck::WCN_DUMMY, eck::g_hInstance);
};