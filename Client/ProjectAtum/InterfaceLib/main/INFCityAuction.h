// INFWorldRankWnd.h: interface for the CINFWorldRankWnd class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INFWORLDRANKWND_H__C2EF2BD5_6F6C_4AAE_80DC_AAC6DC21DB39__INCLUDED_)
#define AFX_INFWORLDRANKWND_H__C2EF2BD5_6F6C_4AAE_80DC_AAC6DC21DB39__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "INFBase.h"
#include "INFDefaultWnd.h"
#include "WorldRankManager.h"


// 각 좌표 최대
#define		MAX_WR_INFO_X				10
#define		MAX_PER_ONEPAGE				9
#define		MAX_PAGES_COUNT				10

#define		TAB_COUNT					3

#define		TAB_BUY_ITEM				0
#define		TAB_SELL_ITEM				1
#define		TAB_FINISHED_ITEM			2
class CINFImageListTreeCtrl;
class CD3DHanFont;
class CINFListBox;
class CINFNumEditBox;
class CINFEditBox;
#define MAX_TREE_COUNT 41
enum { NON_SORT = -1, UP_SORT, DOWN_SORT };
//enum {NONETITME = -1, ENCHANTTITLE, NAMETITLE, LEVELTITLE, TIMETITLE, MONEYTITLE};
enum { NONETITME = -1, MONEYTITLE, LEVELTITLE, ENCHANTTITLE, NAMETITLE, TIMETITLE };
struct TRADEITEMCATEGORYTREE		// 2013-12-03 by ymjoo 거래소 카테고리 목록 확장
{
	int nCategoryId;
	BYTE byKind;
	char* Name;
};
struct CATEGORYTREE
{
	int nMainidx;
	int nSubIdx;
	BYTE byKind;
	char* Name;

};
struct SORTBTNSTATE
{
	POINT ptPos;
	RECT  Range;
	BYTE byBtnState;
	BYTE bySortState;
};
enum { TC_SPI, TC_WP , TC_CREDITS};
struct MARKETITEM_INFO
{
	MSG_FC_MARKET_BASE_INFO_OK* pINFO;
	CItemInfo* pItem;
};
#define ITEMLIST_ITEMNUM					10

struct MARKET_MY_ITEM_INFO
{
	MSG_FC_MARKET_MY_LIST_OK* pINFO;
	CItemInfo* pItem;
	BYTE byBtnState;
};
class CINFCityAuction : public CINFBase
{
public:
	CINFCityAuction(CAtumNode* pParent);
	virtual ~CINFCityAuction();

	virtual HRESULT InitDeviceObjects();
	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	virtual void	Render();
	virtual void	Tick();
	CINFImageBtn* m_pCloseXBtn;
	int nMylistStartFromId;

