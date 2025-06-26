#pragma once

enum class DtTextErr
{
	Ok,
};

DtTextErr ParseDesktopText(PCWSTR pszText, eck::CRefStrW& rsOut);

BOOL ExcludeFromSnapshot(HWND hWnd, BOOL bExclude = TRUE);