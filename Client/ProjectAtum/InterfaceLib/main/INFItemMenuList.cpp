// INFItemMenuList.cpp: implementation of the CINFItemMenuList class.
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

#include "INFWorldRankWnd.h"
#include "INFImageEx.h"									   // 2011. 10. 10 by jskim UI시스템 변경

#include "AtumDatabase.h"


#include "INFItemMenuList.h"

#include "ShuttleChild.h"
#include "IMSocketManager.h"
#include "Chat.h"

#include "INFInven.h"
#include "shellapi.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CINFItemMenuList::CINFItemMenuList()
{
	m_pItemMixInfoBtn = NULL;		// 조합정보
	m_pArmorCollectionBtn = NULL;	// 아머 컬렉션 버튼 // 2013-05-28 by bhsohn 아머 컬렉션 시스템
	m_pPreview3D = NULL;
#ifdef _INET_LINK_ITEMS
	m_pLinkBtn = NULL;
#endif
#ifdef _INET_RIGHTCLICK_PEDIA
	m_pPediaBtn = NULL;
#endif
	m_nShowItemNum = 0;
	m_uItemUniNum = 0;

}

CINFItemMenuList::~CINFItemMenuList()
{
	SAFE_DELETE(m_pItemMixInfoBtn);		// 조합정보
	SAFE_DELETE(m_pArmorCollectionBtn);	// 아머 컬렉션 버튼 // 2013-05-28 by bhsohn 아머 컬렉션 시스템
	SAFE_DELETE(m_pPreview3D);
#ifdef _INET_LINK_ITEMS
	SAFE_DELETE(m_pLinkBtn);
#endif
#ifdef _INET_RIGHTCLICK_PEDIA
	SAFE_DELETE(m_pPediaBtn);
#endif
}

HRESULT CINFItemMenuList::InitDeviceObjects()
{
	char szUpBtn[30], szDownBtn[30], szSelBtn[30], szDisBtn[30];
	{
		wsprintf(szUpBtn, "reci_view3");
		wsprintf(szDownBtn, "reci_view1");
		wsprintf(szSelBtn, "reci_view0");
		wsprintf(szDisBtn, "reci_view2");
		
		if(NULL == m_pItemMixInfoBtn)
		{
			m_pItemMixInfoBtn = new CINFImageBtn;
			m_pItemMixInfoBtn->InitDeviceObjects(szUpBtn, szDownBtn, szSelBtn, szDisBtn);
		}			
	}
	// 2013-05-28 by bhsohn 아머 컬렉션 시스템
	{
		wsprintf(szUpBtn, "coll_view3");
		wsprintf(szDownBtn, "coll_view1");
		wsprintf(szSelBtn, "coll_view0");
		wsprintf(szDisBtn, "coll_view2");
		
		if(NULL == m_pArmorCollectionBtn)
		{
			m_pArmorCollectionBtn = new CINFImageBtn;
			m_pArmorCollectionBtn->InitDeviceObjects(szUpBtn, szDownBtn, szSelBtn, szDisBtn);
		}			
	}
	{
		wsprintf(szUpBtn, "coll_view3");
		wsprintf(szDownBtn, "coll_view1");
		wsprintf(szSelBtn, "coll_view0");
		wsprintf(szDisBtn, "coll_view2");
		
		if(NULL == m_pPreview3D)
		{
			m_pPreview3D = new CINFImageBtn;
			m_pPreview3D->InitDeviceObjects(szUpBtn, szDownBtn, szSelBtn, szDisBtn);
		}			
	}
	// END 2013-05-28 by bhsohn 아머 컬렉션 시스템
#ifdef _INET_LINK_ITEMS
	{
		wsprintf(szUpBtn, "link_btn3");
		wsprintf(szDownBtn, "link_btn1");
		wsprintf(szSelBtn, "link_btn0");
		wsprintf(szDisBtn, "link_btn2");

		if (NULL == m_pLinkBtn)
		{
			m_pLinkBtn = new CINFImageBtn;
			m_pLinkBtn->InitDeviceObjects(szUpBtn, szDownBtn, szSelBtn, szDisBtn);
		}
	}
#endif
#ifdef _INET_RIGHTCLICK_PEDIA
	{
		wsprintf(szUpBtn, "pedia_but3");
		wsprintf(szDownBtn, "pedia_but1");
		wsprintf(szSelBtn, "pedia_but0");
		wsprintf(szDisBtn, "pedia_but2");

		if (NULL == m_pPediaBtn)
		{
			m_pPediaBtn = new CINFImageBtn;
			m_pPediaBtn->InitDeviceObjects(szUpBtn, szDownBtn, szSelBtn, szDisBtn);
		}
	}
#endif
	return S_OK ;
}
HRESULT CINFItemMenuList::RestoreDeviceObjects()
{
	int nWidth = 0;
	int nHeight = 0;
	if(m_pItemMixInfoBtn)
	{
		m_pItemMixInfoBtn->RestoreDeviceObjects();	
		nWidth += m_pItemMixInfoBtn->GetImgSize().x;
		nHeight += m_pItemMixInfoBtn->GetImgSize().y;
	}
	// 2013-05-28 by bhsohn 아머 컬렉션 시스템
	if(m_pArmorCollectionBtn)
	{
		m_pArmorCollectionBtn->RestoreDeviceObjects();
		nHeight += m_pArmorCollectionBtn->GetImgSize().y;
	}	
	if(m_pPreview3D)
	{
		m_pPreview3D->RestoreDeviceObjects();
		nHeight += m_pPreview3D->GetImgSize().y;
	}	
#ifdef _INET_LINK_ITEMS
	if (m_pLinkBtn)
	{
		m_pLinkBtn->RestoreDeviceObjects();
		nHeight += m_pLinkBtn->GetImgSize().y;
	}
#endif
#ifdef _INET_RIGHTCLICK_PEDIA
	if (m_pPediaBtn)
	{
		m_pPediaBtn->RestoreDeviceObjects();
		nHeight += m_pPediaBtn->GetImgSize().y;
	}
#endif
	SetSize(nWidth, nHeight);
	// END 2013-05-28 by bhsohn 아머 컬렉션 시스템

	return S_OK ;
}

