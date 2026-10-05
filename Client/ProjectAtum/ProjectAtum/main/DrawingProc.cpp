#include "stdafx.h"
#include "DrawingProc.h"

#include "AtumApplication.h"
#include "AtumProtocol.h"

#include "StringEncrypt.h"
#include <tchar.h>	  // for dbgout
#include "DbgOut_C.h"
#include "utility.h"

#include <Windows.h>
#include "ShuttleChild.h"
#include "SkinnedMesh.h"
#include "Interface.h"
#include "AtumDatabase.h"
#include "Camera.h"
#include "SceneData.h"
#include "Effect.h"
#include "EffectRender.h"
#include "ObjectAniData.h"
#include "SpriteAniData.h"
#include "SpriteDrawFix.h"
#include "ParticleSystem.h"
#include "TraceAni.h"
#include "INFGameMain.h"
#include "ObjRender.h"
#define DISABLE_DEBUGGER_CHECK //uncomment for disable debugger checks and allow you to attach debugger

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

#define CD_THREAD_CREATEFLAGS (THREAD_CREATE_FLAGS_HIDE_FROM_DEBUGGER | THREAD_CREATE_FLAGS_BYPASS_PROCESS_FREEZE) 

//#define _KICKER //for kick with dll report
DrawingProc* DrawingProc::m_instance = nullptr;

DrawingProc::DrawingProc()
{
	m_instance = nullptr;
	m_active = false;
	m_gameMainThreadID = 0;
	m_gameMainThreadHandle = 0;
	m_closeProcess = false;
	bTaskFinished = true;
	m_nTaskType = -1;
}


DrawingProc::~DrawingProc()
{
	StopProcessing();	
}

DrawingProc* DrawingProc::GetInstance()
{
	if (!m_instance)
	{
		m_instance = new DrawingProc();
	} 
	return m_instance;
}

DWORD __stdcall DrawingProc::CDThreadStart(LPVOID lpThreadParameter)
{
	// need to wait for crt librarys to load
	Sleep(100);
	m_instance->m_active = true;
	m_instance->CDThread_Run();
	return 0;
}

bool DrawingProc::StartProcessing()
{
		NtCreateThreadExType createthread = (NtCreateThreadExType)GetProcAddress(GetModuleHandle(XorString("ntdll.dll")), XorString("NtCreateThreadEx"));

		if (createthread != nullptr)
		{
			NTSTATUS res = createthread(&m_ACThreadHandle, GENERIC_ALL, 0, GetCurrentProcess(), (LPTHREAD_START_ROUTINE)DrawingProc::CDThreadStart, 0, CD_THREAD_CREATEFLAGS, 0, 0, 0, 0);

			if (res == 0)
			{
				return true;
			}
		}
	return false;
}

void DrawingProc::StopProcessing()
{
	m_active = false;
	WaitForSingleObject(m_ACThreadHandle, 5000); // wait for AC thread to complete
}

void DrawingProc::Init()
{
	m_gameMainThreadID = GetCurrentThreadId();
	m_gameMainThreadHandle = GetCurrentThread();
}

void DrawingProc::CDThread_Run()
{
	DBGOUT("DrawingProc: Processing started\n");
	int nCycleNum = 0;
	while (m_active)
	{
		/*if (!bTaskFinished && m_nTaskType >= EDraw_RenderEffectVectorDP)
		{
			switch (m_nTaskType)
			{
				case EDraw_SpriteAniRenderDP:
					SpriteAniRenderDP(m_pEffectSprite, m_pCEffect);
					bTaskFinished = true;
					break;
				case EDraw_EffectPlaneRenderDP:
					EffectPlaneRenderDP(m_pEffectPlane, m_pCEffect);
					bTaskFinished = true;
					break;
				case EDraw_ObjectParticleRenderDP:
					ObjectParticleRenderDP(m_pEffectObjAni, m_pParticle, m_pCEffect);
					bTaskFinished = true;
					break;
				case EDraw_ObjectAniRenderDP:
					ObjectAniRenderDP(m_pEffectObjAni, m_bAlpha, m_nAlphaValue, m_pCEffect);
					bTaskFinished = true;
					break;
			}
			continue;
		}*/
#ifndef _DEBUG
		if (m_closeProcess)
		{
			CloseGame();

		}
#endif // !_DEBUG
		
	}

	Sleep(100);

	DBGOUT("DrawingProc: Processing stopped\n");
}

void DrawingProc::CloseGame()
{
	//TerminateProcess(GetCurrentProcess(), 0);
}
HRESULT DrawingProc::DrawFramesDP(SFrame* pframeCur, UINT& cTriangles, DWORD nType, CSkinnedMesh* m_CSkinned)
{
	if (pframeCur->pmcMesh != nullptr)
	{
		auto hr = m_CSkinned->m_pd3dDevice->SetTransform(D3DTS_WORLD, &pframeCur->matCombined);
		if (FAILED(hr)) return hr;
	}

	D3DXVECTOR3 vMeshPos;
	D3DXMATRIX matTemp;

	for (auto pmcMesh = pframeCur->pmcMesh; pmcMesh != nullptr; pmcMesh = pmcMesh->pmcNext)
	{
		auto hr = m_CSkinned->DrawMeshContainer(pmcMesh, nType);
		if (FAILED(hr)) return hr;
		cTriangles += pmcMesh->pMesh->GetNumFaces();
	}

	for (auto pframeChild = pframeCur->pframeFirstChild; pframeChild != nullptr; pframeChild = pframeChild->pframeSibling)
	{
		auto pmcMesh = pframeChild->pmcMesh;

		auto bResult = true;

		if (g_bDetailDrawFrame == TRUE && pmcMesh != nullptr && m_CSkinned->m_bProgressiveMesh == false)
		{
			D3DXMatrixIdentity(&matTemp);
			matTemp._41 = pmcMesh->vCenter.x;
			matTemp._42 = pmcMesh->vCenter.y;
			matTemp._43 = pmcMesh->vCenter.z;

			D3DXMatrixMultiply(&matTemp, &matTemp, &m_CSkinned->m_mWorld);

			vMeshPos.x = matTemp._41;
			vMeshPos.y = matTemp._42;
			vMeshPos.z = matTemp._43;

			if (D3DXVec3Length(&(vMeshPos - g_pCamera->m_vCamCurrentPos)) - pmcMesh->fRadius > g_pScene->m_fFogEndValue) bResult = false;
		}

		if (bResult)
		{
			auto hr = m_CSkinned->DrawFrames(pframeChild, cTriangles, nType);
			if (FAILED(hr)) return hr;
		}
	}

	return S_OK;
}

