// EffectRender.cpp: implementation of the CEffectRender class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AtumApplication.h"
#include "EffectRender.h"
#include "ObjectChild.h"
#include "GameDataLast.h"
#include "SpriteAniData.h"
#include "ParticleSystem.h"
#include "ObjectAniData.h"
#include "TraceAni.h"
#include "EnemyData.h"
#include "MonsterData.h"
#include "SkinnedMesh.h"
#include "SceneData.h"
#include "Camera.h"
#include "Background.h"
#include "Weapon.h"
#include "ShuttleChild.h"
#include "CharacterChild.h"				// 2005-07-21 by ispark
#include "ItemData.h"
#include "dxutil.h"
#include "SunData.h"
#include "D3DHanFont.h"
#include "ObjRender.h"
#include "SkillEffect.h"
#include "TutorialSystem.h"
#include "Frustum.h"
#include "INFGameMain.h"	// 2008-08-22 by bhsohn EP3 ŔÎşĄĹä¸® Ăł¸®
#include "MeshInitThread.h"  // 2009. 11. 23 by jskim ¸®ĽŇ˝ş ·Îµů ±¸Á¶ şŻ°ć
#include "DrawingProc.h"
extern LPDIRECT3DDEVICE9	g_pD3dDev;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEffectRender::CEffectRender()
{
	FLOG( "CEffectRender()" );
	m_pVB1 = NULL;
	memset(m_pVB2,0x00,4*2);
	memset(m_pVB4,0x00,4*4);
	memset(m_pVB8,0x00,4*8);
	memset(m_pVB16,0x00,4*16);

	m_pTexEffectData = NULL;
	memset(m_pTexture, 0x00, TEX_EFFECT_NUM*sizeof(DWORD));
	memset(m_nTextureRenderCount, 0x00, TEX_EFFECT_NUM*sizeof(int));
//	memset(m_pObjEffectMesh, 0x00, OBJ_EFFECT_NUM*sizeof(DWORD));
	m_fTextureCheckTime = 300.0f;// 5şĐżˇ ÇŃąřľż ŔĚĆĺĆ® »çżëż©şÎ °Ë»ç

	m_pEffectData = NULL;
	m_pEffectData2 = NULL;
	m_pObjectData = NULL;
	m_pObjectData2 = NULL;
	

	m_bZBufferTemp = FALSE;
	m_nParticleEffectCount = 0;
	m_nSpriteEffectCount = 0;
	m_nObjectEffectCount = 0;
	m_nTraceEffectCount = 0;
	m_nParticleEffectRender = 0;
	m_nObjectParticleEffectRender = 0;
	m_nSpriteEffectRender = 0;
	m_nObjectEffectRender = 0;
	m_nTraceEffectRender = 0;

	DBGOUT_EFFECT("-------------------- Effect wear item postion --------------------\n");
	DBGOUT_EFFECT("1-1Type Weapon		:	0\n");
	DBGOUT_EFFECT("1-2Type Weapon		:	1\n");
	DBGOUT_EFFECT("2-1Type Weapon		:	2\n");
	DBGOUT_EFFECT("2-2Type Weapon		:	3\n");
	DBGOUT_EFFECT("Front(Rader)			:	4\n");
	DBGOUT_EFFECT("Center(Armor)		:	5\n");
	DBGOUT_EFFECT("Option(ATTACH)		:	6\n");
	DBGOUT_EFFECT("Back(Engine)			:	7\n");
	DBGOUT_EFFECT("-------------------- wear item postion --------------------\n");

	// 2007-11-08 by bhsohn ŔÎşĄ ŔĚĆĺĆ® °ü·Ă Ăł¸®
	// ŔÎşĄ ĆÄĆĽĹ¬ ĂĘ±âČ­
	m_vecInvenParticleInfo.clear();
}

CEffectRender::~CEffectRender()
{
	FLOG( "~CEffectRender()" );
	SAFE_RELEASE(m_pVB1);
	for(int i=0;i<2;i++)
		SAFE_RELEASE(m_pVB2[i]);
	for(int i=0;i<4;i++)
		SAFE_RELEASE(m_pVB4[i]);
	for(int i=0;i<8;i++)
		SAFE_RELEASE(m_pVB8[i]);
	for(int i=0;i<16;i++)
		SAFE_RELEASE(m_pVB16[i]);

	SAFE_DELETE(m_pTexEffectData);
	SAFE_DELETE(m_pEffectData);
	SAFE_DELETE(m_pEffectData2);
	SAFE_DELETE(m_pObjectData);
	SAFE_DELETE(m_pObjectData2);
	for(int i=0;i<TEX_EFFECT_NUM;i++)
		SAFE_RELEASE(m_pTexture[i]);
//	for(i=0;i<OBJ_EFFECT_NUM;i++)
//		SAFE_DELETE(m_pObjEffectMesh[i]);
}
/*
int CEffectRender::GetEmptyObjectIndex()
{
	FLOG( "CEffectRender::GetEmptyObjectIndex()" );
	int index = 0;
	for(int i=0;i<OBJ_EFFECT_NUM;i++)
	{
		if(!m_pObjEffectMesh[i])
			return i;
	}
	return -1;

}
*/

// 2009. 11. 23 by jskim ¸®ĽŇ˝ş ·Îµů ±¸Á¶ şŻ°ć
//CSkinnedMesh* CEffectRender::LoadObject(char* strName)
CSkinnedMesh* CEffectRender::LoadObject(char* strName, int LoadingPriority)
//end 2009. 11. 23 by jskim ¸®ĽŇ˝ş ·Îµů ±¸Á¶ şŻ°ć
{
	FLOG( "CEffectRender::LoadObject(char* strName)" );
	if(strlen(strName)<=0)
	{
		return NULL;
	}
	map<string,CSkinnedMesh*>::iterator it = m_mapObjNameToMesh.find(strName);
	if(it == m_mapObjNameToMesh.end())
	{
		m_vecLoadObj.push_back(strName);

		// 2009. 11. 23 by jskim ¸®ĽŇ˝ş ·Îµů ±¸Á¶ şŻ°ć
		LoadingPriorityInfo temp;
		temp.LoadingPriority = LoadingPriority;
		strncpy(temp.chEffectName,strName,32);
		m_vecLoadingPriority.push_back(temp);
		//end 2009. 11. 23 by jskim ¸®ĽŇ˝ş ·Îµů ±¸Á¶ şŻ°ć
//		char strPath[MAX_PATH];
//		CGameData* pMeshData = new CGameData();
//		g_pD3dApp->LoadPath( strPath, IDS_DIRECTORY_EFFECT, strName );
//		if(pMeshData->SetFile(strPath,FALSE, NULL, 0))
//		{
//			CSkinnedMesh* pMesh = new CSkinnedMesh(FALSE);
//			pMesh->InitDeviceObjects();
//			pMesh->LoadMeshHierarchyFromMem( pMeshData );
//			pMesh->m_nRenderCount = 2;
//			m_mapObjNameToMesh[strName] = pMesh;
//			delete pMeshData;
//			pMeshData = NULL;
//			return pMesh;
//		}
//		else
//		{
//			DBGOUT("ŔĚĆĺĆ® OBJECT ĆÄŔĎŔĚ ľř˝Ŕ´Ď´Ů.(%s)",strName);
//			return NULL;
//		}
	}
	else
	{
		return it->second;
	}
	return NULL;
}
void CEffectRender::LoadObjectToMap(char* strName)
{
	FLOG("CEffectRender::LoadObject(char* strName)");
	if (strlen(strName) <= 0)
	{
		return;
	}
	map<string, CSkinnedMesh*>::iterator it = m_mapObjNameToMesh.find(strName);
	if (it == m_mapObjNameToMesh.end())
	{
		int  tempPriority = _NOTHING_STEP;
		//vector<LoadingPriorityInfo>::iterator it1 = m_vecLoadingPriority.begin();
		/*for (auto& el : m_vecLoadingPriority)
		{
			if (!strcmp(el.chEffectName, strName))
			{
				tempPriority = el.LoadingPriority;
				break;
			}
		}*/
		//end 2009. 11. 23 by jskim 리소스 로딩 구조 변경
		CSkinnedMesh* pMesh = new CSkinnedMesh(TRUE);
		pMesh->InitDeviceObjects();
		pMesh->m_nRenderCount = 2;
		m_mapObjNameToMesh[strName] = pMesh;

		if (g_pD3dApp->m_dwGameState != _GAME || tempPriority == _NOTHING_STEP)
		{
			char strPath[MAX_PATH];
			CGameData* pMeshData = new CGameData();
			g_pD3dApp->LoadPath(strPath, IDS_DIRECTORY_EFFECT, strName);
			if (pMeshData->SetFile(strPath, FALSE, NULL, 0))
			{
#ifdef _DEBUG_SHIT
				// 보안 코드
				char buf[64];
				memset(buf, 0x00, sizeof(buf));
				strncpy(buf, strName, 8);
				if (pMeshData->Find(buf) == NULL)
				{
					SAFE_DELETE(pMeshData);
					g_pD3dApp->NetworkErrorMsgBox(STRERR_C_RESOURCE_0001);
					DBGOUT("EffectRender::Resource File Error(%s)\n", strName);
					return;
				}
#endif
				pMesh->LoadMeshHierarchyFromMem(pMeshData);
				SAFE_DELETE(pMeshData);
				return;
			}
			else
			{
				SAFE_DELETE(pMeshData);
				DBGOUT("Can't Find Effect OBJECT File.(%s)\n", strName);
				return;
			}
		}
		else
		{
			structLoadingGameInfo* LoadingGameInfo = new structLoadingGameInfo;
			strcpy(LoadingGameInfo->MeshName, strName);
			LoadingGameInfo->MeshType = _EFFECT_TYPE;
			LoadingGameInfo->pSkinnedMesh = pMesh;
			LoadingGameInfo->LoadingPriority = tempPriority;
			EnterCriticalSection(&g_pD3dApp->m_cs);
			g_pD3dApp->m_pMeshInitThread->QuePushGameData(LoadingGameInfo);
			LeaveCriticalSection(&g_pD3dApp->m_cs);
		}
		//end 2009. 11. 23 by jskim 리소스 로딩 구조 변경
	}
}
/*
int CEffectRender::LoadObject(char* strName)
{
	FLOG( "CEffectRender::LoadObject(char* strName)" );
	map<string,int>::iterator it = m_mapObjNameToIndex.find(strName);
	if(it == m_mapObjNameToIndex.end())
	{
		char strPath[MAX_PATH];
		CGameData* pMeshData = new CGameData();
		g_pD3dApp->LoadPath( strPath, IDS_DIRECTORY_EFFECT, strName );
		pMeshData->SetFile(strPath,FALSE, NULL, 0);
		int index = GetEmptyObjectIndex();
		if(index>=0)
		{
			m_pObjEffectMesh[index] = new CSkinnedMesh(FALSE);
			m_pObjEffectMesh[index]->InitDeviceObjects();
			m_pObjEffectMesh[index]->LoadMeshHierarchyFromMem( pMeshData );
			m_pObjEffectMesh[index]->m_nRenderCount = 2;
			m_mapObjNameToIndex[strName] = index;
			delete pMeshData;
			pMeshData = NULL;
			return index;
		}
		else
		{
			DBGOUT("ERROR  CEffectRender::LoadObject, OBJ_EFFECT_NUM Ć÷ŔÎĹÍ ąöĆŰ °łĽö°ˇ şÎÁ·ÇŐ´Ď´Ů.\n");
			return -1;
		}
	}
	else
	{
		return it->second;
	}
}
*/
int CEffectRender::GetEmptyTextureIndex()
{
	FLOG( "CEffectRender::GetEmptyTextureIndex()" );
	int index = 0;
	for(int i=0;i<TEX_EFFECT_NUM;i++)
	{
		if(!m_pTexture[i])
			return i;
	}
	return -1;
}

DataHeader* CEffectRender::FindEffectInfo(char* strName)
{
	FLOG( "CEffectRender::FindEffectInfo(char* strName)" );

	if (g_pD3dApp->m_bTrailMod) {
		if (!m_pEffectData2 && !m_pEffectData)
			return NULL;
	}
	else
	{
		if(!m_pEffectData)
			return NULL;
	}

	char name[9];
	strncpy(name, strName, 8);
	name[8] = '\0';
	//DBGOUT("FindEffectInfo: %s\n", name);
	if (g_pD3dApp->m_bTrailMod) {
		DataHeader* tmpRet = m_pEffectData2->Find(name);
		if(tmpRet == nullptr)
			return m_pEffectData->Find(name);
		else
			return tmpRet;
	}
	else
	{
		return m_pEffectData->Find(name);
	}
}

DataHeader* CEffectRender::FindObjectInfo(char* strName)
{
	FLOG( "CEffectRender::FindObjectInfo(char* strName)" );

	if (g_pD3dApp->m_bTrailMod) {
		if (!m_pObjectData2 && !m_pObjectData)
			return NULL;
	}
	else
	{
		if (!m_pObjectData)
			return NULL;
	}

	char name[9];
	strncpy(name, strName, 8);
	name[8] = '\0';
	//DBGOUT("FindObjectInfo: %s\n", name);
	if (g_pD3dApp->m_bTrailMod) {
		DataHeader* tmpRet = m_pObjectData2->Find(name);
		if (tmpRet == nullptr)
			return m_pObjectData->Find(name);
		else
			return tmpRet;
	}
	else
	{
		return m_pObjectData->Find(name);
	}	
}

int CEffectRender::LoadTexture(char* strName)
{
	FLOG( "CEffectRender::LoadTexture(char* strName)" );
	if(!m_pTexEffectData)
		return -1;
	if(strlen(strName) <= 0)
	{
		return -1;
	}

	char name[9];
	strncpy(name, strName, 8);
	name[8] = '\0';
	//DBGOUT("LoadTexture: %s\n", name);
	
	map<string,int>::iterator it = m_mapTexNameToIndex.find(name);
	if(it == m_mapTexNameToIndex.end())
	{
		DataHeader * pDataHeader = m_pTexEffectData->Find(name);
		if(pDataHeader)
		{
			int index = GetEmptyTextureIndex();
			if (index<0)
			{
				DBGOUT("ERROR, CEffectRender::LoadTexture, GetEmptyTextureIndex() < 0\n");
				return -1;
			}
			// load texture from memory
			HRESULT hr = D3DXCreateTextureFromFileInMemory(g_pD3dDev, pDataHeader->m_pData, pDataHeader->m_DataSize, &m_pTexture[index]);
			if (hr != D3D_OK)
			{
				m_pTexture[index] = NULL;
				DBGOUT("ERROR, CEffectRender::LoadTexture, D3DXCreateTextureFromFileInMemory() != D3D_OK\n");
				return -1;
			}
			m_nTextureRenderCount[index] = 2;
			m_mapTexNameToIndex[name] = index;
			return index;
		}
	} 
	else
	{
		return it->second;
	}
	DBGOUT("ERROR, CEffectRender::LoadTexture (%s)\n", strName);
	return -1;
}

void CEffectRender::Tick(float fElapsedTime)
{
	FLOG( "CEffectRender::Tick(float fElapsedTime)" );
	vector<string>::iterator it = m_vecLoadObj.begin();
	while(it != m_vecLoadObj.end() )
	{
		LoadObjectToMap((char*)(*it).c_str());
		it++;
	}
	m_vecLoadObj.clear();
	// texture delete : 60ĂĘżˇ ÇŃąřľż
	m_fTextureCheckTime -= fElapsedTime * max(1,(g_pD3dApp->m_fFPS/100));
	if(m_fTextureCheckTime>0.0f)
		return;
	// 2009. 11. 23 by jskim ¸®ĽŇ˝ş ·Îµů ±¸Á¶ şŻ°ć
	//m_fTextureCheckTime = 300.0f;
	if( g_pD3dApp->IsEmptyLoadingGameDataList() )
	{
		m_fTextureCheckTime = 300.0f;
	}
	else
	{
		m_fTextureCheckTime = 30.0f;
		return;
	}
	//end 2009. 11. 23 by jskim ¸®ĽŇ˝ş ·Îµů ±¸Á¶ şŻ°ć
	
	for(int i=0;i<TEX_EFFECT_NUM;i++)
	{
		if(m_nTextureRenderCount[i] == 0)
			continue;
//		m_nTextureRenderCount[i]--;
		if(m_nTextureRenderCount[i]==2)// if(m_nTextureRenderCount[i] == 1)
		{
			map<string,int>::iterator it = m_mapTexNameToIndex.begin();
			while(it != m_mapTexNameToIndex.end())
			{
				if(it->second == i)
				{
					it = m_mapTexNameToIndex.erase(it);
					break;
				}
				it++;
			}
			m_nTextureRenderCount[i] = 0;
			SAFE_RELEASE(m_pTexture[i]);
		}
		else// if(m_nTextureRenderCount[i] >= 2)
		{
			m_nTextureRenderCount[i] = 2;
		}
	}
/*	for(i=0;i<OBJ_EFFECT_NUM;i++)
	{
		if(m_pObjEffectMesh[i] && m_pObjEffectMesh[i]->m_nRenderCount)
		{
			if(m_pObjEffectMesh[i]->m_nRenderCount == 2)// if(m_nObjRenderCount[i] == 1)
			{
				map<string,int>::iterator it = m_mapObjNameToIndex.begin();
				while(it != m_mapObjNameToIndex.end())
				{
					if(it->second == i)
					{
						m_mapObjNameToIndex.erase(it);
						break;
					}
					it++;
				}
				m_pObjEffectMesh[i]->InvalidateDeviceObjects();
				m_pObjEffectMesh[i]->DeleteDeviceObjects();
				SAFE_DELETE(m_pObjEffectMesh[i]);
			}
			else// if(m_nObjRenderCount[i] >= 2)
			{
				m_pObjEffectMesh[i]->m_nRenderCount = 2;
			}
		}
	}*/
	map<string, CSkinnedMesh*>::iterator itMesh = m_mapObjNameToMesh.begin();
	while(itMesh != m_mapObjNameToMesh.end())
	{
		if(itMesh->second->m_nRenderCount == 2)
		{
			if (itMesh->second) {
				// 2009. 11. 23 by jskim ¸®ĽŇ˝ş ·Îµů ±¸Á¶ şŻ°ć
				itMesh->second->DeleteLoadingGameData();
				//end 2009. 11. 23 by jskim ¸®ĽŇ˝ş ·Îµů ±¸Á¶ şŻ°ć
				itMesh->second->InvalidateDeviceObjects();
				itMesh->second->DeleteDeviceObjects();
				SAFE_DELETE(itMesh->second);
			}
			itMesh = m_mapObjNameToMesh.erase(itMesh++);
			continue;
		}
		itMesh++;
	}
}
/*
void CEffectRender::RenderSun()
{
	FLOG( "CEffectRender::RenderSun()" );
	CCharacterInfo* pChar;
	// Sun
	g_pD3dDev->SetRenderState( D3DRS_FOGENABLE, FALSE );
	if(g_pD3dApp->m_pScene->m_pSunData)
	{
		pChar = g_pD3dApp->m_pScene->m_pSunData->m_pCharacterInfo;
		if(pChar)
		{
			D3DXMATRIX mat;
			D3DXVECTOR3 vPos = g_pCamera->GetEyePt();
			D3DXVECTOR3 vVel =  g_pD3dApp->m_pScene->m_pSunData->m_vPos - g_pCamera->GetEyePt();
			D3DXVECTOR3 vSide = g_pCamera->GetCross();
			D3DXVECTOR3 vUp;
			D3DXVec3Cross(&vUp,&vVel,&vSide);
			D3DXMatrixLookAtLH(&mat,&vPos,&(vPos + vVel),&vUp);
			COLLISION_RESULT collResult = g_pScene->m_pObjectRender->CheckCollMesh(mat,vPos);
			if(collResult.fDist == 10000)
			{
				RenderCharacterInfo(pChar);
			}
		}
	}
}
*/

