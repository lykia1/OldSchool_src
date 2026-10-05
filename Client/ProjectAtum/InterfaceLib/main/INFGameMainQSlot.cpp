//05-06-2020 - Cleaned by Inetpub
#include "StdAfx.h"
#include "INFGameMainQSlot.h"
#include "AtumApplication.h"
#include "INFTrade.h"
#include "INFIcon.h"
#include "StoreData.h"
#include "INFCharacterInfoExtend.h"
#include "AtumDatabase.h"
#include "ShuttleChild.h"
#include "CharacterChild.h"
#include "Interface.h"
#include "INFGameMain.h"
#include "Chat.h"
#include "GameDataLast.h"
#include "KeyBoardInput.h"
#include "ItemInfo.h"
#include "INFInven.h"
#include "INFImage.h"
#include "INFCityBase.h"
#include "Skill.h"
#include "D3DHanFont.h"
#include "SkillInfo.h"

#define QSLOT_START_X			((g_pD3dApp->GetBackBufferDesc().Width - QSLOT_SIZE_X)/2)
#define QSLOT_START_Y			(g_pD3dApp->GetBackBufferDesc().Height - 34)
#define REAL_TAB_NUMBER			3
#define	BAZAAR_CLICK_TIME		2.0f

#if defined(LANGUAGE_ENGLISH) || defined(LANGUAGE_VIETNAM)|| defined(LANGUAGE_THAI)
#define FONTLINE_X				0
#define FONTLINE_Y				-1
#else
#define FONTLINE_X				1
#define FONTLINE_Y				1
#endif

#define QSLOT_BUTTON_UP_START_X			(m_nX + QSLOT_SIZE_X + 4)
#define QSLOT_BUTTON_UP_START_Y			(m_nY + 0)
#define QSLOT_BUTTON_DOWN_START_X		(m_nX + QSLOT_SIZE_X + 4)
#define QSLOT_BUTTON_DOWN_START_Y		(m_nY + 23)
#define QSLOT_BUTTON_SIZE_X				8
#define QSLOT_BUTTON_SIZE_Y				8

CINFGameMainQSlot::CINFGameMainQSlot(CAtumNode* pParent): m_fCheckQuiclSlotSave(0)
{
	m_pParent = pParent;
	m_pBack = nullptr;
	m_pNumber = nullptr;
	m_pImgDisSkill = nullptr;
	m_nRenderMoveIconIntervalWidth = 0;
	m_nRenderMoveIconIntervalHeight = 0;
	memset(m_pQSlotInfo, 0x00, sizeof(INVEN_DISPLAY_INFO) * QSLOT_NUMBER * QSLOT_TAB_NUMBER);
	m_nX = 0;
	m_nY = 0;
	m_nCurrentTab = 0;
	m_bRestored = FALSE;
	m_nSelectSlotNumber = -1;
	m_fQSlotTimer = 0;
	// 2015-02-26 by jwLee CINFGameMainQSlot::m_pImgTabButton변수 초기화 방법 변경
	//memset(m_pImgTabButton, 0x00, sizeof(DWORD)*QSLOT_BUTTON_STATE_NUMBER*QSLOT_BUTTON_NUMBER);
	int i = 0;
	for (i = 0; i < QSLOT_BUTTON_NUMBER; i++)
	{
		m_pImgTabButton[i] = NULL;
	}
	// end 2015-02-26 by jwLee CINFGameMainQSlot::m_pImgTabButton변수 초기화 방법 변경
	m_nButtonState[QSLOT_BUTTON_UP] = BUTTON_STATE_NORMAL;
	m_nButtonState[QSLOT_BUTTON_DOWN] = BUTTON_STATE_NORMAL;

	for (auto& i : m_vecFontLine)
	{
		i = nullptr;
	}
	for (auto& i : m_vecFontKitCD)
	{
		i = nullptr;
	}

	m_pSelectItem.pItem = nullptr;
	m_nQSlotSwapTab = 0;
	m_nQSlotSwapNum = 0;
	m_bQSlotSwapFlag = FALSE;
	m_nItemType = QSLOT_ITEMTYPE_NONE;

	m_pQSlotMove = nullptr;
	m_bLButtonDown = FALSE;
	m_pFontTabNum = nullptr;
	m_fClickBazaar = -1;

	m_vecJoystikcSkillList.clear();
	m_fJoystikcSkillList = 0.0f;

	for(auto i = 0; i< QSLOT_TAB_NUMBER; i++)
		for (auto j = 0; j < QSLOT_NUMBER; j++)
			m_pImgCooldown[i][j] = NULL;
}

CINFGameMainQSlot::~CINFGameMainQSlot()
{
	int i;
	SAFE_DELETE(m_pBack);
	SAFE_DELETE(m_pNumber);
	SAFE_DELETE(m_pImgDisSkill);
	for (i = 0; i < QSLOT_BUTTON_NUMBER; i++)
	{
		SAFE_DELETE(m_pImgTabButton[i]);
	}

	for(i=0; i<QSLOT_NUMBER; i++) {
		SAFE_DELETE(m_vecFontLine[i]);
	}
	for(i=0; i<QSLOT_NUMBER; i++) {
		SAFE_DELETE(m_vecFontKitCD[i]);
	}
	
	SAFE_DELETE(m_pFontTabNum);

	for(i=0;i<QSLOT_TAB_NUMBER;i++) {
		for(auto j=0;j<QSLOT_NUMBER;j++) {
			SAFE_DELETE(m_pQSlotInfo[i][j].pItem);
		}
	}

	for (auto i = 0; i < QSLOT_TAB_NUMBER; i++)
		for (auto j = 0; j < QSLOT_NUMBER; j++)
			SAFE_DELETE(m_pImgCooldown[i][j])
}

HRESULT CINFGameMainQSlot::InitDeviceObjects()
{
	FLOG( "CINFGameMainQSlot::InitDeviceObjects()" );

	int i;

	auto* pDataHeader = FindResource("mnQSlot");
	m_pBack = new CINFImage;
	m_pBack->InitDeviceObjects(pDataHeader->m_pData, pDataHeader->m_DataSize) ;
	
	m_pNumber = new CINFImage;
	pDataHeader = FindResource("mnQSlotN");
	m_pNumber->InitDeviceObjects(pDataHeader->m_pData, pDataHeader->m_DataSize) ;
	
	m_pImgDisSkill = new CINFImage;
	pDataHeader = FindResource("diskill");
	m_pImgDisSkill->InitDeviceObjects(pDataHeader->m_pData, pDataHeader->m_DataSize) ;

	m_pImgTabButton[QSLOT_BUTTON_UP] = new CINFImageBtn;
	m_pImgTabButton[QSLOT_BUTTON_UP]->InitDeviceObjects("mnQdn03", "mnQdn01", "mnQdn00", "mnQdn02");
	
	m_pImgTabButton[QSLOT_BUTTON_DOWN] = new CINFImageBtn;
	m_pImgTabButton[QSLOT_BUTTON_DOWN]->InitDeviceObjects("mnQup03", "mnQup01", "mnQup00", "mnQup02");

	for(i=0; i<QSLOT_NUMBER; i++) {
		m_vecFontLine[i] = new CD3DHanFont(_T(g_pD3dApp->GetFontStyle()),8, D3DFONT_ZENABLE,  TRUE,256,32);
		m_vecFontLine[i]->InitDeviceObjects(g_pD3dDev);
	}
	for(i=0; i<QSLOT_NUMBER; i++) {
		m_vecFontKitCD[i] = new CD3DHanFont(_T(g_pD3dApp->GetFontStyle()),8, D3DFONT_ZENABLE,  TRUE,256,32);
		m_vecFontKitCD[i]->InitDeviceObjects(g_pD3dDev);
	}

	m_pFontTabNum = new CD3DHanFont(_T(g_pD3dApp->GetFontStyle()),8, D3DFONT_ZENABLE,  TRUE,256,32);
	m_pFontTabNum->InitDeviceObjects(g_pD3dDev);

	for (auto i = 0; i < QSLOT_TAB_NUMBER; i++)
	{
		for (auto j = 0; j < QSLOT_NUMBER; j++)
		{
			pDataHeader = m_pGameData->Find("cd_timer");
			m_pImgCooldown[i][j] = new CINFImage;
			m_pImgCooldown[i][j]->InitDeviceObjects(pDataHeader->m_pData, pDataHeader->m_DataSize);
		}
	}

	return S_OK ;
}

HRESULT CINFGameMainQSlot::RestoreDeviceObjects()
{
	FLOG( "CINFGameMainQSlot::RestoreDeviceObjects()" );
	if(!m_bRestored) {
		m_nX = QSLOT_START_X;
		m_nY = QSLOT_START_Y;
		m_pBack->RestoreDeviceObjects() ;
		m_pNumber->RestoreDeviceObjects() ;
		m_pImgDisSkill->RestoreDeviceObjects() ;
		m_pImgTabButton[QSLOT_BUTTON_UP]->RestoreDeviceObjects();
		m_pImgTabButton[QSLOT_BUTTON_UP]->SetBtnPosition(QSLOT_BUTTON_DOWN_START_X, QSLOT_BUTTON_DOWN_START_Y);
		m_pImgTabButton[QSLOT_BUTTON_DOWN]->RestoreDeviceObjects();
		m_pImgTabButton[QSLOT_BUTTON_DOWN]->SetBtnPosition(QSLOT_BUTTON_UP_START_X, QSLOT_BUTTON_UP_START_Y);
		m_bRestored = TRUE;
	}
	
	for (auto& i : m_vecFontLine) {
		i->RestoreDeviceObjects() ;
	}
	for (auto& i : m_vecFontKitCD) {
		i->RestoreDeviceObjects() ;
	}
	
	m_pFontTabNum->RestoreDeviceObjects() ;

	for (auto i = 0; i < QSLOT_TAB_NUMBER; i++)
		for (auto j = 0; j < QSLOT_NUMBER; j++)
			m_pImgCooldown[i][j]->RestoreDeviceObjects();

	return S_OK ;
}

