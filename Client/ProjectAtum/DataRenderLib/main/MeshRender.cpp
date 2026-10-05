// MeshRender.cpp: implementation of the CMeshRender class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AtumApplication.h"
#include "MeshRender.h"
#include "dxutil.h"
// 2008-10-15 by bhsohn 리소스 메모리 보호 기능 추가
#include "ShuttleChild.h"	
#include "INFGameMain.h"
// end 2008-10-15 by bhsohn 리소스 메모리 보호 기능 추가
// 2009. 11. 23 by jskim 리소스 로딩 구조 변경
#include "MeshInitThread.h"
#include "CharacterChild.h" 
//end 2009. 11. 23 by jskim 리소스 로딩 구조 변경
#include <fstream>
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMeshRender::CMeshRender()
{
}

CMeshRender::~CMeshRender()
{
	map<int,CSkinnedMesh*>::iterator it = m_mapSkinnedMesh.begin();
	while( it != m_mapSkinnedMesh.end()) 
	{
		SAFE_DELETE(it->second);
		it++;
	}
	m_mapSkinnedMesh.clear();
}

// 2009. 11. 23 by jskim 리소스 로딩 구조 변경
//CSkinnedMesh* CMeshRender::InitData(int nMeshIndex)
CSkinnedMesh* CMeshRender::InitData(int nMeshIndex, int nType)
// 2009. 11. 23 by jskim 리소스 로딩 구조 변경
{
	map<int,CSkinnedMesh*>::iterator it = m_mapSkinnedMesh.find(nMeshIndex);
	if( it != m_mapSkinnedMesh.end() )
	{
		return it->second;
	}

	// 2007-08-03 by bhsohn 캐릭터 오브젝트 체크썸 보냄
	// 케릭터에 대한 메쉬 로드
	INT bCharacter = g_pD3dApp->SendMeshObjectCheckSum(nMeshIndex);	
	// end 2007-08-03 by bhsohn 캐릭터 오브젝트 체크썸 보냄
	if (bCharacter == 2) {
	//	FDBG("[Warn] Skipped load for non existing object %d", nMeshIndex);
		return NULL;
	}

	char buf[16];
	char strPath[MAX_PATH];
	wsprintf(buf,"%08d.obj",nMeshIndex);

	CSkinnedMesh* pSkinnedMesh  = new CSkinnedMesh(FALSE);
	pSkinnedMesh->InitDeviceObjects();
	m_mapSkinnedMesh[nMeshIndex] = pSkinnedMesh;

	if( g_pD3dApp->m_dwGameState != _GAME || nType == _OBJECT_TYPE ||
		nMeshIndex == g_pShuttleChild->GetUnitNum() ||
		nMeshIndex == g_pCharacterChild->m_nUnitNum)
	{
		CGameData* gameData = new CGameData;
#ifdef _INET_RES_MODS_DIR
		g_pD3dApp->LoadPath(strPath, IDS_DIR_RES_MODS, buf);
		if (gameData->SetFile(strPath, FALSE, NULL, 0) == TRUE)
		{
			int nObjectIdx = 0;
			if (FALSE == bCharacter)
			{
				nObjectIdx = nMeshIndex;
			}
			pSkinnedMesh->LoadMeshHierarchyFromMem(gameData, nObjectIdx);
			SAFE_DELETE(gameData);
			return pSkinnedMesh;
		}
		else
		{
			g_pD3dApp->LoadPath(strPath, IDS_DIRECTORY_OBJECT, buf);
			if (gameData->SetFile(strPath, FALSE, NULL, 0) == TRUE)
			{
				CHARACTER myShuttleInfo = g_pShuttleChild->GetMyShuttleInfo();
				if (COMPARE_RACE(myShuttleInfo.Race, RACE_OPERATION | RACE_GAMEMASTER))
				{
					// 관리자만 스트링을 찍는다.
					wsprintf(buf, "%08d", nMeshIndex);
					if (gameData->Find(buf) == NULL)
					{
						DbgOut("[GameData] Resource File Error(%d) Path: (%s) - file inside zip have other name than zip file possibly\n", nMeshIndex, strPath);	//리소스 파일 에러
						/*if (nMeshIndex != 0)
						{
							char ErrorMsgMissionList[256];
							wsprintf(ErrorMsgMissionList, "[GameData] Resource File Error(%d) Path: (%s)", nMeshIndex, strPath);
							if (g_pGameMain)
							{
								g_pGameMain->CreateChatChild_OperationMode(ErrorMsgMissionList, COLOR_ERROR);
							}
						}					*/
					}
				}
				int nObjectIdx = 0;
				if (FALSE == bCharacter)
				{
					nObjectIdx = nMeshIndex;
				}
				pSkinnedMesh->LoadMeshHierarchyFromMem(gameData, nObjectIdx);
				SAFE_DELETE(gameData);
				return pSkinnedMesh;
			}
			else
			{
				ifstream fileInput;
				string line;
				bool bRequiredFile = false;
				fileInput.open(".\\Res-Obj\\93115501.obj");
				if (!fileInput.fail()) {
					
					char szTmpVersion[20]; *szTmpVersion = '\0';
					sprintf(szTmpVersion, "%08d", nMeshIndex);
					for (unsigned int curLine = 0; getline(fileInput, line); curLine++) {
						if (line.find(szTmpVersion) != string::npos) {
							bRequiredFile = true;
							break;
						}
					}
					fileInput.close();
				}
				else
				{
					g_pD3dApp->HackDetected(60000,0,0,0,0);
					g_pGameMain->CreateChatChild_OperationMode("[Err] Cannot open check obj file", COLOR_ERROR);
					bRequiredFile = true;
				}

				char ErrorMsgMissionList[256]; *ErrorMsgMissionList = '\0';
				wsprintf(ErrorMsgMissionList, "[GameData] Resource File missing (%d) Path: (%s) Is required: (%d)", nMeshIndex, strPath, bRequiredFile);
				
				if (bRequiredFile) {			
					if (g_pGameMain && g_pShuttleChild)
					{
						
						DBGOUT(ErrorMsgMissionList);
						//g_pGameMain->CreateChatChild_OperationMode(ErrorMsgMissionList, COLOR_ERROR);
						CHARACTER myShuttleInfo = g_pShuttleChild->GetMyShuttleInfo();
						if (!COMPARE_RACE(myShuttleInfo.Race, RACE_OPERATION | RACE_GAMEMASTER)) //kick player if missing obj file
						{
							FDBG(ErrorMsgMissionList);
							MessageBox(NULL, ErrorMsgMissionList, "Error", MB_OK | MB_ICONERROR);
							exit(0);
						}
					}
				}
				else
				{
					DBGOUT(ErrorMsgMissionList);
					//g_pGameMain->CreateChatChild_OperationMode(ErrorMsgMissionList, COLOR_ERROR);
				}
				memset(ErrorMsgMissionList,0x00,sizeof(ErrorMsgMissionList));
				SAFE_DELETE(gameData);
				return NULL;
			}
		}
#else
		g_pD3dApp->LoadPath(strPath, IDS_DIRECTORY_OBJECT, buf);
		if(gameData->SetFile(strPath,FALSE, NULL,0)==TRUE)
		{
			CHARACTER myShuttleInfo = g_pShuttleChild->GetMyShuttleInfo();		
			if(COMPARE_RACE(myShuttleInfo.Race,RACE_OPERATION|RACE_GAMEMASTER))
			{
				// 관리자만 스트링을 찍는다.
				wsprintf(buf,"%08d",nMeshIndex);
					if(gameData->Find(buf) == NULL)
				{
					DBGOUT("Resource File Error(%d)\n",nMeshIndex);	//리소스 파일 에러
					char ErrorMsgMissionList[256];
					wsprintf(ErrorMsgMissionList, "Resource File Error(%d)", nMeshIndex);
					if(g_pGameMain)
					{
						g_pGameMain->CreateChatChild_OperationMode(ErrorMsgMissionList, COLOR_ERROR);
					}				
				}
			}
			int nObjectIdx = 0;
			if(FALSE == bCharacter)
			{
				nObjectIdx = nMeshIndex;			
			}
			pSkinnedMesh->LoadMeshHierarchyFromMem( gameData, nObjectIdx);
			SAFE_DELETE( gameData );
			return pSkinnedMesh;
		}
		else
		{	
			SAFE_DELETE( gameData );
			return NULL;
		}
#endif
	}
	else
	{
		pSkinnedMesh->SetIsLoadingFlag(TRUE);
		structLoadingGameInfo* LoadingGameInfo = new structLoadingGameInfo;			
		strcpy(LoadingGameInfo->MeshName, buf);
		LoadingGameInfo->MeshType		 = nType;
		LoadingGameInfo->pSkinnedMesh	 = pSkinnedMesh;
		if(nMeshIndex == g_pCharacterChild->m_nUnitNum || nMeshIndex == g_pShuttleChild->GetUnitNum())
		{
			LoadingGameInfo->LoadingPriority = _MY_CHARACTER_PRIORITY;		
		}
		else
		{
			LoadingGameInfo->LoadingPriority = _MESH_PRIORITY;		
		}
		EnterCriticalSection(&g_pD3dApp->m_cs);
		g_pD3dApp->m_pMeshInitThread->QuePushGameData( LoadingGameInfo );
		LeaveCriticalSection(&g_pD3dApp->m_cs);
		//end 2009. 11. 23 by jskim 리소스 로딩 구조 변경
		return pSkinnedMesh;
	}
	return NULL;
}

void CMeshRender::DeleteData(int nMeshIndex)
{
	map<int,CSkinnedMesh*>::iterator it = m_mapSkinnedMesh.find(nMeshIndex);
	if( it != m_mapSkinnedMesh.end() )
	{
		it->second->InvalidateDeviceObjects();
		it->second->DeleteDeviceObjects();
		SAFE_DELETE(it->second);
		it = m_mapSkinnedMesh.erase( it );
	}
}
#ifdef  _OLD_SHADOW_SYSTEM
void CMeshRender::RenderShadow(CAtumNode * pNode)
{

}
#endif
void CMeshRender::Render()
{

}

void CMeshRender::Tick(float fElapsedTime)
{

}

HRESULT CMeshRender::InitDeviceObjects()
{
	return S_OK;
}

HRESULT CMeshRender::RestoreDeviceObjects()
{
	return S_OK;
}

HRESULT CMeshRender::InvalidateDeviceObjects()
{
	return S_OK;
}

HRESULT CMeshRender::DeleteDeviceObjects()
{
	return S_OK;
}

