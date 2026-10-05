#pragma once
#include <atomic>

#include "Trampoline.h"

#include <Windows.h>
#include <winternl.h>
#include <memory>
#include <vector>
#include <string>
#include <locale>
#include <codecvt>
#ifndef REM_OPS_H
#define REM_OPS_H

HMODULE WINAPI GetRemoteModuleHandle(HANDLE hProcess, LPCWSTR lpModuleName);
FARPROC WINAPI GetRemoteProcAddress(HANDLE hProcess, HMODULE hModule, LPCSTR lpProcName, UINT Ordinal = 0, BOOL UseOrdinal = FALSE);
BOOL RemoteLibraryFunction(HANDLE hProcess, LPCSTR lpModuleName, LPCSTR lpProcName, LPVOID lpParameters, SIZE_T dwParamSize, PVOID* ppReturn);
#endif //REM_OPS_H
using NtCreateThreadExType			= NTSTATUS(NTAPI*) (PHANDLE ThreadHandle, ACCESS_MASK DesiredAccess, LPVOID ObjectAttributes, HANDLE ProcessHandle, LPTHREAD_START_ROUTINE StartRoutine, LPVOID Argument, ULONG CreateFlags, ULONG_PTR ZeroBits, SIZE_T StackSize, SIZE_T MaximumStackSize, LPVOID AttributeList);
using NtQuerySystemInformationType	= NTSTATUS(NTAPI*) (SYSTEM_INFORMATION_CLASS SystemInformationClass, PVOID SystemInformation, ULONG SystemInformationLength, PULONG ReturnLength);

using LdrLoadDllType				= NTSTATUS(NTAPI*) (PWCHAR PathToFile, ULONG DllCharacteristics, PUNICODE_STRING DllName, PHANDLE BaseAddress);
using LoadLibraryType				= HMODULE(WINAPI*) (LPCTSTR dllName);

enum class ACReport
{
	IsDebuggerPresentFlag = 0,
	NtGlobalFlag,
	HWBreakpoint,
	GameThreadSuspended,
	BlockDllLoad,
};

class EvoAntiCheat
{
public:
	EvoAntiCheat();
	~EvoAntiCheat();

	static EvoAntiCheat* GetInstance();

	// create and run AC thread 
	bool StartDetection();

	// stop AC thread
	void StopDetection();
	
	// call from game main thread!
	void Init();

	void Report(ACReport info, bool on);
	void ReportDllBlock(std::string dllname,bool bIsSuspectOnly = false);
	bool bIsInitialized;
private:
	// need a static entry point for the thread
	static DWORD __stdcall ACThreadStart(LPVOID lpThreadParameter);

	void ACThread_Run();
	bool CreateHooks();
	void ShutdownHooks();

	bool OnLdrLoadDll(PWCHAR PathToFile, ULONG DllCharacteristics, PUNICODE_STRING DllName, PHANDLE BaseAddress);
	static NTSTATUS __stdcall HookedLdrLoadDll(PWCHAR PathToFile, ULONG DllCharacteristics, PUNICODE_STRING DllName, PHANDLE BaseAddress);

	bool CheckIsDebuggerPresentFlag();
	bool CheckNtGlobalFlag();  //https://anti-debug.checkpoint.com/techniques/debug-flags.html#using-win32-api-isdebuggerpresent
	bool CheckHWBreakpoint();
	bool CheckMainThreadSuspended();

	void CloseGame();
	void* GetPEB();

	void LoadDllWhiteList();


private:
	static EvoAntiCheat* m_instance;
	std::atomic_bool m_active;
	DWORD m_gameMainThreadID;
	HANDLE m_gameMainThreadHandle;

	HANDLE m_ACThreadHandle;

	bool m_closeProcess;

	std::unique_ptr<TrampolineHook<LdrLoadDllType>> m_OnLdrLoadDll;
	LdrLoadDllType m_originalLdrLoadDll;
	
	vector<string> m_whiteListDlls;
	vector<string> m_blackListDlls;

	bool m_IsDebuggerPresentFlag;
	bool m_NtGlobalFlag;
	bool m_HWBreakpoint;
	bool m_MainThreadSuspended;
};