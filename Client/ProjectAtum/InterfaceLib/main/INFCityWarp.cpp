// INFCityWarp.cpp: implementation of the CINFCityWarp class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "INFCityWarp.h"
#include "AtumApplication.h"
#include "INFImage.h"
#include "D3DHanFont.h"
#include "GameDataLast.h"
#include "FieldWinSocket.h"
#include "INFCityBase.h"
#include "INFScrollBar.h"
#include "Interface.h"
#include "ShuttleChild.h"
#include "CharacterChild.h"				// 2005-07-21 by ispark
#include "Cinema.h"
#include "AtumSound.h"
#include "INFGameMain.h"
#include "Chat.h"
#include "RangeTime.h"
#include "INFInven.h"
#include "StoreData.h"
#include "dxutil.h"
#include "QuestData.h"
#include "AtumDatabase.h"				// 2010-08-10 by dgwoo 버닝맵 시스템

#define CITY_WARP_START_X					CITY_BASE_NPC_BOX_START_X
#define CITY_WARP_START_Y					(CITY_BASE_NPC_BOX_START_Y-CITY_BASE_WARP_SIZE_Y)

#define CITY_WARP_BACK_START_X				(CITY_WARP_START_X+12)
#define CITY_WARP_BACK_START_Y				(CITY_WARP_START_Y+27)
#define CITY_WARP_TITLE_START_X				(CITY_WARP_START_X+9)
#define CITY_WARP_TITLE_START_Y				(CITY_WARP_START_Y+6)
#define CITY_WARP_LIST_START_X				(CITY_WARP_START_X+24)
#define CITY_WARP_LIST_START_Y				(CITY_WARP_START_Y+53)
#define CITY_WARP_CASH_START_X				(CITY_WARP_START_X+55)
#define CITY_WARP_CASH_START_Y				(CITY_WARP_START_Y+162)
#define CITY_WARP_LIST_INTERVAL				17

#define CITY_WARP_BUTTON_MOVE_START_X		(CITY_WARP_START_X+132)
#define CITY_WARP_BUTTON_MOVE_START_Y		(CITY_WARP_START_Y+161)
#define CITY_WARP_BUTTON_CANCEL_START_X		(CITY_WARP_START_X+172)
#define CITY_WARP_BUTTON_CANCEL_START_Y		(CITY_WARP_START_Y+161)
#define CITY_WARP_BUTTON_SIZE_X				38
#define CITY_WARP_BUTTON_SIZE_Y				17
#define CITY_WARP_LINE_SIZE_X				169

#define SCROLL_START_X						(CITY_WARP_START_X+202)
#define SCROLL_START_Y						(CITY_WARP_START_Y+52)
#define SCROLL_LINE_LENGTH					103

#define CITY_WARP_TEX_X						CITY_WARP_START_X + 163
#define CITY_WARP_TEX_Y						CITY_WARP_START_Y + 5
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#ifdef _INET_MAPWARP_INFO
#define _PHI									"Phillon"
#define _PAN									"Pandea"
#define _COS									"Space"
#define _ROC									"Rocks"
#define _MON									"Moon"
#define _ADV									"Outpost"
#define LINE_HEIGHT							15
#define INFO_START_X						(CITY_WARP_START_X+225)
#define INFO_START_Y						(CITY_WARP_START_Y+2)
#define	MAP_VIEW_CAP_X		(10)
#define	MAP_VIEW_CAP_Y		(-3)
BOOL bRenderMapInfo = FALSE;
POINT pCurMousePosForInfo;
WARP_TARGET_MAP_INFO_4_EXCHANGE *pWarpInfoMap;
#endif
CINFCityWarp::CINFCityWarp(CAtumNode* pParent)
{
	m_pParent = pParent;
#ifdef _INET_MAPWARP_INFO
	m_vecMapInfo.clear();
	m_pFontMonInfo = NULL;
	m_pFontMonTitle = NULL;
#endif
	m_pImgBack = NULL;
	m_pImgTitle = NULL;
	for(int i=0;i<CITY_WARP_BUTTON_NUMBER;i++)
	{
		m_pButtonMove[i] = NULL;
		m_pButtonCancel[i] = NULL;
	}
	m_pImgHightLight = NULL;
	m_bRestored = FALSE;
	memset(m_pFontWarpList, 0x00, sizeof(DWORD)*CITY_WARP_LIST_NUMBER);
	memset(m_pFontWarpPrice, 0x00, sizeof(DWORD)*CITY_WARP_LIST_NUMBER);
	m_pScroll = NULL;
	Reset();
	m_pInfluenceTex = NULL;
#ifdef _INET_MAPWARP_INFO
	for (int i = 0; i < 10; i++)
		m_pMapInfoFont[i] = NULL;
#endif
}

CINFCityWarp::~CINFCityWarp()
{
	SAFE_DELETE(m_pImgBack);
	SAFE_DELETE(m_pImgTitle);
	int i;
	for(i=0;i<CITY_WARP_BUTTON_NUMBER;i++)
	{
		SAFE_DELETE(m_pButtonMove[i]);
		SAFE_DELETE(m_pButtonCancel[i]);
	}
	SAFE_DELETE(m_pImgHightLight);

	for(i=0;i<CITY_WARP_LIST_NUMBER;i++)
	{
		SAFE_DELETE(m_pFontWarpList[i]);
		SAFE_DELETE(m_pFontWarpPrice[i]);
	}

	CVectorWarpTargetInfoIterator it = m_vecWarpTargetInfo.begin();
	while(it != m_vecWarpTargetInfo.end())
	{
		SAFE_DELETE(*it);
		it++;
	}
	m_vecWarpTargetInfo.clear();
	SAFE_DELETE(m_pScroll);
	SAFE_DELETE(m_pInfluenceTex);
#ifdef _INET_MAPWARP_INFO
	for (int i = 0; i < 10; i++)
		SAFE_DELETE(m_pMapInfoFont[i]);

	SAFE_DELETE(m_pFontMonInfo);
	SAFE_DELETE(m_pFontMonTitle);
#endif
}

void CINFCityWarp::Reset()
{
//	m_nCurrentSelectWarpIndex = -1;
	m_nButtonState[0] = BUTTON_STATE_NORMAL;
	m_nButtonState[1] = BUTTON_STATE_NORMAL;
	memset(m_szWarpList, 0x00, CITY_WARP_LIST_NUMBER*CITY_WARP_LIST_STRING_LENGTH);

//	m_nCurrentWarpListScroll = 0;
//	m_bWarpListScrollLock = FALSE;	
	m_nWarpListLineNumber = 0;	

	if(m_vecWarpTargetInfo.size()>0)
	{
		CVectorWarpTargetInfoIterator it = m_vecWarpTargetInfo.begin();
		while(it != m_vecWarpTargetInfo.end())
		{
			SAFE_DELETE(*it);
			it++;
		}
		m_vecWarpTargetInfo.clear();
	}
	m_nMapIndex = 0;	
	m_nTargetIndex = 0;	

}

