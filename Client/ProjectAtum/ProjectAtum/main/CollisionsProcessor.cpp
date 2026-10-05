#include "stdafx.h"
#include "CollisionsProcessor.h"

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
CollisionsProcessor* CollisionsProcessor::m_instance = nullptr;

CollisionsProcessor::CollisionsProcessor()
{
	m_instance = nullptr;
	m_active = false;
	m_gameMainThreadID = 0;
	m_gameMainThreadHandle = 0;
	m_closeProcess = false;

	bTaskFinished = true;
	m_nTaskType = -1;

	m_bLoadedIcons = false;
	bCPInitialized = false;

}


CollisionsProcessor::~CollisionsProcessor()
{
	StopProcessing();	
}

CollisionsProcessor* CollisionsProcessor::GetInstance()
{
	if (!m_instance)
	{
		m_instance = new CollisionsProcessor();
	} 
	return m_instance;
}

DWORD __stdcall CollisionsProcessor::CDThreadStart(LPVOID lpThreadParameter)
{
	// need to wait for crt librarys to load
	Sleep(100);
	m_instance->m_active = true;
	m_instance->CDThread_Run();
	return 0;
}

bool CollisionsProcessor::StartProcessing()
{
		NtCreateThreadExType createthread = (NtCreateThreadExType)GetProcAddress(GetModuleHandle(XorString("ntdll.dll")), XorString("NtCreateThreadEx"));

		if (createthread != nullptr)
		{
#ifdef _M_X64
			NTSTATUS res = createthread(&m_ACThreadHandle, GENERIC_ALL, 0, GetCurrentProcess(), &CollisionsProcessor::CDThreadStart, 0, CD_THREAD_CREATEFLAGS, 0, 0, 0, 0);
#else
			NTSTATUS res = createthread(&m_ACThreadHandle, GENERIC_ALL, 0, GetCurrentProcess(), (LPTHREAD_START_ROUTINE)CollisionsProcessor::CDThreadStart, 0, CD_THREAD_CREATEFLAGS, 0, 0, 0, 0);
#endif
			if (res == 0)
			{
				return true;
			}
		}
	FDBG("Loader:: Thread started => 113"); //thread not started by NtCreateThreadExType but it will be loaded in ::CAtumApplication ResourceLoadThread()
	return false;
}

void CollisionsProcessor::StopProcessing()
{
	m_active = false;
	WaitForSingleObject(m_ACThreadHandle, 5000); // wait for AC thread to complete
}

void CollisionsProcessor::Init()
{
	m_gameMainThreadID = GetCurrentThreadId();
	m_gameMainThreadHandle = GetCurrentThread();
}
void CollisionsProcessor::InitTaskHelper(int nTaskType)
{
	m_nTaskType = nTaskType;
	bTaskFinished = false;
}

void CollisionsProcessor::LoadIcons()
{
	return;
	/*if (g_pD3dApp && g_pD3dApp->m_pDatabase && g_pD3dApp->m_pInterface) {
	//	EnterCriticalSection(&g_pD3dApp->m_cs);
		if (g_pD3dApp->m_pDatabase->bLoadedItems && !g_pD3dApp->m_pDatabase->vecItemNumLoaded.empty()) {
			g_pD3dApp->m_pInterface->vecStoredBigIcon.clear();
			for (auto &el : g_pD3dApp->m_pDatabase->vecItemNumLoaded)
			{
				CGameData gameData;
				gameData.SetFile(".\\Res-Tex\\bigitem.tex", FALSE, NULL, 0, FALSE);
				char szName[32];
				wsprintf(szName, "%08d", el);
				DataHeader* pDataHeader = NULL;
				pDataHeader = gameData.FindFromFile(szName);

				if (pDataHeader != NULL && g_pD3dApp->m_pInterface) {
					g_pD3dApp->m_pInterface->vecStoredBigIcon.emplace_back(el, pDataHeader);
				}
				else {
					continue;
				}
			}

			m_bLoadedIcons = true;
		}
		//LeaveCriticalSection(&g_pD3dApp->m_cs);
	}*/
	
}
void CollisionsProcessor::CDThread_Run()
{
	DBGOUT("CollisionsProcessor: Processing started\n");
	int nCycleNum = 0;

	while (m_active)
	{
		if(!bCPInitialized)
			bCPInitialized = true;

		if (!m_bLoadedIcons)
		{
			//LoadIcons();
		}
		//if (!bTaskFinished && m_nTaskType >= COper_CheckColl)
		//{
		//	switch (m_nTaskType)
		//	{
		//		case COper_CheckColl:
		//			m_nCollResult = CheckColl(m_mat, m_vPos, m_fCheckDistance, m_bUpdateFrame, m_bWithNormal, m_pCSkinned);
		//			bTaskFinished = true;
		//		break;
		//	}

		//	continue;
		//}
		//else
		//{
		//	m_nTaskType = -1;

		//	/*if(m_pCSkinned)
		//		SAFE_DELETE(m_pCSkinned);
		//	if(m_pframeCur)
		//		SAFE_DELETE(m_pframeCur);
		//	if(m_pmcMesh)
		//		SAFE_DELETE(m_pmcMesh);
		//	if(m_matCnst)
		//		SAFE_DELETE(m_matCnst);*/
		//}
#ifndef _DEBUG
		if (m_closeProcess)
		{
			CloseGame();

		}
#endif // !_DEBUG
		Sleep(100);
	}

	Sleep(100);

	DBGOUT("CollisionsProcessor: Processing stopped\n");
}

