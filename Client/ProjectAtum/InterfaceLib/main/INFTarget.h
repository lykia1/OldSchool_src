// INFTarget.h: interface for the CINFTarget class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INFTARGET_H__1E2FD209_BB49_4CB7_ADDA_5BE689875601__INCLUDED_)
#define AFX_INFTARGET_H__1E2FD209_BB49_4CB7_ADDA_5BE689875601__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "INFBase.h"


#define VEL_ATTACK_MOUSE_ROTATION			11
#define VEL_NORMAL_MOUSE_ROTATION			2
#define TARGET_MOUSE_SCALE		((float)g_pD3dApp->GetBackBufferDesc().Width/1024.0f* (g_pSOption->sScalePerc / 100.0f))
#define CENTER_START_X				(g_pD3dApp->GetBackBufferDesc().Width/2 - 15*TARGET_MOUSE_SCALE)
#define CENTER_START_Y				(g_pD3dApp->GetBackBufferDesc().Height/2 - 15*TARGET_MOUSE_SCALE)
#define MOUSE_HALF_SIZE				(25*TARGET_MOUSE_SCALE)
#define MOUSE_START_POS_X			(x - MOUSE_HALF_SIZE)
#define MOUSE_START_POS_Y			(y - MOUSE_HALF_SIZE)
#define SECOND_TARGET_HALF_SIZE_X	(56/2*TARGET_MOUSE_SCALE)
#define SECOND_TARGET_HALF_SIZE_Y	(54/2*TARGET_MOUSE_SCALE)
#define AUTO_TARGET_HALF_SIZE		(42*TARGET_MOUSE_SCALE)
#define HP_START_X_FROM_MOUSE		(AUTO_TARGET_HALF_SIZE - (11*TARGET_MOUSE_SCALE))
#define HP_START_Y_FROM_MOUSE		(AUTO_TARGET_HALF_SIZE + (1*TARGET_MOUSE_SCALE) - 2)				// 2006-11-27 by ispark
#define AUTO_TARGET_HP_SIZE_X		61//*TARGET_MOUSE_SCALE)
#define AUTO_TARGET_HP_SIZE_Y		5//*TARGET_MOUSE_SCALE)
#define TARGET_ARROW_HALF_SIZE		(14*TARGET_MOUSE_SCALE)//28/2
#define NAME_FROM_MOUSE_CENTER_Y	(AUTO_TARGET_HALF_SIZE + 14) // 12 : text height
#define DISTANCE_FROM_MOUSE_CENTER_Y (AUTO_TARGET_HALF_SIZE - 6) // 12 : text height
#define ID_GUILD_MARK_START_X		12
#define ID_GUILD_MARK_START_Y		(AUTO_TARGET_HALF_SIZE+25)
#define TARGET_DIR_ARROW_HALF_SIZE	(19*TARGET_MOUSE_SCALE)//28/2

#define FIRE_ANGLE_START_X			(g_pD3dApp->GetBackBufferDesc().Width/2 - 45*TARGET_MOUSE_SCALE)
#define FIRE_ANGLE_START_Y			(g_pD3dApp->GetBackBufferDesc().Height/2 - 45*TARGET_MOUSE_SCALE)
#define FIRE_ANGLE_START_X1			(g_pD3dApp->GetBackBufferDesc().Width/2)
#define FIRE_ANGLE_START_Y1			(g_pD3dApp->GetBackBufferDesc().Height/2)
#define MOUSE_STATE_NORMAL			0
#define MOUSE_STATE_UP				1
#define MOUSE_WISPERCHAT_IMAGE_GAB  10

#define CENTER_NORMAL				0
#define CENTER_LOCKON				1
#define TARGET_TICK_MOVE_POS_X		1

class CINFImage;
class CAppEffectData;
class CD3DHanFont;
class CINFGameMainOtherInfo;
class CAtumData;
class CUnitData;
// 2005-08-12 by ispark
struct stSecondTarget
{
	int		nTargetIndexNum;
	int		nTargetAnimationTick;
	BOOL	bUse;
	stSecondTarget()
	{
		nTargetIndexNum = 0;
		nTargetAnimationTick = 0;
		bUse = FALSE;
	}
};

class CINFTarget : public CINFBase  
{
public:
	CINFTarget();
	virtual ~CINFTarget();
	int nDistance;
	virtual HRESULT InitDeviceObjects();
	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	void Render();
	void RenderMouse(int x, int y, int type);
	void Render2stTarget(int x, int y, int nTargetAnimationTick);
	void RenderAutoTarget(BOOL bAutoTarget, BOOL bShowAutoTargetHP, CAtumData* pUnit);
	void RenderHP(int x, int y, float fRate);

	virtual void Tick();
	virtual int WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