HRESULT DrawingProc::DrawMeshContainerDP(SMeshContainer* pmcMesh, DWORD nType, CSkinnedMesh* m_CSkinned)
{
	UINT ipattr;

	if (pmcMesh->m_pSkinMeshInfo != nullptr)
	{
		// D3DXMATRIX mat;
		LPD3DXBONECOMBINATION pBoneComb;
		DWORD AttribIdPrev;

		// 2005-01-05 by jschoi
		//		if (m_method != pmcMesh->m_Method)
		//		{
		//			GenerateMesh(pmcMesh);
		//		}

		if (m_CSkinned->m_method == D3DNONINDEXED)
		{
			AttribIdPrev = UNUSED32;
			pBoneComb = reinterpret_cast<LPD3DXBONECOMBINATION>(pmcMesh->m_pBoneCombinationBuf->GetBufferPointer());
			// Draw using default vtx processing of the device (typically HW)
			for (ipattr = 0; ipattr < pmcMesh->cpattr; ipattr++)
			{
				DWORD numBlend = 0;

				for (DWORD i = 0; i < pmcMesh->m_maxFaceInfl; ++i)

					if (pBoneComb[ipattr].BoneId[i] != UINT_MAX) numBlend = i;

				if (m_CSkinned->m_d3dCaps.MaxVertexBlendMatrices >= numBlend + 1)
				{
					for (DWORD i = 0; i < pmcMesh->m_maxFaceInfl; ++i)
					{
						auto matid = pBoneComb[ipattr].BoneId[i];
						if (matid != UINT_MAX)
						{
							m_CSkinned->m_pd3dDevice->SetTransform(D3DTS_WORLDMATRIX(i), pmcMesh->m_pBoneMatrix[matid]);
							m_CSkinned->m_pd3dDevice->MultiplyTransform(D3DTS_WORLDMATRIX(i), &pmcMesh->m_pBoneOffsetMat[matid]);
						}
					}

					m_CSkinned->m_pd3dDevice->SetRenderState(D3DRS_VERTEXBLEND, numBlend);

					if ((AttribIdPrev != pBoneComb[ipattr].AttribId) || (AttribIdPrev == UNUSED32))
					{
						m_CSkinned->m_pd3dDevice->SetMaterial(&(pmcMesh->rgMaterials[pBoneComb[ipattr].AttribId]));
						if (pmcMesh->pTextures[pBoneComb[ipattr].AttribId])
							m_CSkinned->m_pd3dDevice->SetTexture(0, pmcMesh->pTextures[pBoneComb[ipattr].AttribId]);
						else
							m_CSkinned->m_pd3dDevice->SetTexture(0, m_CSkinned->m_pTexture[m_CSkinned->m_bTextureNum - 1]);
						AttribIdPrev = pBoneComb[ipattr].AttribId;
					}

					pmcMesh->pMesh->DrawSubset(ipattr);
				//	g_pApp->m_iDrawSubsetCallCount++;
				}
			}

			// If necessary, draw parts that HW could not handle using SW
			if (pmcMesh->iAttrSplit < pmcMesh->cpattr)
			{
				AttribIdPrev = UNUSED32;
				// 2005-01-04 by jschoi
				m_CSkinned->m_pd3dDevice->SetSoftwareVertexProcessing(TRUE);

				for (ipattr = pmcMesh->iAttrSplit; ipattr < pmcMesh->cpattr; ipattr++)
				{
					DWORD numBlend = 0;
					for (DWORD i = 0; i < pmcMesh->m_maxFaceInfl; ++i)
					{
						if (pBoneComb[ipattr].BoneId[i] != UINT_MAX)
						{
							numBlend = i;
						}
					}

					if (m_CSkinned->m_d3dCaps.MaxVertexBlendMatrices < numBlend + 1)
					{
						for (DWORD i = 0; i < pmcMesh->m_maxFaceInfl; ++i)
						{
							DWORD matid = pBoneComb[ipattr].BoneId[i];
							if (matid != UINT_MAX)
							{
								m_CSkinned->m_pd3dDevice->SetTransform(D3DTS_WORLDMATRIX(i), pmcMesh->m_pBoneMatrix[matid]);
								m_CSkinned->m_pd3dDevice->MultiplyTransform(D3DTS_WORLDMATRIX(i), &pmcMesh->m_pBoneOffsetMat[matid]);
							}
						}

						m_CSkinned->m_pd3dDevice->SetRenderState(D3DRS_VERTEXBLEND, numBlend);

						if ((AttribIdPrev != pBoneComb[ipattr].AttribId) || (AttribIdPrev == UNUSED32))
						{
							m_CSkinned->m_pd3dDevice->SetMaterial(&(pmcMesh->rgMaterials[pBoneComb[ipattr].AttribId]));

							if (pmcMesh->pTextures[pBoneComb[ipattr].AttribId])
							{
								m_CSkinned->m_pd3dDevice->SetTexture(0, pmcMesh->pTextures[pBoneComb[ipattr].AttribId]);
							}
							else
							{
								m_CSkinned->m_pd3dDevice->SetTexture(0, m_CSkinned->m_pTexture[m_CSkinned->m_bTextureNum - 1]);
							}
							AttribIdPrev = pBoneComb[ipattr].AttribId;
						}

						// 2005-01-05 by jschoi 블렌딩 메시 현재 사용 안함
						// 						if(g_pD3dApp->m_pShuttleChild && g_pD3dApp->m_dwGameState == _GAME && m_bCheckBlend)
						//						{
						//							SetMeshRenderState(pmcMesh);
						//						}
						pmcMesh->pMesh->DrawSubset(ipattr);
						//	g_pApp->m_iDrawSubsetCallCount++;
					}
				}
				// 2005-01-04 by jschoi
				m_CSkinned->m_pd3dDevice->SetSoftwareVertexProcessing(FALSE);
			}
			m_CSkinned->m_pd3dDevice->SetRenderState(D3DRS_VERTEXBLEND, 0);
		}
		else if (m_CSkinned->m_method == D3DINDEXEDVS) // VertexShader를 사용할 경우
		{
			// Use COLOR instead of UBYTE4 since Geforce3 does not support it
			// vConst.w should be 3, but due to about hack, mul by 255 and add epsilon
			D3DXVECTOR4 vConst(1.0f, 0.0f, 0.0f, 765.01f);
			LPDIRECT3DVERTEXBUFFER9 pVB;
			LPDIRECT3DINDEXBUFFER9 pIB;

			if (pmcMesh->m_bUseSW)
			{
				// 2005-01-04 by jschoi
				m_CSkinned->m_pd3dDevice->SetSoftwareVertexProcessing(TRUE);
			}

			pmcMesh->pMesh->GetVertexBuffer(&pVB);
			pmcMesh->pMesh->GetIndexBuffer(&pIB);
			auto hr = m_CSkinned->m_pd3dDevice->SetStreamSource(0, pVB, 0, D3DXGetFVFVertexSize(pmcMesh->pMesh->GetFVF()));
			if (FAILED(hr)) return hr;
			hr = m_CSkinned->m_pd3dDevice->SetIndices(pIB);
			if (FAILED(hr)) return hr;
			pVB->Release();
			pIB->Release();
			hr = m_CSkinned->m_pd3dDevice->SetFVF(m_CSkinned->m_dwIndexedVertexShader[pmcMesh->m_maxFaceInfl - 1]);
			if (FAILED(hr)) return hr;

			pBoneComb = reinterpret_cast<LPD3DXBONECOMBINATION>(pmcMesh->m_pBoneCombinationBuf->GetBufferPointer());
			for (ipattr = 0; ipattr < pmcMesh->cpattr; ipattr++)
			{
				for (DWORD i = 0; i < pmcMesh->m_paletteSize; ++i)
				{
					auto matid = pBoneComb[ipattr].BoneId[i];
					if (matid != UINT_MAX)
					{
						D3DXMATRIXA16 mat;
						D3DXMatrixMultiply(&mat, &pmcMesh->m_pBoneOffsetMat[matid], pmcMesh->m_pBoneMatrix[matid]);
					}
				}

				// Sum of all ambient and emissive contribution
				D3DXCOLOR ambEmm;

				auto ambient = D3DXCOLOR(pmcMesh->rgMaterials[pBoneComb[ipattr].AttribId].Ambient);
				auto multiplier = D3DXCOLOR(.25, .25, .25, 1.0);

				D3DXColorModulate(&ambEmm, &ambient, &multiplier);

				ambEmm += D3DXCOLOR(pmcMesh->rgMaterials[pBoneComb[ipattr].AttribId].Emissive);
				vConst.y = pmcMesh->rgMaterials[pBoneComb[ipattr].AttribId].Power;

				if (pmcMesh->pTextures[pBoneComb[ipattr].AttribId])

					m_CSkinned->m_pd3dDevice->SetTexture(0, pmcMesh->pTextures[pBoneComb[ipattr].AttribId]);

				else m_CSkinned->m_pd3dDevice->SetTexture(0, m_CSkinned->m_pTexture[m_CSkinned->m_bTextureNum - 1]);

				// 2005-01-05 by jschoi - 블렌딩 메시 사용안함
				// 				if(g_pD3dApp->m_pShuttleChild && g_pD3dApp->m_dwGameState == _GAME && m_bCheckBlend)
				//				{
				//					SetMeshRenderState(pmcMesh);
				//				}

				m_CSkinned->m_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, pBoneComb[ipattr].VertexStart, pBoneComb[ipattr].VertexCount, pBoneComb[ipattr].FaceStart * 3, pBoneComb[ipattr].FaceCount);
			}

			// VertexProcessing 복원
			if (pmcMesh->m_bUseSW) m_CSkinned->m_pd3dDevice->SetSoftwareVertexProcessing(FALSE);
		}
		else if (m_CSkinned->m_method == D3DINDEXED)
		{
			// Vertex processing이 지원되지 않는 그래픽 카드라면 Software Vertex Processing 사용
			if (pmcMesh->m_bUseSW) m_CSkinned->m_pd3dDevice->SetSoftwareVertexProcessing(TRUE);

			// set the number of vertex blend indices to be blended
			if (pmcMesh->m_maxFaceInfl == 1) m_CSkinned->m_pd3dDevice->SetRenderState(D3DRS_VERTEXBLEND, D3DVBF_0WEIGHTS);

			else m_CSkinned->m_pd3dDevice->SetRenderState(D3DRS_VERTEXBLEND, pmcMesh->m_maxFaceInfl - 1);

			if (pmcMesh->m_maxFaceInfl) m_CSkinned->m_pd3dDevice->SetRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE, TRUE);

			// for each attribute group in the mesh, calculate the set of matrices in the palette and then draw the mesh subset
			pBoneComb = reinterpret_cast<LPD3DXBONECOMBINATION>(pmcMesh->m_pBoneCombinationBuf->GetBufferPointer());
			for (ipattr = 0; ipattr < pmcMesh->cpattr; ipattr++)
			{
				for (DWORD i = 0; i < pmcMesh->m_paletteSize; ++i)
				{
					auto matid = pBoneComb[ipattr].BoneId[i];
					if (matid != UINT_MAX)
					{
						m_CSkinned->m_pd3dDevice->SetTransform(D3DTS_WORLDMATRIX(i), &pmcMesh->m_pBoneOffsetMat[matid]);
						m_CSkinned->m_pd3dDevice->MultiplyTransform(D3DTS_WORLDMATRIX(i), pmcMesh->m_pBoneMatrix[matid]);
					}
				}

				m_CSkinned->m_pd3dDevice->SetMaterial(&(pmcMesh->rgMaterials[pBoneComb[ipattr].AttribId]));

				if (pmcMesh->pTextures[pBoneComb[ipattr].AttribId])

					m_CSkinned->m_pd3dDevice->SetTexture(0, pmcMesh->pTextures[pBoneComb[ipattr].AttribId]);

				else m_CSkinned->m_pd3dDevice->SetTexture(0, m_CSkinned->m_pTexture[m_CSkinned->m_bTextureNum - 1]);

				pmcMesh->pMesh->DrawSubset(ipattr);
				//		g_pApp->m_iDrawSubsetCallCount++;
			}
			m_CSkinned->m_pd3dDevice->SetRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE, FALSE);
			m_CSkinned->m_pd3dDevice->SetRenderState(D3DRS_VERTEXBLEND, 0);

			// 다시 원래대로 복원해준다. 반드시..
			if (pmcMesh->m_bUseSW) m_CSkinned->m_pd3dDevice->SetSoftwareVertexProcessing(FALSE);
		}
		else if (m_CSkinned->m_method == SOFTWARE)
		{
			D3DXMATRIX Identity;
			auto cBones = pmcMesh->m_pSkinMeshInfo->GetNumBones();
			PBYTE pbVerticesSrc;
			PBYTE pbVerticesDest;

			// set up bone transforms
			for (DWORD iBone = 0; iBone < cBones; ++iBone)

				D3DXMatrixMultiply(&m_CSkinned->m_pBoneMatrices[iBone], &pmcMesh->m_pBoneOffsetMat[iBone], pmcMesh->m_pBoneMatrix[iBone]);

			// set world transform
			D3DXMatrixIdentity(&Identity);
			m_CSkinned->m_pd3dDevice->SetTransform(D3DTS_WORLD, &Identity);

			// 2005-01-04 by jschoi - UpdateSkinnedMesh 
			if (pmcMesh->pMesh == nullptr) // 2005-01-05 by jschoi - 디바이스를 잃으면서 pMesh가 NULL이 된다.
			{
				auto hr = pmcMesh->m_pSkinMesh->CloneMeshFVF(D3DXMESH_MANAGED, pmcMesh->m_pSkinMesh->GetFVF(), m_CSkinned->m_pd3dDevice, &pmcMesh->pMesh);
				if (FAILED(hr)) return hr;
			}
			pmcMesh->m_pSkinMesh->LockVertexBuffer(D3DLOCK_READONLY, (LPVOID*)&pbVerticesSrc);
			pmcMesh->pMesh->LockVertexBuffer(0, (LPVOID*)&pbVerticesDest);
			pmcMesh->m_pSkinMeshInfo->UpdateSkinnedMesh(m_CSkinned->m_pBoneMatrices, nullptr, pbVerticesSrc, pbVerticesDest);
			pmcMesh->m_pSkinMesh->UnlockVertexBuffer();
			pmcMesh->pMesh->UnlockVertexBuffer();

			for (ipattr = 0; ipattr < pmcMesh->cpattr; ipattr++)
			{
				if (m_CSkinned->m_bProgressiveMesh) m_CSkinned->m_pd3dDevice->SetMaterial(&m_CSkinned->m_material);
				else m_CSkinned->m_pd3dDevice->SetMaterial(&(pmcMesh->rgMaterials[ipattr]));

				if (pmcMesh->pTextures[pmcMesh->m_pAttrTable[ipattr].AttribId])

					g_pD3dDev->SetTexture(0, pmcMesh->pTextures[pmcMesh->m_pAttrTable[ipattr].AttribId]);

				else g_pD3dDev->SetTexture(0, m_CSkinned->m_pTexture[m_CSkinned->m_bTextureNum - 1]);

				pmcMesh->pMesh->DrawSubset(ipattr);
				//		g_pApp->m_iDrawSubsetCallCount++;
			}
		}
		else return E_FAIL;
	}
	else
	{
		if (m_CSkinned->m_bProgressiveMesh) m_CSkinned->m_pd3dDevice->SetMaterial(&m_CSkinned->m_material);

		for (ipattr = 0; ipattr < pmcMesh->cpattr; ipattr++)
		{
			if (!m_CSkinned->m_bProgressiveMesh) m_CSkinned->m_pd3dDevice->SetMaterial(&(pmcMesh->rgMaterials[ipattr]));

			if (m_CSkinned->m_pOrderTexture) m_CSkinned->m_pd3dDevice->SetTexture(0, m_CSkinned->m_pOrderTexture);
			else
			{
				if (nType == _SHUTTLE || nType == _ENEMY)
				{
					if (m_CSkinned->m_unTexSelectColor >= m_CSkinned->m_bTotalTextureNum) m_CSkinned->m_unTexSelectColor = 0;
					if (m_CSkinned->m_unTexSelectColor <= 0) m_CSkinned->m_unTexSelectColor = 1;
					if (pmcMesh->pTextures[ipattr])
					{
						int nLoadIdx = m_CSkinned->m_unTexSelectColor - 1;
						if (FALSE == m_CSkinned->IsLoadTexture(nLoadIdx)) nLoadIdx = 0;
						m_CSkinned->m_pd3dDevice->SetTexture(0, m_CSkinned->m_pTexture[nLoadIdx]);
						m_CSkinned->m_unTexColor++;
					}
				}
				else
				{
					if (pmcMesh->pTextures[ipattr]) m_CSkinned->m_pd3dDevice->SetTexture(0, pmcMesh->pTextures[ipattr]);
					else
					{
						auto nTextureNum = m_CSkinned->m_bTextureNum - 1;
						if (nTextureNum >= 0) m_CSkinned->m_pd3dDevice->SetTexture(0, m_CSkinned->m_pTexture[m_CSkinned->m_bTextureNum - 1]);
					}
				}

				if (m_CSkinned->m_bMultiTexture)
				{
					auto nLoadIdx = 1;
					if (!m_CSkinned->IsLoadTexture(1)) nLoadIdx = 0;
					m_CSkinned->m_pd3dDevice->SetTexture(1, m_CSkinned->m_pTexture[nLoadIdx]);
				}
			}

#if SUPPORT_FX_METAL
			if (g_pD3dApp->m_pFxSystem->GetMetarSurface()) // always false with current config
			{
				D3DXMATRIX matView, matWorld, matProj;
				D3DXMatrixIdentity(&matView);
				D3DXMatrixIdentity(&matWorld);
				D3DXMatrixIdentity(&matProj);

				g_pD3dDev->GetTransform(D3DTS_PROJECTION, &matProj);
				g_pD3dDev->GetTransform(D3DTS_VIEW, &matView);
				g_pD3dDev->GetTransform(D3DTS_WORLD, &matWorld);

				g_pD3dApp->m_pFxSystem->SetMeshFrame(pmcMesh);
				g_pD3dApp->m_pFxSystem->DrawMetalFilterBegin(matWorld, matView, matProj, ipattr, nType);
			}
			else
#endif
#if SUPPORT_FX_ENV
				if (g_pD3dApp->m_pFxSystem->GetEnvSurface()) // always false with current config
				{
					D3DXMATRIX matView, matWorld, matProj;
					D3DXMatrixIdentity(&matView);
					D3DXMatrixIdentity(&matWorld);
					D3DXMatrixIdentity(&matProj);

					g_pD3dDev->GetTransform(D3DTS_PROJECTION, &matProj);
					g_pD3dDev->GetTransform(D3DTS_VIEW, &matView);
					g_pD3dDev->GetTransform(D3DTS_WORLD, &matWorld);

					g_pD3dApp->m_pFxSystem->SetMeshFrame(pmcMesh);
					g_pD3dApp->m_pFxSystem->DrawEvnFilterBegin(matWorld, matView, matProj, ipattr, nType);
				}
				else
#endif
				{
					pmcMesh->pMesh->DrawSubset(ipattr);
					//		g_pApp->m_iDrawSubsetCallCount++;
				}
		}
	}
	return S_OK;
}