void CEffectRender::DevideCharacterEffect(CCharacterInfo* pChar, BOOL bAlpha, int nAlphaValue)
{
	FLOG( "CEffectRender::DevideCharacterEffect(CCharacterInfo* pChar)" );
	int nParticleEffectCount=0,nSpriteEffectCount=0,nObjectEffectCount=0,nTraceEffectCount=0;
	if (pChar)
	{
		//set<BodyCond_t>::iterator itCurrent = pChar->m_vecCurrentBodyCondition.begin();
		for (auto el : pChar->m_vecCurrentBodyCondition)
		{
			map<BodyCond_t, CBodyConditionInfo*>::iterator itBody = pChar->m_mapBodyCondition.find(el);
			if (itBody != pChar->m_mapBodyCondition.end())
			{
				CBodyConditionInfo* pBody = itBody->second;
				//vector<CEffectInfo*>::iterator itEffect = pBody->m_vecEffect.begin();
				for (auto el2 : pBody->m_vecEffect)
				{
					// effect rendering
					CEffectInfo* pEffectInfo = el2;
					pEffectInfo->m_nAlphaValue = nAlphaValue;

					// 2010. 07. 23 by dhkwon, jskim Áöż¬ ˝ĂŔŰ ŔĚĆŃĆ®°ˇ ¸ŐŔú ·Ł´ő µÇ´Â ąö±× ĽöÁ¤
					if (pEffectInfo->m_fCurrentTime > 0)
					{
					//	itEffect++;
						continue;
					}

					switch (pEffectInfo->m_nEffectType)
					{
					case EFFECT_TYPE_OBJECT:
					{
						if (pEffectInfo->m_pEffect)
						{
							CObjectAni* pEffect = (CObjectAni*)pEffectInfo->m_pEffect;
							// 2006-05-17 by ispark
							D3DXVECTOR3 vPos(pEffect->m_pParent->m_pParent->m_mMatrix._41 - pEffect->m_pParent->m_vPos.z,
								pEffect->m_pParent->m_pParent->m_mMatrix._42 + pEffect->m_pParent->m_vPos.y,
								pEffect->m_pParent->m_pParent->m_mMatrix._43 + pEffect->m_pParent->m_vPos.x);
							//							BOOL bResult = g_pFrustum->CheckSphere( pEffect->m_pParent->m_pParent->m_mMatrix._41, 
							//																	pEffect->m_pParent->m_pParent->m_mMatrix._42, 
							//																	pEffect->m_pParent->m_pParent->m_mMatrix._43,  
							//																	pEffect->m_fRadius);
							BOOL bResult = g_pFrustum->CheckSphere(vPos.x,
								vPos.y,
								vPos.z,
								pEffect->m_fRadius);
							// 2006-05-23 by ispark
							if (g_pD3dApp->m_dwGameState != _GAME)
							{
								bResult = TRUE;
							}
							if (!pEffect->m_bAlphaBlending && bResult == TRUE)
							{
								// 2006-11-16 by ispark
								//								if(bWeapon == TRUE)
								//									ObjectAniRender(pEffect, TRUE);
								//								else if(g_pShuttleChild->m_bInvenRender && !g_pD3dApp->m_bCharacter)
								//									ObjectAniRender(pEffect, TRUE);
								//								else
								ObjectAniRender(pEffect, bAlpha, nAlphaValue);
							}
							else if (pEffect->m_bZbufferEnable && bResult == TRUE)
							{
#ifdef _DEBUG_SHIT
								if((UINT)pEffect == (UINT)0xfdfdfdfd)
								{
									DBGOUT("\n\n\nCRITICAL ERROR\n\n\n");
								}
#endif
								m_vecEffectZBackBuffer.emplace_back(EffectBackBuffer((char*)pEffect, EFFECT_TYPE_OBJECT));
							}
							else if(bResult == TRUE)
							{
								m_vecEffectNonZBackBuffer.emplace_back(EffectBackBuffer((char*)pEffect, EFFECT_TYPE_OBJECT));
							}
							nObjectEffectCount++;
						}
					}
					break;
					case EFFECT_TYPE_SPRITE:
					{
						if (pEffectInfo->m_pEffect)
						{
							CSpriteAni* pEffect = (CSpriteAni*)pEffectInfo->m_pEffect;
							// 2006-05-17 by ispark
							D3DXVECTOR3 vPos(pEffect->m_pParent->m_pParent->m_mMatrix._41 - pEffect->m_pParent->m_vPos.z,
								pEffect->m_pParent->m_pParent->m_mMatrix._42 + pEffect->m_pParent->m_vPos.y,
								pEffect->m_pParent->m_pParent->m_mMatrix._43 + pEffect->m_pParent->m_vPos.x);
							//							BOOL bResult = g_pFrustum->CheckSphere( pEffect->m_pParent->m_pParent->m_mMatrix._41, 
							//																	pEffect->m_pParent->m_pParent->m_mMatrix._42, 
							//																	pEffect->m_pParent->m_pParent->m_mMatrix._43,  
							//																	pEffect->m_fRadius);
							BOOL bResult = g_pFrustum->CheckSphere(vPos.x,
								vPos.y,
								vPos.z,
								pEffect->m_fTextureSize);
							// 2006-05-23 by ispark
							if (g_pD3dApp->m_dwGameState != _GAME)
							{
								bResult = TRUE;
							}
							if (pEffect->m_bZbufferEnable && bResult == TRUE)
							{
#ifdef _DEBUG_SHIT
								if((UINT)pEffect == (UINT)0xfdfdfdfd)
								{
									DBGOUT("\n\n\nCRITICAL ERROR\n\n\n");
								}
#endif
								m_vecEffectZBackBuffer.emplace_back(EffectBackBuffer((char*)pEffect, EFFECT_TYPE_SPRITE));
							}
							else if (bResult == TRUE)
							{
								m_vecEffectNonZBackBuffer.emplace_back(EffectBackBuffer((char*)pEffect, EFFECT_TYPE_SPRITE));
							}
							nSpriteEffectCount++;
						}
					}
					break;
					case EFFECT_TYPE_PARTICLE:
					{
						// 2008-02-26 by bhsohn ŔűŔŻ´Ö ąŮµđ ÄÁµđĽÇ ľČłŞżŔ´Â ąö±× ĽöÁ¤
						//						if(pEffectInfo->m_pEffect &&
						//							// 2007-10-16 by dgwoo ŔĚĆĺĆ®°ˇ ŔÚ˝ĹŔÇ °ÍŔĚ¸ç Äł¸ŻĹÍ »óĹÂżˇĽ± ĆÄĆĽĹ¬Ŕ» ąß»ý˝ĂĹ°Áö ľĘ´Â´Ů.
						//							// »ő·Î µéľî°Ł ą«±â ĆÄĆĽĹ¬ ŔĚĆĺĆ®°ˇ Äł¸ŻĹÍ ¸đµĺżˇĽ­µµ ąß»ýÇĎż© Ăł¸®.
						//							// ĆÄĆĽĹ¬°ú żŔşęÁ§Ć®°ŁŔÇ °ü°č¸¦ şŻ°ćÇŇ ÇĘżäĽşŔĚ ŔÖŔ˝.
						//							(pChar->m_bShuttleChildEffect ))
						//							// 2007-11-08 by bhsohn ŔÎşĄ ŔĚĆĺĆ® °ü·Ă Ăł¸®
						//							//g_pD3dApp->m_bCharacter == FALSE))
						// ŔűŔŻ´Ö ąŮµđ ÄÁµđĽÇ ľČłŞżŔ´Â ąö±× ĽöÁ¤
						if (pEffectInfo->m_pEffect)
						{
							CParticleSystem* pEffect = (CParticleSystem*)pEffectInfo->m_pEffect;
							// 2006-05-17 by ispark
							D3DXVECTOR3 vPos(pEffect->m_pParent->m_pParent->m_mMatrix._41 - pEffect->m_pParent->m_vPos.z,
								pEffect->m_pParent->m_pParent->m_mMatrix._42 + pEffect->m_pParent->m_vPos.y,
								pEffect->m_pParent->m_pParent->m_mMatrix._43 + pEffect->m_pParent->m_vPos.x);
							//							BOOL bResult = g_pFrustum->CheckSphere( pEffect->m_pParent->m_pParent->m_mMatrix._41, 
							//																	pEffect->m_pParent->m_pParent->m_mMatrix._42, 
							//																	pEffect->m_pParent->m_pParent->m_mMatrix._43,  
							//																	pEffect->m_fRadius);
							BOOL bResult = g_pFrustum->CheckSphere(vPos.x,
								vPos.y,
								vPos.z,
								pEffect->m_fRadius);
							//							DBGOUT("%s File : Len(%f:%f), Render(%d)\n", pEffect->m_strName, D3DXVec3Length(&D3DXVECTOR3(g_pShuttleChild->m_vPos - vPos)), pEffect->m_fRadius, bResult);
							// 2006-05-23 by ispark
							if (g_pD3dApp->m_dwGameState != _GAME)
							{
								bResult = TRUE;
							}
							if (bResult == TRUE && pEffect->m_bZbufferEnable)
							{
#ifdef _DEBUG_SHIT
								vector<Effect*>::iterator itP = pEffect->m_vecParticle.begin();
								while( itP != pEffect->m_vecParticle.end() )
								{
									if((UINT)(*itP) == (UINT)0xfdfdfdfd)
									{
										DBGOUT("\n\n\nCRITICAL ERROR\n\n\n");
									}
									itP++;
								}
#endif
								// 2007-11-08 by bhsohn ŔÎşĄ ŔĚĆĺĆ® °ü·Ă Ăł¸®
								//								m_vecEffectZBackBuffer.insert(m_vecEffectZBackBuffer.end(),
								//										pEffect->m_vecParticle.begin(), pEffect->m_vecParticle.end());
								// ÄÉ¸ŻĹÍ ¸đµĺ˝Ăżˇ´Â ĆÄĆĽĹ¬ ŔĚĆĺĆ®¸¦ ľČ±×¸°´Ů.

								// 2007-12-17 by bhsohn ¸¶Ŕ» ĆÄĆĽĹ¬ ą®Á¦ Ăł¸®
								//								if(g_pD3dApp->m_bCharacter == FALSE)
								//								{
								//									m_vecEffectZBackBuffer.insert(m_vecEffectZBackBuffer.end(),
								//										pEffect->m_vecParticle.begin(), pEffect->m_vecParticle.end());
								//								}								
								// end 2007-12-17 by bhsohn ¸¶Ŕ» ĆÄĆĽĹ¬ ą®Á¦ Ăł¸®

								// 2007-11-08 by bhsohn ŔÎşĄ ŔĚĆĺĆ® °ü·Ă Ăł¸®								
								// ŔÎşĄ ŔĚĆĺĆ®´Â ą«Á¶°Ç ±×·ÁľßÇŃ´Ů.
							//	vector<Effect*>::iterator itEffectParticle = pEffect->m_vecParticle.begin();
								for (auto el3: pEffect->m_vecParticle)
								{
									Effect* pEffectParticle = el3;

									/*D3DXVECTOR3 vPos2(pEffect->m_pParent->m_pParent->m_mMatrix._41 - ((CParticle*)pEffectParticle)->m_vPos.z,
										pEffect->m_pParent->m_pParent->m_mMatrix._42 + ((CParticle*)pEffectParticle)->m_vPos.y,
										pEffect->m_pParent->m_pParent->m_mMatrix._43 + ((CParticle*)pEffectParticle)->m_vPos.x);

									BOOL bResult2 = g_pFrustum->CheckSphere(vPos2.x,
										vPos2.y,
										vPos2.z,
										((CParticle*)pEffectParticle)->m_fSize);*/

									BOOL bResult2 = g_pFrustum->CheckSphere(((CParticle*)pEffectParticle)->m_vPos.x,
										((CParticle*)pEffectParticle)->m_vPos.y,
										((CParticle*)pEffectParticle)->m_vPos.z,
										((CParticle*)pEffectParticle)->m_fSize);

									int  nInvenIdx = ((CParticle*)pEffectParticle)->m_pParent->m_pParent->m_nInvenWeaponIndex;
									BOOL pushback = FALSE;
									// 2007-12-17 by bhsohn ¸¶Ŕ» ĆÄĆĽĹ¬ ą®Á¦ Ăł¸®
									if (bResult2 == TRUE) {
										if (g_pD3dApp->m_bCharacter == TRUE)
										{
											if (nInvenIdx == 0)
											{
												// Äł¸ŻĹÍ ¸đµĺ°í ŔÎşĄ ĆÄĆĽĹ¬Ŕ» »Ń¸®Áö ľĘ´Â´Ů.
												pushback = TRUE;
											}
										}
										else
										{
											// Äł¸ŻĹÍ ¸đµĺ°í ŔÎşĄ ĆÄĆĽĹ¬Ŕ» »Ń¸®Áö ľĘ´Â´Ů.
											pushback = TRUE;
										}
									}
									// end 2007-12-17 by bhsohn ¸¶Ŕ» ĆÄĆĽĹ¬ ą®Á¦ Ăł¸®

									if(pushback == TRUE)
									{
										m_vecEffectZBackBuffer.emplace_back(EffectBackBuffer((char*)pEffectParticle, EFFECT_TYPE_PARTICLE, pEffect->m_nParticleType));	
									}

									// 2008-08-22 by bhsohn EP3 ŔÎşĄĹä¸® Ăł¸®
									//if(nInvenIdx > 0 &&  g_pShuttleChild->m_bInvenRender == TRUE)
									if (nInvenIdx > 0 && g_pGameMain->IsEquipInvenShow() == TRUE)
									{
										AddInvenPaticleName(nInvenIdx - 1, ((CParticle*)pEffectParticle)->m_pParent->m_strName);
									}

									//itEffectParticle++;
								}
							}
							else if(bResult == TRUE)
							{
								m_vecEffectNonZBackBuffer.emplace_back(EffectBackBuffer((char*)pEffect, EFFECT_TYPE_PARTICLE, pEffect->m_nParticleType));
							}
							
							nParticleEffectCount += pEffect->m_vecParticle.size();
						}
					}
					break;
					case EFFECT_TYPE_TRACE:
					{
						if (pEffectInfo->m_pEffect)
						{
							CTraceAni* pEffect = (CTraceAni*)pEffectInfo->m_pEffect;
							// 2006-05-17 by ispark
							D3DXVECTOR3 vPos(pEffect->m_pParent->m_pParent->m_mMatrix._41 - pEffect->m_pParent->m_vPos.z,
								pEffect->m_pParent->m_pParent->m_mMatrix._42 + pEffect->m_pParent->m_vPos.y,
								pEffect->m_pParent->m_pParent->m_mMatrix._43 + pEffect->m_pParent->m_vPos.x);
							//							BOOL bResult = g_pFrustum->CheckSphere( pEffect->m_pParent->m_pParent->m_mMatrix._41, 
							//																	pEffect->m_pParent->m_pParent->m_mMatrix._42, 
							//																	pEffect->m_pParent->m_pParent->m_mMatrix._43,  
							//																	pEffect->m_fRadius);
							BOOL bResult = g_pFrustum->CheckSphere(vPos.x,
								vPos.y,
								vPos.z,
								pEffect->m_fRadius);
							// 2006-05-23 by ispark
							if (g_pD3dApp->m_dwGameState != _GAME)
							{
								bResult = TRUE;
							}
							if (pEffect->m_bZbufferEnable && bResult == TRUE)
							{
								m_vecEffectZBackBuffer.emplace_back(EffectBackBuffer((char*)pEffect, EFFECT_TYPE_TRACE));
							}
							else if (bResult == TRUE)
							{
								m_vecEffectNonZBackBuffer.emplace_back(EffectBackBuffer((char*)pEffect, EFFECT_TYPE_TRACE));
							}
							nTraceEffectCount++;
						}
					}
					break;
					}
				//	itEffect++;
				}//if(!Effect.end())
			}//if(!bodyCondition.end())
			//itCurrent++;
		}//end()
		m_nParticleEffectCount += nParticleEffectCount;
		m_nSpriteEffectCount += nSpriteEffectCount;
		m_nObjectEffectCount += nObjectEffectCount;
		m_nTraceEffectCount += nTraceEffectCount;
	}
}

