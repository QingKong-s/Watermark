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

struct Options
{
	BOOLEAN bUia{};			// UIAccess
	BOOLEAN bAutoRun{};		// 开机自启
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

	void FromIni();
	void ToIni();
	ARGB GetCurrColor();
	void SetCurrColor(ARGB cr);
};

extern Options g_Options;