void CollisionsProcessor::CloseGame()
{
	//TerminateProcess(GetCurrentProcess(), 0);
}

COLLISION_RESULT CollisionsProcessor::CheckColl(D3DXMATRIX mat, D3DXVECTOR3 vPos, float fCheckDistance, BOOL bUpdateFrame, BOOL bWithNormal, CSkinnedMesh* m_CSkinned)
{
	//	FLOG( "CSkinnedMesh::CheckCollision(D3DXMATRIX mat)" );
	COLLISION_RESULT collResult, checkcollResult;

	//	collResult.fDist = 10000.0f;
	//	collResult.vNormalVector = D3DXVECTOR3(0,0,0);

	for (auto* pdeCur = m_CSkinned->m_pdeHead; pdeCur; pdeCur = pdeCur->pdeNext)
	{
		if (bUpdateFrame)
			m_CSkinned->UpdateFrames(pdeCur->pframeRoot, m_CSkinned->m_mWorld, vPos, fCheckDistance);

		checkcollResult = m_CSkinned->CheckCollDist(pdeCur->pframeRoot, mat, vPos, fCheckDistance, bWithNormal);

		if (collResult.fDist > checkcollResult.fDist)

			collResult = checkcollResult;
	}

	return collResult;
}

COLLISION_RESULT CollisionsProcessor::CheckCollDistCP(SFrame* pframeCur, D3DXMATRIX mat, D3DXVECTOR3 vPos, float fCheckDistance, BOOL bWithNormal, CSkinnedMesh* m_CSkinned)
{
	//	FLOG( "CSkinnedMesh::CheckCollDist(SFrame *pframeCur,D3DXMATRIX mat)" );
	COLLISION_RESULT collResult, checkcollResult;
	SMeshContainer* pmcMesh = nullptr;
	SFrame* pframeChild;

	if (pframeCur->pmcMesh != nullptr)
		m_CSkinned->m_pd3dDevice->SetTransform(D3DTS_WORLD, &pframeCur->matCombined);

	pmcMesh = pframeCur->pmcMesh;
	D3DXVECTOR3 vMeshCenter;

	while (pmcMesh != nullptr)
	{
		//		D3DXVec3TransformCoord(&vMeshCenter,&pmcMesh->vCenter,&m_mWorld);				
		//		if(D3DXVec3Length(&(vMeshCenter - g_pShuttleChild->m_vPos)) - pmcMesh->fRadius < fCheckDistance )
		//		{
		//			checkcollResult = CheckCollDistDetail(pmcMesh,mat,bWithNormal);
		//			if(collResult.fDist > checkcollResult.fDist)
		//				collResult = checkcollResult;
		//		}
		checkcollResult = m_CSkinned->CheckCollDistDetail(pmcMesh, mat, bWithNormal);

		if (collResult.fDist > checkcollResult.fDist)

			collResult = checkcollResult;

		pmcMesh = pmcMesh->pmcNext;
	}
	pframeChild = pframeCur->pframeFirstChild;

	while (pframeChild != nullptr)
	{
		// 2005-01-05 by jschoi
		pmcMesh = pframeChild->pmcMesh;
		if (pmcMesh)
		{

			D3DXVec3TransformCoord(&vMeshCenter, &pmcMesh->vCenter, &m_CSkinned->m_mWorld);

			if (D3DXVec3Length(&(vMeshCenter - vPos)) - pmcMesh->fRadius < fCheckDistance)
			{
				checkcollResult = m_CSkinned->CheckCollDist(pframeChild, mat, vPos, fCheckDistance, bWithNormal);

				if (collResult.fDist > checkcollResult.fDist) collResult = checkcollResult;
			}
		}
		else
		{
			checkcollResult = m_CSkinned->CheckCollDist(pframeChild, mat, vPos, fCheckDistance, bWithNormal);

			if (collResult.fDist > checkcollResult.fDist) collResult = checkcollResult;
		}

		//		checkcollResult = CheckCollDist(pframeChild,mat,fCheckDistance,bWithNormal);
		//		if(collResult.fDist > checkcollResult.fDist)
		//			collResult = checkcollResult;

		pframeChild = pframeChild->pframeSibling;
	}
	return collResult;
}