HRESULT CINFItemMenuList::DeleteDeviceObjects()
{
	if(m_pItemMixInfoBtn)
	{
		m_pItemMixInfoBtn->DeleteDeviceObjects();
		SAFE_DELETE(m_pItemMixInfoBtn);
	}
	// 2013-05-28 by bhsohn 아머 컬렉션 시스템
	if(m_pArmorCollectionBtn)
	{
		m_pArmorCollectionBtn->DeleteDeviceObjects();
		SAFE_DELETE(m_pArmorCollectionBtn);
	}
	if(m_pPreview3D)
	{
		m_pPreview3D->DeleteDeviceObjects();
		SAFE_DELETE(m_pPreview3D);
	}
	// END 2013-05-28 by bhsohn 아머 컬렉션 시스템
#ifdef _INET_LINK_ITEMS
	if (m_pLinkBtn)
	{
		m_pLinkBtn->DeleteDeviceObjects();
		SAFE_DELETE(m_pLinkBtn);
	}
#endif
#ifdef _INET_RIGHTCLICK_PEDIA
	if (m_pPediaBtn)
	{
		m_pPediaBtn->DeleteDeviceObjects();
		SAFE_DELETE(m_pPediaBtn);
	}
#endif
	return S_OK ;
}

HRESULT CINFItemMenuList::InvalidateDeviceObjects()
{
	if(m_pItemMixInfoBtn)
	{
		m_pItemMixInfoBtn->InvalidateDeviceObjects();		
	}
	// 2013-05-28 by bhsohn 아머 컬렉션 시스템
	if(m_pArmorCollectionBtn)
	{
		m_pArmorCollectionBtn->InvalidateDeviceObjects();		
	}
	if(m_pPreview3D)
	{
		m_pPreview3D->InvalidateDeviceObjects();
	}
	// END 2013-05-28 by bhsohn 아머 컬렉션 시스템
#ifdef _INET_LINK_ITEMS
	if (m_pLinkBtn)
	{
		m_pLinkBtn->InvalidateDeviceObjects();
	}
#endif
#ifdef _INET_RIGHTCLICK_PEDIA
	if (m_pPediaBtn)
	{
		m_pPediaBtn->InvalidateDeviceObjects();
	}
#endif
	return S_OK ;
}