	virtual int WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual void UpdateBtnPos(int nWidth, int nHeight);
	int nRetMyHit(POINT pt);
	int nRetHit(POINT pt);
	void	RqItemListFromServer();	// 랭킹 정보 요청
	void ItemVecAdd(MSG_FC_MARKET_BASE_INFO_OK* pMsg);
	void VecItemInfoClear();
	void VecMyItemInfoClear();
	void Search();
	void BuyItem();
	BYTE				m_byMoneySelect;
	bool bBoughtItem;
	void GetItemName(CItemInfo* pRefItemInfo, char* pName,  BOOL bColor = TRUE);
	void RemoveColor(CItemInfo* pRefItemInfo, char* pChar);
	void NumEditBoxChangeCount(CINFNumEditBox* pNumEditBox, int nNum);
	CINFNumEditBox* m_pNumEditBox;
	CINFNumEditBox* m_pMoneyEditBox;
	CINFImageBtn* m_pMaxBtn;
	CINFImageBtn* m_pUpCntBtn;
	CINFImageBtn* m_pDownCntBtn;
	CINFImageBtn* m_pRegisterItemBtn;
	CINFImage* m_pSellOKBtn[3];
	CINFImage* m_pTimeLimitBtn[3];
	void SendGetRequest(UID64_t UID, BYTE byState, INT nPrice, BYTE MoneyType);
	void ADDItem();
	void ADDItemOk(UID64_t ItemUID);
	void ItemButOk(UID64_t MarketUID);
	CINFListBox* m_pMoneyComboBox;
	vector<MARKETITEM_INFO*>	m_vecItemInfo;
	vector<MARKET_MY_ITEM_INFO*>	m_vecMyItemInfo;
	CItemInfo* m_nCurerntInfo;
	POINT ptMouse;
	void MinMaxValue(BYTE* nMin, BYTE* nMax, CINFNumEditBox* pMinEditBox01, CINFNumEditBox* pMaxEditBox02, int nMaxCount);
	int m_nSellingTime;
	int m_nMaxPages;
	int m_nItemCountTotal;
	int						m_nSelectPage;
	int nFirstPage;
	int m_nSelectListItem;
	int m_nSelectMyListItem;
	int m_nCurrentSortingType;
	void MyItemVecAdd(MSG_FC_MARKET_MY_LIST_OK* pMsg);
	int nTickWaitRefresh;
	int nTickWaitRefreshMY;
	int nTickWaitSortClick;
	BOOL UpLoadItem(CItemInfo* i_pItem);
	CItemInfo* m_pSourceItem;
	void InitData();
	void InvenToSourceItem(CItemInfo* pItemInfo, int nCount);
	int				m_nCurrentTab;
	CINFEditBox* m_pNameEditBox;
	CINFNumEditBox* m_plevelNumEditBox[2];
	CINFNumEditBox* m_pEnchantNumEditBox[2];
private:
	int OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam);
	int OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam);
	int OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam);


	void ListRender();
	void OnClickRankBtn(int i_nSelIdx);
	void LoadListItem();
	void	RenderMyMoney();

	void	RenderSelectPage();

	void	InitInfo();	// 초기화


	BOOL	IsRqPossibleStats();
	BOOL GetRemainTime(ATUM_DATE_TIME regTime, char* buf);
	void MyListRender();

private:
	void RenderCategoryPath(char* strInputSearch, bool bSearch=false);									// 2013-12-04 by ymjoo 거래소 카테고리 경로 표시
	void PrintCategoryPath(char* chCategoryPath);				// 2013-12-04 by ymjoo 거래소 카테고리 경로 표시
	void _PrintCategoryPath(char* chCategoryPath, int nodeId);	// 2013-12-04 by ymjoo 거래소 카테고리 경로 표시
	CINFImage* m_pCategoryPathArrowImage;		// 2013-12-04 by ymjoo 거래소 카테고리 경로 표시
	CD3DHanFont* m_pFontCategoryPath;			// 2013-12-04 by ymjoo 거래소 카테고리 경로 표시
	int					m_nSelectedCategoryId;
	CINFImage* m_pBkImage[TAB_COUNT];

	CINFImageBtn* m_pBtnHover[2];
	CINFImageBtn* m_pBtnBUY;
	CINFImage* m_pItemSelectedRow;
	CINFImage* m_pSortArrowUpDn[2];


	CINFImageListTreeCtrl* m_pListTreeCtrl;
	TRADEITEMCATEGORYTREE* m_pItemCategorytree;

	int NumEditBoxMaxAndMIN(CINFNumEditBox* pNumEditBox, int nMaxCount, BOOL bDefaultText = TRUE);
	
	// 어떤 랭킹을 선택
	CINFImageBtn* m_pRankBtn[TAB_COUNT];
	CINFImage* m_pRankBk[TAB_COUNT];
	CINFImage* m_pMoneyImg[3];
	int						m_nSelectRankInfo;
	CINFIcon* m_pIconInfo;

	// 자신의 랭킹 정보
	CD3DHanFont* m_pFontRankInfo;

	// 유저 정보
	CD3DHanFont* m_pFontTable[MAX_PER_ONEPAGE][MAX_WR_INFO_X];

	// 선택한 페이지
	CD3DHanFont* m_pFontPage[MAX_PAGES_COUNT];


	DWORD					m_dwSendTermTime;	// 시간

	// 기어 선택
	CINFListBox* m_pComboGear;
	CINFListBox* m_pComboCurrency;

};

#endif // !defined(AFX_INFWORLDRANKWND_H__C2EF2BD5_6F6C_4AAE_80DC_AAC6DC21DB39__INCLUDED_)