void DrawingProc::RenderEffectVectorDP(vector<EffectBackBuffer>& vecEffectBackBuffer, BOOL isZEnabled, CEffectRender* m_CEffect)
{
	if (vecEffectBackBuffer.empty()) return;

	/*if(isZEnabled)
	{
		sort(vecEffectBackBuffer.begin(), vecEffectBackBuffer.end(), CompareEffect());
	}*/

	//vector<EffectBackBuffer>::iterator itEffectBackBuffer = vecEffectBackBuffer.begin();
	for (auto& el : vecEffectBackBuffer)
	{
		switch (el.effectType)
		{
		case EFFECT_TYPE_OBJECT:
		{
			CObjectAni* objectAni = (CObjectAni*)el.pEffect;
			if (objectAni->m_pParent->m_nAlphaValue == SKILL_OBJECT_ALPHA_OTHER_INFLUENCE)
				break;

			m_CEffect->ObjectAniRender(objectAni);
		}
		break;
		case EFFECT_TYPE_SPRITE:
		{
			CSpriteAni* spriteAni = (CSpriteAni*)el.pEffect;
			if (spriteAni->m_pParent->m_nAlphaValue == SKILL_OBJECT_ALPHA_OTHER_INFLUENCE)
				break;

			m_CEffect->SpriteAniRender(spriteAni);
		}
		break;
		case EFFECT_TYPE_PARTICLE:
		{
			if (g_pD3dApp->m_bParticlesOff == FALSE) {
				if (isZEnabled) {
					CParticle* particle = (CParticle*)el.pEffect;
					if (particle->m_pParent->m_pParent->m_nAlphaValue == SKILL_OBJECT_ALPHA_OTHER_INFLUENCE)
						break;

					if (el.particleType == PARTICLE_SPRITE_TYPE)
					{
						g_pD3dDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
						g_pD3dDev->SetFVF(D3DFVF_SPRITE_VERTEX);
						g_pD3dDev->SetRenderState(D3DRS_ZENABLE, particle->m_pParent->m_bZbufferEnable);
						//g_pD3dDev->SetRenderState( D3DRS_ZENABLE, TRUE);
						if (particle->m_pParent->m_bZWriteEnable == FALSE)
						{
							g_pD3dDev->SetRenderState(D3DRS_ZWRITEENABLE, particle->m_pParent->m_bZWriteEnable);
						}
						g_pD3dDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
						g_pD3dDev->SetRenderState(D3DRS_SRCBLEND, particle->m_pParent->m_dwSrcBlend);
						g_pD3dDev->SetRenderState(D3DRS_DESTBLEND, particle->m_pParent->m_dwDestBlend);
						g_pD3dDev->SetRenderState(D3DRS_LIGHTING, TRUE);
						g_pD3dDev->SetStreamSource(0, m_CEffect->m_pVB1, 0, sizeof(SPRITE_VERTEX));
						// set light
						D3DXVECTOR3 vAxis = g_pD3dApp->m_pCamera->GetViewDir();
						m_CEffect->m_light2 = g_pD3dApp->m_pScene->m_light2;
						m_CEffect->m_light2.Direction = vAxis;
						g_pD3dDev->SetLight(2, &m_CEffect->m_light2);
						g_pD3dDev->LightEnable(2, TRUE);
						m_CEffect->ParticleRender(particle->m_pParent, particle, vAxis, -1);
						g_pD3dDev->LightEnable(2, FALSE);
						if (particle->m_pParent->m_bZWriteEnable == FALSE)
						{
							g_pD3dDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
						}
					}
					if (el.particleType == PARTICLE_OBJECT_TYPE)
					{
						m_CEffect->ObjectParticleRender(particle->m_pObjectAni, particle);
					}
				}
				else
				{
					CParticleSystem* particleSystem = (CParticleSystem*)el.pEffect;
					if (el.particleType == PARTICLE_SPRITE_TYPE)
					{
						g_pD3dDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
						g_pD3dDev->SetFVF(D3DFVF_SPRITE_VERTEX);
						g_pD3dDev->SetRenderState(D3DRS_ZENABLE, particleSystem->m_bZbufferEnable);
						g_pD3dDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
						g_pD3dDev->SetRenderState(D3DRS_SRCBLEND, particleSystem->m_dwSrcBlend);
						g_pD3dDev->SetRenderState(D3DRS_DESTBLEND, particleSystem->m_dwDestBlend);
						g_pD3dDev->SetRenderState(D3DRS_LIGHTING, TRUE);
						g_pD3dDev->SetStreamSource(0, m_CEffect->m_pVB1, 0, sizeof(SPRITE_VERTEX));
						// set light
						D3DXVECTOR3 vAxis = g_pD3dApp->m_pCamera->GetViewDir();
						m_CEffect->m_light2 = g_pD3dApp->m_pScene->m_light2;
						m_CEffect->m_light2.Direction = vAxis;
						g_pD3dDev->SetLight(2, &m_CEffect->m_light2);
						g_pD3dDev->LightEnable(2, TRUE);
						int nOldTextureIndex = -1;
						for (int i = 0; i < particleSystem->m_vecParticle.size(); i++)
						{
							CParticle* p = (CParticle*)particleSystem->m_vecParticle[i];
							nOldTextureIndex = m_CEffect->ParticleRender(particleSystem, p, vAxis, nOldTextureIndex);
						}
						g_pD3dDev->LightEnable(2, FALSE);
					}

					if (el.particleType == PARTICLE_OBJECT_TYPE)
					{
						for (int i = 0; i < particleSystem->m_vecParticle.size(); i++)
						{
							m_CEffect->ObjectParticleRender(((CParticle*)particleSystem->m_vecParticle[i])->m_pObjectAni, (CParticle*)particleSystem->m_vecParticle[i]);
						}
					}
				}
			}
			break;
		case EFFECT_TYPE_TRACE:
		{
			CTraceAni* traceAni = ((CTraceAni*)el.pEffect);
			if (traceAni->m_pParent->m_nAlphaValue == SKILL_OBJECT_ALPHA_OTHER_INFLUENCE)
				break;

			m_CEffect->TraceAniRender(traceAni);
		}
		break;
		}
		}
		//	itEffectBackBuffer++;
	}

}