void CINFItemMenuList::Render()
{
	if(!IsShowWnd())
	{
		return;
	}

	if(m_pItemMixInfoBtn)
	{
		m_pItemMixInfoBtn->Render();		
	}
	// 2013-05-28 by bhsohn 아머 컬렉션 시스템
	if(m_pArmorCollectionBtn)
	{
		m_pArmorCollectionBtn->Render();		
	}
	if (m_pPreview3D)
	{
		//m_pPreview3D->Render(); //disable not used
	}
	// END 2013-05-28 by bhsohn 아머 컬렉션 시스템
#ifdef _INET_LINK_ITEMS
	if (m_pLinkBtn)
	{
		m_pLinkBtn->Render();
	}
#endif
#ifdef _INET_RIGHTCLICK_PEDIA
	if (m_pPediaBtn)
	{
		m_pPediaBtn->Render();
	}
#endif
	CINFDefaultWnd::Render();
}
void CINFItemMenuList::Tick()
{
	if(!IsShowWnd())
	{
		return;
	}

	CINFDefaultWnd::Tick();

}

void CINFItemMenuList::ShowWnd(BOOL bShowWnd, INT nShowItemNum, UID64_t uItemUniNum, POINT *ptPos/*=NULL*/, int nWndWidth/*=0*/)
{
	m_nShowItemNum = nShowItemNum;
	m_uItemUniNum = uItemUniNum;

	CINFDefaultWnd::ShowWnd(bShowWnd, ptPos, nWndWidth);	

	if(bShowWnd)
	{
		UpdateBtnPos(0, 0);		// 버튼 위치 갱신		
	}
}

int CINFItemMenuList::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(!IsShowWnd())
	{
		return INF_MSGPROC_NORMAL;
	}
	switch(uMsg)
	{
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
	case WM_MOUSEMOVE:
		{
			return OnMouseMove(uMsg, wParam, lParam);
		}
		break;	
	}
	
	return INF_MSGPROC_NORMAL;

}
void CINFItemMenuList::UpdateBtnPos(int nWidth, int nHeight)
{
	int nPosY = m_ptBkPos.y;

	BOOL bShowMixInfo, bShowArmorCollect;
	bShowMixInfo = bShowArmorCollect = FALSE;;
	bShowMixInfo = TRUE;
	bShowArmorCollect = TRUE;
#ifdef C_INGAME_MIX_ITEM
	bShowMixInfo = TRUE;
#endif

#ifdef SC_COLLECTION_ARMOR_JHSEOL_BCKIM
	bShowArmorCollect = FALSE;
	ITEM* pItem = g_pDatabase->GetItemInfoLoadItemData(m_nShowItemNum);
	if(pItem && (ITEMKIND_DEFENSE == pItem->Kind))
	{		 
		bShowArmorCollect = TRUE;
	}

	// 아레나에서는 안되게
	if(g_pD3dApp->GetArenaState() == ARENA_STATE_ARENA_GAMING)
	{
		bShowArmorCollect = FALSE;
	}
#endif
	
	if(bShowMixInfo && m_pItemMixInfoBtn)
	{
		m_pItemMixInfoBtn->ShowWindow(TRUE);
		m_pItemMixInfoBtn->SetBtnPosition(m_ptBkPos.x, m_ptBkPos.y);
		nPosY += m_pItemMixInfoBtn->GetImgSize().y;
	}
	else if(m_pItemMixInfoBtn)
	{
		m_pItemMixInfoBtn->ShowWindow(FALSE);
	}
	// 2013-05-28 by bhsohn 아머 컬렉션 시스템
	if(bShowArmorCollect && m_pArmorCollectionBtn)
	{
//		nPosY -= 3;
		m_pArmorCollectionBtn->ShowWindow(TRUE);
		m_pArmorCollectionBtn->SetBtnPosition(m_ptBkPos.x, nPosY );
		nPosY += m_pArmorCollectionBtn->GetImgSize().y;
	}
	else if(m_pArmorCollectionBtn)
	{
		m_pArmorCollectionBtn->ShowWindow(FALSE);
	}
	if (m_pPreview3D)
	{
		m_pPreview3D->ShowWindow(TRUE);
		m_pPreview3D->SetBtnPosition(m_ptBkPos.x, nPosY);
	}
	// END 2013-05-28 by bhsohn 아머 컬렉션 시스템
#ifdef _INET_LINK_ITEMS
	if (m_pLinkBtn)
	{
		m_pLinkBtn->ShowWindow(TRUE);
		m_pLinkBtn->SetBtnPosition(m_ptBkPos.x, nPosY);
#ifdef _INET_RIGHTCLICK_PEDIA
		nPosY += m_pLinkBtn->GetImgSize().y;
#endif
	}
#endif
#ifdef _INET_RIGHTCLICK_PEDIA
	if (m_pPediaBtn)
	{
		m_pPediaBtn->ShowWindow(TRUE);
		m_pPediaBtn->SetBtnPosition(m_ptBkPos.x, nPosY);
	}
	
#endif
	
//	CINFDefaultWnd::UpdateBtnPos(nWidth, nHeight);		
}