HRESULT CINFGameMainQSlot::DeleteDeviceObjects()
{
	FLOG( "CINFGameMainQSlot::DeleteDeviceObjects()" );
	int i;

	m_pBack->DeleteDeviceObjects() ;
	SAFE_DELETE(m_pBack );	
	m_pImgDisSkill->DeleteDeviceObjects() ;
	SAFE_DELETE(m_pImgDisSkill );
	m_pNumber->DeleteDeviceObjects() ;
	SAFE_DELETE(m_pNumber );

	m_pImgTabButton[QSLOT_BUTTON_UP]->InvalidateDeviceObjects();
	m_pImgTabButton[QSLOT_BUTTON_DOWN]->InvalidateDeviceObjects();
	SAFE_DELETE(m_pImgTabButton[QSLOT_BUTTON_UP]);
	SAFE_DELETE(m_pImgTabButton[QSLOT_BUTTON_DOWN]);

	for(i=0; i<QSLOT_NUMBER; i++) {
		m_vecFontLine[i]->DeleteDeviceObjects() ;
		SAFE_DELETE(m_vecFontLine[i]);
	}
	for(i=0; i<QSLOT_NUMBER; i++) {
		m_vecFontKitCD[i]->DeleteDeviceObjects() ;
		SAFE_DELETE(m_vecFontKitCD[i]);
	}
	
	m_pFontTabNum->DeleteDeviceObjects() ;
	SAFE_DELETE(m_pFontTabNum);

	for (auto i = 0; i < QSLOT_TAB_NUMBER; i++)
	{
		for (auto j = 0; j < QSLOT_NUMBER; j++)
		{
			m_pImgCooldown[i][j]->DeleteDeviceObjects();
			SAFE_DELETE(m_pImgCooldown[i][j]);
		}
	}

	return S_OK ;
}


HRESULT CINFGameMainQSlot::InvalidateDeviceObjects()
{
	FLOG( "CINFGameMainQSlot::InvalidateDeviceObjects()" );
	if(m_bRestored) {
		m_pBack->InvalidateDeviceObjects();
		m_pNumber->InvalidateDeviceObjects();
		m_pImgDisSkill->InvalidateDeviceObjects();
		
		m_pImgTabButton[QSLOT_BUTTON_UP]->InvalidateDeviceObjects();
		m_pImgTabButton[QSLOT_BUTTON_DOWN]->InvalidateDeviceObjects();
		m_bRestored = FALSE;
	}

	for (auto& i : m_vecFontLine) {
		i->InvalidateDeviceObjects();
	}
	for (auto& i : m_vecFontKitCD) {
		i->InvalidateDeviceObjects();
	}
	
	m_pFontTabNum->InvalidateDeviceObjects();

	for (auto i = 0; i < QSLOT_TAB_NUMBER; i++)
		for (auto j = 0; j < QSLOT_NUMBER; j++)
			m_pImgCooldown[i][j]->InvalidateDeviceObjects();
		
	return S_OK ;
}


void CINFGameMainQSlot::Tick()
{
	for (auto i = 0; i < QSLOT_TAB_NUMBER; i++) {
		for (auto j = 0; j < QSLOT_NUMBER; j++) {
			if (m_pQSlotInfo[i][j].pItem && IsValidQSlotInfo(i, j) && m_nCurrentTab != i) {
				auto* pItemInfo = g_pStoreData->FindItemInInventoryByItemNum(m_pQSlotInfo[i][j].pItem->ItemNum);
				bool bIsTicking = false;
				for (int cur = 0; cur < QSLOT_NUMBER; cur++)
					if (m_pQSlotInfo[m_nCurrentTab][cur].pItem && IsValidQSlotInfo(m_nCurrentTab, cur) && pItemInfo && m_pQSlotInfo[m_nCurrentTab][cur].pItem->ItemNum == pItemInfo->ItemNum)
						bIsTicking = true;

				if (pItemInfo && !bIsTicking) {
					auto fElapsedTime = g_pD3dApp->GetElapsedTime();
					pItemInfo->TickReUsable(fElapsedTime);
				}
			}
		}
	}
	FLOG("CINFGameMainQSlot::Tick()");
	if (g_pShuttleChild->IsOperAndObser()) {
		return;
	}

	if (m_fClickBazaar > 0) {
		m_fClickBazaar -= g_pD3dApp->GetCheckElapsedTime();
	}

	if (m_fQSlotTimer > 0) {
		m_fQSlotTimer -= g_pD3dApp->GetElapsedTime();
	}

	if (g_pD3dApp->m_dwGameState == _CITY &&
		g_pInterface->m_pCityBase->GetCurrentBuildingNPC() != nullptr) {
		return;
	}
#ifdef _CTRL_TAB_SWITCH_QSLOTTAB
	if (g_pShuttleChild->m_bCtrlKey) {
		if (g_pD3dApp->GetAsyncKeyState_DIK_DIJ(DIK_1) && g_pD3dApp->m_pKeyBoard->GetAsyncKeyState(DIK_1))
		{
			m_nCurrentTab = 0;
			m_nSelectSlotNumber = -1;
			SetSelectItem(nullptr);
			m_nButtonState[QSLOT_BUTTON_DOWN] = BUTTON_STATE_DOWN;
			return;
		}
		if (g_pD3dApp->GetAsyncKeyState_DIK_DIJ(DIK_2) && g_pD3dApp->m_pKeyBoard->GetAsyncKeyState(DIK_2))
		{
			m_nCurrentTab = 1;
			m_nSelectSlotNumber = -1;
			SetSelectItem(nullptr);
			m_nButtonState[QSLOT_BUTTON_DOWN] = BUTTON_STATE_DOWN;
			return;
		}
		if (g_pD3dApp->GetAsyncKeyState_DIK_DIJ(DIK_3) && g_pD3dApp->m_pKeyBoard->GetAsyncKeyState(DIK_3))
		{
			m_nCurrentTab = 2;
			m_nSelectSlotNumber = -1;
			SetSelectItem(nullptr);
			m_nButtonState[QSLOT_BUTTON_DOWN] = BUTTON_STATE_DOWN;
			return;
		}	
	}
#endif
	if (m_fQSlotTimer <= 0 &&
		g_pInterface->m_pCityBase &&
		g_pInterface->m_pCityBase->GetCurrentBuildingNPC() == nullptr &&
		!dynamic_cast<CINFGameMain*>(m_pParent)->m_pTrade->m_bTrading &&
		!g_pD3dApp->m_bChatMode &&
		g_pShuttleChild->CheckUnitState() == FLIGHT)
	{

		if (g_pD3dApp->GetAsyncKeyState_DIK_DIJ(DIK_NUMPAD0)) {
			if (g_pD3dApp->m_pKeyBoard->GetAsyncKeyState(DIK_NUMPAD0)) {
				UseQuickSlot(m_nCurrentTab, 9);
			}
		}

		if (g_pD3dApp->GetAsyncKeyState_DIK_DIJ(DIK_NUMPAD1)) {
			if (g_pD3dApp->m_pKeyBoard->GetAsyncKeyState(DIK_NUMPAD1)) {
				UseQuickSlot(m_nCurrentTab, 0);
			}
		}
		if (g_pD3dApp->GetAsyncKeyState_DIK_DIJ(DIK_NUMPAD2)) {
			if (g_pD3dApp->m_pKeyBoard->GetAsyncKeyState(DIK_NUMPAD2)) {
				UseQuickSlot(m_nCurrentTab, 1);
			}
		}
		if (g_pD3dApp->GetAsyncKeyState_DIK_DIJ(DIK_NUMPAD3)) {
			if (g_pD3dApp->m_pKeyBoard->GetAsyncKeyState(DIK_NUMPAD3)) {
				UseQuickSlot(m_nCurrentTab, 2);
			}
		}
		if (g_pD3dApp->GetAsyncKeyState_DIK_DIJ(DIK_NUMPAD4)) {
			if (g_pD3dApp->m_pKeyBoard->GetAsyncKeyState(DIK_NUMPAD4)) {
				UseQuickSlot(m_nCurrentTab, 3);
			}
		}
		if (g_pD3dApp->GetAsyncKeyState_DIK_DIJ(DIK_NUMPAD5)) {
			if (g_pD3dApp->m_pKeyBoard->GetAsyncKeyState(DIK_NUMPAD5)) {
				UseQuickSlot(m_nCurrentTab, 4);
			}
		}
		if (g_pD3dApp->GetAsyncKeyState_DIK_DIJ(DIK_NUMPAD6)) {
			if (g_pD3dApp->m_pKeyBoard->GetAsyncKeyState(DIK_NUMPAD6)) {
				UseQuickSlot(m_nCurrentTab, 5);
			}
		}
		if (g_pD3dApp->GetAsyncKeyState_DIK_DIJ(DIK_NUMPAD7)) {
			if (g_pD3dApp->m_pKeyBoard->GetAsyncKeyState(DIK_NUMPAD7)) {
				UseQuickSlot(m_nCurrentTab, 6);
			}
		}
		if (g_pD3dApp->GetAsyncKeyState_DIK_DIJ(DIK_NUMPAD8)) {
			if (g_pD3dApp->m_pKeyBoard->GetAsyncKeyState(DIK_NUMPAD8)) {
				UseQuickSlot(m_nCurrentTab, 7);
			}
		}
		if (g_pD3dApp->GetAsyncKeyState_DIK_DIJ(DIK_NUMPAD9)) {
			if (g_pD3dApp->m_pKeyBoard->GetAsyncKeyState(DIK_NUMPAD9)) {
				UseQuickSlot(m_nCurrentTab, 8);
			}
		}
		for (auto i = 0; i < QSLOT_NUMBER; i++) {
			if (g_pD3dApp->GetAsyncKeyState_DIK_DIJ(DIK_1 + i)) {
				if (g_pD3dApp->m_pKeyBoard->GetAsyncKeyState(DIK_1 + i)) {
					UseQuickSlot(m_nCurrentTab, i);
				}
			}
		}
	}

	TickJoystickSlot();
	TickCheckQuickSlotSave();
}
void CINFGameMainQSlot::TickCheckQuickSlotSave()
{
	if (m_fCheckQuiclSlotSave <= 0.0f) {
		return;
	}
	
	if (g_pD3dApp->GetArenaState() == ARENA_STATE_ARENA_GAMING) {
		return;
	}
	
	m_fCheckQuiclSlotSave -= g_pD3dApp->GetCheckElapsedTime();

	if (m_fCheckQuiclSlotSave <= 0.0f && g_pD3dApp && g_pD3dApp->m_pInterface) {
		m_fCheckQuiclSlotSave = 0.0f;
		DBGOUT("Slots saved!\r\n");
		g_pD3dApp->m_pInterface->SaveCharacterFile();
	}
}