void CINFCityWarp::AddWarpTargetInfoList(WARP_TARGET_MAP_INFO_4_EXCHANGE* pInfo)
{
	// 2006-09-29 by ispark, 내가 갈 수 있는 워프인지 체크
	int nQuestIndex = GetQuestIndexForWarp(pInfo->MapIndex);
	if(nQuestIndex != 0 && !g_pQuestData->IsQuestCompleted(nQuestIndex))
	{
		// 완료 안됀 퀘스트
		return;
	}
	// 2007-09-18 by dgwoo, 다른 세력의 맵은 보여지지 않는다.
	if(!IsNotInfluenceSameMap(g_pShuttleChild->m_myShuttleInfo.InfluenceType, pInfo->MapIndex))
	{
		return;
	}

	// 2010-08-10 by dgwoo 버닝맵 시스템
	BURNING_MAP_INFO* pBurning = g_pDatabase->GetPtr_BurningMapInfo(pInfo->MapIndex);
	if(pBurning)
	{
		
		if(!IS_SAME_UNITKIND(g_pShuttleChild->m_myShuttleInfo.UnitKind,pBurning->ReqUnitKind))
		{
			return;
		}
	}
	// 2010-08-10 by dgwoo 버닝맵 시스템

	WARP_TARGET_MAP_INFO_4_EXCHANGE* pNewInfo = new WARP_TARGET_MAP_INFO_4_EXCHANGE;
	memcpy(pNewInfo, pInfo, sizeof(WARP_TARGET_MAP_INFO_4_EXCHANGE));
	m_vecWarpTargetInfo.push_back(pNewInfo);
//	m_nWarpListLineNumber = m_vecWarpTargetInfo.size();
}

void CINFCityWarp::RecvWarpListDone()
{
	m_nWarpListLineNumber = m_vecWarpTargetInfo.size();
	m_pScroll->SetNumberOfData( m_nWarpListLineNumber );
//	int i =0;
//	CVectorWarpTargetInfoIterator it = m_vecWarpTargetInfo.begin();
//	while(it != m_vecWarpTargetInfo.end())
//	{
//		wsprintf(m_szWarpList[i], "%d    %s",i, (*it)->TargetName);
//		it++;
//		i++;
//	}
}

HRESULT CINFCityWarp::InitDeviceObjects()
{
	DataHeader* pDataHeader;
	pDataHeader = m_pGameData->Find("wpbk");
	m_pImgBack = new CINFImage;
	m_pImgBack->InitDeviceObjects( pDataHeader->m_pData, pDataHeader->m_DataSize );
	pDataHeader = m_pGameData->Find("wptitle");
	m_pImgTitle = new CINFImage;
	m_pImgTitle->InitDeviceObjects( pDataHeader->m_pData, pDataHeader->m_DataSize );
	int i;
	for(i=0;i<CITY_WARP_BUTTON_NUMBER;i++)
	{
		char buf[32];
		wsprintf( buf, "wpmove0%d",i);
		pDataHeader = m_pGameData->Find(buf);
		m_pButtonMove[i] = new CINFImage;
		m_pButtonMove[i]->InitDeviceObjects( pDataHeader->m_pData, pDataHeader->m_DataSize );
		wsprintf( buf, "shmcan0%d",i);
		pDataHeader = m_pGameData->Find(buf);
		m_pButtonCancel[i] = new CINFImage;
		m_pButtonCancel[i]->InitDeviceObjects( pDataHeader->m_pData, pDataHeader->m_DataSize );
	}
	pDataHeader = m_pGameData->Find("wphlgt");
	m_pImgHightLight = new CINFImage;
	m_pImgHightLight->InitDeviceObjects( pDataHeader->m_pData, pDataHeader->m_DataSize );

	for(i=0;i<CITY_WARP_LIST_NUMBER;i++)
	{
		m_pFontWarpList[i] = new CD3DHanFont(_T(g_pD3dApp->GetFontStyle()),9, D3DFONT_ZENABLE,  FALSE,256,32);
		m_pFontWarpList[i]->InitDeviceObjects(g_pD3dDev) ;
		m_pFontWarpPrice[i] = new CD3DHanFont(_T(g_pD3dApp->GetFontStyle()),9, D3DFONT_ZENABLE,  FALSE,256,32);
		m_pFontWarpPrice[i]->InitDeviceObjects(g_pD3dDev) ;
	}
	m_pScroll = new CINFScrollBar(this,
								SCROLL_START_X, 
								SCROLL_START_Y, 
								SCROLL_LINE_LENGTH,
								CITY_WARP_LIST_NUMBER);
	m_pScroll->SetGameData( m_pGameData );
	m_pScroll->InitDeviceObjects();

	m_pInfluenceTex = new CD3DHanFont(_T(g_pD3dApp->GetFontStyle()),9, D3DFONT_ZENABLE,  TRUE,256,32);
	m_pInfluenceTex->InitDeviceObjects(g_pD3dDev);

	m_pInfluenceTex->RestoreDeviceObjects();

#ifdef _INET_MAPWARP_INFO
	for (int j = 0; j < 10; j++)
	{
		m_pMapInfoFont[j] = new CD3DHanFont(_T(g_pD3dApp->GetFontStyle()), 9, D3DFONT_ZENABLE, FALSE, 1024, 32);
		m_pMapInfoFont[j]->InitDeviceObjects(g_pD3dDev);
	}
	RestoreMapView(TRUE, 0);
	{
		if (NULL == m_pFontMonInfo)
		{
			m_pFontMonInfo = new CD3DHanFont(_T(g_pD3dApp->GetFontStyle()), 9, D3DFONT_ZENABLE, TRUE, 256, 32);
		}
		m_pFontMonInfo->InitDeviceObjects(g_pD3dDev);
	}

	{
		if (NULL == m_pFontMonTitle)
		{
			m_pFontMonTitle = new CD3DHanFont(_T(g_pD3dApp->GetFontStyle()), 9, D3DFONT_ZENABLE, TRUE, 256, 32);
		}
		m_pFontMonTitle->InitDeviceObjects(g_pD3dDev);
	}
#endif
	return S_OK;
}

HRESULT CINFCityWarp::RestoreDeviceObjects()
{
	if(!m_bRestored)
	{
#ifdef _INET_MAPWARP_INFO
		for (int i = 0; i < 10; i++)
			m_pMapInfoFont[i]->RestoreDeviceObjects();

		m_pFontMonInfo->RestoreDeviceObjects();
		m_pFontMonTitle->RestoreDeviceObjects();
#endif
		m_pImgBack->RestoreDeviceObjects();
		m_pImgTitle->RestoreDeviceObjects();
		int i;
		for(i=0;i<CITY_WARP_BUTTON_NUMBER;i++)
		{
			m_pButtonMove[i]->RestoreDeviceObjects();
			m_pButtonCancel[i]->RestoreDeviceObjects();
		}
		m_pImgHightLight->RestoreDeviceObjects();
		for(i=0;i<CITY_WARP_LIST_NUMBER;i++)
		{
			m_pFontWarpList[i]->RestoreDeviceObjects() ;
			m_pFontWarpPrice[i]->RestoreDeviceObjects();
		}
		m_pScroll->RestoreDeviceObjects();
		m_pScroll->SetWheelRect(CITY_WARP_LIST_START_X, 
			CITY_WARP_LIST_START_Y,
			CITY_WARP_LIST_START_X+CITY_WARP_LINE_SIZE_X,
			CITY_WARP_LIST_START_Y+CITY_WARP_LIST_INTERVAL*CITY_WARP_LIST_NUMBER);

		m_bRestored = TRUE;
	}
	return S_OK;
}

HRESULT CINFCityWarp::InvalidateDeviceObjects()
{
	if(m_bRestored)
	{
		m_pImgBack->InvalidateDeviceObjects();
		m_pImgTitle->InvalidateDeviceObjects();
		int i;
		for(i=0;i<CITY_WARP_BUTTON_NUMBER;i++)
		{
			m_pButtonMove[i]->InvalidateDeviceObjects();
			m_pButtonCancel[i]->InvalidateDeviceObjects();
		}
		m_pImgHightLight->InvalidateDeviceObjects();
		for(i=0;i<CITY_WARP_LIST_NUMBER;i++)
		{
			m_pFontWarpList[i]->InvalidateDeviceObjects() ;
			m_pFontWarpPrice[i]->InvalidateDeviceObjects();
		}
		m_pScroll->InvalidateDeviceObjects();

		m_bRestored = FALSE;
	}
	m_pInfluenceTex->InvalidateDeviceObjects();
#ifdef _INET_MAPWARP_INFO
	for (int i = 0; i < 10; i++)
		m_pMapInfoFont[i]->InvalidateDeviceObjects();

	m_pFontMonInfo->InvalidateDeviceObjects();
	m_pFontMonTitle->InvalidateDeviceObjects();
#endif
	return S_OK;
}

