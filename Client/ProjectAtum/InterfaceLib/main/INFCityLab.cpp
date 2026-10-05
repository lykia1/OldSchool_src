// INFCityLab.cpp: implementation of the CINFCityLab class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "INFCityLab.h"
#include "AtumApplication.h"
#include "INFImage.h"
#include "D3DHanFont.h"
#include "GameDataLast.h"
#include "INFCityBase.h"
#include "AtumDatabase.h"
#include "INFGameMain.h"
#include "ShuttleChild.h"
#include "CharacterChild.h"
#include "INFWindow.h"
#include "INFInven.h"
#include "StoreData.h"
#include "INFIcon.h"
#include "FieldWinSocket.h"
#include "ItemInfo.h"
#include "Chat.h"
#include "INFImageRadioBtn.h"
#define RENEW_SHOP_SIZE_WIDTH	230
#define LAB_ICON_X				25
#define BACK_START_X			(CITY_BASE_NPC_BOX_START_X+RENEW_SHOP_SIZE_WIDTH + 249)
#define BACK_START_Y			(CITY_BASE_NPC_BOX_START_Y - SIZE_NORMAL_WINDOW_Y + 33)
#define SOURCE_SLOT_START_X		(BACK_START_X + 30)
#define SOURCE_SLOT_START_Y		(BACK_START_Y + 34)
#define SLOT_INTERVAL_X			31
#define SLOT_INTERVAL_Y			32
#define TARGET_SLOT_START_X		(BACK_START_X + 30)
#define TARGET_SLOT_START_Y		(BACK_START_Y + 150)
#define TARGET_FACTORY_SLOT_START_X		(BACK_START_X + 123)
#define CASH_START_X			(CITY_BASE_NPC_BOX_START_X+RENEW_SHOP_SIZE_WIDTH + 251)
#define OK_BUTTON_START_X		(CITY_BASE_NPC_BOX_START_X+RENEW_SHOP_SIZE_WIDTH + 361)
#define OK_BUTTON_START_Y		(CITY_BASE_NPC_BOX_START_Y - SIZE_NORMAL_WINDOW_Y + 240)
#define CANCEL_BUTTON_START_X	(CITY_BASE_NPC_BOX_START_X+RENEW_SHOP_SIZE_WIDTH + 398)
#define CANCEL_BUTTON_START_Y	(CITY_BASE_NPC_BOX_START_Y - SIZE_NORMAL_WINDOW_Y + 240)
#define BUTTON_SIZE_X			35
#define BUTTON_SIZE_Y			16
#define SOURCE_NUMBER_X			4
#define SOURCE_NUMBER_Y			2

#ifdef _INET_ENCHANT_CHANCE
	#define _INET_CHANCE_X	SPI_START_X+75
	#define _INET_CHANCE_Y	SPI_START_Y-85
#endif

#define CASH_START_Y			(CITY_BASE_NPC_BOX_START_Y - SIZE_NORMAL_WINDOW_Y + 239)
#define SPI_START_X				(CASH_START_X + 5)
#define SPI_START_Y				(CASH_START_Y + 1)

#define		MIX_NUM_EDIT_X		(BACK_START_X+31)
#define		MIX_NUM_EDIT_Y		(BACK_START_Y+162)
#define		MIX_NUM_EDIT_W		(70)
#define		MIX_NUM_EDIT_H		(20)

#define		RARE_FIX_PREFIX							1
#define		RARE_FIX_SUFFIX							2

#define		RARE_FIX_ITEM							1
#define		ITIALIZE_ITEM							2

#define		MACRO_SOURCE_SLOT1_X					BACK_START_X + 234
#define		MACRO_SOURCE_SLOT1_Y					BACK_START_Y + 63

#define		MACRO_DESC_START_X						MACRO_SOURCE_SLOT1_X - 41
#define		MACRO_DESC_START_Y						MACRO_SOURCE_SLOT1_Y + 44

CINFCityLab::CINFCityLab(CAtumNode* pParent, BUILDINGNPC* pBuilding)
{
	m_pParent						= pParent;
	m_pBuildingInfo					= pBuilding;
	m_bRestored						= FALSE;
	m_pImgBack						= NULL;
	m_pImgBackFactory				= NULL;
	m_pImgTitle						= NULL;
	m_pImgPrice						= NULL;
	m_pFontPrice					= NULL;
#ifdef _INET_ENCHANT_CHANCE
	m_pFontChance = nullptr;
#endif
	m_nButtonState[0]				= BUTTON_STATE_NORMAL;
	m_nButtonState[1]				= BUTTON_STATE_NORMAL;
	m_pSelectItem					= NULL;
	m_bShowTarget					= FALSE;

	memset( m_pImgButton, 0x00, sizeof(DWORD)*LAB_BUTTON_NUMBER*4);
	memset( m_szPrice, 0x00, 64);
#ifdef _INET_ENCHANT_CHANCE
	memset(m_szChance, 0x00, 64);
#endif
	m_bSelectDown = FALSE;

	// 2008-03-14 by bhsohn Á¶ÇŐ˝Ä °łĽ±ľČ
	m_pNumEditBox= nullptr;

	// 2010. 06. 18 by jskim ŔÎĂ¦Ć® °ć°í ¸Ţ˝ĂÁö Ăß°ˇ
	m_bIsEnchantCheck = FALSE;
	//end 2010. 06. 18 by jskim ŔÎĂ¦Ć® °ć°í ¸Ţ˝ĂÁö Ăß°ˇ
#ifdef _INET_ANTI_MACRO
	vecLastClickTime.clear();
	nTicksSinceLastClick = 0;
#endif
	
	memset(m_szMacroTxt, 0x00, 512);
	m_pFontMacro = nullptr;
	m_vecTarget.clear();
	m_vecSourceMacro.clear();
	b_IsPrefixMacro = false;
	bEnchantMacro = true;
	bEnchantFinished = true;
	for(auto i{ MACRO_SLOW_NONE }; i < MACRO_RADIO_SLOW_OPTIONS_COUNT ; i++)
	{
		m_pMacroRadioBtn[i] = nullptr;
		
		if(i == MACRO_SLOW_NONE)
			m_bMacroRadioSel[i] = true;
		else
			m_bMacroRadioSel[i] = false;
	}
	m_nTicksAfterLastUsage = 0;

	for(auto i{ MACRO_RADIO_ENCHANT_CAP_NO }; i < MACRO_RADIO_ENCHANT_CAP_COUNT ; i++)
	{
		m_pMacroEnchCapBtn[i] = nullptr;
		
		if(i == MACRO_SLOW_NONE)
			m_bMacroEnchCapSel[i] = true;
		else
			m_bMacroEnchCapSel[i] = false;
	}
	nClickCount = 0;

	fE_CurrentPercent = 0.0f;
	fE_IncreaseValue = 0.0f;
	lastPosClicked.x = 0;
	lastPosClicked.y = 0;
}

CINFCityLab::~CINFCityLab()
{
	SAFE_DELETE(m_pImgBack);
	// 2008-03-14 by bhsohn Á¶ÇŐ˝Ä °łĽ±ľČ
	SAFE_DELETE(m_pImgBackFactory);	
	SAFE_DELETE(m_pImgTitle);
	SAFE_DELETE(m_pImgPrice);
	SAFE_DELETE(m_pFontPrice);
#ifdef _INET_ENCHANT_CHANCE
	SAFE_DELETE(m_pFontChance);
#endif
	// 2008-03-14 by bhsohn Á¶ÇŐ˝Ä °łĽ±ľČ
	SAFE_DELETE(m_pNumEditBox);	
	
	for(auto i=0;i<4;i++)
	{
		SAFE_DELETE(m_pImgButton[0][i]);
		SAFE_DELETE(m_pImgButton[1][i]);		
		SAFE_DELETE(m_pImgButton[2][i]);		
	}

	SAFE_DELETE(m_pFontMacro);
	for (auto i{ MACRO_SLOW_NONE }; i < MACRO_RADIO_SLOW_OPTIONS_COUNT; i++)
	{
		SAFE_DELETE(m_pMacroRadioBtn[i]);

		if (i == MACRO_SLOW_NONE)
			m_bMacroRadioSel[i] = true;
		else
			m_bMacroRadioSel[i] = false;
	}

	for (auto i{ MACRO_RADIO_ENCHANT_CAP_NO }; i < MACRO_RADIO_ENCHANT_CAP_COUNT; i++)
	{
		SAFE_DELETE(m_pMacroEnchCapBtn[i]);

		if (i == MACRO_SLOW_NONE)
			m_bMacroEnchCapSel[i] = true;
		else
			m_bMacroEnchCapSel[i] = false;
	}
	
	m_nTicksAfterLastUsage = 0;
	InitDataMacro();
	InitData();
	nClickCount = 0;
}

HRESULT CINFCityLab::InitDeviceObjects()
{
	FLOG("CINFCityLab::InitDeviceObjects()");
	DataHeader* pDataHeader;

	m_pImgBack = new CINFImage;
	pDataHeader = FindResource("shlabbk");
	m_pImgBack->InitDeviceObjects(pDataHeader->m_pData, pDataHeader->m_DataSize);

	// 2008-03-14 by bhsohn Á¶ÇŐ˝Ä °łĽ±ľČ
	m_pImgBackFactory = new CINFImage;
	pDataHeader = FindResource("shlabbk1");
	m_pImgBackFactory->InitDeviceObjects(pDataHeader->m_pData, pDataHeader->m_DataSize);


	m_pImgTitle = new CINFImage;
	pDataHeader = FindResource("lab-titl");
	m_pImgTitle->InitDeviceObjects(pDataHeader->m_pData, pDataHeader->m_DataSize);

	m_pImgPrice = new CINFImage;
	pDataHeader = FindResource("shlacost");
	m_pImgPrice->InitDeviceObjects(pDataHeader->m_pData, pDataHeader->m_DataSize);

	for (auto i = 0; i < 4; i++)
	{
		char buf[16];

		m_pImgButton[0][i] = new CINFImage;
		wsprintf(buf, "shlama0%d", i);
		pDataHeader = FindResource(buf);
		m_pImgButton[0][i]->InitDeviceObjects(pDataHeader->m_pData, pDataHeader->m_DataSize);

		m_pImgButton[1][i] = new CINFImage;
		wsprintf(buf, "shmcan0%d", i);
		pDataHeader = FindResource(buf);
		m_pImgButton[1][i]->InitDeviceObjects(pDataHeader->m_pData, pDataHeader->m_DataSize);

		m_pImgButton[2][i] = new CINFImage;
		wsprintf(buf, "shlaok0%d", i);
		pDataHeader = FindResource(buf);
		m_pImgButton[2][i]->InitDeviceObjects(pDataHeader->m_pData, pDataHeader->m_DataSize);
	}

	m_pFontPrice = new CD3DHanFont(_T(g_pD3dApp->GetFontStyle()), 9, D3DFONT_ZENABLE, TRUE, 128, 32);;
	m_pFontPrice->InitDeviceObjects(g_pD3dDev);
#ifdef _INET_ENCHANT_CHANCE
	m_pFontChance = new CD3DHanFont(_T(g_pD3dApp->GetFontStyle()), 9, D3DFONT_ZENABLE, TRUE, 128, 32);
	m_pFontChance->InitDeviceObjects(g_pD3dDev);
#endif
	// 2008-03-14 by bhsohn Á¶ÇŐ˝Ä °łĽ±ľČ
	{
		if (NULL == m_pNumEditBox)
		{
			m_pNumEditBox = new CINFNumEditBox;
		}
		char chBuff[32];
		char chMaxMixCnt[64];

		wsprintf(chBuff, "1");
		wsprintf(chMaxMixCnt, "%d", COUNT_MAX_MIXING_COUNT);

		POINT ptPos = { (LONG)MIX_NUM_EDIT_X, (LONG)MIX_NUM_EDIT_Y };
		m_pNumEditBox->InitDeviceObjects(9, ptPos, MIX_NUM_EDIT_W, TRUE, MIX_NUM_EDIT_H);
		m_pNumEditBox->SetMaxStringLen(strlen(chMaxMixCnt));
		m_pNumEditBox->SetString(chBuff, 32);
	}
	// end 2008-03-14 by bhsohn Á¶ÇŐ˝Ä °łĽ±ľČ

	m_pFontMacro = new CD3DHanFont(_T(g_pD3dApp->GetFontStyle()), 9, D3DFONT_ZENABLE, TRUE, 256, 32);
	m_pFontMacro->InitDeviceObjects(g_pD3dDev);

	char chRadioOff[30], chRadioOn[30];

	wsprintf(chRadioOff, "radio_b");
	wsprintf(chRadioOn, "radio_a");

	for (auto nMainId = MACRO_SLOW_NONE; nMainId < MACRO_RADIO_SLOW_OPTIONS_COUNT; nMainId++)
	{
		if (nullptr == m_pMacroRadioBtn[nMainId])
			m_pMacroRadioBtn[nMainId] = new CINFImageRadioBtn;

		m_pMacroRadioBtn[nMainId]->InitDeviceObjects(chRadioOff, chRadioOn);

		if(nMainId == MACRO_SLOW_NONE)
			m_pMacroRadioBtn[nMainId]->SetRadioBtn(TRUE);
		else
			m_pMacroRadioBtn[nMainId]->SetRadioBtn(FALSE);
	}
	for (auto nMainId1 = MACRO_RADIO_ENCHANT_CAP_NO; nMainId1 < MACRO_RADIO_ENCHANT_CAP_COUNT; nMainId1++)
	{
		if (nullptr == m_pMacroEnchCapBtn[nMainId1])
			m_pMacroEnchCapBtn[nMainId1] = new CINFImageRadioBtn;

		m_pMacroEnchCapBtn[nMainId1]->InitDeviceObjects(chRadioOff, chRadioOn);

		if(nMainId1 == MACRO_RADIO_ENCHANT_CAP_NO)
			m_pMacroEnchCapBtn[nMainId1]->SetRadioBtn(TRUE);
		else
			m_pMacroEnchCapBtn[nMainId1]->SetRadioBtn(FALSE);
	}
	return S_OK;
}