void CINFGameMainQSlot::SetCheckQuickSlotSave(const float fCheckQuickSlotSave)
{
	m_fCheckQuiclSlotSave = fCheckQuickSlotSave;
}

void CINFGameMainQSlot::Render()
{

	if(g_pShuttleChild->IsObserverMode()) {		
		return;
	}

	FLOG( "CINFGameMainQSlot::Render()" );
	char buf[64];
	int i;
	auto* pIconInfo = dynamic_cast<CINFGameMain*>(m_pParent)->m_pIcon;
	//SIZE iconSize = pIconInfo->GetIconSize();
	m_pBack->Move(m_nX, m_nY);
	m_pBack->Render();
	for (i = 0; i < QSLOT_NUMBER; i++) {
		if (m_pQSlotInfo[m_nCurrentTab][i].pItem && IsValidQSlotInfo(m_nCurrentTab, i)) {
		//	strcpy_s(buf, m_pQSlotInfo[m_nCurrentTab][i].IconName);

			pIconInfo->Render(atol(m_pQSlotInfo[m_nCurrentTab][i].IconName),
							   QSLOT_ICON_INTERVAL * i + m_nX + 1,
							   m_nY + 1, 1.0f);
			//pIconInfo->Render();

			if (IS_SKILL_ITEM(m_pQSlotInfo[m_nCurrentTab][i].pItem->Kind)
				&& FALSE == RenderDisableSkill(m_pQSlotInfo[m_nCurrentTab][i].pItem->ItemNum)) {
				m_pImgDisSkill->Move(QSLOT_ICON_INTERVAL * i + m_nX, m_nY);
				m_pImgDisSkill->Render();
			}
		}
	}

	m_pNumber->Move(m_nX, m_nY);
	m_pNumber->Render();
	
	for(i=0;i<QSLOT_NUMBER;i++) {
		if(m_pQSlotInfo[m_nCurrentTab][i].pItem && IsValidQSlotInfo(m_nCurrentTab, i)) {
			if(IS_SKILL_ITEM(m_pQSlotInfo[m_nCurrentTab][i].pItem->Kind))
				RenderSkillReAttackTime(m_pQSlotInfo[m_nCurrentTab][i].pItem->ItemNum, i);
			else if(ITEMKIND_CARD == m_pQSlotInfo[m_nCurrentTab][i].pItem->Kind
				&& (COMPARE_BIT_FLAG(m_pQSlotInfo[m_nCurrentTab][i].pItem->ItemInfo->ItemAttribute, ITEM_ATTR_TIME_LIMITE) 
					|| COMPARE_BIT_FLAG(m_pQSlotInfo[m_nCurrentTab][i].pItem->ItemInfo->ItemAttribute, ITEM_ATTR_DELETED_TIME_LIMITE_AFTER_USED))
				&& 0 < m_pQSlotInfo[m_nCurrentTab][i].pItem->ItemInfo->ReAttacktime)
				RenderItemUsableReAttackTime(m_pQSlotInfo[m_nCurrentTab][i].pItem->ItemNum, i);
			else if(ITEMKIND_ENERGY == m_pQSlotInfo[m_nCurrentTab][i].pItem->Kind && 0 < m_pQSlotInfo[m_nCurrentTab][i].pItem->ItemInfo->ReAttacktime)			
				RenderItemUsableReAttackTime(m_pQSlotInfo[m_nCurrentTab][i].pItem->ItemNum, i,true);
				
			if( IS_COUNTABLE_ITEM(m_pQSlotInfo[m_nCurrentTab][i].pItem->Kind) ) {
				auto* const pItemInfo = g_pStoreData->FindItemInInventoryByUniqueNumber( m_pQSlotInfo[m_nCurrentTab][i].pItem->UniqueNumber );
				if( pItemInfo->CurrentCount > 1 ) {
					
						if(pItemInfo->CurrentCount >= 1000000000)
							sprintf(buf,"%.0fB",(float)(pItemInfo->CurrentCount/1000000000.0f));
						else if(pItemInfo->CurrentCount >= 100000000)
							sprintf(buf,"%.0fM",(float)(pItemInfo->CurrentCount/1000000.0f));
						else if(pItemInfo->CurrentCount >= 1000000)
							sprintf(buf,"%.0fM",(float)(pItemInfo->CurrentCount/1000000.0f));
						else if(pItemInfo->CurrentCount >= 100000)
							sprintf(buf,"%.0fk",(float)(pItemInfo->CurrentCount/1000.0f));
						else if(pItemInfo->CurrentCount >= 10000)
							sprintf(buf,"%.0fk",(float)(pItemInfo->CurrentCount/1000.0f));
						else if(pItemInfo->CurrentCount >= 1000)
							sprintf(buf,"%.0fk",(float)(pItemInfo->CurrentCount/1000.0f));
						else
							sprintf(buf, "%d",pItemInfo->CurrentCount);
					
					//wsprintf(buf, "%d",pItemInfo->CurrentCount);
					const auto len = lstrlen(buf) - 1;
					m_vecFontLine[i]->DrawText(m_nX+QSLOT_ICON_INTERVAL*i+21 - len * 6.0f,m_nY-1,RGB(170,255,200),buf, 0L);
				}
			}
		}
	}

	wsprintf(buf, "%d",m_nCurrentTab+1);
	m_pFontTabNum->DrawText(QSLOT_BUTTON_UP_START_X + 1,
								QSLOT_BUTTON_UP_START_Y + QSLOT_BUTTON_SIZE_Y , 
								QSLOT_COUNTERBLE_NUMBER,buf, 0L);

	for( i=0; i<QSLOT_NUMBER; ++i ) {
		if(m_pQSlotInfo[m_nCurrentTab][i].pItem && IsValidQSlotInfo(m_nCurrentTab, i)) {
			if( m_pQSlotInfo[m_nCurrentTab][i].pItem->ItemInfo->InvokingDestParamID
				|| m_pQSlotInfo[m_nCurrentTab][i].pItem->ItemInfo->InvokingDestParamIDByUse)
			{
				char strText[128]; *strText = '\0';
				auto* const pItemInfo = g_pStoreData->FindItemInWearByItemNum(m_pQSlotInfo[m_nCurrentTab][i].pItem->ItemNum);
				
				if( pItemInfo && GetString_CoolTime( pItemInfo, strText ) ) {
					const auto len = lstrlen(strText) - 1;
					const auto nFontPosX = m_nX + QSLOT_ICON_INTERVAL * i + FONTLINE_X + 20 - len * 6.0f;
					const auto nFontPosY = m_nY + FONTLINE_Y + 7;

					m_vecFontLine[i]->DrawText(nFontPosX,nFontPosY, QSLOT_COUNTERBLE_NUMBER,strText, 0L);
				}
				memset(strText, 0x00, sizeof(strText));
			}
		}
	}
	if (m_nCurrentTab ==0) {
		m_pImgTabButton[QSLOT_BUTTON_UP]->Render();
	}
	else if(m_nCurrentTab > 0 && m_nCurrentTab < 2) {
		m_pImgTabButton[QSLOT_BUTTON_UP]->Render();
		m_pImgTabButton[QSLOT_BUTTON_DOWN]->Render();
	}
	else
	{
		m_pImgTabButton[QSLOT_BUTTON_DOWN]->Render();
	}
	/*if (m_pIsSlotOpen == TRUE)
	{
		m_pImgTabButton[QSLOT_BUTTON_UP]->Render();
	}
	else
	{
		m_pImgTabButton[QSLOT_BUTTON_DOWN]->Render();
	}*/
	if(m_nSelectSlotNumber != -1 && 
		m_pQSlotInfo[m_nCurrentTab][m_nSelectSlotNumber].IconName[0]) {
		POINT ptCursor;
		GetCursorPos( &ptCursor );
		ScreenToClient( g_pD3dApp->GetHwnd(), &ptCursor );
		CheckMouseReverse(&ptCursor);
		g_pGameMain->m_bQSlotIconFlag = TRUE;
		g_pGameMain->m_nQSlotPosX = ptCursor.x - m_nRenderMoveIconIntervalWidth;
		g_pGameMain->m_nQSlotPosY = ptCursor.y - m_nRenderMoveIconIntervalHeight;
	}
	else {
		g_pGameMain->m_bQSlotIconFlag = FALSE;
	}
}

