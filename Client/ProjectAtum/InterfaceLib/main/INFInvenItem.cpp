#include "stdafx.h"
#include "AtumApplication.h"
#include "INFImage.h"
#include "GameDataLast.h"
#include "INFGameMain.h"
#include "D3DHanFont.h"
#include "INFInvenExtend.h"
#include "INFIcon.h"
#include "StoreData.h"
#include "ItemInfo.h"
#include "ShuttleChild.h"
#include "AtumDatabase.h"
#include "Interface.h"
#include "INFCityBase.h"
#include "INFArenaScrollBar.h"
#include "INFTrade.h"
#include "Chat.h"
#include "INFWindow.h"
#include "Skill.h"
#include "INFImageBtn.h"
#include "INFCityStore.h"
#include "AtumSound.h"
#include "INFLuckyMachine.h"
#include "INFCityLab.h"
#include "INFCityBazaar.h"
#include "INFInvenItem.h"
#include "INFOptionMachine.h"
#include "INFDissolution.h"
#include "INFCityAuction.h"
#ifdef _INET_PET
	#include "INFCharacterInfoExtend.h"
	#include "PetManager.h"
#endif

#ifdef _INET_LINK_CHAT
	#include "INFGameMainChat.h"
	#include "IMSocketManager.h"
#endif

#define EXTEND_INVEN_SLOT_SIZE			30
#define EXTEND_INVEN_SLOT_INTERVAL		32
#define EXTEND_INVEN_ITEM_SLOT_START_X	51
#define EXTEND_INVEN_ITEM_SLOT_START_Y	30

#define	EXTEND_INVEN_SCROLL_WIDTH	11
#define	EXTEND_INVEN_SCROLL_HEIGHT	241

#define EXTEND_INVEN_SCROLL_LINE_START_X		376
#define EXTEND_INVEN_SCROLL_LINE_START_Y		30

#define	EXTEND_INVEN_CAPS_HEIGHT	20

#define INVEN_SPI_START_X		243
#define INVEN_SPI_START_Y		244
#define INVEN_WARPOINT_X		367
#define INVEN_WARPOINT_Y		244

#define EXTEND_WEIGHT_START_X			367
#define EXTEND_WEIGHT_START_Y			225

#define INVEN_GARBAGE_START_X	373
#define INVEN_GARBAGE_START_Y	232
#define INVEN_GARBAGE_SIZE		24

#define INVEN_SPI_WIDTH			90
#define INVEN_SPI_HEIGHT		18


CINFInvenItem::CINFInvenItem(CAtumNode* pParent)
{
	m_pParent = pParent;
	m_bShowWnd = FALSE;
	m_pInvenBase = nullptr;
	m_pFontItemNum = nullptr;
	m_ptBkPos.x = m_ptBkPos.y =0;
	m_pINFInvenScrollBar = nullptr;
	m_bMove = FALSE;	
	m_ptCommOpMouse.x = m_ptCommOpMouse.y = 0;
	m_pMultiItemSelImage = nullptr;
	m_pDisableItemImage = nullptr;

#ifdef _INET_PET
	m_pSelectPetkitItemImageHP = nullptr;
	m_pSelectPetkitItemImageSheld = nullptr;
	m_pSelectPetkitItemImageSP = nullptr;
	m_pSelectPetSocketItemImage = nullptr;
#endif

	m_pEqShow = nullptr;
	m_pCloseBtn = nullptr;
	
#ifdef _INET_SORT_INV
	m_pEQSort = nullptr;
#endif
#ifdef _INET_SELECT_ITEM_WITH_SHIFT
	nClickCount = NULL;
	nFirstIdx = NULL;
	nLastIdx = NULL;
#endif

	m_bTradeItemCenterState = FALSE;
}

CINFInvenItem::~CINFInvenItem()
{
	SAFE_DELETE(m_pInvenBase);
	SAFE_DELETE(m_pFontItemNum);
	SAFE_DELETE(m_pINFInvenScrollBar);
	SAFE_DELETE(m_pDisableItemImage);
	SAFE_DELETE(m_pMultiItemSelImage);	
	SAFE_DELETE(m_pEqShow);	
	SAFE_DELETE(m_pCloseBtn);

#ifdef _INET_PET
	SAFE_DELETE(m_pSelectPetkitItemImageHP);
	SAFE_DELETE(m_pSelectPetkitItemImageSheld);
	SAFE_DELETE(m_pSelectPetkitItemImageSP);
	SAFE_DELETE(m_pSelectPetSocketItemImage);
#endif
	
#ifdef _INET_SORT_INV
	SAFE_DELETE(m_pEQSort);
#endif
#ifdef _INET_SELECT_ITEM_WITH_SHIFT
	nClickCount = NULL;
	nFirstIdx = NULL;
	nLastIdx = NULL;
#endif
}

HRESULT CINFInvenItem::InitDeviceObjects()
{
	DataHeader	* pDataHeader = nullptr;
	if(nullptr == m_pInvenBase)
	{
		m_pInvenBase = new CINFImage;
		pDataHeader = FindResource("w_wi12");
		m_pInvenBase->InitDeviceObjects(pDataHeader->m_pData,pDataHeader->m_DataSize) ;	
	}
	if(nullptr == m_pInvenBase2)
	{
		m_pInvenBase2 = new CINFImage;
		pDataHeader = FindResource("w_wi22");
		m_pInvenBase2->InitDeviceObjects(pDataHeader->m_pData,pDataHeader->m_DataSize) ;	
	}
	if(nullptr == m_pFontItemNum)
	{
		m_pFontItemNum = new CD3DHanFont(_T(g_pD3dApp->GetFontStyle()),8, D3DFONT_ZENABLE,  TRUE,256,32);
		m_pFontItemNum->InitDeviceObjects(g_pD3dDev);
	}
	{
		char szScBk[30], szScBall[30];	
		if(nullptr == m_pINFInvenScrollBar)
		{
			m_pINFInvenScrollBar = new CINFArenaScrollBar;
		}
		wsprintf(szScBk,"arescroll");
		wsprintf(szScBall,"c_scrlb");

		m_pINFInvenScrollBar->InitDeviceObjects(INVEN_Y_NUMBER, szScBall);
	}
	if(nullptr == m_pMultiItemSelImage)
	{
		m_pMultiItemSelImage = new CINFImage;
		pDataHeader = FindResource("selicon");
		m_pMultiItemSelImage->InitDeviceObjects(pDataHeader->m_pData,pDataHeader->m_DataSize) ;	
	}
	if(nullptr == m_pDisableItemImage)
	{
		m_pDisableItemImage = new CINFImage;
		pDataHeader = FindResource("LM_inven");
		m_pDisableItemImage->InitDeviceObjects(pDataHeader->m_pData,pDataHeader->m_DataSize);	
	}
#ifdef _INET_PET
	if (nullptr == m_pSelectPetkitItemImageHP)
	{
		m_pSelectPetkitItemImageHP = new CINFImage;
		pDataHeader = FindResource("PN_usehp");
		m_pSelectPetkitItemImageHP->InitDeviceObjects(pDataHeader->m_pData, pDataHeader->m_DataSize);
	}
	if (nullptr == m_pSelectPetkitItemImageSheld)
	{
		m_pSelectPetkitItemImageSheld = new CINFImage;
		pDataHeader = FindResource("PN_usesh");
		m_pSelectPetkitItemImageSheld->InitDeviceObjects(pDataHeader->m_pData, pDataHeader->m_DataSize);
	}
	if (nullptr == m_pSelectPetkitItemImageSP)
	{
		m_pSelectPetkitItemImageSP = new CINFImage;
		pDataHeader = FindResource("PN_usesp");
		m_pSelectPetkitItemImageSP->InitDeviceObjects(pDataHeader->m_pData, pDataHeader->m_DataSize);
	}
	if (nullptr == m_pSelectPetSocketItemImage)
	{
		m_pSelectPetSocketItemImage = new CINFImage;
		pDataHeader = FindResource("PN_selsoc");
		m_pSelectPetSocketItemImage->InitDeviceObjects(pDataHeader->m_pData, pDataHeader->m_DataSize);
	}
#endif
	{	
		char szUpBtn[30], szDownBtn[30], szSelBtn[30], szDisBtn[30];				
		wsprintf(szUpBtn, "ieg3");
		wsprintf(szDownBtn, "ieg1");
		wsprintf(szSelBtn, "ieg0");
		wsprintf(szDisBtn, "ieg2");
		if(nullptr == m_pEqShow)
		{
			m_pEqShow = new CINFImageBtn;
			m_pEqShow->InitDeviceObjects(szUpBtn, szDownBtn, szSelBtn, szDisBtn);		
		}
		
	}
#ifdef _INET_SORT_INV
		{
			char szUpBtn[30], szDownBtn[30], szSelBtn[30], szDisBtn[30];
			wsprintf(szUpBtn, "isort3");
			wsprintf(szDownBtn, "isort1");
			wsprintf(szSelBtn, "isort0");
			wsprintf(szDisBtn, "isort2");
			if (NULL == m_pEQSort)
			{
				m_pEQSort = new CINFImageBtn;
				m_pEQSort->InitDeviceObjects(szUpBtn, szDownBtn, szSelBtn, szDisBtn);
			}

		}
#endif
	{
		char szUpBtn[30], szDownBtn[30], szSelBtn[30], szDisBtn[30];
		wsprintf(szUpBtn, "xclose");
		wsprintf(szDownBtn, "xclose");
		wsprintf(szSelBtn, "xclose");
		wsprintf(szDisBtn, "xclose");
		if(NULL == m_pCloseBtn)
		{
			m_pCloseBtn = new CINFImageBtn;
		}
		m_pCloseBtn->InitDeviceObjects(szUpBtn, szDownBtn, szSelBtn, szDisBtn);
		
	}

	return S_OK;
}
HRESULT CINFInvenItem::RestoreDeviceObjects()
{
	m_pInvenBase->RestoreDeviceObjects();
	m_pInvenBase2->RestoreDeviceObjects();
	m_pFontItemNum->RestoreDeviceObjects();
	
	{
		m_pINFInvenScrollBar->RestoreDeviceObjects();				
	}
	m_pMultiItemSelImage->RestoreDeviceObjects();

	// 2010. 05. 10 by jskim ˝Ĺ±Ô ·°Ĺ° ¸Ó˝Ĺ Ăß°ˇąćľČ - »óÁˇżˇĽ­ »çżë °ˇ´ÉÇŃ ľĆŔĚĹŰ¸¸ ş¸ż©ÁÖ±â
	if(m_pDisableItemImage)
	{
		m_pDisableItemImage->RestoreDeviceObjects();
	}
	//end 2010. 05. 10 by jskim ˝Ĺ±Ô ·°Ĺ° ¸Ó˝Ĺ Ăß°ˇąćľČ - »óÁˇżˇĽ­ »çżë °ˇ´ÉÇŃ ľĆŔĚĹŰ¸¸ ş¸ż©ÁÖ±â
	if(m_pEqShow)
	{
		m_pEqShow->RestoreDeviceObjects();				
	}
#ifdef _INET_SORT_INV
	if (m_pEQSort)
	{
		m_pEQSort->RestoreDeviceObjects();
	}
#endif
	if(m_pCloseBtn)
	{
		m_pCloseBtn->RestoreDeviceObjects();			
	}
#ifdef _INET_PET
	if (m_pSelectPetkitItemImageHP)
	{
		m_pSelectPetkitItemImageHP->RestoreDeviceObjects();
	}
	if (m_pSelectPetkitItemImageSheld)
	{
		m_pSelectPetkitItemImageSheld->RestoreDeviceObjects();
	}
	if (m_pSelectPetkitItemImageSP)
	{
		m_pSelectPetkitItemImageSP->RestoreDeviceObjects();
	}
	if (m_pSelectPetSocketItemImage)
	{
		m_pSelectPetSocketItemImage->RestoreDeviceObjects();
	}
#endif
	UpdateBtnPos();
		
	return S_OK;
}
HRESULT CINFInvenItem::DeleteDeviceObjects()
{
	m_pInvenBase->DeleteDeviceObjects();
	SAFE_DELETE(m_pInvenBase);
	m_pInvenBase2->DeleteDeviceObjects();
	SAFE_DELETE(m_pInvenBase2);

	m_pFontItemNum->DeleteDeviceObjects();
	SAFE_DELETE(m_pFontItemNum);

	{
		m_pINFInvenScrollBar->DeleteDeviceObjects();	
		SAFE_DELETE(m_pINFInvenScrollBar);
	}
	m_pMultiItemSelImage->DeleteDeviceObjects();

	// 2010. 05. 10 by jskim ˝Ĺ±Ô ·°Ĺ° ¸Ó˝Ĺ Ăß°ˇąćľČ - »óÁˇżˇĽ­ »çżë °ˇ´ÉÇŃ ľĆŔĚĹŰ¸¸ ş¸ż©ÁÖ±â
	if(m_pDisableItemImage)
	{
		m_pDisableItemImage->DeleteDeviceObjects();
	}
	//end 2010. 05. 10 by jskim ˝Ĺ±Ô ·°Ĺ° ¸Ó˝Ĺ Ăß°ˇąćľČ - »óÁˇżˇĽ­ »çżë °ˇ´ÉÇŃ ľĆŔĚĹŰ¸¸ ş¸ż©ÁÖ±â
	SAFE_DELETE(m_pMultiItemSelImage );

	if(m_pEqShow)
	{		
		m_pEqShow->DeleteDeviceObjects();
		SAFE_DELETE(m_pEqShow);
	}
#ifdef _INET_SORT_INV
	if (m_pEQSort)
	{
		m_pEQSort->DeleteDeviceObjects();
		SAFE_DELETE(m_pEQSort);
	}
#endif
	if(m_pCloseBtn)
	{
		m_pCloseBtn->DeleteDeviceObjects();	
		SAFE_DELETE(m_pCloseBtn);
	}
#ifdef _INET_PET
	if (m_pSelectPetkitItemImageHP)
	{
		m_pSelectPetkitItemImageHP->DeleteDeviceObjects();
		SAFE_DELETE(m_pSelectPetkitItemImageHP);
	}
	if (m_pSelectPetkitItemImageSheld)
	{
		m_pSelectPetkitItemImageSheld->DeleteDeviceObjects();
		SAFE_DELETE(m_pSelectPetkitItemImageSheld);
	}
	if (m_pSelectPetkitItemImageSP)
	{
		m_pSelectPetkitItemImageSP->DeleteDeviceObjects();
		SAFE_DELETE(m_pSelectPetkitItemImageSP);
	}
	if (m_pSelectPetSocketItemImage)
	{
		m_pSelectPetSocketItemImage->DeleteDeviceObjects();
		SAFE_DELETE(m_pSelectPetSocketItemImage);
	}
#endif
	return S_OK;
}
HRESULT CINFInvenItem::InvalidateDeviceObjects()
{		
	m_pInvenBase->InvalidateDeviceObjects();
	m_pInvenBase2->InvalidateDeviceObjects();
	m_pFontItemNum->InvalidateDeviceObjects();
	{
		m_pINFInvenScrollBar->InvalidateDeviceObjects();
	}
	m_pMultiItemSelImage->InvalidateDeviceObjects();

	// 2010. 05. 10 by jskim ˝Ĺ±Ô ·°Ĺ° ¸Ó˝Ĺ Ăß°ˇąćľČ - »óÁˇżˇĽ­ »çżë °ˇ´ÉÇŃ ľĆŔĚĹŰ¸¸ ş¸ż©ÁÖ±â
	if(m_pDisableItemImage)
	{
		m_pDisableItemImage->InvalidateDeviceObjects();
	}
	//end 2010. 05. 10 by jskim ˝Ĺ±Ô ·°Ĺ° ¸Ó˝Ĺ Ăß°ˇąćľČ - »óÁˇżˇĽ­ »çżë °ˇ´ÉÇŃ ľĆŔĚĹŰ¸¸ ş¸ż©ÁÖ±â
	if(m_pEqShow)
	{		
		m_pEqShow->InvalidateDeviceObjects();
	}
#ifdef _INET_SORT_INV
	if (m_pEQSort)
	{
		m_pEQSort->InvalidateDeviceObjects();
	}
#endif
	if(m_pCloseBtn)
	{
		m_pCloseBtn->InvalidateDeviceObjects();		
	}
#ifdef _INET_PET
	if (m_pSelectPetkitItemImageHP)
	{
		m_pSelectPetkitItemImageHP->InvalidateDeviceObjects();
	}
	if (m_pSelectPetkitItemImageSheld)
	{
		m_pSelectPetkitItemImageSheld->InvalidateDeviceObjects();
	}
	if (m_pSelectPetkitItemImageSP)
	{
		m_pSelectPetkitItemImageSP->InvalidateDeviceObjects();
	}
	if (m_pSelectPetSocketItemImage)
	{
		m_pSelectPetSocketItemImage->InvalidateDeviceObjects();
	}
#endif
	return S_OK;
}
void CINFInvenItem::Render()
{
	if(!IsShowWnd())
	{
		return;
	}
	CINFInvenExtend* pParent = (CINFInvenExtend*)m_pParent;
	
	int nWindowPosX = m_ptBkPos.x;
	int nWindowPosY = m_ptBkPos.y; 

	int nPosX, nPosY;
	nPosX = nPosY = 0;
	nPosX = m_ptBkPos.x;
	nPosY = m_ptBkPos.y;
	if(!m_bTradeItemCenterState)
	{	
		m_pInvenBase->Move(nPosX, nPosY);
		m_pInvenBase->Render();		
	}
	else
	{
		m_pInvenBase2->Move(nPosX, nPosY);
		m_pInvenBase2->Render();
	}

	m_pINFInvenScrollBar->Render();

	RenderInvenItem();
	if (!m_bTradeItemCenterState)												  // 2013-11-29 by ssjung 거래소 구현
	{
		CD3DHanFont* pFontSpi = pParent->GetFontSpi();
		if(pFontSpi)
		{
			char temp1[MAX_PATH];
			char temp2[MAX_PATH];
			SIZE size;
			
			int	nItemSpi = pParent->GetItemSpi(); // SPI·®
			// SPI
			wsprintf( temp1, "%d", nItemSpi );
			MakeCurrencySeparator( temp2, temp1, 3, ',' );
			size = pFontSpi->GetStringSize(temp2);
			pFontSpi->DrawText(nWindowPosX + INVEN_SPI_START_X-size.cx, nWindowPosY + INVEN_SPI_START_Y, GUI_FONT_COLOR_BM,temp2, 0L);
			
			// War Point
			wsprintf(temp1,"%d",g_pShuttleChild->m_myShuttleInfo.WarPoint);
			MakeCurrencySeparator(temp2,temp1,3,',');
			size = pFontSpi->GetStringSize(temp2);
			pFontSpi->DrawText(nWindowPosX + INVEN_WARPOINT_X-size.cx, nWindowPosY + INVEN_WARPOINT_Y, GUI_FONT_COLOR_BM,temp2, 0L);
		}
	}
	if (!m_bTradeItemCenterState)												  // 2013-11-29 by ssjung 거래소 구현
	{
		CD3DHanFont*  pFontWeight = pParent->GetFontWeight(0);
		char buff[256];
		// 2009. 11. 3 by jskim Äł˝¬(ŔÎşĄ/Ă˘°í Č®Ŕĺ) ľĆŔĚĹŰ Ăß°ˇ ±¸Çö
		//CAtumSJ::GetMaxInventorySize((BOOL)g_pD3dApp->GetPrimiumCardInfo()->nCardItemNum1) - 1);	// 2006-06-23 by ispark, -1Ŕş ˝şÇÇ ľĆŔĚĹŰŔ» Á¦żÜÇĎ´Â °ÍŔĚ´Ů.
		// 2009. 12. 17 by jskim Äł˝¬(ŔÎşĄ/Ă˘°í Č®Ŕĺ) ŔçĽöÁ¤ 
// 		CHARACTER* pMainInfo = g_pD3dApp->GetMFSMyShuttleInfo();
// 		wsprintf(buff, "%s %d/%d", STRMSG_C_INTERFACE_0026, 
// 				(int)(g_pStoreData->GetTotalUseInven()), 		
// 		CAtumSJ::GetMaxInventorySize((BOOL)g_pD3dApp->GetPrimiumCardInfo()->nCardItemNum1, pMainInfo->GetAddedPermanentInventoryCount()) - 1);	// 2006-06-23 by ispark, -1Ŕş ˝şÇÇ ľĆŔĚĹŰŔ» Á¦żÜÇĎ´Â °ÍŔĚ´Ů.
		wsprintf(buff, "%s %d/%d", STRMSG_C_INTERFACE_0026, 
				(int)(g_pStoreData->GetTotalUseInven()), 		
 		CAtumSJ::GetMaxInventorySize((BOOL)g_pD3dApp->GetPrimiumCardInfo()->nCardItemNum1, g_pShuttleChild->m_myShuttleInfo.GetAddedPermanentInventoryCount()) - 1);	// 2006-06-23 by ispark, -1Ŕş ˝şÇÇ ľĆŔĚĹŰŔ» Á¦żÜÇĎ´Â °ÍŔĚ´Ů.
		//end 2009. 12. 17 by jskim Äł˝¬(ŔÎşĄ/Ă˘°í Č®Ŕĺ) ŔçĽöÁ¤ 
		//end 2009. 11. 3 by jskim Äł˝¬(ŔÎşĄ/Ă˘°í Č®Ŕĺ) ľĆŔĚĹŰ Ăß°ˇ ±¸Çö
		
		SIZE size = pFontWeight->GetStringSize(buff);		
		pFontWeight->DrawText(nWindowPosX+EXTEND_WEIGHT_START_X-size.cx, 
								nWindowPosY+EXTEND_WEIGHT_START_Y, GUI_FONT_COLOR_BM, buff, 0 );//"ŔűŔç·®"		
	}

	m_pEqShow->Render();	
#ifdef _INET_SORT_INV
	m_pEQSort->Render();
#endif

	if (!m_bTradeItemCenterState)												  // 2013-11-29 by ssjung 거래소 구현
	{
		m_pCloseBtn->Render();
#ifdef _INET_INVEN_RIGHT_CLICK_MENU	
		if (pParent) {
			pParent->RenderItemMenuListWnd();
		}
#endif
	}
}