HRESULT CINFCityLab::RestoreDeviceObjects()
{
	if(!m_bRestored)
	{
		for(auto i=0;i<4;i++)
		{
			m_pImgButton[0][i]->RestoreDeviceObjects();
			m_pImgButton[0][i]->Move(OK_BUTTON_START_X, OK_BUTTON_START_Y);
			m_pImgButton[1][i]->RestoreDeviceObjects();
			m_pImgButton[1][i]->Move(CANCEL_BUTTON_START_X, CANCEL_BUTTON_START_Y);
			m_pImgButton[2][i]->RestoreDeviceObjects();
			m_pImgButton[2][i]->Move(OK_BUTTON_START_X, OK_BUTTON_START_Y);
		}

		m_pImgBack->RestoreDeviceObjects();
		m_pImgBack->Move(BACK_START_X, BACK_START_Y);
		// 2008-03-14 by bhsohn Á¶ÇŐ˝Ä °łĽ±ľČ
		m_pImgBackFactory->RestoreDeviceObjects();
		m_pImgBackFactory->Move(BACK_START_X, BACK_START_Y);		
		// end 2008-03-14 by bhsohn Á¶ÇŐ˝Ä °łĽ±ľČ
		m_pImgTitle->RestoreDeviceObjects();
		m_pImgTitle->Move(CITY_BASE_NPC_BOX_START_X, CITY_BASE_NPC_BOX_START_Y);
		m_pImgPrice->RestoreDeviceObjects();
		m_pImgPrice->Move(CASH_START_X, CASH_START_Y);
		m_pFontPrice->RestoreDeviceObjects();
#ifdef _INET_ENCHANT_CHANCE
		m_pFontChance->RestoreDeviceObjects();
#endif
		// 2008-03-14 by bhsohn Á¶ÇŐ˝Ä °łĽ±ľČ		
		m_pNumEditBox->RestoreDeviceObjects();		
		// 2008-03-14 by bhsohn Á¶ÇŐ˝Ä °łĽ±ľČ

		m_pFontMacro->RestoreDeviceObjects();
		
		for (auto nMainId = MACRO_SLOW_NONE; nMainId < MACRO_RADIO_SLOW_OPTIONS_COUNT; nMainId++)
			m_pMacroRadioBtn[nMainId]->RestoreDeviceObjects();

		for (auto nMainId = MACRO_RADIO_ENCHANT_CAP_NO; nMainId < MACRO_RADIO_ENCHANT_CAP_COUNT; nMainId++)
			m_pMacroEnchCapBtn[nMainId]->RestoreDeviceObjects();
		
		m_bRestored = TRUE;
	}
	return S_OK;
}

HRESULT CINFCityLab::DeleteDeviceObjects()
{
	for(auto i=0;i<4;i++)
	{
		m_pImgButton[0][i]->DeleteDeviceObjects();
		m_pImgButton[1][i]->DeleteDeviceObjects();
		m_pImgButton[2][i]->DeleteDeviceObjects();
		SAFE_DELETE(m_pImgButton[0][i]);
		SAFE_DELETE(m_pImgButton[1][i]);
		SAFE_DELETE(m_pImgButton[2][i]);
	}
	
	m_pImgBack->DeleteDeviceObjects();
	// 2008-03-14 by bhsohn Á¶ÇŐ˝Ä °łĽ±ľČ
	m_pImgBackFactory->DeleteDeviceObjects();

	m_pImgTitle->DeleteDeviceObjects();
	m_pImgPrice->DeleteDeviceObjects();
	m_pFontPrice->DeleteDeviceObjects();
#ifdef _INET_ENCHANT_CHANCE
	m_pFontChance->DeleteDeviceObjects();
#endif
	SAFE_DELETE(m_pImgBack);
	// 2008-03-14 by bhsohn Á¶ÇŐ˝Ä °łĽ±ľČ
	SAFE_DELETE(m_pImgBackFactory);
	
	SAFE_DELETE(m_pImgTitle);
	SAFE_DELETE(m_pImgPrice);
	SAFE_DELETE(m_pFontPrice);
#ifdef _INET_ENCHANT_CHANCE
	SAFE_DELETE(m_pFontChance);
#endif
	// 2008-03-14 by bhsohn Á¶ÇŐ˝Ä °łĽ±ľČ	
	if(m_pNumEditBox)
	{
		m_pNumEditBox->DeleteDeviceObjects();	
		SAFE_DELETE(m_pNumEditBox);
	}		
	// end 2008-03-14 by bhsohn Á¶ÇŐ˝Ä °łĽ±ľČ
	SAFE_DELETE(m_pFontMacro);

	for (auto nMainId = MACRO_SLOW_NONE; nMainId < MACRO_RADIO_SLOW_OPTIONS_COUNT; nMainId++) {
		m_pMacroRadioBtn[nMainId]->DeleteDeviceObjects();
		SAFE_DELETE(m_pMacroRadioBtn[nMainId]);
	}

	for (auto nMainId = MACRO_RADIO_ENCHANT_CAP_NO; nMainId < MACRO_RADIO_ENCHANT_CAP_COUNT; nMainId++) {
		m_pMacroEnchCapBtn[nMainId]->DeleteDeviceObjects();
		SAFE_DELETE(m_pMacroEnchCapBtn[nMainId]);
	}
	return S_OK;
}

HRESULT CINFCityLab::InvalidateDeviceObjects()
{
	if(m_bRestored)
	{
		for(auto i=0;i<4;i++)
		{
			m_pImgButton[0][i]->InvalidateDeviceObjects();
			m_pImgButton[1][i]->InvalidateDeviceObjects();
			m_pImgButton[2][i]->InvalidateDeviceObjects();
		}

		m_pImgBack->InvalidateDeviceObjects();
		// 2008-03-14 by bhsohn Á¶ÇŐ˝Ä °łĽ±ľČ
		m_pImgBackFactory->InvalidateDeviceObjects();
		m_pImgTitle->InvalidateDeviceObjects();
		m_pImgPrice->InvalidateDeviceObjects();
		m_pFontPrice->InvalidateDeviceObjects();
#ifdef _INET_ENCHANT_CHANCE
		m_pFontChance->InvalidateDeviceObjects();
#endif
		// 2008-03-14 by bhsohn Á¶ÇŐ˝Ä °łĽ±ľČ	
		m_pNumEditBox->InvalidateDeviceObjects();

		m_pFontMacro->InvalidateDeviceObjects();
		for (auto nMainId = MACRO_SLOW_NONE; nMainId < MACRO_RADIO_SLOW_OPTIONS_COUNT; nMainId++) {
			m_pMacroRadioBtn[nMainId]->InvalidateDeviceObjects();
		}

		for (auto nMainId = MACRO_RADIO_ENCHANT_CAP_NO; nMainId < MACRO_RADIO_ENCHANT_CAP_COUNT; nMainId++) {
			m_pMacroEnchCapBtn[nMainId]->InvalidateDeviceObjects();
		}
		m_bRestored = FALSE;
	}
	return S_OK;
}

void CINFCityLab::Render()
{
	if(m_pBuildingInfo->BuildingKind == BUILDINGKIND_FACTORY)
	{
		m_pImgBackFactory->Render();				
	}
	else
	{
		for (auto nMainId = MACRO_SLOW_NONE; nMainId < MACRO_RADIO_SLOW_OPTIONS_COUNT; nMainId++) {
			if(nMainId == MACRO_SLOW_NONE)
				sprintf_s(m_szMacroTxt, "Full speed");
			else if(nMainId == MACRO_SLOW_HALF)
				sprintf_s(m_szMacroTxt, "1.0s");
			else if(nMainId == MACRO_SLOW_SECOND)
				sprintf_s(m_szMacroTxt, "2.0s");
			auto nMove = nMainId == MACRO_SLOW_HALF ? 75 : 60;
			m_pFontMacro->DrawText(MACRO_DESC_START_X+15 + nMove * nMainId, MACRO_SOURCE_SLOT1_Y - 52, GUI_FONT_COLOR_YM, m_szMacroTxt, 0L);
			
			m_pMacroRadioBtn[nMainId]->Render();
			m_pMacroRadioBtn[nMainId]->SetPosition(MACRO_DESC_START_X + nMove * nMainId, MACRO_SOURCE_SLOT1_Y - 52, 45);
		}

		for (auto nMainId = MACRO_RADIO_ENCHANT_CAP_NO; nMainId < MACRO_RADIO_ENCHANT_CAP_COUNT; nMainId++) {
			if(nMainId == MACRO_RADIO_ENCHANT_CAP_NO)
				sprintf_s(m_szMacroTxt, "Non limited");
			else if(nMainId == MACRO_RADIO_ENCHANT_CAP_10)
				sprintf_s(m_szMacroTxt, "E 10");
			else if(nMainId == MACRO_RADIO_ENCHANT_CAP_11)
				sprintf_s(m_szMacroTxt, "E 11");
			else if(nMainId == MACRO_RADIO_ENCHANT_CAP_12)
				sprintf_s(m_szMacroTxt, "E 12");
			
			auto nMove = nMainId == MACRO_RADIO_ENCHANT_CAP_10 ? 75 : 60;
			m_pFontMacro->DrawText(MACRO_DESC_START_X+15 + nMove * nMainId, MACRO_DESC_START_Y +70, GUI_FONT_COLOR_YM, m_szMacroTxt, 0L);
			
			m_pMacroEnchCapBtn[nMainId]->Render();
			m_pMacroEnchCapBtn[nMainId]->SetPosition(MACRO_DESC_START_X + nMove * nMainId, MACRO_DESC_START_Y +70, 45);
		}
		
		sprintf_s(m_szMacroTxt, "Click refine to run macro");
		m_pFontMacro->DrawText(MACRO_DESC_START_X, MACRO_DESC_START_Y+15, GUI_FONT_COLOR_YM, m_szMacroTxt, 0L);

		m_pImgBack->Render();
	}	
	m_pImgPrice->Render();

	if(m_pBuildingInfo->BuildingKind == BUILDINGKIND_FACTORY)
	{
		m_pNumEditBox->Render();
	}

	auto* pIcon = ((CINFGameMain*)m_pParent)->m_pIcon;
	auto it = m_vecSource.begin();
	auto i = 0;
	while(it != m_vecSource.end())
	{
		auto* pItem = *it;
		char buf[20];
		long itemNum = 0;
		if( !pItem->ShapeItemNum )
			itemNum = pItem->ItemInfo->SourceIndex;
		else
		{
			auto* pShapeItem = g_pDatabase->GetServerItemInfo( pItem->ShapeItemNum );
			if( pShapeItem )
				itemNum = pShapeItem->SourceIndex;
			else
				itemNum = pItem->ItemInfo->SourceIndex;
		}

		pIcon->Render(itemNum, SOURCE_SLOT_START_X + i % SOURCE_NUMBER_X * SLOT_INTERVAL_X + 1,
			SOURCE_SLOT_START_Y + i / SOURCE_NUMBER_X * SLOT_INTERVAL_Y + 1);

		if(IS_COUNTABLE_ITEM(pItem->Kind))
		{
			wsprintf( buf, "%d", pItem->CurrentCount);
			auto size = m_pFontPrice->GetStringSize(buf);
			m_pFontPrice->DrawText(SOURCE_SLOT_START_X+i%SOURCE_NUMBER_X*SLOT_INTERVAL_X+1+LAB_ICON_X-size.cx, 
				SOURCE_SLOT_START_Y+i/SOURCE_NUMBER_X*SLOT_INTERVAL_Y+1,GUI_FONT_COLOR_W,buf,0L);
		}else
				{//leg count
					if (COMPARE_BIT_FLAG(pItem->ItemInfo->ItemAttribute, ITEM_ATTR_LEGEND_ITEM) && pItem->GetEnchantNumber() > 0)
					{
						int nSouls = (pItem->ItemInfo->ReqMinLevel - 30)/5;
						char buf[128];
						wsprintf(buf, "%d",nSouls);
						auto size = m_pFontPrice->GetStringSize(buf);
		
						int nFontPosX = SOURCE_SLOT_START_X+i%SOURCE_NUMBER_X*SLOT_INTERVAL_X+1 +LAB_ICON_X - size.cx;
						int nFontPosY = SOURCE_SLOT_START_Y+i/SOURCE_NUMBER_X*SLOT_INTERVAL_Y-1;
						
						m_pFontPrice->DrawText(nFontPosX,nFontPosY+15, RGB(255,100,255),buf, 0L);
					}
					if(pItem->GetEnchantNumber() > 0)
					{
						char buf[128];
						wsprintf(buf, "%d",pItem->GetEnchantNumber());
						int len = strlen(buf) - 1;
		
						int nFontPosX = SOURCE_SLOT_START_X+i%SOURCE_NUMBER_X*SLOT_INTERVAL_X+1 ;
						int nFontPosY = SOURCE_SLOT_START_Y+i/SOURCE_NUMBER_X*SLOT_INTERVAL_Y-1;
						
						m_pFontPrice->DrawText(nFontPosX,nFontPosY, RGB(255,187,61),buf, 0L);
					}
				}
		++it;
		i++;
	}

	auto it1 = m_vecSourceMacro.begin();
	i = 0;
	while (it1 != m_vecSourceMacro.end())
	{
		auto* pItem = *it1;
		
		if (pItem == nullptr || pItem->ItemInfo == nullptr)
			break;
		
		if(i==1)
		{
			if(pItem->ItemInfo->IsExistDesParam(DES_RARE_FIX_SUFFIX))
				b_IsPrefixMacro = false;
			else if (pItem->ItemInfo->IsExistDesParam(DES_RARE_FIX_PREFIX))
				b_IsPrefixMacro = true;
			else if(pItem->Kind == ITEMKIND_ENCHANT)
				bEnchantMacro = true;
		}
		if (i == 0) {
			if (b_IsPrefixMacro) {
				if (pItem->GetPrefixRareInfo() == NULL)
					sprintf(m_szMacroTxt, "- - -");
				else
					sprintf(m_szMacroTxt, "%s", m_vecSourceMacro[0]->GetPrefixRareInfo()->Name);
			} else if (!bEnchantMacro){
				if (pItem->GetSuffixRareInfo() == NULL)
					sprintf(m_szMacroTxt, "- - -");
				else
					sprintf(m_szMacroTxt, "%s", pItem->GetSuffixRareInfo()->Name);
			}

			if(bEnchantMacro)
				sprintf(m_szMacroTxt, "%d", pItem->GetEnchantNumber());
			
			m_pFontMacro->DrawText(MACRO_DESC_START_X + 65, MACRO_DESC_START_Y + 36, RGB(0, 255, 0), m_szMacroTxt, 0L);
		}

		long nItemNum = 0;
		char buf[20];
		if (!pItem->ShapeItemNum)
			nItemNum = pItem->ItemInfo->SourceIndex;
		else
		{
			auto* pShapeItem = g_pDatabase->GetServerItemInfo(pItem->ShapeItemNum);
			if (pShapeItem)
				nItemNum = pShapeItem->SourceIndex;
			else
				nItemNum = pItem->ItemInfo->SourceIndex;
		}

		pIcon->Render(nItemNum, MACRO_SOURCE_SLOT1_X + i % 3 * SLOT_INTERVAL_X + 1,
			MACRO_SOURCE_SLOT1_Y + i / 3 * SLOT_INTERVAL_Y + 1);
		
		if (IS_COUNTABLE_ITEM(pItem->Kind))
		{
						if(pItem->CurrentCount >= 1000000000)
							sprintf(buf,"%.0fB",(float)(pItem->CurrentCount/1000000000.0f));
						else if(pItem->CurrentCount >= 100000000)
							sprintf(buf,"%.0fM",(float)(pItem->CurrentCount/1000000.0f));
						else if(pItem->CurrentCount >= 1000000)
							sprintf(buf,"%.0fM",(float)(pItem->CurrentCount/1000000.0f));
						else if(pItem->CurrentCount >= 100000)
							sprintf(buf,"%.0fk",(float)(pItem->CurrentCount/1000.0f));
						else if(pItem->CurrentCount >= 10000)
							sprintf(buf,"%.0fk",(float)(pItem->CurrentCount/1000.0f));
						else if(pItem->CurrentCount >= 1000)
							sprintf(buf,"%.0fk",(float)(pItem->CurrentCount/1000.0f));
						else
							sprintf(buf, "%d",pItem->CurrentCount);
			
			//wsprintf(buf, "%d", pItem->CurrentCount);
			auto size = m_pFontPrice->GetStringSize(buf);
			m_pFontPrice->DrawText(MACRO_SOURCE_SLOT1_X + i % 3 * SLOT_INTERVAL_X + 1 + LAB_ICON_X - size.cx,
				MACRO_SOURCE_SLOT1_Y + i / 3 * SLOT_INTERVAL_Y + 1, RGB(170,255,200), buf, 0L);
		}else
				{//leg count
					if (COMPARE_BIT_FLAG(pItem->ItemInfo->ItemAttribute, ITEM_ATTR_LEGEND_ITEM) && pItem->GetEnchantNumber() > 0)
					{
						int nSouls = (pItem->ItemInfo->ReqMinLevel - 30)/5;
						char buf[128];
						wsprintf(buf, "%d",nSouls);
						auto size = m_pFontPrice->GetStringSize(buf);
		
						int nFontPosX = MACRO_SOURCE_SLOT1_X + i % 3 * SLOT_INTERVAL_X + 1 + LAB_ICON_X - size.cx;
						int nFontPosY = MACRO_SOURCE_SLOT1_Y + i / 3 * SLOT_INTERVAL_Y;
						
						m_pFontPrice->DrawText(nFontPosX,nFontPosY+15, RGB(255,100,255),buf, 0L);
					}
					if(pItem->GetEnchantNumber() > 0)
					{
						char buf[128];
						wsprintf(buf, "%d",pItem->GetEnchantNumber());
						int len = strlen(buf) - 1;
		
						int nFontPosX = MACRO_SOURCE_SLOT1_X + i % 3 * SLOT_INTERVAL_X + 1;
						int nFontPosY = MACRO_SOURCE_SLOT1_Y + i / 3 * SLOT_INTERVAL_Y;
						
						m_pFontPrice->DrawText(nFontPosX,nFontPosY, RGB(255,187,61),buf, 0L);
					}
				}

		++it1;
		i++;
	}
	
	if( m_bShowTarget )
	{
		it = m_vecTarget.begin();		
		i = 0;
		
		int nSlotStartX = TARGET_SLOT_START_X;
		if(m_pBuildingInfo->BuildingKind == BUILDINGKIND_FACTORY)
		{
			nSlotStartX = TARGET_FACTORY_SLOT_START_X;
		}

		if(it != m_vecTarget.end())
		{
			auto* pItem = *it;
			char buf[20];
			long nItemNum = 0;
			if( !pItem->ShapeItemNum )
				nItemNum = pItem->ItemInfo->SourceIndex ;
			else
			{
				auto* pShapeItem = g_pDatabase->GetServerItemInfo( pItem->ShapeItemNum );
				if( pShapeItem )
					nItemNum = pShapeItem->SourceIndex ;
				else
					nItemNum = pItem->ItemInfo->SourceIndex ;
			}

			pIcon->Render(nItemNum, nSlotStartX+i*SLOT_INTERVAL_X+1, TARGET_SLOT_START_Y+1);

			if(IS_COUNTABLE_ITEM(pItem->Kind))
			{
				wsprintf( buf, "%d", pItem->CurrentCount);
				auto size = m_pFontPrice->GetStringSize(buf);

				m_pFontPrice->DrawText(nSlotStartX+i*SLOT_INTERVAL_X+1+LAB_ICON_X-size.cx, 				
					TARGET_SLOT_START_Y+1,GUI_FONT_COLOR_W,buf,0L);
			}
		}
		// OK
		m_pImgButton[2][m_nButtonState[0]]->Render();
	}
	else
	{
		// SEND
		m_pImgButton[0][m_nButtonState[0]]->Render();
	}
	// CANCEL
	m_pImgButton[1][m_nButtonState[1]]->Render();
	if(m_szPrice[0] != NULL)
	{
		m_pFontPrice->DrawText(SPI_START_X, SPI_START_Y, GUI_FONT_COLOR, m_szPrice, 0L );
	}
#ifdef _INET_ENCHANT_CHANCE
	if (m_szChance[0] != NULL)
	{
		if (fE_CurrentPercent >= 100)
		{
			m_pFontChance->DrawText(_INET_CHANCE_X, _INET_CHANCE_Y, RGB(38, 255, 0), m_szChance, 0L);
		}
		else if (fE_CurrentPercent < 100 && fE_CurrentPercent >= 75)
		{
			m_pFontChance->DrawText(_INET_CHANCE_X, _INET_CHANCE_Y, GUI_FONT_COLOR_Y, m_szChance, 0L);
		}
		else if (fE_CurrentPercent < 75 && fE_CurrentPercent >= 50)
		{
			m_pFontChance->DrawText(_INET_CHANCE_X, _INET_CHANCE_Y, GUI_FONT_COLOR_YM, m_szChance, 0L);
		}
		else if (fE_CurrentPercent < 50)
		{
			m_pFontChance->DrawText(_INET_CHANCE_X, _INET_CHANCE_Y, GUI_FONT_COLOR_R, m_szChance, 0L);
		}
		else
		{
			m_pFontChance->DrawText(_INET_CHANCE_X, _INET_CHANCE_Y, GUI_FONT_COLOR, m_szChance, 0L);
		}
	}
#endif
}

