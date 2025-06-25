#include "pch.h"
#include "CApp.h"
#include "Utils.h"

CApp App{};

void Options::FromIni()
{
	eck::CIniExtMut Ini{};
	const auto rbIni = eck::ReadInFile((eck::GetRunningPath() + L"\\Options.ini").Data());
	Ini.Load((PCWSTR)rbIni.Data(), rbIni.Size() / sizeof WCHAR);
	auto Sec = Ini.GetSection(L"Watermark");

	bUia = Ini.GetKeyValue(Sec, L"UIAccess").GetBool();
	bAutoRun = Ini.GetKeyValue(Sec, L"AutoRun").GetBool();
	ePos = Ini.GetKeyValue(Sec, L"Pos").GetEnumCheck(
		PosType::Min, PosType::Max, PosType::BottomRight);
	dx = Ini.GetKeyValue(Sec, L"MarginX").GetInt(100);
	dy = Ini.GetKeyValue(Sec, L"MarginY").GetInt(50);
	eTheme = Ini.GetKeyValue(Sec, L"Theme").GetEnumCheck(
		ThemeType::Min, ThemeType::Max);
	Ini.GetKeyValue(Sec, L"Font1").GetString(Font1, L"Segoe UI");
	iPoint1 = Ini.GetKeyValue(Sec, L"Size1").GetInt(16);
	iWeight1 = Ini.GetKeyValue(Sec, L"Weight1").GetInt(340);
	Ini.GetKeyValue(Sec, L"Font2").GetString(Font2, L"Segoe UI");
	iPoint2 = Ini.GetKeyValue(Sec, L"Size2").GetInt(12);
	iWeight2 = Ini.GetKeyValue(Sec, L"Weight2").GetInt(340);
	crLight = (ARGB)Ini.GetKeyValue(Sec, L"ColorLight").GetInt(0x66C4C4C4);
	crDark = (ARGB)Ini.GetKeyValue(Sec, L"ColorDark").GetInt(0x33888888);
	crCommon = (ARGB)Ini.GetKeyValue(Sec, L"ColorCommon").GetInt(0x66C4C4C4);
	cyPadding = Ini.GetKeyValue(Sec, L"Padding").GetInt(2);
	Ini.GetKeyValue(Sec, L"Text1").GetString(rsLine1, L"激活 Windows");
	Ini.GetKeyValue(Sec, L"Text2").GetString(rsLine2, L"转到“设置”以激活 Windows");

	Sec = Ini.GetSection(L"DesktopWatermark");
	Ini.GetKeyValue(Sec, L"DtFont").GetString(rsDtFont, L"Segoe UI");
	iDtPoint = Ini.GetKeyValue(Sec, L"DtSize").GetInt(10);
	iDtWeight = Ini.GetKeyValue(Sec, L"DtWeight").GetInt(400);
	crDtText = (ARGB)Ini.GetKeyValue(Sec, L"DtColor").GetInt(0xFFF0F0F0);
	Ini.GetKeyValue(Sec, L"DtText").GetString(rsDtText, LR"(%OSCaption%
评估副本。 %Reg.BuildLabEx%
%Reg.SystemRoot%)");
	ParseDesktopText();
	dxDt = Ini.GetKeyValue(Sec, L"DtMarginX").GetInt(5);
	dyDt = Ini.GetKeyValue(Sec, L"DtMarginY").GetInt(2);
	eDtPos = Ini.GetKeyValue(Sec, L"DtPos").GetEnumCheck(
		PosType::Min, PosType::Max, PosType::BottomRight);
}

void Options::ToIni()
{
	eck::CIniExtMut Ini{};
	auto Sec = Ini.CreateSection(L"Watermark");
	eck::CRefStrW rs{};
	rs.Format(L"%d", bUia);
	Ini.CreateKeyValue(Sec, L"UIAccess", rs.ToStringView());
	rs.Format(L"%d", bAutoRun);
	Ini.CreateKeyValue(Sec, L"AutoRun", rs.ToStringView());
	rs.Format(L"%d", ePos);
	Ini.CreateKeyValue(Sec, L"Pos", rs.ToStringView());
	rs.Format(L"%d", dx);
	Ini.CreateKeyValue(Sec, L"MarginX", rs.ToStringView());
	rs.Format(L"%d", dy);
	Ini.CreateKeyValue(Sec, L"MarginY", rs.ToStringView());
	rs.Format(L"%d", eTheme);
	Ini.CreateKeyValue(Sec, L"Theme", rs.ToStringView());
	Ini.CreateKeyValue(Sec, L"Font1", Font1.ToStringView());
	rs.Format(L"%d", iPoint1);
	Ini.CreateKeyValue(Sec, L"Size1", rs.ToStringView());
	rs.Format(L"%d", iWeight1);
	Ini.CreateKeyValue(Sec, L"Weight1", rs.ToStringView());
	Ini.CreateKeyValue(Sec, L"Font2", Font2.ToStringView());
	rs.Format(L"%d", iPoint2);
	Ini.CreateKeyValue(Sec, L"Size2", rs.ToStringView());
	rs.Format(L"%d", iWeight2);
	Ini.CreateKeyValue(Sec, L"Weight2", rs.ToStringView());
	rs.Format(L"%d", crLight);
	Ini.CreateKeyValue(Sec, L"ColorLight", rs.ToStringView());
	rs.Format(L"%d", crDark);
	Ini.CreateKeyValue(Sec, L"ColorDark", rs.ToStringView());
	rs.Format(L"%d", crCommon);
	Ini.CreateKeyValue(Sec, L"ColorCommon", rs.ToStringView());
	rs.Format(L"%d", cyPadding);
	Ini.CreateKeyValue(Sec, L"Padding", rs.ToStringView());
	Ini.CreateKeyValue(Sec, L"Text1", rsLine1.ToStringView());
	Ini.CreateKeyValue(Sec, L"Text2", rsLine2.ToStringView());

	rs.Clear();
	Ini.Save(rs);
	eck::WriteToFile((eck::GetRunningPath() + L"\\Options.ini").Data(),
		rs.Data(), (DWORD)rs.ByteSizePure());
}

ARGB Options::GetCurrColor() const
{
	ARGB cr;
	switch (eTheme)
	{
	case ThemeType::Light:	cr = crLight;		break;
	case ThemeType::Dark:	cr = crDark;		break;
	case ThemeType::Common: cr = crCommon;	break;
	default: cr = ShouldAppsUseDarkMode() ? crDark : crLight; break;
	}
	return cr;
}

void Options::SetCurrColor(ARGB cr)
{
	switch (eTheme)
	{
	case ThemeType::Light:	crLight = cr;		break;
	case ThemeType::Dark:	crDark = cr;		break;
	case ThemeType::Common: crCommon = cr;	break;
	default: break;
	}
}

void Options::ParseDesktopText()
{
	rsDtTextParsed.Clear();
	::ParseDesktopText(rsDtText.Data(), rsDtTextParsed);
}

void CApp::Init()
{
	GetUserLocaleName(m_rsUserLocale);
}