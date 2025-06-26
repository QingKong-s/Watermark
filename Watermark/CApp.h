#pragma once

enum class PosType : BYTE
{
	Min,
	TopLeft = Min,
	TopRight,
	BottomLeft,
	BottomRight,
	Max
};

enum class ThemeType : BYTE
{
	Min,
	Light = Min,
	Dark,
	Common,
	Auto,
	Max,
};

constexpr float ShadowMax{ 8.f };

struct Options
{
	BOOLEAN bUia{};			// UIAccess
	BOOLEAN bAutoRun{};		// 开机自启
	BOOLEAN bExcludeFromSnapshot{};// 防止被截图

	PosType ePos{};			// 显示位置
	ThemeType eTheme{};		// 主题
	int dx{};				// 偏移X
	int dy{};				// 偏移Y
	eck::CRefStrW Font1{};	// 首行字体
	int iPoint1{};			// 首行字号
	int iWeight1{};			// 首行粗细
	eck::CRefStrW Font2{};	// 次行字体
	int iPoint2{};			// 次行字号
	int iWeight2{};			// 次行粗细
	ARGB crLight{};			// 亮色文本颜色
	ARGB crDark{};			// 暗色文本颜色
	ARGB crCommon{};		// 通用文本颜色
	int cyPadding{};		// 行距
	eck::CRefStrW rsLine1{};// 首行文本
	eck::CRefStrW rsLine2{};// 次行文本
	BOOLEAN bColorFont{};

	BOOLEAN bDtColorFont{};
	PosType eDtPos{};
	BOOLEAN bDtShadow{};
	eck::CRefStrW rsDtFont{};
	int iDtPoint{};
	int iDtWeight{};
	ARGB crDtText{};
	ARGB crDtShadow{};
	int dxDt{};
	int dyDt{};
	eck::CRefStrW rsDtText{};
	eck::CRefStrW rsDtTextParsed{};// 不保存
	float fDtShadowRadius{};
	float fDtShadowExtent{};

	void FromIni();
	void ToIni();
	ARGB GetCurrColor() const;
	void SetCurrColor(ARGB cr);

	void ParseDesktopText();

	EckInlineNdCe auto GetDtlFlags()
	{
		return bColorFont ? D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT : D2D1_DRAW_TEXT_OPTIONS_NONE;
	}

	EckInlineNdCe auto GetDtDtlFlags()
	{
		return bDtColorFont ? D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT : D2D1_DRAW_TEXT_OPTIONS_NONE;
	}
};

enum : UINT
{
	ANF_MA_UPDATE_POS = 1u << 0,	// 主窗口：更新位置
	ANF_MA_UPDATE_PADDING = 1u << 2,// 主窗口：更新行距
	ANF_MA_REFRESH = 1u << 3,		// 主窗口：全部刷新
	ANF_EXIT = 1u << 4,				// 退出程序
	ANF_MA_UPDATE_COLOR = 1u << 5,	// 主窗口：更新颜色
	ANF_DT_REFRESH = 1u << 6,		// 桌面文本：全部刷新
	ANF_DT_UPDATE_POS = 1u << 7,	// 桌面文本：更新位置
	ANF_DT_UPDATE_COLOR = 1u << 1,	// 桌面文本：更新尺寸
};

struct APP_NOTIFY
{
	UINT uFlags;// ANF_*
};

class CApp
{
private:
	eck::CSignal<eck::NoIntercept_T, void, const APP_NOTIFY&> m_Sig{};
	Options m_Opt{};
	eck::CRefStrW m_rsUserLocale{};
public:
	void Init();

	EckInlineNdCe auto& GetSignal() { return m_Sig; }
	EckInlineNdCe auto& GetOpt() { return m_Opt; }
	EckInlineNdCe auto& GetUserLocale() { return m_rsUserLocale; }
};

extern CApp App;