void CINFCityLab::Tick()
{
}

int CINFCityLab::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_MOUSEMOVE:
		{		
			POINT pt;
			pt.x = LOWORD(lParam);
			pt.y = HIWORD(lParam);
			CheckMouseReverse(&pt);
			g_pGameMain->SetItemInfo( 0, 0, 0, 0 );
			if (pt.x > MACRO_SOURCE_SLOT1_X && pt.x < MACRO_SOURCE_SLOT1_X + SLOT_INTERVAL_X * 3)
			{
				if (pt.y > MACRO_SOURCE_SLOT1_Y &&
					pt.y < MACRO_SOURCE_SLOT1_Y + SLOT_INTERVAL_Y)
				{
					if (m_vecSourceMacro.empty()) {
						g_pGameMain->SetItemInfo(0, 0, 0, 0);

						char chTmp[256];
						sprintf(chTmp, "Drag and drop item, gamble and wipe here");
						g_pGameMain->SetToolTip(pt.x, pt.y, chTmp);
						return INF_MSGPROC_BREAK;
					}

					int x = (pt.x - MACRO_SOURCE_SLOT1_X) / SLOT_INTERVAL_X;			
					auto index = x-15;
	
					if (index >= 0 && m_vecSourceMacro.size() > index)
					{
						auto* pItemInfo = m_vecSourceMacro[index];
						if (pItemInfo)
						{
							g_pGameMain->SetItemInfoUser(pItemInfo, pt.x, pt.y);
						}
					}


				}
			}
			else if (pt.x > MACRO_DESC_START_X && pt.x < MACRO_DESC_START_X + 210)
			{
				if (pt.y > MACRO_SOURCE_SLOT1_Y - 60 &&
					pt.y < MACRO_SOURCE_SLOT1_Y - 37)
				{
					char chTmp[256];
					sprintf_s(chTmp, "\\gTIP:\\yYou can select delay between click ability during macro usage");
					g_pGameMain->SetToolTip(pt.x, pt.y-10, chTmp);
	
					return INF_MSGPROC_BREAK;
				}
			}
			else
			{
				if (pt.x > SOURCE_SLOT_START_X &&
					pt.x < SOURCE_SLOT_START_X + SLOT_INTERVAL_X * 4)
				{
					if (pt.y > SOURCE_SLOT_START_Y &&
						pt.y < SOURCE_SLOT_START_Y + SLOT_INTERVAL_Y * 2)
					{
						int x = (pt.x - SOURCE_SLOT_START_X) / SLOT_INTERVAL_X;
						int y = (pt.y - SOURCE_SLOT_START_Y) / SLOT_INTERVAL_Y;
						auto index = x + y * SOURCE_NUMBER_X;
						if (index >= 0 && m_vecSource.size() > index)
						{
							auto* pItemInfo = m_vecSource[index];
							if (pItemInfo)
							{
								g_pGameMain->SetItemInfoUser(pItemInfo, pt.x, pt.y);
							}
						}
					}
					else if (m_bShowTarget &&
						pt.y > TARGET_SLOT_START_Y &&
						pt.y < TARGET_SLOT_START_Y + SLOT_INTERVAL_Y)
					{
						int nSlotStartX = TARGET_SLOT_START_X;
						if (m_pBuildingInfo->BuildingKind == BUILDINGKIND_FACTORY)
						{
							nSlotStartX = TARGET_FACTORY_SLOT_START_X;
						}
						int index = (pt.x - nSlotStartX) / SLOT_INTERVAL_X;
						// end 2008-03-14 by bhsohn Á¶ÇŐ˝Ä °łĽ±ľČ

						if (index >= 0 && m_vecTarget.size() > index)
						{
							auto* pItemInfo = m_vecTarget[index];
							if (pItemInfo)
							{
								// 2009-02-03 by bhsohn ŔĺÂř ľĆŔĚĹŰ şń±ł ĹřĆÁ
								//g_pGameMain->m_pItemInfo->SetItemInfoUser( pItemInfo, pt.x, pt.y );
								g_pGameMain->SetItemInfoUser(pItemInfo, pt.x, pt.y);
								// end 2009-02-03 by bhsohn ŔĺÂř ľĆŔĚĹŰ şń±ł ĹřĆÁ
							}
						}
					}
					else
					{

						g_pGameMain->SetItemInfo(0, 0, 0, 0);
					}
				}
			}

			if(pt.y > OK_BUTTON_START_Y && 
				pt.y < OK_BUTTON_START_Y+BUTTON_SIZE_Y)
			{
				if( pt.x > OK_BUTTON_START_X && 
					pt.x < OK_BUTTON_START_X+BUTTON_SIZE_X)
				{
					if(m_nButtonState[0] != BUTTON_STATE_DOWN)
						m_nButtonState[0] = BUTTON_STATE_UP;
				}
				else 
				{
					m_nButtonState[0] = BUTTON_STATE_NORMAL;
				}
				/////////////////////////////////////////////////////////////////////////////
				
				if( pt.x > CANCEL_BUTTON_START_X && 
					pt.x < CANCEL_BUTTON_START_X+BUTTON_SIZE_X)
				{
					if(m_nButtonState[1] != BUTTON_STATE_DOWN)
						m_nButtonState[1] = BUTTON_STATE_UP;
				}
				else 
				{
					m_nButtonState[1] = BUTTON_STATE_NORMAL;
				}
			}
			else
			{
				m_nButtonState[0] = BUTTON_STATE_NORMAL;
				m_nButtonState[1] = BUTTON_STATE_NORMAL;
			}
		}
		break;
	case WM_LBUTTONDOWN:
		{
			POINT pt;
			pt.x = LOWORD(lParam);
			pt.y = HIWORD(lParam);
			CheckMouseReverse(&pt);
	
			{
				auto bBreak = false;
				for (auto nMainId = MACRO_SLOW_NONE; nMainId < MACRO_RADIO_SLOW_OPTIONS_COUNT; nMainId++)
				{
					if (bBreak)
						break;
				
					if (TRUE == m_pMacroRadioBtn[nMainId]->OnLButtonDown(pt))
					{
						for (auto i{ MACRO_SLOW_NONE }; i < MACRO_RADIO_SLOW_OPTIONS_COUNT; i++) {//disable all buttons
							m_pMacroRadioBtn[i]->SetRadioBtn(FALSE);
							m_bMacroRadioSel[i] = false;
						}
						m_pMacroRadioBtn[nMainId]->SetRadioBtn(TRUE);
						m_bMacroRadioSel[nMainId] = true;
						bBreak = true;
					}

				}
				if(bBreak)
					return  INF_MSGPROC_BREAK;
			}

			{
				auto bBreak = false;
				for (auto nMainId = MACRO_RADIO_ENCHANT_CAP_NO; nMainId < MACRO_RADIO_ENCHANT_CAP_COUNT; nMainId++)
				{
					if (bBreak)
						break;
				
					if (TRUE == m_pMacroEnchCapBtn[nMainId]->OnLButtonDown(pt))
					{
						for (auto i{ MACRO_RADIO_ENCHANT_CAP_NO }; i < MACRO_RADIO_ENCHANT_CAP_COUNT; i++) {//disable all buttons
							m_pMacroEnchCapBtn[i]->SetRadioBtn(FALSE);
							m_bMacroEnchCapSel[i] = false;
						}
						m_pMacroEnchCapBtn[nMainId]->SetRadioBtn(TRUE);
						m_bMacroEnchCapSel[nMainId] = true;
						if(nMainId == MACRO_RADIO_ENCHANT_CAP_10)
							g_pD3dApp->m_pChat->CreateChatChild("Selected e10 cap!", COLOR_CHAT_MAP);
						if(nMainId == MACRO_RADIO_ENCHANT_CAP_11)
							g_pD3dApp->m_pChat->CreateChatChild("Selected e11 cap!", COLOR_CHAT_MAP);
						if (nMainId == MACRO_RADIO_ENCHANT_CAP_12)
							g_pD3dApp->m_pChat->CreateChatChild("Selected e12 cap!", COLOR_CHAT_MAP);
						if (nMainId == MACRO_RADIO_ENCHANT_CAP_NO)
							g_pD3dApp->m_pChat->CreateChatChild("Selected non limited enchanting!", COLOR_CHAT_MAP);

						bBreak = true;
					}

				}
				if(bBreak)
					return  INF_MSGPROC_BREAK;
			}
//			if(((CINFGameMain*)m_pParent)->m_stSelectItem.pSelectItem &&
//				((CINFGameMain*)m_pParent)->m_stSelectItem.bySelectType == ITEM_LAB_POS)
//			{
//				m_bSelectDown = TRUE;
//			}

			// 2008-03-14 by bhsohn Á¶ÇŐ˝Ä °łĽ±ľČ
			auto bOldEditMode = m_pNumEditBox->IsEditMode();
			if(TRUE == m_pNumEditBox->OnLButtonDown(pt) && (m_pBuildingInfo->BuildingKind == BUILDINGKIND_FACTORY))
			{			
				if(!bOldEditMode)
				{
					UpdateMixPrice();// Á¶ÇŐ°ˇ°Ý ÇĄ˝Ă
				}
				m_pNumEditBox->EnableEdit(TRUE);
				// ąöĆ°Ŕ§żˇ ¸¶żě˝ş°ˇ ŔÖ´Ů.
				return  INF_MSGPROC_BREAK;				
			}
			m_pNumEditBox->EnableEdit(FALSE);
			if(bOldEditMode)
			{				
				UpdateMixPrice();// Á¶ÇŐ°ˇ°Ý ÇĄ˝Ă
			}
			// end 2008-03-14 by bhsohn Á¶ÇŐ˝Ä °łĽ±ľČ

			if(pt.y > OK_BUTTON_START_Y && 
				pt.y < OK_BUTTON_START_Y + BUTTON_SIZE_Y)
			{
				if( pt.x > OK_BUTTON_START_X && 
					pt.x < OK_BUTTON_START_X+BUTTON_SIZE_X)
				{
					m_nButtonState[0] = BUTTON_STATE_DOWN;
				}
				else 
				{
					m_nButtonState[0] = BUTTON_STATE_NORMAL;
				}
				if( pt.x > CANCEL_BUTTON_START_X && 
					pt.x < CANCEL_BUTTON_START_X+BUTTON_SIZE_X)
				{
					m_nButtonState[1] = BUTTON_STATE_DOWN;
				}
				else 
				{
					m_nButtonState[1] = BUTTON_STATE_NORMAL;
				}
			}
		}
		break;
	case WM_LBUTTONUP:
		{
			POINT pt;
			pt.x = LOWORD(lParam);
			pt.y = HIWORD(lParam);
			CheckMouseReverse(&pt);
			auto nWindowPosY = g_pGameMain->m_nLeftWindowY;
						
			if(pt.y > OK_BUTTON_START_Y && 
				pt.y < OK_BUTTON_START_Y + BUTTON_SIZE_Y)
			{
				if( pt.x > OK_BUTTON_START_X && 
					pt.x < OK_BUTTON_START_X+BUTTON_SIZE_X)
				{
#ifdef _INET_ANTI_MACRO_OLD
					if(m_vecSourceMacro.empty()){
						if(lastPosClicked.x == pt.x && lastPosClicked.y == pt.y) {					
							nClickCount++;
						}
						else {
							lastPosClicked = pt;
							nClickCount = 1;
						}
						if(nClickCount >= 5) {
							nClickCount = 0;
							MessageBox(nullptr, "Macro usage isn't allowed to create items at OldSchoolRivals! Code for support: 0x211", "OldSchoolRivals", MB_OK | MB_ICONERROR);
							FDBG("AntiCheat :: Trigger Macro 1Q");
							exit(0);
						}
					}
#endif			
					// 2008-03-14 by bhsohn Á¶ÇŐ˝Ä °łĽ±ľČ
					m_pNumEditBox->EnableEdit(FALSE);
					
					if(m_bShowTarget)
					{
						OnButtonClicked(2);// OK
					}
					else
					{
						OnButtonClicked(0); // SEND
					}
					m_nButtonState[0] = BUTTON_STATE_UP;
				}
				else 
				{
					m_nButtonState[0] = BUTTON_STATE_NORMAL;
				}
				if( pt.x > CANCEL_BUTTON_START_X && 
					pt.x < CANCEL_BUTTON_START_X+BUTTON_SIZE_X)
				{
					OnButtonClicked(1);
					m_nButtonState[1] = BUTTON_STATE_UP;
				}
				else 
				{
					m_nButtonState[1] = BUTTON_STATE_NORMAL;
				}
			}
			auto nSourceMaxCount = 0;
			if(m_pBuildingInfo->BuildingKind == BUILDINGKIND_LABORATORY)
			{
				// 2009. 01. 21 by ckPark ŔÎĂ¦Ć® Č®·ü Áő°ˇ Ä«µĺ
				//nSourceMaxCount = 2;
				// Č®·ü Áő°ˇ Ä«µĺ ¶§ą®żˇ ĂÖ´ë 4°ł·Î ´Ă¸°´Ů
				// 2011-11-04 by hsson Á¶ÇŐ ˝Ă Ŕç·á ľĆŔĚĹŰŔĚ ĂÖ´ë 4°łŔÎ°ÍŔ» 5°ł±îÁö ´Ă¸˛
				//nSourceMaxCount = 3;
				nSourceMaxCount = 4;
				// end 2011-11-04 by hsson Á¶ÇŐ ˝Ă Ŕç·á ľĆŔĚĹŰŔĚ ĂÖ´ë 4°łŔÎ°ÍŔ» 5°ł±îÁö ´Ă¸˛
				// end 2009. 01. 21 by ckPark ŔÎĂ¦Ć® Č®·ü Áő°ˇ Ä«µĺ
			}
			else if(m_pBuildingInfo->BuildingKind == BUILDINGKIND_FACTORY)
			{
				nSourceMaxCount = (SOURCE_NUMBER_X*SOURCE_NUMBER_Y)-1;
			}
			if(pt.x > MACRO_SOURCE_SLOT1_X &&
				pt.x < MACRO_SOURCE_SLOT1_X + SLOT_INTERVAL_X * 3 &&
				pt.y > MACRO_SOURCE_SLOT1_Y &&
				pt.y < MACRO_SOURCE_SLOT1_Y + SLOT_INTERVAL_Y &&
				((CINFGameMain*)m_pParent)->m_stSelectItem.pSelectItem &&
				((CINFGameMain*)m_pParent)->m_stSelectItem.bySelectType == ITEM_INVEN_POS &&
				m_vecSourceMacro.size() <= 2)
			{
				auto* pItemInfo = (CItemInfo*)((CINFGameMain*)m_pParent)->m_stSelectItem.pSelectItem->pItem;
				if (IS_COUNTABLE_ITEM(pItemInfo->Kind))
				{
					if (m_pSelectItem == NULL)
					{
						m_pSelectItem = pItemInfo;

						if (BUILDINGKIND_LABORATORY == m_pBuildingInfo->BuildingKind &&
							IS_SPECIAL_COUNTABLE_ITEM(pItemInfo->Kind))
						{
							if (FindItemFromSource(pItemInfo->UniqueNumber, true) == NULL)
							{
								if(IS_RARE_TARGET_ITEMKIND(pItemInfo->Kind))
									InvenToSourceItem(pItemInfo, 1,true);
								else
									InvenToSourceItem(pItemInfo, pItemInfo->CurrentCount, true);
							}
							m_pSelectItem = NULL;
						}
						else
						{
#ifdef _M_X64
							((CINFGameMain*)m_pParent)->m_pInfWindow->AddMsgBox( STRMSG_C_CITYLAP_0001, _Q_LAB_ITEM_NUMBER, (DWORD_PTR)this, pItemInfo->CurrentCount);//"¸î°ł¸¦ żĂ¸®˝Ă°Ú˝Ŕ´Ď±î?"
#else
							((CINFGameMain*)m_pParent)->m_pInfWindow->AddMsgBox( STRMSG_C_CITYLAP_0001, _Q_LAB_ITEM_NUMBER, (DWORD)this, pItemInfo->CurrentCount);//"¸î°ł¸¦ żĂ¸®˝Ă°Ú˝Ŕ´Ď±î?"
#endif
						}
					}
				}
				else
				{
					if (pItemInfo->Wear != WEAR_NOT_ATTACHED)
					{
						((CINFGameMain*)m_pParent)->m_pInfWindow->AddMsgBox(
							STRMSG_C_CITYLAP_0002, _MESSAGE);//"ÂřżëµČ ľĆŔĚĹŰŔş żĂ¸± Ľö ľř˝Ŕ´Ď´Ů."
					}
					else
					{
						// 2007-12-13 by dgwoo ˝Ă°ŁÁ¦ ľĆŔĚĹŰŔĚ¸éĽ­ »çżëÇŃ ľĆŔĚĹŰŔĚ¶ó¸é ĆŃĹä¸®żˇ żĂ¸±Ľö ľř´Ů.
						if ((pItemInfo->ItemInfo->Kind == ITEMKIND_ACCESSORY_TIMELIMIT
							|| COMPARE_BIT_FLAG(pItemInfo->ItemInfo->ItemAttribute, ITEM_ATTR_TIME_LIMITE)
							|| COMPARE_BIT_FLAG(pItemInfo->ItemInfo->ItemAttribute, ITEM_ATTR_DELETED_TIME_LIMITE_AFTER_USED)) // 2008-11-26 by bhsohn Ŕý´ë˝Ă°Ł Á¦ÇŃ ľĆŔĚĹŰ ±¸Çö
							&& pItemInfo->GetItemPassTime() != 0)
						{
							g_pD3dApp->m_pChat->CreateChatChild(STRMSG_C_071212_0100, COLOR_ERROR);//"ÇŘ´ç ľĆŔĚĹŰŔş Á¶ÇŐ ÇŇ Ľö ŔÖ´Â »óĹÂ°ˇ ľĆ´Ő´Ď´Ů."
						}
						else
						{
							if (IS_RARE_TARGET_ITEMKIND(pItemInfo->Kind))
								InvenToSourceItem(pItemInfo, 1, true);
							else
								InvenToSourceItem(pItemInfo, pItemInfo->CurrentCount, true);
						}

					}
				}
			}
			else if( pt.x > SOURCE_SLOT_START_X &&
				pt.x < SOURCE_SLOT_START_X + SLOT_INTERVAL_X*4 &&
				pt.y > SOURCE_SLOT_START_Y &&
				pt.y < SOURCE_SLOT_START_Y + SLOT_INTERVAL_Y*2 &&
				((CINFGameMain*)m_pParent)->m_stSelectItem.pSelectItem &&
				((CINFGameMain*)m_pParent)->m_stSelectItem.bySelectType == ITEM_INVEN_POS &&
				m_vecSource.size() <= nSourceMaxCount )//(SOURCE_NUMBER_X*SOURCE_NUMBER_Y)-1)
			{
				auto* pItemInfo = (CItemInfo*)((CINFGameMain*)m_pParent)->m_stSelectItem.pSelectItem->pItem;

				if(IS_COUNTABLE_ITEM(pItemInfo->Kind))
				{					
					if(m_pSelectItem == NULL)
					{
						m_pSelectItem = pItemInfo;

						// şôµůÁľ·ů°ˇ ĆŃĹä¸®°ˇ ľĆ´Ď°ĹłŞ, ľĆŔĚĹŰŔĚ ŔÎĂľĆ®,°·şíŔĚ ľĆ´Ď¸é ¸Ţ˝ĂÁö ąÚ˝ş¸¦ ¶çżî´Ů.
						if(	BUILDINGKIND_LABORATORY == m_pBuildingInfo->BuildingKind && 
							IS_SPECIAL_COUNTABLE_ITEM(pItemInfo->Kind) )
						{
							if(FindItemFromSource(pItemInfo->UniqueNumber) == NULL)
							{
//								InvenToSourceItem(((CINFGameMain*)m_pParent)->m_pInven->m_pSelectItem, 1);
								InvenToSourceItem(pItemInfo, 1);
							}
							m_pSelectItem = NULL;
						}
						else
						{
#ifdef _M_X64
							((CINFGameMain*)m_pParent)->m_pInfWindow->AddMsgBox( STRMSG_C_CITYLAP_0001, _Q_LAB_ITEM_NUMBER, (DWORD_PTR)this, pItemInfo->CurrentCount);//"¸î°ł¸¦ żĂ¸®˝Ă°Ú˝Ŕ´Ď±î?"
#else
							((CINFGameMain*)m_pParent)->m_pInfWindow->AddMsgBox( STRMSG_C_CITYLAP_0001, _Q_LAB_ITEM_NUMBER, (DWORD)this, pItemInfo->CurrentCount);//"¸î°ł¸¦ żĂ¸®˝Ă°Ú˝Ŕ´Ď±î?"
#endif
						}
					}					
				}
				else
				{
					if(pItemInfo->Wear != WEAR_NOT_ATTACHED)
					{
						((CINFGameMain*)m_pParent)->m_pInfWindow->AddMsgBox(
							STRMSG_C_CITYLAP_0002, _MESSAGE );//"ÂřżëµČ ľĆŔĚĹŰŔş żĂ¸± Ľö ľř˝Ŕ´Ď´Ů."
					}
					else
					{
						// 2007-12-13 by dgwoo ˝Ă°ŁÁ¦ ľĆŔĚĹŰŔĚ¸éĽ­ »çżëÇŃ ľĆŔĚĹŰŔĚ¶ó¸é ĆŃĹä¸®żˇ żĂ¸±Ľö ľř´Ů.
						if((pItemInfo->ItemInfo->Kind == ITEMKIND_ACCESSORY_TIMELIMIT 
							|| COMPARE_BIT_FLAG(pItemInfo->ItemInfo->ItemAttribute, ITEM_ATTR_TIME_LIMITE)
							|| COMPARE_BIT_FLAG(pItemInfo->ItemInfo->ItemAttribute, ITEM_ATTR_DELETED_TIME_LIMITE_AFTER_USED)) // 2008-11-26 by bhsohn Ŕý´ë˝Ă°Ł Á¦ÇŃ ľĆŔĚĹŰ ±¸Çö
							&& pItemInfo->GetItemPassTime() != 0)
						{
							g_pD3dApp->m_pChat->CreateChatChild(STRMSG_C_071212_0100,COLOR_ERROR);//"ÇŘ´ç ľĆŔĚĹŰŔş Á¶ÇŐ ÇŇ Ľö ŔÖ´Â »óĹÂ°ˇ ľĆ´Ő´Ď´Ů."
						}
						else
						{
							InvenToSourceItem(pItemInfo, 1);
						}
						
					}
					//m_vecSource.push_back(((CINFGameMain*)m_pParent)->m_pInven->m_pSelectItem);
				}
				// 2008-08-22 by bhsohn EP3 ŔÎşĄĹä¸® Ăł¸®
				if(g_pGameMain && g_pGameMain->m_pInven)
				{
					g_pGameMain->SetToolTip(NULL, 0, 0);
					g_pGameMain->m_pInven->SetItemInfo(NULL, 0, 0);
					g_pGameMain->m_pInven->SetMultiSelectItem(NULL);	// ´ŮÁß Ăł¸® Á¦°Ĺ
					g_pGameMain->m_pInven->SetSelectItem(NULL);
				}
				// end 2008-08-22 by bhsohn EP3 ŔÎşĄĹä¸® Ăł¸®

			}
		}
		break;
	case WM_KEYDOWN:
		{
			// 2008-03-14 by bhsohn Á¶ÇŐ˝Ä °łĽ±ľČ
			if(m_pNumEditBox->WndProc(uMsg, wParam, lParam))
			{
				UpdateMixPrice();// Á¶ÇŐ°ˇ°Ý ÇĄ˝Ă
				return INF_MSGPROC_BREAK;
			}
			// end 2008-03-14 by bhsohn Á¶ÇŐ˝Ä °łĽ±ľČ
		}
		break;
	}
	return INF_MSGPROC_NORMAL;
}

