// INFCityStore.h: interface for the CINFCityLab class.
// 2004.07.15 ydKim
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INFCITYLAB_H__AD058082_959A_4B02_BEDD_AAE54DE7B3E0__INCLUDED_)
#define AFX_INFCITYLAB_H__AD058082_959A_4B02_BEDD_AAE54DE7B3E0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "INFBase.h"

// 2008-03-14 by bhsohn 조합식 개선안
#include "AtumApplication.h"
#include "INFNumEditBox.h"
//15-05-2021 by Inetpub
#define MACRO_RADIO_SLOW_OPTIONS_COUNT	3
#define MACRO_SLOW_SECOND				2
#define MACRO_SLOW_HALF					1
#define MACRO_SLOW_NONE					0

#define MACRO_RADIO_ENCHANT_CAP_COUNT	4
#define MACRO_RADIO_ENCHANT_CAP_12	3
#define MACRO_RADIO_ENCHANT_CAP_11	2
#define MACRO_RADIO_ENCHANT_CAP_10	1
#define MACRO_RADIO_ENCHANT_CAP_NO	0


#define LAB_BUTTON_NUMBER			3

class CINFImage;
class CD3DHanFont;
class CINFImageRadioBtn;
class CINFCityLab : public CINFBase  
{
public:
	CINFCityLab(CAtumNode* pParent, BUILDINGNPC* pBuilding);
	virtual ~CINFCityLab();
#ifdef _INET_ANTI_MACRO
	vector<int> vecLastClickTime;
	POINT lastPosClicked;
	int nClickCount;
	int nTicksSinceLastClick;
#endif
	virtual HRESULT InitDeviceObjects();
	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	virtual void Render();
	virtual void Tick();
	virtual int WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void OnButtonClicked(int nButton);	
	void InitDataMacro();
	void InitData();	// m_vecsource를 초기화 m_vectarget 초기화 	
	void InvenToSourceItem(CItemInfo* pItemInfo, int nCount, bool bMacro = false);
	void SetPrice(int nPrice);
#ifdef _INET_ENCHANT_CHANCE
	void SendCheckChancePacket(INT EnchantItemNum, INT UniqueNumber, INT IncreaseValue);
	void SetChance(Prob10K_t nChance);
#endif
	void DeleteTargetItem(UID64_t nItemUniqueNumber);
	BOOL PutEnchant(MSG_FC_ITEM_PUT_ENCHANT* pMsg);
	BOOL PutRareInfo(MSG_FC_STORE_UPDATE_RARE_FIX* pMsg);
	void PutTargetItem(ITEM_GENERAL* pItem);
	CItemInfo*	GetTargetItemInfo();
	CItemInfo*	FindItemFromSource(UID64_t UniqueNumber, bool bMacro = false);
	CItemInfo*  FindItemFromTarget(UID64_t UniqueNumber);

	void ReSetTargetItemNum(UID64_t UniqueNumber, int nItemNum);

	void SetSelectItem(CItemInfo* pItemInfo);

	// 2008-03-14 by bhsohn 조합식 개선안	
	void				OnOpenInfWnd();		// 처음 상점 오픈
	void				OnCloseInfWnd();	// 상점 클로우즈

	// 인챈트 실패 경고 해야 하는지 체크.
	bool IsWarning_EnchantFail ( void );
#ifdef _INET_FACTORY8LAB_DB_CLICK
	void UpLoadItem(CItemInfo* i_pItem);
#endif
protected :

	// 유효하지 않은 조합 관련 에러 메시지 출력.
	void ErrorMsg_InvalidEnchantList ( char *pszMessage , const bool a_bInitData = true );
	
private:
	// 2007-12-17 by bhsohn 조합 가격 표시
	void UpdateMixPrice();

public:
	CItemInfo	*		m_pSelectItem;
	BOOL				m_bIsEnchantCheck;
	bool				bEnchantFinished;
protected:
	BOOL				m_bRestored;

	CINFImage	*		m_pImgButton[LAB_BUTTON_NUMBER][4];	// 0 : 제조(SEND), 1 : cancel, 2 : ok
	int					m_nButtonState[2];
	
	CINFImage	*		m_pImgBack;
	CINFImage	*		m_pImgBackFactory;	// 연구소배경 // 2008-03-14 by bhsohn 조합식 개선안
	
	CINFImage	*		m_pImgTitle;
	CINFImage	*		m_pImgPrice;
	// 2008-03-14 by bhsohn 조합식 개선안
	CINFNumEditBox*		m_pNumEditBox;

	CD3DHanFont *		m_pFontPrice;
	char				m_szPrice[64];
	vector<CItemInfo*>  m_vecSource;
	vector<CItemInfo*>  m_vecTarget;
	BOOL				m_bShowTarget;
	BUILDINGNPC*		m_pBuildingInfo;
#ifdef _INET_ENCHANT_CHANCE
	CD3DHanFont *		m_pFontChance;
	char				m_szChance[64];
	float				fE_CurrentPercent;
	float				fE_IncreaseValue;
#endif
	BOOL				m_bSelectDown;
//21-01-2021 by Inetpub - for macro feature
	CD3DHanFont*		m_pFontMacro;
	char				m_szMacroTxt[512];
	vector<CItemInfo*>  m_vecSourceMacro;
	bool				b_IsPrefixMacro;
	bool				bEnchantMacro;
	

	CINFImageRadioBtn* m_pMacroRadioBtn[MACRO_RADIO_SLOW_OPTIONS_COUNT];//15-05-2021 by Inetpub - one dimension array because only 1 row would be used here
	bool			   m_bMacroRadioSel[MACRO_RADIO_SLOW_OPTIONS_COUNT];//15-05-2021 by Inetpub
		time_t			m_nTicksAfterLastUsage;//15-05-2021 by Inetpub
//for enchant cap
	CINFImageRadioBtn* m_pMacroEnchCapBtn[MACRO_RADIO_ENCHANT_CAP_COUNT];
	bool			   m_bMacroEnchCapSel[MACRO_RADIO_ENCHANT_CAP_COUNT];

};

#endif // !defined(AFX_INFCITYSTORE_H__AD058082_959A_4B02_BEDD_AAE54DE7B3E0__INCLUDED_)
