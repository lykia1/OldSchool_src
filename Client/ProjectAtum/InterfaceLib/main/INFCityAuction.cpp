// INFWorldRankWnd.cpp: implementation of the CINFCityAuction class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "AtumApplication.h"
#include "INFImage.h"
#include "GameDataLast.h"
#include "INFGameMain.h"
#include "Interface.h"
#include "AtumSound.h"
#include "D3DHanFont.h"
#include "INFCityBase.h"
#include "INFListBox.h"
#include "INFInvenExtend.h"
#include "INFCityAuction.h"
#include "ShuttleChild.h"
#include "INFImageListTreeCtrl.h"
#include "INFNumEditBox.h"
#include "INFEditBox.h"
#include "ItemInfo.h"
#include "AtumDatabase.h"
#include "INFIcon.h"
#include "INFWindow.h"
#include "INFGameMainChat.h"
#include "Chat.h"
#include "StoreData.h"
#include "INFItemInfo.h"
// 랭킹 정보	
#define NUMEDITBOX_POSITION					{310, 500}
#define NUMEDITBOX_WIDTH					26
#define NUMEDITBOX_CAP						20
#define MINLEVEL_NUMEDITBOX_LOCATION_X		263
#define MAXLEVEL_NUMEDITBOX_LOCATION_X		305
#define MINENCHANT_NUMEDITBOX_LOCATION_X	393
#define MAXENCHANT_NUMEDITBOX_LOCATION_X	437
#define NUMEDITBOX_LOCATION_Y				166
#define MINLEVEL							1
#define MAXLEVEL							CHARACTER_MAX_LEVEL
#define MINENCHANT							1
#define MAXENCHANT							40 
#define CATEGORY_LOCATION_X					46
#define CATEGORY_LOCATION_Y					210
#define CATEGORY_WIDTH						154
#define CATEGORY_HEIGHT						370
#define CATEGORY_SUBITEM_LOCATION_X			21
#define CATEGORY_SUBITEM_LOCATION_Y			5
#define CATEGORY_SCROLL_LOCATION_X			154
#define CATEGORY_SCROLL_LOCATION_Y			-5
#define NAMEEDITBOX_LOCATION_X				295
#define NAMEEDITBOX_LOCATION_Y				133
#define NAMEEDITBOX_POSITION				{232, 102}
#define NAMEEDITBOX_WIDTH					260
#define NAMEEDITBOX_CAP						28
// 검색 카테고리 경로
#define CATEGORYPATH_LOCATION_X				50
#define CATEGORYPATH_LOCATION_Y				100
#define CATEGORYPATH_COLOR					0xA7C4DB
#define CATEGORYPATH_ICON_LOCATION_X		CATEGORYPATH_LOCATION_X - 5
#define CATEGORYPATH_ICON_LOCATION_Y		CATEGORYPATH_LOCATION_Y + 4

#define		WORLD_RANK_PAGE_POS_X		343
#define		WORLD_RANK_PAGE_POS_Y		597
#define		WORLD_RANK_PAGE_WIDTH		30

///////////
#define		INTERVAL_SERVICE_RQ_TIME		500   
#define	MONEY_COMBO_LOCATION_X				700
#define	MONEY_COMBO_LOCATION_Y				370
#define MONEY_COMBO_MAIN_WIDTH				103
#define MONEY_COMBO_MAIN_HEIGHT				17
#define MONEY_COMBO_ELE_WIDTH				87
#define MONEY_COMBO_ELE_HEIGHT				15	
#define MONEYEDITBOX_LOCATION_X				690
#define MONEYEDITBOX_LOCATION_Y				396
#define MONEY_IMAGE_LOCATION_X				602
#define MONEY_IMAGE_LOCATION_Y				395
#define NUMEDITBOX_LOCATION_X				650
//#define NUMEDITBOX_LOCATION_Y				311
#define UPCNTBTN_LOCATION_X					768
#define UPCNTBTN_LOCATION_Y					295
#define MAXBTN_LOCATION_X					780
#define MAXBTN_LOCATION_Y					289
#define DOWNCNTBTN_LOCATION_X				768
#define DOWNCNTBTN_LOCATION_Y				305
// 기체 선택
#define	GEAR_COMBO_DEVICE_X					480
#define	GEAR_COMBO_DEVICE_Y					166
#define GEAR_COMBO_DEVICE_MAIN_WIDTH			110
#define GEAR_COMBO_DEVICE_MAIN_HEIGHT		17
#define GEAR_COMBO_DEVICE_ELE_WIDTH			110
#define GEAR_COMBO_DEVICE_ELE_HEIGHT			13


#define	GEAR_COMBO_CUR_X					592
#define	GEAR_COMBO_CUR_Y					166
#define GEAR_COMBO_CUR_MAIN_WIDTH			110
#define GEAR_COMBO_CUR_MAIN_HEIGHT		17
#define GEAR_COMBO_CUR_ELE_WIDTH			110
#define GEAR_COMBO_CUR_ELE_HEIGHT			13


#define SCROLL_PAGE_X	240
#define SCROLL_PAGE_END_X	800
#define SCROLL_PAGE_Y	200
#define SCROLL_PAGE_END_Y	590


#define ITEMLIST_LOCATION_X					219
#define ITEMLIST_LOCATION_Y					230
#define ITEMLIST_WIDTH						565
#define ITEMLIST_HEIGHT						328
#define ITEMLIST_CELL_HEIGHT				40
#define ITEMLIST_ITEMSL_LOCATION_X			235
#define ITEMLIST_ITEMSL_LOCATION_Y			235
#define ITEMLIST_ICON_LOCATION_X			244
#define ITEMLIST_ICON_LOCATION_Y			235
#define ITEMLIST_ITEMCNT_LOCATION_X			26
#define ITEMLIST_ITEMCNT_LOCATION_Y			-2
#define ITEMLIST_ITEM_WIDTH					58
#define ITEMLIST_ENCHANT_LOCATION_X			270
#define ITEMLIST_ENCHANT_WIDTH				58
#define ITEMLIST_NAME_LOCATION_X			300
#define ITEMLIST_NAME_WIDTH					280
#define ITEMLIST_LEVEL_LOCATION_X			540
#define ITEMLIST_LEVEL_WIDTH				58
#define ITEMLIST_REGISTTIME_LOCATION_X		578
#define ITEMLIST_REGISTTIME_WIDTH			73
#define ITEMLIST_PRICE_LOCATION_X			655
#define ITEMLIST_PRICE_WIDTH				91
#define ITEMLIST_MONEYTYPE_LOCATION_X		750
#define ITEMLIST_MONEYTYPE_LOCATION_Y		241
#define ITEMLIST_MAXITEMNAME_LEN			45

#define MONEYEDITBOX_POSITION				{307, 470}
#define MONEYEDITBOX_WIDTH					102
#define MONEYEDITBOX_CAP					20
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//at page start of array index (nFirstPage + m_nSelectPage - 1) * MAX_PER_ONEPAGE)
CINFCityAuction::CINFCityAuction(CAtumNode* pParent)
{
	int nCnt = 0;
	for (nCnt = 0; nCnt < TAB_COUNT; nCnt++)
	{
		m_pBkImage[nCnt] = NULL;
	}

	for (nCnt = 0; nCnt < 3; nCnt++)
	{
		m_pRankBtn[nCnt] = NULL;
	}

	m_byMoneySelect = TC_SPI;
	m_nCurrentTab = TAB_BUY_ITEM;
	
	m_nSelectPage = 0;

	m_pFontRankInfo = NULL;
	int nX, nY;
	nX = nY = 0;
	for (nY = 0; nY < MAX_PER_ONEPAGE; nY++)
	{
		for (nX = 0; nX < MAX_WR_INFO_X; nX++)
		{
			m_pFontTable[nY][nX] = NULL;
		}
	}

	for (nCnt = 0; nCnt < MAX_PAGES_COUNT; nCnt++)
	{
		m_pFontPage[nCnt] = NULL;
	}

	m_dwSendTermTime = 0;

	m_pComboGear = NULL;
	m_pComboCurrency = NULL;

	m_pListTreeCtrl = NULL;
	m_pBtnHover[0] = NULL;
	m_pBtnHover[1] = NULL;
	m_pBtnBUY = NULL;

	m_pCategoryPathArrowImage = NULL;
	m_pFontCategoryPath = NULL;			// 2013-12-04 by ymjoo 거래소 카테고리 경로 표시
	m_nSelectedCategoryId = 0;			// 초기값 : 전체

	m_pNameEditBox = NULL;

	int i;
	for (i = 0; i < 2; ++i)
	{
		m_plevelNumEditBox[i] = NULL;
		m_pEnchantNumEditBox[i] = NULL;
	}
	m_vecItemInfo.clear();
	m_vecMyItemInfo.clear();
	nFirstPage = 1;

	for (int i = 0; i < 3; ++i)
	{
		m_pMoneyImg[i] = NULL;
	}

	m_pIconInfo = NULL;

	m_nSellingTime = 0;
	m_nMaxPages = 1;
	m_nItemCountTotal = 0;
	m_nSelectListItem = -1;
	m_nSelectMyListItem = -1;
	nTickWaitRefresh = -1;
	nTickWaitRefreshMY = -1;

	m_nCurerntInfo = NULL;

	m_nCurrentSortingType = 0;
	m_pNumEditBox = NULL;

	m_pMoneyEditBox = NULL;

	m_pUpCntBtn = NULL;
	m_pDownCntBtn = NULL;
	m_pMaxBtn = NULL;

	m_pMoneyComboBox = NULL;
	m_pRegisterItemBtn = NULL;

	nMylistStartFromId = 0;

	for (i = 0; i < 3; ++i)
	{
		m_pSellOKBtn[i] = NULL;
		m_pTimeLimitBtn[i] = NULL;
	}

	m_pCloseXBtn = NULL;
	bBoughtItem = false;
}

CINFCityAuction::~CINFCityAuction()
{
	DeleteDeviceObjects();
}

HRESULT CINFCityAuction::InitDeviceObjects()
{
	// 배경이 없을땐 NULL을 넣자~
	//CINFDefaultWnd::InitDeviceObjects(NULL);

	char chBkImg[32];
	ZERO_MEMORY(chBkImg);

	int nCnt = 0;
	for (nCnt = 0; nCnt < TAB_COUNT; nCnt++)
	{
		wsprintf(chBkImg, "osr_trd%d", nCnt);
		DataHeader* pDataHeader = g_pGameMain->FindResource(chBkImg);

		if (NULL == m_pBkImage[nCnt] && pDataHeader)
		{
			m_pBkImage[nCnt] = new CINFImage;
			m_pBkImage[nCnt]->InitDeviceObjects(pDataHeader->m_pData, pDataHeader->m_DataSize);
		}
	}

	{
		char szUpBtn[30], szDownBtn[30], szSelBtn[30], szDisBtn[30];
		{
			wsprintf(szUpBtn, "ot_sel00");
			wsprintf(szDownBtn, "ot_sel01");
			wsprintf(szSelBtn, "ot_sel03");
			wsprintf(szDisBtn, "ot_sel02");

			if (NULL == m_pRankBtn[TAB_SELL_ITEM])
			{
				m_pRankBtn[TAB_SELL_ITEM] = new CINFImageBtn;
				m_pRankBtn[TAB_SELL_ITEM]->InitDeviceObjects(szUpBtn, szDownBtn, szSelBtn, szDisBtn);
			}
		}
		{//buy item tab
			wsprintf(szUpBtn, "ot_buy00");
			wsprintf(szDownBtn, "ot_buy01");
			wsprintf(szSelBtn, "ot_buy03");
			wsprintf(szDisBtn, "ot_buy02");

			if (NULL == m_pRankBtn[TAB_BUY_ITEM])
			{
				m_pRankBtn[TAB_BUY_ITEM] = new CINFImageBtn;
				m_pRankBtn[TAB_BUY_ITEM]->InitDeviceObjects(szUpBtn, szDownBtn, szSelBtn, szDisBtn);
			}
		}
		{
			wsprintf(szUpBtn, "ot_au00");
			wsprintf(szDownBtn, "ot_au01");
			wsprintf(szSelBtn, "ot_au03");
			wsprintf(szDisBtn, "ot_au02");

			if (NULL == m_pRankBtn[TAB_FINISHED_ITEM])
			{
				m_pRankBtn[TAB_FINISHED_ITEM] = new CINFImageBtn;
				m_pRankBtn[TAB_FINISHED_ITEM]->InitDeviceObjects(szUpBtn, szDownBtn, szSelBtn, szDisBtn);
			}
		}

		/*for (nCnt = 0; nCnt < TAB_COUNT; nCnt++)
		{
			wsprintf(chBkImg, "wr_bk%d", nCnt);
			DataHeader* pDataHeader = g_pGameMain->FindResource(chBkImg);

			if (NULL == m_pRankBk[nCnt] && pDataHeader)
			{
				m_pRankBk[nCnt] = new CINFImage;
				m_pRankBk[nCnt]->InitDeviceObjects(pDataHeader->m_pData, pDataHeader->m_DataSize);
			}
		}*/
	}
	{
		if (NULL == m_pFontRankInfo)
		{
			m_pFontRankInfo = new CD3DHanFont(_T(g_pD3dApp->GetFontStyle()), 8, D3DFONT_ZENABLE, TRUE, 1024, 32);
			m_pFontRankInfo->InitDeviceObjects(g_pD3dDev);
		}

		int nX, nY;
		nX = nY = 0;
		for (nY = 0; nY < MAX_PER_ONEPAGE; nY++)
		{
			for (nX = 0; nX < MAX_WR_INFO_X; nX++)
			{
				if (NULL == m_pFontTable[nY][nX])
				{
					m_pFontTable[nY][nX] = new CD3DHanFont(_T(g_pD3dApp->GetFontStyle()), 9, D3DFONT_ZENABLE, TRUE, 1024, 32);
					m_pFontTable[nY][nX]->InitDeviceObjects(g_pD3dDev);
				}
			}
		}

		for (nCnt = 0; nCnt < MAX_PAGES_COUNT; nCnt++)
		{
			if (NULL == m_pFontPage[nCnt])
			{
				m_pFontPage[nCnt] = new CD3DHanFont(_T(g_pD3dApp->GetFontStyle()), 9, D3DFONT_ZENABLE, TRUE, 256, 32);
				m_pFontPage[nCnt]->InitDeviceObjects(g_pD3dDev);
			}
		}
	}

	{
		if (NULL == m_pComboGear)
		{
			m_pComboGear = new CINFListBox("cbarena", "cbarenab");
			m_pComboGear->SetUseCulling(TRUE); //글씨 컬링 사용
			m_pComboGear->InitDeviceObjects();
		}
	}
	{
		if (NULL == m_pComboCurrency)
		{
			m_pComboCurrency = new CINFListBox("cbarena", "cbarenab");
			m_pComboCurrency->SetUseCulling(TRUE); //글씨 컬링 사용
			m_pComboCurrency->InitDeviceObjects();
		}
	}
	g_pFieldWinSocket->SendMsg(T_FC_CHARACTER_GET_CASH_MONEY_COUNT, NULL, 0);
	char chPlus[30], chMinus[30], chItem[30], chSel[30];
	wsprintf(chPlus, "m_plus");
	wsprintf(chMinus, "m_minus");
	wsprintf(chItem, "misradn");
	wsprintf(chSel, "tc_catesel");
	if (NULL == m_pListTreeCtrl)
	{
		m_pListTreeCtrl = new CINFImageListTreeCtrl();
	}
	m_pListTreeCtrl->InitDeviceObjects(20);
	m_pListTreeCtrl->InitDeviceEtc(chPlus, chMinus, chItem, chSel);
	m_pListTreeCtrl->SetTextList(TRUE);



	wsprintf(chBkImg, "tc_arrow");
	DataHeader* pDataHeader = g_pGameMain->FindResource(chBkImg);

	if (NULL == m_pCategoryPathArrowImage && pDataHeader)
	{
		m_pCategoryPathArrowImage = new CINFImage;
		
		m_pCategoryPathArrowImage->InitDeviceObjects(pDataHeader->m_pData, pDataHeader->m_DataSize);
	}
	
	{//search btn
		char szUpBtn[30], szDownBtn[30], szSelBtn[30], szDisBtn[30];
		wsprintf(szUpBtn, "btn_smlN");
		wsprintf(szDownBtn, "btn_smlP");
		wsprintf(szSelBtn, "btn_smlH");
		wsprintf(szDisBtn, "btn_smlD");

		if (NULL == m_pBtnHover[0])
		{
			m_pBtnHover[0] = new CINFImageBtn;
			m_pBtnHover[0]->InitDeviceObjects(szUpBtn, szDownBtn, szSelBtn, szDisBtn);
		}

		if (NULL == m_pBtnHover[1])
		{
			m_pBtnHover[1] = new CINFImageBtn;
			m_pBtnHover[1]->InitDeviceObjects(szUpBtn, szDownBtn, szSelBtn, szDisBtn);
		}
	}
	{//BUY btn
		char szUpBtn[30], szDownBtn[30], szSelBtn[30], szDisBtn[30];
		wsprintf(szUpBtn, "btn_buyN");
		wsprintf(szDownBtn, "btn_buyP");
		wsprintf(szSelBtn, "btn_buyH");
		wsprintf(szDisBtn, "btn_buyD");

		if (NULL == m_pBtnBUY)
		{
			m_pBtnBUY = new CINFImageBtn;
			m_pBtnBUY->InitDeviceObjects(szUpBtn, szDownBtn, szSelBtn, szDisBtn);
		}
	}

	if (NULL == m_pFontCategoryPath)
	{
		m_pFontCategoryPath = new CD3DHanFont(_T(g_pD3dApp->GetFontStyle()), 9, D3DFONT_ZENABLE, FALSE, 1024, 32);
		m_pFontCategoryPath->InitDeviceObjects(g_pD3dDev);
	}

	int i;
	for (i = 0; i < 2; ++i)
	{
		if (NULL == m_plevelNumEditBox[i])
		{
			m_plevelNumEditBox[i] = new CINFNumEditBox;
		}
		char chMaxMixCnt[64];

		wsprintf(chMaxMixCnt, "%d", MAXLEVEL);

		POINT ptPos = NUMEDITBOX_POSITION;
		m_plevelNumEditBox[i]->InitDeviceObjects(9, ptPos, NUMEDITBOX_WIDTH, TRUE, NUMEDITBOX_CAP);
		m_plevelNumEditBox[i]->SetMaxStringLen(strlen(chMaxMixCnt));
		m_plevelNumEditBox[i]->SetString(" ", 32);

		if (NULL == m_pEnchantNumEditBox[i])
		{
			m_pEnchantNumEditBox[i] = new CINFNumEditBox;
		}

		wsprintf(chMaxMixCnt, "%d", MAXENCHANT);

		m_pEnchantNumEditBox[i]->InitDeviceObjects(9, ptPos, NUMEDITBOX_WIDTH, TRUE, NUMEDITBOX_CAP);
		m_pEnchantNumEditBox[i]->SetMaxStringLen(strlen(chMaxMixCnt));
		m_pEnchantNumEditBox[i]->SetString(" ", 32);
	}

	{
		if (NULL == m_pNameEditBox)
		{
			m_pNameEditBox = new CINFEditBox;
		}
		POINT ptPos = NAMEEDITBOX_POSITION;
		m_pNameEditBox->InitDeviceObjects(9, ptPos, NAMEEDITBOX_WIDTH, TRUE, NAMEEDITBOX_CAP);
		m_pNameEditBox->SetStringMaxBuff(SIZE_MAX_ARENA_FULL_NAME);
	}

	if (g_pGameMain)
	{
		pDataHeader = g_pGameMain->FindResource("tc_spi");
		if (pDataHeader)
		{
			if (NULL == m_pMoneyImg[TC_SPI])
			{
				m_pMoneyImg[TC_SPI] = new CINFImage();
				m_pMoneyImg[TC_SPI]->InitDeviceObjects(pDataHeader->m_pData, pDataHeader->m_DataSize);
			}
		}
		pDataHeader = g_pGameMain->FindResource("tc_wp");
		if (pDataHeader)
		{
			if (NULL == m_pMoneyImg[TC_WP])
			{
				m_pMoneyImg[TC_WP] = new CINFImage();
				m_pMoneyImg[TC_WP]->InitDeviceObjects(pDataHeader->m_pData, pDataHeader->m_DataSize);
			}
		}
		pDataHeader = g_pGameMain->FindResource("tc_cr");
		if (pDataHeader)
		{
			if (NULL == m_pMoneyImg[TC_CREDITS])
			{
				m_pMoneyImg[TC_CREDITS] = new CINFImage();
				m_pMoneyImg[TC_CREDITS]->InitDeviceObjects(pDataHeader->m_pData, pDataHeader->m_DataSize);
			}
		}
		pDataHeader = g_pGameMain->FindResource("osr_selH");
		if (pDataHeader)
		{
			if (NULL == m_pItemSelectedRow)
			{
				m_pItemSelectedRow = new CINFImage();
				m_pItemSelectedRow->InitDeviceObjects(pDataHeader->m_pData, pDataHeader->m_DataSize);
			}
		}
		pDataHeader = g_pGameMain->FindResource("mnQup00");
		if (pDataHeader)
		{
			if (NULL == m_pSortArrowUpDn[0])
			{
				m_pSortArrowUpDn[0] = new CINFImage();
				m_pSortArrowUpDn[0]->InitDeviceObjects(pDataHeader->m_pData, pDataHeader->m_DataSize);
			}
		}
		pDataHeader = g_pGameMain->FindResource("mnQdn00");
		if (pDataHeader)
		{
			if (NULL == m_pSortArrowUpDn[1])
			{
				m_pSortArrowUpDn[1] = new CINFImage();
				m_pSortArrowUpDn[1]->InitDeviceObjects(pDataHeader->m_pData, pDataHeader->m_DataSize);
			}
		}


		{
			char szUpBtn[30], szDownBtn[30], szSelBtn[30], szDisBtn[30];
			wsprintf(szUpBtn, "Mallbtn_1");
			wsprintf(szDownBtn, "Mallbtn_2");
			wsprintf(szSelBtn, "Mallbtn_1");
			wsprintf(szDisBtn, "Mallbtn_2");
			if (NULL == m_pMaxBtn)
			{
				m_pMaxBtn = new CINFImageBtn;
			}
			m_pMaxBtn->InitDeviceObjects(szUpBtn, szDownBtn, szSelBtn, szDisBtn);

		}

		{
			char szUpBtn[30], szDownBtn[30], szSelBtn[30], szDisBtn[30];
			wsprintf(szUpBtn, "mnQup03");
			wsprintf(szDownBtn, "mnQup01");
			wsprintf(szSelBtn, "mnQup00");
			wsprintf(szDisBtn, "mnQup02");
			if (NULL == m_pUpCntBtn)
			{
				m_pUpCntBtn = new CINFImageBtn;
			}
			m_pUpCntBtn->InitDeviceObjects(szUpBtn, szDownBtn, szSelBtn, szDisBtn);
		}
		{
			char szUpBtn[30], szDownBtn[30], szSelBtn[30], szDisBtn[30];
			wsprintf(szUpBtn, "mnQdn03");
			wsprintf(szDownBtn, "mnQdn01");
			wsprintf(szSelBtn, "mnQdn00");
			wsprintf(szDisBtn, "mnQdn02");
			if (NULL == m_pDownCntBtn)
			{
				m_pDownCntBtn = new CINFImageBtn;
			}
			m_pDownCntBtn->InitDeviceObjects(szUpBtn, szDownBtn, szSelBtn, szDisBtn);
		}

		{
			char szUpBtn[30], szDownBtn[30], szSelBtn[30], szDisBtn[30];
			wsprintf(szUpBtn, "btn_regN");
			wsprintf(szDownBtn, "btn_regP");
			wsprintf(szSelBtn, "btn_regH");
			wsprintf(szDisBtn, "btn_regD");
			if (NULL == m_pRegisterItemBtn)
			{
				m_pRegisterItemBtn = new CINFImageBtn;
			}
			m_pRegisterItemBtn->InitDeviceObjects(szUpBtn, szDownBtn, szSelBtn, szDisBtn);
		}

		{
			if (NULL == m_pNumEditBox)
			{
				m_pNumEditBox = new CINFNumEditBox;
			}
			char chMaxMixCnt[64];

			wsprintf(chMaxMixCnt, "%d", MAX_ITEM_COUNTS);

			POINT ptPos = NUMEDITBOX_POSITION;
			m_pNumEditBox->InitDeviceObjects(9, ptPos, MONEYEDITBOX_WIDTH, TRUE, NUMEDITBOX_CAP);
			m_pNumEditBox->SetMaxStringLen(strlen(chMaxMixCnt));
			NumEditBoxChangeCount(m_pNumEditBox, 1);
		}

		{
			if (NULL == m_pMoneyEditBox)
			{
				m_pMoneyEditBox = new CINFNumEditBox;
			}
			char chMaxMixCnt[64];

			wsprintf(chMaxMixCnt, "%d", MAX_ITEM_COUNTS);

			POINT ptPos = MONEYEDITBOX_POSITION;
			m_pMoneyEditBox->InitDeviceObjects(9, ptPos, MONEYEDITBOX_WIDTH, TRUE, MONEYEDITBOX_CAP);
			m_pMoneyEditBox->SetMaxStringLen(strlen(chMaxMixCnt));
			NumEditBoxChangeCount(m_pMoneyEditBox, 1);
		}

		if (NULL == m_pMoneyComboBox)
		{
			m_pMoneyComboBox = new CINFListBox("cbarena", "cbarenab");

			m_pMoneyComboBox->SetUseCulling(TRUE); //글씨 컬링 사용
			m_pMoneyComboBox->InitDeviceObjects();

		}
	}
	char ImgName[30] = { 0, };
	for (i = 0; i < 3; ++i)
	{
		if (g_pGameMain)
		{
			wsprintf(ImgName, "oks0%d", i);
			pDataHeader = g_pGameMain->FindResource(ImgName);
		}
		if (pDataHeader)
		{
			if (NULL == m_pSellOKBtn[i])
			{
				m_pSellOKBtn[i] = new CINFImage();
				m_pSellOKBtn[i]->InitDeviceObjects(pDataHeader->m_pData, pDataHeader->m_DataSize);
			}
		}

		if (g_pGameMain)
		{
			wsprintf(ImgName, "apps0%d", i);
			pDataHeader = g_pGameMain->FindResource(ImgName);
		}
		if (pDataHeader)
		{
			if (NULL == m_pTimeLimitBtn[i])
			{
				m_pTimeLimitBtn[i] = new CINFImage();
				m_pTimeLimitBtn[i]->InitDeviceObjects(pDataHeader->m_pData, pDataHeader->m_DataSize);
			}
		}
	}
	if (m_pCloseXBtn == NULL)
	{
		m_pCloseXBtn = new CINFImageBtn;
		m_pCloseXBtn->InitDeviceObjects("xclose", "xclose", "xclose", "xclose");
	}

	return S_OK;
}

