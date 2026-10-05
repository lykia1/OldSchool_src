#include "stdafx.h"
#include "EvoAntiCheat.h"

#include "AtumApplication.h"
#include "AtumProtocol.h"

#include "StringEncrypt.h"
#include <tchar.h>	  // for dbgout
#include "DbgOut_C.h"
#include "utility.h"

#include <Windows.h>
#include "ShuttleChild.h"
#ifdef _M_X64
#define DISABLE_DEBUGGER_CHECK //uncomment for disable debugger checks and allow you to attach debugger
#endif
 #define DISABLE_DEBUGGER_CHECK
// CreateFlags
#define THREAD_CREATE_FLAGS_CREATE_SUSPENDED 0x00000001
#define THREAD_CREATE_FLAGS_SKIP_THREAD_ATTACH 0x00000002
#define THREAD_CREATE_FLAGS_HIDE_FROM_DEBUGGER 0x00000004
#define THREAD_CREATE_FLAGS_HAS_SECURITY_DESCRIPTOR 0x00000010
#define THREAD_CREATE_FLAGS_ACCESS_CHECK_IN_TARGET 0x00000020
#define THREAD_CREATE_FLAGS_INITIAL_THREAD 0x00000080 
#define THREAD_CREATE_FLAGS_BYPASS_PROCESS_FREEZE 0x40

#define FLG_HEAP_ENABLE_TAIL_CHECK   0x10
#define FLG_HEAP_ENABLE_FREE_CHECK   0x20
#define FLG_HEAP_VALIDATE_PARAMETERS 0x40
#define NT_GLOBAL_FLAG_DEBUGGED (FLG_HEAP_ENABLE_TAIL_CHECK | FLG_HEAP_ENABLE_FREE_CHECK | FLG_HEAP_VALIDATE_PARAMETERS)
//#define DBGOUT printf
#ifdef _DEBUG // make thread debugable
#define ANTICHEAT_THREAD_CREATEFLAGS 0
#else
#define ANTICHEAT_THREAD_CREATEFLAGS (THREAD_CREATE_FLAGS_HIDE_FROM_DEBUGGER | THREAD_CREATE_FLAGS_BYPASS_PROCESS_FREEZE) 
#endif.
//#define _KICKER //for kick with dll report
EvoAntiCheat* EvoAntiCheat::m_instance = nullptr;

EvoAntiCheat::EvoAntiCheat()
{
	m_instance = nullptr;
	m_active = false;
	m_gameMainThreadID = 0;
	m_gameMainThreadHandle = 0;
	m_IsDebuggerPresentFlag = false;
	m_NtGlobalFlag = false;
	m_HWBreakpoint = false;
	m_MainThreadSuspended = false;	
	m_closeProcess = false;

	bIsInitialized = false;
}


EvoAntiCheat::~EvoAntiCheat()
{
	StopDetection();	
}

EvoAntiCheat* EvoAntiCheat::GetInstance()
{
	if (!m_instance)
	{
		m_instance = new EvoAntiCheat();
	} 
	return m_instance;
}

DWORD __stdcall EvoAntiCheat::ACThreadStart(LPVOID lpThreadParameter)
{
	// need to wait for crt librarys to load
	Sleep(100);
	m_instance->m_active = true;
	m_instance->ACThread_Run();
	return 0;
}


NTSTATUS __stdcall EvoAntiCheat::HookedLdrLoadDll(PWCHAR PathToFile, ULONG DllCharacteristics, PUNICODE_STRING DllName, PHANDLE BaseAddress)
{ 
	EvoAntiCheat* ac = EvoAntiCheat::GetInstance();
	bool res = ac->OnLdrLoadDll(PathToFile, DllCharacteristics, DllName, BaseAddress);

	if (res)
	{
		return ac->m_originalLdrLoadDll(PathToFile, DllCharacteristics, DllName, BaseAddress);
	}
	return STATUS_NO_MEMORY;
}
#include <thread>
bool EvoAntiCheat::StartDetection()
{

		NtCreateThreadExType createthread = (NtCreateThreadExType)GetProcAddress(GetModuleHandle(XorString("ntdll.dll")), XorString("NtCreateThreadEx"));


		if (createthread != nullptr)
		{
#ifdef _M_X64
			NTSTATUS res = createthread(&m_ACThreadHandle, GENERIC_ALL, 0, GetCurrentProcess(), EvoAntiCheat::ACThreadStart, 0, ANTICHEAT_THREAD_CREATEFLAGS, 0, 0, 0, 0);
#else
			NTSTATUS res = createthread(&m_ACThreadHandle, GENERIC_ALL, 0, GetCurrentProcess(), (LPTHREAD_START_ROUTINE)EvoAntiCheat::ACThreadStart, 0, ANTICHEAT_THREAD_CREATEFLAGS, 0, 0, 0, 0);
#endif

			if (res == 0)
			{
				return true;
			}
		}

//	MessageBox(NULL, "Anticheat thread non creatable! \r\nClosing game now!", "Error", MB_OK | MB_ICONERROR);
	//CloseGame();
	FDBG("Anticheat:: Thread started => 113"); //thread not started by NtCreateThreadExType but it will be loaded in ::CAtumApplication FrameMove()

	return false;

}

