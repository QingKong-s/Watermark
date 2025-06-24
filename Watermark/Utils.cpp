#include "pch.h"
#include "Utils.h"


static BOOL PdtReadRegInfo(std::wstring_view svValue, eck::CRefStrW& rsOut)
{
	eck::CRegKey Key{ HKEY_LOCAL_MACHINE,
		LR"(SOFTWARE\Microsoft\Windows NT\CurrentVersion)",KEY_READ };
	WCHAR szValue[48];
	eck::TcsCopyLenEnd(szValue, svValue.data(),
		std::min(svValue.size(), ARRAYSIZE(szValue) - 1));
	DWORD cbBuf, dwType;
	if (Key.QueryValue(szValue, nullptr, &cbBuf, &dwType) ||
		dwType != REG_SZ)
		return FALSE;
	if (Key.QueryValue(szValue, rsOut.PushBack(cbBuf / sizeof(WCHAR)), &cbBuf))
		return FALSE;
	while (!rsOut.Back())
		rsOut.PopBack();
	return TRUE;
}

DtTextErr ParseDesktopText(PCWSTR pszText, eck::CRefStrW& rsOut)
{
	rsOut.Reserve(128);
	PCWCH pszLastPercent{};
	for (WCHAR ch; ch = *pszText; ++pszText)
	{
		if (ch == L'%')
		{
			if (pszLastPercent)// %xxx% 闭合
			{
				const auto p = pszLastPercent + 1;
				const auto cch = size_t(pszText - pszLastPercent - 1);
				constexpr WCHAR RegPrefix[]{ L"Reg." };
#define PDT_HitString(s) (cch == ARRAYSIZE(s) - 1) && (eck::TcsCompareLenI(p, EckStrAndLen(s)) == 0)
				if (cch >= ARRAYSIZE(RegPrefix) &&
					eck::TcsCompareLenI(p, EckStrAndLen(RegPrefix)) == 0)
				{
					const std::wstring_view svVal
					{
						p + ARRAYSIZE(RegPrefix) - 1,
						cch - ARRAYSIZE(RegPrefix) + 1
					};
					if (!PdtReadRegInfo(svVal, rsOut))
						rsOut.PushBack(pszLastPercent, (int)cch + 2);
				}
				else if (PDT_HitString(L"UserName"))
				{
					DWORD cchBuf{};
					GetUserNameW(nullptr, &cchBuf);
					if (!cchBuf ||
						!GetUserNameW(rsOut.PushBack(cchBuf), &cchBuf))
						rsOut.PushBack(pszLastPercent, (int)cch + 2);
				}
				else if (PDT_HitString(L"OSCaption"))
				{
					VARIANT Var{};
					if (FAILED(eck::WmiQueryClassProp(L"SELECT Caption FROM Win32_OperatingSystem",
						L"Caption", Var)) ||
						Var.vt != VT_BSTR ||
						Var.bstrVal == nullptr)
						rsOut.PushBack(pszLastPercent, (int)cch + 2);
					else
					{
						const auto cch = (int)SysStringLen(Var.bstrVal);
						rsOut.PushBack(Var.bstrVal, cch);
						VariantClear(&Var);
					}
				}
				else
					rsOut.PushBack(pszLastPercent, (int)cch + 2);
#undef PDT_HitString
				pszLastPercent = nullptr;
			}
			else
				if (*(pszText + 1) == L'%')// %% -> %
				{
					++pszText;
					rsOut.PushBackChar(L'%');
				}
				else
					pszLastPercent = pszText;
		}
		else if (!pszLastPercent)
			rsOut.PushBackChar(ch);
	}
	return DtTextErr::Ok;
}