HRESULT CINFCityAuction::RestoreDeviceObjects()
{
	//CINFDefaultWnd::RestoreDeviceObjects();

	int nCnt = 0;
	for (nCnt = 0; nCnt < TAB_COUNT; nCnt++)
	{
		if (m_pBkImage[nCnt])
		{
			m_pBkImage[nCnt]->RestoreDeviceObjects();
			POINT ptSize = m_pBkImage[nCnt]->GetImgSize();
		//	SetSize(ptSize.x, ptSize.y);
		}
	}
	for (nCnt = 0; nCnt < TAB_COUNT; nCnt++)
	{
		if (m_pRankBtn[nCnt])
		{
			m_pRankBtn[nCnt]->RestoreDeviceObjects();
		}
	}
	/*for (nCnt = 0; nCnt < TAB_COUNT; nCnt++)
	{
		if (m_pRankBk[nCnt])
		{
			m_pRankBk[nCnt]->RestoreDeviceObjects();
		}
	}*/
	{
		if (m_pFontRankInfo)
		{
			m_pFontRankInfo->RestoreDeviceObjects();
		}

		int nX, nY;
		nX = nY = 0;
		for (nY = 0; nY < MAX_PER_ONEPAGE; nY++)
		{
			for (nX = 0; nX < MAX_WR_INFO_X; nX++)
			{
				if (m_pFontTable[nY][nX])
				{
					m_pFontTable[nY][nX]->RestoreDeviceObjects();
				}
			}
		}
		for (nCnt = 0; nCnt < MAX_PAGES_COUNT; nCnt++)
		{
			if (m_pFontPage[nCnt])
			{
				m_pFontPage[nCnt]->RestoreDeviceObjects();
			}
		}
	}
	if (m_pComboGear)
	{
		m_pComboGear->RestoreDeviceObjects();

		m_pComboGear->ItemClear();
		m_pComboGear->AddElement(STRCMD_CS_UNITKIND_GEAR_ALL);	// #define		WORLDRANK_GEAR_ALL		0		// 전체 기어
		m_pComboGear->AddElement(STRCMD_CS_UNITKIND_BGEAR);		// #define		WORLDRANK_GEAR_B		1		// B
		m_pComboGear->AddElement(STRCMD_CS_UNITKIND_MGEAR);		// #define		WORLDRANK_GEAR_M		2		// M
		m_pComboGear->AddElement(STRCMD_CS_UNITKIND_IGEAR);		// #define		WORLDRANK_GEAR_I		3		// I
		m_pComboGear->AddElement(STRCMD_CS_UNITKIND_AGEAR);		// #define		WORLDRANK_GEAR_A		4		// A
		m_pComboGear->SetSelectItem(0);
	}
	if (m_pMoneyComboBox)
	{
		m_pMoneyComboBox->ItemClear();
		m_pMoneyComboBox->RestoreDeviceObjects();
		m_pMoneyComboBox->ShowItem(FALSE);
		m_pMoneyComboBox->AddElement("SPI");
		m_pMoneyComboBox->AddElement("WP");
		m_pMoneyComboBox->AddElement("CREDITS");
		m_pMoneyComboBox->SetSelectItem(0);
	}
	if (m_pComboCurrency)
	{
		m_pComboCurrency->RestoreDeviceObjects();

		m_pComboCurrency->ItemClear();		
		m_pComboCurrency->AddElement("SPI");
		m_pComboCurrency->AddElement("WP");
		m_pComboCurrency->AddElement("Credits");
		m_pComboCurrency->AddElement("All");
		
		m_pComboCurrency->SetSelectItem(3);
	}
	if (m_pListTreeCtrl)
	{
		m_pListTreeCtrl->RestoreDeviceObjects();
		m_pListTreeCtrl->SetListCtrlPos(_INET_OSR_MARKET_POS_X + CATEGORY_LOCATION_X, _INET_OSR_MARKET_POS_Y + CATEGORY_LOCATION_Y,
			CATEGORY_SUBITEM_LOCATION_X, CATEGORY_SUBITEM_LOCATION_Y,
			CATEGORY_SCROLL_LOCATION_X, CATEGORY_SCROLL_LOCATION_Y,
			CATEGORY_WIDTH, CATEGORY_HEIGHT);
		LoadListItem();
		m_pListTreeCtrl->RestoreItemDeviceObjects();
		m_pListTreeCtrl->UpdateItemPos();
		m_pListTreeCtrl->SetSelPoint(NULL);
	}
	for (int i = 0; i < 2; ++i)
	{
		if (m_plevelNumEditBox[i])
		{
			m_plevelNumEditBox[i]->RestoreDeviceObjects();
			m_plevelNumEditBox[0]->SetString("1", 32);
			m_plevelNumEditBox[1]->SetString("123", 32);
			m_plevelNumEditBox[i]->EnableEdit(FALSE);
		}

		if (m_pEnchantNumEditBox[i])
		{
			m_pEnchantNumEditBox[i]->RestoreDeviceObjects();
			m_pEnchantNumEditBox[0]->SetString("0", 32);
			m_pEnchantNumEditBox[1]->SetString("40", 32);
			m_pEnchantNumEditBox[i]->EnableEdit(FALSE);
		}
	}

	if (m_pNameEditBox)
	{
		char chBlank[16];
		memset(chBlank, 0x00, 16);

		m_pNameEditBox->SetString(chBlank, 16);
		m_pNameEditBox->RestoreDeviceObjects();
		m_pNameEditBox->EnableEdit(FALSE, FALSE);
	}
	POINT ptBkPos = m_pBkImage[TAB_BUY_ITEM]->GetImgSize();
	UpdateBtnPos(ptBkPos.x, ptBkPos.y);

#ifdef _DEBUG
	//TestDB();
#endif
	if (m_pCategoryPathArrowImage)
		m_pCategoryPathArrowImage->RestoreDeviceObjects();
	
	if (m_pItemSelectedRow)
		m_pItemSelectedRow->RestoreDeviceObjects();
	
	if (m_pSortArrowUpDn[0])
		m_pSortArrowUpDn[0]->RestoreDeviceObjects();
	if (m_pSortArrowUpDn[1])
		m_pSortArrowUpDn[1]->RestoreDeviceObjects();

	if (m_pBtnHover[0])
		m_pBtnHover[0]->RestoreDeviceObjects();
	if (m_pBtnBUY)
		m_pBtnBUY->RestoreDeviceObjects();
	if (m_pBtnHover[1])
		m_pBtnHover[1]->RestoreDeviceObjects();

	if (m_pFontCategoryPath)
		m_pFontCategoryPath->RestoreDeviceObjects();

	m_nSelectedCategoryId = 0;
	for (int i = 0; i < 3; ++i)
	{
		if (m_pMoneyImg[i])
		{
			m_pMoneyImg[i]->RestoreDeviceObjects();
		}
	}

	m_pIconInfo = g_pGameMain->m_pIcon;
	m_nSelectListItem = -1;
	m_nSelectMyListItem = -1;

	g_pGameMain->m_pInven->ShowInven(NULL, NULL);
	g_pGameMain->m_pInven->SetTradeItemCenterState(FALSE);

	if (m_pMaxBtn)
		m_pMaxBtn->RestoreDeviceObjects();

	if (m_pNumEditBox)
	{
		m_pNumEditBox->RestoreDeviceObjects();
		NumEditBoxChangeCount(m_pNumEditBox, 1);
		m_pNumEditBox->EnableEdit(FALSE);
	}
	if (m_pMoneyEditBox)
	{
		m_pMoneyEditBox->RestoreDeviceObjects();
		NumEditBoxChangeCount(m_pMoneyEditBox, 1);
		m_pMoneyEditBox->EnableEdit(FALSE);
	}

	if (m_pUpCntBtn)
		m_pUpCntBtn->RestoreDeviceObjects();
	if (m_pDownCntBtn)
		m_pDownCntBtn->RestoreDeviceObjects();
	
	if (m_pRegisterItemBtn)
		m_pRegisterItemBtn->RestoreDeviceObjects();

	m_byMoneySelect = TC_SPI;

	for (int i = 0; i < 3; ++i)
	{
		if (m_pSellOKBtn[i])
			m_pSellOKBtn[i]->RestoreDeviceObjects();
		if (m_pTimeLimitBtn[i])
			m_pTimeLimitBtn[i]->RestoreDeviceObjects();
	}
	if (m_pCloseXBtn)
		m_pCloseXBtn->RestoreDeviceObjects();

	return S_OK;
}

HRESULT CINFCityAuction::DeleteDeviceObjects()
{
	//CINFDefaultWnd::DeleteDeviceObjects();
	int nCnt = 0;
	for (nCnt = 0; nCnt < TAB_COUNT; nCnt++)
	{
		if (m_pBkImage[nCnt])
		{
			m_pBkImage[nCnt]->DeleteDeviceObjects();
			SAFE_DELETE(m_pBkImage[nCnt]);
		}
	}
	for (nCnt = 0; nCnt < TAB_COUNT; nCnt++)
	{
		if (m_pRankBtn[nCnt])
		{
			m_pRankBtn[nCnt]->DeleteDeviceObjects();
			SAFE_DELETE(m_pRankBtn[nCnt]);
		}
	}
	/*for (nCnt = 0; nCnt < TAB_COUNT; nCnt++)
	{
		if (m_pRankBk[nCnt])
		{
			m_pRankBk[nCnt]->DeleteDeviceObjects();
			SAFE_DELETE(m_pRankBk[nCnt]);
		}
	}*/
	{
		if (m_pFontRankInfo)
		{
			m_pFontRankInfo->DeleteDeviceObjects();
			SAFE_DELETE(m_pFontRankInfo);
		}

		int nX, nY;
		nX = nY = 0;
		for (nY = 0; nY < MAX_PER_ONEPAGE; nY++)
		{
			for (nX = 0; nX < MAX_WR_INFO_X; nX++)
			{
				if (m_pFontTable[nY][nX])
				{
					m_pFontTable[nY][nX]->DeleteDeviceObjects();
					SAFE_DELETE(m_pFontTable[nY][nX]);
				}
			}
		}
		for (nCnt = 0; nCnt < MAX_PAGES_COUNT; nCnt++)
		{
			if (m_pFontPage[nCnt])
			{
				m_pFontPage[nCnt]->DeleteDeviceObjects();
				SAFE_DELETE(m_pFontPage[nCnt]);
			}
		}
	}
	if (m_pComboGear)
	{
		m_pComboGear->DeleteDeviceObjects();
		SAFE_DELETE(m_pComboGear);
	}
	if (m_pComboCurrency)
	{
		m_pComboCurrency->DeleteDeviceObjects();
		SAFE_DELETE(m_pComboCurrency);
	}
	if (m_pListTreeCtrl)
	{
		m_pListTreeCtrl->DeleteDeviceObjects();
		SAFE_DELETE(m_pListTreeCtrl);
	}
	if (m_pCategoryPathArrowImage)
	{
		m_pCategoryPathArrowImage->DeleteDeviceObjects();
		SAFE_DELETE(m_pCategoryPathArrowImage);
	}
	if (m_pItemSelectedRow)
	{
		m_pItemSelectedRow->DeleteDeviceObjects();
		SAFE_DELETE(m_pItemSelectedRow);
	}
	if (m_pSortArrowUpDn[0])
	{
		m_pSortArrowUpDn[0]->DeleteDeviceObjects();
		SAFE_DELETE(m_pSortArrowUpDn[0]);
	}
	if (m_pSortArrowUpDn[1])
	{
		m_pSortArrowUpDn[1]->DeleteDeviceObjects();
		SAFE_DELETE(m_pSortArrowUpDn[1]);
	}

	if (m_pBtnHover[0])
	{
		m_pBtnHover[0]->DeleteDeviceObjects();
		SAFE_DELETE(m_pBtnHover[0]);
	}

	for (int i = 0; i < 3; ++i)
	{
		if (m_pMoneyImg[i])
		{
			m_pMoneyImg[i]->DeleteDeviceObjects();
			SAFE_DELETE(m_pMoneyImg[i]);
		}
	}

	if (m_pBtnBUY)
	{
		m_pBtnBUY->DeleteDeviceObjects();
		SAFE_DELETE(m_pBtnBUY);
	}
	if (m_pRegisterItemBtn)
	{
		m_pRegisterItemBtn->DeleteDeviceObjects();
		SAFE_DELETE(m_pRegisterItemBtn);
	}
	if (m_pBtnHover[1])
	{
		m_pBtnHover[1]->DeleteDeviceObjects();
		SAFE_DELETE(m_pBtnHover[1]);
	}

	if (m_pFontCategoryPath)
	{
		m_pFontCategoryPath->DeleteDeviceObjects();
		SAFE_DELETE(m_pFontCategoryPath);
	}

	int i;
	for (i = 0; i < 2; ++i)
	{
		if (m_plevelNumEditBox[i])
		{
			m_plevelNumEditBox[i]->DeleteDeviceObjects();
			SAFE_DELETE(m_plevelNumEditBox[i]);
		}
		if (m_pEnchantNumEditBox[i])
		{
			m_pEnchantNumEditBox[i]->DeleteDeviceObjects();
			SAFE_DELETE(m_pEnchantNumEditBox[i]);

		}
	}
	
	if (m_pNameEditBox)
	{
		m_pNameEditBox->DeleteDeviceObjects();
		SAFE_DELETE(m_pNameEditBox);
	}
	if (m_pNumEditBox)
	{
		m_pNameEditBox->DeleteDeviceObjects();
		SAFE_DELETE(m_pNumEditBox);
	}
	if (m_pMoneyEditBox)
	{
		m_pMoneyEditBox->DeleteDeviceObjects();
		SAFE_DELETE(m_pMoneyEditBox);
	}
	if (m_pUpCntBtn)
	{
		m_pUpCntBtn->DeleteDeviceObjects();
		SAFE_DELETE(m_pUpCntBtn);
	}
	if (m_pDownCntBtn)
	{
		m_pDownCntBtn->DeleteDeviceObjects();
		SAFE_DELETE(m_pDownCntBtn);
	}
	if (m_pMaxBtn)
	{
		m_pMaxBtn->DeleteDeviceObjects();
		SAFE_DELETE(m_pMaxBtn);
	}
	if (m_pMoneyComboBox)
	{
		m_pMoneyComboBox->DeleteDeviceObjects();
		SAFE_DELETE(m_pMoneyComboBox);
	}
	for (i = 0; i < 3; ++i)
	{
		if (m_pSellOKBtn[i])
		{
			m_pSellOKBtn[i]->DeleteDeviceObjects();
			SAFE_DELETE(m_pSellOKBtn[i]);
		}

		if (m_pTimeLimitBtn[i])
		{
			m_pTimeLimitBtn[i]->DeleteDeviceObjects();
			SAFE_DELETE(m_pTimeLimitBtn[i]);
		}
	}
	InitData();
	if (m_pCloseXBtn)
	{
		m_pCloseXBtn->DeleteDeviceObjects();
		SAFE_DELETE(m_pCloseXBtn);
	}
	return S_OK;
}

HRESULT CINFCityAuction::InvalidateDeviceObjects()
{
//	CINFDefaultWnd::InvalidateDeviceObjects();
	int nCnt = 0;
	for (nCnt = 0; nCnt < TAB_COUNT; nCnt++)
	{
		if (m_pBkImage[nCnt])
		{
			m_pBkImage[nCnt]->InvalidateDeviceObjects();
		}
	}
	for (nCnt = 0; nCnt < TAB_COUNT; nCnt++)
	{
		if (m_pRankBtn[nCnt])
		{
			m_pRankBtn[nCnt]->InvalidateDeviceObjects();
		}
	}
	/*for (nCnt = 0; nCnt < TAB_COUNT; nCnt++)
	{
		if (m_pRankBk[nCnt])
		{
			m_pRankBk[nCnt]->InvalidateDeviceObjects();
		}
	}*/
	{
		if (m_pFontRankInfo)
		{
			m_pFontRankInfo->InvalidateDeviceObjects();
		}

		int nX, nY;
		nX = nY = 0;
		for (nY = 0; nY < MAX_PER_ONEPAGE; nY++)
		{
			for (nX = 0; nX < MAX_WR_INFO_X; nX++)
			{
				if (m_pFontTable[nY][nX])
				{
					m_pFontTable[nY][nX]->InvalidateDeviceObjects();
				}
			}
		}
		for (nCnt = 0; nCnt < MAX_PAGES_COUNT; nCnt++)
		{
			if (m_pFontPage[nCnt])
			{
				m_pFontPage[nCnt]->InvalidateDeviceObjects();
			}
		}
	}
	if (m_pComboGear)
	{
		m_pComboGear->InvalidateDeviceObjects();
	}
	if (m_pComboCurrency)
	{
		m_pComboCurrency->InvalidateDeviceObjects();
	}

	if (m_pListTreeCtrl)
		m_pListTreeCtrl->InvalidateDeviceObjects();

	if (m_pCategoryPathArrowImage)
		m_pCategoryPathArrowImage->InvalidateDeviceObjects();

	if (m_pItemSelectedRow)
		m_pItemSelectedRow->InvalidateDeviceObjects();
	
	if (m_pSortArrowUpDn[0])
		m_pSortArrowUpDn[0]->InvalidateDeviceObjects();
	if (m_pSortArrowUpDn[1])
		m_pSortArrowUpDn[1]->InvalidateDeviceObjects();

	if (m_pBtnHover[0])
		m_pBtnHover[0]->InvalidateDeviceObjects();
	if (m_pBtnHover[1])
		m_pBtnHover[1]->InvalidateDeviceObjects();
	if (m_pBtnBUY)
		m_pBtnBUY->InvalidateDeviceObjects();

	if (m_pFontCategoryPath)
		m_pFontCategoryPath->InvalidateDeviceObjects();

	int i;
	for (i = 0; i < 2; ++i)
	{
		if (m_plevelNumEditBox[i])
		{
			m_plevelNumEditBox[i]->InvalidateDeviceObjects();
		}
		if (m_pEnchantNumEditBox[i])
		{
			m_pEnchantNumEditBox[i]->InvalidateDeviceObjects();
		}
	}

	if (m_pNameEditBox)
		m_pNameEditBox->InvalidateDeviceObjects();

	for (int i = 0; i < 3; ++i)
	{
		if (m_pMoneyImg[i])
		{
			m_pMoneyImg[i]->InvalidateDeviceObjects();
		}
	}
	if (m_pMaxBtn)
		m_pMaxBtn->InvalidateDeviceObjects();

	if (m_pNumEditBox)
		m_pNumEditBox->InvalidateDeviceObjects();

	if (m_pMoneyEditBox)
		m_pMoneyEditBox->InvalidateDeviceObjects();

	if (m_pUpCntBtn)
		m_pUpCntBtn->InvalidateDeviceObjects();

	if (m_pDownCntBtn)
		m_pDownCntBtn->InvalidateDeviceObjects();
	
	if (m_pRegisterItemBtn)
		m_pRegisterItemBtn->InvalidateDeviceObjects();

	if (m_pMoneyComboBox)
		m_pMoneyComboBox->InvalidateDeviceObjects();

	for (i = 0; i < 3; ++i)
	{
		if (m_pSellOKBtn[i])
			m_pSellOKBtn[i]->InvalidateDeviceObjects();

		if (m_pTimeLimitBtn[i])
			m_pTimeLimitBtn[i]->InvalidateDeviceObjects();
	}
	if (m_pCloseXBtn)
		m_pCloseXBtn->InvalidateDeviceObjects();

	return S_OK;
}

