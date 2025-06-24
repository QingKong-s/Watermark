#include "pch.h"
#include "CWndMain.h"
#include "CWndDesktopText.h"
#include "CWndOptions.h"
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

	DWORD dwErr;
	if (const auto iRetInit = eck::Init(hInstance, nullptr, &dwErr);
		iRetInit != eck::InitStatus::Ok)
	{
		EckDbgPrintFormatMessage(dwErr);
		eck::MsgBox(eck::Format(L"Init failed!\r\nInitStatus = %d\r\nError code = %08X",
			(int)iRetInit, dwErr), L"Error", MB_ICONERROR);
		return 0;
	}

	App.Init();
	App.GetOpt().FromIni();

	const auto bAdmin = IsNTAdmin(0, nullptr);
#ifndef _DEBUG
	if (App.GetOpt().bUia && bAdmin)
	{
		if (!eck::UiaIsAcquired())
		{
			eck::UIA uia;
			NTSTATUS nts = eck::UiaTryAcquire(uia);
			if (NT_SUCCESS(nts))
				eck::UiaRestart(uia);
			else
			{
				eck::CRefStrW rs{};
				rs.Format(L"Failed to acquire UI Automation. NTSTATUS = %08X", nts);
				eck::MsgBox(rs);
			}
		}
	}
#endif// _DEBUG

	const auto pWndDt = new CWndDesktopText{};
	const auto pWnd = new CWndMain{};
	const auto hMon = eck::GetOwnerMonitor(nullptr);
	const auto iDpi = eck::GetMonitorDpi(hMon);

	constexpr DWORD dwCommExStyle = WS_EX_TOPMOST |
		WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE;
	const auto hGhost = CreateWindowExW(dwCommExStyle,
		eck::WCN_DUMMY, nullptr, WS_OVERLAPPEDWINDOW,
		-32000, -32000, 0, 0, nullptr, nullptr, hInstance, nullptr);

	pWnd->Create(nullptr, WS_POPUP, dwCommExStyle |
		WS_EX_LAYERED | WS_EX_TRANSPARENT,
		0, 0, 100, 100, nullptr, 0);
	SetWindowLongPtrW(pWnd->HWnd, GWLP_HWNDPARENT, (LONG_PTR)hGhost);
	pWnd->Show(SW_SHOWNOACTIVATE);

	pWndDt->Create(L"AAAAAAAAAAAAAAAAAAAAAAAAA", WS_POPUP, WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE |
		WS_EX_LAYERED | WS_EX_TRANSPARENT, 0, 0, 100, 100, nullptr, 0);
	pWndDt->Show(SW_SHOWNOACTIVATE);

	const auto pOptWnd = new CWndOptions{};
	SIZE size = { 410, 430 };
	eck::DpiScale(size, iDpi);
	const auto pt = eck::CalcCenterWndPos(nullptr, size.cx, size.cy);

	pOptWnd->Create(bAdmin ? L"选项（管理员）" : L"选项",
		WS_OVERLAPPEDWINDOW | WS_POPUP, 0,
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
	DestroyWindow(hGhost);
	delete pOptWnd;
	delete pWnd;
	delete pWndDt;
	eck::ThreadUnInit();
	eck::UnInit();
	CoUninitialize();
	return (int)msg.wParam;
}