void CEffectRender::DevideZBufferEnableEffect()
{
	FLOG( "CEffectRender::DevideZBufferEnableEffect()" );
	CCharacterInfo* pChar;

	//vectorCObjectChildPtr::iterator itObj(g_pScene->m_vectorCulledObjectPtrList.begin());
	for(auto &el : g_pScene->m_vectorCulledObjectPtrList)
	{
		CObjectChild * pObj = el;
		pChar = pObj->m_pCharacterInfo;
		
		float fDistanceToCamera = D3DXVec3Length(&(pObj->m_vPos - g_pD3dApp->m_pCamera->GetEyePt()));		
//		fDistance = (g_pScene->m_fOrgFogEndValue/(10-g_pSOption->sTerrainEffectRender));
		
		// 2007-07-11 by dgwoo Ć©Ĺä¸®ľó ¸ĘżˇĽ­ Ă©ĹÍ(1_1 ~ 1_5)ŔÇ ¸µŔ» Ă©ĹÍ¸¶´Ů ´Ů¸Ł°Ô ±×·ÁÁŕľßÇŃ´Ů.
		if(g_pTutorial->IsTutorialMode() == TRUE &&				// Ć©Ĺä¸®ľó ¸đµĺ ŔĚ¸éĽ­.
//			g_pTutorial->GetLesson() == L1 &&					// ·ą˝Ľ 1ŔĚ¸éĽ­.
			pObj->m_nCode == TUTORIAL_GATE &&					// °ÔŔĚĆ® żŔşęÁ§Ć®¸éĽ­.
			!g_pTutorial->IsEnableTutorialGate(pObj->m_vPos))	// ÇöŔç »çżëÇŘľßÇŇ °ÔŔĚĆ®¶ó¸é.
		{// 
			int i = 0;
		}
		else
		{
			// Ć©Ĺä¸®ľó ¸đµĺ°ˇ ľĆ´Ň˝Ă.
			// 2005-05-10 by jschoi - ŔĚĆĺĆ® żÉĽÇ »čÁ¦
			float fDistance = g_pScene->m_fFogEndValue + pObj->m_pObjMesh->m_fRadius;

			if(fDistanceToCamera < fDistance)
			{
					
				if(pChar)
				{
					DevideCharacterEffect(pChar);
				}
			}
		}
		
		//itObj++;
	}

	if(g_pGround && g_pGround->m_pObjectEvent)
	{
		CObjectChild * pObjEvent = (CObjectChild *)g_pGround->m_pObjectEvent->m_pChild;
		while(pObjEvent)
		{
			pChar = pObjEvent->m_pCharacterInfo;
			if(pChar)
			{
				// 2007-07-11 by dgwoo Ć©Ĺä¸®ľó¸ĘżˇĽ­ ±×¸®Áö ¸»ľĆľßÇŇ ¸µ(ŔĚĆĺĆ®°ˇ ŔÖ´Ů)
				if(g_pTutorial->IsTutorialMode() == TRUE &&				// Ć©Ĺä¸®ľó ¸đµĺ ŔĚ¸éĽ­.
//					g_pTutorial->GetLesson() == L1 &&					// ·ą˝Ľ 1ŔĚ¸éĽ­.
					pObjEvent->m_nCode == TUTORIAL_GATE &&					// °ÔŔĚĆ® żŔşęÁ§Ć®¸éĽ­.
					!g_pTutorial->IsEnableTutorialGate(pObjEvent->m_vPos))	// ÇöŔç »çżëÇŘľßÇŇ °ÔŔĚĆ®¶ó¸é.
				{
					int i = 0;
				}
				else
				{
					DevideCharacterEffect(pChar);
				}
			}
			pObjEvent = (CObjectChild *)pObjEvent->m_pNext;
		}
	}
	// ShuttleChild żÍ CharacterChild
	// 2005-07-26 by ispark
	if(g_pD3dApp->m_bCharacter == FALSE)
	{

//		if(g_pShuttleChild && COMPARE_RACE(g_pShuttleChild->m_myShuttleInfo.Race,RACE_GAMEMASTER)==FALSE)
		if(g_pShuttleChild)
		{
			pChar = g_pD3dApp->m_pShuttleChild->m_pCharacterInfo;
			if(pChar &&
				g_pD3dApp->m_pCamera->GetCamType() == CAMERA_TYPE_NORMAL)
			{
				// 2006-11-16 by ispark, ľËĆÄ ĂĽĹ©
				if(g_pShuttleChild->m_nAlphaValue < SKILL_OBJECT_ALPHA_NONE)
				{
					ALPHA_CHARACTERINFO stAlphaInfo;
					stAlphaInfo.pCharInfo = pChar;
					stAlphaInfo.nAlphaValue = g_pShuttleChild->m_nAlphaValue;
					g_pScene->m_vecAlphaEffectRender.push_back(stAlphaInfo);
				}
				else
				{
					DevideCharacterEffect(pChar);
				}
			}

			// 2004-10-12 by jschoi
		//	vector<SkillEffectInfo>::iterator itEffect = g_pShuttleChild->m_pSkillEffect->m_vecSkillEffect.begin();
			for(auto &el2 : g_pShuttleChild->m_pSkillEffect->m_vecSkillEffect)
				if(el2.pCharacterInfo)				
					DevideCharacterEffect(el2.pCharacterInfo);

			
			// 2006-12-04 by ispark, ĂĽÇÁ »çĂâ
			//vector<CItemData*>::iterator itItem = g_pShuttleChild->m_pChaffData.begin();
			for(auto el3 : g_pShuttleChild->m_pChaffData)
				if(el3->m_pCharacterInfo)
					DevideCharacterEffect(el3->m_pCharacterInfo);

			// 2007-02-12 by dgwoo ˝şÄµ żŔşęÁ§Ć®
			//itItem = g_pScene->m_vecScanData.begin();
			for(auto el4 : g_pScene->m_vecScanData)
				if(el4->m_pCharacterInfo)				
					DevideCharacterEffect(el4->m_pCharacterInfo);
			
		}
	}
	else
	{
//		pChar = g_pD3dApp->m_pShuttleChild->m_pCharacterInfo;
//		if(pChar)
//		{
//			DevideCharacterEffect(pChar);
//		}
		// Äł¸ŻĹÍ ŔĚĆĺĆ®
		// Picking ŔĚĆĺĆ®
		if(g_pCharacterChild &&
			g_pCharacterChild->GetbPickMove())
		{
			pChar = g_pCharacterChild->m_pPickingInfo;
			if(pChar)
			{
				DevideCharacterEffect(pChar);
			}			
		}
	}

	//CVecEnemyIterator itEnemy = g_pD3dApp->m_pScene->m_vecEnemyRenderList.begin();
	for(auto el : g_pD3dApp->m_pScene->m_vecEnemyRenderList)
	{
		// 2005-07-26 by ispark
		// ŔűŔĚ Äł¸ŻĹÍŔÎÁö ĆÇ´Ü
// 2011-07-20 by jhahn	ŔÎÇÇ3Â÷ ˝Ăł×¸¶ÇĂ·ąŔĚÁß Ĺ¸Äł¸ŻĹÍ ľČş¸ŔĚ±â
		if(el->m_bRender == FALSE )
			continue;
		
//end 2011-07-20 by jhahn	ŔÎÇÇ3Â÷ ˝Ăł×¸¶ÇĂ·ąŔĚÁß Ĺ¸Äł¸ŻĹÍ ľČş¸ŔĚ±â
		if(el->m_bEnemyCharacter == FALSE)
		{
			// Ŕű ±âľî ŔĚĆĺĆ®
			pChar = el->m_pCharacterInfo;
			// 2008-07-23 by dgwoo GmľĆ¸ÓŔÇ °ćżě ŔĚĆĺĆ®°ˇ ľČłŞżŔ´Â ąö±× ĽöÁ¤.
			//if(pChar && (*itEnemy)->m_dwPartType != _ADMIN)
			if(pChar)
			{
				// 2006-11-16 by ispark, ľËĆÄ ĂĽĹ©
				if(el->m_nAlphaValue < SKILL_OBJECT_ALPHA_NONE)
				{
					ALPHA_CHARACTERINFO stAlphaInfo;
					stAlphaInfo.pCharInfo = pChar;
					stAlphaInfo.nAlphaValue = el->m_nAlphaValue;
					g_pScene->m_vecAlphaEffectRender.push_back(stAlphaInfo);
				}
				else
				{
					DevideCharacterEffect(pChar);
				}
			}

			// 2004-10-12 by jschoi
			//vector<SkillEffectInfo>::iterator itEffect = el->m_pSkillEffect->m_vecSkillEffect.begin();
			for(auto &el2 : el->m_pSkillEffect->m_vecSkillEffect)
			{
				if(el2.pCharacterInfo)
				{
					// 2006-11-16 by ispark, ľËĆÄ ĂĽĹ©
					if(el->m_nAlphaValue < SKILL_OBJECT_ALPHA_NONE)
					{
						ALPHA_CHARACTERINFO stAlphaInfo;
						stAlphaInfo.pCharInfo = el2.pCharacterInfo;
						stAlphaInfo.nAlphaValue = el->m_nAlphaValue;
						g_pScene->m_vecAlphaEffectRender.push_back(stAlphaInfo);
					}
					else
					{
						DevideCharacterEffect(el2.pCharacterInfo);
					}
				}
				//itEffect++;
			}

			// 2006-12-04 by ispark, ĂĽÇÁ »çĂâ
			//vector<CItemData*>::iterator itItem = (*itEnemy)->m_pChaffData.begin();
			for(auto itItem : el->m_pChaffData)
			{
				if(itItem->m_pCharacterInfo)
				{
					// 2006-11-16 by ispark, ľËĆÄ ĂĽĹ©
					if(el->m_nAlphaValue < SKILL_OBJECT_ALPHA_NONE)
					{
						ALPHA_CHARACTERINFO stAlphaInfo;
						stAlphaInfo.pCharInfo = pChar;
						stAlphaInfo.nAlphaValue = el->m_nAlphaValue;
						g_pScene->m_vecAlphaEffectRender.push_back(stAlphaInfo);
					}
					else
					{
						DevideCharacterEffect(itItem->m_pCharacterInfo);
					}
				}
				//itItem++;
			}
		}
		else
		{
			// Ŕű Äł¸ŻĹÍ ŔĚĆĺĆ®
		}

		//itEnemy++;
	}
//	if(g_pD3dApp->m_bDegree == 2) // °í»çľç
//	{
//		CMapMonsterIterator itMonster = g_pD3dApp->m_pScene->m_mapMonsterList.begin();
//		while(itMonster != g_pD3dApp->m_pScene->m_mapMonsterList.end())
//		{
//			pChar = (itMonster->second)->m_pCharacterInfo;
//			if(pChar)
//			{
//				DevideCharacterEffect(pChar);
//			}
//			itMonster++;
//		}
//	}
//	else
//	{
		CVecMonsterIterator itMonster = g_pD3dApp->m_pScene->m_vecMonsterRenderList.begin();
		while(itMonster != g_pD3dApp->m_pScene->m_vecMonsterRenderList.end())
		{
			pChar = (*itMonster)->m_pCharacterInfo;
			if(pChar)
			{
				DevideCharacterEffect(pChar);
			}


			// 2004-10-12 by jschoi
			vector<SkillEffectInfo>::iterator itEffect = (*itMonster)->m_pSkillEffect->m_vecSkillEffect.begin();
			while(itEffect != (*itMonster)->m_pSkillEffect->m_vecSkillEffect.end())
			{
				if(itEffect->pCharacterInfo)
				{
					DevideCharacterEffect(itEffect->pCharacterInfo);
				}
				itEffect++;
			}

			itMonster++;
		}
//	}
	// Weapon, ĂŃľË ŔĚĆĺĆ®
	if(g_pD3dApp->m_pScene->m_pWeaponData)
	{
		CWeapon * pWeapon = (CWeapon*)g_pD3dApp->m_pScene->m_pWeaponData->m_pChild;
		while(pWeapon)
		{
			pChar = pWeapon->m_pCharacterInfo;
			if(pChar)
			{
				// 2006-07-18 by ispark, ŔÜÁ¸ ¸Ţ¸đ¸ŻŔ» ĂŁ±â Ŕ§ÇŃ DBGOUT
//				if(pWeapon->m_pItemData && (pChar->m_nCurrentBodyCondition == BODYCON_HIT_MASK || pChar->m_nCurrentBodyCondition == BODYCON_FIRE_MASK || pChar->m_nCurrentBodyCondition == BODYCON_BULLET_MASK))
//				{
//					DBGOUT("Effect Check MemoryRick(%d) 0x%x\n", pWeapon->m_pItemData->ItemNum, pChar->m_nCurrentBodyCondition);
//				}
				if( 
					// 2007-02-01 by dgwoo Ä§ą¬˝şĹłŔ» ¸ÂľŇŔ»¶§ ŔÚ˝ĹŔ» Ĺ¸°ŮÇĎ°íŔÖ´Â 2Çüą«±â´Â ş¸ŔĚÁö ľĘ´Â´Ů.
					g_pShuttleChild->GetSkillMissileWarning() &&				//Ä§ą¬˝şĹłŔĚ ąßµżÁ×ŔĚ¸é.
					pWeapon->m_pTarget == g_pShuttleChild &&					//Ĺ¸°ŮŔĚ ŔÚ˝ĹŔĎ¶§
					pWeapon->m_pItemData != NULL &&								
					IS_SECONDARY_WEAPON(pWeapon->m_pItemData->Kind)				//2Çüą«±âŔĎ¶§¸¸. 
// 2007-02-01 by dgwoo ľĆ·ˇ Á¶°ÇŔĚ µéľî°Ą ŔĚŔŻ¸¦ ĂŁÁö ¸řÇĎ°ÚŔ˝ ¤Ń¤Ń;
//					IS_DT(g_pShuttleChild->m_myShuttleInfo.UnitKind) == TRUE &&
//					g_pShuttleChild->m_bAttackMode == _SIEGE &&
//					pWeapon->m_pTarget == g_pShuttleChild &&
//					COMPARE_BODYCON_BIT(pWeapon->m_pCharacterInfo->m_nCurrentBodyCondition, BODYCON_HIT_MASK)
					)
				{
					
					// renderingÇĎÁö ľĘ´Â´Ů.
				}
				else
				{
					DevideCharacterEffect(pChar);
				}
				
			}
			pWeapon = (CWeapon *)pWeapon->m_pNext;
		}
	}
	// Item
	if(g_pD3dApp->m_pScene->m_pItemData)
	{
		CItemData * pItem = (CItemData*)g_pD3dApp->m_pScene->m_pItemData->m_pChild;
		while(pItem)
		{
			pChar = pItem->m_pCharacterInfo;
			if(pChar && pItem->m_bIsRender)
			{
				DevideCharacterEffect(pChar);
			}
			//pItem->RenderItemName();
			pItem = (CItemData *)pItem->m_pNext;
		}
	}
	if( //g_pScene->m_byMapType != MAP_TYPE_CITY && 
		(g_pD3dApp->m_dwGameState == _GAME || g_pD3dApp->m_dwGameState == _SHOP) &&
		g_pScene->m_byWeatherType != WEATHER_FOGGY &&			// ľČ°łł¤
		g_pScene->m_byWeatherType != WEATHER_RAINY &&			// şń
		g_pScene->m_byWeatherType != WEATHER_SNOWY &&			// ´«
//		g_pScene->m_byWeatherType != WEATHER_CLOUDY &&			// ±¸¸§ł¤
		g_pScene->m_pSunData &&
		g_pScene->m_pSunData->m_pCharacterInfo &&
		IsSunRenderEnable(g_pShuttleChild->m_myShuttleInfo.MapChannelIndex.MapIndex))	
	{
		pChar = g_pD3dApp->m_pScene->m_pSunData->m_pCharacterInfo;
		if(pChar)
		{
			DevideCharacterEffect(pChar);
		}
	}

	if(g_pD3dApp->m_pEffectList)
	{
		CAppEffectData * pEffect = (CAppEffectData *)g_pD3dApp->m_pEffectList->m_pChild;
		while(pEffect)
		{
			// 2009. 07. 07 by ckPark ·Îşż±âľî żäĂ»»çÇ×(·Ń¸µ, Ľ±ĹĂČ­¸é, ą«±â, A±âľîĆ÷´ë)
			if( !pEffect->m_bRender )
			{
				pEffect = (CAppEffectData*)pEffect->m_pNext;
				continue;
			}
			// end 2009. 07. 07 by ckPark ·Îşż±âľî żäĂ»»çÇ×(·Ń¸µ, Ľ±ĹĂČ­¸é, ą«±â, A±âľîĆ÷´ë)
			
			pChar = pEffect->m_pCharacterInfo;
			// 2006-11-16 by ispark, ľËĆÄ°Ş µű·Î ŔúŔĺ
			
			// 2011. 01. 13 by jskim, ŔÎşńÁöşí »óĹÂżˇĽ­ ĆęŔĚ ş¸ŔĚ´Â ąö±× ĽöÁ¤
			BOOL bEffectRender = FALSE;
			//if( pEffect->m_nType/1000000 == 7 || pEffect->m_nType/10000000 == 1 || pEffect->m_nType/100000   == 1 )
			if (pEffect->m_nType / 1000000 == 7 || pEffect->m_nType / 10000000 == 1 || pEffect->m_nType / 100000 == 1 || pEffect->m_nType / 1000000 == 4)
			{
				bEffectRender = TRUE;
			}
			// end 2011. 01. 13 by jskim, ŔÎşńÁöşí »óĹÂżˇĽ­ ĆęŔĚ ş¸ŔĚ´Â ąö±× ĽöÁ¤

			if(pChar)
			{
				if( pEffect->m_pParent && 
					(pEffect->m_pParent->m_dwPartType == _ENEMY  || pEffect->m_pParent->m_dwPartType == _ADMIN )&&
					// 2011. 01. 13 by jskim, ŔÎşńÁöşí »óĹÂżˇĽ­ ĆęŔĚ ş¸ŔĚ´Â ąö±× ĽöÁ¤
					//(pEffect->m_nType/1000000 == 7 || pEffect->m_nType/10000000 == 1 || pEffect->m_nType / 100000   == 1))
					bEffectRender )
					// end 2011. 01. 13 by jskim, ŔÎşńÁöşí »óĹÂżˇĽ­ ĆęŔĚ ş¸ŔĚ´Â ąö±× ĽöÁ¤
				{// Ĺ×˝şĆ®(ŔĺÂř ľĆŔĚĹŰŔĎ°ćżě)
					// 2005-07-29 by ispark
					// Ŕű Äł¸ŻĹÍŔĎ ¶§ ÂďÁö¸¶
					if(((CEnemyData *)pEffect->m_pParent)->m_bEnemyCharacter == FALSE)
					{
						// 2006-11-16 by ispark, ľËĆÄ °Ë»ç
						if(CheckAlphaRender(pEffect, pEffect->m_pParent->m_dwPartType))
						{
							ALPHA_CHARACTERINFO stAlphaInfo;
							stAlphaInfo.pCharInfo = pChar;
							stAlphaInfo.nAlphaValue = ((CEnemyData *)pEffect->m_pParent)->m_nAlphaValue;
							g_pScene->m_vecAlphaEffectRender.push_back(stAlphaInfo);
						}
						else if(((CEnemyData *)pEffect->m_pParent)->m_bIsRender &&
						(((CAtumData*)pEffect->m_pParent)->m_bDegree != 0 ||	// LOW°ˇ ľĆ´Ď°ĹłŞ
						// 2011. 01. 13 by jskim, ŔÎşńÁöşí »óĹÂżˇĽ­ ĆęŔĚ ş¸ŔĚ´Â ąö±× ĽöÁ¤
						//pEffect->m_nType/100000 == 78) )						// Skill EffectŔÎ °ćżě
						pEffect->m_nType/100000 == 78) ||						// Skill EffectŔÎ °ćżě
						bEffectRender )
						// end 2011. 01. 13 by jskim, ŔÎşńÁöşí »óĹÂżˇĽ­ ĆęŔĚ ş¸ŔĚ´Â ąö±× ĽöÁ¤
						{
							if ((g_pD3dApp->m_bTrailMod == TRUE) && (((CEnemyData*)pEffect->m_pParent)->m_pWingIn))
							{
								((CEnemyData*)pEffect->m_pParent)->m_pWingIn->m_bRender = FALSE;
							}
							else if (((CEnemyData*)pEffect->m_pParent)->m_pWingIn)
							{
								((CEnemyData*)pEffect->m_pParent)->m_pWingIn->m_bRender = TRUE;
							}

							if (g_pSOption->sTerrainEffectRender > 1)
								DevideCharacterEffect(pChar);
						}
					}
					else
					{
						// 2006-06-29 by ispark, ľÇĽĽ»ç¸® °ćżě´Â Äł¸ŻĹÍ »óĹÂżˇĽ­µµ ·»´ő¸µÇŃ´Ů.
						if((((CEnemyData *)pEffect->m_pParent)->m_pContainer &&
							((CEnemyData *)pEffect->m_pParent)->m_pContainer->m_pCharacterInfo == pChar) ||
							(((CEnemyData *)pEffect->m_pParent)->m_pAccessories &&
							((CEnemyData *)pEffect->m_pParent)->m_pAccessories->m_pCharacterInfo == pChar) ||
							(((CEnemyData *)pEffect->m_pParent)->m_pWingIn &&
							((CEnemyData *)pEffect->m_pParent)->m_pWingIn->m_pCharacterInfo == pChar) ||
							// 2011. 01. 13 by jskim, ŔÎşńÁöşí »óĹÂżˇĽ­ ĆęŔĚ ş¸ŔĚ´Â ąö±× ĽöÁ¤
							(((CEnemyData *)pEffect->m_pParent)->m_pPartner &&
							((CEnemyData *)pEffect->m_pParent)->m_pPartner->m_pCharacterInfo == pChar) ||
							(((CEnemyData *)pEffect->m_pParent)->m_pPartner1 &&
							((CEnemyData *)pEffect->m_pParent)->m_pPartner1->m_pCharacterInfo == pChar))
							// end 2011. 01. 13 by jskim, ŔÎşńÁöşí »óĹÂżˇĽ­ ĆęŔĚ ş¸ŔĚ´Â ąö±× ĽöÁ¤							
						{
							if(((CEnemyData *)pEffect->m_pParent)->m_bIsRender)
							{
								DevideCharacterEffect(pChar);
							}
						}
					}
				}
				else
				{
					// 2006-11-16 by ispark, ľËĆÄ °Ë»ç
					if(pEffect->m_pParent && 
						CheckAlphaRender(pEffect, _SHUTTLE))
					{
						ALPHA_CHARACTERINFO stAlphaInfo;
						stAlphaInfo.pCharInfo = pChar;
						stAlphaInfo.nAlphaValue = g_pShuttleChild->m_nAlphaValue;
						g_pScene->m_vecAlphaEffectRender.push_back(stAlphaInfo);
					}
					else if(g_pD3dApp->m_pCamera->GetCamType() == CAMERA_TYPE_FPS &&
						g_pShuttleChild == pEffect->m_pParent)
					{
						if( (g_pShuttleChild->m_pWeapon1_1_1 && 
							pChar == g_pShuttleChild->m_pWeapon1_1_1->m_pCharacterInfo) || 
							(g_pShuttleChild->m_pWeapon1_1_2 && 
							pChar == g_pShuttleChild->m_pWeapon1_1_2->m_pCharacterInfo) || 
							(g_pShuttleChild->m_pWeapon2_1_1 && 
							pChar == g_pShuttleChild->m_pWeapon2_1_1->m_pCharacterInfo) || 
							(g_pShuttleChild->m_pWeapon2_1_2 && 
							pChar == g_pShuttleChild->m_pWeapon2_1_2->m_pCharacterInfo) || 
							(g_pShuttleChild->m_pWeapon1_2 &&
							pChar == g_pShuttleChild->m_pWeapon1_2->m_pCharacterInfo))
						{
							DevideCharacterEffect(pChar);
						}
					}
					else
					{
						DevideCharacterEffect(pChar);
					}
				}
			}
			pEffect = (CAppEffectData*)pEffect->m_pNext;
		}
	}
}