void CINFGameMainQSlot::SetToolTip(int x, int y, ITEM_BASE* pItem) const
{
	if(pItem) {
		char buf[256];
		memset(buf, 0x00, sizeof(buf));
		if(IS_SKILL_ITEM(pItem->Kind) == FALSE) {
			auto* const pItemInfo = g_pStoreData->FindItemInInventoryByUniqueNumber(pItem->UniqueNumber);
			if(pItemInfo)
			{
				auto* itemInfo = pItemInfo->GetItemInfo();
				if(itemInfo)
				{
					if(IS_COUNTABLE_ITEM(pItem->Kind) && pItemInfo) {
						wsprintf(buf, STRMSG_C_TOOLTIP_0013,itemInfo->ItemName, pItemInfo->CurrentCount);
					}
					else {
						wsprintf(buf, "%s",itemInfo->ItemName);
					}
					const int nLength = lstrlen(buf)*6.5;
					dynamic_cast<CINFGameMain*>(m_pParent)->SetToolTip(x-nLength, y,buf);
				}
			}
		}
		else {
			auto* pSkill = g_pShuttleChild->m_pSkill->FindItem(pItem->ItemNum);
			if(pSkill) {
				const int nLength = lstrlen(pSkill->ItemName)*6.5;
				dynamic_cast<CINFGameMain*>(m_pParent)->SetToolTip(x-nLength, y,pSkill->ItemName);
			}
		}
	}
	else {
		dynamic_cast<CINFGameMain*>(m_pParent)->SetToolTip(0, 0,nullptr);
	}
}