void CINFInvenItem::Tick()
{
	
}
BOOL CINFInvenItem::IsShowWnd()
{
	return m_bShowWnd;
}
void CINFInvenItem::ShowWnd(BOOL bShow, POINT *i_ptPos/*=NULL*/)
{
	m_bShowWnd = bShow;
	// 2009. 08. 19 by jsKim ·Łµů Áß ¸Ţ´ş »ýĽşÇŇ °ćżě ÄżĽ­°ˇ şŻÇĎÁö ľĘ´Â ąö±×
	if(bShow)
	{
		g_INFCnt++;
		g_pGameMain->m_bChangeMousePoint = TRUE;
	}
	else
	{
		g_INFCnt--;
		if(g_INFCnt < 0)
		{
			g_INFCnt = 0;
		}
		if(g_INFCnt==0)
		{
			g_pGameMain->m_bChangeMousePoint = FALSE;
		}

	}
	// end 2009. 08. 19 by jsKim ·Łµů Áß ¸Ţ´ş »ýĽşÇŇ °ćżě ÄżĽ­°ˇ şŻÇĎÁö ľĘ´Â ąö±×
	
	if(i_ptPos)
	{
		m_ptBkPos = (*i_ptPos);
		UpdateBtnPos();
	}
#ifdef _INET_INVEN_RIGHT_CLICK_MENU	
	POINT pt = { 0,0 };
	CINFInvenExtend* pParent = (CINFInvenExtend*)m_pParent;
	pParent->OnClickItemMenuListWnd(FALSE, pt, 0, 0);
#endif
}