void	CINFCityAuction::Render()
{


	if (m_pBkImage[m_nCurrentTab])
	{
		m_pBkImage[m_nCurrentTab]->Move(_INET_OSR_MARKET_POS_X, _INET_OSR_MARKET_POS_Y);
		m_pBkImage[m_nCurrentTab]->Render();
	}
	int nCnt = 0;
	for (nCnt = 0; nCnt < TAB_COUNT; nCnt++)
	{
		if (m_pRankBtn[nCnt])
		{
			m_pRankBtn[nCnt]->Render();
		}
	}

	/*if (m_pRankBk[m_nCurrentTab])
	{
		m_pRankBk[m_nCurrentTab]->Move(_INET_OSR_MARKET_POS_X + 44, _INET_OSR_MARKET_POS_Y + 157);
		m_pRankBk[m_nCurrentTab]->Render();
	}*/

	RenderMyMoney();	// 랭킹 정보 표시

	if (m_nCurerntInfo)
	{
		g_pGameMain->SetItemInfoUser(m_nCurerntInfo, ptMouse.x, ptMouse.y);
	}
	else
	{
		g_pGameMain->SetItemInfo(0, 0, 0, 0);
	}

	if (m_nCurrentTab == TAB_BUY_ITEM) {
		if (m_pListTreeCtrl)
			m_pListTreeCtrl->Render();

		RenderCategoryPath("not used from here");
		RenderSelectPage();
		m_nSelectedCategoryId = m_pListTreeCtrl->FindNodeIdBySelectedItem();
		if (m_pComboGear)
		{
			m_pComboGear->Render();
		}
		if (m_pComboCurrency)
		{
			m_pComboCurrency->Render();
		}

		m_pBtnHover[0]->SetBtnPosition(_INET_OSR_MARKET_POS_X + 578, _INET_OSR_MARKET_POS_Y + 130);
		m_pBtnHover[0]->Render();


		m_pBtnHover[1]->SetBtnPosition(_INET_OSR_MARKET_POS_X + 720, _INET_OSR_MARKET_POS_Y + 162);
		m_pBtnHover[1]->Render();

		m_pBtnBUY->SetBtnPosition(_INET_OSR_MARKET_POS_X + 754, _INET_OSR_MARKET_POS_Y + 162);
		m_pBtnBUY->Render();

		for (int i = 0; i < 2; ++i)
		{
			if (m_plevelNumEditBox[i])
			{
				m_plevelNumEditBox[i]->Render();
			}
			if (m_pEnchantNumEditBox[i])
			{
				m_pEnchantNumEditBox[i]->Render();
			}
		}

		if (m_pNameEditBox)
			m_pNameEditBox->Render();

		ListRender();
		//render sort arrows depend on current sort mode
		/*#define MARKET_SORT_PRICE_UP				1			// 가격 오름차순
#define MARKET_SORT_PRICE_DOWN				2			// 가격 내림차순
#define MARKET_SORT_LEVEL_UP				3			// 레벨 오름차순
#define MARKET_SORT_LEVEL_DOWN				4			// 레벨 내림차순
#define MARKET_SORT_ENCHANT_UP				5			// 인첸트 오름차순
#define MARKET_SORT_ENCHANT_DOWN			6			// 인첸트 내림차순
#define MARKET_SORT_NAME_UP					7			// 이름 오름차순
#define MARKET_SORT_NAME_DOWN				8			// 이름 내림차순
#define MARKET_SORT_TIME_UP					9			// 시간 오름차순
#define MARKET_SORT_TIME_DOWN				10			// 시간 내림차순*/
		bool bRenderUp = false;
		bool bRenderDn = false;

		for (int a = 0; a < 2; a++)
		{
			switch (m_nCurrentSortingType) {
			case 5:
			case 6:
				m_pSortArrowUpDn[a]->Move(_INET_OSR_MARKET_POS_X + ITEMLIST_LOCATION_X + 16 + 43 + 30, _INET_OSR_MARKET_POS_Y + ITEMLIST_LOCATION_Y - 8);
				break;
			case 7:
			case 8:
				m_pSortArrowUpDn[a]->Move(_INET_OSR_MARKET_POS_X + ITEMLIST_LOCATION_X + 16 + 43 + 43 + 140, _INET_OSR_MARKET_POS_Y + ITEMLIST_LOCATION_Y - 15);
				break;
			case 3:
			case 4:
				m_pSortArrowUpDn[a]->Move(_INET_OSR_MARKET_POS_X + ITEMLIST_LOCATION_X + 16 + 43 + 43 + 140 + 120, _INET_OSR_MARKET_POS_Y + ITEMLIST_LOCATION_Y - 8);
				break;
			case 9:
			case 10:
				m_pSortArrowUpDn[a]->Move(_INET_OSR_MARKET_POS_X + ITEMLIST_LOCATION_X + 16 + 43 + 43 + 140 + 120 + 43, _INET_OSR_MARKET_POS_Y + ITEMLIST_LOCATION_Y - 8);
				break;
			case 1:
			case 2:
				m_pSortArrowUpDn[a]->Move(_INET_OSR_MARKET_POS_X + ITEMLIST_LOCATION_X + 16 + 43 + 43 + 140 + 140 + 43 + 90, _INET_OSR_MARKET_POS_Y + ITEMLIST_LOCATION_Y - 15);
				break;
			}
		}

		switch (m_nCurrentSortingType) {
		case 0:
			bRenderUp = false;
			bRenderDn = false;
			//none
			break;
		case 5:
			bRenderUp = true;
			break;
		case 7:
			bRenderUp = true;
			break;
		case 3:
			bRenderUp = true;
			break;
		case 9:
			bRenderUp = true;
			break;
		case 1:
			bRenderUp = true;
			break;

		case 6:
			bRenderDn = true;
			break;
		case 8:
			bRenderDn = true;
			break;
		case 4:
			bRenderDn = true;
			break;
		case 10:
			bRenderDn = true;
			break;
		case 2:
			bRenderDn = true;
			break;
		}

		if (bRenderUp)
			m_pSortArrowUpDn[0]->Render();
		if (bRenderDn)
			m_pSortArrowUpDn[1]->Render();

	}

	if (m_nCurrentTab == TAB_SELL_ITEM)
	{

		int nPosX = 0;
		int nPosY = 0;
		POINT m_ptStartPos;
		m_ptStartPos.x = _INET_OSR_MARKET_POS_X;
		m_ptStartPos.y = _INET_OSR_MARKET_POS_Y;

		if (m_pMoneyComboBox)
		{
			// 장치
			int nMainWidth, nMainHeight;
			int nEleWidth, nEleHeight;
			int nElePosX, nElePosY;
			nElePosX = nElePosY = 0;

			nMainWidth = MONEY_COMBO_MAIN_WIDTH;
			nMainHeight = MONEY_COMBO_MAIN_HEIGHT;
			nEleWidth = MONEY_COMBO_ELE_WIDTH;
			nEleHeight = MONEY_COMBO_ELE_HEIGHT;

			nPosX = m_ptStartPos.x + MONEY_COMBO_LOCATION_X;
			nPosY = m_ptStartPos.y + MONEY_COMBO_LOCATION_Y;

			nElePosX = nPosX;
			nElePosY = nPosY + nMainHeight;

			m_pMoneyComboBox->SetMainArea(nPosX, nPosY, nMainWidth, nMainHeight);
			m_pMoneyComboBox->SetElementArea(nElePosX, nElePosY + 2, nEleWidth, nEleHeight);
			m_pMoneyComboBox->SetBGPos(nElePosX + 6, nElePosY + 4, nEleWidth, nEleHeight);
		}

		//if (m_pAddBtn)
			//m_pAddBtn->SetBtnPosition(m_ptStartPos.x + ADDBTN_LOCATION_X, m_ptStartPos.y + ADDBTN_LOCATION_Y);

		if (m_pMaxBtn)
			m_pMaxBtn->SetBtnPosition(m_ptStartPos.x + MAXBTN_LOCATION_X, m_ptStartPos.y + MAXBTN_LOCATION_Y);

		if (m_pNumEditBox)
			m_pNumEditBox->SetPos(m_ptStartPos.x + NUMEDITBOX_LOCATION_X, m_ptStartPos.y + 295);

		if (m_pMoneyEditBox)
			m_pMoneyEditBox->SetPos(m_ptStartPos.x + MONEYEDITBOX_LOCATION_X, m_ptStartPos.y + MONEYEDITBOX_LOCATION_Y);

		for (int i = 0; i < 3; ++i)
		{
			if (m_pMoneyImg[i])
			{
				m_pMoneyImg[i]->Move(m_ptStartPos.x + MONEY_IMAGE_LOCATION_X, m_ptStartPos.y + MONEY_IMAGE_LOCATION_Y);
			}
		}

		if (m_pUpCntBtn)
			m_pUpCntBtn->SetBtnPosition(m_ptStartPos.x + UPCNTBTN_LOCATION_X, m_ptStartPos.y + UPCNTBTN_LOCATION_Y);

		if (m_pDownCntBtn)
			m_pDownCntBtn->SetBtnPosition(m_ptStartPos.x + DOWNCNTBTN_LOCATION_X, m_ptStartPos.y + DOWNCNTBTN_LOCATION_Y);
		
		if (m_pRegisterItemBtn)
			m_pRegisterItemBtn->SetBtnPosition(m_ptStartPos.x + 617, m_ptStartPos.y + 578);

		if (m_pMaxBtn)
			m_pMaxBtn->Render();

		if (m_pNumEditBox)
			m_pNumEditBox->Render();

		if (m_pMoneyEditBox)
			m_pMoneyEditBox->Render();

		if (m_pMoneyComboBox)
			m_pMoneyComboBox->Render();

		if (m_pUpCntBtn)
			m_pUpCntBtn->Render();
		if (m_pDownCntBtn)
			m_pDownCntBtn->Render();
		if (m_pRegisterItemBtn)
			m_pRegisterItemBtn->Render();

		m_pMoneyImg[m_byMoneySelect]->Render();

		if (m_pSourceItem && m_pIconInfo)
		{
			//char chIconName[64] = { 0, };
			long nItemNum = 0;
			if (!m_pSourceItem->ShapeItemNum)
				nItemNum= m_pSourceItem->ItemInfo->SourceIndex;
			else
			{
				ITEM* pShapeItem = g_pDatabase->GetServerItemInfo(m_pSourceItem->ShapeItemNum);
				if (pShapeItem)
					nItemNum= pShapeItem->SourceIndex;
				else
					nItemNum= m_pSourceItem->ItemInfo->SourceIndex;
			}
			/*if (pt.x > _INET_OSR_MARKET_POS_X + 697 && pt.x < _INET_OSR_MARKET_POS_X + 28 + 697 &&
				pt.y > _INET_OSR_MARKET_POS_Y + 184 && pt.y < _INET_OSR_MARKET_POS_Y + 28 + 184)*/
			
			m_pIconInfo->Render(nItemNum, _INET_OSR_MARKET_POS_X + 691, _INET_OSR_MARKET_POS_Y + 185);

			char szItemName[140];
			GetItemName(m_pSourceItem, szItemName);
			//((CINFTradeItemCenter*)m_pParent)->ChangeMaxLenString(m_pItemName, 120);
			m_pFontTable[1][0]->DrawText(_INET_OSR_MARKET_POS_X + 612, _INET_OSR_MARKET_POS_Y + 224, GUI_FONT_COLOR_W, szItemName);
		}
	}
	

	if (m_nCurrentTab == TAB_FINISHED_ITEM)
	{
		m_pBtnHover[1]->SetBtnPosition(_INET_OSR_MARKET_POS_X + 767, _INET_OSR_MARKET_POS_Y + 133);
		m_pBtnHover[1]->Render();

		MyListRender();
	}

	m_pCloseXBtn->SetBtnPosition(_INET_OSR_MARKET_POS_X + _INET_OSR_MARKET_SIZE_X - 30, _INET_OSR_MARKET_POS_Y + 32);
	m_pCloseXBtn->Render();
}
void CINFCityAuction::MyItemVecAdd(MSG_FC_MARKET_MY_LIST_OK* pMsg)
{
	MARKET_MY_ITEM_INFO* pTemp = new MARKET_MY_ITEM_INFO;
	memset(pTemp, 0x00, sizeof(MARKET_MY_ITEM_INFO));

	pTemp->pINFO = new MSG_FC_MARKET_MY_LIST_OK;
	ITEM_GENERAL* pITEMG = new ITEM_GENERAL;

	memset(pITEMG, 0x00, sizeof(ITEM_GENERAL));
	memset(pTemp->pINFO, 0x00, sizeof(MSG_FC_MARKET_MY_LIST_OK));

	memcpy(pTemp->pINFO, pMsg, sizeof(MSG_FC_MARKET_MY_LIST_OK));

	pITEMG->PrefixCodeNum = pMsg->MarketInfo.PrefixCodeNum;
	pITEMG->SuffixCodeNum = pMsg->MarketInfo.SuffixCodeNum;
	pITEMG->UniqueNumber = pMsg->MarketInfo.ItemUID;
	pITEMG->ShapeItemNum = pMsg->MarketInfo.ShapeItemNum;

	pITEMG->UniqueNumber = pMsg->MarketInfo.ItemUID;
	pITEMG->ItemNum = pMsg->MarketInfo.ItemNum;
	pITEMG->CurrentCount = pMsg->MarketInfo.ItemCount;

	pITEMG->PrefixCodeNum = pMsg->MarketInfo.PrefixCodeNum;
	pITEMG->SuffixCodeNum = pMsg->MarketInfo.SuffixCodeNum;
	pITEMG->ShapeItemNum = pMsg->MarketInfo.ShapeItemNum;
	pITEMG->ColorCode = pMsg->MarketInfo.ColorCode;

	pTemp->pItem = new CItemInfo(pITEMG);

	for (int i = 0; i < 8; ++i)
	{
		for (int j = 0; j < pMsg->MarketInfo.Enchant[i].Count; ++j)
		{
			pTemp->pItem->AddEnchantItem(pMsg->MarketInfo.Enchant[i].ItemNum);
		}
	}
	pTemp->pItem->Kind = pTemp->pItem->ItemInfo->Kind;
	m_vecMyItemInfo.push_back(pTemp);
}
void	CINFCityAuction::RenderMyMoney()
{
	POINT ptBkPos;
	
	ptBkPos.x = _INET_OSR_MARKET_POS_X;
	ptBkPos.y = _INET_OSR_MARKET_POS_Y;//= GetBkPos();

	int nCount = g_pGameMain->m_pInven->GetItemSpi();
	char chTmp1[256], chTmp2[256];
	ZERO_MEMORY(chTmp1);
	ZERO_MEMORY(chTmp2);
	wsprintf(chTmp1, "%d", nCount);
	MakeCurrencySeparator(chTmp2, chTmp1, 3, ',');
	SIZE szRankSize = m_pFontRankInfo->GetStringSize(chTmp2);
	m_pFontRankInfo->DrawText(ptBkPos.x + 146 - szRankSize.cx / 2,
				ptBkPos.y + 131,
				GUI_FONT_COLOR_W,
				chTmp2);

	nCount = g_pShuttleChild->m_myShuttleInfo.WarPoint;
	ZERO_MEMORY(chTmp1);
	ZERO_MEMORY(chTmp2);
	wsprintf(chTmp1, "%d", nCount);
	MakeCurrencySeparator(chTmp2, chTmp1, 3, ',');
	szRankSize = m_pFontRankInfo->GetStringSize(chTmp2);
	m_pFontRankInfo->DrawText(ptBkPos.x + 146 - szRankSize.cx / 2,
		ptBkPos.y + 148,
		GUI_FONT_COLOR_W,
		chTmp2);

	nCount = g_pD3dApp->m_nCashPoints;
	ZERO_MEMORY(chTmp1);
	ZERO_MEMORY(chTmp2);
	wsprintf(chTmp1, "%d", nCount);
	MakeCurrencySeparator(chTmp2, chTmp1, 3, ',');
	szRankSize = m_pFontRankInfo->GetStringSize(chTmp2);
	m_pFontRankInfo->DrawText(ptBkPos.x + 146 - szRankSize.cx / 2,
		ptBkPos.y + 165,
		GUI_FONT_COLOR_W,
		chTmp2);
}