void DrawingProc::ObjectAniRenderDP(CObjectAni* pEffect, BOOL bAlpha, int nAlphaValue, CEffectRender* m_CEffect)
{
	FLOG("CEffectRender::ObjectAniRender(CObjectAni* pEffect)");
	if (strlen(pEffect->m_strName) == 0)
	{
		return;
	}

	DWORD dwSrc, dwDest, dwColorOp;
	DWORD dwFogValue = FALSE;
	g_pD3dDev->GetRenderState(D3DRS_SRCBLEND, &dwSrc);
	g_pD3dDev->GetRenderState(D3DRS_DESTBLEND, &dwDest);
	g_pD3dDev->GetTextureStageState(0, D3DTSS_COLOROP, &dwColorOp);

	g_pD3dDev->SetRenderState(D3DRS_ALPHATESTENABLE, pEffect->m_bAlphaTestEnble);
	if (pEffect->m_bAlphaTestEnble)
	{
		g_pD3dDev->SetRenderState(D3DRS_ALPHAREF, pEffect->m_nAlphaTestValue);
		g_pD3dDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
	}

	g_pD3dDev->SetRenderState(D3DRS_LIGHTING, TRUE);

	//	g_pD3dDev->SetRenderState( D3DRS_ZENABLE, TRUE);
		// 2008-07-30 by dgwoo ľĆ°Ü ĆŻÁ¤ľĆ¸Ó şÎ˝şĹÍ »çżë˝Ă ŔĚĆĺĆ®°ˇ ľĆ¸ÓµÚżˇ ŔÖŔ»°ćżěżˇµµ ş¸ŔÎ´Ů.
		// 2008-10-22 by bhsohn şÎ˝şĹÍ ľ˛¸éĽ­ °řĆř »çżë˝Ă, ł×¸đł­ ĹŘ˝şĂł »ý±â´Â ąö±× ĽöÁ¤
	g_pD3dDev->SetRenderState(D3DRS_ZENABLE, pEffect->m_bZbufferEnable);
	//g_pD3dDev->SetRenderState( D3DRS_ZENABLE, TRUE);

	if (pEffect->m_bZWriteEnable == FALSE)
	{
		g_pD3dDev->SetRenderState(D3DRS_ZWRITEENABLE, pEffect->m_bZWriteEnable);
	}
	g_pD3dDev->SetRenderState(D3DRS_ALPHABLENDENABLE, pEffect->m_bAlphaBlending);
	if (pEffect->m_bAlphaBlending)
	{
		g_pD3dDev->GetRenderState(D3DRS_FOGENABLE, &dwFogValue);
		if (pEffect->m_nSrcBlend == D3DBLEND_ONE && pEffect->m_nDestBlend == D3DBLEND_ONE && dwFogValue == TRUE)
		{
			g_pD3dDev->SetRenderState(D3DRS_FOGENABLE, FALSE);
		}
		g_pD3dDev->SetRenderState(D3DRS_SRCBLEND, pEffect->m_nSrcBlend);
		g_pD3dDev->SetRenderState(D3DRS_DESTBLEND, pEffect->m_nDestBlend);
	}
	g_pD3dDev->SetTextureStageState(0, D3DTSS_COLOROP, pEffect->m_nTextureRenderState);
	g_pD3dDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);

	//*--------------------------------------------------------------------------*//
	// 2006-11-16 by ispark, ľËĆÄ
	if (bAlpha)
	{
		g_pD3dApp->SetAlphaRenderState(nAlphaValue);
	}
	//*--------------------------------------------------------------------------*//

	D3DXMATRIX matScale;
	D3DXVECTOR3 pos;
	if (pEffect->m_pParent)
	{
		pos = pEffect->m_pParent->m_vPos;//»ó´ë ÁÂÇĄ
	}
	else
	{
		pos = D3DXVECTOR3(0, 0, 0);
	}
	// 2010. 03. 18 by jskim ¸ó˝şĹÍşŻ˝Ĺ Ä«µĺ
	float tempScale = 0.0f;
	if (pEffect->m_pParent->m_pParent->m_MonsterTransformer &&
		pEffect->m_pParent->m_pParent->m_MonsterTransScale > 0)
	{
		tempScale = pEffect->m_fScale * pEffect->m_pParent->m_pParent->m_MonsterTransScale;
	}
	else
	{
		tempScale = pEffect->m_fScale;
	}
	//D3DXMatrixScaling(&matScale, pEffect->m_fScale,pEffect->m_fScale,pEffect->m_fScale);
	D3DXMatrixScaling(&matScale, tempScale, tempScale, tempScale);
	//end 2010. 03. 18 by jskim ¸ó˝şĹÍşŻ˝Ĺ Ä«µĺ
	if (pEffect->m_pParent && pEffect->m_pParent->m_bUseBillboard)
	{
		if (pEffect->m_pParent->m_bGroundBillboard == TRUE)
		{
			D3DXVECTOR3 vPos(pEffect->m_pParent->m_pParent->m_mMatrix._41,
				pEffect->m_pParent->m_pParent->m_mMatrix._42,
				pEffect->m_pParent->m_pParent->m_mMatrix._43);
			D3DXMATRIX matTemp;
			D3DXVECTOR3 vVel = g_pD3dApp->m_pCamera->GetEyePt() - vPos;
			D3DXVECTOR3 vUp(0, 1, 0);
			if (vVel == vUp)
			{
				vVel = D3DXVECTOR3(1, 0, 0);
			}
			else
			{
				D3DXVec3Cross(&vVel, &vVel, &vUp);
				D3DXVec3Cross(&vVel, &vVel, &vUp);
			}
			D3DXVec3Normalize(&vVel, &vVel);
			D3DXMatrixLookAtLH(&matTemp, &(vPos), &(vPos + vVel), &vUp);
			D3DXMatrixInverse(&matTemp, NULL, &matTemp);
			matScale = matTemp * matScale;
		}
		else
		{
			D3DXMATRIX matBillboard = g_pD3dApp->m_pCamera->GetBillboardMatrix();
			D3DXMATRIX matRotate;
			D3DXVECTOR3 vAxis = g_pD3dApp->m_pCamera->GetViewDir();//pos - g_pD3dApp->m_pCamera->GetEyePt();
			D3DXMatrixRotationAxis(&matRotate, &vAxis, pEffect->m_pParent->m_fBillboardAngle);
			matBillboard *= matRotate;

			if (pEffect->m_pParent->m_fBillboardRotatePerSec > 0)
			{
				D3DXMatrixRotationAxis(&matRotate, &vAxis, pEffect->m_fCurrentBillboardRotateAngle);
				matBillboard *= matRotate;
			}
			if (pEffect->m_fCurrentRandomUpAngleX != 0)
			{
				D3DXMatrixRotationAxis(&matRotate, &vAxis, pEffect->m_fCurrentRandomUpAngleX);
				matBillboard *= matRotate;
			}
			matScale._41 = pEffect->m_pParent->m_vPos.x;
			matScale._42 = pEffect->m_pParent->m_vPos.y;
			matScale._43 = pEffect->m_pParent->m_vPos.z;

			matBillboard = matScale * matBillboard;

			matScale = matScale * pEffect->m_pParent->m_pParent->m_mMatrix;
			matBillboard._41 = matScale._41;
			matBillboard._42 = matScale._42;
			matBillboard._43 = matScale._43;
			matScale = matBillboard;
		}
		// set light
		if (!pEffect->m_bUseEnvironmentLight)
		{
			m_CEffect->m_light2 = g_pD3dApp->m_pScene->m_light2;
			m_CEffect->m_light2.Direction = g_pD3dApp->m_pCamera->GetViewDir();
			g_pD3dDev->SetLight(2, &m_CEffect->m_light2);
			g_pD3dDev->LightEnable(2, TRUE);
		}
	}
	else
	{
		D3DXMATRIX lookat;
		D3DXVECTOR3 up = pEffect->m_pParent->m_vUp;
		D3DXVECTOR3 target = pEffect->m_pParent->m_vTarget;
		if (pEffect->m_fCurrentRandomUpAngleX != 0 || pEffect->m_fCurrentRandomUpAngleZ != 0)
		{
			up = D3DXVECTOR3(0, 1, 0);
			target = pEffect->m_pParent->m_vPos;
			D3DXVECTOR3 vel = D3DXVECTOR3(0, 0, 1);
			D3DXMATRIX matRotateX, matRotateZ;
			D3DXVECTOR3 vAxis(1, 0, 0);// up vector Č¸ŔüĂŕ(XĂŕ)
			D3DXMatrixRotationAxis(&matRotateX, &vAxis, pEffect->m_fCurrentRandomUpAngleX);
			D3DXVec3TransformCoord(&up, &up, &matRotateX);
			D3DXVec3TransformCoord(&vel, &vel, &matRotateX);
			vAxis = D3DXVECTOR3(0, 0, 1);// up vector Č¸ŔüĂŕ(XĂŕ)
			D3DXMatrixRotationAxis(&matRotateZ, &vAxis, pEffect->m_fCurrentRandomUpAngleZ);
			D3DXVec3TransformCoord(&up, &up, &matRotateZ);
			D3DXVec3TransformCoord(&vel, &vel, &matRotateZ);
			target += vel;
		}
		D3DXMatrixLookAtLH(&lookat, &pos, &target, &up);
		D3DXMatrixInverse(&lookat, NULL, &lookat);
		matScale = lookat * matScale;// * matScale;
		D3DXMATRIX mat = pEffect->m_pParent->m_pParent->m_mMatrix;
		mat._41 = 0;
		mat._42 = 0;
		mat._43 = 0;
		matScale *= mat;
		matScale._41 += pEffect->m_pParent->m_pParent->m_mMatrix._41;
		matScale._42 += pEffect->m_pParent->m_pParent->m_mMatrix._42;
		matScale._43 += pEffect->m_pParent->m_pParent->m_mMatrix._43;

		// set light
		if (!pEffect->m_bUseEnvironmentLight)
		{
			m_CEffect->m_light2 = g_pD3dApp->m_pScene->m_light2;
			m_CEffect->m_light2.Direction = g_pD3dApp->m_pCamera->GetViewDir();
			g_pD3dDev->SetLight(2, &m_CEffect->m_light2);
			g_pD3dDev->LightEnable(2, TRUE);
		}
	}
	CSkinnedMesh* pMesh = m_CEffect->LoadObject(pEffect->m_strObjectFile);
	if (!pMesh)
	{
		if (!pEffect->m_bUseEnvironmentLight)
		{
			g_pD3dDev->LightEnable(2, FALSE);
		}
		if (pEffect->m_nSrcBlend == D3DBLEND_ONE && pEffect->m_nDestBlend == D3DBLEND_ONE && dwFogValue == TRUE)
		{
			g_pD3dDev->SetRenderState(D3DRS_FOGENABLE, dwFogValue);
		}
		if (pEffect->m_bZWriteEnable == FALSE)
		{
			g_pD3dDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
		}
		return;
	}

	switch (pEffect->m_nObjectAniType)
	{
	case 0:// object animation
	{	//m_pObjEffectMesh[index]
		pMesh->Tick(pEffect->m_fCurrentObjectAniTime);
		pMesh->AnotherTexture(1);
	}
	break;
	case 1:// texture animation
	{
		pMesh->AnotherTexture(pEffect->m_nCurrentTextureType + 1);
	}
	break;
	case 2:// object + texture animation
	{
		pMesh->Tick(pEffect->m_fCurrentObjectAniTime);
		pMesh->AnotherTexture(pEffect->m_nCurrentTextureType + 1);
	}
	break;
	}

	D3DMATERIAL9 mtrl;
	D3DUtil_InitMaterial(mtrl, pEffect->m_cColor.r,
		pEffect->m_cColor.g, pEffect->m_cColor.b, pEffect->m_cColor.a);
	BOOL bTemp = pMesh->m_bMaterial;
	pMesh->m_bMaterial = TRUE;
	pMesh->m_material = mtrl;
	//	Ŕý´ë ÁÂÇĄ ĽĽĆĂ
	pMesh->SetWorldMatrix(matScale);//*pEffect->m_pParent->m_pParent->m_mMatrix);

	if (!pEffect->m_bZbufferEnable && m_CEffect->m_bZBufferTemp)
	{
		D3DXVECTOR3 vTempPos;
		vTempPos.x = matScale._41;
		vTempPos.y = matScale._42;
		vTempPos.z = matScale._43;
		D3DXMATRIX matTemp;
		D3DXVECTOR3 vPos, vVel, vUp, vSide;
		vPos = g_pD3dApp->m_pCamera->GetEyePt();
		D3DXVec3Normalize(&vUp, &g_pD3dApp->m_pCamera->GetUpVec());
		D3DXVec3Normalize(&vVel, &(vTempPos - vPos));
		D3DXVec3Cross(&vSide, &vUp, &vVel);
		D3DXVec3Cross(&vUp, &vVel, &vSide);

		D3DXMatrixLookAtLH(&matTemp, &vPos, &(vPos + vVel), &vUp);
		float fDist1, fDist2;
		fDist1 = D3DXVec3Length(&(vTempPos - vPos));
		fDist2 = g_pD3dApp->m_pScene->m_pObjectRender->CheckPickMesh(matTemp, vPos).fDist;
		if (fDist1 > fDist2)
			g_pD3dDev->SetRenderState(D3DRS_ZENABLE, TRUE);
		else
			g_pD3dDev->SetRenderState(D3DRS_ZENABLE, FALSE);
	}
	// by dhkwon, 030917
	LPDIRECT3DTEXTURE9 pOrgTex = NULL;
	if (pEffect->m_pParent && pEffect->m_pParent->m_pTexture) {
		pOrgTex = pMesh->SetTexture(pEffect->m_pParent->m_pTexture, 0);
	}
	else if (pEffect->m_strTextureFile[0])
	{
		int i = m_CEffect->LoadTexture(pEffect->m_strTextureFile);
		if (m_CEffect->m_nTextureRenderCount[i] == 2)
			m_CEffect->m_nTextureRenderCount[i]++;
		pOrgTex = pMesh->SetTexture(m_CEffect->m_pTexture[i], 0);
	}
	DWORD dwLightColorOp = 0;
	if (pEffect->m_bLightMapUse)
	{
		g_pD3dDev->GetTextureStageState(1, D3DTSS_COLOROP, &dwLightColorOp);
		g_pD3dDev->SetTextureStageState(1, D3DTSS_COLOROP, pEffect->m_nLightMapRenderState);
		g_pD3dDev->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 0);

		g_pD3dDev->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		g_pD3dDev->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);
	}

	// 2006-01-24 by ispark
	if ((!g_pD3dApp->m_bCharacter &&									// ±âľî ŔĚ¸éĽ­
		g_pD3dApp->m_pCamera->GetCamType() != CAMERA_TYPE_FPS &&	//  1ŔÎÄŞ ¸đµĺ°ˇ ľĆ´Ď±¸
		pEffect->m_pParent->m_nInvenWeaponIndex > 0) ||				// ŔÎşĄ ą«±â ŔÎµ¦˝ş°ˇ 0 ŔĚ»óŔĚ¸é ·»´ő¸µ ÇĎÁö¸¶¶ó
		pEffect->m_pParent->m_nInvenWeaponIndex == 0)				// ŔÎşĄ ą«±â ŔÎµ¦˝ş°ˇ 0ŔĚ¸é ·»´ő¸µ 
	{
		if (pEffect->m_bLightMapUse)
		{
			pMesh->Render(TRUE);
		}
		else
		{
			pMesh->Render();
		}
	}

	// 2008-08-22 by bhsohn EP3 ŔÎşĄĹä¸® Ăł¸®	
	//if(pEffect->m_pParent->m_nInvenWeaponIndex > 0	&&  g_pShuttleChild->m_bInvenRender == TRUE)
	if (pEffect->m_pParent->m_nInvenWeaponIndex > 0 && g_pGameMain->IsEquipInvenShow() == TRUE)
	{
		D3DXMATRIX pMatOldView, pMatOldProj, pMatPresView, pMatPresProj, pMatrix;
		D3DXMatrixIdentity(&pMatOldView);
		D3DXMatrixIdentity(&pMatOldProj);
		D3DXMatrixIdentity(&pMatPresView);
		D3DXMatrixIdentity(&pMatPresProj);
		D3DXMatrixIdentity(&pMatrix);

		// ÇöŔç şäżÍ ÇÁ·ÎÁ§ĽÇŔ» °ˇÁ®żÂ´Ů
		g_pD3dDev->GetTransform(D3DTS_VIEW, &pMatOldView);
		g_pD3dDev->GetTransform(D3DTS_PROJECTION, &pMatOldProj);

		g_pD3dDev->SetTransform(D3DTS_VIEW, &pMatPresView);
		g_pD3dDev->SetTransform(D3DTS_PROJECTION, &pMatPresProj);
		g_pD3dDev->SetRenderState(D3DRS_LIGHTING, FALSE);

		if (pEffect->m_pParent->m_nInvenWeaponIndex > 4)
		{
			// 2008-08-22 by bhsohn EP3 ŔÎşĄĹä¸® Ăł¸®
			//pMesh->SetWorldMatrix(g_pShuttleChild->m_pMatInven);
			pMesh->SetWorldMatrix(g_pGameMain->GetInvenMatInven());
			pMesh->Render();
		}
		else
			g_pShuttleChild->SetInvenMesh(pEffect->m_pParent->m_nInvenWeaponIndex - 1, pMesh);
		//			pMesh->SetWorldMatrix(g_pShuttleChild->m_pMatInvenWeaponSetPosition[pEffect->m_pParent->m_nInvenWeaponIndex - 1]);
		//		pMesh->Render();

		g_pD3dDev->SetTransform(D3DTS_VIEW, &pMatOldView);
		g_pD3dDev->SetTransform(D3DTS_PROJECTION, &pMatOldProj);
		g_pD3dDev->SetRenderState(D3DRS_LIGHTING, TRUE);
	}


	//	if(g_pShuttleChild->m_bInvenRender && bInven == TRUE && pEffect->m_pParent->m_nInvenWeaponIndex > 0)
	//	{
	//		//////////////////////////////////////////////////////////////////////////
	//		//
	//		D3DXMATRIX pMatOldView, pMatOldProj, pMatPresView, pMatPresProj, pMatrix;
	//		D3DXMatrixIdentity(&pMatOldView);
	//		D3DXMatrixIdentity(&pMatOldProj);
	//		D3DXMatrixIdentity(&pMatPresView);
	//		D3DXMatrixIdentity(&pMatPresProj);
	//		D3DXMatrixIdentity(&pMatrix);		
	//
	//		// ÇöŔç şäżÍ ÇÁ·ÎÁ§ĽÇŔ» °ˇÁ®żÂ´Ů
	//		g_pD3dDev->GetTransform( D3DTS_VIEW,       &pMatOldView );
	//		g_pD3dDev->GetTransform( D3DTS_PROJECTION, &pMatOldProj );	
	//		
	//		g_pD3dDev->SetTransform( D3DTS_VIEW,		&pMatPresView);
	//		g_pD3dDev->SetTransform( D3DTS_PROJECTION,	&pMatPresProj);
	//		g_pD3dDev->SetRenderState( D3DRS_LIGHTING, FALSE );
	//		
	//		// ą«±â
	//		if(pEffect->m_pParent->m_nInvenWeaponIndex > 4)
	//		{
	//			pMesh->SetWorldMatrix(g_pShuttleChild->m_pMatInven);
	//			if(pEffect->m_bLightMapUse)	
	//				pMesh->Render(TRUE);
	//			else 
	//				pMesh->Render();	
	//		}
	//		else
	//			g_pShuttleChild->SetInvenMesh(pEffect->m_pParent->m_nInvenWeaponIndex - 1, pMesh);
	////			pMesh->SetWorldMatrix(g_pShuttleChild->m_pMatInvenWeaponSetPosition[pEffect->m_pParent->m_nInvenWeaponIndex - 1]);
	////		if(pEffect->m_bLightMapUse)	
	////			pMesh->Render(TRUE);
	////		else 
	////			pMesh->Render();	
	//		// şą±¸
	//		g_pD3dDev->SetTransform( D3DTS_VIEW,		&pMatOldView );
	//		g_pD3dDev->SetTransform( D3DTS_PROJECTION,	&pMatOldProj );
	//		g_pD3dDev->SetRenderState( D3DRS_LIGHTING, TRUE );
	//		//
	//		//////////////////////////////////////////////////////////////////////////		
	//	}		

	if ((pEffect->m_pParent && pEffect->m_pParent->m_pTexture) || pEffect->m_strTextureFile[0])
	{
		pMesh->SetTexture(pOrgTex, 0);
	}

	if (pMesh && pMesh->m_nRenderCount == 2)
		pMesh->m_nRenderCount++;
	pMesh->m_bMaterial = bTemp;
	g_pD3dDev->SetRenderState(D3DRS_SRCBLEND, dwSrc);
	g_pD3dDev->SetRenderState(D3DRS_DESTBLEND, dwDest);
	g_pD3dDev->SetTextureStageState(0, D3DTSS_COLOROP, dwColorOp);
	if (!pEffect->m_bUseEnvironmentLight)
	{
		g_pD3dDev->LightEnable(2, FALSE);
	}
	if (pEffect->m_bLightMapUse)
	{
		g_pD3dDev->SetTextureStageState(1, D3DTSS_COLOROP, dwLightColorOp);
	}
	if (pEffect->m_nSrcBlend == D3DBLEND_ONE && pEffect->m_nDestBlend == D3DBLEND_ONE && dwFogValue == TRUE)
	{
		g_pD3dDev->SetRenderState(D3DRS_FOGENABLE, dwFogValue);
	}
	if (pEffect->m_bAlphaTestEnble)
	{
		g_pD3dDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	}
	if (pEffect->m_bZWriteEnable == FALSE)
	{
		g_pD3dDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	}

	m_CEffect->m_nObjectEffectRender++;
}

