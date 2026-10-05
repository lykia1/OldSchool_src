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
#include "SkinnedMesh.h"
using NtCreateThreadExType			= NTSTATUS(NTAPI*) (PHANDLE ThreadHandle, ACCESS_MASK DesiredAccess, LPVOID ObjectAttributes, HANDLE ProcessHandle, LPTHREAD_START_ROUTINE StartRoutine, LPVOID Argument, ULONG CreateFlags, ULONG_PTR ZeroBits, SIZE_T StackSize, SIZE_T MaximumStackSize, LPVOID AttributeList);
using NtQuerySystemInformationType	= NTSTATUS(NTAPI*) (SYSTEM_INFORMATION_CLASS SystemInformationClass, PVOID SystemInformation, ULONG SystemInformationLength, PULONG ReturnLength);

using LdrLoadDllType				= NTSTATUS(NTAPI*) (PWCHAR PathToFile, ULONG DllCharacteristics, PUNICODE_STRING DllName, PHANDLE BaseAddress);
using LoadLibraryType				= HMODULE(WINAPI*) (LPCTSTR dllName);
class CSkinnedMesh;

enum OPERATION_MODES
{
	COper_CheckColl,
	COper_CheckCollDistCP,
	COper_CheckCollDistDetailCP,
	COper_PickCP,
	COper_CheckCollisionCP,
	COper_CheckCollDistCP2
};


class CollisionsProcessor
{
public:
	CollisionsProcessor();
	~CollisionsProcessor();

	static CollisionsProcessor* GetInstance();

	// create and run AC thread 
	bool StartProcessing();

	// stop AC thread
	void StopProcessing();
	bool bCPInitialized;
	void LoadIcons();
	bool bTaskFinished;
	int m_nTaskType;
	bool m_bLoadedIcons;
	// call from game main thread!
	void Init();
	COLLISION_RESULT CheckColl(D3DXMATRIX mat, D3DXVECTOR3 vPos, float fCheckDistance, BOOL bUpdateFrame, BOOL bWithNormal, CSkinnedMesh * m_CSkinned);
	COLLISION_RESULT CheckCollDistCP(SFrame* pframeCur, D3DXMATRIX mat, D3DXVECTOR3 vPos, float fCheckDistance, BOOL bWithNormal, CSkinnedMesh* m_CSkinned);
	COLLISION_RESULT CheckCollDistDetailCP(SMeshContainer* pmcMesh, const D3DXMATRIX& mat, BOOL bWithNormal, CSkinnedMesh* m_CSkinned);
	COLLISION_RESULT PickCP(SMeshContainer* pmcMesh, float fx, float fy, CSkinnedMesh* m_CSkinned);
	COLLISION_RESULT CheckCollisionCP(float fx, float fy, BOOL bUpdateFrame, CSkinnedMesh* m_CSkinned);
	COLLISION_RESULT CheckCollDistCP(SFrame* pframeCur, float fx, float fy, CSkinnedMesh* m_CSkinned);
	//req params to run functions
	CSkinnedMesh* m_pCSkinned;
	D3DXMATRIX m_mat;
	D3DXVECTOR3 m_vPos;
	float m_fCheckDistance;
	BOOL m_bUpdateFrame;
	BOOL m_bWithNormal;
	SFrame* m_pframeCur;
	SMeshContainer* m_pmcMesh;
	D3DXMATRIX* m_matCnst;
	float m_fx;
	float m_fy;
	void InitTaskHelper(int nTaskType);
	COLLISION_RESULT m_nCollResult;
private:
	static DWORD __stdcall CDThreadStart(LPVOID lpThreadParameter);
	void CDThread_Run();
	void CloseGame();
private:
	static CollisionsProcessor* m_instance;
	std::atomic_bool m_active;
	DWORD m_gameMainThreadID;
	HANDLE m_gameMainThreadHandle;

	HANDLE m_ACThreadHandle;

	bool m_closeProcess;
};