void CINFCityLab::OnButtonClicked(int nButton)
{
	switch(nButton)
	{
	case 0:	//	SEND
	{
		if (m_pBuildingInfo->BuildingKind == BUILDINGKIND_LABORATORY)
		{
			if (m_vecSourceMacro.empty()) {
#ifdef _INET_ANTI_MACRO_OLD
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
							FDBG("AntiCheat :: Trigger Macro 2Q");
							exit(0);
						}
						vecLastClickTime.clear();

						nTicksSinceLastClick = 0;
					}
				}
#endif			
				if (g_pGameMain->m_pInfWindow->IsExistMsgBox(_Q_ENCHANT_PREVENTION))
				{
					return;
				}

				// 2010. 06. 18 by jskim ŔÎĂ¦Ć® °ć°í ¸Ţ˝ĂÁö Ăß°ˇ
				if (FALSE == m_bIsEnchantCheck && IsWarning_EnchantFail())
				{
					g_pGameMain->m_pInfWindow->AddMsgBox(STRMSG_C_100618_0407, _Q_ENCHANT_PREVENTION);	//"ŔÎĂľĆ® ĆÄ±«ąćÁö Ä«µĺ°ˇ ľřľî ŔÎĂľĆ® ľĆŔĚĹŰŔĚ ĆÄ±« µÉĽö ŔÖ˝Ŕ´Ď´Ů."	
					return;
				}

				m_bIsEnchantCheck = FALSE;
				//end 2010. 06. 18 by jskim ŔÎĂ¦Ć® °ć°í ¸Ţ˝ĂÁö Ăß°ˇ
				auto bGamble = FALSE;

				MSG_FC_ITEM_USE_ENCHANT sMsg;
				memset((char*)&sMsg, 0x00, sizeof(sMsg));

				// 2010. 04. 21 by jskim ˝Ĺ±Ô ·°Ĺ° ¸Ó˝Ĺ ±¸Çö
				auto nIsPrefix = NULL;
				auto nOverlapItem = NULL;
				//end 2010. 04. 21 by jskim ˝Ĺ±Ô ·°Ĺ° ¸Ó˝Ĺ ±¸Çö

				auto it = m_vecSource.begin();

				CItemInfo* pcItemInfo = NULL;

				while (it != m_vecSource.end())
				{
					pcItemInfo = (CItemInfo*)*it;

					if (pcItemInfo->Kind == ITEMKIND_ENCHANT || pcItemInfo->Kind == ITEMKIND_GAMBLE)
					{
						// 2010. 04. 21 by jskim ˝Ĺ±Ô ·°Ĺ° ¸Ó˝Ĺ ±¸Çö
						//if(sMsg.EnchantItemUniqueNumber != 0)
//						
// 						{
// 							g_pD3dApp->m_pChat->CreateChatChild(STRMSG_C_CITYLAP_0003,COLOR_ERROR);//"Ŕß¸řµČ ľĆŔĚĹŰ ±¸ĽşŔÔ´Ď´Ů."
// 							InitData();
// 							return;
// 						}
						if (nOverlapItem == NULL)
						{
							nOverlapItem = pcItemInfo->Kind;
						}
						else
						{
							if (nOverlapItem != pcItemInfo->Kind)
							{
								ErrorMsg_InvalidEnchantList(STRMSG_C_CITYLAP_0003);
								return;
							}
						}

						//end 2010. 04. 21 by jskim ˝Ĺ±Ô ·°Ĺ° ¸Ó˝Ĺ ±¸Çö
						if (pcItemInfo->Kind == ITEMKIND_GAMBLE)
						{
							bGamble = TRUE;
						}
						// 2010. 04. 21 by jskim ˝Ĺ±Ô ·°Ĺ° ¸Ó˝Ĺ ±¸Çö
						//sMsg.EnchantItemUniqueNumber = (*it)->UniqueNumber;
						if (pcItemInfo->ItemInfo->IsExistDesParam(DES_RARE_FIX_PREFIX))
						{
							if (sMsg.EnchantItemUniqueNumber != 0)
							{
								ErrorMsg_InvalidEnchantList(STRMSG_C_CITYLAP_0003);
								return;
							}
							sMsg.EnchantItemUniqueNumber = pcItemInfo->UniqueNumber;
							nIsPrefix = RARE_FIX_PREFIX;
						}
						else if (pcItemInfo->ItemInfo->IsExistDesParam(DES_RARE_FIX_SUFFIX))
						{
							if (sMsg.EnchantItemUniqueNumber2 != 0)
							{
								ErrorMsg_InvalidEnchantList(STRMSG_C_CITYLAP_0003);
								return;
							}
							sMsg.EnchantItemUniqueNumber2 = pcItemInfo->UniqueNumber;
							nIsPrefix = RARE_FIX_SUFFIX;
						}

						else if (pcItemInfo->ItemInfo->IsExistDesParam(DES_RARE_FIX_PREFIX_INITIALIZE))
						{
							if (sMsg.EnchantItemUniqueNumber != 0)
							{
								ErrorMsg_InvalidEnchantList(STRMSG_C_CITYLAP_0003);
								return;
							}
							sMsg.EnchantItemUniqueNumber = pcItemInfo->UniqueNumber;
							nIsPrefix = RARE_FIX_PREFIX;
						}
						else if (pcItemInfo->ItemInfo->IsExistDesParam(DES_RARE_FIX_SUFFIX_INITIALIZE))
						{
							if (sMsg.EnchantItemUniqueNumber2 != 0)
							{
								ErrorMsg_InvalidEnchantList(STRMSG_C_CITYLAP_0003);
								return;
							}
							sMsg.EnchantItemUniqueNumber2 = pcItemInfo->UniqueNumber;
							nIsPrefix = RARE_FIX_SUFFIX;
						}
						else
						{
							if (sMsg.EnchantItemUniqueNumber != 0)
							{
								ErrorMsg_InvalidEnchantList(STRMSG_C_CITYLAP_0003);
								return;
							}
							sMsg.EnchantItemUniqueNumber = pcItemInfo->UniqueNumber;
						}
						//end 2010. 04. 21 by jskim ˝Ĺ±Ô ·°Ĺ° ¸Ó˝Ĺ ±¸Çö
					}
					else if (pcItemInfo->Kind == ITEMKIND_PREVENTION_DELETE_ITEM)
					{
						// 						// 2008-05-29 by bhsohn ŔÎĂľĆ® °ü·Ă ąö±×ĽöÁ¤
						// 						if(sMsg.AttachItemUniqueNumber != 0)
						// 						{
						// 							g_pD3dApp->m_pChat->CreateChatChild(STRMSG_C_CITYLAP_0003,COLOR_ERROR);//"Ŕß¸řµČ ľĆŔĚĹŰ ±¸ĽşŔÔ´Ď´Ů."
						// 							InitData();
						// 							return;
						// 						}
						//						sMsg.AttachItemUniqueNumber = pcItemInfo->UniqueNumber;
						// 						// end 2008-05-29 by bhsohn ŔÎĂľĆ® °ü·Ă ąö±×ĽöÁ¤


												// 2009. 01. 21 by ckPark ŔÎĂ¦Ć® Č®·ü Áő°ˇ Ä«µĺ
												// ĆÄ±«ąćÁöÄ«µĺłŞ Č®·üÁő°ˇÄ«µĺ µŃ´Ů ľĆŔĚĹŰÄ«ŔÎµĺ°ˇ °°Ŕ¸ąÇ·Î
												// DES_ENCHANT_INCREASE_PROBABILITY ŔĚ Á¸ŔçÇĎ¸é Č®·ü Áő°ˇ Ä«µĺ·Î ŔÎ˝Ä
						if (pcItemInfo->ItemInfo->GetParameterValue(DES_ENCHANT_INCREASE_PROBABILITY) != 0.0f)
						{
							if (sMsg.IncreaseProbabilityItemUID != 0)
							{
								ErrorMsg_InvalidEnchantList(STRMSG_C_CITYLAP_0003);
								return;
							}

							sMsg.IncreaseProbabilityItemUID = pcItemInfo->UniqueNumber;
						}
						// DES_ENCHANT_INCREASE_PROBABILITY ŔĚ Á¸ŔçÇĎÁö ľĘŔ¸¸é ĆÄ±«ąćÁö Ä«µĺ·Î ŔÎ˝Ä
						else
						{
							if (sMsg.AttachItemUniqueNumber != 0)
							{
								ErrorMsg_InvalidEnchantList(STRMSG_C_CITYLAP_0003);
								return;
							}
							sMsg.AttachItemUniqueNumber = pcItemInfo->UniqueNumber;
						}
						// end 2009. 01. 21 by ckPark ŔÎĂ¦Ć® Č®·ü Áő°ˇ Ä«µĺ

					}
					// 2010. 02. 11 by ckPark ąßµż·ů ŔĺÂřľĆŔĚĹŰ

// 					else if(IS_WEAPON((*it)->Kind)
// 						|| ((*it)->Kind == ITEMKIND_DEFENSE)
// 						// 2008-09-26 by bhsohn ˝Ĺ±Ô ŔÎĂľĆ® Ăł¸®
// 						|| ((*it)->Kind == ITEMKIND_SUPPORT)
// 						|| ((*it)->Kind == ITEMKIND_RADAR))	
// 						// end 2008-09-26 by bhsohn ˝Ĺ±Ô ŔÎĂľĆ® Ăł¸®
					else if (IS_ENCHANT_TARGET_ITEMKIND(pcItemInfo->Kind))

						// end 2010. 02. 11 by ckPark ąßµż·ů ŔĺÂřľĆŔĚĹŰ
					{
						// 2008-05-29 by bhsohn ŔÎĂľĆ® °ü·Ă ąö±×ĽöÁ¤
						if (sMsg.TargetItemUniqueNumber != 0)
						{
							ErrorMsg_InvalidEnchantList(STRMSG_C_CITYLAP_0003);
							return;
						}
						// end 2008-05-29 by bhsohn ŔÎĂľĆ® °ü·Ă ąö±×ĽöÁ¤
						sMsg.TargetItemUniqueNumber = pcItemInfo->UniqueNumber;
					}
					else
					{
						// 2008-05-29 by bhsohn ŔÎĂľĆ® °ü·Ă ąö±×ĽöÁ¤
						ErrorMsg_InvalidEnchantList(STRMSG_C_CITYLAP_0003);
						return;
						// end 2008-05-29 by bhsohn ŔÎĂľĆ® °ü·Ă ąö±×ĽöÁ¤
					}

					++it;
				}

				// 2010. 04. 21 by jskim ˝Ĺ±Ô ·°Ĺ° ¸Ó˝Ĺ ±¸Çö
				if (sMsg.EnchantItemUniqueNumber == 0 && sMsg.EnchantItemUniqueNumber2 != 0)
				{
					sMsg.EnchantItemUniqueNumber = sMsg.EnchantItemUniqueNumber2;
					sMsg.EnchantItemUniqueNumber2 = NULL;
				}

				if (sMsg.EnchantItemUniqueNumber != 0 && sMsg.EnchantItemUniqueNumber2 != 0)
				{
					auto IS_FIX_INFO = NULL;
					auto it = m_vecSource.begin();
					while (it != m_vecSource.end())
					{
						if (IS_FIX_INFO == 0)
						{
							if ((*it)->ItemInfo->IsExistDesParam(DES_RARE_FIX_PREFIX_INITIALIZE) ||
								(*it)->ItemInfo->IsExistDesParam(DES_RARE_FIX_SUFFIX_INITIALIZE))
							{
								IS_FIX_INFO = ITIALIZE_ITEM;
							}
							else if ((*it)->ItemInfo->IsExistDesParam(DES_RARE_FIX_PREFIX) ||
								(*it)->ItemInfo->IsExistDesParam(DES_RARE_FIX_SUFFIX))
							{
								IS_FIX_INFO = RARE_FIX_ITEM;
							}
						}
						else
						{
							if (((*it)->ItemInfo->IsExistDesParam(DES_RARE_FIX_PREFIX_INITIALIZE) ||
								(*it)->ItemInfo->IsExistDesParam(DES_RARE_FIX_SUFFIX_INITIALIZE)) &&
								IS_FIX_INFO == RARE_FIX_ITEM)
							{
								ErrorMsg_InvalidEnchantList(STRMSG_C_CITYLAP_0003);
								return;
							}
							else if (((*it)->ItemInfo->IsExistDesParam(DES_RARE_FIX_PREFIX) ||
								(*it)->ItemInfo->IsExistDesParam(DES_RARE_FIX_SUFFIX)) &&
								IS_FIX_INFO == ITIALIZE_ITEM)
							{
								ErrorMsg_InvalidEnchantList(STRMSG_C_CITYLAP_0003);
								return;
							}
						}
						++it;
					}
				}
				//end 2010. 04. 21 by jskim ˝Ĺ±Ô ·°Ĺ° ¸Ó˝Ĺ ±¸Çö

				if (bGamble == TRUE && sMsg.AttachItemUniqueNumber != 0)
				{
					g_pD3dApp->m_pChat->CreateChatChild(STRMSG_C_CITYLAP_0003, COLOR_ERROR);
					InitData();
				}
				else if (sMsg.TargetItemUniqueNumber != 0 && sMsg.EnchantItemUniqueNumber != 0)
				{
					m_vecTarget = m_vecSource;
					m_vecSource.clear();
					m_bShowTarget = FALSE;
					g_pFieldWinSocket->SendMsg(T_FC_ITEM_USE_ENCHANT, (char*)&sMsg, sizeof(sMsg));
					g_pD3dApp->m_bRequestEnable = FALSE;
					bEnchantFinished = false;
				//	g_pD3dApp->m_bRequestEnable = FALSE;			// 2006-06-19 by ispark

					//InitData();
				}
				else
				{
					g_pD3dApp->m_pChat->CreateChatChild(STRMSG_C_CITYLAP_0003, COLOR_ERROR);
					InitData();
				}
			}
			else
			{
//////////////////////////////////////////////////////macro exec/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				MSG_FC_ITEM_USE_ENCHANT sMsg;
				memset((char*)&sMsg, 0x00, sizeof(sMsg));
				if(!g_pStoreData->FindItemInInventoryByItemNum(7025041) && !g_pStoreData->FindItemInInventoryByItemNum(7025042) 
					&& !g_pStoreData->FindItemInInventoryByItemNum(7025043) && !g_pStoreData->FindItemInInventoryByItemNum(7025044))
				{
					g_pD3dApp->m_pChat->CreateChatChild("You need \\cMacro License\\c!", COLOR_ERROR);
					InitData();
					break;
				}
				if(m_vecSourceMacro.size() < 3)
				{
					g_pD3dApp->m_pChat->CreateChatChild("You have to insert all items to run macro!", COLOR_ERROR);
					InitData();
					break;
				}
				if(!IS_RARE_TARGET_ITEMKIND(m_vecSourceMacro[0]->Kind))
				{
					g_pD3dApp->m_pChat->CreateChatChild("First item have to be armor or weapon!", COLOR_ERROR);
					InitData();
					break;
				}
				bool bGamblingRun = false;
				bool bEnchantRun = false;
				if (m_vecSourceMacro[1]->Kind == ITEMKIND_GAMBLE)
				{
					bGamblingRun = true;
					bEnchantRun = false;
				}
				else if (m_vecSourceMacro[1]->Kind == ITEMKIND_ENCHANT)
				{
					bGamblingRun = false;
					bEnchantRun = true;
				}

				if(!bGamblingRun && !bEnchantRun)
				{
					g_pD3dApp->m_pChat->CreateChatChild("Second item have to be gamble (prefix or suffix) or enchant card!", COLOR_ERROR);
					InitData();
					break;
				}

				if (!bEnchantFinished)
				{
					g_pD3dApp->m_pChat->CreateChatChild("Too fast! Server have not responded yet!", COLOR_ERROR);
					break;
				}

				{
					if (!COMPARE_ITEMKIND(m_vecSourceMacro[1]->ItemInfo->ReqItemKind, m_vecSourceMacro[0]->ItemInfo->Kind))
					{
						g_pD3dApp->m_pChat->CreateChatChild("Invalid item combination!", COLOR_ERROR);
						InitData();
						break;
					}

					if(bGamblingRun)
					{
						if (m_vecSourceMacro[1]->ItemInfo->IsExistDesParam(DES_RARE_FIX_PREFIX) || m_vecSourceMacro[1]->ItemInfo->IsExistDesParam(DES_RARE_FIX_SUFFIX)) {
							if (m_vecSourceMacro[2]->ItemInfo->IsExistDesParam(DES_RARE_FIX_PREFIX_INITIALIZE) || m_vecSourceMacro[2]->ItemInfo->IsExistDesParam(DES_RARE_FIX_SUFFIX_INITIALIZE)) {
								if (m_vecSourceMacro[1]->ItemInfo->IsExistDesParam(DES_RARE_FIX_PREFIX) && m_vecSourceMacro[2]->ItemInfo->IsExistDesParam(DES_RARE_FIX_SUFFIX_INITIALIZE))
								{
									g_pD3dApp->m_pChat->CreateChatChild("You need prefix wipes!", COLOR_ERROR);
									InitData();
									break;
								}
								if (m_vecSourceMacro[1]->ItemInfo->IsExistDesParam(DES_RARE_FIX_SUFFIX) && m_vecSourceMacro[2]->ItemInfo->IsExistDesParam(DES_RARE_FIX_PREFIX_INITIALIZE))
								{
									g_pD3dApp->m_pChat->CreateChatChild("You need suffix wipes!", COLOR_ERROR);
									InitData();
									break;
								}
							}
							else
							{
								g_pD3dApp->m_pChat->CreateChatChild("Third item have to be prefix or suffix wipe!", COLOR_ERROR);
								InitData();
								break;
							}
						}
						else
						{
							g_pD3dApp->m_pChat->CreateChatChild("Second item have to be gamble (prefix or suffix)!", COLOR_ERROR);
							InitData();
							break;
						}
					}

					if(bEnchantRun)
					{
						if (m_vecSourceMacro[2]->Kind != ITEMKIND_PREVENTION_DELETE_ITEM) //3rd item need to be prot
						{			
							g_pD3dApp->m_pChat->CreateChatChild("Third item have to be protect card!", COLOR_ERROR);
							InitData();
							break;
						}
					}
				}
				
				if(bGamblingRun)
				{
					if(m_vecSourceMacro[1]->ItemInfo->IsExistDesParam(DES_RARE_FIX_PREFIX) && m_vecSourceMacro[0]->PrefixCodeNum == 0)
					{//new gamble prefix
						time_t now = time(nullptr);
						m_nTicksAfterLastUsage = now;
						b_IsPrefixMacro = true;
						bEnchantMacro = false;
						g_pD3dApp->m_pChat->CreateChatChild("Prefix applied!", COLOR_ERROR);
						sMsg.TargetItemUniqueNumber = m_vecSourceMacro[0]->UniqueNumber;
						sMsg.EnchantItemUniqueNumber = m_vecSourceMacro[1]->UniqueNumber;
						m_vecSourceMacro[1]->CurrentCount > 0 ? m_vecSourceMacro[1]->CurrentCount -= 1 : OnButtonClicked(1);
						sMsg.AttachItemUniqueNumber = NULL;
						sMsg.IncreaseProbabilityItemUID = NULL;
						sMsg.EnchantItemUniqueNumber2 = NULL;				
						m_bShowTarget = FALSE;
						g_pFieldWinSocket->SendMsg(T_FC_ITEM_USE_ENCHANT, reinterpret_cast<char*>(&sMsg), sizeof(sMsg));
						bEnchantFinished = false;
						//g_pD3dApp->m_bRequestEnable = FALSE;
						InitData();
					}
					else if (m_vecSourceMacro[1]->ItemInfo->IsExistDesParam(DES_RARE_FIX_PREFIX) && m_vecSourceMacro[0]->PrefixCodeNum != 0)
					{ //regamble
						if (!m_bMacroRadioSel[MACRO_SLOW_NONE]) {
							time_t now = time(nullptr);

							auto nClicked = now;
							if (m_nTicksAfterLastUsage == 0) {
								m_nTicksAfterLastUsage = now;
							}
							else {
								auto nRes = nClicked - m_nTicksAfterLastUsage;
								if (nRes < 2 && m_bMacroRadioSel[MACRO_SLOW_SECOND]) {
									g_pD3dApp->m_pChat->CreateChatChild("Delay 2s active - you have to wait or deactivate this!", COLOR_ERROR);
									break;
								}
								if (nRes < 1 && m_bMacroRadioSel[MACRO_SLOW_HALF])
								{
									g_pD3dApp->m_pChat->CreateChatChild("Delay 1s active - you have to wait or deactivate this!", COLOR_ERROR);
									break;
								}

								m_nTicksAfterLastUsage = 0;
							}
						}
						b_IsPrefixMacro = true;
						bEnchantMacro = false;
						g_pD3dApp->m_pChat->CreateChatChild("Prefix wipe applied!", COLOR_ERROR);
						sMsg.TargetItemUniqueNumber = m_vecSourceMacro[0]->UniqueNumber;
						sMsg.EnchantItemUniqueNumber = m_vecSourceMacro[2]->UniqueNumber;
						m_vecSourceMacro[2]->CurrentCount > 0 ? m_vecSourceMacro[2]->CurrentCount -= 1 : OnButtonClicked(1);
						sMsg.AttachItemUniqueNumber = NULL;
						sMsg.IncreaseProbabilityItemUID = NULL;
						sMsg.EnchantItemUniqueNumber2 = NULL;
						m_bShowTarget = FALSE;
						g_pFieldWinSocket->SendMsg(T_FC_ITEM_USE_ENCHANT, reinterpret_cast<char*>(&sMsg), sizeof(sMsg));
						bEnchantFinished = false;
						//g_pD3dApp->m_bRequestEnable = FALSE;
						InitData();
					}
					else if (m_vecSourceMacro[1]->ItemInfo->IsExistDesParam(DES_RARE_FIX_SUFFIX) && m_vecSourceMacro[0]->SuffixCodeNum != 0)
					{ //regamble
						if (!m_bMacroRadioSel[MACRO_SLOW_NONE]) {
							time_t now = time(nullptr);

							auto nClicked = now;
							if (m_nTicksAfterLastUsage == 0) {
								m_nTicksAfterLastUsage = now;
							}
							else {
								auto nRes = nClicked - m_nTicksAfterLastUsage;
								if (nRes < 2 && m_bMacroRadioSel[MACRO_SLOW_SECOND]) {
									g_pD3dApp->m_pChat->CreateChatChild("Delay 2s active - you have to wait or deactivate this!", COLOR_ERROR);
									break;
								}
								if (nRes < 1 && m_bMacroRadioSel[MACRO_SLOW_HALF])
								{
									g_pD3dApp->m_pChat->CreateChatChild("Delay 1s active - you have to wait or deactivate this!", COLOR_ERROR);
									break;
								}

								m_nTicksAfterLastUsage = 0;
							}
						}
						b_IsPrefixMacro = false;
						bEnchantMacro = false;
						g_pD3dApp->m_pChat->CreateChatChild("Suffix wipe applied!", COLOR_ERROR);
						sMsg.TargetItemUniqueNumber = m_vecSourceMacro[0]->UniqueNumber;
						sMsg.EnchantItemUniqueNumber = m_vecSourceMacro[2]->UniqueNumber;
						m_vecSourceMacro[2]->CurrentCount > 0 ? m_vecSourceMacro[2]->CurrentCount -= 1 : OnButtonClicked(1);
						sMsg.AttachItemUniqueNumber = NULL;
						sMsg.IncreaseProbabilityItemUID = NULL;
						sMsg.EnchantItemUniqueNumber2 = NULL;
						m_bShowTarget = FALSE;
						g_pFieldWinSocket->SendMsg(T_FC_ITEM_USE_ENCHANT, reinterpret_cast<char*>(&sMsg), sizeof(sMsg));
						bEnchantFinished = false;
						//g_pD3dApp->m_bRequestEnable = FALSE;
						InitData();
					}
					else if (m_vecSourceMacro[1]->ItemInfo->IsExistDesParam(DES_RARE_FIX_SUFFIX) && m_vecSourceMacro[0]->SuffixCodeNum == 0)
					{//new gamble suff
						time_t now = time(nullptr);
						m_nTicksAfterLastUsage = now;
						b_IsPrefixMacro = false;
						bEnchantMacro = false;
						g_pD3dApp->m_pChat->CreateChatChild("Suffix applied!", COLOR_ERROR);
						sMsg.TargetItemUniqueNumber = m_vecSourceMacro[0]->UniqueNumber;
						sMsg.EnchantItemUniqueNumber = m_vecSourceMacro[1]->UniqueNumber;
						m_vecSourceMacro[1]->CurrentCount > 0 ? m_vecSourceMacro[1]->CurrentCount -= 1 : OnButtonClicked(1);
						sMsg.AttachItemUniqueNumber = NULL;
						sMsg.IncreaseProbabilityItemUID = NULL;
						sMsg.EnchantItemUniqueNumber2 = NULL;				
						m_bShowTarget = FALSE;
						g_pFieldWinSocket->SendMsg(T_FC_ITEM_USE_ENCHANT, reinterpret_cast<char*>(&sMsg), sizeof(sMsg));
						bEnchantFinished = false;
						//g_pD3dApp->m_bRequestEnable = FALSE;
						InitData();
					}
				}

				if(bEnchantRun)
				{
					if (m_bMacroEnchCapSel[MACRO_RADIO_ENCHANT_CAP_10]) {
						if(m_vecSourceMacro[0]->GetEnchantNumber() >= 10)
						{
							g_pD3dApp->m_pChat->CreateChatChild("Item is already e10!", COLOR_ERROR);
							break;
						}
					}
					if (m_bMacroEnchCapSel[MACRO_RADIO_ENCHANT_CAP_11]) {
						if(m_vecSourceMacro[0]->GetEnchantNumber() >= 11)
						{
							g_pD3dApp->m_pChat->CreateChatChild("Item is already e11!", COLOR_ERROR);
							break;
						}
					}
					if (m_bMacroEnchCapSel[MACRO_RADIO_ENCHANT_CAP_12]) {
						if(m_vecSourceMacro[0]->GetEnchantNumber() >= 12)
						{
							g_pD3dApp->m_pChat->CreateChatChild("Item is already e12!", COLOR_ERROR);
							break;
						}
					}
					time_t now = time(nullptr);
					m_nTicksAfterLastUsage = now;
					b_IsPrefixMacro = false;
					bEnchantMacro = true;
					sMsg.TargetItemUniqueNumber = m_vecSourceMacro[0]->UniqueNumber;
					sMsg.EnchantItemUniqueNumber = m_vecSourceMacro[1]->UniqueNumber;
					m_vecSourceMacro[1]->CurrentCount > 0 ? m_vecSourceMacro[1]->CurrentCount -= 1 : OnButtonClicked(1);
				
					if(m_vecSourceMacro[2]->ItemInfo->IsExistDesParam(DES_ENCHANT_PREVENTION_DELETE_USE_ENCHANT) 
						&& m_vecSourceMacro[0]->GetEnchantNumber() >= (int)m_vecSourceMacro[2]->ItemInfo->GetParameterValue(DES_ENCHANT_PREVENTION_DELETE_USE_ENCHANT)){
						
						if(m_vecSourceMacro[2])
							sMsg.AttachItemUniqueNumber = m_vecSourceMacro[2]->UniqueNumber;

						m_vecSourceMacro[2]->CurrentCount > 0 ? m_vecSourceMacro[2]->CurrentCount -= 1 : OnButtonClicked(1);
					}
					else
						sMsg.AttachItemUniqueNumber = NULL;
									
					sMsg.IncreaseProbabilityItemUID = NULL;
					sMsg.EnchantItemUniqueNumber2 = NULL;				
					m_bShowTarget = FALSE;
					g_pFieldWinSocket->SendMsg(T_FC_ITEM_USE_ENCHANT, reinterpret_cast<char*>(&sMsg), sizeof(sMsg));
					bEnchantFinished = false;
					//g_pD3dApp->m_bRequestEnable = FALSE;
					InitData();
				}
			}
		}
		else if (m_pBuildingInfo->BuildingKind == BUILDINGKIND_FACTORY)
		{
			if (m_vecSource.empty())
				break;

			char pPacket[SIZE_MAX_PACKET];
			MSG_FC_ITEM_MIX_ITEMS sMsg;
			memset(&sMsg, 0x00, sizeof(MSG_FC_ITEM_MIX_ITEMS));
			sMsg.NumOfItems = m_vecSource.size();

			// 2008-03-14 by bhsohn Á¶ÇŐ˝Ä °łĽ±ľČ
			char chBuff[256];
			STRNCPY_MEMSET(chBuff,m_pNumEditBox->GetString(),256);
			//m_pNumEditBox->GetString(chBuff, 256);
			sMsg.nMixCounts = atoi(chBuff);
			if (sMsg.nMixCounts > COUNT_MAX_MIXING_COUNT)
			{
				sMsg.nMixCounts = COUNT_MAX_MIXING_COUNT;
			}
			// end 2008-03-14 by bhsohn Á¶ÇŐ˝Ä °łĽ±ľČ

			memcpy(pPacket, (char*)&sMsg, sizeof(sMsg));

			auto i = 0;

			auto it = m_vecSource.begin();

			while (it != m_vecSource.end())
			{
				ITEM_UNIQUE_NUMBER_W_COUNT itemCount;
				itemCount.ItemUniqueNumber = (*it)->UniqueNumber;
				itemCount.Count = (*it)->CurrentCount;
				memcpy(pPacket + sizeof(sMsg) + i * sizeof(ITEM_UNIQUE_NUMBER_W_COUNT),
					(char*)&itemCount,
					sizeof(ITEM_UNIQUE_NUMBER_W_COUNT));
				++it;
				i++;
			}

			g_pFieldWinSocket->SendMsg(T_FC_ITEM_MIX_ITEMS,
				pPacket,
				sizeof(sMsg) + sizeof(ITEM_UNIQUE_NUMBER_W_COUNT) * m_vecSource.size());

			g_pD3dApp->m_bRequestEnable = FALSE;			// 2006-06-19 by ispark
			m_vecTarget = m_vecSource;
			m_vecSource.clear();
			m_bShowTarget = FALSE;
		}
	}
		break;

	case 1:	//	CANCEL
		{
			InitDataMacro();
			InitData();		
		}
		break;

	case 2: // OK
		{
			InitData();
		}
		break;
	}
}
#ifdef _INET_ENCHANT_CHANCE
int nenchantCardNum1 = 0;
int nenchItemNum1 = 0;