void	CINFCityAuction::Tick()
{
	for (int i = 0; i < 2; ++i)
	{
		if (m_plevelNumEditBox[i])
		{
			m_plevelNumEditBox[i]->Tick();
		}
		if (m_pEnchantNumEditBox[i])
		{
			m_pEnchantNumEditBox[i]->Tick();
		}
	}
	if (m_pNameEditBox)
		m_pNameEditBox->Tick();
	/*if (!IsShowWnd())
	{
		return;
	}
	CINFDefaultWnd::Tick();*/

	if (nTickWaitRefresh >= 1)
	{
		--nTickWaitRefresh;
	}
	if (nTickWaitRefreshMY >= 1)
	{
		--nTickWaitRefreshMY;
	}

	if (nTickWaitSortClick >= 0)
	{
		--nTickWaitSortClick;
	}

	if(nTickWaitRefresh == 0)
	{
		nTickWaitRefresh = -1;
		m_nSelectMyListItem = -1;
		m_nSelectListItem = -1;
		bBoughtItem = true;
		g_pFieldWinSocket->SendMsg(T_FC_MARKET_BASE_INFO_REQUEST, NULL, 0);
	}

	if(nTickWaitRefreshMY == 0)
	{
		nTickWaitRefreshMY = -1;
		nMylistStartFromId = 0;

		g_pFieldWinSocket->SendMsg(T_FC_MARKET_MY_LIST_REQUEST, NULL, 0);
	}

	if (m_pNumEditBox)
		m_pNumEditBox->Tick();

	if (m_pMoneyEditBox)
		m_pMoneyEditBox->Tick();
}
void CINFCityAuction::LoadListItem()
{
	// 2013-12-03 by ymjoo 거래소 카테고리 목록 확장
	//m_pListCtrl->ResetContent();
	m_pListTreeCtrl->ResetContent();

	if (NULL == m_pItemCategorytree)
	{
		TRADEITEMCATEGORYTREE TempcaTree[] =
		{
			{000, MARKET_ITEM_KIND_ALL,					STRMSG_C_131205_0001},		//전체

			{100, MARKET_ITEM_KIND_WEAPON,				STRMSG_C_131205_0002},		//	무기

			{110, MARKET_ITEM_KIND_PRIMARY_WEAPON,		STRMSG_C_131205_0003},		//		기본무기
			{111, MARKET_ITEM_KIND_VULCAN,				STRMSG_C_131205_0004},		//			발칸
			{112, MARKET_ITEM_KIND_CANNON,				STRMSG_C_131205_0005},		//			캐논
			{113, MARKET_ITEM_KIND_GATLING,				STRMSG_C_131205_0006},		//			개틀링
			{114, MARKET_ITEM_KIND_RIFLE,				STRMSG_C_131205_0007},		//			라이플
			{115, MARKET_ITEM_KIND_AUTOMATIC,			STRMSG_C_131205_0008},		//			오토매틱
			{116, MARKET_ITEM_KIND_DUALIST,				STRMSG_C_131205_0009},		//			듀얼리스트
			{117, MARKET_ITEM_KIND_MASSDRIVE,			"Mass Drive"},		//	STRMSG_C_131205_0010		메스드라이브

			{120, MARKET_ITEM_KIND_SECONDARY_WEAPON,	STRMSG_C_131205_0011},		//		고급무기
			{121, MARKET_ITEM_KIND_MISSILE,				STRMSG_C_131205_0012},		//			미사일
			{122, MARKET_ITEM_KIND_BUNDLE,				STRMSG_C_131205_0013},		//			번들

			{200, MARKET_ITEM_KIND_ARMOR,				STRMSG_C_131205_0014},		//	아머
			{210, MARKET_ITEM_KIND_VEIL,				STRMSG_C_131205_0015},		//		베일
			{220, MARKET_ITEM_KIND_DEFENDER,			STRMSG_C_131205_0016},		//		디펜더
			{230, MARKET_ITEM_KIND_GUARDER,				STRMSG_C_131205_0017},		//		가더
			{240, MARKET_ITEM_KIND_BINDER,				STRMSG_C_131205_0018},		//		바인더

			{300, MARKET_ITEM_KIND_RADAR,				STRMSG_C_131205_0019},		//	레이더
			{310, MARKET_ITEM_KIND_RADAR_B,				STRCMD_CS_UNITKIND_BGEAR},	//		B-GEAR
			{320, MARKET_ITEM_KIND_RADAR_M,				STRCMD_CS_UNITKIND_MGEAR},	//		M-GEAR
			{330, MARKET_ITEM_KIND_RADAR_A,				STRCMD_CS_UNITKIND_AGEAR},	//		A-GEAR
			{340, MARKET_ITEM_KIND_RADAR_I,				STRCMD_CS_UNITKIND_IGEAR},	//		I-GEAR

			{400, MARKET_ITEM_KIND_SUPPORT,				STRMSG_C_131205_0020},		//	보조장비
			{410, MARKET_ITEM_KIND_UNLIMITED_ACCESSORY,	STRMSG_C_131205_0021},		//		무제한 액세서리
			{420, MARKET_ITEM_KIND_TIMELIMIT_ACCESSORY,	STRMSG_C_131205_0022},		//		시간제한 액세서리
			{430, MARKET_ITEM_KIND_COMPUTER,			STRMSG_C_131205_0023},		//		컴퓨터
			{440, MARKET_ITEM_KIND_MARK,				STRCMD_CS_ITEMKIND_MARK},	//		마크

			{500, MARKET_ITEM_KIND_ENGIN,				STRMSG_C_131205_0024},		//	엔진
			{510, MARKET_ITEM_KIND_ENGIN_B,				STRCMD_CS_UNITKIND_BGEAR},	//		B-GEAR
			{520, MARKET_ITEM_KIND_ENGIN_M,				STRCMD_CS_UNITKIND_MGEAR},	//		M-GEAR
			{530, MARKET_ITEM_KIND_ENGIN_A,				STRCMD_CS_UNITKIND_AGEAR},	//		A-GEAR
			{540, MARKET_ITEM_KIND_ENGIN_I,				STRCMD_CS_UNITKIND_IGEAR},	//		I-GEAR

			{600, MARKET_ITEM_KIND_CONSUMABLE,			STRMSG_C_131205_0025},		//	소모품
			{610, MARKET_ITEM_KIND_ENERGY,				STRMSG_C_131205_0026},		//		회복 키트
			{620, MARKET_ITEM_KIND_GAMBLE,				STRMSG_C_131205_0027},		//		갬블 키트
			{630, MARKET_ITEM_KIND_ENCHANT,				STRMSG_C_131205_0028},		//		인챈트 카드
			{640, MARKET_ITEM_KIND_CARD,				STRMSG_C_131205_0029},		//		일반 카드
			{650, MARKET_ITEM_KIND_RANDOMBOX,			STRMSG_C_131205_0030},		//		행운의 상자

			{700, MARKET_ITEM_KIND_ETC,					STRMSG_C_131205_0031},		//	기타
			{710, MARKET_ITEM_KIND_MATERIAL,			STRMSG_C_131205_0032}		//		광석
		};

		m_pItemCategorytree = new TRADEITEMCATEGORYTREE[41];
		for (int i = 0; i < 41; ++i)
		{
			memcpy(&m_pItemCategorytree[i], &TempcaTree[i], sizeof(TRADEITEMCATEGORYTREE));
		}
	}

	m_pListTreeCtrl->InsertTextItemWithByKind(0, -1, 0, m_pItemCategorytree[0].byKind, m_pItemCategorytree[0].Name, 0, TRUE);
	for (int i = 1; i < 41; ++i)
	{
		int nCategoryId = m_pItemCategorytree[i].nCategoryId;
		if (0 == nCategoryId % 100)
		{
			m_pListTreeCtrl->InsertTextItemWithByKind(0, 0, nCategoryId, m_pItemCategorytree[i].byKind, m_pItemCategorytree[i].Name, 0, FALSE);
		}
		else if (1 == nCategoryId / 100)
		{
			if (0 == nCategoryId % 10)
			{
				m_pListTreeCtrl->InsertTextItemWithByKind(0, (nCategoryId / 100) * 100, nCategoryId, m_pItemCategorytree[i].byKind, m_pItemCategorytree[i].Name, 0, FALSE);
			}
			else
			{
				m_pListTreeCtrl->InsertTextItemWithByKind(0, (nCategoryId / 10) * 10, nCategoryId, m_pItemCategorytree[i].byKind, m_pItemCategorytree[i].Name, GUI_FONT_COLOR, FALSE);
			}
		}
		else
		{
			m_pListTreeCtrl->InsertTextItemWithByKind(0, (nCategoryId / 100) * 100, nCategoryId, m_pItemCategorytree[i].byKind, m_pItemCategorytree[i].Name, GUI_FONT_COLOR, FALSE);
		}
	}
	// END 2013-12-03 by ymjoo 거래소 카테고리 목록 확장

}
int CINFCityAuction::NumEditBoxMaxAndMIN(CINFNumEditBox* pNumEditBox, int nMaxCount, BOOL bDefaultText)
{
	if (!pNumEditBox)
	{
		return 0;
	}
	INT nMixCounts = 0;
	char chBuff[256] = { 0, };
	STRNCPY_MEMSET(chBuff,pNumEditBox->GetString(),256);	
	//pNumEditBox->GetString(chBuff, 256);

	nMixCounts = atoi(chBuff);

	if (nMixCounts > nMaxCount)
	{
		nMixCounts = nMaxCount;
	}
	if (strlen(chBuff) == 1 && chBuff[0] == ' ')
		return 0;

	if (strlen(chBuff) == 1 && chBuff[0] == 48)
	{
		if (bDefaultText)
			pNumEditBox->SetString(" ", 32);
	}
	else
	{
		//if (m_pParent)
			//((CINFTradeItemCenter*)m_pParent)->NumEditBoxChangeCount(pNumEditBox, nMixCounts);
	}

	return nMixCounts;
}
int CINFCityAuction::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	
	/*if (!IsShowWnd())
	{
		return INF_MSGPROC_NORMAL;
	}
	if (INF_MSGPROC_BREAK == CINFDefaultWnd::WndProc(uMsg, wParam, lParam))
	{
		POINT ptBkPos = m_pBkImage[TAB_BUY_ITEM]->GetImgSize();
		UpdateBtnPos(ptBkPos.x, ptBkPos.y);

		return INF_MSGPROC_BREAK;
	}*/
	switch (uMsg)
	{
	case WM_MOUSEMOVE:
	{
		return OnMouseMove(uMsg, wParam, lParam);
	}
	break;
	
	case WM_LBUTTONUP:
	{
		return OnLButtonUp(uMsg, wParam, lParam);
	}
	break;
	case WM_LBUTTONDOWN:
	{
		return OnLButtonDown(uMsg, wParam, lParam);
	}
	break;
	

	case WM_MOUSEWHEEL:
	{
		//m_nCurerntInfo = NULL;
		POINT ptBkPos;// = GetBkPos();
		ptBkPos.x = _INET_OSR_MARKET_POS_X;
		ptBkPos.y = _INET_OSR_MARKET_POS_Y;

		POINT pt;
		GetCursorPos(&pt);
		ScreenToClient(g_pD3dApp->GetHwnd(), &pt);
		CheckMouseReverse(&pt);
		if (m_nCurrentTab == TAB_FINISHED_ITEM) {
			//max = m_vecMyItemInfo.size() - ITEMLIST_ITEMNUM
			//	nMylistStartFromId
			int nMaxIdx = m_vecMyItemInfo.size() - 9;
			int nPosY = _INET_OSR_MARKET_POS_Y + ITEMLIST_LOCATION_Y - ITEMLIST_CELL_HEIGHT;
			if ((nPosY <= pt.y) && (pt.y < (nPosY + 450)))
			{
				int nPosX = _INET_OSR_MARKET_POS_X + ITEMLIST_LOCATION_X;
				if ((nPosX <= pt.x) && (pt.x < (nPosX + ITEMLIST_WIDTH)))
				{
					//if (!IsRqPossibleStats()) //no need to limit - list preloaded
						//return INF_MSGPROC_BREAK;

					if ((int)wParam < 0)//scrl  up
					{
						if (nMylistStartFromId < nMaxIdx)
							nMylistStartFromId++;

						return INF_MSGPROC_BREAK;
					}
					else //scr down
					{
						if (nMylistStartFromId > 0)
							nMylistStartFromId--;

						return INF_MSGPROC_BREAK;
					}
				}

			}
		}
		if (m_nCurrentTab == TAB_BUY_ITEM) {
			if (m_pListTreeCtrl->OnMouseWheel(pt, wParam, lParam))
			{
				return INF_MSGPROC_BREAK;
			}

			int nOutPages = m_nMaxPages;
			int nOutPagesMax = m_nMaxPages;

			if (nOutPages >= MAX_PAGES_COUNT)
				nOutPages = MAX_PAGES_COUNT;

			int nCnt = 0;
			float fAddX = (WORLD_RANK_PAGE_WIDTH * (MAX_PAGES_COUNT - nOutPages)) / 2;
			int nPosY = ptBkPos.y + WORLD_RANK_PAGE_POS_Y - 3;
			//scrolling while mouse is over page numbers
			if ((nPosY <= pt.y) && (pt.y < (nPosY + 20)))
			{
				for (nCnt = 0; nCnt < nOutPages; nCnt++)
				{
					int nPosX = fAddX + ptBkPos.x + WORLD_RANK_PAGE_POS_X + (nCnt * WORLD_RANK_PAGE_WIDTH);
					if ((nPosX <= pt.x) && (pt.x < (nPosX + WORLD_RANK_PAGE_WIDTH)))
					{
						if (!IsRqPossibleStats())
							return INF_MSGPROC_BREAK;

						if ((int)wParam < 0)//scrl  up
						{

							if (nFirstPage + 10 <= nOutPagesMax)
								nFirstPage++;

							RqItemListFromServer();

							return INF_MSGPROC_BREAK;
						}
						else //scr down
						{
							if (nFirstPage > 1)
								nFirstPage--;

							RqItemListFromServer();

							return INF_MSGPROC_BREAK;
						}
					}
				}
			}

			//scrolling when mouse is over table - switch page
			if (pt.x > ptBkPos.x + SCROLL_PAGE_X && //start x
				pt.x < ptBkPos.x + SCROLL_PAGE_END_X && //end x
				pt.y > ptBkPos.y + SCROLL_PAGE_Y && //start y
				pt.y < ptBkPos.y + SCROLL_PAGE_END_Y) //end y 
			{
				if (!IsRqPossibleStats())
					return INF_MSGPROC_BREAK;

				if ((int)wParam < 0)
				{
					if (nFirstPage + m_nSelectPage < nOutPagesMax)
					{
						if (m_nSelectPage + 1 > MAX_PER_ONEPAGE)
							nFirstPage++;

						else {
							m_nSelectPage++;

						}
						RqItemListFromServer();
					}

					return INF_MSGPROC_BREAK;
				}
				else
				{
					if (m_nSelectPage <= 0)
						return INF_MSGPROC_BREAK;

					if (nFirstPage > 1)
						nFirstPage--;
					else {
						m_nSelectPage--;

					}

					RqItemListFromServer();

					return INF_MSGPROC_BREAK;
				}
			}
		}
	}
	break;
	
	case WM_IME_STARTCOMPOSITION:
		//	case WM_IME_NOTIFY:
	case WM_IME_COMPOSITION:
	case WM_INPUTLANGCHANGE:
	case WM_IME_ENDCOMPOSITION:
	case WM_IME_SETCONTEXT:
	case WM_CHAR:
	case WM_KEYDOWN:
	case WM_KEYUP:
	{
		if (WM_KEYDOWN == uMsg)
		{
			if (m_nCurrentTab == TAB_SELL_ITEM) {
				
				if (m_pNumEditBox->WndProc(uMsg, wParam, lParam))
				{
					// 갱신
					return INF_MSGPROC_BREAK;
				}
				else
				{
					if (wParam == VK_RETURN)
					{
						if (m_pNumEditBox->IsEditMode())
						{
							int nNum = 0;
							m_pNumEditBox->EnableEdit(FALSE);
							CItemInfo* pItemInfo = g_pStoreData->FindItemInInventoryByUniqueNumber(m_pSourceItem->UniqueNumber);
							if (m_pSourceItem)
							{
								if (!pItemInfo)
								{
									if (m_pSourceItem->CurrentCount <= NumEditBoxMaxAndMIN(m_pNumEditBox, m_pSourceItem->CurrentCount))
										return INF_MSGPROC_BREAK;

									g_pStoreData->PutItem((char*)((ITEM_GENERAL*)m_pSourceItem), TRUE);
									pItemInfo = g_pStoreData->FindItemInInventoryByUniqueNumber(m_pSourceItem->UniqueNumber);
									ASSERT_ASSERT(pItemInfo);
									pItemInfo->CopyItemInfo(m_pSourceItem);
									pItemInfo->CurrentCount = 0;
								}
								nNum = NumEditBoxMaxAndMIN(m_pNumEditBox, m_pSourceItem->CurrentCount + pItemInfo->CurrentCount);
								nNum = nNum - m_pSourceItem->CurrentCount;
								InvenToSourceItem(pItemInfo, nNum);
								return INF_MSGPROC_BREAK;
							}
						}
					}
				}

				if (m_pMoneyEditBox->WndProc(uMsg, wParam, lParam))
				{
					// 갱신
					return INF_MSGPROC_BREAK;
				}
				else
				{
					if (wParam == VK_RETURN)
					{
						if (m_pMoneyEditBox->IsEditMode())
						{
							if (m_pSourceItem)
							{
								m_pMoneyEditBox->EnableEdit(FALSE);
								NumEditBoxMaxAndMIN(m_pMoneyEditBox, MAX_ITEM_COUNTS);
							}
						}
					}

				}
			}
		}
		if (m_nCurrentTab == TAB_BUY_ITEM) {
			if (m_pNameEditBox->WndProc(uMsg, wParam, lParam, NULL, TRUE))
			{
				return INF_MSGPROC_BREAK;
			}

			if (WM_KEYDOWN == uMsg)
			{
				//if (wParam == VK_F5)
				//{
				//	if (!IsRqPossibleStats())
				//		return INF_MSGPROC_BREAK;

				//	//do refresh procedure
				//	m_nSelectListItem = -1;
				//	g_pFieldWinSocket->SendMsg(T_FC_MARKET_BASE_INFO_REQUEST, NULL, 0);

				//	//end refreshing
				//	g_pD3dApp->m_pSound->PlayD3DSound(SOUND_SELECT_BUTTON, D3DXVECTOR3(0, 0, 0), FALSE);

				//	return  INF_MSGPROC_BREAK;
				//}

				for (int i = 0; i < 2; ++i)
				{
					if (m_plevelNumEditBox[i]->WndProc(uMsg, wParam, lParam))
					{
						// 갱신
						return INF_MSGPROC_BREAK;
					}
					else
					{
						if (wParam == VK_RETURN)
						{
							if (m_plevelNumEditBox[i]->IsEditMode())
							{
								if (m_plevelNumEditBox[i])
								{
									NumEditBoxMaxAndMIN(m_plevelNumEditBox[i], MAXLEVEL);
									m_plevelNumEditBox[i]->EnableEdit(FALSE);
								}
							}
						}
					}

					if (m_pEnchantNumEditBox[i]->WndProc(uMsg, wParam, lParam))
					{
						// 갱신
						return INF_MSGPROC_BREAK;
					}
					else
					{
						if (wParam == VK_RETURN)
						{
							if (m_pEnchantNumEditBox[i]->IsEditMode())
							{
								if (m_pEnchantNumEditBox[i])
								{
									NumEditBoxMaxAndMIN(m_pEnchantNumEditBox[i], MAXENCHANT, FALSE);
									m_pEnchantNumEditBox[i]->EnableEdit(FALSE);
								}
							}
						}
					}
				}

				//if (wParam == VK_RETURN)
				//{
				//	if (!IsRqPossibleStats())
				//		return INF_MSGPROC_BREAK;

				//	//do SEARCH procedure

				//	Search();
				//	//end refreshing
				//	g_pD3dApp->m_pSound->PlayD3DSound(SOUND_SELECT_BUTTON, D3DXVECTOR3(0, 0, 0), FALSE);

				//	return  INF_MSGPROC_BREAK;
				//}
			}
		}
	}
	break;
	}

	return INF_MSGPROC_NORMAL;
}
BOOL CINFCityAuction::UpLoadItem(CItemInfo* i_pItem)
{
	if (!g_pGameMain)
		return FALSE;

	if (i_pItem->Wear != WEAR_NOT_ATTACHED)
	{
		g_pGameMain->m_pInfWindow->AddMsgBox(STRMSG_C_CITYLAP_0002, _Q_MARKET_NORMAL_MESSAGE);//"착용된 아이템은 올릴 수 없습니다."
		return FALSE;
	}
	else if (COMPARE_BIT_FLAG(i_pItem->ItemInfo->ItemAttribute, ITEM_ATTR_NO_TRANSFER | ITEM_ATTR_KILL_MARK_ITEM | ITEM_ATTR_ACCOUNT_POSSESSION))
	{
		// 거래 할 수 없는 아이템
		g_pGameMain->m_pInfWindow->AddMsgBox(STRMSG_C_060728_0003, _Q_MARKET_NORMAL_MESSAGE);	// "등록을 할 수 없는 아이템 입니다."
		return FALSE;
	}
	else if (m_pSourceItem && m_pSourceItem->UniqueNumber == i_pItem->UniqueNumber)
	{
		// 중복
		g_pGameMain->m_pInfWindow->AddMsgBox(STRERR_ERROR_0140, _Q_MARKET_NORMAL_MESSAGE);		// "목록에 이미 등록되어 있습니다."
		return FALSE;
	}
	else
	{
		
		// 2007-12-13 by dgwoo 시간제 아이템이면서 사용한 아이템이라면 팩토리에 올릴수 없다.
		if (((i_pItem->ItemInfo->Kind == ITEMKIND_ACCESSORY_TIMELIMIT
			|| COMPARE_BIT_FLAG(i_pItem->ItemInfo->ItemAttribute, ITEM_ATTR_TIME_LIMITE)
			|| COMPARE_BIT_FLAG(i_pItem->ItemInfo->ItemAttribute, ITEM_ATTR_DELETED_TIME_LIMITE_AFTER_USED)) // 2008-11-26 by bhsohn 절대시간 제한 아이템 구현
			&& i_pItem->GetItemPassTime() != 0) 
			|| COMPARE_BIT_FLAG(i_pItem->ItemInfo->ItemAttribute, ITEM_ATTR_DELETED_TIME_LIMITE)) // 26-02-2024 by Inet - forbid to register items that are auto deleted after X time from creation (xmas wreath etc)
		{
			g_pGameMain->m_pInfWindow->AddMsgBox(STRMSG_C_060728_0003, _Q_MARKET_NORMAL_MESSAGE);	// "등록을 할 수 없는 아이템 입니다."
			return FALSE;
		}
		else
		{
			if (IS_COUNTABLE_ITEM(i_pItem->Kind))
			{
				InvenToSourceItem(i_pItem, 1);
			}
			else
			{
				// 2013-06-13 by ssjung 외형 변경 컬렉션 예외처리 추가(외형 적용이 안되야 할 부분에 외형 적용이 안될 수 있도록)
#ifdef SC_COLLECTION_ARMOR_JHSEOL_BCKIM
				if (i_pItem->FixedTermShape.nStatShapeItemNum && i_pItem->FixedTermShape.nStatLevel)
				{
					g_pStoreData->RqCollectionShapeChange(i_pItem->UniqueNumber, 0);
				}
#endif
				//end 2013-06-13 by ssjung 외형 변경 컬렉션 예외처리 추가(외형 적용이 안되야 할 부분에 외형 적용이 안될 수 있도록)
				InvenToSourceItem(i_pItem, 1);
			}
			//if (m_pParent)
			//	((CINFTradeItemCenter*)m_pParent)->GetItemName(m_pSourceItem, m_pItemName);

			NumEditBoxChangeCount(m_pNumEditBox, 1);
			m_pNumEditBox->EnableEdit(FALSE);
			NumEditBoxChangeCount(m_pMoneyEditBox, 1);
			m_pMoneyEditBox->EnableEdit(FALSE);

			return TRUE;
		}

	}
}
void CINFCityAuction::NumEditBoxChangeCount(CINFNumEditBox* pNumEditBox, int nNum)
{
	char chBuff[32];
	wsprintf(chBuff, "%d", nNum);
	pNumEditBox->SetString(chBuff, 32);
}
void CINFCityAuction::InitData()
{
	char chBlank[16];
	memset(chBlank, 0x00, 16);
	nFirstPage = 1;
	m_pNameEditBox->SetString(chBlank, 16);
	NumEditBoxChangeCount(m_pNumEditBox, 1);
	NumEditBoxChangeCount(m_pMoneyEditBox, 1);
	m_pMoneyComboBox->SetSelectItem(0);
	m_byMoneySelect = TC_SPI;

	if (!m_pSourceItem)
		return;

	g_pStoreData->PutItem((char*)((ITEM_GENERAL*)m_pSourceItem), TRUE);
	CItemInfo* pItemInfo = g_pStoreData->FindItemInInventoryByUniqueNumber(m_pSourceItem->UniqueNumber);
	ASSERT_ASSERT(pItemInfo);

	pItemInfo->CopyItemInfo(m_pSourceItem);

	g_pShuttleChild->ResortingItem();
	if (g_pGameMain && g_pGameMain->m_pInven)
	{
		g_pGameMain->m_pInven->SetScrollEndLine();
		g_pGameMain->m_pInven->SetAllIconInfo();
	}
	SAFE_DELETE(m_pSourceItem);
	m_pSourceItem = NULL;
}
void CINFCityAuction::InvenToSourceItem(CItemInfo* pItemInfo, int nCount)
{
	BOOL bRefresh = FALSE;
	if (IS_COUNTABLE_ITEM(pItemInfo->Kind))
	{
		ASSERT_ASSERT(pItemInfo->CurrentCount >= nCount);

		if (m_pSourceItem && m_pSourceItem->UniqueNumber == pItemInfo->UniqueNumber)
		{
			m_pSourceItem->CurrentCount += nCount;
			g_pStoreData->UpdateItemCount(pItemInfo->UniqueNumber, pItemInfo->CurrentCount - nCount);
		}
		else
		{
			InitData();

			CItemInfo* pNewItem = new CItemInfo((ITEM_GENERAL*)pItemInfo);
			pNewItem->CopyItemInfo(pItemInfo);
			pNewItem->CurrentCount = nCount;
			m_pSourceItem = pNewItem;
			g_pStoreData->UpdateItemCount(pItemInfo->UniqueNumber, pItemInfo->CurrentCount - nCount);
		}
	}
	else
	{
		InitData();

		CItemInfo* pNewItem = new CItemInfo((ITEM_GENERAL*)pItemInfo);

		pNewItem->CopyItemInfo(pItemInfo);
		m_pSourceItem = pNewItem;
		g_pStoreData->DeleteItem(pItemInfo->UniqueNumber);
	}
}
void CINFCityAuction::ItemButOk(UID64_t MarketUID)
{
	//((CINFTradeItemCenter*)m_pParent)->TradeCenterLock(FALSE);
	g_pGameMain->m_pInfWindow->AddMsgBox(STRMSG_C_131206_0001, _Q_MARKET_NORMAL_MESSAGE, 0, 0, 0, 0, NULL);	
	nTickWaitRefresh = 200;//safe time to refresh list automatically
}
void CINFCityAuction::ADDItemOk(UID64_t ItemUID)
{
	//((CINFTradeItemCenter*)m_pParent)->TradeCenterLock(FALSE);
	g_pGameMain->m_pInfWindow->AddMsgBox(STRMSG_C_131205_0039, _Q_MARKET_NORMAL_MESSAGE);
	if (m_pSourceItem->UniqueNumber == ItemUID)
	{
		if (m_pSourceItem)
		{
			SAFE_DELETE(m_pSourceItem);
			m_pSourceItem = NULL;
		}
		InitData();
	}
}
void CINFCityAuction::ADDItem()
{
	char cNameTemp[MARKET_ITEM_FULL_NAME] = { 0, };
	MSG_FC_MARKET_SELL_REQUEST sMsg;
	memset(&sMsg, 0x00, sizeof(sMsg));
	UINT nSelect = m_pMoneyComboBox->GetSelect();

	sMsg.ItemUID = m_pSourceItem->UniqueNumber;
	sMsg.MoneyType = m_pMoneyComboBox->GetSelect();
	sMsg.Price = NumEditBoxMaxAndMIN(m_pMoneyEditBox, MAX_ITEM_COUNTS);
	sMsg.Count = m_pSourceItem->CurrentCount;
	//sMsg.Count = NumEditBoxMaxAndMIN(m_pNumEditBox,ITEM_MAX_COUNT);

	GetItemName(m_pSourceItem, cNameTemp, FALSE);

	if (cNameTemp[0] == 32)
		strcpy(sMsg.Name, &cNameTemp[1]);
	else
		strcpy(sMsg.Name, cNameTemp);

	g_pFieldWinSocket->SendMsg(T_FC_MARKET_SELL_REQUEST, (char*)&sMsg, sizeof(sMsg));
	//((CINFTradeItemCenter*)m_pParent)->TradeCenterLock(TRUE);
}
void CINFCityAuction::SendGetRequest(UID64_t UID, BYTE byState, INT nPrice, BYTE MoneyType)
{
	if (MARKET_STATE_SELL == byState)
		g_pGameMain->m_pInfWindow->AddMsgBox(STRMSG_C_131205_0047, _Q_MARKET_GET_ITEM, byState, 0, 0, UID);
	else if (MARKET_STATE_TIME_OUT == byState)
		g_pGameMain->m_pInfWindow->AddMsgBox(STRMSG_C_131205_0048, _Q_MARKET_GET_ITEM, byState, 0, 0, UID);
	else if (MARKET_STATE_SELL_DONE == byState)
	{
		char cTemp[256] = { 0, };
		char cTemp02[256] = { 0, };
		INT nTemp = 0;

		if (MoneyType == TC_SPI)
			nTemp = GetMarketSellChargeSPI(nPrice);
		else if (MoneyType == TC_WP)
			nTemp = GetMarketSellChargeWP(nPrice);
		else if (MoneyType == TC_CREDITS)
			nTemp = GetMarketSellChargeCRED(nPrice);

		wsprintf(cTemp, "%d", nTemp);
		MakeCurrencySeparator(cTemp02, cTemp, 3, ',');

		if (MoneyType == TC_SPI)
			wsprintf(cTemp, STRMSG_C_131205_0049, cTemp02, "SPI");
		else if (MoneyType == TC_WP)
			wsprintf(cTemp, STRMSG_C_131205_0049, cTemp02, "WP");
		else if (MoneyType == TC_CREDITS)
			wsprintf(cTemp, STRMSG_C_131205_0049, cTemp02, "Credits");
		
		g_pGameMain->m_pInfWindow->AddMsgBox(cTemp, _Q_MARKET_GET_ITEM, byState, 0, 0, UID, NULL);
	}
	
}
int CINFCityAuction::OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	POINT pt;
	pt.x = LOWORD(lParam);
	pt.y = HIWORD(lParam);
	CheckMouseReverse(&pt);
	if (m_pCloseXBtn->OnLButtonUp(pt))
	{
		InitData();
		g_pGameMain->SetItemInfo(0, 0, 0, 0);
		g_pGameMain->m_pInven->ShowInven(NULL, NULL);
		g_pGameMain->m_pInven->SetTradeItemCenterState(FALSE);
		m_pMoneyEditBox->EnableEdit(FALSE);
		m_pNumEditBox->EnableEdit(FALSE);
		m_pNameEditBox->EnableEdit(FALSE, FALSE);
		m_plevelNumEditBox[0]->EnableEdit(FALSE);
		m_plevelNumEditBox[1]->EnableEdit(FALSE);
		m_pEnchantNumEditBox[0]->EnableEdit(FALSE);
		m_pEnchantNumEditBox[1]->EnableEdit(FALSE);

		g_pInterface->m_pCityBase->OnCityNPCButtonDown(CITY_NPC_BUTTON_CLOSE);
		return INF_MSGPROC_BREAK;
	}
	int nCnt = 0;
	{
		for (nCnt = 0; nCnt < TAB_COUNT; nCnt++)
		{
			if (m_pRankBtn[nCnt] && TRUE == m_pRankBtn[nCnt]->OnLButtonUp(pt))
			{
				if (nCnt == TAB_FINISHED_ITEM) { //load at first page open without refresh press
					g_pFieldWinSocket->SendMsg(T_FC_MARKET_MY_LIST_REQUEST, NULL, 0);
				}
				OnClickRankBtn(nCnt);
				if(g_pD3dApp->m_pSound)
					g_pD3dApp->m_pSound->PlayD3DSound(SOUND_SELECT_BUTTON, D3DXVECTOR3(0, 0, 0), FALSE);
				return  INF_MSGPROC_BREAK;
			}
		}
		OnClickRankBtn(m_nCurrentTab);
	}
	if (m_nCurrentTab == TAB_FINISHED_ITEM) {
		if (m_pBtnHover[1] && TRUE == m_pBtnHover[1]->OnLButtonUp(pt))//refresh finished auctions
		{
			g_pFieldWinSocket->SendMsg(T_FC_MARKET_MY_LIST_REQUEST, NULL, 0);

			if(g_pD3dApp->m_pSound)
				g_pD3dApp->m_pSound->PlayD3DSound(SOUND_SELECT_BUTTON, D3DXVECTOR3(0, 0, 0), FALSE);
			return  INF_MSGPROC_BREAK;
		}
		int nTemp = -1;
		if (pt.x > _INET_OSR_MARKET_POS_X + ITEMLIST_LOCATION_X + 16  && pt.x < _INET_OSR_MARKET_POS_X + ITEMLIST_LOCATION_X + 16 + ITEMLIST_WIDTH)
		{
			if (pt.y > _INET_OSR_MARKET_POS_Y + ITEMLIST_LOCATION_Y - 1 -ITEMLIST_CELL_HEIGHT && pt.y < _INET_OSR_MARKET_POS_Y + ITEMLIST_LOCATION_Y - 1 + ITEMLIST_HEIGHT + ITEMLIST_CELL_HEIGHT)
			{
				int nStartRowY = 0;
				int nEndRowY = 0;
				int nHitNo = -1;
				for (auto el = 0; el < MAX_PER_ONEPAGE + 2; el++)
				{
					nStartRowY = _INET_OSR_MARKET_POS_Y - ITEMLIST_CELL_HEIGHT + ITEMLIST_LOCATION_Y + (el * ITEMLIST_CELL_HEIGHT + 1);
					nEndRowY = _INET_OSR_MARKET_POS_Y - ITEMLIST_CELL_HEIGHT + ITEMLIST_LOCATION_Y + ((el + 1) * ITEMLIST_CELL_HEIGHT + 1);
					if (pt.y > nStartRowY && pt.y < nEndRowY) {
						nHitNo = el + nMylistStartFromId;
						break;
					}
				}
				if (nHitNo > -1 && nHitNo < m_vecMyItemInfo.size())
				{
					m_nSelectMyListItem = nHitNo;
					//check only for button not for highlight
					if (pt.x > _INET_OSR_MARKET_POS_X + ITEMLIST_LOCATION_X + 16 + ITEMLIST_WIDTH - 45 && pt.x < _INET_OSR_MARKET_POS_X + ITEMLIST_LOCATION_X + 16 + ITEMLIST_WIDTH)
					{
						MARKET_MY_ITEM_INFO* pMarketItem = NULL;
						pMarketItem = m_vecMyItemInfo[m_nSelectMyListItem];
						if (pMarketItem)
						{
							if (pMarketItem->byBtnState == BTN_STATUS_DOWN)
							{
								SendGetRequest(pMarketItem->pINFO->MarketInfo.MarketUID, pMarketItem->pINFO->MarketInfo.MarketState, pMarketItem->pINFO->MarketInfo.Price, pMarketItem->pINFO->MarketInfo.MoneyType);
								pMarketItem->byBtnState = BTN_STATUS_UP;
							}
						}
					}
				}

				return  INF_MSGPROC_BREAK;
			}
		}
	}
	if (m_nCurrentTab == TAB_SELL_ITEM) {
		{
			if (pt.x > _INET_OSR_MARKET_POS_X + 691 && pt.x < _INET_OSR_MARKET_POS_X + 28 + 691 &&
				pt.y > _INET_OSR_MARKET_POS_Y + 185 && pt.y < _INET_OSR_MARKET_POS_Y + 28 + 185)
			{
				if (g_pGameMain && g_pGameMain->m_stSelectItem.pSelectItem && g_pGameMain->m_stSelectItem.pSelectItem->pItem)
				{
					UID64_t uItemUniNum = g_pGameMain->m_stSelectItem.pSelectItem->pItem->UniqueNumber;
					CItemInfo* pItemInfo = g_pStoreData->FindItemInInventoryByUniqueNumber(uItemUniNum);
					if (pItemInfo)
						UpLoadItem(pItemInfo);

					return INF_MSGPROC_NORMAL;
				}
			}
		}

		if (TRUE == m_pMaxBtn->OnLButtonUp(pt))
		{
			if (m_pSourceItem)
			{
				CItemInfo* pItemInfo = g_pStoreData->FindItemInInventoryByUniqueNumber(m_pSourceItem->UniqueNumber);

				if (pItemInfo && IS_COUNTABLE_ITEM(pItemInfo->Kind))
				{
					InvenToSourceItem(pItemInfo, pItemInfo->CurrentCount);
					NumEditBoxChangeCount(m_pNumEditBox, m_pSourceItem->CurrentCount);
				}
			}
			return INF_MSGPROC_BREAK;
		}

		int nNum = 0;
		if (TRUE == m_pUpCntBtn->OnLButtonUp(pt))
		{
			if (m_pSourceItem && IS_COUNTABLE_ITEM(m_pSourceItem->Kind))
			{
				CItemInfo* pItemInfo = g_pStoreData->FindItemInInventoryByUniqueNumber(m_pSourceItem->UniqueNumber);

				if (pItemInfo)
				{
					NumEditBoxChangeCount(m_pNumEditBox, m_pSourceItem->CurrentCount + 1);
					nNum = NumEditBoxMaxAndMIN(m_pNumEditBox, m_pSourceItem->CurrentCount + pItemInfo->CurrentCount);
					nNum = nNum - m_pSourceItem->CurrentCount;
					InvenToSourceItem(pItemInfo, nNum);
				}
			}
			return INF_MSGPROC_BREAK;
		}
		if (TRUE == m_pRegisterItemBtn->OnLButtonUp(pt))
		{
			if (!m_pSourceItem)
				g_pGameMain->m_pInfWindow->AddMsgBox(STRMSG_C_131205_0037, _Q_MARKET_NORMAL_MESSAGE);
			else
				g_pGameMain->m_pInfWindow->AddMsgBox(STRMSG_C_131205_0038, _Q_MARKET_ADD_ITEM);
			return  INF_MSGPROC_BREAK;

		}
		if (TRUE == m_pDownCntBtn->OnLButtonUp(pt))
		{
			int nNum = 0;
			m_pNumEditBox->EnableEdit(FALSE);
			if (m_pSourceItem && IS_COUNTABLE_ITEM(m_pSourceItem->Kind))
			{
				CItemInfo* pItemInfo = g_pStoreData->FindItemInInventoryByUniqueNumber(m_pSourceItem->UniqueNumber);
				if (m_pSourceItem->CurrentCount <= 1)
					return INF_MSGPROC_BREAK;
				else
					NumEditBoxChangeCount(m_pNumEditBox, m_pSourceItem->CurrentCount - 1);

				if (!pItemInfo)
				{
					g_pStoreData->PutItem((char*)((ITEM_GENERAL*)m_pSourceItem), TRUE);
					pItemInfo = g_pStoreData->FindItemInInventoryByUniqueNumber(m_pSourceItem->UniqueNumber);
					ASSERT_ASSERT(pItemInfo);
					pItemInfo->CopyItemInfo(m_pSourceItem);
					pItemInfo->CurrentCount = 0;
				}
				nNum = NumEditBoxMaxAndMIN(m_pNumEditBox, m_pSourceItem->CurrentCount + pItemInfo->CurrentCount);
				nNum = nNum - m_pSourceItem->CurrentCount;
				InvenToSourceItem(pItemInfo, nNum);
			}
			return INF_MSGPROC_BREAK;
		}
	}
	if (m_nCurrentTab == TAB_BUY_ITEM) {
		if (nTickWaitSortClick == -1)
		{
			int nItemWidth[6] = { 42 , 42 , 42 ,42 ,42 ,42 };
			int nSpacer = 0;

			for (auto hdr = 0; hdr < 6; hdr++) {
				if (hdr == 2)
					nSpacer = 190;
				if (hdr == 5)
					nSpacer = 320;

				if (pt.x > _INET_OSR_MARKET_POS_X + ITEMLIST_LOCATION_X + 16 + (hdr * nItemWidth[hdr]) && pt.x < _INET_OSR_MARKET_POS_X + ITEMLIST_LOCATION_X + 16 + nSpacer + ((hdr + 1) * nItemWidth[hdr]))
				{
					if (pt.y > _INET_OSR_MARKET_POS_Y + ITEMLIST_LOCATION_Y - 24 && pt.y < _INET_OSR_MARKET_POS_Y + ITEMLIST_LOCATION_Y - 1)
					{
						char szTmp[64];

						switch (hdr)
						{
						case 0:

						{
							sprintf(szTmp, "Sort mode : \\lNone");
							m_nCurrentSortingType = 0;
							g_pFieldWinSocket->SendMsg(T_FC_MARKET_BASE_INFO_REQUEST, NULL, 0);
						}
						break;
						case 5: //price
						{
							if (m_nCurrentSortingType == MARKET_SORT_PRICE_DOWN) {
								m_nCurrentSortingType = MARKET_SORT_PRICE_UP;
								sprintf(szTmp, "Sort mode : \\lPrice Ascending");
							}
							else if (m_nCurrentSortingType == MARKET_SORT_PRICE_UP)
							{
								m_nCurrentSortingType = MARKET_SORT_PRICE_DOWN;
								sprintf(szTmp, "Sort mode : \\lPrice Descending");
							}
							else {
								m_nCurrentSortingType = MARKET_SORT_PRICE_UP;
								sprintf(szTmp, "Sort mode : \\lPrice Ascending");
							}
						}
						break;
						case 3: //level
						{
							if (m_nCurrentSortingType == MARKET_SORT_LEVEL_DOWN) {
								m_nCurrentSortingType = MARKET_SORT_LEVEL_UP;
								sprintf(szTmp, "Sort mode : \\lLevel Ascending");
							}
							else if (m_nCurrentSortingType == MARKET_SORT_LEVEL_UP)
							{
								m_nCurrentSortingType = MARKET_SORT_LEVEL_DOWN;
								sprintf(szTmp, "Sort mode : \\lLevel Descending");
							}
							else {
								m_nCurrentSortingType = MARKET_SORT_LEVEL_UP;
								sprintf(szTmp, "Sort mode : \\lLevel Ascending");
							}
						}
						break;
						case 1: //enchant
						{
							if (m_nCurrentSortingType == MARKET_SORT_ENCHANT_DOWN) {
								m_nCurrentSortingType = MARKET_SORT_ENCHANT_UP;
								sprintf(szTmp, "Sort mode : \\lEnchant Ascending");
							}
							else if (m_nCurrentSortingType == MARKET_SORT_ENCHANT_UP)
							{
								m_nCurrentSortingType = MARKET_SORT_ENCHANT_DOWN;
								sprintf(szTmp, "Sort mode : \\lEnchant Descending");
							}
							else {
								m_nCurrentSortingType = MARKET_SORT_ENCHANT_UP;
								sprintf(szTmp, "Sort mode : \\lEnchant Ascending");
							}
						}
						break;
						case 2: //name
						{
							if (m_nCurrentSortingType == MARKET_SORT_NAME_DOWN) {
								m_nCurrentSortingType = MARKET_SORT_NAME_UP;
								sprintf(szTmp, "Sort mode : \\lName Ascending");
							}
							else if (m_nCurrentSortingType == MARKET_SORT_NAME_UP)
							{
								m_nCurrentSortingType = MARKET_SORT_NAME_DOWN;
								sprintf(szTmp, "Sort mode : \\lName Descending");
							}
							else {
								m_nCurrentSortingType = MARKET_SORT_NAME_UP;
								sprintf(szTmp, "Sort mode : \\lName Ascending");
							}
						}
						break;
						case 4: //age - exp. time
						{
							if (m_nCurrentSortingType == MARKET_SORT_TIME_DOWN) {
								m_nCurrentSortingType = MARKET_SORT_TIME_UP;
								sprintf(szTmp, "Sort mode : Time Ascending");
							}
							else if (m_nCurrentSortingType == MARKET_SORT_TIME_UP)
							{
								m_nCurrentSortingType = MARKET_SORT_TIME_DOWN;
								sprintf(szTmp, "Sort mode : Time Descending");
							}
							else {
								m_nCurrentSortingType = MARKET_SORT_TIME_UP;
								sprintf(szTmp, "Sort mode : Time Ascending");
							}
						}
						break;
						}
						g_pD3dApp->m_pChat->CreateChatChild(szTmp, COLOR_ITEM);
						//g_pGameMain->m_pInfWindow->AddMsgBox(szTmp, _Q_MARKET_NORMAL_MESSAGE); //messagebox version
						if (m_nCurrentSortingType > 0)
						{
							MSG_FC_MARKET_SORT_REQUEST sMsg;
							memset(&sMsg, 0x00, sizeof(sMsg));
							sMsg.SortingType = m_nCurrentSortingType;
							g_pFieldWinSocket->SendMsg(T_FC_MARKET_SORT_REQUEST, (char*)&sMsg, sizeof(sMsg));
							nTickWaitSortClick = 200;
							return  INF_MSGPROC_BREAK;
						}
					}
				}
			}
		}
		int nTemp = -1;
		if (pt.x > _INET_OSR_MARKET_POS_X + ITEMLIST_LOCATION_X + 16 && pt.x < _INET_OSR_MARKET_POS_X + ITEMLIST_LOCATION_X + 16 + ITEMLIST_WIDTH)
		{
			if (pt.y > _INET_OSR_MARKET_POS_Y + ITEMLIST_LOCATION_Y - 1 && pt.y < _INET_OSR_MARKET_POS_Y + ITEMLIST_LOCATION_Y - 1 + ITEMLIST_HEIGHT + ITEMLIST_CELL_HEIGHT)
			{
				int nStartRowY = 0;
				int nEndRowY = 0;
				int nHitNo = -1;
				for (auto el = 0; el < MAX_PER_ONEPAGE + 1; el++)
				{
					nStartRowY = _INET_OSR_MARKET_POS_Y + ITEMLIST_LOCATION_Y + (el * ITEMLIST_CELL_HEIGHT + 1);
					nEndRowY = _INET_OSR_MARKET_POS_Y + ITEMLIST_LOCATION_Y + ((el + 1) * ITEMLIST_CELL_HEIGHT + 1);
					if (pt.y > nStartRowY && pt.y < nEndRowY) {
						nHitNo = el;
						break;
					}
				}
				if (nHitNo > -1 && nHitNo < m_vecItemInfo.size())
				{
					m_nSelectListItem = nHitNo;
				}

				return  INF_MSGPROC_BREAK;
			}
		}


		if (TRUE == m_pListTreeCtrl->OnLButtonUp(pt))
		{
			return  INF_MSGPROC_BREAK;
		}
		if (m_pBtnHover[0] && TRUE == m_pBtnHover[0]->OnLButtonUp(pt)) //search
		{
			Search();
			if(g_pD3dApp->m_pSound)
				g_pD3dApp->m_pSound->PlayD3DSound(SOUND_SELECT_BUTTON, D3DXVECTOR3(0, 0, 0), FALSE);
			return  INF_MSGPROC_BREAK;
		}
		if (m_pBtnHover[1] && TRUE == m_pBtnHover[1]->OnLButtonUp(pt))//refresh
		{
			g_pFieldWinSocket->SendMsg(T_FC_MARKET_BASE_INFO_REQUEST, NULL, 0);

			if(g_pD3dApp->m_pSound)
				g_pD3dApp->m_pSound->PlayD3DSound(SOUND_SELECT_BUTTON, D3DXVECTOR3(0, 0, 0), FALSE);
			return  INF_MSGPROC_BREAK;
		}
		if (m_pBtnBUY && TRUE == m_pBtnBUY->OnLButtonUp(pt))
		{
			if (m_nSelectListItem < 0 || m_vecItemInfo.size() < m_nSelectListItem + 1)
			{
				g_pGameMain->m_pInfWindow->AddMsgBox(STRMSG_C_131205_0033, _Q_MARKET_NORMAL_MESSAGE);
			}
			else
			{
				char buf[MAX_PATH] = { 0, };
				char buf02[MAX_PATH] = { 0, };
				wsprintf(buf, "%d", m_vecItemInfo[m_nSelectListItem]->pINFO->MarketInfo.Price);
				MakeCurrencySeparator(buf02, buf, 3, ',');
				if (TC_SPI == m_vecItemInfo[m_nSelectListItem]->pINFO->MarketInfo.MoneyType)
					wsprintf(buf, STRMSG_C_131205_0034, m_vecItemInfo[m_nSelectListItem]->pItem->GetEnchantNumber(), m_vecItemInfo[m_nSelectListItem]->pINFO->MarketInfo.ItemName, buf02);
				else if (TC_WP == m_vecItemInfo[m_nSelectListItem]->pINFO->MarketInfo.MoneyType)
					wsprintf(buf, STRMSG_C_131205_0035, m_vecItemInfo[m_nSelectListItem]->pItem->GetEnchantNumber(), m_vecItemInfo[m_nSelectListItem]->pINFO->MarketInfo.ItemName, buf02);
				else if (TC_CREDITS == m_vecItemInfo[m_nSelectListItem]->pINFO->MarketInfo.MoneyType)
					wsprintf(buf, "+%d %s Would you like to buy? \\n(Price : %s CREDITS) ", m_vecItemInfo[m_nSelectListItem]->pItem->GetEnchantNumber(), m_vecItemInfo[m_nSelectListItem]->pINFO->MarketInfo.ItemName, buf02);

				g_pGameMain->m_pInfWindow->AddMsgBox(buf, _Q_MARKET_BUY_ITEM, 0, 0, 0, 0, NULL);
			}

			if(g_pD3dApp->m_pSound)
				g_pD3dApp->m_pSound->PlayD3DSound(SOUND_SELECT_BUTTON, D3DXVECTOR3(0, 0, 0), FALSE);
			return  INF_MSGPROC_BREAK;
		}
	}
	return INF_MSGPROC_NORMAL;
}

