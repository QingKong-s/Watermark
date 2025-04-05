﻿#include "pch.h"
#include "CWndMain.h"
#include "CWndOptions.h"
#include "GlobalOptions.h"
#include "eck\Env.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
	_In_ PWSTR pszCmdLine, _In_ int nCmdShow)
{
	_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);

	if (const auto hr = CoInitialize(nullptr); FAILED(hr))
	{
		EckDbgPrintFormatMessage(hr);
		eck::MsgBox(eck::Format(L"CoInitialize failed!\r\nhr = %08X", hr),
			L"Error", MB_ICONERROR);
		return 0;
	}

	eck::INITPARAM ip{};
	ip.uFlags = eck::EIF_NOINITD2D | eck::EIF_NOINITDWRITE;
	DWORD dwErr;
	if (const auto iRetInit = eck::Init(hInstance, &ip, &dwErr);
		iRetInit != eck::InitStatus::Ok)
	{
		EckDbgPrintFormatMessage(dwErr);
		eck::MsgBox(eck::Format(L"Init failed!\r\nInitStatus = %d\r\nError code = %08X",
			(int)iRetInit, dwErr), L"Error", MB_ICONERROR);
		return 0;
	}

	g_Options.FromIni();

#ifndef _DEBUG
	if (g_Options.bUia && IsNTAdmin(0, nullptr))
	{
		if (!eck::UiaIsAcquired())
		{
			eck::UIA uia;
			if (NT_SUCCESS(eck::UiaTryAcquire(uia)))
				eck::UiaRestart(uia);
		}
	}
#endif// _DEBUG

	const auto pWnd = new CWndMain{};
	const auto hMon = eck::GetOwnerMonitor(nullptr);
	const auto iDpi = eck::GetMonitorDpi(hMon);

	pWnd->Create(nullptr, WS_POPUP, 
		WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST | WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW,
		0, 0, 10, 10, nullptr, 0);
	pWnd->UpdateFont();
	pWnd->Show(SW_SHOWNOACTIVATE);

	const auto pOptWnd = new CWndOptions{ pWnd };
	SIZE size = { 410, 430 };
	eck::DpiScale(size, iDpi);
	const auto pt = eck::CalcCenterWndPos(nullptr, size.cx, size.cy);

	pOptWnd->Create(L"选项", WS_OVERLAPPEDWINDOW | WS_POPUP,0,
		pt.x, pt.y, size.cx, size.cy, nullptr, 0);

	MSG msg;
	while (GetMessageW(&msg, nullptr, 0, 0))
	{
		if (!eck::PreTranslateMessage(msg))
		{
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
	}
	delete pWnd;
	eck::ThreadUnInit();
	eck::UnInit();
	CoUninitialize();
	return (int)msg.wParam;
}