///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		
/// \author		// 2008-08-22 by bhsohn EP3 ŔÎşĄĹä¸® Ăł¸®
/// \date		2008-08-22 ~ 2008-08-22
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CINFInvenItem::RenderInvenItem()
{
	CINFInvenExtend* pParent = (CINFInvenExtend*)m_pParent;
	CINFIcon* pIconInfo = g_pGameMain->m_pIcon;
	int nPosX, nPosY;
	int nIdxPos = 0;
	nPosX = nPosY = 0;

	POINT ptBkPos = m_ptBkPos;
#ifdef _INET_PET
	for (int check = 0; check < 3; check++)
	{
		g_pShuttleChild->GetPetManager()->SetSelectingCheck(check, FALSE);
		g_pShuttleChild->GetPetManager()->SetSelectingCheckSocket(check, FALSE);
	}
#endif	
	for(int i=0;i<INVEN_Y_NUMBER;i++)
	{
		for(int j=0;j<INVEN_X_NUMBER;j++)
		{			
			nIdxPos = i*INVEN_X_NUMBER+j;
			INVEN_DISPLAY_INFO*  pInvenDisplayInfo = pParent->GetInvenDisplayInfo(nIdxPos);
			if(pInvenDisplayInfo)
			{				
				BOOL bMultiSel = FALSE;
				if(g_pD3dApp->CheckMultItemSel(pInvenDisplayInfo->pItem->UniqueNumber))
				{
					// ´ŮÁß Ľ±ĹĂŔĚ Ľş°řÇŃ ľĆŔĚĹŰ
					bMultiSel = TRUE;
				}

				nPosX = ptBkPos.x + EXTEND_INVEN_ITEM_SLOT_START_X + EXTEND_INVEN_SLOT_INTERVAL*j + 1;
				nPosY = ptBkPos.y + EXTEND_INVEN_ITEM_SLOT_START_Y + EXTEND_INVEN_SLOT_INTERVAL*i + 1;

				char buf[64];

				// 2009. 08. 27 by ckPark ±×·ˇÇČ ¸®ĽŇ˝ş şŻ°ć ˝Ă˝şĹŰ ±¸Çö

				//wsprintf(buf, "%08d", pInvenDisplayInfo->pItem->ItemInfo->SourceIndex);
				strcpy( buf, pInvenDisplayInfo->IconName );

				// end 2009. 08. 27 by ckPark ±×·ˇÇČ ¸®ĽŇ˝ş şŻ°ć ˝Ă˝şĹŰ ±¸Çö


				pIconInfo->Render(atol(pInvenDisplayInfo->IconName), nPosX,
					nPosY, 1.0f);
				//pIconInfo->Render();
				// Ä«żîĹÍşíľĆŔĚĹŰ Ľöş¸ŔĚ±â
				if( IS_COUNTABLE_ITEM(pInvenDisplayInfo->pItem->Kind) )
				{
					CItemInfo* pItemInfo = g_pStoreData->FindItemInInventoryByUniqueNumber( 
						pInvenDisplayInfo->pItem->UniqueNumber );
					if( pItemInfo->CurrentCount > 1 )
					{
						// °ąĽö¸¦ ş¸ż©ÁŘ´Ů.
						char buf[128];
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
						
						int len = strlen(buf) - 1;			// ż©±â´Â ÇŃ°ł ŔĚ»ó µéľîżÂ´Ů´Â Á¤ŔÇżˇ -1¸¦ Çß´Ů.

						
						int nFontPosX = ptBkPos.x + EXTEND_INVEN_ITEM_SLOT_START_X + EXTEND_INVEN_SLOT_INTERVAL*j + 21 - len*6; // ż©±âĽ­ 6Ŕş żµą® ĽýŔÚ ĹŘ˝şĆ® °Ł°ÝŔĚ´Ů.
						int nFontPosY = ptBkPos.y + EXTEND_INVEN_ITEM_SLOT_START_Y + EXTEND_INVEN_SLOT_INTERVAL*i - 1;
						
						
						m_pFontItemNum->DrawText(nFontPosX,nFontPosY, RGB(170,255,200),buf, 0L);
					}
				}
				else
				{//leg count
					CItemInfo* pItemInfo = g_pStoreData->FindItemInInventoryByUniqueNumber( 
						pInvenDisplayInfo->pItem->UniqueNumber );
					if (COMPARE_BIT_FLAG(pItemInfo->ItemInfo->ItemAttribute, ITEM_ATTR_LEGEND_ITEM) && pItemInfo->GetEnchantNumber() > 0)
					{
						int nSouls = (pItemInfo->ItemInfo->ReqMinLevel - 30)/5;
						char buf[128];
						wsprintf(buf, "%d",nSouls);
						int len = strlen(buf) - 1;
		
						int nFontPosX = ptBkPos.x + EXTEND_INVEN_ITEM_SLOT_START_X + EXTEND_INVEN_SLOT_INTERVAL*j + 21 - len*6;
						int nFontPosY = ptBkPos.y + EXTEND_INVEN_ITEM_SLOT_START_Y + EXTEND_INVEN_SLOT_INTERVAL*i - 1;
						
						m_pFontItemNum->DrawText(nFontPosX,nFontPosY+15, RGB(255,100,255),buf, 0L);
					}
					if(pItemInfo->GetEnchantNumber() > 0)
					{
						char buf[128];
						wsprintf(buf, "%d",pItemInfo->GetEnchantNumber());
						int len = strlen(buf) - 1;
		
						int nFontPosX = ptBkPos.x + EXTEND_INVEN_ITEM_SLOT_START_X + EXTEND_INVEN_SLOT_INTERVAL*j + 2 /*- len*5*/;
						int nFontPosY = ptBkPos.y + EXTEND_INVEN_ITEM_SLOT_START_Y + EXTEND_INVEN_SLOT_INTERVAL*i - 1;
						
						m_pFontItemNum->DrawText(nFontPosX,nFontPosY, RGB(255,187,61),buf, 0L);
					}
				}
				// 2010. 02. 11 by ckPark ąßµż·ů ŔĺÂřľĆŔĚĹŰ
				if( pInvenDisplayInfo->pItem->ItemInfo->InvokingDestParamID
					|| pInvenDisplayInfo->pItem->ItemInfo->InvokingDestParamIDByUse )
				{
					char buf[128];

					CItemInfo* pItemInfo = g_pStoreData->FindItemInInventoryByUniqueNumber( pInvenDisplayInfo->pItem->UniqueNumber );
					if( pItemInfo && GetString_CoolTime( pItemInfo, buf ) )
					{
						int len = strlen(buf) - 1;

						int nFontPosX = ptBkPos.x + EXTEND_INVEN_ITEM_SLOT_START_X + EXTEND_INVEN_SLOT_INTERVAL * j + 21 - len * 6; // ż©±âĽ­ 6Ŕş żµą® ĽýŔÚ ĹŘ˝şĆ® °Ł°ÝŔĚ´Ů.
						int nFontPosY = ptBkPos.y + EXTEND_INVEN_ITEM_SLOT_START_Y + EXTEND_INVEN_SLOT_INTERVAL * i + 7;

						m_pFontItemNum->DrawText(nFontPosX,nFontPosY, QSLOT_COUNTERBLE_NUMBER,buf, 0L);
					}
				}
				// end 2010. 02. 11 by ckPark ąßµż·ů ŔĺÂřľĆŔĚĹŰ
				// 2010. 05. 10 by jskim ˝Ĺ±Ô ·°Ĺ° ¸Ó˝Ĺ Ăß°ˇąćľČ - »óÁˇżˇĽ­ »çżë °ˇ´ÉÇŃ ľĆŔĚĹŰ¸¸ ş¸ż©ÁÖ±â

				CItemInfo* pItemInfo = g_pStoreData->FindItemInInventoryByUniqueNumber( pInvenDisplayInfo->pItem->UniqueNumber );
				if(pItemInfo && g_pInterface->m_pCityBase->GetCurrentBuildingNPC())
				{
					if(pItemInfo && ShopIsDisableInvenItem(g_pInterface->m_pCityBase->GetCurrentBuildingNPC()->buildingInfo.BuildingKind, pItemInfo))
					{
						m_pDisableItemImage->Move(nPosX, nPosY);
						m_pDisableItemImage->SetScale(pIconInfo->GetIconSize().cx, pIconInfo->GetIconSize().cy);
						m_pDisableItemImage->Render();
					}
				}
				//end 2010. 05. 10 by jskim ˝Ĺ±Ô ·°Ĺ° ¸Ó˝Ĺ Ăß°ˇąćľČ - »óÁˇżˇĽ­ »çżë °ˇ´ÉÇŃ ľĆŔĚĹŰ¸¸ ş¸ż©ÁÖ±â
#ifdef _INET_PET
				if (pItemInfo && g_pGameMain->m_pCharacterInfo->GetPartnerState() == CHARACTER_PARTNER && g_pGameMain->m_pCharacterInfo->IsShowWnd())
				{
					CItemInfo* pAllItem = g_pStoreData->FindItemInInventoryByWindowPos(POS_PET);

					if (pAllItem)
					{
						tPET_CURRENTINFO * tempCurrentInfo = g_pShuttleChild->GetPetManager()->GetPtr_PetCurrentData(pAllItem->UniqueNumber);
						tPET_LEVEL_DATA *psPetLevelData = g_pDatabase->GetPtr_PetLevelData(tempCurrentInfo->PetIndex, tempCurrentInfo->PetLevel);

						for (int num = 0; num < tempCurrentInfo->PetEnableSocketCount; num++)
						{
							if ((tempCurrentInfo->PetSocketItemUID[num] == NULL) &&
								(pItemInfo->ItemInfo->ArrDestParameter[0] == DES_PET_SOCKET_ITEM_AUTOKIT) ||
								(pItemInfo->ItemInfo->ArrDestParameter[0] == DES_PET_SOCKET_ITEM_AUTOSKILL))
							{
								m_pSelectPetSocketItemImage->Move(nPosX - 10, nPosY - 10);
								m_pSelectPetSocketItemImage->Render();
								g_pShuttleChild->GetPetManager()->SetSelectingCheckSocket(num, TRUE);

							}

						}

					}

					if (g_pShuttleChild->GetPetManager()->GetSelectSocket() == SOKET_TYPE_ITEM && pAllItem)
					{
						tPET_CURRENTINFO * tempCurrentInfo = g_pShuttleChild->GetPetManager()->GetPtr_PetCurrentData(pAllItem->UniqueNumber);
						tPET_LEVEL_DATA *psPetLevelData = g_pDatabase->GetPtr_PetLevelData(tempCurrentInfo->PetIndex, tempCurrentInfo->PetLevel);

						if (tempCurrentInfo &&
							pItemInfo->ItemInfo->ArrDestParameter[1] == DES_PET_SLOT_ITEM_AUTOKIT_HP  &&
							psPetLevelData->KitLevelHP >= pItemInfo->ItemInfo->ArrParameterValue[1])
						{
							m_pSelectPetkitItemImageHP->Move(nPosX - 10, nPosY - 10);
							m_pSelectPetkitItemImageHP->Render();
							g_pShuttleChild->GetPetManager()->SetSelectingCheck(0, TRUE);
						}

						else if (tempCurrentInfo &&
							pItemInfo->ItemInfo->ArrDestParameter[1] == DES_PET_SLOT_ITEM_AUTOKIT_SHIELD &&
							psPetLevelData->KitLevelShield >= pItemInfo->ItemInfo->ArrParameterValue[1])
						{
							m_pSelectPetkitItemImageSheld->Move(nPosX - 10, nPosY - 10);
							m_pSelectPetkitItemImageSheld->Render();
							g_pShuttleChild->GetPetManager()->SetSelectingCheck(1, TRUE);
						}

						else if (tempCurrentInfo &&
							pItemInfo->ItemInfo->ArrDestParameter[1] == DES_PET_SLOT_ITEM_AUTOKIT_SP &&
							psPetLevelData->KitLevelSP >= pItemInfo->ItemInfo->ArrParameterValue[1])
						{
							m_pSelectPetkitItemImageSP->Move(nPosX - 10, nPosY - 10);
							m_pSelectPetkitItemImageSP->Render();
							g_pShuttleChild->GetPetManager()->SetSelectingCheck(2, TRUE);
						}
					}
				}
#endif
				if(bMultiSel)
				{					
					m_pMultiItemSelImage->Move(nPosX, nPosY);
					m_pMultiItemSelImage->Render();
					
				}
			}
		}
	}
}
int CINFInvenItem::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL bShowInven)
{
	if(!IsShowWnd())
	{
		return INF_MSGPROC_NORMAL;
	}
#ifdef _INET_INVEN_RIGHT_CLICK_MENU	
	CINFInvenExtend* pParent = (CINFInvenExtend*)m_pParent;
	if (INF_MSGPROC_BREAK == pParent->WndProcItemMenuListWnd(uMsg, wParam, lParam)) {
		return INF_MSGPROC_BREAK;
	}
#endif
	switch(uMsg)
	{
	case WM_LBUTTONUP:
		{
			return OnLButtonUp(uMsg, wParam, lParam, bShowInven);
		}
		break;
	case WM_LBUTTONDOWN:
		{
			return OnLButtonDown(uMsg, wParam, lParam, bShowInven);
		}
		break;
	case WM_MOUSEMOVE:
		{
			return OnMouseMove(uMsg, wParam, lParam, bShowInven);
		}
		break;
	case WM_MOUSEWHEEL:
		{
			return OnMouseWhell(uMsg, wParam, lParam, bShowInven);
		}
		break;
	case WM_LBUTTONDBLCLK:
		{
			return OnLButtonDB(uMsg, wParam, lParam, bShowInven);
		}
		break;
	case WM_RBUTTONDOWN:
		{
			if(!bShowInven)
			{
				// ŔÎşĄŔĚ ľĆ´Ď¸é ŔĚżë ľČÇŃ´Ů.
				return INF_MSGPROC_NORMAL;
			}
			return OnRButtonDown(uMsg, wParam, lParam, bShowInven);
		}
		break;
	case WM_KEYDOWN:
		{
#ifndef _INET_INVEN_RIGHT_CLICK_MENU	
			if(!bShowInven)
			{
				// ŔÎşĄŔĚ ľĆ´Ď¸é ŔĚżë ľČÇŃ´Ů.
				return INF_MSGPROC_NORMAL;
			}

			switch( wParam )
			{
			case VK_CONTROL:
				{
					g_pD3dApp->OnCtrlBtnClick(TRUE);					
				}
				break;
			}
#else
			switch (wParam) {
				case VK_CONTROL:
					{
						g_pD3dApp->OnCtrlBtnClick(TRUE);
					}
					break;
#ifdef _INET_SELECT_ITEM_WITH_SHIFT
				case VK_SHIFT:
				{
					g_pD3dApp->OnShiftBtnClick(TRUE);
				}
				break;
#endif
			}
			return INF_MSGPROC_NORMAL;
#endif
		}
		break;
	case WM_KEYUP:
		{
#ifndef _INET_INVEN_RIGHT_CLICK_MENU	
			if(!bShowInven)
			{
				// ŔÎşĄŔĚ ľĆ´Ď¸é ŔĚżë ľČÇŃ´Ů.
				return INF_MSGPROC_NORMAL;
			}
			switch( wParam )
			{
			case VK_CONTROL:
				{					
					g_pD3dApp->OnCtrlBtnClick(FALSE);
				}
				break;
			}	
#else
			switch (wParam) {
				case VK_CONTROL:
					{
						g_pD3dApp->OnCtrlBtnClick(FALSE);
					}
					break;
#ifdef _INET_SELECT_ITEM_WITH_SHIFT
				case VK_SHIFT:
				{
					g_pD3dApp->OnShiftBtnClick(FALSE);
				}
				break;
#endif
			}
			return INF_MSGPROC_NORMAL;
#endif
		}
		break;
	}
	return INF_MSGPROC_NORMAL;
}

int CINFInvenItem::OnRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL bShowInven)
{
	POINT pt;
	pt.x = LOWORD(lParam);
	pt.y = HIWORD(lParam);
	CheckMouseReverse(&pt);

	if(!IsWndRect(pt) && !m_bMove)
	{
		return INF_MSGPROC_NORMAL;
	}

	CINFInvenExtend* pParent = (CINFInvenExtend*)m_pParent;

	int nItemPosX, nItemPosY;
	nItemPosX = nItemPosY =-1;	

	// ´ŮÁß Ľ±ĹĂ Ăł¸® °ąĽö
	int nSelItemCnt = g_pD3dApp->GetMultiSelectItem();

	// ż©±â şÎĹÍ´Â ľĆŔĚĹŰ Ľ±ĹĂ 
	if(!IsInvenRect(pt, &nItemPosX, &nItemPosY))
	{
		// żµżŞżˇ ľř´Ů.
		pParent->ClearMultiSeletItem();	// ´ŮÁßĽ±ĹĂ ĂĘ±âČ­		
		return INF_MSGPROC_NORMAL;
	}	
	
	
	INVEN_DISPLAY_INFO*  pInvenDisplayInfo = pParent->GetInvenDisplayInfo(nItemPosY*INVEN_X_NUMBER+nItemPosX);
	if(NULL == pInvenDisplayInfo)
	{
		return INF_MSGPROC_NORMAL;
	}
	
	if(nSelItemCnt <= 0 )
	{
		pParent->SetMultiSelectItem(NULL);	// ´ŮÁß Ăł¸® Á¦°Ĺ							
		// ˝Ě±Ű Ľ±ĹĂ
		pParent->SetSelectItem(pInvenDisplayInfo);
	}
	
	if(g_pGameMain->m_stSelectItem.pSelectItem &&
		g_pGameMain->m_stSelectItem.bySelectType == ITEM_INVEN_POS &&
		g_pInterface->m_pBazaarShop == NULL)
	{
		if(IS_ITEM_SHOP_TYPE(g_pInterface->m_pCityBase->GetCurrentBuildingNPC()->buildingInfo.BuildingKind))
		{
			CItemInfo* pItemInfo = (CItemInfo*)g_pGameMain->m_stSelectItem.pSelectItem->pItem;
			if(pItemInfo->Wear == WEAR_NOT_ATTACHED )
			{
				if( IS_COUNTABLE_ITEM(pItemInfo->Kind))
				{
					char buf[128];
					ITEM *pITEM = pItemInfo->GetItemInfo();
					if(pITEM)
					{
						char temp1[MAX_PATH];
						char temp2[MAX_PATH];
						wsprintf( temp1, "%d", CAtumSJ::GetItemSellingPriceAtShop(pITEM) );
						MakeCurrencySeparator( temp2, temp1, 3, ',' );
						wsprintf( buf, STRMSG_C_SHOP_0007, pITEM->ItemName, pITEM->MinTradeQuantity, temp2 );//"%s ¸î°ł ĆÄ˝Ă°Ú˝Ŕ´Ď±î?[°ˇ°Ý:%d°ł´ç %s(˝şÇÇ)]"
						
						// 2005-05-09 by ydkim	ż©·Ż°łŔÇ ¸ŢĽĽÁöąÚ˝ş Á¦°Ĺ
						if(g_pGameMain->m_pInfWindow->IsExistMsgBox(_Q_SHOP_SELL_ITEM))
						{
							g_pGameMain->m_pInfWindow->DeleteMsgBox(_Q_SHOP_SELL_ITEM);
						}
						else if(g_pGameMain->m_pInfWindow->IsExistMsgBox(_Q_SHOP_SELL_ENERGY))
						{
							g_pGameMain->m_pInfWindow->DeleteMsgBox(_Q_SHOP_SELL_ENERGY);
						}
						// 2007-02-12 by bhsohn Item ´ŮÁß Ľ±ĹĂ Ăł¸®
						else if(g_pGameMain->m_pInfWindow->IsExistMsgBox(_Q_SHOP_MULTI_SELL_ITEM ))
						{
							g_pGameMain->m_pInfWindow->DeleteMsgBox(_Q_SHOP_MULTI_SELL_ITEM );
						}
						else if(g_pGameMain->m_pInfWindow->IsExistMsgBox(_Q_STORE_MULTI_PUT_ITEM ))
						{
							g_pGameMain->m_pInfWindow->DeleteMsgBox(_Q_STORE_MULTI_PUT_ITEM );
						}								
						else if(g_pGameMain->m_pInfWindow->IsExistMsgBox(_Q_STORE_MULTI_GET_ITEM ))
						{
							g_pGameMain->m_pInfWindow->DeleteMsgBox(_Q_STORE_MULTI_GET_ITEM );
						}																
						// end 2007-02-12 by bhsohn Item ´ŮÁß Ľ±ĹĂ Ăł¸®								
						g_pGameMain->m_pInfWindow->AddMsgBox(buf, _Q_SHOP_SELL_ENERGY,
#ifdef _M_X64
							(DWORD_PTR)pItemInfo, 
#else
							(DWORD)pItemInfo, 
#endif
							pItemInfo->CurrentCount);
					}
					else
					{
						g_pD3dApp->m_pChat->CreateChatChild( STRMSG_C_SERVER_0004, COLOR_SYSTEM );//"Ľ­ąö·ÎşÎĹÍ Á¤ş¸ ąŢ´Â Áß... ´Ů˝Ă ˝Ăµµ ÇĎĽĽżä."
					}
				}
				else
				{
					char buf[128];
					ITEM *pITEM = pItemInfo->GetItemInfo();
					if(pITEM)
					{
						char temp1[MAX_PATH];
						char temp2[MAX_PATH];
						wsprintf( temp1, "%d", CAtumSJ::GetItemSellingPriceAtShop(pITEM) );
						MakeCurrencySeparator( temp2, temp1, 3, ',' );
						wsprintf( buf, STRMSG_C_SHOP_0009, pITEM->ItemName, temp2);//"%s ¸¦ ĆÄ˝Ă°Ú˝Ŕ´Ď±î?[°ˇ°Ý:%s(˝şÇÇ)]"
						
						// 2005-05-09 by ydkim	ż©·Ż°łŔÇ ¸ŢĽĽÁöąÚ˝ş Á¦°Ĺ
						if(g_pGameMain->m_pInfWindow->IsExistMsgBox(_Q_SHOP_SELL_ITEM))
						{
							g_pGameMain->m_pInfWindow->DeleteMsgBox(_Q_SHOP_SELL_ITEM);
						}
						else if(g_pGameMain->m_pInfWindow->IsExistMsgBox(_Q_SHOP_SELL_ENERGY))
						{
							g_pGameMain->m_pInfWindow->DeleteMsgBox(_Q_SHOP_SELL_ENERGY);
						}
						// 2007-02-12 by bhsohn Item ´ŮÁß Ľ±ĹĂ Ăł¸®
						else if(g_pGameMain->m_pInfWindow->IsExistMsgBox(_Q_SHOP_MULTI_SELL_ITEM ))
						{
							g_pGameMain->m_pInfWindow->DeleteMsgBox(_Q_SHOP_MULTI_SELL_ITEM );
						}
						else if(g_pGameMain->m_pInfWindow->IsExistMsgBox(_Q_STORE_MULTI_PUT_ITEM ))
						{
							g_pGameMain->m_pInfWindow->DeleteMsgBox(_Q_STORE_MULTI_PUT_ITEM );
						}								
						else if(g_pGameMain->m_pInfWindow->IsExistMsgBox(_Q_STORE_MULTI_GET_ITEM ))
						{
							g_pGameMain->m_pInfWindow->DeleteMsgBox(_Q_STORE_MULTI_GET_ITEM );
						}																
						// end 2007-02-12 by bhsohn Item ´ŮÁß Ľ±ĹĂ Ăł¸®								
						
						g_pGameMain->m_pInfWindow->AddMsgBox(buf, _Q_SHOP_SELL_ITEM,
#ifdef _M_X64
							(DWORD_PTR)pItemInfo);
#else
							(DWORD)pItemInfo);
#endif
					}
					else
					{
						g_pD3dApp->m_pChat->CreateChatChild( STRMSG_C_SHOP_0008, COLOR_SYSTEM );//"Ľ­ąö·ÎşÎĹÍ Á¤ş¸ ąŢ´ÂÁß...´Ů˝Ă ˝ĂµµÇĎĽĽżä."
					}
				}
			}
			else
			{
				g_pGameMain->m_pInfWindow->AddMsgBox( STRMSG_C_SHOP_0010, _MESSAGE );//"ŔĺÂřµČ ľĆŔĚĹŰŔş ĆČ Ľö ľř˝Ŕ´Ď´Ů."
			}
		}
	}
	// 2007-02-12 by bhsohn Item ´ŮÁß Ľ±ĹĂ Ăł¸®
	// ´ŮÁß ľĆŔĚĹŰ Ľ±ĹĂ
	else if(nSelItemCnt > 0)
	{			
		if(g_pD3dApp->CheckMultItemSel(pInvenDisplayInfo->pItem->UniqueNumber))
		{			
			// ´ŮÁß Ľ±ĹĂŔĚ Ľş°řÇŃ ľĆŔĚĹŰ
			// ´ŮÁß ľĆŔĚĹŰ Ľ±ĹĂ Ă˘ ĆËľ÷ 
			g_pGameMain->PopupMultiItemSelect();				
		}
		else
		{
			pParent->SetMultiSelectItem(NULL);	// ´ŮÁß Ăł¸® Á¦°Ĺ
		}
		
	}
	// end 2007-02-12 by bhsohn Item ´ŮÁß Ľ±ĹĂ Ăł¸®
	pParent->SetSelectItem(NULL);
	pParent->SetItemInfo(NULL, 0, 0);
	
	return INF_MSGPROC_NORMAL;
}
int CINFInvenItem::OnMouseWhell(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL bShowInven)
{
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(g_pD3dApp->GetHwnd(), &pt);
	CheckMouseReverse(&pt);			
	BOOL bClick = FALSE;			
	
	{
		bClick = m_pINFInvenScrollBar->IsMouseWhellPos(pt);
		if(bClick)		
		{	
			// 2011-08-17 by hsson ŔÎşĄ ¸¶żě˝ş·Î ľĆŔĚĹŰŔ» µç »óĹÂżˇĽ­ ČŢÁöĹëżˇ ąö¸®¸é ´Ů¸Ą ľĆŔĚĹŰ Á¤ş¸ łŞżŔ´ř ą®Á¦
			g_pD3dApp->m_pInterface->m_pGameMain->m_pInven->SetSelectItem(NULL);
			// end 2011-08-17 by hsson ŔÎşĄ ¸¶żě˝ş·Î ľĆŔĚĹŰŔ» µç »óĹÂżˇĽ­ ČŢÁöĹëżˇ ąö¸®¸é ´Ů¸Ą ľĆŔĚĹŰ Á¤ş¸ łŞżŔ´ř ą®Á¦		

			int nOldScroll = m_pINFInvenScrollBar->GetScrollStep();
			m_pINFInvenScrollBar->OnMouseWheel(wParam, lParam);					
			int nNewScroll = m_pINFInvenScrollBar->GetScrollStep();
			if(nOldScroll != nNewScroll)
			{
				UpdateInvenScroll(); // żäĂ»
			}			
			return INF_MSGPROC_BREAK;
		}
		
	}
	if(!IsWndRect(pt))
	{
		return INF_MSGPROC_NORMAL;
	}
	return INF_MSGPROC_NORMAL;
}