int CINFCityAuction::OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	POINT pt;
	pt.x = LOWORD(lParam);
	pt.y = HIWORD(lParam);
	CheckMouseReverse(&pt);

	POINT ptBkPos;// = GetBkPos();
	ptBkPos.x = _INET_OSR_MARKET_POS_X;
	ptBkPos.y = _INET_OSR_MARKET_POS_Y;

	if (m_pCloseXBtn->OnLButtonDown(pt))
		return INF_MSGPROC_BREAK;

	int nCnt = 0;
	for (nCnt = 0; nCnt < TAB_COUNT; nCnt++)
	{
		if (m_pRankBtn[nCnt] && TRUE == m_pRankBtn[nCnt]->OnLButtonDown(pt))
		{
			// 버튼위에 마우스가 있다.
			return  INF_MSGPROC_BREAK;
		}
	}

	if (m_nCurrentTab == TAB_SELL_ITEM) {

		if (m_pNumEditBox->IsEditMode())
		{
			int nNum = 0;
			m_pNumEditBox->EnableEdit(FALSE);
			CItemInfo* pItemInfo = g_pStoreData->FindItemInInventoryByUniqueNumber(m_pSourceItem->UniqueNumber);
			if (m_pSourceItem)
			{
				if (!pItemInfo)
				{
					if (m_pSourceItem->CurrentCount <= NumEditBoxMaxAndMIN(m_pNumEditBox, m_pSourceItem->CurrentCount))
						return INF_MSGPROC_BREAK;

					g_pStoreData->PutItem((char*)((ITEM_GENERAL*)m_pSourceItem), TRUE);
					pItemInfo = g_pStoreData->FindItemInInventoryByUniqueNumber(m_pSourceItem->UniqueNumber);
					ASSERT_ASSERT(pItemInfo);
					pItemInfo->CopyItemInfo(m_pSourceItem);
					pItemInfo->CurrentCount = 0;
				}
				nNum = NumEditBoxMaxAndMIN(m_pNumEditBox, m_pSourceItem->CurrentCount + pItemInfo->CurrentCount);
				nNum = nNum - m_pSourceItem->CurrentCount;
				InvenToSourceItem(pItemInfo, nNum);
			}
		}
		m_pNumEditBox->EnableEdit(FALSE);

		if (m_pMoneyEditBox->IsEditMode())
		{
			if (m_pSourceItem)
				NumEditBoxMaxAndMIN(m_pMoneyEditBox, MAX_ITEM_COUNTS);
		}

		m_pMoneyEditBox->EnableEdit(FALSE);

		if (1 == m_pMoneyComboBox->LButtonDown(pt))
		{
			m_byMoneySelect = m_pMoneyComboBox->GetSelect();
			return  INF_MSGPROC_BREAK;
		}
		if (TRUE == m_pMaxBtn->OnLButtonDown(pt))
		{
			return INF_MSGPROC_BREAK;
		}

		if (TRUE == m_pNumEditBox->OnLButtonDown(pt))
		{
			if (m_pSourceItem && IS_COUNTABLE_ITEM(m_pSourceItem->Kind))
				m_pNumEditBox->EnableEdit(TRUE);
			return  INF_MSGPROC_BREAK;
		}


		if (TRUE == m_pMoneyEditBox->OnLButtonDown(pt))
		{
			if (m_pSourceItem)
				m_pMoneyEditBox->EnableEdit(TRUE);
			return  INF_MSGPROC_BREAK;
		}

		if (TRUE == m_pUpCntBtn->OnLButtonDown(pt))
		{
			return INF_MSGPROC_BREAK;
		}
		if (TRUE == m_pDownCntBtn->OnLButtonDown(pt))
		{
			return INF_MSGPROC_BREAK;
		}
		if (TRUE == m_pRegisterItemBtn->OnLButtonDown(pt))
		{
			return INF_MSGPROC_BREAK;
		}
	}
	if (m_nCurrentTab == TAB_BUY_ITEM) {
		BOOL bReturn = FALSE;
		for (int i = 0; i < 2; ++i)
		{
			INT nMixCounts = 0;
			char chBuff[256] = { 0, };
			if (TRUE == m_plevelNumEditBox[i]->OnLButtonDown(pt))
			{
				m_pNameEditBox->EnableEdit(FALSE, FALSE);
				m_plevelNumEditBox[i]->EnableEdit(TRUE);
				STRNCPY_MEMSET(chBuff,m_plevelNumEditBox[i]->GetString(),256);	
				//m_plevelNumEditBox[i]->GetString(chBuff, 256);
				if (chBuff[0] == ' ')
				{
					//if (m_pParent)
					//	((CINFTradeItemCenter*)m_pParent)->NumEditBoxChangeCount(m_plevelNumEditBox[i], 1);
				}
				//return  INF_MSGPROC_BREAK;				
				bReturn = TRUE;
			}
			else
				m_plevelNumEditBox[i]->EnableEdit(FALSE);

			NumEditBoxMaxAndMIN(m_plevelNumEditBox[i], MAXLEVEL);

			if (TRUE == m_pEnchantNumEditBox[i]->OnLButtonDown(pt))
			{
				m_pNameEditBox->EnableEdit(FALSE, FALSE);
				m_pEnchantNumEditBox[i]->EnableEdit(TRUE);
					STRNCPY_MEMSET(chBuff,m_pEnchantNumEditBox[i]->GetString(),256);	
				//m_pEnchantNumEditBox[i]->GetString(chBuff, 256);
				if (chBuff[0] == ' ')
				{
					//	if (m_pParent)
							//((CINFTradeItemCenter*)m_pParent)->NumEditBoxChangeCount(m_pEnchantNumEditBox[i], 0);
				}
				//return  INF_MSGPROC_BREAK;				
				bReturn = TRUE;
			}
			else
				m_pEnchantNumEditBox[i]->EnableEdit(FALSE);

			NumEditBoxMaxAndMIN(m_pEnchantNumEditBox[i], MAXENCHANT, FALSE);

		}
		if (bReturn)
			return INF_MSGPROC_BREAK;

		if (TRUE == m_pNameEditBox->OnLButtonDown(pt))
		{
			if (m_pNameEditBox->IsEditMode())
			{
				m_pNameEditBox->BackupTxtString();
			}

			m_pNameEditBox->EnableEdit(TRUE, TRUE);
			return INF_MSGPROC_BREAK;
		}
		m_pNameEditBox->EnableEdit(FALSE, FALSE);

		{
			int nSelPage = -1;
			int nOutPages = m_nMaxPages;
			int nOutPagesMax = m_nMaxPages;

			if (nOutPages >= MAX_PAGES_COUNT)
				nOutPages = MAX_PAGES_COUNT;

			float fAddX = (WORLD_RANK_PAGE_WIDTH * (MAX_PAGES_COUNT - nOutPages)) / 2;
			int nPosY = ptBkPos.y + WORLD_RANK_PAGE_POS_Y - 3;
			if ((nPosY <= pt.y) && (pt.y < (nPosY + 20)))
			{
				for (nCnt = 0; nCnt < nOutPages; nCnt++)
				{
					int nPosX = fAddX + ptBkPos.x + WORLD_RANK_PAGE_POS_X + (nCnt * WORLD_RANK_PAGE_WIDTH);
					if ((nPosX <= pt.x) && (pt.x < (nPosX + WORLD_RANK_PAGE_WIDTH)))
					{
						nSelPage = nCnt;
						if (nSelPage >= nOutPagesMax)
							nSelPage = nOutPagesMax;

						break;
					}
				}
			}

			if (nSelPage != -1 && m_nSelectPage != nSelPage)
			{
				if (!IsRqPossibleStats())
				{
					return INF_MSGPROC_BREAK;
				}
				m_nSelectPage = nSelPage;

				RqItemListFromServer();

				return INF_MSGPROC_BREAK;
			}
		}


		if (TRUE == m_pListTreeCtrl->OnLButtonDown(pt))
		{
			return INF_MSGPROC_BREAK;
		}
		if (IsRqPossibleStats())
		{
			int nLBtnDown = m_pComboGear->LButtonDown(pt);
			if (1 == nLBtnDown)
			{
				RqItemListFromServer();
				// 보이다가 안보이는 상황			
				return  INF_MSGPROC_BREAK;
			}
			nLBtnDown = m_pComboCurrency->LButtonDown(pt);
			if (1 == nLBtnDown)
			{
				RqItemListFromServer();
				// 보이다가 안보이는 상황			
				return  INF_MSGPROC_BREAK;
			}
		}
		if (m_pBtnHover[0] && TRUE == m_pBtnHover[0]->OnLButtonDown(pt))
		{
			return  INF_MSGPROC_BREAK;
		}

		if (m_pBtnHover[1] && TRUE == m_pBtnHover[1]->OnLButtonDown(pt))
		{
			return  INF_MSGPROC_BREAK;
		}

		if (m_pBtnBUY && TRUE == m_pBtnBUY->OnLButtonDown(pt))
		{
			return  INF_MSGPROC_BREAK;
		}
	}
	if (m_nCurrentTab == TAB_FINISHED_ITEM) {
		int nTemp = -1;
		if (pt.x > _INET_OSR_MARKET_POS_X + ITEMLIST_LOCATION_X + 16 + ITEMLIST_WIDTH - 45 && pt.x < _INET_OSR_MARKET_POS_X + ITEMLIST_LOCATION_X + 16 + ITEMLIST_WIDTH)
		{
			if (pt.y > _INET_OSR_MARKET_POS_Y + ITEMLIST_LOCATION_Y - 1 - ITEMLIST_CELL_HEIGHT && pt.y < _INET_OSR_MARKET_POS_Y + ITEMLIST_LOCATION_Y - 1 + ITEMLIST_HEIGHT + ITEMLIST_CELL_HEIGHT)
			{
				int nStartRowY = 0;
				int nEndRowY = 0;
				int nHitNo = -1;
				for (auto el = 0; el < MAX_PER_ONEPAGE + 2; el++)
				{
					nStartRowY = _INET_OSR_MARKET_POS_Y - ITEMLIST_CELL_HEIGHT + ITEMLIST_LOCATION_Y + (el * ITEMLIST_CELL_HEIGHT + 1);
					nEndRowY = _INET_OSR_MARKET_POS_Y - ITEMLIST_CELL_HEIGHT + ITEMLIST_LOCATION_Y + ((el + 1) * ITEMLIST_CELL_HEIGHT + 1);
					if (pt.y > nStartRowY && pt.y < nEndRowY) {
						nHitNo = el + nMylistStartFromId;
						break;
					}
				}
				if (nHitNo > -1 && nHitNo < m_vecMyItemInfo.size())
				{
					MARKET_MY_ITEM_INFO* pMarketItem = NULL;
					pMarketItem = m_vecMyItemInfo[nHitNo];
					if (pMarketItem)
					{
						pMarketItem->byBtnState = BTN_STATUS_DOWN;
					}
				}

				return  INF_MSGPROC_BREAK;
			}
		}
		if (m_pBtnHover[1] && TRUE == m_pBtnHover[1]->OnLButtonDown(pt))
		{
			return  INF_MSGPROC_BREAK;
		}
	}



	return INF_MSGPROC_BREAK;
}
int CINFCityAuction::nRetHit(POINT pt)
{
	int nHitNo = -1;
	if (pt.x > _INET_OSR_MARKET_POS_X + ITEMLIST_LOCATION_X + 16 && pt.x < _INET_OSR_MARKET_POS_X + ITEMLIST_LOCATION_X + 16 + 45)
	{
		if (pt.y > _INET_OSR_MARKET_POS_Y + ITEMLIST_LOCATION_Y - 1 && pt.y < _INET_OSR_MARKET_POS_Y + ITEMLIST_LOCATION_Y - 1 + ITEMLIST_HEIGHT + ITEMLIST_CELL_HEIGHT)
		{
			int nStartRowY = 0;
			int nEndRowY = 0;

			for (auto el = 0; el < MAX_PER_ONEPAGE + 1; el++)
			{
				nStartRowY = _INET_OSR_MARKET_POS_Y + ITEMLIST_LOCATION_Y + (el * ITEMLIST_CELL_HEIGHT + 1);
				nEndRowY = _INET_OSR_MARKET_POS_Y + ITEMLIST_LOCATION_Y + ((el + 1) * ITEMLIST_CELL_HEIGHT + 1);
				if (pt.y > nStartRowY && pt.y < nEndRowY) {
					nHitNo = el;
					return nHitNo;
				}
			}
		}
	}
	return nHitNo;
}
int CINFCityAuction::nRetMyHit(POINT pt)
{
	int nHitNo = -1;
	if (pt.x > _INET_OSR_MARKET_POS_X + ITEMLIST_LOCATION_X + 16 && pt.x < _INET_OSR_MARKET_POS_X + ITEMLIST_LOCATION_X + 16 + 45)
	{
		if (pt.y > _INET_OSR_MARKET_POS_Y -ITEMLIST_CELL_HEIGHT + ITEMLIST_LOCATION_Y - 1 && pt.y < _INET_OSR_MARKET_POS_Y + ITEMLIST_LOCATION_Y - 1 + ITEMLIST_HEIGHT + ITEMLIST_CELL_HEIGHT)
		{
			int nStartRowY = 0;
			int nEndRowY = 0;

			for (auto el = 0; el < MAX_PER_ONEPAGE + 1; el++)
			{
				nStartRowY = _INET_OSR_MARKET_POS_Y - ITEMLIST_CELL_HEIGHT + ITEMLIST_LOCATION_Y + (el * ITEMLIST_CELL_HEIGHT + 1);
				nEndRowY = _INET_OSR_MARKET_POS_Y - ITEMLIST_CELL_HEIGHT + ITEMLIST_LOCATION_Y + ((el + 1) * ITEMLIST_CELL_HEIGHT + 1);
				if (pt.y > nStartRowY && pt.y < nEndRowY) {
					nHitNo = el + nMylistStartFromId;
					return nHitNo;
				}
			}
		}
	}
	return nHitNo;
}
void CINFCityAuction::RemoveColor(CItemInfo* pRefItemInfo, char* pChar)
{
	int  nCnt = 0;
	BOOL bContinue = FALSE;
	ITEM* m_pRefITEM = pRefItemInfo->GetItemInfo();
	int len = strlen(m_pRefITEM->ItemName);
	for (int i = 0; i < len; ++i)
	{
		if (m_pRefITEM->ItemName[i] == '\\' || bContinue)
		{
			bContinue ^= TRUE;
			continue;
		}
		else
		{
			pChar[nCnt] = m_pRefITEM->ItemName[i];
			nCnt++;
		}
	}
};
void CINFCityAuction::GetItemName(CItemInfo* pRefItemInfo, char* pName, BOOL bColor)
{
	CItemInfo* m_pRefItemInfo = pRefItemInfo;
	ITEM* m_pRefITEM = pRefItemInfo->GetItemInfo();
	RARE_ITEM_INFO* m_pRefPrefixRareInfo = m_pRefItemInfo->GetPrefixRareInfo();
	RARE_ITEM_INFO* m_pRefSuffixRareInfo = m_pRefItemInfo->GetSuffixRareInfo();
	CParamFactor* m_pRefEnchant = m_pRefEnchant = m_pRefItemInfo->GetEnchantParamFactor();
	BOOL			m_bEnableItem = g_pGameMain->GetINFItemInfo()->IsEnableItem(m_pRefItemInfo->GetRealItemInfo());

	char cTempName[ITEMINFO_ITEM_FULL_NAME] = { 0, };
	memset(pName, 0x00, ITEMINFO_ITEM_FULL_NAME);

	if (m_pRefPrefixRareInfo)
	{
		if (bColor)
			wsprintf(pName, "\\g%s\\g", m_pRefPrefixRareInfo->Name);
		else
			wsprintf(pName, "%s", m_pRefPrefixRareInfo->Name);
	}
	if (m_bEnableItem)
	{
		if (g_pGameMain->GetINFItemInfo()->IsStringColor(m_pRefITEM->ItemName))
		{
			if (bColor)
				wsprintf(pName, "%s %s", pName, m_pRefITEM->ItemName);
			else
			{
				RemoveColor(pRefItemInfo, cTempName);
				wsprintf(pName, "%s %s", pName, cTempName);
			}

		}
		else if (m_pRefPrefixRareInfo || m_pRefSuffixRareInfo)
		{
			if (m_pRefEnchant)
			{
				if (bColor)
					wsprintf(pName, "%s \\e%s\\e", pName, m_pRefITEM->ItemName);
				else
					wsprintf(pName, "%s %s", pName, m_pRefITEM->ItemName);
			}
			else
			{
				if (bColor)
					wsprintf(pName, "%s \\g%s\\g", pName, m_pRefITEM->ItemName);
				else
					wsprintf(pName, "%s %s", pName, m_pRefITEM->ItemName);
			}
		}
		else
		{
			if (m_pRefEnchant)
			{
				if (bColor)
					wsprintf(pName, "%s \\e%s\\e", pName, m_pRefITEM->ItemName);
				else
					wsprintf(pName, "%s %s", pName, m_pRefITEM->ItemName);
			}
			else
			{
				wsprintf(pName, "%s %s", pName, m_pRefITEM->ItemName);
			}
		}
	}
	else
	{
		if (bColor)
			wsprintf(pName, "%s \\r%s\\r", pName, m_pRefITEM->ItemName);
		else
		{
			if (g_pGameMain->GetINFItemInfo()->IsStringColor(m_pRefITEM->ItemName))
			{
				RemoveColor(pRefItemInfo, cTempName);
				wsprintf(pName, "%s %s", pName, cTempName);
			}
			else
				wsprintf(pName, "%s %s", pName, m_pRefITEM->ItemName);
		}

	}
	if (m_pRefSuffixRareInfo)
	{
		if (bColor)
			wsprintf(pName, "%s \\g%s\\g", pName, m_pRefSuffixRareInfo->Name);
		else
			wsprintf(pName, "%s %s", pName, m_pRefSuffixRareInfo->Name);
	}

}
int CINFCityAuction::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	POINT pt;
	pt.x = LOWORD(lParam);
	pt.y = HIWORD(lParam);
	CheckMouseReverse(&pt);
	ptMouse = pt;
	m_pCloseXBtn->OnMouseMove(pt);
	int nCnt = 0;
	for (nCnt = 0; nCnt < TAB_COUNT; nCnt++)
	{
		if (m_pRankBtn[nCnt])
		{
			m_pRankBtn[nCnt]->OnMouseMove(pt);
		}
	}
	if (m_nCurrentTab == TAB_SELL_ITEM) {

		if (pt.x > _INET_OSR_MARKET_POS_X + 691 && pt.x < _INET_OSR_MARKET_POS_X + 28 + 691 &&
			pt.y > _INET_OSR_MARKET_POS_Y + 185 && pt.y < _INET_OSR_MARKET_POS_Y + 28 + 185)
		{
			m_nCurerntInfo = m_pSourceItem;
		}
		else
		{
			m_nCurerntInfo = NULL;
		}

		m_pMoneyComboBox->MouseMove(pt);
		if (TRUE == m_pMaxBtn->OnMouseMove(pt))
		{

			return INF_MSGPROC_BREAK;
		}

		if (TRUE == m_pUpCntBtn->OnMouseMove(pt))
		{
			return INF_MSGPROC_BREAK;
		}
		if (TRUE == m_pDownCntBtn->OnMouseMove(pt))
		{
			return INF_MSGPROC_BREAK;
		}
		if (TRUE == m_pRegisterItemBtn->OnMouseMove(pt))
		{
			return INF_MSGPROC_BREAK;
		}
	}
	if (m_nCurrentTab == TAB_BUY_ITEM) {
		if (m_pBtnHover[0])
			m_pBtnHover[0]->OnMouseMove(pt);

		if (m_pBtnHover[1])
			m_pBtnHover[1]->OnMouseMove(pt);

		if (m_pBtnBUY)
			m_pBtnBUY->OnMouseMove(pt);

		m_pComboGear->MouseMove(pt);
		m_pComboCurrency->MouseMove(pt);
		m_pListTreeCtrl->OnMouseMove(pt);

		int nHitNo = nRetHit(pt);
		if (nHitNo > -1 && nHitNo < m_vecItemInfo.size())
			m_nCurerntInfo = m_vecItemInfo[nHitNo]->pItem;
		else
			m_nCurerntInfo = NULL;
	}
	if (m_nCurrentTab == TAB_FINISHED_ITEM) {
		int nTemp = -1;
		if (pt.x > _INET_OSR_MARKET_POS_X + ITEMLIST_LOCATION_X + 16 + ITEMLIST_WIDTH - 45 && pt.x < _INET_OSR_MARKET_POS_X + ITEMLIST_LOCATION_X + 16 + ITEMLIST_WIDTH)
		{
			if (pt.y > _INET_OSR_MARKET_POS_Y + ITEMLIST_LOCATION_Y - 1 - ITEMLIST_CELL_HEIGHT && pt.y < _INET_OSR_MARKET_POS_Y + ITEMLIST_LOCATION_Y - 1 + ITEMLIST_HEIGHT + ITEMLIST_CELL_HEIGHT)
			{
				int nStartRowY = 0;
				int nEndRowY = 0;
				int nHitNo = -1;
				for (auto el = 0; el < MAX_PER_ONEPAGE + 2; el++)
				{
					nStartRowY = _INET_OSR_MARKET_POS_Y - ITEMLIST_CELL_HEIGHT + ITEMLIST_LOCATION_Y + (el * ITEMLIST_CELL_HEIGHT + 1);
					nEndRowY = _INET_OSR_MARKET_POS_Y - ITEMLIST_CELL_HEIGHT + ITEMLIST_LOCATION_Y + ((el + 1) * ITEMLIST_CELL_HEIGHT + 1);
					if (pt.y > nStartRowY && pt.y < nEndRowY) {
						nHitNo = el + nMylistStartFromId;
						break;
					}
				}
				if (nHitNo > -1 && nHitNo < m_vecMyItemInfo.size())
				{
					MARKET_MY_ITEM_INFO* pMarketItem = NULL;
					pMarketItem = m_vecMyItemInfo[nHitNo];
					if (pMarketItem)
					{
						if (pMarketItem->byBtnState != BTN_STATUS_DOWN)
							pMarketItem->byBtnState = BTN_STATUS_UP;

						return INF_MSGPROC_BREAK;
					}
				}
			}
		}

		if (m_pBtnHover[1])
			m_pBtnHover[1]->OnMouseMove(pt);

		int nHitNo = nRetMyHit(pt);
		if (nHitNo > -1 && nHitNo < m_vecMyItemInfo.size())
			m_nCurerntInfo = m_vecMyItemInfo[nHitNo]->pItem;
		else
			m_nCurerntInfo = NULL;	
	}
	return INF_MSGPROC_NORMAL;
}