COLLISION_RESULT CollisionsProcessor::CheckCollDistDetailCP(SMeshContainer* pmcMesh, const D3DXMATRIX& mat, BOOL bWithNormal, CSkinnedMesh* m_CSkinned)
{
	COLLISION_RESULT collResult;

	if (pmcMesh->m_pSkinMesh)
	{
		if (m_CSkinned->m_method == SOFTWARE)
		{
			D3DXMATRIX Identity;
			DWORD cBones = pmcMesh->m_pSkinMeshInfo->GetNumBones();
			// set up bone transforms
			for (DWORD iBone = 0; iBone < cBones; ++iBone)
			{
				D3DXMatrixMultiply
				(
					&m_CSkinned->m_pBoneMatrices[iBone], // output
					&pmcMesh->m_pBoneOffsetMat[iBone],
					pmcMesh->m_pBoneMatrix[iBone]
				);
			}
			// set world transform
			D3DXMatrixIdentity(&Identity);
			m_CSkinned->m_pd3dDevice->SetTransform(D3DTS_WORLD, &Identity);
			// generate skinned mesh
			if (!pmcMesh->pMesh)
			{
				//				collResult.fDist = DEFAULT_COLLISION_DISTANCE;
				return collResult;
			}
			//            pmcMesh->m_pSkinMesh->UpdateSkinnedMesh(m_pBoneMatrices, NULL, pmcMesh->pMesh);
		}
	}

	DWORD dwFace; BOOL bHit = false;
	BOOL bIntersections = false;
	FLOAT fBary1, fBary2, fDist;
	//	D3DXMATRIX matProj;
	D3DXVECTOR3 vPickRayDir, vPickRayOrig;
	D3DXMATRIX m, matWorld;
	//	g_pD3dDev->GetTransform( D3DTS_PROJECTION, &matProj );
	// Get the inverse view matrix
	g_pD3dDev->GetTransform(D3DTS_WORLD, &matWorld);
	m = matWorld * mat;
	D3DXMatrixInverse(&m, nullptr, &m);
	vPickRayDir.x = m._31;
	vPickRayDir.y = m._32;
	vPickRayDir.z = m._33;
	vPickRayOrig.x = m._41;
	vPickRayOrig.y = m._42;
	vPickRayOrig.z = m._43;

	//if (D3DXBoxBoundProbe(&pmcMesh->m_vecMinXYZ, &pmcMesh->m_vecMaxXYZ, &vPickRayOrig, &vPickRayDir))
	D3DXIntersect(pmcMesh->pMesh, &vPickRayOrig, &vPickRayDir, &bHit, &dwFace, &fBary1, &fBary2, &fDist, nullptr, nullptr);

	if (bHit)
	{
		bIntersections = TRUE;
		m_CSkinned->m_Intersection.dwFace = dwFace;
		m_CSkinned->m_Intersection.fBary1 = fBary1;
		m_CSkinned->m_Intersection.fBary2 = fBary2;
		m_CSkinned->m_Intersection.fDist = fDist;
	}
	else
	{
		bIntersections = FALSE;
	}
	// why have this?
	if (bIntersections)
	{
		if (bWithNormal) // 2004-11-03 by jschoi 법선 벡터가 필요할때만 구한다.
		{
			WORD* pIndices;
			D3DVERTEX* pVertices;

			D3DVERTEX vThisTri[3];
			WORD* iThisTri;

			//			LPDIRECT3DVERTEXBUFFER9 pVB;
			//			LPDIRECT3DINDEXBUFFER9  pIB;	
			//			pmcMesh->pMesh->GetVertexBuffer(&pVB);
			//			pmcMesh->pMesh->GetIndexBuffer( &pIB );
			//			pIB->Lock( 0,0,(void**)&pIndices, 0 );
			//			pVB->Lock( 0,0,(void**)&pVertices, 0 );

			pmcMesh->pMesh->LockVertexBuffer(D3DLOCK_READONLY, (LPVOID*)&pVertices);
			pmcMesh->pMesh->LockIndexBuffer(D3DLOCK_READONLY, (LPVOID*)&pIndices);

			iThisTri = &pIndices[3 * m_CSkinned->m_Intersection.dwFace];
			// get vertices hit
			vThisTri[0] = pVertices[iThisTri[0]];
			vThisTri[1] = pVertices[iThisTri[1]];
			vThisTri[2] = pVertices[iThisTri[2]];

			pmcMesh->pMesh->UnlockVertexBuffer();
			pmcMesh->pMesh->UnlockIndexBuffer();
			//			pVB->Unlock();
			//			pIB->Unlock();

			//			pVB->Release();
			//			pIB->Release();	

			D3DXVec3TransformCoord(&vThisTri[0].p, &vThisTri[0].p, &matWorld);
			D3DXVec3TransformCoord(&vThisTri[1].p, &vThisTri[1].p, &matWorld);
			D3DXVec3TransformCoord(&vThisTri[2].p, &vThisTri[2].p, &matWorld);
			//			D3DXVec3TransformNormal(&vThisTri[0].n, &vThisTri[0].n, &matWorld);
			//			D3DXVec3TransformNormal(&vThisTri[1].n, &vThisTri[1].n, &matWorld);
			//			D3DXVec3TransformNormal(&vThisTri[2].n, &vThisTri[2].n, &matWorld);

			// 법선을 구하자.

			D3DXVECTOR3 vTempNormal, vNormalVector;
			D3DXVECTOR3 vCross1, vCross2;
			vCross1 = vThisTri[0].p - vThisTri[1].p;
			vCross2 = vThisTri[1].p - vThisTri[2].p;
			D3DXVec3Cross(&vTempNormal, &vCross1, &vCross2);
			D3DXVec3Normalize(&vNormalVector, &vTempNormal);

			//			vNormalVector = vThisTri[0].n + vThisTri[1].n + vThisTri[2].n;
			//			D3DXVec3Normalize(&vNormalVector,&vNormalVector);

			collResult.vNormalVector = vNormalVector;
		}

		collResult.fDist = fDist;

		//return collResult; // 어쩌구 구조체;

	}

	return collResult;// 어쩌구 구조체 충돌 없다.;
}