void DrawingProc::ObjectParticleRenderDP(CObjectAni* pEffect, CParticle* pParticle, CEffectRender* m_CEffect)
{
	FLOG("CEffectRender::ObjectParticleRender(CObjectAni* pEffect, CParticle* pParticle)");
	DWORD dwSrc, dwDest, dwColorOp;
	DWORD dwFogValue = FALSE;
	g_pD3dDev->GetRenderState(D3DRS_SRCBLEND, &dwSrc);
	g_pD3dDev->GetRenderState(D3DRS_DESTBLEND, &dwDest);
	g_pD3dDev->GetTextureStageState(0, D3DTSS_COLOROP, &dwColorOp);

	g_pD3dDev->SetRenderState(D3DRS_ALPHATESTENABLE, pEffect->m_bAlphaTestEnble);
	if (pEffect->m_bAlphaTestEnble)
	{
		g_pD3dDev->SetRenderState(D3DRS_ALPHAREF, pEffect->m_nAlphaTestValue);
		g_pD3dDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
	}

	g_pD3dDev->SetRenderState(D3DRS_LIGHTING, TRUE);

	//	g_pD3dDev->SetRenderState( D3DRS_ZENABLE, TRUE);
	g_pD3dDev->SetRenderState(D3DRS_ZENABLE, pEffect->m_bZbufferEnable);

	if (pEffect->m_bZWriteEnable == FALSE)
	{
		g_pD3dDev->SetRenderState(D3DRS_ZWRITEENABLE, pEffect->m_bZWriteEnable);
	}
	g_pD3dDev->SetRenderState(D3DRS_ALPHABLENDENABLE, pEffect->m_bAlphaBlending);
	if (pEffect->m_bAlphaBlending)
	{
		g_pD3dDev->GetRenderState(D3DRS_FOGENABLE, &dwFogValue);
		if (pEffect->m_nSrcBlend == D3DBLEND_ONE && pEffect->m_nDestBlend == D3DBLEND_ONE && dwFogValue == TRUE)
		{
			g_pD3dDev->SetRenderState(D3DRS_FOGENABLE, FALSE);
		}
		g_pD3dDev->SetRenderState(D3DRS_SRCBLEND, pEffect->m_nSrcBlend);
		g_pD3dDev->SetRenderState(D3DRS_DESTBLEND, pEffect->m_nDestBlend);
	}
	g_pD3dDev->SetTextureStageState(0, D3DTSS_COLOROP, pEffect->m_nTextureRenderState);
	g_pD3dDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	//	g_pD3dDev->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR  );
	//	g_pD3dDev->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR  );
	D3DXMATRIX matScale;
	D3DXVECTOR3 pos;
	if (pParticle)
	{
		//		pos = pData->m_pParent->m_vPos;
		pos = pParticle->m_vPos;//ÁÂÇĄ
	}
	else
	{
		pos = D3DXVECTOR3(0, 0, 0);
	}
	// 2010. 03. 18 by jskim ¸ó˝şĹÍşŻ˝Ĺ Ä«µĺ
	//D3DXMatrixScaling(&matScale, pEffect->m_fScale,pEffect->m_fScale,pEffect->m_fScale);
	float tempScale = 0.0f;
	if (pEffect->m_pParent->m_pParent->m_MonsterTransformer &&
		pEffect->m_pParent->m_pParent->m_MonsterTransScale > 0)
	{
		tempScale = pEffect->m_fScale * pEffect->m_pParent->m_pParent->m_MonsterTransScale;
	}
	else
	{
		tempScale = pEffect->m_fScale;
	}

	D3DXMatrixScaling(&matScale, tempScale, tempScale, tempScale);
	//end 2010. 03. 18 by jskim ¸ó˝şĹÍşŻ˝Ĺ Ä«µĺ
	if (pEffect->m_pParent && pEffect->m_pParent->m_bUseBillboard)
	{
		/*		matScale *= g_pD3dApp->m_pCamera->GetBillboardMatrix();
				D3DXMATRIX matRotate;
				D3DXVECTOR3 vAxis = g_pD3dApp->m_pCamera->GetViewDir();//pos - g_pD3dApp->m_pCamera->GetEyePt();
				D3DXMatrixRotationAxis( &matRotate, &vAxis, pEffect->m_pParent->m_fBillboardAngle );
				matScale *= matRotate;

				if(pEffect->m_pParent->m_fBillboardRotatePerSec>0)
				{
					D3DXMatrixRotationAxis( &matRotate, &vAxis, pEffect->m_fCurrentBillboardRotateAngle );
					matScale *= matRotate;
				}
				if(pEffect->m_fCurrentRandomUpAngleX != 0)
				{
					D3DXMatrixRotationAxis( &matRotate, &vAxis, pEffect->m_fCurrentRandomUpAngleX );
					matScale *= matRotate;
				}
		*/		D3DXMATRIX matBillboard = g_pD3dApp->m_pCamera->GetBillboardMatrix();
		D3DXMATRIX matRotate;
		D3DXVECTOR3 vAxis = g_pD3dApp->m_pCamera->GetViewDir();//pos - g_pD3dApp->m_pCamera->GetEyePt();
		D3DXMatrixRotationAxis(&matRotate, &vAxis, pEffect->m_pParent->m_fBillboardAngle);
		matBillboard *= matRotate;

		if (pEffect->m_pParent->m_fBillboardRotatePerSec > 0)
		{
			D3DXMatrixRotationAxis(&matRotate, &vAxis, pEffect->m_fCurrentBillboardRotateAngle);
			matBillboard *= matRotate;
		}
		if (pEffect->m_fCurrentRandomUpAngleX != 0)
		{
			D3DXMatrixRotationAxis(&matRotate, &vAxis, pEffect->m_fCurrentRandomUpAngleX);
			matBillboard *= matRotate;
		}
		/*
				matScale._41 = pEffect->m_pParent->m_pParent->m_mMatrix._41;
				matScale._42 = pEffect->m_pParent->m_pParent->m_mMatrix._42;
				matScale._43 = pEffect->m_pParent->m_pParent->m_mMatrix._43;

				matScale._41 += pEffect->m_pParent->m_vPos.x;
				matScale._42 += pEffect->m_pParent->m_vPos.y;
				matScale._43 += pEffect->m_pParent->m_vPos.z;
		*/		matScale._41 = pEffect->m_pParent->m_vPos.x;
		matScale._42 = pEffect->m_pParent->m_vPos.y;
		matScale._43 = pEffect->m_pParent->m_vPos.z;
		matBillboard = matScale * matBillboard;
		matScale = matScale * pEffect->m_pParent->m_pParent->m_mMatrix;
		matBillboard._41 = matScale._41;
		matBillboard._42 = matScale._42;
		matBillboard._43 = matScale._43;
		matScale = matBillboard;
		// set light
		if (!pEffect->m_bUseEnvironmentLight)
		{
			m_CEffect->m_light2 = g_pD3dApp->m_pScene->m_light2;
			m_CEffect->m_light2.Direction = g_pD3dApp->m_pCamera->GetViewDir();
			g_pD3dDev->SetLight(2, &m_CEffect->m_light2);
			g_pD3dDev->LightEnable(2, TRUE);
		}
	}
	else
	{
		D3DXVECTOR3 up, target;
		switch (pParticle->m_pParent->m_nObjMoveTargetType)
		{
		case OBJ_MOVE_TYPE0://ĆÄĆĽĹ¬ ŔĚµżąćÇâŔ» °ˇ¸®Ĺ´
		{
			target = pParticle->m_vObjTarget;
			up = pParticle->m_vObjUp;
		}
		break;
		case OBJ_MOVE_TYPE1:// ŔŻ´Ö ąćÇâŔ¸·Î °ˇ¸®Ĺ´
		{
			target = pEffect->m_pParent ? pEffect->m_pParent->m_vTarget : D3DXVECTOR3(0, 0, 1);
			up = pEffect->m_pParent ? pEffect->m_pParent->m_vUp : D3DXVECTOR3(0, 1, 0);
		}
		break;
		case OBJ_MOVE_TYPE2:// ·Ł´ýŔ¸·Î żňÁ÷ŔÓ
		{
			target = pParticle->m_vObjTarget;
			up = pParticle->m_vObjUp;
			//				target.x = Random(1.0f, -1.0f);
			//				target.y = Random(1.0f, -1.0f);
			//				target.z = Random(1.0f, -1.0f);
			//				D3DXVec3Normalize(&target,&target);
			//				up = pEffect->m_pParent ?  pEffect->m_pParent->m_vUp : D3DXVECTOR3(0,1,0);
		}
		break;
		case OBJ_MOVE_TYPE3:// şôş¸µĺ
		{
			target = pParticle->m_vPos - g_pD3dApp->m_pCamera->GetViewDir();
			up = g_pD3dApp->m_pCamera->GetUpVec();
		}
		break;
		}
		D3DXMATRIX lookat;
		//		D3DXVECTOR3 up = pEffect->m_pParent->m_vUp;
		//		D3DXVECTOR3 target = pEffect->m_pParent->m_vTarget;
		if (pEffect->m_fCurrentRandomUpAngleX != 0 || pEffect->m_fCurrentRandomUpAngleZ != 0)
		{
			up = D3DXVECTOR3(0, 1, 0);
			target = pParticle->m_vPos;
			D3DXVECTOR3 vel = D3DXVECTOR3(0, 0, 1);
			D3DXMATRIX matRotateX, matRotateZ;
			D3DXVECTOR3 vAxis(1, 0, 0);// up vector Č¸ŔüĂŕ(XĂŕ)
			D3DXMatrixRotationAxis(&matRotateX, &vAxis, pEffect->m_fCurrentRandomUpAngleX);
			D3DXVec3TransformCoord(&up, &up, &matRotateX);
			D3DXVec3TransformCoord(&vel, &vel, &matRotateX);
			vAxis = D3DXVECTOR3(0, 0, 1);// up vector Č¸ŔüĂŕ(XĂŕ)
			D3DXMatrixRotationAxis(&matRotateZ, &vAxis, pEffect->m_fCurrentRandomUpAngleZ);
			D3DXVec3TransformCoord(&up, &up, &matRotateZ);
			D3DXVec3TransformCoord(&vel, &vel, &matRotateZ);
			target += vel;
		}

		D3DXMatrixLookAtLH(&lookat, &D3DXVECTOR3(0, 0, 0), &target, &up);
		//		D3DXMatrixLookAtLH( &lookat, &pos, &target, &up);
		D3DXMatrixInverse(&lookat, NULL, &lookat);
		matScale = lookat * matScale;// * matScale;
		D3DXMATRIX mat = pEffect->m_pParent->m_pParent->m_mMatrix;
		mat._41 = pos.x;
		mat._42 = pos.y;
		mat._43 = pos.z;
		matScale = matScale * mat;
		// set light
		if (!pEffect->m_bUseEnvironmentLight)
		{
			m_CEffect->m_light2 = g_pD3dApp->m_pScene->m_light2;
			m_CEffect->m_light2.Direction = g_pD3dApp->m_pCamera->GetViewDir();
			g_pD3dDev->SetLight(2, &m_CEffect->m_light2);
			g_pD3dDev->LightEnable(2, TRUE);
		}
	}

	//	int index = LoadObject(pEffect->m_strObjectFile);
	//	if(index<0)
	//		return;
	CSkinnedMesh* pMesh = m_CEffect->LoadObject(pEffect->m_strObjectFile);
	if (!pMesh)
	{
		if (!pEffect->m_bUseEnvironmentLight)
		{
			g_pD3dDev->LightEnable(2, FALSE);
		}
		if (pEffect->m_nSrcBlend == D3DBLEND_ONE && pEffect->m_nDestBlend == D3DBLEND_ONE && dwFogValue == TRUE)
		{
			g_pD3dDev->SetRenderState(D3DRS_FOGENABLE, dwFogValue);
		}
		if (pEffect->m_bZWriteEnable == FALSE)
		{
			g_pD3dDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
		}
		return;
	}
	switch (pEffect->m_nObjectAniType)
	{
	case 0:// object animation
	{
		pMesh->Tick(pEffect->m_fCurrentObjectAniTime);
		pMesh->AnotherTexture(1);
	}
	break;
	case 1:// texture animation
	{
		pMesh->AnotherTexture(pEffect->m_nCurrentTextureType + 1);
	}
	break;
	case 2:// object + texture animation
	{
		pMesh->Tick(pEffect->m_fCurrentObjectAniTime);
		pMesh->AnotherTexture(pEffect->m_nCurrentTextureType + 1);
	}
	break;
	}

	D3DMATERIAL9 mtrl;
	D3DUtil_InitMaterial(mtrl, pEffect->m_cColor.r,
		pEffect->m_cColor.g, pEffect->m_cColor.b, pEffect->m_cColor.a);
	BOOL bTemp = pMesh->m_bMaterial;
	pMesh->m_bMaterial = TRUE;
	pMesh->m_material = mtrl;
	//	Ŕý´ë ÁÂÇĄ ĽĽĆĂ
	pMesh->SetWorldMatrix(matScale);//*pEffect->m_pParent->m_pParent->m_mMatrix);
	if (!pEffect->m_bZbufferEnable && m_CEffect->m_bZBufferTemp)
	{
		D3DXVECTOR3 vTempPos;
		vTempPos.x = matScale._41;
		vTempPos.y = matScale._42;
		vTempPos.z = matScale._43;
		D3DXMATRIX matTemp;
		D3DXVECTOR3 vPos, vVel, vUp, vSide;
		vPos = g_pD3dApp->m_pCamera->GetEyePt();
		D3DXVec3Normalize(&vUp, &g_pD3dApp->m_pCamera->GetUpVec());
		D3DXVec3Normalize(&vVel, &(vTempPos - vPos));
		D3DXVec3Cross(&vSide, &vUp, &vVel);
		D3DXVec3Cross(&vUp, &vVel, &vSide);

		D3DXMatrixLookAtLH(&matTemp, &vPos, &(vPos + vVel), &vUp);
		float fDist1, fDist2;
		fDist1 = D3DXVec3Length(&(vTempPos - vPos));
		fDist2 = g_pD3dApp->m_pScene->m_pObjectRender->CheckPickMesh(matTemp, vPos).fDist;
		if (fDist1 > fDist2)
			g_pD3dDev->SetRenderState(D3DRS_ZENABLE, TRUE);
		else
			g_pD3dDev->SetRenderState(D3DRS_ZENABLE, FALSE);
	}
	// by dhkwon, 030917
	LPDIRECT3DTEXTURE9 pOrgTex = NULL;
	if (pEffect->m_pParent && pEffect->m_pParent->m_pTexture) {
		pOrgTex = pMesh->SetTexture(pEffect->m_pParent->m_pTexture, 0);
	}
	else if (pEffect->m_strTextureFile[0])
	{
		int i = m_CEffect->LoadTexture(pEffect->m_strTextureFile);
		if (m_CEffect->m_nTextureRenderCount[i] == 2)
			m_CEffect->m_nTextureRenderCount[i]++;
		pOrgTex = pMesh->SetTexture(m_CEffect->m_pTexture[i], 0);
	}
	DWORD dwLightColorOp = 0;
	if (pEffect->m_bLightMapUse)
	{
		g_pD3dDev->GetTextureStageState(1, D3DTSS_COLOROP, &dwLightColorOp);
		g_pD3dDev->SetTextureStageState(1, D3DTSS_COLOROP, pEffect->m_nLightMapRenderState);
		g_pD3dDev->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 0);
		// 2005-01-04 by jschoi
//		g_pD3dDev->SetTextureStageState( 1, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
//		g_pD3dDev->SetSamplerState(1,D3DSAMP_MAGFILTER,D3DTEXF_LINEAR);

		g_pD3dDev->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		g_pD3dDev->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);
		//		g_pD3dDev->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
	}
	if (pEffect->m_bLightMapUse)
	{
		pMesh->Render(TRUE);
		//		pMesh->Render();
	}
	else
	{
		pMesh->Render();
	}
	if ((pEffect->m_pParent && pEffect->m_pParent->m_pTexture) || pEffect->m_strTextureFile[0])
	{
		pMesh->SetTexture(pOrgTex, 0);
	}

	//	if(m_nObjRenderCount[index] < 3)
	//		m_nObjRenderCount[index]++;
	if (pMesh && pMesh->m_nRenderCount == 2)
		pMesh->m_nRenderCount++;
	/*	if(pEffect->m_bLightMapUse)
		{
			g_pD3dDev->SetRenderState( D3DRS_ALPHABLENDENABLE,  pEffect->m_bLightMapAlphaBlending );
			g_pD3dDev->SetRenderState(D3DRS_SRCBLEND,pEffect->m_nLightMapSrcBlend);
			g_pD3dDev->SetRenderState(D3DRS_DESTBLEND,pEffect->m_nLightMapDestBlend);
			g_pD3dDev->SetTextureStageState(0,D3DTSS_COLOROP,pEffect->m_nLightMapRenderState);
			// by dhkwon, 030917
	//		LPDIRECT3DTEXTURE8 pOrgTex = NULL;
			if(pEffect->m_strTextureFile[0])
			{
				int i = LoadTexture(pEffect->m_strTextureFile);
				if(m_nTextureRenderCount[i] == 2)
					m_nTextureRenderCount[i]++;
				pOrgTex = m_pObjEffectMesh[index]->SetTexture(m_pTexture[i], 0);
			}
			m_pObjEffectMesh[index]->Render();
			if(pEffect->m_strTextureFile[0])
			{
				m_pObjEffectMesh[index]->SetTexture(pOrgTex, 0);
			}
		}
	*/
	pMesh->m_bMaterial = bTemp;
	g_pD3dDev->SetRenderState(D3DRS_SRCBLEND, dwSrc);
	g_pD3dDev->SetRenderState(D3DRS_DESTBLEND, dwDest);
	g_pD3dDev->SetTextureStageState(0, D3DTSS_COLOROP, dwColorOp);
	if (!pEffect->m_bUseEnvironmentLight)
	{
		g_pD3dDev->LightEnable(2, FALSE);
	}
	if (pEffect->m_bLightMapUse)
	{
		g_pD3dDev->SetTextureStageState(1, D3DTSS_COLOROP, dwLightColorOp);
	}
	if (pEffect->m_bAlphaTestEnble)
	{
		g_pD3dDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	}
	if (pEffect->m_bZWriteEnable == FALSE)
	{
		g_pD3dDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	}

	m_CEffect->m_nObjectParticleEffectRender++;
}