HRESULT CINFCityWarp::DeleteDeviceObjects()
{
	m_pImgBack->DeleteDeviceObjects();
	SAFE_DELETE(m_pImgBack);
	m_pImgTitle->DeleteDeviceObjects();
	SAFE_DELETE(m_pImgTitle);
	int i;
	for(i=0;i<CITY_WARP_BUTTON_NUMBER;i++)
	{
		m_pButtonMove[i]->DeleteDeviceObjects();
		m_pButtonCancel[i]->DeleteDeviceObjects();
		SAFE_DELETE(m_pButtonMove[i]);
		SAFE_DELETE(m_pButtonCancel[i]);
	}
	m_pImgHightLight->DeleteDeviceObjects();
	SAFE_DELETE(m_pImgHightLight);
	for(i=0;i<CITY_WARP_LIST_NUMBER;i++)
	{
		m_pFontWarpList[i]->DeleteDeviceObjects();
		SAFE_DELETE(m_pFontWarpList[i]);
		m_pFontWarpPrice[i]->DeleteDeviceObjects();
		SAFE_DELETE(m_pFontWarpPrice[i]);
	}
	m_pScroll->DeleteDeviceObjects();
	SAFE_DELETE(m_pScroll);

	m_pInfluenceTex->DeleteDeviceObjects();
	SAFE_DELETE(m_pInfluenceTex);

#ifdef _INET_MAPWARP_INFO
	for (i = 0; i < 10; i++)
	{
		if (m_pMapInfoFont[i])
		{
			m_pMapInfoFont[i]->DeleteDeviceObjects();
			SAFE_DELETE(m_pMapInfoFont[i]);
		}
	}
	{
		if (m_pFontMonInfo)
		{
			m_pFontMonInfo->DeleteDeviceObjects();
			SAFE_DELETE(m_pFontMonInfo);
		}
	}
	{
		if (m_pFontMonTitle)
		{
			m_pFontMonTitle->DeleteDeviceObjects();
			SAFE_DELETE(m_pFontMonTitle);
		}
	}
#endif
	return S_OK;
}
#ifdef _INET_MAPWARP_INFO

BOOL CINFCityWarp::IsBelligerenceMonster(BYTE monsterBelligerence)
{
	if (IS_SAME_CHARACTER_MONSTER_INFLUENCE(INFLUENCE_TYPE_VCN, monsterBelligerence))
	{
		return TRUE;
	}
	else if (IS_SAME_CHARACTER_MONSTER_INFLUENCE(INFLUENCE_TYPE_ANI, monsterBelligerence))
	{
		return TRUE;
	}
	return FALSE;
}
void CINFCityWarp::RestoreMapView(BOOL bAll, MapIndex_t	MapIndex)
{
	vector<stMapViewInfo1*>::iterator it = m_vecMapInfo.begin();
	while (it != m_vecMapInfo.end())
	{
		stMapViewInfo1* pMapViewInfo = (*it);
		if (pMapViewInfo->pInfImage)
		{
			if (bAll)
			{
				pMapViewInfo->pInfImage->RestoreDeviceObjects();
			}
			else if (pMapViewInfo->MapIndex == MapIndex)
			{
				pMapViewInfo->pInfImage->RestoreDeviceObjects();
			}
		}
		it++;
	}
}
HRESULT CINFCityWarp::GetMapIndex_To_Monster(MapIndex_t	MapIndex, vector<MEX_MONSTER_INFO> *o_vecQuestInfo)
{
	MEX_MONSTER_INFO monsterInfo;
	DataHeader*  pHeader;
	CGameData * pData = new CGameData;
	char strPath[256];
	int nCont = 0;
	memset(&monsterInfo, 0x00, sizeof(MEX_MONSTER_INFO));
	o_vecQuestInfo->clear();
	wsprintf(strPath, ".\\Res-Map\\omd.tex");

	if (pData->SetFile(strPath, FALSE, NULL, 0))
	{
		char* p;
		wsprintf(strPath, "%04d", MapIndex);
		pHeader = pData->Find(strPath);
		if (pHeader)
		{
			p = pHeader->m_pData;
			p += 20;
			memcpy(&nCont, p, sizeof(int));
			p += sizeof(int);

			p += nCont*sizeof(int);
			memcpy(&nCont, p, sizeof(int));
			p += sizeof(int);

			for (int i = 0; i<nCont; i++)
			{
				int nMonType;
				memcpy(&nMonType, p, sizeof(int));
				MEX_MONSTER_INFO * pMonster = g_pGameMain->CheckMonsterInfo(nMonType);

				if (pMonster)
				{
					memcpy(&monsterInfo, pMonster, sizeof(MEX_MONSTER_INFO));

					// 몬스터 이름이 없거나 세력 몬스터면 추가하지않는다.
					if (strlen(monsterInfo.MonsterName) > 0
						&& (FALSE == IsBelligerenceMonster(monsterInfo.Belligerence)))
					{
						// 실질적인 ADD
						o_vecQuestInfo->push_back(monsterInfo);
					}
				}

				p += sizeof(int);
			}
			SAFE_DELETE(pData);
		}
		else
		{
			SAFE_DELETE(pData);
			return E_FAIL;
		}
	}
	else
	{
		SAFE_DELETE(pData);
		return E_FAIL;
	}
	return S_OK;
}
int CINFCityWarp::AddMapInfo(MapIndex_t MapIndex)
{
	int nItemCnt = 0;
	MAP_INFO* pMapInfo = g_pGameMain->GetMapInfo(MapIndex);
	if (NULL == pMapInfo)
	{
		return nItemCnt;
	}

	stMapViewInfo1* pMapViewInfo = new stMapViewInfo1;
	CGameData MiniMapData;

	char buf[256];

	wsprintf(buf, ".\\Res-Map\\%04d.tex", pMapInfo->Tex);
	MiniMapData.SetFile(buf, FALSE, NULL, 0, FALSE);

	wsprintf(buf, "%04d", pMapInfo->Tex);
	pMapViewInfo->pHeader = MiniMapData.FindFromFile(buf);

	if (pMapViewInfo->pHeader)
	{
		
		pMapViewInfo->pInfImage = new CINFImage;
		pMapViewInfo->pInfImage->InitDeviceObjects(pMapViewInfo->pHeader->m_pData,
			pMapViewInfo->pHeader->m_DataSize);
	}
	pMapViewInfo->MapIndex = MapIndex;
	
	pMapViewInfo->vecMonsterInfo.clear();
	GetMapIndex_To_Monster(MapIndex, &pMapViewInfo->vecMonsterInfo);
	nItemCnt = pMapViewInfo->vecMonsterInfo.size();

	memset(pMapViewInfo->chMapName, 0x00, 256);
	strncpy(pMapViewInfo->chMapName, pMapInfo->MapName, strlen(pMapInfo->MapName) + 1);

	m_vecMapInfo.push_back(pMapViewInfo);

	return nItemCnt;

}
stMapViewInfo1* CINFCityWarp::GetMapIdx_To_MapViewInfo(MapIndex_t	selMapIndex)
{
	vector<stMapViewInfo1*>::iterator it = m_vecMapInfo.begin();
	while (it != m_vecMapInfo.end())
	{
		stMapViewInfo1* pMapViewInfo = (*it);
		if (selMapIndex == pMapViewInfo->MapIndex)
		{
			return (*it);
		}
		it++;
	}
	return NULL;
}
void CINFCityWarp::RenderMonsterInfo(vector<MEX_MONSTER_INFO>	*i_vecMonsterInfo,float renderX, float renderY)
{
	int nMonCnt = 0;
	vector<MEX_MONSTER_INFO>::iterator it = i_vecMonsterInfo->begin();
	char chBuf[256], chAttack[256];
	memset(chBuf, 0x00, 256);
	memset(chAttack, 0x00, 256);

	int nCnt = 0;
	for (nCnt = 0; nCnt < (MONSTER_INFO_ITEM_LEN); nCnt++)
	{
		if (it == i_vecMonsterInfo->end())
		{
			break;
		}
		it++;
	}
	int nLineSpacing = renderY;
	while (it != i_vecMonsterInfo->end())
	{
		if (nMonCnt >= MAX_MONSTER_INFO)
		{
			break;
		}
		float fPoxX = m_ptMonPos[nMonCnt].x;
		float fPoxY = m_ptMonPos[nMonCnt].y;

		fPoxX += MAP_VIEW_CAP_X;
		fPoxY += MAP_VIEW_CAP_Y;
		MEX_MONSTER_INFO monsterInfo = (*it);
		wsprintf(chBuf, "\\a%s(Lv%d)\\a", monsterInfo.MonsterName, monsterInfo.Level);

		m_pFontMonInfo->DrawText(renderX,
			nLineSpacing,
			GUI_FONT_COLOR_W,
			chBuf, 0L);
		nLineSpacing += LINE_HEIGHT;
		it++;
		nMonCnt++;
	}

}