void CINFCityLab::SendCheckChancePacket(INT EnchantItemNum, INT UniqueNumber, INT IncreaseValue)
{
	MSG_FC_INFO_GET_ENCHANT_CHANCE cMsg;
	cMsg.EnchantItemNum = EnchantItemNum;
	cMsg.UniqueNumber = UniqueNumber;
	cMsg.IncreaseValue = IncreaseValue;
	g_pFieldWinSocket->SendMsg(T_FC_INFO_GET_ENCHANT_CHANCE, (char*)&cMsg, sizeof(cMsg));
}
#endif
void CINFCityLab::InvenToSourceItem(CItemInfo* pItemInfo, int nCount, bool bMacro/* = false*/)
{
#ifdef _INET_ENCHANT_CHANCE
	if (!bMacro) {
		if (ITEMKIND_ENCHANT == pItemInfo->Kind || ITEMKIND_GAMBLE == pItemInfo->Kind)
		{
			if (ITEMKIND_GAMBLE != pItemInfo->Kind)
			{
				nenchantCardNum1 = pItemInfo->ItemNum;
				if (nenchItemNum1 != 0)
				{
					SendCheckChancePacket(nenchantCardNum1, nenchItemNum1, fE_IncreaseValue);
					if (fE_IncreaseValue != -1)
					{
						fE_IncreaseValue = -1;
						nenchantCardNum1 = 0;
						nenchItemNum1 = 0;
					}
				}
			}
			else
			{

			}
			//then get cost
			MSG_FC_INFO_GET_ENCHANT_COST sMsg;
			sMsg.EnchantItemNum = pItemInfo->ItemNum;
			g_pFieldWinSocket->SendMsg(T_FC_INFO_GET_ENCHANT_COST, (char*)&sMsg, sizeof(sMsg));
		}
		else if (ITEMKIND_PREVENTION_DELETE_ITEM == pItemInfo->Kind)
		{
			if (pItemInfo->ItemInfo->GetParameterValue(DES_ENCHANT_INCREASE_PROBABILITY) != 0.0f)
			{
				fE_IncreaseValue = pItemInfo->ItemInfo->GetParameterValue(DES_ENCHANT_INCREASE_PROBABILITY);//-use this for float (/ 10000.0f) + 1.0f;
				if (nenchItemNum1 != 0 && nenchantCardNum1 != 0 && fE_IncreaseValue != -1)
				{
					SendCheckChancePacket(nenchantCardNum1, nenchItemNum1, fE_IncreaseValue);
					nenchItemNum1 = 0;
					nenchantCardNum1 = 0;
					fE_IncreaseValue = -1;
				}
			}
		}
		else
		{
			nenchItemNum1 = pItemInfo->UniqueNumber;

			if (nenchantCardNum1 != 0)
			{
				SendCheckChancePacket(nenchantCardNum1, nenchItemNum1, fE_IncreaseValue);
				if (fE_IncreaseValue != -1)
				{
					fE_IncreaseValue = -1;
					nenchantCardNum1 = 0;
					nenchItemNum1 = 0;
				}
			}
		}
	}
#else
	if (ITEMKIND_ENCHANT == pItemInfo->Kind || ITEMKIND_GAMBLE == pItemInfo->Kind)
	{
		MSG_FC_INFO_GET_ENCHANT_COST sMsg;
		sMsg.EnchantItemNum = pItemInfo->ItemNum;
		g_pFieldWinSocket->SendMsg(T_FC_INFO_GET_ENCHANT_COST, (char*)&sMsg, sizeof(sMsg));
	}
#endif
	if (pItemInfo == nullptr) {
		return;
	}
	// InvenżˇĽ­ Áöżî´Ů.
	if(IS_COUNTABLE_ITEM(pItemInfo->Kind))
	{
		if (!bMacro) {
			ASSERT_ASSERT(pItemInfo->CurrentCount >= nCount);

			auto it = m_vecSource.begin();
			while (it != m_vecSource.end())
			{
				if ((*it)->UniqueNumber == pItemInfo->UniqueNumber)
				{
					(*it)->CurrentCount += nCount;
					break;
				}
				++it;
			}
			if (it == m_vecSource.end())
			{
				auto* pNewItem = new CItemInfo((ITEM_GENERAL*)pItemInfo);

				// 2010. 02. 11 by ckPark ąßµż·ů ŔĺÂřľĆŔĚĹŰ
				//pNewItem->SetEnchantParam( pItemInfo->GetEnchantParamFactor(), pItemInfo->GetEnchantNumber() );
				// ąßµż·ů ŔÎĂ¦Ć® ą× ÄđĹ¸ŔÓ Ăß°ˇ şą»ç
				pNewItem->CopyItemInfo(pItemInfo);
				// end 2010. 02. 11 by ckPark ąßµż·ů ŔĺÂřľĆŔĚĹŰ

				pNewItem->CurrentCount = nCount;
				m_vecSource.push_back(pNewItem);
				// 2007-12-17 by bhsohn Á¶ÇŐ °ˇ°Ý ÇĄ˝Ă
				// Á¶ÇŐ°ˇ°Ý ÇĄ˝Ă
				UpdateMixPrice();
			}
		}
		else
		{
			ASSERT_ASSERT(pItemInfo->CurrentCount >= nCount);

			auto it = m_vecSourceMacro.begin();
			while (it != m_vecSourceMacro.end())
			{
				if ((*it)->UniqueNumber == pItemInfo->UniqueNumber)
				{
					(*it)->CurrentCount += nCount;
					break;
				}
				++it;
			}
			if (it == m_vecSourceMacro.end())
			{
				auto* pNewItem = new CItemInfo((ITEM_GENERAL*)pItemInfo);
				pNewItem->CopyItemInfo(pItemInfo);
				pNewItem->CurrentCount = nCount;
				m_vecSourceMacro.push_back(pNewItem);
				UpdateMixPrice();
			}
		}
		g_pStoreData->UpdateItemCount( pItemInfo->UniqueNumber, pItemInfo->CurrentCount - nCount);
	}
	else 
	{
		auto* pNewItem = new CItemInfo((ITEM_GENERAL*)pItemInfo);

		pNewItem->CopyItemInfo( pItemInfo );
		
		if(!bMacro)
			m_vecSource.push_back(pNewItem);
		else
			m_vecSourceMacro.push_back(pNewItem);

		g_pStoreData->DeleteItem( pItemInfo->UniqueNumber );

		UpdateMixPrice();
	}
}