void CEffectRender::Render()
{
	FLOG( "CEffectRender::Render()" );
	m_nParticleEffectCount = 0;
	m_nSpriteEffectCount = 0;
	m_nObjectEffectCount = 0;
	m_nTraceEffectCount = 0;
	m_nParticleEffectRender = 0;
	m_nObjectParticleEffectRender = 0;
	m_nSpriteEffectRender = 0;
	m_nObjectEffectRender = 0;
	m_nTraceEffectRender = 0;
	
	g_pScene->m_vecAlphaEffectRender.clear();

	m_vecEffectZBackBuffer.clear();
	m_vecEffectNonZBackBuffer.clear();

	DevideZBufferEnableEffect();

	// effect sorting
	if (g_pD3dApp->m_dwGameState != _GAME &&
		g_pD3dApp->m_dwGameState != _SELECT &&
		g_pD3dApp->m_dwGameState != _CREATE &&
		g_pD3dApp->m_dwGameState != _LOGO &&
		g_pD3dApp->m_dwGameState != _SHOP &&
		g_pD3dApp->m_dwGameState != _CITY)
		//		g_pD3dApp->m_dwGameState != _WAITING)
	{
#ifdef _DEBUG_SHIT
		DBGOUT_EFFECT("ZBufferEnable Effect Disable state , effect number : %d\n", m_vecEffectZBackBuffer.size());
#endif
		m_vecEffectZBackBuffer.clear();
		return;
	}

	if (!m_vecEffectZBackBuffer.empty())
	{
		RenderEffectVector(m_vecEffectZBackBuffer, TRUE);
		m_vecEffectZBackBuffer.clear();
	}

	//m_bZBufferTemp = TRUE;

	if (!m_vecEffectNonZBackBuffer.empty())
	{
		RenderEffectVector(m_vecEffectNonZBackBuffer, FALSE);
		m_vecEffectNonZBackBuffer.clear();
	}

}

void CEffectRender::SpriteAniRender(CSpriteAni* pEffect)
{
	//return g_pDrawingProc->SpriteAniRenderDP( pEffect,this);

	FLOG( "CEffectRender::SpriteAniRender(CSpriteAni* pEffect)" );
	DWORD dwSrc,dwDest,dwColorOp;
	g_pD3dDev->GetRenderState(D3DRS_SRCBLEND,&dwSrc);
	g_pD3dDev->GetRenderState(D3DRS_DESTBLEND,&dwDest);
	g_pD3dDev->GetTextureStageState(0,D3DTSS_COLOROP,&dwColorOp);

	g_pD3dDev->SetRenderState( D3DRS_LIGHTING, TRUE );
//	g_pD3dDev->SetRenderState( D3DRS_ZENABLE, TRUE);
	g_pD3dDev->SetRenderState( D3DRS_ZENABLE, pEffect->m_bZbufferEnable);

	if( pEffect->m_bZWriteEnable == FALSE)
	{
		g_pD3dDev->SetRenderState( D3DRS_ZWRITEENABLE, pEffect->m_bZWriteEnable );
	}
	g_pD3dDev->SetRenderState( D3DRS_ALPHABLENDENABLE,  pEffect->m_bAlphaBlending );
	if(pEffect->m_bAlphaBlending)
	{
		g_pD3dDev->SetRenderState(D3DRS_SRCBLEND,pEffect->m_nSrcBlend);
		g_pD3dDev->SetRenderState(D3DRS_DESTBLEND,pEffect->m_nDestBlend);
	}
	g_pD3dDev->SetTextureStageState(0,D3DTSS_COLOROP,pEffect->m_nTextureRenderState);
//    g_pD3dDev->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR  );
//    g_pD3dDev->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR  );
	g_pD3dDev->SetFVF( D3DFVF_SPRITE_VERTEX );

	// set light
	D3DXVECTOR3 vAxis = g_pD3dApp->m_pCamera->GetViewDir();
	m_light2 = g_pD3dApp->m_pScene->m_light2;
	m_light2.Direction  = vAxis;
 	g_pD3dDev->SetLight( 2, &m_light2 );
	g_pD3dDev->LightEnable( 2, TRUE );

	D3DXMATRIX matScale;
//	D3DXVECTOR3 pos = pEffect->m_pParent->m_pParent->m_pParent->m_vPos + pEffect->m_pParent->m_vPos;
	D3DXVECTOR3 pos = pEffect->m_pParent->m_vPos;
	D3DXVec3TransformCoord( &pos, &pos, &pEffect->m_pParent->m_pParent->m_mMatrix);
	 
	D3DXMatrixScaling(&matScale, pEffect->m_fTextureSize,pEffect->m_fTextureSize,pEffect->m_fTextureSize);
	matScale *= g_pD3dApp->m_pCamera->GetBillboardMatrix();
	if((pEffect->m_pParent && pEffect->m_pParent->m_fBillboardRotateAngle > 0 )|| 
	   pEffect->m_fCurrentRotateAngle != 0)
	{
		D3DXMATRIX matRotate;
//		vAxis = pEffect->m_pParent->m_vPos - g_pD3dApp->m_pCamera->GetEyePt();
//		vAxis = pos - g_pD3dApp->m_pCamera->GetEyePt();
		D3DXMatrixRotationAxis( &matRotate, &vAxis, pEffect->m_fCurrentRotateAngle );
		matScale *= matRotate;
	}
	matScale._41 = pos.x;
	matScale._42 = pos.y;
	matScale._43 = pos.z;

	D3DMATERIAL9 mtrl;
	D3DUtil_InitMaterial(mtrl, pEffect->m_cColor.r, 
		pEffect->m_cColor.g, pEffect->m_cColor.b, pEffect->m_cColor.a);

	g_pD3dDev->SetTransform( D3DTS_WORLD, &matScale );
	if(pEffect->m_nTextureVertexBufferType == 0)
		g_pD3dDev->SetStreamSource( 0, m_pVB4[pEffect->m_nSpriteType],0, sizeof(SPRITE_VERTEX) );
	else if(pEffect->m_nTextureVertexBufferType == 1)
		g_pD3dDev->SetStreamSource( 0, m_pVB8[pEffect->m_nSpriteType],0, sizeof(SPRITE_VERTEX) );
	else if(pEffect->m_nTextureVertexBufferType == 2)
		g_pD3dDev->SetStreamSource( 0, m_pVB16[pEffect->m_nSpriteType],0, sizeof(SPRITE_VERTEX) );
	else if(pEffect->m_nTextureVertexBufferType == 3)
		g_pD3dDev->SetStreamSource( 0, m_pVB2[pEffect->m_nSpriteType],0, sizeof(SPRITE_VERTEX) );
	else if(pEffect->m_nTextureVertexBufferType == 4)
		g_pD3dDev->SetStreamSource( 0, m_pVB1,0, sizeof(SPRITE_VERTEX) );

	g_pD3dDev->SetMaterial( &mtrl );
	// set texture
	if(pEffect->m_pParent && pEffect->m_pParent->m_pTexture ) 
	{
		g_pD3dDev->SetTexture(0, pEffect->m_pParent->m_pTexture ); 
	}
	else
	{
		int index = LoadTexture(pEffect->m_strTextureFile);
		if(index>=0)
		{
			g_pD3dDev->SetTexture( 0, m_pTexture[index]);
			if(m_nTextureRenderCount[index] == 2)
				m_nTextureRenderCount[index]++;
		}
	}
	//22.12.2024 Inet - vb null return
	if(pEffect->m_nTextureVertexBufferType == 0 && !m_pVB4[pEffect->m_nSpriteType])
		return;
	else if(pEffect->m_nTextureVertexBufferType == 1 && !m_pVB8[pEffect->m_nSpriteType])
		return;
	else if(pEffect->m_nTextureVertexBufferType == 2 && !m_pVB16[pEffect->m_nSpriteType])
		return;
	else if(pEffect->m_nTextureVertexBufferType == 3 && !m_pVB2[pEffect->m_nSpriteType])
		return;
	else if(pEffect->m_nTextureVertexBufferType == 4 && !m_pVB1)
		return;

	g_pD3dDev->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );

	if(strlen(pEffect->m_strLightMapFile)>0)
	{
		g_pD3dDev->SetRenderState( D3DRS_ALPHABLENDENABLE,pEffect->m_bLightMapAlphaBlending );
		g_pD3dDev->SetRenderState(D3DRS_SRCBLEND,pEffect->m_nLightMapSrcBlend);
		g_pD3dDev->SetRenderState(D3DRS_DESTBLEND,pEffect->m_nLightMapDestBlend);
		g_pD3dDev->SetTextureStageState(0,D3DTSS_COLOROP,pEffect->m_nLightMapRenderState);
//		Set texture
		if(pEffect->m_pParent && pEffect->m_pParent->m_pTexture ) {
			g_pD3dDev->SetTexture(0, pEffect->m_pParent->m_pTexture ); }
		else
		{
			int index = LoadTexture(pEffect->m_strLightMapFile);
			if(index>=0)
			{
				g_pD3dDev->SetTexture( 0, m_pTexture[index]);
				if(m_nTextureRenderCount[index] == 2)
					m_nTextureRenderCount[index]++;
			}
		}
		g_pD3dDev->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
	}
	g_pD3dDev->SetRenderState(D3DRS_SRCBLEND,dwSrc);
	g_pD3dDev->SetRenderState(D3DRS_DESTBLEND,dwDest);
	g_pD3dDev->SetTextureStageState(0,D3DTSS_COLOROP,dwColorOp);

	g_pD3dDev->LightEnable( 2, FALSE );
	if( pEffect->m_bZWriteEnable == FALSE)
	{
		g_pD3dDev->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
	}
	m_nSpriteEffectRender++;
}

int CEffectRender::ParticleRender(CParticleSystem* pParticleSystem, CParticle* p, D3DXVECTOR3 vAxis, int nOldTextureIndex)
{
	FLOG( "CEffectRender::ParticleRender(CParticleSystem* pParticleSystem, CParticle* p, D3DXVECTOR3 vAxis, int nOldTextureIndex)" );
	D3DXMATRIX matScale;
	D3DXVECTOR3 pos;
	D3DMATERIAL9 mtrl;
	D3DUtil_InitMaterial(mtrl, p->m_cColor.r, p->m_cColor.g,p->m_cColor.b, p->m_cColor.a);
	pos = p->m_vPos;//˝ÇÁ¦ ÁÂÇĄ
	
	// 2010. 03. 18 by jskim ¸ó˝şĹÍşŻ˝Ĺ Ä«µĺ
	float tempScale = 0.0f;
	if(p->m_pParent->m_pParent->m_pParent->m_MonsterTransformer &&
		p->m_pParent->m_pParent->m_pParent->m_MonsterTransScale > 0)
	{
		tempScale = p->m_fSize * p->m_pParent->m_pParent->m_pParent->m_MonsterTransScale;
	}
	else
	{
		tempScale = p->m_fSize;
	}
	//D3DXMatrixScaling(&matScale, p->m_fSize,p->m_fSize,p->m_fSize);	
	D3DXMatrixScaling(&matScale, tempScale,tempScale,tempScale);
	// 2010. 03. 18 by jskim ¸ó˝şĹÍşŻ˝Ĺ Ä«µĺ
	
	matScale *= g_pD3dApp->m_pCamera->GetBillboardMatrix();
	// 2004.2.16 ĆÄĆĽĹ¬ ·Ł´ý Č¸Ŕü
	if(pParticleSystem->m_pParent && p->m_fCurrentRotateAngle != 0 )//pParticleSystem->m_pParent->m_fBillboardRotateAngle > 0)
	{
		D3DXMATRIX matRotate;
//			vAxis = pParticleSystem->m_pParent->m_vPos - g_pD3dApp->m_pCamera->GetEyePt();
//			vAxis = p->m_vPos - g_pD3dApp->m_pCamera->GetEyePt();
		D3DXMatrixRotationAxis( &matRotate, &vAxis, p->m_fCurrentRotateAngle );
		matScale *= matRotate;
	}
	matScale._41 = pos.x;
	matScale._42 = pos.y;
	matScale._43 = pos.z;
//		matScale._41 += pParticleSystem->m_pParent->m_vPos.x;
//		matScale._42 += pParticleSystem->m_pParent->m_vPos.y;
//		matScale._43 += pParticleSystem->m_pParent->m_vPos.z;

	g_pD3dDev->SetTransform( D3DTS_WORLD, &matScale );
	// set texture
	
	int index = 0;
	if(pParticleSystem->m_pParent && pParticleSystem->m_pParent->m_pTexture) {
		g_pD3dDev->SetTexture( 0, pParticleSystem->m_pParent->m_pTexture); }
	else if (nOldTextureIndex == -1 || pParticleSystem->m_nTextureNumber != 1)
	{
		if (p && p != nullptr && p->m_nTextureType >= 0 && p->m_nTextureType < sizeof(pParticleSystem->m_strTextureName)) {//31-07-2024 by inet - there was array overflow
			index = LoadTexture(pParticleSystem->m_strTextureName[p->m_nTextureType]);
			if (index >= 0 && nOldTextureIndex != index)
			{
				g_pD3dDev->SetTexture(0, m_pTexture[index]);
				if (m_nTextureRenderCount[index] == 2)
					m_nTextureRenderCount[index]++;
				nOldTextureIndex = index;
			}
		}
	}
	g_pD3dDev->SetMaterial( &mtrl );

	if(m_pVB1) //22.12.2024 Inet - fix for d3d9 crash
		g_pD3dDev->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );

	m_nParticleEffectRender++;

	return nOldTextureIndex;
}

// 2007-11-08 by bhsohn ŔÎşĄ ŔĚĆĺĆ® °ü·Ă Ăł¸®
void CEffectRender::RenderParticleInvenVector(int nMatIndex, D3DXMATRIX matShuttlePos, D3DXMATRIX matPos, float fUnitScaling)
{
	if(m_vecInvenParticleInfo.empty())
	{
		return;
	}
	int  nSize = m_vecInvenParticleInfo.size();
	int nRender = 0;

//	vector<structInvenParticleInfo>::iterator itInvenEffect = m_vecInvenParticleInfo.begin();
	
	for(auto el: m_vecInvenParticleInfo)
	{
		structInvenParticleInfo struParticleInfo = el;

		if(struParticleInfo.nWindowInvenIdx != nMatIndex)
		{
			//itInvenEffect++;
			continue;
		}
		CEffectInfo*  pEffectInfo = GetEffectInfo(struParticleInfo.chEffectName, struParticleInfo.nWindowInvenIdx);
		if(NULL == pEffectInfo)
		{
			//itInvenEffect++;
			continue;
		}
		
		CParticleSystem* pParticleSystem = (CParticleSystem*)pEffectInfo->m_pEffect;						
		
		if(NULL == pParticleSystem)
		{
			//itInvenEffect++;
			continue;
		}

		{
			g_pD3dDev->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
			g_pD3dDev->SetFVF( D3DFVF_SPRITE_VERTEX );
			g_pD3dDev->SetRenderState( D3DRS_ZENABLE, pParticleSystem->m_bZbufferEnable );
//			g_pD3dDev->SetRenderState( D3DRS_ZENABLE, TRUE);
			if( pParticleSystem->m_bZWriteEnable == FALSE)
			{
				g_pD3dDev->SetRenderState( D3DRS_ZWRITEENABLE, pParticleSystem->m_bZWriteEnable );
			}
			
			g_pD3dDev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
			g_pD3dDev->SetRenderState( D3DRS_SRCBLEND, pParticleSystem->m_dwSrcBlend );
			g_pD3dDev->SetRenderState( D3DRS_DESTBLEND, pParticleSystem->m_dwDestBlend );

			g_pD3dDev->SetRenderState( D3DRS_LIGHTING, TRUE );

			g_pD3dDev->SetStreamSource( 0, m_pVB1,0, sizeof(SPRITE_VERTEX) );
			// set light
			D3DXVECTOR3 vAxis = g_pD3dApp->m_pCamera->GetViewDir();
			m_light2 = g_pD3dApp->m_pScene->m_light2;
			m_light2.Direction  = vAxis;
			g_pD3dDev->SetLight( 2, &m_light2 );
			g_pD3dDev->LightEnable( 2, TRUE );
			
			// ˝ÇÁ¦·Î ĆÄĆĽĹ¬ ŔĚĆĺĆ® ·Ł´ő¸µ ÇŘÁÖ´Â °÷			
			int nOldTextureIndex = -1;
			int i =0;
			for(i=0; i<pParticleSystem->m_vecParticle.size(); i++)
			{
				CParticle* p = (CParticle*)pParticleSystem->m_vecParticle[i];
				nOldTextureIndex = InvenParticleRender(pParticleSystem, p, vAxis, nOldTextureIndex, fUnitScaling, &matPos, &matShuttlePos);
			}			
			
			g_pD3dDev->LightEnable( 2, FALSE );
			if( pParticleSystem->m_bZWriteEnable == FALSE)
			{
				g_pD3dDev->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
			}
			// ĆÄĆĽĹ¬ ČŻ°ć Áľ·á
			{
				g_pD3dDev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
				g_pD3dDev->SetRenderState( D3DRS_ALPHATESTENABLE,  FALSE );
			}

			nRender++;
		}
		//itInvenEffect++;
	}
}