char *CINFCityWarp::GetMapChainName(int MapIndex)
{
	MapChains arrMapChainList[50]
	{
		{ 2004, _ADV },
		{ 2005, _ADV },
		{ 2006, _ADV },
		{ 2007, _ADV },	
		{ 3015, _PAN },
		{ 3019, _ROC },
		{ 3020, _ROC },
		{ 3022, _PAN },
		{ 3023, _PAN },
		{ 3025, _PAN },
		{ 3028, _PAN },
		{ 3029, _PAN },
		{ 3031, _PAN },
		{ 3045, _PAN },
		{ 4007, _COS },
		{ 4010, _PAN },
		{ 6001, _COS },
		{ 6002, _COS },
		{ 6003, _COS },
		{ 6004, _COS },
		{ 7001, _MON },
		{ 7002, _MON },
		{ 7003, _MON },
		{ 7004, _MON },
		{ 7005, _MON },
	};
	for (int i = 0; i < sizeof(arrMapChainList); i++)
	{
		if (MapIndex == arrMapChainList[i].MapIndex)
			return arrMapChainList[i].Chain;
	}
	return "Phillon";
}
#endif
void CINFCityWarp::Render()
{
#ifdef _INET_MAPWARP_INFO
	if (bRenderMapInfo)
	{
		
		char buffer[256][1024];
		wsprintf(buffer[0], "\\g[MAP INFO]\\g");
		if (pWarpInfoMap)
		{
			wsprintf(buffer[1], "1. \\eMapName: %s \\cat %s\\c", pWarpInfoMap->TargetName, GetMapChainName(pWarpInfoMap->MapIndex));
			
			wsprintf(buffer[2], "2. \\eWarp cost: %d\\e", pWarpInfoMap->Fee);
		}
		wsprintf(buffer[3], "   ________________________   ");
		wsprintf(buffer[4], "Monsters at map (for missions):");
		wsprintf(buffer[5], " ");
		wsprintf(buffer[6], " ");
		wsprintf(buffer[7], " ");
		wsprintf(buffer[8], " ");
		wsprintf(buffer[9], " ");
		
		int leng = 470.0f;
		int height = 12.0f;
		//float fScaleY = 9.0f;//height*6; //height * line count
		//g_pGameMain->m_pImgTextPopUp[0]->Move(INFO_START_X - 7 + 15, INFO_START_Y - 2);
		//g_pGameMain->m_pImgTextPopUp[0]->SetScale(1.0f, 1.0f);
		//g_pGameMain->m_pImgTextPopUp[0]->Render();
		g_pGameMain->m_pImgTextPopUp[1]->Move(INFO_START_X + 0.1f + LINE_HEIGHT, INFO_START_Y - 2);
		g_pGameMain->m_pImgTextPopUp[1]->SetScale(leng, height);
		g_pGameMain->m_pImgTextPopUp[1]->Render();
		//g_pGameMain->m_pImgTextPopUp[2]->Move(INFO_START_X + leng + 15, INFO_START_Y - 2);
		//g_pGameMain->m_pImgTextPopUp[2]->SetScale(1.0f, 1.0f);
		//g_pGameMain->m_pImgTextPopUp[2]->Render();
		//////////////////// init Scale to 1 for the other use ingame
		//g_pGameMain->m_pImgTextPopUp[0]->SetScale(1.0f, 1.0f);
		g_pGameMain->m_pImgTextPopUp[1]->SetScale(1.0f, 1.0f);
		//g_pGameMain->m_pImgTextPopUp[2]->SetScale(1.0f, 1.0f);
		int lineheightincrement = 0;
		int nNextLine = 0;
		for (int i = 0; i < 10; i++)
		{

			m_pMapInfoFont[i]->DrawText(INFO_START_X + 23, INFO_START_Y + lineheightincrement, GUI_FONT_COLOR_W, buffer[i]);
			lineheightincrement += LINE_HEIGHT;
			nNextLine = i;
		}

		stMapViewInfo1* pMapViewInfo = GetMapIdx_To_MapViewInfo(pWarpInfoMap->MapIndex);

		if (pMapViewInfo)
		{
			if (pMapViewInfo->pInfImage)
			{
				pMapViewInfo->pInfImage->SetScale(0.35f, 0.35f);
				pMapViewInfo->pInfImage->Move(INFO_START_X + 270, INFO_START_Y+10);
				pMapViewInfo->pInfImage->Render();
			}
			else
			{
				wsprintf(buffer[nNextLine], "\\cNo map preview available.\\c");
				m_pMapInfoFont[nNextLine]->DrawText(INFO_START_X + 230, INFO_START_Y + (lineheightincrement/3), GUI_FONT_COLOR_W, buffer[nNextLine]);
				lineheightincrement += 15;
			}
			RenderMonsterInfo(&pMapViewInfo->vecMonsterInfo, INFO_START_X + 25, INFO_START_Y+75);
		}
		/*MapDrops arrMapUniqeDrops[1024]
		{
			{ 3004, "A-X", "Boss" },
			{ 3004, "B-X", "Boss" },
			{ 3004, "M-X", "Boss" },
			{ 3004, "I-X", "Boss" },
			{ 3004, "Soul of Laplace", "Gold/Boss" },
			{ 3004, "Methamorphose mixture", "Gold/Boss" },
			{ 3004, "Gematria scripture", "Gold/Boss" },
			{ 3004, "Seraphim Bible", "Gold/Boss" },
			{ 3004, "Finish move skill opening card", "Gold/Boss" },
			{ 3004, "Special skill opening card", "Gold/Boss" },
			{ 3005, "A-X", "Boss" },
			{ 3005, "B-X", "Boss" },
			{ 3005, "M-X", "Boss" },
			{ 3005, "I-X", "Boss" },
			{ 3005, "Soul of Laplace", "Gold/Boss" },
			{ 3005, "Methamorphose mixture", "Gold/Boss" },
			{ 3005, "Gematria scripture", "Gold/Boss" },
			{ 3005, "Seraphim Bible", "Gold/Boss" },
			{ 3005, "Finish move skill opening card", "Gold/Boss" },
			{ 3005, "Special skill opening card", "Gold/Boss" },
			{ 3006, "A-X", "Boss" },
			{ 3006, "B-X", "Boss" },
			{ 3006, "M-X", "Boss" },
			{ 3006, "I-X", "Boss" },
			{ 3006, "Soul of Laplace", "Gold/Boss" },
			{ 3006, "Methamorphose mixture", "Gold/Boss" },
			{ 3006, "Gematria scripture", "Gold/Boss" },
			{ 3006, "Seraphim Bible", "Gold/Boss" },
			{ 3006, "Finish move skill opening card", "Gold/Boss" },
			{ 3006, "Special skill opening card", "Gold/Boss" },
			{ 3063, "Soul of Laplace", "Gold" },
			{ 3063, "Methamorphose mixture", "Gold" },
			{ 3063, "Gematria scripture", "Gold" },
			{ 3063, "Seraphim Bible", "Gold" },
			{ 3063, "Finish move skill opening card", "Gold" },
			{ 3063, "Special skill opening card", "Gold" },
			{ 3064, "Hammer", "Boss" },
			{ 3064, "Ace arrow", "Boss" },
			{ 3064, "Speed thunder", "Boss" },
			{ 3064, "Power hunter", "Boss" },
			{ 3064, "Soul of Laplace", "Gold/Boss" },
			{ 3064, "Methamorphose mixture", "Gold/Boss" },
			{ 3064, "Gematria scripture", "Gold/Boss" },
			{ 3064, "Seraphim Bible", "Gold/Boss" },
			{ 3064, "Finish move skill opening card", "Gold/Boss" },
			{ 3064, "Special skill opening card", "Gold/Boss" },
			{ 3067, "Hammer", "Boss" },
			{ 3067, "Ace arrow", "Boss" },
			{ 3067, "Speed thunder", "Boss" },
			{ 3067, "Power hunter", "Boss" },
			{ 3067, "Soul of Laplace", "Boss" },
			{ 3067, "Methamorphose mixture", "Boss" },
			{ 3067, "Gematria scripture", "Boss" },
			{ 3067, "Seraphim Bible", "Boss" },
			{ 3067, "Finish move skill opening card", "Boss" },
			{ 3067, "Special skill opening card", "Boss" },
			{ 3029, "Soul of Laplace", "Gold" },
			{ 3029, "Methamorphose mixture", "Gold" },
			{ 3029, "Gematria scripture", "Gold" },
			{ 3029, "Seraphim Bible", "Gold" },
			{ 3029, "Finish move skill opening card", "Gold" },
			{ 3029, "Special skill opening card", "Gold" },
			{ 4010, "Soul of Laplace", "Gold" },
			{ 4010, "Methamorphose mixture", "Gold" },
			{ 4010, "Gematria scripture", "Gold" },
			{ 4010, "Seraphim Bible", "Gold" },
			{ 4010, "Finish move skill opening card", "Gold" },
			{ 4010, "Special skill opening card", "Gold" },
			{ 3025, "Hammer", "Boss" },
			{ 3025, "Ace arrow", "Boss" },
			{ 3025, "Speed thunder", "Boss" },
			{ 3025, "Power hunter", "Boss" },
			{ 3025, "Soul of Laplace", "Gold/Boss" },
			{ 3025, "Methamorphose mixture", "Gold/Boss" },
			{ 3025, "Gematria scripture", "Gold/Boss" },
			{ 3025, "Seraphim Bible", "Gold/Boss" },
			{ 3025, "Finish move skill opening card", "Gold/Boss" },
			{ 3025, "Special skill opening card", "Gold/Boss" },
			{ 3028, "Hammer", "Boss" },
			{ 3028, "Ace arrow", "Boss" },
			{ 3028, "Speed thunder", "Boss" },
			{ 3028, "Power hunter", "Boss" },
			{ 3028, "Soul of Laplace", "Gold/Boss" },
			{ 3028, "Methamorphose mixture", "Gold/Boss" },
			{ 3028, "Gematria scripture", "Gold/Boss" },
			{ 3028, "Seraphim Bible", "Gold/Boss" },
			{ 3028, "Finish move skill opening card", "Gold/Boss" },
			{ 3028, "Special skill opening card", "Gold/Boss" },
			{ 7001, "Soul of Laplace", "Gold" },
			{ 7001, "Methamorphose mixture", "Gold" },
			{ 7001, "Gematria scripture", "Gold" },
			{ 7001, "Seraphim Bible", "Gold" },
			{ 7001, "Finish move skill opening card", "Gold" },
			{ 7001, "Special skill opening card", "Gold" },
			{ 3013, "Hammer", "Boss" },
			{ 3013, "Ace arrow", "Boss" },
			{ 3013, "Speed thunder", "Boss" },
			{ 3013, "Power hunter", "Boss" },
			{ 3013, "Soul of Laplace", "Boss" },
			{ 3013, "Methamorphose mixture", "Boss" },
			{ 3013, "Gematria scripture", "Boss" },
			{ 3013, "Seraphim Bible", "Boss" },
			{ 3013, "Soul of Laplace", "Gold" },
			{ 3013, "Methamorphose mixture", "Gold" },
			{ 3013, "Gematria scripture", "Gold" },
			{ 3013, "Seraphim Bible", "Gold" },
			{ 3013, "Finish move skill opening card", "Gold" },
			{ 3013, "Special skill opening card", "Gold" },
			{ 3016, "Hammer", "Boss" },
			{ 3016, "Ace arrow", "Boss" },
			{ 3016, "Speed thunder", "Boss" },
			{ 3016, "Power hunter", "Boss" },
			{ 3016, "Soul of Laplace", "Boss" },
			{ 3016, "Methamorphose mixture", "Boss" },
			{ 3016, "Gematria scripture", "Boss" },
			{ 3016, "Seraphim Bible", "Boss" },
			{ 3016, "Finish move skill opening card", "Boss" },
			{ 3016, "Special skill opening card", "Boss" },
			{ 3009, "Hammer", "Boss" },
			{ 3009, "Ace arrow", "Boss" },
			{ 3009, "Speed thunder", "Boss" },
			{ 3009, "Power hunter", "Boss" },
			{ 3009, "Soul of Laplace", "Boss" },
			{ 3009, "Methamorphose mixture", "Boss" },
			{ 3009, "Gematria scripture", "Boss" },
			{ 3009, "Seraphim Bible", "Boss" },
			{ 3009, "Finish move skill opening card", "Boss" },
			{ 3009, "Special skill opening card", "Boss" },
			{ 3007, "Hammer", "Boss" },
			{ 3007, "Ace arrow", "Boss" },
			{ 3007, "Speed thunder", "Boss" },
			{ 3007, "Power hunter", "Boss" },
			{ 3007, "Soul of Laplace", "Boss" },
			{ 3007, "Methamorphose mixture", "Boss" },
			{ 3007, "Gematria scripture", "Boss" },
			{ 3007, "Seraphim Bible", "Boss" },
			{ 3007, "Finish move skill opening card", "Boss" },
			{ 3007, "Special skill opening card", "Boss" },
			{ 3031, "Soul of Laplace", "Gold" },
			{ 3031, "Methamorphose mixture", "Gold" },
			{ 3031, "Gematria scripture", "Gold" },
			{ 3031, "Seraphim Bible", "Gold" },
			{ 3031, "Finish move skill opening card", "Gold" },
			{ 3031, "Special skill opening card", "Gold" },
			{ 3015, "Soul of Laplace", "Gold" },
			{ 3015, "Methamorphose mixture", "Gold" },
			{ 3015, "Gematria scripture", "Gold" },
			{ 3015, "Seraphim Bible", "Gold" },
			{ 3015, "Finish move skill opening card", "Gold" },
			{ 3015, "Special skill opening card", "Gold" },
			{ 3045, "Hammer", "Boss" },
			{ 3045, "Ace arrow", "Boss" },
			{ 3045, "Speed thunder", "Boss" },
			{ 3045, "Power hunter", "Boss" },
			{ 3045, "Soul of Laplace", "Boss" },
			{ 3045, "Methamorphose mixture", "Boss" },
			{ 3045, "Gematria scripture", "Boss" },
			{ 3045, "Seraphim Bible", "Boss" },
			{ 3045, "Finish move skill opening card", "Boss" },
			{ 3045, "Special skill opening card", "Boss" },
			{ 3022, "Hammer", "Boss" },
			{ 3022, "Ace arrow", "Boss" },
			{ 3022, "Speed thunder", "Boss" },
			{ 3022, "Power hunter", "Boss" },
			{ 3022, "Soul of Laplace", "Boss" },
			{ 3022, "Methamorphose mixture", "Boss" },
			{ 3022, "Gematria scripture", "Boss" },
			{ 3022, "Seraphim Bible", "Boss" },
			{ 3022, "Finish move skill opening card", "Boss" },
			{ 3022, "Special skill opening card", "Boss" },
			{ 7002, "Soul of Laplace", "Gold" },
			{ 7002, "Methamorphose mixture", "Gold" },
			{ 7002, "Gematria scripture", "Gold" },
			{ 7002, "Seraphim Bible", "Gold" },
			{ 7002, "Finish move skill opening card", "Gold" },
			{ 7002, "Special skill opening card", "Gold" }
		};*/
		/*lineheightincrement = 0;
		wsprintf(buffer[nNextLine], "\\cUnique drop list:\\c");
		m_pMapInfoFont[nNextLine]->DrawText(INFO_START_X + 250, INFO_START_Y + (lineheightincrement), GUI_FONT_COLOR_W, buffer[nNextLine]);
		lineheightincrement = 15; //new column
		int num = 1;*/
	/*	for (int i = 0; i < sizeof(arrMapUniqeDrops); i++)
		{
			if (pWarpInfoMap->MapIndex == arrMapUniqeDrops[i].MapIndex)
			{
				wsprintf(buffer[nNextLine], "\\c%d. %s from \\r%s\\c", num,arrMapUniqeDrops[i].ItemName, arrMapUniqeDrops[i].MonsterType);
				m_pMapInfoFont[nNextLine]->DrawText(INFO_START_X + 250, INFO_START_Y + (lineheightincrement), GUI_FONT_COLOR_W, buffer[nNextLine]);
				lineheightincrement += 15;
				num++;
			}
		}*/
	}
#endif
	m_pImgBack->Move(CITY_WARP_BACK_START_X,CITY_WARP_BACK_START_Y);
	m_pImgBack->Render();
	m_pImgTitle->Move(CITY_WARP_TITLE_START_X,CITY_WARP_TITLE_START_Y);
	m_pImgTitle->Render();
	if(m_nButtonState[CITY_WARP_BUTTON_MOVE] != BUTTON_STATE_NORMAL)
	{
		m_pButtonMove[m_nButtonState[CITY_WARP_BUTTON_MOVE]]->Move(CITY_WARP_BUTTON_MOVE_START_X, CITY_WARP_BUTTON_MOVE_START_Y);
		m_pButtonMove[m_nButtonState[CITY_WARP_BUTTON_MOVE]]->Render();
	}
	if(m_nButtonState[CITY_WARP_BUTTON_CANCEL] != BUTTON_STATE_NORMAL)
	{
		m_pButtonCancel[m_nButtonState[CITY_WARP_BUTTON_CANCEL]]->Move(CITY_WARP_BUTTON_CANCEL_START_X, CITY_WARP_BUTTON_CANCEL_START_Y);
		m_pButtonCancel[m_nButtonState[CITY_WARP_BUTTON_CANCEL]]->Render();
	}
//	if(m_nCurrentSelectWarpIndex != -1)
//	{
//		m_pImgHightLight->Move(CITY_WARP_LIST_START_X+1,
//			CITY_WARP_LIST_START_Y+CITY_WARP_LIST_INTERVAL*m_nCurrentSelectWarpIndex+1);
//		m_pImgHightLight->Render();
//	}
	if(m_pScroll->GetCurrentSelectWindowIndex() >= 0 &&
		m_pScroll->GetCurrentSelectWindowIndex() < CITY_WARP_LIST_NUMBER)
	{
		m_pImgHightLight->Move(CITY_WARP_LIST_START_X+1,
			CITY_WARP_LIST_START_Y+CITY_WARP_LIST_INTERVAL*m_pScroll->GetCurrentSelectWindowIndex()+1);
		m_pImgHightLight->Render();
	}
//	for(int i=0;i<CITY_WARP_LIST_NUMBER;i++)
//	{
//		if(m_szWarpList[i])
//		{
//			m_pFontWarpList[i]->DrawText(CITY_WARP_LIST_START_X+1, 
//				CITY_WARP_LIST_START_Y+CITY_WARP_LIST_INTERVAL*i+1,
//				m_pScroll->GetCurrentSelectWindowIndex == i ? GUI_SELECT_FONT_COLOR : GUI_FONT_COLOR,
//				m_szWarpList[i],0L);
//		}
//		else 
//		{
//			break;
//		}
//	}
	for(int i=0;i<CITY_WARP_LIST_NUMBER;i++)
	{
		int index = m_pScroll->GetCurrentScrollIndex()+i;
		if(index >= m_vecWarpTargetInfo.size())
		{
			break;
		}
//#if defined(LANGUAGE_ENGLISH) || defined(LANGUAGE_VIETNAM)
		char chWarpPrice[30] = {0, };
		int len = 0;
//		wsprintf(m_szWarpList[i], "%16s%10d", m_vecWarpTargetInfo[index]->TargetName, m_vecWarpTargetInfo[index]->Fee);
		wsprintf(m_szWarpList[i], "%s", m_vecWarpTargetInfo[index]->TargetName);
		int nWarpPrice = (CAtumSJ::GetCityWarTex(m_vecWarpTargetInfo[index]->Fee, m_fTexRate) + m_vecWarpTargetInfo[index]->Fee);
		// 2006-04-21 by ispark, 지도자는 공짜
		if(COMPARE_RACE(g_pShuttleChild->m_myShuttleInfo.Race,RACE_INFLUENCE_LEADER))
		{
			nWarpPrice = 0;
		// 2007-10-10 by dgwoo 멤버쉽 유저는 워프비 절감
		}else if(0 != g_pD3dApp->GetPrimiumCardInfo()->nCardItemNum1)
		{
			nWarpPrice = nWarpPrice - (nWarpPrice / (100/MEMBERSHIP_DISCOUNT_WARPFEE));
		}
		wsprintf(chWarpPrice, "%d", nWarpPrice);
		len = strlen(chWarpPrice);
//#else
//		int nWarpPrice = (CAtumSJ::GetCityWarTex(m_vecWarpTargetInfo[index]->Fee, m_fTexRate) + m_vecWarpTargetInfo[index]->Fee);
//		// 2006-04-21 by ispark, 지도자는 공짜
//		if(COMPARE_RACE(g_pShuttleChild->m_myShuttleInfo.Race,RACE_INFLUENCE_LEADER))
//		{
//			nWarpPrice = 0;
//		}
//		wsprintf(m_szWarpList[i], "%16s%10d", m_vecWarpTargetInfo[index]->TargetName, nWarpPrice);
//#endif
		if(m_szWarpList[i])
		{
//#if defined(LANGUAGE_ENGLISH) || defined(LANGUAGE_VIETNAM)
			m_pFontWarpList[i]->DrawText(CITY_WARP_LIST_START_X+1, 
				CITY_WARP_LIST_START_Y+CITY_WARP_LIST_INTERVAL*i-1,
				m_pScroll->GetCurrentSelectWindowIndex() == i ? GUI_SELECT_FONT_COLOR : GUI_FONT_COLOR,
				m_szWarpList[i],0L);
			m_pFontWarpPrice[i]->DrawText(CITY_WARP_LIST_START_X+165 - len*7, 
				CITY_WARP_LIST_START_Y+CITY_WARP_LIST_INTERVAL*i-1,
				m_pScroll->GetCurrentSelectWindowIndex() == i ? GUI_SELECT_FONT_COLOR : GUI_FONT_COLOR,
				chWarpPrice,0L);
//#else
//			m_pFontWarpList[i]->DrawText(CITY_WARP_LIST_START_X+1, 
//				CITY_WARP_LIST_START_Y+CITY_WARP_LIST_INTERVAL*i+1,
//				m_pScroll->GetCurrentSelectWindowIndex() == i ? GUI_SELECT_FONT_COLOR : GUI_FONT_COLOR,
//				m_szWarpList[i],0L);
//#endif
		}
		else 
		{
			break;
		}
	}
	m_pScroll->Render();

	// 2006-02-08 by ispark, 세력 세금
	char chTexbuf[30] = {0,};
	sprintf(chTexbuf, STRMSG_C_060208_0000, m_fTexRate);
	m_pInfluenceTex->DrawText(CITY_WARP_TEX_X, CITY_WARP_TEX_Y, GUI_FONT_COLOR, chTexbuf, 0L);
}