BOOL CINFCityLab::PutRareInfo(MSG_FC_STORE_UPDATE_RARE_FIX* pMsg)
{
	auto it = m_vecTarget.begin();
	while(it != m_vecTarget.end())
	{
		if((*it)->UniqueNumber == pMsg->ItemUID )
		{
			(*it)->ChangeRareInfo(pMsg->PrefixCodeNum, pMsg->SuffixCodeNum);
			return TRUE;
		}
		++it;
	}

	auto it2 = m_vecSourceMacro.begin();
	while (it2 != m_vecSourceMacro.end())
	{
		if ((*it2)->UniqueNumber == pMsg->ItemUID)
		{
			(*it2)->ChangeRareInfo(pMsg->PrefixCodeNum, pMsg->SuffixCodeNum);
			return TRUE;
		}
		++it2;
	}
	
	return FALSE;
}
///////////////////////////////////////////////////////////////////////////////
/// \fn			BOOL CINFCityLab::PutEnchant(MSG_FC_ITEM_PUT_ENCHANT* pMsg)
/// \brief		ŔÎĂ¦Ć® ˝Ăµµ °á°ú - Ľş°ř˝Ă Ľ­ąöżˇĽ­ łŃ°ÜÁŘ´Ů.
/// \author		dhkwon
/// \date		2004-07-18 ~ 2004-07-18
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
//BOOL CINFCityLab::PutEnchant(MSG_FC_ITEM_PUT_ENCHANT* pMsg)
//{
//	FLOG( "CStoreData::PutEnchant(MSG_FC_ITEM_PUT_ENCHANT* pMsg)" );
//	vector<CItemInfo*>::iterator it = m_vecTarget.begin();
//	while(it != m_vecTarget.end())
//	{
//		if((*it)->UniqueNumber == pMsg->Enchant.TargetItemUniqueNumber )
//		{
//			(*it)->PutEnchant( pMsg->Enchant.DestParameter, pMsg->Enchant.ParameterValue );
//			return TRUE;
//		}
//		it ++;
//	}
//	return FALSE;
//}
BOOL CINFCityLab::PutEnchant(MSG_FC_ITEM_PUT_ENCHANT* pMsg)
{
	FLOG( "CStoreData::PutEnchant(MSG_FC_ITEM_PUT_ENCHANT* pMsg)" );
	auto it = m_vecTarget.begin();
	while(it != m_vecTarget.end())
	{
		if((*it)->UniqueNumber == pMsg->Enchant.TargetItemUniqueNumber )
		{
			(*it)->AddEnchantItem( pMsg->Enchant.EnchantItemNum );
			return TRUE;
		}
		++it;
	}

	auto it2 = m_vecSourceMacro.begin();
	while (it2 != m_vecSourceMacro.end())
	{
		if((*it2)->UniqueNumber == pMsg->Enchant.TargetItemUniqueNumber )
		{
			(*it2)->AddEnchantItem( pMsg->Enchant.EnchantItemNum );
			return TRUE;
		}
		++it2;
	}
	return FALSE;
}
void CINFCityLab::InitDataMacro()
{
	auto it = m_vecSourceMacro.begin();
	while (it != m_vecSourceMacro.end())
	{
		(*it)->ItemWindowIndex = POS_INVALID_POSITION;
		if (g_pStoreData && (*it)->CurrentCount > 0) // for empty spaces in inventory
		{
			g_pStoreData->PutItem(reinterpret_cast<char*>(static_cast<ITEM_GENERAL*>(*it)), TRUE);
			auto* pItemInfo = g_pStoreData->FindItemInInventoryByUniqueNumber((*it)->UniqueNumber);
			ASSERT_ASSERT(pItemInfo);
			pItemInfo->CopyItemInfo((*it));
		}
		SAFE_DELETE(*it);
		++it;
	}
	m_vecSourceMacro.clear();
}
void CINFCityLab::InitData()
{
#ifdef _INET_ENCHANT_CHANCE
	nenchantCardNum1 = 0;
	nenchItemNum1 = 0;
	fE_IncreaseValue = -1;
#endif
	
	auto it = m_vecSource.begin();
	while (it != m_vecSource.end()) {
		(*it)->ItemWindowIndex = POS_INVALID_POSITION;
		if (g_pStoreData)
		{
			g_pStoreData->PutItem(reinterpret_cast<char*>(static_cast<ITEM_GENERAL*>(*it)), TRUE);
			auto* pItemInfo = g_pStoreData->FindItemInInventoryByUniqueNumber((*it)->UniqueNumber);
			ASSERT_ASSERT(pItemInfo);
			pItemInfo->CopyItemInfo((*it));
		}
		SAFE_DELETE(*it);
		++it;
	}
	m_vecSource.clear();

	it = m_vecTarget.begin();
	while (it != m_vecTarget.end())
	{
		(*it)->ItemWindowIndex = POS_INVALID_POSITION;
		if (g_pStoreData)
		{
			g_pStoreData->PutItem(reinterpret_cast<char*>(static_cast<ITEM_GENERAL*>(*it)), TRUE);
			auto* pItemInfo = g_pStoreData->FindItemInInventoryByUniqueNumber((*it)->UniqueNumber);
			ASSERT_ASSERT(pItemInfo);
			pItemInfo->CopyItemInfo((*it));
		}
		SAFE_DELETE(*it);
		++it;
	}
	m_vecTarget.clear();

	g_pShuttleChild->ResortingItem();
	if (g_pGameMain && g_pGameMain->m_pInven) {
		g_pGameMain->m_pInven->SetScrollEndLine();
		g_pGameMain->m_pInven->SetAllIconInfo();
	}
	
	memset(m_szPrice, 0x00, 64);
	m_bShowTarget = FALSE;
	m_bIsEnchantCheck = FALSE;
	
	if (g_pGameMain)
		g_pGameMain->m_pInfWindow->DeleteMsgBox(_Q_ENCHANT_PREVENTION);
}