// 2007-11-08 by bhsohn ŔÎşĄ ŔĚĆĺĆ® °ü·Ă Ăł¸®
void CEffectRender::ResetContentInvneParticle()
{
	// ŔÎşĄ ĆÄĆĽĹ¬ ĂĘ±âČ­
	m_vecInvenParticleInfo.clear();
}
int CEffectRender::InvenParticleRender(CParticleSystem* pParticleSystem, CParticle* p, D3DXVECTOR3 vAxis, int nOldTextureIndex, float fUnitScaling, D3DXMATRIX* pmatPaticlePos, D3DXMATRIX* pmatShttlePos)
{
	FLOG( "CEffectRender::ParticleRender(CParticleSystem* pParticleSystem, CParticle* p, D3DXVECTOR3 vAxis, int nOldTextureIndex)" );
	D3DXMATRIX matScale;
	D3DXVECTOR3 pos;
	D3DMATERIAL9 mtrl;
	D3DUtil_InitMaterial(mtrl, p->m_cColor.r, p->m_cColor.g,p->m_cColor.b, p->m_cColor.a);
	
	float fSize = p->m_fSize;
	pos = p->m_vPos;//˝ÇÁ¦ ÁÂÇĄ
	if(pmatPaticlePos && pmatShttlePos)
	{
		// ˝şÄÉŔĎ¸µ
		//fSize *= 0.0074f;
		fSize *= (fUnitScaling/2);
		
		
		// ÇöŔç żůµĺżˇ ´ëÇŃ Á¤ş¸		
		D3DXVECTOR3 vInvenShuttle, vParticlePos, vObjInvenPos;	// ˝ÇÁ¦ ±×·ÁÁú ĆÄĆĽĹ¬ Ŕ§Äˇ
		D3DXVECTOR3 vDirWorldVel; // żůµĺ ąćÇâş¤ĹÍ 
		D3DXVECTOR3 vDirInvenVel; // ŔÎşĄ ąćÇâş¤ĹÍ 
		D3DXVECTOR3 vWeaponInvenVel; // ą«±â ąćÇâş¤ĹÍ

		vWeaponInvenVel = D3DXVECTOR3(0,0,0);
		D3DXVECTOR3 vParCenter = D3DXVECTOR3(0,0,0);// ĆÄĆĽĹ¬ŔÇ Áß˝ÉÁˇ			
		D3DXVECTOR3 vParResultDir = D3DXVECTOR3(0,0,0);// ĆÄĆĽĹ¬ŔÇ Áß˝ÉÁˇ			
		
		// ŔÎşĄĹä¸®ł»żˇ ±âĂĽŔÇ Ŕ§Äˇ
		{
			vInvenShuttle.x = pmatShttlePos->_41;
			vInvenShuttle.y = pmatShttlePos->_42;
			vInvenShuttle.z = pmatShttlePos->_43;
		}

		// ŔÎşĄĹä¸®ł» ą«±â żŔşęÁ§Ć® Ŕ§Äˇ
		{		
			vObjInvenPos.x = pmatPaticlePos->_41;
			vObjInvenPos.y = pmatPaticlePos->_42;
			vObjInvenPos.z = pmatPaticlePos->_43;	
		}
				
		// ĆÄĆĽĹ¬żˇ ˝ÇÁúŔűŔ¸·Î Ŕ§ÄˇÇŇ Ŕ§Äˇ
		{
			vParticlePos.x = pmatPaticlePos->_41;
			vParticlePos.y = pmatPaticlePos->_42;
			vParticlePos.z = pmatPaticlePos->_43;
		}
		
		// ŔÎşĄ ąćÇâ ş¤ĹÍ
		{			
			vDirInvenVel.x = pmatPaticlePos->_31;
			vDirInvenVel.y = pmatPaticlePos->_32;
			vDirInvenVel.z = pmatPaticlePos->_33;
			vDirInvenVel *= -1;
			D3DXVec3Normalize( &vDirInvenVel, &vDirInvenVel);			
		}
		
		// żůµĺ ÁÂÇĄ ąćÇâ ş¤ĹÍ
		{
			vDirWorldVel.x = pParticleSystem->m_pParent->m_pParent->m_mMatrix._31;
			vDirWorldVel.y = pParticleSystem->m_pParent->m_pParent->m_mMatrix._32;
			vDirWorldVel.z = pParticleSystem->m_pParent->m_pParent->m_mMatrix._33;
			vDirWorldVel *= -1;
			D3DXVec3Normalize( &vDirWorldVel, &vDirWorldVel);			
			
		}
		// żůµĺ ÁÂÇĄżˇ ˝ÇÁúŔűŔÎ ĆÄĆĽĹ¬ŔÇ Áß˝ÉÁˇ		
		{
			vParCenter.x = pParticleSystem->m_pParent->m_pParent->m_mMatrix._41;
			vParCenter.y = pParticleSystem->m_pParent->m_pParent->m_mMatrix._42;
			vParCenter.z = pParticleSystem->m_pParent->m_pParent->m_mMatrix._43;
		}			
		
		D3DXVECTOR3 vCrossWorldAxisY;// Y ĂŕŔ» ±¸ÇŃ´Ů.
		D3DXVECTOR3 vCrossWorldAxisX;// X ĂŕŔ» ±¸ÇŃ´Ů.
		float fAngleRadianY =0.0f;
		float fAngleRadianX =0.0f;
		{
			D3DXVECTOR3 vTmpWeaponVel = p->m_vPos - vParCenter;
			D3DXVec3Normalize( &vTmpWeaponVel, &vTmpWeaponVel);		
			

			D3DXVec3Cross(&vCrossWorldAxisY, &vDirWorldVel, &vTmpWeaponVel); 
			D3DXVec3Normalize(&vCrossWorldAxisY,&vCrossWorldAxisY); // Áß˝ÉĂŕŔĚ´Ů. 
			
			D3DXVec3Cross(&vCrossWorldAxisX, &vDirWorldVel, &vCrossWorldAxisY); 
			D3DXVec3Normalize(&vCrossWorldAxisX,&vCrossWorldAxisX); // Áß˝ÉĂŕŔĚ´Ů. 

			fAngleRadianX = GetRadianVectorBetweenVector(vCrossWorldAxisX, vTmpWeaponVel);
			fAngleRadianY = GetRadianVectorBetweenVector(vDirWorldVel, vTmpWeaponVel);
		}

		{
			D3DXMATRIX matRotateY;
			D3DXMATRIX matRotateX;
			D3DXMATRIX matRotate;

			D3DXMatrixIdentity( &matRotateY); 
			D3DXMatrixIdentity( &matRotateX);
			D3DXMatrixRotationAxis(&matRotateY, &vCrossWorldAxisY, fAngleRadianY); 
			D3DXMatrixRotationAxis(&matRotateX, &vCrossWorldAxisX, fAngleRadianX); 
			matRotate = matRotateX * matRotateY;
			// ˝ÇÁúŔűŔÎ ŔĚĆĺĆ® ş¤ĹÍ¸¦ ±¸ÇŃ´Ů.
			D3DXVec3TransformCoord( &vParResultDir, &vDirInvenVel, &matRotate ); 
			D3DXVec3Normalize(&vParResultDir,&vParResultDir); // Áß˝ÉĂŕŔĚ´Ů. 			
		}
		
		{			
			// żůµĺÁÂÇĄżˇ ĆÄĆĽĹ¬ °Ĺ¸®
			D3DXVECTOR3 vParVel = vParCenter - p->m_vPos;
			float fParticleLength = D3DXVec3Length(&vParVel);			
			
			//fParticleLength *= 0.025f;				
			fParticleLength *= fUnitScaling;			
			
			vParResultDir*= fParticleLength;
			vParticlePos += vParResultDir;
		}

//		{
//			//p->m_vDir;		
//			// żůµĺÁÂÇĄżˇ ĆÄĆĽĹ¬ °Ĺ¸®
//			D3DXVECTOR3 vParVel = vParCenter - p->m_vPos;
//			float fParticleLength = D3DXVec3Length(&vParVel);
//			D3DXVec3Normalize( &vParVel, &vParVel);
//			
//			//fParticleLength *= 0.025f;				
//			fParticleLength *= fUnitScaling;
//			
//			vParVel*= fParticleLength;
//			vParticlePos += vParVel;
//		}
		pos	= vParticlePos;
	}
	
	D3DXMatrixScaling(&matScale, fSize,fSize,fSize);
	
	//matScale *= g_pD3dApp->m_pCamera->GetBillboardMatrix();	
	
	// 2004.2.16 ĆÄĆĽĹ¬ ·Ł´ý Č¸Ŕü
	if(pParticleSystem->m_pParent && p->m_fCurrentRotateAngle != 0 )//pParticleSystem->m_pParent->m_fBillboardRotateAngle > 0)
	{
		D3DXMATRIX matRotate;
//			vAxis = pParticleSystem->m_pParent->m_vPos - g_pD3dApp->m_pCamera->GetEyePt();
//			vAxis = p->m_vPos - g_pD3dApp->m_pCamera->GetEyePt();
		D3DXMatrixRotationAxis( &matRotate, &vAxis, p->m_fCurrentRotateAngle );
		matScale *= matRotate;
	}
	matScale._41 = pos.x;
	matScale._42 = pos.y;
	matScale._43 = pos.z;
//		matScale._41 += pParticleSystem->m_pParent->m_vPos.x;
//		matScale._42 += pParticleSystem->m_pParent->m_vPos.y;
//		matScale._43 += pParticleSystem->m_pParent->m_vPos.z;

	g_pD3dDev->SetTransform( D3DTS_WORLD, &matScale );
	// set texture
	
	int index = 0;
	if(pParticleSystem->m_pParent && pParticleSystem->m_pParent->m_pTexture) {
		g_pD3dDev->SetTexture( 0, pParticleSystem->m_pParent->m_pTexture); }
	else if(nOldTextureIndex == -1 || pParticleSystem->m_nTextureNumber != 1)
	{
		index = LoadTexture(pParticleSystem->m_strTextureName[p->m_nTextureType]);
		if(index>=0 && nOldTextureIndex != index)
		{
			g_pD3dDev->SetTexture( 0, m_pTexture[index]);
			if(m_nTextureRenderCount[index] == 2)
				m_nTextureRenderCount[index]++;
			nOldTextureIndex = index;
		}
	}
	g_pD3dDev->SetMaterial( &mtrl );

	if(m_pVB1) //22.12.2024 Inet - fix for d3d9 crash
		g_pD3dDev->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );

	return nOldTextureIndex;
}


void CEffectRender::EffectPlaneRender( CEffectPlane *pEffect )
{
	//return g_pDrawingProc->EffectPlaneRenderDP( pEffect,this);
	if(pEffect->m_pParent->m_nCurrentNumberOfTrace<=0)
		return;
	int index = LoadTexture(pEffect->m_pParent->m_strTextureName[pEffect->m_pParent->m_nCurrentTextureNumber]);

	DWORD dwSrc,dwDest,dwColorOp;
	g_pD3dDev->GetRenderState(D3DRS_SRCBLEND,&dwSrc);
	g_pD3dDev->GetRenderState(D3DRS_DESTBLEND,&dwDest);
	g_pD3dDev->GetTextureStageState(0,D3DTSS_COLOROP,&dwColorOp);

//	g_pD3dDev->ApplyStateBlock( pEffect->m_dwStateBlock );
	g_pD3dDev->SetRenderState( D3DRS_ZENABLE, pEffect->m_pParent->m_bZbufferEnable );
	if( pEffect->m_pParent->m_bZWriteEnable == FALSE)
	{
		g_pD3dDev->SetRenderState( D3DRS_ZWRITEENABLE, pEffect->m_pParent->m_bZWriteEnable );
	}
	g_pD3dDev->SetRenderState( D3DRS_LIGHTING, FALSE );
	g_pD3dDev->SetRenderState( D3DRS_ALPHABLENDENABLE, pEffect->m_pParent->m_bAlphaBlendEnable );
	g_pD3dDev->SetRenderState( D3DRS_SRCBLEND, pEffect->m_pParent->m_dwSrcBlend );
	g_pD3dDev->SetRenderState( D3DRS_DESTBLEND, pEffect->m_pParent->m_dwDestBlend );
	g_pD3dDev->SetTextureStageState(0,D3DTSS_COLOROP,pEffect->m_pParent->m_nTextureRenderState);
	g_pD3dDev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
//	g_pD3dDev->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
//	g_pD3dDev->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
	// 2005-01-03 by jschoi
//    g_pD3dDev->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR  );
//    g_pD3dDev->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR  );
//	g_pD3dDev->SetSamplerState(0,D3DSAMP_MINFILTER,D3DTEXF_LINEAR);
//	g_pD3dDev->SetSamplerState(0,D3DSAMP_MAGFILTER,D3DTEXF_LINEAR);

	D3DXMATRIX mat;
	D3DXMatrixIdentity( &mat);
	g_pD3dDev->SetTransform( D3DTS_WORLD, &mat );
    g_pD3dDev->SetFVF( D3DFVF_SPRITE_VERTEX );
	if(index>=0)
	{
		if( pEffect->m_pParent && 
			pEffect->m_pParent->m_pParent && 
			pEffect->m_pParent->m_pParent->m_pTexture ) 
		{
			g_pD3dDev->SetTexture( 0, pEffect->m_pParent->m_pParent->m_pTexture);
		}
		else 
		{
			g_pD3dDev->SetTexture( 0, m_pTexture[index]);
		}
		if(m_nTextureRenderCount[index] == 2)
		{
			m_nTextureRenderCount[index]++;
		}
	}

//	g_pD3dDev->SetTexture( 0, NULL);
	D3DMATERIAL9 mtrl;
	D3DUtil_InitMaterial(mtrl, 1.0f,1.0f,1.0f, 1.0f);
	g_pD3dDev->SetMaterial( &mtrl );
	pEffect->Render();
	g_pD3dDev->SetRenderState(D3DRS_SRCBLEND,dwSrc);
	g_pD3dDev->SetRenderState(D3DRS_DESTBLEND,dwDest);
	g_pD3dDev->SetTextureStageState(0,D3DTSS_COLOROP,dwColorOp);
	g_pD3dDev->SetRenderState( D3DRS_LIGHTING, TRUE );
	
	if( pEffect->m_pParent->m_bZWriteEnable == FALSE)
	{
		g_pD3dDev->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
	}
	m_nTraceEffectRender++;
}

void CEffectRender::TraceAniRender( CTraceAni* pEffect )
{
	FLOG( "CEffectRender::TraceAniRender( CTraceAni* pEffect )" );
	
	for(int i=0;i<pEffect->m_nNumberOfTrace;i++)
	{
		if(pEffect->m_pEffectPlane[i])
		{
			EffectPlaneRender(pEffect->m_pEffectPlane[i]);
		}
	}
	
/*	if(pEffect->m_nCurrentNumberOfTrace<=0)
		return;
	int index = LoadTexture(pEffect->m_strTextureName[pEffect->m_nCurrentTextureNumber]);

	DWORD dwSrc,dwDest,dwColorOp;
	g_pD3dDev->GetRenderState(D3DRS_SRCBLEND,&dwSrc);
	g_pD3dDev->GetRenderState(D3DRS_DESTBLEND,&dwDest);
	g_pD3dDev->GetTextureStageState(0,D3DTSS_COLOROP,&dwColorOp);

//	g_pD3dDev->ApplyStateBlock( pEffect->m_dwStateBlock );
	g_pD3dDev->SetRenderState( D3DRS_ZENABLE, pEffect->m_bZbufferEnable );
	g_pD3dDev->SetRenderState( D3DRS_LIGHTING, TRUE );
	g_pD3dDev->SetRenderState( D3DRS_ALPHABLENDENABLE, pEffect->m_bAlphaBlendEnable );
	g_pD3dDev->SetRenderState( D3DRS_SRCBLEND, pEffect->m_dwSrcBlend );
	g_pD3dDev->SetRenderState( D3DRS_DESTBLEND, pEffect->m_dwDestBlend );
	g_pD3dDev->SetTextureStageState(0,D3DTSS_COLOROP,pEffect->m_nTextureRenderState);
	g_pD3dDev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
//	g_pD3dDev->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
//	g_pD3dDev->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
    g_pD3dDev->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR  );
    g_pD3dDev->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR  );

	D3DXMATRIX mat;
	D3DXMatrixIdentity( &mat);
	g_pD3dDev->SetTransform( D3DTS_WORLD, &mat );
    g_pD3dDev->SetVertexShader( D3DFVF_SPRITE_VERTEX );
	if(index>=0)
	{
		if(pEffect->m_pParent && pEffect->m_pParent->m_pTexture) 
		{
			g_pD3dDev->SetTexture( 0, pEffect->m_pParent->m_pTexture);
		}
		else 
		{
			g_pD3dDev->SetTexture( 0, m_pTexture[index]);
		}
		if(m_nTextureRenderCount[index] == 2)
			m_nTextureRenderCount[index]++;
	}
	D3DMATERIAL8 mtrl;
	D3DUtil_InitMaterial(mtrl, 1.0f,1.0f,1.0f, 1.0f);
	g_pD3dDev->SetMaterial( &mtrl );
	g_pD3dDev->SetStreamSource( 0, pEffect->m_pVBTrace[0], sizeof(SPRITE_VERTEX) );
	g_pD3dDev->DrawPrimitive( D3DPT_TRIANGLESTRIP , 0,pEffect->m_nCurrentNumberOfTrace*2 );
	g_pD3dDev->SetStreamSource( 0, pEffect->m_pVBTrace[1], sizeof(SPRITE_VERTEX) );
	g_pD3dDev->DrawPrimitive( D3DPT_TRIANGLESTRIP , 0,pEffect->m_nCurrentNumberOfTrace*2 );
	g_pD3dDev->SetRenderState(D3DRS_SRCBLEND,dwSrc);
	g_pD3dDev->SetRenderState(D3DRS_DESTBLEND,dwDest);
	g_pD3dDev->SetTextureStageState(0,D3DTSS_COLOROP,dwColorOp);
*/
}


