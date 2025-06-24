#pragma once

enum class DtTextErr
{
	Ok,
};

DtTextErr ParseDesktopText(PCWSTR pszText, eck::CRefStrW& rsOut);