	void SetMouseType(int nMouseType);
	int GetMouseType() { return m_nMouseType; }
	void RenderAttackMeMonster();
	void RenderAutoTargetDrow();
	void RenderAttackMeObject();
	void SetTargetAnimation(){m_bTargetAnimation = TRUE;}
	BOOL GetShowTargetCodition();
	void RenderAttackMePKEnemy();
	void RenderOtherTargetMonster();						// 2006-11-09 by ispark, ¸ó˝şĹÍ°ˇ ´Ů¸Ą »ó´ë Ĺ¸°Ů ÇĄ˝Ă
	void RenderInfluenceMonster(CMonsterData * pMon);		// 2006-11-24 by ispark, °°Ŕş ĽĽ·Â ¸ó˝şĹÍżˇ ´ëÇŃ ÇĄ˝Ă

protected:
	void SetMonsterHPString(char* strHP);
	void RenderGameMainMonsterInfo(CMonsterData* pUnit, int x, int y, float fHPCurrent, float fHPMax); //19-04-2016 by Inetpub - changed hprate to hp fHPCurrent and fHPMax
	void RenderGameMainEnemyInfo(CEnemyData* pUnit, int x, int y);
	// 2007-05-22 by bhsohn Ĺ¸ÄĎ ÇĄ˝Ă ĽöÁ¤ľČ Ăł¸®
	//void RenderTargetArrow(D3DXVECTOR3 vPos );
	POINT RenderTargetArrow(D3DXVECTOR3 vPos );
	void RenderOutSideMonster(CMonsterData* pMonster);		// ł»˝ĂľßżˇĽ­ ąţľîł­ łŞ¸¦ °ř°ÝÇĎ´Â ¸ó˝şĹÍ Á¤ş¸ ÇĄ˝Ă
	void RenderOutSideEnemy(CEnemyData* pEnemy);
	void RenderOutSideObject(D3DXVECTOR3 vPos);

	// 2007-05-22 by bhsohn Ĺ¸ÄĎ ÇĄ˝Ă ĽöÁ¤ľČ Ăł¸®
	void RenderDistance(CAtumData* pUnit, POINT ptArrow);
	BOOL IsTargetToParty(ClientIndex_t i_nTargetIndex);

public:
	CINFImage*		m_pCenter[2];				// "t_center0, t_center1", °ˇżîµĄ ÇĄ˝Ă
	CINFImage*		m_pRedMouse[3];				// "t_m_red", ¸¶żě˝ş »ˇ°Ł»ö Ĺ¸°Ů
	CINFImage*		m_pArrowMouse;				// "t_m_whi", ¸¶żě˝ş ÇĎľá»ö ąćÇâÁö˝Ă
	CINFImage*		m_pBlueMouse[3];				// "t_m_blue", ¸¶żě˝ş ĆÄ¶ő»ö Ĺ¸°Ů
	CINFImage*		m_p2stLockOn;				// "t_2st_l", 2Çü ą«±â ¶ôżÂ
	CINFImage*		m_pTextLockOn;				// "t_text_l", "LOCKON!"ĹŘ˝şĆ®
	CINFImage*		m_pAutoTarget;				// "t_ototar", ŔÚµż Ĺ¸°Ů Ľł
	CINFImage*		m_pAutoTargetLong;			// "t_oto", ŔÚµż Ĺ¸°Ů (»çÁ¤°Ĺ¸® ąŰżˇ ŔÖ´Â °ćżě
	CINFImage*		m_pSkillTarget;				// "skTarget", Ĺ¸°ŮŔĚ ŔÖ´Â ˝şĹł »çżë˝Ă Ĺ¸°Ů
	CINFImage*		m_pHPBox;					// "t_hpbox", HPąÚ˝ş
	CINFImage*		m_pHP;						// "t_hp", HPÁ¤ş¸
	CINFImage*		m_pNormalMouse[2];			// "t_m_red", ¸¶żě˝ş »ˇ°Ł»ö Ĺ¸°Ů
	CINFImage*		m_pFireAngle[3][4];			// "t_angle", Č­¸Á (Č­¸ÁŔĚąĚÁö,A±âľî´Â ľČłŞżČ)	

	CINFImage*		m_pWisperBoxLeftTRightB;	// ±Ó¸» Ă¤ĆĂĂ˘ ¸¶żě˝ş ŔĚąĚÁö
	CINFImage*		m_pWisperBoxLeftBRightT;	// ±Ó¸» Ă¤ĆĂĂ˘ ¸¶żě˝ş ŔĚąĚÁö
	CINFImage*		m_pWisperBoxLeftRight;		// ±Ó¸» Ă¤ĆĂĂ˘ ¸¶żě˝ş ŔĚąĚÁö
	CINFImage*		m_pWisperBoxTopBottom;		// ±Ó¸» Ă¤ĆĂĂ˘ ¸¶żě˝ş ŔĚąĚÁö
	CINFImage*		m_pWisperBoxCross;			// ±Ó¸» Ă¤ĆĂĂ˘ ¸¶żě˝ş ŔĚąĚÁö