int CINFInvenItem::OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL bShowInven)
{
	POINT pt;
	pt.x = LOWORD(lParam);
	pt.y = HIWORD(lParam);
	CheckMouseReverse(&pt);

	{
		BOOL bClick = m_pINFInvenScrollBar->GetMouseMoveMode();
		if(bClick)
		{
			m_pINFInvenScrollBar->SetMouseMoveMode(FALSE);
			return INF_MSGPROC_BREAK;
		}				
	}
	{
		if(m_bMove)
		{
			m_bMove = FALSE;
			return INF_MSGPROC_BREAK;
		}		
	}
	CINFInvenExtend* pParent = (CINFInvenExtend*)m_pParent;
	{
		if(TRUE == m_pCloseBtn->OnLButtonUp(pt))
		{	
			GUI_BUILDINGNPC* pTempBase = g_pInterface->m_pCityBase->GetCurrentBuildingNPC();
			if(!pTempBase)
			{
				// »óÁˇŔĚ ľřŔ»‹š¸¸ Ľű±ä´Ů.
				ShowWnd(FALSE, NULL);
				// Ľű±â±â			
				// ąöĆ° Ĺ¬¸Ż 
				if(g_pD3dApp->m_pSound)			
					g_pD3dApp->m_pSound->PlayD3DSound(SOUND_SELECT_BUTTON, D3DXVECTOR3(0,0,0), FALSE);			
				return  INF_MSGPROC_BREAK;
			}
		}
	}

	if(!IsWndRect(pt) && !m_bMove)
	{
		return INF_MSGPROC_NORMAL;
	}
	
#ifdef _INET_SORT_INV
	{
		if (TRUE == m_pEQSort->OnLButtonUp(pt))
		{
			g_pStoreData->ResortingItemInInventorySort();
			if (g_pGameMain && g_pGameMain->m_pTrade && g_pGameMain->m_pTrade->m_bTrading)
			{
				g_pGameMain->m_pInven->SetAllIconInfo();
			}
			else
			{
				g_pGameMain->m_pInven->ShowInven(NULL, NULL, TRUE);
				g_pGameMain->LeftWindowShow(TRUE, 1);
				if(g_pD3dApp->m_pSound)
					g_pD3dApp->m_pSound->PlayD3DSound(SOUND_SELECT_BUTTON, D3DXVECTOR3(0, 0, 0), FALSE);
				return INF_MSGPROC_BREAK;
			}
		}
	}
#endif

	{
		// 2008-12-02 by dgwoo ·°Ĺ° ¸Ó˝ĹĂł¸®.
		GUI_BUILDINGNPC* pTempBase = g_pInterface->m_pCityBase->GetCurrentBuildingNPC();
		BYTE  nBuildingNum = 0;
		if(pTempBase)
			nBuildingNum =  pTempBase->buildingInfo.BuildingKind;
		// 2008-12-02 by dgwoo ·°Ĺ° ¸Ó˝ĹĂł¸®.

		if(TRUE == m_pEqShow->OnLButtonUp(pt) &&
			BUILDINGKIND_LUCKY != nBuildingNum)
		{
			
			pParent->ShowEqInven();
			// ąöĆ° Ĺ¬¸Ż 	
			if(g_pD3dApp->m_pSound)			
				g_pD3dApp->m_pSound->PlayD3DSound(SOUND_SELECT_BUTTON, D3DXVECTOR3(0,0,0), FALSE);			
			return  INF_MSGPROC_BREAK;
		}
	}
	
	int nWindowPosX = m_ptBkPos.x;
	int nWindowPosY = m_ptBkPos.y;

	if( g_pGameMain->m_pQuickSlot->m_nItemType == QSLOT_ITEMTYPE_SKILL || 
				g_pGameMain->m_pQuickSlot->m_nItemType == QSLOT_ITEMTYPE_ITEM)
	{
		// Äü˝˝·Ô ľĆĹŰ Á¦żÜÄÚµĺ
		return INF_MSGPROC_NORMAL;
	}
	
	// Äü˝˝·ÔżˇĽ­ LBUTTONUPŔĎ ¶§
	if(g_pGameMain->m_pQuickSlot->LButtonUpQuickSlot(pt))
	{
		// Äü˝˝·ÔżˇĽ­´Â SelectItem »čÁ¦ ľČÇÔ
		return INF_MSGPROC_NORMAL;
	}

	CItemInfo* pSelectItem = NULL;
	if(g_pGameMain->m_stSelectItem.pSelectItem)
	{
		pSelectItem = (CItemInfo*)(g_pGameMain->m_stSelectItem.pSelectItem->pItem); 
	}
	else
	{
		// ŔÎşĄżˇ Ľ±ĹĂÇŃ ľĆŔĚĹŰŔĚ ľř´Ů.
		return INF_MSGPROC_NORMAL;
	}
	
	switch(g_pGameMain->m_stSelectItem.bySelectType)
	{
	case ITEM_INVEN_POS:
		{
			// ŔÎşĄ ľĆŔĚĹŰ Ľ±ĹĂ
			if(INF_MSGPROC_BREAK == OnLButtonUpInvenPosItem(pt, pSelectItem))
			{
				return INF_MSGPROC_BREAK;
			}
		}
		break;
	case ITEM_STORE_POS:
		{
			// Ă˘°í ľĆŔĚĹŰ Ľ±ĹĂ
			if(INF_MSGPROC_BREAK == OnLButtonUpStorePosItem(pt, pSelectItem))
			{
				return INF_MSGPROC_BREAK;
			}
		}
		break;
	}	
	return INF_MSGPROC_NORMAL;
}

