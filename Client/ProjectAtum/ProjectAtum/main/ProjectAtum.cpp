//#define _INET_DBG //enable hackshield
#include "StdAfx.h"
#include "AtumApplication.h"
#include "dbgHelp.h"

#pragma comment(lib, "dbghelp.lib")

 #define  SET_CRT_DEBUG_FIELD(a) \
                 _CrtSetDbgFlag((a) | _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG))
#ifdef _AC_EXTENDED_THREAD
#include "EvoAntiCheat.h"
#include "CollisionsProcessor.h"
#include "DrawingProc.h"
//#include "cheat-monitor.h"
#endif
extern "C"
{
	__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
}
extern "C"
{
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}
#ifdef _AC_EXTENDED_THREAD
//#ifdef _M_IX86
void NTAPI TLSCallbacks(PVOID DllHandle, DWORD dwReason, PVOID Reserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
			auto ac = EvoAntiCheat::GetInstance();
		ac->Init();
		ac->StartDetection();
		
	/*auto cp = CollisionsProcessor::GetInstance();
		cp->Init();
		cp->StartProcessing();*/


		break;
	}
}

//linker spec
#ifdef _M_IX86
#pragma comment (linker, "/INCLUDE:__tls_used")
#pragma comment (linker, "/INCLUDE:__tls_callback")
#else
#pragma comment (linker, "/INCLUDE:_tls_used")
#pragma comment (linker, "/INCLUDE:_tls_callback")
#endif
EXTERN_C
#ifdef _M_X64
#pragma const_seg (".CRT$XLF")
const
#else
#pragma data_seg (".CRT$XLF")
#endif
//end linker