int nCountOfSendedIt = 0;

int CINFItemMenuList::OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	POINT pt;
	pt.x = LOWORD(lParam);
	pt.y = HIWORD(lParam);
	CheckMouseReverse(&pt);
	
	{
		if(m_bMove)
		{
			m_bMove = FALSE;
			return INF_MSGPROC_BREAK;
		}		
	}		
	if(m_pItemMixInfoBtn && TRUE == m_pItemMixInfoBtn->OnLButtonUp(pt))
	{	
		if (COMPARE_RACE(g_pD3dApp->m_pShuttleChild->m_myShuttleInfo.Race, RACE_OPERATION) == TRUE) {
			char szTemp[100];
			sprintf(szTemp, "[ID:%I64d]", m_uItemUniNum);
			g_pD3dApp->m_pIMSocket->SendChat(T_IC_CHAT_SELL_ALL, g_pD3dApp->m_pShuttleChild->m_myShuttleInfo.CharacterName, szTemp);
			if(g_pD3dApp->m_pSound)
				g_pD3dApp->m_pSound->PlayD3DSound(SOUND_SELECT_BUTTON, D3DXVECTOR3(0, 0, 0), FALSE);
			ShowWnd(FALSE, 0, 0);

			return  INF_MSGPROC_BREAK;
		}
		else {
			if (nCountOfSendedIt == 0) {
				nCountOfSendedIt++;
				serverTime = GetServerDateTime();
				char szTemp[100];
				sprintf(szTemp, "[ID:%I64d]", m_uItemUniNum);
				g_pD3dApp->m_pIMSocket->SendChat(T_IC_CHAT_SELL_ALL, g_pD3dApp->m_pShuttleChild->m_myShuttleInfo.CharacterName, szTemp);
				if(g_pD3dApp->m_pSound)
					g_pD3dApp->m_pSound->PlayD3DSound(SOUND_SELECT_BUTTON, D3DXVECTOR3(0, 0, 0), FALSE);
				ShowWnd(FALSE, 0, 0);

				return  INF_MSGPROC_BREAK;
			}
			else {
				ATUM_DATE_TIME tempTime = GetServerDateTime();
				tempTime.GetCurrentDateTime();

				if (tempTime.GetTimeDiffTimeInSeconds(serverTime) >= 30) {
					tempTime.Reset();
					serverTime.Reset();
					char szTemp[100];
					sprintf(szTemp, "[ID:%I64d]", m_uItemUniNum);
					g_pD3dApp->m_pIMSocket->SendChat(T_IC_CHAT_SELL_ALL, g_pD3dApp->m_pShuttleChild->m_myShuttleInfo.CharacterName, szTemp);
					serverTime = GetServerDateTime();

					if(g_pD3dApp->m_pSound)
					g_pD3dApp->m_pSound->PlayD3DSound(SOUND_SELECT_BUTTON, D3DXVECTOR3(0, 0, 0), FALSE);
					ShowWnd(FALSE, 0, 0);

					return  INF_MSGPROC_BREAK;
				}
				else {
					char szTemp1[1024];
					int secLeft = 120 - tempTime.GetTimeDiffTimeInSeconds(serverTime);
					if (secLeft > 0)
						sprintf(szTemp1, "\\rYou have to wait \\e%d\\r seconds more to link another item !!!\\r", secLeft);
					g_pD3dApp->m_pChat->CreateChatChild(szTemp1, COLOR_ERROR);
				}
			}
		}
		if(g_pD3dApp->m_pSound)
			g_pD3dApp->m_pSound->PlayD3DSound(SOUND_SELECT_BUTTON, D3DXVECTOR3(0,0,0), FALSE);
		ShowWnd(FALSE, 0, 0);
		
		return  INF_MSGPROC_BREAK;
	}	
	
	// 2013-05-28 by bhsohn 아머 컬렉션 시스템
	if(m_pArmorCollectionBtn && TRUE == m_pArmorCollectionBtn->OnLButtonUp(pt))
	{			
		char szTempURL[1024];
		sprintf(szTempURL, "https://oldschoolrivals.com/pedia/item/%d", m_nShowItemNum);
		//open link
		ShellExecute(NULL, "open", szTempURL, NULL, NULL, SW_SHOWNORMAL);
		if(g_pD3dApp->m_pSound)
		g_pD3dApp->m_pSound->PlayD3DSound(SOUND_SELECT_BUTTON, D3DXVECTOR3(0,0,0), FALSE);
		ShowWnd(FALSE, 0, 0);
		
		return  INF_MSGPROC_BREAK;
	}	
	if(m_pPreview3D && TRUE == m_pPreview3D->OnLButtonUp(pt))
	{			
		//g_pGameMain->m_pInven->RenderMirror2(m_nShowItemNum);
		//g_pGameMain->m_pInven->ShowPrewInven(m_nShowItemNum);
		if(g_pD3dApp->m_pSound)
			g_pD3dApp->m_pSound->PlayD3DSound(SOUND_SELECT_BUTTON, D3DXVECTOR3(0,0,0), FALSE);
		ShowWnd(FALSE, 0, 0);
		
		return  INF_MSGPROC_BREAK;
	}	
	// END 2013-05-28 by bhsohn 아머 컬렉션 시스템