int CINFInvenItem::OnLButtonUpInvenPosItem(POINT pt, CItemInfo* pSelectItem)
{
	CINFInvenExtend* pParent = (CINFInvenExtend*)m_pParent;
	int nWindowPosX = m_ptBkPos.x;
	int nWindowPosY = m_ptBkPos.y; 

	// ľĆŔĚĹŰ -> ĆÄ±â
	if(pSelectItem && pt.x>nWindowPosX+INVEN_GARBAGE_START_X && pt.x<nWindowPosX+INVEN_GARBAGE_START_X+INVEN_GARBAGE_SIZE &&
		pt.y>nWindowPosY+INVEN_GARBAGE_START_Y && pt.y<nWindowPosY+INVEN_GARBAGE_START_Y + INVEN_GARBAGE_SIZE)
	{
		if( !g_pGameMain->m_pInfWindow->IsExistMsgBox(_Q_ITEM_DELETE) &&
			!g_pGameMain->m_pInfWindow->IsExistMsgBox(_Q_ITEM_DELETE_NUM))
		{
			char buf[256];
			ITEM *item = g_pDatabase->GetServerItemInfo(pSelectItem->ItemNum);


			// 2008-12-04 by dgwoo ŔĚąĚ »óÁˇ, ¶Ç´Â °Ĺ·ˇĂ˘żˇ żĂ¶ó°Ł ľĆŔĚĹŰŔş »čÁ¦ÇŇĽöľřŔ˝.
			UID64_t nItemUID = 0;
			BOOL bPass = TRUE;
			BOOL bLuckyMechine = FALSE;
			GUI_BUILDINGNPC* pNpc = g_pInterface->m_pCityBase->GetCurrentBuildingNPC();
			if(pNpc)
			{
				if(pNpc->buildingInfo.BuildingKind == BUILDINGKIND_LUCKY)
				{// ·°Ĺ° ¸Ó˝Ĺ.
					CMapCityShopIterator itLucky = g_pInterface->m_pCityBase->m_mapCityShop.find(BUILDINGKIND_LUCKY);
					if(itLucky != g_pInterface->m_pCityBase->m_mapCityShop.end()) 
					{
						//if(pSelectItem->UniqueNumber == ((CINFLuckyMachine*)itLucky->second)->GetSelUID())
						bPass = FALSE;
						bLuckyMechine = TRUE;
					}
				}
				if(pNpc->buildingInfo.BuildingKind == BUILDINGKIND_FACTORY)
				{// ĆŃĹä¸®.
					CMapCityShopIterator it = g_pInterface->m_pCityBase->m_mapCityShop.find(BUILDINGKIND_FACTORY);
					
					if(it != g_pInterface->m_pCityBase->m_mapCityShop.end()) 
					{
						CINFCityLab* pCityLab = ((CINFCityLab*)it->second);
						if(pCityLab->FindItemFromSource(pSelectItem->UniqueNumber))
							bPass = FALSE;
					}
				}

				if(pNpc->buildingInfo.BuildingKind == BUILDINGKIND_LABORATORY)
				{// ż¬±¸ĽŇ
					CMapCityShopIterator it = g_pInterface->m_pCityBase->m_mapCityShop.find(BUILDINGKIND_LABORATORY);
					
					if(it != g_pInterface->m_pCityBase->m_mapCityShop.end()) 
					{
						CINFCityLab* pCityLab = ((CINFCityLab*)it->second);
						if(pCityLab->FindItemFromSource(pSelectItem->UniqueNumber))
							bPass = FALSE;
					}
				}

				
			}
			if(g_pGameMain)
			{// °Ĺ·ˇ˝Ă..
				if(g_pGameMain->m_pTrade)
				{
					if(NULL != g_pGameMain->m_pTrade->FindTradeMyItem(pSelectItem->UniqueNumber))
					{
						bPass = FALSE;
					}
				}
			}
			if(g_pInterface)
			{
				if(g_pInterface->m_pBazaarShop)
				{//°łŔÎ »óÁˇ.
					if(g_pInterface->m_pBazaarShop->GetShopItemInfo(pSelectItem->UniqueNumber))
					{
						bPass = FALSE;	
					}

				}
			}
			if(!bPass)
			{
				if(bLuckyMechine)
				{
					//"\y·°Ĺ°¸Ó˝Ĺ ŔĚżëÁßżˇ´Â ľĆŔĚĹŰŔ» »čÁ¦ ÇŇ Ľö ľř˝Ŕ´Ď´Ů"
					g_pD3dApp->m_pChat->CreateChatChild(STRMSG_C_090324_0201,COLOR_ERROR);
				}
				else
				{
					g_pD3dApp->m_pChat->CreateChatChild(STRMSG_C_081203_0101,COLOR_ERROR);
				}				
			}
			// 2008-12-04 by dgwoo ŔĚąĚ »óÁˇ, ¶Ç´Â °Ĺ·ˇĂ˘żˇ żĂ¶ó°Ł ľĆŔĚĹŰŔş »čÁ¦ÇŇĽöľřŔ˝.

			if(item && bPass && !m_bTradeItemCenterState)
			{
				if(pSelectItem->ItemWindowIndex < POS_ITEMWINDOW_OFFSET)
				{
					g_pGameMain->m_pInfWindow->AddMsgBox(STRMSG_C_ITEM_0008, _MESSAGE);//"ŔĺÂřµČ ľĆŔĚĹŰŔş ąö¸± Ľö ľř˝Ŕ´Ď´Ů."
					pSelectItem = NULL;
				}
				else
				{
					if(IS_COUNTABLE_ITEM(pSelectItem->Kind) && pSelectItem->CurrentCount>1)
					{
						wsprintf(buf, STRMSG_C_ITEM_0009, item->ItemName);//"ľĆŔĚĹŰ %s ¸î°ł¸¦ ąö¸®˝Ă°Ú˝Ŕ´Ď±î?"
						g_pGameMain->m_pInfWindow->AddMsgBox(buf, 
							_Q_ITEM_DELETE_NUM, 
#ifdef _M_X64
							(DWORD_PTR)pSelectItem,
#else
							(DWORD)pSelectItem,
#endif
							pSelectItem->CurrentCount);
					}
					else
					{
						wsprintf(buf, STRMSG_C_ITEM_0010, item->ItemName);//"ľĆŔĚĹŰ %s ¸¦(Ŕ»)  ąö¸®˝Ă°Ú˝Ŕ´Ď±î?"
						g_pGameMain->m_pInfWindow->AddMsgBox(buf, _Q_ITEM_DELETE);
					}
					
					// 2007-06-20 by bhsohn ľĆŔĚĹŰ »čÁ¦˝Ă, ¸Ţ¸đ¸® ąö±× ĽöÁ¤
					//m_pDeleteItem = pSelectItem;
					pParent->SetDeleteItemInfo(pSelectItem);							
					return INF_MSGPROC_NORMAL;
				}				
			}
		}
	}
	int nItemPosX, nItemPosY;
	nItemPosX = nItemPosY= -1;

	if(!IsInvenRect(pt, &nItemPosX, &nItemPosY))
	{
		// żµżŞżˇ ľř´Ů.
		return INF_MSGPROC_NORMAL;
	}

	int nScrollStep = m_pINFInvenScrollBar->GetScrollStep();	
	int nWindowPosition = (nItemPosY*INVEN_X_NUMBER)+nItemPosX+(nScrollStep*INVEN_X_NUMBER)+POS_ITEMWINDOW_OFFSET;
	
	if (pSelectItem) { //crashed once here


		if (pSelectItem->ItemWindowIndex
			&& nWindowPosition != pSelectItem->ItemWindowIndex
			&& pSelectItem->ItemWindowIndex >= POS_ITEMWINDOW_OFFSET)
		{	// ľĆŔĚĹŰ -> ľĆŔĚĹŰ
			if (pSelectItem->ItemWindowIndex < nWindowPosition)
			{
				INVEN_DISPLAY_INFO* pSelectInvenDisplayInfo = pParent->GetInvenDisplayInfo(pSelectItem->ItemWindowIndex);
				for (int i = pSelectItem->ItemWindowIndex + 1; i <= nWindowPosition; i++)
				{
					CMapItemWindowInventoryIterator it = g_pStoreData->m_mapItemWindowPosition.find(i);
					if (it != g_pStoreData->m_mapItemWindowPosition.end())
					{
						CItemInfo* pSwapItem = it->second;
						pSwapItem->ItemWindowIndex = i - 1;
						g_pStoreData->m_mapItemWindowPosition[i - 1] = pSwapItem;
						pParent->SetSingleInvenIconInfo(pSwapItem);
					}
					else {
						pSelectItem->ItemWindowIndex = i - 1;
						g_pStoreData->m_mapItemWindowPosition[i - 1] = pSelectItem;
						pParent->SetAllIconInfo();
						pParent->SetSelectItem(NULL);
						return INF_MSGPROC_NORMAL;
					}
				}
				pSelectItem->ItemWindowIndex = nWindowPosition;
				g_pStoreData->m_mapItemWindowPosition[nWindowPosition] = pSelectItem;
				pParent->SetSingleInvenIconInfo(pSelectItem);
				pParent->SetAllIconInfo();
			}
			else if (pSelectItem->ItemWindowIndex > nWindowPosition)
			{
				INVEN_DISPLAY_INFO* pSelectInvenDisplayInfo = pParent->GetInvenDisplayInfo(pSelectItem->ItemWindowIndex);
				for (int i = pSelectItem->ItemWindowIndex - 1; i >= nWindowPosition; i--)
				{
					CMapItemWindowInventoryIterator it = g_pStoreData->m_mapItemWindowPosition.find(i);
					if (it != g_pStoreData->m_mapItemWindowPosition.end())
					{
						CItemInfo* pSwapItem = it->second;
						pSwapItem->ItemWindowIndex = i + 1;
						g_pStoreData->m_mapItemWindowPosition[i + 1] = pSwapItem;
						pParent->SetSingleInvenIconInfo(pSwapItem);
					}
					else {
						DBGOUT("ERROR : CINFInven::WndProc() Item List crashed!!!!!\n");
						pParent->SetSelectItem(NULL);
						return INF_MSGPROC_NORMAL;// ľĆŔĚĹŰ ¸®˝şĆ®°ˇ ±úÁř°ćżě
					}
				}
				pSelectItem->ItemWindowIndex = nWindowPosition;
				g_pStoreData->m_mapItemWindowPosition[nWindowPosition] = pSelectItem;
				pParent->SetSingleInvenIconInfo(pSelectItem);
				pParent->SetAllIconInfo();
			}

			g_pGameMain->SetToolTip(NULL, 0, 0);
			pParent->SetItemInfo(NULL, 0, 0);
			pParent->SetMultiSelectItem(NULL);	// ´ŮÁß Ăł¸® Á¦°Ĺ
			pParent->SetSelectItem(NULL);
		}
		// ŔĺÂřĂ˘ -> ľĆŔĚĹŰ
		else if (pSelectItem->ItemWindowIndex >= 0 && pSelectItem->ItemWindowIndex < POS_ITEMWINDOW_OFFSET)
		{
			// 2004-12-10 by jschoi
			if (pParent->IsAbleReleaseItem(pSelectItem, pSelectItem->ItemWindowIndex))
			{
				// send item windowNumber (socket)
				MSG_FC_ITEM_CHANGE_WINDOW_POSITION* pMsg;
				char buffer[SIZE_MAX_PACKET];
				*(MessageType_t*)buffer = T_FC_ITEM_CHANGE_WINDOW_POSITION;
				pMsg = (MSG_FC_ITEM_CHANGE_WINDOW_POSITION*)(buffer + SIZE_FIELD_TYPE_HEADER);
				pMsg->CharacterUniqueNumber = g_pD3dApp->m_pShuttleChild->m_myShuttleInfo.CharacterUniqueNumber;
				pMsg->FromItemUniqueNumber = pSelectItem->UniqueNumber;
				pMsg->FromItemWindowIndex = pSelectItem->ItemWindowIndex;
				pMsg->ToItemUniqueNumber = 0;
				// şóŔÚ¸® ąřČŁ·Î ĽĽĆĂÇŃ´Ů.			
				int i = POS_ITEMWINDOW_OFFSET + g_pStoreData->m_mapItemWindowPosition.size();//-count-1;
				pMsg->ToItemWindowIndex = i;

				// ŔĺÂř ľĆŔĚĹŰŔ» ŔÎşĄĹä¸®·Î ł»¸±¶§ ÇŘ´ç ˝şĹłŔ» ż©±âĽ­ ÇŘÁ¦ÇŘ ÁÜ..Ľ­ąö żäĂ»
				if (pSelectItem)
				{
					g_pShuttleChild->m_pSkill->DeleteSkillFromWearItem(pSelectItem->Kind);
				}
				g_pD3dApp->m_pFieldWinSocket->Write(buffer, SIZE_FIELD_TYPE_HEADER + sizeof(MSG_FC_ITEM_CHANGE_WINDOW_POSITION));
				//						g_pD3dApp->m_bRequestEnable = FALSE;			// 2006-06-19 by ispark, ¸ŢĽĽÁö ŔŔ´äŔ» ±â´Ů¸°´Ů.

				g_pGameMain->SetToolTip(NULL, 0, 0);
				pParent->SetItemInfo(NULL, 0, 0);
				pParent->SetMultiSelectItem(NULL);	// ´ŮÁß Ăł¸® Á¦°Ĺ
				pParent->SetSelectItem(NULL);

				return INF_MSGPROC_BREAK;
			}
			else
			{
				char buf[128];
				wsprintf(buf, STRMSG_C_ITEM_0007);//"żä±¸ ˝şĹČŔ¸·Î ŔÎÇĎż© ÇŘ´ç ľĆŔĚĹŰŔ» ÇŘÁ¦ ÇŇ Ľö ľř˝Ŕ´Ď´Ů."
				g_pD3dApp->m_pChat->CreateChatChild(buf, COLOR_ERROR);

				g_pGameMain->SetToolTip(NULL, 0, 0);
				pParent->SetItemInfo(NULL, 0, 0);
				pParent->SetMultiSelectItem(NULL);	// ´ŮÁß Ăł¸® Á¦°Ĺ
				pParent->SetSelectItem(NULL);
			}
		}
	}
	return INF_MSGPROC_NORMAL;
}

int CINFInvenItem::OnLButtonUpStorePosItem(POINT pt, CItemInfo* pSelectItem)
{
	CINFInvenExtend* pParent = (CINFInvenExtend*)m_pParent;
	int nWindowPosX = m_ptBkPos.x;
	int nWindowPosY = m_ptBkPos.y; 

	// µĺ·ą±× ľĆŔĚÄÜ ĂĘ±âČ­
	// Ă˘°í -> ŔÎşĄĹä¸®
	GUI_BUILDINGNPC* pNpc = g_pInterface->m_pCityBase->GetCurrentBuildingNPC();
	
	if(!pNpc 
		|| !(IS_STORE_SHOP_TYPE(pNpc->buildingInfo.BuildingKind))
		|| !g_pGameMain->m_stSelectItem.pSelectItem)		
	{
		return INF_MSGPROC_NORMAL;
	}
	int nItemPosX, nItemPosY;
	nItemPosX = nItemPosY= -1;

	if(!IsInvenRect(pt, &nItemPosX, &nItemPosY))
	{
		// żµżŞżˇ ľř´Ů.
		return INF_MSGPROC_NORMAL;
	}
	
	CItemInfo* pItemInfo = (CItemInfo*)g_pGameMain->m_stSelectItem.pSelectItem->pItem;
	
	int nStoreMultiItem = g_pGameMain->GetCityStoreMultiSelectItem();
	if(nStoreMultiItem <= 0)
	{
		if(IS_COUNTABLE_ITEM(g_pGameMain->m_stSelectItem.pSelectItem->pItem->Kind))
		{
			char buf[256];
			wsprintf( buf, STRMSG_C_STORE_0003, pItemInfo->ItemInfo->ItemName);//"%s ¸î°łŔÇ ľĆŔĚĹŰŔ» ĂŁŔ¸˝Ă°Ú˝Ŕ´Ď±î?"
			g_pGameMain->m_pInfWindow->AddMsgBox(buf, _Q_STORE_PUSH_ITEM,
#ifdef _M_X64
				(DWORD_PTR)pItemInfo,
#else
				(DWORD)pItemInfo,
#endif
				pItemInfo->CurrentCount);
		}
		else
		{
			CINFCityStore* pStore = (CINFCityStore*)g_pInterface->m_pCityBase->FindBuildingShop(BUILDINGKIND_STORE);
			if(pStore)
			{
				pStore->FieldSocketSendItemToCharacter( pItemInfo->UniqueNumber, 1 );
			}
		}
	}
	else
	{
		g_pGameMain->PopupStoreMultiItemSelect();
	}
	
	pParent->SetSelectItem(NULL);	
	return  INF_MSGPROC_BREAK;	
}