	CINFImage*		m_pMonTargetStateImg;		// ¸ó˝şĹÍŔÇ °ř°Ý´ë»ó »óĹÂ ÇĄ˝ĂÇĎ´Â ŔĚąĚÁö(¸ó˝şĹÍŔÇ ŔĚ¸§ ľçÂĘ)
	CINFImage*		m_pImgTargetMe[2];			// ł»°ˇ Ĺ¸°ŮŔ» ŔâŔş ¸ó˝şĹÍ

//	CAppEffectData		* m_pTargetDirArrow;	// Č­¸éżˇ ąţľîłŞ´Â Ĺ¸°Ů ąćÇâ Č­»ěÇĄ
	// 2010. 03. 03 by ckPark ŔÎÇÇ´ĎĆĽ ÇĘµĺ 2Â÷(ŔÔŔĺ UIşŻ°ć)
	//CINFImage*		m_pTargetDirArrow[3];		// Č­¸éżˇ ąţľîłŞ´Â Ĺ¸°Ů ąćÇâ Č­»ěÇĄ
	CINFImage*		m_pTargetDirArrow[4];		// Č­¸éżˇ ąţľîłŞ´Â Ĺ¸°Ů ąćÇâ Č­»ěÇĄ
	CINFImage*        m_pTargetDirArrowMonster[2];        // ȭ�鿡 ����� Ÿ�� ���� ȭ��ǥ
	// end 2010. 03. 03 by ckPark ŔÎÇÇ´ĎĆĽ ÇĘµĺ 2Â÷(ŔÔŔĺ UIşŻ°ć)
	CINFImage*		m_pImgArrowHelp;			// ĽĽ·ÂĽ±ĹĂ Ŕü±îÁö Č­»ěÇĄŔÇ Ľł¸íŔ» ÇŘÁŘ´Ů.
	float			m_fTargetDirVel;
	float			m_fMouseRotationtAngle;		// ¸¶żě˝ş Č¸Ŕü °˘µµ
	int				m_nMouseType;				// ŔĎąÝ, şí·çĹ¸°Ů, ·ąµĺĹ¸°Ů, Č¸Ŕüşí·ç, Č¸Ŕü·ąµĺ
//	float			m_fAutoTargetHPRate;		// ŔÚµż Ĺ¸°Ů ĽłÁ¤˝Ă(±¸.°­Á¦¶ôżÂ) HP ľç
//	BOOL			m_bAutoTargetShow;			// ŔÚµż Ĺ¸°Ů ĽłÁ¤˝Ă(±¸.°­Á¦¶ôżÂ) HP ş¸ż©ÁÖ´ÂÁö ż©şÎ
	CD3DHanFont*	m_pFontAutoTargetName;
	CD3DHanFont*	m_pFontAutoTargetDistance;
	CD3DHanFont*	m_pFontTargetArrow;
	CD3DHanFont*	m_pFontAIMoMGsmall;
	CD3DHanFont*	m_pFontAIMoMGbig;
	int				m_nMouseState;
	BOOL			m_bRestored;
	BOOL			m_bWisperChatBoxMouseFlag;	// ±Ó¸» Ă¤ĆĂĂ˘ ¸¶żě˝ş Ć÷ŔÎĆ® Č°ĽşČ­ şńČ°ĽşČ­ 
	CINFGameMainOtherInfo* m_pOtherInfo;

	float			m_fTickCount;
	int				m_nAttackerTargetX;
	int				m_nTargetColor;
	BOOL			m_bReverseTargetMove;
#ifdef _INET_RANKS
	CINFImage*		m_pImgRank[22];
#endif // RANK_SHOW_IMG_SILVER
	D3DXVECTOR3		m_vecTargetTemp;
protected:
	CINFImage*		m_pImgTargetAnimation[4];
	CINFImage*		m_pImgTargetHelper[2];				// 2007-07-27 by dgwoo Ŕú·ľŔ» Ŕ§ÇŃ Ľł¸í [0] 1Çü, [1] 2Çü.
	CINFImage*		m_pImgSelectTarget[2];
	CINFImage*		m_pImgPartyTarget[2];				// 2008-06-17 by dgwoo ĆÄĆĽżřŔ» Ĺ¸°ŮŔâŔş ¸ó˝şĹÍŔÇ ÇĄ˝Ă.

	list<stSecondTarget>	m_listTargetOld;		// 2005-08-12 by ispark
					// Č­¸Á ·»´ő¸¦ Ŕ§ÇŃ ŔÓ˝Ă şŻĽö
	BOOL			m_bTargetAnimation;
	BOOL			m_bTargetDistance;
	BOOL			m_bTargetDis;					// ¸¶żě˝ş Ć÷ŔÎĹÍ¸¦ ş¸ŔĚÁö ľĘ°Ô ÇĎ´ÂÄÚµĺ
//	int				m_nTargetAnimationTick;			// 2005-08-12 by ispark
	int				m_nOrderTargetX;
	int				m_nOrderTargetY;
	float 			m_fTimeDelay;	

	// 2010. 03. 03 by ckPark ŔÎÇÇ´ĎĆĽ ÇĘµĺ 2Â÷(ŔÔŔĺ UIşŻ°ć)
	CINFImage*		m_pImgFriendlyTarget[2];
	// end 2010. 03. 03 by ckPark ŔÎÇÇ´ĎĆĽ ÇĘµĺ 2Â÷(ŔÔŔĺ UIşŻ°ć)
};

#endif // !defined(AFX_INFTARGET_H__1E2FD209_BB49_4CB7_ADDA_5BE689875601__INCLUDED_)