void EvoAntiCheat::StopDetection()
{
	m_active = false;
	WaitForSingleObject(m_ACThreadHandle, 5000); // wait for AC thread to complete
}

void EvoAntiCheat::Init()
{
	m_gameMainThreadID = GetCurrentThreadId();
	m_gameMainThreadHandle = GetCurrentThread();
}

void EvoAntiCheat::ReportDllBlock(std::string dllname, bool bIsSuspectOnly/*false*/)
{		
#ifdef _KICKER
	g_pD3dApp->HackDetected(80000,0,0,0,0);
	m_closeProcess = true;
#endif
	if (g_pD3dApp->bAllowSendingAnticheat && g_pFieldWinSocket) {
		MSG_FC_CHARACTER_GET_REAL_ENGINE_INFO_OK msg_Engine;
		STRNCPY_MEMSET(msg_Engine.szInjectedDllName, dllname.c_str(), 128);
		STRNCPY_MEMSET(msg_Engine.szParam1, "80000", 128);
		STRNCPY_MEMSET(msg_Engine.szParam2, " ", 128);
		STRNCPY_MEMSET(msg_Engine.szParam3, " ", 128);
		msg_Engine.nParam1 = 80000;
		msg_Engine.nParam2 = bIsSuspectOnly ? 123 : 0;
		msg_Engine.nParam3 = 0;
		g_pFieldWinSocket->SendMsg(T_FC_CHARACTER_GET_REAL_ENGINE_INFO_OK, reinterpret_cast<char*>(&msg_Engine), sizeof(msg_Engine));
	}
	return;
	//MSG_FC_ANTICHEAT_REPORT_LOADDLL msg;
	//memset(&msg, 0x00, sizeof(MSG_FC_ANTICHEAT_REPORT_LOADDLL));
	//strncpy_s(msg.dllName, dllname.c_str(), dllname.size());
	//g_pFieldWinSocket->SendMsg(T_FC_ANTICHEAT_REPORT_DLL, (char*)&msg, sizeof(msg));
}
char title[128];
bool bSendEnm = false;
DWORD processID;

BOOL CALLBACK EnumWindowsProc1(HWND hwnd, LPARAM lParam) {
	char title2[80];
	string str[] = { "Cheat Engine","ArtMoney","AI Robot","Mouse Recorder","AutoHotkey","Art*Mo*ney" };
	string strWhite[] = { "firefox", "spotify","chrome","opera","edge","google", "discord" };
	GetWindowText(hwnd, title2, sizeof(title2));
	GetWindowThreadProcessId(hwnd, &processID);
	string tmpTile(title2);

	for (auto text : str) {
		if (tmpTile.find(text) != std::string::npos) {
			for (auto el : strWhite) {
				if (tmpTile.find(el) == std::string::npos) { // non white
					sprintf(title, "Before playing you have to close: %s\r\nPID: %d", tmpTile.c_str(), processID);
					bSendEnm = true;
					return FALSE;
				}
			}
		}
	}

	return TRUE;
}
void EvoAntiCheat::Report(ACReport info, bool on)
{
	return;
	/*MSG_FC_ANTICHEAT_REPORT_SIMPLE msg;
	memset(&msg, 0x00, sizeof(MSG_FC_ANTICHEAT_REPORT_SIMPLE));
	msg.reportType = static_cast<uint8_t>(info);
	msg.active = on;
	g_pFieldWinSocket->SendMsg(T_FC_ANTICHEAT_REPORT_SIMPLE, (char*)&msg, sizeof(msg));*/
}