int CINFGameMainQSlot::WndProc(const UINT uMsg, const WPARAM wParam, const LPARAM lParam)
{
	FLOG( "CINFGameMainQSlot::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)" );
	switch(uMsg)
	{
	case WM_MOUSEMOVE:
		{
			if(g_pShuttleChild->IsObserverMode()) {
				return INF_MSGPROC_NORMAL;
			}

			POINT pt;
			pt.x = LOWORD(lParam);
			pt.y = HIWORD(lParam);
			CheckMouseReverse(&pt);
			
			if(dynamic_cast<CINFGameMain*>(m_pParent)->m_stSelectItem.pSelectItem &&
				dynamic_cast<CINFGameMain*>(m_pParent)->m_stSelectItem.bySelectType == ITEM_QSLOT_POS) {
				m_bLButtonDown = TRUE;
			}

			if( pt.y > m_nY &&
				pt.y < m_nY+QSLOT_SIZE_Y &&
				pt.x > m_nX &&
				pt.x < m_nX+QSLOT_SIZE_X)
			{
				const int i = (pt.x - m_nX - 1)/QSLOT_ICON_INTERVAL;

				if( m_pQSlotInfo[m_nCurrentTab][i].pItem && 
					i >= 0 && 
					i < QSLOT_NUMBER )
				{					
					SetToolTip(pt.x-10, pt.y+13, m_pQSlotInfo[m_nCurrentTab][i].pItem);
					return INF_MSGPROC_BREAK;
				}

				dynamic_cast<CINFGameMain*>(m_pParent)->SetToolTip(0, 0,nullptr);			
			}
			else
			{
				dynamic_cast<CINFGameMain*>(m_pParent)->SetToolTip(0, 0,nullptr);
			}

			if(GetButtonStateOnMouse(pt, QSLOT_BUTTON_UP_START_X, QSLOT_BUTTON_UP_START_Y, QSLOT_BUTTON_SIZE_X, QSLOT_BUTTON_SIZE_Y))
			{
				m_nButtonState[QSLOT_BUTTON_UP] = BUTTON_STATE_UP;
				m_nButtonState[QSLOT_BUTTON_DOWN] = BUTTON_STATE_NORMAL;
			}
			else if(GetButtonStateOnMouse(pt, QSLOT_BUTTON_DOWN_START_X,QSLOT_BUTTON_DOWN_START_Y, QSLOT_BUTTON_SIZE_X, QSLOT_BUTTON_SIZE_Y))
			{
				m_nButtonState[QSLOT_BUTTON_UP] = BUTTON_STATE_NORMAL;
				m_nButtonState[QSLOT_BUTTON_DOWN] = BUTTON_STATE_UP;
			}
			else
			{
				m_nButtonState[QSLOT_BUTTON_UP] = BUTTON_STATE_NORMAL;
				m_nButtonState[QSLOT_BUTTON_DOWN] = BUTTON_STATE_NORMAL;
			}
		}
		break;
	case WM_RBUTTONDOWN:
		{
			if(g_pShuttleChild->IsObserverMode()) {
				return INF_MSGPROC_NORMAL;
			}

			POINT pt;
			pt.x = LOWORD(lParam);
			pt.y = HIWORD(lParam);
			CheckMouseReverse(&pt);
			
			if( pt.y>m_nY &&
				pt.y<m_nY+QSLOT_SIZE_Y &&
				pt.x > m_nX &&
				pt.x < m_nX+QSLOT_SIZE_X)
			{
				if(!g_pGameMain->m_nLeftWindowInfo && !g_pGameMain->m_nRightWindowInfo) {
					break;
				}

				const int i = (pt.x - m_nX - 1)/QSLOT_ICON_INTERVAL;
				if( m_pQSlotInfo[m_nCurrentTab][i].pItem &&
					i >= 0 && 
					i < QSLOT_NUMBER ) 
				{
					m_pQSlotInfo[m_nCurrentTab][i].pItem = nullptr;								
				}
			}
		}
		break;
	case WM_LBUTTONDOWN:
		{
			if(g_pShuttleChild->IsObserverMode()) {
				return INF_MSGPROC_NORMAL;
			}

			POINT pt;
			pt.x = LOWORD(lParam);
			pt.y = HIWORD(lParam);
			CheckMouseReverse(&pt);
			if( pt.y>m_nY &&
				pt.y<m_nY+QSLOT_SIZE_Y &&
				pt.x > m_nX &&
				pt.x < m_nX+QSLOT_SIZE_X)
			{
				if(!g_pGameMain->m_nLeftWindowInfo && !g_pGameMain->m_nRightWindowInfo) {
					break;
				}
				
				const int i = (pt.x - m_nX - 1)/QSLOT_ICON_INTERVAL;

				if(nullptr != m_pQSlotInfo[m_nCurrentTab][i].pItem &&
					IS_GENERAL_ITEM(m_pQSlotInfo[m_nCurrentTab][i].pItem->Kind) &&
					g_pInterface->m_pBazaarShop == nullptr) {
					if(IS_COUNTABLE_ITEM(m_pQSlotInfo[m_nCurrentTab][i].pItem->Kind)) {
						auto* const pItem = g_pDatabase->GetServerItemInfo(m_pQSlotInfo[m_nCurrentTab][i].pItem->ItemNum);
						if(pItem) {
							auto* const pItemInfo = g_pStoreData->FindItemInInventoryByUniqueNumber(
								static_cast<ITEM_GENERAL*>(m_pQSlotInfo[m_nCurrentTab][i].pItem)->UniqueNumber);
							if(pItemInfo == nullptr) {
								SetQSlotInfo(m_nCurrentTab,i, nullptr);
								m_bLButtonDown = FALSE;
								return INF_MSGPROC_BREAK;
							}
						}
					}
				}

				if(dynamic_cast<CINFGameMain*>(m_pParent)->m_stSelectItem.pSelectItem &&
					dynamic_cast<CINFGameMain*>(m_pParent)->m_stSelectItem.bySelectType == ITEM_QSLOT_POS) {
					m_bLButtonDown = TRUE;
					return INF_MSGPROC_BREAK;
				}

				m_nItemType	= QSLOT_ITEMTYPE_NONE;
				if( m_pQSlotInfo[m_nCurrentTab][i].pItem &&
					i >= 0 && 
					i < QSLOT_NUMBER )  {

					if(dynamic_cast<CINFGameMain*>(m_pParent)->m_stSelectItem.pSelectItem == nullptr) {
						m_nRenderMoveIconIntervalWidth  = pt.x - m_nX - QSLOT_ICON_INTERVAL * i + 1;
						m_nRenderMoveIconIntervalHeight = pt.y - m_nY + 1;
						m_nSelectSlotNumber = i;
						
						if(IS_SKILL_ITEM(m_pQSlotInfo[m_nCurrentTab][i].pItem->Kind)) {
							SetSelectItem(&m_pQSlotInfo[m_nCurrentTab][i]);
							m_nItemType				= QSLOT_ITEMTYPE_SKILL;
						}
						else {
							SetSelectItem(&m_pQSlotInfo[m_nCurrentTab][i]);
							m_nItemType				= QSLOT_ITEMTYPE_ITEM;
						}						
						m_bQSlotSwapFlag		= TRUE;
						m_nQSlotSwapTab			= m_nCurrentTab;
						m_nQSlotSwapNum			= i;

						m_pQSlotMove = m_pQSlotInfo[m_nCurrentTab][i].pItem;
						m_pQSlotInfo[m_nCurrentTab][i].pItem = nullptr;
					}
					else {
						m_bQSlotSwapFlag		= FALSE;
					}
					
					return INF_MSGPROC_BREAK;
				}
			}
			else
			{
				if(m_bLButtonDown &&
					dynamic_cast<CINFGameMain*>(m_pParent)->m_stSelectItem.bySelectType == ITEM_QSLOT_POS)
				{
					m_nSelectSlotNumber = -1;
					m_nItemType = QSLOT_ITEMTYPE_NONE;
					m_bLButtonDown = FALSE;
					SetSelectItem(nullptr);
					return INF_MSGPROC_BREAK;
				}
			}

			if(GetButtonStateOnMouse(pt, QSLOT_BUTTON_UP_START_X, QSLOT_BUTTON_UP_START_Y, QSLOT_BUTTON_SIZE_X, QSLOT_BUTTON_SIZE_Y)) {
				m_nButtonState[QSLOT_BUTTON_UP] = BUTTON_STATE_DOWN;
				m_nButtonState[QSLOT_BUTTON_DOWN] = BUTTON_STATE_NORMAL;
			}
			else if(GetButtonStateOnMouse(pt, QSLOT_BUTTON_DOWN_START_X,QSLOT_BUTTON_DOWN_START_Y, QSLOT_BUTTON_SIZE_X, QSLOT_BUTTON_SIZE_Y)) {
				m_nButtonState[QSLOT_BUTTON_UP] = BUTTON_STATE_NORMAL;
				m_nButtonState[QSLOT_BUTTON_DOWN] = BUTTON_STATE_DOWN;
			}
			else {
				m_nButtonState[QSLOT_BUTTON_UP] = BUTTON_STATE_NORMAL;
				m_nButtonState[QSLOT_BUTTON_DOWN] = BUTTON_STATE_NORMAL;
			}
		}
		break;
	case WM_LBUTTONUP:
		{
			if(g_pShuttleChild->IsObserverMode()) {
				return INF_MSGPROC_NORMAL;
			}

			POINT pt;
			pt.x = LOWORD(lParam);
			pt.y = HIWORD(lParam);
			CheckMouseReverse(&pt);
			if(g_pGameMain->m_pInven->m_bSelectWearItem) break;

			ITEM_BASE* pSelectItem = nullptr;
			if(dynamic_cast<CINFGameMain*>(m_pParent)->m_stSelectItem.pSelectItem) {
				pSelectItem = static_cast<ITEM_BASE*>(dynamic_cast<CINFGameMain*>(m_pParent)->m_stSelectItem.pSelectItem->pItem); 
			}

			if( (pSelectItem && 
				(m_bLButtonDown || dynamic_cast<CINFGameMain*>(m_pParent)->m_stSelectItem.bySelectType != ITEM_QSLOT_POS)) &&
				pt.y>m_nY &&
				pt.y<m_nY+QSLOT_SIZE_Y)
			{
				const int i = (pt.x - m_nX - 1)/QSLOT_ICON_INTERVAL;
				if( i >= 0 && 
					i < QSLOT_NUMBER ) 
				{
					SetQSlotInfo(m_nCurrentTab,i, pSelectItem);
					
					m_nSelectSlotNumber = -1;
					m_nItemType = QSLOT_ITEMTYPE_NONE;
					m_bLButtonDown = FALSE;

					SetSelectItem(nullptr);
					break;
				}
			}
			
			if( m_nSelectSlotNumber>=0 && 
				(pt.x < m_nX || 
				pt.x > m_nX+QSLOT_SIZE_X ||
				pt.y < m_nY || 
				pt.y > m_nY+QSLOT_SIZE_Y))
			{
				m_nItemType = QSLOT_ITEMTYPE_NONE;
				SetQSlotInfo(m_nCurrentTab,m_nSelectSlotNumber, nullptr);
				m_bLButtonDown = FALSE;
				SetSelectItem(nullptr);
			}
			m_nSelectSlotNumber = -1;

			if(GetButtonStateOnMouse(pt, QSLOT_BUTTON_UP_START_X, QSLOT_BUTTON_UP_START_Y, QSLOT_BUTTON_SIZE_X, QSLOT_BUTTON_SIZE_Y)) {
				if(m_nButtonState[QSLOT_BUTTON_UP] == BUTTON_STATE_DOWN) {
					if(m_nCurrentTab > 0)
						m_nCurrentTab --;
				}
				m_nButtonState[QSLOT_BUTTON_UP] = BUTTON_STATE_UP;
				m_nButtonState[QSLOT_BUTTON_DOWN] = BUTTON_STATE_NORMAL;
			}
			else if(GetButtonStateOnMouse(pt, QSLOT_BUTTON_DOWN_START_X,QSLOT_BUTTON_DOWN_START_Y, QSLOT_BUTTON_SIZE_X, QSLOT_BUTTON_SIZE_Y)) {
				if(m_nButtonState[QSLOT_BUTTON_DOWN] == BUTTON_STATE_DOWN) {
					if(m_nCurrentTab<REAL_TAB_NUMBER-1)
						m_nCurrentTab ++;
				}
				m_nButtonState[QSLOT_BUTTON_UP] = BUTTON_STATE_NORMAL;
				m_nButtonState[QSLOT_BUTTON_DOWN] = BUTTON_STATE_UP;
			}
			else {
				m_nButtonState[QSLOT_BUTTON_UP] = BUTTON_STATE_NORMAL;
				m_nButtonState[QSLOT_BUTTON_DOWN] = BUTTON_STATE_NORMAL;
			}
		}
		break;
	case WM_LBUTTONDBLCLK:
		{
			if(g_pShuttleChild->IsObserverMode()) {
				return INF_MSGPROC_NORMAL;
			}

			POINT pt;
			pt.x = LOWORD(lParam);
			pt.y = HIWORD(lParam);
			CheckMouseReverse(&pt);

			if( pt.x > m_nX && 
				pt.x < m_nX + QSLOT_SIZE_X &&
				pt.y>m_nY &&
				pt.y<m_nY+QSLOT_SIZE_Y)
			{
				const int i = (pt.x - m_nX - 1)/QSLOT_ICON_INTERVAL;
				if( m_pQSlotInfo[m_nCurrentTab][i].pItem && 
					i >= 0 && 
					i < QSLOT_NUMBER )
				{//antimacro
					/*time_t seconds;

					int nClicked = time(nullptr);
					if (nTicksSinceLastClick == 0) {
						nTicksSinceLastClick = nClicked;
					}
					else {
						const auto nRes = nClicked - nTicksSinceLastClick;
						vecLastClickTime.push_back(nRes);

						if (vecLastClickTime.size() > 10) {
							const set<int> s(vecLastClickTime.begin(), vecLastClickTime.end());

							if (vecLastClickTime.size() - s.size() >= 5) {
								MessageBox(nullptr, "Macro usage isn't allowed at OldSchoolRivals!", "OldSchoolRivals", MB_OK | MB_ICONERROR);
								FDBG("AntiCheat :: Trigger Macro 4Q");
							//	exit(0);
							}
							vecLastClickTime.clear();

							nTicksSinceLastClick = 0;
						}
					}

					if (lastPosClicked.x == pt.x && lastPosClicked.y == pt.y) {
						nClickCount++;
					}
					else {
						lastPosClicked = pt;
						nClickCount = 1;
					}
					if (nClickCount >= 5) {
						nClickCount = 0;
						MessageBox(nullptr, "Macro usage isn't allowed to create items at OldSchoolRivals! Code for support: 0x211", "OldSchoolRivals", MB_OK | MB_ICONERROR);
						FDBG("AntiCheat :: Trigger Macro 3Q");
						//exit(0);
					}*/

					UseQuickSlot(m_nCurrentTab, i);
				}
			}
		}
		break;
	case WM_KEYDOWN:
		{
			if(g_pShuttleChild->IsObserverMode()) {
				return INF_MSGPROC_NORMAL;
			}

			if(wParam == VK_TAB)
			{
				if(m_nCurrentTab<REAL_TAB_NUMBER-1) {
					m_nCurrentTab ++;
					m_nSelectSlotNumber = -1;
					SetSelectItem(nullptr);
				}
				else {
					m_nCurrentTab = 0;
					m_nSelectSlotNumber = -1;
					SetSelectItem(nullptr);
				}
				m_nButtonState[QSLOT_BUTTON_DOWN] = BUTTON_STATE_DOWN;
			}
		}
		break;
	case WM_KEYUP:
		{
			if(g_pShuttleChild->IsObserverMode()) {
				return INF_MSGPROC_NORMAL;
			}
			if (wParam == VK_LCONTROL) {
				m_nButtonState[QSLOT_BUTTON_DOWN] = BUTTON_STATE_NORMAL;
			}
			if(wParam == VK_TAB) {
				m_nButtonState[QSLOT_BUTTON_DOWN] = BUTTON_STATE_NORMAL;
			}
		}
		break;
	}
	return INF_MSGPROC_NORMAL;
}

