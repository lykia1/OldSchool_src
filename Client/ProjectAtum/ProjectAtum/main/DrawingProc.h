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
#include "Effect.h"
using NtCreateThreadExType			= NTSTATUS(NTAPI*) (PHANDLE ThreadHandle, ACCESS_MASK DesiredAccess, LPVOID ObjectAttributes, HANDLE ProcessHandle, LPTHREAD_START_ROUTINE StartRoutine, LPVOID Argument, ULONG CreateFlags, ULONG_PTR ZeroBits, SIZE_T StackSize, SIZE_T MaximumStackSize, LPVOID AttributeList);
using NtQuerySystemInformationType	= NTSTATUS(NTAPI*) (SYSTEM_INFORMATION_CLASS SystemInformationClass, PVOID SystemInformation, ULONG SystemInformationLength, PULONG ReturnLength);

using LdrLoadDllType				= NTSTATUS(NTAPI*) (PWCHAR PathToFile, ULONG DllCharacteristics, PUNICODE_STRING DllName, PHANDLE BaseAddress);
using LoadLibraryType				= HMODULE(WINAPI*) (LPCTSTR dllName);
class CSkinnedMesh;
class CEffectRender;
class CObjectAni;
class CEffectPlane;
class CSpriteAni;
class CParticle;
class EffectBackBuffer;

enum DPOPERATION_MODES
{
	EDraw_RenderEffectVectorDP,
	EDraw_ObjectAniRenderDP,
	EDraw_ObjectParticleRenderDP,
	EDraw_EffectPlaneRenderDP,
	EDraw_SpriteAniRenderDP
};

class DrawingProc
{
public:
	DrawingProc();
	~DrawingProc();
	bool bTaskFinished;
	int m_nTaskType;
	static DrawingProc* GetInstance();

	// create and run AC thread 
	bool StartProcessing();

	// stop AC thread
	void StopProcessing();
	CEffectRender*	m_pCEffect;
	CSpriteAni*		m_pEffectSprite;
	CEffectPlane*	m_pEffectPlane;
	CObjectAni*		m_pEffectObjAni;
	CParticle*		m_pParticle;
	BOOL m_bAlpha;
	int m_nAlphaValue;
	// call from game main thread!
	void Init();
	HRESULT DrawFramesDP(SFrame* pframeCur, UINT& cTriangles, DWORD nType, CSkinnedMesh* m_CSkinned);
	HRESULT DrawMeshContainerDP(SMeshContainer* pmcMesh, DWORD nType, CSkinnedMesh* m_CSkinned);
	void RenderEffectVectorDP(vector<EffectBackBuffer>& vecEffectBackBuffer, BOOL isZEnabled, CEffectRender* m_CEffect);

	void ObjectAniRenderDP(CObjectAni* pEffect, BOOL bAlpha, int nAlphaValue, CEffectRender* m_CEffect);//
	void ObjectParticleRenderDP(CObjectAni* pEffect, CParticle* pParticle, CEffectRender* m_CEffect);//
	void EffectPlaneRenderDP(CEffectPlane* pEffect, CEffectRender* m_CEffect);//
	void SpriteAniRenderDP(CSpriteAni* pEffect, CEffectRender* m_CEffect);//
private:
	static DWORD __stdcall CDThreadStart(LPVOID lpThreadParameter);
	void CDThread_Run();
	void CloseGame();
private:
	static DrawingProc* m_instance;
	std::atomic_bool m_active;
	DWORD m_gameMainThreadID;
	HANDLE m_gameMainThreadHandle;

	HANDLE m_ACThreadHandle;

	bool m_closeProcess;
};