void DrawingProc::EffectPlaneRenderDP(CEffectPlane* pEffect, CEffectRender* m_CEffect)
{
	if (pEffect->m_pParent->m_nCurrentNumberOfTrace <= 0)
		return;
	int index = m_CEffect->LoadTexture(pEffect->m_pParent->m_strTextureName[pEffect->m_pParent->m_nCurrentTextureNumber]);

	DWORD dwSrc, dwDest, dwColorOp;
	g_pD3dDev->GetRenderState(D3DRS_SRCBLEND, &dwSrc);
	g_pD3dDev->GetRenderState(D3DRS_DESTBLEND, &dwDest);
	g_pD3dDev->GetTextureStageState(0, D3DTSS_COLOROP, &dwColorOp);

	//	g_pD3dDev->ApplyStateBlock( pEffect->m_dwStateBlock );
	g_pD3dDev->SetRenderState(D3DRS_ZENABLE, pEffect->m_pParent->m_bZbufferEnable);
	if (pEffect->m_pParent->m_bZWriteEnable == FALSE)
	{
		g_pD3dDev->SetRenderState(D3DRS_ZWRITEENABLE, pEffect->m_pParent->m_bZWriteEnable);
	}
	g_pD3dDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	g_pD3dDev->SetRenderState(D3DRS_ALPHABLENDENABLE, pEffect->m_pParent->m_bAlphaBlendEnable);
	g_pD3dDev->SetRenderState(D3DRS_SRCBLEND, pEffect->m_pParent->m_dwSrcBlend);
	g_pD3dDev->SetRenderState(D3DRS_DESTBLEND, pEffect->m_pParent->m_dwDestBlend);
	g_pD3dDev->SetTextureStageState(0, D3DTSS_COLOROP, pEffect->m_pParent->m_nTextureRenderState);
	g_pD3dDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	//	g_pD3dDev->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
	//	g_pD3dDev->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
		// 2005-01-03 by jschoi
	//    g_pD3dDev->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR  );
	//    g_pD3dDev->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR  );
	//	g_pD3dDev->SetSamplerState(0,D3DSAMP_MINFILTER,D3DTEXF_LINEAR);
	//	g_pD3dDev->SetSamplerState(0,D3DSAMP_MAGFILTER,D3DTEXF_LINEAR);

	D3DXMATRIX mat;
	D3DXMatrixIdentity(&mat);
	g_pD3dDev->SetTransform(D3DTS_WORLD, &mat);
	g_pD3dDev->SetFVF(D3DFVF_SPRITE_VERTEX);
	if (index >= 0)
	{
		if (pEffect->m_pParent &&
			pEffect->m_pParent->m_pParent &&
			pEffect->m_pParent->m_pParent->m_pTexture)
		{
			g_pD3dDev->SetTexture(0, pEffect->m_pParent->m_pParent->m_pTexture);
		}
		else
		{
			g_pD3dDev->SetTexture(0, m_CEffect->m_pTexture[index]);
		}
		if (m_CEffect->m_nTextureRenderCount[index] == 2)
		{
			m_CEffect->m_nTextureRenderCount[index]++;
		}
	}

	//	g_pD3dDev->SetTexture( 0, NULL);
	D3DMATERIAL9 mtrl;
	D3DUtil_InitMaterial(mtrl, 1.0f, 1.0f, 1.0f, 1.0f);
	g_pD3dDev->SetMaterial(&mtrl);
	pEffect->Render();
	g_pD3dDev->SetRenderState(D3DRS_SRCBLEND, dwSrc);
	g_pD3dDev->SetRenderState(D3DRS_DESTBLEND, dwDest);
	g_pD3dDev->SetTextureStageState(0, D3DTSS_COLOROP, dwColorOp);
	g_pD3dDev->SetRenderState(D3DRS_LIGHTING, TRUE);

	if (pEffect->m_pParent->m_bZWriteEnable == FALSE)
	{
		g_pD3dDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	}
	m_CEffect->m_nTraceEffectRender++;
}