BOOL CINFGameMainQSlot::UseQuickSlot(const int nCurrentTab, const int nSlotNumber)
{
	auto bUseSkill = FALSE;
	if(g_pShuttleChild->IsObserverMode()) {
		g_pD3dApp->m_pChat->CreateChatChild("Cannot use slot item! Err: 0x01", COLOR_SKILL_USE);
		return bUseSkill;
	}	

	if(FALSE == g_pD3dApp->IsLockMode()) {
		g_pD3dApp->m_pChat->CreateChatChild("Cannot use slot item! Err: 0x02", COLOR_SKILL_USE);
		return bUseSkill;
	}

	if( !IsPossibleJoystickSlot()) {
		return bUseSkill;
	}

	FLOG( "CINFGameMainQSlot::UseQuickSlot(int nCurrentTab, int nSlotNumber)" );
	if(IsValidQSlotInfo(nCurrentTab, nSlotNumber))
	{
		if(IS_GENERAL_ITEM(m_pQSlotInfo[nCurrentTab][nSlotNumber].pItem->Kind) &&
			g_pInterface->m_pBazaarShop == nullptr) {
			char buf[256];
			memset(buf, 0x00, sizeof(buf));
			if(IS_COUNTABLE_ITEM(m_pQSlotInfo[nCurrentTab][nSlotNumber].pItem->Kind)) {
				auto* const pItem = g_pDatabase->GetServerItemInfo(m_pQSlotInfo[nCurrentTab][nSlotNumber].pItem->ItemNum);
				if(pItem) {
					auto* const pItemInfo = g_pStoreData->FindItemInInventoryByUniqueNumber(
						static_cast<ITEM_GENERAL*>(m_pQSlotInfo[nCurrentTab][nSlotNumber].pItem)->UniqueNumber);
					if(pItemInfo) {
						wsprintf(buf, STRMSG_C_TOOLTIP_0013,pItem->ItemName,pItemInfo->CurrentCount);
					}
				}
			}
			else {
				auto* const pItem = g_pDatabase->GetServerItemInfo(m_pQSlotInfo[nCurrentTab][nSlotNumber].pItem->ItemNum);
				if(pItem)
					wsprintf(buf, "%s",pItem->ItemName);
			}
			if(strlen(buf) <= 0 ) {
				SetQSlotInfo( nCurrentTab, nSlotNumber, nullptr );
				return bUseSkill;
			}

			CItemInfo* pItemInfo;
			if( m_pQSlotInfo[nCurrentTab][nSlotNumber].pItem->ItemInfo->InvokingDestParamID
				|| m_pQSlotInfo[nCurrentTab][nSlotNumber].pItem->ItemInfo->InvokingDestParamIDByUse ) { 
				pItemInfo = g_pStoreData->FindItemInWearByItemNum( m_pQSlotInfo[nCurrentTab][nSlotNumber].pItem->ItemNum );
			}
			else
				pItemInfo = g_pStoreData->FindItemInInventoryByUniqueNumber( m_pQSlotInfo[nCurrentTab][nSlotNumber].pItem->UniqueNumber );

			if(pItemInfo) {
				if(pItemInfo->GetItemInfo()->AttackTime > 0) {
					if(g_pGameMain->PushDelayItem(pItemInfo))
						bUseSkill = TRUE;
				}
				else {
					dynamic_cast<CINFGameMain*>(m_pParent)->m_pInven->SendUseItem(pItemInfo);

					bUseSkill = TRUE;
				}
 			}	 

			m_fQSlotTimer = QSLOT_TIMER;
		}
		else if(IS_SKILL_ITEM(m_pQSlotInfo[nCurrentTab][nSlotNumber].pItem->Kind) && 
			g_pD3dApp->m_bChatMode == FALSE) {
			if(SKILL_BASE_NUM(m_pQSlotInfo[nCurrentTab][nSlotNumber].pItem->ItemNum) == AGEAR_SKILL_BASENUM_AIRSIEGEMODE) {
				if(g_pShuttleChild && g_pShuttleChild->m_bTurnCamera) {
					g_pShuttleChild->SetBackView(FALSE);
				}
			}
			if (nullptr == g_pShuttleChild->m_pRadarItemInfo
				&& static_cast<int>(m_pQSlotInfo[nCurrentTab][nSlotNumber].pItem->ItemNum) / 10 * 10 ==
				BGEAR_SKILL_BASENUM_AIRBOMBINGMODE)
			{
				m_fQSlotTimer = QSLOT_TIMER;
				g_pD3dApp->m_pChat->CreateChatChild(STRMSG_C_051229_0101, COLOR_SYSTEM);
				return bUseSkill;
			}

			if(IS_BAZAAR_SKILL(m_pQSlotInfo[nCurrentTab][nSlotNumber].pItem->ItemInfo)) {
				if(g_pInterface->IsBazarOpen()) {
					m_fQSlotTimer = QSLOT_TIMER;					
					return bUseSkill;
				}

				m_fClickBazaar = BAZAAR_CLICK_TIME;								
			}

			dynamic_cast<CINFGameMain*>(m_pParent)->m_pCharacterInfo->SendUseSkill(m_pQSlotInfo[nCurrentTab][nSlotNumber].pItem);
			m_fQSlotTimer = QSLOT_TIMER;

			bUseSkill = TRUE;
		}
	}	

	return bUseSkill;
}

BOOL CINFGameMainQSlot::IsBazarOpen() const
{
	if(m_fClickBazaar > 0)
	{
		return TRUE;
	}	
	if(g_pInterface->m_pBazaarShop)
	{
		return TRUE;
	}
	return FALSE;
}


void CINFGameMainQSlot::SetQSlotInfo(int nTab, int nNumber, ITEM_BASE* pItem)
{
	FLOG( "CINFGameMainQSlot::SetQSlotInfo(int nTab, int nNumber, ITEM_BASE* pItem)" );

	if(pItem) {
#ifdef _INET_RESTRICT_QSLOT_PORTALS
		if (pItem->ItemNum == 7009210 || pItem->ItemNum == 7025780 || pItem->ItemNum == 7026850 || pItem->ItemNum == 7004440) //forbid express run portals on quickslot
			return;
#endif
		if(m_bQSlotSwapFlag == TRUE) {
			strcpy_s(m_pQSlotInfo[m_nQSlotSwapTab][m_nQSlotSwapNum].Name, m_pQSlotInfo[nTab][nNumber].Name);
			strcpy_s(m_pQSlotInfo[m_nQSlotSwapTab][m_nQSlotSwapNum].IconName, m_pQSlotInfo[nTab][nNumber].IconName);		
			m_pQSlotInfo[m_nQSlotSwapTab][m_nQSlotSwapNum].pItem = m_pQSlotInfo[nTab][nNumber].pItem;

			if(m_pQSlotInfo[m_nQSlotSwapTab][m_nQSlotSwapNum].pItem) {
				g_pSOptionCharacter->ItemNum[m_nQSlotSwapTab][m_nQSlotSwapNum] 
					= m_pQSlotInfo[m_nQSlotSwapTab][m_nQSlotSwapNum].pItem->ItemNum;
				g_pSOptionCharacter->UniqueNumber[m_nQSlotSwapTab][m_nQSlotSwapNum] 
					= m_pQSlotInfo[m_nQSlotSwapTab][m_nQSlotSwapNum].pItem->UniqueNumber;
			}
			else {
				g_pSOptionCharacter->ItemNum[m_nQSlotSwapTab][m_nQSlotSwapNum] =0;
				g_pSOptionCharacter->UniqueNumber[m_nQSlotSwapTab][m_nQSlotSwapNum]  =0;
			}

			m_bQSlotSwapFlag = FALSE;
			m_nQSlotSwapTab = 0;
			m_nQSlotSwapNum = 0;
 			m_pQSlotMove = nullptr;
			m_pQSlotInfo[nTab][nNumber].pItem = nullptr;
		}

		int i;
		for(i = 0; i < QSLOT_NUMBER; i++) {
			if(m_pQSlotInfo[nTab][i].pItem && m_pQSlotInfo[nTab][i].pItem->UniqueNumber == pItem->UniqueNumber) {
				return;
			}
		}

		if( pItem->ItemInfo->InvokingDestParamID
			|| pItem->ItemInfo->InvokingDestParamIDByUse ) {
			for( i=0; i<QSLOT_NUMBER; ++i ) {
				if( m_pQSlotInfo[nTab][i].pItem && pItem->ItemNum == m_pQSlotInfo[nTab][i].pItem->ItemNum )
					return;
			}
		}

		auto* item = g_pDatabase->GetServerItemInfo(pItem->ItemNum);
		if(item)
			strcpy_s(m_pQSlotInfo[nTab][nNumber].Name, item->ItemName);

		auto* const pItemInfo = g_pStoreData->FindItemInInventoryByUniqueNumber( pItem->UniqueNumber );
		if( pItemInfo )
			SetIconName( pItemInfo, m_pQSlotInfo[nTab][nNumber].IconName );
		else
			sprintf_s( m_pQSlotInfo[nTab][nNumber].IconName, "%08d", SKILL_BASE_NUM( pItem->ItemInfo->SourceIndex ) );

		SAFE_DELETE(m_pQSlotInfo[nTab][nNumber].pItem);
		m_pQSlotInfo[nTab][nNumber].pItem = new ITEM_BASE ;
		m_pQSlotInfo[nTab][nNumber].pItem->Kind = pItem->Kind ;
		m_pQSlotInfo[nTab][nNumber].pItem->ItemNum= pItem->ItemNum;
		m_pQSlotInfo[nTab][nNumber].pItem->ItemInfo = pItem->ItemInfo;
		m_pQSlotInfo[nTab][nNumber].pItem->UniqueNumber = pItem->UniqueNumber;
 		SAFE_DELETE(m_pQSlotMove);
	
		{
			g_pSOptionCharacter->ItemNum[nTab][nNumber] = pItem->ItemNum;
			g_pSOptionCharacter->UniqueNumber[nTab][nNumber] = pItem->UniqueNumber;
		}
	}
	else {
 		SAFE_DELETE(m_pQSlotMove);
		SAFE_DELETE(m_pQSlotInfo[nTab][nNumber].pItem);
		memset(&m_pQSlotInfo[nTab][nNumber], 0x00, sizeof(INVEN_DISPLAY_INFO));

		{
			g_pSOptionCharacter->ItemNum[nTab][nNumber] = 0;
			g_pSOptionCharacter->UniqueNumber[nTab][nNumber] = 0;
		}
	}

	SetCheckQuickSlotSave(QUICKSLOT_SAVE_CHECK_TIME);
}