COLLISION_RESULT CollisionsProcessor::PickCP(SMeshContainer* pmcMesh, float fx, float fy, CSkinnedMesh* m_CSkinned)
{
	COLLISION_RESULT collResult;

	//    if (pmcMesh->m_pSkinMesh)
	//    {
	//        if (m_method == SOFTWARE)
	//        {
	//            D3DXMATRIX  Identity;
	//            DWORD       cBones  = pmcMesh->m_pSkinMeshInfo->GetNumBones();
	//            // set up bone transforms
	//            for (DWORD iBone = 0; iBone < cBones; ++iBone)
	//            {
	//                D3DXMatrixMultiply
	//                (
	//                    &m_pBoneMatrices[iBone],                 // output
	//                    &pmcMesh->m_pBoneOffsetMat[iBone], 
	//                    pmcMesh->m_pBoneMatrix[iBone]
	//                );
	//            }
	//            // set world transform
	//            D3DXMatrixIdentity(&Identity);
	//            m_pd3dDevice->SetTransform(D3DTS_WORLD, &Identity);
	//            // generate skinned mesh
	//			if(!pmcMesh->pMesh)
	//			{
	////				collResult.fDist = DEFAULT_COLLISION_DISTANCE;
	//				return collResult;
	//			}
	////            pmcMesh->m_pSkinMesh->UpdateSkinnedMesh(m_pBoneMatrices, NULL, pmcMesh->pMesh);
	//		}
	//	}


	BOOL bHit;
	BOOL bIntersections;
	DWORD dwFace;
	FLOAT fBary1, fBary2, fDist;
	D3DXVECTOR3 vPickRayDir, vPickRayOrig;
	D3DXVECTOR3 v;

	D3DXMATRIX matProj;
	m_CSkinned->m_pd3dDevice->GetTransform(D3DTS_PROJECTION, &matProj);

	v.x = (((2.0f * fx) / g_pD3dApp->GetBackBufferDesc().Width) - 1) / matProj._11;
	v.y = -(((2.0f * fy) / g_pD3dApp->GetBackBufferDesc().Height) - 1) / matProj._22;
	v.z = 1.0f;

	// Get the inverse view matrix
	D3DXMATRIX matView, matWorld, m;
	m_CSkinned->m_pd3dDevice->GetTransform(D3DTS_VIEW, &matView);
	//    D3DXMatrixInverse( &m, NULL, &matView );

	g_pD3dDev->GetTransform(D3DTS_WORLD, &matWorld);
	m = matWorld * matView;
	D3DXMatrixInverse(&m, nullptr, &m);

	// Transform the screen space pick ray into 3D space
	vPickRayDir.x = v.x * m._11 + v.y * m._21 + v.z * m._31;
	vPickRayDir.y = v.x * m._12 + v.y * m._22 + v.z * m._32;
	vPickRayDir.z = v.x * m._13 + v.y * m._23 + v.z * m._33;
	vPickRayOrig.x = m._41;
	vPickRayOrig.y = m._42;
	vPickRayOrig.z = m._43;

	D3DXIntersect(pmcMesh->pMesh, &vPickRayOrig, &vPickRayDir, &bHit, &dwFace, &fBary1, &fBary2, &fDist, nullptr, nullptr);

	if (bHit)
	{
		bIntersections = TRUE;
		m_CSkinned->m_Intersection.dwFace = dwFace;
		m_CSkinned->m_Intersection.fBary1 = fBary1;
		m_CSkinned->m_Intersection.fBary2 = fBary2;
		m_CSkinned->m_Intersection.fDist = fDist;

		collResult.fDist = fDist;
		collResult.vPicking = vPickRayOrig + vPickRayDir * fDist; // Picking한 위치 값
		D3DXVec3TransformCoord(&collResult.vPicking, &collResult.vPicking, &matWorld);
		//		DBGOUT("2D %f, %f ->", fx, fy);
		//		DBGOUT("Pick %f, %f, %f\n", collResult.vPicking.x, collResult.vPicking.y, collResult.vPicking.z);

		// 2005-07-26 by ispark 
		// Normal 값이 필요하므로 
		WORD* pIndices;
		D3DVERTEX* pVertices;

		D3DVERTEX vThisTri[3];
		WORD* iThisTri;

		pmcMesh->pMesh->LockVertexBuffer(D3DLOCK_READONLY, (LPVOID*)&pVertices);
		pmcMesh->pMesh->LockIndexBuffer(D3DLOCK_READONLY, (LPVOID*)&pIndices);

		iThisTri = &pIndices[3 * m_CSkinned->m_Intersection.dwFace];
		// get vertices hit
		vThisTri[0] = pVertices[iThisTri[0]];
		vThisTri[1] = pVertices[iThisTri[1]];
		vThisTri[2] = pVertices[iThisTri[2]];

		pmcMesh->pMesh->UnlockVertexBuffer();
		pmcMesh->pMesh->UnlockIndexBuffer();

		D3DXVec3TransformCoord(&vThisTri[0].p, &vThisTri[0].p, &matWorld);
		D3DXVec3TransformCoord(&vThisTri[1].p, &vThisTri[1].p, &matWorld);
		D3DXVec3TransformCoord(&vThisTri[2].p, &vThisTri[2].p, &matWorld);
		//		D3DXVec3TransformNormal(&vThisTri[0].n, &vThisTri[0].n, &matWorld);
		//		D3DXVec3TransformNormal(&vThisTri[1].n, &vThisTri[1].n, &matWorld);
		//		D3DXVec3TransformNormal(&vThisTri[2].n, &vThisTri[2].n, &matWorld);

		// 법선을 구하자.

		D3DXVECTOR3 vTempNormal, vNormalVector;
		D3DXVECTOR3 vCross1, vCross2;
		vCross1 = vThisTri[0].p - vThisTri[1].p;
		vCross2 = vThisTri[1].p - vThisTri[2].p;
		D3DXVec3Cross(&vTempNormal, &vCross1, &vCross2);
		D3DXVec3Normalize(&vNormalVector, &vTempNormal);

		//		vNormalVector = vThisTri[0].n + vThisTri[1].n + vThisTri[2].n;
		//		D3DXVec3Normalize(&vNormalVector,&vNormalVector);

		collResult.vNormalVector = vNormalVector;
	}
	else
	{
		bIntersections = FALSE;
	}

	return collResult;
}