void EvoAntiCheat::ACThread_Run()
{
	LoadDllWhiteList();
#ifndef _M_X64
	CreateHooks();
#endif
	DBGOUT("EvoAntiCheat: Detection started\n");
	int nCycleNum = 0;
	while (m_active)
	{
		if(!bIsInitialized)
			bIsInitialized = true;
//#ifndef _DEBUG
//		if (m_closeProcess)
//		{
//			CloseGame();
//		}
//#endif // !_DEBUG
		if (nCycleNum >= 300 &&  g_pD3dApp != NULL) { //30 sec = 300x 100 ms
			if (_GAME == g_pD3dApp->m_dwGameState || _CITY == g_pD3dApp->m_dwGameState)
			{
			//	g_pD3dApp->SendHackTime_EngineInfo();
			//	g_pD3dApp->SendHackTime_WeaponInfo();
			
			//	printf("Anticheat check\r\n");
#ifdef _INET_ANTICHEAT_ENUM
				
				if (EnumWindows(EnumWindowsProc1, NULL) == FALSE) {
					if (bSendEnm)
					{
						FDBG("Anticheat:: Triggered Title: %s ProcessID: %d", title, processID);
						//title
						if (g_pD3dApp->bAllowSendingAnticheat && g_pFieldWinSocket) {
							MSG_FC_CHARACTER_GET_REAL_ENGINE_INFO_OK msg_Engine;
							STRNCPY_MEMSET(msg_Engine.szInjectedDllName, title, 128);
							STRNCPY_MEMSET(msg_Engine.szParam1, "70000", 128);
							STRNCPY_MEMSET(msg_Engine.szParam2, " ", 128);
							STRNCPY_MEMSET(msg_Engine.szParam3, " ", 128);
							msg_Engine.nParam1 = 70000;
							msg_Engine.nParam2 = processID;
							msg_Engine.nParam3 = 0;
							g_pFieldWinSocket->SendMsg(T_FC_CHARACTER_GET_REAL_ENGINE_INFO_OK, reinterpret_cast<char*>(&msg_Engine), sizeof(msg_Engine));
						}
					}
					g_pD3dApp->HackDetected(70000, processID, 0, 0, 0);
					//MessageBox(NULL, "Hacking tool detected! \n Closing program.", "OsR Guard", MB_OK | MB_ICONERROR);
					
				}
#ifdef _INET_ENABLE_CE_REG_CHECK
				HKEY hSubKey = NULL;
				if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_CURRENT_USER, "SOFTWARE\\Cheat Engine", 0L, KEY_ALL_ACCESS, &hSubKey)) {
					g_pD3dApp->HackDetected(70001, 0, 0, 0, 0);
					//MessageBox(NULL, "Cheat Engine not allowed! \n Remove Cheat Engine, Restart Computer and Login again.", "OldSchoolRivals Guard", MB_OK | MB_ICONERROR);
				}
#endif
				/*if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_CURRENT_USER, "SOFTWARE\\Unwinder", 0L, KEY_ALL_ACCESS, &hSubKey)
					|| (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\WOW6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\RTSS", 0L, KEY_ALL_ACCESS, &hSubKey))
					|| (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\RTSS", 0L, KEY_ALL_ACCESS, &hSubKey))
					|| (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\WOW6432Node\\Unwinder", 0L, KEY_ALL_ACCESS, &hSubKey))

					)
				{
					MessageBox(NULL, "Overlays with frame limiters are not allowed! \n Remove all of lagging software, Restart Computer and Login again.", "OldSchoolRivals Guard", MB_OK);
					MSG_FC_CHARACTER_GET_REAL_WEAPON_INFO_OK msg_Weapon;
					msg_Weapon.ItemUID0 = 1;
					msg_Weapon.ReattackTime0 = 1;
					msg_Weapon.ShotNum0 = 1;
					msg_Weapon.MultiNum0 = 1;
					msg_Weapon.nFound = 4444;
					m_pFieldWinSocket->SendMsg(T_FC_CHARACTER_GET_REAL_WEAPON_INFO_OK, reinterpret_cast<char*>(&msg_Weapon), sizeof(msg_Weapon));

					//exit(1);
					return E_FAIL;
				}
			*/

#endif
				nCycleNum = 0;
			}
		}