void CINFCityAuction::UpdateBtnPos(int nWidth, int nHeight)
{
	//CINFDefaultWnd::UpdateBtnPos(nWidth, nHeight);

	POINT ptBkPos;// = GetBkPos();
	ptBkPos.x = _INET_OSR_MARKET_POS_X;
	ptBkPos.y = _INET_OSR_MARKET_POS_Y;

	m_pRankBtn[TAB_BUY_ITEM]->SetBtnPosition(ptBkPos.x + 139, ptBkPos.y + 60);
	m_pRankBtn[TAB_SELL_ITEM]->SetBtnPosition(ptBkPos.x + 390, ptBkPos.y + 60);
	m_pRankBtn[TAB_FINISHED_ITEM]->SetBtnPosition(ptBkPos.x + 634, ptBkPos.y + 60);

	int nPosX, nPosY;
	nPosX = nPosY = 0;
	{
		// 장치
		int nMainWidth, nMainHeight;
		int nEleWidth, nEleHeight;
		int nElePosX, nElePosY;
		nElePosX = nElePosY = 0;
		nMainWidth = GEAR_COMBO_DEVICE_MAIN_WIDTH;
		nMainHeight = GEAR_COMBO_DEVICE_MAIN_HEIGHT;
		nEleWidth = GEAR_COMBO_DEVICE_ELE_WIDTH;
		nEleHeight = GEAR_COMBO_DEVICE_ELE_HEIGHT;

		nPosX = ptBkPos.x + GEAR_COMBO_DEVICE_X;
		nPosY = ptBkPos.y + GEAR_COMBO_DEVICE_Y;

		nElePosX = nPosX;
		nElePosY = nPosY + nMainHeight;

		m_pComboGear->SetMainArea(nPosX, nPosY, nMainWidth, nMainHeight);
		m_pComboGear->SetElementArea(nElePosX, nElePosY, nEleWidth, nEleHeight);
		m_pComboGear->SetBGPos(nElePosX + 6, nElePosY,
			nEleWidth, nEleHeight);
		
		nMainWidth = GEAR_COMBO_CUR_MAIN_WIDTH;
		nMainHeight = GEAR_COMBO_CUR_MAIN_HEIGHT;
		nEleWidth = GEAR_COMBO_CUR_ELE_WIDTH;
		nEleHeight = GEAR_COMBO_CUR_ELE_HEIGHT;

		nPosX = ptBkPos.x + GEAR_COMBO_CUR_X;
		nPosY = ptBkPos.y + GEAR_COMBO_CUR_Y;

		nElePosX = nPosX;
		nElePosY = nPosY + nMainHeight;

		m_pComboCurrency->SetMainArea(nPosX, nPosY, nMainWidth, nMainHeight);
		m_pComboCurrency->SetElementArea(nElePosX, nElePosY, nEleWidth, nEleHeight);
		m_pComboCurrency->SetBGPos(nElePosX + 6, nElePosY,
			nEleWidth, nEleHeight);
	}

	OnClickRankBtn(m_nCurrentTab);

	if (m_plevelNumEditBox[0])
		m_plevelNumEditBox[0]->SetPos(_INET_OSR_MARKET_POS_X + MINLEVEL_NUMEDITBOX_LOCATION_X, _INET_OSR_MARKET_POS_Y + NUMEDITBOX_LOCATION_Y);

	if (m_plevelNumEditBox[1])
		m_plevelNumEditBox[1]->SetPos(_INET_OSR_MARKET_POS_X + MAXLEVEL_NUMEDITBOX_LOCATION_X, _INET_OSR_MARKET_POS_Y + NUMEDITBOX_LOCATION_Y);

	if (m_pEnchantNumEditBox[0])
		m_pEnchantNumEditBox[0]->SetPos(_INET_OSR_MARKET_POS_X + MINENCHANT_NUMEDITBOX_LOCATION_X, _INET_OSR_MARKET_POS_Y + NUMEDITBOX_LOCATION_Y);

	if (m_pEnchantNumEditBox[1])
		m_pEnchantNumEditBox[1]->SetPos(_INET_OSR_MARKET_POS_X + MAXENCHANT_NUMEDITBOX_LOCATION_X, _INET_OSR_MARKET_POS_Y + NUMEDITBOX_LOCATION_Y);

	if (m_pNameEditBox)
	{
		int nPosX, nPosY;
		nPosX = _INET_OSR_MARKET_POS_X + NAMEEDITBOX_LOCATION_X;
		nPosY = _INET_OSR_MARKET_POS_Y + NAMEEDITBOX_LOCATION_Y;
		m_pNameEditBox->SetPos(nPosX, nPosY);
	}


}

