// INFItemInfo.cpp: implementation of the CINFItemInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GameDataLast.h"
#include "INFItemInfo.h"
#include "AtumApplication.h"
#include "ShuttleChild.h"
#include "CharacterChild.h"				// 2005-07-21 by ispark
#include "AtumDatabase.h"
#include "D3DHanFont.h"
#include "INFGameMain.h"
#include "INFWindow.h"
#include "ItemInfo.h"
#include "INFImage.h"
#include "GameDataLast.h"
#include "StoreData.h"
#include "dxutil.h"
#include "SkinnedMesh.h"
// 2009. 11. 02 by ckPark ¿Œ««¥œ∆º « µÂ ¿ŒΩ∫≈œΩ∫ ¥¯¡Ø Ω√Ω∫≈€
#include "Interface.h"
#include "INFIcon.h"
#include "INFCityBase.h"
#include "INFCityInfinityShop.h"
#include "INFInven.h"
#include "SceneData.h"
#include "ObjRender.h"
#include "INFCityShop.h"
#include "INFCityBase.h"

// end 2009. 11. 02 by ckPark ¿Œ««¥œ∆º « µÂ ¿ŒΩ∫≈œΩ∫ ¥¯¡Ø Ω√Ω∫≈€
#ifdef _INET_PET
	#include "PetManager.h"		  //2011-10-06 by jhahn ∆ƒ∆Æ≥  º∫¿Â«¸ Ω√Ω∫≈€
#endif
#ifdef _INET_RANKBUFF
	#include "INFUnitNameInfo.h"
#endif
// 2008-04-14 by bhsohn ¿Ø∑¥ æ∆¿Ã≈€ º≥∏Ì Ω∫∆Æ∏µπÆ¡¶ √≥∏Æ
#if defined(LANGUAGE_ENGLISH) || defined(LANGUAGE_VIETNAM)|| defined(LANGUAGE_THAI)// 2008-04-30 by bhsohn ≈¬±π πˆ¿¸ √ﬂ∞°
#define STRING_CULL ::StringCullingUserData_ToBlank
#else
#define STRING_CULL ::StringCullingUserDataEx
#endif
// end 2008-04-14 by bhsohn ¿Ø∑¥ æ∆¿Ã≈€ º≥∏Ì Ω∫∆Æ∏µπÆ¡¶ √≥∏Æ

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CINFItemInfo::CINFItemInfo(CAtumNode* pParent)
{
	FLOG( "CINFItemInfo(CAtumNode* pParent)" );
	m_pParent = pParent;
	m_bShow = FALSE;
	memset( m_strItemInfo, 0x00, ITEMINFO_PARAMETER_NUMBER*ITEMINFO_ITEM_FULL_NAME);
	memset(m_strDesc, 0x00, ITEMINFO_DESC_SIZE*ITEMINFO_DESC_LINE_NUMBER);
	memset( m_pFontItemInfo, 0x00, ITEMINFO_PARAMETER_NUMBER*sizeof(DWORD));
	memset( m_pFontDescInfo, 0x00, ITEMINFO_DESC_LINE_NUMBER*sizeof(DWORD));

	memset(m_strExtendItemInfo, 0x00, ITEMINFO_DESC_SIZE * ITEMINFO_DESC_LINE_NUMBER);
	memset(m_pFontExtendItemInfo, 0x00, ITEMINFO_DESC_LINE_NUMBER * sizeof(DWORD));

	m_nExtendItemIndex = 0;
	m_pFontItemName	= NULL;
	m_pInchantNum = NULL;

	m_nDescIndex = 0;
	m_nMaxLength = 0;
	m_nDescLine = 0;
	D3DXMatrixIdentity(&m_pMatInven);
	m_ptItemInfo.x = 0;
	m_ptItemInfo.y = 0;
	m_bRestored = FALSE;
	m_bEnableItem = FALSE;

	m_pRefItemInfo = NULL;
	m_pRefPrefixRareInfo = NULL;
	m_pRefSuffixRareInfo = NULL;
#ifdef BONUS_STAT_ITEM
	m_pRefBonus = NULL;
	m_bFullStat = FALSE;
#endif
	m_pRefEnchant = NULL;
	m_pRefITEM = NULL;
	m_pDataHeader = NULL;
	m_nBigIconNum = 0;
	m_pGameData = NULL;

	memset( m_pInfoBoxSide, 0x00, sizeof(m_pInfoBoxSide[0])*9);
	memset( m_strItemName, 0x00, ITEMINFO_ITEM_FULL_NAME);

	m_nOtherItemCount = 0;
	m_vecTickFuntionIndex.clear();

	// 2009-02-03 by bhsohn ¿Â¬¯ æ∆¿Ã≈€ ∫Ò±≥ ≈¯∆¡
	m_bMyEquipItem = FALSE;
	m_pFontMyEquipItem = NULL;
	m_szTooltip.cx = 0;
	m_szTooltip.cy = 0;
	// end 2009-02-03 by bhsohn ¿Â¬¯ æ∆¿Ã≈€ ∫Ò±≥ ≈¯∆¡

	m_pStateBlock = NULL;
	m_pMirrorTexture = NULL;
	m_fRotationZ = SHUTTLE_ROTATION_DEFAULT_Z;
}

CINFItemInfo::~CINFItemInfo()
{
	FLOG( "~CINFItemInfo()" );

	int i;
	for(i=0;i<ITEMINFO_PARAMETER_NUMBER;i++)
	{
		SAFE_DELETE(m_pFontItemInfo[i]);
	}
	for(i=0;i<ITEMINFO_DESC_LINE_NUMBER;i++)
	{
		SAFE_DELETE(m_pFontDescInfo[i]);
	}
	SAFE_DELETE(m_pFontItemName);
	// 2009-02-03 by bhsohn ¿Â¬¯ æ∆¿Ã≈€ ∫Ò±≥ ≈¯∆¡
	SAFE_DELETE(m_pFontMyEquipItem);
	// end 2009-02-03 by bhsohn ¿Â¬¯ æ∆¿Ã≈€ ∫Ò±≥ ≈¯∆¡
	SAFE_DELETE(m_pInchantNum);
	SAFE_DELETE(m_pDataHeader);
	//SAFE_DELETE(m_pBigIcon);
	
	for(i=0;i<9;i++)
	{
		SAFE_DELETE(m_pInfoBoxSide[i]);
	}
	for (i = 0; i < ITEMINFO_DESC_LINE_NUMBER; i++) {
		SAFE_DELETE(m_pFontExtendItemInfo[i]);
	}

	SAFE_RELEASE(m_pMirrorTexture);
}

void CINFItemInfo::InitItemInfo()
{
	FLOG( "CINFItemInfo::InitItemInfo()" );
	m_bShow = FALSE;
	memset( m_strItemInfo,0x00, ITEMINFO_PARAMETER_NUMBER*ITEMINFO_ITEM_FULL_NAME);
	memset( m_strDesc,0x00, ITEMINFO_DESC_LINE_NUMBER*ITEMINFO_DESC_SIZE);
	memset(m_strExtendItemInfo, 0x00, ITEMINFO_DESC_LINE_NUMBER * ITEMINFO_DESC_SIZE);

	m_nExtendItemIndex = 0;
	m_nDescIndex = 0;
	m_nMaxLength = 0;
	m_nDescLine = 0;
	m_bEnableItem = FALSE;
	m_pRefItemInfo = NULL;
	m_pRefPrefixRareInfo = NULL;
	m_pRefSuffixRareInfo = NULL;
#ifdef BONUS_STAT_ITEM
	m_pRefBonus = NULL;
#endif
	m_pRefEnchant = NULL;
	m_pRefITEM = NULL;
	m_vecTickFuntionIndex.clear();
}

HRESULT CINFItemInfo::InitDeviceObjects()
{
	FLOG( "CINFItemInfo::InitDeviceObjects()" );
	DataHeader	* pDataHeader ;
	char buf[16];

	int i;
	for(i=0;i<ITEMINFO_PARAMETER_NUMBER;i++)
	{
		m_pFontItemInfo[i] = new CD3DHanFont(_T(g_pD3dApp->GetFontStyle()),9, D3DFONT_ZENABLE,  FALSE,512,32);
		m_pFontItemInfo[i]->InitDeviceObjects(g_pD3dDev);
	}
	for(i=0;i<ITEMINFO_DESC_LINE_NUMBER;i++)
	{
		m_pFontDescInfo[i] = new CD3DHanFont(_T(g_pD3dApp->GetFontStyle()),9, D3DFONT_ZENABLE,  FALSE,512,32);
		m_pFontDescInfo[i]->InitDeviceObjects(g_pD3dDev);
	}
	for (i = 0; i < ITEMINFO_DESC_LINE_NUMBER; i++) {
		m_pFontExtendItemInfo[i] = new CD3DHanFont(_T(g_pD3dApp->GetFontStyle()), 9, D3DFONT_ZENABLE, FALSE, 512, 32);
		m_pFontExtendItemInfo[i]->InitDeviceObjects(g_pD3dDev);
	}
	m_pFontItemName = new CD3DHanFont(_T(g_pD3dApp->GetFontStyle()),9, D3DFONT_ZENABLE|D3DFONT_BOLD,  TRUE,512,32);
	m_pFontItemName->InitDeviceObjects(g_pD3dDev);

	// 2009-02-03 by bhsohn ¿Â¬¯ æ∆¿Ã≈€ ∫Ò±≥ ≈¯∆¡
	if(NULL == m_pFontMyEquipItem)
	{
		m_pFontMyEquipItem = new CD3DHanFont(_T(g_pD3dApp->GetFontStyle()),9, D3DFONT_ZENABLE|D3DFONT_BOLD,  TRUE,512,32);
		m_pFontMyEquipItem->InitDeviceObjects(g_pD3dDev);	
	}
	// end 2009-02-03 by bhsohn ¿Â¬¯ æ∆¿Ã≈€ ∫Ò±≥ ≈¯∆¡
	
	m_pInchantNum = new CD3DHanFont(_T(g_pD3dApp->GetFontStyle()),8, D3DFONT_ZENABLE|D3DFONT_BOLD,  FALSE,512,32);
	m_pInchantNum->InitDeviceObjects(g_pD3dDev);

	for(i=0;i<3;i++)
	{
		for(int j=0;j<3;j++)
		{
			m_pInfoBoxSide[i*3+j] = new CINFImage;
			wsprintf(buf, "w_whi%d%d", i,j);
			pDataHeader = FindResource(buf);
			m_pInfoBoxSide[i*3+j]->InitDeviceObjects(pDataHeader->m_pData,pDataHeader->m_DataSize) ;		
		}
	}

	if (FAILED(g_pD3dDev->CreateTexture(
		g_pD3dApp->GetBackBufferDesc().Width,
		g_pD3dApp->GetBackBufferDesc().Height,
		1, D3DUSAGE_RENDERTARGET,
		D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT,
		&m_pMirrorTexture, NULL)))
	{
		SAFE_RELEASE(m_pMirrorTexture);
		return E_FAIL;
	}

	g_pD3dDev->CreateStateBlock(D3DSBT_ALL, &m_pStateBlock);

	return S_OK;
}

HRESULT CINFItemInfo::RestoreDeviceObjects()
{
	FLOG( "CINFItemInfo::RestoreDeviceObjects()" );
	int i;
	for(i=0;i<ITEMINFO_PARAMETER_NUMBER;i++)
	{
		m_pFontItemInfo[i]->RestoreDeviceObjects();
	}
	for(i=0;i<ITEMINFO_DESC_LINE_NUMBER;i++)
	{
		m_pFontDescInfo[i]->RestoreDeviceObjects();
	}
	m_pFontItemName->RestoreDeviceObjects();
	// 2009-02-03 by bhsohn ¿Â¬¯ æ∆¿Ã≈€ ∫Ò±≥ ≈¯∆¡
	if(m_pFontMyEquipItem)
	{		
		m_pFontMyEquipItem->RestoreDeviceObjects();	
	}
	// end 2009-02-03 by bhsohn ¿Â¬¯ æ∆¿Ã≈€ ∫Ò±≥ ≈¯∆¡

	m_pInchantNum->RestoreDeviceObjects();
	//if(m_pBigIcon) m_pBigIcon->RestoreDeviceObjects();

	for(i=0;i<9;i++)
	{
		m_pInfoBoxSide[i]->RestoreDeviceObjects();
	}
	for (i = 0; i < ITEMINFO_DESC_LINE_NUMBER; i++) {
		m_pFontExtendItemInfo[i]->RestoreDeviceObjects();
	}
	m_bRestored = TRUE;
	return S_OK;
}

HRESULT CINFItemInfo::InvalidateDeviceObjects()
{
	FLOG( "CINFItemInfo::InvalidateDeviceObjects()" );
	int i;
	for(i=0;i<ITEMINFO_PARAMETER_NUMBER;i++)
	{
		if(m_pFontItemInfo[i])
			m_pFontItemInfo[i]->InvalidateDeviceObjects();
	}
	for(i=0;i<ITEMINFO_DESC_LINE_NUMBER;i++)
	{
		if(m_pFontDescInfo[i])
			m_pFontDescInfo[i]->InvalidateDeviceObjects();
	}
	m_pFontItemName->InvalidateDeviceObjects();
	// 2009-02-03 by bhsohn ¿Â¬¯ æ∆¿Ã≈€ ∫Ò±≥ ≈¯∆¡
	if(m_pFontMyEquipItem)
	{		
		m_pFontMyEquipItem->InvalidateDeviceObjects();	
	}
	// end 2009-02-03 by bhsohn ¿Â¬¯ æ∆¿Ã≈€ ∫Ò±≥ ≈¯∆¡
	m_pInchantNum->InvalidateDeviceObjects();
	//if(m_pBigIcon) m_pBigIcon->InvalidateDeviceObjects();
	
	for(i=0;i<9;i++)
	{
		if(m_pInfoBoxSide[i])
			m_pInfoBoxSide[i]->InvalidateDeviceObjects();
	}
	for (i = 0; i < ITEMINFO_DESC_LINE_NUMBER; i++) {
		m_pFontExtendItemInfo[i]->InvalidateDeviceObjects();
	}
	m_bRestored = FALSE;
	
	return S_OK;

}

HRESULT CINFItemInfo::DeleteDeviceObjects()
{
	FLOG( "CINFItemInfo::DeleteDeviceObjects()" );
	int i;
	for(i=0;i<ITEMINFO_PARAMETER_NUMBER;i++)
	{
		m_pFontItemInfo[i]->DeleteDeviceObjects();
		SAFE_DELETE(m_pFontItemInfo[i]);
	}
	for(i=0;i<ITEMINFO_DESC_LINE_NUMBER;i++)
	{
		m_pFontDescInfo[i]->DeleteDeviceObjects();
		SAFE_DELETE(m_pFontDescInfo[i]);
	}

	m_pFontItemName->DeleteDeviceObjects();
	SAFE_DELETE(m_pFontItemName);
	// 2009-02-03 by bhsohn ¿Â¬¯ æ∆¿Ã≈€ ∫Ò±≥ ≈¯∆¡
	if(m_pFontMyEquipItem)
	{		
		m_pFontMyEquipItem->DeleteDeviceObjects();
		SAFE_DELETE(m_pFontMyEquipItem);
	}
	// end 2009-02-03 by bhsohn ¿Â¬¯ æ∆¿Ã≈€ ∫Ò±≥ ≈¯∆¡

	m_pInchantNum->DeleteDeviceObjects();
	SAFE_DELETE(m_pInchantNum);

	/*if (m_pBigIcon)
	{
		m_pBigIcon->DeleteDeviceObjects();
		SAFE_DELETE(m_pBigIcon);
	}*/
	
	for(i=0;i<9;i++)
	{
		m_pInfoBoxSide[i]->DeleteDeviceObjects();
		SAFE_DELETE(m_pInfoBoxSide[i]);
	}
	for (i = 0; i < ITEMINFO_DESC_LINE_NUMBER; i++) {
		m_pFontExtendItemInfo[i]->DeleteDeviceObjects();
		SAFE_DELETE(m_pFontExtendItemInfo[i]);
	}
	//SAFE_DELETE(m_pGameData);

	{
		SAFE_RELEASE(m_pStateBlock);
		SAFE_RELEASE(m_pMirrorTexture);
	}

	return S_OK;

}

void CINFItemInfo::Render()
{
	FLOG( "CINFItemInfo::Render()" );

	// 2006-10-12 by ispark, ∫∞µµ «‘ºˆ Tick¿ª √≥∏Æ
	SetOtherFuntionTick();
	
	int i;

	if(m_bShow && m_nDescIndex>0 && m_bRestored)
	{
		m_pStateBlock->Capture();

		LPDIRECT3DSURFACE9 pSurfaceMirror;
		LPDIRECT3DSURFACE9 pSurfaceOrig;

		g_pD3dDev->GetRenderTarget(0, &pSurfaceOrig);
		m_pMirrorTexture->GetSurfaceLevel(0, &pSurfaceMirror);

		g_pD3dDev->SetRenderTarget(0, pSurfaceMirror);
		g_pD3dDev->Clear(0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0L);
		g_pD3dDev->SetRenderState(D3DRS_ZENABLE, TRUE);
		g_pD3dDev->SetRenderState(D3DRS_LIGHTING, TRUE);

		int icongab;
		if (m_nBigIconNum > 0)
		{
			icongab = ITEMINFO_BIGICON_GAB;
			//m_ptItemInfo.y = m_ptItemInfo.y + ITEMINFO_BIGICON_GAB;
		}
		else icongab = 0;

// 2006-03-08 by ispark 6*(m_nMaxLength+2) --> (m_nMaxLength+12)
		if( m_ptItemInfo.x + (m_nMaxLength+12) > g_pD3dApp->GetBackBufferDesc().Width )
		{
			m_ptItemInfo.x = g_pD3dApp->GetBackBufferDesc().Width - (m_nMaxLength+12);
		}
		/*if( m_ptItemInfo.y + 14*(m_nDescIndex+1)+14*(m_nDescLine+1)+20 > g_pD3dApp->GetBackBufferDesc().Height )
		{
			m_ptItemInfo.y = (g_pD3dApp->GetBackBufferDesc().Height - ((14*(m_nDescIndex+1)+14*(m_nDescLine+1)+20)));
		}*/
		if (m_ptItemInfo.y + 14 * (m_nDescIndex + 1 + m_nDescLine + m_nExtendItemIndex) + 20 + icongab > g_pD3dApp->GetBackBufferDesc().Height) {
			m_ptItemInfo.y = g_pD3dApp->GetBackBufferDesc().Height - (14 * (m_nDescIndex + 1 + m_nDescLine + m_nExtendItemIndex) + 20 + icongab);
		}
		/////////////////////////////////////////////////////////////////////////////
		// ∑ª¥ı∏µ				

		// 2009-02-03 by bhsohn ¿Â¬¯ æ∆¿Ã≈€ ∫Ò±≥ ≈¯∆¡
		//RenderInfoWindows(m_ptItemInfo.x,m_ptItemInfo.y-icongab,m_nMaxLength+12,14*(m_nDescIndex+1)+14*(m_nDescLine+1)+20+icongab);
		
		// end 2009-02-03 by bhsohn ¿Â¬¯ æ∆¿Ã≈€ ∫Ò±≥ ≈¯∆¡

		int temp;
		
		if (m_strItemInfo[0][0])
		{
			if (m_pRefItemInfo && m_pRefItemInfo != nullptr) {
				if ((m_pRefItemInfo->Kind == ITEMKIND_DEFENSE) || (m_pRefItemInfo->Kind == ITEMKIND_INGOT))
					m_szTooltip.cy = m_szTooltip.cy < 400 ? 400 : m_szTooltip.cy;

				if (m_pRefItemInfo->Kind == ITEMKIND_COLOR_ITEM)
				{
					m_szTooltip.cx = m_szTooltip.cx < 300 ? 300 : m_szTooltip.cx;
					m_szTooltip.cy = m_szTooltip.cy < 250 ? 250 : m_szTooltip.cy;
				}
			}
			else if (m_pRefITEM && m_pRefITEM != nullptr) {
				if ((m_pRefITEM->Kind == ITEMKIND_DEFENSE) || m_pRefITEM->LinkItem > 0)
					m_szTooltip.cy = m_szTooltip.cy < 400 ? 400 : m_szTooltip.cy;

				if (m_pRefITEM->Kind == ITEMKIND_COLOR_ITEM)
				{
					m_szTooltip.cx = m_szTooltip.cx < 300 ? 300 : m_szTooltip.cx;
					m_szTooltip.cy = m_szTooltip.cy < 250 ? 250 : m_szTooltip.cy;
				}
			}
			
			RenderInfoWindows(m_ptItemInfo.x, m_ptItemInfo.y - icongab,
				m_szTooltip.cx,
				m_szTooltip.cy);

			char buff[128];
			memset(buff, 0x00, sizeof(buff[0]*128));
			strcpy(buff, m_strItemInfo[0]);
			// 2006-03-08 by ispark, ∞ËªÍ πŸ≤ﬁ
//			temp = ((strlen(m_strItemName)/2)*6.5)+7;
			temp = m_pFontItemName->GetStringSize(m_strItemName).cx / 2;
			if (m_nBigIconNum > 0)
			{
				m_pFontItemName->DrawText(m_ptItemInfo.x+((m_nMaxLength+12)/2)-temp,
					(m_ptItemInfo.y-icongab)+ITEMINFO_NAME_IMAGE_GAB , 
					GUI_FONT_COLOR,
					buff, 0L);
			}
			// 2008-09-26 by bhsohn Ω≈±‘ ¿Œ√æ∆Æ √≥∏Æ
			else if(m_pRefItemInfo && m_pRefItemInfo->GetEnchantNumber() > 0)
			{
				m_pFontItemName->DrawText(m_ptItemInfo.x+((m_nMaxLength+12)/2)-temp, 
					(m_ptItemInfo.y-icongab)+ITEMINFO_NAME_IMAGE_GAB-ITEMINFO_BIGICON_GAB+8, 
					GUI_FONT_COLOR,
					buff, 0L);
			}
			// end 2008-09-26 by bhsohn Ω≈±‘ ¿Œ√æ∆Æ √≥∏Æ
			else 
			{
				m_pFontItemName->DrawText(m_ptItemInfo.x+((m_nMaxLength+12)/2)-temp, 
					(m_ptItemInfo.y-icongab)+ITEMINFO_NAME_IMAGE_GAB-ITEMINFO_BIGICON_GAB, 
					GUI_FONT_COLOR,
					buff, 0L);
			}
		}
		
		if (m_nBigIconNum > 0)
		{
			
			temp = ITEMINFO_BIGICON_WIDTH;
			temp = ((m_nMaxLength+12)/2) - temp/2;

			g_pGameMain->m_pBigIcon->Render(m_nBigIconNum,
				m_ptItemInfo.x + temp,
				(m_ptItemInfo.y - icongab) + ITEMINFO_TOP_GAB);
			
			if(m_pRefItemInfo && m_pRefItemInfo->GetEnchantNumber())
			{
				char buf[62];
				// 2007-03-29 by bhsohn China String
				//sprintf(buf, "\\eEnchant:%d\\e", m_pRefItemInfo->GetEnchantNumber());
				sprintf(buf, STRMSG_C_070329_0100, m_pRefItemInfo->GetEnchantNumber());
				// end 2007-03-29 by bhsohn China String
				m_pInchantNum->DrawText(m_ptItemInfo.x+temp+INCHANTNUM_X,(m_ptItemInfo.y-icongab)+ITEMINFO_TOP_GAB+42,GUI_FONT_COLOR,buf,0L);
			}
		}
		// 2008-09-26 by bhsohn Ω≈±‘ ¿Œ√æ∆Æ √≥∏Æ
		else if(m_pRefItemInfo && m_pRefItemInfo->GetEnchantNumber() > 0)
		{
			temp = ITEMINFO_BIGICON_WIDTH;
			temp = ((m_nMaxLength+12)/2) - temp/2;
			
			char buf[62];
			sprintf(buf, STRMSG_C_070329_0100, m_pRefItemInfo->GetEnchantNumber());
			m_pInchantNum->DrawText(m_ptItemInfo.x+temp+INCHANTNUM_X,
							(m_ptItemInfo.y-icongab)+ITEMINFO_NAME_IMAGE_GAB-ITEMINFO_BIGICON_GAB-5,
							GUI_FONT_COLOR,buf,0L);
		}
		// end 2008-09-26 by bhsohn Ω≈±‘ ¿Œ√æ∆Æ √≥∏Æ

		// 2009-02-03 by bhsohn ¿Â¬¯ æ∆¿Ã≈€ ∫Ò±≥ ≈¯∆¡
		int nItemPos = 0;
		for(i=1;i<m_nDescIndex;i++)
		{
			if(m_strItemInfo[i][0]) 
			{
				m_pFontItemInfo[i]->DrawText(m_ptItemInfo.x+5, m_ptItemInfo.y+20+14*i, GUI_FONT_COLOR,m_strItemInfo[i], 0L); 				
				nItemPos++;
			}
		}
		for(i=0;i<ITEMINFO_DESC_LINE_NUMBER;i++)
		{
			if(m_strDesc[i][0])
			{
				m_pFontDescInfo[i]->DrawText(m_ptItemInfo.x+5, m_ptItemInfo.y+20+14*(i+m_nDescIndex), GUI_FONT_COLOR,m_strDesc[i], 0L);
				nItemPos++;
			}
		}
#ifdef _INET_INVEN_RIGHT_CLICK_MENU
		for (i = 0; i < ITEMINFO_DESC_LINE_NUMBER; i++) {
			if (m_strExtendItemInfo[i][0]) {
				m_pFontExtendItemInfo[i]->DrawText(m_ptItemInfo.x + 5, m_ptItemInfo.y + 34 + 14 * (i + m_nDescIndex + m_nDescLine), GUI_FONT_COLOR, m_strExtendItemInfo[i], 0L);
				nItemPos++;
			}
		}
#endif
		if(m_bMyEquipItem)
		{
			nItemPos++;
			char chTmpBuff[256];
			ZERO_MEMORY(chTmpBuff);
			wsprintf(chTmpBuff, STRMSG_C_090203_0202);
			m_pFontMyEquipItem->DrawText(m_ptItemInfo.x+5, m_ptItemInfo.y+20+14*(nItemPos), GUI_FONT_COLOR,chTmpBuff, 0L);
		}
		// end 2009-02-03 by bhsohn ¿Â¬¯ æ∆¿Ã≈€ ∫Ò±≥ ≈¯∆¡s
#ifdef BONUS_STAT_ITEM
		if (m_bFullStat)
		{
			nItemPos++;
			char chTmpBuff[256];
			ZERO_MEMORY(chTmpBuff);
			sprintf(chTmpBuff, "\\g*Perfect Stat Item*\\g");
			m_pFontMyEquipItem->DrawText(m_ptItemInfo.x + 5, m_ptItemInfo.y + 20 + 14 * (nItemPos), GUI_FONT_COLOR, chTmpBuff, 0L);
		}
		//30-03-2017 by Ineptub - add info about numpad 0 
		if (!m_bMyEquipItem && !m_bFullStat && m_pRefItemInfo)
		{
			if (IS_PRIMARY_WEAPON(m_pRefItemInfo->Kind) || IS_SECONDARY_WEAPON(m_pRefItemInfo->Kind))
			{
				nItemPos++;
				char chTmpBuff[256];
				ZERO_MEMORY(chTmpBuff);
				sprintf(chTmpBuff, "\\yPress NUMPAD 0 while previewing Item to see perfect stats!\\y");
				m_pFontMyEquipItem->DrawText(m_ptItemInfo.x + 5, m_ptItemInfo.y + 20 + 14 * (nItemPos), GUI_FONT_COLOR, chTmpBuff, 0L);
			}
		}
#endif
		// 2009. 11. 02 by ckPark ¿Œ««¥œ∆º « µÂ ¿ŒΩ∫≈œΩ∫ ¥¯¡Ø Ω√Ω∫≈€
		int nX = m_ptItemInfo.x+5;
		int nY = m_ptItemInfo.y+20+14*(nItemPos+1) + 7;
		char szTemp[ 16 ];

		// ¿Œ««¥œ∆º ±≥»Ø æ∆¿Ã≈€
		if( !m_vecExchageMtrl.empty() )
		{
			for( std::vector< std::pair<ItemNum_t,InfinityShopItemCnt_t> >::iterator it = m_vecExchageMtrl.begin();
				 it != m_vecExchageMtrl.end();
				 ++it )
			{
				// æ∆¿Ãƒ‹
				g_pGameMain->m_pIcon->Render((*it).first, nX, nY);

				// ºˆ∑Æ
				sprintf( szTemp, "%d", (*it).second );
				m_pFontMyEquipItem->DrawText( nX + 14 - m_pFontMyEquipItem->GetStringSize( szTemp ).cx / 2, nY + 30, GUI_FONT_COLOR, szTemp );

				nX += 46;
			}
		}
		// end 2009. 11. 02 by ckPark ¿Œ««¥œ∆º « µÂ ¿ŒΩ∫≈œΩ∫ ¥¯¡Ø Ω√Ω∫≈€
		bool bCardContour = false;
		short nRefItemInfo = -1;
		ITEM* pCountourCardItem = NULL;

		if (m_pRefItemInfo != nullptr && m_pRefItemInfo->GetRealItemInfo() != nullptr) {
			pCountourCardItem = g_pDatabase->GetServerItemInfo(m_pRefItemInfo->GetRealItemInfo()->LinkItem);
			nRefItemInfo = 0;
		}
		else if (m_pRefITEM != nullptr) {
			pCountourCardItem = g_pDatabase->GetServerItemInfo(m_pRefITEM->LinkItem);
			nRefItemInfo = 1;
		}
		
		if (pCountourCardItem && pCountourCardItem->Kind == ITEMKIND_DEFENSE)
			bCardContour = true;
		
		if ((nRefItemInfo == 0 && m_pRefItemInfo != nullptr && (m_pRefItemInfo->Kind == ITEMKIND_DEFENSE || m_pRefItemInfo->Kind == ITEMKIND_COLOR_ITEM))
			|| (nRefItemInfo == 1 && m_pRefITEM != nullptr && (m_pRefITEM->Kind == ITEMKIND_DEFENSE || m_pRefITEM->Kind == ITEMKIND_COLOR_ITEM))
			|| bCardContour)
		{
			ITEM* pColorItem = NULL;

			if (nRefItemInfo == 0)
				pColorItem = m_pRefItemInfo->GetRealItemInfo();
			else
				pColorItem = m_pRefITEM;

			if (bCardContour)//contour card link
			{			
				pColorItem = pCountourCardItem;			
			}
			else
			{
				if (nRefItemInfo == 0) {
					if (m_pRefItemInfo->ShapeItemNum)//contour
					{
						ITEM* pShapeItem = g_pDatabase->GetServerItemInfo(m_pRefItemInfo->ShapeItemNum);
						if (pShapeItem)
							pColorItem = pShapeItem;
					}
					if (m_pRefItemInfo->ColorCode > 0 && m_pRefItemInfo->ShapeItemNum == 0)//color
					{
						ITEM* pShapeItem2 = g_pDatabase->GetServerItemInfo(m_pRefItemInfo->ColorCode);
						if (pShapeItem2)
							pColorItem = pShapeItem2;
					}
				}
			}
			if (pColorItem) {
				int nResNum = g_pShuttleChild->GetUnitNumFromCharacter(pColorItem->SourceIndex, pColorItem->ReqUnitKind, 0, 0, 2);

				CSkinnedMesh* pSkinnedMeshEngine = g_pScene->m_pObjectRender->InitData(nResNum, _OBJECT_TYPE);

				D3DXMATRIX pMatOldView, pMatOldProj, pMatPresView, pMatPresProj, pMatrix;
				D3DXMatrixIdentity(&pMatOldView);
				D3DXMatrixIdentity(&pMatOldProj);
				D3DXMatrixIdentity(&pMatPresView);
				D3DXMatrixIdentity(&pMatPresProj);
				D3DXMatrixIdentity(&pMatrix);

				D3DXMATRIX pTemp, pMatRotX, pMatRotZ, pMatScaling;
				D3DXMatrixIdentity(&pTemp);
				D3DXMatrixIdentity(&pMatRotX);
				D3DXMatrixIdentity(&pMatRotZ);
				D3DXMatrixIdentity(&pMatScaling);

				g_pD3dDev->GetTransform(D3DTS_VIEW, &pMatOldView);
				g_pD3dDev->GetTransform(D3DTS_PROJECTION, &pMatOldProj);

				float fUnitScaling = 0.01f;
				float fEqPosX = ((float)m_ptItemInfo.x / (float)g_pD3dApp->GetBackBufferDesc().Width) * 2;
				float fEqCenterX = (640.0f / (float)g_pD3dApp->GetBackBufferDesc().Width);
				float fEqPosY = ((float)m_ptItemInfo.y / (float)g_pD3dApp->GetBackBufferDesc().Height) * 2;
				float fDecreaseY = 500.0f;

				if(pColorItem->Kind == ITEMKIND_COLOR_ITEM)
					fDecreaseY -= 300.0f;

				float fEqCenterY = (fDecreaseY / (float)g_pD3dApp->GetBackBufferDesc().Height);

				float tempscal = (float)g_pD3dApp->GetBackBufferDesc().Width / (float)g_pD3dApp->GetBackBufferDesc().Height;
				D3DXMatrixScaling(&pMatScaling, fUnitScaling, fUnitScaling * tempscal, fUnitScaling);
				D3DXMatrixTranslation(&pTemp, -1.0f + fEqPosX + fEqCenterX, 1.0f - fEqPosY - fEqCenterY, 0.5f);

				float fRotationX = SHUTTLE_ROTATION_DEFAULT_X + 0.15f;
				m_fRotationZ += g_pD3dApp->GetElapsedTime() * 1.5f * 1.0f;
				float fRotationZ = m_fRotationZ;

				D3DXMatrixRotationX(&pMatRotX, fRotationX);
				D3DXMatrixRotationY(&pMatRotZ, fRotationZ);
				pMatrix = pMatScaling * pMatRotX * pMatRotZ * pTemp;

				g_pD3dDev->SetTransform(D3DTS_VIEW, &pMatPresView);
				g_pD3dDev->SetTransform(D3DTS_PROJECTION, &pMatPresProj);

				m_pMatInven = pMatrix;

				if (pSkinnedMeshEngine != NULL) {
					pSkinnedMeshEngine->SetWorldMatrix(pMatrix);
					pSkinnedMeshEngine->AnotherTexture(1);
					pSkinnedMeshEngine->Render(FALSE, _SHUTTLE);
				}

				g_pD3dDev->SetTransform(D3DTS_VIEW, &pMatOldView);
				g_pD3dDev->SetTransform(D3DTS_PROJECTION, &pMatOldProj);
				g_pD3dDev->SetRenderState(D3DRS_LIGHTING, TRUE);
			}
		}

		g_pD3dDev->SetRenderTarget(0, pSurfaceOrig);
		SAFE_RELEASE(pSurfaceOrig);
		SAFE_RELEASE(pSurfaceMirror);

		m_pStateBlock->Apply();

		D3DXVECTOR2 v2Trans{
			float(m_ptItemInfo.x),
			float(m_ptItemInfo.y)
		};

		RECT texRect{
			m_ptItemInfo.x,
			m_ptItemInfo.y,
			m_ptItemInfo.x + m_szTooltip.cx*2,
			m_ptItemInfo.y + m_szTooltip.cy
		};

		g_pD3dApp->GetSpriteBatch()->Draw(m_pMirrorTexture, &texRect, NULL, NULL, 0, &v2Trans);
	}

	

}

void CINFItemInfo::Tick()
{
	FLOG( "CINFItemInfo::Tick()" );

}

///////////////////////////////////////////////////////////////////////////////
/// \fn			void CINFItemInfo::SetItemName(int nParameterIndex)
/// \brief		æ∆¿Ã≈€ ¿Ã∏ß¿ª ¡§«—¥Ÿ.
/// \author		dhkwon
/// \date		2004-04-02 ~ 2004-04-02
/// \warning	prefix, suffix∏¶ ∫Ÿø©¡ÿ¥Ÿ.
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CINFItemInfo::SetItemName(int nParameterIndex)
{
	if (m_nBigIconNum > 0)
		m_ptItemInfo.y = m_ptItemInfo.y + ITEMINFO_BIGICON_GAB;

	memset( m_strItemInfo[nParameterIndex], 0x00, ITEMINFO_ITEM_FULL_NAME);
	memset( m_strItemName, 0x00, ITEMINFO_ITEM_FULL_NAME);
	if(m_pRefPrefixRareInfo)
	{
		wsprintf( m_strItemName, "%s",m_pRefPrefixRareInfo->Name );
		wsprintf( m_strItemInfo[nParameterIndex], "\\g%s\\g", m_pRefPrefixRareInfo->Name );
	}
	if(m_bEnableItem)
	{
		if(IsStringColor(m_pRefITEM->ItemName))	
		{
			wsprintf( m_strItemName,"%s %s", m_strItemName, m_pRefITEM->ItemName);
			wsprintf( m_strItemInfo[nParameterIndex],"%s %s", m_strItemInfo[nParameterIndex], m_pRefITEM->ItemName);				
		}
		else if(m_pRefPrefixRareInfo || m_pRefSuffixRareInfo)
		{
			if(m_pRefEnchant)
			{
				wsprintf( m_strItemName,"%s %s", m_strItemName, m_pRefITEM->ItemName);
				wsprintf( m_strItemInfo[nParameterIndex],"%s \\e%s\\e", m_strItemInfo[nParameterIndex], m_pRefITEM->ItemName);
			}
			else
			{
				wsprintf( m_strItemName,"%s %s", m_strItemName, m_pRefITEM->ItemName);
				wsprintf( m_strItemInfo[nParameterIndex],"%s \\g%s\\g", m_strItemInfo[nParameterIndex], m_pRefITEM->ItemName);
			}
		}
		else
		{
			if(m_pRefEnchant)
			{
				wsprintf( m_strItemName,"%s %s", m_strItemName, m_pRefITEM->ItemName);
				wsprintf( m_strItemInfo[nParameterIndex],"%s \\e%s\\e", m_strItemInfo[nParameterIndex], m_pRefITEM->ItemName);				
			}
			else
			{
				wsprintf( m_strItemName,"%s %s", m_strItemName, m_pRefITEM->ItemName);
				wsprintf( m_strItemInfo[nParameterIndex],"%s %s", m_strItemInfo[nParameterIndex], m_pRefITEM->ItemName);				
			}
		}
	}
	else
	{
		wsprintf( m_strItemName,"%s %s", m_strItemName, m_pRefITEM->ItemName);
		wsprintf( m_strItemInfo[nParameterIndex],"%s \\r%s\\r", m_strItemInfo[nParameterIndex], m_pRefITEM->ItemName);
	}
	if(m_pRefSuffixRareInfo)
	{
		wsprintf( m_strItemName, "%s %s", m_strItemName, m_pRefSuffixRareInfo->Name );
		wsprintf( m_strItemInfo[nParameterIndex], "%s \\g%s\\g", m_strItemInfo[nParameterIndex], m_pRefSuffixRareInfo->Name );
	}

#ifdef _INET_LINK_CHAT
	if(COMPARE_RACE(g_pShuttleChild->m_myShuttleInfo.Race,RACE_OPERATION|RACE_GAMEMASTER) && m_pRefItemInfo)
#else
	if (m_pRefItemInfo)
#endif
	{
		wsprintf ( m_strItemName , "%s [ID:%I64d]" , m_strItemName , m_pRefItemInfo->UniqueNumber );
		wsprintf ( m_strItemInfo[nParameterIndex] , "%s [ID:%I64d]" , m_strItemInfo[nParameterIndex] , m_pRefItemInfo->UniqueNumber );
	}

	if ( COMPARE_RACE ( g_pShuttleChild->m_myShuttleInfo.Race , RACE_OPERATION|RACE_GAMEMASTER ) )
	{
		wsprintf ( m_strItemName , "%s (Idx:%d)" , m_strItemName , m_pRefITEM->ItemNum );
		wsprintf ( m_strItemInfo[nParameterIndex] , "%s (Idx:%d)" , m_strItemInfo[nParameterIndex] , m_pRefITEM->ItemNum );
	}

}

///////////////////////////////////////////////////////////////////////////////
/// \fn			BOOL CINFItemInfo::IsStringColor(char *i_szStr)
/// \brief		Ω∫∆Æ∏µø° ªˆªÛ¿Ã ¿÷¥¬¡ˆ √º≈©.
/// \author		dgwoo
/// \date		2008-04-16 ~ 2008-04-16
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CINFItemInfo::IsStringColor(char *i_szStr)
{
	BOOL bColor = FALSE;
	int  nCnt = 0;
	if(NULL == i_szStr)
		return bColor;
	while(strlen(i_szStr) > nCnt)
	{
		if(i_szStr[nCnt] == '\\')
		{
			bColor = TRUE;
			break;
		}
		nCnt++;
	}
	return bColor;
}
///////////////////////////////////////////////////////////////////////////////
/// \fn			CINFItemInfo::SetItemKind( int nParameterIndex )
/// \brief		æ∆¿Ã≈€ ¡æ∑˘
/// \author		dhkwon
/// \date		2004-09-03 ~ 2004-09-03
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CINFItemInfo::SetItemKind( int nParameterIndex )
{
	wsprintf( m_strItemInfo[nParameterIndex],STRMSG_C_ITEM_0017, CAtumSJ::GetItemKindName(m_pRefITEM->Kind));//"¡æ∑˘ : %s"
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			CINFItemInfo::SetUnitKind(int nParameterIndex)
/// \brief		
/// \author		dhkwon
/// \date		2004-06-14 ~ 2004-06-14
/// \warning	RareInfo, Enchant æ¯¿Ω
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CINFItemInfo::SetUnitKind(int nParameterIndex)
{
	FLOG( "CINFItemInfo::SetUnitKind(int nParameterIndex, USHORT nUnitKind)" );
	strcpy( m_strItemInfo[nParameterIndex], STRMSG_C_ITEM_0018);//"±‚¡æ : "

	USHORT nUnitKind = m_pRefITEM->ReqUnitKind;
	if(UNITKIND_ALL_MASK == nUnitKind)
	{
		strcat(m_strItemInfo[nParameterIndex],STRMSG_C_ITEM_0019);//"¿¸±‚¡æøÎ"
	}
	else if(0 == nUnitKind)
	{
		strcat(m_strItemInfo[nParameterIndex],STRMSG_C_ITEM_0020);//"¿œπ›"
	}
	else
	{
		if( (g_pShuttleChild->m_myShuttleInfo.UnitKind & m_pRefITEM->ReqUnitKind) == 0)
		{
			strcat(m_strItemInfo[nParameterIndex],"\\r");
		}
		// 2007-03-29 by bhsohn China String
		char chGear[64];
		memset(chGear, 0x00, 64);
		if(IS_BT(nUnitKind))
		{
			//strcat(m_strItemInfo[nParameterIndex],"B-GEAR ");
			wsprintf(chGear, "%s ", STRCMD_CS_UNITKIND_BGEAR);
			strcat(m_strItemInfo[nParameterIndex],chGear);
		}
		if(IS_OT(nUnitKind))
		{
			//strcat(m_strItemInfo[nParameterIndex],"M-GEAR ");
			wsprintf(chGear, "%s ", STRCMD_CS_UNITKIND_MGEAR);
			strcat(m_strItemInfo[nParameterIndex],chGear);
		}
		if(IS_DT(nUnitKind))
		{
			//strcat(m_strItemInfo[nParameterIndex],"A-GEAR ");
			wsprintf(chGear, "%s ", STRCMD_CS_UNITKIND_AGEAR);
			strcat(m_strItemInfo[nParameterIndex],chGear);
		}
		if(IS_ST(nUnitKind))
		{
			//strcat(m_strItemInfo[nParameterIndex],"I-GEAR ");
			wsprintf(chGear, "%s ", STRCMD_CS_UNITKIND_IGEAR);
			strcat(m_strItemInfo[nParameterIndex],chGear);
		}
		// end 2007-03-29 by bhsohn China String
	}
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			SetReqLevel(int nParameterIndex, BYTE nReqLevel )
/// \brief		
/// \author		dhkwon
/// \date		2004-06-14 ~ 2004-06-14
/// \warning	RareInfo, Enchant æ¯¿Ω
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CINFItemInfo::SetReqLevel(int nParameterIndex )
{
	FLOG( "CINFItemInfo::SetReqLevel(int nParameterIndex, BYTE nReqLevel )" );
	if(m_pRefItemInfo != NULL)
	{
		ITEM* pRealItem = m_pRefItemInfo->GetRealItemInfo();

		if(pRealItem == NULL)
			return;

		
		// 2006-09-13 by ispark, ∑π∫ß ¿˚¿∫ ∞Õ∏∏ ∆«¥‹¿∏∑Œ µ«æÓ ¿÷æÓº≠ ReqMaxLevel ∆˜«‘Ω√≈¥
//		if(m_bEnableItem || (g_pShuttleChild->m_myShuttleInfo.Level >= pRealItem->ReqMinLevel &&
//			g_pShuttleChild->m_myShuttleInfo.Level <= pRealItem->ReqMinLevel))
		if(m_bEnableItem || (g_pShuttleChild->m_myShuttleInfo.Level >= pRealItem->ReqMinLevel &&
			g_pShuttleChild->m_myShuttleInfo.Level <= pRealItem->ReqMaxLevel))
		{
			if(pRealItem->ReqMaxLevel == 0)
			{
				// 2008-09-26 by bhsohn Ω≈±‘ ¿Œ√æ∆Æ √≥∏Æ
				float fPreSuffixLV = GetEnchantPreSuffixInfo(DES_REQ_MIN_LEVEL);		

				CParamFactor *pPFactor = m_pRefItemInfo->GetParamFactor();
				if(pPFactor
					&& 0 != pPFactor->pfp_REQ_MIN_LEVEL)
				{					
					if(fPreSuffixLV > pPFactor->pfp_REQ_MIN_LEVEL)
					{						
						int nMinLevel = pPFactor->pfp_REQ_MIN_LEVEL - fPreSuffixLV;
						if(nMinLevel != 0)
						{
							// ø£¡¯ ¿Œ√æ∏∏«— π´±‚
							wsprintf( m_strItemInfo[nParameterIndex], STRMSG_C_ITEM_0022, m_pRefITEM->ReqMinLevel);//"ø‰±∏ : LEVEL[%d]"
							wsprintf( m_strItemInfo[nParameterIndex], "%s\\e[%d]\\e", m_strItemInfo[nParameterIndex], nMinLevel);//"ø‰±∏ : LEVEL[%d]"
						}
						else
						{
							// ø£¡¯ ¿Œ√æ π◊ ¡¢µŒ, ¡¢πÃø…º«
							wsprintf( m_strItemInfo[nParameterIndex], STRMSG_C_ITEM_0021, m_pRefITEM->ReqMinLevel, pPFactor->pfp_REQ_MIN_LEVEL);//"ø‰±∏ : LEVEL(%d\\g[%d]\\g)"						
							wsprintf( m_strItemInfo[nParameterIndex], "%s\\e[%d]\\e", m_strItemInfo[nParameterIndex], nMinLevel);//"ø‰±∏ : LEVEL[%d]"
						}
						
					}				
					else
					{					
						// ¡¢µŒ, ¡¢πÃ ø…º«∏∏ ¿÷¥¬ ø£¡¯
						wsprintf( m_strItemInfo[nParameterIndex], STRMSG_C_ITEM_0021, m_pRefITEM->ReqMinLevel, pPFactor->pfp_REQ_MIN_LEVEL);//"ø‰±∏ : LEVEL(%d\\g[%d]\\g)"						
						
					}
				}
				else
				{
					wsprintf( m_strItemInfo[nParameterIndex], STRMSG_C_ITEM_0022, m_pRefITEM->ReqMinLevel);//"ø‰±∏ : LEVEL[%d]"
				}
			}
			else
			{
				wsprintf( m_strItemInfo[nParameterIndex], STRMSG_C_ITEM_0023, //"ø‰±∏ : LEVEL[%d]~[%d]"
				pRealItem->ReqMinLevel, pRealItem->ReqMaxLevel);
			}
		}
		else
		{
			if(pRealItem->ReqMaxLevel == 0)
			{
				CParamFactor *pPFactor = m_pRefItemInfo->GetParamFactor();
				if(pPFactor
					&& 0 != pPFactor->pfp_REQ_MIN_LEVEL)
				{
					wsprintf( m_strItemInfo[nParameterIndex], STRMSG_C_ITEM_0024, m_pRefITEM->ReqMinLevel, pPFactor->pfp_REQ_MIN_LEVEL);//"ø‰±∏ : \\rLEVEL(%d\\g[%d]\\r)\\r"
				}				
				else
				{
					wsprintf( m_strItemInfo[nParameterIndex], STRMSG_C_ITEM_0025, pRealItem->ReqMinLevel); //"ø‰±∏ : \\rLEVEL[%d]\\r"
				}
			}
			else
			{
				wsprintf( m_strItemInfo[nParameterIndex], STRMSG_C_ITEM_0026, //"ø‰±∏ : \\rLEVEL[%d]~[%d]"
				pRealItem->ReqMinLevel, pRealItem->ReqMaxLevel);
			}
		}
	}
	else
	{
		if(m_bEnableItem || (g_pShuttleChild->m_myShuttleInfo.Level >= m_pRefITEM->ReqMinLevel &&
			g_pShuttleChild->m_myShuttleInfo.Level <= m_pRefITEM->ReqMinLevel))
		{
			if(m_pRefITEM->ReqMaxLevel == 0)
				wsprintf( m_strItemInfo[nParameterIndex], STRMSG_C_ITEM_0022, m_pRefITEM->ReqMinLevel);
			else
				wsprintf( m_strItemInfo[nParameterIndex], STRMSG_C_ITEM_0023, 
				m_pRefITEM->ReqMinLevel, m_pRefITEM->ReqMaxLevel);
		}
		else
		{
			if(m_pRefITEM->ReqMaxLevel == 0)
				wsprintf( m_strItemInfo[nParameterIndex], STRMSG_C_ITEM_0025, m_pRefITEM->ReqMinLevel); 
			else
				wsprintf( m_strItemInfo[nParameterIndex], STRMSG_C_ITEM_0026, 
				m_pRefITEM->ReqMinLevel, m_pRefITEM->ReqMaxLevel);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			SetShopReqItem(int nParameterIndex )
/// \brief		Ω∫≈≥ ªÛ¡°ø°º≠ ±∏¿‘Ω√ø° « ø‰«— æ∆¿Ã≈€ ∫∏ø©¡÷±‚
/// \author		dhkwon
/// \date		2004-11-11 ~ 2004-11-11
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CINFItemInfo::SetShopReqItem(int nParameterIndex )
{
	ITEM* pITEM = g_pDatabase->GetServerItemInfo( m_pRefITEM->LinkItem );
	if(pITEM == NULL )
	{
		return;
	}
	if(g_pStoreData->FindItemInInventoryByItemNum( m_pRefITEM->LinkItem ) == NULL )
	{
		wsprintf( m_strItemInfo[nParameterIndex], STRMSG_C_ITEM_0027, pITEM->ItemName); //"±∏¿‘Ω√ « ø‰ : \\r%s\\r"
	}
	else
	{
		wsprintf( m_strItemInfo[nParameterIndex], STRMSG_C_ITEM_0028, pITEM->ItemName); //"±∏¿‘Ω√ « ø‰ : %s"
	}
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			CINFItemInfo::SetReqStat(int nParameterIndex )
/// \brief		
/// \author		dhkwon
/// \date		2004-09-09 ~ 2004-09-09
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CINFItemInfo::SetReqStat(int nParameterIndex )
{
	strcpy( m_strItemInfo[nParameterIndex], STRMSG_C_STAT_0008 );//"ø‰±∏Ω∫≈» :"
	if(m_pRefITEM->ReqGearStat.AttackPart != 0 )
	{
		if(g_pShuttleChild->m_myShuttleInfo.TotalGearStat.AttackPart >= m_pRefITEM->ReqGearStat.AttackPart)
		{
			wsprintf( m_strItemInfo[nParameterIndex], STRMSG_C_STAT_0009,m_strItemInfo[nParameterIndex], m_pRefITEM->ReqGearStat.AttackPart); //"%s ∞¯∞›[%d]"
		}
		else
		{
			wsprintf( m_strItemInfo[nParameterIndex], STRMSG_C_STAT_0010,m_strItemInfo[nParameterIndex], m_pRefITEM->ReqGearStat.AttackPart); //"%s \\r∞¯∞›[%d]\\r"
		}
	}
	if(m_pRefITEM->ReqGearStat.DefensePart != 0 )//≥ª±∏->πÊæÓ
	{
		if(g_pShuttleChild->m_myShuttleInfo.TotalGearStat.DefensePart >= m_pRefITEM->ReqGearStat.DefensePart)
		{
			wsprintf( m_strItemInfo[nParameterIndex], STRMSG_C_STAT_0011,m_strItemInfo[nParameterIndex], m_pRefITEM->ReqGearStat.DefensePart); //"%s πÊæÓ[%d]"
		}
		else
		{
			wsprintf( m_strItemInfo[nParameterIndex], STRMSG_C_STAT_0012,m_strItemInfo[nParameterIndex], m_pRefITEM->ReqGearStat.DefensePart); //"%s \\rπÊæÓ[%d]\\r"
		}
	}
	if(m_pRefITEM->ReqGearStat.FuelPart != 0 )
	{
		if(g_pShuttleChild->m_myShuttleInfo.TotalGearStat.FuelPart >= m_pRefITEM->ReqGearStat.FuelPart)
		{
			wsprintf( m_strItemInfo[nParameterIndex], STRMSG_C_STAT_0013,m_strItemInfo[nParameterIndex], m_pRefITEM->ReqGearStat.FuelPart); //"%s ø¨∑·[%d]"
		}
		else
		{
			wsprintf( m_strItemInfo[nParameterIndex], STRMSG_C_STAT_0014, m_strItemInfo[nParameterIndex],m_pRefITEM->ReqGearStat.FuelPart); //"%s \\rø¨∑·[%d]\\r"
		}
	}
	if(m_pRefITEM->ReqGearStat.SoulPart != 0 )
	{
		if(g_pShuttleChild->m_myShuttleInfo.TotalGearStat.SoulPart >= m_pRefITEM->ReqGearStat.SoulPart)
		{
			wsprintf( m_strItemInfo[nParameterIndex], STRMSG_C_STAT_0015,m_strItemInfo[nParameterIndex], m_pRefITEM->ReqGearStat.SoulPart); //"%s ¡§Ω≈[%d]"
		}
		else
		{
			wsprintf( m_strItemInfo[nParameterIndex], STRMSG_C_STAT_0016,m_strItemInfo[nParameterIndex], m_pRefITEM->ReqGearStat.SoulPart); //"%s \\r¡§Ω≈[%d]\\r"
		}
	}
	if(m_pRefITEM->ReqGearStat.ShieldPart != 0)//πÊæÓ->ΩØµÂ
	{
		if(g_pShuttleChild->m_myShuttleInfo.TotalGearStat.ShieldPart >= m_pRefITEM->ReqGearStat.ShieldPart)
		{
			wsprintf( m_strItemInfo[nParameterIndex], STRMSG_C_STAT_0017,m_strItemInfo[nParameterIndex], m_pRefITEM->ReqGearStat.ShieldPart); //"%s ΩØµÂ[%d]"
		}
		else
		{
			wsprintf( m_strItemInfo[nParameterIndex], STRMSG_C_STAT_0018,m_strItemInfo[nParameterIndex], m_pRefITEM->ReqGearStat.ShieldPart); //"%s \\rΩØµÂ[%d]\\r"
		}
	}
	if(m_pRefITEM->ReqGearStat.DodgePart != 0 )
	{
		if(g_pShuttleChild->m_myShuttleInfo.TotalGearStat.DodgePart >= m_pRefITEM->ReqGearStat.DodgePart)
		{
			wsprintf( m_strItemInfo[nParameterIndex], STRMSG_C_STAT_0019,m_strItemInfo[nParameterIndex], m_pRefITEM->ReqGearStat.DodgePart); //"%s »∏««[%d]"
		}
		else
		{
			wsprintf( m_strItemInfo[nParameterIndex], STRMSG_C_STAT_0020,m_strItemInfo[nParameterIndex], m_pRefITEM->ReqGearStat.DodgePart); //"%s \\r»∏««[%d]\\r"
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			CINFItemInfo::SetReqItemKind( int nParameterIndex )
/// \brief		ø‰±∏ æ∆¿Ã≈€ ¡æ∑˘
/// \author		dhkwon
/// \date		2004-09-03 ~ 2004-09-03
/// \warning	Ω∫≈≥,¿Œ√¶∆Æ ƒ´µÂø°º≠ ªÁøÎ
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CINFItemInfo::SetReqItemKind( int nParameterIndex )
{
	wsprintf( m_strItemInfo[nParameterIndex], STRMSG_C_ITEM_0029, CAtumSJ::GetItemKindName(m_pRefITEM->ReqItemKind)); //"¿˚øÎ æ∆¿Ã≈€ : %s"
}
#ifdef BONUS_STAT_ITEM
float CalcPercentDecreased(float num1, float num2)
{
	return ((num1 / num2)*100.0f) - 100.0f;
}
void CINFItemInfo::SetAttack(int nParameterIndex, BOOL Bonus)
{
	float nBonusAttackMin = m_pRefBonus ? m_pRefBonus->pfm_MINATTACK_01 + m_pRefBonus->pfm_MINATTACK_02 : 0;
	float nBonusAttackMax = m_pRefBonus ? m_pRefBonus->pfm_MAXATTACK_01 + m_pRefBonus->pfm_MAXATTACK_02 : 0;
	float fEnchantAttackMin = m_pRefEnchant ? m_pRefEnchant->pfm_MINATTACK_01 + m_pRefEnchant->pfm_MINATTACK_02 : 0;
	float fEnchantAttackMax = m_pRefEnchant ? m_pRefEnchant->pfm_MAXATTACK_01 + m_pRefEnchant->pfm_MAXATTACK_02 : 0;
	float fRareInfoAttackMin = 0;
	float fRareInfoAttackMax = 0;
	BYTE nEnchantShotNum = m_pRefEnchant ? m_pRefEnchant->pfp_SHOTNUM_01 : 0;
	BYTE nEnchantMultiNum = m_pRefEnchant ? m_pRefEnchant->pfp_MULTINUM_02 : 0;
	BYTE nRareInfoShotNum = 0;
	BYTE nRareInfoMultiNum = 0;
	if (m_pRefPrefixRareInfo)
	{
		for (int i = 0; i<SIZE_DES_PARAM_PER_RARE_ITEM_INFO; i++)
		{
			if (m_pRefPrefixRareInfo->DesParameter[i] == DES_MINATTACK_01 ||
				m_pRefPrefixRareInfo->DesParameter[i] == DES_MINATTACK_02)
			{
				fRareInfoAttackMin += m_pRefPrefixRareInfo->ParameterValue[i];
			}
			else if (m_pRefPrefixRareInfo->DesParameter[i] == DES_MAXATTACK_01 ||
				m_pRefPrefixRareInfo->DesParameter[i] == DES_MAXATTACK_02)
			{
				fRareInfoAttackMax += m_pRefPrefixRareInfo->ParameterValue[i];
			}
			else if (m_pRefPrefixRareInfo->DesParameter[i] == DES_SHOTNUM_01 ||
				m_pRefPrefixRareInfo->DesParameter[i] == DES_SHOTNUM_02)
			{
				nRareInfoShotNum += (BYTE)m_pRefPrefixRareInfo->ParameterValue[i];
			}
			else if (//m_pRefPrefixRareInfo->DesParameter[i] == DES_MULTINUM_01 ||
				m_pRefPrefixRareInfo->DesParameter[i] == DES_MULTINUM_02)
			{
				nRareInfoMultiNum += (BYTE)m_pRefPrefixRareInfo->ParameterValue[i];
			}
		}
	}
	if (m_pRefSuffixRareInfo)
	{
		for (int i = 0; i<SIZE_DES_PARAM_PER_RARE_ITEM_INFO; i++)
		{
			if (m_pRefSuffixRareInfo->DesParameter[i] == DES_MINATTACK_01 ||
				m_pRefSuffixRareInfo->DesParameter[i] == DES_MINATTACK_02)
			{
				fRareInfoAttackMin += m_pRefSuffixRareInfo->ParameterValue[i];
			}
			else if (m_pRefSuffixRareInfo->DesParameter[i] == DES_MAXATTACK_01 ||
				m_pRefSuffixRareInfo->DesParameter[i] == DES_MAXATTACK_02)
			{
				fRareInfoAttackMax += m_pRefSuffixRareInfo->ParameterValue[i];
			}
			else if (m_pRefSuffixRareInfo->DesParameter[i] == DES_SHOTNUM_01 ||
				m_pRefSuffixRareInfo->DesParameter[i] == DES_SHOTNUM_02)
			{
				nRareInfoShotNum += (BYTE)m_pRefSuffixRareInfo->ParameterValue[i];
			}
			else if (//m_pRefSuffixRareInfo->DesParameter[i] == DES_MULTINUM_01 ||
				m_pRefSuffixRareInfo->DesParameter[i] == DES_MULTINUM_02)
			{
				nRareInfoMultiNum += (BYTE)m_pRefSuffixRareInfo->ParameterValue[i];
			}
		}
	}
	if (Bonus)
	{
		//check with server and use m_bFullStat for print decrease value
		//server side code its max -30% of nominal value
#ifdef BONUS_STAT_INVERTED
		sprintf(m_strItemInfo[nParameterIndex], STRMSG_C_ITEM_0030, (m_pRefITEM->AbilityMin));
#else
		sprintf(m_strItemInfo[nParameterIndex], STRMSG_C_ITEM_0030, (m_pRefITEM->AbilityMin + (m_pRefITEM->AbilityMin * 0.30)));
#endif
	}
	else
	{
#ifdef BONUS_STAT_INVERTED
		sprintf(m_strItemInfo[nParameterIndex], STRMSG_C_ITEM_0030, m_pRefITEM->AbilityMin - nBonusAttackMin*100.0f);//"∞¯∞› : (%.0f"
#else
		sprintf(m_strItemInfo[nParameterIndex], STRMSG_C_ITEM_0030, m_pRefITEM->AbilityMin + nBonusAttackMin*100.0f);//"∞¯∞› : (%.0f"
#endif
		if (fEnchantAttackMin > 0)
			sprintf(m_strItemInfo[nParameterIndex], "%s\\e[+%.0f%%]\\e", m_strItemInfo[nParameterIndex], fEnchantAttackMin*100.0f);
		else if (fEnchantAttackMin < 0)
			sprintf(m_strItemInfo[nParameterIndex], "%s\\e[%.0f%%]\\e", m_strItemInfo[nParameterIndex], fEnchantAttackMin*100.0f);
		if (fRareInfoAttackMin > 0)
			sprintf(m_strItemInfo[nParameterIndex], "%s\\g[+%.0f%%]\\g", m_strItemInfo[nParameterIndex], fRareInfoAttackMin*100.0f);
		else if (fRareInfoAttackMin < 0)
			sprintf(m_strItemInfo[nParameterIndex], "%s\\g[%.0f%%]\\g", m_strItemInfo[nParameterIndex], fRareInfoAttackMin*100.0f);
	}
	if (Bonus)
	{
#ifdef BONUS_STAT_INVERTED
		float nDecMin = CalcPercentDecreased(m_pRefItemInfo->GetRealItemInfo()->AbilityMin, m_pRefItemInfo->GetItemInfo()->AbilityMin);
		float nDecMax = CalcPercentDecreased(m_pRefItemInfo->GetRealItemInfo()->AbilityMax, m_pRefItemInfo->GetItemInfo()->AbilityMax);
	
		if (nDecMin < 0.0f && nDecMax < 0.0f)
			sprintf(m_strItemInfo[nParameterIndex], "%s\\r(%.2f%%)\\r ~ %.0f\\r(%.2f%%)\\r", m_strItemInfo[nParameterIndex], nDecMin, (m_pRefITEM->AbilityMax), nDecMax);
		else if (nDecMin<0.0f && nDecMax == 0.0f)
			sprintf(m_strItemInfo[nParameterIndex], "%s\\r(%.2f%%)\\r ~ %.0f\\g*\\g", m_strItemInfo[nParameterIndex], nDecMin, (m_pRefITEM->AbilityMax));
		else if (nDecMin == 0.0f && nDecMax < 0.0f)
			sprintf(m_strItemInfo[nParameterIndex], "%s\\g*\\g ~ %.0f\\r(%.2f%%)\\r", m_strItemInfo[nParameterIndex], (m_pRefITEM->AbilityMax), nDecMax);
		else if (nDecMin == 0.0f && nDecMax == 0.0f)
			sprintf(m_strItemInfo[nParameterIndex], "%s\\g*\\g ~ %.0f\\g*\\g", m_strItemInfo[nParameterIndex], (m_pRefITEM->AbilityMax));
		else
			sprintf(m_strItemInfo[nParameterIndex], "%s ~ %.0f", m_strItemInfo[nParameterIndex], (m_pRefITEM->AbilityMax));
		
#else
		sprintf(m_strItemInfo[nParameterIndex], "%s ~ %.0f", m_strItemInfo[nParameterIndex], (m_pRefITEM->AbilityMax + (m_pRefITEM->AbilityMax * 0.30)));
#endif
		sprintf(m_strItemInfo[nParameterIndex], "%s) X (%d", m_strItemInfo[nParameterIndex], m_pRefITEM->ShotNum);
		sprintf(m_strItemInfo[nParameterIndex], "%s X %d", m_strItemInfo[nParameterIndex], m_pRefITEM->MultiNum);
	}
	else
	{
#ifdef BONUS_STAT_INVERTED
		sprintf(m_strItemInfo[nParameterIndex], "%s ~ %.0f", m_strItemInfo[nParameterIndex], m_pRefITEM->AbilityMax - nBonusAttackMax*100.0f);
#else
		sprintf(m_strItemInfo[nParameterIndex], "%s ~ %.0f", m_strItemInfo[nParameterIndex], m_pRefITEM->AbilityMax + nBonusAttackMax*100.0f);
#endif
	if (fEnchantAttackMax > 0)
			sprintf(m_strItemInfo[nParameterIndex], "%s\\e[+%.0f%%]\\e", m_strItemInfo[nParameterIndex], fEnchantAttackMax*100.0f);
		else if (fEnchantAttackMin < 0)
			sprintf(m_strItemInfo[nParameterIndex], "%s\\e[%.0f%%]\\e", m_strItemInfo[nParameterIndex], fEnchantAttackMax*100.0f);
		if (fRareInfoAttackMax > 0)
			sprintf(m_strItemInfo[nParameterIndex], "%s\\g[+%.0f%%]\\g", m_strItemInfo[nParameterIndex], fRareInfoAttackMax*100.0f);
		else if (fRareInfoAttackMin < 0)
			sprintf(m_strItemInfo[nParameterIndex], "%s\\g[%.0f%%]\\g", m_strItemInfo[nParameterIndex], fRareInfoAttackMax*100.0f);
		sprintf(m_strItemInfo[nParameterIndex], "%s) X (%d", m_strItemInfo[nParameterIndex], m_pRefITEM->ShotNum);

		if (nEnchantShotNum > 0)
			sprintf(m_strItemInfo[nParameterIndex], "%s\\e[+%d]\\e", m_strItemInfo[nParameterIndex], nEnchantShotNum);
		else if (nEnchantShotNum < 0)
			sprintf(m_strItemInfo[nParameterIndex], "%s\\e[%d]\\e", m_strItemInfo[nParameterIndex], nEnchantShotNum);
		if (nRareInfoShotNum > 0)
			sprintf(m_strItemInfo[nParameterIndex], "%s\\g[+%d]\\g", m_strItemInfo[nParameterIndex], nRareInfoShotNum);
		else if (nRareInfoShotNum < 0)
			sprintf(m_strItemInfo[nParameterIndex], "%s\\g[%d]\\g", m_strItemInfo[nParameterIndex], nRareInfoShotNum);
		sprintf(m_strItemInfo[nParameterIndex], "%s X %d", m_strItemInfo[nParameterIndex], m_pRefITEM->MultiNum);

		if (nEnchantMultiNum > 0)
			sprintf(m_strItemInfo[nParameterIndex], "%s\\e[+%d]\\e", m_strItemInfo[nParameterIndex], nEnchantMultiNum);
		else if (nEnchantMultiNum < 0)
			sprintf(m_strItemInfo[nParameterIndex], "%s\\e[%d]\\e", m_strItemInfo[nParameterIndex], nEnchantMultiNum);
		if (nRareInfoMultiNum > 0)
			sprintf(m_strItemInfo[nParameterIndex], "%s\\g[+%d]\\g", m_strItemInfo[nParameterIndex], nRareInfoMultiNum);
		else if (nRareInfoMultiNum < 0)
			sprintf(m_strItemInfo[nParameterIndex], "%s\\g[%d]\\g", m_strItemInfo[nParameterIndex], nRareInfoMultiNum);
	}
	strcat(m_strItemInfo[nParameterIndex], ")");
}

#else
///////////////////////////////////////////////////////////////////////////////
/// \fn			CINFItemInfo::SetAttack(int nParameterIndex)
/// \brief		
/// \author		dhkwon
/// \date		2004-06-14 ~ 2004-06-14
/// \warning	Enchant¡§∫∏, RareInfo¡§∫∏ ¿÷¿Ω 
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CINFItemInfo::SetAttack(int nParameterIndex)
{
	float fEnchantAttackMin = m_pRefEnchant ? m_pRefEnchant->pfm_MINATTACK_01+m_pRefEnchant->pfm_MINATTACK_02 : 0;
	float fEnchantAttackMax = m_pRefEnchant ? m_pRefEnchant->pfm_MAXATTACK_01+m_pRefEnchant->pfm_MAXATTACK_02 : 0;
	float fRareInfoAttackMin = 0;
	float fRareInfoAttackMax = 0;
	BYTE nEnchantShotNum = m_pRefEnchant ? m_pRefEnchant->pfp_SHOTNUM_01 : 0;
	BYTE nEnchantMultiNum = m_pRefEnchant ? m_pRefEnchant->pfp_MULTINUM_02 : 0;
	BYTE nRareInfoShotNum = 0;
	BYTE nRareInfoMultiNum = 0;
	if(m_pRefPrefixRareInfo)
	{
		for(int i=0; i<SIZE_DES_PARAM_PER_RARE_ITEM_INFO; i++)
		{
			if( m_pRefPrefixRareInfo->DesParameter[i] == DES_MINATTACK_01 || 
				m_pRefPrefixRareInfo->DesParameter[i] == DES_MINATTACK_02)
			{
				fRareInfoAttackMin += m_pRefPrefixRareInfo->ParameterValue[i];
			}
			else if(m_pRefPrefixRareInfo->DesParameter[i] == DES_MAXATTACK_01 ||
				m_pRefPrefixRareInfo->DesParameter[i] == DES_MAXATTACK_02)
			{
				fRareInfoAttackMax += m_pRefPrefixRareInfo->ParameterValue[i];
			}
			else if(m_pRefPrefixRareInfo->DesParameter[i] == DES_SHOTNUM_01  ||
				m_pRefPrefixRareInfo->DesParameter[i] == DES_SHOTNUM_02)
			{
				nRareInfoShotNum += (BYTE)m_pRefPrefixRareInfo->ParameterValue[i];
			}
			else if(//m_pRefPrefixRareInfo->DesParameter[i] == DES_MULTINUM_01 ||
				m_pRefPrefixRareInfo->DesParameter[i] == DES_MULTINUM_02)
			{
				nRareInfoMultiNum += (BYTE)m_pRefPrefixRareInfo->ParameterValue[i];
			}
		}
	}
	if(m_pRefSuffixRareInfo)
	{
		for(int i=0; i<SIZE_DES_PARAM_PER_RARE_ITEM_INFO; i++)
		{
			if( m_pRefSuffixRareInfo->DesParameter[i] == DES_MINATTACK_01 || 
				m_pRefSuffixRareInfo->DesParameter[i] == DES_MINATTACK_02)
			{
				fRareInfoAttackMin += m_pRefSuffixRareInfo->ParameterValue[i];
			}
			else if(m_pRefSuffixRareInfo->DesParameter[i] == DES_MAXATTACK_01 ||
				m_pRefSuffixRareInfo->DesParameter[i] == DES_MAXATTACK_02)
			{
				fRareInfoAttackMax += m_pRefSuffixRareInfo->ParameterValue[i];
			}
			else if(m_pRefSuffixRareInfo->DesParameter[i] == DES_SHOTNUM_01 ||
				m_pRefSuffixRareInfo->DesParameter[i] == DES_SHOTNUM_02)
			{
				nRareInfoShotNum += (BYTE)m_pRefSuffixRareInfo->ParameterValue[i];
			}
			else if(//m_pRefSuffixRareInfo->DesParameter[i] == DES_MULTINUM_01 ||
				m_pRefSuffixRareInfo->DesParameter[i] == DES_MULTINUM_02)
			{
				nRareInfoMultiNum += (BYTE)m_pRefSuffixRareInfo->ParameterValue[i];
			}
		}
	}

	sprintf( m_strItemInfo[nParameterIndex], STRMSG_C_ITEM_0030, m_pRefITEM->AbilityMin );//"∞¯∞› : (%.0f"
	if(fEnchantAttackMin > 0)
		sprintf( m_strItemInfo[nParameterIndex], "%s\\e[+%.0f%%]\\e", m_strItemInfo[nParameterIndex], fEnchantAttackMin*100.0f );
	else if(fEnchantAttackMin < 0) 
		sprintf( m_strItemInfo[nParameterIndex], "%s\\e[%.0f%%]\\e", m_strItemInfo[nParameterIndex], fEnchantAttackMin*100.0f );
	if(fRareInfoAttackMin > 0)
		sprintf( m_strItemInfo[nParameterIndex], "%s\\g[+%.0f%%]\\g", m_strItemInfo[nParameterIndex], fRareInfoAttackMin*100.0f );
	else if(fRareInfoAttackMin < 0) 
		sprintf( m_strItemInfo[nParameterIndex], "%s\\g[%.0f%%]\\g", m_strItemInfo[nParameterIndex], fRareInfoAttackMin*100.0f );

	sprintf( m_strItemInfo[nParameterIndex], "%s ~ %.0f", m_strItemInfo[nParameterIndex], m_pRefITEM->AbilityMax );
	if(fEnchantAttackMax > 0)
		sprintf( m_strItemInfo[nParameterIndex], "%s\\e[+%.0f%%]\\e", m_strItemInfo[nParameterIndex], fEnchantAttackMax*100.0f );
	else if(fEnchantAttackMin < 0) 
		sprintf( m_strItemInfo[nParameterIndex], "%s\\e[%.0f%%]\\e", m_strItemInfo[nParameterIndex], fEnchantAttackMax*100.0f );
	if(fRareInfoAttackMax > 0)
		sprintf( m_strItemInfo[nParameterIndex], "%s\\g[+%.0f%%]\\g", m_strItemInfo[nParameterIndex], fRareInfoAttackMax*100.0f );
	else if(fRareInfoAttackMin < 0) 
		sprintf( m_strItemInfo[nParameterIndex], "%s\\g[%.0f%%]\\g", m_strItemInfo[nParameterIndex], fRareInfoAttackMax*100.0f );
	sprintf( m_strItemInfo[nParameterIndex], "%s) X (%d",m_strItemInfo[nParameterIndex],m_pRefITEM->ShotNum);

	if(nEnchantShotNum>0)
		sprintf( m_strItemInfo[nParameterIndex], "%s\\e[+%d]\\e", m_strItemInfo[nParameterIndex], nEnchantShotNum );
	else if(nEnchantShotNum<0)
		sprintf( m_strItemInfo[nParameterIndex], "%s\\e[%d]\\e", m_strItemInfo[nParameterIndex], nEnchantShotNum );
	if(nRareInfoShotNum>0)
		sprintf( m_strItemInfo[nParameterIndex], "%s\\g[+%d]\\g", m_strItemInfo[nParameterIndex], nRareInfoShotNum );
	else if(nRareInfoShotNum<0)
		sprintf( m_strItemInfo[nParameterIndex], "%s\\g[%d]\\g", m_strItemInfo[nParameterIndex], nRareInfoShotNum );
	sprintf( m_strItemInfo[nParameterIndex], "%s X %d",m_strItemInfo[nParameterIndex],m_pRefITEM->MultiNum);

	if(nEnchantMultiNum>0)
		sprintf( m_strItemInfo[nParameterIndex], "%s\\e[+%d]\\e", m_strItemInfo[nParameterIndex], nEnchantMultiNum );
	else if(nEnchantMultiNum<0)
		sprintf( m_strItemInfo[nParameterIndex], "%s\\e[%d]\\e", m_strItemInfo[nParameterIndex], nEnchantMultiNum );
	if(nRareInfoMultiNum>0)
		sprintf( m_strItemInfo[nParameterIndex], "%s\\g[+%d]\\g", m_strItemInfo[nParameterIndex], nRareInfoMultiNum );
	else if(nRareInfoMultiNum<0)
		sprintf( m_strItemInfo[nParameterIndex], "%s\\g[%d]\\g", m_strItemInfo[nParameterIndex], nRareInfoMultiNum );
	strcat(m_strItemInfo[nParameterIndex], ")");
}
#endif
#ifdef BONUS_STAT_ITEM
void CINFItemInfo::SetSecAttack(int nParameterIndex, BOOL Bonus)
{
	if (m_pRefItemInfo)
	{
		
		
		if (Bonus){
			sprintf(m_strItemInfo[nParameterIndex], STRMSG_C_ITEM_0031, CAtumSJ::GetMinAttackPerSecond(m_pRefItemInfo->GetItemInfo(), FALSE));//"√ ¥Á ∞¯∞›∑¬ : %.0f"
			sprintf(m_strItemInfo[nParameterIndex], "%s ~ %.0f", m_strItemInfo[nParameterIndex], CAtumSJ::GetMaxAttackPerSecond(m_pRefItemInfo->GetItemInfo(), FALSE));
			/*
			ITEM * i_pMaxItem = m_pRefItemInfo->GetItemInfo(); //get normal item stats like from db
			float fMinAttack = (i_pMaxItem->AbilityMin)*i_pMaxItem->ShotNum*i_pMaxItem->MultiNum;
			float fMaxAttack = (i_pMaxItem->AbilityMax)*i_pMaxItem->ShotNum*i_pMaxItem->MultiNum;

			sprintf(m_strItemInfo[nParameterIndex], STRMSG_C_ITEM_0031, fMinAttack);
			sprintf(m_strItemInfo[nParameterIndex], "%s ~ %.0f", m_strItemInfo[nParameterIndex], fMaxAttack);
			*/
		}
		else
		{
			sprintf(m_strItemInfo[nParameterIndex], STRMSG_C_ITEM_0031, CAtumSJ::GetMinAttackPerSecond(m_pRefItemInfo->GetRealItemInfo(), FALSE));//"√ ¥Á ∞¯∞›∑¬ : %.0f"
			sprintf(m_strItemInfo[nParameterIndex], "%s ~ %.0f", m_strItemInfo[nParameterIndex], CAtumSJ::GetMaxAttackPerSecond(m_pRefItemInfo->GetRealItemInfo(), FALSE));
			/*ITEM * i_pRealItem = m_pRefItemInfo->GetRealItemInfo(); //get current item stats (with bonus applied)
			float fMinAttack = (i_pRealItem->AbilityMin)*i_pRealItem->ShotNum*i_pRealItem->MultiNum;
			float fMaxAttack = (i_pRealItem->AbilityMax)*i_pRealItem->ShotNum*i_pRealItem->MultiNum;

			sprintf(m_strItemInfo[nParameterIndex], STRMSG_C_ITEM_0031, fMinAttack);//"√ ¥Á ∞¯∞›∑¬ : %.0f"
			sprintf(m_strItemInfo[nParameterIndex], "%s ~ %.0f", m_strItemInfo[nParameterIndex], fMaxAttack);*/
		}
	}
	else
	{
		ASSERT_ASSERT(m_pRefITEM);
		sprintf(m_strItemInfo[nParameterIndex], STRMSG_C_ITEM_0031, CAtumSJ::GetMinAttackPerSecond(m_pRefITEM,FALSE));
		sprintf(m_strItemInfo[nParameterIndex], "%s ~ %.0f", m_strItemInfo[nParameterIndex], CAtumSJ::GetMaxAttackPerSecond(m_pRefITEM, FALSE));
		/*ASSERT_ASSERT(m_pRefITEM);
		ITEM * i_pRealItem = m_pRefITEM; //get current item stats (with bonus applied)
		float fMinAttack = i_pRealItem->AbilityMin*i_pRealItem->ShotNum*i_pRealItem->MultiNum;
		float fMaxAttack = i_pRealItem->AbilityMax*i_pRealItem->ShotNum*i_pRealItem->MultiNum;

		sprintf(m_strItemInfo[nParameterIndex], STRMSG_C_ITEM_0031, fMinAttack);
		sprintf(m_strItemInfo[nParameterIndex], "%s ~ %.0f", m_strItemInfo[nParameterIndex], fMaxAttack);*/
	}
}
#else
void CINFItemInfo::SetSecAttack(int nParameterIndex)
{
	if(m_pRefItemInfo) {
		float fTempAutomaticAttackTime, fTempNormalAttackTime;
		fTempAutomaticAttackTime = g_pShuttleChild->GetAutomaticAttackTime(m_pRefItemInfo->GetRealItemInfo()->OrbitType);
		fTempNormalAttackTime =  (m_pRefItemInfo->GetRealItemInfo()->ReAttacktime/1000.0f)/m_pRefItemInfo->GetRealItemInfo()->ShotNum;
		auto AttackCheckTime = fTempAutomaticAttackTime < fTempNormalAttackTime ? fTempAutomaticAttackTime : fTempNormalAttackTime;
		auto RealRea = m_pRefItemInfo->GetRealItemInfo()->ReAttacktime/1000.0f + AttackCheckTime*(m_pRefItemInfo->GetRealItemInfo()->ShotNum -1);
		
		sprintf( m_strItemInfo[nParameterIndex], STRMSG_C_ITEM_0031, CAtumSJ::GetMinAttackPerSecond(m_pRefItemInfo->GetRealItemInfo(),RealRea));//"√ ¥Á ∞¯∞›∑¬ : %.0f"
		sprintf( m_strItemInfo[nParameterIndex], "%s ~ %.0f", m_strItemInfo[nParameterIndex], CAtumSJ::GetMaxAttackPerSecond(m_pRefItemInfo->GetRealItemInfo(),RealRea));
	}
	else {
		ASSERT_ASSERT(m_pRefITEM);

		float fTempAutomaticAttackTime, fTempNormalAttackTime;
		fTempAutomaticAttackTime = g_pShuttleChild->GetAutomaticAttackTime(m_pRefITEM->OrbitType);
		fTempNormalAttackTime = (m_pRefITEM->ReAttacktime/1000.0f)/m_pRefITEM->ShotNum;
		auto AttackCheckTime = fTempAutomaticAttackTime < fTempNormalAttackTime ? fTempAutomaticAttackTime : fTempNormalAttackTime;
		auto RealRea = m_pRefITEM->ReAttacktime/1000.0f + AttackCheckTime*(m_pRefITEM->ShotNum -1);
		
		sprintf( m_strItemInfo[nParameterIndex], STRMSG_C_ITEM_0031, CAtumSJ::GetMinAttackPerSecond(m_pRefITEM,RealRea));
		sprintf( m_strItemInfo[nParameterIndex], "%s ~ %.0f", m_strItemInfo[nParameterIndex], CAtumSJ::GetMaxAttackPerSecond(m_pRefITEM,RealRea));
	}
}
#endif
#ifdef BONUS_STAT_ITEM
void CINFItemInfo::SetDefense(int nParameterIndex, BOOL Bonus)
#else
void CINFItemInfo::SetDefense(int nParameterIndex)
#endif
{
#ifdef BONUS_STAT_ITEM
	float nBonusAttackMin = m_pRefBonus ? m_pRefBonus->pfp_DEFENSE_01 + m_pRefBonus->pfp_DEFENSE_02 : 0;
#endif
	float fEnchantDefense = m_pRefEnchant ? m_pRefEnchant->pfp_DEFENSE_01+m_pRefEnchant->pfp_DEFENSE_02 : 0;
	float fRareInfoDefense = 0;
	if(m_pRefPrefixRareInfo)
	{
		for(int i=0; i<SIZE_DES_PARAM_PER_RARE_ITEM_INFO; i++)
		{
			if( m_pRefPrefixRareInfo->DesParameter[i] == DES_DEFENSE_01 || 
				m_pRefPrefixRareInfo->DesParameter[i] == DES_DEFENSE_02)
			{
				fRareInfoDefense += m_pRefPrefixRareInfo->ParameterValue[i];
			}
		}
	}
	if(m_pRefSuffixRareInfo)
	{
		for(int i=0; i<SIZE_DES_PARAM_PER_RARE_ITEM_INFO; i++)
		{
			if( m_pRefSuffixRareInfo->DesParameter[i] == DES_DEFENSE_01 || 
				m_pRefSuffixRareInfo->DesParameter[i] == DES_DEFENSE_02)
			{
				fRareInfoDefense += m_pRefSuffixRareInfo->ParameterValue[i];
			}
		}
	}
#ifdef BONUS_STAT_ITEM
	if (Bonus)
#endif
		sprintf(m_strItemInfo[nParameterIndex], STRMSG_C_ITEM_0032, m_pRefITEM->AbilityMin);//"πÊæÓ : %.0f"
#ifdef BONUS_STAT_ITEM
	else
		sprintf( m_strItemInfo[nParameterIndex], STRMSG_C_ITEM_0032, m_pRefITEM->AbilityMin - nBonusAttackMin);//"πnÊæÓ : %.0f"
#endif
	if(fEnchantDefense > 0)
		sprintf( m_strItemInfo[nParameterIndex], "%s\\e[+%.0f]\\e",m_strItemInfo[nParameterIndex], fEnchantDefense) ;
	else if(fEnchantDefense < 0)
		sprintf( m_strItemInfo[nParameterIndex], "%s\\e[%.0f]\\e",m_strItemInfo[nParameterIndex], fEnchantDefense) ;
	if(fRareInfoDefense > 0)
		sprintf( m_strItemInfo[nParameterIndex], "%s\\g[+%.0f]\\g",m_strItemInfo[nParameterIndex], fRareInfoDefense) ;
	else if(fRareInfoDefense < 0)
		sprintf( m_strItemInfo[nParameterIndex], "%s\\g[%.0f]\\g",m_strItemInfo[nParameterIndex], fRareInfoDefense) ;
}
#ifdef BONUS_STAT_ITEM
void CINFItemInfo::SetRate(int nParameterIndex, BOOL Bonus)
{
	float nBonusHitRate = m_pRefBonus ? m_pRefBonus->pfp_ATTACKPROBABILITY_01 + m_pRefBonus->pfp_ATTACKPROBABILITY_02 : 0;
	float nEnchantHitRate = m_pRefEnchant ? m_pRefEnchant->pfp_ATTACKPROBABILITY_01 + m_pRefEnchant->pfp_ATTACKPROBABILITY_02 : 0;
	float nRareInfoHitRate = 0;
	if (m_pRefPrefixRareInfo)
	{
		for (int i = 0; i<SIZE_DES_PARAM_PER_RARE_ITEM_INFO; i++)
		{
			if (m_pRefPrefixRareInfo->DesParameter[i] == DES_ATTACKPROBABILITY_01 ||
				m_pRefPrefixRareInfo->DesParameter[i] == DES_ATTACKPROBABILITY_02)
			{
				nRareInfoHitRate += m_pRefPrefixRareInfo->ParameterValue[i];
			}
		}
	}
	if (m_pRefSuffixRareInfo)
	{
		for (int i = 0; i<SIZE_DES_PARAM_PER_RARE_ITEM_INFO; i++)
		{
			if (m_pRefSuffixRareInfo->DesParameter[i] == DES_ATTACKPROBABILITY_01 ||
				m_pRefSuffixRareInfo->DesParameter[i] == DES_ATTACKPROBABILITY_02)
			{
				nRareInfoHitRate += m_pRefSuffixRareInfo->ParameterValue[i];
			}
		}
	}
	if (Bonus){
#ifdef BONUS_STAT_INVERTED
		if (m_pRefItemInfo->GetBonusParamFactor())
		{
			float nProbDec1 = (m_pRefItemInfo->GetBonusParamFactor()->pfp_ATTACKPROBABILITY_01 + m_pRefItemInfo->GetBonusParamFactor()->pfp_ATTACKPROBABILITY_02);
			float nProbDec = ((float)m_pRefITEM->HitRate + nProbDec1) - (float)m_pRefItemInfo->GetItemInfo()->HitRate;
			if (nProbDec < 0.0f)
				sprintf(m_strItemInfo[nParameterIndex], "Accuracy : %.2f%% \\r(%.2f%%)\\r", (float)m_pRefITEM->HitRate / PROB100_MAX_VALUE*100.0f, nProbDec/100.0f);
			else if (nProbDec == 0.0f)
				sprintf(m_strItemInfo[nParameterIndex], "Accuracy : %.2f%% \\g*\\g", (float)m_pRefITEM->HitRate / PROB100_MAX_VALUE*100.0f);
			else
				sprintf(m_strItemInfo[nParameterIndex], "Accuracy : %.2f%%", (float)m_pRefITEM->HitRate / PROB100_MAX_VALUE*100.0f);
		}
		else
			sprintf(m_strItemInfo[nParameterIndex], "Accuracy : %.2f%%\\g*\\g", (float)m_pRefITEM->HitRate / PROB100_MAX_VALUE*100.0f);

		//sprintf(m_strItemInfo[nParameterIndex], STRMSG_C_ITEM_0033, (float)m_pRefITEM->HitRate / PROB100_MAX_VALUE*100.0f);
#else
		sprintf(m_strItemInfo[nParameterIndex], STRMSG_C_ITEM_0033, ((float)m_pRefITEM->HitRate + (m_pRefITEM->HitRate * 0.015)) / PROB100_MAX_VALUE *100.0f);
#endif
	}
	else{
#ifdef BONUS_STAT_INVERTED
		sprintf(m_strItemInfo[nParameterIndex], STRMSG_C_ITEM_0033, ((float)m_pRefITEM->HitRate / PROB100_MAX_VALUE*100.0f) + nBonusHitRate/100.0f);
#else
		sprintf(m_strItemInfo[nParameterIndex], STRMSG_C_ITEM_0033, ((float)m_pRefITEM->HitRate / PROB100_MAX_VALUE *100.0f) + nBonusHitRate);
#endif
		if (nEnchantHitRate > 0)
			sprintf(m_strItemInfo[nParameterIndex], "%s\\e[+%.2f%%]\\e", m_strItemInfo[nParameterIndex], (float)nEnchantHitRate / (float)PROB100_MAX_VALUE *100.0f);
		else if (nEnchantHitRate < 0)
			sprintf(m_strItemInfo[nParameterIndex], "%s\\e[%.2f%%]\\e", m_strItemInfo[nParameterIndex], (float)nEnchantHitRate / (float)PROB100_MAX_VALUE *100.0f);
		if (nRareInfoHitRate > 0)
			sprintf(m_strItemInfo[nParameterIndex], "%s\\g[+%.2f%%]\\g", m_strItemInfo[nParameterIndex], (float)nRareInfoHitRate / (float)PROB100_MAX_VALUE *100.0f);
		else if (nRareInfoHitRate < 0)
			sprintf(m_strItemInfo[nParameterIndex], "%s\\g[%.2f%%]\\g", m_strItemInfo[nParameterIndex], (float)nRareInfoHitRate / (float)PROB100_MAX_VALUE *100.0f);
	}
}
#else
void CINFItemInfo::SetRate(int nParameterIndex )
{
    // 2010-07-28 by dgwoo »Æ∑¸ ºˆΩƒ ∫Ø∞Ê (255 => 100%)
	float nEnchantHitRate = m_pRefEnchant ? m_pRefEnchant->pfp_ATTACKPROBABILITY_01+m_pRefEnchant->pfp_ATTACKPROBABILITY_02 : 0;
	float nRareInfoHitRate = 0;
	if(m_pRefPrefixRareInfo)
	{
		for(int i=0; i<SIZE_DES_PARAM_PER_RARE_ITEM_INFO; i++)
		{
			if( m_pRefPrefixRareInfo->DesParameter[i] == DES_ATTACKPROBABILITY_01 || 
				m_pRefPrefixRareInfo->DesParameter[i] == DES_ATTACKPROBABILITY_02)
			{
				// 2010-07-28 by dgwoo »Æ∑¸ ºˆΩƒ ∫Ø∞Ê (255 => 100%)
				//nRareInfoHitRate += (Prob256_t)m_pRefPrefixRareInfo->ParameterValue[i];
				nRareInfoHitRate += m_pRefPrefixRareInfo->ParameterValue[i];
			}
		}
	}
	if(m_pRefSuffixRareInfo)
	{
		for(int i=0; i<SIZE_DES_PARAM_PER_RARE_ITEM_INFO; i++)
		{
			if( m_pRefSuffixRareInfo->DesParameter[i] == DES_ATTACKPROBABILITY_01 || 
				m_pRefSuffixRareInfo->DesParameter[i] == DES_ATTACKPROBABILITY_02)
			{
				// 2010-07-28 by dgwoo »Æ∑¸ ºˆΩƒ ∫Ø∞Ê (255 => 100%)
				//nRareInfoHitRate += (Prob256_t)m_pRefSuffixRareInfo->ParameterValue[i];
				nRareInfoHitRate += m_pRefSuffixRareInfo->ParameterValue[i];
			}
		}
	}
	// 2010-07-28 by dgwoo »Æ∑¸ ºˆΩƒ ∫Ø∞Ê (255 => 100%)
	//sprintf( m_strItemInfo[nParameterIndex], STRMSG_C_ITEM_0033, (float)m_pRefITEM->HitRate / (float)PROB256_MAX_VALUE *100.0f) ;//"»Æ∑¸ : %.2f%%"
	sprintf( m_strItemInfo[nParameterIndex], STRMSG_C_ITEM_0033, (float)m_pRefITEM->HitRate /PROB100_MAX_VALUE *100.0f) ;//"»Æ∑¸ : %.2f%%"
	if(nEnchantHitRate > 0)
		//sprintf( m_strItemInfo[nParameterIndex], "%s\\e[+%.2f%%]\\e",m_strItemInfo[nParameterIndex], (float)nEnchantHitRate / (float)PROB256_MAX_VALUE *100.0f) ;
		sprintf( m_strItemInfo[nParameterIndex], "%s\\e[+%.2f%%]\\e",m_strItemInfo[nParameterIndex], (float)nEnchantHitRate / (float)PROB100_MAX_VALUE *100.0f) ;
	else if(nEnchantHitRate < 0)
		//sprintf( m_strItemInfo[nParameterIndex], "%s\\e[%.2f%%]\\e",m_strItemInfo[nParameterIndex], (float)nEnchantHitRate / (float)PROB256_MAX_VALUE *100.0f) ;
		sprintf( m_strItemInfo[nParameterIndex], "%s\\e[%.2f%%]\\e",m_strItemInfo[nParameterIndex], (float)nEnchantHitRate / (float)PROB100_MAX_VALUE *100.0f) ;
	if(nRareInfoHitRate > 0)
		//sprintf( m_strItemInfo[nParameterIndex], "%s\\g[+%.2f%%]\\g",m_strItemInfo[nParameterIndex], (float)nRareInfoHitRate / (float)PROB256_MAX_VALUE *100.0f) ;
		sprintf( m_strItemInfo[nParameterIndex], "%s\\g[+%.2f%%]\\g",m_strItemInfo[nParameterIndex], (float)nRareInfoHitRate / (float)PROB100_MAX_VALUE *100.0f) ;
	else if(nRareInfoHitRate < 0)
		//sprintf( m_strItemInfo[nParameterIndex], "%s\\g[%.2f%%]\\g",m_strItemInfo[nParameterIndex], (float)nRareInfoHitRate / (float)PROB256_MAX_VALUE *100.0f) ;
		sprintf( m_strItemInfo[nParameterIndex], "%s\\g[%.2f%%]\\g",m_strItemInfo[nParameterIndex], (float)nRareInfoHitRate / (float)PROB100_MAX_VALUE *100.0f) ;
	// end 2010-07-28 by dgwoo »Æ∑¸ ºˆΩƒ ∫Ø∞Ê (255 => 100%)
}
#endif
#ifdef BONUS_STAT_ITEM
void CINFItemInfo::SetRange(int nParameterIndex, BOOL Bonus)
{
	float BonusRange = m_pRefBonus ? m_pRefBonus->pfm_RANGE_01 + m_pRefBonus->pfm_RANGE_02 : 0;
	float fEnchantRange = m_pRefEnchant ? m_pRefEnchant->pfm_RANGE_01 + m_pRefEnchant->pfm_RANGE_02 : 0;
	float fRareInfoRange = 0;
	if (m_pRefPrefixRareInfo)
	{
		for (int i = 0; i<SIZE_DES_PARAM_PER_RARE_ITEM_INFO; i++)
		{
			if (m_pRefPrefixRareInfo->DesParameter[i] == DES_RANGE_01 ||
				m_pRefPrefixRareInfo->DesParameter[i] == DES_RANGE_02)
			{
				fRareInfoRange += m_pRefPrefixRareInfo->ParameterValue[i];
			}
		}
	}
	if (m_pRefSuffixRareInfo)
	{
		for (int i = 0; i<SIZE_DES_PARAM_PER_RARE_ITEM_INFO; i++)
		{
			if (m_pRefSuffixRareInfo->DesParameter[i] == DES_RANGE_01 ||
				m_pRefSuffixRareInfo->DesParameter[i] == DES_RANGE_02)
			{
				fRareInfoRange += m_pRefSuffixRareInfo->ParameterValue[i];
			}
		}
	}
	if (Bonus){
#ifdef BONUS_STAT_INVERTED
		float nRanDec = m_pRefItemInfo->GetRealItemInfo()->Range - m_pRefItemInfo->GetItemInfo()->Range;
		if (nRanDec < 0.0f)
			sprintf(m_strItemInfo[nParameterIndex], "Range : %dm \\r(%.0fm)\\r", m_pRefITEM->Range, nRanDec);
		else if (nRanDec == 0.0f)
			sprintf(m_strItemInfo[nParameterIndex], "Range : %dm \\g*\\g", m_pRefITEM->Range);
		else
			sprintf(m_strItemInfo[nParameterIndex], "Range : %dm", m_pRefITEM->Range);

		//sprintf(m_strItemInfo[nParameterIndex], STRMSG_C_ITEM_0034, m_pRefITEM->Range);
#else
		sprintf(m_strItemInfo[nParameterIndex], STRMSG_C_ITEM_0034, (USHORT)((float)m_pRefITEM->Range * 0.08) + m_pRefITEM->Range);
#endif
	}
	else
	{
#ifdef BONUS_STAT_INVERTED
		sprintf(m_strItemInfo[nParameterIndex], STRMSG_C_ITEM_0034, (m_pRefITEM->Range - (USHORT)BonusRange));
#else
		sprintf(m_strItemInfo[nParameterIndex], STRMSG_C_ITEM_0034, (m_pRefITEM->Range + (USHORT)BonusRange));
#endif
		if (fEnchantRange > 0)
			sprintf(m_strItemInfo[nParameterIndex], "%s\\e[+%2.0f%%]\\e", m_strItemInfo[nParameterIndex], fEnchantRange * 100);
		else if (fEnchantRange < 0)
			sprintf(m_strItemInfo[nParameterIndex], "%s\\e[%2.0f%%]\\e", m_strItemInfo[nParameterIndex], fEnchantRange * 100);
		if (fRareInfoRange > 0)
			sprintf(m_strItemInfo[nParameterIndex], "%s\\g[+%2.0f%%]\\g", m_strItemInfo[nParameterIndex], fRareInfoRange * 100);
		else if (fRareInfoRange < 0)
			sprintf(m_strItemInfo[nParameterIndex], "%s\\g[%2.0f%%]\\g", m_strItemInfo[nParameterIndex], fRareInfoRange * 100);
	}
}
#else
void CINFItemInfo::SetRange( int nParameterIndex )
{
	float fEnchantRange = m_pRefEnchant ? m_pRefEnchant->pfm_RANGE_01+m_pRefEnchant->pfm_RANGE_02 : 0;
	float fRareInfoRange = 0;
	if(m_pRefPrefixRareInfo)
	{
		for(int i=0; i<SIZE_DES_PARAM_PER_RARE_ITEM_INFO; i++)
		{
			if( m_pRefPrefixRareInfo->DesParameter[i] == DES_RANGE_01 || 
				m_pRefPrefixRareInfo->DesParameter[i] == DES_RANGE_02)
			{
				fRareInfoRange += m_pRefPrefixRareInfo->ParameterValue[i];
			}
		}
	}
	if(m_pRefSuffixRareInfo)
	{
		for(int i=0; i<SIZE_DES_PARAM_PER_RARE_ITEM_INFO; i++)
		{
			if( m_pRefSuffixRareInfo->DesParameter[i] == DES_RANGE_01 || 
				m_pRefSuffixRareInfo->DesParameter[i] == DES_RANGE_02)
			{
				fRareInfoRange += m_pRefSuffixRareInfo->ParameterValue[i];
			}
		}
	}
	sprintf( m_strItemInfo[nParameterIndex], STRMSG_C_ITEM_0034, m_pRefITEM->Range) ;//"∞≈∏Æ : %dm"
	if(fEnchantRange > 0)
		sprintf( m_strItemInfo[nParameterIndex], "%s\\e[+%2.0f%%]\\e",m_strItemInfo[nParameterIndex], fEnchantRange*100) ;
	else if(fEnchantRange < 0)
		sprintf( m_strItemInfo[nParameterIndex], "%s\\e[%2.0f%%]\\e",m_strItemInfo[nParameterIndex], fEnchantRange*100) ;
	if(fRareInfoRange > 0)
		sprintf( m_strItemInfo[nParameterIndex], "%s\\g[+%2.0f%%]\\g",m_strItemInfo[nParameterIndex], fRareInfoRange*100) ;
	else if(fRareInfoRange < 0)
		sprintf( m_strItemInfo[nParameterIndex], "%s\\g[%2.0f%%]\\g",m_strItemInfo[nParameterIndex], fRareInfoRange*100) ;

}
#endif
#ifdef BONUS_STAT_ITEM
void CINFItemInfo::SetReAttackTime(int nParameterIndex, BOOL Bonus)
{
	float BonusReAttacktime = m_pRefBonus ? m_pRefBonus->pfm_REATTACKTIME_01 + m_pRefBonus->pfm_REATTACKTIME_02 : 0;
	float fEnchantReAttacktime = m_pRefEnchant ? m_pRefEnchant->pfm_REATTACKTIME_01 + m_pRefEnchant->pfm_REATTACKTIME_02 : 0;
	float fRareInfoReAttacktime = 0;
	if (m_pRefPrefixRareInfo)
	{
		for (int i = 0; i<SIZE_DES_PARAM_PER_RARE_ITEM_INFO; i++)
		{
			if (m_pRefPrefixRareInfo->DesParameter[i] == DES_REATTACKTIME_01 ||
				m_pRefPrefixRareInfo->DesParameter[i] == DES_REATTACKTIME_02)
			{
				fRareInfoReAttacktime += m_pRefPrefixRareInfo->ParameterValue[i];
			}
		}
	}
	if (m_pRefSuffixRareInfo)
	{
		for (int i = 0; i<SIZE_DES_PARAM_PER_RARE_ITEM_INFO; i++)
		{
			if (m_pRefSuffixRareInfo->DesParameter[i] == DES_REATTACKTIME_01 ||
				m_pRefSuffixRareInfo->DesParameter[i] == DES_REATTACKTIME_02)
			{
				fRareInfoReAttacktime += m_pRefSuffixRareInfo->ParameterValue[i];
			}
		}
	}
	if (Bonus)
	{
		
		if (m_pRefItemInfo->GetBonusParamFactor())
		{
			float fReaDec1 = (m_pRefItemInfo->GetBonusParamFactor()->pfm_REATTACKTIME_01 + m_pRefItemInfo->GetBonusParamFactor()->pfm_REATTACKTIME_02);
			float fReaDec = CalcPercentDecreased((float)m_pRefITEM->ReAttacktime + fReaDec1, (float)m_pRefItemInfo->GetItemInfo()->ReAttacktime);
			if (fReaDec > 0.0f)
				sprintf(m_strItemInfo[nParameterIndex], "Reattack time : %.2fs \\r(+%.2f%%)\\r", (float)m_pRefITEM->ReAttacktime / 1000.0f, fReaDec);
			else if (fReaDec == 0.0f)
				sprintf(m_strItemInfo[nParameterIndex], "Reattack time : %.2fs \\g*\\g", (float)m_pRefITEM->ReAttacktime / 1000.0f);
			else
				sprintf(m_strItemInfo[nParameterIndex], "Reattack time : %.2fs", (float)m_pRefITEM->ReAttacktime / 1000.0f);
		}
		else
		{
			sprintf(m_strItemInfo[nParameterIndex], "Reattack time : %.2fs \\g*\\g", (float)m_pRefITEM->ReAttacktime / 1000.0f);
		}
			

		//sprintf(m_strItemInfo[nParameterIndex], STRMSG_C_ITEM_0035, (float)m_pRefITEM->ReAttacktime / 1000.0f);
	}
	else
	{
		sprintf(m_strItemInfo[nParameterIndex], STRMSG_C_ITEM_0035, ((float)m_pRefITEM->ReAttacktime + BonusReAttacktime) / 1000.0f);
	}
	if (fEnchantReAttacktime > 0)
		sprintf(m_strItemInfo[nParameterIndex], "%s\\e[+%2.2f%%]\\e", m_strItemInfo[nParameterIndex], fEnchantReAttacktime * 100);
	else if (fEnchantReAttacktime < 0)
		sprintf(m_strItemInfo[nParameterIndex], "%s\\e[%2.2f%%]\\e", m_strItemInfo[nParameterIndex], fEnchantReAttacktime * 100);
	if (fRareInfoReAttacktime > 0)
		sprintf(m_strItemInfo[nParameterIndex], "%s\\g[+%2.2f%%]\\g", m_strItemInfo[nParameterIndex], fRareInfoReAttacktime * 100);
	else if (fRareInfoReAttacktime < 0)
		sprintf(m_strItemInfo[nParameterIndex], "%s\\g[%2.2f%%]\\g", m_strItemInfo[nParameterIndex], fRareInfoReAttacktime * 100);

}
#else
void CINFItemInfo::SetReAttackTime( int nParameterIndex )
{
	float fEnchantReAttacktime = m_pRefEnchant ? m_pRefEnchant->pfm_REATTACKTIME_01+m_pRefEnchant->pfm_REATTACKTIME_02 : 0;
	float fRareInfoReAttacktime = 0;
	if(m_pRefPrefixRareInfo)
	{
		for(int i=0; i<SIZE_DES_PARAM_PER_RARE_ITEM_INFO; i++)
		{
			if( m_pRefPrefixRareInfo->DesParameter[i] == DES_REATTACKTIME_01 || 
				m_pRefPrefixRareInfo->DesParameter[i] == DES_REATTACKTIME_02)
			{
				fRareInfoReAttacktime += m_pRefPrefixRareInfo->ParameterValue[i];
			}
		}
	}
	if(m_pRefSuffixRareInfo)
	{
		for(int i=0; i<SIZE_DES_PARAM_PER_RARE_ITEM_INFO; i++)
		{
			if( m_pRefSuffixRareInfo->DesParameter[i] == DES_REATTACKTIME_01 || 
				m_pRefSuffixRareInfo->DesParameter[i] == DES_REATTACKTIME_02)
			{
				fRareInfoReAttacktime += m_pRefSuffixRareInfo->ParameterValue[i];
			}
		}
	}
	sprintf( m_strItemInfo[nParameterIndex], STRMSG_C_ITEM_0035, (float)m_pRefITEM->ReAttacktime/1000.0f) ;//"¿Á∞¯∞›Ω√∞£ : %.2fsec"
	if(fEnchantReAttacktime > 0)
		sprintf( m_strItemInfo[nParameterIndex], "%s\\e[+%2.2f%%]\\e",m_strItemInfo[nParameterIndex], fEnchantReAttacktime*100) ;
	else if(fEnchantReAttacktime < 0)
		sprintf( m_strItemInfo[nParameterIndex], "%s\\e[%2.2f%%]\\e",m_strItemInfo[nParameterIndex], fEnchantReAttacktime*100) ;
	if(fRareInfoReAttacktime > 0)
		sprintf( m_strItemInfo[nParameterIndex], "%s\\g[+%2.2f%%]\\g",m_strItemInfo[nParameterIndex], fRareInfoReAttacktime*100) ;
	else if(fRareInfoReAttacktime < 0)
		sprintf( m_strItemInfo[nParameterIndex], "%s\\g[%2.2f%%]\\g",m_strItemInfo[nParameterIndex], fRareInfoReAttacktime*100) ;

}
#endif
void CINFItemInfo::SetOverHeatTime( int nParameterIndex )
{
	float fEnchantOverheatTime = m_pRefEnchant ? m_pRefEnchant->pfm_TIME_01+m_pRefEnchant->pfm_TIME_02 : 0;
	float fRareInfoOverheatTime = 0;
	if(m_pRefPrefixRareInfo)
	{
		for(int i=0; i<SIZE_DES_PARAM_PER_RARE_ITEM_INFO; i++)
		{
			if( m_pRefPrefixRareInfo->DesParameter[i] == DES_TIME_01 || 
				m_pRefPrefixRareInfo->DesParameter[i] == DES_TIME_02)
			{
				fRareInfoOverheatTime += m_pRefPrefixRareInfo->ParameterValue[i];
			}
		}
	}
	if(m_pRefSuffixRareInfo)
	{
		for(int i=0; i<SIZE_DES_PARAM_PER_RARE_ITEM_INFO; i++)
		{
			if( m_pRefSuffixRareInfo->DesParameter[i] == DES_TIME_01 || 
				m_pRefSuffixRareInfo->DesParameter[i] == DES_TIME_02)
			{
				fRareInfoOverheatTime += m_pRefSuffixRareInfo->ParameterValue[i];
			}
		}
	}
	sprintf( m_strItemInfo[nParameterIndex], STRMSG_C_ITEM_0036, (float)m_pRefITEM->Time/1000.0f) ;//"∞˙ø≠Ω√∞£ : %.2fsec"
	if(fEnchantOverheatTime > 0)
		sprintf( m_strItemInfo[nParameterIndex], "%s\\e[+%2.2f%%]\\e",m_strItemInfo[nParameterIndex], fEnchantOverheatTime*100) ;
	else if(fEnchantOverheatTime < 0)
		sprintf( m_strItemInfo[nParameterIndex], "%s\\e[%2.2f%%]\\e",m_strItemInfo[nParameterIndex], fEnchantOverheatTime*100) ;
	if(fRareInfoOverheatTime > 0)
		sprintf( m_strItemInfo[nParameterIndex], "%s\\g[+%2.2f%%]\\g",m_strItemInfo[nParameterIndex], fRareInfoOverheatTime*100) ;
	else if(fRareInfoOverheatTime < 0)
		sprintf( m_strItemInfo[nParameterIndex], "%s\\g[%2.2f%%]\\g",m_strItemInfo[nParameterIndex], fRareInfoOverheatTime*100) ;

}

void CINFItemInfo::SetRangeAngle( int nParameterIndex )
{
	float fEnchantRangeAngle = m_pRefEnchant ? m_pRefEnchant->pfp_RANGEANGLE_01+m_pRefEnchant->pfp_RANGEANGLE_02 : 0;
	float fRareInfoRangeAngle = 0;
	if(m_pRefPrefixRareInfo)
	{
		for(int i=0; i<SIZE_DES_PARAM_PER_RARE_ITEM_INFO; i++)
		{
			if( m_pRefPrefixRareInfo->DesParameter[i] == DES_RANGEANGLE_01 || 
				m_pRefPrefixRareInfo->DesParameter[i] == DES_RANGEANGLE_02)
			{
				fRareInfoRangeAngle += m_pRefPrefixRareInfo->ParameterValue[i];
			}
		}
	}
	if(m_pRefSuffixRareInfo)
	{
		for(int i=0; i<SIZE_DES_PARAM_PER_RARE_ITEM_INFO; i++)
		{
			if( m_pRefSuffixRareInfo->DesParameter[i] == DES_RANGEANGLE_01 || 
				m_pRefSuffixRareInfo->DesParameter[i] == DES_RANGEANGLE_02)
			{
				fRareInfoRangeAngle += m_pRefSuffixRareInfo->ParameterValue[i];
			}
		}
	}
	sprintf( m_strItemInfo[nParameterIndex], STRMSG_C_ITEM_0037, (int)(m_pRefITEM->RangeAngle/PI*180)) ;//"¿Ø»ø∞¢µµ : %dµµ"
	if(fEnchantRangeAngle > 0)
		sprintf( m_strItemInfo[nParameterIndex], STRMSG_C_ITEM_0038,m_strItemInfo[nParameterIndex], fEnchantRangeAngle/PI*180) ;//"%s\\e[+%.1fµµ]\\e"
	else if(fEnchantRangeAngle < 0)
		sprintf( m_strItemInfo[nParameterIndex], STRMSG_C_ITEM_0039,m_strItemInfo[nParameterIndex], fEnchantRangeAngle/PI*180) ;//"%s\\e[%.1fµµ]\\e"
	if(fRareInfoRangeAngle > 0)
		sprintf( m_strItemInfo[nParameterIndex], STRMSG_C_ITEM_0040,m_strItemInfo[nParameterIndex], fRareInfoRangeAngle/PI*180) ;//"%s\\g[+%.1fµµ]\\g"
	else if(fRareInfoRangeAngle < 0)
		sprintf( m_strItemInfo[nParameterIndex], STRMSG_C_ITEM_0041,m_strItemInfo[nParameterIndex], fRareInfoRangeAngle/PI*180) ;//"%s\\g[%.1fµµ]\\g"


}

void CINFItemInfo::SetExplosionRange( int nParameterIndex )
{
	float fEnchantExplosionRange = m_pRefEnchant ? m_pRefEnchant->pfp_EXPLOSIONRANGE_01+m_pRefEnchant->pfp_EXPLOSIONRANGE_02 : 0;
	float fRareInfoExplosionRange = 0;
	if(m_pRefPrefixRareInfo)
	{
		for(int i=0; i<SIZE_DES_PARAM_PER_RARE_ITEM_INFO; i++)
		{
			if( m_pRefPrefixRareInfo->DesParameter[i] == DES_EXPLOSIONRANGE_01 || 
				m_pRefPrefixRareInfo->DesParameter[i] == DES_EXPLOSIONRANGE_02)
			{
				fRareInfoExplosionRange += m_pRefPrefixRareInfo->ParameterValue[i];
			}
		}
	}
	if(m_pRefSuffixRareInfo)
	{
		for(int i=0; i<SIZE_DES_PARAM_PER_RARE_ITEM_INFO; i++)
		{
			if( m_pRefSuffixRareInfo->DesParameter[i] == DES_EXPLOSIONRANGE_01 || 
				m_pRefSuffixRareInfo->DesParameter[i] == DES_EXPLOSIONRANGE_02)
			{
				fRareInfoExplosionRange += m_pRefSuffixRareInfo->ParameterValue[i];
			}
		}
	}
	sprintf( m_strItemInfo[nParameterIndex], STRMSG_C_ITEM_0042, m_pRefITEM->ExplosionRange) ;//"∆¯πﬂπ›∞Ê : %dm"
	if(fEnchantExplosionRange > 0)
		sprintf( m_strItemInfo[nParameterIndex], "%s\\e[+%dm]\\e",m_strItemInfo[nParameterIndex], (int)fEnchantExplosionRange) ;
	else if(fEnchantExplosionRange < 0)
		sprintf( m_strItemInfo[nParameterIndex], "%s\\e[%dm]\\e",m_strItemInfo[nParameterIndex], (int)fEnchantExplosionRange) ;
	if(fRareInfoExplosionRange > 0)
		sprintf( m_strItemInfo[nParameterIndex], "%s\\g[+%dm]\\g",m_strItemInfo[nParameterIndex], (int)fRareInfoExplosionRange) ;
	else if(fRareInfoExplosionRange < 0)
		sprintf( m_strItemInfo[nParameterIndex], "%s\\g[%dm]\\g",m_strItemInfo[nParameterIndex], (int)fRareInfoExplosionRange) ;
}

void CINFItemInfo::SetReactionRange( int nParameterIndex )
{
	float fEnchantReactionRange = m_pRefEnchant ? m_pRefEnchant->pfp_REACTION_RANGE : 0;
	float fRareInfoReactionRange = 0;
	if(m_pRefPrefixRareInfo)
	{
		for(int i=0; i<SIZE_DES_PARAM_PER_RARE_ITEM_INFO; i++)
		{
			if( m_pRefPrefixRareInfo->DesParameter[i] == DES_REACTION_RANGE )
			{
				fRareInfoReactionRange += m_pRefPrefixRareInfo->ParameterValue[i];
			}
		}
	}
	if(m_pRefSuffixRareInfo)
	{
		for(int i=0; i<SIZE_DES_PARAM_PER_RARE_ITEM_INFO; i++)
		{
			if( m_pRefSuffixRareInfo->DesParameter[i] == DES_REACTION_RANGE )
			{
				fRareInfoReactionRange += m_pRefSuffixRareInfo->ParameterValue[i];
			}
		}
	}
	sprintf( m_strItemInfo[nParameterIndex], STRMSG_C_ITEM_0043, m_pRefITEM->ExplosionRange) ;//"π›¿¿π›∞Ê : %dm"
	if(fEnchantReactionRange > 0)
		sprintf( m_strItemInfo[nParameterIndex], "%s\\e[+%dm]\\e",m_strItemInfo[nParameterIndex], (int)fEnchantReactionRange) ;
	else if(fEnchantReactionRange < 0)
		sprintf( m_strItemInfo[nParameterIndex], "%s\\e[%dm]\\e",m_strItemInfo[nParameterIndex], (int)fEnchantReactionRange) ;
	if(fRareInfoReactionRange > 0)
		sprintf( m_strItemInfo[nParameterIndex], "%s\\g[+%dm]\\g",m_strItemInfo[nParameterIndex], (int)fRareInfoReactionRange) ;
	else if(fRareInfoReactionRange < 0)
		sprintf( m_strItemInfo[nParameterIndex], "%s\\g[%dm]\\g",m_strItemInfo[nParameterIndex], (int)fRareInfoReactionRange) ;
}

void CINFItemInfo::SetSkillLevel( int nParameterIndex )
{
	wsprintf( m_strItemInfo[nParameterIndex], STRMSG_C_SKILL_0005, SKILL_LEVEL(m_pRefITEM->ItemNum)) ;//"Ω∫≈≥∑π∫ß : [%d]"
}


void CINFItemInfo::SetSkillTime( int nParameterIndex)
{
	float fEnchantTime = m_pRefEnchant ? m_pRefEnchant->pfm_TIME_01+m_pRefEnchant->pfm_TIME_02 : 0;
	float fRareInfoTime = 0;
	if(m_pRefPrefixRareInfo)
	{
		for(int i=0; i<SIZE_DES_PARAM_PER_RARE_ITEM_INFO; i++)
		{
			if( m_pRefPrefixRareInfo->DesParameter[i] == DES_TIME_01 || 
				m_pRefPrefixRareInfo->DesParameter[i] == DES_TIME_02)
			{
				fRareInfoTime += m_pRefPrefixRareInfo->ParameterValue[i];
			}
		}
	}
	if(m_pRefSuffixRareInfo)
	{
		for(int i=0; i<SIZE_DES_PARAM_PER_RARE_ITEM_INFO; i++)
		{
			if( m_pRefSuffixRareInfo->DesParameter[i] == DES_TIME_01 || 
				m_pRefSuffixRareInfo->DesParameter[i] == DES_TIME_02)
			{
				fRareInfoTime += m_pRefSuffixRareInfo->ParameterValue[i];
			}
		}
	}
//	sprintf( m_strItemInfo[nParameterIndex], STRMSG_C_SKILL_0006, (float)m_pRefITEM->Time/1000.0f) ;//"Ω√∞£ : %.2fsec"
// 2005-11-22 by ispark Ω√∞£ -> πﬂµø Ω√∞£, ¿Áπﬂµø Ω√∞£ : √ﬂ∞°
	sprintf( m_strItemInfo[nParameterIndex], STRMSG_C_051122_0006, (float)m_pRefITEM->Time/1000.0f) ;//"πﬂµø Ω√∞£ : %.2fsec"
	if(fEnchantTime > 0)
		sprintf( m_strItemInfo[nParameterIndex], "%s\\e[+%2.2f%%]\\e",m_strItemInfo[nParameterIndex], fEnchantTime*100) ;
	else if(fEnchantTime < 0)
		sprintf( m_strItemInfo[nParameterIndex], "%s\\e[%2.2f%%]\\e",m_strItemInfo[nParameterIndex], fEnchantTime*100) ;
	if(fRareInfoTime > 0)
		sprintf( m_strItemInfo[nParameterIndex], "%s\\g[+%2.2f%%]\\g",m_strItemInfo[nParameterIndex], fRareInfoTime*100) ;
	else if(fRareInfoTime < 0)
		sprintf( m_strItemInfo[nParameterIndex], "%s\\g[%2.2f%%]\\g",m_strItemInfo[nParameterIndex], fRareInfoTime*100) ;
}

void CINFItemInfo::SetAttackTime( int nParameterIndex)
{
	UINT nEnchantAttacktime = m_pRefEnchant ? m_pRefEnchant->pfp_ATTACKTIME_01+m_pRefEnchant->pfp_ATTACKTIME_02 : 0;
	UINT nRareInfoAttacktime = 0;
	if(m_pRefPrefixRareInfo)
	{
		for(int i=0; i<SIZE_DES_PARAM_PER_RARE_ITEM_INFO; i++)
		{
			if( m_pRefPrefixRareInfo->DesParameter[i] == DES_ATTACKTIME_01 || 
				m_pRefPrefixRareInfo->DesParameter[i] == DES_ATTACKTIME_02)
			{
				nRareInfoAttacktime += (UINT)m_pRefPrefixRareInfo->ParameterValue[i];
			}
		}
	}
	if(m_pRefSuffixRareInfo)
	{
		for(int i=0; i<SIZE_DES_PARAM_PER_RARE_ITEM_INFO; i++)
		{
			if( m_pRefSuffixRareInfo->DesParameter[i] == DES_ATTACKTIME_01 || 
				m_pRefSuffixRareInfo->DesParameter[i] == DES_ATTACKTIME_02)
			{
				nRareInfoAttacktime += (UINT)m_pRefSuffixRareInfo->ParameterValue[i];
			}
		}
	}
	sprintf( m_strItemInfo[nParameterIndex], STRMSG_C_ITEM_0044, (float)m_pRefITEM->AttackTime/1000.0f) ;//"πﬂµø : %.2fsec"
	if(nEnchantAttacktime > 0)
		sprintf( m_strItemInfo[nParameterIndex], "%s\\e[+%.2fsec]\\e",m_strItemInfo[nParameterIndex], (float)nEnchantAttacktime/1000.0f) ;
	else if(nEnchantAttacktime < 0)
		sprintf( m_strItemInfo[nParameterIndex], "%s\\e[%.2fsec]\\e",m_strItemInfo[nParameterIndex], (float)nEnchantAttacktime/1000.0f) ;
	if(nRareInfoAttacktime > 0)
		sprintf( m_strItemInfo[nParameterIndex], "%s\\g[+%.2fsec]\\g",m_strItemInfo[nParameterIndex], (float)nRareInfoAttacktime/1000.0f) ;
	else if(nRareInfoAttacktime < 0)
		sprintf( m_strItemInfo[nParameterIndex], "%s\\g[%.2fsec]\\g",m_strItemInfo[nParameterIndex], (float)nRareInfoAttacktime/1000.0f) ;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			SetWeight( int nParameterIndex)
/// \brief		
/// \author		dhkwon
/// \date		2004-06-14 ~ 2004-06-14
/// \warning	Enchant ¡§∫∏ , RareInfo ¡§∫∏ ¿÷¿Ω
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////

void CINFItemInfo::SetWeight( int nParameterIndex)
{
	//USHORT nEnchantWeight = m_pRefEnchant ? m_pRefEnchant->pfm_WEIGHT : 0;
	float fEnchantWeight = m_pRefEnchant ? m_pRefEnchant->pfm_WEIGHT_01+m_pRefEnchant->pfm_WEIGHT_02 : 0;
	float fRareInfoWeight = 0;
	if(m_pRefPrefixRareInfo)
	{
		for(int i=0; i<SIZE_DES_PARAM_PER_RARE_ITEM_INFO; i++)
		{
			if( m_pRefPrefixRareInfo->DesParameter[i] == DES_WEIGHT_01 || 
				m_pRefPrefixRareInfo->DesParameter[i] == DES_WEIGHT_02)
			{
				fRareInfoWeight += m_pRefPrefixRareInfo->ParameterValue[i];
			}
		}
	}
	if(m_pRefSuffixRareInfo)
	{
		for(int i=0; i<SIZE_DES_PARAM_PER_RARE_ITEM_INFO; i++)
		{
			if( m_pRefSuffixRareInfo->DesParameter[i] == DES_WEIGHT_01 || 
				m_pRefSuffixRareInfo->DesParameter[i] == DES_WEIGHT_02)
			{
				fRareInfoWeight += m_pRefSuffixRareInfo->ParameterValue[i];
			}
		}
	}
	sprintf( m_strItemInfo[nParameterIndex], STRMSG_C_ITEM_0045, m_pRefITEM->Weight) ;//"¡ﬂ∑Æ : %d"
	if(fEnchantWeight > 0)
		sprintf( m_strItemInfo[nParameterIndex], "%s\\e[+%2.0f%%]\\e",m_strItemInfo[nParameterIndex], fEnchantWeight*100) ;
	else if(fEnchantWeight < 0)
		sprintf( m_strItemInfo[nParameterIndex], "%s\\e[%2.0f%%]\\e",m_strItemInfo[nParameterIndex], fEnchantWeight*100) ;
	if(fRareInfoWeight > 0)
		sprintf( m_strItemInfo[nParameterIndex], "%s\\g[+%2.0f%%]\\g",m_strItemInfo[nParameterIndex], fRareInfoWeight*100) ;
	else if(fRareInfoWeight < 0)
		sprintf( m_strItemInfo[nParameterIndex], "%s\\g[%2.0f%%]\\g",m_strItemInfo[nParameterIndex], fRareInfoWeight*100) ;

}

// 2009. 08. 27 by ckPark ±◊∑°«» ∏Æº“Ω∫ ∫Ø∞Ê Ω√Ω∫≈€ ±∏«ˆ
void CINFItemInfo::SetShapeInfo( int nParameterIndex )
{
	char buf[256];
	ZERO_MEMORY(buf);

	if( !m_pRefItemInfo->ShapeItemNum )
		strcpy( buf, STRMSG_C_090901_0302 );	// "±‚∫ªø‹«¸"
	else
	{
		ITEM* pITEM = g_pDatabase->GetServerItemInfo( m_pRefItemInfo->ShapeItemNum );
		if( !pITEM )
			strcpy( buf, STRMSG_C_090901_0302 );// "±‚∫ªø‹«¸"
		else
			strcpy( buf, pITEM->ItemName );
	}
	
	wsprintf( m_strItemInfo[nParameterIndex], STRMSG_C_090901_0301, buf);	// "ø‹«¸ : %s"
}

void CINFItemInfo::SetEffectInfo( int nParameterIndex )
{
	char buf[256];
	ZERO_MEMORY(buf);

	if( !m_pRefItemInfo->ColorCode )
		strcpy( buf, STRMSG_C_090901_0304 );	// "±‚∫ª¿Ã∆Â∆Æ"
	else
	{
		ITEM* pITEM = g_pDatabase->GetServerItemInfo( m_pRefItemInfo->ColorCode );
		if( !pITEM )
			strcpy( buf, STRMSG_C_090901_0304 );// "±‚∫ª¿Ã∆Â∆Æ"
		else
			strcpy( buf, pITEM->ItemName );
	}
	
	wsprintf( m_strItemInfo[nParameterIndex], STRMSG_C_090901_0303, buf);	// "¿Ã∆Â∆Æ : %s"
}
// end 2009. 08. 27 by ckPark ±◊∑°«» ∏Æº“Ω∫ ∫Ø∞Ê Ω√Ω∫≈€ ±∏«ˆ


// 2010. 02. 11 by ckPark πﬂµø∑˘ ¿Â¬¯æ∆¿Ã≈€
void CINFItemInfo::SetInvokeDestParam( int* pParameterIndex )
{
	// πﬂµø∑˘ µ•Ω∫∆ƒ∂˜ ≈¯∆¡ √ﬂ∞°
	if( m_pRefITEM && m_pRefITEM->InvokingDestParamID )
	{
		// µ•Ω∫∆ƒ∂˜ ∏Ò∑œ æÚæÓø¿±‚
		CVectorInvokingWearItemDP dpList;
		g_pDatabase->GetInvokingWearItemDPList( &dpList, m_pRefITEM->InvokingDestParamID );

		if( !dpList.empty() )
		{
			CVectorInvokingWearItemDPIt it = dpList.begin();
			while( it != dpList.end() )
			{
				// ±‚¥… ≈¯∆¡ √ﬂ∞°
				SetFunction( (*pParameterIndex)++, (*it)->InvokingDestParam, (*it)->InvokingDestParamValue, 0, 0, FUNCTIONTYPE_EQUIP );
				++it;
			}
		}
	}

	// ªÁøÎ∑˘ µ•Ω∫∆ƒ∂˜ ≈¯∆¡ √ﬂ∞°
	if( m_pRefITEM && m_pRefITEM->InvokingDestParamIDByUse )
	{
		// µ•Ω∫∆ƒ∂˜ ∏Ò∑œ æÚæÓø¿±‚
		CVectorInvokingWearItemDP dpList;
		g_pDatabase->GetInvokingWearItemDPByUseList( &dpList, m_pRefITEM->InvokingDestParamIDByUse );

		if( !dpList.empty() )
		{
			CVectorInvokingWearItemDPIt it = dpList.begin();
			while( it != dpList.end() )
			{
				// ±‚¥… ≈¯∆¡ √ﬂ∞°
				SetFunction( (*pParameterIndex)++, (*it)->InvokingDestParam, (*it)->InvokingDestParamValue, 0, 0, FUNCTIONTYPE_USE );
				++it;
			}
		}
	}
}

void CINFItemInfo::SetItemCoolTime( int* pParameterIndex, BOOL bSetTick /* = TRUE */ )
{

	if (!m_pRefItemInfo || !m_pRefItemInfo->ItemInfo)
		return;

	// ƒ≈∏¿” ≈¯∆¡ √ﬂ∞°
	if( m_pRefItemInfo )
	{
		int nSec = ((int)(m_pRefItemInfo->GetRealItemInfo()->ReAttacktime) - m_pRefItemInfo->GetCoolElapsedTime()) / 1000;
		if( nSec < 0 )
			nSec = 0;

		sprintf( m_strItemInfo[*pParameterIndex],
				 STRMSG_C_100218_0307, nSec );	// "¿Áπﬂµø Ω√∞£ : %2dsec"

		if( bSetTick )
		{
			// ≈¯∆¡ ƒ≈∏¿”ø° √ﬂ∞°
			stTickFuntionIndex stAddTemp;
			stAddTemp.nFuntionIndex		= FUNCTION_INDEX_ITEM_COOL_TIME;
			stAddTemp.nDataLineIndex	= *pParameterIndex;
			m_vecTickFuntionIndex.push_back(stAddTemp);

			++(*pParameterIndex);
		}
	}
}
// end 2010. 02. 11 by ckPark πﬂµø∑˘ ¿Â¬¯æ∆¿Ã≈€


///////////////////////////////////////////////////////////////////////////////
/// \fn			SetDefenseColorInfo(int  nParameterIndex )
/// \brief		
/// \author		ydkim
/// \date		2005-12-09
/// \warning	æ∆∏” ƒÆ∂Û ¡§∫∏
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CINFItemInfo::SetDefenseColorInfo(int  nParameterIndex )
{
	char buf[256];
	ZERO_MEMORY(buf);

	ITEM* pITEM = g_pDatabase->GetServerItemInfo(m_pRefItemInfo->ColorCode);
	if(NULL == pITEM)
	{
		pITEM = g_pDatabase->GetServerItemInfo(m_pRefITEM->SourceIndex+1);
	}

	if(pITEM)
		strcpy(buf, pITEM->ItemName);
	else
		strcpy(buf, STRMSG_C_051208_0001);	//"±‚∫ªƒÆ∂Û"
	
	wsprintf( m_strItemInfo[nParameterIndex], STRMSG_C_051208_0002, buf) ;	// "ªˆªÛ¡§∫∏ : %s"
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			SetCountableWeight( int nParameterIndex)
/// \brief		COUNTABLE ITEM¿« ∞πºˆ ºº∆√
/// \author		dhkwon
/// \date		2004-09-17 ~ 2004-09-17
/// \warning	Enchant,Prefix,Suffix∞° æ¯¥Ÿ.
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CINFItemInfo::SetCountableWeight( int nParameterIndex )
{
	int nCount=1;
	if(m_pRefItemInfo)
	{
		nCount = m_pRefItemInfo->CurrentCount/m_pRefITEM->MinTradeQuantity;
		int nRemain = m_pRefItemInfo->CurrentCount%m_pRefITEM->MinTradeQuantity;
		if(nRemain > 0)
		{
			nCount += 1;
		}
	}
	sprintf( m_strItemInfo[nParameterIndex], STRMSG_C_ITEM_0045, 
		(int)m_pRefITEM->Weight*nCount );
}
///////////////////////////////////////////////////////////////////////////////
/// \fn			SetReqSP( int nParameterIndex)
/// \brief		
/// \author		dhkwon
/// \date		2004-06-14 ~ 2004-06-14
/// \warning	Enchant, UTC ¡§∫∏ æ¯¿Ω
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CINFItemInfo::SetReqSP( int nParameterIndex)
{
	if(m_bEnableItem || g_pShuttleChild->m_myShuttleInfo.SP >= m_pRefITEM->ReqSP)
	{
		sprintf( m_strItemInfo[nParameterIndex], STRMSG_C_SKILL_0007, m_pRefITEM->ReqSP) ;//"SP(Ω∫≈≥∆˜¿Œ∆Æ) : %d"
	}
	else
	{
		sprintf( m_strItemInfo[nParameterIndex], STRMSG_C_SKILL_0008, m_pRefITEM->ReqSP) ;//"SP(Ω∫≈≥∆˜¿Œ∆Æ) : \\r%d\\r"
	}

}

void CINFItemInfo::SetBullet( int nParameterIndex)
{
	FLOG( "CINFItemInfo::SetBullet( int nParameterIndex, int nCount )" );
	wsprintf( m_strItemInfo[nParameterIndex], STRMSG_C_ITEM_0046, m_pRefItemInfo ? m_pRefItemInfo->CurrentCount : 0); //"ºˆ∑Æ : %d"
}

void CINFItemInfo::SetFUEL( int nParameterIndex)
{
	wsprintf( m_strItemInfo[nParameterIndex], STRMSG_C_ITEM_0047, m_pRefItemInfo ? m_pRefItemInfo->CurrentCount : 0, m_pRefITEM->Charging ); //"ø¨∑· : %d / %d"
}

void CINFItemInfo::SetCount( int nParameterIndex)
{
	FLOG( "CINFItemInfo::SetCount( int nParameterIndex, int nCount )" );
//	wsprintf( m_strItemInfo[nParameterIndex], STRMSG_C_ITEM_0048, m_pRefItemInfo ? m_pRefItemInfo->CurrentCount : 0 ); //"ºˆ∑Æ : %d "
	// 2006-03-14 by ispark, æ∆¿Ã≈€ ¡§∫∏∞° æ¯¿ªΩ√ ∫∞µµ¿« æ∆¿Ã≈€ ƒ´øÓ∑Œ «—¥Ÿ.
	wsprintf( m_strItemInfo[nParameterIndex], STRMSG_C_ITEM_0048, m_pRefItemInfo ? m_pRefItemInfo->CurrentCount : m_nOtherItemCount ); //"ºˆ∑Æ : %d "
}

void CINFItemInfo::SetSpeed( int nParameterIndex )
{
	// 2008-09-26 by bhsohn Ω≈±‘ ¿Œ√æ∆Æ √≥∏Æ
	//sprintf( m_strItemInfo[nParameterIndex], STRMSG_C_ITEM_0049, m_pRefITEM->AbilityMin, m_pRefITEM->AbilityMax );//"º”µµ : %.0fm/s ~ %.0fm/s"
	sprintf( m_strItemInfo[nParameterIndex], STRMSG_C_080929_0209);//

	// √÷º“ º”µµ
	char chTmp[ITEMINFO_ITEM_FULL_NAME];
	memset(chTmp, 0x00, ITEMINFO_ITEM_FULL_NAME);
	{
		sprintf(chTmp, STRMSG_C_080929_0210, m_pRefITEM->AbilityMin);
		sprintf( m_strItemInfo[nParameterIndex], "%s%s", m_strItemInfo[nParameterIndex], chTmp);//
		float fEnchant = m_pRefEnchant ? m_pRefEnchant->pfn_ENGINE_MIN_SPEED_UP: 0;		
		if(fEnchant > 0)
		{			
			sprintf(chTmp, STRMSG_C_080929_0210, fEnchant);
			sprintf( m_strItemInfo[nParameterIndex], "%s\\e[+%s]\\e", m_strItemInfo[nParameterIndex], chTmp);//
		}
		strcat( m_strItemInfo[nParameterIndex], " ~ ");//
	}
	// √÷¥Î º”µµ
	{
		sprintf(chTmp, STRMSG_C_080929_0210, m_pRefITEM->AbilityMax);
		sprintf( m_strItemInfo[nParameterIndex], "%s%s", m_strItemInfo[nParameterIndex], chTmp);//
		float fEnchant = m_pRefEnchant ? m_pRefEnchant->pfn_ENGINE_MAX_SPEED_UP: 0;		
		if(fEnchant > 0)
		{			
			sprintf(chTmp, STRMSG_C_080929_0210, fEnchant);
			sprintf( m_strItemInfo[nParameterIndex], "%s\\e[+%s]\\e", m_strItemInfo[nParameterIndex], chTmp);//
		}		
	}

	
		
}
void CINFItemInfo::SetBoosterSpeed( int nParameterIndex )
{
	// 2008-09-26 by bhsohn Ω≈±‘ ¿Œ√æ∆Æ √≥∏Æ
	//sprintf( m_strItemInfo[nParameterIndex], STRMSG_C_ITEM_0050, m_pRefITEM->Range );//"∫ŒΩ∫≈Õº”µµ : %dm/s"
	sprintf( m_strItemInfo[nParameterIndex], STRMSG_C_ITEM_0050, m_pRefITEM->Range );//"∫ŒΩ∫≈Õº”µµ : %dm/s"
	float fEnchant = m_pRefEnchant ? m_pRefEnchant->pfn_ENGINE_BOOSTER_SPEED_UP: 0;		
	if(fEnchant > 0)
	{			
		char chTmp[ITEMINFO_ITEM_FULL_NAME];
		memset(chTmp, 0x00, ITEMINFO_ITEM_FULL_NAME);

		sprintf(chTmp, STRMSG_C_080929_0210, fEnchant);
		sprintf( m_strItemInfo[nParameterIndex], "%s\\e[+%s]\\e", m_strItemInfo[nParameterIndex], chTmp);//
	}		

}
void CINFItemInfo::SetCharging(int nParameterIndex)
{
	// 2008-09-26 by bhsohn Ω≈±‘ ¿Œ√æ∆Æ √≥∏Æ
	//sprintf(m_strItemInfo[nParameterIndex],STRMSG_C_061018_0101,m_pRefITEM->Charging);//"¡ˆªÛº”µµ : %dm/s"
	sprintf(m_strItemInfo[nParameterIndex],STRMSG_C_061018_0101,m_pRefITEM->Charging);//"¡ˆªÛº”µµ : %dm/s"
	float fEnchant = m_pRefEnchant ? m_pRefEnchant->pfn_ENGINE_GROUND_SPEED_UP: 0;		
	if(fEnchant > 0)
	{			
		char chTmp[ITEMINFO_ITEM_FULL_NAME];
		memset(chTmp, 0x00, ITEMINFO_ITEM_FULL_NAME);

		sprintf(chTmp, STRMSG_C_080929_0210, fEnchant);
		sprintf( m_strItemInfo[nParameterIndex], "%s\\e[+%s]\\e", m_strItemInfo[nParameterIndex], chTmp);//
	}		
}
void CINFItemInfo::SetBoosterTime( int nParameterIndex )
{
	// 2008-09-26 by bhsohn Ω≈±‘ ¿Œ√æ∆Æ √≥∏Æ
	//sprintf( m_strItemInfo[nParameterIndex], STRMSG_C_ITEM_0051, m_pRefITEM->Time/1000 );//"∫ŒΩ∫≈ÕΩ√∞£ : %d√ "
	float fEnchantHitRate = m_pRefEnchant ? m_pRefEnchant->pfn_ENGINE_BOOSTER_TIME_UP: 0;
	sprintf( m_strItemInfo[nParameterIndex], STRMSG_C_ITEM_0051,m_pRefITEM->Time/1000 );//"∫ŒΩ∫≈ÕΩ√∞£ : %d√ "
	if(fEnchantHitRate > 0)
	{
		sprintf( m_strItemInfo[nParameterIndex], STRMSG_C_080929_0208, m_strItemInfo[nParameterIndex], fEnchantHitRate) ;	
	}
}
void CINFItemInfo::SetRotateAngle( int nParameterIndex )
{
	// 2008-09-26 by bhsohn Ω≈±‘ ¿Œ√æ∆Æ √≥∏Æ	
	//sprintf( m_strItemInfo[nParameterIndex], STRMSG_C_ITEM_0052, (m_pRefITEM->RangeAngle/PI)*180.0f );//"¿œπ›»∏¿¸∞¢ : %.0fµµ"		
	float fEnEnchantAngle = m_pRefEnchant ? (m_pRefEnchant->pfm_ENGINE_ANGLE_UP): 0;;
	sprintf( m_strItemInfo[nParameterIndex], STRMSG_C_ITEM_0052, (m_pRefITEM->RangeAngle/PI)*180.0f );//"¿œπ›»∏¿¸∞¢ : %.0fµµ"
	if(fEnEnchantAngle > 0)
	{
		char chTmp[ITEMINFO_ITEM_FULL_NAME];
		memset(chTmp, 0x00, ITEMINFO_ITEM_FULL_NAME);
		sprintf(chTmp, STRMSG_C_080929_0213, (fEnEnchantAngle/PI)*180.0f );
		sprintf( m_strItemInfo[nParameterIndex], "%s\\e[+%s]\\e", m_strItemInfo[nParameterIndex], chTmp);//": %.0fµµ"

	}
}
void CINFItemInfo::SetBoosterRotateAngle( int nParameterIndex )
{
	// 2008-09-26 by bhsohn Ω≈±‘ ¿Œ√æ∆Æ √≥∏Æ	
	//sprintf( m_strItemInfo[nParameterIndex], STRMSG_C_ITEM_0053, (m_pRefITEM->BoosterAngle/PI)*180.0f );//"∫ŒΩ∫≈Õ»∏¿¸∞¢ : %.0fµµ"
	float fEnEnchantAngle = m_pRefEnchant ? (m_pRefEnchant->pfm_ENGINE_BOOSTERANGLE_UP): 0;;
	sprintf( m_strItemInfo[nParameterIndex], STRMSG_C_ITEM_0053, (m_pRefITEM->BoosterAngle/PI)*180.0f );//"∫ŒΩ∫≈Õ»∏¿¸∞¢ : %.0fµµ"

	if(fEnEnchantAngle > 0)
	{
		char chTmp[ITEMINFO_ITEM_FULL_NAME];
		memset(chTmp, 0x00, ITEMINFO_ITEM_FULL_NAME);
		sprintf(chTmp, STRMSG_C_080929_0213, (fEnEnchantAngle/PI)*180.0f );
		sprintf( m_strItemInfo[nParameterIndex], "%s\\e[+%s]\\e", m_strItemInfo[nParameterIndex], chTmp);//": %.0fµµ"

	}
}
// 2010. 04. 21 by jskim Ω≈±‘ ∑∞≈∞ ∏”Ω≈ ±∏«ˆ
// 2010. 02. 11 by ckPark πﬂµø∑˘ ¿Â¬¯æ∆¿Ã≈€
//void CINFItemInfo::SetFunction(int nParameterIndex, BYTE bType1, float fValue1, BYTE bType2, float fValue2)
//void CINFItemInfo::SetFunction( int nParameterIndex,
//								BYTE bType1,
//								float fValue1,
//								BYTE bType2,
//								float fValue2,
//								FUNCTION_TYPE nFunctionType /* = FUNCTIONTYPE_NORMAL */ )
//// end 2010. 02. 11 by ckPark πﬂµø∑˘ ¿Â¬¯æ∆¿Ã≈€
//// 2010. 02. 11 by ckPark πﬂµø∑˘ ¿Â¬¯æ∆¿Ã≈€
#ifdef BONUS_STAT_ITEM
void CINFItemInfo::SetFunction(int nParameterIndex,
	DestParam_t bType1, // 2011-08-02 by jhahn ∆ƒ∆Æ≥  º∫¿Â«¸ Ω√Ω∫≈€ Byteø°º≠  USHORT«¸ µ•Ω∫∆ƒ∂˜¿∏∑Œ ∫Ø∞Ê
	float fValue1,
	BYTE bType2,
	float fValue2,
	FUNCTION_TYPE nFunctionType /* = FUNCTIONTYPE_NORMAL */,
	float fRareValue,
	BOOL Bonus)
{
	FLOG("CINFItemInfo::SetFunction(int nParameterIndex, BYTE bType1, float fValue1, BYTE bType2, float fValue2)");
	char buf1[128], buf2[128];

	memset(buf1, 0x00, sizeof(buf1));
	memset(buf2, 0x00, sizeof(buf2));

	BOOL bDefEnchant;
	if (nFunctionType == FUNCTIONTYPE_NORMAL)
		bDefEnchant = TRUE;
	else	//	FUNCTIONTYPE_EQUIP or FUNCTIONTYPE_USE
		bDefEnchant = FALSE;

	// 2010. 04. 21 by jskim Ω≈±‘ ∑∞≈∞ ∏”Ω≈ ±∏«ˆ
	if (fRareValue > 0)
	{
		fValue1 += fRareValue;
	}
	// 2010. 04. 21 by jskim Ω≈±‘ ∑∞≈∞ ∏”Ω≈ ±∏«ˆ
#ifdef BONUS_STAT_ITEM
/*	if (DES_HP == bType1 || bType2)
	{
		if (m_pRefBonus)
		{
			fValue1 = (fValue1 - m_pRefBonus->pfp_HP);
			fValue2 = (fValue2 - m_pRefBonus->pfp_HP);
		}
	}
	if (DES_DP == bType1 || bType2)
	{
		if (m_pRefBonus)
		{
			fValue1 = (fValue1 - m_pRefBonus->pfp_DP);
			fValue2 = (fValue2 - m_pRefBonus->pfp_DP);
		}
	}*/
	if (DES_DEFENSE_01 == bType1 || bType2)
	{
		if (m_pRefBonus)
		{
			fValue1 = fValue1 + (m_pRefBonus->pfp_DEFENSE_01 / 100);
			fValue2 = fValue2 + (m_pRefBonus->pfp_DEFENSE_01 / 100);
		}
	}
	if (DES_DEFENSE_02 == bType1 || bType2)
	{
		if (m_pRefBonus)
		{
			fValue1 = fValue1 + (m_pRefBonus->pfp_DEFENSE_02 / 100);
			fValue2 = fValue2 + (m_pRefBonus->pfp_DEFENSE_02 / 100);
		}
	}
	if (DES_DEFENSEPROBABILITY_01 == bType1 || bType2)
	{
		if (m_pRefBonus)
		{
			fValue1 = fValue1 + (m_pRefBonus->pfp_DEFENSEPROBABILITY_01 / 100);
			fValue2 = fValue2 + (m_pRefBonus->pfp_DEFENSEPROBABILITY_01 / 100);
		}
	}
	if (DES_DEFENSEPROBABILITY_02 == bType1 || bType2)
	{
		if (m_pRefBonus)
		{
			fValue1 = fValue1 + (m_pRefBonus->pfp_DEFENSEPROBABILITY_02 / 100);
			fValue2 = fValue2 + (m_pRefBonus->pfp_DEFENSEPROBABILITY_02 / 100);
		}
	}
#endif
	if (bType1 != 0)
		SetParameterInfo(buf1, bType1, fValue1, bDefEnchant, Bonus);	// 2013-06-12 by ssjung æ∆∏” ƒ√∑∫º« ≈¯∆¡ «•Ω√
	if (bType2 != 0)
		SetParameterInfo(buf2, bType2, fValue2, bDefEnchant, Bonus);

	if (m_pRefITEM
		&& m_pRefITEM->Kind == ITEMKIND_ENCHANT)
	{
		nFunctionType = FUNCTIONTYPE_NORMAL;
	}
	switch (nFunctionType)
	{
	case FUNCTIONTYPE_NORMAL:
	{
		if (strlen(buf1) > 0 && strlen(buf2) > 0)
			sprintf(m_strItemInfo[nParameterIndex], STRMSG_C_ITEM_0054, buf1, buf2);	//"±‚¥… : \\g%s/%s"
		else if (strlen(buf1) > 0)
			sprintf(m_strItemInfo[nParameterIndex], STRMSG_C_ITEM_0055, buf1);		//"±‚¥… : \\g%s"
		else if (strlen(buf2) > 0)
			sprintf(m_strItemInfo[nParameterIndex], STRMSG_C_ITEM_0055, buf2);		//"±‚¥… : \\g%s"
	}
	break;

	case FUNCTIONTYPE_EQUIP:
	{
		if (strlen(buf1) > 0 && strlen(buf2) > 0)
			sprintf(m_strItemInfo[nParameterIndex], STRMSG_C_100218_0309, buf1, buf2);//"¬¯øÎ±‚¥… : \\g%s/%s"
		else if (strlen(buf1) > 0)
			sprintf(m_strItemInfo[nParameterIndex], STRMSG_C_100218_0310, buf1);		//"¬¯øÎ±‚¥… : \\g%s"
		else if (strlen(buf2) > 0)
			sprintf(m_strItemInfo[nParameterIndex], STRMSG_C_100218_0310, buf2);		//"¬¯øÎ±‚¥… : \\g%s"
	}
	break;

	case FUNCTIONTYPE_USE:
	{
		if (strlen(buf1) > 0 && strlen(buf2) > 0)
			sprintf(m_strItemInfo[nParameterIndex], STRMSG_C_100218_0311, buf1, buf2);//"ªÁøÎ±‚¥… : \\g%s/%s"
		else if (strlen(buf1) > 0)
			sprintf(m_strItemInfo[nParameterIndex], STRMSG_C_100218_0312, buf1);		//"ªÁøÎ±‚¥… : \\g%s"
		else if (strlen(buf2) > 0)
			sprintf(m_strItemInfo[nParameterIndex], STRMSG_C_100218_0312, buf2);		//"ªÁøÎ±‚¥… : \\g%s"
	}
	break;
	}
	// end 2010. 02. 11 by ckPark πﬂµø∑˘ ¿Â¬¯æ∆¿Ã≈€
}
#else
void CINFItemInfo::SetFunction( int nParameterIndex,
							   BYTE bType1,
							   float fValue1,
							   BYTE bType2,
							   float fValue2,
							   FUNCTION_TYPE nFunctionType /* = FUNCTIONTYPE_NORMAL */,
							   float fRareValue )
// end 2010. 02. 11 by ckPark πﬂµø∑˘ ¿Â¬¯æ∆¿Ã≈€
//end 2010. 04. 21 by jskim Ω≈±‘ ∑∞≈∞ ∏”Ω≈ ±∏«ˆ
{
	FLOG( "CINFItemInfo::SetFunction(int nParameterIndex, BYTE bType1, float fValue1, BYTE bType2, float fValue2)" );
	char buf1[128], buf2[128];
	memset( buf1, 0x00, sizeof(buf1));
	memset( buf2, 0x00, sizeof(buf2));
	

	// 2010. 02. 11 by ckPark πﬂµø∑˘ ¿Â¬¯æ∆¿Ã≈€
/*
//	if(fValue1 != 0) {
	if(bType1 != 0) {
		SetParameterInfo( buf1, bType1, fValue1); }
//	if(fValue2 != 0) {
	if(bType2 != 0) {
		SetParameterInfo( buf2, bType2, fValue2); }

	if(strlen(buf1)>0 && strlen(buf2)>0){
		wsprintf( m_strItemInfo[nParameterIndex], STRMSG_C_ITEM_0054, buf1, buf2);}//"±‚¥… : \\g%s/%s"
	else if(strlen(buf1)>0){
		wsprintf( m_strItemInfo[nParameterIndex], STRMSG_C_ITEM_0055, buf1);}//"±‚¥… : \\g%s"
	else if(strlen(buf2)>0){
		wsprintf( m_strItemInfo[nParameterIndex], STRMSG_C_ITEM_0055, buf2);}
*/

	BOOL bDefEnchant;
	if( nFunctionType == FUNCTIONTYPE_NORMAL )
		bDefEnchant = TRUE;
	else	//	FUNCTIONTYPE_EQUIP or FUNCTIONTYPE_USE
		bDefEnchant = FALSE;

	// 2010. 04. 21 by jskim Ω≈±‘ ∑∞≈∞ ∏”Ω≈ ±∏«ˆ
	if(fRareValue > 0)
	{
		fValue1 += fRareValue;
 	}
	// 2010. 04. 21 by jskim Ω≈±‘ ∑∞≈∞ ∏”Ω≈ ±∏«ˆ

	if(bType1 != 0)
		SetParameterInfo( buf1, bType1, fValue1, bDefEnchant );
	if(bType2 != 0)
		SetParameterInfo( buf2, bType2, fValue2, bDefEnchant );
	
	// 2010. 04. 21 by jskim Ω≈±‘ ∑∞≈∞ ∏”Ω≈ ±∏«ˆ
//  	if(fRareValue > 0)
//  	{
// 		if(bType1 != 0)
// 				SetRareParameterInfo(buf1, fRareValue);
// 		if(bType2 != 0)
// 				SetRareParameterInfo(buf2, fRareValue);
// 	}
	// 2010. 04. 21 by jskim Ω≈±‘ ∑∞≈∞ ∏”Ω≈ ±∏«ˆ



	if( m_pRefITEM
		&& m_pRefITEM->Kind == ITEMKIND_ENCHANT )
	{
		nFunctionType = FUNCTIONTYPE_NORMAL;
	}
	
	switch( nFunctionType )
	{
		case FUNCTIONTYPE_NORMAL:
		{
			if( strlen(buf1) > 0 && strlen(buf2) > 0 )
				wsprintf( m_strItemInfo[nParameterIndex], STRMSG_C_ITEM_0054, buf1, buf2);	//"±‚¥… : \\g%s/%s"
			else if( strlen(buf1) > 0 )
				wsprintf( m_strItemInfo[nParameterIndex], STRMSG_C_ITEM_0055, buf1);		//"±‚¥… : \\g%s"
			else if( strlen(buf2) > 0 )
				wsprintf( m_strItemInfo[nParameterIndex], STRMSG_C_ITEM_0055, buf2);		//"±‚¥… : \\g%s"
		}
		break;

		case FUNCTIONTYPE_EQUIP:
		{
			if( strlen(buf1) > 0 && strlen(buf2) > 0 )
				wsprintf( m_strItemInfo[nParameterIndex], STRMSG_C_100218_0309, buf1, buf2);//"¬¯øÎ±‚¥… : \\g%s/%s"
			else if( strlen(buf1) > 0 )
				wsprintf( m_strItemInfo[nParameterIndex], STRMSG_C_100218_0310, buf1);		//"¬¯øÎ±‚¥… : \\g%s"
			else if( strlen(buf2) > 0 )
				wsprintf( m_strItemInfo[nParameterIndex], STRMSG_C_100218_0310, buf2);		//"¬¯øÎ±‚¥… : \\g%s"
		}
		break;

		case FUNCTIONTYPE_USE:
		{
			if( strlen(buf1) > 0 && strlen(buf2) > 0 )
				wsprintf( m_strItemInfo[nParameterIndex], STRMSG_C_100218_0311, buf1, buf2);//"ªÁøÎ±‚¥… : \\g%s/%s"
			else if( strlen(buf1) > 0 )
				wsprintf( m_strItemInfo[nParameterIndex], STRMSG_C_100218_0312, buf1);		//"ªÁøÎ±‚¥… : \\g%s"
			else if( strlen(buf2) > 0 )
				wsprintf( m_strItemInfo[nParameterIndex], STRMSG_C_100218_0312, buf2);		//"ªÁøÎ±‚¥… : \\g%s"
		}
		break;
	}

	// end 2010. 02. 11 by ckPark πﬂµø∑˘ ¿Â¬¯æ∆¿Ã≈€
}
#endif
// 2009. 11. 02 by ckPark ¿Œ««¥œ∆º « µÂ ¿ŒΩ∫≈œΩ∫ ¥¯¡Ø Ω√Ω∫≈€
// ªÁøÎ ∞°¥… ¡ˆø™ ≈¯∆¡
void CINFItemInfo::SetUseAreaInfinity( int* p_nParameterIndex )
{
	if( m_pRefITEM
		&& COMPARE_BIT_FLAG( m_pRefITEM->ItemAttribute, ITEM_ATTR_ONLY_USE_INFINITY ) )
	{
		sprintf( m_strItemInfo[(*p_nParameterIndex)++], STRMSG_C_091103_0325 ); // "ªÁøÎ ∞°¥…¡ˆø™ : ¿Œ««¥œ∆º « µÂ"
	}
}

void CINFItemInfo::SetExchangeMaterial( BOOL bShop /* = FALSE */ )
{
	m_vecExchageMtrl.clear();

	if( bShop )
	{
		if( m_pRefITEM )
		{
			CMapCityShopList::iterator it = g_pInterface->m_pCityBase->m_mapCityShop.find( BUILDINGKIND_INFINITY_SHOP );
			if( it != g_pInterface->m_pCityBase->m_mapCityShop.end() )
			{
				INFINITY_SHOP_INFO info;
				memset( &info, 0, sizeof( INFINITY_SHOP_INFO ) );

				if( ((CINFCityInfinityShop*)(it->second))->FindInfinityShopInfo_From_CurrentTab( m_pRefITEM->ItemNum, &info ) )
				{
					for( int i=0; i<MAX_ORB_KIND; ++i )
					{
						ItemNum_t				nItemNum;
						InfinityShopItemCnt_t	nItemCnt;
						
						((CINFCityInfinityShop*)(it->second))->GetOrbInfo( &info, i, &nItemNum, &nItemCnt );
						
						if( nItemNum )
							m_vecExchageMtrl.push_back( std::pair<ItemNum_t,InfinityShopItemCnt_t>(nItemNum, nItemCnt) );
					}
				}
			}
		}
	}
}
// end 2009. 11. 02 by ckPark ¿Œ««¥œ∆º « µÂ ¿ŒΩ∫≈œΩ∫ ¥¯¡Ø Ω√Ω∫≈€

void CINFItemInfo::SetItemExtendInfo(BOOL bShow, BOOL bflag)
{
	if (!bShow)
		return;
#ifdef _INET_INVEN_RIGHT_CLICK_MENU
	int nIndex = 0;
	m_nExtendItemIndex = 0;
	wsprintf(m_strExtendItemInfo[nIndex++], " ");
	wsprintf(m_strExtendItemInfo[nIndex++], "\\sCtrl+Click in Inventory:\\s");
	wsprintf(m_strExtendItemInfo[nIndex++], "\\nAdvanced item menu\\n");
	
	wsprintf(m_strExtendItemInfo[nIndex++], " ");
	wsprintf(m_strExtendItemInfo[nIndex++], "\\sCtrl+Click in Shop:\\s");
	wsprintf(m_strExtendItemInfo[nIndex++], "\\nSelect multiple items\\n");
	wsprintf(m_strExtendItemInfo[nIndex++], " ");
	m_nExtendItemIndex = nIndex;
#endif
}
void CINFItemInfo::SetDesc( int nParameterIndex)
{
	FLOG( "CINFItemInfo::SetDesc( int nParameterIndex, char* strDesc )" );
	m_nDescIndex = nParameterIndex;
	memset(m_strDesc, 0x00, ITEMINFO_DESC_SIZE*ITEMINFO_DESC_LINE_NUMBER);
//	if(strlen(strDesc) == 0)
	if(strlen(m_pRefITEM->Description) == 0)
	{
//		DBGOUT("Item discription is NULL.\n");
		return;
	}
	int i = 0;
	int nPoint = 0;
	int nCheckPoint = 0;
	int nBreakPoint = 0;
	int nLine = 0;
	char strBuf[SIZE_MAX_ITEM_DESCRIPTION+15];
	memset(strBuf, 0x00,SIZE_MAX_ITEM_DESCRIPTION+15);
	strcpy( strBuf, STRMSG_C_ITEM_0056);//"º≥∏Ì : "
//	strcat( strBuf, strDesc );
	strcat( strBuf, m_pRefITEM->Description );
	if(COMPARE_BIT_FLAG(m_pRefITEM->ItemAttribute, ITEM_ATTR_CASH_ITEM))
	{
		for(int i=0; i<strlen(strBuf); i++)
		{
			if(strBuf[i]=='$')
				strBuf[i] = ' ';
		}
	}
//	while(TRUE)
//	{
//		if(strBuf[i] == ' ' || strBuf[i] == '.' || strBuf[i] == '!' || strBuf[i] == NULL)
//		{
//			if(nPoint >= ITEMINFO_DESC_SIZE-1)
//			{
//				if(nLine >= ITEMINFO_DESC_LINE_NUMBER)
//				{
//					DBGOUT("CINFItemInfo::SetDesc(),1 Item Discription is Too Long to Draw .\n");
//					return;
//				}
//				memcpy(m_strDesc[nLine], strBuf + nCheckPoint, nBreakPoint+1);
//				nPoint -= nBreakPoint;
//				nCheckPoint += nBreakPoint+1;
//				nBreakPoint = nPoint-1;
//				nLine ++;
//				i++;
//				continue;
//			}
//			if(strBuf[i] == NULL)
//			{
//				if(nLine >= ITEMINFO_DESC_LINE_NUMBER)
//				{
//					DBGOUT("CINFItemInfo::SetDesc(),2 Item Discription is too Long to Draw .\n");
//					return;
//				}
//				memcpy(m_strDesc[nLine], strBuf + nCheckPoint, nPoint);
//				break;
//			}
//			nBreakPoint = nPoint;
//		}
//		i++;
//		nPoint++;
//	}
	vector<string> vectemp;
	// 2008-04-14 by bhsohn ¿Ø∑¥ æ∆¿Ã≈€ º≥∏Ì Ω∫∆Æ∏µπÆ¡¶ √≥∏Æ
	//::StringCullingUserData(strBuf, 40, &vectemp);	
	vector<string> vectempCulling;
	::StringCullingUserData(strBuf, 40, &vectempCulling);	
	int nItemStringLen = 0;
	if(vectempCulling.size() > 0)
	{
		nItemStringLen = m_pFontDescInfo[0]->GetStringSize((char*)vectempCulling[0].c_str()).cx;
	}
	else
	{
		nItemStringLen = GetItemStringLen();
		if(nItemStringLen < 40)
		{
			nItemStringLen = 40;
		}
	}	
	
	STRING_CULL(strBuf, nItemStringLen, &vectemp, m_pFontDescInfo[0]);
	
	m_nDescLine = vectemp.size();
	for(i=0; i<vectemp.size(); i++)
	{
		//if(i>ITEMINFO_DESC_LINE_NUMBER) return;
		if(i >= ITEMINFO_DESC_LINE_NUMBER) 
		{
			break;
		}
		strcpy(m_strDesc[i], vectemp[i].c_str());		
	}
	// end 2008-04-14 by bhsohn ¿Ø∑¥ æ∆¿Ã≈€ º≥∏Ì Ω∫∆Æ∏µπÆ¡¶ √≥∏Æ
}

void CINFItemInfo::SetShopSellInfo( int nParameterIndex )
{
	if(m_pRefITEM->MinTradeQuantity >= 2)
	{
		sprintf( m_strItemInfo[nParameterIndex], STRMSG_C_ITEM_0057, //"ªÛ¡°∞°∞› : %d(Ω∫««) / %d∞≥¥Á"
			CAtumSJ::GetItemSellingPriceAtShop(m_pRefITEM), m_pRefITEM->MinTradeQuantity);
	}
	else
	{
		sprintf( m_strItemInfo[nParameterIndex], STRMSG_C_ITEM_0058, //"ªÛ¡°∞°∞› : %d(Ω∫««)"
			CAtumSJ::GetItemSellingPriceAtShop(m_pRefITEM) );
	}
}


// 2010-06-15 by shcho&hslee ∆ÍΩ√Ω∫≈€ - ∆Í ≈∏¿‘ ≈¯∆¡ √ﬂ∞°.
void CINFItemInfo :: SetPetType ( int nParameterIndex )
{
	tPET_BASE_ALL_DATA *psPetBaseAllData = g_pDatabase->GetPtr_PetAllDataByIndex ( m_pRefITEM->LinkItem );

	//tPET_LEVEL_DATA *psPetLevelData = psPetBaseAllData->rtn_LevelData( m_pRefITEM->SkillLevel );

	if ( NULL == psPetBaseAllData || NULL == psPetBaseAllData->rtn_LevelData( m_pRefITEM->SkillLevel ) )
		return;

	char szTemp[256] = {0, };
#ifdef _INET_PET
	if (m_pRefItemInfo)
	{
		tPET_CURRENTINFO *pPetCurrentInfo = g_pShuttleChild->GetPetManager()->GetPtr_PetCurrentData(m_pRefItemInfo->UniqueNumber);
		if (pPetCurrentInfo)
		{
			tPET_LEVEL_DATA *psPetLevelData = psPetBaseAllData->rtn_LevelData(pPetCurrentInfo->PetLevel);
			sprintf(szTemp, STRMSG_C_100812_0401, pPetCurrentInfo->PetLevel);
		}
	}
	else
		sprintf(szTemp, STRMSG_C_100812_0401, 1);
#else
		sprintf ( szTemp, STRMSG_C_100812_0401 , m_pRefITEM->SkillLevel );//"Lv.%d"		
#endif
	sprintf( m_strItemInfo[nParameterIndex], STRMSG_C_100812_0402, g_pInterface->GetString_PetType ( psPetBaseAllData->BaseData.PetKind ) , szTemp ); //"≈∏¿‘ : %s[%s]"

}

void CINFItemInfo :: SetPetReName( int nParameterIndex )
{
	char szTemp[256] = {0, };
	char buf[256] = {0, };
	if(m_pRefItemInfo->GetReName())
	{
		strcpy( buf, STRMSG_C_100812_0404 );		//"∞°¥…"	
		sprintf( m_strItemInfo[nParameterIndex], STRMSG_C_100812_0403,buf);  //"¿Ã∏ß ∫Ø∞Ê : %s"
	}
	else
	{
		strcpy( buf, STRMSG_C_100812_0405 );		//"∫“∞°¥…"
		sprintf( m_strItemInfo[nParameterIndex], STRMSG_C_100812_0403,buf);  //"¿Ã∏ß ∫Ø∞Ê : %s"
	}	
}
void CINFItemInfo :: SetPetEnableLevelUp( int nParameterIndex, BOOL bShop /* = FALSE */ )
{
	char szTemp[256] = {0, };
	char buf[256] = {0, };
	BOOL bTemp;
	
	if( bShop )
	{
		tPET_BASE_ALL_DATA *psPetBaseAllData = g_pDatabase->GetPtr_PetAllDataByIndex ( m_pRefITEM->LinkItem );	

		if( psPetBaseAllData->BaseData.EnableLevel )
		{
			bTemp = TRUE;
		}
		else
		{	
			bTemp = FALSE;
		}	
	}
	else
	{
		if(m_pRefItemInfo && m_pRefItemInfo->GetPetEnableLevelUp())
		{
			bTemp = TRUE;
		}
		else
		{	
			bTemp = FALSE;
		}	
	}

	if( bTemp )
	{
		strcpy( buf, STRMSG_C_100812_0404 );		//"∞°¥…"
		sprintf( m_strItemInfo[nParameterIndex], STRMSG_C_100812_0407,buf); //"º∫¿Âø©∫Œ : %s"
	}
	else
	{	
		strcpy( buf, STRMSG_C_100812_0405 );		//"∫“∞°¥…"
		sprintf( m_strItemInfo[nParameterIndex], STRMSG_C_100812_0407,buf); //"º∫¿Âø©∫Œ : %s"
	}	
}

void CINFItemInfo :: SetPetExp( int nParameterIndex )
{
#ifdef _INET_PET
	tPET_LEVEL_DATA* tempPetData = g_pDatabase->GetPtr_PetLevelData(m_pRefITEM->LinkItem, m_pRefITEM->SkillLevel);

	if (m_pRefITEM->LinkItem == 1000)
	{
		float Temp = 0;
		char szTemp[256] = { 0, };
		sprintf(m_strItemInfo[nParameterIndex], STRMSG_C_100812_0406, Temp);
	}
	else
	{
		double Temp = 0.0f;

		tPET_BASE_ALL_DATA *psPetBaseAllData = g_pDatabase->GetPtr_PetAllDataByIndex(m_pRefITEM->LinkItem);
		tPET_CURRENTINFO *pPetCurrentInfo = g_pShuttleChild->GetPetManager()->GetPtr_PetCurrentData(m_pRefItemInfo->UniqueNumber);
		if (pPetCurrentInfo &&  tempPetData->NeedExp > .0f)
		{
			int templevel;
			if (pPetCurrentInfo->PetLevel - 1 <= 0)
				templevel = 1;
			else
				templevel = pPetCurrentInfo->PetLevel - 1;

			tPET_LEVEL_DATA *psPetLevelDataPrev = g_pDatabase->GetPtr_PetLevelData(pPetCurrentInfo->PetIndex, pPetCurrentInfo->PetLevel - 1);
			tPET_LEVEL_DATA *psPetLevelDataNext = g_pDatabase->GetPtr_PetLevelData(pPetCurrentInfo->PetIndex, pPetCurrentInfo->PetLevel);

			float	ExpNow, ExpMax;

			if (psPetLevelDataPrev)
			{
				ExpNow = (float)pPetCurrentInfo->PetExp - (float)psPetLevelDataPrev->NeedExp;
				ExpMax = (float)psPetLevelDataNext->NeedExp - (float)psPetLevelDataPrev->NeedExp;
			}
			else
			{
				ExpNow = (float)pPetCurrentInfo->PetExp;
				ExpMax = (float)psPetLevelDataNext->NeedExp;
			}
			Temp = ExpNow / ExpMax * 100;
		}
		char szTemp[256] = { 0, };
		sprintf(m_strItemInfo[nParameterIndex], STRMSG_C_100812_0406, Temp);  //"EXP : %.2f%%" 
	}

#else
	tPET_LEVEL_DATA* tempPetData = g_pDatabase->GetPtr_PetLevelData( m_pRefITEM->LinkItem, m_pRefITEM->SkillLevel);

	float Temp = (float)m_pRefItemInfo->GetPetExp() / (float)tempPetData->NeedExp;

	char szTemp[256] = {0, };
	sprintf( m_strItemInfo[nParameterIndex], STRMSG_C_100812_0406, Temp);  //"EXP : %.2f%%" 
#endif
}
// End 2010-06-15 by shcho&hslee ∆ÍΩ√Ω∫≈€ - ∆Í ≈∏¿‘ ≈¯∆¡ √ﬂ∞°.


///////////////////////////////////////////////////////////////////////////////
/// \fn			CINFItemInfo::SetPrimaryRange(int nParameterIndex)
/// \brief		±‚∫ªπ´±‚ ¡∂¡ÿ ∞≈∏Æ
/// \author		ispark
/// \date		2005-08-17 ~ 2005-08-17
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CINFItemInfo::SetPrimaryRange(int nParameterIndex)
{
	// 2008-09-26 by bhsohn Ω≈±‘ ¿Œ√æ∆Æ √≥∏Æ
//	sprintf( m_strItemInfo[nParameterIndex], STRMSG_C_050817_0001, 
//			CAtumSJ::GetPrimaryRadarRange(m_pRefITEM, &g_pShuttleChild->m_paramFactor));	

	float fRangeMin = m_pRefITEM ? m_pRefITEM->AbilityMin: 0;			 	
	sprintf( m_strItemInfo[nParameterIndex], STRMSG_C_050817_0001, fRangeMin);

	if(m_pRefEnchant && m_pRefEnchant->pfm_ATTACK_RANGE_01 > 0)
	{		
		char chTmp[ITEMINFO_ITEM_FULL_NAME];
		memset(chTmp, 0x00, ITEMINFO_ITEM_FULL_NAME);
		sprintf(chTmp, STRMSG_C_080923_0200, m_pRefEnchant->pfm_ATTACK_RANGE_01*100.0f);
		sprintf( m_strItemInfo[nParameterIndex], "%s\\e[+%s]\\e", m_strItemInfo[nParameterIndex], chTmp);
	}	
	
}


///////////////////////////////////////////////////////////////////////////////
/// \fn			CINFItemInfo::SetSecondaryRange(int nParameterIndex)
/// \brief		∞Ì±ﬁπ´±‚ ¡∂¡ÿ∞≈∏Æ
/// \author		ispark
/// \date		2005-08-17 ~ 2005-08-17
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CINFItemInfo::SetSecondaryRange(int nParameterIndex)
{
	// 2008-09-26 by bhsohn Ω≈±‘ ¿Œ√æ∆Æ √≥∏Æ
//	sprintf( m_strItemInfo[nParameterIndex], STRMSG_C_050817_0002, 
//			CAtumSJ::GetSecondaryRadarRange(m_pRefITEM, &g_pShuttleChild->m_paramFactor));
	float fRangeMax = m_pRefITEM ? m_pRefITEM->AbilityMax: 0;			 	
	sprintf( m_strItemInfo[nParameterIndex], STRMSG_C_050817_0002, fRangeMax);	

	if(m_pRefEnchant && m_pRefEnchant->pfm_ATTACK_RANGE_02 > 0)
	{		
		char chTmp[ITEMINFO_ITEM_FULL_NAME];
		memset(chTmp, 0x00, ITEMINFO_ITEM_FULL_NAME);
		sprintf(chTmp, STRMSG_C_080923_0200, m_pRefEnchant->pfm_ATTACK_RANGE_02*100.0f);
		sprintf( m_strItemInfo[nParameterIndex], "%s\\e[+%s]\\e", m_strItemInfo[nParameterIndex], chTmp);
	}	
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			CINFItemInfo::SetRadarRange(int nParameterIndex)
/// \brief		∑π¿Ã¥ı π∞√º ∞®¡ˆ π›∞Ê
/// \author		ispark
/// \date		2005-08-17 ~ 2005-08-17
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CINFItemInfo::SetRadarRange(int nParameterIndex)
{
	// 2008-09-26 by bhsohn Ω≈±‘ ¿Œ√æ∆Æ √≥∏Æ	
	//sprintf( m_strItemInfo[nParameterIndex], STRMSG_C_050817_0003, m_pRefITEM->Range);	
	float fEnRadarRange = m_pRefEnchant ? (m_pRefEnchant->pfn_RADAR_OBJECT_DETECT_RANGE): 0;;
	sprintf( m_strItemInfo[nParameterIndex], STRMSG_C_050817_0003, m_pRefITEM->Range);	
	if(fEnRadarRange > 0)
	{
		char chTmp[ITEMINFO_ITEM_FULL_NAME];
		memset(chTmp, 0x00, ITEMINFO_ITEM_FULL_NAME);
		sprintf(chTmp, STRMSG_C_080929_0212, fEnRadarRange);
		sprintf( m_strItemInfo[nParameterIndex], "%s\\e[+%s]\\e", m_strItemInfo[nParameterIndex], chTmp);//	
	}
}


///////////////////////////////////////////////////////////////////////////////
/// \fn			CINFItemInfo::SetEnginTurningAngle(int nParameterIndex)
/// \brief		ø£¡¯ º±»∏ ∞°º”∑¬
/// \author		ispark
/// \date		2005-08-17 ~ 2005-08-17
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CINFItemInfo::SetEnginTurningAngle(int nParameterIndex)
{
	sprintf( m_strItemInfo[nParameterIndex], STRMSG_C_050817_0004, m_pRefITEM->SpeedPenalty);	
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			CINFItemInfo::SetReUseTime(int nParameterIndex)
/// \brief		æ∆¿Ã≈€ ¿Áπﬂµø Ω√∞£
/// \author		ispark
/// \date		2005-12-07 ~ 2005-12-07
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CINFItemInfo::SetReUseTime(int nParameterIndex)
{
	sprintf( m_strItemInfo[nParameterIndex], STRMSG_C_051207_0002, (float)m_pRefITEM->ReAttacktime / 1000);	
}

#ifdef BONUS_STAT_ITEM
void CINFItemInfo::SetPrimaryWeaponInfo(BOOL bShop, BOOL Bonus)							  // 2013-06-26 by ssjung ¿Œ∫•≈‰∏Æ √ﬂ∞° ≈¯∆¡ 
#else
void CINFItemInfo::SetPrimaryWeaponInfo(BOOL bShop)							  // 2013-06-26 by ssjung ¿Œ∫•≈‰∏Æ √ﬂ∞° ≈¯∆¡ 
#endif
{
	int index = 0;
	SetItemName( index++ );
	SetItemKind( index++ );

	// 2009. 10. 28 by ckPark Ω√∞£/±‚∞£ ¡¶«— æ∆¿Ã≈€ ≈¯∆¡ ºˆ¡§
	SetRemainTime_Imp( m_pRefItemInfo, &index, bShop );
	// end 2009. 10. 28 by ckPark Ω√∞£/±‚∞£ ¡¶«— æ∆¿Ã≈€ ≈¯∆¡ ºˆ¡§
		
	SetUnitKind( index++ );
	SetReqLevel( index++ );
	
	
	
#ifdef BONUS_STAT_ITEM
	SetAttack(index++, Bonus);
	SetSecAttack(index++, Bonus);
	SetRate(index++, Bonus);
	SetPrimaryPierce(index++);	// ««æÓΩ∫ ∑¸
	SetRange(index++, Bonus);
	SetReAttackTime(index++, Bonus);
	//SetWeight(index++, Bonus);
#else
	SetAttack(index++);
	SetSecAttack(index++);
	SetRate(index++);
	SetPrimaryPierce(index++);	// ««æÓΩ∫ ∑¸
	SetRange(index++);
	SetReAttackTime(index++);
#endif
	
	
	
	if(m_pRefITEM->AttackTime > 0)
	{
		SetAttackTime( index++ );
	}
	SetOverHeatTime( index++ );
	SetWeight( index++ );

	// 2009. 08. 27 by ckPark ±◊∑°«» ∏Æº“Ω∫ ∫Ø∞Ê Ω√Ω∫≈€ ±∏«ˆ
	if( m_pRefItemInfo )
	{
		SetShapeInfo( index++ );
		SetEffectInfo( index++ );
	}
	// end 2009. 08. 27 by ckPark ±◊∑°«» ∏Æº“Ω∫ ∫Ø∞Ê Ω√Ω∫≈€ ±∏«ˆ

	if(bShop == FALSE)
	{
		SetShopSellInfo(index++);
		// 2007-09-07 by bhsohn æ∆¿Ã≈€ ∞≈∑°ø©∫Œ «•Ω√
		SetItemAttribute(index++);
	}

	// 2009. 11. 02 by ckPark ¿Œ««¥œ∆º « µÂ ¿ŒΩ∫≈œΩ∫ ¥¯¡Ø Ω√Ω∫≈€
	SetUseAreaInfinity( &index );
	SetExchangeMaterial( bShop );
	// end 2009. 11. 02 by ckPark ¿Œ««¥œ∆º « µÂ ¿ŒΩ∫≈œΩ∫ ¥¯¡Ø Ω√Ω∫≈€

	SetDesc( index );
	SetItemExtendInfo(TRUE);
	ASSERT_ASSERT(index <= ITEMINFO_PARAMETER_NUMBER);
	SetMaxLength();
}

#ifdef BONUS_STAT_ITEM
void CINFItemInfo::SetSecondaryWeaponInfo(BOOL bShop, BOOL Bonus)				  // 2013-06-26 by ssjung ¿Œ∫•≈‰∏Æ √ﬂ∞° ≈¯∆¡ 
{
#else
void CINFItemInfo::SetSecondaryWeaponInfo(BOOL bShop)
{
#endif
	int index = 0;
	SetItemName( index++);
	SetItemKind( index++ );

	// 2009. 10. 28 by ckPark Ω√∞£/±‚∞£ ¡¶«— æ∆¿Ã≈€ ≈¯∆¡ ºˆ¡§
	SetRemainTime_Imp( m_pRefItemInfo, &index, bShop );
	// end 2009. 10. 28 by ckPark Ω√∞£/±‚∞£ ¡¶«— æ∆¿Ã≈€ ≈¯∆¡ ºˆ¡§

	SetUnitKind( index++);
	SetReqLevel( index++);
#ifdef BONUS_STAT_ITEM
	SetAttack(index++, Bonus);
	SetSecAttack(index++, Bonus);
	SetRate(index++, Bonus);
	SetSecondaryPierce(index++);	// ««æÓΩ∫ ∑¸
	SetRange(index++, Bonus);
	SetReAttackTime(index++, Bonus);
	SetRangeAngle(index++);
	SetWeaponSpeed(index++, Bonus);
#else
	SetAttack(index++);
	SetSecAttack(index++);
	SetRate(index++);
	SetSecondaryPierce(index++);
	SetRange(index++);
	SetReAttackTime(index++);
	SetRangeAngle(index++);
	SetWeaponSpeed(index++);
#endif
	SetWeaponAngle(index++);
	if( m_pRefITEM->Kind == ITEMKIND_ROCKET || 
		m_pRefITEM->Kind == ITEMKIND_BUNDLE ||
		m_pRefITEM->Kind == ITEMKIND_MINE ||
		m_pRefITEM->Kind == ITEMKIND_MISSILE)// 2007-02-05 by bhsohn πÃªÁ¿œ æ∆¿Ã≈€ ≈¯∆¡ æ»≥™ø¿¥¬ ∫Œ∫– ºˆ¡§
	{
		SetExplosionRange( index++ );
	}
	if( m_pRefITEM->Kind == ITEMKIND_MINE )
	{
		SetReactionRange( index++ );
	}
	SetWeight( index++);

	// 2009. 08. 27 by ckPark ±◊∑°«» ∏Æº“Ω∫ ∫Ø∞Ê Ω√Ω∫≈€ ±∏«ˆ
	if( m_pRefItemInfo )
	{
		SetShapeInfo( index++ );
		SetEffectInfo( index++ );
	}
	// end 2009. 08. 27 by ckPark ±◊∑°«» ∏Æº“Ω∫ ∫Ø∞Ê Ω√Ω∫≈€ ±∏«ˆ

	if(bShop == FALSE)
	{
		SetShopSellInfo( index++ );
		// 2007-09-07 by bhsohn æ∆¿Ã≈€ ∞≈∑°ø©∫Œ «•Ω√
		SetItemAttribute(index++);
	}

	// 2009. 11. 02 by ckPark ¿Œ««¥œ∆º « µÂ ¿ŒΩ∫≈œΩ∫ ¥¯¡Ø Ω√Ω∫≈€
	SetUseAreaInfinity( &index );
	SetExchangeMaterial( bShop );
	// end 2009. 11. 02 by ckPark ¿Œ««¥œ∆º « µÂ ¿ŒΩ∫≈œΩ∫ ¥¯¡Ø Ω√Ω∫≈€

	SetDesc( index);
	SetItemExtendInfo(TRUE);
	ASSERT_ASSERT(index <= ITEMINFO_PARAMETER_NUMBER);
	SetMaxLength();
}

void CINFItemInfo::SetSkillItemInfo(BOOL bShop)
{
	int index = 0;
	SetItemName( index++);
	SetUnitKind( index++);

	// 2009. 10. 28 by ckPark Ω√∞£/±‚∞£ ¡¶«— æ∆¿Ã≈€ ≈¯∆¡ ºˆ¡§
	SetRemainTime_Imp( m_pRefItemInfo, &index, bShop );
	// end 2009. 10. 28 by ckPark Ω√∞£/±‚∞£ ¡¶«— æ∆¿Ã≈€ ≈¯∆¡ ºˆ¡§
	
	SetSkillLevel( index++ );
	SetReqLevel( index++);
	if( bShop == TRUE && m_pRefITEM->LinkItem != 0)
	{
		SetShopReqItem( index++ );
	}
	if(m_pRefITEM->ReqItemKind != ITEMKIND_ALL_ITEM)
	{
		SetReqItemKind( index++ );
	}
	if(m_pRefITEM->ReqSP > 0)
	{
		SetReqSP( index++);
	}
	if( m_pRefITEM->SkillType == SKILLTYPE_CLICK || 
		m_pRefITEM->SkillType == SKILLTYPE_TIMELIMIT )
	{
		SetSkillTime( index++ );
		SetSkillReAttackTime( index++ );
	}	

	SetParameter(&index);

	// 2007-09-07 by bhsohn æ∆¿Ã≈€ ∞≈∑°ø©∫Œ «•Ω√
	if(FALSE == bShop)
	{
		// 2007-10-16 by bhsohn Ω∫≈≥æ∆¿Ã≈€¿∫ ∞≈∑°ø©∫Œ «•Ω√ æ»µ«∞‘ ∫Ø∞Ê
		//SetItemAttribute(index++);
	}

	// 2009. 11. 02 by ckPark ¿Œ««¥œ∆º « µÂ ¿ŒΩ∫≈œΩ∫ ¥¯¡Ø Ω√Ω∫≈€
	SetUseAreaInfinity( &index );
	SetExchangeMaterial( bShop );
	// end 2009. 11. 02 by ckPark ¿Œ««¥œ∆º « µÂ ¿ŒΩ∫≈œΩ∫ ¥¯¡Ø Ω√Ω∫≈€

	SetDesc( index );
	SetItemExtendInfo(TRUE);
	ASSERT_ASSERT(index <= ITEMINFO_PARAMETER_NUMBER);
	SetMaxLength();
}
#ifdef BONUS_STAT_ITEM
void CINFItemInfo::SetDefenseItemInfo(BOOL bShop, BOOL Bonus)										  // 2013-06-26 by ssjung ¿Œ∫•≈‰∏Æ √ﬂ∞° ≈¯∆¡ 
{
	int index = 0;
	SetItemName(index++);
	SetItemKind(index++);

	// 2009. 10. 28 by ckPark Ω√∞£/±‚∞£ ¡¶«— æ∆¿Ã≈€ ≈¯∆¡ ºˆ¡§
	SetRemainTime_Imp(m_pRefItemInfo, &index, bShop);
	// end 2009. 10. 28 by ckPark Ω√∞£/±‚∞£ ¡¶«— æ∆¿Ã≈€ ≈¯∆¡ ºˆ¡§

	SetUnitKind(index++);
	SetReqLevel(index++);
	SetWeight(index++);

	// 2009. 08. 27 by ckPark ±◊∑°«» ∏Æº“Ω∫ ∫Ø∞Ê Ω√Ω∫≈€ ±∏«ˆ
	// 	if(m_pRefItemInfo != 0)
	// 		SetDefenseColorInfo(index++);

	if (m_pRefItemInfo)
		SetShapeInfo(index++);
	// end 2009. 08. 27 by ckPark ±◊∑°«» ∏Æº“Ω∫ ∫Ø∞Ê Ω√Ω∫≈€ ±∏«ˆ

	// 2008-09-26 by bhsohn Ω≈±‘ ¿Œ√æ∆Æ √≥∏Æ
	//SetParameter(&index);
	SetParameter(&index, FALSE, TRUE, 0.0f, Bonus);

	if (bShop == FALSE)
	{
		SetShopSellInfo(index++);
	}
	// 2007-09-07 by bhsohn æ∆¿Ã≈€ ∞≈∑°ø©∫Œ «•Ω√
	if (FALSE == bShop)
	{
		SetItemAttribute(index++);
	}

	// 2009. 11. 02 by ckPark ¿Œ««¥œ∆º « µÂ ¿ŒΩ∫≈œΩ∫ ¥¯¡Ø Ω√Ω∫≈€
	SetUseAreaInfinity(&index);
	SetExchangeMaterial(bShop);
	// end 2009. 11. 02 by ckPark ¿Œ««¥œ∆º « µÂ ¿ŒΩ∫≈œΩ∫ ¥¯¡Ø Ω√Ω∫≈€

	//	SetItemLimit(&index);		// 2012-10-13 by jhahn ±‚∞£¡¶ æ∆¿Ã≈€ √ﬂ∞°
	SetDesc(index);
	ASSERT_ASSERT(index <= ITEMINFO_PARAMETER_NUMBER);
	SetMaxLength();
}

#else
void CINFItemInfo::SetDefenseItemInfo(BOOL bShop)
{
	int index = 0;
	SetItemName( index++);
	SetItemKind( index++ );

	// 2009. 10. 28 by ckPark Ω√∞£/±‚∞£ ¡¶«— æ∆¿Ã≈€ ≈¯∆¡ ºˆ¡§
	SetRemainTime_Imp( m_pRefItemInfo, &index, bShop );
	// end 2009. 10. 28 by ckPark Ω√∞£/±‚∞£ ¡¶«— æ∆¿Ã≈€ ≈¯∆¡ ºˆ¡§
	
	SetUnitKind( index++);
	SetReqLevel( index++);
	SetWeight( index++);
	
	// 2009. 08. 27 by ckPark ±◊∑°«» ∏Æº“Ω∫ ∫Ø∞Ê Ω√Ω∫≈€ ±∏«ˆ
	// 	if(m_pRefItemInfo != 0)
	// 		SetDefenseColorInfo(index++);
	
	if( m_pRefItemInfo )
		SetShapeInfo( index++ );
	// end 2009. 08. 27 by ckPark ±◊∑°«» ∏Æº“Ω∫ ∫Ø∞Ê Ω√Ω∫≈€ ±∏«ˆ

	// 2008-09-26 by bhsohn Ω≈±‘ ¿Œ√æ∆Æ √≥∏Æ
	//SetParameter(&index);
	SetParameter(&index, FALSE, TRUE);

	if(bShop == FALSE)
	{
		SetShopSellInfo( index++ );
	}
	// 2007-09-07 by bhsohn æ∆¿Ã≈€ ∞≈∑°ø©∫Œ «•Ω√
	if(FALSE == bShop)
	{
		SetItemAttribute(index++);
	}

	// 2009. 11. 02 by ckPark ¿Œ««¥œ∆º « µÂ ¿ŒΩ∫≈œΩ∫ ¥¯¡Ø Ω√Ω∫≈€
	SetUseAreaInfinity( &index );
	SetExchangeMaterial( bShop );
	// end 2009. 11. 02 by ckPark ¿Œ««¥œ∆º « µÂ ¿ŒΩ∫≈œΩ∫ ¥¯¡Ø Ω√Ω∫≈€

	SetDesc( index );
	SetItemExtendInfo(TRUE);
	ASSERT_ASSERT(index <= ITEMINFO_PARAMETER_NUMBER);
	SetMaxLength();
}
#endif
void CINFItemInfo::SetSupportItemInfo(BOOL bShop)
{
	int index = 0;
	SetItemName( index++);

	// 2009. 10. 28 by ckPark Ω√∞£/±‚∞£ ¡¶«— æ∆¿Ã≈€ ≈¯∆¡ ºˆ¡§
	SetRemainTime_Imp( m_pRefItemInfo, &index, bShop );
	// end 2009. 10. 28 by ckPark Ω√∞£/±‚∞£ ¡¶«— æ∆¿Ã≈€ ≈¯∆¡ ºˆ¡§
	
	SetUnitKind( index++);
	SetReqLevel( index++);
	SetReqStat( index++);
	SetSpeed( index++ );
	SetBoosterSpeed( index++ );
	// 2006-10-18 by dgwoo A±‚æÓ ø£¡¯ø°∏∏ ¡ˆªÛ º”µµ∏¶ ∫∏ø©¡ÿ¥Ÿ.
	if(IS_DT(m_pRefITEM->ReqUnitKind))
		SetCharging(index++);
	SetBoosterTime( index++ );
	SetRotateAngle( index++ );
	SetBoosterRotateAngle( index++ );
	SetEnginTurningAngle(index++);							// 2005-08-17 by ispark
	SetWeight( index++);
	if(bShop == FALSE)
	{
		SetShopSellInfo( index++ );
		// 2007-09-07 by bhsohn æ∆¿Ã≈€ ∞≈∑°ø©∫Œ «•Ω√
		SetItemAttribute(index++);
	}

	// 2009. 11. 02 by ckPark ¿Œ««¥œ∆º « µÂ ¿ŒΩ∫≈œΩ∫ ¥¯¡Ø Ω√Ω∫≈€
	SetUseAreaInfinity( &index );
	SetExchangeMaterial( bShop );
	// end 2009. 11. 02 by ckPark ¿Œ««¥œ∆º « µÂ ¿ŒΩ∫≈œΩ∫ ¥¯¡Ø Ω√Ω∫≈€

	SetDesc( index );
	SetItemExtendInfo(TRUE);
	ASSERT_ASSERT(index <= ITEMINFO_PARAMETER_NUMBER);
	SetMaxLength();

}

void CINFItemInfo::SetEnergyItemInfo(BOOL bShop)
{
	int index = 0;
	SetItemName( index++ );

	// 2009. 10. 28 by ckPark Ω√∞£/±‚∞£ ¡¶«— æ∆¿Ã≈€ ≈¯∆¡ ºˆ¡§
	SetRemainTime_Imp( m_pRefItemInfo, &index, bShop );
	// end 2009. 10. 28 by ckPark Ω√∞£/±‚∞£ ¡¶«— æ∆¿Ã≈€ ≈¯∆¡ ºˆ¡§
	
	SetUnitKind( index++ );
	if(COMPARE_BIT_FLAG(m_pRefITEM->ItemAttribute, ITEM_ATTR_CASH_ITEM))
	{
		SetReqLevel( index++ );
	}
	SetCount( index++ );
	SetSkillReAttackTime( index++ );
	SetParameter(&index);
	
	//	if( IS_COUNTABLE_ITEM(m_pRefITEM->Kind) )
	//	{
//		SetCountableWeight( index++ );
//	}
//	else
//	{
//		SetWeight( index++ );
//	}
	if(bShop == FALSE)
	{
		SetShopSellInfo( index++ );
		// 2007-09-07 by bhsohn æ∆¿Ã≈€ ∞≈∑°ø©∫Œ «•Ω√
		SetItemAttribute(index++);
	}

	// 2009. 11. 02 by ckPark ¿Œ««¥œ∆º « µÂ ¿ŒΩ∫≈œΩ∫ ¥¯¡Ø Ω√Ω∫≈€
	SetUseAreaInfinity( &index );
	SetExchangeMaterial( bShop );
	// end 2009. 11. 02 by ckPark ¿Œ««¥œ∆º « µÂ ¿ŒΩ∫≈œΩ∫ ¥¯¡Ø Ω√Ω∫≈€

	SetDesc( index );
	SetItemExtendInfo(TRUE);
	ASSERT_ASSERT(index <= ITEMINFO_PARAMETER_NUMBER);
	SetMaxLength();

}

void CINFItemInfo::SetIngotItemInfo(BOOL bShop)
{
	int index = 0;
	SetItemName( index++ );
//	SetUnitKind( index++ );
	SetCount( index++ );

	// 2009. 10. 28 by ckPark Ω√∞£/±‚∞£ ¡¶«— æ∆¿Ã≈€ ≈¯∆¡ ºˆ¡§
	SetRemainTime_Imp( m_pRefItemInfo, &index, bShop );
	// end 2009. 10. 28 by ckPark Ω√∞£/±‚∞£ ¡¶«— æ∆¿Ã≈€ ≈¯∆¡ ºˆ¡§
	
	SetWeight( index++ );
	if(bShop == FALSE)
	{
		SetShopSellInfo( index++ );
		// 2007-09-07 by bhsohn æ∆¿Ã≈€ ∞≈∑°ø©∫Œ «•Ω√
		SetItemAttribute(index++);
	}

	// 2009. 11. 02 by ckPark ¿Œ««¥œ∆º « µÂ ¿ŒΩ∫≈œΩ∫ ¥¯¡Ø Ω√Ω∫≈€
	SetUseAreaInfinity( &index );
	SetExchangeMaterial( bShop );
	// end 2009. 11. 02 by ckPark ¿Œ««¥œ∆º « µÂ ¿ŒΩ∫≈œΩ∫ ¥¯¡Ø Ω√Ω∫≈€

	SetDesc( index );
	SetItemExtendInfo(TRUE);
	ASSERT_ASSERT(index <= ITEMINFO_PARAMETER_NUMBER);
	SetMaxLength();
}

void CINFItemInfo::SetCardItemInfo(BOOL bShop)
{
	auto index = 0;
	SetItemName( index++ );
	SetUnitKind( index++ );

	if( m_pRefITEM ) {
		if( !COMPARE_BIT_FLAG( m_pRefITEM->ItemAttribute, ITEM_ATTR_KILL_MARK_ITEM ) )
			SetReqLevel( index++ );
		else {
			if( g_pShuttleChild->m_myShuttleInfo.Level >= m_pRefITEM->ReqMinLevel )
				wsprintf( m_strItemInfo[index++], STRMSG_C_ITEM_0022, m_pRefITEM->ReqMinLevel);
			else
				wsprintf( m_strItemInfo[index++], STRMSG_C_ITEM_0025, m_pRefITEM->ReqMinLevel);
		}
	}

	SetCount( index++ );
	
	if( !SetRemainTime_Imp( m_pRefItemInfo, &index, bShop ) ) {

		if( m_pRefItemInfo && m_pRefItemInfo->GetRealItemInfo()  &&
			!m_pRefItemInfo->GetRealItemInfo()->IsExistDesParam( DES_FIELD_STORE ) &&
			!m_pRefItemInfo->GetRealItemInfo()->IsExistDesParam( DES_INCREASE_INVENTORY_SPACE ) &&
			!m_pRefItemInfo->GetRealItemInfo()->IsExistDesParam( DES_INCREASE_STORE_SPACE ) &&
			!m_pRefItemInfo->GetRealItemInfo()->IsExistDesParam( DES_CASH_GUILD_MEMBER_SUMMON ) //by Inetpub :: used for stack capsules
			)
			SetItemAllTime(index++, TRUE);
	}

	if( m_pRefITEM
		&& !m_pRefITEM->IsExistDesParam( DES_FIELD_STORE )
		&& !m_pRefITEM->IsExistDesParam( DES_INCREASE_INVENTORY_SPACE )
		&& !m_pRefITEM->IsExistDesParam( DES_INCREASE_STORE_SPACE ) 
		&& !m_pRefITEM->IsExistDesParam(DES_CASH_GUILD_MEMBER_SUMMON) //by Inetpub :: used for stack capsules
		)
		SetParameter(&index);			
	
	if(bShop == FALSE) {
		SetShopSellInfo( index++ );
		SetItemAttribute(index++);
	}

	SetUseAreaInfinity( &index );
	SetExchangeMaterial( bShop );

	SetDesc( index );
	SetItemExtendInfo(TRUE);
	ASSERT_ASSERT(index <= ITEMINFO_PARAMETER_NUMBER);
	SetMaxLength();
}

void CINFItemInfo::PrevetionDeleteItem(BOOL bShop)
{
	int index = 0;
	SetItemName( index++ );
	SetUnitKind( index++ );
	SetCount( index++ );

	// 2009. 10. 28 by ckPark Ω√∞£/±‚∞£ ¡¶«— æ∆¿Ã≈€ ≈¯∆¡ ºˆ¡§
	SetRemainTime_Imp( m_pRefItemInfo, &index, bShop );
	// end 2009. 10. 28 by ckPark Ω√∞£/±‚∞£ ¡¶«— æ∆¿Ã≈€ ≈¯∆¡ ºˆ¡§
	
	//	SetWeight( index++ );
	//	SetParameter(&index);
	
	
	// 2009. 06. 16 by ckpark ¿Œ√¶∆Æ»Æ∑¸¡ı∞°ƒ´µÂ, ∆ƒπÊƒ´µÂ ≈¯∆¡ πˆ±◊ ºˆ¡§
	
	// 	// 2009. 01. 21 by ckPark ¿Œ√¶∆Æ »Æ∑¸ ¡ı∞° ƒ´µÂ
	// 	//wsprintf( m_strItemInfo[index++], STRMSG_C_ITEM_0055, STRMSG_C_060424_0020); //"¿Œ√æ∆Æ Ω«∆–Ω√ æ∆¿Ã≈€ ∆ƒ±´ πÊ¡ˆ"
	// 
	// 	// DES_ENCHANT_INCREASE_PROBABILITY¿Ã ¡∏¿Á«—¥Ÿ∏È ¿Œ√¶∆Æ »Æ∑¸ ¡ı∞°∑Œ ¿ŒΩƒ æ∆¥œ∏È ∆ƒπÊ¿∏∑Œ ¿ŒΩƒz
	// 	if(m_pRefItemInfo->ItemInfo->GetParameterValue(DES_ENCHANT_INCREASE_PROBABILITY) != 0.0f)
// 		wsprintf( m_strItemInfo[index++], STRMSG_C_ITEM_0055, STRMSG_C_090123_0302); //"¿Œ√¶∆Æ »Æ∑¸ ¡ı∞°"
// 	else
// 		wsprintf( m_strItemInfo[index++], STRMSG_C_ITEM_0055, STRMSG_C_060424_0020); //"¿Œ√æ∆Æ Ω«∆–Ω√ æ∆¿Ã≈€ ∆ƒ±´ πÊ¡ˆ"
// 
// 	// end 2009. 01. 21 by ckPark ¿Œ√¶∆Æ »Æ∑¸ ¡ı∞° ƒ´µÂ
	
	if( m_pRefITEM )
	{
		if( m_pRefITEM->GetParameterValue( DES_ENCHANT_INCREASE_PROBABILITY ) != 0.0f )
			wsprintf( m_strItemInfo[index++], STRMSG_C_ITEM_0055, STRMSG_C_090123_0302 ); //"¿Œ√¶∆Æ »Æ∑¸ ¡ı∞°"
		else
			wsprintf( m_strItemInfo[index++], STRMSG_C_ITEM_0055, STRMSG_C_060424_0020 ); //"¿Œ√æ∆Æ Ω«∆–Ω√ æ∆¿Ã≈€ ∆ƒ±´ πÊ¡ˆ"
	}
	
	// end 2009. 06. 16 by ckpark ¿Œ√¶∆Æ»Æ∑¸¡ı∞°ƒ´µÂ, ∆ƒπÊƒ´µÂ ≈¯∆¡ πˆ±◊ ºˆ¡§


	// 2007-09-07 by bhsohn æ∆¿Ã≈€ ∞≈∑°ø©∫Œ «•Ω√
	if(FALSE  == bShop)
	{
		SetItemAttribute(index++);
	}

	// 2009. 11. 02 by ckPark ¿Œ««¥œ∆º « µÂ ¿ŒΩ∫≈œΩ∫ ¥¯¡Ø Ω√Ω∫≈€
	SetUseAreaInfinity( &index );
	SetExchangeMaterial( bShop );
	// end 2009. 11. 02 by ckPark ¿Œ««¥œ∆º « µÂ ¿ŒΩ∫≈œΩ∫ ¥¯¡Ø Ω√Ω∫≈€

	SetDesc( index );
	SetItemExtendInfo(TRUE);
	ASSERT_ASSERT(index <= ITEMINFO_PARAMETER_NUMBER);
	SetMaxLength();
}

void CINFItemInfo::SetArmorColorInfo(BOOL bShop)
{
	int index = 0;
	SetItemName( index++ );

	// 2009. 10. 28 by ckPark Ω√∞£/±‚∞£ ¡¶«— æ∆¿Ã≈€ ≈¯∆¡ ºˆ¡§
	SetRemainTime_Imp( m_pRefItemInfo, &index, bShop );
	// end 2009. 10. 28 by ckPark Ω√∞£/±‚∞£ ¡¶«— æ∆¿Ã≈€ ≈¯∆¡ ºˆ¡§
	
	SetUnitKind( index++ );
	SetReqLevel( index++ );
	// 2007-09-07 by bhsohn æ∆¿Ã≈€ ∞≈∑°ø©∫Œ «•Ω√
	if(FALSE == bShop)
	{
		SetItemAttribute(index++);
	}

	// 2009. 11. 02 by ckPark ¿Œ««¥œ∆º « µÂ ¿ŒΩ∫≈œΩ∫ ¥¯¡Ø Ω√Ω∫≈€
	SetUseAreaInfinity( &index );
	SetExchangeMaterial( bShop );
	// end 2009. 11. 02 by ckPark ¿Œ««¥œ∆º « µÂ ¿ŒΩ∫≈œΩ∫ ¥¯¡Ø Ω√Ω∫≈€

	SetDesc( index );
	SetItemExtendInfo(TRUE);
	SetMaxLength();
}

void CINFItemInfo::SetEnchantItemInfo(BOOL bShop)
{
	int index = 0;
	SetItemName( index++ );
	SetUnitKind( index++ );
	SetCount( index++ );
	if(m_pRefITEM->ReqItemKind != ITEMKIND_ALL_ITEM)
	{
		SetReqItemKind( index++ );
	}

	// 2009. 10. 28 by ckPark Ω√∞£/±‚∞£ ¡¶«— æ∆¿Ã≈€ ≈¯∆¡ ºˆ¡§
	//SetItemAllTime(index++, TRUE);

	if( !SetRemainTime_Imp( m_pRefItemInfo, &index, bShop ) )
	{
		if( m_pRefItemInfo )
			SetItemAllTime(index++, TRUE);
	}
	// end 2009. 10. 28 by ckPark Ω√∞£/±‚∞£ ¡¶«— æ∆¿Ã≈€ ≈¯∆¡ ºˆ¡§
	
	SetParameter(&index);

	// 2010. 02. 11 by ckPark πﬂµø∑˘ ¿Â¬¯æ∆¿Ã≈€
	SetInvokeDestParam( &index );
	// end 2010. 02. 11 by ckPark πﬂµø∑˘ ¿Â¬¯æ∆¿Ã≈€



	//	SetWeight( index++ );
	//	if(bShop == FALSE)
	//	{
	//		SetShopSellInfo( index++ );
	//	}
	// 2007-09-07 by bhsohn æ∆¿Ã≈€ ∞≈∑°ø©∫Œ «•Ω√
	if(FALSE == bShop)
	{
		SetItemAttribute(index++);
	}

	// 2009. 11. 02 by ckPark ¿Œ««¥œ∆º « µÂ ¿ŒΩ∫≈œΩ∫ ¥¯¡Ø Ω√Ω∫≈€
	SetUseAreaInfinity( &index );
	SetExchangeMaterial( bShop );
	// end 2009. 11. 02 by ckPark ¿Œ««¥œ∆º « µÂ ¿ŒΩ∫≈œΩ∫ ¥¯¡Ø Ω√Ω∫≈€

	SetDesc( index );
	SetItemExtendInfo(TRUE);
	ASSERT_ASSERT(index <= ITEMINFO_PARAMETER_NUMBER);
	SetMaxLength();
}
#ifdef BONUS_STAT_ITEM
void CINFItemInfo::SetAccessoryUnLimitItemInfo(BOOL bShop, BOOL Bonus)					 // 2013-06-26 by ssjung ¿Œ∫•≈‰∏Æ √ﬂ∞° ≈¯∆¡ 
{
	int index = 0;
	SetItemName(index++);
	SetUnitKind(index++);
	SetReqLevel(index++);

	// 2009. 10. 28 by ckPark Ω√∞£/±‚∞£ ¡¶«— æ∆¿Ã≈€ ≈¯∆¡ ºˆ¡§
	//SetItemAllTime(index++);
	//SetItemRemainTime(index++,bShop);

	if (!SetRemainTime_Imp(m_pRefItemInfo, &index, bShop))
	{
		if (m_pRefItemInfo)
			SetItemAllTime(index++);
	}
	// end 2009. 10. 28 by ckPark Ω√∞£/±‚∞£ ¡¶«— æ∆¿Ã≈€ ≈¯∆¡ ºˆ¡§

	SetParameter(&index, 0, 0, 0.0f, Bonus);

	// 2010. 02. 11 by ckPark πﬂµø∑˘ ¿Â¬¯æ∆¿Ã≈€
	// πﬂµø∑˘ µ•Ω∫∆ƒ∂˜ ±‚¥… ≈¯∆¡
	SetInvokeDestParam(&index);
	// æ∆¿Ã≈€ ƒ≈∏¿” ≈¯∆¡
	SetItemCoolTime(&index);
	// end 2010. 02. 11 by ckPark πﬂµø∑˘ ¿Â¬¯æ∆¿Ã≈€

	SetWeight(index++);
	// 2007-09-07 by bhsohn æ∆¿Ã≈€ ∞≈∑°ø©∫Œ «•Ω√
	if (FALSE == bShop)
	{
		SetItemAttribute(index++);
	}

	// 2009. 11. 02 by ckPark ¿Œ««¥œ∆º « µÂ ¿ŒΩ∫≈œΩ∫ ¥¯¡Ø Ω√Ω∫≈€
	SetUseAreaInfinity(&index);
	SetExchangeMaterial(bShop);
	// end 2009. 11. 02 by ckPark ¿Œ««¥œ∆º « µÂ ¿ŒΩ∫≈œΩ∫ ¥¯¡Ø Ω√Ω∫≈€

	SetDesc(index);
	ASSERT_ASSERT(index <= ITEMINFO_PARAMETER_NUMBER);
	SetMaxLength();

}
#else
void CINFItemInfo::SetAccessoryUnLimitItemInfo(BOOL bShop)
{
	int index = 0;
	SetItemName( index++ );
	SetUnitKind( index++ );
	SetReqLevel(index++);
		
	// 2009. 10. 28 by ckPark Ω√∞£/±‚∞£ ¡¶«— æ∆¿Ã≈€ ≈¯∆¡ ºˆ¡§
	//SetItemAllTime(index++);
	//SetItemRemainTime(index++,bShop);

	if( !SetRemainTime_Imp( m_pRefItemInfo, &index, bShop ) )
	{
		if( m_pRefItemInfo )
			SetItemAllTime(index++);
	}
	// end 2009. 10. 28 by ckPark Ω√∞£/±‚∞£ ¡¶«— æ∆¿Ã≈€ ≈¯∆¡ ºˆ¡§
	
	SetParameter(&index);

	// 2010. 02. 11 by ckPark πﬂµø∑˘ ¿Â¬¯æ∆¿Ã≈€
	// πﬂµø∑˘ µ•Ω∫∆ƒ∂˜ ±‚¥… ≈¯∆¡
	SetInvokeDestParam( &index );
	// æ∆¿Ã≈€ ƒ≈∏¿” ≈¯∆¡
	SetItemCoolTime( &index );
	// end 2010. 02. 11 by ckPark πﬂµø∑˘ ¿Â¬¯æ∆¿Ã≈€
	
	SetWeight( index++ );
	// 2007-09-07 by bhsohn æ∆¿Ã≈€ ∞≈∑°ø©∫Œ «•Ω√
	if(FALSE == bShop)
	{
		SetItemAttribute(index++);
	}

	// 2009. 11. 02 by ckPark ¿Œ««¥œ∆º « µÂ ¿ŒΩ∫≈œΩ∫ ¥¯¡Ø Ω√Ω∫≈€
	SetUseAreaInfinity( &index );
	SetExchangeMaterial( bShop );
	// end 2009. 11. 02 by ckPark ¿Œ««¥œ∆º « µÂ ¿ŒΩ∫≈œΩ∫ ¥¯¡Ø Ω√Ω∫≈€

	SetDesc( index );
	SetItemExtendInfo(TRUE);
	ASSERT_ASSERT(index <= ITEMINFO_PARAMETER_NUMBER);
	SetMaxLength();

}
#endif
#ifdef _INET_PET
void CINFItemInfo::SetPetSoketItemInfo(BOOL bShop, BOOL bInven)							// 2013-06-26 by ssjung ¿Œ∫•≈‰∏Æ √ﬂ∞° ≈¯∆¡ 
{
	int index = 0;
	SetItemName(index++);
	SetUnitKind(index++);

	// 2009. 10. 28 by ckPark Ω√∞£/±‚∞£ ¡¶«— æ∆¿Ã≈€ ≈¯∆¡ ºˆ¡§
	SetItemAllTime(index++);
	SetItemRemainTime(index++,bShop);

	//if (!SetRemainTime_Imp(m_pRefITEM, &index, bShop)) // 2013-07-05 by ssjung ¡∂«’¡§∫∏ ≈©∑°Ω¨ ∞¸∑√ (¿Œ¿⁄∞™¿ª CItemInfo*ø°º≠ ITEM*¿∏∑Œ πŸ≤ﬁ)
	//{
	//	if (m_pRefITEM)
	//		SetItemAllTime(index++);
	//}
	// end 2009. 10. 28 by ckPark Ω√∞£/±‚∞£ ¡¶«— æ∆¿Ã≈€ ≈¯∆¡ ºˆ¡§

	//	SetParameter(&index);

	SetWeight(index++);
	// 2007-09-07 by bhsohn æ∆¿Ã≈€ ∞≈∑°ø©∫Œ «•Ω√
	if (FALSE == bShop)
	{
		SetItemAttribute(index++);
	}

	// 2009. 11. 02 by ckPark ¿Œ««¥œ∆º « µÂ ¿ŒΩ∫≈œΩ∫ ¥¯¡Ø Ω√Ω∫≈€
	SetUseAreaInfinity(&index);
	SetExchangeMaterial(bShop);
	// end 2009. 11. 02 by ckPark ¿Œ««¥œ∆º « µÂ ¿ŒΩ∫≈œΩ∫ ¥¯¡Ø Ω√Ω∫≈€

	SetDesc(index);
	SetItemExtendInfo(TRUE);
//	SetItemExtendInfo(bInven, FALSE);																			// 2013-06-26 by ssjung ¿Œ∫•≈‰∏Æ √ﬂ∞° ≈¯∆¡ 
	ASSERT_ASSERT(index <= ITEMINFO_PARAMETER_NUMBER);
	SetMaxLength();
}
#endif
void CINFItemInfo::SetBulletItemInfo(BOOL bShop)
{
	int index = 0;
	SetItemName( index++ );

	// 2009. 10. 28 by ckPark Ω√∞£/±‚∞£ ¡¶«— æ∆¿Ã≈€ ≈¯∆¡ ºˆ¡§
	SetRemainTime_Imp( m_pRefItemInfo, &index, bShop );
	// end 2009. 10. 28 by ckPark Ω√∞£/±‚∞£ ¡¶«— æ∆¿Ã≈€ ≈¯∆¡ ºˆ¡§
	
	SetUnitKind( index++ );
	SetBullet( index++ );
	SetCountableWeight( index++ );
	if(bShop == FALSE)
	{
		SetShopSellInfo( index++ );
	}
	// 2007-09-07 by bhsohn æ∆¿Ã≈€ ∞≈∑°ø©∫Œ «•Ω√
	if(FALSE == bShop)
	{
		SetItemAttribute(index++);
	}

	// 2009. 11. 02 by ckPark ¿Œ««¥œ∆º « µÂ ¿ŒΩ∫≈œΩ∫ ¥¯¡Ø Ω√Ω∫≈€
	SetUseAreaInfinity( &index );
	SetExchangeMaterial( bShop );
	// end 2009. 11. 02 by ckPark ¿Œ««¥œ∆º « µÂ ¿ŒΩ∫≈œΩ∫ ¥¯¡Ø Ω√Ω∫≈€

	SetDesc( index );
	SetItemExtendInfo(TRUE);
	ASSERT_ASSERT(index <= ITEMINFO_PARAMETER_NUMBER);
	SetMaxLength();
}

void CINFItemInfo::SetComputerItemInfo(BOOL bShop)
{
	int index = 0;
	SetItemName( index++ );

	// 2009. 10. 28 by ckPark Ω√∞£/±‚∞£ ¡¶«— æ∆¿Ã≈€ ≈¯∆¡ ºˆ¡§
	SetRemainTime_Imp( m_pRefItemInfo, &index, bShop );
	// end 2009. 10. 28 by ckPark Ω√∞£/±‚∞£ ¡¶«— æ∆¿Ã≈€ ≈¯∆¡ ºˆ¡§
	
	SetUnitKind( index++ );
	SetReqLevel( index++ );
	SetParameter(&index);
	SetWeight( index++ );
	if(bShop == FALSE)
	{
		SetShopSellInfo( index++ );
		// 2007-09-07 by bhsohn æ∆¿Ã≈€ ∞≈∑°ø©∫Œ «•Ω√
		SetItemAttribute(index++);
	}

	// 2009. 11. 02 by ckPark ¿Œ««¥œ∆º « µÂ ¿ŒΩ∫≈œΩ∫ ¥¯¡Ø Ω√Ω∫≈€
	SetUseAreaInfinity( &index );
	SetExchangeMaterial( bShop );
	// end 2009. 11. 02 by ckPark ¿Œ««¥œ∆º « µÂ ¿ŒΩ∫≈œΩ∫ ¥¯¡Ø Ω√Ω∫≈€

	SetDesc( index );
	SetItemExtendInfo(TRUE);
	ASSERT_ASSERT(index <= ITEMINFO_PARAMETER_NUMBER);
	SetMaxLength();
}

void CINFItemInfo::SetMaxLength()
{
	// 2006-03-08 by ispark, ∆˘∆Æ ±Ê¿Ã ∞ËªÍ πŸ≤ﬁ
	FLOG( "CINFItemInfo::SetMaxLength()" );
	int len=0; 
	m_nMaxLength=0;
	int i;
	for(i=0;i<ITEMINFO_PARAMETER_NUMBER;i++)
	{
		len = m_pFontItemInfo[0]->GetStringSize(m_strItemInfo[i]).cx;
//		len = strlen(m_strItemInfo[i]);
		if(m_nMaxLength < len) {
			m_nMaxLength = len; }
	}
	for(i=0;i<ITEMINFO_DESC_LINE_NUMBER;i++)
	{
		len = m_pFontDescInfo[0]->GetStringSize(m_strDesc[i]).cx;
//		len = strlen(m_strDesc[i]);
		if(m_nMaxLength < len) {
			m_nMaxLength = len; }
	}

	// 2009-02-03 by bhsohn ¿Â¬¯ æ∆¿Ã≈€ ∫Ò±≥ ≈¯∆¡	
	m_szTooltip.cx = m_nMaxLength+12;

	int nIcongab = 0;
	if (m_nBigIconNum > 0)
	{
		nIcongab = ITEMINFO_BIGICON_GAB;		
	}			
	m_szTooltip.cy = 14 * (m_nDescIndex + 1) + 14 * (m_nDescLine + m_nExtendItemIndex) + 20 + nIcongab;
	// end 2009-02-03 by bhsohn ¿Â¬¯ æ∆¿Ã≈€ ∫Ò±≥ ≈¯∆¡

	// 2009. 11. 02 by ckPark ¿Œ««¥œ∆º « µÂ ¿ŒΩ∫≈œΩ∫ ¥¯¡Ø Ω√Ω∫≈€
	if( !m_vecExchageMtrl.empty() )
		m_szTooltip.cy += 30;
	// end 2009. 11. 02 by ckPark ¿Œ««¥œ∆º « µÂ ¿ŒΩ∫≈œΩ∫ ¥¯¡Ø Ω√Ω∫≈€
}
#ifdef BONUS_STAT_ITEM

void CINFItemInfo::SetItemInfoWithBonus(CItemInfo* pItemInfo, int x, int y, int nLinkItem, BOOL bInven)
{

	m_ptItemInfo.x = x;
	m_ptItemInfo.y = y;

	if (m_pBigIcon)
	{
		m_pBigIcon->InvalidateDeviceObjects();
		m_pBigIcon->DeleteDeviceObjects();
		SAFE_DELETE(m_pBigIcon);
	}

	if (pItemInfo)
	{
		if (!pItemInfo->ShapeItemNum)
		{
			if (!nLinkItem)
			{
				m_pBigIcon = FindBigIcon(pItemInfo->ItemInfo->SourceIndex);
			}
			else
			{
				m_pBigIcon = FindBigIcon(nLinkItem);
			}
		}
		else
		{
			ITEM* pShapeItem = g_pDatabase->GetServerItemInfo(pItemInfo->ShapeItemNum);
			if (pShapeItem)
				m_pBigIcon = FindBigIcon(pShapeItem->SourceIndex);
			else
				m_pBigIcon = FindBigIcon(pItemInfo->ItemInfo->SourceIndex);
		}

		m_pRefItemInfo = pItemInfo;
		m_pRefITEM = pItemInfo->GetItemInfo();

		m_bShow = TRUE;
		m_vecTickFuntionIndex.clear();
		if (IS_PRIMARY_WEAPON(m_pRefITEM->Kind))
		{
			SetPrimaryWeaponInfo(FALSE, TRUE);
		}
		else if (IS_SECONDARY_WEAPON(m_pRefITEM->Kind))
		{
			SetSecondaryWeaponInfo(FALSE, TRUE);
		}
		else if (ITEMKIND_DEFENSE == m_pRefITEM->Kind)
		{
			SetDefenseItemInfo(FALSE, TRUE);
		}
		/*		else if (ITEMKIND_SUPPORT == m_pRefITEM->Kind)
		{
		SetSupportItemInfo(FALSE, bInven);
		}
		else if (ITEMKIND_RADAR == m_pRefITEM->Kind)
		{
		SetRadarItemInfo(FALSE, bInven);
		}
		else if (ITEMKIND_COMPUTER == m_pRefITEM->Kind)
		{
		SetComputerItemInfo(FALSE, bInven);
		}*/
		else if (ITEMKIND_ACCESSORY_UNLIMITED == m_pRefITEM->Kind)
		{
			SetAccessoryUnLimitItemInfo(FALSE, TRUE);
		}


		if (m_ptItemInfo.x > g_pD3dApp->GetBackBufferDesc().Width - (m_nMaxLength + 12))
		{
			m_ptItemInfo.x = g_pD3dApp->GetBackBufferDesc().Width - (m_nMaxLength + 12);
		}
		if (m_ptItemInfo.y > g_pD3dApp->GetBackBufferDesc().Height - (14 * (m_nDescIndex + 1) + 14 * (m_nDescLine + 1) + 20))
		{
			m_ptItemInfo.y = g_pD3dApp->GetBackBufferDesc().Height - (14 * (m_nDescIndex + 1) + 14 * (m_nDescLine + 1) + 20);
		}
		ItemWithBonus.push_back(m_pRefItemInfo->UniqueNumber);
	}
	else
	{
		InitItemInfo();
	}

}

#endif
///////////////////////////////////////////////////////////////////////////////
/// \fn			CINFItemInfo::SetItemInfoUser( CItemInfo* pItemInfo, int x, int y )
/// \brief		æ∆¿Ã≈€¿« ¡§∫∏∏¶ ºº∆√«—¥Ÿ.
/// \author		dhkwon
/// \date		2004-04-20 ~ 2004-04-20
/// \warning	ªÛ¡°¿Œ∞ÊøÏ pItem¿Ã NULL¿Ã¥Ÿ.
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CINFItemInfo::SetItemInfoUser( CItemInfo* pItemInfo, int x, int y )
{
	m_ptItemInfo.x = x;
	m_ptItemInfo.y = y;// + ITEMINFO_BIGICON_GAB;

	/*if (m_nBigIconNum > 0)
	{
//		m_ptItemInfo.y = m_ptItemInfo.y + ITEMINFO_BIGICON_GAB;
		m_pBigIcon->InvalidateDeviceObjects();
		m_pBigIcon->DeleteDeviceObjects();
		SAFE_DELETE(m_pBigIcon);
	}*/

	if(pItemInfo)
	{
		
		// 2009. 08. 27 by ckPark ±◊∑°«» ∏Æº“Ω∫ ∫Ø∞Ê Ω√Ω∫≈€ ±∏«ˆ
		
		//m_pBigIcon = FindBigIcon(pItemInfo->ItemInfo->SourceIndex );			// 2005-08-23 by ispark
		if( !pItemInfo->ShapeItemNum )
			m_nBigIconNum = pItemInfo->ItemInfo->SourceIndex;
		else
		{
			ITEM* pShapeItem = g_pDatabase->GetServerItemInfo( pItemInfo->ShapeItemNum );
			if( pShapeItem )
				m_nBigIconNum = pShapeItem->SourceIndex;
			else
				m_nBigIconNum = pItemInfo->ItemInfo->SourceIndex;
		}
		if (m_nBigIconNum > 0 && !g_pGameMain->m_pBigIcon->Exists(m_nBigIconNum))
		{
			// set to 0 if the icon not exists
			m_nBigIconNum = 0;
		}
		// end 2009. 08. 27 by ckPark ±◊∑°«» ∏Æº“Ω∫ ∫Ø∞Ê Ω√Ω∫≈€ ±∏«ˆ

		m_pRefItemInfo = pItemInfo;
		m_pRefITEM = pItemInfo->GetItemInfo();
		m_pRefPrefixRareInfo = m_pRefItemInfo->GetPrefixRareInfo();
		m_pRefSuffixRareInfo = m_pRefItemInfo->GetSuffixRareInfo();
#ifdef BONUS_STAT_ITEM
		m_pRefBonus = m_pRefItemInfo->GetBonusParamFactor();
#endif
		m_pRefEnchant = m_pRefItemInfo->GetEnchantParamFactor();
		m_bEnableItem = IsEnableItem( m_pRefItemInfo->GetRealItemInfo() );
		m_bShow = TRUE;
		m_vecTickFuntionIndex.clear();
		if(IS_PRIMARY_WEAPON(m_pRefITEM->Kind))
		{
			SetPrimaryWeaponInfo();
		}
		else if(IS_SECONDARY_WEAPON(m_pRefITEM->Kind))
		{
			SetSecondaryWeaponInfo();
		}
		else if(IS_SKILL_ITEM(m_pRefITEM->Kind))
		{
			SetSkillItemInfo();
		}
		else if(ITEMKIND_DEFENSE == m_pRefITEM->Kind)
		{
			SetDefenseItemInfo();
		}
		else if(ITEMKIND_SUPPORT == m_pRefITEM->Kind)
		{
			SetSupportItemInfo();
		}
		else if(ITEMKIND_ENERGY == m_pRefITEM->Kind)
		{
			SetEnergyItemInfo();
		}
		else if(ITEMKIND_INGOT == m_pRefITEM->Kind)
		{
			SetIngotItemInfo();
		}
#ifdef BONUS_STAT_ITEM
		else if (ITEMKIND_BONUS_ALL_REMOVAL == m_pRefITEM->Kind)
		{
			SetIngotItemInfo();
		}
		else if (ITEMKIND_BONUS_RANDOM_REMOVAL == m_pRefITEM->Kind)
		{
			SetIngotItemInfo();
		} 
#endif
		else if(ITEMKIND_CARD == m_pRefITEM->Kind)
		{
			SetCardItemInfo();
		}
		else if(ITEMKIND_ENCHANT == m_pRefITEM->Kind || ITEMKIND_GAMBLE == m_pRefITEM->Kind)
		{
			SetEnchantItemInfo();
		}
		else if(ITEMKIND_TANK == m_pRefITEM->Kind)
		{
			SetAccessoryUnLimitItemInfo();
		}
		else if(ITEMKIND_BULLET == m_pRefITEM->Kind)
		{
			SetBulletItemInfo();
		}
		else if(ITEMKIND_QUEST == m_pRefITEM->Kind)
		{
			SetIngotItemInfo();
		}
		else if(ITEMKIND_RADAR == m_pRefITEM->Kind)
		{
			// 2005-08-17 by ispark
			SetRadarItemInfo();
//			SetIngotItemInfo();
			//SetEnchantItemInfo();
		}
		else if(ITEMKIND_COMPUTER == m_pRefITEM->Kind)
		{
			SetComputerItemInfo();
			//SetEnchantItemInfo();
		}
		else if(ITEMKIND_PREVENTION_DELETE_ITEM == m_pRefITEM->Kind)
		{
			PrevetionDeleteItem();
			//SetEnchantItemInfo();
		}
		else if(ITEMKIND_COLOR_ITEM == m_pRefITEM->Kind)
		{
			SetArmorColorInfo();
			//SetEnchantItemInfo();
		}
		// 2006-03-30 by ispark æ«ººªÁ∏Æ √ﬂ∞° (ex. ¿Ø∑· æ∆¿Ã≈€)
		else if(ITEMKIND_ACCESSORY_UNLIMITED == m_pRefITEM->Kind)
		{
			SetAccessoryUnLimitItemInfo();
		}
		else if(ITEMKIND_ACCESSORY_TIMELIMIT == m_pRefITEM->Kind)
		{
			SetAccessoryTimeLimitItemInfo();
		}
		else if(ITEMKIND_INFLUENCE_BUFF == m_pRefITEM->Kind)
		{
			SetInfluenceBuffItemInfo();
		}
		else if(ITEMKIND_INFLUENCE_GAMEEVENT == m_pRefITEM->Kind)
		{
			SetInfluenceGameEventItemInfo();
		}
		else if(ITEMKIND_RANDOMBOX == m_pRefITEM->Kind)
		{
			SetRandomBoxItemInfo();
		}
		else if(ITEMKIND_MARK == m_pRefITEM->Kind)
		{
			SetMarkItemInfo();
		}
		else if(ITEMKIND_SKILL_SUPPORT_ITEM == m_pRefITEM->Kind)
		{
			SetSkillSupportItem();
		}
		else if ( ITEMKIND_PET_ITEM == m_pRefITEM->Kind )
		{	// 2010-06-15 by shcho&hslee ∆ÍΩ√Ω∫≈€
			SetPetItemInfo();
		}

		/*
		if(COMPARE_RACE(g_pShuttleChild->m_myShuttleInfo.Race,RACE_OPERATION|RACE_GAMEMASTER) && m_pRefItemInfo)
		{
			wsprintf( m_strItemInfo[0], "%s (%I64d)", m_strItemInfo[0], m_pRefItemInfo->UniqueNumber );
		}
		*/
#ifdef _INET_PET
		else if (ITEMKIND_PET_SOCKET_ITEM == m_pRefITEM->Kind)
		{	// 2010-06-15 by shcho&hslee ∆ÍΩ√Ω∫≈€
			SetPetSoketItemInfo(FALSE, FALSE);																			// 2013-06-26 by ssjung ¿Œ∫•≈‰∏Æ √ﬂ∞° ≈¯∆¡ 
		}
#endif
		if( m_ptItemInfo.x > g_pD3dApp->GetBackBufferDesc().Width - (m_nMaxLength+12) )
		{
			m_ptItemInfo.x = g_pD3dApp->GetBackBufferDesc().Width - (m_nMaxLength+12);
		}
		if( m_ptItemInfo.y > g_pD3dApp->GetBackBufferDesc().Height - (14*(m_nDescIndex+1)+14*(m_nDescLine+1)+20) )
		{
			m_ptItemInfo.y = g_pD3dApp->GetBackBufferDesc().Height - (14*(m_nDescIndex+1)+14*(m_nDescLine+1)+20);
		}
#ifdef BONUS_STAT_ITEM //added by inet
		ItemWithBonusOver.push_back(m_pRefItemInfo->UniqueNumber);
#endif
	}
	else
	{
		InitItemInfo();
	}
	
}
void CINFItemInfo::SetItemInfoNormal( ITEM* pITEM, int x, int y, BOOL bShop, int nItemCount )//(ITEM_GENERAL* pItem, ITEM* pItemInfo,ITEM_ENCHANT* pEnchant, int x, int y)
{
	m_ptItemInfo.x = x;
	m_ptItemInfo.y = y;// + ITEMINFO_BIGICON_GAB;
	
//	if(m_pBigIcon)
//	{
////		m_ptItemInfo.y = m_ptItemInfo.y + ITEMINFO_BIGICON_GAB;
//		m_pBigIcon->InvalidateDeviceObjects();
//		m_pBigIcon->DeleteDeviceObjects();
//		SAFE_DELETE(m_pBigIcon);
//	}

	if(pITEM)
	{
		m_nBigIconNum = pITEM->SourceIndex;
		if (m_nBigIconNum > 0 && !g_pGameMain->m_pBigIcon->Exists(m_nBigIconNum))
		{
			// set to 0 if the icon not exists
			m_nBigIconNum = 0;
		}
		m_nOtherItemCount = nItemCount;							// 2006-03-14 by ispark
		m_pRefItemInfo = NULL;
#ifdef BONUS_STAT_ITEM
		m_pRefBonus = NULL;
#endif
		m_pRefEnchant = NULL;
		m_pRefPrefixRareInfo = NULL;
		m_pRefSuffixRareInfo = NULL;
		m_pRefITEM = pITEM;
		m_bEnableItem = IsEnableItem( pITEM );
		m_bShow = TRUE;
		m_vecTickFuntionIndex.clear();
		if(IS_PRIMARY_WEAPON(pITEM->Kind))
		{
			SetPrimaryWeaponInfo(bShop);
		}
		else if(IS_SECONDARY_WEAPON(pITEM->Kind))
		{
			SetSecondaryWeaponInfo(bShop);
		}
		else if(IS_SKILL_ITEM(pITEM->Kind))
		{
			SetSkillItemInfo(bShop);
		}
		else if(ITEMKIND_DEFENSE == pITEM->Kind)
		{
			SetDefenseItemInfo(bShop);
		}
		else if(ITEMKIND_SUPPORT == pITEM->Kind)
		{
			SetSupportItemInfo(bShop);
		}
		else if(ITEMKIND_ENERGY == pITEM->Kind)
		{
			SetEnergyItemInfo(bShop);
		}
		else if(ITEMKIND_INGOT == pITEM->Kind)
		{
			SetIngotItemInfo(bShop);
		}
		else if(ITEMKIND_CARD == pITEM->Kind)
		{
			SetCardItemInfo(bShop);
		}
		else if(ITEMKIND_ENCHANT == pITEM->Kind || ITEMKIND_GAMBLE == pITEM->Kind)
		{
			SetEnchantItemInfo(bShop);
		}
		else if(ITEMKIND_TANK == pITEM->Kind)
		{
			SetAccessoryUnLimitItemInfo(bShop);
		}
		else if(ITEMKIND_BULLET == pITEM->Kind)
		{
			SetBulletItemInfo(bShop);
		}
		else if(ITEMKIND_QUEST == m_pRefITEM->Kind)
		{
			SetIngotItemInfo(bShop);
			//SetEnchantItemInfo();
		}
		else if(ITEMKIND_RADAR == m_pRefITEM->Kind)
		{
			SetRadarItemInfo(bShop);
			//SetEnchantItemInfo();
		}
		else if(ITEMKIND_COMPUTER == m_pRefITEM->Kind)
		{
			SetComputerItemInfo(bShop);
			//SetEnchantItemInfo();
		}
		else if(ITEMKIND_PREVENTION_DELETE_ITEM == m_pRefITEM->Kind)
		{
			PrevetionDeleteItem(bShop);
		}
		else if(ITEMKIND_COLOR_ITEM == m_pRefITEM->Kind)
		{
			SetArmorColorInfo(bShop);
		}
		// 2006-03-30 by ispark æ«ººªÁ∏Æ √ﬂ∞° (ex. ¿Ø∑· æ∆¿Ã≈€)
		else if(ITEMKIND_ACCESSORY_UNLIMITED == m_pRefITEM->Kind)
		{
			SetAccessoryUnLimitItemInfo(bShop);
		}
		else if(ITEMKIND_ACCESSORY_TIMELIMIT == m_pRefITEM->Kind)
		{
			SetAccessoryTimeLimitItemInfo(bShop);
		}
		else if(ITEMKIND_INFLUENCE_BUFF == m_pRefITEM->Kind)
		{
			SetInfluenceBuffItemInfo(bShop);
		}
		else if(ITEMKIND_INFLUENCE_GAMEEVENT == m_pRefITEM->Kind)
		{
			SetInfluenceGameEventItemInfo(bShop);
		}
		else if(ITEMKIND_RANDOMBOX == m_pRefITEM->Kind)
		{
			SetRandomBoxItemInfo(bShop);
		}
		else if(ITEMKIND_MARK == m_pRefITEM->Kind)
		{
			SetMarkItemInfo();
		}
		else if(ITEMKIND_SKILL_SUPPORT_ITEM == pITEM->Kind)
		{
			SetSkillSupportItem(bShop);
		}
		else if(ITEMKIND_PET_ITEM == m_pRefITEM->Kind)
		{
			SetPetItemInfo(bShop);
		}
#ifdef _INET_PET
		else if (ITEMKIND_PET_SOCKET_ITEM == m_pRefITEM->Kind)
		{
			SetPetSoketItemInfo();
		}
#endif
	}
	else
	{
		InitItemInfo();
	}

}
// 2010. 04. 21 by jskim Ω≈±‘ ∑∞≈∞ ∏”Ω≈ ±∏«ˆ
void CINFItemInfo::SetRareParameterInfo(char* str, float fValue)
{
	char  temp[256];
	wsprintf(temp,"[%.2f]",fValue);
	sprintf(str,"\\g%s\\g", temp);
}
//end 2010. 04. 21 by jskim Ω≈±‘ ∑∞≈∞ ∏”Ω≈ ±∏«ˆ

// 2010. 02. 11 by ckPark πﬂµø∑˘ ¿Â¬¯æ∆¿Ã≈€
//void CINFItemInfo::SetParameterInfo(char * str, BYTE bType,float fValue)
#ifdef BONUS_STAT_ITEM
BOOL CINFItemInfo::SingofDecimalDecision(float fVal)
{
	int nTemp = fVal * 100;
	nTemp = nTemp % 100;
	if (0 < nTemp) return TRUE;
	else return FALSE;
}
void CINFItemInfo::SetParameterInfo(char * str, DestParam_t bType, float fValue, BOOL bDefEnchant ,  BOOL Bonus)
{
	FLOG("CINFItemInfo::SetParameterInfo(char * str, BYTE bType,float fValue)");
	char buf[128];
	FUNCTION_VALUE_TYPE nValueType = FUNCTION_VALUE_TYPE_NORMAL;
	memset(buf, 0x00, sizeof(buf));
	switch (bType)
	{
	case DES_NULL:					// ¥ÎªÛ ∆ƒ∂ÛπÃ≈Õ∞° æ¯¥¬ ∞ÊøÏ ªÁøÎ
	{
		sprintf(str, STRMSG_C_PARAM_0001);//"¡§∫∏ æ¯¿Ω"
		return;
	}
	break;
	case DES_ATTACK_PART:			// ∞¯∞›
		sprintf(buf, STRMSG_C_PARAM_0002);//"[∞¯∞›"
		break;
	case DES_DEFENSE_PART:		// ≥ª±∏-->πÊæÓ
		sprintf(buf, STRMSG_C_PARAM_0003);//"[πÊæÓ"
		break;
	case DES_FUEL_PART:				// ø¨∑·
		sprintf(buf, STRMSG_C_PARAM_0004);//"[ø¨∑·"
		break;
	case DES_SOUL_PART:				// ∞®¿¿
		sprintf(buf, STRMSG_C_PARAM_0005);//"[¡§Ω≈"
		break;
	case DES_SHIELD_PART:			// πÊæÓ-->ΩØµÂ
		sprintf(buf, STRMSG_C_PARAM_0006);//"[ΩØµÂ"
		break;
	case DES_DODGE_PART:			// »∏««
		sprintf(buf, STRMSG_C_PARAM_0007);//"[»∏««"
		break;
	case DES_BODYCONDITION:			// ∏ˆªÛ≈¬(Ω√¡Ó∏µÂ,∆¯∞›∏µÂ)
		sprintf(str, STRMSG_C_PARAM_0008);//"[¿Ø¥÷ªÛ≈¬∫Ø»≠]"
		return;
	case DES_ENDURANCE_01: 			// ≥ª±∏µµ 01
		sprintf(buf, STRMSG_C_PARAM_0009);//"[±‚∫ªπ´±‚ ≥ª±∏µµ"
		break;
	case DES_ENDURANCE_02:			// ≥ª±∏µµ 02
		sprintf(buf, STRMSG_C_PARAM_0009);//"[±‚∫ªπ´±‚ ≥ª±∏µµ"
		break;
	case DES_CHARGING_01:			// ¿Â≈∫ºˆ 01
		sprintf(buf, STRMSG_C_PARAM_0010);//"[±‚∫ªπ´±‚ ¿Â≈∫ºˆ"
		break;
	case DES_CHARGING_02:			// ¿Â≈∫ºˆ 02
		sprintf(buf, STRMSG_C_PARAM_0011);//"[∞Ì±ﬁπ´±‚ ¿Â≈∫ºˆ"
		break;
	case DES_PROPENSITY:			// º∫«‚
		sprintf(buf, STRMSG_C_PARAM_0012);//"[º∫«‚"
		break;
	case DES_HP:					// »˜∆Æ∆˜¿Œ∆Æ
		sprintf(buf, STRMSG_C_PARAM_0013);//"[ø°≥ ¡ˆ"
		break;
	case DES_MAX_SP_UP:					// 2010-08-26 by shcho&&jskim, π„ æ∆¿Ã≈€ ±∏«ˆ
	case DES_SP:					// º“øÔ∆˜¿Œ∆Æ
		sprintf(buf, STRMSG_C_PARAM_0014);//"[Ω∫≈≥∆˜¿Œ∆Æ"
		break;
	case DES_EP:					// ø£¡¯∆˜¿Œ∆Æ
		sprintf(buf, STRMSG_C_PARAM_0004);//"[ø¨∑·"
		break;
	case DES_DP:					// ø£¡¯∆˜¿Œ∆Æ
		//sprintf(buf, STRMSG_C_PARAM_0006);//"[ΩØµÂ"
		sprintf(buf, STRMSG_C_PARAM_0006);//"[ΩØµÂ"	// 2013-01-23 by mspark, ƒ≥≥™¥Ÿ CPU ΩØµÂ Ω∫≈», ±‚æÓ ΩØµÂ∑Æ ±∏∫–
		break;
	case DES_SPRECOVERY:			// º“øÔ∆˜¿Œ∆Æ»∏∫π∑¬			C
		sprintf(buf, STRMSG_C_PARAM_0015);//"[Ω∫≈≥∆˜¿Œ∆Æ»∏∫π∑¬"
		break;
	case DES_HPRECOVERY:			// ø°≥ ¡ˆ∆˜¿Œ∆Æ»∏∫π∑¬		C
		sprintf(buf, STRMSG_C_PARAM_0016);//"[ø°≥ ¡ˆ»∏∫π∑¬"
		break;
	case DES_MINATTACK_01:				//  ∞¯∞›∑¬ 1«¸			C
		sprintf(buf, STRMSG_C_PARAM_0017);//"[±‚∫ªπ´±‚ ∞¯∞›∑¬(√÷º“)"
		nValueType = FUNCTION_VALUE_TYPE_PROB100;
		break;
	case DES_MINATTACK_02:	            //  ∞¯∞›∑¬ 2«¸			C
		sprintf(buf, STRMSG_C_PARAM_0018);//"[∞Ì±ﬁπ´±‚ ∞¯∞›∑¬(√÷º“)"
		nValueType = FUNCTION_VALUE_TYPE_PROB100;
		break;
	case DES_MAXATTACK_01:				//  ∞¯∞›∑¬ 1«¸			C
		sprintf(buf, STRMSG_C_PARAM_0019);//"[±‚∫ªπ´±‚ ∞¯∞›∑¬(√÷¥Î)"
		nValueType = FUNCTION_VALUE_TYPE_PROB100;
		break;
	case DES_MAXATTACK_02:	            //  ∞¯∞›∑¬ 2«¸			C
		sprintf(buf, STRMSG_C_PARAM_0020);//"[∞Ì±ﬁπ´±‚ ∞¯∞›∑¬(√÷¥Î)"
		nValueType = FUNCTION_VALUE_TYPE_PROB100;
		break;
	case DES_ATTACKPROBABILITY_01:	// ∞¯∞›»Æ∑¸ 01				C, PROB256_MAX_VALUE
		sprintf(buf, STRMSG_C_PARAM_0021);//"[±‚∫ªπ´±‚ ∞¯∞›»Æ∑¸"
		// 2010-07-28 by dgwoo »Æ∑¸ ºˆΩƒ ∫Ø∞Ê (255 => 100%)
		//nValueType = FUNCTION_VALUE_TYPE_PROB255;
		nValueType = FUNCTION_VALUE_TYPE_PROB;
		break;
	case DES_ATTACKPROBABILITY_02:    // ∞¯∞›»Æ∑¸ 02				C, PROB256_MAX_VALUE
		sprintf(buf, STRMSG_C_PARAM_0022);//"[∞Ì±ﬁπ´±‚ ∞¯∞›»Æ∑¸"
		// 2010-07-28 by dgwoo »Æ∑¸ ºˆΩƒ ∫Ø∞Ê (255 => 100%)
		//nValueType = FUNCTION_VALUE_TYPE_PROB255;
		nValueType = FUNCTION_VALUE_TYPE_PROB;
		break;
	case DES_DEFENSE_01:			// πÊæÓ∑¬ 01				C, PROB256_MAX_VALUE
		sprintf(buf, STRMSG_C_PARAM_0023);//"[±‚∫ªπ´±‚ø° ¥Î«— πÊæÓ∑¬"
		// 2010-07-28 by dgwoo »Æ∑¸ ºˆΩƒ ∫Ø∞Ê (255 => 100%)
		//nValueType = FUNCTION_VALUE_TYPE_PROB255;
		nValueType = FUNCTION_VALUE_TYPE_PROB;
		break;
	case DES_DEFENSE_02:			// πÊæÓ∑¬ 02				C, PROB256_MAX_VALUE
		sprintf(buf, STRMSG_C_PARAM_0024);//"[∞Ì±ﬁπ´±‚ø° ¥Î«— πÊæÓ∑¬"
		// 2010-07-28 by dgwoo »Æ∑¸ ºˆΩƒ ∫Ø∞Ê (255 => 100%)
		//nValueType = FUNCTION_VALUE_TYPE_PROB255;
		nValueType = FUNCTION_VALUE_TYPE_PROB;
		break;
	case DES_DEFENSEPROBABILITY_01:	// πÊæÓ»Æ∑¸ 01				C, PROB256_MAX_VALUE
		sprintf(buf, STRMSG_C_PARAM_0025);//"[±‚∫ªπ´±‚ø° ¥Î«— »∏««∑¬"
		// 2010-07-28 by dgwoo »Æ∑¸ ºˆΩƒ ∫Ø∞Ê (255 => 100%)
		//nValueType = FUNCTION_VALUE_TYPE_PROB255;
		nValueType = FUNCTION_VALUE_TYPE_PROB;
		break;
	case DES_DEFENSEPROBABILITY_02:	// πÊæÓ»Æ∑¸ 02				C, PROB256_MAX_VALUE
		sprintf(buf, STRMSG_C_PARAM_0026);//"[∞Ì±ﬁπ´±‚ø° ¥Î«— »∏««∑¬"
		// 2010-07-28 by dgwoo »Æ∑¸ ºˆΩƒ ∫Ø∞Ê (255 => 100%)
		//nValueType = FUNCTION_VALUE_TYPE_PROB255;
		nValueType = FUNCTION_VALUE_TYPE_PROB;
		break;
	case DES_SKILLPROBABILITY_01:		// ∏∂¿ŒµÂ ƒ¡∆Æ∑—∞¯∞›»Æ∑¸				C, PROB256_MAX_VALUE
		sprintf(buf, STRMSG_C_PARAM_0027);//"[Ω∫≈≥ ±‚∫ªπ´±‚ ∞¯∞›»Æ∑¸"
		nValueType = FUNCTION_VALUE_TYPE_PROB255;
		break;
	case DES_SKILLPROBABILITY_02:		// ∏∂¿ŒµÂ ƒ¡∆Æ∑—∞¯∞›»Æ∑¸				C, PROB256_MAX_VALUE
		sprintf(buf, STRMSG_C_PARAM_0028);//"[Ω∫≈≥ ∞Ì±ﬁπ´±‚ ∞¯∞›»Æ∑¸"
		nValueType = FUNCTION_VALUE_TYPE_PROB255;
		break;
	case DES_FACTIONRESISTANCE_01:		// º”º∫¿˙«◊∑¬				C, PROB256_MAX_VALUE
		sprintf(buf, STRMSG_C_PARAM_0029);//"[±‚∫ªπ´±‚ º”º∫¿˙«◊∑¬"
		nValueType = FUNCTION_VALUE_TYPE_PROB255;
		break;
	case DES_FACTIONRESISTANCE_02:		// º”º∫¿˙«◊∑¬				C, PROB256_MAX_VALUE
		sprintf(buf, STRMSG_C_PARAM_0030);//"[∞Ì±ﬁπ´±‚ º”º∫¿˙«◊∑¬"
		nValueType = FUNCTION_VALUE_TYPE_PROB255;
		break;
	case DES_SPEED:					// ¿Ãµøº”µµ					C
		sprintf(buf, STRMSG_C_PARAM_0031);//"[¿Ãµøº”µµ"
		nValueType = FUNCTION_VALUE_TYPE_PROB100; // 2005-11-26 by ispark
		break;
	case DES_TRANSPORT:				// øÓπ›∑¬
		sprintf(buf, STRMSG_C_PARAM_0032);//"[øÓπ›∑¬"
		break;
	case DES_MATERIAL:				// ¿Á¡˙
		sprintf(buf, STRMSG_C_PARAM_0033);//"[¿Á¡˙"
		break;
	case DES_REATTACKTIME_01:		// (*) ∏ÆæÓ≈√≈∏¿” 01		C
		sprintf(buf, STRMSG_C_PARAM_0034);//"[±‚∫ªπ´±‚ ¿Á∞¯∞›Ω√∞£"
		nValueType = FUNCTION_VALUE_TYPE_PROB100;
		break;
	case DES_REATTACKTIME_02:		// (*) ∏ÆæÓ≈√≈∏¿” 02		C
		sprintf(buf, STRMSG_C_PARAM_0035);//"[∞Ì±ﬁπ´±‚ ¿Á∞¯∞›Ω√∞£"
		nValueType = FUNCTION_VALUE_TYPE_PROB100;
		break;
	case DES_ABRASIONRATE_01:		// ∏∂∏¿≤ 01				C, PROB256_MAX_VALUE
		sprintf(buf, STRMSG_C_PARAM_0036);//"[±‚∫ªπ´±‚ ∏∂∏¿≤"
		nValueType = FUNCTION_VALUE_TYPE_PROB255;
		break;
	case DES_ABRASIONRATE_02:		// ∏∂∏¿≤ 02				C, PROB256_MAX_VALUE
		sprintf(buf, STRMSG_C_PARAM_0037);//"[∞Ì±ﬁπ´±‚ ∏∂∏¿≤"
		nValueType = FUNCTION_VALUE_TYPE_PROB255;
		break;
	case DES_RANGE_01:				// (*) ¿Ø»ø∞≈∏Æ 01			C
		sprintf(buf, STRMSG_C_PARAM_0038);//"[±‚∫ªπ´±‚ ¿Ø»ø∞≈∏Æ"
		nValueType = FUNCTION_VALUE_TYPE_PROB100;
		break;
	case DES_RANGE_02:				// (*) ¿Ø»ø∞≈∏Æ 02			C
		sprintf(buf, STRMSG_C_PARAM_0039);//"[∞Ì±ﬁπ´±‚ ªÁ∞≈∏Æ"
		nValueType = FUNCTION_VALUE_TYPE_PROB100;
		break;
	case DES_RANGEANGLE_01:			// ¿Ø»ø∞¢µµ 01				C
		sprintf(buf, STRMSG_C_PARAM_0040);//"[±‚∫ªπ´±‚ ¿Ø»ø∞¢µµ"
		break;
	case DES_RANGEANGLE_02:			// ¿Ø»ø∞¢µµ 02				C
		sprintf(buf, STRMSG_C_PARAM_0041);//"[∞Ì±ﬁπ´±‚ ¿Ø»ø∞¢µµ"
		break;
	case DES_MULTITAGET_01:			// ∏÷∆º≈∏∞Ÿ					C
		sprintf(buf, STRMSG_C_PARAM_0042);//"[±‚∫ªπ´±‚ ∏÷∆º≈∏∞Ÿ"
		break;
	case DES_MULTITAGET_02:			// ∏÷∆º≈∏∞Ÿ					C
		sprintf(buf, STRMSG_C_PARAM_0043);//"[∞Ì±ﬁπ´±‚ ∏÷∆º≈∏∞Ÿ"
		break;
	case DES_EXPLOSIONRANGE_01:		// ∆¯πﬂπ›∞Ê					C
		sprintf(buf, STRMSG_C_PARAM_0044);//"[±‚∫ªπ´±‚ ∆¯πﬂπ›∞Ê"
		break;
	case DES_EXPLOSIONRANGE_02:		// ∆¯πﬂπ›∞Ê					C
		sprintf(buf, STRMSG_C_PARAM_0045);//"[∞Ì±ﬁπ´±‚ ∆¯πﬂπ›∞Ê"
		break;
	case DES_UNIT:					// ¿Ø¥÷¿« ¡æ∑˘ (28 ~ 29¿Ã ∞∞¿Ã æ≤ø© ¿Ø¥÷∏∂¥Ÿ¿« ∫∏¡§∞™¿∏∑Œ ªÁøÎµ )
		sprintf(buf, STRMSG_C_PARAM_0046);//"[¿Ø¥÷¿« ¡æ∑˘"
		break;
	case DES_REVISION:				// ¿Ø¥÷¿« ∫∏¡§∞™ (28 ~ 29¿Ã ∞∞¿Ã æ≤ø© ¿Ø¥÷∏∂¥Ÿ¿« ∫∏¡§∞™¿∏∑Œ ªÁøÎµ )
		sprintf(buf, STRMSG_C_PARAM_0047);//"[¿Ø¥÷¿« ∫∏¡§∞™"
		break;
	case DES_FACTIONPROBABILITY_01:		// º”º∫ø° ¥Î«— πÊæÓ»Æ∑¸		C, PROB256_MAX_VALUE
		sprintf(buf, STRMSG_C_PARAM_0048);//"[±‚∫ªπ´±‚ º”º∫ø° ¥Î«— »∏««∑¬"
		nValueType = FUNCTION_VALUE_TYPE_PROB255;
		break;
	case DES_FACTIONPROBABILITY_02:		// º”º∫ø° ¥Î«— πÊæÓ»Æ∑¸		C, PROB256_MAX_VALUE
		sprintf(buf, STRMSG_C_PARAM_0049);//"[∞Ì±ﬁπ´±‚ º”º∫ø° ¥Î«— »∏««∑¬"
		nValueType = FUNCTION_VALUE_TYPE_PROB255;
		break;
	case DES_SHOTNUM_01:				// ¿œ¡°ªÁ ¥Á πﬂªÁ ºˆ		C
		sprintf(buf, STRMSG_C_PARAM_0050);//"[±‚∫ªπ´±‚ ø¨ªÁ∑¬"
		break;
	case DES_SHOTNUM_02:				// ¿œ¡°ªÁ ¥Á πﬂªÁ ºˆ		C
		sprintf(buf, STRMSG_C_PARAM_0051);//"[∞Ì±ﬁπ´±‚ ø¨ªÁ∑¬"
		break;
	case DES_MULTINUM_01:				// µøΩ√ πﬂªÁ ºˆ				C
		sprintf(buf, STRMSG_C_PARAM_0052);//"[±‚∫ªπ´±‚ ∏÷∆ºº¶"
		break;
	case DES_MULTINUM_02:				// µøΩ√ πﬂªÁ ºˆ				C
		sprintf(buf, STRMSG_C_PARAM_0053);//"[∞Ì±ﬁπ´±‚ ∏÷∆ºº¶"
		break;
	case DES_ATTACKTIME_01:			// √≥¿Ω ∞¯∞› Ω√¿« ≈∏¿” 01	C
		sprintf(buf, STRMSG_C_PARAM_0054);//"[±‚∫ªπ´±‚ πﬂµøΩ√∞£"
		break;
	case DES_ATTACKTIME_02:			// √≥¿Ω ∞¯∞› Ω√¿« ≈∏¿” 02	C
		sprintf(buf, STRMSG_C_PARAM_0055);//"[∞Ì±ﬁπ´±‚ πﬂµøΩ√∞£"
		break;
	case DES_TIME_01:				// (*)∞˙ø≠ Ω√∞£ 01				C
		sprintf(buf, STRMSG_C_PARAM_0056);//"[±‚∫ªπ´±‚ ∞˙ø≠Ω√∞£"
		nValueType = FUNCTION_VALUE_TYPE_PROB100;
		break;
	case DES_TIME_02:				// (*)∞˙ø≠ Ω√∞£ 02				C
		sprintf(buf, STRMSG_C_PARAM_0057);//"[∞Ì±ﬁπ´±‚ ∞˙ø≠Ω√∞£"
		nValueType = FUNCTION_VALUE_TYPE_PROB100;
		break;
	case DES_WEIGHT_01:				// (*)π´∞‘ 			C
		sprintf(buf, STRMSG_C_PARAM_0058);//"[±‚∫ªπ´±‚ π´∞‘"
		nValueType = FUNCTION_VALUE_TYPE_PROB100;
		break;
	case DES_WEIGHT_02:				// (*)π´∞‘ 			C
		sprintf(buf, STRMSG_C_PARAM_0059);//"[∞Ì±ﬁπ´±‚ π´∞‘"
		nValueType = FUNCTION_VALUE_TYPE_PROB100;
		break;
	case DES_REACTION_RANGE:
		sprintf(buf, STRMSG_C_PARAM_0060);//"[∞Ì±ﬁπ´±‚ π›¿¿∞≈∏Æ"
		break;
	case DES_OVERHITTIME_01:// (*) ø¿πˆ»˝
	case DES_OVERHITTIME_02:// (*) ø¿πˆ»˝
		sprintf(buf, STRMSG_C_050620_0001);//"[ø¿πˆ»˝»∏∫πΩ√∞£"
		break;
	case DES_BAZAAR_SELL:
		sprintf(str, STRMSG_C_060801_0008);//"[∆«∏≈ªÛ¡°¿ª Ω√¿€«“ ºˆ ¿÷¥Ÿ. πŸ¿⁄∏ ¿« ºΩ≈Õ ?ø°º≠∏∏ ªÁøÎ∞°¥…]"
		return;
	case DES_BAZAAR_BUY:
		sprintf(str, STRMSG_C_060801_0009);//"[±∏∏≈ªÛ¡°¿ª Ω√¿€«“ ºˆ ¿÷¥Ÿ. πŸ¿⁄∏ ¿« ºΩ≈Õ ?ø°º≠∏∏ ªÁøÎ∞°¥…]"
		return;
	case DES_UNITKIND:
	{// ±‚√º æ˜±◊∑π¿ÃµÂΩ√ «ÿ¥Á ±‚√º
		sprintf(buf, STRMSG_C_PARAM_0061);//"[æ˜±◊∑π¿ÃµÂ ±‚√º:"
		switch ((USHORT)fValue)
		{
		case UNITKIND_BT01:
			sprintf(str, STRMSG_C_PARAM_0062, buf);//"%sB-GEAR 1«¸]"
			break;
		case UNITKIND_BT02:
			sprintf(str, STRMSG_C_PARAM_0063, buf);//"%sB-GEAR 2«¸]"
			break;
		case UNITKIND_BT03:
			sprintf(str, STRMSG_C_PARAM_0064, buf);//"%sB-GEAR 3«¸]"
			break;
		case UNITKIND_BT04:
			sprintf(str, STRMSG_C_PARAM_0065, buf);//"%sB-GEAR 4«¸]"
			break;
		case UNITKIND_OT01:
			sprintf(str, STRMSG_C_PARAM_0066, buf);//"%sM-GEAR 1«¸]"
			break;
		case UNITKIND_OT02:
			sprintf(str, STRMSG_C_PARAM_0067, buf);//"%sM-GEAR 2«¸]"
			break;
		case UNITKIND_OT03:
			sprintf(str, STRMSG_C_PARAM_0068, buf);//"%sM-GEAR 3«¸]"
			break;
		case UNITKIND_OT04:
			sprintf(str, STRMSG_C_PARAM_0069, buf);//"%sM-GEAR 4«¸]"
			break;
		case UNITKIND_DT01:
			sprintf(str, STRMSG_C_PARAM_0070, buf);//"%sA-GEAR 1«¸]"
			break;
		case UNITKIND_DT02:
			sprintf(str, STRMSG_C_PARAM_0071, buf);//"%sA-GEAR 2«¸]"
			break;
		case UNITKIND_DT03:
			sprintf(str, STRMSG_C_PARAM_0072, buf);//"%sA-GEAR 3«¸]"
			break;
		case UNITKIND_DT04:
			sprintf(str, STRMSG_C_PARAM_0073, buf);//"%sA-GEAR 4«¸]"
			break;
		case UNITKIND_ST01:
			sprintf(str, STRMSG_C_PARAM_0074, buf);//"%sI-GEAR 1«¸]"
			break;
		case UNITKIND_ST02:
			sprintf(str, STRMSG_C_PARAM_0075, buf);//"%sI-GEAR 2«¸]"
			break;
		case UNITKIND_ST03:
			sprintf(str, STRMSG_C_PARAM_0076, buf);//"%sI-GEAR 3«¸]"
			break;
		case UNITKIND_ST04:
			sprintf(str, STRMSG_C_PARAM_0077, buf);//"%sI-GEAR 4«¸]"
			break;
		}
		return;
	}
	break;
	case DES_ITEMKIND:				// æ∆¿Ã≈€¡æ∑˘
	{
		sprintf(buf, STRMSG_C_PARAM_0078);//"[æ∆¿Ã≈€:"
		switch ((USHORT)fValue)
		{
		case ITEMKIND_AUTOMATIC:
			sprintf(str, STRMSG_C_PARAM_0079, buf);//"%sø¿≈‰∏≈∆Ω]"
			break;
		case ITEMKIND_VULCAN:
			sprintf(str, STRMSG_C_PARAM_0080, buf);//"%sπﬂƒ≠]"
			break;
			//				case ITEMKIND_GRENADE:
			//					sprintf(str, STRMSG_C_PARAM_0081 ,buf);//"%s±◊∑π≥◊¿ÃµÂ]" -> "%sµ‡æÛ∏ÆΩ∫∆Æ]"
		case ITEMKIND_DUALIST:						// 2005-08-02 by ispark
			sprintf(str, STRMSG_C_050802_0001, buf);//"%sµ‡æÛ∏ÆΩ∫∆Æ]"
			break;
		case ITEMKIND_CANNON:
			sprintf(str, STRMSG_C_PARAM_0082, buf);//"%sƒ≥≥Ì]"
			break;
		case ITEMKIND_RIFLE:
			sprintf(str, STRMSG_C_PARAM_0083, buf);//"%s∂Û¿Ã«√]"
			break;
		case ITEMKIND_GATLING:
			sprintf(str, STRMSG_C_PARAM_0084, buf);//"%s∞≥∆≤∏µ]"
			break;
		case ITEMKIND_LAUNCHER:
			sprintf(str, STRMSG_C_PARAM_0085, buf);//"%s∑±√ƒ]"
			break;
		case ITEMKIND_MASSDRIVE:
			sprintf(str, STRMSG_C_PARAM_0086, buf);//"%s∏≈Ω∫µÂ∂Û¿Ã∫Í]"
			break;
		case ITEMKIND_ROCKET:
			sprintf(str, STRMSG_C_PARAM_0087, buf);//"%s∑Œƒœ∑˘]"
			break;
		case ITEMKIND_MISSILE:
			sprintf(str, STRMSG_C_PARAM_0088, buf);//"%sπÃªÁ¿œ∑˘]"
			break;
		case ITEMKIND_BUNDLE:
			sprintf(str, STRMSG_C_PARAM_0089, buf);//"%sπ¯µÈ∑˘]"
			break;
		case ITEMKIND_MINE:
			sprintf(str, STRMSG_C_PARAM_0090, buf);//"%s∏∂¿Œ∑˘]"
			break;
		case ITEMKIND_SHIELD:
			sprintf(str, STRMSG_C_PARAM_0091, buf);//"%sΩØµÂ]"
			break;
		case ITEMKIND_DUMMY:
			sprintf(str, STRMSG_C_PARAM_0092, buf);//"%s¥ıπÃ]"
			break;
		case ITEMKIND_FIXER:
			sprintf(str, STRMSG_C_PARAM_0093, buf);//"%s«»º≠]"
			break;
		case ITEMKIND_DECOY:
			sprintf(str, STRMSG_C_PARAM_0094, buf);//"%sµƒ⁄¿Ã]"
			break;
		case ITEMKIND_ALL_WEAPON:
			sprintf(str, STRMSG_C_PARAM_0095, buf);//"%s∏µÁ π´±‚]"
			break;
		case ITEMKIND_PRIMARY_WEAPON_ALL:
			sprintf(str, STRMSG_C_PARAM_0096, buf);//"%s±‚∫ªπ´±‚]"
			break;
		case ITEMKIND_PRIMARY_WEAPON_1:
			sprintf(str, STRMSG_C_PARAM_0097, buf);//"%s√—æÀ«¸ ±‚∫ªπ´±‚]"
			break;
		case ITEMKIND_PRIMARY_WEAPON_2:
			sprintf(str, STRMSG_C_PARAM_0098, buf);//"%sø¨∑·«¸ ±‚∫ªπ´±‚]"
			break;
		case ITEMKIND_SECONDARY_WEAPON_ALL:
			sprintf(str, STRMSG_C_PARAM_0099, buf);//"%s∞Ì±ﬁπ´±‚]"
			break;
		case ITEMKIND_SECONDARY_WEAPON_1:
			sprintf(str, STRMSG_C_PARAM_0099, buf);//"%s∞Ì±ﬁπ´±‚]"
			break;
		case ITEMKIND_SECONDARY_WEAPON_2:
			sprintf(str, STRMSG_C_PARAM_0100, buf);//"%s∞Ì±ﬁπ´±‚(∫∏¡∂)]"
			break;
		case ITEMKIND_DEFENSE:
			sprintf(str, STRMSG_C_PARAM_0101, buf);//"%sæ∆∏”]"
			break;
		case ITEMKIND_SUPPORT:
			sprintf(str, STRMSG_C_PARAM_0102, buf);//"%s∫∏¡∂/ø£¡¯]"
			break;
		case ITEMKIND_BLASTER:					// 2005-08-02 by ispark
			sprintf(str, STRMSG_C_050802_0002, buf);//"%s∫Ì∑°Ω∫≈Õ]"
			break;
		case ITEMKIND_RAILGUN:					// 2005-08-02 by ispark
			sprintf(str, STRMSG_C_050802_0003, buf);//"%s∑π¿œ∞«]"
			break;
		}
		return;
	}
	break;
	case DES_GROUNDMODE:			// ∆¯∞› ∏µÂ
		sprintf(buf, STRMSG_C_PARAM_0103);//"[∆¯∞› ∏µÂ"
		break;
	case DES_SIEGEMODE:				// Ω√¡Ó ∏µÂ
		sprintf(buf, STRMSG_C_PARAM_0104);//"[Ω√¡Ó ∏µÂ"
		break;
	case DES_IMMEDIATE_HP_UP:
		sprintf(buf, STRMSG_C_PARAM_0105);//"[ø°≥ ¡ˆ"
		break;
	case DES_IMMEDIATE_DP_UP:
		sprintf(buf, STRMSG_C_PARAM_0106);//"[ΩØµÂ"
		break;
	case DES_IMMEDIATE_SP_UP:
		sprintf(buf, STRMSG_C_PARAM_0107);//"[Ω∫≈≥P"
		break;
	case DES_IMMEDIATE_EP_UP:
		sprintf(buf, STRMSG_C_PARAM_0108);//"[ø¨∑·"
		break;
		// 2004-12-06 by ydkim ¿Œ√¶∆Æ ∞¸∑√
	case DES_RARE_FIX_PREFIX:
		sprintf(str, STRMSG_C_PARAM_0109);//"¡¢µŒªÁ"
		return;
	case DES_RARE_FIX_SUFFIX:
		sprintf(str, STRMSG_C_PARAM_0110);//"¡¢πÃªÁ"
		return;
	case DES_RARE_FIX_BOTH:
		sprintf(str, STRMSG_C_PARAM_0111);//"¡¢µŒªÁ, ¡¢πÃªÁ"
		return;
	case DES_RARE_FIX_PREFIX_INITIALIZE:
		sprintf(str, STRMSG_C_PARAM_0112);//"∏µÁπ´±‚¿« ¡¢µŒªÁ ∑πæÓ ø…º« √ ±‚»≠"
		return;
	case DES_RARE_FIX_SUFFIX_INITIALIZE:
		sprintf(str, STRMSG_C_PARAM_0113);//"∏µÁπ´±‚¿« ¡¢πÃªÁ ∑πæÓ ø…º« √ ±‚»≠"
		return;
		// 2005-11-22 by ispark ªı∑Œ ±∏«ˆµ» Ω∫≈≥ ¡§∫∏
	case DES_SKILL_REDUCE_SHIELD_DAMAGE:
		sprintf(buf, STRMSG_C_051122_0001);// "[ΩØµÂ µ•πÃ¡ˆ ∞®º“"
		nValueType = FUNCTION_VALUE_TYPE_PROB100;
		break;
	case DES_ATTACK_RANGE_01:
		sprintf(buf, STRMSG_C_051122_0003);// "[∑π¿Ã¥Ÿ 1«¸ ªÁ∞≈∏Æ ¡ı∞°"
		nValueType = FUNCTION_VALUE_TYPE_PERCENT;
		break;
	case DES_ATTACK_RANGE_02:
		sprintf(buf, STRMSG_C_051122_0004);// "[∑π¿Ã¥Ÿ 2«¸ ªÁ∞≈∏Æ ¡ı∞°"
		nValueType = FUNCTION_VALUE_TYPE_PERCENT;
		break;
	case DES_SKILL_COLLISIONDAMAGE_DOWN:
		sprintf(buf, STRMSG_C_051122_0002);// "[√Ê∞› µ•πÃ¡ˆ ∞®º“"
		nValueType = FUNCTION_VALUE_TYPE_PROB100;
		break;
	case DES_SKILL_REVERSEENGINE:
		sprintf(str, STRMSG_C_051126_0001);//  "[»ƒ¡¯ ∞°¥…]"
		return;
	case DES_SKILL_SMARTSP:
		sprintf(buf, STRMSG_C_051122_0008);// "[SP º“∏∑Æ ∞®º“"
		nValueType = FUNCTION_VALUE_TYPE_PROB100;
		break;
	case DES_SKILL_SUMMON_FORMATION_MEMBER:
		sprintf(str, STRMSG_C_051125_0001);// "[∆Ì¥Îø¯ 1∏Ì º“»Ø]"
		return;
	case DES_SKILL_REACTIONSPEED:
		sprintf(buf, STRMSG_C_051128_0001);// "[ø£¡¯ º±»∏∑¬"
		nValueType = FUNCTION_VALUE_TYPE_PROB100;
		break;
	case DES_SKILL_ENGINEANGLE:
		sprintf(buf, STRMSG_C_051202_0001);//  "[ø£¡¯ º±»∏∞¢"
		nValueType = FUNCTION_VALUE_TYPE_PROB100;
		break;
	case DES_SKILL_ENGINEBOOSTERANGLE:
		sprintf(buf, STRMSG_C_051202_0002);// "[∫ŒΩ∫≈Õø£¡¯ º±»∏∞¢"
		nValueType = FUNCTION_VALUE_TYPE_PROB100;
		break;
	case DES_WARP:
		sprintf(str, STRMSG_C_051222_0100);// "[µµΩ√∑Œ ±Õ»Ø]"
		return;
	case DES_SKILL_SLOWMOVING:
		sprintf(buf, STRMSG_C_051230_0100);// "[¿Ãµøº”µµ ∞®º“"
		nValueType = FUNCTION_VALUE_TYPE_PROB100;
		break;
	case DES_CASH_NORMAL_RESTORE:
		sprintf(buf, STRMSG_C_060424_0003);// "[∫Œ»∞»Æ∑¸"
		nValueType = FUNCTION_VALUE_TYPE_PROB;
		break;
	case DES_DROP_EXP:
		sprintf(buf, STRMSG_C_060424_0007);// "[∞Ê«Ëƒ°"
		nValueType = FUNCTION_VALUE_TYPE_PROB100;
		break;
	case DES_DROP_SPI:
		sprintf(buf, STRMSG_C_060424_0008);// "[SPI µÂ∂¯¿≤"
		nValueType = FUNCTION_VALUE_TYPE_PROB100;
		break;
	case DES_DROP_ITEM:
		sprintf(buf, STRMSG_C_060424_0009);// "[æ∆¿Ã≈€ µÂ∂¯¿≤"
		nValueType = FUNCTION_VALUE_TYPE_PROB100;
		break;
	case DES_HP_REPAIR_RATE_FLIGHTING:
		sprintf(str, STRMSG_C_060424_0010);// "∫Ò«‡Ω√ HP»∏∫π ∞°¥…"
		return;
	case DES_DP_REPAIR_RATE:
		sprintf(buf, STRMSG_C_060424_0011);// "[∆ÚªÛΩ√¿« ΩØµÂ »∏∫π¿≤"
		nValueType = FUNCTION_VALUE_TYPE_PROB100;
		break;
	case DES_SP_REPAIR_RATE:
		sprintf(buf, STRMSG_C_060424_0012);// "[SP »∏∫πº”µµ"
		nValueType = FUNCTION_VALUE_TYPE_PROB100;
		break;
	case DES_CASH_STAT_ALL_INITIALIZE:
		sprintf(str, STRMSG_C_060424_0013);// "ƒ≥∏Ø≈Õ¿« ∏µÁ Ω∫≈»¿ª √ ±‚»≠"
		return;
	case DES_CASH_STAT_PART_INITIALIZE:
		sprintf(str, STRMSG_C_060424_0014);// "ƒ≥∏Ø≈Õ¿« º±≈√«— «—∞°¡ˆ Ω∫≈»¿ª √ ±‚»≠"
		return;
	case DES_CASH_CHANGE_CHARACTERNAME:
		sprintf(str, STRMSG_C_060424_0015);// "ƒ≥∏Ø≈Õ¿« ¿Ã∏ß¿ª ∫Ø∞Ê«‘"
		return;
	case DES_CASH_STEALTH:
		sprintf(str, STRMSG_C_060424_0021);// "∏µÁ ∏ÛΩ∫≈Õ¿« ∫Òº±∞¯»≠"
		return;
	case DES_KILLMARK_EXP:
		sprintf(buf, STRMSG_C_060424_0007);// "[∞Ê«Ëƒ°"
		break;
	case DES_GRADUAL_DP_UP:
		sprintf(buf, STRMSG_C_PARAM_0106);//"[ΩØµÂ"
		break;
	case DES_SKILL_CHAFF_HP:
		sprintf(buf, STRMSG_C_061206_0103);//[√º«¡HP
		break;
	case DES_SKILL_HALLUCINATION:
		sprintf(buf, STRMSG_C_061208_0101);//"[√º«¡∞≥ºˆ"
		break;
	case DES_SKILL_HYPERSHOT:
		sprintf(buf, STRMSG_C_PARAM_0044);
		break;
	case DES_SKILL_ROLLING_TIME:
		sprintf(str, STRMSG_C_061206_0104);
		return;
	case DES_SKILL_SHIELD_PARALYZE:
		sprintf(str, STRMSG_C_061206_0105);
		return;
	case DES_SKILL_BARRIER:
		sprintf(str, STRMSG_C_061206_0106);
		return;
	case DES_SKILL_BIG_BOOM:
		sprintf(str, STRMSG_C_061206_0107);
		return;
	case DES_INVISIBLE:
		sprintf(str, STRMSG_C_061206_0108);
		return;
	case DES_SKILL_CANCELALL:
		sprintf(str, STRMSG_C_061206_0109);
		return;
	case DES_SKILL_INVINCIBLE:
		sprintf(str, STRMSG_C_061206_0110);
		return;
	case DES_SKILL_FULL_RECOVERY:
		sprintf(str, STRMSG_C_061206_0111);
		return;
	case DES_SKILL_SCANNING:
		sprintf(str, STRMSG_C_061206_0112);
		return;
	case DES_SKILL_NO_WARNING:
		sprintf(str, STRMSG_C_061206_0113);
		return;
	case DES_SKILL_CAMOUFLAGE:
		sprintf(str, STRMSG_C_061206_0114);
		return;
	case DES_WARHEAD_SPEED:
		sprintf(buf, STRMSG_C_070614_0100);
		nValueType = FUNCTION_VALUE_TYPE_PROB100;
		break;
		// 2007-09-12 by bhsohn ¿¸¡¯ ±‚¡ˆ ∆˜≈ª æ∆¿Ã≈€ º≥∏Ì √ﬂ∞°
	case DES_WARP_OUTPOST:
	{
		sprintf(str, STRMSG_C_070912_0201);
		return;
	}
	break;
	// end2007-09-12 by bhsohn ¿¸¡¯ ±‚¡ˆ ∆˜≈ª æ∆¿Ã≈€ º≥∏Ì √ﬂ∞°
	// 2007-09-12 by bhsohn Ω∫««ƒø æ∆¿Ã≈€ º≥∏Ì √ﬂ∞°
	case DES_CHAT_ALL_INFLUENCE:
	{
		sprintf(str, STRMSG_C_070912_0202);
		return;
	}
	break;
	// end 2007-09-12 by bhsohn Ω∫««ƒø æ∆¿Ã≈€ º≥∏Ì √ﬂ∞°
	// 2007-11-23 by dgwoo ƒ≥∏Ø≈Õ ∫Ø∞Ê ƒ´µÂ 
	case DES_CASH_CHANGE_PILOTFACE:
	{
		sprintf(str, STRMSG_C_071123_0100);
		return;
	}
	break;
	// 2007-12-17 by bhsohn ¿Œ√æ∆Æ √ ±‚»≠ ƒ´µÂ º≥∏Ì √ﬂ∞°
	case DES_ENCHANT_INITIALIZE:
	{
		sprintf(str, STRMSG_C_071217_0201);
		return;
	}
	break;
	// end 2007-12-17 by bhsohn ¿Œ√æ∆Æ √ ±‚»≠ ƒ´µÂ º≥∏Ì √ﬂ∞°
	// 2008-06-18 by bhsohn ø©¥‹ø¯¡ı∞° ƒ´µÂ ∞¸∑√ √≥∏Æ
	case DES_CASH_GUILD:
	{
		sprintf(str, STRMSG_C_080619_0203);	//"ø©¥‹ ∞°¿‘ √÷¥Î ¡¶«— ¿Œø¯ ¡ı∞°"
		return;
	}
	break;
	// end 2008-06-18 by bhsohn ø©¥‹ø¯¡ı∞° ƒ´µÂ ∞¸∑√ √≥∏Æ
	// 2008-09-26 by bhsohn Ω≈±‘ ¿Œ√æ∆Æ √≥∏Æ
	case DES_ENGINE_BOOSTER_TIME_UP:	// ∫ŒΩ∫≈Õ Ω√∞£ ¡ı∞°
	{
		sprintf(str, STRMSG_C_080929_0200, fValue);
		return;
	}
	break;
	case DES_ENGINE_MAX_SPEED_UP:		// ø£¡¯ ¿œπ›º”µµ(√÷¥Î) ¡ı∞°
	{
		sprintf(str, STRMSG_C_080929_0201, fValue);
		return;
	}
	break;
	case DES_ENGINE_MIN_SPEED_UP:		// ø£¡¯ ¿œπ›º”µµ(√÷º“) ¡ı∞°
	{
		sprintf(str, STRMSG_C_080929_0202, fValue);
		return;
	}
	break;
	case DES_ENGINE_BOOSTER_SPEED_UP:		// ø£¡¯ ∫ŒΩ∫≈Õº”µµ ¡ı∞°
	{
		sprintf(str, STRMSG_C_080929_0203, fValue);
		return;
	}
	break;
	case DES_ENGINE_GROUND_SPEED_UP:		// ø£¡¯ ¡ˆªÛº”µµ ¡ı∞°
	{
		sprintf(str, STRMSG_C_080929_0204, fValue);
		return;
	}
	break;
	case DES_RADAR_OBJECT_DETECT_RANGE:		// ∑π¿Ã¥ı π∞√º ∞®¡ˆ π›∞Ê
	{
		sprintf(str, STRMSG_C_080929_0205, fValue);
		return;
	}
	break;
	case DES_PIERCE_UP_01:		// ±‚∫ªπ´±‚ ««æÓΩ∫¿≤ ¡ı∞° ƒ´µÂ
	{
		sprintf(buf, STRMSG_C_080929_0206);
		// 2010-07-28 by dgwoo »Æ∑¸ ºˆΩƒ ∫Ø∞Ê (255 => 100%)
		//nValueType = FUNCTION_VALUE_TYPE_PROB255;	
		nValueType = FUNCTION_VALUE_TYPE_PROB;
	}
	break;
	case DES_PIERCE_UP_02:		// ∞Ì±ﬁπ´±‚ ««æÓΩ∫¿≤ ¡ı∞° ƒ´µÂ
	{
		sprintf(buf, STRMSG_C_080929_0207);
		// 2010-07-28 by dgwoo »Æ∑¸ ºˆΩƒ ∫Ø∞Ê (255 => 100%)
		//nValueType = FUNCTION_VALUE_TYPE_PROB255;	
		nValueType = FUNCTION_VALUE_TYPE_PROB;
	}
	break;
	// 2010. 12. 05 by jskim, ∑πæÓæ∆¿Ã≈€ µÂ∂¯»Æ∑¸ ¡ı∞° æ∆¿Ã≈€ ±∏«ˆ
	case DES_RARE_ITEM_DROP_RATE:
	{
		sprintf(buf, STRMSG_C_101206_0401);	 ///	"[∑πæÓ æ∆¿Ã≈€ µÂ∂¯¿≤"
		nValueType = FUNCTION_VALUE_TYPE_PROB100;
	}
	break;
	// end 2010. 12. 05 by jskim, ∑πæÓæ∆¿Ã≈€ µÂ∂¯»Æ∑¸ ¡ı∞° æ∆¿Ã≈€ ±∏«ˆ
	// 2010-12-21 by jskim, ∏∂¿ª ¿Ãµø º”µµ ¡ı∞° æ∆¿Ã≈€ ±∏«ˆ
	case DES_RARE_ITEM_PARTNER_SPEED:
	{
		sprintf(str, STRMSG_C_101221_0401, fValue * 100);		// "∏∂¿ª ¿Ãµø º”µµ %.2f%% ¡ı∞°"
		return;
	}
	break;
	// end 2010-12-21 by jskim, ∏∂¿ª ¿Ãµø º”µµ ¡ı∞° æ∆¿Ã≈€ ±∏«ˆ
	// 2010-12-21 by jskim, ∆ƒ∆Æ≥  µ•πÃ¡ˆ ¡ı∞° æ∆¿Ã≈€ ±∏«ˆ
	case DES_RARE_ITEM_PARTNER_DAMAGE:
	{
		sprintf(str, STRMSG_C_101221_0402, fValue * 100);		// "∆ƒ∆Æ≥  µ•πÃ¡ˆ %.2f%% ¡ı∞°"
		return;
	}
	break;
	// end 2010-12-21 by jskim, ∆ƒ∆Æ≥  µ•πÃ¡ˆ ¡ı∞° æ∆¿Ã≈€ ±∏«ˆ
	// 2010-12-21 by jskim, HP, DP ≈∞∆Æ ¿˚øÎ∑Æ ¡ı∞° æ∆¿Ã≈€ ±∏«ˆ
	case DES_RARE_ITEM_HPDP:
	{
		sprintf(str, STRMSG_C_101221_0403, fValue * 100);		// "ºˆ∏Æ, ΩØµÂ ≈∞∆Æ ¿˚øÎ∑Æ %.2f%% ¡ı∞°"
		return;
	}
	break;
	// end 2010-12-21 by jskim, HP, DP ≈∞∆Æ ¿˚øÎ∑Æ ¡ı∞° æ∆¿Ã≈€ ±∏«ˆ
	case DES_REQ_MIN_LEVEL:		// æ∆¿Ã≈€¿Â¬¯ ø‰±∏ MinLevel¿ª ≥∑√·¥Ÿ
	case DES_REQ_MAX_LEVEL:		// æ∆¿Ã≈€¿Â¬¯ ø‰±∏ MaxLevel¿ª ≥∑√·¥Ÿ
	{
		sprintf(str, STRMSG_C_080929_0211, fValue);
		return;
	}
	break;
	case DES_ENGINE_ANGLE_UP:	// ø£¡¯ »∏¿¸∞¢ ¡ı∞° ƒ´µÂ
	{
		sprintf(str, STRMSG_C_080929_0214, (fValue / PI*180.0f));
		return;
	}
	break;
	case DES_ENGINE_BOOSTERANGLE_UP:	// ø£¡¯ ∫ŒΩ∫≈Õ »∏¿¸∞¢ ¡ı∞° ƒ´µÂ
	{
		sprintf(str, STRMSG_C_080929_0215, (fValue / PI*180.0f));
		return;
	}
	break;
	// end 2008-09-26 by bhsohn Ω≈±‘ ¿Œ√æ∆Æ √≥∏Æ
	// 2008-12-30 by bhsohn ¡ˆµµ¿⁄ √§∆√ ¡¶«— ƒ´µÂ ±‚»πæ»
	case DES_CHAT_BLOCK:	// ¡ˆµµ¿⁄ √§∆√ ¡¶«— ƒ´µÂ ±∏«ˆ - 
	{
		sprintf(str, STRMSG_C_081230_0208, (int)(fValue / 60.0f));
		return;
	}
	break;
	// end 2008-12-30 by bhsohn ¡ˆµµ¿⁄ √§∆√ ¡¶«— ƒ´µÂ ±‚»πæ»
	// 2009-01-19 by bhsohn "∏ÛΩ∫≈Õ º“»Øƒ´µÂ" º≥∏Ì √ﬂ∞°
	case DES_CASH_MONSTER_SUMMON:
	{
		sprintf(str, STRMSG_C_090120_0201);
		return;
	}
	break;
	// end 2009-01-19 by bhsohn "∏ÛΩ∫≈Õ º“»Øƒ´µÂ" º≥∏Ì √ﬂ∞°
	// 2010. 03. 18 by jskim ∏ÛΩ∫≈Õ∫ØΩ≈ ƒ´µÂ
	case DES_TRANSFORM_TO_MONSTER:
	{
		sprintf(str, STRMSG_C_100401_0401);
		return;
	}
	break;
	case DES_TRANSFORM_TO_GEAR:
	{
		sprintf(str, STRMSG_C_100401_0402);
		return;
	}
	break;
	//end 2010. 03. 18 by jskim ∏ÛΩ∫≈Õ∫ØΩ≈ ƒ´µÂ
	// 2009. 11. 02 by ckPark ¿Œ««¥œ∆º « µÂ ¿ŒΩ∫≈œΩ∫ ¥¯¡Ø Ω√Ω∫≈€
	case DES_ITEM_RESISTANCE:
	{
		sprintf(str, STRMSG_C_091103_0326, fValue);	// "[ªÛ≈¬ ¿ÃªÛ ¿˙«◊:+%.2f%%]"
		return;
	}
	break;
	case DES_ITEM_ADDATTACK:
	{
			sprintf(str, STRMSG_C_091103_0342, fValue);	// "[±‚∫ªπ´±‚ √ﬂ∞° ≈∏∞›ƒ°:+%.2f]"
		return;
	}
	break;
	case DES_ITEM_ADDATTACK_SEC:
	{
			sprintf(str, STRMSG_C_091103_0343, fValue);	// "[∞Ì±ﬁπ´±‚ √ﬂ∞° ≈∏∞›ƒ°:+%.2f]"
		return;
	}
	break;
	case DES_ITEM_IGNOREDEFENCE:
	{
		sprintf(str, STRMSG_C_091103_0328, fValue);	// "[πÊæÓ∑¬ π´Ω√:+%.2f%%]"
		return;
	}
	break;
	case DES_ITEM_IGNOREAVOID:
	{
		sprintf(str, STRMSG_C_091103_0329, fValue);	// "[»∏««∑¬ π´Ω√:+%.2f%%]"
		return;
	}
	break;
	case DES_ITEM_REDUCEDAMAGE:
	{
		sprintf(str, STRMSG_C_091103_0330, fValue);	// "[µ•πÃ¡ˆ ∫–ªÍ:+%.2f]"
		return;
	}
	break;
	case DES_SKILL_RELEASE:
	{
		sprintf(str, STRMSG_C_091103_0347);	// "¥ÎªÛ¿« µπˆ«¡ Ω∫≈≥¿ª «ÿ¡¶ «—¥Ÿ."
		return;
	}
	break;
	// end 2009. 11. 02 by ckPark ¿Œ««¥œ∆º « µÂ ¿ŒΩ∫≈œΩ∫ ¥¯¡Ø Ω√Ω∫≈€

	// 2010. 02. 11 by ckPark πﬂµø∑˘ ¿Â¬¯æ∆¿Ã≈€
	case DES_PAIR_DRAIN_1_RATE:
		strcpy(buf, STRMSG_C_100218_0301);	// "[µÂ∑π¿Œ »Æ∑¸
		nValueType = FUNCTION_VALUE_TYPE_PROB;
		break;

	case DES_PAIR_DRAIN_2_HP_DP_UP_RATE:
		strcpy(buf, STRMSG_C_100218_0302);	// "[πﬂµø Ω√ »Ìºˆ∑Æ
		nValueType = FUNCTION_VALUE_TYPE_PROB100;
		break;

	case DES_PAIR_REFLECTION_1_RATE:
		strcpy(buf, STRMSG_C_100218_0303);	// "[πÃ∑Ø∏µ »Æ∑¸
		nValueType = FUNCTION_VALUE_TYPE_PROB;
		break;

	case DES_PAIR_REFLECTION_2_DAMAGE_RATE:
		strcpy(buf, STRMSG_C_100218_0304);	// "[µ•πÃ¡ˆ π›ªÁ∑Æ
		nValueType = FUNCTION_VALUE_TYPE_PROB100;
		break;

	case DES_ANTI_DRAIN_RATE:
		strcpy(buf, STRMSG_C_100218_0305);	// "[µÂ∑π¿Œ ¿˙«◊ »Æ∑¸
		nValueType = FUNCTION_VALUE_TYPE_PROB;
		break;

	case DES_ANTI_REFLECTION_RATE:
		strcpy(buf, STRMSG_C_100218_0306);	// "[πÃ∑Ø∏µ ¿˙«◊ »Æ∑¸
		nValueType = FUNCTION_VALUE_TYPE_PROB;
		break;
		// end 2010. 02. 11 by ckPark πﬂµø∑˘ ¿Â¬¯æ∆¿Ã≈€

		// 2010. 03. 23 by ckPark ¿Œ««¥œ∆º « µÂ 2¬˜(¿Œ««¥œ∆º « µÂ ¿‘¿Â ƒ≥Ω¨æ∆¿Ã≈€)
	case DES_INFINITY_REENTRY_TICKET:
	{
		sprintf(str, STRMSG_C_100310_0309);	// "¿Œ««¥œ∆º« µÂ √ﬂ∞° ¿‘¿Â"
		return;
	}
	break;
	// end 2010. 03. 23 by ckPark ¿Œ««¥œ∆º « µÂ 2¬˜(¿Œ««¥œ∆º « µÂ ¿‘¿Â ƒ≥Ω¨æ∆¿Ã≈€)

	// 2010. 03. 03 by ckPark ¿Œ««¥œ∆º « µÂ 2¬˜(¿‘¿Â UI∫Ø∞Ê)
	case DES_SKILL_MON_SILENCE_PRIMARY:
	{
		strcpy(str, STRMSG_C_100310_0312);	// "[±‚∫ªπ´±‚ ªÁøÎ∫“∞°]"
		return;
	}
	break;
	case DES_SKILL_MON_SILENCE_SECOND:
	{
		strcpy(str, STRMSG_C_100310_0313);	// "[∞Ì±ﬁπ´±‚ ªÁøÎ∫“∞°]"
		return;
	}
	break;
	case DES_SKILL_MON_FREEZE_HP:
	{
		strcpy(str, STRMSG_C_100310_0315);	// "[ø°≥ ¡ˆ »∏∫π ∫“∞°]"
		return;
	}
	break;
	case DES_SKILL_MON_FREEZE_DP:
	{
		strcpy(str, STRMSG_C_100310_0316);	// "[Ω«µÂ »∏∫π ∫“∞°]"
		return;
	}
	break;
	case DES_SKILL_MON_FREEZE_SP:
	{
		strcpy(str, STRMSG_C_100310_0317);	// "[SP »∏∫π ∫“∞°]"
		return;
	}
	break;
	case DES_SKILL_MON_HOLD:
	{
		strcpy(str, STRMSG_C_100310_0318);	// "[¿Ãµø ∫“∞°]"
		return;
	}
	break;
	case DES_SKILL_MON_STEALING:
	{
		sprintf(str, STRMSG_C_100310_0314, fValue);	// "[SP »Ìºˆ:-%.2f]"
		return;
	}
	break;
	case DES_SKILL_MON_DRAIN:
	{
		sprintf(str, STRMSG_C_100310_0311, fValue);	// "[HP »Ìºˆ:-%.2f]"
		return;
	}
	break;
	case DES_SKILL_MON_SILENCE_SKILL:
		break;
		// end 2010. 03. 03 by ckPark ¿Œ««¥œ∆º « µÂ 2¬˜(¿‘¿Â UI∫Ø∞Ê)
		// 2010. 05. 18 by jskim ∞›≈ıø’¿« »∆¿Â ≈¯∆¡ ±∏«ˆ
	case DES_PLUS_WARPOINT_RATE:
	{
		sprintf(str, STRMSG_C_100518_0401, fValue + 1);	// "[WarPoint :%.2fπË ¡ı∞°]"
		return;
	}
	break;
	//end 2010. 05. 18 by jskim ∞›≈ıø’¿« »∆¿Â ≈¯∆¡ ±∏«ˆ
	// 2010. 06. 08 by jskim »®«¡∏ÆπÃæˆ UI ¿€æ˜
	case DES_PCROOM_USE_CARD:
	{
		sprintf(str, STRMSG_C_100610_0401);	// "[»® «¡∏ÆπÃæˆ º≠∫ÒΩ∫]"
		return;
	}
	break;
	//end 2010. 06. 08 by jskim »®«¡∏ÆπÃæˆ UI ¿€æ˜
	// 2010-08-26 by shcho&&jskim, WARPOINT ¡ı∞° æ∆¿Ã≈€ ±∏«ˆ
	case DES_WAR_POINT_UP:
	{
		sprintf(str, STRMSG_C_100826_0401, fValue);	// "WARPOINT %d ¡ı∞°"
		return;
	}
	break;
	// end 2010-08-26 by shcho&&jskim, WARPOINT ¡ı∞° æ∆¿Ã≈€ ±∏«ˆ
	// 2013-02-14 by bhsohn ∆Ø¡§ º”º∫ æ∆¿Ã≈€ ≈¯∆¡ ¿ÃªÛ«œ∞‘ ¬Ô»˜¥¬ «ˆªÛ √≥∏Æ
	case DES_SUMMON_POSITION_X:		// º“»ØΩ√ ¿ßƒ° ∫Ø∞Ê (ªÛ¥Î∞™)
	case DES_SUMMON_POSITION_Y:		// º“»ØΩ√ ¿ßƒ° ∫Ø∞Ê (ªÛ¥Î∞™)
	case DES_SUMMON_POSITION_Z:		// º“»ØΩ√ ¿ßƒ° ∫Ø∞Ê (ªÛ¥Î∞™)	
	case DES_ITEM_BUFF_PARTY:	// 2013-02-19 by bhsohn πÃº« ∏∂Ω∫≈Õ ∞¸∑√ æ∆¿Ã≈€ ≈¯∆¡ ¿ﬂ∏¯ ≥™ø¿¥¬ πÆ¡¶ «ÿ∞·
	{
		sprintf(str, "");
		return;
	}
	break;
#ifdef _INET_RANKBUFF
	case DES_SKILL_BUFF_MON_ATTACK_POWER:			// ∏ÛΩ∫≈Õ ∞¯∞›Ω√ - ∞¯∞›∑¬ ¡ı∞° : Value ¡ı∞° %
	{
		sprintf(buf,STRMSG_C_130520_0001);
		nValueType = FUNCTION_VALUE_TYPE_PROB100;
	}
	break;
	case DES_SKILL_BUFF_MON_ATTACK_PROBABILITY:		// ∏ÛΩ∫≈Õ ∞¯∞›Ω√ - ∞¯∞›∑¬ »Æ¿≤ : Value ¡ı∞° %
	{
		sprintf(buf, STRMSG_C_130520_0002);
		nValueType = FUNCTION_VALUE_TYPE_PROB;
	}
	break;
	case DES_SKILL_BUFF_MON_ATTACK_PIERCE:			// ∏ÛΩ∫≈Õ ∞¯∞›Ω√ - ««æÓΩ∫ ¡ı∞° : Value ¡ı∞° %
	{
		sprintf(buf, STRMSG_C_130520_0003);
		nValueType = FUNCTION_VALUE_TYPE_PROB;
	}
	break;
	case DES_SKILL_BUFF_MON_DEFENCE:				// ∏ÛΩ∫≈Õ πÊæÓΩ√ - πÊæÓ∑¬ ¡ı∞° : Value ¡ı∞° %
	{
		sprintf(buf, STRMSG_C_130520_0004);
		nValueType = FUNCTION_VALUE_TYPE_PROB;
	}
	break;
	case DES_SKILL_BUFF_MON_DEFENCE_AVOID:			// ∏ÛΩ∫≈Õ πÊæÓΩ√ - »∏««∑¬ ¡ı∞° : Value ¡ı∞° %
	{
		sprintf(buf, STRMSG_C_130520_0005);
		nValueType = FUNCTION_VALUE_TYPE_PROB;
	}
	break;
	case DES_SKILL_BUFF_PVP_ATTACK_POWER:			// PVP - ∞¯∞›∑¬ ¡ı∞° : Value ¡ı∞° %
	{
		sprintf(buf, STRMSG_C_130520_0006);
		// 2013-08-02 by ssjung ø™¿¸¿« πˆ«¡ ≈¯∆¡«•Ω√
		nValueType = FUNCTION_VALUE_TYPE_PROB;
		fValue = TURN_AROUND_BUFF_SKILL_100P_VALUE * 1;
	}
	break;
	case DES_SKILL_BUFF_PVP_ATTACK_PROBABILITY:	// PVP - ∏Ì¡ﬂ∑¸ ¡ı∞° : Value ¡ı∞° %
	{
		sprintf(buf, STRMSG_C_130801_0001);
		nValueType = FUNCTION_VALUE_TYPE_PROB;
		fValue = TURN_AROUND_BUFF_SKILL_100P_VALUE * 1;
	}
	break;
	case DES_SKILL_BUFF_PVP_ATTACK_PIERCE:			// PVP - ««æÓΩ∫ ¡ı∞° : Value ¡ı∞° %
	{
		sprintf(buf, STRMSG_C_130801_0002);
		nValueType = FUNCTION_VALUE_TYPE_PROB;
		fValue = TURN_AROUND_BUFF_SKILL_100P_VALUE * 1;
	}
	break;
	case DES_SKILL_BUFF_PVP_DEFENCE:				// PVP - πÊæÓ∑¬ ¡ı∞° : Value ¡ı∞° %
	{
		sprintf(buf, STRMSG_C_130801_0003);
		nValueType = FUNCTION_VALUE_TYPE_PROB;
		fValue = TURN_AROUND_BUFF_SKILL_100P_VALUE * 1;
	}
	break;
	case DES_SKILL_BUFF_PVP_DEFENCE_PROBABILITY:	// PVP - »∏««∑¬ ¡ı∞° : Value ¡ı∞° %
	{
		sprintf(buf, STRMSG_C_130801_0004);
		nValueType = FUNCTION_VALUE_TYPE_PROB;
		fValue = TURN_AROUND_BUFF_SKILL_100P_VALUE * 1;
		// end 2013-08-02 by ssjung ø™¿¸¿« πˆ«¡ ≈¯∆¡«•Ω√
	}
	break;
#endif
	default:
	{
		if (0 == fValue)
		{
			// ∞™¿Ã æ¯¥Ÿ. ø¿∑˘ ªÛ»≤
			sprintf(str, "");
			DBGOUT("CINFItemInfo::SetParameterInfo FALSE[%d]", bType);
			return;
		}
	}
	break;
	// END2013-03-27 by bhsohn DestParam ¿⁄∑·«¸ ºˆ¡§
	}
	if (fValue > 0)
	{
		switch (nValueType)
		{
		case FUNCTION_VALUE_TYPE_NORMAL:
		{
			// 2010. 02. 11 by ckPark πﬂµø∑˘ ¿Â¬¯æ∆¿Ã≈€
			// 				if( m_pRefEnchant != NULL )
			// 				{
			// 					float fEnchant = GetParamFactor_DesParam( *m_pRefEnchant, bType );
			// 					if( fEnchant == 0)
			// 					{
			// 						sprintf(str,"%s:+%.2f]",buf,fValue);
			// 					}
			// 					else
			// 					{
			// 						if( fEnchant > 0)
			// 						{
			// 							sprintf(str,"%s:+%.2f\\e[+%.2f]\\g]",buf,fValue, fEnchant);
			// 						}
			// 						else
			// 						{
			// 							sprintf(str,"%s:+%.2f\\e[%.2f]\\g]",buf,fValue, fEnchant);
			// 						}
			// 					}
			// 				}
			// 				else
			// 				{
			// 					// 2006-12-14 by dgwoo ¿Ø»ø∞¢µµ¿« ∞ÊøÏ¥¬ ∂Ûµæ»∞™¿ª ∞ËªÍ«ÿº≠ √‚∑¬
			// 					if(bType == DES_RANGEANGLE_02)
			// 					{
			// 						sprintf(str,"%s:+%.2f]",buf,fValue/PI*180);
			// 					}
			// 					else
			// 					{
			// 						sprintf(str,"%s:+%.2f]",buf,fValue);
			// 					}
			// 				}

			float fEnchant = 0.0f;
			float fBonus = 0.0f;
			// ±‚∫ª ¿Œ√¶∆Æµ•Ω∫∆ƒ∂˜¿∫ CParamFactorø°º≠
			if (Bonus)
			{
				if (m_pRefEnchant != NULL)
					fBonus = GetParamFactor_DesParam(*m_pRefBonus, bType);
			}
			if (bDefEnchant)
			{
				if (m_pRefEnchant != NULL)
					fEnchant = GetParamFactor_DesParam(*m_pRefEnchant, bType);
			}
			else
			{
				// πﬂµø∑˘ µ•Ω∫∆ƒ∂˜¿∫ πﬂµø∑˘ ¿Œ√¶∆Æ ¡§∫∏ø°º≠
				if (m_pRefItemInfo)
					fEnchant = m_pRefItemInfo->GetInvokeEnchantParamValue(bType);
			}

			if (fEnchant != 0.0f)
			{
				if (fEnchant > 0.0f)
				{
					//2013-04-11 by ssjung º“º˝¡°¿Ã ¿÷¿ª∂ß∏∏ º“º˝¡° «•Ω√
					if (SingofDecimalDecision(fValue))
					{
#ifdef BONUS_STAT_ITEM_param
						if (Bonus)
						{
							if (m_pRefITEM->ItemNum == 9000500 || m_pRefITEM->ItemNum == 9000505) //XXL Energy
							{
								if (bType == 13)
									sprintf(str, "%s:+%.2f\\e[+%.2f]\\g]", buf, fValue + (fValue * 0.12), fEnchant);
							}
							else if (m_pRefITEM->ItemNum == 9000600 || m_pRefITEM->ItemNum == 9000605) //XXL Shield
							{
								if (bType == 89)
									sprintf(str, "%s:+%.2f\\e[+%.2f]\\g]", buf, fValue + (fValue * 0.12), fEnchant);
							}
							else
							{
								/*if (bType == 13 || bType == 89)
								{
									sprintf(str, "%s:+%.2f\\e[+%.2f]\\g]", buf, fValue + (fValue * 0.05), fEnchant);
								}
								else
								{*/
									sprintf(str, "%s:+%.2f\\e[+%.2f]\\g]", buf, fValue, fEnchant);
								//}
							}
						}
						else
						{
							if ((bType == 13 || bType == 89) && m_pRefBonus != NULL)
							{
								if (bType == 13)
									sprintf(str, "%s:+%.2f\\e[+%.2f]\\g]", buf, fValue - m_pRefBonus->pfp_HP, fEnchant);
								else
									sprintf(str, "%s:+%.2f\\e[+%.2f]\\g]", buf, fValue - m_pRefBonus->pfp_DP, fEnchant);
							}
							else
							{
								sprintf(str, "%s:+%.2f\\e[+%.2f]\\g]", buf, fValue, fEnchant);
							}
						}
#else
						sprintf(str, "%s:+%.2f\\e[+%.2f]\\g]", buf, fValue, fEnchant);
#endif
					}
					else
					{
#ifdef BONUS_STAT_ITEM_param
						if (Bonus)
						{
							if (m_pRefITEM->ItemNum == 9000500 || m_pRefITEM->ItemNum == 9000505) //XXL Energy
							{
								if (bType == 13)
									sprintf(str, "%s:+%d\\e[+%.2f]\\g]", buf, (int)(fValue + (fValue * 0.12)), fEnchant);
							}
							else if (m_pRefITEM->ItemNum == 9000600 || m_pRefITEM->ItemNum == 9000605) //XXL Shield
							{
								if (bType == 89)
									sprintf(str, "%s:+%d\\e[+%.2f]\\g]", buf, (int)(fValue + (fValue * 0.12)), fEnchant);
							}
							else
							{
								/*if (bType == 13 || bType == 89)
									sprintf(str, "%s:+%d\\e[+%.2f]\\g]", buf, (int)(fValue + (fValue * 0.05)), fEnchant);
								else*/
									sprintf(str, "%s:+%d\\e[+%.2f]\\g]", buf, (int)fValue, fEnchant);
							}
						}
						else
						{
							if ((bType == 13 || bType == 89) && m_pRefBonus != NULL)
							{
								if (bType == 13)
									sprintf(str, "%s:+%d\\e[+%.2f]\\g]", buf, (int)(fValue - m_pRefBonus->pfp_HP), fEnchant);
								else
									sprintf(str, "%s:+%.2f\\e[+%.2f]\\g]", buf, (int)(fValue - m_pRefBonus->pfp_DP), fEnchant);
							}
							else
							{
								sprintf(str, "%s:+%d\\e[+%.2f]\\g]", buf, (int)fValue, fEnchant);
							}
						}
#else
						sprintf(str, "%s:+%d\\e[+%.2f]\\g]", buf, (int)fValue, fEnchant);
#endif
					}
				}
				else
				{
					if (SingofDecimalDecision(fValue)){
#ifdef BONUS_STAT_ITEM_param
						if (Bonus)
						{
							if (m_pRefITEM->ItemNum == 9000500 || m_pRefITEM->ItemNum == 9000505) //XXL Energy
							{
								if (bType == 13)
									sprintf(str, "%s:+%.2f\\e[%.2f]\\g]", buf, fValue + (fValue * 0.12), fEnchant);
							}
							else if (m_pRefITEM->ItemNum == 9000600 || m_pRefITEM->ItemNum == 9000605) //XXL Shield
							{
								if (bType == 89)
									sprintf(str, "%s:+%.2f\\e[%.2f]\\g]", buf, fValue + (fValue * 0.12), fEnchant);
							}
							else
							{
								/*if (bType == 13 || bType == 89)
									sprintf(str, "%s:+%.2f\\e[%.2f]\\g]", buf, fValue + (fValue * 0.05), fEnchant);
								else*/
									sprintf(str, "%s:+%.2f\\e[%.2f]\\g]", buf, fValue, fEnchant);
							}
						}
						else
						{
							if ((bType == 13 || bType == 89) && m_pRefBonus != NULL)
							{
								if (bType == 13)
									sprintf(str, "%s:+%.2f\\e[%.2f]\\g]", buf, fValue - m_pRefBonus->pfp_HP, fEnchant);
								else
									sprintf(str, "%s:+%.2f\\e[%.2f]\\g]", buf, fValue - m_pRefBonus->pfp_DP, fEnchant);
							}
							else
							{
								sprintf(str, "%s:+%.2f\\e[%.2f]\\g]", buf, fValue, fEnchant);
							}
						}
#else
						sprintf(str, "%s:+%.2f\\e[%.2f]\\g]", buf, fValue, fEnchant);
#endif
					}
					else
					{
#ifdef BONUS_STAT_ITEM_param
						if (Bonus)
						{
							if (m_pRefITEM->ItemNum == 9000500 || m_pRefITEM->ItemNum == 9000505) //XXL Energy
							{
								if (bType == 13)
									sprintf(str, "%s:+%d\\e[%.2f]\\g]", buf, (int)(fValue + (fValue * 0.12)), fEnchant);
							}
							else if (m_pRefITEM->ItemNum == 9000600 || m_pRefITEM->ItemNum == 9000605) //XXL Shield
							{
								if (bType == 89)
									sprintf(str, "%s:+%d\\e[%.2f]\\g]", buf, (int)(fValue + (fValue * 0.12)), fEnchant);
							}
							else
							{
							/*	if (bType == 13 || bType == 89)
									sprintf(str, "%s:+%d\\e[%.2f]\\g]", buf, (int)(fValue + (fValue * 0.05)), fEnchant);
								else*/
									sprintf(str, "%s:+%d\\e[%.2f]\\g]", buf, (int)fValue, fEnchant);
							}
						}
						else
						{
							if ((bType == 13 || bType == 89) && m_pRefBonus != NULL)
							{
								if (bType == 13)
									sprintf(str, "%s:+%d\\e[%.2f]\\g]", buf, (int)(fValue - m_pRefBonus->pfp_HP), fEnchant);
								else
									sprintf(str, "%s:+%d\\e[%.2f]\\g]", buf, (int)(fValue - m_pRefBonus->pfp_DP), fEnchant);
							}
							else
							{
								sprintf(str, "%s:+%d\\e[%.2f]\\g]", buf, (int)fValue, fEnchant);
							}
						}
#else
						sprintf(str, "%s:+%d\\e[%.2f]\\g]", buf, (int)fValue, fEnchant);
#endif
					}
				}
			}
			else
			{
				if (bType == DES_RANGEANGLE_02)
					sprintf(str, "%s:+%.2f]", buf, fValue / PI * 180);
				else
				{
					if (SingofDecimalDecision(fValue))
					{
#ifdef BONUS_STAT_ITEM_param
						if (Bonus)
						{
							if (m_pRefITEM->ItemNum == 9000500 || m_pRefITEM->ItemNum == 9000505) //XXL Energy
							{
								if (bType == 13)
									sprintf(str, "%s:+%2.2f]", buf, fValue + (fValue * 0.12));
							}
							else if (m_pRefITEM->ItemNum == 9000600 || m_pRefITEM->ItemNum == 9000605) //XXL Shield
							{
								if (bType == 89)
									sprintf(str, "%s:+%2.2f]", buf, fValue + (fValue * 0.12));
							}
							else
							{
								/*if (bType == 13 || bType == 89)
									sprintf(str, "%s:+%2.2f]", buf, fValue + (fValue * 0.05));
								else*/
									sprintf(str, "%s:+%2.2f]", buf, fValue);
							}
						}
						else
						{
							if ((bType == 13 || bType == 89) && m_pRefBonus != NULL)
							{
								if (bType == 13)
									sprintf(str, "%s:+%2.2f]", buf, fValue - m_pRefBonus->pfp_HP);
								else
									sprintf(str, "%s:+%2.2f]", buf, fValue - m_pRefBonus->pfp_DP);
							}
							else
							{
								sprintf(str, "%s:+%2.2f]", buf, fValue);
							}
						}
#else
						sprintf(str, "%s:+%2.2f]", buf, fValue);
#endif
					}
					else
					{
#ifdef BONUS_STAT_ITEM_param
						if (Bonus)
						{
							if (m_pRefITEM->ItemNum == 9000500 || m_pRefITEM->ItemNum == 9000505) //XXL Energy
							{
								if (bType == 13)
									sprintf(str, "%s:+%d]", buf, (int)(fValue + (fValue * 0.12)));
							}
							else if (m_pRefITEM->ItemNum == 9000600 || m_pRefITEM->ItemNum == 9000605) //XXL Shield
							{
								if (bType == 89)
									sprintf(str, "%s:+%d]", buf, (int)(fValue + (fValue * 0.12)));
							}
							else
							{
								/*if (bType == 13 || bType == 89)
									sprintf(str, "%s:+%d]", buf, (int)(fValue + (fValue * 0.05)));
								else*/
									sprintf(str, "%s:+%d]", buf, (int)fValue);
							}
						}
						else
						{
							if ((bType == 13 || bType == 89) && m_pRefBonus != NULL)
							{
								if (bType == 13)
									sprintf(str, "%s:+%d]", buf, (int)(fValue - m_pRefBonus->pfp_HP));
								else
									sprintf(str, "%s:+%d]", buf, (int)(fValue - m_pRefBonus->pfp_DP));
							}
							else
							{
								sprintf(str, "%s:+%d]", buf, (int)fValue);
							}
						}
#else
						sprintf(str, "%s:+%d]", buf, (int)fValue);
#endif
					}
				}
			}
			// end 2010. 02. 11 by ckPark πﬂµø∑˘ ¿Â¬¯æ∆¿Ã≈€
		}
		break;
		// 2006-04-24 by ispark
		case FUNCTION_VALUE_TYPE_PROB:
		{
			// 2010. 02. 11 by ckPark πﬂµø∑˘ ¿Â¬¯æ∆¿Ã≈€
			// 				if( m_pRefEnchant != NULL )
			// 				{
			// 					float fEnchant = GetParamFactor_DesParam( *m_pRefEnchant, bType );
			// 					if( fEnchant == 0)
			// 					{
			// 						sprintf(str, "%s:+%2.0f%%]",buf,fValue);
			// 					}
			// 					else
			// 					{
			// 						if( fEnchant > 0)
			// 						{
			// 							sprintf(str, "%s:+%2.0f%%\\e[+%2.0f%%]\\g]",buf,fValue, fEnchant);
			// 						}
			// 						else
			// 						{
			// 							sprintf(str, "%s:+%2.0f%%\\e[%2.0f%%]\\g]",buf,fValue, fEnchant);
			// 						}
			// 					}
			// 				}
			// 				else
			// 				{
			// 					sprintf(str, "%s:+%2.0f%%]",buf,fValue);
			// 				}

			float fEnchant = 0.0f;
			float fBonus = 0.0f;
			// ±‚∫ª ¿Œ√¶∆Æµ•Ω∫∆ƒ∂˜¿∫ CParamFactorø°º≠
			if (Bonus)
			{
				if (m_pRefEnchant != NULL)
					fBonus = GetParamFactor_DesParam(*m_pRefBonus, bType);
			}
			if (bDefEnchant)
			{
				if (m_pRefEnchant != NULL)
					fEnchant = GetParamFactor_DesParam(*m_pRefEnchant, bType);
			}
			else
			{
				// πﬂµø∑˘ µ•Ω∫∆ƒ∂˜¿∫ πﬂµø∑˘ ¿Œ√¶∆Æ ¡§∫∏ø°º≠
				if (m_pRefItemInfo)
					fEnchant = m_pRefItemInfo->GetInvokeEnchantParamValue(bType);
			}

			if (fEnchant == 0.0f)
			{
				//2013-04-11 by ssjung º“º˝¡°¿Ã ¿÷¿ª∂ß∏∏ º“º˝¡° «•Ω√

				if (SingofDecimalDecision(fValue))
					sprintf(str, "%s:+%2.2f%%]", buf, fValue);
				else
					sprintf(str, "%s:+%d%%]", buf, (int)fValue);


			}
			else
			{
				if (fEnchant > 0.0f)
				{
					if (SingofDecimalDecision(fValue))
						sprintf(str, "%s:+%2.2f%%\\e[+%2.2f%%]\\g]", buf, fValue, fEnchant);
					else
						sprintf(str, "%s:+%d%%\\e[+%2.2f%%]\\g]", buf, (int)fValue, fEnchant);
				}
				else
				{
					if (SingofDecimalDecision(fValue))
						sprintf(str, "%s:+%2.2f%%\\e[%2.2f%%]\\g]", buf, fValue, fEnchant);
					else
						sprintf(str, "%s:+%d%%\\e[%2.2f%%]\\g]", buf, (int)fValue, fEnchant);
					//end 2013-04-11 by ssjung º“º˝¡°¿Ã ¿÷¿ª∂ß∏∏ º“º˝¡° «•Ω√
				}
			}
			// end 2010. 02. 11 by ckPark πﬂµø∑˘ ¿Â¬¯æ∆¿Ã≈€
		}
		break;
		case FUNCTION_VALUE_TYPE_PROB100:
		{
			// 2010. 02. 11 by ckPark πﬂµø∑˘ ¿Â¬¯æ∆¿Ã≈€
			// 				if( m_pRefEnchant != NULL )
			// 				{
			// 					float fEnchant = GetParamFactor_DesParam( *m_pRefEnchant, bType );
			// 					if( fEnchant == 0)
			// 					{
			// 						sprintf(str, "%s:+%2.0f%%]",buf,fValue*100);
			// 					}
			// 					else
			// 					{
			// 						if( fEnchant > 0)
			// 						{
			// 							sprintf(str, "%s:+%2.0f%%\\e[+%2.0f%%]\\g]",buf,fValue*100, fEnchant*100);
			// 						}
			// 						else
			// 						{
			// 							sprintf(str, "%s:+%2.0f%%\\e[%2.0f%%]\\g]",buf,fValue*100, fEnchant*100);
			// 						}
			// 					}
			// 				}
			// 				else
			// 				{
			// 					sprintf(str, "%s:+%2.0f%%]",buf,fValue*100);
			// 				}

			float fEnchant = 0.0f;
			// ±‚∫ª ¿Œ√¶∆Æµ•Ω∫∆ƒ∂˜¿∫ CParamFactorø°º≠
			if (bDefEnchant)
			{
				if (m_pRefEnchant != NULL)
					fEnchant = GetParamFactor_DesParam(*m_pRefEnchant, bType);
			}
			else
			{
				// πﬂµø∑˘ µ•Ω∫∆ƒ∂˜¿∫ πﬂµø∑˘ ¿Œ√¶∆Æ ¡§∫∏ø°º≠
				if (m_pRefItemInfo)
					fEnchant = m_pRefItemInfo->GetInvokeEnchantParamValue(bType);
			}

			if (fEnchant == 0.0f)
			{
				//2013-04-11 by ssjung º“º˝¡°¿Ã ¿÷¿ª∂ß∏∏ º“º˝¡° «•Ω√
				if (SingofDecimalDecision(fValue * 100))
					sprintf(str, "%s:+%2.2f%%]", buf, fValue * 100);
				else
					sprintf(str, "%s:+%d%%]", buf, (int)(fValue * 100));
			}
			else
			{
				if (fEnchant > 0.0f)
				{
					if (SingofDecimalDecision(fValue * 100))
						sprintf(str, "%s:+%2.2f%%\\e[+%2.2f%%]\\g]", buf, fValue * 100, fEnchant * 100);
					else
						sprintf(str, "%s:+%d%%\\e[+%2.2f%%]\\g]", buf, (int)(fValue * 100), fEnchant * 100);
				}
				else
				{
					if (SingofDecimalDecision(fValue * 100))
						sprintf(str, "%s:+%2.2f%%\\e[%2.2f%%]\\g]", buf, fValue * 100, fEnchant * 100);
					else
						sprintf(str, "%s:+%d%%\\e[%2.2f%%]\\g]", buf, (int)(fValue * 100), fEnchant * 100);
				}
				//end 2013-04-11 by ssjung º“º˝¡°¿Ã ¿÷¿ª∂ß∏∏ º“º˝¡° «•Ω√
			}
			// end 2010. 02. 11 by ckPark πﬂµø∑˘ ¿Â¬¯æ∆¿Ã≈€
		}
		break;
		case FUNCTION_VALUE_TYPE_PROB255:
		{
			// 2010. 02. 11 by ckPark πﬂµø∑˘ ¿Â¬¯æ∆¿Ã≈€
			// 				if( m_pRefEnchant != NULL )
			// 				{
			// 					float fEnchant = GetParamFactor_DesParam( *m_pRefEnchant, bType );
			// 					if( fEnchant == 0)
			// 					{
			// 						sprintf(str, "%s:+%.2f%%]",buf,((fValue/255)*100));
			// 					}
			// 					else
			// 					{
			// 						if( fEnchant > 0)
			// 						{
			// 							sprintf(str, "%s:+%.2f%%\\e[+%.2f%%]\\g]",buf,(fValue/255)*100, (fEnchant/255)*100);
			// 						}
			// 						else
			// 						{
			// 							sprintf(str, "%s:+%.2f%%\\e[%.2f%%]\\g]",buf,(fValue/255)*100, (fEnchant/255)*100);
			// 						}
			// 					}
			// 				}
			// 				else
			// 				{
			// 					sprintf(str, "%s:+%.2f%%]",buf,(fValue/255)*100);
			// 				}

			float fEnchant = 0.0f;
			// ±‚∫ª ¿Œ√¶∆Æµ•Ω∫∆ƒ∂˜¿∫ CParamFactorø°º≠
			if (bDefEnchant)
			{
				if (m_pRefEnchant != NULL)
					fEnchant = GetParamFactor_DesParam(*m_pRefEnchant, bType);
			}
			else
			{
				// πﬂµø∑˘ µ•Ω∫∆ƒ∂˜¿∫ πﬂµø∑˘ ¿Œ√¶∆Æ ¡§∫∏ø°º≠
				if (m_pRefItemInfo)
					fEnchant = m_pRefItemInfo->GetInvokeEnchantParamValue(bType);
			}

			if (fEnchant == 0.0f)
			{
				//2013-04-11 by ssjung º“º˝¡°¿Ã ¿÷¿ª∂ß∏∏ º“º˝¡° «•Ω√
				if (SingofDecimalDecision((fValue / 255) * 100))
					sprintf(str, "%s:+%.2f%%]", buf, (fValue / 255) * 100);
				else
					sprintf(str, "%s:+%d%%]", buf, (int)((fValue / 255) * 100));
			}
			else
			{
				if (fEnchant > 0.0f)
				{
					if (SingofDecimalDecision((fValue / 255) * 100))
						sprintf(str, "%s:+%.2f%%\\e[+%.2f%%]\\g]", buf, (fValue / 255) * 100, (fEnchant / 255) * 100);
					else
						sprintf(str, "%s:+%d%%\\e[+%.2f%%]\\g]", buf, (int)((fValue / 255) * 100), (fEnchant / 255) * 100);
				}
				else
				{
					if (SingofDecimalDecision((fValue / 255) * 100))
						sprintf(str, "%s:+%.2f%%\\e[%.2f%%]\\g]", buf, (fValue / 255) * 100, (fEnchant / 255) * 100);
					else

						sprintf(str, "%s:+%d%%\\e[%.2f%%]\\g]", buf, (int)((fValue / 255) * 100), (fEnchant / 255) * 100);
					//end 2013-04-11 by ssjung º“º˝¡°¿Ã ¿÷¿ª∂ß∏∏ º“º˝¡° «•Ω√	
				}
			}
			// end 2010. 02. 11 by ckPark πﬂµø∑˘ ¿Â¬¯æ∆¿Ã≈€
		}
		break;
		// 2008-09-26 by bhsohn Ω≈±‘ ¿Œ√æ∆Æ √≥∏Æ
		case FUNCTION_VALUE_TYPE_PERCENT:
		{
			//2013-04-11 by ssjung º“º˝¡°¿Ã ¿÷¿ª∂ß∏∏ º“º˝¡° «•Ω√
			if (SingofDecimalDecision(fValue*100.0f))
				sprintf(str, "%s:+%.2f%%]", buf, (fValue*100.0f));
			else
				sprintf(str, "%s:+%d%%]", buf, (int)(fValue*100.0f));
			//end 2013-04-11 by ssjung º“º˝¡°¿Ã ¿÷¿ª∂ß∏∏ º“º˝¡° «•Ω√
		}
		break;
		// end 2008-09-26 by bhsohn Ω≈±‘ ¿Œ√æ∆Æ √≥∏Æ
		}
	}
	else if (fValue < 0)
	{
		switch (nValueType)
		{
		case FUNCTION_VALUE_TYPE_NORMAL:
		{
			// 2010. 02. 11 by ckPark πﬂµø∑˘ ¿Â¬¯æ∆¿Ã≈€
			// 				if( m_pRefEnchant != NULL )
			// 				{
			// 					float fEnchant = GetParamFactor_DesParam( *m_pRefEnchant, bType );
			// 					if( fEnchant == 0)
			// 					{
			// 						sprintf(str,"%s:\\r%.2f\\g]",buf,fValue);
			// 					}
			// 					else
			// 					{
			// 						if( fEnchant > 0)
			// 						{
			// 							sprintf(str,"%s:\\r%.2f\\e[+%.2f]\\g]",buf,fValue, fEnchant);
			// 						}
			// 						else
			// 						{
			// 							sprintf(str,"%s:\\r%.2f\\e[%.2f]\\g]",buf,fValue, fEnchant);
			// 						}
			// 					}
			// 				}
			// 				else
			// 				{
			// 					sprintf(str,"%s:\\r%.2f\\g]",buf,fValue);
			// 				}

			float fEnchant = 0.0f;
			float fBonus = 0.0f;
			// ±‚∫ª ¿Œ√¶∆Æµ•Ω∫∆ƒ∂˜¿∫ CParamFactorø°º≠
			if (Bonus)
			{
				if (m_pRefEnchant != NULL)
					fBonus = GetParamFactor_DesParam(*m_pRefBonus, bType);
			}
			if (bDefEnchant)
			{
				if (m_pRefEnchant != NULL)
					fEnchant = GetParamFactor_DesParam(*m_pRefEnchant, bType);
			}
			else
			{
				// πﬂµø∑˘ µ•Ω∫∆ƒ∂˜¿∫ πﬂµø∑˘ ¿Œ√¶∆Æ ¡§∫∏ø°º≠
				if (m_pRefItemInfo)
					fEnchant = m_pRefItemInfo->GetInvokeEnchantParamValue(bType);
			}

			if (fEnchant == 0.0f)
			{
				//2013-04-11 by ssjung º“º˝¡°¿Ã ¿÷¿ª∂ß∏∏ º“º˝¡° «•Ω√
				if (SingofDecimalDecision(fValue))
					sprintf(str, "%s:\\r%.2f\\g]", buf, fValue);
				else
					sprintf(str, "%s:\\r%d\\g]", buf, (int)fValue);
			}
			else
			{
				if (fEnchant > 0.0f)
				{
					if (SingofDecimalDecision(fValue))
						sprintf(str, "%s:\\r%.2f\\e[+%.2f]\\g]", buf, fValue, fEnchant);
					else
						sprintf(str, "%s:\\r%d\\e[+%.2f]\\g]", buf, (int)fValue, fEnchant);
				}
				else
				{
					if (SingofDecimalDecision(fValue))
						sprintf(str, "%s:\\r%.2f\\e[%.2f]\\g]", buf, fValue, fEnchant);
					else
						sprintf(str, "%s:\\r%d\\e[%.2f]\\g]", buf, (int)fValue, fEnchant);
					//end 2013-04-11 by ssjung º“º˝¡°¿Ã ¿÷¿ª∂ß∏∏ º“º˝¡° «•Ω√
				}
			}
			// end 2010. 02. 11 by ckPark πﬂµø∑˘ ¿Â¬¯æ∆¿Ã≈€
		}
		break;
		// 2006-04-24 by ispark
		case FUNCTION_VALUE_TYPE_PROB:
		{
			// 2010. 02. 11 by ckPark πﬂµø∑˘ ¿Â¬¯æ∆¿Ã≈€
			// 				if( m_pRefEnchant != NULL )
			// 				{
			// 					float fEnchant = GetParamFactor_DesParam( *m_pRefEnchant, bType );
			// 					if( fEnchant == 0)
			// 					{
			// 						sprintf(str, "%s:+%2.0f%%]",buf,fValue);
			// 					}
			// 					else
			// 					{
			// 						if( fEnchant > 0)
			// 						{
			// 							sprintf(str, "%s:+%2.0f%%\\e[+%2.0f%%]\\g]",buf,fValue, fEnchant);
			// 						}
			// 						else
			// 						{
			// 							sprintf(str, "%s:+%2.0f%%\\e[%2.0f%%]\\g]",buf,fValue, fEnchant);
			// 						}
			// 					}
			// 				}
			// 				else
			// 				{
			// 					sprintf(str, "%s:+%2.0f%%]",buf,fValue);
			// 				}

			float fEnchant = 0.0f;
			float fBonus = 0.0f;
			// ±‚∫ª ¿Œ√¶∆Æµ•Ω∫∆ƒ∂˜¿∫ CParamFactorø°º≠
			if (Bonus)
			{
				if (m_pRefEnchant != NULL)
					fBonus = GetParamFactor_DesParam(*m_pRefBonus, bType);
			}
			if (bDefEnchant)
			{
				if (m_pRefEnchant != NULL)
					fEnchant = GetParamFactor_DesParam(*m_pRefEnchant, bType);
			}
			else
			{
				// πﬂµø∑˘ µ•Ω∫∆ƒ∂˜¿∫ πﬂµø∑˘ ¿Œ√¶∆Æ ¡§∫∏ø°º≠
				if (m_pRefItemInfo)
					fEnchant = m_pRefItemInfo->GetInvokeEnchantParamValue(bType);
			}

			if (fEnchant == 0.0f)
			{
				//2013-04-11 by ssjung º“º˝¡°¿Ã ¿÷¿ª∂ß∏∏ º“º˝¡° «•Ω√
				if (SingofDecimalDecision(fValue))
					sprintf(str, "%s:+%2.2f%%]", buf, fValue);
				else
					sprintf(str, "%s:+%d%%]", buf, (int)fValue);
			}
			else
			{
				if (fEnchant > 0.0f)
				{
					if (SingofDecimalDecision(fValue))
						sprintf(str, "%s:+%2.2f%%\\e[+%2.2f%%]\\g]", buf, fValue, fEnchant);
					else
						sprintf(str, "%s:+%d%%\\e[+%2.2f%%]\\g]", buf, (int)fValue, fEnchant);
				}
				else
				{
					if (SingofDecimalDecision(fValue))
						sprintf(str, "%s:+%2.2f%%\\e[%2.2f%%]\\g]", buf, fValue, fEnchant);
					else
						sprintf(str, "%s:+%d%%\\e[%2.2f%%]\\g]", buf, (int)fValue, fEnchant);
				}
				//end 2013-04-11 by ssjung º“º˝¡°¿Ã ¿÷¿ª∂ß∏∏ º“º˝¡° «•Ω√
			}
			// end 2010. 02. 11 by ckPark πﬂµø∑˘ ¿Â¬¯æ∆¿Ã≈€
		}
		break;
		case FUNCTION_VALUE_TYPE_PROB100:
		{
			// 2010. 02. 11 by ckPark πﬂµø∑˘ ¿Â¬¯æ∆¿Ã≈€
			// 				if( m_pRefEnchant != NULL )
			// 				{
			// 					float fEnchant = GetParamFactor_DesParam( *m_pRefEnchant, bType );
			// 					if( fEnchant == 0)
			// 					{
			// 						sprintf(str, "%s:\\r%2.0f%%\\g]",buf,fValue*100);
			// 					}
			// 					else
			// 					{
			// 						if( fEnchant > 0)
			// 						{
			// 							sprintf(str, "%s:\\r%2.0f%%\\e[+%2.0f%%]\\g]",buf,fValue*100, fEnchant*100);
			// 						}
			// 						else
			// 						{
			// 							sprintf(str, "%s:\\r%2.0f%%\\e[%2.0f%%]\\g]",buf,fValue*100, fEnchant*100);
			// 						}
			// 					}
			// 				}
			// 				else
			// 				{
			// 					sprintf(str, "%s:\\r%2.0f%%\\g]",buf,fValue*100);
			// 				}

			float fEnchant = 0.0f;
			// ±‚∫ª ¿Œ√¶∆Æµ•Ω∫∆ƒ∂˜¿∫ CParamFactorø°º≠
			if (bDefEnchant)
			{
				if (m_pRefEnchant != NULL)
					fEnchant = GetParamFactor_DesParam(*m_pRefEnchant, bType);
			}
			else
			{
				// πﬂµø∑˘ µ•Ω∫∆ƒ∂˜¿∫ πﬂµø∑˘ ¿Œ√¶∆Æ ¡§∫∏ø°º≠
				if (m_pRefItemInfo)
					fEnchant = m_pRefItemInfo->GetInvokeEnchantParamValue(bType);
			}

			if (fEnchant == 0.0f)
			{
				//2013-04-11 by ssjung º“º˝¡°¿Ã ¿÷¿ª∂ß∏∏ º“º˝¡° «•Ω√
				if (SingofDecimalDecision(fValue * 100))
					sprintf(str, "%s:\\r%2.2f%%\\g]", buf, fValue * 100);
				else
					sprintf(str, "%s:\\r%d%%\\g]", buf, (int)(fValue * 100));
			}
			else
			{
				if (fEnchant > 0.0f)
				{
					if (SingofDecimalDecision(fValue * 100))
						sprintf(str, "%s:\\r%2.2f%%\\e[+%2.2f%%]\\g]", buf, fValue * 100, fEnchant * 100);
					else
						sprintf(str, "%s:\\r%d%%\\e[+%2.2f%%]\\g]", buf, (int)(fValue * 100), fEnchant * 100);
				}
				else
				{
					if (SingofDecimalDecision(fValue * 100))
						sprintf(str, "%s:\\r%2.2f%%\\e[%2.2f%%]\\g]", buf, fValue * 100, fEnchant * 100);
					else
						sprintf(str, "%s:\\r%d%%\\e[%2.2f%%]\\g]", buf, (int)(fValue * 100), fEnchant * 100);
				}
				//end 2013-04-11 by ssjung º“º˝¡°¿Ã ¿÷¿ª∂ß∏∏ º“º˝¡° «•Ω√
			}
			// end 2010. 02. 11 by ckPark πﬂµø∑˘ ¿Â¬¯æ∆¿Ã≈€
		}
		break;
		case FUNCTION_VALUE_TYPE_PROB255:
		{
			// 2010. 02. 11 by ckPark πﬂµø∑˘ ¿Â¬¯æ∆¿Ã≈€
			// 				if( m_pRefEnchant != NULL )
			// 				{
			// 					float fEnchant = GetParamFactor_DesParam( *m_pRefEnchant, bType );
			// 					if( fEnchant == 0)
			// 					{
			// 						sprintf(str, "%s:\\r%.2f%%\\g]",buf,((fValue/255)*100));
			// 					}
			// 					else
			// 					{
			// 						if( fEnchant > 0)
			// 						{
			// 							sprintf(str, "%s:\\r%.2f%%\\e[+%.2f%%]\\g]",buf,(fValue/255)*100, (fEnchant/255)*100);
			// 						}
			// 						else
			// 						{
			// 							sprintf(str, "%s:\\r%.2f%%\\e[%.2f%%]\\g]",buf,(fValue/255)*100, (fEnchant/255)*100);
			// 						}
			// 					}
			// 				}
			// 				else
			// 				{
			// 					sprintf(str, "%s:\\r%.2f%%\\g]",buf,((fValue/255)*100));
			// 				}

			float fEnchant = 0.0f;
			// ±‚∫ª ¿Œ√¶∆Æµ•Ω∫∆ƒ∂˜¿∫ CParamFactorø°º≠
			if (bDefEnchant)
			{
				if (m_pRefEnchant != NULL)
					fEnchant = GetParamFactor_DesParam(*m_pRefEnchant, bType);
			}
			else
			{
				// πﬂµø∑˘ µ•Ω∫∆ƒ∂˜¿∫ πﬂµø∑˘ ¿Œ√¶∆Æ ¡§∫∏ø°º≠
				if (m_pRefItemInfo)
					fEnchant = m_pRefItemInfo->GetInvokeEnchantParamValue(bType);
			}

			if (fEnchant == 0.0f)
			{
				//2013-04-11 by ssjung º“º˝¡°¿Ã ¿÷¿ª∂ß∏∏ º“º˝¡° «•Ω√
				if (SingofDecimalDecision((fValue / 255) * 100))
					sprintf(str, "%s:\\r%.2f%%\\g]", buf, ((fValue / 255) * 100));
				else
					sprintf(str, "%s:\\r%d%%\\g]", buf, (int)((fValue / 255) * 100));

			}
			else
			{
				if (fEnchant > 0.0f)
				{
					if (SingofDecimalDecision((fValue / 255) * 100))
						sprintf(str, "%s:\\r%.2f%%\\e[+%.2f%%]\\g]", buf, (fValue / 255) * 100, (fEnchant / 255) * 100);
					else
						sprintf(str, "%s:\\r%d%%\\e[+%.2f%%]\\g]", buf, (int)((fValue / 255) * 100), (fEnchant / 255) * 100);
				}
				else
				{
					if (SingofDecimalDecision((fValue / 255) * 100))
						sprintf(str, "%s:\\r%.2f%%\\e[%.2f%%]\\g]", buf, (fValue / 255) * 100, (fEnchant / 255) * 100);
					else
						sprintf(str, "%s:\\r%d%%\\e[%.2f%%]\\g]", buf, (int)((fValue / 255) * 100), (fEnchant / 255) * 100);
				}
				//end 2013-04-11 by ssjung º“º˝¡°¿Ã ¿÷¿ª∂ß∏∏ º“º˝¡° «•Ω√
			}
			// end 2010. 02. 11 by ckPark πﬂµø∑˘ ¿Â¬¯æ∆¿Ã≈€
		}
		break;
		}
		//		if(bPercent)
		//		{
		//			sprintf(str, "%s:\\r%d%%\\g]",buf,(int)(fValue*100));
		//		}
		//		else
		//		{
		//			sprintf(str,"%s:\\r%.2f\\g]",buf,fValue);
		//		}
	}

	// 2010. 02. 11 by ckPark πﬂµø∑˘ ¿Â¬¯æ∆¿Ã≈€

	// 	// 2008-09-26 by bhsohn Ω≈±‘ ¿Œ√æ∆Æ √≥∏Æ
	// 	else if(0 ==fValue 
	// 		&& ((DES_DEFENSE_01 == bType)
	// 		    ||(DES_DEFENSE_02 == bType)
	// 			||(DES_DEFENSEPROBABILITY_01 == bType)
	// 			||(DES_DEFENSEPROBABILITY_02 == bType)))
	// 	{
	// 		if( m_pRefEnchant != NULL )
	// 		{
	// 			if( m_pRefEnchant != NULL )
	// 			{
	// 				float fEnchant = GetParamFactor_DesParam( *m_pRefEnchant, bType );
	// 				if( fEnchant == 0)
	// 				{
	// 					sprintf(str, "%s:+%.2f%%]",buf,((fValue/255)*100));
	// 				}
	// 				else
	// 				{
	// 					if( fEnchant > 0)
	// 					{
	// 						sprintf(str, "%s:+%.2f%%\\e[+%.2f%%]\\g]",buf,(fValue/255)*100, (fEnchant/255)*100);
	// 					}
	// 					else
	// 					{
	// 						sprintf(str, "%s:+%.2f%%\\e[%.2f%%]\\g]",buf,(fValue/255)*100, (fEnchant/255)*100);
	// 					}
	// 				}
	// 			}
	// 			else
	// 			{
	// 				sprintf(str, "%s:+%.2f%%]",buf,(fValue/255)*100);
	// 			}
	// 		}
	// 		else
	// 		{
	// 			sprintf(str, "%s]", buf);
	// 		}
	// 	}
	// 	// end 2008-09-26 by bhsohn Ω≈±‘ ¿Œ√æ∆Æ √≥∏Æ
	// 	else
	// 	{
	// 		sprintf(str, "%s]", buf);
	// 	}


	else if (fValue == 0.0f)		// ¿Œ√¶∆Æ∑Œ ¿Œ«ÿ ªı∑Œ ∫Œø©µ» ±‚¥…
	{
		switch (nValueType)
		{
		case FUNCTION_VALUE_TYPE_NORMAL:
		{
			float fEnchant = 0.0f;
			float fBonus = 0.0f;
			// ±‚∫ª ¿Œ√¶∆Æµ•Ω∫∆ƒ∂˜¿∫ CParamFactorø°º≠
			if (Bonus)
			{
				if (m_pRefEnchant != NULL)
					fBonus = GetParamFactor_DesParam(*m_pRefBonus, bType);
			}
			if (bDefEnchant)
			{
				if (m_pRefEnchant != NULL)
					fEnchant = GetParamFactor_DesParam(*m_pRefEnchant, bType);
			}
			else
			{
				// πﬂµø∑˘ µ•Ω∫∆ƒ∂˜¿∫ πﬂµø∑˘ ¿Œ√¶∆Æ ¡§∫∏ø°º≠
				if (m_pRefItemInfo)
					fEnchant = m_pRefItemInfo->GetInvokeEnchantParamValue(bType);
			}

			if (fEnchant != 0.0f)
			{
				if (fEnchant > 0.0f)
				{
					//2013-04-11 by ssjung º“º˝¡°¿Ã ¿÷¿ª∂ß∏∏ º“º˝¡° «•Ω√
					if (SingofDecimalDecision(fValue))
						sprintf(str, "%s:+%.2f\\e[+%.2f]\\g]", buf, fValue, fEnchant);
					else
						sprintf(str, "%s:+%d\\e[+%.2f]\\g]", buf, (int)fValue, fEnchant);
				}
				else
				{
					if (SingofDecimalDecision(fValue))
						sprintf(str, "%s:+%.2f\\e[%.2f]\\g]", buf, fValue, fEnchant);
					else
						sprintf(str, "%s:+%d\\e[%.2f]\\g]", buf, (int)fValue, fEnchant);
				}
			}
			else
			{
				if (bType == DES_RANGEANGLE_02)
					sprintf(str, "%s:+%.2f]", buf, fValue / PI * 180);
				else
				{
					if (SingofDecimalDecision(fValue))
						sprintf(str, "%s:+%.2f]", buf, fValue);
					else
						sprintf(str, "%s:+%d]", buf, (int)fValue);
				}
				//end 2013-04-11 by ssjung º“º˝¡°¿Ã ¿÷¿ª∂ß∏∏ º“º˝¡° «•Ω√
			}
		}
		break;
		case FUNCTION_VALUE_TYPE_PROB:
		{
			float fEnchant = 0.0f;
			float fBonus = 0.0f;
			// ±‚∫ª ¿Œ√¶∆Æµ•Ω∫∆ƒ∂˜¿∫ CParamFactorø°º≠
			if (Bonus)
			{
				if (m_pRefBonus != NULL)
				{
					fBonus = GetParamFactor_DesParam(*m_pRefBonus, bType);
				}
			}

			if (bDefEnchant)
			{
				if (m_pRefEnchant != NULL)
					fEnchant = GetParamFactor_DesParam(*m_pRefEnchant, bType);
			}
			else
			{
				// πﬂµø∑˘ µ•Ω∫∆ƒ∂˜¿∫ πﬂµø∑˘ ¿Œ√¶∆Æ ¡§∫∏ø°º≠
				if (m_pRefItemInfo)
					fEnchant = m_pRefItemInfo->GetInvokeEnchantParamValue(bType);
			}

			if (fEnchant == 0.0f)
			{
				//2013-04-11 by ssjung º“º˝¡°¿Ã ¿÷¿ª∂ß∏∏ º“º˝¡° «•Ω√
				if (SingofDecimalDecision(fValue))
					sprintf(str, "%s:+%2.2f%%]", buf, fValue);
				else
					sprintf(str, "%s:+%d%%]", buf, (int)fValue);
			}
			else
			{
				if (fEnchant > 0.0f)
				{
					if (SingofDecimalDecision(fValue))
						sprintf(str, "%s:+%2.2f%%\\e[+%2.2f%%]\\g]", buf, fValue, fEnchant);
					else
						sprintf(str, "%s:+%d%%\\e[+%2.2f%%]\\g]", buf, (int)fValue, fEnchant);

				}
				else
				{
					if (SingofDecimalDecision(fValue))
						sprintf(str, "%s:+%2.2f%%\\e[%2.2f%%]\\g]", buf, fValue, fEnchant);
					else
						sprintf(str, "%s:+%d%%\\e[%2.2f%%]\\g]", buf, (int)fValue, fEnchant);
					//end 2013-04-11 by ssjung º“º˝¡°¿Ã ¿÷¿ª∂ß∏∏ º“º˝¡° «•Ω√
				}
			}
		}
		break;
		case FUNCTION_VALUE_TYPE_PROB100:
		{
			float fEnchant = 0.0f;
			// ±‚∫ª ¿Œ√¶∆Æµ•Ω∫∆ƒ∂˜¿∫ CParamFactorø°º≠
			if (bDefEnchant)
			{
				if (m_pRefEnchant != NULL)
					fEnchant = GetParamFactor_DesParam(*m_pRefEnchant, bType);
			}
			else
			{
				// πﬂµø∑˘ µ•Ω∫∆ƒ∂˜¿∫ πﬂµø∑˘ ¿Œ√¶∆Æ ¡§∫∏ø°º≠
				if (m_pRefItemInfo)
					fEnchant = m_pRefItemInfo->GetInvokeEnchantParamValue(bType);
			}

			if (fEnchant == 0.0f)
			{
				//2013-04-11 by ssjung º“º˝¡°¿Ã ¿÷¿ª∂ß∏∏ º“º˝¡° «•Ω√
				if (SingofDecimalDecision(fValue * 100))
					sprintf(str, "%s:+%2.2f%%]", buf, fValue * 100);
				else
					sprintf(str, "%s:+%d%%]", buf, (int)(fValue * 100));
			}
			else
			{
				if (fEnchant > 0.0f)
				{
					if (SingofDecimalDecision(fValue * 100))
						sprintf(str, "%s:+%2.2f%%\\e[+%2.2f%%]\\g]", buf, fValue * 100, fEnchant * 100);
					else
						sprintf(str, "%s:+%d%%\\e[+%2.2f%%]\\g]", buf, (int)(fValue * 100), fEnchant * 100);
				}
				else
				{
					if (SingofDecimalDecision(fValue * 100))
						sprintf(str, "%s:+%2.2f%%\\e[%2.2f%%]\\g]", buf, fValue * 100, fEnchant * 100);
					else
						sprintf(str, "%s:+%d%%\\e[%2.2f%%]\\g]", buf, (int)(fValue * 100), fEnchant * 100);
					//end 2013-04-11 by ssjung º“º˝¡°¿Ã ¿÷¿ª∂ß∏∏ º“º˝¡° «•Ω√
				}
			}
		}
		break;
		case FUNCTION_VALUE_TYPE_PROB255:
		{
			float fEnchant = 0.0f;
			// ±‚∫ª ¿Œ√¶∆Æµ•Ω∫∆ƒ∂˜¿∫ CParamFactorø°º≠
			if (bDefEnchant)
			{
				if (m_pRefEnchant != NULL)
					fEnchant = GetParamFactor_DesParam(*m_pRefEnchant, bType);
			}
			else
			{
				// πﬂµø∑˘ µ•Ω∫∆ƒ∂˜¿∫ πﬂµø∑˘ ¿Œ√¶∆Æ ¡§∫∏ø°º≠
				if (m_pRefItemInfo)
					fEnchant = m_pRefItemInfo->GetInvokeEnchantParamValue(bType);
			}

			if (fEnchant == 0.0f)
			{
				//2013-04-11 by ssjung º“º˝¡°¿Ã ¿÷¿ª∂ß∏∏ º“º˝¡° «•Ω√
				if (SingofDecimalDecision((fValue / 255) * 100))
					sprintf(str, "%s:+%.2f%%]", buf, ((fValue / 255) * 100));
				else
					sprintf(str, "%s:+%d%%]", buf, (int)((fValue / 255) * 100));
			}
			else
			{
				if (fEnchant > 0.0f)
				{
					if (SingofDecimalDecision((fValue / 255) * 100))
						sprintf(str, "%s:+%.2f%%\\e[+%.2f%%]\\g]", buf, (fValue / 255) * 100, (fEnchant / 255) * 100);
					else
						sprintf(str, "%s:+%d%%\\e[+%.2f%%]\\g]", buf, (int)((fValue / 255) * 100), (fEnchant / 255) * 100);
				}
				else
				{
					if (SingofDecimalDecision((fValue / 255) * 100))
						sprintf(str, "%s:+%.2f%%\\e[%.2f%%]\\g]", buf, (fValue / 255) * 100, (fEnchant / 255) * 100);
					else
						sprintf(str, "%s:+%d%%\\e[%.2f%%]\\g]", buf, (int)((fValue / 255) * 100), (fEnchant / 255) * 100);
					//end 2013-04-11 by ssjung º“º˝¡°¿Ã ¿÷¿ª∂ß∏∏ º“º˝¡° «•Ω√
				}
			}
		}
		break;
		// 2008-09-26 by bhsohn Ω≈±‘ ¿Œ√æ∆Æ √≥∏Æ
		case FUNCTION_VALUE_TYPE_PERCENT:
		{
			if (SingofDecimalDecision(fValue*100.0f))
				sprintf(str, "%s:+%.2f%%]", buf, (fValue*100.0f));
			else
				sprintf(str, "%s:+%d%%]", buf, (int)(fValue*100.0f));
		}
		break;
		}
	}
	// end 2010. 02. 11 by ckPark πﬂµø∑˘ ¿Â¬¯æ∆¿Ã≈€
	ZERO_MEMORY(buf);
}

#else
void CINFItemInfo::SetParameterInfo( char * str, BYTE bType,float fValue, BOOL bDefEnchant /* = TRUE */ )
// end 2010. 02. 11 by ckPark πﬂµø∑˘ ¿Â¬¯æ∆¿Ã≈€
{
	FLOG( "CINFItemInfo::SetParameterInfo(char * str, BYTE bType,float fValue)" );
	char buf[128];
	FUNCTION_VALUE_TYPE nValueType = FUNCTION_VALUE_TYPE_NORMAL;
	memset(buf,0x00, sizeof(buf));
	switch(bType)
	{
	case DES_NULL:					// ¥ÎªÛ ∆ƒ∂ÛπÃ≈Õ∞° æ¯¥¬ ∞ÊøÏ ªÁøÎ
		{
			wsprintf(str,STRMSG_C_PARAM_0001);//"¡§∫∏ æ¯¿Ω"
			return;
		}
		break;
	case DES_ATTACK_PART:			// ∞¯∞›
		wsprintf(buf,STRMSG_C_PARAM_0002);//"[∞¯∞›"
		break;
	case DES_DEFENSE_PART:		// ≥ª±∏-->πÊæÓ
		wsprintf(buf,STRMSG_C_PARAM_0003);//"[πÊæÓ"
		break;
	case DES_FUEL_PART:				// ø¨∑·
		wsprintf(buf,STRMSG_C_PARAM_0004);//"[ø¨∑·"
		break;
	case DES_SOUL_PART:				// ∞®¿¿
		wsprintf(buf,STRMSG_C_PARAM_0005);//"[¡§Ω≈"
		break;
	case DES_SHIELD_PART:			// πÊæÓ-->ΩØµÂ
		wsprintf(buf,STRMSG_C_PARAM_0006);//"[ΩØµÂ"
		break;
	case DES_DODGE_PART:			// »∏««
		wsprintf(buf,STRMSG_C_PARAM_0007);//"[»∏««"
		break;
	case DES_BODYCONDITION:			// ∏ˆªÛ≈¬(Ω√¡Ó∏µÂ,∆¯∞›∏µÂ)
		wsprintf(str,STRMSG_C_PARAM_0008);//"[¿Ø¥÷ªÛ≈¬∫Ø»≠]"
		return;
	case DES_ENDURANCE_01: 			// ≥ª±∏µµ 01
		wsprintf(buf,STRMSG_C_PARAM_0009);//"[±‚∫ªπ´±‚ ≥ª±∏µµ"
		break; 
	case DES_ENDURANCE_02:			// ≥ª±∏µµ 02
		wsprintf(buf,STRMSG_C_PARAM_0009);//"[±‚∫ªπ´±‚ ≥ª±∏µµ"
		break;
	case DES_CHARGING_01:			// ¿Â≈∫ºˆ 01
		wsprintf(buf,STRMSG_C_PARAM_0010);//"[±‚∫ªπ´±‚ ¿Â≈∫ºˆ"
		break;
	case DES_CHARGING_02:			// ¿Â≈∫ºˆ 02
		wsprintf(buf,STRMSG_C_PARAM_0011);//"[∞Ì±ﬁπ´±‚ ¿Â≈∫ºˆ"
		break;
	case DES_PROPENSITY:			// º∫«‚
		wsprintf(buf,STRMSG_C_PARAM_0012);//"[º∫«‚"
		break;
	case DES_HP:					// »˜∆Æ∆˜¿Œ∆Æ
		wsprintf(buf,STRMSG_C_PARAM_0013);//"[ø°≥ ¡ˆ"
		break;
	case DES_MAX_SP_UP:					// 2010-08-26 by shcho&&jskim, π„ æ∆¿Ã≈€ ±∏«ˆ
	case DES_SP:					// º“øÔ∆˜¿Œ∆Æ
		wsprintf(buf,STRMSG_C_PARAM_0014);//"[Ω∫≈≥∆˜¿Œ∆Æ"
		break;
	case DES_EP:					// ø£¡¯∆˜¿Œ∆Æ
		wsprintf(buf,STRMSG_C_PARAM_0004);//"[ø¨∑·"
		break;
	case DES_DP:					// ø£¡¯∆˜¿Œ∆Æ
		wsprintf(buf,STRMSG_C_PARAM_0006);//"[ΩØµÂ"
		break;
	case DES_SPRECOVERY:			// º“øÔ∆˜¿Œ∆Æ»∏∫π∑¬			C
		wsprintf(buf,STRMSG_C_PARAM_0015);//"[Ω∫≈≥∆˜¿Œ∆Æ»∏∫π∑¬"
		break;
	case DES_HPRECOVERY:			// ø°≥ ¡ˆ∆˜¿Œ∆Æ»∏∫π∑¬		C
		wsprintf(buf,STRMSG_C_PARAM_0016);//"[ø°≥ ¡ˆ»∏∫π∑¬"
		break;
	case DES_MINATTACK_01:				//  ∞¯∞›∑¬ 1«¸			C
		wsprintf(buf,STRMSG_C_PARAM_0017);//"[±‚∫ªπ´±‚ ∞¯∞›∑¬(√÷º“)"
		nValueType = FUNCTION_VALUE_TYPE_PROB100;
		break;
	case DES_MINATTACK_02:	            //  ∞¯∞›∑¬ 2«¸			C
		wsprintf(buf,STRMSG_C_PARAM_0018);//"[∞Ì±ﬁπ´±‚ ∞¯∞›∑¬(√÷º“)"
		nValueType = FUNCTION_VALUE_TYPE_PROB100;
		break;
	case DES_MAXATTACK_01:				//  ∞¯∞›∑¬ 1«¸			C
		wsprintf(buf,STRMSG_C_PARAM_0019);//"[±‚∫ªπ´±‚ ∞¯∞›∑¬(√÷¥Î)"
		nValueType = FUNCTION_VALUE_TYPE_PROB100;
		break;
	case DES_MAXATTACK_02:	            //  ∞¯∞›∑¬ 2«¸			C
		wsprintf(buf,STRMSG_C_PARAM_0020);//"[∞Ì±ﬁπ´±‚ ∞¯∞›∑¬(√÷¥Î)"
		nValueType = FUNCTION_VALUE_TYPE_PROB100;
		break;
	case DES_ATTACKPROBABILITY_01:	// ∞¯∞›»Æ∑¸ 01				C, PROB256_MAX_VALUE
		wsprintf(buf,STRMSG_C_PARAM_0021);//"[±‚∫ªπ´±‚ ∞¯∞›»Æ∑¸"
		// 2010-07-28 by dgwoo »Æ∑¸ ºˆΩƒ ∫Ø∞Ê (255 => 100%)
		//nValueType = FUNCTION_VALUE_TYPE_PROB255;
		nValueType = FUNCTION_VALUE_TYPE_PROB;
		break;
	case DES_ATTACKPROBABILITY_02:    // ∞¯∞›»Æ∑¸ 02				C, PROB256_MAX_VALUE
		wsprintf(buf,STRMSG_C_PARAM_0022);//"[∞Ì±ﬁπ´±‚ ∞¯∞›»Æ∑¸"
		// 2010-07-28 by dgwoo »Æ∑¸ ºˆΩƒ ∫Ø∞Ê (255 => 100%)
		//nValueType = FUNCTION_VALUE_TYPE_PROB255;
		nValueType = FUNCTION_VALUE_TYPE_PROB;
		break;
	case DES_DEFENSE_01:			// πÊæÓ∑¬ 01				C, PROB256_MAX_VALUE
		wsprintf(buf,STRMSG_C_PARAM_0023);//"[±‚∫ªπ´±‚ø° ¥Î«— πÊæÓ∑¬"
		// 2010-07-28 by dgwoo »Æ∑¸ ºˆΩƒ ∫Ø∞Ê (255 => 100%)
		//nValueType = FUNCTION_VALUE_TYPE_PROB255;
		nValueType = FUNCTION_VALUE_TYPE_PROB;
		break;
	case DES_DEFENSE_02:			// πÊæÓ∑¬ 02				C, PROB256_MAX_VALUE
		wsprintf(buf,STRMSG_C_PARAM_0024);//"[∞Ì±ﬁπ´±‚ø° ¥Î«— πÊæÓ∑¬"
		// 2010-07-28 by dgwoo »Æ∑¸ ºˆΩƒ ∫Ø∞Ê (255 => 100%)
		//nValueType = FUNCTION_VALUE_TYPE_PROB255;
		nValueType = FUNCTION_VALUE_TYPE_PROB;
		break;
	case DES_DEFENSEPROBABILITY_01:	// πÊæÓ»Æ∑¸ 01				C, PROB256_MAX_VALUE
		wsprintf(buf,STRMSG_C_PARAM_0025);//"[±‚∫ªπ´±‚ø° ¥Î«— »∏««∑¬"
		// 2010-07-28 by dgwoo »Æ∑¸ ºˆΩƒ ∫Ø∞Ê (255 => 100%)
		//nValueType = FUNCTION_VALUE_TYPE_PROB255;
		nValueType = FUNCTION_VALUE_TYPE_PROB;
		break;
	case DES_DEFENSEPROBABILITY_02:	// πÊæÓ»Æ∑¸ 02				C, PROB256_MAX_VALUE
		wsprintf(buf,STRMSG_C_PARAM_0026);//"[∞Ì±ﬁπ´±‚ø° ¥Î«— »∏««∑¬"
		// 2010-07-28 by dgwoo »Æ∑¸ ºˆΩƒ ∫Ø∞Ê (255 => 100%)
		//nValueType = FUNCTION_VALUE_TYPE_PROB255;
		nValueType = FUNCTION_VALUE_TYPE_PROB;
		break;
	case DES_SKILLPROBABILITY_01:		// ∏∂¿ŒµÂ ƒ¡∆Æ∑—∞¯∞›»Æ∑¸				C, PROB256_MAX_VALUE
		wsprintf(buf,STRMSG_C_PARAM_0027);//"[Ω∫≈≥ ±‚∫ªπ´±‚ ∞¯∞›»Æ∑¸"
		nValueType = FUNCTION_VALUE_TYPE_PROB255;
		break;
	case DES_SKILLPROBABILITY_02:		// ∏∂¿ŒµÂ ƒ¡∆Æ∑—∞¯∞›»Æ∑¸				C, PROB256_MAX_VALUE
		wsprintf(buf,STRMSG_C_PARAM_0028);//"[Ω∫≈≥ ∞Ì±ﬁπ´±‚ ∞¯∞›»Æ∑¸"
		nValueType = FUNCTION_VALUE_TYPE_PROB255;
		break;
	case DES_FACTIONRESISTANCE_01:		// º”º∫¿˙«◊∑¬				C, PROB256_MAX_VALUE
		wsprintf(buf,STRMSG_C_PARAM_0029);//"[±‚∫ªπ´±‚ º”º∫¿˙«◊∑¬"
		nValueType = FUNCTION_VALUE_TYPE_PROB255;
		break;
	case DES_FACTIONRESISTANCE_02:		// º”º∫¿˙«◊∑¬				C, PROB256_MAX_VALUE
		wsprintf(buf,STRMSG_C_PARAM_0030);//"[∞Ì±ﬁπ´±‚ º”º∫¿˙«◊∑¬"
		nValueType = FUNCTION_VALUE_TYPE_PROB255;
		break;
	case DES_SPEED:					// ¿Ãµøº”µµ					C
		wsprintf(buf,STRMSG_C_PARAM_0031);//"[¿Ãµøº”µµ"
		nValueType = FUNCTION_VALUE_TYPE_PROB100; // 2005-11-26 by ispark
		break;
	case DES_TRANSPORT:				// øÓπ›∑¬
		wsprintf(buf,STRMSG_C_PARAM_0032);//"[øÓπ›∑¬"
		break;
	case DES_MATERIAL:				// ¿Á¡˙
		wsprintf(buf,STRMSG_C_PARAM_0033);//"[¿Á¡˙"
		break;
	case DES_REATTACKTIME_01:		// (*) ∏ÆæÓ≈√≈∏¿” 01		C
		wsprintf(buf,STRMSG_C_PARAM_0034);//"[±‚∫ªπ´±‚ ¿Á∞¯∞›Ω√∞£"
		nValueType = FUNCTION_VALUE_TYPE_PROB100;
		break; 
	case DES_REATTACKTIME_02:		// (*) ∏ÆæÓ≈√≈∏¿” 02		C
		wsprintf(buf,STRMSG_C_PARAM_0035);//"[∞Ì±ﬁπ´±‚ ¿Á∞¯∞›Ω√∞£"
		nValueType = FUNCTION_VALUE_TYPE_PROB100;
		break;
	case DES_ABRASIONRATE_01:		// ∏∂∏¿≤ 01				C, PROB256_MAX_VALUE
		wsprintf(buf,STRMSG_C_PARAM_0036);//"[±‚∫ªπ´±‚ ∏∂∏¿≤"
		nValueType = FUNCTION_VALUE_TYPE_PROB255;
		break;
	case DES_ABRASIONRATE_02:		// ∏∂∏¿≤ 02				C, PROB256_MAX_VALUE
		wsprintf(buf,STRMSG_C_PARAM_0037);//"[∞Ì±ﬁπ´±‚ ∏∂∏¿≤"
		nValueType = FUNCTION_VALUE_TYPE_PROB255;
		break;
	case DES_RANGE_01:				// (*) ¿Ø»ø∞≈∏Æ 01			C
		wsprintf(buf,STRMSG_C_PARAM_0038);//"[±‚∫ªπ´±‚ ¿Ø»ø∞≈∏Æ"
		nValueType = FUNCTION_VALUE_TYPE_PROB100;
		break;
	case DES_RANGE_02:				// (*) ¿Ø»ø∞≈∏Æ 02			C
		wsprintf(buf,STRMSG_C_PARAM_0039);//"[∞Ì±ﬁπ´±‚ ªÁ∞≈∏Æ"
		nValueType = FUNCTION_VALUE_TYPE_PROB100;
		break;
	case DES_RANGEANGLE_01:			// ¿Ø»ø∞¢µµ 01				C
		wsprintf(buf,STRMSG_C_PARAM_0040);//"[±‚∫ªπ´±‚ ¿Ø»ø∞¢µµ"
		break;	
	case DES_SKILL_BUFF_MON_ATTACK_POWER:
		wsprintf(buf, STRMSG_C_130520_0001);
		nValueType = FUNCTION_VALUE_TYPE_PROB100;		
		break;
	case DES_RANGEANGLE_02:			// ¿Ø»ø∞¢µµ 02				C
		wsprintf(buf,STRMSG_C_PARAM_0041);//"[∞Ì±ﬁπ´±‚ ¿Ø»ø∞¢µµ"
		break;
	case DES_MULTITAGET_01:			// ∏÷∆º≈∏∞Ÿ					C
		wsprintf(buf,STRMSG_C_PARAM_0042);//"[±‚∫ªπ´±‚ ∏÷∆º≈∏∞Ÿ"
		break;
	case DES_MULTITAGET_02:			// ∏÷∆º≈∏∞Ÿ					C
		wsprintf(buf,STRMSG_C_PARAM_0043);//"[∞Ì±ﬁπ´±‚ ∏÷∆º≈∏∞Ÿ"
		break;
	case DES_EXPLOSIONRANGE_01:		// ∆¯πﬂπ›∞Ê					C
		wsprintf(buf,STRMSG_C_PARAM_0044);//"[±‚∫ªπ´±‚ ∆¯πﬂπ›∞Ê"
		break;
	case DES_EXPLOSIONRANGE_02:		// ∆¯πﬂπ›∞Ê					C
		wsprintf(buf,STRMSG_C_PARAM_0045);//"[∞Ì±ﬁπ´±‚ ∆¯πﬂπ›∞Ê"
		break;
	case DES_UNIT:					// ¿Ø¥÷¿« ¡æ∑˘ (28 ~ 29¿Ã ∞∞¿Ã æ≤ø© ¿Ø¥÷∏∂¥Ÿ¿« ∫∏¡§∞™¿∏∑Œ ªÁøÎµ )
		wsprintf(buf,STRMSG_C_PARAM_0046);//"[¿Ø¥÷¿« ¡æ∑˘"
		break;
	case DES_REVISION:				// ¿Ø¥÷¿« ∫∏¡§∞™ (28 ~ 29¿Ã ∞∞¿Ã æ≤ø© ¿Ø¥÷∏∂¥Ÿ¿« ∫∏¡§∞™¿∏∑Œ ªÁøÎµ )
		wsprintf(buf,STRMSG_C_PARAM_0047);//"[¿Ø¥÷¿« ∫∏¡§∞™"
		break;
	case DES_FACTIONPROBABILITY_01:		// º”º∫ø° ¥Î«— πÊæÓ»Æ∑¸		C, PROB256_MAX_VALUE
		wsprintf(buf,STRMSG_C_PARAM_0048);//"[±‚∫ªπ´±‚ º”º∫ø° ¥Î«— »∏««∑¬"
		nValueType = FUNCTION_VALUE_TYPE_PROB255;
		break;
	case DES_FACTIONPROBABILITY_02:		// º”º∫ø° ¥Î«— πÊæÓ»Æ∑¸		C, PROB256_MAX_VALUE
		wsprintf(buf,STRMSG_C_PARAM_0049);//"[∞Ì±ﬁπ´±‚ º”º∫ø° ¥Î«— »∏««∑¬"
		nValueType = FUNCTION_VALUE_TYPE_PROB255;
		break;
	case DES_SHOTNUM_01:				// ¿œ¡°ªÁ ¥Á πﬂªÁ ºˆ		C
		wsprintf(buf,STRMSG_C_PARAM_0050);//"[±‚∫ªπ´±‚ ø¨ªÁ∑¬"
		break;
	case DES_SHOTNUM_02:				// ¿œ¡°ªÁ ¥Á πﬂªÁ ºˆ		C
		wsprintf(buf,STRMSG_C_PARAM_0051);//"[∞Ì±ﬁπ´±‚ ø¨ªÁ∑¬"
		break;
	case DES_MULTINUM_01:				// µøΩ√ πﬂªÁ ºˆ				C
		wsprintf(buf,STRMSG_C_PARAM_0052);//"[±‚∫ªπ´±‚ ∏÷∆ºº¶"
		break;
	case DES_MULTINUM_02:				// µøΩ√ πﬂªÁ ºˆ				C
		wsprintf(buf,STRMSG_C_PARAM_0053);//"[∞Ì±ﬁπ´±‚ ∏÷∆ºº¶"
		break;
	case DES_ATTACKTIME_01:			// √≥¿Ω ∞¯∞› Ω√¿« ≈∏¿” 01	C
		wsprintf(buf,STRMSG_C_PARAM_0054);//"[±‚∫ªπ´±‚ πﬂµøΩ√∞£"
		break;
	case DES_ATTACKTIME_02:			// √≥¿Ω ∞¯∞› Ω√¿« ≈∏¿” 02	C
		wsprintf(buf,STRMSG_C_PARAM_0055);//"[∞Ì±ﬁπ´±‚ πﬂµøΩ√∞£"
		break;
	case DES_TIME_01:				// (*)∞˙ø≠ Ω√∞£ 01				C
		wsprintf(buf,STRMSG_C_PARAM_0056);//"[±‚∫ªπ´±‚ ∞˙ø≠Ω√∞£"
		nValueType = FUNCTION_VALUE_TYPE_PROB100;
		break;
	case DES_TIME_02:				// (*)∞˙ø≠ Ω√∞£ 02				C
		wsprintf(buf,STRMSG_C_PARAM_0057);//"[∞Ì±ﬁπ´±‚ ∞˙ø≠Ω√∞£"
		nValueType = FUNCTION_VALUE_TYPE_PROB100;
		break;
	case DES_WEIGHT_01:				// (*)π´∞‘ 			C
		wsprintf(buf,STRMSG_C_PARAM_0058);//"[±‚∫ªπ´±‚ π´∞‘"
		nValueType = FUNCTION_VALUE_TYPE_PROB100;
		break;
	case DES_WEIGHT_02:				// (*)π´∞‘ 			C
		wsprintf(buf,STRMSG_C_PARAM_0059);//"[∞Ì±ﬁπ´±‚ π´∞‘"
		nValueType = FUNCTION_VALUE_TYPE_PROB100;
		break;
	case DES_REACTION_RANGE:
		wsprintf(buf,STRMSG_C_PARAM_0060);//"[∞Ì±ﬁπ´±‚ π›¿¿∞≈∏Æ"
		break;
	case DES_OVERHITTIME_01:// (*) ø¿πˆ»˝
	case DES_OVERHITTIME_02:// (*) ø¿πˆ»˝
		wsprintf(buf,STRMSG_C_050620_0001);//"[ø¿πˆ»˝»∏∫πΩ√∞£"
		break;
	case DES_BAZAAR_SELL:
		wsprintf(str,STRMSG_C_060801_0008);//"[∆«∏≈ªÛ¡°¿ª Ω√¿€«“ ºˆ ¿÷¥Ÿ. πŸ¿⁄∏ ¿« ºΩ≈Õ ?ø°º≠∏∏ ªÁøÎ∞°¥…]"
		return;
	case DES_BAZAAR_BUY:
		wsprintf(str,STRMSG_C_060801_0009);//"[±∏∏≈ªÛ¡°¿ª Ω√¿€«“ ºˆ ¿÷¥Ÿ. πŸ¿⁄∏ ¿« ºΩ≈Õ ?ø°º≠∏∏ ªÁøÎ∞°¥…]"
		return;
	case DES_UNITKIND:
		{// ±‚√º æ˜±◊∑π¿ÃµÂΩ√ «ÿ¥Á ±‚√º
			wsprintf(buf,STRMSG_C_PARAM_0061);//"[æ˜±◊∑π¿ÃµÂ ±‚√º:"
			switch((USHORT)fValue)
			{
				case UNITKIND_BT01:
					sprintf(str,STRMSG_C_PARAM_0062,buf);//"%sB-GEAR 1«¸]"
					break;
				case UNITKIND_BT02:
					sprintf(str,STRMSG_C_PARAM_0063,buf);//"%sB-GEAR 2«¸]"
					break;
				case UNITKIND_BT03:
					sprintf(str,STRMSG_C_PARAM_0064,buf);//"%sB-GEAR 3«¸]"
					break;
				case UNITKIND_BT04:
					sprintf(str,STRMSG_C_PARAM_0065,buf);//"%sB-GEAR 4«¸]"
					break;
				case UNITKIND_OT01:
					sprintf(str,STRMSG_C_PARAM_0066,buf);//"%sM-GEAR 1«¸]"
					break;
				case UNITKIND_OT02:
					sprintf(str,STRMSG_C_PARAM_0067,buf);//"%sM-GEAR 2«¸]"
					break;
				case UNITKIND_OT03:
					sprintf(str,STRMSG_C_PARAM_0068,buf);//"%sM-GEAR 3«¸]"
					break;
				case UNITKIND_OT04:
					sprintf(str,STRMSG_C_PARAM_0069,buf);//"%sM-GEAR 4«¸]"
					break;
				case UNITKIND_DT01:
					sprintf(str,STRMSG_C_PARAM_0070,buf);//"%sA-GEAR 1«¸]"
					break;
				case UNITKIND_DT02:
					sprintf(str,STRMSG_C_PARAM_0071,buf);//"%sA-GEAR 2«¸]"
					break;
				case UNITKIND_DT03:
					sprintf(str,STRMSG_C_PARAM_0072,buf);//"%sA-GEAR 3«¸]"
					break;
				case UNITKIND_DT04:
					sprintf(str,STRMSG_C_PARAM_0073,buf);//"%sA-GEAR 4«¸]"
					break;
				case UNITKIND_ST01:
					sprintf(str,STRMSG_C_PARAM_0074,buf);//"%sI-GEAR 1«¸]"
					break;
				case UNITKIND_ST02:
					sprintf(str,STRMSG_C_PARAM_0075,buf);//"%sI-GEAR 2«¸]"
					break;
				case UNITKIND_ST03:
					sprintf(str,STRMSG_C_PARAM_0076,buf);//"%sI-GEAR 3«¸]"
					break;
				case UNITKIND_ST04:
					sprintf(str,STRMSG_C_PARAM_0077,buf);//"%sI-GEAR 4«¸]"
					break;
			}
			return;
		}
		break;
	case DES_ITEMKIND:				// æ∆¿Ã≈€¡æ∑˘
		{
			wsprintf(buf,STRMSG_C_PARAM_0078);//"[æ∆¿Ã≈€:"
			switch((USHORT)fValue)
			{
				case ITEMKIND_AUTOMATIC:
					sprintf(str,STRMSG_C_PARAM_0079,buf);//"%sø¿≈‰∏≈∆Ω]"
					break;
				case ITEMKIND_VULCAN:
					sprintf(str,STRMSG_C_PARAM_0080,buf);//"%sπﬂƒ≠]"
					break;
//				case ITEMKIND_GRENADE:
//					sprintf(str,STRMSG_C_PARAM_0081,buf);//"%s±◊∑π≥◊¿ÃµÂ]" -> "%sµ‡æÛ∏ÆΩ∫∆Æ]"
				case ITEMKIND_DUALIST:						// 2005-08-02 by ispark
					sprintf(str,STRMSG_C_050802_0001,buf);//"%sµ‡æÛ∏ÆΩ∫∆Æ]"
					break;
				case ITEMKIND_CANNON:
					sprintf(str,STRMSG_C_PARAM_0082,buf);//"%sƒ≥≥Ì]"
					break;
				case ITEMKIND_RIFLE:
					sprintf(str,STRMSG_C_PARAM_0083,buf);//"%s∂Û¿Ã«√]"
					break;
				case ITEMKIND_GATLING:
					sprintf(str,STRMSG_C_PARAM_0084,buf);//"%s∞≥∆≤∏µ]"
					break;
				case ITEMKIND_LAUNCHER:
					sprintf(str,STRMSG_C_PARAM_0085,buf);//"%s∑±√ƒ]"
					break;
				case ITEMKIND_MASSDRIVE:
					sprintf(str,STRMSG_C_PARAM_0086,buf);//"%s∏≈Ω∫µÂ∂Û¿Ã∫Í]"
					break;
				case ITEMKIND_ROCKET:
					sprintf(str,STRMSG_C_PARAM_0087,buf);//"%s∑Œƒœ∑˘]"
					break;
				case ITEMKIND_MISSILE:
					sprintf(str,STRMSG_C_PARAM_0088,buf);//"%sπÃªÁ¿œ∑˘]"
					break;
				case ITEMKIND_BUNDLE:
					sprintf(str,STRMSG_C_PARAM_0089,buf);//"%sπ¯µÈ∑˘]"
					break;
				case ITEMKIND_MINE:
					sprintf(str,STRMSG_C_PARAM_0090,buf);//"%s∏∂¿Œ∑˘]"
					break;
				case ITEMKIND_SHIELD:
					sprintf(str,STRMSG_C_PARAM_0091,buf);//"%sΩØµÂ]"
					break;
				case ITEMKIND_DUMMY:
					sprintf(str,STRMSG_C_PARAM_0092,buf);//"%s¥ıπÃ]"
					break;
				case ITEMKIND_FIXER:
					sprintf(str,STRMSG_C_PARAM_0093,buf);//"%s«»º≠]"
					break;
				case ITEMKIND_DECOY:
					sprintf(str,STRMSG_C_PARAM_0094,buf);//"%sµƒ⁄¿Ã]"
					break;
				case ITEMKIND_ALL_WEAPON:
					sprintf(str,STRMSG_C_PARAM_0095,buf);//"%s∏µÁ π´±‚]"
					break;
				case ITEMKIND_PRIMARY_WEAPON_ALL:
					sprintf(str,STRMSG_C_PARAM_0096,buf);//"%s±‚∫ªπ´±‚]"
					break;
				case ITEMKIND_PRIMARY_WEAPON_1:
					sprintf(str,STRMSG_C_PARAM_0097,buf);//"%s√—æÀ«¸ ±‚∫ªπ´±‚]"
					break;
				case ITEMKIND_PRIMARY_WEAPON_2:
					sprintf(str,STRMSG_C_PARAM_0098,buf);//"%sø¨∑·«¸ ±‚∫ªπ´±‚]"
					break;
				case ITEMKIND_SECONDARY_WEAPON_ALL:
					sprintf(str,STRMSG_C_PARAM_0099,buf);//"%s∞Ì±ﬁπ´±‚]"
					break;
				case ITEMKIND_SECONDARY_WEAPON_1:
					sprintf(str,STRMSG_C_PARAM_0099,buf);//"%s∞Ì±ﬁπ´±‚]"
					break;
				case ITEMKIND_SECONDARY_WEAPON_2:
					sprintf(str,STRMSG_C_PARAM_0100,buf);//"%s∞Ì±ﬁπ´±‚(∫∏¡∂)]"
					break;
				case ITEMKIND_DEFENSE:
					sprintf(str,STRMSG_C_PARAM_0101,buf);//"%sæ∆∏”]"
					break;
				case ITEMKIND_SUPPORT:
					sprintf(str,STRMSG_C_PARAM_0102,buf);//"%s∫∏¡∂/ø£¡¯]"
					break;
				case ITEMKIND_BLASTER:					// 2005-08-02 by ispark
					sprintf(str,STRMSG_C_050802_0002,buf);//"%s∫Ì∑°Ω∫≈Õ]"
					break;
				case ITEMKIND_RAILGUN:					// 2005-08-02 by ispark
					sprintf(str,STRMSG_C_050802_0003,buf);//"%s∑π¿œ∞«]"
					break;
			}
			return;
		}
		break;
	case DES_GROUNDMODE:			// ∆¯∞› ∏µÂ
		wsprintf(buf,STRMSG_C_PARAM_0103);//"[∆¯∞› ∏µÂ"
		break;
	case DES_SIEGEMODE:				// Ω√¡Ó ∏µÂ
		wsprintf(buf,STRMSG_C_PARAM_0104);//"[Ω√¡Ó ∏µÂ"
		break;
	case DES_IMMEDIATE_HP_UP:
		wsprintf(buf,STRMSG_C_PARAM_0105);//"[ø°≥ ¡ˆ"
		break;
	case DES_IMMEDIATE_DP_UP:
		wsprintf(buf,STRMSG_C_PARAM_0106);//"[ΩØµÂ"
		break;
	case DES_IMMEDIATE_SP_UP:
		wsprintf(buf,STRMSG_C_PARAM_0107);//"[Ω∫≈≥P"
		break;
	case DES_IMMEDIATE_EP_UP:
		wsprintf(buf,STRMSG_C_PARAM_0108);//"[ø¨∑·"
		break;
// 2004-12-06 by ydkim ¿Œ√¶∆Æ ∞¸∑√
	case DES_RARE_FIX_PREFIX:
		wsprintf(str,STRMSG_C_PARAM_0109);//"¡¢µŒªÁ"
		return;
	case DES_RARE_FIX_SUFFIX:
		wsprintf(str,STRMSG_C_PARAM_0110);//"¡¢πÃªÁ"
		return;
	case DES_RARE_FIX_BOTH:
		wsprintf(str,STRMSG_C_PARAM_0111);//"¡¢µŒªÁ, ¡¢πÃªÁ"
		return;
	case DES_RARE_FIX_PREFIX_INITIALIZE:
		wsprintf(str,STRMSG_C_PARAM_0112);//"∏µÁπ´±‚¿« ¡¢µŒªÁ ∑πæÓ ø…º« √ ±‚»≠"
		return;
	case DES_RARE_FIX_SUFFIX_INITIALIZE:
		wsprintf(str,STRMSG_C_PARAM_0113);//"∏µÁπ´±‚¿« ¡¢πÃªÁ ∑πæÓ ø…º« √ ±‚»≠"
		return;
// 2005-11-22 by ispark ªı∑Œ ±∏«ˆµ» Ω∫≈≥ ¡§∫∏
	case DES_SKILL_REDUCE_SHIELD_DAMAGE:
		wsprintf(buf,STRMSG_C_051122_0001);// "[ΩØµÂ µ•πÃ¡ˆ ∞®º“"
		nValueType = FUNCTION_VALUE_TYPE_PROB100;
		break;
	case DES_ATTACK_RANGE_01:
		wsprintf(buf,STRMSG_C_051122_0003);// "[∑π¿Ã¥Ÿ 1«¸ ªÁ∞≈∏Æ ¡ı∞°"
		nValueType = FUNCTION_VALUE_TYPE_PERCENT;
		break;
	case DES_ATTACK_RANGE_02:
		wsprintf(buf,STRMSG_C_051122_0004);// "[∑π¿Ã¥Ÿ 2«¸ ªÁ∞≈∏Æ ¡ı∞°"
		nValueType = FUNCTION_VALUE_TYPE_PERCENT;
		break;
	case DES_SKILL_COLLISIONDAMAGE_DOWN:
		wsprintf(buf,STRMSG_C_051122_0002);// "[√Ê∞› µ•πÃ¡ˆ ∞®º“"
		nValueType = FUNCTION_VALUE_TYPE_PROB100;
		break;
	case DES_SKILL_REVERSEENGINE:
		wsprintf(str,STRMSG_C_051126_0001);//  "[»ƒ¡¯ ∞°¥…]"
		return;
	case DES_SKILL_SMARTSP:
		wsprintf(buf,STRMSG_C_051122_0008);// "[SP º“∏∑Æ ∞®º“"
		nValueType = FUNCTION_VALUE_TYPE_PROB100;
		break;
	case DES_SKILL_SUMMON_FORMATION_MEMBER:
		wsprintf(str, STRMSG_C_051125_0001);// "[∆Ì¥Îø¯ 1∏Ì º“»Ø]"
		return;
	case DES_SKILL_REACTIONSPEED:
		wsprintf(buf, STRMSG_C_051128_0001);// "[ø£¡¯ º±»∏∑¬"
		nValueType = FUNCTION_VALUE_TYPE_PROB100;
		break;		
	case DES_SKILL_ENGINEANGLE:
		wsprintf(buf, STRMSG_C_051202_0001);//  "[ø£¡¯ º±»∏∞¢"
		nValueType = FUNCTION_VALUE_TYPE_PROB100;
		break;		
	case DES_SKILL_ENGINEBOOSTERANGLE:
		wsprintf(buf, STRMSG_C_051202_0002);// "[∫ŒΩ∫≈Õø£¡¯ º±»∏∞¢"
		nValueType = FUNCTION_VALUE_TYPE_PROB100;
		break;
	case DES_WARP:
		wsprintf(str, STRMSG_C_051222_0100);// "[µµΩ√∑Œ ±Õ»Ø]"
		return;
	case DES_SKILL_SLOWMOVING:
		wsprintf(buf, STRMSG_C_051230_0100);// "[¿Ãµøº”µµ ∞®º“"
		nValueType = FUNCTION_VALUE_TYPE_PROB100;
		break;
	case DES_CASH_NORMAL_RESTORE:
		wsprintf(buf, STRMSG_C_060424_0003);// "[∫Œ»∞»Æ∑¸"
		nValueType = FUNCTION_VALUE_TYPE_PROB;	
		break;
	case DES_DROP_EXP:
		wsprintf(buf, STRMSG_C_060424_0007);// "[∞Ê«Ëƒ°"
		nValueType = FUNCTION_VALUE_TYPE_PROB100;	
		break;
	case DES_DROP_SPI:
		wsprintf(buf, STRMSG_C_060424_0008);// "[SPI µÂ∂¯¿≤"
		nValueType = FUNCTION_VALUE_TYPE_PROB100;	
		break;
	case DES_DROP_ITEM:
		wsprintf(buf, STRMSG_C_060424_0009);// "[æ∆¿Ã≈€ µÂ∂¯¿≤"
		nValueType = FUNCTION_VALUE_TYPE_PROB100;	
		break;
	case DES_HP_REPAIR_RATE_FLIGHTING:
		wsprintf(str, STRMSG_C_060424_0010);// "∫Ò«‡Ω√ HP»∏∫π ∞°¥…"
		return;
	case DES_DP_REPAIR_RATE:
		wsprintf(buf, STRMSG_C_060424_0011);// "[∆ÚªÛΩ√¿« ΩØµÂ »∏∫π¿≤"
		nValueType = FUNCTION_VALUE_TYPE_PROB100;	
		break;
	case DES_SP_REPAIR_RATE:
		wsprintf(buf, STRMSG_C_060424_0012);// "[SP »∏∫πº”µµ"
		nValueType = FUNCTION_VALUE_TYPE_PROB100;	
		break;
	case DES_CASH_STAT_ALL_INITIALIZE:
		wsprintf(str, STRMSG_C_060424_0013);// "ƒ≥∏Ø≈Õ¿« ∏µÁ Ω∫≈»¿ª √ ±‚»≠"
		return;
	case DES_CASH_STAT_PART_INITIALIZE:
		wsprintf(str, STRMSG_C_060424_0014);// "ƒ≥∏Ø≈Õ¿« º±≈√«— «—∞°¡ˆ Ω∫≈»¿ª √ ±‚»≠"
		return;
	case DES_CASH_CHANGE_CHARACTERNAME:
		wsprintf(str, STRMSG_C_060424_0015);// "ƒ≥∏Ø≈Õ¿« ¿Ã∏ß¿ª ∫Ø∞Ê«‘"
		return;
	case DES_CASH_STEALTH:
		wsprintf(str, STRMSG_C_060424_0021);// "∏µÁ ∏ÛΩ∫≈Õ¿« ∫Òº±∞¯»≠"
		return;
	case DES_KILLMARK_EXP:
		wsprintf(buf, STRMSG_C_060424_0007);// "[∞Ê«Ëƒ°"
		break;
	case DES_GRADUAL_DP_UP:
		wsprintf(buf,STRMSG_C_PARAM_0106);//"[ΩØµÂ"
		break;
	case DES_SKILL_CHAFF_HP:
		wsprintf(buf,STRMSG_C_061206_0103);//[√º«¡HP
		break;
	case DES_SKILL_HALLUCINATION:
		wsprintf(buf,STRMSG_C_061208_0101);//"[√º«¡∞≥ºˆ"
		break;
	case DES_SKILL_HYPERSHOT:
		wsprintf(buf,STRMSG_C_PARAM_0044);
		break;
	case DES_SKILL_ROLLING_TIME:
		wsprintf(str,STRMSG_C_061206_0104);
		return;
	case DES_SKILL_SHIELD_PARALYZE:
		wsprintf(str,STRMSG_C_061206_0105);
		return;
	case DES_SKILL_BARRIER:
		wsprintf(str,STRMSG_C_061206_0106);
		return;
	case DES_SKILL_BIG_BOOM:
		wsprintf(str,STRMSG_C_061206_0107);
		return;
	case DES_INVISIBLE:
		wsprintf(str,STRMSG_C_061206_0108);
		return;
	case DES_SKILL_CANCELALL:
		wsprintf(str,STRMSG_C_061206_0109);
		return;
	case DES_SKILL_INVINCIBLE:
		wsprintf(str,STRMSG_C_061206_0110);
		return;
	case DES_SKILL_FULL_RECOVERY:
		wsprintf(str,STRMSG_C_061206_0111);
		return;
	case DES_SKILL_SCANNING:
		wsprintf(str,STRMSG_C_061206_0112);
		return;
	case DES_SKILL_NO_WARNING:
		wsprintf(str,STRMSG_C_061206_0113);
		return;
	case DES_SKILL_REVERSECONTROL:
		wsprintf(str, "Target player would have reversed controls while skill is active");
		return;
	case DES_SKILL_CAMOUFLAGE:
		wsprintf(str,STRMSG_C_061206_0114);
		return;
	case DES_WARHEAD_SPEED:
		wsprintf(buf,STRMSG_C_070614_0100);
		nValueType = FUNCTION_VALUE_TYPE_PROB100;	
		break;
		// 2007-09-12 by bhsohn ¿¸¡¯ ±‚¡ˆ ∆˜≈ª æ∆¿Ã≈€ º≥∏Ì √ﬂ∞°
	case DES_WARP_OUTPOST:
		{
			wsprintf(str,STRMSG_C_070912_0201);
			return;
		}
		break;
		// end2007-09-12 by bhsohn ¿¸¡¯ ±‚¡ˆ ∆˜≈ª æ∆¿Ã≈€ º≥∏Ì √ﬂ∞°
		// 2007-09-12 by bhsohn Ω∫««ƒø æ∆¿Ã≈€ º≥∏Ì √ﬂ∞°
	case DES_CHAT_ALL_INFLUENCE:
		{
			wsprintf(str,STRMSG_C_070912_0202);
			return;
		}
		break;
		// end 2007-09-12 by bhsohn Ω∫««ƒø æ∆¿Ã≈€ º≥∏Ì √ﬂ∞°
	// 2007-11-23 by dgwoo ƒ≥∏Ø≈Õ ∫Ø∞Ê ƒ´µÂ 
	case DES_CASH_CHANGE_PILOTFACE:
		{
			wsprintf(str,STRMSG_C_071123_0100);
			return;
		}
		break;
	// 2007-12-17 by bhsohn ¿Œ√æ∆Æ √ ±‚»≠ ƒ´µÂ º≥∏Ì √ﬂ∞°
	case DES_ENCHANT_INITIALIZE:
		{
			wsprintf(str,STRMSG_C_071217_0201);
			return;
		}
		break;
	// end 2007-12-17 by bhsohn ¿Œ√æ∆Æ √ ±‚»≠ ƒ´µÂ º≥∏Ì √ﬂ∞°
		// 2008-06-18 by bhsohn ø©¥‹ø¯¡ı∞° ƒ´µÂ ∞¸∑√ √≥∏Æ
	case DES_CASH_GUILD:
		{
			wsprintf(str,STRMSG_C_080619_0203);	//"ø©¥‹ ∞°¿‘ √÷¥Î ¡¶«— ¿Œø¯ ¡ı∞°"
			return;
		}
		break;
		// end 2008-06-18 by bhsohn ø©¥‹ø¯¡ı∞° ƒ´µÂ ∞¸∑√ √≥∏Æ
	// 2008-09-26 by bhsohn Ω≈±‘ ¿Œ√æ∆Æ √≥∏Æ
	case DES_ENGINE_BOOSTER_TIME_UP:	// ∫ŒΩ∫≈Õ Ω√∞£ ¡ı∞°
		{				
			sprintf(str,STRMSG_C_080929_0200, fValue);				
			return;
		}
		break;
	case DES_ENGINE_MAX_SPEED_UP:		// ø£¡¯ ¿œπ›º”µµ(√÷¥Î) ¡ı∞°
		{
			sprintf(str, STRMSG_C_080929_0201, fValue);
			return;
		}
		break;
	case DES_ENGINE_MIN_SPEED_UP:		// ø£¡¯ ¿œπ›º”µµ(√÷º“) ¡ı∞°
		{
			sprintf(str, STRMSG_C_080929_0202, fValue);
			return;
		}
		break;
	case DES_ENGINE_BOOSTER_SPEED_UP:		// ø£¡¯ ∫ŒΩ∫≈Õº”µµ ¡ı∞°
		{
			sprintf(str, STRMSG_C_080929_0203, fValue);
			return;
		}
		break;
	case DES_ENGINE_GROUND_SPEED_UP:		// ø£¡¯ ¡ˆªÛº”µµ ¡ı∞°
		{
			sprintf(str, STRMSG_C_080929_0204, fValue);
			return;
		}
		break;
	case DES_RADAR_OBJECT_DETECT_RANGE:		// ∑π¿Ã¥ı π∞√º ∞®¡ˆ π›∞Ê
		{
			sprintf(str, STRMSG_C_080929_0205, fValue);
			return;
		}
		break;
	case DES_PIERCE_UP_01:		// ±‚∫ªπ´±‚ ««æÓΩ∫¿≤ ¡ı∞° ƒ´µÂ
		{
			sprintf(buf,STRMSG_C_080929_0206);
			// 2010-07-28 by dgwoo »Æ∑¸ ºˆΩƒ ∫Ø∞Ê (255 => 100%)
			//nValueType = FUNCTION_VALUE_TYPE_PROB255;	
			nValueType = FUNCTION_VALUE_TYPE_PROB;	
		}
		break;
	case DES_PIERCE_UP_02:		// ∞Ì±ﬁπ´±‚ ««æÓΩ∫¿≤ ¡ı∞° ƒ´µÂ
		{
			sprintf(buf,STRMSG_C_080929_0207);
			// 2010-07-28 by dgwoo »Æ∑¸ ºˆΩƒ ∫Ø∞Ê (255 => 100%)
			//nValueType = FUNCTION_VALUE_TYPE_PROB255;	
			nValueType = FUNCTION_VALUE_TYPE_PROB;	
		}
		break;
	// 2010. 12. 05 by jskim, ∑πæÓæ∆¿Ã≈€ µÂ∂¯»Æ∑¸ ¡ı∞° æ∆¿Ã≈€ ±∏«ˆ
	case DES_RARE_ITEM_DROP_RATE:
		{
			wsprintf(buf,STRMSG_C_101206_0401);	 ///	"[∑πæÓ æ∆¿Ã≈€ µÂ∂¯¿≤"
			nValueType = FUNCTION_VALUE_TYPE_PROB100;
		}
	break;

	// end 2010. 12. 05 by jskim, ∑πæÓæ∆¿Ã≈€ µÂ∂¯»Æ∑¸ ¡ı∞° æ∆¿Ã≈€ ±∏«ˆ
	// 2010-12-21 by jskim, ∏∂¿ª ¿Ãµø º”µµ ¡ı∞° æ∆¿Ã≈€ ±∏«ˆ
	case DES_RARE_ITEM_PARTNER_SPEED:
		{
			sprintf(str,STRMSG_C_101221_0401, fValue * 100 );		// "∏∂¿ª ¿Ãµø º”µµ %.2f%% ¡ı∞°"
			return;
		}
		break;
	// end 2010-12-21 by jskim, ∏∂¿ª ¿Ãµø º”µµ ¡ı∞° æ∆¿Ã≈€ ±∏«ˆ
	// 2010-12-21 by jskim, ∆ƒ∆Æ≥  µ•πÃ¡ˆ ¡ı∞° æ∆¿Ã≈€ ±∏«ˆ
	case DES_RARE_ITEM_PARTNER_DAMAGE:
		{
			sprintf(str,STRMSG_C_101221_0402, fValue * 100 );		// "∆ƒ∆Æ≥  µ•πÃ¡ˆ %.2f%% ¡ı∞°"
			return;
		}
		break;
	// end 2010-12-21 by jskim, ∆ƒ∆Æ≥  µ•πÃ¡ˆ ¡ı∞° æ∆¿Ã≈€ ±∏«ˆ
	// 2010-12-21 by jskim, HP, DP ≈∞∆Æ ¿˚øÎ∑Æ ¡ı∞° æ∆¿Ã≈€ ±∏«ˆ
	case DES_RARE_ITEM_HPDP:
		{
			sprintf(str,STRMSG_C_101221_0403, fValue * 100 );		// "ºˆ∏Æ, ΩØµÂ ≈∞∆Æ ¿˚øÎ∑Æ %.2f%% ¡ı∞°"
			return;
		}
		break;
	// end 2010-12-21 by jskim, HP, DP ≈∞∆Æ ¿˚øÎ∑Æ ¡ı∞° æ∆¿Ã≈€ ±∏«ˆ
	case DES_REQ_MIN_LEVEL:		// æ∆¿Ã≈€¿Â¬¯ ø‰±∏ MinLevel¿ª ≥∑√·¥Ÿ
	case DES_REQ_MAX_LEVEL:		// æ∆¿Ã≈€¿Â¬¯ ø‰±∏ MaxLevel¿ª ≥∑√·¥Ÿ
		{			
			sprintf(str, STRMSG_C_080929_0211, fValue);
			return;
		}
		break;
	case DES_ENGINE_ANGLE_UP:	// ø£¡¯ »∏¿¸∞¢ ¡ı∞° ƒ´µÂ
		{			
			sprintf(str, STRMSG_C_080929_0214, (fValue/PI*180.0f));
			return;
		}
		break;
	case DES_ENGINE_BOOSTERANGLE_UP:	// ø£¡¯ ∫ŒΩ∫≈Õ »∏¿¸∞¢ ¡ı∞° ƒ´µÂ
		{
			sprintf(str, STRMSG_C_080929_0215, (fValue/PI*180.0f));
			return;
		}
		break;
		// end 2008-09-26 by bhsohn Ω≈±‘ ¿Œ√æ∆Æ √≥∏Æ
		// 2008-12-30 by bhsohn ¡ˆµµ¿⁄ √§∆√ ¡¶«— ƒ´µÂ ±‚»πæ»
	case DES_CHAT_BLOCK:	// ¡ˆµµ¿⁄ √§∆√ ¡¶«— ƒ´µÂ ±∏«ˆ - 
		{
			wsprintf(str,STRMSG_C_081230_0208, (int)(fValue/60.0f));
			return;
		}
		break;
		// end 2008-12-30 by bhsohn ¡ˆµµ¿⁄ √§∆√ ¡¶«— ƒ´µÂ ±‚»πæ»
		// 2009-01-19 by bhsohn "∏ÛΩ∫≈Õ º“»Øƒ´µÂ" º≥∏Ì √ﬂ∞°
	case DES_CASH_MONSTER_SUMMON:
		{
			wsprintf(str,STRMSG_C_090120_0201);
			return;
		}
		break;
		// end 2009-01-19 by bhsohn "∏ÛΩ∫≈Õ º“»Øƒ´µÂ" º≥∏Ì √ﬂ∞°
		// 2010. 03. 18 by jskim ∏ÛΩ∫≈Õ∫ØΩ≈ ƒ´µÂ
	case DES_TRANSFORM_TO_MONSTER:
		{
			wsprintf(str,STRMSG_C_100401_0401);
			return;	
		}
		break;
	case DES_TRANSFORM_TO_GEAR:
		{
			wsprintf(str,STRMSG_C_100401_0402);
			return;	
		}
		break;	
		//end 2010. 03. 18 by jskim ∏ÛΩ∫≈Õ∫ØΩ≈ ƒ´µÂ
	// 2009. 11. 02 by ckPark ¿Œ««¥œ∆º « µÂ ¿ŒΩ∫≈œΩ∫ ¥¯¡Ø Ω√Ω∫≈€
	case DES_ITEM_RESISTANCE:
		{
			sprintf( str, STRMSG_C_091103_0326, fValue );	// "[ªÛ≈¬ ¿ÃªÛ ¿˙«◊:+%.2f%%]"
			return;
		}
		break;
	case DES_ITEM_ADDATTACK:
		{
			sprintf( str, STRMSG_C_091103_0342, fValue );	// "[±‚∫ªπ´±‚ √ﬂ∞° ≈∏∞›ƒ°:+%.2f]"
			return;
		}
		break;
	case DES_ITEM_ADDATTACK_SEC:
		{
			sprintf( str, STRMSG_C_091103_0343, fValue );	// "[∞Ì±ﬁπ´±‚ √ﬂ∞° ≈∏∞›ƒ°:+%.2f]"
			return;
		}
		break;
	case DES_ITEM_IGNOREDEFENCE:
		{
			sprintf( str, STRMSG_C_091103_0328, fValue );	// "[πÊæÓ∑¬ π´Ω√:+%.2f%%]"
			return;
		}
		break;
	case DES_ITEM_IGNOREAVOID:
		{
			sprintf( str, STRMSG_C_091103_0329, fValue );	// "[»∏««∑¬ π´Ω√:+%.2f%%]"
			return;
		}
		break;
	case DES_ITEM_REDUCEDAMAGE:
		{
			sprintf( str, STRMSG_C_091103_0330, fValue );	// "[µ•πÃ¡ˆ ∫–ªÍ:+%.2f]"
			return;
		}
		break;
	case DES_SKILL_RELEASE:
		{
			sprintf( str, STRMSG_C_091103_0347 );	// "¥ÎªÛ¿« µπˆ«¡ Ω∫≈≥¿ª «ÿ¡¶ «—¥Ÿ."
			return;
		}
		break;
	// end 2009. 11. 02 by ckPark ¿Œ««¥œ∆º « µÂ ¿ŒΩ∫≈œΩ∫ ¥¯¡Ø Ω√Ω∫≈€

	// 2010. 02. 11 by ckPark πﬂµø∑˘ ¿Â¬¯æ∆¿Ã≈€
 	case DES_PAIR_DRAIN_1_RATE:
		strcpy( buf, STRMSG_C_100218_0301 );	// "[µÂ∑π¿Œ »Æ∑¸
		nValueType = FUNCTION_VALUE_TYPE_PROB;
		break;

	case DES_PAIR_DRAIN_2_HP_DP_UP_RATE:
		strcpy( buf, STRMSG_C_100218_0302 );	// "[πﬂµø Ω√ »Ìºˆ∑Æ
		nValueType = FUNCTION_VALUE_TYPE_PROB100;
		break;

	case DES_PAIR_REFLECTION_1_RATE:
		strcpy( buf, STRMSG_C_100218_0303 );	// "[πÃ∑Ø∏µ »Æ∑¸
		nValueType = FUNCTION_VALUE_TYPE_PROB;
		break;

	case DES_PAIR_REFLECTION_2_DAMAGE_RATE:
		strcpy( buf, STRMSG_C_100218_0304 );	// "[µ•πÃ¡ˆ π›ªÁ∑Æ
		nValueType = FUNCTION_VALUE_TYPE_PROB100;
		break;

	case DES_ANTI_DRAIN_RATE:
		strcpy( buf, STRMSG_C_100218_0305 );	// "[µÂ∑π¿Œ ¿˙«◊ »Æ∑¸
		nValueType = FUNCTION_VALUE_TYPE_PROB;
		break;

	case DES_ANTI_REFLECTION_RATE:
		strcpy( buf, STRMSG_C_100218_0306 );	// "[πÃ∑Ø∏µ ¿˙«◊ »Æ∑¸
		nValueType = FUNCTION_VALUE_TYPE_PROB;
		break;
	// end 2010. 02. 11 by ckPark πﬂµø∑˘ ¿Â¬¯æ∆¿Ã≈€

	// 2010. 03. 23 by ckPark ¿Œ««¥œ∆º « µÂ 2¬˜(¿Œ««¥œ∆º « µÂ ¿‘¿Â ƒ≥Ω¨æ∆¿Ã≈€)
	case DES_INFINITY_REENTRY_TICKET:
		{
			sprintf( str, STRMSG_C_100310_0309 );	// "¿Œ««¥œ∆º« µÂ √ﬂ∞° ¿‘¿Â"
			return;
		}
		break;
	// end 2010. 03. 23 by ckPark ¿Œ««¥œ∆º « µÂ 2¬˜(¿Œ««¥œ∆º « µÂ ¿‘¿Â ƒ≥Ω¨æ∆¿Ã≈€)

	// 2010. 03. 03 by ckPark ¿Œ««¥œ∆º « µÂ 2¬˜(¿‘¿Â UI∫Ø∞Ê)
	case DES_SKILL_MON_SILENCE_PRIMARY:
		{
			strcpy( str, STRMSG_C_100310_0312 );	// "[±‚∫ªπ´±‚ ªÁøÎ∫“∞°]"
			return;
		}
		break;
	case DES_SKILL_MON_SILENCE_SECOND:
		{
			strcpy( str, STRMSG_C_100310_0313 );	// "[∞Ì±ﬁπ´±‚ ªÁøÎ∫“∞°]"
			return;
		}
		break;
	case DES_SKILL_MON_FREEZE_HP:
		{
			strcpy( str, STRMSG_C_100310_0315 );	// "[ø°≥ ¡ˆ »∏∫π ∫“∞°]"
			return;
		}
		break;
	case DES_SKILL_MON_FREEZE_DP:
		{
			strcpy( str, STRMSG_C_100310_0316 );	// "[Ω«µÂ »∏∫π ∫“∞°]"
			return;
		}
		break;
	case DES_SKILL_MON_FREEZE_SP:
		{
			strcpy( str, STRMSG_C_100310_0317 );	// "[SP »∏∫π ∫“∞°]"
			return;
		}
		break;
	case DES_SKILL_MON_HOLD:
		{
			strcpy( str, STRMSG_C_100310_0318 );	// "[¿Ãµø ∫“∞°]"
			return;
		}
		break;
	case DES_SKILL_MON_STEALING:
		{
			sprintf( str, STRMSG_C_100310_0314, fValue );	// "[SP »Ìºˆ:-%.2f]"
			return;
		}
		break;
	case DES_SKILL_MON_DRAIN:
		{
			sprintf( str, STRMSG_C_100310_0311, fValue );	// "[HP »Ìºˆ:-%.2f]"
			return;
		}
		break;
	case DES_SKILL_MON_SILENCE_SKILL:
		break;
	// end 2010. 03. 03 by ckPark ¿Œ««¥œ∆º « µÂ 2¬˜(¿‘¿Â UI∫Ø∞Ê)
	// 2010. 05. 18 by jskim ∞›≈ıø’¿« »∆¿Â ≈¯∆¡ ±∏«ˆ
	case DES_PLUS_WARPOINT_RATE:
		{
			sprintf( str, STRMSG_C_100518_0401, fValue + 1);	// "[WarPoint :%.2fπË ¡ı∞°]"
			return;
		}
	break;
	//end 2010. 05. 18 by jskim ∞›≈ıø’¿« »∆¿Â ≈¯∆¡ ±∏«ˆ
	// 2010. 06. 08 by jskim »®«¡∏ÆπÃæˆ UI ¿€æ˜
	case DES_PCROOM_USE_CARD:
		{
			sprintf( str, STRMSG_C_100610_0401);	// "[»® «¡∏ÆπÃæˆ º≠∫ÒΩ∫]"
			return;
		}
	break;
	//end 2010. 06. 08 by jskim »®«¡∏ÆπÃæˆ UI ¿€æ˜
	// 2010-08-26 by shcho&&jskim, WARPOINT ¡ı∞° æ∆¿Ã≈€ ±∏«ˆ
	case DES_WAR_POINT_UP:
		{
			sprintf( str, STRMSG_C_100826_0401, fValue);	// "WARPOINT %d ¡ı∞°"
			return;
		}
	break;
	case DES_CASHPOINT_ADD:
	{
		sprintf(str, "Add % .2f of CashPoints", fValue);	// "WARPOINT %d ¡ı∞°"
		return;
	}
	break;
	// end 2010-08-26 by shcho&&jskim, WARPOINT ¡ı∞° æ∆¿Ã≈€ ±∏«ˆ
	// 2013-02-14 by bhsohn ∆Ø¡§ º”º∫ æ∆¿Ã≈€ ≈¯∆¡ ¿ÃªÛ«œ∞‘ ¬Ô»˜¥¬ «ˆªÛ √≥∏Æ
	case DES_SUMMON_POSITION_X:		// º“»ØΩ√ ¿ßƒ° ∫Ø∞Ê (ªÛ¥Î∞™)
	case DES_SUMMON_POSITION_Y:		// º“»ØΩ√ ¿ßƒ° ∫Ø∞Ê (ªÛ¥Î∞™)
	case DES_SUMMON_POSITION_Z:		// º“»ØΩ√ ¿ßƒ° ∫Ø∞Ê (ªÛ¥Î∞™)
	{
		sprintf(str, "");
		return;
	}
	break;
	case DES_ITEM_BUFF_PARTY:	// 16-09-2020 by Ineptub
	{
		if (bType == DES_DROP_EXP) {
			sprintf(str, "EXP increased in formation by: %.2f", fValue);		
		}
		else if(bType == DES_DROP_SPI)
		{
			sprintf(str, "SPI drop increased in formation by: %.2f", fValue);
		}
		else if (bType == DES_DROP_ITEM)
		{
			sprintf(str, "Item drop increased in formation by: %.2f", fValue);
		}else
		{
			sprintf(str, "Gives bonus to formation");
		}
		return;
	}
	break;

	default:
	{
		if (0 == fValue)
		{
			// ∞™¿Ã æ¯¥Ÿ. ø¿∑˘ ªÛ»≤
			sprintf(str, "");
			DBGOUT("CINFItemInfo::SetParameterInfo FALSE[%d]", bType);
			return;
		}
	}
	break;
	// END2013-03-27 by bhsohn DestParam ¿⁄∑·«¸ ºˆ¡§
	}
	if(fValue > 0)
	{
		switch(nValueType)
		{
		case FUNCTION_VALUE_TYPE_NORMAL:
			{
				// 2010. 02. 11 by ckPark πﬂµø∑˘ ¿Â¬¯æ∆¿Ã≈€
// 				if( m_pRefEnchant != NULL )
// 				{
// 					float fEnchant = GetParamFactor_DesParam( *m_pRefEnchant, bType );
// 					if( fEnchant == 0)
// 					{
// 						sprintf(str,"%s:+%.2f]",buf,fValue);
// 					}
// 					else
// 					{
// 						if( fEnchant > 0)
// 						{
// 							sprintf(str,"%s:+%.2f\\e[+%.2f]\\g]",buf,fValue, fEnchant);
// 						}
// 						else
// 						{
// 							sprintf(str,"%s:+%.2f\\e[%.2f]\\g]",buf,fValue, fEnchant);
// 						}
// 					}
// 				}
// 				else
// 				{
// 					// 2006-12-14 by dgwoo ¿Ø»ø∞¢µµ¿« ∞ÊøÏ¥¬ ∂Ûµæ»∞™¿ª ∞ËªÍ«ÿº≠ √‚∑¬
// 					if(bType == DES_RANGEANGLE_02)
// 					{
// 						sprintf(str,"%s:+%.2f]",buf,fValue/PI*180);
// 					}
// 					else
// 					{
// 						sprintf(str,"%s:+%.2f]",buf,fValue);
// 					}
// 				}

				float fEnchant = 0.0f;
				// ±‚∫ª ¿Œ√¶∆Æµ•Ω∫∆ƒ∂˜¿∫ CParamFactorø°º≠
				if( bDefEnchant )
				{
					if( m_pRefEnchant != NULL )
						fEnchant = GetParamFactor_DesParam( *m_pRefEnchant, bType );
					}
					else
					{
					// πﬂµø∑˘ µ•Ω∫∆ƒ∂˜¿∫ πﬂµø∑˘ ¿Œ√¶∆Æ ¡§∫∏ø°º≠
					if( m_pRefItemInfo )
						fEnchant = m_pRefItemInfo->GetInvokeEnchantParamValue( bType );
				}

				if( fEnchant != 0.0f )
				{
					if( fEnchant > 0.0f )
						sprintf(str,"%s:+%.2f\\e[+%.2f]\\g]",buf,fValue, fEnchant);
					else
						sprintf(str,"%s:+%.2f\\e[%.2f]\\g]",buf,fValue, fEnchant);
				}
				else
				{
					if(bType == DES_RANGEANGLE_02)
						sprintf(str,"%s:+%.2f]",buf,fValue/PI*180);
					else
						sprintf(str,"%s:+%.2f]",buf,fValue);
				}
				// end 2010. 02. 11 by ckPark πﬂµø∑˘ ¿Â¬¯æ∆¿Ã≈€
			}
			break;
		// 2006-04-24 by ispark
		case FUNCTION_VALUE_TYPE_PROB:
			{
				// 2010. 02. 11 by ckPark πﬂµø∑˘ ¿Â¬¯æ∆¿Ã≈€
// 				if( m_pRefEnchant != NULL )
// 				{
// 					float fEnchant = GetParamFactor_DesParam( *m_pRefEnchant, bType );
// 					if( fEnchant == 0)
// 					{
// 						sprintf(str, "%s:+%2.0f%%]",buf,fValue);
// 					}
// 					else
// 					{
// 						if( fEnchant > 0)
// 						{
// 							sprintf(str, "%s:+%2.0f%%\\e[+%2.0f%%]\\g]",buf,fValue, fEnchant);
// 						}
// 						else
// 						{
// 							sprintf(str, "%s:+%2.0f%%\\e[%2.0f%%]\\g]",buf,fValue, fEnchant);
// 						}
// 					}
// 				}
// 				else
// 				{
// 					sprintf(str, "%s:+%2.0f%%]",buf,fValue);
// 				}

				float fEnchant = 0.0f;
				// ±‚∫ª ¿Œ√¶∆Æµ•Ω∫∆ƒ∂˜¿∫ CParamFactorø°º≠
				if( bDefEnchant )
				{
					if( m_pRefEnchant != NULL )
						fEnchant = GetParamFactor_DesParam( *m_pRefEnchant, bType );
				}
				else
				{
					// πﬂµø∑˘ µ•Ω∫∆ƒ∂˜¿∫ πﬂµø∑˘ ¿Œ√¶∆Æ ¡§∫∏ø°º≠
					if( m_pRefItemInfo )
						fEnchant = m_pRefItemInfo->GetInvokeEnchantParamValue( bType );
				}

				if( fEnchant == 0.0f )
				{
					sprintf(str, "%s:+%2.2f%%]",buf,fValue);
				}
				else
				{
					if( fEnchant > 0.0f )
					{
						sprintf(str, "%s:+%2.2f%%\\e[+%2.2f%%]\\g]",buf,fValue, fEnchant);
					}
					else
					{
						sprintf(str, "%s:+%2.2f%%\\e[%2.2f%%]\\g]",buf,fValue, fEnchant);
					}
				}
				// end 2010. 02. 11 by ckPark πﬂµø∑˘ ¿Â¬¯æ∆¿Ã≈€
			}
			break;
		case FUNCTION_VALUE_TYPE_PROB100:
			{
				// 2010. 02. 11 by ckPark πﬂµø∑˘ ¿Â¬¯æ∆¿Ã≈€
// 				if( m_pRefEnchant != NULL )
// 				{
// 					float fEnchant = GetParamFactor_DesParam( *m_pRefEnchant, bType );
// 					if( fEnchant == 0)
// 					{
// 						sprintf(str, "%s:+%2.0f%%]",buf,fValue*100);
// 					}
// 					else
// 					{
// 						if( fEnchant > 0)
// 						{
// 							sprintf(str, "%s:+%2.0f%%\\e[+%2.0f%%]\\g]",buf,fValue*100, fEnchant*100);
// 						}
// 						else
// 						{
// 							sprintf(str, "%s:+%2.0f%%\\e[%2.0f%%]\\g]",buf,fValue*100, fEnchant*100);
// 						}
// 					}
// 				}
// 				else
// 				{
// 					sprintf(str, "%s:+%2.0f%%]",buf,fValue*100);
// 				}

				float fEnchant = 0.0f;
				// ±‚∫ª ¿Œ√¶∆Æµ•Ω∫∆ƒ∂˜¿∫ CParamFactorø°º≠
				if( bDefEnchant )
				{
					if( m_pRefEnchant != NULL )
						fEnchant = GetParamFactor_DesParam( *m_pRefEnchant, bType );
				}
				else
				{
					// πﬂµø∑˘ µ•Ω∫∆ƒ∂˜¿∫ πﬂµø∑˘ ¿Œ√¶∆Æ ¡§∫∏ø°º≠
					if( m_pRefItemInfo )
						fEnchant = m_pRefItemInfo->GetInvokeEnchantParamValue( bType );
				}

				if( fEnchant == 0.0f )
				{
					sprintf(str, "%s:+%2.2f%%]",buf,fValue*100);
				}
				else
				{
					if( fEnchant > 0.0f )
					{
						sprintf(str, "%s:+%2.2f%%\\e[+%2.2f%%]\\g]",buf,fValue*100, fEnchant*100);
					}
					else
					{
						sprintf(str, "%s:+%2.2f%%\\e[%2.2f%%]\\g]",buf,fValue*100, fEnchant*100);
					}
				}
				// end 2010. 02. 11 by ckPark πﬂµø∑˘ ¿Â¬¯æ∆¿Ã≈€
			}
			break;
		case FUNCTION_VALUE_TYPE_PROB255:
			{
				// 2010. 02. 11 by ckPark πﬂµø∑˘ ¿Â¬¯æ∆¿Ã≈€
// 				if( m_pRefEnchant != NULL )
// 				{
// 					float fEnchant = GetParamFactor_DesParam( *m_pRefEnchant, bType );
// 					if( fEnchant == 0)
// 					{
// 						sprintf(str, "%s:+%.2f%%]",buf,((fValue/255)*100));
// 					}
// 					else
// 					{
// 						if( fEnchant > 0)
// 						{
// 							sprintf(str, "%s:+%.2f%%\\e[+%.2f%%]\\g]",buf,(fValue/255)*100, (fEnchant/255)*100);
// 						}
// 						else
// 						{
// 							sprintf(str, "%s:+%.2f%%\\e[%.2f%%]\\g]",buf,(fValue/255)*100, (fEnchant/255)*100);
// 						}
// 					}
// 				}
// 				else
// 				{
// 					sprintf(str, "%s:+%.2f%%]",buf,(fValue/255)*100);
// 				}

				float fEnchant = 0.0f;
				// ±‚∫ª ¿Œ√¶∆Æµ•Ω∫∆ƒ∂˜¿∫ CParamFactorø°º≠
				if( bDefEnchant )
				{
					if( m_pRefEnchant != NULL )
						fEnchant = GetParamFactor_DesParam( *m_pRefEnchant, bType );
				}
				else
				{
					// πﬂµø∑˘ µ•Ω∫∆ƒ∂˜¿∫ πﬂµø∑˘ ¿Œ√¶∆Æ ¡§∫∏ø°º≠
					if( m_pRefItemInfo )
						fEnchant = m_pRefItemInfo->GetInvokeEnchantParamValue( bType );
				}

				if( fEnchant == 0.0f )
				{
					sprintf(str, "%s:+%.2f%%]",buf,(fValue/255)*100);
				}
				else
				{
					if( fEnchant > 0.0f )
					{

						sprintf(str, "%s:+%.2f%%\\e[+%.2f%%]\\g]",buf,(fValue/255)*100, (fEnchant/255)*100);
					}
					else
					{
						sprintf(str, "%s:+%.2f%%\\e[%.2f%%]\\g]",buf,(fValue/255)*100, (fEnchant/255)*100);
					}
				}
				// end 2010. 02. 11 by ckPark πﬂµø∑˘ ¿Â¬¯æ∆¿Ã≈€
			}
			break;	
			// 2008-09-26 by bhsohn Ω≈±‘ ¿Œ√æ∆Æ √≥∏Æ
		case FUNCTION_VALUE_TYPE_PERCENT:
			{
				sprintf(str,"%s:+%.2f%%]",buf,(fValue*100.0f));
			}
			break;
			// end 2008-09-26 by bhsohn Ω≈±‘ ¿Œ√æ∆Æ √≥∏Æ
		}
	}
	else if(fValue < 0)
	{
		switch(nValueType)
		{
		case FUNCTION_VALUE_TYPE_NORMAL:
			{
				// 2010. 02. 11 by ckPark πﬂµø∑˘ ¿Â¬¯æ∆¿Ã≈€
// 				if( m_pRefEnchant != NULL )
// 				{
// 					float fEnchant = GetParamFactor_DesParam( *m_pRefEnchant, bType );
// 					if( fEnchant == 0)
// 					{
// 						sprintf(str,"%s:\\r%.2f\\g]",buf,fValue);
// 					}
// 					else
// 					{
// 						if( fEnchant > 0)
// 						{
// 							sprintf(str,"%s:\\r%.2f\\e[+%.2f]\\g]",buf,fValue, fEnchant);
// 						}
// 						else
// 						{
// 							sprintf(str,"%s:\\r%.2f\\e[%.2f]\\g]",buf,fValue, fEnchant);
// 						}
// 					}
// 				}
// 				else
// 				{
// 					sprintf(str,"%s:\\r%.2f\\g]",buf,fValue);
// 				}

				float fEnchant = 0.0f;
				// ±‚∫ª ¿Œ√¶∆Æµ•Ω∫∆ƒ∂˜¿∫ CParamFactorø°º≠
				if( bDefEnchant )
				{
					if( m_pRefEnchant != NULL )
						fEnchant = GetParamFactor_DesParam( *m_pRefEnchant, bType );
				}
				else
				{
					// πﬂµø∑˘ µ•Ω∫∆ƒ∂˜¿∫ πﬂµø∑˘ ¿Œ√¶∆Æ ¡§∫∏ø°º≠
					if( m_pRefItemInfo )
						fEnchant = m_pRefItemInfo->GetInvokeEnchantParamValue( bType );
				}

				if( fEnchant == 0.0f )
				{
					sprintf(str,"%s:\\r%.2f\\g]",buf,fValue);
				}
				else
				{
					if( fEnchant > 0.0f )
					{
						sprintf(str,"%s:\\r%.2f\\e[+%.2f]\\g]",buf,fValue, fEnchant);
					}
					else
					{
						sprintf(str,"%s:\\r%.2f\\e[%.2f]\\g]",buf,fValue, fEnchant);
					}
				}
				// end 2010. 02. 11 by ckPark πﬂµø∑˘ ¿Â¬¯æ∆¿Ã≈€
			}
			break;
		// 2006-04-24 by ispark
		case FUNCTION_VALUE_TYPE_PROB:
			{
				// 2010. 02. 11 by ckPark πﬂµø∑˘ ¿Â¬¯æ∆¿Ã≈€
// 				if( m_pRefEnchant != NULL )
// 				{
// 					float fEnchant = GetParamFactor_DesParam( *m_pRefEnchant, bType );
// 					if( fEnchant == 0)
// 					{
// 						sprintf(str, "%s:+%2.0f%%]",buf,fValue);
// 					}
// 					else
// 					{
// 						if( fEnchant > 0)
// 						{
// 							sprintf(str, "%s:+%2.0f%%\\e[+%2.0f%%]\\g]",buf,fValue, fEnchant);
// 						}
// 						else
// 						{
// 							sprintf(str, "%s:+%2.0f%%\\e[%2.0f%%]\\g]",buf,fValue, fEnchant);
// 						}
// 					}
// 				}
// 				else
// 				{
// 					sprintf(str, "%s:+%2.0f%%]",buf,fValue);
// 				}

				float fEnchant = 0.0f;
				// ±‚∫ª ¿Œ√¶∆Æµ•Ω∫∆ƒ∂˜¿∫ CParamFactorø°º≠
				if( bDefEnchant )
				{
					if( m_pRefEnchant != NULL )
						fEnchant = GetParamFactor_DesParam( *m_pRefEnchant, bType );
				}
				else
				{
					// πﬂµø∑˘ µ•Ω∫∆ƒ∂˜¿∫ πﬂµø∑˘ ¿Œ√¶∆Æ ¡§∫∏ø°º≠
					if( m_pRefItemInfo )
						fEnchant = m_pRefItemInfo->GetInvokeEnchantParamValue( bType );
				}

				if( fEnchant == 0.0f )
				{
					sprintf(str, "%s:+%2.2f%%]",buf,fValue);
				}
				else
				{
					if( fEnchant > 0.0f )
					{
						sprintf(str, "%s:+%2.2f%%\\e[+%2.2f%%]\\g]",buf,fValue, fEnchant);
					}
					else
					{
						sprintf(str, "%s:+%2.2f%%\\e[%2.2f%%]\\g]",buf,fValue, fEnchant);
					}
				}
				// end 2010. 02. 11 by ckPark πﬂµø∑˘ ¿Â¬¯æ∆¿Ã≈€
			}
			break;
		case FUNCTION_VALUE_TYPE_PROB100:
			{
				// 2010. 02. 11 by ckPark πﬂµø∑˘ ¿Â¬¯æ∆¿Ã≈€
// 				if( m_pRefEnchant != NULL )
// 				{
// 					float fEnchant = GetParamFactor_DesParam( *m_pRefEnchant, bType );
// 					if( fEnchant == 0)
// 					{
// 						sprintf(str, "%s:\\r%2.0f%%\\g]",buf,fValue*100);
// 					}
// 					else
// 					{
// 						if( fEnchant > 0)
// 						{
// 							sprintf(str, "%s:\\r%2.0f%%\\e[+%2.0f%%]\\g]",buf,fValue*100, fEnchant*100);
// 						}
// 						else
// 						{
// 							sprintf(str, "%s:\\r%2.0f%%\\e[%2.0f%%]\\g]",buf,fValue*100, fEnchant*100);
// 						}
// 					}
// 				}
// 				else
// 				{
// 					sprintf(str, "%s:\\r%2.0f%%\\g]",buf,fValue*100);
// 				}

				float fEnchant = 0.0f;
				// ±‚∫ª ¿Œ√¶∆Æµ•Ω∫∆ƒ∂˜¿∫ CParamFactorø°º≠
				if( bDefEnchant )
				{
					if( m_pRefEnchant != NULL )
						fEnchant = GetParamFactor_DesParam( *m_pRefEnchant, bType );
				}
				else
				{
					// πﬂµø∑˘ µ•Ω∫∆ƒ∂˜¿∫ πﬂµø∑˘ ¿Œ√¶∆Æ ¡§∫∏ø°º≠
					if( m_pRefItemInfo )
						fEnchant = m_pRefItemInfo->GetInvokeEnchantParamValue( bType );
				}

				if( fEnchant == 0.0f )
				{
					sprintf(str, "%s:\\r%2.2f%%\\g]",buf,fValue*100);
				}
				else
				{
					if( fEnchant > 0.0f )
					{
						sprintf(str, "%s:\\r%2.2f%%\\e[+%2.2f%%]\\g]",buf,fValue*100, fEnchant*100);
					}
					else
					{
						sprintf(str, "%s:\\r%2.2f%%\\e[%2.2f%%]\\g]",buf,fValue*100, fEnchant*100);
					}
				}
				// end 2010. 02. 11 by ckPark πﬂµø∑˘ ¿Â¬¯æ∆¿Ã≈€
			}
			break;
		case FUNCTION_VALUE_TYPE_PROB255:
			{
				// 2010. 02. 11 by ckPark πﬂµø∑˘ ¿Â¬¯æ∆¿Ã≈€
// 				if( m_pRefEnchant != NULL )
// 				{
// 					float fEnchant = GetParamFactor_DesParam( *m_pRefEnchant, bType );
// 					if( fEnchant == 0)
// 					{
// 						sprintf(str, "%s:\\r%.2f%%\\g]",buf,((fValue/255)*100));
// 					}
// 					else
// 					{
// 						if( fEnchant > 0)
// 						{
// 							sprintf(str, "%s:\\r%.2f%%\\e[+%.2f%%]\\g]",buf,(fValue/255)*100, (fEnchant/255)*100);
// 						}
// 						else
// 						{
// 							sprintf(str, "%s:\\r%.2f%%\\e[%.2f%%]\\g]",buf,(fValue/255)*100, (fEnchant/255)*100);
// 						}
// 					}
// 				}
// 				else
// 				{
// 					sprintf(str, "%s:\\r%.2f%%\\g]",buf,((fValue/255)*100));
// 				}

				float fEnchant = 0.0f;
				// ±‚∫ª ¿Œ√¶∆Æµ•Ω∫∆ƒ∂˜¿∫ CParamFactorø°º≠
				if( bDefEnchant )
				{
					if( m_pRefEnchant != NULL )
						fEnchant = GetParamFactor_DesParam( *m_pRefEnchant, bType );
				}
				else
				{
					// πﬂµø∑˘ µ•Ω∫∆ƒ∂˜¿∫ πﬂµø∑˘ ¿Œ√¶∆Æ ¡§∫∏ø°º≠
					if( m_pRefItemInfo )
						fEnchant = m_pRefItemInfo->GetInvokeEnchantParamValue( bType );
				}

				if( fEnchant == 0.0f )
				{
					sprintf(str, "%s:\\r%.2f%%\\g]",buf,((fValue/255)*100));
				}
				else
				{
					if( fEnchant > 0.0f )
					{
						sprintf(str, "%s:\\r%.2f%%\\e[+%.2f%%]\\g]",buf,(fValue/255)*100, (fEnchant/255)*100);
					}
					else
					{
						sprintf(str, "%s:\\r%.2f%%\\e[%.2f%%]\\g]",buf,(fValue/255)*100, (fEnchant/255)*100);
					}
				}
				// end 2010. 02. 11 by ckPark πﬂµø∑˘ ¿Â¬¯æ∆¿Ã≈€
			}
			break;
		}
//		if(bPercent)
//		{
//			sprintf(str, "%s:\\r%d%%\\g]",buf,(int)(fValue*100));
//		}
//		else
//		{
//			sprintf(str,"%s:\\r%.2f\\g]",buf,fValue);
//		}
	}
	
	// 2010. 02. 11 by ckPark πﬂµø∑˘ ¿Â¬¯æ∆¿Ã≈€

// 	// 2008-09-26 by bhsohn Ω≈±‘ ¿Œ√æ∆Æ √≥∏Æ
// 	else if(0 ==fValue 
// 		&& ((DES_DEFENSE_01 == bType)
// 		    ||(DES_DEFENSE_02 == bType)
// 			||(DES_DEFENSEPROBABILITY_01 == bType)
// 			||(DES_DEFENSEPROBABILITY_02 == bType)))
// 	{
// 		if( m_pRefEnchant != NULL )
// 		{
// 			if( m_pRefEnchant != NULL )
// 			{
// 				float fEnchant = GetParamFactor_DesParam( *m_pRefEnchant, bType );
// 				if( fEnchant == 0)
// 				{
// 					sprintf(str, "%s:+%.2f%%]",buf,((fValue/255)*100));
// 				}
// 				else
// 				{
// 					if( fEnchant > 0)
// 					{
// 						sprintf(str, "%s:+%.2f%%\\e[+%.2f%%]\\g]",buf,(fValue/255)*100, (fEnchant/255)*100);
// 					}
// 					else
// 					{
// 						sprintf(str, "%s:+%.2f%%\\e[%.2f%%]\\g]",buf,(fValue/255)*100, (fEnchant/255)*100);
// 					}
// 				}
// 			}
// 			else
// 			{
// 				sprintf(str, "%s:+%.2f%%]",buf,(fValue/255)*100);
// 			}
// 		}
// 		else
// 		{
// 			wsprintf(str, "%s]", buf);
// 		}
// 	}
// 	// end 2008-09-26 by bhsohn Ω≈±‘ ¿Œ√æ∆Æ √≥∏Æ
// 	else
// 	{
// 		wsprintf(str, "%s]", buf);
// 	}


	else if( fValue == 0.0f )		// ¿Œ√¶∆Æ∑Œ ¿Œ«ÿ ªı∑Œ ∫Œø©µ» ±‚¥…
	{
		switch(nValueType)
		{
		case FUNCTION_VALUE_TYPE_NORMAL:
			{
				float fEnchant = 0.0f;
				// ±‚∫ª ¿Œ√¶∆Æµ•Ω∫∆ƒ∂˜¿∫ CParamFactorø°º≠
				if( bDefEnchant )
				{
					if( m_pRefEnchant != NULL )
						fEnchant = GetParamFactor_DesParam( *m_pRefEnchant, bType );
				}
				else
				{
					// πﬂµø∑˘ µ•Ω∫∆ƒ∂˜¿∫ πﬂµø∑˘ ¿Œ√¶∆Æ ¡§∫∏ø°º≠
					if( m_pRefItemInfo )
						fEnchant = m_pRefItemInfo->GetInvokeEnchantParamValue( bType );
				}

				if( fEnchant != 0.0f )
				{
					if( fEnchant > 0.0f )
						sprintf(str,"%s:+%.2f\\e[+%.2f]\\g]",buf,fValue, fEnchant);
					else
						sprintf(str,"%s:+%.2f\\e[%.2f]\\g]",buf,fValue, fEnchant);
				}
				else
				{
					if(bType == DES_RANGEANGLE_02)
						sprintf(str,"%s:+%.2f]",buf,fValue/PI*180);
					else
						sprintf(str,"%s:+%.2f]",buf,fValue);
				}
			}
			break;
		case FUNCTION_VALUE_TYPE_PROB:
			{
				float fEnchant = 0.0f;
				// ±‚∫ª ¿Œ√¶∆Æµ•Ω∫∆ƒ∂˜¿∫ CParamFactorø°º≠
				if( bDefEnchant )
				{
					if( m_pRefEnchant != NULL )
						fEnchant = GetParamFactor_DesParam( *m_pRefEnchant, bType );
				}
				else
				{
					// πﬂµø∑˘ µ•Ω∫∆ƒ∂˜¿∫ πﬂµø∑˘ ¿Œ√¶∆Æ ¡§∫∏ø°º≠
					if( m_pRefItemInfo )
						fEnchant = m_pRefItemInfo->GetInvokeEnchantParamValue( bType );
				}

				if( fEnchant == 0.0f )
				{
					sprintf(str, "%s:+%2.2f%%]",buf,fValue);
				}
				else
				{
					if( fEnchant > 0.0f )
					{
						sprintf(str, "%s:+%2.2f%%\\e[+%2.2f%%]\\g]",buf,fValue, fEnchant);
					}
					else
					{
						sprintf(str, "%s:+%2.2f%%\\e[%2.2f%%]\\g]",buf,fValue, fEnchant);
					}
				}
			}
			break;
		case FUNCTION_VALUE_TYPE_PROB100:
			{
				float fEnchant = 0.0f;
				// ±‚∫ª ¿Œ√¶∆Æµ•Ω∫∆ƒ∂˜¿∫ CParamFactorø°º≠
				if( bDefEnchant )
				{
					if( m_pRefEnchant != NULL )
						fEnchant = GetParamFactor_DesParam( *m_pRefEnchant, bType );
				}
				else
				{
					// πﬂµø∑˘ µ•Ω∫∆ƒ∂˜¿∫ πﬂµø∑˘ ¿Œ√¶∆Æ ¡§∫∏ø°º≠
					if( m_pRefItemInfo )
						fEnchant = m_pRefItemInfo->GetInvokeEnchantParamValue( bType );
				}

				if( fEnchant == 0.0f )
				{
					sprintf(str, "%s:+%2.2f%%]",buf,fValue*100);
				}
				else
				{
					if( fEnchant > 0.0f )
					{
						sprintf(str, "%s:+%2.2f%%\\e[+%2.2f%%]\\g]",buf,fValue*100, fEnchant*100);
					}
					else
					{
						sprintf(str, "%s:+%2.2f%%\\e[%2.2f%%]\\g]",buf,fValue*100, fEnchant*100);
					}
				}
			}
			break;
		case FUNCTION_VALUE_TYPE_PROB255:
			{
				float fEnchant = 0.0f;
				// ±‚∫ª ¿Œ√¶∆Æµ•Ω∫∆ƒ∂˜¿∫ CParamFactorø°º≠
				if( bDefEnchant )
				{
					if( m_pRefEnchant != NULL )
						fEnchant = GetParamFactor_DesParam( *m_pRefEnchant, bType );
				}
				else
				{
					// πﬂµø∑˘ µ•Ω∫∆ƒ∂˜¿∫ πﬂµø∑˘ ¿Œ√¶∆Æ ¡§∫∏ø°º≠
					if( m_pRefItemInfo )
						fEnchant = m_pRefItemInfo->GetInvokeEnchantParamValue( bType );
				}

				if( fEnchant == 0.0f )
				{
					sprintf(str, "%s:+%.2f%%]",buf,((fValue/255)*100));
				}
				else
				{
					if( fEnchant > 0.0f )
					{
						sprintf(str, "%s:+%.2f%%\\e[+%.2f%%]\\g]",buf,(fValue/255)*100, (fEnchant/255)*100);
					}
					else
					{
						sprintf(str, "%s:+%.2f%%\\e[%.2f%%]\\g]",buf,(fValue/255)*100, (fEnchant/255)*100);
					}
				}
			}
			break;	
			// 2008-09-26 by bhsohn Ω≈±‘ ¿Œ√æ∆Æ √≥∏Æ
		case FUNCTION_VALUE_TYPE_PERCENT:
			{
				sprintf(str,"%s:+%.2f%%]",buf,(fValue*100.0f));
			}
			break;
		}
	}
	// end 2010. 02. 11 by ckPark πﬂµø∑˘ ¿Â¬¯æ∆¿Ã≈€
}
#endif

///////////////////////////////////////////////////////////////////////////////
/// \fn			CINFItemInfo::IsEnableItem(ITEM* pITEM)
/// \brief		
/// \author		dhkwon
/// \date		2004-06-12 ~ 2004-06-12
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CINFItemInfo::IsEnableItem(ITEM* pITEM)
{
	if(pITEM == NULL)
		return FALSE;

	if( pITEM->ReqGearStat.AttackPart <= g_pShuttleChild->m_myShuttleInfo.TotalGearStat.AttackPart &&
		pITEM->ReqGearStat.ShieldPart <= g_pShuttleChild->m_myShuttleInfo.TotalGearStat.ShieldPart &&
		pITEM->ReqGearStat.DodgePart <= g_pShuttleChild->m_myShuttleInfo.TotalGearStat.DodgePart &&
		pITEM->ReqGearStat.DefensePart <= g_pShuttleChild->m_myShuttleInfo.TotalGearStat.DefensePart &&
		pITEM->ReqGearStat.FuelPart <= g_pShuttleChild->m_myShuttleInfo.TotalGearStat.FuelPart &&
		pITEM->ReqMinLevel <= g_pShuttleChild->m_myShuttleInfo.Level &&
		pITEM->ReqMaterial <= g_pShuttleChild->m_myShuttleInfo.Material &&
		CompareBitFlag( pITEM->ReqRace , g_pShuttleChild->m_myShuttleInfo.Race ) &&
		pITEM->ReqGearStat.SoulPart <= g_pShuttleChild->m_myShuttleInfo.TotalGearStat.SoulPart &&
		pITEM->ReqSP <= g_pShuttleChild->m_myShuttleInfo.SP &&
		CompareBitFlag( pITEM->ReqUnitKind, g_pShuttleChild->m_myShuttleInfo.UnitKind ) )
	{
		if(	pITEM->ReqMaxLevel == 0 ||
			pITEM->ReqMaxLevel >= g_pShuttleChild->m_myShuttleInfo.Level)
		{
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
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			CINFItemInfo::LoadNPCImage(int nNPCIndex)
/// \brief		
/// \author		ydkim
/// \date		2004-08-13~
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
/*CINFImage* CINFItemInfo::FindBigIcon(int nItemNum)
{
	SAFE_DELETE(m_pDataHeader);
	
	CGameData gameData;
	gameData.SetFile( ".\\Res-Tex\\bigitem.tex", FALSE, NULL, 0, FALSE );
	char szName[32];
	wsprintf(szName, "%08d", nItemNum);
	m_pDataHeader = gameData.FindFromFile(szName);
	if(m_pDataHeader == NULL)
	{
		return NULL;
	}
	CINFImage *pImage = new CINFImage;
	pImage->InitDeviceObjects( m_pDataHeader->m_pData, m_pDataHeader->m_DataSize );
	pImage->RestoreDeviceObjects();
	//g_pInterface->vecStoredBigIcon.emplace_back(nItemNum, m_pDataHeader); //cache
	return pImage;
}*/

///////////////////////////////////////////////////////////////////////////////
/// \fn			CINFItemInfo::RenderInfoWindows(int x, int y, int cx, int cy)
/// \brief		
/// \author		ydkim
/// \date		2004-09-01~
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CINFItemInfo::RenderInfoWindows(int x, int y, int cx, int cy)
{
	m_pInfoBoxSide[0]->Move(x,y);
	m_pInfoBoxSide[0]->Render();
	m_pInfoBoxSide[1]->Move(x+ITEMINFO_SIDE_TOPSIZE_WIDTH,y);
	m_pInfoBoxSide[1]->SetScale(cx-(2*ITEMINFO_SIDE_TOPSIZE_WIDTH), 1);
	m_pInfoBoxSide[1]->Render();
	m_pInfoBoxSide[2]->Move(x+cx - ITEMINFO_SIDE_TOPSIZE_WIDTH,y);
	m_pInfoBoxSide[2]->Render();

	m_pInfoBoxSide[3]->Move(x, y+ITEMINFO_SIDE_TOPSIZE_HEIGHT);
	m_pInfoBoxSide[3]->SetScale(1, cy - (ITEMINFO_SIDE_TOPSIZE_HEIGHT + ITEMINFO_SIDE_BOTSIZE_HEIGHT));
	m_pInfoBoxSide[3]->Render();
	m_pInfoBoxSide[4]->Move(x+ITEMINFO_SIDE_TOPSIZE_WIDTH, y+ITEMINFO_SIDE_TOPSIZE_HEIGHT);
	m_pInfoBoxSide[4]->SetScale(cx-(2*ITEMINFO_SIDE_TOPSIZE_WIDTH), cy - (ITEMINFO_SIDE_TOPSIZE_HEIGHT + ITEMINFO_SIDE_BOTSIZE_HEIGHT));
	m_pInfoBoxSide[4]->Render();
	m_pInfoBoxSide[5]->Move(x+cx - ITEMINFO_SIDE_TOPSIZE_WIDTH, y+ITEMINFO_SIDE_TOPSIZE_HEIGHT);
	m_pInfoBoxSide[5]->SetScale(1, cy - (ITEMINFO_SIDE_TOPSIZE_HEIGHT + ITEMINFO_SIDE_BOTSIZE_HEIGHT));
	m_pInfoBoxSide[5]->Render();

	m_pInfoBoxSide[6]->Move(x, y+cy-ITEMINFO_SIDE_BOTSIZE_HEIGHT);
	m_pInfoBoxSide[6]->Render();
	m_pInfoBoxSide[7]->Move(x + ITEMINFO_SIDE_BOTSIZE_WIDTH, y+cy-ITEMINFO_SIDE_BOTSIZE_HEIGHT);
	m_pInfoBoxSide[7]->SetScale(cx-(2*ITEMINFO_SIDE_BOTSIZE_WIDTH), 1);
	m_pInfoBoxSide[7]->Render();
	m_pInfoBoxSide[8]->Move(x+cx-ITEMINFO_SIDE_BOTSIZE_WIDTH, y+cy-ITEMINFO_SIDE_BOTSIZE_HEIGHT);
	m_pInfoBoxSide[8]->Render();

}

DataHeader * CINFItemInfo::FindResource(char* szRcName)
{
	FLOG( "CINFSelect::FindResource(char* szRcName)" );
	DataHeader* pHeader = NULL;
	if(m_pGameData)
	{
		pHeader = m_pGameData->Find(szRcName);
	}
	return pHeader;
}

void CINFItemInfo::SetGameData(CGameData * pData)
{
	FLOG( "CINFBase::SetGameData(CGameData * pData)" );
	m_pGameData = pData ;
}



///////////////////////////////////////////////////////////////////////////////
/// \fn			CINFItemInfo::SetRadarInfo()
/// \brief		∑π¿Ã¥ı ¡§∫∏¿‘∑¬
/// \author		ispark
/// \date		2005-08-17 ~ 2005-08-17
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CINFItemInfo::SetRadarItemInfo(BOOL bShop)
{
	int index = 0;
	SetItemName( index++ );

	// 2009. 10. 28 by ckPark Ω√∞£/±‚∞£ ¡¶«— æ∆¿Ã≈€ ≈¯∆¡ ºˆ¡§
	SetRemainTime_Imp( m_pRefItemInfo, &index, bShop );
	// end 2009. 10. 28 by ckPark Ω√∞£/±‚∞£ ¡¶«— æ∆¿Ã≈€ ≈¯∆¡ ºˆ¡§
	
	SetUnitKind(index++);				// 2005-12-26 by ispark, SetCount() -> SetUnitKind()∑Œ ∫Ø∞Ê
	SetReqLevel(index++);				// 2007-06-14 by dgwoo ∑π¿Ã¥Ÿµµ ø‰±∏ ∑π∫ß¿Ã µÈæÓ∞®.
	SetWeight( index++ );
	
	// 2009. 08. 27 by ckPark ±◊∑°«» ∏Æº“Ω∫ ∫Ø∞Ê Ω√Ω∫≈€ ±∏«ˆ
	if( m_pRefItemInfo )
		SetShapeInfo( index++ );
	// end 2009. 08. 27 by ckPark ±◊∑°«» ∏Æº“Ω∫ ∫Ø∞Ê Ω√Ω∫≈€ ±∏«ˆ
	
	SetPrimaryRange(index++);
	SetSecondaryRange(index++);
	SetRadarRange(index++);
	if(bShop == FALSE)
	{
		SetShopSellInfo( index++ );
		// 2007-09-07 by bhsohn æ∆¿Ã≈€ ∞≈∑°ø©∫Œ «•Ω√
		SetItemAttribute(index++);
	}

	// 2009. 11. 02 by ckPark ¿Œ««¥œ∆º « µÂ ¿ŒΩ∫≈œΩ∫ ¥¯¡Ø Ω√Ω∫≈€
	SetUseAreaInfinity( &index );
	SetExchangeMaterial( bShop );
	// end 2009. 11. 02 by ckPark ¿Œ««¥œ∆º « µÂ ¿ŒΩ∫≈œΩ∫ ¥¯¡Ø Ω√Ω∫≈€

	SetDesc( index );
	SetItemExtendInfo(TRUE);
	ASSERT_ASSERT(index <= ITEMINFO_PARAMETER_NUMBER);
	SetMaxLength();	
}


///////////////////////////////////////////////////////////////////////////////
/// \fn			CINFItemInfo::SetSkillReAttackTime(int nParameterIndex)
/// \brief		¿Áπﬂµø Ω√∞£ 
/// \author		ispark
/// \date		2005-11-22 ~ 2005-11-22
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CINFItemInfo::SetSkillReAttackTime(int nParameterIndex)
{
// 2005-11-22 by ispark ¿Áπﬂµø Ω√∞£ : √ﬂ∞°
	sprintf( m_strItemInfo[nParameterIndex], STRMSG_C_051122_0007, (float)m_pRefITEM->ReAttacktime/1000.0f) ;//"¿Áπﬂµø Ω√∞£ : %.2fsec"
//	if(fEnchantTime > 0)
//		sprintf( m_strItemInfo[nParameterIndex], "%s\\e[+%2.0f%%]\\e",m_strItemInfo[nParameterIndex], fEnchantTime*100) ;
//	else if(fEnchantTime < 0)
//		sprintf( m_strItemInfo[nParameterIndex], "%s\\e[%2.0f%%]\\e",m_strItemInfo[nParameterIndex], fEnchantTime*100) ;
//	if(fRareInfoTime > 0)
//		sprintf( m_strItemInfo[nParameterIndex], "%s\\g[+%2.0f%%]\\g",m_strItemInfo[nParameterIndex], fRareInfoTime*100) ;
//	else if(fRareInfoTime < 0)
//		sprintf( m_strItemInfo[nParameterIndex], "%s\\g[%2.0f%%]\\g",m_strItemInfo[nParameterIndex], fRareInfoTime*100) ;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			CINFItemInfo::SecondaryWeaponSpeed(int nParameterIndex)
/// \brief		π´±‚ º”µµ
/// \author		ispark
/// \date		2005-12-15 ~ 2005-12-15
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
#ifdef BONUS_STAT_ITEM
void CINFItemInfo::SetWeaponSpeed(int nParameterIndex, BOOL Bonus)
{
	FLOAT fEnchantSpeed = m_pRefEnchant ? (m_pRefEnchant->pfm_WARHEAD_SPEED * 100) : 0;
	sprintf(m_strItemInfo[nParameterIndex], STRMSG_C_051215_0100, m_pRefITEM->RepeatTime);//"º”µµ : %dm/s"
	if (fEnchantSpeed > 0)
	{
		sprintf(m_strItemInfo[nParameterIndex], "%s\\e[+%2.0f%%]\\e", m_strItemInfo[nParameterIndex], fEnchantSpeed);
	}
	/*FLOAT BonusSpeed = m_pRefBonus ? (m_pRefBonus->pfm_WARHEAD_SPEED * 100) : 0;
	FLOAT fEnchantSpeed = m_pRefEnchant ? (m_pRefEnchant->pfm_WARHEAD_SPEED * 100) : 0;
	if (Bonus)
#ifdef BONUS_STAT_INVERTED
		sprintf(m_strItemInfo[nParameterIndex], STRMSG_C_051215_0100, m_pRefITEM->RepeatTime);
#else
		sprintf(m_strItemInfo[nParameterIndex], STRMSG_C_051215_0100, (USHORT)(m_pRefITEM->RepeatTime + (m_pRefITEM->RepeatTime * 0.02)));
#endif
	else
	{
#ifdef BONUS_STAT_INVERTED
		int n = static_cast<int>(BonusSpeed);
		sprintf(m_strItemInfo[nParameterIndex], STRMSG_C_051215_0100, m_pRefITEM->RepeatTime - (USHORT)BonusSpeed*10.0f);
#else
		sprintf(m_strItemInfo[nParameterIndex], STRMSG_C_051215_0100, m_pRefITEM->RepeatTime + (USHORT)BonusSpeed);
#endif
	}
	if (fEnchantSpeed > 0)
	{
		sprintf(m_strItemInfo[nParameterIndex], "%s\\e[+%2.0f%%]\\e", m_strItemInfo[nParameterIndex], fEnchantSpeed);
	}*/
}

#else
void CINFItemInfo::SetWeaponSpeed(int nParameterIndex)
{
	FLOAT fEnchantSpeed = m_pRefEnchant ? (m_pRefEnchant->pfm_WARHEAD_SPEED * 100): 0;
	sprintf( m_strItemInfo[nParameterIndex], STRMSG_C_051215_0100, m_pRefITEM->RepeatTime);//"º”µµ : %dm/s"
	if(fEnchantSpeed > 0)
	{
		sprintf( m_strItemInfo[nParameterIndex], "%s\\e[+%2.0f%%]\\e", m_strItemInfo[nParameterIndex],fEnchantSpeed);
	}
}
#endif
///////////////////////////////////////////////////////////////////////////////
/// \fn			CINFItemInfo::SetWeaponAngle(int nParameterIndex)
/// \brief		π´±‚ √÷¥Î º±»∏∞¢
/// \author		ispark
/// \date		2005-12-15 ~ 2005-12-15
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CINFItemInfo::SetWeaponAngle(int nParameterIndex)
{
	sprintf( m_strItemInfo[nParameterIndex], STRMSG_C_051215_0101, m_pRefITEM->BoosterAngle);//"º±»∏∞¢ : %.0fµµ"
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			CINFItemInfo::SetAccessoryTimeLimitItemInfo(BOOL bShop)
/// \brief		æ«ººªÁ∏Æ
/// \author		ispark
/// \date		2006-03-30 ~ 2006-03-30
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CINFItemInfo::SetAccessoryTimeLimitItemInfo(BOOL bShop)
{
	int index = 0;
	SetItemName( index++ );
	SetUnitKind( index++ );

	// 2009. 10. 28 by ckPark Ω√∞£/±‚∞£ ¡¶«— æ∆¿Ã≈€ ≈¯∆¡ ºˆ¡§
	//SetItemAllTime(index++);
	//SetItemRemainTime(index++,bShop);

	if( !SetRemainTime_Imp( m_pRefItemInfo, &index, bShop ) )
	{
		if( m_pRefItemInfo )
			SetItemAllTime(index++);
	}
	// end 2009. 10. 28 by ckPark Ω√∞£/±‚∞£ ¡¶«— æ∆¿Ã≈€ ≈¯∆¡ ºˆ¡§
	
	SetParameter(&index);
	
	SetWeight( index++ );
	// 2007-09-07 by bhsohn æ∆¿Ã≈€ ∞≈∑°ø©∫Œ «•Ω√
	if(FALSE == bShop)
	{
		SetItemAttribute(index++);
	}

	// 2009. 11. 02 by ckPark ¿Œ««¥œ∆º « µÂ ¿ŒΩ∫≈œΩ∫ ¥¯¡Ø Ω√Ω∫≈€
	SetUseAreaInfinity( &index );
	SetExchangeMaterial( bShop );
	// end 2009. 11. 02 by ckPark ¿Œ««¥œ∆º « µÂ ¿ŒΩ∫≈œΩ∫ ¥¯¡Ø Ω√Ω∫≈€

	SetDesc( index );
	SetItemExtendInfo(TRUE);
	ASSERT_ASSERT(index <= ITEMINFO_PARAMETER_NUMBER);
	SetMaxLength();
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			CINFItemInfo::SetItemRemainTime(int nParameterIndex, BOOL bLinkItem)
/// \brief		æ∆¿Ã≈€ ªÁøÎ ≥≤¿∫ Ω√∞£
/// \author		ispark
/// \date		2006-03-31 ~ 2006-03-31
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CINFItemInfo::SetItemRemainTime(int nParameterIndex,BOOL bShop, BOOL bLinkItem, BOOL bSetTick)
{
	int nTime = 0;
	int nItemTime = m_pRefITEM->Time;
	if(bLinkItem == TRUE && m_pRefITEM->LinkItem)
	{
		ITEM* pItemInfo = g_pDatabase->GetServerItemInfo(m_pRefITEM->LinkItem);
		nItemTime = pItemInfo->Time;
	}
	// 2010. 05. 18 by jskim ∞›≈ıø’¿« »∆¿Â ≈¯∆¡ ±∏«ˆ
	if(nItemTime >= MAX_INT_VALUE)
	{
		return;
	}
	//end 2010. 05. 18 by jskim ∞›≈ıø’¿« »∆¿Â ≈¯∆¡ ±∏«ˆ
	// 2008-12-02 by bhsohn Ω≈±‘ æ∆¿Ã≈€ º”º∫ √ﬂ∞°
	if((nItemTime > 0.0f)
		&& m_pRefItemInfo 
		&& m_pRefItemInfo->GetRealItemInfo()
		&& COMPARE_BIT_FLAG(m_pRefItemInfo->GetRealItemInfo()->ItemAttribute, ITEM_ATTR_DELETED_TIME_LIMITE_AFTER_USED))
		{
		char strRemain[1024] = {0,};
		int nSecTime = (int)(nItemTime);
		if(!bShop && m_pRefItemInfo)
		{
			nSecTime = (int)((nItemTime ) - m_pRefItemInfo->GetItemPassTime());
		}

		wsprintf( m_strItemInfo[nParameterIndex], STRMSG_C_061012_0002, GetStringDateTimeFormSecond(strRemain, nSecTime));

		if(bSetTick)
		{
			stTickFuntionIndex stAddTemp;
			stAddTemp.nFuntionIndex = FUNCTION_INDEX_ITEM_REMAIN_TIME;
			stAddTemp.nDataLineIndex = nParameterIndex;
			stAddTemp.nParam1 = bLinkItem;
			m_vecTickFuntionIndex.push_back(stAddTemp);
		}
	}
    // end 2008-12-02 by bhsohn Ω≈±‘ æ∆¿Ã≈€ º”º∫ √ﬂ∞°
	else if(nItemTime > 0.0f)
    
	{
		char strRemain[1024] = {0,};
		int nSecTime = (int)(nItemTime / 1000);
		if(!bShop && m_pRefItemInfo)
		{
			nSecTime = (int)((nItemTime / 1000) - m_pRefItemInfo->GetItemPassTime());
		}

		wsprintf( m_strItemInfo[nParameterIndex], STRMSG_C_061012_0002, GetStringDateTimeFormSecond(strRemain, nSecTime));

		if(bSetTick)
		{
			stTickFuntionIndex stAddTemp;
			stAddTemp.nFuntionIndex = FUNCTION_INDEX_ITEM_REMAIN_TIME;
			stAddTemp.nDataLineIndex = nParameterIndex;
			stAddTemp.nParam1 = bLinkItem;
			m_vecTickFuntionIndex.push_back(stAddTemp);
		}
	}
	else
	{
		wsprintf( m_strItemInfo[nParameterIndex], STRMSG_C_060424_0006); // "≥≤¿∫Ω√∞£ : π´¡¶«—"
	}
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			CINFItemInfo::SetItemAllTime(int nParameterIndex, BOOL bCard, BOOL bLinkItem)
/// \brief		Ω√∞£ ¡¶«— æ∆¿Ã≈€ √— Ω√∞£
/// \author		ispark
/// \date		2006-03-31 ~ 2006-03-31
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CINFItemInfo::SetItemAllTime(int nParameterIndex, BOOL bCard, BOOL bLinkItem)
{
	int nTime = 0;
	if(bCard)
	{
		wsprintf( m_strItemInfo[nParameterIndex], STRMSG_C_060424_0004, nTime); // "Ω√∞£ : ¡ÔΩ√"
	}
	else
	{
		int nItemTime = m_pRefITEM->Time;
		if(bLinkItem == TRUE && m_pRefITEM->LinkItem)
		{
			ITEM* pItemInfo = g_pDatabase->GetServerItemInfo(m_pRefITEM->LinkItem);
			if(pItemInfo == NULL)
				return;
			nItemTime = pItemInfo->Time;
		}
		
		if(nItemTime > 0.0f)
		{
			if(nItemTime >= 60000)
			{
				nTime = (int)(nItemTime / 60000);
				wsprintf( m_strItemInfo[nParameterIndex], STRMSG_C_060331_0002, nTime); // "Ω√∞£ : %d∫–"
			}
			else
			{
				nTime = (int)(nItemTime / 1000);
				wsprintf( m_strItemInfo[nParameterIndex], STRMSG_C_061011_0002, nTime); // "Ω√∞£ : %d√ "			
			}
		}
		else 
		{
			wsprintf( m_strItemInfo[nParameterIndex], STRMSG_C_060424_0005, nTime); // "Ω√∞£ : øµø¯"
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			void CINFItemInfo::SetItemDelAllTime(int nParameterIndex)
/// \brief		æ∆¿Ã≈€ ªË¡¶ √— Ω√∞£
/// \author		ispark
/// \date		2006-10-11 ~ 2006-10-11
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CINFItemInfo::SetItemDelAllTime(int nParameterIndex)
{
	// 2006-10-11 by ispark, ¿⁄µø ªË¡¶ Ω√∞£
	if(COMPARE_BIT_FLAG(m_pRefItemInfo->GetRealItemInfo()->ItemAttribute, ITEM_ATTR_DELETED_TIME_LIMITE))
	{
		int nItemTime = m_pRefItemInfo->GetRealItemInfo()->Endurance;

		int nTime = (int)(nItemTime / 60000);
		wsprintf( m_strItemInfo[nParameterIndex], STRMSG_C_060331_0002, nTime); // "Ω√∞£ : %d∫–"
	}
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			void CINFItemInfo::SetItemDelRemainTime(int nParameterIndex)
/// \brief		æ∆¿Ã≈€ ªË¡¶ ≥≤¿∫ Ω√∞£
/// \author		ispark
/// \date		2006-10-11 ~ 2006-10-11
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CINFItemInfo::SetItemDelRemainTime(int nParameterIndex, BOOL bSetTick)
{
	// 2009-01-12 by bhsohn æ∆¿Ã≈€ ≈¯∆¡ ¿ﬂ∏¯ µ«æÓ πˆ±◊ ºˆ¡§
	memset(m_strItemInfo[nParameterIndex], 0x00, ITEMINFO_ITEM_FULL_NAME);
	// end 2009-01-12 by bhsohn æ∆¿Ã≈€ ≈¯∆¡ ¿ﬂ∏¯ µ«æÓ πˆ±◊ ºˆ¡§


	// 2009. 09. 15 by ckPark Ω√∞£¡¶«— æ∆¿Ã≈€ ≈¯∆¡ πˆ±◊ ºˆ¡§
	if( !m_pRefItemInfo )
		return;
	// end 2009. 09. 15 by ckPark Ω√∞£¡¶«— æ∆¿Ã≈€ ≈¯∆¡ πˆ±◊ ºˆ¡§


	// 2006-10-11 by ispark, ¿⁄µø ªË¡¶ ≥≤¿∫ Ω√∞£
	if(COMPARE_BIT_FLAG(m_pRefItemInfo->GetRealItemInfo()->ItemAttribute, ITEM_ATTR_DELETED_TIME_LIMITE))
	{
		char strDelRemain[1024] = {0,};
		ATUM_DATE_TIME curRemainTime;
		ATUM_DATE_TIME curServerTime = GetServerDateTime();
		int nRemainSecond = ((int)m_pRefItemInfo->GetRealItemInfo()->Endurance * 3600) - (curServerTime.GetTimeInSeconds() - m_pRefItemInfo->CreatedTime.GetTimeInSeconds());
		
		wsprintf( m_strItemInfo[nParameterIndex], STRMSG_C_061011_0003, GetStringDateTimeFormSecond(strDelRemain, nRemainSecond));

		if(bSetTick)
		{
			stTickFuntionIndex stAddTemp;
			stAddTemp.nFuntionIndex = FUNCTION_INDEX_ITEM_DEL_REMAIN_TIME;
			stAddTemp.nDataLineIndex = nParameterIndex;
			m_vecTickFuntionIndex.push_back(stAddTemp);
		}
	}	
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			CINFItemInfo::SetInfluenceBuffItemInfo(BOOL bShop)
/// \brief		
/// \author		ispark
/// \date		2006-04-25 ~ 2006-04-25
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CINFItemInfo::SetInfluenceBuffItemInfo(BOOL bShop)
{
	int index = 0;
	SetItemName( index++ );
	SetUnitKind( index++ );
	SetCount(index++);
	// 2008-01-05 by bhsohn πÃº« ∫∏ªÛ ºˆ¡§æ»
	//SetExclusiveUser( index++ );
	if(m_pRefITEM 
		&& (ITEMKIND_INFLUENCE_BUFF == m_pRefITEM->Kind)
		// 2009-04-21 by bhsohn æ∆¿Ã≈€ DesParam√ﬂ∞°
		//&& (DES_ITEM_BUFF_PARTY == m_pRefITEM->DestParameter1)) // ∆Ì¥Îπˆ«¡ æ∆¿Ã≈€
		&& (DES_ITEM_BUFF_PARTY == m_pRefITEM->ArrDestParameter[0])) // ∆Ì¥Îπˆ«¡ æ∆¿Ã≈€
	{
	//	wsprintf(m_strItemInfo[index++], "\\wType: \\gFormation buff item\\g");
	}
	else
	{
		SetExclusiveUser( index++ );
	}	
	// end 2008-01-05 by bhsohn πÃº« ∫∏ªÛ ºˆ¡§æ»
	
	
	// 2009. 10. 28 by ckPark Ω√∞£/±‚∞£ ¡¶«— æ∆¿Ã≈€ ≈¯∆¡ ºˆ¡§
	//SetItemAllTime(index++, FALSE, TRUE);
	//SetItemRemainTime(index++, bShop, TRUE);

	if( !SetRemainTime_Imp( m_pRefItemInfo, &index, bShop ) )
	{
		if( m_pRefItemInfo )
			SetItemAllTime(index++, FALSE, TRUE);
	}
	// end 2009. 10. 28 by ckPark Ω√∞£/±‚∞£ ¡¶«— æ∆¿Ã≈€ ≈¯∆¡ ºˆ¡§
	
	// ∏µ≈© æ∆¿Ã≈€ ªÁøÎ
	SetParameter(&index, TRUE);
	
	// 2007-09-07 by bhsohn æ∆¿Ã≈€ ∞≈∑°ø©∫Œ «•Ω√
	if(FALSE == bShop)
	{
		SetItemAttribute(index++);
	}

	// 2009. 11. 02 by ckPark ¿Œ««¥œ∆º « µÂ ¿ŒΩ∫≈œΩ∫ ¥¯¡Ø Ω√Ω∫≈€
	SetUseAreaInfinity( &index );
	SetExchangeMaterial( bShop );
	// end 2009. 11. 02 by ckPark ¿Œ««¥œ∆º « µÂ ¿ŒΩ∫≈œΩ∫ ¥¯¡Ø Ω√Ω∫≈€

	SetDesc( index );
	SetItemExtendInfo(TRUE);
	ASSERT_ASSERT(index <= ITEMINFO_PARAMETER_NUMBER);
	SetMaxLength();
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			CINFItemInfo::SetInfluenceGameEventItemInfo(BOOL bShop)
/// \brief		
/// \author		ispark
/// \date		2006-04-25 ~ 2006-04-25
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CINFItemInfo::SetInfluenceGameEventItemInfo(BOOL bShop)
{
	int index = 0;
	SetItemName( index++ );
	SetUnitKind( index++ );
	// 2008-01-05 by bhsohn πÃº« ∫∏ªÛ ºˆ¡§æ»
	//SetExclusiveUser( index++ );
	if(m_pRefITEM 
		&& (ITEMKIND_INFLUENCE_BUFF == m_pRefITEM->Kind)
		// 2009-04-21 by bhsohn æ∆¿Ã≈€ DesParam√ﬂ∞°
		//&& (DES_ITEM_BUFF_PARTY == m_pRefITEM->DestParameter1)) // ∆Ì¥Îπˆ«¡ æ∆¿Ã≈€)
		&& (DES_ITEM_BUFF_PARTY == m_pRefITEM->ArrDestParameter[0])) // ∆Ì¥Îπˆ«¡ æ∆¿Ã≈€)
	{
	}
	else
	{
		SetExclusiveUser( index++ );
	}
	// end 2008-01-05 by bhsohn πÃº« ∫∏ªÛ ºˆ¡§æ»
	SetCount( index++ );

	// 2009. 10. 28 by ckPark Ω√∞£/±‚∞£ ¡¶«— æ∆¿Ã≈€ ≈¯∆¡ ºˆ¡§
	//SetItemAllTime(index++);
	//SetItemRemainTime(index++,bShop);

	if( !SetRemainTime_Imp( m_pRefItemInfo, &index, bShop ) )
	{
		if( m_pRefItemInfo )
			SetItemAllTime(index++, TRUE);
	}
	// end 2009. 10. 28 by ckPark Ω√∞£/±‚∞£ ¡¶«— æ∆¿Ã≈€ ≈¯∆¡ ºˆ¡§
	
	// ≥ªøÎ
	wsprintf( m_strItemInfo[index++], STRMSG_C_ITEM_0055, STRMSG_C_060424_0019); //"EXP, SPI, æ∆¿Ã≈€ µÂ∂¯¿≤ ¿œ¡§Ω√∞£ ªÛΩ¬"
	
	// 2007-09-07 by bhsohn æ∆¿Ã≈€ ∞≈∑°ø©∫Œ «•Ω√
	if(FALSE == bShop)
	{
		SetItemAttribute(index++);
	}

	// 2009. 11. 02 by ckPark ¿Œ««¥œ∆º « µÂ ¿ŒΩ∫≈œΩ∫ ¥¯¡Ø Ω√Ω∫≈€
	SetUseAreaInfinity( &index );
	SetExchangeMaterial( bShop );
	// end 2009. 11. 02 by ckPark ¿Œ««¥œ∆º « µÂ ¿ŒΩ∫≈œΩ∫ ¥¯¡Ø Ω√Ω∫≈€

	SetDesc( index );
	SetItemExtendInfo(TRUE);
	ASSERT_ASSERT(index <= ITEMINFO_PARAMETER_NUMBER);
	SetMaxLength();
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			VOID CINFItemInfo::SetRandomBoxItemInfo(BOOL bShop)
/// \brief		∑£¥˝π⁄Ω∫ ≈¯∆¡ ¡§∫∏.
/// \author		dgwoo
/// \date		2006-08-11 ~ 2006-08-11
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CINFItemInfo::SetRandomBoxItemInfo(BOOL bShop)
{
	int index = 0;
	SetItemName( index++ );
	SetItemKind( index++ );

	// 2009. 10. 28 by ckPark Ω√∞£/±‚∞£ ¡¶«— æ∆¿Ã≈€ ≈¯∆¡ ºˆ¡§
// 	// 2009. 09. 15 by ckPark Ω√∞£¡¶«— æ∆¿Ã≈€ ≈¯∆¡ πˆ±◊ ºˆ¡§
// 	
// 	//SetItemDelRemainTime(index++);			// 2006-10-11 by ispark
// 	if( m_pRefItemInfo )
// 		SetItemDelRemainTime(index++);
// 	
// 	// end 2009. 09. 15 by ckPark Ω√∞£¡¶«— æ∆¿Ã≈€ ≈¯∆¡ πˆ±◊ ºˆ¡§

	SetRemainTime_Imp( m_pRefItemInfo, &index, bShop );
	// end 2009. 10. 28 by ckPark Ω√∞£/±‚∞£ ¡¶«— æ∆¿Ã≈€ ≈¯∆¡ ºˆ¡§

	// 2010. 02. 10 by ckPark ∑£¥˝π⁄Ω∫æ∆¿Ã≈€ø° ∑π∫ß ¡¶«— √ﬂ∞°, ≈≥∏∂≈© ∑π∫ß¡¶«—≈¯∆¡ ªË¡¶
	SetReqLevel( index++ );
	// end 2010. 02. 10 by ckPark ∑£¥˝π⁄Ω∫æ∆¿Ã≈€ø° ∑π∫ß ¡¶«— √ﬂ∞°, ≈≥∏∂≈© ∑π∫ß¡¶«—≈¯∆¡ ªË¡¶

	// 2007-09-07 by bhsohn æ∆¿Ã≈€ ∞≈∑°ø©∫Œ «•Ω√
	if(FALSE == bShop)
	{
		SetItemAttribute(index++);
	}

	// 2009. 11. 02 by ckPark ¿Œ««¥œ∆º « µÂ ¿ŒΩ∫≈œΩ∫ ¥¯¡Ø Ω√Ω∫≈€
	SetUseAreaInfinity( &index );
	SetExchangeMaterial( bShop );
	// end 2009. 11. 02 by ckPark ¿Œ««¥œ∆º « µÂ ¿ŒΩ∫≈œΩ∫ ¥¯¡Ø Ω√Ω∫≈€

	SetDesc( index );
	SetItemExtendInfo(TRUE);
	ASSERT_ASSERT(index <= ITEMINFO_PARAMETER_NUMBER);
	SetMaxLength();
}


///////////////////////////////////////////////////////////////////////////////
/// \fn			VOID CINFItemInfo::SetMarkItemInfo(BOOL bShop)
/// \brief		∏∂≈© æ∆¿Ã≈€
/// \author		ispark
/// \date		2006-08-21 ~ 2006-08-21
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CINFItemInfo::SetMarkItemInfo(BOOL bShop)
{
	int index = 0;
	SetItemName( index++ );
	SetItemKind( index++ );
	SetParameter(&index);
	//SetWeight( index++ );

	// 2009. 08. 27 by ckPark ±◊∑°«» ∏Æº“Ω∫ ∫Ø∞Ê Ω√Ω∫≈€ ±∏«ˆ
	if( m_pRefItemInfo )
		SetShapeInfo( index++ );
	// end 2009. 08. 27 by ckPark ±◊∑°«» ∏Æº“Ω∫ ∫Ø∞Ê Ω√Ω∫≈€ ±∏«ˆ

	// 2007-09-07 by bhsohn Ω√∞£¡¶ ∏∂≈© æ∆¿Ã≈€ √ﬂ∞°
	
	
	// 2009. 10. 28 by ckPark Ω√∞£/±‚∞£ ¡¶«— æ∆¿Ã≈€ ≈¯∆¡ ºˆ¡§
// 	// 2009. 09. 15 by ckPark Ω√∞£¡¶«— æ∆¿Ã≈€ ≈¯∆¡ πˆ±◊ ºˆ¡§
// 	
// 	//SetItemDelRemainTime(index++);
// 	if( m_pRefItemInfo )
// 		SetItemDelRemainTime(index++);
// 	
// 	// end 2009. 09. 15 by ckPark Ω√∞£¡¶«— æ∆¿Ã≈€ ≈¯∆¡ πˆ±◊ ºˆ¡§
	SetRemainTime_Imp( m_pRefItemInfo, &index, bShop );
	// end 2009. 10. 28 by ckPark Ω√∞£/±‚∞£ ¡¶«— æ∆¿Ã≈€ ≈¯∆¡ ºˆ¡§
	
	if(FALSE == bShop)
	{
		SetItemAttribute(index++);
	}

	// 2009. 11. 02 by ckPark ¿Œ««¥œ∆º « µÂ ¿ŒΩ∫≈œΩ∫ ¥¯¡Ø Ω√Ω∫≈€
	SetUseAreaInfinity( &index );
	SetExchangeMaterial( bShop );
	// end 2009. 11. 02 by ckPark ¿Œ««¥œ∆º « µÂ ¿ŒΩ∫≈œΩ∫ ¥¯¡Ø Ω√Ω∫≈€

	SetDesc( index );	
	SetItemExtendInfo(TRUE);
	ASSERT_ASSERT(index <= ITEMINFO_PARAMETER_NUMBER);
	SetMaxLength();
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			CINFItemInfo::SetExclusiveUser(int nParameterIndex)
/// \brief		ªÁøÎ±««—
/// \author		ispark
/// \date		2006-04-25 ~ 2006-04-25
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CINFItemInfo::SetExclusiveUser(int nParameterIndex)
{
	switch(m_pRefITEM->Kind) 
	{
	case ITEMKIND_INFLUENCE_BUFF:
		{
			wsprintf( m_strItemInfo[nParameterIndex], STRMSG_C_060424_0016, STRMSG_C_060424_0017);
		}
		break;
	case ITEMKIND_INFLUENCE_GAMEEVENT:
		{
			wsprintf( m_strItemInfo[nParameterIndex], STRMSG_C_060424_0016, STRMSG_C_060424_0017);
		}
		break;
	}
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			CINFItemInfo::SetParameter(int* index, BOOL bLinkItem)
/// \brief		æ∆¿Ã≈€ ±‚¥…
/// \author		ispark
/// \date		2006-04-25 ~ 2006-04-25
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
// 2010. 04. 21 by jskim Ω≈±‘ ∑∞≈∞ ∏”Ω≈ ±∏«ˆ
//void CINFItemInfo::SetParameter(int* index, BOOL bLinkItem, BOOL bArmorItem/*= FALSE*/)
#ifdef BONUS_STAT_ITEM
void CINFItemInfo::SetParameter(int* index, BOOL bLinkItem, BOOL bArmorItem/*= FALSE*/, float RareValue/*= 0.0f*/, BOOL Bonus)
{
	// ≥ªøÎ
	if (bLinkItem == TRUE)
	{
		// ∏µ≈©æ∆¿Ã≈€ ªÁøÎ
		ITEM* pItemInfo = g_pDatabase->GetServerItemInfo(m_pRefItemInfo->GetRealItemInfo()->LinkItem);
		// 2009-04-21 by bhsohn æ∆¿Ã≈€ DesParam√ﬂ∞°
		// 		if(pItemInfo->DestParameter1 != 0)
		// 			SetFunction( (*index)++, pItemInfo->DestParameter1, pItemInfo->ParameterValue1, 0,0);
		// 		if(pItemInfo->DestParameter2 != 0)
		// 			SetFunction( (*index)++, pItemInfo->DestParameter2, pItemInfo->ParameterValue2, 0,0);
		// 		if(pItemInfo->DestParameter3 != 0)
		// 			SetFunction( (*index)++, pItemInfo->DestParameter3, pItemInfo->ParameterValue3, 0,0);
		// 		if(pItemInfo->DestParameter4 != 0)
		// 			SetFunction( (*index)++, pItemInfo->DestParameter4, pItemInfo->ParameterValue4, 0,0);
		int nArrParamCnt = 0;
		for (nArrParamCnt = 0; nArrParamCnt < SIZE_MAX_DESPARAM_COUNT_IN_ITEM; nArrParamCnt++)
		{
			// 2010. 04. 21 by jskim Ω≈±‘ ∑∞≈∞ ∏”Ω≈ ±∏«ˆ
			// 			if(m_pRefITEM->ArrDestParameter[nArrParamCnt]!= 0)
			// 			{
			// 2009. 12. 16 by jskim DES_CASH_HP_AND_DP_UP æ∆¿Ã≈€ø° ¥Î«— ≈¯∆¡ √≥∏Æ  
			// 				if(!(int)m_pRefITEM->ArrParameterValue[nArrParamCnt] && m_pRefITEM->ArrDestParameter[0] == DES_CASH_HP_AND_DP_UP)
			// 					continue;
			//end 2009. 12. 16 by jskim DES_CASH_HP_AND_DP_UP æ∆¿Ã≈€ø° ¥Î«— ≈¯∆¡ √≥∏Æ  
			// 				SetFunction( (*index)++, m_pRefITEM->ArrDestParameter[nArrParamCnt], 
			// 							m_pRefITEM->ArrParameterValue[nArrParamCnt], 0,0);
			float tempValue = 0.0f;
			tempValue = GetRareParameterValue(m_pRefItemInfo, nArrParamCnt);
			if (m_pRefITEM->ArrDestParameter[nArrParamCnt] != 0)
			{
				// 2009. 12. 16 by jskim DES_CASH_HP_AND_DP_UP æ∆¿Ã≈€ø° ¥Î«— ≈¯∆¡ √≥∏Æ  
				if (!(int)m_pRefITEM->ArrParameterValue[nArrParamCnt] && m_pRefITEM->ArrDestParameter[0] == DES_CASH_HP_AND_DP_UP)
					continue;
				//end 2009. 12. 16 by jskim DES_CASH_HP_AND_DP_UP æ∆¿Ã≈€ø° ¥Î«— ≈¯∆¡ √≥∏Æ  
				SetFunction((*index)++, m_pRefITEM->ArrDestParameter[nArrParamCnt],
					m_pRefITEM->ArrParameterValue[nArrParamCnt], 0, 0, FUNCTIONTYPE_NORMAL, tempValue, Bonus);
			}
		}
		DefferentFunction(index, m_pRefItemInfo);
		//end 2010. 04. 21 by jskim Ω≈±‘ ∑∞≈∞ ∏”Ω≈ ±∏«ˆ
		// end 2009-04-21 by bhsohn æ∆¿Ã≈€ DesParam√ﬂ∞°

	}
	else
	{
		// 2009-04-21 by bhsohn æ∆¿Ã≈€ DesParam√ﬂ∞°
		// 		if(m_pRefITEM->DestParameter1 != 0)
		// 			SetFunction( (*index)++, m_pRefITEM->DestParameter1, m_pRefITEM->ParameterValue1, 0,0);
		// 		if(m_pRefITEM->DestParameter2 != 0)
		// 			SetFunction( (*index)++, m_pRefITEM->DestParameter2, m_pRefITEM->ParameterValue2, 0,0);
		// 		if(m_pRefITEM->DestParameter3 != 0)
		// 			SetFunction( (*index)++, m_pRefITEM->DestParameter3, m_pRefITEM->ParameterValue3, 0,0);
		// 		if(m_pRefITEM->DestParameter4 != 0)
		// 			SetFunction( (*index)++, m_pRefITEM->DestParameter4, m_pRefITEM->ParameterValue4, 0,0);
		int nArrParamCnt = 0;
		for (nArrParamCnt = 0; nArrParamCnt < SIZE_MAX_DESPARAM_COUNT_IN_ITEM; nArrParamCnt++)
		{
			// 2010. 04. 21 by jskim Ω≈±‘ ∑∞≈∞ ∏”Ω≈ ±∏«ˆ
			// 			if(m_pRefITEM->ArrDestParameter[nArrParamCnt]!= 0)
			// 			{
			// 2009. 12. 16 by jskim DES_CASH_HP_AND_DP_UP æ∆¿Ã≈€ø° ¥Î«— ≈¯∆¡ √≥∏Æ  
			// 				if(!(int)m_pRefITEM->ArrParameterValue[nArrParamCnt] && m_pRefITEM->ArrDestParameter[0] == DES_CASH_HP_AND_DP_UP)
			// 					continue;
			//end 2009. 12. 16 by jskim DES_CASH_HP_AND_DP_UP æ∆¿Ã≈€ø° ¥Î«— ≈¯∆¡ √≥∏Æ  
			// 				SetFunction( (*index)++, m_pRefITEM->ArrDestParameter[nArrParamCnt], 
			// 										m_pRefITEM->ArrParameterValue[nArrParamCnt], 0,0);
			float tempValue = 0.0f;
			tempValue = GetRareParameterValue(m_pRefItemInfo, nArrParamCnt);
#ifdef _INET_PET
			//2011-10-06 by jhahn ∆ƒ∆Æ≥  º∫¿Â«¸ Ω√Ω∫≈€
			if (m_pRefITEM->ArrDestParameter[nArrParamCnt] != 0 &&
				m_pRefITEM->ArrDestParameter[nArrParamCnt] != DES_PET_SLOT_ITEM_AUTOKIT_HP &&
				m_pRefITEM->ArrDestParameter[nArrParamCnt] != DES_PET_SLOT_ITEM_AUTOKIT_SHIELD &&
				m_pRefITEM->ArrDestParameter[nArrParamCnt] != DES_PET_SLOT_ITEM_AUTOKIT_SP &&
				m_pRefITEM->ArrDestParameter[nArrParamCnt] != DES_PET_SLOT_ITEM_AUTOSKILL_AGEAR &&
				m_pRefITEM->ArrDestParameter[nArrParamCnt] != DES_PET_SLOT_ITEM_AUTOSKILL_BGEAR &&
				m_pRefITEM->ArrDestParameter[nArrParamCnt] != DES_PET_SLOT_ITEM_AUTOSKILL_IGEAR &&
				m_pRefITEM->ArrDestParameter[nArrParamCnt] != DES_PET_SLOT_ITEM_AUTOSKILL_MGEAR)

				//end 2011-10-06 by jhahn ∆ƒ∆Æ≥  º∫¿Â«¸ Ω√Ω∫≈€
#else
			if (m_pRefITEM->ArrDestParameter[nArrParamCnt] != 0)
#endif
			{
				// 2009. 12. 16 by jskim DES_CASH_HP_AND_DP_UP æ∆¿Ã≈€ø° ¥Î«— ≈¯∆¡ √≥∏Æ  
				if (!(int)m_pRefITEM->ArrParameterValue[nArrParamCnt] && m_pRefITEM->ArrDestParameter[0] == DES_CASH_HP_AND_DP_UP)
					continue;
				//end 2009. 12. 16 by jskim DES_CASH_HP_AND_DP_UP æ∆¿Ã≈€ø° ¥Î«— ≈¯∆¡ √≥∏Æ  
				SetFunction((*index)++, m_pRefITEM->ArrDestParameter[nArrParamCnt],
					m_pRefITEM->ArrParameterValue[nArrParamCnt], 0, 0, FUNCTIONTYPE_NORMAL, tempValue, Bonus);
			}
		}
		DefferentFunction(index, m_pRefItemInfo);
		//end 2010. 04. 21 by jskim Ω≈±‘ ∑∞≈∞ ∏”Ω≈ ±∏«ˆ
		// end 2009-04-21 by bhsohn æ∆¿Ã≈€ DesParam√ﬂ∞°
	}

	// 2010. 02. 11 by ckPark πﬂµø∑˘ ¿Â¬¯æ∆¿Ã≈€

	// 	// 2008-09-26 by bhsohn Ω≈±‘ ¿Œ√æ∆Æ √≥∏Æ
	// 	if(bArmorItem && m_pRefEnchant)
	// 	{
	// 		// æ∆∏” æ∆¿Ã≈€¿∫ ¿Œ√æ¿Ã ¿÷¿ªºˆ ¿÷¥Ÿ.
	// 		int nArmorCheck[4] =
	// 		{
	// 			DES_DEFENSE_01,
	// 			DES_DEFENSE_02,
	// 			DES_DEFENSEPROBABILITY_01,
	// 			DES_DEFENSEPROBABILITY_02
	// 		};		
	// 		int nCnt = 0;		
	// 		for(nCnt = 0;nCnt < 4;nCnt++)
	// 		{
	// 			float fEnchant = GetParamFactor_DesParam( *m_pRefEnchant, nArmorCheck[nCnt] );
	// 			if( fEnchant <= 0)
	// 			{
	// 				// ¿Œ√æ∆Æ ø©∫Œ ¿÷¥¬√º≈©
	// 				continue;
	// 			}
	// 
	// 			// 2009-04-21 by bhsohn æ∆¿Ã≈€ DesParam√ﬂ∞°
	// // 			if((m_pRefITEM->DestParameter1 != nArmorCheck[nCnt])
	// // 				&& (m_pRefITEM->DestParameter2 != nArmorCheck[nCnt])
	// // 				&& (m_pRefITEM->DestParameter3 != nArmorCheck[nCnt])
	// // 				&& (m_pRefITEM->DestParameter4 != nArmorCheck[nCnt]))
	// // 			{
	// // 				SetFunction( (*index)++, nArmorCheck[nCnt], 0, 0,0);
	// // 			}			
	// 			int nArrParamCnt = 0;
	// 			BOOL bDesParam = TRUE;
	// 			for(nArrParamCnt = 0; nArrParamCnt < SIZE_MAX_DESPARAM_COUNT_IN_ITEM; nArrParamCnt++)
	// 			{
	// 				bDesParam &= (m_pRefITEM->ArrDestParameter[nArrParamCnt] != nArmorCheck[nCnt]);	
	// 			}
	// 			if(bDesParam)				
	// 			{
	// 				// 2009. 12. 16 by jskim DES_CASH_HP_AND_DP_UP æ∆¿Ã≈€ø° ¥Î«— ≈¯∆¡ √≥∏Æ  
	// 				if(!(int)m_pRefITEM->ArrParameterValue[nArrParamCnt] && m_pRefITEM->ArrDestParameter[0] == DES_CASH_HP_AND_DP_UP)
	// 					continue;
	// 				//end 2009. 12. 16 by jskim DES_CASH_HP_AND_DP_UP æ∆¿Ã≈€ø° ¥Î«— ≈¯∆¡ √≥∏Æ  
	// 				SetFunction( (*index)++, nArmorCheck[nCnt], 0, 0,0);
	// 			}			
	// 			// end 2009-04-21 by bhsohn æ∆¿Ã≈€ DesParam√ﬂ∞°
	// 		}
	// 		
	// 
	// 	}
	// 	// end 2008-09-26 by bhsohn Ω≈±‘ ¿Œ√æ∆Æ √≥∏Æ

	if (m_pRefItemInfo)
	{
		std::vector<DestParam_t>	vecAdditionalFunctionList;		// ¿Œ√¶∆Æ∑Œ ¿Œ«ÿ ªı∑Œ ∫Œø©µ» ±‚¥…∏ÆΩ∫∆Æ
		std::vector<ENCHANT_PARAM>	vecDefEnchant = *(m_pRefItemInfo->GetDefEnchantParamList());
		std::vector<ENCHANT_PARAM>::iterator it = vecDefEnchant.begin();
		while (it != vecDefEnchant.end())
		{
			// ø¯∑° ∞Ì¿Ø¿« ±‚¥…¿Ã æ∆¥œ∞Ì ªı∑Œ ∫Œø©µ» ±‚¥…¿œ ∞ÊøÏ ≈¯∆¡¿ª √ﬂ∞°«—¥Ÿ
			if (!m_pRefItemInfo->ItemInfo->IsExistDesParam((*it).m_nDesParam)
				&& vecAdditionalFunctionList.end() == std::find(vecAdditionalFunctionList.begin(), vecAdditionalFunctionList.end(), (*it).m_nDesParam))
			{
				SetFunction((*index)++, (*it).m_nDesParam, 0.0f, 0, 0.0f);
				// µø¿œ«— ±‚¥…ø°º≠ ¿Œ√¶∆Æ∫∞∑Œ ≈¯∆¡¿ª ∂ÁøÏ¡ˆ æ ¿Ω
				vecAdditionalFunctionList.push_back((*it).m_nDesParam);
			}

			++it;
		}
		/*
		std::vector<ENCHANT_PARAM>	vecDefBonus = *(m_pRefItemInfo->GetDefBonusParamList());
		std::vector<ENCHANT_PARAM>::iterator itr = vecDefBonus.begin();
		while (itr != vecDefBonus.end())
		{
			// ø¯∑° ∞Ì¿Ø¿« ±‚¥…¿Ã æ∆¥œ∞Ì ªı∑Œ ∫Œø©µ» ±‚¥…¿œ ∞ÊøÏ ≈¯∆¡¿ª √ﬂ∞°«—¥Ÿ
			// 2013-02-14 by bhsohn ∆Ø¡§ º”º∫ æ∆¿Ã≈€ ≈¯∆¡ ¿ÃªÛ«œ∞‘ ¬Ô»˜¥¬ «ˆªÛ √≥∏Æ
			// 			if( !m_pRefItemInfo->ItemInfo->IsExistDesParam( (*it).m_nDesParam )
			// 				&& vecAdditionalFunctionList.end() == std::find( vecAdditionalFunctionList.begin(), vecAdditionalFunctionList.end(), (*it).m_nDesParam ) )
			if (!m_pRefItemInfo->ItemInfo->IsExistDesParam((*itr).m_nDesParam)
		// ≈¯∆¡ø° ∫∏ø©¡Ÿ¡ˆ ø©∫Œ ∆«¥‹
				&& vecAdditionalFunctionList.end() == std::find(vecAdditionalFunctionList.begin(), vecAdditionalFunctionList.end(), (*itr).m_nDesParam))

			{
				SetFunction((*index)++, (*itr).m_nDesParam, 0.0f, 0, 0.0f);
				// µø¿œ«— ±‚¥…ø°º≠ ¿Œ√¶∆Æ∫∞∑Œ ≈¯∆¡¿ª ∂ÁøÏ¡ˆ æ ¿Ω
				vecAdditionalFunctionList.push_back((*itr).m_nDesParam);
			}

			++itr;
		}
		*/
	}

	// end 2010. 02. 11 by ckPark πﬂµø∑˘ ¿Â¬¯æ∆¿Ã≈€
}
#else
void CINFItemInfo::SetParameter(int* index, BOOL bLinkItem, BOOL bArmorItem/*= FALSE*/, float RareValue/*= 0.0f*/)
//end 2010. 04. 21 by jskim Ω≈±‘ ∑∞≈∞ ∏”Ω≈ ±∏«ˆ
{
	//16-09-2020 by Ientpub - remaked that shit
	if(bLinkItem == TRUE) {
		if(m_pRefItemInfo){
			auto* pItemInfo = g_pDatabase->GetServerItemInfo(m_pRefItemInfo->GetRealItemInfo()->LinkItem);

			for(auto nArrParamCnt = 0; nArrParamCnt < SIZE_MAX_DESPARAM_COUNT_IN_ITEM; nArrParamCnt++) {
				const auto tempValue = GetRareParameterValue(m_pRefItemInfo, nArrParamCnt);
				if(pItemInfo->ArrDestParameter[nArrParamCnt]!= 0) {
				
					if(!static_cast<int>(pItemInfo->ArrParameterValue[nArrParamCnt]) && pItemInfo->ArrDestParameter[0] == DES_CASH_HP_AND_DP_UP)
						continue;
				
					SetFunction( (*index)++, pItemInfo->ArrDestParameter[nArrParamCnt],
						pItemInfo->ArrParameterValue[nArrParamCnt], 0,0, FUNCTIONTYPE_NORMAL, tempValue);
				}
			}	
			DefferentFunction(index, m_pRefItemInfo);
		}
	}
	else
	{
		// 2009-04-21 by bhsohn æ∆¿Ã≈€ DesParam√ﬂ∞°
		// 		if(m_pRefITEM->DestParameter1 != 0)
		// 			SetFunction( (*index)++, m_pRefITEM->DestParameter1, m_pRefITEM->ParameterValue1, 0,0);
		// 		if(m_pRefITEM->DestParameter2 != 0)
		// 			SetFunction( (*index)++, m_pRefITEM->DestParameter2, m_pRefITEM->ParameterValue2, 0,0);
		// 		if(m_pRefITEM->DestParameter3 != 0)
		// 			SetFunction( (*index)++, m_pRefITEM->DestParameter3, m_pRefITEM->ParameterValue3, 0,0);
		// 		if(m_pRefITEM->DestParameter4 != 0)
		// 			SetFunction( (*index)++, m_pRefITEM->DestParameter4, m_pRefITEM->ParameterValue4, 0,0);
		int nArrParamCnt = 0;
		for(nArrParamCnt = 0; nArrParamCnt < SIZE_MAX_DESPARAM_COUNT_IN_ITEM; nArrParamCnt++)
		{
			// 2010. 04. 21 by jskim Ω≈±‘ ∑∞≈∞ ∏”Ω≈ ±∏«ˆ
			// 			if(m_pRefITEM->ArrDestParameter[nArrParamCnt]!= 0)
			// 			{
			// 2009. 12. 16 by jskim DES_CASH_HP_AND_DP_UP æ∆¿Ã≈€ø° ¥Î«— ≈¯∆¡ √≥∏Æ  
			// 				if(!(int)m_pRefITEM->ArrParameterValue[nArrParamCnt] && m_pRefITEM->ArrDestParameter[0] == DES_CASH_HP_AND_DP_UP)
			// 					continue;
			//end 2009. 12. 16 by jskim DES_CASH_HP_AND_DP_UP æ∆¿Ã≈€ø° ¥Î«— ≈¯∆¡ √≥∏Æ  
			// 				SetFunction( (*index)++, m_pRefITEM->ArrDestParameter[nArrParamCnt], 
			// 										m_pRefITEM->ArrParameterValue[nArrParamCnt], 0,0);
			float tempValue= 0.0f;
			tempValue = GetRareParameterValue(m_pRefItemInfo, nArrParamCnt);
#ifdef _INET_PET
			//2011-10-06 by jhahn ∆ƒ∆Æ≥  º∫¿Â«¸ Ω√Ω∫≈€
			if (m_pRefITEM->ArrDestParameter[nArrParamCnt] != 0 &&
				m_pRefITEM->ArrDestParameter[nArrParamCnt] != DES_PET_SLOT_ITEM_AUTOKIT_HP &&
				m_pRefITEM->ArrDestParameter[nArrParamCnt] != DES_PET_SLOT_ITEM_AUTOKIT_SHIELD &&
				m_pRefITEM->ArrDestParameter[nArrParamCnt] != DES_PET_SLOT_ITEM_AUTOKIT_SP &&
				m_pRefITEM->ArrDestParameter[nArrParamCnt] != DES_PET_SLOT_ITEM_AUTOSKILL_AGEAR &&
				m_pRefITEM->ArrDestParameter[nArrParamCnt] != DES_PET_SLOT_ITEM_AUTOSKILL_BGEAR &&
				m_pRefITEM->ArrDestParameter[nArrParamCnt] != DES_PET_SLOT_ITEM_AUTOSKILL_IGEAR &&
				m_pRefITEM->ArrDestParameter[nArrParamCnt] != DES_PET_SLOT_ITEM_AUTOSKILL_MGEAR)

				//end 2011-10-06 by jhahn ∆ƒ∆Æ≥  º∫¿Â«¸ Ω√Ω∫≈€
#else
			if(m_pRefITEM->ArrDestParameter[nArrParamCnt]!= 0)
#endif
			{
				// 2009. 12. 16 by jskim DES_CASH_HP_AND_DP_UP æ∆¿Ã≈€ø° ¥Î«— ≈¯∆¡ √≥∏Æ  
				if(!(int)m_pRefITEM->ArrParameterValue[nArrParamCnt] && m_pRefITEM->ArrDestParameter[0] == DES_CASH_HP_AND_DP_UP)
					continue;
				//end 2009. 12. 16 by jskim DES_CASH_HP_AND_DP_UP æ∆¿Ã≈€ø° ¥Î«— ≈¯∆¡ √≥∏Æ  
				SetFunction( (*index)++, m_pRefITEM->ArrDestParameter[nArrParamCnt], 
					m_pRefITEM->ArrParameterValue[nArrParamCnt], 0,0, FUNCTIONTYPE_NORMAL, tempValue);
			}			
		}
		DefferentFunction(index, m_pRefItemInfo);
		//end 2010. 04. 21 by jskim Ω≈±‘ ∑∞≈∞ ∏”Ω≈ ±∏«ˆ
		// end 2009-04-21 by bhsohn æ∆¿Ã≈€ DesParam√ﬂ∞°
	}
	
	// 2010. 02. 11 by ckPark πﬂµø∑˘ ¿Â¬¯æ∆¿Ã≈€

// 	// 2008-09-26 by bhsohn Ω≈±‘ ¿Œ√æ∆Æ √≥∏Æ
// 	if(bArmorItem && m_pRefEnchant)
// 	{
// 		// æ∆∏” æ∆¿Ã≈€¿∫ ¿Œ√æ¿Ã ¿÷¿ªºˆ ¿÷¥Ÿ.
// 		int nArmorCheck[4] =
// 		{
// 			DES_DEFENSE_01,
// 			DES_DEFENSE_02,
// 			DES_DEFENSEPROBABILITY_01,
// 			DES_DEFENSEPROBABILITY_02
// 		};		
// 		int nCnt = 0;		
// 		for(nCnt = 0;nCnt < 4;nCnt++)
// 		{
// 			float fEnchant = GetParamFactor_DesParam( *m_pRefEnchant, nArmorCheck[nCnt] );
// 			if( fEnchant <= 0)
// 			{
// 				// ¿Œ√æ∆Æ ø©∫Œ ¿÷¥¬√º≈©
// 				continue;
// 			}
// 
// 			// 2009-04-21 by bhsohn æ∆¿Ã≈€ DesParam√ﬂ∞°
// // 			if((m_pRefITEM->DestParameter1 != nArmorCheck[nCnt])
// // 				&& (m_pRefITEM->DestParameter2 != nArmorCheck[nCnt])
// // 				&& (m_pRefITEM->DestParameter3 != nArmorCheck[nCnt])
// // 				&& (m_pRefITEM->DestParameter4 != nArmorCheck[nCnt]))
// // 			{
// // 				SetFunction( (*index)++, nArmorCheck[nCnt], 0, 0,0);
// // 			}			
// 			int nArrParamCnt = 0;
// 			BOOL bDesParam = TRUE;
// 			for(nArrParamCnt = 0; nArrParamCnt < SIZE_MAX_DESPARAM_COUNT_IN_ITEM; nArrParamCnt++)
// 			{
// 				bDesParam &= (m_pRefITEM->ArrDestParameter[nArrParamCnt] != nArmorCheck[nCnt]);	
// 			}
// 			if(bDesParam)				
// 			{
// 				// 2009. 12. 16 by jskim DES_CASH_HP_AND_DP_UP æ∆¿Ã≈€ø° ¥Î«— ≈¯∆¡ √≥∏Æ  
// 				if(!(int)m_pRefITEM->ArrParameterValue[nArrParamCnt] && m_pRefITEM->ArrDestParameter[0] == DES_CASH_HP_AND_DP_UP)
// 					continue;
// 				//end 2009. 12. 16 by jskim DES_CASH_HP_AND_DP_UP æ∆¿Ã≈€ø° ¥Î«— ≈¯∆¡ √≥∏Æ  
// 				SetFunction( (*index)++, nArmorCheck[nCnt], 0, 0,0);
// 			}			
// 			// end 2009-04-21 by bhsohn æ∆¿Ã≈€ DesParam√ﬂ∞°
// 		}
// 		
// 
// 	}
// 	// end 2008-09-26 by bhsohn Ω≈±‘ ¿Œ√æ∆Æ √≥∏Æ

	if( m_pRefItemInfo )
	{
		std::vector<DestParam_t>	vecAdditionalFunctionList;		// ¿Œ√¶∆Æ∑Œ ¿Œ«ÿ ªı∑Œ ∫Œø©µ» ±‚¥…∏ÆΩ∫∆Æ
		std::vector<ENCHANT_PARAM>	vecDefEnchant	= *(m_pRefItemInfo->GetDefEnchantParamList());
		std::vector<ENCHANT_PARAM>::iterator it		= vecDefEnchant.begin();
		while( it != vecDefEnchant.end() )
		{
			// ø¯∑° ∞Ì¿Ø¿« ±‚¥…¿Ã æ∆¥œ∞Ì ªı∑Œ ∫Œø©µ» ±‚¥…¿œ ∞ÊøÏ ≈¯∆¡¿ª √ﬂ∞°«—¥Ÿ
			if( !m_pRefItemInfo->GetRealItemInfo()->IsExistDesParam( (*it).m_nDesParam )
				&& vecAdditionalFunctionList.end() == std::find( vecAdditionalFunctionList.begin(), vecAdditionalFunctionList.end(), (*it).m_nDesParam ) )
			{
				SetFunction( (*index)++, (*it).m_nDesParam, 0.0f, 0, 0.0f );
				// µø¿œ«— ±‚¥…ø°º≠ ¿Œ√¶∆Æ∫∞∑Œ ≈¯∆¡¿ª ∂ÁøÏ¡ˆ æ ¿Ω
				vecAdditionalFunctionList.push_back( (*it).m_nDesParam );
			}

			++it;
		}
	}

	// end 2010. 02. 11 by ckPark πﬂµø∑˘ ¿Â¬¯æ∆¿Ã≈€
}
#endif
// 2010. 04. 21 by jskim Ω≈±‘ ∑∞≈∞ ∏”Ω≈ ±∏«ˆ
int	 CINFItemInfo::GetRareParameterValue(CItemInfo* pRefItemInfo, int num)
{
	float tempValue= 0.0f;
	for(int i=0; i < SIZE_MAX_DESPARAM_COUNT_IN_ITEM; i++)
	{
		if(pRefItemInfo && pRefItemInfo->GetPrefixRareInfo())
		{
			if(m_pRefITEM->ArrDestParameter[num] == pRefItemInfo->GetPrefixRareInfo()->DesParameter[i])
				tempValue += m_pRefItemInfo->GetPrefixRareInfo()->ParameterValue[i];
		}				
		if(m_pRefItemInfo && m_pRefItemInfo->GetSuffixRareInfo())
		{
			if(m_pRefITEM->ArrDestParameter[num] == m_pRefItemInfo->GetSuffixRareInfo()->DesParameter[i])
				tempValue += m_pRefItemInfo->GetSuffixRareInfo()->ParameterValue[i];
		}
	}
	return tempValue;
}

void CINFItemInfo::DefferentFunction(int* index, CItemInfo* pRefItemInfo)
{
	vector<BYTE> tempDesParameter;
	for(int i=0; i < SIZE_MAX_DESPARAM_COUNT_IN_ITEM; i++)
	{
		BOOL tempPreCmp = FALSE;
		BOOL tempSufCmp = FALSE;
		for(int j = 0; j< SIZE_MAX_DESPARAM_COUNT_IN_ITEM; j++)
		{
			if(pRefItemInfo && pRefItemInfo->GetPrefixRareInfo())
			{
				if(pRefItemInfo->GetPrefixRareInfo()->DesParameter[i] == m_pRefITEM->ArrDestParameter[j] &&
					pRefItemInfo->GetPrefixRareInfo()->DesParameter[i] != 0)
				{
					tempPreCmp = TRUE;
				}
			}		
			if(pRefItemInfo && pRefItemInfo->GetSuffixRareInfo())
			{
				if(pRefItemInfo->GetSuffixRareInfo()->DesParameter[i] == m_pRefITEM->ArrDestParameter[j] &&
					pRefItemInfo->GetSuffixRareInfo()->DesParameter[i] != 0)
				{
					tempSufCmp = TRUE;
				}
			}
			if(pRefItemInfo && pRefItemInfo->GetPrefixRareInfo() && pRefItemInfo->GetSuffixRareInfo() &&
				pRefItemInfo->GetPrefixRareInfo()->DesParameter[i] != NULL &&
				pRefItemInfo->GetSuffixRareInfo()->DesParameter[i] != NULL )
			{
				if(pRefItemInfo->GetPrefixRareInfo()->DesParameter[i] == pRefItemInfo->GetSuffixRareInfo()->DesParameter[j]	)
				{
					tempDesParameter.push_back(pRefItemInfo->GetPrefixRareInfo()->DesParameter[i]);
				}			
			}
		}

		float tempCmp = 0.0f;
		if(!tempPreCmp)
		{
			if(pRefItemInfo && pRefItemInfo->GetPrefixRareInfo())
			{	
				if((!pRefItemInfo->GetPrefixRareInfo()->DesParameter[i]	== NULL	|| !pRefItemInfo->GetPrefixRareInfo()->ParameterValue[i] == NULL))
				{
					for(int j = 0; j< SIZE_MAX_DESPARAM_COUNT_IN_ITEM; j++)
					{
						if(pRefItemInfo->GetPrefixRareInfo() && pRefItemInfo->GetSuffixRareInfo())
						{
							if(pRefItemInfo->GetPrefixRareInfo()->DesParameter[i] == pRefItemInfo->GetSuffixRareInfo()->DesParameter[j])
							{
								tempCmp += pRefItemInfo->GetSuffixRareInfo()->ParameterValue[j];
							}
						}
					}
					SetFunction( (*index)++, pRefItemInfo->GetPrefixRareInfo()->DesParameter[i], 
										pRefItemInfo->GetPrefixRareInfo()->ParameterValue[i] + tempCmp, 0,0);
				}
			}
		}
		if(!tempSufCmp)
		{
			if(pRefItemInfo && pRefItemInfo->GetSuffixRareInfo())
			{	
				if(!pRefItemInfo->GetSuffixRareInfo()->DesParameter[i] == NULL || !pRefItemInfo->GetSuffixRareInfo()->ParameterValue[i] == NULL)
				{
					BOOL tempcmp1 = false; 
					for(int j = 0; j< SIZE_MAX_DESPARAM_COUNT_IN_ITEM; j++)
					{
						if(pRefItemInfo->GetSuffixRareInfo() && pRefItemInfo->GetPrefixRareInfo())
						{
							if(pRefItemInfo->GetSuffixRareInfo()->DesParameter[i] == pRefItemInfo->GetPrefixRareInfo()->DesParameter[j])
							{
								tempcmp1 = true;
								break;
							}
						}						
					}		
					if(!tempcmp1)
					{
						SetFunction( (*index)++, pRefItemInfo->GetSuffixRareInfo()->DesParameter[i], 
							pRefItemInfo->GetSuffixRareInfo()->ParameterValue[i], 0,0);
					}
				}
			}
		}
	}
}
//end 2010. 04. 21 by jskim Ω≈±‘ ∑∞≈∞ ∏”Ω≈ ±∏«ˆ

///////////////////////////////////////////////////////////////////////////////
/// \fn			void CINFItemInfo::SetSkillSupportItem(BOOL bShop)
/// \brief		Ω∫≈≥ ∫∏¡∂ æ∆¿Ã≈€
/// \author		ispark
/// \date		2006-10-02 ~ 2006-10-02
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CINFItemInfo::SetSkillSupportItem(BOOL bShop)
{
	int index = 0;
	SetItemName( index++ );
	SetItemKind( index++ );
	SetReqLevel( index++ );


	// 2009. 10. 28 by ckPark Ω√∞£/±‚∞£ ¡¶«— æ∆¿Ã≈€ ≈¯∆¡ ºˆ¡§
//	SetItemAllTime(index++);			// 2006-10-11 by ispark
// 	// 2009. 09. 15 by ckPark Ω√∞£¡¶«— æ∆¿Ã≈€ ≈¯∆¡ πˆ±◊ ºˆ¡§
// 	
// 	//SetItemDelRemainTime(index++);
// 	if( m_pRefItemInfo )
// 		SetItemDelRemainTime(index++);
// 	
// 	// end 2009. 09. 15 by ckPark Ω√∞£¡¶«— æ∆¿Ã≈€ ≈¯∆¡ πˆ±◊ ºˆ¡§

	if( !SetRemainTime_Imp( m_pRefItemInfo, &index, bShop ) )
	{
		if( m_pRefItemInfo )
			SetItemAllTime(index++);
	}
	// end 2009. 10. 28 by ckPark Ω√∞£/±‚∞£ ¡¶«— æ∆¿Ã≈€ ≈¯∆¡ ºˆ¡§
	
	
	// 2007-09-07 by bhsohn æ∆¿Ã≈€ ∞≈∑°ø©∫Œ «•Ω√
	if(FALSE == bShop )
	{
		SetItemAttribute(index++);
	}

	// 2009. 11. 02 by ckPark ¿Œ««¥œ∆º « µÂ ¿ŒΩ∫≈œΩ∫ ¥¯¡Ø Ω√Ω∫≈€
	SetUseAreaInfinity( &index );
	SetExchangeMaterial( bShop );
	// end 2009. 11. 02 by ckPark ¿Œ««¥œ∆º « µÂ ¿ŒΩ∫≈œΩ∫ ¥¯¡Ø Ω√Ω∫≈€

	SetDesc( index );
	SetItemExtendInfo(TRUE);
	ASSERT_ASSERT(index <= ITEMINFO_PARAMETER_NUMBER);
	SetMaxLength();
}


// 2010-06-15 by shcho&hslee ∆ÍΩ√Ω∫≈€
/********************************************************************************
**
**	∆Í æ∆¿Ã≈€ ≈¯∆¡ ¡§∫∏ º≥¡§.
**
**	Create Info :	2010. 06. 24 by hsLee.
**
*********************************************************************************/
void CINFItemInfo :: SetPetItemInfo ( BOOL bShop /*= FALSE*/ )
{
	int iIndex = 0;

	SetItemName ( iIndex++ );
	SetItemKind ( iIndex++ );

#ifndef	SC_PARTNER_SHAPE_CHANGE_HSKIM
	if (m_pRefItemInfo)
	{
		SetShapeInfo(iIndex++);
	}
#endif

	SetReqLevel ( iIndex++ );
	SetPetType ( iIndex++ );
	
	SetPetEnableLevelUp ( iIndex++ );
	
	if( !bShop )
	{
		//SetPetReName ( iIndex++ );		
		SetPetExp ( iIndex++ ); 
	}

	// 2010-12-21 by jskim, ∏∂¿ª ¿Ãµø º”µµ ¡ı∞° æ∆¿Ã≈€ ±∏«ˆ
	if( m_pRefITEM
		&& !m_pRefITEM->IsExistDesParam( DES_FIELD_STORE )
		&& !m_pRefITEM->IsExistDesParam( DES_INCREASE_INVENTORY_SPACE )
		&& !m_pRefITEM->IsExistDesParam( DES_INCREASE_STORE_SPACE ) )		
	{
		SetParameter(&iIndex);
	}
	// end 2010-12-21 by jskim, ∏∂¿ª ¿Ãµø º”µµ ¡ı∞° æ∆¿Ã≈€ ±∏«ˆ
	
	if ( FALSE == bShop )
		SetItemAttribute( iIndex++ );

	SetUseAreaInfinity( &iIndex );

	SetDesc ( iIndex );
	SetItemExtendInfo(TRUE);
	ASSERT_ASSERT ( iIndex <= ITEMINFO_PARAMETER_NUMBER );
	SetMaxLength();


}
// End 2010-06-15 by shcho&hslee ∆ÍΩ√Ω∫≈€


///////////////////////////////////////////////////////////////////////////////
/// \fn			void CINFItemInfo::SetOtherFuntionTick()
/// \brief		∫∞µµ¿« «‘ºˆ∞° Tick¿ª µπ∞‘ √≥∏Æ
/// \author		ispark
/// \date		2006-10-12 ~ 2006-10-12
/// \warning	
///
/// \param		FALSE¥¬ π´¡∂∞« æ¥¥Ÿ.
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CINFItemInfo::SetOtherFuntionTick()
{
	vector<stTickFuntionIndex>::iterator itFuntionIndex = m_vecTickFuntionIndex.begin();

	while(m_vecTickFuntionIndex.end() != itFuntionIndex)
	{
		switch(itFuntionIndex->nFuntionIndex)
		{
		case FUNCTION_INDEX_ITEM_REMAIN_TIME:
			{
				SetItemRemainTime(itFuntionIndex->nDataLineIndex,FALSE, itFuntionIndex->nParam1 ,FALSE);
			}
			break;
		case FUNCTION_INDEX_ITEM_DEL_REMAIN_TIME:
			{
				SetItemDelRemainTime(itFuntionIndex->nDataLineIndex, FALSE);
			}
			break;
		// 2010. 02. 11 by ckPark πﬂµø∑˘ ¿Â¬¯æ∆¿Ã≈€
		case FUNCTION_INDEX_ITEM_COOL_TIME:
			{
				SetItemCoolTime( &((*itFuntionIndex).nDataLineIndex), FALSE );
			}
			break;
		// end 2010. 02. 11 by ckPark πﬂµø∑˘ ¿Â¬¯æ∆¿Ã≈€
		}

		itFuntionIndex++;
	}
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			void CINFItemInfo::SetItemAttribute( int nParameterIndex)
/// \brief		
/// \author		// 2007-09-07 by bhsohn æ∆¿Ã≈€ ∞≈∑°ø©∫Œ «•Ω√
/// \date		2007-09-07 ~ 2007-09-07
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
//inet - changed item attribute description for items
#define STR_UPGR  "Upgrade[%s]"
#define STR_ENCH  "Ench[%s]"
#define STR_TRADE "Trade[%s]"
#define STR_SELL  "Sell[%s]"
#define STR_TRASH "Del[%s]"
#define STR_WH	  "WH[%s]"
#define STR_WH_P  "Share WH[%s]"
#define STR_WH_G  "Brig WH[%s]"
//end of inet
void CINFItemInfo::SetItemAttribute( int nParameterIndex)
{
#ifdef _INET_CHANGED_ATTR_DESC
	char *szY = "\\gY\\g";
	char *szN = "\\rN\\r";
	char szEnch[32], szTrade[32], szSell[32], szTrash[32], szWH[32], szWHP[32], szWHG[32];
	char szTemp[128];

	if (NULL == m_pRefItemInfo)
	{
		sprintf(m_strItemInfo[nParameterIndex], "\n");
		return;
	}

	BitFlag64_t bfCurrentBitFlag = m_pRefItemInfo->GetRealItemInfo()->ItemAttribute;
	BYTE byCurrentKind = m_pRefItemInfo->GetRealItemInfo()->Kind;
	int nShowEnchant = 0;

	sprintf(szEnch, STR_ENCH, szY);
	sprintf(szTrade, STR_TRADE, szY);
	sprintf(szSell, STR_SELL, szY);
	sprintf(szTrash, STR_TRASH, szY);
	sprintf(szWH, STR_WH, szY);
	sprintf(szWHP, STR_WH_P, szY);
	sprintf(szWHG, STR_WH_G, szY);

	//check if is pet item
	if (byCurrentKind == ITEMKIND_PET_ITEM)
	{
		sprintf(szEnch, STR_UPGR, szY);
	}
	//check if it can be enchanted
	if (!IS_COUNTABLE_ITEM(byCurrentKind) && TRUE == IS_ENCHANT_TARGET_ITEMKIND(byCurrentKind))
	{
		if (COMPARE_BIT_FLAG(bfCurrentBitFlag, ITEM_ATTR_UNIQUE_ITEM | ITEM_ATTR_LEGEND_ITEM))
		{
			nShowEnchant = 1;
			sprintf(szEnch, STR_ENCH, szN);
		}
		else
		{
			nShowEnchant = 1;
		}
	}
	//check if it can be traded behind players
	if ((COMPARE_BIT_FLAG(m_pRefItemInfo->GetRealItemInfo()->ItemAttribute, ITEM_ATTR_KILL_MARK_ITEM))
		|| (COMPARE_BIT_FLAG(m_pRefItemInfo->GetRealItemInfo()->ItemAttribute, ITEM_ATTR_ACCOUNT_POSSESSION)))
	{
		sprintf(szTrade, STR_TRADE, szN);
		sprintf(szWHG, STR_WH_G, szN);
	}
	//check if it can be placed in personal store
	if (!COMPARE_BIT_FLAG(bfCurrentBitFlag, ITEM_ATTR_BAZAAR_ITEM))
	{
		sprintf(szSell, STR_SELL, szN);
	}
	//no transfer - no wh, no delete, no trade
	if (COMPARE_BIT_FLAG(bfCurrentBitFlag, ITEM_ATTR_NO_TRANSFER))
	{
		nShowEnchant = 0;
		sprintf(szTrade, STR_TRADE, szN);
		sprintf(szTrash, STR_TRASH, szN);
		sprintf(szWH, STR_WH, szN);
		sprintf(szWHP, STR_WH_P, szN);
		sprintf(szWHG, STR_WH_G, szN);
	}
	//wh behind chars disabled
	//ep4 content
	/*if (COMPARE_BIT_FLAG(bfCurrentBitFlag, ITEM_ATTR_WAREHOUSE_SHARE_BANNED))
	{
		sprintf(szWHP, STR_WH_P, szN);
		sprintf(szWHG, STR_WH_G, szN);
	}*/
	// wh disabled
	if (COMPARE_BIT_FLAG(bfCurrentBitFlag, ITEM_ATTR_NOT_STORE_SAVE))
	{
		sprintf(szWH, STR_WH, szN);
		sprintf(szWHP, STR_WH_P, szN);
		sprintf(szWHG, STR_WH_G, szN);
	}
	//else default vals are yes so print this shit

	if (nShowEnchant == 1)
	{
		sprintf(szTemp, "%s %s %s %s %s %s %s", szEnch, szTrade, szSell, szTrash, szWH, szWHP, szWHG);
		sprintf(m_strItemInfo[nParameterIndex], "%s", szTemp);
	}
	else
	{
		sprintf(szTemp, "%s %s %s %s %s %s", szTrade, szSell, szTrash, szWH, szWHP, szWHG);
		sprintf(m_strItemInfo[nParameterIndex], "%s", szTemp);
	}

#else
	if (NULL == m_pRefItemInfo)
	{
		sprintf(m_strItemInfo[nParameterIndex], STRMSG_C_070907_0204, STRMSG_C_070907_0203);//"∞≈∑°∞°¥…, ªË¡¶∞°¥…, √¢∞Ì∞°¥…"
		return;
	}

	// 2011-06-07 by shcho, ∫£∆Æ≥≤ ø‰√ª √¢∞Ìø°∏∏ ¿˙¿Â ∫“∞° º”º∫ ±∏«ˆ
	if (COMPARE_BIT_FLAG(m_pRefItemInfo->ItemInfo->ItemAttribute, ITEM_ATTR_NOT_STORE_SAVE))
	{
		sprintf(m_strItemInfo[nParameterIndex], STRMSG_C_070907_0204, STRMSG_C_110608_0001);// "\\r∞≈∑°∫“∞°\\r, \\rªË¡¶∫“∞°\\r, \\r√¢∞Ì∫“∞°\\r"
	}
	else if (COMPARE_BIT_FLAG(m_pRefItemInfo->ItemInfo->ItemAttribute, ITEM_ATTR_NO_TRANSFER))
		// end 2011-06-07 by shcho, ∫£∆Æ≥≤ ø‰√ª √¢∞Ìø°∏∏ ¿˙¿Â ∫“∞° º”º∫ ±∏«ˆ
	{
		sprintf(m_strItemInfo[nParameterIndex], STRMSG_C_070907_0204, STRMSG_C_070907_0201);// "\\r∞≈∑°∫“∞°\\r, \\rªË¡¶∫“∞°\\r, \\r√¢∞Ì∫“∞°\\r"
	}
	// 2007.09.19 by bhsohn æ∆¿Ã≈€¡§∫∏ «•Ω√ √ﬂ∞° ±‚»πæ» √≥∏Æ
	else if ((COMPARE_BIT_FLAG(m_pRefItemInfo->ItemInfo->ItemAttribute, ITEM_ATTR_KILL_MARK_ITEM))
		|| (COMPARE_BIT_FLAG(m_pRefItemInfo->ItemInfo->ItemAttribute, ITEM_ATTR_ACCOUNT_POSSESSION)))
	{
		sprintf(m_strItemInfo[nParameterIndex], STRMSG_C_070907_0204, STRMSG_C_070907_0202);//"\\r∞≈∑°∫“∞°\\r, ªË¡¶∞°¥…, √¢∞Ì∞°¥…"
	}
	else
	{
		sprintf(m_strItemInfo[nParameterIndex], STRMSG_C_070907_0204, STRMSG_C_070907_0203);//"∞≈∑°∞°¥…, ªË¡¶∞°¥…, √¢∞Ì∞°¥…"
	}
#endif
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		
/// \author		// 2008-04-14 by bhsohn ¿Ø∑¥ æ∆¿Ã≈€ º≥∏Ì Ω∫∆Æ∏µπÆ¡¶ √≥∏Æ
/// \date		2008-04-14 ~ 2008-04-14
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
int CINFItemInfo::GetItemStringLen()
{
	int i =0;
	int nMaxLen,nLen;
	nMaxLen = nLen = 0;
	//for(int i=0;i<ITEMINFO_PARAMETER_NUMBER;i++)
	for(i=1;i<m_nDescIndex;i++)
	{
		if(NULL == m_strItemInfo[i][0])
		{
			continue;
		}
		nLen = m_pFontItemInfo[i]->GetStringSize(m_strItemInfo[i]).cx;
		if(nMaxLen < nLen) 
		{
			nMaxLen = nLen; 
		}
	}
	return nMaxLen;
}



///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		// ««æÓΩ∫ ∑¸
/// \author		// 2008-09-26 by bhsohn Ω≈±‘ ¿Œ√æ∆Æ √≥∏Æ
/// \date		2008-09-26 ~ 2008-09-26
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CINFItemInfo::SetPrimaryPierce(int nParameterIndex)
{	
	char chTmp[ITEMINFO_ITEM_FULL_NAME];
	memset(chTmp, 0x00, ITEMINFO_ITEM_FULL_NAME);
	
	float fWeaponPierce = 0.0f;
	if(NULL !=  m_pRefItemInfo)
	{	
		ITEM* pRealItem = m_pRefItemInfo->GetRealItemInfo();
		if(pRealItem)
		{
			fWeaponPierce = pRealItem->FractionResistance;		
		}
		
	}
	if(fWeaponPierce > 0)
	{
		// 2010-07-28 by dgwoo »Æ∑¸ ºˆΩƒ ∫Ø∞Ê (255 => 100%)
		//fWeaponPierce = ((float)fWeaponPierce / (float)PROB256_MAX_VALUE *100.0f);
		fWeaponPierce = ((float)fWeaponPierce / (float)PROB100_MAX_VALUE *100.0f);
		// 2008-03-19 by bhsohn FLOAT«¸ ¿Á¡§∑ƒ «œø© ªÁøÎ	
		fWeaponPierce = FloatSecRangeSharp(fWeaponPierce);
	}
	else
	{
		fWeaponPierce =0.0f;
	}
	FLOAT fEhchantPierce = m_pRefEnchant ? (m_pRefEnchant->pfm_PIERCE_UP_01): 0;	
	
	sprintf( m_strItemInfo[nParameterIndex], STRMSG_C_070116_0100, fWeaponPierce) ;//"««æÓΩ∫¿≤ : %.2f%%"	

	// 2009-02-16 by bhsohn ¡¢µŒ, ¡¢π´ ««æÓΩ∫ ¿≤ æ»∫∏¿Ã¥¬ «ˆªÛ √≥∏Æ
	float fPreSuffixPierce = GetEnchantPreSuffixInfo(DES_PIERCE_UP_01);		
	if(fPreSuffixPierce > 0)
	{
		// 2010-07-28 by dgwoo »Æ∑¸ ºˆΩƒ ∫Ø∞Ê (255 => 100%)
		//float fPreSuffixPierceRate = ((float)fPreSuffixPierce / (float)PROB256_MAX_VALUE *100.0f);		
		float fPreSuffixPierceRate = ((float)fPreSuffixPierce / (float)PROB100_MAX_VALUE *100.0f);		
		fPreSuffixPierceRate = FloatSecRangeSharp(fPreSuffixPierceRate);
		
		sprintf(chTmp, STRMSG_C_080923_0200, fPreSuffixPierceRate);
		sprintf( m_strItemInfo[nParameterIndex], "%s\\g[+%s]\\g", m_strItemInfo[nParameterIndex], chTmp) ;//"««æÓΩ∫¿≤ : %.2f%%"			
	}
	// end 2009-02-16 by bhsohn ¡¢µŒ, ¡¢π´ ««æÓΩ∫ ¿≤ æ»∫∏¿Ã¥¬ «ˆªÛ √≥∏Æ

	if(fEhchantPierce > 0)
	{
		// 2010-07-28 by dgwoo »Æ∑¸ ºˆΩƒ ∫Ø∞Ê (255 => 100%)
		//float fEhchantPierceRate = ((float)fEhchantPierce / (float)PROB256_MAX_VALUE *100.0f);
		float fEhchantPierceRate = ((float)fEhchantPierce / (float)PROB100_MAX_VALUE *100.0f);
		// 2008-03-19 by bhsohn FLOAT«¸ ¿Á¡§∑ƒ «œø© ªÁøÎ			
		fEhchantPierceRate = FloatSecRangeSharp(fEhchantPierceRate);

		sprintf(chTmp, STRMSG_C_080923_0200, fEhchantPierceRate);
		sprintf(m_strItemInfo[nParameterIndex], "%s\\e[+%s]\\e", m_strItemInfo[nParameterIndex], chTmp) ;//"««æÓΩ∫¿≤ : %.2f%%"	
	}

	
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		// ««æÓΩ∫ ∑¸
/// \author		// 2008-09-26 by bhsohn Ω≈±‘ ¿Œ√æ∆Æ √≥∏Æ
/// \date		2008-09-26 ~ 2008-09-26
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CINFItemInfo::SetSecondaryPierce(int nParameterIndex)
{	
	char chTmp[ITEMINFO_ITEM_FULL_NAME];
	memset(chTmp, 0x00, ITEMINFO_ITEM_FULL_NAME);

	float fWeaponPierce = 0.0f;
	if(NULL !=  m_pRefItemInfo)
	{	
		ITEM* pRealItem = m_pRefItemInfo->GetRealItemInfo();
		if(pRealItem)
		{
			fWeaponPierce = pRealItem->FractionResistance;		
		}
		
	}	
	if(fWeaponPierce > 0)
	{
		// 2010-07-28 by dgwoo »Æ∑¸ ºˆΩƒ ∫Ø∞Ê (255 => 100%)
		//fWeaponPierce = ((float)fWeaponPierce / (float)PROB256_MAX_VALUE *100.0f);
		fWeaponPierce = ((float)fWeaponPierce / (float)PROB100_MAX_VALUE *100.0f);
		// 2008-03-19 by bhsohn FLOAT«¸ ¿Á¡§∑ƒ «œø© ªÁøÎ					
		fWeaponPierce = FloatSecRangeSharp(fWeaponPierce);
	}
	else
	{
		fWeaponPierce =0.0f;
	}
	FLOAT fEhchantPierce = m_pRefEnchant ? (m_pRefEnchant->pfm_PIERCE_UP_02 ): 0;	

	sprintf( m_strItemInfo[nParameterIndex], STRMSG_C_070116_0100, fWeaponPierce) ;//"««æÓΩ∫¿≤ : %.2f%%"	

	// 2009-02-16 by bhsohn ¡¢µŒ, ¡¢π´ ««æÓΩ∫ ¿≤ æ»∫∏¿Ã¥¬ «ˆªÛ √≥∏Æ
	float fPreSuffixPierce = GetEnchantPreSuffixInfo(DES_PIERCE_UP_02);		
	if(fPreSuffixPierce > 0)
	{
		// 2010-07-28 by dgwoo »Æ∑¸ ºˆΩƒ ∫Ø∞Ê (255 => 100%)
		//float fPreSuffixPierceRate = ((float)fPreSuffixPierce / (float)PROB256_MAX_VALUE *100.0f);		
		float fPreSuffixPierceRate = ((float)fPreSuffixPierce / (float)PROB100_MAX_VALUE *100.0f);		
		fPreSuffixPierceRate = FloatSecRangeSharp(fPreSuffixPierceRate);
		
		sprintf(chTmp, STRMSG_C_080923_0200, fPreSuffixPierceRate);
		sprintf( m_strItemInfo[nParameterIndex], "%s\\g[+%s]\\g", m_strItemInfo[nParameterIndex], chTmp) ;//"««æÓΩ∫¿≤ : %.2f%%"			
	}
	// end 2009-02-16 by bhsohn ¡¢µŒ, ¡¢π´ ««æÓΩ∫ ¿≤ æ»∫∏¿Ã¥¬ «ˆªÛ √≥∏Æ

	
	if(fEhchantPierce > 0)
	{
		// 2010-07-28 by dgwoo »Æ∑¸ ºˆΩƒ ∫Ø∞Ê (255 => 100%)
		//float fEhchantPierceRate = ((float)fEhchantPierce / (float)PROB256_MAX_VALUE *100.0f);
		float fEhchantPierceRate = ((float)fEhchantPierce / (float)PROB100_MAX_VALUE *100.0f);
		// 2008-03-19 by bhsohn FLOAT«¸ ¿Á¡§∑ƒ «œø© ªÁøÎ							
		fEhchantPierceRate = FloatSecRangeSharp(fEhchantPierceRate);
		
		sprintf(chTmp, STRMSG_C_080923_0200, fEhchantPierceRate);
		sprintf( m_strItemInfo[nParameterIndex], "%s\\e[+%s]\\e", m_strItemInfo[nParameterIndex], chTmp) ;//"««æÓΩ∫¿≤ : %.2f%%"			
	}
	
}


///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		// ««æÓΩ∫ ∑¸
/// \author		// 2008-09-26 by bhsohn Ω≈±‘ ¿Œ√æ∆Æ √≥∏Æ
/// \date		2008-09-26 ~ 2008-09-26
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
float CINFItemInfo::GetEnchantPreSuffixInfo(int nDesParameter)
{
	float fRareInfoReAttacktime = 0;
	float fEnchantInfo = 0.0f;
	if(m_pRefPrefixRareInfo)
	{
		for(int i=0; i<SIZE_DES_PARAM_PER_RARE_ITEM_INFO; i++)
		{
			if( m_pRefPrefixRareInfo->DesParameter[i] == nDesParameter)
			{
				fEnchantInfo += m_pRefPrefixRareInfo->ParameterValue[i];
			}
		}
	}
	if(m_pRefSuffixRareInfo)
	{
		for(int i=0; i<SIZE_DES_PARAM_PER_RARE_ITEM_INFO; i++)
		{
			if( m_pRefSuffixRareInfo->DesParameter[i] == nDesParameter)
			{
				fEnchantInfo += m_pRefSuffixRareInfo->ParameterValue[i];
			}
		}
	}
	return fEnchantInfo;
}

// 2009. 10. 28 by ckPark Ω√∞£/±‚∞£ ¡¶«— æ∆¿Ã≈€ ≈¯∆¡ ºˆ¡§
BOOL	CINFItemInfo::SetRemainTime_Imp( CItemInfo* pRefItem, int* pIndex, BOOL bShop )
{
	BOOL ret = FALSE;
	
	if(pRefItem && pRefItem->ItemInfo)
	{
		if(COMPARE_BIT_FLAG(pRefItem->ItemInfo->ItemAttribute, ITEM_ATTR_DELETED_TIME_LIMITE_AFTER_USED))
		{	// ªÁøÎ »ƒ ±‚∞£¡¶«— æ∆¿Ã≈€
			SetItemRemainTime((*pIndex)++,bShop);		
			ret = TRUE;
		}
		else if(COMPARE_BIT_FLAG(pRefItem->ItemInfo->ItemAttribute, ITEM_ATTR_TIME_LIMITE)
			|| pRefItem->Kind == ITEMKIND_ACCESSORY_TIMELIMIT )
		{	// ªÁøÎ »ƒ Ω√∞£¡¶«— æ∆¿Ã≈€
			SetItemRemainTime((*pIndex)++,bShop);		
			ret = TRUE;
		}
		
		// ª˝º∫ »ƒ ±‚∞£¡¶«— æ∆¿Ã≈€
		if(COMPARE_BIT_FLAG(pRefItem->ItemInfo->ItemAttribute, ITEM_ATTR_DELETED_TIME_LIMITE))
		{
			SetItemDelRemainTime((*pIndex)++);
			ret = TRUE;
		}
	}
	return FALSE;
}
// end 2009. 10. 28 by ckPark Ω√∞£/±‚∞£ ¡¶«— æ∆¿Ã≈€ ≈¯∆¡ ºˆ¡§

///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		
/// \author		// 2009-02-03 by bhsohn ¿Â¬¯ æ∆¿Ã≈€ ∫Ò±≥ ≈¯∆¡
/// \date		2009-02-03 ~ 2009-02-03
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
int	CINFItemInfo::GetMaxLength()
{
	return m_nMaxLength;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		
/// \author		// 2009-02-03 by bhsohn ¿Â¬¯ æ∆¿Ã≈€ ∫Ò±≥ ≈¯∆¡
/// \date		2009-02-03 ~ 2009-02-03
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CINFItemInfo::SetMyEquipItem(BOOL bMyEquipItem)
{
	m_bMyEquipItem = bMyEquipItem;
}
///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		
/// \author		// 2009-02-03 by bhsohn ¿Â¬¯ æ∆¿Ã≈€ ∫Ò±≥ ≈¯∆¡
/// \date		2009-02-03 ~ 2009-02-03
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
CItemInfo*	CINFItemInfo::GetRefItemInfo()
{
	return m_pRefItemInfo;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		
/// \author		// 2009-02-03 by bhsohn ¿Â¬¯ æ∆¿Ã≈€ ∫Ò±≥ ≈¯∆¡
/// \date		2009-02-03 ~ 2009-02-03
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
POINT CINFItemInfo::GetItemInfoPos()
{
	POINT ptItemInfoPos = m_ptItemInfo;
	int icongab;
	if (m_nBigIconNum > 0)
	{
		icongab = ITEMINFO_BIGICON_GAB;
	}
	else
	{
		icongab = 0;
	}
	ptItemInfoPos.y -= icongab;

	return ptItemInfoPos;
}
///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		
/// \author		// 2009-02-03 by bhsohn ¿Â¬¯ æ∆¿Ã≈€ ∫Ò±≥ ≈¯∆¡
/// \date		2009-02-03 ~ 2009-02-03
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CINFItemInfo::SetItemInfoPos(POINT i_ptItemInfoPos)
{
	int icongab;
	if (m_nBigIconNum > 0)
	{
		icongab = ITEMINFO_BIGICON_GAB;
	}
	else
	{
		icongab = 0;
	}
	i_ptItemInfoPos.y += icongab;

	m_ptItemInfo = i_ptItemInfoPos;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		
/// \author		// 2009-02-03 by bhsohn ¿Â¬¯ æ∆¿Ã≈€ ∫Ò±≥ ≈¯∆¡
/// \date		2009-02-03 ~ 2009-02-03
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
SIZE CINFItemInfo::GetItemInfoTooltipSize()
{
	return m_szTooltip;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		
/// \author		// 2009-02-03 by bhsohn ¿Â¬¯ æ∆¿Ã≈€ ∫Ò±≥ ≈¯∆¡
/// \date		2009-02-03 ~ 2009-02-03
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CINFItemInfo::SetItemInfoTooltipSize(SIZE	i_szTooltip)
{
	m_szTooltip  = i_szTooltip;
}
///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		
/// \author		// 2009-02-03 by bhsohn ¿Â¬¯ æ∆¿Ã≈€ ∫Ò±≥ ≈¯∆¡
/// \date		2009-02-03 ~ 2009-02-03
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CINFItemInfo::IsShowItemInfo()
{
	return m_bShow;
}