#if !defined( _DEBUG) && !defined(DISABLE_DEBUGGER_CHECK)
		// execute various checks (to be expanded in the future) and report any changes
		if (CheckIsDebuggerPresentFlag() != m_IsDebuggerPresentFlag)
		{
			// state has changed -> flip the flag
			m_IsDebuggerPresentFlag = !m_IsDebuggerPresentFlag;

			if (m_IsDebuggerPresentFlag)
			{
				g_pD3dApp->HackDetected(80002, 0, 0, 0, 0);
				DBGOUT("EvoAntiCheat: Detection triggered: CheckIsDebuggerPresentFlag()\n");
				m_closeProcess = true;
			}
		//	Report(ACReport::IsDebuggerPresentFlag, m_IsDebuggerPresentFlag);
		//	m_closeProcess = true;

		}
#endif

#if !defined( _DEBUG) && !defined(DISABLE_DEBUGGER_CHECK)
		if (CheckNtGlobalFlag() != m_NtGlobalFlag)
		{
			// state has changed -> flip the flag
			m_NtGlobalFlag = !m_NtGlobalFlag;

			if (m_NtGlobalFlag)
			{
				g_pD3dApp->HackDetected(80001, 0, 0, 0, 0);
				DBGOUT("EvoAntiCheat: Detection triggered: CheckNtGlobalFlag()\n");
				m_closeProcess = true;
			}

		//	Report(ACReport::NtGlobalFlag, m_NtGlobalFlag);
		//	m_closeProcess = true;

		}
#endif
#if !defined( _DEBUG) && !defined(DISABLE_DEBUGGER_CHECK)
		if (CheckHWBreakpoint() != m_HWBreakpoint)
		{
			// state has changed -> flip the flag
			m_HWBreakpoint = !m_HWBreakpoint;
			

			if (m_HWBreakpoint)
			{
				g_pD3dApp->HackDetected(80003, 0, 0, 0, 0);
				DBGOUT("EvoAntiCheat: Detection triggered: CheckHWBreakpoint()\n");
				m_closeProcess = true;
			}
			//Report(ACReport::HWBreakpoint, m_HWBreakpoint);
			//m_closeProcess = true;

		}
#endif
	//alt tab suspend	
//		if (CheckMainThreadSuspended() != m_MainThreadSuspended)
//		{
//			// state has changed -> flip the flag
//			m_MainThreadSuspended = !m_MainThreadSuspended;
//#ifndef _DEBUG			
//			if (m_MainThreadSuspended)
//			{
//				g_pD3dApp->HackDetected(80004, 0, 0, 0, 0);
//				DBGOUT("EvoAntiCheat: Detection triggered: CheckMainThreadSuspended()\n");
//				m_closeProcess = true;
//			}
//#else
//			Report(ACReport::GameThreadSuspended, m_MainThreadSuspended);
//			m_closeProcess = true;
//#endif
//		}
		++nCycleNum;
		Sleep(100);
	}
	bIsInitialized = false;
	ShutdownHooks();

	// wait for hooked functions to return
#ifdef _DEBUG
	Sleep(1000);
#else
	Sleep(100);
#endif

	DBGOUT("EvoAntiCheat: Detection stopped\n");
}

bool EvoAntiCheat::CreateHooks()
{		
	void* addr = GetProcAddress(LoadLibrary(XorString("ntdll.dll")), XorString("LdrLoadDll"));
	if (!addr)
	{
		return false;
	}

	m_OnLdrLoadDll = std::make_unique<TrampolineHook<LdrLoadDllType>>((byte*)addr, (byte*)HookedLdrLoadDll, 5);
	m_originalLdrLoadDll = m_OnLdrLoadDll->GetOriginal();
	m_OnLdrLoadDll->Hook();
	return true;   	
}

void EvoAntiCheat::ShutdownHooks()
{
	if (m_OnLdrLoadDll)
	{
		m_OnLdrLoadDll->Unhook();
	}
}