int nCountOfSended = 0;
int CINFInvenItem::OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL bShowInven)
{
	POINT ptBkPos = m_ptBkPos;
	CINFInvenExtend* pParent = (CINFInvenExtend*)m_pParent;

	POINT pt;
	pt.x = LOWORD(lParam);
	pt.y = HIWORD(lParam);
	CheckMouseReverse(&pt);
	if(!IsWndRect(pt) && !m_bMove)
	{
		return INF_MSGPROC_NORMAL;
	}
	pParent->SetWndOrder(INVEN_ITEM_WND);

	{
		if(TRUE == m_pCloseBtn->OnLButtonDown(pt))
		{
			// ąöĆ°Ŕ§żˇ ¸¶żě˝ş°ˇ ŔÖ´Ů.
			return  INF_MSGPROC_BREAK;
		}		
	}
	{
		GUI_BUILDINGNPC* pTempBase = g_pInterface->m_pCityBase->GetCurrentBuildingNPC();			

		if(IsMouseCaps(pt) && !pTempBase)
		{
			m_ptCommOpMouse.x = pt.x - m_ptBkPos.x;
			m_ptCommOpMouse.y = pt.y - m_ptBkPos.y;
			m_bMove = TRUE;

			g_pGameMain->SetToolTip(NULL, 0, 0);
			pParent->SetItemInfo(NULL, 0, 0);
			pParent->SetMultiSelectItem(NULL);	// ´ŮÁß Ăł¸® Á¦°Ĺ

			return INF_MSGPROC_BREAK;
		}
	}
	int nItemPosX, nItemPosY;
	nItemPosX = nItemPosY =-1;
	
	{
		BOOL bClick = m_pINFInvenScrollBar->IsMouseBallPos(pt);
		if(bClick)
		{
			m_pINFInvenScrollBar->SetMouseMoveMode(TRUE);
			return INF_MSGPROC_BREAK;
		}
	}
	
	
	{
		if(TRUE == m_pEqShow->OnLButtonDown(pt))
		{
			// ąöĆ°Ŕ§żˇ ¸¶żě˝ş°ˇ ŔÖ´Ů.
			return  INF_MSGPROC_BREAK;
		}		
	}
#ifdef _INET_SORT_INV
	{
		if (TRUE == m_pEQSort->OnLButtonDown(pt))
		{
			return INF_MSGPROC_BREAK;
		}
	}
#endif
	// SPIĂ˘
	if((pt.x > (ptBkPos.x+INVEN_SPI_START_X-100))
		&& (pt.x < ptBkPos.x+INVEN_SPI_START_X-100+ INVEN_SPI_WIDTH) 
		&& (pt.y > (ptBkPos.y+INVEN_SPI_START_Y))
		&&(pt.y < (ptBkPos.y+INVEN_SPI_START_Y+INVEN_SPI_HEIGHT)))
	{
		GUI_BUILDINGNPC* pBuilding = g_pInterface->m_pCityBase->GetCurrentBuildingNPC();	

		if( NULL == pBuilding && LEFT_WINDOW_TRANS == g_pGameMain->m_nLeftWindowInfo)		
		{
			// ĂÖ´ë Ĺ©±â ş¸´Ů Ĺ©¸é ¸®ĹĎÇŃ´Ů.
			if(!g_pGameMain->m_pInfWindow->IsExistMsgBox(_Q_TRADE_ITEM_NUMBER))
			{
				if(g_pGameMain->m_pInven->GetItemSpi()>0)
				{						
					g_pGameMain->m_pInfWindow->AddMsgBox(
						STRMSG_C_TRADE_0005, _Q_TRADE_ITEM_NUMBER, 0, g_pGameMain->m_pInven->GetItemSpi());//"ľó¸¶¸¦ żĂ¸®˝Ă°Ú˝Ŕ´Ď±î?"
					g_pInterface->SetWindowOrder(WNDInfWindow);
					return INF_MSGPROC_BREAK;
				}
			}
		}
		else if(pBuilding && IS_STORE_SHOP_TYPE(pBuilding->buildingInfo.BuildingKind) 
			&& (g_pGameMain->m_pInfWindow->IsExistMsgBox(_Q_PUT_ITEM_SPI) == FALSE)
			&& (g_pGameMain->m_pInven->GetItemSpi()>STORE_KEEPING_COST))
		{				
			char buf[256];
			wsprintf( buf, STRMSG_C_STORE_0001, STORE_KEEPING_COST);//"ľó¸¶¸¦ ¸Ă±â˝Ă°Ú˝Ŕ´Ď±î?[°ˇ°Ý : %d ˝şÇÇ]"
			g_pGameMain->m_pInfWindow->AddMsgBox(
				buf, _Q_PUT_ITEM_SPI, 0, g_pGameMain->m_pInven->GetItemSpi()-STORE_KEEPING_COST);
			return INF_MSGPROC_BREAK;
		}
		
	}		

	// ż©±â şÎĹÍ´Â ľĆŔĚĹŰ Ľ±ĹĂ 
	if(!IsInvenRect(pt, &nItemPosX, &nItemPosY))
	{
		// żµżŞżˇ ľř´Ů.
		pParent->ClearMultiSeletItem();	// ´ŮÁßĽ±ĹĂ ĂĘ±âČ­
		return INF_MSGPROC_BREAK;
	}	

	
	
	if(g_pGameMain->m_stSelectItem.pSelectItem)
	{
		// Ľ±ĹĂÇŃ ľĆŔĚĹŰŔĚ ŔÖ´Ů.
		pParent->ClearMultiSeletItem();	// ´ŮÁßĽ±ĹĂ ĂĘ±âČ­
		return INF_MSGPROC_BREAK;
	}

	INVEN_DISPLAY_INFO*  pInvenDisplayInfo = pParent->GetInvenDisplayInfo(nItemPosY*INVEN_X_NUMBER+nItemPosX);
	if(NULL == pInvenDisplayInfo)
	{
		// ¸Ţ¸đ¸® żŔ·ů
		pParent->ClearMultiSeletItem();	// ´ŮÁßĽ±ĹĂ ĂĘ±âČ­
		return INF_MSGPROC_BREAK;
	}
	POINT ptIconPos;
	
	ptIconPos.x = pt.x - (ptBkPos.x + EXTEND_INVEN_ITEM_SLOT_START_X + (EXTEND_INVEN_SLOT_INTERVAL*nItemPosX));
	ptIconPos.y = pt.y - (ptBkPos.y + EXTEND_INVEN_ITEM_SLOT_START_Y + (EXTEND_INVEN_SLOT_INTERVAL*nItemPosY));
	
	// 2007-02-12 by bhsohn Item ´ŮÁß Ľ±ĹĂ Ăł¸®			
	BOOL bMuitiItemSel = FALSE;	
#ifdef _INET_LINK_CHAT
	//if (GetAsyncKeyState(VK_SHIFT) && g_pSOption && g_pSOption->bItemLink) //trade chat
	//{
	//	if (g_pGameMain->m_stSelectItem.pSelectItem == nullptr)
	//	{
	//		
	//		if (COMPARE_RACE(g_pShuttleChild->m_myShuttleInfo.Race, RACE_OPERATION) == TRUE)
	//		{
	//			char szTemp[100];
	//			sprintf(szTemp, "[ID:%I64d]", pInvenDisplayInfo->pItem->UniqueNumber);
	//			
	//			g_pD3dApp->m_pIMSocket->SendChat(T_IC_CHAT_SELL_ALL, g_pD3dApp->m_pShuttleChild->m_myShuttleInfo.CharacterName, szTemp);
	//			return INF_MSGPROC_BREAK;
	//		}
	//		else
	//		{
	//			if (nCountOfSended == 0) //fresh init
	//			{
	//				nCountOfSended++;
	//				serverTime = GetServerDateTime();
	//				char szTemp[100];
	//				sprintf(szTemp, "[ID:%I64d]", pInvenDisplayInfo->pItem->UniqueNumber);
	//				g_pD3dApp->m_pIMSocket->SendChat(T_IC_CHAT_SELL_ALL, g_pD3dApp->m_pShuttleChild->m_myShuttleInfo.CharacterName, szTemp);
	//				return INF_MSGPROC_BREAK;
	//			}
	//			else
	//			{
	//				ATUM_DATE_TIME tempTime = GetServerDateTime();
	//				tempTime.GetCurrentDateTime();

	//				if (tempTime.GetTimeDiffTimeInSeconds(serverTime) >= 120)
	//				{
	//					tempTime.Reset();
	//					serverTime.Reset();
	//					char szTemp[100];
	//					sprintf(szTemp, "[ID:%I64d]", pInvenDisplayInfo->pItem->UniqueNumber);
	//					g_pD3dApp->m_pIMSocket->SendChat(T_IC_CHAT_SELL_ALL, g_pD3dApp->m_pShuttleChild->m_myShuttleInfo.CharacterName, szTemp);
	//					serverTime = GetServerDateTime();
	//					return INF_MSGPROC_BREAK;
	//				}
	//				else
	//				{
	//					char szTemp1[1024];
	//					int secLeft = 120 - tempTime.GetTimeDiffTimeInSeconds(serverTime);
	//					if (secLeft > 0)
	//						sprintf(szTemp1, "\\rYou have to wait \\e%d\\r seconds more to link another item !!!\\r", secLeft);
	//					g_pD3dApp->m_pChat->CreateChatChild(szTemp1, COLOR_ERROR);
	//					return INF_MSGPROC_BREAK;
	//				}
	//			}
	//		}
	//	}
	//	else
	//	{
	//		pParent->SetMultiSelectItem(NULL);
	//	}
	//}
#endif
	if(g_pD3dApp->GetCtrlBtnClick())
	{							
		// ÄÁĆ®·ŃŔ» ´©¸Ł°í Ĺ¬¸ŻÇßłÄ?
		GUI_BUILDINGNPC* pNpc = g_pInterface->m_pCityBase->GetCurrentBuildingNPC();
		if(pNpc)
		{
			if(IS_ITEM_SHOP_TYPE(pNpc->buildingInfo.BuildingKind)
				|| IS_STORE_SHOP_TYPE(pNpc->buildingInfo.BuildingKind)
				|| IS_WARPOINT_SHOP_TYPE(pNpc->buildingInfo.BuildingKind))
			{
				bMuitiItemSel = TRUE;
			}								
		}
#ifdef _INET_INVEN_RIGHT_CLICK_MENU
		else {
			if (pInvenDisplayInfo && pInvenDisplayInfo->pItem) {
				pParent->OnClickItemMenuListWnd(TRUE, pt, pInvenDisplayInfo->pItem->ItemNum, pInvenDisplayInfo->pItem->UniqueNumber);
				pParent->ClearMultiSeletItem();
				return INF_MSGPROC_BREAK;
			}
		}
#endif
	}	
	if(!bMuitiItemSel)
	{
#ifdef _INET_SELECT_ITEM_WITH_SHIFT    
		if (g_pD3dApp->GetShiftBtnClick())
		{
			GUI_BUILDINGNPC* pNpc = g_pInterface->m_pCityBase->GetCurrentBuildingNPC();
			if (pNpc)
			{
				if (IS_ITEM_SHOP_TYPE(pNpc->buildingInfo.BuildingKind)
					|| IS_STORE_SHOP_TYPE(pNpc->buildingInfo.BuildingKind)
					|| IS_WARPOINT_SHOP_TYPE(pNpc->buildingInfo.BuildingKind))
				{
					nClickCount++; // count of clicks on shift
					if (nClickCount == 1)
					{
						nFirstIdx = nItemPosY * INVEN_X_NUMBER + nItemPosX;
						pParent->SetMultiSelectItem(pParent->GetInvenDisplayInfo(nItemPosY * INVEN_X_NUMBER + nItemPosX));
					}
					else
					{
						nLastIdx = nItemPosY * INVEN_X_NUMBER + nItemPosX;
						if (nLastIdx > nFirstIdx)
						{//select forward
							for (int i = nFirstIdx + 1; i <= nLastIdx; i++)
							{
								if (i >= 0)
								{
									pParent->SetMultiSelectItem(pParent->GetInvenDisplayInfo(i));
								}
							}
							nFirstIdx = NULL;
						}
						else
						{					
								for (int i = nLastIdx; i <= nFirstIdx; i++)
								{
									if (i >= 0)
									{
										pParent->SetMultiSelectItem(pParent->GetInvenDisplayInfo(i));
									}
								}
								nFirstIdx = NULL;						
						}
						nClickCount = 0;
					}
					if (nClickCount >= 2)
						nClickCount = 0;

					pParent->SetItemInfo(NULL, 0, 0);
					return INF_MSGPROC_BREAK;
				}
			}
		}
#endif
		int nSelItemCnt = g_pD3dApp->GetMultiSelectItem();				
		BOOL bMultiDragSel = FALSE;
		
		// ÇöŔç ľĆŔĚĹŰŔ» ¶Ç Ľ±ĹĂÇßłÄ?
		if(g_pD3dApp->CheckMultItemSel(pInvenDisplayInfo->pItem->UniqueNumber))
		{
			bMultiDragSel = TRUE;
		}
		if(!bMultiDragSel)
		{
			pParent->SetMultiSelectItem(NULL);	// ´ŮÁß Ăł¸® Á¦°Ĺ
		}							
		
		// ľĆŔĚĹŰ Ľ±ĹĂ
		if(g_pGameMain->m_stSelectItem.pSelectItem == NULL)						
		{
			pParent->SetSelectItem(pInvenDisplayInfo, &ptIconPos);			
			return INF_MSGPROC_BREAK;
		}
	}						
	else
	{
		// ´ŮÁß Ľ±ĹĂ Ăß°ˇ
		if(g_pGameMain->m_stSelectItem.pSelectItem == NULL)						
		{
			pParent->SetMultiSelectItem(pInvenDisplayInfo);
			pParent->SetItemInfo(NULL, 0, 0);
			return INF_MSGPROC_BREAK;
		}
		else
		{
			pParent->SetMultiSelectItem(NULL);	// ´ŮÁß Ăł¸® Á¦°Ĺ
		}
	}												
	// Item ´ŮÁß Ľ±ĹĂ Ăł¸®	
	pParent->ClearMultiSeletItem();	// ´ŮÁßĽ±ĹĂ ĂĘ±âČ­	

	return INF_MSGPROC_BREAK;
}
#ifdef _INET_INVEN_RIGHT_CLICK_MENU
void CINFInvenItem::SetBkPos(POINT ptBkPos)
{
	m_ptBkPos = ptBkPos;
	UpdateBtnPos();
}
#endif
int CINFInvenItem::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL bShowInven)
{	
	CINFInvenExtend* pParent = (CINFInvenExtend*)m_pParent;

	BOOL bSelectItem = FALSE;
	POINT pt;
	pt.x = LOWORD(lParam);
	pt.y = HIWORD(lParam);
	CheckMouseReverse(&pt);
	CINFCityAuction* pCityAuction = NULL;
	if (m_bTradeItemCenterState)
	{
		CMapCityShopIterator it = g_pInterface->m_pCityBase->m_mapCityShop.find(BUILDINGKIND_AUCTION);
		if (it == g_pInterface->m_pCityBase->m_mapCityShop.end() || it->second == NULL)
			return INF_MSGPROC_BREAK;

		pCityAuction = ((CINFCityAuction*)it->second);
	}

	if (m_bTradeItemCenterState && pCityAuction)
		pCityAuction->m_nCurerntInfo = NULL;

	g_pGameMain->SetToolTip(NULL, 0, 0);

	if(!IsWndRect(pt) && !m_bMove)
	{
		if (m_bTradeItemCenterState && pCityAuction)
			pCityAuction->m_nCurerntInfo = NULL;

		pParent->SetItemInfo(NULL, 0, 0);			
		return INF_MSGPROC_NORMAL;
	}	

	m_pEqShow->OnMouseMove(pt);
	m_pCloseBtn->OnMouseMove(pt);
#ifdef _INET_SORT_INV
	m_pEQSort->OnMouseMove(pt);
#endif
	{
		if(m_bMove)
		{
			m_ptBkPos.x = pt.x - m_ptCommOpMouse.x;
			m_ptBkPos.y = pt.y - m_ptCommOpMouse.y;				
			// UIŔŻŔú ÁöÁ¤ 
			UpdateBtnPos();
			return INF_MSGPROC_BREAK;
		}
	}
	

	int nItemPosX, nItemPosY;
	nItemPosX = nItemPosY =-1;

	{
		if(m_pINFInvenScrollBar->GetMouseMoveMode())
		{
			if(FALSE == m_pINFInvenScrollBar->IsMouseScrollPos(pt))
			{
				m_pINFInvenScrollBar->SetMouseMoveMode(FALSE);
			}
			else
			{
				int nOldScroll = m_pINFInvenScrollBar->GetScrollStep();
				m_pINFInvenScrollBar->SetScrollPos(pt);
				int nNewScroll = m_pINFInvenScrollBar->GetScrollStep();
				if(nOldScroll != nNewScroll)
				{
					UpdateInvenScroll();			// żäĂ»
				}
				
				return INF_MSGPROC_BREAK;
			}
		}
	}

	if(!IsInvenRect(pt, &nItemPosX, &nItemPosY))
	{
		// żµżŞżˇ ľř´Ů.		
		pParent->SetWearPosition(POS_INVALID_POSITION);
#ifdef _INET_PET
		g_pShuttleChild->GetPetManager()->SetWearPetSocketPosition(POS_INVALID_POSITION);
#endif
		if(!pParent->m_pSelectItem)
		{				
			if (m_bTradeItemCenterState && pCityAuction)
				pCityAuction->m_nCurerntInfo = NULL;

			pParent->SetItemInfo(NULL, 0, 0);
		}
		if(m_pEqShow->IsMouseOverlab(pt))
		{
			char chTmp[256];
			sprintf( chTmp, STRMSG_C_081014_0204);
			g_pGameMain->SetToolTip( pt.x, pt.y, chTmp);
			return INF_MSGPROC_BREAK;
		}
#ifdef _INET_SORT_INV
		if (m_pEQSort->IsMouseOverlab(pt))
		{
			char chTmp[256];
			sprintf(chTmp, "Sort items in Inventory.");
			g_pGameMain->SetToolTip(pt.x, pt.y, chTmp);
			return INF_MSGPROC_BREAK;
		}
#endif
		return INF_MSGPROC_BREAK;
	}		

	if(m_pEqShow->IsMouseOverlab(pt))
	{
		char chTmp[256];
		sprintf( chTmp, STRMSG_C_081014_0204);
		g_pGameMain->SetToolTip( pt.x, pt.y, chTmp);
		return INF_MSGPROC_BREAK;
	}
#ifdef _INET_SORT_INV
	if (m_pEQSort->IsMouseOverlab(pt))
	{
		char chTmp[256];
		sprintf(chTmp, "Sort items in Inventory.");
		g_pGameMain->SetToolTip(pt.x, pt.y, chTmp);
		return INF_MSGPROC_BREAK;
	}
#endif
	if(g_pGameMain->m_stSelectItem.pSelectItem 
		&& g_pGameMain->m_stSelectItem.bySelectType == ITEM_INVEN_POS)
	{	
		return INF_MSGPROC_NORMAL;
	}	
	

	{
		INVEN_DISPLAY_INFO*  pInvenDisplayInfo = pParent->GetInvenDisplayInfo(nItemPosY*INVEN_X_NUMBER+nItemPosX);
		if(pInvenDisplayInfo)
		{
			char buf[256];
			ITEM *item = NULL;
			// set tooltip
			if(COMPARE_RACE(g_pD3dApp->m_pShuttleChild->m_myShuttleInfo.Race,RACE_OPERATION|RACE_GAMEMASTER))
			{
				if(IS_COUNTABLE_ITEM(pInvenDisplayInfo->pItem->Kind))
				{
					item = g_pDatabase->GetServerItemInfo(pInvenDisplayInfo->pItem->ItemNum);
					if(item)
						wsprintf(buf, STRMSG_C_TOOLTIP_0014,item->ItemName,pInvenDisplayInfo->pItem->ItemNum,(int)((ITEM_GENERAL*)pInvenDisplayInfo->pItem)->UniqueNumber,//"%s(%08d)(%08d)(%d °ł)"
						((ITEM_GENERAL*)pInvenDisplayInfo->pItem)->CurrentCount );
				}
				else
				{
					item = g_pDatabase->GetServerItemInfo(pInvenDisplayInfo->pItem->ItemNum);
					if(item)
						wsprintf(buf, "%s(%8d)(%08d)",item->ItemName,pInvenDisplayInfo->pItem->ItemNum, (int)((ITEM_GENERAL*)pInvenDisplayInfo->pItem)->UniqueNumber);
				}
			}
			else
			{
				if(IS_COUNTABLE_ITEM(pInvenDisplayInfo->pItem->Kind))
				{
					item = g_pDatabase->GetServerItemInfo(pInvenDisplayInfo->pItem->ItemNum);
					if(item)
						wsprintf(buf, STRMSG_C_TOOLTIP_0013,item->ItemName,((ITEM_GENERAL*)pInvenDisplayInfo->pItem)->CurrentCount);//"%s (%d °ł)"
				}
				else
				{
					item = g_pDatabase->GetServerItemInfo(pInvenDisplayInfo->pItem->ItemNum);
					if(item)
						wsprintf(buf, "%s",item->ItemName);
				}
			}
#ifdef _INET_PET
			if (item->ArrDestParameter[0] == DES_PET_SOCKET_ITEM_AUTOKIT || item->ArrDestParameter[0] == DES_PET_SOCKET_ITEM_AUTOSKILL)
			{
				g_pShuttleChild->GetPetManager()->SetWearPetSocketPosition(POS_HIDDEN_ITEM);
			}
			else if (item)
#else
			if( item )
#endif
			{
				pParent->SetWearPosition(item->Position);			
#ifdef _INET_PET
				g_pShuttleChild->GetPetManager()->SetWearPetSocketPosition(item->Position);
#endif
			}
			else
			{
				pParent->SetWearPosition(POS_INVALID_POSITION);			
#ifdef _INET_PET
				g_pShuttleChild->GetPetManager()->SetWearPetSocketPosition(POS_INVALID_POSITION);
#endif
			}	

			if (m_bTradeItemCenterState && pCityAuction){
				CItemInfo* pItemInfo = g_pStoreData->FindItemInInventoryByUniqueNumber(pInvenDisplayInfo->pItem->UniqueNumber);
				if(pItemInfo)
				{
					pCityAuction->m_nCurerntInfo = pItemInfo;
					pCityAuction->ptMouse = pt;
				}
			}
			else
			{
				pParent->SetItemInfo(pInvenDisplayInfo, pt.x, pt.y);
			}

			return INF_MSGPROC_BREAK;
		}
		else
		{
			pParent->SetWearPosition(POS_INVALID_POSITION);			
#ifdef _INET_PET
			g_pShuttleChild->GetPetManager()->SetWearPetSocketPosition(POS_INVALID_POSITION);
#endif
			if (m_bTradeItemCenterState && pCityAuction)
				pCityAuction->m_nCurerntInfo = NULL;

			pParent->SetItemInfo(NULL, 0, 0);			
			// 2009-02-03 by bhsohn ŔĺÂř ľĆŔĚĹŰ şń±ł ĹřĆÁ
			//g_pGameMain->m_pItemInfo->SetItemInfoNormal( NULL, 0 , 0 );
			g_pGameMain->SetItemInfoNormal( NULL, 0 , 0 );
			// end 2009-02-03 by bhsohn ŔĺÂř ľĆŔĚĹŰ şń±ł ĹřĆÁ
		}
	}
	return INF_MSGPROC_BREAK;
}

