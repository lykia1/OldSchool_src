// NPCMapChannel.cpp: implementation of the CNPCMapChannel class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NPCMapChannel.h"
#include "NPCMonster.h"
#include "NPCMapWorkspace.h"
#include "NPCMapProject.h"
#include "NPCGlobal.h"
#include "MonsterDBAccess.h"
#include "NPCIOCP.h"
#include "DebugCheckTime.h"
#include "SkinnedMesh.h"


#define MONSTER_CREATION_HEIGHTGAP_WITH_MAPHEIGHT			100
#define MONSTER_CREATION_MAX_HEIGHTGAP_WITH_MAPHEIGHT		500
#define MONSTER_MIN_SET_TARGET_INDEX_TIME					7000				// ¸ó˝şĹÍ°ˇ TargetIndex¸¦ ĽłÁ¤ÇĎ¸é ĂÖµµ ŔĚ˝Ă°Ł µżľČŔş TargetIndex¸¦ ąö¸®Áö ľĘ´Â´Ů


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNPCMapChannel::CNPCMapChannel(CNPCMapWorkspace *i_pWorkspace,
							   CNPCMapProject *i_pProject,
							   ChannelIndex_t i_nChannelIndex)
	: CMapChannel(i_pWorkspace, i_pProject, i_nChannelIndex)
	, m_vectorMonsterCreateRegionInfoEX(i_pProject->m_vectorMONSTER_CREATE_REGION_INFO.size())
// 2008-12-02 by cmkwon, NPCServer Ĺ¬¶óŔĚľđĆ® °ü¸® ±¸Á¶ ĽöÁ¤ - CNPCIOCP ·Î ŔĚµż ÇÔ.
//	, m_vectorClientInfo(SIZE_MAX_FIELDSERVER_SESSION)
{
	LastMSSpawned = time(nullptr);
	LastMSSpawned -= 61; //decrease by over minute so at start server will spawn normally sp's
	nLastMSMonsterUID = -1;
	
	m_pNPCMapWorkspace			= i_pWorkspace;
	m_pNPCMapProject			= i_pProject;
	m_pNPCIOCPServer			= (CNPCIOCP*)i_pWorkspace->m_pIOCPServer;
	m_vectorObjectMonsterInfoCopy = m_pNPCMapProject->m_vectorObjectMonsterInfo;
	
	///////////////////////////////////////////////////////////////////////////////
	DWORD dwTick = timeGetTime();
	vectorObjectInfoServer::iterator itr(m_vectorObjectMonsterInfoCopy.begin());
// 2007-08-18 by cmkwon, żŔşęÁ§Ć® ¸ó˝şĹÍ ĽŇČŻ Á¤ş¸żˇ MONSTER_INFO * ĽłÁ¤ÇĎ±â - ľĆ·ˇżÍ °°ŔĚ ĽöÁ¤ÇÔ
//	for(; itr != m_vectorObjectMonsterInfoCopy.end(); itr++)
//	{// 2006-11-22 by cmkwon, żŔşęÁ§Ć® ş¸˝ş ¸ó˝şĹÍ ĽŇČŻ°ü·Ă ˝Ă°Ł ĽłÁ¤
//		if(itr->m_EventInfo.m_byBossMonster)
//		{
//			itr->m_EventInfo.m_dwLastTimeObjectMonsterCreated	= dwTick;	// 2006-11-22 by cmkwon
//			itr->dwObjBossMonResTime	= (3 + itr->m_EventInfo.m_EventwParam3/3 + RANDI(1, itr->m_EventInfo.m_EventwParam3*2/3))*60*1000;
//		}
//	}
	while(itr != m_vectorObjectMonsterInfoCopy.end())
	{
		OBJECTINFOSERVER *pObjInfoServ = &*itr;
		pObjInfoServ->m_pMonsterInfo	= m_pNPCIOCPServer->GetMonsterInfo(pObjInfoServ->m_EventInfo.m_nObejctMonsterUnitKind);
		if(NULL == pObjInfoServ->m_pMonsterInfo)
		{// 2007-08-18 by cmkwon, żŔşęÁ§Ć® ¸ó˝şĹÍ ĽŇČŻ Á¤ş¸żˇ MONSTER_INFO * ĽłÁ¤ÇĎ±â - ¸ó˝şĹÍ Á¤ş¸¸¦ ĂŁŔ»Ľö ľřŔ¸¸é ·Î±× ł˛±â±â
			char	szError[1024];
			sprintf(szError, "[Error] CNPCMapChannel::CNPCMapChannel invalid MonsterType, MAP(%4d) ObjectMonsterInfoIndex[%2d] MonsterType[%6d]\r\n"
				, this->m_MapChannelIndex.MapIndex, pObjInfoServ->m_EventInfo.m_EventwParam1, pObjInfoServ->m_EventInfo.m_nObejctMonsterUnitKind);
			g_pNPCGlobal->WriteSystemLog(szError);
			DbgOut(szError);

			itr = m_vectorObjectMonsterInfoCopy.erase(itr);
			continue;
		}

		if(itr->m_EventInfo.m_byBossMonster)
		{
			itr->m_EventInfo.m_dwLastTimeObjectMonsterCreated	= dwTick;	// 2006-11-22 by cmkwon
			// 2009-10-23 by cmkwon, NPCServer 0Ŕ¸·Î łŞ´©´Â ąö±× ĽöÁ¤ - CNPCMapChannel::CNPCMapChannel#,
			itr->dwObjBossMonResTime	= (3 + itr->m_EventInfo.m_EventwParam3/3 + RANDI(1, max(1,itr->m_EventInfo.m_EventwParam3*2/3)))*60*1000;
		}
		
		itr++;
	}
	

	m_uiMissileUniqueIndex		= 0;
	m_uiAttackedItemUniqueIndex	= 0;
	m_CityWarOccupyGuildUID		= INVALID_GUILD_UID;

	m_dwWorkeredTick			= 0;		// 2008-12-03 by cmkwon, NPCServer Ĺ¬¶óŔĚľđĆ® ľř´Â ¸ĘĂ¤łÎ Ăł¸® ĽöÁ¤ - 
	m_bExistUserInMapChannel	= FALSE;	// 2008-12-03 by cmkwon, NPCServer Ĺ¬¶óŔĚľđĆ® ľř´Â ¸ĘĂ¤łÎ Ăł¸® ĽöÁ¤ - 
	m_dwChangedTickforExistUser	= 0;		// 2008-12-03 by cmkwon, NPCServer Ĺ¬¶óŔĚľđĆ® ľř´Â ¸ĘĂ¤łÎ Ăł¸® ĽöÁ¤ - 

// 	static int sTotalCount	= 0;
// 	static int sTotalByte	= 0;
// 	sTotalCount	+= m_nSizemtvectorMonsterPtr;
// 	sTotalByte	+= sizeof(CNPCMonster) * m_nSizemtvectorMonsterPtr;
// 	g_pNPCGlobal->WriteSystemLogEX(TRUE, "[TEMP] 100408 CNPCMonster TotalCount(%8d) TotalBytes(%8d) \r\n", sTotalCount, sTotalByte);

	m_ArrNPCMonster				= new CNPCMonster[m_nSizemtvectorMonsterPtr];
	m_mtvectorMonsterPtr.lock();
	m_mtvectorMonsterPtr.reserve(m_nSizemtvectorMonsterPtr);
	for(int i = 0;i < m_nSizemtvectorMonsterPtr; i++)
	{
		m_ArrNPCMonster[i].SetMonsterIndex(i + MONSTER_CLIENT_INDEX_START_NUM);
		m_mtvectorMonsterPtr.push_back(&m_ArrNPCMonster[i]);
	}
	m_mtvectorMonsterPtr.unlock();

// 2008-12-02 by cmkwon, NPCServer Ĺ¬¶óŔĚľđĆ® °ü¸® ±¸Á¶ ĽöÁ¤ - CNPCIOCP ·Î ŔĚµż ÇÔ.
//	for(i = 0; i < SIZE_MAX_FIELDSERVER_SESSION; i++)
//	{		
//		m_vectorClientInfo[i].ClientIndex	= i;
//		m_vectorClientInfo[i].ResetClientInfo();
//	}
	
	for( int i = 0; i < m_vectorMonsterCreateRegionInfoEX.size(); i++)
	{
		m_vectorMonsterCreateRegionInfoEX[i].nCreatedCount		= 0;
		m_vectorMonsterCreateRegionInfoEX[i].nCurrentCount		= 0;
		m_vectorMonsterCreateRegionInfoEX[i].dwLastTimeMonsterCreate = dwTick;
		m_vectorMonsterCreateRegionInfoEX[i].dwBossMonResTime	= 5000;

		if(i_pProject->m_vectorMONSTER_CREATE_REGION_INFO[i].bMonType == MONSTER_CREATETYPE_BOSS)
		{
			// 2006-12-11 by cmkwon, ŔĎąÝ ş¸˝ş ¸ó˝şĹÍ´Â NPC Ľ­ąö ˝ĂŔŰ ČÄ 5şĐ°ć°ú ČÄżˇ ¶ßµµ·Ď ĽöÁ¤
			m_vectorMonsterCreateRegionInfoEX[i].dwBossMonResTime = 5*60*1000;
		}

		////////////////////////////////////////////////////////////////////////////////
		// 2009-09-09 ~ 2010 by dhjin, ŔÎÇÇ´ĎĆĽ - ŔÎÇÇ ¸ĘżˇĽ± ¸®Á¨ ˝Ă°ŁŔĚ Ăą ĽŇČŻ˝Ă°ŁŔĚ°í ÇŃ ąř ĽŇČŻµČ ¸ó˝şĹÍ´Â ´ő ŔĚ»ó ĽŇČŻ˝ĂĹ°Áö ľĘ´Â´Ů.
//		if(IS_MAP_INFLUENCE_INFINITY(this->GetMapInfluenceTypeW()))	{
//			if(i_pProject->m_vectorMONSTER_CREATE_REGION_INFO[i].bMonType == MONSTER_CREATETYPE_BOSS) { 
//				// ş¸˝ş´Â 1şĐ ´ÜŔ§
//				m_vectorMonsterCreateRegionInfoEX[i].dwBossMonResTime = m_pNPCMapProject->m_vectorMONSTER_CREATE_REGION_INFO[i].sResTime * 60 * TICK_CREATE_MONSTER_TERM;
//			}
//			else {
//				// ş¸˝ş Á¦żÜ´Â ĂĘ´ÜŔ§
//				m_vectorMonsterCreateRegionInfoEX[i].dwBossMonResTime = m_pNPCMapProject->m_vectorMONSTER_CREATE_REGION_INFO[i].sResTime * TICK_CREATE_MONSTER_TERM;
//			}
//		}

	}

	m_bNotCreateMonster	= FALSE;	// 2007-08-29 by dhjin

	m_bAutoCreateMonsterChannel = m_pNPCMapProject->m_bAutoCreateMonster;	// 2009-09-09 ~ 2010 by dhjin, ŔÎÇÇ´ĎĆĽ -  Ă¤łÎ ş°·Î Č®ŔÎÇŃ´Ů.
	m_mtDeletedObjectInfoList.clear();		// 2009-09-09 ~ 2010-01 by dhjin, ŔÎÇÇ´ĎĆĽ - ĽŇ˝ş ĂĽĹ©, // 2009-09-09 ~ 2010 by dhjin, ŔÎÇÇ´ĎĆĽ - şŻ°ć żŔşęÁ§Ć®¸¦ Ŕ§ÇŘ!!!!
	m_mtNewObjectInfoList.clear();			// 2009-09-09 ~ 2010-01 by dhjin, ŔÎÇÇ´ĎĆĽ - ĽŇ˝ş ĂĽĹ©, // 2009-09-09 ~ 2010 by dhjin, ŔÎÇÇ´ĎĆĽ - şŻ°ć żŔşęÁ§Ć®¸¦ Ŕ§ÇŘ!!!!
	
	m_mtvectorMSG_FN_MONSTER_CREATE_OK.reserve(10);
	m_mtvectorMSG_FN_MONSTER_CREATE_OKProcess.reserve(10);
	m_mtvectorMSG_FN_MONSTER_DELETE.reserve(10);
	m_mtvectorMSG_FN_MONSTER_DELETEProcess.reserve(10);
	m_mtvectorMSG_FN_MONSTER_SKILL_END_SKILL.reserve(10);
	m_mtvectorMSG_FN_MONSTER_SKILL_END_SKILLProcess.reserve(10);
	m_mtvectorMSG_FN_BATTLE_SET_ATTACK_CHARACTER.reserve(10);
	m_mtvectorMSG_FN_BATTLE_SET_ATTACK_CHARACTERProcess.reserve(10);
	m_mtvectorMSG_FN_CITYWAR_START_WAR.reserve(2);
	m_mtvectorMSG_FN_CITYWAR_END_WAR.reserve(2);
	m_mtvectorMSG_FN_CITYWAR_CHANGE_OCCUPY_INFO.reserve(2);
	m_mtvectorMSG_FN_MONSTER_STRATEGYPOINT_INIT.reserve(1);		
	m_mtvectorMSG_FN_MONSTER_OUTPOST_INIT.reserve(1);		
	m_mtvectMSG_MONSTER_SUMMON_BY_BELL.reserve(1);			// 2007-09-19 by cmkwon, Bell·Î ĽŇČŻ Ăł¸®
	m_mtvectorMSG_FN_NPCSERVER_CINEMA_MONSTER_CREATE.reserve(10);	// 2010-03-31 by dhjin, ŔÎÇÇ´ĎĆĽ(±âÁöąćľî) - // 2009-09-09 ~ 2010 by dhjin, ŔÎÇÇ´ĎĆĽ - Key¸ó˝şĹÍ »ýĽş
	m_mtvectorMSG_FN_NPCSERVER_CINEMA_MONSTER_DESTROY.reserve(10);	// 2011-04-28 by hskim, ŔÎÇÇ´ĎĆĽ 3Â÷ - ˝Ăł×¸¶ °ü·Ă ±â´É Ăß°ˇ - ÇŘ´ç ¸ĘĂ¤łÎ ĆŻÁ¤ ¸ó˝şĹÍ »čÁ¦ ±â´É Ăß°ˇ
	m_mtvectorMSG_FN_NPCSERVER_CINEMA_MONSTER_CHANGE.reserve(10);	// 2011-05-11 by hskim, ŔÎÇÇ´ĎĆĽ 3Â÷ - ˝Ăł×¸¶ °ü·Ă ±â´É Ăß°ˇ - ÇŘ´ç ¸ĘĂ¤łÎ ĆŻÁ¤ ¸ó˝şĹÍ şŻ°ć ±â´É Ăß°ˇ
}

CNPCMapChannel::~CNPCMapChannel()
{
	SAFE_DELETE_ARRAY(m_ArrNPCMonster);
}

//////////////////////////////////////////////////////////////////////
// Method
//////////////////////////////////////////////////////////////////////

BOOL CNPCMapChannel::InitMapChannel(void)
{
	CMapChannel::InitMapChannel();
	
	
	ResetMapChannel();
	
	return TRUE;
}

CNPCMonster * CNPCMapChannel::GetNPCMonster(ClientIndex_t i_MonsterIndex)
{
	int mIdx = i_MonsterIndex - MONSTER_CLIENT_INDEX_START_NUM;
	if(FALSE == IS_VALID_ARRAY_INDEX(mIdx, m_nSizemtvectorMonsterPtr))
	{
		return NULL;
	}

	return &m_ArrNPCMonster[mIdx];
}

// 2009-12-11 by cmkwon, µĄąĚÁö ľî±×·Î·Î Ĺ¸°ŮŔ» şŻ°ćÇĎ´Â ¸ó˝şĹÍ ±¸Çö - CNPCMapChannel::GetClientInfo#
CLIENT_INFO* CNPCMapChannel::GetClientInfo(int i_Characteridx, MAP_CHANNEL_INDEX *i_pMapChann/*=NULL*/)
{
	// 2009-12-11 by cmkwon, µĄąĚÁö ľî±×·Î·Î Ĺ¸°ŮŔ» şŻ°ćÇĎ´Â ¸ó˝şĹÍ ±¸Çö - 
	//return m_pNPCIOCPServer->GetClientInfoO(i_Characteridx);		// 2008-12-02 by cmkwon, NPCServer Ĺ¬¶óŔĚľđĆ® °ü¸® ±¸Á¶ ĽöÁ¤ - 
	///////////////////////////////////////////////////////////////////////////////	
	// 2009-12-11 by cmkwon, µĄąĚÁö ľî±×·Î·Î Ĺ¸°ŮŔ» şŻ°ćÇĎ´Â ¸ó˝şĹÍ ±¸Çö - 
	CLIENT_INFO *pCliInfo = m_pNPCIOCPServer->GetClientInfoO(i_Characteridx);
	if(NULL == pCliInfo)
	{
		return NULL;
	}
	if(i_pMapChann
		&& FALSE == i_pMapChann->IsSameMapChannelIndex(pCliInfo->MapChannelIdx))
	{
		return NULL;
	}
	return pCliInfo;

// 2008-12-02 by cmkwon, NPCServer Ĺ¬¶óŔĚľđĆ® °ü¸® ±¸Á¶ ĽöÁ¤ - Ŕ§żÍ °°ŔĚ m_pNPCIOCPServer->GetClientInfoO()¸¦ ČŁĂâ ÇÔ.
// 	if(FALSE == IS_VALID_ARRAY_INDEX(i_Characteridx, SIZE_MAX_FIELDSERVER_SESSION))
// 	{
// 		return NULL;
// 	}
// 
// 	return &m_vectorClientInfo[i_Characteridx];
}

MONSTER_CREATE_REGION_INFO_EX * CNPCMapChannel::GetMonsterCreateRegionInfoEXWidhIndex(int i_nCreateRegionIdex)
{
	if(FALSE == IS_VALID_ARRAY_INDEX(i_nCreateRegionIdex, m_vectorMonsterCreateRegionInfoEX.size()))
	{
		return NULL;
	}
	return &m_vectorMonsterCreateRegionInfoEX[i_nCreateRegionIdex];
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			void CNPCMapChannel::CreateMonsterMapChannel(BYTE *pSendBuf, vector<D3DXVECTOR3> *pVECTOR2vector, DWORD i_dwCurrentTick)
/// \brief		
/// \author		cmkwon
/// \date		2004-03-26 ~ 2004-03-26
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CNPCMapChannel::CreateMonstersAllCreateRegion(BYTE *pSendBuf, vector<D3DXVECTOR3> *pVECTOR2vector, DWORD i_dwCurrentTick)
{
	// check: Ć©Ĺä¸®ľó ¸ĘŔĚ¸é ¸ó˝şĹÍ »ýĽş ľČ ÇÔ, ŔÓ˝Ă·Î ÄÚµůÇÔ, Ă¶ąÎľľ°ˇ ´Ů˝Ă °íĂÄľß ÇÔ! 20040220, kelovon
	////////////////////////////////////////////////////////////////////////////////
	// 2009-09-09 ~ 2010 by dhjin, ŔÎÇÇ´ĎĆĽ - ąŘ°ú °°ŔĚ ĽöÁ¤ - Ă¤łÎş°·Î »ýĽş °ˇ´ÉÇĎ°Ô
//	if(FALSE == m_pNPCMapProject->m_bAutoCreateMonster
	if(FALSE == m_bAutoCreateMonsterChannel
		|| IS_TUTORIAL_MAP_INDEX(this->GetMapChannelIndex().MapIndex)
		|| IS_MAP_INFLUENCE_OUTPOST(this->GetMapInfluenceTypeW()) ) // 2007-08-24 by dhjin, ŔüÁř±âÁö ¸Ęµµ ¸ó˝şĹÍ »ýĽşÇĎÁö ľĘ´Â´Ů.
	{
		return;
	}

	CNPCMonster					*pNMonster = NULL;
	MONSTER_CREATE_REGION_INFO	*pMonsterCreateRegionInfo = NULL;
	MONSTER_CREATE_REGION_INFO_EX *pRegionInfoEX;
	int							nMonsterInfoSize;
	D3DXVECTOR3					posVector3;

	///////////////////////////////////////////////////////////////////////////////
	// żŔşęÁ§Ć® ¸ó˝şĹÍ »ýĽş Ăł¸®
	vectorObjectInfoServer::iterator itr(m_vectorObjectMonsterInfoCopy.begin());
	for(; itr != m_vectorObjectMonsterInfoCopy.end(); itr++)
	{
		this->CreateMonstersBYObject(pSendBuf, &*itr, i_dwCurrentTick);
// 2006-11-22 by cmkwon, Ŕ§ŔÇ ÇÔĽö·Î Ăł¸®
//		OBJECTINFOSERVER *pObjInfo = itr;
//		if(pObjInfo->m_EventInfo.m_byObjectMonsterCreated)
//		{// ŔĚąĚ »ýĽşµČ  °ćżě
//			continue;
//		}
//		else if(pObjInfo->m_EventInfo.m_EventwParam3*1000 > i_dwCurrentTick - pObjInfo->m_EventInfo.m_dwLastTimeObjectMonsterCreated)
//		{// ¸®Á¨ Ĺ¸ŔÓŔĚ ÁöłŞÁö ľĘŔ˝
//			continue;
//		}
//		else if(pObjInfo->m_EventInfo.m_byIsCityWarMonster && FALSE == this->m_bCityWarStarted)
//		{// µµ˝ĂÁˇ·ÉŔü¸ó˝şĹÍŔĚ¸éĽ­ µµ˝ĂÁˇ·ÉŔüŔĚ ÁřÇŕÁßŔĚ ľĆ´Ň¶§
//			continue;
//		}
//		else if(m_uiLimitMonsterCountsInChannel <= m_nCurMonsterCountInChannel && FALSE == pObjInfo->m_EventInfo.m_byIsCityWarMonster)
//		{// ¸ĘŔÇ ĂÖ´ë ¸ó˝şĹÍ Ä«żîĆ® ş¸´Ů Ĺ©¸éĽ­ µµ˝ĂÁˇ·ÉŔü ¸ó˝şĹÍ°ˇ ľĆ´Ď´Ů
//			continue;
//		}
//		
//		map<int, MONSTER_INFO>::iterator it = m_pNPCMapProject->m_pMapMonsterParameter->find(pObjInfo->m_EventInfo.m_nObejctMonsterUnitKind);
//		if(it == m_pNPCMapProject->m_pMapMonsterParameter->end())
//		{// »ýĽş ÇĎ·Á´Â ¸ó˝şĹÍ Á¤ş¸°ˇ ľřŔ˝
//			char	szError[1024];
//			sprintf(szError, "[Error] CNPCMapProject::NPCMonsterCreate_1 invalid MonsterType, MAP(%4d) ObjectMonsterInfoIndex[%2d] MonsterType[%6d]\r\n"
//				, this->m_MapChannelIndex.MapIndex, pObjInfo->m_EventInfo.m_EventwParam1, pObjInfo->m_EventInfo.m_nObejctMonsterUnitKind);
//			g_pNPCGlobal->WriteSystemLog(szError);
//			DbgOut(szError);
//			continue;
//		}
//
//		mt_auto_lock mtMon(&m_mtvectorMonsterPtr);
//		pObjInfo->m_EventInfo.m_dwLastTimeObjectMonsterCreated = i_dwCurrentTick;
//		if(m_vectorUsableMonsterIndex.empty())
//		{
//			char	szError[1024];
//			sprintf(szError, "[Error] CNPCMapProject::NPCMonsterCreate_2 Monster is full. MaxMonsterCount[%d], CurrentMonsterCount(%d)\r\n"
//				, m_nMaxMonsterCountInChannel, m_nCurMonsterCountInChannel);
//			g_pNPCGlobal->WriteSystemLog(szError);
//			DbgOut(szError);
//			return;
//		}
//		ClientIndex_t nMonsterIndex = m_vectorUsableMonsterIndex.front();
//		m_vectorUsableMonsterIndex.pop_front();
//		pNMonster = GetNPCMonster(nMonsterIndex);
//		if(NULL == pNMonster 
//			|| pNMonster->m_enMonsterState != MS_NULL)
//		{
//			continue;
//		}
//		m_nCurMonsterCountInChannel++;				// Current Counts¸¦ Áő°ˇ
//		pObjInfo->m_EventInfo.m_byObjectMonsterCreated = TRUE;
//
//// 2006-04-25 by cmkwon, µµ˝ĂÁˇ·ÉŔü ˝Ă˝şĹŰŔş »çżëÇĎÁö ľĘ°í ŔÖŔ˝
//// 		if(it->second.MonsterUnitKind == 2012000)
//// 		{
//// 			g_pNPCGlobal->WriteSystemLogEX(TRUE, STRMSG_S_N2NOTIFY_0001, it->second.MonsterName);
//// 		}
//
//		// ¸ó˝şĹÍ »ýĽş, ±âş»°Ş ĽĽĆĂ
//		pNMonster->CreateNPCMonster(&it->second, &pObjInfo->m_vPos, i_dwCurrentTick, pObjInfo->m_EventInfo.m_EventwParam1
//			, MONSTER_TARGETTYPE_NORMAL, 0, 0, this->GetMonsterVisibleDiameterW(), 0, pObjInfo->m_EventInfo.m_bEventType, &pObjInfo->m_vVel);					
//		SetInitialPositionAndSendCreateMonster(pNMonster, pSendBuf);
	}// end_for(; itr != m_vectorObjectMonsterInfoCopy.end(); itr++)
	// end_żŔşęÁ§Ć® ¸ó˝şĹÍ »ýĽş Ăł¸®
	///////////////////////////////////////////////////////////////////////////////
	
	///////////////////////////////////////////////////////////////////////////////
	// ŔĎąÝ żµżŞ ¸ó˝şĹÍ »ýĽş Ăł¸®
	nMonsterInfoSize = m_pNPCMapProject->m_vectorMONSTER_CREATE_REGION_INFO.size();
	for(int i=0; i < nMonsterInfoSize; i++)
	{
		pMonsterCreateRegionInfo	= &m_pNPCMapProject->m_vectorMONSTER_CREATE_REGION_INFO[i];
		pRegionInfoEX				= &m_vectorMonsterCreateRegionInfoEX[i];
		this->CreateMonstersBYRegion(pSendBuf, pVECTOR2vector, i_dwCurrentTick, i, pMonsterCreateRegionInfo, pRegionInfoEX);
// 2006-11-20 by cmkwon, Ŕ§ŔÇ ÇÔĽö·Î Ăł¸®
//				
//		///////////////////////////////////////////////////////////////////////////////
//		// ş¸˝ş±ŢŔş »ýĽş ˝Ă°Ł´ÜŔ§°ˇ şĐ´ÜŔ§·Î °č»ęÇŃ´Ů.
//		// ş¸˝ş±Ţ ¸ó˝şĹÍ°ˇ ľĆ´Ď¸é »ýĽş ˝Ă°ŁŔ» ąĐ¸® ĽĽÄÁµĺ ´ÜŔ§·Î Ăł¸®ÇĎ¸ç
//		DWORD dwTimeGap				= i_dwCurrentTick-pRegionInfoEX->dwLastTimeMonsterCreate;
//		if (MONSTER_CREATETYPE_BOSS == pMonsterCreateRegionInfo->bMonType)
//		{
//			if((0 == pRegionInfoEX->nCreatedCount && dwTimeGap < 3*60*1000)
//				|| (0 != pRegionInfoEX->nCreatedCount && dwTimeGap < pRegionInfoEX->dwBossMonResTime)
//				)
//			{// ş¸˝ş ¸ó˝şĹÍ´Â ĂłŔ˝ ¸®Á¨ Ĺ¸ŔÓ 3şĐŔĚ ÁöłŞÁö ľĘľŇ°ĹłŞ ¸®Á¨ Ĺ¸ŔÓŔĚ °ć°ú µÇÁö ľĘŔ˝
//				continue;
//			}
//			else if(pRegionInfoEX->nCurrentCount >= pMonsterCreateRegionInfo->sMaxMon)
//			{// żµżŞ ¸ó˝şĹÍ ĂÖ´ëÄ«żîĆ® ş¸´Ů Ĺ¬¶§
//				continue;
//			}
//		}
//		else
//		{// ş¸˝ş¸ó˝şĹÍ°ˇ ľĆ´Ô
//			
//			if(dwTimeGap < ((DWORD)pMonsterCreateRegionInfo->sResTime)*1000)
//			{// ¸®Á¨ Ĺ¸ŔÓŔĚ °ć°ú ÇĎÁö ľĘŔş °ćżě
//				continue;
//			}
//			else if(m_uiLimitMonsterCountsInChannel <= m_nCurMonsterCountInChannel)
//			{// ¸ĘŔÇ ĂÖ´ë ¸ó˝şĹÍ Ä«żîĆ® ş¸´Ů Ĺ¬¶§
//				continue;
//			}
//			else if(pRegionInfoEX->nCurrentCount >= (pMonsterCreateRegionInfo->sMaxMon+pMonsterCreateRegionInfo->sMaxMon/10)*m_nMaxMonsterCountInChannel/m_uiLimitMonsterCountsInChannel)
//			{// żµżŞ ¸ó˝şĹÍ ĂÖ´ëÄ«żîĆ® ş¸´Ů Ĺ¬¶§
//				continue;
//			}
//		}
//		
//		MONSTER_INFO *pMonInfo = m_pNPCIOCPServer->GetMonsterInfo(pMonsterCreateRegionInfo->sMonType);
//		if(NULL == pMonInfo)
//		{
//			char	szError[1024];
//			sprintf(szError, "[Error] CNPCMapProject::NPCMonsterCreate_3 invalid MonsterType, MAP(%4d) MonsterInfoIndex[%2d] MonsterType[%6d]\r\n"
//				, this->m_MapChannelIndex.MapIndex, i, pMonsterCreateRegionInfo->sMonType);
//			g_pNPCGlobal->WriteSystemLog(szError);
//			DbgOut(szError);
//			continue;
//		}
//
//		///////////////////////////////////////////////////////////////////////////////
//		// 2006-04-18 by cmkwon, »ýĽş ˝Ă°ŁŔ» ĽłÁ¤ÇŃ´Ů.
//		pRegionInfoEX->dwLastTimeMonsterCreate = i_dwCurrentTick;				// »ýĽş ˝Ă°ŁŔ» ĽłÁ¤ÇŃ´Ů.
//
//		///////////////////////////////////////////////////////////////////////////////
//		// 2006-04-18 by cmkwon
//		if(FALSE == this->IsEnableCreateMonster(pMonInfo))
//		{
//			char szTest[1024];
//			wsprintf(szTest, "  CreateRegionInfo MapIndexChannel(%d,%d) InfoIdx(%d) Mon(%8d) ResTime(%dĂĘ), dwTimeGap(%dĂĘ)\r\n"
//				, this->GetMapChannelIndex().MapIndex, this->GetMapChannelIndex().ChannelIndex, i
//				, pMonsterCreateRegionInfo->sMonType, pMonsterCreateRegionInfo->sResTime, dwTimeGap/1000);
//			g_pNPCGlobal->WriteSystemLog(szTest);
//			DbgOut(szTest);
//			continue;
//		}
//
//		int curMonsters = NPCGetMonsterCountInRegion(pMonsterCreateRegionInfo->sStartx, pMonsterCreateRegionInfo->sStartz,
//			pMonsterCreateRegionInfo->sEndx, pMonsterCreateRegionInfo->sEndz, pMonsterCreateRegionInfo->sMonType,
//			pMonsterCreateRegionInfo->sMaxMon);
//		creatMonsters = pMonsterCreateRegionInfo->sMaxMon - curMonsters;
//		if(creatMonsters <= 0)
//		{// »ýĽş ¸ó˝şĹÍ Ľö°ˇ ŔŻČżÇĎÁö ľĘŔ˝
//
//			continue;
//		}
//
//		
//		creatMonsters = creatMonsters > pMonsterCreateRegionInfo->sResNum ? pMonsterCreateRegionInfo->sResNum : creatMonsters;
//		creatMonsters = rand()%creatMonsters + 1;
//		NPCGetCreatablePosition(pMonInfo->MonsterForm, pMonInfo->Size, pMonsterCreateRegionInfo->sStartx, pMonsterCreateRegionInfo->sStartz,
//			pMonsterCreateRegionInfo->sEndx, pMonsterCreateRegionInfo->sEndz, 
//			m_pNPCMapProject->m_sMinimumAltitude, m_pNPCMapProject->m_sMaximumAltitude
//			, *pVECTOR2vector, creatMonsters);
//
//		mt_auto_lock mtMon(&m_mtvectorMonsterPtr);
//		while(creatMonsters > 0 && false == pVECTOR2vector->empty())
//		{
//			creatMonsters--;
//			if(m_vectorUsableMonsterIndex.empty())
//			{
//				char	szError[1024];
//				sprintf(szError, "[Error] CNPCMapProject::NPCMonsterCreate_4 Monster is full. MaxMonsterCount[%d], CurrentMonsterCount(%d)\r\n"
//					, m_nMaxMonsterCountInChannel, m_nCurMonsterCountInChannel);
//				g_pNPCGlobal->WriteSystemLog(szError);
//				DbgOut(szError);
//				return;
//			}
//			ClientIndex_t nMonsterIndex = m_vectorUsableMonsterIndex.front();
//			m_vectorUsableMonsterIndex.pop_front();
//			pNMonster = GetNPCMonster(nMonsterIndex);
//			if(NULL == pNMonster 
//				|| pNMonster->m_enMonsterState != MS_NULL)
//			{
//				continue;
//			}
//			m_nCurMonsterCountInChannel++;				// Current Counts¸¦ Áő°ˇ
//			pRegionInfoEX->nCreatedCount++;				// żµżŞŔÇ ´©Ŕű »ýĽş Counts¸¦ Áő°ˇ
//			pRegionInfoEX->nCurrentCount++;				// żµżŞŔÇ Current »ýĽş Counts¸¦ Áő°ˇ
//
//			posVector3 = pVECTOR2vector->back();
//			pVECTOR2vector->pop_back();
//		
//			// ¸ó˝şĹÍ »ýĽş, ±âş»°Ş ĽĽĆĂ
//			pNMonster->CreateNPCMonster(pMonInfo, &posVector3, i_dwCurrentTick, i
//				, MONSTER_TARGETTYPE_NORMAL, 0, 0, this->GetMonsterVisibleDiameterW());
//			SetInitialPositionAndSendCreateMonster(pNMonster, pSendBuf);
//		}
	}
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			void CNPCMapChannel::CreateMonstersBYRegion(BYTE *pSendBuf, vector<D3DXVECTOR3> *pVECTOR2vector, DWORD i_dwCurrentTick, int i, MONSTER_CREATE_REGION_INFO *pMonsterCreateRegionInfo, MONSTER_CREATE_REGION_INFO_EX *pRegionInfoEX, BOOL i_bMustCreate/*=FALSE*/)
/// \brief		
/// \author		cmkwon
/// \date		2006-11-20 ~ 2006-11-20
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CNPCMapChannel::CreateMonstersBYRegion(BYTE *pSendBuf, vector<D3DXVECTOR3> *pVECTOR2vector, DWORD i_dwCurrentTick
											, int i_nArrIdx, MONSTER_CREATE_REGION_INFO *pMonsterCreateRegionInfo, MONSTER_CREATE_REGION_INFO_EX *pRegionInfoEX
											, BOOL i_bMustCreate/*=FALSE*/)
{
	CNPCMonster			*pNMonster = NULL;
	int					creatMonsters;						// »ýĽşÇŇ ¸ó˝şĹÍ °łĂĽĽö
	D3DXVECTOR3			posVector3;

	DWORD dwTimeGap				= i_dwCurrentTick - pRegionInfoEX->dwLastTimeMonsterCreate;
	
#ifdef _INET_BOSS_DB_CONTROL
	for(auto &el : m_pNPCIOCPServer->vectMonsters)
	{
		if(el.nMonsterUID == pMonsterCreateRegionInfo->sMonType && el.bSpawned == false && el.MapIndex == this->m_MapChannelIndex.MapIndex){
			short nRetMonBoss = m_pNPCMapProject->LoadDB_Boss_CheckSpawnAllow(pMonsterCreateRegionInfo->sMonType,this->m_MapChannelIndex.MapIndex);
			if(nRetMonBoss == 2) //2 means in table and not time passed for respawn
				return;
			
			if(nRetMonBoss == 100){
				el.bSpawned = true; //is spawned
				i_bMustCreate = TRUE;
			}
		}
	}
#endif
	
	if(FALSE == i_bMustCreate) //if its 100 then its time to spawn
	{
		///////////////////////////////////////////////////////////////////////////////
		// ş¸˝ş±ŢŔş »ýĽş ˝Ă°Ł´ÜŔ§°ˇ şĐ´ÜŔ§·Î °č»ęÇŃ´Ů.
		// ş¸˝ş±Ţ ¸ó˝şĹÍ°ˇ ľĆ´Ď¸é »ýĽş ˝Ă°ŁŔ» ąĐ¸® ĽĽÄÁµĺ ´ÜŔ§·Î Ăł¸®ÇĎ¸ç
		if (MONSTER_CREATETYPE_BOSS == pMonsterCreateRegionInfo->bMonType)
		{
			if(dwTimeGap < pRegionInfoEX->dwBossMonResTime)
			{// ¸®Á¨ Ĺ¸ŔÓŔĚ °ć°ú µÇÁö ľĘŔ˝
				return;
			}
			else if(pRegionInfoEX->nCurrentCount >= pMonsterCreateRegionInfo->sMaxMon)
			{// żµżŞ ¸ó˝şĹÍ ĂÖ´ëÄ«żîĆ® ş¸´Ů Ĺ¬¶§
				return;
			} 
#ifdef _MASANG
			DWORD dwMinMinutes = max(  1, pMonsterCreateRegionInfo->sResTime/3 + RANDI(1, max(1,pMonsterCreateRegionInfo->sResTime*2/3))  );
			pRegionInfoEX->dwBossMonResTime = dwMinMinutes*60*TICK_CREATE_MONSTER_TERM;
#else
			//18-07-2020 by Inetpub - preparations for boss monsters respawn times in DB table
			DWORD dwMinMinutes = max(1, pMonsterCreateRegionInfo->sResTime / 3 + RANDI(1, max(1, pMonsterCreateRegionInfo->sResTime * 2 / 3)));
			pRegionInfoEX->dwBossMonResTime = dwMinMinutes * 60 * TICK_CREATE_MONSTER_TERM;
#endif
		}
		else
		{// ş¸˝ş¸ó˝şĹÍ°ˇ ľĆ´Ô
			
			if(dwTimeGap < ((DWORD)pMonsterCreateRegionInfo->sResTime)*TICK_CREATE_MONSTER_TERM)
			{// ¸®Á¨ Ĺ¸ŔÓŔĚ °ć°ú ÇĎÁö ľĘŔş °ćżě
				return;
			}
			else if(m_uiLimitMonsterCountsInChannel <= m_nCurMonsterCountInChannel)
			{// ¸ĘŔÇ ĂÖ´ë ¸ó˝şĹÍ Ä«żîĆ® ş¸´Ů Ĺ¬¶§
				return;
			}
			else if(pRegionInfoEX->nCurrentCount >= (pMonsterCreateRegionInfo->sMaxMon+pMonsterCreateRegionInfo->sMaxMon/10)*m_nMaxMonsterCountInChannel/m_uiLimitMonsterCountsInChannel)
			{// żµżŞ ¸ó˝şĹÍ ĂÖ´ëÄ«żîĆ® ş¸´Ů Ĺ¬¶§
				return;
			}
			////////////////////////////////////////////////////////////////////////////////
			// 2009-09-09 ~ 2010 by dhjin, ŔÎÇÇ´ĎĆĽ - ŔÎÇÇ ¸ĘżˇĽ± ¸®Á¨ ˝Ă°ŁŔĚ Ăą ĽŇČŻ˝Ă°ŁŔĚ°í ÇŃ ąř ĽŇČŻµČ ¸ó˝şĹÍ´Â ´ő ŔĚ»ó ĽŇČŻ˝ĂĹ°Áö ľĘ´Â´Ů.
//			if(IS_MAP_INFLUENCE_INFINITY(this->GetMapInfluenceTypeW()))	{
//				pRegionInfoEX->dwBossMonResTime = TICK_CREATE_MONSTER_TERM_ONLY_INFINITY;
//			}
		}
	}
	
	MONSTER_INFO *pMonInfo = m_pNPCIOCPServer->GetMonsterInfo(pMonsterCreateRegionInfo->sMonType);
	if(NULL == pMonInfo)
	{
		char	szError[1024];
		sprintf(szError, "[Error] CNPCMapChannel::CreateMonstersBYRegion_1 invalid MonsterType, MAP(%4d) MonsterInfoIndex[%2d] MonsterType[%6d]\r\n"
			, this->m_MapChannelIndex.MapIndex, i_nArrIdx, pMonsterCreateRegionInfo->sMonType);
		g_pNPCGlobal->WriteSystemLog(szError);
		DbgOut(szError);
		return;
	}
	
	//////////////////////////////////////////////////////////////////////////
	// 2007-08-29 by dhjin, ¸ó˝şĹÍ »ýĽş ±ÝÁöŔĚ¸é »ýĽşÇĎÁö ľĘ´Â´Ů. ÁÖŔÇ~!! ĽĽ·Â ¸ó˝şĹÍ´Â Á¦żÜŔĚ´Ů.
	if(m_bNotCreateMonster
		&& (BELL_INFLUENCE_VCN != pMonInfo->Belligerence
			|| BELL_INFLUENCE_ANI != pMonInfo->Belligerence))
	{
		pRegionInfoEX->dwLastTimeMonsterCreate = i_dwCurrentTick;				// 2009-10-15 by cmkwon, ĽĽ·Â¸ĘŔÇ ľČŔüĂ¤łÎżˇ ĽĽ·Â¸ó˝şĹÍ ĽŇČŻ ¸·±â - »ýĽş ˝Ă°ŁŔ» ĽłÁ¤ÇŃ´Ů.
		return;
	}

	// 2009-10-15 by cmkwon, ĽĽ·Â¸ĘŔÇ ľČŔüĂ¤łÎżˇ ĽĽ·Â¸ó˝şĹÍ ĽŇČŻ ¸·±â - CNPCMapChannel::CreateMonstersBYRegion#, 
	if( (IS_MAP_INFLUENCE_VCN(GetMapInfluenceTypeW()) || IS_MAP_INFLUENCE_ANI(GetMapInfluenceTypeW()))
		&& 0 != m_MapChannelIndex.ChannelIndex
		&& IS_INFLWAR_MONSTER(pMonInfo->Belligerence))
	{
		pRegionInfoEX->dwLastTimeMonsterCreate = i_dwCurrentTick;				// »ýĽş ˝Ă°ŁŔ» ĽłÁ¤ÇŃ´Ů.
		return;
	}
	
	///////////////////////////////////////////////////////////////////////////////
	// 2006-04-18 by cmkwon, »ýĽş ˝Ă°ŁŔ» ĽłÁ¤ÇŃ´Ů.
	pRegionInfoEX->dwLastTimeMonsterCreate = i_dwCurrentTick;				// »ýĽş ˝Ă°ŁŔ» ĽłÁ¤ÇŃ´Ů.

	///////////////////////////////////////////////////////////////////////////////
	// 2006-04-18 by cmkwon
	if(FALSE == this->IsEnableCreateMonster(pMonInfo))
	{
// 2008-11-11 by cmkwon, ÇĘżä ľřŔ˝ - 
//		char szTest[1024];
//		wsprintf(szTest, "  CreateRegionInfo MapIndexChannel(%d,%d) InfoIdx(%d) Mon(%8d) ResTime(%dĂĘ), dwTimeGap(%dĂĘ)\r\n"
//			, this->GetMapChannelIndex().MapIndex, this->GetMapChannelIndex().ChannelIndex, i_nArrIdx
//			, pMonsterCreateRegionInfo->sMonType, pMonsterCreateRegionInfo->sResTime, dwTimeGap/1000);
//		g_pNPCGlobal->WriteSystemLog(szTest);
//		DBGOUT(szTest);
		return;
	}

	int curMonsters = NPCGetMonsterCountInRegion(pMonsterCreateRegionInfo->sStartx, pMonsterCreateRegionInfo->sStartz,
		pMonsterCreateRegionInfo->sEndx, pMonsterCreateRegionInfo->sEndz, pMonsterCreateRegionInfo->sMonType,
		pMonsterCreateRegionInfo->sMaxMon);
	creatMonsters = pMonsterCreateRegionInfo->sMaxMon - curMonsters;
	if(creatMonsters <= 0)
	{// »ýĽş ¸ó˝şĹÍ Ľö°ˇ ŔŻČżÇĎÁö ľĘŔ˝

		return;
	}
	
	creatMonsters = creatMonsters > pMonsterCreateRegionInfo->sResNum ? pMonsterCreateRegionInfo->sResNum : creatMonsters;

	if(0 >= creatMonsters)
	{// 2010-05-28 by cmkwon, ĽŇČŻ ¸¶¸®Ľö 0ŔĎ¶§ NPCServer Á×´Â ąö±× ĽöÁ¤ - 
		return;
	}


	creatMonsters = rand()%creatMonsters + 1;
	NPCGetCreatablePosition(pMonInfo->MonsterForm, pMonInfo->Size, pMonsterCreateRegionInfo->sStartx, pMonsterCreateRegionInfo->sStartz,
		pMonsterCreateRegionInfo->sEndx, pMonsterCreateRegionInfo->sEndz, 
		m_pNPCMapProject->m_sMinimumAltitude, m_pNPCMapProject->m_sMaximumAltitude
		, *pVECTOR2vector, creatMonsters);

	mt_auto_lock mtMon(&m_mtvectorMonsterPtr);
	while(creatMonsters > 0 && false == pVECTOR2vector->empty())
	{
		creatMonsters--;
		if(m_vectorUsableMonsterIndex.empty())
		{
			char	szError[1024];
			sprintf(szError, "[Error] CNPCMapChannel::CreateMonstersBYRegion_2 Monster is full. MaxMonsterCount[%d], CurrentMonsterCount(%d)\r\n"
				, m_nMaxMonsterCountInChannel, m_nCurMonsterCountInChannel);
			g_pNPCGlobal->WriteSystemLog(szError);
			DbgOut(szError);
			return;
		}
		ClientIndex_t nMonsterIndex = m_vectorUsableMonsterIndex.front();
		m_vectorUsableMonsterIndex.pop_front();
		pNMonster = GetNPCMonster(nMonsterIndex);
		if(NULL == pNMonster 
			|| pNMonster->m_enMonsterState != MS_NULL)
		{
			continue;
		}
		m_nCurMonsterCountInChannel++;				// Current Counts¸¦ Áő°ˇ
		pRegionInfoEX->nCreatedCount++;				// żµżŞŔÇ ´©Ŕű »ýĽş Counts¸¦ Áő°ˇ
		pRegionInfoEX->nCurrentCount++;				// żµżŞŔÇ Current »ýĽş Counts¸¦ Áő°ˇ

		posVector3 = pVECTOR2vector->back();
		pVECTOR2vector->pop_back();
	
		////////////////////////////////////////////////////////////////////////
		// 2009-09-09 ~ 2010 by dhjin, ŔÎÇÇ´ĎĆĽ - HPAction ·Îµů
		vectHPAction * pvectHPAction = m_pNPCIOCPServer->m_mapHPAction.findEZ_ptr(pMonInfo->HPActionIdx);
		if(NULL != pvectHPAction) {
			pNMonster->m_HPAction.InitHPActionListByDB(pvectHPAction);
		}
		// ¸ó˝şĹÍ »ýĽş, ±âş»°Ş ĽĽĆĂ
		pNMonster->SetCurrentMapChannel(this);				// 2009-09-09 ~ 2010 by dhjin, ŔÎÇÇ´ĎĆĽ - ¸Ę Ă¤łÎ ąĚ¸® ĽłÁ¤
		pNMonster->CreateNPCMonster(pMonInfo, &posVector3, i_dwCurrentTick, i_nArrIdx
			, MONSTER_TARGETTYPE_NORMAL, 0, 0, this->GetMonsterVisibleDiameterW());
		SetInitialPositionAndSendCreateMonster(pNMonster, pSendBuf);

	}
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			void CNPCMapChannel::CreateMonstersBYObject(BYTE *pSendBuf, OBJECTINFOSERVER *pObjInfo, DWORD i_dwCurrentTick, BOOL i_bMustCreate/*=FALSE*/)
/// \brief		
/// \author		cmkwon
/// \date		2006-11-22 ~ 2006-11-22
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CNPCMapChannel::CreateMonstersBYObject(BYTE *pSendBuf, OBJECTINFOSERVER *pObjInfo, DWORD i_dwCurrentTick, BOOL i_bMustCreate/*=FALSE*/)
{
	if (pObjInfo->m_EventInfo.m_byObjectMonsterCreated)
	{// ŔĚąĚ »ýĽşµČ  °ćżě
		return;
	}

	// start 2011-06-02 ŔÎÇÇ´ĎĆĽ 3Â÷ - ˝şĹÜ 6 - ÁÖ±âŔű ĽŇČŻ ±â´É Á¦ŔŰ
	MONSTER_BALANCE_DATA *pMonsterBalanceInfo = NULL;

	if (TRUE == IS_MAP_INFLUENCE_INFINITY(this->GetMapInfluenceTypeW()))		// ŔÎÇÇ´ĎĆĽ ŔĚ°í
	{
		if (EVENT_TYPE_OBJECT_MONSTER == pObjInfo->m_EventInfo.m_bEventType)		// ObjectMonster Position Information Object ŔĚ¸éĽ­
		{
			if (TRUE == pObjInfo->m_bNotCreateMonster)		// m_bNotCreateMonster TRUE ¸é
			{
				return;		// ĽŇČŻ ±ÝÁö
			}

			pMonsterBalanceInfo = &pObjInfo->MonsterBalanceInfo;
		}
	}
#ifdef _INET_BOSS_DB_CONTROL
	for(auto &el : m_pNPCIOCPServer->vectMonsters)
	{
		if(el.nMonsterUID == pObjInfo->m_pMonsterInfo->MonsterUnitKind && el.bSpawned == false && el.MapIndex == this->m_MapChannelIndex.MapIndex){
			short nRetMonBoss = m_pNPCMapProject->LoadDB_Boss_CheckSpawnAllow(pObjInfo->m_pMonsterInfo->MonsterUnitKind,this->m_MapChannelIndex.MapIndex);
			if(nRetMonBoss == 2) //2 means in table and not time passed for respawn
				return;
			
			if(nRetMonBoss == 100){
				el.bSpawned = true; //spawned
				i_bMustCreate = TRUE;
			}
		}
	}
#endif
	if (FALSE == i_bMustCreate)
	{
		// 2007-10-04 by cmkwon, Ŕü·«Ć÷ŔÎĆ®, ĹÚ·ąĆ÷Ć®´Â 
		if (IS_STRATEGYPOINT_MONSTER(pObjInfo->m_pMonsterInfo->Belligerence)
			|| IS_TELEPORT_MONSTER(pObjInfo->m_pMonsterInfo->Belligerence))
		{
			return;
		}

		if (pObjInfo->m_EventInfo.m_EventwParam3*TICK_CREATE_MONSTER_TERM > i_dwCurrentTick - pObjInfo->m_EventInfo.m_dwLastTimeObjectMonsterCreated)
		{// ¸®Á¨ Ĺ¸ŔÓŔĚ ÁöłŞÁö ľĘŔ˝
			return;
		}

		if (m_uiLimitMonsterCountsInChannel <= m_nCurMonsterCountInChannel)
		{// ¸ĘŔÇ ĂÖ´ë ¸ó˝şĹÍ Ä«żîĆ® ş¸´Ů Ĺ©¸é
			return;
		}
	}


	// 2007-08-18 by cmkwon, żŔşęÁ§Ć® ¸ó˝şĹÍ ĽŇČŻ Á¤ş¸żˇ MONSTER_INFO * ĽłÁ¤ÇĎ±â - ąĚ¸® ĽłŔúÇĎż´±â ¶§ą®żˇ NULL ĂĽĹ©¸¸ ÇŃ´Ů
	//	MONSTER_INFO *pMonInfo = m_pNPCIOCPServer->GetMonsterInfo(pObjInfo->m_EventInfo.m_nObejctMonsterUnitKind);
	MONSTER_INFO *pMonInfo = pObjInfo->m_pMonsterInfo;	// 2007-08-18 by cmkwon, żŔşęÁ§Ć® ¸ó˝şĹÍ ĽŇČŻ Á¤ş¸żˇ MONSTER_INFO * ĽłÁ¤ÇĎ±â
	if (NULL == pMonInfo)
	{
		char	szError[1024];
		sprintf(szError, "[Error] CNPCMapChannel::CreateMonstersBYObject_1 invalid MonsterType, MAP(%4d) ObjectMonsterInfoIndex[%2d] MonsterType[%6d]\r\n"
			, this->m_MapChannelIndex.MapIndex, pObjInfo->m_EventInfo.m_EventwParam1, pObjInfo->m_EventInfo.m_nObejctMonsterUnitKind);
		g_pNPCGlobal->WriteSystemLog(szError);
		DbgOut(szError);
		return;
	}

	//////////////////////////////////////////////////////////////////////////
	// 2007-08-29 by dhjin, ¸ó˝şĹÍ »ýĽş ±ÝÁöŔĚ¸é »ýĽşÇĎÁö ľĘ´Â´Ů. ÁÖŔÇ~!! ĽĽ·Â ¸ó˝şĹÍ´Â Á¦żÜŔĚ´Ů.
	if (m_bNotCreateMonster
		&& (BELL_INFLUENCE_VCN != pMonInfo->Belligerence)
		&& (BELL_INFLUENCE_ANI != pMonInfo->Belligerence)
		&& (BELL_STRATEGYPOINT_VCN != pMonInfo->Belligerence)
		&& (BELL_STRATEGYPOINT_ANI != pMonInfo->Belligerence)
		&& (BELL_INFLUENCE_TELEPORT_VCN != pMonInfo->Belligerence)
		&& (BELL_INFLUENCE_TELEPORT_ANI != pMonInfo->Belligerence)
		&& !IS_ONEY_ATTACK_MONSTER(pMonInfo->Belligerence)) // 2010-07-06 by jskim, ±âż©µµ ľř´Â ĽĽ·Â ¸ó˝şĹÍ Ăß°ˇ
	{
		pObjInfo->m_EventInfo.m_dwLastTimeObjectMonsterCreated = i_dwCurrentTick;		// 2009-10-15 by cmkwon, ĽĽ·Â¸ĘŔÇ ľČŔüĂ¤łÎżˇ ĽĽ·Â¸ó˝şĹÍ ĽŇČŻ ¸·±â - »ýĽş ˝Ă°ŁŔ» ĽłÁ¤ÇŃ´Ů.
		return;
	}

	// 2009-10-15 by cmkwon, ĽĽ·Â¸ĘŔÇ ľČŔüĂ¤łÎżˇ ĽĽ·Â¸ó˝şĹÍ ĽŇČŻ ¸·±â - CNPCMapChannel::CreateMonstersBYObject#, 
	if ((IS_MAP_INFLUENCE_VCN(GetMapInfluenceTypeW()) || IS_MAP_INFLUENCE_ANI(GetMapInfluenceTypeW()))
		&& 0 != m_MapChannelIndex.ChannelIndex
		&& IS_INFLWAR_MONSTER(pMonInfo->Belligerence))
	{
		pObjInfo->m_EventInfo.m_dwLastTimeObjectMonsterCreated = i_dwCurrentTick;		// 2009-10-15 by cmkwon, ĽĽ·Â¸ĘŔÇ ľČŔüĂ¤łÎżˇ ĽĽ·Â¸ó˝şĹÍ ĽŇČŻ ¸·±â - »ýĽş ˝Ă°ŁŔ» ĽłÁ¤ÇŃ´Ů.
		return;
	}

	///////////////////////////////////////////////////////////////////////////////
	// 2006-11-23 by cmkwon
	pObjInfo->m_EventInfo.m_dwLastTimeObjectMonsterCreated = i_dwCurrentTick;
	// 2009-09-09 ~ 2010 by dhjin, ŔÎÇÇ´ĎĆĽ - ŔÎÇÇ ¸ĘżˇĽ± ¸®Á¨ ˝Ă°ŁŔĚ Ăą ĽŇČŻ˝Ă°ŁŔĚ°í ÇŃ ąř ĽŇČŻµČ ¸ó˝şĹÍ´Â ´ő ŔĚ»ó ĽŇČŻ˝ĂĹ°Áö ľĘ´Â´Ů.
	//	if(IS_MAP_INFLUENCE_INFINITY(this->GetMapInfluenceTypeW()))	{
	//		pObjInfo->m_EventInfo.m_dwLastTimeObjectMonsterCreated = TICK_CREATE_MONSTER_TERM_ONLY_INFINITY;
	//	}

	///////////////////////////////////////////////////////////////////////////////
	// 2006-04-18 by cmkwon
	if (FALSE == this->IsEnableCreateMonster(pMonInfo))
	{
		// 2008-11-11 by cmkwon, ÇĘżä ľřŔ˝ - 
		//		char szTest[1024];
		//		wsprintf(szTest, "  CreateRegionInfo MapChannel(%s) ObjectMonsterInfoIndex[%2d] MonsterType[%6d]\r\n"
		//			, GET_MAP_STRING(this->GetMapChannelIndex()), pObjInfo->m_EventInfo.m_EventwParam1, pObjInfo->m_EventInfo.m_nObejctMonsterUnitKind);
		//		g_pNPCGlobal->WriteSystemLog(szTest);
		//		DBGOUT(szTest);
		return;
	}

	mt_auto_lock mtMon(&m_mtvectorMonsterPtr);
	if (m_vectorUsableMonsterIndex.empty())
	{
		char	szError[1024];
		sprintf(szError, "[Error] CNPCMapChannel::CreateMonstersBYObject_2 Monster is full. MaxMonsterCount[%d], CurrentMonsterCount(%d)\r\n"
			, m_nMaxMonsterCountInChannel, m_nCurMonsterCountInChannel);
		g_pNPCGlobal->WriteSystemLog(szError);
		DbgOut(szError);
		return;
	}
	ClientIndex_t nMonsterIndex = m_vectorUsableMonsterIndex.front();
	m_vectorUsableMonsterIndex.pop_front();
	CNPCMonster *pNMonster = GetNPCMonster(nMonsterIndex);
	if (NULL == pNMonster
		|| pNMonster->m_enMonsterState != MS_NULL)
	{
		return;
	}
	m_nCurMonsterCountInChannel++;				// Current Counts¸¦ Áő°ˇ
	pObjInfo->m_EventInfo.m_byObjectMonsterCreated = TRUE;

	////////////////////////////////////////////////////////////////////////
	// 2009-09-09 ~ 2010 by dhjin, ŔÎÇÇ´ĎĆĽ - HPAction ·Îµů
	vectHPAction * pvectHPAction = m_pNPCIOCPServer->m_mapHPAction.findEZ_ptr(pMonInfo->HPActionIdx);
	if (NULL != pvectHPAction) {
		pNMonster->m_HPAction.InitHPActionListByDB(pvectHPAction);
	}
	// ¸ó˝şĹÍ »ýĽş, ±âş»°Ş ĽĽĆĂ
#ifdef _INET_LEVELING_SP
	MAP_CHANNEL_INDEX tmp_MapChanBCU;
	MAP_CHANNEL_INDEX tmp_MapChanANI;
	tmp_MapChanBCU.MapIndex = 3003;
	tmp_MapChanBCU.ChannelIndex = 0;
	tmp_MapChanANI.MapIndex = 3018;
	tmp_MapChanANI.ChannelIndex = 0;
	bool bIsMS = false;
	if(m_pNPCIOCPServer->GetNPCMapChannelByMapChannelIndex(tmp_MapChanBCU) && m_pNPCIOCPServer->GetNPCMapChannelByMapChannelIndex(tmp_MapChanBCU)->m_bNotCreateMonster == TRUE//is MS now for BCU
		|| m_pNPCIOCPServer->GetNPCMapChannelByMapChannelIndex(tmp_MapChanANI) && m_pNPCIOCPServer->GetNPCMapChannelByMapChannelIndex(tmp_MapChanANI)->m_bNotCreateMonster == TRUE) //is ANI MS 
	{
		bIsMS = true;
	}
	time_t now = time(nullptr);
	auto nDiff = now - LastMSSpawned;
	int nLevelRand = rand32() % 8;
	if ((bIsMS && IS_STRATEGYPOINT_MONSTER(pMonInfo->Belligerence)) 
		|| (IS_STRATEGYPOINT_MONSTER(pMonInfo->Belligerence) && (pMonInfo->MonsterUnitKind == 2052000 || pMonInfo->MonsterUnitKind == 2052100))) { //02-02-2022 by Inet
		
		auto nSummonLevelBCU = 0;
		auto nSummonLevelANI = 0;
		char	szSystemLog[256];
		CMonsterDBAccess MonsterDBAccess;
		auto const nBCUContrib = MonsterDBAccess.GetInfluenceContributtionPoints(2);
		auto const nANIContrib = MonsterDBAccess.GetInfluenceContributtionPoints(4);
		
		sprintf_s(szSystemLog, "[INET] Get contibuttion Points ANI(%d) BCU(%d) for SPNUM(%d)\r\n", nANIContrib, nBCUContrib,pMonInfo->MonsterUnitKind);

		g_pNPCGlobal->WriteSystemLog(szSystemLog);
		if (nDiff > 60)
		{
			if (nBCUContrib > nANIContrib) { //BCU Wins

				auto const Diff1 = nBCUContrib - nANIContrib;

				if (Diff1 >= 0 && Diff1 < 10000) {
					nSummonLevelBCU = 5;
					nSummonLevelANI = 5;
				}
				else if (Diff1 >= 10000 && Diff1 < 20000) {
					nSummonLevelBCU = 4;
					nSummonLevelANI = 6;
				}
				else if (Diff1 >= 20000 && Diff1 < 30000) {
					nSummonLevelBCU = 3;
					nSummonLevelANI = 7;
				}
				else if (Diff1 >= 30000 && Diff1 < 40000) {
					nSummonLevelBCU = 2;
					nSummonLevelANI = 8;
				}
				else if (Diff1 >= 40000) {
					nSummonLevelBCU = 1;
					nSummonLevelANI = 9;
				}
			}
			else { //ANI Wins

				auto const Diff2 = nANIContrib - nBCUContrib;

				if (Diff2 >= 0 && Diff2 < 10000) {
					nSummonLevelBCU = 5;
					nSummonLevelANI = 5;
				}
				else if (Diff2 >= 10000 && Diff2 < 20000) {
					nSummonLevelANI = 4;
					nSummonLevelBCU = 6;
				}
				else if (Diff2 >= 20000 && Diff2 < 30000) {
					nSummonLevelANI = 3;
					nSummonLevelBCU = 7;
				}
				else if (Diff2 >= 30000 && Diff2 < 40000) {
					nSummonLevelANI = 2;
					nSummonLevelBCU = 8;
				}
				else if (Diff2 >= 40000) {
					nSummonLevelANI = 1;
					nSummonLevelBCU = 9;
				}
			}
		}
		else
		{
			switch (nLastMSMonsterUID)
			{//spawn same level sp as MS
				case 2087800:
					nSummonLevelANI = 1;
					break;
				case 2087700:
					nSummonLevelANI = 2;
					break;
				case 2087600:
					nSummonLevelANI = 3;
					break;
				case 2087500:
					nSummonLevelANI = 4;
					break;
				case 2027300:
					nSummonLevelANI = 5;
					break;
				case 2057300:
					nSummonLevelANI = 6;
					break;
				case 2057400:
					nSummonLevelANI = 7;
					break;
				case 2057500:
					nSummonLevelANI = 8;
					break;
				case 2057600:
					nSummonLevelANI = 9;
					break;
				case 2087400:
					nSummonLevelBCU = 1;
					break;
				case 2087300:
					nSummonLevelBCU = 2;
					break;
				case 2087200:
					nSummonLevelBCU = 3;
					break;
				case 2087100:
					nSummonLevelBCU = 4;
					break;
				case 2027200:
					nSummonLevelBCU = 5;
					break;
				case 2056900:
					nSummonLevelBCU = 6;
					break;
				case 2057000:
					nSummonLevelBCU = 7;
					break;
				case 2057100:
					nSummonLevelBCU = 8;
					break;
				case 2057200:
					nSummonLevelBCU = 9;
					break;
				default:
				{
					nSummonLevelANI = 0;
					nSummonLevelBCU = 0;
				}
				break;
			}
		}
		
		if(!bIsMS){
			if (pObjInfo->m_pMonsterInfo->Belligerence == BELL_STRATEGYPOINT_VCN)
				pObjInfo->m_pMonsterInfo->MonsterUnitKind = 2052000 + nSummonLevelBCU;
			else if (pObjInfo->m_pMonsterInfo->Belligerence == BELL_STRATEGYPOINT_ANI)
				pObjInfo->m_pMonsterInfo->MonsterUnitKind = 2052100 + nSummonLevelANI;
				
			/*
			if (pObjInfo->m_pMonsterInfo->Belligerence == BELL_STRATEGYPOINT_VCN)
				pObjInfo->m_pMonsterInfo->MonsterUnitKind = 2052001 + nLevelRand;
			else if (pObjInfo->m_pMonsterInfo->Belligerence == BELL_STRATEGYPOINT_ANI)
				pObjInfo->m_pMonsterInfo->MonsterUnitKind = 2052101 + nLevelRand;
				*/
			sprintf_s(szSystemLog, "[INET] Monster Create SP,	MONSTER(%d) at Map(%s)\r\n"
			, pMonInfo->MonsterUnitKind, GET_MAP_STRING(this->m_MapChannelIndex));
		
		}
		else
		{
			

			if (pObjInfo->m_pMonsterInfo->Belligerence == BELL_STRATEGYPOINT_VCN)
				pObjInfo->m_pMonsterInfo->MonsterUnitKind = 2052005; //level 5 sp
			else if (pObjInfo->m_pMonsterInfo->Belligerence == BELL_STRATEGYPOINT_ANI)
				pObjInfo->m_pMonsterInfo->MonsterUnitKind = 2052105; //level 5 sp

			sprintf_s(szSystemLog, "[INET] Object Monster Create SP (!There is MS war!),	MONSTER(%d) at Map(%s) SummonLevelANI(%d) SummonLevelBCU(%d)\r\n"
				, pObjInfo->m_pMonsterInfo->MonsterUnitKind, GET_MAP_STRING(this->m_MapChannelIndex), nSummonLevelANI, nSummonLevelBCU);
		}
				
		g_pNPCGlobal->WriteSystemLog(szSystemLog);
	}
#endif
		pNMonster->SetCurrentMapChannel(this);				// 2009-09-09 ~ 2010 by dhjin, ŔÎÇÇ´ĎĆĽ - ¸Ę Ă¤łÎ ąĚ¸® ĽłÁ¤
		pNMonster->CreateNPCMonster(pMonInfo, &pObjInfo->m_vPos, i_dwCurrentTick, pObjInfo->m_EventInfo.m_EventwParam1
			, MONSTER_TARGETTYPE_NORMAL, 0, 0, this->GetMonsterVisibleDiameterW(), 0, pObjInfo->m_EventInfo.m_bEventType, &pObjInfo->m_vVel, pMonsterBalanceInfo);		// 2011-06-02 ŔÎÇÇ´ĎĆĽ 3Â÷ - ˝şĹÜ 6 - ÁÖ±âŔű ĽŇČŻ ±â´É Á¦ŔŰ				
		SetInitialPositionAndSendCreateMonster(pNMonster, pSendBuf);
	
	////////////////////////////////////////////////////////////////////////
	// 2009-09-09 ~ 2010 by dhjin, ŔÎÇÇ´ĎĆĽ
}

void CNPCMapChannel::UpdateMonsterPositionAllMonster(BYTE *pSendBuf,
											  vector<ClientIndex_t> *pvecClientIndex,
											  DWORD i_dwCurrentTick)
{
	CNPCMonster		*ptmpNPCMonster = NULL;
	for (int nIdxMonsterArray = 0; nIdxMonsterArray < m_nSizemtvectorMonsterPtr; nIdxMonsterArray++)	
	{		
		ptmpNPCMonster = &m_ArrNPCMonster[nIdxMonsterArray];
		switch(ptmpNPCMonster->m_enMonsterState)
		{
		case MS_NULL:
			break;
		case MS_PLAYING:
			{
				///////////////////////////////////////////////////////////////////////////////
				// 2010-03-31 by dhjin, ŔÎÇÇ´ĎĆĽ(±âÁöąćľî) -
				ptmpNPCMonster->CheckExpireSkill();

				if(i_dwCurrentTick - ptmpNPCMonster->m_dwTimeLastMoved >= MONSTER_UPDATE_MOVE_TERM_TICK)
				{	// ¸ó˝şĹÍŔÇ ŔĚµż TermŔĚ łµ´Ů

					ptmpNPCMonster->SetCurrentTick(i_dwCurrentTick);

					///////////////////////////////////////////////////////////
					// HP Č¸şą Ăł¸®
					if(ptmpNPCMonster->MonsterInfoPtr->HPRecoveryValue
						&& i_dwCurrentTick - ptmpNPCMonster->m_dwLastHPRecoveryTime > ptmpNPCMonster->MonsterInfoPtr->HPRecoveryTime)
					{
						ptmpNPCMonster->m_dwLastHPRecoveryTime = i_dwCurrentTick;

						// 2010. 05. 19 by hsLee ŔÎÇÇ´ĎĆĽ ÇĘµĺ 2Â÷ ł­ŔĚµµ Á¶Ŕý. (˝ĹČŁĂł¸® + ¸ó˝şĹÍ Ăł¸®(Ľ­ąö) )
						//if(ptmpNPCMonster->MonsterInfoPtr->MonsterHP > ptmpNPCMonster->CurrentHP)
						if ( ptmpNPCMonster->MonsterInfoExtend.fMaxHP > ptmpNPCMonster->CurrentHP )
						{
							INIT_MSG(MSG_FN_MONSTER_HPRECOVERY, T_FN_MONSTER_HPRECOVERY, pSendHPRecovery, pSendBuf);
							pSendHPRecovery->ChannelIndex	= m_MapChannelIndex.ChannelIndex;
							pSendHPRecovery->MonsterIndex	= ptmpNPCMonster->MonsterIndex;

							// 2010. 05. 19 by hsLee ŔÎÇÇ´ĎĆĽ ÇĘµĺ 2Â÷ ł­ŔĚµµ Á¶Ŕý. (˝ĹČŁĂł¸® + ¸ó˝şĹÍ Ăł¸®(Ľ­ąö) )
							//pSendHPRecovery->RecoveryHP		= min(ptmpNPCMonster->MonsterInfoPtr->HPRecoveryValue, ptmpNPCMonster->MonsterInfoPtr->MonsterHP - ptmpNPCMonster->CurrentHP);

							// 2010. 05. 31 by hsLee ŔÎÇÇ´ĎĆĽ ÇĘµĺ 2Â÷ ł­ŔĚµµ Á¶Ŕý. (żŔşęÁ§Ć® ¸ó˝şĹÍ ąë·±˝ş Ŕűżë ą®Á¦ ĽöÁ¤.) - ¸ó˝şĹÍ HP Č¸şą·Â ąë·±˝ş Á¶Á¤.
							pSendHPRecovery->RecoveryHP		= min(ptmpNPCMonster->MonsterInfoPtr->HPRecoveryValue * ptmpNPCMonster->MonsterInfoBalance.fMaxHPRatio , ptmpNPCMonster->MonsterInfoExtend.fMaxHP - ptmpNPCMonster->CurrentHP);
							// End 2010. 05. 31 by hsLee ŔÎÇÇ´ĎĆĽ ÇĘµĺ 2Â÷ ł­ŔĚµµ Á¶Ŕý. (żŔşęÁ§Ć® ¸ó˝şĹÍ ąë·±˝ş Ŕűżë ą®Á¦ ĽöÁ¤.)
							// End 2010. 05. 19 by hsLee ŔÎÇÇ´ĎĆĽ ÇĘµĺ 2Â÷ ł­ŔĚµµ Á¶Ŕý. (˝ĹČŁĂł¸® + ¸ó˝şĹÍ Ăł¸®(Ľ­ąö) )

							Send2FieldServerW(pSendBuf, MSG_SIZE(MSG_FN_MONSTER_HPRECOVERY));
						}
					}

					///////////////////////////////////////////////////////////
					// ¸ó˝şĹÍ ŔĚµż Ăł¸®
					// 1. Á¤»óŔűŔÎ ŔĚµż Ăł¸®
					//		- TargetŔĚ ŔÖ°ĹłŞ Č¤Ŕş »ýĽşµÇ°í ŔĚµżľřŔĚ ŔĎÁ¤ ˝Ă°Ł(ŔĎąÝ ¸ó˝şĹÍżÍ ŔĚşĄĆ® ¸ó˝şĹÍ µÎ°ˇÁö·Î łŞ´®)ŔĚ Áöł­ »óĹÂ
					// 2. ŔĚµżľřŔĚ Move¸¸ ŔüĽŰ
					if(ptmpNPCMonster->m_nTargetIndex != 0
						|| i_dwCurrentTick - ptmpNPCMonster->m_dwTimeCreated > ((COMPARE_MPOPTION_BIT(ptmpNPCMonster->MonsterInfoPtr->MPOption, MPOPTION_BIT_PATTERN_MONSTER))? MONSTER_EVENT_MON_NOT_MOVE_AFTER_CREATED_TERM_TICK : MONSTER_NOT_MOVE_AFTER_CREATED_TERM_TICK))
					{
						if(COMPARE_BODYCON_BIT(ptmpNPCMonster->BodyCondition, BODYCON_CREATION_MASK))
						{
							BodyCond_t tmBody = ptmpNPCMonster->BodyCondition;
							CLEAR_BODYCON_BIT(tmBody, BODYCON_CREATION_MASK);
							if(FORM_GROUND_MOVE == ptmpNPCMonster->CurrentMonsterForm)
							{
								SET_BODYCON_BIT(tmBody, BODYCON_LANDED_MASK);
							}
							else
							{
								SET_BODYCON_BIT(tmBody, BODYCON_FLY_MASK|BODYCON_BOOSTER1_MASK);	
							}
							ptmpNPCMonster->ChangeBodyCondition(&tmBody);

							INIT_MSG(MSG_FN_MONSTER_CHANGE_BODYCONDITION, T_FN_MONSTER_CHANGE_BODYCONDITION, pSeBody, pSendBuf);
							pSeBody->ChannelIndex	= this->m_MapChannelIndex.ChannelIndex;
							pSeBody->ClientIndex	= ptmpNPCMonster->MonsterIndex;
							pSeBody->BodyCondition	= ptmpNPCMonster->BodyCondition;
							Send2FieldServerW(pSendBuf, MSG_SIZE(MSG_FN_MONSTER_CHANGE_BODYCONDITION));
						}
						NPCMonsterMPOption(ptmpNPCMonster);						
						UpdateMonsterPositionHandler(ptmpNPCMonster, pSendBuf, pvecClientIndex, FALSE);
					}
					else
					{
						UpdateMonsterPositionHandler(ptmpNPCMonster, pSendBuf, pvecClientIndex, TRUE);
					}
				}// end_if(i_dwCurrentTick - ptmpNPCMonster->m_dwTimeLastMoved >= MONSTER_UPDATE_MOVE_TERM_TICK)


				if(i_dwCurrentTick - ptmpNPCMonster->m_dwTimeLastAttackRoutine > MONSTER_MIN_ATTACK_TERM_TICK
					&& ptmpNPCMonster->m_nTargetIndex >= CLIENT_INDEX_START_NUM)
				{	// °ř°ÝÇŃ ˝Ă°ŁŔĚ ĂÖĽŇ °ř°Ý˝Ă°Ł ŔĚ»ó Áöłµ°í ¸ó˝şĹÍ°ˇ °ř°ÝÇĎ°íŔÚÇĎ´Â Ĺ¸ÄĎŔĚ ŔÖŔ˝ 
					
					ptmpNPCMonster->m_dwTimeLastAttackRoutine = i_dwCurrentTick;
					AttackMonster2Unit(ptmpNPCMonster, pSendBuf);					
				}

				///////////////////////////////////////////////////////////////////////////////
				// 2005-10-27 by cmkwon, ŔÚµż ĽŇ¸ę ¸ó˝şĹÍ ĂĽĹ©
				if(COMPARE_MPOPTION_BIT(ptmpNPCMonster->MonsterInfoPtr->MPOption, MPOPTION_BIT_AUTO_DESTROY))
				{
					DWORD dwLiveTime = ptmpNPCMonster->MonsterInfoPtr->MPOptionParam1*60*1000;
					if(dwLiveTime < i_dwCurrentTick-ptmpNPCMonster->m_dwTimeCreated)
					{
						BodyCond_t tmBodyCon;
						MEMSET_ZERO(&tmBodyCon, sizeof(tmBodyCon));
						SET_BODYCON_BIT(tmBodyCon, BODYCON_MON_AUTODESTROYED_MASK);
						ptmpNPCMonster->ChangeBodyCondition(&tmBodyCon);

// 2007-11-26 by cmkwon, ¸ó˝şĹÍ ŔÚµż»čÁ¦ ¸Ţ˝ĂÁö TCP·Î ŔüĽŰ(N->F) - ľĆ·ˇżÍ °°ŔĚ TCP·Î ŔüĽŰ
//						INIT_MSG(MSG_FN_MONSTER_CHANGE_BODYCONDITION, T_FN_MONSTER_CHANGE_BODYCONDITION, pSeBody, pSendBuf);
//						pSeBody->ChannelIndex	= this->m_MapChannelIndex.ChannelIndex;
//						pSeBody->ClientIndex	= ptmpNPCMonster->MonsterIndex;
//						pSeBody->BodyCondition	= ptmpNPCMonster->BodyCondition;
//						Send2FieldServerW(pSendBuf, MSG_SIZE(MSG_FN_MONSTER_CHANGE_BODYCONDITION));
//
//						///////////////////////////////////////////////////////////////////////////////
//						// 2006-04-17 by cmkwon, ŔÚµż ĽŇ¸ę Á¤ş¸¸¦ FieldServer·Î ŔüĽŰÇŃ´Ů.
//						INIT_MSG(MSG_FN_MONSTER_AUTO_DESTROYED, T_FN_MONSTER_AUTO_DESTROYED, pSDestroyed, pSendBuf);
//						pSDestroyed->ChannelIndex	= this->GetMapChannelIndex().ChannelIndex;
//						pSDestroyed->MonsterIndex	= ptmpNPCMonster->MonsterIndex;
//						Send2FieldServerW(pSendBuf, MSG_SIZE(MSG_FN_MONSTER_AUTO_DESTROYED));
//
//						///////////////////////////////////////////////////////////////////////////////
//						// 2006-11-20 by cmkwon, ąŮ·Î »čÁ¦Ăł¸®ÇŃ´Ů.
//						INIT_MSG(MSG_FN_MONSTER_DELETE, T_FN_MONSTER_DELETE, pMonsterDelete, pSendBuf);
//						pMonsterDelete->ChannelIndex	= m_MapChannelIndex.ChannelIndex;
//						pMonsterDelete->MonsterIndex	= ptmpNPCMonster->MonsterIndex;
//						Send2FieldServerW(pSendBuf, MSG_SIZE(MSG_FN_MONSTER_DELETE));

						///////////////////////////////////////////////////////////////////////////////
						// 2006-04-17 by cmkwon, ŔÚµż ĽŇ¸ę Á¤ş¸¸¦ FieldServer·Î ŔüĽŰÇŃ´Ů.
						// 2007-11-26 by cmkwon, ¸ó˝şĹÍ ŔÚµż»čÁ¦ ¸Ţ˝ĂÁö TCP·Î ŔüĽŰ(N->F) - ľĆ·ˇŔÇ ¸Ţ˝ĂÁö ÇĎłŞ¸¸ ŔüĽŰ, łŞ¸ÓÁö´Â FServerżˇĽ­ Ăł¸®
						INIT_MSG(MSG_FN_MONSTER_AUTO_DESTROYED, T_FN_MONSTER_AUTO_DESTROYED, pSDestroyed, pSendBuf);
						pSDestroyed->MapChannIdx	= this->GetMapChannelIndex();		// 2007-11-26 by cmkwon, ¸ó˝şĹÍ ŔÚµż»čÁ¦ ¸Ţ˝ĂÁö TCP·Î ŔüĽŰ(N->F) - 
						pSDestroyed->MonsterIndex	= ptmpNPCMonster->MonsterIndex;
						pSDestroyed->BodyCondition	= ptmpNPCMonster->BodyCondition;	// 2007-11-26 by cmkwon, ¸ó˝şĹÍ ŔÚµż»čÁ¦ ¸Ţ˝ĂÁö TCP·Î ŔüĽŰ(N->F) - 
						Send2FieldServerByTCPW(pSendBuf, MSG_SIZE(MSG_FN_MONSTER_AUTO_DESTROYED));

						DelelteMonsterHandler(ptmpNPCMonster);
					}
				}
			}
			break;
		case MS_DEAD:
			{
#ifdef _INET_BOSS_DB_CONTROL
			for(auto &el : m_pNPCIOCPServer->vectMonsters)
			{
				if(el.nMonsterUID == ptmpNPCMonster->MonsterInfoPtr->MonsterUnitKind && el.MapIndex == this->m_MapChannelIndex.MapIndex){
					el.bSpawned = false; //spawn only 1
					m_pNPCMapProject->UpdateLastSpawn(ptmpNPCMonster->MonsterInfoPtr->MonsterUnitKind,this->m_MapChannelIndex.MapIndex);
				}
			}
#endif
				ptmpNPCMonster->SetCurrentTick(i_dwCurrentTick);
				CLIENT_INFO* pAttackerCli = NULL;
				CNPCMonster* pAttackerNMon = NULL;
				float fIncrRate = 1.0f;
				if (FALSE == this->GetUnitObject(ptmpNPCMonster->m_nTargetIndex, &pAttackerCli, &pAttackerNMon))
				{
					//cannot get attacker idx from monster
				}
				else
				{
					//if have pattackercli then apply summon probability
					if (pAttackerCli != NULL)
					{
						fIncrRate = pAttackerCli->InfluenceType1 == INFLUENCE_TYPE_ANI ? m_pNPCMapProject->fEventMonsterProbRateANI : m_pNPCMapProject->fEventMonsterProbRateBCU;
					}
				}
				if(0 == ptmpNPCMonster->m_dwTimeDeath)
				{// 2005-12-17 by cmkwon, FieldServer·Î´Â »čÁ¦ ¸Ţ˝ĂÁö¸¦ ŔüĽŰÇŃ´Ů.
					ptmpNPCMonster->m_dwTimeDeath	= ptmpNPCMonster->m_dwCurrentTick;
					

					///////////////////////////////////////////////////////////////////////////////
					// 2008-04-16 by cmkwon, ¸ó˝şĹÍ »ç¸Á ˝Ă ¸ó˝şĹÍ ĽŇČŻ ŔĚşĄĆ® ˝Ă˝şĹŰ ±¸Çö - 
					mtvectSSUMMON_EVENT_MONSTER summonEvMonList;
					m_pNPCIOCPServer->GetSummonEventMonsterListAfterDead(&summonEvMonList, this->GetMapChannelIndex(), ptmpNPCMonster->MonsterInfoPtr);
					ptmpNPCMonster->SetSummonEventMonsterListAfterDead(&summonEvMonList);

					// Field Server·Î »čÁ¦ ¸ŢĽĽÁö¸¦ ŔüĽŰ
					INIT_MSG(MSG_FN_MONSTER_DELETE, T_FN_MONSTER_DELETE, pMonsterDelete, pSendBuf);
					pMonsterDelete->ChannelIndex	= m_MapChannelIndex.ChannelIndex;
					pMonsterDelete->MonsterIndex	= ptmpNPCMonster->MonsterIndex;
					pMonsterDelete->CinemaDelete	= FALSE;		// 2011-05-30 by hskim, ŔÎÇÇ´ĎĆĽ 3Â÷ - ¸ó˝şĹÍ »čÁ¦ Ĺ¬¶óŔĚľđĆ® ąÝżµ
					Send2FieldServerW(pSendBuf, MSG_SIZE(MSG_FN_MONSTER_DELETE));

					////////////////////////////////////////////////////////////////////////////////
					// 2009-09-09 ~ 2010-01-27 by dhjin, ŔÎÇÇ´ĎĆĽ - Success, Fail˝Ă ¸ó˝şĹÍ ĂĘ±âČ­, »ç¸Á˝Ăżˇ¸¸ ´ë»ç Ăâ·ÂÇĎµµ·Ď DelelteMonsterHandlerÇÔĽöżˇĽ­ ŔĚÂĘŔ¸·Î Ŕ§Äˇ şŻ°ć
					// 2010-03-09 by cmkwon, ŔÎÇÇ »ç¸Á˝Ă ŔüĽŰ¸Ţ˝ĂÁö Ăł¸® ąö±× ĽöÁ¤ - 1Č¸¸¸ ąßĽŰµÇ°Ô Ăł¸®
					if(ptmpNPCMonster->m_HPAction.CheckValidSizeTalkDead()) {
						HPACTION_TALK_HPRATE MsgTalk;
						MEMSET_ZERO(&MsgTalk, sizeof(HPACTION_TALK_HPRATE));
						if(ptmpNPCMonster->m_HPAction.GetTalkDead(&MsgTalk)) {
							// Á×Ŕ˝˝Ă ´ëČ­°ˇ ŔÖ´Ů¸é ŔüĽŰÇŃ´Ů.
							this->SendFSvrHPTalk(ptmpNPCMonster, &MsgTalk);
						}
					}
				}

				BOOL bDeleteProcess = TRUE;
				_BattleAttackOnMonsterDead(&bDeleteProcess, ptmpNPCMonster, fIncrRate);

// 2010-03-09 by cmkwon, ŔÎÇÇ »ç¸Á˝Ă ŔüĽŰ¸Ţ˝ĂÁö Ăł¸® ąö±× ĽöÁ¤ - Ŕ§żÍ ŔĚµżÇŘĽ­ 1Č¸¸¸ ąßĽŰµÇ°Ô Ăł¸®
// 				////////////////////////////////////////////////////////////////////////////////
// 				// 2009-09-09 ~ 2010-01-27 by dhjin, ŔÎÇÇ´ĎĆĽ - Success, Fail˝Ă ¸ó˝şĹÍ ĂĘ±âČ­, »ç¸Á˝Ăżˇ¸¸ ´ë»ç Ăâ·ÂÇĎµµ·Ď DelelteMonsterHandlerÇÔĽöżˇĽ­ ŔĚÂĘŔ¸·Î Ŕ§Äˇ şŻ°ć
// 				if(ptmpNPCMonster->m_HPAction.CheckValidSizeTalkDead()) {
// 					HPACTION_TALK_HPRATE MsgTalk;
// 					MEMSET_ZERO(&MsgTalk, sizeof(HPACTION_TALK_HPRATE));
// 					if(ptmpNPCMonster->m_HPAction.GetTalkDead(&MsgTalk)) {
// 						// Á×Ŕ˝˝Ă ´ëČ­°ˇ ŔÖ´Ů¸é ŔüĽŰÇŃ´Ů.
// 						this->SendFSvrHPTalk(ptmpNPCMonster, &MsgTalk);
// 					}
// 				}

				if(bDeleteProcess)
				{
//					char	szSystemLog[256];
//					sprintf(szSystemLog, "  %10s : => MonIndex[%4d] MonType[%d] CurMonCount[%d] TotalMonCount[%d]\r\n",
//						m_projectInfo.m_strProjectName, ptmpNPCMonster->MonsterIndex,
//						ptmpNPCMonster->MonsterUnitKind, m_nCurMonsterCount, m_nTotalMonsterCount);
//					g_pNPCGlobal->WriteSystemLog(szSystemLog);
//					DBGOUT(szSystemLog);


					// 2007-09-20 by cmkwon, ĹÚ·ąĆ÷Ć® ĽŇČŻ °ü·Ă ĽöÁ¤ - »čÁ¦ Ăł¸®ÇĎ±â Ŕüżˇ Á¤ş¸¸¦ °Ë»öÇŃ´Ů.
					OBJECTINFOSERVER *pTeleportObjMonSummonInfo = GetTeleportObjectMonsterSummonInfo(ptmpNPCMonster);
		
					// NPC ServerżˇĽ­ ¸ó˝şĹÍ¸¦ »čÁ¦ÇŃ´Ů.
					DelelteMonsterHandler(ptmpNPCMonster);

					// 2007-09-20 by cmkwon, ĹÚ·ąĆ÷Ć® ĽŇČŻ °ü·Ă ĽöÁ¤ - »čÁ¦ ČÄ ´Ů˝Ă ĽŇČŻ Ăł¸®¸¦ ÇŃ´Ů.
					if(pTeleportObjMonSummonInfo)
					{
						this->CreateMonstersBYObject(pSendBuf, pTeleportObjMonSummonInfo, i_dwCurrentTick, TRUE);
					}
				}
			}
			break;
		case MS_CREATED:
			{
				if(i_dwCurrentTick > ptmpNPCMonster->m_dwTimeCreated
					&& i_dwCurrentTick - ptmpNPCMonster->m_dwTimeCreated > MONSTER_DELETE_AFTER_CREATED_TERM_TICK)
				{	// NPCżˇĽ­ Field·Î »ýĽş żäĂ»Ŕ» ş¸łÂÁö¸¸ ŔŔ´äŔĚ 10ĂĘ°ˇ ľřŔ» °ćżě »čÁ¦ Ăł¸®
					
// 2008-11-11 by cmkwon, ÇĘżä ľřŔ˝ - 
// 					char	szSystemLog[256];
// 					sprintf(szSystemLog, "[ERROR] Monster Create Error,	MapName(%s) MonIndex[%4d] MonType[%d]\r\n"
// 						, GET_MAP_STRING(this->m_MapChannelIndex), ptmpNPCMonster->MonsterIndex
// 						, ptmpNPCMonster->MonsterInfoPtr->MonsterUnitKind);
// 					g_pNPCGlobal->WriteSystemLog(szSystemLog);
// 					DBGOUT(szSystemLog);
					

					if(IS_TUTORIAL_MAP_INDEX(this->m_MapChannelIndex.MapIndex)						// 2006-09-21 by cmkwon, Ć©Ĺä¸®ľó ¸ĘŔĎ °ćżě °čĽÓ ˝Ăµµ
						|| IS_MAP_INFLUENCE_OUTPOST(this->m_pMapProject->m_nMapInfluenceType)		// 2007-10-04 by cmkwon, ŔüÁř±âÁö ¸ĘŔĎ °ćżě °čĽÓ ˝Ăµµ
						|| COMPARE_MPOPTION_BIT(ptmpNPCMonster->MonsterInfoPtr->MPOption, MPOPTION_BIT_BOSS_MONSTER)
						|| IS_TELEPORT_MONSTER(ptmpNPCMonster->MonsterInfoPtr->Belligerence)			// 2007-10-01 by cmkwon, ĹÚ·ąĆ÷Ć® ¸ó˝şĹÍ °čĽÓ ˝Ăµµ Ăß°ˇ
						|| IS_OUTPOST_MONSTER(ptmpNPCMonster->MonsterInfoPtr->Belligerence)				// 2007-10-01 by cmkwon, ŔüÁř±âÁöŔü °ü·Ă ¸ó˝şĹÍ °čĽÓ ˝Ăµµ Ăß°ˇ
						|| IS_STRATEGYPOINT_MONSTER(ptmpNPCMonster->MonsterInfoPtr->Belligerence)		// 2006-11-21 by cmkwon, Ŕü·«Ć÷ŔÎĆ® ¸ó˝şĹÍ °čĽÓ ˝Ăµµ
						|| COMPARE_MPOPTION_BIT(ptmpNPCMonster->MonsterInfoPtr->MPOption, MPOPTION_BIT_KEY_MONSTER)		// 2009-09-09 ~ 2010 by dhjin, ŔÎÇÇ´ĎĆĽ - Ĺ° ¸ó˝şĹÍµµ »ýĽş µÇľîľß ÇŃ´Ů.
						|| COMPARE_MPOPTION_BIT(ptmpNPCMonster->MonsterInfoPtr->MPOption, MPOPTION_BIT_KEY_MONSTER_ALIVE_FOR_GAMECLEAR)		// 2010-03-31 by dhjin, ŔÎÇÇ´ĎĆĽ(±âÁöąćľî) -
						)
					{// 2006-08-09 by cmkwon, ş¸˝ş ¸ó˝şĹÍŔĎ °ćżě´Â »ýĽş ˝Ă°ŁŔ» ĂĘ±âČ­ ÇĎ°í ´Ů˝Ă ŔüĽŰÇŃ´Ů.
						ptmpNPCMonster->m_dwTimeCreated			= i_dwCurrentTick;

						INIT_MSG(MSG_FN_MONSTER_CREATE, T_FN_MONSTER_CREATE, pSMonsterCreate, pSendBuf);
						pSMonsterCreate->ChannelIndex			= this->m_MapChannelIndex.ChannelIndex;
						pSMonsterCreate->MonsterIndex			= ptmpNPCMonster->MonsterIndex;
						
							pSMonsterCreate->MonsterUnitKind = ptmpNPCMonster->MonsterInfoPtr->MonsterUnitKind;
						
						pSMonsterCreate->MonsterTargetType1		= ptmpNPCMonster->m_byMonsterTargetType;
						pSMonsterCreate->TargetTypeData1		= ptmpNPCMonster->m_nTargetTypeData;
						pSMonsterCreate->CltIdxForTargetType1	= ptmpNPCMonster->GetClientIndexForTargetType();
						pSMonsterCreate->BodyCondition			= ptmpNPCMonster->BodyCondition;
						pSMonsterCreate->PositionVector			= ptmpNPCMonster->PositionVector;
						pSMonsterCreate->TargetVector			= ptmpNPCMonster->TargetVector*1000.0f;
						pSMonsterCreate->ObjectMonsterType		= ptmpNPCMonster->m_byObjectMonsterType;

						// 2010. 05. 19 by hsLee ŔÎÇÇ´ĎĆĽ ÇĘµĺ 2Â÷ ł­ŔĚµµ Á¶Ŕý. (˝ĹČŁĂł¸® + ¸ó˝şĹÍ Ăł¸®(Ľ­ąö) )
						pSMonsterCreate->MonsterBalanceData		= ptmpNPCMonster->MonsterInfoBalance;
						// End 2010. 05. 19 by hsLee ŔÎÇÇ´ĎĆĽ ÇĘµĺ 2Â÷ ł­ŔĚµµ Á¶Ŕý. (˝ĹČŁĂł¸® + ¸ó˝şĹÍ Ăł¸®(Ľ­ąö) )

						Send2FieldServerW(pSendBuf, MSG_SIZE(MSG_FN_MONSTER_CREATE));
					}
					else
					{
						// Field Server·Î »čÁ¦ ¸ŢĽĽÁö¸¦ ŔüĽŰ
						INIT_MSG(MSG_FN_MONSTER_DELETE, T_FN_MONSTER_DELETE, pMonsterDelete, pSendBuf);
						pMonsterDelete->ChannelIndex	= m_MapChannelIndex.ChannelIndex;
						pMonsterDelete->MonsterIndex	= ptmpNPCMonster->MonsterIndex;
						pMonsterDelete->CinemaDelete	= FALSE;		// 2011-05-30 by hskim, ŔÎÇÇ´ĎĆĽ 3Â÷ - ¸ó˝şĹÍ »čÁ¦ Ĺ¬¶óŔĚľđĆ® ąÝżµ
						Send2FieldServerW(pSendBuf, MSG_SIZE(MSG_FN_MONSTER_DELETE));
			
						// NPC ServerżˇĽ­ ¸ó˝şĹÍ¸¦ »čÁ¦ÇŃ´Ů.
						DelelteMonsterHandler(ptmpNPCMonster);
					}
				}
			}
			break;
		}		
	}
}

void CNPCMapChannel::SetCityWarOccupyGuildUID(UID32_t i_guildUID)
{
	m_CityWarOccupyGuildUID = i_guildUID;
}
BOOL CNPCMapChannel::NPCOnMonsterCreateOK(MSG_FN_MONSTER_CREATE_OK * i_pCreateOK)
{
	m_mtvectorMSG_FN_MONSTER_CREATE_OK.lock();
	m_mtvectorMSG_FN_MONSTER_CREATE_OK.push_back(*i_pCreateOK);
	m_mtvectorMSG_FN_MONSTER_CREATE_OK.unlock();
	return TRUE;
}

void CNPCMapChannel::ProcessNPCOnMonsterCreateOK(MSG_FN_MONSTER_CREATE_OK * i_pCreateOK)
{
	CNPCMonster *ptmNPCMonster = GetNPCMonster(i_pCreateOK->MonsterIndex);
	if(NULL == ptmNPCMonster
		|| ptmNPCMonster->m_enMonsterState != MS_CREATED)
	{
		return;
	}
	ptmNPCMonster->m_enMonsterState = MS_PLAYING;
	m_nTotalMonsterCountInChannel++;

	///////////////////////////////////////////////////////////////////////////////
	// 2010-05-17 by shcho - ŔÎÇÇ´ĎĆĽ ¸ó˝şĹÍ °ü·Ă ·Î±× ł˛±â±â 
	if(g_pNPCGlobal->GetIsArenaServer()
		&& 9201 == m_MapChannelIndex.MapIndex)	// ľĆ·ąłŞ Ľ­ąöżÍ ŔÎÇÇ´ĎĆĽ 1Â÷(9201ąř´ë) ŔĎ¶§¸¸ Âď´Â´Ů.
	{
		g_pNPCGlobal->WriteSystemLogEX(TRUE, "  %10s : <= CreateOK MonIndex[%4d] MonsterUnitKind[%8d] CurMonCount[%3d] TotalMonCount[%d]\r\n", // ¸ó˝şĹÍ ŔÎµ¦˝ş, ¸ó˝şĹÍ ąřČŁ, ĽŇČŻµČ ¸ó˝şĹÍ °ąĽö, »ýĽşµČ ¸đµç ¸ó˝şĹÍ Ľö
			GET_MAP_STRING(m_MapChannelIndex), i_pCreateOK->MonsterIndex,ptmNPCMonster->MonsterInfoPtr->MonsterUnitKind,m_nCurMonsterCountInChannel, m_nTotalMonsterCountInChannel);
	}

// 	if(2035200 == ptmNPCMonster->MonsterInfoPtr->MonsterUnitKind
// 		|| 2035900 == ptmNPCMonster->MonsterInfoPtr->MonsterUnitKind)
// 	{// 2006-04-25 by cmkwon, ŔÓ˝Ă·Î
// 		char	szSystemLog[256];
// 		sprintf(szSystemLog, "  %10s : <= CreateOK Monster[%8d] Position(%4d, %4d %4d) CurMonCount[%3d] TotalMonCount[%d]\r\n",
// 		GET_MAP_STRING(m_MapChannelIndex), ptmNPCMonster->MonsterInfoPtr->MonsterUnitKind, (int)ptmNPCMonster->PositionVector.x,
// 		(int)ptmNPCMonster->PositionVector.y, (int)ptmNPCMonster->PositionVector.z,
// 		m_nCurMonsterCountInChannel, m_nTotalMonsterCountInChannel);
// 		g_pNPCGlobal->WriteSystemLog(szSystemLog);
// 		DbgOut(szSystemLog);
// 	}
}


BOOL CNPCMapChannel::NPCOnMonsterDelete(MSG_FN_MONSTER_DELETE * i_pMonDelete)
{	
	m_mtvectorMSG_FN_MONSTER_DELETE.lock();
	m_mtvectorMSG_FN_MONSTER_DELETE.push_back(*i_pMonDelete);
	m_mtvectorMSG_FN_MONSTER_DELETE.unlock();
	return TRUE;
}

void CNPCMapChannel::ProcessNPCOnMonsterDelete(MSG_FN_MONSTER_DELETE * i_pMonDelete)
{
	CNPCMonster * ptmNMonster = GetNPCMonster(i_pMonDelete->MonsterIndex);
	if(NULL == ptmNMonster){			return;}

	DelelteMonsterHandler(ptmNMonster);
}

BOOL CNPCMapChannel::NPCOnMoveOK(MSG_FN_MOVE_OK	* i_pMoveOK)
{
	CLIENT_INFO * ptmClient = GetClientInfo(i_pMoveOK->ClientIndex);
	if(NULL == ptmClient){					return FALSE;}

	///////////////////////////////////////////////////////////////////////////////
	// 2008-12-02 by cmkwon, NPCServer Ĺ¬¶óŔĚľđĆ® °ü¸® ±¸Á¶ ĽöÁ¤ - ÇöŔç MapChannelIndex¸¦ ĂĽĹ©ÇŘĽ­ ´Ů¸Ł¸é ĂĘ±âČ­ Ăł¸®ÇŃ´Ů.
	if(CS_NULL != ptmClient->ClientState
		&& FALSE == ptmClient->MapChannelIdx.IsSameMapChannelIndex(this->GetMapChannelIndex()))
	{
		g_pNPCGlobal->WriteSystemLogEX(TRUE, "[Error] CNPCMapChannel::NPCOnMoveOK# MapChannelIdex error !!, CliIdx(%4d) ClientMapChannelIdx(%s) NPCMapChannelIdx(%s)\r\n"
			, ptmClient->ClientIndex, GET_MAP_STRING(ptmClient->MapChannelIdx), GET_MAP_STRING(this->GetMapChannelIndex()));
		CNPCMapChannel *pNMapChann = m_pNPCMapWorkspace->GetNPCMapChannelByMapChannelIndex(ptmClient->MapChannelIdx);
		if(pNMapChann)
		{
			MSG_FN_CLIENT_GAMEEND_OK tmGameEndOK;
			tmGameEndOK.ChannelIndex	= ptmClient->MapChannelIdx.ChannelIndex;
			tmGameEndOK.ClientIndex		= ptmClient->ClientIndex;
			pNMapChann->NPCOnClientGameEndOK(&tmGameEndOK);
		}
		return FALSE;	// 2008-12-02 by cmkwon, NPCServer Ĺ¬¶óŔĚľđĆ® °ü¸® ±¸Á¶ ĽöÁ¤ - ĂĘ±âČ­¸¸ ÇĎ°í ¸®ĹĎÇŃ´Ů
	}

	this->SetExistUserInMapChannel(TRUE);		// 2008-12-03 by cmkwon, NPCServer Ĺ¬¶óŔĚľđĆ® ľř´Â ¸ĘĂ¤łÎ Ăł¸® ĽöÁ¤ - ŔŻŔú°ˇ ŔÖŔ˝Ŕ» ĽłÁ¤ÇŃ´Ů.

	if(ptmClient->ClientState == CS_NULL)
	{	// Äł¸ŻĹÍ Á¤ş¸°ˇ ľřŔ¸¸é Á¤ş¸¸¦ żäĂ»ÇŃ´Ů.

		if(ptmClient->IsSendableReq_FN_GET_CHARACTER_INFO())
		{// 2008-12-02 by cmkwon, NPCServer Ĺ¬¶óŔĚľđĆ® °ü¸® ±¸Á¶ ĽöÁ¤ - ŔĎÁ¤ ˝Ă°Łżˇ ÇŃąřľż żäĂ»Ŕ» ŔüĽŰÇŃ´Ů.
			BYTE SendBuf[128];
			INIT_MSG(MSG_FN_GET_CHARACTER_INFO, T_FN_GET_CHARACTER_INFO, pSendCharacterInfo, SendBuf);
			pSendCharacterInfo->ChannelIndex	= m_MapChannelIndex.ChannelIndex;
			pSendCharacterInfo->ClientIndex		= i_pMoveOK->ClientIndex;
			Send2FieldServerW(SendBuf, MSG_SIZE(MSG_FN_GET_CHARACTER_INFO));
		}
	}
	else if(ptmClient->ClientState == CS_GAMESTARTED)
	{	// Äł¸Ż Á¤ş¸°ˇ ŔÖ°í Á×Ŕş »óĹÂ°ˇ ľĆ´Ď¸é PositionŔÇ Update°ˇ Ăł¸®µČ´Ů.

		D3DXVECTOR3 tmVector3Pos = A2DX(i_pMoveOK->PositionVector);

		UpdateBlockPosition(ptmClient->PositionVector.x, ptmClient->PositionVector.z,
			tmVector3Pos.x, tmVector3Pos.z, ptmClient->ClientIndex);
		ptmClient->PositionVector	= tmVector3Pos;
		ptmClient->TargetVector		= A2DX(i_pMoveOK->TargetVector);
		D3DXVec3Normalize(&ptmClient->TargetVector, &ptmClient->TargetVector);
	}
	return TRUE;
}

BOOL CNPCMapChannel::NPCOnAdminSummonMonster(MSG_FN_ADMIN_SUMMON_MONSTER * i_pSummonMonster, int i_nTargetIndex/*=0*/)
{
	char szError[1024];
	auto tmVector3Pos = A2DX(i_pSummonMonster->Position);
	
	if(FALSE == m_pNPCMapProject->m_bCreateNPCThread) {
		sprintf_s(szError, "[Error] CNPCMapProject::NPCAdminSummonMonster No Create Thread for NPCServer[%d]\r\n"
			, m_pNPCMapProject->m_bCreateNPCThread);
		CNPCGlobal::WriteSystemLog(szError);
		DbgOut(szError);
		return FALSE;
	}

	if(i_pSummonMonster->NumOfMonster <= 0 
		|| i_pSummonMonster->NumOfMonster > COUNT_MAX_ADMIN_SUMMON_MONSTER
		|| FALSE == m_pNPCMapProject->IsValidPosition(tmVector3Pos.x, tmVector3Pos.z)) {
		sprintf_s(szError, "[Error] CNPCMapProject::NPCAdminSummonMonster invalid parameter, NumMonster[%d], Position(%d, %d, %d)\r\n"
			, i_pSummonMonster->NumOfMonster, (int)tmVector3Pos.x, (int)tmVector3Pos.y, (int)tmVector3Pos.z);
		CNPCGlobal::WriteSystemLog(szError);
		DbgOut(szError);
		return FALSE;
	}

	if(m_nSizemtvectorMonsterPtr <= m_nCurMonsterCountInChannel)
	{
		sprintf_s(szError, "[Error] CNPCMapProject::NPCAdminSummonMonster1 Monster is full. ArrayMonsterSize[%d], CurrentMonsterCount(%d)\r\n"
			, m_nSizemtvectorMonsterPtr, m_nCurMonsterCountInChannel);
		CNPCGlobal::WriteSystemLog(szError);
		DbgOut(szError);
		return FALSE;
	}

	auto it = m_pNPCMapProject->m_pMapMonsterParameter->find(i_pSummonMonster->MonsterUnitKind);
	if(it == m_pNPCMapProject->m_pMapMonsterParameter->end())
	{
		sprintf(szError, "[Error] CNPCMapProject::NPCAdminSummonMonster invalid parameter, MonsterUnitKind[%d]\r\n"
			, i_pSummonMonster->MonsterUnitKind);
		CNPCGlobal::WriteSystemLog(szError);
		DbgOut(szError);
		return FALSE;
	}

	if(_strnicmp(STRMSG_S_N2TESTMONNAME_0000, it->second.MonsterName, SIZE_MAX_MONSTER_NAME) == 0
		&& i_pSummonMonster->NumOfMonster > 10) {
		CMonsterDBAccess MonsterDBAccess;
		MonsterDBAccess.GetAllMonsters(m_pNPCIOCPServer->m_mapMonsterParameter,
			m_pNPCIOCPServer->GetPtrMapItemInfo(), MONSTER_LOAD_TYPE_SIZE_FOR_SERVER
			, &(g_pNPCGlobal->m_Localization));
		return FALSE;
	}

	auto					nIdxMonsterArray = 0;
	CNPCMonster				*ptmMonster = nullptr;
	BYTE					SendBuf[SIZE_MAX_PACKET];
	auto const				tick = timeGetTime();
	
///////////////////////////////////////////////////////////////////////////////
// Object Monster handling


	if(FORM_OBJECT_STOP == it->second.MonsterForm
		|| FORM_OBJECT_PLANE_ROTATE == it->second.MonsterForm
		|| FORM_OBJECT_CANNON == it->second.MonsterForm) {

		auto *pObjInfo = FindObjectMonsterInfoByMonsterUniqueNumberAndMinimumDistance(i_pSummonMonster->MonsterUnitKind, &A2DX(i_pSummonMonster->Position));

		if(nullptr == pObjInfo)
			return FALSE;
		else if(pObjInfo->m_EventInfo.m_byObjectMonsterCreated)
			return FALSE;
		
		auto const dwTimeGap = tick - pObjInfo->m_EventInfo.m_dwLastTimeObjectMonsterCreated;
		if(pObjInfo->m_EventInfo.m_EventwParam3*1000 - 1000 < dwTimeGap
			&& pObjInfo->m_EventInfo.m_EventwParam2*1000 + 1000 > dwTimeGap)
			return FALSE;

		if(IS_STRATEGYPOINT_MONSTER(pObjInfo->m_pMonsterInfo->Belligerence)
			&& COMPARE_MPOPTION_BIT(pObjInfo->m_pMonsterInfo->MPOption, MPOPTION_BIT_AUTO_DESTROY))
			pObjInfo->m_pMonsterInfo->MPOptionParam1	= STRATEGYPOINT_SUMMONTIME_BY_INFLUENCEBOSS/4; //26-02-2024 by Inet changed from /2 to /4 to 30 minute SP
			
		pObjInfo->m_EventInfo.m_dwLastTimeObjectMonsterCreated = tick;
		mt_auto_lock mtMon(&m_mtvectorMonsterPtr);
		if(m_vectorUsableMonsterIndex.empty()) {
			char	szError[1024]; *szError = '\0';
			sprintf_s(szError, "[Error] CNPCMapChannel::NPCOnAdminSummonMonster_ Monster is full. MaxMonsterCount[%d], CurrentMonsterCount(%d)\r\n"
				, m_nMaxMonsterCountInChannel, m_nCurMonsterCountInChannel);
			CNPCGlobal::WriteSystemLog(szError);
			DbgOut(szError);
			return FALSE;				
		}
		nIdxMonsterArray = m_vectorUsableMonsterIndex.front();
		m_vectorUsableMonsterIndex.pop_front();
		ptmMonster = GetNPCMonster(nIdxMonsterArray);
		
		if(nullptr == ptmMonster
			|| ptmMonster->m_enMonsterState != MS_NULL)
			return FALSE;

		m_nCurMonsterCountInChannel++;
		pObjInfo->m_EventInfo.m_byObjectMonsterCreated = TRUE;
		
		auto * pvectHPAction = m_pNPCIOCPServer->m_mapHPAction.findEZ_ptr((&it->second)->HPActionIdx);
		
		if(nullptr != pvectHPAction)
			ptmMonster->m_HPAction.InitHPActionListByDB(pvectHPAction);

#ifdef _INET_LEVELING_SP
		MAP_CHANNEL_INDEX tmp_MapChanBCU;
		MAP_CHANNEL_INDEX tmp_MapChanANI;
		tmp_MapChanBCU.MapIndex = 3003;
		tmp_MapChanBCU.ChannelIndex = 0;
		tmp_MapChanANI.MapIndex = 3018;
		tmp_MapChanANI.ChannelIndex = 0;
		char	szSystemLog[256];
		bool bIsMS = false;
		if(m_pNPCIOCPServer->GetNPCMapChannelByMapChannelIndex(tmp_MapChanBCU) && m_pNPCIOCPServer->GetNPCMapChannelByMapChannelIndex(tmp_MapChanBCU)->m_bNotCreateMonster == TRUE//is MS now for BCU
			|| m_pNPCIOCPServer->GetNPCMapChannelByMapChannelIndex(tmp_MapChanANI) && m_pNPCIOCPServer->GetNPCMapChannelByMapChannelIndex(tmp_MapChanANI)->m_bNotCreateMonster == TRUE) //is ANI MS 
		{
			bIsMS = true;
		}
		int nLevelRand = rand32() % 8;
		time_t now = time(nullptr);
		auto nDiff = now - LastMSSpawned;
		int nLevelDefined = i_pSummonMonster->NumOfMonster;
		if (IS_STRATEGYPOINT_MONSTER(pObjInfo->m_pMonsterInfo->Belligerence)) {		
			if (nLevelDefined > 1 && !bIsMS) {
				if (pObjInfo->m_pMonsterInfo->Belligerence == BELL_STRATEGYPOINT_VCN)
					pObjInfo->m_pMonsterInfo->MonsterUnitKind = 2052000 + nLevelDefined-1;
				else if (pObjInfo->m_pMonsterInfo->Belligerence == BELL_STRATEGYPOINT_ANI)
					pObjInfo->m_pMonsterInfo->MonsterUnitKind = 2052100 + nLevelDefined-1;

				sprintf_s(szSystemLog, "[INET] Object Monster Create SP,	MONSTER(%d) at Map(%s) SummonLevel defined (%d)\r\n",
					pObjInfo->m_pMonsterInfo->MonsterUnitKind, GET_MAP_STRING(this->m_MapChannelIndex), nLevelDefined);			
			}
			else
			{
				CMonsterDBAccess MonsterDBAccess;
				auto const nBCUContrib = MonsterDBAccess.GetInfluenceContributtionPoints(2);
				auto const nANIContrib = MonsterDBAccess.GetInfluenceContributtionPoints(4);
				
				auto nSummonLevelBCU = 0;
				auto nSummonLevelANI = 0;

				sprintf_s(szSystemLog, "[INET] Get contibuttion Points ANI(%d) BCU(%d) for SPNUM(%d)\r\n", nANIContrib, nBCUContrib, pObjInfo->m_pMonsterInfo->MonsterUnitKind);

				CNPCGlobal::WriteSystemLog(szSystemLog);
				if (nDiff > 60)
				{
					if (nBCUContrib > nANIContrib) { //BCU Wins

						auto const Diff1 = nBCUContrib - nANIContrib;

						if (Diff1 >= 0 && Diff1 < 10000) {
							nSummonLevelBCU = 5;
							nSummonLevelANI = 5;
						}
						else if (Diff1 >= 10000 && Diff1 < 20000) {
							nSummonLevelBCU = 4;
							nSummonLevelANI = 6;
						}
						else if (Diff1 >= 20000 && Diff1 < 30000) {
							nSummonLevelBCU = 3;
							nSummonLevelANI = 7;
						}
						else if (Diff1 >= 30000 && Diff1 < 40000) {
							nSummonLevelBCU = 2;
							nSummonLevelANI = 8;
						}
						else if (Diff1 >= 40000) {
							nSummonLevelBCU = 1;
							nSummonLevelANI = 9;
						}
					}
					else { //ANI Wins

						auto const Diff2 = nANIContrib - nBCUContrib;

						if (Diff2 >= 0 && Diff2 < 10000) {
							nSummonLevelBCU = 5;
							nSummonLevelANI = 5;
						}
						else if (Diff2 >= 10000 && Diff2 < 20000) {
							nSummonLevelANI = 4;
							nSummonLevelBCU = 6;
						}
						else if (Diff2 >= 20000 && Diff2 < 30000) {
							nSummonLevelANI = 3;
							nSummonLevelBCU = 7;
						}
						else if (Diff2 >= 30000 && Diff2 < 40000) {
							nSummonLevelANI = 2;
							nSummonLevelBCU = 8;
						}
						else if (Diff2 >= 40000) {
							nSummonLevelANI = 1;
							nSummonLevelBCU = 9;
						}
					}
				}
				else
				{
					switch (nLastMSMonsterUID)
					{//spawn same level sp as MS
					case 2087800:
						nSummonLevelANI = 1;
						break;
					case 2087700:
						nSummonLevelANI = 2;
						break;
					case 2087600:
						nSummonLevelANI = 3;
						break;
					case 2087500:
						nSummonLevelANI = 4;
						break;
					case 2027300:
						nSummonLevelANI = 5;
						break;
					case 2057300:
						nSummonLevelANI = 6;
						break;
					case 2057400:
						nSummonLevelANI = 7;
						break;
					case 2057500:
						nSummonLevelANI = 8;
						break;
					case 2057600:
						nSummonLevelANI = 9;
						break;
					case 2087400:
						nSummonLevelBCU = 1;
						break;
					case 2087300:
						nSummonLevelBCU = 2;
						break;
					case 2087200:
						nSummonLevelBCU = 3;
						break;
					case 2087100:
						nSummonLevelBCU = 4;
						break;
					case 2027200:
						nSummonLevelBCU = 5;
						break;
					case 2056900:
						nSummonLevelBCU = 6;
						break;
					case 2057000:
						nSummonLevelBCU = 7;
						break;
					case 2057100:
						nSummonLevelBCU = 8;
						break;
					case 2057200:
						nSummonLevelBCU = 9;
						break;
					default:
					{
						nSummonLevelANI = 0;
						nSummonLevelBCU = 0;
					}
					break;
					}
				}
				
				if (!bIsMS) {
						if (pObjInfo->m_pMonsterInfo->Belligerence == BELL_STRATEGYPOINT_VCN)
							pObjInfo->m_pMonsterInfo->MonsterUnitKind = 2052000 + nSummonLevelBCU;
						else if (pObjInfo->m_pMonsterInfo->Belligerence == BELL_STRATEGYPOINT_ANI)
							pObjInfo->m_pMonsterInfo->MonsterUnitKind = 2052100 + nSummonLevelANI;
					/*
					if (pObjInfo->m_pMonsterInfo->Belligerence == BELL_STRATEGYPOINT_VCN)
						pObjInfo->m_pMonsterInfo->MonsterUnitKind = 2052001 + nLevelRand;
					else if (pObjInfo->m_pMonsterInfo->Belligerence == BELL_STRATEGYPOINT_ANI)
						pObjInfo->m_pMonsterInfo->MonsterUnitKind = 2052101 + nLevelRand;*/
					sprintf_s(szSystemLog, "[INET] Object Monster Create SP (No MS war),	MONSTER(%d) at Map(%s)\r\n"
						, pObjInfo->m_pMonsterInfo->MonsterUnitKind, GET_MAP_STRING(this->m_MapChannelIndex));
				}
				else
				{
					if (pObjInfo->m_pMonsterInfo->Belligerence == BELL_STRATEGYPOINT_VCN)
						pObjInfo->m_pMonsterInfo->MonsterUnitKind = 2052005; //level 5 sp
					else if (pObjInfo->m_pMonsterInfo->Belligerence == BELL_STRATEGYPOINT_ANI)
						pObjInfo->m_pMonsterInfo->MonsterUnitKind = 2052105; //level 5 sp

					sprintf_s(szSystemLog, "[INET] Object Monster Create SP (!There is MS war!),	MONSTER(%d) at Map(%s)\r\n"
						, pObjInfo->m_pMonsterInfo->MonsterUnitKind, GET_MAP_STRING(this->m_MapChannelIndex));
				}


			}
			CNPCGlobal::WriteSystemLog(szSystemLog);
		}
#endif

		
		ptmMonster->SetCurrentMapChannel(this);				// 2009-09-09 ~ 2010 by dhjin, ŔÎÇÇ´ĎĆĽ - ¸Ę Ă¤łÎ ąĚ¸® ĽłÁ¤
		ptmMonster->CreateNPCMonster(&it->second, &pObjInfo->m_vPos, tick, pObjInfo->m_EventInfo.m_EventwParam1
			, i_pSummonMonster->MonsterTargetType1, i_pSummonMonster->TargetTypeData1, i_pSummonMonster->CltIdxForTargetType1
			, this->GetMonsterVisibleDiameterW(), 0, pObjInfo->m_EventInfo.m_bEventType, &pObjInfo->m_vVel , &i_pSummonMonster->MonsterBalanceData ); // 2010. 06. 08 by hsLee ŔÎÇÇ´ĎĆĽ ÇĘµĺ 2Â÷ ł­ŔĚµµ Á¶Ŕý. (ľĆ±ş µżŔĎ ąë·±˝ş Ŕűżë.) - ¸ó˝şĹÍ ąë·±˝ş Ŕűżë.
		SetInitialPositionAndSendCreateMonster(ptmMonster, SendBuf);

		return TRUE;		
	}

	D3DXVECTOR3				posVector3;
	vector<D3DXVECTOR3>		vecD3DXVECTOR;
	vecD3DXVECTOR.reserve(i_pSummonMonster->NumOfMonster);

	auto nRegionTileCounts = SIZE_TILE_ADMIN_SUMMON_MONSTER_REGION;
	
	if(0 != i_nTargetIndex)
		nRegionTileCounts = SIZE_TILE_ADMIN_SUMMON_MONSTER_REGION*4;
	

	auto nTemp = static_cast<int>(tmVector3Pos.x)/SIZE_MAP_TILE_SIZE;
	auto const nStartx = max(0, nTemp - nRegionTileCounts);
	auto const nEndx = min(m_pNPCMapProject->m_sXSize - 1, nTemp + nRegionTileCounts);
	nTemp = static_cast<int>(tmVector3Pos.z)/SIZE_MAP_TILE_SIZE;
	auto const nStartz = max(0, nTemp - nRegionTileCounts);
	auto const nEndz = min(m_pNPCMapProject->m_sYSize - 1, nTemp + nRegionTileCounts);
	auto creatMonsters = min(i_pSummonMonster->NumOfMonster, m_nSizemtvectorMonsterPtr - m_nCurMonsterCountInChannel);
	NPCGetCreatablePosition(it->second.MonsterForm, it->second.Size, nStartx, nStartz, nEndx, nEndz
		, static_cast<int>(tmVector3Pos.y - 20), static_cast<int>(tmVector3Pos.y + 20)
		, vecD3DXVECTOR, creatMonsters, FALSE, TRUE);

	if(creatMonsters <= 0)
	{
		return FALSE;
	}
	
	if (strcmp(i_pSummonMonster->CharacterName, "InfluenceWar") == 0) {//16-05-2021 by Inetpub if ms is spawned then update last spawn time
		LastMSSpawned = time(nullptr);
		nLastMSMonsterUID = i_pSummonMonster->MonsterUnitKind;
	}
	sprintf_s(szError, "Admin Summon Master(%10s) ==> %s, MonType[%d], Position(%5d, %4d, %5d) ReqCount[%2d] SummonCount[%2d]\r\n"
		, i_pSummonMonster->CharacterName, GET_MAP_STRING(this->GetMapChannelIndex()), i_pSummonMonster->MonsterUnitKind
		, static_cast<int>(tmVector3Pos.x), static_cast<int>(tmVector3Pos.y), static_cast<int>(tmVector3Pos.z)
		, i_pSummonMonster->NumOfMonster, vecD3DXVECTOR.size());
	CNPCGlobal::WriteSystemLog(szError);
	DBGOUT(szError);
	mt_auto_lock mtMon(&m_mtvectorMonsterPtr);
	while(creatMonsters > 0 && false == vecD3DXVECTOR.empty())
	{
		creatMonsters--;
		if(m_vectorUsableMonsterIndex.empty())
		{
			sprintf(szError, "[Error] CNPCMapProject::NPCAdminSummonMonster2 Monster is full. ArrayMonsterSize[%d], CurrentMonsterCount(%d)\r\n"
				, m_nSizemtvectorMonsterPtr, m_nCurMonsterCountInChannel);
			CNPCGlobal::WriteSystemLog(szError);
			DbgOut(szError);
			return FALSE;
		}
		nIdxMonsterArray = m_vectorUsableMonsterIndex.front();
		m_vectorUsableMonsterIndex.pop_front();
		ptmMonster = GetNPCMonster(nIdxMonsterArray);
		
		if(nullptr == ptmMonster
			|| ptmMonster->m_enMonsterState != MS_NULL)
			return FALSE;

		m_nCurMonsterCountInChannel++;			
		posVector3 = vecD3DXVECTOR.back();
		vecD3DXVECTOR.pop_back();

		auto * pvectHPAction = m_pNPCIOCPServer->m_mapHPAction.findEZ_ptr((&it->second)->HPActionIdx);
		
		if(nullptr != pvectHPAction)
			ptmMonster->m_HPAction.InitHPActionListByDB(pvectHPAction);
		
		ptmMonster->SetCurrentMapChannel(this);

		ptmMonster->CreateNPCMonster(&it->second, &posVector3, tick, 0xFFFF, i_pSummonMonster->MonsterTargetType1
			, i_pSummonMonster->TargetTypeData1, i_pSummonMonster->CltIdxForTargetType1, this->GetMonsterVisibleDiameterW(), i_nTargetIndex , EVENT_TYPE_NO_OBJECT_MONSTER , nullptr , &i_pSummonMonster->MonsterBalanceData );

		D3DXVECTOR3 tmUnitVec3Tar;
		D3DXVec3Normalize(&tmUnitVec3Tar, &(ptmMonster->PositionVector - tmVector3Pos));
		ptmMonster->SetMoveTargetVector(&tmUnitVec3Tar);
		ptmMonster->SetTargetVector(&tmUnitVec3Tar);
		SetInitialPositionAndSendCreateMonster(ptmMonster, SendBuf, &tmVector3Pos);
	}
	return TRUE;
}

BOOL CNPCMapChannel::NPCOnClientGameStartOK(MSG_FN_CLIENT_GAMESTART_OK * i_pClientStartOK)
{
	CLIENT_INFO * ptmClient = GetClientInfo(i_pClientStartOK->ClientIndex);
	if(NULL == ptmClient){					return FALSE;}

	///////////////////////////////////////////////////////////////////////////////
	// 2008-12-02 by cmkwon, NPCServer Ĺ¬¶óŔĚľđĆ® °ü¸® ±¸Á¶ ĽöÁ¤ - ÇöŔç MapChannelIndexżÍ ´Ů¸Ł¸é ĂĘ±âČ­ ÇĎ°í ĽłÁ¤ÇŃ´Ů.
	if(CS_NULL != ptmClient->ClientState
		&& FALSE == ptmClient->MapChannelIdx.IsSameMapChannelIndex(this->GetMapChannelIndex()))
	{
		g_pNPCGlobal->WriteSystemLogEX(TRUE, "[Error] CNPCMapChannel::NPCOnClientGameStartOK# MapChannelIdex error !!, CliIdx(%4d) ClientMapChannelIdx(%s) NPCMapChannelIdx(%s)\r\n"
			, ptmClient->ClientIndex, GET_MAP_STRING(ptmClient->MapChannelIdx), GET_MAP_STRING(this->GetMapChannelIndex()));
		CNPCMapChannel *pNMapChann = m_pNPCMapWorkspace->GetNPCMapChannelByMapChannelIndex(ptmClient->MapChannelIdx);
		if(pNMapChann)
		{
			MSG_FN_CLIENT_GAMEEND_OK tmGameEndOK;
			tmGameEndOK.ChannelIndex	= ptmClient->MapChannelIdx.ChannelIndex;
			tmGameEndOK.ClientIndex		= ptmClient->ClientIndex;
			pNMapChann->NPCOnClientGameEndOK(&tmGameEndOK);
		}
	}

	this->SetExistUserInMapChannel(TRUE);		// 2008-12-03 by cmkwon, NPCServer Ĺ¬¶óŔĚľđĆ® ľř´Â ¸ĘĂ¤łÎ Ăł¸® ĽöÁ¤ - ŔŻŔú°ˇ ŔÖŔ˝Ŕ» ĽłÁ¤ÇŃ´Ů.

	if(ptmClient->ClientState == CS_NULL)
	{
		ptmClient->ClientState				= CS_GAMESTARTED;
		*ptmClient							= i_pClientStartOK->mexCharacter;
		ptmClient->GuildMasterCharacterUID	= i_pClientStartOK->GuildMasterCharUID;
		ptmClient->bStealthState			= i_pClientStartOK->bStealthState1;
		ptmClient->bInvisible				= i_pClientStartOK->bInvisible;				// 2006-11-27 by dhjin
		D3DXVec3Normalize(&ptmClient->TargetVector, &ptmClient->PositionVector);
		ptmClient->MapChannelIdx			= this->GetMapChannelIndex();				// 2008-12-02 by cmkwon, NPCServer Ĺ¬¶óŔĚľđĆ® °ü¸® ±¸Á¶ ĽöÁ¤ -

		if(FALSE == SetInitialPosition(ptmClient->PositionVector.x, ptmClient->PositionVector.z, ptmClient->ClientIndex))
		{
			char szTemp[256];
			sprintf(szTemp, "[Error] SetInitialPosition_3 Error, MapChannel(%s) UnitIndex(%5d) XZ(%5.0f, %5.0f) \r\n"
				, GET_MAP_STRING(this->m_MapChannelIndex), ptmClient->ClientIndex
				, ptmClient->PositionVector.x, ptmClient->PositionVector.z);
			DBGOUT(szTemp);
			g_pNPCGlobal->WriteSystemLog(szTemp);
		}
	}
	else
	{
		D3DXVECTOR3 tmVector3Pos = A2DX(i_pClientStartOK->mexCharacter.PositionVector);

		UpdateBlockPosition(ptmClient->PositionVector.x, ptmClient->PositionVector.z,
			tmVector3Pos.x, tmVector3Pos.z, ptmClient->ClientIndex);

		ptmClient->ClientState				= CS_GAMESTARTED;
		*ptmClient							= i_pClientStartOK->mexCharacter;
		ptmClient->GuildMasterCharacterUID	= i_pClientStartOK->GuildMasterCharUID;
		ptmClient->bStealthState			= i_pClientStartOK->bStealthState1;
		ptmClient->bInvisible				= i_pClientStartOK->bInvisible;			// 2006-11-27 by dhjin
	}

	return TRUE;
}

BOOL CNPCMapChannel::NPCOnClientGameEndOK(MSG_FN_CLIENT_GAMEEND_OK * i_pClientEndOK)
{
	CLIENT_INFO * ptmClient = GetClientInfo(i_pClientEndOK->ClientIndex);
	if(NULL == ptmClient || ptmClient->ClientState == CS_NULL){				return FALSE;}
	
	///////////////////////////////////////////////////////////////////////////////
	// 2008-12-02 by cmkwon, NPCServer Ĺ¬¶óŔĚľđĆ® °ü¸® ±¸Á¶ ĽöÁ¤ - ÇöŔç MapChannelIndex¸¦ ĂĽĹ© ČÄżˇ Ăł¸®
	if(FALSE == ptmClient->MapChannelIdx.IsSameMapChannelIndex(this->GetMapChannelIndex()))
	{
		g_pNPCGlobal->WriteSystemLogEX(TRUE, "[Error] CNPCMapChannel::NPCOnClientGameEndOK# MapChannelIdex error !!, CliIdx(%4d) ClientMapChannelIdx(%s) NPCMapChannelIdx(%s)\r\n"
			, ptmClient->ClientIndex, GET_MAP_STRING(ptmClient->MapChannelIdx), GET_MAP_STRING(this->GetMapChannelIndex()));
		return FALSE;		// 2008-12-02 by cmkwon, NPCServer Ĺ¬¶óŔĚľđĆ® °ü¸® ±¸Á¶ ĽöÁ¤ - MSG_FN_CLIENT_GAMEEND_OK żˇĽ­¸ĘĂ¤łÎŔĚ ´Ů¸Ł¸é ±×łÉ ¸®ĹĎÇŃ´Ů.
	}

	this->SetExistUserInMapChannel(TRUE);		// 2008-12-03 by cmkwon, NPCServer Ĺ¬¶óŔĚľđĆ® ľř´Â ¸ĘĂ¤łÎ Ăł¸® ĽöÁ¤ - ŔŻŔú°ˇ ŔÖŔ˝Ŕ» ĽłÁ¤ÇŃ´Ů.

	if(FALSE == DeleteBlockPosition(ptmClient->PositionVector.x, ptmClient->PositionVector.z, ptmClient->ClientIndex))
	{
		char szTemp[256];
		sprintf(szTemp, "[Error] DeleteBlockPosition_1 Error, MapChannel(%s) UnitIndex(%5d) XZ(%5.0f, %5.0f)\n"
			, GET_MAP_STRING(this->m_MapChannelIndex), ptmClient->ClientIndex
			, ptmClient->PositionVector.x, ptmClient->PositionVector.z);
		DBGOUT(szTemp);
		g_pNPCGlobal->WriteSystemLog(szTemp);
	}
	ptmClient->ResetClientInfo();

	return TRUE;	
}

BOOL CNPCMapChannel::NPCOnCharacterChangeBodycondition(MSG_FN_CHARACTER_CHANGE_BODYCONDITION * i_pChange)
{
	CLIENT_INFO * ptmClient = GetClientInfo(i_pChange->ClientIndex);
	if(NULL == ptmClient || ptmClient->ClientState == CS_NULL){		return FALSE;}
		
	ptmClient->BodyCondition = i_pChange->BodyCondition;
	if(COMPARE_BODYCON_BIT(ptmClient->BodyCondition, BODYCON_DEAD_MASK))
	{
		ptmClient->ClientState = CS_DEAD;
	}
	return TRUE;
}

BOOL CNPCMapChannel::NPCOnCharacterChangeStealthState(MSG_FN_CHARACTER_CHANGE_STEALTHSTATE * i_pChange)
{
	CLIENT_INFO * ptmClient = GetClientInfo(i_pChange->ClientIndex);
	if(NULL == ptmClient || ptmClient->ClientState == CS_NULL){		return FALSE;}
		
	ptmClient->bStealthState = i_pChange->bStealthState2;

	///////////////////////////////////////////////////////////////////////////////
	// 2006-11-27 by dhjin, ˝şĹÚ˝ş »óĹÂ°ˇ ÄŃÁú¶§ Äł¸ŻĹÍ¸¦ Ĺ¸°ŮŔ¸·Î ÇĎ´Â ¸ó˝şĹÍŔÇ TargetIndex¸¦ ĂĘ±âČ­ ÇŃ´Ů
	if(ptmClient->bStealthState)
	{
		mt_auto_lock mtA(&m_mtvectorMonsterPtr);
		for(int i=0; i < m_mtvectorMonsterPtr.size(); i++)
		{
			CNPCMonster *pNMon = (CNPCMonster*)m_mtvectorMonsterPtr[i];
			if(MS_PLAYING == pNMon->m_enMonsterState
				&& FALSE == COMPARE_BODYCON_BIT(pNMon->BodyCondition, BODYCON_DEAD_MASK)
				&& i_pChange->ClientIndex == pNMon->m_nTargetIndex)
			{
				pNMon->SetTargetIndex(0);
			}
		}
	}
	
	return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
/// \fn			BOOL CNPCMapChannel::NPCOnCharacterChangeInvisible(MSG_FN_CHARACTER_CHANGE_INVISIBLE * i_pChange)
/// \brief		
/// \author		dhjin
/// \date		2006-11-27 ~ 2006-11-27
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CNPCMapChannel::NPCOnCharacterChangeInvisible(MSG_FN_CHARACTER_CHANGE_INVISIBLE * i_pChange)
{
	CLIENT_INFO * ptmClient = GetClientInfo(i_pChange->ClientIndex);
	if(NULL == ptmClient || ptmClient->ClientState == CS_NULL){		return FALSE;}
		
	ptmClient->bInvisible = i_pChange->bInvisible;

	///////////////////////////////////////////////////////////////////////////////
	// 2006-11-27 by dhjin, Ĺő¸í ˝şĹłŔĚ ÄŃÁú¶§ Äł¸ŻĹÍ¸¦ Ĺ¸°ŮŔ¸·Î ÇĎ´Â ¸ó˝şĹÍŔÇ TargetIndex¸¦ ĂĘ±âČ­ ÇŃ´Ů
	if(ptmClient->bInvisible)
	{
		mt_auto_lock mtA(&m_mtvectorMonsterPtr);
		for(int i=0; i < m_mtvectorMonsterPtr.size(); i++)
		{
			CNPCMonster *pNMon = (CNPCMonster*)m_mtvectorMonsterPtr[i];
			if(MS_PLAYING == pNMon->m_enMonsterState
				&& FALSE == COMPARE_BODYCON_BIT(pNMon->BodyCondition, BODYCON_DEAD_MASK)
				&& i_pChange->ClientIndex == pNMon->m_nTargetIndex)
			{
				pNMon->SetTargetIndex(0);
			}
		}
	}
	
	return TRUE;
}

BOOL CNPCMapChannel::NPCOnCharacterChangeCurrentHPDPSPEP(MSG_FN_CHARACTER_CHANGE_CURRENTHPDPSPEP * i_pChange)
{
	if(i_pChange->ClientIndex < MONSTER_CLIENT_INDEX_START_NUM)
	{	// Äł¸ŻĹÍŔÇ BodyCondition

		CLIENT_INFO * ptmClient = GetClientInfo(i_pChange->ClientIndex);
		if(NULL == ptmClient || ptmClient->ClientState == CS_NULL){		return FALSE;}
		
		ptmClient->CurrentHP = i_pChange->CurrentHP;
	}
	else
	{
		CNPCMonster * ptmNMonster = GetNPCMonster(i_pChange->ClientIndex);
		if(NULL == ptmNMonster || ptmNMonster->m_enMonsterState == MS_NULL){		return FALSE;}

		ptmNMonster->CurrentHP = i_pChange->CurrentHP;
	}
	return TRUE;
}

BOOL CNPCMapChannel::NPCOnCharacterChangeCharacterMode(MSG_FN_CHARACTER_CHANGE_CHARACTER_MODE_OK * i_pChange)
{
	CLIENT_INFO * ptmClient = GetClientInfo(i_pChange->ClientIndex);
	if(NULL == ptmClient || ptmClient->ClientState == CS_NULL){		return FALSE;}
	
	ptmClient->CharacterMode1 = i_pChange->CharacterMode0;
	return TRUE;	
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			BOOL CNPCMapChannel::NPCOnCharacterChangeInfluenceType(MSG_FN_CHARACTER_CHANGE_INFLUENCE_TYPE * i_pChange)
/// \brief		
/// \author		cmkwon
/// \date		2005-12-03 ~ 2005-12-03
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CNPCMapChannel::NPCOnCharacterChangeInfluenceType(MSG_FN_CHARACTER_CHANGE_INFLUENCE_TYPE * i_pChange)
{
	CLIENT_INFO * ptmClient = GetClientInfo(i_pChange->ClientIndex);
	if(NULL == ptmClient || ptmClient->ClientState == CS_NULL){		return FALSE;}
	
	// 2005-12-03 by cmkwon, ľĆÁ÷ NPC Ľ­ąöżˇ´Â ĽĽ·ÂÁ¤ş¸°ˇ ľř´Ů
	// 2005-12-27 by cmkwon
	ptmClient->InfluenceType1 = i_pChange->InfluenceType0;
	return TRUE;	
}

BOOL CNPCMapChannel::NPCOnMonsterChangeHP(MSG_FN_MONSTER_CHANGE_HP * i_pChange)
{
	CNPCMonster * ptmNMonster = GetNPCMonster(i_pChange->MonsterIndex);
	if(NULL == ptmNMonster){			return FALSE;}
	
	// ¸ó˝şĹÍ Á¤ş¸ ľ÷µĄŔĚĆ®	
	ptmNMonster->CurrentHP = i_pChange->CurrentHP;	

	////////////////////////////////////////////////////////////////////////////////
	// 2009-09-09 ~ 2010 by dhjin, ŔÎÇÇ´ĎĆĽ - 
	if(ptmNMonster->m_HPAction.CheckValidSizeTalkHPRate()) {
		int CurrentMonHPRate = 1;
		if( 1 < ptmNMonster->CurrentHP ) 
		{
			// 2010. 07. 06 by hsLee. ŔÎÇÇ´ĎĆĽ ł­ŔĚµµ Á¶Ŕý. ( ¸ó˝şĹÍ ˝şĹł - HPČ¸şą ˝şĹł °ü·Ă ąŮ˛ď HP°ü·Ă şŻĽö¸¦ »çżëÇĎµµ·Ď ĽöÁ¤. )
			/*CurrentMonHPRate = (ptmNMonster->CurrentHP * 100) / ptmNMonster->MonsterInfoPtr->MonsterHP;*/
			CurrentMonHPRate = (ptmNMonster->CurrentHP * 100) / ptmNMonster->MonsterInfoExtend.fMaxHP;
			// End 2010. 07. 06 by hsLee. ŔÎÇÇ´ĎĆĽ ł­ŔĚµµ Á¶Ŕý. ( ¸ó˝şĹÍ ˝şĹł - HPČ¸şą ˝şĹł °ü·Ă ąŮ˛ď HP°ü·Ă şŻĽö¸¦ »çżëÇĎµµ·Ď ĽöÁ¤. )
		}

		HPACTION_TALK_HPRATE MsgTalk;
		MEMSET_ZERO(&MsgTalk, sizeof(HPACTION_TALK_HPRATE));
		if(ptmNMonster->m_HPAction.GetTalkHPRate(CurrentMonHPRate, &MsgTalk)) {
			// HP°Ş °ü·Ă ´ëČ­°ˇ ŔÖ´Ů¸é ŔüĽŰÇŃ´Ů.
			this->SendFSvrHPTalk(ptmNMonster, &MsgTalk);
		}
	}

	return TRUE;
}



BOOL CNPCMapChannel::NPCOnGetCharacterInfoOK(MSG_FN_GET_CHARACTER_INFO_OK * i_pInfoOK)
{
	CLIENT_INFO * ptmClient = GetClientInfo(i_pInfoOK->ClientIndex);
	if(NULL == ptmClient
		|| ptmClient->ClientState != CS_NULL)
	{	// Äł¸Ż ŔÎµ¦˝ş°ˇ ŔŻČżÇĎÁö ľĘ°ĹłŞ Äł¸Ż Á¤ş¸°ˇ ŔĚąĚ ŔÖŔ»¶§

		return FALSE;
	}

	this->SetExistUserInMapChannel(TRUE);		// 2008-12-03 by cmkwon, NPCServer Ĺ¬¶óŔĚľđĆ® ľř´Â ¸ĘĂ¤łÎ Ăł¸® ĽöÁ¤ - 

	ptmClient->ClientState				= CS_GAMESTARTED;
	*ptmClient							= i_pInfoOK->mexCharacter;
	ptmClient->GuildMasterCharacterUID	= i_pInfoOK->GuildMasterCharUID;
	ptmClient->bStealthState			= i_pInfoOK->bStealthState1;
	ptmClient->bInvisible				= i_pInfoOK->bInvisible;			// 2006-11-27 by dhjin
	D3DXVec3Normalize(&ptmClient->TargetVector, &ptmClient->PositionVector);
	ptmClient->MapChannelIdx			= this->m_MapChannelIndex;			// 2008-12-02 by cmkwon, NPCServer Ĺ¬¶óŔĚľđĆ® °ü¸® ±¸Á¶ ĽöÁ¤ - 

	if(FALSE == SetInitialPosition(ptmClient->PositionVector.x, ptmClient->PositionVector.z, ptmClient->ClientIndex))
	{
		char szTemp[256];
		sprintf(szTemp, "[Error] SetInitialPosition_4 Error, MapChannel(%s) UnitIndex(%5d) XZ(%5.0f, %5.0f)\n"
			, GET_MAP_STRING(this->m_MapChannelIndex), ptmClient->ClientIndex
			, ptmClient->PositionVector.x, ptmClient->PositionVector.z);
		DBGOUT(szTemp);
		g_pNPCGlobal->WriteSystemLog(szTemp);
	}
	return TRUE;
}


BOOL CNPCMapChannel::NPCOnSkillUseSkillOK(MSG_FN_SKILL_USE_SKILL_OK * i_pSkillOK)
{
	CLIENT_INFO * ptmClient = GetClientInfo(i_pSkillOK->AttackIndex);
	if(NULL == ptmClient
		|| ptmClient->ClientState == CS_NULL
		|| TRUE == COMPARE_BODYCON_BIT(ptmClient->BodyCondition, BODYCON_DEAD_MASK))
	{
		return FALSE;
	}

	ITEM *pItemSkill = m_pNPCIOCPServer->GetItemInfo(i_pSkillOK->SkillItemID.ItemNum);
	if(NULL == pItemSkill
		|| FALSE == IS_SKILL_ITEM(pItemSkill->Kind))
	{
		return FALSE;
	}

	switch(SKILL_BASE_NUM(pItemSkill->ItemNum))
	{
	case BGEAR_SKILL_BASENUM_BACKMOVEMACH:
	case BGEAR_SKILL_BASENUM_TURNAROUND:
	case IGEAR_SKILL_BASENUM_BACKMOVEMACH:
	case IGEAR_SKILL_BASENUM_TURNAROUND:
		{
			ptmClient->TimeUseControlSkill	= timeGetTime();
			ptmClient->UseControlSkill		= TRUE;
		}
		break;
	}

	if(FALSE == IS_MONSTER_CLIENT_INDEX(i_pSkillOK->TargetIndex))
	{
		return TRUE;
	}

	CNPCMonster *ptmNMonster = GetNPCMonster(i_pSkillOK->TargetIndex);
	if(NULL == ptmNMonster
		|| ptmNMonster->m_enMonsterState == MS_NULL
		|| TRUE == COMPARE_BODYCON_BIT(ptmNMonster->BodyCondition, BODYCON_DEAD_MASK))
	{
		return FALSE;
	}
	
// 2009-04-21 by cmkwon, ITEMżˇ DesParam ÇĘµĺ °łĽö 8°ł·Î ´Ă¸®±â - ľĆ·ˇżÍ °°ŔĚ ĽöÁ¤
// 	if(DES_SKILL_SLOWMOVING == pItemSkill->DestParameter1
// 		|| DES_SKILL_SLOWMOVING == pItemSkill->DestParameter2
// 		|| DES_SKILL_SLOWMOVING == pItemSkill->DestParameter3
// 		|| DES_SKILL_SLOWMOVING == pItemSkill->DestParameter4)
	if(pItemSkill->IsExistDesParam(DES_SKILL_SLOWMOVING))
	{
		AttackedItemInfo aItem;
		aItem.AttackIndex		= i_pSkillOK->AttackIndex;
		aItem.pAttackITEM		= pItemSkill;
		aItem.dwExpireTick		= timeGetTime() + pItemSkill->Time;
		aItem.AttackedItemIndex	= m_uiAttackedItemUniqueIndex++;
		ptmNMonster->InsertAttackedItemInfo(&aItem);
	}
	
	return TRUE;
}


BOOL CNPCMapChannel::NPCOnBattleSetAttackCharacter(MSG_FN_BATTLE_SET_ATTACK_CHARACTER * i_pAttackInfo)
{
	m_mtvectorMSG_FN_BATTLE_SET_ATTACK_CHARACTER.lock();
	m_mtvectorMSG_FN_BATTLE_SET_ATTACK_CHARACTER.push_back(*i_pAttackInfo);
	m_mtvectorMSG_FN_BATTLE_SET_ATTACK_CHARACTER.unlock();
	return TRUE;
}

void CNPCMapChannel::ProcessNPCOnBattleSetAttackCharacter(MSG_FN_BATTLE_SET_ATTACK_CHARACTER * i_pAttackInfo)
{
// 2010-03-16 by cmkwon, ŔÎÇÇ2Â÷ MtoM, MtoC Ĺ¸°Ů şŻ°ć °ü·Ă ĽöÁ¤ - ľĆ·ˇżÍ °°ŔĚ ĽöÁ¤ÇÔ.
// 	///////////////////////////////////////////////////////////////////////////////
// 	// °ř°Ý ąŢŔş ¸ó˝şĹÍŔÇ ŔŻČżĽşŔ» ĂĽĹ©ÇŃ´Ů
// 	CNPCMonster * ptmNMonster = GetNPCMonster(i_pAttackInfo->TargetIndex);
// 	if(NULL == ptmNMonster 
// 		|| ptmNMonster->m_enMonsterState == MS_NULL
// 		|| TRUE == COMPARE_BODYCON_BIT(ptmNMonster->BodyCondition, BODYCON_DEAD_MASK)
// 		|| BELL_INFINITY_DEFENSE_MONSTER == ptmNMonster->MonsterInfoPtr->Belligerence) // 2009-09-09 ~ 2010 by dhjin, ŔÎÇÇ´ĎĆĽ - ŔŻŔú¸¦ °ř°ÝÇĎÁö ľĘ´Â´Ů.
// 	{
// 		return;
// 	}
// 
// 	////////////////////////////////////////////////////////////////////////////////
// 	// 2009-09-09 ~ 2010 by dhjin, ŔÎÇÇ´ĎĆĽ - ¸ó˝şĹÍżˇ°ÔĽ­ Ĺ¸°Ů şŻ°ć ľČµÇ´Â ¸ó˝şĹÍŔĚ¸é ¸®ĹĎÇŃ´Ů.
// 	if(BELL_INFINITY_ATTACK_MONSTER == ptmNMonster->MonsterInfoPtr->Belligerence
// 		&& FALSE == ptmNMonster->MonsterInfoPtr->ChangeTarget) {
// 		return;
// 	}
// 
// 	///////////////////////////////////////////////////////////////////////////////
// 	// °ř°ÝÇŃ Äł¸ŻĹÍŔÇ ŔŻČżĽşŔ» ĂĽĹ©ÇŃ´Ů
// 	CLIENT_INFO * ptmClient = GetClientInfo(i_pAttackInfo->AttackIndex);
// 	if(NULL == ptmClient
// 		|| ptmClient->ClientState == CS_NULL
// 		|| TRUE == COMPARE_BODYCON_BIT(ptmClient->BodyCondition, BODYCON_DEAD_MASK))
// 	{
// 		return;
// 	}
// 	
// // 2009-12-11 by cmkwon, µĄąĚÁö ľî±×·Î·Î Ĺ¸°ŮŔ» şŻ°ćÇĎ´Â ¸ó˝şĹÍ ±¸Çö - CNPCMapChannel::ProcessNPCOnBattleSetAttackCharacter#, ľĆ·ˇżÍ °°ŔĚ ĽöÁ¤ÇÔ.
// // 	///////////////////////////////////////////////////////////////////////////////
// // 	// ¸ó˝şĹÍŔÇ °ř°Ý ľĆŔĚĹŰŔĚ ľřŔ¸¸é ¸®ĹĎÇŃ´Ů(żŔşęÁ§Ć® ¸ó˝şĹÍŔĎ¶§)
// // 	if(NULL == ptmNMonster->m_pUsingMonsterItem)
// // 	{
// // 		return;
// // 	}
// // 
// // 	D3DXVECTOR3						TargetVector3;
// // 	float							fDistanceGap;
// // 	vector<ClientIndex_t>			vecClientTemp;
// // 	vector<ClientIndex_t>::iterator	itr;
// // 	DWORD							dwCur = timeGetTime();
// // 	int								nBeforeTargetIndex = ptmNMonster->m_nTargetIndex;
// // 
// // 	vecClientTemp.reserve(10);
// // 	////////////////////////////////////////////////////////////////////////////////
// // 	// 2009-09-09 ~ 2010 by dhjin, ŔÎÇÇ´ĎĆĽ - 
// // 	if(0 < i_pAttackInfo->DamageAmount 
// // 		&& ptmNMonster->m_HPAction.CheckValidSizeTalkDamagedRadom()) {
// // 		int CurrentMonHPRate = 1;
// // 		if(1 < ptmNMonster->CurrentHP) {
// // 			CurrentMonHPRate = (ptmNMonster->CurrentHP * 100) / ptmNMonster->MonsterInfoPtr->MonsterHP;
// // 		}
// // 		
// // 		HPACTION_TALK_HPRATE MsgTalk;
// // 		MEMSET_ZERO(&MsgTalk, sizeof(HPACTION_TALK_HPRATE));
// // 		if(ptmNMonster->m_HPAction.GetTalkDamagedRandom(MsgTalk.HPTalk)) {
// // 			// ÇÇ°Ý˝Ă ´ëČ­°ˇ ŔÖ´Ů¸é ŔüĽŰÇŃ´Ů.
// // 			MsgTalk.HPTalkImportance	= HPACTION_TALK_IMPORTANCE_CHANNEL;
// // 			this->SendFSvrHPTalk(ptmNMonster, &MsgTalk);
// // 		}
// // 	}	
// // 
// // 	if(i_pAttackInfo->ItemKind != ITEMKIND_MINE)
// // 	{	// °ř°Ý ąŢŔş ą«±â°ˇ ¸¶ŔÎŔĚ ľĆ´Ď´Ů
// // 				
// // 		ptmNMonster->InserttoAttackedInfoList(&ActionInfo(i_pAttackInfo->AttackIndex, dwCur, i_pAttackInfo->DamageAmount));
// // 
// // 		///////////////////////////////////////////////////////////////////////////////
// // 		// ŔĚŔü TargetIndex°ˇ 0ŔĚ°í °ř°ÝąŢŔş ¸ó˝şĹÍ°ˇ ¸öĹë °ř°Ý ľĆŔĚĹŰŔĚ ľĆ´Ń°ćżě Ăł¸®
// // 		if(0 == nBeforeTargetIndex
// // 			&& ptmNMonster->m_pUsingMonsterItem->pItemInfo->OrbitType != ORBIT_BODYSLAM)
// // 		{
// // 			TargetVector3 = ptmClient->PositionVector - ptmNMonster->PositionVector;
// // 			fDistanceGap = D3DXVec3Length(&TargetVector3);
// // 			if(fDistanceGap > ptmNMonster->m_pUsingMonsterItem->pItemInfo->Range
// // 				&& dwCur%100 < 50)
// // 			{
// // 				ptmNMonster->SetEnforceTargetVector(&TargetVector3, ptmNMonster->GetSpeed()*1.5f, MSS_OUT_OF_ATTACK_RANGE);
// // 			}
// // 		}		
// // 
// // 		///////////////////////////////////////////////////////////////////////////////
// // 		// ÇöŔç TargetIndex°ˇ ĽłÁ¤µÇ°í °ř°ÝąŢŔş ¸ó˝şĹÍ°ˇ µżÁ·ĽşÇâŔĚ°ĹłŞ ĆÄĆĽ¸ó˝şĹÍ ŔĎ¶§
// // 		if(0 != ptmNMonster->m_nTargetIndex
// // 			&& ptmClient->IsTargetableCharacter()		// 2006-11-27 by dhjin, ĽöÁ¤ÇÔ(FALSE == ptmClient->bStealthState -> ptmClient->IsTargetableCharacter())
// // 			&& (ptmNMonster->MonsterInfoPtr->AttackObject == ATTACKOBJ_SAMERACE || COMPARE_MPOPTION_BIT(ptmNMonster->MonsterInfoPtr->MPOption, MPOPTION_BIT_MOVE_PARTY))
// // 			)
// // 		{		
// // 			///////////////////////////////////////////////////////////////////////////////
// // 			// ÁÖŔ§żˇĽ­ ¸ó˝şĹÍ¸¦ °Ë»öÇŃ´Ů
// // 			NPCGetAdjacentMonsterIndexes(&ptmNMonster->PositionVector, MONSTER_ATTACKOBJ_SAMERACE_MAX_DISTANCE, MONSTER_ATTACKOBJ_SAMERACE_MAX_DISTANCE<<1, &vecClientTemp);
// // 			itr = vecClientTemp.begin();
// // 			while(itr != vecClientTemp.end())
// // 			{
// // 				CNPCMonster *pMTemp = GetNPCMonster(*itr);
// // 				if(pMTemp
// // 					&& pMTemp->m_enMonsterState != MS_NULL
// // 					&& COMPARE_BODYCON_BIT(pMTemp->BodyCondition, BODYCON_DEAD_MASK) == FALSE
// // 					&& pMTemp->MonsterInfoPtr->MonsterUnitKind == ptmNMonster->MonsterInfoPtr->MonsterUnitKind
// // 					&& pMTemp->m_nTargetIndex == 0)
// // 				{
// // 					pMTemp->SetTargetIndex(ptmNMonster->m_nTargetIndex);
// // 					if(pMTemp->m_pUsingMonsterItem->pItemInfo->OrbitType != ORBIT_BODYSLAM)
// // 					{
// // 						TargetVector3 = ptmClient->PositionVector - pMTemp->PositionVector;
// // 						fDistanceGap = D3DXVec3Length(&TargetVector3);
// // 						if(fDistanceGap > ptmNMonster->m_pUsingMonsterItem->pItemInfo->Range
// // 							&& dwCur%100 < 50)
// // 						{
// // 							pMTemp->SetEnforceTargetVector(&TargetVector3, pMTemp->GetSpeed()*1.5f, MSS_OUT_OF_ATTACK_RANGE);
// // 						}
// // 					}
// // 				}
// // 				itr++;
// // 			}
// // 		}		
// // 	}
// // 	else if(0 == nBeforeTargetIndex)
// // 	{// °ř°Ý ąŢŔş ą«±â°ˇ ¸¶ŔÎŔĚ¸ç ¸ó˝şĹÍ°ˇ Ĺ¸°ŮŔĚ ľřŔ»¶§, ¸ó˝şĹÍ°ˇ ÁÖŔ§żˇĽ­ Ĺ¸°ŮŔ» ĂŁ´Â´Ů
// // 		
// // 		///////////////////////////////////////////////////////////////////////////////
// // 		// ÁÖŔ§żˇĽ­ Äł¸ŻĹÍ¸¦ °Ë»öÇŃ´Ů
// // 		if(0 != NPCGetAdjacentCharacterIndexes(&ptmNMonster->PositionVector, ptmNMonster->m_pUsingMonsterItem->pItemInfo->Range
// // 			, ptmNMonster->m_pUsingMonsterItem->pItemInfo->Range*2, &vecClientTemp))
// // 		{	
// // 			///////////////////////////////////////////////////////////////////////////////
// // 			// °Ë»öÇŃ Äł¸ŻĹÍÁßżˇĽ­ Ĺ¸°Ů ŔÎµ¦˝ş¸¦ ĽłÁ¤ÇŃ´Ů. 
// // 			ptmNMonster->SelectTargetIndex(NPCGetTargetwithAttackObj(ATTACKOBJ_CLOSERANGE, ptmNMonster, vecClientTemp));
// // 
// // 			///////////////////////////////////////////////////////////////////////////////
// // 			// ÇöŔç TargetIndex°ˇ ĽłÁ¤µÇ°í °ř°ÝąŢŔş ¸ó˝şĹÍ°ˇ µżÁ·ĽşÇâŔĚ°ĹłŞ ĆÄĆĽ¸ó˝şĹÍ ŔĎ¶§
// // 			if( ptmNMonster->m_nTargetIndex != 0
// // 				&& ptmClient->IsTargetableCharacter()		// 2006-11-27 by dhjin, ĽöÁ¤ÇÔ(FALSE == ptmClient->bStealthState -> ptmClient->IsTargetableCharacter())
// // 				&& (ptmNMonster->MonsterInfoPtr->AttackObject == ATTACKOBJ_SAMERACE || COMPARE_MPOPTION_BIT(ptmNMonster->MonsterInfoPtr->MPOption, MPOPTION_BIT_MOVE_PARTY)) )
// // 			{
// // 
// // 				///////////////////////////////////////////////////////////////////////////////
// // 				// ÁÖŔ§żˇĽ­ ¸ó˝şĹÍ¸¦ °Ë»öÇŃ´Ů
// // 				NPCGetAdjacentMonsterIndexes(&ptmNMonster->PositionVector, MONSTER_ATTACKOBJ_SAMERACE_MAX_DISTANCE, MONSTER_ATTACKOBJ_SAMERACE_MAX_DISTANCE<<1, &vecClientTemp);
// // 				itr = vecClientTemp.begin();
// // 				while(itr != vecClientTemp.end())
// // 				{
// // 					
// // 					CNPCMonster *pMTemp = GetNPCMonster(*itr);
// // 					if(pMTemp
// // 						&& pMTemp->m_enMonsterState != MS_NULL
// // 						&& COMPARE_BODYCON_BIT(pMTemp->BodyCondition, BODYCON_DEAD_MASK) == FALSE
// // 						&& pMTemp->MonsterInfoPtr->MonsterUnitKind == ptmNMonster->MonsterInfoPtr->MonsterUnitKind
// // 						&& pMTemp->m_nTargetIndex == 0)
// // 					{
// // 						pMTemp->SetTargetIndex(ptmNMonster->m_nTargetIndex);
// // 						if(pMTemp->m_pUsingMonsterItem->pItemInfo->OrbitType != ORBIT_BODYSLAM)
// // 						{
// // 							TargetVector3 = ptmClient->PositionVector - pMTemp->PositionVector;
// // 							fDistanceGap = D3DXVec3Length(&TargetVector3);
// // 							if(fDistanceGap > pMTemp->m_pUsingMonsterItem->pItemInfo->Range
// // 								&& dwCur%100 < 50)
// // 							{
// // 								pMTemp->SetEnforceTargetVector(&TargetVector3, pMTemp->GetSpeed()*1.5f, MSS_OUT_OF_ATTACK_RANGE);
// // 							}
// // 						}
// // 					}
// // 					itr++;
// // 				}
// // 			}
// // 		}
// // 	}
// 	///////////////////////////////////////////////////////////////////////////////
// 	// 2009-12-11 by cmkwon, µĄąĚÁö ľî±×·Î·Î Ĺ¸°ŮŔ» şŻ°ćÇĎ´Â ¸ó˝şĹÍ ±¸Çö - CNPCMapChannel::ProcessNPCOnBattleSetAttackCharacter#
// 	MONSTER_INFO	*pAttMonsterInfo	= ptmNMonster->MonsterInfoPtr;
// 	MONSTER_ITEM	*pAttMonsterItem	= ptmNMonster->m_pUsingMonsterItem;
// 	if(NULL == pAttMonsterInfo
// 		|| NULL == pAttMonsterItem)
// 	{
// 		return;
// 	}
// 
// 	D3DXVECTOR3						TargetVector3;
// 	float							fDistanceGap;
// 	vector<ClientIndex_t>			vecClientTemp;
// 	vector<ClientIndex_t>::iterator	itr;
// 	DWORD							dwCur = timeGetTime();
// 	int								nBeforeTargetIndex = ptmNMonster->m_nTargetIndex;
// 
// 	vecClientTemp.reserve(10);
// 	////////////////////////////////////////////////////////////////////////////////
// 	// 2009-09-09 ~ 2010 by dhjin, ŔÎÇÇ´ĎĆĽ - 
// 	if(0 < i_pAttackInfo->DamageAmount 
// 		&& ptmNMonster->m_HPAction.CheckValidSizeTalkDamagedRadom()) {
// 		int CurrentMonHPRate = 1;
// 		if(1 < ptmNMonster->CurrentHP) {
// 			CurrentMonHPRate = (ptmNMonster->CurrentHP * 100) / pAttMonsterInfo->MonsterHP;
// 		}
// 		
// 		HPACTION_TALK_HPRATE MsgTalk;
// 		MEMSET_ZERO(&MsgTalk, sizeof(HPACTION_TALK_HPRATE));
// 		if(ptmNMonster->m_HPAction.GetTalkDamagedRandom(MsgTalk.HPTalk)) {
// 			// ÇÇ°Ý˝Ă ´ëČ­°ˇ ŔÖ´Ů¸é ŔüĽŰÇŃ´Ů.
// 			MsgTalk.HPTalkImportance	= HPACTION_TALK_IMPORTANCE_CHANNEL;
// 			this->SendFSvrHPTalk(ptmNMonster, &MsgTalk);
// 		}
// 	}	
// 
// 	ptmNMonster->InserttoAttackedInfoList(&ActionInfo(i_pAttackInfo->AttackIndex, dwCur, i_pAttackInfo->DamageAmount));
// 
// 	///////////////////////////////////////////////////////////////////////////////
// 	// ŔĚŔü TargetIndex°ˇ 0ŔĚ°í °ř°ÝąŢŔş ¸ó˝şĹÍ°ˇ ¸öĹë °ř°Ý ľĆŔĚĹŰŔĚ ľĆ´Ń°ćżě Ăł¸®
// 	if(0 == nBeforeTargetIndex
// 		&& pAttMonsterItem->pItemInfo->OrbitType != ORBIT_BODYSLAM)
// 	{
// 		TargetVector3 = ptmClient->PositionVector - ptmNMonster->PositionVector;
// 		fDistanceGap = D3DXVec3Length(&TargetVector3);
// 		if(fDistanceGap > pAttMonsterItem->pItemInfo->Range
// 			&& dwCur%100 < 50)
// 		{
// 			ptmNMonster->SetEnforceTargetVector(&TargetVector3, ptmNMonster->GetSpeed()*1.5f, MSS_OUT_OF_ATTACK_RANGE);
// 		}
// 	}		
// 
// 	///////////////////////////////////////////////////////////////////////////////
// 	// ÇöŔç TargetIndex°ˇ ĽłÁ¤µÇ°í °ř°ÝąŢŔş ¸ó˝şĹÍ°ˇ µżÁ·ĽşÇâŔĚ°ĹłŞ ĆÄĆĽ¸ó˝şĹÍ ŔĎ¶§
// 	ClientIndex_t curTargetIdx = ptmNMonster->GetTargetIndex();
// 	if(0 != curTargetIdx
// 		&& ptmClient->IsTargetableCharacter()
// 		&& (pAttMonsterInfo->AttackObject == ATTACKOBJ_SAMERACE || COMPARE_MPOPTION_BIT(pAttMonsterInfo->MPOption, MPOPTION_BIT_MOVE_PARTY))
// 		)
// 	{		
// 		///////////////////////////////////////////////////////////////////////////////
// 		// ÁÖŔ§żˇĽ­ ¸ó˝şĹÍ¸¦ °Ë»öÇŃ´Ů
// 		NPCGetAdjacentMonsterIndexes(&ptmNMonster->PositionVector, MONSTER_ATTACKOBJ_SAMERACE_MAX_DISTANCE, MONSTER_ATTACKOBJ_SAMERACE_MAX_DISTANCE<<1, &vecClientTemp);
// 		itr = vecClientTemp.begin();
// 		for(; itr != vecClientTemp.end(); itr++)
// 		{
// 			CNPCMonster *pNPCMon = GetNPCMonster(*itr);
// 			if(pNPCMon
// 				&& pNPCMon->IsValidMonster()				
// 				&& pNPCMon->MonsterInfoPtr->MonsterUnitKind == pAttMonsterInfo->MonsterUnitKind
// 				&& 0 == pNPCMon->GetTargetIndex())
// 			{
// 				pNPCMon->SelectTargetIndex(curTargetIdx);
// 			}		
// 		}
// 	}
	///////////////////////////////////////////////////////////////////////////////
	// 2010-03-16 by cmkwon, ŔÎÇÇ2Â÷ MtoM, MtoC Ĺ¸°Ů şŻ°ć °ü·Ă ĽöÁ¤ - 
	CLIENT_INFO *pAttackerCli = NULL;
	CNPCMonster	*pAttackerNMon = NULL;
	if(FALSE == this->GetUnitObject(i_pAttackInfo->AttackIndex, &pAttackerCli, &pAttackerNMon))
	{
		return;
	}
	CNPCMonster *pTargetNMon = GetNPCMonster(i_pAttackInfo->TargetIndex);
	if(NULL == pTargetNMon
		|| FALSE == pTargetNMon->IsValidMonster())
	{
		return;
	}

	///////////////////////////////////////////////////////////////////////////////
	// 2009-12-11 by cmkwon, µĄąĚÁö ľî±×·Î·Î Ĺ¸°ŮŔ» şŻ°ćÇĎ´Â ¸ó˝şĹÍ ±¸Çö - CNPCMapChannel::ProcessNPCOnBattleSetAttackCharacter#
	MONSTER_INFO	*pTarMonsterInfo	= pTargetNMon->MonsterInfoPtr;
	MONSTER_ITEM	*pTarMonsterItem	= pTargetNMon->m_pUsingMonsterItem;
	if(NULL == pTarMonsterInfo
		|| NULL == pTarMonsterItem)
	{
		return;
	}

	////////////////////////////////////////////////////////////////////////////////
	// 2009-09-09 ~ 2010 by dhjin, ŔÎÇÇ´ĎĆĽ - 
	if(0 < i_pAttackInfo->DamageAmount 
		&& pTargetNMon->m_HPAction.CheckValidSizeTalkDamagedRadom())
	{
		int CurrentMonHPRate = 1;
		if(1 < pTargetNMon->CurrentHP)
		{
			CurrentMonHPRate = (pTargetNMon->CurrentHP * 100) / pTarMonsterInfo->MonsterHP;
		}
		
		HPACTION_TALK_HPRATE MsgTalk;
		MEMSET_ZERO(&MsgTalk, sizeof(HPACTION_TALK_HPRATE));
		if(pTargetNMon->m_HPAction.GetTalkDamagedRandom(MsgTalk.HPTalk)) {
			// ÇÇ°Ý˝Ă ´ëČ­°ˇ ŔÖ´Ů¸é ŔüĽŰÇŃ´Ů.
			MsgTalk.HPTalkImportance	= HPACTION_TALK_IMPORTANCE_CHANNEL;
			this->SendFSvrHPTalk(pTargetNMon, &MsgTalk);
		}
	}
	int		nBeforeTargetIndex = pTargetNMon->GetTargetIndex();
	DWORD	dwCur = timeGetTime();
	pTargetNMon->InserttoAttackedInfoList(&ActionInfo(i_pAttackInfo->AttackIndex, dwCur, i_pAttackInfo->DamageAmount));

	///////////////////////////////////////////////////////////////////////////////
	// ŔĚŔü TargetIndex°ˇ 0ŔĚ°í °ř°ÝąŢŔş ¸ó˝şĹÍ°ˇ ¸öĹë °ř°Ý ľĆŔĚĹŰŔĚ ľĆ´Ń°ćżě Ăł¸®
	if(0 == nBeforeTargetIndex
		&& pTarMonsterItem->pItemInfo->OrbitType != ORBIT_BODYSLAM)
	{
		D3DXVECTOR3		TargetVector3;
		float			fDistanceGap = 0.0f;
		TargetVector3	= (pAttackerCli?pAttackerCli->PositionVector : pAttackerNMon->PositionVector) - pTargetNMon->PositionVector;
		fDistanceGap	= D3DXVec3Length(&TargetVector3);
		if(fDistanceGap > pTarMonsterItem->pItemInfo->Range
			&& dwCur%100 < 50)
		{
			pTargetNMon->SetEnforceTargetVector(&TargetVector3, pTargetNMon->GetSpeed()*1.5f, MSS_OUT_OF_ATTACK_RANGE);
		}
	}

	///////////////////////////////////////////////////////////////////////////////
	// ÇöŔç TargetIndex°ˇ ĽłÁ¤µÇ°í °ř°ÝąŢŔş ¸ó˝şĹÍ°ˇ µżÁ·ĽşÇâŔĚ°ĹłŞ ĆÄĆĽ¸ó˝şĹÍ ŔĎ¶§
	ClientIndex_t curTargetIdx = pTargetNMon->GetTargetIndex();
	if(0 != curTargetIdx
		&& (pTarMonsterInfo->AttackObject == ATTACKOBJ_SAMERACE || COMPARE_MPOPTION_BIT(pTarMonsterInfo->MPOption, MPOPTION_BIT_MOVE_PARTY))
		)
	{		
		vector<ClientIndex_t>			vecClientTemp;				
		vecClientTemp.reserve(10);
		///////////////////////////////////////////////////////////////////////////////
		// ÁÖŔ§żˇĽ­ ¸ó˝şĹÍ¸¦ °Ë»öÇŃ´Ů
		NPCGetAdjacentMonsterIndexes(&pTargetNMon->PositionVector, MONSTER_ATTACKOBJ_SAMERACE_MAX_DISTANCE, MONSTER_ATTACKOBJ_SAMERACE_MAX_DISTANCE<<1, &vecClientTemp, pTarMonsterInfo->MonsterUnitKind);
		vector<ClientIndex_t>::iterator	itr = vecClientTemp.begin();
		for(; itr != vecClientTemp.end(); itr++)
		{
			CNPCMonster *pNPCMon = GetNPCMonster(*itr);
			if(pNPCMon
				&& pNPCMon->IsValidMonster()
				&& 0 == pNPCMon->GetTargetIndex())
			{
				pNPCMon->SelectTargetIndex(curTargetIdx);
			}		
		}
	}
}

void CNPCMapChannel::ProcessNPCOnCityWarStartWar(MSG_FN_CITYWAR_START_WAR *i_pStartWar)
{
	m_bCityWarStarted = TRUE;			// CityWar°ˇ ˝ĂŔŰµĘ
}
void CNPCMapChannel::ProcessNPCOnCityWarEndWar(MSG_FN_CITYWAR_END_WAR *i_pEndWar)
{
	m_bCityWarStarted	= FALSE;		// CityWar°ˇ Áľ·áµĘ
	this->SetCityWarOccupyGuildUID(i_pEndWar->OccupyGuildUID4);

	this->DeleteAllMonster();
// 2007-08-22 by cmkwon, ÇŘ´ç ¸ĘĂ¤łÎ ¸ó˝şĹÍ ¸đµÎ »čÁ¦ÇĎ±â ±â´É Ăß°ˇ - Ŕ§żÍ °°ŔĚ ÇÔĽö·Î şŻ°ć
//	///////////////////////////////////////////////////////////////////////////////
//	// ¸ĘľČŔÇ ¸đµç ¸ó˝şĹÍ¸¦ Á¦°ĹÇŃ´Ů
//	mt_auto_lock mtA(&m_mtvectorMonsterPtr);
//	INIT_MSG_WITH_BUFFER(MSG_FN_MONSTER_DELETE, T_FN_MONSTER_DELETE, pMonsterDelete, SendBuf);
//	pMonsterDelete->ChannelIndex	= m_MapChannelIndex.ChannelIndex;
//	for(int i=0; i < m_mtvectorMonsterPtr.size(); i++)
//	{
//		CNPCMonster *pNMon = (CNPCMonster*)m_mtvectorMonsterPtr[i];
//		if(MS_PLAYING == pNMon->m_enMonsterState
//			&& FALSE == COMPARE_BODYCON_BIT(pNMon->BodyCondition, BODYCON_DEAD_MASK))
//		{	
//			pMonsterDelete->MonsterIndex	= pNMon->MonsterIndex;
//			Send2FieldServerW(SendBuf, MSG_SIZE(MSG_FN_MONSTER_DELETE));
//
//			this->DelelteMonsterHandler(pNMon);
//		}
//	}
}
void CNPCMapChannel::ProcessNPCOnCityWarChangeOccupyInfo(MSG_FN_CITYWAR_CHANGE_OCCUPY_INFO *i_pChangeOccupyInfo)
{	
	this->SetCityWarOccupyGuildUID(i_pChangeOccupyInfo->OccupyGuildUID4);

	///////////////////////////////////////////////////////////////////////////////
	// ¸ĘľČŔÇ ¸đµç ¸ó˝şĹÍŔÇ Ĺ¸°ŮŔÎµ¦˝şżˇĽ­ Áˇ·Éż©´ÜżřŔ» Á¦°ĹÇŃ´Ů
	for(int i=0; i < m_mtvectorMonsterPtr.size(); i++)
	{
		CNPCMonster *pNMon = (CNPCMonster*)m_mtvectorMonsterPtr[i];
		if(MS_PLAYING == pNMon->m_enMonsterState
			&& FALSE == COMPARE_BODYCON_BIT(pNMon->BodyCondition, BODYCON_DEAD_MASK)
			&& 0 != pNMon->m_nTargetIndex)
		{
			CLIENT_INFO *pCInfo = GetClientInfo(pNMon->m_nTargetIndex);
			if(pCInfo
				&& pCInfo->GuildUID10 == m_CityWarOccupyGuildUID)
			{
				pNMon->SetTargetIndex(0);
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			void CNPCMapChannel::ProcessNPCOnMonsterStrategyPointInit(MSG_FN_MONSTER_STRATEGYPOINT_INIT *i_pMsg)
/// \brief		
/// \author		cmkwon
/// \date		2006-11-20 ~ 2006-11-20
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CNPCMapChannel::ProcessNPCOnMonsterStrategyPointInit(MSG_FN_MONSTER_STRATEGYPOINT_INIT *i_pMsg)
{
	///////////////////////////////////////////////////////////////////////////////
	// 1. ¸ĘľČŔÇ Ŕü·«Ć÷ŔÎĆ® ¸ó˝şĹÍ¸¦ Á¦°ĹÇŃ´Ů
	// 2007-09-20 by cmkwon, ĹÚ·ąĆ÷Ć® ĽöÁ¤ - ĹÚ·ąĆ÷Ć®µµ »čÁ¦Ăł¸®ÇŃ´Ů.
	this->DeleteAllMonster(FALSE, BELL_STRATEGYPOINT_VCN, BELL_STRATEGYPOINT_ANI);
	this->DeleteAllMonster(FALSE, BELL_INFLUENCE_TELEPORT_VCN, BELL_INFLUENCE_TELEPORT_ANI);	// 2007-09-20 by cmkwon, ĹÚ·ąĆ÷Ć® ĽöÁ¤ - ĹÚ·ąĆ÷Ć®µµ »čÁ¦Ăł¸®ÇŃ´Ů.
// 2007-08-22 by cmkwon, ÇŘ´ç ¸ĘĂ¤łÎ ¸ó˝şĹÍ ¸đµÎ »čÁ¦ÇĎ±â ±â´É Ăß°ˇ - Ŕ§żÍ °°ŔĚ ÇÔĽö·Î ´ëĂĽ
//	mt_auto_lock mtA(&m_mtvectorMonsterPtr);
//	for(int i=0; i < m_mtvectorMonsterPtr.size(); i++)
//	{
//		CNPCMonster *pNMon		= (CNPCMonster*)m_mtvectorMonsterPtr[i];
//		MONSTER_INFO *pMonInfo	= pNMon->MonsterInfoPtr;					// 2006-12-13 by cmkwon
//		if(MS_NULL != pNMon->m_enMonsterState
//			&& pMonInfo
//			&& IS_STRATEGYPOINT_MONSTER(pMonInfo->Belligerence))
//		{
//			///////////////////////////////////////////////////////////////////////////////
//			// 2006-11-20 by cmkwon, ąŮ·Î »čÁ¦Ăł¸®ÇŃ´Ů.
//			INIT_MSG_WITH_BUFFER(MSG_FN_MONSTER_DELETE, T_FN_MONSTER_DELETE, pMonsterDelete, SendBuf);
//			pMonsterDelete->ChannelIndex	= m_MapChannelIndex.ChannelIndex;
//			pMonsterDelete->MonsterIndex	= pNMon->MonsterIndex;
//			Send2FieldServerW(SendBuf, MSG_SIZE(MSG_FN_MONSTER_DELETE));
//
//			DelelteMonsterHandler(pNMon);
//		}
//	}
//	mtA.auto_unlock_cancel();

	if(i_pMsg->bCreateFlag)
	{// 2006-11-20 by cmkwon, »čÁ¦ ČÄ ąŮ·Î Ŕü·«Ć÷ŔÎĆ® ¸ó˝şĹÍ¸¦ ĽŇČŻ ÇŃ´Ů
		
		BYTE					SendBuf[SIZE_MAX_PACKET];

		// 2007-08-18 by cmkwon, żŔşęÁ§Ć® ¸ó˝şĹÍ ĽŇČŻ Á¤ş¸żˇ MONSTER_INFO * ĽłÁ¤ÇĎ±â - ľĆ·ˇżÍ °°ŔĚ Belligerence °Ë»öŔ¸·Î ĽöÁ¤
		//OBJECTINFOSERVER *pObjBossMonsterObjInfo = FindObjectBossMonsterInfo();
		OBJECTINFOSERVER *pObjBossMonsterObjInfo = this->FindObjectMonsterInfoBYBelligerence(i_pMsg->bVCNMapInflTyforInit?BELL_STRATEGYPOINT_VCN:BELL_STRATEGYPOINT_ANI);
		if(NULL == pObjBossMonsterObjInfo)
		{
			return;
		}

		///////////////////////////////////////////////////////////////////////////////
		// 2007-10-18 by cmkwon, °ĹÁˇ¸ó˝şĹÍ ŔÚµż ĽŇ¸ę ˝Ă°Ł ĽłÁ¤ ĽöÁ¤
		if(COMPARE_MPOPTION_BIT(pObjBossMonsterObjInfo->m_pMonsterInfo->MPOption, MPOPTION_BIT_AUTO_DESTROY))
		{
			if(FALSE == i_pMsg->bInfluenceBoss)
			{
				// 2007-09-21 by cmkwon, ¸đĽ±Ŕü˝Ă °ĹÁˇ ¸ó˝şĹÍ ĽŇČŻŔĚ ľĆ´ĎąÇ·Î  ŔÚµż ĽŇ¸ę˝Ă°ŁŔ» STRATEGYPOINT_SUMMONTIME_BY_INFLUENCEBOSS/2 ·Î ĽłÁ¤ÇŃ´Ů. 
				pObjBossMonsterObjInfo->m_pMonsterInfo->MPOptionParam1	= STRATEGYPOINT_SUMMONTIME_BY_INFLUENCEBOSS / 4; //26-02-2024 by Inet changed from /2 to /4 to 30 minute SP
			}
			else
			{
				// 2007-09-21 by cmkwon, ¸đĽ±Ŕü˝Ă °ĹÁˇ ¸ó˝şĹÍ ĽŇČŻŔĚąÇ·Î  ŔÚµż ĽŇ¸ę˝Ă°ŁŔ» STRATEGYPOINT_SUMMONTIME_BY_INFLUENCEBOSS ·Î ĽłÁ¤ÇŃ´Ů. 
				// 2007-12-07 by dhjin, ŔÚµż ĽŇ¸ę ˝Ă°Ł ąö±×·Î ąŘ°ú °°ŔĚ ĽöÁ¤, 130şĐŔ¸·Î ĽöÁ¤
				// pObjBossMonsterObjInfo->m_pMonsterInfo->MPOptionParam1	= STRATEGYPOINT_SUMMONTIME_BY_INFLUENCEBOSS;
				pObjBossMonsterObjInfo->m_pMonsterInfo->MPOptionParam1	= STRATEGYPOINT_SUMMONTIME_BY_INFLUENCEBOSS + 10;
			}
		}
		this->CreateMonstersBYObject(SendBuf, pObjBossMonsterObjInfo, timeGetTime(), TRUE);

// 2006-11-22 by cmkwon, ŔĎ´Ü Ŕü·«Ć÷ŔÎĆ® ¸ó˝şĹÍ´Â żŔşęÁ§Ć® ¸ó˝şĹÍ·Î ¸¸µé °ÍŔÓ
//		else
//		{
//			vector<D3DXVECTOR3>		vectVec3;
//			int nMonsterInfoSize = m_pNPCMapProject->m_vectorMONSTER_CREATE_REGION_INFO.size();
//			for(int i=0; i < nMonsterInfoSize; i++)
//			{
//				MONSTER_CREATE_REGION_INFO *pMonsterCreateRegionInfo = &m_pNPCMapProject->m_vectorMONSTER_CREATE_REGION_INFO[i];
//				MONSTER_INFO *pMonInfo = m_pNPCIOCPServer->GetMonsterInfo(pMonsterCreateRegionInfo->sMonType);
//				if(pMonInfo
//					&& IS_STRATEGYPOINT_MONSTER(pMonInfo->Belligerence))
//				{
//					this->CreateMonstersBYRegion(SendBuf, &vectVec3, timeGetTime(), i, pMonsterCreateRegionInfo, &m_vectorMonsterCreateRegionInfoEX[i], TRUE);
//					break;
//				}
//			}
//		}
	}
	else if(FALSE == i_pMsg->bCreateFlag && FALSE == i_pMsg->bInfluenceBoss)
	{// 2007-09-16 by dhjin, ¸đĽ±ŔĚ ĆÄ±«µČ °ćżě´Â ¸ó˝şĹÍ ĽŇČŻŔ» Çă¶ôÇŃ´Ů.
		// 2009-12-04 by cmkwon, ¸ó˝şĹÍ »ýĽş±ÝÁö °ü·Ă ·Î±× Ăß°ˇ - ľĆ·ˇżÍ °°ŔĚ SetNotCreateMonsterValue()ÇÔĽö·Î şŻ°ć
		//m_bNotCreateMonster	= FALSE;
		this->SetNotCreateMonsterValue(FALSE);
	}
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			void CNPCMapChannel::ProcessNPCOnMonsterDeleteMonsterInMapChannel(MSG_FN_NPCSERVER_DELETE_MONSTER_IN_MAPCHANNEL *i_pMsg)
/// \brief		// 2007-08-22 by cmkwon, ÇŘ´ç ¸ĘĂ¤łÎ ¸ó˝şĹÍ ¸đµÎ »čÁ¦ÇĎ±â ±â´É Ăß°ˇ
/// \author		cmkwon
/// \date		2007-08-22 ~ 2007-08-22
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CNPCMapChannel::ProcessNPCOnMonsterDeleteMonsterInMapChannel(MSG_FN_NPCSERVER_DELETE_MONSTER_IN_MAPCHANNEL *i_pMsg)
{
	if(i_pMsg->bNotCreateMonster)
	{// 2007-08-29 by dhjin, ¸ó˝şĹÍ »ýĽş ±ÝÁöŔĚ¸é m_bNotCreateMonster = TRUE·Î ĽłÁ¤ÇĎż© ¸ó˝şĹÍżˇ ¸®Á¨Ŕ» ¸·´Â´Ů.
		// 2009-12-04 by cmkwon, ¸ó˝şĹÍ »ýĽş±ÝÁö °ü·Ă ·Î±× Ăß°ˇ - ľĆ·ˇżÍ °°ŔĚ SetNotCreateMonsterValue()ÇÔĽö·Î şŻ°ć
		//m_bNotCreateMonster = TRUE;
		this->SetNotCreateMonsterValue(TRUE);
	}

	this->DeleteAllMonster(i_pMsg->bAllFlag, i_pMsg->bell1, i_pMsg->bell2, i_pMsg->excludeBell1, i_pMsg->excludeBell2);
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			void CNPCMapChannel::ProcessNPCOnMonsterOutPostInit(MSG_FN_MONSTER_OUTPOST_INIT *i_pMsg)
/// \brief		
/// \author		dhjin
/// \date		2007-08-24 ~ 2007-08-24
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CNPCMapChannel::ProcessNPCOnMonsterOutPostInit(MSG_FN_MONSTER_OUTPOST_INIT *i_pMsg)
{
	// 2007-10-02 by dhjin, Ĺ×˝şĆ® żë
	g_pNPCGlobal->WriteSystemLogEX(TRUE, "[Notify]: OUTPOST_SUMMON bell 1 : [%6d], bell 2 : [%6d], bell 3 : [%6d]\r\n"
	, i_pMsg->bell1, i_pMsg->bell2, i_pMsg->bell3);
	
	///////////////////////////////////////////////////////////////////////////////
	// 1. ¸ĘľČŔÇ Ŕü·«Ć÷ŔÎĆ® ¸ó˝şĹÍ¸¦ Á¦°ĹÇŃ´Ů
	this->DeleteAllMonster(TRUE);


	// 2. ŔüÁř±âÁö ¸Ężˇ »ýĽşÇŘľß µÇ´Â ¸ó˝şĹÍ ŔüşÎ ĽŇČŻ
	BYTE	SendBuf[SIZE_MAX_PACKET];
	
	vectorObjectInfoServer::iterator itr(m_vectorObjectMonsterInfoCopy.begin());
	for(; itr != m_vectorObjectMonsterInfoCopy.end(); itr++)
	{
		if(itr->m_pMonsterInfo
			&& (itr->m_pMonsterInfo->Belligerence == i_pMsg->bell1 
				|| itr->m_pMonsterInfo->Belligerence == i_pMsg->bell2
				|| itr->m_pMonsterInfo->Belligerence == i_pMsg->bell3) )
		{
			OBJECTINFOSERVER *pObjBossMonsterObjInfo = &*itr;
			if(pObjBossMonsterObjInfo)
			{// 2006-11-22 by cmkwon
				this->CreateMonstersBYObject(SendBuf, pObjBossMonsterObjInfo, timeGetTime(), TRUE);
				
				// 2007-10-02 by dhjin, Ĺ×˝şĆ® żë
				g_pNPCGlobal->WriteSystemLogEX(TRUE, "[Notify]: OUTPOST_SUMMON bell : [%6d]\r\n"
				, pObjBossMonsterObjInfo->m_pMonsterInfo->Belligerence);
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			void CNPCMapChannel::ProcessNPCOnMonsterCinemaMonsterCreate(MSG_FN_NPCSERVER_CINEMA_MONSTER_CREATE *i_pMsg)
/// \brief		// 2010-03-31 by dhjin, ŔÎÇÇ´ĎĆĽ(±âÁöąćľî) - ŔÎÇÇ´ĎĆĽ - Key¸ó˝şĹÍ »ýĽş
///				// 2010-03-31 by cmkwon, ŔÎÇÇ2Â÷ Ăß°ˇ ĽöÁ¤ - 
/// \author		dhjin
/// \date		2009-09-09 ~ 2010
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CNPCMapChannel::ProcessNPCOnMonsterCinemaMonsterCreate(MSG_FN_NPCSERVER_CINEMA_MONSTER_CREATE *i_pMsg)
{

// 2010-03-31 by cmkwon, ŔÎÇÇ2Â÷ Ăß°ˇ ĽöÁ¤ - ľĆ·ˇżÍ °°ŔĚ ĽöÁ¤.	
// 	// żŔşęÁ§Ć® ¸ó˝şĹÍ Á¤ş¸ °ˇÁ®żŔ±â
// 	OBJECTINFOSERVER CinemaObjectMonster;
// 	if(FALSE == this->GetObjectMonsterByMonsterIdx(i_pMsg->MonsterUnitKind, &CinemaObjectMonster)) {
// 		g_pNPCGlobal->WriteSystemLogEX(TRUE, "[Infinity]: CinemaMonsterCreate Fail!!!!! MapIdx : [%6d], MapChannel : [%6d], MonsterIdx : [%6d]\r\n"
// 					, i_pMsg->mapChann.MapIndex, i_pMsg->mapChann.ChannelIndex, i_pMsg->MonsterUnitKind);		
// 		return;
// 	}
// 
// 	CNPCMonster				*pCinemaMonster = NULL;
// 	BYTE					SendBuf[SIZE_MAX_PACKET];
// 	DWORD					tick = timeGetTime();
// 	D3DXVECTOR3				RandomposVector3;
// 	
// 	MONSTER_INFO * pCinemaMonInfo = m_pNPCIOCPServer->GetMonsterInfo(i_pMsg->MonsterUnitKind);
// 	if(NULL == pCinemaMonInfo)
// 	{
// 		g_pNPCGlobal->WriteSystemLogEX(TRUE, "[Error] CNPCMapProject::NPCAdminSummonMonster invalid parameter, MonsterUnitKind[%d]\r\n", i_pMsg->MonsterUnitKind);		
// 		return;
// 	}
// 
// 	mt_auto_lock mtMon(&m_mtvectorMonsterPtr);
// 	for(int i=0; i < i_pMsg->MonsterSummonCount; i++) 
// 	{	// ĽŇČŻ µÇľîľß µÇ´Â Ľö¸¸Ĺ­ ĽŇČŻ!
// 		
// 		if(m_vectorUsableMonsterIndex.empty())
// 		{
// 			g_pNPCGlobal->WriteSystemLogEX(TRUE, "[Error] CNPCMapProject::ProcessNPCOnMonsterCinemaMonsterCreate Monster is full. ArrayMonsterSize[%d], CurrentMonsterCount(%d)\r\n"
// 				, m_nSizemtvectorMonsterPtr, m_nCurMonsterCountInChannel);		
// 			return;
// 		}
// 		
// 		pCinemaMonster = this->GetInitNPCMonster();
// 		if(NULL == pCinemaMonster
// 			|| pCinemaMonster->m_enMonsterState != MS_NULL)
// 		{
// 			return;
// 		}
// 		// ¸ó˝şĹÍ »ýĽş, ±âş»°Ş ĽĽĆĂ
// 		m_nCurMonsterCountInChannel++;			
// 
// 		// ·Ł´ý ÁÂÇĄ ĽłÁ¤
// 		RandomposVector3.x	= CinemaObjectMonster.m_vPos.x + RANDI(0, i_pMsg->MaxRandomDistance);
// 		RandomposVector3.y	= CinemaObjectMonster.m_vPos.y + RANDI(0, i_pMsg->MaxRandomDistance);
// 		RandomposVector3.z	= CinemaObjectMonster.m_vPos.z + RANDI(0, i_pMsg->MaxRandomDistance);
// 		RandomposVector3.x	= min(RandomposVector3.x, this->GetSizeMapXW()-1.0f);
// 		RandomposVector3.z	= min(RandomposVector3.z, this->GetSizeMapZW()-1.0f);
// 
// 		////////////////////////////////////////////////////////////////////////
// 		// 2009-09-09 ~ 2010 by dhjin, ŔÎÇÇ´ĎĆĽ - HPAction ·Îµů
// 		vectHPAction * pvectHPAction = m_pNPCIOCPServer->m_mapHPAction.findEZ_ptr(pCinemaMonInfo->HPActionIdx);
// 		if(NULL != pvectHPAction) {
// 			pCinemaMonster->m_HPAction.InitHPActionListByDB(pvectHPAction);
// 		}
// 		
// 		// ¸ó˝şĹÍ »ýĽş, ±âş»°Ş ĽĽĆĂ
// 		pCinemaMonster->SetCurrentMapChannel(this);				// 2009-09-09 ~ 2010 by dhjin, ŔÎÇÇ´ĎĆĽ - ¸Ę Ă¤łÎ ąĚ¸® ĽłÁ¤
// 		pCinemaMonster->CreateNPCMonster(pCinemaMonInfo, &RandomposVector3, tick, 0xFFFF
// 			, MONSTER_TARGETTYPE_NORMAL, 0, 0, this->GetMonsterVisibleDiameterW(), 0, CinemaObjectMonster.m_EventInfo.m_bEventType, &CinemaObjectMonster.m_vVel);
// 		SetInitialPositionAndSendCreateMonster(pCinemaMonster, SendBuf);
// 		////////////////////////////////////////////////////////////////////////
// 		// 2009-09-09 ~ 2010 by dhjin, ŔÎÇÇ´ĎĆĽ - 
// 		g_pNPCGlobal->WriteSystemLogEX(TRUE, "[Infinity]: CinemaMonsterCreate MapIdx : [%6d], MapChannel : [%6d], MonsterIdx : [%6d]\r\n"
// 				, i_pMsg->mapChann.MapIndex, i_pMsg->mapChann.ChannelIndex, i_pMsg->MonsterUnitKind);
// 	}
	///////////////////////////////////////////////////////////////////////////////
	// 2010-03-31 by cmkwon, ŔÎÇÇ2Â÷ Ăß°ˇ ĽöÁ¤ - 
	MONSTER_INFO * pCinemaMonInfo = m_pNPCIOCPServer->GetMonsterInfo(i_pMsg->MonsterUnitKind);
	if(NULL == pCinemaMonInfo)
	{
		g_pNPCGlobal->WriteSystemLogEX(TRUE, "[Error] CNPCMapProject::NPCAdminSummonMonster invalid parameter, MonsterUnitKind[%d]\r\n", i_pMsg->MonsterUnitKind);		
		return;
	}

	vectorObjectInfoServerPtr tmObjInfoServList;
	this->GetObjectMonsterByMonsterIdx(i_pMsg->MonsterUnitKind, &tmObjInfoServList);
	if(tmObjInfoServList.empty())
	{
		g_pNPCGlobal->WriteSystemLogEX(TRUE, "[Infinity]: CinemaMonsterCreate Fail!!!!! MapIdx : [%6d], MapChannel : [%6d], MonsterIdx : [%6d]\r\n"
					, i_pMsg->mapChann.MapIndex, i_pMsg->mapChann.ChannelIndex, i_pMsg->MonsterUnitKind);		
		return;
	}

	mt_auto_lock mtMon(&m_mtvectorMonsterPtr);

	OBJECTINFOSERVER *pObjInfo4Summon = NULL;
	vectorObjectInfoServerPtr::iterator itr(tmObjInfoServList.begin());
	for(; itr != tmObjInfoServList.end(); itr++)
	{
		pObjInfo4Summon	= *itr;

		BYTE					SendBuf[SIZE_MAX_PACKET];
		for(int i=0; i < i_pMsg->MonsterSummonCount; i++) 
		{	// ĽŇČŻ µÇľîľß µÇ´Â Ľö¸¸Ĺ­ ĽŇČŻ!
			
			if(m_vectorUsableMonsterIndex.empty())
			{
				g_pNPCGlobal->WriteSystemLogEX(TRUE, "[Error] CNPCMapProject::ProcessNPCOnMonsterCinemaMonsterCreate Monster is full. ArrayMonsterSize[%d], CurrentMonsterCount(%d)\r\n"
					, m_nSizemtvectorMonsterPtr, m_nCurMonsterCountInChannel);		
				return;
			}
			
			CNPCMonster *pCinemaMonster = this->GetInitNPCMonster();
			if(NULL == pCinemaMonster
				|| pCinemaMonster->m_enMonsterState != MS_NULL)
			{
				return;
			}
			// ¸ó˝şĹÍ »ýĽş, ±âş»°Ş ĽĽĆĂ
			m_nCurMonsterCountInChannel++;			
			
			// ·Ł´ý ÁÂÇĄ ĽłÁ¤
			D3DXVECTOR3				RandomposVector3;
			RandomposVector3.x	= pObjInfo4Summon->m_vPos.x + RANDI(0, i_pMsg->MaxRandomDistance);
			RandomposVector3.y	= pObjInfo4Summon->m_vPos.y + RANDI(0, i_pMsg->MaxRandomDistance);
			RandomposVector3.z	= pObjInfo4Summon->m_vPos.z + RANDI(0, i_pMsg->MaxRandomDistance);
			RandomposVector3.x	= min(RandomposVector3.x, this->GetSizeMapXW()-1.0f);
			RandomposVector3.z	= min(RandomposVector3.z, this->GetSizeMapZW()-1.0f);
			
			////////////////////////////////////////////////////////////////////////
			// 2009-09-09 ~ 2010 by dhjin, ŔÎÇÇ´ĎĆĽ - HPAction ·Îµů
			vectHPAction * pvectHPAction = m_pNPCIOCPServer->m_mapHPAction.findEZ_ptr(pCinemaMonInfo->HPActionIdx);
			if(NULL != pvectHPAction) {
				pCinemaMonster->m_HPAction.InitHPActionListByDB(pvectHPAction);
			}
			
			// ¸ó˝şĹÍ »ýĽş, ±âş»°Ş ĽĽĆĂ
			pCinemaMonster->SetCurrentMapChannel(this);				// 2009-09-09 ~ 2010 by dhjin, ŔÎÇÇ´ĎĆĽ - ¸Ę Ă¤łÎ ąĚ¸® ĽłÁ¤

			// 2010. 05. 19 by hsLee ŔÎÇÇ´ĎĆĽ ÇĘµĺ 2Â÷ ł­ŔĚµµ Á¶Ŕý. (˝ĹČŁĂł¸® + ¸ó˝şĹÍ Ăł¸®(Ľ­ąö) ) - ¸ó˝şĹÍ Č®ŔĺÁ¤ş¸(ąë·±˝ş) Ăß°ˇ.
			pCinemaMonster->CreateNPCMonster(pCinemaMonInfo, &RandomposVector3, timeGetTime(), 0xFFFF
				, MONSTER_TARGETTYPE_NORMAL, 0, 0, this->GetMonsterVisibleDiameterW(), 0, pObjInfo4Summon->m_EventInfo.m_bEventType, &pObjInfo4Summon->m_vVel , &i_pMsg->MonsterBalanceInfo );
			
			// End 2010. 05. 19 by hsLee ŔÎÇÇ´ĎĆĽ ÇĘµĺ 2Â÷ ł­ŔĚµµ Á¶Ŕý. (˝ĹČŁĂł¸® + ¸ó˝şĹÍ Ăł¸®(Ľ­ąö) )
			SetInitialPositionAndSendCreateMonster(pCinemaMonster, SendBuf);
			////////////////////////////////////////////////////////////////////////
			// 2009-09-09 ~ 2010 by dhjin, ŔÎÇÇ´ĎĆĽ - 
//			g_pNPCGlobal->WriteSystemLogEX(TRUE, "[Infinity]: CNPCMapChannel::ProcessNPCOnMonsterCinemaMonsterCreate# Map(%s), MonNum(%6d) MonIdx(%d) Summon(%d/%d) ObjListCnt(%d) \r\n"
//				, GetMapString(i_pMsg->mapChann, string()), i_pMsg->MonsterUnitKind, pCinemaMonster->MonsterIndex, i, i_pMsg->MonsterSummonCount, tmObjInfoServList.size());
		}
	} // END - for(; itr != tmObjInfoServList.end(); itr++)
}

// start 2011-04-28 by hskim, ŔÎÇÇ´ĎĆĽ 3Â÷ - ˝Ăł×¸¶ °ü·Ă ±â´É Ăß°ˇ - ÇŘ´ç ¸ĘĂ¤łÎ ĆŻÁ¤ ¸ó˝şĹÍ »čÁ¦/şŻ°ć ±â´É Ăß°ˇ
///////////////////////////////////////////////////////////////////////////////
/// \fn			void CNPCMapChannel::ProcessNPCOnMonsterCinemaMonsterDestroy(MSG_FN_NPCSERVER_CINEMA_MONSTER_CREATE *i_pMsg)
/// \brief		// 2011-04-28 by hskim, ŔÎÇÇ´ĎĆĽ 3Â÷ - ˝Ăł×¸¶ °ü·Ă ±â´É Ăß°ˇ - ÇŘ´ç ¸ĘĂ¤łÎ ĆŻÁ¤ ¸ó˝şĹÍ »čÁ¦ ±â´É Ăß°ˇ
/// \author		hskim
/// \date		20110428
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CNPCMapChannel::ProcessNPCOnMonsterCinemaMonsterDestroy(MSG_FN_NPCSERVER_CINEMA_MONSTER_DESTROY *i_pMsg)
{
	this->DeleteUnitKindMonster(i_pMsg->MonsterUnitKind);
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			void CNPCMapChannel::ProcessNPCOnMonsterCinemaMonsterChange(MSG_FN_NPCSERVER_CINEMA_MONSTER_CHANGE *i_pMsg)
/// \brief		// 2011-04-28 by hskim, ŔÎÇÇ´ĎĆĽ 3Â÷ - ˝Ăł×¸¶ °ü·Ă ±â´É Ăß°ˇ - ÇŘ´ç ¸ĘĂ¤łÎ ĆŻÁ¤ ¸ó˝şĹÍ şŻ°ć ±â´É Ăß°ˇ
/// \author		hskim
/// \date		20110511
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CNPCMapChannel::ProcessNPCOnMonsterCinemaMonsterChange(MSG_FN_NPCSERVER_CINEMA_MONSTER_CHANGE *i_pMsg)
{
	this->ChangeUnitKindMonster(i_pMsg->MonsterUnitKind, i_pMsg->ChangeMonsterUnitKind);
}
// end 2011-04-28 by hskim, ŔÎÇÇ´ĎĆĽ 3Â÷ - ˝Ăł×¸¶ °ü·Ă ±â´É Ăß°ˇ - ÇŘ´ç ¸ĘĂ¤łÎ ĆŻÁ¤ ¸ó˝şĹÍ »čÁ¦/şŻ°ć ±â´É Ăß°ˇ

///////////////////////////////////////////////////////////////////////////////
/// \fn			CNPCMonster * CNPCMapChannel::GetInitNPCMonster() 
/// \brief		// 2010-03-31 by dhjin, ŔÎÇÇ´ĎĆĽ(±âÁöąćľî) - 
/// \author		dhjin
/// \date		2009-09-09 ~ 2010
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
CNPCMonster * CNPCMapChannel::GetInitNPCMonster() 
{	// 2010-03-31 by dhjin, ŔÎÇÇ´ĎĆĽ(±âÁöąćľî) -
	int		nIdxMonster;
	nIdxMonster = m_vectorUsableMonsterIndex.front();
	m_vectorUsableMonsterIndex.pop_front();
	return GetNPCMonster(nIdxMonster);
}


///////////////////////////////////////////////////////////////////////////////
/// \fn			void CNPCMapChannel::NPCOnSummonObjectMonsterBYBelligerence(int i_nbell)
/// \brief		
/// \author		dhjin
/// \date		2007-08-24 ~ 2007-08-24
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CNPCMapChannel::NPCOnSummonObjectMonsterBYBelligerence(int i_nbell)
{
	BYTE					SendBuf[SIZE_MAX_PACKET];

	OBJECTINFOSERVER *pObjBossMonsterObjInfo = this->FindObjectMonsterInfoBYBelligerence(i_nbell);
	if(pObjBossMonsterObjInfo)
	{// 2006-11-22 by cmkwon

		if(IS_STRATEGYPOINT_MONSTER(pObjBossMonsterObjInfo->m_pMonsterInfo->Belligerence)
			&& COMPARE_MPOPTION_BIT(pObjBossMonsterObjInfo->m_pMonsterInfo->MPOption, MPOPTION_BIT_AUTO_DESTROY))
		{
			// 2007-09-21 by cmkwon, ¸đĽ±Ŕü˝Ă °ĹÁˇ ¸ó˝şĹÍ ĽŇČŻŔĚ ľĆ´ĎąÇ·Î  ŔÚµż ĽŇ¸ę˝Ă°ŁŔ» STRATEGYPOINT_SUMMONTIME_BY_INFLUENCEBOSS/2 ·Î ĽłÁ¤ÇŃ´Ů. 
			pObjBossMonsterObjInfo->m_pMonsterInfo->MPOptionParam1	= STRATEGYPOINT_SUMMONTIME_BY_INFLUENCEBOSS / 4; //26-02-2024 by Inet changed from /2 to /4 to 30 minute SP
		}
		this->CreateMonstersBYObject(SendBuf, pObjBossMonsterObjInfo, timeGetTime(), TRUE);
	}	
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			void CNPCMapChannel::ProcessNPCOnMonsterSummonByBell(MSG_MONSTER_SUMMON_BY_BELL *i_pMsg)
/// \brief		// 2007-09-19 by cmkwon, Bell·Î ĽŇČŻ Ăł¸®
/// \author		cmkwon
/// \date		2007-09-19 ~ 2007-09-19
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CNPCMapChannel::ProcessNPCOnMonsterSummonByBell(MSG_MONSTER_SUMMON_BY_BELL *i_pMsg)
{
	this->NPCOnSummonObjectMonsterBYBelligerence(i_pMsg->MonsterBell);
}

BOOL CNPCMapChannel::NPCOnMonsterChangeBodycondition(MSG_FN_MONSTER_CHANGE_BODYCONDITION * i_pChange)
{
	CNPCMonster * ptmNMonster = GetNPCMonster(i_pChange->ClientIndex);
	if(NULL == ptmNMonster || ptmNMonster->m_enMonsterState == MS_NULL){		return FALSE;}

	ptmNMonster->ChangeBodyCondition(&i_pChange->BodyCondition);
	if(COMPARE_BODYCON_BIT(ptmNMonster->BodyCondition, BODYCON_DEAD_MASK))
	{
		ptmNMonster->m_enMonsterState = MS_DEAD;

		// 2010-05-17 by shcho - ŔÎÇÇ´ĎĆĽ ¸ó˝şĹÍ °ü·Ă ·Î±× ł˛±â±â 
		if(g_pNPCGlobal->GetIsArenaServer()
			&& 9201 == m_MapChannelIndex.MapIndex)	// ľĆ·ąłŞ Ľ­ąöżÍ ŔÎÇÇ´ĎĆĽ 1Â÷(9201)ŔĎ¶§¸¸ Âď´Â´Ů.
		{
			g_pNPCGlobal->WriteSystemLogEX(TRUE,"  %10s : <= DeadMonster MonsterUnitKind[%8d]\r\n",GET_MAP_STRING(m_MapChannelIndex),ptmNMonster->MonsterInfoPtr->MonsterUnitKind);
		}
	}
	
	return TRUE;
}


BOOL CNPCMapChannel::NPCOnBattleDropFixer(MSG_FN_BATTLE_DROP_FIXER * i_pFixer)
{
	CNPCMonster *pMON = this->GetNPCMonster(i_pFixer->TargetIndex);
	ITEM *pIT = m_pNPCIOCPServer->GetItemInfo(i_pFixer->ItemNum);
	if(pMON
		&& pMON->m_enMonsterState != MS_NULL
		&& pIT)
	{
		AttackedItemInfo aItem;
		aItem.AttackIndex = i_pFixer->AttackIndex;
		aItem.pAttackITEM = pIT;
		aItem.dwExpireTick = timeGetTime() + pIT->Time;
		aItem.AttackedItemIndex = m_uiAttackedItemUniqueIndex++;
		pMON->InsertAttackedItemInfo(&aItem);

		INIT_MSG_WITH_BUFFER(MSG_FN_BATTLE_DROP_FIXER_OK, T_FN_BATTLE_DROP_FIXER_OK, pSFixerOK, SendB);
		pSFixerOK->ChannelIndex		= m_MapChannelIndex.ChannelIndex;
		pSFixerOK->AttackIndex		= i_pFixer->AttackIndex;
		pSFixerOK->TargetIndex		= i_pFixer->TargetIndex;
		pSFixerOK->ItemNum			= i_pFixer->ItemNum;
		pSFixerOK->ItemFieldIndex	= aItem.AttackedItemIndex;
		this->Send2FieldServerW(SendB, MSG_SIZE(MSG_FN_BATTLE_DROP_FIXER_OK));
	}
	return TRUE;
}

BOOL CNPCMapChannel::NPCOnMonsterSkillEndSkill(MSG_FN_MONSTER_SKILL_END_SKILL * i_pEndSkill)
{
	m_mtvectorMSG_FN_MONSTER_SKILL_END_SKILL.lock();
	m_mtvectorMSG_FN_MONSTER_SKILL_END_SKILL.push_back(*i_pEndSkill);
	m_mtvectorMSG_FN_MONSTER_SKILL_END_SKILL.unlock();
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			BOOL CNPCMapChannel::NPCOnMonsterStrategyPointInit(MSG_FN_MONSTER_STRATEGYPOINT_INIT * i_pMsg)
/// \brief		
/// \author		cmkwon
/// \date		2006-11-20 ~ 2006-11-20
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CNPCMapChannel::NPCOnMonsterStrategyPointInit(MSG_FN_MONSTER_STRATEGYPOINT_INIT * i_pMsg)
{
	mt_auto_lock mtA(&m_mtvectorMSG_FN_MONSTER_STRATEGYPOINT_INIT);
	m_mtvectorMSG_FN_MONSTER_STRATEGYPOINT_INIT.push_back(*i_pMsg);	
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			BOOL CNPCMapChannel::NPCOnMonsterOutPostInit(MSG_FN_MONSTER_OUTPOST_INIT * i_pMsg)
/// \brief		
/// \author		dhjin
/// \date		2007-08-24 ~ 2007-08-24
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CNPCMapChannel::NPCOnMonsterOutPostInit(MSG_FN_MONSTER_OUTPOST_INIT * i_pMsg)
{
	mt_auto_lock mtA(&m_mtvectorMSG_FN_MONSTER_OUTPOST_INIT);
	m_mtvectorMSG_FN_MONSTER_OUTPOST_INIT.push_back(*i_pMsg);	
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			BOOL CNPCMapChannel::NPCOnMonsterSummonByBell(MSG_MONSTER_SUMMON_BY_BELL * i_pMsg)
/// \brief		// 2007-09-19 by cmkwon, Bell·Î ĽŇČŻ Ăł¸®
/// \author		cmkwon
/// \date		2007-09-19 ~ 2007-09-19
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CNPCMapChannel::NPCOnMonsterSummonByBell(MSG_MONSTER_SUMMON_BY_BELL * i_pMsg)
{
	mt_auto_lock mtA(&m_mtvectMSG_MONSTER_SUMMON_BY_BELL);
	m_mtvectMSG_MONSTER_SUMMON_BY_BELL.push_back(*i_pMsg);	
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			BOOL CNPCMapChannel::NPCOnMonsterCinemaMonsterCreate(MSG_FN_NPCSERVER_CINEMA_MONSTER_CREATE * i_pMsg)
/// \brief		// 2010-03-31 by dhjin, ŔÎÇÇ´ĎĆĽ(±âÁöąćľî) - ŔÎÇÇ´ĎĆĽ - Key¸ó˝şĹÍ »ýĽş 
/// \author		dhjin
/// \date		2009-09-09 ~ 2010
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CNPCMapChannel::NPCOnMonsterCinemaMonsterCreate(MSG_FN_NPCSERVER_CINEMA_MONSTER_CREATE * i_pMsg) {
	mt_auto_lock mtA(&m_mtvectorMSG_FN_NPCSERVER_CINEMA_MONSTER_CREATE);
	m_mtvectorMSG_FN_NPCSERVER_CINEMA_MONSTER_CREATE.push_back(*i_pMsg);	
	return TRUE;
}

// start 2011-04-28 by hskim, ŔÎÇÇ´ĎĆĽ 3Â÷ - ˝Ăł×¸¶ °ü·Ă ±â´É Ăß°ˇ - ÇŘ´ç ¸ĘĂ¤łÎ ĆŻÁ¤ ¸ó˝şĹÍ »čÁ¦/şŻ°ć ±â´É Ăß°ˇ
///////////////////////////////////////////////////////////////////////////////
/// \fn			BOOL CNPCMapChannel::NPCOnMonsterCinemaMonsterDestroy(MSG_FN_NPCSERVER_CINEMA_MONSTER_DESTROY * i_pMsg)
/// \brief		// 2011-04-28 by hskim, ŔÎÇÇ´ĎĆĽ 3Â÷ - ˝Ăł×¸¶ °ü·Ă ±â´É Ăß°ˇ - ÇŘ´ç ¸ĘĂ¤łÎ ĆŻÁ¤ ¸ó˝şĹÍ »čÁ¦ ±â´É Ăß°ˇ
/// \author		hskim
/// \date		2011-04-28
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CNPCMapChannel::NPCOnMonsterCinemaMonsterDestroy(MSG_FN_NPCSERVER_CINEMA_MONSTER_DESTROY * i_pMsg) {
	mt_auto_lock mtA(&m_mtvectorMSG_FN_NPCSERVER_CINEMA_MONSTER_DESTROY);
	m_mtvectorMSG_FN_NPCSERVER_CINEMA_MONSTER_DESTROY.push_back(*i_pMsg);	
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			BOOL CNPCMapChannel::NPCOnMonsterCinemaMonsterChange(MSG_FN_NPCSERVER_CINEMA_MONSTER_CHANGE * i_pMsg)
/// \brief		// 2011-04-28 by hskim, ŔÎÇÇ´ĎĆĽ 3Â÷ - ˝Ăł×¸¶ °ü·Ă ±â´É Ăß°ˇ - ÇŘ´ç ¸ĘĂ¤łÎ ĆŻÁ¤ ¸ó˝şĹÍ şŻ°ć ±â´É Ăß°ˇ
/// \author		hskim
/// \date		2011-05-11
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CNPCMapChannel::NPCOnMonsterCinemaMonsterChange(MSG_FN_NPCSERVER_CINEMA_MONSTER_CHANGE * i_pMsg) {
	mt_auto_lock mtA(&m_mtvectorMSG_FN_NPCSERVER_CINEMA_MONSTER_CHANGE);
	m_mtvectorMSG_FN_NPCSERVER_CINEMA_MONSTER_CHANGE.push_back(*i_pMsg);	
	return TRUE;
}
// end 2011-04-28 by hskim, ŔÎÇÇ´ĎĆĽ 3Â÷ - ˝Ăł×¸¶ °ü·Ă ±â´É Ăß°ˇ - ÇŘ´ç ¸ĘĂ¤łÎ ĆŻÁ¤ ¸ó˝şĹÍ »čÁ¦/şŻ°ć ±â´É Ăß°ˇ

// start 2011-06-02 ŔÎÇÇ´ĎĆĽ 3Â÷ - ˝şĹÜ 6 - ÁÖ±âŔű ĽŇČŻ ±â´É Á¦ŔŰ
///////////////////////////////////////////////////////////////////////////////
/// \fn			BOOL CNPCMapChannel::NPCOnMonsterCinemaMonsterRegen(MSG_FN_NPCSERVER_CINEMA_MONSTER_REGEN * i_pMsg)
/// \brief		// 2011-06-02 ŔÎÇÇ´ĎĆĽ 3Â÷ - ˝şĹÜ 6 - ÁÖ±âŔű ĽŇČŻ ±â´É Á¦ŔŰ
/// \author		hskim
/// \date		2011-05-11
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CNPCMapChannel::NPCOnMonsterCinemaMonsterRegen(MSG_FN_NPCSERVER_CINEMA_MONSTER_REGEN * i_pMsg) 
{
	DWORD dwCurTick = timeGetTime();

	vectorObjectInfoServer::iterator itr(m_vectorObjectMonsterInfoCopy.begin());
	for(; itr != m_vectorObjectMonsterInfoCopy.end(); itr++)
	{
		if( itr->m_pMonsterInfo->MonsterUnitKind == i_pMsg->iMonsterUnitKind )
		{
			if( TRUE == i_pMsg->bRegen )
			{
				itr->m_EventInfo.m_dwLastTimeObjectMonsterCreated = dwCurTick;
				itr->MonsterBalanceInfo = i_pMsg->MonsterBalanceInfo;
				itr->m_bNotCreateMonster = FALSE;
			}
			else
			{
				itr->m_bNotCreateMonster = TRUE;
			}
		}
	}

	return TRUE;
}
// end 2011-06-02 ŔÎÇÇ´ĎĆĽ 3Â÷ - ˝şĹÜ 6 - ÁÖ±âŔű ĽŇČŻ ±â´É Á¦ŔŰ

BOOL CNPCMapChannel::NPCOnCityWarStart(MSG_FN_CITYWAR_START_WAR *i_pCityWarStart)
{
	mt_auto_lock mtA(&m_mtvectorMSG_FN_CITYWAR_START_WAR);
	m_mtvectorMSG_FN_CITYWAR_START_WAR.push_back(*i_pCityWarStart);	
	return TRUE;
}
BOOL CNPCMapChannel::NPCOnCityWarEnd(MSG_FN_CITYWAR_END_WAR *i_pCityWarEnd)
{
	mt_auto_lock mtA(&m_mtvectorMSG_FN_CITYWAR_END_WAR);
	m_mtvectorMSG_FN_CITYWAR_END_WAR.push_back(*i_pCityWarEnd);	
	return TRUE;
}
BOOL CNPCMapChannel::NPCOnCityWarChangeOccupyInfo(MSG_FN_CITYWAR_CHANGE_OCCUPY_INFO *i_pCityWarChangeOccupyInfo)
{
	mt_auto_lock mtA(&m_mtvectorMSG_FN_CITYWAR_CHANGE_OCCUPY_INFO);
	m_mtvectorMSG_FN_CITYWAR_CHANGE_OCCUPY_INFO.push_back(*i_pCityWarChangeOccupyInfo);	
	return TRUE;
}


void CNPCMapChannel::ProcessNPCOnMonsterSkillEndSkill(MSG_FN_MONSTER_SKILL_END_SKILL * i_pEndSkill)
{
	CNPCMonster * ptmNMonster = GetNPCMonster(i_pEndSkill->MonsterIndex);
	if(NULL == ptmNMonster || ptmNMonster->m_enMonsterState == MS_NULL){		return;}

// 2009-09-09 ~ 2010 by dhjin, ŔÎÇÇ´ĎĆĽ - ÄÁĆ®·Ń˝şĹł·Î ŔĚ¸§ şŻ°ć
//	if(FALSE == COMPARE_BODYCON_BIT(ptmNMonster->BodyCondition, BODYCON_MON_ATTACK6_MASK))
	if(FALSE == COMPARE_BODYCON_BIT(ptmNMonster->BodyCondition, BODYCON_MON_CONTROLSKILL_MASK))
	{
		return;
	}

	///////////////////////////////////////////////////////////////////////////////
	// BodyConditio Change and Send
	BodyCond_t tmBodyCon = ptmNMonster->BodyCondition;
// 2009-09-09 ~ 2010 by dhjin, ŔÎÇÇ´ĎĆĽ - ÄÁĆ®·Ń˝şĹł·Î ŔĚ¸§ şŻ°ć
//	CLEAR_BODYCON_BIT(tmBodyCon, BODYCON_MON_PREATTACK6_MASK);
	CLEAR_BODYCON_BIT(tmBodyCon, BODYCON_MON_PRECONTROLSKILL_MASK);
	ptmNMonster->ChangeBodyCondition(&tmBodyCon);
	INIT_MSG_WITH_BUFFER(MSG_FN_MONSTER_CHANGE_BODYCONDITION, T_FN_MONSTER_CHANGE_BODYCONDITION, pSeBody, tmSendBuf);
	pSeBody->ChannelIndex	= this->m_MapChannelIndex.ChannelIndex;
	pSeBody->ClientIndex	= ptmNMonster->MonsterIndex;
	pSeBody->BodyCondition	= ptmNMonster->BodyCondition;
	Send2FieldServerW(tmSendBuf, MSG_SIZE(MSG_FN_MONSTER_CHANGE_BODYCONDITION));
	
	ptmNMonster->m_BeforePosition = ptmNMonster->PositionVector;
	///////////////////////////////////////////////////////////////////////////////
	// Monster PositionVector and TargetVector Update
	ptmNMonster->PositionVector = A2DX(i_pEndSkill->PositionVector);
	ptmNMonster->SetTargetVector(&A2DX(i_pEndSkill->TargetVector));
	ptmNMonster->SetMoveTargetVector(&A2DX(i_pEndSkill->TargetVector));
	
	///////////////////////////////////////////////////////////////////////////////
	// Move State Change
	ptmNMonster->SetMoveState(MSS_NORMAL);
	
	///////////////////////////////////////////////////////////////////////////////
	// ŔĚµż ÁÂÇĄ Block Ăł¸®
	UpdateBlockPosition(ptmNMonster->m_BeforePosition.x, ptmNMonster->m_BeforePosition.z,
		ptmNMonster->PositionVector.x, ptmNMonster->PositionVector.z, ptmNMonster->MonsterIndex);
	
	///////////////////////////////////////////////////////////////////////////////
	// Send EndSkill
	INIT_MSG(MSG_FN_MONSTER_SKILL_END_SKILL, T_FN_MONSTER_SKILL_END_SKILL, pSeSkill, tmSendBuf);
	pSeSkill->ChannelIndex	= this->m_MapChannelIndex.ChannelIndex;
	pSeSkill->MonsterIndex	= ptmNMonster->MonsterIndex;
	pSeSkill->PositionVector= ptmNMonster->PositionVector;
	pSeSkill->TargetVector	= ptmNMonster->TargetVector*1000.0f;
	Send2FieldServerW(tmSendBuf, MSG_SIZE(MSG_FN_MONSTER_SKILL_END_SKILL));
}

void CNPCMapChannel::_UpdateAttackedItemInfo(CNPCMonster *i_pNMonster, BYTE *i_pSendBuf)
{
	if(i_pNMonster->m_mtvectorAttackedItemInfo.empty() == false)
	{	// ¸ó˝şĹÍ°ˇ ÇČĽ­ ľĆŔĚĹŰµîżˇ Ĺ¸°ÝŔ» ąŢľŇ´Ů

		i_pNMonster->LockVectorAttackedItemInfo();
		AttackedItemInfo	*pAItemInfo;
		float				fSpeedPenalty = 0.0f;
		mtvectorAttackedItemInfo::iterator itr(i_pNMonster->m_mtvectorAttackedItemInfo.begin());
		while(itr != i_pNMonster->m_mtvectorAttackedItemInfo.end())
		{
			pAItemInfo = &*itr;
			if(i_pNMonster->m_dwCurrentTick > pAItemInfo->dwExpireTick)
			{
				if(ITEMKIND_FIXER == pAItemInfo->pAttackITEM->Kind)
				{
					//////////////////////////////////////////////////////////
					// ÇČĽ­ ľĆŔĚĹŰŔ» Áöżě°í Field Serverżˇ ŔüĽŰÇŘÁŘ´Ů
					INIT_MSG(MSG_FN_BATTLE_ATTACK_HIDE_ITEM_W_KIND, T_FN_BATTLE_ATTACK_HIDE_ITEM_W_KIND, pSExplodeItem, i_pSendBuf);
					pSExplodeItem->ChannelIndex			= m_MapChannelIndex.ChannelIndex;
					pSExplodeItem->ItemKind				= pAItemInfo->pAttackITEM->Kind;
					pSExplodeItem->TargetIndex			= i_pNMonster->MonsterIndex;
					pSExplodeItem->TargetItemFieldIndex = pAItemInfo->AttackedItemIndex;
					Send2FieldServerW(i_pSendBuf, MSG_SIZE(MSG_FN_BATTLE_ATTACK_HIDE_ITEM_W_KIND));
				}
				itr = i_pNMonster->m_mtvectorAttackedItemInfo.erase(itr);

				continue;
			}

			if(ITEMKIND_FIXER == pAItemInfo->pAttackITEM->Kind)
			{
// 2009-04-21 by cmkwon, ITEMżˇ DesParam ÇĘµĺ °łĽö 8°ł·Î ´Ă¸®±â - ľĆ·ˇżÍ °°ŔĚ ĽöÁ¤
// 				if(DES_TRANSPORT == pAItemInfo->pAttackITEM->DestParameter1)
// 				{
// 					fSpeedPenalty += pAItemInfo->pAttackITEM->ParameterValue1;
// 				}
// 				if(DES_TRANSPORT == pAItemInfo->pAttackITEM->DestParameter2)
// 				{
// 					fSpeedPenalty += pAItemInfo->pAttackITEM->ParameterValue2;
// 				}
// 				if(DES_TRANSPORT == pAItemInfo->pAttackITEM->DestParameter3)
// 				{
// 					fSpeedPenalty += pAItemInfo->pAttackITEM->ParameterValue3;
// 				}
// 				if(DES_TRANSPORT == pAItemInfo->pAttackITEM->DestParameter4)
// 				{
// 					fSpeedPenalty += pAItemInfo->pAttackITEM->ParameterValue4;
// 				}
				if(pAItemInfo->pAttackITEM->IsExistDesParam(DES_TRANSPORT))
				{
					fSpeedPenalty += pAItemInfo->pAttackITEM->GetParameterValue(DES_TRANSPORT);
				}
			}
			else if(IS_SKILL_ITEM(pAItemInfo->pAttackITEM->Kind))
			{
// 2009-04-21 by cmkwon, ITEMżˇ DesParam ÇĘµĺ °łĽö 8°ł·Î ´Ă¸®±â - ľĆ·ˇżÍ °°ŔĚ ĽöÁ¤
// 				if(DES_SKILL_SLOWMOVING == pAItemInfo->pAttackITEM->DestParameter1)
// 				{
// 					fSpeedPenalty += pAItemInfo->pAttackITEM->ParameterValue1;
// 				}
// 				if(DES_SKILL_SLOWMOVING == pAItemInfo->pAttackITEM->DestParameter2)
// 				{
// 					fSpeedPenalty += pAItemInfo->pAttackITEM->ParameterValue2;
// 				}
// 				if(DES_SKILL_SLOWMOVING == pAItemInfo->pAttackITEM->DestParameter3)
// 				{
// 					fSpeedPenalty += pAItemInfo->pAttackITEM->ParameterValue3;
// 				}
// 				if(DES_SKILL_SLOWMOVING == pAItemInfo->pAttackITEM->DestParameter4)
// 				{
// 					fSpeedPenalty += pAItemInfo->pAttackITEM->ParameterValue4;
// 				}
				if(pAItemInfo->pAttackITEM->IsExistDesParam(DES_SKILL_SLOWMOVING))
				{
					fSpeedPenalty += pAItemInfo->pAttackITEM->GetParameterValue(DES_SKILL_SLOWMOVING);
				}
			}
			
			itr++;			
		}// end_while(itr != i_pNMonster->m_mtvectorAttackedItemInfo.end())

		i_pNMonster->m_usSpeedPercent = (USHORT)(i_pNMonster->m_usSpeedPercent - max(i_pNMonster->m_usSpeedPercent, (USHORT)i_pNMonster->m_usSpeedPercent*fSpeedPenalty)); 
		i_pNMonster->UnlockVectorAttackedItemInfo();
	}// end_if(i_pNMonster->m_mtvectorAttackedItemInfo.empty() == false)
}


///////////////////////////////////////////////////////////////////////////////
/// \fn			void CNPCMapChannel::_BattleAttackOnMonsterDead(BOOL *i_bDeleteProcess, CNPCMonster * i_pNPCMon)
/// \brief		
/// \author		cmkwon
/// \date		2005-12-17 ~ 2005-12-17
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CNPCMapChannel::_BattleAttackOnMonsterDead(BOOL *i_bDeleteProcess, CNPCMonster * i_pNPCMon, float fIncrSpawnRate)
{
	///////////////////////////////////////////////////////////////////////////////
	// 2008-04-16 by cmkwon, ¸ó˝şĹÍ »ç¸Á ˝Ă ¸ó˝şĹÍ ĽŇČŻ ŔĚşĄĆ® ˝Ă˝şĹŰ ±¸Çö - 
	SSUMMON_EVENT_MONSTER summonEvMon;
	MEMSET_ZERO(&summonEvMon, sizeof(SSUMMON_EVENT_MONSTER));
	*i_bDeleteProcess		= i_pNPCMon->CheckSummonEventMonsterListAfterDead(&summonEvMon, fIncrSpawnRate);
	if(summonEvMon.SummonMonsterNum)
	{// 2008-04-16 by cmkwon, ÇŘ´ç Á¤ş¸¸¦ °ˇÁö°í ĽŇČŻ Ăł¸® ÇŃ´Ů.
		this->NPCMonsterAttackSkill(i_pNPCMon, &summonEvMon);
	}

// 2008-04-16 by cmkwon, ¸ó˝şĹÍ »ç¸Á ˝Ă ¸ó˝şĹÍ ĽŇČŻ ŔĚşĄĆ® ˝Ă˝şĹŰ ±¸Çö - Ŕ§żÍ °°ŔĚ ĽöÁ¤ÇÔ
//	*i_bDeleteProcess		= TRUE;
//
// 	MONSTER_INFO *pMonInfo	= i_pNPCMon->MonsterInfoPtr;
// 	int nMaxDelayTick		= 0;
// 	for(int i=0; i < ARRAY_SIZE_MONSTER_ITEM; i++)
// 	{
// 		// 2005-10-28 by cmkwon, Á×Ŕ» ¶§ »çżë °ˇ´ÉÇŃ ľĆŔĚĹŰŔÎÁö ĂĽĹ©
// 		ITEM *pMonItem = pMonInfo->ItemInfo[i].pItemInfo;
// 		if(NULL == pMonItem
// 			|| 0 != pMonItem->Charging					// Charging=0 ŔÎ ľĆŔĚĹŰŔĚ Á×Ŕ» ¶§ »çżëÇĎ´Â ¸ó˝şĹÍ ľĆŔĚĹŰ
// 			|| DES_SUMMON != pMonItem->DestParameter1)	// ÇöŔç´Â ĽŇČŻ ľĆŔĚĹŰ¸¸ »çżë °ˇ´É
// 		{
// 			continue;
// 		}
// 
// 		nMaxDelayTick = max(nMaxDelayTick, pMonItem->AttackTime);
// 	}
// 
// 	if(0 >= nMaxDelayTick
// 		|| nMaxDelayTick <= i_pNPCMon->m_dwCurrentTick - i_pNPCMon->m_dwTimeDeath)
// 	{
// 		for(int i=0; i < ARRAY_SIZE_MONSTER_ITEM; i++)
// 		{
// 			// 2005-10-28 by cmkwon, »çżë °ˇ´ÉÇŃ ľĆŔĚĹŰŔÎÁö ĂĽĹ©
// 			ITEM *pMonItem = pMonInfo->ItemInfo[i].pItemInfo;
// 			if(NULL == pMonItem
// 				|| 0 != pMonItem->Charging					// Charging=0 ŔÎ ľĆŔĚĹŰŔĚ Á×Ŕ» ¶§ »çżëÇĎ´Â ¸ó˝şĹÍ ľĆŔĚĹŰ
// 				|| DES_SUMMON != pMonItem->DestParameter1)	// ÇöŔç´Â ĽŇČŻ ľĆŔĚĹŰ¸¸ »çżë °ˇ´É
// 			{
// 				continue;
// 			}
// 			// 2005-10-28 by cmkwon, »çżë Č®·ü ĂĽĹ©
// 			int nRand = RAND256();
// 			if(nRand > pMonItem->HitRate)
// 			{
// 				continue;
// 			}
// 
// 			this->NPCMonsterAttackSkill(i_pNPCMon, pMonItem);
// 		}
// 	}
// 	else
// 	{
// 		*i_bDeleteProcess = FALSE;				// 2005-12-17 by cmkwon, ľĆÁ÷ »čÁ¦ÇĎ¸é ľČµČ´Ů
// 	}
}


//////////////////////////////////////////////////////////////////
// 2°ˇÁö ÇüĹÂŔÇ MonsterFormŔ» °ˇÁř ¸ó˝şĹÍ Ăł¸®
//   - ŔĎ´Ü şń°ř°Ý »óĹÂ¸¸ MonsterFormŔÇ şŻČ­°ˇ ŔÖ´Ů
BOOL CNPCMapChannel::_CheckMonsterChangeMonsterForm(CNPCMonster * i_pNMonster)
{
	if(i_pNMonster->MonsterInfoPtr->MonsterForm == FORM_FLYINGandGROUND_RIGHT
		|| i_pNMonster->MonsterInfoPtr->MonsterForm == FORM_FLYINGandGROUND_COPTER)
	{
		i_pNMonster->m_nTimeGapChangeMonsterForm -= (i_pNMonster->m_dwCurrentTick - i_pNMonster->m_dwTimeLastMoved);
		if(i_pNMonster->m_nTimeGapChangeMonsterForm < 0)
		{
			i_pNMonster->m_nTimeGapChangeMonsterForm = 10000 + i_pNMonster->m_dwCurrentTick/5000;
			if(i_pNMonster->CurrentMonsterForm != FORM_GROUND_MOVE)
			{
				i_pNMonster->SetCurrentMonsterForm(FORM_GROUND_MOVE);
			}
			else if(i_pNMonster->MonsterInfoPtr->MonsterForm == FORM_FLYINGandGROUND_RIGHT)
			{
				i_pNMonster->SetCurrentMonsterForm(FORM_FLYING_RIGHT);
			}
			else if(i_pNMonster->MonsterInfoPtr->MonsterForm == FORM_FLYINGandGROUND_COPTER)
			{
				i_pNMonster->SetCurrentMonsterForm(FORM_FLYING_COPTER);
			}
			i_pNMonster->SetMonsterMoveInfo();
			
			BodyCond_t tmBody = 0;
			if(FORM_GROUND_MOVE == i_pNMonster->CurrentMonsterForm)
			{
				SET_BODYCON_BIT(tmBody, BODYCON_LANDED_MASK);
			}
			else
			{
				SET_BODYCON_BIT(tmBody, BODYCON_FLY_MASK|BODYCON_BOOSTER1_MASK);
			}
			i_pNMonster->ChangeBodyCondition(&tmBody);

			INIT_MSG_WITH_BUFFER(MSG_FN_MONSTER_CHANGE_BODYCONDITION, T_FN_MONSTER_CHANGE_BODYCONDITION, pSeBody, tmSenBuf);
			pSeBody->ChannelIndex	= this->m_MapChannelIndex.ChannelIndex;
			pSeBody->ClientIndex	= i_pNMonster->MonsterIndex;
			pSeBody->BodyCondition	= i_pNMonster->BodyCondition;
			Send2FieldServerW(tmSenBuf, MSG_SIZE(MSG_FN_MONSTER_CHANGE_BODYCONDITION));
			return TRUE;
		}
	}

	return FALSE;
}

D3DXVECTOR3 CNPCMapChannel::GetFrontPosition(CNPCMonster * i_pNPCMon)
{
	return i_pNPCMon->PositionVector + i_pNPCMon->m_MoveTargetVector * m_pNPCMapProject->m_fFrontPositionDistance;
}


void CNPCMapChannel::UpdateMonsterPositionHandler(CNPCMonster *i_pNMonster
												  , BYTE *i_pSendBuf
												  , vector<ClientIndex_t> *i_pvecClientIndex
												  , BOOL i_bNotMove)
{
	if(i_bNotMove)
	{
		SendMonsterMove2FieldServer(i_pNMonster, i_pSendBuf);
		return;
	}

	///////////////////////////////////////////////////////////////////////////////
	// ¸ó˝şĹÍ°ˇ °ř°ÝąŢŔş ľĆŔĚĹŰŔÇ Ăł¸®
	_UpdateAttackedItemInfo(i_pNMonster, i_pSendBuf);
	
	///////////////////////////////////////////////////////////////////////////////
	// ¸ó˝şĹÍŔÇ CurrentSpeedŔÇ Up or DownŔ» Update ÇŃ´Ů
	i_pNMonster->UpdateCurrentSpeed();

	///////////////////////////////////////////////////////////////////////////////
	// ¸ó˝şĹÍ°ˇ ˝şĹłŔ» »çżëÁßŔĚ´Ů
// 2009-09-09 ~ 2010 by dhjin, ŔÎÇÇ´ĎĆĽ - ÄÁĆ®·Ń˝şĹł·Î ŔĚ¸§ şŻ°ć
//	if(COMPARE_BODYCON_BIT(i_pNMonster->BodyCondition, BODYCON_MON_ATTACK6_MASK)
	if(COMPARE_BODYCON_BIT(i_pNMonster->BodyCondition, BODYCON_MON_CONTROLSKILL_MASK)
		&& i_pNMonster->MonsterInfoPtr->ItemInfo[ARRAY_INDEX_MONSTER_SKILL_ITEM].pItemInfo)
	{
		i_pNMonster->m_dwTimeLastMoved	= i_pNMonster->m_dwCurrentTick;
		
		if(i_pNMonster->m_dwCurrentTick - i_pNMonster->m_ArrLastReattackTime[ARRAY_INDEX_MONSTER_SKILL_ITEM] > i_pNMonster->MonsterInfoPtr->ItemInfo[ARRAY_INDEX_MONSTER_SKILL_ITEM].pItemInfo->Time)
		{
			///////////////////////////////////////////////////////////////////////////////
			// BodyConditio Change and Send
			BodyCond_t tmBodyCon = i_pNMonster->BodyCondition;
// 2009-09-09 ~ 2010 by dhjin, ŔÎÇÇ´ĎĆĽ - ÄÁĆ®·Ń˝şĹł·Î ŔĚ¸§ şŻ°ć
//			CLEAR_BODYCON_BIT(tmBodyCon, BODYCON_MON_PREATTACK6_MASK);
			CLEAR_BODYCON_BIT(tmBodyCon, BODYCON_MON_PRECONTROLSKILL_MASK);
			i_pNMonster->ChangeBodyCondition(&tmBodyCon);			
			INIT_MSG(MSG_FN_MONSTER_CHANGE_BODYCONDITION, T_FN_MONSTER_CHANGE_BODYCONDITION, pSeBody, i_pSendBuf);
			pSeBody->ChannelIndex	= this->m_MapChannelIndex.ChannelIndex;
			pSeBody->ClientIndex	= i_pNMonster->MonsterIndex;
			pSeBody->BodyCondition	= i_pNMonster->BodyCondition;
			Send2FieldServerW(i_pSendBuf, MSG_SIZE(MSG_FN_MONSTER_CHANGE_BODYCONDITION));

			///////////////////////////////////////////////////////////////////////////////
			// Move State Change
			i_pNMonster->SetMoveState(MSS_NORMAL);

			///////////////////////////////////////////////////////////////////////////////
			// Send EndSkill
			INIT_MSG(MSG_FN_MONSTER_SKILL_END_SKILL, T_FN_MONSTER_SKILL_END_SKILL, pSeSkill, i_pSendBuf);
			pSeSkill->ChannelIndex	= this->m_MapChannelIndex.ChannelIndex;
			pSeSkill->MonsterIndex	= i_pNMonster->MonsterIndex;
			pSeSkill->PositionVector= i_pNMonster->PositionVector;
			pSeSkill->TargetVector	= i_pNMonster->TargetVector*1000.0f;
			Send2FieldServerW(i_pSendBuf, MSG_SIZE(MSG_FN_MONSTER_SKILL_END_SKILL));
		}
		
		// ŔĚµżÇŃ ÁÂÇĄ¸¦ Field Server·Î ŔüĽŰÇŃ´Ů.
		SendMonsterMove2FieldServer(i_pNMonster, i_pSendBuf);
		return;
	}

	///////////////////////////////////////////////////////////////////////////////
	// Ĺ¸°ŮŔĚ ĽłÁ¤ µÇľî ŔÖ´Â ¸ó˝şĹÍ Ăł¸®
	if(0 != i_pNMonster->m_nTargetIndex
		&& NULL != i_pNMonster->m_pUsingMonsterItem)	// 2007-07-20 by cmkwon, Ĺ¸°ŮŔş ŔÖÁö¸¸ ą«±â ĽłÁ¤ŔĚ ľČµÇľî ŔÖ´Â ¸ó˝şĹÍ
	{
		_UpdateMonsterPositionHandlerAttack(i_pNMonster, i_pSendBuf, i_pvecClientIndex);
		return;
	}
	
	///////////////////////////////////////////////////////////////////////////////
	// Ĺ¸ÄĎŔĚ ĽłÁ¤ µÇľî ŔÖÁö ľĘŔş ¸ó˝şĹÍ ŔĚµż	
	float fMoveRate					= min(2.0f, (float)((i_pNMonster->m_dwCurrentTick - i_pNMonster->m_dwTimeLastMoved)/MONSTER_UPDATE_MOVE_TERM_TICK));
	i_pNMonster->m_dwTimeLastMoved	= i_pNMonster->m_dwCurrentTick;
	i_pNMonster->m_BeforePosition	= i_pNMonster->PositionVector;
	

	//////////////////////////////////////////////////////////////////
	// 2°ˇÁö ÇüĹÂŔÇ MonsterFormŔ» °ˇÁř ¸ó˝şĹÍ Ăł¸®, ÇöŔç´Â şń °ř°Ý˝Ăżˇ¸¸ Change°ˇ ŔĎľîł­´Ů.		
	this->_CheckMonsterChangeMonsterForm(i_pNMonster);

	///////////////////////////////////////////////////////////////////////////////
	// 2010-03-31 by dhjin, ŔÎÇÇ´ĎĆĽ(±âÁöąćľî) - ľĆ·ąłŞ Ľ­ąöżˇĽ­´Â ¸đµÎ ŔüĽŰÇŃ´Ů. ąŘ°ú °°ŔĚ ĽöÁ¤	
	//////////////////////////////////////////////////////////////////
	// ÁÖŔ§żˇ Äł¸ŻŔĚ ŔÖľîĽ­ ¸ó˝şĹÍŔÇ ŔĚµż Á¤ş¸¸¦ Field Server·Î ŔüĽŰÇŘľß ÇĎ´ÂÁö
//	BOOL bSendMoveFlag = NPCCharacterExistInRange(&i_pNMonster->PositionVector, i_pNMonster->SendMoveRange);
	BOOL bSendMoveFlag = TRUE;
	if(FALSE == g_pNPCGlobal->GetIsArenaServer()) 
	{
		bSendMoveFlag = NPCCharacterExistInRange(&i_pNMonster->PositionVector, i_pNMonster->SendMoveRange);
	}
		
	////////////////////////////////////////////////////////////////////////////////
	// 2009-09-09 ~ 2010 by dhjin, ŔÎÇÇ´ĎĆĽ - ąŘ°ú °°ŔĚ ĽöÁ¤
// 	//////////////////////////////////////////////////////////////////
// 	// °ř°Ý ĽşÇâ ¸ó˝şĹÍ°ˇ ÁÖŔ§żˇĽ­ Ĺ¸°ŮŔ» °Ë»öÇŃ´Ů.
// 	if(i_pNMonster->CheckEnableSearchTarget()		// 2007-06-26 by cmkwon, ¸ó˝şĹÍ Ĺ¸°Ů °Ë»ö ˝Ă°Ł ĽöÁ¤ - °Ë»ö °ˇ´É ż©şÎ Č®ŔÎ
// 		&& bSendMoveFlag
// 		&& (i_pNMonster->MonsterInfoPtr->Belligerence == BELL_ATATTACK
// 		|| i_pNMonster->MonsterInfoPtr->Belligerence == BELL_ATTACK_OUTPOST_PROTECTOR	// 2007-08-23 by cmkwon, BELL_ATATTACK°ú µżŔĎÇĎ°Ô µżŔŰ
// 		|| i_pNMonster->MonsterInfoPtr->Belligerence == BELL_TAGETATATTACK
// 		|| IS_INFLWAR_MONSTER(i_pNMonster->MonsterInfoPtr->Belligerence)		// 2006-11-20 by cmkwon
// 		|| (i_pNMonster->MonsterInfoPtr->Belligerence == BELL_RETREAT && i_pNMonster->CurrentHP < i_pNMonster->MonsterInfoPtr->MonsterHP*3/10))
// 		)
// 	{
// 		if(NPCGetAdjacentCharacterIndexes(&i_pNMonster->PositionVector, i_pNMonster->MonsterInfoPtr->AttackRange, i_pNMonster->MonsterInfoPtr->AttackRange*2, i_pvecClientIndex))
// 		{
// 			i_pNMonster->SelectTargetIndex(NPCGetTargetwithAttackObj(i_pNMonster->MonsterInfoPtr->AttackObject, i_pNMonster, *i_pvecClientIndex));
// 		}
// 	}

// 2010-03-16 by cmkwon, ŔÎÇÇ2Â÷ MtoM, MtoC Ĺ¸°Ů şŻ°ć °ü·Ă ĽöÁ¤ - ľĆ·ˇżÍ °°ŔĚ ĽöÁ¤
// 	//////////////////////////////////////////////////////////////////
// 	// °ř°Ý ĽşÇâ ¸ó˝şĹÍ°ˇ ÁÖŔ§żˇĽ­ Ĺ¸°ŮŔ» °Ë»öÇŃ´Ů.
// 	if(i_pNMonster->CheckEnableSearchTarget()) {	// 2007-06-26 by cmkwon, ¸ó˝şĹÍ Ĺ¸°Ů °Ë»ö ˝Ă°Ł ĽöÁ¤ - °Ë»ö °ˇ´É ż©şÎ Č®ŔÎ
// 		if(BELL_INFINITY_DEFENSE_MONSTER == i_pNMonster->MonsterInfoPtr->Belligerence) {
// 			// BELL_INFINITY_ATTACK_MONSTER ¸ó˝şĹÍ¸¸ °ř°Ý
// 			if(0 != NPCGetAdjacentMonsterIndexesByBell(&i_pNMonster->PositionVector, i_pNMonster->MonsterInfoPtr->AttackRange, i_pNMonster->MonsterInfoPtr->AttackRange*2, BELL_INFINITY_ATTACK_MONSTER, i_pvecClientIndex)) {
// 				i_pNMonster->SelectTargetIndex(NPCGetTargetMonsterwithAttackObj(i_pNMonster->MonsterInfoPtr->AttackObject, i_pNMonster, *i_pvecClientIndex));
// 			}
// 		}
// 		else if (BELL_INFINITY_ATTACK_MONSTER == i_pNMonster->MonsterInfoPtr->Belligerence) {
// 			// ľĆą«¸® ¸Ö¸® ŔÖľîµµ °ˇľßµČ´Ů!!, żěĽ± MonsterTargetŔÎ ¸ó˝şĹÍ¸¦ °ř°Ý, ľřŔ¸¸é BELL_INFINITY_DEFENSE_MONSTER ¸ó˝şĹÍ °ř°Ý, ľřŔ¸¸é ŔŻŔú °ř°Ý
// 			if(0 != this->GetMonsterIndexesByTargetMonsterNum(i_pNMonster->MonsterInfoPtr->MonsterTarget, i_pvecClientIndex)) {
// 				i_pNMonster->SelectTargetIndex(NPCGetTargetMonsterwithAttackObj(i_pNMonster->MonsterInfoPtr->AttackObject, i_pNMonster, *i_pvecClientIndex));
// 			}
// 			else if(0 != this->GetMonsterIndexesByBell(BELL_INFINITY_DEFENSE_MONSTER, i_pvecClientIndex)) {
// 				i_pNMonster->SelectTargetIndex(NPCGetTargetMonsterwithAttackObj(i_pNMonster->MonsterInfoPtr->AttackObject, i_pNMonster, *i_pvecClientIndex));
// 			}
// 			else if(TRUE == i_pNMonster->MonsterInfoPtr->ChangeTarget
// 					&& NPCGetAdjacentCharacterIndexes(&i_pNMonster->PositionVector, i_pNMonster->MonsterInfoPtr->AttackRange, i_pNMonster->MonsterInfoPtr->AttackRange*2, i_pvecClientIndex, i_pNMonster->MonsterInfoPtr->Belligerence)) {
// 				i_pNMonster->SelectTargetIndex(NPCGetTargetwithAttackObj(i_pNMonster->MonsterInfoPtr->AttackObject, i_pNMonster, *i_pvecClientIndex));
// 			}
// 		}
// 		else if (bSendMoveFlag
// 			&& (i_pNMonster->MonsterInfoPtr->Belligerence == BELL_ATATTACK
// 			|| i_pNMonster->MonsterInfoPtr->Belligerence == BELL_ATTACK_OUTPOST_PROTECTOR	// 2007-08-23 by cmkwon, BELL_ATATTACK°ú µżŔĎÇĎ°Ô µżŔŰ
// 			|| i_pNMonster->MonsterInfoPtr->Belligerence == BELL_TAGETATATTACK
// 			|| IS_INFLWAR_MONSTER(i_pNMonster->MonsterInfoPtr->Belligerence)		// 2006-11-20 by cmkwon
// 			|| (i_pNMonster->MonsterInfoPtr->Belligerence == BELL_RETREAT && i_pNMonster->CurrentHP < i_pNMonster->MonsterInfoPtr->MonsterHP*3/10))
// 			) {
// 			if(NPCGetAdjacentCharacterIndexes(&i_pNMonster->PositionVector, i_pNMonster->MonsterInfoPtr->AttackRange, i_pNMonster->MonsterInfoPtr->AttackRange*2, i_pvecClientIndex, i_pNMonster->MonsterInfoPtr->Belligerence)) {
// 				i_pNMonster->SelectTargetIndex(NPCGetTargetwithAttackObj(i_pNMonster->MonsterInfoPtr->AttackObject, i_pNMonster, *i_pvecClientIndex));
// 			}
// 		}
// 	}
	///////////////////////////////////////////////////////////////////////////////
	// 2010-03-16 by cmkwon, ŔÎÇÇ2Â÷ MtoM, MtoC Ĺ¸°Ů şŻ°ć °ü·Ă ĽöÁ¤ - 
	if(bSendMoveFlag
		&& i_pNMonster->CheckEnableSearchTarget())
	{
		if(i_pNMonster->MonsterInfoPtr->Belligerence == BELL_ATATTACK
			|| i_pNMonster->MonsterInfoPtr->Belligerence == BELL_ATTACK_OUTPOST_PROTECTOR
			|| i_pNMonster->MonsterInfoPtr->Belligerence == BELL_TAGETATATTACK
			|| IS_INFLWAR_MONSTER(i_pNMonster->MonsterInfoPtr->Belligerence)
 			|| (i_pNMonster->MonsterInfoPtr->Belligerence == BELL_RETREAT && i_pNMonster->CurrentHP < i_pNMonster->MonsterInfoPtr->MonsterHP*3/10)
			|| BELL_INFINITY_ATTACK_MONSTER == i_pNMonster->MonsterInfoPtr->Belligerence
			|| BELL_INFINITY_DEFENSE_MONSTER == i_pNMonster->MonsterInfoPtr->Belligerence)

		{
			ClientIndex_t tarIdx = this->SearchTarget(i_pNMonster);
			if(0 != tarIdx)
			{
				i_pNMonster->SelectTargetIndex(tarIdx);
			}
		}
	}

	///////////////////////////////////////////////////////////////////////////////
	// ÁÖŔ§ Äł¸ŻÁßżˇ Ĺ¸°ŮŔ» Ľ±ĹĂÇß´Ů
	// ŔĚµżŔş ÇĎÁö ľĘ°í ąŮ·Î ¸®ĹĎÇŃ´Ů.
	if(i_pNMonster->m_nTargetIndex != 0)
	{
		SendMonsterMove2FieldServer(i_pNMonster, i_pSendBuf);
		return;
	}

	///////////////////////////////////////////////////////////////////////////////
	// 1. ĆÄĆĽżřŔĚ¸é ĆÄĆĽŔĺŔ» ±âÁŘŔ¸·Î Ŕ§Äˇ°ˇ ĽłÁ¤µĘ, ÇöŔç´Â şń°ř°Ý˝Ă¸¸ ĆÄĆĽ°ˇ ÇüĽşµĘ
	// 2. ±×ŔĚżÜŔÇ ¸đµç ¸ó˝şĹÍ
	if(i_pNMonster->m_nPartyManagerIndex != 0
		&& i_pNMonster->m_nPartyManagerIndex != i_pNMonster->MonsterIndex)
	{

		NPCSetPartyPosition(i_pNMonster);
	}
	else
	{		
		///////////////////////////////////////////////////////////////////////////////
		// »ýĽş Ŕ§Äˇ·ÎşÎĹÍ ŔĚµż żµżŞŔÇ ĂĽĹ©, Ăćµą »óĹÂŔĚ¸é ±×łÉ ¸®ĹĎµČ´Ů
		if(TRUE == i_pNMonster->CheckMoveRange())
		{
			D3DXVECTOR3 tmVec3Tar(i_pNMonster->m_CreatedPosition - i_pNMonster->PositionVector);
			D3DXVec3Normalize(&tmVec3Tar, &tmVec3Tar);

			///////////////////////////////////////////////////////////////////////////////
			// 1. »ýĽş ÁÂÇĄ±îÁö ĂćµąŔĚ ąß»ýÇĎ¸é
			//		- MoveTargetVector¸¦ Ŕ§·Î żĂ¸°´Ů
			// 2. »ýĽş ÁÂÇĄ±îÁö ĂćµąŔĚ ąß»ýÇĎÁö ľĘŔ¸¸é
			//		- tmVec3TarŔ» ±×łÉ »çżëÇÔ
			if(TRUE == this->CheckImpactStraightLineMapAndObjects(&i_pNMonster->PositionVector, &i_pNMonster->m_CreatedPosition
				, DEFAULT_OBJECT_MONSTER_OBJECT+i_pNMonster->MonsterInfoPtr->MonsterUnitKind, FALSE))
			{
				GNPCRotateTargetVectorVertical(&tmVec3Tar, &tmVec3Tar, MSD_UP_10, 20);
			}
			i_pNMonster->SetEnforceTargetVector(&tmVec3Tar, i_pNMonster->GetSpeed(), MSS_RANGE_DISTANCE_IMPACT);
		}

		///////////////////////////////////////////////////////////////////////////////
		// 1. »ýĽş Ŕ§Äˇ·Î ŔĚµż żµżŞŔÇ ĂĽĹ© ČÄ MoveState°ˇ şŻČ­ ľřŔ¸¸é 
		//		MoveTargetVector ąćÇâŔ¸·Î ŔüÁř ÇßŔ»¶§ ¸Ę°úŔÇ ĂćµąŔĚ ąß»ýÇŇ°ÍŔÎ°ˇ¸¦ ĂĽĹ©
		D3DXVECTOR3 tmVec3(0, 0, 0);
		if(TRUE == CheckImpactFrontPositionMap(i_pNMonster, &tmVec3))
		{
			i_pNMonster->SetEnforceTargetVector(&tmVec3, i_pNMonster->GetSpeed(), MSS_MAP_IMPACT);
		}
		
		///////////////////////////////////////////////////////////////////////////////
		// Ăćµą »óĹÂżˇĽ­ Ĺ¸°Ů ş¤ĹÍŔÇ ĽöÁ¤ Ăł¸®, Ăćµą »óĹÂ°ˇ ľĆ´Ď¸é ±×łÉ ¸®ĹĎµČ´Ů
		if(MSS_NORMAL != i_pNMonster->m_enMoveState)
		{
			i_pNMonster->UpdateEnforceTargetVector();
		}		
		
		///////////////////////////////////////////////////////////////////////////////
		// ˝ÇÁ¦ ¸ó˝şĹÍ ŔĚµżŔĚ Ăł¸®µČ´Ů.
		i_pNMonster->UpdatePositionVector(fMoveRate);

		///////////////////////////////////////////////////////////////////////////////
		// ¸ó˝şĹÍŔÇ TargetVector¸¦ Ăł¸®ÇŃ´Ů
		//		1. ÁˇÇÁÇü
		D3DXVECTOR3 tmVec3Tar(i_pNMonster->m_MoveTargetVector);
		if(FORM_GROUND_MOVE == i_pNMonster->CurrentMonsterForm)
		{
			if(i_pNMonster->PositionVector == i_pNMonster->m_BeforePosition)
			{
				tmVec3Tar = i_pNMonster->m_MoveTargetVector;
			}
			else
			{
				tmVec3Tar = i_pNMonster->PositionVector - i_pNMonster->m_BeforePosition;
			}			
		}
		i_pNMonster->SetTargetVector(&tmVec3Tar);
	}

	///////////////////////////////////////////////////////////////////////////////
	// ÇöŔç ÁÂÇĄŔÇ łôŔĚ¸¦ ĂĽĹ©ÇŃ´Ů.
	//		- Áö»ó ¸ó˝şĹÍ łôŔĚ¸¦ ĽłÁ¤(ÁˇÇÁÇü, ÁöÇĎŔĚµż ¸ó˝şĹÍ łôŔĚ ĽłÁ¤)
	//		- łŞ¸ÓÁö ¸ó˝şĹÍ PositionVectorŔÇ ŔŻČżĽş ĂĽĹ©
	CheckMonsterPosition(i_pNMonster, fMoveRate);
	
	///////////////////////////////////////////////////////////////////////////////
	// ŔĚµż ÁÂÇĄ Block Ăł¸®
	UpdateBlockPosition(i_pNMonster->m_BeforePosition.x, i_pNMonster->m_BeforePosition.z,
		i_pNMonster->PositionVector.x, i_pNMonster->PositionVector.z, i_pNMonster->MonsterIndex);

	///////////////////////////////////////////////////////////////////////////////
	// ¸ó˝şĹÍ MoveInfo Á¤ş¸ Update
	i_pNMonster->UpdateMoveInfoAllCurrentCount();
	
	if(bSendMoveFlag)
	{
		switch(i_pNMonster->m_enMoveState)
		{
		case MSS_QUICK_TURN_GENERAL:
		case MSS_QUICK_TURN_SKILL:
			{
				if(FALSE == COMPARE_BODYCON_BIT(i_pNMonster->BodyCondition, BODYCON_BOOSTER3_MASK))
				{
					BodyCond_t tmBody = i_pNMonster->BodyCondition;
					SET_BODYCON_BIT(tmBody, BODYCON_BOOSTER3_MASK);
					i_pNMonster->ChangeBodyCondition(&tmBody);
					
					INIT_MSG(MSG_FN_MONSTER_CHANGE_BODYCONDITION, T_FN_MONSTER_CHANGE_BODYCONDITION, pSeBody, i_pSendBuf);
					pSeBody->ChannelIndex	= this->m_MapChannelIndex.ChannelIndex;
					pSeBody->ClientIndex	= i_pNMonster->MonsterIndex;
					pSeBody->BodyCondition	= i_pNMonster->BodyCondition;
					Send2FieldServerW(i_pSendBuf, MSG_SIZE(MSG_FN_MONSTER_CHANGE_BODYCONDITION));
				}
			}
			break;
		default:
			{
				if(FORM_GROUND_MOVE != i_pNMonster->CurrentMonsterForm
					&& FALSE == COMPARE_BODYCON_BIT(i_pNMonster->BodyCondition, BODYCON_BOOSTER1_MASK))
				{
					BodyCond_t tmBody = i_pNMonster->BodyCondition;
					SET_BODYCON_BIT(tmBody, BODYCON_BOOSTER1_MASK);
					i_pNMonster->ChangeBodyCondition(&tmBody);

					INIT_MSG(MSG_FN_MONSTER_CHANGE_BODYCONDITION, T_FN_MONSTER_CHANGE_BODYCONDITION, pSeBody, i_pSendBuf);
					pSeBody->ChannelIndex	= this->m_MapChannelIndex.ChannelIndex;
					pSeBody->ClientIndex	= i_pNMonster->MonsterIndex;
					pSeBody->BodyCondition	= i_pNMonster->BodyCondition;
					Send2FieldServerW(i_pSendBuf, MSG_SIZE(MSG_FN_MONSTER_CHANGE_BODYCONDITION));
				}
			}
		}
		
		SendMonsterMove2FieldServer(i_pNMonster, i_pSendBuf);
	}

}	// MoveMonsterHander_end()

void CNPCMapChannel::_UpdateMonsterPositionHandlerAttack(CNPCMonster *i_pNMonster
														, BYTE *i_pSendBuf
														, vector<ClientIndex_t> *i_pvecClientIndex)
{
	float fMoveRate					= min(2.0f, (float)((i_pNMonster->m_dwCurrentTick - i_pNMonster->m_dwTimeLastMoved)/MONSTER_UPDATE_MOVE_TERM_TICK));
	i_pNMonster->m_dwTimeLastMoved	= i_pNMonster->m_dwCurrentTick;
	i_pNMonster->m_BeforePosition	= i_pNMonster->PositionVector;

	////////////////////////////////////////////////////////////////////////////////
	// 2009-09-09 ~ 2010 by dhjin, ŔÎÇÇ´ĎĆĽ - Ĺ¸°Ů ÄÉ¸ŻĹÍżÍ ¸ó˝şĹÍ ÁÂÇĄ°ŞŔ» °řĹëŔ¸·Î »çżë - ąŘ°ú °°ŔĚ ĽöÁ¤
// 	///////////////////////////////////////////////////////////////////////////////
// 	// Ĺ¸°ŮŔĚ ¸ó˝şĹÍŔĎ¶§ Ăł¸®, łŞÁßżˇ ±¸Çö
// 	if(i_pNMonster->m_nTargetIndex >= MONSTER_CLIENT_INDEX_START_NUM)
// 	{
// 		// 2005-04-25 by cmkwon
// 		//		ASSERT_NEVER_GET_HERE();
// 		return;
// 	}
// 	
// 	///////////////////////////////////////////////////////////////////////////////
// 	// Ĺ¸°ŮŔĚ Äł¸ŻĹÍŔĎ¶§ Ăł¸®
// 	///////////////////////////////////////////////////////////////////////////////
// 	
// 	///////////////////////////////////////////////////////////////////////////////
// 	// Äł¸ŻĹÍ°ˇ ŔŻČżÇŃÁö ĂĽĹ© ÇŃ´Ů.
// 	CLIENT_INFO	*pClientInfo = GetClientInfo(i_pNMonster->m_nTargetIndex);
// 	if(NULL == pClientInfo 
// 		|| FALSE == pClientInfo->IsEnbleTargeted(i_pNMonster->MonsterInfoPtr->Belligerence, COMPARE_MPOPTION_BIT(i_pNMonster->MonsterInfoPtr->MPOption, MPOPTION_BIT_RECOGNIZE_INVISIBLE), TRUE))
// 	{
// 		i_pNMonster->DeleteAttackedInfowithIndex();
// 		return;
// 	}
// 	
// 	D3DXVECTOR3			Vec3TarM2C;					// (¸ó˝şĹÍ --> Äł¸ŻĹÍ)·ÎŔÇ ąćÇâş¤ĹÍ
// 	D3DXVECTOR3			Vec3TarM2CPlane;			// (¸ó˝şĹÍ --> Äł¸ŻĹÍ)·ÎŔÇ Ćň¸é ąćÇâ ş¤ĹÍ
// 	float				fDistanceM2CPlane;			// (¸ó˝şĹÍżÍ Äł¸ŻĹÍ)ŔÇ Ćň¸é »óŔÇ °Ĺ¸®	
// 	
// 	Vec3TarM2C			= pClientInfo->PositionVector - i_pNMonster->PositionVector;		// ¸ó˝şĹÍżˇĽ­ Ĺ¸°ŮŔ¸·ÎŔÇ ş¤ĹÍ¸¦ ±¸ÇŃ´Ů.

// 2010-01-06 by cmkwon, ¸ó˝şĹÍ °ř°Ý˝Ă Ĺ¸°Ů °ř°Ý °ˇ´É ĂĽĹ© Ăß°ˇ(Ĺ¸°ŮşŻ°ć) - ľĆ·ˇżÍ °°ŔĚ ĽöÁ¤
// 	D3DXVECTOR3	TargetUnitPositionVector;
// 	D3DXVECTOR3	TargetUnitTargetVector;
// 	ClientIndex_t curTargetIdx	= i_pNMonster->GetTargetIndex();
// 	if(IS_MONSTER_CLIENT_INDEX(curTargetIdx)) {
// 		// ¸ó˝şĹÍ ĂĽĹ©
// 		CNPCMonster * pTargetMonster = GetNPCMonster(curTargetIdx);
// 		if(NULL == pTargetMonster || FALSE == pTargetMonster->IsValidMonster()) {	
// 			i_pNMonster->DeleteAttackedInfowithIndex();
// 			return;
// 		}
// 		TargetUnitPositionVector = pTargetMonster->PositionVector;
// 		TargetUnitTargetVector	 = pTargetMonster->TargetVector;
// 	}
// 	else {
// 		///////////////////////////////////////////////////////////////////////////////
// 		// Äł¸ŻĹÍ°ˇ ŔŻČżÇŃÁö ĂĽĹ© ÇŃ´Ů.
// 		CLIENT_INFO	*pClientInfo = GetClientInfo(curTargetIdx, &m_MapChannelIndex);
// 		if(NULL == pClientInfo 
// 			|| FALSE == pClientInfo->IsEnbleTargeted(i_pNMonster->MonsterInfoPtr->Belligerence, COMPARE_MPOPTION_BIT(i_pNMonster->MonsterInfoPtr->MPOption, MPOPTION_BIT_RECOGNIZE_INVISIBLE), TRUE))
// 		{
// 			i_pNMonster->DeleteAttackedInfowithIndex();
// 			return;
// 		}
// 		TargetUnitPositionVector = pClientInfo->PositionVector;
// 		TargetUnitTargetVector	= pClientInfo->TargetVector;
// 	}
// 
// 	D3DXVECTOR3			Vec3TarM2C;					// (¸ó˝şĹÍ --> Äł¸ŻĹÍ)·ÎŔÇ ąćÇâş¤ĹÍ
// 	D3DXVECTOR3			Vec3TarM2CPlane;			// (¸ó˝şĹÍ --> Äł¸ŻĹÍ)·ÎŔÇ Ćň¸é ąćÇâ ş¤ĹÍ
// 	float				fDistanceM2CPlane;			// (¸ó˝şĹÍżÍ Äł¸ŻĹÍ)ŔÇ Ćň¸é »óŔÇ °Ĺ¸®	
// 
// 	Vec3TarM2C			= TargetUnitPositionVector - i_pNMonster->PositionVector;		// ¸ó˝şĹÍżˇĽ­ Ĺ¸°ŮŔ¸·ÎŔÇ ş¤ĹÍ¸¦ ±¸ÇŃ´Ů.
// 	Vec3TarM2CPlane		= Vec3TarM2C;
// 	Vec3TarM2CPlane.y	= 0.0f;
// 	fDistanceM2CPlane	= D3DXVec3Length(&Vec3TarM2CPlane);		// Ŕ§żˇĽ­ ±¸ÇŃ ş¤ĹÍŔÇ ±ćŔĚ¸¦ ±¸ÇŃ´Ů.(¸ó˝şĹÍżÍ Ĺ¸°Ů°úŔÇ °Ĺ¸®, Ćň¸é»óŔÇ °Ĺ¸®¸¦ ±¸ÇŃ´Ů)
// 
// // 2009-12-11 by cmkwon, µĄąĚÁö ľî±×·Î·Î Ĺ¸°ŮŔ» şŻ°ćÇĎ´Â ¸ó˝şĹÍ ±¸Çö - CNPCMapChannel::_UpdateMonsterPositionHandlerAttack#
// // 	if(fDistanceM2CPlane > i_pNMonster->MonsterInfoPtr->Range
// // 		&& MONSTER_TARGETTYPE_TUTORIAL != i_pNMonster->m_byMonsterTargetType)	// 2007-07-19 by cmkwon, Ć©Ĺä¸®ľó ¸ó˝şĹÍ ĽöÁ¤ - Ć©Ĺä¸®ľó¸ó˝şĹÍ´Â °Ĺ¸® ĂĽĹ©ÇĎÁö ľĘ´Â´Ů
// // 	{
// // 		if(i_pNMonster->m_dwCurrentTick - i_pNMonster->m_dwSetTargetIndexLastTick > MONSTER_MIN_SET_TARGET_INDEX_TIME
// // 			|| i_pNMonster->m_dwCurrentTick - i_pNMonster->m_dwTimeMonsterLastAttack > 2*MONSTER_MIN_SET_TARGET_INDEX_TIME)
// // 		{
// // 			i_pNMonster->DeleteAttackedInfowithIndex();
// // 			return;
// // 		}
// // 	}
// 	///////////////////////////////////////////////////////////////////////////////	
// 	// 2009-12-11 by cmkwon, µĄąĚÁö ľî±×·Î·Î Ĺ¸°ŮŔ» şŻ°ćÇĎ´Â ¸ó˝şĹÍ ±¸Çö - CNPCMapChannel::_UpdateMonsterPositionHandlerAttack#
// 	if(fDistanceM2CPlane > i_pNMonster->MonsterInfoPtr->Range
// 		&& MONSTER_TARGETTYPE_TUTORIAL != i_pNMonster->m_byMonsterTargetType)	// 2007-07-19 by cmkwon, Ć©Ĺä¸®ľó ¸ó˝şĹÍ ĽöÁ¤ - Ć©Ĺä¸®ľó¸ó˝şĹÍ´Â °Ĺ¸® ĂĽĹ©ÇĎÁö ľĘ´Â´Ů
// 	{
// 		int nElapsedTickAfterAttack = (int)((INT64)i_pNMonster->m_dwCurrentTick - (INT64)i_pNMonster->m_dwTimeMonsterLastAttack);
// 		if(TICK_MONSTER_DELETE_TARGET_TERM < nElapsedTickAfterAttack)
// 		{
// 			i_pNMonster->DeleteAttackedInfowithIndex();
// 			return;
// 		}
// 	}
	///////////////////////////////////////////////////////////////////////////////
	// 2010-01-06 by cmkwon, ¸ó˝şĹÍ °ř°Ý˝Ă Ĺ¸°Ů °ř°Ý °ˇ´É ĂĽĹ© Ăß°ˇ(Ĺ¸°ŮşŻ°ć) - 
	BOOL bIsTargetCharacter = FALSE;
	CLIENT_INFO *pTargetCli	= NULL;
	CNPCMonster *pTargetMon	= NULL;
	
	if(FALSE == GetTargetObject(&bIsTargetCharacter, &pTargetCli, &pTargetMon, i_pNMonster))
	{// 2010-01-06 by cmkwon, Ĺ¸°ŮŔĚ ŔŻČżÇĎÁö ľĘ´Ů
		
		i_pNMonster->DeleteAttackedInfowithIndex();
		return;
	}
	D3DXVECTOR3	TargetUnitPositionVector;
	D3DXVECTOR3	TargetUnitTargetVector;
	if(bIsTargetCharacter)
	{
		TargetUnitPositionVector	= pTargetCli->PositionVector;
		TargetUnitTargetVector		= pTargetCli->TargetVector;
	}
	else
	{
		// start 2011-04-05 by hskim, ŔÎÇÇ´ĎĆĽ 3Â÷ - ¸ó˝şĹÍ ¸ÖĆĽ Ĺ¸°ŮĆĂ ±â´É Ăß°ˇ
		//////////
		// ±âÁ¸ //
		// TargetUnitPositionVector	= pTargetMon->PositionVector;

		//////////
		// ĽöÁ¤ //
		if( pTargetMon->IsMultiTargetMonster() == TRUE )
		{
			int iMTCount = 0;
			TargetUnitPositionVector	= pTargetMon->GetNearMultiTarget(i_pNMonster->PositionVector, &iMTCount);
		}
		else
		{
			TargetUnitPositionVector	= pTargetMon->PositionVector;
		}
		// end 2011-04-05 by hskim, ŔÎÇÇ´ĎĆĽ 3Â÷ - ¸ó˝şĹÍ ¸ÖĆĽ Ĺ¸°ŮĆĂ ±â´É Ăß°ˇ

		TargetUnitTargetVector		= pTargetMon->TargetVector;
	}
	D3DXVECTOR3	Vec3TarM2C = i_pNMonster->PositionVector - TargetUnitPositionVector;		// (¸ó˝şĹÍ --> Äł¸ŻĹÍ)·ÎŔÇ ąćÇâş¤ĹÍ, ¸ó˝şĹÍżˇĽ­ Ĺ¸°ŮŔ¸·ÎŔÇ ş¤ĹÍ¸¦ ±¸ÇŃ´Ů.

	// 2010-02-19 by cmkwon, ¸ó˝şĹÍ °ř°Ý°ˇ´É °Ĺ¸® ŔĚČÄżˇµµ Ĺ¸ÄĎ ŔŻÁöµÇ´Â ąö±× ĽöÁ¤ - CheckValidTarget# ÇÔĽöżˇĽ­ FALSE¸¦ ¸®ĹĎ˝Ă Ăł¸®µĘ.
	if(FALSE == this->CheckValidTarget(i_pNMonster))
	{
		return;
	}
	// END - // 2010-01-06 by cmkwon, ¸ó˝şĹÍ °ř°Ý˝Ă Ĺ¸°Ů °ř°Ý °ˇ´É ĂĽĹ© Ăß°ˇ(Ĺ¸°ŮşŻ°ć) - 
	///////////////////////////////////////////////////////////////////////////////
	

	if(i_pNMonster->IsChangeableTarget())
	{
		if(this->ChangeTarget(i_pNMonster))
		{// 2010-03-16 by cmkwon, ŔÎÇÇ2Â÷ MtoM, MtoC Ĺ¸°Ů şŻ°ć °ü·Ă ĽöÁ¤ - Ĺ¸°ŮŔĚ şŻ°ćµÇľúŔ» ¶§¸¸ return Ăł¸®
			return;
		}
	}
	
	D3DXVECTOR3			tmVec3;
	float				fDistanceM2C;

	///////////////////////////////////////////////////////////////////////////////
	// 1. MoveTargetVector ąćÇâŔ¸·Î ŔüÁř ÇßŔ»¶§ ¸Ę°úŔÇ ĂćµąŔĚ ąß»ýÇŇ°ÍŔÎ°ˇ¸¦ ĂĽĹ©
	tmVec3 = D3DXVECTOR3(0, 0, 0);
	if(TRUE == CheckImpactFrontPositionMap(i_pNMonster, &tmVec3))
	{
		i_pNMonster->SetEnforceTargetVector(&tmVec3, i_pNMonster->GetSpeed(), MSS_MAP_IMPACT);		
	}
	
	///////////////////////////////////////////////////////////////////////////////
	// 1. °­Á¦ Ĺ¸°Ů ş¤ĹÍ ŔĚµż »óĹÂŔĎ¶§ Ĺ¸°Ů ş¤ĹÍŔÇ ĽöÁ¤ Ăł¸®żÍ ŔĚµż Ăł¸®
	//		- 
	// 2. °­Á¦ Ĺ¸°Ů ş¤ĹÍ ŔĚµż »óĹÂ ľĆ´Ň¶§ ŔĚµż Ăł¸®
	//		- FORM_FLYING_RIGHT´Â TargetVectorŔş Ĺ¸ÄĎŔ» ÇâÇĎ¸ç MoveTargetVectorŔş TargetVectorŔĚ ÇŇ´ç µČ´Ů
	//		- FORM_FLYING_COPTER´Â TargetVectorŔş Ĺ¸ÄĎŔ» ÇâÇĎ¸ç MoveTargetVector°ú´Â µű·Î µżŔŰµČ´Ů.	
	if(MSS_NORMAL != i_pNMonster->m_enMoveState)
	{
		i_pNMonster->m_MoveInfo.FBDirect		= MSD_FRONT;
		i_pNMonster->m_MoveInfo.FBCurrentCount	= 0;
		i_pNMonster->UpdateEnforceTargetVector();
		
		///////////////////////////////////////////////////////////////////////////////
		// ˝ÇÁ¦ ¸ó˝şĹÍ ŔĚµżŔĚ Ăł¸®µČ´Ů.
		i_pNMonster->UpdatePositionVector(fMoveRate);
		
		////////////////////////////////////////////////////////////////////////////////
		// 2009-09-09 ~ 2010 by dhjin, ŔÎÇÇ´ĎĆĽ - Ĺ¸°Ů ÄÉ¸ŻĹÍżÍ ¸ó˝şĹÍ ÁÂÇĄ°ŞŔ» °řĹëŔ¸·Î »çżë - ąŘ°ú °°ŔĚ ĽöÁ¤
//		Vec3TarM2C = pClientInfo->PositionVector - i_pNMonster->PositionVector;		// ¸ó˝şĹÍżˇĽ­ Ĺ¸°ŮŔ¸·ÎŔÇ ş¤ĹÍ¸¦ ±¸ÇŃ´Ů.
		Vec3TarM2C = TargetUnitPositionVector - i_pNMonster->PositionVector;	// ¸ó˝şĹÍżˇĽ­ Ĺ¸°ŮŔ¸·ÎŔÇ ş¤ĹÍ¸¦ ±¸ÇŃ´Ů.
		fDistanceM2C = D3DXVec3Length(&Vec3TarM2C);

		///////////////////////////////////////////////////////////////////////////////
		// ¸ó˝şĹÍŔÇ TargetVector¸¦ Ăł¸®ÇŃ´Ů
		D3DXVECTOR3 tmVec3Tar(i_pNMonster->m_MoveTargetVector);
		if(FORM_GROUND_MOVE == i_pNMonster->CurrentMonsterForm)
		{
			if(i_pNMonster->PositionVector == i_pNMonster->m_BeforePosition)
			{
				tmVec3Tar = i_pNMonster->m_MoveTargetVector;
			}
			else
			{
				tmVec3Tar = i_pNMonster->PositionVector - i_pNMonster->m_BeforePosition;
			}
		}
		i_pNMonster->SetTargetVector(&tmVec3Tar);

		switch(i_pNMonster->m_enMoveState)
		{
		case MSS_QUICK_TURN_GENERAL:
		case MSS_QUICK_TURN_SKILL:
			{
				if(fDistanceM2C >= i_pNMonster->MonsterInfoPtr->Range*0.8f)
				{
					i_pNMonster->m_MoveInfo.FBDirect = MSD_FRONT;
					i_pNMonster->m_MoveInfo.FBCurrentCount = 0;
					i_pNMonster->SetMoveState(MSS_NORMAL);
				}
			}
			break;
		}
	}
	else
	{
		///////////////////////////////////////////////////////////////////////////////
		// MoveTargetVectorżˇ µű¶ó ˝ÇÁ¦ ¸ó˝şĹÍ ŔĚµżŔĚ Ăł¸®µČ´Ů.
		i_pNMonster->UpdatePositionVector(fMoveRate);
		////////////////////////////////////////////////////////////////////////////////
		// 2009-09-09 ~ 2010 by dhjin, ŔÎÇÇ´ĎĆĽ - Ĺ¸°Ů ÄÉ¸ŻĹÍżÍ ¸ó˝şĹÍ ÁÂÇĄ°ŞŔ» °řĹëŔ¸·Î »çżë - ąŘ°ú °°ŔĚ ĽöÁ¤
//		Vec3TarM2C = pClientInfo->PositionVector - i_pNMonster->PositionVector;			// ¸ó˝şĹÍżˇĽ­ Ĺ¸°ŮŔ¸·ÎŔÇ ş¤ĹÍ¸¦ ±¸ÇŃ´Ů.
		Vec3TarM2C = TargetUnitPositionVector - i_pNMonster->PositionVector;		// ¸ó˝şĹÍżˇĽ­ Ĺ¸°ŮŔ¸·ÎŔÇ ş¤ĹÍ¸¦ ±¸ÇŃ´Ů.
		fDistanceM2C = D3DXVec3Length(&Vec3TarM2C);

		D3DXVECTOR3			UnitVec3TarM2C;				// (¸ó˝şĹÍ --> Äł¸ŻĹÍ)·ÎŔÇ ąćÇâ ´ÜŔ§ ş¤ĹÍ
		D3DXVec3Normalize(&UnitVec3TarM2C, &Vec3TarM2C);

		///////////////////////////////////////////////////////////////////////////////
		// ¸ó˝şĹÍŔÇ TargetVectorŔ» Ĺ¸ÄĎŔ» ÇâÇĎµµ·Ď ÇŃ´Ů.
		D3DXMATRIX tmMatrix;
		tmVec3 = i_pNMonster->TargetVector;		
		
		if(ORBIT_BODYSLAM == i_pNMonster->m_pUsingMonsterItem->pItemInfo->OrbitType)
		{
			if(i_pNMonster->m_dwCurrentTick - i_pNMonster->m_dwTimeMonsterLastAttack > 3*i_pNMonster->m_pUsingMonsterItem->pItemInfo->ReAttacktime)
			{
				tmMatrix = GNPCGetMaxTargetVector(&tmVec3, &UnitVec3TarM2C, PI/2);
			}
			else
			{
				tmMatrix = GNPCGetMaxTargetVector(&tmVec3, &UnitVec3TarM2C, 2*i_pNMonster->MonsterInfoPtr->TurnAngle);
			}			
		}
		else if(i_pNMonster->m_dwCurrentTick - i_pNMonster->m_dwTimeMonsterLastAttack > 4*i_pNMonster->m_pUsingMonsterItem->pItemInfo->ReAttacktime)
		{
			tmMatrix = GNPCGetMaxTargetVector(&tmVec3, &UnitVec3TarM2C, PI/2);
		}
		else if(i_pNMonster->m_MoveInfo.MoveCount < 5
			|| fDistanceM2C > i_pNMonster->MonsterInfoPtr->Range*0.7)
		{
			tmMatrix = GNPCGetMaxTargetVector(&tmVec3, &UnitVec3TarM2C, 2*i_pNMonster->MonsterInfoPtr->TurnAngle);
		}
		else
		{
			tmMatrix = GNPCGetMaxTargetVector(&tmVec3, &UnitVec3TarM2C, i_pNMonster->MonsterInfoPtr->TurnAngle);
		}
		i_pNMonster->SetTargetVector(&tmVec3);

		///////////////////////////////////////////////////////////////////////////////
		// ¸ó˝şĹÍ MoveTargetVector Ăł¸®
		//	1. Áö»óÇü ¸ó˝şĹÍ
		//		- ÁˇÇÁÇü, ¶ĄĽÓŔĚµżÇüŔĚ ľĆ´Ď°í °ř°ÝŔĚ ĂłŔ˝˝Ă´Â MoveTargetVectorŔ» Ĺ¸°ŮŔ» ÇâÇĎ´Â Ćň¸é ş¤ĹÍ·Î ĽłÁ¤ÇŃ´Ů.
		//	2. şńÇŕÇüŔĚ¸éĽ­ ÄßĹÍÇü
		//		- °ř°ÝŔĚ ĂłŔ˝˝Ă´Â MoveTargetVectorŔ» Ĺ¸°ŮŔ» ÇâÇĎµµ·Ď ÇŃ´Ů
		//		- ĂłŔ˝ŔĚ ľĆ´Ď¸é TargetVectorŔĚ Č¸ŔüÇĎ´Â °˘ ¸¸Ĺ­ MoveTargetVectorµµ Č¸Ŕü˝ĂĹ˛´Ů.
		//	3. şńÇŕÇüŔĚ¸éĽ­ Á÷ÁřÇü
		//		- TargetVector°ú °°°Ô ÇŘÁÖ¸é µČ´Ů
		if(FALSE == i_pNMonster->m_MoveInfo.MovableFlag)
		{
			i_pNMonster->SetMoveTargetVector(&i_pNMonster->TargetVector);
		}
		else if(FORM_GROUND_MOVE == i_pNMonster->CurrentMonsterForm)
		{
			if(FALSE == i_pNMonster->m_FlagNPCMonster.MoveTargetVectorSetAttack)
			{
				i_pNMonster->SetNPCMonsterFlagMoveTargetVectorSetAttack(TRUE);
				////////////////////////////////////////////////////////////////////////////////
				// 2009-09-09 ~ 2010 by dhjin, ŔÎÇÇ´ĎĆĽ - Ĺ¸°Ů ÄÉ¸ŻĹÍżÍ ¸ó˝şĹÍ ÁÂÇĄ°ŞŔ» °řĹëŔ¸·Î »çżë - ąŘ°ú °°ŔĚ ĽöÁ¤
//				m_pNPCMapProject->ChangePlaneUnitVec3(&tmVec3, &i_pNMonster->TargetVector, &pClientInfo->TargetVector);
				m_pNPCMapProject->ChangePlaneUnitVec3(&tmVec3, &i_pNMonster->TargetVector, &TargetUnitTargetVector);
				i_pNMonster->SetMoveTargetVector(&tmVec3);
			}
		}
		else if(TRUE == i_pNMonster->m_MoveInfo.FBFlag)
		{
			if(FALSE == i_pNMonster->m_FlagNPCMonster.MoveTargetVectorSetAttack)
			{
				i_pNMonster->SetNPCMonsterFlagMoveTargetVectorSetAttack(TRUE);
				i_pNMonster->SetMoveTargetVector(&i_pNMonster->TargetVector);
			}
			else
			{
				CNPCMapProject::ChangePlaneUnitVec3(&tmVec3, &i_pNMonster->TargetVector, &i_pNMonster->TargetVector);
				float fPinPoint = ACOS(D3DXVec3Dot(&i_pNMonster->TargetVector, &tmVec3));
				if(fPinPoint <= MONSTER_COPTER_MAXTARGET_PINPOINT
// 2005-06-28 by cmkwon
//					|| i_pNMonster->m_dwCurrentTick - i_pNMonster->m_dwTimeMonsterLastAttack > i_pNMonster->m_pUsingMonsterItem->pItemInfo->ReAttacktime*3
					)
				{
					D3DXVec3TransformCoord(&tmVec3, &i_pNMonster->m_MoveTargetVector, &tmMatrix);
					i_pNMonster->SetMoveTargetVector(&tmVec3);
				}
				else
				{
					CNPCMapProject::ChangePlaneUnitVec3(&tmVec3, &UnitVec3TarM2C, &UnitVec3TarM2C);
					GNPCGetMaxTargetVector(&tmVec3, &UnitVec3TarM2C, MONSTER_COPTER_MAXMOVE_PINPOINT);
					i_pNMonster->SetEnforceTargetVector(&tmVec3, i_pNMonster->GetCurrentSpeed(), MSS_QUICK_TURN_GENERAL);
				}
			}
		}
		else
		{
			i_pNMonster->SetMoveTargetVector(&i_pNMonster->TargetVector);
		}

		///////////////////////////////////////////////////////////////////////////////
		// °ř°Ý˝Ă MoveInfo Ăł¸®
		////////////////////////////////////////////////////////////////////////////////
		// 2009-09-09 ~ 2010 by dhjin, ŔÎÇÇ´ĎĆĽ - Ĺ¸°Ů ÄÉ¸ŻĹÍżÍ ¸ó˝şĹÍ ÁÂÇĄ°ŞŔ» °řĹëŔ¸·Î »çżë - ąŘ°ú °°ŔĚ ĽöÁ¤
//		i_pNMonster->UpdateMoveInfoAttack(&pClientInfo->PositionVector, &pClientInfo->TargetVector);
		i_pNMonster->UpdateMoveInfoAttack(&TargetUnitPositionVector, &TargetUnitTargetVector);

		//////////////////////////////////////////////////////////////////////////////
		// 1. ˝şĹłŔĚ ąßµż µÇľú´Ů
		// 2. QuickTurnŔĚ ąßµż µÇľú´Ů
		if(MSS_QUICK_TURN_SKILL == i_pNMonster->m_enMoveState)
		{
			i_pNMonster->m_ArrLastReattackTime[ARRAY_INDEX_MONSTER_SKILL_ITEM] = i_pNMonster->m_dwCurrentTick;
			
			BodyCond_t tmBodyCon = i_pNMonster->BodyCondition;
// 2009-09-09 ~ 2010 by dhjin, ŔÎÇÇ´ĎĆĽ - ÄÁĆ®·Ń˝şĹł·Î ŔĚ¸§ şŻ°ć
//			SET_BODYCON_BIT(tmBodyCon, BODYCON_MON_PREATTACK6_MASK);
			SET_BODYCON_BIT(tmBodyCon, BODYCON_MON_PRECONTROLSKILL_MASK);
			i_pNMonster->ChangeBodyCondition(&tmBodyCon);
			
			INIT_MSG(MSG_FN_MONSTER_CHANGE_BODYCONDITION, T_FN_MONSTER_CHANGE_BODYCONDITION, pSeBody, i_pSendBuf);
			pSeBody->ChannelIndex	= this->m_MapChannelIndex.ChannelIndex;
			pSeBody->ClientIndex	= i_pNMonster->MonsterIndex;
			pSeBody->BodyCondition	= i_pNMonster->BodyCondition;
			Send2FieldServerW(i_pSendBuf, MSG_SIZE(MSG_FN_MONSTER_CHANGE_BODYCONDITION));
			
			INIT_MSG(MSG_FN_MONSTER_SKILL_USE_SKILL, T_FN_MONSTER_SKILL_USE_SKILL, pSeSkill, i_pSendBuf);
			pSeSkill->ChannelIndex	= this->m_MapChannelIndex.ChannelIndex;
			pSeSkill->MonsterIndex	= i_pNMonster->MonsterIndex;
			pSeSkill->ClientIndex	= i_pNMonster->m_nTargetIndex;
			pSeSkill->SkillItemNum	= i_pNMonster->MonsterInfoPtr->ItemInfo[ARRAY_INDEX_MONSTER_SKILL_ITEM].pItemInfo->ItemNum;
			Send2FieldServerW(i_pSendBuf, MSG_SIZE(MSG_FN_MONSTER_SKILL_USE_SKILL));
			
//			DBGOUT("	Use Skill Pos(%d, %d, %d)\n"
//				, (int)i_pNMonster->PositionVector.x, (int)i_pNMonster->PositionVector.y
//				, (int)i_pNMonster->PositionVector.z);
		}
		else if(MSS_QUICK_TURN_GENERAL == i_pNMonster->m_enMoveState)
		{
			if(FALSE == COMPARE_BODYCON_BIT(i_pNMonster->BodyCondition, BODYCON_BOOSTER3_MASK))
			{
				BodyCond_t tmBodyCon = i_pNMonster->BodyCondition;
				SET_BODYCON_BIT(tmBodyCon, BODYCON_BOOSTER3_MASK);
				i_pNMonster->ChangeBodyCondition(&tmBodyCon);
				
				INIT_MSG(MSG_FN_MONSTER_CHANGE_BODYCONDITION, T_FN_MONSTER_CHANGE_BODYCONDITION, pSeBody, i_pSendBuf);
				pSeBody->ChannelIndex	= this->m_MapChannelIndex.ChannelIndex;
				pSeBody->ClientIndex	= i_pNMonster->MonsterIndex;
				pSeBody->BodyCondition	= i_pNMonster->BodyCondition;
				Send2FieldServerW(i_pSendBuf, MSG_SIZE(MSG_FN_MONSTER_CHANGE_BODYCONDITION));
			}
		}			
	}
	
	///////////////////////////////////////////////////////////////////////////////
	// ÇöŔç ÁÂÇĄŔÇ łôŔĚ¸¦ ĂĽĹ©ÇŃ´Ů.
	//		- Áö»ó ¸ó˝şĹÍ łôŔĚ¸¦ ĽłÁ¤(ÁˇÇÁÇü, ÁöÇĎŔĚµż ¸ó˝şĹÍ łôŔĚ ĽłÁ¤)
	//		- łŞ¸ÓÁö ¸ó˝şĹÍ PositionVectorŔÇ ŔŻČżĽş ĂĽĹ©
	CheckMonsterPosition(i_pNMonster, fMoveRate);
	
	///////////////////////////////////////////////////////////////////////////////
	// ŔĚµż ÁÂÇĄ Block Ăł¸®
	UpdateBlockPosition(i_pNMonster->m_BeforePosition.x, i_pNMonster->m_BeforePosition.z,
		i_pNMonster->PositionVector.x, i_pNMonster->PositionVector.z, i_pNMonster->MonsterIndex);

	///////////////////////////////////////////////////////////////////////////////
	// ¸ó˝şĹÍ MoveInfo Á¤ş¸ Update
	i_pNMonster->UpdateMoveInfoAllCurrentCount();
	
	SendMonsterMove2FieldServer(i_pNMonster, i_pSendBuf);
}


void CNPCMapChannel::AttackMonster2Unit(CNPCMonster *i_pnMonster, BYTE *pSendBuf)
{
	if(i_pnMonster->CheckDeadBodyCondition())
	{
		return;
	}

	if (i_pnMonster->m_nTargetIndex < MONSTER_CLIENT_INDEX_START_NUM)
	{
		AttackMonster2Character(i_pnMonster, pSendBuf);
	}
	else
	{
		AttackMonster2Monster(i_pnMonster, pSendBuf);
	}
}

void CNPCMapChannel::AttackMonster2Character(CNPCMonster *i_pnMonster, BYTE *pSendBuf)
{
	CLIENT_INFO			* ptmClientInfo = GetClientInfo(i_pnMonster->m_nTargetIndex);
	if(NULL == ptmClientInfo || ptmClientInfo->ClientState != CS_GAMESTARTED)
	{	// Ĺ¸°ŮŔĚ  ŔŻČżÇĎÁö ľĘŔ˝
		// ŔŻČżÇĎÁö ľĘŔ˝ Ĺ¸°ŮŔ» »čÁ¦ÇĎ°í »ő·Îżî Ĺ¸°ŮŔ¸·Î ĽłÁ¤ÇŃ´Ů.
		// LastAttackTimeŔş ĂĘ±âČ­ ÇĎÁö ľĘ´Â´Ů, 

		i_pnMonster->DeleteAttackedInfowithIndex();
		return;
	}

	if(NULL == i_pnMonster->m_pUsingMonsterItem
// 2009-09-09 ~ 2010 by dhjin, ŔÎÇÇ´ĎĆĽ - ÄÁĆ®·Ń˝şĹł·Î ŔĚ¸§ şŻ°ć
//		|| COMPARE_BODYCON_BIT(i_pnMonster->BodyCondition, BODYCON_MON_ATTACK6_MASK))
		|| COMPARE_BODYCON_BIT(i_pnMonster->BodyCondition, BODYCON_MON_CONTROLSKILL_MASK))
	{
		return;
	}

	if(FALSE == COMPARE_BODYCON_BIT(i_pnMonster->BodyCondition, BODYCON_MON_ATTACKALL_MASK)
		&& FALSE == CheckMonsterSelectedItem(i_pnMonster))
	{
		i_pnMonster->SelectUsingMonsterItem();
		return;
	}

	this->AttackMonster2Character(i_pnMonster, ptmClientInfo, pSendBuf);
}


void CNPCMapChannel::SendAttack2FieldServer(CNPCMonster *i_pnMonster
											 , CLIENT_INFO * i_pClientInfo
											 , BYTE *pSendBuf)
{
	////////////////////////////////////////////////////////////////////////////////
	// 2009-09-09 ~ 2010 by dhjin, ŔÎÇÇ´ĎĆĽ - 
	if(NULL == i_pnMonster) {
		return;
	}
	
	// 2009-09-09 ~ 2010-01-13 by dhjin, ŔÎÇÇ´ĎĆĽ - ÇŃ ľĆŔĚĹŰŔ¸·Î ż©·Ż ´ë»ç °ˇ´ÉÇĎ°Ô ĽöÁ¤, ąŘ°ú °°ŔĚ ĽöÁ¤ HPActionItem »çżë˝Ă »çżë Ä«żîĆ®¸¦ ÁŮŔÎ´Ů.
	i_pnMonster->m_HPAction.SetSuccessAttackItemIdxHPRate();
	HPACTION_TALK_HPRATE MsgTalk;
	MEMSET_ZERO(&MsgTalk, sizeof(HPACTION_TALK_HPRATE));
	if(i_pnMonster->m_HPAction.GetHPTalkAttack(i_pnMonster->m_pUsingMonsterItem->pItemInfo->ItemNum, MsgTalk.HPTalk, &MsgTalk.HPCameraTremble)) {
		// Attack°Ş ŔüĽŰ.
		MsgTalk.HPTalkImportance	= HPACTION_TALK_IMPORTANCE_CHANNEL;
		this->SendFSvrHPTalk(i_pnMonster, &MsgTalk);
	}

	if(ITEMKIND_FOR_MON_SKILL == i_pnMonster->m_pUsingMonsterItem->pItemInfo->Kind) {
// 2010-03-31 by dhjin, ŔÎÇÇ´ĎĆĽ(±âÁöąćľî) - ąŘ°ú °°ŔĚ Ăł¸® CNPCMonster::UseSkill()żˇĽ­ Ăł¸®ÇŃ´Ů.
// 		INIT_MSG_WITH_BUFFER(MSG_FN_BATTLE_ATTACK_SKILL, T_FN_BATTLE_ATTACK_SKILL, pSendMsg, pSendBuf);
// 		pSendMsg->MapInfo			= m_MapChannelIndex;
// 		pSendMsg->MonsterIndex		= i_pnMonster->MonsterIndex;
// 		pSendMsg->ClientIndex		= i_pClientInfo->ClientIndex;	
// 		pSendMsg->SkillItemNum		= i_pnMonster->m_pUsingMonsterItem->pItemInfo->ItemNum;
// 		if(DES_WARP == i_pnMonster->m_pUsingMonsterItem->pItemInfo->ArrDestParameter[0]) {
// 			// Ľř°ŁŔĚµż ˝şĹłŔĚ¸é ş¤ĹÍ¸¦ Ĺ¸°Ů ŔŻŔú·Î şŻ°ć˝ĂĹ˛´Ů.
// 			i_pnMonster->SetUserPositionVector(&i_pClientInfo->PositionVector, &i_pClientInfo->TargetVector);
// 
// 			CheckMonsterPositionWarp(i_pnMonster, min(2.0f, (float)((i_pnMonster->m_dwCurrentTick - i_pnMonster->m_dwTimeLastMoved)/MONSTER_UPDATE_MOVE_TERM_TICK)) );
// 
// 			///////////////////////////////////////////////////////////////////////////////
// 			// ŔĚµż ÁÂÇĄ Block Ăł¸®
// 			UpdateBlockPosition(i_pnMonster->m_BeforePosition.x, i_pnMonster->m_BeforePosition.z,
// 				i_pnMonster->PositionVector.x, i_pnMonster->PositionVector.z, i_pnMonster->MonsterIndex);
// 
// 			pSendMsg->PositionVector	= i_pClientInfo->PositionVector;
// 			pSendMsg->TargetVector		= i_pClientInfo->TargetVector*1000.0f;
// 
// 		}
// 		else if (DES_SKILL_INVINCIBLE == i_pnMonster->m_pUsingMonsterItem->pItemInfo->ArrDestParameter[0]) {
// 			// ąč¸®ľî Č®Ŕ˛ ŔüĽŰÇĎ°í ąč¸®ľî´Â ÇŃ ąř ąßµżÇĎ¸é Á×Ŕ»¶§±îÁö ąßµżŔĚ±â ¶§ą®żˇ HPActionżˇĽ­ Áöżöąö¸°´Ů.
// 			i_pnMonster->m_HPAction.EraseHPActionByUseItemArrayIdx(i_pnMonster->m_BarrierUseItemArrayIdx);
// 		}
// 		else {
// 			pSendMsg->PositionVector	= i_pnMonster->PositionVector;	// ¸ó˝şĹÍżˇ ÇöŔç Ŕ§Äˇ Á¤ş¸´Â ŔüĽŰÇĎŔÚ!
// 		}
// 		Send2FieldServerW(pSendBuf, MSG_SIZE(MSG_FN_BATTLE_ATTACK_SKILL));
// 		char szTemp[256];
// 		sprintf(szTemp, "[Check] T_FN_BATTLE_ATTACK_SKILL Test, SkillNum = (%d) \r\n", pSendMsg->SkillItemNum);
// 		g_pNPCGlobal->WriteSystemLog(szTemp);
// 		////////////////////////////////////////////////////////////////////////////////
// 		// 2009-09-09 ~ 2010 by dhjin, ŔÎÇÇ´ĎĆĽ - ˝şĹł ¸ÖĆĽ Ĺ¸°Ů Ăł¸®, ¸ÂŔş ŔŻŔú Á¦żÜÇĎ°í Ăł¸®. żěĽ± ľî±×·Î´Â »ý°˘ÇĎÁö ľĘ´Â´Ů.
// 		if(1 < i_pnMonster->m_pUsingMonsterItem->pItemInfo->MultiTarget
// 			&& DES_WARP != i_pnMonster->m_pUsingMonsterItem->pItemInfo->ArrDestParameter[0]
// 			&& DES_SKILL_INVINCIBLE != i_pnMonster->m_pUsingMonsterItem->pItemInfo->ArrDestParameter[0]) {
// 			int		MultiTargetCount  = 0;
// 			vectorClientIndex tmClientIdxList;
// 			tmClientIdxList.reserve(10);
// 			if(1 >= NPCGetAdjacentCharacterIndexes(&i_pnMonster->PositionVector, i_pnMonster->m_pUsingMonsterItem->pItemInfo->Range
// 				, i_pnMonster->m_pUsingMonsterItem->pItemInfo->Range*2, &tmClientIdxList, i_pnMonster->MonsterInfoPtr->Belligerence)) {
// 				// ÇŃ ¸íŔ» Ăł¸® Çß±â ¶§ą®żˇ µÎ ¸í ŔĚ»óŔĚ¸é Ăł¸®
// 				return;
// 			}
// 			int CheckSize = i_pnMonster->m_pUsingMonsterItem->pItemInfo->MultiTarget - 1;
// 			if(tmClientIdxList.size() < i_pnMonster->m_pUsingMonsterItem->pItemInfo->MultiTarget) {
// 				// ´ŮĽö ŔűżëŔĚ ľî±×·Î ¸®˝şĆ® ş¸´Ů ¸ą´Ů¸é ľî±×·Î ¸®˝şĆ® Ľö·Î ĂÖ´ë·® ÇŇ´ç.
// 				CheckSize = tmClientIdxList.size();
// 			}
// 			vectorClientIndex::iterator itr(tmClientIdxList.begin());
// 			for(; itr != tmClientIdxList.end(); itr++) {
// 				CLIENT_INFO		* ptmClientInfo = GetClientInfo(*itr);
// 				if(ptmClientInfo->ClientIndex == i_pClientInfo->ClientIndex) {
// 					// ŔĚąĚ Ăł¸®µČ ŔŻŔú´Ů ´ŮŔ˝ ŔŻŔú °ˇÁ®żŔ±â
// 					continue;
// 				}
// 				if(NULL != ptmClientInfo && ptmClientInfo->ClientState == CS_GAMESTARTED) {
// 					INIT_MSG_WITH_BUFFER(MSG_FN_BATTLE_ATTACK_SKILL, T_FN_BATTLE_ATTACK_SKILL, pSendMsg, pSendBuf);
// 					pSendMsg->MapInfo			= m_MapChannelIndex;
// 					pSendMsg->MonsterIndex		= i_pnMonster->MonsterIndex;
// 					pSendMsg->ClientIndex		= ptmClientInfo->ClientIndex;	
// 					pSendMsg->SkillItemNum		= i_pnMonster->m_pUsingMonsterItem->pItemInfo->ItemNum;
// 					pSendMsg->PositionVector	= i_pnMonster->PositionVector;	// ¸ó˝şĹÍżˇ ÇöŔç Ŕ§Äˇ Á¤ş¸´Â ŔüĽŰÇĎŔÚ!
// 					Send2FieldServerW(pSendBuf, MSG_SIZE(MSG_FN_BATTLE_ATTACK_SKILL));
// 					MultiTargetCount++;
// 				}
// 				if(MultiTargetCount >= CheckSize) {
// 					break;
// 				}
// 			}			
// 		}		
//		////////////////////////////////////////////////////////////////////////////////
//		// 2009-09-09 ~ 2010-01-18 by dhjin, ŔÎÇÇ´ĎĆĽ - ÁÖĽ® Ăł¸®, Ŕ§żˇĽ­ ŔĚąĚ ş¸ł»°í ŔÖľú´Ů. 2ąř ŔüĽŰÇĎ´ř ąö±× ĽöÁ¤.
//		// 2009-09-09 ~ 2010-01-14 by dhjin, ŔÎÇÇ´ĎĆĽ - ˝şĹł ¸ÖĆĽ Ĺ¸°Ů Ăł¸®ÇĎ¸éĽ­ ŔÚ±â ŔÚ˝Ĺ ˝şĹł Ăł¸® ľČµČ şÎşĐ Ăß°ˇ
// 		else if(0 == i_pnMonster->m_pUsingMonsterItem->pItemInfo->MultiTarget) {
// 			INIT_MSG_WITH_BUFFER(MSG_FN_BATTLE_ATTACK_SKILL, T_FN_BATTLE_ATTACK_SKILL, pSendMsg, pSendBuf);
// 			pSendMsg->MapInfo			= m_MapChannelIndex;
// 			pSendMsg->MonsterIndex		= i_pnMonster->MonsterIndex;
// 			pSendMsg->ClientIndex		= 0;	
// 			pSendMsg->SkillItemNum		= i_pnMonster->m_pUsingMonsterItem->pItemInfo->ItemNum;
// 			pSendMsg->PositionVector	= i_pnMonster->PositionVector;	// ¸ó˝şĹÍżˇ ÇöŔç Ŕ§Äˇ Á¤ş¸´Â ŔüĽŰÇĎŔÚ!
// 			Send2FieldServerW(pSendBuf, MSG_SIZE(MSG_FN_BATTLE_ATTACK_SKILL));
// 		}
		i_pnMonster->UseSkill(i_pClientInfo->ClientIndex);		// 2010-03-31 by dhjin, ŔÎÇÇ´ĎĆĽ(±âÁöąćľî) -
		return;
	}

	if(IS_PRIMARY_WEAPON_MONSTER(i_pnMonster->m_pUsingMonsterItem->pItemInfo->Kind))
	{
		///////////////////////////////////////////////////////////////////////
		// MultiNum Ăł¸®
		for(int i = 0; i < i_pnMonster->m_pUsingMonsterItem->pItemInfo->MultiNum; i++)
		{
			INIT_MSG(MSG_FN_BATTLE_ATTACK_PRIMARY, T_FN_BATTLE_ATTACK_PRIMARY, pSendBattleAttackPri, pSendBuf);
			pSendBattleAttackPri->ChannelIndex		= m_MapChannelIndex.ChannelIndex;
			pSendBattleAttackPri->AttackIndex		= i_pnMonster->MonsterIndex;
			pSendBattleAttackPri->TargetIndex		= i_pClientInfo->ClientIndex;			
			pSendBattleAttackPri->WeaponItemNumber	= i_pnMonster->m_pUsingMonsterItem->pItemInfo->ItemNum;
			pSendBattleAttackPri->WeaponIndex		= 0;
			pSendBattleAttackPri->TargetPosition	= i_pClientInfo->PositionVector;
			Send2FieldServerW(pSendBuf, MSG_SIZE(MSG_FN_BATTLE_ATTACK_PRIMARY));
		}
	}
	else
	{
		////////////////////////////////////////////////////////////////////////////////
		// 2009-09-09 ~ 2010 by dhjin, ŔÎÇÇ´ĎĆĽ - ąŘżˇ ÇÔĽö·Î şŻ°ć
		///////////////////////////////////////////////////////////////////////
		// MultiNum Ăł¸®
// 		BYTE byExplosionPos = 128 - i_pnMonster->m_pUsingMonsterItem->pItemInfo->MultiNum/2;		
// 		for(int i = 0; i < i_pnMonster->m_pUsingMonsterItem->pItemInfo->MultiNum; i++)
// 		{
// 			INIT_MSG_WITH_BUFFER(MSG_FN_BATTLE_ATTACK_SECONDARY, T_FN_BATTLE_ATTACK_SECONDARY, pSendBattleAttackSec, pSendBuf);
// 			pSendBattleAttackSec->ChannelIndex		= m_MapChannelIndex.ChannelIndex;
// 			pSendBattleAttackSec->AttackIndex		= i_pnMonster->MonsterIndex;
// 			pSendBattleAttackSec->TargetIndex		= i_pClientInfo->ClientIndex;
// 			pSendBattleAttackSec->WeaponIndex		= m_uiMissileUniqueIndex++;
// 			pSendBattleAttackSec->WeaponItemNumber	= i_pnMonster->m_pUsingMonsterItem->pItemInfo->ItemNum;
// 			pSendBattleAttackSec->TargetPosition	= i_pClientInfo->PositionVector;
// 			pSendBattleAttackSec->Distance			= byExplosionPos;
// 			pSendBattleAttackSec->SecAttackType		= 0;
// 			pSendBattleAttackSec->AttackPosition	= i_pnMonster->PositionVector;		// 2009-09-09 ~ 2010 by dhjin, ŔÎÇÇ´ĎĆĽ - ¸ó˝şĹÍ Ŕ§Äˇ
// 			Send2FieldServerW(pSendBuf, MSG_SIZE(MSG_FN_BATTLE_ATTACK_SECONDARY));
// 			
// 			if(0 == i_pnMonster->m_pUsingMonsterItem->pItemInfo->MultiNum%2
// 				&& 127 == byExplosionPos)
// 			{
// 				byExplosionPos = 129;
// 			}
// 			else
// 			{
// 				byExplosionPos++;
// 			}
//		}
		this->SendFSvrBattleAttackSec(i_pnMonster, i_pClientInfo->ClientIndex, &i_pClientInfo->PositionVector);
		////////////////////////////////////////////////////////////////////////////////
		// 2009-09-09 ~ 2010 by dhjin, ŔÎÇÇ´ĎĆĽ - ąĚ»çŔĎ ¸ÖĆĽ Ĺ¸°Ů Ăł¸®, ¸ÂŔş ŔŻŔú Á¦żÜÇĎ°í Ăł¸®. żěĽ± ľî±×·Î´Â »ý°˘ÇĎÁö ľĘ´Â´Ů.
		if(1 < i_pnMonster->m_pUsingMonsterItem->pItemInfo->MultiTarget) {
			int MultiTargetSize = i_pnMonster->m_pUsingMonsterItem->pItemInfo->MultiTarget-1;
			if(BELL_INFINITY_ATTACK_MONSTER == i_pnMonster->MonsterInfoPtr->Belligerence) {
				// BELL_INFINITY_ATTACK_MONSTER Ăł¸®
				MultiTargetSize = (MultiTargetSize) - (this->BattleAttackSecMultiTargetUser(i_pnMonster, i_pClientInfo, MultiTargetSize));
				if(1 <= MultiTargetSize) {
					// ¸ÖĆĽ Ĺ¸°ŮŔĚ ł˛ľĆ ŔÖ´Ů¸é ¸ó˝şĹÍ °Ë»ö
					this->BattleAttackSecMultiTargetMonster(i_pnMonster, NULL, MultiTargetSize);
				}
				return;
			}
			else {
				this->BattleAttackSecMultiTargetUser(i_pnMonster, i_pClientInfo, MultiTargetSize);
			}	
		}
	}	
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			void CNPCMapChannel::SendAttack2MonFieldServer(CNPCMonster *i_pnMonster, CNPCMonster *i_pTargetMonster, BYTE *pSendBuf)
/// \brief		ŔÎÇÇ´ĎĆĽ - ¸ó˝şĹÍ°Ł °ř°Ý 
/// \author		dhjin
/// \date		2009-09-09 ~ 2010
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CNPCMapChannel::SendAttack2MonFieldServer(CNPCMonster *i_pnMonster, CNPCMonster *i_pTargetMonster, BYTE *pSendBuf) {
	////////////////////////////////////////////////////////////////////////////////
	// 2009-09-09 ~ 2010 by dhjin, ŔÎÇÇ´ĎĆĽ - ÇöŔç´Â 2Áľ·ů Bell¸ó˝şĹÍ¸¸ Ăł¸®ÇŃ´Ů.
	if(BELL_INFINITY_DEFENSE_MONSTER != i_pnMonster->MonsterInfoPtr->Belligerence
		&& BELL_INFINITY_ATTACK_MONSTER != i_pnMonster->MonsterInfoPtr->Belligerence) {
		return;
	}

	// start 2011-03-22 by hskim, ŔÎÇÇ´ĎĆĽ 3Â÷ - ¸ó˝şĹÍ ¸ÖĆĽ Ĺ¸°ŮĆĂ ±â´É Ăß°ˇ
	int fMonsterMultiTargetIndex = 0;
  	D3DXVECTOR3 fMonsterMultiTargetVector = i_pTargetMonster->GetNearMultiTarget(i_pnMonster->PositionVector, &fMonsterMultiTargetIndex);
	// end 2011-03-22 by hskim, ŔÎÇÇ´ĎĆĽ 3Â÷ - ¸ó˝şĹÍ ¸ÖĆĽ Ĺ¸°ŮĆĂ ±â´É Ăß°ˇ

	// 2010-03-31 by dhjin, ŔÎÇÇ´ĎĆĽ(±âÁöąćľî) -
	// 2009-09-09 ~ 2010-01-13 by dhjin, ŔÎÇÇ´ĎĆĽ - ÇŃ ľĆŔĚĹŰŔ¸·Î ż©·Ż ´ë»ç °ˇ´ÉÇĎ°Ô ĽöÁ¤, ąŘ°ú °°ŔĚ ĽöÁ¤ HPActionItem »çżë˝Ă »çżë Ä«żîĆ®¸¦ ÁŮŔÎ´Ů.
	i_pnMonster->m_HPAction.SetSuccessAttackItemIdxHPRate();
	HPACTION_TALK_HPRATE MsgTalk;
	MEMSET_ZERO(&MsgTalk, sizeof(HPACTION_TALK_HPRATE));
	if(i_pnMonster->m_HPAction.GetHPTalkAttack(i_pnMonster->m_pUsingMonsterItem->pItemInfo->ItemNum, MsgTalk.HPTalk, &MsgTalk.HPCameraTremble)) {
		// Attack°Ş ŔüĽŰ.
		MsgTalk.HPTalkImportance	= HPACTION_TALK_IMPORTANCE_CHANNEL;
		this->SendFSvrHPTalk(i_pnMonster, &MsgTalk);
	}

	if(ITEMKIND_FOR_MON_SKILL == i_pnMonster->m_pUsingMonsterItem->pItemInfo->Kind) {
		i_pnMonster->UseSkill(i_pTargetMonster->MonsterIndex);		// 2010-03-31 by dhjin, ŔÎÇÇ´ĎĆĽ(±âÁöąćľî) -
		return;
	}
	
	if(IS_PRIMARY_WEAPON_MONSTER(i_pnMonster->m_pUsingMonsterItem->pItemInfo->Kind))
	{
		///////////////////////////////////////////////////////////////////////
		// MultiNum Ăł¸®
		for(int i = 0; i < i_pnMonster->m_pUsingMonsterItem->pItemInfo->MultiNum; i++)
		{
			INIT_MSG(MSG_FN_BATTLE_ATTACK_PRIMARY, T_FN_BATTLE_ATTACK_PRIMARY, pSendBattleAttackPri, pSendBuf);
			pSendBattleAttackPri->ChannelIndex		= m_MapChannelIndex.ChannelIndex;
			pSendBattleAttackPri->AttackIndex		= i_pnMonster->MonsterIndex;
			pSendBattleAttackPri->TargetIndex		= i_pTargetMonster->MonsterIndex;			
			pSendBattleAttackPri->WeaponItemNumber	= i_pnMonster->m_pUsingMonsterItem->pItemInfo->ItemNum;
			pSendBattleAttackPri->WeaponIndex		= 0;

			// start 2011-03-22 by hskim, ŔÎÇÇ´ĎĆĽ 3Â÷ - ¸ó˝şĹÍ ¸ÖĆĽ Ĺ¸°ŮĆĂ ±â´É Ăß°ˇ
			///////////////
			// ±âÁ¸ ÄÚµĺ //
			//pSendBattleAttackPri->TargetPosition	= i_pTargetMonster->PositionVector;
			///////////////
			// ĽöÁ¤ ÄÚµĺ //
			pSendBattleAttackPri->TargetPosition	= fMonsterMultiTargetVector;
			pSendBattleAttackPri->MultiTargetIndex	= fMonsterMultiTargetIndex;
			// end 2011-03-22 by hskim, ŔÎÇÇ´ĎĆĽ 3Â÷ - ¸ó˝şĹÍ ¸ÖĆĽ Ĺ¸°ŮĆĂ ±â´É Ăß°ˇ
			Send2FieldServerW(pSendBuf, MSG_SIZE(MSG_FN_BATTLE_ATTACK_PRIMARY));
		}
	}
	else
	{
		////////////////////////////////////////////////////////////////////////////////
		// 2009-09-09 ~ 2010 by dhjin, ŔÎÇÇ´ĎĆĽ - ąŘżˇ ÇÔĽö·Î şŻ°ć
// 		///////////////////////////////////////////////////////////////////////
// 		// MultiNum Ăł¸®
// 		BYTE byExplosionPos = 128 - i_pnMonster->m_pUsingMonsterItem->pItemInfo->MultiNum/2;		
// 		for(int i = 0; i < i_pnMonster->m_pUsingMonsterItem->pItemInfo->MultiNum; i++)
// 		{
// 			INIT_MSG(MSG_FN_BATTLE_ATTACK_SECONDARY, T_FN_BATTLE_ATTACK_SECONDARY, pSendBattleAttackSec, pSendBuf);
// 			pSendBattleAttackSec->ChannelIndex		= m_MapChannelIndex.ChannelIndex;
// 			pSendBattleAttackSec->AttackIndex		= i_pnMonster->MonsterIndex;
// 			pSendBattleAttackSec->TargetIndex		= i_pTargetMonster->MonsterIndex;
// 			pSendBattleAttackSec->WeaponIndex		= m_uiMissileUniqueIndex++;
// 			pSendBattleAttackSec->WeaponItemNumber	= i_pnMonster->m_pUsingMonsterItem->pItemInfo->ItemNum;
// 			pSendBattleAttackSec->TargetPosition	= i_pTargetMonster->PositionVector;
// 			pSendBattleAttackSec->Distance			= byExplosionPos;
// 			pSendBattleAttackSec->SecAttackType		= 0;
// 			pSendBattleAttackSec->AttackPosition	= i_pnMonster->PositionVector;		// 2009-09-09 ~ 2010 by dhjin, ŔÎÇÇ´ĎĆĽ - ¸ó˝şĹÍ Ŕ§Äˇ
// 			Send2FieldServerW(pSendBuf, MSG_SIZE(MSG_FN_BATTLE_ATTACK_SECONDARY));
// 			
// 			if(0 == i_pnMonster->m_pUsingMonsterItem->pItemInfo->MultiNum%2
// 				&& 127 == byExplosionPos)
// 			{
// 				byExplosionPos = 129;
// 			}
// 			else
// 			{
// 				byExplosionPos++;
// 			}
// 		}

		// start 2011-03-22 by hskim, ŔÎÇÇ´ĎĆĽ 3Â÷ - ¸ó˝şĹÍ ¸ÖĆĽ Ĺ¸°ŮĆĂ ±â´É Ăß°ˇ
		///////////////
		// ±âÁ¸ ÄÚµĺ //
		// this->SendFSvrBattleAttackSec(i_pnMonster, i_pTargetMonster->MonsterIndex, &i_pTargetMonster->PositionVector);
		///////////////
		// ĽöÁ¤ ÄÚµĺ //
		this->SendFSvrBattleAttackSec(i_pnMonster, i_pTargetMonster->MonsterIndex, &fMonsterMultiTargetVector, fMonsterMultiTargetIndex);
		// end 2011-03-22 by hskim, ŔÎÇÇ´ĎĆĽ 3Â÷ - ¸ó˝şĹÍ ¸ÖĆĽ Ĺ¸°ŮĆĂ ±â´É Ăß°ˇ
		
		////////////////////////////////////////////////////////////////////////////////
		// 2009-09-09 ~ 2010 by dhjin, ŔÎÇÇ´ĎĆĽ - ąĚ»çŔĎ ¸ÖĆĽ Ĺ¸°Ů Ăł¸®, ¸ÂŔş ŔŻŔú Á¦żÜÇĎ°í Ăł¸®. żěĽ± ľî±×·Î´Â »ý°˘ÇĎÁö ľĘ´Â´Ů.
		if(1 < i_pnMonster->m_pUsingMonsterItem->pItemInfo->MultiTarget) {
			if(BELL_INFINITY_DEFENSE_MONSTER == i_pnMonster->MonsterInfoPtr->Belligerence) {
				// BELL_INFINITY_DEFENSE_MONSTER Ăł¸®
				this->BattleAttackSecMultiTargetMonster(i_pnMonster, i_pTargetMonster, i_pnMonster->m_pUsingMonsterItem->pItemInfo->MultiTarget-1);
				return;
			}
			else if(BELL_INFINITY_ATTACK_MONSTER == i_pnMonster->MonsterInfoPtr->Belligerence) {
				// BELL_INFINITY_ATTACK_MONSTER Ăł¸®
				int MultiTargetSize = i_pnMonster->m_pUsingMonsterItem->pItemInfo->MultiTarget-1;
				MultiTargetSize = (MultiTargetSize) - (this->BattleAttackSecMultiTargetMonster(i_pnMonster, i_pTargetMonster, MultiTargetSize));
				if(1 <= MultiTargetSize) {
					// ¸ÖĆĽ Ĺ¸°ŮŔĚ ł˛ľĆ ŔÖ´Ů¸é ŔŻŔú¸¦ °Ë»öÇĎż© Ăł¸®
					this->BattleAttackSecMultiTargetUser(i_pnMonster, NULL, MultiTargetSize);
				}
				return;
			}
		}
	}		
}


void CNPCMapChannel::SendMonsterMove2FieldServer(CNPCMonster *i_pnMonster, BYTE *i_SendBuf)
{
	INIT_MSG(MSG_FN_MONSTER_MOVE, T_FN_MONSTER_MOVE, pSendFNMonsterMove, i_SendBuf);
	pSendFNMonsterMove->ChannelIndex	= m_MapChannelIndex.ChannelIndex;
	pSendFNMonsterMove->ClientIndex		= i_pnMonster->MonsterIndex;
	pSendFNMonsterMove->TargetIndex		= i_pnMonster->m_nTargetIndex;
	pSendFNMonsterMove->PositionVector	= i_pnMonster->PositionVector;
	pSendFNMonsterMove->TargetVector	= i_pnMonster->TargetVector*1000.0f;
	pSendFNMonsterMove->usSendRange		= i_pnMonster->SendMoveRange;

	Send2FieldServerW(i_SendBuf, MSG_SIZE(MSG_FN_MONSTER_MOVE));
}

void CNPCMapChannel::AttackMonster2Character(CNPCMonster *i_pnMonster
											 , CLIENT_INFO * i_pClientInfo
											 , BYTE *pSendBuf)
{
	BodyCond_t			tmBodyCon;
	MONSTER_ITEM		*pMonItem = i_pnMonster->m_pUsingMonsterItem;
	if(COMPARE_BODYCON_BIT(i_pnMonster->BodyCondition, BODYCON_MON_FIREATTACK_ALL_MASK))
	{
		if(1 == pMonItem->pItemInfo->ShotNum
			|| i_pnMonster->m_nCurrentShotNumCount >= pMonItem->pItemInfo->ShotNum)
		{
			if(i_pnMonster->m_dwCurrentTick - i_pnMonster->m_ArrLastReattackTime[pMonItem->byArrayIndex] >= pMonItem->pItemInfo->ReAttacktime)
			{				
				i_pnMonster->ResetAttackBodyCondition();
				i_pnMonster->SelectUsingMonsterItem();
			}
			return;
		}
	}
	
	BOOL bIsPrimaryWeapon = IS_PRIMARY_WEAPON_MONSTER(i_pnMonster->m_pUsingMonsterItem->pItemInfo->Kind)?TRUE:FALSE;

	if(i_pnMonster->m_nCurrentShotNumCount > 0)
	{
		////////////////////////////////////////////////////////////////////////////////
		// 2009-09-09 ~ 2010 by dhjin, ŔÎÇÇ´ĎĆĽ - ąŘ°ú °°ŔĚ ĽöÁ¤, ShotNumş¸´Ů ¸ąŔĚ °ř°Ý ÇŇ Ľö ľř°Ô ĽöÁ¤
		if(i_pnMonster->m_nCurrentShotNumCount >= pMonItem->pItemInfo->ShotNum) {
			i_pnMonster->ResetAttackBodyCondition();
 			i_pnMonster->SelectUsingMonsterItem();
			return;
		}

		this->SendAttack2FieldServer(i_pnMonster, i_pClientInfo, pSendBuf);

		i_pnMonster->m_nCurrentShotNumCount++;									// ShotNumŔ» Áő°ˇÇŃ´Ů.
		i_pnMonster->m_dwTimeMonsterLastAttack	= i_pnMonster->m_dwCurrentTick;	// Monster°ˇ °ř°ÝÇŃ ¸¶Áö¸· ˝Ă°Ł
	}
	else if(i_pnMonster->m_dwCurrentTick - i_pnMonster->m_ArrLastReattackTime[pMonItem->byArrayIndex] > pMonItem->pItemInfo->ReAttacktime)
	{
		float		fMaxItemRange = pMonItem->pItemInfo->Range;				
		if(FALSE == i_pnMonster->m_MoveInfo.MovableFlag
			|| EVENT_TYPE_NO_OBJECT_MONSTER != i_pnMonster->m_byObjectMonsterType
			|| FORM_GROUND_MOVE == i_pnMonster->CurrentMonsterForm)
		{// Á¤ÁöÇü, żŔşęÁ§Ć®, Áö»ó ¸ó˝şĹÍ

			fMaxItemRange *= 1.5f;
		}
		else if(0 != pMonItem->pItemInfo->AttackTime 
// 2009-09-09 ~ 2010 by dhjin, ŔÎÇÇ´ĎĆĽ - 6~15, ÄÁĆ®·Ń˝şĹł±îÁö Ăß°ˇ·Î ąŮµđÄÁµđĽÇ°Ş ľňľîżŔ´Â şÎşĐ ĽöÁ¤
//			&& COMPARE_BODYCON_BIT(i_pnMonster->BodyCondition, BODYCON_MON_PREATTACK1_MASK<<(pMonItem->byBodyConArrayIndex*COUNT_MONSTER_ATTACK_BODYCON)))
			&& COMPARE_BODYCON_BIT(i_pnMonster->BodyCondition, this->GetPreAttackBodyCondMask(pMonItem->byBodyConArrayIndex)))
		{// AttackTimeŔĚ 0ŔĚ ľĆ´Ď¸éĽ­ AttackÁŘşń°ˇ żĎ·áµČ ¸ó˝şĹÍ

			fMaxItemRange *= 0.8f;
		}

		float fDisFromAttackPosToTargetPos = D3DXVec3Length(&(i_pClientInfo->PositionVector - i_pnMonster->PositionVector));	// Ĺ¬¶óŔĚľđĆ®żÍ ¸ó˝şĹÍŔÇ °Ĺ¸®¸¦ ±¸ÇŃ´Ů.

		BOOL bNoCheck = FALSE;
		if(pMonItem->pItemInfo->AttackTime
// 2009-09-09 ~ 2010 by dhjin, ŔÎÇÇ´ĎĆĽ - 6~15, ÄÁĆ®·Ń˝şĹł±îÁö Ăß°ˇ·Î ąŮµđÄÁµđĽÇ°Ş ľňľîżŔ´Â şÎşĐ ĽöÁ¤
//			&& COMPARE_BODYCON_BIT(i_pnMonster->BodyCondition, BODYCON_MON_PREATTACK1_MASK<<(pMonItem->byBodyConArrayIndex*COUNT_MONSTER_ATTACK_BODYCON)))
			&& COMPARE_BODYCON_BIT(i_pnMonster->BodyCondition, this->GetPreAttackBodyCondMask(pMonItem->byBodyConArrayIndex)))
		{
			bNoCheck = TRUE;
		}
		if(bNoCheck
			|| fDisFromAttackPosToTargetPos <= fMaxItemRange)
		{
			D3DXVECTOR3		tmUnitVec3M2C;
			D3DXVec3Normalize(&tmUnitVec3M2C, &(i_pClientInfo->PositionVector - i_pnMonster->PositionVector));		// ÇöŔç ¸ó˝şĹÍżˇĽ­ Ĺ¬¶óŔĚľđĆ®¸¦ ÇâÇĎ´Â Target Vector¸¦ ±¸ÇŃ´Ů.
			float fPinPoint = ACOS(D3DXVec3Dot(&i_pnMonster->TargetVector, &tmUnitVec3M2C));						// ¸ó˝şĹÍżˇĽ­ Target VectorżÍ Ŕ§żˇĽ­ ±¸ÇŃ ÇöŔç Target Vector »çŔĚŔÇ °˘Ŕ» ±¸ÇŃ´Ů
			if(bNoCheck
				||( fPinPoint <= pMonItem->pItemInfo->RangeAngle && FALSE == CheckImpactStraightLineMapAndObjects(&i_pnMonster->PositionVector, &i_pClientInfo->PositionVector, DEFAULT_OBJECT_MONSTER_OBJECT+i_pnMonster->MonsterInfoPtr->MonsterUnitKind) )
				)
			{
				if(pMonItem->pItemInfo->AttackTime == 0
// 2009-09-09 ~ 2010 by dhjin, ŔÎÇÇ´ĎĆĽ - 6~15, ÄÁĆ®·Ń˝şĹł±îÁö Ăß°ˇ·Î ąŮµđÄÁµđĽÇ°Ş ľňľîżŔ´Â şÎşĐ ĽöÁ¤
//					|| (COMPARE_BODYCON_BIT(i_pnMonster->BodyCondition, BODYCON_MON_PREATTACK1_MASK<<(pMonItem->byBodyConArrayIndex*COUNT_MONSTER_ATTACK_BODYCON)) && i_pnMonster->m_dwCurrentTick - i_pnMonster->m_dwLastAttackTime > pMonItem->pItemInfo->AttackTime)
					|| (COMPARE_BODYCON_BIT(i_pnMonster->BodyCondition, this->GetPreAttackBodyCondMask(pMonItem->byBodyConArrayIndex)) && i_pnMonster->m_dwCurrentTick - i_pnMonster->m_dwLastAttackTime > pMonItem->pItemInfo->AttackTime) // 2009-09-09 ~ 2010-01-18 by dhjin, ŔÎÇÇ´ĎĆĽ - ti_item -> AttackTime ĂĽĹ© şüÁř şÎşĐ Ăß°ˇ
					)
				{
					tmBodyCon = i_pnMonster->BodyCondition;
					CLEAR_BODYCON_BIT(tmBodyCon, BODYCON_MON_ATTACKALL_MASK);
// 2009-09-09 ~ 2010 by dhjin, ŔÎÇÇ´ĎĆĽ - 6~15, ÄÁĆ®·Ń˝şĹł±îÁö Ăß°ˇ·Î ąŮµđÄÁµđĽÇ°Ş ľňľîżŔ´Â şÎşĐ ĽöÁ¤
//					SET_BODYCON_BIT(tmBodyCon, BODYCON_MON_FIREATTACK1_MASK<<(pMonItem->byBodyConArrayIndex*COUNT_MONSTER_ATTACK_BODYCON));
					SET_BODYCON_BIT(tmBodyCon, this->GetFireAttackBodyCondMask(pMonItem->byBodyConArrayIndex));
					i_pnMonster->ChangeBodyCondition(&tmBodyCon);
					
					INIT_MSG(MSG_FN_MONSTER_CHANGE_BODYCONDITION, T_FN_MONSTER_CHANGE_BODYCONDITION, pSeBody, pSendBuf);
					pSeBody->ChannelIndex	= this->m_MapChannelIndex.ChannelIndex;
					pSeBody->ClientIndex	= i_pnMonster->MonsterIndex;
					pSeBody->BodyCondition	= i_pnMonster->BodyCondition;
					Send2FieldServerW(pSendBuf, MSG_SIZE(MSG_FN_MONSTER_CHANGE_BODYCONDITION));

					if(FALSE == NPCMonsterAttackSkill(i_pnMonster, pMonItem->pItemInfo))
					{
						this->SendAttack2FieldServer(i_pnMonster, i_pClientInfo, pSendBuf);
					}
					
					i_pnMonster->m_dwTimeMonsterLastAttack	= i_pnMonster->m_dwCurrentTick;	// Monster°ˇ °ř°ÝÇŃ ¸¶Áö¸· ˝Ă°Ł
					i_pnMonster->m_nCurrentShotNumCount		= 1;							// ShotNumŔÇ ˝ĂŔŰŔ» łŞĹ¸ł˝´Ů, ShotNum == 1 ŔĚľîµµ »ę°üľřŔ˝
					i_pnMonster->m_ArrLastReattackTime[pMonItem->byArrayIndex] = i_pnMonster->m_dwCurrentTick;	// ReAttackTimeŔ» UpdateÇŃ´Ů.
					
					///////////////////////////////////////////////////////////////////////////////
					// ¸öĹë °ř°Ý ¸ó˝şĹÍ°ˇ °ř°Ý ČÄ Ăł¸®
					if(pMonItem->pItemInfo->OrbitType == ORBIT_BODYSLAM)
					{
						tmUnitVec3M2C.y = 0.0f;
						D3DXVec3Normalize(&tmUnitVec3M2C, &tmUnitVec3M2C);
						GNPCRotateTargetVectorHorizontal(&tmUnitVec3M2C, &tmUnitVec3M2C, i_pnMonster->m_MoveInfo.LRDirect*(MONSTER_MAX_QUICK_TURN_ANGLE - i_pnMonster->MonsterInfoPtr->QuickTurnAngle), i_pnMonster->MonsterInfoPtr->QuickTurnAngle);
						i_pnMonster->SetEnforceTargetVector(&tmUnitVec3M2C, i_pnMonster->GetSpeed(), MSS_QUICK_TURN_GENERAL);
						
						if(MSS_QUICK_TURN_GENERAL == i_pnMonster->m_enMoveState)
						{
							tmBodyCon = i_pnMonster->BodyCondition;
							SET_BODYCON_BIT(tmBodyCon, BODYCON_BOOSTER3_MASK);
							i_pnMonster->ChangeBodyCondition(&tmBodyCon);
							
							INIT_MSG(MSG_FN_MONSTER_CHANGE_BODYCONDITION, T_FN_MONSTER_CHANGE_BODYCONDITION, pSeBody, pSendBuf);
							pSeBody->ChannelIndex	= this->m_MapChannelIndex.ChannelIndex;
							pSeBody->ClientIndex	= i_pnMonster->MonsterIndex;
							pSeBody->BodyCondition	= i_pnMonster->BodyCondition;
							Send2FieldServerW(pSendBuf, MSG_SIZE(MSG_FN_MONSTER_CHANGE_BODYCONDITION));
						}
					}
				}
// 2009-09-09 ~ 2010 by dhjin, ŔÎÇÇ´ĎĆĽ - 6~15, ÄÁĆ®·Ń˝şĹł±îÁö Ăß°ˇ·Î ąŮµđÄÁµđĽÇ°Ş ľňľîżŔ´Â şÎşĐ ĽöÁ¤
//				else if(FALSE == COMPARE_BODYCON_BIT(i_pnMonster->BodyCondition, BODYCON_MON_PREATTACK1_MASK<<(pMonItem->byBodyConArrayIndex*COUNT_MONSTER_ATTACK_BODYCON)))
				else if(FALSE == COMPARE_BODYCON_BIT(i_pnMonster->BodyCondition, this->GetPreAttackBodyCondMask(pMonItem->byBodyConArrayIndex)))
				{
					i_pnMonster->m_dwLastAttackTime = i_pnMonster->m_dwCurrentTick;
					
					tmBodyCon = i_pnMonster->BodyCondition;
					CLEAR_BODYCON_BIT(tmBodyCon, BODYCON_MON_ATTACKALL_MASK);
// 2009-09-09 ~ 2010 by dhjin, ŔÎÇÇ´ĎĆĽ - 6~15, ÄÁĆ®·Ń˝şĹł±îÁö Ăß°ˇ·Î ąŮµđÄÁµđĽÇ°Ş ľňľîżŔ´Â şÎşĐ ĽöÁ¤
//					SET_BODYCON_BIT(tmBodyCon, BODYCON_MON_PREATTACK1_MASK<<(pMonItem->byBodyConArrayIndex*COUNT_MONSTER_ATTACK_BODYCON));
					SET_BODYCON_BIT(tmBodyCon, this->GetPreAttackBodyCondMask(pMonItem->byBodyConArrayIndex));
					i_pnMonster->ChangeBodyCondition(&tmBodyCon);
					
					INIT_MSG(MSG_FN_MONSTER_CHANGE_BODYCONDITION, T_FN_MONSTER_CHANGE_BODYCONDITION, pSeBody, pSendBuf);
					pSeBody->ChannelIndex	= this->m_MapChannelIndex.ChannelIndex;
					pSeBody->ClientIndex	= i_pnMonster->MonsterIndex;
					pSeBody->BodyCondition	= i_pnMonster->BodyCondition;
					Send2FieldServerW(pSendBuf, MSG_SIZE(MSG_FN_MONSTER_CHANGE_BODYCONDITION));
				}
			}
			else
			{
				i_pnMonster->ResetAttackBodyCondition();
			}
		}
		else
		{
			// 2007-01-15 by cmkwon, »ç°Ĺ¸® ¶§ą®żˇ °ř°ÝÇĎÁö ¸řÇĎ¸é ą«±â¸¦ ±łČŻÇĎµµ·Ď Ăł¸®ÇŃ´Ů
			i_pnMonster->SelectUsingMonsterItem();
			i_pnMonster->ResetAttackBodyCondition();
		}
	}
}


// 2005-01-19 by cmkwon
//
/////////////////////////////////////////////////////////////////////////////////
///// \fn			void CNPCMapChannel::AttackMonster2CharacterWithPrimaryItem(CNPCMonster *i_pnMonster
/////															, CLIENT_INFO * i_pClientInfo
/////															, MONSTER_ITEM *i_pItemInfo
/////															, BYTE *pSendBuf
/////															, DWORD dwCurrentTick)
///// \brief		¸ó˝şĹÍ°ˇ PrimaryItemŔ¸·Î Äł¸ŻĹÍ¸¦ °ř°ÝÇĎ´Â Ăł¸®
///// \author		cmkwon
///// \date		2004-04-02 ~ 2004-04-02
///// \warning	
/////
///// \param		
///// \return		
/////////////////////////////////////////////////////////////////////////////////
//void CNPCMapChannel::AttackMonster2CharacterWithPrimaryItem(CNPCMonster *i_pnMonster
//															, CLIENT_INFO * i_pClientInfo
//															, BYTE *pSendBuf)
//{	
//	float							fDisFromAttackPosToTargetPos;			// °ř°ÝŔÚżÍ ÇÇ°ř°ÝŔÚżÍŔÇ Position °Ĺ¸®
//	D3DXVECTOR3						TempVector3;
//	float							fPinPoint=0.0f;
//	BodyCond_t						tmBodyCon;
//	MONSTER_ITEM					*pMonItem = i_pnMonster->m_pUsingMonsterItem;
//	float							fMaxItemRange = pMonItem->pItemInfo->Range;
//
//	///////////////////////////////////////////////////////////////////////////////
//	// 1Çü ą«±â´Â »çÁ¤°Ĺ¸®ş¸´Ů Ĺ©´ő¶óµµ ąß»ç ÇŃ´Ů
//	// ¸öĹë °ř°Ý ľĆŔĚĹŰ ¸ó˝şĹÍ¸¦ Á¦żÜÇĎ°í
//	if(FALSE == i_pnMonster->m_MoveInfo.MovableFlag
//		|| EVENT_TYPE_NO_OBJECT_MONSTER != i_pnMonster->m_byObjectMonsterType)
//	{
//		fMaxItemRange *= 2;
//	}
//	else if(pMonItem->pItemInfo->OrbitType != ORBIT_BODYSLAM)
//	{
//		fMaxItemRange *= 1.5f;
//	}
//
//	///////////////////////////////////////////////////////////////////////
//	// 1. ShotNumŔĚ 2ŔĚ»óŔĚ°í ˝î±â ˝ĂŔŰÇßŔ»°ćżě
//	// 2. 1ŔĚ ľĆ´Ď¸éĽ­ ReAttack ˝Ă°ŁŔĚ Áöł­°ćżě
//	// 3. 1,2ŔĚ ľĆ´Ń°ćżě
//	if(pMonItem->pItemInfo->ShotNum > 1
//		&& i_pnMonster->m_nCurrentShotNumCount > 0)
//	{	// PrimaryItemŔÇ ShotNumŔĚ 1ş¸´Ů Ĺ©°í AttackCount°ˇ 0ş¸´Ů Ĺ©¸é ż¬»ç¸¦ ˝ĂŔŰÇŃ °ćżě ŔĚąÇ·Î ąß»ç
//
//		///////////////////////////////////////////////////////////////////////
//		// MultiNum Ăł¸®
//		for(int i = 0; i < pMonItem->pItemInfo->MultiNum; i++)
//		{
//			INIT_MSG(MSG_FN_BATTLE_ATTACK_PRIMARY, T_FN_BATTLE_ATTACK_PRIMARY, pSendBattleAttackPri, pSendBuf);
//			pSendBattleAttackPri->ChannelIndex		= m_MapChannelIndex.ChannelIndex;
//			pSendBattleAttackPri->AttackIndex		= i_pnMonster->MonsterIndex;
//			pSendBattleAttackPri->TargetIndex		= i_pClientInfo->ClientIndex;			
//			pSendBattleAttackPri->WeaponItemNumber	= pMonItem->pItemInfo->ItemNum;
//			pSendBattleAttackPri->WeaponIndex		= 0;
//			pSendBattleAttackPri->TargetPosition	= i_pClientInfo->PositionVector;
//			Send2FieldServerW(pSendBuf, MSG_SIZE(MSG_FN_BATTLE_ATTACK_PRIMARY));
//		}
//
//		///////////////////////////////////////////////////////////////////////
//		// Monster°ˇ °ř°ÝÇŃ ¸¶Áö¸· ˝Ă°Ł
//		i_pnMonster->m_dwTimeMonsterLastAttack = i_pnMonster->m_dwCurrentTick;
//
//		///////////////////////////////////////////////////////////////////////
//		// ShotNumŔ» Áő°ˇÇŃ´Ů.
//		i_pnMonster->m_nCurrentShotNumCount++;
//
//		///////////////////////////////////////////////////////////////////////
//		// ShotNum ¸¸Ĺ­ ąß»ç°ˇ µÇľúŔ¸¸é ShotNum Count¸¦ ĂĘ±âČ­ ÇŃ´Ů.	
//		if(pMonItem->pItemInfo->ShotNum <= i_pnMonster->m_nCurrentShotNumCount)
//		{
//			i_pnMonster->m_nCurrentShotNumCount	= 0;
//			i_pnMonster->m_ArrLastReattackTime[pMonItem->byArrayIndex]		= i_pnMonster->m_dwCurrentTick;
//			
//			tmBodyCon = i_pnMonster->BodyCondition;
//			CLEAR_BODYCON_BIT(tmBodyCon, BODYCON_MON_ATTACKALL_MASK);
//			i_pnMonster->ChangeBodyCondition(&tmBodyCon);
//		}
//	}
//	else if(i_pnMonster->m_dwCurrentTick - i_pnMonster->m_ArrLastReattackTime[pMonItem->byArrayIndex] > pMonItem->pItemInfo->ReAttacktime)
//	{	//
//		if(COMPARE_BODYCON_BIT(i_pnMonster->BodyCondition, BODYCON_MON_FIREATTACK1_MASK<<(pMonItem->byArrayIndex*COUNT_MONSTER_ATTACK_BODYCON)))
//		{
//			tmBodyCon = i_pnMonster->BodyCondition;
//			CLEAR_BODYCON_BIT(tmBodyCon, BODYCON_MON_ATTACKALL_MASK);			
//			i_pnMonster->ChangeBodyCondition(&tmBodyCon);
//		}
//		else
//		{
//			fDisFromAttackPosToTargetPos = D3DXVec3Length(&(i_pClientInfo->PositionVector - i_pnMonster->PositionVector));	// Ĺ¬¶óŔĚľđĆ®żÍ ¸ó˝şĹÍŔÇ °Ĺ¸®¸¦ ±¸ÇŃ´Ů.
//
//			if(  (0 != pMonItem->pItemInfo->AttackTime && FALSE == COMPARE_BODYCON_BIT(i_pnMonster->BodyCondition, BODYCON_MON_PREATTACK1_MASK<<(pMonItem->byArrayIndex*COUNT_MONSTER_ATTACK_BODYCON)) && fDisFromAttackPosToTargetPos < fMaxItemRange*2/3)
//				|| ( (0 == pMonItem->pItemInfo->AttackTime || COMPARE_BODYCON_BIT(i_pnMonster->BodyCondition, BODYCON_MON_PREATTACK1_MASK<<(pMonItem->byArrayIndex*COUNT_MONSTER_ATTACK_BODYCON))) && fDisFromAttackPosToTargetPos < fMaxItemRange )  )
////			if(fDisFromAttackPosToTargetPos < pMonItem->pItemInfo->Range)
//			{	// Primary ľĆŔĚĹŰŔÇ °ř°Ý ŔŻČż °Ĺ¸®żˇ ŔÖŔ˝
//				
//				D3DXVec3Normalize(&TempVector3, &(i_pClientInfo->PositionVector - i_pnMonster->PositionVector));		// ÇöŔç ¸ó˝şĹÍżˇĽ­ Ĺ¬¶óŔĚľđĆ®¸¦ ÇâÇĎ´Â Target Vector¸¦ ±¸ÇŃ´Ů.
//				fPinPoint = ACOS(D3DXVec3Dot(&i_pnMonster->TargetVector, &TempVector3));								// ¸ó˝şĹÍżˇĽ­ Target VectorżÍ Ŕ§żˇĽ­ ±¸ÇŃ ÇöŔç Target Vector »çŔĚŔÇ °˘Ŕ» ±¸ÇŃ´Ů
//				if(fPinPoint <= pMonItem->pItemInfo->RangeAngle
//					&& FALSE == CheckImpactStraightLineMapAndObjects(&i_pnMonster->PositionVector, &i_pClientInfo->PositionVector))
//				{	// ľĆŔĚĹŰŔÇ °ř°Ý ŔŻČż°˘ŔĚ¸ç Ĺ¸°Ů±îÁö Á÷Ľ±Ŕ¸·Î ¸ĘŔÇ ĂćµąŔĚ ľřŔ˝
//				
//					if(pMonItem->pItemInfo->AttackTime == 0
//						|| (COMPARE_BODYCON_BIT(i_pnMonster->BodyCondition, BODYCON_MON_PREATTACK1_MASK<<(pMonItem->byArrayIndex*COUNT_MONSTER_ATTACK_BODYCON))
//						&& i_pnMonster->m_dwCurrentTick - i_pnMonster->m_dwLastAttackTime > pMonItem->pItemInfo->AttackTime)
//						)
//					{
//						tmBodyCon = i_pnMonster->BodyCondition;
//						CLEAR_BODYCON_BIT(tmBodyCon, BODYCON_MON_ATTACKALL_MASK);
//						SET_BODYCON_BIT(tmBodyCon, BODYCON_MON_FIREATTACK1_MASK<<(pMonItem->byArrayIndex*COUNT_MONSTER_ATTACK_BODYCON));
//						i_pnMonster->ChangeBodyCondition(&tmBodyCon);
//						
//						INIT_MSG(MSG_FN_MONSTER_CHANGE_BODYCONDITION, T_FN_MONSTER_CHANGE_BODYCONDITION, pSeBody, pSendBuf);
//						pSeBody->ChannelIndex	= this->m_MapChannelIndex.ChannelIndex;
//						pSeBody->ClientIndex	= i_pnMonster->MonsterIndex;
//						pSeBody->BodyCondition	= i_pnMonster->BodyCondition;
//						Send2FieldServerW(pSendBuf, MSG_SIZE(MSG_FN_MONSTER_CHANGE_BODYCONDITION));
//						
//						if(FALSE == NPCMonsterAttackSkill(i_pnMonster, pMonItem->pItemInfo))
//						{
//							///////////////////////////////////////////////////////////////////////////////
//							// ¸ÖĆĽłŃ Ăł¸®
//							for(int i = 0; i < pMonItem->pItemInfo->MultiNum; i++)
//							{
//								INIT_MSG(MSG_FN_BATTLE_ATTACK_PRIMARY, T_FN_BATTLE_ATTACK_PRIMARY, pSendBattleAttackPri, pSendBuf);
//								pSendBattleAttackPri->ChannelIndex		= m_MapChannelIndex.ChannelIndex;
//								pSendBattleAttackPri->AttackIndex		= i_pnMonster->MonsterIndex;
//								pSendBattleAttackPri->TargetIndex		= i_pClientInfo->ClientIndex;
//								pSendBattleAttackPri->WeaponItemNumber	= pMonItem->pItemInfo->ItemNum;
//								pSendBattleAttackPri->WeaponIndex		= 0;
//								pSendBattleAttackPri->TargetPosition	= i_pClientInfo->PositionVector;
//								Send2FieldServerW(pSendBuf, MSG_SIZE(MSG_FN_BATTLE_ATTACK_PRIMARY));
//							}
//						}
//						///////////////////////////////////////////////////////////////////////
//						// Monster°ˇ °ř°ÝÇŃ ¸¶Áö¸· ˝Ă°Ł
//						i_pnMonster->m_dwTimeMonsterLastAttack = i_pnMonster->m_dwCurrentTick;
//						
//						///////////////////////////////////////////////////////////////////////////////
//						// ShotNumŔÇ ˝ĂŔŰŔ» łŞĹ¸ł˝´Ů, ShotNum == 1 ŔĚľîµµ »ę°üľřŔ˝
//						i_pnMonster->m_nCurrentShotNumCount = 1;
//						
//						///////////////////////////////////////////////////////////////////////////////
//						// ReAttackTimeŔ» UpdateÇŃ´Ů.
//						i_pnMonster->m_ArrLastReattackTime[pMonItem->byArrayIndex] = i_pnMonster->m_dwCurrentTick;
//						
//						///////////////////////////////////////////////////////////////////////////////
//						// ¸öĹë °ř°Ý ¸ó˝şĹÍ°ˇ °ř°Ý ČÄ Ăł¸®
//						if(pMonItem->pItemInfo->OrbitType == ORBIT_BODYSLAM)
//						{
//							TempVector3.y = 0.0f;
//							D3DXVec3Normalize(&TempVector3, &TempVector3);
//							GNPCRotateTargetVectorHorizontal(&TempVector3, &TempVector3, i_pnMonster->m_MoveInfo.LRDirect*(MONSTER_MAX_QUICK_TURN_ANGLE - i_pnMonster->MonsterInfoPtr->QuickTurnAngle), i_pnMonster->MonsterInfoPtr->QuickTurnAngle);
//							i_pnMonster->SetEnforceTargetVector(&TempVector3, i_pnMonster->GetSpeed(), MSS_QUICK_TURN_GENERAL);
//							
//							if(MSS_QUICK_TURN_GENERAL == i_pnMonster->m_enMoveState)
//							{
//								tmBodyCon = i_pnMonster->BodyCondition;
//								SET_BODYCON_BIT(tmBodyCon, BODYCON_BOOSTER3_MASK);
//								i_pnMonster->ChangeBodyCondition(&tmBodyCon);
//								
//								INIT_MSG(MSG_FN_MONSTER_CHANGE_BODYCONDITION, T_FN_MONSTER_CHANGE_BODYCONDITION, pSeBody, pSendBuf);
//								pSeBody->ChannelIndex	= this->m_MapChannelIndex.ChannelIndex;
//								pSeBody->ClientIndex	= i_pnMonster->MonsterIndex;
//								pSeBody->BodyCondition	= i_pnMonster->BodyCondition;
//								Send2FieldServerW(pSendBuf, MSG_SIZE(MSG_FN_MONSTER_CHANGE_BODYCONDITION));
//							}
//						}
//					}
//					else if(FALSE == COMPARE_BODYCON_BIT(i_pnMonster->BodyCondition, BODYCON_MON_PREATTACK1_MASK<<(pMonItem->byArrayIndex*COUNT_MONSTER_ATTACK_BODYCON)))
//					{
//						i_pnMonster->m_dwLastAttackTime = i_pnMonster->m_dwCurrentTick;
//						
//						tmBodyCon = i_pnMonster->BodyCondition;
//						CLEAR_BODYCON_BIT(tmBodyCon, BODYCON_MON_ATTACKALL_MASK);
//						SET_BODYCON_BIT(tmBodyCon, BODYCON_MON_PREATTACK1_MASK<<(pMonItem->byArrayIndex*COUNT_MONSTER_ATTACK_BODYCON));
//						i_pnMonster->ChangeBodyCondition(&tmBodyCon);
//						
//						INIT_MSG(MSG_FN_MONSTER_CHANGE_BODYCONDITION, T_FN_MONSTER_CHANGE_BODYCONDITION, pSeBody, pSendBuf);
//						pSeBody->ChannelIndex	= this->m_MapChannelIndex.ChannelIndex;
//						pSeBody->ClientIndex	= i_pnMonster->MonsterIndex;
//						pSeBody->BodyCondition	= i_pnMonster->BodyCondition;
//						Send2FieldServerW(pSendBuf, MSG_SIZE(MSG_FN_MONSTER_CHANGE_BODYCONDITION));
//					}
//				}
//				else if(COMPARE_BODYCON_BIT(i_pnMonster->BodyCondition, BODYCON_MON_PREATTACK1_MASK<<(pMonItem->byArrayIndex*COUNT_MONSTER_ATTACK_BODYCON))
//					&& i_pnMonster->m_dwCurrentTick - i_pnMonster->m_dwLastAttackTime > pMonItem->pItemInfo->AttackTime)
//				{
//					tmBodyCon = i_pnMonster->BodyCondition;
//					CLEAR_BODYCON_BIT(tmBodyCon, BODYCON_MON_ATTACKALL_MASK);
//					i_pnMonster->ChangeBodyCondition(&tmBodyCon);
//				}
//				else if(COMPARE_BODYCON_BIT(i_pnMonster->BodyCondition, BODYCON_MON_FIREATTACK1_MASK<<(pMonItem->byArrayIndex*COUNT_MONSTER_ATTACK_BODYCON)))
//				{	// Attack BodyConditionŔ» ĂĘ±âČ­, AngleŔĚ ľČµÇ°ĹłŞ Á÷Ľ±°Ĺ¸®żˇ ĂćµąŔĚ ŔÖ´Ů
//					
//					tmBodyCon = i_pnMonster->BodyCondition;
//					CLEAR_BODYCON_BIT(tmBodyCon, BODYCON_MON_ATTACKALL_MASK);
//					i_pnMonster->ChangeBodyCondition(&tmBodyCon);
//				}
//			}
//			else
//			{	// Attack BodyConditionŔ» ĂĘ±âČ­, ľĆŔĚĹŰ ŔŻČż °Ĺ¸®¸¦ ąţľîłµ´Ů.
//				
//				if(COMPARE_BODYCON_BIT(i_pnMonster->BodyCondition, BODYCON_MON_ATTACK1_MASK<<(pMonItem->byArrayIndex*COUNT_MONSTER_ATTACK_BODYCON)))
//				{
//					tmBodyCon = i_pnMonster->BodyCondition;
//					CLEAR_BODYCON_BIT(tmBodyCon, BODYCON_MON_ATTACKALL_MASK);
//					i_pnMonster->ChangeBodyCondition(&tmBodyCon);
//				}
//			}
//		}		
//	}
//	else
//	{	// 
//
//	}
//}
//
//void CNPCMapChannel::AttackMonster2CharacterWithSecondaryItem(CNPCMonster *i_pnMonster
//															  , CLIENT_INFO * i_pClientInfo
//															  , BYTE *pSendBuf)
//{
//	float							fDisFromAttackPosToTargetPos;			// °ř°ÝŔÚżÍ ÇÇ°ř°ÝŔÚżÍŔÇ Position °Ĺ¸®
//	D3DXVECTOR3						TempVector3;
//	float							fPinPoint=0.0f;
//	BodyCond_t						tmBodyCon;
//	MONSTER_ITEM					*pMonItem = i_pnMonster->m_pUsingMonsterItem;
//	float							fMaxItemRange = pMonItem->pItemInfo->Range;
//
//
//	///////////////////////////////////////////////////////////////////////////////
//	// 2004-12-06 by cmkwon, 2Çü ą«±âµµ »çÁ¤°Ĺ¸®ş¸´Ů Ĺ©´ő¶óµµ ąß»ç ÇŃ´Ů
//	// ¸öĹë °ř°Ý ľĆŔĚĹŰ ¸ó˝şĹÍ¸¦ Á¦żÜÇĎ°í
//	if(FALSE == i_pnMonster->m_MoveInfo.MovableFlag
//		|| EVENT_TYPE_NO_OBJECT_MONSTER != i_pnMonster->m_byObjectMonsterType)
//	{
//		fMaxItemRange *= 2.0f;
//	}
//
//	///////////////////////////////////////////////////////////////////////
//	// 1. ShotNumŔĚ 2ŔĚ»óŔĚ°í ˝î±â ˝ĂŔŰÇßŔ»°ćżě
//	// 2. 1ŔĚ ľĆ´Ď¸éĽ­ ReAttack ˝Ă°ŁŔĚ Áöł­°ćżě
//	// 3. 1,2ŔĚ ľĆ´Ń°ćżě
//	if(pMonItem->pItemInfo->ShotNum > 1
//		&& i_pnMonster->m_nCurrentShotNumCount > 0)
//	{	// PrimaryItemŔÇ ShotNumŔĚ 1ş¸´Ů Ĺ©°í AttackCount°ˇ 0ş¸´Ů Ĺ©¸é ż¬»ç¸¦ ˝ĂŔŰÇŃ °ćżě ŔĚąÇ·Î ąß»ç
//
//		///////////////////////////////////////////////////////////////////////
//		// MultiNum Ăł¸®
//		BYTE byExplosionPos = 128 - pMonItem->pItemInfo->MultiNum/2;		
//		for(int i = 0; i < pMonItem->pItemInfo->MultiNum; i++)
//		{
//			INIT_MSG(MSG_FN_BATTLE_ATTACK_SECONDARY, T_FN_BATTLE_ATTACK_SECONDARY, pSendBattleAttackSec, pSendBuf);
//			pSendBattleAttackSec->ChannelIndex		= m_MapChannelIndex.ChannelIndex;
//			pSendBattleAttackSec->AttackIndex		= i_pnMonster->MonsterIndex;
//			pSendBattleAttackSec->TargetIndex		= i_pClientInfo->ClientIndex;
//			pSendBattleAttackSec->WeaponIndex		= m_uiMissileUniqueIndex++;
//			pSendBattleAttackSec->WeaponItemNumber	= pMonItem->pItemInfo->ItemNum;
//			pSendBattleAttackSec->TargetPosition	= i_pClientInfo->PositionVector;
//			pSendBattleAttackSec->Distance			= byExplosionPos;
//			pSendBattleAttackSec->SecAttackType		= 0;
//			Send2FieldServerW(pSendBuf, MSG_SIZE(MSG_FN_BATTLE_ATTACK_SECONDARY));
//
//			if(0 == pMonItem->pItemInfo->MultiNum%2
//				&& 127 == byExplosionPos)
//			{
//				byExplosionPos = 129;
//			}
//			else
//			{
//				byExplosionPos++;
//			}
//		}
//
//		///////////////////////////////////////////////////////////////////////
//		// Monster°ˇ °ř°ÝÇŃ ¸¶Áö¸· ˝Ă°Ł
//		i_pnMonster->m_dwTimeMonsterLastAttack = i_pnMonster->m_dwCurrentTick;
//
//		///////////////////////////////////////////////////////////////////////
//		// ShotNumŔ» Áő°ˇÇŃ´Ů.
//		i_pnMonster->m_nCurrentShotNumCount++;
//
//		///////////////////////////////////////////////////////////////////////
//		// ShotNum ¸¸Ĺ­ ąß»ç°ˇ µÇľúŔ¸¸é ShotNum Count¸¦ ĂĘ±âČ­ ÇŃ´Ů.	
//		if(pMonItem->pItemInfo->ShotNum <= i_pnMonster->m_nCurrentShotNumCount)
//		{
//			i_pnMonster->m_nCurrentShotNumCount	= 0;
//			i_pnMonster->m_ArrLastReattackTime[pMonItem->byArrayIndex]		= i_pnMonster->m_dwCurrentTick;
//			
//			tmBodyCon = i_pnMonster->BodyCondition;
//			CLEAR_BODYCON_BIT(tmBodyCon, BODYCON_MON_ATTACKALL_MASK);
//			i_pnMonster->ChangeBodyCondition(&tmBodyCon);
//		}
//	}// if_end
//	else if(i_pnMonster->m_dwCurrentTick - i_pnMonster->m_ArrLastReattackTime[pMonItem->byArrayIndex] > pMonItem->pItemInfo->ReAttacktime)
//	{	//
//
//		fDisFromAttackPosToTargetPos = D3DXVec3Length(&(i_pClientInfo->PositionVector - i_pnMonster->PositionVector));	// Ĺ¬¶óŔĚľđĆ®żÍ ¸ó˝şĹÍŔÇ °Ĺ¸®¸¦ ±¸ÇŃ´Ů.
//		if(fDisFromAttackPosToTargetPos < fMaxItemRange)
//		{	// Primary ľĆŔĚĹŰŔÇ °ř°Ý ŔŻČż °Ĺ¸®żˇ ŔÖŔ˝
//
//			D3DXVec3Normalize(&TempVector3, &(i_pClientInfo->PositionVector - i_pnMonster->PositionVector));		// ÇöŔç ¸ó˝şĹÍżˇĽ­ Ĺ¬¶óŔĚľđĆ®¸¦ ÇâÇĎ´Â Target Vector¸¦ ±¸ÇŃ´Ů.
//			fPinPoint = ACOS(D3DXVec3Dot(&i_pnMonster->TargetVector, &TempVector3));								// ¸ó˝şĹÍżˇĽ­ Target VectorżÍ Ŕ§żˇĽ­ ±¸ÇŃ ÇöŔç Target Vector »çŔĚŔÇ °˘Ŕ» ±¸ÇŃ´Ů
//			if(fPinPoint <= pMonItem->pItemInfo->RangeAngle
//				&& FALSE == CheckImpactStraightLineMapAndObjects(&i_pnMonster->PositionVector, &i_pClientInfo->PositionVector, FALSE))
//			{	// ľĆŔĚĹŰŔÇ °ř°Ý ŔŻČż°˘ŔĚ¸ç Ĺ¸°Ů±îÁö Á÷Ľ±Ŕ¸·Î ¸ĘŔÇ ĂćµąŔĚ ľřŔ˝
//
//				if(pMonItem->pItemInfo->AttackTime == 0
//					|| (COMPARE_BODYCON_BIT(i_pnMonster->BodyCondition, BODYCON_MON_PREATTACK1_MASK<<(pMonItem->byArrayIndex*COUNT_MONSTER_ATTACK_BODYCON))
//					&& i_pnMonster->m_dwCurrentTick - i_pnMonster->m_dwLastAttackTime > pMonItem->pItemInfo->AttackTime)
//					)
//				{
//
//					tmBodyCon = i_pnMonster->BodyCondition;
//					CLEAR_BODYCON_BIT(tmBodyCon, BODYCON_MON_ATTACKALL_MASK);
//					SET_BODYCON_BIT(tmBodyCon, BODYCON_MON_FIREATTACK1_MASK<<(pMonItem->byArrayIndex*COUNT_MONSTER_ATTACK_BODYCON));
//					i_pnMonster->ChangeBodyCondition(&tmBodyCon);
//					
//					INIT_MSG(MSG_FN_MONSTER_CHANGE_BODYCONDITION, T_FN_MONSTER_CHANGE_BODYCONDITION, pSeBody, pSendBuf);
//					pSeBody->ChannelIndex	= this->m_MapChannelIndex.ChannelIndex;
//					pSeBody->ClientIndex	= i_pnMonster->MonsterIndex;
//					pSeBody->BodyCondition	= i_pnMonster->BodyCondition;
//					Send2FieldServerW(pSendBuf, MSG_SIZE(MSG_FN_MONSTER_CHANGE_BODYCONDITION));
//
//					if(FALSE == NPCMonsterAttackSkill(i_pnMonster, pMonItem->pItemInfo))
//					{
//						///////////////////////////////////////////////////////////////////////////////
//						// ¸ÖĆĽłŃ Ăł¸®
//						BYTE byExplosionPos = 128 - pMonItem->pItemInfo->MultiNum/2;
//						for(int i = 0; i < pMonItem->pItemInfo->MultiNum; i++)
//						{
//							INIT_MSG(MSG_FN_BATTLE_ATTACK_SECONDARY, T_FN_BATTLE_ATTACK_SECONDARY, pSendBattleAttackSec, pSendBuf);
//							pSendBattleAttackSec->ChannelIndex		= m_MapChannelIndex.ChannelIndex;
//							pSendBattleAttackSec->AttackIndex		= i_pnMonster->MonsterIndex;
//							pSendBattleAttackSec->TargetIndex		= i_pClientInfo->ClientIndex;
//							pSendBattleAttackSec->WeaponIndex		= m_uiMissileUniqueIndex++;
//							pSendBattleAttackSec->WeaponItemNumber	= pMonItem->pItemInfo->ItemNum;
//							pSendBattleAttackSec->TargetPosition	= i_pClientInfo->PositionVector;
//							pSendBattleAttackSec->Distance			= byExplosionPos;
//							pSendBattleAttackSec->SecAttackType		= 0;
//							Send2FieldServerW(pSendBuf, MSG_SIZE(MSG_FN_BATTLE_ATTACK_SECONDARY));
//							
//							if(0 == pMonItem->pItemInfo->MultiNum%2
//								&& 127 == byExplosionPos)
//							{
//								byExplosionPos = 129;
//							}
//							else
//							{
//								byExplosionPos++;
//							}
//						}
//					}
//
//					///////////////////////////////////////////////////////////////////////
//					// Monster°ˇ °ř°ÝÇŃ ¸¶Áö¸· ˝Ă°Ł
//					i_pnMonster->m_dwTimeMonsterLastAttack = i_pnMonster->m_dwCurrentTick;
//
//					///////////////////////////////////////////////////////////////////////////////
//					// ShotNumŔÇ ˝ĂŔŰŔ» łŞĹ¸ł˝´Ů, ShotNum == 1 ŔĚľîµµ »ę°üľřŔ˝
//					i_pnMonster->m_nCurrentShotNumCount = 1;
//
//					///////////////////////////////////////////////////////////////////////////////
//					// ReAttackTimeŔ» UpdateÇŃ´Ů.
//					i_pnMonster->m_ArrLastReattackTime[pMonItem->byArrayIndex] = i_pnMonster->m_dwCurrentTick;
//
//					///////////////////////////////////////////////////////////////////////////////
//					// ¸öĹë °ř°Ý ¸ó˝şĹÍ°ˇ °ř°Ý ČÄ Ăł¸®
//					if(pMonItem->pItemInfo->OrbitType == ORBIT_BODYSLAM)
//					{						
//						TempVector3.y = 0.0f;
//						D3DXVec3Normalize(&TempVector3, &TempVector3);
//						GNPCRotateTargetVectorHorizontal(&TempVector3, &TempVector3, i_pnMonster->m_MoveInfo.LRDirect*(MONSTER_MAX_QUICK_TURN_ANGLE - i_pnMonster->MonsterInfoPtr->QuickTurnAngle), i_pnMonster->MonsterInfoPtr->QuickTurnAngle);
//						i_pnMonster->SetEnforceTargetVector(&TempVector3, i_pnMonster->GetSpeed(), MSS_QUICK_TURN_GENERAL);
//						
//						if(MSS_QUICK_TURN_GENERAL == i_pnMonster->m_enMoveState)
//						{
//							tmBodyCon = i_pnMonster->BodyCondition;
//							SET_BODYCON_BIT(tmBodyCon, BODYCON_BOOSTER3_MASK);
//							i_pnMonster->ChangeBodyCondition(&tmBodyCon);
//							
//							INIT_MSG(MSG_FN_MONSTER_CHANGE_BODYCONDITION, T_FN_MONSTER_CHANGE_BODYCONDITION, pSeBody, pSendBuf);
//							pSeBody->ChannelIndex	= this->m_MapChannelIndex.ChannelIndex;
//							pSeBody->ClientIndex	= i_pnMonster->MonsterIndex;
//							pSeBody->BodyCondition	= i_pnMonster->BodyCondition;
//							Send2FieldServerW(pSendBuf, MSG_SIZE(MSG_FN_MONSTER_CHANGE_BODYCONDITION));
//						}
//					}
//				}
//				else if(FALSE == COMPARE_BODYCON_BIT(i_pnMonster->BodyCondition, BODYCON_MON_PREATTACK1_MASK<<(pMonItem->byArrayIndex*COUNT_MONSTER_ATTACK_BODYCON)))
//				{
//					i_pnMonster->m_dwLastAttackTime = i_pnMonster->m_dwCurrentTick;
//					
//					tmBodyCon = i_pnMonster->BodyCondition;
//					CLEAR_BODYCON_BIT(tmBodyCon, BODYCON_MON_ATTACKALL_MASK);
//					SET_BODYCON_BIT(tmBodyCon, BODYCON_MON_PREATTACK1_MASK<<(pMonItem->byArrayIndex*COUNT_MONSTER_ATTACK_BODYCON));
//					i_pnMonster->ChangeBodyCondition(&tmBodyCon);
//					
//					INIT_MSG(MSG_FN_MONSTER_CHANGE_BODYCONDITION, T_FN_MONSTER_CHANGE_BODYCONDITION, pSeBody, pSendBuf);
//					pSeBody->ChannelIndex	= this->m_MapChannelIndex.ChannelIndex;
//					pSeBody->ClientIndex	= i_pnMonster->MonsterIndex;
//					pSeBody->BodyCondition	= i_pnMonster->BodyCondition;
//					Send2FieldServerW(pSendBuf, MSG_SIZE(MSG_FN_MONSTER_CHANGE_BODYCONDITION));
//				}
//			}
//			else if(COMPARE_BODYCON_BIT(i_pnMonster->BodyCondition, BODYCON_MON_PREATTACK1_MASK<<(pMonItem->byArrayIndex*COUNT_MONSTER_ATTACK_BODYCON))
//				&& i_pnMonster->m_dwCurrentTick - i_pnMonster->m_dwLastAttackTime > pMonItem->pItemInfo->AttackTime)
//			{
//				tmBodyCon = i_pnMonster->BodyCondition;
//				CLEAR_BODYCON_BIT(tmBodyCon, BODYCON_MON_ATTACKALL_MASK);
//				i_pnMonster->ChangeBodyCondition(&tmBodyCon);
//			}
//			else if(COMPARE_BODYCON_BIT(i_pnMonster->BodyCondition, BODYCON_MON_FIREATTACK1_MASK<<(pMonItem->byArrayIndex*COUNT_MONSTER_ATTACK_BODYCON)))
//			{	// Attack BodyConditionŔ» ĂĘ±âČ­, AngleŔĚ ľČµÇ°ĹłŞ Á÷Ľ±°Ĺ¸®żˇ ĂćµąŔĚ ŔÖ´Ů
//				
//				tmBodyCon = i_pnMonster->BodyCondition;
//				CLEAR_BODYCON_BIT(tmBodyCon, BODYCON_MON_ATTACKALL_MASK);
//				i_pnMonster->ChangeBodyCondition(&tmBodyCon);
//			}
//		}// if_end
//		else
//		{	// Attack BodyConditionŔ» ĂĘ±âČ­, °Ĺ¸®°ˇ ąţľîłµ´Ů.
//
//			if(COMPARE_BODYCON_BIT(i_pnMonster->BodyCondition, BODYCON_MON_ATTACK1_MASK<<(pMonItem->byArrayIndex*COUNT_MONSTER_ATTACK_BODYCON)) )
//			{
//				tmBodyCon = i_pnMonster->BodyCondition;
//				CLEAR_BODYCON_BIT(tmBodyCon, BODYCON_MON_ATTACKALL_MASK);
//				i_pnMonster->ChangeBodyCondition(&tmBodyCon);
//			}
//		}
//	}// elseif_end
//	else
//	{	// 
//		
//	}
//}

void CNPCMapChannel::AttackMonster2Monster(CNPCMonster *i_pnMonster, BYTE *pSendBuf)
{
	////////////////////////////////////////////////////////////////////////////////
	// 2009-09-09 ~ 2010 by dhjin, ŔÎÇÇ´ĎĆĽ - ąŘ°ú °°ŔĚ ¸ó˝şĹÍ °ř°Ý ˝Ă˝şĹŰ ±¸Çö!!!!
//	ASSERT_NEVER_GET_HERE();

	CNPCMonster * pTargetMonster = GetNPCMonster(i_pnMonster->m_nTargetIndex);
	if(NULL == pTargetMonster || pTargetMonster->m_enMonsterState != MS_PLAYING)
	{	// Ĺ¸°ŮŔĚ  ŔŻČżÇĎÁö ľĘŔ˝
		// ŔŻČżÇĎÁö ľĘŔ˝ Ĺ¸°ŮŔ» »čÁ¦ÇĎ°í »ő·Îżî Ĺ¸°ŮŔ¸·Î ĽłÁ¤ÇŃ´Ů.
		// LastAttackTimeŔş ĂĘ±âČ­ ÇĎÁö ľĘ´Â´Ů, 
		
		i_pnMonster->DeleteAttackedInfowithIndex();
		return;
	}
	
	if(NULL == i_pnMonster->m_pUsingMonsterItem
		// 2009-09-09 ~ 2010 by dhjin, ŔÎÇÇ´ĎĆĽ - ÄÁĆ®·Ń˝şĹł·Î ŔĚ¸§ şŻ°ć
		//		|| COMPARE_BODYCON_BIT(i_pnMonster->BodyCondition, BODYCON_MON_ATTACK6_MASK))
		|| COMPARE_BODYCON_BIT(i_pnMonster->BodyCondition, BODYCON_MON_CONTROLSKILL_MASK))
	{
		return;
	}
	
	if(FALSE == COMPARE_BODYCON_BIT(i_pnMonster->BodyCondition, BODYCON_MON_ATTACKALL_MASK)
		&& FALSE == CheckMonsterSelectedItem(i_pnMonster))
	{
		i_pnMonster->SelectUsingMonsterItem();
		return;
	}
	
//	this->AttackMonster2Character(i_pnMonster, pTargetMonster, pSendBuf);

	BodyCond_t			tmBodyCon;
	MONSTER_ITEM		*pMonItem = i_pnMonster->m_pUsingMonsterItem;
	if(COMPARE_BODYCON_BIT(i_pnMonster->BodyCondition, BODYCON_MON_FIREATTACK_ALL_MASK))
	{
		if(1 == pMonItem->pItemInfo->ShotNum
			|| i_pnMonster->m_nCurrentShotNumCount >= pMonItem->pItemInfo->ShotNum)
		{
			if(i_pnMonster->m_dwCurrentTick - i_pnMonster->m_ArrLastReattackTime[pMonItem->byArrayIndex] >= pMonItem->pItemInfo->ReAttacktime)
			{				
				i_pnMonster->ResetAttackBodyCondition();
				i_pnMonster->SelectUsingMonsterItem();
			}
			return;
		}
	}
	
	BOOL bIsPrimaryWeapon = IS_PRIMARY_WEAPON_MONSTER(i_pnMonster->m_pUsingMonsterItem->pItemInfo->Kind)?TRUE:FALSE;

	if(i_pnMonster->m_nCurrentShotNumCount > 0)
	{
		////////////////////////////////////////////////////////////////////////////////
		// 2009-09-09 ~ 2010 by dhjin, ŔÎÇÇ´ĎĆĽ - ąŘ°ú °°ŔĚ ĽöÁ¤, ShotNumş¸´Ů ¸ąŔĚ °ř°Ý ÇŇ Ľö ľř°Ô ĽöÁ¤
		if(i_pnMonster->m_nCurrentShotNumCount > pMonItem->pItemInfo->ShotNum) {
			i_pnMonster->ResetAttackBodyCondition();
			i_pnMonster->SelectUsingMonsterItem();
			return;
		}

		this->SendAttack2MonFieldServer(i_pnMonster, pTargetMonster, pSendBuf);

		i_pnMonster->m_nCurrentShotNumCount++;									// ShotNumŔ» Áő°ˇÇŃ´Ů.
		i_pnMonster->m_dwTimeMonsterLastAttack	= i_pnMonster->m_dwCurrentTick;	// Monster°ˇ °ř°ÝÇŃ ¸¶Áö¸· ˝Ă°Ł
	}
	else if(i_pnMonster->m_dwCurrentTick - i_pnMonster->m_ArrLastReattackTime[pMonItem->byArrayIndex] > pMonItem->pItemInfo->ReAttacktime)
	{
		float		fMaxItemRange = pMonItem->pItemInfo->Range;				
		if(FALSE == i_pnMonster->m_MoveInfo.MovableFlag
			|| EVENT_TYPE_NO_OBJECT_MONSTER != i_pnMonster->m_byObjectMonsterType
			|| FORM_GROUND_MOVE == i_pnMonster->CurrentMonsterForm)
		{// Á¤ÁöÇü, żŔşęÁ§Ć®, Áö»ó ¸ó˝şĹÍ

			fMaxItemRange *= 1.5f;
		}
		else if(0 != pMonItem->pItemInfo->AttackTime 
// 2009-09-09 ~ 2010 by dhjin, ŔÎÇÇ´ĎĆĽ - 6~15, ÄÁĆ®·Ń˝şĹł±îÁö Ăß°ˇ·Î ąŮµđÄÁµđĽÇ°Ş ľňľîżŔ´Â şÎşĐ ĽöÁ¤
//			&& COMPARE_BODYCON_BIT(i_pnMonster->BodyCondition, BODYCON_MON_PREATTACK1_MASK<<(pMonItem->byBodyConArrayIndex*COUNT_MONSTER_ATTACK_BODYCON)))
			&& COMPARE_BODYCON_BIT(i_pnMonster->BodyCondition, this->GetPreAttackBodyCondMask(pMonItem->byBodyConArrayIndex)))
		{// AttackTimeŔĚ 0ŔĚ ľĆ´Ď¸éĽ­ AttackÁŘşń°ˇ żĎ·áµČ ¸ó˝şĹÍ

			fMaxItemRange *= 0.8f;
		}
		
		// start 2011-03-22 by hskim, ŔÎÇÇ´ĎĆĽ 3Â÷ - ¸ó˝şĹÍ ¸ÖĆĽ Ĺ¸°ŮĆĂ ±â´É Ăß°ˇ
		//////////
		// ±âÁ¸ //
		// float fDisFromAttackPosToTargetPos = D3DXVec3Length(&(pTargetMonster->PositionVector - i_pnMonster->PositionVector));	// ¸ó˝şĹÍżÍ ¸ó˝şĹÍŔÇ °Ĺ¸®¸¦ ±¸ÇŃ´Ů.

		//////////////////////
		// ´ŮŔ˝°ú °°ŔĚ ĽöÁ¤ //
		int fMonsterMultiTargetIndex = 0;
		D3DXVECTOR3 fMonsterMultiTargetVector = pTargetMonster->GetNearMultiTarget(i_pnMonster->PositionVector, &fMonsterMultiTargetIndex);

		float fDisFromAttackPosToTargetPos = D3DXVec3Length(&(fMonsterMultiTargetVector - i_pnMonster->PositionVector));	// ¸ó˝şĹÍżÍ ¸ó˝şĹÍŔÇ °Ĺ¸®¸¦ ±¸ÇŃ´Ů.
		// end 2011-03-22 by hskim, ŔÎÇÇ´ĎĆĽ 3Â÷ - ¸ó˝şĹÍ ¸ÖĆĽ Ĺ¸°ŮĆĂ ±â´É Ăß°ˇ

		BOOL bNoCheck = FALSE;
		if(pMonItem->pItemInfo->AttackTime
// 2009-09-09 ~ 2010 by dhjin, ŔÎÇÇ´ĎĆĽ - 6~15, ÄÁĆ®·Ń˝şĹł±îÁö Ăß°ˇ·Î ąŮµđÄÁµđĽÇ°Ş ľňľîżŔ´Â şÎşĐ ĽöÁ¤
//			&& COMPARE_BODYCON_BIT(i_pnMonster->BodyCondition, BODYCON_MON_PREATTACK1_MASK<<(pMonItem->byBodyConArrayIndex*COUNT_MONSTER_ATTACK_BODYCON)))
			&& COMPARE_BODYCON_BIT(i_pnMonster->BodyCondition, this->GetPreAttackBodyCondMask(pMonItem->byBodyConArrayIndex)))
		{
			bNoCheck = TRUE;
		}
		if(bNoCheck
			|| fDisFromAttackPosToTargetPos <= fMaxItemRange)
		{
			D3DXVECTOR3		tmUnitVec3M2C;

			// start 2011-03-22 by hskim, ŔÎÇÇ´ĎĆĽ 3Â÷ - ¸ó˝şĹÍ ¸ÖĆĽ Ĺ¸°ŮĆĂ ±â´É Ăß°ˇ
			//////////
			// ±âÁ¸ //
			// D3DXVec3Normalize(&tmUnitVec3M2C, &(pTargetMonster->PositionVector - i_pnMonster->PositionVector));		// ÇöŔç ¸ó˝şĹÍżˇĽ­ ¸ó˝şĹÍ¸¦ ÇâÇĎ´Â Target Vector¸¦ ±¸ÇŃ´Ů.

			//////////////////////
			// ´ŮŔ˝°ú °°ŔĚ ĽöÁ¤ //
			D3DXVec3Normalize(&tmUnitVec3M2C, &(fMonsterMultiTargetVector - i_pnMonster->PositionVector));		// ÇöŔç ¸ó˝şĹÍżˇĽ­ ¸ó˝şĹÍ¸¦ ÇâÇĎ´Â Target Vector¸¦ ±¸ÇŃ´Ů.
			// end 2011-03-22 by hskim, ŔÎÇÇ´ĎĆĽ 3Â÷ - ¸ó˝şĹÍ ¸ÖĆĽ Ĺ¸°ŮĆĂ ±â´É Ăß°ˇ

			float fPinPoint = ACOS(D3DXVec3Dot(&i_pnMonster->TargetVector, &tmUnitVec3M2C));						// ¸ó˝şĹÍżˇĽ­ Target VectorżÍ Ŕ§żˇĽ­ ±¸ÇŃ ÇöŔç Target Vector »çŔĚŔÇ °˘Ŕ» ±¸ÇŃ´Ů

			// start 2011-03-22 by hskim, ŔÎÇÇ´ĎĆĽ 3Â÷ - ¸ó˝şĹÍ ¸ÖĆĽ Ĺ¸°ŮĆĂ ±â´É Ăß°ˇ
			//////////
			// ±âÁ¸ //
			//	if(bNoCheck
			//	||( fPinPoint <= pMonItem->pItemInfo->RangeAngle && FALSE == CheckImpactStraightLineMapAndObjects(&i_pnMonster->PositionVector, &pTargetMonster->PositionVector, DEFAULT_OBJECT_MONSTER_OBJECT+i_pnMonster->MonsterInfoPtr->MonsterUnitKind) )
			////	||( FALSE == CheckImpactStraightLineMapAndObjects(&i_pnMonster->PositionVector, &pTargetMonster->PositionVector, DEFAULT_OBJECT_MONSTER_OBJECT+i_pnMonster->MonsterInfoPtr->MonsterUnitKind) )
			//	)

			//////////////////////
			// ´ŮŔ˝°ú °°ŔĚ ĽöÁ¤ //
				if(bNoCheck
				||( fPinPoint <= pMonItem->pItemInfo->RangeAngle && FALSE == CheckImpactStraightLineMapAndObjects(&i_pnMonster->PositionVector, &fMonsterMultiTargetVector, DEFAULT_OBJECT_MONSTER_OBJECT+i_pnMonster->MonsterInfoPtr->MonsterUnitKind) ) )
			// end 2011-03-22 by hskim, ŔÎÇÇ´ĎĆĽ 3Â÷ - ¸ó˝şĹÍ ¸ÖĆĽ Ĺ¸°ŮĆĂ ±â´É Ăß°ˇ
			{
				if(pMonItem->pItemInfo->AttackTime == 0
// 2009-09-09 ~ 2010 by dhjin, ŔÎÇÇ´ĎĆĽ - 6~15, ÄÁĆ®·Ń˝şĹł±îÁö Ăß°ˇ·Î ąŮµđÄÁµđĽÇ°Ş ľňľîżŔ´Â şÎşĐ ĽöÁ¤
//					|| (COMPARE_BODYCON_BIT(i_pnMonster->BodyCondition, BODYCON_MON_PREATTACK1_MASK<<(pMonItem->byBodyConArrayIndex*COUNT_MONSTER_ATTACK_BODYCON)) && i_pnMonster->m_dwCurrentTick - i_pnMonster->m_dwLastAttackTime > pMonItem->pItemInfo->AttackTime)
					|| COMPARE_BODYCON_BIT(i_pnMonster->BodyCondition, this->GetPreAttackBodyCondMask(pMonItem->byBodyConArrayIndex))
					)
				{
					tmBodyCon = i_pnMonster->BodyCondition;
					CLEAR_BODYCON_BIT(tmBodyCon, BODYCON_MON_ATTACKALL_MASK);
// 2009-09-09 ~ 2010 by dhjin, ŔÎÇÇ´ĎĆĽ - 6~15, ÄÁĆ®·Ń˝şĹł±îÁö Ăß°ˇ·Î ąŮµđÄÁµđĽÇ°Ş ľňľîżŔ´Â şÎşĐ ĽöÁ¤
//					SET_BODYCON_BIT(tmBodyCon, BODYCON_MON_FIREATTACK1_MASK<<(pMonItem->byBodyConArrayIndex*COUNT_MONSTER_ATTACK_BODYCON));
					SET_BODYCON_BIT(tmBodyCon, this->GetFireAttackBodyCondMask(pMonItem->byBodyConArrayIndex));
					i_pnMonster->ChangeBodyCondition(&tmBodyCon);
					
					INIT_MSG(MSG_FN_MONSTER_CHANGE_BODYCONDITION, T_FN_MONSTER_CHANGE_BODYCONDITION, pSeBody, pSendBuf);
					pSeBody->ChannelIndex	= this->m_MapChannelIndex.ChannelIndex;
					pSeBody->ClientIndex	= i_pnMonster->MonsterIndex;
					pSeBody->BodyCondition	= i_pnMonster->BodyCondition;
					Send2FieldServerW(pSendBuf, MSG_SIZE(MSG_FN_MONSTER_CHANGE_BODYCONDITION));

					if(FALSE == NPCMonsterAttackSkill(i_pnMonster, pMonItem->pItemInfo))
					{
						this->SendAttack2MonFieldServer(i_pnMonster, pTargetMonster, pSendBuf);
					}
					
					i_pnMonster->m_dwTimeMonsterLastAttack	= i_pnMonster->m_dwCurrentTick;	// Monster°ˇ °ř°ÝÇŃ ¸¶Áö¸· ˝Ă°Ł
					i_pnMonster->m_nCurrentShotNumCount		= 1;							// ShotNumŔÇ ˝ĂŔŰŔ» łŞĹ¸ł˝´Ů, ShotNum == 1 ŔĚľîµµ »ę°üľřŔ˝
					i_pnMonster->m_ArrLastReattackTime[pMonItem->byArrayIndex] = i_pnMonster->m_dwCurrentTick;	// ReAttackTimeŔ» UpdateÇŃ´Ů.
					
					///////////////////////////////////////////////////////////////////////////////
					// ¸öĹë °ř°Ý ¸ó˝şĹÍ°ˇ °ř°Ý ČÄ Ăł¸®
					if(pMonItem->pItemInfo->OrbitType == ORBIT_BODYSLAM)
					{
						tmUnitVec3M2C.y = 0.0f;
						D3DXVec3Normalize(&tmUnitVec3M2C, &tmUnitVec3M2C);
						GNPCRotateTargetVectorHorizontal(&tmUnitVec3M2C, &tmUnitVec3M2C, i_pnMonster->m_MoveInfo.LRDirect*(MONSTER_MAX_QUICK_TURN_ANGLE - i_pnMonster->MonsterInfoPtr->QuickTurnAngle), i_pnMonster->MonsterInfoPtr->QuickTurnAngle);
						i_pnMonster->SetEnforceTargetVector(&tmUnitVec3M2C, i_pnMonster->GetSpeed(), MSS_QUICK_TURN_GENERAL);
						
						if(MSS_QUICK_TURN_GENERAL == i_pnMonster->m_enMoveState)
						{
							tmBodyCon = i_pnMonster->BodyCondition;
							SET_BODYCON_BIT(tmBodyCon, BODYCON_BOOSTER3_MASK);
							i_pnMonster->ChangeBodyCondition(&tmBodyCon);
							
							INIT_MSG(MSG_FN_MONSTER_CHANGE_BODYCONDITION, T_FN_MONSTER_CHANGE_BODYCONDITION, pSeBody, pSendBuf);
							pSeBody->ChannelIndex	= this->m_MapChannelIndex.ChannelIndex;
							pSeBody->ClientIndex	= i_pnMonster->MonsterIndex;
							pSeBody->BodyCondition	= i_pnMonster->BodyCondition;
							Send2FieldServerW(pSendBuf, MSG_SIZE(MSG_FN_MONSTER_CHANGE_BODYCONDITION));
						}
					}
				}
// 2009-09-09 ~ 2010 by dhjin, ŔÎÇÇ´ĎĆĽ - 6~15, ÄÁĆ®·Ń˝şĹł±îÁö Ăß°ˇ·Î ąŮµđÄÁµđĽÇ°Ş ľňľîżŔ´Â şÎşĐ ĽöÁ¤
//				else if(FALSE == COMPARE_BODYCON_BIT(i_pnMonster->BodyCondition, BODYCON_MON_PREATTACK1_MASK<<(pMonItem->byBodyConArrayIndex*COUNT_MONSTER_ATTACK_BODYCON)))
				else if(FALSE == COMPARE_BODYCON_BIT(i_pnMonster->BodyCondition, this->GetPreAttackBodyCondMask(pMonItem->byBodyConArrayIndex)))
				{
					i_pnMonster->m_dwLastAttackTime = i_pnMonster->m_dwCurrentTick;
					
					tmBodyCon = i_pnMonster->BodyCondition;
					CLEAR_BODYCON_BIT(tmBodyCon, BODYCON_MON_ATTACKALL_MASK);
// 2009-09-09 ~ 2010 by dhjin, ŔÎÇÇ´ĎĆĽ - 6~15, ÄÁĆ®·Ń˝şĹł±îÁö Ăß°ˇ·Î ąŮµđÄÁµđĽÇ°Ş ľňľîżŔ´Â şÎşĐ ĽöÁ¤
//					SET_BODYCON_BIT(tmBodyCon, BODYCON_MON_PREATTACK1_MASK<<(pMonItem->byBodyConArrayIndex*COUNT_MONSTER_ATTACK_BODYCON));
					SET_BODYCON_BIT(tmBodyCon, this->GetPreAttackBodyCondMask(pMonItem->byBodyConArrayIndex));
					i_pnMonster->ChangeBodyCondition(&tmBodyCon);
					
					INIT_MSG(MSG_FN_MONSTER_CHANGE_BODYCONDITION, T_FN_MONSTER_CHANGE_BODYCONDITION, pSeBody, pSendBuf);
					pSeBody->ChannelIndex	= this->m_MapChannelIndex.ChannelIndex;
					pSeBody->ClientIndex	= i_pnMonster->MonsterIndex;
					pSeBody->BodyCondition	= i_pnMonster->BodyCondition;
					Send2FieldServerW(pSendBuf, MSG_SIZE(MSG_FN_MONSTER_CHANGE_BODYCONDITION));
				}
			}
			else
			{
				i_pnMonster->ResetAttackBodyCondition();
			}
		}
		else
		{
			// 2007-01-15 by cmkwon, »ç°Ĺ¸® ¶§ą®żˇ °ř°ÝÇĎÁö ¸řÇĎ¸é ą«±â¸¦ ±łČŻÇĎµµ·Ď Ăł¸®ÇŃ´Ů
			i_pnMonster->SelectUsingMonsterItem();
			i_pnMonster->ResetAttackBodyCondition();
		}
	}



}

void CNPCMapChannel::UpdateMissilePosition2Character(CNPCMonster *i_pMonster, BYTE *pSendBuf)
{
//	MISSILE						*pMissile = NULL;
//	MSG_FN_BATTLE_ATTACK_FIND	*pSendBattleAttackFind = NULL;
//	float						fDistanceGap;
//	CLIENT_INFO					*ptmClientInfo = NULL;
//	D3DXVECTOR3					TempVector3;
//
//	list<MISSILE*>::iterator itrMissile = i_pMonster->m_mtlistShootedMissile.begin();
//	while(itrMissile != i_pMonster->m_mtlistShootedMissile.end())
//	{
//		pMissile = *itrMissile;
//		if(dwCurrentTick - pMissile->m_dwTimeCreated > 4000)
//		{	// ˝Ă°Ł Á¦ÇŃŔ¸·Î Ćřąß
//
//			INIT_MSG(MSG_FN_BATTLE_ATTACK_FIND, T_FN_BATTLE_ATTACK_FIND, pSendBattleAttackFind, pSendBuf);
//			pSendBattleAttackFind->ChannelIndex	= m_MapChannelIndex.ChannelIndex;
//			pSendBattleAttackFind->AttackIndex	= i_pMonster->MonsterIndex;
//			pSendBattleAttackFind->TargetIndex	= 0;
////			pSendBattleAttackFind->WeaponType	= 1;		// by kelovon, 20030811
//			pSendBattleAttackFind->WeaponIndex	= pMissile->WeaponIndex;
//			Send2FieldServerW(pSendBuf, MSG_SIZE(MSG_FN_BATTLE_ATTACK_FIND));
//
//			SAFE_DELETE(pMissile);
//			i_pMonster->m_mtlistShootedMissile.erase(itrMissile);
//			break;
//		}
//		else if(dwCurrentTick - pMissile->m_dwTimeLastMoved > MONSTER_UPDATE_MOVE_TERM_TICK)
//		{	// ŔĚµż
//
//			pMissile->m_dwTimeLastMoved = dwCurrentTick;
//			if(pMissile->m_TargetIndex < MONSTER_CLIENT_INDEX_START_NUM)
//			{
//				ptmClientInfo = GetClientInfo(pMissile->m_TargetIndex);
//				if(ptmClientInfo && ptmClientInfo->ClientState == CS_GAMESTARTED)
//				{
//					fDistanceGap = D3DXVec3Length(&(ptmClientInfo->PositionVector - pMissile->PositionVector));	// Ĺ¬¶óŔĚľđĆ®żÍ ¸ó˝şĹÍŔÇ °Ĺ¸®¸¦ ±¸ÇŃ´Ů.
//					if(fDistanceGap > pMissile->m_fSpeed)
//					{
//						pMissile->PositionVector = pMissile->PositionVector + pMissile->TargetVector * pMissile->m_fSpeed;
//					}
//					else
//					{
//						pMissile->PositionVector = pMissile->PositionVector + pMissile->TargetVector * fDistanceGap;
//					}
//
//					if(TRUE == CheckImpactPositionMapAndObjects(&pMissile->PositionVector))
//					{	// ¸Ę°úŔÇ Ăćµą·Î Ćřąß
//
//						INIT_MSG(MSG_FN_BATTLE_ATTACK_FIND, T_FN_BATTLE_ATTACK_FIND, pSendBattleAttackFind, pSendBuf);
//						pSendBattleAttackFind->ChannelIndex	= m_MapChannelIndex.ChannelIndex;
//						pSendBattleAttackFind->AttackIndex	= i_pMonster->MonsterIndex;
//						pSendBattleAttackFind->TargetIndex	= 0;
////						pSendBattleAttackFind->WeaponType	= 1;		// by kelovon, 20030811
//						pSendBattleAttackFind->WeaponIndex	= pMissile->WeaponIndex;
//						Send2FieldServerW(pSendBuf, MSG_SIZE(MSG_FN_BATTLE_ATTACK_FIND));
//
//						i_pMonster->m_mtlistShootedMissile.erase(itrMissile);
//						delete pMissile;
//						break;
//					}
//
//					fDistanceGap = D3DXVec3Length(&(ptmClientInfo->PositionVector - pMissile->PositionVector));	// Ĺ¬¶óŔĚľđĆ®żÍ ¸ó˝şĹÍŔÇ °Ĺ¸®¸¦ ±¸ÇŃ´Ů.
//					if (fDistanceGap < 5)
//					{
//						INIT_MSG(MSG_FN_BATTLE_ATTACK_FIND, T_FN_BATTLE_ATTACK_FIND, pSendBattleAttackFind, pSendBuf);
//						pSendBattleAttackFind->ChannelIndex	= m_MapChannelIndex.ChannelIndex;
//						pSendBattleAttackFind->AttackIndex	= i_pMonster->MonsterIndex;
//						pSendBattleAttackFind->TargetIndex	= pMissile->m_TargetIndex;
//						pSendBattleAttackFind->TargetPosition = ptmClientInfo->PositionVector;
////						pSendBattleAttackFind->WeaponType = 1;		// by kelovon, 20030811
//						pSendBattleAttackFind->WeaponIndex	= pMissile->WeaponIndex;
//						Send2FieldServerW(pSendBuf, MSG_SIZE(MSG_FN_BATTLE_ATTACK_FIND));
//
//						SAFE_DELETE(pMissile);
//						i_pMonster->m_mtlistShootedMissile.erase(itrMissile);
//						break;
//					}
//					else
//					{
//						if(dwCurrentTick - pMissile->m_dwTimeCreated < MONSTER_UPDATE_MOVE_TERM_TICK * 3)
//						{
//							pMissile->PositionVector += pMissile->TargetVector * pMissile->m_fSpeed;
//						}
//						else
//						{
//							D3DXVec3Normalize(&pMissile->TargetVector, &(ptmClientInfo->PositionVector - pMissile->PositionVector));
//							if(fDistanceGap > pMissile->m_fSpeed)
//							{
//								pMissile->PositionVector += pMissile->TargetVector * pMissile->m_fSpeed;
//							}
//							else
//							{
//								pMissile->PositionVector += pMissile->TargetVector * fDistanceGap;
//							}
//
//						}
//						if(pMissile->m_fSpeed < 16.0f)
//						{
//							pMissile->m_fSpeed += 1.0f;
//						}
//					}
//				}
//				else
//				{
//					pMissile->PositionVector = pMissile->PositionVector + pMissile->TargetVector * pMissile->m_fSpeed;
//					if(pMissile->m_fSpeed < 16.0f)
//					{
//						pMissile->m_fSpeed += 1.0f;
//					}
//				}
//			}
//		}
//		itrMissile++;
//	}
}

void CNPCMapChannel::DelelteMonsterHandler(CNPCMonster * i_pNMonster)
{
	if(i_pNMonster->m_enMonsterState == MS_NULL){			return;}

	if(FALSE == DeleteBlockPosition(i_pNMonster->PositionVector.x, i_pNMonster->PositionVector.z, i_pNMonster->MonsterIndex))
	{
		char szTemp[256];
		sprintf(szTemp, "[Error] DeleteBlockPosition_2 Error, MapChannel(%s) UnitIndex(%5d) XZ(%5.0f, %5.0f)\n"
			, GET_MAP_STRING(this->m_MapChannelIndex), i_pNMonster->MonsterIndex
			, i_pNMonster->PositionVector.x, i_pNMonster->PositionVector.z);
		DBGOUT(szTemp);
		g_pNPCGlobal->WriteSystemLog(szTemp);
	}
	
	m_mtvectorMonsterPtr.lock();
	if(FALSE == i_pNMonster->m_byObjectMonsterType)
	{// żŔşęÁ§Ć® ¸ó˝şĹÍ°ˇ ľĆ´Ô

		MONSTER_CREATE_REGION_INFO_EX	*pInfoEX 
			= GetMonsterCreateRegionInfoEXWidhIndex(i_pNMonster->m_dwIndexCreatedMonsterData);

		if(NULL != pInfoEX
			&& pInfoEX->nCurrentCount > 0)
		{
			pInfoEX->nCurrentCount--;
			pInfoEX->dwLastTimeMonsterCreate = timeGetTime();
		}
	}
	else
	{// żŔşęÁ§Ć® ¸ó˝şĹÍŔÓ
		
		OBJECTINFOSERVER *pObjMonsterInfo = this->FindObjectMonsterInfoByObjectEventIndex(i_pNMonster->m_dwIndexCreatedMonsterData);
		if(pObjMonsterInfo)
		{
			pObjMonsterInfo->m_EventInfo.m_dwLastTimeObjectMonsterCreated = timeGetTime();
			pObjMonsterInfo->m_EventInfo.m_byObjectMonsterCreated = FALSE;
		}
	}
	if(m_nCurMonsterCountInChannel > 0)
	{
		m_nCurMonsterCountInChannel--;
	}

	////////////////////////////////////////////////////////////////////////////////
	// 2009-09-09 ~ 2010-01-27 by dhjin, ŔÎÇÇ´ĎĆĽ - Success, Fail˝Ă ¸ó˝şĹÍ ĂĘ±âČ­, »ç¸Á˝Ăżˇ¸¸ ´ë»ç Ăâ·ÂÇĎµµ·Ď Ŕ§Äˇ şŻ°ć ÁÖĽ® Ăł¸®ÇÔ
	// 2009-09-09 ~ 2010 by dhjin, ŔÎÇÇ´ĎĆĽ - 
// 	if(i_pNMonster->m_HPAction.CheckValidSizeTalkDead()) {
// 		HPACTION_TALK_HPRATE MsgTalk;
// 		MEMSET_ZERO(&MsgTalk, sizeof(HPACTION_TALK_HPRATE));
// 		if(i_pNMonster->m_HPAction.GetTalkDead(&MsgTalk)) {
// 			// Á×Ŕ˝˝Ă ´ëČ­°ˇ ŔÖ´Ů¸é ŔüĽŰÇŃ´Ů.
// 			this->SendFSvrHPTalk(i_pNMonster, &MsgTalk);
// 		}
// 	}

	i_pNMonster->ResetMonster();
	m_vectorUsableMonsterIndex.push_back(i_pNMonster->MonsterIndex);
	m_mtvectorMonsterPtr.unlock();
}


BOOL CNPCMapChannel::CheckImpactFrontPositionMap(CNPCMonster * i_pNMon, D3DXVECTOR3 *o_pTarVector3)
{
	///////////////////////////////////////////////////////////////////////////////
	// - MoveState°ˇ (Object, ¸Ę)Ăćµą »óĹÂŔĚ¸é FALSE¸¦ ¸®ĹĎÇŃ´Ů.
	if(MSS_MAP_IMPACT == i_pNMon->m_enMoveState
		|| FALSE == i_pNMon->m_MoveInfo.MovableFlag)
	{

		return FALSE;
	}

	D3DXVECTOR3 tmVec3Front = this->GetFrontPosition(i_pNMon);
	if(FALSE == m_pNPCMapProject->IsValidPosition(&tmVec3Front))
	{
		// ¸ĘŔÇ Áß˝ÉŔ» ÇâÇĎ´Â Ĺ¸°Ů ş¤ĹÍ¸¦ ÇŇ´ç
		(*o_pTarVector3) = m_pNPCMapProject->GetTargetVectorForMapCenterPosition(&tmVec3Front);
		return TRUE;
	}

	///////////////////////////////////////////////////////////////////////////////
	// 1. (Frontş¤ĹÍŔÇ łôŔĚ + 20.0f)ş¸´Ů Frontş¤ĹÍżˇĽ­ŔÇ ¸ĘŔÇ łôŔĚ°ˇ łôŔ¸¸é ¸Ę°ú ĂćµąŔĚ ąß»ýÇŃ °ÍŔĚ´Ů
	float	fMapHeight = m_pNPCMapProject->GetMapHeightIncludeWater(&tmVec3Front);
#ifdef _DEBUG
//	if(tmVec3Front.y < fMapHeight)
//	{
//		DBGOUT("		¸Ę Ăćµą ĂĽĹ© łôŔĚ Â÷ŔĚ(%d)\n", (int)(fMapHeight - tmVec3Front.y));
//	}
#endif // _DEBUG_endif

	if(tmVec3Front.y + MONSTER_MAP_IMPACK_HEIGHT_GAP < fMapHeight)
	{	// ĂćµąŔĚ ąß»ýÇĎż´Ŕ¸ąÇ·Î Č¸ŔüÇŇ ş¤ĹÍ¸¦ ¸®ĹĎÇŘÁŘ´Ů.
							
		D3DXVECTOR3 tmVec3Nor = m_pNPCMapProject->GetNormalVectorWithMapTile(&tmVec3Front);
		CNPCMapProject::ChangePlaneUnitVec3(o_pTarVector3, &tmVec3Nor, &i_pNMon->m_MoveTargetVector);
		///////////////////////////////////////////////////////////////////////////////
		// Ĺ¸ŔĎŔÇ ąýĽ± ş¤ĹÍ¸¦ ±¸ÇĎ°í Ćň¸é ´ÜŔ§ ş¤ĹÍ·Î ¸¸µç´Ů
		// 1. Áö»ó ¸ó˝şĹÍ°ˇ ľĆ´Ď¸é ş¤ĹÍ¸¦ Á¶±Ý żĂ¸°´Ů.
		// 2. Áö»ó ¸ó˝şĹÍ´Â Ćň¸é ´ÜŔ§ ş¤ĹÍ¸¦ ¸®ĹĎ´Ü´Ů				
		if(FORM_GROUND_MOVE != i_pNMon->CurrentMonsterForm)
		{
			o_pTarVector3->y = RANDF2(0.20f, 0.50f);					// 0.1f ~ 0.3f ŔÇ °ŞŔ» °ˇÁř´Ů
			D3DXVec3Normalize(o_pTarVector3, o_pTarVector3);
		}
		return TRUE;
	}
	
	return FALSE;
}


BOOL CNPCMapChannel::CheckImpactStraightLineMapAndObjects(D3DXVECTOR3 *vMonPos
														  , D3DXVECTOR3 *vTarPos
														   , INT i_nExcludeObjNum
														   , BOOL bFlagObjectCheck/*=TRUE*/)
{
	D3DXVECTOR3	vVel, vPos;
	int			cont = 4;
	float		fLength;

	vVel = *vTarPos - *vMonPos;
	cont = (int)(D3DXVec3Length(&vVel)/SIZE_MAP_TILE_SIZE) + 1;
	fLength = D3DXVec3Length(&vVel)/cont;		// ÇŃąřżˇ ĂĽĹ©ÇŇ °Ĺ¸®¸¦ ±¸ÇŃ´Ů
	D3DXVec3Normalize(&vVel, &vVel);			// ąćÇâ ş¤ĹÍ¸¦ ±¸ÇŃ´Ů.
	int i=1;
	while(i < cont)
	{
		vPos = *vMonPos + i*fLength*vVel;
		if(FALSE == m_pNPCMapProject->IsValidPosition(&vPos)
			|| vPos.y < m_pNPCMapProject->GetMapHeightIncludeWater(&vPos))
		{// ŔŻČżÇŃ ÁÂÇĄ°ˇ ľĆ´Ď°ĹłŞ Č¤Ŕş ÁöÇü łôŔĚ ş¸´Ů ł·´Ů

			return TRUE;
		}
		i++;
	}
	if(bFlagObjectCheck
		&& TRUE == CheckImpactPositionObjects(vMonPos, vTarPos, i_nExcludeObjNum))
	{// żŔşęÁ§Ć®żÍŔÇ Ăćµą ąß»ýÇÔ
		
		return TRUE;
	}
	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
// ¸ó˝şĹÍ°ˇ Attack˝Ă ObjectżÍŔÇ ĂćµąŔ» ĂĽĹ©
BOOL CNPCMapChannel::CheckImpactPositionObjects(D3DXVECTOR3 *i_pVec3Start, D3DXVECTOR3 *i_pVec3End, INT i_nExcludeObjNum)
{
	if(FALSE == m_pNPCMapProject->IsValidPosition(i_pVec3End->x, i_pVec3End->z))
	{// ˝ĂŔŰÁˇŔş ŔŻČżĽş ĂĽĹ©°ˇ ÇĘżäľřŔ˝

		return TRUE;
	}

	D3DXVECTOR3 tmUnitVec3 = *i_pVec3End - *i_pVec3Start;
	float fLength = D3DXVec3Length(&tmUnitVec3);
	D3DXVec3Normalize(&tmUnitVec3, &tmUnitVec3);
	////////////////////////////////////////////////////////////////////////////////
	// 2009-09-09 ~ 2010 by dhjin, ŔÎÇÇ´ĎĆĽ - şŻ°ć żŔşęÁ§Ć®¸¦ Ŕ§ÇŘ!!!! ąŘ°ú °°ŔĚ ĽöÁ¤
//	D3DXVECTOR3 retedUnitVec3 = m_pNPCMapProject->CheckCollisionMesh(i_pVec3Start, &tmUnitVec3, fLength, i_nExcludeObjNum);
	D3DXVECTOR3 retedUnitVec3 = m_pNPCMapProject->CheckCollisionMesh(i_pVec3Start, &tmUnitVec3, fLength, i_nExcludeObjNum, &m_mtDeletedObjectInfoList, &m_mtNewObjectInfoList);	// 2009-09-09 ~ 2010-01 by dhjin, ŔÎÇÇ´ĎĆĽ - ĽŇ˝ş ĂĽĹ©
	if(retedUnitVec3 != D3DXVECTOR3(0,0,0)) 
	{
		return TRUE;
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// ¸ó˝şĹÍ°ˇ ŔĚµż˝Ă Object żÍŔÇ Ăćµą Ăł¸®
BOOL CNPCMapChannel::CheckAndModifyImpactPositionObjects(CNPCMonster *pMon)
{
	if(FALSE == pMon->m_MoveInfo.MovableFlag
		|| pMon->PositionVector == pMon->m_BeforePosition)
	{
		return FALSE;
	}

//	ĂćµąÇŃ ¸éŔÇ ąýĽ± ş¤ĹÍ¸¦ ąŢ´Â´Ů. 2004.07.03 jschoi
	D3DXVECTOR3 tmUnitVec3, retedUnitVec3;
	D3DXVec3Normalize(&tmUnitVec3, &(pMon->PositionVector-pMon->m_BeforePosition));
	////////////////////////////////////////////////////////////////////////////////
	// 2009-09-09 ~ 2010 by dhjin, ŔÎÇÇ´ĎĆĽ - şŻ°ć żŔşęÁ§Ć®¸¦ Ŕ§ÇŘ!!!! ąŘ°ú °°ŔĚ ĽöÁ¤
//	retedUnitVec3 = m_pNPCMapProject->CheckCollisionMesh(&pMon->m_BeforePosition, &tmUnitVec3
//		, pMon->MonsterInfoPtr->Size + pMon->GetCurrentSpeed(), DEFAULT_OBJECT_MONSTER_OBJECT+pMon->MonsterInfoPtr->MonsterUnitKind);
	retedUnitVec3 = m_pNPCMapProject->CheckCollisionMesh(&pMon->m_BeforePosition, &tmUnitVec3
		, pMon->MonsterInfoPtr->Size + pMon->GetCurrentSpeed(), DEFAULT_OBJECT_MONSTER_OBJECT+pMon->MonsterInfoPtr->MonsterUnitKind, &m_mtDeletedObjectInfoList, &m_mtNewObjectInfoList);	// 2009-09-09 ~ 2010-01 by dhjin, ŔÎÇÇ´ĎĆĽ - ĽŇ˝ş ĂĽĹ©
	if(retedUnitVec3 != D3DXVECTOR3(0,0,0)) 
	{
		// 2013-11-27 by jekim, 몬스터가 벽에 박히는 문제 수정 - 모든 경우에 오브젝트 안으로 들어가려는 상황이므로 아래 if문 주석 처리.
		///////////////////////////////////////////////////////////////////////////////
		// 충돌 면의 법선벡터와 몬스터이동 TargetVector의 각 차이를 구한다.		
		float fAngle = ACOS(D3DXVec3Dot(&tmUnitVec3, &retedUnitVec3));
		if(fAngle >= PI/4)
		{// 각차이가 PI/4보다 크다면 오브젝트 안으로 들어가려는 상황이므로 이전좌표로 돌리고 방향을 랜덤으로 설정한다.

		pMon->PositionVector = pMon->m_BeforePosition;
		CNPCMonster::GetRandomVector(&tmUnitVec3);
		pMon->SetMoveTargetVector(&tmUnitVec3);
		if (pMon->m_nTargetIndex == 0) {// 공격시가 아니면 강제 TargetVector를 설정한다

			pMon->SetEnforceTargetVector(&tmUnitVec3, pMon->GetSpeed(), MSS_MAP_IMPACT);
		}
//#ifdef S_IMSI_MONSTER_LOCKED_SHCHOI //non locking in walls
		pMon->PositionVector.z++; // 2014-12-16 by shchoi 오브젝트에 몬스터 끼일 경우 강제로 좌표 변경해주도록 수정
		pMon->PositionVector.y++; // 2014-12-16 by shchoi 오브젝트에 몬스터 끼일 경우 강제로 좌표 변경해주도록 수정
//#endif
		return TRUE;
		}
		// end 2013-11-27 by jekim, 몬스터가 벽에 박히는 문제 수정 - 모든 경우에 오브젝트 안으로 들어가려는 상황이므로 아래 if문 주석 처리.
	}
	return FALSE;
}


///////////////////////////////////////////////////////////////////////////////
// ÇÔĽöŔÚ Ĺ¬·ˇ˝ş
//	Äł¸ŻĹÍ°ˇ °Ĺ¸®ľČżˇ ŔÖ°í ŔŻČżÇĎ¸é ¸®ĹĎÇŃ´Ů.
//
///////////////////////////////////////////////////////////////////////////////
class find_if_functor_NPCCharacterExistInRange
{
public:
	find_if_functor_NPCCharacterExistInRange(CNPCMapChannel *i_pNMapChann, D3DXVECTOR3 *i_pPos, float i_fBoundary)
		:m_pNMapChannel(i_pNMapChann), m_pVec3Pos(i_pPos), m_fBoundaryDistance(i_fBoundary)
	{
	};
	bool operator()(ClientIndex_t index)
	{
		CLIENT_INFO		*pClient = m_pNMapChannel->GetClientInfo(index);
		if(pClient
			&& pClient->ClientState != CS_NULL)
		{	// Ĺ¬¶óŔĚľđĆ®°ˇ ŔŻČż ÇŇ °ćżě

			///////////////////////////////////////////////////////////////////////////////
			// Ćň¸é»óŔÇ °Ĺ¸®¸¦ ±¸ÇĎż© şń±łÇŃ´Ů.
			D3DXVECTOR3	vec3Tm = *m_pVec3Pos - pClient->PositionVector;
			vec3Tm.y = 0.0f;
			if(m_fBoundaryDistance > D3DXVec3Length(&vec3Tm))
			{
				return true;
			}
		}
		return false;
	}

	CNPCMapChannel			*m_pNMapChannel;
	D3DXVECTOR3				*m_pVec3Pos;
	float					m_fBoundaryDistance;
};

// ÁÖŔ§żˇ Äł¸ŻĹÍ°ˇ ŔÖ´ÂÁö ĂĽĹ©ÇĎ´Â ÇÔĽö
BOOL CNPCMapChannel::NPCCharacterExistInRange(D3DXVECTOR3 *pPos, int nBlockGap)
{
	TWO_BLOCK_INDEXES	blockIdx;
	int					i, j;

	m_pNPCMapProject->GetBlockAdjacentToPositionHalfDistance(pPos->x, pPos->z, nBlockGap, blockIdx);

	i = blockIdx.sMinX;
	while(i <= blockIdx.sMaxX)
	{
		j = blockIdx.sMinZ;
		while(j <= blockIdx.sMaxZ)
		{
			CMapBlock	*pMapBlock = &m_arrMapBlock[i][j];
			if(false == pMapBlock->m_CharacterIndexMtlist.empty())
			{				
				pMapBlock->m_CharacterIndexMtlist.lock();
				{
					mtlistUnitIndex_t::iterator itr = find_if(pMapBlock->m_CharacterIndexMtlist.begin(), pMapBlock->m_CharacterIndexMtlist.end()
					, find_if_functor_NPCCharacterExistInRange(this, pPos, nBlockGap/2));
					if(itr != pMapBlock->m_CharacterIndexMtlist.end())
					{
						pMapBlock->m_CharacterIndexMtlist.unlock();
						return TRUE;
					}
				}
				pMapBlock->m_CharacterIndexMtlist.unlock();
			}		
			j++;
		}
		i++;
	}
	
	return FALSE;
}


///////////////////////////////////////////////////////////////////////////////
// ÇÔĽöŔÚ Ĺ¬·ˇ˝ş
//	Äł¸ŻĹÍ°ˇ °Ĺ¸®ľČżˇ ŔÖ°í ŔŻČżÇĎ¸ç żîżµ,¸¶˝şĹÍ,°ü¸®ŔÚ °čÁ¤ŔĚ ľĆ´Ď¸é Vectorżˇ Ăß°ˇ
// 2010-04-01 by cmkwon, ŔÎÇÇ2Â÷ Ăß°ˇ ĽöÁ¤ - Ŕű±ş/ľĆ±ş ¸®˝şĆ® °ˇÁ®żŔ±â ÇĂ·ˇ±× Ăß°ˇ
//
///////////////////////////////////////////////////////////////////////////////
class for_each_functor_NPCGetAdjacentCharacterIndexes
{
public:
	for_each_functor_NPCGetAdjacentCharacterIndexes(CNPCMapChannel *i_pNMapChann, D3DXVECTOR3 *i_pPos, float i_fBoundary, vector<ClientIndex_t> *i_pIndexVector, BYTE i_AttMonsterBell, BOOL i_bGetEnemyList)
		:m_pNMapChannel(i_pNMapChann), m_pVec3Pos(i_pPos), m_fBoundaryDistance(i_fBoundary), m_pClientIndexVector(i_pIndexVector), m_attMonsterBell(i_AttMonsterBell), m_bGetEnemyList(i_bGetEnemyList)		// 2010-04-01 by cmkwon, ŔÎÇÇ2Â÷ Ăß°ˇ ĽöÁ¤ - 
	{
	};
	void operator()(ClientIndex_t index)
	{
		CLIENT_INFO		*pClient = m_pNMapChannel->GetClientInfo(index);
		if(NULL == pClient
			|| CS_GAMESTARTED != pClient->ClientState
			|| COMPARE_BODYCON_BIT(pClient->BodyCondition, BODYCON_DEAD_MASK))
		{// Ĺ¬¶óŔĚľđĆ®°ˇ ŔŻČż ÇŇ °ćżě
			return;
		}

		if(IS_VALID_UNIQUE_NUMBER(m_pNMapChannel->m_CityWarOccupyGuildUID) 
			&& m_pNMapChannel->m_CityWarOccupyGuildUID == pClient->GuildUID10)
		{// Áˇ·É ż©´ÜżřŔĎ °ćżě Á¦żÜ
			return;
		}

// 2010-03-16 by cmkwon, ŔÎÇÇ2Â÷ MtoM, MtoC Ĺ¸°Ů şŻ°ć °ü·Ă ĽöÁ¤ - ˝şĹÚ˝ş »óĹÂĂĽĹ©´Â µű·Î Ăł¸®ÇŃ´Ů.
// 		///////////////////////////////////////////////////////////////////////////////
// 		// 2007-01-16 by cmkwon, ŔÎşńÁöşí »óĹÂŔÇ ŔŻŔú´Â ¸®˝şĆ®żˇ Ăß°ˇÇŘľßÇÔ
// 		if(FALSE == m_pNMapChannel->m_bCityWarStarted
// 			&& FALSE == pClient->IsTargetableCharacter(TRUE))		// 2006-11-27 by dhjin, ĽöÁ¤ÇÔ(pClient->bStealthState -> ptmClient->IsTargetableCharacter())
// 		{// µµ˝ĂÁˇ·ÉŔü ÁřÇŕÁßŔĚ ľĆ´Ď°í ˝şĹÚ˝ş »óĹÂ¶ó¸é Á¦żÜ
// 			return;
// 		}

		///////////////////////////////////////////////////////////////////////////////
		// 2010-04-01 by cmkwon, ŔÎÇÇ2Â÷ Ăß°ˇ ĽöÁ¤ - 
		if(m_bGetEnemyList)
		{
			///////////////////////////////////////////////////////////////////////////////
			// 2009-12-11 by cmkwon, µĄąĚÁö ľî±×·Î·Î Ĺ¸°ŮŔ» şŻ°ćÇĎ´Â ¸ó˝şĹÍ ±¸Çö - ĽĽ·Â¸ó˝şĹÍ ŔĎ °ćżě °°Ŕş ĽĽ·ÂŔş Á¦żÜ
			if(IS_INFLWAR_MONSTER(m_attMonsterBell)
				&& IS_SAME_CHARACTER_MONSTER_INFLUENCE(pClient->InfluenceType1, m_attMonsterBell))
			{
				return;
			}

 			///////////////////////////////////////////////////////////////////////////////
 			// 2010-03-16 by cmkwon, ŔÎÇÇ2Â÷ MtoM, MtoC Ĺ¸°Ů şŻ°ć °ü·Ă ĽöÁ¤ - BELL_INFINITY_DEFENSE_MONSTER´Â Äł¸ŻĹÍ°ˇ ľĆ±şŔÓ
 			if(BELL_INFINITY_DEFENSE_MONSTER == m_attMonsterBell)
 			{
 				return;
 			}
		}
		else
		{
			///////////////////////////////////////////////////////////////////////////////
			// 2009-12-11 by cmkwon, µĄąĚÁö ľî±×·Î·Î Ĺ¸°ŮŔ» şŻ°ćÇĎ´Â ¸ó˝şĹÍ ±¸Çö - ĽĽ·Â¸ó˝şĹÍ ŔĎ °ćżě °°Ŕş ĽĽ·ÂŔş Á¦żÜ
			if(IS_INFLWAR_MONSTER(m_attMonsterBell)
				&& (INFLUENCE_TYPE_NORMAL != pClient->InfluenceType1 || FALSE == IS_SAME_CHARACTER_MONSTER_INFLUENCE(pClient->InfluenceType1, m_attMonsterBell)))
			{
				return;
			}

			if(BELL_INFINITY_ATTACK_MONSTER == m_attMonsterBell)
			{
				return;
 			}
		}
		
		if(m_fBoundaryDistance >= D3DXVec3Length(&(*m_pVec3Pos - pClient->PositionVector)))
		{
			m_pClientIndexVector->push_back(index);
		}				
	}

	CNPCMapChannel			*m_pNMapChannel;
	D3DXVECTOR3				*m_pVec3Pos;
	float					m_fBoundaryDistance;
	vector<ClientIndex_t>	*m_pClientIndexVector;
	BYTE					m_attMonsterBell;		// 2009-12-11 by cmkwon, µĄąĚÁö ľî±×·Î·Î Ĺ¸°ŮŔ» şŻ°ćÇĎ´Â ¸ó˝şĹÍ ±¸Çö - 
	BOOL					m_bGetEnemyList;		// 2010-04-01 by cmkwon, ŔÎÇÇ2Â÷ Ăß°ˇ ĽöÁ¤ - 
};

// 2009-12-11 by cmkwon, µĄąĚÁö ľî±×·Î·Î Ĺ¸°ŮŔ» şŻ°ćÇĎ´Â ¸ó˝şĹÍ ±¸Çö - ŔÎŔÚĂß°ˇ(, BYTE i_AttMonsterBell)
// 2010-04-01 by cmkwon, ŔÎÇÇ2Â÷ Ăß°ˇ ĽöÁ¤ - 
int CNPCMapChannel::NPCGetAdjacentCharacterIndexes(D3DXVECTOR3 *pPos, int nDistance, int nBlockDistance, vector<ClientIndex_t> *pClientIndexVector, BYTE i_AttMonsterBell, BOOL i_bGetEnemyList/*=TRUE*/)
{
	TWO_BLOCK_INDEXES	blockIdx;
	int					i, j;

#ifdef _DEBUG
	int tmpCap = pClientIndexVector->capacity();
	pClientIndexVector->clear();
	int tmpCap2 = pClientIndexVector->capacity();
	assert( tmpCap == tmpCap2 );
#endif

	m_pNPCMapProject->GetBlockAdjacentToPositionHalfDistance(pPos->x, pPos->z, nBlockDistance, blockIdx);

	pClientIndexVector->clear();
	i = blockIdx.sMinX;
	while(i <= blockIdx.sMaxX)
	{
		j = blockIdx.sMinZ;
		while(j <= blockIdx.sMaxZ)
		{
			CMapBlock	*pMapBlock = &m_arrMapBlock[i][j];

			if (false == pMapBlock->m_CharacterIndexMtlist.empty())
			{
				mtlistUnitIndex_t vectorTemp;
				pMapBlock->m_CharacterIndexMtlist.lock();
				{
					vectorTemp.reserve(pMapBlock->m_CharacterIndexMtlist.size());
					vectorTemp.insert(vectorTemp.end()
						, pMapBlock->m_CharacterIndexMtlist.begin(), pMapBlock->m_CharacterIndexMtlist.end());
				}
				pMapBlock->m_CharacterIndexMtlist.unlock();
				
				for_each(vectorTemp.begin(), vectorTemp.end()
					, for_each_functor_NPCGetAdjacentCharacterIndexes(this, pPos, nDistance, pClientIndexVector, i_AttMonsterBell, i_bGetEnemyList));
			}
			j++;
		}
		i++;
	}

	return pClientIndexVector->size();
}


////////////////////////////////////////////////////////////////////////////////
//
// ÇÔ Ľö ŔĚ ¸§  : CNPCMapProject::NPCMonsterExistInRange
// ąÝČŻµÇ´Â Çü  : BOOL
// ÇÔ Ľö ŔÎ ŔÚ  : int nMonsterIdx
// ÇÔ Ľö ŔÎ ŔÚ  : D3DXVECTOR3 posVector3
// ÇÔ Ľö ŔÎ ŔÚ  : float fDisGap ==> Ăćµą Ăł¸® °č»ęŔ» Ŕ§ÇŃ °Ĺ¸®
// ÇÔ Ľö ŔÎ ŔÚ  : float fBlockGap ==> Block¸¦ °ˇÁ®żĂ °Ĺ¸®
// ÇÔ Ľö Ľł ¸í  : ŔÔ·ÂµČ ÁÂÇĄ·Î şÎĹÍ °Ĺ¸® ľČżˇ ¸ó˝şĹÍ°ˇ ŔÖ´ÂÁöŔÇ ż©şÎ
//
CMonster* CNPCMapChannel::NPCMonsterExistInRange(int nMonsterIdx, const D3DXVECTOR3 *pPositionVector3, float fDisGap, float fBlockGap)
{
	TWO_BLOCK_INDEXES	blockIdx;
	CMonster			*pMonster = NULL;
	int					i, j;
	CMapBlock			*pMapBlock = NULL;

	m_pNPCMapProject->GetBlockAdjacentToPositionHalfDistance(pPositionVector3->x, pPositionVector3->z, fBlockGap, blockIdx);

	i = blockIdx.sMinX;
	while(i <= blockIdx.sMaxX)
	{
		j = blockIdx.sMinZ;
		while(j <= blockIdx.sMaxZ)
		{
			pMapBlock = &m_arrMapBlock[i][j];

			if (false == pMapBlock->m_MonsterIndexMtlist.empty())
			{
				mtlistUnitIndex_t	vectorTemp;				
				pMapBlock->m_MonsterIndexMtlist.lock();
				{
					vectorTemp.reserve(pMapBlock->m_MonsterIndexMtlist.size());
					vectorTemp.insert(vectorTemp.end()
						, pMapBlock->m_MonsterIndexMtlist.begin(), pMapBlock->m_MonsterIndexMtlist.end());					
				}
				pMapBlock->m_MonsterIndexMtlist.unlock();

				mtlistUnitIndex_t::iterator itr = vectorTemp.begin();
				while(itr != vectorTemp.end())
				{
					if(*itr != nMonsterIdx)
					{
						pMonster = GetMonster(*itr);
						if(pMonster && pMonster->m_enMonsterState != MS_NULL)
						{ // Ĺ¬¶óŔĚľđĆ®°ˇ ŔŻČż ÇŇ °ćżě
							
							if(D3DXVec3Length(&(*pPositionVector3 - pMonster->PositionVector)) < fDisGap)
							{
								return pMonster;
							}
						}
					}
					itr++;
				}
			}
			j++;
		}
		i++;
	}

	return NULL;
}

///////////////////////////////////////////////////////////////////////////////
// ÇÔĽöŔÚ Ĺ¬·ˇ˝ş
//	¸ó˝şĹÍ°ˇ °Ĺ¸®ľČżˇ ŔÖ°í ŔŻČżÇĎ¸é Vectorżˇ Ăß°ˇ
//
///////////////////////////////////////////////////////////////////////////////
class for_each_functor_NPCGetAdjacentMonsterIndexes
{
public:
	for_each_functor_NPCGetAdjacentMonsterIndexes(CNPCMapChannel *i_pNMapChann, D3DXVECTOR3 *i_pPos, float i_fBoundary, vector<ClientIndex_t> *i_pIndexVector, INT i_unitKind)
		:m_pNMapChannel(i_pNMapChann), m_pVec3Pos(i_pPos), m_fBoundaryDistance(i_fBoundary), m_pClientIndexVector(i_pIndexVector), m_MonUnitKind(i_unitKind)
	{
	};
	void operator()(ClientIndex_t index)
	{
		CMonster *pMonst = m_pNMapChannel->GetMonster(index);
		if( pMonst
			&& pMonst->m_enMonsterState != MS_NULL
			&& COMPARE_BODYCON_BIT(pMonst->BodyCondition, BODYCON_DEAD_MASK) == FALSE
			&& (0==m_MonUnitKind || m_MonUnitKind == pMonst->MonsterInfoPtr->MonsterUnitKind) )
		{	// ¸ó˝şĹÍ°ˇ ŔŻČż ÇŇ °ćżě
			
			if(m_fBoundaryDistance >= D3DXVec3Length(&(*m_pVec3Pos - pMonst->PositionVector)))
			{
				m_pClientIndexVector->push_back(index);
			}
		}
	}

	CNPCMapChannel			*m_pNMapChannel;
	D3DXVECTOR3				*m_pVec3Pos;
	float					m_fBoundaryDistance;
	vector<ClientIndex_t>	*m_pClientIndexVector;
	INT						m_MonUnitKind;
};

////////////////////////////////////////////////////////////////////////////////
// 2009-09-09 ~ 2010 by dhjin, ŔÎÇÇ´ĎĆĽ -
///////////////////////////////////////////////////////////////////////////////
// ÇÔĽöŔÚ Ĺ¬·ˇ˝ş
//	¸ó˝şĹÍ°ˇ °Ĺ¸®ľČżˇ ŔÖ°í ŔŻČżÇĎ¸é Vectorżˇ Ăß°ˇ
//
///////////////////////////////////////////////////////////////////////////////
class for_each_functor_NPCGetAdjacentMonsterIndexesByBell
{
public:
	for_each_functor_NPCGetAdjacentMonsterIndexesByBell(CNPCMapChannel *i_pNMapChann, D3DXVECTOR3 *i_pPos, float i_fBoundary, vector<ClientIndex_t> *i_pIndexVector, BYTE i_Bell)
		:m_pNMapChannel(i_pNMapChann), m_pVec3Pos(i_pPos), m_fBoundaryDistance(i_fBoundary), m_pClientIndexVector(i_pIndexVector), m_Bell(i_Bell)
	{
	};
	void operator()(ClientIndex_t index)
	{
		CMonster *pMonst = m_pNMapChannel->GetMonster(index);
		if( pMonst
			&& pMonst->m_enMonsterState != MS_NULL
			&& COMPARE_BODYCON_BIT(pMonst->BodyCondition, BODYCON_DEAD_MASK) == FALSE
			&& m_Bell == pMonst->MonsterInfoPtr->Belligerence)
		{	// ¸ó˝şĹÍ°ˇ ŔŻČż ÇŇ °ćżě
			
			if(m_fBoundaryDistance >= D3DXVec3Length(&(*m_pVec3Pos - pMonst->PositionVector)))
			{
				m_pClientIndexVector->push_back(index);
			}
		}
	}
	
	CNPCMapChannel			*m_pNMapChannel;
	D3DXVECTOR3				*m_pVec3Pos;
	float					m_fBoundaryDistance;
	vector<ClientIndex_t>	*m_pClientIndexVector;
	BYTE					m_Bell;
};


int CNPCMapChannel::NPCGetAdjacentMonsterIndexes(D3DXVECTOR3 *pPos
												 , int nDistance
												 , int nBlockDistance
												 , vector<ClientIndex_t> *pClientIndexVector
												 , INT i_MonsterUnitKind/*=0*/)
{
	TWO_BLOCK_INDEXES	blockIdx;
	int					i, j;

#ifdef _DEBUG
	int tmpCap = pClientIndexVector->capacity();
	pClientIndexVector->clear();
	int tmpCap2 = pClientIndexVector->capacity();
	assert( tmpCap == tmpCap2 );
#endif

	m_pNPCMapProject->GetBlockAdjacentToPositionHalfDistance(pPos->x, pPos->z, nBlockDistance, blockIdx);

	pClientIndexVector->clear();
	i = blockIdx.sMinX;
	while(i <= blockIdx.sMaxX)
	{
		j = blockIdx.sMinZ;
		while(j <= blockIdx.sMaxZ)
		{
			CMapBlock	*pMapBlock = &m_arrMapBlock[i][j];

			if (false == pMapBlock->m_MonsterIndexMtlist.empty())
			{
				mtlistUnitIndex_t vectorTemp;
				pMapBlock->m_MonsterIndexMtlist.lock();
				{
					vectorTemp.reserve(pMapBlock->m_MonsterIndexMtlist.size());
					vectorTemp.insert(vectorTemp.end()
						, pMapBlock->m_MonsterIndexMtlist.begin(), pMapBlock->m_MonsterIndexMtlist.end());
				}
				pMapBlock->m_MonsterIndexMtlist.unlock();

				for_each(vectorTemp.begin(), vectorTemp.end()
					, for_each_functor_NPCGetAdjacentMonsterIndexes(this, pPos, nDistance, pClientIndexVector, i_MonsterUnitKind));
			}
			j++;
		}
		i++;
	}

	return pClientIndexVector->size();
}

////////////////////////////////////////////////////////////////////////////////
//
// ÇÔ Ľö ŔĚ ¸§  : CNPCMapProject::NPCGetMonsterCountInRegion
// ąÝČŻµÇ´Â Çü  : int
// ÇÔ Ľö ŔÎ ŔÚ  : int tileStartXIdx
// ÇÔ Ľö ŔÎ ŔÚ  : int tileStartZIdx
// ÇÔ Ľö ŔÎ ŔÚ  : int tileEndXIdx
// ÇÔ Ľö ŔÎ ŔÚ  : int tileEndZIdx
// ÇÔ Ľö ŔÎ ŔÚ  : UINT nMonType
// ÇÔ Ľö Ľł ¸í  : ŔÎŔÚ·Î ÁÖľîÁř Ĺ¸ŔĎ żµżŞżˇ nMonTypeŔÇ ¸ó˝şĹÍŔÇ Count¸¦ ¸®ĹĎÇŃ´Ů
//
int CNPCMapChannel::NPCGetMonsterCountInRegion(int tileStartXIdx, int tileStartZIdx, int tileEndXIdx, int tileEndZIdx, int nMonType, int nMaxCount)
{
	TWO_BLOCK_INDEXES	blockIdx;
	int					i, j;
	int					nCount = 0;
	CMonster			*pMonster = NULL;

	m_pNPCMapProject->GetBlockIndexWithTileIndex(tileStartXIdx, tileStartZIdx, tileEndXIdx, tileEndZIdx, blockIdx);
	i = blockIdx.sMinX;
	while(i <= blockIdx.sMaxX)
	{
		j = blockIdx.sMinZ;
		while(j <= blockIdx.sMaxZ)
		{
			CMapBlock *pMapBlock = &(m_arrMapBlock[i][j]);
			// ¸ó˝şĹÍ°ˇ ľřŔ¸¸é łŃľî°¨
			if (false == pMapBlock->m_MonsterIndexMtlist.empty())
			{
				mtlistUnitIndex_t vectorTemp;
				pMapBlock->m_MonsterIndexMtlist.lock();
				{
					vectorTemp.reserve(pMapBlock->m_MonsterIndexMtlist.size());
					vectorTemp.insert(vectorTemp.end()
						, pMapBlock->m_MonsterIndexMtlist.begin(), pMapBlock->m_MonsterIndexMtlist.end());
				}
				pMapBlock->m_MonsterIndexMtlist.unlock();

				mtlistUnitIndex_t::iterator itr = vectorTemp.begin();
				while (itr != vectorTemp.end())
				{
					pMonster = GetMonster(*itr);
					if(NULL != pMonster 
						&& pMonster->MonsterInfoPtr->MonsterUnitKind == nMonType)
					{
						nCount++;
						if(nCount >= nMaxCount)
						{
							return nCount;
						}
					}
					itr++;
				}
			}
			j++;
		}
		i++;
	}

	return nCount;
}


////////////////////////////////////////////////////////////////////////////////
//
// ÇÔ Ľö ŔĚ ¸§  : CNPCMapProject::NPCGetCreatablePosition
// ąÝČŻµÇ´Â Çü  : int
// ÇÔ Ľö ŔÎ ŔÚ  : UINT nMonType
// ÇÔ Ľö ŔÎ ŔÚ  : int tileStartXIdx
// ÇÔ Ľö ŔÎ ŔÚ  : int tileStartZIdx
// ÇÔ Ľö ŔÎ ŔÚ  : int tileEndXIdx
// ÇÔ Ľö ŔÎ ŔÚ  : int tileEndZIdx
// ÇÔ Ľö ŔÎ ŔÚ  : vector<D3DXVECTOR3> &vecVECTOR2
// ÇÔ Ľö ŔÎ ŔÚ  : int nMaxCount
// ÇÔ Ľö Ľł ¸í  : ŔÎŔÚ·Î ÁÖľîÁř Ĺ¸ŔĎ żµżŞżˇĽ­ ¸ó˝şĹÍ Ĺ¸ŔÔżˇ ¸Â´Â »ýĽş °ˇ´ÉÇŃ ÁÂÇĄ¸¦ vecVector2żˇ Ăß°ˇÇĎ°í ±× »çŔĚÁî¸¦ ¸®ĹĎÇŃ´Ů
//
int CNPCMapChannel::NPCGetCreatablePosition(BYTE nMonsterForm, int nMonsterSize
											, int tileStartXIdx, int tileStartZIdx
											, int tileEndXIdx, int tileEndZIdx
											, int nMinHeight, int nMaxHeight
											, vector<D3DXVECTOR3> &vecVECTOR2, int nMaxCount
											, BOOL bCharCheckFlag/*=TRUE*/
											, BOOL i_bAbsoluteAltitude/*=FALSE*/)
{
	int			nCount = 0;
	float		fHeight = 0.0f;
	D3DXVECTOR3	tmpVector3;
	D3DXVECTOR3 tmUnitVec3(1, 0, 0);
	BOOL		bInsertFlag;

	vecVECTOR2.clear();
	while((int)vecVECTOR2.size() < nMaxCount 
		&& nCount < nMaxCount * 2)
	{
		bInsertFlag		= FALSE;
		int nRandx = rand();
		tmpVector3.x = (float)( tileStartXIdx*SIZE_MAP_TILE_SIZE + nRandx%((tileEndXIdx - tileStartXIdx)*SIZE_MAP_TILE_SIZE) );
		tmpVector3.z = (float)( tileStartZIdx*SIZE_MAP_TILE_SIZE + rand()%((tileEndZIdx - tileStartZIdx)*SIZE_MAP_TILE_SIZE) );
		tmpVector3.y = 0.0f;
		if(FALSE == m_pNPCMapProject->IsValidTileForCreateMonster(&tmpVector3)
			|| (TRUE == bCharCheckFlag && TRUE == NPCCharacterExistInRange(&tmpVector3, SIZE_MONSTER_CREATION_RANGE * 2)))
		{	// ¸ó˝şĹÍ ŔĚµż şŇ°ˇ ÁöżŞŔÎÁö ĂĽĹ©
			// ÁÖŔ§żˇ Äł¸ŻŔĚ ŔÖ´ÂÁö ĂĽĹ©
			
			nCount++;
			continue;
		}
		fHeight = m_pNPCMapProject->GetMapHeightIncludeWater(&tmpVector3);
		
		switch(nMonsterForm)
		{
		case FORM_FLYING_RIGHT:
		case FORM_FLYINGandGROUND_RIGHT:
		case FORM_FLYING_COPTER:
		case FORM_FLYINGandGROUND_COPTER:
			{
				if(FALSE == i_bAbsoluteAltitude)
				{
					tmpVector3.y	= fHeight + nMinHeight + nRandx%(nMaxHeight-nMinHeight);
					bInsertFlag		= TRUE;
				}
				else
				{
					if(fHeight < nMinHeight)
					{
						tmpVector3.y = nMinHeight + nRandx%(nMaxHeight - nMinHeight);						
						bInsertFlag	= TRUE;
					}
					else if(fHeight+5 < nMaxHeight)
					{
						tmpVector3.y = fHeight + nRandx%(int(nMaxHeight - fHeight));						
						bInsertFlag	= TRUE;
					}
				}
			}
			break;
		case FORM_GROUND_MOVE:
			{
				tmpVector3.y	= fHeight + nMonsterSize;
				bInsertFlag		= TRUE;
			}
			break;
		case FORM_SWIMMINGFLYING_RIGHT:
		case FORM_SWIMMINGFLYING_COPTER:
			{
				if(FALSE == i_bAbsoluteAltitude)
				{
					if(m_pNPCMapProject->IsWaterTile(&tmpVector3))
					{
						float fMapHeight = m_pNPCMapProject->GetMapHeightExcludeWater(&tmpVector3);
						if(fMapHeight+SIZE_MAP_TILE_SIZE < fHeight)
						{
							tmpVector3.y	= fHeight - SIZE_MAP_TILE_SIZE/2;							
						}
						else
						{
							tmpVector3.y	= fMapHeight + SIZE_MAP_TILE_SIZE/2;
						}
						bInsertFlag			= TRUE;	
					}
					else
					{
						tmpVector3.y	= fHeight + nMinHeight + nRandx%(nMaxHeight-nMinHeight);
						bInsertFlag		= TRUE;
					}
				}
				else
				{
					if(fHeight < nMinHeight)
					{
						tmpVector3.y	= nMinHeight + nRandx%(nMaxHeight - nMinHeight);						
						bInsertFlag		= TRUE;
					}
					else if(fHeight+5 < nMaxHeight)
					{
						tmpVector3.y	= fHeight + nRandx%(int(nMaxHeight - fHeight));						
						bInsertFlag		= TRUE;
					}
				}				
			}
			break;
		}// end_switch(nMonsterForm)

		if(bInsertFlag)
		{
			vecVECTOR2.push_back(tmpVector3);			
		}

		nCount++;
	}// end_while((int)vecVECTOR2.size() < nMaxCount && nCount < nMaxCount * 2)

	return vecVECTOR2.size();
}


int CNPCMapChannel::NPCGetTargetwithAttackObj(BYTE AttackObj, CMonster *pM, vector<ClientIndex_t> &ClientIndexVector)
{
	if(ClientIndexVector.empty()){				return 0;}

// 2010-03-16 by cmkwon, ŔÎÇÇ2Â÷ MtoM, MtoC Ĺ¸°Ů şŻ°ć °ü·Ă ĽöÁ¤ - ľĆ·ˇżÍ °°ŔĚ ĽöÁ¤ÇÔ.
// 	///////////////////////////////////////////////////////////////////////////////
// 	// 2007-05-22 by cmkwon, ¸ó˝şĹÍ-->Ĺ¸°ŮżˇĽ­ żŔşęÁ§Ć® ĂćµąĂĽĹ© Ăß°ˇÇÔ
// 	CLIENT_INFO		*pCli = NULL;
// 	BOOL			bRecognizeInvisible = COMPARE_MPOPTION_BIT(pM->MonsterInfoPtr->MPOption, MPOPTION_BIT_RECOGNIZE_INVISIBLE);
// 	vectClientIndex_t::iterator itr(ClientIndexVector.begin());
// 	while(itr != ClientIndexVector.end())
// 	{
// 		pCli = GetClientInfo(*itr);
// 		if(NULL == pCli
// 			|| FALSE == pCli->IsEnbleTargeted(pM->MonsterInfoPtr->Belligerence, bRecognizeInvisible)
// 			|| FALSE != CheckImpactStraightLineMapAndObjects(&pM->PositionVector, &pCli->PositionVector
// 				, DEFAULT_OBJECT_MONSTER_OBJECT+pM->MonsterInfoPtr->MonsterUnitKind))
// 		{
// 			itr = ClientIndexVector.erase(itr);
// 			continue;
// 		}
// 		itr++;
// 	}
// 	if(ClientIndexVector.empty()){				return 0;}
// 
// 	int				nRetTarget = 0;
// 	if(ClientIndexVector.size() == 1)
// 	{
// 		pCli = GetClientInfo(ClientIndexVector[0]);
// 		if(pCli
// 			&& pCli->IsEnbleTargeted(pM->MonsterInfoPtr->Belligerence, bRecognizeInvisible))
// 		{
// 			return ClientIndexVector[0];
// 		}
// 		return 0;
// 	}
// 
// 	switch(AttackObj)
// 	{
// 	case ATTACKOBJ_CLOSERANGE:
// 		{
// 			float	fDistance;
// 			float	fMinDistance = this->GetMonsterVisibleDiameterW();
// 			vector<ClientIndex_t>::iterator itr = ClientIndexVector.begin();
// 			while(itr != ClientIndexVector.end())
// 			{
// 				pCli = GetClientInfo(*itr);
// 				if(pCli
// 					&& pCli->IsEnbleTargeted(pM->MonsterInfoPtr->Belligerence, bRecognizeInvisible)
// 					&& (fDistance = D3DXVec3Length(&(pCli->PositionVector - pM->PositionVector))) < fMinDistance)
// 				{
// 					nRetTarget = *itr;
// 					fMinDistance = fDistance;
// 				}
// 				itr++;
// 			}
// 		}
// 		break;
// 	case ATTACKOBJ_LOWHP:
// 		{
// 			float	fMinHP = 65535.0f;
// 			vector<ClientIndex_t>::iterator itr = ClientIndexVector.begin();
// 			while(itr != ClientIndexVector.end())
// 			{
// 				pCli = GetClientInfo(*itr);
// 				if(pCli
// 					&& pCli->IsEnbleTargeted(pM->MonsterInfoPtr->Belligerence, bRecognizeInvisible)
// 					&& pCli->CurrentHP < fMinHP)
// 				{
// 					nRetTarget = *itr;
// 					fMinHP = pCli->CurrentHP;
// 				}
// 				itr++;
// 			}
// 		}
// 		break;
// 	case ATTACKOBJ_HIGHHP:
// 		{
// 			float	fMaxHP = 0.0f;
// 			vector<ClientIndex_t>::iterator itr = ClientIndexVector.begin();
// 			while(itr != ClientIndexVector.end())
// 			{
// 				pCli = GetClientInfo(*itr);
// 				if(pCli
// 					&& pCli->IsEnbleTargeted(pM->MonsterInfoPtr->Belligerence, bRecognizeInvisible)
// 					&& pCli->CurrentHP > fMaxHP)
// 				{
// 					nRetTarget = *itr;
// 					fMaxHP = pCli->CurrentHP;
// 				}
// 				itr++;
// 			}
// 		}
// 		break;
// 	default:
// 		// ATTACKOBJ_FIRSTATTACK
// 		// ATTACKOBJ_SAMERACE
// 		// ATTACKOBJ_RANDOM
// 		// ATTACKOBJ_AGGRO			// 2009-12-11 by cmkwon, µĄąĚÁö ľî±×·Î·Î Ĺ¸°ŮŔ» şŻ°ćÇĎ´Â ¸ó˝şĹÍ ±¸Çö - 
// 		{
// 			for(int i=0; i < 5; i++)
// 			{
// 				nRetTarget = ClientIndexVector[GetTickCount()%ClientIndexVector.size()];
// 				pCli = GetClientInfo(nRetTarget);
// 				if(pCli
// 					&& pCli->IsEnbleTargeted(pM->MonsterInfoPtr->Belligerence, bRecognizeInvisible))
// 				{
// 					return nRetTarget;
// 				}
// 			}
// 			nRetTarget = 0;
// 		}
// 	}
	///////////////////////////////////////////////////////////////////////////////
	// 2010-03-16 by cmkwon, ŔÎÇÇ2Â÷ MtoM, MtoC Ĺ¸°Ů şŻ°ć °ü·Ă ĽöÁ¤ - 
	CLIENT_INFO		*pCli = NULL;
	CNPCMonster		*pNPCMon = NULL;
	BOOL			bRecognizeInvisible = COMPARE_MPOPTION_BIT(pM->MonsterInfoPtr->MPOption, MPOPTION_BIT_RECOGNIZE_INVISIBLE);
	vectClientIndex_t::iterator itr(ClientIndexVector.begin());
	while(itr != ClientIndexVector.end())
	{
		pCli		= NULL;
		pNPCMon		= NULL;
		ClientIndex_t tarIdx = *itr;
		if(FALSE == this->GetUnitObject(tarIdx, &pCli, &pNPCMon))
		{
			itr = ClientIndexVector.erase(itr);
			continue;
		}
		
		if(pCli)
		{
			if(FALSE == pCli->IsValidClient()
				|| FALSE == pCli->IsEnbleTargeted(pM->MonsterInfoPtr->Belligerence, bRecognizeInvisible)
				|| FALSE != CheckImpactStraightLineMapAndObjects(&pM->PositionVector, &pCli->PositionVector, DEFAULT_OBJECT_MONSTER_OBJECT+pM->MonsterInfoPtr->MonsterUnitKind))
			{
				itr = ClientIndexVector.erase(itr);
				continue;
			}
		}
		if(pNPCMon)
		{
			if(FALSE == pNPCMon->IsValidMonster()
				|| FALSE != CheckImpactStraightLineMapAndObjects(&pM->PositionVector, &pNPCMon->PositionVector, DEFAULT_OBJECT_MONSTER_OBJECT+pM->MonsterInfoPtr->MonsterUnitKind))
			{
				itr = ClientIndexVector.erase(itr);
				continue;
			}

		}
		itr++;
	}
	if(ClientIndexVector.empty()){					return 0;}


	int				nRetTarget = 0;
	if(ClientIndexVector.size() == 1)
	{
		return ClientIndexVector[0];
	}

	switch(AttackObj)
	{
	case ATTACKOBJ_CLOSERANGE:
		{
			float	fDistance;
			float	fMinDistance = this->GetMonsterVisibleDiameterW();
			vector<ClientIndex_t>::iterator itr = ClientIndexVector.begin();
			for(;itr != ClientIndexVector.end(); itr++)
			{
				pCli		= NULL;
				pNPCMon		= NULL;
				ClientIndex_t tarIdx = *itr;
				if(FALSE == this->GetUnitObject(tarIdx, &pCli, &pNPCMon))
				{
					continue;
				}
				if(pCli && pCli->IsValidClient())
				{
					fDistance = D3DXVec3Length(&(pCli->PositionVector - pM->PositionVector));
					if(fDistance < fMinDistance)
					{
						nRetTarget		= tarIdx;
						fMinDistance	= fDistance;
					}
				}

				if(pNPCMon && pNPCMon->IsValidMonster())
				{
					fDistance = D3DXVec3Length(&(pNPCMon->PositionVector - pM->PositionVector));
					if(fDistance < fMinDistance)
					{
						nRetTarget		= tarIdx;
						fMinDistance	= fDistance;
					}
				}
			}
		}
		break;
	case ATTACKOBJ_LOWHP:
		{
			float	fMinHP = 65535.0f;
			vector<ClientIndex_t>::iterator itr = ClientIndexVector.begin();
			for(;itr != ClientIndexVector.end(); itr++)
			{
				pCli		= NULL;
				pNPCMon		= NULL;
				ClientIndex_t tarIdx = *itr;
				if(FALSE == this->GetUnitObject(tarIdx, &pCli, &pNPCMon))
				{
					continue;
				}				
				if(pCli && pCli->IsValidClient())
				{
					if(pCli->CurrentHP < fMinHP)
					{
						nRetTarget	= tarIdx;
						fMinHP		= pCli->CurrentHP;
					}
				}
				if(pNPCMon && pNPCMon->IsValidMonster())
				{
					if(pNPCMon->CurrentHP < fMinHP)
					{
						nRetTarget	= tarIdx;
						fMinHP		= pNPCMon->CurrentHP;
					}
				}
			}
		}
		break;
	case ATTACKOBJ_HIGHHP:
		{
			float	fMaxHP = 0.0f;
			vector<ClientIndex_t>::iterator itr = ClientIndexVector.begin();
			for(;itr != ClientIndexVector.end(); itr++)
			{
				pCli		= NULL;
				pNPCMon		= NULL;
				ClientIndex_t tarIdx = *itr;
				if(FALSE == this->GetUnitObject(tarIdx, &pCli, &pNPCMon))
				{
					continue;
				}				
				if(pCli && pCli->IsValidClient())
				{
					if(pCli->CurrentHP > fMaxHP)
					{
						nRetTarget	= tarIdx;
						fMaxHP		= pCli->CurrentHP;
					}
				}
				if(pNPCMon && pNPCMon->IsValidMonster())
				{
					if(pNPCMon->CurrentHP > fMaxHP)
					{
						nRetTarget	= tarIdx;
						fMaxHP		= pNPCMon->CurrentHP;
					}
				}
			}
		}
		break;
	default:
		// ATTACKOBJ_FIRSTATTACK
		// ATTACKOBJ_SAMERACE
		// ATTACKOBJ_RANDOM
		// ATTACKOBJ_AGGRO			// 2009-12-11 by cmkwon, µĄąĚÁö ľî±×·Î·Î Ĺ¸°ŮŔ» şŻ°ćÇĎ´Â ¸ó˝şĹÍ ±¸Çö - 
		{
			nRetTarget = ClientIndexVector[GetTickCount()%ClientIndexVector.size()];
		}
	}

	return nRetTarget;
}



void CNPCMapChannel::NPCSetPartyPosition(CNPCMonster *pMons)
{
	CNPCMonster *pManager = GetNPCMonster(pMons->m_nPartyManagerIndex);
	if(NULL == pManager
		|| pManager->m_enMonsterState == MS_NULL
		|| COMPARE_BODYCON_BIT(pManager->BodyCondition, BODYCON_DEAD_MASK)
		|| 0 != pManager->m_nTargetIndex)
	{
		pMons->ResetPartyVariable();
	}

	const float fFormationFBInterval = 20.0f;		// Ćí´ë ´ëÇüŔ» Ŕ§ÇŃ °Ł°Ý
	D3DXVECTOR3	vec3NewPos;
	D3DXVECTOR3	vec3Left90;
	D3DXVECTOR3	vec3Right90;
	float		fDist;
	switch(pManager->MonsterInfoPtr->MPOptionParam1)
	{
	case FORMATION_COLUMN:
		{
			if(1 == pMons->m_byPartyFormationIndex%2)
			{
				vec3NewPos = pManager->PositionVector - pManager->TargetVector*pManager->MonsterInfoPtr->Size*3*(1 + (pMons->m_byPartyFormationIndex>>1))
					- pManager->TargetVector*fFormationFBInterval*(1 + (pMons->m_byPartyFormationIndex>>1));
			}
			else
			{
				GNPCRotateTargetVectorHorizontal(&vec3Right90, &pManager->TargetVector, MSD_LEFT_90, 0);
				vec3Right90 = pManager->PositionVector + vec3Right90*pManager->MonsterInfoPtr->Size + vec3Right90*fFormationFBInterval;
				vec3NewPos = vec3Right90 - pManager->TargetVector*pManager->MonsterInfoPtr->Size*3*(pMons->m_byPartyFormationIndex>>1)
					- pManager->TargetVector*fFormationFBInterval*(pMons->m_byPartyFormationIndex>>1);
			}
		}
		break;
	case FORMATION_LINE:
		{
			if(1 == pMons->m_byPartyFormationIndex%2)
			{
				GNPCRotateTargetVectorHorizontal(&vec3Right90, &pManager->TargetVector, MSD_LEFT_90, 0);
				vec3NewPos = pManager->PositionVector + vec3Right90*pManager->MonsterInfoPtr->Size*(1 + (pMons->m_byPartyFormationIndex>>1))
					+vec3Right90*fFormationFBInterval*(1 + (pMons->m_byPartyFormationIndex>>1));
			}
			else
			{
				GNPCRotateTargetVectorHorizontal(&vec3Right90, &pManager->TargetVector, MSD_LEFT_90, 0);
				vec3NewPos = pManager->PositionVector - pManager->TargetVector*pManager->MonsterInfoPtr->Size*3
					+ vec3Right90*pManager->MonsterInfoPtr->Size*(pMons->m_byPartyFormationIndex>>1) + vec3Right90*fFormationFBInterval*(pMons->m_byPartyFormationIndex>>1);
			}
		}
		break;
	case FORMATION_TRIANGLE:
		{
			switch(pMons->m_byPartyFormationIndex)
			{
			case 0:
				{
					GNPCRotateTargetVectorHorizontal(&vec3Left90, &pManager->TargetVector, MSD_RIGHT_90, 0);
					vec3NewPos = pManager->PositionVector
						- pManager->TargetVector*(pManager->MonsterInfoPtr->Size + fFormationFBInterval)
						+ vec3Left90*(pManager->MonsterInfoPtr->Size + fFormationFBInterval)/2;
				}
				break;
			case 1:
				{
					GNPCRotateTargetVectorHorizontal(&vec3Right90, &pManager->TargetVector, MSD_LEFT_90, 0);
					vec3NewPos = pManager->PositionVector
						- pManager->TargetVector*(pManager->MonsterInfoPtr->Size + fFormationFBInterval)
						+ vec3Right90*(pManager->MonsterInfoPtr->Size + fFormationFBInterval)/2;
				}
				break;
			case 2:
				{
					GNPCRotateTargetVectorHorizontal(&vec3Left90, &pManager->TargetVector, MSD_RIGHT_90, 0);
					vec3NewPos = pManager->PositionVector
						- pManager->TargetVector*(pManager->MonsterInfoPtr->Size + fFormationFBInterval)*2
						+ vec3Left90*(pManager->MonsterInfoPtr->Size + fFormationFBInterval);
				}
				break;
			case 3:
				{
					vec3NewPos = pManager->PositionVector
						- pManager->TargetVector*(pManager->MonsterInfoPtr->Size + fFormationFBInterval)*2;
				}
				break;
			case 4:
				{
					GNPCRotateTargetVectorHorizontal(&vec3Right90, &pManager->TargetVector, MSD_LEFT_90, 0);
					vec3NewPos = pManager->PositionVector
						- pManager->TargetVector*(pManager->MonsterInfoPtr->Size + fFormationFBInterval)*2
						+ vec3Right90*(pManager->MonsterInfoPtr->Size + fFormationFBInterval);
				}
				break;
			case 5:
				{
					GNPCRotateTargetVectorHorizontal(&vec3Left90, &pManager->TargetVector, MSD_RIGHT_90, 0);
					vec3NewPos = pManager->PositionVector
						- pManager->TargetVector*(pManager->MonsterInfoPtr->Size + fFormationFBInterval)*3
						+ vec3Left90*(pManager->MonsterInfoPtr->Size + fFormationFBInterval)*3/2;
				}
				break;
			case 6:
				{
					GNPCRotateTargetVectorHorizontal(&vec3Left90, &pManager->TargetVector, MSD_RIGHT_90, 0);
					vec3NewPos = pManager->PositionVector
						- pManager->TargetVector*(pManager->MonsterInfoPtr->Size + fFormationFBInterval)*3
						+ vec3Left90*(pManager->MonsterInfoPtr->Size + fFormationFBInterval)/2;
				}
				break;
			case 7:
				{
					GNPCRotateTargetVectorHorizontal(&vec3Right90, &pManager->TargetVector, MSD_LEFT_90, 0);
					vec3NewPos = pManager->PositionVector
						- pManager->TargetVector*(pManager->MonsterInfoPtr->Size + fFormationFBInterval)*3
						+ vec3Right90*(pManager->MonsterInfoPtr->Size + fFormationFBInterval)/2;
				}
				break;
			default:
				{
					GNPCRotateTargetVectorHorizontal(&vec3Right90, &pManager->TargetVector, MSD_LEFT_90, 0);
					vec3NewPos = pManager->PositionVector
						- pManager->TargetVector*(pManager->MonsterInfoPtr->Size + fFormationFBInterval)*3
						+ vec3Right90*(pManager->MonsterInfoPtr->Size + fFormationFBInterval)*3/2;
				}
			}
		}
		break;
	case FORMATION_INVERTED_TRIANGLE:
		{
			switch(pMons->m_byPartyFormationIndex)
			{
			case 0:
				{
					GNPCRotateTargetVectorHorizontal(&vec3Right90, &pManager->TargetVector, MSD_LEFT_90, 0);
					vec3NewPos = pManager->PositionVector
						+ vec3Right90*(pManager->MonsterInfoPtr->Size+fFormationFBInterval);
				}
				break;
			case 1:
				{
					GNPCRotateTargetVectorHorizontal(&vec3Right90, &pManager->TargetVector, MSD_LEFT_90, 0);
					vec3NewPos = pManager->PositionVector
						- pManager->TargetVector*(pManager->MonsterInfoPtr->Size + fFormationFBInterval)
						+ vec3Right90*(pManager->MonsterInfoPtr->Size + fFormationFBInterval)/2;
				}
				break;
			case 2:
				{
					GNPCRotateTargetVectorHorizontal(&vec3Right90, &pManager->TargetVector, MSD_LEFT_90, 0);
					vec3NewPos = pManager->PositionVector
						+ vec3Right90*(pManager->MonsterInfoPtr->Size + fFormationFBInterval)*2;
				}
				break;
			case 3:
				{
					GNPCRotateTargetVectorHorizontal(&vec3Right90, &pManager->TargetVector, MSD_LEFT_90, 0);
					vec3NewPos = pManager->PositionVector
						- pManager->TargetVector*(pManager->MonsterInfoPtr->Size + fFormationFBInterval)
						+ vec3Right90*(pManager->MonsterInfoPtr->Size + fFormationFBInterval)*3/2;
				}
				break;
			case 4:
				{
					GNPCRotateTargetVectorHorizontal(&vec3Right90, &pManager->TargetVector, MSD_LEFT_90, 0);
					vec3NewPos = pManager->PositionVector
						- pManager->TargetVector*(pManager->MonsterInfoPtr->Size + fFormationFBInterval)*2
						+ vec3Right90*(pManager->MonsterInfoPtr->Size + fFormationFBInterval);
				}
				break;
			case 5:
				{
					GNPCRotateTargetVectorHorizontal(&vec3Right90, &pManager->TargetVector, MSD_LEFT_90, 0);
					vec3NewPos = pManager->PositionVector
						+ vec3Right90*(pManager->MonsterInfoPtr->Size + fFormationFBInterval)*3;
				}
				break;
			case 6:
				{
					GNPCRotateTargetVectorHorizontal(&vec3Right90, &pManager->TargetVector, MSD_LEFT_90, 0);
					vec3NewPos = pManager->PositionVector
						- pManager->TargetVector*(pManager->MonsterInfoPtr->Size + fFormationFBInterval)
						+ vec3Right90*(pManager->MonsterInfoPtr->Size + fFormationFBInterval)*5/2;
				}
				break;
			case 7:
				{
					GNPCRotateTargetVectorHorizontal(&vec3Right90, &pManager->TargetVector, MSD_LEFT_90, 0);
					vec3NewPos = pManager->PositionVector
						- pManager->TargetVector*(pManager->MonsterInfoPtr->Size + fFormationFBInterval)*2
						+ vec3Right90*(pManager->MonsterInfoPtr->Size + fFormationFBInterval)*2;
				}
				break;
			default:
				{
					GNPCRotateTargetVectorHorizontal(&vec3Right90, &pManager->TargetVector, MSD_LEFT_90, 0);
					vec3NewPos = pManager->PositionVector
						- pManager->TargetVector*(pManager->MonsterInfoPtr->Size + fFormationFBInterval)*3
						+ vec3Right90*(pManager->MonsterInfoPtr->Size + fFormationFBInterval)*3/2;
				}
			}
		}
		break;
	case FORMATION_BELL:
		{
			switch(pMons->m_byPartyFormationIndex)
			{
			case 0:
				{
					GNPCRotateTargetVectorHorizontal(&vec3Left90, &pManager->TargetVector, MSD_RIGHT_90, 0);
					vec3NewPos = pManager->PositionVector
						- pManager->TargetVector*(pManager->MonsterInfoPtr->Size + fFormationFBInterval)
						+ vec3Left90*(pManager->MonsterInfoPtr->Size + fFormationFBInterval);
				}
				break;
			case 1:
				{
					vec3NewPos = pManager->PositionVector
						- pManager->TargetVector*(pManager->MonsterInfoPtr->Size + fFormationFBInterval);
				}
				break;
			case 2:
				{
					GNPCRotateTargetVectorHorizontal(&vec3Right90, &pManager->TargetVector, MSD_LEFT_90, 0);
					vec3NewPos = pManager->PositionVector
						- pManager->TargetVector*(pManager->MonsterInfoPtr->Size + fFormationFBInterval)
						+ vec3Left90*(pManager->MonsterInfoPtr->Size + fFormationFBInterval);
				}
				break;
			case 3:
				{
					GNPCRotateTargetVectorHorizontal(&vec3Left90, &pManager->TargetVector, MSD_RIGHT_90, 0);
					vec3NewPos = pManager->PositionVector
						- pManager->TargetVector*(pManager->MonsterInfoPtr->Size + fFormationFBInterval)*3/2
						+ vec3Left90*(pManager->MonsterInfoPtr->Size + fFormationFBInterval)*3/2;
				}
				break;
			case 4:
				{
					GNPCRotateTargetVectorHorizontal(&vec3Left90, &pManager->TargetVector, MSD_RIGHT_90, 0);
					vec3NewPos = pManager->PositionVector
						- pManager->TargetVector*(pManager->MonsterInfoPtr->Size + fFormationFBInterval)*3/2
						+ vec3Left90*(pManager->MonsterInfoPtr->Size + fFormationFBInterval)/2;
				}
				break;
			case 5:
				{
					GNPCRotateTargetVectorHorizontal(&vec3Right90, &pManager->TargetVector, MSD_LEFT_90, 0);
					vec3NewPos = pManager->PositionVector
						- pManager->TargetVector*(pManager->MonsterInfoPtr->Size + fFormationFBInterval)*3/2
						+ vec3Right90*(pManager->MonsterInfoPtr->Size + fFormationFBInterval)/2;
				}
				break;
			case 6:
				{
					GNPCRotateTargetVectorHorizontal(&vec3Right90, &pManager->TargetVector, MSD_LEFT_90, 0);
					vec3NewPos = pManager->PositionVector
						- pManager->TargetVector*(pManager->MonsterInfoPtr->Size + fFormationFBInterval)*3/2
						+ vec3Right90*(pManager->MonsterInfoPtr->Size + fFormationFBInterval)*3/2;
				}
				break;
			case 7:
				{
					GNPCRotateTargetVectorHorizontal(&vec3Left90, &pManager->TargetVector, MSD_RIGHT_90, 0);
					vec3NewPos = pManager->PositionVector
						- pManager->TargetVector*(pManager->MonsterInfoPtr->Size + fFormationFBInterval)*5/2
						+ vec3Left90*(pManager->MonsterInfoPtr->Size + fFormationFBInterval)/2;
				}
				break;
			default:
				{
					GNPCRotateTargetVectorHorizontal(&vec3Right90, &pManager->TargetVector, MSD_LEFT_90, 0);
					vec3NewPos = pManager->PositionVector
						- pManager->TargetVector*(pManager->MonsterInfoPtr->Size + fFormationFBInterval)*5/2
						+ vec3Right90*(pManager->MonsterInfoPtr->Size + fFormationFBInterval)/2;
				}
			}
		}
		break;
	case FORMATION_INVERTED_BELL:
		{
			switch(pMons->m_byPartyFormationIndex)
			{
			case 0:
				{
					GNPCRotateTargetVectorHorizontal(&vec3Right90, &pManager->TargetVector, MSD_LEFT_90, 0);
					vec3NewPos = pManager->PositionVector
						+ vec3Right90*(pManager->MonsterInfoPtr->Size + fFormationFBInterval);
				}
				break;
			case 1:
				{
					GNPCRotateTargetVectorHorizontal(&vec3Left90, &pManager->TargetVector, MSD_RIGHT_90, 0);
					vec3NewPos = pManager->PositionVector
						- pManager->TargetVector*(pManager->MonsterInfoPtr->Size + fFormationFBInterval)
						+ vec3Left90*(pManager->MonsterInfoPtr->Size + fFormationFBInterval);
				}
				break;
			case 2:
				{
					vec3NewPos = pManager->PositionVector
						- pManager->TargetVector*(pManager->MonsterInfoPtr->Size + fFormationFBInterval);
				}
				break;
			case 3:
				{
					GNPCRotateTargetVectorHorizontal(&vec3Right90, &pManager->TargetVector, MSD_LEFT_90, 0);
					vec3NewPos = pManager->PositionVector
						- pManager->TargetVector*(pManager->MonsterInfoPtr->Size + fFormationFBInterval)
						+ vec3Right90*(pManager->MonsterInfoPtr->Size + fFormationFBInterval);
				}
				break;
			case 4:
				{
					GNPCRotateTargetVectorHorizontal(&vec3Right90, &pManager->TargetVector, MSD_LEFT_90, 0);
					vec3NewPos = pManager->PositionVector
						- pManager->TargetVector*(pManager->MonsterInfoPtr->Size + fFormationFBInterval)
						+ vec3Right90*(pManager->MonsterInfoPtr->Size + fFormationFBInterval)*2;
				}
				break;
			case 5:
				{
					GNPCRotateTargetVectorHorizontal(&vec3Left90, &pManager->TargetVector, MSD_RIGHT_90, 0);
					vec3NewPos = pManager->PositionVector
						- pManager->TargetVector*(pManager->MonsterInfoPtr->Size + fFormationFBInterval)*3/2
						+ vec3Left90*(pManager->MonsterInfoPtr->Size + fFormationFBInterval)/2;
				}
				break;
			case 6:
				{
					GNPCRotateTargetVectorHorizontal(&vec3Right90, &pManager->TargetVector, MSD_LEFT_90, 0);
					vec3NewPos = pManager->PositionVector
						- pManager->TargetVector*(pManager->MonsterInfoPtr->Size + fFormationFBInterval)*3/2
						+ vec3Right90*(pManager->MonsterInfoPtr->Size + fFormationFBInterval)/2;
				}
				break;
			case 7:
				{
					GNPCRotateTargetVectorHorizontal(&vec3Right90, &pManager->TargetVector, MSD_LEFT_90, 0);
					vec3NewPos = pManager->PositionVector
						- pManager->TargetVector*(pManager->MonsterInfoPtr->Size + fFormationFBInterval)*3/2
						+ vec3Right90*(pManager->MonsterInfoPtr->Size + fFormationFBInterval)*3/2;
				}
				break;
			default:
				{
					GNPCRotateTargetVectorHorizontal(&vec3Right90, &pManager->TargetVector, MSD_LEFT_90, 0);
					vec3NewPos = pManager->PositionVector
						- pManager->TargetVector*(pManager->MonsterInfoPtr->Size + fFormationFBInterval)*5/2
						+ vec3Right90*(pManager->MonsterInfoPtr->Size + fFormationFBInterval)/2;
				}
			}
		}
		break;
	default:
		{	// FORMATION_COLUMN°ú °°°Ô Ăł¸®
			if(1 == pMons->m_byPartyFormationIndex%2)
			{
				vec3NewPos = pManager->PositionVector - pManager->TargetVector*pManager->MonsterInfoPtr->Size*(1 + (pMons->m_byPartyFormationIndex>>1))
					- pManager->TargetVector*fFormationFBInterval*(1 + (pMons->m_byPartyFormationIndex>>1));
			}
			else
			{
				GNPCRotateTargetVectorHorizontal(&vec3Right90, &pManager->TargetVector, MSD_LEFT_90, 0);
				vec3Right90 = pManager->PositionVector + vec3Right90*pManager->MonsterInfoPtr->Size + vec3Right90*fFormationFBInterval;
				vec3NewPos = vec3Right90 - pManager->TargetVector*pManager->MonsterInfoPtr->Size*(pMons->m_byPartyFormationIndex>>1)
					- pManager->TargetVector*fFormationFBInterval*(pMons->m_byPartyFormationIndex>>1);
			}
		}
	}

	pMons->TargetVector = pManager->TargetVector;
	fDist = D3DXVec3Length(&(vec3NewPos - pMons->PositionVector));
	if(fDist > MONSTER_MAX_PARTY_DISTANCE/2)
	{
		D3DXVec3Normalize(&vec3Right90, &(vec3NewPos - pMons->PositionVector));
		pMons->PositionVector = pMons->PositionVector + vec3Right90 * fDist/2;
	}
	else
	{
		pMons->PositionVector = vec3NewPos;
	}
}

// 2009-09-09 ~ 2010-01-13 by dhjin, ŔÎÇÇ´ĎĆĽ - ĽŇČŻ °ř°Ýµµ HPAction»çżë °ˇ´ÉÇĎ°Ô ĽöÁ¤, ąŘ°ú °°ŔĚ CMonster -> CNPCMonster·Î ĽöÁ¤
// BOOL CNPCMapChannel::NPCMonsterAttackSkill(CMonster *pMonster, ITEM *pSkillItem)
BOOL CNPCMapChannel::NPCMonsterAttackSkill(CNPCMonster *pMonster, ITEM *pSkillItem)
{
// 2009-04-21 by cmkwon, ITEMżˇ DesParam ÇĘµĺ °łĽö 8°ł·Î ´Ă¸®±â - ľĆ·ˇżÍ °°ŔĚ ĽöÁ¤
//	switch(pSkillItem->DestParameter1)
	switch(pSkillItem->ArrDestParameter[0])
	{
	case DES_SUMMON:
		{
			// 2009-09-09 ~ 2010-01-13 by dhjin, ŔÎÇÇ´ĎĆĽ - ĽŇČŻ °ř°Ýµµ HPAction»çżë °ˇ´ÉÇĎ°Ô ĽöÁ¤
			pMonster->m_HPAction.SetSuccessAttackItemIdxHPRate();
			HPACTION_TALK_HPRATE MsgTalk;
			MEMSET_ZERO(&MsgTalk, sizeof(HPACTION_TALK_HPRATE));
			if(pMonster->m_HPAction.GetHPTalkAttack(pSkillItem->ItemNum, MsgTalk.HPTalk, &MsgTalk.HPCameraTremble)) {
				// Attack°Ş ŔüĽŰ.
				MsgTalk.HPTalkImportance	= HPACTION_TALK_IMPORTANCE_CHANNEL;
				this->SendFSvrHPTalk(pMonster, &MsgTalk);
			}

			// start 2011-03-30 by hskim, ŔÎÇÇ´ĎĆĽ 3Â÷ - ¸ó˝şĹÍ Č®·üŔű ĽŇČŻ ±â´É Ăß°ˇ - Č®ŔÎ ČÄ »đŔÔ
			//float nRandVal = RANDF1(0.0f, 100.0f);		// ĽŇĽöÁˇ±îÁö Ăł¸®ÇŇ ÇĘżä°ˇ ŔÖŔ»±î?

			//DBGOUT("[%d][%s] ·Ł´ý ĽŇČŻ Č®·ü [%4.2f > %4.2f]", pSkillItem->ItemNum, pSkillItem->ItemName, pSkillItem->HitRate, nRandVal);
			//if( nRandVal > pSkillItem->HitRate )
			//{		
			//	DBGOUT("==> ĽŇČŻ ˝ÇĆĐ\n");
			//	break;
			//}
			//DBGOUT("====================> ĽŇČŻ Ľş°ř\n");
			// end 2011-03-30 by hskim, ŔÎÇÇ´ĎĆĽ 3Â÷ - ¸ó˝şĹÍ Č®·üŔű ĽŇČŻ ±â´É Ăß°ˇ

			MSG_FN_ADMIN_SUMMON_MONSTER tmSummonMonster;
			tmSummonMonster.ChannelIndex			= m_MapChannelIndex.ChannelIndex;
			STRNCPY_MEMSET(tmSummonMonster.CharacterName, pMonster->MonsterInfoPtr->MonsterName, SIZE_MAX_CHARACTER_NAME);
// 2009-04-21 by cmkwon, ITEMżˇ DesParam ÇĘµĺ °łĽö 8°ł·Î ´Ă¸®±â - ľĆ·ˇżÍ °°ŔĚ ĽöÁ¤
//			tmSummonMonster.MonsterUnitKind			= pSkillItem->ParameterValue1;
			tmSummonMonster.MonsterUnitKind			= pSkillItem->ArrParameterValue[0];
			tmSummonMonster.MonsterTargetType1		= MONSTER_TARGETTYPE_NORMAL;
			tmSummonMonster.TargetTypeData1			= 0;
			tmSummonMonster.CltIdxForTargetType1	= 0;

			// 2010. 06. 08 by hsLee ŔÎÇÇ´ĎĆĽ ÇĘµĺ 2Â÷ ł­ŔĚµµ Á¶Ŕý. (ľĆ±ş µżŔĎ ąë·±˝ş Ŕűżë.) - ĽŇČŻµÉ ¸ó˝şĹÍ Ľöżˇ ąë·±˝ş Ŕűżë.
			//tmSummonMonster.NumOfMonster			= max(1, pSkillItem->MultiNum);
			tmSummonMonster.NumOfMonster			= max( 1 , (INT)(pSkillItem->MultiNum * pMonster->MonsterInfoBalance.fSummonCountRatio) );

			// start 2011-05-02 by hskim, ŔÎÇÇ´ĎĆĽ 3Â÷ - ˝Ăł×¸¶ °ü·Ă ±â´É Ăß°ˇ - ĆŻÁ¤ ÁÂÇĄżˇ ĽŇČŻ
			//////////
			// ±âÁ¸ //
			//tmSummonMonster.Position				= pMonster->PositionVector;

			//////////
			// ĽöÁ¤ //
			D3DXVECTOR3 fSummonPosition(0.0f, 0.0f, 0.0f);

			fSummonPosition.x						= pSkillItem->GetParameterValue(DES_SUMMON_POSITION_X);
			fSummonPosition.y						= pSkillItem->GetParameterValue(DES_SUMMON_POSITION_Y);
			fSummonPosition.z						= pSkillItem->GetParameterValue(DES_SUMMON_POSITION_Z);

			tmSummonMonster.Position				= pMonster->PositionVector + fSummonPosition;
			// end 2011-05-02 by hskim, ŔÎÇÇ´ĎĆĽ 3Â÷ - ˝Ăł×¸¶ °ü·Ă ±â´É Ăß°ˇ - ĆŻÁ¤ ÁÂÇĄżˇ ĽŇČŻ

			// 2010. 06. 08 by hsLee ŔÎÇÇ´ĎĆĽ ÇĘµĺ 2Â÷ ł­ŔĚµµ Á¶Ŕý. (ľĆ±ş µżŔĎ ąë·±˝ş Ŕűżë.) - ¸ó˝şĹÍŔÇ ¸ó˝şĹÍ ĽŇČŻ˝Ă ąë·±˝ş Ŕűżë.
			tmSummonMonster.MonsterBalanceData		= pMonster->MonsterInfoBalance;

			NPCOnAdminSummonMonster(&tmSummonMonster, pMonster->m_nTargetIndex);
		}
		break;
	default:
		{
			return FALSE;
		}
	}
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			BOOL CNPCMapChannel::NPCMonsterAttackSkill(CMonster *pMonster, SSUMMON_EVENT_MONSTER *i_pSummonEvMon)
/// \brief		// 2008-04-16 by cmkwon, ¸ó˝şĹÍ »ç¸Á ˝Ă ¸ó˝şĹÍ ĽŇČŻ ŔĚşĄĆ® ˝Ă˝şĹŰ ±¸Çö - CNPCMapChannel::NPCMonsterAttackSkill() Ăß°ˇ
/// \author		cmkwon
/// \date		2008-04-16 ~ 2008-04-16
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CNPCMapChannel::NPCMonsterAttackSkill(CMonster *pMonster, SSUMMON_EVENT_MONSTER *i_pSummonEvMon)
{
	MSG_FN_ADMIN_SUMMON_MONSTER tmSummonMonster;
	tmSummonMonster.ChannelIndex			= m_MapChannelIndex.ChannelIndex;
	STRNCPY_MEMSET(tmSummonMonster.CharacterName, pMonster->MonsterInfoPtr->MonsterName, SIZE_MAX_CHARACTER_NAME);
	tmSummonMonster.MonsterUnitKind			= i_pSummonEvMon->SummonMonsterNum;
	tmSummonMonster.MonsterTargetType1		= MONSTER_TARGETTYPE_NORMAL;
	tmSummonMonster.TargetTypeData1			= 0;
	tmSummonMonster.CltIdxForTargetType1	= 0;
	tmSummonMonster.NumOfMonster			= max(1, i_pSummonEvMon->SummonMonsterCount);
	tmSummonMonster.Position				= pMonster->PositionVector;

	// 2010. 06. 08 by hsLee ŔÎÇÇ´ĎĆĽ ÇĘµĺ 2Â÷ ł­ŔĚµµ Á¶Ŕý. (ľĆ±ş µżŔĎ ąë·±˝ş Ŕűżë.) - ¸ó˝şĹÍŔÇ ¸ó˝şĹÍ ĽŇČŻ˝Ă ąë·±˝ş Ŕűżë.
	tmSummonMonster.MonsterBalanceData		= pMonster->MonsterInfoBalance;

	return NPCOnAdminSummonMonster(&tmSummonMonster, pMonster->m_nTargetIndex);
}

void CNPCMapChannel::NPCMonsterMPOption(CNPCMonster *pMonster)
{
	if(COMPARE_MPOPTION_BIT(pMonster->MonsterInfoPtr->MPOption, MPOPTION_BIT_MOVE_PARTY))
	{	// ĆÄĆĽ

		if(pMonster->MonsterInfoPtr->MPOptionParam2 > MONSTER_MAX_PARTY_MEMBER_COUNTS)
		{
			pMonster->MonsterInfoPtr->MPOptionParam2 = MONSTER_MAX_PARTY_MEMBER_COUNTS;
		}
		if(pMonster->m_nPartyManagerIndex)
		{
			CNPCMonster *pPartyMon = GetNPCMonster(pMonster->m_nPartyManagerIndex);
			if(NULL == pPartyMon
				|| pPartyMon->m_enMonsterState == MS_NULL
				|| COMPARE_BODYCON_BIT(pPartyMon->BodyCondition, BODYCON_DEAD_MASK)
				|| pPartyMon->m_byPartyMemberCounts <= 0
				|| MONSTER_MAX_PARTY_DISTANCE + 50.0f < D3DXVec3Length(&(pPartyMon->PositionVector - pMonster->PositionVector))
				|| pPartyMon->m_nPartyManagerIndex != pPartyMon->MonsterIndex)
			{
				pMonster->m_nPartyManagerIndex		= 0;
				pMonster->m_byPartyMemberCounts		= 0;
				pMonster->m_byPartyFormationIndex	= 0xFF;
			}
			else if(pMonster->m_nPartyManagerIndex == pMonster->MonsterIndex
				&& pMonster->m_byPartyMemberCounts < pMonster->MonsterInfoPtr->MPOptionParam2 - 1)
			{// ŔÚ˝ĹŔĚ ĆÄĆĽŔĺŔĚ¸éĽ­ ĆÄĆĽżřŔĚ ¸¸żřŔĚ ľĆ´Ď´Ů, ĂĘ´ë °ˇ´ÉÇÔ

				vector<ClientIndex_t>	vecClientTemp;
				vecClientTemp.reserve(10);
				this->NPCGetAdjacentMonsterIndexes(&pMonster->PositionVector, MONSTER_MAX_PARTY_DISTANCE, ((int)MONSTER_MAX_PARTY_DISTANCE)<<1, &vecClientTemp);
				vector<ClientIndex_t>::iterator itr = vecClientTemp.begin();
				while(itr != vecClientTemp.end())
				{
					CNPCMonster *pMTemp = GetNPCMonster(*itr);
					if(pMTemp
						&& pMTemp->m_enMonsterState != MS_NULL
						&& pMTemp->m_nTargetIndex == 0					
						&& COMPARE_BODYCON_BIT(pMTemp->BodyCondition, BODYCON_DEAD_MASK) == FALSE
						&& pMTemp->MonsterInfoPtr->MonsterUnitKind == pMonster->MonsterInfoPtr->MonsterUnitKind
						&& pMTemp->m_nPartyManagerIndex == 0
						&& pMTemp->m_nTargetIndex == 0
						&& D3DXVec3Length(&(pMonster->PositionVector - pMTemp->PositionVector)) < MONSTER_MAX_PARTY_DISTANCE
						&& pMonster->m_dwCurrentTick%100 < 50)
					{
						pMTemp->m_nPartyManagerIndex = pMonster->m_nPartyManagerIndex;
						pMTemp->m_byPartyFormationIndex = pMonster->m_byPartyMemberCounts;
						pMonster->m_byPartyMemberCounts++;
						break;
					}
					itr++;
				}
			}
		}
		else if(0 == pMonster->m_nTargetIndex)
		{// ĆÄĆĽżˇ Âüż©ÁßŔĚ ľĆ´Ď¸éĽ­ Ĺ¸°ŮŔĚ ľř´Ů, ĆÄĆĽ »ýĽşŔĚ °ˇ´ÉÇĎ°í ĂĘ´ë°ˇ °ˇ´ÉÇĎ´Ů

			vector<ClientIndex_t>	vecClientTemp;
			vecClientTemp.reserve(10);
			this->NPCGetAdjacentMonsterIndexes(&pMonster->PositionVector, MONSTER_MAX_PARTY_DISTANCE, ((int)MONSTER_MAX_PARTY_DISTANCE)<<1, &vecClientTemp);
			vector<ClientIndex_t>::iterator itr = vecClientTemp.begin();
			while(itr != vecClientTemp.end())
			{
				CNPCMonster *pMTemp = this->GetNPCMonster(*itr);
				if(pMonster != pMTemp
					&& pMTemp
					&& pMTemp->m_enMonsterState != MS_NULL
					&& pMTemp->m_nTargetIndex == 0
					&& COMPARE_BODYCON_BIT(pMTemp->BodyCondition, BODYCON_DEAD_MASK) == FALSE
					&& pMTemp->MonsterInfoPtr->MonsterUnitKind == pMonster->MonsterInfoPtr->MonsterUnitKind
					&& pMTemp->m_nPartyManagerIndex == 0
					&& pMTemp->m_nTargetIndex == 0
					&& D3DXVec3Length(&(pMonster->PositionVector - pMTemp->PositionVector)) < MONSTER_MAX_PARTY_DISTANCE
					&& pMonster->m_dwCurrentTick%100 < 50)
				{
					//////////////////////////////////////////////////////
					// Ç×»ó ŔÚ˝ĹŔĚ ĆÄĆĽŔĺŔĚ µČ´Ů
					pMonster->m_nPartyManagerIndex = pMonster->MonsterIndex;
					pMTemp->m_nPartyManagerIndex = pMonster->m_nPartyManagerIndex;
					pMTemp->m_byPartyFormationIndex = 0;
					pMonster->m_byPartyMemberCounts = 1;
					break;
				}
				itr++;
			}
		}
	}
}


///////////////////////////////////////////////////////////////////////////////
// ČŁ´ĎľČ Äý °ř°Ý°ü·Ă »çÇ×
//	1 : ĆŢ˝ş°ř°Ý
//		ŔĎąÝŔűŔÎ »óČ˛ŔÓŔ¸·Î ş°´Ů¸Ą ±¸Çö »çÇ× ľřŔ˝
//	2 : ŔüĂĽ°ř°Ý
//		°ř°Ý ąüŔ§ ł»żˇ 5¸í ŔĚ»ó Á¸ŔçÇĎ´Â °ćżě
//	3 : ĽŇČŻ°ř°Ý
//		¸ó˝şĹÍ¸¦ °ř°ÝÇĎ°í ŔÖ´Â ŔŻ´ÖŔĚ 10¸í ŔĚ»óŔÎ °ćżě , HP°ˇ 50% ŔĚ»ó ±ďŔÎ ŔĚČÄ 10% °¨ĽŇµÉ °ćżě
//
// ¸¶żîĆľ ĽĽŔĚÁö °ř°Ý°ü·Ă »çÇ×
//	1 : şŇ°ř°Ý
//		ŔĎąÝŔűŔÎ »óČ˛ŔÓŔ¸·Î ş°´Ů¸Ą ±¸Çö »çÇ× ľřŔ˝
//	2 : şö°ř°Ý
//		°ř°Ý ąüŔ§ ł»żˇ 5¸í ŔĚ»ó Á¸ŔçÇĎ´Â °ćżě
BOOL CNPCMapChannel::CheckMonsterSelectedItem(CNPCMonster * i_pnMonster)
{
	// 2010-06-09 by dhjin, NPCĽ­ąö Á×´Â ąö±× ĽöÁ¤ - ş¸ľČ
	if(NULL == i_pnMonster->m_pUsingMonsterItem->pItemInfo)
	{
		g_pNPCGlobal->WriteSystemLogEX(TRUE,"@CheckMonsterSelectedItem MonsterNum [%d]\r\n",i_pnMonster->MonsterInfoPtr->MonsterUnitKind);
		return FALSE;	
	}

	vector<ClientIndex_t>	vecClientTemp;
	vecClientTemp.reserve(10);

// 2009-04-21 by cmkwon, ITEMżˇ DesParam ÇĘµĺ °łĽö 8°ł·Î ´Ă¸®±â - ľĆ·ˇżÍ °°ŔĚ ĽöÁ¤
//	if(DES_SUMMON == i_pnMonster->m_pUsingMonsterItem->pItemInfo->DestParameter1)
	if(DES_SUMMON == i_pnMonster->m_pUsingMonsterItem->pItemInfo->ArrDestParameter[0])
	{// ĽŇČŻ °ř°Ý ¸ó˝şĹÍ Ăł¸®

// 2009-04-21 by cmkwon, ITEMżˇ DesParam ÇĘµĺ °łĽö 8°ł·Î ´Ă¸®±â - ľĆ·ˇżÍ °°ŔĚ ĽöÁ¤
// 		NPCGetAdjacentMonsterIndexes(&i_pnMonster->PositionVector, i_pnMonster->MonsterInfoPtr->Range
// 			, i_pnMonster->MonsterInfoPtr->Range*2, &vecClientTemp, (INT)i_pnMonster->m_pUsingMonsterItem->pItemInfo->ParameterValue1);
		NPCGetAdjacentMonsterIndexes(&i_pnMonster->PositionVector, i_pnMonster->MonsterInfoPtr->Range
			, i_pnMonster->MonsterInfoPtr->Range*2, &vecClientTemp, (INT)i_pnMonster->m_pUsingMonsterItem->pItemInfo->ArrParameterValue[0]);
		
		if(vecClientTemp.size() >= i_pnMonster->m_pUsingMonsterItem->pItemInfo->ShotNum*2)
		{// ĽŇČŻ ´É·ÂŔÇ 2ąčş¸´Ů ¸ąŔĚ ĽŇČŻ µÇľîŔÖŔ¸¸é FALSE¸¦ ¸®ĹĎÇŃ´Ů

			return FALSE;
		}
	}

// 2005-12-13 by cmkwon, »čÁ¦ÇÔ
//	switch(i_pnMonster->m_pUsingMonsterItem->pItemInfo->ItemNum)
//	{
//	case 2004400:	// ¸¶żîĆ®ĽĽŔĚÁö şö°ř°Ý
//		{
//			if(i_pnMonster->CurrentHP > i_pnMonster->MonsterInfoPtr->MonsterHP/5)
//			{// CurrentHP°ˇ HP/5 ş¸´Ů Ĺ¬¶§´Â ÁÖŔ§żˇ Äł¸ŻŔĚ 5¸í ŔĚ»óŔĎ ¶§¸¸ »çżë °ˇ´É	
//				
//				NPCGetAdjacentCharacterIndexes(&i_pnMonster->PositionVector, i_pnMonster->m_pUsingMonsterItem->pItemInfo->Range
//					, i_pnMonster->m_pUsingMonsterItem->pItemInfo->Range*2, &vecClientTemp);
//				if(vecClientTemp.size() < 5)
//				{// ÁÖŔ§żˇ Äł¸ŻĹÍ°ˇ 5¸íŔĚ»óŔĚľîľß »çżë °ˇ´É
//
//					return FALSE;
//				}
//			}
//		}
//		break;
//	case 7500670:	// ČŁ´ĎľČÄý ŔüĂĽ°ř°Ý
//		{
//			///////////////////////////////////////////////////////////////////////////////
//			// ¸ó˝şĹÍ HP°ˇ 1/2ŔĚÇĎ ŔĎ¶§ »çżë °ˇ´É
//			if(i_pnMonster->CurrentHP > i_pnMonster->MonsterInfoPtr->MonsterHP/2)
//			{
//				return FALSE;
//			}
//		}
//	}

	return TRUE;
}


BOOL CNPCMapChannel::Send2FieldServerW(BYTE *pData, int nSize)
{
	return m_pNPCMapProject->Send2FieldServer(pData, nSize);
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			BOOL CNPCMapChannel::Send2FieldServerByTCPW(BYTE *pData, int nSize)
/// \brief		// 2007-11-26 by cmkwon, ¸ó˝şĹÍ ŔÚµż»čÁ¦ ¸Ţ˝ĂÁö TCP·Î ŔüĽŰ(N->F) - CNPCMapChannel::Send2FieldServerByTCPW() Ăß°ˇ
/// \author		cmkwon
/// \date		2007-11-26 ~ 2007-11-26
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CNPCMapChannel::Send2FieldServerByTCPW(BYTE *pData, int nSize)
{
	return m_pNPCMapProject->Send2FieldServerByTCP(pData, nSize);
}

BOOL CNPCMapChannel::SetInitialPositionAndSendCreateMonster(CNPCMonster *i_pMons, BYTE *i_pBufSend, D3DXVECTOR3 *i_pSummonPos/*=NULL*/)
{
	if(FALSE == SetInitialPosition(i_pMons->PositionVector.x, i_pMons->PositionVector.z, i_pMons->MonsterIndex))
	{
		char szTemp[256];
		sprintf(szTemp, "[Error] SetInitialPosition_1 Error, MapChannel(%s) UnitIndex(%5d) XZ(%5.0f, %5.0f)\n"
			, GET_MAP_STRING(this->m_MapChannelIndex), i_pMons->MonsterIndex
			, i_pMons->PositionVector.x, i_pMons->PositionVector.z);
		DBGOUT(szTemp);
		g_pNPCGlobal->WriteSystemLog(szTemp);
	}
	INIT_MSG(MSG_FN_MONSTER_CREATE, T_FN_MONSTER_CREATE, pSMonsterCreate, i_pBufSend);
	pSMonsterCreate->ChannelIndex		= this->m_MapChannelIndex.ChannelIndex;
	pSMonsterCreate->MonsterIndex		= i_pMons->MonsterIndex;
	pSMonsterCreate->MonsterUnitKind	= i_pMons->MonsterInfoPtr->MonsterUnitKind;
	pSMonsterCreate->MonsterTargetType1	= i_pMons->m_byMonsterTargetType;
	pSMonsterCreate->TargetTypeData1	= i_pMons->m_nTargetTypeData;
	i_pMons->m_mtvectClientIdxForTargetType.lock();
	if(false == i_pMons->m_mtvectClientIdxForTargetType.empty())
	{
		pSMonsterCreate->CltIdxForTargetType1	= i_pMons->m_mtvectClientIdxForTargetType[0];
	}
	i_pMons->m_mtvectClientIdxForTargetType.unlock();
	pSMonsterCreate->BodyCondition		= i_pMons->BodyCondition;
	pSMonsterCreate->PositionVector		= i_pMons->PositionVector;
	if(i_pSummonPos)
	{
		pSMonsterCreate->PositionVector	= *i_pSummonPos;
	}
	pSMonsterCreate->TargetVector		= i_pMons->TargetVector*1000.0f;
	pSMonsterCreate->ObjectMonsterType	= i_pMons->m_byObjectMonsterType;

	// 2010. 05. 19 by hsLee ŔÎÇÇ´ĎĆĽ ÇĘµĺ 2Â÷ ł­ŔĚµµ Á¶Ŕý. (˝ĹČŁĂł¸® + ¸ó˝şĹÍ Ăł¸®(Ľ­ąö) )
	pSMonsterCreate->MonsterBalanceData = i_pMons->MonsterInfoBalance;
	// End 2010. 05. 19 by hsLee ŔÎÇÇ´ĎĆĽ ÇĘµĺ 2Â÷ ł­ŔĚµµ Á¶Ŕý. (˝ĹČŁĂł¸® + ¸ó˝şĹÍ Ăł¸®(Ľ­ąö) )
	
	return Send2FieldServerW(i_pBufSend, MSG_SIZE(MSG_FN_MONSTER_CREATE));
}

// żŔşęÁ§Ć® ¸ó˝şĹÍ °ü·Ă
OBJECTINFOSERVER *CNPCMapChannel::FindObjectMonsterInfoByObjectEventIndex(int i_nObjectEventIndex)
{
	vectorObjectInfoServer::iterator itr(m_vectorObjectMonsterInfoCopy.begin());
	for(; itr != m_vectorObjectMonsterInfoCopy.end(); itr++)
	{
		if(itr->m_EventInfo.m_EventwParam1 == i_nObjectEventIndex)
		{
			return &*itr;
		}
	}
	return NULL;
}

// CityWar Object Monster
// 2006-11-22 by cmkwon, ÇÔĽö¸í şŻ°ć(FindCityWarObjectMonsterInfo->FindObjectBossMonsterInfo)
OBJECTINFOSERVER *CNPCMapChannel::FindObjectBossMonsterInfo(void)
{
	vectorObjectInfoServer::iterator itr(m_vectorObjectMonsterInfoCopy.begin());
	for(; itr != m_vectorObjectMonsterInfoCopy.end(); itr++)
	{
// 2006-11-22 by cmkwon, ľĆ·ˇżÍ °°ŔĚ ĽöÁ¤ÇÔ		if(itr->m_EventInfo.m_byIsCityWarMonster)
		if(itr->m_EventInfo.m_byBossMonster)		// 2006-11-22 by cmkwon, şŻĽö¸í şŻ°ć(m_byIsCityWarMonster->m_byBossMonster) - µµ˝ĂÁˇ·ÉŔüŔş ľřľîÁü
		{
			return &*itr;
		}
	}
	return NULL;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			OBJECTINFOSERVER *CNPCMapChannel::FindObjectMonsterInfoBYBelligerence(BYTE i_byBellig)
/// \brief		// 2007-08-18 by cmkwon, żŔşęÁ§Ć® ¸ó˝şĹÍ ĽŇČŻ Á¤ş¸żˇ MONSTER_INFO * ĽłÁ¤ÇĎ±â - ÇÔĽöĂß°ˇ
/// \author		cmkwon
/// \date		2007-08-18 ~ 2007-08-18
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
OBJECTINFOSERVER *CNPCMapChannel::FindObjectMonsterInfoBYBelligerence(BYTE i_byBellig)
{
	vectorObjectInfoServer::iterator itr(m_vectorObjectMonsterInfoCopy.begin());
	for(; itr != m_vectorObjectMonsterInfoCopy.end(); itr++)
	{
		if(itr->m_pMonsterInfo
			&& itr->m_pMonsterInfo->Belligerence == i_byBellig)
		{
			return &*itr;
		}
	}
	return NULL;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			BOOL CNPCMapChannel::GetObjectMonsterByMonsterIdx(INT i_nMonsterIdx, OBJECTINFOSERVER * o_pMonsterInfo)
/// \brief		ŔÎÇÇ´ĎĆĽ(±âÁöąćľî) - 
///				// 2010-03-31 by cmkwon, ŔÎÇÇ2Â÷ Ăß°ˇ ĽöÁ¤ - 
/// \author		dhjin
/// \date		2010-03-31
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CNPCMapChannel::GetObjectMonsterByMonsterIdx(INT i_nMonsterIdx, vectorObjectInfoServerPtr *o_pObjectInfoServList)
{
	vectorObjectInfoServer::iterator itr(m_vectorObjectMonsterInfoCopy.begin());
	for(; itr != m_vectorObjectMonsterInfoCopy.end(); itr++)
	{
		OBJECTINFOSERVER *pObjInfoServ = &*itr;
		if(pObjInfoServ->m_pMonsterInfo
			&& pObjInfoServ->m_pMonsterInfo->MonsterUnitKind == i_nMonsterIdx)
		{
			o_pObjectInfoServList->push_back(pObjInfoServ);
		}
	}	

	return o_pObjectInfoServList->size();
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			BOOL CNPCMapChannel::IsEnableCreateMonster(MONSTER_INFO *i_pMonInfo)
/// \brief		
/// \author		cmkwon
/// \date		2006-04-18 ~ 2006-04-18
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CNPCMapChannel::IsEnableCreateMonster(MONSTER_INFO *i_pMonInfo)
{
	if(FALSE == IS_INFLWAR_MONSTER(i_pMonInfo->Belligerence))
	{
		return TRUE;
	}
	
	if(IS_MAP_INFLUENCE_ARENA(this->GetMapInfluenceTypeW()))
	{// 2007-05-16 by cmkwon, ľĆ·ąłŞ¸ĘŔĚ°ćżě Ç×»ó TRUE
		return TRUE;
	}

	if(IS_STRATEGYPOINT_MONSTER(i_pMonInfo->Belligerence)
		&& 0 != this->GetMapChannelIndex().ChannelIndex)
	{// 2006-11-21 by cmkwon, Ŕü·«Ć÷ŔÎĆ® ¸ó˝şĹÍ´Â 0ąř Ă¤łÎżˇ¸¸ »ýĽşµČ´Ů.
		return FALSE;
	}

	if(IS_BELL_VCN(i_pMonInfo->Belligerence)
		&& (IS_MAP_INFLUENCE_VCN(m_pNPCMapProject->m_nMapInfluenceType)
			|| IS_MAP_INFLUENCE_OUTPOST(m_pNPCMapProject->m_nMapInfluenceType)))
	{// 2007-09-07 by dhjin, ŔüÁř±âÁö ¸ĘżˇĽ­ ĽĽ·Â ¸ó˝şĹÍ´Â »ýĽşÇŃ´Ů.
		return TRUE;
	}
	else if(IS_BELL_ANI(i_pMonInfo->Belligerence)
		&& (IS_MAP_INFLUENCE_ANI(m_pNPCMapProject->m_nMapInfluenceType)
			|| IS_MAP_INFLUENCE_OUTPOST(m_pNPCMapProject->m_nMapInfluenceType)))
	{
		return TRUE;
	}

	if(IS_TELEPORT_MONSTER(i_pMonInfo->Belligerence))
	{// 2007-09-19 by dhjin, ĹÚ·ąĆ÷Ć®ŔĚ¸é ĽŇČŻ
		return TRUE;
	}

	///////////////////////////////////////////////////////////////////////////////
	// 2006-04-18 by cmkwon
	return m_pNPCIOCPServer->CheckSummonJacoMonster(i_pMonInfo->Belligerence);
}


///////////////////////////////////////////////////////////////////////////////
/// \fn			void CNPCMapChannel::DeleteAllMonster(BOOL i_bAll/*=TRUE*/, int i_byBell1/*=-1*/, int i_byBell2/*=-1*/, int i_byExcludeBell1/*=-1*/, int i_byExcludeBell2/*=-1*/)
/// \brief		// 2007-08-22 by cmkwon, ÇŘ´ç ¸ĘĂ¤łÎ ¸ó˝şĹÍ ¸đµÎ »čÁ¦ÇĎ±â ±â´É Ăß°ˇ
/// \author		cmkwon
/// \date		2007-08-22 ~ 2007-08-22
/// \warning	MapWorker ˝ş·ąµĺżˇĽ­ ČŁĂâ ÇŘľßÇÔ
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CNPCMapChannel::DeleteAllMonster(BOOL i_bAll/*=TRUE*/, int i_byBell1/*=-1*/, int i_byBell2/*=-1*/, int i_byExcludeBell1/*=-1*/, int i_byExcludeBell2/*=-1*/)
{
	///////////////////////////////////////////////////////////////////////////////
	// ¸ĘľČŔÇ ¸đµç ¸ó˝şĹÍ¸¦ Á¦°ĹÇŃ´Ů
	mt_auto_lock mtA(&m_mtvectorMonsterPtr);
	INIT_MSG_WITH_BUFFER(MSG_FN_MONSTER_DELETE, T_FN_MONSTER_DELETE, pMonsterDelete, SendBuf);
	pMonsterDelete->ChannelIndex	= m_MapChannelIndex.ChannelIndex;
	pMonsterDelete->CinemaDelete	= FALSE;		// 2011-05-30 by hskim, ŔÎÇÇ´ĎĆĽ 3Â÷ - ¸ó˝şĹÍ »čÁ¦ Ĺ¬¶óŔĚľđĆ® ąÝżµ

	int nSize = m_mtvectorMonsterPtr.size();
	for(int i=0; i < nSize; i++)
	{
		CNPCMonster		*pNMon = (CNPCMonster*)m_mtvectorMonsterPtr[i];
		if(NULL == pNMon
			|| MS_NULL == pNMon->m_enMonsterState)
		{
			continue;
		}

		MONSTER_INFO	*pMonInfo = pNMon->MonsterInfoPtr;
		if(NULL == pMonInfo)
		{
			continue;
		}

		if(FALSE == i_bAll)
		{// ŔüĂĽ »čÁ¦ ż©şÎ ÇĂ·ą±× ĂĽĹ©

			if(0 <= i_byExcludeBell1 || 0 <= i_byExcludeBell2)
			{// żążÜ BellŔĚ ĽłÁ¤µÇľî ŔÖŔ˝

				if(pMonInfo->Belligerence == i_byExcludeBell1
					|| pMonInfo->Belligerence == i_byExcludeBell2)
				{
					continue;
				}
			}

			if(0 <= i_byBell1 || 0 <= i_byBell2)
			{// 2007-08-22 by cmkwon, bellŔĚ ÇĎłŞ¶óµµ ĽłÁ¤ŔĚ µÇľî ŔÖ´Ů¸é 

				if(pMonInfo->Belligerence != i_byBell1
					&& pMonInfo->Belligerence != i_byBell2)
				{
					continue;
				}
			}
		}
		
		pMonsterDelete->MonsterIndex	= pNMon->MonsterIndex;
		Send2FieldServerW(SendBuf, MSG_SIZE(MSG_FN_MONSTER_DELETE));

		this->DelelteMonsterHandler(pNMon);		
	}
}

// start 2011-04-28 by hskim, ŔÎÇÇ´ĎĆĽ 3Â÷ - ˝Ăł×¸¶ °ü·Ă ±â´É Ăß°ˇ - ÇŘ´ç ¸ĘĂ¤łÎ ĆŻÁ¤ ¸ó˝şĹÍ »čÁ¦/şŻ°ć ±â´É Ăß°ˇ
///////////////////////////////////////////////////////////////////////////////
/// \fn			void CNPCMapChannel::DeleteUnitKindMonster(INT iMonsterUnitKind)
/// \brief		// 2011-04-28 by hskim, ŔÎÇÇ´ĎĆĽ 3Â÷ - ˝Ăł×¸¶ °ü·Ă ±â´É Ăß°ˇ - ÇŘ´ç ¸ĘĂ¤łÎ ĆŻÁ¤ ¸ó˝şĹÍ »čÁ¦ ±â´É Ăß°ˇ
/// \author		hskim
/// \date		2011-04-28
/// \warning	MapWorker ˝ş·ąµĺżˇĽ­ ČŁĂâ ÇŘľßÇÔ
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CNPCMapChannel::DeleteUnitKindMonster(INT iMonsterUnitKind)
{
	mt_auto_lock mtA(&m_mtvectorMonsterPtr);
	INIT_MSG_WITH_BUFFER(MSG_FN_MONSTER_DELETE, T_FN_MONSTER_DELETE, pMonsterDelete, SendBuf);
	pMonsterDelete->ChannelIndex	= m_MapChannelIndex.ChannelIndex;
	pMonsterDelete->CinemaDelete	= TRUE;		// 2011-05-30 by hskim, ŔÎÇÇ´ĎĆĽ 3Â÷ - ¸ó˝şĹÍ »čÁ¦ Ĺ¬¶óŔĚľđĆ® ąÝżµ
	
	int nSize = m_mtvectorMonsterPtr.size();
	for(int i=0; i < nSize; i++)
	{
		CNPCMonster		*pNMon = (CNPCMonster*)m_mtvectorMonsterPtr[i];
		if(NULL == pNMon
			|| MS_NULL == pNMon->m_enMonsterState)
		{
			continue;
		}
		
		MONSTER_INFO	*pMonInfo = pNMon->MonsterInfoPtr;
		if(NULL == pMonInfo)
		{
			continue;
		}

		if( pNMon->MonsterInfoPtr->MonsterUnitKind != iMonsterUnitKind ) 
		{
			continue;
		}

		pMonsterDelete->MonsterIndex	= pNMon->MonsterIndex;
		Send2FieldServerW(SendBuf, MSG_SIZE(MSG_FN_MONSTER_DELETE));

		this->DelelteMonsterHandler(pNMon);		
	}
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			void CNPCMapChannel::ChangeUnitKindMonster(INT iMonsterUnitKind, INT iChangeMonsterUnitKind)
/// \brief		// 2011-05-11 by hskim, ŔÎÇÇ´ĎĆĽ 3Â÷ - ˝Ăł×¸¶ °ü·Ă ±â´É Ăß°ˇ - ÇŘ´ç ¸ĘĂ¤łÎ ĆŻÁ¤ ¸ó˝şĹÍ şŻ°ć ±â´É Ăß°ˇ
/// \author		hskim
/// \date		2011-05-11
/// \warning	MapWorker ˝ş·ąµĺżˇĽ­ ČŁĂâ ÇŘľßÇÔ
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CNPCMapChannel::ChangeUnitKindMonster(INT iMonsterUnitKind, INT iChangeMonsterUnitKind)
{
	mt_auto_lock mtA(&m_mtvectorMonsterPtr);
	INIT_MSG_WITH_BUFFER(MSG_FN_MONSTER_CHANGE_OK, T_FN_MONSTER_CHANGE_OK, pMonsterChange, SendBuf);
	pMonsterChange->ChannelIndex	= m_MapChannelIndex.ChannelIndex;

	MONSTER_INFO *pChangeMonInfo = m_pNPCIOCPServer->GetMonsterInfo(iChangeMonsterUnitKind);
	if(NULL == pChangeMonInfo) return ;

	int nSize = m_mtvectorMonsterPtr.size();
	for(int i=0; i < nSize; i++)
	{
		CNPCMonster		*pNMon = (CNPCMonster*)m_mtvectorMonsterPtr[i];
		if(NULL == pNMon
			|| MS_NULL == pNMon->m_enMonsterState)
		{
			continue;
		}

		MONSTER_INFO	*pMonInfo = pNMon->MonsterInfoPtr;
		if(NULL == pMonInfo)
		{
			continue;
		}
		
		if( pNMon->MonsterInfoPtr->MonsterUnitKind != iMonsterUnitKind ) 
		{
			continue;
		}
		
		// ¸ó˝şĹÍ Á¤ş¸ ±łĂĽ
		pNMon->MonsterInfoPtr = pChangeMonInfo;
		
		pMonsterChange->MonsterIndex	= pNMon->MonsterIndex;
		pMonsterChange->ChangeMonsterUnitKind	= iChangeMonsterUnitKind;
		Send2FieldServerW(SendBuf, MSG_SIZE(MSG_FN_MONSTER_CHANGE_OK));
		
		g_pNPCGlobal->WriteSystemLogEX(TRUE, "  %10s : <= ChangeOK MonIndex[%4d] MonsterUnitKind[%8d] ChangeMonsterUnitKind[%d]\r\n",
			GET_MAP_STRING(m_MapChannelIndex), pMonsterChange->MonsterIndex,pNMon->MonsterInfoPtr->MonsterUnitKind, iChangeMonsterUnitKind);
	}
}
// end 2011-04-28 by hskim, ŔÎÇÇ´ĎĆĽ 3Â÷ - ˝Ăł×¸¶ °ü·Ă ±â´É Ăß°ˇ - ÇŘ´ç ¸ĘĂ¤łÎ ĆŻÁ¤ ¸ó˝şĹÍ »čÁ¦/şŻ°ć ±â´É Ăß°ˇ

///////////////////////////////////////////////////////////////////////////////
/// \fn			void CNPCMapChannel::SetNotCreateMonsterValue(bool i_bNotCreateMonster)
/// \brief		¸ó˝şĹÍ »ýĽş °ˇ´É ż©şÎ ĽłÁ¤
/// \author		dhjin
/// \date		2007-08-29 ~ 2007-08-29
/// \warning	
///
/// \param		
/// \return		
/////////////////////////////////////////////////////////////////////////////////
void CNPCMapChannel::SetNotCreateMonsterValue(bool i_bNotCreateMonster)
{
	// 2009-12-04 by cmkwon, ¸ó˝şĹÍ »ýĽş±ÝÁö °ü·Ă ·Î±× Ăß°ˇ - 
	g_pNPCGlobal->WriteSystemLogEX(TRUE, "  [Notify] MapChannel(%d:%d) CNPCMapChannel::SetNotCreateMonsterValue# CurValue(%d) NewValue(%d) \r\n"
		, this->GetMapChannelIndex().MapIndex, this->GetMapChannelIndex().ChannelIndex, m_bNotCreateMonster, i_bNotCreateMonster);

	m_bNotCreateMonster = i_bNotCreateMonster;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			OBJECTINFOSERVER *CNPCMapChannel::GetTeleportObjectMonsterSummonInfo(CNPCMonster *i_pNMon)
/// \brief		// 2007-09-20 by cmkwon, ĹÚ·ąĆ÷Ć® ĽŇČŻ °ü·Ă ĽöÁ¤
/// \author		cmkwon
/// \date		2007-09-20 ~ 2007-09-20
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
OBJECTINFOSERVER *CNPCMapChannel::GetTeleportObjectMonsterSummonInfo(CNPCMonster *i_pNMon)
{
	if(FALSE == i_pNMon->m_byObjectMonsterType)
	{
		return NULL;
	}

	OBJECTINFOSERVER *pObjInfoServ = this->FindObjectMonsterInfoByObjectEventIndex(i_pNMon->m_dwIndexCreatedMonsterData);
	if(NULL == pObjInfoServ
		|| FALSE == IS_TELEPORT_MONSTER(pObjInfoServ->m_pMonsterInfo->Belligerence))
	{
		return NULL;
	}
	
	return pObjInfoServ;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		// 2009-12-11 by cmkwon, µĄąĚÁö ľî±×·Î·Î Ĺ¸°ŮŔ» şŻ°ćÇĎ´Â ¸ó˝şĹÍ ±¸Çö - 
/// \author		cmkwon
/// \date		2009-12-16 ~ 2009-12-16
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CNPCMapChannel::ChangeTarget(CNPCMonster *i_pNMon)
{
	MONSTER_INFO *pMonInfo = i_pNMon->MonsterInfoPtr;
	if(NULL == pMonInfo
		|| FALSE == i_pNMon->IsValidMonster())		// 2010-03-16 by cmkwon, ŔÎÇÇ2Â÷ MtoM, MtoC Ĺ¸°Ů şŻ°ć °ü·Ă ĽöÁ¤ - ĂĽĹ© Ăß°ˇ
	{
		return FALSE;
	}

// 2009-12-10 by cmkwon, TEMP 
//	g_pNPCGlobal->WriteSystemLogEX(TRUE, "[TEMP] infi2 100317 10000 CNPCMapChannel::ChangeTarget# Map(%s) MonIdx(%d) MonsNum(%d) ChangeTarget(%d) CurTargetIndex(%d) NewTargetIdx(%d) CurTick(%d) nAttackerCnt(%d) ChangeTargetTime(%d) \r\n"
//		, GetMapString(this->GetMapChannelIndex(), string()), i_pNMon->MonsterIndex, i_pNMon->MonsterInfoPtr->MonsterUnitKind, i_pNMon->MonsterInfoPtr->ChangeTarget, i_pNMon->m_nTargetIndex, 0, i_pNMon->m_dwCurrentTick, i_pNMon->m_mtvectorAttackedInfoPtr.size(), i_pNMon->MonsterInfoPtr->ChangeTargetTime);

// 2010-03-16 by cmkwon, ŔÎÇÇ2Â÷ MtoM, MtoC Ĺ¸°Ů şŻ°ć °ü·Ă ĽöÁ¤ - 
// 	// 2009-12-16 by cmkwon, TargetIndex şŻ°ćµĘŔ» ĽłÁ¤ÇŃ´Ů.
// 	i_pNMon->SetChangedTargetIndexTick(i_pNMon->m_dwCurrentTick);		// 2009-12-11 by cmkwon, µĄąĚÁö ľî±×·Î·Î Ĺ¸°ŮŔ» şŻ°ćÇĎ´Â ¸ó˝şĹÍ ±¸Çö - 
	///////////////////////////////////////////////////////////////////////////////
	// 2010-03-16 by cmkwon, ŔÎÇÇ2Â÷ MtoM, MtoC Ĺ¸°Ů şŻ°ć °ü·Ă ĽöÁ¤ - CNPCMapChannel::ChangeTarget#
	i_pNMon->SetTimeLastCheckChangeTarget(i_pNMon->m_dwCurrentTick);	

	ClientIndex_t newTargetIdx = 0;
	///////////////////////////////////////////////////////////////////////////////
	// 2010-03-31 by dhjin, ŔÎÇÇ´ĎĆĽ(±âÁöąćľî) - ľĆ±ş Ĺ¸°Ů ŔâľĆľß µÇ´Â ˝şĹłĹ¸°ŮĹ¸ŔÔľĆŔĚĹŰŔĚ¸é ż©±âĽ­ Ĺ¸°Ů Ăł¸®~!!!
	newTargetIdx = this->GetOurTagetByUsingItem(i_pNMon);
	if(FALSE != newTargetIdx)
	{
		i_pNMon->SelectTargetIndex(newTargetIdx);
		return newTargetIdx;
	}

	if(ATTACKOBJ_AGGRO == pMonInfo->AttackObject)
	{
		newTargetIdx = i_pNMon->GetMaxDamageUser();
		i_pNMon->ResetSumDamageInAttackedInfoList();	// 2009-12-11 by cmkwon, ±âÁ¸ µĄąĚÁö¸¦ ĂĘ±âČ­ ÇŃ´Ů.

// 2009-12-10 by cmkwon, TEMP 
//		g_pNPCGlobal->WriteSystemLogEX(TRUE, "[TEMP] infi2 100317 11000 CNPCMapChannel::ChangeTarget# Map(%s) MonIdx(%d) MonsNum(%d) ChangeTarget(%d) CurTargetIndex(%d) NewTargetIdx(%d) CurTick(%d) nAttackerCnt(%d) \r\n"
//			, GetMapString(this->GetMapChannelIndex(), string()), i_pNMon->MonsterIndex, i_pNMon->MonsterInfoPtr->MonsterUnitKind, i_pNMon->MonsterInfoPtr->ChangeTarget, i_pNMon->m_nTargetIndex, newTargetIdx, i_pNMon->m_dwCurrentTick, i_pNMon->m_mtvectorAttackedInfoPtr.size());
	}
	else
	{
		///////////////////////////////////////////////////////////////////////////////
		// 2010-04-14 by cmkwon, ŔÎÇÇ2Â÷ ¸ó˝şĹÍ ·ŁĹŇ Ĺ¸°Ů şŻ°ć Ăł¸® - ATTACKOBJ_RANDOM ŔĚ¸é 30%¸¸ ˝ÇÁ¦ Ĺ¸°ŮşŻ°ćŔ» Ăł¸®ÇŃ´Ů.
		int nAttObjRandomVal = RAND100();
		if(ATTACKOBJ_RANDOM == pMonInfo->AttackObject
			&& 30 <= nAttObjRandomVal)
		{
// 2009-12-10 by cmkwon, TEMP 
//			g_pNPCGlobal->WriteSystemLogEX(TRUE, "[TEMP] infi2 100317 12000 CNPCMapChannel::ChangeTarget# Map(%s) MonIdx(%d) MonsNum(%d) ChangeTarget(%d) AttObj(%d) RandVal(%d) CurTargetIndex(%d) NewTargetIdx(%d) CurTick(%d)\r\n"
//				, GetMapString(this->GetMapChannelIndex(), string()), i_pNMon->MonsterIndex, i_pNMon->MonsterInfoPtr->MonsterUnitKind, i_pNMon->MonsterInfoPtr->ChangeTarget, i_pNMon->MonsterInfoPtr->AttackObject, nAttObjRandomVal, i_pNMon->m_nTargetIndex, newTargetIdx, i_pNMon->m_dwCurrentTick);

			return FALSE;
		}

		vectorClientIndex vectTargetableList;
		
		int tmClientCnt = 0;

		if(IS_BELL_ATTACK(pMonInfo->Belligerence))
		{
			// 2009-12-16 by cmkwon, ÁÖŔ§ Äł¸ŻĹÍŔÇ ¸®˝şĆ® °Ë»ö
			this->NPCGetAdjacentCharacterIndexes(&i_pNMon->PositionVector, pMonInfo->AttackRange, pMonInfo->AttackRange*2, &vectTargetableList, pMonInfo->Belligerence);

			tmClientCnt = vectTargetableList.size();
		}

		// 2009-12-16 by cmkwon, ¸ó˝şĹÍ¸¦ °Ë»öÇŃ ¸®Ć®˝ş °Ë»ö
		i_pNMon->GetAttackedInfoClientIndexList(&vectTargetableList);		

		newTargetIdx = this->NPCGetTargetwithAttackObj(pMonInfo->AttackObject, i_pNMon, vectTargetableList);

// 2009-12-10 by cmkwon, TEMP 
//		g_pNPCGlobal->WriteSystemLogEX(TRUE, "[TEMP] infi2 100317 13000 CNPCMapChannel::ChangeTarget# Map(%s) MonIdx(%d) MonsNum(%d) ChangeTarget(%d) AttObj(%d) RandVal(%d) CurTargetIndex(%d) NewTargetIdx(%d) CurTick(%d) nAttackerCnt(%d) ClientCnt(%d) \r\n"
//			, GetMapString(this->GetMapChannelIndex(), string()), i_pNMon->MonsterIndex, i_pNMon->MonsterInfoPtr->MonsterUnitKind, i_pNMon->MonsterInfoPtr->ChangeTarget, i_pNMon->MonsterInfoPtr->AttackObject, nAttObjRandomVal, i_pNMon->m_nTargetIndex, newTargetIdx, i_pNMon->m_dwCurrentTick, vectTargetableList.size(), tmClientCnt);
	}

	if(0 == newTargetIdx
		|| newTargetIdx == i_pNMon->GetTargetIndex())
	{
		return FALSE;
	}

	i_pNMon->SelectTargetIndex(newTargetIdx);

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			OBJECTINFOSERVER *FindObjectMonsterInfoByMonsterUniqueNumberAndMinimumDistance(DWORD i_dwMonsterUniqueNumber, D3DXVECTOR3 *i_pVec3Position)
/// \brief		
/// \author		cmkwon
/// \date		2004-11-26 ~ 2004-11-26
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
OBJECTINFOSERVER *CNPCMapChannel::FindObjectMonsterInfoByMonsterUniqueNumberAndMinimumDistance(INT i_nMonsterUniqueNumber
																							   , D3DXVECTOR3 *i_pVec3Position)
{
	OBJECTINFOSERVER	*pRetObj = NULL;
	float				fMinimumDistance = 300.0f;

	vectorObjectInfoServer::iterator itr(m_vectorObjectMonsterInfoCopy.begin());
	for(; itr != m_vectorObjectMonsterInfoCopy.end(); itr++)
	{
		if(itr->m_EventInfo.m_nObejctMonsterUnitKind == i_nMonsterUniqueNumber)
		{
			float ftmDist = D3DXVec3Length(&(*i_pVec3Position - itr->m_vPos));
			if(ftmDist < fMinimumDistance)
			{
				pRetObj = &*itr;
				ftmDist = fMinimumDistance;
			}
		}
	}

	return pRetObj;
}



///////////////////////////////////////////////////////////////////////////////
/// \fn			BOOL CNPCMapChannel::CheckMonsterPosition(CNPCMonster *pMon, float fTimeRate)
/// \brief		
/// \author		cmkwon
/// \date		2004-04-06 ~ 2004-04-06
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CNPCMapChannel::CheckMonsterPosition(CNPCMonster *pMon, float fTimeRate)
{
	if(FALSE == pMon->m_MoveInfo.MovableFlag)
	{
		return TRUE;
	}

	if(FALSE == m_pNPCMapProject->IsValidPosition(pMon->PositionVector.x, pMon->PositionVector.z))
	{	// XÁÂÇĄżÍ YÁÂÇĄ°ˇ mapŔÇ ąüŔ§¸¦ ąţľîłŞ´ÂÁö¸¦ ¸ŐŔú Č®ŔÎÇŃ´Ů

		pMon->SetMoveState(MSS_NORMAL);
		pMon->PositionVector = pMon->m_BeforePosition;
		GNPCRotateTargetVectorHorizontal(&pMon->TargetVector, &pMon->TargetVector, pMon->m_MoveInfo.LRDirect * MSD_LEFT_90, 45);
		return FALSE;
	}

	///////////////////////////////////////////////////////////////////////////////
	// ¸ŐŔú ObjecżÍ Ăćµż Ăł¸®¸¦ ÇŃ´Ů.
	CheckAndModifyImpactPositionObjects(pMon);
	if(FALSE == m_pNPCMapProject->IsValidPosition(pMon->PositionVector.x, pMon->PositionVector.z))
	{// XÁÂÇĄżÍ YÁÂÇĄ°ˇ mapŔÇ ąüŔ§¸¦ ąţľîłŞ´ÂÁö¸¦ ¸ŐŔú Č®ŔÎÇŃ´Ů
			
		pMon->PositionVector = pMon->m_BeforePosition;
		GNPCRotateTargetVectorHorizontal(&pMon->TargetVector, &pMon->TargetVector, pMon->m_MoveInfo.LRDirect * MSD_LEFT_90, 45);
		return FALSE;
	}

	///////////////////////////////////////////////////////////////////////////////
	// 1. Áö»ó ¸ó˝şĹÍŔÇ łôŔĚ ĽłÁ¤
	// 2. łŞ¸ÓÁö ¸ó˝şĹÍŔÇ łôŔĚ ĽłÁ¤
	//		- ŔŻżµ şńÇŕÇüŔÇ ¸ĘłôŔĚ´Â ą°łôŔĚ¸¦ Ć÷ÇÔÇĎÁö ľĘ´Â łôŔĚŔĚ´Ů
	//		- ¸ó˝şĹÍŔÇ łôŔĚ°ˇ ¸ĘłôŔĚ ş¸´Ů ł·Ŕ¸¸é ¸ĘłôŔĚ·Î łôż©ÁŘ´Ů.	
	if(pMon->CurrentMonsterForm == FORM_GROUND_MOVE)
	{	// Áö»ó ¸ó˝şĹÍŔÇ łôŔĚ ĽłÁ¤

		float fMapHeight = m_pNPCMapProject->GetMapHeightIncludeWater(&pMon->PositionVector);			// ÁÂÇĄŔÇ ¸ĘŔÇ łôŔĚ¸¦ ±¸ÇŃ´Ů.
		pMon->PositionVector.y = fMapHeight + pMon->MonsterInfoPtr->Size;

		///////////////////////////////////////////////////////////////////////////////
		// Áö»ó ¸ó˝şĹÍŔÇ PositionŔĚ şŻ°ćµÇľúŔ¸ąÇ·Î żŔşęÁ§Ć® Ăćµą Ăł¸®¸¦ ÇŃąř´ő ÇŃ´Ů
		CheckAndModifyImpactPositionObjects(pMon);
	}
	else
	{
		float fMapHeight;
		if(pMon->CurrentMonsterForm == FORM_SWIMMINGFLYING_RIGHT
			|| pMon->CurrentMonsterForm == FORM_SWIMMINGFLYING_COPTER)
		{
			fMapHeight = m_pNPCMapProject->GetMapHeightExcludeWater(&pMon->PositionVector);			// ą°łôŔĚ°ˇ Á¦żÜµČ ÁÂÇĄŔÇ ¸ĘŔÇ łôŔĚ¸¦ ±¸ÇŃ´Ů.
		}
		else
		{
			fMapHeight = m_pNPCMapProject->GetMapHeightIncludeWater(&pMon->PositionVector);			// ą°łôŔĚ°ˇ Ć÷ÇÔµČ ÁÂÇĄŔÇ ¸ĘŔÇ łôŔĚ¸¦ ±¸ÇŃ´Ů.
		}

		float fTmMinHeight = fMapHeight + pMon->MonsterInfoPtr->Size;
		if(pMon->PositionVector.y-fTmMinHeight < 0.0f)
		{	// ÇöŔç ¸ó˝şĹÍ ÁÂÇĄ°ˇ (¸ĘłôŔĚ + ¸ó˝şĹÍ »çŔĚÁî) ş¸´Ů ŔŰ´Ů
		
			pMon->PositionVector.y = fTmMinHeight;
		
			///////////////////////////////////////////////////////////////////////////////
			// PositionŔĚ şŻ°ćµÇľúŔ¸ąÇ·Î żŔşęÁ§Ć® Ăćµą Ăł¸®¸¦ ÇŃąř´ő ÇŃ´Ů
			CheckAndModifyImpactPositionObjects(pMon);
		}
	}

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			BOOL CNPCMapChannel::CheckMonsterPositionWarp(CNPCMonster *pMon, float fTimeRate)
/// \brief		ŔÎÇÇ´ĎĆĽ - 
/// \author		dhjin
/// \date		2009-09-09 ~ 2010
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CNPCMapChannel::CheckMonsterPositionWarp(CNPCMonster *pMon, float fTimeRate) {
	if(FALSE == m_pNPCMapProject->IsValidPosition(pMon->PositionVector.x, pMon->PositionVector.z))
	{	// XÁÂÇĄżÍ YÁÂÇĄ°ˇ mapŔÇ ąüŔ§¸¦ ąţľîłŞ´ÂÁö¸¦ ¸ŐŔú Č®ŔÎÇŃ´Ů
		
		pMon->SetMoveState(MSS_NORMAL);
		pMon->PositionVector = pMon->m_BeforePosition;
		GNPCRotateTargetVectorHorizontal(&pMon->TargetVector, &pMon->TargetVector, pMon->m_MoveInfo.LRDirect * MSD_LEFT_90, 45);
		return FALSE;
	}
	
	///////////////////////////////////////////////////////////////////////////////
	// ¸ŐŔú ObjecżÍ Ăćµż Ăł¸®¸¦ ÇŃ´Ů.
	CheckAndModifyImpactPositionObjects(pMon);
	if(FALSE == m_pNPCMapProject->IsValidPosition(pMon->PositionVector.x, pMon->PositionVector.z))
	{// XÁÂÇĄżÍ YÁÂÇĄ°ˇ mapŔÇ ąüŔ§¸¦ ąţľîłŞ´ÂÁö¸¦ ¸ŐŔú Č®ŔÎÇŃ´Ů
		
		pMon->PositionVector = pMon->m_BeforePosition;
		GNPCRotateTargetVectorHorizontal(&pMon->TargetVector, &pMon->TargetVector, pMon->m_MoveInfo.LRDirect * MSD_LEFT_90, 45);
		return FALSE;
	}
	
	///////////////////////////////////////////////////////////////////////////////
	// 1. Áö»ó ¸ó˝şĹÍŔÇ łôŔĚ ĽłÁ¤
	// 2. łŞ¸ÓÁö ¸ó˝şĹÍŔÇ łôŔĚ ĽłÁ¤
	//		- ŔŻżµ şńÇŕÇüŔÇ ¸ĘłôŔĚ´Â ą°łôŔĚ¸¦ Ć÷ÇÔÇĎÁö ľĘ´Â łôŔĚŔĚ´Ů
	//		- ¸ó˝şĹÍŔÇ łôŔĚ°ˇ ¸ĘłôŔĚ ş¸´Ů ł·Ŕ¸¸é ¸ĘłôŔĚ·Î łôż©ÁŘ´Ů.	
	if(pMon->CurrentMonsterForm == FORM_GROUND_MOVE)
	{	// Áö»ó ¸ó˝şĹÍŔÇ łôŔĚ ĽłÁ¤
		
		float fMapHeight = m_pNPCMapProject->GetMapHeightIncludeWater(&pMon->PositionVector);			// ÁÂÇĄŔÇ ¸ĘŔÇ łôŔĚ¸¦ ±¸ÇŃ´Ů.
		pMon->PositionVector.y = fMapHeight + pMon->MonsterInfoPtr->Size;
		
		///////////////////////////////////////////////////////////////////////////////
		// Áö»ó ¸ó˝şĹÍŔÇ PositionŔĚ şŻ°ćµÇľúŔ¸ąÇ·Î żŔşęÁ§Ć® Ăćµą Ăł¸®¸¦ ÇŃąř´ő ÇŃ´Ů
		CheckAndModifyImpactPositionObjects(pMon);
	}
	else
	{
		float fMapHeight;
		if(pMon->CurrentMonsterForm == FORM_SWIMMINGFLYING_RIGHT
			|| pMon->CurrentMonsterForm == FORM_SWIMMINGFLYING_COPTER)
		{
			fMapHeight = m_pNPCMapProject->GetMapHeightExcludeWater(&pMon->PositionVector);			// ą°łôŔĚ°ˇ Á¦żÜµČ ÁÂÇĄŔÇ ¸ĘŔÇ łôŔĚ¸¦ ±¸ÇŃ´Ů.
		}
		else
		{
			fMapHeight = m_pNPCMapProject->GetMapHeightIncludeWater(&pMon->PositionVector);			// ą°łôŔĚ°ˇ Ć÷ÇÔµČ ÁÂÇĄŔÇ ¸ĘŔÇ łôŔĚ¸¦ ±¸ÇŃ´Ů.
		}
		
		float fTmMinHeight = fMapHeight + pMon->MonsterInfoPtr->Size;
		if(pMon->PositionVector.y-fTmMinHeight < 0.0f)
		{	// ÇöŔç ¸ó˝şĹÍ ÁÂÇĄ°ˇ (¸ĘłôŔĚ + ¸ó˝şĹÍ »çŔĚÁî) ş¸´Ů ŔŰ´Ů
			
			pMon->PositionVector.y = fTmMinHeight;
			
			///////////////////////////////////////////////////////////////////////////////
			// PositionŔĚ şŻ°ćµÇľúŔ¸ąÇ·Î żŔşęÁ§Ć® Ăćµą Ăł¸®¸¦ ÇŃąř´ő ÇŃ´Ů
			CheckAndModifyImpactPositionObjects(pMon);
		}
	}
	
	return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
// ÇÔĽöŔÚ Ĺ¬·ˇ˝ş
//
///////////////////////////////////////////////////////////////////////////////
class for_each_functor_MSG_FN_MONSTER_CREATE_OK
{
public:
	for_each_functor_MSG_FN_MONSTER_CREATE_OK(CNPCMapChannel *i_pNMapChann)
		:m_pNMapChannel(i_pNMapChann)
	{
	};
	void operator()(MSG_FN_MONSTER_CREATE_OK createOK)
	{
		m_pNMapChannel->ProcessNPCOnMonsterCreateOK(&createOK);
	}
	CNPCMapChannel			*m_pNMapChannel;
};

class for_each_functor_MSG_FN_MONSTER_DELETE
{
public:
	for_each_functor_MSG_FN_MONSTER_DELETE(CNPCMapChannel *i_pNMapChann)
		:m_pNMapChannel(i_pNMapChann)
	{
	};
	void operator()(MSG_FN_MONSTER_DELETE MonDel)
	{
		m_pNMapChannel->ProcessNPCOnMonsterDelete(&MonDel);
	}
	CNPCMapChannel			*m_pNMapChannel;
};
class for_each_functor_MSG_FN_MONSTER_SKILL_END_SKILL
{
public:
	for_each_functor_MSG_FN_MONSTER_SKILL_END_SKILL(CNPCMapChannel *i_pNMapChann)
		:m_pNMapChannel(i_pNMapChann)
	{
	};
	void operator()(MSG_FN_MONSTER_SKILL_END_SKILL endSkill)
	{
		m_pNMapChannel->ProcessNPCOnMonsterSkillEndSkill(&endSkill);
	}
	CNPCMapChannel			*m_pNMapChannel;
};
class for_each_functor_MSG_FN_BATTLE_SET_ATTACK_CHARACTER
{
public:
	for_each_functor_MSG_FN_BATTLE_SET_ATTACK_CHARACTER(CNPCMapChannel *i_pNMapChann)
		:m_pNMapChannel(i_pNMapChann)
	{
	};
	void operator()(MSG_FN_BATTLE_SET_ATTACK_CHARACTER i_SetAttack)
	{
		m_pNMapChannel->ProcessNPCOnBattleSetAttackCharacter(&i_SetAttack);
	}
	CNPCMapChannel			*m_pNMapChannel;
};
struct for_each_functor_MSG_FN_CITYWAR_START_WAR
{
	for_each_functor_MSG_FN_CITYWAR_START_WAR(CNPCMapChannel *i_pNMChann)
		:m_pNMapChannel(i_pNMChann)
	{
	};
	void operator()(MSG_FN_CITYWAR_START_WAR i_Msg)
	{
		m_pNMapChannel->ProcessNPCOnCityWarStartWar(&i_Msg);
	}
	CNPCMapChannel			*m_pNMapChannel;
};
struct for_each_functor_MSG_FN_CITYWAR_END_WAR
{
	for_each_functor_MSG_FN_CITYWAR_END_WAR(CNPCMapChannel *i_pNMChann)
		:m_pNMapChannel(i_pNMChann)
	{
	};
	void operator()(MSG_FN_CITYWAR_END_WAR i_Msg)
	{
		m_pNMapChannel->ProcessNPCOnCityWarEndWar(&i_Msg);
	}
	CNPCMapChannel			*m_pNMapChannel;
};
struct for_each_functor_MSG_FN_CITYWAR_CHANGE_OCCUPY_INFO
{
	for_each_functor_MSG_FN_CITYWAR_CHANGE_OCCUPY_INFO(CNPCMapChannel *i_pNMChann)
		:m_pNMapChannel(i_pNMChann)
	{
	};
	void operator()(MSG_FN_CITYWAR_CHANGE_OCCUPY_INFO i_Msg)
	{
		m_pNMapChannel->ProcessNPCOnCityWarChangeOccupyInfo(&i_Msg);
	}
	CNPCMapChannel			*m_pNMapChannel;
};
struct for_each_functor_MSG_FN_MONSTER_STRATEGYPOINT_INIT
{// 2006-11-20 by cmkwon
	for_each_functor_MSG_FN_MONSTER_STRATEGYPOINT_INIT(CNPCMapChannel *i_pNMChann)
		:m_pNMapChannel(i_pNMChann)
	{
	};
	void operator()(MSG_FN_MONSTER_STRATEGYPOINT_INIT i_Msg)
	{
		m_pNMapChannel->ProcessNPCOnMonsterStrategyPointInit(&i_Msg);
	}
	CNPCMapChannel			*m_pNMapChannel;
};

struct for_each_functor_MSG_FN_NPCSERVER_DELETE_MONSTER_IN_MAPCHANNEL		// 2007-08-22 by cmkwon, ÇŘ´ç ¸ĘĂ¤łÎ ¸ó˝şĹÍ ¸đµÎ »čÁ¦ÇĎ±â ±â´É Ăß°ˇ
{
	for_each_functor_MSG_FN_NPCSERVER_DELETE_MONSTER_IN_MAPCHANNEL(CNPCMapChannel *i_pNMChann)
		:m_pNMapChannel(i_pNMChann)
	{
	};
	void operator()(MSG_FN_NPCSERVER_DELETE_MONSTER_IN_MAPCHANNEL i_Msg)
	{
		m_pNMapChannel->ProcessNPCOnMonsterDeleteMonsterInMapChannel(&i_Msg);
	}
	CNPCMapChannel			*m_pNMapChannel;
};

struct for_each_functor_MSG_FN_MONSTER_OUTPOST_INIT
{// 2007-08-24 by dhjin
	for_each_functor_MSG_FN_MONSTER_OUTPOST_INIT(CNPCMapChannel *i_pNMChann)
		:m_pNMapChannel(i_pNMChann)
	{
	};
	void operator()(MSG_FN_MONSTER_OUTPOST_INIT i_Msg)
	{
		m_pNMapChannel->ProcessNPCOnMonsterOutPostInit(&i_Msg);
	}
	CNPCMapChannel			*m_pNMapChannel;
};

struct for_each_functor_MSG_MONSTER_SUMMON_BY_BELL
{// 2007-09-19 by cmkwon, Bell·Î ĽŇČŻ Ăł¸®
	for_each_functor_MSG_MONSTER_SUMMON_BY_BELL(CNPCMapChannel *i_pNMChann)
		:m_pNMapChannel(i_pNMChann)
	{
	};
	void operator()(MSG_MONSTER_SUMMON_BY_BELL i_Msg)
	{
		m_pNMapChannel->ProcessNPCOnMonsterSummonByBell(&i_Msg);
	}
	CNPCMapChannel			*m_pNMapChannel;
};

struct for_each_functor_MSG_FN_NPCSERVER_CINEMA_MONSTER_CREATE
{// 2010-03-31 by dhjin, ŔÎÇÇ´ĎĆĽ(±âÁöąćľî) - // 2009-09-09 ~ 2010 by dhjin, ŔÎÇÇ´ĎĆĽ - Key¸ó˝şĹÍ »ýĽş 
	for_each_functor_MSG_FN_NPCSERVER_CINEMA_MONSTER_CREATE(CNPCMapChannel *i_pNMChann)
		:m_pNMapChannel(i_pNMChann)
	{
	};
	void operator()(MSG_FN_NPCSERVER_CINEMA_MONSTER_CREATE i_Msg)
	{
		m_pNMapChannel->ProcessNPCOnMonsterCinemaMonsterCreate(&i_Msg);
	}
	CNPCMapChannel			*m_pNMapChannel;
};

// start 2011-04-28 by hskim, ŔÎÇÇ´ĎĆĽ 3Â÷ - ˝Ăł×¸¶ °ü·Ă ±â´É Ăß°ˇ - ÇŘ´ç ¸ĘĂ¤łÎ ĆŻÁ¤ ¸ó˝şĹÍ »čÁ¦/şŻ°ć ±â´É Ăß°ˇ
struct for_each_functor_MSG_FN_NPCSERVER_CINEMA_MONSTER_DESTROY
{
	for_each_functor_MSG_FN_NPCSERVER_CINEMA_MONSTER_DESTROY(CNPCMapChannel *i_pNMChann)
		:m_pNMapChannel(i_pNMChann)
	{
	};
	void operator()(MSG_FN_NPCSERVER_CINEMA_MONSTER_DESTROY i_Msg)
	{
		m_pNMapChannel->ProcessNPCOnMonsterCinemaMonsterDestroy(&i_Msg);
	}
	CNPCMapChannel			*m_pNMapChannel;
};

struct for_each_functor_MSG_FN_NPCSERVER_CINEMA_MONSTER_CHANGE
{
	for_each_functor_MSG_FN_NPCSERVER_CINEMA_MONSTER_CHANGE(CNPCMapChannel *i_pNMChann)
		:m_pNMapChannel(i_pNMChann)
	{
	};
	void operator()(MSG_FN_NPCSERVER_CINEMA_MONSTER_CHANGE i_Msg)
	{
		m_pNMapChannel->ProcessNPCOnMonsterCinemaMonsterChange(&i_Msg);
	}
	CNPCMapChannel			*m_pNMapChannel;
};
// end 2011-04-28 by hskim, ŔÎÇÇ´ĎĆĽ 3Â÷ - ˝Ăł×¸¶ °ü·Ă ±â´É Ăß°ˇ - ÇŘ´ç ¸ĘĂ¤łÎ ĆŻÁ¤ ¸ó˝şĹÍ »čÁ¦/şŻ°ć ±â´É Ăß°ˇ

void CNPCMapChannel::ProcessReceivedAllProtocol(void)
{
	m_mtvectorMSG_FN_MONSTER_CREATE_OK.lock();
	m_mtvectorMSG_FN_MONSTER_CREATE_OK.swap(m_mtvectorMSG_FN_MONSTER_CREATE_OKProcess);	
	m_mtvectorMSG_FN_MONSTER_CREATE_OK.unlock();
	
	m_mtvectorMSG_FN_MONSTER_DELETE.lock();
	m_mtvectorMSG_FN_MONSTER_DELETE.swap(m_mtvectorMSG_FN_MONSTER_DELETEProcess);
	m_mtvectorMSG_FN_MONSTER_DELETE.unlock();

	m_mtvectorMSG_FN_MONSTER_SKILL_END_SKILL.lock();
	m_mtvectorMSG_FN_MONSTER_SKILL_END_SKILL.swap(m_mtvectorMSG_FN_MONSTER_SKILL_END_SKILLProcess);	
	m_mtvectorMSG_FN_MONSTER_SKILL_END_SKILL.unlock();

	m_mtvectorMSG_FN_BATTLE_SET_ATTACK_CHARACTER.lock();
	m_mtvectorMSG_FN_BATTLE_SET_ATTACK_CHARACTER.swap(m_mtvectorMSG_FN_BATTLE_SET_ATTACK_CHARACTERProcess);
	m_mtvectorMSG_FN_BATTLE_SET_ATTACK_CHARACTER.unlock();
	
	for_each(m_mtvectorMSG_FN_MONSTER_CREATE_OKProcess.begin(), m_mtvectorMSG_FN_MONSTER_CREATE_OKProcess.end()
		, for_each_functor_MSG_FN_MONSTER_CREATE_OK(this));
	m_mtvectorMSG_FN_MONSTER_CREATE_OKProcess.clear();
	
	for_each(m_mtvectorMSG_FN_MONSTER_DELETEProcess.begin(), m_mtvectorMSG_FN_MONSTER_DELETEProcess.end()
		, for_each_functor_MSG_FN_MONSTER_DELETE(this));
	m_mtvectorMSG_FN_MONSTER_DELETEProcess.clear();

	for_each(m_mtvectorMSG_FN_MONSTER_SKILL_END_SKILLProcess.begin(), m_mtvectorMSG_FN_MONSTER_SKILL_END_SKILLProcess.end()
		, for_each_functor_MSG_FN_MONSTER_SKILL_END_SKILL(this));
	m_mtvectorMSG_FN_MONSTER_SKILL_END_SKILLProcess.clear();
	
	for_each(m_mtvectorMSG_FN_BATTLE_SET_ATTACK_CHARACTERProcess.begin(), m_mtvectorMSG_FN_BATTLE_SET_ATTACK_CHARACTERProcess.end()
		, for_each_functor_MSG_FN_BATTLE_SET_ATTACK_CHARACTER(this));
	m_mtvectorMSG_FN_BATTLE_SET_ATTACK_CHARACTERProcess.clear();

	m_mtvectorMSG_FN_CITYWAR_START_WAR.lock();
	for_each(m_mtvectorMSG_FN_CITYWAR_START_WAR.begin(), m_mtvectorMSG_FN_CITYWAR_START_WAR.end()
		, for_each_functor_MSG_FN_CITYWAR_START_WAR(this));
	m_mtvectorMSG_FN_CITYWAR_START_WAR.clear();
	m_mtvectorMSG_FN_CITYWAR_START_WAR.unlock();

	m_mtvectorMSG_FN_CITYWAR_END_WAR.lock();
	for_each(m_mtvectorMSG_FN_CITYWAR_END_WAR.begin(), m_mtvectorMSG_FN_CITYWAR_END_WAR.end()
		, for_each_functor_MSG_FN_CITYWAR_END_WAR(this));
	m_mtvectorMSG_FN_CITYWAR_END_WAR.clear();
	m_mtvectorMSG_FN_CITYWAR_END_WAR.unlock();

	m_mtvectorMSG_FN_CITYWAR_CHANGE_OCCUPY_INFO.lock();
	for_each(m_mtvectorMSG_FN_CITYWAR_CHANGE_OCCUPY_INFO.begin(), m_mtvectorMSG_FN_CITYWAR_CHANGE_OCCUPY_INFO.end()
		, for_each_functor_MSG_FN_CITYWAR_CHANGE_OCCUPY_INFO(this));
	m_mtvectorMSG_FN_CITYWAR_CHANGE_OCCUPY_INFO.clear();
	m_mtvectorMSG_FN_CITYWAR_CHANGE_OCCUPY_INFO.unlock();	

	// 2006-11-20 by cmkwon
	m_mtvectorMSG_FN_MONSTER_STRATEGYPOINT_INIT.lock();
	for_each(m_mtvectorMSG_FN_MONSTER_STRATEGYPOINT_INIT.begin(), m_mtvectorMSG_FN_MONSTER_STRATEGYPOINT_INIT.end()
		, for_each_functor_MSG_FN_MONSTER_STRATEGYPOINT_INIT(this));
	m_mtvectorMSG_FN_MONSTER_STRATEGYPOINT_INIT.clear();
	m_mtvectorMSG_FN_MONSTER_STRATEGYPOINT_INIT.unlock();	
	
	// 2007-08-22 by cmkwon, ÇŘ´ç ¸ĘĂ¤łÎ ¸ó˝şĹÍ ¸đµÎ »čÁ¦ÇĎ±â ±â´É Ăß°ˇ
	m_mtvectMSG_FN_NPCSERVER_DELETE_MONSTER_IN_MAPCHANNEL.lock();
	for_each(m_mtvectMSG_FN_NPCSERVER_DELETE_MONSTER_IN_MAPCHANNEL.begin(), m_mtvectMSG_FN_NPCSERVER_DELETE_MONSTER_IN_MAPCHANNEL.end()
		, for_each_functor_MSG_FN_NPCSERVER_DELETE_MONSTER_IN_MAPCHANNEL(this));
	m_mtvectMSG_FN_NPCSERVER_DELETE_MONSTER_IN_MAPCHANNEL.clear();
	m_mtvectMSG_FN_NPCSERVER_DELETE_MONSTER_IN_MAPCHANNEL.unlock();	

	// 2007-08-24 by dhjin, ŔüÁř±âÁö ¸ó˝şĹÍ °ü·Ă
	m_mtvectorMSG_FN_MONSTER_OUTPOST_INIT.lock();
	for_each(m_mtvectorMSG_FN_MONSTER_OUTPOST_INIT.begin(), m_mtvectorMSG_FN_MONSTER_OUTPOST_INIT.end()
		, for_each_functor_MSG_FN_MONSTER_OUTPOST_INIT(this));
	m_mtvectorMSG_FN_MONSTER_OUTPOST_INIT.clear();
	m_mtvectorMSG_FN_MONSTER_OUTPOST_INIT.unlock();	

	// 2007-09-19 by cmkwon, Bell·Î ĽŇČŻ Ăł¸®
	m_mtvectMSG_MONSTER_SUMMON_BY_BELL.lock();
	for_each(m_mtvectMSG_MONSTER_SUMMON_BY_BELL.begin(), m_mtvectMSG_MONSTER_SUMMON_BY_BELL.end()
		, for_each_functor_MSG_MONSTER_SUMMON_BY_BELL(this));
	m_mtvectMSG_MONSTER_SUMMON_BY_BELL.clear();
	m_mtvectMSG_MONSTER_SUMMON_BY_BELL.unlock();	

	// 2010-03-31 by dhjin, ŔÎÇÇ´ĎĆĽ(±âÁöąćľî) - // 2009-09-09 ~ 2010 by dhjin, ŔÎÇÇ´ĎĆĽ - Key¸ó˝şĹÍ »ýĽş
	m_mtvectorMSG_FN_NPCSERVER_CINEMA_MONSTER_CREATE.lock();
	for_each(m_mtvectorMSG_FN_NPCSERVER_CINEMA_MONSTER_CREATE.begin(), m_mtvectorMSG_FN_NPCSERVER_CINEMA_MONSTER_CREATE.end()
		, for_each_functor_MSG_FN_NPCSERVER_CINEMA_MONSTER_CREATE(this));
	m_mtvectorMSG_FN_NPCSERVER_CINEMA_MONSTER_CREATE.clear();
	m_mtvectorMSG_FN_NPCSERVER_CINEMA_MONSTER_CREATE.unlock();	

	// start 2011-04-28 by hskim, ŔÎÇÇ´ĎĆĽ 3Â÷ - ˝Ăł×¸¶ °ü·Ă ±â´É Ăß°ˇ - ÇŘ´ç ¸ĘĂ¤łÎ ĆŻÁ¤ ¸ó˝şĹÍ »čÁ¦ ±â´É Ăß°ˇ
	m_mtvectorMSG_FN_NPCSERVER_CINEMA_MONSTER_DESTROY.lock();
	for_each(m_mtvectorMSG_FN_NPCSERVER_CINEMA_MONSTER_DESTROY.begin(), m_mtvectorMSG_FN_NPCSERVER_CINEMA_MONSTER_DESTROY.end()
		, for_each_functor_MSG_FN_NPCSERVER_CINEMA_MONSTER_DESTROY(this));
	m_mtvectorMSG_FN_NPCSERVER_CINEMA_MONSTER_DESTROY.clear();
	m_mtvectorMSG_FN_NPCSERVER_CINEMA_MONSTER_DESTROY.unlock();
	// end 2011-04-28 by hskim, ŔÎÇÇ´ĎĆĽ 3Â÷ - ˝Ăł×¸¶ °ü·Ă ±â´É Ăß°ˇ - ÇŘ´ç ¸ĘĂ¤łÎ ĆŻÁ¤ ¸ó˝şĹÍ »čÁ¦ ±â´É Ăß°ˇ

	// start 2011-05-11 by hskim, ŔÎÇÇ´ĎĆĽ 3Â÷ - ˝Ăł×¸¶ °ü·Ă ±â´É Ăß°ˇ - ÇŘ´ç ¸ĘĂ¤łÎ ĆŻÁ¤ ¸ó˝şĹÍ şŻ°ć ±â´É Ăß°ˇ
	m_mtvectorMSG_FN_NPCSERVER_CINEMA_MONSTER_CHANGE.lock();
	for_each(m_mtvectorMSG_FN_NPCSERVER_CINEMA_MONSTER_CHANGE.begin(), m_mtvectorMSG_FN_NPCSERVER_CINEMA_MONSTER_CHANGE.end()
		, for_each_functor_MSG_FN_NPCSERVER_CINEMA_MONSTER_CHANGE(this));
	m_mtvectorMSG_FN_NPCSERVER_CINEMA_MONSTER_CHANGE.clear();
	m_mtvectorMSG_FN_NPCSERVER_CINEMA_MONSTER_CHANGE.unlock();
	// end 2011-05-11 by hskim, ŔÎÇÇ´ĎĆĽ 3Â÷ - ˝Ăł×¸¶ °ü·Ă ±â´É Ăß°ˇ - ÇŘ´ç ¸ĘĂ¤łÎ ĆŻÁ¤ ¸ó˝şĹÍ şŻ°ć ±â´É Ăß°ˇ
}


///////////////////////////////////////////////////////////////////////////////
/// \fn			BOOL CNPCMapChannel::IsProcessableWorker(DWORD i_dwCurTick)
/// \brief		// 2008-12-03 by cmkwon, NPCServer Ĺ¬¶óŔĚľđĆ® ľř´Â ¸ĘĂ¤łÎ Ăł¸® ĽöÁ¤ - 
/// \author		cmkwon
/// \date		2008-12-03 ~ 2008-12-03
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CNPCMapChannel::IsProcessableWorker(DWORD i_dwCurTick)
{
	if(FALSE == this->IsEnabled())
	{
		return FALSE;
	}

	if(FALSE == m_bExistUserInMapChannel)
	{
		// 2008-12-02 by cmkwon, ¸ĘĂ¤łÎżˇ ŔŻŔú°ˇ Á¸ŔçÇĎÁö ľĘŔ» ¶§żˇ´Â 1şĐżˇ ÇŃąřľż¸¸ TickŔ» Ăł¸®ÇŃ´Ů.
		int nElapseTick = i_dwCurTick - m_dwWorkeredTick;
		if(60000 > nElapseTick)
		{
			return FALSE;
		}
	}
	else
	{
		// 2008-12-02 by cmkwon, ¸ĹĂ¤łÎżˇ ŔŻŔú°ˇ Á¸Ŕç ĽłÁ¤ şŻ°ćŔĚ  1şĐŔĚ»ó °ć°ú µÇľúŔ» °ćżěżˇ´Â ŔŻŔú°ˇ ŔÖ´ÂÁö ĂĽĹ©¸¦ ÇŃ´Ů.
		int nElapseTick = i_dwCurTick - m_dwChangedTickforExistUser;
		if(60000 < nElapseTick)
		{
			if(FALSE == m_pNPCIOCPServer->IsExistClient(this->GetMapChannelIndex()))
			{
				this->SetExistUserInMapChannel(FALSE);		// 2008-12-03 by cmkwon, NPCServer Ĺ¬¶óŔĚľđĆ® ľř´Â ¸ĘĂ¤łÎ Ăł¸® ĽöÁ¤ - ŔŻŔú°ˇ Á¸ŔçÇĎÁö ľĘŔ˝Ŕ» ĽłÁ¤ÇŃ´Ů.
				return FALSE;
			}
			this->SetExistUserInMapChannel(TRUE);
		}
	}

	m_dwWorkeredTick	= i_dwCurTick;
	return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
/// \fn			void CNPCMapChannel::SetExistUserInMapChannel(BOOL i_bIsExistUser)
/// \brief		// 2008-12-03 by cmkwon, NPCServer Ĺ¬¶óŔĚľđĆ® ľř´Â ¸ĘĂ¤łÎ Ăł¸® ĽöÁ¤ - 
/// \author		cmkwon
/// \date		2008-12-04 ~ 2008-12-04
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CNPCMapChannel::SetExistUserInMapChannel(BOOL i_bIsExistUser)
{
	m_bExistUserInMapChannel	= i_bIsExistUser;
	m_dwChangedTickforExistUser	= timeGetTime();
}

BodyCond_t CNPCMapChannel::GetPreAttackBodyCondMask(INT i_nAttackItemIdx) {
	if(0 > i_nAttackItemIdx || ARRAY_SIZE_MONSTER_ITEM < i_nAttackItemIdx) {
		return NULL;
	}
	switch(i_nAttackItemIdx) {
		case 0 : return BODYCON_MON_PREATTACK1_MASK;
		case 1 : return BODYCON_MON_PREATTACK2_MASK;
		case 2 : return BODYCON_MON_PREATTACK3_MASK;
		case 3 : return BODYCON_MON_PREATTACK4_MASK;
		case 4 : return BODYCON_MON_PREATTACK5_MASK;
		case 5 : return BODYCON_MON_PREATTACK6_MASK;
		case 6 : return BODYCON_MON_PREATTACK7_MASK;
		case 7 : return BODYCON_MON_PREATTACK8_MASK;
		case 8 : return BODYCON_MON_PREATTACK9_MASK;
		case 9 : return BODYCON_MON_PREATTACK10_MASK;
		case 10 : return BODYCON_MON_PREATTACK11_MASK;
		case 11 : return BODYCON_MON_PREATTACK12_MASK;
		case 12 : return BODYCON_MON_PREATTACK13_MASK;
		case 13 : return BODYCON_MON_PREATTACK14_MASK;
		case 14 : return BODYCON_MON_PREATTACK15_MASK;
		case 15 : return BODYCON_MON_PRECONTROLSKILL_MASK;
	}
	return NULL;
}

BodyCond_t CNPCMapChannel::GetFireAttackBodyCondMask(INT i_nAttackItemIdx) {
	if(0 > i_nAttackItemIdx || ARRAY_SIZE_MONSTER_ITEM < i_nAttackItemIdx) {
		return NULL;
	}
	switch(i_nAttackItemIdx) {
		case 0 : return BODYCON_MON_FIREATTACK1_MASK;
		case 1 : return BODYCON_MON_FIREATTACK2_MASK;
		case 2 : return BODYCON_MON_FIREATTACK3_MASK;
		case 3 : return BODYCON_MON_FIREATTACK4_MASK;
		case 4 : return BODYCON_MON_FIREATTACK5_MASK;
		case 5 : return BODYCON_MON_FIREATTACK6_MASK;
		case 6 : return BODYCON_MON_FIREATTACK7_MASK;
		case 7 : return BODYCON_MON_FIREATTACK8_MASK;
		case 8 : return BODYCON_MON_FIREATTACK9_MASK;
		case 9 : return BODYCON_MON_FIREATTACK10_MASK;
		case 10 : return BODYCON_MON_FIREATTACK11_MASK;
		case 11 : return BODYCON_MON_FIREATTACK12_MASK;
		case 12 : return BODYCON_MON_FIREATTACK13_MASK;
		case 13 : return BODYCON_MON_FIREATTACK14_MASK;
		case 14 : return BODYCON_MON_FIREATTACK15_MASK;
		case 15 : return BODYCON_MON_FIRECONTROLSKILL_MASK;
	}
	return NULL;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			BOOL CNPCMapChannel::SendFSvrHPTalk(CNPCMonster *i_pMons, HPACTION_TALK_HPRATE * i_pTalkHPRate)
/// \brief		ŔÎÇÇ´ĎĆĽ - ¸ó˝şĹÍ HPżˇ µű¶ó ´ë»ç ŔüĽŰ
/// \author		dhjin
/// \date		2009-09-09 ~ 2010
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CNPCMapChannel::SendFSvrHPTalk(CNPCMonster *i_pMons, HPACTION_TALK_HPRATE * i_pTalkHPRate) {
	INIT_MSG_WITH_BUFFER(MSG_FN_MONSTER_HPTALK, T_FN_MONSTER_HPTALK, pMonsterHPTalk, i_pBufSend);
	pMonsterHPTalk->ChannelIndex		= this->GetMapChannelIndex();
	pMonsterHPTalk->MonsterIndex		= i_pMons->MonsterIndex;
	pMonsterHPTalk->MonsterUnitKind		= i_pMons->MonsterInfoPtr->MonsterUnitKind;
	pMonsterHPTalk->HPValueRate			= i_pTalkHPRate->HPValueRate;
	pMonsterHPTalk->HPTalkImportance	= i_pTalkHPRate->HPTalkImportance;
	pMonsterHPTalk->HPCameraTremble		= i_pTalkHPRate->HPCameraTremble;
	pMonsterHPTalk->TargetIndex			= i_pTalkHPRate->TargetClientIdx;
	STRNCPY_MEMSET(pMonsterHPTalk->HPTalk, i_pTalkHPRate->HPTalk, SIZE_MAX_HPTALK_DESCRIPTION);

	return Send2FieldServerW(i_pBufSend, MSG_SIZE(MSG_FN_MONSTER_HPTALK));
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			void CNPCMapChannel::SetAutoCreateMonsterChannel(BOOL i_bCreate)
/// \brief		ŔÎÇÇ´ĎĆĽ - ¸÷ »ýĽşŔĚ °ˇ´ÉÇŃÁö
/// \author		dhjin
/// \date		2009-09-09 ~ 2010
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CNPCMapChannel::SetAutoCreateMonsterChannel(BOOL i_bCreate) {
	m_bAutoCreateMonsterChannel	= i_bCreate;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			void CNPCMapChannel::ChangeNewObject(ObjectIdx_t i_dwDeleteObjectUID, ObjectNum_t i_dwNewObject)
/// \brief		ŔÎÇÇ´ĎĆĽ - şŻ°ć żŔşęÁ§Ć®¸¦ Ŕ§ÇŘ!!!!
/// \author		dhjin
/// \date		2009-09-09 ~ 2010
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CNPCMapChannel::ChangeNewObject(ObjectIdx_t i_dwDeleteObjectUID, ObjectNum_t i_dwNewObject) {
	m_mtDeletedObjectInfoList.pushBackLock(i_dwDeleteObjectUID);		// 2009-09-09 ~ 2010-01 by dhjin, ŔÎÇÇ´ĎĆĽ - ĽŇ˝ş ĂĽĹ©
	m_pNPCMapProject->CreateNewObject(i_dwDeleteObjectUID, i_dwNewObject, &m_mtNewObjectInfoList);	// 2009-09-09 ~ 2010-01 by dhjin, ŔÎÇÇ´ĎĆĽ - ĽŇ˝ş ĂĽĹ©
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			void CNPCMapChannel::ResetChangeObject()
/// \brief		ŔÎÇÇ´ĎĆĽ - şŻ°ć żŔşęÁ§Ć®¸¦ Ŕ§ÇŘ!!!!
/// \author		dhjin
/// \date		2009-09-09 ~ 2010
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CNPCMapChannel::ResetChangeObject() {
	m_mtDeletedObjectInfoList.clearLock();	// 2009-09-09 ~ 2010-01 by dhjin, ŔÎÇÇ´ĎĆĽ - ĽŇ˝ş ĂĽĹ©
	m_mtNewObjectInfoList.clearLock();		// 2009-09-09 ~ 2010-01 by dhjin, ŔÎÇÇ´ĎĆĽ - ĽŇ˝ş ĂĽĹ©

	// start 2011-06-02 ŔÎÇÇ´ĎĆĽ 3Â÷ - ˝şĹÜ 6 - ÁÖ±âŔű ĽŇČŻ ±â´É Á¦ŔŰ
	vectorObjectInfoServer::iterator itr(m_vectorObjectMonsterInfoCopy.begin());
	for(; itr != m_vectorObjectMonsterInfoCopy.end(); itr++)
	{
		itr->m_bNotCreateMonster = TRUE;
		itr->m_EventInfo.m_dwLastTimeObjectMonsterCreated = 0;
	}
	// end 2011-06-02 ŔÎÇÇ´ĎĆĽ 3Â÷ - ˝şĹÜ 6 - ÁÖ±âŔű ĽŇČŻ ±â´É Á¦ŔŰ
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			int CNPCMapChannel::GetMonsterIndexesByBell(BYTE i_byMonsterBell, vector<ClientIndex_t> *pClientIndexVector)
/// \brief		ŔÎÇÇ´ĎĆĽ - ¸ĘżˇĽ­ ¸ó˝şĹÍBell°ŞŔÎ ¸ó˝şĹÍ ĂŁ±â
/// \author		dhjin
/// \date		2009-09-09 ~ 2010
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
int CNPCMapChannel::GetMonsterIndexesByBell(BYTE i_byMonsterBell, vector<ClientIndex_t> *pClientIndexVector) {
	mt_auto_lock mtA(&m_mtvectorMonsterPtr);
	int nSize = m_mtvectorMonsterPtr.size();
	int returnSize = 0;
	for(int i=0; i < nSize; i++) {
		CNPCMonster		*pNMon = (CNPCMonster*)m_mtvectorMonsterPtr[i];
		if(NULL == pNMon
			|| MS_NULL == pNMon->m_enMonsterState) {
			continue;
		}
		
		MONSTER_INFO	*pMonInfo = pNMon->MonsterInfoPtr;
		if(NULL == pMonInfo) {
			continue;
		}

		if(i_byMonsterBell == pMonInfo->Belligerence) {
			pClientIndexVector->push_back(pNMon->MonsterIndex);
			returnSize++;
		}
	}

	return returnSize;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			int CNPCMapChannel::NPCGetAdjacentMonsterIndexesByBell(D3DXVECTOR3 *pPos, int nDistance, int nBlockDistance, BYTE i_byMonsterBell, vector<ClientIndex_t> *pClientIndexVector)
/// \brief		ŔÎÇÇ´ĎĆĽ - ¸ó˝şĹÍ ÁÖŔ§żˇ Bell°ŞŔÎ ¸ó˝şĹÍ ĂŁ±â
/// \author		dhjin
/// \date		2009-09-09 ~ 2010
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
int CNPCMapChannel::NPCGetAdjacentMonsterIndexesByBell(D3DXVECTOR3 *pPos, int nDistance, int nBlockDistance, BYTE i_byMonsterBell, vector<ClientIndex_t> *pClientIndexVector) {
	TWO_BLOCK_INDEXES	blockIdx;
	int					i, j;
	
#ifdef _DEBUG
	int tmpCap = pClientIndexVector->capacity();
	pClientIndexVector->clear();
	int tmpCap2 = pClientIndexVector->capacity();
	assert( tmpCap == tmpCap2 );
#endif
	
	m_pNPCMapProject->GetBlockAdjacentToPositionHalfDistance(pPos->x, pPos->z, nBlockDistance, blockIdx);
	
	pClientIndexVector->clear();
	i = blockIdx.sMinX;
	while(i <= blockIdx.sMaxX)
	{
		j = blockIdx.sMinZ;
		while(j <= blockIdx.sMaxZ)
		{
			CMapBlock	*pMapBlock = &m_arrMapBlock[i][j];
			
			if (false == pMapBlock->m_MonsterIndexMtlist.empty())
			{
				mtlistUnitIndex_t vectorTemp;
				pMapBlock->m_MonsterIndexMtlist.lock();
				{
					vectorTemp.reserve(pMapBlock->m_MonsterIndexMtlist.size());
					vectorTemp.insert(vectorTemp.end()
						, pMapBlock->m_MonsterIndexMtlist.begin(), pMapBlock->m_MonsterIndexMtlist.end());
				}
				pMapBlock->m_MonsterIndexMtlist.unlock();
				
				for_each(vectorTemp.begin(), vectorTemp.end()
					, for_each_functor_NPCGetAdjacentMonsterIndexesByBell(this, pPos, nDistance, pClientIndexVector, i_byMonsterBell));
			}
			j++;
		}
		i++;
	}

	return pClientIndexVector->size();
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			int CNPCMapChannel::GetMonsterIndexesByTargetMonsterNum(INT i_nTargetMonsterNum, vector<ClientIndex_t> *pClientIndexVector)
/// \brief		ŔÎÇÇ´ĎĆĽ - ¸ĘżˇĽ­ TargetMonster ĂŁ±â
/// \author		dhjin
/// \date		2009-09-09 ~ 2010
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
int CNPCMapChannel::GetMonsterIndexesByTargetMonsterNum(INT i_nTargetMonsterNum, vector<ClientIndex_t> *pClientIndexVector)
{
// 2010-03-16 by cmkwon, ŔÎÇÇ2Â÷ MtoM, MtoC Ĺ¸°Ů şŻ°ć °ü·Ă ĽöÁ¤ - ľĆ·ˇżÍ °°ŔĚ ĽöÁ¤
// 	mt_auto_lock mtA(&m_mtvectorMonsterPtr);
// 	int nSize = m_mtvectorMonsterPtr.size();
// 	int returnSize = 0;
// 	for(int i=0; i < nSize; i++)
// 	{
// 		CNPCMonster		*pNMon = GetNPCMonster() (CNPCMonster*)m_mtvectorMonsterPtr[i];
// 		if(NULL == pNMon
// 			|| MS_NULL == pNMon->m_enMonsterState) {
// 			continue;
// 		}
// 		
// 		MONSTER_INFO	*pMonInfo = pNMon->MonsterInfoPtr;
// 		if(NULL == pMonInfo) {
// 			continue;
// 		}
// 		
// 		if(i_nTargetMonsterNum == pMonInfo->MonsterUnitKind) {
// 			pClientIndexVector->push_back(pNMon->MonsterIndex);
// 			returnSize++;
// 		}
// 	}
// 	
// 	return returnSize;
	///////////////////////////////////////////////////////////////////////////////
	// 2010-03-16 by cmkwon, ŔÎÇÇ2Â÷ MtoM, MtoC Ĺ¸°Ů şŻ°ć °ü·Ă ĽöÁ¤ - 
	if(0 == i_nTargetMonsterNum){			return 0;}

 	int returnSize = 0;
	CNPCMonster		*pNPCMon = NULL;
	for (int nIdxMonsterArray = 0; nIdxMonsterArray < m_nSizemtvectorMonsterPtr; nIdxMonsterArray++)	
	{
		pNPCMon = &m_ArrNPCMonster[nIdxMonsterArray];
		if(NULL == pNPCMon
			|| FALSE == pNPCMon->IsValidMonster())
		{
			continue;
		}

		MONSTER_INFO *pMonInfo = pNPCMon->MonsterInfoPtr;
		if(NULL == pMonInfo)
		{
			continue;
		}
		
		if(i_nTargetMonsterNum == pMonInfo->MonsterUnitKind)
		{
			pClientIndexVector->push_back(pNPCMon->MonsterIndex);
			returnSize++;
		}
	}
	return returnSize;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			int CNPCMapChannel::NPCGetTargetMonsterwithAttackObj(BYTE AttackObj, CMonster *pAttackMon, vector<ClientIndex_t> &ClientIndexVector)
/// \brief		ŔÎÇÇ´ĎĆĽ - TargetMonster °áÁ¤ÇĎ±â
/// \author		dhjin
/// \date		2009-09-09 ~ 2010
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
int CNPCMapChannel::NPCGetTargetMonsterwithAttackObj(BYTE AttackObj, CMonster *pAttackMon, vector<ClientIndex_t> &ClientIndexVector)
{
	// start 2011-06-17 ŔÎÇÇ´ĎĆĽ 3Â÷ - ¸đĽ± °ř°Ý ľČÇĎ´Â ą®Á¦ ĽöÁ¤ 
	int iMTCount = 0;
	D3DXVECTOR3 PositionMTVector(0.0f, 0.0f, 0.0f);
	// end 2011-06-17 ŔÎÇÇ´ĎĆĽ 3Â÷ - ¸đĽ± °ř°Ý ľČÇĎ´Â ą®Á¦ ĽöÁ¤ 

	if(ClientIndexVector.empty()){				return 0;}

	///////////////////////////////////////////////////////////////////////////////
	// 2007-05-22 by cmkwon, ¸ó˝şĹÍ-->Ĺ¸°ŮżˇĽ­ żŔşęÁ§Ć® ĂćµąĂĽĹ© Ăß°ˇÇÔ
	CNPCMonster		*pMonster = NULL;
	BOOL			bRecognizeInvisible = COMPARE_MPOPTION_BIT(pAttackMon->MonsterInfoPtr->MPOption, MPOPTION_BIT_RECOGNIZE_INVISIBLE);
	vectClientIndex_t::iterator itr(ClientIndexVector.begin());
	while(itr != ClientIndexVector.end()) {
		pMonster = GetNPCMonster(*itr);

		// start 2011-06-17 ŔÎÇÇ´ĎĆĽ 3Â÷ - ¸đĽ± °ř°Ý ľČÇĎ´Â ą®Á¦ ĽöÁ¤ 
			if( pMonster->IsMultiTargetMonster() == TRUE ) 
			{
			PositionMTVector = pMonster->GetNearMultiTarget(pAttackMon->PositionVector, &iMTCount);
		}
		else
		{
			PositionMTVector = pMonster->PositionVector;
		}
		// end 2011-06-17 ŔÎÇÇ´ĎĆĽ 3Â÷ - ¸đĽ± °ř°Ý ľČÇĎ´Â ą®Á¦ ĽöÁ¤ 

		if(NULL == pMonster
			|| FALSE != CheckImpactStraightLineMapAndObjects(&pAttackMon->PositionVector, &PositionMTVector		// 2011-06-17 ŔÎÇÇ´ĎĆĽ 3Â÷ - ¸đĽ± °ř°Ý ľČÇĎ´Â ą®Á¦ ĽöÁ¤
				, DEFAULT_OBJECT_MONSTER_OBJECT+pAttackMon->MonsterInfoPtr->MonsterUnitKind)) {

			itr = ClientIndexVector.erase(itr);
			continue;
		}
		itr++;
	}
	if(ClientIndexVector.empty()){				return 0;}

	int				nRetTarget = 0;
	if(ClientIndexVector.size() == 1) {
		pMonster = GetNPCMonster(ClientIndexVector[0]);
		if(pMonster) {
			return ClientIndexVector[0];
		}
		return 0;
	}

	switch(AttackObj)
	{
	case ATTACKOBJ_CLOSERANGE: {
			float	fDistance;
			float	fMinDistance = this->GetMonsterVisibleDiameterW();
			vector<ClientIndex_t>::iterator itr = ClientIndexVector.begin();
			while(itr != ClientIndexVector.end()) {
				pMonster = GetNPCMonster(*itr);
				if(pMonster					
					&& (fDistance = D3DXVec3Length(&(pMonster->PositionVector - pAttackMon->PositionVector))) < fMinDistance) {
					nRetTarget = *itr;
					fMinDistance = fDistance;
				}
				itr++;
			}
		}
		break;
	case ATTACKOBJ_LOWHP: {
			float	fMinHP = 65535.0f;
			vector<ClientIndex_t>::iterator itr = ClientIndexVector.begin();
			while(itr != ClientIndexVector.end()) {
				pMonster = GetNPCMonster(*itr);
				if(pMonster
					&& pMonster->CurrentHP < fMinHP) {
					nRetTarget = *itr;
					fMinHP = pMonster->CurrentHP;
				}
				itr++;
			}
		}
		break;
	case ATTACKOBJ_HIGHHP: {
			float	fMaxHP = 0.0f;
			vector<ClientIndex_t>::iterator itr = ClientIndexVector.begin();
			while(itr != ClientIndexVector.end()) {
				pMonster = GetNPCMonster(*itr); 
				if(pMonster
					&& pMonster->CurrentHP > fMaxHP) {
					nRetTarget = *itr;
					fMaxHP = pMonster->CurrentHP;
				}
				itr++;
			}
		}
		break;
	default:
		// ATTACKOBJ_FIRSTATTACK
		// ATTACKOBJ_SAMERACE
		// ATTACKOBJ_RANDOM
		{
			for(int i=0; i < 5; i++) {
				nRetTarget = ClientIndexVector[GetTickCount()%ClientIndexVector.size()];
				pMonster = GetNPCMonster(nRetTarget); // 2009-09-09 ~ 2010-01-11 by dhjin, ŔÎÇÇ´ĎĆĽ - ¸ó˝şĹÍ°Ł Ĺ¸°Ů ĽłÁ¤Ŕ» Ŕ§ÇŘ ĽöÁ¤
				if(pMonster) {
					return nRetTarget;
				}
			}
			nRetTarget = 0;
		}
	}

	return nRetTarget;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			int CNPCMapChannel::BattleAttackSecMultiTargetMonster(CNPCMonster *i_pAttackMon, CNPCMonster *i_pTargetMonster, int MultiTargetCheckSize)
/// \brief		ŔÎÇÇ´ĎĆĽ - 2Çü ą«±â ¸ÖĆĽ Ĺ¸°Ů ¸ó˝şĹÍ Ăł¸®
/// \author		dhjin
/// \date		2009-09-09 ~ 2010
/// \warning	
///
/// \param		
/// \return		°ř°Ý ąŢŔş ¸ó˝şĹÍ Ľö
///////////////////////////////////////////////////////////////////////////////
int CNPCMapChannel::BattleAttackSecMultiTargetMonster(CNPCMonster *i_pAttackMon, CNPCMonster *i_pTargetMonster, int MultiTargetCheckSize) {
	int		MultiTargetCount  = 0;
	vectorClientIndex tmClientIdxList;
	tmClientIdxList.reserve(10);
	if(BELL_INFINITY_DEFENSE_MONSTER == i_pAttackMon->MonsterInfoPtr->Belligerence) {
		if(0 >= NPCGetAdjacentMonsterIndexesByBell(&i_pAttackMon->PositionVector, i_pAttackMon->m_pUsingMonsterItem->pItemInfo->Range
			, i_pAttackMon->m_pUsingMonsterItem->pItemInfo->Range*2, BELL_INFINITY_ATTACK_MONSTER, &tmClientIdxList)) {
			return MultiTargetCount;
		}
	}
	else if(BELL_INFINITY_ATTACK_MONSTER == i_pAttackMon->MonsterInfoPtr->Belligerence) {
		if(0 >= NPCGetAdjacentMonsterIndexesByBell(&i_pAttackMon->PositionVector, i_pAttackMon->m_pUsingMonsterItem->pItemInfo->Range
			, i_pAttackMon->m_pUsingMonsterItem->pItemInfo->Range*2, BELL_INFINITY_DEFENSE_MONSTER, &tmClientIdxList)) {
			return MultiTargetCount;
		}
	}
	else {
		if(0 >= NPCGetAdjacentMonsterIndexes(&i_pAttackMon->PositionVector, i_pAttackMon->m_pUsingMonsterItem->pItemInfo->Range
			, i_pAttackMon->m_pUsingMonsterItem->pItemInfo->Range*2, &tmClientIdxList)) {
			return MultiTargetCount;
		}
	}
	int CheckSize = MultiTargetCheckSize;
	if(tmClientIdxList.size() < MultiTargetCheckSize) {
		// ´ŮĽö ŔűżëŔĚ ľî±×·Î ¸®˝şĆ® ş¸´Ů ¸ą´Ů¸é ľî±×·Î ¸®˝şĆ® Ľö·Î ĂÖ´ë·® ÇŇ´ç.
		CheckSize = tmClientIdxList.size();
	}
	vectorClientIndex::iterator itr(tmClientIdxList.begin());
	for(; itr != tmClientIdxList.end(); itr++) {
		CNPCMonster		* ptmMonsterInfo = GetNPCMonster(*itr);
		if(NULL != i_pTargetMonster
			&& NULL != ptmMonsterInfo
			&& ptmMonsterInfo->MonsterIndex == i_pTargetMonster->MonsterIndex) {
			// ŔĚąĚ Ăł¸®µČ ŔŻŔú, ´ŮŔ˝ ŔŻŔú °ˇÁ®żŔ±â
			continue;
		}
		if(NULL != ptmMonsterInfo && ptmMonsterInfo->m_enMonsterState == MS_PLAYING) {

			// start 2011-03-24 by hskim, ŔÎÇÇ´ĎĆĽ 3Â÷ - ¸ó˝şĹÍ ¸ÖĆĽ Ĺ¸°ŮĆĂ ±â´É Ăß°ˇ
			//////////
			// ±âÁ¸ //
			// this->SendFSvrBattleAttackSec(i_pAttackMon, ptmMonsterInfo->MonsterIndex, &ptmMonsterInfo->PositionVector);	

			//////////
			// ĽöÁ¤ //
			int fMonsterMultiTargetIndex = 0;
			D3DXVECTOR3 fMonsterMultiTargetVector = ptmMonsterInfo->GetNearMultiTarget(i_pAttackMon->PositionVector, &fMonsterMultiTargetIndex);

			this->SendFSvrBattleAttackSec(i_pAttackMon, ptmMonsterInfo->MonsterIndex, &fMonsterMultiTargetVector, fMonsterMultiTargetIndex);
			// end 2011-03-24 by hskim, ŔÎÇÇ´ĎĆĽ 3Â÷ - ¸ó˝şĹÍ ¸ÖĆĽ Ĺ¸°ŮĆĂ ±â´É Ăß°ˇ

			MultiTargetCount++;
		}
		if(MultiTargetCount >= CheckSize) {
			break;
		}
	}	
	
	return MultiTargetCount;

}

///////////////////////////////////////////////////////////////////////////////
/// \fn			int CNPCMapChannel::BattleAttackSecMultiTargetUser(CNPCMonster *i_pAttackMon, CLIENT_INFO * i_pClientInfo, int MultiTargetCheckSize)
/// \brief		ŔÎÇÇ´ĎĆĽ - 2Çü ą«±â ¸ÖĆĽ Ĺ¸°Ů Ăł¸®
/// \author		dhjin
/// \date		2009-09-09 ~ 2010
/// \warning	
///
/// \param		
/// \return		°ř°Ý ąŢŔş ŔŻŔú Ľö
///////////////////////////////////////////////////////////////////////////////
int CNPCMapChannel::BattleAttackSecMultiTargetUser(CNPCMonster *i_pAttackMon, CLIENT_INFO * i_pClientInfo, int MultiTargetCheckSize) {
	int		MultiTargetCount  = 0;
	vectorClientIndex tmClientIdxList;
	tmClientIdxList.reserve(10);
	if(0 >= NPCGetAdjacentCharacterIndexes(&i_pAttackMon->PositionVector, i_pAttackMon->m_pUsingMonsterItem->pItemInfo->Range
		, i_pAttackMon->m_pUsingMonsterItem->pItemInfo->Range*2, &tmClientIdxList, i_pAttackMon->MonsterInfoPtr->Belligerence)) {
		return MultiTargetCount;
	}
	int CheckSize = MultiTargetCheckSize;
	if(tmClientIdxList.size() < MultiTargetCheckSize) {
		// ´ŮĽö ŔűżëŔĚ ľî±×·Î ¸®˝şĆ® ş¸´Ů ¸ą´Ů¸é ľî±×·Î ¸®˝şĆ® Ľö·Î ĂÖ´ë·® ÇŇ´ç.
		CheckSize = tmClientIdxList.size();
	}
	vectorClientIndex::iterator itr(tmClientIdxList.begin());
	for(; itr != tmClientIdxList.end(); itr++) {
		CLIENT_INFO		* ptmClientInfo = GetClientInfo(*itr);
		if(NULL != i_pClientInfo
			&& NULL != ptmClientInfo
			&& ptmClientInfo->ClientIndex == i_pClientInfo->ClientIndex) {
			// ŔĚąĚ Ăł¸®µČ ŔŻŔú, ´ŮŔ˝ ŔŻŔú °ˇÁ®żŔ±â
			continue;
		}
		if(NULL != ptmClientInfo && ptmClientInfo->ClientState == CS_GAMESTARTED) {
			this->SendFSvrBattleAttackSec(i_pAttackMon, ptmClientInfo->ClientIndex, &ptmClientInfo->PositionVector);
			MultiTargetCount++;
		}
		if(MultiTargetCount >= CheckSize) {
			break;
		}
	}	

	return MultiTargetCount;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			int CNPCMapChannel::SendFSvrBattleAttackSec(CNPCMonster *i_pAttackMon, ClientIndex_t i_ClientIdx, D3DXVECTOR3 * i_pTargetPosition) 
/// \brief		ŔÎÇÇ´ĎĆĽ - 2Çü ą«±â °ř°Ý ĆĐĹ¶
/// \author		dhjin
/// \date		2009-09-09 ~ 2010
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CNPCMapChannel::SendFSvrBattleAttackSec(CNPCMonster *i_pAttackMon, ClientIndex_t i_ClientIdx, D3DXVECTOR3 * i_pTargetPosition, int fMonsterMultiTargetIndex) // 2011-03-21 by hskim, ŔÎÇÇ´ĎĆĽ 3Â÷ - ¸ó˝şĹÍ ¸ÖĆĽ Ĺ¸°ŮĆĂ ±â´É Ăß°ˇ - fMonsterMultiTargetIndex °Ş Ăß°ˇ
{
	///////////////////////////////////////////////////////////////////////
	// MultiNum Ăł¸®
	BYTE byExplosionPos = 128 - i_pAttackMon->m_pUsingMonsterItem->pItemInfo->MultiNum/2;		
	for(int i = 0; i < i_pAttackMon->m_pUsingMonsterItem->pItemInfo->MultiNum; i++)
	{
		INIT_MSG_WITH_BUFFER(MSG_FN_BATTLE_ATTACK_SECONDARY, T_FN_BATTLE_ATTACK_SECONDARY, pSendBattleAttackSec, pSendBuf);
		pSendBattleAttackSec->ChannelIndex		= m_MapChannelIndex.ChannelIndex;
		pSendBattleAttackSec->AttackIndex		= i_pAttackMon->MonsterIndex;
		pSendBattleAttackSec->TargetIndex		= i_ClientIdx;
		pSendBattleAttackSec->WeaponIndex		= m_uiMissileUniqueIndex++;
		pSendBattleAttackSec->WeaponItemNumber	= i_pAttackMon->m_pUsingMonsterItem->pItemInfo->ItemNum;
		pSendBattleAttackSec->TargetPosition	= *i_pTargetPosition;
		pSendBattleAttackSec->MultiTargetIndex  = fMonsterMultiTargetIndex;		// 2011-03-21 by hskim, ŔÎÇÇ´ĎĆĽ 3Â÷ - ¸ó˝şĹÍ ¸ÖĆĽ Ĺ¸°ŮĆĂ ±â´É Ăß°ˇ
		pSendBattleAttackSec->Distance			= byExplosionPos;
		pSendBattleAttackSec->SecAttackType		= 0;
		pSendBattleAttackSec->AttackPosition	= i_pAttackMon->PositionVector;		// 2009-09-09 ~ 2010 by dhjin, ŔÎÇÇ´ĎĆĽ - ¸ó˝şĹÍ Ŕ§Äˇ
		Send2FieldServerW(pSendBuf, MSG_SIZE(MSG_FN_BATTLE_ATTACK_SECONDARY));
		
		if(0 == i_pAttackMon->m_pUsingMonsterItem->pItemInfo->MultiNum%2
			&& 127 == byExplosionPos)
		{
			byExplosionPos = 129;
		}
		else
		{
			byExplosionPos++;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		// 2010-01-06 by cmkwon, ¸ó˝şĹÍ °ř°Ý˝Ă Ĺ¸°Ů °ř°Ý °ˇ´É ĂĽĹ© Ăß°ˇ(Ĺ¸°ŮşŻ°ć) - 
/// \author		cmkwon
/// \date		2010-01-06 ~ 2010-01-06
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CNPCMapChannel::GetTargetObject(BOOL *o_pbIsTargetCharacter, CLIENT_INFO **o_ppClient, CNPCMonster **o_ppMonster, CNPCMonster *i_pNMon, BOOL i_bLiveCheck/*=TRUE*/)
{
	*o_pbIsTargetCharacter	= FALSE;
	*o_ppClient		= NULL;
	*o_ppMonster	= NULL;

	if(FALSE == i_pNMon->IsValidMonster())
	{
		return FALSE;
	}

	ClientIndex_t targetIdx = i_pNMon->GetTargetIndex();
	// 2009-09-09 ~ 2010-01-11 by dhjin, ŔÎÇÇ´ĎĆĽ - ¸ó˝şĹÍ°Ł Ĺ¸°Ů ĽłÁ¤Ŕ» Ŕ§ÇŘ ĽöÁ¤, ąŘ°ú °°ŔĚ ĽöÁ¤
//	if(FALSE == IS_VALID_CLIENT_INDEX(targetIdx))
	if(FALSE == IS_VALID_CHARACTER_AND_MONSTER_INDEX(targetIdx))
	{
		return FALSE;
	}

	if(IS_CHARACTER_CLIENT_INDEX(targetIdx))
	{
		CLIENT_INFO *pCli = this->GetClientInfo(targetIdx, &(this->GetMapChannelIndex()));
		if(NULL == pCli
			|| FALSE == pCli->IsValidClient(i_bLiveCheck))
		{
			return FALSE;
		}
		*o_pbIsTargetCharacter	= TRUE;
		*o_ppClient				= pCli;
	}
	else
	{
		CNPCMonster *pNMon = this->GetNPCMonster(targetIdx);
		if(NULL == pNMon
			|| FALSE == pNMon->IsValidMonster(i_bLiveCheck))
		{
			return FALSE;
		}
		*o_pbIsTargetCharacter	= FALSE;
		*o_ppMonster			= pNMon;
	}

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		// 2010-01-06 by cmkwon, ¸ó˝şĹÍ °ř°Ý˝Ă Ĺ¸°Ů °ř°Ý °ˇ´É ĂĽĹ© Ăß°ˇ(Ĺ¸°ŮşŻ°ć) - 
/// \author		cmkwon
/// \date		2010-01-06 ~ 2010-01-06
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CNPCMapChannel::CheckValidTarget(CNPCMonster *i_pNMon)
{
	BOOL bIsTargetCharacter = FALSE;
	CLIENT_INFO *pTargetCli	= NULL;
	CNPCMonster *pTargetMon	= NULL;

	if(FALSE == GetTargetObject(&bIsTargetCharacter, &pTargetCli, &pTargetMon, i_pNMon))
	{// 2010-01-06 by cmkwon, Ĺ¸°ŮŔĚ ŔŻČżÇĎÁö ľĘ´Ů
		i_pNMon->DeleteAttackedInfowithIndex();
		return FALSE;
	}

	if(MONSTER_TARGETTYPE_TUTORIAL == i_pNMon->m_byMonsterTargetType)
	{// 2010-01-06 by cmkwon, Ć©Ĺä¸®ľó ¸ó˝şĹÍ´Â ŔŻČżĽş ĂĽĹ© ÇĘżä ľřŔ˝.
		return TRUE;
	}

	int nElapsedTickCheckValidTarget = (int)((INT64)i_pNMon->m_dwCurrentTick - (INT64)i_pNMon->GetTimeCheckValidTarget());
	if(1000 > nElapsedTickCheckValidTarget)
	{// 2010-01-06 by cmkwon, 1ĂĘ¸¶´Ů Ĺ¸°Ů ŔŻČżĽş ĂĽĹ©
		return TRUE;
	}
	i_pNMon->SetTimeCheckValidTarget(i_pNMon->m_dwCurrentTick);

	// start 2011-06-13 by hskim, ŔÎÇÇ´ĎĆĽ 3Â÷ - ¸ó˝şĹÍ ¸ÖĆĽ Ĺ¸°ŮĆĂ ±â´É Ăß°ˇ
	int fMonsterMultiTargetIndex = 0;

	//////////
	// ±âÁ¸
	//D3DXVECTOR3	Vec3TargetPos		= (FALSE == bIsTargetCharacter) ? pTargetMon->PositionVector : pTargetCli->PositionVector;

	//////////
	// ĽöÁ¤
	D3DXVECTOR3	Vec3TargetPos		= (FALSE == bIsTargetCharacter) ? pTargetMon->GetNearMultiTarget(i_pNMon->PositionVector, &fMonsterMultiTargetIndex) : pTargetCli->PositionVector;
	// end 2011-06-13 by hskim, ŔÎÇÇ´ĎĆĽ 3Â÷ - ¸ó˝şĹÍ ¸ÖĆĽ Ĺ¸°ŮĆĂ ±â´É Ăß°ˇ

	D3DXVECTOR3 Vec3Me2Targ			= i_pNMon->PositionVector - Vec3TargetPos;
	D3DXVECTOR3	Vec3Me2TargPlane(Vec3Me2Targ.x, 0.0f, Vec3Me2Targ.z);
	float		fDistMe2TargPlane	= D3DXVec3Length(&Vec3Me2TargPlane);			// Ćň¸é °Ĺ¸®¸¦ ±¸ÇŃ´Ů.

	if(fDistMe2TargPlane > i_pNMon->MonsterInfoPtr->Range)
	{
		int nElapsedTickAfterAttack = (int)((INT64)i_pNMon->m_dwCurrentTick - (INT64)i_pNMon->m_dwTimeMonsterLastAttack);
		if(TICK_MONSTER_DELETE_TARGET_TERM < nElapsedTickAfterAttack)
		{
			i_pNMon->DeleteAttackedInfowithIndex();
			return FALSE;
		}
	}
	
	if(TRUE == this->CheckImpactStraightLineMapAndObjects(&i_pNMon->PositionVector, &Vec3TargetPos, DEFAULT_OBJECT_MONSTER_OBJECT+i_pNMon->MonsterInfoPtr->MonsterUnitKind, TRUE))
	{
		int nElapsedTick = (int)((INT64)i_pNMon->m_dwCurrentTick - (INT64)i_pNMon->GetTimeCheckedLastValidTarget());
		if(5000 < nElapsedTick)
		{// 2010-01-06 by cmkwon, 5ĂĘŔĚ»ó Ĺ¸°ŮŔĚ ŔŻČżÇĎÁö ľĘŔ¸¸é Ĺ¸°ŮŔ» »čÁ¦ÇŃ´Ů.
			i_pNMon->DeleteAttackedInfowithIndex();
		}
		return FALSE;
	}
	i_pNMon->SetTimeCheckedLastValidTarget(i_pNMon->m_dwCurrentTick);
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		// 2010-03-16 by cmkwon, ŔÎÇÇ2Â÷ MtoM, MtoC Ĺ¸°Ů şŻ°ć °ü·Ă ĽöÁ¤ - 
/// \author		cmkwon
/// \date		2010-03-16 ~ 2010-03-16
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CNPCMapChannel::SearchTarget(CNPCMonster *i_pNMon)
{
	ClientIndex_t		retTarIdx = 0;
	vectClientIndex_t targetIdxList;
	targetIdxList.reserve(100);

	///////////////////////////////////////////////////////////////////////////////
	// 2010-03-31 by dhjin, ŔÎÇÇ´ĎĆĽ(±âÁöąćľî) - ľĆ±ş Ĺ¸°Ů ŔâľĆľß µÇ´Â ˝şĹłĹ¸°ŮĹ¸ŔÔľĆŔĚĹŰŔĚ¸é ż©±âĽ­ Ĺ¸°Ů Ăł¸®~!!!
	retTarIdx = this->GetOurTagetByUsingItem(i_pNMon);
	if(FALSE != retTarIdx)
	{
		return retTarIdx;
	}

	// 2010-03-16 by cmkwon, 1/3. BELL_INFINITY_DEFENSE_MONSTER : MtoM, Äł¸ŻĹÍ´Â ľĆ±ş
	if(BELL_INFINITY_DEFENSE_MONSTER == i_pNMon->MonsterInfoPtr->Belligerence)
	{
		// ÁÖŔ§żˇĽ­ BELL_INFINITY_ATTACK_MONSTERŔÎ ¸ó˝şĹÍ¸¦ °Ë»ö
		if(0 != NPCGetAdjacentMonsterIndexesByBell(&i_pNMon->PositionVector, i_pNMon->MonsterInfoPtr->AttackRange, i_pNMon->MonsterInfoPtr->AttackRange*2, BELL_INFINITY_ATTACK_MONSTER, &targetIdxList))
		{
			retTarIdx = NPCGetTargetMonsterwithAttackObj(i_pNMon->MonsterInfoPtr->AttackObject, i_pNMon, targetIdxList);
		}

		// 2009-12-10 by cmkwon, TEMP 
		//g_pNPCGlobal->WriteSystemLogEX(TRUE, "[TEMP] infi2 100317 30000 CNPCMapChannel::SearchTarget# MonIdx(%d) CurTargetIndex(%d) NewTargetIdx(%d) CurTick(%d) nAttackerCnt(%d) \r\n"
		//	, i_pNMon->MonsterIndex, i_pNMon->m_nTargetIndex, retTarIdx, i_pNMon->m_dwCurrentTick, i_pNMon->m_mtvectorAttackedInfoPtr.size());		
		return retTarIdx;
	}

	// 2010-03-16 by cmkwon, 2/3. BELL_INFINITY_ATTACK_MONSTER : MtoM, MtoC, Äł¸ŻĹÍ´Â ÇŘ´ç ¸ó˝şĹÍŔÇ Ŕű±ş
	if (BELL_INFINITY_ATTACK_MONSTER == i_pNMon->MonsterInfoPtr->Belligerence)
	{
		// 2010-03-16 by cmkwon, 2/3-1/4. MonsterTargetŔ» 1Â÷ żěĽ± °Ë»öÇŘĽ­ ĂĽĹ©
		if(0 == retTarIdx
			&& 0 != this->GetMonsterIndexesByTargetMonsterNum(i_pNMon->MonsterInfoPtr->MonsterTarget, &targetIdxList))
		{
			retTarIdx = NPCGetTargetMonsterwithAttackObj(i_pNMon->MonsterInfoPtr->AttackObject, i_pNMon, targetIdxList);
		}

		// 2010-03-16 by cmkwon, 2/3-2/4. MonsterTarget2Ŕ» 2Â÷ żěĽ± °Ë»öÇŘĽ­ ĂĽĹ©
		if(0 == retTarIdx
			&& 0 != this->GetMonsterIndexesByTargetMonsterNum(i_pNMon->MonsterInfoPtr->MonsterTarget2, &targetIdxList))
		{
			retTarIdx =  NPCGetTargetMonsterwithAttackObj(i_pNMon->MonsterInfoPtr->AttackObject, i_pNMon, targetIdxList);
		}

		// 2010-03-16 by cmkwon, 2/3-3/4. ÁÖŔ§ŔÇ Äł¸ŻĹÍ¸¦ 3Â÷ °Ë»öÇŘĽ­ ĂĽĹ©
		if(0 == retTarIdx
			&& 0 != NPCGetAdjacentCharacterIndexes(&i_pNMon->PositionVector, i_pNMon->MonsterInfoPtr->AttackRange, i_pNMon->MonsterInfoPtr->AttackRange*2, &targetIdxList, i_pNMon->MonsterInfoPtr->Belligerence))
		{
			retTarIdx = NPCGetTargetwithAttackObj(i_pNMon->MonsterInfoPtr->AttackObject, i_pNMon, targetIdxList);
		}
		
		// 2010-03-17 by cmkwon, 2/3-4/4. ÁÖŔ§żˇĽ­ BELL_INFINITY_DEFENSE_MONSTERŔÎ ¸ó˝şĹÍ¸¦ 4Â÷ żěĽ± °Ë»ö
		if(0 == retTarIdx
			&& 0 != NPCGetAdjacentMonsterIndexesByBell(&i_pNMon->PositionVector, i_pNMon->MonsterInfoPtr->AttackRange, i_pNMon->MonsterInfoPtr->AttackRange*2, BELL_INFINITY_DEFENSE_MONSTER, &targetIdxList))
		{
			retTarIdx = NPCGetTargetMonsterwithAttackObj(i_pNMon->MonsterInfoPtr->AttackObject, i_pNMon, targetIdxList);
		}

		// 2009-12-10 by cmkwon, TEMP 
		//g_pNPCGlobal->WriteSystemLogEX(TRUE, "[TEMP] infi2 100317 31000 CNPCMapChannel::SearchTarget# MonIdx(%d) CurTargetIndex(%d) NewTargetIdx(%d) CurTick(%d) nAttackerCnt(%d) \r\n"
		//	, i_pNMon->MonsterIndex, i_pNMon->m_nTargetIndex, targetIdxList, i_pNMon->m_dwCurrentTick, i_pNMon->m_mtvectorAttackedInfoPtr.size());

		return retTarIdx;
	}

	// 2010-03-16 by cmkwon, 3/3. łŞ¸ÓÁö : M to C
	if(NPCGetAdjacentCharacterIndexes(&i_pNMon->PositionVector, i_pNMon->MonsterInfoPtr->AttackRange, i_pNMon->MonsterInfoPtr->AttackRange*2, &targetIdxList, i_pNMon->MonsterInfoPtr->Belligerence))
	{
		retTarIdx = NPCGetTargetwithAttackObj(i_pNMon->MonsterInfoPtr->AttackObject, i_pNMon, targetIdxList);
	}

	return retTarIdx;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		// 2010-03-16 by cmkwon, ŔÎÇÇ2Â÷ MtoM, MtoC Ĺ¸°Ů şŻ°ć °ü·Ă ĽöÁ¤ - 
/// \author		cmkwon
/// \date		2010-03-16 ~ 2010-03-16
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CNPCMapChannel::GetUnitObject(ClientIndex_t i_unitIdx, CLIENT_INFO **o_ppCliInfo, CNPCMonster **o_ppNPCMon)
{
	*o_ppCliInfo	= NULL;
	*o_ppNPCMon		= NULL;

	if(IS_CHARACTER_CLIENT_INDEX(i_unitIdx))
	{
		*o_ppCliInfo = this->GetClientInfo(i_unitIdx, &m_MapChannelIndex);
		return (BOOL)(*o_ppCliInfo);		// 2010-05-24 by cmkwon, ŔÎÇÇ2Â÷ ŔĚČÄ NPC Ľ­ąö Á×´Â ąö±× ĽöÁ¤ - 
	}
	
	if(IS_MONSTER_CLIENT_INDEX(i_unitIdx))
	{
		*o_ppNPCMon = this->GetNPCMonster(i_unitIdx);
		return (BOOL)(*o_ppNPCMon);			// 2010-05-24 by cmkwon, ŔÎÇÇ2Â÷ ŔĚČÄ NPC Ľ­ąö Á×´Â ąö±× ĽöÁ¤ - 
	}

	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			BOOL CNPCMapChannel::SetMultiTargetEnemy8CheckFullList(vector<ClientIndex_t> * i_pTargetIndexList, vector<ClientIndex_t> *o_pTargetIndexList, ClientIndex_t i_ExceptTargetIdx, int MultiTargetCheckSize)
/// \brief		ŔÎÇÇ´ĎĆĽ(±âÁöąćľî) - 
/// \author		dhjin
/// \date		2010-03-31
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CNPCMapChannel::SetMultiTargetEnemy8CheckFullList(vector<ClientIndex_t> * i_pTargetIndexList, vector<ClientIndex_t> *o_pTargetIndexList, ClientIndex_t i_ExceptTargetIdx, int MultiTargetCheckSize)
{
	if(NULL == i_pTargetIndexList
		|| NULL == o_pTargetIndexList)
	{
		return FALSE;
	}

	vectorClientIndex::iterator itr = i_pTargetIndexList->begin();
	for(; itr != i_pTargetIndexList->end(); itr++)
	{
		if(MultiTargetCheckSize <= o_pTargetIndexList->size()) 
		{
			return TRUE;
		}
		
		if(i_ExceptTargetIdx != *itr) 
		{
			o_pTargetIndexList->push_back(*itr);
		}
	}

	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			void CNPCMapChannel::GetMultiTargetEnemyList(CNPCMonster *i_pMon, ClientIndex_t i_TargetIdx,vector<ClientIndex_t> *o_pTargetIndexList, int MultiTargetCheckSize, BOOL i_bCheckDistance /*= TRUE*/)
/// \brief		ŔÎÇÇ´ĎĆĽ(±âÁöąćľî) - 
/// \author		dhjin
/// \date		2010-03-31
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CNPCMapChannel::GetMultiTargetEnemyList(CNPCMonster *i_pMon, ClientIndex_t i_TargetIdx, vector<ClientIndex_t> *o_pTargetIndexList, int MultiTargetCheckSize, BOOL i_bCheckDistance /*= TRUE*/)
{
	if(NULL == i_pMon
		|| NULL == o_pTargetIndexList)
	{
		return;
	}

	int TargetListCount = 0;
	vectorClientIndex	targetIdxList;
	targetIdxList.reserve(100);
	
	// Ĺ¸°Ů ŔâČů łŕĽ® ŔĎ´Ü łÖ°í
	o_pTargetIndexList->push_back(i_TargetIdx);	
		
	// 2010-03-16 by cmkwon, 1/3. BELL_INFINITY_DEFENSE_MONSTER : MtoM, Äł¸ŻĹÍ´Â ľĆ±ş
	if(BELL_INFINITY_DEFENSE_MONSTER == i_pMon->MonsterInfoPtr->Belligerence)
	{
		// ÁÖŔ§żˇĽ­ BELL_INFINITY_ATTACK_MONSTERŔÎ ¸ó˝şĹÍ¸¦ °Ë»ö
		this->NPCGetAdjacentMonsterIndexesByBell(&i_pMon->PositionVector, i_pMon->MonsterInfoPtr->AttackRange, i_pMon->MonsterInfoPtr->AttackRange*2, BELL_INFINITY_ATTACK_MONSTER, &targetIdxList);
		this->SetMultiTargetEnemy8CheckFullList(&targetIdxList, o_pTargetIndexList, i_TargetIdx, MultiTargetCheckSize);
		return;
	}

	// 2010-03-16 by cmkwon, 2/3. BELL_INFINITY_ATTACK_MONSTER : MtoM, MtoC, Äł¸ŻĹÍ´Â ÇŘ´ç ¸ó˝şĹÍŔÇ Ŕű±ş
	if (BELL_INFINITY_ATTACK_MONSTER == i_pMon->MonsterInfoPtr->Belligerence)
	{
		// 2010-03-16 by cmkwon, 2/3-1/4. MonsterTargetŔ» 1Â÷ żěĽ± °Ë»öÇŘĽ­ ĂĽĹ©
		this->GetMonsterIndexesByTargetMonsterNum(i_pMon->MonsterInfoPtr->MonsterTarget, &targetIdxList);
		if(MultiTargetCheckSize < targetIdxList.size())
		{
			this->SetMultiTargetEnemy8CheckFullList(&targetIdxList, o_pTargetIndexList, i_TargetIdx, MultiTargetCheckSize);
			return;
		}
			
		// 2010-03-16 by cmkwon, 2/3-2/4. MonsterTarget2Ŕ» 2Â÷ żěĽ± °Ë»öÇŘĽ­ ĂĽĹ©
		this->GetMonsterIndexesByTargetMonsterNum(i_pMon->MonsterInfoPtr->MonsterTarget2, &targetIdxList);
		if(MultiTargetCheckSize < targetIdxList.size())
		{
			this->SetMultiTargetEnemy8CheckFullList(&targetIdxList, o_pTargetIndexList, i_TargetIdx, MultiTargetCheckSize);
			return;			
		}
		
		// 2010-03-16 by cmkwon, 2/3-3/4. ÁÖŔ§ŔÇ Äł¸ŻĹÍ¸¦ 3Â÷ °Ë»öÇŘĽ­ ĂĽĹ©
		this->NPCGetAdjacentCharacterIndexes(&i_pMon->PositionVector, i_pMon->MonsterInfoPtr->AttackRange, i_pMon->MonsterInfoPtr->AttackRange*2, &targetIdxList, i_pMon->MonsterInfoPtr->Belligerence);
		if(MultiTargetCheckSize < targetIdxList.size())
		{
			this->SetMultiTargetEnemy8CheckFullList(&targetIdxList, o_pTargetIndexList, i_TargetIdx, MultiTargetCheckSize);
			return;			
		}
		
		// 2010-03-17 by cmkwon, 2/3-4/4. ÁÖŔ§żˇĽ­ BELL_INFINITY_DEFENSE_MONSTERŔÎ ¸ó˝şĹÍ¸¦ 4Â÷ żěĽ± °Ë»ö
		this->NPCGetAdjacentMonsterIndexesByBell(&i_pMon->PositionVector, i_pMon->MonsterInfoPtr->AttackRange, i_pMon->MonsterInfoPtr->AttackRange*2, BELL_INFINITY_DEFENSE_MONSTER, &targetIdxList);
		if(MultiTargetCheckSize < targetIdxList.size())
		{
			this->SetMultiTargetEnemy8CheckFullList(&targetIdxList, o_pTargetIndexList, i_TargetIdx, MultiTargetCheckSize);
			return;			
		}
	}
	
	// 2010-03-16 by cmkwon, 3/3. łŞ¸ÓÁö : M to C
	this->NPCGetAdjacentCharacterIndexes(&i_pMon->PositionVector, i_pMon->MonsterInfoPtr->AttackRange, i_pMon->MonsterInfoPtr->AttackRange*2, &targetIdxList, i_pMon->MonsterInfoPtr->Belligerence);
	this->SetMultiTargetEnemy8CheckFullList(&targetIdxList, o_pTargetIndexList, i_TargetIdx, MultiTargetCheckSize);
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			void CNPCMapChannel::GetMultiTargetEnemyList(CNPCMonster *i_pMon, vector<ClientIndex_t> *o_pTargetIndexList, BOOL i_bExceptMe /*= TRUE*/, BOOL i_bCheckDistance /*= TRUE*/)
/// \brief		ŔÎÇÇ´ĎĆĽ(±âÁöąćľî) - 
/// \author		dhjin
/// \date		2010-03-31
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CNPCMapChannel::GetTargetOurList(CNPCMonster *i_pMon, vector<ClientIndex_t> *o_pTargetIndexList, BOOL i_bExceptMe /*= TRUE*/, BOOL i_bCheckDistance /*= TRUE*/)
{
	if(NULL == i_pMon
		|| NULL == o_pTargetIndexList)
	{
		return;
	}

	// 1. BELL_INFINITY_DEFENSE_MONSTER ŔÎ ¸ó˝şĹÍżˇ ľĆ±şŔ» ĂŁŔÚ. ŔŻŔú, BELL_INFINITY_DEFENSE_MONSTER ľĆ±ş
	if(BELL_INFINITY_DEFENSE_MONSTER == i_pMon->MonsterInfoPtr->Belligerence)
	{
		vectClientIndex_t tmCliIdxList;			// 2010-04-01 by cmkwon, ŔÎÇÇ2Â÷ Ăß°ˇ ĽöÁ¤ - 
		tmCliIdxList.reserve(100);

		// ÁÖŔ§żˇĽ­ BELL_INFINITY_DEFENSE_MONSTER ¸ó˝şĹÍ¸¦ °Ë»ö
		this->NPCGetAdjacentMonsterIndexesByBell(&i_pMon->PositionVector, i_pMon->MonsterInfoPtr->AttackRange, i_pMon->MonsterInfoPtr->AttackRange*2, BELL_INFINITY_DEFENSE_MONSTER, &tmCliIdxList);
		o_pTargetIndexList->insert(o_pTargetIndexList->end(), tmCliIdxList.begin(), tmCliIdxList.end());		// 2010-04-01 by cmkwon, ŔÎÇÇ2Â÷ Ăß°ˇ ĽöÁ¤ - 

		// ÁÖŔ§żˇĽ­ ŔŻŔú °Ë»ö
		// 2010-04-01 by cmkwon, ŔÎÇÇ2Â÷ Ăß°ˇ ĽöÁ¤ - ÁÖŔ§żˇĽ­ ľĆ±şŔŻŔú ¸®˝şĆ®¸¦ °ˇÁ®żÂ´Ů.
		this->NPCGetAdjacentCharacterIndexes(&i_pMon->PositionVector, i_pMon->MonsterInfoPtr->AttackRange, i_pMon->MonsterInfoPtr->AttackRange*2, &tmCliIdxList, i_pMon->MonsterInfoPtr->Belligerence, FALSE);
		o_pTargetIndexList->insert(o_pTargetIndexList->end(), tmCliIdxList.begin(), tmCliIdxList.end());		// 2010-04-01 by cmkwon, ŔÎÇÇ2Â÷ Ăß°ˇ ĽöÁ¤ - 
	}
	
	// 2. BELL_INFINITY_ATTACK_MONSTER ŔÎ ¸ó˝şĹÍżˇ ľĆ±şŔ» ĂŁŔÚ. BELL_INFINITY_ATTACK_MONSTER ľĆ±ş
	else if (BELL_INFINITY_ATTACK_MONSTER == i_pMon->MonsterInfoPtr->Belligerence)
	{
		// ÁÖŔ§żˇĽ­ BELL_INFINITY_DEFENSE_MONSTER ¸ó˝şĹÍ¸¦ °Ë»ö
		this->NPCGetAdjacentMonsterIndexesByBell(&i_pMon->PositionVector, i_pMon->MonsterInfoPtr->AttackRange, i_pMon->MonsterInfoPtr->AttackRange*2, BELL_INFINITY_ATTACK_MONSTER, o_pTargetIndexList);
	}

	if(TRUE == i_bExceptMe)
	{	// ŔÚ±â ŔÚ˝ĹŔş »čÁ¦
		this->DeleteClientIdx(o_pTargetIndexList, i_pMon->MonsterIndex);
	}
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			BOOL CNPCMapChannel::DeleteClientIdx(vector<ClientIndex_t> *o_pTargetIndexList, ClientIndex_t i_DeleteClientIdx)
/// \brief		ŔÎÇÇ´ĎĆĽ(±âÁöąćľî) - 
/// \author		dhjin
/// \date		2010-03-31
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CNPCMapChannel::DeleteClientIdx(vector<ClientIndex_t> *o_pTargetIndexList, ClientIndex_t i_DeleteClientIdx)
{
	if(NULL == o_pTargetIndexList)
	{
		return FALSE;
	}

	vector<ClientIndex_t>::iterator itr = o_pTargetIndexList->begin();
	while(itr!=o_pTargetIndexList->end())
	{
		if(i_DeleteClientIdx == *itr)
		{
			itr = o_pTargetIndexList->erase(itr);
			return TRUE;
		}
		itr++;
	}
	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			ClientIndex_t CNPCMapChannel::GetTargetOur(CNPCMonster *i_pMon, BOOL i_bRepair /*= FALSE*/, BOOL i_bExceptMe /*= TRUE*/, BOOL i_bCheckDistance /*= TRUE*/)
/// \brief		ŔÎÇÇ´ĎĆĽ(±âÁöąćľî) - 
/// \author		dhjin
/// \date		2010-03-31
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
ClientIndex_t CNPCMapChannel::GetTargetOur(CNPCMonster *i_pMon, BOOL i_bRepair /*= FALSE*/, BOOL i_bExceptMe /*= TRUE*/, BOOL i_bCheckDistance /*= TRUE*/)
{
	if(NULL == i_pMon)
	{
		return FALSE;
	}
	
	vectorClientIndex	targetIdxList;
	targetIdxList.reserve(100);

	// 1. BELL_INFINITY_DEFENSE_MONSTER ŔÎ ¸ó˝şĹÍżˇ ľĆ±şŔ» ĂŁŔÚ. ŔŻŔú, BELL_INFINITY_DEFENSE_MONSTER ľĆ±ş
	if(BELL_INFINITY_DEFENSE_MONSTER == i_pMon->MonsterInfoPtr->Belligerence)
	{
		vectClientIndex_t tmCliIdxList;			// 2010-04-01 by cmkwon, ŔÎÇÇ2Â÷ Ăß°ˇ ĽöÁ¤ - 
		tmCliIdxList.reserve(100);

		// ÁÖŔ§żˇĽ­ BELL_INFINITY_DEFENSE_MONSTER ¸ó˝şĹÍ¸¦ °Ë»ö
		this->NPCGetAdjacentMonsterIndexesByBell(&i_pMon->PositionVector, i_pMon->MonsterInfoPtr->AttackRange, i_pMon->MonsterInfoPtr->AttackRange*2, BELL_INFINITY_DEFENSE_MONSTER, &tmCliIdxList);
		targetIdxList.insert(targetIdxList.end(), tmCliIdxList.begin(), tmCliIdxList.end());		// 2010-04-01 by cmkwon, ŔÎÇÇ2Â÷ Ăß°ˇ ĽöÁ¤ - 

		// ÁÖŔ§żˇĽ­ ŔŻŔú °Ë»ö
		// 2010-04-01 by cmkwon, ŔÎÇÇ2Â÷ Ăß°ˇ ĽöÁ¤ - ľĆ±ş Äł¸ŻĹÍ °Ë»öÇĎ±â
		this->NPCGetAdjacentCharacterIndexes(&i_pMon->PositionVector, i_pMon->MonsterInfoPtr->AttackRange, i_pMon->MonsterInfoPtr->AttackRange*2, &tmCliIdxList, i_pMon->MonsterInfoPtr->Belligerence, FALSE);
		targetIdxList.insert(targetIdxList.end(), tmCliIdxList.begin(), tmCliIdxList.end());		// 2010-04-01 by cmkwon, ŔÎÇÇ2Â÷ Ăß°ˇ ĽöÁ¤ - 
	}
	
	// 2. BELL_INFINITY_ATTACK_MONSTER ŔÎ ¸ó˝şĹÍżˇ ľĆ±şŔ» ĂŁŔÚ. BELL_INFINITY_ATTACK_MONSTER ľĆ±ş
	else if (BELL_INFINITY_ATTACK_MONSTER == i_pMon->MonsterInfoPtr->Belligerence)
	{
		// ÁÖŔ§żˇĽ­ BELL_INFINITY_DEFENSE_MONSTER ¸ó˝şĹÍ¸¦ °Ë»ö
		this->NPCGetAdjacentMonsterIndexesByBell(&i_pMon->PositionVector, i_pMon->MonsterInfoPtr->AttackRange, i_pMon->MonsterInfoPtr->AttackRange*2, BELL_INFINITY_ATTACK_MONSTER, &targetIdxList);
	}

	if(TRUE == i_bExceptMe)
	{	// ŔÚ±â ŔÚ˝ĹŔş »čÁ¦
		this->DeleteClientIdx(&targetIdxList, i_pMon->MonsterIndex);
	}
	
	// ´ë»óŔÚ ĂŁ±â
	if(TRUE == i_bRepair)
	{
		return this->GetPossibleRepairTarget(i_pMon, targetIdxList);
	}
	else 
	{
		return this->NPCGetTargetwithAttackObj(i_pMon->MonsterInfoPtr->AttackObject, i_pMon, targetIdxList);
	}

}

///////////////////////////////////////////////////////////////////////////////
/// \fn			void CNPCMapChannel::SetMonsterTargetInRangeByBell(CNPCMonster *i_pMon)
/// \brief		ŔÎÇÇ´ĎĆĽ(±âÁöąćľî) - 
/// \author		dhjin
/// \date		2010-03-31
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CNPCMapChannel::SetMonsterTargetInRangeByBell(CNPCMonster *i_pMon)
{
	if(NULL == i_pMon)
	{
		return;
	}

	vectorClientIndex	targetIdxList;
	targetIdxList.reserve(100);
	
	ClientIndex_t ChangeTargetClientIdx = i_pMon->m_nTargetIndex;

	this->NPCGetAdjacentMonsterIndexesByBell(&i_pMon->PositionVector, i_pMon->MonsterInfoPtr->AttackRange, i_pMon->MonsterInfoPtr->AttackRange*2, i_pMon->MonsterInfoPtr->Belligerence, &targetIdxList);

	CNPCMonster * ChangeTargetMonster = NULL;
	vectorClientIndex::iterator itr = targetIdxList.begin();
	for(;itr != targetIdxList.end();itr++)
	{
		ChangeTargetMonster = this->GetNPCMonster(*itr);
		if(NULL == ChangeTargetMonster
			|| FALSE == ChangeTargetMonster->IsValidMonster(TRUE))
		{
			continue;
		}
		ChangeTargetMonster->SelectTargetIndex(ChangeTargetClientIdx);
	}

}

///////////////////////////////////////////////////////////////////////////////
/// \fn			ClientIndex_t CNPCMapChannel::GetOurTagetByUsingItem(CNPCMonster *i_pMon)
/// \brief		ŔÎÇÇ´ĎĆĽ(±âÁöąćľî) - 
/// \author		dhjin
/// \date		2010-03-31
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
ClientIndex_t CNPCMapChannel::GetOurTagetByUsingItem(CNPCMonster *i_pMon)
{
	if(NULL == i_pMon->m_pUsingMonsterItem)
	{
		return FALSE;
	}
	ITEM *pAttItemInfo = i_pMon->m_pUsingMonsterItem->pItemInfo;
	if(NULL == pAttItemInfo
		|| ITEMKIND_FOR_MON_SKILL != pAttItemInfo->Kind)
	{
		return FALSE;
	}

	ClientIndex_t TargetIdx = 0;
	switch(pAttItemInfo->SkillTargetType)
	{
	case SKILLTARGETTYPE_ONE_OURS_INRANGE_WITHOUT_ME:
		{
			TargetIdx = this->GetTargetOur(i_pMon, this->CheckRepairDesParam(pAttItemInfo->ArrDestParameter[0]));
		}
		break;
	case SKILLTARGETTYPE_ALL_OURS_INRANGE_WITHOUT_ME:
		{
			TargetIdx = this->GetTargetOur(i_pMon, this->CheckRepairDesParam(pAttItemInfo->ArrDestParameter[0]));
		}
		break;
	default:
		{
		}
	}	

	return TargetIdx;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			BOOL CNPCMapChannel::CheckRepairDesParam(DestParam_t i_DestParam)
/// \brief		ŔÎÇÇ´ĎĆĽ(±âÁöąćľî) - 
/// \author		dhjin
/// \date		2010-03-31
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CNPCMapChannel::CheckRepairDesParam(DestParam_t i_DestParam)
{
	switch(i_DestParam)
	{
		case DES_IMMEDIATE_HP_UP:
		case DES_IMMEDIATE_DP_UP:
		case DES_IMMEDIATE_SP_UP:
		case DES_IMMEDIATE_EP_UP:
		case DES_IMMEDIATE_HP_OR_DP_UP:
		{
			return TRUE;
		}
		break;
	default:
		{
			return FALSE;
		}
	}

	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			ClientIndex_t CNPCMapChannel::GetPossibleRepairTarget(CMonster *pM, vector<ClientIndex_t> &ClientIndexVector)
/// \brief		ŔÎÇÇ´ĎĆĽ(±âÁöąćľî) - 
/// \author		dhjin
/// \date		2010-03-31
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
ClientIndex_t CNPCMapChannel::GetPossibleRepairTarget(CMonster *pM, vector<ClientIndex_t> &ClientIndexVector)
{
	CLIENT_INFO		*pCli = NULL;
	CNPCMonster		*pNPCMon = NULL;
	BOOL			bRecognizeInvisible = COMPARE_MPOPTION_BIT(pM->MonsterInfoPtr->MPOption, MPOPTION_BIT_RECOGNIZE_INVISIBLE);
	int				nRetTarget = 0;
	float			fDistance;
	float			fMinDistance = this->GetMonsterVisibleDiameterW();
	float			fMinHPRate = 100.0f;
	vectClientIndex_t::iterator itr(ClientIndexVector.begin());
	while(itr != ClientIndexVector.end())
	{
		pCli		= NULL;
		pNPCMon		= NULL;
		ClientIndex_t tarIdx = *itr;
		
		if(FALSE == this->GetUnitObject(tarIdx, &pCli, &pNPCMon))
		{
			itr = ClientIndexVector.erase(itr);
			continue;
		}
		if(pCli)
		{	// 1. ŔŻ´Ö ±âş» ĂĽĹ©
			if(FALSE == pCli->IsValidClient()
				|| FALSE == pCli->IsEnbleTargeted(pM->MonsterInfoPtr->Belligerence, bRecognizeInvisible)
				|| FALSE != CheckImpactStraightLineMapAndObjects(&pM->PositionVector, &pCli->PositionVector, DEFAULT_OBJECT_MONSTER_OBJECT+pM->MonsterInfoPtr->MonsterUnitKind))
			{
				itr = ClientIndexVector.erase(itr);
				continue;
			}

			// ÇöŔç Ĺ¸°Ů, ÇÇ ą× °Ĺ¸® ĂĽĹ©
			fDistance = D3DXVec3Length(&(pCli->PositionVector - pM->PositionVector));
			if(tarIdx != pM->m_nTargetIndex
				&& fDistance < fMinDistance
				)
			{
				nRetTarget		= tarIdx;
				fMinDistance	= fDistance;
			}
		}
		if(pNPCMon)
		{	// 1. ŔŻ´Ö ±âş» ĂĽĹ©
			if(FALSE == pNPCMon->IsValidMonster()
				|| FALSE != CheckImpactStraightLineMapAndObjects(&pM->PositionVector, &pNPCMon->PositionVector, DEFAULT_OBJECT_MONSTER_OBJECT+pM->MonsterInfoPtr->MonsterUnitKind))
			{
				itr = ClientIndexVector.erase(itr);
				continue;
			}

			// ÇöŔç Ĺ¸°Ů,ÇÇ ą× °Ĺ¸® ĂĽĹ©
			fDistance = D3DXVec3Length(&(pNPCMon->PositionVector - pM->PositionVector));
			if(tarIdx != pM->m_nTargetIndex
				&& fDistance < fMinDistance
				&& (pNPCMon->CurrentHP*100/pNPCMon->MonsterInfoPtr->MonsterHP) < fMinHPRate)
			{
				nRetTarget		= tarIdx;
				fMinDistance	= fDistance;
				fMinHPRate		= pNPCMon->CurrentHP;
			}
		}
		itr++;
	}
	if(ClientIndexVector.empty()){					return 0;}
	
	if(ClientIndexVector.size() == 1)
	{
		return ClientIndexVector[0];
	}

	return nRetTarget;
}

// start 2011-05-23 by hskim, ŔÎÇÇ´ĎĆĽ 3Â÷ - żţŔĚĆ÷ŔÎĆ® ±¸Çö
CNPCIOCP *CNPCMapChannel::GetNPCIOCPServer()
{ 
	return m_pNPCIOCPServer; 
}
// end 2011-05-23 by hskim, ŔÎÇÇ´ĎĆĽ 3Â÷ - żţŔĚĆ÷ŔÎĆ® ±¸Çö
