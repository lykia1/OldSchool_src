// INFUnitNameInfo.h: interface for the CINFUnitNameInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INFUNITNAMEINFO_H__580A7F50_EF83_4ADC_B707_DE01602B3EE7__INCLUDED_)
#define AFX_INFUNITNAMEINFO_H__580A7F50_EF83_4ADC_B707_DE01602B3EE7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "INFBase.h"

#define NAME_INFO_BALON_CHAT		9

class CChatMoveData;
class CINFImage;
class CD3DHanFont;
class CINFUnitNameInfo : public CINFBase
{
public:
	CINFUnitNameInfo(CAtumNode* pParent);
	virtual ~CINFUnitNameInfo();

	virtual HRESULT InitDeviceObjects();
	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	virtual void	Render();
	virtual void	Tick();

	void	RenderCharacterInfo(int x, int y, CChatMoveData* pChatData, char* strName, 
	BOOL	bShowHP, float fHP, int nGuildUniqueNumber, int nCharacterUniqueNumber, char *strMent, DWORD dwNameColor
#ifdef _INET_SHOW_GUILD_NAME 
	, BYTE byInflMask, BYTE byRaceMask
#endif
#ifdef _INET_RANKS
	, INT nLevel, USHORT nRace, CHARACTER* charin = NULL, ENEMYINFO* enemyin= NULL
#endif
	);
	void	RenderChatBalon(int x, int y, int cx, int cy);
	void	ChatCreateInfo( int nStringLength, char *strMsgChat, char *strFromCharacterName,LONGLONG nDonatorRank =0 );
	// 2009. 09. 18 by ckPark 인피니티 필드 HP에 따른 몬스터 대화
	void	MonsterChatCreateInfo( int nStringLength, char *strMsgChat, int nMonsterUID );
	// end 2009. 09. 18 by ckPark 인피니티 필드 HP에 따른 몬스터 대화
	DWORD	GetNameColor(BYTE byCityWarTeamType, BYTE byInfluenceLeader, LONGLONG nDonatorRank = 0);
	DWORD	GetInfluenceColor(int byInfluence, BYTE byInfluenceLeader, LONGLONG nDonatorRank=0);
	
public:
//	CINFImage	*	m_pUnitInfo;
//	CINFImage	*	m_pPartyUnitInfo;
//	CINFImage	*	m_pMonsterInfo;
//
//	CINFImage	*	m_pPartyUnitHP;
//	CINFImage	*	m_pPartyUnitEP;
//	CINFImage	*	m_pMonsterHP;
//	CINFImage	*	m_pUp;
//	CINFImage	*	m_pDown;
	CINFImage	*	m_pBalonChat[NAME_INFO_BALON_CHAT];

	CD3DHanFont*	m_pFontMent;

	float			m_fRenderDistance;
	int		nOldx;
	int		nOldy;
	char	m_strChatFullLen[1024];

	// 2007-02-23 by bhsohn ID가 뒤로 밀리는 현상 처리	
	POINT	m_ptUnitNameFirst;	// 처음위치	
#ifdef _INET_RANKS
	CINFImage*		m_pImgRank[22];
#endif // RANK_SHOW_IMG_SILVER
#ifdef _INET_DRANKS
#define DRANK_COUNT	10
#define SPECIAL_TAG_RANK 10	//10 extra slots only for right side
#define EVENT_RANKS 2 //1 extra for left+ right Halloween
#define NEW_RANKS_2024 22 //22 extra ranks  by romu 19-09-2024
#define RANK_CNT_OVERALL DRANK_COUNT + SPECIAL_TAG_RANK+ NEW_RANKS_2024 + EVENT_RANKS
	CINFImage*		m_pImgDRank[2][RANK_CNT_OVERALL+ EVENT_RANKS + NEW_RANKS_2024];
#endif

};

#endif // !defined(AFX_INFUNITNAMEINFO_H__580A7F50_EF83_4ADC_B707_DE01602B3EE7__INCLUDED_)