BOOL CINFGameMainQSlot::IsValidQSlotInfo(const int nTab, const int nNumber) const
{
	FLOG( "CINFGameMainQSlot::IsValidQSlotInfo(int nTab, int nNumber)" );
	if(m_pQSlotInfo[nTab][nNumber].pItem) {
		if(IS_GENERAL_ITEM(m_pQSlotInfo[nTab][nNumber].pItem->Kind)) {
			auto* const pItemInfo = g_pStoreData->FindItemInInventoryByUniqueNumber(
				static_cast<ITEM_GENERAL*>(m_pQSlotInfo[nTab][nNumber].pItem)->UniqueNumber);
			if( pItemInfo ) {
				return TRUE;
			}
		}
		else if(IS_SKILL_ITEM(m_pQSlotInfo[nTab][nNumber].pItem->Kind)) {
			const auto itSkillInfo = g_pShuttleChild->m_pSkill->m_mapSkill.find(
				static_cast<ITEM_SKILL*>(m_pQSlotInfo[nTab][nNumber].pItem)->
				ItemNum);
			if(itSkillInfo != g_pShuttleChild->m_pSkill->m_mapSkill.end())
			{
				return TRUE;
			}
		}
	}
	return FALSE;
}			

void CINFGameMainQSlot::UpdateQuick(const int nItemNum) 
{	
	FLOG( "CINFGameMainQSlot::UpdateQuick(int nItemNum)" );
	if(nItemNum) {
		for(auto i=0;i<QSLOT_TAB_NUMBER;i++) {
			for(auto j=0;j<QSLOT_NUMBER;j++) {			
				if(g_pSOptionCharacter->ItemNum[i][j] == nItemNum) {
					auto* const item = g_pDatabase->GetServerItemInfo(nItemNum);
					if (item && IS_GENERAL_ITEM(item->Kind)) {
						SAFE_DELETE(m_pQSlotInfo[i][j].pItem);
						m_pQSlotInfo[i][j].pItem = new ITEM_BASE;
						m_pQSlotInfo[i][j].pItem->Kind = item->Kind;
						m_pQSlotInfo[i][j].pItem->UniqueNumber = g_pSOptionCharacter->UniqueNumber[i][j];
						m_pQSlotInfo[i][j].pItem->ItemNum = g_pSOptionCharacter->ItemNum[i][j];
						m_pQSlotInfo[i][j].pItem->ItemInfo = item;
						memset(m_pQSlotInfo[i][j].IconName, 0x00, sizeof(m_pQSlotInfo[i][j].IconName));

						auto* const pItemInfo = g_pStoreData->FindItemInInventoryByUniqueNumber(m_pQSlotInfo[i][j].pItem->UniqueNumber);
						if (pItemInfo)
							SetIconName(pItemInfo, m_pQSlotInfo[i][j].IconName);
						else
							sprintf_s(m_pQSlotInfo[i][j].IconName, "%08d", SKILL_BASE_NUM(item->SourceIndex));
					}
					return;
				}
			}
		}
	}
}

void CINFGameMainQSlot::SetAllQSlotInfo()
{
	FLOG( "CINFGameMainQSlot::SetAllQSlotInfo()" );
		
	for(auto i=0;i<QSLOT_TAB_NUMBER;i++) {
		for(auto j=0;j<QSLOT_NUMBER;j++) {
			if( g_pSOptionCharacter->UniqueNumber[i][j] == 0 ) {
				continue;
			}
			auto* const pItem = g_pDatabase->GetServerItemInfo(g_pSOptionCharacter->ItemNum[i][j]);
			if( pItem == nullptr ) {
				SAFE_DELETE(m_pQSlotInfo[i][j].pItem);
				memset(&m_pQSlotInfo[i][j], 0x00, sizeof(INVEN_DISPLAY_INFO)) ;

				continue;
			}
			if(!IS_SKILL_ITEM(pItem->Kind)) {
				auto* pItemInfo = g_pStoreData->FindItemInInventoryByUniqueNumber( g_pSOptionCharacter->UniqueNumber[i][j] );
				if( pItemInfo == nullptr ) {
					if( IS_COUNTABLE_ITEM(pItem->Kind ) == FALSE ) {
						SAFE_DELETE(m_pQSlotInfo[i][j].pItem);
						memset(&m_pQSlotInfo[i][j], 0x00, sizeof(INVEN_DISPLAY_INFO)) ;

						continue;
					}
					pItemInfo = g_pStoreData->FindItemInInventoryByItemNum( g_pSOptionCharacter->ItemNum[i][j] );
					if( pItemInfo == nullptr ) {
						SAFE_DELETE(m_pQSlotInfo[i][j].pItem);
						memset(&m_pQSlotInfo[i][j], 0x00, sizeof(INVEN_DISPLAY_INFO)) ;

						continue;
					}
					g_pSOptionCharacter->UniqueNumber[i][j] = pItemInfo->UniqueNumber;
				}
			}
			SAFE_DELETE(m_pQSlotInfo[i][j].pItem);
			memset(&m_pQSlotInfo[i][j], 0x00, sizeof(INVEN_DISPLAY_INFO)) ;
			m_pQSlotInfo[i][j].pItem = new ITEM_BASE ;
			m_pQSlotInfo[i][j].pItem->Kind = pItem->Kind ;
			m_pQSlotInfo[i][j].pItem->ItemNum= pItem->ItemNum;
			m_pQSlotInfo[i][j].pItem->ItemInfo = pItem;
			m_pQSlotInfo[i][j].pItem->UniqueNumber = g_pSOptionCharacter->UniqueNumber[i][j];

			if ( IS_SKILL_ITEM(m_pQSlotInfo[i][j].pItem->Kind)  == FALSE) {
				auto* const pItemInfo = g_pStoreData->FindItemInInventoryByUniqueNumber( m_pQSlotInfo[i][j].pItem->UniqueNumber );
				if( pItemInfo )
					SetIconName( pItemInfo, m_pQSlotInfo[i][j].IconName );
			}
			else {
				sprintf_s( m_pQSlotInfo[i][j].IconName, "%08d", SKILL_BASE_NUM( pItem->SourceIndex ) );
			}
		}
	}
}

BOOL CINFGameMainQSlot::RenderDisableSkill(int nSkillNum)
{
	auto* const pItemSkill = g_pShuttleChild->m_pSkill->FindItemSkill(nSkillNum);
	
	if( pItemSkill->ItemInfo->SkillTargetType == SKILLTARGETTYPE_ME &&
		pItemSkill->ItemInfo->ReqItemKind != ITEMKIND_ALL_ITEM &&
		g_pStoreData->IsWearItem( pItemSkill->ItemInfo->ReqItemKind ) == FALSE &&
		pItemSkill->ItemInfo->SkillType != SKILLTYPE_PERMANENT) {	
		return FALSE;
	}

	return TRUE;
}
BOOL CINFGameMainQSlot::IsQSlotShowTime(ITEM *itemInfo)
{
	if(itemInfo->IsExistDesParam(DES_SKILL_HALLUCINATION))
		return TRUE;
	return FALSE;
}

void CINFGameMainQSlot::StartReattackTime(ITEM *pItem)
{
	auto* const pItemInfo = g_pStoreData->FindItemInInventoryByItemNum(pItem->ItemNum);
	if(pItemInfo) {
		pItemInfo->UseItem();
	}
}

void CINFGameMainQSlot::RenderItemUsableReAttackTime(const int nItemNum, const int nRenderIndex, bool bIsKit/*=false*/)
{
	auto* pItemInfo = g_pStoreData->FindItemInInventoryByItemNum(nItemNum);
	const auto fElapsedTime = g_pD3dApp->GetElapsedTime();
	auto nRemainedReattackTime = pItemInfo->TickReUsable(fElapsedTime);
	auto nMilis = nRemainedReattackTime;
	nRemainedReattackTime = static_cast<int>(nRemainedReattackTime / 1000);
	
	if(nRemainedReattackTime > 0 || nMilis > 0) {
		char strRemainedTime[32];
		auto bIsMinute = true;

		if(nRemainedReattackTime >= 5940) {
			nRemainedReattackTime = 99;
		}
		else if(nRemainedReattackTime >= 60) {
			nRemainedReattackTime /= 60;
		}
		else {
			bIsMinute = FALSE;
		}

		if(nRemainedReattackTime >= 0) {
			if (bIsKit){
				if(m_pImgCooldown[m_nCurrentTab][nRenderIndex])
				{
					m_pImgCooldown[m_nCurrentTab][nRenderIndex]->Move(m_nX + QSLOT_ICON_INTERVAL * nRenderIndex, m_nY);
					m_pImgCooldown[m_nCurrentTab][nRenderIndex]->Rotate(15, 15, PI / 4 * 180.0f);
					m_pImgCooldown[m_nCurrentTab][nRenderIndex]->SetRect(0, 0, 30,30* (nMilis+1000) / (float)(pItemInfo->ItemInfo->ReAttacktime+1000));
					m_pImgCooldown[m_nCurrentTab][nRenderIndex]->Render();
				}


				if (bIsMinute) {
					wsprintf(strRemainedTime, STRMSG_C_SKILL_0009, nRemainedReattackTime+1);
					m_vecFontKitCD[nRenderIndex]->DrawText(m_nX + QSLOT_ICON_INTERVAL * nRenderIndex + FONTLINE_X+28 - m_vecFontKitCD[nRenderIndex]->GetStringSize(strRemainedTime).cx,
														  m_nY + FONTLINE_Y+14,
														  D3DCOLOR_ARGB(0, 0, 255, 255),
														  strRemainedTime, 0L);
				}
				else {
					wsprintf(strRemainedTime, STRMSG_C_SKILL_0010, nRemainedReattackTime+1);//"%dĂĘ"
					m_vecFontKitCD[nRenderIndex]->DrawText(m_nX + QSLOT_ICON_INTERVAL * nRenderIndex + FONTLINE_X + 28 - m_vecFontKitCD[nRenderIndex]->GetStringSize(strRemainedTime).cx,
														  m_nY + FONTLINE_Y+14,
														  D3DCOLOR_ARGB(0, 255, 255, 255),
														  strRemainedTime, 0L);
				}
			}
			else
			{
				if(bIsMinute) {
					wsprintf(strRemainedTime, STRMSG_C_SKILL_0009, nRemainedReattackTime);
					m_vecFontLine[nRenderIndex]->DrawText(m_nX + QSLOT_ICON_INTERVAL * nRenderIndex + FONTLINE_X,
														  m_nY + FONTLINE_Y,
														  D3DCOLOR_ARGB(0,0,255,255),
														  strRemainedTime, 0L);
				}
				else {
					wsprintf(strRemainedTime, STRMSG_C_SKILL_0010, nRemainedReattackTime);//"%dĂĘ"
					m_vecFontLine[nRenderIndex]->DrawText(m_nX + QSLOT_ICON_INTERVAL * nRenderIndex + FONTLINE_X,
														  m_nY + FONTLINE_Y,
														  D3DCOLOR_ARGB(0,255,255,255),
														  strRemainedTime, 0L);
				}
			}
		}
	}	
}