#ifdef _INET_LINK_ITEMS
	if (m_pLinkBtn && TRUE == m_pLinkBtn->OnLButtonUp(pt)) {
		if (COMPARE_RACE(g_pD3dApp->m_pShuttleChild->m_myShuttleInfo.Race, RACE_OPERATION) == TRUE) {
			char szTemp[100];
			sprintf(szTemp, "[ID:%d]", m_uItemUniNum);
			g_pD3dApp->m_pIMSocket->SendChat(T_IC_CHAT_SELL_ALL, g_pD3dApp->m_pShuttleChild->m_myShuttleInfo.CharacterName, szTemp);

			if(g_pD3dApp->m_pSound)
				g_pD3dApp->m_pSound->PlayD3DSound(SOUND_SELECT_BUTTON, D3DXVECTOR3(0, 0, 0), FALSE);
			ShowWnd(FALSE, 0, 0);

			return  INF_MSGPROC_BREAK;
		}
		else {
			if (nCountOfSendedIt == 0) {
				nCountOfSendedIt++;
				serverTime = GetServerDateTime();
				char szTemp[100];
				sprintf(szTemp, "[ID:%d]", m_uItemUniNum);
				g_pD3dApp->m_pIMSocket->SendChat(T_IC_CHAT_SELL_ALL, g_pD3dApp->m_pShuttleChild->m_myShuttleInfo.CharacterName, szTemp);

				if(g_pD3dApp->m_pSound)
					g_pD3dApp->m_pSound->PlayD3DSound(SOUND_SELECT_BUTTON, D3DXVECTOR3(0, 0, 0), FALSE);
				ShowWnd(FALSE, 0, 0);

				return  INF_MSGPROC_BREAK;
			}
			else {
				ATUM_DATE_TIME tempTime = GetServerDateTime();
				tempTime.GetCurrentDateTime();

				if (tempTime.GetTimeDiffTimeInSeconds(serverTime) >= 120)
				{
					tempTime.Reset();
					serverTime.Reset();
					char szTemp[100];
					sprintf(szTemp, "[ID:%d]", m_uItemUniNum);
					g_pD3dApp->m_pIMSocket->SendChat(T_IC_CHAT_SELL_ALL, g_pD3dApp->m_pShuttleChild->m_myShuttleInfo.CharacterName, szTemp);
					serverTime = GetServerDateTime();

					if(g_pD3dApp->m_pSound)
						g_pD3dApp->m_pSound->PlayD3DSound(SOUND_SELECT_BUTTON, D3DXVECTOR3(0, 0, 0), FALSE);
					ShowWnd(FALSE, 0, 0);

					return  INF_MSGPROC_BREAK;
				}
				else {
					char szTemp1[1024];
					int secLeft = 120 - tempTime.GetTimeDiffTimeInSeconds(serverTime);
					if (secLeft > 0)
						sprintf(szTemp1, "\\rYou have to wait \\e%d\\r seconds more to link another item !!!\\r", secLeft);
					g_pD3dApp->m_pChat->CreateChatChild(szTemp1, COLOR_ERROR);
				}
			}
		}

		if(g_pD3dApp->m_pSound)
			g_pD3dApp->m_pSound->PlayD3DSound(SOUND_SELECT_BUTTON, D3DXVECTOR3(0, 0, 0), FALSE);
		ShowWnd(FALSE, 0, 0);

		return  INF_MSGPROC_BREAK;
	}