void CINFCityLab::SetPrice(int nPrice)
{
	wsprintf( m_szPrice, "%d", nPrice);
	if( g_pGameMain->m_pInven->GetItemSpi() < nPrice )
	{
		g_pD3dApp->m_pChat->CreateChatChild(STRMSG_C_CITYLAP_0004,COLOR_ERROR);//"ĽöĽö·á°ˇ şÎÁ·ÇŐ´Ď´Ů."
	}
}
#ifdef _INET_ENCHANT_CHANCE
void CINFCityLab::SetChance(Prob10K_t nChance)
{
	auto p100 = nChance / 100.0f;
	fE_CurrentPercent = p100;
	char sztmp[100];
	sprintf(m_szChance, "Chance : %0.2f%%", fE_CurrentPercent);

	if (p100 < 100)
	{
		char szTep[1024];
		sprintf(szTep, "Chance to enchant is less than 100 %% , It's (%0.2f %%), be careful with enchanting to avoid loosing items", p100);
		if (p100 >= 75)
		{
			g_pD3dApp->m_pChat->CreateChatChild(szTep, COLOR_ERROR);//"ĽöĽö·á°ˇ şÎÁ·ÇŐ´Ď´Ů."
		}
		else if (p100 >= 50 && p100 < 75)
		{
			g_pD3dApp->m_pChat->CreateChatChild(szTep, COLOR_CHAT_SELL);
		}
		else if (p100 < 50)
		{
			g_pD3dApp->m_pChat->CreateChatChild(szTep, COLOR_CHAT_WAR);
		}
	}
}
#endif
void CINFCityLab::DeleteTargetItem(UID64_t nItemUniqueNumber)
{
	auto it =  m_vecTarget.begin();
	while( it != m_vecTarget.end() )
	{
		if( (*it)->UniqueNumber == nItemUniqueNumber )
		{
			SAFE_DELETE( *it );
			it = m_vecTarget.erase( it );
			break;
		}
		++it;
	}
	auto i2t =  m_vecSourceMacro.begin();
	while( i2t != m_vecSourceMacro.end() )
	{
		if( (*i2t)->UniqueNumber == nItemUniqueNumber )
		{
			SAFE_DELETE( *i2t );
			i2t = m_vecSourceMacro.erase( i2t );
			break;
		}
		++i2t;
	}
	if(m_vecTarget.size() > 0)
	{
		DBGOUT("Enchant/Mix Result(View)\n");
		
		if(m_vecSourceMacro.empty())
			m_bShowTarget = TRUE;
		
		g_pD3dApp->m_bRequestEnable = TRUE;			// 2006-06-19 by ispark
		g_pD3dApp->m_fRequestEnableTime = REQUEST_ENABLE_INIT_TIME;
	}
	else
	{
		m_bShowTarget = FALSE;
	}
}

