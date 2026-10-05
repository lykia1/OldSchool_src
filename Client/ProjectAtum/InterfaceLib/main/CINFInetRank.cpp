#include "StdAfx.h"
#include "AtumApplication.h"
#include "INFGameMain.h"
#include "INFGameMainUnitInfoBar.h"
#include "ShuttleChild.h"
#include "CINFInetRank.h"

#define RANK_IMG_COUNT 22
#define OBJECTIVES_COUNT 10

INET_RANKING CINFInetRank::nPointTable[] =
{
	{	0	,	1600	,	STRMSG_C_RANK_0000	,	0	},
	{	1600	,	5400	,	STRMSG_C_RANK_0001	,	1	},
	{	5400	,	9200	,	STRMSG_C_RANK_0002	,	2	},
	{	9200	,	13000	,	STRMSG_C_RANK_0003	,	3	},
	{	13000	,	16800	,	STRMSG_C_RANK_0004	,	4	},
	{	16800	,	20600	,	STRMSG_C_RANK_0005	,	5	},
	{	20600	,	24400	,	STRMSG_C_RANK_0006	,	6	},
	{	24400	,	28200	,	STRMSG_C_RANK_0007	,	7	},
	{	28200	,	32000	,	STRMSG_C_RANK_0008	,	8	},
	{	32000	,	35800	,	STRMSG_C_RANK_0009	,	9	},
	{	35800	,	39600	,	STRMSG_C_RANK_0010	,	10	},
	{	39600	,	43400	,	STRMSG_C_RANK_0011	,	11	},
	{	43400	,	47200	,	STRMSG_C_RANK_0012	,	12	},
	{	47200	,	51000	,	STRMSG_C_RANK_0013	,	13	},
	{	51000	,	54800	,	STRMSG_C_RANK_0014	,	14	},
	{	54800	,	58600	,	STRMSG_C_RANK_0015	,	15	},
	{	58600	,	62400	,	STRMSG_C_RANK_0016	,	16	},
	{	62400	,	66200	,	STRMSG_C_RANK_0017	,	17	},
	{	66200	,	70000	,	STRMSG_C_RANK_0018	,	18	},
	{	70000	,	73800	,	STRMSG_C_RANK_0019	,	19	}

};

enum
{
	PLAYTIME = 0,
	FAME,
	LVL,
	WP,
	ARENA,
	PVP,
	MULTI_RANK_CNT
};

