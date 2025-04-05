#include "pch.h"
#include "GlobalOptions.h"


Options g_Options{};

void Options::FromIni()
{
	eck::CIniExtMut Ini{};
	const auto rbIni = eck::ReadInFile(L"Options.ini");
	Ini.Load((PCWSTR)rbIni.Data(), rbIni.Size() / sizeof WCHAR);
	auto Sec = Ini.GetSection(L"Watermark");

	g_Options.bUia = Ini.GetKeyValue(Sec, L"UIAccess").GetBool();
	g_Options.bAutoRun = Ini.GetKeyValue(Sec, L"AutoRun").GetBool();
	g_Options.ePos = Ini.GetKeyValue(Sec, L"Pos").GetEnumCheck(
		PosType::Min, PosType::Max, PosType::BottomRight);
	g_Options.dx = Ini.GetKeyValue(Sec, L"MarginX").GetInt(100);
	g_Options.dy = Ini.GetKeyValue(Sec, L"MarginY").GetInt(40);
	g_Options.eTheme = Ini.GetKeyValue(Sec, L"Theme").GetEnumCheck(
		ThemeType::Min, ThemeType::Max);
	Ini.GetKeyValue(Sec, L"Font1").GetString(g_Options.Font1, L"Segoe UI");
	g_Options.iPoint1 = Ini.GetKeyValue(Sec, L"Size1").GetInt(18);
	g_Options.iWeight1 = Ini.GetKeyValue(Sec, L"Weight1").GetInt(400);
	Ini.GetKeyValue(Sec, L"Font2").GetString(g_Options.Font2, L"Segoe UI");
	g_Options.iPoint2 = Ini.GetKeyValue(Sec, L"Size2").GetInt(12);
	g_Options.iWeight2 = Ini.GetKeyValue(Sec, L"Weight2").GetInt(400);
	g_Options.crLight = (ARGB)Ini.GetKeyValue(Sec, L"ColorLight").GetInt(0x66C4C4C4);
	g_Options.crDark = (ARGB)Ini.GetKeyValue(Sec, L"ColorDark").GetInt(0x33888888);
	g_Options.crCommon = (ARGB)Ini.GetKeyValue(Sec, L"ColorCommon").GetInt(0x66C4C4C4);
	g_Options.cyPadding = Ini.GetKeyValue(Sec, L"Padding").GetInt(-2);
	Ini.GetKeyValue(Sec, L"Text1").GetString(g_Options.rsLine1, L"激活 Windows");
	Ini.GetKeyValue(Sec, L"Text2").GetString(g_Options.rsLine2, L"转到“设置”以激活 Windows");
}

void Options::ToIni()
{
	eck::CIniExtMut Ini{};
	auto Sec = Ini.CreateSection(L"Watermark");
	eck::CRefStrW rs{};
	rs.Format(L"%d", g_Options.bUia);
	Ini.CreateKeyValue(Sec, L"UIAccess", rs.ToStringView());
	rs.Format(L"%d", g_Options.bAutoRun);
	Ini.CreateKeyValue(Sec, L"AutoRun", rs.ToStringView());
	rs.Format(L"%d", g_Options.ePos);
	Ini.CreateKeyValue(Sec, L"Pos", rs.ToStringView());
	rs.Format(L"%d", g_Options.dx);
	Ini.CreateKeyValue(Sec, L"MarginX", rs.ToStringView());
	rs.Format(L"%d", g_Options.dy);
	Ini.CreateKeyValue(Sec, L"MarginY", rs.ToStringView());
	rs.Format(L"%d", g_Options.eTheme);
	Ini.CreateKeyValue(Sec, L"Theme", rs.ToStringView());
	Ini.CreateKeyValue(Sec, L"Font1", g_Options.Font1.ToStringView());
	rs.Format(L"%d", g_Options.iPoint1);
	Ini.CreateKeyValue(Sec, L"Size1", rs.ToStringView());
	rs.Format(L"%d", g_Options.iWeight1);
	Ini.CreateKeyValue(Sec, L"Weight1", rs.ToStringView());
	Ini.CreateKeyValue(Sec, L"Font2", g_Options.Font2.ToStringView());
	rs.Format(L"%d", g_Options.iPoint2);
	Ini.CreateKeyValue(Sec, L"Size2", rs.ToStringView());
	rs.Format(L"%d", g_Options.iWeight2);
	Ini.CreateKeyValue(Sec, L"Weight2", rs.ToStringView());
	rs.Format(L"%d", g_Options.crLight);
	Ini.CreateKeyValue(Sec, L"ColorLight", rs.ToStringView());
	rs.Format(L"%d", g_Options.crDark);
	Ini.CreateKeyValue(Sec, L"ColorDark", rs.ToStringView());
	rs.Format(L"%d", g_Options.crCommon);
	Ini.CreateKeyValue(Sec, L"ColorCommon", rs.ToStringView());
	rs.Format(L"%d", g_Options.cyPadding);
	Ini.CreateKeyValue(Sec, L"Padding", rs.ToStringView());
	Ini.CreateKeyValue(Sec, L"Text1", g_Options.rsLine1.ToStringView());
	Ini.CreateKeyValue(Sec, L"Text2", g_Options.rsLine2.ToStringView());

	rs.Clear();
	Ini.Save(rs);
	eck::WriteToFile(L"Options.ini", rs.Data(), (DWORD)rs.ByteSizePure());
}

ARGB Options::GetCurrColor()
{
	ARGB cr;
	switch (g_Options.eTheme)
	{
	case ThemeType::Light:	cr = g_Options.crLight;		break;
	case ThemeType::Dark:	cr = g_Options.crDark;		break;
	case ThemeType::Common: cr = g_Options.crCommon;	break;
	default: cr = ShouldAppsUseDarkMode() ? g_Options.crDark : g_Options.crLight; break;
	}
	return cr;
}

void Options::SetCurrColor(ARGB cr)
{
	switch (g_Options.eTheme)
	{
	case ThemeType::Light:	g_Options.crLight = cr;		break;
	case ThemeType::Dark:	g_Options.crDark = cr;		break;
	case ThemeType::Common: g_Options.crCommon = cr;	break;
	default: break;
	}
}