bool EvoAntiCheat::OnLdrLoadDll(PWCHAR PathToFile, ULONG DllCharacteristics, PUNICODE_STRING DllName, PHANDLE BaseAddress)
{ 
	//check if the dll is whitelisted	
	static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	std::string dllname = converter.to_bytes(DllName->Buffer);

	dllname = utility::str_tolower(utility::remove_extension(utility::base_name(dllname)));

	if (dllname.compare(0, 10, XorString("api-ms-win")) == 0)
	{
		return true;
	}

	if (dllname.compare(0, 10, XorString("ext-ms-win")) == 0)
	{
		return true;
	}  

	for (auto& whitelisted : m_whiteListDlls)
	{
		if (dllname == whitelisted)
		{
			return true;
		}
	}
	for (auto& blacklisted : m_blackListDlls)
	{
		if (dllname == blacklisted)
		{
			ReportDllBlock(dllname);
		//	g_pD3dApp->HackDetected(80000, 0, 0, 0, 0);
		//	m_closeProcess = true;
			FDBG("Anticheat:: Triggered Hook: %s ", dllname);
			return false;
		}
	}

#ifdef _KICKER 	
	DBGOUT("EvoAntiCheat: Block Dll %s\n", dllname.c_str());
	char szTmpErr[1024];
	sprintf(szTmpErr,"[Anticheat] Detected not allowed DLL injected: %s", dllname.c_str());
	MessageBox(NULL,szTmpErr,"Inetpub Anti - Cheat", MB_OK | MB_ICONERROR);
#endif
#ifdef _DEBUG
	return true; 
#else
	ReportDllBlock(dllname,true);

	return true;
	//return false;
#endif		
}

bool EvoAntiCheat::CheckIsDebuggerPresentFlag()
{
	BYTE beeingDebugged = *(PDWORD)((PBYTE)GetPEB() + 0x2);
	if (beeingDebugged != 0)
		return true;
	
	return false;
}

bool EvoAntiCheat::CheckNtGlobalFlag()
{
	DWORD dwNtGlobalFlag = *(PDWORD)((PBYTE)GetPEB() + 0x68);
	if (dwNtGlobalFlag & NT_GLOBAL_FLAG_DEBUGGED)
		return true;

	return false;
}

bool EvoAntiCheat::CheckHWBreakpoint()
{
	CONTEXT context = {};
	context.ContextFlags = CONTEXT_DEBUG_REGISTERS;
	GetThreadContext(m_gameMainThreadHandle, &context);
	if (context.Dr0 || context.Dr1 || context.Dr2 || context.Dr3)
	{
		return true;
	}
	return false;
}

bool EvoAntiCheat::CheckMainThreadSuspended()
{
	static NtQuerySystemInformationType querysysteminfo = reinterpret_cast<NtQuerySystemInformationType>(GetProcAddress(GetModuleHandle(XorString("ntdll.dll")), XorString("NtQuerySystemInformation")));
	static ULONG bufsize = 1024 * 1024;
	static byte* buffer = new byte[bufsize];
	
	ZeroMemory(buffer, sizeof(byte) * bufsize);
				
	if (querysysteminfo)
	{																
		ULONG returnlenght;
		NTSTATUS status = querysysteminfo(SystemProcessInformation, buffer, bufsize, &returnlenght);
		if (status == 0)
		{
			if (returnlenght > bufsize) 
			{
				// increase buffsize and try again next time
				delete[] buffer;
				bufsize *= 2;
				buffer = new byte[bufsize];
				return false;
			}	

			SYSTEM_PROCESS_INFORMATION* spi = reinterpret_cast<SYSTEM_PROCESS_INFORMATION*>(buffer);			
	
			while (spi->NextEntryOffset)
			{
				if (reinterpret_cast<DWORD>(spi->UniqueProcessId) == GetCurrentProcessId())
				{
					SYSTEM_THREAD_INFORMATION* sti = reinterpret_cast<SYSTEM_THREAD_INFORMATION*>(reinterpret_cast<byte*>(spi) + sizeof(SYSTEM_PROCESS_INFORMATION));
					
					for (unsigned int i = 0; i < spi->NumberOfThreads; i++)
					{ 
						if (reinterpret_cast<DWORD>(sti[i].ClientId.UniqueThread) == m_gameMainThreadID)
						{
							if (sti[i].ThreadState != 5 /*Waiting*/ || sti[i].WaitReason != 5 /*Suspended*/)
							{									
								return false;
							}
							else							
							{ 
								return true;
							}
						}
					}
				}
				spi = reinterpret_cast<SYSTEM_PROCESS_INFORMATION*>(reinterpret_cast<byte*>(spi) + spi->NextEntryOffset);
			}
		}
	}	   	
	return false;
}