float CINFInetRank::GetCurrentMultiplier(CHARACTER* s_char, ENEMYINFO* enemy/*=nullptr*/)
{
	float fMultiplier[6];

	if (enemy)
	{
		//level
		int nLevOfEnemy = enemy->CharacterInfo.Level1;

		if (nLevOfEnemy > CHARACTER_MAX_LEVEL)
			nLevOfEnemy = CHARACTER_MAX_LEVEL;

		fMultiplier[LVL] = (static_cast<float>(nLevOfEnemy) / static_cast<float>(CHARACTER_MAX_LEVEL)) * 100.0f;
		//fame	
		const auto nFame = enemy->CharacterInfo.Propensity + 1; //for 0 division

		int nFameReq;

		if (IS_BT(enemy->CharacterInfo.UnitKind)) //bg
		{
			nFameReq = 18000;
		}
		else if (IS_OT(enemy->CharacterInfo.UnitKind)) //mg
		{
			nFameReq = 8000;
		}
		else if (IS_DT(enemy->CharacterInfo.UnitKind)) //ag
		{
			nFameReq = 20000;
		}
		else  //ig
		{
			nFameReq = 15000;
		}

		fMultiplier[FAME] = (static_cast<float>(nFame) / static_cast<float>(nFameReq)) * 100.0f;
		//playtime
		fMultiplier[PLAYTIME] = ((static_cast<float>(enemy->CharacterInfo.TotalPlayTime) / (24.0f * 3600.0f)) / 31.0f) * 100.0f;
		//CWP
		fMultiplier[WP] = (static_cast<float>(enemy->CharacterInfo.CumulativeWarPoint + 1) / 10000001.0f) * 100.0f;
		//ARENA
		fMultiplier[ARENA] = (static_cast<float>(enemy->CharacterInfo.ArenaWin + 1) / 51.0f) * 100.0f;
		//PVP
		fMultiplier[PVP] = (static_cast<float>(enemy->CharacterInfo.PKWinPoint + 1) / 101.0f) * 100.0f;

	}
	else
	{
		int nLevOfEnemy = s_char->Level;

		if (nLevOfEnemy > CHARACTER_MAX_LEVEL)
			nLevOfEnemy = CHARACTER_MAX_LEVEL;

		fMultiplier[LVL] = (static_cast<float>(nLevOfEnemy) / static_cast<float>(CHARACTER_MAX_LEVEL)) * 100.0f;
		const auto nFame = s_char->Propensity + 1;
		int nFameReq;
		
		if (IS_BGEAR(s_char->UnitKind))
			nFameReq = 18000;
		else if (IS_MGEAR(s_char->UnitKind))
			nFameReq = 8000;
		else if (IS_AGEAR(s_char->UnitKind))
			nFameReq = 20000;
		else  //ig
			nFameReq = 15000;

		fMultiplier[FAME] = (static_cast<float>(nFame) / static_cast<float>(nFameReq)) * 100.0f;
		//playtime
		fMultiplier[PLAYTIME] = ((static_cast<float>(s_char->TotalPlayTime) / (24.0f * 3600.0f)) / 31.0f) * 100.0f;
		//CWP
		fMultiplier[WP] = (static_cast<float>(s_char->CumulativeWarPoint + 1) / 10000001.0f) * 100.0f;
		//ARENA
		fMultiplier[ARENA] = (static_cast<float>(s_char->ArenaWin + 1) / 51.0f) * 100.0f;
		//PVP
		fMultiplier[PVP] = (static_cast<float>(s_char->PKWinPoint + 1) / 201.0f) * 100.0f;
	}

	for (auto& g : fMultiplier) {
		if (g > 100.0f)
			g = 100.0f;
		else if (g <= 0.0f)
			g = 0.0f;
	}

	auto fFinalMultiplier = 0.0f;

	for (auto& m : fMultiplier)
		fFinalMultiplier += m;

	return fFinalMultiplier;
}



RANK CINFInetRank::CurrentRanking(CHARACTER* character, ENEMYINFO* enemy/*=nullptr*/)
{
	int nLevel;
	float fMultiplier;
	
	if (enemy) {
		nLevel = enemy->CharacterInfo.Level1;
		fMultiplier = GetCurrentMultiplier(nullptr, enemy);
	}
	else {
		nLevel = character->Level;
		fMultiplier = GetCurrentMultiplier(character);
	}

	auto nPointForSearch = static_cast<int>(fMultiplier) * nLevel;
	if (nPointForSearch >= 73800)
		nPointForSearch = 73800;
	
	for (auto& i : nPointTable)
	{
		if (IS_IN_RANGE(i.PointsMin, nPointForSearch, i.PointsMax))
			return i;
	}

	return {};
}

CINFInetRank::CINFInetRank()
{
}

CINFInetRank::~CINFInetRank()
{
	
}

void CINFInetRank::RefreshRankExpBar()
{
	if (g_pShuttleChild) {
		auto character = g_pShuttleChild->GetMyShuttleInfo();
		const auto fMinPoint = CurrentRanking(&character).PointsMin;
		const auto fMaxPoint = CurrentRanking(&character).PointsMax;

		const auto fMultiplier = GetCurrentMultiplier(&character);
		auto nPointForSearch = character.Level * static_cast<int>(fMultiplier);
		if (nPointForSearch >= 73800)
			nPointForSearch = 73800;

		const auto fExpRate = static_cast<float>(((nPointForSearch - fMinPoint) / (fMaxPoint - fMinPoint)) * 100);

		if (g_pGameMain->m_pUnitInfoBar)
			g_pGameMain->m_pUnitInfoBar->SetREXP(fExpRate);
	}
}