void CINFCityWarp::Tick()
{

}

int CINFCityWarp::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(m_pScroll)
	{
		if(m_pScroll->WndProc(uMsg, wParam, lParam) == INF_MSGPROC_BREAK)
		{
			return INF_MSGPROC_BREAK;
		}
	}
	switch(uMsg)
	{
	case WM_MOUSEMOVE:
		{

			POINT pt;
			pt.x = LOWORD(lParam);
			pt.y = HIWORD(lParam);
			CheckMouseReverse(&pt);
			if(pt.y > CITY_WARP_BUTTON_MOVE_START_Y && 
				pt.y < CITY_WARP_BUTTON_MOVE_START_Y + CITY_WARP_BUTTON_SIZE_Y)
			{
				if( pt.x > CITY_WARP_BUTTON_MOVE_START_X && 
					pt.x < CITY_WARP_BUTTON_MOVE_START_X+CITY_WARP_BUTTON_SIZE_X)
				{
					if(m_nButtonState[CITY_WARP_BUTTON_MOVE] != BUTTON_STATE_DOWN)
						m_nButtonState[CITY_WARP_BUTTON_MOVE] = BUTTON_STATE_UP;
				}
				else 
				{
					m_nButtonState[CITY_WARP_BUTTON_MOVE] = BUTTON_STATE_NORMAL;
				}
				if( pt.x > CITY_WARP_BUTTON_CANCEL_START_X && 
					pt.x < CITY_WARP_BUTTON_CANCEL_START_X+CITY_WARP_BUTTON_SIZE_X)
				{
					if(m_nButtonState[CITY_WARP_BUTTON_CANCEL] != BUTTON_STATE_DOWN)
						m_nButtonState[CITY_WARP_BUTTON_CANCEL] = BUTTON_STATE_UP;
				}
				else 
				{
					m_nButtonState[CITY_WARP_BUTTON_CANCEL] = BUTTON_STATE_NORMAL;
				}
			}
			else
			{
				m_nButtonState[CITY_WARP_BUTTON_MOVE] = BUTTON_STATE_NORMAL;
				m_nButtonState[CITY_WARP_BUTTON_CANCEL] = BUTTON_STATE_NORMAL;
			}


		}
		break;
	case WM_LBUTTONDOWN:
		{
			POINT pt;
			pt.x = LOWORD(lParam);
			pt.y = HIWORD(lParam);
			CheckMouseReverse(&pt);
#ifdef _INET_MAPWARP_INFO
			int nCurrentSelectRealIndex = m_pScroll->GetCurrentSelectDataIndex();
			if (nCurrentSelectRealIndex >= 0 &&
				nCurrentSelectRealIndex < m_vecWarpTargetInfo.size())
			{
				WARP_TARGET_MAP_INFO_4_EXCHANGE *pWarpInfo = m_vecWarpTargetInfo[nCurrentSelectRealIndex];
				if (pWarpInfo)
				{
					AddMapInfo(pWarpInfo->MapIndex);
					RestoreMapView(FALSE, pWarpInfo->MapIndex);
					pWarpInfoMap = pWarpInfo;
					bRenderMapInfo = TRUE;
				}
				else
				{
					bRenderMapInfo = FALSE;
				}
			}
			else
			{
				bRenderMapInfo = FALSE;
			}
#endif	
			if(pt.y > CITY_WARP_BUTTON_MOVE_START_Y && 
				pt.y < CITY_WARP_BUTTON_MOVE_START_Y + CITY_WARP_BUTTON_SIZE_Y)
			{
				if( pt.x > CITY_WARP_BUTTON_MOVE_START_X && 
					pt.x < CITY_WARP_BUTTON_MOVE_START_X+CITY_WARP_BUTTON_SIZE_X)
				{
					m_nButtonState[CITY_WARP_BUTTON_MOVE] = BUTTON_STATE_DOWN;
				}
				else 
				{
					m_nButtonState[CITY_WARP_BUTTON_MOVE] = BUTTON_STATE_NORMAL;
				}
				if( pt.x > CITY_WARP_BUTTON_CANCEL_START_X && 
					pt.x < CITY_WARP_BUTTON_CANCEL_START_X+CITY_WARP_BUTTON_SIZE_X)
				{
					m_nButtonState[CITY_WARP_BUTTON_CANCEL] = BUTTON_STATE_DOWN;
				}
				else 
				{
					m_nButtonState[CITY_WARP_BUTTON_CANCEL] = BUTTON_STATE_NORMAL;
				}
			}
//			if( pt.x > CITY_WARP_LIST_START_X && 
//				pt.x < CITY_WARP_LIST_START_X + CITY_WARP_LINE_SIZE_X &&
//				pt.y > CITY_WARP_LIST_START_Y && 
//				pt.y < CITY_WARP_LIST_START_Y + CITY_WARP_LIST_INTERVAL*CITY_WARP_LIST_NUMBER)
//			{
//				int i = (pt.y-CITY_WARP_LIST_START_Y)/CITY_WARP_LIST_INTERVAL;
//				if(i>=0 && i<CITY_WARP_LIST_NUMBER)
//				{
//					if(m_vecWarpTargetInfo.size() >= i + m_nCurrentWarpListScroll)
//					{
//						m_nCurrentSelectWarpIndex = i;
//					}
//				}
//			}
		}
		break;
	case WM_LBUTTONUP:
		{
			POINT pt;
			pt.x = LOWORD(lParam);
			pt.y = HIWORD(lParam);
			CheckMouseReverse(&pt);
			if(pt.y > CITY_WARP_BUTTON_MOVE_START_Y && 
				pt.y < CITY_WARP_BUTTON_MOVE_START_Y + CITY_WARP_BUTTON_SIZE_Y)
			{
				if( pt.x > CITY_WARP_BUTTON_MOVE_START_X && 
					pt.x < CITY_WARP_BUTTON_MOVE_START_X+CITY_WARP_BUTTON_SIZE_X)
				{
					if(m_nButtonState[CITY_WARP_BUTTON_MOVE] == BUTTON_STATE_DOWN)
					{
						OnButtonClicked(CITY_WARP_BUTTON_MOVE);
					}
					m_nButtonState[CITY_WARP_BUTTON_MOVE] = BUTTON_STATE_UP;
				}
				else 
				{
					m_nButtonState[CITY_WARP_BUTTON_MOVE] = BUTTON_STATE_NORMAL;
				}
				if( pt.x > CITY_WARP_BUTTON_CANCEL_START_X && 
					pt.x < CITY_WARP_BUTTON_CANCEL_START_X+CITY_WARP_BUTTON_SIZE_X)
				{
					if(m_nButtonState[CITY_WARP_BUTTON_CANCEL] == BUTTON_STATE_DOWN)
					{
						OnButtonClicked(CITY_WARP_BUTTON_CANCEL);
					}
					m_nButtonState[CITY_WARP_BUTTON_CANCEL] = BUTTON_STATE_UP;
				}
				else 
				{
					m_nButtonState[CITY_WARP_BUTTON_CANCEL] = BUTTON_STATE_NORMAL;
				}
			}
		}
		break;
	}
	return INF_MSGPROC_NORMAL;
}