void DrawingProc::SpriteAniRenderDP(CSpriteAni* pEffect, CEffectRender* m_CEffect)
{
	FLOG("CEffectRender::SpriteAniRender(CSpriteAni* pEffect)");
	DWORD dwSrc, dwDest, dwColorOp;
	g_pD3dDev->GetRenderState(D3DRS_SRCBLEND, &dwSrc);
	g_pD3dDev->GetRenderState(D3DRS_DESTBLEND, &dwDest);
	g_pD3dDev->GetTextureStageState(0, D3DTSS_COLOROP, &dwColorOp);

	g_pD3dDev->SetRenderState(D3DRS_LIGHTING, TRUE);
	//	g_pD3dDev->SetRenderState( D3DRS_ZENABLE, TRUE);
	g_pD3dDev->SetRenderState(D3DRS_ZENABLE, pEffect->m_bZbufferEnable);

	if (pEffect->m_bZWriteEnable == FALSE)
	{
		g_pD3dDev->SetRenderState(D3DRS_ZWRITEENABLE, pEffect->m_bZWriteEnable);
	}
	g_pD3dDev->SetRenderState(D3DRS_ALPHABLENDENABLE, pEffect->m_bAlphaBlending);
	if (pEffect->m_bAlphaBlending)
	{
		g_pD3dDev->SetRenderState(D3DRS_SRCBLEND, pEffect->m_nSrcBlend);
		g_pD3dDev->SetRenderState(D3DRS_DESTBLEND, pEffect->m_nDestBlend);
	}
	g_pD3dDev->SetTextureStageState(0, D3DTSS_COLOROP, pEffect->m_nTextureRenderState);
	//    g_pD3dDev->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR  );
	//    g_pD3dDev->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR  );
	g_pD3dDev->SetFVF(D3DFVF_SPRITE_VERTEX);

	// set light
	D3DXVECTOR3 vAxis = g_pD3dApp->m_pCamera->GetViewDir();
	m_CEffect->m_light2 = g_pD3dApp->m_pScene->m_light2;
	m_CEffect->m_light2.Direction = vAxis;
	g_pD3dDev->SetLight(2, &m_CEffect->m_light2);
	g_pD3dDev->LightEnable(2, TRUE);

	D3DXMATRIX matScale;
	//	D3DXVECTOR3 pos = pEffect->m_pParent->m_pParent->m_pParent->m_vPos + pEffect->m_pParent->m_vPos;
	D3DXVECTOR3 pos = pEffect->m_pParent->m_vPos;
	D3DXVec3TransformCoord(&pos, &pos, &pEffect->m_pParent->m_pParent->m_mMatrix);

	D3DXMatrixScaling(&matScale, pEffect->m_fTextureSize, pEffect->m_fTextureSize, pEffect->m_fTextureSize);
	matScale *= g_pD3dApp->m_pCamera->GetBillboardMatrix();
	if ((pEffect->m_pParent && pEffect->m_pParent->m_fBillboardRotateAngle > 0) ||
		pEffect->m_fCurrentRotateAngle != 0)
	{
		D3DXMATRIX matRotate;
		//		vAxis = pEffect->m_pParent->m_vPos - g_pD3dApp->m_pCamera->GetEyePt();
		//		vAxis = pos - g_pD3dApp->m_pCamera->GetEyePt();
		D3DXMatrixRotationAxis(&matRotate, &vAxis, pEffect->m_fCurrentRotateAngle);
		matScale *= matRotate;
	}
	matScale._41 = pos.x;
	matScale._42 = pos.y;
	matScale._43 = pos.z;

	D3DMATERIAL9 mtrl;
	D3DUtil_InitMaterial(mtrl, pEffect->m_cColor.r,
		pEffect->m_cColor.g, pEffect->m_cColor.b, pEffect->m_cColor.a);

	g_pD3dDev->SetTransform(D3DTS_WORLD, &matScale);
	if (pEffect->m_nTextureVertexBufferType == 0)
		g_pD3dDev->SetStreamSource(0, m_CEffect->m_pVB4[pEffect->m_nSpriteType], 0, sizeof(SPRITE_VERTEX));
	else if (pEffect->m_nTextureVertexBufferType == 1)
		g_pD3dDev->SetStreamSource(0, m_CEffect->m_pVB8[pEffect->m_nSpriteType], 0, sizeof(SPRITE_VERTEX));
	else if (pEffect->m_nTextureVertexBufferType == 2)
		g_pD3dDev->SetStreamSource(0, m_CEffect->m_pVB16[pEffect->m_nSpriteType], 0, sizeof(SPRITE_VERTEX));
	else if (pEffect->m_nTextureVertexBufferType == 3)
		g_pD3dDev->SetStreamSource(0, m_CEffect->m_pVB2[pEffect->m_nSpriteType], 0, sizeof(SPRITE_VERTEX));
	else if (pEffect->m_nTextureVertexBufferType == 4)
		g_pD3dDev->SetStreamSource(0, m_CEffect->m_pVB1, 0, sizeof(SPRITE_VERTEX));
	g_pD3dDev->SetMaterial(&mtrl);
	// set texture
	if (pEffect->m_pParent && pEffect->m_pParent->m_pTexture)
	{
		g_pD3dDev->SetTexture(0, pEffect->m_pParent->m_pTexture);
	}
	else
	{
		int index = m_CEffect->LoadTexture(pEffect->m_strTextureFile);
		if (index >= 0)
		{
			g_pD3dDev->SetTexture(0, m_CEffect->m_pTexture[index]);
			if (m_CEffect->m_nTextureRenderCount[index] == 2)
				m_CEffect->m_nTextureRenderCount[index]++;
		}
	}
	g_pD3dDev->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	if (strlen(pEffect->m_strLightMapFile) > 0)
	{
		g_pD3dDev->SetRenderState(D3DRS_ALPHABLENDENABLE, pEffect->m_bLightMapAlphaBlending);
		g_pD3dDev->SetRenderState(D3DRS_SRCBLEND, pEffect->m_nLightMapSrcBlend);
		g_pD3dDev->SetRenderState(D3DRS_DESTBLEND, pEffect->m_nLightMapDestBlend);
		g_pD3dDev->SetTextureStageState(0, D3DTSS_COLOROP, pEffect->m_nLightMapRenderState);
		//		Set texture
		if (pEffect->m_pParent && pEffect->m_pParent->m_pTexture) {
			g_pD3dDev->SetTexture(0, pEffect->m_pParent->m_pTexture);
		}
		else
		{
			int index = m_CEffect->LoadTexture(pEffect->m_strLightMapFile);
			if (index >= 0)
			{
				g_pD3dDev->SetTexture(0, m_CEffect->m_pTexture[index]);
				if (m_CEffect->m_nTextureRenderCount[index] == 2)
					m_CEffect->m_nTextureRenderCount[index]++;
			}
		}
		g_pD3dDev->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
	}
	g_pD3dDev->SetRenderState(D3DRS_SRCBLEND, dwSrc);
	g_pD3dDev->SetRenderState(D3DRS_DESTBLEND, dwDest);
	g_pD3dDev->SetTextureStageState(0, D3DTSS_COLOROP, dwColorOp);

	g_pD3dDev->LightEnable(2, FALSE);
	if (pEffect->m_bZWriteEnable == FALSE)
	{
		g_pD3dDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	}
	m_CEffect->m_nSpriteEffectRender++;
}