//tls import
PIMAGE_TLS_CALLBACK _tls_callback = TLSCallbacks;
#pragma const_seg ()
#pragma data_seg ()
#endif
//end 
// tls declaration
//#endif
LONG __stdcall exception_minidump(_EXCEPTION_POINTERS* pExceptionInfo)
{
	char temp[256]; *temp = '\0';
#ifdef _M_IX86
	wsprintf(temp, "An unknown error has occurred.\nDumpfile has been created!\nPlease contact customer support at: https://support.oldschoolrivals.com\nRemember to attach CrashInfo.DMP file to your ticket!\nError code [0x%08x]", pExceptionInfo->ExceptionRecord->ExceptionCode);
#else
	wsprintf(temp, "An unknown error has occurred.\nDumpfile has been created!\nPlease contact customer support at: https://support.oldschoolrivals.com\nRemember to attach CrashInfo64.DMP file to your ticket!\nError code [0x%08x]", pExceptionInfo->ExceptionRecord->ExceptionCode);
#endif
	MessageBox(nullptr, temp, "Ooopsie :X", MB_OK);
	memset(temp, 0x00, sizeof(temp));
	
    auto* const hProcess = GetCurrentProcess();
	const auto dProcessId = GetCurrentProcessId();
#ifdef _M_IX86
	auto* const hFile = CreateFile(".\\CrashInfo.dmp", GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
#else
	auto* const hFile = CreateFile(".\\CrashInfo64.dmp", GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
#endif

    MINIDUMP_EXCEPTION_INFORMATION eInfo;
    eInfo.ThreadId = GetCurrentThreadId();
    eInfo.ExceptionPointers = pExceptionInfo;
    eInfo.ClientPointers = FALSE;

    MiniDumpWriteDump(hProcess, dProcessId, hFile, MiniDumpNormal, pExceptionInfo ? &eInfo : nullptr, nullptr, nullptr);

    return EXCEPTION_EXECUTE_HANDLER;
}

HANDLE g_hMutexMonoInstance = nullptr;

HHOOK hHook;

//LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
//{
//	LPKBDLLHOOKSTRUCT kbdCheck = (LPKBDLLHOOKSTRUCT)lParam;
//	if ((kbdCheck->flags & LLKHF_INJECTED) || (kbdCheck->flags & LLKHF_LOWER_IL_INJECTED)) // injected
//	{
//		MSG_FC_CHARACTER_GET_REAL_WEAPON_INFO_OK msg_Weapon;
//		msg_Weapon.ItemUID0 = 111;
//		msg_Weapon.ReattackTime0 = 1;
//		msg_Weapon.ShotNum0 = 0;
//		msg_Weapon.MultiNum0 = 0;
//		msg_Weapon.nFound = 50002;
//
//		if (g_pD3dApp && g_pD3dApp->m_pFieldWinSocket)
//			g_pD3dApp->m_pFieldWinSocket->SendMsg(T_FC_CHARACTER_GET_REAL_WEAPON_INFO_OK, reinterpret_cast<char*>(&msg_Weapon), sizeof(msg_Weapon));
//
//		//MessageBox(NULL, "Keyboard macro detected. \r\nCode for support(0xAC150001)", "Inetpub Anti - Cheat", MB_OK | MB_ICONERROR);
//		FDBG("AntiCheat :: Triggered in skill check!");
//		//SendMessage(g_pD3dApp->GetHwnd(), WM_CLOSE, 0, 0);
//		return 0;
//
//	}
//	//MessageBox(NULL, "Keyboard macro detected. \r\nCode for support(0xAC150001)", "Inetpub Anti - Cheat", MB_OK | MB_ICONERROR);
//	return CallNextHookEx(hHook, nCode, wParam, lParam);
//}
/*
LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	LPMSLLHOOKSTRUCT detectMacro = (LPMSLLHOOKSTRUCT)lParam;
	if ((detectMacro->flags & LLMHF_INJECTED) || (detectMacro->flags & LLMHF_LOWER_IL_INJECTED)) // injected
	{
		MSG_FC_CHARACTER_GET_REAL_WEAPON_INFO_OK msg_Weapon;
		msg_Weapon.ItemUID0 = 222;
		msg_Weapon.ReattackTime0 = 2;
		msg_Weapon.ShotNum0 = 0;
		msg_Weapon.MultiNum0 = 0;
		msg_Weapon.nFound = 50001;

		if (g_pD3dApp && g_pD3dApp->m_pFieldWinSocket)
			g_pD3dApp->m_pFieldWinSocket->SendMsg(T_FC_CHARACTER_GET_REAL_WEAPON_INFO_OK, reinterpret_cast<char*>(&msg_Weapon), sizeof(msg_Weapon));

		//MessageBox(NULL, "Mouse macro detected. \r\nCode for support(0xAC150002)", "Inetpub Anti - Cheat", MB_OK | MB_ICONERROR);
		FDBG("AntiCheat :: Triggered in skill check!");
		//SendMessage(g_pD3dApp->GetHwnd(), WM_CLOSE, 0, 0);
		return 0;
	}
	//MessageBox(NULL, "Mouse macro detected. \r\nCode for support(0xAC150002)", "Inetpub Anti - Cheat", MB_OK | MB_ICONERROR);
	return CallNextHookEx(hHook, nCode, wParam, lParam);
}
*/
#ifdef _DEBUG
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  //DoSomethingWithData(Data, Size);
  return 0;
}
#endif
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{

#ifdef _DEBUG_CONSOLE
	AllocConsole();
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);
	//system("pause"); //for debugger attach
#endif
	SetUnhandledExceptionFilter(exception_minidump);
	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
		//hHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, hInstance, 0);
		/*hHook = SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, hInstance, 0);
	*/
	if(g_hMutexMonoInstance)
		return 0;

	g_hMutexMonoInstance = CreateMutex(nullptr, TRUE, STRMSG_WINDOW_TEXT);
	if(nullptr == g_hMutexMonoInstance)
		return 0;

#ifdef DISALLOW_MULTIPLE_CLIENT
	if(ERROR_ALREADY_EXISTS == ::GetLastError())
	{
		MessageBox(nullptr, "ERROR : \nApplication is running already...\n We does not support playing 2 account from the same PC\n Please use other PC if you want to play 2 accounts from the same IP!", STRMSG_WINDOW_TEXT, MB_OK);
		return 0;
	}
#endif // ALLOW_MULTIPLE_CLIENT
	
	CAtumApplication pD3dApp;

	if(__argc == 11)
		sscanf(lpCmdLine,"%s %d %s %d %s %s %d %d %d %d", pD3dApp.m_strFieldIP,\
			&pD3dApp.m_nFieldPort, pD3dApp.m_strChatIP, &pD3dApp.m_nChatPort,\
			pD3dApp.m_strUserID,pD3dApp.m_strUserPassword, &pD3dApp.m_IsFullMode,\
			&pD3dApp.m_nWidth, &pD3dApp.m_nHeight, &pD3dApp.m_bDegree);
	else {
		MessageBox(nullptr, "Exec parameters error! Use OldSchoolRivals.exe to start game!", STRMSG_WINDOW_TEXT, MB_OK);
		CloseHandle(g_hMutexMonoInstance);
#ifdef _AC_EXTENDED_THREAD
	#ifdef _M_IX86
			EvoAntiCheat::GetInstance()->StopDetection();
			CollisionsProcessor::GetInstance()->StopProcessing();
	#endif
#endif
		exit(1);
	}
#ifdef ONLY_FULL_WINDOW_HSSON
		pD3dApp.m_IsFullMode = TRUE;
#endif // ONLY_FULL_WINDOW_HSSON

	DbgOut("FullMode = %d\n",pD3dApp.m_IsFullMode);
	if( FAILED( pD3dApp.Create( hInstance ) ) )
		return 0;

	g_input.InitInput();

	const auto nResult = pD3dApp.Run();

	CloseHandle(g_hMutexMonoInstance);

	if(pD3dApp.m_bShutDown && strlen(pD3dApp.m_strMsgLastError))
		MessageBox(nullptr, pD3dApp.m_strMsgLastError,STRMSG_WINDOW_TEXT, MB_OK);
#ifdef _AC_EXTENDED_THREAD
	EvoAntiCheat::GetInstance()->StopDetection();
	CollisionsProcessor::GetInstance()->StopProcessing();
#endif
	return nResult;
}