void CINFCityWarp::OnButtonClicked(int nButton)
{
	switch(nButton)
	{
	case CITY_WARP_BUTTON_MOVE:
		{
			int nCurrentSelectRealIndex = m_pScroll->GetCurrentSelectDataIndex();
			if( nCurrentSelectRealIndex >= 0 && 
				nCurrentSelectRealIndex < m_vecWarpTargetInfo.size())
			{
				WARP_TARGET_MAP_INFO_4_EXCHANGE *pWarpInfo = m_vecWarpTargetInfo[nCurrentSelectRealIndex];
				if(pWarpInfo)
				{
					if (0 < g_pGameMain->GetSummonMotherShipCnt()) {
						if (pWarpInfo->MapIndex == 3002) {
							g_pD3dApp->m_pChat->CreateChatChild("\\rCannot warp to Stones Ruin during MS\\r", COLOR_SYSTEM);
							return;
						}
					}
					// 2010-08-10 by dgwoo 버닝맵 시스템
					BURNING_MAP_INFO* pBurning = g_pDatabase->GetPtr_BurningMapInfo(pWarpInfo->MapIndex);
					if(pBurning)
					{
						
						if((pBurning->ReqMaxLv < g_pShuttleChild->m_myShuttleInfo.Level)
							|| (pBurning->ReqMinLv > g_pShuttleChild->m_myShuttleInfo.Level))
						{
							CHAR buf[256] = {0,};
							sprintf(buf,STRMSG_C_100810_0100,pBurning->ReqMinLv,pBurning->ReqMaxLv);
							g_pD3dApp->m_pChat->CreateChatChild( buf, COLOR_ERROR );
						}
					}
					// 2010-08-10 by dgwoo 버닝맵 시스템

					// 2008-01-31 by dgwoo 멤버쉽 유저의 경우 워프비 할인.
					INT nWarpPrice = pWarpInfo->Fee;
					if(0 != g_pD3dApp->GetPrimiumCardInfo()->nCardItemNum1)
					{
						nWarpPrice = nWarpPrice - (nWarpPrice / (100/MEMBERSHIP_DISCOUNT_WARPFEE));
					}
					if( nWarpPrice > g_pGameMain->m_pInven->GetItemSpi() )
					{
						g_pD3dApp->m_pChat->CreateChatChild( STRMSG_C_CITY_0006, COLOR_ERROR );//"워프에 필요한 스피가 부족합니다."
						break;
					}
					if( g_pStoreData->FindItemInInventoryByWindowPos( POS_REAR ) == NULL )
					{
						g_pD3dApp->m_pChat->CreateChatChild( STRMSG_C_CITY_0007, COLOR_ERROR );//"엔진이 없으면 출격 할 수 없습니다."
						break;
					}
					
					GUI_BUILDINGNPC* pBuilding = NULL;
					pBuilding = g_pInterface->m_pCityBase->GetCurrentBuildingNPC();
					if(pBuilding != NULL)
					{
						g_pInterface->m_pCityBase->SendLeaveEnterBuilding( pBuilding->buildingInfo.BuildingIndex, -1 );
					}
//					SendFieldSocketRequestShopWarp(pWarpInfo->MapIndex, pWarpInfo->TargetIndex);
					m_nMapIndex = pWarpInfo->MapIndex;	
					m_nTargetIndex = pWarpInfo->TargetIndex;	

					// 2004-10-25 by jschoi
					// 아래 모두 지우고 출격 가능한가 서버로 요청한다.
					// 서버에서 확인을 받으면 아래 지운 부분을 동작하도록 한다.
					// 서버에서 에러가 떨어지거나 출격 불가능한 이유가 오면 이유를 출력한다.
						g_pShuttleChild->m_nEventType = EVENT_CITY_OUT_MOVE;
					g_pFieldWinSocket->SendMsg(T_FC_CITY_CHECK_WARP_STATE, NULL, 0);

//					SAFE_DELETE(g_pShuttleChild->m_pCinemaCamera);
//					if (g_pShuttleChild->InitCinemaUnit(PATTERN_UNIT_CITY_OUT) == TRUE)
//					{
//						g_pD3dApp->m_pSound->PlayD3DSound( SOUND_TAKEINGOFF_IN_CITY, g_pShuttleChild->m_vPos );
//						g_pShuttleChild->ChangeSingleBodyCondition(BODYCON_TAKEOFF_MASK);
//						g_pShuttleChild->m_nEventType = EVENT_CITY_OUT_MOVE;
//						g_pShuttleChild->ChangeUnitState( _TAKINGOFF );
//						CAppEffectData * pEffect = new CAppEffectData(RC_EFF_LANDING_TAKEOFF,MAP_TYPE_CITY_UNIT_POS);
//						pEffect->ChangeBodyCondition(BODYCON_LANDED_MASK);
//						g_pD3dApp->m_pEffectList->AddChild(pEffect);
//
//					}
//					else
//					{
//						SendFieldSocketRequestShopWarp();
//					}
				}
			}
		}
		break;
	case CITY_WARP_BUTTON_CANCEL:
		{
		}
		break;
	}

}

void CINFCityWarp::SendFieldSocketRequestShopWarp()//int nMapIndex, int nTargetIndex)
{
	MSG_FC_EVENT_REQUEST_SHOP_WARP sMsg;
	sMsg.MapIndex = m_nMapIndex;
	sMsg.TargetIndex = m_nTargetIndex;
	g_pFieldWinSocket->SendMsg(T_FC_EVENT_REQUEST_SHOP_WARP, (char*)&sMsg, sizeof(sMsg));
	DBGOUT("FieldSocket : Request Warp. T_FC_EVENT_REQUEST_SHOP_WARP\n");
}

WARP_TARGET_MAP_INFO_4_EXCHANGE *CINFCityWarp::GetCurrentWarpInfo()
{
//	return m_vecWarpTargetInfo[m_nCurrentSelectWarpIndex + m_nCurrentWarpListScroll];
	return m_vecWarpTargetInfo[m_pScroll->GetCurrentSelectDataIndex()];
}