void CINFCityAuction::OnClickRankBtn(int i_nSelIdx)
{
	int nOldSelIdx = m_nCurrentTab;

	m_pRankBtn[m_nCurrentTab]->PushButton(FALSE);
	m_nCurrentTab = i_nSelIdx;
	m_pRankBtn[m_nCurrentTab]->PushButton(TRUE);

	if (nOldSelIdx != m_nCurrentTab)
	{
		if (!IsRqPossibleStats())
		{
			m_pRankBtn[m_nCurrentTab]->PushButton(FALSE);
			m_nCurrentTab = nOldSelIdx;
			m_pRankBtn[m_nCurrentTab]->PushButton(TRUE);
			return;
		}
		//RqWorldRankInfo(); // 랭킹정보 요청
	}
	if (m_nCurrentTab == TAB_SELL_ITEM) {
		POINT ptItem, ptEq;
		ptItem.x = _INET_OSR_MARKET_POS_X + 190;
		ptItem.y = _INET_OSR_MARKET_POS_Y + 340;
		ptEq.x = _INET_OSR_MARKET_POS_X + 260;
		ptEq.y = _INET_OSR_MARKET_POS_Y + 100;
		g_pGameMain->m_pInven->ShowInven(&ptItem, &ptEq, FALSE, TRUE);
		g_pGameMain->m_pInven->SetTradeItemCenterState(TRUE);
	}
	else
	{
		g_pGameMain->m_pInven->ShowInven(NULL, NULL);
		g_pGameMain->m_pInven->SetTradeItemCenterState(FALSE);
	}
	
}

void CINFCityAuction::RqItemListFromServer()
{
//TODO
	MSG_FC_MARKET_PAGING_REQUEST sMsg;
	memset(&sMsg, 0x00, sizeof(sMsg));
	sMsg.SelectPage = m_nSelectPage + nFirstPage;
	sMsg.MarketUID = 0;
	g_pFieldWinSocket->SendMsg(T_FC_MARKET_PAGING_REQUEST, (char*)&sMsg, sizeof(sMsg));
}

void CINFCityAuction::InitInfo()// 초기화
{
	m_nCurrentTab = TAB_BUY_ITEM;
	m_nSelectPage = 0;
}

void CINFCityAuction::RenderSelectPage()
{
	POINT ptBkPos;
	ptBkPos.x = _INET_OSR_MARKET_POS_X;
	ptBkPos.y = _INET_OSR_MARKET_POS_Y;
	int nCnt = 0;
	
	char chTmp[32];
	DWORD dwColor = GUI_FONT_COLOR_W;

	int nOutPages = m_nMaxPages;
	int nOutPagesMax = m_nMaxPages;

	if(nOutPages >= MAX_PAGES_COUNT)
		nOutPages = MAX_PAGES_COUNT;

	float fAddX = (WORLD_RANK_PAGE_WIDTH * (MAX_PAGES_COUNT - nOutPages)) / 2;
	for (nCnt = 0; nCnt < nOutPages; nCnt++)
	{
		dwColor = GUI_FONT_COLOR_W;
		if (nCnt == m_nSelectPage)
		{
			dwColor = GUI_FONT_COLOR_YM;
		}

		if(nCnt + nFirstPage >= nOutPagesMax)
			wsprintf(chTmp, "%2d", nOutPagesMax);
		else
			wsprintf(chTmp, "%2d", nCnt + nFirstPage);

		m_pFontPage[nCnt]->DrawText(fAddX + ptBkPos.x + WORLD_RANK_PAGE_POS_X + (nCnt * WORLD_RANK_PAGE_WIDTH),
			ptBkPos.y + WORLD_RANK_PAGE_POS_Y,
			dwColor,
			chTmp);
	}
	dwColor = GUI_FONT_COLOR_W;
	wsprintf(chTmp, "Total: %d items", m_nItemCountTotal);
	m_pFontRankInfo->DrawText(ptBkPos.x + 715,
		ptBkPos.y + WORLD_RANK_PAGE_POS_Y,
		dwColor,
		chTmp);
}

BOOL CINFCityAuction::IsRqPossibleStats()
{
	DWORD dwCurrentTime = timeGetTime();
	DWORD dwCap = dwCurrentTime - m_dwSendTermTime;
	if (dwCap < INTERVAL_SERVICE_RQ_TIME)
	{
		return FALSE;
	}
	m_dwSendTermTime = dwCurrentTime;
	return TRUE;
}

void CINFCityAuction::RenderCategoryPath(char* strInputSearch, bool bSearch/* = false*/)
{
	char chCategoryPath[1024];
	if (bSearch)
	{	
		sprintf(chCategoryPath,"Search result for %s", strInputSearch);
		m_pFontCategoryPath->DrawText(_INET_OSR_MARKET_POS_X + CATEGORYPATH_LOCATION_X, _INET_OSR_MARKET_POS_Y + CATEGORYPATH_LOCATION_Y, CATEGORYPATH_COLOR, chCategoryPath, 0L);
		return;
	}

	PrintCategoryPath(chCategoryPath);

	m_pFontCategoryPath->DrawText(_INET_OSR_MARKET_POS_X + CATEGORYPATH_LOCATION_X, _INET_OSR_MARKET_POS_Y + CATEGORYPATH_LOCATION_Y, CATEGORYPATH_COLOR, chCategoryPath, 0L);

	if (chCategoryPath[0] != NULL)
	{
		m_pCategoryPathArrowImage->Move(_INET_OSR_MARKET_POS_X + CATEGORYPATH_ICON_LOCATION_X, _INET_OSR_MARKET_POS_Y + CATEGORYPATH_ICON_LOCATION_Y);
		m_pCategoryPathArrowImage->Render();
	}
}

void CINFCityAuction::PrintCategoryPath(char* chCategoryPath)
{
	memset(chCategoryPath, 0x00, sizeof(chCategoryPath));

	_PrintCategoryPath(chCategoryPath, m_nSelectedCategoryId);
}

void CINFCityAuction::_PrintCategoryPath(char* chCategoryPath, int nodeId)
{
	if (-1 != nodeId)
	{
		int parentId = m_pListTreeCtrl->FindParentIdById(nodeId);
		_PrintCategoryPath(chCategoryPath, parentId);

		char* chNodeCategoryPath = m_pListTreeCtrl->GetItemTitleById(nodeId);

		if (chCategoryPath[0] != NULL)
		{
			strcat(chCategoryPath, " > ");
		}
		strcat(chCategoryPath, chNodeCategoryPath);
	}
}

void CINFCityAuction::ItemVecAdd(MSG_FC_MARKET_BASE_INFO_OK* pMsg)
{
	MARKETITEM_INFO* pTemp = new MARKETITEM_INFO;
	pTemp->pINFO = new MSG_FC_MARKET_BASE_INFO_OK;
	ITEM_GENERAL* pITEMG = new ITEM_GENERAL;

	memset(pITEMG, 0x00, sizeof(ITEM_GENERAL));
	memset(pTemp->pINFO, 0x00, sizeof(MSG_FC_MARKET_BASE_INFO_OK));

	memcpy(pTemp->pINFO, pMsg, sizeof(MSG_FC_MARKET_BASE_INFO_OK));

	pITEMG->PrefixCodeNum = pMsg->MarketInfo.PrefixCodeNum;
	pITEMG->SuffixCodeNum = pMsg->MarketInfo.SuffixCodeNum;
	pITEMG->UniqueNumber = pMsg->MarketInfo.ItemUID;
	pITEMG->ShapeItemNum = pMsg->MarketInfo.ShapeItemNum;

	pITEMG->UniqueNumber = pMsg->MarketInfo.ItemUID;
	pITEMG->ItemNum = pMsg->MarketInfo.ItemNum;
	pITEMG->CurrentCount = pMsg->MarketInfo.ItemCount;

	pITEMG->PrefixCodeNum = pMsg->MarketInfo.PrefixCodeNum;
	pITEMG->SuffixCodeNum = pMsg->MarketInfo.SuffixCodeNum;
	pITEMG->ShapeItemNum = pMsg->MarketInfo.ShapeItemNum;
	pITEMG->ColorCode = pMsg->MarketInfo.ColorCode;

	pTemp->pItem = new CItemInfo(pITEMG);
	if (pTemp->pItem) {
		for (int i = 0; i < 8; ++i)
		{
			for (int j = 0; j < pMsg->MarketInfo.Enchant[i].Count; ++j)
			{
				pTemp->pItem->AddEnchantItem(pMsg->MarketInfo.Enchant[i].ItemNum);
			}
		}
		if (pTemp->pItem->ItemInfo) {
			pTemp->pItem->Kind = pTemp->pItem->ItemInfo->Kind;
			m_vecItemInfo.push_back(pTemp);
		}
	}
}
void CINFCityAuction::VecItemInfoClear()
{
	if (0 < m_vecItemInfo.size())
	{
		vector<MARKETITEM_INFO*>::iterator it;
		for (it = m_vecItemInfo.begin(); it != m_vecItemInfo.end();)
		{
			MARKETITEM_INFO* pMarketItem = (*it);
			if (pMarketItem)
			{
				SAFE_DELETE(pMarketItem->pINFO);
				SAFE_DELETE(pMarketItem->pItem);
				SAFE_DELETE(pMarketItem);
				it = m_vecItemInfo.erase(it);
			}
			else
				it++;
		}
		if (m_vecItemInfo.empty() == false)
			m_vecItemInfo.clear();
	}
}
void CINFCityAuction::VecMyItemInfoClear()
{
	if (0 < m_vecMyItemInfo.size())
	{
		vector<MARKET_MY_ITEM_INFO*>::iterator it;
		for (it = m_vecMyItemInfo.begin(); it != m_vecMyItemInfo.end();)
		{
			MARKET_MY_ITEM_INFO* pMarketItem = (*it);
			if (pMarketItem)
			{
				SAFE_DELETE(pMarketItem->pINFO);
				SAFE_DELETE(pMarketItem->pItem);
				SAFE_DELETE(pMarketItem);
				it = m_vecMyItemInfo.erase(it);
			}
			else
				it++;
		}
		if (m_vecMyItemInfo.empty() == false)
			m_vecMyItemInfo.clear();
	}
}
void CINFCityAuction::MinMaxValue(BYTE* nMin, BYTE* nMax, CINFNumEditBox* pMinEditBox01, CINFNumEditBox* pMaxEditBox02, int nMaxCount)
{
	BYTE nTemp = 0;

	*nMin = (BYTE)NumEditBoxMaxAndMIN(pMinEditBox01, nMaxCount);
	*nMax = (BYTE)NumEditBoxMaxAndMIN(pMaxEditBox02, nMaxCount);

	if (*nMin && *nMax)
	{
		if (*nMin > *nMax)
		{
			nTemp = *nMin;
			*nMin = *nMax;
			*nMax = nTemp;
		}
	}
	else if (*nMax == 0 && *nMin)
	{
		*nMax = nMaxCount;
	}
}
void CINFCityAuction::Search()
{
	/*if (!IsRqPossibleStats())
		return;*/

	nFirstPage = 1;
	m_nSelectListItem = -1;

	m_nSelectedCategoryId = m_pListTreeCtrl->FindNodeIdBySelectedItem();

	MSG_FC_MARKET_SEARCH_REQUEST sMsg;
	memset(&sMsg, 0x00, sizeof(sMsg));

	sMsg.Kind = m_pListTreeCtrl->FindKindBySelectedItem();

	MinMaxValue(&sMsg.LevelMin, &sMsg.LevelMax, m_plevelNumEditBox[0], m_plevelNumEditBox[1], MAXLEVEL);
	MinMaxValue(&sMsg.EnchantMin, &sMsg.EnchantMax, m_pEnchantNumEditBox[0], m_pEnchantNumEditBox[1], MAXENCHANT);


	INT nTemp = m_pComboGear->GetSelect();

	if (0 == nTemp)
		sMsg.ItemGear = UNITKIND_ALL_MASK;
	else if (1 == nTemp)
		sMsg.ItemGear = UNITKIND_BGEAR_MASK;
	else if (2 == nTemp)
		sMsg.ItemGear = UNITKIND_MGEAR_MASK;
	else if (3 == nTemp)
		sMsg.ItemGear = UNITKIND_IGEAR_MASK;
	else if (4 == nTemp)
		sMsg.ItemGear = UNITKIND_AGEAR_MASK;

	sMsg.MoneyType = m_pComboCurrency->GetSelect();
	STRNCPY_MEMSET(sMsg.Name,m_pNameEditBox->GetString(),MARKET_ITEM_FULL_NAME);	
	//m_pNameEditBox->GetString(sMsg.Name, MARKET_ITEM_FULL_NAME);

	g_pFieldWinSocket->SendMsg(T_FC_MARKET_SEARCH_REQUEST, (char*)&sMsg, sizeof(sMsg));
}
BOOL CINFCityAuction::GetRemainTime(ATUM_DATE_TIME regTime, char* buf)
{
	ATUM_DATE_TIME curServerTime = GetServerDateTime();
	regTime.AddDateTime(0, 0, 0, 0, m_nSellingTime);

	if (curServerTime > regTime)
	{
		wsprintf(buf, "Expired");	// "\\r기간만료\\r"
		return FALSE;
	}
	else
	{
		int nRemainSecond = (regTime.GetTimeInSeconds() - curServerTime.GetTimeInSeconds());

		int m_nDay = (nRemainSecond) / 86400;
		int m_nHour = (nRemainSecond) % 86400 / 3600;
		int m_nMin = (nRemainSecond) % 86400 % 3600 / 60;
		int m_nSec = (nRemainSecond) % 86400 % 3600 % 60 / 1;

		if (m_nDay)
		{
			wsprintf(buf, STRMSG_C_131205_0041, m_nDay);				// "%d일"
		}
		else if ((m_nDay == NULL) && m_nHour)
		{
			wsprintf(buf, STRMSG_C_131205_0042, m_nHour);	// "%d시간"
		}
		else if ((m_nDay == NULL) && (m_nHour == NULL) && m_nMin)
		{
			wsprintf(buf, STRMSG_C_131205_0043, m_nMin);			// "%d분"
		}
		else if ((m_nDay == NULL) && (m_nHour == NULL) && (m_nMin == NULL) && m_nSec)
		{
			wsprintf(buf, STRMSG_C_131205_0044);				// "1분이하"
		}
	}
	return TRUE;
}