BOOL CINFInvenItem::IsInvenRect(POINT pt, int *o_pPosX, int *o_pPosY)
{
	POINT ptBkPos = m_ptBkPos;
	(*o_pPosX) = (*o_pPosY) = -1;

	POINT ptBkSize = m_pInvenBase->GetImgSize();
	if((pt.x > (ptBkPos.x+ptBkSize.x))
		|| (pt.x < ptBkPos.x))
	{
		// ĂÖ´ë Ĺ©±â ş¸´Ů Ĺ©¸é ¸®ĹĎÇŃ´Ů.
		return FALSE;
	}
	else if((pt.y > (ptBkPos.y+ptBkSize.y))
		|| (pt.y < (ptBkPos.y+EXTEND_INVEN_CAPS_HEIGHT)))
	{
		// ĂÖ´ë Ĺ©±â ş¸´Ů Ĺ©¸é ¸®ĹĎÇŃ´Ů.
		return FALSE;
	}	


	int nTmpItemPosX = (pt.x-ptBkPos.x-EXTEND_INVEN_ITEM_SLOT_START_X)/EXTEND_INVEN_SLOT_INTERVAL;		
	int nTmpItemPosY = (pt.y - ptBkPos.y - EXTEND_INVEN_ITEM_SLOT_START_Y)/EXTEND_INVEN_SLOT_INTERVAL;

	if( nTmpItemPosX >= 0 && nTmpItemPosX < INVEN_X_NUMBER)
	{
		if(nTmpItemPosY >= 0 && nTmpItemPosY < INVEN_Y_NUMBER)
		{	
			(*o_pPosX) = nTmpItemPosX;
			(*o_pPosY) = nTmpItemPosY;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CINFInvenItem::IsWndRect(POINT ptPos)
{
	POINT ptBakPos = m_ptBkPos;	

	POINT ptSize = m_pInvenBase->GetImgSize();

	if((ptPos.x >= ptBakPos.x && (ptPos.x <= ptBakPos.x+ptSize.x))
		&& (ptPos.y >= ptBakPos.y && (ptPos.y <= ptBakPos.y+ptSize.y)))
	{
		return TRUE;
	}
	return FALSE;
}
int CINFInvenItem::GetScrollStep()
{
	return m_pINFInvenScrollBar->GetScrollStep();
}
void CINFInvenItem::SetMaxScrollStep(int nStep)
{
	m_pINFInvenScrollBar->SetMaxItem(nStep);	

}

void CINFInvenItem::SetScrollEndLine()
{
	int nMaxStep = m_pINFInvenScrollBar->GetMaxStepCnt();
	
	int nScrollStep = nMaxStep - INVEN_Y_NUMBER;
	if(nScrollStep < 0)
	{
		nScrollStep = 0;
	}
	
	// ˝şĹ©·Ń Ŕ§Äˇ´Â °ˇŔĺ ľĆ·ˇ
	m_pINFInvenScrollBar->SetScrollStep(nScrollStep);		
	
	
}
// żäĂ»
void CINFInvenItem::UpdateInvenScroll()			// żäĂ»
{
	CINFInvenExtend* pParent = (CINFInvenExtend*)m_pParent;
	pParent->SetAllIconInfo();
	
}

void CINFInvenItem::UpdateBtnPos()
{
	POINT ptBkPos = m_ptBkPos;
	POINT ptBkSize = m_pInvenBase->GetImgSize();
	
	{
		RECT rcMouseWhell, rcMousePos;
		POINT ptScrollPos = ptBkPos;
		
		ptScrollPos.x += EXTEND_INVEN_SCROLL_LINE_START_X;
		ptScrollPos.y += EXTEND_INVEN_SCROLL_LINE_START_Y;
		
		// ˝şĹ©·Ń x = Ŕ§ÄˇŔÇ -5
		// ˝şĹ©·Ń height = ŔĚąĚÁö ±ćŔĚŔÇ - 34
		m_pINFInvenScrollBar->SetPosition(ptScrollPos.x ,ptScrollPos.y,11,155);
		rcMouseWhell.left		= ptScrollPos.x - ptBkSize.x;
		rcMouseWhell.top		= ptScrollPos.y - 30;
		rcMouseWhell.right		= ptScrollPos.x + 60;
		rcMouseWhell.bottom		= ptScrollPos.y + 185;
		m_pINFInvenScrollBar->SetMouseWhellRect(rcMouseWhell);
		rcMousePos.left			= ptScrollPos.x - 11;
		rcMousePos.top			= ptScrollPos.y ;
		rcMousePos.right		= rcMousePos.left + 32;
		rcMousePos.bottom		= rcMousePos.top + 195;
		m_pINFInvenScrollBar->SetMouseBallRect(rcMousePos);
	}

	{
		int nPosX, nPosY;
		nPosX = ptBkPos.x + 50;
		nPosY = ptBkPos.y + 227;		
		m_pEqShow->SetBtnPosition(nPosX, nPosY);	
		
	}
#ifdef _INET_SORT_INV

	{
		int nPosX, nPosY;
		nPosX = ptBkPos.x + 93;
		nPosY = ptBkPos.y + 231;
		m_pEQSort->SetBtnPosition(nPosX, nPosY);

	}
#endif
	{
		int nPosX, nPosY;
		nPosX = ptBkPos.x + 405;
		nPosY = ptBkPos.y + 5;		
		m_pCloseBtn->SetBtnPosition(nPosX, nPosY);	
	}
	
}

BOOL CINFInvenItem::IsMouseCaps(POINT ptPos)
{
	POINT ptBakPos = m_ptBkPos;	
	POINT ptBkSize = m_pInvenBase->GetImgSize();
	if((ptPos.x >= ptBakPos.x && (ptPos.x <= ptBakPos.x+ptBkSize.x))
		&& (ptPos.y >= ptBakPos.y && (ptPos.y <= ptBakPos.y+EXTEND_INVEN_CAPS_HEIGHT)))
	{
		return TRUE;
	}
	return FALSE;

}

int CINFInvenItem::OnLButtonDB(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL bShowInven)
{
	if(g_pGameMain->m_pTrade->m_bTrading)
	{
		return INF_MSGPROC_NORMAL;
	}
	POINT pt;
	pt.x = LOWORD(lParam);
	pt.y = HIWORD(lParam);
	CheckMouseReverse(&pt);

	int nItemPosX, nItemPosY;
	nItemPosX = nItemPosY= -1;

	if(!IsWndRect(pt) && !m_bMove)
	{
		return INF_MSGPROC_NORMAL;
	}

	if(!IsInvenRect(pt, &nItemPosX, &nItemPosY))
	{
		// żµżŞżˇ ľř´Ů.
		return INF_MSGPROC_NORMAL;
	}	
	CINFInvenExtend* pParent = (CINFInvenExtend*)m_pParent;

	int nItemIdx = nItemPosY*INVEN_X_NUMBER+nItemPosX;

	INVEN_DISPLAY_INFO*  pInvenDisplayInfo = pParent->GetInvenDisplayInfo(nItemIdx);
	UID64_t	 UniqueNumber = pParent->GetDeleteItemUID();
	
	if(0 == UniqueNumber && pInvenDisplayInfo)
	{
		// set tooltip
		ITEM_BASE* pItem = pInvenDisplayInfo->pItem;
		ITEM *item = g_pDatabase->GetServerItemInfo( pItem->ItemNum);
		if(item)
		{
			// 2009-04-02 by bhsohn ·°Ĺ° ¸Ó˝Ĺ Ăß°ˇ ±âČąľČ
			GUI_BUILDINGNPC* pCurrentBuildingNPC = g_pInterface->m_pCityBase->GetCurrentBuildingNPC();
			// end 2009-04-02 by bhsohn ·°Ĺ° ¸Ó˝Ĺ Ăß°ˇ ±âČąľČ

			// 2008-06-03 by dgwoo »óÁˇŔĚ ż­·ÁŔÖ´Â »óĹÂżˇĽ­ ´őşíĹ¬¸ŻŔ¸·Î ľĆŔĚĹŰ ±łĂĽ¸¦ ±ÝÁö.
			//if(g_pInterface->m_pCityBase->GetCurrentBuildingNPC())
			if(pCurrentBuildingNPC)
			{
				// 2009-04-02 by bhsohn ·°Ĺ° ¸Ó˝Ĺ Ăß°ˇ ±âČąľČ
				//g_pD3dApp->m_pChat->CreateChatChild(STRMSG_C_080603_0100,COLOR_ERROR);
				switch(pCurrentBuildingNPC->buildingInfo.BuildingKind)
				{
				case BUILDINGKIND_LUCKY:
					{
						// ·°Ĺ° ¸Ó˝ĹżˇĽ­ŔÇ ´őşí Ĺ¬¸Ż
						if(!OnLButtonDbClick_LuckyMechine(item, (ITEM_GENERAL*)pInvenDisplayInfo->pItem))
						{							
						}
					}
					break;
				// 2010. 04. 21 by jskim ˝Ĺ±Ô ·°Ĺ° ¸Ó˝Ĺ ±¸Çö
				case BUILDINGKIND_LUCKY_OPTION_MACHINE:
					{
						// ·°Ĺ° ¸Ó˝ĹżˇĽ­ŔÇ ´őşí Ĺ¬¸Ż
						if(!OnLButtonDbClick_OptionMechine(item, pInvenDisplayInfo->pItem->UniqueNumber))
						{							
						}
					}
					break;
				//end 2010. 04. 21 by jskim ˝Ĺ±Ô ·°Ĺ° ¸Ó˝Ĺ ±¸Çö
				
				// 2010-08-31 by shcho&&jskim, żëÇŘ ˝Ă˝şĹŰ ±¸Çö
				case BUILDINGKIND_DISSOLUTION:
					{	
						if(!OnLButtonDbClick_Dissolution(item, pInvenDisplayInfo->pItem->UniqueNumber))
						{							
						}
					}
				break;
				// end 2010-08-31 by shcho&&jskim, żëÇŘ ˝Ă˝şĹŰ ±¸Çö
#ifdef _INET_FACTORY8LAB_DB_CLICK
				case BUILDINGKIND_LABORATORY:
				{
					OnLButtonDbClick_Laboratory(item, pInvenDisplayInfo->pItem->UniqueNumber);
				}
				break;
				case BUILDINGKIND_FACTORY:
				{
					OnLButtonDbClick_Factory(item, pInvenDisplayInfo->pItem->UniqueNumber);
				}
				break;
				case BUILDINGKIND_AUCTION:
				{
					OnLButtonDbClick_TradeCenter(item, pInvenDisplayInfo->pItem->UniqueNumber);
				}
				break;
#endif
				default:
					{
						//"\\y»óÁˇżˇĽ­´Â ŔĚżë ÇŇ Ľö ľř˝Ŕ´Ď´Ů."
						g_pD3dApp->m_pChat->CreateChatChild(STRMSG_C_080603_0100, COLOR_ERROR);
					}
					break;
				}
			}
			else
			{
				switch(item->Position)
				{
				case POS_INVALID_POSITION:
					{
						// 2007-10-17 by bhsohn ´ŮÁß Ľ±ĹĂČÄ ľĆŔĚĹŰ »çżëąö±×ĽöÁ¤
						// ´ŮÁß Ľ±ĹĂ Ç×¸ń ĂĘ±âČ­
						g_pD3dApp->DelMultiItemList(TRUE);
						if(g_pGameMain->m_pInfWindow->IsExistMsgBox( _Q_STORE_PUT_COUNTABLE_ITEM ))
						{
							break;
						}
						// end 2007-10-17 by bhsohn ´ŮÁß Ľ±ĹĂČÄ ľĆŔĚĹŰ »çżëąö±×ĽöÁ¤
						
						
												
						// 2008. 12. 16 by ckPark ľĆŔĚĹŰ »çżë Áöż¬
						//pParent->SendUseItem(pItem);

						// ľĆŔĚĹŰ Á¤ş¸ ľňľîżŔ±â
						CItemInfo* pItemInfo = g_pStoreData->FindItemInInventoryByUniqueNumber( pItem->UniqueNumber );
						// Á¤ş¸¸¦ ľňľîżÔŔ» °ćżě
						if(pItemInfo)
						{
							// AttackTimeŔĚ 0ŔĚ»óŔÎ °ÍµéŔş µô·ąŔĚ ¸®˝şĆ®żˇ Ăß°ˇÇŃ´Ů
							if(pItemInfo->GetItemInfo()->AttackTime > 0)
								g_pGameMain->PushDelayItem( pItemInfo );
							else
							// ľĆ´Ń °ÍµéŔş ąŮ·Î ĆĐĹ¶ ŔüĽŰ
								pParent->SendUseItem(pItem);
						}
						// end 2008. 12. 16 by ckPark ľĆŔĚĹŰ »çżë Áöż¬

					}
					break;
				case POS_PROWIN:
				case POS_PROWOUT:
				case POS_WINGIN:
				case POS_WINGOUT:
				case POS_PROW:
				case POS_CENTER:
				case POS_REAR:
				case POS_ACCESSORY_UNLIMITED :
				case POS_ACCESSORY_TIME_LIMIT :	// 2006-03-31 by ispark
				case POS_PET:					// 2010-06-15 by shcho&hslee Ćę˝Ă˝şĹŰ
					{
						pParent->SetSelectItem(pInvenDisplayInfo);
						pParent->SendChangeWearWindowPos(item->Position);
					}
					break;
#ifdef _INET_PET
				case POS_HIDDEN_ITEM:
				{
					char buf[128];
					wsprintf(buf, STRMSG_C_ITEM_0011, pItem->ItemNum);
					g_pD3dApp->m_pChat->CreateChatChild(buf, COLOR_ERROR);

				}
				break;
#endif
				default:
					{
						char buf[128];
						wsprintf(buf,STRMSG_C_ITEM_0011,pItem->ItemNum);//"item [ %08d ] ´Â Âřżë ÇŇ Ľö ľř´Â ľĆŔĚĹŰŔÔ´Ď´Ů."
						g_pD3dApp->m_pChat->CreateChatChild(buf,COLOR_ERROR);
					}
				}
			}
		}		
		pParent->SetSelectItem(NULL);
		pParent->SetItemInfo(NULL, 0, 0);
		g_pGameMain->SetToolTip(0,0,NULL);
		return INF_MSGPROC_BREAK;
	}
	return INF_MSGPROC_NORMAL;

}

POINT CINFInvenItem::GetBkPos()
{
	return m_ptBkPos;
}
///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		// ·°Ĺ° ¸Ó˝ĹżˇĽ­ŔÇ ´őşí Ĺ¬¸Ż
/// \author		// 2009-04-02 by bhsohn ·°Ĺ° ¸Ó˝Ĺ Ăß°ˇ ±âČąľČ
/// \date		2009-04-02 ~ 2009-04-02
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CINFInvenItem::OnLButtonDbClick_LuckyMechine(ITEM *pItem , ITEM_GENERAL* pItemGeneral)
{
	CMapCityShopIterator itLucky = g_pInterface->m_pCityBase->m_mapCityShop.find(BUILDINGKIND_LUCKY);
	if(itLucky == g_pInterface->m_pCityBase->m_mapCityShop.end())
	{
		return FALSE;
	}	
	CINFLuckyMachine* pINFLuckyMachine = ((CINFLuckyMachine*)itLucky->second);

	if(pINFLuckyMachine->GetConinItemInfo())
	{
		// ŔĚąĚ µżŔüŔĚ ŔÖ´Ů.
		return TRUE;
	}

	if(!pINFLuckyMachine->IsPossibleUpLoadCoin(pItem))
	{
		return FALSE;
	}
	// µżŔüŔ» żĂ¸®ŔÚ
	pINFLuckyMachine->UpLoadCoin(pItemGeneral);
	return TRUE;
}
// 2010. 04. 21 by jskim ˝Ĺ±Ô ·°Ĺ° ¸Ó˝Ĺ ±¸Çö
BOOL CINFInvenItem::OnLButtonDbClick_OptionMechine(ITEM *pItem , int UniqueNumber)
{
	CMapCityShopIterator itOptionMechine = g_pInterface->m_pCityBase->m_mapCityShop.find(BUILDINGKIND_LUCKY_OPTION_MACHINE);
 	if(itOptionMechine == g_pInterface->m_pCityBase->m_mapCityShop.end())
 	{
 		return FALSE;
	}	
 	CINFOptionMachine* pINFOptionMechine = ((CINFOptionMachine*)itOptionMechine->second);
 	
	int nPosition = pINFOptionMechine->PossibleUpLoadItemNum(pItem);
	if(pINFOptionMechine->IsPossibleUpLoadItem(pItem, nPosition) == FALSE)
	{
		return FALSE;
	}
	CItemInfo* pItemInfo =g_pStoreData->FindItemInInventoryByUniqueNumber(UniqueNumber);
	pINFOptionMechine->UpLoadItem(pItemInfo, nPosition);
	return TRUE;
}
//end 2010. 04. 21 by jskim ˝Ĺ±Ô ·°Ĺ° ¸Ó˝Ĺ ±¸Çö
// 2010-08-31 by shcho&&jskim, żëÇŘ ˝Ă˝şĹŰ ±¸Çö
BOOL CINFInvenItem::OnLButtonDbClick_Dissolution(ITEM *pItem , int UniqueNumber)
{
	CMapCityShopIterator itDissolution = g_pInterface->m_pCityBase->m_mapCityShop.find(BUILDINGKIND_DISSOLUTION);
	if(itDissolution == g_pInterface->m_pCityBase->m_mapCityShop.end())
	{
		return FALSE;
	}	
	CINFDissolution* pINFitDissolution = ((CINFDissolution*)itDissolution->second);
	if(g_pDatabase->Is_DissolutionitemInfo(pItem->ItemNum) == FALSE)
	{
		g_pD3dApp->m_pChat->CreateChatChild(STRMSG_C_100421_0401,COLOR_ERROR);//"\\yżĂ·Á łőŔ» Ľö ľř˝Ŕ´Ď´Ů.\\y"
		return FALSE;
	}
	CItemInfo* pItemInfo =g_pStoreData->FindItemInInventoryByUniqueNumber(UniqueNumber);
	pINFitDissolution->UpLoadItem(pItemInfo);
	return TRUE;
}
// end 2010-08-31 by shcho&&jskim, żëÇŘ ˝Ă˝şĹŰ ±¸Çö
// 2010. 05. 10 by jskim ˝Ĺ±Ô ·°Ĺ° ¸Ó˝Ĺ Ăß°ˇąćľČ - »óÁˇżˇĽ­ »çżë °ˇ´ÉÇŃ ľĆŔĚĹŰ¸¸ ş¸ż©ÁÖ±â
BOOL CINFInvenItem::ShopIsDisableInvenItem(BYTE BuildingKind, CItemInfo* pItemInfo)
{
	if(BuildingKind == BUILDINGKIND_LUCKY_OPTION_MACHINE)
	{
		if(pItemInfo->ItemInfo->IsExistDesParam(DES_RARE_FIX_PREFIX_INITIALIZE) ||
			pItemInfo->ItemInfo->IsExistDesParam(DES_RARE_FIX_SUFFIX_INITIALIZE) ||
			pItemInfo->ItemInfo->IsExistDesParam(DES_RARE_FIX_PREFIX) ||
			pItemInfo->ItemInfo->IsExistDesParam(DES_RARE_FIX_SUFFIX) ||
			IS_WEAPON(pItemInfo->Kind) ||
			ITEMKIND_DEFENSE == pItemInfo->Kind)
		{
				return FALSE;
		}
	}
	else if(BuildingKind == BUILDINGKIND_LUCKY)
	{
		if(COMPARE_BIT_FLAG(pItemInfo->ItemInfo->ItemAttribute, ITEM_ATTR_LUCKY_ITEM))
		{
 			CINFLuckyMachine* pStore = (CINFLuckyMachine*)g_pInterface->m_pCityBase->FindBuildingShop(BUILDINGKIND_LUCKY);
 			int nMachineNum = pStore->GetLuckyMachineInfo().MachineNum;
 			int nLuckyMachineLen = g_pDatabase->GetLuckyMachineLen();
			
			for(int nCnt = 0; nCnt<nLuckyMachineLen; nCnt++)
			{
				LUCKY_MACHINE_OMI*  pLuckyMachine = g_pDatabase->GetLuckyMachineInfo(nCnt);
				if(NULL == pLuckyMachine)
				{
					break;
				}
				if(nMachineNum == pLuckyMachine->MachineNum)
				{
					if(pItemInfo->ItemInfo->ItemNum == pLuckyMachine->CoinItemNum )
					{
						return FALSE;
					}					
				}
			}
		}
	}
	// 2010-08-31 by shcho&&jskim, żëÇŘ ˝Ă˝şĹŰ ±¸Çö
	else if(BuildingKind == BUILDINGKIND_DISSOLUTION)	
	{
		if(g_pDatabase->Is_DissolutionitemInfo(pItemInfo->ItemNum))
 		{
	 		return FALSE;  // ±×¸˛ŔÚ ľřŔ˝
 		}
	else
		{
			return TRUE;   // ±×¸˛ŔÚ ŔÖŔ˝
		}
	}
	// end 2010-08-31 by shcho&&jskim, żëÇŘ ˝Ă˝şĹŰ ±¸Çö
	else
	{
 		return FALSE;
	}
	return TRUE;
}
//end 2010. 05. 10 by jskim ˝Ĺ±Ô ·°Ĺ° ¸Ó˝Ĺ Ăß°ˇąćľČ - »óÁˇżˇĽ­ »çżë °ˇ´ÉÇŃ ľĆŔĚĹŰ¸¸ ş¸ż©ÁÖ±â
BOOL CINFInvenItem::OnLButtonDbClick_TradeCenter(ITEM* pItem, UID64_t UniqueNumber) {
	CMapCityShopIterator itTrade = g_pInterface->m_pCityBase->m_mapCityShop.find(BUILDINGKIND_AUCTION);
	if (itTrade == g_pInterface->m_pCityBase->m_mapCityShop.end())
	{
		return FALSE;
	}
	CINFCityAuction* pINFLab = ((CINFCityAuction*)itTrade->second);
	if (pINFLab->m_nCurrentTab == TAB_SELL_ITEM) {
		CItemInfo* pItemInfo = g_pStoreData->FindItemInInventoryByUniqueNumber(UniqueNumber);
		if (pItemInfo)
			pINFLab->UpLoadItem(pItemInfo);
	}
	return TRUE;
}
BOOL CINFInvenItem::OnLButtonDbClick_Factory(ITEM *pItem, UID64_t UniqueNumber)
{
	CMapCityShopIterator itFactory = g_pInterface->m_pCityBase->m_mapCityShop.find(BUILDINGKIND_FACTORY);
	if (itFactory == g_pInterface->m_pCityBase->m_mapCityShop.end())
	{
		return FALSE;
	}
	CINFCityLab* pINFLab = ((CINFCityLab*)itFactory->second);

	// 	if(g_pDatabase->Is_DissolutionitemInfo(pItem->ItemNum) == FALSE)
	// 	{
	// 		g_pD3dApp->m_pChat->CreateChatChild(STRMSG_C_100421_0401,COLOR_ERROR);//"\\yżĂ·Á łőŔ» Ľö ľř˝Ŕ´Ď´Ů.\\y"
	// 		return FALSE;
	// 	}
	CItemInfo* pItemInfo = g_pStoreData->FindItemInInventoryByUniqueNumber(UniqueNumber);
	pINFLab->UpLoadItem(pItemInfo);
	return TRUE;
}

BOOL CINFInvenItem::OnLButtonDbClick_Laboratory(ITEM *pItem, UID64_t UniqueNumber)
{
	CMapCityShopIterator itLaboratory = g_pInterface->m_pCityBase->m_mapCityShop.find(BUILDINGKIND_LABORATORY);
	if (itLaboratory == g_pInterface->m_pCityBase->m_mapCityShop.end())
	{
		return FALSE;
	}
	CINFCityLab* pINFLab = ((CINFCityLab*)itLaboratory->second);

	// 	if(g_pDatabase->Is_DissolutionitemInfo(pItem->ItemNum) == FALSE)
	// 	{
	// 		g_pD3dApp->m_pChat->CreateChatChild(STRMSG_C_100421_0401,COLOR_ERROR);//"\\yżĂ·Á łőŔ» Ľö ľř˝Ŕ´Ď´Ů.\\y"
	// 		return FALSE;
	// 	}
	CItemInfo* pItemInfo = g_pStoreData->FindItemInInventoryByUniqueNumber(UniqueNumber);
	pINFLab->UpLoadItem(pItemInfo);
	return TRUE;
}