void CEffectRender::ObjectAniRender(CObjectAni* pEffect, BOOL bAlpha, int nAlphaValue)
{
//	return g_pDrawingProc->ObjectAniRenderDP( pEffect,  bAlpha,  nAlphaValue,this);
	FLOG( "CEffectRender::ObjectAniRender(CObjectAni* pEffect)" );
	if( strlen(pEffect->m_strName) == 0 )
	{
		return;
	}
	
	DWORD dwSrc,dwDest,dwColorOp;
	DWORD dwFogValue = FALSE;
	g_pD3dDev->GetRenderState(D3DRS_SRCBLEND,&dwSrc);
	g_pD3dDev->GetRenderState(D3DRS_DESTBLEND,&dwDest);
	g_pD3dDev->GetTextureStageState(0,D3DTSS_COLOROP,&dwColorOp);

    g_pD3dDev->SetRenderState( D3DRS_ALPHATESTENABLE,  pEffect->m_bAlphaTestEnble );
	if(pEffect->m_bAlphaTestEnble)
	{
		g_pD3dDev->SetRenderState( D3DRS_ALPHAREF, pEffect->m_nAlphaTestValue );
		g_pD3dDev->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
	}

	g_pD3dDev->SetRenderState( D3DRS_LIGHTING, TRUE );

//	g_pD3dDev->SetRenderState( D3DRS_ZENABLE, TRUE);
	// 2008-07-30 by dgwoo ľĆ°Ü ĆŻÁ¤ľĆ¸Ó şÎ˝şĹÍ »çżë˝Ă ŔĚĆĺĆ®°ˇ ľĆ¸ÓµÚżˇ ŔÖŔ»°ćżěżˇµµ ş¸ŔÎ´Ů.
	// 2008-10-22 by bhsohn şÎ˝şĹÍ ľ˛¸éĽ­ °řĆř »çżë˝Ă, ł×¸đł­ ĹŘ˝şĂł »ý±â´Â ąö±× ĽöÁ¤
	g_pD3dDev->SetRenderState( D3DRS_ZENABLE, pEffect->m_bZbufferEnable);
	//g_pD3dDev->SetRenderState( D3DRS_ZENABLE, TRUE);

	if( pEffect->m_bZWriteEnable == FALSE)
	{
		g_pD3dDev->SetRenderState( D3DRS_ZWRITEENABLE, pEffect->m_bZWriteEnable );
	}
	g_pD3dDev->SetRenderState( D3DRS_ALPHABLENDENABLE,  pEffect->m_bAlphaBlending );
	if(pEffect->m_bAlphaBlending)
	{
		g_pD3dDev->GetRenderState( D3DRS_FOGENABLE,  &dwFogValue );
		if(pEffect->m_nSrcBlend == D3DBLEND_ONE && pEffect->m_nDestBlend == D3DBLEND_ONE && dwFogValue == TRUE)
		{
			g_pD3dDev->SetRenderState( D3DRS_FOGENABLE,  FALSE );
		}
		g_pD3dDev->SetRenderState(D3DRS_SRCBLEND,pEffect->m_nSrcBlend);
		g_pD3dDev->SetRenderState(D3DRS_DESTBLEND,pEffect->m_nDestBlend);
	}
	g_pD3dDev->SetTextureStageState(0,D3DTSS_COLOROP,pEffect->m_nTextureRenderState);
	g_pD3dDev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	
	//*--------------------------------------------------------------------------*//
	// 2006-11-16 by ispark, ľËĆÄ
	if(bAlpha)
	{
		g_pD3dApp->SetAlphaRenderState(nAlphaValue);
	}
	//*--------------------------------------------------------------------------*//

	D3DXMATRIX matScale;
	D3DXVECTOR3 pos;
	if(pEffect->m_pParent)
	{
		pos = pEffect->m_pParent->m_vPos;//»ó´ë ÁÂÇĄ
	}
	else 
	{
		pos = D3DXVECTOR3(0,0,0);
	}
	// 2010. 03. 18 by jskim ¸ó˝şĹÍşŻ˝Ĺ Ä«µĺ
	float tempScale = 0.0f;
	if(pEffect->m_pParent->m_pParent->m_MonsterTransformer &&
	   pEffect->m_pParent->m_pParent->m_MonsterTransScale > 0)
	{
		tempScale = pEffect->m_fScale * pEffect->m_pParent->m_pParent->m_MonsterTransScale;
	}
	else
	{
		tempScale = pEffect->m_fScale;
	}
	//D3DXMatrixScaling(&matScale, pEffect->m_fScale,pEffect->m_fScale,pEffect->m_fScale);
	D3DXMatrixScaling(&matScale, tempScale,tempScale,tempScale);
	//end 2010. 03. 18 by jskim ¸ó˝şĹÍşŻ˝Ĺ Ä«µĺ
	if(pEffect->m_pParent && pEffect->m_pParent->m_bUseBillboard)
	{
		if( pEffect->m_pParent->m_bGroundBillboard == TRUE )
		{
			D3DXVECTOR3 vPos(pEffect->m_pParent->m_pParent->m_mMatrix._41, 
				pEffect->m_pParent->m_pParent->m_mMatrix._42,
				pEffect->m_pParent->m_pParent->m_mMatrix._43);
			D3DXMATRIX matTemp;
			D3DXVECTOR3 vVel = g_pD3dApp->m_pCamera->GetEyePt() - vPos;
			D3DXVECTOR3 vUp(0,1,0);
			if( vVel == vUp )
			{
				vVel = D3DXVECTOR3(1,0,0);
			}
			else
			{
				D3DXVec3Cross(&vVel, &vVel, &vUp );
				D3DXVec3Cross(&vVel, &vVel, &vUp );
			}
			D3DXVec3Normalize( &vVel, &vVel );
			D3DXMatrixLookAtLH( &matTemp, &(vPos), &(vPos + vVel ), &vUp);
			D3DXMatrixInverse( &matTemp, NULL, &matTemp );
			matScale = matTemp * matScale;
		}
		else
		{
			D3DXMATRIX matBillboard = g_pD3dApp->m_pCamera->GetBillboardMatrix();
			D3DXMATRIX matRotate;
			D3DXVECTOR3 vAxis = g_pD3dApp->m_pCamera->GetViewDir();//pos - g_pD3dApp->m_pCamera->GetEyePt();
			D3DXMatrixRotationAxis( &matRotate, &vAxis, pEffect->m_pParent->m_fBillboardAngle );
			matBillboard *= matRotate;

			if(pEffect->m_pParent->m_fBillboardRotatePerSec>0)
			{
				D3DXMatrixRotationAxis( &matRotate, &vAxis, pEffect->m_fCurrentBillboardRotateAngle );
				matBillboard *= matRotate;
			} 
			if(pEffect->m_fCurrentRandomUpAngleX != 0)
			{
				D3DXMatrixRotationAxis( &matRotate, &vAxis, pEffect->m_fCurrentRandomUpAngleX );
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
		if(!pEffect->m_bUseEnvironmentLight)
		{
			m_light2 = g_pD3dApp->m_pScene->m_light2;
			m_light2.Direction  = g_pD3dApp->m_pCamera->GetViewDir();
 			g_pD3dDev->SetLight( 2, &m_light2 );
			g_pD3dDev->LightEnable( 2, TRUE );
		}
	} else
	{
		D3DXMATRIX lookat;
		D3DXVECTOR3 up = pEffect->m_pParent->m_vUp;
		D3DXVECTOR3 target = pEffect->m_pParent->m_vTarget;
		if(pEffect->m_fCurrentRandomUpAngleX != 0 || pEffect->m_fCurrentRandomUpAngleZ != 0)
		{
			up = D3DXVECTOR3(0,1,0);
			target = pEffect->m_pParent->m_vPos;
			D3DXVECTOR3 vel = D3DXVECTOR3(0,0,1);
			D3DXMATRIX matRotateX, matRotateZ;
			D3DXVECTOR3 vAxis(1,0,0);// up vector Č¸ŔüĂŕ(XĂŕ)
			D3DXMatrixRotationAxis( &matRotateX, &vAxis, pEffect->m_fCurrentRandomUpAngleX);
			D3DXVec3TransformCoord( &up, &up, &matRotateX );
			D3DXVec3TransformCoord( &vel, &vel, &matRotateX );
			vAxis = D3DXVECTOR3(0,0,1);// up vector Č¸ŔüĂŕ(XĂŕ)
			D3DXMatrixRotationAxis( &matRotateZ, &vAxis, pEffect->m_fCurrentRandomUpAngleZ);
			D3DXVec3TransformCoord( &up, &up, &matRotateZ );
			D3DXVec3TransformCoord( &vel, &vel, &matRotateZ );
			target += vel;
		} 
		D3DXMatrixLookAtLH( &lookat, &pos, &target, &up);
		D3DXMatrixInverse( &lookat, NULL, &lookat );
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
		if(!pEffect->m_bUseEnvironmentLight)
		{
			m_light2 = g_pD3dApp->m_pScene->m_light2;
			m_light2.Direction  = g_pD3dApp->m_pCamera->GetViewDir();
 			g_pD3dDev->SetLight( 2, &m_light2 );
			g_pD3dDev->LightEnable( 2, TRUE );
		}
	}
	CSkinnedMesh* pMesh = LoadObject(pEffect->m_strObjectFile);
	if(!pMesh)
	{
		if(!pEffect->m_bUseEnvironmentLight)
		{
			g_pD3dDev->LightEnable( 2, FALSE );
		}
		if(pEffect->m_nSrcBlend == D3DBLEND_ONE && pEffect->m_nDestBlend == D3DBLEND_ONE && dwFogValue == TRUE)
		{
			g_pD3dDev->SetRenderState( D3DRS_FOGENABLE,  dwFogValue );
		}
		if( pEffect->m_bZWriteEnable == FALSE)
		{
			g_pD3dDev->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
		}
		return;
	}

	switch(pEffect->m_nObjectAniType)
	{
	case 0:// object animation
		{	//m_pObjEffectMesh[index]
			pMesh->Tick(pEffect->m_fCurrentObjectAniTime);
			pMesh->AnotherTexture(1);
		}
		break;
	case 1:// texture animation
		{
			pMesh->AnotherTexture(pEffect->m_nCurrentTextureType+1);
		}
		break;
	case 2:// object + texture animation
		{
			pMesh->Tick(pEffect->m_fCurrentObjectAniTime);
			pMesh->AnotherTexture(pEffect->m_nCurrentTextureType+1);
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

	if(!pEffect->m_bZbufferEnable && m_bZBufferTemp)
	{
		D3DXVECTOR3 vTempPos;
		vTempPos.x = matScale._41;
		vTempPos.y = matScale._42;
		vTempPos.z = matScale._43;
		D3DXMATRIX matTemp;
		D3DXVECTOR3 vPos,vVel,vUp,vSide;
		vPos = g_pD3dApp->m_pCamera->GetEyePt();
		D3DXVec3Normalize(&vUp,&g_pD3dApp->m_pCamera->GetUpVec());
		D3DXVec3Normalize(&vVel,&(vTempPos - vPos));
		D3DXVec3Cross(&vSide,&vUp,&vVel);
		D3DXVec3Cross(&vUp,&vVel,&vSide);

		D3DXMatrixLookAtLH(&matTemp,&vPos,&(vPos + vVel),&vUp);
		float fDist1,fDist2;
		fDist1 = D3DXVec3Length(&(vTempPos - vPos));
		fDist2 = g_pD3dApp->m_pScene->m_pObjectRender->CheckPickMesh(matTemp,vPos).fDist;
		if(fDist1 > fDist2)
			g_pD3dDev->SetRenderState( D3DRS_ZENABLE, TRUE);
		else
			g_pD3dDev->SetRenderState( D3DRS_ZENABLE, FALSE);
	}
	// by dhkwon, 030917
	LPDIRECT3DTEXTURE9 pOrgTex = NULL;
	if(pEffect->m_pParent && pEffect->m_pParent->m_pTexture ) {
		pOrgTex = pMesh->SetTexture(pEffect->m_pParent->m_pTexture, 0); }
	else if(pEffect->m_strTextureFile[0])
	{
		int i = LoadTexture(pEffect->m_strTextureFile);
		if(m_nTextureRenderCount[i] == 2)
			m_nTextureRenderCount[i]++;
		pOrgTex = pMesh->SetTexture(m_pTexture[i], 0);
	}
	DWORD dwLightColorOp = 0;
	if(pEffect->m_bLightMapUse)
	{
		g_pD3dDev->GetTextureStageState( 1, D3DTSS_COLOROP, &dwLightColorOp );
		g_pD3dDev->SetTextureStageState( 1, D3DTSS_COLOROP, pEffect->m_nLightMapRenderState );
        g_pD3dDev->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 0 );

		g_pD3dDev->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		g_pD3dDev->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );
	}

	// 2006-01-24 by ispark
	if((!g_pD3dApp->m_bCharacter &&									// ±âľî ŔĚ¸éĽ­
		g_pD3dApp->m_pCamera->GetCamType() != CAMERA_TYPE_FPS &&	//  1ŔÎÄŞ ¸đµĺ°ˇ ľĆ´Ď±¸
		pEffect->m_pParent->m_nInvenWeaponIndex > 0) ||				// ŔÎşĄ ą«±â ŔÎµ¦˝ş°ˇ 0 ŔĚ»óŔĚ¸é ·»´ő¸µ ÇĎÁö¸¶¶ó
		pEffect->m_pParent->m_nInvenWeaponIndex == 0)				// ŔÎşĄ ą«±â ŔÎµ¦˝ş°ˇ 0ŔĚ¸é ·»´ő¸µ 
	{
		if(pEffect->m_bLightMapUse)
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
	if(pEffect->m_pParent->m_nInvenWeaponIndex > 0	&&  g_pGameMain->IsEquipInvenShow() == TRUE)
	{
		D3DXMATRIX pMatOldView, pMatOldProj, pMatPresView, pMatPresProj, pMatrix;
		D3DXMatrixIdentity(&pMatOldView);
		D3DXMatrixIdentity(&pMatOldProj);
		D3DXMatrixIdentity(&pMatPresView);
		D3DXMatrixIdentity(&pMatPresProj);
		D3DXMatrixIdentity(&pMatrix);		
		
		// ÇöŔç şäżÍ ÇÁ·ÎÁ§ĽÇŔ» °ˇÁ®żÂ´Ů
		g_pD3dDev->GetTransform( D3DTS_VIEW,       &pMatOldView );
		g_pD3dDev->GetTransform( D3DTS_PROJECTION, &pMatOldProj );	
		
		g_pD3dDev->SetTransform( D3DTS_VIEW,		&pMatPresView);
		g_pD3dDev->SetTransform( D3DTS_PROJECTION,	&pMatPresProj);
		g_pD3dDev->SetRenderState( D3DRS_LIGHTING, FALSE );
		
		if(pEffect->m_pParent->m_nInvenWeaponIndex > 4)
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
		
		g_pD3dDev->SetTransform( D3DTS_VIEW,		&pMatOldView );
		g_pD3dDev->SetTransform( D3DTS_PROJECTION,	&pMatOldProj );
		g_pD3dDev->SetRenderState( D3DRS_LIGHTING, TRUE );
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

	if((pEffect->m_pParent && pEffect->m_pParent->m_pTexture) || pEffect->m_strTextureFile[0])
	{
		pMesh->SetTexture(pOrgTex, 0);
	}

	if(pMesh && pMesh->m_nRenderCount == 2)
		pMesh->m_nRenderCount++;
	pMesh->m_bMaterial = bTemp;
	g_pD3dDev->SetRenderState(D3DRS_SRCBLEND,dwSrc);
	g_pD3dDev->SetRenderState(D3DRS_DESTBLEND,dwDest);
	g_pD3dDev->SetTextureStageState(0,D3DTSS_COLOROP,dwColorOp);
	if(!pEffect->m_bUseEnvironmentLight)
	{
		g_pD3dDev->LightEnable( 2, FALSE );
	}
	if(pEffect->m_bLightMapUse)
	{
		g_pD3dDev->SetTextureStageState( 1, D3DTSS_COLOROP, dwLightColorOp );
	}
	if(pEffect->m_nSrcBlend == D3DBLEND_ONE && pEffect->m_nDestBlend == D3DBLEND_ONE && dwFogValue == TRUE)
	{
		g_pD3dDev->SetRenderState( D3DRS_FOGENABLE,  dwFogValue );
	}
	if(pEffect->m_bAlphaTestEnble)
	{
		g_pD3dDev->SetRenderState( D3DRS_ALPHATESTENABLE,  FALSE );
	}
	if( pEffect->m_bZWriteEnable == FALSE)
	{
		g_pD3dDev->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
	}

	m_nObjectEffectRender++;
}

void CEffectRender::ObjectParticleRender(CObjectAni* pEffect, CParticle* pParticle)
{
//	return g_pDrawingProc->ObjectParticleRenderDP( pEffect,  pParticle,this);

	FLOG( "CEffectRender::ObjectParticleRender(CObjectAni* pEffect, CParticle* pParticle)" );
	DWORD dwSrc,dwDest,dwColorOp;
	DWORD dwFogValue = FALSE;
	g_pD3dDev->GetRenderState(D3DRS_SRCBLEND,&dwSrc);
	g_pD3dDev->GetRenderState(D3DRS_DESTBLEND,&dwDest);
	g_pD3dDev->GetTextureStageState(0,D3DTSS_COLOROP,&dwColorOp);

    g_pD3dDev->SetRenderState( D3DRS_ALPHATESTENABLE,  pEffect->m_bAlphaTestEnble );
	if(pEffect->m_bAlphaTestEnble)
	{
		g_pD3dDev->SetRenderState( D3DRS_ALPHAREF, pEffect->m_nAlphaTestValue );
		g_pD3dDev->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
	}

	g_pD3dDev->SetRenderState( D3DRS_LIGHTING, TRUE );

//	g_pD3dDev->SetRenderState( D3DRS_ZENABLE, TRUE);
	g_pD3dDev->SetRenderState( D3DRS_ZENABLE, pEffect->m_bZbufferEnable);

	if( pEffect->m_bZWriteEnable == FALSE)
	{
		g_pD3dDev->SetRenderState( D3DRS_ZWRITEENABLE, pEffect->m_bZWriteEnable );
	}
	g_pD3dDev->SetRenderState( D3DRS_ALPHABLENDENABLE,  pEffect->m_bAlphaBlending );
	if(pEffect->m_bAlphaBlending)
	{
		g_pD3dDev->GetRenderState( D3DRS_FOGENABLE,  &dwFogValue );
		if(pEffect->m_nSrcBlend == D3DBLEND_ONE && pEffect->m_nDestBlend == D3DBLEND_ONE && dwFogValue == TRUE)
		{
			g_pD3dDev->SetRenderState( D3DRS_FOGENABLE,  FALSE );
		}
		g_pD3dDev->SetRenderState(D3DRS_SRCBLEND,pEffect->m_nSrcBlend);
		g_pD3dDev->SetRenderState(D3DRS_DESTBLEND,pEffect->m_nDestBlend);
	}
	g_pD3dDev->SetTextureStageState(0,D3DTSS_COLOROP,pEffect->m_nTextureRenderState);
	g_pD3dDev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
//	g_pD3dDev->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR  );
//	g_pD3dDev->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR  );
	D3DXMATRIX matScale;
	D3DXVECTOR3 pos;
	if(pParticle)
	{
//		pos = pData->m_pParent->m_vPos;
		pos = pParticle->m_vPos;//ÁÂÇĄ
	}
	else 
	{
		pos = D3DXVECTOR3(0,0,0);
	}
	// 2010. 03. 18 by jskim ¸ó˝şĹÍşŻ˝Ĺ Ä«µĺ
	//D3DXMatrixScaling(&matScale, pEffect->m_fScale,pEffect->m_fScale,pEffect->m_fScale);
	float tempScale = 0.0f;
	if(pEffect->m_pParent->m_pParent->m_MonsterTransformer &&
		pEffect->m_pParent->m_pParent->m_MonsterTransScale > 0)
	{
		tempScale = pEffect->m_fScale * pEffect->m_pParent->m_pParent->m_MonsterTransScale;
	}
	else
	{
		tempScale = pEffect->m_fScale;
	}
	
	D3DXMatrixScaling(&matScale, tempScale,tempScale,tempScale);
	//end 2010. 03. 18 by jskim ¸ó˝şĹÍşŻ˝Ĺ Ä«µĺ
	if(pEffect->m_pParent && pEffect->m_pParent->m_bUseBillboard)
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
		D3DXMatrixRotationAxis( &matRotate, &vAxis, pEffect->m_pParent->m_fBillboardAngle );
		matBillboard *= matRotate;

		if(pEffect->m_pParent->m_fBillboardRotatePerSec>0)
		{
			D3DXMatrixRotationAxis( &matRotate, &vAxis, pEffect->m_fCurrentBillboardRotateAngle );
			matBillboard *= matRotate;
		} 
		if(pEffect->m_fCurrentRandomUpAngleX != 0)
		{
			D3DXMatrixRotationAxis( &matRotate, &vAxis, pEffect->m_fCurrentRandomUpAngleX );
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
		if(!pEffect->m_bUseEnvironmentLight)
		{
			m_light2 = g_pD3dApp->m_pScene->m_light2;
			m_light2.Direction  = g_pD3dApp->m_pCamera->GetViewDir();
 			g_pD3dDev->SetLight( 2, &m_light2 );
			g_pD3dDev->LightEnable( 2, TRUE );
		}
	} else
	{
		D3DXVECTOR3 up,target;
		switch(pParticle->m_pParent->m_nObjMoveTargetType)
		{
		case OBJ_MOVE_TYPE0://ĆÄĆĽĹ¬ ŔĚµżąćÇâŔ» °ˇ¸®Ĺ´
			{
				target = pParticle->m_vObjTarget;
				up = pParticle->m_vObjUp;
			}
			break;
		case OBJ_MOVE_TYPE1:// ŔŻ´Ö ąćÇâŔ¸·Î °ˇ¸®Ĺ´
			{
				target = pEffect->m_pParent ? pEffect->m_pParent->m_vTarget : D3DXVECTOR3(0,0,1);
				up = pEffect->m_pParent ?  pEffect->m_pParent->m_vUp : D3DXVECTOR3(0,1,0);
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
		if(pEffect->m_fCurrentRandomUpAngleX != 0 || pEffect->m_fCurrentRandomUpAngleZ != 0)
		{
			up = D3DXVECTOR3(0,1,0);
			target = pParticle->m_vPos;
			D3DXVECTOR3 vel = D3DXVECTOR3(0,0,1);
			D3DXMATRIX matRotateX, matRotateZ;
			D3DXVECTOR3 vAxis(1,0,0);// up vector Č¸ŔüĂŕ(XĂŕ)
			D3DXMatrixRotationAxis( &matRotateX, &vAxis, pEffect->m_fCurrentRandomUpAngleX);
			D3DXVec3TransformCoord( &up, &up, &matRotateX );
			D3DXVec3TransformCoord( &vel, &vel, &matRotateX );
			vAxis = D3DXVECTOR3(0,0,1);// up vector Č¸ŔüĂŕ(XĂŕ)
			D3DXMatrixRotationAxis( &matRotateZ, &vAxis, pEffect->m_fCurrentRandomUpAngleZ);
			D3DXVec3TransformCoord( &up, &up, &matRotateZ );
			D3DXVec3TransformCoord( &vel, &vel, &matRotateZ );
			target += vel;
		} 
		
		D3DXMatrixLookAtLH( &lookat, &D3DXVECTOR3(0,0,0), &target, &up);
//		D3DXMatrixLookAtLH( &lookat, &pos, &target, &up);
		D3DXMatrixInverse( &lookat, NULL, &lookat );
		matScale = lookat * matScale;// * matScale;
		D3DXMATRIX mat = pEffect->m_pParent->m_pParent->m_mMatrix;
		mat._41 = pos.x;
		mat._42 = pos.y;
		mat._43 = pos.z;
		matScale = matScale * mat;
		// set light
		if(!pEffect->m_bUseEnvironmentLight)
		{
			m_light2 = g_pD3dApp->m_pScene->m_light2;
			m_light2.Direction  = g_pD3dApp->m_pCamera->GetViewDir();
 			g_pD3dDev->SetLight( 2, &m_light2 );
			g_pD3dDev->LightEnable( 2, TRUE );
		}
	}

//	int index = LoadObject(pEffect->m_strObjectFile);
//	if(index<0)
//		return;
	CSkinnedMesh* pMesh = LoadObject(pEffect->m_strObjectFile);
	if(!pMesh)
	{
		if(!pEffect->m_bUseEnvironmentLight)
		{
		g_pD3dDev->LightEnable( 2, FALSE );
		}
		if(pEffect->m_nSrcBlend == D3DBLEND_ONE && pEffect->m_nDestBlend == D3DBLEND_ONE && dwFogValue == TRUE)
		{
			g_pD3dDev->SetRenderState( D3DRS_FOGENABLE,  dwFogValue );
		}
		if( pEffect->m_bZWriteEnable == FALSE)
		{
			g_pD3dDev->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
		}
		return;
	}
	switch(pEffect->m_nObjectAniType)
	{
	case 0:// object animation
		{
			pMesh->Tick(pEffect->m_fCurrentObjectAniTime);
			pMesh->AnotherTexture(1);
		}
		break;
	case 1:// texture animation
		{
			pMesh->AnotherTexture(pEffect->m_nCurrentTextureType+1);
		}
		break;
	case 2:// object + texture animation
		{
			pMesh->Tick(pEffect->m_fCurrentObjectAniTime);
			pMesh->AnotherTexture(pEffect->m_nCurrentTextureType+1);
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
	if(!pEffect->m_bZbufferEnable && m_bZBufferTemp)
	{
		D3DXVECTOR3 vTempPos;
		vTempPos.x = matScale._41;
		vTempPos.y = matScale._42;
		vTempPos.z = matScale._43;
		D3DXMATRIX matTemp;
		D3DXVECTOR3 vPos,vVel,vUp,vSide;
		vPos = g_pD3dApp->m_pCamera->GetEyePt();
		D3DXVec3Normalize(&vUp,&g_pD3dApp->m_pCamera->GetUpVec());
		D3DXVec3Normalize(&vVel,&(vTempPos - vPos));
		D3DXVec3Cross(&vSide,&vUp,&vVel);
		D3DXVec3Cross(&vUp,&vVel,&vSide);

		D3DXMatrixLookAtLH(&matTemp,&vPos,&(vPos + vVel),&vUp);
		float fDist1,fDist2;
		fDist1 = D3DXVec3Length(&(vTempPos - vPos));
		fDist2 = g_pD3dApp->m_pScene->m_pObjectRender->CheckPickMesh(matTemp,vPos).fDist;
		if(fDist1 > fDist2)
			g_pD3dDev->SetRenderState( D3DRS_ZENABLE, TRUE);
		else
			g_pD3dDev->SetRenderState( D3DRS_ZENABLE, FALSE);
	}
	// by dhkwon, 030917
	LPDIRECT3DTEXTURE9 pOrgTex = NULL;
	if(pEffect->m_pParent && pEffect->m_pParent->m_pTexture ) {
		pOrgTex = pMesh->SetTexture(pEffect->m_pParent->m_pTexture, 0); }
	else if(pEffect->m_strTextureFile[0])
	{
		int i = LoadTexture(pEffect->m_strTextureFile);
		if(m_nTextureRenderCount[i] == 2)
			m_nTextureRenderCount[i]++;
		pOrgTex = pMesh->SetTexture(m_pTexture[i], 0);
	}
	DWORD dwLightColorOp = 0;
	if(pEffect->m_bLightMapUse)
	{
		g_pD3dDev->GetTextureStageState( 1, D3DTSS_COLOROP, &dwLightColorOp );
		g_pD3dDev->SetTextureStageState( 1, D3DTSS_COLOROP, pEffect->m_nLightMapRenderState );
        g_pD3dDev->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 0 );
		// 2005-01-04 by jschoi
//		g_pD3dDev->SetTextureStageState( 1, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
//		g_pD3dDev->SetSamplerState(1,D3DSAMP_MAGFILTER,D3DTEXF_LINEAR);

		g_pD3dDev->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		g_pD3dDev->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );
//		g_pD3dDev->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
	}
	if(pEffect->m_bLightMapUse)
	{
		pMesh->Render(TRUE);
//		pMesh->Render();
	}
	else
	{
		pMesh->Render();
	}
	if((pEffect->m_pParent && pEffect->m_pParent->m_pTexture) || pEffect->m_strTextureFile[0])
	{
		pMesh->SetTexture(pOrgTex, 0);
	}

//	if(m_nObjRenderCount[index] < 3)
//		m_nObjRenderCount[index]++;
	if(pMesh && pMesh->m_nRenderCount == 2)
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
	g_pD3dDev->SetRenderState(D3DRS_SRCBLEND,dwSrc);
	g_pD3dDev->SetRenderState(D3DRS_DESTBLEND,dwDest);
	g_pD3dDev->SetTextureStageState(0,D3DTSS_COLOROP,dwColorOp);
	if(!pEffect->m_bUseEnvironmentLight)
	{
	g_pD3dDev->LightEnable( 2, FALSE );
	}
	if(pEffect->m_bLightMapUse)
	{
		g_pD3dDev->SetTextureStageState( 1, D3DTSS_COLOROP, dwLightColorOp );
	}
	if(pEffect->m_bAlphaTestEnble)
	{
		g_pD3dDev->SetRenderState( D3DRS_ALPHATESTENABLE,  FALSE );
	}
	if( pEffect->m_bZWriteEnable == FALSE)
	{
		g_pD3dDev->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
	}

	m_nObjectParticleEffectRender++;
}

#define DEBUG_COLOR		D3DCOLOR_ARGB(255,255,255,0)

void CEffectRender::RenderEffectVector(vector<EffectBackBuffer> &vecEffectBackBuffer, BOOL isZEnabled)
{
	//return g_pDrawingProc->RenderEffectVectorDP(vecEffectBackBuffer,  isZEnabled,this);

	if (vecEffectBackBuffer.empty()) return;

	/*if(isZEnabled)
	{
		sort(vecEffectBackBuffer.begin(), vecEffectBackBuffer.end(), CompareEffect());
	}*/

	//vector<EffectBackBuffer>::iterator itEffectBackBuffer = vecEffectBackBuffer.begin();
	for (auto &el: vecEffectBackBuffer)
	{
		switch (el.effectType)
		{
		case EFFECT_TYPE_OBJECT:
		{
			CObjectAni* objectAni = (CObjectAni*)el.pEffect;
			if (objectAni->m_pParent->m_nAlphaValue == SKILL_OBJECT_ALPHA_OTHER_INFLUENCE)
				break;

			ObjectAniRender(objectAni);
		}
		break;
		case EFFECT_TYPE_SPRITE:
		{
			CSpriteAni* spriteAni = (CSpriteAni*)el.pEffect;
			if (spriteAni->m_pParent->m_nAlphaValue == SKILL_OBJECT_ALPHA_OTHER_INFLUENCE)
				break;

			SpriteAniRender(spriteAni);
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
						g_pD3dDev->SetStreamSource(0, m_pVB1, 0, sizeof(SPRITE_VERTEX));
						// set light
						D3DXVECTOR3 vAxis = g_pD3dApp->m_pCamera->GetViewDir();
						m_light2 = g_pD3dApp->m_pScene->m_light2;
						m_light2.Direction = vAxis;
						g_pD3dDev->SetLight(2, &m_light2);
						g_pD3dDev->LightEnable(2, TRUE);
						ParticleRender(particle->m_pParent, particle, vAxis, -1);
						g_pD3dDev->LightEnable(2, FALSE);
						if (particle->m_pParent->m_bZWriteEnable == FALSE)
						{
							g_pD3dDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
						}
					}
					if (el.particleType == PARTICLE_OBJECT_TYPE)
					{
						ObjectParticleRender(particle->m_pObjectAni, particle);
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
						g_pD3dDev->SetStreamSource(0, m_pVB1, 0, sizeof(SPRITE_VERTEX));
						// set light
						D3DXVECTOR3 vAxis = g_pD3dApp->m_pCamera->GetViewDir();
						m_light2 = g_pD3dApp->m_pScene->m_light2;
						m_light2.Direction = vAxis;
						g_pD3dDev->SetLight(2, &m_light2);
						g_pD3dDev->LightEnable(2, TRUE);
						int nOldTextureIndex = -1;
						for (int i = 0; i < particleSystem->m_vecParticle.size(); i++)
						{
							CParticle* p = (CParticle*)particleSystem->m_vecParticle[i];
							nOldTextureIndex = ParticleRender(particleSystem, p, vAxis, nOldTextureIndex);
						}
						g_pD3dDev->LightEnable(2, FALSE);
					}

					if (el.particleType == PARTICLE_OBJECT_TYPE)
					{
						for (int i = 0; i < particleSystem->m_vecParticle.size(); i++)
						{
							ObjectParticleRender(((CParticle*)particleSystem->m_vecParticle[i])->m_pObjectAni, (CParticle*)particleSystem->m_vecParticle[i]);
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

			TraceAniRender(traceAni);
		}
		break;
		}
		}
	//	itEffectBackBuffer++;
	}

}

HRESULT CEffectRender::InitDeviceObjects()// load game data file(texture)
{
	FLOG( "CEffectRender::InitDeviceObjects()" );
	m_pTexEffectData = new CGameData;
	char strPath[MAX_PATH];

	if(g_pD3dApp->m_bTrailMod)
		strcpy(strPath, ".\\Res-Tex\\spreff2.tex");
	else
		strcpy(strPath, ".\\Res-Tex\\spreff.tex");

	m_pTexEffectData->SetFile(strPath, FALSE,NULL, 0);
	m_pEffectData = new CGameData;
	strcpy(strPath,".\\Res-Eff\\effectInfo.inf");
	m_pEffectData->SetFile(strPath, FALSE,NULL, 0);

	m_pEffectData2 = new CGameData;
	strcpy(strPath,".\\Res-Eff\\effectInfo2.inf");
	m_pEffectData2->SetFile(strPath, FALSE,NULL, 0);

	m_pObjectData = new CGameData;
	strcpy(strPath,".\\Res-Eff\\objectInfo.inf");
	m_pObjectData->SetFile(strPath, FALSE,NULL, 0);
	
	m_pObjectData2 = new CGameData;
	strcpy(strPath,".\\Res-Eff\\objectInfo2.inf");
	m_pObjectData2->SetFile(strPath, FALSE,NULL, 0);

	return S_OK;
}

HRESULT CEffectRender::RestoreDeviceObjects()// create vertex buffer
{
	FLOG( "CEffectRender::RestoreDeviceObjects()" );
	float hsx,hsy;
	int i,j;
	hsx = 0.5f;
	hsy = 0.5f;
	SPRITE_VERTEX* v;
	for(i=0;i<4;i++)
	{
		for(j=0;j<4;j++)
		{
			if( FAILED( g_pD3dDev->CreateVertexBuffer( 4*sizeof( SPRITE_VERTEX ),
				D3DUSAGE_WRITEONLY, D3DFVF_SPRITE_VERTEX, D3DPOOL_MANAGED, &m_pVB16[4*i+j] ,NULL) ) )
				return E_FAIL;
			m_pVB16[4*i+j]->Lock( 0, 0, (void**)&v, 0 );
			v[0].p = D3DXVECTOR3(-hsx,-hsy,0);	v[0].tu=0.25f*j;		v[0].tv=0.25f*(i+1);	v[0].c = 0xffffffff;
			v[1].p = D3DXVECTOR3(-hsx,hsy,0);	v[1].tu=0.25f*j;		v[1].tv=0.25f*i;		v[1].c = 0xffffffff;
			v[2].p = D3DXVECTOR3(hsx,-hsy,0);	v[2].tu=0.25f*(j+1);	v[2].tv=0.25f*(i+1);	v[2].c = 0xffffffff;
			v[3].p = D3DXVECTOR3(hsx,hsy,0);	v[3].tu=0.25f*(j+1);	v[3].tv=0.25f*i;		v[3].c = 0xffffffff;
			m_pVB16[4*i+j]->Unlock();
		}
	}
	for(i=0;i<2;i++)
	{
		for(j=0;j<4;j++)
		{
			if( FAILED( g_pD3dDev->CreateVertexBuffer( 4*sizeof( SPRITE_VERTEX ),
				D3DUSAGE_WRITEONLY, D3DFVF_SPRITE_VERTEX, D3DPOOL_MANAGED, &m_pVB8[4*i+j] ,NULL) ) )
				return E_FAIL;
			m_pVB8[2*i+j]->Lock( 0, 0, (void**)&v, 0 );
			v[0].p = D3DXVECTOR3(-hsx,-hsy,0);	v[0].tu=0.25f*j;		v[0].tv=0.5f*(i+1);		v[0].c = 0xffffffff;
			v[1].p = D3DXVECTOR3(-hsx,hsy,0);	v[1].tu=0.25f*j;		v[1].tv=0.5f*i;			v[1].c = 0xffffffff;
			v[2].p = D3DXVECTOR3(hsx,-hsy,0);	v[2].tu=0.25f*(j+1);	v[2].tv=0.5f*(i+1);		v[2].c = 0xffffffff;
			v[3].p = D3DXVECTOR3(hsx,hsy,0);	v[3].tu=0.25f*(j+1);	v[3].tv=0.5f*i;			v[3].c = 0xffffffff;
			m_pVB8[2*i+j]->Unlock();
		}
	}
	for(i=0;i<2;i++)
	{
		for(j=0;j<2;j++)
		{
			if( FAILED( g_pD3dDev->CreateVertexBuffer( 4*sizeof( SPRITE_VERTEX ),
				D3DUSAGE_WRITEONLY, D3DFVF_SPRITE_VERTEX, D3DPOOL_MANAGED, &m_pVB4[2*i+j],NULL ) ) )
				return E_FAIL;
			m_pVB4[2*i+j]->Lock( 0, 0, (void**)&v, 0 );
			v[0].p = D3DXVECTOR3(-hsx,-hsy,0);	v[0].tu=0.5f*j;			v[0].tv=0.5f*(i+1);		v[0].c = 0xffffffff;
			v[1].p = D3DXVECTOR3(-hsx,hsy,0);	v[1].tu=0.5f*j;			v[1].tv=0.5f*i;			v[1].c = 0xffffffff;
			v[2].p = D3DXVECTOR3(hsx,-hsy,0);	v[2].tu=0.5f*(j+1);		v[2].tv=0.5f*(i+1);		v[2].c = 0xffffffff;
			v[3].p = D3DXVECTOR3(hsx,hsy,0);	v[3].tu=0.5f*(j+1);		v[3].tv=0.5f*i;			v[3].c = 0xffffffff;
			m_pVB4[2*i+j]->Unlock();
		}
	}
	for(i=0;i<2;i++)
	{
		if( FAILED( g_pD3dDev->CreateVertexBuffer( 4*sizeof( SPRITE_VERTEX ),
			D3DUSAGE_WRITEONLY, D3DFVF_SPRITE_VERTEX, D3DPOOL_MANAGED, &m_pVB2[i],NULL ) ) )
			return E_FAIL;
		m_pVB2[i]->Lock( 0, 0, (void**)&v, 0 );
		v[0].p = D3DXVECTOR3(-hsx,-hsy,0);	v[0].tu=0.5f*i;		v[0].tv=1.0f;		v[0].c = 0xffffffff;
		v[1].p = D3DXVECTOR3(-hsx,hsy,0);	v[1].tu=0.5f*i;		v[1].tv=0.0f;		v[1].c = 0xffffffff;
		v[2].p = D3DXVECTOR3(hsx,-hsy,0);	v[2].tu=0.5f*(i+1);	v[2].tv=1.0f;		v[2].c = 0xffffffff;
		v[3].p = D3DXVECTOR3(hsx,hsy,0);	v[3].tu=0.5f*(i+1);	v[3].tv=0.0f;		v[3].c = 0xffffffff;
		m_pVB2[i]->Unlock();
	}
	if( FAILED( g_pD3dDev->CreateVertexBuffer( 4*sizeof( SPRITE_VERTEX ),
		D3DUSAGE_WRITEONLY, D3DFVF_SPRITE_VERTEX, D3DPOOL_MANAGED, &m_pVB1,NULL ) ) )
		return E_FAIL;
	m_pVB1->Lock( 0, 0, (void**)&v, 0 );
	v[0].p = D3DXVECTOR3(-hsx,-hsy,0);	v[0].tu=0.0f;	v[0].tv=1.0f;		v[0].c = 0xffffffff;
	v[1].p = D3DXVECTOR3(-hsx,hsy,0);	v[1].tu=0.0f;	v[1].tv=0.0f;		v[1].c = 0xffffffff;
	v[2].p = D3DXVECTOR3(hsx,-hsy,0);	v[2].tu=1.0f;	v[2].tv=1.0f;		v[2].c = 0xffffffff;
	v[3].p = D3DXVECTOR3(hsx,hsy,0);	v[3].tu=1.0f;	v[3].tv=0.0f;		v[3].c = 0xffffffff;
	m_pVB1->Unlock();
	DBGOUT("CEffectRender::RestoreDeviceObjects()\n");
	return S_OK;
}

HRESULT CEffectRender::InvalidateDeviceObjects()
{
	FLOG( "CEffectRender::InvalidateDeviceObjects()" );
	SAFE_RELEASE(m_pVB1);
	for(int i=0;i<2;i++)
		SAFE_RELEASE(m_pVB2[i]);
	for(int i=0;i<4;i++)
		SAFE_RELEASE(m_pVB4[i]);
	for(int i=0;i<8;i++)
		SAFE_RELEASE(m_pVB8[i]);
	for(int i=0;i<16;i++)
		SAFE_RELEASE(m_pVB16[i]);
	return S_OK;
}

HRESULT CEffectRender::DeleteDeviceObjects()
{
	FLOG( "CEffectRender::DeleteDeviceObjects()" );
	SAFE_DELETE(m_pTexEffectData);
	SAFE_DELETE(m_pEffectData);
	SAFE_DELETE(m_pEffectData2);
	SAFE_DELETE(m_pObjectData);
	SAFE_DELETE(m_pObjectData2);
	for(int i=0;i<TEX_EFFECT_NUM;i++)
	{
		SAFE_RELEASE(m_pTexture[i]);
	}
	m_mapTexNameToIndex.clear();
/*	m_mapObjNameToIndex.clear();
	for(i=0;i<OBJ_EFFECT_NUM;i++)
	{
		if(m_pObjEffectMesh[i])
		{
			DbgOut("m_pObjEffectMesh[%d]", i);
			m_pObjEffectMesh[i]->InvalidateDeviceObjects();
			m_pObjEffectMesh[i]->DeleteDeviceObjects();
			SAFE_DELETE(m_pObjEffectMesh[i]);
			DbgOut("Deleted\n", i);
		}
	}
*/
	map<string, CSkinnedMesh*>::iterator itMesh = m_mapObjNameToMesh.begin();
	while(itMesh != m_mapObjNameToMesh.end())
	{
		// 2004-10-12 by jschoi ż©±âĽ­ Á×Ŕ˝ LostDevice
		// 2009. 11. 23 by jskim ¸®ĽŇ˝ş ·Îµů ±¸Á¶ şŻ°ć
		itMesh->second->DeleteLoadingGameData();
		//end 2009. 11. 23 by jskim ¸®ĽŇ˝ş ·Îµů ±¸Á¶ şŻ°ć
		itMesh->second->InvalidateDeviceObjects();
		itMesh->second->DeleteDeviceObjects();
		SAFE_DELETE(itMesh->second);
		itMesh++;
	}
	m_mapObjNameToMesh.clear();

	map<string,LPDIRECT3DTEXTURE9>::iterator it = m_mapTextToTexture.begin();
	while(it != m_mapTextToTexture.end())
	{
		LPDIRECT3DTEXTURE9 pTexture = it->second;
		SAFE_RELEASE(pTexture);
		it++;
	}
	m_mapTextToTexture.clear();
	m_mapTextRenderCount.clear();
	
	// 2009. 11. 23 by jskim ¸®ĽŇ˝ş ·Îµů ±¸Á¶ şŻ°ć
	m_vecLoadingPriority.clear();
	//end 2009. 11. 23 by jskim ¸®ĽŇ˝ş ·Îµů ±¸Á¶ şŻ°ć
	return S_OK;
}


// pEffect´Â ĂÖ´ë»çŔĚÁî·Î ąöĆŰ¸¦ Ŕâ°í ŔĚ ÇÔĽöżˇ µéľî°Ł´Ů.
// ÇÔĽö°ˇ łˇł­ ČÄżˇ ł»żëŔ» »çŔĚÁî¸¸Ĺ­ şą»çÇĎ°í, ąöĆŰ´Â »čÁ¦ÇŃ´Ů.
DWORD CEffectRender::LoadEffect(char* strName, DWORD dwEffectType, char* pEffect)
{
	FLOG( "CEffectRender::LoadEffect(char* strName, DWORD dwEffectType, char* pEffect)" );
	DataHeader* pDataHeader = FindEffectInfo(strName);
	if (pDataHeader)
	{
		switch(dwEffectType)
		{
			case EFFECT_TYPE_OBJECT:
					memcpy( pEffect, pDataHeader->m_pData+sizeof(EffectFileHeader), sizeof(ObjectAniData)-sizeof(Effect));
				break;
			case EFFECT_TYPE_SPRITE:
					memcpy( pEffect, pDataHeader->m_pData+sizeof(EffectFileHeader), sizeof(SpriteAniData)-sizeof(Effect));
				break;
			case EFFECT_TYPE_PARTICLE:
					memcpy( pEffect, pDataHeader->m_pData+sizeof(EffectFileHeader), sizeof(ParticleData));
				break;
			case EFFECT_TYPE_TRACE:
					memcpy( pEffect, pDataHeader->m_pData+sizeof(EffectFileHeader), sizeof(TraceData));
				break;
		}
		dwEffectType = ((EffectFileHeader*)pDataHeader->m_pData)->dwEffectType;
	}

	//DBGOUT("[ResourceLoader] Effect Loaded (%d)(%s) => %s\n", dwEffectType, pDataHeader ? "TRUE" : "FALSE", strName);
	
	return dwEffectType;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			BOOL CEffectRender::AddFontTexture(char* strText, LPDIRECT3DTEXTURE8 pTexture )
/// \brief		ĆůĆ® ĹŘ˝şĂÄ¸¦ ADDÇŃ´Ů.
/// \author		dhkwon
/// \date		2004-03-25 ~ 2004-03-25
/// \warning	
///
/// \param		
/// \return		BOOL ( TRUE:AddĽş°ř, FALSE:ŔĚąĚ Á¸Ŕç)
///////////////////////////////////////////////////////////////////////////////
BOOL CEffectRender::AddFontTexture(char* strText, LPDIRECT3DTEXTURE9 pTexture )
{
	FLOG( "CEffectRender::AddFontTexture(char* strText, LPDIRECT3DTEXTURE9 pTexture )" );
	map<string,LPDIRECT3DTEXTURE9>::iterator it = m_mapTextToTexture.find(strText);
	if(it == m_mapTextToTexture.end())
	{
		m_mapTextToTexture[strText] = pTexture;
		m_mapTextRenderCount[strText] = 2;
		return TRUE;
	}
	else
	{
		map<string,int>::iterator it = m_mapTextRenderCount.find(strText);
		if(it != m_mapTextRenderCount.end())
		{
			(it->second) ++;
		}
		return FALSE;// ŔĚąĚ Á¸ŔçÇÔ
	}
	return FALSE;
}

// ĆůĆ®żˇĽ­ Texture¸¦ ÁöżěÁö ľĘ°í, ŔĚ ÇÔĽö¸¦ ĹëÇŘ Texture°ˇ ÁöżöÁř´Ů.
// °đ, ĆůĆ®żˇĽ­ Effect Render·Î µéľî°Ą Texture¸¦ »©¸é ±× ĆůĆ®ŔÇ Texture=NULL·Î ĽĽĆĂÇŃ´Ů.
BOOL CEffectRender::DeleteFontTexture(char* strText)
{
	FLOG( "CEffectRender::DeleteFontTexture(char* strText)" );
	map<string,LPDIRECT3DTEXTURE9>::iterator it = m_mapTextToTexture.find(strText);
	if(it != m_mapTextToTexture.end())
	{
		map<string,int>::iterator it2 = m_mapTextRenderCount.find(strText);
		if(it2 != m_mapTextRenderCount.end())
		{
			if((it2->second)>2)
			{
				it2->second --;
			}
			else
			{
				LPDIRECT3DTEXTURE9 pTexture = it->second;
				SAFE_RELEASE(pTexture);
				m_mapTextToTexture.erase(strText);
				m_mapTextRenderCount.erase(strText);
			}
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
	return FALSE;

}

///////////////////////////////////////////////////////////////////////////////
/// \fn			BOOL CEffectRender::CheckAlphaRender(CAppEffectData* pParent, DWORD dwType)
/// \brief		
/// \author		ispark
/// \date		2006-11-16 ~ 2006-11-16
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CEffectRender::CheckAlphaRender(CAppEffectData * pEffect, DWORD dwType)
{
	CCharacterInfo * pChar = pEffect->m_pCharacterInfo;

	if(dwType == _SHUTTLE)
	{
		if( ((g_pShuttleChild->m_pWeapon1_1_1 && 
			pChar == g_pShuttleChild->m_pWeapon1_1_1->m_pCharacterInfo) || 
			(g_pShuttleChild->m_pWeapon1_1_2 && 
			pChar == g_pShuttleChild->m_pWeapon1_1_2->m_pCharacterInfo) || 
			(g_pShuttleChild->m_pWeapon2_1_1 && 
			pChar == g_pShuttleChild->m_pWeapon2_1_1->m_pCharacterInfo) || 
			(g_pShuttleChild->m_pWeapon2_1_2 && 
			pChar == g_pShuttleChild->m_pWeapon2_1_2->m_pCharacterInfo) || 
			(g_pShuttleChild->m_pWeapon1_2 &&
			pChar == g_pShuttleChild->m_pWeapon1_2->m_pCharacterInfo) ||
			(g_pShuttleChild->m_pEngine &&
			pChar == g_pShuttleChild->m_pEngine->m_pCharacterInfo) ||
			(g_pShuttleChild->m_pRadar &&
			pChar == g_pShuttleChild->m_pRadar->m_pCharacterInfo) ||
			(g_pShuttleChild->m_pContainer &&
			pChar == g_pShuttleChild->m_pContainer->m_pCharacterInfo) ||
			(g_pShuttleChild->m_pAccessories &&
			pChar == g_pShuttleChild->m_pAccessories->m_pCharacterInfo) ||
			(g_pShuttleChild->m_pWingIn &&
			pChar == g_pShuttleChild->m_pWingIn->m_pCharacterInfo) ||
			// 2011. 01. 13 by jskim, ŔÎşńÁöşí »óĹÂżˇĽ­ ĆęŔĚ ş¸ŔĚ´Â ąö±× ĽöÁ¤
			(g_pShuttleChild->m_pPartner &&
			pChar == g_pShuttleChild->m_pPartner->m_pCharacterInfo) ||
			(g_pShuttleChild->m_pPartner1 &&
			pChar == g_pShuttleChild->m_pPartner1->m_pCharacterInfo)) &&
			// end 2011. 01. 13 by jskim, ŔÎşńÁöşí »óĹÂżˇĽ­ ĆęŔĚ ş¸ŔĚ´Â ąö±× ĽöÁ¤ 
			g_pShuttleChild->m_nAlphaValue < SKILL_OBJECT_ALPHA_NONE)
		{
			return TRUE;
		}
	}
	else if(dwType == _ENEMY ||
			dwType == _ADMIN)
	{
		CEnemyData * pEnemyData = (CEnemyData *)pEffect->m_pParent;
		if( ((pEnemyData->m_pWeapon1_1_1 && 
			pChar == pEnemyData->m_pWeapon1_1_1->m_pCharacterInfo) || 
			(pEnemyData->m_pWeapon1_1_2 && 
			pChar == pEnemyData->m_pWeapon1_1_2->m_pCharacterInfo) || 
			(pEnemyData->m_pWeapon2_1_1 && 
			pChar == pEnemyData->m_pWeapon2_1_1->m_pCharacterInfo) || 
			(pEnemyData->m_pWeapon2_1_2 && 
			pChar == pEnemyData->m_pWeapon2_1_2->m_pCharacterInfo) || 
			(pEnemyData->m_pWeapon1_2 &&
			pChar == pEnemyData->m_pWeapon1_2->m_pCharacterInfo) ||
			(pEnemyData->m_pEngine &&
			pChar == pEnemyData->m_pEngine->m_pCharacterInfo) ||
			(pEnemyData->m_pRadar &&
			pChar == pEnemyData->m_pRadar->m_pCharacterInfo) ||
			(pEnemyData->m_pContainer &&
			pChar == pEnemyData->m_pContainer->m_pCharacterInfo) ||
			(pEnemyData->m_pAccessories &&
			pChar == pEnemyData->m_pAccessories->m_pCharacterInfo) ||
			(pEnemyData->m_pWingIn &&
			pChar == pEnemyData->m_pWingIn->m_pCharacterInfo)) /*||
			// 2011. 01. 13 by jskim, ŔÎşńÁöşí »óĹÂżˇĽ­ ĆęŔĚ ş¸ŔĚ´Â ąö±× ĽöÁ¤
			(pEnemyData->m_pPartner && pEnemyData->m_pPartner->m_pCharacterInfo &&
			pChar == pEnemyData->m_pPartner->m_pCharacterInfo) ||
			(pEnemyData->m_pPartner1 && pEnemyData->m_pPartner1->m_pCharacterInfo &&
			pChar == pEnemyData->m_pPartner1->m_pCharacterInfo))*/ &&
			// end 2011. 01. 13 by jskim, ŔÎşńÁöşí »óĹÂżˇĽ­ ĆęŔĚ ş¸ŔĚ´Â ąö±× ĽöÁ¤			
			pEnemyData->m_nAlphaValue < SKILL_OBJECT_ALPHA_NONE)
		{
			return TRUE;
		}		
	}
	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			void CEffectRender::AddInvenPaticleName(int nInvenIdx, char* pEffectName)
/// \brief		ŔÎşĄ ŔĚĆĺĆ® Ăß°ˇ
/// \author		// 2007-11-08 by bhsohn ŔÎşĄ ŔĚĆĺĆ® °ü·Ă Ăł¸®
/// \date		2007-11-08 ~ 2007-11-08
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CEffectRender::AddInvenPaticleName(int nInvenIdx, char* pEffectName)
{
	structInvenParticleInfo struInvenParticleInfoTmp;
	memset(&struInvenParticleInfoTmp, 0x00, sizeof(structInvenParticleInfo));

	struInvenParticleInfoTmp.nWindowInvenIdx = nInvenIdx;
	strncpy(struInvenParticleInfoTmp.chEffectName, pEffectName, 32);

	m_vecInvenParticleInfo.push_back(struInvenParticleInfoTmp);
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			CEffectInfo* CEffectRender::GetEffectInfo(char* pEffectName, int nWindowInvenIdx)
/// \brief		ŔÎşĄ ŔĚĆĺĆ® Á¤ş¸ °ˇÁ®żÂ´Ů.
/// \author		// 2007-11-08 by bhsohn ŔÎşĄ ŔĚĆĺĆ® °ü·Ă Ăł¸®
/// \date		2007-11-08 ~ 2007-11-08
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
CEffectInfo* CEffectRender::GetEffectInfo(char* pEffectName, int nWindowInvenIdx)
{
	if(NULL == g_pD3dApp->m_pEffectList)
	{
		return NULL;
	}
	CAppEffectData * pEffect = (CAppEffectData *)g_pD3dApp->m_pEffectList->m_pChild;		
	while(pEffect)
	{
		CCharacterInfo* pChar = pEffect->m_pCharacterInfo;
		if(pChar)
		{
//			if( pEffect->m_pParent 
//				&& (pEffect->m_pParent->m_dwPartType == _SHUTTLE))
			{				
				CEffectInfo* pRtnParticle = GetCharInfo_To_Effect(pChar, pEffectName, nWindowInvenIdx);
				if(pRtnParticle )
				{
					return pRtnParticle;										
				}
			}			
			
		}				
		// ´ŮŔ˝Ŕ¸·Î łŃľî°¨.
		pEffect = (CAppEffectData*)pEffect->m_pNext;
	}
	return NULL;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			CEffectInfo* CEffectRender::GetCharInfo_To_Effect(CCharacterInfo* pChar, char* pEffectName,int nWindowInvenIdx)
/// \brief		ŔÎşĄ ŔĚĆĺĆ® Á¤ş¸ °ˇÁ®żÂ´Ů.
/// \author		// 2007-11-08 by bhsohn ŔÎşĄ ŔĚĆĺĆ® °ü·Ă Ăł¸®
/// \date		2007-11-08 ~ 2007-11-08
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
CEffectInfo* CEffectRender::GetCharInfo_To_Effect(CCharacterInfo* pChar, char* pEffectName,int nWindowInvenIdx)
{
	//set<BodyCond_t>::iterator itCurrent = pChar->m_vecCurrentBodyCondition.begin();
	for(auto el: pChar->m_vecCurrentBodyCondition)
	{		
		map<BodyCond_t,CBodyConditionInfo*>::iterator itBody = pChar->m_mapBodyCondition.find(el);		
		if(itBody != pChar->m_mapBodyCondition.end())
		{
			CBodyConditionInfo* pBody = itBody->second;
			// Äł¸ŻĹÍ ş¸µđ ÄÁµđĽÇŔÇ ŔĚĆĺĆ®¸¦ ĂĽĹ©
			//vector<CEffectInfo*>::iterator itEffect = pBody->m_vecEffect.begin();
			for(auto el2: pBody->m_vecEffect)
			{			
				// effect rendering
				CEffectInfo* pEffectInfo = el2;
				switch(pEffectInfo->m_nEffectType)
				{
				case EFFECT_TYPE_PARTICLE:
					{						
						if(pEffectInfo->m_pEffect 
							&& (0 == strncmp(pEffectName, pEffectInfo->m_strEffectName, strlen(pEffectName)))
							&& ((pEffectInfo->m_nInvenWeaponIndex-1) == nWindowInvenIdx))
						{
//							CParticleSystem* pEffect = (CParticleSystem*)pEffectInfo->m_pEffect;						
//							return pEffect;
							return pEffectInfo;
						}
					}
				}			
			//	itEffect++;			
			}
		}
		//itCurrent++;		
	}
	return NULL;
}

CEffectInfo* CEffectRender::GetObjEffectInfo(char* pObjName)
{
	CAppEffectData * pEffect = (CAppEffectData *)g_pD3dApp->m_pEffectList->m_pChild;
	CCharacterInfo* pChar;
	while(pEffect)
	{
		pChar = pEffect->m_pCharacterInfo;
		
		if(pChar)
		{
			//set<BodyCond_t>::iterator itCurrent = pChar->m_vecCurrentBodyCondition.begin();
			for(auto el: pChar->m_vecCurrentBodyCondition)
			{
				map<BodyCond_t,CBodyConditionInfo*>::iterator itBody = pChar->m_mapBodyCondition.find(el);
				if(itBody != pChar->m_mapBodyCondition.end())
				{
					CBodyConditionInfo* pBody = itBody->second;
					// Äł¸ŻĹÍ ş¸µđ ÄÁµđĽÇŔÇ ŔĚĆĺĆ®¸¦ ĂĽĹ©
					//vector<CEffectInfo*>::iterator itEffect = pBody->m_vecEffect.begin();
					for (auto el2 : pBody->m_vecEffect)
					{			
						// effect rendering
						CEffectInfo* pEffectInfo = el2;

						//if(0 == strncmp(pObjName, pEffectInfo->m_strEffectName, strlen(pObjName)))
						if(0 == strncmp(pObjName, pEffectInfo->m_strEffectName, 8))
						{
							return pEffectInfo;
						}								
					//	itEffect++;			
					}
				}
			//	itCurrent++;
			}

		}
		pEffect = (CAppEffectData*)pEffect->m_pNext;
	}
	return NULL;
}