void CINFCityAuction::MyListRender()
{
	int nCnt = 0;
	char cList[512] = { 0, };
	char cTemp[512] = { 0, };
	CINFImage* pImg = NULL;

	int nMaxPerPage = ITEMLIST_ITEMNUM;

	vector<MARKET_MY_ITEM_INFO*>::iterator it;
	for (it = m_vecMyItemInfo.begin(); it != m_vecMyItemInfo.end(); it++)
	{
		if (nCnt >= nMylistStartFromId && nCnt < nMylistStartFromId + ITEMLIST_ITEMNUM) {
			MARKET_MY_ITEM_INFO* pMarketItem = (*it);

			if (!pMarketItem)
				return;

			m_pItemSelectedRow->Move(_INET_OSR_MARKET_POS_X + ITEMLIST_LOCATION_X + 16, _INET_OSR_MARKET_POS_Y + ITEMLIST_LOCATION_Y + (nCnt - nMylistStartFromId) * (ITEMLIST_CELL_HEIGHT + 1) - 2 - ITEMLIST_CELL_HEIGHT - 2);
			if (nCnt == m_nSelectMyListItem)
				m_pItemSelectedRow->Render();

			//wsprintf(cList, "\\e+%d\\e", pMarketItem->pItem->GetEnchantNumber());
			if (pMarketItem->pINFO->MarketInfo.MarketState == MARKET_STATE_SELL) wsprintf(cList, "\\e+%d\\e", pMarketItem->pItem->GetEnchantNumber());
			else if (pMarketItem->pINFO->MarketInfo.MarketState == MARKET_STATE_SELL_DONE) wsprintf(cList, "\\y+%d\\y", pMarketItem->pItem->GetEnchantNumber());
			else if (pMarketItem->pINFO->MarketInfo.MarketState == MARKET_STATE_TIME_OUT) wsprintf(cList, "\\r+%d\\r", pMarketItem->pItem->GetEnchantNumber());
			SIZE sSize = m_pFontTable[0][0]->GetStringSize(cList);
			m_pFontTable[0][0]->DrawText(_INET_OSR_MARKET_POS_X + ITEMLIST_ENCHANT_LOCATION_X + ITEMLIST_LEVEL_WIDTH / 2 - sSize.cx / 2,
				_INET_OSR_MARKET_POS_Y + ITEMLIST_LOCATION_Y - ITEMLIST_CELL_HEIGHT - 2 + ITEMLIST_CELL_HEIGHT / 2 - sSize.cy / 2 + (nCnt - nMylistStartFromId) * (ITEMLIST_CELL_HEIGHT + 1), GUI_FONT_COLOR, cList);

		//	wsprintf(cList, "%s", pMarketItem->pINFO->MarketInfo.ItemName);
			if (pMarketItem->pINFO->MarketInfo.MarketState == MARKET_STATE_SELL) wsprintf(cList, "%s", pMarketItem->pINFO->MarketInfo.ItemName);
			else if (pMarketItem->pINFO->MarketInfo.MarketState == MARKET_STATE_SELL_DONE) wsprintf(cList, "\\y%s\\y", pMarketItem->pINFO->MarketInfo.ItemName);
			else if (pMarketItem->pINFO->MarketInfo.MarketState == MARKET_STATE_TIME_OUT) wsprintf(cList, "\\r%s\\r", pMarketItem->pINFO->MarketInfo.ItemName);

			sSize = m_pFontTable[0][1]->GetStringSize(cList);
			m_pFontTable[0][1]->DrawText(_INET_OSR_MARKET_POS_X + ITEMLIST_NAME_LOCATION_X + ITEMLIST_NAME_WIDTH / 2 - sSize.cx / 2,
				_INET_OSR_MARKET_POS_Y + ITEMLIST_LOCATION_Y - ITEMLIST_CELL_HEIGHT - 2 + ITEMLIST_CELL_HEIGHT / 2 - sSize.cy / 2 + (nCnt - nMylistStartFromId) * (ITEMLIST_CELL_HEIGHT + 1), GUI_FONT_COLOR, cList);

			if (pMarketItem->pINFO->MarketInfo.MarketState == MARKET_STATE_SELL) wsprintf(cList, "%d", pMarketItem->pINFO->MarketInfo.ItemLevel);
			else if (pMarketItem->pINFO->MarketInfo.MarketState == MARKET_STATE_SELL_DONE) wsprintf(cList, "\\y%d\\y", pMarketItem->pINFO->MarketInfo.ItemLevel);
			else if (pMarketItem->pINFO->MarketInfo.MarketState == MARKET_STATE_TIME_OUT) wsprintf(cList, "\\r%d\\r", pMarketItem->pINFO->MarketInfo.ItemLevel);

			sSize = m_pFontTable[0][2]->GetStringSize(cList);
			m_pFontTable[0][2]->DrawText(_INET_OSR_MARKET_POS_X + ITEMLIST_LEVEL_LOCATION_X + ITEMLIST_LEVEL_WIDTH / 2 - sSize.cx / 2,
				_INET_OSR_MARKET_POS_Y + ITEMLIST_LOCATION_Y - ITEMLIST_CELL_HEIGHT - 2 + ITEMLIST_CELL_HEIGHT / 2 - sSize.cy / 2 + (nCnt - nMylistStartFromId) * (ITEMLIST_CELL_HEIGHT + 1), GUI_FONT_COLOR, cList);

			//GetRemainTime(pMarketItem->pINFO->MarketInfo.RegistTime, cList);
			if (pMarketItem->pINFO->MarketInfo.MarketState == MARKET_STATE_SELL) GetRemainTime(pMarketItem->pINFO->MarketInfo.RegistTime, cList);
			else if (pMarketItem->pINFO->MarketInfo.MarketState == MARKET_STATE_SELL_DONE) wsprintf(cList, STRMSG_C_131205_0045);
			else if (pMarketItem->pINFO->MarketInfo.MarketState == MARKET_STATE_TIME_OUT) wsprintf(cList, STRMSG_C_131205_0046);
			sSize = m_pFontTable[0][3]->GetStringSize(cList);
			m_pFontTable[0][3]->DrawText(_INET_OSR_MARKET_POS_X + ITEMLIST_REGISTTIME_LOCATION_X + ITEMLIST_REGISTTIME_WIDTH / 2 - sSize.cx / 2,
				_INET_OSR_MARKET_POS_Y + ITEMLIST_LOCATION_Y - ITEMLIST_CELL_HEIGHT - 2 + ITEMLIST_CELL_HEIGHT / 2 - sSize.cy / 2 + (nCnt - nMylistStartFromId) * (ITEMLIST_CELL_HEIGHT + 1), GUI_FONT_COLOR, cList);

			char cTempChar[5] = { 0, };
			if (pMarketItem->pINFO->MarketInfo.Price < 10000)
				wsprintf(cTempChar, "\\w");
			else if (pMarketItem->pINFO->MarketInfo.Price >= 10000 && pMarketItem->pINFO->MarketInfo.Price < 100000)
				wsprintf(cTempChar, "\\c");
			else if (pMarketItem->pINFO->MarketInfo.Price >= 100000 && pMarketItem->pINFO->MarketInfo.Price < 1000000)
				wsprintf(cTempChar, "\\g");
			else if (pMarketItem->pINFO->MarketInfo.Price >= 1000000 && pMarketItem->pINFO->MarketInfo.Price < 10000000)
				wsprintf(cTempChar, "\\y");
			else if (pMarketItem->pINFO->MarketInfo.Price >= 10000000 && pMarketItem->pINFO->MarketInfo.Price < 100000000)
				wsprintf(cTempChar, "\\m");
			else if (pMarketItem->pINFO->MarketInfo.Price >= 100000000 && pMarketItem->pINFO->MarketInfo.Price < 1000000000)
				wsprintf(cTempChar, "\\e");
			else if (pMarketItem->pINFO->MarketInfo.Price >= 1000000000)
				wsprintf(cTempChar, "\\r");

			wsprintf(cList, "%d", pMarketItem->pINFO->MarketInfo.Price);
			MakeCurrencySeparator(cTemp, cList, 3, ',');

			if (pMarketItem->pINFO->MarketInfo.MarketState == MARKET_STATE_SELL) wsprintf(cList, "%s%s%s", cTempChar, cTemp, cTempChar);
			else if (pMarketItem->pINFO->MarketInfo.MarketState == MARKET_STATE_SELL_DONE) wsprintf(cList, "\\y%s\\y", cTemp);
			else if (pMarketItem->pINFO->MarketInfo.MarketState == MARKET_STATE_TIME_OUT) wsprintf(cList, "\\r%s\\r", cTemp);

			sSize = m_pFontTable[0][4]->GetStringSize(cList);
			m_pFontTable[0][4]->DrawText(_INET_OSR_MARKET_POS_X + ITEMLIST_PRICE_LOCATION_X + ITEMLIST_PRICE_WIDTH - sSize.cx - 22,
				_INET_OSR_MARKET_POS_Y + ITEMLIST_LOCATION_Y - ITEMLIST_CELL_HEIGHT - 2 + ITEMLIST_CELL_HEIGHT / 2 - sSize.cy / 2 + (nCnt - nMylistStartFromId) * (ITEMLIST_CELL_HEIGHT + 1), GUI_FONT_COLOR, cList);

			if (TC_SPI == pMarketItem->pINFO->MarketInfo.MoneyType)
				pImg = m_pMoneyImg[TC_SPI];
			else if (TC_WP == pMarketItem->pINFO->MarketInfo.MoneyType)
				pImg = m_pMoneyImg[TC_WP];
			else if (TC_CREDITS == pMarketItem->pINFO->MarketInfo.MoneyType)
				pImg = m_pMoneyImg[TC_CREDITS];

			if (pImg)
			{
				pImg->Move(_INET_OSR_MARKET_POS_X + ITEMLIST_MONEYTYPE_LOCATION_X - 22,
					_INET_OSR_MARKET_POS_Y - ITEMLIST_CELL_HEIGHT - 2 + ITEMLIST_MONEYTYPE_LOCATION_Y + (nCnt - nMylistStartFromId) * (ITEMLIST_CELL_HEIGHT + 1));
				pImg->Render();
			}
			if (pMarketItem->pINFO->MarketInfo.MarketState == MARKET_STATE_SELL)
			{
				m_pTimeLimitBtn[pMarketItem->byBtnState]->Move(_INET_OSR_MARKET_POS_X + ITEMLIST_MONEYTYPE_LOCATION_X + 20,
					_INET_OSR_MARKET_POS_Y - ITEMLIST_CELL_HEIGHT - 2 + ITEMLIST_MONEYTYPE_LOCATION_Y + (nCnt - nMylistStartFromId) * (ITEMLIST_CELL_HEIGHT + 1));
				m_pTimeLimitBtn[pMarketItem->byBtnState]->Render();
			}
			else if (pMarketItem->pINFO->MarketInfo.MarketState == MARKET_STATE_SELL_DONE)
			{
				m_pSellOKBtn[pMarketItem->byBtnState]->Move(_INET_OSR_MARKET_POS_X + ITEMLIST_MONEYTYPE_LOCATION_X + 20,
					_INET_OSR_MARKET_POS_Y - ITEMLIST_CELL_HEIGHT - 2 + ITEMLIST_MONEYTYPE_LOCATION_Y + (nCnt - nMylistStartFromId) * (ITEMLIST_CELL_HEIGHT + 1));
				m_pSellOKBtn[pMarketItem->byBtnState]->Render();
			}
			else if (pMarketItem->pINFO->MarketInfo.MarketState == MARKET_STATE_TIME_OUT)
			{
				m_pTimeLimitBtn[pMarketItem->byBtnState]->Move(_INET_OSR_MARKET_POS_X + ITEMLIST_MONEYTYPE_LOCATION_X + 20,
					_INET_OSR_MARKET_POS_Y - ITEMLIST_CELL_HEIGHT - 2 + ITEMLIST_MONEYTYPE_LOCATION_Y + (nCnt - nMylistStartFromId) * (ITEMLIST_CELL_HEIGHT + 1));
				m_pTimeLimitBtn[pMarketItem->byBtnState]->Render();
			}
			if (m_pIconInfo)
			{
				long nItemNum = 0;
				//char chIconName[64] = { 0, };
				if (!pMarketItem->pItem->ShapeItemNum)
					nItemNum = pMarketItem->pItem->ItemInfo->SourceIndex;
				else
				{
					ITEM* pShapeItem = g_pDatabase->GetServerItemInfo(pMarketItem->pItem->ShapeItemNum);
					if (pShapeItem)
						nItemNum= pShapeItem->SourceIndex;
					else
						nItemNum = pMarketItem->pItem->ItemInfo->SourceIndex;
				}

				m_pIconInfo->Render(nItemNum, _INET_OSR_MARKET_POS_X + ITEMLIST_ICON_LOCATION_X,
					_INET_OSR_MARKET_POS_Y - ITEMLIST_CELL_HEIGHT - 2 + ITEMLIST_ICON_LOCATION_Y + (nCnt - nMylistStartFromId) * (ITEMLIST_CELL_HEIGHT + 1));
				
				if (IS_COUNTABLE_ITEM(pMarketItem->pItem->Kind))
				{
					if (pMarketItem->pINFO->MarketInfo.ItemCount > 1)
					{
						char buf[128];
						if (pMarketItem->pINFO->MarketInfo.ItemCount >= 1000000000)
							sprintf(buf, "%.0fB", (float)(pMarketItem->pINFO->MarketInfo.ItemCount / 1000000000.0f));
						else if (pMarketItem->pINFO->MarketInfo.ItemCount >= 100000000)
							sprintf(buf, "%.0fM", (float)(pMarketItem->pINFO->MarketInfo.ItemCount / 1000000.0f));
						else if (pMarketItem->pINFO->MarketInfo.ItemCount >= 1000000)
							sprintf(buf, "%.0fM", (float)(pMarketItem->pINFO->MarketInfo.ItemCount / 1000000.0f));
						else if (pMarketItem->pINFO->MarketInfo.ItemCount >= 100000)
							sprintf(buf, "%.0fk", (float)(pMarketItem->pINFO->MarketInfo.ItemCount / 1000.0f));
						else if (pMarketItem->pINFO->MarketInfo.ItemCount >= 10000)
							sprintf(buf, "%.0fk", (float)(pMarketItem->pINFO->MarketInfo.ItemCount / 1000.0f));
						else if (pMarketItem->pINFO->MarketInfo.ItemCount >= 1000)
							sprintf(buf, "%.0fk", (float)(pMarketItem->pINFO->MarketInfo.ItemCount / 1000.0f));
						else
							sprintf(buf, "%d", pMarketItem->pINFO->MarketInfo.ItemCount);

						int len = strlen(buf) - 1;

						int itemCntStrLen = m_pFontTable[0][5]->GetStringSize(buf).cx;
						m_pFontTable[0][5]->DrawText(_INET_OSR_MARKET_POS_X + ITEMLIST_ICON_LOCATION_X + ITEMLIST_ITEMCNT_LOCATION_X - itemCntStrLen,
							_INET_OSR_MARKET_POS_Y - ITEMLIST_CELL_HEIGHT - 2 + ITEMLIST_ICON_LOCATION_Y + ITEMLIST_ITEMCNT_LOCATION_Y + (nCnt - nMylistStartFromId) * (ITEMLIST_CELL_HEIGHT + 1),
							RGB(170, 255, 200), buf, 0L);
					}
				}
				else
				{//leg count
					if (COMPARE_BIT_FLAG(pMarketItem->pItem->ItemInfo->ItemAttribute, ITEM_ATTR_LEGEND_ITEM) && pMarketItem->pItem->GetEnchantNumber() > 0)
					{
						int nSouls = (pMarketItem->pItem->ItemInfo->ReqMinLevel - 30) / 5;
						char buf[128];
						wsprintf(buf, "%d", nSouls);
						int len = strlen(buf) - 1;

						int itemCntStrLen = m_pFontTable[0][5]->GetStringSize(buf).cx;
						m_pFontTable[0][5]->DrawText(_INET_OSR_MARKET_POS_X + ITEMLIST_ICON_LOCATION_X + ITEMLIST_ITEMCNT_LOCATION_X - itemCntStrLen - 1,
							_INET_OSR_MARKET_POS_Y - ITEMLIST_CELL_HEIGHT - 2 + ITEMLIST_ICON_LOCATION_Y + ITEMLIST_ITEMCNT_LOCATION_Y + (nCnt - nMylistStartFromId) * (ITEMLIST_CELL_HEIGHT + 1) + 16,
							RGB(255, 100, 255), buf, 0L);
					}
					if (pMarketItem->pItem->GetEnchantNumber() > 0)
					{
						char buf[128];
						wsprintf(buf, "%d", pMarketItem->pItem->GetEnchantNumber());
						int len = strlen(buf) - 1;

						int itemCntStrLen = m_pFontTable[0][5]->GetStringSize(buf).cx;
						m_pFontTable[0][5]->DrawText(_INET_OSR_MARKET_POS_X + ITEMLIST_ICON_LOCATION_X + 1,
							_INET_OSR_MARKET_POS_Y - ITEMLIST_CELL_HEIGHT - 2 + ITEMLIST_ICON_LOCATION_Y + ITEMLIST_ITEMCNT_LOCATION_Y + (nCnt - nMylistStartFromId) * (ITEMLIST_CELL_HEIGHT + 1),
							RGB(255, 187, 61), buf, 0L);
					}
				}
			}		
		}
		nCnt++;
	}
}

void CINFCityAuction::ListRender()
{
	int nCnt = 0;
	char cList[512] = { 0, };
	char cTemp[512] = { 0, };
	CINFImage* pImg = NULL;

	vector<MARKETITEM_INFO*>::iterator it;
	for (it = m_vecItemInfo.begin(); it != m_vecItemInfo.end(); it++)
	{
		MARKETITEM_INFO* pMarketItem = (*it);

		if (!pMarketItem)
			return;

		if (GetRemainTime(pMarketItem->pINFO->MarketInfo.RegistTime, cList) == FALSE && MARKET_STATE_SELL_DONE != pMarketItem->pINFO->MarketInfo.MarketState)
		{
			pMarketItem->pINFO->MarketInfo.MarketState = MARKET_STATE_TIME_OUT;
		}

		m_pItemSelectedRow->Move(_INET_OSR_MARKET_POS_X + ITEMLIST_LOCATION_X + 16, _INET_OSR_MARKET_POS_Y + ITEMLIST_LOCATION_Y + nCnt * (ITEMLIST_CELL_HEIGHT + 1) - 2);
		if(nCnt == m_nSelectListItem)
		m_pItemSelectedRow->Render();

		wsprintf(cList, "\\e+%d\\e", pMarketItem->pItem->GetEnchantNumber());

		SIZE sSize = m_pFontTable[0][0]->GetStringSize(cList);
		m_pFontTable[0][0]->DrawText(_INET_OSR_MARKET_POS_X + ITEMLIST_ENCHANT_LOCATION_X + ITEMLIST_LEVEL_WIDTH / 2 - sSize.cx / 2,
			_INET_OSR_MARKET_POS_Y + ITEMLIST_LOCATION_Y + ITEMLIST_CELL_HEIGHT / 2 - sSize.cy / 2 + nCnt * (ITEMLIST_CELL_HEIGHT + 1), GUI_FONT_COLOR, cList);

		wsprintf(cList, "%s", pMarketItem->pINFO->MarketInfo.ItemName);
		
		sSize = m_pFontTable[0][1]->GetStringSize(cList);
		m_pFontTable[0][1]->DrawText(_INET_OSR_MARKET_POS_X + ITEMLIST_NAME_LOCATION_X + ITEMLIST_NAME_WIDTH / 2 - sSize.cx / 2,
			_INET_OSR_MARKET_POS_Y + ITEMLIST_LOCATION_Y + ITEMLIST_CELL_HEIGHT / 2 - sSize.cy / 2 + nCnt * (ITEMLIST_CELL_HEIGHT + 1), GUI_FONT_COLOR, cList);

		wsprintf(cList, "%d", pMarketItem->pINFO->MarketInfo.ItemLevel);

		sSize = m_pFontTable[0][2]->GetStringSize(cList);
		m_pFontTable[0][2]->DrawText(_INET_OSR_MARKET_POS_X + ITEMLIST_LEVEL_LOCATION_X + ITEMLIST_LEVEL_WIDTH / 2 - sSize.cx / 2,
			_INET_OSR_MARKET_POS_Y + ITEMLIST_LOCATION_Y + ITEMLIST_CELL_HEIGHT / 2 - sSize.cy / 2 + nCnt * (ITEMLIST_CELL_HEIGHT + 1), GUI_FONT_COLOR, cList);

		GetRemainTime(pMarketItem->pINFO->MarketInfo.RegistTime, cList);

		sSize = m_pFontTable[0][3]->GetStringSize(cList);
		m_pFontTable[0][3]->DrawText(_INET_OSR_MARKET_POS_X + ITEMLIST_REGISTTIME_LOCATION_X + ITEMLIST_REGISTTIME_WIDTH / 2 - sSize.cx / 2,
			_INET_OSR_MARKET_POS_Y + ITEMLIST_LOCATION_Y + ITEMLIST_CELL_HEIGHT / 2 - sSize.cy / 2 + nCnt * (ITEMLIST_CELL_HEIGHT + 1), GUI_FONT_COLOR, cList);

		char cTempChar[5] = { 0, };
		if (pMarketItem->pINFO->MarketInfo.Price < 10000)
			wsprintf(cTempChar, "\\w");
		else if (pMarketItem->pINFO->MarketInfo.Price >= 10000 && pMarketItem->pINFO->MarketInfo.Price < 100000)
			wsprintf(cTempChar, "\\c");
		else if (pMarketItem->pINFO->MarketInfo.Price >= 100000 && pMarketItem->pINFO->MarketInfo.Price < 1000000)
			wsprintf(cTempChar, "\\g");
		else if (pMarketItem->pINFO->MarketInfo.Price >= 1000000 && pMarketItem->pINFO->MarketInfo.Price < 10000000)
			wsprintf(cTempChar, "\\y");
		else if (pMarketItem->pINFO->MarketInfo.Price >= 10000000 && pMarketItem->pINFO->MarketInfo.Price < 100000000)
			wsprintf(cTempChar, "\\m");
		else if (pMarketItem->pINFO->MarketInfo.Price >= 100000000 && pMarketItem->pINFO->MarketInfo.Price < 1000000000)
			wsprintf(cTempChar, "\\e");
		else if (pMarketItem->pINFO->MarketInfo.Price >= 1000000000)
			wsprintf(cTempChar, "\\r");

		wsprintf(cList, "%d", pMarketItem->pINFO->MarketInfo.Price);
		MakeCurrencySeparator(cTemp, cList, 3, ',');
		wsprintf(cList, "%s%s%s", cTempChar, cTemp, cTempChar);

		if (IS_COUNTABLE_ITEM(pMarketItem->pItem->Kind) && pMarketItem->pItem->CurrentCount > 1) {

			sSize = m_pFontTable[0][4]->GetStringSize(cList);
			m_pFontTable[0][4]->DrawText(_INET_OSR_MARKET_POS_X + ITEMLIST_PRICE_LOCATION_X + ITEMLIST_PRICE_WIDTH - sSize.cx,
				_INET_OSR_MARKET_POS_Y + ITEMLIST_LOCATION_Y - 10 + ITEMLIST_CELL_HEIGHT / 2 - sSize.cy / 2 + nCnt * (ITEMLIST_CELL_HEIGHT + 1), GUI_FONT_COLOR, cList);
			wsprintf(cTempChar, "\\w");
			wsprintf(cList, "%d", pMarketItem->pINFO->MarketInfo.Price / pMarketItem->pItem->CurrentCount);
			MakeCurrencySeparator(cTemp, cList, 3, ',');
			wsprintf(cList, "%s%s%s / pce", cTempChar, cTemp, cTempChar);

			sSize = m_pFontTable[0][4]->GetStringSize(cList);
			m_pFontTable[0][4]->DrawText(_INET_OSR_MARKET_POS_X + ITEMLIST_PRICE_LOCATION_X + ITEMLIST_PRICE_WIDTH - sSize.cx,
				_INET_OSR_MARKET_POS_Y + ITEMLIST_LOCATION_Y + 7 + ITEMLIST_CELL_HEIGHT / 2 - sSize.cy / 2 + nCnt * (ITEMLIST_CELL_HEIGHT + 1), GUI_FONT_COLOR, cList);
		}
		else
		{
			sSize = m_pFontTable[0][4]->GetStringSize(cList);
			m_pFontTable[0][4]->DrawText(_INET_OSR_MARKET_POS_X + ITEMLIST_PRICE_LOCATION_X + ITEMLIST_PRICE_WIDTH - sSize.cx,
				_INET_OSR_MARKET_POS_Y + ITEMLIST_LOCATION_Y + ITEMLIST_CELL_HEIGHT / 2 - sSize.cy / 2 + nCnt * (ITEMLIST_CELL_HEIGHT + 1), GUI_FONT_COLOR, cList);
		}
		if (TC_SPI == pMarketItem->pINFO->MarketInfo.MoneyType)
			pImg = m_pMoneyImg[TC_SPI];
		else if (TC_WP == pMarketItem->pINFO->MarketInfo.MoneyType)
			pImg = m_pMoneyImg[TC_WP];
		else if (TC_CREDITS == pMarketItem->pINFO->MarketInfo.MoneyType)
			pImg = m_pMoneyImg[TC_CREDITS];

		if (pImg)
		{
			pImg->Move(_INET_OSR_MARKET_POS_X + ITEMLIST_MONEYTYPE_LOCATION_X,
				_INET_OSR_MARKET_POS_Y + ITEMLIST_MONEYTYPE_LOCATION_Y + nCnt * (ITEMLIST_CELL_HEIGHT + 1));
			pImg->Render();
		}

		if (m_pIconInfo)
		{
			long nItemNum = 0;
			if (!pMarketItem->pItem->ShapeItemNum)
				nItemNum= pMarketItem->pItem->ItemInfo->SourceIndex;
			else
			{
				ITEM* pShapeItem = g_pDatabase->GetServerItemInfo(pMarketItem->pItem->ShapeItemNum);
				if (pShapeItem)
					nItemNum = pShapeItem->SourceIndex;
				else
					nItemNum = pMarketItem->pItem->ItemInfo->SourceIndex;
			}

			m_pIconInfo->Render(nItemNum, _INET_OSR_MARKET_POS_X + ITEMLIST_ICON_LOCATION_X,
				_INET_OSR_MARKET_POS_Y + ITEMLIST_ICON_LOCATION_Y + nCnt * (ITEMLIST_CELL_HEIGHT + 1));
	
			if (IS_COUNTABLE_ITEM(pMarketItem->pItem->Kind))
			{
				if (pMarketItem->pINFO->MarketInfo.ItemCount > 1)
				{
					char buf[128];
					if (pMarketItem->pINFO->MarketInfo.ItemCount >= 1000000000)
						sprintf(buf, "%.0fB", (float)(pMarketItem->pINFO->MarketInfo.ItemCount / 1000000000.0f));
					else if (pMarketItem->pINFO->MarketInfo.ItemCount >= 100000000)
						sprintf(buf, "%.0fM", (float)(pMarketItem->pINFO->MarketInfo.ItemCount / 1000000.0f));
					else if (pMarketItem->pINFO->MarketInfo.ItemCount >= 1000000)
						sprintf(buf, "%.0fM", (float)(pMarketItem->pINFO->MarketInfo.ItemCount / 1000000.0f));
					else if (pMarketItem->pINFO->MarketInfo.ItemCount >= 100000)
						sprintf(buf, "%.0fk", (float)(pMarketItem->pINFO->MarketInfo.ItemCount / 1000.0f));
					else if (pMarketItem->pINFO->MarketInfo.ItemCount >= 10000)
						sprintf(buf, "%.0fk", (float)(pMarketItem->pINFO->MarketInfo.ItemCount / 1000.0f));
					else if (pMarketItem->pINFO->MarketInfo.ItemCount >= 1000)
						sprintf(buf, "%.0fk", (float)(pMarketItem->pINFO->MarketInfo.ItemCount / 1000.0f));
					else
						sprintf(buf, "%d", pMarketItem->pINFO->MarketInfo.ItemCount);

					int len = strlen(buf) - 1;

					int itemCntStrLen = m_pFontTable[0][5]->GetStringSize(buf).cx;
					m_pFontTable[0][5]->DrawText(_INET_OSR_MARKET_POS_X + ITEMLIST_ICON_LOCATION_X + ITEMLIST_ITEMCNT_LOCATION_X - itemCntStrLen,
						_INET_OSR_MARKET_POS_Y + ITEMLIST_ICON_LOCATION_Y + ITEMLIST_ITEMCNT_LOCATION_Y + nCnt * (ITEMLIST_CELL_HEIGHT + 1),
						RGB(170, 255, 200), buf, 0L);
				}
			}
			else
			{//leg count
				if (COMPARE_BIT_FLAG(pMarketItem->pItem->ItemInfo->ItemAttribute, ITEM_ATTR_LEGEND_ITEM) && pMarketItem->pItem->GetEnchantNumber() > 0)
				{
					int nSouls = (pMarketItem->pItem->ItemInfo->ReqMinLevel - 30) / 5;
					char buf[128];
					wsprintf(buf, "%d", nSouls);
					int len = strlen(buf) - 1;

					int itemCntStrLen = m_pFontTable[0][5]->GetStringSize(buf).cx;
					m_pFontTable[0][5]->DrawText(_INET_OSR_MARKET_POS_X + ITEMLIST_ICON_LOCATION_X + ITEMLIST_ITEMCNT_LOCATION_X - itemCntStrLen - 1,
						_INET_OSR_MARKET_POS_Y + ITEMLIST_ICON_LOCATION_Y + ITEMLIST_ITEMCNT_LOCATION_Y + nCnt * (ITEMLIST_CELL_HEIGHT + 1) + 16,
						RGB(255, 100, 255), buf, 0L);
				}
				if (pMarketItem->pItem->GetEnchantNumber() > 0)
				{
					char buf[128];
					wsprintf(buf, "%d", pMarketItem->pItem->GetEnchantNumber());
					int len = strlen(buf) - 1;

					int itemCntStrLen = m_pFontTable[0][5]->GetStringSize(buf).cx;
					m_pFontTable[0][5]->DrawText(_INET_OSR_MARKET_POS_X + ITEMLIST_ICON_LOCATION_X + 1,
						_INET_OSR_MARKET_POS_Y + ITEMLIST_ICON_LOCATION_Y + ITEMLIST_ITEMCNT_LOCATION_Y + nCnt * (ITEMLIST_CELL_HEIGHT + 1),
						RGB(255, 187, 61), buf, 0L);
				}
			}
		}

		nCnt++;
	}
}

void CINFCityAuction::BuyItem()
{
	MSG_FC_MARKET_BUY_REQUEST sMsg;
	memset(&sMsg, 0x00, sizeof(sMsg));

	sMsg.MarketUID = m_vecItemInfo[m_nSelectListItem]->pINFO->MarketInfo.MarketUID;

	g_pFieldWinSocket->SendMsg(T_FC_MARKET_BUY_REQUEST, (char*)&sMsg, sizeof(sMsg));

}