void CINFGameMainQSlot::RenderSkillReAttackTime(int nItemNum, int nRenderIndex)
{
	auto itSkillInfo = g_pShuttleChild->m_pSkill->m_mapSkill.find(nItemNum);
	if(itSkillInfo != g_pShuttleChild->m_pSkill->m_mapSkill.end()) {
		if(itSkillInfo->second->ItemInfo->SkillType == SKILLTYPE_TIMELIMIT || 
			itSkillInfo->second->ItemInfo->SkillType == SKILLTYPE_CLICK ||
			itSkillInfo->second->ItemInfo->SkillType == SKILLTYPE_CHARGING)
		{
			const auto fRemainedReattackTime = itSkillInfo->second->GetCheckReattackTime();

			if(fRemainedReattackTime > 0) {
				char strRemainedTime[32];
				auto bIsMinute = true;
				auto nRemainedReattackTime = static_cast<int>(fRemainedReattackTime)/1000;
				nRemainedReattackTime++;
				
				if(nRemainedReattackTime >= 5940) {
					nRemainedReattackTime = 99;
				}
				else if(nRemainedReattackTime >= 60) {
					nRemainedReattackTime /= 60;
				}
				else {
					bIsMinute = FALSE;
				}

				if(nRemainedReattackTime >= 0) {
					if(bIsMinute) {
						wsprintf(strRemainedTime, STRMSG_C_SKILL_0009, nRemainedReattackTime);
						m_vecFontLine[nRenderIndex]->DrawText(m_nX + QSLOT_ICON_INTERVAL * nRenderIndex + FONTLINE_X,
																m_nY + FONTLINE_Y,
																D3DCOLOR_ARGB(0,0,255,255),
																strRemainedTime, 0L);
					}
					else {
						wsprintf(strRemainedTime, STRMSG_C_SKILL_0010, nRemainedReattackTime);
						m_vecFontLine[nRenderIndex]->DrawText(m_nX + QSLOT_ICON_INTERVAL * nRenderIndex + FONTLINE_X,
																m_nY + FONTLINE_Y,
																D3DCOLOR_ARGB(0,255,255,255),
																strRemainedTime, 0L);
					}
				}
			}
		}
	}
}

BOOL CINFGameMainQSlot::LButtonUpQuickSlot(const POINT pt) const
{
	if( pt.y > m_nY &&
		pt.y < m_nY + QSLOT_SIZE_Y &&
		pt.x > m_nX &&
		pt.x < m_nX + QSLOT_SIZE_X)
	{
		return TRUE;
	}

	return FALSE;
}

void CINFGameMainQSlot::SetSelectItem(INVEN_DISPLAY_INFO *pDisplayInfo)
{
	POINT ptIcon;
	ptIcon.x = m_nRenderMoveIconIntervalWidth;
	ptIcon.y = m_nRenderMoveIconIntervalHeight;
	m_pSelectItem.pItem = nullptr;

	if(pDisplayInfo) {
		m_pSelectItem.pItem = pDisplayInfo->pItem;
		strcpy(m_pSelectItem.IconName, pDisplayInfo->IconName);
		strcpy(m_pSelectItem.Name, pDisplayInfo->Name);

		static_cast<CINFGameMain*>(m_pParent)->SetSelectItem(&m_pSelectItem, ptIcon, ITEM_QSLOT_POS);
	}
	else {
		static_cast<CINFGameMain*>(m_pParent)->SetSelectItem(nullptr, ptIcon, ITEM_QSLOT_POS);
	}
}

void CINFGameMainQSlot::RefreshQSlotInfo()
{
	for(auto i = 0; i < QUICKTABCOUNT; i++) {
		for(auto j = 0; j < QUICKSLOTCOUNT; j++) {
			auto* const pItemInfo = g_pStoreData->FindItemInInventoryByItemNum(g_pSOptionCharacter->ItemNum[i][j]);
			auto* const pSkillInfo = g_pShuttleChild->m_pSkill->FindItemSkill(g_pSOptionCharacter->ItemNum[i][j]);
			if(pItemInfo) {
				g_pSOptionCharacter->UniqueNumber[i][j] = pItemInfo->UniqueNumber;
			}
			else if(pSkillInfo) {
				g_pSOptionCharacter->UniqueNumber[i][j] = pSkillInfo->UniqueNumber;
			}		 
		}
	}
	SetAllQSlotInfo();
}

void CINFGameMainQSlot::AddCurrentTab()
{
	if(m_nCurrentTab<REAL_TAB_NUMBER-1) {
		m_nCurrentTab ++;
		m_nSelectSlotNumber = -1;		

		SetSelectItem(nullptr);
	}
	else {
		m_nCurrentTab = 0;
		m_nSelectSlotNumber = -1;
		
		SetSelectItem(nullptr);
	}
	m_nButtonState[QSLOT_BUTTON_DOWN] = BUTTON_STATE_DOWN;
}

void CINFGameMainQSlot::AddJoystickQuickSlotList(int nCurrentTab, int nSlotNumber)
{
	if(g_pGameMain && g_pGameMain->IsShowOpJoystick()) {
		return;
	}

	DBGOUT("AddJoystickQuickSlotList [%d][%d]\n", nCurrentTab, nSlotNumber);

	structJoystikcSkillList		strutJoystickSkillList;
	memset(&strutJoystickSkillList, 0x00, sizeof(structJoystikcSkillList));

	strutJoystickSkillList.nCurrentTab = nCurrentTab;
	strutJoystickSkillList.nSlotNumber = nSlotNumber;

	m_vecJoystikcSkillList.push_back(strutJoystickSkillList);

	m_fQSlotTimer = QSLOT_TIMER;
	m_fJoystikcSkillList = QSLOT_TIMER+(QSLOT_TIMER/3.0f);

}
	
void CINFGameMainQSlot::TickJoystickSlot()
{
	auto* const pJoyStick = g_pD3dApp->GetJoystickControl();
	
	if(!pJoyStick && !g_pD3dApp->IsUseJoyStick()) {
		return;
	}

	if(m_vecJoystikcSkillList.empty()) {
		return;
	}

	m_fJoystikcSkillList -= g_pD3dApp->GetCheckElapsedTime(); 
	
	if(m_fJoystikcSkillList > 0.0f) {
		return;
	}
	
	m_fJoystikcSkillList = QSLOT_TIMER+(QSLOT_TIMER/3.0f);

	const auto itSkillList = m_vecJoystikcSkillList.begin();
	if(itSkillList == m_vecJoystikcSkillList.end()) {
		return;
	}

	const auto stTmp = (*itSkillList);

	if(IsPossibleJoystickSlot()) {
		UseQuickSlot(stTmp.nCurrentTab, stTmp.nSlotNumber);
		m_vecJoystikcSkillList.pop_front();	
	}

}

BOOL CINFGameMainQSlot::IsPossibleJoystickSlot() const
{
	if (!g_pInterface->m_pCityBase ||
		g_pInterface->m_pCityBase->GetCurrentBuildingNPC() != nullptr) {
		return FALSE;
	}
	if (dynamic_cast<CINFGameMain*>(m_pParent)->m_pTrade->m_bTrading) {
		return FALSE;
	}
	if(g_pD3dApp->m_bChatMode) {
		return FALSE;
	}
	if (g_pShuttleChild->CheckUnitState() != FLIGHT) {
		return FALSE;
	}

	return TRUE;
}

void	CINFGameMainQSlot::SetIconName( CItemInfo* pItemInfo, char* szName )
{
	if( IS_SKILL_ITEM( pItemInfo->ItemInfo->Kind ) )
		sprintf( szName, "%08d", SKILL_BASE_NUM( pItemInfo->ItemInfo->SourceIndex ) );
	else {
		if( pItemInfo->ItemInfo->ItemNum == pItemInfo->GetShapeItemNum() )
			sprintf( szName, "%08d", pItemInfo->ItemInfo->SourceIndex );
		else {
			auto* const pShapeItem = g_pDatabase->GetServerItemInfo( pItemInfo->GetShapeItemNum() );
			if( pShapeItem )
				sprintf( szName, "%08d", pShapeItem->SourceIndex );
			else
				sprintf( szName, "%08d", pItemInfo->ItemInfo->SourceIndex );
		}
	}
}

void	CINFGameMainQSlot::UpdateIconName( CItemInfo* pItemInfo )
{
	for (auto& i : m_pQSlotInfo) {
		for (auto& j : i) {
			if(j.pItem
				&& j.pItem->UniqueNumber == pItemInfo->UniqueNumber ) {
				SetIconName( pItemInfo, j.IconName );
				break;
			}
		}
	}
}