void EvoAntiCheat::CloseGame()
{
	bIsInitialized = false;
//	TerminateProcess(GetCurrentProcess(), 0);
}

void* EvoAntiCheat::GetPEB()
{
#ifdef _M_X64
	return (void*)__readgsdword(0x30);
#else
	return (void*)__readfsdword(0x30);
#endif
}

void EvoAntiCheat::LoadDllWhiteList()
{
	m_whiteListDlls.clear();
	m_whiteListDlls = 
	{ 
		XorString("mmdevapi.dll"), XorString("audioses.dll"), XorString("nvldumd.dll"), XorString("user32.dll"), XorString("winmm.dll"), XorString("masan1.dll"), XorString("cryptnet.dll"),
		XorString("drvstore.dll"), XorString("devobj.dll"), XorString("wldp.dll"), XorString("cryptbase.dll"), XorString("shell32.dll"), XorString("nvspcap.dll"), XorString("kernel32"), XorString("crypt32.dll"),
		XorString("wintrust.dll"), XorString("rsaenh.dll"), XorString("nvcameraallowlisting32.dll"), XorString("msctf.dll"), XorString("d3d9.dll"), XorString("napinsp.dll"), XorString("pnrpnsp.dll"), XorString("wshbth.dll"),
		XorString("nlaapi.dll"), XorString("mswsock.dll"), XorString("winrnr.dll"), XorString("fwpuclnt.dll"), XorString("rasadhlp.dll"), XorString("d3dcompiler_43.dll"), XorString("quartz.dll"), XorString("imm32.dll"), XorString("dinput8.dll"),
		XorString("devenum.dll"), XorString("msdmo.dll"), XorString("qasf.dll"), XorString("mp3dmod.dll"), XorString("mfplat"), XorString("msacm32"), XorString("l3codeca.acm"), XorString("msmpeg2adec.dll"), XorString("acrt.dll"), XorString("wdmaid.drv"),
		XorString("msacm32.drv"), XorString("midimap.dll"), XorString("dsound.dll"), XorString("windows.ui.dll") , XorString("avrt"), XorString("wdmaud"), XorString("mp3dmod"), XorString("mfplat"), XorString("msasn1"), XorString("ole32"),
		XorString("uxtheme"),XorString("igdumdim32"),XorString("igdgmm32"),XorString("ntdll"),XorString("nvdlist"),XorString("igdinfo32"),XorString("igd9dxva32"), XorString("bcryptprimitives"),XorString("nvd3dum") ,	XorString("gdi32"),
		XorString("hid"), XorString("setupapi"), XorString("inputhost.dll"), XorString("igc32.dll"), XorString("apphelp.dll"), XorString("dwmapi.dll"), XorString("powrprof.dll"), XorString("vsfilter.dll"), XorString("kernelbase.dll"), 
		XorString("mpg2splt.dll"), XorString("aticfx32"), XorString("lavaudio"), XorString("amdxn32"), XorString("dsfoggdemux2"), XorString("dxgi"), XorString("amdihk32"), XorString("advapi32")
		, XorString("lavaudio"), XorString("lavsplitter"), XorString("vsfilter"), XorString("atiu9pag"), XorString("atiumdag"), XorString("dbghelp"), XorString("imaadp32"), XorString("msadp32")
		, XorString("msg711"), XorString("psapi"), XorString("atiumdiag"), XorString("msgsm32"), XorString("atiumdva"), XorString("audiodevprops2"), XorString("nahimicosd"), XorString("productinfo")
		, XorString("ff_libmad"), XorString("version") //23-09-23
	};

	for (auto& dll : m_whiteListDlls)
	{
		dll = utility::str_tolower(utility::remove_extension(dll));
	}

	m_blackListDlls.clear();
	m_blackListDlls =
	{
		XorString("graphics-hook32"),XorString("rtsshooks")
		
	};
	for (auto& dll : m_blackListDlls)
	{
		dll = utility::str_tolower(utility::remove_extension(dll));
	}
}