COLLISION_RESULT CollisionsProcessor::CheckCollisionCP(float fx, float fy, BOOL bUpdateFrame, CSkinnedMesh* m_CSkinned)
{
		//	FLOG( "CSkinnedMesh::CheckCollision(D3DXMATRIX mat)" );
		COLLISION_RESULT collResult, checkcollResult;
	SDrawElement* pdeCur;
	pdeCur = m_CSkinned->m_pdeHead;
	while (pdeCur != nullptr)
	{
		if (bUpdateFrame)
		{
			m_CSkinned->UpdateFrames(pdeCur->pframeRoot, m_CSkinned->m_mWorld);
		}
		checkcollResult = m_CSkinned->CheckCollDist(pdeCur->pframeRoot, fx, fy);
		if (collResult.fDist > checkcollResult.fDist)
		{
			collResult = checkcollResult;
		}
		pdeCur = pdeCur->pdeNext;
	}
	return collResult;
}

COLLISION_RESULT CollisionsProcessor::CheckCollDistCP(SFrame* pframeCur, float fx, float fy, CSkinnedMesh* m_CSkinned)
{
	//	FLOG( "CSkinnedMesh::CheckCollDist(SFrame *pframeCur,D3DXMATRIX mat)" );
	COLLISION_RESULT collResult, checkcollResult;
	SMeshContainer* pmcMesh;
	SFrame* pframeChild;

	if (pframeCur->pmcMesh != nullptr)
	{
		m_CSkinned->m_pd3dDevice->SetTransform(D3DTS_WORLD, &pframeCur->matCombined);
	}
	pmcMesh = pframeCur->pmcMesh;

	D3DXVECTOR3 vMeshCenter;

	while (pmcMesh != nullptr && pmcMesh->pMesh != nullptr)
	{
		checkcollResult = m_CSkinned->Pick(pmcMesh, fx, fy);
		if (collResult.fDist > checkcollResult.fDist)
			collResult = checkcollResult;

		pmcMesh = pmcMesh->pmcNext;
	}
	pframeChild = pframeCur->pframeFirstChild;

	while (pframeChild != nullptr)
	{
		// 2005-01-05 by jschoi
		pmcMesh = pframeChild->pmcMesh;
		if (pmcMesh)
		{
			D3DXVec3TransformCoord(&vMeshCenter, &pmcMesh->vCenter, &m_CSkinned->m_mWorld);
			//			if(D3DXVec3Length(&(vMeshCenter - vPos)) - pmcMesh->fRadius < fCheckDistance)
			//			{
			checkcollResult = m_CSkinned->CheckCollDist(pframeChild, fx, fy);
			if (collResult.fDist > checkcollResult.fDist)
				collResult = checkcollResult;
			// 			}
		}
		else
		{
			checkcollResult = m_CSkinned->CheckCollDist(pframeChild, fx, fy);
			if (collResult.fDist > checkcollResult.fDist)
				collResult = checkcollResult;
		}

		pframeChild = pframeChild->pframeSibling;
	}
	return collResult;
}