void CINFCityLab::PutTargetItem(ITEM_GENERAL* pItem)
{
	auto it =  m_vecTarget.begin();
	while( it != m_vecTarget.end() )
	{
		if( (*it)->UniqueNumber == pItem->UniqueNumber )
		{
			DBGOUT("Mix System Result - Already Being Item.");
			return;
		}
		++it;
	}
	auto* pNewItem = new CItemInfo(pItem);
	m_vecTarget.push_back(pNewItem);
	
	if(m_vecSourceMacro.empty())
		m_bShowTarget = TRUE;
	
	g_pD3dApp->m_bRequestEnable = TRUE;			// 2006-06-19 by ispark
	g_pD3dApp->m_fRequestEnableTime = REQUEST_ENABLE_INIT_TIME;
}

CItemInfo* CINFCityLab::GetTargetItemInfo()
{
	if(bEnchantMacro && !m_vecSourceMacro.empty())
	{
		auto it =  m_vecSourceMacro.begin();
		while( it != m_vecSourceMacro.end() )
		{
			if( (*it)->Kind != ITEMKIND_ENCHANT 
				&& (*it)->Kind != ITEMKIND_GAMBLE 
				&& (*it)->Kind != ITEMKIND_PREVENTION_DELETE_ITEM)
			{
				return *it;
			}
			++it;
		}
	}
	else
	{
		auto it =  m_vecTarget.begin();
		while( it != m_vecTarget.end() )
		{
			if( (*it)->Kind != ITEMKIND_ENCHANT 
				&& (*it)->Kind != ITEMKIND_GAMBLE 
				&& (*it)->Kind != ITEMKIND_PREVENTION_DELETE_ITEM)
			{
				return *it;
			}
			++it;
		}
	}
	return NULL;
}

CItemInfo* CINFCityLab::FindItemFromSource(UID64_t UniqueNumber, bool bMacro/* = false*/)
{
	if (!bMacro) {
		auto it = m_vecSource.begin();
		while (it != m_vecSource.end())
		{
			if (UniqueNumber == (*it)->UniqueNumber)
			{
				return (*it);
			}
			++it;
		}
	}
	else
	{
		auto it = m_vecSourceMacro.begin();
		while (it != m_vecSourceMacro.end())
		{
			if (UniqueNumber == (*it)->UniqueNumber)
			{
				return (*it);
			}
			++it;
		}
	}
	
	return nullptr;
}

CItemInfo* CINFCityLab::FindItemFromTarget(UID64_t UniqueNumber)
{
	auto it = m_vecTarget.begin();
	while(it != m_vecTarget.end())
	{
		if(UniqueNumber == (*it)->UniqueNumber)
		{
			return (*it);
		}
		++it;
	}
	return NULL;
}
///////////////////////////////////////////////////////////////////////////////
/// \fn			void CINFCityLab::ReSetTargetItemNum(UID64_t UniqueNumber, int nItemNum)
/// \brief		Á¶ÇŐ˝Ă ľĆŔĚĹŰ łŃąö¸¸ ąŮ˛ď´Ů.
/// \author		ispark
/// \date		2006-06-15 ~ 2006-06-15
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CINFCityLab::ReSetTargetItemNum(UID64_t UniqueNumber, int nItemNum)
{
	auto* pItemInfo = FindItemFromTarget(UniqueNumber);
	auto* pItem = g_pDatabase->GetServerItemInfo(nItemNum);

	if(pItem == NULL || pItemInfo == NULL)
	{
		return;
	}
	
	pItemInfo->ItemInfo = pItem;
	pItemInfo->ItemNum = nItemNum;

	// 2009-04-07 by bhsohn ľĆŔĚĹŰ łŃąö ąö±× ĽöÁ¤
	pItemInfo->ResetRealItemInfo();
	// end 2009-04-07 by bhsohn ľĆŔĚĹŰ łŃąö ąö±× ĽöÁ¤
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			void CINFCityLab::SetSelectItem(CItemInfo* pItemInfo)
/// \brief		
/// \author		ispark
/// \date		2006-07-27 ~ 2006-07-27
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CINFCityLab::SetSelectItem(CItemInfo* pItemInfo)
{

}

///////////////////////////////////////////////////////////////////////////////
/// \fn			void CINFCityLab::UpdateMixPrice()
/// \brief		
/// \author		// 2007-12-17 by bhsohn Á¶ÇŐ °ˇ°Ý ÇĄ˝Ă
/// \date		2007-12-17 ~ 2007-12-17
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CINFCityLab::UpdateMixPrice()
{
	if(NULL == m_pBuildingInfo 
		|| m_pBuildingInfo->BuildingKind != BUILDINGKIND_FACTORY)
	{
		return;
	}
	// 2008-03-14 by bhsohn Á¶ÇŐ˝Ä °łĽ±ľČ
	char chBuff[256];
	STRNCPY_MEMSET(chBuff,m_pNumEditBox->GetString(),256);
	//m_pNumEditBox->GetString(chBuff, 256);
	auto nMixCounts = atoi(chBuff);
	if(nMixCounts <= 0)
	{
		return;
	}
	else if(nMixCounts > COUNT_MAX_MIXING_COUNT)
	{
		nMixCounts = COUNT_MAX_MIXING_COUNT;
	}
	// end 2008-03-14 by bhsohn Á¶ÇŐ˝Ä °łĽ±ľČ

	auto nCnt =0;
	ITEM_MIXING_INFO	stMixInfo;
	memset(&stMixInfo, 0x00, sizeof(ITEM_MIXING_INFO));
	auto it = m_vecSource.begin();
	while(it != m_vecSource.end())
	{
		if(nCnt >= COUNT_ITEM_MIXING_SOURCE)
		{
			break;
		}
		auto* pItem = (*it);
		// ľĆŔĚĹŰ ąřČŁ
		stMixInfo.SourceItem[nCnt].ItemNum = pItem->ItemNum;
		// °ąĽö
		if(IS_COUNTABLE_ITEM(pItem->Kind))
		{
			// 2008-03-14 by bhsohn Á¶ÇŐ˝Ä °łĽ±ľČ
			//stMixInfo.SourceItem[nCnt].Count = pItem->CurrentCount;			
			stMixInfo.SourceItem[nCnt].Count = pItem->CurrentCount/nMixCounts;			
		}
		else
		{
			stMixInfo.SourceItem[nCnt].Count = 1;			
		}
		nCnt++;
		++it;
	}
	stMixInfo.NumOfSourceItems = nCnt;

	auto* pMixInfo = g_pDatabase->GetMixerPrice(&stMixInfo);
	if(pMixInfo)
	{
		auto nAllPrice = pMixInfo->MixingCost*nMixCounts;
		if(nAllPrice >= 0)
		{
			SetPrice(nAllPrice);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			void				OnOpenInfWnd();
/// \brief		// ĂłŔ˝ »óÁˇ żŔÇÂ
/// \author		// 2008-03-14 by bhsohn Á¶ÇŐ˝Ä °łĽ±ľČ
/// \date		2008-03-14 ~ 2008-03-14
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CINFCityLab::OnOpenInfWnd()
{
	char chBuff[32];
	wsprintf(chBuff, "1");
	m_pNumEditBox->SetString(chBuff, 32);
	m_pNumEditBox->EnableEdit(FALSE);
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			void				OnOpenInfWnd();
/// \brief		// ĂłŔ˝ »óÁˇ żŔÇÂ
/// \author		// 2008-03-14 by bhsohn Á¶ÇŐ˝Ä °łĽ±ľČ
/// \date		2008-03-14 ~ 2008-03-14
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CINFCityLab::OnCloseInfWnd()
{
	m_pNumEditBox->EnableEdit(FALSE);
}


/************************************************************************************************************************************************************
**
**	ŔÎĂ¦Ć® ˝ÇĆĐ °ć°í ÇŘľß ÇĎ´ÂÁö ĂĽĹ©.
**
**	Create Info :	2010. 09. 27. by hsLee.
**
**	Base Info : 'ITEMKIND_PREVENTION_DELETE_ITEM' - ŔÎĂ¦Ć® ĆÄ±« ąćÁö Ä«µĺżÍ ŔÎĂ¦Ć® Č®·ü Áő°ˇ Ä«µĺ´Â Kind°ˇ °°±â ¶§ą®żˇ 
**				'DES_ENCHANT_INCREASE_PROBABILITY' DesParam°ŞŔÇ ŔŻą«·Î ±¸ş°. 
**
**	Update Info : 'ŔÎĂ¦Ć® ĆÄ±« ąćÁö Ä«µĺ'żÍ 'ŔÎĂ¦Ć® Č®·ü Áő°ˇ Ä«µĺ'ŔÇ Kind°ŞŔĚ °°ľĆĽ­ °ć°í ¸Ţ˝ĂÁö ľřŔĚ łŃľî°ˇ´Â ą®Á¦ ĽöÁ¤.	2010. 09. 27. by hsLee.
** 
*************************************************************************************************************************************************************/
bool CINFCityLab :: IsWarning_EnchantFail ( void )
{

	bool bHavePreventionDeleteItem = FALSE;
	bool bWaringEnchantLevel = FALSE;
	bool bEnableEnchant = FALSE;

	auto itr_SourceItem = m_vecSource.begin();
	
	CItemInfo *pcItemInfo = NULL;
	
	while( itr_SourceItem != m_vecSource.end() )
	{
		pcItemInfo = (CItemInfo *)*itr_SourceItem;
		
		if ( pcItemInfo->Kind == ITEMKIND_PREVENTION_DELETE_ITEM && pcItemInfo->ItemInfo->GetParameterValue(DES_ENCHANT_INCREASE_PROBABILITY) == 0.0f )
		{
			bHavePreventionDeleteItem = TRUE;
		}
#ifdef _INET_ENCHANT_CHANCE
		if (fE_CurrentPercent < 100) //if prob is less than 100% THEN show this message box else ignore even without prot
		{
			bWaringEnchantLevel = TRUE;
		}
#else
		if (pcItemInfo->GetEnchantNumber() >= 5)
		{
			bWaringEnchantLevel = TRUE;
		}
#endif
		
		if ( pcItemInfo->Kind == ITEMKIND_ENCHANT && !pcItemInfo->ItemInfo->IsExistDesParam ( DES_ENCHANT_INITIALIZE ) )
		{
			bEnableEnchant = TRUE;
		}
		
		++itr_SourceItem;
	}

	if ( bEnableEnchant && bWaringEnchantLevel && !bHavePreventionDeleteItem )
		return true;

	return false;

}


/**************************************************************************************
**
**	ŔŻČżÇĎÁö ľĘŔş Á¶ÇŐ °ü·Ă żˇ·Ż ¸Ţ˝ĂÁö Ăâ·Â.
**
**	Create Info :	2010. 09. 27. by hsLee.
**
***************************************************************************************/
void CINFCityLab :: ErrorMsg_InvalidEnchantList ( char *pszMessage , const bool a_bInitData /*= true*/ )
{

	if ( pszMessage && pszMessage[0] != 0 )
		g_pD3dApp->m_pChat->CreateChatChild ( pszMessage , COLOR_ERROR ); //"Ŕß¸řµČ ľĆŔĚĹŰ ±¸ĽşŔÔ´Ď´Ů."

	if ( a_bInitData )
		InitData();
}
#ifdef _INET_FACTORY8LAB_DB_CLICK
void CINFCityLab::UpLoadItem(CItemInfo* i_pItem)
{
	if (!m_vecSourceMacro.empty())
		return;
	
	if (m_vecSource.size() <= (SOURCE_NUMBER_X*SOURCE_NUMBER_Y) - 1)
	{
		if (IS_COUNTABLE_ITEM(i_pItem->Kind))
		{
			if (m_pSelectItem == NULL)
			{
				m_pSelectItem = i_pItem;
				if (BUILDINGKIND_LABORATORY == m_pBuildingInfo->BuildingKind &&
					IS_SPECIAL_COUNTABLE_ITEM(i_pItem->Kind))
				{
					if (FindItemFromSource(i_pItem->UniqueNumber) == NULL)
					{
						InvenToSourceItem(i_pItem, 1);
					}
					m_pSelectItem = NULL;
				}
				else
				{
#ifdef _M_X64
					((CINFGameMain*)m_pParent)->m_pInfWindow->AddMsgBox( STRMSG_C_CITYLAP_0001, _Q_LAB_ITEM_NUMBER, (DWORD_PTR)this, i_pItem->CurrentCount);
#else
					((CINFGameMain*)m_pParent)->m_pInfWindow->AddMsgBox( STRMSG_C_CITYLAP_0001, _Q_LAB_ITEM_NUMBER, (DWORD)this, i_pItem->CurrentCount);
#endif
				}
			}
		}
		else
		{
			if (i_pItem->Wear != WEAR_NOT_ATTACHED)
			{
				((CINFGameMain*)m_pParent)->m_pInfWindow->AddMsgBox(
					STRMSG_C_CITYLAP_0002, _MESSAGE);
			}
			else
			{
				if ((i_pItem->ItemInfo->Kind == ITEMKIND_ACCESSORY_TIMELIMIT
					|| COMPARE_BIT_FLAG(i_pItem->ItemInfo->ItemAttribute, ITEM_ATTR_TIME_LIMITE)
					|| COMPARE_BIT_FLAG(i_pItem->ItemInfo->ItemAttribute, ITEM_ATTR_DELETED_TIME_LIMITE_AFTER_USED))
					&& i_pItem->GetItemPassTime() != 0)
				{
					g_pD3dApp->m_pChat->CreateChatChild(STRMSG_C_071212_0100, COLOR_ERROR);
				}
				else
				{
					InvenToSourceItem(i_pItem, 1);
				}

			}
		}
		if (g_pGameMain && g_pGameMain->m_pInven)
		{
			g_pGameMain->SetToolTip(NULL, 0, 0);
			g_pGameMain->m_pInven->SetItemInfo(NULL, 0, 0);
			g_pGameMain->m_pInven->SetMultiSelectItem(NULL);
			g_pGameMain->m_pInven->SetSelectItem(NULL);
		}
	}
}
#endif