#endif
#ifdef _INET_RIGHTCLICK_PEDIA
	if (m_pPediaBtn && TRUE == m_pPediaBtn->OnLButtonUp(pt))
	{
		char szTempURL[1024];
		sprintf(szTempURL, "https://rivals-evolution.net/pedia/item/%d", m_nShowItemNum);
		//open link
		ShellExecute(NULL, "open", szTempURL, NULL, NULL, SW_SHOWNORMAL);

		if(g_pD3dApp->m_pSound)
			g_pD3dApp->m_pSound->PlayD3DSound(SOUND_SELECT_BUTTON, D3DXVECTOR3(0, 0, 0), FALSE);
		ShowWnd(FALSE, 0, 0);

		return  INF_MSGPROC_BREAK;
	}
#endif
	if(!IsWndRect(pt) && !m_bMove)
	{
		return INF_MSGPROC_NORMAL;
	}
	
	return INF_MSGPROC_NORMAL;
}

int CINFItemMenuList::OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	POINT pt;
	pt.x = LOWORD(lParam);
	pt.y = HIWORD(lParam);
	CheckMouseReverse(&pt);	
	
	if(!IsWndRect(pt) && !m_bMove)
	{
		ShowWnd(FALSE,0, 0); // 다른곳 클릭시 숨기게 수정
		return INF_MSGPROC_NORMAL;
	}
	
	
	{
		if(m_pItemMixInfoBtn && TRUE == m_pItemMixInfoBtn->OnLButtonDown(pt))
		{
			// 버튼위에 마우스가 있다.
			return  INF_MSGPROC_BREAK;
		}		
	}	
	
	// 2013-05-28 by bhsohn 아머 컬렉션 시스템
	{
		if(m_pArmorCollectionBtn && TRUE == m_pArmorCollectionBtn->OnLButtonDown(pt))
		{
			// 버튼위에 마우스가 있다.
			return  INF_MSGPROC_BREAK;
		}		
	}
	{
		if(m_pPreview3D && TRUE == m_pPreview3D->OnLButtonDown(pt))
		{
			// 버튼위에 마우스가 있다.
			return  INF_MSGPROC_BREAK;
		}		
	}
	// END 2013-05-28 by bhsohn 아머 컬렉션 시스템
#ifdef _INET_LINK_ITEMS
	{
		if (m_pLinkBtn && TRUE == m_pLinkBtn->OnLButtonDown(pt))
		{
			// 버튼위에 마우스가 있다.
			return  INF_MSGPROC_BREAK;
		}
	}
#endif
#ifdef _INET_RIGHTCLICK_PEDIA
	{
		if (m_pPediaBtn && TRUE == m_pPediaBtn->OnLButtonDown(pt))
		{
			// 버튼위에 마우스가 있다.
			return  INF_MSGPROC_BREAK;
		}
	}
#endif
	return INF_MSGPROC_NORMAL;
	
	
}

int CINFItemMenuList::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	POINT pt;
	pt.x = LOWORD(lParam);
	pt.y = HIWORD(lParam);
	CheckMouseReverse(&pt);		

	// 2013-05-28 by bhsohn 아머 컬렉션 시스템
	BOOL bRtn = m_pItemMixInfoBtn->OnMouseMove(pt);	
	if(m_pArmorCollectionBtn )
	{
		bRtn |= m_pArmorCollectionBtn->OnMouseMove(pt);
	}
	if(m_pPreview3D)
	{
		bRtn |= m_pPreview3D->OnMouseMove(pt);
	}
#ifdef _INET_LINK_ITEMS
	if (m_pLinkBtn)
	{
		bRtn |= m_pLinkBtn->OnMouseMove(pt);
	}
#endif
#ifdef _INET_RIGHTCLICK_PEDIA
	if (m_pPediaBtn)
	{
		bRtn |= m_pPediaBtn->OnMouseMove(pt);
	}
#endif
	if(bRtn)	
	{
		g_pGameMain->SetToolTip(0,0,NULL);
		g_pGameMain->SetItemInfoUser( NULL, 0, 0 );
		
		return INF_MSGPROC_BREAK;
	}
	// END 2013-05-28 by bhsohn 아머 컬렉션 시스템

	if(!IsWndRect(pt) && !m_bMove)
	{
		return INF_MSGPROC_NORMAL;
	}
	
	{
		if(m_bMove)
		{
			m_ptBkPos.x = pt.x - m_ptCommOpMouse.x;
			m_ptBkPos.y = pt.y - m_ptCommOpMouse.y;				
			// UI유저 지정 
			//UpdateBtnPos();
			return INF_MSGPROC_BREAK;
		}
	}
	return INF_MSGPROC_NORMAL;
	
}