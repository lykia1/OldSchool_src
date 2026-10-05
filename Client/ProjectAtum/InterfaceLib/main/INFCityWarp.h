// INFCityWarp.h: interface for the CINFCityWarp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INFCITYWARP_H__8C8FFF17_5918_4808_A782_6368A036D467__INCLUDED_)
#define AFX_INFCITYWARP_H__8C8FFF17_5918_4808_A782_6368A036D467__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "INFBase.h"

#define CITY_WARP_BUTTON_NUMBER			3
#define CITY_WARP_BUTTON_MOVE			0
#define CITY_WARP_BUTTON_CANCEL			1

#define CITY_WARP_LIST_NUMBER			6
#define CITY_WARP_LIST_STRING_LENGTH	64
#ifdef _INET_MAPWARP_INFO
#define	MONSTER_INFO_ITEM_LEN	2
#define	MAX_MONSTER_INFO		8

typedef struct
{
	CINFImage*		pInfImage;		// 맵 이미지
	DataHeader*		pHeader;
	MapIndex_t		MapIndex;
	char			chMapName[256];
	vector<MEX_MONSTER_INFO>	vecMonsterInfo;
} stMapViewInfo1;

typedef struct
{
	int				MapIndex;
	char*			Chain;
} MapChains;
typedef struct
{
	int				MapIndex;
	char*			ItemName;
	char*			MonsterType;
} MapDrops;
#endif
class CAtumNode;
class CINFImage;
class CD3DHanFont;
class CINFScrollBar;
class CINFCityWarp : public CINFBase  
{
public:
	CINFCityWarp(CAtumNode* pParent);
	virtual ~CINFCityWarp();

	void Reset();
	void AddWarpTargetInfoList(WARP_TARGET_MAP_INFO_4_EXCHANGE* pInfo);
	void RecvWarpListDone();
	WARP_TARGET_MAP_INFO_4_EXCHANGE *GetCurrentWarpInfo();

	virtual HRESULT InitDeviceObjects();
	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	void Render();
	virtual void Tick();
	virtual int WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void OnButtonClicked(int nButton);
	void SendFieldSocketRequestShopWarp();//int nMapIndex, int nTargetIndex);

private:
#ifdef _INET_MAPWARP_INFO

	CD3DHanFont *			m_pFontMonInfo;
	char *GetMapChainName(int MapIndex);
	CD3DHanFont *			m_pFontMonTitle;
	POINT					m_ptMonPos[MAX_MONSTER_INFO];
	POINT					m_ptMonTitle;
	void RenderMonsterInfo(vector<MEX_MONSTER_INFO>	*i_vecMonsterInfo, float renderX, float renderY);
	void RestoreMapView(BOOL bAll, MapIndex_t	MapIndex);
	vector<stMapViewInfo1*>	m_vecMapInfo;
	stMapViewInfo1* GetMapIdx_To_MapViewInfo(MapIndex_t	selMapIndex);
	BOOL IsBelligerenceMonster(BYTE monsterBelligerence);
	HRESULT GetMapIndex_To_Monster(MapIndex_t	MapIndex, vector<MEX_MONSTER_INFO> *o_vecQuestInfo);
	int AddMapInfo(MapIndex_t MapIndex);
	CD3DHanFont*	m_pMapInfoFont[10]; //10 lines
#endif
protected:
	BOOL		m_bRestored;
	CINFScrollBar*	m_pScroll;
	CINFImage*	m_pImgBack;
	CINFImage*	m_pImgTitle;
	CINFImage*	m_pButtonMove[CITY_WARP_BUTTON_NUMBER];
	CINFImage*	m_pButtonCancel[CITY_WARP_BUTTON_NUMBER];
	CINFImage*	m_pImgHightLight;

	int			m_nButtonState[2];		// 버튼 상태, CITY_WARP_BUTTON_MOVE, CITY_WARP_BUTTON_CANCEL
	CD3DHanFont *m_pFontWarpList[CITY_WARP_LIST_NUMBER];
	CD3DHanFont *m_pFontWarpPrice[CITY_WARP_LIST_NUMBER];
	char		m_szWarpList[CITY_WARP_LIST_NUMBER][CITY_WARP_LIST_STRING_LENGTH];
	int			m_nWarpListLineNumber;		// 리스트 전체 개수
	int			m_nMapIndex;				// 워프할 맵 인덱스
	int			m_nTargetIndex;				// 워프할 맵의 타겟 인덱스
	
	CVectorWarpTargetInfo m_vecWarpTargetInfo;

	CD3DHanFont*	m_pInfluenceTex;
};

#endif // !defined(AFX_INFCITYWARP_H__8C8FFF17_5918_4808_A782_6368A036D467__INCLUDED_)
