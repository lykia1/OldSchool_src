#pragma once

class CINFInetRank
{
public:
	CINFInetRank();
	~CINFInetRank();
	static void RefreshRankExpBar();
	static RANK CurrentRanking(CHARACTER* Character, ENEMYINFO* enemy = nullptr);
	static float GetCurrentMultiplier(CHARACTER* s_char, ENEMYINFO* enemy = nullptr);
	static INET_RANKING nPointTable[];
};

