// ShuttleChild.cpp: implementation of the CShuttleChild class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ShuttleChild.h"
#include "AtumApplication.h"
#include "ChatMoveData.h"
#include "ItemData.h"
//#include "TraceData.h"
// remodeling, 2004-04-28 by dhkwon
#include "StoreData.h"	// Itemtable, enchant, store
#include "QuestData.h"	// Quest, CharacterQuest °ü¸®

#include "EnemyData.h"
#include "INFQuest.h"
#include "MonsterData.h"
#include "AtumSJ.h"
#include "Background.h"
#include "SceneData.h"
#include "KeyBoardInput.h"
#include "JoyStickInput.h"					// 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
#include "FieldWinSocket.h"
#include "INFGameMain.h"
#include "Camera.h"
#include "AtumDatabase.h"
#include "INFGameMainUnitInfoBar.h"
//#include "ObjectRender.h"
#include "AtumSound.h"
#include "Chat.h"
#include "RangeTime.h"
#include "INFUnitState.h"
#include "INFGameMainUnitInfoBar.h"
#include "Weapon.h"
#include "WeaponShieldData.h"
#include "ObjectChild.h"
#include "INFWeaponInfo.h"
//#include "INFNotice.h"
//#include "TraceRender.h"
// 2007-11-22 by bhsohn ľĆ·ąłŞ ĹëÇŐĽ­ąö
//#include "IMSocket.h"
#include "IMSocketManager.h"

#include "Interface.h"
#include "INFWindow.h"
//#include "MonsterRender.h"
//#include "INFShop.h"
#include "INFTarget.h"
#include "INFTrade.h"
#include "WeaponMineData.h"
#include "ItemInfo.h"
#include "Cinema.h"
#include "INFCityBase.h"
#include "INFCityWarp.h"
// 2008-03-19 by bhsohn Ep3żÉĽÇĂ˘
//#include "INFSystem.h"
#include "INFOptionSystem.h"

#include "WeaponItemInfo.h"
#include "Skill.h"
#include "UnitRender.h"
#include "ClientParty.h"
#include "ObjRender.h"
#include "MonRender.h"
#include "INFCommunity.h"
#include "INFCommunityGuild.h"
#include "TickTimeProcess.h"
#include "INFSkill.h"
#include "SkillEffect.h"
#include "dxutil.h"
#include "PkNormalTimer.h"
#include "AtumDefine.h"
#include "INFMp3Player.h"
#include "TutorialSystem.h"
#include "CharacterChild.h"
#include "INFMissionInfo.h"
#include "GlobalImage.h"

#include "INFSelect.h"

#include "ObjectAniData.h"
#include "EffectRender.h"
#include "INFGameArena.h"
#include "INFOpMain.h"
#include "INFMissionMain.h" // 2008-12-09 by dgwoo ąĚĽÇ¸¶˝şĹÍ.

// 2009-02-25 by bhsohn Á¶ŔĚ˝şĆ˝ ŔĚżë, ¸Ţ´şĂ˘ ¶ěżě¸é A±âľî´Â ¸¶żě˝ş·Î Á¶Ŕý
#include "INFCharacterInfoExtend.h"
#include "INFInvenExtend.h"
// end 2009-02-25 by bhsohn Á¶ŔĚ˝şĆ˝ ŔĚżë, ¸Ţ´şĂ˘ ¶ěżě¸é A±âľî´Â ¸¶żě˝ş·Î Á¶Ŕý

#include "INFInfinity.h"		// 2010. 07. 27 by hsLee ŔÎÇÇ´ĎĆĽ 2Â÷ °ĹÁˇ ąćľî ˝Ăł×¸¶ ż¬Ăâ ˝şĹµ Ăł¸®.

#include "PetManager.h"	// 2010-06-15 by shcho&hslee Ćę˝Ă˝şĹŰ - Ćę °ř°Ý Ăł¸®
#include "TimeMgr.h" // 2011-08-12 by hsson Ćí´ë ´ëż­żˇ Ăß°ˇ/Ĺ»Ĺđ ˝Ă µô·ąŔĚ¸¦ »đŔÔ (Ćí´ë µż±âČ­ ąö±×¸¦ ÁŮŔÓ)
#define SELECT_MENU_ITEM_SHIFT(x)		(WEAPON_BODYCON_LOADING << (x*10))//10 = ±âľî°ˇ Áő°ˇÇĎ¸éĽ­ bodyconditionŔĚ Áő°ˇÇĎ´Â °Ş
#include "INFUnitNameInfo.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define TUTORIAL_MISSION_1			1
#define TUTORIAL_MISSION_2			2
#define TUTORIAL_MISSION_3			3
#define TUTORIAL_MISSION_4			4
#define TUTORIAL_MISSION_11			11

// 2008-07-07 by dgwoo GetAmorGearHeight() şŻ°ć.
//#define SHUTTLE_HEIGHT				7.087f

#define OVERHEAT_REPAIR_TIME		10.0f	// 10ĂĘ
#define UNIT_STOP_DELAY				2.0f	// 2ĂĘ			// 2005-09-26 by ispark

#define AIR_SIDERATE_MAX			0.7f
#define AIR_UPDOWNRATE_MAX			0.5f

//////////////////////////////////////////////////////////////////////
// Âř·ú˝Ă ŔĚµż°ü·Ă Á¤ŔÇą® 
// ydkim
// 2004.6.7
//////////////////////////////////////////////////////////////////////
#define KEYBOARD_INPUT_TIMER		0.5f
#define MOVEFRAME_TIMER		        0.5f
#define SIGEMODE_MOUSE_FITPOINT     50.0f
#define A_GEAR_SIGEMODE_MOVETIMING  0.1f
#define ANIMATION_UNIT_TIMING		0.01f

#define SHUTTLE_CONNER_MOVE_SPEED	30

#define ANI_UPDOWN_FLAG				1
#define ANI_LEFT_FLAG				2
#define ANI_RIGHT_FLAG				3

#define ANI_REFLEXION_LEFT_FLAG		4
#define ANI_REFLEXION_RIGHT_FLAG	5

#define REFLEXION_TILE_MIN_SIZE		1

///////////////////////////////////////////////////////////////////////
///		˝şĹł °ü·Ă Á¤ŔÇą®
///		jschoi
///		2006.06.26
///////////////////////////////////////////////////////////////////////

#define TARGET_ON		1
#define TARGET_OFF		2

// Âř·úŔĺ ĆĐĹĎ
#define AIR_PORT_LANDING_PATTERN_NONE	0
#define AIR_PORT_LANDING_PATTERN_START	1
#define AIR_PORT_LANDING_PATTERN_END	2
#define AIR_PORT_TAKEOFF_PATTERN_START	3
#define AIR_PORT_TAKEOFF_PATTERN_END	4

#define AIR_PORT_MIN_SPEED				150.0f

// Ćí´ë ´ëÇü˝Ă ŔĚµż ąüŔ§
#define FORMATION_UPDOWN_RANGE			0.1f		
#define FORMATION_SIDE_RANGE			0.1f		
#define FORMATION_MOVE_RATE				20

// 2007-04-02 by bhsohn MoveĆĐĹ¶Ŕ¸·Î Ŕű±â »óĹÂ ĂĽĹ©
#define	UNIT_CHG_STATE_TIME				0.5f
/*
//originals
#define		AGEAR_SIEGEMODE_MAX_ANGLE_RADIAN			0.266f			// ĂÖ´ë 15µµ*DegToRad
#define		AGEAR_SIEGEMODE_MIN_ANGLE_RADIAN			2.0943f 		// ĂÖĽŇ 120µµ*DegToRad
#define		AGEAR_SIEGEMODE_MIN_CAP						0.001f

// 2007-05-15 by bhsohn A±âľî ş¸żĎ
#define		AGEAR_LANDING_SPEED							45.0f			// Âř·ú ĽÓµµ
#define		AGEAR_RASING_SPEED							60.0f			// ŔĚ·ú ĽÓµµ
#define		AGEAR_RASING_TIME							0.4f			// ŔĚ·ú ˝Ă°Ł
*/
// 2007-04-13 by bhsohn A±âľî Ć÷´ëą®Á¦
#define		AGEAR_SIEGEMODE_MAX_ANGLE_RADIAN			0.266f			// ĂÖ´ë 15µµ*DegToRad
#define		AGEAR_SIEGEMODE_MIN_ANGLE_RADIAN			2.0943f 		// ĂÖĽŇ 120µµ*DegToRad
#define		AGEAR_SIEGEMODE_MIN_CAP						0.001f 			

// 2007-05-15 by bhsohn A±âľî ş¸żĎ
#define		AGEAR_LANDING_SPEED							45.0f			// Âř·ú ĽÓµµ
#define		AGEAR_RASING_SPEED							60.0f			// ŔĚ·ú ĽÓµµ
#define		AGEAR_RASING_TIME							0.4f			// ŔĚ·ú ˝Ă°Ł

// 2007-12-05 by bhsohn ş®żˇ ¶Ő°í µéľî°ˇ´Â Çö»ó Ăł¸®
// ĂÖ´ë °˘µµ 10µµ
#define		MAX_LADING_DO							10.0f			

// 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
#define		JOYSTIC_STEP_MAX_VALUE					(1000.0f)

// 2009-02-19 by bhsohn Ä«¸Ţ¶ó ˝ĂĽ± şŻ°ćČÄ, ˝±°Ô »çłÉ ąćąý ¸·Ŕ˝
#define		ATTACK_POSSIBLE_DISTANCE				(20.0f)
// end 2009-02-19 by bhsohn Ä«¸Ţ¶ó ˝ĂĽ± şŻ°ćČÄ, ˝±°Ô »çłÉ ąćąý ¸·Ŕ˝

// 2009-03-16 by bhsohn A±âľî Ć÷´ë ąćÇâ ÇÁ·ÎĹäÄÝ ĂÖĽŇČ­
#define		SEND_MOVE_VEL_CHECK_TIME				(1000)	// 1ĂĘżˇ ÇŃąřľż ş¸ł˝´Ů.
// end 2009-03-16 by bhsohn A±âľî Ć÷´ë ąćÇâ ÇÁ·ÎĹäÄÝ ĂÖĽŇČ­

// 2009-03-30 by bhsohn Â÷ÂˇĽ¦ ŔĚĆĺĆ® ąö±× ĽöÁ¤
#define		BGEAR_CHAGINGSHOT_SHOT_CAP_TIME			(1.2f)	
// end 2009-03-30 by bhsohn Â÷ÂˇĽ¦ ŔĚĆĺĆ® ąö±× ĽöÁ¤

// 2010. 03. 25 by ckPark »ç¸Á˝Ă 5ĂĘŔĚ»ó Ăß¶ôÇĎ¸é şÎČ°¸ŢĽĽÁö Ăß°ˇ
#define		FALLING_TIME	5000
// end 2010. 03. 25 by ckPark »ç¸Á˝Ă 5ĂĘŔĚ»ó Ăß¶ôÇĎ¸é şÎČ°¸ŢĽĽÁö Ăß°ˇ
#ifdef _INET_PET
#define		MAX_PETUSETITEM_CAP		300
#endif
#ifdef _DEBUG
void TempDataRenderLib() {}
#endif // _DEBUG_endif

CShuttleChild::CShuttleChild()
{
	FLOG( "CShuttleChild()" );
	m_pGameData = NULL;
	g_pShuttleChild = this;
	m_pCharacterInfo = NULL;
	m_pRender = g_pD3dApp->m_pUnitRender;
	m_dwPartType = _SHUTTLE;
    m_vBasisUp = D3DXVECTOR3(0.0f, 1.0f, 0.0f);			// Up Vector
	D3DXVECTOR3 vPos = D3DXVECTOR3(150.0f,30.0f,150.0f);
	SetPos(vPos);
	D3DXVec3Normalize(&m_vVel,&m_vVel);
    D3DXVec3Cross(&m_vSideVel,&m_vUp,&m_vVel);          
	//m_dwState = _NORMAL;
	m_fSideRate = 0.0f;									// ÁÂżě Č¸Ŕü ĽÓµµ Á¶Ŕý
	m_fSideCheckRate = 0.0f;
	m_fCurrentTime = 0.0f;
	memset(&m_myShuttleInfo,0x00,sizeof(CHARACTER));
	
	// 2007-05-15 by bhsohn ±âľî ˝şĹČ °ü·Ă Ăł¸®
	memset(&m_myShuttleFullStatInfo,0x00,sizeof(GEAR_STAT));	// ŔüĂĽ ˝şĹČÁ¤ş¸ 	

	m_fAxisRate = 0.0f;
	m_nUDState = 0;

	m_bGetOtherInfo = FALSE;
	m_fGetOtherInfoCheckTime = 0.0f;
	m_nGetOtherIndex = 0;

	m_bColl = FALSE;
	m_vCollPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_fCheckCollTime = 0.3f;
	m_pMoveChat = NULL;
	m_pMoveChat = new CAtumNode;
	m_pIDChat = NULL;
	m_pIDChat = new CAtumNode;

	m_nBooster = BOOSTER_TYPE_STOP;
	m_nHyperBooster = HYPER_BOOSTER_NORMAL;
	m_vOldPos = D3DXVECTOR3(-1.0f, -1.0f, -1.0f);// edited by dhkwon 2003.8.20

	m_bIsUnderWater = FALSE;
	m_bIsWater = FALSE;

	m_fLRVelRate = 0.0f;

	m_bCollMap = FALSE;
	m_bOnObject = TRUE;

	m_bIsAir = FALSE;
	m_vNormal = m_vUp;

	m_fHeightRate = 0.0f;
#ifdef  _OLD_SHADOW_SYSTEM
	m_bShadowIsRender = TRUE;
#endif
	m_bSkillMoveIsUse = FALSE;
	m_bSkillMoveType = 0;
	m_nCurrentPatternNumber = 0;

	m_bCollShuttleCheck = 0;
	m_bCollMonsterCheck = 0;
	m_bCollObjectCheck = 0;
	m_bCollAttackCheck = 0;

	m_fObjectSize = 15.0f;

	m_bTurnCamera = FALSE;
	m_pTarget = NULL;
	m_pOrderTarget = NULL;

	ChangeEngine(0,0);

	m_bCheckBlend = FALSE;

	m_bEffectGround = 0;
	m_fExpPer = 0.0f;

	m_bMoveGroundType = 0;

	m_fDTEventCheckTime = 0.0f;

	m_bMoveInputType = 0;
	m_fShuttleSpeed = 0;
	m_fShuttleSpeedMax = 0;
	m_fShuttleSpeedMin = 0;
	m_fShuttleSpeedBoosterOn = 0;
	m_bySpeedPenalty = 0;								// 2005-08-16 by ispark
	m_fBoosterCheckTime = BOOSTER_KEY_CHECK_TIME;
	m_fChangeBodyForBoosterCheckTime = 0.2f;
	m_nShuttleBoosterState = 0;
	m_nShuttleStopState = 0;
	m_fNumpad0keyTime = 2.0f;							// 2006-05-30 by ispark, 1.0f -> 2.0f
	m_fCancelSkillCheckTime = 0.0f;

	m_fMouseLRMoveRate = 0.0f;
	m_fMouseUDMoveRate = 0.0f;

	m_fDefenseTotal = 0.0f;	
	m_fDefenseForItem = 0.0f;	
	m_fDefenseForSkill = 0.0f;	
	m_fDefenseForShuttle = 0.0f;
	m_fTransportTotal = 0.0f;	
	m_fTransportCurrent = 0.0f;

	int i;
	for(i=0;i<MAX_CHAT_PTOP_CONT;i++)
		memset(m_strChatPToP[i],0x00,SIZE_MAX_CHARACTER_NAME);
	m_bCurPToPPos = 0;
	m_bOldPToPPos = 200;

	m_bRButtonState = FALSE;
	m_bLButtonState = FALSE;
	m_bMButtonState = FALSE;
	m_bReChargeBURN = TRUE;
	m_bUseBooster = TRUE;

	m_bUpButtonState = FALSE;
	m_bDownButtonState = FALSE;
	m_bLeftButtonState = FALSE;
	m_bRightButtonState = FALSE;

			
	m_fBURN = 1.0f;
	m_fCurrentBURN = 0.0f;

	m_ptOldPoint.x = 0;
	m_ptOldPoint.y = 0;
	m_bFirstStart = FALSE;

	m_bAutoLockon = FALSE;
	m_bAutoHeightControl = FALSE;

//	m_bEventReady = FALSE;

	m_fWeaponMaxDistance = 0.0f;

	m_nMonsterCount = 0;
	m_bAttackMode = _AIR;
//	m_fEventCheckTime = 2.0f;


	m_fNextHP = 0.0f;
	m_fNextEP = 0.0f;
	m_fNextSP = 0.0f;
	m_fNextDP = 0.0f;
	m_fNextHPRate = 0.0f;
	m_fNextEPRate = 0.0f;
	m_fNextSPRate = 0.0f;
	m_fNextDPRate = 0.0f;

	m_bIsSetFlighting = FALSE;
	m_fCollSendDamageCheckTime = 0.0f;

	m_vMouseDir = D3DXVECTOR3(0,0,0);
	m_fMouseRate = 0.5f;
	m_pt.x = 0;
	m_pt.y = 0;
	m_ptDir.x = 0;
	m_ptDir.y = 0;

	m_fOldUpRate = 0.0f;
	m_vCollCenter = D3DXVECTOR3(0,0,0);
	m_bBeginnerQuestDegree = 0;

	m_bMouseLock = FALSE;

	m_bOrderMove = FALSE;
	m_bOrderMoveType = FALSE;
	m_vTargetPos = D3DXVECTOR3(0,0,0);
	m_vTargetVel = D3DXVECTOR3(1,0,0);
	m_fLandingCheckTime = 0.0f;

	m_fRasingCheckTime = 0.0f;

	m_pStoreData = NULL;
	m_fKeyboardInputTimer = 0;

	m_fFrameLerp          = 0;							// ŔĚµż ľÖ´Ď¸ŢŔĚĽÇ ·ŻÇÁ
	m_fAniFlag            = 0;							// ŔĚµż ľÖ´Ď¸ŢŔĚĽÇ ÇĂ·ą±×
	m_bAniKeyBlock        = FALSE;						// ľÖ´Ď¸ŢŔĚĽÇÁß Ĺ°şí·°°Ş
	m_bAgearPortflag	  = FALSE;						// A ±âľî Âř·úŔĺ ÇĂ·ą±×
	m_bReflexionKeyBlock  = FALSE;						// ş®żˇşÎµđÄĄ¶§ Ĺ°şí·° °Ş 
	m_bLandingOldCamtypeFPS		 = FALSE;

	m_fAnimationMoveTimer		 = ANIMATION_UNIT_TIMING;
	m_fSinMove = 0;
	m_dwLastAttackTick			= 0;		
	m_bIsCameraPattern = FALSE;				// 2004.06.28 jschoi Ä«¸Ţ¶óĆĐĹĎ »çżëÁßŔÎ°ˇ?
	m_pCinemaUnitPattern = NULL;			// 2004.06.17 jschoi ŔŻ´Ö(˝şĹł,ż¬Ăâ) ĆĐĹĎ
	m_pCinemaCamera = NULL;					// 2005.06.17 jschoi ˝şĹł Ä«¸Ţ¶ó
	m_nEventType = EVENT_WARP_IN;
	m_pCurrentObjEvent = NULL;
	m_pCurrentObjEventOld = NULL;

	m_pPrimaryWeapon = NULL;
	m_pSecondaryWeapon = NULL;

	m_pSkill = NULL;
	m_pClientParty = NULL;

	m_pGuildWarInfo = NULL;
	m_fWarpOutDistance = 0;

	// 2010-06-15 by shcho&hslee Ćę˝Ă˝şĹŰ - ŔĺÂřÇ° ĂÖ´ë °łĽö°Ş şŻ°ć.
	//memset(	&m_wearItemParamFactor, 0x00, sizeof(WEAR_ITEM_PARAM_FACTOR)*WEAR_ITEM_NUMBER );
	memset(	&m_wearItemParamFactor, 0x00, sizeof(WEAR_ITEM_PARAM_FACTOR)*WEAR_ITEM_NUMBER );
	memset(	&m_wearPreRareItemParamFactor, 0x00, sizeof(WEAR_ITEM_PARAM_FACTOR)*MAX_EQUIP_POS );
	memset(	&m_wearSufRareItemParamFactor, 0x00, sizeof(WEAR_ITEM_PARAM_FACTOR)*MAX_EQUIP_POS );
	// End 2010-06-15 by shcho&hslee Ćę˝Ă˝şĹŰ - ŔĺÂřÇ° ĂÖ´ë °łĽö°Ş şŻ°ć.
	
	m_nOldMapIndex = 0;
//#ifdef _DEBUG
//	m_nDebugWarpMove = 0;
//	m_nDebugSendPacketMove = 0;
//	m_fDebugWarpCheckTime = 0;
//	m_fDebugSendCheckTime = 0;
//#endif
//	m_fMissileFireTime = 0;
	
	// 2005-02-15 by jschoi  -  PK Delay Timer
	m_pPkNormalTimer = new CPkNormalTimer;
	
	m_bCtrlKey = FALSE;

	m_bWarpGateZone = FALSE;
	
	m_byCityWarTeamType = CITYWAR_TEAM_TYPE_NORMAL;
	m_fCurrentAngle = 0;
	m_fCurrentSideRate = 0;

	m_bMouseMoveLock = FALSE;				// ±âş»ŔűŔ¸·Î ¸·Ŕ˝	// 2005-07-05 by ispark

	// 2005-07-05 by ispark
	// ·Ń¸µ ˝Ă˝şĹŰ ĂĘ±âČ­
	InitRolling();

	m_nEventTypeAirPort = AIR_PORT_LANDING_PATTERN_NONE;
	m_nEventIndex = 0;
	m_bKeyBlock = FALSE;
	m_bWarpLink = FALSE;
	m_bUnitStop = FALSE;
	m_bPenaltyUnitStop = FALSE;			// 2005-08-09 by ispark

	m_pRadarItemInfo = NULL;			// 2005-08-16 by ispark
	m_bLandingField = FALSE;
	m_bSkipPattern = FALSE;
	m_bAttack = TRUE;					//°ř°ÝŔĚ °ˇ´ÉÇŃ ¸đµĺ·Î ĽĽĆĂ.
	InitUnitStop();
	m_fRotationX = SHUTTLE_ROTATION_DEFAULT_X;
	m_fRotationY = 0.0f;
	m_fRotationZ = SHUTTLE_ROTATION_DEFAULT_Z;
	m_fRotationA = 0.0f;	
	
	// 2008-08-22 by bhsohn EP3 ŔÎşĄĹä¸® Ăł¸®
	//D3DXMatrixIdentity(&m_pMatInven);
	//m_bInvenRender = FALSE;

	m_pTexturesBackInven = NULL;
	m_bRenderInven = FALSE;

	m_bSkillAirMove = FALSE;
	m_fCheckWearItemTime = 0.0f;
	m_nCheckDamage = 0;

	m_fAdvanceTime = 0.0f;
	m_fCheckDead = 0.0f;

	m_bObserve = FALSE;
	m_bOperation = FALSE;
	// 2008-08-22 by bhsohn EP3 ŔÎşĄĹä¸® Ăł¸®
//	for(i = 0; i < 4; i++)
//	{
//		D3DXMatrixIdentity(&m_pMatInvenWeaponSetPosition[i]);
//		D3DXMatrixIdentity(&m_pMatInvenWeaponOrgPosition[i]);
//	}
	// end 2008-08-22 by bhsohn EP3 ŔÎşĄĹä¸® Ăł¸®

	m_nArmorColorIndex = 0;
	m_nInvenArmorColorIndex = 0;

	// 2007-11-09 by dgwoo ą«±â ¸Ţ˝¬¸¦ ąčż­żˇĽ­ ş¤ĹÍ·Î şŻ°ć.
	m_vectInvenWeaponMesh.clear();
	for(i = 0 ; i < OBSERVE_MAX_QUICK ; i++)
	{
		m_ObserveQuick[i] = 0;
	}

	m_byFormationFlightType = 0;

	// 2007-04-17 by bhsohn °ˇ±îżî °Ĺ¸®żˇĽ­ŔÇ ˝ĂÁî¸đµĺ˝Ă µĄŔĚÁö ľČµéľî°ˇ´Â Çö»ó Ăł¸®
	m_bAGearFollowEnemy = FALSE;

	m_fUnitX = m_fUnitY = m_fUnitZ = m_fScaling = 0;
	m_bNowMousePosWindowArea = TRUE;
	m_bOldMousePosWindowArea = TRUE;	
	m_fAmorHeight = 7.087f;

	// 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
	m_bSetCursorPos = FALSE;
	m_nJoyStickRolling = 0;	

	// 2009-03-16 by bhsohn A±âľî Ć÷´ë ąćÇâ ÇÁ·ÎĹäÄÝ ĂÖĽŇČ­
	m_dwCheckTimeSendMoveVel = 0;
	// end 2009-03-16 by bhsohn A±âľî Ć÷´ë ąćÇâ ÇÁ·ÎĹäÄÝ ĂÖĽŇČ­

	// 2009-03-30 by bhsohn Â÷ÂˇĽ¦ ŔĚĆĺĆ® ąö±× ĽöÁ¤
	m_fDelCapChagingSkill = 0.0f;
	// end 2009-03-30 by bhsohn Â÷ÂˇĽ¦ ŔĚĆĺĆ® ąö±× ĽöÁ¤

	// 2010. 03. 25 by ckPark »ç¸Á˝Ă 5ĂĘŔĚ»ó Ăß¶ôÇĎ¸é şÎČ°¸ŢĽĽÁö Ăß°ˇ
	m_bDeadMessageEnable	= TRUE;

	m_dwDeadTime	= 0;
	// end 2010. 03. 25 by ckPark »ç¸Á˝Ă 5ĂĘŔĚ»ó Ăß¶ôÇĎ¸é şÎČ°¸ŢĽĽÁö Ăß°ˇ
	// 2010. 05. 27 by jskim ˝Ăł×¸¶ Ŕűżë Ä«¸Ţ¶ó ±¸Çö
	SetShowCinema ( FALSE );
	//end 2010. 05. 27 by jskim ˝Ăł×¸¶ Ŕűżë Ä«¸Ţ¶ó ±¸Çö

	
	// 2011-06-30 by jhahn  ŔÎÇÇ3Â÷ ˝Ăł×¸¶ ˝şĹµ±ÝÁö
	m_InfiState		= TRUE;
	//end 2011-06-30 by jhahn  ŔÎÇÇ3Â÷ ˝Ăł×¸¶ ˝şĹµ±ÝÁö
#ifdef _INET_PET
	// 2012-09-20 by jhahn ŔÎşńÁöşíżˇĽ­ ĆÄĆ®łĘ ľĆŔĚĹŰ »çżë°ˇ´É
	HpCharge = FALSE;				 //2011-10-06 by jhahn ĆÄĆ®łĘ ĽşŔĺÇü ˝Ă˝şĹŰ
	ShieldCharge = FALSE;			 //2011-10-06 by jhahn ĆÄĆ®łĘ ĽşŔĺÇü ˝Ă˝şĹŰ
	SpCharge = FALSE;				 //2011-10-06 by jhahn ĆÄĆ®łĘ ĽşŔĺÇü ˝Ă˝şĹŰ
	//end 2012-09-20 by jhahn ŔÎşńÁöşíżˇĽ­ ĆÄĆ®łĘ ľĆŔĚĹŰ »çżë°ˇ´É
	m_dwPetUseItemTime_Spell = 0;
	m_dwPetUseItemTime_Hp = 0;
	m_dwPetUseItemTime_Dp = 0;
	m_dwPetUseItemTime_Sp = 0;
#endif
	m_bLendSoundPlay = FALSE;

	vectPurifyUsed.clear();
}

CShuttleChild::~CShuttleChild()
{
	FLOG( "~CShuttleChild()" );

	SAFE_DELETE(m_pGameData);

	if(m_pCharacterInfo)//Ăß°ˇ
	{
		m_pCharacterInfo->InvalidateDeviceObjects();
		m_pCharacterInfo->DeleteDeviceObjects();
		SAFE_DELETE(m_pCharacterInfo);
	}

	SAFE_DELETE( m_pCinemaUnitPattern );			// 2004.06.17 jschoi ˝şĹł ĆĐĹĎ
	SAFE_DELETE( m_pCinemaCamera );			// 2004.06.17 jschoi ˝şĹł Ä«¸Ţ¶ó

	SAFE_DELETE( m_pPrimaryWeapon );
	SAFE_DELETE( m_pSecondaryWeapon );

	SAFE_DELETE(m_pStoreData);	// m_pPrimaryWeapon, m_pSecondaryWeapon µÚżˇ Áöżöľß ÇŃ´Ů.

	SAFE_DELETE(m_pSkill);		// 2004-07-23 jschoi ˝şĹł
	SAFE_DELETE(m_pClientParty);	// 2004-08-19 jschoi ĆÄĆĽ

	SAFE_DELETE(m_pGuildWarInfo);
#ifdef  _OLD_SHADOW_SYSTEM
//	InvalidateShadow();
#endif
	SAFE_DELETE(m_pPkNormalTimer);
	
	g_pShuttleChild = NULL;
	m_bTargetChange = FALSE;

	// 2007-11-09 by dgwoo ą«±â ¸Ţ˝¬¸¦ ąčż­żˇĽ­ ş¤ĹÍ·Î şŻ°ć.
	m_vectInvenWeaponMesh.clear();
}

void CShuttleChild::SetPos(const D3DXVECTOR3 &vPos)
{
	FLOG( "CShuttleChild::SetPos(const D3DXVECTOR3 &vPos)" );
	m_vPos = vPos;
}

HRESULT CShuttleChild::InitDeviceObjects()
{
	FLOG( "CShuttleChild::InitDeviceObjects()" );
	if(m_pIDChat && !m_pIDChat->m_pChild)
	{

		DWORD col = COLOR_CHARACTER_ID00;

		if(m_myShuttleInfo.PCBangTotalPlayTime > 1000)
			col = g_pGameMain->m_pUnitNameInfo->GetInfluenceColor(m_myShuttleInfo.InfluenceType, m_myShuttleInfo.Race, m_myShuttleInfo.PCBangTotalPlayTime);


		CChatMoveData * pData = new CChatMoveData(m_pIDChat,m_myShuttleInfo.CharacterName, col,9,256,32);
		pData->InitDeviceObjects(); // Restore´Â CShuttleChild::RestoreDeviceObjects()żˇĽ­ ÇŃ´Ů.
		pData = (CChatMoveData *)m_pIDChat->AddChild(pData);
	}

	// remodling, 2004-03-21 by dhkwon
	m_pStoreData = new CStoreData;
	m_pSkill = new CSkill;
	m_pClientParty = new CClientParty;

	char strPath[256];
	g_pD3dApp->LoadPath( strPath, IDS_DIRECTORY_TEXTURE, "select.tex");
	SetResourceFile(strPath);
	
	DataHeader	* pDataHeader = NULL;
	pDataHeader = FindResource("inven");
//	m_pImgBack->InitDeviceObjects(pDataHeader->m_pData,pDataHeader->m_DataSize) ;
	// 2014-05-20 by ymjoo 텍스처 로딩 익셉션 예외 처리
	if (pDataHeader)
	{
		D3DXCreateTextureFromFileInMemory(g_pD3dDev, pDataHeader->m_pData, pDataHeader->m_DataSize,
			&m_pTexturesBackInven);
	}
	// END 2014-05-20 by ymjoo 텍스처 로딩 익셉션 예외 처리

//	D3DUtil_CreateTexture( g_pD3dDev, pDataHeader->m_pData, &m_pTexturesBackInven );
	// 2010-06-15 by shcho&hslee Ćę˝Ă˝şĹŰ - Ćę °ř°Ý Ăł¸®
	m_pPetManager	= new CPetManager;
	//end 2010-06-15 by shcho&hslee Ćę˝Ă˝şĹŰ - Ćę °ř°Ý Ăł¸®

	return S_OK;
}

HRESULT CShuttleChild::SetResourceFile(char* szFileName)
{
	FLOG( "CShuttleChild::SetResourceFile(char* szFileName)" );
	SAFE_DELETE(m_pGameData);
	m_pGameData = new CGameData;
	if(!m_pGameData->SetFile(szFileName, FALSE, NULL, 0))
	{
		SAFE_DELETE(m_pGameData);
		return E_FAIL;
	}

	return S_OK;
}

DataHeader * CShuttleChild::FindResource(char* szRcName)
{
	FLOG( "CShuttleChild::FindResource(char* szRcName)" );
	DataHeader* pHeader = NULL;
	if(m_pGameData)
	{
		pHeader = m_pGameData->Find(szRcName);
	}
	return pHeader;
}

HRESULT CShuttleChild::RestoreDeviceObjects()
{
	if(m_pIDChat)
		m_pIDChat->RestoreDeviceObjects();
	if(m_pMoveChat)
		m_pMoveChat->RestoreDeviceObjects();
#ifdef  _OLD_SHADOW_SYSTEM
	//RestoreShadow();
#endif
	return CUnitData::RestoreDeviceObjects();
}

/*
void CShuttleChild::RestoreChat()
{
	FLOG( "CShuttleChild::RestoreChat()" );
	if(m_pIDChat)
		m_pIDChat->RestoreDeviceObjects();
	if(m_pMoveChat)
		m_pMoveChat->RestoreDeviceObjects();
#ifdef  _OLD_SHADOW_SYSTEM
	RestoreShadow();
#endif
}
*/
//void CShuttleChild::InvalidateChat()
//{
//	FLOG( "CShuttleChild::InvalidateChat()" );
//	if(m_pIDChat)
//		m_pIDChat->InvalidateDeviceObjects();
//	if(m_pMoveChat)
//		m_pMoveChat->InvalidateDeviceObjects();
#ifdef  _OLD_SHADOW_SYSTEM
//	InvalidateShadow();
#endif
//}
HRESULT CShuttleChild::InvalidateDeviceObjects()
{
	FLOG( "CShuttleChild::InvalidateChat()" );
	if(m_pIDChat)
		m_pIDChat->InvalidateDeviceObjects();
	if(m_pMoveChat)
		m_pMoveChat->InvalidateDeviceObjects();

	return CUnitData::InvalidateDeviceObjects();
}

HRESULT CShuttleChild::DeleteDeviceObjects()
{
	FLOG( "CShuttleChild::DeleteDeviceObjects()" );

	SAFE_DELETE(m_pGameData);
	if(m_pIDChat)// && m_pIDChat->m_pChild)
	{
		m_pIDChat->DeleteDeviceObjects();
		SAFE_DELETE(m_pIDChat);
	}
	if(m_pMoveChat)
	{
		m_pMoveChat->DeleteDeviceObjects();
		SAFE_DELETE(m_pMoveChat);
	}
	if(m_pCharacterInfo)//Ăß°ˇ
	{
		m_pCharacterInfo->InvalidateDeviceObjects();
		m_pCharacterInfo->DeleteDeviceObjects();
		SAFE_DELETE(m_pCharacterInfo);
	}

	SAFE_DELETE(m_pSkill);
	SAFE_DELETE(m_pClientParty);
	// »čÁ¦Ŕü ąĚ¸® ŔĚĆĺĆ® Ć÷ŔÎĆ® ĂĽĹ©
//	if(m_pWeapon1_1)
//	{
//		m_pWeapon1_1->m_bUsing = FALSE;
//		m_pWeapon1_1 = NULL;
//	}
	if(m_pWeapon1_1_1)
	{
		m_pWeapon1_1_1->m_bUsing = FALSE;
		m_pWeapon1_1_1 = NULL;
	}
	if(m_pWeapon1_1_2)
	{
		m_pWeapon1_1_2->m_bUsing = FALSE;
		m_pWeapon1_1_2 = NULL;
	}
	if(m_pWeapon1_2)
	{
		m_pWeapon1_2->m_bUsing = FALSE;
		m_pWeapon1_2 = NULL;
	}
//	if(m_pWeapon2_1)
//	{
//		m_pWeapon2_1->m_bUsing = FALSE;
//		m_pWeapon2_1 = NULL;
//	}
	if(m_pWeapon2_1_1)
	{
		m_pWeapon2_1_1->m_bUsing = FALSE;
		m_pWeapon2_1_1 = NULL;
	}
	if(m_pWeapon2_1_2)
	{
		m_pWeapon2_1_2->m_bUsing = FALSE;
		m_pWeapon2_1_2 = NULL;
	}
	if(m_pWeapon2_2)
	{
		m_pWeapon2_2->m_bUsing = FALSE;
		m_pWeapon2_2 = NULL;
	}

	// 2009. 07. 07 by ckPark ·Îşż±âľî żäĂ»»çÇ×(·Ń¸µ, Ľ±ĹĂČ­¸é, ą«±â, A±âľîĆ÷´ë)
	if( m_pRobotAGearWeapon_1 )
	{
		m_pRobotAGearWeapon_1->m_bUsing		= FALSE;
		m_pRobotAGearWeapon_1				= NULL;
	}
	// end 2009. 07. 07 by ckPark ·Îşż±âľî żäĂ»»çÇ×(·Ń¸µ, Ľ±ĹĂČ­¸é, ą«±â, A±âľîĆ÷´ë)

	if(m_pEngine)
	{
		m_pEngine->m_bUsing = FALSE;
		m_pEngine = NULL;
	}
	if(m_pRadar)
	{
		m_pRadar->m_bUsing = FALSE;
		m_pRadar = NULL;
	}
//	if(m_pArmor)
//	{
//		m_pArmor->m_bUsing = FALSE;
//		m_pArmor = NULL;
//	}
	if(m_pContainer)
	{
		m_pContainer->m_bUsing = FALSE;
		m_pContainer = NULL;
	}
	if(m_pAccessories)
	{
		m_pAccessories->m_bUsing = FALSE;
		m_pAccessories = NULL;
	}
	// 2010-06-08 by dgwoo, Ćę˝Ă˝şĹŰ Ăß°ˇ. 
	if(m_pPartner)
	{
		m_pPartner->m_bUsing = FALSE;
		m_pPartner = NULL;
	}
	if(m_pPartner1)
	{
		m_pPartner1->m_bUsing = FALSE;
		m_pPartner1 = NULL;
	}

	if(m_pDummyPartner)
	{
		m_pDummyPartner->m_bUsing = FALSE;
		m_pDummyPartner = NULL;
	}
	
	if(m_pDummyPartner1)
	{
		m_pDummyPartner1->m_bUsing = FALSE;
		m_pDummyPartner1 = NULL;
	}
	// 2010-06-08 by dgwoo, Ćę˝Ă˝şĹŰ Ăß°ˇ. 
	if(m_pWingIn)
	{
		m_pWingIn->m_bUsing = FALSE;
		m_pWingIn = NULL;
	}	
	SAFE_RELEASE( m_pTexturesBackInven );

	// 2007-11-09 by dgwoo ą«±â ¸Ţ˝¬¸¦ ąčż­żˇĽ­ ş¤ĹÍ·Î şŻ°ć.
	m_vectInvenWeaponMesh.clear();

	// 2010-06-15 by shcho&hslee Ćę˝Ă˝şĹŰ - Ćę °ř°Ý Ăł¸®
	if(m_pPetManager)
		SAFE_DELETE(m_pPetManager);
	//end 2010-06-15 by shcho&hslee Ćę˝Ă˝şĹŰ - Ćę °ř°Ý Ăł¸®
	return S_OK;
}


void CShuttleChild::UnitInfoBarSmoothRepair(float fElapsedTime)
{
	// HP - Č¸şą˝Ă Ľ­Ľ­Č÷ şÎµĺ·´°Ô Č¸şą
	if(m_fNextHP > m_myShuttleInfo.CurrentHP)
	{
		m_myShuttleInfo.CurrentHP += m_fNextHPRate*fElapsedTime;
		if(m_myShuttleInfo.CurrentHP > m_fNextHP)
			m_myShuttleInfo.CurrentHP = m_fNextHP;
		if(g_pGameMain->m_pUnitInfoBar)
		{
			g_pGameMain->m_pUnitInfoBar->SetHP(m_myShuttleInfo.CurrentHP, (float)m_myShuttleInfo.HP);
		}
	}
	// EP - Č¸şą˝Ă Ľ­Ľ­Č÷ şÎµĺ·´°Ô Č¸şą
	if(m_fNextEP > m_myShuttleInfo.CurrentEP)
	{
		m_myShuttleInfo.CurrentEP += m_fNextEPRate*fElapsedTime;
		if(m_myShuttleInfo.CurrentEP > m_fNextEP)
			m_myShuttleInfo.CurrentEP = m_fNextEP;
		if(g_pGameMain->m_pUnitInfoBar)
		{
			g_pGameMain->m_pUnitInfoBar->SetEP(m_myShuttleInfo.CurrentEP, (float)m_myShuttleInfo.EP);
		}
	}
	// SP - Č¸şą˝Ă Ľ­Ľ­Č÷ şÎµĺ·´°Ô Č¸şą
	if(m_fNextSP > m_myShuttleInfo.CurrentSP)
	{// SPŔÇ °ćżě SHORT ÇüŔĚ±â ¶§ą®żˇ ľĆ·ˇŔÇ ÄÚµĺ Ăß°ˇ
		float fTempSP = m_myShuttleInfo.CurrentSP;
		fTempSP += m_fNextSPRate*fElapsedTime;
		m_myShuttleInfo.CurrentSP = (SHORT)fTempSP;
		if((float)m_myShuttleInfo.CurrentSP < fTempSP)
			m_myShuttleInfo.CurrentSP++;
		
		if(m_myShuttleInfo.CurrentSP > m_fNextSP)
			m_myShuttleInfo.CurrentSP = m_fNextSP;
		if(g_pGameMain->m_pUnitInfoBar)
		{
			g_pGameMain->m_pUnitInfoBar->SetSP((float)m_myShuttleInfo.CurrentSP, (float) m_myShuttleInfo.SP);
		}
	}
	// HP - Č¸şą˝Ă Ľ­Ľ­Č÷ şÎµĺ·´°Ô Č¸şą
	if(m_fNextDP > m_myShuttleInfo.CurrentDP)
	{
		m_myShuttleInfo.CurrentDP += m_fNextDPRate*fElapsedTime;
		if(m_myShuttleInfo.CurrentDP > m_fNextDP)
			m_myShuttleInfo.CurrentDP = m_fNextDP;
		if(g_pGameMain->m_pUnitInfoBar)
		{
			g_pGameMain->m_pUnitInfoBar->SetDP(m_myShuttleInfo.CurrentDP, (float)m_myShuttleInfo.DP);
		}
	}
}

void CShuttleChild::TurnOnWall(float fElapsedTime)
{
	// 2005-02-02 by jschoi - żěÁÖ¸ĘŔş ŔĚ ÇÔĽö Ŕűżë ľČµĘ.
	// 2007-01-04 by bhsohn ą«şę ˝şĹł·Î ¸Ę Ĺ¸ŔĎżˇ şÎµúÇűŔ» ‹šżˇ ´ëÇŃ ąö±×Ăł¸®
	//if(IS_SPACE_MAP_INDEX(m_myShuttleInfo.MapChannelIndex.MapIndex) == TRUE)
	// żěÁÖ¸Ę ĆÇ´ÜŔş ŔĚÁ¦ Ĺ¸ŔĎ¸ĘŔĚ ŔÖ°í ľřłÄ·Î ĆÇ´ÜÇŃ´Ů.
	if(FALSE == IsTileMapTickEnable(g_pShuttleChild->m_myShuttleInfo.MapChannelIndex.MapIndex))
	{
		return;
	}

	if (m_dwState == _FALLEN || m_dwState == _FALLING)
		return;

	// Ĺ¸ŔĎ Á¤ş¸ ĂŁ±â 
	D3DXVECTOR3 Vp = D3DXVECTOR3(0,1,0);				// ŔŻ´ÖŔÇ ľŐµŢ¸éŔ» ĂŁ±âŔ§ÇŃ ÇÔĽö 
	float ads = D3DXVec3Dot(&Vp, &m_vUp);	

	int nmyXCheck = (int)(m_vPos.x / TILE_SIZE);
	int nmyYCheck = (int)(m_vPos.z / TILE_SIZE);
	
	int max_x = ((int)g_pGround->m_projectInfo.sXSize - REFLEXION_TILE_MIN_SIZE);
	int max_y = ((int)g_pGround->m_projectInfo.sYSize - REFLEXION_TILE_MIN_SIZE);
	
//	if(m_fShuttleSpeed > 0)
	// 2005-08-01 by ispark
	// ¸Ę ş®żˇ Ăćµą˝Ă ¸¶żě˝ş°ˇ ŔĚ»óÇŃ Çŕµż ąö±× ĽöÁ¤ ĂćµąČÄ ¶łľîÁłŔ» ¶§ else ĽŇ˝ş şÎşĐ »çżë ľČÇÔ
	if(m_dwState == _FALLEN || m_dwState == _FALLING)
	{
		m_bReflexionKeyBlock = FALSE;
	}
	else
	{
		if(m_nCurrentPatternNumber == 0)
		{
			if( nmyXCheck <= REFLEXION_TILE_MIN_SIZE+10 )
			{ // Ĺ¸ŔĎłˇ °ć°í ¸ŢĽĽÁö
				g_pGImage->SetImageInfo(OUT_ZONE);
			}
			else if( nmyYCheck <= REFLEXION_TILE_MIN_SIZE+10 )
			{ // Ĺ¸ŔĎłˇ °ć°í ¸ŢĽĽÁö
				g_pGImage->SetImageInfo(OUT_ZONE);
			}
			else if( nmyXCheck >= max_x-10 )
			{ // Ĺ¸ŔĎłˇ °ć°í ¸ŢĽĽÁö
				g_pGImage->SetImageInfo(OUT_ZONE);
			}
			else if( nmyYCheck >= max_y-10 )
			{ // Ĺ¸ŔĎłˇ °ć°í ¸ŢĽĽÁö
				g_pGImage->SetImageInfo(OUT_ZONE);
			}
		}

		if( nmyXCheck <= REFLEXION_TILE_MIN_SIZE )// XĂŕĹ¸ŔĎŔĚ 1 ş¸´Ů ŔŰŔ» °ćżě  
		{
			D3DXVECTOR3 BottomVec = D3DXVECTOR3(0,0,-1);
			float reS = D3DXVec3Dot(&m_vVel, &BottomVec);
			
			POINT pt,pt2;
			int nX;
			GetCursorPos(&pt);
			pt2 = pt;
			ScreenToClient(g_pD3dApp->GetHwnd(), &pt);
			CheckMouseReverse(&pt);
			
			if(reS*100 <= -1)
			{
				if(IS_DT(m_myShuttleInfo.UnitKind) || IS_OT(m_myShuttleInfo.UnitKind))
				{
						if(ads > 0 )
							m_fAniFlag = ANI_REFLEXION_RIGHT_FLAG;	// ĽĹĆ˛ŔÇ ľŐµŢ¸éŔ» ±¸şĐÇŘĽ­ ¸®ÇĂ·şĽÇ ąćÇâĽłÁ¤ 
						else 
							m_fAniFlag = ANI_REFLEXION_LEFT_FLAG;

//						m_fShuttleSpeed = SHUTTLE_CONNER_MOVE_SPEED;			// 2005-07-11 by ispark	// ľ˛Áö ľĘľĆĽ­ Áöżň
						
						nX = pt2.x - pt.x;
						nX += g_pD3dApp->GetBackBufferDesc().Width/2 + 100*g_pD3dApp->GetBackBufferDesc().Width/800;
				}
				else
				{
					if(ads > 0) 
						m_fAniFlag = ANI_REFLEXION_RIGHT_FLAG;	// ĽĹĆ˛ŔÇ ľŐµŢ¸éŔ» ±¸şĐÇŘĽ­ ¸®ÇĂ·şĽÇ ąćÇâĽłÁ¤ 
					else 
						m_fAniFlag = ANI_REFLEXION_LEFT_FLAG;
						
					nX = pt2.x - pt.x;
					nX += g_pD3dApp->GetBackBufferDesc().Width/2 + 100*g_pD3dApp->GetBackBufferDesc().Width/800;			
				}
				
			}
			else                    
			{
				if(IS_DT(m_myShuttleInfo.UnitKind) || IS_OT(m_myShuttleInfo.UnitKind))
				{
					
						if(ads > 0 )m_fAniFlag = ANI_REFLEXION_LEFT_FLAG;	// ĽĹĆ˛ŔÇ ľŐµŢ¸éŔ» ±¸şĐÇŘĽ­ ¸®ÇĂ·şĽÇ ąćÇâĽłÁ¤ 
							else m_fAniFlag = ANI_REFLEXION_RIGHT_FLAG;
							
//					if(m_fShuttleSpeed < 0) m_fShuttleSpeed = -20;				// 2005-07-11 by ispark	// ľ˛Áö ľĘľĆĽ­ Áöżň
//					else m_fShuttleSpeed = SHUTTLE_CONNER_MOVE_SPEED;
						nX = pt2.x - pt.x;
						nX += g_pD3dApp->GetBackBufferDesc().Width/2 - 100*g_pD3dApp->GetBackBufferDesc().Width/800;
					
				}
				else
				{
					if(ads > 0 )m_fAniFlag = ANI_REFLEXION_LEFT_FLAG;	// ĽĹĆ˛ŔÇ ľŐµŢ¸éŔ» ±¸şĐÇŘĽ­ ¸®ÇĂ·şĽÇ ąćÇâĽłÁ¤ 
							else m_fAniFlag = ANI_REFLEXION_RIGHT_FLAG;
							
					nX = pt2.x - pt.x;
					nX += g_pD3dApp->GetBackBufferDesc().Width/2 - 100*g_pD3dApp->GetBackBufferDesc().Width/800;
				}
			}
			// 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
			m_bSetCursorPos = TRUE;

			SetCursorPos(nX,pt2.y);		
			SetUnitMovingAnimation(fElapsedTime);
//			m_bReflexionKeyBlock = TRUE;								// 2006-09-27 by ispark
		}
		else if( nmyYCheck <= REFLEXION_TILE_MIN_SIZE )// ZĂŕĹ¸ŔĎŔĚ 1 ş¸´Ů ŔŰŔ» °ćżě  
		{
			D3DXVECTOR3 BottomVec = D3DXVECTOR3(1,0,0);
			float reS = D3DXVec3Dot(&m_vVel, &BottomVec);
			
			POINT pt,pt2;
			int nX;
			GetCursorPos(&pt);
			pt2 = pt;
			ScreenToClient(g_pD3dApp->GetHwnd(), &pt);
			CheckMouseReverse(&pt);
			
			if(reS*100 <= -1)
			{
				if(IS_DT(m_myShuttleInfo.UnitKind) || IS_OT(m_myShuttleInfo.UnitKind))
				{
						if(ads > 0 )m_fAniFlag = ANI_REFLEXION_RIGHT_FLAG;	// ĽĹĆ˛ŔÇ ľŐµŢ¸éŔ» ±¸şĐÇŘĽ­ ¸®ÇĂ·şĽÇ ąćÇâĽłÁ¤ 
							else m_fAniFlag = ANI_REFLEXION_LEFT_FLAG;

//					if(m_fShuttleSpeed < 0) m_fShuttleSpeed = -20;				// 2005-07-11 by ispark	// ľ˛Áö ľĘľĆĽ­ Áöżň
//					else m_fShuttleSpeed = SHUTTLE_CONNER_MOVE_SPEED;

						nX = pt2.x - pt.x;
						nX += g_pD3dApp->GetBackBufferDesc().Width/2 + 100*g_pD3dApp->GetBackBufferDesc().Width/800;
				}
				else
				{
					if(ads > 0 )m_fAniFlag = ANI_REFLEXION_RIGHT_FLAG;	// ĽĹĆ˛ŔÇ ľŐµŢ¸éŔ» ±¸şĐÇŘĽ­ ¸®ÇĂ·şĽÇ ąćÇâĽłÁ¤ 
						else m_fAniFlag = ANI_REFLEXION_LEFT_FLAG;

					nX = pt2.x - pt.x;
					nX += g_pD3dApp->GetBackBufferDesc().Width/2 + 100*g_pD3dApp->GetBackBufferDesc().Width/800;
				}
			}
			else 
			{
				if(IS_DT(m_myShuttleInfo.UnitKind) || IS_OT(m_myShuttleInfo.UnitKind))
				{
						if(ads > 0 )m_fAniFlag = ANI_REFLEXION_LEFT_FLAG;	// ĽĹĆ˛ŔÇ ľŐµŢ¸éŔ» ±¸şĐÇŘĽ­ ¸®ÇĂ·şĽÇ ąćÇâĽłÁ¤ 
							else m_fAniFlag = ANI_REFLEXION_RIGHT_FLAG;
							
//					if(m_fShuttleSpeed < 0) m_fShuttleSpeed = -20;				// 2005-07-11 by ispark	// ľ˛Áö ľĘľĆĽ­ Áöżň
//					else m_fShuttleSpeed = SHUTTLE_CONNER_MOVE_SPEED;

						nX = pt2.x - pt.x;
						nX += g_pD3dApp->GetBackBufferDesc().Width/2 - 100*g_pD3dApp->GetBackBufferDesc().Width/800;
						
				}
				else
				{
					if(ads > 0 )m_fAniFlag = ANI_REFLEXION_LEFT_FLAG;	// ĽĹĆ˛ŔÇ ľŐµŢ¸éŔ» ±¸şĐÇŘĽ­ ¸®ÇĂ·şĽÇ ąćÇâĽłÁ¤ 
							else m_fAniFlag = ANI_REFLEXION_RIGHT_FLAG;
							
					nX = pt2.x - pt.x;
					nX += g_pD3dApp->GetBackBufferDesc().Width/2 - 100*g_pD3dApp->GetBackBufferDesc().Width/800;
				}
			}		
			// 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
			m_bSetCursorPos = TRUE;

			SetCursorPos(nX,pt2.y);		
			SetUnitMovingAnimation(fElapsedTime);
//			m_bReflexionKeyBlock = TRUE;							// 2006-09-27 by ispark	
		}
		else if( nmyXCheck >= max_x )// XĂŕĹ¸ŔĎŔ» 255 łŃ±ć°ćżě 
		{
			D3DXVECTOR3 BottomVec = D3DXVECTOR3(0,0,1);
			float reS = D3DXVec3Dot(&m_vVel, &BottomVec);
			
			POINT pt,pt2;
			int nX;
			GetCursorPos(&pt);
			pt2 = pt;
			ScreenToClient(g_pD3dApp->GetHwnd(), &pt);
			CheckMouseReverse(&pt);
			
			if(reS*100 <= -1)
			{
//				DBGOUT("Dot(%f)\n",reS);
				if(IS_DT(m_myShuttleInfo.UnitKind) || IS_OT(m_myShuttleInfo.UnitKind))
				{
						if(ads > 0 )m_fAniFlag = ANI_REFLEXION_RIGHT_FLAG;	// ĽĹĆ˛ŔÇ ľŐµŢ¸éŔ» ±¸şĐÇŘĽ­ ¸®ÇĂ·şĽÇ ąćÇâĽłÁ¤ 
							else m_fAniFlag = ANI_REFLEXION_LEFT_FLAG;
						
//					if(m_fShuttleSpeed < 0) m_fShuttleSpeed = -20;				// 2005-07-11 by ispark	// ľ˛Áö ľĘľĆĽ­ Áöżň
//					else m_fShuttleSpeed = SHUTTLE_CONNER_MOVE_SPEED;
					
						nX = pt2.x - pt.x;
						nX += g_pD3dApp->GetBackBufferDesc().Width/2 + 100*g_pD3dApp->GetBackBufferDesc().Width/800;
				}
				else
				{
					if(ads > 0 ) m_fAniFlag = ANI_REFLEXION_RIGHT_FLAG;	// ĽĹĆ˛ŔÇ ľŐµŢ¸éŔ» ±¸şĐÇŘĽ­ ¸®ÇĂ·şĽÇ ąćÇâĽłÁ¤ 
						else m_fAniFlag = ANI_REFLEXION_LEFT_FLAG;

					nX = pt2.x - pt.x;
					nX += g_pD3dApp->GetBackBufferDesc().Width/2 + 100*g_pD3dApp->GetBackBufferDesc().Width/800;
				}
			}
			else 
			{
				if(IS_DT(m_myShuttleInfo.UnitKind) || IS_OT(m_myShuttleInfo.UnitKind))
				{
						if(ads > 0 )m_fAniFlag = ANI_REFLEXION_LEFT_FLAG;	// ĽĹĆ˛ŔÇ ľŐµŢ¸éŔ» ±¸şĐÇŘĽ­ ¸®ÇĂ·şĽÇ ąćÇâĽłÁ¤ 
							else m_fAniFlag = ANI_REFLEXION_RIGHT_FLAG;
							
//					if(m_fShuttleSpeed < 0) m_fShuttleSpeed = -20;				// 2005-07-11 by ispark	// ľ˛Áö ľĘľĆĽ­ Áöżň
//					else m_fShuttleSpeed = SHUTTLE_CONNER_MOVE_SPEED;
					
						nX = pt2.x - pt.x;
						nX += g_pD3dApp->GetBackBufferDesc().Width/2 - 100*g_pD3dApp->GetBackBufferDesc().Width/800;
				}
				else
				{
					if(ads > 0 )m_fAniFlag = ANI_REFLEXION_LEFT_FLAG;	// ĽĹĆ˛ŔÇ ľŐµŢ¸éŔ» ±¸şĐÇŘĽ­ ¸®ÇĂ·şĽÇ ąćÇâĽłÁ¤ 
							else m_fAniFlag = ANI_REFLEXION_RIGHT_FLAG;
							
					nX = pt2.x - pt.x;
					nX += g_pD3dApp->GetBackBufferDesc().Width/2 - 100*g_pD3dApp->GetBackBufferDesc().Width/800;
				}
			}
			// 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
			m_bSetCursorPos = TRUE;

			SetCursorPos(nX,pt2.y);		
			SetUnitMovingAnimation(fElapsedTime);
//			m_bReflexionKeyBlock = TRUE;								// 2006-09-27 by ispark
//			DBGOUT("Dot(%f)\n",reS);
		}
		else if( nmyYCheck >= max_y )// ZĂŕĹ¸ŔĎŔ» 255 łŃ±ć°ćżě 
		{
			D3DXVECTOR3 BottomVec = D3DXVECTOR3(-1,0,0);
			float reS = D3DXVec3Dot(&m_vVel, &BottomVec);
			
			POINT pt,pt2;
			int nX;
			GetCursorPos(&pt);
			pt2 = pt;
			ScreenToClient(g_pD3dApp->GetHwnd(), &pt);
			CheckMouseReverse(&pt);
			
			if(reS*100 <= -1)
			{
				if(IS_DT(m_myShuttleInfo.UnitKind) || IS_OT(m_myShuttleInfo.UnitKind))
				{
						if(ads > 0 )m_fAniFlag = ANI_REFLEXION_RIGHT_FLAG;	// ĽĹĆ˛ŔÇ ľŐµŢ¸éŔ» ±¸şĐÇŘĽ­ ¸®ÇĂ·şĽÇ ąćÇâĽłÁ¤ 
							else m_fAniFlag = ANI_REFLEXION_LEFT_FLAG;
	
//					if(m_fShuttleSpeed < 0) m_fShuttleSpeed = -20;				// 2005-07-11 by ispark	// ľ˛Áö ľĘľĆĽ­ Áöżň
//					else m_fShuttleSpeed = SHUTTLE_CONNER_MOVE_SPEED;
					
						nX = pt2.x - pt.x;
						nX += g_pD3dApp->GetBackBufferDesc().Width/2 + 100*g_pD3dApp->GetBackBufferDesc().Width/800;
				}
				else
				{
					if(ads > 0 )m_fAniFlag = ANI_REFLEXION_RIGHT_FLAG;	// ĽĹĆ˛ŔÇ ľŐµŢ¸éŔ» ±¸şĐÇŘĽ­ ¸®ÇĂ·şĽÇ ąćÇâĽłÁ¤ 
						else m_fAniFlag = ANI_REFLEXION_LEFT_FLAG;

					nX = pt2.x - pt.x;
					nX += g_pD3dApp->GetBackBufferDesc().Width/2 + 100*g_pD3dApp->GetBackBufferDesc().Width/800;
				}
			}
			else 
			{
				if(IS_DT(m_myShuttleInfo.UnitKind) || IS_OT(m_myShuttleInfo.UnitKind))
				{
						if(ads > 0 )m_fAniFlag = ANI_REFLEXION_LEFT_FLAG;	// ĽĹĆ˛ŔÇ ľŐµŢ¸éŔ» ±¸şĐÇŘĽ­ ¸®ÇĂ·şĽÇ ąćÇâĽłÁ¤ 
							else m_fAniFlag = ANI_REFLEXION_RIGHT_FLAG;
							
//					if(m_fShuttleSpeed < 0) m_fShuttleSpeed = -20;				// 2005-07-11 by ispark	// ľ˛Áö ľĘľĆĽ­ Áöżň
//					else m_fShuttleSpeed = SHUTTLE_CONNER_MOVE_SPEED;
					
						nX = pt2.x - pt.x;
						nX += g_pD3dApp->GetBackBufferDesc().Width/2 - 100*g_pD3dApp->GetBackBufferDesc().Width/800;
				}
				else
				{
					if(ads > 0 )m_fAniFlag = ANI_REFLEXION_LEFT_FLAG;	// ĽĹĆ˛ŔÇ ľŐµŢ¸éŔ» ±¸şĐÇŘĽ­ ¸®ÇĂ·şĽÇ ąćÇâĽłÁ¤ 
							else m_fAniFlag = ANI_REFLEXION_RIGHT_FLAG;
							
					nX = pt2.x - pt.x;
					nX += g_pD3dApp->GetBackBufferDesc().Width/2 - 100*g_pD3dApp->GetBackBufferDesc().Width/800;
				}
			}
			if(m_fAniFlag)
			{ 
				// 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
				m_bSetCursorPos = TRUE;

				SetCursorPos(nX,pt2.y);		
				SetUnitMovingAnimation(fElapsedTime);
//				m_bReflexionKeyBlock = TRUE;						// 2006-09-27 by ispark
			}
//			DBGOUT("Dot(%f)\n",reS);
		}
		else
		{
			m_bReflexionKeyBlock = FALSE;		
		}
	}

}
void CShuttleChild::TickCity(float fElapsedTime)
{
	switch(m_dwState)
	{
	case _NORMAL:
		break;
	case _LANDING:
		{
			EVENT_POINT UnitPoint;
			BOOL bResult = FALSE;
			if(m_pCinemaUnitPattern != NULL)
			{
				bResult = m_pCinemaUnitPattern->SkillTick();
				UnitPoint=m_pCinemaUnitPattern->GetCurrentCinemaPoint();
				m_vPos = UnitPoint.vPosition;				// Ŕ§Äˇ
				m_vVel = UnitPoint.vDirection;				// ąćÇâ
				m_vUp = UnitPoint.vUpVector;
				m_vTargetPosition = UnitPoint.vTarget;			
				int nPatternType = m_pCinemaUnitPattern->GetHeader().nPatternType;
				switch(nPatternType)
				{
				case TARGET_ON:	// Ĺ¸ÄĎ ąćÇâŔĚ ŔÖ´Â ĆĐĹĎ
					m_vVel = m_vTargetPosition - m_vPos;
					D3DXVec3Normalize(&m_vVel,&m_vVel);	
					break;
				case TARGET_OFF: // Ĺ¸ÄĎ ąćÇâŔĚ ľř´Â ĆĐĹĎ
					break;
				}

			}
			if( m_vPos.y <= 19 )//bResult)
			{
				ChangeSingleBodyCondition(BODYCON_LANDED_MASK);
				m_vPos = MAP_TYPE_CITY_UNIT_POS;
				m_vVel.y = 0;
				DeletePattern();
				ChangeUnitState(_LANDED);
				SendPacket(T_FC_MOVE_LANDING_DONE);
				DBGOUT("T_FC_MOVE_LANDING_DONE\n");
				// 2007-01-02 by bhsohn A±âľî b+Enter·Î ¸ŘĂá ČÄ, Âř·úÇĎ¸é ľČ żňÁ÷ż©Áö´Â Çö»ó Ăł¸®
				if(IS_DT(m_myShuttleInfo.UnitKind))
				{
					// A±âľî ¶ó¸é żňÁ÷ŔÓ ĂĘ±âČ­
					InitUnitStop();
				}
				// end 2007-01-02 by bhsohn A±âľî b+Enter·Î ¸ŘĂá ČÄ, Âř·úÇĎ¸é ľČ żňÁ÷ż©Áö´Â Çö»ó Ăł¸®
			}
		}
		break;
	case _LANDED:
		{
		}
		break;
	case _TAKINGOFF:
		{
			EVENT_POINT UnitPoint;
			BOOL bResult = FALSE;
			if(m_pCinemaUnitPattern != NULL)
			{
				bResult = m_pCinemaUnitPattern->SkillTick();
				UnitPoint=m_pCinemaUnitPattern->GetCurrentCinemaPoint();
				m_vPos = UnitPoint.vPosition;				// Ŕ§Äˇ
				m_vVel = UnitPoint.vDirection;				// ąćÇâ
				m_vUp = UnitPoint.vUpVector;
				m_vTargetPosition = UnitPoint.vTarget;			
				int nPatternType = m_pCinemaUnitPattern->GetHeader().nPatternType;
				switch(nPatternType)
				{
				case TARGET_ON:	// Ĺ¸ÄĎ ąćÇâŔĚ ŔÖ´Â ĆĐĹĎ
					m_vVel = m_vTargetPosition - m_vPos;
					D3DXVec3Normalize(&m_vVel,&m_vVel);	
					break;
				case TARGET_OFF: // Ĺ¸ÄĎ ąćÇâŔĚ ľř´Â ĆĐĹĎ
					break;
				}

			}
			if(bResult)
			{
				if(m_nEventType == EVENT_CITY_OUTDOOR)
				{
					g_pInterface->m_pCityBase->SendEventRequestWarpToOutdoor();
				}
				else if(m_nEventType == EVENT_CITY_OUT_MOVE)
				{
					CINFCityWarp* pCityWarp = g_pInterface->m_pCityBase->GetCityWarp();
					ASSERT_ASSERT(pCityWarp);
					pCityWarp->SendFieldSocketRequestShopWarp();
				}
				DeletePattern();
				ChangeUnitState(_NORMAL);
			}
		}
		break;
	}
	TickBurn(fElapsedTime);
	CheckAniTime();
	D3DXMatrixLookAtRH( &m_mMatrix, &m_vPos, &(m_vPos + m_vVel), &m_vUp);
	D3DXMatrixInverse( &m_mMatrix, NULL, &m_mMatrix );
	if(m_pCharacterInfo)
	{
		m_pCharacterInfo->SetAllBodyConditionMatrix( m_mMatrix );
		m_pCharacterInfo->Tick(fElapsedTime);
	}
#ifdef  _OLD_SHADOW_SYSTEM
	CheckShadowPos();
#endif
	// 2005-03-18 by jschoi - For ToolTip
	GetCursorPos(&m_pt);
	ScreenToClient(g_pD3dApp->GetHwnd(), &m_pt);				
	CheckMouseReverse(&m_pt);


//	const int nKeyMove = 1; // Ĺ°ş¸µĺ °¨µµ
//	if(g_pD3dApp->m_pKeyBoard->GetAsyncKeyState(DIK_LEFT))
//	{
//		g_pCamera->SetCameraRotate(nKeyMove,0);
//	}
//	if(g_pD3dApp->m_pKeyBoard->GetAsyncKeyState(DIK_RIGHT))
//	{
//		g_pCamera->SetCameraRotate(-nKeyMove,0);
//	}
//	if(g_pD3dApp->m_pKeyBoard->GetAsyncKeyState(DIK_UP))
//	{
//		g_pCamera->SetCameraRotate(0,nKeyMove);
//	}
//	if(g_pD3dApp->m_pKeyBoard->GetAsyncKeyState(DIK_DOWN))
//	{
//		g_pCamera->SetCameraRotate(0,-nKeyMove);
//	}
//	if(g_pD3dApp->m_pKeyBoard->GetAsyncKeyState(DIK_PRIOR))
//	{
//		g_pCamera->SetCameraZoomInOut(nKeyMove);
//	}
//	if(g_pD3dApp->m_pKeyBoard->GetAsyncKeyState(DIK_NEXT))
//	{
//		g_pCamera->SetCameraZoomInOut(-nKeyMove);
//	}


}

void CShuttleChild::MoveLandingStation(float fElapsedTime)
{
	
	if(g_pTutorial->IsTutorialMode() == TRUE &&
		//g_pTutorial->GetLesson() != L1
		g_pTutorial->GetChapterState() == TUTORIAL_STATE_READY)
		return;

	int			nXCheck, nYCheck;
	if(m_bKeyBlock)		return;		// Âř·úÁßŔĚ¸é Ĺ°şí·°
	
	m_fKeyboardInputTimer -= fElapsedTime;         // Ĺ°ŔÔ·Â Ĺ¸ŔĚ¸Ó 

	// 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
//	if( (g_pD3dApp->m_pKeyBoard->GetAsyncKeyState(DIK_S)		  // Ĺ°ŔÔ·Â˝Ăżˇ¸¸ °č»ę (ÇĘżäľř´Â µô·ąŔĚ »čÁ¦)     
//			|| g_pD3dApp->m_pKeyBoard->GetAsyncKeyState(DIK_W)
//			|| g_pD3dApp->m_pKeyBoard->GetAsyncKeyState(DIK_A)
//			|| g_pD3dApp->m_pKeyBoard->GetAsyncKeyState(DIK_D)) 
	if( (g_pD3dApp->GetAsyncKeyState_DIK_DIJ(DIK_S)		  // Ĺ°ŔÔ·Â˝Ăżˇ¸¸ °č»ę (ÇĘżäľř´Â µô·ąŔĚ »čÁ¦)     
			|| g_pD3dApp->GetAsyncKeyState_DIK_DIJ(DIK_W)
			|| g_pD3dApp->GetAsyncKeyState_DIK_DIJ(DIK_A)
			|| g_pD3dApp->GetAsyncKeyState_DIK_DIJ(DIK_D)) 
		&& m_bReflexionKeyBlock == FALSE 
		&& m_dwState != _SHOP)
	{	
		nXCheck = (int)(m_vPos.x / TILE_SIZE);
		nYCheck = (int)(m_vPos.z / TILE_SIZE);

		if (nXCheck < 0)
			nXCheck = 0;

		if (nYCheck < 0)
			nYCheck = 0;

		m_bMoveGroundType = g_pGround->m_pTileInfo[nXCheck*g_pGround->m_projectInfo.sXSize + nYCheck].bEnableLand;			// Ĺ¸ŔĎÁ¤ş¸°ˇ Âř·úŔĺŔĎ¶§¸¸ ˝ÇÇŕ 				

		if(m_bMoveGroundType == 1)
		{				
			if(m_fKeyboardInputTimer < 0)
			{
				m_fKeyboardInputTimer = KEYBOARD_INPUT_TIMER;
				// 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
				//if(!g_pD3dApp->m_bChatMode && g_pD3dApp->m_pKeyBoard->GetAsyncKeyState(DIK_S) && !m_bAniKeyBlock)
				if(!g_pD3dApp->m_bChatMode && g_pD3dApp->GetAsyncKeyState_DIK_DIJ(DIK_S) && !m_bAniKeyBlock)
				{   // µÚ·Î ŔĚµż
					//m_vPos.x = (int(m_vPos.x/TILE_SIZE))*TILE_SIZE + TILE_SIZE/2;								// ŔÚ˝ĹŔÇ Áß˝ÉÁˇ Ŕ§Äˇ °č»ę
					//m_vPos.z = (int(m_vPos.z/TILE_SIZE))*TILE_SIZE + TILE_SIZE/2;
					
					D3DXVECTOR3 temp;

					temp = m_vPos - m_vVel * TILE_SIZE;
					temp.x = (int(temp.x/TILE_SIZE))*TILE_SIZE + TILE_SIZE/2;								// ŔÚ˝ĹŔÇ Áß˝ÉÁˇ Ŕ§Äˇ °č»ę
					temp.z = (int(temp.z/TILE_SIZE))*TILE_SIZE + TILE_SIZE/2;
					
					D3DXVec3Normalize(&m_vVel, &(m_vPos - temp));

					m_bAniKeyBlock = TRUE;
					m_vAniFrame = m_vPos;
					m_vAniFrame = m_vPos - m_vVel * TILE_SIZE;
					m_vAniFrame.x = (int(m_vAniFrame.x/TILE_SIZE))*TILE_SIZE + TILE_SIZE/2;
					m_vAniFrame.z = (int(m_vAniFrame.z/TILE_SIZE))*TILE_SIZE + TILE_SIZE/2;							
					
					nXCheck = (int)(m_vAniFrame.x / TILE_SIZE);
					nYCheck = (int)(m_vAniFrame.z / TILE_SIZE);			
					if (nXCheck < 0)
						nXCheck = 0;

					if (nYCheck < 0)
						nYCheck = 0;
					m_bMoveGroundType = g_pGround->m_pTileInfo[nXCheck*g_pGround->m_projectInfo.sXSize + nYCheck].bEnableLand;								
					
					BOOL Een = ScanEnemyFrontBAckAndSpace();
					if(m_bMoveGroundType == 1 && !Een)
					{	
						m_fAniFlag = ANI_UPDOWN_FLAG;
						m_vMyOldPos = m_vPos;									
					}
					else
					{
						m_bAniKeyBlock = FALSE;
					}
					//else if(IS_DT(m_myShuttleInfo.UnitKind))
					//{
					//	m_fAniFlag = 1;
					//	m_vMyOldPos = m_vPos;
					//}
				}
				// 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
				//else if(!g_pD3dApp->m_bChatMode && g_pD3dApp->m_pKeyBoard->GetAsyncKeyState(DIK_W) && !m_bAniKeyBlock)
				else if(!g_pD3dApp->m_bChatMode && g_pD3dApp->GetAsyncKeyState_DIK_DIJ(DIK_W) && !m_bAniKeyBlock)
				{   // ľŐŔ¸·Î ŔĚµż
					
					D3DXVECTOR3 temp;

					temp = m_vPos + m_vVel * TILE_SIZE;
					temp.x = (int(temp.x/TILE_SIZE))*TILE_SIZE + TILE_SIZE/2;								// ŔÚ˝ĹŔÇ Áß˝ÉÁˇ Ŕ§Äˇ °č»ę
					temp.z = (int(temp.z/TILE_SIZE))*TILE_SIZE + TILE_SIZE/2;
					
					D3DXVec3Normalize(&m_vVel, &(temp - m_vPos));
					
					m_bAniKeyBlock = TRUE;
					m_vAniFrame = m_vPos;								
					m_vAniFrame = m_vAniFrame + m_vVel * TILE_SIZE;
					m_vAniFrame.x = (int(m_vAniFrame.x/TILE_SIZE))*TILE_SIZE + TILE_SIZE/2;
					m_vAniFrame.z = (int(m_vAniFrame.z/TILE_SIZE))*TILE_SIZE + TILE_SIZE/2;
					
					nXCheck = (int)(m_vAniFrame.x / TILE_SIZE);
					nYCheck = (int)(m_vAniFrame.z / TILE_SIZE);			

					if (nXCheck < 0)
						nXCheck = 0;

					if (nYCheck < 0)
						nYCheck = 0;

					m_bMoveGroundType = g_pGround->m_pTileInfo[nXCheck*g_pGround->m_projectInfo.sXSize + nYCheck].bEnableLand;
					
					BOOL Een = ScanEnemyFrontBAckAndSpace();
					if(m_bMoveGroundType == 1 && !Een) // °íĂÄľßÇŇşÎşĐ A±âľî °ü·Ă ŔĚµż 
					{
						m_fAniFlag = ANI_UPDOWN_FLAG;
						m_vMyOldPos = m_vPos;									
					}
					else
					{
						m_bAniKeyBlock = FALSE;
					}
					//else if(m_myShuttleInfo.UnitKind == 256)
					//{
					//	m_fAniFlag = 1;
					//	m_vMyOldPos = m_vPos;
					//}
					
				}
				// 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
				//else if(!g_pD3dApp->m_bChatMode && g_pD3dApp->m_pKeyBoard->GetAsyncKeyState(DIK_A) && !m_bAniKeyBlock)
				else if(!g_pD3dApp->m_bChatMode && g_pD3dApp->GetAsyncKeyState_DIK_DIJ(DIK_A) && !m_bAniKeyBlock)
				{   // ÁÂ·Î Č¸Ŕü	
					m_bAniKeyBlock = TRUE;
					m_fAniFlag = ANI_LEFT_FLAG;	
					SetTargetVectorforLandedMove();
					
				}
				// 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
				//else if(!g_pD3dApp->m_bChatMode && g_pD3dApp->m_pKeyBoard->GetAsyncKeyState(DIK_D) && !m_bAniKeyBlock)
				else if(!g_pD3dApp->m_bChatMode && g_pD3dApp->GetAsyncKeyState_DIK_DIJ(DIK_D) && !m_bAniKeyBlock)
				{   // żě·Î ŔĚµż
					m_bAniKeyBlock = TRUE;
					m_fAniFlag = ANI_RIGHT_FLAG;	
					SetTargetVectorforLandedMove();
				}
									
			}
		}
	}
	if(m_fAniFlag) SetUnitMovingAnimation(fElapsedTime);			
}


void CShuttleChild::TickBurn(float fElapsedTime)
{
	ASSERT_ASSERT(m_fBURN>0.0f);
	
	// 2006-07-05 by ispark, ÇÁ¸®ąĚľö °łłä şŻ°ć
	// 2006-07-06 by ispark, ą«ÇŃ şÎ˝şĹÍ »čÁ¦
//	if(	m_myShuttleInfo.CurrentEP > 0 && 
//		g_pD3dApp->GetPrimiumCardInfo()->nCardItemNum1 )
//	{
//		// 2005-03-28 by jschoi - ż¬·á°ˇ ŔÖ´Â ˝´ĆŰ ÇÁ¸®ąĚľö »çżëŔÚ´Â ą«ÇŃ şÎ˝şĹÍ
//		m_fCurrentBURN = m_fBURN;
//		if(g_pGameMain->m_pUnitInfoBar)
//		{
//			g_pGameMain->m_pUnitInfoBar->SetBURN(m_fCurrentBURN, m_fBURN);
//		}
//		m_bReChargeBURN = TRUE;
//		m_bUseBooster = TRUE;
//		return;
//	}
	
//	float fOldBURN = m_fCurrentBURN;
	if(m_nBooster == BOOSTER_TYPE_MAX && m_nHyperBooster == HYPER_BOOSTER_NORMAL)
	{
		if(m_fCurrentBURN > 0.0f)
		{
			m_fCurrentBURN -= fElapsedTime;
			if(g_pGameMain->m_pUnitInfoBar)
			{
				g_pGameMain->m_pUnitInfoBar->SetBURN(m_fCurrentBURN, m_fBURN); 
			}
		}
		if(m_fCurrentBURN < 0.0f)
		{
			m_fCurrentBURN = 0.0f;
			m_bReChargeBURN = FALSE;
		}
	}
	else
	{
//		if(!m_bReChargeBURN)
//		{
//			m_fCurrentBURN += (m_fBURN*fElapsedTime)/BOOSTER_OVERHEAT_REPAIR_TIME;
//		}
//		else
//		{
//			m_fCurrentBURN += fElapsedTime/2.0f;
//		}
		if(m_fCurrentBURN < m_fBURN)
		{
			m_fCurrentBURN += (m_fBURN*fElapsedTime)/BOOSTER_OVERHEAT_REPAIR_TIME;
			if(m_fCurrentBURN/m_fBURN >= 0.9f)
			{
				m_bReChargeBURN = TRUE;
			}
			if(m_fCurrentBURN > m_fBURN)
			{
				m_fCurrentBURN = m_fBURN;
			}
			if(g_pGameMain->m_pUnitInfoBar)
			{
				g_pGameMain->m_pUnitInfoBar->SetBURN(m_fCurrentBURN, m_fBURN);
			}
		}
	}

	// 2005-07-11 by ispark	
	// Áß·Â °č»ęŔ» ľČÇßŔ˝. ±×·ˇĽ­ ¸Ę °ć°čĽ±żˇĽ­ şÎ˝şĹÍ ąö±×°ˇ ŔÖľúŔ˝
//	if( m_bUseBooster == FALSE && m_fShuttleSpeed <= m_fShuttleSpeedMax)
//	if( m_bUseBooster == FALSE && m_fShuttleSpeed <= m_fRealShuttleSpeedMax)
	if( m_bUseBooster == FALSE && m_fShuttleSpeed - m_fTempGravity <= m_fRealShuttleSpeedMax)
	{
		m_bUseBooster = TRUE;
	}
//	if(m_fCurrentBURN <= 0.0f || m_fBURN == 0.0f)
//		m_bReChargeBURN = FALSE;
//	else if(m_fCurrentBURN/m_fBURN >= 0.9f)
//		m_bReChargeBURN = TRUE;
//	if(fOldBURN != m_fCurrentBURN)
//	{
//		ASSERT_ASSERT(m_fBURN>0.0f);
//		if(g_pGameMain->m_pUnitInfoBar)
//		{
//			g_pGameMain->m_pUnitInfoBar->SetBURN(m_fCurrentBURN, m_fBURN);
//		}
//	}

}

void CShuttleChild::TickWeapon(float fElapsedTime)
{
	 TickChangeClientArea();
	if(m_pPrimaryWeapon)
	{
//		m_PrimaryAttack.fCheckAutoMaticTimer -= fElapsedTime;
//		m_PrimaryAttack.fCheckAttackTime -= fElapsedTime;

		// 2005-08-02 by ispark
		// Âř·ú¶§łŞ ŔĚ·ú¶§´Â ąß˝Î¸¦ ¸·Ŕ˝
//		if( IS_WEAPON_ENABLESTATE(m_dwState, m_nBooster))
		if( IS_WEAPON_ENABLESTATE(m_dwState, m_nBooster) && m_bWarpLink == FALSE)
		{
			m_pPrimaryWeapon->Tick( fElapsedTime, m_bLButtonState );
		}
		else
		{
			m_pPrimaryWeapon->Tick( fElapsedTime, FALSE );
		}
	}
	if( m_pSecondaryWeapon )
	{
//		m_SecondaryAttack.fCheckAutoMaticTimer -= fElapsedTime;
//		m_SecondaryAttack.fCheckAttackTime -= fElapsedTime;
//		if(m_fMissileFireTime > 0)
//		{
//			m_fMissileFireTime -= fElapsedTime;
//			if(m_fMissileFireTime <= 0)
//			{
//				m_fMissileFireTime = 0;
//			}
//		}

		// 2005-08-02 by ispark
		// Âř·ú¶§łŞ ŔĚ·ú¶§´Â ąß˝Î¸¦ ¸·Ŕ˝
//		if( IS_WEAPON_ENABLESTATE(m_dwState, m_nBooster) )
		if( IS_WEAPON_ENABLESTATE(m_dwState, m_nBooster) && m_bWarpLink == FALSE)
		{
			m_pSecondaryWeapon->Tick( fElapsedTime, m_bRButtonState );
		}
		else
		{
			m_pSecondaryWeapon->Tick( fElapsedTime, FALSE );
		}
	}
	// 2010-06-15 by shcho&hslee Ćę˝Ă˝şĹŰ - Ćę °ř°Ý Ăł¸®
	if( m_pPetManager &&
		m_pPetManager->GetWeaponItemInfo() ) 
	{
		if(IS_WEAPON_ENABLESTATE(m_dwState, m_nBooster) && m_bWarpLink == FALSE)
		{
			m_pPetManager->GetWeaponItemInfo()->Tick( fElapsedTime, m_bLButtonState );
		}
		else
		{
			m_pPetManager->GetWeaponItemInfo()->Tick( fElapsedTime, FALSE );
		}
	}
	// end 2010-06-15 by shcho&hslee Ćę˝Ă˝şĹŰ - Ćę °ř°Ý Ăł¸®

}



void CShuttleChild::Tick(float fElapsedTime)
{
	FLOG( "CShuttleChild::Tick(float fElapsedTime)" );

	// 2007-05-28 by bhsohn żˇľî ˝ĂÁî ą«Á¶°Ç µĄąĚÁö µéľî°ˇ´Â ąö±× ĽöÁ¤
	m_bAGearFollowEnemy = FALSE;

	// 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
	m_bSetCursorPos = FALSE;

	// Ĺ×˝şĆ® ÄÚµĺ
// 2005-08-08 by ispark
// ±âÁ¸ F11Ĺ°¸¦ ľřľŘ´Ů.
//#ifdef _DEBUG
//	if(g_pD3dApp->m_pKeyBoard->GetAsyncKeyState(DIK_F11))
//	{// ŔúĽÓ
//		m_bUnitStop = !m_bUnitStop;
//	}
//
//	if(m_bUnitStop)
//	{
//		m_fShuttleSpeed = 0.0f;
//		m_fTempGravity = 0.0f;
//		m_fRealShuttleSpeedMin = 0.0f;
//	}
//#endif
	// 2007-03-29 by dgwoo żÉŔúąö ¸đµĺŔĎ¶§ ´ÜĂŕĹ°°Şżˇ µű¶ó ŔŻŔú¸¦ şŻ°ć.
	if(m_bObserve)
	{
		TickObserve(fElapsedTime);
	}
	if(m_bOperation)
	{
		TickOperation(fElapsedTime);
	}
//	if(m_pSkillEffect)
//	{
//		m_pSkillEffect->Tick(fElapsedTime);
//	}
	
	// Ground Attack
	if(m_pSecondaryWeapon && m_pSecondaryWeapon->GetAttackMode() == ATT_TYPE_GROUND_BOMBING_SEC)
	{
		SetGroundTargetPos();
	}
	// 2006-12-01 by ispark, °řÁß Ćř°Ý
	else if(m_pSecondaryWeapon && m_pSecondaryWeapon->GetAttackMode() == ATT_TYPE_AIR_BOMBING_SEC)
	{
		SetAirGroundTargetPos();
	}

	// ĆÄĆĽ Tick
	if(m_pClientParty->IsParty())
	{
		m_pClientParty->Tick(fElapsedTime);
	}
	
	if(m_dwState == _INITIALIZING)
	{
		return; // initializing
	}

	// 2005-02-15 by jschoi - Pk Delay Timer
	m_pPkNormalTimer->Tick(fElapsedTime);

	// żÉŔúąö ¸đµĺ˝Ăżˇ´Â ŔÚµżŔ¸·Î żĂ¶ó°ˇ´Â°É ¸·´Â´Ů.
	if(!IsObserverMode())
	{
		UnitInfoBarSmoothRepair(fElapsedTime);
	}
	CheckDamageBodyCondition();
	UsedItemTick(fElapsedTime);
	m_pSkill->Tick(fElapsedTime);
	// Ĺ¬¶óŔĚľđĆ® Ĺ¸ŔĚ¸Ó °č»ę
	m_timeProcess.Tick(fElapsedTime);

	if(g_pScene->m_byMapType == MAP_TYPE_CITY)
	{
		TickCity(fElapsedTime);
	}
	else
	{
		TurnOnWall(fElapsedTime);
		if(m_dwState == _LANDED && !IS_DT(m_myShuttleInfo.UnitKind) && m_bLandedMove)		// Âř·úÁßżˇ´Â »çżë ±ÝÁö							
		{
			MoveLandingStation(fElapsedTime);
		}
		if(g_pScene->m_byMapType == MAP_TYPE_TUTORIAL)
		{
			CheckClientQuestList();
		}

		if(m_fCollSendDamageCheckTime >= 0.0f)
		{
			m_fCollSendDamageCheckTime -= fElapsedTime;
		}
		if(m_fGetOtherInfoCheckTime >= 0.0f)
		{
			m_fGetOtherInfoCheckTime -= fElapsedTime;
		}
		if(m_fGetOtherInfoCheckTime < 0.0f && m_bGetOtherInfo)
		{
			m_bGetOtherInfo = FALSE;
			m_nGetOtherIndex = 0;
		}

		D3DXVECTOR3  vUpTemp;
		D3DXMATRIX	matTemp;
		m_bIsUnderWater = FALSE;
		m_vOldPos = m_vPos;

		HandleInput(fElapsedTime);
		CheckMouseDir();

		// ŔĚşĄĆ®¸¦ ĂĽĹ©ÇĎż© ŔűżëÇŃ´Ů.
		if(m_dwState == _NORMAL || m_bSkillAirMove)
		{
			CheckEventType();
			CheckEventTypeObject();
		}

		// Âř·úŔĺżˇĽ­ ´Ů¸ĄŔĺĽŇ·Î ĹÚ·ąĆ÷Ć®˝Ă..
		if(m_nEventIndex != 0)
		{
			int nSpeed = (int)(m_fShuttleSpeed - m_fTempGravity);
			if(nSpeed <= 0 || m_bSkipPattern == TRUE)
			{
//				CObjectChild* pObj = g_pScene->FindObjectByIndex(m_nEventIndex);
				CObjectChild* pObj = g_pScene->FindRandomObjectByEventType(EVENT_TYPE_OBJ_ENTER_BUILDING, TRUE);
				m_nEventIndex = 0;
				m_bKeyBlock = FALSE;
				m_bSkipPattern = FALSE;
				if(pObj)
				{
					// Ŕ§Äˇ ŔĚµż
					g_pD3dApp->StartFadeEffect(TRUE,3,D3DCOLOR_ARGB(0,0,0,0));
					// ·»´ý Ŕ§Äˇ ÁöÁ¤
//					m_vPos = pObj->m_vPos;//CAtumSJ::GetRandomXZVec3(&pObj->m_vPos, SIZE_CITY_POSITION_RANDOM_XZ_RADIUS);
					m_vPos = CAtumSJ::GetRandomXZVec3(&pObj->m_vPos, SIZE_CITY_POSITION_RANDOM_XZ_RADIUS);
					g_pCharacterChild->m_vPos = m_vPos;
					// 2010. 01. 25 by jskim żöÇÁ ŔĚµż˝Ă ¸Ę ÄĂ¸µ Ŕß¸řÇĎ´Â ąö±× ĽöÁ¤ 
					g_pD3dApp->SetCamPosInit();					
					//end 2010. 01. 25 by jskim żöÇÁ ŔĚµż˝Ă ¸Ę ÄĂ¸µ Ŕß¸řÇĎ´Â ąö±× ĽöÁ¤ 
					DBGOUT("GetRandomXZVec3 %d, %d, %d\n", (int)m_vPos.x, (int)m_vPos.y, (int)m_vPos.z);

					// ¸ŢĽĽÁö Ăł¸®
					INIT_MSG_WITH_BUFFER(MSG_FC_CHARACTER_CHANGE_CHARACTER_MODE,
						T_FC_CHARACTER_CHANGE_CHARACTER_MODE, pSMode, SendBuf);
					pSMode->CharacterMode0 = TRUE;
					pSMode->PositionAVec3 = m_vPos;
					pSMode->TargetAVec3 = m_vVel*1000.0f;
					g_pFieldWinSocket->Write(SendBuf, MSG_SIZE(MSG_FC_CHARACTER_CHANGE_CHARACTER_MODE));
					// 2005-11-03 by ispark
					// Ćí´ë şńÇŕ ÁßŔĚľúµű¸é ÇŘĂĽ
					if(	m_pClientParty &&
						m_pClientParty->IsFormationFlightMove()
#ifdef _INET_BREAK_FORM_FLIGHT_WHEN_SIEGE
						|| m_bAttackMode == _SIEGE //18.12.2025 Inet - when using siege mode exclude from form flight
#endif
						)
					{
						SendPacket(T_IC_PARTY_CANCEL_FLIGHT_POSITION);
						m_pClientParty->FormationFlightClear();
						g_pD3dApp->m_pChat->CreateChatChild(STRMSG_C_PARTY_0001,COLOR_SYSTEM);//"Ćí´ëşńÇŕżˇĽ­ ąţľîłµ˝Ŕ´Ď´Ů."
					}
					// 2005-07-28 by ispark
					// Äł¸ŻĹÍ ĂĘ±â ąŮµđ ÄÁµđĽÇ ş¸łż. ±× ŔĚČÄ´Â ş¸ł»Áö ľĘ´Â´Ů. Client°ˇ ˝ş˝ş·Î ąŮµđÄÁµđĽÇŔ» ĆÇ´ÜÇŃ´Ů.
					SendFieldSocketChangeBodyCondition(m_myShuttleInfo.ClientIndex, BODYCON_SET_OR_CLEAR_MASK);
					// Äł¸ŻĹÍ ¸đµĺ şŻČŻ
					g_pD3dApp->m_bCharacter = TRUE;
					// 2007-02-22 by dgwoo ¸ĘĽÂĆĂ.
					g_pInterface->m_pGameMain->InitWarp();
					m_bLandingField = FALSE;
					m_pTarget = NULL;
					m_pOrderTarget = NULL;
					g_pCharacterChild->InitCharacterData();
				}				
			}			
		}

		if(m_bSkillMoveIsUse )//&& m_bEventReady == FALSE)
		{
			UseSkillMove(fElapsedTime);
		}
		else
		{
			// 2005-06-01 by jschoi
			if(m_bIsAir == FALSE && IS_DT(m_myShuttleInfo.UnitKind))
			{
				Move_Ground(fElapsedTime);
			}
			else
			{
				if (m_bLendSoundPlay) {
					if(g_pD3dApp->m_pSound)
						g_pD3dApp->m_pSound->StopD3DSound(SOUND_GROUND_MOVING_A_GEAR);
					m_bLendSoundPlay = FALSE;
				}

				Move_Air(fElapsedTime);
			}
		}

		// 2006-01-18 by ispark, Dead Áß°Ł Á¤°Ë, 
		// ¸¸ľŕżˇ Ľ­ąö·ÎşÎĹÍ 4ĂĘµżľČ ¸ŢĽĽÁö¸¦ ąŢÁö ¸řÇĎ¸é °­Á¦·Î _FALLINGŔ» ÇŃ´Ů.
		if(m_fCheckDead >= 1.0f)
		{
			m_fCheckDead += fElapsedTime;
			if(m_fCheckDead > 5.0f)
			{
				if(!UNIT_STATE_DEAD(m_dwState))
				{
					DBGOUT("My State[%d] --> Dead State\n", m_dwState);
					ChangeUnitStateDead();
				}
				else
				{
					m_fCheckDead = 0.0f;
				}
			}
		}

		// Burn
		TickBurn(fElapsedTime);
		// 2006-11-27 by dgwoo °ř°ÝŔĚ °ˇ´ÉÇŇ¶§¸¸ WeaponÇÔĽö·Î µéľî°Ł´Ů.
		if(m_bAttack)
			TickWeapon(fElapsedTime);   

#ifdef _INET_PET
		PetUseItme();
#endif

		CheckTarget();


		// 2005-03-31 by jschoi
		SetEffectPos(fElapsedTime);

		CheckGroundEffect();
		CheckOnObject();			// żŔşęÁ§Ć® Ŕ§ŔÎÁö ĂĽĹ©ÇĎ´Â ÇÔĽö
//		m_bOnObject = FALSE;		// żŔşęÁ§Ć® Ŕ§ŔÎÁö şŻĽö¸¦ ĂĘ±âČ­ -> ŔĚşŻĽö´Â żŔşęÁ§Ć®ŔÇ TickżˇĽ­ °áÁ¤µČ´Ů.
		CheckAniTime();
		m_pMoveChat->Tick();
		m_pIDChat->Tick();
		// 1~2ÇüŔÇ ąß»ç˝Ă ¶ôżÂ ŔĚĆĺĆ®¸¦ Ŕ§ÇŃ şŻĽö ĂĘ±âČ­
//		m_bPrimaryAttackFire = FALSE;
//		m_bSecondaryAttackFire = FALSE;

//		for(int i = 0;i<MAX_MINE_NUM;i++)
//		{
//			m_pCurrentMine[i] = NULL;
//		}
//		m_nCurrentMineCont = 0;

		// 2006-12-04 by ispark, ĂĽÇÁ »çĂâ
		ChaffDataTick();
	}

	// 2007-04-05 by dgwoo Move Air ľĆ·ˇ·Î Ŕ§Äˇ şŻ°ć.
	if(m_pSkillEffect)
	{
		m_pSkillEffect->Tick(fElapsedTime);
		// 2009-03-30 by bhsohn Â÷ÂˇĽ¦ ŔĚĆĺĆ® ąö±× ĽöÁ¤
		CCharacterInfo*  pCharacterInfo = m_pSkillEffect->FindSkillEffect(BGEAR_STATE_CHARINGSHOT);
		if(m_pSkill && pCharacterInfo)
		{				
			if(!m_pSkill->IsSkillUse(BGEAR_STATE_CHARINGSHOT))
			{
				m_fDelCapChagingSkill += g_pD3dApp->GetCheckElapsedTime();
				if(m_fDelCapChagingSkill > BGEAR_CHAGINGSHOT_SHOT_CAP_TIME)
				{					
					// Â÷Âˇ ˝şĹł Á¦°Ĺ
					m_pSkillEffect->DeleteChargingStateSkillEffect();
					m_fDelCapChagingSkill = 0.0f;
				}									
			}
			else
			{
				m_fDelCapChagingSkill = 0.0f;
			}
		}
		// end 2009-03-30 by bhsohn Â÷ÂˇĽ¦ ŔĚĆĺĆ® ąö±× ĽöÁ¤
	}
	SetUnitState();

	// 2005-12-29 by ispark, ĂĽĹ© ŔĺÂř ľĆŔĚĹŰ
	CheckWearItem(fElapsedTime);

//	const int nKeyMove = 1; // Ĺ°ş¸µĺ °¨µµ
//	if(g_pD3dApp->m_pKeyBoard->GetAsyncKeyState(DIK_LEFT))
//	{
//		g_pCamera->SetControlCamSideAngle(nKeyMove);
//	}
//	if(g_pD3dApp->m_pKeyBoard->GetAsyncKeyState(DIK_RIGHT))
//	{
//		g_pCamera->SetControlCamSideAngle(-nKeyMove);
//	}
//	if(g_pD3dApp->m_pKeyBoard->GetAsyncKeyState(DIK_UP))
//	{
//		g_pCamera->SetControlCamAngle(nKeyMove);
//	}
//	if(g_pD3dApp->m_pKeyBoard->GetAsyncKeyState(DIK_DOWN))
//	{
//		g_pCamera->SetControlCamAngle(-nKeyMove);
//	}
//	if(g_pD3dApp->m_pKeyBoard->GetAsyncKeyState(DIK_PRIOR))
//	{
//		g_pCamera->SetControlCamPos(nKeyMove);
//	}
//	if(g_pD3dApp->m_pKeyBoard->GetAsyncKeyState(DIK_NEXT))
//	{
//		g_pCamera->SetControlCamPos(-nKeyMove);
//	}

	// 2005-03-08 by jschoi
	if((IsPkEnableMap(m_myShuttleInfo.MapChannelIndex.MapIndex) ||
		m_pPkNormalTimer->IsCityWar()) &&
		g_pScene->FindWarpGateByPosition(m_vPos,CHECK_WARP_GATE_DISTANCE))
	{
		m_bWarpGateZone = TRUE;
	}
	else
	{
		m_bWarpGateZone = FALSE;
	}

	// 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
	if(g_pD3dApp->IsUseJoyStick())
	{		
		if(!m_bSetCursorPos)
		{
			SetCursorJostickPos();
		}
		CheckJoystickKeyDown();
	}
	
	// end 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷

	
	// 2009. 02. 18 by ckPark ˝ĂÁî ¸đµĺ ÇŘÁ¦˝Ă şÎ˝şĹÍ ŔĚĆĺĆ® ąö±×
	if( IS_DT(m_myShuttleInfo.UnitKind)
		&& ( GetCurrentBodyCondition() & BODYCON_SIEGE_OFF_MASK ) )
	{
		if(GetCurrentAnimationTime() >= GetCurrentBodyConditionEndAnimationTime())
			ChangeSingleBodyCondition(BODYCON_LANDED_MASK);
	}
	// end 2009. 02. 18 by ckPark ˝ĂÁî ¸đµĺ ÇŘÁ¦˝Ă şÎ˝şĹÍ ŔĚĆĺĆ® ąö±×
	// 2009-02-19 by bhsohn Ä«¸Ţ¶ó ˝ĂĽ± şŻ°ćČÄ, ˝±°Ô »çłÉ ąćąý ¸·Ŕ˝
	TickCheckGearAttackStat();	
	// end 2009-02-19 by bhsohn Ä«¸Ţ¶ó ˝ĂĽ± şŻ°ćČÄ, ˝±°Ô »çłÉ ąćąý ¸·Ŕ˝
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			BOOL CShuttleChild::GetAttackClientArea()
/// \brief		¸¶żě˝ş°ˇ °ř°ÝÇŇĽö ŔÖ´Â Ĺ¬¶óŔĚľđĆ® żµżŞżˇ ŔÖ´Â°ˇ¸¦ ĂĽĹ©.
/// \author		dgwoo
/// \date		2008-01-14 ~ 2008-01-14
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CShuttleChild::TickChangeClientArea()
{
	BOOL bMousePosArea = TRUE;
	//ScreenToClient()
	m_bNowMousePosWindowArea =  IsMousePosClientArea();
	if(!m_bNowMousePosWindowArea)
	{// ¸¶żě˝ş°ˇ Ĺ¬¶óŔĚľđĆ® żµżŞŔ» ąţľîłµŔ»°ćżě.
		bMousePosArea = FALSE;
		// 2008-03-13 by dgwoo  ą«±âŔÇ ĽÂĆĂŔ» ĂĘ±âČ­.
		m_bLButtonState = FALSE;
		m_bRButtonState = FALSE;
	}
	if(m_bNowMousePosWindowArea != m_bOldMousePosWindowArea)
	{
		//DBGOUT("m_bNowMouseArea = %d, m_bOldMouseArea = %d",m_bNowMousePosWindowArea,m_bOldMousePosWindowArea);
		m_bOldMousePosWindowArea = m_bNowMousePosWindowArea;
		// 2009-02-19 by bhsohn Ä«¸Ţ¶ó ˝ĂĽ± şŻ°ćČÄ, ˝±°Ô »çłÉ ąćąý ¸·Ŕ˝
		//if(GetKeyState(VK_LBUTTON) & 0x8000)
		if((GetKeyState(VK_LBUTTON) & 0x8000) && IsPossibleAttackStat()) 
		{
			m_bLButtonState = TRUE;
		}
		else
		{
			m_bLButtonState = FALSE;
		}
		// 2009-02-19 by bhsohn Ä«¸Ţ¶ó ˝ĂĽ± şŻ°ćČÄ, ˝±°Ô »çłÉ ąćąý ¸·Ŕ˝							
		//if(GetKeyState(VK_RBUTTON) & 0x8000)
		if((GetKeyState(VK_RBUTTON) & 0x8000) && IsPossibleAttackStat())
		{
			m_bRButtonState = TRUE;
		}
		else
		{
			m_bRButtonState = FALSE;
		}
		//DBGOUT("m_bLButton = %d(%x), m_bRButton = %d(%x)\n",m_bLButtonState,GetKeyState(VK_LBUTTON),m_bRButtonState,GetKeyState(VK_RBUTTON));
	}
	return bMousePosArea;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			CShuttleChild::SetUnitState()
/// \brief		Č­¸é »ó´Üżˇ ŔŻ´ÖŔÇ »óĹÂ ÇĄ˝Ă
/// \author		dhkwon
/// \date		2004-06-01 ~ 2004-06-01
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CShuttleChild::SetUnitState()
{
	float fWeight = g_pStoreData->GetTotalWeight() / CAtumSJ::GetTransport(&g_pShuttleChild->m_myShuttleInfo);
	if(fWeight > 0.9f)
	{
		g_pGameMain->m_pUnitState->InsertState(INF_UW_FULLITEM3_MASK);
		g_pGameMain->m_pUnitState->OffState(INF_UW_FULLITEM2_MASK);
		g_pGameMain->m_pUnitState->OffState(INF_UW_FULLITEM1_MASK);
	}
	else if(fWeight > WEIGHT_RATE_VERY_HEAVY)
	{
		g_pGameMain->m_pUnitState->InsertState(INF_UW_FULLITEM2_MASK);
		g_pGameMain->m_pUnitState->OffState(INF_UW_FULLITEM3_MASK);
		g_pGameMain->m_pUnitState->OffState(INF_UW_FULLITEM1_MASK);
	}
	else if(fWeight > WEIGHT_RATE_HEAVY)
	{
		g_pGameMain->m_pUnitState->InsertState(INF_UW_FULLITEM1_MASK);
		g_pGameMain->m_pUnitState->OffState(INF_UW_FULLITEM2_MASK);
		g_pGameMain->m_pUnitState->OffState(INF_UW_FULLITEM3_MASK);
	}
	else
	{
		g_pGameMain->m_pUnitState->OffState(INF_UW_FULLITEM1_MASK);
		g_pGameMain->m_pUnitState->OffState(INF_UW_FULLITEM2_MASK);
		g_pGameMain->m_pUnitState->OffState(INF_UW_FULLITEM3_MASK);
	}
	if(!m_bReChargeBURN) // şÎ˝şĹÍ °úż­
	{
		g_pGameMain->m_pUnitState->InsertState(INF_UW_OVERHEAT_MASK);
	}
	else
	{
		g_pGameMain->m_pUnitState->OffState(INF_UW_OVERHEAT_MASK);
	}
	if(m_nBooster == BOOSTER_TYPE_AIRBREAK) // °řÁß şÎľç
	{
		g_pGameMain->m_pUnitState->SetState(INF_US_STOP_MASK);
	}
	else
	{
		g_pGameMain->m_pUnitState->OffState(INF_US_STOP_MASK);
	}
	if(m_bMouseLock && m_bUnitStop == FALSE)							// 2005-08-08 by ispark ŔŻ´ÖÁ¤Áö˝Ă ¸¶żě˝ş¸¦ ¸·´Â´Ů. ±×·ŻłŞ STATE°ˇ UNIT ROCK »óĹÂ°ˇ ľĆ´Ď´Ů.
	{
//		g_pGameMain->m_pUnitState->OffState(INF_US_STOP_MASK);
//		g_pGameMain->m_pUnitState->SetState(INF_US_LOCK_MASK);
	}
	else
	{
		g_pGameMain->m_pUnitState->OffState(INF_US_LOCK_MASK);
	}
	if( m_dwState == _TAKINGOFF )	// ŔĚ·ú
	{
		g_pGameMain->m_pUnitState->SetState(INF_US_TAKEOFF_MASK);
	}
	else
	{
		g_pGameMain->m_pUnitState->OffState(INF_US_TAKEOFF_MASK);
	}
	if( m_dwState == _LANDED )	// Âř·ú
	{
		// 2009. 08. 19 by jsKim ·Łµů Áß ¸Ţ´ş »ýĽşÇŇ °ćżě ÄżĽ­°ˇ şŻÇĎÁö ľĘ´Â ąö±×
		g_INFCnt = 0;
		// end 2009. 08. 19 by jsKim ·Łµů Áß ¸Ţ´ş »ýĽşÇŇ °ćżě ÄżĽ­°ˇ şŻÇĎÁö ľĘ´Â ąö±×
		g_pGameMain->m_pUnitState->SetState(INF_US_LAND_MASK);		
	}
	else
	{
		g_pGameMain->m_pUnitState->OffState(INF_US_LAND_MASK);
	}
	if( m_pClientParty->IsFormationFlightMove() ) // Ćí´ëşń
	{
		g_pGameMain->m_pUnitState->SetState(INF_US_FORM_MASK);
	}
	else
	{
		g_pGameMain->m_pUnitState->OffState(INF_US_FORM_MASK);
	}
	// 2005-07-11 by ispark
	// ŔĚµż °ˇ´ÉÇŃ ĂÖ´ë łôŔĚ ŔÚµż ĽÂĆĂ
	if(m_vPos.y > g_pScene->m_nMaxAtitudeHeight + g_pGround->m_projectInfo.fWaterHeight - 100)
	{
		g_pGameMain->m_pUnitState->InsertState(INF_UW_TOOHIGH_MASK);
	}
	else
	{
		g_pGameMain->m_pUnitState->OffState(INF_UW_TOOHIGH_MASK);
	}
}
void CShuttleChild::CheckMapRegion()
{
	FLOG( "CShuttleChild::CheckMapRegion()" );
	// ¸ĘŔ» ąţľîłŞ¸é ¸řłŞ°ˇ°Ô ĽÂĆĂ ĂĽĹ©
	if(g_pScene->m_byMapType == MAP_TYPE_TUTORIAL)
	{
		if(m_vPos.x < 2660.0f)
			m_vPos.x = 2660.0f;
		if(m_vPos.x > 3760.0f)
			m_vPos.x = 3760.0f;
		if(m_vPos.z < 67.0f)
			m_vPos.z = 67.0f;
		if(m_vPos.z > 967)
			m_vPos.z = 967;
		// ąö±× ąß»ýŔ¸·Î ą«ÇŃ łôŔĚ·Î żĂ¶ó°¬´Ů¸é ł»¸°´Ů.
		if(m_vPos.y < 0.0f)// || m_vPos.y < -1000.0f)
			m_vPos.y = 0.0f;
		if(m_vPos.y > 400.0f)// || m_vPos.y < -1000.0f)
			m_vPos.y = 400.0f;
	}
	else
	{
		if(m_vPos.x < 40.0f)
			m_vPos.x = 40.0f;
		if(m_vPos.x > (g_pGround->m_projectInfo.sXSize-1)*TILE_SIZE)
			m_vPos.x = (g_pGround->m_projectInfo.sXSize-1)*TILE_SIZE;
		if(m_vPos.z < 40.0f)
			m_vPos.z = 40.0f;
		if(m_vPos.z > (g_pGround->m_projectInfo.sYSize-1)*TILE_SIZE)
			m_vPos.z = (g_pGround->m_projectInfo.sYSize-1)*TILE_SIZE;
		// ąö±× ąß»ýŔ¸·Î ą«ÇŃ łôŔĚ·Î żĂ¶ó°¬´Ů¸é ł»¸°´Ů. ĽĹĆ˛ŔÇ ĂÖ´ë łôŔĚ
//		if(m_vPos.y > g_pGround->m_projectInfo.fHeightMax + WEATHER_SUNNY_FOG_END)// || m_vPos.y < -1000.0f)
//			m_vPos.y = g_pGround->m_projectInfo.fHeightMax + WEATHER_SUNNY_FOG_END;


		// 2005-02-02 by jschoi
		// żěÁÖ¸ĘŔÇ »óÇĎ ŔĚµż °ˇ´É ąÝ°ćŔ» Č®Ŕĺ
		// ALTITUDE_SAPCE_ALTITUDE °Ş ¸¸Ĺ­ Ŕ§,ľĆ·ˇ·Î Č®Ŕĺ
		// 2006-04-12 by ispark, DBżˇĽ­ ¸Ę łôŔĚ¸¦ ĽÂĆĂ ÇĎąÇ·Î żěÁÖ¸ĘŔĚ¶ó°í ÇŘĽ­ ş°µµ·Î łôŔĎ ŔĚŔŻ°ˇ ľř´Ů.
//		if(IS_SPACE_MAP_INDEX(m_myShuttleInfo.MapChannelIndex.MapIndex) == TRUE)
//		{// żěÁÖ¸ĘŔĚ¶ó¸é
//			// 2005-07-11 by ispark
//			// ŔĚµż °ˇ´ÉÇŃ ĂÖ´ë łôŔĚ ŔÚµż ĽÂĆĂ
//			if(m_vPos.y > g_pScene->m_nMaxAtitudeHeight + ALTITUDE_SAPCE_ALTITUDE + g_pGround->m_projectInfo.fWaterHeight)
//			{
//				m_vPos.y = g_pScene->m_nMaxAtitudeHeight + ALTITUDE_SAPCE_ALTITUDE + g_pGround->m_projectInfo.fWaterHeight;
//			}
////			else if(m_vPos.y < ALTITUDE_APPLY_MIN - ALTITUDE_SAPCE_ALTITUDE + g_pGround->m_projectInfo.fWaterHeight)
////			{
////				m_vPos.y = ALTITUDE_APPLY_MIN - ALTITUDE_SAPCE_ALTITUDE + g_pGround->m_projectInfo.fWaterHeight;
////			}
//		}
//		else
//		{// żěÁÖ¸ĘŔĚ ľĆ´Ď¶ó¸é
			// 2005-07-11 by ispark
			// ŔĚµż °ˇ´ÉÇŃ ĂÖ´ë łôŔĚ ŔÚµż ĽÂĆĂ
			if(m_vPos.y > g_pScene->m_nMaxAtitudeHeight + g_pGround->m_projectInfo.fWaterHeight)
			{
				m_vPos.y = g_pScene->m_nMaxAtitudeHeight + g_pGround->m_projectInfo.fWaterHeight;
			}
//		}

		// 2005-12-12 by ispark, ÁöÇüŔ» ±×¸®Áö ľĘ´Â ¸ĘŔĚ¶ó¸é łôŔĚ 0 ŔĚÇĎ·Î °ˇÁö ¸řÇĎ°Ô ÇŃ´Ů.
		if(!IsTileMapRenderEnable(m_myShuttleInfo.MapChannelIndex.MapIndex))
		{
			if(m_vPos.y <= 0)
				m_vPos.y = 0;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			void CShuttleChild::CheckBazaarInfo(MSG_FC_BAZAAR_INFO_OK* pMsg)
/// \brief		°łŔÎ»óÁˇ Á¤ş¸
/// \author		ispark
/// \date		2006-07-28 ~ 2006-07-28
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CShuttleChild::CheckBazaarInfo(MSG_FC_BAZAAR_INFO_OK* pMsg)
{
	if(m_myShuttleInfo.ClientIndex == pMsg->clientIndex0)
	{
		return;
	}
	else
	{
		CEnemyData * pEnemy = NULL;
		CMapEnemyIterator itEnemy = g_pScene->m_mapEnemyList.find(pMsg->clientIndex0);
		if(itEnemy != g_pScene->m_mapEnemyList.end())
		{
			pEnemy = itEnemy->second;
		}

		if(pEnemy)
		{
			pEnemy->m_byBazaarType = pMsg->byBazaarType;
			if(pMsg->byBazaarType == 1)
			{
				wsprintf(pEnemy->m_szBazaarName, STRMSG_C_060811_0000, pMsg->szBazaarName);
				pEnemy->m_pMoveChatShop = new CChatMoveData(NULL,pEnemy->m_szBazaarName,COLOR_SKILL_USE,11,500,52,240,FALSE, 0x0001);
				pEnemy->m_pMoveChatShop->InitDeviceObjects();
				pEnemy->m_pMoveChatShop->RestoreDeviceObjects();
			}
			else if(pMsg->byBazaarType == 2)
			{
				wsprintf(pEnemy->m_szBazaarName, STRMSG_C_060811_0001, pMsg->szBazaarName);
				pEnemy->m_pMoveChatShop = new CChatMoveData(NULL,pEnemy->m_szBazaarName,GUI_FONT_COLOR_YM,11,500,52,240, FALSE, 0x0001);
				pEnemy->m_pMoveChatShop->InitDeviceObjects();
				pEnemy->m_pMoveChatShop->RestoreDeviceObjects();
			}
		}
	}
}

void CShuttleChild::CheckIndexList(MSG_FC_CHARACTER_GET_OTHER_INFO_OK* pMsg)
{
	FLOG( "CShuttleChild::CheckIndexList(MSG_FC_CHARACTER_GET_OTHER_INFO_OK* pMsg)" );
 	if(m_myShuttleInfo.ClientIndex == pMsg->CharacterInfo.ClientIndex)
		return;
	else
	{
		if(pMsg->CharacterInfo.ClientIndex == m_nGetOtherIndex)
		{
			m_bGetOtherInfo = FALSE;
			m_fGetOtherInfoCheckTime = 0.0f;
			m_nGetOtherIndex = 0;
		}
		CEnemyData * pEnemy = NULL;
		CMapEnemyIterator itEnemy = g_pScene->m_mapEnemyList.find(pMsg->CharacterInfo.ClientIndex);
		if(itEnemy != g_pScene->m_mapEnemyList.end())
		{
			if(itEnemy->second->m_infoCharacter.CharacterInfo.CharacterUniqueNumber == pMsg->CharacterInfo.CharacterUniqueNumber)
			{
				pEnemy = itEnemy->second;
			}
		}
		if(pEnemy)
		{
			// 2006-07-05 by ispark »ó´ë ĽĽ·Â ¸ŕĆ® »čÁ¦
			if(!IsInfluenceCharacter(g_pShuttleChild->m_myShuttleInfo.InfluenceType, pMsg->CharacterInfo.InfluenceType))
			{
				memset(pMsg->CharacterInfo.szCharacterMent, 0x00, sizeof(pMsg->CharacterInfo.szCharacterMent));
			}
			pEnemy->m_bIsInit = TRUE;
			pEnemy->m_infoCharacter.CharacterInfo = pMsg->CharacterInfo;
			pEnemy->m_dwPartType = (COMPARE_RACE(pMsg->CharacterInfo.Race, RACE_GAMEMASTER) ? _ADMIN:_ENEMY);
//			pEnemy->m_infoCharacter.UnitKind = pMsg->CharacterInfo.UnitKind;
//			pEnemy->m_infoCharacter.Race = pMsg->CharacterInfo.Race;
//			pEnemy->m_infoCharacter.GuildUniqueNumber = pMsg->CharacterInfo.GuildUniqueNumber;
//			pEnemy->m_infoCharacter.BodyCondition = pMsg->CharacterInfo.BodyCondition;
//			pEnemy->m_infoCharacter.ClientIndex = pMsg->CharacterInfo.ClientIndex;
			pEnemy->m_vPos = A2DX(pMsg->CharacterInfo.PositionVector);
			pEnemy->m_vNextPos = A2DX(pMsg->CharacterInfo.PositionVector);
			pEnemy->m_bEnemyCharacter = pMsg->CharacterInfo.CharacterMode0;			// 2005-12-30 by ispark
			// 2005-07-28 by ispark
			D3DXVec3Normalize(&pEnemy->m_vVel, &A2DX(pMsg->CharacterInfo.TargetVector));
			pEnemy->m_infoCharacter.CharacterRenderInfo = pMsg->CharacterRenderInfo;
			// 10. 03. 18 by jskim ¸ó˝şĹÍşŻ˝Ĺ Ä«µĺ
			pEnemy->SetMonsterTransformer(pMsg->CharacterRenderInfo.RI_MonsterUnitKind_ForTransformer);
			//end 10. 03. 18 by jskim ¸ó˝şĹÍşŻ˝Ĺ Ä«µĺ
			if(!pEnemy->m_pCharacterInfo && pEnemy->m_pCharacterInfo != nullptr)
			{
				// 2005-11-24 by ispark, ItemNum -> SourceIndexŔ¸·Î şŻ°ć


				// 2009. 08. 27 by ckPark ±×·ˇÇČ ¸®ĽŇ˝ş şŻ°ć ˝Ă˝şĹŰ ±¸Çö
				//ITEM * pItem = g_pDatabase->GetServerItemInfo(pEnemy->m_infoCharacter.CharacterRenderInfo.RI_Center);
				// żÜÇüľĆŔĚĹŰąřČŁ°ˇ ľř´Ů¸é żř·ˇ ľĆŔĚĹŰ ąřČŁ¸¦ »çżëÇŃ´Ů
				int nItemNum = 0;
				if( pEnemy->m_infoCharacter.CharacterRenderInfo.RI_Center_ShapeItemNum )
					nItemNum = pEnemy->m_infoCharacter.CharacterRenderInfo.RI_Center_ShapeItemNum;
				else
					nItemNum = pEnemy->m_infoCharacter.CharacterRenderInfo.RI_Center;

				ITEM * pItem = g_pDatabase->GetServerItemInfo( nItemNum );
				// end 2009. 08. 27 by ckPark ±×·ˇÇČ ¸®ĽŇ˝ş şŻ°ć ˝Ă˝şĹŰ ±¸Çö
				// 10. 03. 18 by jskim ¸ó˝şĹÍşŻ˝Ĺ ˝şĹł
				BOOL bMonTras = FALSE;
				if(pEnemy->GetMonsterTransformer())
				{
					// 10. 03. 18 by jskim ¸ó˝şĹÍşŻ˝Ĺ ˝şĹł
					pEnemy->ChangeUnitCharacterInfo(nItemNum, pEnemy->m_infoCharacter.CharacterInfo.UnitKind, pMsg->CharacterInfo.BodyCondition, FALSE);
					pEnemy->SetMonsterTransPrimarySidePos(pEnemy->m_PrimaryAttack.vSidePos);
					pEnemy->SetMonsterTransSecondarySidePos(pEnemy->m_SecondaryAttack.vSidePos);
				}
				//end 10. 03. 18 by jskim ¸ó˝şĹÍşŻ˝Ĺ ˝şĹł
				if(pItem)
				{
					// 10. 03. 18 by jskim ¸ó˝şĹÍşŻ˝Ĺ ˝şĹł
					if(pEnemy->GetMonsterTransformer())
					{
						bMonTras = TRUE;						
						MEX_MONSTER_INFO * pTempIn = g_pGameMain->CheckMonsterInfo( pEnemy->GetMonsterTransformer() );
						nItemNum = pTempIn->SourceIndex;			
						pEnemy->ChangeUnitCharacterInfo(nItemNum, 
							pEnemy->m_infoCharacter.CharacterInfo.UnitKind, pMsg->CharacterInfo.BodyCondition, FALSE , bMonTras);
					}	
					else
					{
						pEnemy->ChangeUnitCharacterInfo(pItem->SourceIndex, 
						pEnemy->m_infoCharacter.CharacterInfo.UnitKind, pMsg->CharacterInfo.BodyCondition, FALSE , bMonTras);
					}
					//end 10. 03. 18 by jskim ¸ó˝şĹÍşŻ˝Ĺ ˝şĹł
				}
				else
				{
					// µđĆúĆ®
					ChangeUnitCharacterInfo(0, 
						pEnemy->m_infoCharacter.CharacterInfo.UnitKind, pMsg->CharacterInfo.BodyCondition, FALSE);
				}
				
				// 2005-07-28 by ispark
				// Äł¸ŻĹÍ ·»´ő¸µ ąřČŁ ŔÔ·Â
				pEnemy->SetPilotNumber(pEnemy->m_infoCharacter.CharacterInfo.PilotFace);
			
			pEnemy->ChangeUnitBodyCondition(pMsg->CharacterInfo.BodyCondition);
			// ľĆŔĚĹŰ ·»´ő¸µŔ» Ŕ§ÇŃ Á¤ş¸ ŔúŔĺ
			memcpy(&pEnemy->m_infoCharacter.CharacterRenderInfo,&pMsg->CharacterRenderInfo,sizeof(CHARACTER_RENDER_INFO));
			}
		}
		else
		{
			// 2006-07-05 by ispark »ó´ë ĽĽ·Â ¸ŕĆ® »čÁ¦
 			if(!IsInfluenceCharacter(g_pShuttleChild->m_myShuttleInfo.InfluenceType, pMsg->CharacterInfo.InfluenceType))
			{
				memset(pMsg->CharacterInfo.szCharacterMent, 0x00, sizeof(pMsg->CharacterInfo.szCharacterMent));
			}
			pEnemy = new CEnemyData(pMsg);
			g_pScene->InsertToBlockData(pEnemy);
//			pEnemy->m_infoCharacter.CharacterInfo = pMsg->CharacterInfo;
//			pEnemy->m_infoCharacter.ClientIndex = pMsg->CharacterInfo.ClientIndex;
//			pEnemy->m_infoCharacter.CharacterUniqueNumber = pMsg->CharacterInfo.CharacterUniqueNumber;
//			strcpy(pEnemy->m_infoCharacter.CharacterName,pMsg->CharacterInfo.CharacterName);//,sizeof(SIZE_MAX_CHARACTER_NAME));
//			pEnemy->m_infoCharacter.UnitKind = pMsg->CharacterInfo.UnitKind;
//			pEnemy->m_infoCharacter.Race = pMsg->CharacterInfo.Race;
//			pEnemy->m_infoCharacter.GuildUniqueNumber = pMsg->CharacterInfo.GuildUniqueNumber;
//			pEnemy->m_infoCharacter.BodyCondition = pMsg->CharacterInfo.BodyCondition;

//			pEnemy->m_vPos = A2DX(pMsg->CharacterInfo.PositionVector);
//			pEnemy->m_vNextPos = A2DX(pMsg->CharacterInfo.PositionVector);
//			if(COMPARE_BODYCON_BIT(pMsg->CharacterInfo.BodyCondition,BODYCON_DEAD_MASK))
//			{
//				pEnemy->ChangeUnitState( _FALLEN );
//				pMsg->CharacterInfo.BodyCondition = BODYCON_DEAD_MASK | BODYCON_DAMAGE1_MASK | BODYCON_DAMAGE2_MASK | BODYCON_DAMAGE3_MASK;
//			}
//			pEnemy->m_infoCharacter.CharacterRenderInfo = pMsg->CharacterRenderInfo;
			pEnemy->Init();
			if (MAP_INFLUENCE_PVP_ALL == g_pD3dApp->GetMyShuttleMapInfo()->MapInfluenceType)
			strcpy(((CChatMoveData*)pEnemy->m_pIDChat->m_pChild)->m_szString, "FFA Enemy");
			else
			strcpy(((CChatMoveData *)pEnemy->m_pIDChat->m_pChild)->m_szString,pEnemy->m_infoCharacter.CharacterInfo.CharacterName);
			
			// 2007-11-22 by bhsohn ľĆ·ąłŞ ĹëÇŐĽ­ąö
			// ľĆ·ąłŞ ¸ĘżˇĽ­´Â '\'µÚ´Â Ľ­ąöŔĚ¸§ŔĚąÇ·Î şü¸± ÇĘżä°ˇľř´Ů.
			g_pD3dApp->ConevertArenaRenderUserName(g_pD3dApp->GetArenaState(), ((CChatMoveData *)pEnemy->m_pIDChat->m_pChild)->m_szString);
			
			g_pScene->m_mapEnemyList[pEnemy->m_infoCharacter.CharacterInfo.ClientIndex] = pEnemy;
			pEnemy->ChangeBodyConditionFromServer(pMsg->CharacterInfo.BodyCondition);
		}
		pEnemy->InitItemPoint();
 		PARTYENEMYINFO* pPartyEnemtInfo = m_pClientParty->PartyFindMemberByUniqueNumber(pMsg->CharacterInfo.CharacterUniqueNumber);
		if(pPartyEnemtInfo)
		{
			if(pPartyEnemtInfo->m_bPartyType == _PARTYOTHERMAPMASTER)
			{
				pPartyEnemtInfo->m_bPartyType = _PARTYMASTER;
			}
			else if(pPartyEnemtInfo->m_bPartyType == _PARTYOTHERMAPMEMBER)
			{
				pPartyEnemtInfo->m_bPartyType = _PARTYMEMBER;
			}
			pEnemy->m_nPartyID = m_pClientParty->GetPartyInfo().PartyID;
			pPartyEnemtInfo->m_pEnemyData = pEnemy;
			// ĆÄĆĽ Á¤ş¸ żäĂ»;
			MSG_FC_PARTY_GET_MEMBER sMsg;
			sMsg.CharacterUniqueNumber = pMsg->CharacterInfo.CharacterUniqueNumber;
			g_pFieldWinSocket->SendMsg(T_FC_PARTY_GET_MEMBER, (char*)&sMsg, sizeof(sMsg));
		}
	}
}

void CShuttleChild::CheckIndexList(MSG_FC_CHARACTER_GET_MONSTER_INFO_OK* pMsg)
{
	FLOG( "CShuttleChild::CheckIndexList(MSG_FC_CHARACTER_GET_MONSTER_INFO_OK* pMsg)" );
	CMapMonsterIterator itMonster = g_pScene->m_mapMonsterList.find(pMsg->MonsterIndex);
	if(itMonster != g_pScene->m_mapMonsterList.end())
	{
		memcpy(&itMonster->second->m_info,pMsg,sizeof(MSG_FC_CHARACTER_GET_MONSTER_INFO_OK));
		return;
	}
	if(pMsg->MonsterIndex == m_nGetOtherIndex)
	{
		m_bGetOtherInfo = FALSE;
		m_fGetOtherInfoCheckTime = 0.0f;
		m_nGetOtherIndex = 0;
	}
	CMonsterData * pMonster = new CMonsterData(pMsg);
//	DBGOUT("MONSTER[%d] RENDER_INFO_OK Distance[%d]\n",pMsg->ClientIndex,
//			(int)g_pD3dApp->Distance3DTo2D(m_vPos, A2DX(pMsg->PositionVector)));
	g_pScene->InsertToBlockData(pMonster);
	// ąŮµđÄÁµđĽÇ Á¤¸®(newżˇĽ­ ´Ů Ăł¸®µĘ)
/*	pMonster->ChangeBodyConditionFromServer(pMsg->BodyCondition);
	if(pMonster->m_pCharacterInfo)
	{
		pMonster->m_pCharacterInfo->SetBodyCondition(pMsg->BodyCondition);
	}
	if(COMPARE_BODYCON_BIT(pMonster->GetCurrentBodyCondition(),BODYCON_DEAD_MASK))
	{
		//pMonster->m_dwState = _EXPLODED;
		pMonster->ChangeUnitState( _EXPLODED );
	}
*/	g_pScene->m_mapMonsterList[pMsg->MonsterIndex] = pMonster;
}

void CShuttleChild::CheckMove(MSG_FC_MOVE_OK* pMsg)
{
	FLOG( "CShuttleChild::CheckMove(MSG_FC_MOVE_OK* pMsg)" );
	if(m_myShuttleInfo.ClientIndex == pMsg->ClientIndex)
	{
		return;
	}
	else
	{
		if(pMsg->ClientIndex < 10000)
		{
			CMapEnemyIterator itEnemy = g_pScene->m_mapEnemyList.find(pMsg->ClientIndex);
			// 2005-12-30 by ispark, Enemy°ˇ ¸®˝şĆ®żˇ ľřŔ» ¶§ Á¤ş¸ żäĂ»
			if(itEnemy != g_pScene->m_mapEnemyList.end())
			{
				if(m_pClientParty->FormationFlightFindMemberByClientIndex(pMsg->ClientIndex) == NULL)
				{
					itEnemy->second->SetPosition(pMsg);
				}
				
				// 2007-04-02 by bhsohn MoveĆĐĹ¶Ŕ¸·Î Ŕű±â »óĹÂ ĂĽĹ©
				// ÇöŔç Ŕű±â »óĹÂżÍ żě¸®ŔÇ »óĹÂ°ˇ şŻ°ć˝Ă °­Á¦·Î Ŕű±â ĽÂĆĂ
				UpdateEnemyState(itEnemy->second, pMsg);
				// end 2007-04-02 by bhsohn MoveĆĐĹ¶Ŕ¸·Î Ŕű±â »óĹÂ ĂĽĹ©
			}
			else
			{
				// 2005-12-30 by ispark, ŔÚÁÖ żäĂ» ÇŇ °ˇ¸ÁĽşŔĚ ŔÖŔ˝ 
//				if(!m_bGetOtherInfo)
				{
 					m_bGetOtherInfo = TRUE;
					//float fCounter = (30 - min(g_pD3dApp->m_fFPS, 30)) * ((g_pD3dApp->m_fPingLat/2)*1000);
					m_fGetOtherInfoCheckTime = 0.5f;
					m_nGetOtherIndex = pMsg->ClientIndex;

					// 2006-06-15 by ispark, ŔĎÁ¤˝Ă°Łżˇ ĆĐĹ¶Ŕ» ş¸ł»´Â °ÍŔ¸·Î şŻ°ć
//					MSG_FC_CHARACTER_GET_OTHER_INFO sMsg; 
//					memset(&sMsg,0x00,sizeof(sMsg));
//					char buffer[SIZE_MAX_PACKET];
//					sMsg.ClientIndex = pMsg->ClientIndex;
//					int nType = T_FC_CHARACTER_GET_OTHER_INFO;
//					memcpy(buffer, &nType, SIZE_FIELD_TYPE_HEADER);
//					memcpy(buffer+SIZE_FIELD_TYPE_HEADER, &sMsg, sizeof(sMsg));
//					g_pD3dApp->m_pFieldWinSocket->Write(buffer, SIZE_FIELD_TYPE_HEADER + sizeof(sMsg));
					SetClientIndexDataByVector(g_pD3dApp->m_vecCharacterOtherInfo, pMsg->ClientIndex);

					if( UNIT_STATE_NO_MOVE(m_dwState))
					{
//						MSG_FC_MOVE_TARGET sMsg2;
//						sMsg2.ClientIndex = pMsg->ClientIndex;
//						g_pFieldWinSocket->SendMsg( T_FC_MOVE_TARGET, (char*)&sMsg2, sizeof(sMsg2));
						SetClientIndexDataByVector(g_pD3dApp->m_vecMoveTarget, pMsg->ClientIndex);
					}
				}
			}
		}
		else
		{
			CMapMonsterIterator itMonster = g_pScene->m_mapMonsterList.find(pMsg->ClientIndex);
			if(itMonster != g_pScene->m_mapMonsterList.end())
			{
				itMonster->second->SetPosition(pMsg);
			}
			else
			{
				// ¸®˝şĆ®żˇ ľřŔ¸¸é Ĺ¬¶óŔĚľđĆ® Á¤ş¸ żäĂ»
				if(!m_bGetOtherInfo)
				{
					m_bGetOtherInfo = TRUE;
				//	float fCounter = (30 - min(g_pD3dApp->m_fFPS, 30)) * ((g_pD3dApp->m_fPingLat / 2) * 1000);
					m_fGetOtherInfoCheckTime = 0.5f;
					m_nGetOtherIndex = pMsg->ClientIndex;

					// 2006-06-15 by ispark, ŔĎÁ¤˝Ă°Łżˇ ĆĐĹ¶Ŕ» ş¸ł»´Â °ÍŔ¸·Î şŻ°ć
//					MSG_FC_CHARACTER_GET_OTHER_INFO sMsg;
//					memset(&sMsg,0x00,sizeof(sMsg));
//					char buffer[SIZE_MAX_PACKET];
//					sMsg.ClientIndex = pMsg->ClientIndex;
//					int nType = T_FC_CHARACTER_GET_OTHER_INFO;
//					memcpy(buffer, &nType, SIZE_FIELD_TYPE_HEADER);
//					memcpy(buffer+SIZE_FIELD_TYPE_HEADER, &sMsg, sizeof(sMsg));
//					g_pD3dApp->m_pFieldWinSocket->Write(buffer, SIZE_FIELD_TYPE_HEADER + sizeof(sMsg));
					SetClientIndexDataByVector(g_pD3dApp->m_vecCharacterOtherInfo, pMsg->ClientIndex);
				}
			}
		}
	}
}


///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		Ŕű±âŔÇ »óĹÂ¸¦ °»˝ĹÇŘÁŘ´Ů. 
/// \author		// 2007-04-02 by bhsohn MoveĆĐĹ¶Ŕ¸·Î Ŕű±â »óĹÂ ĂĽĹ©
/// \date		2007-04-02 ~ 2007-04-02
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CShuttleChild::UpdateEnemyState(CEnemyData *pEnemyData, MSG_FC_MOVE_OK* pMsg)
{
	if(g_pShuttleChild->m_myShuttleInfo.ClientIndex == pMsg->ClientIndex)
	{
		// ŔÚ±â ŔÚ˝ĹŔĚ´Ů. 
		return;
	}
	// Ŕű±â°ˇ ľĆ´Ď´Ů. 
	if (!IS_CHARACTER_CLIENT_INDEX(pMsg->ClientIndex))
	{
		return;
	}
	
	// Enemy skill ąßµż˝Ă
	CUnitData* pUnit = g_pScene->FindUnitDataByClientIndex( pMsg->ClientIndex );
	if(NULL == pUnit)
	{
		return;
	}
	float fElaspedTime = g_pD3dApp->GetCheckElapsedTime();
	float fSkillTime = 0;
	INT ItemNum = 0;
	bool bTmp = false;

	// ˝şĹł ·ˇş§ 1 ·Î °­Á¦ ĽÂĆĂ
	INT nSkillUid[UNIT_STATE_MAX] = 
	{
		BGEAR_SKILL_BASENUM_INVISIBLE + 1,		// ŔÎşńÁöşí
		BGEAR_STATE_CHARINGSHOT + 1,			// Â÷ÂˇĽ¦
		AGEAR_STATE_HYPERSHOT + 1				// ÇĎŔĚĆŰĽ¦
		, UNIT_STATE_BARRIER	+1
// end 2013-01-30 by mspark, A±âľî ąč¸®ľî ˝şĹł żöÇÁČÄżˇ »ó´ëąćżˇ°Ô ş¸ŔĚÁö ľĘ´Â ą®Á¦ ĽöÁ¤
// 2013-02-14 by mspark, M±âľî ą«Ŕű ˝şĹł żöÇÁČÄżˇ »ó´ëąćżˇ°Ô ş¸ŔĚÁö ľĘ´Â ą®Á¦ ĽöÁ¤
, UNIT_STATE_INVINCIBLE	+1
// end 2013-02-14 by mspark, M±âľî ą«Ŕű ˝şĹł żöÇÁČÄżˇ »ó´ëąćżˇ°Ô ş¸ŔĚÁö ľĘ´Â ą®Á¦ ĽöÁ¤
// 2012-11-29 by mspark, Ľ­ÄˇľĆŔĚ Ŕ§Äˇ°ˇ ŔĚµżÇĎ´Â Äł¸ŻĹÍŔÇ Ŕ§ÄˇżÍ µżŔĎÇĎµµ·Ď ĽöÁ¤
, UNIT_STATE_SEARCHEYE	+1
, UNIT_STATE_SEARCHEYE2	+1
	};
	
	// ŔÎşńÁöşí
	bTmp = pUnit->IsUnitState(UNIT_STATE_INVISIBLE);
	if(pMsg->moveBitFlag.Invisible0 != bTmp)
	{
		pUnit->AddSkillChgTime(UNIT_STATE_INVISIBLE, fElaspedTime); // ˝Ă°Ł Ăß°ˇ
		fSkillTime = pUnit->GetSkillChgTime(UNIT_STATE_INVISIBLE);

		if(fSkillTime > UNIT_CHG_STATE_TIME)
		{
			pUnit->AddSkillChgTime(UNIT_STATE_INVISIBLE, -1);	// şŻĽö ĂĘ±âČ­ 
			bTmp ^= TRUE;	//==(bTmp = !bTmp)
			pUnit->SetUnitSkillState(UNIT_STATE_INVISIBLE, bTmp);
			if(bTmp)
			{
				// ON				
				SetEmenyState(pUnit, nSkillUid[UNIT_STATE_INVISIBLE]);
			}
			else
			{
				ClearEmenyState(pUnit, nSkillUid[UNIT_STATE_INVISIBLE]);
			}

		}		
	}
	else
	{
		pUnit->AddSkillChgTime(UNIT_STATE_INVISIBLE, -1);	// şŻĽö ĂĘ±âČ­ 
	}
	
	// Â÷ÂˇĽ¦	
	bTmp = pUnit->IsUnitState(UNIT_STATE_CHARINGSHOT);
	if(pMsg->moveBitFlag.ChargingSkill != bTmp)
	{
		pUnit->AddSkillChgTime(UNIT_STATE_CHARINGSHOT, fElaspedTime);
		fSkillTime = pUnit->GetSkillChgTime(UNIT_STATE_CHARINGSHOT);
		
		if(fSkillTime > UNIT_CHG_STATE_TIME)
		{
			pUnit->AddSkillChgTime(UNIT_STATE_CHARINGSHOT, -1);	// şŻĽö ĂĘ±âČ­ 			
			bTmp ^= TRUE;
			pUnit->SetUnitSkillState(UNIT_STATE_CHARINGSHOT, bTmp);
			if(bTmp)
			{
				// ON				
				SetEmenyState(pUnit, nSkillUid[UNIT_STATE_CHARINGSHOT]);				
			}
			else
			{
				ClearEmenyState(pUnit, nSkillUid[UNIT_STATE_CHARINGSHOT]);
			}		
		}
	}		
	else
	{
		pUnit->AddSkillChgTime(UNIT_STATE_CHARINGSHOT, -1);	// şŻĽö ĂĘ±âČ­ 			
	}
	
	// ÇĎŔĚĆŰĽ¦
	bTmp = pUnit->IsUnitState(UNIT_STATE_HYPERSHOT);
	if(pMsg->moveBitFlag.HyperShot != bTmp)
	{
		pUnit->AddSkillChgTime(UNIT_STATE_HYPERSHOT, fElaspedTime);
		fSkillTime = pUnit->GetSkillChgTime(UNIT_STATE_HYPERSHOT);
		
		if(fSkillTime > UNIT_CHG_STATE_TIME)
		{
			pUnit->AddSkillChgTime(UNIT_STATE_HYPERSHOT, -1);	// şŻĽö ĂĘ±âČ­ 	
			bTmp ^= TRUE;
			pUnit->SetUnitSkillState(UNIT_STATE_HYPERSHOT, bTmp);
			if(bTmp)
			{
				// ON				
				SetEmenyState(pUnit, nSkillUid[UNIT_STATE_HYPERSHOT]);				
			}
			else
			{
				ClearEmenyState(pUnit, nSkillUid[UNIT_STATE_HYPERSHOT]);
			}
		}
	}
	else
	{
		pUnit->AddSkillChgTime(UNIT_STATE_HYPERSHOT, -1);	// şŻĽö ĂĘ±âČ­ 	
	}
	
}	

void CShuttleChild::SetEmenyState(CUnitData* pUnit, INT ItemNum)
{
	if(NULL == pUnit)
	{
		return;
	}
	// Skill Prepare
	if(pUnit->m_pSkillEffect)
	{
		pUnit->m_pSkillEffect->CreateSkillEffect(ItemNum, BODYCON_CHARGING_MASK);
	}

	// USE_SKILL_OK
	CEnemyData *pEnemyData = ((CEnemyData *)pUnit);	
	
	pEnemyData->UseSkillFromServer(ItemNum);
	ITEM* pItem = g_pDatabase->GetServerItemInfo(ItemNum);
	
	if(pUnit->m_pSkillEffect)
	{
		if(pUnit->m_pSkillEffect->FindSkillEffect(ItemNum))
		{ 
			// ŔĚąĚ »ýĽşµČ ˝şĹłŔĚ ŔÖ´Ů¸é..
			pUnit->m_pSkillEffect->ChangeBodyCondition(ItemNum, BODYCON_USE_SKILL_MASK);
		}
		else
		{ // »ýĽşµČ ˝şĹłŔĚ ľř´Ů¸é »ýĽşÇŃ´Ů.
			
			// Ľř°ŁŔĚĆĺĆ® - łŞÁßżˇ ąŮ˛ŮŔÚ..
			// 2005-08-23 by ispark
			CAppEffectData* pEff = new CAppEffectData(pUnit,SKILL_BASE_NUM(pItem->SourceIndex),D3DXVECTOR3(0,0,0));
			if(pEff->m_bUsing)
			{
				pEff = (CAppEffectData *)g_pD3dApp->m_pEffectList->AddChild(pEff);
			}
			else
			{
				delete pEff;
			}
		}
	}
}

void CShuttleChild::ClearEmenyState(CUnitData* pUnit, INT ItemNum)
{
	if(NULL == pUnit)
	{
		return;
	}			
	
	CEnemyData* pEnemy = ((CEnemyData *)pUnit);
	pEnemy->CancelUsingSkill(ItemNum);
}


void CShuttleChild::HandleInput(float fElapsedTime)
{
	FLOG( "CShuttleChild::HandleInput(float fElapsedTime)" );
	if(g_pD3dApp->GetHwnd()!=GetActiveWindow())
		return;

	static float fNkeyTime = 0.0f;
	static float fLockonkeyTime = 1.0f;

	static float fFkeyTime = 0.0f;
	static float fVkeyTime = 0.0f;
	static float fTabkeyTime = 0.0f;
//	static float fInsertKeyCheckTime = 0.0f;

	if(fFkeyTime >= 0.0f)
		fFkeyTime -= fElapsedTime;
	if(fVkeyTime >= 0.0f)
		fVkeyTime -= fElapsedTime;
	if(fTabkeyTime >= 0.0f)
		fTabkeyTime -= fElapsedTime;

//	if(fInsertKeyCheckTime >= 0.0f)
//		fInsertKeyCheckTime -= fElapsedTime;

	static float fDeletekeyTime = 0.0f;
	if(fDeletekeyTime >= 0.0f)
		fDeletekeyTime -= fElapsedTime;

	if(m_fCancelSkillCheckTime >= 0.0f)
		m_fCancelSkillCheckTime -= fElapsedTime;
	if(m_fChangeBodyForBoosterCheckTime >= 0.0f)
		m_fChangeBodyForBoosterCheckTime -= fElapsedTime;
	if(m_fNumpad0keyTime >= 0.0f)
		m_fNumpad0keyTime -= fElapsedTime;

	if( FAILED( g_pD3dApp->m_pKeyBoard->ReadImmediateData( g_pD3dApp->GetHwnd() ) ) )
	{
		return;
	}
#ifdef _DEBUG
	if(COMPARE_RACE(m_myShuttleInfo.Race,RACE_OPERATION|RACE_GAMEMASTER))
	{
		if ( !g_pD3dApp->m_pKeyBoard->GetAsyncKeyState( DIK_LCONTROL )
			&& !g_pD3dApp->m_pKeyBoard->GetAsyncKeyState( DIK_LMENU )
			&& g_pD3dApp->m_pKeyBoard->GetAsyncKeyState( DIK_ADD ) )
		{
			m_fMouseRate += 0.1f*fElapsedTime;
		}
		if ( !g_pD3dApp->m_pKeyBoard->GetAsyncKeyState( DIK_LCONTROL ) 
			&& !g_pD3dApp->m_pKeyBoard->GetAsyncKeyState( DIK_LMENU )
			&& g_pD3dApp->m_pKeyBoard->GetAsyncKeyState( DIK_SUBTRACT ) )
		{
			m_fMouseRate -= 0.1f*fElapsedTime;
			if(m_fMouseRate < 0.0f)
			{
				m_fMouseRate = 0.0f;
			}
		}
		if ( g_pD3dApp->m_pKeyBoard->GetAsyncKeyState( DIK_LCONTROL ) 
			&& g_pD3dApp->m_pKeyBoard->GetAsyncKeyState( DIK_ADD ) )
		{
			g_pD3dApp->m_pCamera->SetDetailGround(0,true);
		}
		if ( g_pD3dApp->m_pKeyBoard->GetAsyncKeyState( DIK_LCONTROL ) 
			&& g_pD3dApp->m_pKeyBoard->GetAsyncKeyState( DIK_SUBTRACT ) )
		{
			g_pD3dApp->m_pCamera->SetDetailGround(0);
		}
		if ( g_pD3dApp->m_pKeyBoard->GetAsyncKeyState( DIK_LMENU ) 
			&& g_pD3dApp->m_pKeyBoard->GetAsyncKeyState( DIK_ADD ) )
		{
			g_pD3dApp->m_pCamera->SetDetailGround(1,true);
		}
		if ( g_pD3dApp->m_pKeyBoard->GetAsyncKeyState( DIK_LMENU ) 
			&& g_pD3dApp->m_pKeyBoard->GetAsyncKeyState( DIK_SUBTRACT ) )
		{
			g_pD3dApp->m_pCamera->SetDetailGround( 1 );
		}
	}
#endif // _DEBUG_endif
/*	if(g_pD3dApp->m_pKeyBoard->GetAsyncKeyState(DIK_TAB) && fTabkeyTime <= 0.0f)
	{// °­Á¦ ¶ôżÂŔ» ´Ů¸Ą Ĺ¸°ŮŔ¸·Î ąŮ˛Ű´Ů
		bool bCheck = false;
//		if(m_bPKMode)
		if(IsPkEnable())
		{
			CVecEnemyIterator itEnemy = g_pScene->m_vecEnemyRenderList.begin();
			while(itEnemy != g_pScene->m_vecEnemyRenderList.end())
			{
//				if(bCheck && (*itEnemy)->m_dwAttackEnemy != PK_NONE)
				if(bCheck && (*itEnemy)->IsPkEnable() && ATTACK_AVAILABLE_STATE((*itEnemy)->m_dwState))
				{
					m_pOrderTarget = (*itEnemy);
					break;
				}
				if((*itEnemy) == m_pOrderTarget)
				{
					bCheck = true;
					m_pOrderTarget = NULL;
				}
				itEnemy++;
			}
		}
		if(!bCheck || !m_pOrderTarget)
		{
			CVecMonsterIterator itMon = g_pScene->m_vecMonsterRenderList.begin();
			while(itMon != g_pScene->m_vecMonsterRenderList.end())
			{
				if(bCheck && ATTACK_AVAILABLE_STATE((*itMon)->m_dwState))
				{
					m_pOrderTarget = (*itMon);
					break;
				}
				if(*itMon == m_pOrderTarget)
				{
					bCheck = true;
					m_pOrderTarget = NULL;
				}
				itMon++;
			}
		}
		if(!bCheck || !m_pOrderTarget)
		{// °­Á¦ ¶ôżÂ °É Ĺ¸°ŮŔĚ ľřŔ»¶§ ±âÁ¸ ¶ôżÂŔ» ÇŘÁ¦˝ĂĹ°´Â±â´ÉŔ» ÇĎŔÚ
			CVecMonsterIterator itMon = g_pScene->m_vecMonsterRenderList.begin();
			if(itMon != g_pScene->m_vecMonsterRenderList.end() && ATTACK_AVAILABLE_STATE((*itMon)->m_dwState))
			{
				if(m_pOrderTarget != *itMon)
				{
					m_pOrderTarget = *itMon;
					g_pD3dApp->m_pSound->PlayD3DSound(SOUND_LOCKON_PRIMARY, g_pShuttleChild->m_vPos, FALSE);
				}
			}
		}
		fTabkeyTime = 0.5f;
	}

*/
/*	
	if(COMPARE_RACE(m_myShuttleInfo.Race,RACE_OPERATION|RACE_GAMEMASTER))
	{
		if(g_pD3dApp->m_pKeyBoard->GetAsyncKeyState( DIK_LCONTROL) && g_pD3dApp->m_pKeyBoard->GetAsyncKeyState( DIK_F12))
		{
			if(!m_bSkillMoveIsUse)
			{
				m_bSkillMoveIsUse = TRUE;
				m_vSkillMoveVel = m_vVel;
				m_fSkillMoveTime = 0.0f;
				m_bSkillMoveType++;
				if(m_bSkillMoveType > 9)
					m_bSkillMoveType = 0;
			}
		}
		if(g_pD3dApp->m_pKeyBoard->GetAsyncKeyState(DIK_LCONTROL) && g_pD3dApp->m_pKeyBoard->GetAsyncKeyState(DIK_L))
		{// Áö»ó Ćř°Ý ¸đµĺ ŔüČŻ
			if(fDeletekeyTime <= 0.0f)
			{
				m_bSecondaryAttackMode++;
				if(m_bSecondaryAttackMode == 2)
					m_bSecondaryAttackMode = 0;
				fDeletekeyTime = 1.0f;
			}
		}
	}
*/
	// 2009-04-23 by bhsohn żˇµđĆ®Ă˘ ÄżĽ­ ŔĚµż
	//if(m_dwState != _LANDING && g_pD3dApp->m_pKeyBoard->GetAsyncKeyState(DIK_DELETE))
	if(m_dwState != _LANDING && !g_pD3dApp->m_bChatMode && g_pD3dApp->m_pKeyBoard->GetAsyncKeyState(DIK_DELETE))
	{
		if(g_pGameMain->m_pMp3Player->m_bMp3PlayListShow == FALSE)
		{
			// 2005-04-07 by jschoi - Tutorial
			if(g_pTutorial->IsTutorialMode() == FALSE)
			{
				if(fDeletekeyTime <= 0.0f)
				{
					fDeletekeyTime = 1.0f;
					// 2009-04-07 by bhsohn żˇľî ˝ĂÁî˝Ă, ˝ĂÁˇ ľČąŮ˛î°Ô ÇÔ										
					//if(m_bIsAir == TRUE)
					if((m_bIsAir == TRUE) && (_SIEGE != m_bAttackMode) && m_InfiState)	//2011-07-20 by jhahn	ŔÎÇÇ3Â÷ ˝Ăł×¸¶ Áß  ˝ĂÁˇşŻ°ć ±ÝÁö
					{				
						g_pD3dApp->m_pCamera->ChangeCamType();
						g_pInterface->SaveOptionFile();
					}
				}
			}
		}
	}

/*	if(g_pD3dApp->m_pKeyBoard->GetAsyncKeyState( DIK_INSERT))
	{
		if(fInsertKeyCheckTime <= 0.0f)
		{
			fInsertKeyCheckTime = 0.5f;
			m_bMoveInputType++;
			if(m_bMoveInputType == 2)
				m_bMoveInputType = 0;
			g_pSOption->InputType  = m_bMoveInputType ;
			if(g_pSOption->InputType)
			 strcpy(g_pGameMain->m_pSystem->m_strSelectModeComboData[4], "ľ×ĆĽşę");
			else
			 strcpy(g_pGameMain->m_pSystem->m_strSelectModeComboData[4],  "żŔĹä");
			g_pInterface->m_pGageBox->SetControlMode(m_bMoveInputType);
		}
	}
*/	
//	if(fLockonkeyTime >= 0.0f)
//		fLockonkeyTime -= fElapsedTime;
//	if( (m_bAutoLockon || 
//		g_pD3dApp->m_pKeyBoard->GetAsyncKeyState(DIK_LCONTROL) ) && 
//		fLockonkeyTime < 0.0f )
//	{
//		if(m_pOrderTarget)
//		{
//			if(g_pD3dApp->m_pKeyBoard->GetAsyncKeyState( DIK_LCONTROL) )
//			{
//				if(	m_pOrderTarget->m_dwPartType == _ENEMY &&						// 1. żŔ´őĹ¸°ŮŔĚ Enemyż´°í,
//					((CEnemyData*)m_pOrderTarget)->GetPkState() == PK_NORMAL &&		// 2. ĽřĽö °­Á¦ Ĺ¸°ŮŔĚľú´Ů¸é.
//					g_pShuttleChild->m_pPkNormalTimer->IsCityWar() == FALSE)		// 3. µµ˝Ă Áˇ·ÉŔüÁßżˇ´Â ş¸ł»Áö ľĘ´Â´Ů.
//				{	// Ľ­ąö·Î ş¸łż
//					MSG_FC_MOVE_UNLOCKON sMsg;
//					sMsg.AttackIndex = g_pShuttleChild->m_myShuttleInfo.ClientIndex;
//					sMsg.TargetIndex = ((CEnemyData*)m_pOrderTarget)->m_infoCharacter.CharacterInfo.ClientIndex;	
//					g_pFieldWinSocket->SendMsg( T_FC_MOVE_UNLOCKON, (char*)&sMsg, sizeof(sMsg) );				
//				}
//				g_pD3dApp->m_pChat->CreateChatChild(STRMSG_C_INTERFACE_0001,COLOR_SKILL_USE);
////				m_pTarget = NULL;
//				m_pOrderTarget = NULL;
//				fLockonkeyTime = 0.5f;
//				m_pPkNormalTimer->ReleasePkDelay();
//			}
//		}
//		else if(IsPkEnableMap(m_myShuttleInfo.MapChannelIndex.MapIndex) == FALSE )
//		{
//			CMapEnemyIterator itEnemy = g_pScene->m_mapEnemyList.begin();
//			if(itEnemy != g_pScene->m_mapEnemyList.end() && )
//			{
//				g_pD3dApp->m_pChat->CreateChatChild("PK ¶ôżÂ ŔŰµż",COLOR_SKILL_USE);
//				m_pOrderTarget = itEnemy->second;
//				fLockonkeyTime = 0.5f;
//				g_pD3dApp->m_pSound->PlayD3DSound(300012,m_vPos);
//			}
//			if(!m_pOrderTarget)
//			{
//				CMapMonsterIterator itMonster = g_pScene->m_mapMonsterList.begin();
//				while(itMonster != g_pScene->m_mapMonsterList.end())
//				{
//					if(itMonster->second->m_dwState == _NORMAL )//&& 
//					{
//						g_pD3dApp->m_pChat->CreateChatChild("¶ôżÂ ŔŰµż",COLOR_SKILL_USE);
//						m_pOrderTarget = itMonster->second;
//						fLockonkeyTime = 0.5f;
//						g_pD3dApp->m_pSound->PlayD3DSound(300012,m_vPos);
//						break;
//					}
//					itMonster++;
//				}
//			}
//		}
//	}
/*
	if(g_pD3dApp->m_pKeyBoard->GetAsyncKeyState(DIK_PRIOR))
	{
		if(g_pD3dApp->m_pCamera->m_bIsCamControl)
		{
			g_pD3dApp->m_pCamera->SetControlCamAngle(10);
		}
	}
	else if(g_pD3dApp->m_pKeyBoard->GetAsyncKeyState(DIK_NEXT))
	{
		if(g_pD3dApp->m_pCamera->m_bIsCamControl)
		{
			g_pD3dApp->m_pCamera->SetControlCamAngle(-10);
		}
	}
	else
	{
		g_pD3dApp->m_pCamera->SetControlCamAngle(0,FALSE);
	}
*/
}
void CShuttleChild::Render()
{
	FLOG( "CShuttleChild::Render()" );
	// 2007-03-26 by dgwoo żÉŔúąö ¸đµĺ˝Ă ±âľî¸¦ ±×¸®Áö ľĘ´Â´Ů.
	if(IsOperAndObser())
	{
		return;
	}

	// 2007-03-07 by bhsohn ˝ĂÁî ¸đµĺ ąö±× ĽöÁ¤	
	BOOL bCameraStatus = (g_pCamera->GetCamType() == CAMERA_TYPE_NORMAL) && m_bAttackMode != _SIEGE;
	if(FALSE == bCameraStatus)
	{
		BOOL bCamContorl = g_pCamera->m_bIsCamControl || m_bTurnCamera;
		// Ä«¸Ţ¶ó ¸đµĺŔÎÁö ĆÇ´Ü. 
		if( bCamContorl && _SIEGE == m_bAttackMode )
		{
			// ˝ĂÁî¸éĽ­ Ä«¸Ţ¶ó Á¦ľî¸đµĺ 
			bCameraStatus = TRUE;
		}
	}	
//	if( (g_pCamera->GetCamType() == CAMERA_TYPE_NORMAL && 
//		m_bAttackMode != 2 ) ||
//		m_bIsCameraPattern == TRUE ||
//		g_pD3dApp->m_dwGameState == _CITY)// 1ŔÎÄŞŔÎ °ćżě, ˝ĂÁî¸đµĺŔÎ °ćżě ·»´ő¸µ ľČÇÔ, µµ˝ĂżˇĽ­´Â ą«Á¶°Ç ·»´ő¸µ ÇÔ
	if( bCameraStatus
		|| m_bIsCameraPattern == TRUE 
		|| 	g_pD3dApp->m_dwGameState == _CITY)// 1ŔÎÄŞŔÎ °ćżě, ˝ĂÁî¸đµĺŔÎ °ćżě ·»´ő¸µ ľČÇÔ, µµ˝ĂżˇĽ­´Â ą«Á¶°Ç ·»´ő¸µ ÇÔ
	{
		((CUnitRender*)m_pRender)->Render(this, m_nArmorColorIndex);
		
		// ŔÎşĄĹä¸® ·»´ő¸µ
//		m_bInvenRender = FALSE;
//		if(g_pGameMain->m_pInfWindow->m_nLeftWindowInfo == 1 && m_bRenderInven == TRUE)
//		{
//			m_bInvenRender = TRUE;
//		}
	}

//	RenderMirror();
}


// 2008-08-22 by bhsohn EP3 ŔÎşĄĹä¸® Ăł¸®
void CShuttleChild::RenderMirror(POINT *pMirrorPos/*=NULL*/)
{	
	g_pGameMain->RenderMirror(pMirrorPos);		
}
//{
////	if(g_pGameMain->m_pInfWindow->m_nLeftWindowInfo == 1 && m_bRenderInven == TRUE)
//	{
//		m_bInvenRender = TRUE;
////		g_pCamera->SetCamType(CAMERA_TYPE_NORMAL);
//		
//		int nAmorNum = GetUnitNum();
//		CEffectInfo* pEffectEngine = NULL;
//		char chEngineObjectNum[20] = {0,};
//
//		if(g_pD3dApp->m_bCharacter)
//		{
//			if(g_pD3dApp->m_dwGameState != _SHOP)
//			{
//				CItemInfo* pItem = m_pStoreData->FindItemInInventoryByWindowPos( POS_CENTER );
//				nAmorNum = GetUnitNumFromCharacter((pItem?pItem->ItemInfo->SourceIndex:0), m_myShuttleInfo.UnitKind, 0, 0, 2);
//			}
//			else if(g_pD3dApp->m_dwGameState == _SHOP)
//			{
//				CItemInfo* pItem = m_pStoreData->FindItemInInventoryByWindowPos( POS_CENTER );
//				if(pItem && g_pGameMain->m_pItemInfoWear[POS_CENTER] == NULL)
//					nAmorNum = GetUnitNumFromCharacter((pItem?pItem->ItemInfo->SourceIndex:0), m_myShuttleInfo.UnitKind, 0, 0, 2);
//				else
//					nAmorNum = GetUnitNumFromCharacter((g_pGameMain->m_pItemInfoWear[POS_CENTER]?g_pGameMain->m_pItemInfoWear[POS_CENTER]->ItemInfo->SourceIndex:0), m_myShuttleInfo.UnitKind, 0, 0, 2);			
//			}
//		
//			GUIUnitRenderInfo pInfo;
//			memset(&pInfo, 0x00, sizeof(GUIUnitRenderInfo));
//			CAppEffectData	*pEffectTemp;
//			pEffectTemp = CUnitData::CreateWearItemEffectAtSelectMenu(&pInfo, nAmorNum, BODYCON_LANDED_MASK);
//
//			// ą«±â ¸ŢĆ®¸Ż˝ş
//			m_pMatInvenWeaponOrgPosition[0] = pEffectTemp->m_pCharacterInfo->GetEffectMatrix(BODYCON_WEAPON_POSITION_MASK, RC_EFF_PRIMARY_RIGHT_POSITION);
//			m_pMatInvenWeaponOrgPosition[1] = pEffectTemp->m_pCharacterInfo->GetEffectMatrix(BODYCON_WEAPON_POSITION_MASK, RC_EFF_PRIMARY_LEFT_POSITION);
//			m_pMatInvenWeaponOrgPosition[2] = pEffectTemp->m_pCharacterInfo->GetEffectMatrix(BODYCON_WEAPON_POSITION_MASK, RC_EFF_SECONDARY_RIGHT_POSITION);
//			m_pMatInvenWeaponOrgPosition[3] = pEffectTemp->m_pCharacterInfo->GetEffectMatrix(BODYCON_WEAPON_POSITION_MASK, RC_EFF_SECONDARY_LEFT_POSITION);
//
//			// żŁÁř żŔşęÁ§Ć®
//			CBodyConditionInfo* pBodyTemp = pEffectTemp->m_pCharacterInfo->FindBodyConditionInfo(BODYCON_LANDED_MASK);
//			if(pBodyTemp)
//			{
//				char chEffNum[20] = {0,};
//				wsprintf(chEffNum, "0%d.eff", GetInvenUseEngineEffectNum(m_myShuttleInfo.UnitKind));
//				pEffectEngine = pBodyTemp->GetEffectInfo(chEffNum);
//				if(pEffectEngine)
//				{
//					wsprintf(chEngineObjectNum, "0%d.obj", GetInvenUseEngineObjectNum(m_myShuttleInfo.UnitKind));
//				}
//			}
//
//			if(g_pD3dApp->m_pEffectList)
//			{
//				g_pD3dApp->m_pEffectList->DeleteChild((CAtumNode*)pEffectTemp);
//			}
//		}
//		else
//		{
//			// ą«±â ¸ŢĆ®¸Ż˝ş
//			m_pMatInvenWeaponOrgPosition[0] = m_pMatWeaponPosition[0];
//			m_pMatInvenWeaponOrgPosition[1] = m_pMatWeaponPosition[1];
//			m_pMatInvenWeaponOrgPosition[2] = m_pMatWeaponPosition[2];
//			m_pMatInvenWeaponOrgPosition[3] = m_pMatWeaponPosition[3];
//
//			// żŁÁř żŔşęÁ§Ć®
//			CBodyConditionInfo* pBodyTemp = m_pCharacterInfo->FindBodyConditionInfo(BODYCON_LANDED_MASK);
//			if(pBodyTemp)
//			{
//				char chEffNum[20] = {0,};
//				wsprintf(chEffNum, "0%d.eff", GetInvenUseEngineEffectNum(m_myShuttleInfo.UnitKind));
//				pEffectEngine = pBodyTemp->GetEffectInfo(chEffNum);
//				if(pEffectEngine)
//				{
//					wsprintf(chEngineObjectNum, "0%d.obj", GetInvenUseEngineObjectNum(m_myShuttleInfo.UnitKind));
//				}
//			}
//		}
//		
//		// ±âľî
//		CSkinnedMesh* pSkinnedMesh = ((CUnitRender*)m_pRender)->GetUnitMesh(nAmorNum);
//		if(pSkinnedMesh == NULL)
//			return;
//		// żŁÁř
//		CSkinnedMesh* pSkinnedMeshEngine = NULL;
//		if(pEffectEngine != NULL)
//			pSkinnedMeshEngine = g_pD3dApp->m_pEffectRender->LoadObject(chEngineObjectNum);
//		float fUnitScaling;
//		float fUnitPosX;
//		float fUnitPosY;
//		float fUnitPosZ;
//
//		GetResolutionPos(i_dwKind,
//			fUnitPosX,fUnitPosY,fUnitPosZ,fUnitScaling);
//		
//		fUnitScaling += m_fScaling;
//		fUnitPosX	 += m_fUnitX;
//		fUnitPosY	 += m_fUnitY;
//		fUnitPosZ	 += m_fUnitZ;
//
//		//////////////////////////////////////////////////////////////////////////
//		//
//		D3DXMATRIX pMatOldView, pMatOldProj, pMatPresView, pMatPresProj, pMatrix;
//		D3DXMatrixIdentity(&pMatOldView);
//		D3DXMatrixIdentity(&pMatOldProj);
//		D3DXMatrixIdentity(&pMatPresView);
//		D3DXMatrixIdentity(&pMatPresProj);
//		D3DXMatrixIdentity(&pMatrix);
//		
//		D3DXMATRIX pTemp, pMatRotX, pMatRotZ, pMatScaling;
//		D3DXMatrixIdentity(&pTemp);
//		D3DXMatrixIdentity(&pMatRotX);
//		D3DXMatrixIdentity(&pMatRotZ);
//		D3DXMatrixIdentity(&pMatScaling);
//		
//		// ÇöŔç şäżÍ ÇÁ·ÎÁ§ĽÇŔ» °ˇÁ®żÂ´Ů
//		g_pD3dDev->GetTransform( D3DTS_VIEW,       &pMatOldView );
//		g_pD3dDev->GetTransform( D3DTS_PROJECTION, &pMatOldProj );	
//		
//		D3DXMatrixScaling(&pMatScaling, fUnitScaling, fUnitScaling+0.002f, fUnitScaling);
//		D3DXMatrixTranslation(&pTemp, fUnitPosX, fUnitPosY, fUnitPosZ);
//		D3DXMatrixRotationX(&pMatRotX, m_fRotationX);
//		D3DXMatrixRotationY(&pMatRotZ, m_fRotationZ);
//		pMatrix = pMatRotZ*pMatRotX*pTemp*pMatScaling;
//
//		// 2006-01-16 by ispark, ą«±â
//		m_pMatInvenWeaponSetPosition[0] = m_pMatInvenWeaponOrgPosition[0]*pMatrix;
//		m_pMatInvenWeaponSetPosition[1] = m_pMatInvenWeaponOrgPosition[1]*pMatrix;
//		m_pMatInvenWeaponSetPosition[2] = m_pMatInvenWeaponOrgPosition[2]*pMatrix;
//		m_pMatInvenWeaponSetPosition[3] = m_pMatInvenWeaponOrgPosition[3]*pMatrix;
//		
//		g_pD3dDev->SetTransform( D3DTS_VIEW,		&pMatPresView);
//		g_pD3dDev->SetTransform( D3DTS_PROJECTION,	&pMatPresProj);	
//		
//		// 2008-01-21 by bhsohn Ă˘°íżˇĽ­ ľĆ¸Ó»öŔĚ ľČąŮ˛î´Â ąö±× ĽöÁ¤
//		GUI_BUILDINGNPC* pCurrentBuildingNpc = g_pInterface->m_pCityBase->GetCurrentBuildingNPC();
//
//		// ±âĂĽ
//		// 2006-02-17 by ispark
//		if(g_pD3dApp->m_dwGameState != _SHOP 
//			|| (pCurrentBuildingNpc && IS_STORE_SHOP_TYPE(pCurrentBuildingNpc->buildingInfo.BuildingKind)))// 2008-01-21 by bhsohn Ă˘°íżˇĽ­ ľĆ¸Ó»öŔĚ ľČąŮ˛î´Â ąö±× ĽöÁ¤
//		{
//			m_nInvenArmorColorIndex = m_nArmorColorIndex;
//		}
//		pSkinnedMesh->m_unTexSelectColor = m_nInvenArmorColorIndex;
//		pSkinnedMesh->SetWorldMatrix(pMatrix);
//		pSkinnedMesh->AnotherTexture(1);
//		pSkinnedMesh->Render(FALSE, _SHUTTLE);
////      pSkinnedMesh->SetDecalCount(0);
//		m_pMatInven = pMatrix;
//
//		// żŁÁř
//		if(pSkinnedMeshEngine != NULL)
//		{
//			pSkinnedMeshEngine->SetWorldMatrix(pMatrix);
//			pSkinnedMeshEngine->AnotherTexture(1);
//			pSkinnedMeshEngine->Render();
//		}
//
//		// 2007-11-09 by dgwoo ą«±â ¸Ţ˝¬¸¦ ąčż­żˇĽ­ ş¤ĹÍ·Î şŻ°ć.
//		for(int i = 0; i < m_vectInvenWeaponMesh.size(); i++)
//		{
//			if(m_vectInvenWeaponMesh[i].pInvenWeaponMesh)
//			{		
//				DWORD dwSrc,dwDest,dwColorOp;
//				DWORD dwFogValue = FALSE;	
//				DWORD dwLightColorOp = 0;					
//				CEffectInfo* pEffectInfo = GetEffectWeapon(i);
//				
//				if(pEffectInfo 	&& (EFFECT_TYPE_OBJECT == pEffectInfo->m_nEffectType))// żŔşęÁ§Ć®¸¸ ·Ł´ő
//				{
//					CObjectAni* pEffect = (CObjectAni*)pEffectInfo->m_pEffect;
//					
//					//06800370								
//					{					
//						g_pD3dDev->GetRenderState(D3DRS_SRCBLEND,&dwSrc);
//						g_pD3dDev->GetRenderState(D3DRS_DESTBLEND,&dwDest);
//						g_pD3dDev->GetTextureStageState(0,D3DTSS_COLOROP,&dwColorOp);
//						
//						g_pD3dDev->SetRenderState( D3DRS_ALPHATESTENABLE,   pEffect->m_bAlphaTestEnble);//pEffect->m_bAlphaTestEnble
//						g_pD3dDev->SetRenderState( D3DRS_LIGHTING, TRUE );
//						
//						g_pD3dDev->SetRenderState( D3DRS_ZENABLE, pEffect->m_bZbufferEnable);
//						if( pEffect->m_bZWriteEnable == FALSE)
//						{
//							g_pD3dDev->SetRenderState( D3DRS_ZWRITEENABLE, pEffect->m_bZWriteEnable );
//						}
//						g_pD3dDev->SetRenderState( D3DRS_ALPHABLENDENABLE,  pEffect->m_bAlphaBlending );
//						if(pEffect->m_bAlphaBlending)
//						{
//							g_pD3dDev->GetRenderState( D3DRS_FOGENABLE,  &dwFogValue );
//							
//							g_pD3dDev->SetRenderState(D3DRS_SRCBLEND,pEffect->m_nSrcBlend);
//							g_pD3dDev->SetRenderState(D3DRS_DESTBLEND,pEffect->m_nDestBlend);
//						}
//						g_pD3dDev->SetTextureStageState(0,D3DTSS_COLOROP,pEffect->m_nTextureRenderState);
//						g_pD3dDev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );					
//						
//						if(pEffect->m_bLightMapUse)
//						{
//							g_pD3dDev->GetTextureStageState( 1, D3DTSS_COLOROP, &dwLightColorOp );
//							g_pD3dDev->SetTextureStageState( 1, D3DTSS_COLOROP, pEffect->m_nLightMapRenderState );
//							g_pD3dDev->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 0 );
//							
//							g_pD3dDev->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
//							g_pD3dDev->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );
//						}
//						
//					}					
//				}			
//				m_vectInvenWeaponMesh[i].pInvenWeaponMesh->SetWorldMatrix(m_pMatInvenWeaponSetPosition[m_vectInvenWeaponMesh[i].nMatIndex]);
//				m_vectInvenWeaponMesh[i].pInvenWeaponMesh->Render();
//				if(pEffectInfo 	&& (EFFECT_TYPE_OBJECT == pEffectInfo->m_nEffectType))// żŔşęÁ§Ć®¸¸ ·Ł´ő
//				{
//					g_pD3dDev->SetRenderState(D3DRS_SRCBLEND,dwSrc);
//					g_pD3dDev->SetRenderState(D3DRS_DESTBLEND,dwDest);
//					g_pD3dDev->SetTextureStageState(0,D3DTSS_COLOROP,dwColorOp);
//
//					CObjectAni* pEffect = (CObjectAni*)pEffectInfo->m_pEffect;
//					
//					//g_pD3dDev->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
//					if(!pEffect->m_bUseEnvironmentLight)
//					{
//						g_pD3dDev->LightEnable( 2, FALSE );
//					}
//					if(pEffect->m_bLightMapUse)
//					{
//						g_pD3dDev->SetTextureStageState( 1, D3DTSS_COLOROP, dwLightColorOp );
//					}
//					if(pEffect->m_nSrcBlend == D3DBLEND_ONE && pEffect->m_nDestBlend == D3DBLEND_ONE && dwFogValue == TRUE)
//					{
//						g_pD3dDev->SetRenderState( D3DRS_FOGENABLE,  dwFogValue );
//					}
//					if(pEffect->m_bAlphaTestEnble)
//					{
//						g_pD3dDev->SetRenderState( D3DRS_ALPHATESTENABLE,  FALSE );
//					}
//					if( pEffect->m_bZWriteEnable == FALSE)
//					{
//						g_pD3dDev->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
//					}
//				}				
//			}
//		  
//			// 2007-11-08 by bhsohn ŔÎşĄ ŔĚĆĺĆ® °ü·Ă Ăł¸®
//			// ĆÄĆĽĹ¬ ·Ł´ő			
//			//if(m_stInvenWeaponMesh[i].pInvenParticle)
//			{				
//				//g_pD3dApp->m_pEffectRender->ParticleInvenRender();
//				
//				g_pD3dApp->m_pEffectRender->RenderParticleInvenVector(m_vectInvenWeaponMesh[i].nMatIndex, 
//						pMatrix, 
//						m_pMatInvenWeaponSetPosition[m_vectInvenWeaponMesh[i].nMatIndex], 
//						fUnitScaling);
//				
//			}			
//		}
//		// ·Ł´ő¸µ ąöĆŰ ĂĘ±âČ­
//		g_pD3dApp->m_pEffectRender->ResetContentInvneParticle();
//
//		// şą±¸
//		g_pD3dDev->SetTransform( D3DTS_VIEW,		&pMatOldView );
//		g_pD3dDev->SetTransform( D3DTS_PROJECTION,	&pMatOldProj );
//		g_pD3dDev->SetRenderState( D3DRS_LIGHTING, TRUE );
//	}
//}
// end 2008-08-22 by bhsohn EP3 ŔÎşĄĹä¸® Ăł¸®

#ifdef  _OLD_SHADOW_SYSTEM
void CShuttleChild::CheckShadowPos()
{
	FLOG( "CShuttleChild::CheckShadowPos()" );
	D3DXVECTOR3 vTempShadowPos[4];
	if( IsTileMapRenderEnable(g_pShuttleChild->m_myShuttleInfo.MapChannelIndex.MapIndex) == FALSE &&
		m_bOnObject == FALSE)
	{
		m_bShadowIsRender = FALSE;
		return;
	}
	else
	{
		m_bShadowIsRender = TRUE;
	}
//	if( g_pSOption->sShadowState == 0 )
//	{
//		return;
//	}
	D3DXVECTOR3 vVel, vSideVel;
	D3DXMATRIX matTemp;
	if(IS_DT(m_myShuttleInfo.UnitKind) && !m_bIsAir)
	{
		vVel = m_vVel;
	}
	else
	{
		D3DXMatrixRotationAxis( &matTemp, &m_vSideVel, m_fAxisRate);
		D3DXVec3TransformCoord( &vVel, &m_vVel, &matTemp );
	}
	if(m_vUp.y < 0)
	{
		vSideVel = -m_vSideVel;
	}
	else
	{
		vSideVel = m_vSideVel;
	}
	m_vShadowPos1 = m_vPos+m_fObjectSize*(vVel-vSideVel);
	m_vShadowPos2 = m_vPos+m_fObjectSize*(vVel+vSideVel);
	m_vShadowPos3 = m_vPos-m_fObjectSize*(vVel-vSideVel);
	m_vShadowPos4 = m_vPos-m_fObjectSize*(vVel+vSideVel);

	FLOAT fDist;
	int i,j;
	i = (int)(m_vShadowPos1.x/TILE_SIZE);
	j = (int)(m_vShadowPos1.z/TILE_SIZE);
	D3DXVECTOR3 v,orig,dir,vPos;//,v2;
	D3DXVECTOR3 vSide;
	orig = m_vShadowPos1;
	dir = D3DXVECTOR3(0,-1,0);
	vSide = D3DXVECTOR3(0,0,1);
	if(i >= 0 && i < g_pGround->m_projectInfo.sXSize && j >= 0 && j < g_pGround->m_projectInfo.sYSize)
	{
		if(g_pGround->IntersectTriangle(orig,dir,g_pGround->m_pTileVertexArray[i*(g_pGround->m_projectInfo.sYSize+1)+j].pos,
			g_pGround->m_pTileVertexArray[i*(g_pGround->m_projectInfo.sYSize+1)+(j+1)].pos,
			g_pGround->m_pTileVertexArray[(i+1)*(g_pGround->m_projectInfo.sYSize+1)+(j)].pos,
			&fDist))//, &fBary1, &fBary2 ))
		{
			m_vShadowPos1 = orig + dir*fDist;
			if(CheckIsWater(orig))
			{
				if(m_vShadowPos1.y < g_pGround->m_projectInfo.fWaterHeight)
					m_vShadowPos1.y = g_pGround->m_projectInfo.fWaterHeight;
			}
			vTempShadowPos[0] = m_vShadowPos1;
			float fTempHeight = DEFAULT_COLLISION_DISTANCE;// = 100.0f;
			if(m_bOnObject)
			{
				D3DXMatrixLookAtLH(&matTemp,&orig,&(orig + dir),&vSide);
				fTempHeight = g_pScene->m_pObjectRender->CheckCollMesh(matTemp,orig, FALSE).fDist;
				if(m_vShadowPos1.y < orig.y - fTempHeight)
					m_vShadowPos1.y = orig.y - fTempHeight;
				else if(IsTileMapRenderEnable(g_pShuttleChild->m_myShuttleInfo.MapChannelIndex.MapIndex) == FALSE)
					m_bShadowIsRender = FALSE;
			}
		}
		else if(g_pGround->IntersectTriangle(orig,dir,g_pGround->m_pTileVertexArray[i*(g_pGround->m_projectInfo.sYSize+1)+j+1].pos,
			g_pGround->m_pTileVertexArray[(i+1)*(g_pGround->m_projectInfo.sYSize+1)+(j)].pos,
			g_pGround->m_pTileVertexArray[(i+1)*(g_pGround->m_projectInfo.sYSize+1)+j+1].pos,
			&fDist))//, &fBary1, &fBary2 ))
		{
			m_vShadowPos1 = orig + dir*fDist;
			if(CheckIsWater(orig))
			{
				if(m_vShadowPos1.y < g_pGround->m_projectInfo.fWaterHeight)
					m_vShadowPos1.y = g_pGround->m_projectInfo.fWaterHeight;
			}
			vTempShadowPos[0] = m_vShadowPos1;
			float fTempHeight = DEFAULT_COLLISION_DISTANCE;// = 100.0f;
			if(m_bOnObject && 
				g_pScene &&
				g_pScene->m_pObjectRender)
			{
				D3DXMatrixLookAtLH(&matTemp,&orig,&(orig + dir),&vSide);
				fTempHeight = g_pScene->m_pObjectRender->CheckCollMesh(matTemp,orig, FALSE).fDist;
				if(m_vShadowPos1.y < orig.y - fTempHeight)
					m_vShadowPos1.y = orig.y - fTempHeight;
				else if(IsTileMapRenderEnable(g_pShuttleChild->m_myShuttleInfo.MapChannelIndex.MapIndex) == FALSE)
					m_bShadowIsRender = FALSE;
			}
		}
	}
	i = (int)(m_vShadowPos2.x/TILE_SIZE);
	j = (int)(m_vShadowPos2.z/TILE_SIZE);
	orig = m_vShadowPos2;
	if(i >= 0 && i < g_pGround->m_projectInfo.sXSize && j >= 0 && j < g_pGround->m_projectInfo.sYSize)
	{
		vPos = m_vShadowPos2;
		if(g_pGround->IntersectTriangle(orig,dir,g_pGround->m_pTileVertexArray[i*(g_pGround->m_projectInfo.sYSize+1)+j].pos,
			g_pGround->m_pTileVertexArray[i*(g_pGround->m_projectInfo.sYSize+1)+(j+1)].pos,
			g_pGround->m_pTileVertexArray[(i+1)*(g_pGround->m_projectInfo.sYSize+1)+(j)].pos,
			&fDist))//, &fBary1, &fBary2 ))
		{
			m_vShadowPos2 = orig + dir*fDist;
			if(CheckIsWater(orig))
			{
				if(m_vShadowPos2.y < g_pGround->m_projectInfo.fWaterHeight)
					m_vShadowPos2.y = g_pGround->m_projectInfo.fWaterHeight;
			}
			vTempShadowPos[1] = m_vShadowPos2;
			float fTempHeight = DEFAULT_COLLISION_DISTANCE;// = 100.0f;
			if(m_bOnObject && g_pScene->m_pObjectRender)
			{
				D3DXMatrixLookAtLH(&matTemp,&orig,&(orig + dir),&vSide);
				fTempHeight = g_pScene->m_pObjectRender->CheckCollMesh(matTemp,orig, FALSE).fDist;
				if(m_vShadowPos2.y < orig.y - fTempHeight)
					m_vShadowPos2.y = orig.y - fTempHeight;
				else if(IsTileMapRenderEnable(g_pShuttleChild->m_myShuttleInfo.MapChannelIndex.MapIndex) == FALSE)
					m_bShadowIsRender = FALSE;
			}
		}
		else if(g_pGround->IntersectTriangle(orig,dir,g_pGround->m_pTileVertexArray[i*(g_pGround->m_projectInfo.sYSize+1)+j+1].pos,
			g_pGround->m_pTileVertexArray[(i+1)*(g_pGround->m_projectInfo.sYSize+1)+(j)].pos,
			g_pGround->m_pTileVertexArray[(i+1)*(g_pGround->m_projectInfo.sYSize+1)+j+1].pos,
			&fDist))//, &fBary1, &fBary2 ))
		{
			m_vShadowPos2 = orig + dir*fDist;
			if(CheckIsWater(orig))
			{
				if(m_vShadowPos2.y < g_pGround->m_projectInfo.fWaterHeight)
					m_vShadowPos2.y = g_pGround->m_projectInfo.fWaterHeight;
			}
			vTempShadowPos[1] = m_vShadowPos2;
			float fTempHeight = DEFAULT_COLLISION_DISTANCE;// = 100.0f;
			if(m_bOnObject && g_pScene->m_pObjectRender)
			{
				D3DXMatrixLookAtLH(&matTemp,&orig,&(orig + dir),&vSide);
				fTempHeight = g_pScene->m_pObjectRender->CheckCollMesh(matTemp,orig, FALSE).fDist;
				if(m_vShadowPos2.y < orig.y - fTempHeight)
					m_vShadowPos2.y = orig.y - fTempHeight;
				else if(IsTileMapRenderEnable(g_pShuttleChild->m_myShuttleInfo.MapChannelIndex.MapIndex) == FALSE)
					m_bShadowIsRender = FALSE;
			}
		}
	}
	i = (int)(m_vShadowPos3.x/TILE_SIZE);
	j = (int)(m_vShadowPos3.z/TILE_SIZE);
	orig = m_vShadowPos3;
	if(i >= 0 && i < g_pGround->m_projectInfo.sXSize && j >= 0 && j < g_pGround->m_projectInfo.sYSize)
	{
		vPos = m_vShadowPos3;
		if(g_pGround->IntersectTriangle(orig,dir,g_pGround->m_pTileVertexArray[i*(g_pGround->m_projectInfo.sYSize+1)+j].pos,
			g_pGround->m_pTileVertexArray[i*(g_pGround->m_projectInfo.sYSize+1)+(j+1)].pos,
			g_pGround->m_pTileVertexArray[(i+1)*(g_pGround->m_projectInfo.sYSize+1)+(j)].pos,
			&fDist))//, &fBary1, &fBary2 ))
		{
			m_vShadowPos3 = orig + dir*fDist;
			if(CheckIsWater(orig))
			{
				if(m_vShadowPos3.y < g_pGround->m_projectInfo.fWaterHeight)
					m_vShadowPos3.y = g_pGround->m_projectInfo.fWaterHeight;
			}
			vTempShadowPos[2] = m_vShadowPos3;
			float fTempHeight = DEFAULT_COLLISION_DISTANCE;// = 100.0f;
			if(m_bOnObject && g_pScene->m_pObjectRender)
			{
				D3DXMatrixLookAtLH(&matTemp,&orig,&(orig + dir),&vSide);
				fTempHeight = g_pScene->m_pObjectRender->CheckCollMesh(matTemp,orig, FALSE).fDist;
				if(m_vShadowPos3.y < orig.y - fTempHeight)
					m_vShadowPos3.y = orig.y - fTempHeight;
				else if(IsTileMapRenderEnable(g_pShuttleChild->m_myShuttleInfo.MapChannelIndex.MapIndex) == FALSE)
					m_bShadowIsRender = FALSE;
			}
		}
		else if(g_pGround->IntersectTriangle(orig,dir,g_pGround->m_pTileVertexArray[i*(g_pGround->m_projectInfo.sYSize+1)+j+1].pos,
			g_pGround->m_pTileVertexArray[(i+1)*(g_pGround->m_projectInfo.sYSize+1)+(j)].pos,
			g_pGround->m_pTileVertexArray[(i+1)*(g_pGround->m_projectInfo.sYSize+1)+j+1].pos,
			&fDist))//, &fBary1, &fBary2 ))
		{
			m_vShadowPos3 = orig + dir*fDist;
			if(CheckIsWater(orig))
			{
				if(m_vShadowPos3.y < g_pGround->m_projectInfo.fWaterHeight)
					m_vShadowPos3.y = g_pGround->m_projectInfo.fWaterHeight;
			}
			vTempShadowPos[2] = m_vShadowPos3;
			float fTempHeight = DEFAULT_COLLISION_DISTANCE;// = 100.0f;
			if(m_bOnObject && g_pScene->m_pObjectRender)
			{
				D3DXMatrixLookAtLH(&matTemp,&orig,&(orig + dir),&vSide);
				fTempHeight = g_pScene->m_pObjectRender->CheckCollMesh(matTemp,orig, FALSE).fDist;
				if(m_vShadowPos3.y < orig.y - fTempHeight)
					m_vShadowPos3.y = orig.y - fTempHeight;
				else if(IsTileMapRenderEnable(g_pShuttleChild->m_myShuttleInfo.MapChannelIndex.MapIndex) == FALSE)
					m_bShadowIsRender = FALSE;
			}
		}
	}
	i = (int)(m_vShadowPos4.x/TILE_SIZE);
	j = (int)(m_vShadowPos4.z/TILE_SIZE);
	orig = m_vShadowPos4;
	if(i >= 0 && i < g_pGround->m_projectInfo.sXSize && j >= 0 && j < g_pGround->m_projectInfo.sYSize)
	{
		vPos = m_vShadowPos4;
		if(g_pGround->IntersectTriangle(orig,dir,g_pGround->m_pTileVertexArray[i*(g_pGround->m_projectInfo.sYSize+1)+j].pos,
			g_pGround->m_pTileVertexArray[i*(g_pGround->m_projectInfo.sYSize+1)+(j+1)].pos,
			g_pGround->m_pTileVertexArray[(i+1)*(g_pGround->m_projectInfo.sYSize+1)+(j)].pos,
			&fDist))//, &fBary1, &fBary2 ))
		{
			m_vShadowPos4 = orig + dir*fDist;
			if(CheckIsWater(orig))
			{
				if(m_vShadowPos4.y < g_pGround->m_projectInfo.fWaterHeight)
					m_vShadowPos4.y = g_pGround->m_projectInfo.fWaterHeight;
			}
			vTempShadowPos[3] = m_vShadowPos4;
			float fTempHeight = DEFAULT_COLLISION_DISTANCE;// = 100.0f;
			if(m_bOnObject && g_pScene->m_pObjectRender)
			{
				D3DXMatrixLookAtLH(&matTemp,&orig,&(orig + dir),&vSide);
				fTempHeight = g_pScene->m_pObjectRender->CheckCollMesh(matTemp,orig, FALSE).fDist;
				if(m_vShadowPos4.y < orig.y - fTempHeight)
					m_vShadowPos4.y = orig.y - fTempHeight;
				else if(IsTileMapRenderEnable(g_pShuttleChild->m_myShuttleInfo.MapChannelIndex.MapIndex) == FALSE)
					m_bShadowIsRender = FALSE;
			}
		}
		else if(g_pGround->IntersectTriangle(orig,dir,g_pGround->m_pTileVertexArray[i*(g_pGround->m_projectInfo.sYSize+1)+j+1].pos,
			g_pGround->m_pTileVertexArray[(i+1)*(g_pGround->m_projectInfo.sYSize+1)+(j)].pos,
			g_pGround->m_pTileVertexArray[(i+1)*(g_pGround->m_projectInfo.sYSize+1)+j+1].pos,
			&fDist))//, &fBary1, &fBary2 ))
		{
			m_vShadowPos4 = orig + dir*fDist;
			if(CheckIsWater(orig))
			{
				if(m_vShadowPos4.y < g_pGround->m_projectInfo.fWaterHeight)
					m_vShadowPos4.y = g_pGround->m_projectInfo.fWaterHeight;
			}
			vTempShadowPos[3] = m_vShadowPos4;
			float fTempHeight = DEFAULT_COLLISION_DISTANCE;// = 100.0f;
			if(m_bOnObject && g_pScene->m_pObjectRender)
			{
				D3DXMatrixLookAtLH(&matTemp,&orig,&(orig + dir),&vSide);
				fTempHeight = g_pScene->m_pObjectRender->CheckCollMesh(matTemp,orig, FALSE).fDist;
				if(m_vShadowPos4.y < orig.y - fTempHeight)
					m_vShadowPos4.y = orig.y - fTempHeight;
				else if(IsTileMapRenderEnable(g_pShuttleChild->m_myShuttleInfo.MapChannelIndex.MapIndex) == FALSE)
					m_bShadowIsRender = FALSE;
			}
		}
	}
	if(m_bOnObject && g_pScene->m_pObjectRender &&
		(D3DXVec3Length(&(m_vShadowPos4-m_vShadowPos2)) > m_fObjectSize*6 ||
		D3DXVec3Length(&(m_vShadowPos1-m_vShadowPos3)) > m_fObjectSize*6))
	{// ±×¸˛ŔÚ°ˇ żŔÇÁÁ§Ć®żˇ °ÉĂÄ ´ĂľîÁö¸é ÁöÇü°ú ą°Ŕ» ±âÁŘŔ¸·Î ±×¸˛ŔÚ Ŕ§Äˇ¸¦ ĽĽĆĂÇŃ´Ů.
		m_vShadowPos1 = vTempShadowPos[0];
		m_vShadowPos2 = vTempShadowPos[1];
		m_vShadowPos3 = vTempShadowPos[2];
		m_vShadowPos4 = vTempShadowPos[3];
	}
	SPRITEVERTEX* pV;
	m_pVBShadow->Lock( 0, 0, (void**)&pV,	0 );
	pV[0].p = m_vShadowPos4; 
	pV[1].p = m_vShadowPos1; 
	pV[2].p = m_vShadowPos3; 
	pV[3].p = m_vShadowPos2;
	m_pVBShadow->Unlock();
}
#endif

FLOAT CShuttleChild::CheckMap(D3DXVECTOR3 vPos)
{
	FLOG( "CShuttleChild::CheckMap(D3DXVECTOR3 vPos)" );
	// 2007-04-19 by bhsohn Ăćµą Ăł¸® ą®Á¦
	//if(IsTileMapRenderEnable(g_pShuttleChild->m_myShuttleInfo.MapChannelIndex.MapIndex) == FALSE)
	if(IsTileMapTickEnable(g_pShuttleChild->m_myShuttleInfo.MapChannelIndex.MapIndex) == FALSE)
	{
		if(vPos.y <= g_pGround->m_projectInfo.fWaterHeight + 1.0f)
		{
			m_bCollMap = TRUE;
		}
		return g_pGround->m_projectInfo.fWaterHeight;
	}
	FLOAT fDist,fMaxPoint;
	int i,j;
	i = (int)(vPos.x/TILE_SIZE);
	j = (int)(vPos.z/TILE_SIZE);
	D3DXVECTOR3 v,dir;
	dir = D3DXVECTOR3(0,-1,0);
	m_bIsWater = CheckIsWater(vPos);
	// 2005-09-14 by ispark
	// ¸¸ľŕżˇ żÜ°űŔĚ¶ó¸é ľČÂĘ ÁöÇüŔ¸·Î °Ë»ç
	if((i < 0 || i > g_pGround->m_projectInfo.sXSize) || (j < 0 || j > g_pGround->m_projectInfo.sYSize))
	{
		if(i < 0)
		{
			vPos.x = 0;
			i = 0;
		}
		else if(i > g_pGround->m_projectInfo.sXSize)
		{
			i = g_pGround->m_projectInfo.sXSize - 1;
			vPos.x = i * TILE_SIZE;
		}
		if(j < 0)
		{
			vPos.z = 0;
			j = 0;
		}
		else if(j > g_pGround->m_projectInfo.sYSize)
		{
			j = g_pGround->m_projectInfo.sYSize - 1;
			vPos.z = j * TILE_SIZE;
		}
	}
	if(i >= 0 && i < g_pGround->m_projectInfo.sXSize && j >= 0 && j < g_pGround->m_projectInfo.sYSize)
	{
		if(g_pGround->IntersectTriangle(vPos,dir,g_pGround->m_pTileVertexArray[i*(g_pGround->m_projectInfo.sYSize+1)+j].pos,
			g_pGround->m_pTileVertexArray[i*(g_pGround->m_projectInfo.sYSize+1)+(j+1)].pos,
			g_pGround->m_pTileVertexArray[(i+1)*(g_pGround->m_projectInfo.sYSize+1)+(j)].pos,
			&fDist))//, &fBary1, &fBary2 ))
		{
			// ĽĽÁˇÁß ĂÖ°í łôŔşÁˇŔ» ŔúŔĺÇŃ´Ů.
			fMaxPoint = g_pGround->m_pTileVertexArray[i*(g_pGround->m_projectInfo.sYSize+1)+j].pos.y;
			if(fMaxPoint < g_pGround->m_pTileVertexArray[i*(g_pGround->m_projectInfo.sYSize+1)+(j+1)].pos.y)
				fMaxPoint = g_pGround->m_pTileVertexArray[i*(g_pGround->m_projectInfo.sYSize+1)+(j+1)].pos.y;
			if(fMaxPoint < g_pGround->m_pTileVertexArray[(i+1)*(g_pGround->m_projectInfo.sYSize+1)+(j)].pos.y)
				fMaxPoint = g_pGround->m_pTileVertexArray[(i+1)*(g_pGround->m_projectInfo.sYSize+1)+(j)].pos.y;
			// ÁöÇüŔÇ łë¸Öş¤ĹÍ¸¦ ±¸ÇŃ´Ů.
			D3DXVec3Cross(&m_vNormal,&(g_pGround->m_pTileVertexArray[(i)*(g_pGround->m_projectInfo.sYSize+1)+(j+1)].pos
				-g_pGround->m_pTileVertexArray[(i)*(g_pGround->m_projectInfo.sYSize+1)+j].pos),
				&(g_pGround->m_pTileVertexArray[(i+1)*(g_pGround->m_projectInfo.sYSize+1)+(j)].pos
				-g_pGround->m_pTileVertexArray[(i)*(g_pGround->m_projectInfo.sYSize+1)+(j+1)].pos));
			D3DXVec3Normalize(&m_vNormal,&m_vNormal);
			v = vPos + dir*fDist;
			SetMoveType(m_vNormal,fMaxPoint);
			if(m_bIsWater)
			{
				if(v.y > g_pGround->m_projectInfo.fWaterHeight)
				{
					if(vPos.y <= v.y + 1.0f)
						m_bCollMap = TRUE;
					return v.y;
				}
				else
				{
					m_bCheckMove = 3;
					if(vPos.y <= g_pGround->m_projectInfo.fWaterHeight + 1.0f)
					{
						m_bCollMap = TRUE;
					}
					return g_pGround->m_projectInfo.fWaterHeight;
				}
			}
			else
			{
				if(vPos.y <= v.y + 1.0f)
					m_bCollMap = TRUE;
				return v.y;
			}
		}
		else if(g_pGround->IntersectTriangle(vPos,dir,g_pGround->m_pTileVertexArray[i*(g_pGround->m_projectInfo.sYSize+1)+j+1].pos,
			g_pGround->m_pTileVertexArray[(i+1)*(g_pGround->m_projectInfo.sYSize+1)+(j)].pos,
			g_pGround->m_pTileVertexArray[(i+1)*(g_pGround->m_projectInfo.sYSize+1)+j+1].pos,
			&fDist))//, &fBary1, &fBary2 ))
		{
			// ĽĽÁˇÁß ĂÖ°í łôŔşÁˇŔ» ŔúŔĺÇŃ´Ů.
			fMaxPoint = g_pGround->m_pTileVertexArray[i*(g_pGround->m_projectInfo.sYSize+1)+j+1].pos.y;
			if(fMaxPoint < g_pGround->m_pTileVertexArray[(i+1)*(g_pGround->m_projectInfo.sYSize+1)+(j)].pos.y)
				fMaxPoint = g_pGround->m_pTileVertexArray[(i+1)*(g_pGround->m_projectInfo.sYSize+1)+(j)].pos.y;
			if(fMaxPoint < g_pGround->m_pTileVertexArray[(i+1)*(g_pGround->m_projectInfo.sYSize+1)+(j+1)].pos.y)
				fMaxPoint = g_pGround->m_pTileVertexArray[(i+1)*(g_pGround->m_projectInfo.sYSize+1)+(j+1)].pos.y;
			// ÁöÇüŔÇ łë¸Öş¤ĹÍ¸¦ ±¸ÇŃ´Ů.
			D3DXVec3Cross(&m_vNormal,&(g_pGround->m_pTileVertexArray[(i+1)*(g_pGround->m_projectInfo.sYSize+1)+(j+1)].pos
				-g_pGround->m_pTileVertexArray[(i)*(g_pGround->m_projectInfo.sYSize+1)+j+1].pos),
				&(g_pGround->m_pTileVertexArray[(i+1)*(g_pGround->m_projectInfo.sYSize+1)+(j)].pos
				-g_pGround->m_pTileVertexArray[(i+1)*(g_pGround->m_projectInfo.sYSize+1)+(j+1)].pos));
			D3DXVec3Normalize(&m_vNormal,&m_vNormal);
			v = vPos + dir*fDist;
			SetMoveType(m_vNormal,fMaxPoint);
			if(m_bIsWater)
			{
				if(v.y > g_pGround->m_projectInfo.fWaterHeight)
				{
					if(vPos.y <= v.y + 1.0f)
						m_bCollMap = TRUE;
					return v.y;
				}
				else
				{
					m_bCheckMove = 3;
					if(vPos.y <= g_pGround->m_projectInfo.fWaterHeight + 1.0f)
					{
						m_bCollMap = TRUE;
					}
					return g_pGround->m_projectInfo.fWaterHeight;
				}
			}
			else
			{
				if(vPos.y <= v.y + 1.0f)
					m_bCollMap = TRUE;
				return v.y;
			}
		}
	}
	return 0.0f;
}

FLOAT CShuttleChild::CheckMapIsColl(D3DXVECTOR3 vPos, D3DXVECTOR3 vVel,int cont)
{
	FLOG( "CShuttleChild::CheckMapIsColl(D3DXVECTOR3 vPos, D3DXVECTOR3 vVel,int cont)" );
	if(IsTileMapRenderEnable(g_pShuttleChild->m_myShuttleInfo.MapChannelIndex.MapIndex) == FALSE)
	{
		return vPos.y - g_pGround->m_projectInfo.fWaterHeight;
	}
	FLOAT fDist;
	D3DXVECTOR3 vTempPos;
	int i,j;
	for(int k=0;k<cont;k++)
	{
		vTempPos = vPos + TILE_SIZE*vVel*k;
		i = (int)(vTempPos.x/TILE_SIZE);
		j = (int)(vTempPos.z/TILE_SIZE);
		if(i >= 0 && i < g_pGround->m_projectInfo.sXSize && j >= 0 && j < g_pGround->m_projectInfo.sYSize)
		{
			if(g_pGround->IntersectTriangle(vPos,vVel,g_pGround->m_pTileVertexArray[i*(g_pGround->m_projectInfo.sYSize+1)+j].pos,
				g_pGround->m_pTileVertexArray[i*(g_pGround->m_projectInfo.sYSize+1)+(j+1)].pos,
				g_pGround->m_pTileVertexArray[(i+1)*(g_pGround->m_projectInfo.sYSize+1)+(j)].pos,
				&fDist))//, &fBary1, &fBary2 ))
			{
				return fDist;
			}
			if(g_pGround->IntersectTriangle(vPos,vVel,g_pGround->m_pTileVertexArray[i*(g_pGround->m_projectInfo.sYSize+1)+j+1].pos,
				g_pGround->m_pTileVertexArray[(i+1)*(g_pGround->m_projectInfo.sYSize+1)+(j)].pos,
				g_pGround->m_pTileVertexArray[(i+1)*(g_pGround->m_projectInfo.sYSize+1)+j+1].pos,
				&fDist))//, &fBary1, &fBary2 ))
			{
				return fDist;
			}
		}
	}
	return 0.0f;
}

BOOL CShuttleChild::CheckMapIsColl(D3DXVECTOR3 vPos, D3DXVECTOR3 vVel)
{
	FLOG( "CShuttleChild::CheckMapIsColl(D3DXVECTOR3 vPos, D3DXVECTOR3 vVel)" );
	if(IsTileMapRenderEnable(g_pShuttleChild->m_myShuttleInfo.MapChannelIndex.MapIndex) == FALSE)
	{
		return TRUE;
	}
	FLOAT fDist,fMaxPoint;
	int i,j;
	i = (int)(vPos.x/TILE_SIZE);
	j = (int)(vPos.z/TILE_SIZE);
	if(i >= 0 && i < g_pGround->m_projectInfo.sXSize && j >= 0 && j < g_pGround->m_projectInfo.sYSize)
	{
		if(g_pGround->IntersectTriangle(vPos,vVel,g_pGround->m_pTileVertexArray[i*(g_pGround->m_projectInfo.sYSize+1)+j].pos,
			g_pGround->m_pTileVertexArray[i*(g_pGround->m_projectInfo.sYSize+1)+(j+1)].pos,
			g_pGround->m_pTileVertexArray[(i+1)*(g_pGround->m_projectInfo.sYSize+1)+(j)].pos,
			&fDist))//, &fBary1, &fBary2 ))
		{
			// ĽĽÁˇÁß ĂÖ°í łôŔşÁˇŔ» ŔúŔĺÇŃ´Ů.
			fMaxPoint = g_pGround->m_pTileVertexArray[i*(g_pGround->m_projectInfo.sYSize+1)+j].pos.y;
			if(fMaxPoint < g_pGround->m_pTileVertexArray[i*(g_pGround->m_projectInfo.sYSize+1)+(j+1)].pos.y)
				fMaxPoint = g_pGround->m_pTileVertexArray[i*(g_pGround->m_projectInfo.sYSize+1)+(j+1)].pos.y;
			if(fMaxPoint < g_pGround->m_pTileVertexArray[(i+1)*(g_pGround->m_projectInfo.sYSize+1)+(j)].pos.y)
				fMaxPoint = g_pGround->m_pTileVertexArray[(i+1)*(g_pGround->m_projectInfo.sYSize+1)+(j)].pos.y;
			// ÁöÇüŔÇ łë¸Öş¤ĹÍ¸¦ ±¸ÇŃ´Ů.
			D3DXVec3Cross(&m_vNormal,&(g_pGround->m_pTileVertexArray[(i)*(g_pGround->m_projectInfo.sYSize+1)+(j+1)].pos
				-g_pGround->m_pTileVertexArray[(i)*(g_pGround->m_projectInfo.sYSize+1)+j].pos),
				&(g_pGround->m_pTileVertexArray[(i+1)*(g_pGround->m_projectInfo.sYSize+1)+(j)].pos
				-g_pGround->m_pTileVertexArray[(i)*(g_pGround->m_projectInfo.sYSize+1)+(j+1)].pos));
			D3DXVec3Normalize(&m_vNormal,&m_vNormal);
			if(m_vNormal.y > 0.5f) // °ć»ç ĂĽĹ©ÇĎż© »ó˝Â ÇŇ°ÍŔÎÁö ÇĎÁö ľĘŔ»°ÍŔÎÁö? TRUE:»ó˝Â FALSE:»ó˝ÂľČÇÔ
				return TRUE;
			else
				return FALSE;
		}
		if(g_pGround->IntersectTriangle(vPos,vVel,g_pGround->m_pTileVertexArray[i*(g_pGround->m_projectInfo.sYSize+1)+j+1].pos,
			g_pGround->m_pTileVertexArray[(i+1)*(g_pGround->m_projectInfo.sYSize+1)+(j)].pos,
			g_pGround->m_pTileVertexArray[(i+1)*(g_pGround->m_projectInfo.sYSize+1)+j+1].pos,
			&fDist))//, &fBary1, &fBary2 ))
		{
			// ĽĽÁˇÁß ĂÖ°í łôŔşÁˇŔ» ŔúŔĺÇŃ´Ů.
			fMaxPoint = g_pGround->m_pTileVertexArray[i*(g_pGround->m_projectInfo.sYSize+1)+j+1].pos.y;
			if(fMaxPoint < g_pGround->m_pTileVertexArray[(i+1)*(g_pGround->m_projectInfo.sYSize+1)+(j)].pos.y)
				fMaxPoint = g_pGround->m_pTileVertexArray[(i+1)*(g_pGround->m_projectInfo.sYSize+1)+(j)].pos.y;
			if(fMaxPoint < g_pGround->m_pTileVertexArray[(i+1)*(g_pGround->m_projectInfo.sYSize+1)+(j+1)].pos.y)
				fMaxPoint = g_pGround->m_pTileVertexArray[(i+1)*(g_pGround->m_projectInfo.sYSize+1)+(j+1)].pos.y;
			// ÁöÇüŔÇ łë¸Öş¤ĹÍ¸¦ ±¸ÇŃ´Ů.
			D3DXVec3Cross(&m_vNormal,&(g_pGround->m_pTileVertexArray[(i+1)*(g_pGround->m_projectInfo.sYSize+1)+(j+1)].pos
				-g_pGround->m_pTileVertexArray[(i)*(g_pGround->m_projectInfo.sYSize+1)+j+1].pos),
				&(g_pGround->m_pTileVertexArray[(i+1)*(g_pGround->m_projectInfo.sYSize+1)+(j)].pos
				-g_pGround->m_pTileVertexArray[(i+1)*(g_pGround->m_projectInfo.sYSize+1)+(j+1)].pos));
			D3DXVec3Normalize(&m_vNormal,&m_vNormal);
			if(m_vNormal.y > 0.5f)
				return TRUE;
			else
				return FALSE;
		}
	}
	return FALSE;
}

void CShuttleChild::SetMoveType(D3DXVECTOR3 vPos,float fHeight)
{
	FLOG( "CShuttleChild::SetMoveType(D3DXVECTOR3 vPos,float fHeight)" );
	if(IS_DT(m_myShuttleInfo.UnitKind) && !m_bIsAir)
	{
		if(vPos.y > 0.9f) // - ?
			m_bCheckMove = 3;
		else if(vPos.y > 0.866f) // - 30
			m_bCheckMove = 2;
		else if(vPos.y > 0.7f) // 30-45
			m_bCheckMove = 1;
		else // 45µµ ŔĚ»ó
			m_bCheckMove = 0;
	}
	else
	{
		if(vPos.y > 0.866f)
			m_bCheckMove = 3; // 30µµ ąĚ¸¸
		else if(vPos.y > 0.7f) //30-45
			m_bCheckMove = 2;
		else if(vPos.y > 0.5f) // 45-60
			m_bCheckMove = 1;
		else // 60µµ ŔĚ»ó
		{
			if(fHeight - m_vPos.y < 20.0f)
				m_bCheckMove = 1;	// °ć»ç°ˇ ±ŢÇŘµµ ŔŰŔş ľđ´öŔĚ¶ó¸é łŃľî°ˇŔÚ
			else
				m_bCheckMove = 0;
		}
	}
}

void CShuttleChild::ResortingItem()
{
	FLOG( "CShuttleChild::ResortingItem()" );
	if(m_pStoreData)
	{
		m_pStoreData->ResortingItemInInventory();
	}
}

BOOL CShuttleChild::CheckIsWater(D3DXVECTOR3 vPos)
{
	FLOG( "CShuttleChild::CheckIsWater(D3DXVECTOR3 vPos)" );

	// 2005-02-11 by jschoi ą° żŔşęÁ§Ć® ¸ŐŔú °Ë»ç
	D3DXMATRIX mat;
	D3DXVECTOR3 vSide(0,0,1),vUp(0,1,0);
	D3DXMatrixLookAtLH(&mat,&m_vPos,&(m_vPos - m_vUp),&vSide);		// ľĆ·ˇ
		
	if(	g_pScene &&
		g_pScene->m_pObjectRender)
	{	
		COLLISION_RESULT collResult = g_pScene->m_pObjectRender->CheckCollMeshWaterObject(mat,m_vPos);
		if(collResult.fDist != DEFAULT_COLLISION_DISTANCE)
		{
			return TRUE;
		}
	}

	int i, j;
	i = (int)(vPos.x / TILE_SIZE);
	j = (int)(vPos.z / TILE_SIZE);
	if(i >= 0 && i < g_pGround->m_projectInfo.sXSize && j >= 0 && j < g_pGround->m_projectInfo.sYSize)
	{
		int k = (i*g_pGround->m_projectInfo.sYSize+j);

		// 2005-04-06 by jschoi
//		return g_pGround->m_pTileInfo[k].useWater;
		return g_pGround->m_pTileInfo[k].dwWaterType == 1 ? TRUE : FALSE ;
	}

	return FALSE;
}

//void CShuttleChild::UpdateEngine(int nEngineType)
//{
//	FLOG( "CShuttleChild::UpdateEngine(int nEngineType)" );
//	ITEM * item = g_pDatabase->GetServerItemInfo(nEngineType);
//	if(item)
//	{
//		m_fShuttleSpeed = item->AbilityMin;
//		m_fShuttleSpeedMax = item->AbilityMax;
//		m_fShuttleSpeedBoosterOn = (float)item->Range;
//		m_fShuttleSpeedMin = item->AbilityMin;
//		m_fBURN = (float)item->Time/1000.0f;
//		m_fMouseRate = item->RangeAngle;
//		ASSERT_ASSERT(m_fBURN>0.0f);
//		if(g_pGameMain->m_pUnitInfoBar)
//		{
//			g_pGameMain->m_pUnitInfoBar->SetBURN(m_fCurrentBURN, m_fBURN);
//			g_pGameMain->m_pUnitInfoBar->SetSpeed(m_fShuttleSpeed, item->AbilityMin, (float)item->Range);
//		}
//
//	}
//}

//void CShuttleChild::ChangeEngine(int nEngineType, int nCurrentBURN)
//{
//	FLOG( "CShuttleChild::ChangeEngine(int nEngineType, int nCurrentBURN)" );
//	if(m_pEngine)
//	{
//		m_pEngine->m_bUsing = FALSE;
//		m_pEngine = NULL;
//	}
//	if(nEngineType && g_pD3dApp->m_pEffectList)
//	{
//		D3DXVECTOR3 vTempPos = D3DXVECTOR3(0,0,0);
//		m_pEngine = new CAppEffectData(this,nEngineType,vTempPos);
//		m_pEngine = (CAppEffectData *)g_pD3dApp->m_pEffectList->AddChild(m_pEngine);
//	}
//
//	ITEM * item = g_pDatabase->GetServerItemInfo(nEngineType);
//	if(item)
//	{
//		m_fShuttleSpeed = item->AbilityMin;
//		m_fShuttleSpeedMax = item->AbilityMax;
//		m_fShuttleSpeedBoosterOn = (float)item->Range;
//		m_fShuttleSpeedMin = item->AbilityMin;
//		m_fBURN = (float)item->Time/1000.0f;
//		m_fCurrentBURN = nCurrentBURN;
//		m_fMouseRate = item->RangeAngle;
//	}
//	else
//	{
//		m_fShuttleSpeed = 0.0f;
//		m_fShuttleSpeedMax = 10.0f;
//		m_fShuttleSpeedBoosterOn = 10.0f;
//		m_fShuttleSpeedMin = 0.0f;
//		m_fBURN = 1;
//		m_fCurrentBURN = 1;
//		m_fMouseRate = 0.5f;
//	}
//	ASSERT_ASSERT(m_fBURN>0.0f);
//	if(g_pGameMain && g_pGameMain->m_pUnitInfoBar)
//	{
//		g_pGameMain->m_pUnitInfoBar->SetBURN(m_fCurrentBURN, m_fBURN);
//		g_pGameMain->m_pUnitInfoBar->SetSpeed(m_fShuttleSpeed, m_fShuttleSpeedMin, m_fShuttleSpeedBoosterOn);
//	}
//}

void CShuttleChild::UpdateEngine(CItemInfo* pItemInfo)
{
	FLOG( "CShuttleChild::UpdateEngine(int nEngineType)" );
	ASSERT_ASSERT(pItemInfo);
	ITEM* pITEM = pItemInfo->GetRealItemInfo();
	m_fShuttleSpeed = pITEM->AbilityMin;
	m_fShuttleSpeedMax = pITEM->AbilityMax;
	m_fShuttleSpeedBoosterOn = (float)pITEM->Range;
	m_fShuttleSpeedMin = pITEM->AbilityMin;
	m_bySpeedPenalty = pITEM->SpeedPenalty;					// 2005-08-16 by ispark
	m_fBURN = (float)pITEM->Time/1000.0f;

	// 2008-09-26 by bhsohn ˝Ĺ±Ô ŔÎĂľĆ® Ăł¸®
	// ĂÖĽŇ ĽÓµµ
	{		
		float fTime = GetEnchantDesParam(pItemInfo, DES_ENGINE_MIN_SPEED_UP);
		m_fShuttleSpeedMin += fTime;

	}
	// ĂÖ´ë ĽÓµµ
	{		
		float fTime = GetEnchantDesParam(pItemInfo, DES_ENGINE_MAX_SPEED_UP);
		m_fShuttleSpeedMax += fTime;		
	}

	// Áö»ó ĽÓµµ
	{		
		float fTime = GetEnchantDesParam(pItemInfo, DES_ENGINE_GROUND_SPEED_UP);
		m_fShuttleGroundSpeedMax += fTime;		
	}

	// şÎ˝şĹÍ ĽÓµµ
	{		
		float fTime = GetEnchantDesParam(pItemInfo, DES_ENGINE_BOOSTER_SPEED_UP);
		m_fShuttleSpeedBoosterOn += fTime;		
	}
	// şÎ˝şĹÍ Ăß°ˇ ˝Ă°Ł 
	float fTimeUp = GetEnchantDesParam(pItemInfo, DES_ENGINE_BOOSTER_TIME_UP);
	m_fBURN += fTimeUp;

	// 2008-03-19 by bhsohn FLOATÇü ŔçÁ¤·Ä ÇĎż© »çżë
	m_fBURN = FloatRangeDecimal(m_fBURN, 2);

//	m_fMouseRate = pITEM->RangeAngle;
	CParamFactor*	pRefEnchant = pItemInfo->GetEnchantParamFactor();	
	if(pRefEnchant)
	{
		m_paramFactor.pfm_ENGINE_ANGLE_UP = pRefEnchant->pfm_ENGINE_ANGLE_UP;
		m_paramFactor.pfm_ENGINE_BOOSTERANGLE_UP = pRefEnchant->pfm_ENGINE_BOOSTERANGLE_UP;
	}
	else
	{
		// ĂĘ±âČ­		
		m_paramFactor.pfm_ENGINE_ANGLE_UP = m_paramFactor.pfm_ENGINE_BOOSTERANGLE_UP = 0.0f;
	}

	m_fMouseRate = CAtumSJ::GetEngineRangeAngle(pITEM, &m_paramFactor);;
	ASSERT_ASSERT(m_fBURN>0.0f);
	if(g_pGameMain->m_pUnitInfoBar)
	{
		g_pGameMain->m_pUnitInfoBar->SetBURN(m_fCurrentBURN, m_fBURN);
		g_pGameMain->m_pUnitInfoBar->SetSpeed(m_fShuttleSpeed, m_fShuttleSpeedMin, m_fShuttleSpeedBoosterOn);
	}

}

void CShuttleChild::ChangeEngine(CItemInfo* pItemInfo, int nCurrentBURN)
{
	FLOG( "CShuttleChild::ChangeEngine(int nEngineType, int nCurrentBURN)" );
	if(pItemInfo == NULL)
	{
		m_fShuttleSpeed = 0.0f;
		m_fShuttleSpeedMax = 10.0f;
		m_fShuttleSpeedBoosterOn = 10.0f;
		m_fShuttleSpeedMin = 0.0f;
		m_bySpeedPenalty = 0;							// 2005-08-16 by ispark
		m_fBURN = 1;
		m_fCurrentBURN = 1;
		m_fMouseRate = 0.5f;
		return;
	}
	if(m_pEngine)
	{
		m_pEngine->m_bUsing = FALSE;
		m_pEngine = NULL;
	}
	D3DXVECTOR3 vTempPos = D3DXVECTOR3(0,0,0);
//	m_pEngine = new CAppEffectData(this,pItemInfo->ItemNum,vTempPos);				// 2005-08-23 by ispark
	// 2005-10-25 by ispark
	if(m_dwPartType != _ADMIN)
	{
		m_pEngine = new CAppEffectData(this,pItemInfo->ItemInfo->SourceIndex,vTempPos, ENGINE_POSITION);
		m_pEngine = (CAppEffectData *)g_pD3dApp->m_pEffectList->AddChild(m_pEngine);
	}

	ITEM* pITEM = pItemInfo->GetRealItemInfo();

// 2010. 04. 15 by jskim şÎ˝şĹÍ ĂÖĽŇ ĽÓµµ, ĂÖ°í ĽÓµµ 2ąč µÇ´Â ąö±× ĽöÁ¤
// 	// 2010. 03. 25 by jskim Speed, Angle Ăß°ˇ °č»ę
// 
// // 	// 2004-11-04 by jschoi
// // 	// 2005-11-03 by ispark
// // 	// Á¤Áö »óĹÂżˇĽ­ żŁÁřŔ» ±łČŻÇŇ ¶§ ľŐŔ¸·Î ŔĚµżÇĎÁö ľĘµµ·Ď ÇŃ´Ů.
// // 	if(m_fShuttleSpeed != 0.0f)
// // 	{
// // //		m_fShuttleSpeed = pITEM->AbilityMin;
// // 		m_fShuttleSpeed = 0.0f;
// // 	}
// // 	m_fShuttleSpeedMax = pITEM->AbilityMax;
// // 	m_fShuttleSpeedBoosterOn = (float)pITEM->Range;
// // 	m_fShuttleSpeedMin = pITEM->AbilityMin;
// // 	m_fShuttleGroundSpeedMax = pITEM->Charging;			// 2005-11-26 by ispark
// // 	m_bySpeedPenalty = pITEM->SpeedPenalty;				// 2005-08-16 by ispark
// // 	m_fBURN = (float)pITEM->Time/1000.0f;
// // 
// // 	// 2008-09-26 by bhsohn ˝Ĺ±Ô ŔÎĂľĆ® Ăł¸®
// // 	// ĂÖĽŇ ĽÓµµ
// // 	{		
// // 		float fTime = GetEnchantDesParam(pItemInfo, DES_ENGINE_MIN_SPEED_UP);
// // 		m_fShuttleSpeedMin += fTime;
// // 
// // 	}
// // 	// ĂÖ´ë ĽÓµµ
// // 	{		
// // 		float fTime = GetEnchantDesParam(pItemInfo, DES_ENGINE_MAX_SPEED_UP);
// // 		m_fShuttleSpeedMax += fTime;		
// // 	}
// // 
// // 	// Áö»ó ĽÓµµ
// // 	{		
// // 		float fTime = GetEnchantDesParam(pItemInfo, DES_ENGINE_GROUND_SPEED_UP);
// // 		m_fShuttleGroundSpeedMax += fTime;		
// // 	}
// // 
// // 	// şÎ˝şĹÍ ĽÓµµ
// // 	{		
// // 		float fTime = GetEnchantDesParam(pItemInfo, DES_ENGINE_BOOSTER_SPEED_UP);
// // 		m_fShuttleSpeedBoosterOn += fTime;		
// // 	}
// // 	// şÎ˝şĹÍ Ăß°ˇ ˝Ă°Ł 
// // 	float fTimeUp = GetEnchantDesParam(pItemInfo, DES_ENGINE_BOOSTER_TIME_UP);
// // 	m_fBURN += fTimeUp;
// // 	
// // 	// 2008-03-19 by bhsohn FLOATÇü ŔçÁ¤·Ä ÇĎż© »çżë
// // 	m_fBURN = FloatRangeDecimal(m_fBURN, 2);
// // 
// // 	//DbgOut("#1 m_fCurrentBURN[%.2f] nCurrentBURN[%d] m_fBURN[%.2f] \n", m_fCurrentBURN, nCurrentBURN, m_fBURN);
// // 	if((m_fCurrentBURN == 0) ||							// 2005-12-10 by ispark, ÇöŔç şÎ˝şĹÍ °ÔŔĚÁö°ˇ 0ŔĚ¶ó¸é ĽÂĆĂ. ŔÚÁÖ ¸®ĽÂ µÇ´Â °ćżě ¶§ą®żˇ ŔĚ·¸°Ô ÇÔ
// // 		(m_fCurrentBURN > m_fBURN))						// 2005-12-15 by ispark, ÇöŔç şÎ˝şĹÍ °ÔŔĚÁö°ˇ ĽÂĆĂµÇ´Â żŁÁř şÎ˝şĹÍ ĂÖ´ëş¸´Ů Ĺ©¸é ´Ů˝Ă ¸®ĽÂ
// // 	{
// // 		m_fCurrentBURN = nCurrentBURN;
// // 	}
// // //	m_fMouseRate = pITEM->RangeAngle;
// // 	CParamFactor*	pRefEnchant = pItemInfo->GetEnchantParamFactor();
// // 	if(pRefEnchant)
// // 	{
// // 		m_paramFactor.pfm_ENGINE_ANGLE_UP = pRefEnchant->pfm_ENGINE_ANGLE_UP;
// // 		m_paramFactor.pfm_ENGINE_BOOSTERANGLE_UP = pRefEnchant->pfm_ENGINE_BOOSTERANGLE_UP;
// // 	}
// // 	else
// // 	{
// // 		// ĂĘ±âČ­		
// // 		m_paramFactor.pfm_ENGINE_ANGLE_UP = m_paramFactor.pfm_ENGINE_BOOSTERANGLE_UP = 0.0f;
// // 	}
// // 
// // 	m_fMouseRate = CAtumSJ::GetEngineRangeAngle(pITEM, &m_paramFactor);
// 
// 	// Á¤Áö »óĹÂżˇĽ­ żŁÁřŔ» ±łČŻÇŇ ¶§ ľŐŔ¸·Î ŔĚµżÇĎÁö ľĘµµ·Ď ÇŃ´Ů.
// 	if(m_fShuttleSpeed != 0.0f)
// 		m_fShuttleSpeed = 0.0f;
// 
// 	m_fShuttleSpeedMax			= pITEM->AbilityMax + GetEnchantDesParam(pItemInfo, DES_ENGINE_MAX_SPEED_UP);
// 	m_fShuttleSpeedBoosterOn	= (float)pITEM->Range + GetEnchantDesParam(pItemInfo, DES_ENGINE_BOOSTER_SPEED_UP);
// 	m_fShuttleSpeedMin			= pITEM->AbilityMin	+ GetEnchantDesParam(pItemInfo, DES_ENGINE_MIN_SPEED_UP);
// 	m_fShuttleGroundSpeedMax	= pITEM->Charging + GetEnchantDesParam(pItemInfo, DES_ENGINE_GROUND_SPEED_UP);
// 	m_bySpeedPenalty			= pITEM->SpeedPenalty;
// 	m_fBURN						= (float)pITEM->Time/1000.0f + GetEnchantDesParam(pItemInfo, DES_ENGINE_BOOSTER_TIME_UP);
// 	m_fBURN						= FloatRangeDecimal(m_fBURN, 2);
// 
// 	CParamFactor*	pRefEnchant = pItemInfo->GetEnchantParamFactor();
// 	if(pRefEnchant)
// 	{
// 		m_paramFactor.pfm_ENGINE_ANGLE_UP			= pRefEnchant->pfm_ENGINE_ANGLE_UP;
// 		m_paramFactor.pfm_ENGINE_BOOSTERANGLE_UP	= pRefEnchant->pfm_ENGINE_BOOSTERANGLE_UP;
// 	}
// 	else
// 		m_paramFactor.pfm_ENGINE_ANGLE_UP			= m_paramFactor.pfm_ENGINE_BOOSTERANGLE_UP = 0.0f;
// 
// 	m_fMouseRate = CAtumSJ::GetEngineRangeAngle(pITEM, &m_paramFactor);
// 
// 	for( int i=POS_PROW; i <= POS_PET; ++i )
// 	{
// 		CItemInfo*	pEquipItem = g_pStoreData->FindItemInInventoryByWindowPos( i );
// 
// 		if( !pEquipItem || i == POS_REAR )
// 			continue;
// 
// 		// şÎ˝şĹÍ Ăß°ˇ ˝Ă°Ł
// 		m_fBURN				+= GetEnchantDesParam( pEquipItem, DES_ENGINE_BOOSTER_TIME_UP )
// 							   + pEquipItem->GetRealItemInfo()->GetParameterValue( DES_ENGINE_BOOSTER_TIME_UP );
// 		FloatRangeDecimal( m_fBURN, 2 );
// 
// 		// ĂÖĽŇ ĽÓµµ
// 		m_fShuttleSpeedMin	+= GetEnchantDesParam( pEquipItem, DES_ENGINE_MIN_SPEED_UP )
// 							   + pEquipItem->GetRealItemInfo()->GetParameterValue( DES_ENGINE_MIN_SPEED_UP );
// 
// 		// ĂÖ´ë ĽÓµµ
// 		m_fShuttleSpeedMax	+= GetEnchantDesParam( pEquipItem, DES_ENGINE_MAX_SPEED_UP )
// 							   + pEquipItem->GetRealItemInfo()->GetParameterValue( DES_ENGINE_MAX_SPEED_UP );
// 
// 		// Áö»ó ĽÓµµ
// 		m_fShuttleGroundSpeedMax	+= GetEnchantDesParam( pEquipItem, DES_ENGINE_GROUND_SPEED_UP )
// 									   + pEquipItem->GetRealItemInfo()->GetParameterValue( DES_ENGINE_GROUND_SPEED_UP );
// 
// 		// şÎ˝şĹÍ ˝Ă°Ł
// 		m_fShuttleSpeedBoosterOn	+= GetEnchantDesParam( pEquipItem, DES_ENGINE_BOOSTER_SPEED_UP )
// 									   + pEquipItem->GetRealItemInfo()->GetParameterValue( DES_ENGINE_BOOSTER_SPEED_UP );
// 
// 		// Ľ±Č¸°˘
// 		m_paramFactor.pfm_ENGINE_ANGLE_UP	+= GetEnchantDesParam( pEquipItem, DES_ENGINE_ANGLE_UP )
// 											   + pEquipItem->GetRealItemInfo()->GetParameterValue( DES_ENGINE_ANGLE_UP );
// 
// 		// şÎ˝şĹÍ˝Ă Ľ±Č¸°˘
// 		m_paramFactor.pfm_ENGINE_BOOSTERANGLE_UP	+= GetEnchantDesParam( pEquipItem, DES_ENGINE_BOOSTERANGLE_UP )
// 													   + pEquipItem->GetRealItemInfo()->GetParameterValue( DES_ENGINE_BOOSTERANGLE_UP );
// 
// 		m_fMouseRate	= CAtumSJ::GetEngineRangeAngle(pITEM, &m_paramFactor);
// 	}
// 
// 
// 	if((m_fCurrentBURN == 0) ||			// ÇöŔç şÎ˝şĹÍ °ÔŔĚÁö°ˇ 0ŔĚ¶ó¸é ĽÂĆĂ. ŔÚÁÖ ¸®ĽÂ µÇ´Â °ćżě ¶§ą®żˇ ŔĚ·¸°Ô ÇÔ
// 		(m_fCurrentBURN > m_fBURN))		// ÇöŔç şÎ˝şĹÍ °ÔŔĚÁö°ˇ ĽÂĆĂµÇ´Â żŁÁř şÎ˝şĹÍ ĂÖ´ëş¸´Ů Ĺ©¸é ´Ů˝Ă ¸®ĽÂ
// 	{
// 		m_fCurrentBURN = nCurrentBURN;
// 	}
// 
// 	// end 2010. 03. 25 by jskim Speed, Angle Ăß°ˇ °č»ę

 	if(m_fShuttleSpeed != 0.0f)
 	{
		m_fShuttleSpeed = pITEM->AbilityMin;
   	    m_fShuttleSpeed = 0.0f;
 	}
 	m_fShuttleSpeedMax = pITEM->AbilityMax;
 	m_fShuttleSpeedBoosterOn = (float)pITEM->Range;
 	m_fShuttleSpeedMin = pITEM->AbilityMin;
 	m_fShuttleGroundSpeedMax = pITEM->Charging;			// 2005-11-26 by ispark
 	m_bySpeedPenalty = pITEM->SpeedPenalty;				// 2005-08-16 by ispark
 	m_fBURN = (float)pITEM->Time/1000.0f;
 
 	// 2008-09-26 by bhsohn ˝Ĺ±Ô ŔÎĂľĆ® Ăł¸®
 	// ĂÖĽŇ ĽÓµµ
 	{		
 		float fTime = GetEnchantDesParam(pItemInfo, DES_ENGINE_MIN_SPEED_UP);
 		m_fShuttleSpeedMin += fTime; 
 	}
 	// ĂÖ´ë ĽÓµµ
 	{		
 		float fTime = GetEnchantDesParam(pItemInfo, DES_ENGINE_MAX_SPEED_UP);
 		m_fShuttleSpeedMax += fTime;		
 	}
 
 	// Áö»ó ĽÓµµ
 	{		
 		float fTime = GetEnchantDesParam(pItemInfo, DES_ENGINE_GROUND_SPEED_UP);
 		m_fShuttleGroundSpeedMax += fTime;		
 	}
 
 	// şÎ˝şĹÍ ĽÓµµ
 	{		
		float fTime = GetEnchantDesParam(pItemInfo, DES_ENGINE_BOOSTER_SPEED_UP);
 		m_fShuttleSpeedBoosterOn += fTime;		
 	}
 	// şÎ˝şĹÍ Ăß°ˇ ˝Ă°Ł 
 	float fTimeUp = GetEnchantDesParam(pItemInfo, DES_ENGINE_BOOSTER_TIME_UP);
 	m_fBURN += fTimeUp;
 	
 	// 2008-03-19 by bhsohn FLOATÇü ŔçÁ¤·Ä ÇĎż© »çżë
 	m_fBURN = FloatRangeDecimal(m_fBURN, 2);

	// 2010. 04. 27 by jskim ş¸±Ţ »óÁˇżˇĽ­ ŔĚ·ú˝Ă şÎ˝şĹÍ 0µÇ´Â ąö±× ĽöÁ¤
	for(int i = POS_PROW; i <= POS_PET; i++)
	{
		CItemInfo* pAllItem = g_pStoreData->FindItemInInventoryByWindowPos( i ); 
		
		if(!pAllItem || i == POS_REAR )
			continue;
		
		// 2010. 04. 21 by jskim ˝Ĺ±Ô ·°Ĺ° ¸Ó˝Ĺ ±¸Çö
		//// şÎ˝şĹÍ Ăß°ˇ ˝Ă°Ł 
		//m_fBURN += GetEnchantDesParam(pAllItem, DES_ENGINE_BOOSTER_TIME_UP) + pAllItem->GetRealItemInfo()->GetParameterValue(DES_ENGINE_BOOSTER_TIME_UP);
		float tempBURN = 0.0f;
		if(pAllItem && i == POS_CENTER)
		{
			CItemInfo* pAllItem = g_pStoreData->FindItemInInventoryByWindowPos( i );
			
			for(int j=0; j <SIZE_DES_PARAM_PER_RARE_ITEM_INFO; j++)
			{
				if(pAllItem->GetPrefixRareInfo())
				{
					if(pAllItem->GetPrefixRareInfo()->DesParameter[j] == DES_ENGINE_BOOSTER_TIME_UP)
					{
						tempBURN += pAllItem->GetPrefixRareInfo()->ParameterValue[j];
					}
				}
				if(pAllItem->GetSuffixRareInfo())
				{
					if(pAllItem->GetSuffixRareInfo()->DesParameter[j] == DES_ENGINE_BOOSTER_TIME_UP)
					{
						tempBURN += pAllItem->GetSuffixRareInfo()->ParameterValue[j];
					}
				}
			}
		}	
		// şÎ˝şĹÍ Ăß°ˇ ˝Ă°Ł 
		m_fBURN += GetEnchantDesParam(pAllItem, DES_ENGINE_BOOSTER_TIME_UP) + pAllItem->GetRealItemInfo()->GetParameterValue(DES_ENGINE_BOOSTER_TIME_UP) + tempBURN;
		//end 2010. 04. 21 by jskim ˝Ĺ±Ô ·°Ĺ° ¸Ó˝Ĺ ±¸Çö
		m_fBURN = FloatRangeDecimal(m_fBURN, 2);
	}
	
	// »çżë·ů ľĆŔĚĹŰ ±â´É °ˇÁ®żŔ±â
	vector<ItemFontInfo*> ItemInfo = g_pGameMain->m_pInfSkill->GetItemFontInfo();
	vector<ItemFontInfo*>::iterator it = ItemInfo.begin();
	while(it != ItemInfo.end())
	{
		// şÎ˝şĹÍ Ăß°ˇ ˝Ă°Ł 
		if((*it)->pItemInfo)
		{
			m_fBURN += (*it)->pItemInfo->GetParameterValue(DES_ENGINE_BOOSTER_TIME_UP);
			// FLOATÇü ŔçÁ¤·Ä ÇĎż© »çżë
			m_fBURN = FloatRangeDecimal(m_fBURN, 2);
		}
		it++;
 	}
	//end 2010. 04. 27 by jskim ş¸±Ţ »óÁˇżˇĽ­ ŔĚ·ú˝Ă şÎ˝şĹÍ 0µÇ´Â ąö±× ĽöÁ¤
 
 	//DbgOut("#1 m_fCurrentBURN[%.2f] nCurrentBURN[%d] m_fBURN[%.2f] \n", m_fCurrentBURN, nCurrentBURN, m_fBURN);
 	if((m_fCurrentBURN == 0) ||							// 2005-12-10 by ispark, ÇöŔç şÎ˝şĹÍ °ÔŔĚÁö°ˇ 0ŔĚ¶ó¸é ĽÂĆĂ. ŔÚÁÖ ¸®ĽÂ µÇ´Â °ćżě ¶§ą®żˇ ŔĚ·¸°Ô ÇÔ
 		(m_fCurrentBURN > m_fBURN))						// 2005-12-15 by ispark, ÇöŔç şÎ˝şĹÍ °ÔŔĚÁö°ˇ ĽÂĆĂµÇ´Â żŁÁř şÎ˝şĹÍ ĂÖ´ëş¸´Ů Ĺ©¸é ´Ů˝Ă ¸®ĽÂ
 	{
 		m_fCurrentBURN = nCurrentBURN;
 	}
 //	m_fMouseRate = pITEM->RangeAngle;
 	CParamFactor*	pRefEnchant = pItemInfo->GetEnchantParamFactor();
 	if(pRefEnchant)
 	{
 		m_paramFactor.pfm_ENGINE_ANGLE_UP = pRefEnchant->pfm_ENGINE_ANGLE_UP;
 		m_paramFactor.pfm_ENGINE_BOOSTERANGLE_UP = pRefEnchant->pfm_ENGINE_BOOSTERANGLE_UP;
 	}
 	else
 	{
 		// ĂĘ±âČ­		
 		m_paramFactor.pfm_ENGINE_ANGLE_UP = m_paramFactor.pfm_ENGINE_BOOSTERANGLE_UP = 0.0f;
 	}
  	m_fMouseRate = CAtumSJ::GetEngineRangeAngle(pITEM, &m_paramFactor);
//end 2010. 04. 15 by jskim şÎ˝şĹÍ ĂÖĽŇ ĽÓµµ, ĂÖ°í ĽÓµµ 2ąč µÇ´Â ąö±× ĽöÁ¤

	ASSERT_ASSERT(m_fBURN>0.0f);

	if(g_pGameMain && g_pGameMain->m_pUnitInfoBar)
	{
		g_pGameMain->m_pUnitInfoBar->SetBURN(m_fCurrentBURN, m_fBURN);
//		g_pGameMain->m_pUnitInfoBar->SetSpeed(m_fShuttleSpeed, m_fShuttleSpeedMin, m_fShuttleSpeedBoosterOn);
		g_pGameMain->m_pUnitInfoBar->SetSpeed(m_fShuttleSpeed, m_fShuttleSpeedMin, m_fShuttleSpeedBoosterOn);
	}
}

void CShuttleChild::InitEffect()
{
	FLOG( "CShuttleChild::InitEffect()" );

	if(COMPARE_RACE(m_myShuttleInfo.Race,RACE_GAMEMASTER))
	{
		//m_dwPartType = _ADMIN;
	}

//	if(m_dwPartType == _ADMIN)
//	{
//		if(!m_pCharacterInfo)
//		{
//			char strFileName[16];
//			wsprintf( strFileName, "10000000");
//			LoadCharacterEffect(strFileName);
//			if(m_pCharacterInfo)
//			{
//				m_pCharacterInfo->SetAllBodyConditionMatrix(m_mMatrix);
//				m_pCharacterInfo->SetCharacterAnimationBodyConditionMask(BODYCON_CHARACTER_ANIMATION_TIME);
//				m_pCharacterInfo->SetBodyCondition(m_myShuttleInfo.BodyCondition);
//			}
//			m_nUnitNum = 10000000;
//		}
//	}
//	else
//	{
		CItemInfo* pItem = m_pStoreData->FindItemInInventoryByWindowPos( POS_CENTER );
		// 2005-07-20 by ispark
		// Äł¸ŻĹÍ ·»´ő¸µ ąřČŁ¸¦ ŔÔ·ÂÇŃ´Ů. ż©±âżˇ ŔĚĆĺĆ® Ć÷ÇÔ

		// 2009. 08. 27 by ckPark ±×·ˇÇČ ¸®ĽŇ˝ş şŻ°ć ˝Ă˝şĹŰ ±¸Çö
		//ChangeUnitCharacterInfo((pItem?pItem->ItemInfo->SourceIndex:0), m_myShuttleInfo.UnitKind, GetCurrentBodyCondition(), FALSE);

		int nDefenseNum = 0;

		if( !pItem )
			nDefenseNum = 0;
		else
		{
			if (!pItem->ShapeItemNum) {
				if(!pItem->ColorCode)
					nDefenseNum = pItem->ItemInfo->SourceIndex;
				else
					nDefenseNum = pItem->ColorCode;
			}
			else
			{
				ITEM* pShapeItem = g_pDatabase->GetServerItemInfo( pItem->ShapeItemNum );
				if( pShapeItem )
					nDefenseNum = pShapeItem->SourceIndex;
				else
					nDefenseNum = pItem->ItemInfo->SourceIndex;
			}
		}
		// 2010. 03. 18 by jskim ¸ó˝şĹÍşŻ˝Ĺ Ä«µĺ
		//ChangeUnitCharacterInfo( nDefenseNum, m_myShuttleInfo.UnitKind, GetCurrentBodyCondition(), FALSE );
		BOOL bMonTras = FALSE;

		if( m_MonsterTransformer )
		{
			ChangeUnitCharacterInfo(nDefenseNum, m_myShuttleInfo.UnitKind, GetCurrentBodyCondition(), FALSE);
			SetMonsterTransPrimarySidePos(m_PrimaryAttack.vSidePos);
			SetMonsterTransSecondarySidePos(m_SecondaryAttack.vSidePos);
		}

		if( m_MonsterTransformer )
		{
			bMonTras = TRUE;
			MEX_MONSTER_INFO * pTempIn = g_pGameMain->CheckMonsterInfo( m_MonsterTransformer );
			if(pTempIn)
				nDefenseNum = pTempIn->SourceIndex;
		}
		ChangeUnitCharacterInfo( nDefenseNum, m_myShuttleInfo.UnitKind, GetCurrentBodyCondition(), FALSE, bMonTras );
		//end 2010. 03. 18 by jskim ¸ó˝şĹÍşŻ˝Ĺ Ä«µĺ
		// end 2009. 08. 27 by ckPark ±×·ˇÇČ ¸®ĽŇ˝ş şŻ°ć ˝Ă˝şĹŰ ±¸Çö
//	}
}

void CShuttleChild::CheckGroundEffect()
{
	FLOG( "CShuttleChild::CheckGroundEffect()" );
	if( g_pScene->m_byMapType == MAP_TYPE_CITY || 
		m_dwState == _EXPLODED || 
		m_dwState == _FALLEN || 
		m_dwState == _LANDED)
	{
		CAppEffectData * pData = (CAppEffectData*)g_pD3dApp->m_pEffectList->m_pChild;
		while(pData)
		{
			if(pData->m_pParent == this && !COMPARE_BODYCON_BIT(pData->m_bodyCondition,BODYCON_EXPLOSION_MASK))
			{
				if(pData->m_nType == RC_EFF_GROUND_01 || pData->m_nType == RC_EFF_GROUND_02)
				{
					pData->ChangeBodyCondition(BODYCON_EXPLOSION_MASK);
//					pData->m_bodyCondition = BODYCON_EXPLOSION_MASK;
//					if(pData->m_pCharacterInfo)
//						pData->m_pCharacterInfo->ChangeBodyCondition(pData->m_bodyCondition);
				}
			}
			pData = (CAppEffectData*)pData->m_pNext;
		}
		m_bEffectGround = 0;
	}
	else// if(m_dwState != _EXPLODED)
	{
		BYTE bOldEffectGround = m_bEffectGround;
		m_bEffectGround = 0;
		FLOAT fDist,fHeight,fCheckHeight;
		if(m_bOnObject)
			fCheckHeight = 15.0f;
		else
			fCheckHeight = 30.0f;
		int i,j;
		i = (int)(m_vPos.x/TILE_SIZE);
		j = (int)(m_vPos.z/TILE_SIZE);
		D3DXVECTOR3 v,dir;
		dir = D3DXVECTOR3(0,-1,0);
		m_bIsWater = CheckIsWater(m_vPos);

		// 2005-02-11 by jschoi
		D3DXMATRIX mat;
		D3DXVECTOR3 vSide(0,0,1),vUp(0,1,0);
		D3DXMatrixLookAtLH(&mat,&m_vPos,&(m_vPos - m_vUp),&vSide);		// ľĆ·ˇ
		if(	m_bIsWater &&
			g_pScene &&
			g_pScene->m_pObjectRender)
		{	
			COLLISION_RESULT collResult = g_pScene->m_pObjectRender->CheckCollMeshWaterObject(mat,m_vPos);
			if(collResult.fDist < fCheckHeight)
			{
				fHeight = m_vPos.y - collResult.fDist + 1.0f;
				m_bEffectGround = 1;
			}
		}

		if(i >= 0 && i < g_pGround->m_projectInfo.sXSize && j >= 0 && j < g_pGround->m_projectInfo.sYSize)
		{
			if(g_pGround->IntersectTriangle(m_vPos,dir,g_pGround->m_pTileVertexArray[i*(g_pGround->m_projectInfo.sYSize+1)+j].pos,
				g_pGround->m_pTileVertexArray[i*(g_pGround->m_projectInfo.sYSize+1)+(j+1)].pos,
				g_pGround->m_pTileVertexArray[(i+1)*(g_pGround->m_projectInfo.sYSize+1)+(j)].pos,
				&fDist))
			{
				v = m_vPos + dir*fDist;
				if(m_bIsWater)
				{
					if(v.y > g_pGround->m_projectInfo.fWaterHeight)
					{//¶Ą
						if(m_vPos.y <= v.y + fCheckHeight)
						{
							fHeight = v.y + 1.0f;
							m_bEffectGround = 2;
						}
					}
					else
					{//ą°
						if(	m_vPos.y <= g_pGround->m_projectInfo.fWaterHeight + fCheckHeight )
						{
							fHeight = g_pGround->m_projectInfo.fWaterHeight + 1.0f;
							m_bEffectGround = 1;
						}
					}
				}
				else
				{//¶Ą
					if(m_vPos.y <= v.y + fCheckHeight)
					{
						fHeight = v.y + 1.0f;
						m_bEffectGround = 2;
					}
				}
			}
			else if(g_pGround->IntersectTriangle(m_vPos,dir,g_pGround->m_pTileVertexArray[i*(g_pGround->m_projectInfo.sYSize+1)+j+1].pos,
				g_pGround->m_pTileVertexArray[(i+1)*(g_pGround->m_projectInfo.sYSize+1)+(j)].pos,
				g_pGround->m_pTileVertexArray[(i+1)*(g_pGround->m_projectInfo.sYSize+1)+j+1].pos,
				&fDist))
			{
				v = m_vPos + dir*fDist;
				if(m_bIsWater)
				{
					if(v.y > g_pGround->m_projectInfo.fWaterHeight)
					{//¶Ą
						if(m_vPos.y <= v.y + fCheckHeight)
						{
							fHeight = v.y + 1.0f;
							m_bEffectGround = 2;
						}
					}
					else
					{//ą°						
						if(m_vPos.y <= g_pGround->m_projectInfo.fWaterHeight + fCheckHeight)
						{
							fHeight = g_pGround->m_projectInfo.fWaterHeight + 1.0f;
							m_bEffectGround = 1;
						}
					}
				}
				else
				{//¶Ą
					if(m_vPos.y <= v.y + fCheckHeight)
					{
						fHeight = v.y + 1.0f;
						m_bEffectGround = 2;
					}
				}
			}
		}
		// 2008-12-04 by bhsohn ĂÖĽŇČ­ ¸đµĺ˝Ă, ą° ·Ł´ő¸µ Ăł¸®
//		BYTE bEffectGround = m_bEffectGround;
//		if(g_pSOption->sLowQuality)
//		{
//			// ŔĚ şÎşĐŔ» ĽöÁ¤ÇĎ¸é ĂÖĽŇ 7ÇÁ·ąŔÓ ŔĚ»ó ş¸Ŕĺ~~~
//			// ˛Ŕ, ĽöÁ¤ÇĎŔÚ~~~~!
//			m_bEffectGround = 0;
//		}
		// end 2008-12-04 by bhsohn ĂÖĽŇČ­ ¸đµĺ˝Ă, ą° ·Ł´ő¸µ Ăł¸®
		if( IsTileMapRenderEnable(m_myShuttleInfo.MapChannelIndex.MapIndex) == TRUE &&
			bOldEffectGround != m_bEffectGround)
		{
			CAppEffectData * pData = (CAppEffectData*)g_pD3dApp->m_pEffectList->m_pChild;
			while(pData)
			{
				if(pData->m_pParent == this && !COMPARE_BODYCON_BIT(pData->m_bodyCondition,BODYCON_EXPLOSION_MASK))
				{
					if(pData->m_nType == RC_EFF_GROUND_01 || pData->m_nType == RC_EFF_GROUND_02)
					{
						pData->ChangeBodyCondition(BODYCON_EXPLOSION_MASK);
//						pData->m_bodyCondition = BODYCON_EXPLOSION_MASK;
//						if(pData->m_pCharacterInfo)
//							pData->m_pCharacterInfo->ChangeBodyCondition(pData->m_bodyCondition);
					}
				}
				pData = (CAppEffectData*)pData->m_pNext;
			}
			if(m_bEffectGround == 1)
			{
				D3DXVECTOR3 vTargetPos;
				vTargetPos.x = 0.0f;
				vTargetPos.y = fHeight;
				vTargetPos.z = 0.0f;
				pData = new CAppEffectData(this,RC_EFF_GROUND_01,vTargetPos);
				pData = (CAppEffectData *)g_pD3dApp->m_pEffectList->AddChild(pData);
			}
			else if(m_bEffectGround == 2)
			{
				D3DXVECTOR3 vTargetPos;
				vTargetPos.x = 7.0f;
				vTargetPos.y = -20.0f;
				vTargetPos.z = -3.0f;
				pData = new CAppEffectData(this,RC_EFF_GROUND_02,vTargetPos);
				pData = (CAppEffectData *)g_pD3dApp->m_pEffectList->AddChild(pData);
				vTargetPos.x = -7.0f;
				vTargetPos.y = -20.0f;
				vTargetPos.z = -3.0f;
				pData = new CAppEffectData(this,RC_EFF_GROUND_02,vTargetPos);
				pData = (CAppEffectData *)g_pD3dApp->m_pEffectList->AddChild(pData);
				vTargetPos.x = 0.0f;
				vTargetPos.y = -20;
				vTargetPos.z = 0.0f;
				pData = new CAppEffectData(this,RC_EFF_GROUND_02,vTargetPos);
				pData = (CAppEffectData *)g_pD3dApp->m_pEffectList->AddChild(pData);
			}
		}
	}
}

//void CShuttleChild::AutoControlHeight()
//{// ŔÚµż °íµµ Á¶Ŕý - Enemy,Monster °Ë»ö
//	FLOG( "CShuttleChild::AutoControlHeight()" );
//	float fDist;
//	if(m_pTarget && 
//		m_pTarget->m_dwState == _NORMAL && 
//		m_pTarget->m_dwPartType == _MONSTER)
//	{
//		fDist = m_pTarget->m_vPos.y - m_vPos.y;
//		if(fDist > 10.0f)
//			fDist = 10.0f; // ŔĚµż ĽÓµµ Á¦ÇŃ
//		m_vPos.y += fDist*g_pD3dApp->GetElapsedTime();
//	}
//}

void CShuttleChild::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	FLOG( "CShuttleChild::WndProc()" );
	float fElapsedTime = g_pD3dApp->GetElapsedTime();

	if(!g_pD3dApp->m_bChatMode)// && m_bMoveInputType == 0)	
	{	
		switch(uMsg)
		{
		case WM_KEYDOWN:
			{
				switch( wParam )
				{
				case VK_LEFT:
					/*m_bLeftButtonState = TRUE;
					
					if((g_pTutorial->IsTutorialMode() == FALSE || 
						(g_pTutorial->IsTutorialMode() == TRUE && g_pTutorial->GetLesson() >= L3)) &&
						m_bIsAir == TRUE &&
						m_dwState == _NORMAL)
					{
						if(m_fADoubleKeyTime < DOUBLEKEY_TIME 
							&& m_bKeyDownACheck == FALSE
							// 2007-02-28 by dgwoo şÎ˝şĹÍ »çżë˝Ăżˇ´Â ·Ń¸µŔ» ŔúŔĺÇĎÁö ľĘ´Â´Ů.
							&& m_nBooster != BOOSTER_TYPE_MAX)
					{
						m_bKeyDownADouble = TRUE;
						m_bKeyDownDDouble = FALSE;
					}
					else
					{
						m_fADoubleKeyTime = 0.0f;
					}
					}*/
					ActiveKeyDownADouble();
					break;
				case VK_UP:
					m_bUpButtonState = TRUE;
					break;
				case VK_RIGHT:
					/*m_bRightButtonState = TRUE;

					if((g_pTutorial->IsTutorialMode() == FALSE || 
						(g_pTutorial->IsTutorialMode() == TRUE && g_pTutorial->GetLesson() >= L3)) &&
						m_bIsAir == TRUE &&
						m_dwState == _NORMAL)
					{
						if(m_fDDoubleKeyTime < DOUBLEKEY_TIME 
							&& m_bKeyDownDCheck == FALSE
							// 2007-02-28 by dgwoo şÎ˝şĹÍ »çżë˝Ăżˇ´Â ·Ń¸µŔ» ŔúŔĺÇĎÁö ľĘ´Â´Ů.
							&& m_nBooster != BOOSTER_TYPE_MAX)
					{
						m_bKeyDownDDouble = TRUE;
						m_bKeyDownADouble = FALSE;
					}
					else
					{
						m_fDDoubleKeyTime = 0.0f;
					}
					}*/
					ActiveKeyDownDDouble();
					break;
				case VK_DOWN:
					m_bDownButtonState = TRUE;
					break;
//				case VK_END:
//					{
//						// 2006-05-03 by ispark, ąéşä ¸¶żě˝ş ČŮĹ¬¸Ż°ú RąöĆ°Ŕ¸·Î şŻ°ć
//						// 2005-04-07 by jschoi - Tutorial
//						if(g_pTutorial->IsTutorialMode() == FALSE)
//						{
//							m_bTurnCamera = TRUE;
//							m_bMouseLock  = TRUE;					
//							m_fOldUpRate = 0.0f;							
//						}
//					}
//					break;
				case VK_CONTROL:
					{
						if( IsPkEnableMap(m_myShuttleInfo.MapChannelIndex.MapIndex) )
						{
							SetPKMode( TRUE );
						}
						m_bCtrlKey = TRUE;
					}
					break;
				case VK_SHIFT:
					{
						SetUnLockOn();		// 2005-10-20 by ispark

						if( IsPkEnableMap(m_myShuttleInfo.MapChannelIndex.MapIndex) )
						{
							SetPKMode( FALSE );
						}						
					}
					break;
				}
				switch(lParam)
				{
				case WM_KEYDOWN_LPARAM_E:
					{
						// 2009-02-19 by bhsohn Ä«¸Ţ¶ó ˝ĂĽ± şŻ°ćČÄ, ˝±°Ô »çłÉ ąćąý ¸·Ŕ˝					
						//m_bRButtonState = TRUE;
						if(IsPossibleAttackStat())
						{
							m_bRButtonState = TRUE;
						}						
						// end 2009-02-19 by bhsohn Ä«¸Ţ¶ó ˝ĂĽ± şŻ°ćČÄ, ˝±°Ô »çłÉ ąćąý ¸·Ŕ˝					
						
					}
					break;
				case WM_KEYDOWN_LPARAM_Z:
					{
						if( m_pClientParty->GetNtoNCallState() == TRUE ||
							g_pGameMain->m_pCommunity->GetGuild()->GetNtoNCallState() == TRUE)
						{
							// 2008-10-14 by bhsohn VOIP°łĽ±
//							COPYDATASTRUCT	copyData;
//							memset(&copyData, 0x00, sizeof(COPYDATASTRUCT));
//							AV_MSG_WM_COPYDATA voIP(_NtoNCall, AV_MT_TALK, 0);
//							copyData.dwData = (DWORD)voIP;
//							copyData.lpData = NULL;
//							copyData.cbData = 0;
//							::SendMessage(g_pD3dApp->m_VOIPState.m_hWndAtumVoIPClient, WM_COPYDATA, (WPARAM)g_pD3dApp->GetHwnd(), (LPARAM)&copyData);
							g_pGameMain->ChangeVOIPMode();
						}
					}
					break;
				case WM_KEYDOWN_LPARAM_B:
					{
						// 2005-08-08 by ispark
						// ŔŻ´ÖÁ¤Áö ˝Ă˝şĹŰ
						// °ü¸®ŔÚ, żîżµŔÚ ¸đµĺżˇĽ­¸¸ »çżë
						// // 2005-08-10 by ispark ±×żÜ Á¦żÜ µÇ´Â »óĹÂ (˝şĹł Ć÷ÇÔ)
						// 2005-09-26 by ispark şÎ·Î »óČ˛ Áľ·á
//						if(!COMPARE_RACE(m_myShuttleInfo.Race,RACE_OPERATION|RACE_GAMEMASTER) || 
//							m_bSkillMoveIsUse ||
//							m_dwState == _FALLEN || 
//							m_dwState == _EXPLODED || 
//							m_dwState == _LANDED ||
//							m_dwState == _LANDING || 
//							m_dwState == _TAKINGOFF)
//							break;
						// 2006-10-24 by dgwoo ±âľîŔÇ »óĹÂ°ˇ Á×Ŕş »óČ˛ŔĎ¶§´Â BĹ°´Â ¸·´Â´Ů.
						if(CheckUnitState() == BREAKDOWN)
							break;
					
						if(m_bIsAir == FALSE && IS_DT(m_myShuttleInfo.UnitKind)			//A±âľî°ˇ ¶Ążˇ ŔÖŔ»¶§
							|| g_pD3dApp->m_dwGameState == _SHOP						//»óÁˇżˇ ŔÖŔ»¶§
							|| m_bAttackMode == _SIEGE)									//˝ĂÁî ¸đµĺŔĎ¶§ 
							break;
						// 2005-10-06 by ispark
						// Ć©Ĺä¸®ľóŔĎ¶§´Â Á¤Áö »çżë ľČÇÔ
						// Ćí´ë şńÇŕŔĎ¶§ Ćí´ëŔĺ ŔĚżÜ´Â Á¤˝Ă »çżë ±ÝÁö
						// ±âľî°ˇ LANDEDŔĎ¶§ Á¤Áö »çżë ľČÇÔ
						// ĆĐĹĎ »çżëÁßżˇ´Â Á¤Áö »çżë ľČÇÔ
						if((g_pTutorial->IsTutorialMode() == FALSE || 
							(g_pTutorial->GetLesson() == L2 && g_pTutorial->GetChapter() == L2_2 && g_pTutorial->GetChapterState() == TUTORIAL_STATE_MOTION)) &&
							m_pClientParty->IsFormationFlightMove() == FALSE &&
							m_bLandedMove == FALSE &&
							m_bSkillMoveIsUse == FALSE)
						{
							if(m_bUnitStop == FALSE)	// Á¤Áö˝Ă ¸ŢĽĽÁö¸¦ ą°ľîş»´Ů.
							{
								g_pGameMain->m_pInfWindow->AddMsgBox(STRMSG_C_050809_0001, //"¸ó˝şĹÍŔÇ °ř°ÝŔ» ąŢŔ» Ľö ŔÖ˝Ŕ´Ď´Ů. Á¤Áö ÇĎ˝Ă°Ú˝Ŕ´Ď±î?"
								_Q_UNIT_STOP);
							}
							else if(m_bSkillGoMove)	// Ăâąß ÇŇ ¶§
							{
								// 2008-07-03 by bhsohn EP3 ąö±× ¸®Ć÷Ć® ĽöÁ¤
								if(g_pGameMain->m_pCommunity->IsShowCommunityWindow())
								{
									// Ăâąß˝Ă °­Á¦·Î Ă˘´ÝŔ˝
									g_pGameMain->m_pCommunity->ShowCommunityWindow(FALSE);
								}
								if(g_pGameMain->m_pSystem && g_pGameMain->m_pSystem->IsOptionShow())
								{
									g_pGameMain->PopupSystemOptinWnd();
								}
								// end 2008-07-03 by bhsohn EP3 ąö±× ¸®Ć÷Ć® ĽöÁ¤
								
								m_bUnitStop = FALSE;
								m_bPenaltyUnitStop = TRUE;
								m_bMouseMoveLock = FALSE;
								m_nBooster = BOOSTER_TYPE_MIN;
								g_pInterface->m_pGameMain->InitShowWindow();
								// 2010. 03. 30 by jskim ŔŻ´Ö »óĹÂ ąŮµđÄÁµđĽÇ Ľ­ąö Ŕü´Ţ
								SendFieldSocketChangeBodyCondition( m_myShuttleInfo.ClientIndex, BODYCON_SET_OR_CLEAR_MASK );
								//end 2010. 03. 30 by jskim ŔŻ´Ö »óĹÂ ąŮµđÄÁµđĽÇ Ľ­ąö Ŕü´Ţ	
							}						
						}
					}
					break;
				case WM_KEYDOWN_LPARAM_A:
					{
						// 2005-10-06 by ispark
						// 1. Ć©Ĺä¸®ľóŔĚ ľĆ´Ď´Ů
						// 2. Ć©Ĺä¸®ľóŔĚ¸éĽ­ ·ą˝Ľ5 ŔĚ´Ů
						// 3. şńÇŕÁßŔĚ´Ů
						// 4. şńÇŕ»óĹÂ°ˇ _NORMAL
						// »çżëÇŃ´Ů.
						/*if((g_pTutorial->IsTutorialMode() == FALSE || 
							(g_pTutorial->IsTutorialMode() == TRUE && g_pTutorial->GetLesson() >= L3)) &&
							m_bIsAir == TRUE &&
							m_dwState == _NORMAL)
						{
//							DBGOUT("A Push %f\n", m_fADoubleKeyTime);
//							m_bKeyDownAPush = TRUE;
							if(m_fADoubleKeyTime < DOUBLEKEY_TIME 
								&& m_bKeyDownACheck == FALSE 
								// 2007-02-28 by dgwoo şÎ˝şĹÍ »çżë˝Ăżˇ´Â ·Ń¸µŔ» ŔúŔĺÇĎÁö ľĘ´Â´Ů.
								&& m_nBooster != BOOSTER_TYPE_MAX)
							{
								m_bKeyDownADouble = TRUE;
								m_bKeyDownDDouble = FALSE;
							}
							else
							{
								m_fADoubleKeyTime = 0.0f;
							}
						}*/
						ActiveKeyDownADouble();
					}
					break;
				case WM_KEYDOWN_LPARAM_D:
					{
						/*if((g_pTutorial->IsTutorialMode() == FALSE || 
							(g_pTutorial->IsTutorialMode() == TRUE && g_pTutorial->GetLesson() >= L3)) &&
							m_bIsAir == TRUE &&
							m_dwState == _NORMAL)
						{
//							DBGOUT("D Push %f\n", m_fDDoubleKeyTime);
//							m_bKeyDownDPush = TRUE;
							if(m_fDDoubleKeyTime < DOUBLEKEY_TIME 
								&& m_bKeyDownDCheck == FALSE
								// 2007-02-28 by dgwoo şÎ˝şĹÍ »çżë˝Ăżˇ´Â ·Ń¸µŔ» ŔúŔĺÇĎÁö ľĘ´Â´Ů.
								&& m_nBooster != BOOSTER_TYPE_MAX)
							{
								m_bKeyDownDDouble = TRUE;
								m_bKeyDownADouble = FALSE;
							}
							else
							{
								m_fDDoubleKeyTime = 0.0f;
							}
						}*/
						ActiveKeyDownDDouble();
					}
					break;
				case WM_KEYDOWN_LPARAM_R:
					{
						// 2006-05-03 by ispark, ąéşä
						SetBackView(TRUE);
					}
					break;
				// 2008-10-23 by bhsohn ŔÚ±â ŔÚ˝ĹÇŃĹ× Čú ´ÜĂŕĹ° Ăß°ˇ
				case WM_KEYDOWN_LPARAM_FLOW:
					{
						if(	m_pSkill && m_pSkill->GetSkillTargetState() == TRUE )
						{
							m_pSkill->FindTargetForSkill_ToMe();
						}
					}
					break;
				// end 2008-10-23 by bhsohn ŔÚ±â ŔÚ˝ĹÇŃĹ× Čú ´ÜĂŕĹ° Ăß°ˇ
					// 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
				case WM_KEYDOWN_LPARAM_KEYPAD_PLUS:
					{
						if(!g_pD3dApp->IsUseJoyStick())
						{
							return;
						}
						g_pJoysticOp->nJoysticSense++;
						if(g_pJoysticOp->nJoysticSense >= MAX_JOYSTICK_SENSITY)
						{
							g_pJoysticOp->nJoysticSense = MAX_JOYSTICK_SENSITY-1;
						}
						else
						{
							char buf[256];
							// 2008-12-23 by bhsohn Á¶ŔĚ˝şĆ˝ Ăß°ˇ żäĂ» Ăł¸®
							wsprintf(buf, STRMSG_C_081120_0200, g_pJoysticOp->nJoysticSense-MAX_JOYSTICK_DEFAULTSENSITY );
							g_pD3dApp->m_pChat->CreateChatChild(buf, COLOR_SYSTEM);
						}
					}
					break;
				case WM_KEYDOWN_LPARAM_KEYPAD_MINUS:
					{
						if(!g_pD3dApp->IsUseJoyStick())
						{
							return;
						}

						g_pJoysticOp->nJoysticSense--;
						if(g_pJoysticOp->nJoysticSense < 0)
						{
							g_pJoysticOp->nJoysticSense = 0;
						}
						else
						{
							char buf[256];
							wsprintf(buf, STRMSG_C_081120_0201, g_pJoysticOp->nJoysticSense-MAX_JOYSTICK_DEFAULTSENSITY );
							g_pD3dApp->m_pChat->CreateChatChild(buf, COLOR_SYSTEM);
						}
						
					}
					break;
					// end 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
					
				}	
			}
			break;
		case WM_KEYUP:
			{
				switch( wParam )
				{
					case VK_LEFT:
						m_bLeftButtonState = FALSE;
						break;
					case VK_UP:
						m_bUpButtonState = FALSE;
						break;
					case VK_RIGHT:
								m_bRightButtonState = FALSE;
						break;
					case VK_DOWN:
						m_bDownButtonState = FALSE;
						break;
	//				case VK_END:
	//					{
	//						// 2006-05-03 by ispark, ąéşä ¸¶żě˝ş ČŮĹ¬¸Ż°ú RąöĆ°Ŕ¸·Î şŻ°ć
	//						// 2005-04-07 by jschoi - Tutorial
	//						if(g_pTutorial->IsTutorialMode() == FALSE)
	//						{
	//							m_bTurnCamera = FALSE;
	////							g_pCamera->SetCamType(CAMERA_TYPE_NORMAL);
	//							//m_bCamType = CAMERA_TYPE_NORMAL;
	//							m_bMouseLock  = FALSE;
	//							m_fOldUpRate = 0.0f;
	//						}
	//					}
	//					break;
					case VK_CONTROL:
						{
							// 2005-10-20 by ispark
	//						if( IsPkEnableMap(m_myShuttleInfo.MapChannelIndex.MapIndex) )
	//						{
	//							SetPKMode( FALSE );
	//						}
							
							m_bCtrlKey = FALSE;
						}
						break;
				}

				switch(lParam)
				{
					case WM_KEYUP_LPARAM_E:
						{
							m_bRButtonState = FALSE;
							m_SecondaryAttack.AttackData.TargetInfo.SetNullTarget();
	//						if(m_pSecondaryWeapon)
	//						{
	//							m_pSecondaryWeapon->SetUseToggleWeapon(TRUE);
	//						}
						}
						break;
					case WM_KEYUP_LPARAM_Z:
						{
							// 2008-10-14 by bhsohn VOIP°łĽ±							
	//						if( m_pClientParty->GetNtoNCallState() == TRUE ||
	//							g_pGameMain->m_pCommunity->GetGuild()->GetNtoNCallState() == TRUE)
	//						{
	//							COPYDATASTRUCT	copyData;
	//							memset(&copyData, 0x00, sizeof(COPYDATASTRUCT));
	//							AV_MSG_WM_COPYDATA voIP(_NtoNCall, AV_MT_TALK_CANCEL, 0);
	//							copyData.dwData = (DWORD)voIP;
	//							copyData.lpData = NULL;
	//							copyData.cbData = 0;
	//							::SendMessage(g_pD3dApp->m_VOIPState.m_hWndAtumVoIPClient, WM_COPYDATA, (WPARAM)g_pD3dApp->GetHwnd(), (LPARAM)&copyData);
	//						}
							// end 2008-10-14 by bhsohn VOIP°łĽ±							
						}
						break;
					case WM_KEYUP_LPARAM_R:
						{
							// 2006-05-03 by ispark, ąéşä
							SetBackView(FALSE);
						}
						break;
				}
			}
			break;
/*		case WM_MBUTTONDBLCLK:
			{
				m_bTurnCamera = !m_bTurnCamera;
			}
			break;
*/		case WM_MBUTTONDOWN:
			{
				// 2010. 05. 27 by jskim ˝Ăł×¸¶ Ŕűżë Ä«¸Ţ¶ó ±¸Çö
				if( IsShowCinema() )
				{
					break;
				}
				//end 2010. 05. 27 by jskim ˝Ăł×¸¶ Ŕűżë Ä«¸Ţ¶ó ±¸Çö
				m_bMButtonState = TRUE;
				// 2005-08-08 by ispark
				// MąöĆ°Ŕ» ¸·´Â´Ů. ±×Ŕüżˇ ŔÖľîĽ­ ·Ń¸µ˝Ă MąöĆ° »çżë±ÝÁöµµ Ăß°ˇ
//				if(m_bRollUsed == FALSE)
//				{
//					SetShuttleFlight();
//				}
// 2005-08-03 by ispark
// »çżë ±ÝÁö
//#ifdef _DEBUG
//				DeletePattern();
//				CObjectChild * pCurrentObjEvent = g_pScene->FindEventObjectByTypeAndPosition(EVENT_TYPE_OBJ_ENTER_BUILDING, m_vPos, 7000.0f);				
//				if(pCurrentObjEvent)
//				{
//					CObjectChild* pObj = g_pScene->FindObjectByIndex(pCurrentObjEvent->m_sEventIndexFrom);					
//					if(pObj)
//					{
//						// Ŕ§Äˇ ŔĚµż
//						m_vPos = pObj->m_vPos;
//						g_pCharacterChild->m_vPos = pObj->m_vPos;
//						
//						// ¸ŢĽĽÁö Ăł¸®
//						INIT_MSG_WITH_BUFFER(MSG_FC_CHARACTER_CHANGE_CHARACTER_MODE,
//							T_FC_CHARACTER_CHANGE_CHARACTER_MODE, pSMode, SendBuf);
//						pSMode->CharacterMode0 = TRUE;
//						pSMode->PositionAVec3 = m_vPos;
//						pSMode->TargetAVec3 = m_vVel*1000.0f;
//						g_pFieldWinSocket->Write(SendBuf, MSG_SIZE(MSG_FC_CHARACTER_CHANGE_CHARACTER_MODE));
//						
//						// Äł¸ŻĹÍ ¸đµĺ şŻČŻ
//						g_pD3dApp->m_bCharacter = TRUE;
//						g_pCharacterChild->InitCharacterData();			  
//					}
//				}
//#endif
				// 2006-05-03 by ispark, ąéşä
				SetBackView(TRUE);
			}
			break;
		case WM_MBUTTONUP:
			{
				m_bMButtonState = FALSE;
				// 2006-05-03 by ispark, ąéşä
				SetBackView(FALSE);
			}
			break;
		case WM_MOUSEWHEEL:
			{
//				if( m_dwState != _NORMAL ||
//					g_pGameMain->IsShowing() ||
//					m_bAttackMode == _SIEGE ||
//					m_bMouseLock == TRUE ||
//					m_nBooster == BOOSTER_TYPE_AIRBREAK ||
//					g_pTutorial->IsTutorialMode() == TRUE )
//				{
//					break;
//				}
//				if( (int)wParam > 0 )
//				{
//					m_fShuttleSpeed += 10;
//					if(m_fShuttleSpeed > m_fRealShuttleSpeedMax)
//						m_fShuttleSpeed = m_fRealShuttleSpeedMax;
//				}
//				else
//				{
//					m_fShuttleSpeed -= 10;
//					if(m_fShuttleSpeed < m_fRealShuttleSpeedMin)
//						m_fShuttleSpeed = m_fRealShuttleSpeedMin;
//				}
			}
			break;
		case WM_LBUTTONDOWN:
			{
				// 2010. 05. 27 by jskim ˝Ăł×¸¶ Ŕűżë Ä«¸Ţ¶ó ±¸Çö
				if( IsShowCinema() )
				{
					break;
				}
				//end 2010. 05. 27 by jskim ˝Ăł×¸¶ Ŕűżë Ä«¸Ţ¶ó ±¸Çö
				//	ł»°ˇ Áö±Ý Ĺ¸°Ů ˝şĹłŔ» »çżëÇĎ±â Ŕ§ÇŘ Ĺ¸°ŮŔ» ĂŁ´Â ÁßŔĚ¶ó¸é..
				//	ĂŃľËŔĚ ąß»çµÇÁö ľĘ°í, ´ë˝Ĺżˇ ˝şĹł Ĺ¬·ˇ˝şżˇ ŔÖ´Â Ĺ¸°ŮŔ» ĂŁ´Â ÇÔĽö¸¦ ČŁĂâÇŃ´Ů.
				if(	m_pSkill->GetSkillTargetState() == TRUE )
				{
					m_pSkill->FindTargetForSkill();
				}
				else
				{
					// 2009-02-19 by bhsohn Ä«¸Ţ¶ó ˝ĂĽ± şŻ°ćČÄ, ˝±°Ô »çłÉ ąćąý ¸·Ŕ˝
					//if(IS_WEAPON_ENABLESTATE(m_dwState, m_nBooster))
					if(IS_WEAPON_ENABLESTATE(m_dwState, m_nBooster) && IsPossibleAttackStat())					
					{
						m_bLButtonState = TRUE;
					}				
				}
				//Ăąąß»ç°ˇ ľČş¸ŔĚ´Â °ćżě°ˇ »ý°Ü ŔĚ ÄÚµĺ¸¦ Ăß°ˇ
	//			m_pPrimaryAttackTarget = NULL;
			}
			break;
		case WM_LBUTTONUP:
			{
				m_bLButtonState = FALSE;
				m_PrimaryAttack.AttackData.TargetInfo.SetNullTarget();
				m_bTargetChange = TRUE;

			}
			break;
		case WM_RBUTTONDOWN:
			{
				// 2010. 05. 27 by jskim ˝Ăł×¸¶ Ŕűżë Ä«¸Ţ¶ó ±¸Çö
				if( IsShowCinema() )
				{
					break;
				}
				//end 2010. 05. 27 by jskim ˝Ăł×¸¶ Ŕűżë Ä«¸Ţ¶ó ±¸Çö
				if( m_dwState == _FALLEN || 
					m_dwState == _EXPLODED || 
					m_dwState == _LANDED || 
//					m_nBooster == BOOSTER_TYPE_AIRBREAK || 
//					m_bFormationFlight || 
					m_bMouseLock)
				{// Ä«¸Ţ¶ó Á¶Ŕý ¸đµĺ·Î ŔüČŻ
					if(g_pCamera->GetCamType() == CAMERA_TYPE_NORMAL && m_bAttackMode != _SIEGE)
					{
						g_pCamera->m_bIsCamControl = TRUE;
						POINT pt;
						pt.x = LOWORD(lParam);
						pt.y = HIWORD(lParam);
						CheckMouseReverse(&pt);
	
						m_pOldMousePoint = pt;
					}
				}
				// 2009-02-19 by bhsohn Ä«¸Ţ¶ó ˝ĂĽ± şŻ°ćČÄ, ˝±°Ô »çłÉ ąćąý ¸·Ŕ˝					
				//m_bRButtonState = TRUE;
				if(IsPossibleAttackStat())
				{
					m_bRButtonState = TRUE;
				}
				// end 2009-02-19 by bhsohn Ä«¸Ţ¶ó ˝ĂĽ± şŻ°ćČÄ, ˝±°Ô »çłÉ ąćąý ¸·Ŕ˝					
				
//				if(IS_WEAPON_ENABLESTATE(m_dwState, m_nBooster))
//				{
//					m_bRButtonState = TRUE;
//				}
			}
			break;
		case WM_RBUTTONDBLCLK:
		case WM_LBUTTONDBLCLK:
			{
				if(IS_WEAPON_ENABLESTATE(m_dwState, m_nBooster) && m_myShuttleInfo.Level < 6)
				{
//					g_pGameMain->m_pNotice->SetNotice( "°ř°ÝŔş ¸¶żě˝ş ÁÂ/żě ąöĆ°Ŕ» ´©¸Ł°í ŔÖŔ¸¸é ŔÚµżŔ¸·Î µË´Ď´Ů.");
					g_pGameMain->SetNotice( 14 );
				}
			}
			break;
		case WM_RBUTTONUP:
			{
				// Ä«¸Ţ¶ó ŔüČŻ ¸đµĺ ÇŘÁ¦
				if(g_pD3dApp->m_pCamera->m_bIsCamControl)
				{
					g_pD3dApp->m_pCamera->m_bIsCamControl = FALSE;
				}
				m_bRButtonState = FALSE;
				m_SecondaryAttack.AttackData.TargetInfo.SetNullTarget();
//				if(m_pSecondaryWeapon)
//				{
//					m_pSecondaryWeapon->SetUseToggleWeapon(TRUE);
//				}
			}
			break;
		}
	}
	// 2008-09-02 by dgwoo MouseMoveŔĚşĄĆ®´Â Ă¤ĆĂ ŔÔ·ÂÁßżˇµµ ŔĚşĄĆ® Ăł¸®.
	switch(uMsg)
	{
		case WM_MOUSEMOVE:
			{
				POINT pt;
				pt.x = LOWORD(lParam);
				pt.y = HIWORD(lParam);
				CheckMouseReverse(&pt);

				if((m_dwState == _FALLEN || 
					m_dwState == _EXPLODED || 
					m_dwState == _LANDED || 
					m_pClientParty->IsFormationFlightMove() || 
					m_bMouseLock) &&
					m_bPenaltyUnitStop == FALSE)				// ĆäłÎĆĽ »óĹÂ°ˇ ľĆ´Ň¶§ żňÁ÷ŔÓ Çăżë					
//					((IS_OT(m_myShuttleInfo.UnitKind) || IS_DT(m_myShuttleInfo.UnitKind)) && m_bMouseLock))					
				{// Ä«¸Ţ¶ó Á¶Ŕý ¸đµĺ·Î ŔüČŻ
					if(g_pCamera->GetCamType() == CAMERA_TYPE_NORMAL)
					{
						if( g_pCamera->GetCamType() == CAMERA_TYPE_NORMAL &&
							g_pD3dApp->m_pCamera->m_bIsCamControl && 
							m_bRButtonState)
						{
							// 2005-08-19 by ispark
							if(m_pOldMousePoint.y - pt.y != 0)
								g_pCamera->SetControlCamAngle(pt.y - m_pOldMousePoint.y);
							if(m_pOldMousePoint.x - pt.x != 0)
								g_pCamera->SetControlCamSideAngle(pt.x - m_pOldMousePoint.x);
						}
						m_pOldMousePoint = pt;	
					}
				}
				m_pOldMousePoint = pt;
			}
			break;
		}
}

void CShuttleChild::SetShuttleData(BYTE type)
{
	FLOG( "CShuttleChild::SetShuttleData(BYTE type)" );
//	map<int, ITEM_GENERAL*>::iterator it = NULL;
	switch(type)
	{
	case 0:
		{
			m_fDefenseTotal = 0.0f;
			m_fDefenseForItem = 0.0f;
			m_fDefenseForSkill = 0.0f;
			m_fDefenseForShuttle = 0.0f;
			m_fTransportTotal = 0.0f;
			m_fTransportCurrent = 0.0f;

			// 2007-07-27 by dgwoo ĽĽ·Â Ľ±ĹĂŔ» ÇŃ ŔŻŔúŔÎ°ˇ?
			if(FALSE == IS_NORMAL_INFLUENCE_TYPE(m_myShuttleInfo.InfluenceType))
			{
				g_pD3dApp->m_bInfregular = TRUE;
			}
			else
			{
				g_pD3dApp->m_bInfregular = FALSE;
			}

			// ąćľî·Â Ŕűżë
			// Âřżë ľĆŔĚĹŰżˇ µű¸Ą ąćľî·Â
			for(int i=0;i<POS_INVALID_POSITION;i++)
			{
				CItemInfo* pItemInfo = m_pStoreData->FindItemInInventoryByWindowPos(i);
				if( pItemInfo && 
					pItemInfo->Kind == ITEMKIND_DEFENSE &&
					pItemInfo->Wear == WEAR_ATTACHED)
				{
					ITEM* pITEM = pItemInfo->GetRealItemInfo();
					m_fDefenseForItem += pITEM->AbilityMax;
					m_fTransportCurrent += pITEM->Weight;
				}
			}
//			it = m_pStoreData->m_mapItemInInventory.begin();
//			while(it != m_pStoreData->m_mapItemInInventory.end())
//			{
//				ITEM *item = g_pDatabase->GetServerItemInfo(it->second->ItemNum);
//				if(item)
//				{
//					if(it->second->Kind == ITEMKIND_DEFENSE && it->second->Wear == WEAR_ATTACHED)
//					{
//						m_fDefenseForItem += item->AbilityMax;
//					}
//					// ÇöŔç ±âĂĽżˇ ˝Ç·Á ŔÖ´Â ĂŃÁß·®->EnergyŔÇ °ćżě ą«°Ô Ŕűżë °í·ÁÇŘľßÇÔ
//					m_fTransportCurrent += item->Weight;
//				}
//				it++;
//			}
			// »çżëÁßŔÎ(ŔűżëÁß) ¸¶ŔÎµĺ ÄÁĆ®·Ńżˇ µű¸Ą ąćľî·Â

			// ±âĂĽŔÇ ´É·Âżˇ µű¸Ą ąćľî·Â
			m_fDefenseForShuttle = g_pAtumSJ->GetDefenseC(&m_myShuttleInfo, &m_paramFactor, TRUE);
			// ĂŃąćľî·Â Ŕűżë
			m_fDefenseTotal = m_fDefenseForShuttle + m_fDefenseForItem + m_fDefenseForSkill;
			
			// Áß·® Ŕűżë
			// ±âĂĽŔÇ ´É·Âżˇ µű¸Ą ĂŃ°ˇ´ÉÁß·®
			m_fTransportTotal = g_pAtumSJ->GetTransport(&m_myShuttleInfo);
		}
		break;
	case 1:
		{// ÂřżëÇŃ ľĆŔĚĹŰżˇ µű¸Ą ąćľî·Â Ŕűżë
			m_fDefenseTotal -= m_fDefenseForItem;
			m_fDefenseForItem = 0.0f;
			for(int i=0;i<POS_INVALID_POSITION;i++)
			{
				CItemInfo* pItemInfo = m_pStoreData->FindItemInInventoryByWindowPos(i);
				if( pItemInfo && 
					pItemInfo->Kind == ITEMKIND_DEFENSE &&
					pItemInfo->Wear == WEAR_ATTACHED)
				{
					ITEM* pITEM = pItemInfo->GetRealItemInfo();
					m_fDefenseForItem += pITEM->AbilityMax;
				}
			}
//			it = m_pStoreData->m_mapItemInInventory.begin();
//			while(it != m_pStoreData->m_mapItemInInventory.end())
//			{
//				ITEM *item = g_pDatabase->GetServerItemInfo(it->second->ItemNum);
//				if(item)
//				{
//					if(it->second->Kind == ITEMKIND_DEFENSE && it->second->Wear == WEAR_ATTACHED)
//					{
//						m_fDefenseForItem += item->AbilityMax;
//					}
//				}
//				it++;
//			}
			m_fDefenseTotal += m_fDefenseForItem;
		}
		break;
	case 2:
		{// »çżëÁßŔÎ(ŔűżëÁß) ¸¶ŔÎµĺ ÄÁĆ®·Ńżˇ µű¸Ą ąćľî·Â Ŕűżë
			m_fDefenseTotal -= m_fDefenseForSkill;
			m_fDefenseTotal += m_fDefenseForSkill;
		}
		break;
	case 3:
		{// ±âĂĽ ´É·Âżˇ µű¸Ą ąćľî·Â Ŕűżë
			m_fDefenseTotal -= m_fDefenseForShuttle;
//			m_fDefenseForShuttle = g_pAtumSJ->GetMaxDefense(&m_myShuttleInfo);
			m_fDefenseForShuttle = g_pAtumSJ->GetDefenseC(&m_myShuttleInfo, &m_paramFactor, TRUE);
			m_fDefenseTotal += m_fDefenseForShuttle;
		}
		break;
	case 4:
		{// ĂŃ°ˇ´ÉÁß·® Ŕűżë
			m_fTransportTotal = g_pAtumSJ->GetTransport(&m_myShuttleInfo);
		}
		break;
	case 5:
		{// ÇöŔç ĽŇŔŻÇŃ ľĆŔĚĹŰ ĂŃÁß·®
			m_fTransportCurrent = 0.0f;
			m_fTransportCurrent = m_pStoreData->GetTotalWeight();
//			it = m_pStoreData->m_mapItemInInventory.begin();
//			while(it != m_pStoreData->m_mapItemInInventory.end())
//			{
//				ITEM *item = g_pDatabase->GetServerItemInfo(it->second->ItemNum);
//				if(item)
//				{
//					// ÇöŔç ±âĂĽżˇ ˝Ç·Á ŔÖ´Â ĂŃÁß·®->EnergyŔÇ °ćżě ą«°Ô Ŕűżë °í·ÁÇŘľßÇÔ
//					m_fTransportCurrent += item->Weight;
//				}
//				it++;
//			}
		}
		break;
	}
}

void CShuttleChild::CheckEventType()
{
	FLOG( "CShuttleChild::CheckEventType()" );
	// ŔĚşĄĆ® ÁöżŞ ĂĽĹ© - Shop
//	if(m_bPKMode)
//		return;
	if(IsPkState(PK_PVP) || IsPkState(PK_FVF))	// 1:1°áĹő°ĹłŞ Ćí´ëŔüŔÎ °ćżěżˇ´Â ŔĚşĄĆ®¸¦ ąŢÁö ľĘ´Â´Ů.
		return;
	int nCheckX = ((int)m_vPos.x)/TILE_SIZE;
	int nCheckZ = ((int)m_vPos.z)/TILE_SIZE;
	
	if(nCheckX < 0 || nCheckZ < 0)
		return;

	if(m_myShuttleInfo.Level < 6)
	{
		if(!g_pGround->m_pTileInfo[m_ptOldPoint.x*g_pGround->m_projectInfo.sYSize + m_ptOldPoint.y].bEnableLand &&
			g_pGround->m_pTileInfo[nCheckX*g_pGround->m_projectInfo.sYSize + nCheckZ].bEnableLand &&
			m_fLandingDist < 180.0f)
		{
//			g_pGameMain->m_pNotice->SetNotice("ÇöŔç Âř·úŔĺ »ó°řŔÔ´Ď´Ů. Âř·úĹ°´Â 'C'Ĺ°ŔÔ´Ď´Ů.");
			g_pGameMain->SetNotice( 15 );
		}
	}

	if((g_pGround->m_pTileInfo[m_ptOldPoint.x*g_pGround->m_projectInfo.sYSize + m_ptOldPoint.y].bEventType 
		!= g_pGround->m_pTileInfo[nCheckX*g_pGround->m_projectInfo.sYSize + nCheckZ].bEventType)
		&& m_vPos.y < g_pGround->m_projectInfo.fHeightMax - 20.0f)
	{
		switch(g_pGround->m_pTileInfo[nCheckX*g_pGround->m_projectInfo.sYSize + nCheckZ].bEventType)
		{
		case EVENT_TYPE_WARP:
			{
				MSG_FC_MOVE sMsg;
				memset(&sMsg,0x00,sizeof(sMsg));
				char buffer[SIZE_MAX_PACKET];
				sMsg.ClientIndex = m_myShuttleInfo.ClientIndex;
				sMsg.TimeGap = g_pD3dApp->m_dwSpeedHackTimer;
				sMsg.PositionVector = m_vPos;
				sMsg.TargetVector = m_vVel*1000.0f;
				D3DXVECTOR3 vUp;
				vUp.x = m_mMatrix._21;
				vUp.y = m_mMatrix._22;
				vUp.z = m_mMatrix._23;
				sMsg.UpVector = vUp*1000.0f;
				int nType = T_FC_MOVE;
				memcpy(buffer, &nType, SIZE_FIELD_TYPE_HEADER);
				memcpy(buffer+SIZE_FIELD_TYPE_HEADER, &sMsg, sizeof(sMsg));
				g_pD3dApp->m_pFieldWinSocket->Write(buffer, SIZE_FIELD_TYPE_HEADER + sizeof(sMsg));
				g_pD3dApp->m_vClientOldPos = m_vPos;
				g_pD3dApp->m_dwSpeedHackTimer  = 0;
				
				if(m_pClientParty->GetPartyInfo().bPartyType == _PARTYMASTER)
				{
					m_pClientParty->FSendPartyRequestPartyWarp();
				}
				else
				{
					// żöÇÁ¸¦ żäĂ»
					if(!m_pClientParty->IsFormationFlightMove())
					{
						MSG_FC_EVENT_REQUEST_WARP sMsg2;
						memset(&sMsg2,0x00,sizeof(sMsg2));
						memset(buffer,0x00,SIZE_MAX_PACKET);
						sMsg2.ClientIndex = m_myShuttleInfo.ClientIndex;
						nType = T_FC_EVENT_REQUEST_WARP;
						memcpy(buffer, &nType, SIZE_FIELD_TYPE_HEADER);
						memcpy(buffer+SIZE_FIELD_TYPE_HEADER, &sMsg2, sizeof(sMsg2));
						g_pD3dApp->m_pFieldWinSocket->Write(buffer, SIZE_FIELD_TYPE_HEADER + sizeof(sMsg2));
						DBGOUT("Request Warp : MSG_FC_EVENT_REQUEST_WARP\n");
					}
				}				
			}
			break;
		case EVENT_TYPE_WARP_TARGET:
			{// Ĺ¬¶óŔĚľđĆ® ÇöŔç »çżëľČÇÔ
			}
			break;
		case EVENT_TYPE_ENTER_BUILDING:
			{// »óÁˇ Object
				//SendPacket(T_FC_MOVE_LANDING_DONE);
//				m_bEventReady = TRUE;
			}
			break;
		case EVENT_TYPE_LANDING:
			{// Âř·ú Object
			}
			break;
		case EVENT_TYPE_RACING_CHECK_POTINT:
			{// ·ąŔĚ˝Ě ĂĽĹ© Ć÷ŔÎĆ®
			}
			break;
		case EVENT_TYPE_OBJECT_MONSTER:
			{// żŔşęÁ§Ć® ¸ó˝şĹÍ - ľĆą«°Íµµ ľČÇÔ.
			}
			break;
		case EVENT_TYPE_PATTERN_POINT:
			{// Äł¸ŻĹÍ żöÇÁ˝Ă ĆĐĹĎŔÇ ˝ĂŔŰ/łˇ
			}
			break;
		case EVENT_TYPE_LANDING_PATTERN_START:
			{// ĆĐĹĎĹ¸ŔÔ »óÁˇ Âř·ú ˝ĂŔŰ 
			}
			break;
		case EVENT_TYPE_LANDING_PATTERN_END:
			{// ĆĐĹĎĹ¸ŔÔ »óÁˇ Âř·ú łˇ
			}
			break;
		case EVENT_TYPE_TAKEOFF_PATTERN_START:
			{// ĆĐĹĎĹ¸ŔÔ »óÁˇ ŔĚ·ú ˝ĂŔŰ
			}
			break;
		case EVENT_TYPE_TAKEOFF_PATTERN_END:
			{// ĆĐĹĎĹ¸ŔÔ »óÁˇ ŔĚ·ú łˇ
			}
			break;
		}
	}
}
BOOL CShuttleChild::ISSetAutoPos()
{
	FLOG( "CShuttleChild::ISSetAutoPos()" );
	switch(m_myShuttleInfo.UnitKind)
	{
	case UNITKIND_BT01:
	case UNITKIND_BT02:
	case UNITKIND_BT03:
	case UNITKIND_BT04:
		{
			if(!m_nSideRotateDir && 
				(m_nFBDir && m_nBooster == BOOSTER_TYPE_MIN) && 
				!m_nLRDir)
				return TRUE;
		}
		break;
	case UNITKIND_OT01:
	case UNITKIND_OT02:
	case UNITKIND_OT03:
	case UNITKIND_OT04:
		{
			if(!m_nSideRotateDir && !m_nFBDir && !m_nLRDir)
				return TRUE;
		}
		break;
	case UNITKIND_DT01:
	case UNITKIND_DT02:
	case UNITKIND_DT03:
	case UNITKIND_DT04:
		{
			if(!m_nSideRotateDir && !m_nFBDir && !m_nLRDir)
				return TRUE;
		}
		break;
	case UNITKIND_ST01:
	case UNITKIND_ST02:
	case UNITKIND_ST03:
	case UNITKIND_ST04:
		{
			if(!m_nSideRotateDir && 
				(m_nFBDir && m_nBooster == BOOSTER_TYPE_MIN) && 
				!m_nLRDir)
				return TRUE;
		}
		break;
	}
	return FALSE;
}

void CShuttleChild::SetWeaponData(BYTE bType,BOOL bReset)//,BOOL bWear)
{
	FLOG( "CShuttleChild::SetWeaponData(BYTE bType,BOOL bReset)" );
	byte bWeaponType = 0;
	D3DXVECTOR3 vTempPos = D3DXVECTOR3(0,0,0);
	InitInvenMesh();					// 2006-04-04 by ispark
	if(bType == 0) // 1Çü
	{
		if(bReset)
		{
			DeleteWearItem( WEAR_ITEM_KIND_WEAPON_11 );
			DeleteWearItem( WEAR_ITEM_KIND_WEAPON_12 );
			
			// 2009. 07. 07 by ckPark ·Îşż±âľî żäĂ»»çÇ×(·Ń¸µ, Ľ±ĹĂČ­¸é, ą«±â, A±âľîĆ÷´ë)
			// A±âľî Ć÷´ëŔĚĆĺĆ®µµ Áöżî´Ů
			DeleteRobotAGearWeapon();
			// end 2009. 07. 07 by ckPark ·Îşż±âľî żäĂ»»çÇ×(·Ń¸µ, Ľ±ĹĂČ­¸é, ą«±â, A±âľîĆ÷´ë)

			if(g_pGameMain)
			{
				g_pGameMain->m_pWeaponInfo->ChangeWeapon(1);
			}
			if(m_pPrimaryWeapon == NULL)
			{
				// 2009. 08. 27 by ckPark ±×·ˇÇČ ¸®ĽŇ˝ş şŻ°ć ˝Ă˝şĹŰ ±¸Çö
				//CreatePrimaryWeaponItem(NULL);
				CreatePrimaryWeaponItem( NULL, NULL );
				// end 2009. 08. 27 by ckPark ±×·ˇÇČ ¸®ĽŇ˝ş şŻ°ć ˝Ă˝şĹŰ ±¸Çö
			}
			else
			{

				// 2009. 08. 27 by ckPark ±×·ˇÇČ ¸®ĽŇ˝ş şŻ°ć ˝Ă˝şĹŰ ±¸Çö
				//CreatePrimaryWeaponItem(m_pPrimaryWeapon->GetRealItemInfo());

				ITEM*	pShapeItem	= g_pDatabase->GetServerItemInfo( m_pPrimaryWeapon->GetItemGeneral()->GetShapeItemNum() ) ;
				CreatePrimaryWeaponItem( m_pPrimaryWeapon->GetRealItemInfo(), pShapeItem );

				// end 2009. 08. 27 by ckPark ±×·ˇÇČ ¸®ĽŇ˝ş şŻ°ć ˝Ă˝şĹŰ ±¸Çö

				if(IS_PRIMARY_WEAPON_1(m_pPrimaryWeapon->GetRealItemInfo()->Kind))
				{
					if(m_pPrimaryWeapon->GetItemGeneral()->CurrentCount <= 0)
					{
						ResetPrimaryWeaponBodyCondition(WEAPON_BODYCON_RELOAD);
					}
				}
				else if(IS_PRIMARY_WEAPON_2(m_pPrimaryWeapon->GetRealItemInfo()->Kind))
				{
					if(m_myShuttleInfo.CurrentEP <= 0)
					{
						ResetPrimaryWeaponBodyCondition(WEAPON_BODYCON_RELOAD);
					}
				}
			}
		}
		else
		{
			if(m_pPrimaryWeapon)
			{
				if(IS_PRIMARY_WEAPON_1(m_pPrimaryWeapon->GetRealItemInfo()->Kind))
				{
					if(m_pPrimaryWeapon->GetItemGeneral()->CurrentCount <= 0)
					{
						ResetPrimaryWeaponBodyCondition(WEAPON_BODYCON_RELOAD);
					}
				}
				else if(IS_PRIMARY_WEAPON_2(m_pPrimaryWeapon->GetRealItemInfo()->Kind))
				{
					if(m_myShuttleInfo.CurrentEP <= 0)
					{
						ResetPrimaryWeaponBodyCondition(WEAPON_BODYCON_RELOAD);
					}
				}
				// 2010-06-15 by shcho&hslee Ćę˝Ă˝şĹŰ - ą«±â ŔĚĆŃĆ® Ăß°ˇ
				else
				{
					ResetPetWeaponBodyCondition(WEAPON_BODYCON_RELOAD);					
				}
				// end 2010-06-15 by shcho&hslee Ćę˝Ă˝şĹŰ - ą«±â ŔĚĆŃĆ® Ăß°ˇ

			}
		}
	}
	else if(bType == 1) // 2Çü
	{
		if(bReset)
		{
			DeleteWearItem( WEAR_ITEM_KIND_WEAPON_21 );
			DeleteWearItem( WEAR_ITEM_KIND_WEAPON_22 );
			if(g_pGameMain)
			{
				g_pGameMain->m_pWeaponInfo->ChangeWeapon(2);
			}
			if(m_pSecondaryWeapon == NULL)
			{
				// 2009. 08. 27 by ckPark ±×·ˇÇČ ¸®ĽŇ˝ş şŻ°ć ˝Ă˝şĹŰ ±¸Çö
				//CreateSecondaryWeaponItem(NULL);
				CreateSecondaryWeaponItem( NULL, NULL );
				// end 2009. 08. 27 by ckPark ±×·ˇÇČ ¸®ĽŇ˝ş şŻ°ć ˝Ă˝şĹŰ ±¸Çö
			}
			else
			{
				// 2009. 08. 27 by ckPark ±×·ˇÇČ ¸®ĽŇ˝ş şŻ°ć ˝Ă˝şĹŰ ±¸Çö
				//CreateSecondaryWeaponItem(m_pSecondaryWeapon->GetRealItemInfo());
				ITEM*	pShapeItem	= g_pDatabase->GetServerItemInfo( m_pSecondaryWeapon->GetItemGeneral()->GetShapeItemNum() ) ;
				
				CreateSecondaryWeaponItem( m_pSecondaryWeapon->GetRealItemInfo(), pShapeItem );
				// end 2009. 08. 27 by ckPark ±×·ˇÇČ ¸®ĽŇ˝ş şŻ°ć ˝Ă˝şĹŰ ±¸Çö

				if(m_pSecondaryWeapon->GetItemGeneral()->CurrentCount <= 0)
				{
					ResetSecondaryWeaponBodyCondition(WEAPON_BODYCON_RELOAD);
					ResetSecondaryWeaponBodyCondition(WEAPON_BODYCON_RELOAD, FALSE);
				}
			}
		}
		else
		{
			if(m_pSecondaryWeapon)
			{
				if(m_pSecondaryWeapon->GetItemGeneral()->CurrentCount <= 0)
				{
					ResetSecondaryWeaponBodyCondition(WEAPON_BODYCON_RELOAD);
					ResetSecondaryWeaponBodyCondition(WEAPON_BODYCON_RELOAD, FALSE);
				}
			}
		}
	}
	float fRange1 , fRange2;
	fRange1 = fRange2 = 0;
	if(m_pPrimaryWeapon)
		fRange1 = m_pPrimaryWeapon->GetRealItemInfo()->Range;
	if(m_pSecondaryWeapon)
		fRange2 = m_pSecondaryWeapon->GetRealItemInfo()->Range;
	m_fWeaponMaxDistance = max(fRange1,fRange2);
}

void CShuttleChild::SetShuttleFirstPosition(BOOL bMode)
{
	FLOG( "CShuttleChild::SetShuttleFirstPosition(BOOL bMode)" );
	float fElapsedTime = g_pD3dApp->GetElapsedTime();
	float fTempHeight1 = DEFAULT_COLLISION_DISTANCE;
	D3DXVECTOR3 vPos = m_vPos;
//	if(m_bOnObject || bMode)
//	{
		D3DXMATRIX matTemp;
		D3DXVECTOR3 vSide;
		vSide = D3DXVECTOR3(0,0,1);
		D3DXMatrixLookAtLH(&matTemp,&vPos,&(vPos - m_vBasisUp),&vSide);
		if(g_pScene->m_pObjectRender)
			fTempHeight1 = g_pScene->m_pObjectRender->CheckCollMesh(matTemp,vPos, FALSE).fDist;
		D3DXMatrixLookAtLH(&matTemp,&vPos,&(vPos + m_vBasisUp),&vSide);
		float fTempCheck = DEFAULT_COLLISION_DISTANCE;
		if(g_pScene->m_pObjectRender)
			fTempCheck = g_pScene->m_pObjectRender->CheckCollMesh(matTemp,vPos, FALSE).fDist;
		if(fTempHeight1 != DEFAULT_COLLISION_DISTANCE && fTempCheck != DEFAULT_COLLISION_DISTANCE)
		{
			if(vPos.y - fTempHeight1 < vPos.y + fTempCheck)
				fTempHeight1 = vPos.y + fTempCheck;
			else
				fTempHeight1 = vPos.y - fTempHeight1;
		}
		else
		{
			fTempHeight1 = vPos.y - fTempHeight1;
		}
//	}
	float fTempHeight2 = CheckMap(m_vPos);
	if(!bMode)
	{
		if(fTempHeight2 < fTempHeight1)
		{
			if(m_vPos.y > fTempHeight1 + m_fAmorHeight)
			{
				m_vPos.y -= (m_vPos.y - fTempHeight1 - m_fAmorHeight)*0.5f*fElapsedTime;
				if(m_vPos.y < fTempHeight1 + m_fAmorHeight)
					m_vPos.y = fTempHeight1 + m_fAmorHeight;
			}
			else if(m_vPos.y < fTempHeight1 + m_fAmorHeight)
			{
				m_vPos.y -= (m_vPos.y - fTempHeight1 - m_fAmorHeight)*0.5f*fElapsedTime;
				if(m_vPos.y > fTempHeight1 + m_fAmorHeight)
					m_vPos.y = fTempHeight1 + m_fAmorHeight;
			}
		}
		else// if(fTempHeight2 > fTempHeight1)
		{
			if(m_vPos.y > fTempHeight2 + m_fAmorHeight)
			{
				m_vPos.y -= (m_vPos.y - fTempHeight2 - m_fAmorHeight)*0.5f*fElapsedTime;
				if(m_vPos.y < fTempHeight2 + m_fAmorHeight)
					m_vPos.y = fTempHeight2 + m_fAmorHeight;
			}
			else if(m_vPos.y < fTempHeight2 + m_fAmorHeight)
			{
				m_vPos.y -= (m_vPos.y - fTempHeight2 - m_fAmorHeight)*0.5f*fElapsedTime;
				if(m_vPos.y > fTempHeight2 + m_fAmorHeight)
					m_vPos.y = fTempHeight2 + m_fAmorHeight;
			}
		}
	}
	else
	{
		if(fTempHeight2 < fTempHeight1)
		{
			if(m_vPos.y > fTempHeight1 + m_fAmorHeight)
			{
				m_vPos.y = fTempHeight1 + m_fAmorHeight;
			}
			else if(m_vPos.y < fTempHeight1 + m_fAmorHeight)
			{
				m_vPos.y = fTempHeight1 + m_fAmorHeight;
			}
		}
		else// if(fTempHeight2 > fTempHeight1)
		{
			if(m_vPos.y > fTempHeight2 + m_fAmorHeight)
			{
				m_vPos.y = fTempHeight2 + m_fAmorHeight;
			}
			else if(m_vPos.y < fTempHeight2 + m_fAmorHeight)
			{
				m_vPos.y = fTempHeight2 + m_fAmorHeight;
			}
		}
	}
//	if(m_dwState == _NORMAL)
//		m_vPos.y += m_fAmorHeight;// ÁöÇüş¸´Ů ľŕ°Ł łô°Ô ¶çżî´Ů
//		m_vPos.y += 300.0f;// ÁöÇüş¸´Ů ľŕ°Ł łô°Ô ¶çżî´Ů
	
}

void CShuttleChild::InitShuttleData(BOOL bState)
{
	FLOG( "CShuttleChild::InitShuttleData()" );
	DBGOUT("CShuttleChild::InitShuttleData()\n");
	D3DXMatrixIdentity(&m_mMatrix);
	m_bRButtonState = FALSE;
	m_bLButtonState = FALSE;
	m_bUpButtonState = FALSE;
	m_bDownButtonState = FALSE;
	m_bLeftButtonState = FALSE;
	m_bRightButtonState = FALSE;
	m_bMButtonState = FALSE;
	ChangeUnitState( _NORMAL );
	//m_dwState = _NORMAL;
	m_fShuttleSpeed = 0.0f;
	m_fLRVelRate = 0.0f;
	m_pTarget = NULL;
	m_pOrderTarget = NULL;
	m_bOnObject = TRUE;
	D3DXVECTOR3 vTempPos;
	vTempPos.x = g_pGround->m_projectInfo.sXSize*TILE_SIZE/2.0f+rand()%100;
	vTempPos.y = m_vPos.y;
	vTempPos.z = g_pGround->m_projectInfo.sYSize*TILE_SIZE/2.0f+rand()%100;
	D3DXVec3Normalize(&m_vVel,&(vTempPos-m_vPos));
	DBGOUT("ShuttleChild,m_vPos(%.2f, %.2f, %.2f)\n", m_vPos.x, m_vPos.y, m_vPos.z);
	DBGOUT("ShuttleChild,m_vVel(%.2f, %.2f, %.2f)\n", m_vVel.x, m_vVel.y, m_vVel.z);
	m_vUp = D3DXVECTOR3(0,1,0);
    m_vBasisUp = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	D3DXVec3Normalize(&m_vVel,&m_vVel);
    D3DXVec3Cross(&m_vSideVel,&m_vUp,&m_vVel);          
	m_vWeaponVel = m_vVel;

	// 2010-06-15 by shcho&hslee Ćę˝Ă˝şĹŰ - A-Gear ˝ĂÁî»óĹÂ
	m_vPetLeftVel = m_vVel;
	m_vPetRightVel = m_vVel;
	// end 2010-06-15 by shcho&hslee Ćę˝Ă˝şĹŰ - A-Gear ˝ĂÁî»óĹÂ
	m_bIsCameraPattern = FALSE;
	m_bMouseMoveLock = FALSE;
	m_nMissileCount = 0;											// 2005-08-10 by ispark
	m_bMissileWarning = FALSE;
	m_bSkillMissileWarning = FALSE;
	m_bSkillGoMove			= TRUE;
	m_bWarpLink = FALSE;
	m_bLandingField = FALSE;
	m_bKeyDownA = FALSE;											// 2005-10-13 by ispark
	m_bKeyDownD = FALSE;
	m_bRollUsed = FALSE;
	m_bUnitStop = FALSE;
	m_bLandedMove = FALSE;
	m_bPenaltyUnitStop = FALSE;
	m_fCurrentSideRate = 0.0f;
	m_fCurrentAngle = 0.0f;
	
	// 2005-08-02 by ispark
	// ±âş» şÎ˝şĹÍ´Â MINŔĚ´Ů.
	m_nBooster = BOOSTER_TYPE_MIN;
	
	if(g_pScene->m_byMapType == MAP_TYPE_TUTORIAL)
	{
		m_vPos = D3DXVECTOR3(2702,178,516.5);
		m_vVel = D3DXVECTOR3(1,0,0);
	}
	if(g_pScene->m_byMapType == MAP_TYPE_CITY)
	{
		m_nBooster = BOOSTER_TYPE_STOP;
		m_vPos = MAP_TYPE_CITY_UNIT_POS;
		m_myShuttleInfo.PositionVector = MAP_TYPE_CITY_UNIT_POS;
	}
	else
	{
// 2005-08-03 by ispark, żąŔü 2001ąř ¸Ę Ăł¸® »çÇ×
//		if(m_nOldMapIndex == 1001 && m_myShuttleInfo.MapChannelIndex.MapIndex == 2001)
//		{
//			m_vVel.y = 0;
//			m_vPos.y = 400.0f;
//			if(m_vPos.x/TILE_SIZE > 120.0f)
//			{
//				m_vVel.x = 0.7f;
//			}
//			else
//			{
//				m_vVel.x = -0.7f;
//			}
//			if(m_vPos.z/TILE_SIZE > 120.0f)
//			{
//				m_vVel.z = 0.7f;
//			}
//			else
//			{
//				m_vVel.z = -0.7f;
//			}
//		}
//		else if(m_nOldMapIndex != 0 && m_myShuttleInfo.MapChannelIndex.MapIndex == 2001)
//		{
//			m_vPos.y = 400.0f;
//		}
		// 2005-08-02 by ispark
		// şÎ˝şĹÍ¸¦ ˛ô´Â żřŔÎŔĚ ż©±â´Ů.
		// żöÇÁ·Î µéľîżŔ´Â °Ĺ¶ó¸é »ó°üŔĚ ľř´Ů. żöÇÁżˇĽ­ ĽöÁ¤ŔĚ µČ´Ů.
		// ±×·ŻłŞ, ±×żÜ´Â ľČ‰Â´Ů. 2001ąřŔ¸·Î µéľîżĂ¶§ ±×·¸´Ů.
//		if(IS_BT(m_myShuttleInfo.UnitKind) || IS_ST(m_myShuttleInfo.UnitKind))
//		{
//			m_nShuttleStopState = 3;
//			m_nBooster = BOOSTER_TYPE_AIRBREAK;
//		}
//		else if(IS_DT(m_myShuttleInfo.UnitKind))
		if(IS_DT(m_myShuttleInfo.UnitKind))
		{
			m_bIsAir = TRUE;//˝ĂŔŰ˝Ă °ˇ¶óľÉÁö ľĘ°Ô..// 2004-06-22 by dhkwon
		}
	}
	m_vNextPos = m_vPos;
//	m_fOldUpRate = 0;
//	m_fSideRate = 0;
	m_fSideCheckRate = 0;

	D3DXMatrixLookAtRH( &m_mMatrix, &m_vPos, &(m_vPos + m_vVel), &m_vUp);
	D3DXMatrixInverse( &m_mMatrix, NULL, &m_mMatrix );
	g_pD3dApp->SetCamPosInit();
	g_pD3dApp->m_bFixUnitDirMode = FALSE;

	// ĂÖĂĘ ˝ĂŔŰ ÁöÁˇ ĂĽĹ©
	int x = ((int)m_vPos.x)/TILE_SIZE;
	int z = ((int)m_vPos.z)/TILE_SIZE;
	// 2005-08-03 by ispark
	m_ptOldPoint.x = x;
	m_ptOldPoint.y = z;

	// 2005-08-22 by ispark
	// µéľîżÔŔ» ¶§ Âř·úÁöÁˇżˇĽ­ Âř·ú ¸řÇĎ°Ô ¸·Ŕ˝. Á¤»óŔűŔÎ Âř·úŔĚ ľĆ´Ď¸é łŻ°Ô ÇŃ´Ů.
//	if(g_pGround->m_pTileInfo[x*g_pGround->m_projectInfo.sYSize + z].bEnableLand && bState == FALSE)
//	{
////		SendPacket(T_FC_MOVE_LANDING);
//		// 2004-11-04 by jschoi
//		// ĂÖĂĘ ˝ĂŔŰ ÁöÁˇ ĂĽĹ©żˇĽ­ ÇöŔç Ŕ§Äˇ°ˇ Âř·úÁöÁˇŔĚ¶ó¸é OnObjectŔÎÁö °Ë»ç¸¦ Ŕ§ÇŘ m_vPos ¸¦ m_fAmorHeight ¸¸Ĺ­ żĂ·ÁÁŘ´Ů.
//		m_vPos.y += m_fAmorHeight;
//		if(SetOrderMoveTargetPos() == TRUE )
//		{
//			m_vPos = m_vTargetPos;
//			m_vNextPos = m_vTargetPos;
//			//m_dwState = _LANDED;
//			ChangeUnitState( _LANDED );
//			m_bIsAir = FALSE;
////			SendPacket(T_FC_MOVE);
////			g_pD3dApp->SendMovePacket(FALSE);
//			g_fSendMovePacket = max(g_fSendMovePacket,5.0f);
//		}
//	}
//	else
//	{
//		//m_vPos.y += 300.0f;// ÁöÇüş¸´Ů ľŕ°Ł łô°Ô ¶çżî´Ů, żÖ żĂ¸±±î?
//	}
	// Âř·úŔü ˝ĂÁˇŔĚ 1ŔÎÄŞŔĚż´Ŕ¸¸é ´Ů˝Ă 1ŔÎÄŞŔ¸·Î şŻČŻ 
	if(m_bLandingOldCamtypeFPS)
	{
		g_pD3dApp->m_pCamera->ChangeCamType();
		m_bLandingOldCamtypeFPS = FALSE;
	}
	
	SetCursorInit();
	
	m_bDegree = 2;// 2004-05-06 by dhkwon
	
//	SetShuttleFirstPosition(TRUE);
	InitEffect();
//	g_pScene->SetDay();
	InitItemPoint();
	InitRolling();

	// Ćí´ë´ëÇü ĂĘ±âČ­
	m_pClientParty->FormationFlightClear();
	SetShuttleSpeed();
	
	m_byCityWarTeamType = CITYWAR_TEAM_TYPE_NORMAL;

	// 2005-10-06 by ispark
	// Ć©Ĺä¸®ľó ¸đµĺ¶ó¸é Ă˘ ĂĘ±âČ­ ˝ÇÇŕ ľČÇÔ
	if(g_pInterface->m_pGameMain)
	{
		g_pInterface->m_pGameMain->InitShowWindow();			// 2005-08-05 by ispark
	}

	// 2008-06-20 by bhsohn EP3 żÉĽÇ°ü·Ă Ăł¸®
	{
		//g_pInterface->m_bShowInterface = TRUE;						// 2005-08-30 by ispark
		//BOOL bShowInter = !g_pD3dApp->IsOptionEtc(OPTION_RADIO_INTERFACE_HIDE);
		//g_pInterface->SetShowInterface(bShowInter);		
	}
	// end 2008-06-20 by bhsohn EP3 żÉĽÇ°ü·Ă Ăł¸®
	if(g_pInterface->m_pCityBase)
	{
		g_pInterface->m_pCityBase->CloseCurrentEnterBuilding();	// 2005-11-11 by ispark »çżëÁßŔĚ´ř »óÁˇ ´Ý±â
	}

	// ÇŮ °Ë»ç¸¦ Ŕ§ÇŃ ÁÂÇĄ ĂĘ±âČ­
	g_pD3dApp->m_vShuttleOldPos = m_vPos;

	m_fCheckDead = 0.0f;
	m_bKeyBlock = FALSE;

	// 2006-11-14 by ispark, żěÁÖ¸ĘżˇĽ­ Á×Ŕ¸¸é ĽöĆň ŔŻÁö°ˇ »ç¶óÁö´Â ąö±× ĽöÁ¤
	m_bCollMap = FALSE;
}

void CShuttleChild::CheckClientQuestList()
{
	FLOG( "CShuttleChild::CheckClientQuestList()" );
	static float fMonsterMoveCheckTime = 0.2f;
	static float fDegreeCheckTime = 0.0f; // Äů˝şĆ® ˝ĂŔŰ˝Ă şÎĹÍ żĎ·á˝Ă±îÁö Ĺ¸ŔÓŔ» ´©ŔűÇĎż© ÁˇĽö¸¦ ±¸ÇŇ¶§ ľ´´Ů.
	float fElapsedTime = g_pD3dApp->GetElapsedTime();
	int cont = 0;
	if(fMonsterMoveCheckTime > 0.0f)
		fMonsterMoveCheckTime -= fElapsedTime;
	map<int, MEX_QUEST_INFO*>::iterator itQuest = g_pQuestData->m_mapCharacterQuest.begin();
	while(itQuest != g_pQuestData->m_mapCharacterQuest.end())
	{
		int nState = itQuest->second->QuestState;
		if(itQuest->second->QuestIndex == TUTORIAL_MISSION_1 || itQuest->second->QuestIndex == TUTORIAL_MISSION_2)
		{
			//CMapObjectIterator itObj = g_pScene->m_mapObjectList.begin();
			vectorCObjectChildPtr::iterator itObj(g_pScene->m_vectorCollisionObjectPtrList.begin());
			while(itObj != g_pScene->m_vectorCollisionObjectPtrList.end())
			{
				(*itObj)->m_fCurrentTime = 0.0f;
				itObj++;
			}
		}
		if(nState == QUEST_STATE_IN_PROGRESS)
		{
			if(itQuest->second->QuestIndex == TUTORIAL_MISSION_1)
			{// Ľł¸íŔ» µč´Â Äů˝şĆ® - ŔĚµżľČ ĽĹĆ˛Ŕş żňÁ÷ŔĚÁö ľĘ´Â´Ů.
				m_bOrderMove = TRUE;
				return;
			}
			else if(itQuest->second->QuestIndex == TUTORIAL_MISSION_2)
			{
				if(m_bOrderMove)
				{// °­Á¦ ŔĚµż ĂĽĹ©
					if(MoveOrder(m_bOrderMoveType))
						m_bOrderMove = FALSE;
				}
				if(m_bBeginnerQuestDegree == 1)
				{
					fDegreeCheckTime = 0.0f;
				}
				else
				{
					fDegreeCheckTime += fElapsedTime;
				}
				// 1´Ü°č ¸ó˝şĹÍ ĽŇČŻ
/*				m_nMonsterCount = 0;
				CMapMonsterIterator itMon = g_pScene->m_mapMonsterList.begin();
				while(itMon != g_pScene->m_mapMonsterList.end())
				{
					if(itMon->second->m_dwState == _NORMAL)
					{
						m_nMonsterCount++;
					}
					itMon++;
				}
				if(!m_nMonsterCount)
*/				if(g_pScene->m_mapMonsterList.empty())
				{// bQuestDegree ´Ü°č ¸ó˝şĹÍ ĽŇČŻ
					if(m_bBeginnerQuestDegree < 6)
					{
						vector<MONSTERINFO *>::iterator itResMon = g_pGround->m_vecMonsterResList.begin();
						while(itResMon != g_pGround->m_vecMonsterResList.end())
						{
							MONSTERINFO * pData = *itResMon;
							if(pData->sMaxMon == m_bBeginnerQuestDegree)
							{
								MSG_FC_CHARACTER_GET_MONSTER_INFO_OK info;
								info.MonsterIndex = 10000+cont+1;
								info.MonsterUnitKind = pData->nMonType;
								info.BodyCondition = BODYCON_CREATION_MASK;
								
								info.PositionVector.x = pData->sStartx*40.0f + rand()%40;
								info.PositionVector.y = pData->sResTime;
								info.PositionVector.z = pData->sStartz*40.0f + rand()%40;
								info.CurrentHP = 45;			// ¸ó˝şĹÍŔÇ CurrentHP´Â SHORT
								CMonsterData * pMon = new CMonsterData(&info);
								if(rand()%2 == 0)
									pMon->m_nSideDir = 1;
								else
									pMon->m_nSideDir = -1;
								g_pScene->InsertToBlockData(pMon);
								if(pMon->m_pCharacterInfo)
								{
									pMon->m_pCharacterInfo->SetBodyCondition(info.BodyCondition);
								}
								memcpy(&pMon->m_ResInfo,pData,sizeof(MONSTERINFO));
								g_pScene->m_mapMonsterList[info.MonsterIndex] = pMon;
								cont++;
							}
							itResMon++;
						}
						if(cont)
						{
							char buf[256];
							wsprintf(buf,STRMSG_C_MONSTER_0001,cont);//"%d±âŔÇ şńĆ®°ˇ ĽŇČŻ µÇľú˝Ŕ´Ď´Ů"
							g_pD3dApp->m_pChat->CreateChatChild(buf, COLOR_SYSTEM);
							if(m_bBeginnerQuestDegree == 1)
							{
//								g_pGameMain->SetNotice("¸ńÇĄą°ŔÇ Á¶ÁŘŔş ¸¶żě˝ş¸¦ ¸ńÇĄą°żˇ °ˇÁ®´Ů µÇ¸é µË´Ď´Ů.");
							}
							else if(m_bBeginnerQuestDegree == 2)
							{
//								g_pGameMain->SetNotice("±âĂĽ Č¸ŔüŔş ¸¶żě˝ş ŔĚµżŔ¸·Î °ˇ´ÉÇĎ¸ç °ř°ÝŔş ¸¶żě˝ş żŢÂĘ ąöĆ° ŔÔ´Ď´Ů.");
							}
						}
						m_bBeginnerQuestDegree++;
					}
					else
					{// Äů˝şĆ® żĎ·á 
						MSG_FC_QUEST_REQUEST_SUCCESS sMsg;
						memset(&sMsg,0x00,sizeof(sMsg));
						char buffer[SIZE_MAX_PACKET];
						sMsg.QuestIndex = TUTORIAL_MISSION_2;
						if(fDegreeCheckTime <= 20.0f)
						{// A  index : 10
							sMsg.QuestResult = 10;
						}
						else if(fDegreeCheckTime <= 40.0f)
						{// B  index : 11
							sMsg.QuestResult = 11;
						}
						else if(fDegreeCheckTime <= 80.0f)
						{// C  index : 12
							sMsg.QuestResult = 12;
						}
						else
						{// D  index : 13
							sMsg.QuestResult = 13;
						}
						int nType = T_FC_QUEST_REQUEST_SUCCESS;
						memcpy(buffer, &nType, SIZE_FIELD_TYPE_HEADER);
						memcpy(buffer+SIZE_FIELD_TYPE_HEADER, &sMsg, sizeof(sMsg));
						g_pD3dApp->m_pFieldWinSocket->Write(buffer, SIZE_FIELD_TYPE_HEADER + sizeof(sMsg));
						char buf[256];
						wsprintf(buf,STRMSG_C_MONSTER_0002);//"şńĆ® ĽŇĹÁ żĎ·á"
						g_pD3dApp->m_pChat->CreateChatChild(buf, COLOR_SKILL_USE);
						m_bOrderMove = TRUE;	// °­Á¦ ŔĚµżżˇ µéľî°Ł´Ů.
						if(IS_DT(m_myShuttleInfo.UnitKind))
						{
//							m_vTargetPos = D3DXVECTOR3(2690,10,507);
							m_bIsAir = TRUE;
						}
//						else
							m_vTargetPos = D3DXVECTOR3(2702,178,516.5);
						m_vTargetVel = D3DXVECTOR3(1,0,0);
						m_bOrderMoveType = 0;
						itQuest->second->QuestState = QUEST_STATE_COMPLETED;
//						g_pGameMain->SetNotice("Âř·úŔş ĽýŔÚĆÇŔÇ 'C'Ĺ°¸¦ ´©¸Ł¸é µË´Ď´Ů.");
					}
				}
				else
				{
					if(fMonsterMoveCheckTime <= 0.0f)
					{
						map<INT,CMonsterData *>::iterator itMonster = g_pScene->m_mapMonsterList.begin();
						while(itMonster != g_pScene->m_mapMonsterList.end())
						{
							if(!m_pOrderTarget && ATTACK_AVAILABLE_STATE(itMonster->second->m_dwState))
							{
								if(m_pOrderTarget != itMonster->second)
								{
									m_pOrderTarget = itMonster->second;
									if(g_pD3dApp->m_pSound)
						g_pD3dApp->m_pSound->PlayD3DSound(SOUND_LOCKON_PRIMARY, g_pShuttleChild->m_vPos, FALSE);
								}
							}
							CheckMonsterMove(itMonster->second);
							itMonster++;
						}
						fMonsterMoveCheckTime = 0.2f;
					}
				}
			}
			else if(itQuest->second->QuestIndex == TUTORIAL_MISSION_3)
			{
				if(m_bOrderMove)
				{// °­Á¦ ŔĚµż ĂĽĹ©
					if(!MoveOrder(m_bOrderMoveType))
						return;
					m_bOrderMove = FALSE;
//					g_pGameMain->SetNotice("Ĺ°ş¸µĺ 'W'´Â ĽÓµµ¸¦ żĂ¸®°í 'S'´Â ĽÓµµ¸¦ ł»¸ł´Ď´Ů.");
				}

				if(m_bBeginnerQuestDegree <= 6)
				{
					m_bBeginnerQuestDegree = 6;
				}
				if(m_bBeginnerQuestDegree == 6)
				{
					fDegreeCheckTime = 0.0f;
				}
				else
				{
					fDegreeCheckTime += fElapsedTime;
				}
//				if(m_bBeginnerQuestDegree > 3)
//				{// 2´Ü°č ¸ó˝şĹÍ ĽŇČŻ
/*					m_nMonsterCount = 0;
					CMapMonsterIterator itMon = g_pScene->m_mapMonsterList.begin();
					while(itMon != g_pScene->m_mapMonsterList.end())
					{
						if(itMon->second->m_dwState == _NORMAL)
						{
							m_nMonsterCount++;
						}
						itMon++;
					}
					if(!m_nMonsterCount)
*/					if(g_pScene->m_mapMonsterList.empty())
					{// bQuestDegree ´Ü°č ¸ó˝şĹÍ ĽŇČŻ
						if(m_bBeginnerQuestDegree < 20)
						{
							vector<MONSTERINFO *>::iterator itResMon = g_pGround->m_vecMonsterResList.begin();
							while(itResMon != g_pGround->m_vecMonsterResList.end())
							{
								MONSTERINFO * pData = *itResMon;
								if(pData->sMaxMon == m_bBeginnerQuestDegree)
								{
									MSG_FC_CHARACTER_GET_MONSTER_INFO_OK info;
									info.MonsterIndex = 10000+cont+1;
									info.MonsterUnitKind = pData->nMonType;
									info.BodyCondition = BODYCON_CREATION_MASK;
									
									info.PositionVector.x = pData->sStartx*40.0f + rand()%40;
									if(m_bBeginnerQuestDegree == 5)
										info.PositionVector.y = pData->sResTime + rand()%100;
									else
										info.PositionVector.y = pData->sResTime;
									info.PositionVector.z = pData->sStartz*40.0f + rand()%40;
									info.CurrentHP = 45;			// ¸ó˝şĹÍŔÇ CurrentHP´Â SHORT
									CMonsterData * pMon = new CMonsterData(&info);
									if(rand()%2 == 0)
										pMon->m_nSideDir = 1;
									else
										pMon->m_nSideDir = -1;
									g_pScene->InsertToBlockData(pMon);
									if(pMon->m_pCharacterInfo)
									{
										pMon->m_pCharacterInfo->SetBodyCondition(info.BodyCondition);
									}
									memcpy(&pMon->m_ResInfo,pData,sizeof(MONSTERINFO));
									g_pScene->m_mapMonsterList[info.MonsterIndex] = pMon;
									cont++;
								}
								itResMon++;
							}
							if(cont)
							{
								char buf[256];
								wsprintf(buf,STRMSG_C_MONSTER_0001,cont);//"%d±âŔÇ şńĆ®°ˇ ĽŇČŻ µÇľú˝Ŕ´Ď´Ů"
								g_pD3dApp->m_pChat->CreateChatChild(buf, COLOR_SYSTEM);
								if(m_bBeginnerQuestDegree == 7)
								{
//									g_pGameMain->SetNotice("ľĆŔĚĹŰ »çżë ´ÜĂŕĹ°´Â 1~8 ąřŔÔ´Ď´Ů.");
								}
							}
							m_bBeginnerQuestDegree++;
						}
						else
						{// Äů˝şĆ® żĎ·á 
							MSG_FC_QUEST_REQUEST_SUCCESS sMsg;
							memset(&sMsg,0x00,sizeof(sMsg));
							char buffer[SIZE_MAX_PACKET];
							sMsg.QuestIndex = TUTORIAL_MISSION_3;
							if(fDegreeCheckTime <= 60.0f)
							{// A  index : 10
								sMsg.QuestResult = 10;
							}
							else if(fDegreeCheckTime <= 90.0f)
							{// B  index : 11
								sMsg.QuestResult = 11;
							}
							else if(fDegreeCheckTime <= 120.0f)
							{// C  index : 12
								sMsg.QuestResult = 12;
							}
							else
							{// D  index : 13
								sMsg.QuestResult = 13;
							}
							int nType = T_FC_QUEST_REQUEST_SUCCESS;
							memcpy(buffer, &nType, SIZE_FIELD_TYPE_HEADER);
							memcpy(buffer+SIZE_FIELD_TYPE_HEADER, &sMsg, sizeof(sMsg));
							g_pD3dApp->m_pFieldWinSocket->Write(buffer, SIZE_FIELD_TYPE_HEADER + sizeof(sMsg));
							char buf[256];
							wsprintf(buf,STRMSG_C_MONSTER_0002);//"şńĆ® ĽŇĹÁ żĎ·á"
							g_pD3dApp->m_pChat->CreateChatChild(buf, COLOR_SKILL_USE);
							itQuest->second->QuestState = QUEST_STATE_COMPLETED;
						}
					}
					else
					{
						if(g_pScene->m_mapMonsterList.size() == 3 && m_bBeginnerQuestDegree == 7)
						{
//							g_pGameMain->SetNotice("Ŕüąćżˇ ŔÖ´Â ¸ńÇĄą°¸¸ ¸¶żě˝ş¸¦ ŔĚżëÇĎż© Á¶ÁŘŔĚ °ˇ´ÉÇŐ´Ď´Ů.");
						}
						if(g_pScene->m_mapMonsterList.size() == 1 && m_bBeginnerQuestDegree == 7)
						{
//							g_pGameMain->SetNotice("±âĂĽŔÇ Á¦ŔÚ¸®żˇĽ­ ÁÂżě ŔĚµżŔş Ĺ°ş¸µĺ 'A' , 'D' ąöĆ°ŔÔ´Ď´Ů.");
						}
						if(g_pScene->m_mapMonsterList.size() == 3 && m_bBeginnerQuestDegree == 8)
						{
//							g_pGameMain->SetNotice("¸ńÇĄą°Ŕ» şŻ°ćÇĎ°í ˝ÍŔ»¶§´Â Tab ąöĆ°Ŕ» ´©¸Ł˝Ă±â ąŮ¶ř´Ď´Ů.");
						}
						if(fMonsterMoveCheckTime <= 0.0f)
						{
							map<INT,CMonsterData *>::iterator itMonster = g_pScene->m_mapMonsterList.begin();
							while(itMonster != g_pScene->m_mapMonsterList.end())
							{
								if(!m_pOrderTarget && ATTACK_AVAILABLE_STATE(itMonster->second->m_dwState))
								{
									if(m_pOrderTarget != itMonster->second)
									{
										m_pOrderTarget = itMonster->second;
										if(g_pD3dApp->m_pSound)
											g_pD3dApp->m_pSound->PlayD3DSound(SOUND_LOCKON_PRIMARY, g_pShuttleChild->m_vPos, FALSE);
									}
								}
								CheckMonsterMove(itMonster->second);
								itMonster++;
							}
							fMonsterMoveCheckTime = 0.2f;
						}
					}
//					CMapObjectIterator itObj = g_pScene->m_mapObjectList.begin();
					vectorCObjectChildPtr::iterator itObj(g_pScene->m_vectorCollisionObjectPtrList.begin());
					while(itObj != g_pScene->m_vectorCollisionObjectPtrList.end())
					{
						if((*itObj)->m_fCurrentTime > 450.0f)
							(*itObj)->m_fCurrentTime = 450.0f;
						itObj++;
					}
//				}
			}
			break;
		}
		itQuest++;
	}
}

void CShuttleChild::SendFieldSocketRequestEventObjectWarpIn(CObjectChild * pObj)
{
	if(m_pClientParty->GetPartyInfo().bPartyType == _PARTYMASTER && !IS_CITY_MAP_INDEX(g_pShuttleChild->m_myShuttleInfo.MapChannelIndex.MapIndex))
	{
		m_pClientParty->FSendPartyRequestPartyObjectEvent(pObj);
	}
	else
	{
		if(!m_pClientParty->IsFormationFlightMove())
		{
			MSG_FC_EVENT_REQUEST_OBJECT_EVENT sMsg;
			sMsg.ObjectType = pObj->m_nCode;
			sMsg.ObjectPosition = pObj->m_vPos;
			g_pFieldWinSocket->SendMsg( T_FC_EVENT_REQUEST_OBJECT_EVENT, (char*)&sMsg, sizeof(sMsg) );
			DBGOUT("Request Warp : MSG_FC_EVENT_REQUEST_OBJECT_EVENT(5974)\n");
		}
	}
}

void CShuttleChild::CheckEventTypeObject()
{
	FLOG( "CShuttleChild::CheckEventTypeObject()" );
	if(IsPkState(PK_PVP) || IsPkState(PK_FVF))	// 1:1°áĹő°ĹłŞ Ćí´ëŔüŔÎ °ćżěżˇ´Â ŔĚşĄĆ®¸¦ ąŢÁö ľĘ´Â´Ů.
		return;
	if(g_pScene->m_pObjectRender)
	{
//		CObjectChild * pObj = (CObjectChild *)g_pScene->m_pObjectRender->CheckCollMeshEvent();
		m_pCurrentObjEvent = (CObjectChild *)g_pScene->m_pObjectRender->CheckCollMeshEvent(500.0f);
		
		if(!m_pCurrentObjEvent)
			return;
		else if(m_bSkillAirMove)
		{
			DeletePattern();
		}
		switch(m_pCurrentObjEvent->m_bEventType)
		{
		case EVENT_TYPE_WARP:
		{// żöÇÁÁöżŞ(ŔÔ±¸)
			if (m_nCurrentPatternNumber == PATTERN_UNIT_WARP_IN)	// żöÇÁŔÎ ĆĐĹĎ Áßżˇ¸¸ żöÇÁ°ˇ °ˇ´ÉÇĎ´Ů.
			{
#ifdef _CLOSE_STONE_RUINS_AT_MS
				if (0 < g_pGameMain->GetSummonMotherShipCnt()) {
					if (m_pCurrentObjEvent->m_sEventIndexTo == 204 || m_pCurrentObjEvent->m_sEventIndexTo == 203) {
						DeletePattern();
						return;
					}
				}
#endif
				if (IsUnitStop()) { //cannot b enter in warp
					DeletePattern();
					return;
				}
				if (SKILL_BASE_NUM(m_pSkill->GetPreSkillItemNum()) == BGEAR_SKILL_BASENUM_BIG_BOOM) { //cannot drag bb by himself
					DeletePattern();
					return;
				}
				/*//BL
				if (m_pCurrentObjEvent->m_sEventIndexTo == _RES_EVENT_IDX_1_1 || m_pCurrentObjEvent->m_sEventIndexTo == _RES_EVENT_IDX_1_2) {
					if (m_myShuttleInfo.Level < _RESTRICTED_MAP_LV_1)
						return;
				}
				//check if map is restricted only for staff
				if(_IS_ADMIN_RESTRICTED_MAP(m_pCurrentObjEvent->m_sEventIndexTo) 
					&& !COMPARE_RACE(m_myShuttleInfo.Race,RACE_OPERATION|RACE_GAMEMASTER|RACE_DEMO))
					return;
					
				//RC
				if (m_pCurrentObjEvent->m_sEventIndexTo == _RES_EVENT_IDX_2_1 && m_myShuttleInfo.Level < _RESTRICTED_MAP_LV_2) {
					return;
				}
				//VL
				if (m_pCurrentObjEvent->m_sEventIndexTo == _RES_EVENT_IDX_3_1 && m_myShuttleInfo.Level < _RESTRICTED_MAP_LV_3) {
					return;
				}
				//RCO
				if (m_pCurrentObjEvent->m_sEventIndexTo == _RES_EVENT_IDX_4_1 && m_myShuttleInfo.Level < _RESTRICTED_MAP_LV_4) {
					return;
				}
				//J-ARK
				if (m_pCurrentObjEvent->m_sEventIndexTo == _RES_EVENT_IDX_5_1 && m_myShuttleInfo.Level < _RESTRICTED_MAP_LV_5) {
					return;
				}
				//J-ARK Engine Room
				if (m_pCurrentObjEvent->m_sEventIndexTo == _RES_EVENT_IDX_6_1 && m_myShuttleInfo.Level < _RESTRICTED_MAP_LV_6) {
					return;
				}
				*/

				DeletePattern();
				m_nEventType = EVENT_WARP_IN;
				//					m_bEventReady = TRUE;
				//					m_fEventCheckTime = 2.0f;
				ChangeUnitState(_WARP);
				if (g_pD3dApp->m_pEffectList)
				{
					CAppEffectData* pData = new CAppEffectData(this, RC_EFF_OTHER_WARP, D3DXVECTOR3(0, 0, 0));
					g_pD3dApp->m_pEffectList->AddChild(pData);
				}
				// 2007-10-04 by dgwoo żöÇÁ Ĺë°ú·Î ŔÎÇŃ ÇÁ·Ń·Î±×°ˇ ľĆ´Ń ¸đµç ąĚĽÇŔ» żĎ·áÇĎż´Ŕ»°ćżě 
				// ÇÁ·Ń·Î±×¸¦ ş¸ż©ÁŘ´Ů.
//					if(g_pTutorial->IsTutorialMode() == TRUE && g_pTutorial->GetLesson() == L10)
//					{
//						g_pTutorial->EndTutorial();
//						// 2006-09-07 by ispark
//						STAGE_EFFECT_DATA stStageData;
//						stStageData.pWarpObject = (int *)m_pCurrentObjEvent;
//						g_pInterface->InitSoundScript("Prolog.tex", &stStageData, TUTORIAL_COMPLETE, TRUE);
//						break;
//					}

//					if( InitCinemaUnit(PATTERN_UNIT_WARP_IN) == FALSE ||
//						InitCinemaCamera(PATTERN_CAMERA_WARP_IN) == FALSE )
//					{// ĆĐĹĎ »ýĽş ˝ÇĆĐ˝Ă ±×łÉ ş¸ł˝´Ů.
				SendFieldSocketRequestEventObjectWarpIn(m_pCurrentObjEvent);
				// 					}
				//					else
				//					{
				//						CObjectChild* pWarpgate = g_pScene->FindWarpGateByPosition( m_vPos );
				//						if( pWarpgate )
				//						{
				//							pWarpgate->ChangeBodycondition( BODYCON_WARP_MASK );
				//						}
				//					}
			}
			break;
		}
			break;
		case EVENT_TYPE_ENTER_BUILDING:
			{// »óÁˇ Object
				if(g_pD3dApp->m_dwGameState != _SHOP)
				{
					MSG_FC_EVENT_REQUEST_OBJECT_EVENT sMsg;
					memset(&sMsg,0x00,sizeof(sMsg));
					char buffer[SIZE_MAX_PACKET];
					int nType = T_FC_EVENT_REQUEST_OBJECT_EVENT;
//					sMsg.ObjectIndex = pObj->m_nObjectCont;
					sMsg.ObjectType = m_pCurrentObjEvent->m_nCode;
					sMsg.ObjectPosition = m_pCurrentObjEvent->m_vPos;
					memcpy(buffer,&nType,SIZE_FIELD_TYPE_HEADER);
					memcpy(buffer+SIZE_FIELD_TYPE_HEADER,&sMsg,sizeof(sMsg));
					g_pD3dApp->m_pFieldWinSocket->Write(buffer,SIZE_FIELD_TYPE_HEADER + sizeof(sMsg));
//					m_dwState = _LANDED;
					DBGOUT("Request Warp : MSG_FC_EVENT_REQUEST_OBJECT_EVENT(5997)\n");
//					m_bEventReady = TRUE;
//					m_fEventCheckTime = 2.0f;
				}
			}
			break;
		case EVENT_TYPE_WARP_TARGET:
			{// Ĺ¬¶óŔĚľđĆ® ÇöŔç »çżëľČÇÔ
			}
			break;
		case EVENT_TYPE_LANDING:
			{// Âř·ú Object
			}
			break;
		case EVENT_TYPE_RACING_CHECK_POTINT:
			{// ·ąŔĚ˝Ě ĂĽĹ© Ć÷ŔÎĆ®
			}
			break;
		case EVENT_TYPE_OBJECT_MONSTER:
			{// żŔşęÁ§Ć® ¸ó˝şĹÍ - ľĆą«°Íµµ ľČÇÔ
			}
			break;
		case 7:
			{// ąŮ˛đżąÁ¤
			}
			break;
		case EVENT_TYPE_PATTERN_POINT:
			{// Äł¸ŻĹÍ żöÇÁ˝Ă ĆĐĹĎŔÇ ˝ĂŔŰ/łˇ
				if(m_nEventTypeAirPort == AIR_PORT_TAKEOFF_PATTERN_START && 
					m_bWarpLink == TRUE)// 2006-05-04 by ispark, ŔĚ·ú˝Ă żöÇÁ¸¦ ¸¸łŞ¸é żöÇÁÇŃ´Ů.
				{
					// ŔĚ·úĂł¸® ÇŃ´Ů.
					m_nEventTypeAirPort = AIR_PORT_TAKEOFF_PATTERN_END;
					DeletePattern();
					ChangeUnitState( _NORMAL );
					m_bWarpLink = FALSE;
					m_bKeyBlock = FALSE;										// 2005-10-26 by ispark
					// 2007-06-12 by bhsohn ąĚĽÇ ŔÎĹÍĆäŔĚ˝ş ĽöÁ¤ľČ					
//					if(g_pGameMain->m_pMissionInfo->GetMissionMapIndex())
//					{
//						INIT_MSG_WITH_BUFFER(MSG_FC_QUEST_MOVE_QUEST_MAP, T_FC_QUEST_MOVE_QUEST_MAP,
//											pSQuestMove, SendBuf);
//						pSQuestMove->QuestIndex0 = g_pGameMain->m_pMissionInfo->GetMissionIndex()->QuestIndex;
//						g_pFieldWinSocket->Write(SendBuf, MSG_SIZE(MSG_FC_QUEST_MOVE_QUEST_MAP));
//					}
					SendGoMissionMap();					
					
				}
  				if(m_nCurrentPatternNumber == PATTERN_UNIT_WARP_OUT)	// żöÇÁľĆżô ĆĐĹĎ ÁßŔĚ¸é ĆĐĹĎŔĚ Áľ·áµČ´Ů.
				{
					DeletePattern();
				}
				else if(m_nCurrentPatternNumber == 0)						// ÇöŔç ÁřÇŕÁßŔÎ ĆĐĹĎŔĚ ľřŔ¸¸é żöÇÁŔÎ ĆĐĹĎŔ» »ýĽşÇŃ´Ů.
				{
					// 2007-04-05 by bhsohn şňşŐ ąö±× Ăł¸®
					BOOL bWarpSuccess = (g_pTutorial->IsTutorialMode() == FALSE);
					if(bWarpSuccess && m_pSkill)
					{
						// şňşŐŔ» ľ˛¸éĽ­ Ć÷Ĺ»Ŕ» ľ˛Áö ¸řÇĎ°Ô şŻ°ć
						if(SKILL_BASE_NUM(m_pSkill->GetPreSkillItemNum()) == BGEAR_SKILL_BASENUM_BIG_BOOM)
						{							
							bWarpSuccess = FALSE;
						}						
					}
//					if(g_pTutorial->IsTutorialMode() == FALSE || 
//						(g_pTutorial->IsTutorialMode() == TRUE && g_pTutorial->GetLesson() == L8))
					if( bWarpSuccess
						|| (g_pTutorial->IsTutorialMode() == TRUE && g_pTutorial->GetLesson() == L10))
					{
						if(g_pTutorial->IsTutorialMode() == TRUE && g_pTutorial->GetLesson() == L10)
						{
							// Ć©Ĺä¸®ľóŔ» łˇł»°íĽ­ żöÇÁ¸¦ ÇßŔ˝. 
							g_pTutorial->SetTutorialEd(TRUE);
						}

						// ĆĐĹĎ Ŕ§ÇÁ żŔşęÁ§Ć®ŔÇ ąćÇâ°ú ĽĹĆ˛ŔÇ ąćÇâ »çŔĚŔÇ °˘Ŕ» ±¸ÇŃ´Ů.
						CObjectChild* pObj = g_pScene->FindEventObjectByIndex(m_pCurrentObjEvent->m_vPos, m_pCurrentObjEvent->m_nNextEventIndex);
						if(pObj)
						{
							D3DXVECTOR3 vPatternVel = pObj->m_vPos - m_pCurrentObjEvent->m_vPos; // ĆĐĹĎ ąćÇâ
							D3DXVec3Normalize(&vPatternVel,&vPatternVel);
							float fAngle = ACOS(D3DXVec3Dot(&m_vVel,&vPatternVel));	
							if( fAngle > 0 &&
								fAngle < PI/2 &&
								!m_pClientParty->IsFormationFlightMove())			// 2006-11-02 by ispark, Ćí´ë şńÇŕŔĚ¸é żöÇÁ ľČĹ˝
							{
								PatternWarpIn();
							}
						}
					}
				}
			}
			break;
		case EVENT_TYPE_LANDING_PATTERN_START:
			{				
				// ŔĚşĄĆ® żöÇÁ ĆĐĹĎ ˝şĹ¸Ć® ( Âř·ú˝Ă )
				CObjectChild* pObj = g_pScene->FindObjectByIndex(m_pCurrentObjEvent->m_nNextEventIndex);
				if(m_nCurrentPatternNumber == PATTERN_UNIT_AIRPORT_IN)	// żöÇÁľĆżô ĆĐĹĎ ÁßŔĚ¸é ĆĐĹĎŔĚ Áľ·áµČ´Ů.
				{
				}
				else if(pObj )
				{
					// ˝şĹµŔ» Ŕ§ÇŃ şŻĽö
					if(m_pCurrentObjEvent)
						m_pCurrentObjEventOld = m_pCurrentObjEvent;

					D3DXVECTOR3 vPatternVel = pObj->m_vPos - m_pCurrentObjEvent->m_vPos; // ĆĐĹĎ ąćÇâ
					D3DXVec3Normalize(&vPatternVel,&vPatternVel);
					float fAngle = ACOS(D3DXVec3Dot(&m_vVel,&vPatternVel));	
					if( fAngle > 0 &&
						fAngle < PI/2)	
					{
						m_bWarpLink = TRUE;
						m_nEventTypeAirPort = AIR_PORT_LANDING_PATTERN_START;
						D3DXVECTOR3 vPatternVel = pObj->m_vPos - m_pCurrentObjEvent->m_vPos; // ĆĐĹĎ ąćÇâ
						D3DXVec3Normalize(&vPatternVel,&vPatternVel);
						float fAngle = ACOS(D3DXVec3Dot(&m_vVel,&vPatternVel));
						m_fShuttleSpeed = AIR_PORT_MIN_SPEED;
						// 2005-08-05 by ispark
						g_pInterface->m_pGameMain->InitShowWindow();
						m_nBooster = BOOSTER_TYPE_MIN;
						CheckBoosterAni();
						
						D3DXVECTOR3 vVel,vUp;
						vVel = m_vPos - pObj->m_vPos;
						vUp.x = m_mMatrix._21;
						vUp.y = m_mMatrix._22;
						vUp.z = m_mMatrix._23;
						if(vVel == vUp)
						{// ¸ĹĆ®¸Ż˝ş°ˇ ±úÁöÁö ľĘµµ·Ď
							m_mMatrix._21 += 0.001f;
							m_mMatrix._22 -= 0.001f;
							m_mMatrix._23 += 0.002f;
						}
						m_mMatrix._31 = vVel.x;
						m_mMatrix._32 = vVel.y;
						m_mMatrix._33 = vVel.z;
						
						int nPattNum = PATTERN_UNIT_AIRPORT_IN + m_pCurrentObjEvent->m_sEventIndexFrom;
						if(InitCinemaUnit(nPattNum, FALSE) == TRUE)
						{
							m_bSkillMoveIsUse = TRUE;
							m_nCurrentPatternNumber = nPattNum ;
							InitCinemaCamera(PATTERN_CAMERA_AIRPORT_IN);
							m_bKeyBlock = TRUE;
						}
						else if(InitCinemaUnit(PATTERN_UNIT_AIRPORT_IN, FALSE) == TRUE)
						{
							m_bSkillMoveIsUse = TRUE;
							m_nCurrentPatternNumber = PATTERN_UNIT_AIRPORT_IN;
							InitCinemaCamera(PATTERN_CAMERA_AIRPORT_IN);
							m_bKeyBlock = TRUE;
						}
						else
						{
							DBGOUT("[PATTERN_UNIT_AIRPORT_IN] : Pattern File Non Exsistent.\n");
						}
					}
				}
			}
			break;
		case EVENT_TYPE_LANDING_PATTERN_END:
			{
				if(m_pCurrentObjEvent && TRUE == m_bWarpLink)
				{
					m_nEventTypeAirPort = AIR_PORT_LANDING_PATTERN_END;
					DeletePattern();
					ChangeUnitState( _LANDING );
					m_nEventIndex = m_pCurrentObjEvent->m_nNextEventIndex;
					m_bWarpLink = FALSE;
					m_bLandingField = TRUE;
				}
			}
			break;
		case EVENT_TYPE_TAKEOFF_PATTERN_START:
			{
				// ŔĚşĄĆ® żöÇÁ ĆĐĹĎ ˝şĹ¸Ć® ( ŔĚ·ú˝Ă )
				if(m_bWarpLink == TRUE)
				{
					CObjectChild* pObj = g_pScene->FindObjectByIndex(m_pCurrentObjEvent->m_nNextEventIndex);
					if(m_nCurrentPatternNumber == PATTERN_UNIT_AIRPORT_OUT)	// żöÇÁľĆżô ĆĐĹĎ ÁßŔĚ¸é ĆĐĹĎŔĚ Áľ·áµČ´Ů.
					{
					}
					else if(pObj )
					{
						//					ChangeUnitState( _LANDING );
						m_nEventTypeAirPort = AIR_PORT_TAKEOFF_PATTERN_START;
						D3DXVECTOR3 vPatternVel = pObj->m_vPos - m_pCurrentObjEvent->m_vPos; // ĆĐĹĎ ąćÇâ
						D3DXVec3Normalize(&vPatternVel,&vPatternVel);
						float fAngle = ACOS(D3DXVec3Dot(&m_vVel,&vPatternVel));
						m_fShuttleSpeed = m_fShuttleSpeedBoosterOn;
						
						// ¸ŢĽĽÁö Ăł¸®
						INIT_MSG_WITH_BUFFER(MSG_FC_CHARACTER_CHANGE_CHARACTER_MODE,
							T_FC_CHARACTER_CHANGE_CHARACTER_MODE, pSMode, SendBuf);
						pSMode->CharacterMode0 = FALSE;
						pSMode->PositionAVec3 = m_vPos;
						pSMode->TargetAVec3 = m_vVel*1000.0f;
						g_pFieldWinSocket->Write(SendBuf, MSG_SIZE(MSG_FC_CHARACTER_CHANGE_CHARACTER_MODE));
						
						// 2005-07-29 by ispark
						// Ăâąß ąŮµđÄÁµđĽÇ ş¸łż
						SendFieldSocketChangeBodyCondition(m_myShuttleInfo.ClientIndex, BODYCON_FLY_MASK|BODYCON_SET_OR_CLEAR_MASK);
						SendFieldSocketChangeBodyCondition(m_myShuttleInfo.ClientIndex, BODYCON_BOOSTER1_MASK|BODYCON_SET_OR_CLEAR_MASK);
						CheckBodyConditionToServer(m_myShuttleInfo.BodyCondition);
						m_nBooster = BOOSTER_TYPE_MIN;		// -1:ČÄÁřşÎ˝şĹÍ 0:ÁßÁö 1:±âş»şÎ˝şĹÍ 2:°ˇĽÓşÎ˝şĹÍ
						ChangeSingleBodyCondition(BODYCON_BOOSTER1_MASK);

						D3DXVECTOR3 vVel,vUp;
						vVel = m_vPos - pObj->m_vPos;
						vUp.x = m_mMatrix._21;
						vUp.y = m_mMatrix._22;
						vUp.z = m_mMatrix._23;
						if(vVel == vUp)
						{// ¸ĹĆ®¸Ż˝ş°ˇ ±úÁöÁö ľĘµµ·Ď
							m_mMatrix._21 += 0.001f;
							m_mMatrix._22 -= 0.001f;
							m_mMatrix._23 += 0.002f;
						}
						m_mMatrix._31 = vVel.x;
						m_mMatrix._32 = vVel.y;
						m_mMatrix._33 = vVel.z;
						
						// 2005-08-01 by ispark
						// ÁÂÇĄ ĽłÁ¤ żŔ·ů
						m_mMatrix._41 = m_vPos.x;
						m_mMatrix._42 = m_vPos.y;
						m_mMatrix._43 = m_vPos.z;


						int nPattNum = PATTERN_UNIT_AIRPORT_OUT + m_pCurrentObjEvent->m_sEventIndexFrom;
						if(InitCinemaUnit(nPattNum, FALSE) == TRUE)
						{
							m_bSkillMoveIsUse = TRUE;
							m_nCurrentPatternNumber = nPattNum ;

							// 2007-06-13 by bhsohn ľĆÄ«µĄąĚ¸Ę ˝Ăł×¸¶ Ăß°ˇ
							//InitCinemaCamera(PATTERN_CAMERA_AIRPORT_OUT);			// 2005-08-01 by ispark ŔÓ˝Ă·Î ľ˛´Â ąřČŁ
							int nCameraType = PATTERN_CAMERA_AIRPORT_OUT;
							// 2009. 10. 14 by jskim ÇÁ¸®˝şÄ« Á¦°Ĺ 
// 							if(IS_NORMAL_CITY_MAP_INDEX(g_pD3dApp->GetMyShuttleMapChannelIndex().MapIndex))
// 							{
// 								nCameraType = PATTERN_CAMERA_AIRPORT_NORMALCITY_OUT;
// 							}
							//end 2009. 10. 14 by jskim ÇÁ¸®˝şÄ« Á¦°Ĺ 
							InitCinemaCamera(nCameraType);			// 2005-08-01 by ispark ŔÓ˝Ă·Î ľ˛´Â ąřČŁ
							
							m_bKeyBlock = TRUE;
						}
						else if(InitCinemaUnit(PATTERN_UNIT_AIRPORT_OUT, FALSE) == TRUE)
						{
							m_bSkillMoveIsUse = TRUE;
							m_nCurrentPatternNumber = PATTERN_UNIT_AIRPORT_OUT;

							// 2007-06-13 by bhsohn ľĆÄ«µĄąĚ¸Ę ˝Ăł×¸¶ Ăß°ˇ
							//InitCinemaCamera(PATTERN_CAMERA_AIRPORT_OUT);			// 2005-08-01 by ispark
							int nCameraType = PATTERN_CAMERA_AIRPORT_OUT;
							// 2009. 10. 14 by jskim ÇÁ¸®˝şÄ« Á¦°Ĺ 
// 							if(IS_NORMAL_CITY_MAP_INDEX(g_pD3dApp->GetMyShuttleMapChannelIndex().MapIndex))
// 							{
// 								nCameraType = PATTERN_CAMERA_AIRPORT_NORMALCITY_OUT;
// 							}
							//end 2009. 10. 14 by jskim ÇÁ¸®˝şÄ« Á¦°Ĺ 
							InitCinemaCamera(nCameraType);		

							m_bKeyBlock = TRUE;
						}
						else
						{
							DBGOUT("[PATTERN_UNIT_AIRPORT_OUT] : Pattern File Non Exsistent.\n");
						}
					}
				}
			}
			break;
		case EVENT_TYPE_TAKEOFF_PATTERN_END:
			{
				if(m_bWarpLink == TRUE && m_nEventTypeAirPort != AIR_PORT_LANDING_PATTERN_START)
				{
					m_nEventTypeAirPort = AIR_PORT_TAKEOFF_PATTERN_END;
					DeletePattern();
					ChangeUnitState( _NORMAL );
					m_bWarpLink = FALSE;
					m_bKeyBlock = FALSE;										// 2005-10-26 by ispark
					// 2007-06-12 by bhsohn ąĚĽÇ ŔÎĹÍĆäŔĚ˝ş ĽöÁ¤ľČ
//					if(g_pGameMain->m_pMissionInfo->GetMissionMapIndex())
//					{
//						INIT_MSG_WITH_BUFFER(MSG_FC_QUEST_MOVE_QUEST_MAP, T_FC_QUEST_MOVE_QUEST_MAP,
//											pSQuestMove, SendBuf);
//						pSQuestMove->QuestIndex0 = g_pGameMain->m_pMissionInfo->GetMissionIndex()->QuestIndex;
//						g_pFieldWinSocket->Write(SendBuf, MSG_SIZE(MSG_FC_QUEST_MOVE_QUEST_MAP));
//					}	
					SendGoMissionMap();
				}
			}
			break;
		case EVENT_TYPE_TUTORIAL_GATE_POINT:
			if(g_pTutorial->IsTutorialMode() == TRUE && g_pTutorial->GetLesson() == L1)
			{
				g_pTutorial->MakeDisableTutorialGate(m_pCurrentObjEvent->m_vPos);
			}
			break;
		case EVENT_TYPE_OBJ_QUEST_OBJECT:
			{
				MSG_FC_EVENT_REQUEST_OBJECT_EVENT sMsg;
				sMsg.ObjectType		= m_pCurrentObjEvent->m_nCode;
				sMsg.ObjectPosition = m_pCurrentObjEvent->m_vPos;
				g_pFieldWinSocket->SendMsg( T_FC_EVENT_REQUEST_OBJECT_EVENT, (char*)&sMsg, sizeof(sMsg) );
				
				DBGOUT("EVENT_TYPE_OBJ_QUEST_OBJECT\n");
			}
			break;
		}
	}
}

void CShuttleChild::SetWeaponCountPrimary(INT nClientIndex, INT nCount)
{
	FLOG( "CShuttleChild::SetWeaponCountPrimary(INT nClientIndex, INT nCount)" );
	if(m_pPrimaryWeapon)
	{
		m_pPrimaryWeapon->SetWeaponCount( nCount );
	}
}

void CShuttleChild::SetWeaponCountSecondary(INT nClientIndex,INT nCount)
{
	FLOG( "CShuttleChild::SetWeaponCountSecondary(INT nClientIndex,INT nCont)" );
	if(m_pSecondaryWeapon)
	{
		m_pSecondaryWeapon->SetWeaponCount( nCount );
	}
}

void CShuttleChild::InitItemPoint()
{
	FLOG( "CShuttleChild::InitItemPoint()" );
	ITEM_GENERAL* pItem;
	DeleteItemPoint();
	// 2006-07-21 by ispark, Äł¸ŻĹÍżÍ ±âľî ŔĚĆĺĆ® ĂĘ±âČ­
	/*--------------------------------------------------------------------------*/
	// ±âľî ŔĚĆĺĆ®
	if(g_pD3dApp->m_bCharacter == FALSE)
	{
		
		// 2009. 08. 27 by ckPark ±×·ˇÇČ ¸®ĽŇ˝ş şŻ°ć ˝Ă˝şĹŰ ±¸Çö
// 		CreatePrimaryWeaponItem(m_pPrimaryWeapon ? m_pPrimaryWeapon->GetRealItemInfo() : NULL);
// 		CreateSecondaryWeaponItem(m_pSecondaryWeapon ? m_pSecondaryWeapon->GetRealItemInfo() : NULL);

		if( !m_pPrimaryWeapon )
			CreatePrimaryWeaponItem( NULL, NULL );
		else
		{
			// Ăß°ˇŔűŔÎ żÜÇüľĆŔĚĹŰ
			ITEM*	pShapeItem	= g_pDatabase->GetServerItemInfo( m_pPrimaryWeapon->GetItemGeneral()->GetShapeItemNum() );
			CreatePrimaryWeaponItem( m_pPrimaryWeapon->GetRealItemInfo(), pShapeItem  );
		}

		if( !m_pSecondaryWeapon )
			CreateSecondaryWeaponItem( NULL, NULL );
		else
		{
			// Ăß°ˇŔűŔÎ żÜÇüľĆŔĚĹŰ
			ITEM*	pShapeItem	= g_pDatabase->GetServerItemInfo( m_pSecondaryWeapon->GetItemGeneral()->GetShapeItemNum() );
			
			// 2009. 10. 21 by ckPark ¸Ę ŔĚµż˝Ă 2Çü ą«±âŔĺÂř ľČłŞżŔ´Â Çö»ó
			//CreatePrimaryWeaponItem( m_pSecondaryWeapon->GetRealItemInfo(), pShapeItem );
			CreateSecondaryWeaponItem( m_pSecondaryWeapon->GetRealItemInfo(), pShapeItem );
			// end 2009. 10. 21 by ckPark ¸Ę ŔĚµż˝Ă 2Çü ą«±âŔĺÂř ľČłŞżŔ´Â Çö»ó
		}
		// end 2009. 08. 27 by ckPark ±×·ˇÇČ ¸®ĽŇ˝ş şŻ°ć ˝Ă˝şĹŰ ±¸Çö

		pItem = m_pStoreData->FindItemInInventoryByWindowPos( POS_PROW );
		if( pItem )
		{
			CreateWearItem( WEAR_ITEM_KIND_PROW, EFFECT_NUMBER_WEAR_RADAR(pItem->ItemInfo->SourceIndex) );
		}
		pItem = m_pStoreData->FindItemInInventoryByWindowPos( POS_REAR );
		if(pItem)
		{
			CreateWearItem( WEAR_ITEM_KIND_ENGINE, pItem->ItemInfo->SourceIndex );
		}
	}
	/*--------------------------------------------------------------------------*/
	// Äł¸ŻĹÍ ŔĚĆĺĆ®
	else if(g_pD3dApp->m_bCharacter == TRUE)
	{
		// 2007-12-17 by bhsohn ŔüÁř ±âÁö ¸ĘżˇĽ­ ą«±â ľČłŞżŔ´Â Çö»óĂł¸®

		// 2009. 08. 27 by ckPark ±×·ˇÇČ ¸®ĽŇ˝ş şŻ°ć ˝Ă˝şĹŰ ±¸Çö
// 		if(m_pPrimaryWeapon)
// 		{
// 			CreatePrimaryWeaponItem(m_pPrimaryWeapon ? m_pPrimaryWeapon->GetRealItemInfo() : NULL);
// 		}		
// 
// 		if(m_pSecondaryWeapon)
// 		{
// 			CreateSecondaryWeaponItem(m_pSecondaryWeapon ? m_pSecondaryWeapon->GetRealItemInfo() : NULL);
// 		}		

		if( !m_pPrimaryWeapon )
			CreatePrimaryWeaponItem( NULL, NULL );
		else
		{
			// Ăß°ˇŔűŔÎ żÜÇüľĆŔĚĹŰ
			ITEM*	pShapeItem	= g_pDatabase->GetServerItemInfo( m_pPrimaryWeapon->GetItemGeneral()->GetShapeItemNum() );
			CreatePrimaryWeaponItem( m_pPrimaryWeapon->GetRealItemInfo(), pShapeItem );
		}
		
		if( !m_pSecondaryWeapon )
			CreateSecondaryWeaponItem( NULL, NULL );
		else
		{
			// Ăß°ˇŔűŔÎ żÜÇüľĆŔĚĹŰ
			ITEM*	pShapeItem	= g_pDatabase->GetServerItemInfo( m_pSecondaryWeapon->GetItemGeneral()->GetShapeItemNum() );

			// 2009. 10. 21 by ckPark ¸Ę ŔĚµż˝Ă 2Çü ą«±âŔĺÂř ľČłŞżŔ´Â Çö»ó
			//CreatePrimaryWeaponItem( m_pSecondaryWeapon->GetRealItemInfo(), pShapeItem );
			CreateSecondaryWeaponItem( m_pSecondaryWeapon->GetRealItemInfo(), pShapeItem );
			// end 2009. 10. 21 by ckPark ¸Ę ŔĚµż˝Ă 2Çü ą«±âŔĺÂř ľČłŞżŔ´Â Çö»ó
		}
		// end 2009. 08. 27 by ckPark ±×·ˇÇČ ¸®ĽŇ˝ş şŻ°ć ˝Ă˝şĹŰ ±¸Çö

		pItem = m_pStoreData->FindItemInInventoryByWindowPos( POS_PROW );
		if(pItem)
		{
			CreateWearItem( WEAR_ITEM_KIND_PROW, EFFECT_NUMBER_WEAR_RADAR(pItem->ItemInfo->SourceIndex) );
		}
		
		pItem = m_pStoreData->FindItemInInventoryByWindowPos( POS_REAR );
		if(pItem)
		{
			CreateWearItem( WEAR_ITEM_KIND_ENGINE, pItem->ItemInfo->SourceIndex );
		}
		// end 2007-12-17 by bhsohn ŔüÁř ±âÁö ¸ĘżˇĽ­ ą«±â ľČłŞżŔ´Â Çö»óĂł¸®
	}

	/*--------------------------------------------------------------------------*/
	// ŔüĂĽ ŔĚĆĺĆ®
	// 2006-07-21 by ispark, ŔĚĆĺĆ® Äł˝¬ ľĆŔĚĹŰ
	pItem = m_pStoreData->FindItemInInventoryByWindowPos( POS_WINGIN );
	if(pItem)
	{
		int nShapeNum;
		ITEM* pShapeItem = g_pDatabase->GetServerItemInfo(pItem->ShapeItemNum);
		ITEM* pShapeItem2 = g_pDatabase->GetServerItemInfo(pItem->ColorCode);
		if(pShapeItem)
			nShapeNum= pShapeItem->SourceIndex;
		else if(pShapeItem2)
			nShapeNum= pShapeItem2->SourceIndex;
		else
			nShapeNum = pItem->ItemInfo->SourceIndex;

		

		CreateWearItem( WEAR_ITEM_KIND_WINGIN, nShapeNum, g_pD3dApp->m_bCharacter );
	}

	pItem = m_pStoreData->FindItemInInventoryByWindowPos( POS_ACCESSORY_UNLIMITED );
	if(pItem)
	{
		CreateWearItem( WEAR_ITEM_KIND_ACCESSORY_UNLIMITED, pItem->ItemInfo->SourceIndex, g_pD3dApp->m_bCharacter );
	}
	pItem = m_pStoreData->FindItemInInventoryByWindowPos( POS_ACCESSORY_TIME_LIMIT );
	if(pItem)
	{
		CreateWearItem( WEAR_ITEM_KIND_ACCESSORY_TIME_LIMIT, pItem->ItemInfo->SourceIndex, g_pD3dApp->m_bCharacter );
	}
	// 2010-06-08 by dgwoo, Ćę˝Ă˝şĹŰ Ăß°ˇ. 
	pItem = m_pStoreData->FindItemInInventoryByWindowPos( POS_PET );
	if(pItem)
	{
		CreateWearItem( WEAR_ITEM_KIND_PET, GetPetManager()->GetSourceIndex_PetCurrentData( pItem->UniqueNumber , g_pD3dApp->m_bCharacter == TRUE ? true : false ) , g_pD3dApp->m_bCharacter );
	}										
	// 2010-06-08 by dgwoo, Ćę˝Ă˝şĹŰ Ăß°ˇ. 


	/*--------------------------------------------------------------------------*/
	pItem = m_pStoreData->FindItemInInventoryByWindowPos( POS_CENTER );
	// żěĽ± ±âĂĽżˇ °üÇŃ °ÍŔĚąÇ·Î FALSE·Î ĽłÁ¤
	// 2005-08-23 by ispark

	// 2009. 08. 27 by ckPark ±×·ˇÇČ ¸®ĽŇ˝ş şŻ°ć ˝Ă˝şĹŰ ±¸Çö
	//ChangeUnitCharacterInfo((pItem?pItem->ItemInfo->SourceIndex:0), m_myShuttleInfo.UnitKind, m_myShuttleInfo.BodyCondition, FALSE );

	int nDefenseNum = 0;

	if( !pItem )
		nDefenseNum = 0;
	else
	{
		if (!pItem->ShapeItemNum) {
			nDefenseNum = pItem->ItemInfo->SourceIndex;
		}
		else
		{
			ITEM* pShapeItem = g_pDatabase->GetServerItemInfo( pItem->ShapeItemNum );
			if( pShapeItem )
				nDefenseNum = pShapeItem->SourceIndex;
			else
				nDefenseNum = pItem->ItemInfo->SourceIndex;
		}
		if (pItem->ColorCode && !pItem->ShapeItemNum)
		{
			ITEM* pShapeItem2 = g_pDatabase->GetServerItemInfo(pItem->ColorCode);
			if (pShapeItem2)
			{
				nDefenseNum = pShapeItem2->SourceIndex;
			}
		}
	}
	// 2010. 03. 18 by jskim ¸ó˝şĹÍşŻ˝Ĺ Ä«µĺ
	//ChangeUnitCharacterInfo( nDefenseNum, m_myShuttleInfo.UnitKind, m_myShuttleInfo.BodyCondition, FALSE );
	BOOL bMonTras = FALSE;

	if( m_MonsterTransformer )
	{
		ChangeUnitCharacterInfo(nDefenseNum, m_myShuttleInfo.UnitKind, m_myShuttleInfo.BodyCondition, FALSE);
		SetMonsterTransPrimarySidePos(m_PrimaryAttack.vSidePos);
		SetMonsterTransSecondarySidePos(m_SecondaryAttack.vSidePos);
	}

	if( m_MonsterTransformer )
	{
		bMonTras = TRUE;
		MEX_MONSTER_INFO * pTempIn = g_pGameMain->CheckMonsterInfo( m_MonsterTransformer );
		if(pTempIn)
			nDefenseNum = pTempIn->SourceIndex;
	}
	ChangeUnitCharacterInfo( nDefenseNum, m_myShuttleInfo.UnitKind, m_myShuttleInfo.BodyCondition, FALSE, bMonTras );
	//end 2010. 03. 18 by jskim ¸ó˝şĹÍşŻ˝Ĺ Ä«µĺ
	// end 2009. 08. 27 by ckPark ±×·ˇÇČ ¸®ĽŇ˝ş şŻ°ć ˝Ă˝şĹŰ ±¸Çö
}
/*
///////////////////////////////////////////////////////////////////////////////
/// \fn			void CShuttleChild::CheckPrimaryAttack(BOOL bReset, ITEM* pPrimary)
/// \brief		Primary weaponŔÇ actionŔ» ĽöÇŕÇŃ´Ů.
/// \author		dhkwon
/// \date		2004-03-21 ~ 2004-03-21
/// \warning	CUnitDataŔÇ PrimaryAttackActionÇÔĽö¸¦ żŔąö¶óŔĚµů(virtual function)ÇĎż© Ŕç±¸Ľş
///				((CUnitData*)p)->PrimaryAttackActionŔ» ĽöÇŕÇĎ´ő¶óµµ ŔÚ˝ÄŔÇ °ÍŔĚ ĽöÇŕµĘ
///				ŔĚ şÎşĐŔş CEnemyData,CShuttleChild°Łżˇ ľŕ°ŁŔÇ Â÷ŔĚ°ˇ ŔÖľî µű·Î ±¸ÇöµÇľú´Ů.
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CShuttleChild::CheckPrimaryAttack(BOOL bReset, ITEM* pPrimary)
{
	FLOG( "CShuttleChild::CheckPrimaryAttack(BOOL bReset)" );
	if(m_pPrimaryAttackTarget && 
		(!m_pPrimaryAttackTarget->m_bUsing || 
		ATTACK_AVAILABLE_STATE(m_pPrimaryAttackTarget->m_dwState) == FALSE))
//		m_pPrimaryAttackTarget->m_dwState != _NORMAL))
	{
		m_pPrimaryAttackTarget = NULL;	// µĄŔĚĹ¸°ˇ ÁöżöÁú °ÍŔÎÁö ĂĽĹ©ÇĎż© Ĺ¸°ŮĆ÷ŔÎĹÍ¸¦ NULLĽÂĆĂ
	}

	if( m_pPrimaryWeapon && m_pPrimaryWeapon->GetRealItemInfo() )
	{
		CUnitData::CheckPrimaryAttack( bReset, m_pPrimaryWeapon->GetRealItemInfo() );
	}
}
*/

// 2009. 08. 27 by ckPark ±×·ˇÇČ ¸®ĽŇ˝ş şŻ°ć ˝Ă˝şĹŰ ±¸Çö
//void CShuttleChild::CheckAttack(ATTACK_DATA &attackData, BOOL bReset, ITEM* pWeaponITEM)
void CShuttleChild::CheckAttack( ATTACK_DATA &attackData, BOOL bReset, ITEM* pWeaponITEM /* = NULL */, ITEM* pEffectItem /* = NULL */ )
// end 2009. 08. 27 by ckPark ±×·ˇÇČ ¸®ĽŇ˝ş şŻ°ć ˝Ă˝şĹŰ ±¸Çö
{
	if(IS_PRIMARY_ATT_TYPE(attackData.AttackData.AttackType))
	{
		if(m_pPrimaryWeapon)
		{
			// 2009. 08. 27 by ckPark ±×·ˇÇČ ¸®ĽŇ˝ş şŻ°ć ˝Ă˝şĹŰ ±¸Çö
			//CUnitData::CheckAttack( m_PrimaryAttack, bReset, m_pPrimaryWeapon->GetRealItemInfo() );

			// Ăß°ˇŔűŔÎ ŔĚĆĺĆ® ľĆŔĚĹŰ
			ITEM* pEffectItem = g_pDatabase->GetServerItemInfo(	m_pPrimaryWeapon->GetItemGeneral()->ColorCode );
			CUnitData::CheckAttack( m_PrimaryAttack, bReset, m_pPrimaryWeapon->GetRealItemInfo(), pEffectItem );
			// end 2009. 08. 27 by ckPark ±×·ˇÇČ ¸®ĽŇ˝ş şŻ°ć ˝Ă˝şĹŰ ±¸Çö
		}
	}
	else if(IS_SECONDARY_ATT_TYPE(attackData.AttackData.AttackType))
	{
		if(m_pSecondaryWeapon)
		{
			// 2009. 08. 27 by ckPark ±×·ˇÇČ ¸®ĽŇ˝ş şŻ°ć ˝Ă˝şĹŰ ±¸Çö
			//CUnitData::CheckAttack( m_SecondaryAttack, bReset, m_pSecondaryWeapon->GetRealItemInfo() );

			// Ăß°ˇŔűŔÎ ŔĚĆĺĆ® ľĆŔĚĹŰ
			ITEM* pEffectItem = g_pDatabase->GetServerItemInfo(	m_pSecondaryWeapon->GetItemGeneral()->ColorCode );
			CUnitData::CheckAttack( m_SecondaryAttack, bReset, m_pSecondaryWeapon->GetRealItemInfo(), pEffectItem );
			// end 2009. 08. 27 by ckPark ±×·ˇÇČ ¸®ĽŇ˝ş şŻ°ć ˝Ă˝şĹŰ ±¸Çö
		}
	}
	// 2010-06-15 by shcho&hslee Ćę˝Ă˝şĹŰ - Ćę °ř°Ý Ăł¸®
	else if(ATT_TYPE_PET_GENERAL == attackData.AttackData.AttackType)
	{
		if(GetPetManager()->GetWeaponItemInfo())
		{
			CUnitData::CheckAttack( m_SecondaryAttack, bReset, m_pSecondaryWeapon->GetRealItemInfo() );
		}
	}
	// end 2010-06-15 by shcho&hslee Ćę˝Ă˝şĹŰ - Ćę °ř°Ý Ăł¸®
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			CShuttleChild::CreateSecondaryShieldDamage(D3DXVECTOR3 vCollPos)
/// \brief		˝Żµĺ µĄąĚÁö°ˇ Ľ­ąöżˇĽ­ żÔŔ» °ćżě ŔĚĆĺĆ® »ýĽş
/// \author		dhkwon
/// \date		2004-05-18 ~ 2004-05-18
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CShuttleChild::CreateSecondaryShieldDamage(D3DXVECTOR3 vCollPos)
{
//	ITEM_GENERAL *pItem = g_pStoreData->FindItemInInventoryByWindowPos(POS_WINGOUT);
//	if(!pItem || pItem->Kind != ITEMKIND_SHIELD)
//	{
//		pItem = g_pStoreData->FindItemInInventoryByWindowPos(POS_WINGIN);
//	}
//	if(pItem && pItem->Kind == ITEMKIND_SHIELD)
//	{
		//CWeaponShieldData* pShield = new CWeaponShieldData(this, pItem->ItemNum, vCollPos );
		CWeaponShieldData* pShield = new CWeaponShieldData(this, RC_EFF_SHIELD_HIT, vCollPos );
		g_pScene->m_pWeaponData->AddChild(pShield);
//	}
}


void CShuttleChild::SetShuttleFlight()
{
	FLOG( "CShuttleChild::SetShuttleFlight()" );
	if(g_pD3dApp->GetHwnd()!=GetActiveWindow())
		return;

	SetCursorInit();
	m_bIsSetFlighting = TRUE;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			void CShuttleChild::Move_AirSiege(float fElapsedTime)
/// \brief		żˇľî ˝ĂÁî¸¸Ŕ» µű·Î Ăł¸®ÇŃ´Ů.
/// \author		dgwoo
/// \date		2006-11-21 ~ 2006-11-21
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CShuttleChild::Move_AirSiege(float fElapsedTime)
{
	D3DXVec3Normalize(&m_vVel,&m_vVel);
	D3DXVec3Normalize(&m_vUp,&m_vUp);
	D3DXVec3Cross(&m_vSideVel,&m_vUp,&m_vVel);
	m_fAdvanceTime -= fElapsedTime;
	if(m_fAdvanceTime <= 0.0f)
		m_fAdvanceTime = 0.0f;
	
	m_bLandedMove = FALSE;
	m_bIsAir = TRUE;
	int nSideMove = 0;	
	m_nBooster = BOOSTER_TYPE_STOP;
	CheckMoveRate(fElapsedTime,nSideMove);
	if(m_myShuttleInfo.CurrentEP)
		m_vPos += (m_vNextPos - m_vPos)*fElapsedTime;
	else 
		m_vPos += 0.5f * (m_vNextPos - m_vPos) * fElapsedTime;
	CheckCollForMap();
	CheckCollForObject(fElapsedTime);
	// 2007-03-14 by dgwoo żˇľî ˝ĂÁî˝Ă ĆňÇŕŔ» ŔŻÁöÇĎÁö ľĘ±â·Î şŻ°ć.
	//SetShuttleFlightInit(fElapsedTime);
	if(	(g_pSOption->sMenuBalance == TRUE ||
		g_pGameMain->m_pSystem->m_bQuitGameMessage == TRUE)&&			// ¸Ţ´ş»çżë˝Ă ŔŻ´ÖÁ¤Áö żÉĽÇ ĂĽĹ©
		(g_pD3dApp->m_bMessageBox == TRUE || 
		g_pD3dApp->m_bFixUnitDirMode == TRUE) )
	{// ŔÎĹÍĆäŔĚ˝ş Ă˘ŔĚ ¶°ŔÖ´Â °ćżě ±âľî¸¦ Ľ­Ľ­Č÷ °¨ĽÓÇŃ´Ů.
		m_nShuttleStopState = 4;
	}
	CheckBoosterAni();
	CheckMapRegion();
	int nXCheck,nYCheck;
	nXCheck = (int)(m_vPos.x/TILE_SIZE);
	nYCheck = (int)(m_vPos.z/TILE_SIZE);

	if (nXCheck < 0)
		nXCheck = 0;

	if (nYCheck < 0)
		nYCheck = 0;

	m_bMoveGroundType = g_pGround->m_pTileInfo[nXCheck*g_pGround->m_projectInfo.sXSize+nYCheck].bMove;
	if( m_dwState == _FALLEN || 
		m_dwState == _EXPLODED || 
//		g_pGameMain->IsShowing() ||				// 2006-11-07 by ispark, Ă˘ŔĚ ¶°µµ żňÁ÷ŔĚ°Ô ĽöÁ¤
		m_bOrderMove || 
		m_bMouseLock || 
		//g_pD3dApp->m_bChatMode ||				// 2008-09-02 by dgwoo Ă¤ĆĂÁßżˇµµ ¸¶żě˝ş ÄÁĆ®·Ń °ˇ´ÉÇĎ°Ô.
		g_pD3dApp->m_bFixUnitDirMode == TRUE)	// ŔŻ´Ö ÁřÇŕ ąćÇâ °íÁ¤ ¸đµĺ
	{
		m_fLRVelRate = 0.0f;
		m_fSideRate = 0.0f;
	}

	SetMatrix_Move_Ground(fElapsedTime);//	SetEffectPos_B_Gear(fElapsedTime);
	SiegeTarget();

	// 2010-12-21 by hsson Ćę˝Ă˝şĹŰ - ˝ĂÁî ¸đµĺ˝Ă Ćę Ľű±â±â 
	// 2010-06-15 by shcho&hslee Ćę˝Ă˝şĹŰ - ˝ĂÁî ¸đµĺ˝Ă Ćę Ľű±â±â 
	//HidePet(TRUE);
	// end 2010-06-15 by shcho&hslee Ćę˝Ă˝şĹŰ - ˝ĂÁî ¸đµĺ˝Ă Ćę Ľű±â±â 
	// end 2010-12-21 by hsson Ćę˝Ă˝şĹŰ - ˝ĂÁî ¸đµĺ˝Ă Ćę Ľű±â±â
	
}
void CShuttleChild::ObserveUpdateGage(MSG_FC_CHARACTER_OBSERVER_INFO *pMsg)
{
	m_myShuttleInfo.HP = pMsg->HP;
	m_myShuttleInfo.SP = pMsg->SP;
	m_myShuttleInfo.DP = pMsg->DP;
	m_myShuttleInfo.EP = pMsg->EP;
	if(pMsg->CurrentHP < 1)
		m_myShuttleInfo.CurrentHP = 1;
	else
		m_myShuttleInfo.CurrentHP = pMsg->CurrentHP;

	if(pMsg->CurrentSP < 1)
		m_myShuttleInfo.CurrentSP = 1;
	else
		m_myShuttleInfo.CurrentSP = pMsg->CurrentSP;

	if(pMsg->CurrentDP < 1)
		m_myShuttleInfo.CurrentDP = 1;
	else
		m_myShuttleInfo.CurrentDP = pMsg->CurrentDP;

	if(pMsg->CurrentEP < 1)
		m_myShuttleInfo.CurrentEP = 1;
	else
		m_myShuttleInfo.CurrentEP = pMsg->CurrentEP;
}

void CShuttleChild::Move_Observe()
{
	D3DXVec3Normalize(&g_pShuttleChild->m_vVel,&m_stObserve.vObserveLook);
	g_pShuttleChild->m_vUp				= m_stObserve.vObserveUp;
	g_pShuttleChild->m_fOldUpRate		= 0;
	g_pShuttleChild->m_vPos				= m_stObserve.vObservePos;
	g_pShuttleChild->m_vNextPos			= m_stObserve.vObserveNextPos;
	g_pShuttleChild->m_vSideVel			= m_stObserve.vObserveSide;
	return;
}

void CShuttleChild::Observe_PosSeting()
{
	m_stObserve.vObserveLook = m_stObserve.pEnemyData->m_vVel;
	m_stObserve.vObservePos = m_stObserve.pEnemyData->m_vPos;
	m_stObserve.vObserveSide = m_stObserve.pEnemyData->m_vSideVel;
	// 2007-03-27 by bhsohn żÉŔúąö ¸đµĺ ľ÷ş¤ĹÍ °íÁ¤
	m_stObserve.vObserveUp = D3DXVECTOR3(0.0f, 1.0f, 0.0f);			
	m_stObserve.vObserveNextPos = m_stObserve.pEnemyData->m_vNextPos;
}


void CShuttleChild::Move_Air(float fElapsedTime)	//void CShuttleChild::Move_B_Gear(float fElapsedTime)
{
	char message[256];
	CheckRollKeyDown(fElapsedTime);						// 2005-07-05 by ispark

	// 2007-03-20 by dgwoo żÉŔúąö
	if(IsOperAndObser() && IsObserverMode())
	{
		Observe_PosSeting();
		Move_Observe();
	}
	//	if(m_bFormationFlight)
	if(m_pClientParty->IsFormationFlightMove())
	{
//		DBGOUT("Ćí´ë\n");
		m_pClientParty->SetMasterInfo();
		m_pClientParty->FormationFlightMoveForShuttleChild();
		m_pClientParty->FormationFlightMoveForEnemy();

		// 2005-11-21 by ispark
		//  Ćí´ëşńÇŕ˝Ă Ĺ¸°ŮŔ» Ŕ§ÇŃ şńÇŕ Č¸Ŕü
		SetFormationMove(fElapsedTime);

		if(D3DXVec3Length(&(m_vNextPos - m_vPos)) > 1000.0f ||
			m_pClientParty->GetPartyInfo().bFormationFlyingType == FLIGHT_FORM_NONE
#ifdef _INET_BREAK_FORM_FLIGHT_WHEN_SIEGE
			|| m_bAttackMode == _SIEGE //18.12.2025 Inet - when using siege mode exclude from form flight
#endif
			) // ŔÚŔŻşńÇŕŔş ÇŘÁ¦
		{
			SendPacket(T_IC_PARTY_CANCEL_FLIGHT_POSITION);
			m_pClientParty->FormationFlightClear();
//			m_infoParty.bFormationFlyingPositionType = 0;
//			m_bFormationFlight = FALSE; // Ćí´ë şńÇŕŔ» ÄË˝˝ÇĎ°í ş»ŔÎŔÇ ŔÇÁö´ë·Î şńÇŕÇŃ´Ů.
			wsprintf(message,STRMSG_C_PARTY_0001);//"Ćí´ëşńÇŕżˇĽ­ ąţľîłµ˝Ŕ´Ď´Ů."
			g_pD3dApp->m_pChat->CreateChatChild(message,COLOR_SYSTEM);

			// ÇŘÁ¦˝Ă ąćÇâ°Ş ĂĘ±âČ­
			m_fCurrentSideRate = 0.0f;
			m_fCurrentAngle = 0.0f;
		}
	}
	else if(m_bAttackMode == _SIEGE)
	{// 2006-11-20 by dgwoo ˝ĂÁî ¸đµĺ·Î şŻ°ć˝Ă.
		// 2010-12-21 by hsson Ćę˝Ă˝şĹŰ - ˝ĂÁî ¸đµĺ˝Ă Ćę Ľű±â±â 
		HidePet(TRUE);
		// end 2010-12-21 by hsson Ćę˝Ă˝şĹŰ - ˝ĂÁî ¸đµĺ˝Ă Ćę Ľű±â±â 

		Move_AirSiege(fElapsedTime);

		// 2008-03-04 by bhsohn ˝ĂÁî¸đµĺČÄ °ÔŔÓÁľ·á¸é ¸ŘĂß´Â Çö»óĂł¸®
		if((m_dwState == _WARP)
			&& (EVENT_GAME_END  == m_nEventType))
		{
			SetSelectCharacterItem();
			g_pGameMain->m_pSystem->SendFieldSocketCharacterGameEnd();
		}
		// end 2008-03-04 by bhsohn ˝ĂÁî¸đµĺČÄ °ÔŔÓÁľ·á¸é ¸ŘĂß´Â Çö»óĂł¸®
		return ;
	}
	else
	{
		// 2010-12-21 by hsson Ćę˝Ă˝şĹŰ - ˝ĂÁî ¸đµĺ˝Ă Ćę Ľű±â±â 
		HidePet(FALSE);
		// end 2010-12-21 by hsson Ćę˝Ă˝şĹŰ - ˝ĂÁî ¸đµĺ˝Ă Ćę Ľű±â±â 

		// 2005-08-10 by ispark
		// ŔŻ´ÖÁ¤Áö·Î ŔÎÇĎż© Ŕ§Äˇ¸¦ ąŮ˛Ű´Ů.
		D3DXVec3Normalize(&m_vVel,&m_vVel);
		D3DXVec3Normalize(&m_vUp,&m_vUp);
		D3DXVec3Cross(&m_vSideVel,&m_vUp,&m_vVel);

		// 2005-07-05 by ispark
		// ·Ń¸µ ˝Ă˝şĹŰ »çżë˝Ă ¸¶żě˝ş żňÁ÷ŔÓŔ» ¸·´Â´Ů.
		// 2010. 12. 06. by jskim Ŕ©µµżě şńČ°ĽşČ­ ŔĎ¶§ ¸¶żě˝ş Ăł¸® ľČÇĎ°Ô ÇÔ.
		//if(!m_bMouseMoveLock)
		if(!m_bMouseMoveLock && g_pD3dApp->m_bHasFocus)
		// end 2010. 12. 06. by jskim Ŕ©µµżě şńČ°ĽşČ­ ŔĎ¶§ ¸¶żě˝ş Ăł¸® ľČÇĎ°Ô ÇÔ.
		{
			D3DXMATRIX  matTemp;
			int nx,ny;
			nx = g_pD3dApp->GetBackBufferDesc().Width/2;
			ny = g_pD3dApp->GetBackBufferDesc().Height/2;

			// 2005-11-28 by ispark, żŁÁř ľĆŔĚĹŰ °ˇÁ®żŔ±â
			ITEM* pItemInfo = g_pStoreData->GetItemInfoFromItemGeneral(g_pStoreData->FindItemInInventoryByWindowPos( POS_REAR ));

			// Ćň¸é ąćÁ¤˝ÄŔ» ±¸ÇŃ´Ů.
			// a*x + b*y + c*z + d = 0 << Ćň¸é ąćÁ¤˝Ä
			float fTemp = -((m_vUp.x*m_vMouseDir.x + m_vUp.y*m_vMouseDir.y + m_vUp.z*m_vMouseDir.z)/(m_vUp.x*m_vUp.x + m_vUp.y*m_vUp.y + m_vUp.z*m_vUp.z));
			D3DXVECTOR3 vNew;
			vNew.x = m_vUp.x*fTemp + m_vMouseDir.x;
			vNew.y = m_vUp.y*fTemp + m_vMouseDir.y;
			vNew.z = m_vUp.z*fTemp + m_vMouseDir.z;
			D3DXVec3Normalize(&vNew,&vNew);
			float fAngle = (float)(AIR_UPDOWNRATE_MAX * m_ptDir.y) / ny;
			if(fAngle > AIR_UPDOWNRATE_MAX)
				fAngle = AIR_UPDOWNRATE_MAX;
			else if(fAngle < -AIR_UPDOWNRATE_MAX)
				fAngle = -AIR_UPDOWNRATE_MAX;

			if( abs(ny - m_pt.y) > 3*((float)g_pD3dApp->GetBackBufferDesc().Height/(float)600) && 
				m_dwState == _NORMAL && 
				m_nBooster != BOOSTER_TYPE_AIRBREAK && 
//				!g_pGameMain->IsShowing() &&				// 2006-11-07 by ispark, Ă˘ŔĚ ¶°µµ żňÁ÷ŔĚ°Ô ĽöÁ¤
				!m_bMouseLock && !m_bOrderMove && 
				//!g_pD3dApp->m_bChatMode &&				// 2008-09-02 by dgwoo Ă¤ĆĂÁßżˇµµ ¸¶żě˝ş ÄÁĆ®·Ń °ˇ´ÉÇĎ°Ô.
				g_pD3dApp->m_bFixUnitDirMode == FALSE)		// ŔŻ´Ö ÁřÇŕ ąćÇâ °íÁ¤ ¸đµĺ
			{
				// 2005-09-06 by ispark
				// Ľ±Č¸ÇŇ Ľö ŔÖ´Â ĂÖ´ë ĽöÄˇ·Î ÁˇÁˇ Áő°ˇ
				// 2005-11-28 by ispark, ÇĎŔĚĆŰą«şů ˝şĹł Ăß°ˇ
				float fSpeedPenalty = 0.0f;
				if(pItemInfo)
				{
					fSpeedPenalty = CAtumSJ::GetCircularAngle(pItemInfo, &m_paramFactor);
				}
//				m_fCurrentAngle += m_bySpeedPenalty * fElapsedTime * fAngle * 0.1f;
				m_fCurrentAngle += fSpeedPenalty * fElapsedTime * fAngle * 0.1f;
				if(fAngle > 0 && m_fCurrentAngle > fAngle)
					m_fCurrentAngle = fAngle;
				else if(fAngle < 0 && m_fCurrentAngle < fAngle)
					m_fCurrentAngle = fAngle;
					D3DXMatrixRotationAxis(&matTemp,&m_vSideVel, m_fMouseRate*m_fCurrentAngle*fElapsedTime);
					D3DXVec3TransformCoord(&m_vVel,&m_vVel, &matTemp );
			}
			float fSideRateTemp = m_fSideRate;
			m_fSideRate = (float)(AIR_SIDERATE_MAX * m_ptDir.x) / nx;
			if(m_fSideRate > AIR_SIDERATE_MAX)
				m_fSideRate = AIR_SIDERATE_MAX;
			else if(m_fSideRate < -AIR_SIDERATE_MAX)
				m_fSideRate = -AIR_SIDERATE_MAX;
			
			if( abs(nx - m_pt.x) > 3*((float)g_pD3dApp->GetBackBufferDesc().Width/(float)800) && 
				(m_dwState == _NORMAL || m_dwState == _LANDING) && 
				!g_pD3dApp->m_pCamera->m_bIsCamControl && 
//				!g_pGameMain->IsShowing() &&				// 2006-11-07 by ispark, Ă˘ŔĚ ¶°µµ żňÁ÷ŔĚ°Ô ĽöÁ¤ 
				!m_bMouseLock && !m_bOrderMove &&
				//!g_pD3dApp->m_bChatMode  &&					// 2008-09-02 by dgwoo Ă¤ĆĂÁßżˇµµ ¸¶żě˝ş ÄÁĆ®·Ń °ˇ´ÉÇĎ°Ô.
				 g_pD3dApp->m_bFixUnitDirMode == FALSE)		// ŔŻ´Ö ÁřÇŕ ąćÇâ °íÁ¤ ¸đµĺ
			{
				// 2005-09-06 by ispark
				// Ľ±Č¸ÇŇ Ľö ŔÖ´Â ĂÖ´ë ĽöÄˇ·Î ÁˇÁˇ Áő°ˇ
				// 2005-11-28 by ispark, ÇĎŔĚĆŰą«şů ˝şĹł Ăß°ˇ
				float fSpeedPenalty = 0.0f;
				if(pItemInfo)
				{
					fSpeedPenalty = CAtumSJ::GetCircularAngle(pItemInfo, &m_paramFactor);
				}
//				m_fCurrentSideRate += m_bySpeedPenalty * fElapsedTime * m_fSideRate * 0.1f;
				m_fCurrentSideRate += fSpeedPenalty * fElapsedTime * m_fSideRate * 0.1f;
				if(m_fSideRate > 0 && m_fCurrentSideRate > m_fSideRate)
					m_fCurrentSideRate = m_fSideRate;
				else if(m_fSideRate < 0 && m_fCurrentSideRate < m_fSideRate)
					m_fCurrentSideRate = m_fSideRate;
				
				D3DXMatrixRotationAxis( &matTemp, &m_vUp, m_fMouseRate*m_fCurrentSideRate*fElapsedTime);
				D3DXVec3TransformCoord( &m_vVel, &m_vVel, &matTemp );
			}	
			//
			else if(!m_bIsSetFlighting && g_pSOption->sAutoBalance == FALSE)
			{// ĽöĆň ŔŻÁöÁßŔĚ ľĆ´Ň¶§¸¸-ÁÂżě Č¸Ŕü ą× ±âżďŔÓŔĚľřŔ»¶§ ĽöĆň ŔÚµż ŔŻÁö µÇµµ·Ď ÇĎŔÚ(ÁÂżě ĽöĆň¸¸)-Ľ­Ľ­Č÷
				SetShuttleFlightInit(fElapsedTime, false);
 			}

			
			// 2005-02-23 by jschoi - ŔÚµż ĽöĆň ŔŻÁö żÉĽÇ Ăß°ˇ
			if(g_pSOption->sAutoBalance == TRUE)		// ŔÚµż ĽöĆň ŔŻÁö żÉĽÇ ĂĽĹ©
			{
				// 2005-01-27 by jschoi - ĽöĆň ŔŻÁö¸¦ Ŕ§ÇŘ »ő·Îżî ľË°í¸®Áň Ŕűżë(B±âľî, I±âľî)
// 2007-08-07 by dgwoo ÇŘ»óµµ°ˇ Ĺ« PCżˇĽ± Č¸Ŕü°˘ŔĚ ´ő ÄżÁř´Ů.
//				float fDistanceToCenter = D3DXVec2Length(&(D3DXVECTOR2(nx - m_pt.x,ny - m_pt.y)));	// ¸¶żě˝ş Ć÷ŔÎĹÍżÍ Áß˝É°úŔÇ °Ĺ¸®
//				float fDistanceToMaxView = D3DXVec2Length(&(D3DXVECTOR2(nx,ny)));					// Č­¸é łˇ°ú Áß˝É°úŔÇ °Ĺ¸®
//				float fAutoRecoveryShuttle = (1.0f - fDistanceToCenter/fDistanceToMaxView);			// ĽöĆň ŔÚµż ŔŻÁö Á¤µµ Á¶Á¤
//				SetShuttleFlightInit(fElapsedTime * fAutoRecoveryShuttle , false);
				SetShuttleFlightInit(fElapsedTime, false);
			}

			if(m_bIsSetFlighting)
			{
				if((abs(ny - m_pt.y) > 7 || abs(nx - m_pt.x) > 7) && 
					m_dwState == _NORMAL &&
					m_nBooster != BOOSTER_TYPE_AIRBREAK)
				{
					m_bIsSetFlighting = FALSE;
				}
			}
			if(m_bIsSetFlighting)
				m_fSideRate = fSideRateTemp;
		}	// 2005-07-05 by ispark		// ż©±â±îÁö ¸¶żě˝ş żňÁ÷ŔÓ ¸·±â

		D3DXVec3Cross(&m_vSideVel,&m_vUp,&m_vVel);
		D3DXVec3Cross(&m_vUp,&m_vVel,&m_vSideVel);

		if(m_pClientParty->IsFormationFlight())
		{		
			m_pClientParty->SetMasterInfo();
			m_pClientParty->FormationFlightMoveForEnemy();
		}
	}

	if(m_dwState==_NORMAL)
	{
		// 2006-01-06 by ispark, AdvanceTime
		m_fAdvanceTime -= fElapsedTime;
		if(m_fAdvanceTime <= 0.0f)
			m_fAdvanceTime = 0.0f;
		
		m_bLandedMove = FALSE;
		m_bIsAir = TRUE;
		int nSideMove = 0;	
		
		if(!g_pD3dApp->m_bChatMode && m_bReflexionKeyBlock == FALSE &&
			g_pD3dApp->m_bFixUnitDirMode == FALSE &&					// ŔŻ´Ö ÁřÇŕ ąćÇâ °íÁ¤ ¸đµĺ
			m_bUnitStop == FALSE &&
			(g_pTutorial->IsTutorialMode() == FALSE || g_pTutorial->GetLesson() >= L2) )	// Ć©Ĺä¸®ľó ·ą˝Ľ 2şÎĹÍ
		{
			int nOldBoosterType = m_nBooster;
			m_nBooster = BOOSTER_TYPE_MIN;		// -1:ČÄÁřşÎ˝şĹÍ 0:ÁßÁö 1:±âş»şÎ˝şĹÍ 2:°ˇĽÓşÎ˝şĹÍ
			bool bFormationFlight = true;
			if(g_pTutorial->IsTutorialMode() == FALSE || g_pTutorial->GetLesson() >= L3)	// Ć©Ĺä¸®ľó ·ą˝Ľ 3şÎĹÍ
			{
				// 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
				// 2010. 03. 26 by jskim Ăâ°Ý Áß Ĺ°ş¸µĺ Ĺ¬¸Ż ¸řÇĎµµ·Ď ĽöÁ¤
				//if(g_pD3dApp->m_pKeyBoard->GetAsyncKeyState(DIK_D) || m_bKeyDownD || m_bRightButtonState)
				//if(g_pD3dApp->GetAsyncKeyState_DIK_DIJ(DIK_D) || m_bKeyDownD || m_bRightButtonState)
				if((g_pD3dApp->GetAsyncKeyState_DIK_DIJ(DIK_D) || m_bKeyDownD || m_bRightButtonState) && !IS_CITY_MAP_INDEX(m_myShuttleInfo.MapChannelIndex.MapIndex))
				//end 2010. 03. 26 by jskim Ăâ°Ý Áß Ĺ°ş¸µĺ Ĺ¬¸Ż ¸řÇĎµµ·Ď ĽöÁ¤
				{// żě·Î ŔĚµż
					nSideMove++;
					bFormationFlight = false;
					g_pTutorial->m_Lesson3State.bRMove = TRUE;
				}
				// 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
				// 2010. 03. 26 by jskim Ăâ°Ý Áß Ĺ°ş¸µĺ Ĺ¬¸Ż ¸řÇĎµµ·Ď ĽöÁ¤
				//if(g_pD3dApp->m_pKeyBoard->GetAsyncKeyState(DIK_A) || m_bKeyDownA || m_bLeftButtonState)
				if((g_pD3dApp->GetAsyncKeyState_DIK_DIJ(DIK_A) || m_bKeyDownA || m_bLeftButtonState) && !IS_CITY_MAP_INDEX(m_myShuttleInfo.MapChannelIndex.MapIndex))
				//end 2010. 03. 26 by jskim Ăâ°Ý Áß Ĺ°ş¸µĺ Ĺ¬¸Ż ¸řÇĎµµ·Ď ĽöÁ¤
				{// ÁÂ·Î ŔĚµż
					nSideMove--;
					bFormationFlight = false;
					g_pTutorial->m_Lesson3State.bLMove = TRUE;
				}
			}
			// 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
			//if(g_pD3dApp->m_pKeyBoard->GetAsyncKeyState(DIK_W) || m_bUpButtonState)
			if(g_pD3dApp->GetAsyncKeyState_DIK_DIJ(DIK_W) || m_bUpButtonState)
			{
			}
			// 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
			// 2010. 03. 26 by jskim Ăâ°Ý Áß Ĺ°ş¸µĺ Ĺ¬¸Ż ¸řÇĎµµ·Ď ĽöÁ¤
			//if(g_pD3dApp->m_pKeyBoard->GetAsyncKeyState(DIK_S) || m_bDownButtonState)
			if((g_pD3dApp->GetAsyncKeyState_DIK_DIJ(DIK_S) || m_bDownButtonState) && !IS_CITY_MAP_INDEX(m_myShuttleInfo.MapChannelIndex.MapIndex))
			//end 2010. 03. 26 by jskim Ăâ°Ý Áß Ĺ°ş¸µĺ Ĺ¬¸Ż ¸řÇĎµµ·Ď ĽöÁ¤
			{// ŔúĽÓ
				m_nBooster = BOOSTER_TYPE_STOP;				// 2006-01-19 by ispark, ŔúĽÓ ŔĚĆĺĆ®
				bFormationFlight = false;
			}
			if(!g_pD3dApp->m_bChatMode && !m_bReflexionKeyBlock &&
				// 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
				//(g_pD3dApp->m_pKeyBoard->GetAsyncKeyState(DIK_SPACE)) &&
				(g_pD3dApp->GetAsyncKeyState_DIK_DIJ(DIK_SPACE)) &&
				(!m_bKeyDownA && !m_bKeyDownD) &&
				m_bPenaltyUnitStop == FALSE)				// 2005-08-09 by ispark ŔŻ´ÖÁ¤Áö ĆäłÎĆĽ°ˇ »çżëÁßŔĚ ľĆ´Ď¶ó¸é
			{
				// 2005-04-07 by jschoi
				if(g_pTutorial->IsTutorialMode() == FALSE || g_pTutorial->GetLesson() >= L4)
				{
					if( m_myShuttleInfo.CurrentEP && 
						m_bReChargeBURN &&
						m_bUseBooster &&
						(g_pStoreData->GetTotalWeight() / CAtumSJ::GetTransport(&g_pShuttleChild->m_myShuttleInfo)) < WEIGHT_RATE_VERY_HEAVY)
					{
						m_bKeyDownA = FALSE;			// A ÁÂĂř ·Ń¸µ
						m_bKeyDownD = FALSE;			// D żěĂř ·Ń¸µ
						m_bKeyDownACheck = FALSE;		// A ÁÂĂř »çżë Çă°ˇ ĂĽĹ©
						m_bKeyDownDCheck = FALSE;		// D żěĂř »çżë Çă°ˇ ĂĽĹ©
						m_bKeyDownADouble = FALSE;		// A ´őşíĹ¬¸Ż
						m_bKeyDownDDouble = FALSE;		// D ´őşíĹ¬¸Ż
						
						m_nShuttleStopState = 0;
						m_nBooster = BOOSTER_TYPE_MAX;
//						nFBMove = 1;
						bFormationFlight = false;
						m_fAdvanceTime = 10.0f;
					}
				}
			}
//			if(m_bFormationFlight && !bFormationFlight)
			// 2006-01-06 by ispark, ·Ń¸µ »çżë˝Ă Ćí´ë ÇŘÁ¦
			if(m_bKeyDownA || m_bKeyDownD)
			{
//				if(m_pClientParty->GetPartyInfo().bPartyType == _PARTYMASTER &&
//					m_pClientParty->IsFormationFlight())
//				{
//					g_pShuttleChild->m_pClientParty->ISendPartyChangeFlightFormation(FLIGHT_FORM_NONE);
//				}
				bFormationFlight = false;
			}

			if(	m_pClientParty &&
				m_pClientParty->IsFormationFlightMove() &&
				!bFormationFlight)
			{
				// 2011-08-12 by hsson Ćí´ë ´ëż­żˇ Ăß°ˇ/Ĺ»Ĺđ ˝Ă µô·ąŔĚ¸¦ »đŔÔ (Ćí´ë µż±âČ­ ąö±×¸¦ ÁŮŔÓ)
				if( g_cTimeMgr.CheckTime( 1, 200 ) )
				{
					SendPacket(T_IC_PARTY_CANCEL_FLIGHT_POSITION);
					m_pClientParty->FormationFlightClear();
					//				m_infoParty.bFormationFlyingPositionType = 0;
					//				m_bFormationFlight = FALSE; // Ćí´ë şńÇŕŔ» ÄË˝˝ÇĎ°í ş»ŔÎŔÇ ŔÇÁö´ë·Î şńÇŕÇŃ´Ů.
					wsprintf(message,STRMSG_C_PARTY_0001);//"Ćí´ëşńÇŕżˇĽ­ ąţľîłµ˝Ŕ´Ď´Ů."
					g_pD3dApp->m_pChat->CreateChatChild(message,COLOR_SYSTEM);
					
					// ÇŘÁ¦˝Ă ąćÇâ°Ş ĂĘ±âČ­ 
					m_fCurrentSideRate = 0.0f;
					m_fCurrentAngle = 0.0f;
				}
				// end 2011-08-12 by hsson Ćí´ë ´ëż­żˇ Ăß°ˇ/Ĺ»Ĺđ ˝Ă µô·ąŔĚ¸¦ »đŔÔ (Ćí´ë µż±âČ­ ąö±×¸¦ ÁŮŔÓ)
			}


			// 2014-07-29 by ymjoo 콜 오브 히어로 승인 후 빅붐 사용중이면 움직이지 못하게 수정
			if (SKILL_BASE_NUM(g_pShuttleChild->m_pSkill->GetPreSkillItemNum()) == BGEAR_SKILL_BASENUM_BIG_BOOM)
			{
				g_pShuttleChild->m_bUnitStop = TRUE;
				g_pShuttleChild->m_bPenaltyUnitStop = TRUE;
				g_pShuttleChild->m_bMouseMoveLock = TRUE;
				g_pShuttleChild->m_bSkillGoMove = FALSE;
			}
			// END 2014-07-29 by ymjoo 콜 오브 히어로 승인 후 빅붐 사용중이면 움직이지 못하게 수정
		}
		else
		{// Ă¤ĆĂ ¸đµĺŔĎ¶§ żˇľî şę·ąŔĚĹ©°ˇ ľĆ´Ď¶ó¸é şÎ˝şĹÍŔÇ »óĹÂ¸¦ ŔĎąÝ Ĺ°¸¦ ¶ľ »óĹÂ·Î ąŮ˛Ű´Ů
			// 2005-08-11 by ispark
			// Á¤Áö ĆäłÎĆĽ Ăâąß˝Ă STOPşÎ˝şĹÍ ľ˛Áö ľĘ°Ô
			if(m_nBooster != BOOSTER_TYPE_AIRBREAK && m_bPenaltyUnitStop == FALSE)
			{
				m_nBooster = BOOSTER_TYPE_STOP;
			}
		}

		// ŔŻ´Ö Á¤Áö ˝Ă˝şĹŰ Ăł¸®
		// 2006-06-23 by ispark, Äł˝¬ »óÁˇ ŔĚżë˝Ă »çżë ±ÝÁö
		// 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
		//if(g_pD3dApp->m_pKeyBoard->GetAsyncKeyState(DIK_W) && 
		if(g_pD3dApp->GetAsyncKeyState_DIK_DIJ(DIK_W) && 
			!g_pD3dApp->m_bChatMode && 
			g_pD3dApp->m_dwGameState != _SHOP &&
			(!g_pTutorial->IsTutorialMode() ||
			(g_pTutorial->IsTutorialMode() && g_pTutorial->GetChapterState() != TUTORIAL_STATE_READY && g_pTutorial->GetLesson() != L6)))
		{
			// WĹ°¸¦ ´©·¶Ŕ» ¶§ ŔŻ´Ö Á¤Áö¸¦ Ç¬´Ů.
			if(m_bUnitStop == TRUE && m_bSkillGoMove )
			{
				// 2008-07-03 by bhsohn EP3 ąö±× ¸®Ć÷Ć® ĽöÁ¤
				if(g_pGameMain->m_pCommunity->IsShowCommunityWindow())
				{
					// Ăâąß˝Ă °­Á¦·Î Ă˘´ÝŔ˝
					g_pGameMain->m_pCommunity->ShowCommunityWindow(FALSE);
				}
				if(g_pGameMain->m_pSystem && g_pGameMain->m_pSystem->IsOptionShow())
				{
					g_pGameMain->PopupSystemOptinWnd();
				}
				// end 2008-07-03 by bhsohn EP3 ąö±× ¸®Ć÷Ć® ĽöÁ¤

				m_bUnitStop = FALSE;
				m_bPenaltyUnitStop = TRUE;
				m_bMouseMoveLock = FALSE;
				m_nBooster = BOOSTER_TYPE_MIN;
				// 2010. 07. 07 by jskim Č¦µĺ ¸ÂŔ»˝Ă Á¤Áö Ç®¶§ ľČÇ®¸®´Â ąö±× ĽöÁ¤
				if( m_pSkill->IsExistDesParamSKill( DES_SKILL_MON_HOLD ) )
				{
					m_bMouseLock = FALSE;
					g_pD3dApp->m_bFixUnitDirMode = FALSE;
				}
				//end 2010. 07. 07 by jskim Č¦µĺ ¸ÂŔ»˝Ă Á¤Áö Ç®¶§ ľČÇ®¸®´Â ąö±× ĽöÁ¤
				g_pInterface->m_pGameMain->InitShowWindow();			// 2005-08-05 by ispark
				// 2010. 03. 30 by jskim ŔŻ´Ö »óĹÂ ąŮµđÄÁµđĽÇ Ľ­ąö Ŕü´Ţ	
				SendFieldSocketChangeBodyCondition( m_myShuttleInfo.ClientIndex, BODYCON_SET_OR_CLEAR_MASK );
				//end 2010. 03. 30 by jskim ŔŻ´Ö »óĹÂ ąŮµđÄÁµđĽÇ Ľ­ąö Ŕü´Ţ	
			}
		}
		else if(m_bUnitStop == TRUE)
		{
			// ŔŻ´Ö Á¤Áö˝Ă şÎ˝şĹÍ »óĹÂ
			m_nBooster = BOOSTER_TYPE_AIRBREAK;
		}
			
		if( g_pGameMain && 
			g_pGameMain->m_pMission && 
			g_pGameMain->m_pMission->m_bShow)
		{
			m_nBooster = BOOSTER_TYPE_AIRBREAK;
		}
		if(m_bOrderMove && m_nBooster == BOOSTER_TYPE_MAX)
		{
			m_bOrderMove = FALSE;
		}
		if(!m_pClientParty->IsFormationFlightMove() && 
//			m_nBooster != BOOSTER_TYPE_AIRBREAK &&
			!m_bOrderMove)
		{
//			CheckMoveRate(fElapsedTime,nSideMove,nFBMove);
			// 2007-03-27 by dgwoo żÉŔúąö ¸đµĺ˝Ă żňÁ÷ŔÓŔ» ¸·´Â´Ů.
			if(!IsObserverMode())
			{
				CheckMoveRate(fElapsedTime,nSideMove);
			}
			if(m_myShuttleInfo.CurrentEP)
				m_vPos += (m_vNextPos - m_vPos)*fElapsedTime;
			else	// EP°ˇ ľřŔ»¶§-ĽÓµµ¸¦ ąÝŔ¸·Î ÁŮŔÎ´Ů.
				m_vPos += 0.5f*(m_vNextPos - m_vPos)*fElapsedTime;
		}
		if(g_pD3dApp->m_pCamera->m_bIsCamControl)
		{
			m_fSideCheckRate = 0.0f;
		}
		else if(m_nBooster == BOOSTER_TYPE_AIRBREAK)
		{
			if(m_fSideCheckRate > 0.5f)
				m_fSideCheckRate -= 0.5f*fElapsedTime;
			if(m_fSideCheckRate < 0.5f)
				m_fSideCheckRate = 0.5f;
		}
		else
		{
			m_fSideCheckRate = 2.0f;	// ±âżďŔÓ Á¶ŔýşŻĽö 1.0 Ŕ¸·Î ĽÂĆĂ
		}
		CheckCollForMap();		// ¸Ę°úŔÇ Ăćµą °Ë»ç
		CheckCollForObject(fElapsedTime);	// żŔşęÁ§Ć®żÍ Ăćµą °Ë»ç b±âľî
		if( !g_pD3dApp->m_bChatMode && m_nBooster != BOOSTER_TYPE_MAX &&
			// 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
			//((g_pD3dApp->m_pKeyBoard->GetAsyncKeyState(DIK_C) &&	// 20s05-11-07 by ispark Ç¬´Ů
			((g_pD3dApp->GetAsyncKeyState_DIK_DIJ(DIK_C) &&	// 20s05-11-07 by ispark Ç¬´Ů
			(!m_bKeyDownA && !m_bKeyDownD)) ||
//			((g_pD3dApp->m_pKeyBoard->GetAsyncKeyState(DIK_C) &&
//			IS_DT(m_myShuttleInfo.UnitKind))||		// 2005-08-03 by ispark A±âľî ŔĚżÜ´Â CąöĆ° »çżë ±ÝÁö
			g_pD3dApp->m_pKeyBoard->GetAsyncKeyState(DIK_NUMPAD0) || 
			g_pD3dApp->m_pKeyBoard->GetAsyncKeyState(DIK_BACKSLASH)) &&
			g_pD3dApp->m_dwGameState != _SHOP &&
			m_bSkillGoMove)
		{
			TryLandingState();
			// 2007-07-26 by dgwoo CąöĆ°Ŕş Âř·ú˝Ăżˇ »çżë.
			//InitUnitStop();
		}

		// 2005-11-16 by ispark	
		// BąöĆ° ŔĚżÜ´Â ĽöĆň ŔŻÁö »čÁ¦
//		if(m_bMButtonState ||							// 2005-08-09 by ispark ¸¶żě˝ş MąöĆ° »čÁ¦
//		if( m_bIsSetFlighting || 
//			g_pD3dApp->m_bChatMode || 
//			m_nBooster == BOOSTER_TYPE_AIRBREAK ||
//			g_pD3dApp->m_bFixUnitDirMode == TRUE)		// ŔŻ´Ö ÁřÇŕ ąćÇâ °íÁ¤ ¸đµĺ
		if(g_pD3dApp->m_bFixUnitDirMode == TRUE)
		{// ĆňÇü ¸¶Ăß±â
			SetShuttleFlightInit(fElapsedTime);
		}

		// 2005-02-01 by jschoi
		if(	(g_pSOption->sMenuBalance == TRUE ||
			g_pGameMain->m_pSystem->m_bQuitGameMessage == TRUE)&&			// ¸Ţ´ş»çżë˝Ă ŔŻ´ÖÁ¤Áö żÉĽÇ ĂĽĹ©
			(g_pD3dApp->m_bMessageBox == TRUE || 
			g_pD3dApp->m_bFixUnitDirMode == TRUE) )
		{// ŔÎĹÍĆäŔĚ˝ş Ă˘ŔĚ ¶°ŔÖ´Â °ćżě ±âľî¸¦ Ľ­Ľ­Č÷ °¨ĽÓÇŃ´Ů.
			m_nShuttleStopState = 4;
		}

		// 2005-08-22 by ispark
		// şńÇŕÁß ż¬·á°ˇ 0ŔĎ ¶§ Ăß¶ô
		if(0 >= m_myShuttleInfo.CurrentEP)
		{
			m_nBooster = BOOSTER_TYPE_AIRBREAK;
			ChangeUnitStateDead();
		}

		
		// 2009. 07. 07 by ckPark ·Îşż±âľî żäĂ»»çÇ×(·Ń¸µ, Ľ±ĹĂČ­¸é, ą«±â, A±âľîĆ÷´ë)
		//CheckBoosterAni();

		// ·Îşż±âľî ·Ń¸µľÖ´Ď¸ŢŔĚĽÇ˝Ă şÎ˝şĹÍ ľÖ´Ď¸ŢŔĚĽÇŔĚ µ¤ľîľ˛ąÇ·Î ·Ń¸µ˝Ă şÎ˝şĹÍ ľÖ´Ď¸ŢŔĚĽÇ ĂĽĹ©¸¦ ľČÇŃ´Ů
		if( !( m_bKeyDownA || m_bKeyDownD ) )
			CheckBoosterAni();
		// end 2009. 07. 07 by ckPark ·Îşż±âľî żäĂ»»çÇ×(·Ń¸µ, Ľ±ĹĂČ­¸é, ą«±â, A±âľîĆ÷´ë)
	}
	else if(m_dwState == _LANDING)
	{
		// 2007-01-02 by bhsohn A±âľî b+Enter·Î ¸ŘĂá ČÄ, Âř·úÇĎ¸é ľČ żňÁ÷ż©Áö´Â Çö»ó Ăł¸®
		BOOL bLandingDone = FALSE;

		// 2005-11-11 by ispark
		// ´ë°áÁßżˇ´Â ÁöżěÁö ľĘŔ˝
		if(m_pOrderTarget &&
			m_dwPkState != PK_PVP &&
			m_dwPkState != PK_FVF && 
			m_pOrderTarget->m_dwPartType != _ENEMY &&
			!IS_DT(m_myShuttleInfo.UnitKind))				// 2006-05-19 by ispark, A±âľî¶ó¸é Ĺ¸°Ů »čÁ¦ Á¦żÜ
		{
			m_pTarget = NULL;
			m_pOrderTarget = NULL;
		}
		m_nBooster = BOOSTER_TYPE_STOP;
		m_bMouseMoveLock = TRUE;							// 2005-08-05 by ispark Âř·ú˝Ă żňÁ÷ŔÓ ¸·´Â´Ů.

		// Âř·ú˝Ă 1ŔÎÄŞŔĚ¶ó¸é 3ŔÎÄŞŔ¸·Î şŻ°ć
		if(g_pD3dApp->m_pCamera->GetCamType() == CAMERA_TYPE_FPS)
		{
			g_pD3dApp->m_pCamera->SetCamType(CAMERA_TYPE_NORMAL);
			g_pShuttleChild->m_bLandingOldCamtypeFPS = TRUE;
		}		
		if(!COMPARE_BODYCON_BIT(GetCurrentBodyCondition(),BODYCON_LANDING_MASK))
		{
			ChangeSingleBodyCondition(BODYCON_LANDING_MASK);
		}
		if(m_bOrderMove)
		{
			D3DXVECTOR3 vtempPos = m_vPos;
			if(MoveOrder(m_bOrderMoveType))
			{
				m_bOrderMove = FALSE;
			}
			m_vMoveVel = m_vPos - vtempPos;
			D3DXVec3Normalize(&m_vMoveVel, &m_vMoveVel);			// 2006-07-13 by ispark, ±âľî ŔĚµż ąćÇâ
		}
		else
		{
			if(m_fSideCheckRate > 0.0f)
				m_fSideCheckRate -= 0.5f*fElapsedTime;
			if(m_fSideCheckRate < 0.0f)
				m_fSideCheckRate = 0.0f;
			// ĆňÇü ¸¶Ăß±â
			SetShuttleFlightInit(fElapsedTime);
			CheckMoveRate(fElapsedTime,0);
			if(m_myShuttleInfo.CurrentEP)
				m_vPos += (m_vNextPos - m_vPos)*fElapsedTime;
			else	// EP°ˇ ľřŔ»¶§-ĽÓµµ¸¦ ąÝŔ¸·Î ÁŮŔÎ´Ů.
				m_vPos += 0.5f*(m_vNextPos - m_vPos)*fElapsedTime;

			// 2007-05-15 by bhsohn A±âľî ş¸żĎ
			//m_vPos.y -= 15.0f*fElapsedTime;
			if(IS_DT(m_myShuttleInfo.UnitKind))
			{
				m_vPos.y -= AGEAR_LANDING_SPEED*fElapsedTime;
			}
			else
			{
				m_vPos.y -= 15.0f*fElapsedTime;
			}

			float fTempHeight = 100.0f;
			// żŔşęÁ§Ć® °Ë»ç
			if(m_bOnObject)
			{
				D3DXMATRIX matTemp;
				D3DXVECTOR3 vSide;
				vSide = D3DXVECTOR3(0,0,1);
				D3DXMatrixLookAtLH(&matTemp,&m_vPos,&(m_vPos - m_vBasisUp),&vSide);
				fTempHeight = g_pScene->m_pObjectRender->CheckCollMesh(matTemp,m_vPos, FALSE).fDist;
			}
			float fCheckMapHeight = CheckMap(m_vPos);
			if(m_vPos.y <= fCheckMapHeight+m_fAmorHeight || m_fAmorHeight >= fTempHeight)
			{
				if(fCheckMapHeight + m_fAmorHeight > m_vPos.y + m_fAmorHeight - fTempHeight)
					m_vPos.y = fCheckMapHeight + m_fAmorHeight;
				else
					m_vPos.y += m_fAmorHeight - fTempHeight;

				// 2011-02-25 by shcho&hsSon, A±âľî ±âĂĽżÍ Ä«¸Ţ¶ó°ˇ µÚÁýČ÷´ř ą®Á¦ ĽöÁ¤
				// 2011-04-20 by hsson A±âľî ŔÚµżĽöĆň ŔŻÁö żÉĽÇŔ» ˛ŻŔ» ¶§ Âř·ú˝Ă °ř°Ý ¸ř ÇĎ´ř ąö±×
				if( 0 > m_vUp.y )
				{
					// Ŕ˝Ľö¸é ±âĂĽ°ˇ µÚÁýÇô ŔÖ´Â°ÍŔĚ´Ů. µÚÁýÇô ŔÖ´Â¸é yĂŕŔ» ĂµĂµČ÷ ş¸Á¤ÇŘ ÁŘ´Ů.
					m_vUp.y += 0.01f;
				}
				else
				{
					// ľçĽöŔĎ°ćżě Á¦´ë·Î Ľ­ŔÖŔ¸ąÇ·Î ±¦Âú´Ů.
					SendPacket(T_FC_MOVE_LANDING_DONE);
					DBGOUT("T_FC_MOVE_LANDING_DONE\n");
					ChangeUnitState( _LANDED );
				}
				// end 2011-04-20 by hsson A±âľî ŔÚµżĽöĆň ŔŻÁö żÉĽÇŔ» ˛ŻŔ» ¶§ Âř·ú˝Ă °ř°Ý ¸ř ÇĎ´ř ąö±×
				// end 2011-02-25 by shcho&hsSon, A±âľî ±âĂĽżÍ Ä«¸Ţ¶ó°ˇ µÚÁýČ÷´ř ą®Á¦ ĽöÁ¤

				// 2007-01-02 by bhsohn A±âľî b+Enter·Î ¸ŘĂá ČÄ, Âř·úÇĎ¸é ľČ żňÁ÷ż©Áö´Â Çö»ó Ăł¸®
				bLandingDone = TRUE;

				// Ä«¸Ţ¶ó şą±¸
				m_bIsCameraPattern = FALSE;
			}
			// ą°ĽÓŔÎÁö ĂĽĹ©
			if(m_bIsWater && m_vPos.y < g_pGround->m_projectInfo.fWaterHeight)
				m_bIsUnderWater = TRUE;
			// 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
			//if(!m_bKeyBlock && !g_pD3dApp->m_bChatMode && !m_bReflexionKeyBlock && ( g_pD3dApp->m_pKeyBoard->GetAsyncKeyState(DIK_SPACE)))
			if(!m_bKeyBlock && !g_pD3dApp->m_bChatMode && !m_bReflexionKeyBlock && ( g_pD3dApp->GetAsyncKeyState_DIK_DIJ(DIK_SPACE)))
			{
				// 2005-04-07 by jschoi
				if(g_pTutorial->IsTutorialMode() == FALSE || g_pTutorial->GetLesson() >= L4)
				{
					if( m_myShuttleInfo.CurrentEP && 
						m_bReChargeBURN && 
						m_bUseBooster &&
						(g_pStoreData->GetTotalWeight() / CAtumSJ::GetTransport(&g_pShuttleChild->m_myShuttleInfo)) < 0.8f)
					{
						m_nShuttleStopState = 0;
						m_nBooster = BOOSTER_TYPE_MAX;
					}
				}
			}
		}

		// 2005-10-26 by ispark
		CheckCollForObject(fElapsedTime);	// żŔşęÁ§Ć®żÍ Ăćµą °Ë»ç b±âľî

		// Âř·úÁß ´Ů˝Ă ŔĚ·ú
		if( !m_bKeyBlock && 
			!g_pD3dApp->m_bChatMode &&
			!m_bReflexionKeyBlock &&
			// 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
			//(m_bUpButtonState || g_pD3dApp->m_pKeyBoard->GetAsyncKeyState( DIK_SPACE )))
			(m_bUpButtonState || g_pD3dApp->GetAsyncKeyState_DIK_DIJ( DIK_SPACE )))
		{
			// 2005-04-07 by jschoi
			if(g_pTutorial->IsTutorialMode() == FALSE)
			{
				if(m_fNumpad0keyTime <= 0.0f)
				{
					// Âř·úŔü ˝ĂÁˇŔĚ 1ŔÎÄŞŔĚż´Ŕ¸¸é ´Ů˝Ă 1ŔÎÄŞŔ¸·Î şŻČŻ 
					if(m_bLandingOldCamtypeFPS)
					{
						g_pD3dApp->m_pCamera->ChangeCamType();
						m_bLandingOldCamtypeFPS = FALSE;
					}

					SendPacket(T_FC_MOVE_TAKEOFF);
					DBGOUT("T_FC_MOVE_TAKEOFF\n");
					g_pCamera->Init(m_vPos - 10.0f*m_vVel,m_vVel, 3.14f/13.0f, 60.0f);
					m_fNumpad0keyTime = 2.0f;
					ChangeUnitState(_TAKINGOFF);
					// 2007-05-15 by bhsohn A±âľî ş¸żĎ
					//m_fRasingCheckTime = 1.0f;
					m_fRasingCheckTime = GetRasingCheckTime();
				}
			}
		}
		// _LANDING Ä«¸Ţ¶ó ĆĐĹĎ Ŕűżë
		if(m_bIsCameraPattern == TRUE)
		{
			EVENT_POINT CameraPoint;
			m_pCinemaCamera->SkillTick();
			D3DXVECTOR3 vCameraPos, vCameraVel, vCameraUp;
			CameraPoint=m_pCinemaCamera->GetCurrentCinemaPoint();
			vCameraPos = CameraPoint.vPosition;
			vCameraVel = m_vPos - CameraPoint.vPosition;
			vCameraUp = CameraPoint.vUpVector;
			
			// Ä«¸Ţ¶óŔÇ Ŕ§Äˇ ĽÂĆĂ 2004.06.24 jschoi
			g_pD3dApp->m_pCamera->Tick(vCameraPos,vCameraVel,vCameraUp);
			
		}
		// 2007-01-02 by bhsohn A±âľî b+Enter·Î ¸ŘĂá ČÄ, Âř·úÇĎ¸é ľČ żňÁ÷ż©Áö´Â Çö»ó Ăł¸®		
		if(bLandingDone && IS_DT(m_myShuttleInfo.UnitKind))
		{
			// A±âľî ¶ó¸é żňÁ÷ŔÓ ĂĘ±âČ­
			InitUnitStop();
		}		
		// end 2007-01-02 by bhsohn A±âľî b+Enter·Î ¸ŘĂá ČÄ, Âř·úÇĎ¸é ľČ żňÁ÷ż©Áö´Â Çö»ó Ăł¸®
	}
	else if(m_dwState == _LANDED)
	{
		// 2005-11-11 by ispark
		// ´ë°áÁßżˇ´Â ÁöżěÁö ľĘŔ˝
		if(m_pOrderTarget &&
			m_dwPkState != PK_PVP &&
			m_dwPkState != PK_FVF && 
			m_pOrderTarget->m_dwPartType != _ENEMY &&
			!IS_DT(m_myShuttleInfo.UnitKind))				// 2006-05-19 by ispark, A±âľî¶ó¸é Ĺ¸°Ů »čÁ¦ Á¦żÜ
		{
			m_pTarget = NULL;
			m_pOrderTarget = NULL;
		}
		m_nBooster = BOOSTER_TYPE_BACK;
		m_bMouseMoveLock = TRUE;							// 2005-08-05 by ispark

		if(!COMPARE_BODYCON_BIT(GetCurrentBodyCondition(),BODYCON_LANDED_MASK))
		{
			ChangeSingleBodyCondition(BODYCON_LANDED_MASK);
		}
		if(m_fSideCheckRate > 0.0f)
			m_fSideCheckRate -= 0.5f*fElapsedTime;
		if(m_fSideCheckRate < 0.0f)
			m_fSideCheckRate = 0.0f;
		SetShuttleFlightInit(fElapsedTime);
		CheckMoveRate(fElapsedTime,0);
		if(m_myShuttleInfo.CurrentEP)
			m_vPos += (m_vNextPos - m_vPos)*fElapsedTime;
		else	// EP°ˇ ľřŔ»¶§-ĽÓµµ¸¦ ąÝŔ¸·Î ÁŮŔÎ´Ů.
			m_vPos += 0.5f*(m_vNextPos - m_vPos)*fElapsedTime;

		float fTempHeight = 100.0f;
		if(m_bOnObject)
		{
			D3DXMATRIX matTemp;
			D3DXVECTOR3 vSide;
			vSide = D3DXVECTOR3(0,0,1);
			D3DXMatrixLookAtLH(&matTemp,&m_vPos,&(m_vPos - m_vBasisUp),&vSide);
			fTempHeight = g_pScene->m_pObjectRender->CheckCollMesh(matTemp,m_vPos, FALSE).fDist;
		}
		float fCheckMapHeight = CheckMap(m_vPos);
		if(fCheckMapHeight + m_fAmorHeight > m_vPos.y + m_fAmorHeight - fTempHeight)
		{
			m_vPos.y = fCheckMapHeight + m_fAmorHeight;
		}
		else
		{
			if((m_fAmorHeight - fTempHeight) <= 1.0f && (m_fAmorHeight - fTempHeight) >= -1.0f)
			{
				m_vPos.y += m_fAmorHeight - fTempHeight;
			}
			else
			{
				if(m_fAmorHeight - fTempHeight > 0) 
				{
					m_vPos.y += m_fAmorHeight - fTempHeight;
				}
				else
				{
					float fRealObjectHeight = m_vPos.y - fTempHeight;
					m_vPos.y -= fElapsedTime*50.0f;		// Âř·úµČ »óĹÂżˇĽ­ ąŮ´ÚŔ¸·Î ¶łľîÁö´Â ·®			
					if( fRealObjectHeight + m_fAmorHeight > m_vPos.y )
					{
						m_vPos.y = fRealObjectHeight + m_fAmorHeight;			// ±âľî°ˇ ¶Ą(żŔşęÁ§Ć®)ĽÓŔ¸·Î´Â µéľî°Ą Ľö ľř´Ů.
					}
				}
			}
		}
		
		// ÁöÇü ą× żŔşęÁ§Ć®ŔÇ łôŔĚ¸¦±¸ÇĎż© ĽĹĆ˛ŔÇ łôŔĚ¸¦ ŔűżëÇŃ´Ů.	
		m_bFirstStart = FALSE;
		// ą°ĽÓŔÎÁö ĂĽĹ©
		if(m_bIsWater && m_vPos.y < g_pGround->m_projectInfo.fWaterHeight)
			m_bIsUnderWater = TRUE;
		// 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
		//if(!m_bKeyBlock && !g_pD3dApp->m_bChatMode && !m_bReflexionKeyBlock && ( g_pD3dApp->m_pKeyBoard->GetAsyncKeyState(DIK_SPACE)))
		if(!m_bKeyBlock && !g_pD3dApp->m_bChatMode && !m_bReflexionKeyBlock && ( g_pD3dApp->GetAsyncKeyState_DIK_DIJ(DIK_SPACE)))
		{
			// 2005-04-07 by jschoi
			if(g_pTutorial->IsTutorialMode() == FALSE || g_pTutorial->GetLesson() == L4)
			{
				if( m_myShuttleInfo.CurrentEP && 
					m_bReChargeBURN && 
					m_bUseBooster &&
					(g_pStoreData->GetTotalWeight() / CAtumSJ::GetTransport(&g_pShuttleChild->m_myShuttleInfo)) < 0.8f)
				{
					m_nShuttleStopState = 0;
					m_nBooster = BOOSTER_TYPE_MAX;
				}
			}
		}
		// 2006-11-02 by dgwoo µµ˝Ă ¸ĘżˇĽ± CąöĆ°Ŕ» ¸·Ŕ˝Ŕ¸·ÎĽ­ 
		// ąö±×ĽöÁ¤(Âř·ú˝Ă CąöĆ°Ŕ» ´©¸Ł°í ŔÖŔ»¸é ş¸±ŢĂ˘ŔĚ ľČ¶á´Ů.)
		if( !IS_CITY_MAP_INDEX(m_myShuttleInfo.MapChannelIndex.MapIndex) &&
			!g_pD3dApp->m_bChatMode &&
			!m_bReflexionKeyBlock &&
			!m_bKeyBlock &&
			// 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
			//(g_pD3dApp->m_pKeyBoard->GetAsyncKeyState(DIK_C) || 
			(g_pD3dApp->GetAsyncKeyState_DIK_DIJ(DIK_C) || 
			m_bUpButtonState || 
			// 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
			//g_pD3dApp->m_pKeyBoard->GetAsyncKeyState( DIK_SPACE )))
			g_pD3dApp->GetAsyncKeyState_DIK_DIJ( DIK_SPACE )))
		{
			m_bLandedMove = FALSE;
			g_pInterface->m_pGameMain->InitShowWindow();
			// 2005-04-07 by jschoi
			if(g_pTutorial->IsTutorialMode() == FALSE)
			{
				if(m_fNumpad0keyTime <= 0.0f)
				{
					// Âř·úŔü ˝ĂÁˇŔĚ 1ŔÎÄŞŔĚż´Ŕ¸¸é ´Ů˝Ă 1ŔÎÄŞŔ¸·Î şŻČŻ 
					if(m_bLandingOldCamtypeFPS)
					{
						g_pD3dApp->m_pCamera->ChangeCamType();
						m_bLandingOldCamtypeFPS = FALSE;
					}

					SendPacket(T_FC_MOVE_TAKEOFF);
					DBGOUT("T_FC_MOVE_TAKEOFF\n");
					g_pCamera->Init(m_vPos - 10.0f*m_vVel,m_vVel, 3.14f/13.0f, 60.0f);
					m_fNumpad0keyTime = 2.0f;
					ChangeUnitState(_TAKINGOFF);					
					// 2007-05-15 by bhsohn A±âľî ş¸żĎ
					//m_fRasingCheckTime = 1.0f;
					m_fRasingCheckTime = GetRasingCheckTime();

				}
			}
		}

		// 2005-10-26 by ispark
		if(m_bLandedMove == FALSE)
			CheckCollForObject(fElapsedTime);	// żŔşęÁ§Ć®żÍ Ăćµą °Ë»ç b±âľî

		// _LANDING Ä«¸Ţ¶ó ĆĐĹĎ Ŕűżë
		if(m_bIsCameraPattern == TRUE)
		{
			EVENT_POINT CameraPoint;
			m_pCinemaCamera->SkillTick();
			D3DXVECTOR3 vCameraPos, vCameraVel, vCameraUp;
			CameraPoint=m_pCinemaCamera->GetCurrentCinemaPoint();
			vCameraPos = CameraPoint.vPosition;
			vCameraVel = m_vPos - CameraPoint.vPosition;
			vCameraUp = CameraPoint.vUpVector;
			
			// Ä«¸Ţ¶óŔÇ Ŕ§Äˇ ĽÂĆĂ 2004.06.24 jschoi
			g_pD3dApp->m_pCamera->Tick(vCameraPos,vCameraVel,vCameraUp);
			
		}
		if(m_fShuttleSpeed <= 0)
		{
			m_bIsCameraPattern = FALSE;
		}
	}
	else if(m_dwState == _TAKINGOFF)
	{
		InitRolling();
		m_bMouseMoveLock = FALSE;				// 2005-08-05 by ispark		
		m_bIsCameraPattern = FALSE;

		if(!COMPARE_BODYCON_BIT(GetCurrentBodyCondition(),BODYCON_TAKEOFF_MASK))
		{
			ChangeSingleBodyCondition(BODYCON_TAKEOFF_MASK);
		}
		if(m_fRasingCheckTime > 0.0f)
			m_fRasingCheckTime -= fElapsedTime;
		if(m_fRasingCheckTime <= 0.0f)
		{
			m_fRasingCheckTime = 0.0f;
			ChangeUnitState( _NORMAL );
			// 2007-07-26 by dgwoo ŔĚ·ú˝Ă Ŕ§Äˇżˇ µµ´ŢÇĎ¸é ľŐŔ¸·Î ŔüÁř.
			InitUnitStop();
		}
		D3DXVECTOR3 vtempPos = m_vPos;
		
		// 2007-05-15 by bhsohn A±âľî ş¸żĎ
//		m_vPos.y += 30.0f*fElapsedTime;				
//		m_vPos += 30.0f*m_vVel*fElapsedTime;
		if(IS_DT(m_myShuttleInfo.UnitKind))
		{
			m_vPos.y += AGEAR_RASING_SPEED*fElapsedTime;				
			m_vPos += AGEAR_RASING_SPEED*m_vVel*fElapsedTime;
		}
		else
		{
			m_vPos.y += 30.0f*fElapsedTime;				
			m_vPos += 30.0f*m_vVel*fElapsedTime;
		}

		m_vMoveVel = m_vPos - vtempPos;
		D3DXVec3Normalize(&m_vMoveVel, &m_vMoveVel);			// 2006-07-13 by ispark, ±âľî ŔĚµż ąćÇâ
		//m_myShuttleInfo.BodyCondition = BODYCON_TAKEOFF_MASK;
		// 2005-01-19 by jschoi - ŔĚ·ú ˝Ă Ăćµą Ăł¸®
		CheckCollForMap();		// ¸Ę°úŔÇ Ăćµą °Ë»ç
		CheckCollForObject(fElapsedTime);	// żŔşęÁ§Ć®żÍ Ăćµą °Ë»ç b±âľî
	}
	else if(m_dwState == _FALLING)
	{
		D3DXVECTOR3 vtempPos = m_vPos;
		if(m_pClientParty->IsFormationFlightMove())
		{
			SendPacket(T_IC_PARTY_CANCEL_FLIGHT_POSITION);
			m_pClientParty->FormationFlightClear();
			wsprintf(message,STRMSG_C_PARTY_0001);//"Ćí´ëşńÇŕżˇĽ­ ąţľîłµ˝Ŕ´Ď´Ů."
			g_pD3dApp->m_pChat->CreateChatChild(message,COLOR_SYSTEM);
		}
		EVENT_POINT UnitPoint, CameraPoint;
		if(m_pCinemaUnitPattern)
		{
			m_pCinemaUnitPattern->SkillTick();
			UnitPoint=m_pCinemaUnitPattern->GetCurrentCinemaPoint();
			m_vPos = UnitPoint.vPosition;				// Ŕ§Äˇ
			m_vVel = UnitPoint.vDirection;				// ąćÇâ
			m_vUp = UnitPoint.vUpVector;
			m_vTargetPosition = UnitPoint.vTarget;
		}

		// 2005-08-01 by ispark
		// Ä«¸Ţ¶ó ĆĐĹĎ »çżë ľČÇÔ
//		if(m_bIsCameraPattern == TRUE)
//		{
//			m_pCinemaCamera->SkillTick();
//			D3DXVECTOR3 vCameraPos, vCameraVel, vCameraUp;
//			CameraPoint=m_pCinemaCamera->GetCurrentCinemaPoint();
//			vCameraPos = CameraPoint.vPosition;
//			vCameraVel = m_vPos - CameraPoint.vPosition;
//			vCameraUp = CameraPoint.vUpVector;
//			
//			// Ä«¸Ţ¶óŔÇ Ŕ§Äˇ ĽÂĆĂ 2004.06.24 jschoi
//			g_pCamera->Tick(vCameraPos,vCameraVel,vCameraUp);
//			
//		}
		m_vMoveVel = m_vPos - vtempPos;
		D3DXVec3Normalize(&m_vMoveVel, &m_vMoveVel);			// 2006-07-13 by ispark, ±âľî ŔĚµż ąćÇâ


		// 2010. 03. 25 by ckPark »ç¸Á˝Ă 5ĂĘŔĚ»ó Ăß¶ôÇĎ¸é şÎČ°¸ŢĽĽÁö Ăß°ˇ

// 		// Ăß¶ô˝Ă Ăćµą Ăł¸® 2004.06.19 jschoi
// 		BOOL bResult = CheckCollMapObject();
// 		if(m_vPos.y < CheckMap(m_vPos) + 2.0f || bResult)
// 		{
// 			ChangeUnitState( _FALLEN );
// 			g_fSendMovePacket = max(g_fSendMovePacket,5.0f);
// 
// 			// 2005-08-22 by ispark
// 			// ż¬·á°ˇ 0ŔĎ ¶§ Ăß¶ôČÄ ş¸ż©ÁŮ ¸Ţ˝ĂÁö ąÚ˝şżÍ ¸Ţ˝ĂÁö Ăł¸®
// 			// 2005-12-16 by ispark
// 			// Á×Ŕş»óĹÂ°ˇ ľĆ´Ń °ćżě¸¸ FUELALLIN Ăł¸®
// 			if(FALSE == COMPARE_BODYCON_BIT(GetCurrentBodyCondition(),BODYCON_DEAD_MASK)
// 				&& 0 >= m_myShuttleInfo.CurrentEP)// && 
// 			//	!COMPARE_BODYCON_BIT(g_pShuttleChild->m_myShuttleInfo.BodyCondition,BODYCON_DEAD_MASK))
// 			{
// 				ChangeUnitState( _FUELALLIN );
// 				// ¸ŢĽĽÁö Ăł¸®
// 				g_pFieldWinSocket->WriteMessageType(T_FC_CHARACTER_FALLING_BY_FUEL_ALLIN);
// 				g_pD3dApp->m_pChat->CreateChatChild(STRMSG_C_050819_0100, COLOR_ERROR);
// 			}
// 			else
// 			{
// 				// 2005-03-28 by jschoi - Á×ľúŔ» ¶§ şÎČ° Ä«µĺ »çżë
// 				CItemInfo* pItemInfo = g_pStoreData->SerchRevivalUseItem();
// 				MAP_INFO* pMapInfo = g_pDatabase->GetMapInfo(g_pShuttleChild->m_myShuttleInfo.MapChannelIndex.MapIndex);
// // 2005-12-05 by ispark, 
// //				if(g_pD3dApp->m_bDeadByP2PPK == TRUE)// 1:1°áĹőÇĎ´Ů°ˇ Ŕűżˇ°Ô Á×Ŕş °ćżě
// 				// 2006-01-02 by ispark, ´Ů¸Ą ĽĽ·Â¸ĘŔĚ¸é
// 				// 2006-04-10 by ispark, şÎČ° Ä«µĺ ĽöÁ¤, 1:1 °áĹő´Â Á¦żÜ
// 				// 2006-11-17 by ispark, ŔĚşĄĆ® ¸ĘżˇĽ­´Â Á¦żÜ
// 				// 2007-05-15 by dgwoo, ľĆ·ąłŞ ¸ĘżˇĽ­ Á¦żÜ.
// 				if(g_pD3dApp->m_bDeadByP2PPK == FALSE && 
// 					pItemInfo &&
// 					IsRevivalUseItem())
// 				{
// 					char strRevivalMsg[512] = {0,};
// 					// 2009-04-21 by bhsohn ľĆŔĚĹŰ DesParamĂß°ˇ
// 					//sprintf(strRevivalMsg, STRMSG_C_060410_0003, pItemInfo->ItemInfo->ItemName, pItemInfo->ItemInfo->ParameterValue1);	// "%s¸¦ »çżëÇĎ˝Ă°Ú˝Ŕ´Ď±î? Č®·üŔş %.0fŔÔ´Ď´Ů."
// 					sprintf(strRevivalMsg, STRMSG_C_060410_0003, pItemInfo->ItemInfo->ItemName, pItemInfo->ItemInfo->ArrParameterValue[0]);	// "%s¸¦ »çżëÇĎ˝Ă°Ú˝Ŕ´Ď±î? Č®·üŔş %.0fŔÔ´Ď´Ů."
// 					g_pGameMain->m_pInfWindow->AddMsgBox(strRevivalMsg, _Q_USE_NORMAL_RESTORE,0,0,0,pItemInfo->UniqueNumber);
// 				}
// 				else
// 				{
// 					// 2006-11-17 by ispark, şÎČ° Ä«µĺ°ˇ ŔÖÁö¸¸ ŔĚşĄĆ® ¸ĘŔĚ¶ó¸é
// 					if(pItemInfo && IS_MAP_INFLUENCE_EVENT_AREA(pMapInfo->MapInfluenceType))
// 					{
// 						char chMsg[512] = {0,};
// 						wsprintf(chMsg, STRMSG_C_061117_0000, pItemInfo->ItemInfo->ItemName);	// "ŔĚşĄĆ®¸ĘżˇĽ­´Â %s ľĆŔĚĹŰŔ» »çżëÇŇ Ľö ľř˝Ŕ´Ď´Ů."
// 						g_pD3dApp->m_pChat->CreateChatChild(chMsg, COLOR_ERROR);
// 					}
// 
// 					SendDeadMsg();
// 				}
// 
// 
// // 2005-12-05 by ispark, şÎČ° Ä«µĺ »čÁ¦
// //				else if(pItemInfo = g_pStoreData->FindItemInInventoryByItemNum(ITEM_NUM_CASH_CASH_NORMAL_RESTORE))
// //				{
// //					g_pGameMain->m_pInfWindow->AddMsgBox(STRMSG_C_CARD_0001, _Q_USE_NORMAL_RESTORE,0,0,0,pItemInfo->UniqueNumber);//"ŔĎąÝ şÎČ° Ä«µĺ¸¦ »çżëÇĎ˝Ă°Ú˝Ŕ´Ď±î? (ĂëĽŇ˝Ă °Ýłł°í·Î ŔĚµżÇŐ´Ď´Ů.)"
// //				}
// //				else if(pItemInfo = g_pStoreData->FindItemInInventoryByItemNum(ITEM_NUM_CASH_CASH_SUPER_RESTORE))
// //				{
// //					g_pGameMain->m_pInfWindow->AddMsgBox(STRMSG_C_CARD_0002, _Q_USE_SUPER_RESTORE,0,0,0,pItemInfo->UniqueNumber);//"˝´ĆŰ şÎČ° Ä«µĺ¸¦ »çżëÇĎ˝Ă°Ú˝Ŕ´Ď±î? (ĂëĽŇ˝Ă °Ýłł°í·Î ŔĚµżÇŐ´Ď´Ů.)"
// //				}
// //				else
// //				{// şÎČ° Ä«µĺ°ˇ ľř´Ů¸é
// //					g_pGameMain->m_pInfWindow->AddMsgBox(STRMSG_C_CARD_0003, _Q_DEAD_RETURN);	//"°Ýłł°í·Î ŔĚµżÇŐ´Ď´Ů."
// //				}
// 
// 				ResetBodyCondition(BODYCON_EXPLOSION_MASK);
// 			}
// 		}
		

		// Ăß¶ô˝Ă Ăćµą Ăł¸®
		BOOL bResult = CheckCollMapObject();
		if( m_vPos.y < CheckMap(m_vPos) + 2.0f || bResult )
		{
			ChangeUnitState( _FALLEN );
			g_fSendMovePacket = max(g_fSendMovePacket,5.0f);

			if( m_bDeadMessageEnable )
			{
				m_bDeadMessageEnable	= FALSE;

				// ż¬·á°ˇ 0ŔĎ ¶§ Ăß¶ôČÄ ş¸ż©ÁŮ ¸Ţ˝ĂÁö ąÚ˝şżÍ ¸Ţ˝ĂÁö Ăł¸®
				// Á×Ŕş»óĹÂ°ˇ ľĆ´Ń °ćżě¸¸ FUELALLIN Ăł¸®
				if( FALSE == COMPARE_BODYCON_BIT(GetCurrentBodyCondition(),BODYCON_DEAD_MASK)
					&& 0 >= m_myShuttleInfo.CurrentEP )
				{
					ChangeUnitState( _FUELALLIN );
					// ¸ŢĽĽÁö Ăł¸®
					g_pFieldWinSocket->WriteMessageType(T_FC_CHARACTER_FALLING_BY_FUEL_ALLIN);
					g_pD3dApp->m_pChat->CreateChatChild(STRMSG_C_050819_0100, COLOR_ERROR);
				}
				else
				{
					// Á×ľúŔ» ¶§ şÎČ° Ä«µĺ »çżë
					CItemInfo* pItemInfo	= g_pStoreData->SerchRevivalUseItem();
					MAP_INFO* pMapInfo		= g_pDatabase->GetMapInfo(g_pShuttleChild->m_myShuttleInfo.MapChannelIndex.MapIndex);
					// 1:1°áĹőÇĎ´Ů°ˇ Ŕűżˇ°Ô Á×Ŕş °ćżě
					// ´Ů¸Ą ĽĽ·Â¸ĘŔĚ¸é
					// şÎČ° Ä«µĺ ĽöÁ¤, 1:1 °áĹő´Â Á¦żÜ
					// ŔĚşĄĆ® ¸ĘżˇĽ­´Â Á¦żÜ
					// ľĆ·ąłŞ ¸ĘżˇĽ­ Á¦żÜ.
					if(g_pD3dApp->m_bDeadByP2PPK == FALSE && 
						pItemInfo &&
						IsRevivalUseItem())
					{
						char strRevivalMsg[512] = {0,};
						// "%s¸¦ »çżëÇĎ˝Ă°Ú˝Ŕ´Ď±î? Č®·üŔş %.0fŔÔ´Ď´Ů."
						sprintf(strRevivalMsg, STRMSG_C_060410_0003, pItemInfo->ItemInfo->ItemName, pItemInfo->ItemInfo->ArrParameterValue[0]);
						g_pGameMain->m_pInfWindow->AddMsgBox(strRevivalMsg, _Q_USE_NORMAL_RESTORE,0,0,0,pItemInfo->UniqueNumber);
					}
					else
					{
						if(pItemInfo && IS_MAP_INFLUENCE_EVENT_AREA(pMapInfo->MapInfluenceType))
						{
							char chMsg[512] = {0,};
							// "ŔĚşĄĆ®¸ĘżˇĽ­´Â %s ľĆŔĚĹŰŔ» »çżëÇŇ Ľö ľř˝Ŕ´Ď´Ů."
							wsprintf(chMsg, STRMSG_C_061117_0000, pItemInfo->ItemInfo->ItemName);
							g_pD3dApp->m_pChat->CreateChatChild(chMsg, COLOR_ERROR);
						}

						SendDeadMsg();
					}

					ResetBodyCondition(BODYCON_EXPLOSION_MASK);
				}
			}
		}

		if( m_dwDeadTime + FALLING_TIME < timeGetTime() )
		{
			if( m_bDeadMessageEnable )
			{
				m_bDeadMessageEnable	= FALSE;

				// ż¬·á°ˇ 0ŔĎ ¶§ Ăß¶ôČÄ ş¸ż©ÁŮ ¸Ţ˝ĂÁö ąÚ˝şżÍ ¸Ţ˝ĂÁö Ăł¸®
				// Á×Ŕş»óĹÂ°ˇ ľĆ´Ń °ćżě¸¸ FUELALLIN Ăł¸®
				if( FALSE == COMPARE_BODYCON_BIT(GetCurrentBodyCondition(),BODYCON_DEAD_MASK)
					&& 0 >= m_myShuttleInfo.CurrentEP )
				{
					ChangeUnitState( _FUELALLIN );
					// ¸ŢĽĽÁö Ăł¸®
					g_pFieldWinSocket->WriteMessageType(T_FC_CHARACTER_FALLING_BY_FUEL_ALLIN);
					g_pD3dApp->m_pChat->CreateChatChild(STRMSG_C_050819_0100, COLOR_ERROR);
				}
				else
				{
					// Á×ľúŔ» ¶§ şÎČ° Ä«µĺ »çżë
					CItemInfo* pItemInfo	= g_pStoreData->SerchRevivalUseItem();
					MAP_INFO* pMapInfo		= g_pDatabase->GetMapInfo(g_pShuttleChild->m_myShuttleInfo.MapChannelIndex.MapIndex);
					// 1:1°áĹőÇĎ´Ů°ˇ Ŕűżˇ°Ô Á×Ŕş °ćżě
					// ´Ů¸Ą ĽĽ·Â¸ĘŔĚ¸é
					// şÎČ° Ä«µĺ ĽöÁ¤, 1:1 °áĹő´Â Á¦żÜ
					// ŔĚşĄĆ® ¸ĘżˇĽ­´Â Á¦żÜ
					// ľĆ·ąłŞ ¸ĘżˇĽ­ Á¦żÜ.
					if(g_pD3dApp->m_bDeadByP2PPK == FALSE && 
						pItemInfo &&
						IsRevivalUseItem())
					{
						char strRevivalMsg[512] = {0,};
						// "%s¸¦ »çżëÇĎ˝Ă°Ú˝Ŕ´Ď±î? Č®·üŔş %.0fŔÔ´Ď´Ů."
						sprintf(strRevivalMsg, STRMSG_C_060410_0003, pItemInfo->ItemInfo->ItemName, pItemInfo->ItemInfo->ArrParameterValue[0]);
						g_pGameMain->m_pInfWindow->AddMsgBox(strRevivalMsg, _Q_USE_NORMAL_RESTORE,0,0,0,pItemInfo->UniqueNumber);
					}
					else
					{
						if(pItemInfo && IS_MAP_INFLUENCE_EVENT_AREA(pMapInfo->MapInfluenceType))
						{
							char chMsg[512] = {0,};
							// "ŔĚşĄĆ®¸ĘżˇĽ­´Â %s ľĆŔĚĹŰŔ» »çżëÇŇ Ľö ľř˝Ŕ´Ď´Ů."
							wsprintf(chMsg, STRMSG_C_061117_0000, pItemInfo->ItemInfo->ItemName);
							g_pD3dApp->m_pChat->CreateChatChild(chMsg, COLOR_ERROR);
						}

						SendDeadMsg();
					}
				}
			}
		}

		// end 2010. 03. 25 by ckPark »ç¸Á˝Ă 5ĂĘŔĚ»ó Ăß¶ôÇĎ¸é şÎČ°¸ŢĽĽÁö Ăß°ˇ


		// ą°ĽÓŔÎÁö ĂĽĹ©
		if(CheckIsWater(m_vPos) && m_vPos.y < g_pGround->m_projectInfo.fWaterHeight)
			m_bIsUnderWater = TRUE;
	}
	else if( m_dwState == _FALLEN)
	{
		if(!COMPARE_BODYCON_BIT(GetCurrentBodyCondition(),BODYCON_DEAD_MASK))
		{
			m_pTarget = NULL;
			m_pOrderTarget = NULL;
			m_nBooster = BOOSTER_TYPE_BACK;
			g_pD3dApp->SendFieldSocketChangeCharacterBodyCondition(BODYCON_DAMAGE1_MASK,TRUE);
			g_pD3dApp->SendFieldSocketChangeCharacterBodyCondition(BODYCON_DAMAGE2_MASK,TRUE);
			g_pD3dApp->SendFieldSocketChangeCharacterBodyCondition(BODYCON_DAMAGE3_MASK,TRUE);
			ChangeSingleBodyCondition(BODYCON_DEAD_MASK);//BODYCON_DEAD_MASK°ˇ ŔÖ´ÂÁö Č®ŔÎ
		}
	}
	else if(m_dwState==_EXPLODING)
	{
		m_pTarget = NULL;
		m_pOrderTarget = NULL;
		ChangeUnitState( _EXPLODED );
		//m_dwState = _EXPLODED;
	}
	else if(m_dwState==_EXPLODED)
	{
		m_pTarget = NULL;
		m_pOrderTarget = NULL;
	}
	else if(m_dwState == _WARP)
	{
		EVENT_POINT UnitPoint, CameraPoint;
		BOOL bResult = FALSE;
		if(m_pCinemaUnitPattern != NULL)
		{
			bResult = m_pCinemaUnitPattern->SkillTick();
			UnitPoint=m_pCinemaUnitPattern->GetCurrentCinemaPoint();
			m_vPos = UnitPoint.vPosition;				// Ŕ§Äˇ
			m_vVel = UnitPoint.vDirection;				// ąćÇâ
			m_vUp = UnitPoint.vUpVector;
			m_vTargetPosition = UnitPoint.vTarget;			
		}

		if(m_bIsCameraPattern == TRUE)
		{
			bResult = m_pCinemaCamera->SkillTick();
			D3DXVECTOR3 vCameraPos, vCameraVel, vCameraUp;
			CameraPoint=m_pCinemaCamera->GetCurrentCinemaPoint();
			vCameraPos = CameraPoint.vPosition;
			vCameraVel = m_vPos - CameraPoint.vPosition;
			vCameraUp = CameraPoint.vUpVector;
			g_pCamera->Tick(vCameraPos,vCameraVel,vCameraUp);
			
		}
//		m_fEventCheckTime = 2.0f;
		if(bResult)	// Ä«¸Ţ¶ó ĆĐĹĎŔĚ łˇłŞ´ř ľĆ´Ď¸é ŔŻ´Ö ĆĐĹĎŔĚ łˇłŞ¸é Áľ·áÇŃ´Ů.
		{
			switch(m_nEventType)
			{
			case EVENT_WARP_IN:
				{
					if(m_pCurrentObjEvent)
					{
						SendFieldSocketRequestEventObjectWarpIn(m_pCurrentObjEvent);
						m_pCurrentObjEvent = NULL;
					}
				}
				break;
			case EVENT_WARP_OUT:
				{
					m_bIsCameraPattern = FALSE;
					ChangeUnitState( _NORMAL );
				}
				break;
			case EVENT_GAME_START:
				{
					m_bIsCameraPattern = FALSE;
					ChangeUnitState( _NORMAL );
				}
				break;
			case EVENT_GAME_END:
				{
					SetSelectCharacterItem();
					g_pGameMain->m_pSystem->SendFieldSocketCharacterGameEnd();
				}
				break;
			}
//			m_bEventReady = FALSE; WARP_OKżˇĽ­ ÇŃ´Ů.
			
		}
	}

	CheckMapRegion();
	// 2006-08-08 by ispark, żěÁÖ¸ĘŔĚ¸é ÇŃąř´ő Ăćµą °Ë»çÇŃ´Ů.
	// ¸Ę łˇżˇĽ­ żŔşęÁ§Ć®¸¦ ¶Őľîąö¸®´Â ąö±× ĽöÁ¤
	if(!IsTileMapRenderEnable(m_myShuttleInfo.MapChannelIndex.MapIndex) && m_dwState == _NORMAL)
	{		
		m_vMoveVel = m_vPos - m_vOldPos;
		CheckCollForObject(fElapsedTime);
	}

	// ¸ř°ˇ´Â ÁöżŞĂĽĹ©ÇĎż© »Ş˝ĂĹ˛´Ů.
	int nXCheck, nYCheck;
	nXCheck = (int)(m_vPos.x / TILE_SIZE);
	nYCheck = (int)(m_vPos.z / TILE_SIZE);

	if (nXCheck < 0)
		nXCheck = 0;

	if (nYCheck < 0)
		nYCheck = 0;

	m_bMoveGroundType = g_pGround->m_pTileInfo[nXCheck*g_pGround->m_projectInfo.sXSize + nYCheck].bMove;
	if(m_bMoveGroundType == 2)
	{
		m_fShuttleSpeed *= -0.5f;
		m_fLRVelRate *= 0.8f;

		// ĂÖĂĘ Á˘ĽÓ˝Ă ŔĚµż şŇ°ˇ ÁöżŞŔÎ°ćżě ąö±× »ý±č edited by dhkwon 2003.8.20
		if(m_vOldPos.x != -1 && m_vOldPos.y != -1 && m_vOldPos.z != -1)
		{
			m_vPos = m_vPos + 20.0f*(m_vPos-m_vOldPos)*fElapsedTime;
		}
	}
	////////////////////////ż©±â±îÁö Ĺ¬¶óŔĚľđĆ® ÁÂÇĄ ĽłÁ¤(ŔĚµż)//////////////////////////////////////////////////////

	if( m_dwState == _FALLEN || 
		m_dwState == _EXPLODED || 
//		g_pGameMain->IsShowing() ||				// 2006-11-07 by ispark, Ă˘ŔĚ ¶°µµ żňÁ÷ŔĚ°Ô ĽöÁ¤
		m_bOrderMove || 
		m_bMouseLock || 
		//g_pD3dApp->m_bChatMode ||				// 2008-09-02 by dgwoo Ă¤ĆĂÁßżˇµµ ¸¶żě˝ş ÄÁĆ®·Ń °ˇ´ÉÇĎ°Ô.
		g_pD3dApp->m_bFixUnitDirMode == TRUE)	// ŔŻ´Ö ÁřÇŕ ąćÇâ °íÁ¤ ¸đµĺ
	{
		m_fLRVelRate = 0.0f;
		m_fSideRate = 0.0f;
	}

	SetMatrix_Move_Air(fElapsedTime);//	SetEffectPos_B_Gear(fElapsedTime);

}

void CShuttleChild::SetMatrix_Move_Air(float fElapsedTime) //void CShuttleChild::SetEffectPos_B_Gear(float fElapsedTime)
{
	D3DXVECTOR3 vVirtualVel, vVirtualUp,vSide;
	D3DXMATRIX	matVirtualTemp;
	D3DXVec3Cross(&m_vSideVel,&m_vUp,&m_vVel);
	

	// 2005-07-06 by ispark
	// ·Ń¸µ ˝Ă˝şĹŰŔ¸·Î ŔÎÇĎż© ±âżďŔÓ Á¦ÇŃŔ» Ç®±â Ŕ§ÇŘ Á¶°ÇŔ» şń±ł
	if(m_bKeyDownA || m_bKeyDownD)
	{
//		float fUpRate = m_fOldUpRate + (-2.0f*m_fSideRate*m_fSideCheckRate - m_fOldUpRate)*fElapsedTime;

		float fUpRate = RollingSystem(fElapsedTime);		// 2005-07-05 by ispark

		// 2009. 07. 07 by ckPark ·Îşż±âľî żäĂ»»çÇ×(·Ń¸µ, Ľ±ĹĂČ­¸é, ą«±â, A±âľîĆ÷´ë)

		// ·ÎşżľĆ¸ÓŔĎ °ćżě ·Ń¸µ Č¸ŔüŔ» ÇĎÁö ľĘ´Â´Ů
 		if( IsRobotArmor() )
 			fUpRate = 0.0f;
		// end 2009. 07. 07 by ckPark ·Îşż±âľî żäĂ»»çÇ×(·Ń¸µ, Ľ±ĹĂČ­¸é, ą«±â, A±âľîĆ÷´ë)

		D3DXMatrixRotationAxis(&matVirtualTemp, &m_vVel, fUpRate);
		m_fOldUpRate = fUpRate;

	}
	else if((m_fLRVelRate <= 0.0f && m_fSideRate <= m_fLRVelRate*0.5f) 
			|| (m_fLRVelRate >= 0.0f && m_fSideRate >= m_fLRVelRate*0.5f))
	{
		float fUpRate = m_fOldUpRate + (-2.0f*m_fSideRate*m_fSideCheckRate - m_fOldUpRate)*fElapsedTime;

		// 2005-05-26 by jschoi - ±âĂĽ ĂÖ´ë ±âżďŔÓ Á¦ÇŃ
		if(m_dwState == _LANDED)
		{
			fUpRate = 0.0f;
		}
		else if(fUpRate > D3DX_PI/2*0.8f)
		{
			fUpRate = D3DX_PI/2*0.8f;
		}
		else if(fUpRate < -D3DX_PI/2*0.8f)
		{
			fUpRate = -D3DX_PI/2*0.8f;
		}

		D3DXMatrixRotationAxis(&matVirtualTemp, &m_vVel, fUpRate);
		m_fOldUpRate = fUpRate;
	}
	else
	{
		float fUpRate = m_fOldUpRate + (-2.0f*(m_fLRVelRate*0.5f)*m_fSideCheckRate - m_fOldUpRate)*fElapsedTime;

		// 2005-05-26 by jschoi - ±âĂĽ ĂÖ´ë ±âżďŔÓ Á¦ÇŃ
		if(m_dwState == _LANDED)
		{
			fUpRate = 0.0f;
		}
		else if(fUpRate > D3DX_PI/2*0.8f)
		{
			fUpRate = D3DX_PI/2*0.8f;
		}
		else if(fUpRate < -D3DX_PI/2*0.8f)
		{
			fUpRate = -D3DX_PI/2*0.8f;
		}
		
		D3DXMatrixRotationAxis(&matVirtualTemp, &m_vVel, fUpRate);
		m_fOldUpRate = fUpRate;
	}
	D3DXVec3TransformCoord(&vVirtualUp, &m_vUp, &matVirtualTemp );
	D3DXMatrixLookAtRH(&m_mMatrix, &m_vPos, &(m_vPos+m_vVel), &vVirtualUp);
	D3DXMatrixInverse(&m_mMatrix, NULL, &m_mMatrix);

	vSide.x = m_mMatrix._11;
	vSide.y = m_mMatrix._12;
	vSide.z = m_mMatrix._13;

	// 4.0f
	D3DXVec3TransformCoord(&m_vLWPos, &m_PrimaryAttack.vSidePos, &m_mMatrix);
	m_vLWPos += 3.0f*m_vVel;
	D3DXVECTOR3 vPrimaryRightPos = D3DXVECTOR3(-m_PrimaryAttack.vSidePos.x, m_PrimaryAttack.vSidePos.y, m_PrimaryAttack.vSidePos.z);
	D3DXVec3TransformCoord(&m_vRWPos, &vPrimaryRightPos, &m_mMatrix);
	m_vRWPos += 3.0f*m_vVel;

	D3DXVec3TransformCoord(&m_vLWSecondaryPos, &m_SecondaryAttack.vSidePos, &m_mMatrix);
	D3DXVECTOR3 vSecondaryRightPos = D3DXVECTOR3(-m_SecondaryAttack.vSidePos.x, m_SecondaryAttack.vSidePos.y, m_SecondaryAttack.vSidePos.z);
	D3DXVec3TransformCoord(&m_vRWSecondaryPos, &vSecondaryRightPos, &m_mMatrix);

	// 2010-06-15 by shcho&hslee Ćę˝Ă˝şĹŰ - ą«±â Ŕ§Äˇ
	D3DXVec3TransformCoord(&m_vPetLeftPos, &m_PetAttack.vSidePos, &m_mMatrix);
	D3DXVECTOR3 vPetRightPos = D3DXVECTOR3(-m_PetAttack.vSidePos.x,
											m_PetAttack.vSidePos.y, 
											m_PetAttack.vSidePos.z);
	D3DXVec3TransformCoord(&m_vPetRightPos, &vPetRightPos, &m_mMatrix);
	// end 2010-06-15 by shcho&hslee Ćę˝Ă˝şĹŰ - ą«±â Ŕ§Äˇ

	m_vWeaponVel.x = m_mMatrix._31;
	m_vWeaponVel.y = m_mMatrix._32;
	m_vWeaponVel.z = m_mMatrix._33;
	D3DXVec3Normalize(&m_vWeaponVel,&-m_vWeaponVel);

	// 2010-06-15 by shcho&hslee Ćę˝Ă˝şĹŰ - A-Gear ˝ĂÁî»óĹÂ
	m_vPetLeftVel.x		= m_mMatrix._31; 
	m_vPetLeftVel.y		= m_mMatrix._32;
	m_vPetLeftVel.z		= m_mMatrix._33;

	m_vPetRightVel.x	= m_mMatrix._31; 
	m_vPetRightVel.y	= m_mMatrix._32;
	m_vPetRightVel.z	= m_mMatrix._33;

	D3DXVec3Normalize(&m_vPetLeftVel,&-m_vPetLeftVel);
	D3DXVec3Normalize(&m_vPetRightVel,&-m_vPetRightVel);
	// end 2010-06-15 by shcho&hslee Ćę˝Ă˝şĹŰ - A-Gear ˝ĂÁî»óĹÂ

	if(IS_DT(m_myShuttleInfo.UnitKind))
	{

		// 2009. 07. 07 by ckPark ·Îşż±âľî żäĂ»»çÇ×(·Ń¸µ, Ľ±ĹĂČ­¸é, ą«±â, A±âľîĆ÷´ë)

////	D3DXVec3TransformCoord(&m_vLWBackPos, &m_PrimaryAttack.vSideBackPos, &m_mMatrix);
// 		D3DXVec3TransformCoord(&m_vLWBackPos, &m_PrimaryAttack.vSidePos, &m_mMatrix);
// 		D3DXVECTOR3 vTempPrimary = m_PrimaryAttack.vSideBackPos - m_PrimaryAttack.vSidePos;
// 		float fLen = D3DXVec3Length(&vTempPrimary);
// 		D3DXVec3Normalize(&m_vWeaponVel,&m_vWeaponVel);
// 		m_vLWPos = m_vLWBackPos + fLen*m_vWeaponVel;
// 
// 		D3DXVECTOR3 vPrimaryRightBackPos = D3DXVECTOR3(-m_PrimaryAttack.vSideBackPos.x, m_PrimaryAttack.vSideBackPos.y, m_PrimaryAttack.vSideBackPos.z);
// 		D3DXVECTOR3 vRWBackPos;
// 		D3DXVec3TransformCoord(&vRWBackPos, &vPrimaryRightBackPos, &m_mMatrix);
// 		m_vRWPos = vRWBackPos + fLen*m_vWeaponVel;

		// ąß»ç Ŕ§ÄˇŔÇ vSideBackPosżˇĽ­ °ˇÁ®żŔłŞ
		// yĂŕŔş ľĆ¸Ó Ŕ§ÄˇżˇĽ­ °ˇÁ®żÂ´Ů(vSidePos)
		D3DXVECTOR3 vPrimaryLeftBackPos = m_PrimaryAttack.vSideBackPos;
		vPrimaryLeftBackPos.y = m_PrimaryAttack.vSidePos.y;

		// żŢÂĘ ąß»ç Ŕ§Äˇ
		D3DXVec3TransformCoord( &m_vLWBackPos, &vPrimaryLeftBackPos, &m_mMatrix );
		D3DXVECTOR3 vTempPrimary = vPrimaryLeftBackPos - m_PrimaryAttack.vSidePos;

		float fLen = D3DXVec3Length( &vTempPrimary );
		D3DXVec3Normalize( &m_vWeaponVel,&m_vWeaponVel );
		m_vLWPos = m_vLWBackPos + fLen * m_vWeaponVel;
		
		// żŔ¸ĄÂĘ ąß»ç Ŕ§Äˇ
		D3DXVECTOR3 vPrimaryRightBackPos = vPrimaryLeftBackPos;
		vPrimaryRightBackPos.x = -vPrimaryRightBackPos.x;

		D3DXVECTOR3 vRWBackPos;
		D3DXVec3TransformCoord( &vRWBackPos, &vPrimaryRightBackPos, &m_mMatrix) ;
		m_vRWPos = vRWBackPos + fLen * m_vWeaponVel;

		// end 2009. 07. 07 by ckPark ·Îşż±âľî żäĂ»»çÇ×(·Ń¸µ, Ľ±ĹĂČ­¸é, ą«±â, A±âľîĆ÷´ë)

		D3DXVec3TransformCoord(&m_vLWSecondaryPos, &m_SecondaryAttack.vSidePos, &m_mMatrix);
		D3DXVECTOR3 vSecondaryRightPos = D3DXVECTOR3(-m_SecondaryAttack.vSidePos.x, m_SecondaryAttack.vSidePos.y, m_SecondaryAttack.vSidePos.z);
		D3DXVec3TransformCoord(&m_vRWSecondaryPos, &vSecondaryRightPos, &m_mMatrix);	
	}

}
void CShuttleChild::Move_Ground(float fElapsedTime)		//void CShuttleChild::Move_A_Gear(float fElapsedTime)
{
	if (m_dwState == _LANDED || m_dwState == _NORMAL)
	{
		ChangeUnitState(_NORMAL);
		//m_dwState = _NORMAL;
		// Ä«¸Ţ¶ó şą±¸
		m_bIsCameraPattern = FALSE;
		m_bMouseMoveLock = FALSE;				// 2005-08-05 by ispark		

		// 2005-08-19 by ispark
		// A±âľî ż¬·á°ˇ 0ŔĎ ¶§ 
		if (0 >= m_myShuttleInfo.CurrentEP)
		{
			m_fShuttleSpeed = 0.0f;
			g_pGameMain->m_pInfWindow->AddMsgBox(STRMSG_C_050819_0101, _Q_AGEAR_FUEL_ALLIN);	//"ż¬·á°ˇ ĽŇ¸đµÇľî µµ˝Ă·Î żöÇÁµË´Ď´Ů."
			ChangeUnitState(_CLOSE);		// ´Ů¸ĄĂł¸® ¸řÇĎµµ·Ď ÇŃ´Ů.
		}

		// 2005-08-31 by ispark
		// ·Ń¸µÁßŔĚľú´Ů¸é ·Ń¸µ Ç¬´Ů.
		if (m_bRollUsed)
		{
			m_bKeyDownA = m_bKeyDownD = FALSE;
			m_bKeyDownACheck = m_bKeyDownDCheck = FALSE;
			m_bRollUsed = FALSE;
			m_fOldUpRate = 0.0f;			// ±âżďŔÎ Ă¤·Î ŔĚ·úÇŘĽ­ ·Ń¸µŔĚľú´Ů¸é ±âżďŔÓŔ» ĂĘ±âČ­ ˝ĂĹ˛´Ů.
		}
	}
	else
	{
		m_bIsAir = TRUE;

		// 2005-08-19 by ispark
		// A±âľî ż¬·á°ˇ 0ŔĎ ¶§ 
		if (0 >= m_myShuttleInfo.CurrentEP) {
			if(g_pD3dApp->m_pSound)
				g_pD3dApp->m_pSound->StopD3DSound(SOUND_GROUND_MOVING_A_GEAR);	// 2005-08-19 by ispark
			m_bIsAir = FALSE;
		}
	}

	//		if( m_bAttackMode != _SIEGE &&
	//			!COMPARE_BODYCON_BIT(GetCurrentBodyCondition(),BODYCON_EXPLOSION_MASK) && 
	//			!COMPARE_BODYCON_BIT(GetCurrentBodyCondition(),BODYCON_LANDED_MASK))
	//		{
	//			ChangeSingleBodyCondition(BODYCON_LANDED_MASK);
	//		}

	D3DXMATRIX  matTemp;
	D3DXVec3Normalize(&m_vVel, &m_vVel);
	D3DXVec3Normalize(&m_vUp, &m_vUp);
	D3DXVec3Cross(&m_vSideVel, &m_vUp, &m_vVel);
	//		int nx,ny,nz;
	//		g_pD3dApp->CalcObjectSourceScreenCoords(m_vPos+10000.0f*m_vVel, g_pD3dApp->GetBackBufferDesc().Width, g_pD3dApp->GetBackBufferDesc().Height, 
	//			nx, ny, nz );


	/*		// Ćň¸é ąćÁ¤˝ÄŔ» ±¸ÇŃ´Ů.
	// a*x + b*y + c*z + d = 0 << Ćň¸é ąćÁ¤˝Ä
	float fTemp = -((m_vUp.x*m_vMouseDir.x + m_vUp.y*m_vMouseDir.y + m_vUp.z*m_vMouseDir.z)/(m_vUp.x*m_vUp.x + m_vUp.y*m_vUp.y + m_vUp.z*m_vUp.z));
	D3DXVECTOR3 vNew;
	vNew.x = m_vUp.x*fTemp + m_vMouseDir.x;
	vNew.y = m_vUp.y*fTemp + m_vMouseDir.y;
	vNew.z = m_vUp.z*fTemp + m_vMouseDir.z;
	D3DXVec3Normalize(&vNew,&vNew);

	m_fSideRate = ACOS(D3DXVec3Dot(&m_vVel, &vNew));
	if(nx > m_pt.x)
	m_fSideRate *= -1.0f;
	if(abs(nx - m_pt.x) > 3*((float)g_pD3dApp->GetBackBufferDesc().Width/(float)800) && !g_pGameMain->IsShowing()
	&& !m_bMouseLock && !m_bOrderMove)
	{
	D3DXVECTOR3 vVel,vSide;
	vVel = m_vVel;
	vVel.y = 0.0f;
	D3DXVec3Normalize(&vVel,&vVel);
	float fAngleTemp = ACOS(D3DXVec3Dot(&m_vVel,&vVel));
	if(m_vVel.y > 0.0f)
	fAngleTemp *= -1.0f;

	D3DXMatrixRotationAxis(&matTemp, &m_vBasisUp, m_fMouseRate*m_fSideRate*fElapsedTime);
	D3DXVec3TransformCoord(&vVel, &vVel, &matTemp);

	D3DXVec3Cross(&vSide,&m_vBasisUp,&vVel);
	D3DXMatrixRotationAxis(&matTemp, &vSide, fAngleTemp);
	D3DXVec3TransformCoord(&m_vVel, &vVel, &matTemp);
	}
	D3DXVec3Cross(&m_vSideVel,&m_vUp,&m_vVel);
	D3DXVec3Cross(&m_vUp,&m_vVel,&m_vSideVel);
	*/
	int nSideMove = 0;
	//	int nFBMove = 0;
	if (m_bLandingField == FALSE &&
		g_pD3dApp->m_bFixUnitDirMode == FALSE &&		// ŔŻ´Ö ÁřÇŕ ąćÇâ °íÁ¤ ¸đµĺ
		!g_pD3dApp->m_bChatMode &&
		m_bReflexionKeyBlock == FALSE &&
		m_bAttackMode != _SIEGE &&
		g_pD3dApp->m_dwGameState != _SHOP)
		//	(g_pTutorial->IsTutorialMode() == FALSE || g_pTutorial->GetLesson() >= L2) )
	{
		int nOldBoosterType = m_nBooster;
		m_nBooster = BOOSTER_TYPE_MIN;
		//			if( m_pItemSecondary && g_pD3dApp->m_pKeyBoard->GetAsyncKeyState(DIK_E) &&
		//				m_fSecondaryWeaponRACheckTime >= ((float)m_pItemSecondary->ReAttacktime)/1000.0f)
		//			{// 2Çü ą«±â ąß»ç
		//				m_bSecondaryAttackOrder = TRUE;
		//			}
		// 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
		//if(g_pD3dApp->m_pKeyBoard->GetAsyncKeyState(DIK_D) || m_bRightButtonState)
		if (g_pD3dApp->GetAsyncKeyState_DIK_DIJ(DIK_D) || m_bRightButtonState)
		{// żě·Î ŔĚµż->Č¸ŔüŔ¸·Î şŻ°ć
			D3DXMatrixRotationAxis(&matTemp, &m_vUp, 1.0f*fElapsedTime);
			D3DXVec3TransformCoord(&m_vVel, &m_vVel, &matTemp);
			//				nSideMove++;
		}
		// 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
		//if(g_pD3dApp->m_pKeyBoard->GetAsyncKeyState(DIK_A) || m_bLeftButtonState)
		if (g_pD3dApp->GetAsyncKeyState_DIK_DIJ(DIK_A) || m_bLeftButtonState)
		{// ÁÂ·Î ŔĚµż->Č¸ŔüŔ¸·Î şŻ°ć
			D3DXMatrixRotationAxis(&matTemp, &m_vUp, -1.0f*fElapsedTime);
			D3DXVec3TransformCoord(&m_vVel, &m_vVel, &matTemp);
			//				nSideMove--;
		}
		// 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷

		if (g_pD3dApp->GetAsyncKeyState_DIK_DIJ(DIK_W) || m_bUpButtonState) {
			if (0 == m_fShuttleSpeed || 0 == g_pShuttleChild->m_nShuttleBoosterState) {
				if(g_pD3dApp->m_pSound)
					g_pD3dApp->m_pSound->PlayD3DSound(SOUND_GROUND_MOVING_A_GEAR, m_vPos, FALSE);
				m_bLendSoundPlay = TRUE;
			}
			CheckBoosterState(fElapsedTime, true, nOldBoosterType);
		}
		else {
			CheckBoosterState(fElapsedTime, false, nOldBoosterType);
		}

		if (g_pD3dApp->GetAsyncKeyState_DIK_DIJ(DIK_S) || m_bDownButtonState && !m_bAniKeyBlock) {
			if (0 == m_fShuttleSpeed || 0 == g_pShuttleChild->m_nShuttleBoosterState) {
				if(g_pD3dApp->m_pSound)
					g_pD3dApp->m_pSound->PlayD3DSound(SOUND_GROUND_MOVING_A_GEAR, m_vPos, FALSE);
				m_bLendSoundPlay = TRUE;
			}
		}
	}
	else {
		if (m_nBooster != BOOSTER_TYPE_AIRBREAK) {
			m_nBooster = BOOSTER_TYPE_STOP;
		}
	}
	//		if(!g_pD3dApp->m_bChatMode &&
	//			!m_bReflexionKeyBlock && 
	//			(g_pD3dApp->m_pKeyBoard->GetAsyncKeyState(DIK_SPACE))&&
	//			m_myShuttleInfo.CurrentEP > 0 &&
	//			m_bAttackMode != _SIEGE &&
	//			m_bReChargeBURN )
	//		{
	//			m_nShuttleStopState = 0;
	//			m_nBooster = BOOSTER_TYPE_MAX;
	//			nFBMove = 1;
	//		}
	//		if(m_nBooster == BOOSTER_TYPE_MAX)
	//		{
	//			// ŔĚ·úÇĎ°Ô µČ´Ů.
	//			SendPacket(T_FC_MOVE_TAKEOFF);
	//			m_bIsAir = TRUE;
	//		}
	// 2006-11-02 by dgwoo A±âľî´Â ż©±âĽ­ Ăł¸®.
	if (!IS_CITY_MAP_INDEX(m_myShuttleInfo.MapChannelIndex.MapIndex) &&
		m_bLandingField == FALSE &&
		!g_pD3dApp->m_bChatMode &&
		!m_bReflexionKeyBlock &&
		m_bAttackMode != _SIEGE &&
		m_myShuttleInfo.CurrentEP > 0 &&
		// 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
		//( g_pD3dApp->m_pKeyBoard->GetAsyncKeyState(DIK_C) ||
		(g_pD3dApp->GetAsyncKeyState_DIK_DIJ(DIK_C) ||
		//			m_bUpButtonState || 
		// 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
		//g_pD3dApp->m_pKeyBoard->GetAsyncKeyState( DIK_SPACE )))
		g_pD3dApp->GetAsyncKeyState_DIK_DIJ(DIK_SPACE)))
	{
		m_bLandedMove = FALSE;
		g_pInterface->m_pGameMain->InitShowWindow();

		// 2005-04-07 by jschoi
		if (g_pTutorial->IsTutorialMode() == FALSE)
		{
			if (m_fNumpad0keyTime <= 0.0f)
			{
				// Âř·úŔü ˝ĂÁˇŔĚ 1ŔÎÄŞŔĚż´Ŕ¸¸é ´Ů˝Ă 1ŔÎÄŞŔ¸·Î şŻČŻ 
				if (m_bLandingOldCamtypeFPS)
				{
					g_pD3dApp->m_pCamera->ChangeCamType();
					m_bLandingOldCamtypeFPS = FALSE;
				}

				SendPacket(T_FC_MOVE_TAKEOFF);
				DBGOUT("T_FC_MOVE_TAKEOFF\n");
				g_pCamera->Init(m_vPos - 10.0f*m_vVel, m_vVel, 3.14f / 13.0f, 60.0f);
				m_fNumpad0keyTime = 2.0f;
				ChangeUnitState(_TAKINGOFF);
				// 2007-05-15 by bhsohn A±âľî ş¸żĎ
				//m_fRasingCheckTime = 1.0f;
				m_fRasingCheckTime = GetRasingCheckTime();
			}
		}
	}

	if (0 == g_pShuttleChild->m_nShuttleBoosterState && TRUE == m_bLendSoundPlay) {
		if(g_pD3dApp->m_pSound)
			g_pD3dApp->m_pSound->StopD3DSound(SOUND_GROUND_MOVING_A_GEAR);
		m_bLendSoundPlay = FALSE;
	}

	if (g_pD3dApp->m_dwGameState == _SHOP) {
		if (g_pD3dApp->GetAsyncKeyState_DIK_DIJ(DIK_W) ||
			g_pD3dApp->GetAsyncKeyState_DIK_DIJ(DIK_S) ||
			g_pD3dApp->GetAsyncKeyState_DIK_DIJ(DIK_A) ||
			g_pD3dApp->GetAsyncKeyState_DIK_DIJ(DIK_D) ||
			g_pD3dApp->GetAsyncKeyState_DIK_DIJ(DIK_UP) ||
			g_pD3dApp->GetAsyncKeyState_DIK_DIJ(DIK_DOWN) ||
			g_pD3dApp->GetAsyncKeyState_DIK_DIJ(DIK_LEFT) ||
			g_pD3dApp->GetAsyncKeyState_DIK_DIJ(DIK_RIGHT)) {

			g_pInterface->m_pCityBase->OnCityNPCButtonDown(CITY_NPC_BUTTON_CLOSE);

		}
	}

	if (g_pGameMain->m_pMission->m_bShow)
	{
		m_nBooster = BOOSTER_TYPE_STOP;
		nSideMove = 0;
		//		nFBMove = 0;
	}
	//		CheckMoveRate(fElapsedTime,nSideMove,nFBMove);
	//	CheckMoveRate(fElapsedTime,0,nFBMove);
	CheckMoveRate(fElapsedTime, 0);
	// Ĺ¸ Äł¸Ż ĂĽĹ©
	//		if(m_bSecondaryFireType == 1)
	//			CheckEnemyType1(1);
	//		else if(m_bSecondaryFireType == 2)
	//			CheckEnemyType2(1);
	m_fSideRate = 0.0f;

	D3DXVECTOR3  vUpTemp;
	D3DXVECTOR3 vFront, vBack, vLeft, vRight;
	vFront = m_vPos + 6.0f*m_vVel;
	vBack = m_vPos - 3.0f*m_vVel;
	vLeft = m_vPos - 6.0f*m_vSideVel;
	vRight = m_vPos + 6.0f*m_vSideVel;
	m_bCheckMove = 4;
	m_bCollMap = FALSE;
	BYTE bTemp;
	BYTE bCollCont = 0;
	// Front
	bTemp = m_bCheckMove;
	CheckMap(vFront);
	if (bTemp < m_bCheckMove)
		m_bCheckMove = bTemp;
	if (m_bCollMap)
	{
		if (g_pD3dApp->m_pEffectList)
		{
			CAppEffectData * pData = new CAppEffectData(NULL, RC_EFF_COLL_01, vFront);
			pData = (CAppEffectData *)g_pD3dApp->m_pEffectList->AddChild(pData);
		}
		bCollCont++;
	}
	// Back
	m_bCollMap = FALSE;
	bTemp = m_bCheckMove;
	CheckMap(vBack);
	if (bTemp < m_bCheckMove)
		m_bCheckMove = bTemp;
	if (m_bCollMap)
	{
		if (g_pD3dApp->m_pEffectList)
		{
			CAppEffectData * pData = new CAppEffectData(NULL, RC_EFF_COLL_01, vBack);
			pData = (CAppEffectData *)g_pD3dApp->m_pEffectList->AddChild(pData);
		}
		bCollCont++;
	}
	// Left
	m_bCollMap = FALSE;
	bTemp = m_bCheckMove;
	CheckMap(vLeft);
	if (bTemp < m_bCheckMove)
		m_bCheckMove = bTemp;
	if (m_bCollMap)
	{
		if (g_pD3dApp->m_pEffectList)
		{
			CAppEffectData * pData = new CAppEffectData(NULL, RC_EFF_COLL_01, vLeft);
			pData = (CAppEffectData *)g_pD3dApp->m_pEffectList->AddChild(pData);
		}
		bCollCont++;
	}
	// Right	
	m_bCollMap = FALSE;
	bTemp = m_bCheckMove;
	CheckMap(vRight);
	if (bTemp < m_bCheckMove)
		m_bCheckMove = bTemp;
	if (m_bCollMap)
	{
		if (g_pD3dApp->m_pEffectList)
		{
			CAppEffectData * pData = new CAppEffectData(NULL, RC_EFF_COLL_01, vRight);
			pData = (CAppEffectData *)g_pD3dApp->m_pEffectList->AddChild(pData);
		}
		bCollCont++;
	}

	float fRate = 1.0f;
	// A±âľî Áö»ó ŔĚµż˝Ă ÁöÇüżˇ ´ëÇŃ ±âľîŔÇ łôł·ŔĚ ĽĽĆĂ
	float fHeight = 0.0f;

	float fObjHeight = -DEFAULT_COLLISION_DISTANCE;

	// ¸Ę łôŔĚ »ęĂâ
	// 2004-11-04 by jschoi
	if (m_bOnObject)
	{
		float fTempDistance;
		D3DXMATRIX matTemp;
		D3DXMatrixLookAtLH(&matTemp, &m_vPos, &(m_vPos - D3DXVECTOR3(0, 1, 0)), &D3DXVECTOR3(0, 0, 1));		// ľĆ·ˇ
		fTempDistance = g_pScene->m_pObjectRender->CheckCollMesh(matTemp, m_vPos, FALSE).fDist;
		if (fTempDistance != DEFAULT_COLLISION_DISTANCE)
		{
			fHeight = CheckMap(m_vPos);
			if (m_vPos.y - fTempDistance > fHeight)
			{
				fHeight = m_vPos.y - fTempDistance;
			}
		}
		else
		{
			fHeight = CheckMap(m_vPos);
		}
	}
	else
	{
		fHeight = CheckMap(m_vPos);
	}

	if ((m_vPos.y - fHeight) < 5.0f)
	{
		switch (m_bCheckMove)
		{
		case 0:
		{// ¸ř°ˇ´Â ÁöżŞ
			D3DXVECTOR3 vTempvel;
			vTempvel = m_vNormal;
			vTempvel.y = 0.0f;
			D3DXVec3Normalize(&vTempvel, &vTempvel);
			// ąćÇâ Ć˛ľîÁü 
			D3DXVec3Normalize(&m_vVel, &(20.0f*m_vVel + vTempvel - 100.0f*m_vBasisUp*fElapsedTime));
			D3DXVec3Normalize(&vTempvel, &(m_vVel + 1.5f*vTempvel));
			if (m_vPos.y < m_vNextPos.y || bCollCont)
			{
				if (m_fShuttleSpeed > 0.0f)	// ľŐąćÇâŔ¸·Î ŔüÁřÇĎ¸ç şÎµúČú¶§
					m_vNextPos = m_vPos + D3DXVec3Length(&(m_vPos - m_vNextPos))*vTempvel;
				else	// µÚ·Î °ˇ¸ç şÎµúČú¶§
				{
					m_fShuttleSpeed *= -0.8f;
					m_vNextPos = m_vPos + 20.0f*D3DXVec3Length(&(m_vPos - m_vNextPos))*vTempvel;
				}
				if (m_fLRVelRate > 0.0f)
					m_fLRVelRate *= -0.3f;
				else
				{
					m_fLRVelRate *= -0.3f;
				}
			}
			break;
		}
		case 1:
		{// 2Â÷°¨ĽÓ
		}
		case 2:
		{// 1Â÷°¨ĽÓ
		}
		case 3:
		{// °ĄĽöŔÖ´Â ÁöżŞ
			fRate = 1.0f;
			break;
		}
		}
	}
	// Ć÷ÁöĽÇŔ» Ŕűżë
	if (m_myShuttleInfo.CurrentEP)
		m_vPos += fRate*(m_vNextPos - m_vPos)*fElapsedTime;
	else
		m_vPos += 0.5f*fRate*(m_vNextPos - m_vPos)*fElapsedTime;
	// şńÇŕłôŔĚ »ęĂâ

	// 2004-11-04 by jschoi
	float fHeightDistance = m_vPos.y - fHeight - 10.0f;
#ifdef _INET_NOMOREAGTROLL	
	if (fHeightDistance > _INET_NOMOREAGTROLL_REACTION_HEIGHT)
	{
		if(_INET_NOMOREAGTROLL_MODE == 0)
		{
			if (g_pTutorial->IsTutorialMode() == FALSE)
			{
				if (m_fNumpad0keyTime <= 0.0f)
				{
					if (m_bLandingOldCamtypeFPS)
					{
						g_pD3dApp->m_pCamera->ChangeCamType();
						m_bLandingOldCamtypeFPS = FALSE;
					}

					SendPacket(T_FC_MOVE_TAKEOFF);
					DBGOUT("T_FC_MOVE_TAKEOFF\n");
					g_pCamera->Init(m_vPos - 10.0f*m_vVel, m_vVel, 3.14f / 13.0f, 60.0f);
					m_fNumpad0keyTime = 2.0f;
					ChangeUnitState(_TAKINGOFF);
					m_fRasingCheckTime = GetRasingCheckTime();
				}
			}
		}
		else
		{
			if(fHeightDistance > 10.0f)
			{
				fHeightDistance = 10.0f;
			}
			m_vPos.y -= 205.0f*fHeightDistance*fElapsedTime;
			if( fHeight + m_fAmorHeight > m_vPos.y )
			{
				m_vPos.y = fHeight + m_fAmorHeight;
			}
		}
	}
	else if (fHeightDistance > 0.2f)
	{
		if (fHeightDistance > 10.0f)
		{
			fHeightDistance = 10.0f;
		}
		m_vPos.y -= 5.0f*fHeightDistance*fElapsedTime;
		if (fHeight + m_fAmorHeight > m_vPos.y)
		{
			m_vPos.y = fHeight + m_fAmorHeight;
		}

	}
#else
	if (fHeightDistance > 0.2f)
	{
		if (fHeightDistance > 10.0f)
		{
			fHeightDistance = 10.0f;		// ĂÖ´ë°Ş
		}
		m_vPos.y -= 5.0f*fHeightDistance*fElapsedTime;		// A±âľî Áö»ó ŔĚµż˝Ă ¶łľîÁö´Â łôŔĚ //inetpub - change 5.0f to 205.0f and agear during langing will fall like a stone
		if (fHeight + m_fAmorHeight > m_vPos.y)
		{
			m_vPos.y = fHeight + m_fAmorHeight;			// ±âľî°ˇ ¶Ą(żŔşęÁ§Ć®)ĽÓŔ¸·Î´Â µéľî°Ą Ľö ľř´Ů.
		}

	}
#endif
	else if(fHeightDistance < -0.2f)
	{
		if(fHeightDistance < -10.0f)
		{
			fHeightDistance = -10.0f;		// ĂÖ´ë°Ş
		}
		m_vPos.y -= 10.0f*fHeightDistance*fElapsedTime;		// A±âľî Áö»ó ŔĚµż˝Ă ±âľî żĂ¶ó°ˇ´Â łôŔĚ
	}

	// 2009. 07. 07 by ckPark ·Îşż±âľî żäĂ»»çÇ×(·Ń¸µ, Ľ±ĹĂČ­¸é, ą«±â, A±âľîĆ÷´ë)
	// ¶Ążˇ Âř·ú˝Ă ľĆ¸Ó¸¦ °ĄľĆłĄ ˝Ă ±âľî°ˇ ĆÄ ąŻČ÷´Â°ÍŔ» ąćÁöÇĎ±â Ŕ§ÇŘ ÇŃąř ´ő ĂĽĹ©
	if( fHeight + m_fAmorHeight > m_vPos.y )
		m_vPos.y = fHeight + m_fAmorHeight;			// ±âľî°ˇ ¶Ą(żŔşęÁ§Ć®)ĽÓŔ¸·Î´Â µéľî°Ą Ľö ľř´Ů.
	// end 2009. 07. 07 by ckPark ·Îşż±âľî żäĂ»»çÇ×(·Ń¸µ, Ľ±ĹĂČ­¸é, ą«±â, A±âľîĆ÷´ë)
	
	//		if((m_vPos.y - (fHeight+10.0f)) > 0.2f)
	//		{
	//			m_vPos.y -= 10.0f*fElapsedTime;
	//		}
	//		else if((m_vPos.y - (fHeight+10.0f)) < -0.2f)
	//		{
	//			m_vPos.y += 10.0f*((fHeight+10.0f) - m_vPos.y)*fElapsedTime;
	//		}
	
	
	FLOAT fDist;
	int i,j;
	D3DXVECTOR3 v,orig,dir;
	D3DXVECTOR3 vSide,vVel;
	D3DXVECTOR3 v1,v2,v3;
	D3DXVECTOR3 vNextVel, vNextUp,vPos;
	float fDistTemp;
	v = D3DXVECTOR3(0,0,0);
	vSide = D3DXVECTOR3(0,0,1);
	//		orig = m_vPos+10.0f*m_vVel;
	float fTempHeight = 100.0f;
	//		i = (int)(orig.x/TILE_SIZE);
	//		j = (int)(orig.z/TILE_SIZE);
	dir = D3DXVECTOR3(0,-1,0);
	
	orig = m_vPos + 5.0f*m_vUp + 6.0f*m_vVel;		// Front
	D3DXMatrixLookAtLH(&matTemp,&orig,&(orig - m_vBasisUp),&vSide);
	fDistTemp = g_pScene->m_pObjectRender->CheckCollMesh(matTemp,orig, FALSE).fDist;
	//		if(fDistTemp > 14.0f)
	//			fDistTemp = 14.0f;
	v1 = orig - m_vBasisUp*fDistTemp;
	
	i = (int)(orig.x/TILE_SIZE);
	j = (int)(orig.z/TILE_SIZE);
	if(i >= 0 && i < g_pGround->m_projectInfo.sXSize && j >= 0 && j < g_pGround->m_projectInfo.sYSize
		&&IsTileMapRenderEnable(g_pShuttleChild->m_myShuttleInfo.MapChannelIndex.MapIndex))					// 2006-06-07 by ispark
	{
		if(g_pGround->IntersectTriangle(orig,dir,g_pGround->m_pTileVertexArray[i*(g_pGround->m_projectInfo.sYSize+1)+j].pos,
			g_pGround->m_pTileVertexArray[i*(g_pGround->m_projectInfo.sYSize+1)+(j+1)].pos,
			g_pGround->m_pTileVertexArray[(i+1)*(g_pGround->m_projectInfo.sYSize+1)+(j)].pos,
			&fDist))//, &fBary1, &fBary2 ))
		{
			v = orig + dir*fDist;
		}
		else if(g_pGround->IntersectTriangle(orig,dir,g_pGround->m_pTileVertexArray[i*(g_pGround->m_projectInfo.sYSize+1)+j+1].pos,
			g_pGround->m_pTileVertexArray[(i+1)*(g_pGround->m_projectInfo.sYSize+1)+(j)].pos,
			g_pGround->m_pTileVertexArray[(i+1)*(g_pGround->m_projectInfo.sYSize+1)+j+1].pos,
			&fDist))//, &fBary1, &fBary2 ))
		{
			v = orig + dir*fDist;
		}
		if(CheckIsWater(v))
		{
			if(v.y < g_pGround->m_projectInfo.fWaterHeight)
				v.y = g_pGround->m_projectInfo.fWaterHeight;
		}
	}
	if(v1.y <= v.y)
		v1 = v;
	
	orig = m_vPos + 5.0f*m_vUp + 6.0f*m_vSideVel;	// Left
	D3DXMatrixLookAtLH(&matTemp,&orig,&(orig - m_vBasisUp),&vSide);
	fDistTemp = g_pScene->m_pObjectRender->CheckCollMesh(matTemp,orig, FALSE).fDist;
	//		if(fDistTemp > 14.0f)
	//			fDistTemp = 14.0f;
	v2 = orig - m_vBasisUp*fDistTemp;
	v = D3DXVECTOR3(0,0,0);
	i = (int)(orig.x/TILE_SIZE);
	j = (int)(orig.z/TILE_SIZE);
	if(i >= 0 && i < g_pGround->m_projectInfo.sXSize && j >= 0 && j < g_pGround->m_projectInfo.sYSize
		&& IsTileMapRenderEnable(g_pShuttleChild->m_myShuttleInfo.MapChannelIndex.MapIndex))					// 2006-06-07 by ispark
	{
		if(g_pGround->IntersectTriangle(orig,dir,g_pGround->m_pTileVertexArray[i*(g_pGround->m_projectInfo.sYSize+1)+j].pos,
			g_pGround->m_pTileVertexArray[i*(g_pGround->m_projectInfo.sYSize+1)+(j+1)].pos,
			g_pGround->m_pTileVertexArray[(i+1)*(g_pGround->m_projectInfo.sYSize+1)+(j)].pos,
			&fDist))//, &fBary1, &fBary2 ))
		{
			v = orig + dir*fDist;
		}
		else if(g_pGround->IntersectTriangle(orig,dir,g_pGround->m_pTileVertexArray[i*(g_pGround->m_projectInfo.sYSize+1)+j+1].pos,
			g_pGround->m_pTileVertexArray[(i+1)*(g_pGround->m_projectInfo.sYSize+1)+(j)].pos,
			g_pGround->m_pTileVertexArray[(i+1)*(g_pGround->m_projectInfo.sYSize+1)+j+1].pos,
			&fDist))//, &fBary1, &fBary2 ))
		{
			v = orig + dir*fDist;
		}
		if(CheckIsWater(v))
		{
			if(v.y < g_pGround->m_projectInfo.fWaterHeight)
				v.y = g_pGround->m_projectInfo.fWaterHeight;
		}
	}
	if(v2.y <= v.y)
		v2 = v;
	
	orig = m_vPos + 5.0f*m_vUp - 6.0f*m_vSideVel;	// Right
	D3DXMatrixLookAtLH(&matTemp,&orig,&(orig - m_vBasisUp),&vSide);
	fDistTemp = g_pScene->m_pObjectRender->CheckCollMesh(matTemp,orig, FALSE).fDist;
	//		if(fDistTemp > 14.0f)
	//			fDistTemp = 14.0f;
	v3 = orig - m_vBasisUp*fDistTemp;
	v = D3DXVECTOR3(0,0,0);
	i = (int)(orig.x/TILE_SIZE);
	j = (int)(orig.z/TILE_SIZE);
	if(i >= 0 && i < g_pGround->m_projectInfo.sXSize && j >= 0 && j < g_pGround->m_projectInfo.sYSize
		&&IsTileMapRenderEnable(g_pShuttleChild->m_myShuttleInfo.MapChannelIndex.MapIndex))					// 2006-06-07 by ispark
	{
		if(g_pGround->IntersectTriangle(orig,dir,g_pGround->m_pTileVertexArray[i*(g_pGround->m_projectInfo.sYSize+1)+j].pos,
			g_pGround->m_pTileVertexArray[i*(g_pGround->m_projectInfo.sYSize+1)+(j+1)].pos,
			g_pGround->m_pTileVertexArray[(i+1)*(g_pGround->m_projectInfo.sYSize+1)+(j)].pos,
			&fDist))//, &fBary1, &fBary2 ))
		{
			v = orig + dir*fDist;
		}
		else if(g_pGround->IntersectTriangle(orig,dir,g_pGround->m_pTileVertexArray[i*(g_pGround->m_projectInfo.sYSize+1)+j+1].pos,
			g_pGround->m_pTileVertexArray[(i+1)*(g_pGround->m_projectInfo.sYSize+1)+(j)].pos,
			g_pGround->m_pTileVertexArray[(i+1)*(g_pGround->m_projectInfo.sYSize+1)+j+1].pos,
			&fDist))//, &fBary1, &fBary2 ))
		{
			v = orig + dir*fDist;
		}
		if(CheckIsWater(v))
		{
			if(v.y < g_pGround->m_projectInfo.fWaterHeight)
				v.y = g_pGround->m_projectInfo.fWaterHeight;
		}
	}
	if(v3.y <= v.y)
		v3 = v;
	
	if(abs(v1.y - v2.y) > 50.0f || abs(v2.y - v3.y) > 50.0f)
	{// 
		vNextUp = m_vUp;
	}
	else
	{
		D3DXVec3Cross(&vNextUp,&(v3-v2),&(v1-v2));
		D3DXVec3Normalize(&vNextUp,&vNextUp);
	}
	// 2008-01-09 by dgwoo UP vector°ˇ 0,0,0ŔĚ¸é ĽöĆňŔŻÁö!
	if(vNextUp == D3DXVECTOR3(0,0,0))
	{
		vNextUp = D3DXVECTOR3(0,1,0);
	}
	
	v1.y += 5.0f;
	D3DXVec3Normalize(&vNextVel,&(v1-m_vPos));
	//////////////////////////////////////////////////////	
	if(m_bOnObject)
	{
		D3DXMatrixLookAtLH(&matTemp,&m_vPos,&(m_vPos - m_vBasisUp),&vSide);
		fTempHeight = g_pScene->m_pObjectRender->CheckCollMesh(matTemp,m_vPos, FALSE).fDist;
	}
	
	if(fTempHeight <= m_fAmorHeight && fHeight + m_fAmorHeight < m_vPos.y + m_fAmorHeight - fTempHeight)
	{
		// żŔşęÁ§Ć®Ŕ§żˇ ŔÖŔ»¶§
		fHeight = m_vPos.y + m_fAmorHeight - fTempHeight;
		m_vPos.y += m_fAmorHeight - fTempHeight;
	}
	else if((m_vPos.y - fHeight) > m_fAmorHeight+10.0f)//10.0f : A±âľî ŔŻ´Ö Â÷ĂĽ łôŔĚ
	{
		// °řÁßżˇ ŔÖŔ»¶§
		vNextUp = D3DXVECTOR3(0,1,0);
		vNextVel.y = 0.0f;
		D3DXVec3Normalize(&vNextVel,&vNextVel);
	}
	
	m_vUp += 1.0f*(vNextUp-m_vUp)*fElapsedTime;
	//		if(m_vUp.y < 0.0f)
	//			m_vUp.y *= -1.0f;
	//		m_vVel += 1.0f*(vNextVel-m_vVel)*fElapsedTime;
	
	/// 2004.06.11 jschoi - ĂćµąĂĽĹ© Ăß°ˇ
	CheckCollForObject(fElapsedTime);		// Âř·ú »óĹÂŔÇ Ăćµą ĂĽĹ© 
	
	// ¸ř°ˇ´Â ÁöżŞĂĽĹ©ÇĎż© »Ş˝ĂĹ˛´Ů.
	CheckMapRegion();
	
	
	
	int nXCheck, nYCheck;
	nXCheck = (int)(m_vPos.x / TILE_SIZE);
	nYCheck = (int)(m_vPos.z / TILE_SIZE);

	if (nXCheck < 0)
		nXCheck = 0;

	if (nYCheck < 0)
		nYCheck = 0;

	m_bMoveGroundType = g_pGround->m_pTileInfo[nXCheck*g_pGround->m_projectInfo.sXSize + nYCheck].bMove;
	if(m_bMoveGroundType == 2)
	{
		m_fShuttleSpeed *= -0.5f;
		m_fLRVelRate *= 0.8f;
		
		// ĂÖĂĘ Á˘ĽÓ˝Ă ŔĚµż şŇ°ˇ ÁöżŞŔÎ°ćżě ąö±× »ý±č edited by dhkwon 2003.8.20
		if(m_vOldPos.x != -1 && m_vOldPos.y != -1 && m_vOldPos.z != -1)
		{
			m_vPos = m_vPos + 20.0f*(m_vPos-m_vOldPos)*fElapsedTime;
		}
	}
	////////////////////////ż©±â±îÁö Ĺ¬¶óŔĚľđĆ® ÁÂÇĄ ĽłÁ¤(ŔĚµż)//////////////////////////////////////////////////////
	
	// 2005-04-01 by jschoi
	SetMatrix_Move_Ground(fElapsedTime);	//	SetEffectPos_A_Gear(fElapsedTime);
		
	// ŔĚşĄĆ® ÁöżŞ ĂĽĹ© - Shop
	int nCheckX = ((int)m_vPos.x)/TILE_SIZE;
	int nCheckZ = ((int)m_vPos.z)/TILE_SIZE;

	if (nCheckX < 0)
		nCheckX = 0;

	if (nCheckZ < 0)
		nCheckZ = 0;

	if((g_pGround->m_pTileInfo[m_ptOldPoint.x*g_pGround->m_projectInfo.sYSize + m_ptOldPoint.y].bEventType 
		!= g_pGround->m_pTileInfo[nCheckX*g_pGround->m_projectInfo.sYSize + nCheckZ].bEventType)
		&& (g_pGround->m_pTileInfo[nCheckX*g_pGround->m_projectInfo.sYSize + nCheckZ].bEventType == EVENT_TYPE_ENTER_BUILDING
		|| g_pGround->m_pTileInfo[nCheckX*g_pGround->m_projectInfo.sYSize + nCheckZ].bEventType == EVENT_TYPE_LANDING))
	{
		// »óÁˇŔ¸·Î µéľî°¨Ŕ» ľË¸°´Ů
//		if(m_fDTEventCheckTime <= 0.0f && m_vPos.y <= fHeight + m_fAmorHeight)
		{
			// °­Á¦·Î Move¸¦ ŔüĽŰÇŃ´Ů.
			//				SendPacket(T_FC_MOVE);
			//				g_pD3dApp->SendMovePacket(FALSE);
			
			g_pD3dApp->m_vClientOldPos = m_vPos;
			//				g_pD3dApp->m_dwLastTickTime = g_pD3dApp->m_dwCurTickTime;
			
			m_fDTEventCheckTime = 0.01f;
			
			SendPacket(T_FC_MOVE_LANDING_DONE);
			DBGOUT("T_FC_MOVE_LANDING_DONE\n");

			// 2007-01-02 by bhsohn A±âľî b+Enter·Î ¸ŘĂá ČÄ, Âř·úÇĎ¸é ľČ żňÁ÷ż©Áö´Â Çö»ó Ăł¸®								
			if(IS_DT(m_myShuttleInfo.UnitKind))
			{
				// A±âľî ¶ó¸é żňÁ÷ŔÓ ĂĘ±âČ­
				InitUnitStop();
			}				
			// end 2007-01-02 by bhsohn A±âľî b+Enter·Î ¸ŘĂá ČÄ, Âř·úÇĎ¸é ľČ żňÁ÷ż©Áö´Â Çö»ó Ăł¸®

			//				m_bEventReady = TRUE;			
		}
	}
	else if(g_pGround->m_pTileInfo[nCheckX*g_pGround->m_projectInfo.sYSize + nCheckZ].bEnableLand)
	{
		if(!COMPARE_BODYCON_BIT(GetCurrentBodyCondition(),BODYCON_LANDED_MASK) 
			&& m_fDTEventCheckTime <= 0.0f && m_vPos.y <= fHeight + m_fAmorHeight)
		{
			// °­Á¦·Î Move¸¦ ŔüĽŰÇŃ´Ů.
			//				SendPacket(T_FC_MOVE);
			//				g_pD3dApp->SendMovePacket(FALSE);
			g_pD3dApp->m_vClientOldPos = m_vPos;
			//				g_pD3dApp->m_dwLastTickTime = g_pD3dApp->m_dwCurTickTime;
			
			m_fDTEventCheckTime = 0.01f;
			SendPacket(T_FC_MOVE_LANDING_DONE);
			DBGOUT("T_FC_MOVE_LANDING_DONE\n");
			//				m_bEventReady = TRUE;
			
			ChangeSingleBodyCondition(BODYCON_LANDED_MASK);
			
			// 2007-01-02 by bhsohn A±âľî b+Enter·Î ¸ŘĂá ČÄ, Âř·úÇĎ¸é ľČ żňÁ÷ż©Áö´Â Çö»ó Ăł¸®								
			if(IS_DT(m_myShuttleInfo.UnitKind))
			{
				// A±âľî ¶ó¸é żňÁ÷ŔÓ ĂĘ±âČ­
				InitUnitStop();
			}
			// end 2007-01-02 by bhsohn A±âľî b+Enter·Î ¸ŘĂá ČÄ, Âř·úÇĎ¸é ľČ żňÁ÷ż©Áö´Â Çö»ó Ăł¸®
		}
	}
	else
	{
		//			//  »óÁˇ şÎşĐ : ±¸ÇöÇŘľß ÇŃ´Ů.
		//			if( g_pD3dApp->m_dwGameState == _SHOP &&
		//				g_pGameMain->m_pShop->m_pNPCWindow->m_buildingInfo.BuildingIndex != 0)
		//			{
		//				SendPacket(T_FC_EVENT_LEAVE_BUILDING);
		//				//´őŔĚ»ó ĆĐĹ¶Ŕ» ľČş¸ł»±â Ŕ§ÇŘ ąĚ¸® łŞżÂ´Ů.
		//				g_pD3dApp->ChangeGameState(_GAME);
		//				m_fEventCheckTime = 2.0f;
		//				m_bEventReady = TRUE;
		//				ChangeUnitState( _NORMAL );
		//				//m_dwState = _NORMAL;
		//				g_pGameMain->m_pMission->m_bShow = FALSE;
		//
		//				//SET_BODYCON_BIT(m_myShuttleInfo.BodyCondition,BODYCON_TAKEOFF_MASK);
		//				if(!COMPARE_BODYCON_BIT(GetCurrentBodyCondition(),BODYCON_TAKEOFF_MASK))
		//				{
		//					ChangeSingleBodyCondition(BODYCON_TAKEOFF_MASK);
		//				}
		//
		//			}
		/*			else if(COMPARE_BODYCON_BIT(m_myShuttleInfo.BodyCondition,BODYCON_LANDED_MASK) && )
		{
		SendPacket(T_FC_MOVE_TAKEOFF);
		
		  SET_BODYCON_BIT(m_myShuttleInfo.BodyCondition,BODYCON_TAKEOFF_MASK);
		  if(m_pCharacterInfo)
		  m_pCharacterInfo->ChangeBodyCondition(m_myShuttleInfo.BodyCondition);
		  if(m_pEngine && m_pEngine->m_pCharacterInfo)
		  {
		  m_pEngine->ChangeBodyCondition(BODYCON_BOOSTER1_MASK);
		  m_pEngine->m_bodyCondition = BODYCON_BOOSTER1_MASK;
		  m_pEngine->m_pCharacterInfo->ChangeBodyCondition(m_pEngine->m_bodyCondition);
		  }
		  }
		*/			if(m_fDTEventCheckTime >= 0.0f)
		m_fDTEventCheckTime -= fElapsedTime;
	}
	
	// 2007-04-17 by bhsohn °ˇ±îżî °Ĺ¸®żˇĽ­ŔÇ ˝ĂÁî¸đµĺ˝Ă µĄŔĚÁö ľČµéľî°ˇ´Â Çö»ó Ăł¸®
	m_bAGearFollowEnemy = FALSE;

	// ˝ĂÁî¸đµĺ
	if(m_bAttackMode == _SIEGE)
	{
		if(g_pGameMain->bOpenInvReq)
			g_pGameMain->bOpenInvReq = false; //close inventory when in siege mode

		SiegeTarget();
		// 2010-06-15 by shcho&hslee Ćę˝Ă˝şĹŰ - ˝ĂÁî ¸đµĺ˝Ă Ćę Ľű±â±â 
		HidePet(TRUE);
		// end 2010-06-15 by shcho&hslee Ćę˝Ă˝şĹŰ - ˝ĂÁî ¸đµĺ˝Ă Ćę Ľű±â±â 
	}
	// ŔĎąÝ¸đµĺ
	else
	{
		// 2006-01-03 by ispark, ŔĎąÝ A±âľî ą«şę ±×¶óżîµĺ »óĹÂ (Ĺ¸°ŮŔĚ ŔÖ´ř, ľř´ř)
		m_vWeaponVel = m_vMouseDir;
		// 2010-06-15 by shcho&hslee Ćę˝Ă˝şĹŰ - ˝ĂÁî ¸đµĺ˝Ă Ćę Ľű±â±â 
		m_vPetLeftVel	= m_vMouseDir;
		m_vPetRightVel	= m_vMouseDir;
		HidePet(FALSE);
		// end 2010-06-15 by shcho&hslee Ćę˝Ă˝şĹŰ - ˝ĂÁî ¸đµĺ˝Ă Ćę Ľű±â±â 
	}			

//	m_pAutoTargetOldMousePos = pt;
	
	// ŔŻ´Ö ¶ô»óĹÂŔĎ¶§ Ć÷ŔĚµż ľřŔ˝
	if(m_bMouseLock == TRUE) m_vWeaponVel = m_vVel;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			void CShuttleChild::SiegeTarget(float fElapsedTime)
/// \brief		˝ĂÁî ¸đµĺ˝Ă Ĺ¸°ŮŔ» ŔâŔş »óĹÂ·Î °čĽÓ ¶§¸± Ľö ŔÖµµ·Ď.
/// \author		dgwoo
/// \date		2006-11-16 ~ 2006-11-16
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CShuttleChild::SiegeTarget()
{
	POINT pt;							// ÇöŔç ¸¶żě˝ş Ć÷ŔÎĹÍ
	POINT pt2;							// ¸ó˝şĹÍŔÇ 2DŔ§Äˇ.
	BOOL  pointmoveflag = FALSE;
	GetCursorPos(&pt);
	// OrderĹ¸°ŮŔĚ ŔÖŔ» ¶§
	if(m_pOrderTarget)
	{
		// Ĺ¸°ŮŔ» µű¶ó°ˇ±â
		//--------------------------------------------------------------------------//
		// 2006-11-03 by dgwoo
		pointmoveflag = TRUE;

		// 2011. 03. 08 by jskim ŔÎÇÇ3Â÷ ±¸Çö - łÍ Ĺ¸°Ů ˝Ă˝şĹŰ
// 		pt2.x = m_pOrderTarget->m_nObjScreenX;
// 		pt2.y = m_pOrderTarget->m_nObjScreenY;
		D3DXVECTOR3 temp;
		// 2011. 09. 28 by jskim łÍ Ĺ¸°Ů ˝Ă˝şĹŰ ąö±× ĽöÁ¤( ÄÉ˝şĆĂ ą®Á¦ )
		//if( ((CMonsterData*)m_pOrderTarget)->m_vecvmultiData.size() )
		if( m_pOrderTarget &&
			m_pOrderTarget->m_dwPartType == _MONSTER &&
			((CMonsterData*)m_pOrderTarget)->m_vecvmultiData.size() )
		// end 2011. 09. 28 by jskim łÍ Ĺ¸°Ů ˝Ă˝şĹŰ ąö±× ĽöÁ¤( ÄÉ˝şĆĂ ą®Á¦ )
		{
			// 2011. 03. 08 by jskim ŔÎÇÇ3Â÷ ±¸Çö - łÍ Ĺ¸°Ů ˝Ă˝şĹŰ
			temp = ((CMonsterData*)m_pOrderTarget)->GetMultiPos( ((CMonsterData*)m_pOrderTarget)->m_nMultiIndex );
			int tempX, tempY, tempW;

			((CMonsterData*)m_pOrderTarget)->GetScreenPos( temp, tempX, tempY, tempW ); 
			pt2.x = tempX;
			pt2.y = tempY;
		}
		else
		{
			temp = m_pOrderTarget->m_vPos;
			pt2.x = m_pOrderTarget->m_nObjScreenX;
			pt2.y = m_pOrderTarget->m_nObjScreenY;
		}
		// end 2011. 03. 08 by jskim ŔÎÇÇ3Â÷ ±¸Çö - łÍ Ĺ¸°Ů ˝Ă˝şĹŰ


		ClientToScreen(g_pD3dApp->GetHwnd(), &pt2);
		ScreenToClient(g_pD3dApp->GetHwnd(), &pt);
		CheckMouseReverse(&pt);
		ClientToScreen(g_pD3dApp->GetHwnd(),&pt);
		
		if(pt2.x + SIGEMODE_MOUSE_FITPOINT < pt.x ||
			pt2.x - SIGEMODE_MOUSE_FITPOINT > pt.x)
		{
			pointmoveflag = FALSE;
			// 2011. 09. 28 by jskim łÍ Ĺ¸°Ů ˝Ă˝şĹŰ ąö±× ĽöÁ¤( ÄÉ˝şĆĂ ą®Á¦ )
			//// 2011. 03. 08 by jskim ŔÎÇÇ3Â÷ ±¸Çö - łÍ Ĺ¸°Ů ˝Ă˝şĹŰ
			//((CMonsterData*)m_pOrderTarget)->m_bSiegeMode = FALSE;
			//// end 2011. 03. 08 by jskim ŔÎÇÇ3Â÷ ±¸Çö - łÍ Ĺ¸°Ů ˝Ă˝şĹŰ
			if( m_pOrderTarget && m_pOrderTarget->m_dwPartType == _MONSTER )
			{
			((CMonsterData*)m_pOrderTarget)->m_bSiegeMode = FALSE;
			}			
			// end 2011. 09. 28 by jskim łÍ Ĺ¸°Ů ˝Ă˝şĹŰ ąö±× ĽöÁ¤( ÄÉ˝şĆĂ ą®Á¦ )
		}
		if(pt2.y + SIGEMODE_MOUSE_FITPOINT < pt.y ||
			pt2.y - SIGEMODE_MOUSE_FITPOINT > pt.y)
		{
			pointmoveflag = FALSE;
			// 2011. 09. 28 by jskim łÍ Ĺ¸°Ů ˝Ă˝şĹŰ ąö±× ĽöÁ¤( ÄÉ˝şĆĂ ą®Á¦ )
			//// 2011. 03. 08 by jskim ŔÎÇÇ3Â÷ ±¸Çö - łÍ Ĺ¸°Ů ˝Ă˝şĹŰ
			//((CMonsterData*)m_pOrderTarget)->m_bSiegeMode = FALSE;
			//// end 2011. 03. 08 by jskim ŔÎÇÇ3Â÷ ±¸Çö - łÍ Ĺ¸°Ů ˝Ă˝şĹŰ
			if( m_pOrderTarget && m_pOrderTarget->m_dwPartType == _MONSTER )
			{
			((CMonsterData*)m_pOrderTarget)->m_bSiegeMode = FALSE;
			}			
			// end 2011. 09. 28 by jskim łÍ Ĺ¸°Ů ˝Ă˝şĹŰ ąö±× ĽöÁ¤( ÄÉ˝şĆĂ ą®Á¦ )
		}
		pt2 = pt;

		ScreenToClient(g_pD3dApp->GetHwnd(), &pt);
		//CheckMouseReverse(&pt);
		//--------------------------------------------------------------------------//
		// Ĺ¬¶óŔĚľđĆ®żÍ ˝şĹ©¸° ÁÂÇĄŔÇ Â÷¸¦ ±¸ÇŃ´Ů.
		int nX = pt2.x - pt.x;
		int nY = pt2.y - pt.y;
		
		// ±× Â÷¸¸Ĺ­ Ĺ¬¶óŔĚľđĆ®ŔÇ Áß˝ÉÁÂÇĄ¸¦ nX,nYżˇ łÖľîÁŘ´Ů.
		nX += g_pD3dApp->GetBackBufferDesc().Width/2 + g_pD3dApp->GetBackBufferDesc().Width/800;
		nY += g_pD3dApp->GetBackBufferDesc().Height/2 + g_pD3dApp->GetBackBufferDesc().Height/600;	
		if(pointmoveflag) 
		{
			// 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
			m_bSetCursorPos = TRUE;

			SetCursorPos(nX,nY);
			// 2011. 03. 08 by jskim ŔÎÇÇ3Â÷ ±¸Çö - łÍ Ĺ¸°Ů ˝Ă˝şĹŰ
			//D3DXVec3Normalize(&m_vWeaponVel,&(m_pOrderTarget->m_vPos - m_vPos));
 			D3DXVec3Normalize(&m_vWeaponVel,&(temp - m_vPos));

			// Ć÷ŔÇ ŔĎÁ¤°˘µµ ľĆ·ˇ·Î ł»·Á°ˇÁö ľĘ´Â´Ů.
			float dot2 = D3DXVec3Dot(&m_vUp, &m_vWeaponVel);
			if(dot2 < -0.3f && (m_pt.y > (g_pD3dApp->m_d3dsdBackBuffer.Height / 2)))
			{
				double dRadian = 0.0f;
				D3DXVECTOR3 vtempWeaponVel;
				D3DXVECTOR3 vVelF = m_vWeaponVel;
				D3DXVECTOR3 vVelS;
				D3DXMATRIX matRot;
				D3DXVec3Normalize(&vVelF, &vVelF);
				D3DXVec3Cross(&vVelS, &m_vUp, &vVelF);
				vtempWeaponVel = m_vUp;
				dRadian = ACOS(-0.37);
				D3DXMatrixRotationAxis(&matRot , &vVelS , dRadian);
				D3DXVec3Normalize(&vtempWeaponVel, &vtempWeaponVel);
				D3DXVec3TransformCoord(&vtempWeaponVel , &vtempWeaponVel , &matRot); // view * rot·Î »ő·Îżî dst vector¸¦ ±¸ÇŃ´Ů.
				m_vWeaponVel = vtempWeaponVel;
				D3DXVec3Normalize(&m_vWeaponVel, &m_vWeaponVel);
			}		
		}		
	}

	// Ĺ¸°ŮŔĚ ľř°ĹłŞ Ĺ¸°Ůżˇ ¸Ö¸® ¶łľîÁ® ŔÖŔ» ¶§
	// 2008-12-16 by bhsohn A±âľî °řÁß˝ĂÁö ±â´É ľ÷±×·ąŔĚµĺ				
//	if(pointmoveflag == FALSE)
//	{	
//		D3DXVECTOR3 Av, Uv;					// Ć÷ŔĚµż ĽÓµµ°ü·Ă 
//		D3DXMATRIX matV;
//		
//		D3DXVECTOR3 v, MouseD;				// Ŕ§Äˇ°ü·Ă ¸¶żě˝ş °ü·Ă 
//		D3DXMATRIX matProj,matView,matTemp;;
//		
//		float m_fAnimationShotMove = A_GEAR_SIGEMODE_MOVETIMING;
//		D3DXVec3Lerp(&Av, &m_vWeaponVel, &m_vMouseDir, m_fAnimationShotMove);				
//		
//		// 2007-04-13 by bhsohn A±âľî Ć÷´ëą®Á¦
//		D3DXVec3Normalize(&Av, &Av);
//		// °˘(¶óµđľČ)Ŕ» ±¸ÇŃ´Ů. 		
//		float fAngleRadian = GetRadianVectorBetweenVector(m_vUp, m_vWeaponVel);
//		if(fAngleRadian < AGEAR_SIEGEMODE_MAX_ANGLE_RADIAN)		
//		{		
//			// Ć÷ŔÇ ŔĎÁ¤°˘µµ Ŕ§·Î żĂ¶ó °ˇÁö ľĘ´Â´Ů.
//			m_vWeaponVel = GetRadianVector(m_vWeaponVel, m_vUp, AGEAR_SIEGEMODE_MAX_ANGLE_RADIAN+AGEAR_SIEGEMODE_MIN_CAP);			
//				}
//		else if(fAngleRadian > AGEAR_SIEGEMODE_MIN_ANGLE_RADIAN)	
//		{
//				// Ć÷ŔÇ ŔĎÁ¤°˘µµ ľĆ·ˇ·Î ł»·Á°ˇÁö ľĘ´Â´Ů.
//			m_vWeaponVel = GetRadianVector(m_vWeaponVel, m_vUp, AGEAR_SIEGEMODE_MIN_ANGLE_RADIAN-AGEAR_SIEGEMODE_MIN_CAP);
//			}
//			else
//			{
//			float fAngleAv = GetRadianVectorBetweenVector(Av, m_vUp);
//			if(fAngleAv < AGEAR_SIEGEMODE_MAX_ANGLE_RADIAN)		
//			{
//				// Ć÷ŔÇ ŔĎÁ¤°˘µµ Ŕ§·Î żĂ¶ó °ˇÁö ľĘ´Â´Ů.
//				Av = GetRadianVector(Av, m_vUp, AGEAR_SIEGEMODE_MAX_ANGLE_RADIAN);				
//			}
//			else if(fAngleAv > AGEAR_SIEGEMODE_MIN_ANGLE_RADIAN)	
//			{
//				// Ć÷ŔÇ ŔĎÁ¤°˘µµ ľĆ·ˇ·Î ł»·Á°ˇÁö ľĘ´Â´Ů.
//				Av = GetRadianVector(Av, m_vUp, AGEAR_SIEGEMODE_MIN_ANGLE_RADIAN);
//			}			
//				m_vWeaponVel = Av;
//			}									
//	}	
	if(pointmoveflag == FALSE)
	{	
		if(m_bIsAir == FALSE)
		{			
			GetSiegeUpVelVector(&m_vWeaponVel, &m_vUp);
		}
		else
		{
			GetAirSiegeUpVelVector(&m_vWeaponVel, &m_vUp);
		}		
	}	
	// end 2008-12-16 by bhsohn A±âľî °řÁß˝ĂÁö ±â´É ľ÷±×·ąŔĚµĺ

	// 2007-04-17 by bhsohn °ˇ±îżî °Ĺ¸®żˇĽ­ŔÇ ˝ĂÁî¸đµĺ˝Ă µĄŔĚÁö ľČµéľî°ˇ´Â Çö»ó Ăł¸®
	m_bAGearFollowEnemy = pointmoveflag;
}
///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		A±âľî °řÁß˝ĂÁö ±â´É ľ÷±×·ąŔĚµĺ
/// \author		// 2008-12-16 by bhsohn A±âľî °řÁß˝ĂÁö ±â´É ľ÷±×·ąŔĚµĺ
/// \date		2008-12-16 ~ 2008-12-16
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CShuttleChild::GetSiegeUpVelVector(D3DXVECTOR3	*o_vWeaponVel,	D3DXVECTOR3* o_vWeaponUp)
{
	D3DXVECTOR3	vTmpWeaponVel = (*o_vWeaponVel);
	D3DXVECTOR3	vTmpWeaponUp = (*o_vWeaponUp);
	
	D3DXVECTOR3 Av, Uv;					// Ć÷ŔĚµż ĽÓµµ°ü·Ă 
	D3DXMATRIX matV;
	
	D3DXVECTOR3 v, MouseD;				// Ŕ§Äˇ°ü·Ă ¸¶żě˝ş °ü·Ă 
	D3DXMATRIX matProj,matView,matTemp;;
	
	float fAnimationShotMove = A_GEAR_SIGEMODE_MOVETIMING;
	D3DXVec3Lerp(&Av, &vTmpWeaponVel, &m_vMouseDir, fAnimationShotMove);				
	
	// 2007-04-13 by bhsohn A±âľî Ć÷´ëą®Á¦
	D3DXVec3Normalize(&Av, &Av);
	// °˘(¶óµđľČ)Ŕ» ±¸ÇŃ´Ů. 		
	float fAngleRadian = GetRadianVectorBetweenVector(vTmpWeaponUp, vTmpWeaponVel);
	if(fAngleRadian < AGEAR_SIEGEMODE_MAX_ANGLE_RADIAN)		
	{		
		// Ć÷ŔÇ ŔĎÁ¤°˘µµ Ŕ§·Î żĂ¶ó °ˇÁö ľĘ´Â´Ů.
		vTmpWeaponVel = GetRadianVector(vTmpWeaponVel, vTmpWeaponUp, AGEAR_SIEGEMODE_MAX_ANGLE_RADIAN+AGEAR_SIEGEMODE_MIN_CAP);			
	}
	else if(fAngleRadian > AGEAR_SIEGEMODE_MIN_ANGLE_RADIAN)	
	{
		// Ć÷ŔÇ ŔĎÁ¤°˘µµ ľĆ·ˇ·Î ł»·Á°ˇÁö ľĘ´Â´Ů.
		vTmpWeaponVel = GetRadianVector(vTmpWeaponVel, vTmpWeaponUp, AGEAR_SIEGEMODE_MIN_ANGLE_RADIAN-AGEAR_SIEGEMODE_MIN_CAP);
	}
	else
	{
		float fAngleAv = GetRadianVectorBetweenVector(Av, vTmpWeaponUp);
		if(fAngleAv < AGEAR_SIEGEMODE_MAX_ANGLE_RADIAN)		
		{
			// Ć÷ŔÇ ŔĎÁ¤°˘µµ Ŕ§·Î żĂ¶ó °ˇÁö ľĘ´Â´Ů.
			Av = GetRadianVector(Av, vTmpWeaponUp, AGEAR_SIEGEMODE_MAX_ANGLE_RADIAN);				
		}
		else if(fAngleAv > AGEAR_SIEGEMODE_MIN_ANGLE_RADIAN)	
		{
			// Ć÷ŔÇ ŔĎÁ¤°˘µµ ľĆ·ˇ·Î ł»·Á°ˇÁö ľĘ´Â´Ů.
			Av = GetRadianVector(Av, vTmpWeaponUp, AGEAR_SIEGEMODE_MIN_ANGLE_RADIAN);
		}			
		vTmpWeaponVel = Av;
	}			

	(*o_vWeaponVel) = vTmpWeaponVel;
	(*o_vWeaponUp) = vTmpWeaponUp;
	
}
///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		A±âľî °řÁß˝ĂÁö ±â´É ľ÷±×·ąŔĚµĺ
/// \author		// 2008-12-16 by bhsohn A±âľî °řÁß˝ĂÁö ±â´É ľ÷±×·ąŔĚµĺ
/// \date		2008-12-16 ~ 2008-12-16
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CShuttleChild::GetAirSiegeUpVelVector(D3DXVECTOR3	*o_vWeaponVel,	D3DXVECTOR3* o_vWeaponUp)
{
	D3DXVECTOR3	vTmpWeaponVel = (*o_vWeaponVel);
	D3DXVECTOR3	vTmpWeaponUp = (*o_vWeaponUp);

	BOOL bRefreshUpVector = FALSE;
	BOOL bRefreshVelVector = FALSE;
	float fAnimationShotMove = A_GEAR_SIGEMODE_MOVETIMING;
	
	D3DXVECTOR3 Av, Uv;					// Ć÷ŔĚµż ĽÓµµ°ü·Ă 
	D3DXMATRIX matV;
	
	D3DXVECTOR3 v, MouseD;				// Ŕ§Äˇ°ü·Ă ¸¶żě˝ş °ü·Ă 
	D3DXMATRIX matProj,matView,matTemp;;
	
	
	D3DXVec3Lerp(&Av, &vTmpWeaponVel, &m_vMouseDir, fAnimationShotMove);				
	
	// 2007-04-13 by bhsohn A±âľî Ć÷´ëą®Á¦
	D3DXVec3Normalize(&Av, &Av);
	// °˘(¶óµđľČ)Ŕ» ±¸ÇŃ´Ů. 		
	float fAngleRadian = GetRadianVectorBetweenVector(vTmpWeaponUp, vTmpWeaponVel);
	if(fAngleRadian < AGEAR_SIEGEMODE_MAX_ANGLE_RADIAN)		
	{	
		// Ć÷ŔÇ ŔĎÁ¤°˘µµ Ŕ§·Î żĂ¶ó °ˇÁö ľĘ´Â´Ů.
		//vTmpWeaponVel = GetRadianVector(vTmpWeaponVel, vTmpWeaponUp, AGEAR_SIEGEMODE_MAX_ANGLE_RADIAN+AGEAR_SIEGEMODE_MIN_CAP);			
		bRefreshUpVector= TRUE;
		bRefreshVelVector = TRUE;
	}
	else if(fAngleRadian > AGEAR_SIEGEMODE_MIN_ANGLE_RADIAN)	
	{
		// Ć÷ŔÇ ŔĎÁ¤°˘µµ ľĆ·ˇ·Î ł»·Á°ˇÁö ľĘ´Â´Ů.
		//vTmpWeaponVel = GetRadianVector(vTmpWeaponVel, vTmpWeaponUp, AGEAR_SIEGEMODE_MIN_ANGLE_RADIAN-AGEAR_SIEGEMODE_MIN_CAP);
		bRefreshUpVector= TRUE;
	}
	else
	{
		float fAngleAv = GetRadianVectorBetweenVector(Av, vTmpWeaponUp);
		if(fAngleAv < AGEAR_SIEGEMODE_MAX_ANGLE_RADIAN)		
		{
			// Ć÷ŔÇ ŔĎÁ¤°˘µµ Ŕ§·Î żĂ¶ó °ˇÁö ľĘ´Â´Ů.
			//Av = GetRadianVector(Av, vTmpWeaponUp, AGEAR_SIEGEMODE_MAX_ANGLE_RADIAN);				
			bRefreshUpVector= TRUE;
			bRefreshVelVector = TRUE;
		}
		else if(fAngleAv > AGEAR_SIEGEMODE_MIN_ANGLE_RADIAN)	
		{
			// Ć÷ŔÇ ŔĎÁ¤°˘µµ ľĆ·ˇ·Î ł»·Á°ˇÁö ľĘ´Â´Ů.
			//Av = GetRadianVector(Av, vTmpWeaponUp, AGEAR_SIEGEMODE_MIN_ANGLE_RADIAN);
			bRefreshUpVector= TRUE;
		}			
		vTmpWeaponVel = Av;
	}	
	if(bRefreshUpVector)
	{
		// °­Á¦·Î ±âĂĽ ľ÷ş¤ĹÍ¸¦ şŻ°ć˝ĂĹ˛´Ů.
		D3DXVECTOR3 vUpTmp;
		D3DXVec3Lerp(&vUpTmp, &vTmpWeaponUp, &m_vMouseDir, fAnimationShotMove);
		D3DXVec3Normalize(&vUpTmp, &vUpTmp);
		vTmpWeaponUp = vUpTmp;
		
		if(bRefreshVelVector)
		{
			vTmpWeaponVel = GetRadianVector(vTmpWeaponVel, vTmpWeaponUp, 
				AGEAR_SIEGEMODE_MAX_ANGLE_RADIAN+AGEAR_SIEGEMODE_MIN_CAP);
		}
	}
	(*o_vWeaponVel) = vTmpWeaponVel;
	(*o_vWeaponUp) = vTmpWeaponUp;	
}
//*/
void CShuttleChild::SetMatrix_Move_Ground(float fElapsedTime) // void CShuttleChild::SetEffectPos_A_Gear(float fElapsedTime)
{
	D3DXVec3Normalize(&m_vUp,&m_vUp);
	D3DXVec3Normalize(&m_vVel,&m_vVel);
	D3DXVec3Cross(&m_vSideVel,&m_vUp,&m_vVel);
	D3DXVec3Cross(&m_vVel,&m_vSideVel,&m_vUp);

	D3DXMatrixLookAtRH(&m_mMatrix, &m_vPos, &(m_vPos+m_vVel), &m_vUp);
	D3DXMatrixInverse(&m_mMatrix, NULL, &m_mMatrix );

}

//void CShuttleChild::Move_I_Gear(float fElapsedTime)
//{
//	FLOG( "CShuttleChild::Move_I_Gear(float fElapsedTime)" );
//	// B-Gear żÍ I-Gear ´Â ±¸µżąć˝ÄŔĚ °°Ŕ˝
//	Move_B_Gear(fElapsedTime);
//}

void CShuttleChild::CheckBoosterState(float fElapsedTime,bool bSet,int nOldBoosterType)
{
	FLOG( "CShuttleChild::CheckBoosterState(float fElapsedTime,bool bSet,int nOldBoosterType)" );
	if(bSet)
	{
		m_nShuttleStopState = 0;
		if(m_nShuttleBoosterState == 0)
		{
			m_nShuttleBoosterState = 1;
		}
		else if(m_nShuttleBoosterState == 1)
		{
			m_nBooster = BOOSTER_TYPE_MIN;
			m_fBoosterCheckTime = BOOSTER_KEY_CHECK_TIME;
		}
		else if(m_nShuttleBoosterState == 2)
		{
			m_nShuttleBoosterState = 3;
			m_fBoosterCheckTime = BOOSTER_KEY_CHECK_TIME;
		}
		else if(m_nShuttleBoosterState == 3 )
		{
			m_fBoosterCheckTime = BOOSTER_KEY_CHECK_TIME;
//				if( m_myShuttleInfo.CurrentEP && 
//					m_bReChargeBURN && 
//					m_bUseBooster &&
//					(g_pStoreData->GetTotalWeight() / CAtumSJ::GetTransport(&g_pShuttleChild->m_myShuttleInfo)) < 0.9f)
//				{
//					m_nBooster = BOOSTER_TYPE_MAX;
//				}
//				else
//				{
//					m_nBooster = BOOSTER_TYPE_MIN;
//				}
		}
	}
	else
	{
		if(m_nShuttleBoosterState == 1)
		{
			m_nShuttleBoosterState = 2;
		}
		else if(m_nShuttleBoosterState == 2)
		{
			if(m_fBoosterCheckTime >= 0.0f)
				m_fBoosterCheckTime -= fElapsedTime;
			if(m_fBoosterCheckTime <= 0.0f)
			{
				m_fBoosterCheckTime = BOOSTER_KEY_CHECK_TIME;
				m_nShuttleBoosterState=0;
			}
		}
		else if(m_nShuttleBoosterState == 3)
		{
			if(m_fBoosterCheckTime >= 0.0f)
				m_fBoosterCheckTime -= fElapsedTime;
			if(m_fBoosterCheckTime <= 0.0f)
			{
				m_fBoosterCheckTime = BOOSTER_KEY_CHECK_TIME;
				m_nShuttleBoosterState = 0;
			}
		}
	}
}

//void CShuttleChild::CheckAirBreakState(float fElapsedTime,bool bSet)
//{
//	FLOG( "CShuttleChild::CheckAirBreakState(float fElapsedTime,bool bSet)" );
//
//	// 2005-05-06 by jschoi
//	if(	m_nShuttleStopState == 3 &&
//		(IS_DT(m_myShuttleInfo.UnitKind) || IS_OT(m_myShuttleInfo.UnitKind)) )
//	{
//		m_nShuttleStopState = 1;
//	}
//
//	if(bSet)
//	{
//		m_nBooster = BOOSTER_TYPE_STOP;
//		if(m_nShuttleStopState == 0)
//		{
//			m_nShuttleStopState = 1;
//		}
//		else if(m_nShuttleStopState == 1)
//		{
//			m_nBooster = BOOSTER_TYPE_STOP;
//			m_fBoosterCheckTime = BOOSTER_KEY_CHECK_TIME;
//		}
//		else if(m_nShuttleStopState == 2)
//		{
//			m_nShuttleStopState = 3;
//			m_fBoosterCheckTime = BOOSTER_KEY_CHECK_TIME;
//		}
//		else if(m_nShuttleStopState == 3 )
//		{
//			m_fShuttleSpeed = 0.0f;
////			m_fShuttleSpeed -= m_fShuttleSpeed*fElapsedTime;
////			if(m_fShuttleSpeed < m_fRealShuttleSpeedMin)
////			{
////				m_fShuttleSpeed += (m_fRealShuttleSpeedMin - m_fShuttleSpeed)*fElapsedTime;
////			}
//			if(m_fShuttleSpeed < 0)
//				m_fShuttleSpeed = 0;
//			m_nBooster = BOOSTER_TYPE_AIRBREAK;
//			m_fBoosterCheckTime = BOOSTER_KEY_CHECK_TIME;
//		}
//	}
//	else
//	{
//		if(m_nShuttleStopState == 1)
//		{
//			m_nShuttleStopState = 2;
//		}
//		else if(m_nShuttleStopState == 2)
//		{
//			if(m_fBoosterCheckTime >= 0.0f)
//				m_fBoosterCheckTime -= fElapsedTime;
//			if(m_fBoosterCheckTime <= 0.0f)
//			{
//				m_fBoosterCheckTime = BOOSTER_KEY_CHECK_TIME;
//				m_nShuttleStopState = 0;
//			}
//		}
//		else if(m_nShuttleStopState == 3)
//		{
//			m_fShuttleSpeed = 0.0f;
////			m_fShuttleSpeed -= m_fShuttleSpeed*fElapsedTime;
////			if(m_fShuttleSpeed < m_fRealShuttleSpeedMin)
////			{
////				m_fShuttleSpeed += (m_fRealShuttleSpeedMin - m_fShuttleSpeed)*fElapsedTime;
////			}
//			if(m_fShuttleSpeed < 0)
//				m_fShuttleSpeed = 0;
//			m_nBooster = BOOSTER_TYPE_AIRBREAK;
//		}
//	}
//}

void CShuttleChild::SendPacket(int nType,UINT64 n)
{
	FLOG( "CShuttleChild::SendPacket(int nType,UINT64 n)" );
	char buffer[SIZE_MAX_PACKET];
	switch(nType)
	{
	case T_FC_MOVE_LANDING:
		{
			MSG_FC_MOVE_LANDING sMsg;
			memset(&sMsg,0x00,sizeof(sMsg));
			int nType = T_FC_MOVE_LANDING;
			sMsg.ClientIndex = m_myShuttleInfo.ClientIndex;
			sMsg.Position = m_vPos;
			memcpy(buffer, &nType, SIZE_FIELD_TYPE_HEADER);
			memcpy(buffer+SIZE_FIELD_TYPE_HEADER, &sMsg, sizeof(sMsg));
			g_pD3dApp->m_pFieldWinSocket->Write(buffer, SIZE_FIELD_TYPE_HEADER + sizeof(sMsg));
		}
		break;
	case T_FC_CHARACTER_APPLY_COLLISION_DAMAGE:
		{
			MSG_FC_CHARACTER_APPLY_COLLISION_DAMAGE sMsg;
			memset(&sMsg,0x00,sizeof(sMsg));
			sMsg.CollisionType = n;
  			sMsg.SpeedOfCollision = m_fShuttleSpeed - m_fTempGravity;			// 2005-07-08 by ispark
			int nType = T_FC_CHARACTER_APPLY_COLLISION_DAMAGE;
			memcpy(buffer, &nType, SIZE_FIELD_TYPE_HEADER);
			memcpy(buffer+SIZE_FIELD_TYPE_HEADER, &sMsg, sizeof(sMsg));
			g_pD3dApp->m_pFieldWinSocket->Write(buffer, SIZE_FIELD_TYPE_HEADER + sizeof(sMsg));
		}
		break;
	case T_IC_PARTY_CANCEL_FLIGHT_POSITION:
		{
			MSG_IC_PARTY_CANCEL_FLIGHT_POSITION sMsg;
			memset(&sMsg,0x00,sizeof(sMsg));
			int nType = T_IC_PARTY_CANCEL_FLIGHT_POSITION;
			// 2007-11-22 by bhsohn ľĆ·ąłŞ ĹëÇŐĽ­ąö
			//sMsg.CharacterUniqueNumber = m_myShuttleInfo.CharacterUniqueNumber;
			sMsg.CharacterUniqueNumber = GetCharacterUniqueNumber();
			memcpy(buffer, &nType, SIZE_FIELD_TYPE_HEADER);
			memcpy(buffer+SIZE_FIELD_TYPE_HEADER, &sMsg, sizeof(sMsg));
			// 2007-11-22 by bhsohn ľĆ·ąłŞ ĹëÇŐĽ­ąö
			g_pD3dApp->m_pIMSocket->Write(buffer, SIZE_FIELD_TYPE_HEADER + sizeof(sMsg));
		}
		break;
	case T_FC_MOVE_LANDING_DONE:
		{
			MSG_FC_MOVE_LANDING_DONE sMsg;
			memset(&sMsg,0x00,sizeof(sMsg));
			int nType = T_FC_MOVE_LANDING_DONE;
			sMsg.Position = m_vPos;
			memcpy(buffer,&nType,SIZE_FIELD_TYPE_HEADER);
			memcpy(buffer+SIZE_FIELD_TYPE_HEADER,&sMsg,sizeof(sMsg));
			g_pD3dApp->m_pFieldWinSocket->Write(buffer,SIZE_FIELD_TYPE_HEADER + sizeof(sMsg));
			m_bIsAir = FALSE;
			ChangeSingleBodyCondition(BODYCON_LANDED_MASK);
//			SendPacket(T_FC_MOVE);
//			g_pD3dApp->SendMovePacket(FALSE);
			g_fSendMovePacket = max(g_fSendMovePacket,5.0f);
			if (g_pD3dApp->m_pSound) {
				g_pD3dApp->m_pSound->StopD3DSound(SOUND_LANDING);
				g_pD3dApp->m_pSound->PlayD3DSound(SOUND_LANDING_DONE, m_vPos, FALSE);
			}

		}
		break;
	case T_FC_CHARACTER_CHANGE_BODYCONDITION:
		{
			MSG_FC_CHARACTER_CHANGE_BODYCONDITION sMsg;
			memset(&sMsg,0x00,sizeof(sMsg));
			int nType = T_FC_CHARACTER_CHANGE_BODYCONDITION;
			sMsg.ClientIndex = m_myShuttleInfo.ClientIndex;
			sMsg.BodyCondition = n;
			memcpy(buffer, &nType, SIZE_FIELD_TYPE_HEADER);
			memcpy(buffer+SIZE_FIELD_TYPE_HEADER, &sMsg, sizeof(sMsg));
			g_pD3dApp->m_pFieldWinSocket->Write(buffer, SIZE_FIELD_TYPE_HEADER + sizeof(sMsg));
		}
		break;
	case T_FC_MOVE_TAKEOFF:
		{
			m_bAniKeyBlock = FALSE;
			MSG_FC_MOVE_TAKEOFF sMsg;
			memset(&sMsg,0x00,sizeof(sMsg));
			int nType = T_FC_MOVE_TAKEOFF;
			sMsg.ClientIndex = m_myShuttleInfo.ClientIndex;
			sMsg.Position = m_vPos;
			memcpy(buffer, &nType, SIZE_FIELD_TYPE_HEADER);
			memcpy(buffer+SIZE_FIELD_TYPE_HEADER, &sMsg, sizeof(sMsg));
			g_pD3dApp->m_pFieldWinSocket->Write(buffer, SIZE_FIELD_TYPE_HEADER + sizeof(sMsg));
			m_bIsAir = TRUE;
			if(g_pD3dApp->m_dwGameState == _SHOP)
			{
				g_pInterface->m_pCityBase->OnCityNPCButtonDown(CITY_NPC_BUTTON_CLOSE);
			}
			if(g_pD3dApp->m_pSound)
				g_pD3dApp->m_pSound->PlayD3DSound( SOUND_TAKINGOFF, m_vPos, FALSE);
		}
		break;
	case T_FC_MOVE:
		{
			MSG_FC_MOVE sMsg;
			memset(&sMsg,0x00,sizeof(sMsg));
			sMsg.ClientIndex = m_myShuttleInfo.ClientIndex;
//			g_pD3dApp->m_dwCurTickTime = timeGetTime();
//			sMsg.TimeGap = g_pD3dApp->m_dwCurTickTime - g_pD3dApp->m_dwLastTickTime;
			sMsg.TimeGap = g_pD3dApp->m_dwSpeedHackTimer;
			//float fDistance = D3DXVec3Length(&(m_vPos-g_pD3dApp->m_vClientOldPos));
			//if(fDistance > 255)
			//	fDistance = 255.0f; 
			//sMsg.DistanceGap = (BYTE)fDistance;
			sMsg.PositionVector = m_vPos;
			sMsg.TargetVector = m_vVel*1000.0f;
			D3DXVECTOR3 vUp;
			vUp.x = m_mMatrix._21;
			vUp.y = m_mMatrix._22;
			vUp.z = m_mMatrix._23;
			sMsg.UpVector = vUp*1000.0f;
			int nType = T_FC_MOVE;
			memcpy(buffer, &nType, SIZE_FIELD_TYPE_HEADER);
			memcpy(buffer+SIZE_FIELD_TYPE_HEADER, &sMsg, sizeof(sMsg));
			g_pD3dApp->m_pFieldWinSocket->Write(buffer, SIZE_FIELD_TYPE_HEADER + sizeof(sMsg));
			g_pD3dApp->m_vClientOldPos = m_vPos;
//			g_pD3dApp->m_dwLastTickTime = g_pD3dApp->m_dwCurTickTime;
			g_pD3dApp->m_dwSpeedHackTimer = 0;
//#ifdef _DEBUG
//			m_nDebugSendPacketMove++;
//#endif
		}
		break;
	case T_FC_EVENT_LEAVE_BUILDING:
		{
			MSG_FC_EVENT_LEAVE_BUILDING sMsg;
			memset(&sMsg,0x00,sizeof(sMsg));
//			sMsg.BuildingIndex = g_pGameMain->m_pShop->m_pNPCWindow->m_buildingInfo.BuildingIndex;
			int nType = T_FC_EVENT_LEAVE_BUILDING;
			memcpy(buffer, &nType, SIZE_FIELD_TYPE_HEADER);
			memcpy(buffer+SIZE_FIELD_TYPE_HEADER, &sMsg, sizeof(sMsg));
			g_pD3dApp->m_pFieldWinSocket->Write(buffer, SIZE_FIELD_TYPE_HEADER + sizeof(sMsg));
		}
		break;
	}
}

//void CShuttleChild::CheckMoveRate(float fElapsedTime,int nSideMove, int nFBMove)
void CShuttleChild::CheckMoveRate(float fElapsedTime,int nSideMove)
{
	FLOG( "CShuttleChild::CheckMoveRate(float fElapsedTime,int nSideMove, int nFBMove)" );
	if(nSideMove)
	{
		// ShipŔÇ ÁÂÇĄ şŻČ­...ŔĚµż[ÁÂ,żě]
		m_fLRVelRate += 10.0f*nSideMove*fElapsedTime;
		if(m_fLRVelRate < -1.0f)
			m_fLRVelRate = -1.0f;
		else if(m_fLRVelRate > 1.0f)
			m_fLRVelRate = 1.0f;
	}
	else
	{
		if(m_fLRVelRate > 0.0f)
		{
			m_fLRVelRate -= fElapsedTime;
			if(m_fLRVelRate < 0.0f)
				m_fLRVelRate = 0.0f;
		}
		else if(m_fLRVelRate < 0.0f)
		{
			m_fLRVelRate += fElapsedTime;
			if(m_fLRVelRate > 0.0f)
				m_fLRVelRate = 0.0f;
		}
	}
//	m_vNextPos = m_vPos + 0.5f*m_fLRVelRate*m_vSideVel*m_fShuttleSpeedMax; //  ÁÂżě ŔĚµżŔş żŁÁřŔÇ ĂÖ´ë Ľş´Éżˇ żµÇâŔ» ąŢŔÚ
	m_vNextPos = m_vPos + 0.5f*m_fLRVelRate*m_vSideVel*m_fRealShuttleSpeedMax; //  ÁÂżě ŔĚµżŔş żŁÁřŔÇ ĂÖ´ë Ľş´Éżˇ żµÇâŔ» ąŢŔÚ
	// shipŔÇ żňÁ÷ŔÓ[ľŐ,µÚ] Ĺ°°ˇ ľČ´­·ŻÁłŔ»¶§
	if(m_dwState == _NORMAL)
	{
		// 2005-06-03 by jschoi - şńÇŕ ˝Ă˝şĹŰ şŻ°ć
				
		if(IS_DT(m_myShuttleInfo.UnitKind) && !m_bIsAir )
		{// Move_Ground 
			// 2006-09-22 by ispark Ć©Ĺä¸®ľó¸¸ ż¬°üĽş ŔÖŔ˝
			if(g_pTutorial && g_pTutorial->IsTutorialMode() && g_pTutorial->GetLesson() == L0)
			{
				return;
			}

			if(m_bAttackMode != _SIEGE && m_bUnitStop == FALSE)
			{
				// 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
				//if((g_pD3dApp->m_pKeyBoard->GetAsyncKeyState(DIK_W)
//				|| g_pD3dApp->m_pKeyBoard->GetAsyncKeyState(DIK_UP))&& 
//					!g_pD3dApp->m_pKeyBoard->GetAsyncKeyState(DIK_S) && 
				if((g_pD3dApp->GetAsyncKeyState_DIK_DIJ(DIK_W)
					|| g_pD3dApp->GetAsyncKeyState_DIK_DIJ(DIK_UP))&& 
					!g_pD3dApp->GetAsyncKeyState_DIK_DIJ(DIK_S) && 
					m_bLandingField == FALSE 
					&&	g_pD3dApp->m_bChatMode == FALSE)			// 2008-11-04 by dgwoo Âř·ú˝Ă Ă¤ĆĂÁßŔĚ ľĆ´Ďż©¸¸ Á¶ÁľŔĚ °ˇ´É.
				{
					// 2005-09-28 by ispark żŁÁř ĽÓµµ 0Ŕ¸·Î ŔÎÇŃ m_fRealShuttleSpeedMin -> m_fShuttleGroundSpeedŔ¸·Î şŻ°ć
					m_fShuttleSpeed += (m_fShuttleGroundSpeed - m_fShuttleSpeed)*fElapsedTime;
				}
				// 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
//				else if((g_pD3dApp->m_pKeyBoard->GetAsyncKeyState(DIK_S) 
//                          || g_pD3dApp->m_pKeyBoard->GetAsyncKeyState(DIK_DOWN)) &&
//						 !g_pD3dApp->m_pKeyBoard->GetAsyncKeyState(DIK_W) && 
				else if((g_pD3dApp->GetAsyncKeyState_DIK_DIJ(DIK_S) 
                          || g_pD3dApp->GetAsyncKeyState_DIK_DIJ(DIK_DOWN)) &&
						 !g_pD3dApp->GetAsyncKeyState_DIK_DIJ(DIK_W) && 
						 m_bLandingField == FALSE
						 && g_pD3dApp->m_bChatMode == FALSE)			// 2008-11-04 by dgwoo Âř·ú˝Ă Ă¤ĆĂÁßŔĚ ľĆ´Ďż©¸¸ Á¶ÁľŔĚ °ˇ´É.
				{
					if(m_fShuttleSpeed > 0)
					{
						m_fShuttleSpeed -= (m_fShuttleSpeed + m_fShuttleGroundSpeed/2)*fElapsedTime;
					}
					else
					{
						m_fShuttleSpeed -= (m_fShuttleGroundSpeed/2 - m_fShuttleSpeed)*fElapsedTime;
					}

					if(m_fShuttleSpeed < -m_fShuttleGroundSpeed/2)
					{
						m_fShuttleSpeed = -m_fShuttleGroundSpeed/2;
					}
				}
				else
				{

					float fAddSpeed = m_fShuttleSpeed*fElapsedTime;

					if ( fabs(fAddSpeed) < 0.005f )
						m_fShuttleSpeed /= 2;
					else
						m_fShuttleSpeed -= fAddSpeed;

					// ±âÁŘ°Ş ŔÓŔÇ Ăł¸®. ĽŇĽýÁˇ 2ŔÚ¸®ŔĚÇĎ 0ŔĎ °ćżě Áˇ°¨Ăł¸®żˇĽ­ ąŮ·Î 0Ŕ¸·Î ĽÂĆĂ. 2010. 08. 24 by hsLee.
					if ( (int)(m_fShuttleSpeed * 100.0f) == 0 )
						m_fShuttleSpeed = .0f;
					
				}
			}
			else
			{
				m_fShuttleSpeed = 0.0f;
			}

			if(m_bPenaltyUnitStop == TRUE && m_bUnitStop == FALSE)
			{
				m_bPenaltyUnitStop = FALSE;
				m_bMouseLock = FALSE;
				m_bMouseMoveLock = FALSE;
				g_pD3dApp->m_bFixUnitDirMode = FALSE;		
			}
		}	
//		else if(IS_DT(m_myShuttleInfo.UnitKind) && (m_bAttackMode == _SIEGE))
//		{// 2006-11-16 by dgwoo żˇľî˝ĂÁî ¸đµĺŔĎ¶§.
//			//m_fShuttleSpeed -= m_fShuttleSpeed * fElapsedTime * 4.0f;
//			m_fShuttleSpeed = 0;
//			m_fTempGravity = 0.0f;
//			
//		}

		else
		{// Move_Air
			
			// ŔĚ şÎşĐŔş »ő·Î ¸¸µéľîÁř şÎşĐ
			// ÄÚµĺżˇĽ­ ľĆ·ˇÂĘżˇ ÁÖĽ®Ŕ¸·Î µČ şÎşĐŔĚ żąŔü ÄÚµĺ 
			// ŔŻ´ÖŔÇ żňÁ÷ŔÓżˇ ´ëÇŘ ¸Ĺżě ąÎ°¨ÇŃ şÎşĐŔĚąÇ·Î ąÝµĺ˝Ă Č®ŔÎ ÇŘľß ÇÔ.

			// 2005-05-31 by jschoi - ŔŻ´ÖŔĚ »óÇĎ ŔĚµż ˝Ă ĽÓµµ Â÷ŔĚ¸¦ µÎ°í ÇĎ°­ ˝Ă şü¸Ą ĽÓµµ·Î ŔĚµż
			float GRAVITY = 98.0f;		// Â¦Ĺü Áß·Â °ˇĽÓµµ 
			float fTempAngle = ACOS(D3DXVec3Dot(&m_vVel,&D3DXVECTOR3(0,1,0)))/PI;
			float fTempGravity;
			if(fTempAngle > 0 && fTempAngle < 0.5f)
			{// »ó˝Â Áß 
				fTempGravity = GRAVITY*(0.5f - fTempAngle)*2.0f;
			}
			else if(fTempAngle > 0.5f && fTempAngle < 1.0f)
			{// ÇĎ°­ Áß 
				fTempGravity = -GRAVITY*(fTempAngle - 0.5f)*2.0f;
			}
			else
			{
				fTempGravity = 0;
			}

			if(m_fTempGravity > fTempGravity)
			{
				m_fTempGravity -= (m_fTempGravity - fTempGravity)*fElapsedTime;
			}
			else if(m_fTempGravity < fTempGravity)
			{
				m_fTempGravity += (fTempGravity - m_fTempGravity)*fElapsedTime;
			}
				 

			if(m_fTempGravity < -GRAVITY)
			{
				m_fTempGravity = -GRAVITY;
			}
			else if(m_fTempGravity > GRAVITY)
			{
				m_fTempGravity = GRAVITY;
			}

			// 2005-08-10 by ispark
			if((m_fRealShuttleSpeedMin > (m_fShuttleSpeed - m_fTempGravity)) && m_bPenaltyUnitStop == FALSE)
			{
				m_fTempGravity = m_fShuttleSpeed - m_fRealShuttleSpeedMin;
			}

			// 2005-06-03 by jschoi - SĹ° ±â´É ŔçÁ¤ŔÇ
			const float AIR_BREAK = 4.0f;	// Default 1.0f - ŔĚ °ŞŔĚ Ĺ©¸é Ĺ¬Ľö·Ď şę·ąŔĚĹ©°ˇ şü¸Ł°Ô ŔűżëµČ´Ů.
			if(m_bAttackMode == _SIEGE)
			{
				m_fShuttleSpeed = 0;
				m_fTempGravity = 0.0f;
				if(m_fShuttleSpeed < 0.0f)
				{
					m_fShuttleSpeed = 0.0f;
				}
			}
			// 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
//			else if(((g_pD3dApp->m_pKeyBoard->GetAsyncKeyState(DIK_S) &&
//				!g_pD3dApp->m_pKeyBoard->GetAsyncKeyState(DIK_SPACE))|| // ˝şĆäŔĚ˝şąŮ ľČ ´­·¶Ŕ» ¶§
			else if(((g_pD3dApp->GetAsyncKeyState_DIK_DIJ(DIK_S) &&
				!g_pD3dApp->GetAsyncKeyState_DIK_DIJ(DIK_SPACE))|| // ˝şĆäŔĚ˝şąŮ ľČ ´­·¶Ŕ» ¶§
				m_bOrderMove || 
//				g_pD3dApp->m_bChatMode ||				// 2005-11-16 by ispark Ă¤ĆĂ˝Ă żˇľîşę·ąŔĚĹ© »čÁ¦
				g_pD3dApp->m_bFixUnitDirMode) &&
				m_bUnitStop == FALSE &&
				m_bPenaltyUnitStop == FALSE)								// 2005-08-08 by ispark ŔŻ´ÖÁ¤Áö˝Ă »çżë ľČÇÔ
			{

				float fAddSpeed = (m_fShuttleSpeed - m_fRealShuttleSpeedMin) * fElapsedTime * AIR_BREAK;
				// 2015-05-13 by jwlee 리버스엔진 사용시 속도 끊김 현상 수정
// 				if ( fabs(fAddSpeed) < 0.005f )
// 					m_fShuttleSpeed /= 2;
// 				else
//					m_fShuttleSpeed -= fAddSpeed;
				if (fabs(fAddSpeed) >= 0.005f)
					m_fShuttleSpeed -= fAddSpeed;
				// end 2015-05-13 by jwlee 리버스엔진 사용시 속도 끊김 현상 수정

				if(m_fShuttleSpeed < m_fRealShuttleSpeedMin)
				{
					m_fShuttleSpeed = m_fRealShuttleSpeedMin;
				}
			}
			else if(m_bKeyDownA || m_bKeyDownD)
			{
				// 2005-07-07 by ispark
				// ·ŃŔ× ˝Ă˝şĹŰ »çżë˝Ă ĽÓµµ şŻČ­
				// Ľ­Ľ­Č÷ żř·ˇ ĽÓµµŔÇ 2/3Ŕ¸·Î ÁŮľîµç´Ů. (ÁÂ,żě °Ĺ¸®żˇ µű¶ó ÁŮľîµé°Ô ÇĎż´Ŕ˝)
				float fLegth = D3DXVec3Length(&(m_vRollOldPos - m_vPos));	// Ŕü ÁÂÇĄżÍ şń±łÇĎż© ľî´ŔÁ¤µµ Áöłµ´ÂÁö °č»ę

				if(fLegth)
				{
					if(80 > D3DXVec3Length(&(m_vRollStart - m_vPos)))		// ĂłŔ˝ ˝ĂŔŰČÄ ÁÂ, żě ŔĚµż °Ĺ¸®żÍ °č»ę
					{
						// ĽÓµµ¸¦ °Ĺ¸®żˇ µű¶ó Ľ­Ľ­Č÷ ÁŮľî ÁÜ
						m_fShuttleSpeed -= (80.0f / m_fRollDownSpeed) * (80.0f / fLegth);
						
						m_vRollOldPos = m_vPos;
					}
					else
					{
						// °Ĺ¸® ĂĘ°ú˝Ă żř·ˇ ĽÓµµŔÇ 2/3Ŕ¸·Î ÁŮľîµé°Ô ÇÔ
						m_fShuttleSpeed = m_fRollDownSpeed;
					}
				}

				// °č´ÜµČ ĽÓµµ°ˇ ĂÖŔúş¸´Ů ŔŰ´Ů¸é ŔŰŔş ĽÓµµ·Î ĽöÁ¤
				if(m_fShuttleSpeed < m_fRealShuttleSpeedMin)
				{
					m_fShuttleSpeed = m_fRealShuttleSpeedMin;
				}
			}
			// 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
			//else if(g_pD3dApp->m_pKeyBoard->GetAsyncKeyState(DIK_SPACE)
			else if(g_pD3dApp->GetAsyncKeyState_DIK_DIJ(DIK_SPACE)
				&& m_fShuttleSpeed < m_fRealShuttleSpeedBoosterOn
				&& m_nBooster == BOOSTER_TYPE_MAX
				&& m_bPenaltyUnitStop == FALSE)					// 2005-07-11 by ispark // şÎ˝şĹÍ »çżëÁßŔĚ¶ó¸é °č»ęÇĎ¶ó
			{
				m_fShuttleSpeed += (m_fRealShuttleSpeedBoosterOn - m_fShuttleSpeed)*fElapsedTime;
			}
			else if(m_bUnitStop == TRUE)							// 2005-08-08 by ispark ŔŻ´ÖÁ¤Áö Ăß°ˇ
			{
				m_fShuttleSpeed = 0;
				m_fTempGravity = 0.0f;
				if(m_fShuttleSpeed < 0.0f)
				{
					m_fShuttleSpeed = 0.0f;
				}
			}
			else if(m_bPenaltyUnitStop == TRUE)						// 2005-08-12 by ispark Á¤ÁöČÄ Ăßąß˝Ă ĆäłÎĆĽ Ŕűżë
			{
				float fPenaltySpeedMax;
				if((g_pStoreData->GetTotalWeight() / CAtumSJ::GetTransport(&g_pShuttleChild->m_myShuttleInfo)) > 0.9f)
				{
					fPenaltySpeedMax = m_fRealShuttleSpeedMin;
				}
				else
				{
					fPenaltySpeedMax = m_fRealShuttleSpeedMax;
				}

				m_fShuttleSpeed += fElapsedTime * (fPenaltySpeedMax / UNIT_STOP_DELAY);
				if(m_fShuttleSpeed > fPenaltySpeedMax)
				{
					// 2005-11-17 by ispark
					InitUnitStop();	
					m_fShuttleSpeed = fPenaltySpeedMax;
					m_fCurrentSideRate = 0.0f;

					// 2005-10-12 by ispark	Ć©Ĺä¸®ľó
					if(g_pTutorial->IsTutorialMode() == TRUE && g_pTutorial->GetLesson() >= L2)
					{
						g_pTutorial->m_Lesson2State.nBKey = 2;
					}
				}
			
//				SetCursorInit();
			}
			else
			{
				if(m_fShuttleSpeed < m_fRealShuttleSpeedMax)
				{
					m_fShuttleSpeed += (m_fRealShuttleSpeedMax - m_fShuttleSpeed)*fElapsedTime;
				}
				else if(m_fShuttleSpeed > m_fRealShuttleSpeedMax)
				{
					m_fShuttleSpeed -= (m_fShuttleSpeed - m_fRealShuttleSpeedMax)*fElapsedTime;
				}
			}
		}
/*
		// 2004-11-30 by jschoi
		if(m_nShuttleStopState == 4)	// żöÇÁ ľĆżô ĆĐĹĎ Áľ·á˝Ă żˇľî şę·ąŔĚĹ© Ľ­Ľ­Č÷ ĽÓ·ÂŔ» ÁŮŔÎ´Ů.
		{
			m_fShuttleSpeed -= (m_fRealShuttleSpeedMin - m_fShuttleSpeed)*fElapsedTime;
			if(m_fShuttleSpeed < m_fRealShuttleSpeedMin)
			{
				m_fShuttleSpeed = m_fRealShuttleSpeedMin;
			}

//			if(m_fShuttleSpeed > 60.0f)
//			{
//				m_fShuttleSpeed -= m_fShuttleSpeed*fElapsedTime/2;
//			}
//			else
//			{
//				m_fShuttleSpeed -= 30.0f*fElapsedTime;
//			}
//
//			if(m_fShuttleSpeed < 0)
//			{
//				m_fShuttleSpeed = 0;
//
//				// 2005-05-11 by jschoi
////				m_nShuttleStopState = 3;
//				if(	IS_DT(m_myShuttleInfo.UnitKind) || IS_OT(m_myShuttleInfo.UnitKind) )
//				{// M,A ±âľî
//					m_nShuttleStopState = 1;
//				}
//				else
//				{// B,I ±âľî
//					m_nShuttleStopState = 3;
//					m_nBooster = BOOSTER_TYPE_AIRBREAK;
//					g_pD3dApp->m_pSound->StopD3DSound( SOUND_HIGH_BOOSTER );
//				}			
//				
//			}
		}
		else if(nFBMove == 0)
		{
			if(g_pD3dApp->m_dwGameState == _SHOP)
			{// »óÁˇ ŔĚşĄĆ® ÁßŔĎ¶§ ĽÓµµ´Â ÁŮŔÎ´Ů.
				if(m_fShuttleSpeed > 1.0f)
				{
					m_fShuttleSpeed -= 50.0f*fElapsedTime;
					if(m_fShuttleSpeed < m_fRealShuttleSpeedMin)
					{
						m_fShuttleSpeed = m_fRealShuttleSpeedMin;
					}
//					if(m_fShuttleSpeed < 0.0f)
//					{
//						m_fShuttleSpeed = 0.0f;
//					}
				}
				else if(m_fShuttleSpeed < -1.0f)
				{
					m_fShuttleSpeed += 50.0f*fElapsedTime;
					if(m_fShuttleSpeed > 0.0f)
					{
						m_fShuttleSpeed = 0.0f;
					}
				}
				else
				{
					m_fShuttleSpeed = 0.0f;
				}
			}
			else
			{
				if(IS_BT(m_myShuttleInfo.UnitKind) || IS_ST(m_myShuttleInfo.UnitKind))
				{
					if(m_nBooster == BOOSTER_TYPE_AIRBREAK)
					{
//						m_fShuttleSpeed = 0.0f;
						m_fShuttleSpeed -= m_fShuttleSpeed*fElapsedTime;
						if(m_fShuttleSpeed < m_fRealShuttleSpeedMin)
						{
							m_fShuttleSpeed = m_fRealShuttleSpeedMin;
						}
//						if(m_fShuttleSpeed < 0)
//							m_fShuttleSpeed = 0;
					}
					else
					{
						// ĽÓµµ°ˇ żŔąö µÇľú´ÂÁö ĂĽĹ©
						if(m_fShuttleSpeed < m_fRealShuttleSpeedMin)
							m_fShuttleSpeed = m_fRealShuttleSpeedMin;
//						else if(m_fShuttleSpeed > m_fShuttleSpeedBoosterOn)
//							m_fShuttleSpeed = m_fShuttleSpeedBoosterOn;
						else if(m_fShuttleSpeed > m_fRealShuttleSpeedBoosterOn)
							m_fShuttleSpeed = m_fRealShuttleSpeedBoosterOn;
//						else if(m_fShuttleSpeed > m_fShuttleSpeedMax)
						else if(m_fShuttleSpeed > m_fRealShuttleSpeedMax)
							m_fShuttleSpeed -= 30.0f*fElapsedTime;
					}
				}
				else
				{
					if(IS_DT(m_myShuttleInfo.UnitKind) && !m_bIsAir)
					{
						if(m_fShuttleSpeed > 1.0f)
						{
							m_fShuttleSpeed -= 20.0f*fElapsedTime;
							if(m_fShuttleSpeed < 0.0f)
							{
								g_pD3dApp->m_pSound->StopD3DSound( SOUND_GROUND_MOVING_A_GEAR );
								m_fShuttleSpeed = 0.0f;
							}
						}
						else if(m_fShuttleSpeed < -1.0f)
						{
							m_fShuttleSpeed += 20.0f*fElapsedTime;
							if(m_fShuttleSpeed > 0.0f)
							{
								g_pD3dApp->m_pSound->StopD3DSound( SOUND_GROUND_MOVING_A_GEAR );
								m_fShuttleSpeed = 0.0f;
							}
						}
						else
						{
							g_pD3dApp->m_pSound->StopD3DSound( SOUND_GROUND_MOVING_A_GEAR );
							m_fShuttleSpeed = 0.0f;
						}
					}
				
					// ľŐ Ĺ°°ˇ ľČ´­·¶Ŕ»¶§ ĽÓµµ Áő°¨ŔĚ ľř´Ů ÇöŔç ĽÓµµ ŔŻÁö
					if(m_fShuttleSpeed < -1.0f)
					{
						m_fShuttleSpeed += 20.0f*fElapsedTime;
						if(m_fShuttleSpeed > 0.0f)
						{
							g_pD3dApp->m_pSound->StopD3DSound( SOUND_GROUND_MOVING_A_GEAR );
							m_fShuttleSpeed = 0.0f;
						}
					}
					if((g_pStoreData->GetTotalWeight() / CAtumSJ::GetTransport(&g_pShuttleChild->m_myShuttleInfo)) > 0.9f)
					{
						if(m_fShuttleSpeed < 0)
						{
							g_pD3dApp->m_pSound->StopD3DSound( SOUND_GROUND_MOVING_A_GEAR );
							m_fShuttleSpeed = 0;
						}
						else if(m_fShuttleSpeed > (m_fRealShuttleSpeedMin<0 ? 10 :m_fRealShuttleSpeedMin))
						{
							m_fShuttleSpeed = (m_fRealShuttleSpeedMin<0 ? 10 :m_fRealShuttleSpeedMin);
						}
					}
					// ĽÓµµ°ˇ żŔąö µÇľú´ÂÁö ĂĽĹ©
					if(m_fShuttleSpeed < m_fRealShuttleSpeedMin)
						m_fShuttleSpeed = m_fRealShuttleSpeedMin;
//					else if(m_fShuttleSpeed > m_fShuttleSpeedBoosterOn)
//						m_fShuttleSpeed = m_fShuttleSpeedBoosterOn;
					else if(m_fShuttleSpeed > m_fRealShuttleSpeedBoosterOn)
						m_fShuttleSpeed = m_fRealShuttleSpeedBoosterOn;
//					else if(m_fShuttleSpeed > m_fShuttleSpeedMax)
					else if(m_fShuttleSpeed > m_fRealShuttleSpeedMax)
						m_fShuttleSpeed -= 30.0f*fElapsedTime;
				}
			}
		}
		else
		{	// shipŔÇ żňÁ÷ŔÓ[ľŐ,µÚ] Ĺ°°ˇ ´­·ŻÁłŔ»¶§
			// ľĆ·ˇ 3ÁŮ ... şÎ˝şĹÍ ŔĎ¶§´Â şÎ˝şĹÍ ŔÇ ĽÓµµ ¸¸Ĺ­ °ˇĽÓµµ°ˇ ŔűżëµČ´Ů-ŔĎąÝşńÇŕ˝Ă´Â ŔĎąÝĽÓµµ¸¸Ĺ­.
			if(m_nBooster == BOOSTER_TYPE_AIRBREAK)
			{
//				m_fShuttleSpeed = 0.0f;
				m_fShuttleSpeed -= m_fShuttleSpeed*fElapsedTime;
				if(m_fShuttleSpeed < 0)
				{
					g_pD3dApp->m_pSound->StopD3DSound( SOUND_GROUND_MOVING_A_GEAR );
					m_fShuttleSpeed = 0;
				}
			}
			else
			{
				if(nFBMove == -1)
				{
					if(m_fShuttleSpeed > m_fRealShuttleSpeedMin)
						m_fShuttleSpeed -= 70.0f*fElapsedTime;
					else if(m_fShuttleSpeed < m_fRealShuttleSpeedMin)
						m_fShuttleSpeed = m_fRealShuttleSpeedMin;
				}
				else
				{
					if(m_nBooster == BOOSTER_TYPE_MAX)
					{
//						if(m_fShuttleSpeed < m_fShuttleSpeedBoosterOn)
						if(m_fShuttleSpeed < m_fRealShuttleSpeedBoosterOn)
						{
//							float fGap = m_fShuttleSpeedBoosterOn - m_fShuttleSpeed;
							float fGap = m_fRealShuttleSpeedBoosterOn - m_fShuttleSpeed;
							m_fShuttleSpeed += fGap*fElapsedTime;
//							if(m_fShuttleSpeed > m_fShuttleSpeedBoosterOn)
//								m_fShuttleSpeed = m_fShuttleSpeedBoosterOn;
							if(m_fShuttleSpeed > m_fRealShuttleSpeedBoosterOn)
								m_fShuttleSpeed = m_fRealShuttleSpeedBoosterOn;
						}
					}
					else
					{
//						if(m_fShuttleSpeed < m_fShuttleSpeedMax)
						if(m_fShuttleSpeed < m_fRealShuttleSpeedMax)
						{
//							float fGap = m_fShuttleSpeedMax - m_fShuttleSpeed;
							float fGap = m_fRealShuttleSpeedMax - m_fShuttleSpeed;
							m_fShuttleSpeed += fGap*fElapsedTime;
//							if(m_fShuttleSpeed > m_fShuttleSpeedMax)
//								m_fShuttleSpeed = m_fShuttleSpeedMax;
							if(m_fShuttleSpeed > m_fRealShuttleSpeedMax)
								m_fShuttleSpeed = m_fRealShuttleSpeedMax;
						}
//						else if(m_fShuttleSpeed > m_fShuttleSpeedMax)
						else if(m_fShuttleSpeed > m_fRealShuttleSpeedMax)
						{
//							float fGap = m_fShuttleSpeed - m_fShuttleSpeedMax;
							float fGap = m_fShuttleSpeed - m_fRealShuttleSpeedMax;
							m_fShuttleSpeed -= fGap*fElapsedTime;
//							if(m_fShuttleSpeed < m_fShuttleSpeedMax)
//								m_fShuttleSpeed = m_fShuttleSpeedMax;
							if(m_fShuttleSpeed < m_fRealShuttleSpeedMax)
								m_fShuttleSpeed = m_fRealShuttleSpeedMax;
						}
					}
				}
			}
		}

*/

	}
	else if(m_dwState == _LANDING)
	{
		/////////////////////////////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////////////////
		D3DXVECTOR3 myShutllePos, myShutlleNextPos;

		myShutllePos = m_vPos + m_vVel * TILE_SIZE;
		
		D3DXVec3Normalize(&m_vVel, &(myShutllePos - m_vPos));
				
		myShutlleNextPos = m_vPos;								
		myShutlleNextPos = myShutlleNextPos + m_vVel * TILE_SIZE;
		myShutlleNextPos.x = (int(myShutlleNextPos.x/TILE_SIZE))*TILE_SIZE + TILE_SIZE/2;
		myShutlleNextPos.z = (int(myShutlleNextPos.z/TILE_SIZE))*TILE_SIZE + TILE_SIZE/2;
		
		int nXCheck = (int)(myShutlleNextPos.x / TILE_SIZE);
		int nYCheck = (int)(myShutlleNextPos.z / TILE_SIZE);		

		if(nXCheck < 0)
			nXCheck = 0;

		if (nYCheck < 0)
			nYCheck = 0;

		m_bMoveGroundType = g_pGround->m_pTileInfo[nXCheck*g_pGround->m_projectInfo.sXSize + nYCheck].bEnableLand;
		/////////////////////////////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////////////////

		if(m_fShuttleSpeed > 1.0f)
		{
			m_fShuttleSpeed -= 30.0f*fElapsedTime;			// 2005-10-26 by ispark ĽÓµµ°ˇ şü¸Ą °ü°č·Î »ˇ¸® ÁŮŔÓ
			if(m_fShuttleSpeed < 0.0f)
			{
				m_fShuttleSpeed = 0.0f;
			}
		}
		else if(m_fShuttleSpeed < -1.0f)
		{
			m_fShuttleSpeed += 5.0f*fElapsedTime;
			if(m_fShuttleSpeed > 0.0f)
			{
				m_fShuttleSpeed = 0.0f;
			}
		}
		else
		{
			m_fShuttleSpeed = 0.0f;
		}

		if(m_bMoveGroundType != 1) // Ăß°ˇ ÄÚµĺ ydkim
		{
			m_fShuttleSpeed = 0.0f;
		}

		// 2005-06-03 by jschoi
		m_fTempGravity = 0.0f;
	}
	else
	{
		/////////////////////////////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////////////////
		D3DXVECTOR3 myShutllePos, myShutlleNextPos;

		myShutllePos = m_vPos + m_vVel * TILE_SIZE;
		
		D3DXVec3Normalize(&m_vVel, &(myShutllePos - m_vPos));
				
		myShutlleNextPos = m_vPos;								
		myShutlleNextPos = myShutlleNextPos + m_vVel * TILE_SIZE;
		myShutlleNextPos.x = (int(myShutlleNextPos.x/TILE_SIZE))*TILE_SIZE + TILE_SIZE/2;
		myShutlleNextPos.z = (int(myShutlleNextPos.z/TILE_SIZE))*TILE_SIZE + TILE_SIZE/2;
		
		int nXCheck = (int)(myShutlleNextPos.x / TILE_SIZE);
		int nYCheck = (int)(myShutlleNextPos.z / TILE_SIZE);	

		if (nXCheck < 0)
			nXCheck = 0;

		if (nYCheck < 0)
			nYCheck = 0;

		m_bMoveGroundType = g_pGround->m_pTileInfo[nXCheck*g_pGround->m_projectInfo.sXSize + nYCheck].bEnableLand;
		/////////////////////////////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////////////////
		
		if(m_fShuttleSpeed > 1.0f)
		{
			m_fShuttleSpeed -= 80.0f*fElapsedTime;			// 2005-10-26 by ispark ĽÓµµ°ˇ şü¸Ą °ü°č·Î »ˇ¸® ÁŮŔÓ
			if(m_fShuttleSpeed < 0.0f)
			{
				m_bLandedMove = TRUE;
				m_fShuttleSpeed = 0.0f;
			}
		}
		else if(m_fShuttleSpeed < -1.0f)
		{
			m_fShuttleSpeed += 30.0f*fElapsedTime;
			if(m_fShuttleSpeed > 0.0f)
			{
				m_bLandedMove = TRUE;
				m_fShuttleSpeed = 0.0f;
			}
		}
		else
		{
			m_bLandedMove = TRUE;
			m_fShuttleSpeed = 0.0f;
		}

		if(m_bMoveGroundType != 1) // Ăß°ˇ ÄÚµĺ ydkim TEST
		{
			m_fShuttleSpeed = 0.0f;
		}

		// 2005-06-03 by jschoi
		m_fTempGravity = 0.0f;
	}

	if((g_pStoreData->GetTotalWeight() / CAtumSJ::GetTransport(&g_pShuttleChild->m_myShuttleInfo)) > 0.9f)
	{
		if(m_fShuttleSpeed < 0)
		{
			m_fShuttleSpeed = 0;
		}
		else if(m_fShuttleSpeed > (m_fRealShuttleSpeedMin<0 ? 10 :m_fRealShuttleSpeedMin))
		{
			m_fShuttleSpeed = (m_fRealShuttleSpeedMin<0 ? 10 :m_fRealShuttleSpeedMin);
		}
	}

	if(m_dwState == _NORMAL)
	{
		// 2009. 09. 21 by ckPark ŔÎÇÇ´ĎĆĽ ÇĘµĺ ¸ó˝şĹÍ ˝şĹł ±¸Çö

		//m_vNextPos += m_vVel*(m_fShuttleSpeed - m_fTempGravity);
		// Č¦µĺ ˝şĹł °É¸®¸é Áß·Â °č»ęÇĎÁö ľĘ´Â´Ů
		if( !m_pSkill->IsExistDesParamSKill( DES_SKILL_MON_HOLD ) )
			m_vNextPos += m_vVel*(m_fShuttleSpeed - m_fTempGravity);
		else
			m_vNextPos += m_vVel*m_fShuttleSpeed;

		// end 2009. 09. 21 by ckPark ŔÎÇÇ´ĎĆĽ ÇĘµĺ ¸ó˝şĹÍ ˝şĹł ±¸Çö
	}
	else
	{
		m_vNextPos += m_vVel*m_fShuttleSpeed;
	}

	m_vMoveVel = m_vNextPos - m_vPos;
	D3DXVec3Normalize(&m_vMoveVel, &m_vMoveVel);			// 2006-07-13 by ispark, ±âľî ŔĚµż ąćÇâ

// 2005-08-08 by ispark
// ±âÁ¸ F11Ĺ°¸¦ ľřľŘ´Ů.
//#ifdef _DEBUG
//	if(m_bUnitStop)
//	{
//		m_fShuttleSpeed = 0.0f;
//		m_fTempGravity = 0.0f;
//		m_fRealShuttleSpeedMin = 0.0f;
//	}
//#endif
}

#define SHUTTLE_MAP_COLL_RATE			5.0f
#define SHUTTLE_AUTO_FLIGHTINIT_HEIGHT	30.0f
#define SHUTTLE_MAX_MOUSE_MOVE			100

void CShuttleChild::CheckCollForMap()
{
	FLOG( "CShuttleChild::CheckCollForMap()" );

	//28-07-2024 by Inet - allow wall & object passing by staff from tool
	if (g_pD3dApp->m_bNoClipping)
		return;

	// 2005-02-02 by jschoi
	// żěÁÖ¸ĘŔş ÁöÇü°ú Ăćµą°Ë»ç¸¦ ÇĎÁö ľĘ´Â´Ů.
	// 2005-12-12 by ispark, ÁöÇüŔ» ±×¸®Áö ľĘ´Â´Ů¸é Ăćµą°Ë»ç¸¦ ľČÇŃ´Ů. (µż±Ľ, żěÁÖ¸Ę)
	// 2007-06-18 by bhsohn ľËĆ®ĹÜ °ü·Ă ąö±× Ăł¸®
	//if(!IsTileMapRenderEnable(m_myShuttleInfo.MapChannelIndex.MapIndex))
	if(!IsTileMapTickEnable(m_myShuttleInfo.MapChannelIndex.MapIndex))
	{
		// 2007-01-04 by bhsohn ą«şę ˝şĹł·Î ¸Ę Ĺ¸ŔĎżˇ şÎµúÇűŔ» ‹šżˇ ´ëÇŃ ąö±×Ăł¸®
		m_bCollMap = FALSE;

		return;
	}

	float fElapsedTime = g_pD3dApp->GetElapsedTime();
	// ¸Ę°úŔÇ Ăćµą °Ë»ç-ĽĹĆ˛ŔÇ Áßżä 4şÎşĐŔÇ Ăćµą °Ë»ç¸¦ ÇĎŔÚ ľŐ µÚ ÁÂ żě(4 ±ÍĹüŔĚ ÁÂÇĄ¸¦ ¸ÂĂß´Â ŔŰľ÷µµ ÇŘÁŕľßÇŇ °ÍŔĚ´Ů)
	D3DXMATRIX	m;

	D3DXVECTOR3 vSide, vVel;
//	float fAngle;

	vVel = m_vVel;
	vVel.y = 0.0f;
	D3DXVec3Normalize(&vVel,&vVel);

	vSide.x = m_mMatrix._11;
	vSide.y = m_mMatrix._12;
	vSide.z = m_mMatrix._13;
	vSide *= -1.0f;
	
	D3DXVECTOR3 vFront, vBack, vLeft, vRight, vParticlePos;
	vFront = m_vPos + 8.0f*m_vVel;
	vBack = m_vPos - 3.0f*m_vVel;
	vLeft = m_vPos - 7.0f*vSide;
	vRight = m_vPos + 7.0f*vSide;
	
	m_bCheckMove = 4;
	float fMapHeightTemp;
	BOOL bIsColl = FALSE;
	m_bCollMap = FALSE;
	fMapHeightTemp = CheckMap(vFront);
	if(m_bCollMap)
	{// ľŐşÎşĐ Ăćµą - ÁÂÇĄ ŔĚµżČÄ Ŕ§ÂĘŔ¸·Î ąćÇâŔĚ Ć˛¸˛
		vParticlePos = vFront;
		if(g_pD3dApp->m_pEffectList && !g_pScene->FindEffect(RC_EFF_COLL_01))
		{
			CAppEffectData * pData = new CAppEffectData(NULL,RC_EFF_COLL_01,vParticlePos);
			pData = (CAppEffectData *)g_pD3dApp->m_pEffectList->AddChild(pData);
		}

//		m_vPos += SHUTTLE_MAP_COLL_RATE*m_vNormal*fElapsedTime;
		m_vPos += (m_fShuttleSpeed / 2.0f)*m_vNormal*fElapsedTime;
		D3DXVec3Cross(&vSide,&m_vUp,&m_vVel);
//		m_fShuttleSpeed = m_fRealShuttleSpeedMin>30 ? m_fRealShuttleSpeedMin : 30;
		if(m_vUp.y >= 0.0f)// ąŮ·Î şńÇŕÇŇ¶§
		{
			// 2005-12-01 by ispark, ŔúşńÇŕżˇĽ­´Â Ăćµą˝Ă ąćÇâŔüČŻŔĚ ľŕÇŘĽ­ °­ÇŘ°Ô ÇÔ
//			if(m_fShuttleSpeed < 80.0f)
//			{
//				D3DXMatrixRotationAxis(&m,&vSide,-5.0f*fElapsedTime);
//			}
//			else
//			{
//				D3DXMatrixRotationAxis(&m,&vSide,-0.05f*m_fShuttleSpeed*fElapsedTime);
//			}
			// 2006-05-09 by ispark, Ăćµą ąÝ»ç ÄÚµĺ şŻ°ć
			GetCollReflect(&m_vVel, &m_vVel, &m_vNormal);
		}
		else// °Ĺ˛Ů·Î şńÇŕÇŇ¶§
		{
//			if(m_fShuttleSpeed < 80.0f)
//			{
//				D3DXMatrixRotationAxis(&m,&vSide,-5.0f*fElapsedTime);
//			}
//			else
//			{
//				D3DXMatrixRotationAxis(&m,&vSide,0.05f*m_fShuttleSpeed*fElapsedTime);
//			}
			// 2006-05-09 by ispark, Ăćµą ąÝ»ç ÄÚµĺ şŻ°ć
			GetCollReflect(&m_vVel, &m_vVel, &m_vNormal, FALSE);
		}
//		D3DXVec3TransformCoord(&m_vVel,&m_vVel,&m);
//		D3DXVec3TransformCoord(&m_vUp,&m_vUp,&m);
		// ¸¶żě˝ş ÁÂÇĄ Ŕ§·Î ŔĚµż - 50¸¸Ĺ­
//		POINT pt,pt2;
//		GetCursorPos(&pt);
//		pt2 = pt;
//		ScreenToClient(g_pD3dApp->GetHwnd(), &pt);
//		CheckMouseReverse(&pt);
//
//		int nY = pt2.y - pt.y;
//		nY += g_pD3dApp->GetBackBufferDesc().Height/2 - SHUTTLE_MAX_MOUSE_MOVE*g_pD3dApp->GetBackBufferDesc().Height/600;
//		SetCursorPos(pt2.x,nY);
		bIsColl = TRUE;
	}
	m_bCollMap = FALSE;
	fMapHeightTemp = CheckMap(vBack);
	if(m_bCollMap)
	{// µÚşÎşĐ Ăćµą
		vParticlePos = vBack;
		if(g_pD3dApp->m_pEffectList && !g_pScene->FindEffect(RC_EFF_COLL_01))
		{
			CAppEffectData * pData = new CAppEffectData(NULL,RC_EFF_COLL_01,vParticlePos);
			pData = (CAppEffectData *)g_pD3dApp->m_pEffectList->AddChild(pData);
		}
//		m_fShuttleSpeed = m_fRealShuttleSpeedMin>30 ? m_fRealShuttleSpeedMin : 30;
		m_vPos += SHUTTLE_MAP_COLL_RATE*m_vNormal*fElapsedTime;
		// 2006-05-09 by ispark, Ăćµą ąÝ»ç ÄÚµĺ şŻ°ć
		GetCollReflect(&m_vVel, &m_vVel, &m_vNormal);
		bIsColl = TRUE;
	}
	m_bCollMap = FALSE;
	fMapHeightTemp = CheckMap(vLeft);
	if(m_bCollMap)
	{// ÁÂĂř şÎşĐ Ăćµą
		vParticlePos = vLeft;
		if(g_pD3dApp->m_pEffectList && !g_pScene->FindEffect(RC_EFF_COLL_01))
		{
			CAppEffectData * pData = new CAppEffectData(NULL,RC_EFF_COLL_01,vParticlePos);
			pData = (CAppEffectData *)g_pD3dApp->m_pEffectList->AddChild(pData);
		}
		m_vPos += SHUTTLE_MAP_COLL_RATE*m_vNormal*fElapsedTime;
//		m_fShuttleSpeed = m_fRealShuttleSpeedMin>30 ? m_fRealShuttleSpeedMin : 30;
//		D3DXMatrixRotationAxis(&m,&m_vUp,0.05f*m_fShuttleSpeed*fElapsedTime);
//		D3DXVec3TransformCoord(&m_vVel,&m_vVel,&m);
		// 2006-05-09 by ispark, Ăćµą ąÝ»ç ÄÚµĺ şŻ°ć
		GetCollReflect(&m_vVel, &m_vVel, &m_vNormal);
		//¸¶żě˝ş ÁÂÇĄ żě·Î ŔĚµż - 50¸¸Ĺ­
//		POINT pt,pt2;
//		GetCursorPos(&pt);
//		pt2 = pt;
//		ScreenToClient(g_pD3dApp->GetHwnd(), &pt);
//		CheckMouseReverse(&pt);
//
//		int nX = pt2.x - pt.x;
//		nX += g_pD3dApp->GetBackBufferDesc().Width/2 + SHUTTLE_MAX_MOUSE_MOVE*g_pD3dApp->GetBackBufferDesc().Width/800;
//		SetCursorPos(nX,pt2.y);
		bIsColl = TRUE;
	}
	m_bCollMap = FALSE;
	fMapHeightTemp = CheckMap(vRight);
	if(m_bCollMap)
	{// żěĂř şÎşĐ Ăćµą
		vParticlePos = vRight;
		if(g_pD3dApp->m_pEffectList && !g_pScene->FindEffect(RC_EFF_COLL_01))
		{
			CAppEffectData * pData = new CAppEffectData(NULL,RC_EFF_COLL_01,vParticlePos);
			pData = (CAppEffectData *)g_pD3dApp->m_pEffectList->AddChild(pData);
		}
		m_vPos += SHUTTLE_MAP_COLL_RATE*m_vNormal*fElapsedTime;
//		m_fShuttleSpeed = m_fRealShuttleSpeedMin>30 ? m_fRealShuttleSpeedMin : 30;
//		D3DXMatrixRotationAxis(&m,&m_vUp,-0.05f*m_fShuttleSpeed*fElapsedTime);
//		D3DXVec3TransformCoord(&m_vVel,&m_vVel,&m);
		// 2006-05-09 by ispark, Ăćµą ąÝ»ç ÄÚµĺ şŻ°ć
		GetCollReflect(&m_vVel, &m_vVel, &m_vNormal);
		//¸¶żě˝ş ÁÂÇĄ ÁÂ·Î ŔĚµż - 50¸¸Ĺ­
//		POINT pt,pt2;
//		GetCursorPos(&pt);
//		pt2 = pt;
//		ScreenToClient(g_pD3dApp->GetHwnd(), &pt);
//		CheckMouseReverse(&pt);
//
//		int nX = pt2.x - pt.x;
//		nX += g_pD3dApp->GetBackBufferDesc().Width/2 - SHUTTLE_MAX_MOUSE_MOVE*g_pD3dApp->GetBackBufferDesc().Width/800;
//		SetCursorPos(nX,pt2.y);
		bIsColl = TRUE;
	}
	m_bCollMap = FALSE;
	fMapHeightTemp = CheckMap(m_vPos - m_fAmorHeight*m_vUp);
	if(m_bCollMap)
	{// ľĆ·ˇşÎşĐ Ăćµą
		vParticlePos = m_vPos - m_fAmorHeight*m_vUp;
		if(g_pD3dApp->m_pEffectList && !g_pScene->FindEffect(RC_EFF_COLL_01))
		{
			CAppEffectData * pData = new CAppEffectData(NULL,RC_EFF_COLL_01,vParticlePos);
			pData = (CAppEffectData *)g_pD3dApp->m_pEffectList->AddChild(pData);
		}
		m_vPos += SHUTTLE_MAP_COLL_RATE*m_vNormal*fElapsedTime;
		D3DXVec3Cross(&vSide,&m_vUp,&m_vVel);
//		m_fShuttleSpeed = m_fRealShuttleSpeedMin>30 ? m_fRealShuttleSpeedMin : 30;
//		D3DXMatrixRotationAxis(&m,&vSide,-0.05f*m_fShuttleSpeed*fElapsedTime);
//		D3DXVec3TransformCoord(&m_vVel,&m_vVel,&m);
//		D3DXVec3TransformCoord(&m_vUp,&m_vUp,&m);
		// 2006-05-09 by ispark, Ăćµą ąÝ»ç ÄÚµĺ şŻ°ć
		GetCollReflect(&m_vVel, &m_vVel, &m_vNormal);
		//¸¶żě˝ş ÁÂÇĄ Ŕ§·Î ŔĚµż - 50¸¸Ĺ­
//		POINT pt,pt2;
//		GetCursorPos(&pt);
//		pt2 = pt;
//		ScreenToClient(g_pD3dApp->GetHwnd(), &pt);
//		CheckMouseReverse(&pt);
//
//		int nY = pt2.y - pt.y;
//		nY += g_pD3dApp->GetBackBufferDesc().Height/2 - SHUTTLE_MAX_MOUSE_MOVE*g_pD3dApp->GetBackBufferDesc().Height/600;
//		SetCursorPos(pt2.x,nY);
		bIsColl = TRUE;
	}
	m_bCollMap = FALSE;
	fMapHeightTemp = CheckMap(m_vPos + m_fAmorHeight*m_vUp);
	if(m_bCollMap)
	{// Ŕ§şÎşĐ Ăćµą
		vParticlePos = m_vPos + m_fAmorHeight*m_vUp;
		if(g_pD3dApp->m_pEffectList && !g_pScene->FindEffect(RC_EFF_COLL_01))
		{
			CAppEffectData * pData = new CAppEffectData(NULL,RC_EFF_COLL_01,vParticlePos);
			pData = (CAppEffectData *)g_pD3dApp->m_pEffectList->AddChild(pData);
		}
		m_vPos += SHUTTLE_MAP_COLL_RATE*m_vNormal*fElapsedTime;
		D3DXVec3Cross(&vSide,&m_vUp,&m_vVel);
//		m_fShuttleSpeed = m_fRealShuttleSpeedMin>30 ? m_fRealShuttleSpeedMin : 30;
//		D3DXMatrixRotationAxis(&m,&vSide,0.05f*m_fShuttleSpeed*fElapsedTime);
//		D3DXVec3TransformCoord(&m_vVel,&m_vVel,&m);
//		D3DXVec3TransformCoord(&m_vUp,&m_vUp,&m);
		// 2006-05-09 by ispark, Ăćµą ąÝ»ç ÄÚµĺ şŻ°ć
		GetCollReflect(&m_vVel, &m_vVel, &m_vNormal);
		//¸¶żě˝ş ÁÂÇĄ ľĆ·ˇ·Î ŔĚµż - 50¸¸Ĺ­
//		POINT pt,pt2;
//		GetCursorPos(&pt);
//		pt2 = pt;
//		ScreenToClient(g_pD3dApp->GetHwnd(), &pt);
//		CheckMouseReverse(&pt);
//
//		int nY = pt2.y - pt.y;
//		nY += g_pD3dApp->GetBackBufferDesc().Height/2 + SHUTTLE_MAX_MOUSE_MOVE*g_pD3dApp->GetBackBufferDesc().Height/600;
//		SetCursorPos(pt2.x,nY);
		bIsColl = TRUE;
	}

	// 2009. 07. 07 by ckPark ·Îşż±âľî żäĂ»»çÇ×(·Ń¸µ, Ľ±ĹĂČ­¸é, ą«±â, A±âľîĆ÷´ë)
	//if(bIsColl)
	
	// ·Îşż±âľî Ćň»ó˝ĂżÍ ŔĚ·ú˝Ă łôŔĚ Â÷ŔĚ°ˇ ¸ąŔĚłŞ ±Ľ°îŔÖ´Â ÁöÇüżˇĽ­ ŔĚ·ú˝Ă
	// Ăćµą Ăł¸®·Î ŔÎÇŘ żˇłĘÁö°ˇ ±ďŔĚąÇ·Î A±âľî ŔĚ·úÁßŔĎ˝Ă ĂćµąĆĐĹ¶ ş¸ł»Áö ľĘŔ˝
	if( bIsColl
		&& !( IS_DT( m_myShuttleInfo.UnitKind ) && ( GetCurrentBodyCondition() & BODYCON_TAKEOFF_MASK ) ) )
	// end 2009. 07. 07 by ckPark ·Îşż±âľî żäĂ»»çÇ×(·Ń¸µ, Ľ±ĹĂČ­¸é, ą«±â, A±âľîĆ÷´ë)

	{
		// ÁöÇü°ú Ăćµą˝Ă
		m_fShuttleSpeed = m_fRealShuttleSpeedMin>60 ? m_fRealShuttleSpeedMin : 60;
		g_pCamera->ChangeCamCollType();
		if(m_fCollSendDamageCheckTime < 0.0f)
		{
			if(m_myShuttleInfo.CurrentHP > 0.0f && m_myShuttleInfo.Level > 5 && m_fShuttleSpeed != 0)
			{
				// °ü¸®ŔÚ, żîżµŔÚ, µĄ¸đ°čÁ¤ µĄąĚÁö ľřŔ˝
//				if(!COMPARE_RACE(m_myShuttleInfo.Race,RACE_OPERATION|RACE_GAMEMASTER|RACE_DEMO))
				{
#ifdef _INET_CANNOT_CRASH_UNDER_ATT
					if (!IsMissileWarning())
					{
#endif
					SendPacket(T_FC_CHARACTER_APPLY_COLLISION_DAMAGE, COLLISION_TYPE_LAND);
					m_fCollSendDamageCheckTime = 0.5f;

					// 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
					// ÁöÇüĂćµą
					g_pD3dApp->SetJoyStickFeedback(TRUE, JOY_FEED_COLLISION_DAMAGE);
					// end 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
#ifdef _INET_CANNOT_CRASH_UNDER_ATT
				}
					else
					{
						SendPacket(T_FC_CHARACTER_APPLY_COLLISION_DAMAGE, COLLISION_TYPE_INET);
						m_fCollSendDamageCheckTime = 0.5f;
						g_pD3dApp->SetJoyStickFeedback(TRUE, JOY_FEED_COLLISION_DAMAGE);
					}
#endif
				}
			}
		}
//		if(m_bFormationFlight)
		if(m_pClientParty->IsFormationFlightMove())
		{
			SendPacket(T_IC_PARTY_CANCEL_FLIGHT_POSITION);
			m_pClientParty->FormationFlightClear();
//			m_infoParty.bFormationFlyingPositionType = 0;
//			m_bFormationFlight = FALSE;
			g_pD3dApp->m_pChat->CreateChatChild(STRERR_C_PARTY_0001,COLOR_SYSTEM);//"ĂćµąŔĚ ąß»ýÇĎż© Ćí´ëşńÇŕżˇĽ­ ąţľîłµ˝Ŕ´Ď´Ů."
		}

	}
//	if(bSetShuttleFlightInit)
//		SetShuttleFlightInit(fElapsedTime,true);
}

void CShuttleChild::CheckCollForObject(float fElapsedTime)
{
	FLOG( "CShuttleChild::CheckCollForObject(float fElapsedTime)" );

//28-07-2024 by Inet - allow wall & object passing by staff from tool
	if(g_pD3dApp->m_bNoClipping)
		return;

	// 2007-03-21 by dgwoo żÉŔúąö ¸đµĺ˝Ă ĂćµąĂł¸®´Â ÇĎÁö ľĘ´Â´Ů.
	if(IsObserverMode())
		return;
	//*--------------------------------------------------------------------------*//
	// 2006-11-08 by ispark
	if(CheckCollForObjectFrontByDistance(fElapsedTime, m_vPos, m_vOldPos))
		return;
	//*--------------------------------------------------------------------------*//

	// 2007-01-04 by bhsohn ±âĂĽ ÂřÁö ČÄ ¸ó˝şĹÍżÍ Ăćµą Ăł¸® ľČµÇ´Â ą®Á¦ Ăł¸®
//	if(m_bOrderMoveType)
//	{
//		return;
//	}
	// end 2007-01-04 by bhsohn ±âĂĽ ÂřÁö ČÄ ¸ó˝şĹÍżÍ Ăćµą Ăł¸® ľČµÇ´Â ą®Á¦ Ăł¸®
	
	m_vCollCenter = D3DXVECTOR3(0,0,0);
	// żŔşęÁ§Ć® ą× ¸ó˝şĹÍ, Ĺ¸Äł¸ŻĹÍ żÍŔÇ Ăćµą °ËĂâ
	
	COLLISION_RESULT collResult;
	
	D3DXMATRIX matArray[6];
	D3DXVECTOR3 vSide,vVel,vParticlePos;
	
	D3DXVec3Cross(&vSide,&m_vUp,&m_vVel);
	
	D3DXMatrixLookAtLH(&matArray[COLL_FRONT],&m_vPos,&(m_vPos + m_vVel),&m_vUp);	// ľŐ
	D3DXMatrixLookAtLH(&matArray[COLL_BACK],&m_vPos,&(m_vPos - m_vVel),&m_vUp);	// µÚ
	D3DXMatrixLookAtLH(&matArray[COLL_LEFT],&m_vPos,&(m_vPos - vSide),&m_vUp);		// ÁÂ
	D3DXMatrixLookAtLH(&matArray[COLL_RIGHT],&m_vPos,&(m_vPos + vSide),&m_vUp);		// żě
	D3DXMatrixLookAtLH(&matArray[COLL_UP],&m_vPos,&(m_vPos + m_vUp),&vSide);		// Ŕ§
	D3DXMatrixLookAtLH(&matArray[COLL_DOWN],&m_vPos,&(m_vPos - m_vUp),&vSide);		// ľĆ·ˇ
	
	float fFrontMove = m_fShuttleSpeed*fElapsedTime;
//	float fSideMove = m_fLRVelRate*m_fRealShuttleSpeedMax*fElapsedTime;
	float size[6] = { 10+fFrontMove,10-fFrontMove,10,10,5,5 };	// ŔŻ´Ö Ĺ©±â	ľŐ,µÚ,ÁÂ,żě,Ŕ§,ľĆ·ˇ
	
	// żŔşęÁ§Ć®żÍ Ăćµą Ăł¸® 2004.06.18 jschoi
	
	if(	/*m_bCollObjectCheck && */
		g_pScene &&
		g_pScene->m_byMapType != MAP_TYPE_TUTORIAL &&
		g_pScene->m_pObjectRender)
	{
		collResult = g_pScene->m_pObjectRender->CheckCollMesh(matArray,size);
		
		// 2006-06-29 by ispark Ăćµą ąö±× ĽöÁ¤
		vVel = collResult.vNormalVector;
//		D3DXVECTOR3 vIVel = -1.0f * m_vVel;
		D3DXVECTOR3 vIVel = -1.0f * m_vMoveVel;
		D3DXVECTOR3 vNewVel;
		float fDot1 = D3DXVec3Dot(&vIVel,&vVel);
//		if(collResult.nCollType != COLL_NONE)
//			DBGOUT("ąćÇâ %f %f %f(dot = %f)%f %f %f\n", m_vMoveVel.x, m_vMoveVel.y, m_vMoveVel.z, fDot1, vVel.x, vVel.y, vVel.z);
//		DBGOUT("°Ĺ¸® %f(Type = %d)\n", collResult.fDist, collResult.nCollType);

		if((collResult.nCollType != COLL_NONE) && fDot1 > 0) // ĂćµąŔĚ ŔÖ´Ů¸é..
		{
			if(m_myShuttleInfo.Level > 5 && m_fCollSendDamageCheckTime < 0.0f && m_fShuttleSpeed != 0)
			{
				// °ü¸®ŔÚ, żîżµŔÚ, µĄ¸đ°čÁ¤ µĄąĚÁö ľřŔ˝
//				if(!COMPARE_RACE(m_myShuttleInfo.Race,RACE_OPERATION|RACE_GAMEMASTER|RACE_DEMO))
				{
#ifdef _INET_CANNOT_CRASH_UNDER_ATT
					if (!IsMissileWarning())
					{
#endif
					SendPacket(T_FC_CHARACTER_APPLY_COLLISION_DAMAGE, COLLISION_TYPE_BUILDING);
					m_fCollSendDamageCheckTime = 1.0f;
					// 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
					// ÁöÇüĂćµą
					g_pD3dApp->SetJoyStickFeedback(TRUE, JOY_FEED_COLLISION_DAMAGE);
					// end 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
#ifdef _INET_CANNOT_CRASH_UNDER_ATT
				}
					else
					{
						SendPacket(T_FC_CHARACTER_APPLY_COLLISION_DAMAGE, COLLISION_TYPE_INET);
						m_fCollSendDamageCheckTime = 1.0f;
						g_pD3dApp->SetJoyStickFeedback(TRUE, JOY_FEED_COLLISION_DAMAGE);
					}
#endif			
				}
			}
			m_fCheckCollTime = 0.0f;

			// 2006-07-13 by ispark, şńÇŕ˝Ăżˇ´Â ĂÖŔú ĽÓµµ·Î şŻ°ćÇĎÁö¸¸, ¶ĄżˇĽ­ ´Ů´Ň°ćżě 20 ĽÓµµ·ÎÇŃ´Ů.
			// ¶ĄżˇĽ­´Â ĽÓµµ°ˇ ¸Ĺżě Ĺ©Áö ľĘ±â ¶§ą®ŔĚ´Ů.
			if(m_bIsAir == TRUE)
			{
				m_fShuttleSpeed = m_fRealShuttleSpeedMin>60 ? m_fRealShuttleSpeedMin : 60;
			}
			else
			{
				m_fShuttleSpeed = 20.0f;
			}

			g_pCamera->ChangeCamCollType();
			g_pCamera->SetSmoothMove();
//			DBGOUT("Ăćµą--------------> %f %f %f\n", vVel.x, vVel.y, vVel.z);
			m_vPos = m_vOldPos + 0.5f*m_fShuttleSpeed*vVel*fElapsedTime;
//			float fAngleDegree = ACOS(D3DXVec3Dot(&m_vVel,&vVel))/PI;
//			D3DXVec3Normalize(&m_vVel,&(m_vVel + fAngleDegree*vVel));
			// 2006-08-08 by ispark, ¸Ę łˇżˇĽ­ Ăćµą
			if(m_vPos.y > g_pScene->m_nMaxAtitudeHeight + g_pGround->m_projectInfo.fWaterHeight)
			{
				vNewVel.x = g_pGround->m_projectInfo.sXSize*TILE_SIZE/2.0f+rand()%100;
				vNewVel.y = 0.0;
				vNewVel.z = g_pGround->m_projectInfo.sYSize*TILE_SIZE/2.0f+rand()%100;
			}
			else
			{
				// 2006-05-06 by ispark, Ăćµą ąÝ»ç ÄÚµĺ şŻ°ć
				vNewVel = (2 * fDot1) * vVel + m_vVel;
			}

			D3DXVec3Normalize(&m_vVel,&vNewVel);
			D3DXVec3Cross(&vSide,&m_vUp,&m_vVel);
			D3DXVec3Cross(&m_vUp,&m_vVel,&vSide);
			vParticlePos = m_vPos + 8.0f*m_vVel;
			
			if(collResult.nCollType == COLL_WHOLE)
			{
				// ŔĚ°÷Ŕ¸·Î µéľîżÂ´Ů¸é łŞÁßżˇ ąş°ˇ Ăł¸®ÇŘ ÁÖŔÚ.
				DBGOUT("Whole Collision \n");
			}
			if(g_pD3dApp->m_pEffectList)
			{
				CAppEffectData * pData = new CAppEffectData(NULL,RC_EFF_COLL_01,vParticlePos);
				pData = (CAppEffectData *)g_pD3dApp->m_pEffectList->AddChild(pData);
			}
			SetCursorInit();

//			if(m_bFormationFlight)
			if(m_pClientParty->IsFormationFlightMove())
			{
				SendPacket(T_IC_PARTY_CANCEL_FLIGHT_POSITION);
				m_pClientParty->FormationFlightClear();
//				m_infoParty.bFormationFlyingPositionType = 0;
//				m_bFormationFlight = FALSE;
				g_pD3dApp->m_pChat->CreateChatChild(STRERR_C_PARTY_0001,COLOR_SYSTEM);
			}

			// 2005-11-17 by ispark ĆäłÎĆĽ »óČ˛ Áľ·á
			if(m_bPenaltyUnitStop == TRUE)
			{
				InitUnitStop();
			}
		}
	}
	
	
	// ¸Ę°úŔÇ ĂćµąŔĚ ľřŔ»¶§¸¸ ¸ó˝şĹÍ ĽĹĆ˛°úŔÇ Ăćµą °Ë»ç¸¦ ÇŃ´Ů.
	if(collResult.nCollType == COLL_NONE && !m_bCollMap)
	{
		if(collResult.nCollType == COLL_NONE && m_bCollMonsterCheck)
		{
			float fColledMonsterSize = 0;
			ClientIndex_t monsterIndex = 0;
			collResult = g_pScene->m_pMonsterRender->CheckCollMesh(matArray,size,&fColledMonsterSize, &monsterIndex);
			//2011-06-01 by jhahn	ĂćµąĂł¸® ˝Ă°ŁĂĽĹ© şŻ°ć
			//if(collResult.nCollType != COLL_NONE && m_fCollSendDamageCheckTime < 0.0f) // ĂćµąŔĚ ŔÖ´Ů¸é..	
			if(collResult.nCollType != COLL_NONE) // ĂćµąŔĚ ŔÖ´Ů¸é..
			// end 2011-06-01 by jhahn	ĂćµąĂł¸® ˝Ă°ŁĂĽĹ© şŻ°ć
			{
				char buffer[512];
				INIT_MSG(MSG_FC_CHARACTER_APPLY_COLLISION_DAMAGE, T_FC_CHARACTER_APPLY_COLLISION_DAMAGE, pSColl, buffer);
				pSColl->CollisionType	= COLLISION_TYPE_MONSTER;
				pSColl->SpeedOfCollision = m_fShuttleSpeed - m_fTempGravity;			// 2005-07-11 by ispark	// ĂćµąĂł¸®
				pSColl->UnitIndex		= monsterIndex;
				g_pD3dApp->m_pFieldWinSocket->Write(buffer, MSG_SIZE(MSG_FC_CHARACTER_APPLY_COLLISION_DAMAGE));

				// 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
				// ÁöÇüĂćµą
				g_pD3dApp->SetJoyStickFeedback(TRUE, JOY_FEED_COLLISION_DAMAGE);
				// end 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷

				// 2004-11-24 by ydkim
				//SendPacket(T_FC_CHARACTER_APPLY_COLLISION_DAMAGE, COLLISION_TYPE_MONSTER);

				m_fCollSendDamageCheckTime = 1.0f;
				m_fCheckCollTime = 0.0f;

				m_fShuttleSpeed = m_fRealShuttleSpeedMin>30 ? m_fRealShuttleSpeedMin : 30;
   
				g_pCamera->ChangeCamCollType();
				g_pCamera->SetSmoothMove();
				vVel = collResult.vNormalVector;
				// 2004-10-22 by jschoi Ăćµą ˝Ă Č¸Ŕü °Ş ĽłÁ¤
				fColledMonsterSize = fColledMonsterSize/100;
				if(fColledMonsterSize < 0.2f)
				{	
					fColledMonsterSize = 0.2f;
				}
				else if(fColledMonsterSize > 1.0f)
				{
					fColledMonsterSize = 1.0f;
					m_vPos = m_vOldPos + 0.5f*m_fShuttleSpeed*vVel*fElapsedTime;
					float fColledMonsterSize = ACOS(D3DXVec3Dot(&m_vVel,&vVel))/PI;
				}
				D3DXVec3Normalize(&m_vVel,&(m_vVel + fColledMonsterSize*vVel));
				D3DXVec3Cross(&vSide,&m_vUp,&m_vVel);
				D3DXVec3Cross(&m_vUp,&m_vVel,&vSide);
				vParticlePos = m_vPos;
				
				if(collResult.nCollType == COLL_WHOLE)
				{
					// ŔĚ°÷Ŕ¸·Î µéľîżÂ´Ů¸é łŞÁßżˇ ąş°ˇ Ăł¸®ÇŘ ÁÖŔÚ.
					DBGOUT("Whole Collision \n");
				}
				if(g_pD3dApp->m_pEffectList)
				{
					CAppEffectData * pData = new CAppEffectData(NULL,RC_EFF_COLL_01,vParticlePos);
					pData = (CAppEffectData *)g_pD3dApp->m_pEffectList->AddChild(pData);
				}
				SetCursorInit();

				// 2005-11-17 by ispark ĆäłÎĆĽ »óČ˛ Áľ·á
				if(m_bPenaltyUnitStop == TRUE)
				{
					InitUnitStop();
				}
			}
		}
		// ĽĹĆ˛ Ăćµą ÇöŔç »çżë ľČÇÔ
		//	else if(nCollType == COLL_NONE && m_bCollShuttleCheck)
		//	{
		//		nCollType = g_pD3dApp->m_pShuttleRender->CheckCollMesh(matArray,size);
		//	}
	}
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			BOOL CShuttleChild::CheckCollForObjectFrontByDistance(float fElapsedTime, D3DXVECTOR3 vNewPos D3DXVECTOR3 vOldPos)
/// \brief		żŔşęÁ§Ć® °Ĺ¸®·Î ĆÇ´Ü
/// \author		ispark
/// \date		2006-11-08 ~ 2006-11-08
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////		
BOOL CShuttleChild::CheckCollForObjectFrontByDistance(float fElapsedTime, D3DXVECTOR3 vNewPos, D3DXVECTOR3 vOldPos)
{
	FLOG( "CShuttleChild::CheckCollForObjectFrontByDistance(float fElapsedTime, D3DXVECTOR3 vNewPos, D3DXVECTOR3 vOldPos)" );
	// 2008-10-16 by bhsohn A±âľî Ăćµą ąö±× ĽöÁ¤
	if((m_dwState == _LANDING) && (IS_DT(m_myShuttleInfo.UnitKind))	)
	{
		// A±âľî Âř·úÁßżˇ´Â °Ĺ¸®żˇ µű¸Ą ·Łµů ĂĽĹ©ľČÇÔ.
		return FALSE;
	}
	// end 2008-10-16 by bhsohn A±âľî Ăćµą ąö±× ĽöÁ¤
	
		
	COLLISION_RESULT collResult;
	
	D3DXMATRIX	matColl;
	D3DXVECTOR3 vSide,vVel,vParticlePos;

	D3DXVECTOR3 vDistVel	= vNewPos - vOldPos;										// ąćÇâ
	float		fDist		= D3DXVec3Length(&vDistVel);								// °Ĺ¸®
	
	D3DXVec3Normalize(&vDistVel, &vDistVel);
	D3DXMatrixLookAtLH(&matColl,&vOldPos,&(vOldPos + vDistVel),&m_vUp);					// ľŐ

	// Ăćµą °Ë»ç
	if(	/*m_bCollObjectCheck && */
		g_pScene &&
		g_pScene->m_byMapType != MAP_TYPE_TUTORIAL &&
		g_pScene->m_pObjectRender)
	{
		collResult = g_pScene->m_pObjectRender->CheckCollMesh(matColl, vOldPos);
		
		vVel = collResult.vNormalVector;
		D3DXVECTOR3 vNewVel;
		D3DXVECTOR3 vTempDistVel = -1.0f * vDistVel;
		float fDot1 = D3DXVec3Dot(&vTempDistVel,&vVel);
//		DBGOUT("Ŕü %f %f %f, Çö %f %f %f\n", vOldPos.x, vOldPos.y, vOldPos.z, vNewPos, vNewPos.x, vNewPos.y, vNewPos.z);

		// °Ĺ¸® şń±ł, żŔşęÁ§Ć® łë¸Ö ąćÇâ°ú °ü°č ľř´Ů.
		// ł»°ˇ żňÁ÷ŔÎ °Ĺ¸® »çŔĚżˇ żŔşęÁ§Ć®°ˇ ŔÖ´Ů¸é Ăćµą Ăł¸®
		// 2007-06-12 by bhsohn A±âľî ¸¶Ŕ»żˇ Âř·ú˝Ă µĄąĚÁö µéľî°ˇ´Â Çö»ó Ăł¸®
		//if(collResult.fDist < fDist)
		if(collResult.fDist < fDist && (FALSE == g_pD3dApp->IsMyShuttleCharacter()))
		{
//			DBGOUT("żřş» °Ĺ¸® = %f, Ăćµą °Ĺ¸® = %f\n", fDist, collResult.fDist);
			if(m_myShuttleInfo.Level > 5 && m_fCollSendDamageCheckTime < 0.0f && m_fShuttleSpeed != 0)
			{
#ifdef _INET_CANNOT_CRASH_UNDER_ATT
				if (!IsMissileWarning())
				{
#endif
					SendPacket(T_FC_CHARACTER_APPLY_COLLISION_DAMAGE, COLLISION_TYPE_BUILDING);
					m_fCollSendDamageCheckTime = 1.0f;

					// 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
					// ÁöÇüĂćµą
					g_pD3dApp->SetJoyStickFeedback(TRUE, JOY_FEED_COLLISION_DAMAGE);
					// end 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
#ifdef _INET_CANNOT_CRASH_UNDER_ATT
				}
				else
				{
					SendPacket(T_FC_CHARACTER_APPLY_COLLISION_DAMAGE, COLLISION_TYPE_INET);
					m_fCollSendDamageCheckTime = 1.0f;
					g_pD3dApp->SetJoyStickFeedback(TRUE, JOY_FEED_COLLISION_DAMAGE);
				}
#endif
			}
			m_fCheckCollTime = 0.0f;

			if(m_bIsAir == TRUE)
			{
				m_fShuttleSpeed = m_fRealShuttleSpeedMin>60 ? m_fRealShuttleSpeedMin : 60;
			}
			else
			{
				m_fShuttleSpeed = 20.0f;
			}

			g_pCamera->ChangeCamCollType();
			g_pCamera->SetSmoothMove();
//			DBGOUT("Ăćµą--------------> %f %f %f\n", vVel.x, vVel.y, vVel.z);
			m_vPos = m_vOldPos + 0.5f*m_fShuttleSpeed*vVel*fElapsedTime;
			// 2006-08-08 by ispark, ¸Ę łˇżˇĽ­ Ăćµą
			if(m_vPos.y > g_pScene->m_nMaxAtitudeHeight + g_pGround->m_projectInfo.fWaterHeight)
			{
				vNewVel.x = g_pGround->m_projectInfo.sXSize*TILE_SIZE/2.0f+rand()%100;
				vNewVel.y = 0.0;
				vNewVel.z = g_pGround->m_projectInfo.sYSize*TILE_SIZE/2.0f+rand()%100;
			}
			else
			{
				// 2006-05-06 by ispark, Ăćµą ąÝ»ç ÄÚµĺ şŻ°ć
				vNewVel = (2 * fDot1) * vVel + m_vVel;
			}

			D3DXVec3Normalize(&m_vVel,&vNewVel);
			D3DXVec3Cross(&vSide,&m_vUp,&m_vVel);
			D3DXVec3Cross(&m_vUp,&m_vVel,&vSide);
			vParticlePos = m_vPos + 8.0f*m_vVel;
			
			if(collResult.nCollType == COLL_WHOLE)
			{
				// ŔĚ°÷Ŕ¸·Î µéľîżÂ´Ů¸é łŞÁßżˇ ąş°ˇ Ăł¸®ÇŘ ÁÖŔÚ.
				DBGOUT("Whole Collision \n");
			}
			if(g_pD3dApp->m_pEffectList)
			{
				CAppEffectData * pData = new CAppEffectData(NULL,RC_EFF_COLL_01,vParticlePos);
				pData = (CAppEffectData *)g_pD3dApp->m_pEffectList->AddChild(pData);
			}
			SetCursorInit();

			if(m_pClientParty->IsFormationFlightMove())
			{
				SendPacket(T_IC_PARTY_CANCEL_FLIGHT_POSITION);
				m_pClientParty->FormationFlightClear();
				g_pD3dApp->m_pChat->CreateChatChild(STRERR_C_PARTY_0001,COLOR_SYSTEM);
			}

			// 2005-11-17 by ispark ĆäłÎĆĽ »óČ˛ Áľ·á
			if(m_bPenaltyUnitStop == TRUE)
			{
				InitUnitStop();
			}

			return TRUE;
		}
	}
	
	return FALSE;
}

void CShuttleChild::SetShuttleFlightInit(float fElapsedTime,bool bType)							// ĽĹĆ˛ŔÇ ĆňÇü¸¶Ăß±â
{
	FLOG( "CShuttleChild::SetShuttleFlightInit(float fElapsedTime,bool bType)" );
	D3DXVECTOR3 vVel,v,v2;
	D3DXMATRIX m;
	// Ăćµą ŔÖŔ»˝Ăżˇ ŔŻ´Ö°íÁ¤ľřŔ˝
	// 2010. 05. 27 by jskim ˝Ăł×¸¶ Ŕűżë Ä«¸Ţ¶ó ±¸Çö
	//if(m_bCollMap)
	if(m_bCollMap || IsShowCinema() )
	//end 2010. 05. 27 by jskim ˝Ăł×¸¶ Ŕűżë Ä«¸Ţ¶ó ±¸Çö
		return;

	float fAngle1, fAngle2,fAngleRate;
	fAngleRate = 1.0f;
	if(bType)
	{
		vVel = m_vVel;
		vVel.y = 0.0f;
		if(vVel.x == 0.0f && vVel.z == 0.0f)
		{// żˇ·Ż
			vVel.x = 1.0f;
		}
		D3DXVec3Normalize(&vVel,&vVel);
		fAngle1 = ACOS(D3DXVec3Dot(&vVel,&m_vVel));
		D3DXVec3Cross(&m_vSideVel,&m_vUp,&m_vVel);
		if(m_vUp.y > 0.0f)
		{
			if(m_vVel.y > 0.0f)
			{
				D3DXMatrixRotationAxis(&m, &m_vSideVel, fAngleRate*fAngle1*fElapsedTime);
				D3DXVec3TransformCoord(&m_vVel, &m_vVel, &m);
				D3DXVec3TransformCoord(&m_vUp, &m_vUp, &m);
				if(m_vVel.y < 0.0f)
				{
					m_vVel.y = 0.0f;
				}
			}
			else
			{
				D3DXMatrixRotationAxis(&m, &m_vSideVel, -fAngleRate*fAngle1*fElapsedTime);
				D3DXVec3TransformCoord(&m_vVel, &m_vVel, &m);
				D3DXVec3TransformCoord(&m_vUp, &m_vUp, &m);
				if(m_vVel.y > 0.0f)
				{
					m_vVel.y = 0.0f;
				}
			}
		}
		else
		{
			if(m_vVel.y > 0.0f)
			{
				D3DXMatrixRotationAxis(&m, &m_vSideVel, -fAngleRate*fAngle1*fElapsedTime);
				if(m_vVel.y < 0.0f)
				{
					m_vVel.y = 0.0f;
				}
			}
			else
			{
				D3DXMatrixRotationAxis(&m, &m_vSideVel, fAngleRate*fAngle1*fElapsedTime);
				if(m_vVel.y > 0.0f)
				{
					m_vVel.y = 0.0f;
				}
			}
		}
		D3DXVec3TransformCoord(&m_vVel, &m_vVel, &m);
		D3DXVec3TransformCoord(&m_vUp, &m_vUp, &m);
		if(abs(m_vVel.y) < 0.0001f)
		{
			m_vVel.y = 0.0f;
		}
	}
	v = D3DXVECTOR3(0,1,0);
	D3DXVec3Cross(&v2,&m_vUp,&m_vVel);
	fAngle2 = ACOS(D3DXVec3Dot(&v,&m_vUp));
	if(v2.y > 0.0f)
	{
		D3DXMatrixRotationAxis(&m, &m_vVel, -fAngleRate*fAngle2*fElapsedTime);
		D3DXVec3TransformCoord(&m_vUp, &m_vUp, &m);
		D3DXVec3Cross(&v2,&m_vUp,&m_vVel);
		if(v2.y < 0.0f)
			m_vUp = v;
	}
	else
	{
		D3DXMatrixRotationAxis(&m, &m_vVel, fAngleRate*fAngle2*fElapsedTime);
		D3DXVec3TransformCoord(&m_vUp, &m_vUp, &m);
		D3DXVec3Cross(&v2,&m_vUp,&m_vVel);
		if(v2.y > 0.0f)
			m_vUp = v;
	}
	
	if(bType)
	{
		if(m_fSideRate > 0.0f)
		{
			m_fSideRate -= 3.0f*fElapsedTime;
			if(m_fSideRate < 0.0f)
				m_fSideRate = 0.0f;
		}
		else if(m_fSideRate < 0.0f)
		{
			m_fSideRate += 3.0f*fElapsedTime;
			if(m_fSideRate > 0.0f)
				m_fSideRate = 0.0f;
		}
		if(fAngle1 < 0.01f && fAngle2 < 0.01f && abs(m_fSideRate) < 0.01f)
			m_bIsSetFlighting = FALSE;
	}
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			CShuttleChild::ChangeUnitStateDead()
/// \brief		Á×ľúŔ» ¶§ µüÇŃąř Ăł¸®
/// \author		dhkwon
/// \date		2004-06-30 ~ 2004-06-30
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CShuttleChild::ChangeUnitStateDead()
{
	// 2014-02-20 by ymjoo 리젠 후 움직이지 않는 버그 수정
	m_bOrderMove = FALSE;
	g_pD3dApp->m_bFixUnitDirMode = FALSE;
	// END 2014-02-20 by ymjoo 리젠 후 움직이지 않는 버그 수정
	// 2010-06-15 by shcho&hslee Ćę˝Ă˝şĹŰ - żŔĆŰ·ąŔĚĹÍ ±¸Çö
	if( g_pStoreData->FindItemInInventoryByWindowPos( POS_PET ) )
	{
		g_pInterface->m_pINFOpMain->GetOpInfo()->SetOperatorAction(1,4);
	}
	// end 2010-06-15 by shcho&hslee Ćę˝Ă˝şĹŰ - żŔĆŰ·ąŔĚĹÍ ±¸Çö
	// 2009. 09. 24 by jskim ÇĘµĺ Ă˘°í Äł˝¬ ľĆŔĚĹŰ
	g_pInterface->m_pCityBase->CloseCurrentEnterBuilding();
	//end 2009. 09. 24 by jskim ÇĘµĺ Ă˘°í Äł˝¬ ľĆŔĚĹŰ

	// 2007-04-05 by bhsohn şňşŐ ąö±× Ăł¸®
	// Á×ľúŔ»˝Ă °ú°Ĺ ˝şĹłĂĘ±âČ­
	m_pSkill->InitPreSkillItemNum();

	// 2008-11-04 by bhsohn Á×ľúŔ»˝Ă, Ĺ¸ÄĎÇü ˝şĹł ÇŘÁ¦
	m_pSkill->SetSkillTargetState(FALSE);		// ˝şĹł ¸¶żě˝ş Ć÷ŔÎĹÍ ÇŘÁ¦

	// 2009-02-19 by bhsohn °Ĺ·ˇÁß ŔŻ´Ö Á×ľúŔ»¶§żˇ ´ëÇŃ Ăł¸®
	if(g_pGameMain->m_pTrade)
	{
		if(g_pGameMain->m_pTrade->m_bTrading)
		{
			// °Ĺ·ˇÁßŔĚ¸é ĂëĽŇ¸¦ ş¸ł˝´Ů.
			g_pGameMain->m_pTrade->CancelTrade();
			if(g_pD3dApp->m_pChat)
			{
				g_pD3dApp->m_pChat->CreateChatChild(STRMSG_C_TRADE_0015,COLOR_TRADE);//"°Ĺ·ˇ°ˇ ĂëĽŇ µÇľú˝Ŕ´Ď´Ů."
			}
		}
	}
	// end 2009-02-19 by bhsohn °Ĺ·ˇÁß ŔŻ´Ö Á×ľúŔ»¶§żˇ ´ëÇŃ Ăł¸®

	// 2007-10-16 by bhsohn °Ĺ·ˇ »óżˇ Á×ľúŔ»˝Ă, Ĺ° ľČ¸Ô´Â ą®Á¦Ăł¸®
	// °Ĺ·ˇÁßŔĚ¶ó¸é °Ĺ·ˇµµ Ç¬´Ů.
	g_pGameMain->SetTradeInfo(FALSE);

	m_fShuttleSpeed = 0;
	m_pTarget = NULL;
	m_pOrderTarget = NULL;
	m_mMatrix._32 = 0;
	m_mMatrix._21 = 0;
	m_mMatrix._22 = 1;
	m_mMatrix._23 = 0;
	// 2005-08-01 by ispark
	// Ä«¸Ţ¶ó ĆĐĹĎ »çżë ľČÇÔ
	m_bIsCameraPattern = FALSE;
	// Permanent¸¦ Á¦żÜÇŃ »çżëÁßŔÎ ¸đµç ˝şĹł ÇŘÁ¦
	// 2008-12-29 by bhsohn »ç¸Á ˝Ă ąöÇÁ ˝şĹł ŔŻÁö ˝Ă˝şĹŰ
	//m_pSkill->ReleaseAllUsingSkillWithOutPermanent();
	m_pSkill->ReleaseStatDEAD_AllUsingSkillWithOutPermanent();


	// 2008. 12. 16 by ckPark ľĆŔĚĹŰ »çżë Áöż¬
	// Á×ľúŔ» ˝Ă µô·ąŔĚ ľĆŔĚĹŰ ĂĘ±âČ­
	g_pGameMain->ReleaseDelayItem();
	// end 2008. 12. 16 by ckPark ľĆŔĚĹŰ »çżë Áöż¬


	// 2008. 12. 23 by ckPark ˝şĹł »çżë Áöż¬ »çżë Áöż¬
	// Á×ľúŔ» ˝Ă µô·ąŔĚ ˝şĹł ĂĘ±âČ­
	g_pGameMain->ReleaseDelaySkill();
	// end 2008. 12. 23 by ckPark ˝şĹł »çżë Áöż¬ »çżë Áöż¬


	// 2005-08-22 by ispark
	// ż¬·á°ˇ 0ŔĚľú´Ů¸é FALLENŔ¸·Î. FALLING Ăł¸®´Â ŔĚąĚ ÇßŔ˝
	if( InitCinemaUnit(PATTERN_UNIT_FALLING, TRUE) == FALSE || m_dwState == _FUELALLIN)
 	{
		// 2005-03-28 by jschoi - Á×ľúŔ» ¶§ şÎČ° Ä«µĺ »çżë
		CItemInfo* pItemInfo = g_pStoreData->SerchRevivalUseItem();
		MAP_INFO* pMapInfo = g_pDatabase->GetMapInfo(g_pShuttleChild->m_myShuttleInfo.MapChannelIndex.MapIndex);
// 2005-12-05 by ispark
//		if(g_pD3dApp->m_bDeadByP2PPK == TRUE)// 1:1°áĹőÇĎ´Ů°ˇ Ŕűżˇ°Ô Á×Ŕş °ćżě
		// 2006-01-02 by ispark, ´Ů¸Ą ĽĽ·Â¸ĘŔÎÁö ĆÇ´Ü 
		// 2006-04-10 by ispark, şÎČ° Ä«µĺ ĽöÁ¤, 1:1 °áĹő´Â Á¦żÜ
		// 2006-11-17 by ispark, ŔĚşĄĆ® ¸ĘżˇĽ­´Â Á¦żÜ
		if(g_pD3dApp->m_bDeadByP2PPK == FALSE && 
			pItemInfo &&
			IsRevivalUseItem())
		{
			char strRevivalMsg[512] = {0,};
			// 2009-04-21 by bhsohn ľĆŔĚĹŰ DesParamĂß°ˇ
			//sprintf(strRevivalMsg, STRMSG_C_060410_0003, pItemInfo->ItemInfo->ItemName, pItemInfo->ItemInfo->ParameterValue1);	// "%s¸¦ »çżëÇĎ˝Ă°Ú˝Ŕ´Ď±î? Č®·üŔş %.0fŔÔ´Ď´Ů."
			sprintf(strRevivalMsg, STRMSG_C_060410_0003, pItemInfo->ItemInfo->ItemName, pItemInfo->ItemInfo->ArrParameterValue[0]);	// "%s¸¦ »çżëÇĎ˝Ă°Ú˝Ŕ´Ď±î? Č®·üŔş %.0fŔÔ´Ď´Ů."
			g_pGameMain->m_pInfWindow->AddMsgBox(strRevivalMsg, _Q_USE_NORMAL_RESTORE,0,0,0,pItemInfo->UniqueNumber);
		}
		else
		{
			// 2006-11-17 by ispark, şÎČ° Ä«µĺ°ˇ ŔÖÁö¸¸ ŔĚşĄĆ® ¸ĘŔĚ¶ó¸é
			if(pItemInfo && IS_MAP_INFLUENCE_EVENT_AREA(pMapInfo->MapInfluenceType))
			{
				g_pD3dApp->m_pChat->CreateChatChild(STRMSG_C_061117_0000, COLOR_ERROR);// "ŔĚşĄĆ®¸ĘżˇĽ­´Â %s ľĆŔĚĹŰŔ» »çżëÇŇ Ľö ľř˝Ŕ´Ď´Ů."
			}

			SendDeadMsg();
		}

//		else if(pItemInfo = g_pStoreData->FindItemInInventoryByItemNum(ITEM_NUM_CASH_CASH_NORMAL_RESTORE))
//		{
//			g_pGameMain->m_pInfWindow->AddMsgBox(STRMSG_C_CARD_0004, _Q_USE_NORMAL_RESTORE,0,0,0,pItemInfo->UniqueNumber);//"ŔĎąÝ şÎČ° Ä«µĺ¸¦ »çżëÇĎ˝Ă°Ú˝Ŕ´Ď±î?"
//		}
//		else if(pItemInfo = g_pStoreData->FindItemInInventoryByItemNum(ITEM_NUM_CASH_CASH_SUPER_RESTORE))
//		{
//			g_pGameMain->m_pInfWindow->AddMsgBox(STRMSG_C_CARD_0005, _Q_USE_SUPER_RESTORE,0,0,0,pItemInfo->UniqueNumber);//"˝´ĆŰ şÎČ° Ä«µĺ¸¦ »çżëÇĎ˝Ă°Ú˝Ŕ´Ď±î?"
//		}
//		else
//		{// şÎČ° Ä«µĺ°ˇ ľř´Ů¸é
//			g_pGameMain->m_pInfWindow->AddMsgBox(STRMSG_C_CARD_0003, _Q_DEAD_RETURN);//"°Ýłł°í·Î ŔĚµżÇŐ´Ď´Ů."
//		}

		ChangeUnitState( _FALLEN );
		g_fSendMovePacket = max(g_fSendMovePacket,5.0f);

		ResetBodyCondition(BODYCON_EXPLOSION_MASK);
	}
	else
	{
		m_bSkillMoveIsUse = FALSE;			// 2006-01-10 by ispark, ˝şĹł ą«şę¸¦ ľ˛Áö ľĘ´Â´Ů.(_FALLINGżˇ ŔÖŔ˝)
		RemoveEnemyTarget();				// 2006-01-13 by ispark, Ŕű Ĺ¸°Ů »čÁ¦
		ChangeUnitState( _FALLING );
		g_pGameMain->InitShowWindow();		// 2006-08-07 by ispark, Ă˘ ĂĘ±âČ­

		// 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
		// °ÝĂß Ářµż
		g_pD3dApp->SetJoyStickFeedback(TRUE, JOY_FEED_SHUTTLE_EXPLODED);
		// end 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
	}

	// 2010. 03. 25 by ckPark »ç¸Á˝Ă 5ĂĘŔĚ»ó Ăß¶ôÇĎ¸é şÎČ°¸ŢĽĽÁö Ăß°ˇ
	if( m_bDeadMessageEnable )
		m_dwDeadTime = timeGetTime();
	// end 2010. 03. 25 by ckPark »ç¸Á˝Ă 5ĂĘŔĚ»ó Ăß¶ôÇĎ¸é şÎČ°¸ŢĽĽÁö Ăß°ˇ
}

void CShuttleChild::ChangeBodyConditionFromServer(BodyCond_t hyBodyCondition)
{
	//Äł¸ŻĹÍ°ˇ Á×Ŕş °ćżě
	if(	COMPARE_BODYCON_BIT(hyBodyCondition,BODYCON_DEAD_MASK) )
	{
  		if( !COMPARE_BODYCON_BIT(GetCurrentBodyCondition(),BODYCON_EXPLOSION_MASK) )
		{
//			m_myShuttleInfo.CurrentHP = 0.0f;
//			m_pOrderTarget = NULL;
//			ChangeUnitStateDead();

			ChangeSingleBodyCondition(BODYCON_EXPLOSION_MASK);
			g_pD3dApp->SendFieldSocketChangeCharacterBodyCondition( BODYCON_DAMAGE1_MASK, FALSE );
			g_pD3dApp->SendFieldSocketChangeCharacterBodyCondition( BODYCON_DAMAGE2_MASK, FALSE );
			g_pD3dApp->SendFieldSocketChangeCharacterBodyCondition( BODYCON_DAMAGE3_MASK, TRUE );
			/////////////////////////////////////////////////////////////////
//			m_vNextPos = m_vPos - 70.0f*m_vVel + 100.0f*m_vUp;
			/////////////////////////////////////////////////////////////////
			m_fCurrentTime = 186.0f;
			// ŔĚÄł¸ŻŔ» Ĺ¸°ŮŔ¸·Î ĽÂĆĂÇĎ°í ŔÖ´Â °ÍµéŔ» Ĺ¸°ŮĂĘ±âČ­ ˝ĂĹ˛´Ů.
//			CMapEnemyIterator itEnemy = g_pScene->m_mapEnemyList.begin();
//			while(itEnemy != g_pScene->m_mapEnemyList.end())
//			{
//				itEnemy->second->m_bAttackEnemy = FALSE;
//				itEnemy->second->SetPkState( PK_ALL, FALSE );
//				SetAttackAvaliableEnemyIndex( itEnemy->second->m_infoCharacter.ClientIndex, FALSE );
//				itEnemy->second->m_nEnemyTypePrimary = ENEMYDATA_ENEMYLIST;
//				itEnemy->second->m_nEnemyTypeSecondary = ENEMYDATA_ENEMYLIST;
//				itEnemy++;
//			}
//			m_vecAttackAvailableEnemyIndex.clear();
//			CMapMonsterIterator itMonster = g_pScene->m_mapMonsterList.begin();
//			while(itMonster != g_pScene->m_mapMonsterList.end())
//			{
//				itMonster->second->m_nMonsterTypePrimary = MONSTERDATA_MONSTERLIST;
//				itMonster->second->m_nMonsterTypeSecondary = MONSTERDATA_MONSTERLIST;
//				itMonster++;
//			}
			// 2006-01-18 by ispark, Áö±Ý ł»»óĹÂ °Ë»ç, ĆřąßŔĚ ¸Â´ÂÁö...
			if(!UNIT_STATE_DEAD(m_dwState))
			{
				if(m_fCheckDead == 0.0f)
					m_fCheckDead = 1.0f;
//				DBGOUT("My State[%d] --> Dead BodyCondition Check On\n", m_dwState);
			}
		}

		return;
	}
	else
	{
		// 2009. 07. 07 by ckPark ·Îşż±âľî żäĂ»»çÇ×(·Ń¸µ, Ľ±ĹĂČ­¸é, ą«±â, A±âľîĆ÷´ë)
		
		// ·Ń¸µ˝Ă şÎ˝şĹÍ ľÖ´Ď¸ŢŔĚĽÇŔĚ µ¤ľîľşżöÁöąÇ·Î ¸·´Â´Ů(ĂĽŔÎ·Ń¸µ˝ĂłŞ ·Ń¸µ˝Ă ·şą®Á¦)
		if( COMPARE_BODYCON_BIT(hyBodyCondition, BODYCON_BOOSTER1_MASK | BODYCON_BOOSTER2_MASK | BODYCON_BOOSTER3_MASK | BODYCON_BOOSTER4_MASK | BODYCON_BOOSTER5_MASK) )
		{
			if( IsRobotArmor() && (m_bKeyDownA || m_bKeyDownD) )
				return;
		}

		// end 2009. 07. 07 by ckPark ·Îşż±âľî żäĂ»»çÇ×(·Ń¸µ, Ľ±ĹĂČ­¸é, ą«±â, A±âľîĆ÷´ë)
		
		// şÎ˝şĹÍ°ˇ ÄŃÁř °ćżě
		if(	COMPARE_BODYCON_BIT(hyBodyCondition,BODYCON_BOOSTER3_MASK) )
		{
			if(!COMPARE_BODYCON_BIT(GetCurrentBodyCondition(),BODYCON_BOOSTER3_MASK))
			{
				// 2008-01-16 by bhsohn A±âľî żöÇÁ ˝ÇĆĐ˝Ă Ŕ§Ŕĺ,±×¶óżîµĺ ż˘Ľż ĂëĽŇ ˝ĂĹ´
				//m_bIsAir = TRUE;
				SetShuttleAirMode(TRUE);
				// end 2008-01-16 by bhsohn A±âľî żöÇÁ ˝ÇĆĐ˝Ă Ŕ§Ŕĺ,±×¶óżîµĺ ż˘Ľż ĂëĽŇ ˝ĂĹ´

				if(COMPARE_BODYCON_BIT(GetCurrentBodyCondition(),BODYCON_LANDED_MASK))
				{
					m_nBooster = BOOSTER_TYPE_MIN;
					ChangeSingleBodyCondition(BODYCON_BOOSTER1_MASK);
				}
				else
				{
					m_nBooster = BOOSTER_TYPE_MAX;
					ChangeSingleBodyCondition(BODYCON_BOOSTER3_MASK);
					// mouse rate Á¶Ŕý
					ITEM* pItemInfo = g_pStoreData->GetItemInfoFromItemGeneral(g_pStoreData->FindItemInInventoryByWindowPos( POS_REAR ));
					if(pItemInfo)
					{
//						m_fMouseRate = pItemInfo->BoosterAngle;
						m_fMouseRate = CAtumSJ::GetEngineBoosterRangeAngle(pItemInfo, &m_paramFactor);
					}
					// ˝şĹ©¸° ŔĚĆĺĆ®
					if(g_pD3dApp->m_pEffectList)
					{
						CAppEffectData* pEffect = g_pScene->FindEffect( RC_EFF_BOOSTER );
						if(pEffect == NULL)
						{
							//pEffect = new CAppEffectData(g_pCamera,RC_EFF_BOOSTER,100);
							pEffect = new CAppEffectData(g_pCamera, RC_EFF_BOOSTER, 10, 0, 0, 5);//by Inet 29-02-2020 - fix for booster effect on high fov
							g_pD3dApp->m_pEffectList->AddChild(pEffect);
						}
					}
					if((IS_BT(g_pShuttleChild->m_myShuttleInfo.UnitKind) || IS_ST(g_pShuttleChild->m_myShuttleInfo.UnitKind)) && g_pD3dApp->m_pSound)
					{
//						g_pD3dApp->m_pSound->StopD3DSound( SOUND_FLYING_B_I_GEAR );
						g_pD3dApp->m_pSound->PlayD3DSound( SOUND_HIGH_BOOSTER_START, m_vPos, FALSE);
						g_pD3dApp->m_pSound->PlayD3DSound( SOUND_HIGH_BOOSTER, m_vPos, FALSE);
					}
					else if(g_pD3dApp->m_pSound)
					{
//						g_pD3dApp->m_pSound->StopD3DSound( SOUND_FLYING_M_A_GEAR );
						g_pD3dApp->m_pSound->PlayD3DSound( SOUND_LOW_BOOSTER_START, m_vPos, FALSE);
						g_pD3dApp->m_pSound->PlayD3DSound( SOUND_LOW_BOOSTER, m_vPos, FALSE);
					}
				
				}
			}
			return;
		}
		// şÎ˝şĹÍ°ˇ ˛¨Ář °ćżě
		else if( COMPARE_BODYCON_BIT(GetCurrentBodyCondition(),BODYCON_BOOSTER3_MASK) && 
			!COMPARE_BODYCON_BIT(hyBodyCondition,BODYCON_BOOSTER3_MASK))
		{
			ChangeSingleBodyCondition(BODYCON_BOOSTER4_MASK);
//			DBGOUT("CShuttleChild::ChangeBodyConditionFromServer(BODYCON_BOOSTER4_MASK)\n");
			return;
		}
		else
		{
			ChangeUnitBodyCondition(hyBodyCondition);
//			DBGOUT("CShuttleChild::ChangeBodyConditionFromServer(%016I64X)\n",hyBodyCondition);
		}
	}
}


///////////////////////////////////////////////////////////////////////////////
/// \fn			CShuttleChild::CheckBodyConditionToServer(BodyCond_t hyBodyCondition)
/// \brief		ąŮµđÄÁµđĽÇŔ» şĐ·ůČÄ ÇĎłŞľżĽ­ąö·Î ş¸ł˝´Ů.
///				ÇĘżä˝Ăżˇ´Â ÇĎłŞľż Ăß°ˇ ąŮ¶÷
/// \author		ispark
/// \date		2005-07-28 ~ 2005-07-28
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CShuttleChild::CheckBodyConditionToServer(BodyCond_t hyBodyCondition)
{
	if(COMPARE_BODYCON_BIT(hyBodyCondition, BODYCON_DAMAGE1_MASK))
	{
		g_pD3dApp->SendFieldSocketChangeCharacterBodyCondition( BODYCON_DAMAGE1_MASK, TRUE );
	}
	else
	{
		g_pD3dApp->SendFieldSocketChangeCharacterBodyCondition( BODYCON_DAMAGE1_MASK, FALSE );

	}
	if(COMPARE_BODYCON_BIT(hyBodyCondition, BODYCON_DAMAGE2_MASK))
	{
		g_pD3dApp->SendFieldSocketChangeCharacterBodyCondition( BODYCON_DAMAGE2_MASK, TRUE );
	}	
	else
	{
		g_pD3dApp->SendFieldSocketChangeCharacterBodyCondition( BODYCON_DAMAGE2_MASK, FALSE );

	}
	if(COMPARE_BODYCON_BIT(hyBodyCondition, BODYCON_DAMAGE3_MASK))
	{
		g_pD3dApp->SendFieldSocketChangeCharacterBodyCondition( BODYCON_DAMAGE3_MASK, TRUE );
	}
	else
	{
		g_pD3dApp->SendFieldSocketChangeCharacterBodyCondition( BODYCON_DAMAGE3_MASK, FALSE );

	}
}



// ŔŻ´ÖŔÇ ąŮµđÄÁµđĽÇŔĚ ŔŻÁöµÉ¶§, ľÖ´Ď¸ŢŔĚĽÇ ˝Ă°ŁŔ» °ˇÁ®żČ
#define BODYCON_CLIENT_KEEPING_ANIMATION_MASK		(BODYCON_BOOSTER_EX_STATE_CLEAR_MASK | BODYCON_LANDING_MASK | BODYCON_LANDED_MASK | BODYCON_TAKEOFF_MASK)
void CShuttleChild::CheckAniTime()
{
	FLOG( "CShuttleChild::CheckAniTime()" );

	m_fCurrentTime = GetCurrentAnimationTime();
//	sprintf( g_pD3dApp->m_strDebug, "0x%016I64x %.3f",GetCurrentBodyCondition(), m_fCurrentTime );
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			void CShuttleChild::SendFieldSocketChangeBodyCondition(ClientIndex_t nClientIndex, BodyCond_t hyBody)
/// \brief		MSG_FC_CHARACTER_CHANGE_BODYCONDITION
/// \author		dhkwon
/// \date		2004-03-28 ~ 2004-03-28
/// \warning	APPżˇµµ °°Ŕş ±â´ÉŔ» ÇĎ´Â ÇÔĽö°ˇ ŔÖ´Ů.Âü°íÇŇ°Í.
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CShuttleChild::SendFieldSocketChangeBodyCondition(ClientIndex_t nClientIndex, BodyCond_t hyBody)
{
	FLOG("CShuttleChild::SendFieldSocketChangeBodyCondition(ClientIndex_t nClientIndex, BodyCond_t hyBody)");
//	DBGOUT("CShuttleChild::SendFieldSocketChangeBodyCondition(%016I64X)\n",hyBody);
	MSG_FC_CHARACTER_CHANGE_BODYCONDITION sMsg;
	sMsg.ClientIndex = nClientIndex;
	sMsg.BodyCondition = hyBody;
	g_pD3dApp->m_pFieldWinSocket->SendMsg( T_FC_CHARACTER_CHANGE_BODYCONDITION, (char*)&sMsg, sizeof(sMsg));
}
void CShuttleChild::CheckBoosterAni()
{
	FLOG( "CShuttleChild::CheckBoosterAni()" );
	switch(m_nBooster)
	{
	case BOOSTER_TYPE_AIRBREAK:// żŞĂßÁř şÎ˝şĹÍ
		{
			if(!COMPARE_BODYCON_BIT(GetCurrentBodyCondition(),BODYCON_BOOSTER5_MASK))//żŞĂßÁř
			{
				SendFieldSocketChangeBodyCondition(m_myShuttleInfo.ClientIndex, 
						BODYCON_BOOSTER5_MASK | BODYCON_SET_OR_CLEAR_MASK);
				//SET_BODYCON_BIT(m_myShuttleInfo.BodyCondition,BODYCON_BOOSTER5_MASK);//żŞĂßÁř
				ChangeSingleBodyCondition(BODYCON_BOOSTER5_MASK);
			}
//			m_fAttackDelayCheckTime = 1.0f;
		}
		break;
	case BOOSTER_TYPE_STOP://ÁßÁö
		{
			if(COMPARE_BODYCON_BIT(GetCurrentBodyCondition(),BODYCON_BOOSTER3_MASK))//°íĽÓ şÎ˝şĹÍ
			{
				//SET_BODYCON_BIT(m_myShuttleInfo.BodyCondition,BODYCON_BOOSTER4_MASK);//°íĽÓŔ» ˛ř¶§ 
				SendFieldSocketChangeBodyCondition(m_myShuttleInfo.ClientIndex, 
						BODYCON_BOOSTER4_MASK | BODYCON_SET_OR_CLEAR_MASK);
				ChangeSingleBodyCondition(BODYCON_BOOSTER4_MASK);
				// 2006-12-11 by dgwoo SĹ° ´©¸Ą »óĹÂżÍ µż˝Ăżˇ şÎ˝şĹÍ»çżë˝Ă ŔŰµżľČµĘ
 				//m_bUseBooster = FALSE;
			}
			else
			{
				if(COMPARE_BODYCON_BIT(GetCurrentBodyCondition(),BODYCON_BOOSTER4_MASK))//°íĽÓŔ» ˛ř¶§
				{
					if(m_pCharacterInfo)
					{
						map<BodyCond_t,CBodyConditionInfo*>::iterator it = m_pCharacterInfo->m_mapBodyCondition.find(BODYCON_BOOSTER4_MASK);
						if(it != m_pCharacterInfo->m_mapBodyCondition.end())
						{
							if((it->second)->m_fCurrentAnimationTime >= (it->second)->m_fEndAnimationTime)
							{
								SendFieldSocketChangeBodyCondition(m_myShuttleInfo.ClientIndex, 
										BODYCON_BOOSTER1_MASK | BODYCON_SET_OR_CLEAR_MASK);
								//SET_BODYCON_BIT(m_myShuttleInfo.BodyCondition,BODYCON_BOOSTER1_MASK);//Ĺ°¸¦ ľČ´©¸Ł°í şńÇŕ(Ćň±ŐĽÓµµ ŔĚÇĎ, ĽÓµµÁő°ˇĹ° ľČ´©¸¦¶§)
								ChangeSingleBodyCondition(BODYCON_BOOSTER1_MASK);
							}
						}
					}
				}
				else
				{
					if(!COMPARE_BODYCON_BIT(GetCurrentBodyCondition(),BODYCON_BOOSTER1_MASK))//Ĺ°¸¦ ľČ´©¸Ł°í şńÇŕ(Ćň±ŐĽÓµµ ŔĚÇĎ, ĽÓµµÁő°ˇĹ° ľČ´©¸¦¶§)
					{
//						DBGOUT("CheckBoosterAni(%016I64X) SendFieldSocketChangeBodyCondition(BOOSTER_TYPE_STOP)\n",GetCurrentBodyCondition());
						SendFieldSocketChangeBodyCondition(m_myShuttleInfo.ClientIndex, 
								BODYCON_BOOSTER1_MASK | BODYCON_SET_OR_CLEAR_MASK);
						//SET_BODYCON_BIT(m_myShuttleInfo.BodyCondition,BODYCON_BOOSTER1_MASK);
						ChangeSingleBodyCondition(BODYCON_BOOSTER1_MASK);
//						DBGOUT("CheckBoosterAni(%016I64X) SendFieldSocketChangeBodyCondition(BOOSTER_TYPE_STOP)\n",GetCurrentBodyCondition());
					}
				}
			}
		}
		break;
	case BOOSTER_TYPE_MIN://±âş»şÎ˝şĹÍ 
		{
			if(COMPARE_BODYCON_BIT(GetCurrentBodyCondition(),BODYCON_BOOSTER3_MASK))//°íĽÓ şÎ˝şĹÍ
			{
				//SET_BODYCON_BIT(m_myShuttleInfo.BodyCondition,BODYCON_BOOSTER4_MASK);//°íĽÓŔ» ˛ř¶§
				ChangeSingleBodyCondition(BODYCON_BOOSTER4_MASK);
			}
			else
			{
				if(COMPARE_BODYCON_BIT(GetCurrentBodyCondition(),BODYCON_BOOSTER4_MASK))//°íĽÓŔ» ˛ř¶§
				{
					if(m_pCharacterInfo)
					{
						map<BodyCond_t,CBodyConditionInfo*>::iterator it = m_pCharacterInfo->m_mapBodyCondition.find(BODYCON_BOOSTER4_MASK);
						if(it != m_pCharacterInfo->m_mapBodyCondition.end())
						{
							if((it->second)->m_fCurrentAnimationTime >= (it->second)->m_fEndAnimationTime)
							{
								SendFieldSocketChangeBodyCondition(m_myShuttleInfo.ClientIndex, 
										BODYCON_BOOSTER2_MASK | BODYCON_SET_OR_CLEAR_MASK);
								//SET_BODYCON_BIT(m_myShuttleInfo.BodyCondition,BODYCON_BOOSTER2_MASK);
								ChangeSingleBodyCondition(BODYCON_BOOSTER2_MASK);
							}
						}
					}
				}
				else
				{
					if(!COMPARE_BODYCON_BIT(GetCurrentBodyCondition(),BODYCON_BOOSTER2_MASK))// ŔüÁřĹ° ´©¸Ł´Â »óĹÂ(Ćň±ŐĽÓµµ ŔĚ»ó, ĽÓµµÁő°ˇĹ° ´©¸¦¶§)
					{
						SendFieldSocketChangeBodyCondition(m_myShuttleInfo.ClientIndex, 
								BODYCON_BOOSTER2_MASK | BODYCON_SET_OR_CLEAR_MASK);
						//SET_BODYCON_BIT(m_myShuttleInfo.BodyCondition,BODYCON_BOOSTER2_MASK);
						ChangeSingleBodyCondition(BODYCON_BOOSTER2_MASK);
					}
				}
			}
		}
		break;
	case BOOSTER_TYPE_MAX://°ˇĽÓşÎ˝şĹÍ
		{
			if(!COMPARE_BODYCON_BIT(GetCurrentBodyCondition(),BODYCON_BOOSTER3_MASK))//°íĽÓ şÎ˝şĹÍ
			{
				m_nBooster = BOOSTER_TYPE_MIN;
				if(m_fChangeBodyForBoosterCheckTime < 0.0f)
				{
					m_fChangeBodyForBoosterCheckTime = 0.2f;
					// Ľ­ąö ŔüĽŰČÄ OK ¸¦ ąŢľĆľßÁö¸¸ şÎ˝şĹÍ »óĹÂ°ˇ ÄŃÁö°Ô µČ´Ů. ÇöŔç´Â şÎ˝şĹÍ »çżëŔüŔĚ´Ů.
					SendFieldSocketChangeBodyCondition(m_myShuttleInfo.ClientIndex, 
							BODYCON_BOOSTER3_MASK | BODYCON_SET_OR_CLEAR_MASK);
				}
			}
		}
		break;
	}
}

void CShuttleChild::CheckMouseDir()
{
	FLOG( "CShuttleChild::CheckMouseDir()" );

	GetCursorPos(&m_pt);
	ScreenToClient(g_pD3dApp->GetHwnd(), &m_pt);
	CheckMouseReverse(&m_pt);

	D3DXVECTOR3 v;
	D3DXMATRIX matProj,matView;
	matProj = g_pD3dApp->m_pCamera->GetProjMatrix();

	// Compute the vector of the pick ray in screen space
	v.x =  (((2.0f * m_pt.x) / g_pD3dApp->m_d3dsdBackBuffer.Width) - 1) / matProj._11;
	v.y = -(((2.0f * m_pt.y) / g_pD3dApp->m_d3dsdBackBuffer.Height) - 1) / matProj._22;
	v.z =  1.0f;

	// Get the inverse view matrix
	D3DXMATRIX  matTemp;
	matView = g_pD3dApp->m_pCamera->GetViewMatrix();
	D3DXMatrixInverse(&matTemp, NULL, &matView);

	// Transform the screen space pick ray into 3D space
	m_vMouseDir.x  = v.x*matTemp._11 + v.y*matTemp._21 + v.z*matTemp._31;
	m_vMouseDir.y  = v.x*matTemp._12 + v.y*matTemp._22 + v.z*matTemp._32;
	m_vMouseDir.z  = v.x*matTemp._13 + v.y*matTemp._23 + v.z*matTemp._33;
	D3DXVec3Normalize(&m_vMouseDir,&m_vMouseDir);
	if(m_bTurnCamera)
		m_vMouseDir *= -1.0f;

	// 2010-06-15 by shcho&hslee Ćę˝Ă˝şĹŰ - ¸¶żě˝ş Ŕ§Äˇ
	m_vMousePos = D3DXVECTOR3(matTemp._41, matTemp._42, matTemp._43);
	// end 2010-06-15 by shcho&hslee Ćę˝Ă˝şĹŰ - ¸¶żě˝ş Ŕ§Äˇ
	// 2005-09-05 by ispark
	// ¸¶żě˝ş ąćÇâ
	int iCenterX = g_pD3dApp->GetBackBufferDesc().Width / 2;
	int iCenterY = g_pD3dApp->GetBackBufferDesc().Height / 2;
	
	// 2010. 03. 18 by ckPark ŔÎÇÇ´ĎĆĽ ÇĘµĺ 2Â÷(¸ó˝şĹÍ ˝şĹł Ăß°ˇ)

// 	m_ptDir.x = m_pt.x - iCenterX;
// 	m_ptDir.y = m_pt.y - iCenterY;

	if( !m_pSkill->IsExistDesParamSKill( DES_SKILL_REVERSECONTROL ) && g_pShuttleChild->GetSkillMissileWarning() == FALSE)
	{
		m_ptDir.x = m_pt.x - iCenterX;
		m_ptDir.y = m_pt.y - iCenterY;

		m_bAGearFollowEnemy = FALSE; //reverse skill and silence breaks AG targeting
	}
	else{
		m_ptDir.x = -(m_pt.x - iCenterX);
		m_ptDir.y = -(m_pt.y - iCenterY);
	}

	// end 2010. 03. 18 by ckPark ŔÎÇÇ´ĎĆĽ ÇĘµĺ 2Â÷(¸ó˝şĹÍ ˝şĹł Ăß°ˇ)
}

void CShuttleChild::CheckTarget()
{
	// 2005-08-16 by ispark
	// ·ąŔĚ´ő ąĚŔĺÂř˝Ă Ĺ¸°Ů °Ë»ç ľČÇŃ´Ů.
	if(m_pRadarItemInfo == NULL || IsObserverMode())
	{
		return;
	}

	if(g_pD3dApp->m_dwTargetting == NEW_TARGETTING)
	{
		NewCheckTarget();
	}
	else if(g_pD3dApp->m_dwTargetting == OLD2_TARGETTING)
	{
		Old2CheckTarget();
	}
	else
	{// OLD_TARGETTING
		OldCheckTarget();
	}
}

void CShuttleChild::OldCheckTarget()
{
	// 2005-03-24 by jschoi - ±âÁ¸ŔÇ Ĺ¸°ŮĆĂ ąć˝Ä
	FLOG( "CShuttleChild::CheckTarget()" );
	if(m_pSkill->GetSkillTargetState() == TRUE)
	{
		g_pInterface->m_pTarget->SetMouseType(MOUSE_TYPE_11);
		return;
	}

	D3DXVECTOR2 v1, v2, v3;
	v1 = D3DXVECTOR2(m_pt.x,m_pt.y);
	v3 = D3DXVECTOR2(g_pD3dApp->GetBackBufferDesc().Width/2,g_pD3dApp->GetBackBufferDesc().Height/2);

	// 2006-02-15 by ispark
	if(//g_pGameMain->IsShowing() ||					// 2006-11-07 by ispark
		g_pGameMain->m_bMenuLock || 
		g_pGameMain->m_bChangeMousePoint == TRUE || 
		g_pD3dApp->m_dwGameState == _SHOP || 
		m_bUnitStop == TRUE ||
		m_dwState == _LANDED)
	{
		g_pInterface->m_pTarget->SetMouseType(MOUSE_TYPE_0);
	}
	else if(m_pPrimaryWeapon)
	{
		float fLength;//, fLength2;

		GetCursorPos(&m_pt);
		ScreenToClient(g_pD3dApp->GetHwnd(), &m_pt);
		CheckMouseReverse(&m_pt);

		m_pTarget = NULL;

		// 2011. 03. 08 by jskim ŔÎÇÇ3Â÷ ±¸Çö - łÍ Ĺ¸°Ů ˝Ă˝şĹŰ
		//vector<INT>::iterator it = m_pPrimaryWeapon->m_vecTargetIndex.begin();
		//while(it != m_pPrimaryWeapon->m_vecTargetIndex.end())
		vector<TARGET_DATA>::iterator it = m_pPrimaryWeapon->m_vecTargetIndexData.begin();
		while(it != m_pPrimaryWeapon->m_vecTargetIndexData.end())
		// end 2011. 03. 08 by jskim ŔÎÇÇ3Â÷ ±¸Çö - łÍ Ĺ¸°Ů ˝Ă˝şĹŰ
		{
			// 2011. 03. 08 by jskim ŔÎÇÇ3Â÷ ±¸Çö - łÍ Ĺ¸°Ů ˝Ă˝şĹŰ
			//CUnitData* pUnit = g_pScene->FindUnitDataByClientIndex( (*it) );
			CUnitData* pUnit = g_pScene->FindUnitDataByClientIndex( (*it).nTargetIndex );
			// end 2011. 03. 08 by jskim ŔÎÇÇ3Â÷ ±¸Çö - łÍ Ĺ¸°Ů ˝Ă˝şĹŰ
			ASSERT_ASSERT(pUnit);
			if(pUnit)
			{
				v2 = D3DXVECTOR2(pUnit->m_nObjScreenX,pUnit->m_nObjScreenY);
				if(IS_DT(m_myShuttleInfo.UnitKind))	// A±âľîŔÎ °ćżě
				{
					if( D3DXVec2Length(&(v1 - v2)) < SHUTTLE_TARGET_MOUSE_CIRCLE_RADIUS*((float)g_pD3dApp->GetBackBufferDesc().Width/(float)800) )
					{
						m_pTarget = pUnit;
//						if(m_bLButtonState)
//						{// °ř°ÝÁß(Ĺ¬¸ŻµĘ)
//							g_pInterface->m_pTarget->SetMouseType(MOUSE_TYPE_4);
//						}
//						else
//						{// °ř°ÝÇĎÁö ľĘŔ˝(łë Ĺ¬¸Ż)
//							g_pInterface->m_pTarget->SetMouseType(MOUSE_TYPE_2);
//						}
//						break;
					}
					else
					{
//						if(m_bLButtonState)
//						{// °ř°ÝÁß(Ĺ¬¸ŻµĘ)
//							g_pInterface->m_pTarget->SetMouseType(MOUSE_TYPE_5);
//						}
//						else
//						{// °ř°ÝÇĎÁö ľĘŔ˝(łë Ĺ¬¸Ż)
//							g_pInterface->m_pTarget->SetMouseType(MOUSE_TYPE_3);
//						}
//						break;
					}
				}
				else if(D3DXVec2Length(&(v1 - v3)) < SHUTTLE_ATTACK_CENTER_RADIUS*((float)g_pD3dApp->GetBackBufferDesc().Width/(float)800))// ¸¶żě˝ş Ć÷ŔÎĹÍ°ˇ Č­¸Á ľČżˇ ŔÖ´Â °ćżě
				{
					fLength = D3DXVec2Length(&(v1-v2));
					if( fLength < SHUTTLE_TARGET_MOUSE_CIRCLE_RADIUS*((float)g_pD3dApp->GetBackBufferDesc().Width/(float)800))
					{
						m_pTarget = pUnit;
						if(m_bLButtonState)
						{// °ř°ÝÁß(Ĺ¬¸ŻµĘ)
							g_pInterface->m_pTarget->SetMouseType(MOUSE_TYPE_4);
						}
						else
						{// °ř°ÝÇĎÁö ľĘŔ˝(łë Ĺ¬¸Ż)
							g_pInterface->m_pTarget->SetMouseType(MOUSE_TYPE_2);
						}
						break;
					}
					else
					{
						if(m_bLButtonState)
						{// °ř°ÝÁß(Ĺ¬¸ŻµĘ)
							g_pInterface->m_pTarget->SetMouseType(MOUSE_TYPE_5);
						}
						else
						{// °ř°ÝÇĎÁö ľĘŔ˝(łë Ĺ¬¸Ż)
							g_pInterface->m_pTarget->SetMouseType(MOUSE_TYPE_3);
						}
						break;
					}
				}
				else
				{
					g_pInterface->m_pTarget->SetMouseType(MOUSE_TYPE_1);
					if(m_pOrderTarget == NULL || m_pOrderTarget == pUnit)
					{
						m_pTarget = pUnit;
						break;
					}
					
				}
			}
			it++;
		} 
        // 2011. 03. 08 by jskim ŔÎÇÇ3Â÷ ±¸Çö - łÍ Ĺ¸°Ů ˝Ă˝şĹŰ
		//if(it == m_pPrimaryWeapon->m_vecTargetIndex.end())
		if(it == m_pPrimaryWeapon->m_vecTargetIndexData.end())
        // end 2011. 03. 08 by jskim ŔÎÇÇ3Â÷ ±¸Çö - łÍ Ĺ¸°Ů ˝Ă˝şĹŰ
		{
			if( IS_DT(m_myShuttleInfo.UnitKind) ||	// A±âľîŔÎ °ćżě
				D3DXVec2Length(&(v1 - v3)) < SHUTTLE_ATTACK_CENTER_RADIUS*((float)g_pD3dApp->GetBackBufferDesc().Width/(float)800))// ¸¶żě˝ş Ć÷ŔÎĹÍ°ˇ Č­¸Á ľČżˇ ŔÖ´Â °ćżě
			{
				if(m_bLButtonState)
				{// °ř°ÝÁß(Ĺ¬¸ŻµĘ)
					g_pInterface->m_pTarget->SetMouseType(MOUSE_TYPE_5);
				}
				else
				{// °ř°ÝÇĎÁö ľĘŔ˝(łë Ĺ¬¸Ż)
					g_pInterface->m_pTarget->SetMouseType(MOUSE_TYPE_3);
				}
			}
			else
			{
				g_pInterface->m_pTarget->SetMouseType(MOUSE_TYPE_1);
			}
		}
		if( m_pTarget && m_pOrderTarget == NULL )
		{
			// 2005-02-15 by jschoi - żŔ´ő Ĺ¸°ŮŔ» Ŕâ´Â şÎşĐ
			m_pOrderTarget = m_pTarget;

			// 2005-10-19 by ispark
			// Enemy Ĺ¸°Ůżˇ ŔÖ´ÂÁö °Ë»ç
			vector<PK_TARGET *>::iterator itEnemy = m_vecPKTarget.begin();
			while(itEnemy != m_vecPKTarget.end())
			{
				CEnemyData* pTarget = (CEnemyData*)g_pScene->FindUnitDataByClientIndex( (*itEnemy)->nTargetIndex );
				if(m_pOrderTarget == pTarget)
				{
					// ŔÖŔ˝
					return;
				}
				itEnemy++;
			}

			if(	m_pTarget->m_dwPartType == _ENEMY &&						// 1. Ĺ¸°ŮŔĚ EnemyŔĚ°í,
//				((CEnemyData*)m_pTarget)->GetPkState() == PK_NORMAL &&		// 2. ĽřĽö °­Á¦ Ĺ¸°ŮŔĚ¸é // 2006-01-07 by ispark, »čÁ¦
				g_pShuttleChild->m_pPkNormalTimer->IsCityWar() == FALSE)	// 3. µµ˝Ă Áˇ·ÉŔüÁßżˇ´Â ş¸ł»Áö ľĘ´Â´Ů.
			{
				// Ľ­ąö·Î ş¸ł˝´Ů.
				MSG_FC_MOVE_LOCKON sMsg;
				sMsg.AttackIndex = g_pShuttleChild->m_myShuttleInfo.ClientIndex;
				sMsg.TargetIndex = ((CEnemyData*)m_pTarget)->m_infoCharacter.CharacterInfo.ClientIndex;	
				g_pFieldWinSocket->SendMsg( T_FC_MOVE_LOCKON, (char*)&sMsg, sizeof(sMsg) );				
			}
			if(g_pD3dApp->m_pSound)
				g_pD3dApp->m_pSound->PlayD3DSound(SOUND_LOCKON_PRIMARY, g_pShuttleChild->m_vPos, FALSE);
		}
	}
	else
	{
		// 1Çü ą«±â¸¦ ŔĺÂřÇĎÁö ľĘŔş °ćżě
		if( IS_DT(m_myShuttleInfo.UnitKind) ||	// A±âľîŔÎ °ćżě
			D3DXVec2Length(&(v1 - v3)) < SHUTTLE_ATTACK_CENTER_RADIUS*((float)g_pD3dApp->GetBackBufferDesc().Width/(float)800))// ¸¶żě˝ş Ć÷ŔÎĹÍ°ˇ Č­¸Á ľČżˇ ŔÖ´Â °ćżě
		{
			if(m_bLButtonState)
			{// °ř°ÝÁß(Ĺ¬¸ŻµĘ)
				g_pInterface->m_pTarget->SetMouseType(MOUSE_TYPE_5);
			}
			else
			{// °ř°ÝÇĎÁö ľĘŔ˝(łë Ĺ¬¸Ż)
				g_pInterface->m_pTarget->SetMouseType(MOUSE_TYPE_3);
			}
		}
		else
		{
			g_pInterface->m_pTarget->SetMouseType(MOUSE_TYPE_1);
		}
	}
}

void CShuttleChild::Old2CheckTarget()
{
	// 2005-03-29 by jschoi - ±âÁ¸ Ĺ¸°Ů ąć˝Ä şŻ°ć
	FLOG( "CShuttleChild::CheckTarget()" );
	if(m_pSkill->GetSkillTargetState() == TRUE)
	{
		g_pInterface->m_pTarget->SetMouseType(MOUSE_TYPE_11);
		return;
	}

	D3DXVECTOR2 v1, v2, v3;
	v1 = D3DXVECTOR2(m_pt.x,m_pt.y);
	v3 = D3DXVECTOR2(g_pD3dApp->GetBackBufferDesc().Width/2,g_pD3dApp->GetBackBufferDesc().Height/2);

	// 2006-02-15 by ispark
	if(//g_pGameMain->IsShowing() ||						// 2006-11-07 by ispark
		g_pGameMain->m_bMenuLock || 
		g_pGameMain->m_bChangeMousePoint == TRUE || 
		g_pD3dApp->m_dwGameState == _SHOP || 
		m_bUnitStop == TRUE ||
		m_dwState == _LANDED)
	{
		g_pInterface->m_pTarget->SetMouseType(MOUSE_TYPE_0);
	}
	else if(m_pPrimaryWeapon)
	{
		float fLength;//, fLength2;

		GetCursorPos(&m_pt);
		ScreenToClient(g_pD3dApp->GetHwnd(), &m_pt);
		CheckMouseReverse(&m_pt);

		m_pTarget = NULL;

		// 2011. 03. 08 by jskim ŔÎÇÇ3Â÷ ±¸Çö - łÍ Ĺ¸°Ů ˝Ă˝şĹŰ
// 		vector<int>::iterator it = m_pPrimaryWeapon->m_vecTargetIndex.begin();
// 		while(it != m_pPrimaryWeapon->m_vecTargetIndex.end())
		vector<TARGET_DATA>::iterator it = m_pPrimaryWeapon->m_vecTargetIndexData.begin();
		while(it != m_pPrimaryWeapon->m_vecTargetIndexData.end())		
		// end011. 03. 08 by jskim ŔÎÇÇ3Â÷ ±¸Çö - łÍ Ĺ¸°Ů ˝Ă˝şĹŰ
		{
			// 2011. 03. 08 by jskim ŔÎÇÇ3Â÷ ±¸Çö - łÍ Ĺ¸°Ů ˝Ă˝şĹŰ
			//CUnitData* pUnit = g_pScene->FindUnitDataByClientIndex( *it );
			CUnitData* pUnit = g_pScene->FindUnitDataByClientIndex( (*it).nTargetIndex );
			// 2011. 03. 08 by jskim ŔÎÇÇ3Â÷ ±¸Çö - łÍ Ĺ¸°Ů ˝Ă˝şĹŰ
			ASSERT_ASSERT(pUnit);
			if(pUnit)
			{
				v2 = D3DXVECTOR2(pUnit->m_nObjScreenX,pUnit->m_nObjScreenY);
				if(IS_DT(m_myShuttleInfo.UnitKind) && m_bIsAir == FALSE)	// A±âľîŔÎ °ćżě
				{// 2007-07-27 by dgwoo A±âľîŔÇ °ćżě ¶ĄżˇĽ­ µű·Î Ăł¸®ÇŘÁŘ´Ů.

					if( D3DXVec2Length(&(v1 - v2)) < SHUTTLE_TARGET_MOUSE_CIRCLE_RADIUS*((float)g_pD3dApp->GetBackBufferDesc().Width/(float)800) )
					{
						m_pTarget = pUnit;
						float fDistance = D3DXVec3Length(&(m_vPos - m_pTarget->m_vPos));
						if(	(GetTargetClientIndext() == GetTargetToClientIdx(m_pTarget))
							&& g_pShuttleChild->m_pRadarItemInfo && CAtumSJ::GetPrimaryRadarRange(g_pShuttleChild->m_pRadarItemInfo->ItemInfo, &g_pShuttleChild->m_paramFactor) > fDistance)
							//&& (CAtumSJ::GetAttackRange(m_pPrimaryWeapon->GetRealItemInfo(), &m_paramFactor ) > fDistance))
						{
							if(m_bLButtonState)
							{
								g_pInterface->m_pTarget->SetMouseType(MOUSE_TYPE_4);
							}
							else
							{
								g_pInterface->m_pTarget->SetMouseType(MOUSE_TYPE_2);
							}
						}
						else
						{
							if(m_bLButtonState)
							{
								g_pInterface->m_pTarget->SetMouseType(MOUSE_TYPE_5);
							}
							else
							{
								g_pInterface->m_pTarget->SetMouseType(MOUSE_TYPE_3);
							}
						}
						break;
					}
					else
					{
						g_pInterface->m_pTarget->SetMouseType(MOUSE_TYPE_3);
					}
				}
				// 2005-03-29 by jschoi - °ˇżîµĄ·Î Ĺ¸°ŮŔĎ ¶§ ż©±â şÎĹÍ ÁÖĽ®
				else if(D3DXVec2Length(&(v1 - v3)) < SHUTTLE_ATTACK_CENTER_RADIUS*((float)g_pD3dApp->GetBackBufferDesc().Width/(float)800))// ¸¶żě˝ş Ć÷ŔÎĹÍ°ˇ Č­¸Á ľČżˇ ŔÖ´Â °ćżě
				{
					fLength = D3DXVec2Length(&(v1-v2));
					if( fLength < SHUTTLE_TARGET_MOUSE_CIRCLE_RADIUS*((float)g_pD3dApp->GetBackBufferDesc().Width/(float)800) ||
						D3DXVec2Length(&(v1 - v3)) < SHUTTLE_TARGET_MOUSE_CIRCLE_RADIUS*((float)g_pD3dApp->GetBackBufferDesc().Width/(float)800))
					{
 						m_pTarget = pUnit;
						if(m_bLButtonState)
						{// °ř°ÝÁß(Ĺ¬¸ŻµĘ) °ř°ÝÁß 1Çü Ĺ¸°Ů şŻ°ć
							// ¸¶żě˝ş ľ÷ąöĆ° ŔĚ¸é Ĺ¸°Ů Ă¤ŔÎÁö
							// 2006-01-19 by ispark
							if(TRUE == m_bTargetChange && m_pOrderTarget == NULL)
							{
								// 2009. 11. 02 by ckPark ŔÎÇÇ´ĎĆĽ ÇĘµĺ ŔÎ˝şĹĎ˝ş ´řÁŻ ˝Ă˝şĹŰ
								// BELL_INFINITY_DEFENSE_MONSTER ŔÎ ¸ó˝şĹÍ´Â Ĺ¸°ŮŔĚ ľČŔâČ÷µµ·Ď ÇŃ´Ů
								// BODYCON_CREATION_MASK ŔÎ ¸ó˝şĹÍ´Â Ĺ¸°ŮŔĚ ľČŔâČ÷µµ·Ď ÇŃ´Ů
								if( m_pTarget->m_dwPartType == _MONSTER )
								{
									// 2010. 03. 24 by ckPark ¸®Á¨ÁßŔÎ ¸ó˝şĹÍÁß 10ĂĘ »ýĽş ľÖ´Ď¸ŢŔĚĽÇŔĚ ŔÖ´Â ¸ó˝şĹÍ¸¸ Ĺ¸°Ů ľČŔâČ÷µµ·Ď şŻ°ć
// 									if( ((CMonsterData*)(m_pTarget))->m_pMonsterInfo->Belligerence == BELL_INFINITY_DEFENSE_MONSTER
// 										|| COMPARE_BODYCON_BIT( ((CMonsterData*)(m_pTarget))->GetCurrentBodyCondition(), BODYCON_CREATION_MASK ) )
									if( ((CMonsterData*)(m_pTarget))->m_pMonsterInfo->Belligerence == BELL_INFINITY_DEFENSE_MONSTER
										||
										( COMPARE_MPOPTION_BIT(((CMonsterData*)(m_pTarget))->m_pMonsterInfo->MPOption, MPOPTION_BIT_PATTERN_MONSTER )
										&& COMPARE_BODYCON_BIT(((CMonsterData*)(m_pTarget))->GetCurrentBodyCondition(), BODYCON_CREATION_MASK) ) )
									// end 2010. 03. 24 by ckPark ¸®Á¨ÁßŔÎ ¸ó˝şĹÍÁß 10ĂĘ »ýĽş ľÖ´Ď¸ŢŔĚĽÇŔĚ ŔÖ´Â ¸ó˝şĹÍ¸¸ Ĺ¸°Ů ľČŔâČ÷µµ·Ď şŻ°ć
									{
										break;
									}
								}
								// end 2009. 11. 02 by ckPark ŔÎÇÇ´ĎĆĽ ÇĘµĺ ŔÎ˝şĹĎ˝ş ´řÁŻ ˝Ă˝şĹŰ


								m_pOrderTarget = m_pTarget;
								g_pInterface->m_pTarget->SetMouseType(MOUSE_TYPE_4);
							}
						}
						else
						{// °ř°ÝÇĎÁö ľĘŔ˝(łë Ĺ¬¸Ż)
							g_pInterface->m_pTarget->SetMouseType(g_pInterface->m_pTarget->m_nTargetColor);
						}
						break;
					}
					else
					{

						break;
					}
				}
				else
				{
					g_pInterface->m_pTarget->SetMouseType(MOUSE_TYPE_1);
					if(m_pOrderTarget == NULL || m_pOrderTarget == pUnit)
					{
						m_pTarget = pUnit;
						break;
					}
					
				}
			}
			it++;
		}

		// 2005-03-29 by jschoi - °ˇżîµĄ·Î Ĺ¸°ŮŔĎ ¶§ ż©±â şÎĹÍ ÁÖĽ®
		// 2011. 03. 08 by jskim ŔÎÇÇ3Â÷ ±¸Çö - łÍ Ĺ¸°Ů ˝Ă˝şĹŰ
		//if(it == m_pPrimaryWeapon->m_vecTargetIndex.end())
		if(it == m_pPrimaryWeapon->m_vecTargetIndexData.end())
		// end 2011. 03. 08 by jskim ŔÎÇÇ3Â÷ ±¸Çö - łÍ Ĺ¸°Ů ˝Ă˝şĹŰ
		{
			if( (IS_DT(m_myShuttleInfo.UnitKind) && m_bIsAir == FALSE) ||	// A±âľîŔÎ °ćżě
				D3DXVec2Length(&(v1 - v3)) < SHUTTLE_ATTACK_CENTER_RADIUS*((float)g_pD3dApp->GetBackBufferDesc().Width/(float)800))// ¸¶żě˝ş Ć÷ŔÎĹÍ°ˇ Č­¸Á ľČżˇ ŔÖ´Â °ćżě
			{
				if(m_bLButtonState)
				{// °ř°ÝÁß(Ĺ¬¸ŻµĘ)
					g_pInterface->m_pTarget->SetMouseType(MOUSE_TYPE_5);
				}
				else
				{// °ř°ÝÇĎÁö ľĘŔ˝(łë Ĺ¬¸Ż)
					g_pInterface->m_pTarget->SetMouseType(g_pInterface->m_pTarget->m_nTargetColor);
				}
			}
			else
			{
				g_pInterface->m_pTarget->SetMouseType(MOUSE_TYPE_1);
			}
		}

		// 2007-05-17 by bhsohn żŔşęÁ§Ć® µÚżˇ ĽűľúŔ»˝Ă żˇ ´ëÇŃ Ăł °Ë»ç Ăł¸®
		// if( m_pTarget && m_pOrderTarget == NULL )
		BOOL bTargetColl = TRUE;
		if(NULL == m_pOrderTarget )
		{
			bTargetColl = IsCheckObjectColl(m_pTarget);
		}
		
		if( (FALSE == bTargetColl )&& m_pTarget && m_pOrderTarget == NULL )
		{
			// 2009. 11. 02 by ckPark ŔÎÇÇ´ĎĆĽ ÇĘµĺ ŔÎ˝şĹĎ˝ş ´řÁŻ ˝Ă˝şĹŰ
			// BELL_INFINITY_DEFENSE_MONSTER ŔÎ ¸ó˝şĹÍ´Â Ĺ¸°ŮŔĚ ľČŔâČ÷µµ·Ď ÇŃ´Ů
			// BODYCON_CREATION_MASK ŔÎ ¸ó˝şĹÍ´Â Ĺ¸°ŮŔĚ ľČŔâČ÷µµ·Ď ÇŃ´Ů
			if( m_pTarget->m_dwPartType == _MONSTER )
			{
				// 2010. 03. 24 by ckPark ¸®Á¨ÁßŔÎ ¸ó˝şĹÍÁß 10ĂĘ »ýĽş ľÖ´Ď¸ŢŔĚĽÇŔĚ ŔÖ´Â ¸ó˝şĹÍ¸¸ Ĺ¸°Ů ľČŔâČ÷µµ·Ď şŻ°ć
// 				if( ((CMonsterData*)(m_pTarget))->m_pMonsterInfo->Belligerence == BELL_INFINITY_DEFENSE_MONSTER
// 					|| COMPARE_BODYCON_BIT( ((CMonsterData*)(m_pTarget))->GetCurrentBodyCondition(), BODYCON_CREATION_MASK ) )
				if( ((CMonsterData*)(m_pTarget))->m_pMonsterInfo->Belligerence == BELL_INFINITY_DEFENSE_MONSTER
					||
					( COMPARE_MPOPTION_BIT(((CMonsterData*)(m_pTarget))->m_pMonsterInfo->MPOption, MPOPTION_BIT_PATTERN_MONSTER )
					&& COMPARE_BODYCON_BIT(((CMonsterData*)(m_pTarget))->GetCurrentBodyCondition(), BODYCON_CREATION_MASK) ) )
				// end 2010. 03. 24 by ckPark ¸®Á¨ÁßŔÎ ¸ó˝şĹÍÁß 10ĂĘ »ýĽş ľÖ´Ď¸ŢŔĚĽÇŔĚ ŔÖ´Â ¸ó˝şĹÍ¸¸ Ĺ¸°Ů ľČŔâČ÷µµ·Ď şŻ°ć
				{
					return;
				}
			}
			// end 2009. 11. 02 by ckPark ŔÎÇÇ´ĎĆĽ ÇĘµĺ ŔÎ˝şĹĎ˝ş ´řÁŻ ˝Ă˝şĹŰ

			// 2005-02-15 by jschoi - żŔ´ő Ĺ¸°ŮŔ» Ŕâ´Â şÎşĐ
			m_pOrderTarget = m_pTarget;

			// 2005-10-19 by ispark
			// Enemy Ĺ¸°Ůżˇ ŔÖ´ÂÁö °Ë»ç
			vector<PK_TARGET *>::iterator itEnemy = m_vecPKTarget.begin();
			while(itEnemy != m_vecPKTarget.end())
			{
				CEnemyData* pTarget = (CEnemyData*)g_pScene->FindUnitDataByClientIndex( (*itEnemy)->nTargetIndex );
				if(m_pOrderTarget == pTarget)
				{
					// ŔÖŔ˝
					return;
				}
				itEnemy++;
			}

			if(	m_pTarget->m_dwPartType == _ENEMY &&						// 1. Ĺ¸°ŮŔĚ EnemyŔĚ°í,
//				((CEnemyData*)m_pTarget)->GetPkState() == PK_NORMAL &&		// 2. ĽřĽö °­Á¦ Ĺ¸°ŮŔĚ¸é // 2006-01-07 by ispark, »čÁ¦
				g_pShuttleChild->m_pPkNormalTimer->IsCityWar() == FALSE)	// 3. µµ˝Ă Áˇ·ÉŔüÁßżˇ´Â ş¸ł»Áö ľĘ´Â´Ů.
			{
				// Ľ­ąö·Î ş¸ł˝´Ů.
				MSG_FC_MOVE_LOCKON sMsg;
				sMsg.AttackIndex = g_pShuttleChild->m_myShuttleInfo.ClientIndex;
				sMsg.TargetIndex = ((CEnemyData*)m_pTarget)->m_infoCharacter.CharacterInfo.ClientIndex;	
				g_pFieldWinSocket->SendMsg( T_FC_MOVE_LOCKON, (char*)&sMsg, sizeof(sMsg) );				
			}
			if(g_pD3dApp->m_pSound)
				g_pD3dApp->m_pSound->PlayD3DSound(SOUND_LOCKON_PRIMARY, g_pShuttleChild->m_vPos, FALSE);
		}
	}
	else
	{
		// 1Çü ą«±â¸¦ ŔĺÂřÇĎÁö ľĘŔş °ćżě
		if( (IS_DT(m_myShuttleInfo.UnitKind) && m_bIsAir == FALSE) ||	// A±âľîŔÎ °ćżě
			D3DXVec2Length(&(v1 - v3)) < SHUTTLE_ATTACK_CENTER_RADIUS*((float)g_pD3dApp->GetBackBufferDesc().Width/(float)800))// ¸¶żě˝ş Ć÷ŔÎĹÍ°ˇ Č­¸Á ľČżˇ ŔÖ´Â °ćżě
		{
			if(m_bLButtonState)
			{// °ř°ÝÁß(Ĺ¬¸ŻµĘ)
				g_pInterface->m_pTarget->SetMouseType(MOUSE_TYPE_4);
			}
			else
			{// °ř°ÝÇĎÁö ľĘŔ˝(łë Ĺ¬¸Ż)
				g_pInterface->m_pTarget->SetMouseType(MOUSE_TYPE_3);
			}
		}
		else
		{
			g_pInterface->m_pTarget->SetMouseType(MOUSE_TYPE_1);
		}
	}
}

void CShuttleChild::NewCheckTarget()
{
	// 2005-03-24 by jschoi - »ő·Îżî Ĺ¸°ŮĆĂ ąć˝Ä

	// Ĺ¸°ŮĆĂ Ľ±ĹĂ(żĄ±âľî ĽÓĽş˝şĹł)
	if(m_pSkill->GetSkillTargetState() == TRUE)
	{
		g_pInterface->m_pTarget->SetMouseType(MOUSE_TYPE_11);
		return;
	}

	// ŔÎĹÍĆäŔĚ˝ş ¸đµĺ
	// 2006-02-15 by ispark
	if(//g_pGameMain->IsShowing() ||							// 2006-11-07 by ispark
		g_pGameMain->m_bMenuLock || 
		g_pGameMain->m_bChangeMousePoint == TRUE || 
		g_pD3dApp->m_dwGameState == _SHOP || 
		m_bUnitStop == TRUE ||
		m_dwState == _LANDED)
	{
		g_pInterface->m_pTarget->SetMouseType(MOUSE_TYPE_0);
		return;
	}

	if(m_pPrimaryWeapon)
	{
		GetCursorPos(&m_pt);
		ScreenToClient(g_pD3dApp->GetHwnd(), &m_pt);
		CheckMouseReverse(&m_pt);

		m_pTarget = NULL;

		if(m_bLButtonState)
		{// °ř°ÝÁß(Ĺ¬¸ŻµĘ)
			g_pInterface->m_pTarget->SetMouseType(MOUSE_TYPE_5);
		}
		else
		{// °ř°ÝÇĎÁö ľĘŔ˝(łë Ĺ¬¸Ż)
			g_pInterface->m_pTarget->SetMouseType(MOUSE_TYPE_3);
		}

		// 2011. 03. 08 by jskim ŔÎÇÇ3Â÷ ±¸Çö - łÍ Ĺ¸°Ů ˝Ă˝şĹŰ
// 		vector<int>::iterator it = m_pPrimaryWeapon->m_vecTargetIndex.begin();
// 		DBGOUT(" m_vecTargetIndex : %d\n",m_pPrimaryWeapon->m_vecTargetIndex.size());
// 		while(it != m_pPrimaryWeapon->m_vecTargetIndex.end())
		vector<TARGET_DATA>::iterator it = m_pPrimaryWeapon->m_vecTargetIndexData.begin();
		DBGOUT(" m_vecTargetIndexData : %d\n",m_pPrimaryWeapon->m_vecTargetIndexData.size());
		while(it != m_pPrimaryWeapon->m_vecTargetIndexData.end())
		// end 2011. 03. 08 by jskim ŔÎÇÇ3Â÷ ±¸Çö - łÍ Ĺ¸°Ů ˝Ă˝şĹŰ
		{
			// 2011. 03. 08 by jskim ŔÎÇÇ3Â÷ ±¸Çö - łÍ Ĺ¸°Ů ˝Ă˝şĹŰ
			//CUnitData* pUnit = g_pScene->FindUnitDataByClientIndex( (*it) );
			CUnitData* pUnit = g_pScene->FindUnitDataByClientIndex( (*it).nTargetIndex );
			// end 2011. 03. 08 by jskim ŔÎÇÇ3Â÷ ±¸Çö - łÍ Ĺ¸°Ů ˝Ă˝şĹŰ
			ASSERT_ASSERT(pUnit);
			if(pUnit)
			{
				D3DXVECTOR2 v1, v2, v3;
				v1 = D3DXVECTOR2(m_pt.x,m_pt.y);
				v2 = D3DXVECTOR2(pUnit->m_nObjScreenX,pUnit->m_nObjScreenY);
				v3 = D3DXVECTOR2(g_pD3dApp->GetBackBufferDesc().Width/2,g_pD3dApp->GetBackBufferDesc().Height/2);

				if( D3DXVec2Length(&(v1 - v2)) < SHUTTLE_TARGET_MOUSE_CIRCLE_RADIUS*((float)g_pD3dApp->GetBackBufferDesc().Width/(float)800) ||
					CheckPickingTarget(pUnit))
				{
					m_pTarget = pUnit;
					if(m_bLButtonState)
					{// °ř°ÝÁß(Ĺ¬¸ŻµĘ)
						g_pInterface->m_pTarget->SetMouseType(MOUSE_TYPE_4);
					}
					else
					{// °ř°ÝÇĎÁö ľĘŔ˝(łë Ĺ¬¸Ż)
						g_pInterface->m_pTarget->SetMouseType(MOUSE_TYPE_2);
					}
					break;
				}
			}
			it++;
		}

		if( m_pTarget && m_pOrderTarget == NULL )
		{
			// 2005-02-15 by jschoi - żŔ´ő Ĺ¸°ŮŔ» Ŕâ´Â şÎşĐ
 			m_pOrderTarget = m_pTarget;

			// 2005-10-19 by ispark
			// Enemy Ĺ¸°Ůżˇ ŔÖ´ÂÁö °Ë»ç
			vector<PK_TARGET *>::iterator itEnemy = m_vecPKTarget.begin();
			while(itEnemy != m_vecPKTarget.end())
			{
				CEnemyData* pTarget = (CEnemyData*)g_pScene->FindUnitDataByClientIndex( (*itEnemy)->nTargetIndex );
				if(m_pOrderTarget == pTarget)
				{
					// ŔÖŔ˝
					return;
				}
				itEnemy++;
			}
			
			if(	m_pTarget->m_dwPartType == _ENEMY &&						// 1. Ĺ¸°ŮŔĚ EnemyŔĚ°í,
//				((CEnemyData*)m_pTarget)->GetPkState() == PK_NORMAL &&		// 2. ĽřĽö °­Á¦ Ĺ¸°ŮŔĚ¸é // 2006-01-07 by ispark, »čÁ¦
				g_pShuttleChild->m_pPkNormalTimer->IsCityWar() == FALSE)	// 3. µµ˝Ă Áˇ·ÉŔüÁßżˇ´Â ş¸ł»Áö ľĘ´Â´Ů.
			{
				// Ľ­ąö·Î ş¸ł˝´Ů.
				MSG_FC_MOVE_LOCKON sMsg;
				sMsg.AttackIndex = g_pShuttleChild->m_myShuttleInfo.ClientIndex;
				sMsg.TargetIndex = ((CEnemyData*)m_pTarget)->m_infoCharacter.CharacterInfo.ClientIndex;	
				g_pFieldWinSocket->SendMsg( T_FC_MOVE_LOCKON, (char*)&sMsg, sizeof(sMsg) );				
			}
			if(g_pD3dApp->m_pSound)
				g_pD3dApp->m_pSound->PlayD3DSound(SOUND_LOCKON_PRIMARY, g_pShuttleChild->m_vPos, FALSE);
		}
	}
	else
	{
		// 1Çü ą«±â¸¦ ŔĺÂřÇĎÁö ľĘŔş °ćżě
		g_pInterface->m_pTarget->SetMouseType(MOUSE_TYPE_1);
	}
}

void CShuttleChild::CheckMonsterMove(CMonsterData * pMon)
{
	FLOG( "CShuttleChild::CheckMonsterMove(CMonsterData * pMon)" );
	float fElapsedTime = g_pD3dApp->GetElapsedTime();
	D3DXVECTOR3 vVel;
	D3DXVECTOR3 vUp = D3DXVECTOR3(0,1,0);
	D3DXVECTOR3 vSide = D3DXVECTOR3(0,0,1);
	int nSpeed;
	MSG_FC_MONSTER_MOVE_OK info;
	memset(&info,0x00,sizeof(MSG_FC_MONSTER_MOVE_OK));
	info.MonsterIndex = pMon->m_info.MonsterIndex;
	switch(pMon->m_ResInfo.bMonType)
	{
	case 0:
		{// żňÁ÷ŔÓ ľřŔ˝
			info.PositionVector = pMon->m_vPos;
		}
		break;
	case 1:
		{// »óÇĎ żňÁ÷ŔÓ
//			if(pMon->m_vPos.y >= g_pGround->m_projectInfo.fHeightMax - 20.0f
//				|| pMon->m_vPos.y <= 20.0f)
			if(pMon->m_vPos.y >= 250.0f)
			{
				pMon->m_nUpDir = -1;
			}
			else if(pMon->m_vPos.y <= 80.0f)
			{
				pMon->m_nUpDir = 1;
			}
			nSpeed = rand()%200 + 70;
			info.PositionVector = pMon->m_vPos + pMon->m_nUpDir*nSpeed*vUp*fElapsedTime;
		}
		break;
	case 2:
		{// ÁÂżě żňÁ÷ŔÓ
//			if(pMon->m_vPos.x <= 2670.0f || pMon->m_vPos.x >= 3750.0f)// || pMon->m_vPos.z <= 77.0f|| pMon->m_vPos.z >= 957.0f)
			if(pMon->m_vPos.z <= 77.0f)// || pMon->m_vPos.z <= 77.0f|| pMon->m_vPos.z >= 957.0f)
			{
				pMon->m_nSideDir = 1;
			}
			else if(pMon->m_vPos.z >= 957.0f)
			{
				pMon->m_nSideDir = -1;
			}
			nSpeed = rand()%200 + 100;
			info.PositionVector = pMon->m_vPos + pMon->m_nSideDir*nSpeed*vSide*fElapsedTime;
		}
		break;
	case 3:
		{// »óÇĎ ÁÂżě żňÁ÷ŔÓ
//			if(pMon->m_vPos.x <= 2670.0f || pMon->m_vPos.x >= 3750.0f)// || pMon->m_vPos.z <= 77.0f|| pMon->m_vPos.z >= 957.0f)
			if(pMon->m_vPos.z <= 77.0f)// || pMon->m_vPos.z <= 77.0f|| pMon->m_vPos.z >= 957.0f)
			{
				pMon->m_nSideDir = 1;
			}
			else if(pMon->m_vPos.z >= 957.0f)
			{
				pMon->m_nSideDir = -1;
			}
			nSpeed = rand()%200 + 120;
			info.PositionVector = pMon->m_vPos + pMon->m_nSideDir*nSpeed*vSide*fElapsedTime;
//			if(pMon->m_vPos.y >= g_pGround->m_projectInfo.fHeightMax - 20.0f
//				|| pMon->m_vPos.y <= 20.0f)
			if(pMon->m_vPos.y >= 250.0f)
			{
				pMon->m_nUpDir = -1;
			}
			else if(pMon->m_vPos.y <= 80.0f)
			{
				pMon->m_nUpDir = 1;
			}
			nSpeed = rand()%200 + 150;
			info.PositionVector = pMon->m_vPos + pMon->m_nUpDir*nSpeed*vUp*fElapsedTime;
		}
		break;
	case 4:
		{
		}
		break;
	}
	D3DXVec3Normalize(&vVel,&(m_vPos - pMon->m_vPos));
	info.TargetVector = vVel;
	pMon->m_vVel = vVel;
	pMon->SetMonPosition(&info);
}

BOOL CShuttleChild::MoveOrder(BYTE bType)
{
	FLOG( "CShuttleChild::MoveOrder(BYTE bType)" );
	float fElapsedTime = g_pD3dApp->GetElapsedTime();
	D3DXVECTOR3 vVel, vOldPos, vSide, vSide2 ;
	D3DXMATRIX mat;
	float fAngle;
	D3DXVECTOR2 vPos1, vPos2, vVel2,vOldPos2;
	
	D3DXVec3Cross(&vSide,&m_vUp,&m_vVel);

	switch(bType) 
	{
	case 0:
		{
			SetShuttleFlightInit(fElapsedTime);
			if(D3DXVec3Length(&(m_vPos - m_vTargetPos)) < 0.5f && ACOS(D3DXVec3Dot(&m_vTargetVel,&m_vVel)) < 0.01f 
				&& vSide.y < 0.01f)
			{// °­Á¦ ŔĚµż ÇŘÁ¦
				int nCheckX = ((int)m_vPos.x)/TILE_SIZE;
				int nCheckZ = ((int)m_vPos.z)/TILE_SIZE;
				if(g_pGround->m_pTileInfo[nCheckX*g_pGround->m_projectInfo.sYSize + nCheckZ].bEnableLand)
				{
					if(SetOrderMoveTargetPos() == TRUE )
					{
						m_fNumpad0keyTime = 2.0f;
						SendPacket(T_FC_MOVE_LANDING);
					}
				}
				return TRUE;
			}

			vPos1.x = m_vPos.x;
			vPos1.y = m_vPos.z;
			vPos2.x = m_vTargetPos.x;
			vPos2.y = m_vTargetPos.z;
			
			vOldPos2 = vPos1;	
			vOldPos = m_vPos;

			if(D3DXVec2Length(&(vPos1 - vPos2)) > 0.5f)
			{// ¸ńŔűÁö ±îÁö °ˇ´Â Áßżˇ °ˇ´Â °÷Ŕ» ąŮ¶óş¸°Ô ÇŃ´Ů.
				D3DXVec2Normalize(&vVel2,&(vPos2 - vPos1));
				vVel.x = vVel2.x;
				vVel.z = vVel2.y;
				vVel.y = m_vVel.y;

				if(m_vTargetPos.y + 20.0f - m_vPos.y < 10.0f)
//					vPos1 += m_fShuttleSpeedMax*vVel2*fElapsedTime;
					vPos1 += m_fRealShuttleSpeedMax*vVel2*fElapsedTime;

				m_vPos.x = vPos1.x;
				m_vPos.z = vPos1.y;
				m_vPos.y += (m_vTargetPos.y + 20.0f - m_vPos.y)*fElapsedTime;
				if(D3DXVec2Length(&(vOldPos2 - vPos2)) < D3DXVec2Length(&(vPos1 - vPos2)))
				{// Ĺ¸°Ů ÁöÁˇŔ» ÁöłŞ°ˇ°Ô µÉ¶§ Ĺ¸°Ů ÁöÁˇŔ¸·Î ŔĚµż˝ĂĹ˛´Ů.
					m_vPos.x = m_vTargetPos.x;
					m_vPos.z = m_vTargetPos.z;
				}

				fAngle = ACOS(D3DXVec3Dot(&m_vVel,&vVel));
				D3DXVec3Cross(&vSide,&m_vVel,&vVel);
				if(fAngle > 0.01f)
				{
					D3DXMatrixRotationAxis(&mat,&vSide,fAngle*fElapsedTime);
				}
				else
				{
					D3DXMatrixRotationAxis(&mat,&vSide,fAngle);
				}
				D3DXVec3TransformCoord(&m_vVel,&m_vVel,&mat);

				D3DXVec3Cross(&vSide,&m_vUp,&m_vVel);
				D3DXVec3Cross(&m_vUp,&m_vVel,&vSide);
				vSide2 = vSide;
				vSide2.y = 0.0f;
				D3DXVec3Normalize(&vSide2, &vSide2);
				fAngle = ACOS(D3DXVec3Dot(&vSide2,&vSide));
				D3DXVec3Cross(&vVel,&vSide,&vSide2);
				if(fAngle > 0.01f)
				{
					D3DXMatrixRotationAxis(&mat,&vVel,fAngle*fElapsedTime);
				}
				else
				{
					D3DXMatrixRotationAxis(&mat,&vVel,fAngle);
				}
				D3DXVec3TransformCoord(&m_vUp,&m_vUp,&mat);
			}
			else
			{// ¸ńŔűÁö ±îÁö µµÂřČÄ ĽłÁ¤ÇŃ ąćÇâŔ¸·Î ş¸°Ô ÇŃ´Ů.
				m_vPos.y += 0.5f*(m_vTargetPos.y - m_vPos.y)*fElapsedTime;
				
				vVel = m_vTargetVel;
				fAngle = ACOS(D3DXVec3Dot(&m_vVel,&vVel));
				D3DXVec3Cross(&vSide,&m_vVel,&vVel);
				if(fAngle > 0.01f)
				{
					D3DXMatrixRotationAxis(&mat,&vSide,fAngle*fElapsedTime);
				}
				else
				{
					D3DXMatrixRotationAxis(&mat,&vSide,fAngle);
				}
				D3DXVec3TransformCoord(&m_vVel,&m_vVel,&mat);

				D3DXVec3Cross(&vSide,&m_vUp,&m_vVel);
				D3DXVec3Cross(&m_vUp,&m_vVel,&vSide);
				vSide2 = vSide;
				vSide2.y = 0.0f;
				D3DXVec3Normalize(&vSide2, &vSide2);
				fAngle = ACOS(D3DXVec3Dot(&vSide2,&vSide));
				D3DXVec3Cross(&vVel,&vSide,&vSide2);
				if(fAngle > 0.01f)
				{
					D3DXMatrixRotationAxis(&mat,&vVel,fAngle*fElapsedTime);
				}
				else
				{
					D3DXMatrixRotationAxis(&mat,&vVel,fAngle);
				}
				D3DXVec3TransformCoord(&m_vUp,&m_vUp,&mat);
			}
		}
		break;
	case 1:// Á÷Ľ± Âř·ú
		{
			if(m_fSideCheckRate > 0.0f)
				m_fSideCheckRate -= 0.5f*fElapsedTime;
			if(m_fSideCheckRate < 0.0f)
				m_fSideCheckRate = 0.0f;
			// ĆňÇü ¸¶Ăß±â
			// 2005-12-07 by ispark, µÚ·Î żňÁ÷ŔĚ°í ŔÖľú´Ů¸é ˝şÇÇµĺ 0Ŕ¸·Î ¸¸µéľîĽ­ Á¤Áö
			if(m_fShuttleSpeed < 0)
				m_fShuttleSpeed = 0.0f;
			SetShuttleFlightInit(fElapsedTime);
//			CheckMoveRate(fElapsedTime,0,-1);
			CheckMoveRate(fElapsedTime,0);
			m_vPos += m_fShuttleSpeed*m_vVel*fElapsedTime;
			if(m_fSinMove < 1.0f)
			{
				m_fSinMove += fElapsedTime;
			}
			m_vPos.y -= 50.0f*fElapsedTime*m_fSinMove;          // Âř·ú˝Ă łôŔĚ °¨ĽŇ·® ĽĽĆĂ
			float fTempHeight = 100.0f;
			COLLISION_RESULT collResult;
			if(m_bOnObject)
			{
				D3DXMATRIX matTemp;
				D3DXVECTOR3 vSide;
				vSide = D3DXVECTOR3(0,0,1);
				D3DXMatrixLookAtLH(&matTemp,&m_vPos,&(m_vPos - m_vBasisUp),&vSide);
//				fTempHeight = g_pScene->m_pObjectRender->CheckCollMesh(matTemp,m_vPos).fDist;
				collResult = g_pScene->m_pObjectRender->CheckCollMesh(matTemp,m_vPos, FALSE);
				if(collResult.fDist != DEFAULT_COLLISION_DISTANCE)
				{
					fTempHeight = m_vPos.y - collResult.fDist;
				}
			}
			float fCheckMapHeight = CheckMap(m_vPos);
			if(collResult.fDist != DEFAULT_COLLISION_DISTANCE)
			{
				if(m_vPos.y <= fTempHeight+m_fAmorHeight)
				{
					m_vPos.y = fTempHeight+m_fAmorHeight;
					// 2007-12-05 by bhsohn ş®żˇ ¶Ő°í µéľî°ˇ´Â Çö»ó Ăł¸®
//					SendPacket(T_FC_MOVE_LANDING_DONE);
//					DBGOUT("T_FC_MOVE_LANDING_DONE\n");
//					ChangeUnitState( _LANDED );
					// ĂćµąŔĚ ŔÖľîłµ´Ů.
					if(FALSE == CheckObjLandingColl(collResult.vNormalVector))
					{
						if(CancelLandingMode())
						{
							return TRUE;
						}						
					}
					SendPacket(T_FC_MOVE_LANDING_DONE);
					DBGOUT("T_FC_MOVE_LANDING_DONE\n");
					ChangeUnitState( _LANDED );
					// 2007-01-02 by bhsohn A±âľî b+Enter·Î ¸ŘĂá ČÄ, Âř·úÇĎ¸é ľČ żňÁ÷ż©Áö´Â Çö»ó Ăł¸®								
					if(IS_DT(m_myShuttleInfo.UnitKind))
					{
						// A±âľî ¶ó¸é żňÁ÷ŔÓ ĂĘ±âČ­
						InitUnitStop();
					}				
					// end 2007-01-02 by bhsohn A±âľî b+Enter·Î ¸ŘĂá ČÄ, Âř·úÇĎ¸é ľČ żňÁ÷ż©Áö´Â Çö»ó Ăł¸®
					
					return TRUE;
				}
			}
			else
			{
				if(m_vPos.y <= fCheckMapHeight+m_fAmorHeight)
				{
					// 2008-03-04 by bhsohn A±âľî°ˇ ľĆ´Ń ±âľî°ˇ ¶Ążˇ Âř·ú˝Ă °­Á¦ Ăâ°Ý˝ĂĹ´
					if((g_pTutorial->IsTutorialMode() == FALSE)
						&& !IS_DT(m_myShuttleInfo.UnitKind))
					{
						if(CancelLandingMode())
						{
							return TRUE;
						}
					}
					// end 2008-03-04 by bhsohn A±âľî°ˇ ľĆ´Ń ±âľî°ˇ ¶Ążˇ Âř·ú˝Ă °­Á¦ Ăâ°Ý˝ĂĹ´
					m_vPos.y = fCheckMapHeight+m_fAmorHeight;
					SendPacket(T_FC_MOVE_LANDING_DONE);
					DBGOUT("T_FC_MOVE_LANDING_DONE\n");
					ChangeUnitState( _LANDED );
					// 2007-01-02 by bhsohn A±âľî b+Enter·Î ¸ŘĂá ČÄ, Âř·úÇĎ¸é ľČ żňÁ÷ż©Áö´Â Çö»ó Ăł¸®								
					if(IS_DT(m_myShuttleInfo.UnitKind))
					{
						// A±âľî ¶ó¸é żňÁ÷ŔÓ ĂĘ±âČ­
						InitUnitStop();
					}				
					// end 2007-01-02 by bhsohn A±âľî b+Enter·Î ¸ŘĂá ČÄ, Âř·úÇĎ¸é ľČ żňÁ÷ż©Áö´Â Çö»ó Ăł¸®

					return TRUE;
				}
			}

//			if(m_vPos.y <= fCheckMapHeight+m_fAmorHeight || m_vPos.y <= fTempHeight+m_fAmorHeight)
//			{
//				if(fCheckMapHeight + m_fAmorHeight > m_vPos.y + m_fAmorHeight - fTempHeight)
//					m_vPos.y = fCheckMapHeight + m_fAmorHeight;
//				else
//					m_vPos.y += m_fAmorHeight - fTempHeight;
//				SendPacket(T_FC_MOVE_LANDING_DONE);
//				ChangeUnitState( _LANDED );
//
//				return TRUE;
//			}
		}
		break;
	case 2:// Č¸Ŕü Âř·ú
		{
			SetShuttleFlightInit(fElapsedTime);
			if(m_fLandingCheckTime > 0.0f)
				m_fLandingCheckTime -= fElapsedTime;
			if(m_fLandingCheckTime > 0.0f)
			{
				vVel = m_vVel;
				vVel.y = 0.0f;
				D3DXVec3Normalize(&vVel,&vVel);
				D3DXMatrixRotationAxis(&mat,&m_vBasisUp,(D3DX_PI/2.0f)*fElapsedTime);
				D3DXVec3TransformCoord(&vVel,&vVel,&mat);
				D3DXVec3TransformCoord(&m_vVel,&m_vVel,&mat);
				D3DXVec3TransformCoord(&m_vUp,&m_vUp,&mat);
//				if(m_fShuttleSpeed > (m_fLandingCheckTime/4.0f)*100)
//					m_vPos += (m_fLandingCheckTime/4.0f)*100*vVel*fElapsedTime;
//				else
//					m_vPos += m_fShuttleSpeed*vVel*fElapsedTime;
				m_vPos += 100.0f*vVel*fElapsedTime;
				if(m_vPos.y > m_vTargetPos.y)
				{
					m_vPos.y -= (m_fLandingDist/4.0f)*fElapsedTime;
					if(m_vPos.y < m_vTargetPos.y)
					{
						m_vPos.y = m_vTargetPos.y;
						m_fLandingCheckTime = 0.0f;
					}
				}
				fAngle = ACOS(D3DXVec3Dot(&m_vVel,&vVel));
				if(fAngle != 0.0f)
				{
					D3DXVec3Cross(&vSide,&m_vVel,&vVel);
					D3DXMatrixRotationAxis(&mat,&vSide,fAngle*fElapsedTime);
					D3DXVec3TransformCoord(&m_vVel,&m_vVel,&mat);
					D3DXVec3TransformCoord(&m_vUp,&m_vUp,&mat);
//					D3DXVec3Cross(&m_vUp,&m_vVel,&vSide);
				}
			}
			else
			{
//				if(D3DXVec3Length(&(m_vPos - m_vTargetPos)) < 5.0f)// && ACOS(D3DXVec3Dot(&m_vTargetVel,&m_vVel)) < 0.01f 
				if(m_vPos.y <= m_vTargetPos.y + 5.0f)// && ACOS(D3DXVec3Dot(&m_vTargetVel,&m_vVel)) < 0.01f 
//					&& vSide.y < 0.01f)
				{// °­Á¦ ŔĚµż ÇŘÁ¦
					m_vPos.y = m_vTargetPos.y;
					SendPacket(T_FC_MOVE_LANDING_DONE);
					DBGOUT("T_FC_MOVE_LANDING_DONE\n");
					ChangeUnitState( _LANDED );
					//m_dwState = _LANDED;
					m_fShuttleSpeed = 100.0f;
					
					// 2007-01-02 by bhsohn A±âľî b+Enter·Î ¸ŘĂá ČÄ, Âř·úÇĎ¸é ľČ żňÁ÷ż©Áö´Â Çö»ó Ăł¸®								
					if(IS_DT(m_myShuttleInfo.UnitKind))
					{
						// A±âľî ¶ó¸é żňÁ÷ŔÓ ĂĘ±âČ­
						InitUnitStop();
					}				
					// end 2007-01-02 by bhsohn A±âľî b+Enter·Î ¸ŘĂá ČÄ, Âř·úÇĎ¸é ľČ żňÁ÷ż©Áö´Â Çö»ó Ăł¸®
					return TRUE;
				}
				m_vPos += 0.5f*(m_vTargetPos - m_vPos)*fElapsedTime;
				vVel = m_vTargetVel;
				fAngle = ACOS(D3DXVec3Dot(&m_vVel,&vVel));
				D3DXVec3Cross(&vSide,&m_vVel,&vVel);
				if(fAngle > 0.01f)
				{
					D3DXMatrixRotationAxis(&mat,&vSide,fAngle*fElapsedTime);
				}
				else
				{
					D3DXMatrixRotationAxis(&mat,&vSide,fAngle);
				}
				D3DXVec3TransformCoord(&m_vVel,&m_vVel,&mat);

				D3DXVec3Cross(&vSide,&m_vUp,&m_vVel);
				D3DXVec3Cross(&m_vUp,&m_vVel,&vSide);
				vSide2 = vSide;
				vSide2.y = 0.0f;
				D3DXVec3Normalize(&vSide2, &vSide2);
				fAngle = ACOS(D3DXVec3Dot(&vSide2,&vSide));
				D3DXVec3Cross(&vVel,&vSide,&vSide2);
				if(fAngle > 0.01f)
				{
					D3DXMatrixRotationAxis(&mat,&vVel,fAngle*fElapsedTime);
				}
				else
				{
					D3DXMatrixRotationAxis(&mat,&vVel,fAngle);
				}
				D3DXVec3TransformCoord(&m_vUp,&m_vUp,&mat);
			}
		}
		break;
	}
	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			BOOL CShuttleChild::SetOrderMoveTargetPos(BOOL bObjectLanding)
/// \brief		Âř·ú ÁöÁˇ °č»ę
/// \author		dhkwon
/// \date		2004-06-29 ~ 2004-06-29
/// \warning	żŔşęÁ§Ć®°ˇ ŔÖ´Â °ćżě ŔŻ´ÖŔÇ łôŔĚş¸´Ů łôŔ¸¸é Âř·ú ˝ÇĆĐ
///				A±âľî´Â °ćżě bObjectLanding=FALSE
///
///		Ăß°ˇ	Âř·ú ÁöÁˇ °č»ężˇĽ­ ą° żŔşęÁ§Ć®´Â Á¦żÜÇŃ´Ů.	// 2005-02-11 by jschoi	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CShuttleChild::SetOrderMoveTargetPos(BOOL bObjectLanding)
{
	FLOG( "CShuttleChild::SetOrderMoveTargetPos()" );
	if( IsTileMapRenderEnable(g_pShuttleChild->m_myShuttleInfo.MapChannelIndex.MapIndex) == FALSE &&
		m_bOnObject == FALSE )
	{
		return FALSE;
	}
	m_vTargetPos = m_vPos;
	m_vTargetVel = m_vVel;
	m_vTargetVel.y = 0.0f;
	D3DXVec3Normalize(&m_vTargetVel,&m_vTargetVel);
	float fTempHeight1 = -DEFAULT_COLLISION_DISTANCE;
	D3DXVECTOR3 vPos = m_vPos;

	D3DXMATRIX matTemp;
	D3DXVECTOR3 vSide;
	vSide = D3DXVECTOR3(0,0,1);
	D3DXMatrixLookAtLH(&matTemp,&vPos,&(vPos - m_vBasisUp),&vSide);
	COLLISION_RESULT collResult = g_pScene->m_pObjectRender->CheckCollMeshWithOutWaterObject(matTemp,vPos);
	if(collResult.fDist != DEFAULT_COLLISION_DISTANCE)
	{
		fTempHeight1 = m_vPos.y - collResult.fDist;
	}
	else if(bObjectLanding)
	{
		return FALSE;
	}

//		fTempHeight1 = g_pScene->m_pObjectRender->CheckCollMesh(matTemp,vPos).fDist;// object Ăćµą łôŔĚ
//		if(fTempHeight1 > vPos.y)
//		{
//			if(bObjectLanding)
//			{
//				return FALSE;
//			}
//			else
//			{
//				fTempHeight1 = vPos.y;
//			}
//		}
//		else
//		{
//			fTempHeight1 = vPos.y - fTempHeight1;
//		}
	float fTempHeight2 = CheckMap(m_vPos);// ¸Ę Ăćµą łôŔĚ
	if(fTempHeight2 < fTempHeight1)
	{
		if(m_vPos.y > fTempHeight1 + m_fAmorHeight)
		{
			m_vTargetPos.y = fTempHeight1 + m_fAmorHeight;
		}
		else if(m_vPos.y < fTempHeight1 + m_fAmorHeight)
		{
			m_vTargetPos.y = fTempHeight1 + m_fAmorHeight;
		}
	}
	else// if(fTempHeight2 < fTempHeight1)
	{
		if(m_vPos.y > fTempHeight2 + m_fAmorHeight)
		{
			m_vTargetPos.y = fTempHeight2 + m_fAmorHeight;
		}
		else if(m_vPos.y < fTempHeight2 + m_fAmorHeight)
		{
			m_vTargetPos.y = fTempHeight2 + m_fAmorHeight;
		}
	}
//	m_fLandingDist = m_vPos.y - m_vTargetPos.y;
	m_fSinMove = 0.0f;
	return TRUE;
}

void CShuttleChild::SetCursorInit()
{
#ifdef _INET_FIX_MOUSE
	HWND hWnd = GetFocus();
	if (g_pD3dApp->GetHwnd() != hWnd)
		return;
#endif
	FLOG( "CShuttleChild::SetCursorInit()" );
	POINT pt,pt2;
	GetCursorPos(&pt);
	pt2 = pt;
//	CheckMouseReverse(&pt2);
	ScreenToClient(g_pD3dApp->GetHwnd(), &pt);
//	CheckMouseReverse(&pt);
	int nX = pt2.x - pt.x;
	int nY = pt2.y - pt.y;
	nX += g_pD3dApp->GetBackBufferDesc().Width/2;
	nY += g_pD3dApp->GetBackBufferDesc().Height/2;
	SetCursorPos(nX,nY);

	// 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
	m_bSetCursorPos = TRUE;
}

void CShuttleChild::PrimaryBulletReloaded(MSG_FC_BATTLE_PRI_BULLET_RELOADED* pMsg)
{
	if(m_pPrimaryWeapon)
	{
		m_pPrimaryWeapon->BulletReloaded( pMsg );
	}
}

void CShuttleChild::SecondaryBulletReloaded(MSG_FC_BATTLE_SEC_BULLET_RELOADED* pMsg)
{
	if(m_pSecondaryWeapon)
	{
		if(g_pD3dApp->m_pSound)
			g_pD3dApp->m_pSound->PlayD3DSound(SOUND_SEC_BULLET_RELOADED, m_vPos, FALSE);
		m_pSecondaryWeapon->BulletReloaded( (MSG_FC_BATTLE_PRI_BULLET_RELOADED*)pMsg );
	}
}

USHORT CShuttleChild::GetShiftWeaponBodyconditionByUnitKind()
{
	if(IS_BT(m_myShuttleInfo.UnitKind))
		return UNIT_KIND_SHIFT_B_GEAR;
	if(IS_ST(m_myShuttleInfo.UnitKind))
		return UNIT_KIND_SHIFT_I_GEAR;
	if(IS_OT(m_myShuttleInfo.UnitKind))
		return UNIT_KIND_SHIFT_M_GEAR;
	if(IS_DT(m_myShuttleInfo.UnitKind))
		return UNIT_KIND_SHIFT_A_GEAR;
	return UNIT_KIND_SHIFT_DEFAULT;
}


void CShuttleChild::UseSkillMove(float fElapsedTime)
{
	FLOG( "CShuttleChild::UseSkillMove(float fElapsedTime)" );

	int nPatternType = TARGET_OFF;
	m_fAdvanceTime = 10.0f;

	D3DXVECTOR3 vOldPos;

	if(m_pCinemaUnitPattern != NULL)
	{
		nPatternType = m_pCinemaUnitPattern->GetHeader().nPatternType;
		BOOL bSkillStop = m_pCinemaUnitPattern->SkillTick();
		//	m_pCinemaCamera->CameraTick(); 
		
		EVENT_POINT SkillPoint, CameraPoint;
		SkillPoint=m_pCinemaUnitPattern->GetCurrentCinemaPoint();
		vOldPos = m_vPos;							// ŔĚŔü Ŕ§Äˇ
		m_vPos = SkillPoint.vPosition;				// Ŕ§Äˇ
		m_vVel = SkillPoint.vDirection;				// ąćÇâ
		m_vUp = SkillPoint.vUpVector;
		m_vTargetPosition = SkillPoint.vTarget;

		// 2005-12-02 by ispark, ĽÓµµ ŔÔ·Â
		m_fShuttleSpeed = SkillPoint.fVelocity;

		if(m_bIsCameraPattern == TRUE)
		{
			m_pCinemaCamera->SkillTick();
			D3DXVECTOR3 vCameraPos, vCameraVel, vCameraUp;
			CameraPoint=m_pCinemaCamera->GetCurrentCinemaPoint();
			vCameraPos = CameraPoint.vPosition;
			vCameraVel = m_vPos - CameraPoint.vPosition;
			vCameraUp = CameraPoint.vUpVector;
			
			// Ä«¸Ţ¶óŔÇ Ŕ§Äˇ ĽÂĆĂ 2004.06.24 jschoi
			g_pD3dApp->m_pCamera->Tick(vCameraPos,vCameraVel,vCameraUp);
			
		}		
		
		if(bSkillStop)
		{

			if ( g_pShuttleChild->IsShowCinema() )
				g_pInterface->m_pInfinity->SendPacket_SkipEndingCinema( true );

			DeletePattern();
			m_nEventTypeAirPort = AIR_PORT_LANDING_PATTERN_NONE;
			// 2008-06-17 by bhsohn Ćí´ë °ü·Ă Ăł¸®
			// ą«şę ˝şĹłŔĚ łˇłµŔ¸¸é °ú°Ĺ Ćí´ë Á¤ş¸·Î şŻ°ć
			m_pClientParty->RefreshFormationType();
			// end 2008-06-17 by bhsohn Ćí´ë °ü·Ă Ăł¸®
		}
	}

	// Âř·ú ĆĐĹĎ
	BOOL bLANDINGPattern = FALSE;
	switch(m_nEventTypeAirPort)
	{
	case AIR_PORT_LANDING_PATTERN_NONE:
		{
			m_nEventIndex = 0;
		}
		break;
	case AIR_PORT_LANDING_PATTERN_START:
		{
//			m_fShuttleSpeed -= g_pD3dApp->GetElapsedTime()*100;
//			if(m_fShuttleSpeed < AIR_PORT_MIN_SPEED)
//				m_fShuttleSpeed = AIR_PORT_MIN_SPEED;
//			SetShuttleFlightInit(fElapsedTime);
			m_nEventIndex = 0;
			bLANDINGPattern = TRUE;
		}
		break;
	case AIR_PORT_LANDING_PATTERN_END:
		{
			bLANDINGPattern = TRUE;
		}
		break;
	case AIR_PORT_TAKEOFF_PATTERN_START:
		{
			bLANDINGPattern = TRUE;
		}
		break;
	case AIR_PORT_TAKEOFF_PATTERN_END:
		{
			bLANDINGPattern = TRUE;
		}
		break;
	}

	switch(nPatternType)
	{
	case TARGET_ON:	// Ĺ¸ÄĎ ąćÇâŔĚ ŔÖ´Â ĆĐĹĎ
		{
			D3DXMatrixLookAtRH( &m_mMatrix, &m_vPos, &(m_vTargetPosition), &m_vUp);
			D3DXVec3Normalize( &m_vVel , &(m_vTargetPosition - m_vPos) ); 
		}
		break;
	case TARGET_OFF: // Ĺ¸ÄĎ ąćÇâŔĚ ľř´Â ĆĐĹĎ
		{
			D3DXMatrixLookAtLH( &m_mMatrix, &m_vPos, &(m_vPos - m_vVel), &m_vUp);
		}
		break;
	}

	// 2005-11-29 by ispark
	// żŔąöşÎ˝şĹÍ ˝şĹłŔ» »çżë ÁßŔĚ¶ó¸é ĆŻş° ŔĚĆĺĆ®(şÎ˝şĹÍ) ˝ÇÇŕ
	if(m_pSkill->IsSkillOverBooster())
	{
		m_nBooster = BOOSTER_TYPE_MAX;
		CheckBoosterAni();		
	}

	D3DXMatrixInverse( &m_mMatrix, NULL, &m_mMatrix );

	// ÁÂżě łŻ°ł żŁÁř Ŕ§Äˇ ĽłÁ¤
	m_vSideVel.x = m_mMatrix._11;
	m_vSideVel.y = m_mMatrix._12;
	m_vSideVel.z = m_mMatrix._13;

	D3DXVec3TransformCoord(&m_vLWPos, &m_PrimaryAttack.vSidePos, &m_mMatrix);
	m_vLWPos += 3.0f*m_vVel+3.0f*m_vUp; // 3.0f*m_vVel;
	D3DXVECTOR3 vPrimaryRightPos = D3DXVECTOR3(-m_PrimaryAttack.vSidePos.x, m_PrimaryAttack.vSidePos.y, m_PrimaryAttack.vSidePos.z);
	D3DXVec3TransformCoord(&m_vRWPos, &vPrimaryRightPos, &m_mMatrix);
	m_vRWPos += 3.0f*m_vVel+3.0f*m_vUp; // 3.0f*m_vVel;
	D3DXVec3TransformCoord(&m_vLWSecondaryPos, &m_SecondaryAttack.vSidePos, &m_mMatrix);
	D3DXVECTOR3 vSecondaryRightPos = D3DXVECTOR3(-m_SecondaryAttack.vSidePos.x, m_SecondaryAttack.vSidePos.y, m_SecondaryAttack.vSidePos.z);
	D3DXVec3TransformCoord(&m_vRWSecondaryPos, &vSecondaryRightPos, &m_mMatrix);

	// 2010-06-15 by shcho&hslee Ćę˝Ă˝şĹŰ - ą«±â Ŕ§Äˇ
	D3DXVec3TransformCoord(&m_vPetLeftPos, &m_PetAttack.vSidePos, &m_mMatrix);
	
	D3DXVECTOR3 vPetRightPos = D3DXVECTOR3(-m_PetAttack.vSidePos.x,
											m_PetAttack.vSidePos.y, 
											m_PetAttack.vSidePos.z);
	D3DXVec3TransformCoord(&m_vPetRightPos, &vPetRightPos, &m_mMatrix);

	m_vPetLeftVel = m_vVel;
	m_vPetRightVel = m_vVel;
	// end 2010-06-15 by shcho&hslee Ćę˝Ă˝şĹŰ - ą«±â Ŕ§Äˇ
	// ą«±â ąćÇâ(żˇŔĚ ±âľî)

	if(IS_DT(m_myShuttleInfo.UnitKind))
	{
		// 2006-01-16 by ispark, żÖ A±âľî°ˇ ĆĐĹĎ ŔĚµżŔĎ ¶§ Ć÷¸¦ żňÁ÷ŔĚ°Ô ÇßŔ»±î? 
//		if(g_pTutorial->IsTutorialMode() == TRUE)
//		{
			m_vWeaponVel = m_vVel;
//		}
//		else
//		{
//			m_vWeaponVel = m_vMouseDir;
//		}
	}
	
	// ˝şĹ©¸°»óŔÇ ÁÂÇĄ¸¦ ±¸ÇŃ´Ů.
	g_pD3dApp->CalcObjectSourceScreenCoords(m_vPos, g_pD3dApp->GetBackBufferDesc().Width, g_pD3dApp->GetBackBufferDesc().Height, 
		m_nObjScreenX, m_nObjScreenY,m_nObjScreenW);
	// Ä«¸Ţ¶ó·ÎşÎĹÍŔÇ °Ĺ¸®
	m_fDistanceCamera = D3DXVec3Length(&(m_vPos - g_pD3dApp->m_pCamera->GetEyePt()));
	m_pMoveChat->Tick();
	m_pIDChat->Tick();


	// 2004-11-26 by jschoi - Ăćµą Ăł¸®
	// ĆĐĹĎ ŔĚµż Áß Ăćµą Ăł¸®(´Ü, żöÇÁŔÎ, żöÇÁľĆżô ĆĐĹĎŔş ĂćµąĂł¸® »ý·«)
	if(	m_nCurrentPatternNumber != PATTERN_UNIT_WARP_IN &&
		m_nCurrentPatternNumber != PATTERN_UNIT_WARP_OUT &&
		bLANDINGPattern == FALSE)
	{
		// ¸Ę°úŔÇ Ăćµą °Ë»ç-ĽĹĆ˛ŔÇ Áßżä 4şÎşĐŔÇ Ăćµą °Ë»ç¸¦ ÇĎŔÚ ľŐ µÚ ÁÂ żě(4 ±ÍĹüŔĚ ÁÂÇĄ¸¦ ¸ÂĂß´Â ŔŰľ÷µµ ÇŘÁŕľßÇŇ °ÍŔĚ´Ů)
		D3DXVECTOR3 vFront, vBack, vLeft, vRight;
		D3DXVECTOR3 vParticlePos;
		vFront = m_vPos + 8.0f*m_vVel;
		vBack = m_vPos - 3.0f*m_vVel;
		vLeft = m_vPos - 7.0f*m_vSideVel;
		vRight = m_vPos + 7.0f*m_vSideVel;
		int nMapCheckType = 0;
		if(vFront.y < CheckMap(vFront))
		{
			nMapCheckType = 1;
			vParticlePos = vFront;
			if(g_pD3dApp->m_pEffectList)
			{
				CAppEffectData * pData = new CAppEffectData(NULL,RC_EFF_COLL_01,vParticlePos);
				pData = (CAppEffectData *)g_pD3dApp->m_pEffectList->AddChild(pData);
			}
		}
		else if(vBack.y < CheckMap(vBack))
		{
			nMapCheckType = 2;
			vParticlePos = vBack;
			if(g_pD3dApp->m_pEffectList)
			{
				CAppEffectData * pData = new CAppEffectData(NULL,RC_EFF_COLL_01,vParticlePos);
				pData = (CAppEffectData *)g_pD3dApp->m_pEffectList->AddChild(pData);
			}
		}
		else if(vLeft.y < CheckMap(vLeft))
		{
			nMapCheckType = 3;
			vParticlePos = vLeft;
			if(g_pD3dApp->m_pEffectList)
			{
				CAppEffectData * pData = new CAppEffectData(NULL,RC_EFF_COLL_01,vParticlePos);
				pData = (CAppEffectData *)g_pD3dApp->m_pEffectList->AddChild(pData);
			}
		}
		else if(vRight.y < CheckMap(vRight))
		{
			nMapCheckType = 4;
			vParticlePos = vRight;
			if(g_pD3dApp->m_pEffectList)
			{
				CAppEffectData * pData = new CAppEffectData(NULL,RC_EFF_COLL_01,vParticlePos);
				pData = (CAppEffectData *)g_pD3dApp->m_pEffectList->AddChild(pData);
			}
		}
		if(nMapCheckType)
		{
			DeletePattern();
			m_vPos = vOldPos;			// Ăćµą ÇßŔ¸ąÇ·Î ŔĚŔü Ŕ§Äˇ·Î
			m_bKeyBlock = FALSE;
		}

		int nCollType = COLL_NONE;	
		D3DXMATRIX matArray[6];
		D3DXVECTOR3 vSide,vVel;
		D3DXVec3Cross(&vSide,&m_vUp,&m_vVel);
		D3DXMatrixLookAtLH(&matArray[COLL_FRONT],&m_vPos,&(m_vPos + m_vVel),&m_vUp);	// ľŐ
		D3DXMatrixLookAtLH(&matArray[COLL_BACK],&m_vPos,&(m_vPos - m_vVel),&m_vUp);	// µÚ
		D3DXMatrixLookAtLH(&matArray[COLL_LEFT],&m_vPos,&(m_vPos - vSide),&m_vUp);		// ÁÂ
		D3DXMatrixLookAtLH(&matArray[COLL_RIGHT],&m_vPos,&(m_vPos + vSide),&m_vUp);		// żě
		D3DXMatrixLookAtLH(&matArray[COLL_UP],&m_vPos,&(m_vPos + m_vUp),&vSide);		// Ŕ§
		D3DXMatrixLookAtLH(&matArray[COLL_DOWN],&m_vPos,&(m_vPos - m_vUp),&vSide);		// ľĆ·ˇ
		float fMoveDistance = D3DXVec3Length(&(m_vPos - vOldPos));
		float size[6] = { 10+fMoveDistance,10+fMoveDistance,10+fMoveDistance,10+fMoveDistance,5+fMoveDistance,5+fMoveDistance };	// ŔŻ´Ö Ĺ©±â	ľŐ,µÚ,ÁÂ,żě,Ŕ§,ľĆ·ˇ	
		nCollType = g_pScene->m_pObjectRender->CheckCollMesh(matArray,size).nCollType;
		if(nCollType != COLL_NONE)
		{
			// Ăćµą Çß´Ů
			DeletePattern();
			m_vPos = vOldPos;			// Ăćµą ÇßŔ¸ąÇ·Î ŔĚŔü Ŕ§Äˇ·Î
			m_bKeyBlock = FALSE;
		}
	}

	if(	m_nCurrentPatternNumber == PATTERN_UNIT_WARP_OUT )
	{
		if(m_fWarpOutDistance < D3DXVec3Length(&(m_vPos - m_vWarpOutPosition)) )
		{
			// żöÇÁ Áľ·á ÇŘľßÇŇ Ŕ§Äˇ¸¦ ÁöłŞĂĆ´Ů.
			m_fWarpOutDistance = 0.0f;
			DeletePattern();
			m_bKeyBlock = FALSE;
		}
	}

	// ¸ĘŔ» ąţľîłŞ¸é ¸řłŞ°ˇ°Ô ĽÂĆĂ ĂĽĹ©
	if(m_vPos.x < 0.0f || m_vPos.x > g_pGround->m_projectInfo.sXSize*TILE_SIZE || 
		m_vPos.z < 0.0f || m_vPos.z > g_pGround->m_projectInfo.sYSize*TILE_SIZE)
	{
		DeletePattern();
		m_vPos = vOldPos;			// ŔĚŔü Ŕ§Äˇ·Î 
	}

	// ¸ř°ˇ´Â ÁöżŞĂĽĹ©ÇĎż© »Ş˝ĂĹ˛´Ů.
	int nXCheck, nYCheck;
	nXCheck = (int)(m_vPos.x / TILE_SIZE);
	nYCheck = (int)(m_vPos.z / TILE_SIZE);

	if (nXCheck < 0)
		nXCheck = 0;

	if (nYCheck < 0)
		nYCheck = 0;


	m_bMoveGroundType = g_pGround->m_pTileInfo[nXCheck*g_pGround->m_projectInfo.sXSize + nYCheck].bMove;
	if(m_bMoveGroundType == 2)
	{
		DeletePattern();
		m_vPos = vOldPos;			// ŔĚŔü Ŕ§Äˇ·Î 
	}

#ifdef  _OLD_SHADOW_SYSTEM
	// ±×¸˛ŔÚ Ŕ§Äˇ ĽłÁ¤
//	CheckShadowPos();
#endif

	if(m_pCharacterInfo)
	{
		m_pCharacterInfo->SetAllBodyConditionMatrix( m_mMatrix );
		m_pCharacterInfo->Tick(fElapsedTime);
	}

}



void CShuttleChild::SetTargetVectorforLandedMove(void)
{
	D3DXVECTOR3 temp;
	m_vPos.x = (int(m_vPos.x/TILE_SIZE))*TILE_SIZE + TILE_SIZE/2;							// ŔÚ˝ĹŔÇ Áß˝ÉÁˇ Ŕ§Äˇ °č»ę
	m_vPos.z = (int(m_vPos.z/TILE_SIZE))*TILE_SIZE + TILE_SIZE/2;

	temp = m_vPos + m_vVel * TILE_SIZE;
	temp.x = (int(temp.x/TILE_SIZE))*TILE_SIZE + TILE_SIZE/2;								// ŔÚ˝ĹŔÇ Áß˝ÉÁˇ Ŕ§Äˇ °č»ę
	temp.z = (int(temp.z/TILE_SIZE))*TILE_SIZE + TILE_SIZE/2;
	
	D3DXVec3Normalize(&m_vVel, &(temp - m_vPos));
}

void CShuttleChild::SetUnitMovingAnimation(float fElapsedTime)
{
	//	if(m_fAnimationMoveTimer < 0)
	//	{
	//m_fAnimationMoveTimer = ANIMATION_UNIT_TIMING;		
		
	FLOAT		Yaw;	
	
	if(m_fAniFlag == ANI_UPDOWN_FLAG)														/// ŔüČÄÁř ÁÂÇĄ°č»ę 
	{	
	
		D3DXVec3Lerp( &m_vPos, &m_vMyOldPos, &m_vAniFrame, m_fFrameLerp );						

		// 2005-10-26 by ispark
		// LANDED żŔşęÁ§Ć® °Ë»ç
		CheckCollForObject2(fElapsedTime);
		
//		m_fFrameLerp += 0.05f;  //0.05	
		m_fFrameLerp += fElapsedTime;  // 2004-07-21 by dhkwon

		if(m_fFrameLerp >= 1.0f )
		{ 				
			m_fAniFlag   = 0;
			m_fFrameLerp = 0;
			m_bAniKeyBlock = FALSE;					
			SetTargetVectorforLandedMove();			
		}
	}
	else if(m_fAniFlag == ANI_LEFT_FLAG || m_fAniFlag == ANI_RIGHT_FLAG )					/// ÁÂżě Č¸Ŕü ÁÂÇĄ°č»ę
	{		
		if(m_fAniFlag == ANI_LEFT_FLAG)
		{
			Yaw	= D3DX_PI * -5.0f / 90.0f;									
		}
		else if(m_fAniFlag == ANI_RIGHT_FLAG)
		{
			Yaw = D3DX_PI * 5.0f / 90.0f;	
		}

		m_fAnimationMoveTimer -= fElapsedTime;
		
		if(m_fAnimationMoveTimer < 0) 
		{
			m_fAnimationMoveTimer = ANIMATION_UNIT_TIMING;
			m_fFrameLerp += 0.1f;
//			m_fFrameLerp += fElapsedTime;  // 2004-07-21 by dhkwon
			
			D3DXMATRIX mat;
			D3DXQUATERNION	g_aniRot[2];											/// Č¸Ŕü(quaternion)Ĺ° °Ş
			
			FLOAT Pitch	= 0;
			FLOAT Roll	= 0;
			D3DXQuaternionRotationYawPitchRoll( &g_aniRot[0], Yaw, Pitch, Roll );	/// »çżřĽö Ĺ°(YĂŕ90µµ)
			
			Yaw	= 0;
			Pitch	= 0;															/// XĂŕ 0µµ Č¸Ŕü
			Roll	= 0;
			D3DXQuaternionRotationYawPitchRoll( &g_aniRot[1], Yaw, Pitch, Roll );	/// »çżřĽö Ĺ°(XĂŕ90µµ)
			
			D3DXQUATERNION quat;
			D3DXQuaternionSlerp( &quat, &g_aniRot[0], &g_aniRot[1], m_fFrameLerp );
			D3DXMatrixRotationQuaternion( &mat, &quat );							/// »çżřĽö¸¦ Č¸ŔüÇŕ·Ä°ŞŔ¸·Î şŻČŻ
			D3DXVec3TransformCoord(&m_vVel, &m_vVel, &mat);
			
			
			if( m_fFrameLerp > 1.0f )
			{
				m_fFrameLerp = 0.0f;	
				m_fAniFlag   = 0;
				m_bAniKeyBlock = FALSE;
			}
		}
	}
	
	else if(m_fAniFlag == ANI_REFLEXION_RIGHT_FLAG || m_fAniFlag == ANI_REFLEXION_LEFT_FLAG)/// ş®Ŕ» ¸¸łŻ¶§ Č¸Ŕü ąÝ°ć°č»ę 									/// ÁÂżě Č¸Ŕü ÁÂÇĄ°č»ę
	{		
		if(m_fAniFlag == ANI_REFLEXION_LEFT_FLAG)
		{
			if(IS_DT(m_myShuttleInfo.UnitKind) && !m_bIsAir)
			{
				// 2008-02-12 by dgwoo A±âľî°ˇ ¶ĄżˇĽ­ Ăćµą˝Ă ąćÇâŔüČŻŔ» ´Ů¸Ą±âľîżÍ Á¶±Ý ´Ů¸Ł°Ô Ăł¸®.
				D3DXMATRIX matTemp;	
				
				D3DXMatrixRotationAxis(&matTemp, &m_vUp, -0.08f);
				D3DXVec3TransformCoord(&m_vVel, &m_vVel, &matTemp);
				
				Yaw = D3DX_PI * -2.0f / 240.0f;	

			}
			
			else
			{
				D3DXMATRIX matTemp;	
				D3DXMatrixRotationAxis(&matTemp, &m_vUp, -0.1f);
				D3DXVec3TransformCoord(&m_vVel, &m_vVel, &matTemp);
				Yaw = D3DX_PI * -5.0f / 240.0f;			
			}
			
		}
		else
		{
			if(IS_DT(m_myShuttleInfo.UnitKind))
			{
				D3DXMATRIX matTemp;	
				
				D3DXMatrixRotationAxis(&matTemp, &m_vUp, 0.08f);
				D3DXVec3TransformCoord(&m_vVel, &m_vVel, &matTemp);
				
				Yaw = D3DX_PI * 2.0f / 240.0f;
			}
			else
			{

				D3DXMATRIX matTemp;	
					
				D3DXMatrixRotationAxis(&matTemp, &m_vUp, 0.1f);
				D3DXVec3TransformCoord(&m_vVel, &m_vVel, &matTemp);

				Yaw = D3DX_PI * 5.0f / 240.0f;
			}
		
//			m_fFrameLerp += 0.1f;
			m_fFrameLerp += fElapsedTime;// 2004-07-21 by dhkwon
			
			D3DXMATRIX mat;
			D3DXQUATERNION	g_aniRot[2];											/// Č¸Ŕü(quaternion)Ĺ° °Ş
			
			FLOAT Pitch	= 0;
			FLOAT Roll	= 0;
			D3DXQuaternionRotationYawPitchRoll( &g_aniRot[0], Yaw, Pitch, Roll );	/// »çżřĽö Ĺ°(YĂŕ90µµ)
			
			Yaw	= 0;
			Pitch	= 0;															/// XĂŕ 0µµ Č¸Ŕü
			Roll	= 0;
			D3DXQuaternionRotationYawPitchRoll( &g_aniRot[1], Yaw, Pitch, Roll );	/// »çżřĽö Ĺ°(XĂŕ90µµ)
			
			D3DXQUATERNION quat;
			D3DXQuaternionSlerp( &quat, &g_aniRot[0], &g_aniRot[1], m_fFrameLerp );
			D3DXMatrixRotationQuaternion( &mat, &quat );							/// »çżřĽö¸¦ Č¸ŔüÇŕ·Ä°ŞŔ¸·Î şŻČŻ
			D3DXVec3TransformCoord(&m_vVel, &m_vVel, &mat);
			
			
			if( m_fFrameLerp > 1.0f )
			{
				m_fFrameLerp = 0.0f;	
				m_fAniFlag   = 0;
				m_bAniKeyBlock = FALSE;
			}
		}
		
			if( m_fShuttleSpeed <= 0)
			{
				m_fShuttleSpeed = 10;
			}
			if(m_nBooster == BOOSTER_TYPE_AIRBREAK)
			{
				m_nBooster = BOOSTER_TYPE_MIN;
		}
	}	
//	SendPacket(T_FC_MOVE);
	g_fSendMovePacket = max(g_fSendMovePacket,5.0f);
}	
		



BOOL CShuttleChild::ScanEnemyFrontBAckAndSpace(void)
{	
	float		LengthCenter;
		 
	CMapEnemyIterator itEnemy = g_pScene->m_mapEnemyList.begin();
	while(itEnemy != g_pScene->m_mapEnemyList.end())
	{
		LengthCenter  = D3DXVec3Length(&(itEnemy->second->m_vPos - m_vAniFrame));
		
		if(LengthCenter <= 5)
		{
			return TRUE;
		}	
				
		itEnemy++;
	}
	return FALSE;
}

BOOL CShuttleChild::InitCinemaSkill(int nSkillItemNumber)
{
	// 2005-12-07 by ispark, żˇľî˝şĹł »çżëÇŃ´Ů.
	m_bSkillAirMove = TRUE;

	if(InitCinemaUnit(nSkillItemNumber, TRUE) == FALSE)
	{
		m_bSkillMoveIsUse = FALSE;
//		ITEM_SKILL* pItemSkill = m_pSkill->FindItemSkill(nSkillItemNumber);
//		map<LONGLONG, USED_ITEM*>::iterator	itUsedItem = m_mapUsedItem.find(pItemSkill->UniqueNumber);
//		if(itUsedItem != m_mapUsedItem.end())
//		{
//			m_mapUsedItem.erase(itUsedItem);
//		}
//		vector<USED_ITEM*>::iterator itUsedItem = m_vecUsedItem.begin();
//		while(itUsedItem != m_vecUsedItem.end())
//		{
//			if((*itUsedItem)->ItemNum == pItemSkill->ItemNum)
//			{
//				SAFE_DELETE(itUsedItem);
//				m_vecUsedItem.erase(itUsedItem);
//				break;
//			}
//			itUsedItem++;
//		}

		m_nCurrentPatternNumber = 0;
		return FALSE;
	}
	m_bSkillMoveIsUse = TRUE;
	m_nCurrentPatternNumber = nSkillItemNumber;
	return TRUE;
}

BOOL CShuttleChild::InitCinemaUnit(int nUnitPatternType, BOOL bUpUse)
{
	char str[32];
	sprintf(str,"%08d",nUnitPatternType);
	if(m_pCinemaUnitPattern != NULL)
	{
		SAFE_DELETE( m_pCinemaUnitPattern );
	}
 	m_pCinemaUnitPattern = g_pScene->LoadCinemaData(str);
	if(m_pCinemaUnitPattern == NULL)
	{
		DBGOUT( "Cinema : Can't Find Pattern(Unit) File.(%s)\n",str);
		return FALSE;
	}
	EVENT_POINT ep;
	ep.vPosition = D3DXVECTOR3(m_mMatrix._41,m_mMatrix._42,m_mMatrix._43);
	ep.vDirection = -D3DXVECTOR3(m_mMatrix._31,m_mMatrix._32,m_mMatrix._33);
	if(bUpUse)	// 2005-08-12 by ispark
	{
		ep.vUpVector = D3DXVECTOR3(m_mMatrix._21,m_mMatrix._22,m_mMatrix._23);
	}
	else
	{
		// °­Á¦ ĽÂĆĂ
		ep.vUpVector = D3DXVECTOR3(0, 1, 0);
	}
	D3DXVec3Normalize(&ep.vDirection,&ep.vDirection);
	ep.fVelocity = 100;
	ep.fCurvature = DEFAULT_CURVATURE;	// ĂĘ±â°Ş ĽÂĆĂ
	m_pCinemaUnitPattern->InitCinemaData(ep);
	return TRUE;
}
// 2010. 05. 27 by jskim ˝Ăł×¸¶ Ŕűżë Ä«¸Ţ¶ó ±¸Çö
BOOL CShuttleChild::InitCinemaUnit(int nUnitPatternType, BOOL bUpUse,D3DXVECTOR3 vPosition, D3DXVECTOR3 vDirection)
{
	char str[32];
	sprintf(str,"%08d",nUnitPatternType);
	if(m_pCinemaUnitPattern != NULL)
	{
		SAFE_DELETE( m_pCinemaUnitPattern );
	}
	m_pCinemaUnitPattern = g_pScene->LoadCinemaData(str);
	if(m_pCinemaUnitPattern == NULL)
	{
		DBGOUT( "Cinema : Can't Find Pattern(Unit) File.(%s)\n",str);
		return FALSE;
	}
	EVENT_POINT ep;
	ep.vPosition = D3DXVECTOR3(vPosition.x, vPosition.y,vPosition.z);
	ep.vDirection = D3DXVECTOR3(vDirection.x,vDirection.y,vDirection.z);
	//m_vPos = vTarget;
	if(bUpUse)	// 2005-08-12 by ispark
	{
		ep.vUpVector = D3DXVECTOR3(m_mMatrix._21,m_mMatrix._22,m_mMatrix._23);
	}
	else
	{
		// °­Á¦ ĽÂĆĂ
		ep.vUpVector = D3DXVECTOR3(0, 1, 0);
	}
	D3DXVec3Normalize(&ep.vDirection,&ep.vDirection);
	ep.fVelocity = 100;
	ep.fCurvature = DEFAULT_CURVATURE;	// ĂĘ±â°Ş ĽÂĆĂ
	m_pCinemaUnitPattern->InitCinemaData(ep);
	return TRUE;
}

void CShuttleChild::InfinityCinema(BOOL bSuccess)
{

	if ( TRUE == g_pInterface->m_pInfinity->GetEnableCinemaSkip() )
		return;

	CObjectChild * pCurrentObjEvent = g_pScene->FindEventObjectByTypeAndPositionIndex(EVENT_TYPE_INFI_CINEMA, EVENT_TYPE_PARAM_INFI_CINEMA);

	if(pCurrentObjEvent)
	{
		D3DXVECTOR3 vVel,vUp, vPos;
		vVel = m_vPos - pCurrentObjEvent->m_vPos;
		D3DXVec3Normalize(&vPos, &(pCurrentObjEvent->m_vPos - g_pShuttleChild->m_vPos));
		vUp.x = m_mMatrix._21;
		vUp.y = m_mMatrix._22;
		vUp.z = m_mMatrix._23;
		if(vVel == vUp)
		{// ¸ĹĆ®¸Ż˝ş°ˇ ±úÁöÁö ľĘµµ·Ď
			m_mMatrix._21 += 0.001f;
			m_mMatrix._22 -= 0.001f;
			m_mMatrix._23 += 0.002f;
		}
		m_mMatrix._31 = vVel.x;
		m_mMatrix._32 = vVel.y;
		m_mMatrix._33 = vVel.z;
		int succestype;
		if(bSuccess)
		{
			succestype = PATTERN_CAMERA_SUCCESS_INFINITY;
		}
		else
		{
			succestype = PATTERN_CAMERA_FAIL_INFINITY;
		}
	
		if(InitCinemaUnit(succestype, FALSE, pCurrentObjEvent->m_vPos, vPos) == TRUE)
		{
			m_bSkillMoveIsUse = TRUE;
			g_pD3dApp->StartFadeEffect(TRUE,3,D3DCOLOR_ARGB(0,0,0,0));
			if(g_pInterface->m_bShowInterface)
			{				
				SetShowCinema ( TRUE );
				g_pInterface->SetScreenShotMode(TRUE);
				m_bKeyBlock = TRUE;
			}
		}	
		else
		{
			DBGOUT("[succestype] : Pattern File Non Exsistent.\n");

			g_pInterface->m_pInfinity->SendPacket_SkipEndingCinema();
		}
	}

}

BOOL CShuttleChild::InitCinemaCamera(int nCameraPattern, D3DXVECTOR3 vPosition, D3DXVECTOR3 vDirection)
{
	D3DXVECTOR3 vPos,vVel,vVel2,vUp;
	vVel = vDirection;
	vUp =  D3DXVECTOR3(0,1,0);
	vPos = vPosition;	
	char str[32];
	sprintf(str,"%08d",nCameraPattern);
	if(m_pCinemaCamera != NULL)
	{
		SAFE_DELETE( m_pCinemaCamera );
	}
	m_pCinemaCamera = g_pScene->LoadCinemaData(str);
	if(m_pCinemaCamera == NULL) // ÇŘ´ç ĆĐĹĎ ĆÄŔĎŔĚ ľř´Â °ćżě
	{
		m_bIsCameraPattern = FALSE;
		DBGOUT( "Cinema : Can't Find Pattern(Camera) File.(%s)\n",str);
		return FALSE;
	}
	m_bIsCameraPattern = TRUE;
	EVENT_POINT ep;
	//	ep.vPosition = g_pShuttleChild->m_vPos;
	//	ep.vDirection = g_pShuttleChild->m_vVel;
	//	ep.vUpVector = D3DXVECTOR3(0,1,0);
	ep.vPosition = vPos; 
	ep.vDirection = vVel;
	ep.vUpVector = vUp;
	D3DXVec3Normalize(&ep.vDirection,&ep.vDirection);
	ep.fVelocity = 100;
	ep.fCurvature = DEFAULT_CURVATURE;	// ĂĘ±â°Ş ĽÂĆĂ
	m_pCinemaCamera->InitCinemaData(ep);
	return TRUE;
}
//end 2010. 05. 27 by jskim ˝Ăł×¸¶ Ŕűżë Ä«¸Ţ¶ó ±¸Çö 

BOOL CShuttleChild::InitCinemaCamera(int nCameraPattern)
{
	D3DXVECTOR3 vPos,vVel,vUp;
	vVel = m_vVel;
	vVel.y = 0;
	D3DXVec3Normalize(&vVel,&vVel);
	vUp =  D3DXVECTOR3(0,1,0);
	vPos = m_vPos - vVel*100 + vUp*100;
//	g_pCamera->SetViewParams( vPos , vVel , vUp );
	
	char str[32];
	sprintf(str,"%08d",nCameraPattern);	
	if(m_pCinemaCamera != NULL)
	{
		SAFE_DELETE( m_pCinemaCamera );
	}
	m_pCinemaCamera = g_pScene->LoadCinemaData(str);
	if(m_pCinemaCamera == NULL) // ÇŘ´ç ĆĐĹĎ ĆÄŔĎŔĚ ľř´Â °ćżě
	{
		m_bIsCameraPattern = FALSE;
		DBGOUT( "Cinema : Can't Find Pattern(Camera) File.(%s)\n",str);
		return FALSE;
	}
	m_bIsCameraPattern = TRUE;
	EVENT_POINT ep;
//	ep.vPosition = g_pShuttleChild->m_vPos;
//	ep.vDirection = g_pShuttleChild->m_vVel;
//	ep.vUpVector = D3DXVECTOR3(0,1,0);
	ep.vPosition = vPos;
	ep.vDirection = vVel;
	ep.vUpVector = vUp;
	D3DXVec3Normalize(&ep.vDirection,&ep.vDirection);
	ep.fVelocity = 100;
	ep.fCurvature = DEFAULT_CURVATURE;	// ĂĘ±â°Ş ĽÂĆĂ
	m_pCinemaCamera->InitCinemaData(ep);
	return TRUE;
}

// Ăćµą °Ë»ç 2004.06.19 jschoi 
// ŔĚ ÇÔĽö´Â ´ÜÁö ĽĹĆ˛°ú ¸Ę żŔşęÁ§Ć®żÍŔÇ Ăćµą »óĹÂ¸¸ ş»´Ů.
BOOL CShuttleChild::CheckCollMapObject()
{
	float fElapsedTime = g_pD3dApp->GetElapsedTime();
	int nCollType = COLL_NONE;	
	
	D3DXMATRIX matArray[6];
	D3DXVECTOR3 vSide,vVel;

	D3DXVec3Cross(&vSide,&m_vUp,&m_vVel);

	D3DXMatrixLookAtLH(&matArray[COLL_FRONT],&m_vPos,&(m_vPos + m_vVel),&m_vUp);	// ľŐ
	D3DXMatrixLookAtLH(&matArray[COLL_BACK],&m_vPos,&(m_vPos - m_vVel),&m_vUp);	// µÚ
	D3DXMatrixLookAtLH(&matArray[COLL_LEFT],&m_vPos,&(m_vPos - vSide),&m_vUp);		// ÁÂ
	D3DXMatrixLookAtLH(&matArray[COLL_RIGHT],&m_vPos,&(m_vPos + vSide),&m_vUp);		// żě
	D3DXMatrixLookAtLH(&matArray[COLL_UP],&m_vPos,&(m_vPos + m_vUp),&vSide);		// Ŕ§
	D3DXMatrixLookAtLH(&matArray[COLL_DOWN],&m_vPos,&(m_vPos - m_vUp),&vSide);		// ľĆ·ˇ
	
	float fFrontMove = m_fShuttleSpeed*fElapsedTime;
//	float fSideMove = m_fLRVelRate*m_fRealShuttleSpeedMax*fElapsedTime;
	float size[6] = { 10+fFrontMove,10-fFrontMove,10,10,5,5 };	// ŔŻ´Ö Ĺ©±â	ľŐ,µÚ,ÁÂ,żě,Ŕ§,ľĆ·ˇ

	nCollType = g_pScene->m_pObjectRender->CheckCollMesh(matArray,size).nCollType;
	
	if(nCollType == COLL_NONE)
	{
		// Ăćµą ľČÇÔ
		return FALSE;
	}
	else
	{
		// Ăćµą Çß´Ů
		return TRUE;
	}
}

void CShuttleChild::DeletePattern()
{
	// 2005-07-18 by ispark
	// FALLING»óĹÂŔĎ¶§ Ä«¸Ţ¶ó ĆĐĹĎŔ» ľřľÖ¸é Á¤ÁöµÇ´Â ąö±×
	if(m_bIsCameraPattern == TRUE && m_dwState == _FALLING)
	{
		return;
	}

	if(	m_nCurrentPatternNumber == PATTERN_UNIT_WARP_OUT)// żöÇÁ ľĆżô ĆĐĹĎ Áľ·á ˝Ăżˇ´Â żŁÁř ˝şĹľ
	{
		m_nShuttleStopState = 4;	// Air_Break Type Ăß°ˇ
	}
	m_nCurrentPatternNumber = 0;
	m_fOldUpRate= 0;		// ŔĚŔü ľ÷ş¤ĹÍ¸¦ ±â·ĎÇĎż© şÎµĺ·´°Ô ±âżďŔÎ´Ů(ÁÂżě ŔĚµż ±âżďŔÓ°ú Č¸Ŕü ±âżďŔÓ Á¶Ŕý) - jschoi
	m_fSkillMoveTime = 0.0f;
	SetCursorInit();
	m_bSkillMoveIsUse = FALSE;
	SAFE_DELETE(m_pCinemaUnitPattern);
	SAFE_DELETE(m_pCinemaCamera);
	m_bSkillAirMove = FALSE;		// 2005-12-07 by ispark

	if(m_bIsCameraPattern == TRUE)
	{
//		g_pD3dApp->m_pCamera->Tick(m_vPos , m_vVel, m_vUp);
		m_bIsCameraPattern = FALSE;				
	}
	// 2010. 05. 27 by jskim ˝Ăł×¸¶ Ŕűżë Ä«¸Ţ¶ó ±¸Çö
	if( IsShowCinema() )
	{
		g_pInterface->SetScreenShotMode(FALSE);
		SetShowCinema ( FALSE );
		m_bKeyBlock = FALSE;
// 2011-06-30 by jhahn ŔÎÇÇ3Â÷ ˝ĂÁˇ 1ŔÎÄŞ şŻ°ć
		g_pD3dApp->m_pCamera->SetCamType( CAMERA_TYPE_NORMAL);   
		m_InfiState = TRUE;
//end 2011-06-30 by jhahn ŔÎÇÇ3Â÷ ˝ĂÁˇ 1ŔÎÄŞ şŻ°ć

//2011-07-20 by jhahn	ŔÎÇÇ3Â÷ ˝Ăł×¸¶ÇĂ·ąŔĚÁß Ĺ¸Äł¸ŻĹÍ ľČş¸ŔĚ±â

		CMapEnemyIterator itEnemy = g_pScene->m_mapEnemyList.begin();	
		while(itEnemy != g_pScene->m_mapEnemyList.end())
		{ 							
			itEnemy->second->m_bRender = TRUE;	
			itEnemy->second->SetShowData();
			itEnemy++;
		}			

//end 2011-07-20 by jhahn	ŔÎÇÇ3Â÷ ˝Ăł×¸¶ÇĂ·ąŔĚÁß Ĺ¸Äł¸ŻĹÍ ľČş¸ŔĚ±â
	}
	//end 2010. 05. 27 by jskim ˝Ăł×¸¶ Ŕűżë Ä«¸Ţ¶ó ±¸Çö
}

//void CShuttleChild::SetAttackAvaliableEnemyIndex( int nTargetIndex, BOOL bSet )
//{
//	if(bSet)
//	{
//		vector<int>::iterator it = m_vecAttackAvailableEnemyIndex.begin();
//		while(it != m_vecAttackAvailableEnemyIndex.end() )
//		{
//			if( *it == nTargetIndex )
//			{
//				return;
//			}
//			it++;
//		}
//		m_vecAttackAvailableEnemyIndex.push_back( nTargetIndex );
//	}
//	else
//	{
//		vector<int>::iterator it = m_vecAttackAvailableEnemyIndex.begin();
//		while(it != m_vecAttackAvailableEnemyIndex.end() )
//		{
//			if( *it == nTargetIndex )
//			{
//				m_vecAttackAvailableEnemyIndex.erase( it );
//				break;
//			}
//			it++;
//		}
//	}
//}

///////////////////////////////////////////////////////////////////////////////
/// \fn			CShuttleChild::SetPKMode(BOOL bSet)
/// \brief		PK°ˇ´É¸ĘżˇĽ­ EnemyµéŔ» ¸đµÎ m_vecAttackAvailableEnemyIndexżˇ łÖ°í »«´Ů.
/// \author		dhkwon
/// \date		2004-07-21 ~ 2004-07-21
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CShuttleChild::SetPKMode(BOOL bSet)
{
	if(bSet)
	{
		CMapEnemyIterator itMap = g_pScene->m_mapEnemyList.begin();
		while( itMap != g_pScene->m_mapEnemyList.end() )
		{
			itMap->second->SetPkState( PK_NORMAL, TRUE );
//			vector<int>::iterator itVec = m_vecAttackAvailableEnemyIndex.begin();
//			while(itVec != m_vecAttackAvailableEnemyIndex.end() )
//			{
//				if( *itVec == (*itEnemy)->m_infoCharacter.ClientIndex )
//				{
//					break;
//				}
//				itVec++;
//			}
//			if(itVec == m_vecAttackAvailableEnemyIndex.end())
//			{
//				m_vecAttackAvailableEnemyIndex.push_back((*itEnemy)->m_infoCharacter.ClientIndex);
//			}
			itMap++;
		}
	}
	else
	{
		CMapEnemyIterator itMap = g_pScene->m_mapEnemyList.begin();
		while( itMap != g_pScene->m_mapEnemyList.end() )
		{
			if( itMap->second->IsPkEnable() == TRUE )
			{
				itMap->second->SetPkState( PK_NORMAL, FALSE );
			}
			itMap++;
		}
//		vector<int>::iterator itVec = m_vecAttackAvailableEnemyIndex.begin();
//		while(itVec != m_vecAttackAvailableEnemyIndex.end() )
//		{
//			if( *itVec < MONSTER_CLIENT_INDEX_START_NUM )
//			{
//				CMapEnemyIterator itMap = g_pScene->m_mapEnemyList.find( *itVec );
//				if( itMap != g_pScene->m_mapEnemyList.end() )
//				{
//					if( itMap->second->IsPkEnable() == FALSE )
//					{
//						m_vecAttackAvailableEnemyIndex.erase( itVec );
//						continue;
//					}
//				}
//				else
//				{
//					m_vecAttackAvailableEnemyIndex.erase( itVec );
//					continue;
//				}
//			}
//			itVec++;
//		}
	}
}


DWORD CShuttleChild::GetLastAttackTick()
{
	return max( m_pPrimaryWeapon ? m_pPrimaryWeapon->GetAttackTick() : 0, 
				m_pSecondaryWeapon ? m_pSecondaryWeapon->GetAttackTick() : 0);
}

void CShuttleChild::TryLandingState()
{
	// 2005-04-11 by jschoi - Tutorial
	if(g_pTutorial->IsTutorialMode() == TRUE &&
		g_pTutorial->GetLesson() != L1)
		return;

	if(m_fNumpad0keyTime <= 0.0f)
	{
		int nCheckX = ((int)m_vPos.x)/TILE_SIZE;
		int nCheckZ = ((int)m_vPos.z)/TILE_SIZE;

		if (nCheckX < 0)
			nCheckX = 0;

		if (nCheckZ < 0)
			nCheckZ = 0;

		if( g_pGround->m_pTileInfo[nCheckX*g_pGround->m_projectInfo.sYSize + nCheckZ].bEnableLand ||
			(IS_DT(m_myShuttleInfo.UnitKind) && m_bIsAir))
		{
			BOOL bObjectLanging = 
				(IS_DT(m_myShuttleInfo.UnitKind)==FALSE ||
				IsTileMapRenderEnable(m_myShuttleInfo.MapChannelIndex.MapIndex) == FALSE) ? TRUE : FALSE;

			if(SetOrderMoveTargetPos(bObjectLanging) == TRUE)// ŔŻ´ÖŔÇ Âř·úÁöÁˇ ÁÂÇĄ °č»ę
			{
				m_fLandingDist = m_vPos.y - m_vTargetPos.y;
				if(m_fLandingDist < 180.0f)
				{
					// ŔÎÄŞ şŻČŻ Âř·úŔĺżˇĽ­ 1ŔÎÄŞŔ¸·Î Âř·ú˝Ă 3ŔÎÄŞŔüČŻ 
					if(g_pD3dApp->m_pCamera->m_bCamType == CAMERA_TYPE_FPS)
					{
						g_pD3dApp->m_pCamera->ChangeCamType();
						m_bLandingOldCamtypeFPS = TRUE;
					}

					m_fAniFlag = 0;
					m_bAniKeyBlock = FALSE;
					m_fNumpad0keyTime = 2.0f;
					SendPacket(T_FC_MOVE_LANDING);
					if(IS_DT(m_myShuttleInfo.UnitKind))
					{
						ChangeUnitState(_LANDING);
					}
				}
			}
		}
	}
//	SetShuttleFlight();		// 2005-11-16 by ispark
//	if(m_bFormationFlight)
	if(m_pClientParty->IsFormationFlightMove())
	{
		SendPacket(T_IC_PARTY_CANCEL_FLIGHT_POSITION);
		m_pClientParty->FormationFlightClear();
//		m_infoParty.bFormationFlyingPositionType = 0;
//		m_bFormationFlight = FALSE;
		g_pD3dApp->m_pChat->CreateChatChild(STRMSG_C_PARTY_0001,COLOR_SYSTEM);//"Ćí´ëşńÇŕżˇĽ­ ąţľîłµ˝Ŕ´Ď´Ů."
	}
}


void CShuttleChild::UsedItemTick(float fElapsedTime)
{
	vector<USED_ITEM*>::iterator itUsedItem = m_vecUsedItem.begin();
	while (itUsedItem != m_vecUsedItem.end()) {
		(*itUsedItem)->fOverHeatCheckTime += fElapsedTime * ((*itUsedItem)->fOverHeatTime / OVERHEAT_REPAIR_TIME);
		if ((*itUsedItem)->fOverHeatCheckTime >= (*itUsedItem)->fOverHeatTime) {
			SAFE_DELETE(*itUsedItem);
			itUsedItem = m_vecUsedItem.erase(itUsedItem);
		}
		else {
			itUsedItem++;
		}
	}
}

void CShuttleChild::PutUsedItem(CWeaponItemInfo* pWeaponItemInfo)
{
	USED_ITEM* pUsedItem = new USED_ITEM;
	pUsedItem->UniqueNumber = pWeaponItemInfo->GetItemGeneral()->UniqueNumber;
	pUsedItem->bIsOverHeat = pWeaponItemInfo->IsOverHeat();
	pUsedItem->fOverHeatTime = pWeaponItemInfo->GetOverHeatTime();
	pUsedItem->fOverHeatCheckTime = pWeaponItemInfo->GetOverHeatCheckTime();
	m_vecUsedItem.push_back(pUsedItem);
}

USED_ITEM* CShuttleChild::GetUsedItem(LONGLONG UniqueNumber)
{
	vector<USED_ITEM*>::iterator itUsedItem = m_vecUsedItem.begin();
	while(itUsedItem != m_vecUsedItem.end())
	{
		if((*itUsedItem)->UniqueNumber == UniqueNumber)
		{
			return *itUsedItem;
		}
		itUsedItem++;
	}
	return NULL;
}

void CShuttleChild::DeleteUsedItem()
{
	vector<USED_ITEM*>::iterator itUsedItem = m_vecUsedItem.begin();
	while(itUsedItem != m_vecUsedItem.end())
	{
		SAFE_DELETE(*itUsedItem);
		itUsedItem++;
	}
	m_vecUsedItem.clear();
}

void CShuttleChild::SetWearItemParamFactor( int nPos, ITEM* pITEM )
{
	// 2009-04-21 by bhsohn ľĆŔĚĹŰ DesParamĂß°ˇ	
// 	if(pITEM)
// 	{
// 		m_wearItemParamFactor[nPos].DestParameter1 = pITEM->DestParameter1;
// 		m_wearItemParamFactor[nPos].DestParameter2 = pITEM->DestParameter2;
// 		m_wearItemParamFactor[nPos].DestParameter3 = pITEM->DestParameter3;
// 		m_wearItemParamFactor[nPos].DestParameter4 = pITEM->DestParameter4;
// 		m_wearItemParamFactor[nPos].ParameterValue1 = pITEM->ParameterValue1;
// 		m_wearItemParamFactor[nPos].ParameterValue2 = pITEM->ParameterValue2;
// 		m_wearItemParamFactor[nPos].ParameterValue3 = pITEM->ParameterValue3;
// 		m_wearItemParamFactor[nPos].ParameterValue4 = pITEM->ParameterValue4;
// 	}
// 	else
// 	{
// 		m_wearItemParamFactor[nPos].DestParameter1 = 0;
// 		m_wearItemParamFactor[nPos].DestParameter2 = 0;
// 		m_wearItemParamFactor[nPos].DestParameter3 = 0;
// 		m_wearItemParamFactor[nPos].DestParameter4 = 0;
// 		m_wearItemParamFactor[nPos].ParameterValue1 = 0;
// 		m_wearItemParamFactor[nPos].ParameterValue2 = 0;
// 		m_wearItemParamFactor[nPos].ParameterValue3 = 0;
// 		m_wearItemParamFactor[nPos].ParameterValue4 = 0;
// 	}
	int nArrParamCnt = 0;
	if(pITEM)
	{
		for(nArrParamCnt = 0; nArrParamCnt < SIZE_MAX_DESPARAM_COUNT_IN_ITEM;nArrParamCnt++)
		{
			m_wearItemParamFactor[nPos].ArrDestParameter[nArrParamCnt] = pITEM->ArrDestParameter[nArrParamCnt];			
			m_wearItemParamFactor[nPos].ArrParameterValue[nArrParamCnt] = pITEM->ArrParameterValue[nArrParamCnt];			
		}
	}
 	else
	{
		for(nArrParamCnt = 0; nArrParamCnt < SIZE_MAX_DESPARAM_COUNT_IN_ITEM;nArrParamCnt++)
		{
			m_wearItemParamFactor[nPos].ArrDestParameter[nArrParamCnt] = 0;
			m_wearItemParamFactor[nPos].ArrParameterValue[nArrParamCnt] = 0;
		}		
	}
	// end 2009-04-21 by bhsohn ľĆŔĚĹŰ DesParamĂß°ˇ
}

// 2010. 04. 21 by jskim ˝Ĺ±Ô ·°Ĺ° ¸Ó˝Ĺ ±¸Çö
void CShuttleChild::SetWearRareItemToCharacterParamFactor(int nPos, CItemInfo* pItemInfo)
{
	if(nPos != POS_CENTER)
	{
		return;
	}
	for(int nArrParamCnt = 0; nArrParamCnt < SIZE_MAX_DESPARAM_COUNT_IN_ITEM;nArrParamCnt++)
	{
		if(pItemInfo->GetPrefixRareInfo())
		{
			m_wearPreRareItemParamFactor[nPos].ArrDestParameter[nArrParamCnt] =		pItemInfo->GetPrefixRareInfo()->DesParameter[nArrParamCnt];
			m_wearPreRareItemParamFactor[nPos].ArrParameterValue[nArrParamCnt] =	pItemInfo->GetPrefixRareInfo()->ParameterValue[nArrParamCnt];
		}
		else
		{
			m_wearPreRareItemParamFactor[nPos].ArrDestParameter[nArrParamCnt] =		0;
			m_wearPreRareItemParamFactor[nPos].ArrParameterValue[nArrParamCnt] =	0;
		}
		
		if(pItemInfo->GetSuffixRareInfo())
		{
			m_wearSufRareItemParamFactor[nPos].ArrDestParameter[nArrParamCnt] =		pItemInfo->GetSuffixRareInfo()->DesParameter[nArrParamCnt];			
			m_wearSufRareItemParamFactor[nPos].ArrParameterValue[nArrParamCnt] =	pItemInfo->GetSuffixRareInfo()->ParameterValue[nArrParamCnt];
		}
		else
		{
			m_wearSufRareItemParamFactor[nPos].ArrDestParameter[nArrParamCnt] =		0;
			m_wearSufRareItemParamFactor[nPos].ArrParameterValue[nArrParamCnt] =	0;
		}
	}
}
//end 2010. 04. 21 by jskim ˝Ĺ±Ô ·°Ĺ° ¸Ó˝Ĺ ±¸Çö

// 2010. 04. 21 by jskim ˝Ĺ±Ô ·°Ĺ° ¸Ó˝Ĺ ±¸Çö
//void CShuttleChild::SetWearItemToCharacterParamFactor(int nPos, ITEM* pITEM)		// ITEMŔÇ ParamFactor¸¦ CHARACTERżˇ ŔűżëÇŃ´Ů.
void CShuttleChild::SetWearItemToCharacterParamFactor(int nPos, CItemInfo* pItemInfo)
//end 2010. 04. 21 by jskim ˝Ĺ±Ô ·°Ĺ° ¸Ó˝Ĺ ±¸Çö
{
	// 2010. 04. 21 by jskim ˝Ĺ±Ô ·°Ĺ° ¸Ó˝Ĺ ±¸Çö
	//SetWearItemParamFactor( nPos, pITEM );
	SetWearItemParamFactor( nPos, pItemInfo->GetRealItemInfo() );
	//end 2010. 04. 21 by jskim ˝Ĺ±Ô ·°Ĺ° ¸Ó˝Ĺ ±¸Çö		
	// 2009-04-21 by bhsohn ľĆŔĚĹŰ DesParamĂß°ˇ	
// 	SetParamFactorDesParam(m_paramFactor, m_wearItemParamFactor[nPos].DestParameter1, m_wearItemParamFactor[nPos].ParameterValue1 );
// 	SetParamFactorDesParam(m_paramFactor, m_wearItemParamFactor[nPos].DestParameter2, m_wearItemParamFactor[nPos].ParameterValue2 );
// 	SetParamFactorDesParam(m_paramFactor, m_wearItemParamFactor[nPos].DestParameter3, m_wearItemParamFactor[nPos].ParameterValue3 );
// 	SetParamFactorDesParam(m_paramFactor, m_wearItemParamFactor[nPos].DestParameter4, m_wearItemParamFactor[nPos].ParameterValue4 );
// 
// 	if( m_wearItemParamFactor[nPos].DestParameter1 == DES_SHOTNUM_01 || 
// 		m_wearItemParamFactor[nPos].DestParameter1 == DES_MULTINUM_01 ||
// 		m_wearItemParamFactor[nPos].DestParameter1 == DES_REATTACKTIME_01 ||
// 		m_wearItemParamFactor[nPos].DestParameter2 == DES_SHOTNUM_01 || 
// 		m_wearItemParamFactor[nPos].DestParameter2 == DES_MULTINUM_01 || 
// 		m_wearItemParamFactor[nPos].DestParameter2 == DES_REATTACKTIME_01 ||
// 		m_wearItemParamFactor[nPos].DestParameter3 == DES_SHOTNUM_01 || 
// 		m_wearItemParamFactor[nPos].DestParameter3 == DES_MULTINUM_01 || 
// 		m_wearItemParamFactor[nPos].DestParameter3 == DES_REATTACKTIME_01 ||
// 		m_wearItemParamFactor[nPos].DestParameter4 == DES_SHOTNUM_01 || 
// 		m_wearItemParamFactor[nPos].DestParameter4 == DES_MULTINUM_01 || 
// 		m_wearItemParamFactor[nPos].DestParameter4 == DES_REATTACKTIME_01)
// 	{
// 		FieldSocketCharacterSendParamfactorInRange( POS_PROWOUT );
// 	}
// 	else if( m_wearItemParamFactor[nPos].DestParameter1 == DES_SHOTNUM_02 || 
// 		m_wearItemParamFactor[nPos].DestParameter1 == DES_MULTINUM_02 ||
// 		m_wearItemParamFactor[nPos].DestParameter1 == DES_REATTACKTIME_02 ||
// 		m_wearItemParamFactor[nPos].DestParameter2 == DES_SHOTNUM_02 || 
// 		m_wearItemParamFactor[nPos].DestParameter2 == DES_MULTINUM_02 || 
// 		m_wearItemParamFactor[nPos].DestParameter2 == DES_REATTACKTIME_02 ||
// 		m_wearItemParamFactor[nPos].DestParameter3 == DES_SHOTNUM_02 || 
// 		m_wearItemParamFactor[nPos].DestParameter3 == DES_MULTINUM_02 || 
// 		m_wearItemParamFactor[nPos].DestParameter3 == DES_REATTACKTIME_02 ||
// 		m_wearItemParamFactor[nPos].DestParameter4 == DES_SHOTNUM_02 || 
// 		m_wearItemParamFactor[nPos].DestParameter4 == DES_MULTINUM_02 || 
// 		m_wearItemParamFactor[nPos].DestParameter4 == DES_REATTACKTIME_02)
// 	{
// 		FieldSocketCharacterSendParamfactorInRange( POS_WINGOUT );
// 	}
	// 2010. 04. 21 by jskim ˝Ĺ±Ô ·°Ĺ° ¸Ó˝Ĺ ±¸Çö
	
	SetWearRareItemToCharacterParamFactor(nPos, pItemInfo);
	int nArrParamCnt = 0;
	for(nArrParamCnt = 0; nArrParamCnt < SIZE_MAX_DESPARAM_COUNT_IN_ITEM;nArrParamCnt++)
	{
		SetParamFactorDesParam(m_paramFactor, 
								m_wearItemParamFactor[nPos].ArrDestParameter[nArrParamCnt], 
								m_wearItemParamFactor[nPos].ArrParameterValue[nArrParamCnt]);

	}
	// 2010. 04. 21 by jskim ˝Ĺ±Ô ·°Ĺ° ¸Ó˝Ĺ ±¸Çö	
	nArrParamCnt=0;
	for(nArrParamCnt = 0; nArrParamCnt < SIZE_MAX_DESPARAM_COUNT_IN_ITEM;nArrParamCnt++)
	{
		// Á˘µÎ ´É·ÂÄˇ Ŕűżë
		SetParamFactorDesParam(m_paramFactor, 
		m_wearPreRareItemParamFactor[nPos].ArrDestParameter[nArrParamCnt], 
		m_wearPreRareItemParamFactor[nPos].ArrParameterValue[nArrParamCnt]);

		// Á˘ąĚ ´É·ÂÄˇ Ŕűżë
		SetParamFactorDesParam(m_paramFactor, 
			m_wearSufRareItemParamFactor[nPos].ArrDestParameter[nArrParamCnt], 
			m_wearSufRareItemParamFactor[nPos].ArrParameterValue[nArrParamCnt]);
	}
	//end 2010. 04. 21 by jskim ˝Ĺ±Ô ·°Ĺ° ¸Ó˝Ĺ ±¸Çö
	BOOL bBreak = FALSE;
	for(nArrParamCnt = 0; nArrParamCnt < SIZE_MAX_DESPARAM_COUNT_IN_ITEM;nArrParamCnt++)
	{
		if(m_wearItemParamFactor[nPos].ArrDestParameter[nArrParamCnt] == DES_SHOTNUM_01 || 
			m_wearItemParamFactor[nPos].ArrDestParameter[nArrParamCnt] == DES_MULTINUM_01 ||
			m_wearItemParamFactor[nPos].ArrDestParameter[nArrParamCnt] == DES_REATTACKTIME_01)
		{
			FieldSocketCharacterSendParamfactorInRange( POS_PROWOUT );
			bBreak = TRUE;
			break;
		}		
	}	
	for(nArrParamCnt = 0; nArrParamCnt < SIZE_MAX_DESPARAM_COUNT_IN_ITEM;nArrParamCnt++)
	{
		if(bBreak)
		{
			break;
		}
		if(m_wearItemParamFactor[nPos].ArrDestParameter[nArrParamCnt] == DES_SHOTNUM_02 || 
			m_wearItemParamFactor[nPos].ArrDestParameter[nArrParamCnt] == DES_MULTINUM_02 ||
			m_wearItemParamFactor[nPos].ArrDestParameter[nArrParamCnt] == DES_REATTACKTIME_02)
		{
			FieldSocketCharacterSendParamfactorInRange( POS_WINGOUT );
			break;
		}
	}
	// end 2009-04-21 by bhsohn ľĆŔĚĹŰ DesParamĂß°ˇ	
}

void CShuttleChild::ReleaseWearItemToCharacterParamFactor(int nPos)	// ITEMŔÇ ParamFactor¸¦ CHARACTERżˇĽ­ »«´Ů.
{
	// 2009-04-21 by bhsohn ľĆŔĚĹŰ DesParamĂß°ˇ	
// 	ReleaseParamFactorDesParam(m_paramFactor, m_wearItemParamFactor[nPos].DestParameter1, m_wearItemParamFactor[nPos].ParameterValue1 );
// 	ReleaseParamFactorDesParam(m_paramFactor, m_wearItemParamFactor[nPos].DestParameter2, m_wearItemParamFactor[nPos].ParameterValue2 );
// 	ReleaseParamFactorDesParam(m_paramFactor, m_wearItemParamFactor[nPos].DestParameter3, m_wearItemParamFactor[nPos].ParameterValue3 );
// 	ReleaseParamFactorDesParam(m_paramFactor, m_wearItemParamFactor[nPos].DestParameter4, m_wearItemParamFactor[nPos].ParameterValue4 );
// // 	SetWearItemParamFactor( nPos, NULL );
// 	if( m_wearItemParamFactor[nPos].DestParameter1 == DES_SHOTNUM_01 || 
// 		m_wearItemParamFactor[nPos].DestParameter1 == DES_MULTINUM_01 ||
// 		m_wearItemParamFactor[nPos].DestParameter1 == DES_REATTACKTIME_01 ||
// 		m_wearItemParamFactor[nPos].DestParameter2 == DES_SHOTNUM_01 || 
// 		m_wearItemParamFactor[nPos].DestParameter2 == DES_MULTINUM_01 || 
// 		m_wearItemParamFactor[nPos].DestParameter2 == DES_REATTACKTIME_01 ||
// 		m_wearItemParamFactor[nPos].DestParameter3 == DES_SHOTNUM_01 || 
// 		m_wearItemParamFactor[nPos].DestParameter3 == DES_MULTINUM_01 || 
// 		m_wearItemParamFactor[nPos].DestParameter3 == DES_REATTACKTIME_01 ||
// 		m_wearItemParamFactor[nPos].DestParameter4 == DES_SHOTNUM_01 || 
// 		m_wearItemParamFactor[nPos].DestParameter4 == DES_MULTINUM_01 || 
// 		m_wearItemParamFactor[nPos].DestParameter4 == DES_REATTACKTIME_01)
// 	{
// 		FieldSocketCharacterSendParamfactorInRange( POS_PROWOUT );
// 	}
// 	else if( m_wearItemParamFactor[nPos].DestParameter1 == DES_SHOTNUM_02 || 
// 		m_wearItemParamFactor[nPos].DestParameter1 == DES_MULTINUM_02 ||
// 		m_wearItemParamFactor[nPos].DestParameter1 == DES_REATTACKTIME_02 ||
// 		m_wearItemParamFactor[nPos].DestParameter2 == DES_SHOTNUM_02 || 
// 		m_wearItemParamFactor[nPos].DestParameter2 == DES_MULTINUM_02 || 
// 		m_wearItemParamFactor[nPos].DestParameter2 == DES_REATTACKTIME_02 ||
// 		m_wearItemParamFactor[nPos].DestParameter3 == DES_SHOTNUM_02 || 
// 		m_wearItemParamFactor[nPos].DestParameter3 == DES_MULTINUM_02 || 
// 		m_wearItemParamFactor[nPos].DestParameter3 == DES_REATTACKTIME_02 ||
// 		m_wearItemParamFactor[nPos].DestParameter4 == DES_SHOTNUM_02 || 
// 		m_wearItemParamFactor[nPos].DestParameter4 == DES_MULTINUM_02 || 
// 		m_wearItemParamFactor[nPos].DestParameter4 == DES_REATTACKTIME_02)
// 	{
// 		FieldSocketCharacterSendParamfactorInRange( POS_WINGOUT );
// 	}
	int nArrParamCnt = 0;
	for(nArrParamCnt = 0; nArrParamCnt < SIZE_MAX_DESPARAM_COUNT_IN_ITEM;nArrParamCnt++)
	{
		ReleaseParamFactorDesParam(m_paramFactor, 
										m_wearItemParamFactor[nPos].ArrDestParameter[nArrParamCnt], 
										m_wearItemParamFactor[nPos].ArrParameterValue[nArrParamCnt] );		
		
	}
	// 2010. 04. 21 by jskim ˝Ĺ±Ô ·°Ĺ° ¸Ó˝Ĺ ±¸Çö
	if(nPos == POS_CENTER)
	{
		for(nArrParamCnt = 0; nArrParamCnt < SIZE_MAX_DESPARAM_COUNT_IN_ITEM;nArrParamCnt++)
		{
			// Á˘µÎ ´É·ÂÄˇ ÇŘÁ¦
			ReleaseParamFactorDesParam(m_paramFactor, 
				m_wearPreRareItemParamFactor[nPos].ArrDestParameter[nArrParamCnt], 
				m_wearPreRareItemParamFactor[nPos].ArrParameterValue[nArrParamCnt]);
	 
 			// Á˘ąĚ ´É·ÂÄˇ ÇŘÁ¦
 			ReleaseParamFactorDesParam(m_paramFactor, 
				m_wearSufRareItemParamFactor[nPos].ArrDestParameter[nArrParamCnt], 
 				m_wearSufRareItemParamFactor[nPos].ArrParameterValue[nArrParamCnt]);
		}

		for(nArrParamCnt = 0; nArrParamCnt < SIZE_MAX_DESPARAM_COUNT_IN_ITEM;nArrParamCnt++)
		{
			m_wearPreRareItemParamFactor[nPos].ArrDestParameter[nArrParamCnt] =		0;
			m_wearPreRareItemParamFactor[nPos].ArrParameterValue[nArrParamCnt] =	0;
			m_wearSufRareItemParamFactor[nPos].ArrDestParameter[nArrParamCnt] =		0;
			m_wearSufRareItemParamFactor[nPos].ArrParameterValue[nArrParamCnt] =	0;
		}
	}
	//end 2010. 04. 21 by jskim ˝Ĺ±Ô ·°Ĺ° ¸Ó˝Ĺ ±¸Çö
	

	BOOL bBreak = FALSE;	
	for(nArrParamCnt = 0; nArrParamCnt < SIZE_MAX_DESPARAM_COUNT_IN_ITEM;nArrParamCnt++)
	{
		if( m_wearItemParamFactor[nPos].ArrDestParameter[nArrParamCnt] == DES_SHOTNUM_01 || 
			m_wearItemParamFactor[nPos].ArrDestParameter[nArrParamCnt] == DES_MULTINUM_01 ||
			m_wearItemParamFactor[nPos].ArrDestParameter[nArrParamCnt] == DES_REATTACKTIME_01)
		{
			FieldSocketCharacterSendParamfactorInRange( POS_PROWOUT );
			bBreak = TRUE;	
			break;
		}

	}
	for(nArrParamCnt = 0; nArrParamCnt < SIZE_MAX_DESPARAM_COUNT_IN_ITEM;nArrParamCnt++)
	{
		if(bBreak)
		{
			break;
		}
		if(m_wearItemParamFactor[nPos].ArrDestParameter[nArrParamCnt] == DES_SHOTNUM_02 
			|| m_wearItemParamFactor[nPos].ArrDestParameter[nArrParamCnt] == DES_MULTINUM_02 
			|| m_wearItemParamFactor[nPos].ArrDestParameter[nArrParamCnt] == DES_REATTACKTIME_02 )
		{
			FieldSocketCharacterSendParamfactorInRange( POS_WINGOUT );
			break;
		}

	}	
	// end 2009-04-21 by bhsohn ľĆŔĚĹŰ DesParamĂß°ˇ	
	SetWearItemParamFactor( nPos, NULL );
}
// 2010. 04. 21 by jskim ˝Ĺ±Ô ·°Ĺ° ¸Ó˝Ĺ ±¸Çö
//void CShuttleChild::ChangeWearItemToCharacterParamFactor(int nPos, ITEM* pITEM)
void CShuttleChild::ChangeWearItemToCharacterParamFactor(int nPos, CItemInfo* pItemInfo)
//end 2010. 04. 21 by jskim ˝Ĺ±Ô ·°Ĺ° ¸Ó˝Ĺ ±¸Çö
{
	ReleaseWearItemToCharacterParamFactor( nPos );
	// 2010. 04. 21 by jskim ˝Ĺ±Ô ·°Ĺ° ¸Ó˝Ĺ ±¸Çö
	//SetWearItemToCharacterParamFactor( nPos, pITEM );
	SetWearItemToCharacterParamFactor( nPos, pItemInfo );
	//end 2010. 04. 21 by jskim ˝Ĺ±Ô ·°Ĺ° ¸Ó˝Ĺ ±¸Çö
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			FieldSocketCharacterSendParamfactorInRange(int nPos)
/// \brief		ÁÖşŻŔÇ Äł¸ŻżˇĽ­ ShuttleChildŔÇ 1Çü ParamFactor¸¦ ŔüĽŰÇŃ´Ů.
/// \author		dhkwon
/// \date		2004-07-31 ~ 2004-07-31
/// \warning	ShotNum, ReattackTime¸¸ ŔűżëµČ´Ů.
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CShuttleChild::FieldSocketCharacterSendParamfactorInRange(int nPos)
{
	MSG_FC_CHARACTER_SEND_PARAMFACTOR_IN_RANGE sMsg;
	sMsg.ItemPosition = nPos;
	g_pFieldWinSocket->SendMsg( T_FC_CHARACTER_SEND_PARAMFACTOR_IN_RANGE, (char*)&sMsg, sizeof(sMsg) );
}


void CShuttleChild::SetShuttleSpeed()
{
	// 2005-11-26 by ispark
	// A±âľîŔĚ¸é ¶ĄżˇĽ­ ´Ů´Ď¸é ĽÓµµ ĽÂĆĂ
	if(IS_DT(m_myShuttleInfo.UnitKind))
	{
		

		// 2009. 08. 11 by ckPark Ćí´ë ´ëÇü ˝şĹł

		//m_fShuttleGroundSpeed = m_fShuttleGroundSpeedMax*(1.0f + m_paramFactor.pfm_SPEED);
		// ˝şĹł µĄ˝ş ĆÄ¶÷°Şµµ Ŕűżë
		m_fShuttleGroundSpeed = ( m_fShuttleGroundSpeedMax + GetParamFactor_DesParam( m_paramFactor, DES_ENGINE_GROUND_SPEED_UP ) ) * ( 1.0f + GetParamFactor_DesParam( m_paramFactor, DES_SPEED ) );

		// end 2009. 08. 11 by ckPark Ćí´ë ´ëÇü ˝şĹł


		if(m_fRealShuttleSpeedMax < 0.0f)
		{
			m_fRealShuttleSpeedMax = 0.0f;	// ĂÖĽŇ ĽÓ·Â
		}
	}

	// 2005-12-03 by ispark, żŁÁř°ú ľĆ¸Ó°ˇ ŔĺÂřµÇľî ŔÖ´ÂÁö ĆÇ´Ü
	// µŃÁßżˇ ÇĎ°ł¶óµµ ľř´Ů¸é... ˝şÇÇµĺ ĽÂĆĂ żˇ·Ż
	ITEM_GENERAL* pItemEngine = m_pStoreData->FindItemInInventoryByWindowPos( POS_REAR );
	ITEM_GENERAL* pItemArmor = m_pStoreData->FindItemInInventoryByWindowPos( POS_CENTER );
	if(pItemEngine && pItemArmor)
	{
	//	m_fRealShuttleSpeedMax = m_fShuttleSpeedMax*(1.0f + m_paramFactor.pfm_SPEED)+1000.0f;


		// 2009. 08. 11 by ckPark Ćí´ë ´ëÇü ˝şĹł

		//m_fRealShuttleSpeedMax = m_fShuttleSpeedMax*(1.0f + m_paramFactor.pfm_SPEED);
		// ˝şĹł µĄ˝ş ĆÄ¶÷°Şµµ Ŕűżë
		m_fRealShuttleSpeedMax = ( m_fShuttleSpeedMax + GetParamFactor_DesParam( m_paramFactor, DES_ENGINE_MAX_SPEED_UP ) ) * ( 1.0f + GetParamFactor_DesParam( m_paramFactor, DES_SPEED ) );
		
		// end 2009. 08. 11 by ckPark Ćí´ë ´ëÇü ˝şĹł
		
		
		if(m_fRealShuttleSpeedMax < 40.0f)
		{
			m_fRealShuttleSpeedMax = 40.0f;	// ĂÖĽŇ ĽÓ·Â
		}

	//	m_fRealShuttleSpeedBoosterOn = m_fShuttleSpeedBoosterOn*(1.0f + m_paramFactor.pfm_SPEED)+1000.0f;


		// 2009. 08. 11 by ckPark Ćí´ë ´ëÇü ˝şĹł

		//m_fRealShuttleSpeedBoosterOn = m_fShuttleSpeedBoosterOn*(1.0f + m_paramFactor.pfm_SPEED);
		// ˝şĹł µĄ˝ş ĆÄ¶÷°Şµµ Ŕűżë
		m_fRealShuttleSpeedBoosterOn = ( m_fShuttleSpeedBoosterOn + GetParamFactor_DesParam( m_paramFactor, DES_ENGINE_BOOSTER_SPEED_UP ) ) * ( 1.0f + GetParamFactor_DesParam( m_paramFactor, DES_SPEED ) );

		// end 2009. 08. 11 by ckPark Ćí´ë ´ëÇü ˝şĹł



		if(m_fRealShuttleSpeedBoosterOn < 50.0f)
		{
			m_fRealShuttleSpeedBoosterOn = 50.0f;	// ĂÖĽŇ şÎ˝şĹÍ ĽÓ·Â
		}

		if(m_pSkill->GetSmartSpeedState())
		{
			m_fRealShuttleSpeedMin = m_fRealShuttleSpeedMax * (-1);
		}
		else
		{

			// 2009. 08. 11 by ckPark Ćí´ë ´ëÇü ˝şĹł

			//m_fRealShuttleSpeedMin = m_fShuttleSpeedMin;
			// ˝şĹł µĄ˝ş ĆÄ¶÷°Şµµ Ŕűżë
			m_fRealShuttleSpeedMin = ( m_fShuttleSpeedMin + GetParamFactor_DesParam( m_paramFactor, DES_ENGINE_MIN_SPEED_UP ) );

			// 0ş¸´Ů ŔűŔ¸¸é 0Ŕ¸·Î ĽÂĆĂ(żŞ»ď°˘ ÇüĹÂ : Á¤Áö şńÇŕ)
			if( m_fRealShuttleSpeedMin <= 0.0f )
				m_fRealShuttleSpeedMin = 0.0f;

			// end 2009. 08. 11 by ckPark Ćí´ë ´ëÇü ˝şĹł
		}
	}
	else
	{
		// 2005-12-03 by ispark, żŁÁřŔĚłŞ ľĆ¸Ó°ˇ ąĚĂ˘ŔŰŔĚąÇ·Î ĽÓµµ ĂÖŔú·Î ±×¸®°í ŔŻ´Ö Á¤Áö
		m_fRealShuttleSpeedMax = 40.0f;
		m_fRealShuttleSpeedMin = 40.0f;
		m_fRealShuttleSpeedBoosterOn = 0.0f;
	}

	// 2005-12-02 by ispark, ÇĎŔĚĆŰ ą«şů, şÎ˝şĹÍ´Â şÎ˝şĹÍ ľµ ¶§
	ITEM* pItemInfo = g_pStoreData->GetItemInfoFromItemGeneral(g_pStoreData->FindItemInInventoryByWindowPos( POS_REAR ));
	if(pItemInfo)
		m_fMouseRate = CAtumSJ::GetEngineRangeAngle(pItemInfo, &m_paramFactor);


	// 2009. 09. 21 by ckPark ŔÎÇÇ´ĎĆĽ ÇĘµĺ ¸ó˝şĹÍ ˝şĹł ±¸Çö
	// Č¦µĺ »óĹÂ¸é ¸đµç ĽÓµµ 0
	if( m_pSkill->GetMonsterHoldState() )
	{
		m_fRealShuttleSpeedMax = 0.0f;
		m_fRealShuttleSpeedMin = 0.0f;
		m_fRealShuttleSpeedBoosterOn = 0.0f;

		m_fShuttleGroundSpeed = 0.0f;			// 2014-01-06 by ssjung A기어 지상에서 몬스터 홀드스킬 안먹는 버그 수정
	}
	// end 2009. 09. 21 by ckPark ŔÎÇÇ´ĎĆĽ ÇĘµĺ ¸ó˝şĹÍ ˝şĹł ±¸Çö
}

void CShuttleChild::SetParamFactorDesParam( CParamFactor &paramFactor, int nDestParam, float fDestValue)
{
	float fpm_SPEED = paramFactor.pfm_SPEED;
	SetParamFactor_DesParam( paramFactor, nDestParam, fDestValue);
	if(fpm_SPEED == paramFactor.pfm_SPEED)
	{
		SetShuttleSpeed();
	}
}

void CShuttleChild::ReleaseParamFactorDesParam( CParamFactor &paramFactor, int nDestParam, float fDestValue)
{
	float fpm_SPEED = paramFactor.pfm_SPEED;
	ReleaseParamFactor_DesParam( paramFactor, nDestParam, fDestValue);
	if(fpm_SPEED == paramFactor.pfm_SPEED)
	{
		SetShuttleSpeed();
	}
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			CShuttleChild::SetGroundTargetPos()
/// \author		jschoi
/// \date		2004-10-07 ~ 2004-10-07
/// \warning	Áö»ó Ćř°ÝŔ» ÇŇ Ŕ§Äˇ¸¦ ĽÂĆĂÇŃ´Ů.
///
/// \param		
/// \return		m_vGroundTargetPos : Áö»ó Ćř°Ý Ŕ§Äˇ
///////////////////////////////////////////////////////////////////////////////
void CShuttleChild::SetGroundTargetPos()
{
	D3DXMATRIX mat;
	D3DXVECTOR3 vVel,vTargetVel;
	D3DXVECTOR3 vGroundTargetCheckObject,vGroundTargetCheckGround;
	vVel = m_vVel;
	vVel.y = 0;
	D3DXVec3Normalize(&vVel,&vVel);

	// ĂĘ±â ĽÂĆĂ
	float fRange = m_pSecondaryWeapon->GetRealItemInfo()->Range;						// ą«±âŔÇ »ç°Ĺ¸®
	m_vGroundTargetPos = m_vPos + vVel * fRange + D3DXVECTOR3(0,-1,0) * (fRange/2) ;	// Ĺ¸ÄĎŔÇ Ŕ§Äˇ
	float fTargetDistance = D3DXVec3Length(&(m_vGroundTargetPos - m_vPos));				// Ĺ¸ÄĎ°úŔÇ °Ĺ¸®
	vGroundTargetCheckObject = m_vGroundTargetPos;
	vGroundTargetCheckGround = m_vGroundTargetPos;
	vTargetVel = m_vGroundTargetPos - m_vPos;											// Ĺ¸ÄĎŔÇ ąćÇâ
	D3DXVec3Normalize(&vTargetVel,&vTargetVel);

	// 1. Ĺ¸ÄĎąćÇâŔ¸·Î  żŔşęÁ§Ć®żÍ Ăćµą °Ë»ç
	D3DXMatrixLookAtLH(&mat,&m_vPos,&(m_vPos + vTargetVel),&D3DXVECTOR3(0,1,0));	// ĽĹĆ˛ ľŐąćÇâ
	COLLISION_RESULT collResult = g_pScene->m_pObjectRender->CheckCollMesh(mat,m_vPos, FALSE);
	if(collResult.fDist < fTargetDistance) 
	{ // 2Çü ą«±âŔÇ »ç°Ĺ¸® ľČÂĘżˇ żŔşęÁ§Ć®°ˇ ŔÖ´Ů¸é żŔşęÁ§Ć®żˇ Ăćµą
		vGroundTargetCheckObject = m_vPos + vTargetVel*collResult.fDist;	
	}

	// 2. Ĺ¸ÄĎąćÇâŔ¸·Î ÁöÇü°ú Ăćµą°Ë»ç
	float fTempDistance = 0.0f;
	D3DXVECTOR3 vTempPos;
	while (fTempDistance < fTargetDistance)	// ÁöÇü ą× ą°°ú Ăćµą °Ë»ç
	{
		vTempPos = m_vPos + vTargetVel * fTempDistance;
		if(g_pGround->CheckHeightMap(vTempPos) > vTempPos.y)
		{	// ÁöÇü ą× ą°°ú Ăćµą Çß´Ů.
			vGroundTargetCheckGround = vTempPos;
			break;
		}
		fTempDistance += TILE_SIZE;
	}

	if(D3DXVec3Length(&(vGroundTargetCheckObject - m_vPos)) > D3DXVec3Length(&(vGroundTargetCheckGround - m_vPos))) 
	{ // ÁöÇü°ú ¸ŐŔú ĂćµąÇŃ´Ů.
		m_vGroundTargetPos = vGroundTargetCheckGround;
	}
	else
	{ // żŔşęÁ§Ć®żÍ ¸ŐŔú ĂćµąÇŃ´Ů.
		m_vGroundTargetPos = vGroundTargetCheckObject;
	}
	g_pGameMain->m_pInfSkill->SetGroundTarget(m_vGroundTargetPos);
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			void CShuttleChild::SetAirGroundTargetPos()
/// \brief		°řÁß Ćř°Ý
/// \author		ispark
/// \date		2006-12-01 ~ 2006-12-01
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CShuttleChild::SetAirGroundTargetPos()
{
	// 2009. 07. 21 by ckPark °řÁßĆř°ÝÁßżˇ ·ąŔĚ´őÇŘÁ¦˝Ă Ć¨±â´Â ą®Á¦
	if( !m_pRadarItemInfo )
	{
		for( vector<CSkillInfo*>::iterator it = m_pSkill->m_vecUsingSkill.begin();
			 it != m_pSkill->m_vecUsingSkill.end();
			 ++it )
		{
			if( SKILL_BASE_NUM((*it)->ItemNum) == BGEAR_SKILL_BASENUM_AIRBOMBINGMODE )
			{
				g_pGameMain->m_pCharacterInfo->SendUseSkill( *it );
				break;
			}
		}

		return;
	}
	// end 2009. 07. 21 by ckPark °řÁßĆř°ÝÁßżˇ ·ąŔĚ´őÇŘÁ¦˝Ă Ć¨±â´Â ą®Á¦
	
	D3DXMATRIX mat;
	D3DXVECTOR3 vVel,vTargetVel;
	D3DXVECTOR3 vGroundTargetCheckObject,vGroundTargetCheckGround;
	vVel = m_vVel;
	D3DXVec3Normalize(&vVel,&vVel);

	// ĂĘ±â ĽÂĆĂ
	// ·ąŔĚ´ő 2Çü ą«±â »ç°Ĺ¸®
	float fRange = CAtumSJ::GetSecondaryRadarRange(m_pRadarItemInfo->ItemInfo, &m_paramFactor);
	m_vGroundTargetPos = m_vPos + vVel * fRange;										// Ĺ¸ÄĎŔÇ Ŕ§Äˇ
	float fTargetDistance = D3DXVec3Length(&(m_vGroundTargetPos - m_vPos));				// Ĺ¸ÄĎ°úŔÇ °Ĺ¸®
	vGroundTargetCheckObject = m_vGroundTargetPos;
	vGroundTargetCheckGround = m_vGroundTargetPos;
	vTargetVel = m_vGroundTargetPos - m_vPos;											// Ĺ¸ÄĎŔÇ ąćÇâ
	D3DXVec3Normalize(&vTargetVel,&vTargetVel);

	// 1. Ĺ¸ÄĎąćÇâŔ¸·Î  żŔşęÁ§Ć®żÍ Ăćµą °Ë»ç
	D3DXMatrixLookAtLH(&mat,&m_vPos,&(m_vPos + vTargetVel),&D3DXVECTOR3(0,1,0));	// ĽĹĆ˛ ľŐąćÇâ
	COLLISION_RESULT collResult = g_pScene->m_pObjectRender->CheckCollMesh(mat,m_vPos, FALSE);
	if(collResult.fDist < fTargetDistance) 
	{ // 2Çü ą«±âŔÇ »ç°Ĺ¸® ľČÂĘżˇ żŔşęÁ§Ć®°ˇ ŔÖ´Ů¸é żŔşęÁ§Ć®żˇ Ăćµą
		vGroundTargetCheckObject = m_vPos + vTargetVel*collResult.fDist;	
	}

	// 2. Ĺ¸ÄĎąćÇâŔ¸·Î ÁöÇü°ú Ăćµą°Ë»ç
	float fTempDistance = 0.0f;
	D3DXVECTOR3 vTempPos;
	while (fTempDistance < fTargetDistance)	// ÁöÇü ą× ą°°ú Ăćµą °Ë»ç
	{
		vTempPos = m_vPos + vTargetVel * fTempDistance;
		if(g_pGround->CheckHeightMap(vTempPos) > vTempPos.y)
		{	// ÁöÇü ą× ą°°ú Ăćµą Çß´Ů.
			vGroundTargetCheckGround = vTempPos;
			break;
		}
		fTempDistance += TILE_SIZE;
	}

	if(D3DXVec3Length(&(vGroundTargetCheckObject - m_vPos)) > D3DXVec3Length(&(vGroundTargetCheckGround - m_vPos))) 
	{ // ÁöÇü°ú ¸ŐŔú ĂćµąÇŃ´Ů.
		m_vGroundTargetPos = vGroundTargetCheckGround;
	}
	else
	{ // żŔşęÁ§Ć®żÍ ¸ŐŔú ĂćµąÇŃ´Ů.
		m_vGroundTargetPos = vGroundTargetCheckObject;
	}
	g_pGameMain->m_pInfSkill->SetAirGroundTarget(m_vGroundTargetPos);
}

void CShuttleChild::SendMoveWeaponVel()
{
	// 2009-03-16 by bhsohn A±âľî Ć÷´ë ąćÇâ ÇÁ·ÎĹäÄÝ ĂÖĽŇČ­
	DWORD dwTime = g_pD3dApp->GetCurrentGameTime();		
	if(dwTime < m_dwCheckTimeSendMoveVel)		
	{		
		m_dwCheckTimeSendMoveVel = dwTime;
		return;
	}	
	else if((dwTime-m_dwCheckTimeSendMoveVel) < SEND_MOVE_VEL_CHECK_TIME)
	{
		return;
	}
	
	m_dwCheckTimeSendMoveVel = dwTime;
	// end 2009-03-16 by bhsohn A±âľî Ć÷´ë ąćÇâ ÇÁ·ÎĹäÄÝ ĂÖĽŇČ­

	MSG_FC_MOVE_WEAPON_VEL sMsg;
	sMsg.WeaponVel = m_vWeaponVel;
	sMsg.PetLeftVel = m_vPetLeftVel;
	sMsg.PetRightVel = m_vPetRightVel;

	g_pFieldWinSocket->SendMsg( T_FC_MOVE_WEAPON_VEL, (char*)&sMsg, sizeof(sMsg) );
}

void CShuttleChild::CheckOnObject()
{
	D3DXMATRIX mat;
	D3DXVECTOR3 vSide(0,0,1),vUp(0,1,0);
	D3DXMatrixLookAtLH(&mat,&m_vPos,&(m_vPos - m_vUp),&vSide);		// ľĆ·ˇ
		
	if(	g_pScene &&
		g_pScene->m_pObjectRender)
	{	
		COLLISION_RESULT collResult = g_pScene->m_pObjectRender->CheckCollMesh(mat,m_vPos, FALSE);
		if(collResult.fDist != DEFAULT_COLLISION_DISTANCE)
		{
			m_bOnObject = TRUE;
		}
		else
		{
 			m_bOnObject = FALSE;
		}
	}
}


///////////////////////////////////////////////////////////////////////////////
/// \fn			CShuttleChild::PatternWarpIn()
/// \brief		ĆĐĹĎ żöÇÁ żŔşęÁ§Ć®żˇ ÁřŔÔ˝Ă ĆĐĹĎŔ» »ýĽşÇŃ´Ů.
/// \author		jschoi
/// \date		2004-11-27 ~ 2004-11-27
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CShuttleChild::PatternWarpIn()
{
	CObjectChild* pObj = g_pScene->FindEventObjectByIndex(m_pCurrentObjEvent->m_vPos, m_pCurrentObjEvent->m_nNextEventIndex);
	if(pObj)
	{
		D3DXVECTOR3 vVel,vUp;
		vVel = m_vPos - pObj->m_vPos;
		vUp.x = m_mMatrix._21;
		vUp.y = m_mMatrix._22;
		vUp.z = m_mMatrix._23;
		if(vVel == vUp)
		{// ¸ĹĆ®¸Ż˝ş°ˇ ±úÁöÁö ľĘµµ·Ď
			m_mMatrix._21 += 0.001f;
			m_mMatrix._22 -= 0.001f;
			m_mMatrix._23 += 0.002f;
		}
		m_mMatrix._31 = vVel.x;
		m_mMatrix._32 = vVel.y;
		m_mMatrix._33 = vVel.z;
		if(InitCinemaUnit(PATTERN_UNIT_WARP_IN, TRUE) == TRUE)
		{
			// żöÇÁ°ÔŔĚĆ® ŔĚĆĺĆ®
			CObjectChild* pWarpgate = g_pScene->FindWarpGateByPosition( m_vPos );
			if( pWarpgate )
			{
				// 2008-06-24 by dgwoo żöÇÁ °ÔŔĚĆ® Ăß°ˇ.
				if(pWarpgate->m_pObjectInfo->Code == WARP_GATE_OBJECT_NUM_2)
				{
					if(g_pD3dApp->m_pSound)
						g_pD3dApp->m_pSound->PlayD3DSound(SOUND_WARP_2, pWarpgate->m_vPos, FALSE);
				}
				else
				{
					if(g_pD3dApp->m_pSound)
						g_pD3dApp->m_pSound->PlayD3DSound(SOUND_WARP, pWarpgate->m_vPos, FALSE);
				}
				pWarpgate->ChangeBodycondition( BODYCON_WARP_MASK );
				
				// ÁÖşŻŔÇ Enemyµéżˇ°Ô żöÇÁ°ÔŔĚĆ® ąŮµđÄÁĆĽĽÇ Change¸¦ ŔüĽŰÇŃ´Ů.
				MSG_FC_OBJECT_CHANGE_BODYCONDITION sMsg;
				sMsg.AVec3Position = pWarpgate->m_vPos;
				sMsg.bodyCondition = BODYCON_WARP_MASK;
				sMsg.nObjectCodeNum = pWarpgate->m_nCode;
				g_pFieldWinSocket->SendMsg(T_FC_OBJECT_CHANGE_BODYCONDITION, (char*)&sMsg, sizeof(sMsg));
			}
			m_bSkillMoveIsUse = TRUE;
			m_nCurrentPatternNumber = PATTERN_UNIT_WARP_IN;
			InitCinemaCamera(PATTERN_CAMERA_WARP_IN);
		}
		else
		{
			DBGOUT("[PATTERN_UNIT_WARP_IN] : Pattern File Non Exsistent.\n");
		}
	}
	m_fShuttleSpeed = m_fShuttleSpeedBoosterOn;
	if(m_pCharacterInfo && !COMPARE_BODYCON_BIT(m_pCharacterInfo->m_nCurrentBodyCondition,BODYCON_BOOSTER3_MASK))
	{
		ChangeBodyConditionFromServer(BODYCON_BOOSTER3_MASK);
	}

	m_nBooster = BOOSTER_TYPE_MIN;						// 2006-09-27 by ispark, şÎ˝şĹÍ °ÔŔĚÁö ´ŢÁö ľĘ°Ô ÇĎ±â
}


///////////////////////////////////////////////////////////////////////////////
/// \fn			CShuttleChild::PatternWarpOut()
/// \brief		żöÇÁ°ÔŔĚĆ® ŔĚşĄĆ® żŔşęÁ§Ć®żˇĽ­ żöÇÁ°ˇ żĎ·áµÇ¸é, żöÇÁľĆżô ĆĐĹĎŔ» »ýĽşÇŃ´Ů.
/// \author		jschoi
/// \date		2004-11-27 ~ 2004-11-27
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CShuttleChild::PatternWarpOut(BOOL bOtherMap)
{
	// 2004-11-26 by jschoi - Ŕ§ÇÁľĆżô ˝Ă ĆĐĹĎ Ŕűżë
	CObjectChild * pCurrentObj = g_pScene->FindEventObjectByTypeAndPosition(EVENT_TYPE_WARP,m_vPos);
	if(pCurrentObj == NULL)
	{
		pCurrentObj = g_pScene->FindEventObjectByTypeAndPosition(EVENT_TYPE_WARP_TARGET,m_vPos);
	}
	if(pCurrentObj)
	{
		CObjectChild* pObjTarget = g_pScene->FindEventObjectByIndex(pCurrentObj->m_vPos, pCurrentObj->m_nNextEventIndex);
		if(pObjTarget)
		{
			if(g_pD3dApp->m_pEffectList)
			{
				CAppEffectData *pData = new CAppEffectData(pCurrentObj,RC_EFF_OTHER_WARP,D3DXVECTOR3(0,0,0));
				g_pD3dApp->m_pEffectList->AddChild(pData);
			}

			// 2006-04-25 by ispark, ŔĚşĄĆ® żŔşęÁ§Ć® Upş¤ĹÍ¸¦ °ˇÁö°í ĽÂĆĂ
//			D3DXVECTOR3 vVel,vUp(0,1,0);
			D3DXVECTOR3 vVel,vUp = pCurrentObj->m_vUp;
			vVel = m_vPos - pObjTarget->m_vPos;
			m_fWarpOutDistance = D3DXVec3Length(&vVel);
			m_vWarpOutPosition = pCurrentObj->m_vPos;
			D3DXVec3Normalize(&vVel,&vVel);
			if(vVel == vUp)
			{
				vUp.x += 0.001f;
				vUp.y -= 0.002f;
				vUp.z += 0.001f;
				D3DXVec3Normalize(&vUp,&vUp);
			}
			m_mMatrix._41 = m_vPos.x;
			m_mMatrix._42 = m_vPos.y;
			m_mMatrix._43 = m_vPos.z;
			m_mMatrix._31 = vVel.x;
			m_mMatrix._32 = vVel.y;
			m_mMatrix._33 = vVel.z;
			m_mMatrix._21 = vUp.x;
			m_mMatrix._22 = vUp.y;
			m_mMatrix._23 = vUp.z;
			g_pCamera->Init(m_vPos - 10.0f*vVel,vVel, 3.14f/13.0f, 60.0f);
			if(InitCinemaUnit(PATTERN_UNIT_WARP_OUT, TRUE) == TRUE)
			{
				// żöÇÁ°ÔŔĚĆ® ŔĚĆĺĆ®
				CObjectChild* pWarpgate = g_pScene->FindWarpGateByPosition( m_vPos );
				if( pWarpgate )
				{
					// 2008-06-24 by dgwoo żöÇÁ °ÔŔĚĆ® Ăß°ˇ.
					if(pWarpgate->m_pObjectInfo->Code == WARP_GATE_OBJECT_NUM_2 && g_pD3dApp->m_pSound)
					{
						g_pD3dApp->m_pSound->PlayD3DSound(SOUND_WARP_2, pWarpgate->m_vPos, FALSE);
					}
					else if(g_pD3dApp->m_pSound)
					{
						g_pD3dApp->m_pSound->PlayD3DSound(SOUND_WARP, pWarpgate->m_vPos, FALSE);
					}
					pWarpgate->ChangeBodycondition( BODYCON_WARP_MASK );
					
					// ÁÖşŻŔÇ Enemyµéżˇ°Ô żöÇÁ°ÔŔĚĆ® ąŮµđÄÁĆĽĽÇ Change¸¦ ŔüĽŰÇŃ´Ů.
					MSG_FC_OBJECT_CHANGE_BODYCONDITION sMsg;
					sMsg.AVec3Position = pWarpgate->m_vPos;
					sMsg.bodyCondition = BODYCON_WARP_MASK;
					sMsg.nObjectCodeNum = pWarpgate->m_nCode;
					g_pFieldWinSocket->SendMsg(T_FC_OBJECT_CHANGE_BODYCONDITION, (char*)&sMsg, sizeof(sMsg));
				}

				m_bSkillMoveIsUse = TRUE;
				m_nCurrentPatternNumber = PATTERN_UNIT_WARP_OUT;
				InitCinemaCamera(PATTERN_CAMERA_WARP_OUT);
			}
			else
			{
				DBGOUT("[PATTERN_UNIT_WARP_OUT] : Pattern File Non Exsistent.\n");
			}
		}
		
		if (bOtherMap)
		{
			// 2009. 09. 03 by ckPark ·Îşż±âľî ·Ń¸µÇĎ¸éĽ­ żöÇÁ Ĺë°ú˝Ă ŔĚĆĺĆ® ł˛ľĆŔÖ´Â ą®Á¦
			if (m_pCharacterInfo && COMPARE_BODYCON_BIT(m_pCharacterInfo->m_nCurrentBodyCondition, BODYCON_ROLLING_MASK))
				g_pD3dApp->SendFieldSocketChangeCharacterBodyCondition(BODYCON_ROLLING_MASK, FALSE);
			// end 2009. 09. 03 by ckPark ·Îşż±âľî ·Ń¸µÇĎ¸éĽ­ żöÇÁ Ĺë°ú˝Ă ŔĚĆĺĆ® ł˛ľĆŔÖ´Â ą®Á¦

			m_nShuttleStopState = 0;
			m_nBooster = BOOSTER_TYPE_MAX;
			ChangeSingleBodyCondition(BODYCON_BOOSTER2_MASK);
			ChangeBodyConditionFromServer(BODYCON_BOOSTER3_MASK);
			m_nBooster = BOOSTER_TYPE_MIN;							// 2006-09-27 by ispark, şÎ˝şĹÍ °ÔŔĚÁö ´ŢÁö ľĘ°Ô ÇĎ±â

		}
		m_fShuttleSpeed = m_fShuttleSpeedBoosterOn;		
	}
}


///////////////////////////////////////////////////////////////////////////////
/// \fn			CheckPickingTarget(CUnitData* pTarget)
/// \brief		¸¶żě˝ş Ć÷ŔÎĆ®żˇ ´ëÇŃ pTargetŔÇ ¸Ţ˝Ă PickingŔ» ĽöÇŕÇŃ´Ů.
/// \author		jschoi
/// \date		2005-03-24 ~ 2005-03-24
/// \warning	
///
/// \param		
/// \return		TRUE : ĂćµąÇŃ´Ů.(Picked)
///				FALSE : Pick ˝ÇĆĐ
///////////////////////////////////////////////////////////////////////////////
BOOL CShuttleChild::CheckPickingTarget(CUnitData* pTarget)
{
	if(pTarget == NULL)
		return FALSE;

	CSkinnedMesh* pMesh = NULL;
	if(pTarget->m_dwPartType == _ENEMY)
	{// Enemy
		CEnemyData* pEnemy = (CEnemyData*)pTarget;
		int nUnitNum = pTarget->GetUnitNum();
		map<int,CSkinnedMesh*>::iterator itEnemy = pEnemy->m_pRender->m_mapSkinnedMesh.find(nUnitNum);		
		if(itEnemy != pEnemy->m_pRender->m_mapSkinnedMesh.end())
		{
			pMesh = itEnemy->second;
		}
	}
	else if(pTarget->m_dwPartType == _MONSTER)
	{// Monster
		CMonsterData* pMon = (CMonsterData*)pTarget;
		if(pMon->m_pMonMesh)
		{
			pMesh = pMon->m_pMonMesh;
		}
	}

	if(pMesh)
	{
		D3DXMATRIX matProj,matView,matWorld,mat;
		g_pD3dDev->GetTransform(D3DTS_PROJECTION,&matProj);
		g_pD3dDev->GetTransform(D3DTS_VIEW,&matView);
		g_pD3dDev->GetTransform(D3DTS_WORLD,&matWorld);

		POINT MousePt;
		GetCursorPos(&MousePt);
		ScreenToClient(g_pD3dApp->GetHwnd(), &MousePt);
		CheckMouseReverse(&MousePt);

		D3DXVECTOR3 v,vDir,vPos,vUp;
		v.x =  (((2.0f*MousePt.x)/g_pD3dApp->GetBackBufferDesc().Width)-1)/matProj._11;
		v.y = -(((2.0f*MousePt.y)/g_pD3dApp->GetBackBufferDesc().Height)-1)/matProj._22;
		v.z = 1.0f;

		D3DXMatrixInverse(&matView,NULL,&matView);

		vDir.x = v.x*matView._11 + v.y*matView._21 + v.z*matView._31;
		vDir.y = v.x*matView._12 + v.y*matView._22 + v.z*matView._32;
		vDir.z = v.x*matView._13 + v.y*matView._23 + v.z*matView._33;

		D3DXVec3Normalize(&vDir,&vDir);

		vPos.x = matView._41;
		vPos.y = matView._42;
		vPos.z = matView._43;

		vUp.x = 0.0f;
		vUp.y = 1.0f;
		vUp.z = 0.0f;

		D3DXMatrixLookAtLH(&mat,&vPos,&(vPos + vDir),&vUp);
		
		float fRadius = pMesh->m_fRadius;

//		pMesh->Tick(pTarget->m_fCurrentTime);
//		pMesh->SetWorldMatrix(pTarget->m_mMatrix);

		float fDistance = pMesh->CheckCollision(mat,vPos,DEFAULT_COLLISION_DISTANCE,TRUE,FALSE).fDist;
		if(fDistance < DEFAULT_COLLISION_DISTANCE)
		{
			DBGOUT("fDistance : %f  \n",fDistance);
			return TRUE;
		}
	}

	return FALSE;
}

void CShuttleChild::SetEffectPos(float fElapsedTime)
{
	// ÁÂżě łŻ°ł żŁÁř Ŕ§Äˇ ĽłÁ¤
	D3DXVECTOR3 vSide;
	vSide.x = m_mMatrix._11;
	vSide.y = m_mMatrix._12;
	vSide.z = m_mMatrix._13;

	if(IS_DT(m_myShuttleInfo.UnitKind) && !m_bIsAir)
	{//4.0f


		// 2009. 07. 07 by ckPark ·Îşż±âľî żäĂ»»çÇ×(·Ń¸µ, Ľ±ĹĂČ­¸é, ą«±â, A±âľîĆ÷´ë)

////	D3DXVec3TransformCoord(&m_vLWBackPos, &m_PrimaryAttack.vSideBackPos, &m_mMatrix);
// 		D3DXVec3TransformCoord(&m_vLWBackPos, &m_PrimaryAttack.vSidePos, &m_mMatrix);
// 		D3DXVECTOR3 vTempPrimary = m_PrimaryAttack.vSideBackPos - m_PrimaryAttack.vSidePos;
// 		float fLen = D3DXVec3Length(&vTempPrimary);
// 		D3DXVec3Normalize(&m_vWeaponVel,&m_vWeaponVel);
// 		m_vLWPos = m_vLWBackPos + fLen*m_vWeaponVel;
// 
// 		//D3DXVECTOR3 vPrimaryRightPos = D3DXVECTOR3(-m_vPrimarySidePos.x, m_vPrimarySidePos.y, m_vPrimarySidePos.z);
// 		D3DXVECTOR3 vPrimaryRightBackPos = D3DXVECTOR3(-m_PrimaryAttack.vSideBackPos.x, m_PrimaryAttack.vSideBackPos.y, m_PrimaryAttack.vSideBackPos.z);
// 		D3DXVECTOR3 vRWBackPos;
// 		D3DXVec3TransformCoord(&vRWBackPos, &vPrimaryRightBackPos, &m_mMatrix);
// 		m_vRWPos = vRWBackPos + fLen*m_vWeaponVel;

		// ąß»ç Ŕ§ÄˇŔÇ vSideBackPosżˇĽ­ °ˇÁ®żŔłŞ
		// yĂŕŔş ľĆ¸Ó Ŕ§ÄˇżˇĽ­ °ˇÁ®żÂ´Ů(vSidePos)
		D3DXVECTOR3 vPrimaryLeftBackPos = m_PrimaryAttack.vSideBackPos;
		vPrimaryLeftBackPos.y = m_PrimaryAttack.vSidePos.y;
		
		// żŢÂĘ ąß»ç Ŕ§Äˇ
		D3DXVec3TransformCoord( &m_vLWBackPos, &vPrimaryLeftBackPos, &m_mMatrix );
		D3DXVECTOR3 vTempPrimary = vPrimaryLeftBackPos - m_PrimaryAttack.vSidePos;
		
		float fLen = D3DXVec3Length( &vTempPrimary );
		D3DXVec3Normalize( &m_vWeaponVel,&m_vWeaponVel );
		m_vLWPos = m_vLWBackPos + fLen * m_vWeaponVel;
		
		// żŔ¸ĄÂĘ ąß»ç Ŕ§Äˇ
		D3DXVECTOR3 vPrimaryRightBackPos = vPrimaryLeftBackPos;
		vPrimaryRightBackPos.x = -vPrimaryRightBackPos.x;
		
		D3DXVECTOR3 vRWBackPos;
		D3DXVec3TransformCoord( &vRWBackPos, &vPrimaryRightBackPos, &m_mMatrix) ;
		m_vRWPos = vRWBackPos + fLen * m_vWeaponVel;

		// end 2009. 07. 07 by ckPark ·Îşż±âľî żäĂ»»çÇ×(·Ń¸µ, Ľ±ĹĂČ­¸é, ą«±â, A±âľîĆ÷´ë)
		//m_vRWPos += 3.0f*m_vWeaponVel;
		D3DXVec3TransformCoord(&m_vLWSecondaryPos, &m_SecondaryAttack.vSidePos, &m_mMatrix);
		D3DXVECTOR3 vSecondaryRightPos = D3DXVECTOR3(-m_SecondaryAttack.vSidePos.x, m_SecondaryAttack.vSidePos.y, m_SecondaryAttack.vSidePos.z);
		D3DXVec3TransformCoord(&m_vRWSecondaryPos, &vSecondaryRightPos, &m_mMatrix);

		// 2010-06-15 by shcho&hslee Ćę˝Ă˝şĹŰ - ą«±â Ŕ§Äˇ
		D3DXVec3TransformCoord(&m_vPetLeftPos, &m_PetAttack.vSidePos, &m_mMatrix);
		D3DXVECTOR3 vPetRightPos = D3DXVECTOR3(-m_PetAttack.vSidePos.x,
												m_PetAttack.vSidePos.y, 
												m_PetAttack.vSidePos.z);
		D3DXVec3TransformCoord(&m_vPetRightPos, &vPetRightPos, &m_mMatrix);
		// end 2010-06-15 by shcho&hslee Ćę˝Ă˝şĹŰ - ą«±â Ŕ§Äˇ
	}

	// ˝şĹ©¸°»óŔÇ ÁÂÇĄ¸¦ ±¸ÇŃ´Ů.
	g_pD3dApp->CalcObjectSourceScreenCoords(m_vPos, g_pD3dApp->GetBackBufferDesc().Width, g_pD3dApp->GetBackBufferDesc().Height, 
		m_nObjScreenX, m_nObjScreenY,m_nObjScreenW);
	// Ä«¸Ţ¶ó·ÎşÎĹÍŔÇ °Ĺ¸®
	m_fDistanceCamera = D3DXVec3Length(&(m_vPos - g_pD3dApp->m_pCamera->GetEyePt()));
	// ±×¸˛ŔÚ Ŕ§Äˇ ĽłÁ¤
#ifdef  _OLD_SHADOW_SYSTEM
	CheckShadowPos();
#endif
	m_bCollObjectCheck = 0;				// ĂĘ±âČ­
	m_bCollMonsterCheck = 0;
	m_bCollShuttleCheck = 0;
	m_bCollAttackCheck = 0;			// ĂĘ±âČ­
	// effect matrix & ticking
	if(m_pCharacterInfo)
	{
		m_pCharacterInfo->SetAllBodyConditionMatrix( m_mMatrix );
		m_pCharacterInfo->Tick(fElapsedTime);
	}

}


///////////////////////////////////////////////////////////////////////////////
/// \fn			RollingSystem()
/// \brief		·Ń¸µ ˝Ă˝şĹŰ(Ĺ°¸¦ °Ë»öČÄ ±×żˇ ¸Â´Â ľÖ´Ď¸ŢŔĚĽÇ ÇĎµĺÄÚµů)
///				Č¸Ŕü°ŞŔ» ¸®ĹĎ
///				(´Ü, 0Ŕş ŔĚµżŔĚ ľřŔ» ¶§ ľ´´Ů.)
/// \author		ispark
/// \date		2005-07-05 ~ 2005-07-05
/// \warning	
///
/// \param		void
/// \return		float
///////////////////////////////////////////////////////////////////////////////
float CShuttleChild::RollingSystem(float fElapsedTime)
{
	// AĹ° ÁÂĂř ·Ń¸µ
	if(m_bKeyDownA)
	{
		m_fRollTime += fElapsedTime;
		if(m_fRollTime >= ROLLING_USE_TIME)					// ·Ń¸µ ÁöĽÓ˝Ă°Ł
		{
			m_bKeyDownA = FALSE;

			m_bMouseMoveLock = FALSE;
			m_bRollUsed = FALSE;
			m_fCurrentSideRate = 0.0f;
			if(g_pTutorial->IsTutorialMode() == TRUE && g_pTutorial->GetChapter() == L3_2 && g_pTutorial->GetLesson() >= L3)
			{
				g_pTutorial->m_Lesson3State.bLRolling = TRUE;
			}

			// 2009. 07. 07 by ckPark ·Îşż±âľî żäĂ»»çÇ×(·Ń¸µ, Ľ±ĹĂČ­¸é, ą«±â, A±âľîĆ÷´ë)
			g_pD3dApp->SendFieldSocketChangeCharacterBodyCondition( BODYCON_ROLLING_MASK, FALSE );
			// end 2009. 07. 07 by ckPark ·Îşż±âľî żäĂ»»çÇ×(·Ń¸µ, Ľ±ĹĂČ­¸é, ą«±â, A±âľîĆ÷´ë)

			return 0.0f;
		}

	
//		if(80 <= D3DXVec3Length(&(m_vRollStart - m_vPos)))
		{
			m_fRollAngle += 360 * fElapsedTime;

			if(D3DXToRadian(360) <= D3DXToRadian(m_fRollAngle))
			{
				m_bKeyDownA = FALSE;

				m_bMouseMoveLock = FALSE;
				m_bRollUsed = FALSE;
				m_fCurrentSideRate = 0.0f;
				if(g_pTutorial->IsTutorialMode() == TRUE && g_pTutorial->GetChapter() == L3_2 && g_pTutorial->GetLesson() >= L3)
				{
					g_pTutorial->m_Lesson3State.bLRolling = TRUE;
				}
	
				// 2006-07-14 by ispark, ·Ń¸µ˝Ă Ćí´ë şńÇŕ ÇüĹÂ żř»óşą±Í
				if(m_pClientParty->GetPartyInfo().bPartyType == _PARTYMASTER &&
					m_pClientParty->IsFormationFlight())
				{
					g_pShuttleChild->m_pClientParty->ISendPartyChangeFlightFormation(m_byFormationFlightType);
				}

				// 2009. 07. 07 by ckPark ·Îşż±âľî żäĂ»»çÇ×(·Ń¸µ, Ľ±ĹĂČ­¸é, ą«±â, A±âľîĆ÷´ë)
				g_pD3dApp->SendFieldSocketChangeCharacterBodyCondition( BODYCON_ROLLING_MASK, FALSE );
				// end 2009. 07. 07 by ckPark ·Îşż±âľî żäĂ»»çÇ×(·Ń¸µ, Ľ±ĹĂČ­¸é, ą«±â, A±âľîĆ÷´ë)

				return 0.0f;
			}
			else
			{
				return D3DXToRadian(m_fRollAngle);
			}
		}
//		else
//		{
//			return D3DXToRadian(m_fRollAngle);
//		}
	}
	
	// DĹ° żěĂř ·Ń¸µ
	if(m_bKeyDownD)
	{
		m_fRollTime += fElapsedTime;
		if(m_fRollTime >= ROLLING_USE_TIME)					// ·Ń¸µ ÁöĽÓ ˝Ă°Ł
		{
			m_bKeyDownD = FALSE;

			m_bMouseMoveLock = FALSE;
			m_bRollUsed = FALSE;
			m_fCurrentSideRate = 0.0f;
			if(g_pTutorial->IsTutorialMode() == TRUE && g_pTutorial->GetChapter() == L3_2 && g_pTutorial->GetLesson() >= L3)
			{
				g_pTutorial->m_Lesson3State.bRRolling = TRUE;
			}

			// 2009. 07. 07 by ckPark ·Îşż±âľî żäĂ»»çÇ×(·Ń¸µ, Ľ±ĹĂČ­¸é, ą«±â, A±âľîĆ÷´ë)
			g_pD3dApp->SendFieldSocketChangeCharacterBodyCondition( BODYCON_ROLLING_MASK, FALSE );
			// end 2009. 07. 07 by ckPark ·Îşż±âľî żäĂ»»çÇ×(·Ń¸µ, Ľ±ĹĂČ­¸é, ą«±â, A±âľîĆ÷´ë)

			return 0.0f;
		}

	
//		if(80 <= D3DXVec3Length(&(m_vRollStart - m_vPos)))
		{
			m_fRollAngle += 360 * fElapsedTime;

			if(D3DXToRadian(360) <= D3DXToRadian(m_fRollAngle))
			{
				m_bKeyDownD = FALSE;

				m_bMouseMoveLock = FALSE;
				m_bRollUsed = FALSE;
				m_fCurrentSideRate = 0.0f;
				if(g_pTutorial->IsTutorialMode() == TRUE && g_pTutorial->GetChapter() == L3_2 && g_pTutorial->GetLesson() >= L3)
				{
					g_pTutorial->m_Lesson3State.bRRolling = TRUE;
				}
				
				// 2006-07-14 by ispark, ·Ń¸µ˝Ă Ćí´ë şńÇŕ ÇüĹÂ żř»óşą±Í
				if(m_pClientParty->GetPartyInfo().bPartyType == _PARTYMASTER &&
					m_pClientParty->IsFormationFlight())
				{
					g_pShuttleChild->m_pClientParty->ISendPartyChangeFlightFormation(m_byFormationFlightType);
				}
				
				// 2009. 07. 07 by ckPark ·Îşż±âľî żäĂ»»çÇ×(·Ń¸µ, Ľ±ĹĂČ­¸é, ą«±â, A±âľîĆ÷´ë)
				g_pD3dApp->SendFieldSocketChangeCharacterBodyCondition( BODYCON_ROLLING_MASK, FALSE );
				// end 2009. 07. 07 by ckPark ·Îşż±âľî żäĂ»»çÇ×(·Ń¸µ, Ľ±ĹĂČ­¸é, ą«±â, A±âľîĆ÷´ë)

				return 0.0f;
			}
			else
			{
				return -D3DXToRadian(m_fRollAngle);
			}
		}
//		else
//		{
//			return -D3DXToRadian(m_fRollAngle);
// 		}
	}

	return 0.0f;
}


///////////////////////////////////////////////////////////////////////////////
/// \fn			CheckRollKeyDown()
/// \brief		´őşíĹ¬¸ŻŔ¸·Î ·Ń¸µŔ¸·Î şŻ°ć
/// \author		ispark
/// \date		2005-07-05 ~ 2005-07-05, 2005-09-26
/// \warning	
///
/// \param		void
/// \return		void
///////////////////////////////////////////////////////////////////////////////
void CShuttleChild::CheckRollKeyDown(float fElapsedTime)
{
	// ´őşíĹ¬¸Ż ˝Ă°Ł
	m_fADoubleKeyTime += fElapsedTime;
	m_fDDoubleKeyTime += fElapsedTime;
	
	// 2008-11-13 by bhsohn ·Ń¸µ şŻĽö żŔąö ÇĂ·Îżě łŻĽö ŔÖ´Â ĽŇÁö Á¦°Ĺ
	// ·Ń¸µ şŻĽö żŔąö ÇĂ·Îżě łŻĽö ŔÖ´Â ĽŇÁö Á¦°Ĺ
	if(m_fADoubleKeyTime > 100000.0f)
	{
		m_fADoubleKeyTime = 0.5f;
	}
	if(m_fDDoubleKeyTime > 100000.0f)
	{
		m_fDDoubleKeyTime = 0.5f;
	}
	// ľî´Ŕ ÇĎłŞ¶óµµ TRUEŔĚ¸é ÇÔĽö¸¦ ľ˛Áö ľĘ´Â´Ů. 
	if(m_bKeyDownA || m_bKeyDownD)
	{
		return;
	}
	else if(g_pD3dApp->m_bChatMode ||										// Ă¤ĆĂ ¸đµĺ
		// 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
			//g_pD3dApp->m_pKeyBoard->GetAsyncKeyState(DIK_SPACE) ||			// şÎ˝şĹÍ »çżë 
			g_pD3dApp->GetAsyncKeyState_DIK_DIJ(DIK_SPACE) ||			// şÎ˝şĹÍ »çżë 
			g_pD3dApp->m_bFixUnitDirMode ||									// ŔŻ´Ö °íÁ¤˝Ă
			g_pGameMain->m_pSystem->m_bQuitGameMessage ||					// ¸Ţ´ş»çżë˝Ă ŔŻ´ÖÁ¤Áö żÉĽÇ ĂĽĹ©
			g_pD3dApp->m_bMessageBox ||										// ¸ŢĽĽÁö ąÚ˝ş ¶ąŔ» ¶§
			m_dwState != _NORMAL  ||										// şńÇŕÁßŔĚ ľĆ´Ď¶ó¸é 
			m_bUnitStop == TRUE ||											// 2005-08-08 by ispark ŔŻ´ÖÁ¤Áö˝Ă »çżë ¸řÇÔ
			m_bPenaltyUnitStop == TRUE)										// 2005-08-10 by ispark ĆäłÎĆĽ˝Ă »çżë ±ÝÁö
	{
		return;
	}

	if(m_bKeyDownACheck || m_bKeyDownDCheck)
	{
		// ŔĚąĚ ş¸ł˝ ¸ŢĽĽÁö ŔĚąÇ·Î ´ő ŔĚ»ó ş¸ł»Áö ľĘ´Â´Ů.
		m_fRollMessageTime += fElapsedTime;

		// 2ĂĘ ČÄżˇ´Â ş¸łľ Ľö ŔÖ´Ů.
		if(2.0f >= m_fRollMessageTime)
			return;
	
		m_fRollMessageTime = 0.0f;
		m_bKeyDownACheck = FALSE;
		m_bKeyDownDCheck = FALSE;
	}



	// 2009. 09. 21 by ckPark ŔÎÇÇ´ĎĆĽ ÇĘµĺ ¸ó˝şĹÍ ˝şĹł ±¸Çö
	// Č¦µĺ°ˇ °É·ČŔ»¶§´Â ·Ń¸µ ¸řÇÔ
	if( m_pSkill->IsExistDesParamSKill( DES_SKILL_MON_HOLD ) )
		return;
	// end 2009. 09. 21 by ckPark ŔÎÇÇ´ĎĆĽ ÇĘµĺ ¸ó˝şĹÍ ˝şĹł ±¸Çö



	// ´őşíĹ¬¸Ż łˇ ĂĽĹ©
	// AĹ° ÁÂĂř ·Ń¸µ
	if(m_bKeyDownADouble == TRUE)
	{
		m_bKeyDownACheck = TRUE;
		m_bKeyDownADouble = FALSE;

		// ·Ń¸µ ĆĐĹ¶
		INIT_MSG_WITH_BUFFER(MSG_FC_MOVE_ROLLING, T_FC_MOVE_ROLLING, pSRolling, SendBuf);
		pSRolling->PositionAVec3	= m_vPos;
		pSRolling->TargetAVec3		= m_vVel * 1000.0f;
		D3DXVECTOR3 vUp;
		vUp.x = m_mMatrix._21;
		vUp.y = m_mMatrix._22;
		vUp.z = m_mMatrix._23;
		pSRolling->UpAVec3			= vUp * 1000.0f;
		pSRolling->byLeftDirectionFlag	= TRUE;
		g_pFieldWinSocket->Write(SendBuf, MSG_SIZE(MSG_FC_MOVE_ROLLING));
//		DBGOUT("ĆĐĹ¶ ş¸łÂľű~~~\n");
		return;
	}

	// DĹ° ÁÂĂř ·Ń¸µ
	if(m_bKeyDownDDouble == TRUE)
	{
		m_bKeyDownDCheck = TRUE;
		m_bKeyDownDDouble = FALSE;

		// ·Ń¸µ ĆĐĹ¶
		INIT_MSG_WITH_BUFFER(MSG_FC_MOVE_ROLLING, T_FC_MOVE_ROLLING, pSRolling, SendBuf);
		pSRolling->PositionAVec3	= m_vPos;
		pSRolling->TargetAVec3		= m_vVel * 1000.0f;
		D3DXVECTOR3 vUp;
		vUp.x = m_mMatrix._21;
		vUp.y = m_mMatrix._22;
		vUp.z = m_mMatrix._23;
		pSRolling->UpAVec3			= vUp * 1000.0f;
		pSRolling->byLeftDirectionFlag	= FALSE;
		g_pFieldWinSocket->Write(SendBuf, MSG_SIZE(MSG_FC_MOVE_ROLLING));
//		DBGOUT("ĆĐĹ¶ ş¸łÂľű~~~\n");
		return;
	}

}


///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		
/// \author		ispark
/// \date		2005-07-07 ~ 2005-07-07
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CShuttleChild::CheckRoll(BYTE byDir)
{
//	DBGOUT("ĆĐĹ¶ ąŢľŇľű~~~\n");
	if(byDir)
	{
		m_bKeyDownA = TRUE;
	}
	else
	{
		m_bKeyDownD = TRUE;
	}

	m_fRollTime = 0.0f;	
	m_bKeyDownACheck = m_bKeyDownDCheck = FALSE;
	m_fRollMessageTime = 0.0f;
	m_fRollAngle = 0.0f;
	m_vRollStart = m_vRollOldPos = m_vPos;
	m_bMouseMoveLock = TRUE;
	m_fRollDownSpeed = m_fShuttleSpeed * (float)(2.0f / 3.0f);
	m_bRollUsed = TRUE;

	if(m_pClientParty->GetPartyInfo().bPartyType == _PARTYMASTER &&
		m_pClientParty->IsFormationFlight())
	{
		g_pShuttleChild->m_pClientParty->ISendPartyChangeFlightFormation(FLIGHT_FORM_NONE);
	}
	m_byFormationFlightType = g_pShuttleChild->m_pClientParty->GetPartyInfo().bFormationFlyingType;

	// 2009. 07. 07 by ckPark ·Îşż±âľî żäĂ»»çÇ×(·Ń¸µ, Ľ±ĹĂČ­¸é, ą«±â, A±âľîĆ÷´ë)
	// ·ÎşżľĆ¸ÓŔĎ °ćżě
	if( IsRobotArmor() )
	{
		// şÎ˝şĹÍ ľÖ´Ď¸ŢŔĚĽÇ°ú ·Ń¸µ ąŮµđÄÁĽÇŔ» ´Ů łˇł˝µÚ, »ő·Î ˝ĂŔŰÇŃ´Ů
		g_pD3dApp->SendFieldSocketChangeCharacterBodyCondition( BODYCON_BOOSTER1_MASK, FALSE );
		g_pD3dApp->SendFieldSocketChangeCharacterBodyCondition( BODYCON_BOOSTER2_MASK, FALSE );
		g_pD3dApp->SendFieldSocketChangeCharacterBodyCondition( BODYCON_BOOSTER3_MASK, FALSE );
		g_pD3dApp->SendFieldSocketChangeCharacterBodyCondition( BODYCON_BOOSTER4_MASK, FALSE );
		g_pD3dApp->SendFieldSocketChangeCharacterBodyCondition( BODYCON_BOOSTER5_MASK, FALSE );
		g_pD3dApp->SendFieldSocketChangeCharacterBodyCondition( BODYCON_ROLLING_MASK, FALSE );
	}
	g_pD3dApp->SendFieldSocketChangeCharacterBodyCondition( BODYCON_ROLLING_MASK, TRUE );
	// end 2009. 07. 07 by ckPark ·Îşż±âľî żäĂ»»çÇ×(·Ń¸µ, Ľ±ĹĂČ­¸é, ą«±â, A±âľîĆ÷´ë)
}


///////////////////////////////////////////////////////////////////////////////
/// \fn			CShuttleChild::InitRolling()
/// \brief		·Ń¸µ ĂĘ±âČ­
/// \author		ispark
/// \date		2005-11-15 ~ 2005-11-15
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CShuttleChild::InitRolling()
{
	m_bKeyDownA = FALSE;			// A ÁÂĂř ·Ń¸µ
	m_bKeyDownD = FALSE;			// D żěĂř ·Ń¸µ
	m_bKeyDownACheck = FALSE;		// A ÁÂĂř »çżë Çă°ˇ ĂĽĹ©
	m_bKeyDownDCheck = FALSE;		// D żěĂř »çżë Çă°ˇ ĂĽĹ©
	m_bKeyDownADouble = FALSE;		// A ´őşíĹ¬¸Ż
	m_bKeyDownDDouble = FALSE;		// D ´őşíĹ¬¸Ż
	m_fADoubleKeyTime = 0.5f;			// A ´őşíĹ° Ĺ¸ŔÓ
	m_fDDoubleKeyTime = 0.5f;			// D ´őşíĹ° Ĺ¸ŔÓ
	m_fRollTime = 0.0f;				// ·Ń¸µ Ĺ¸ŔÓ
	m_fRollAngle = 0.0f;			// ·Ń¸µ Č¸Ŕü °˘µµ
	m_fRollLength = 0.0f;			// ·Ń¸µ °Ĺ¸® ĂĽĹ©
	m_fRollDownSpeed = 0.0f;
	m_fRollMessageTime = 0.0f;		// ·Ń¸µ ¸ŢĽĽÁö
	m_bMissileWarning = FALSE;		// ¸¶»çŔĎ °ć°í
	m_bRollUsed = FALSE;			// 2005-07-14 by ispark
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			CShuttleChild::GetIsUseInterface()
/// \brief		ŔÎĹÍĆäŔĚ˝ş »çżë °ˇ´ÉÇŃ°ˇ? ±âľî »óĹÂżˇ µű¶ó ´Ů¸§
/// \author		ispark
/// \date		2005-10-26 ~ 2005-10-26
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CShuttleChild::GetIsUseInterface()
{
	// 1. ±âľî »óĹÂŔĚ´Ů
	// 2. BĹ° ±âľî Á¤Áö°ˇ ľĆ´Ď´Ů
	// 3. Âř·ú »óĹÂ ľĆ´Ď´Ů
	// 4. A±âľî Ground »óĹÂ°ˇ ľĆ´Ď´Ů
	// ŔÎĹÍĆäŔĚ˝ş »çżë ±ÝÁö
	if(g_pD3dApp->m_bCharacter == FALSE && 
	   g_pShuttleChild->m_bUnitStop == FALSE && 
	   g_pShuttleChild->m_bLandedMove == FALSE &&
	   m_bIsAir == TRUE)
	   return FALSE;

	return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
/// \fn			CShuttleChild::CheckCollForObject2()
/// \brief		żŔşęÁ§Ć® °Ë»ç LANDED »óĹÂ ŔĎ ¶§¸¸
/// \author		ispark
/// \date		2005-10-26 ~ 2005-10-26
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CShuttleChild::CheckCollForObject2(float fElapsedTime)
{
	COLLISION_RESULT CollResult;
	D3DXMATRIX matArray[2];
	D3DXVECTOR3 vVel,vFrontPos, vBackPos;
	
	vFrontPos = m_vPos;			// »ç¶÷ Ĺ°łôŔĚ¸¦ »ý°˘ÇĎŔÚ
	vBackPos = m_vPos;			// Ŕýş®¸¦ °č»ęÇĎ±â Ŕ§ÇŘĽ­ ł» Ŕ§Äˇ ş¸´Ů ´ő ľŐÂĘ ÁÂÇĄ ŔÔ·Â
	vBackPos += m_vVel * -5;

	D3DXMatrixLookAtLH(&matArray[0],&vFrontPos,&(vFrontPos + m_vVel),&m_vUp);		// ľŐ
	D3DXMatrixLookAtLH(&matArray[1],&vBackPos,&(vBackPos - m_vVel),&m_vUp);		// µÚ
	
	float fFrontMove = m_fShuttleSpeed*fElapsedTime;
	float size[2] = {25 + fFrontMove, 25 + fFrontMove};	// ŔŻ´Ö Ĺ©±â	ľŐ, ľĆ·ˇ
	
	// żŔşęÁ§Ć®żÍ Ăćµą Ăł¸® 2004.06.18 jschoi
	
	if(	/*m_bCollObjectCheck && */
		g_pScene &&
		g_pScene->m_byMapType != MAP_TYPE_TUTORIAL &&
		g_pScene->m_pObjectRender)
	{
		// Á¤¸é
		CollResult = g_pScene->m_pObjectRender->CheckCollMesh(matArray[0], m_vPos);	
		// 2005-07-29 by ispark
		// żŔşęÁ§Ć®ľČżˇĽ­ łŞżŔÁö ¸řÇĎ´Â ąö±× żążÜ Ăł¸®(łŞżŔ°Ô ÇÔ)
		float dot = D3DXVec3Dot(&m_vVel, &CollResult.vNormalVector);
//		if(0.0f >= dot && g_pD3dApp->m_pKeyBoard->GetAsyncKeyState(DIK_W))
		if(0.0f >= dot)
		{
			if(CollResult.fDist < size[0])
			{
				m_vPos = m_vMyOldPos;
//				m_vPos = m_vOldPos;
//				m_vMyOldPos = m_vOldPos;
				m_fFrameLerp = 1.0f;
			}
		}

		// µÚ
		CollResult = g_pScene->m_pObjectRender->CheckCollMesh(matArray[1], m_vPos);
		dot = D3DXVec3Dot(&(-m_vVel), &CollResult.vNormalVector);
//		if(0.0f >= dot && g_pD3dApp->m_pKeyBoard->GetAsyncKeyState(DIK_W))
		if(0.0f >= dot)
		{
			if(CollResult.fDist < size[1])
			{
				m_vPos = m_vMyOldPos;
//				m_vPos = m_vOldPos;
//				m_vMyOldPos = m_vOldPos;
				m_fFrameLerp = 1.0f;
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
/// \class		CShuttleChild::SetUnLockOn()
///
/// \brief		
/// \author		ispark
/// \version	
/// \date		2005-10-19 ~ 2005-10-19
/// \warning	
///////////////////////////////////////////////////////////////////////////////
void CShuttleChild::SetUnLockOn()
{
	if(m_pOrderTarget)
	{
		if(	m_pOrderTarget->m_dwPartType == _ENEMY &&						// 1. żŔ´őĹ¸°ŮŔĚ Enemyż´°í,
//			!IsInfluenceCharacter(m_myShuttleInfo.InfluenceType, ((CEnemyData*)m_pOrderTarget)->m_infoCharacter.CharacterInfo.InfluenceType) && // 2. Ľ­·Î ´Ů¸Ą ĽĽ·ÂŔĚ¶ó¸é
//			((CEnemyData*)m_pOrderTarget)->GetPkState() == PK_NORMAL &&		// 3. ĽřĽö °­Á¦ Ĺ¸°ŮŔĚľú´Ů¸é.
//			((CEnemyData*)m_pOrderTarget)->GetbPkAttack() &&				// 4. ĽřĽö °­Á¦ Ĺ¸°ŮŔĚľú´Ů¸é.
			g_pShuttleChild->m_pPkNormalTimer->IsCityWar() == FALSE)		// 3. µµ˝Ă Áˇ·ÉŔüÁßżˇ´Â ş¸ł»Áö ľĘ´Â´Ů.
		{	// Ľ­ąö·Î ş¸łż
//			DBGOUT("ľđ¶ôżÂ ş¸łż\n");
			MSG_FC_MOVE_UNLOCKON sMsg;
			sMsg.AttackIndex = m_myShuttleInfo.ClientIndex;
			sMsg.TargetIndex = ((CEnemyData*)m_pOrderTarget)->m_infoCharacter.CharacterInfo.ClientIndex;	
			g_pFieldWinSocket->SendMsg( T_FC_MOVE_UNLOCKON, (char*)&sMsg, sizeof(sMsg) );				
		}
		g_pD3dApp->m_pChat->CreateChatChild(STRMSG_C_INTERFACE_0001,COLOR_SKILL_USE,CHAT_TAB_SYSTEM);
//				m_pTarget = NULL;
		m_pOrderTarget = NULL;
	}
}


///////////////////////////////////////////////////////////////////////////////
/// \fn			CShuttleChild::IsEnemyPKTarget(int nTagetIndex)
/// \brief		
/// \author		ispark
/// \date		2005-10-19 ~ 2005-10-19
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CShuttleChild::IsEnemyPKTarget(int nTagetIndex)
{
	// 2005-11-03 by ispark
	// Enemy Ĺ¸°ŮŔĚ ŔÖ´ÂÁö °Ë»ç
	CMapEnemyIterator it = g_pScene->m_mapEnemyList.find(nTagetIndex);
	if(it != g_pScene->m_mapEnemyList.end())
	{
//		int index = it->second->m_nTargetIndex;
//		if(it->second->m_nTargetIndex == g_pShuttleChild->m_myShuttleInfo.ClientIndex)
			return TRUE;
	}

	return FALSE;
}


///////////////////////////////////////////////////////////////////////////////
/// \fn			CShuttleChild::IsEnemyPKAttackTime(int nTagetIndex)
/// \brief		
/// \author		ispark
/// \date		2005-11-03 ~ 2005-11-03
/// \warning	
///
/// \param		
/// \return		BOOL
///////////////////////////////////////////////////////////////////////////////
BOOL CShuttleChild::IsEnemyPKAttackTime(int nTagetIndex)
{
	// 2005-11-03 by ispark
	// Enemy °ř°Ý °ˇ´É °Ë»ç
	CMapEnemyIterator it = g_pScene->m_mapEnemyList.find(nTagetIndex);
	if(it != g_pScene->m_mapEnemyList.end())
	{
		// µµ˝ĂÁˇ·ÉŔü ÁßŔĚ¸é °ř°Ý °ˇ´É. ˝Ă°Ł°ú °ü°č ľřŔ˝
		if(it->second->m_pPkNormalTimer->IsPkEnableNormalOrderTarget() || m_pPkNormalTimer->IsCityWar())
			return TRUE;
	}

	return FALSE;
}

BOOL CShuttleChild::WarpSkipInOutAirPort()
{
	if(m_bWarpLink == TRUE)
	{
		switch(m_nEventTypeAirPort)
		{
		case AIR_PORT_LANDING_PATTERN_START:
		case AIR_PORT_LANDING_PATTERN_END:
			{
				m_nEventTypeAirPort = AIR_PORT_LANDING_PATTERN_END;
				DeletePattern();
				ChangeUnitState( _LANDING );
				m_pCurrentObjEvent = m_pCurrentObjEventOld;
				m_nEventIndex = m_pCurrentObjEvent->m_nNextEventIndex;

				CObjectChild* pObj = g_pScene->FindObjectByIndex(m_nEventIndex);
				m_nEventIndex = pObj->m_nNextEventIndex;
				m_bWarpLink = FALSE;
				m_bLandingField = TRUE;
				m_bSkipPattern = TRUE;
			}
			return TRUE;
		case AIR_PORT_TAKEOFF_PATTERN_START:
		case AIR_PORT_TAKEOFF_PATTERN_END:
			{
				m_nEventTypeAirPort = AIR_PORT_TAKEOFF_PATTERN_END;
				DeletePattern();
				ChangeUnitState( _NORMAL );
				m_bWarpLink = FALSE;
				m_bKeyBlock = FALSE;
				// 2007-06-12 by bhsohn ąĚĽÇ ŔÎĹÍĆäŔĚ˝ş ĽöÁ¤ľČ
//				if(g_pGameMain->m_pMissionInfo->GetMissionMapIndex())
//				{
//					INIT_MSG_WITH_BUFFER(MSG_FC_QUEST_MOVE_QUEST_MAP, T_FC_QUEST_MOVE_QUEST_MAP,
//						pSQuestMove, SendBuf);
//					pSQuestMove->QuestIndex0 = g_pGameMain->m_pMissionInfo->GetMissionIndex()->QuestIndex;
//					g_pFieldWinSocket->Write(SendBuf, MSG_SIZE(MSG_FC_QUEST_MOVE_QUEST_MAP));
//				}
				
				// 2007.09.19 by bhsohn ŔüÁř ±âÁö Ăâ°Ý Ăł¸®
				//SendGoMissionMap();
				BOOL bGoMissionMap = SendGoMissionMap();
				if(FALSE == bGoMissionMap)
				{
					// ąĚĽÇ¸Ę ľČ ş¸ł»¸é Ăâ°Ý ąßµż
					CObjectChild* pObj = g_pScene->FindEventObjectByWarp();
					if(pObj)
					{
						SendFieldSocketRequestEventObjectWarpIn(pObj);
					}
				}				
				// end 2007.09.19 by bhsohn ŔüÁř ±âÁö Ăâ°Ý Ăł¸®
			}
			return TRUE;
		}
	}

	return FALSE;
}


///////////////////////////////////////////////////////////////////////////////
/// \fn			CShuttleChild::InitCharacterToShuttleData()
/// \brief		Äł¸ŻĹÍ°ˇ ĽĹĆ˛ÇŃĹ× ĂĘ±âČ­ ˝ĂĹ°´Â ÇÔĽö. Äł¸ŻĹÍ¸¸ ľ´´Ů.
/// \author		ispark
/// \date		2005-11-10 ~ 2005-11-10
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CShuttleChild::InitCharacterToShuttleData()
{
	m_pTarget = NULL;
	m_pOrderTarget = NULL;

	if(m_pPrimaryWeapon)
	{
		// 2011. 03. 08 by jskim ŔÎÇÇ3Â÷ ±¸Çö - łÍ Ĺ¸°Ů ˝Ă˝şĹŰ
		//m_pPrimaryWeapon->m_vecTargetIndex.clear();
		m_pPrimaryWeapon->m_vecTargetIndexData.clear();
		// end 2011. 03. 08 by jskim ŔÎÇÇ3Â÷ ±¸Çö - łÍ Ĺ¸°Ů ˝Ă˝şĹŰ
	}
	if(m_pSecondaryWeapon)
	{
		// 2011. 03. 08 by jskim ŔÎÇÇ3Â÷ ±¸Çö - łÍ Ĺ¸°Ů ˝Ă˝şĹŰ
		//m_pSecondaryWeapon->m_vecTargetIndex.clear();
		m_pSecondaryWeapon->m_vecTargetIndexData.clear();
		// end 2011. 03. 08 by jskim ŔÎÇÇ3Â÷ ±¸Çö - łÍ Ĺ¸°Ů ˝Ă˝şĹŰ
	}
	
	SetMissileCount(0);						// 2005-08-10 by ispark
	SetMissileWarning(FALSE);

	m_nBooster = BOOSTER_TYPE_STOP;
	SendFieldSocketChangeBodyCondition(g_pShuttleChild->m_myShuttleInfo.ClientIndex, BODYCON_LANDED_MASK);
	SendFieldSocketChangeBodyCondition(g_pShuttleChild->m_myShuttleInfo.ClientIndex, BODYCON_BOOSTER5_MASK|BODYCON_SET_OR_CLEAR_MASK);
	CheckBodyConditionToServer(g_pShuttleChild->m_myShuttleInfo.BodyCondition);
	ChangeSingleBodyCondition(BODYCON_BOOSTER5_MASK);
}


///////////////////////////////////////////////////////////////////////////////
/// \fn			CShuttleChild::InitUnitStop()
/// \brief		ŔŻ´ÖÁ¤Áö »óĹÂ¸¦ Ç¬´Ů. 
/// \author		ispark
/// \date		2005-11-17 ~ 2005-11-17
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CShuttleChild::InitUnitStop()
{
	m_bUnitStop = FALSE;
	m_bPenaltyUnitStop = FALSE;
	m_bMouseMoveLock = FALSE;
	m_nBooster = BOOSTER_TYPE_MIN;
	m_bMouseLock = FALSE;

	g_pD3dApp->m_bFixUnitDirMode = FALSE;



	if(g_pInterface && g_pInterface->m_pGameMain)
		g_pInterface->m_pGameMain->InitShowWindow();			// 2005-08-05 by ispark
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			void CShuttleChild::UnitStop()
/// \brief		ŔŻ´ÖŔ» Á¤Áö ˝ĂĹ˛´Ů.
/// \author		dgwoo
/// \date		2007-05-11 ~ 2007-05-11
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CShuttleChild::UnitStop()
{
	if (m_bSkillMoveIsUse)//cannot do bstop because using skill BMM or TurnAround
	{
		g_pD3dApp->m_pChat->CreateChatChild("Can not stop now !", COLOR_ERROR, CHAT_TAB_SYSTEM);
		return;
	}
	//g_pShuttleChild->m_bUnitStop = !g_pShuttleChild->m_bUnitStop;
	g_pShuttleChild->m_bUnitStop = TRUE;
	g_pShuttleChild->m_bMouseLock = TRUE;
	g_pShuttleChild->m_bMouseMoveLock = TRUE;
#ifndef _DISABLE_BSTOP_AUTOHORIZONT //28-07-2024 by Inet - dont align gear at B stop
	g_pD3dApp->m_bFixUnitDirMode = FALSE;
#else
	g_pD3dApp->m_bFixUnitDirMode = TRUE;
#endif
	// 2010. 03. 30 by jskim ŔŻ´Ö »óĹÂ ąŮµđÄÁµđĽÇ Ľ­ąö Ŕü´Ţ	
	SendFieldSocketChangeBodyCondition( m_myShuttleInfo.ClientIndex, BODYCON_STOP_MASK | BODYCON_SET_OR_CLEAR_MASK);
	//end 2010. 03. 30 by jskim ŔŻ´Ö »óĹÂ ąŮµđÄÁµđĽÇ Ľ­ąö Ŕü´Ţ
	// 2008-11-06 by bhsohn ¸¶żě˝ş °ˇµÎ±â ¸đµĺ ş¸żĎ
	g_pD3dApp->RefreshMouseMode();
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			CShuttleChild::SetFormationMove()
/// \brief		Ćí´ëşńÇŕ˝Ă ąćÇâ ŔüČŻ. Ĺ¸°ŮŔ» ¸ÂĂß±â Ŕ§ÇŃ ĽöÁ¤. ´Ü Ćí´ëŔĺŔş Á¦żÜ
/// \author		ispark
/// \date		2005-11-21 ~ 2005-11-21
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CShuttleChild::SetFormationMove(float fElapsedTime)
{
	int	nx = g_pD3dApp->GetBackBufferDesc().Width/2;
	int	ny = g_pD3dApp->GetBackBufferDesc().Height/2;

	D3DXMATRIX  matTemp;
	
	// Ćí´ëŔĺŔş ŔÚŔŻŔÚŔç·Î Á¶Á¤ŔĚ °ˇ´ÉÇĎąÇ·Î Ćí´ëŔĺŔĚ ľĆ´Ń°ćżě¸¸ ÇŃ´Ů.
	if(m_pClientParty->GetPartyInfo().bPartyType != _PARTYMASTER)
	{
		if((m_ptDir.x - m_ptDirOld.x) > 0)
		{
			m_ptDirOld.x += FORMATION_MOVE_RATE;
			if(m_ptDir.x < m_ptDirOld.x)
				m_ptDirOld.x = m_ptDir.x;
		}
		else if((m_ptDir.x - m_ptDirOld.x) < 0)
		{
			m_ptDirOld.x += -FORMATION_MOVE_RATE;
			if(m_ptDir.x > m_ptDirOld.x)
				m_ptDirOld.x = m_ptDir.x;
		}
		
		if((m_ptDir.y - m_ptDirOld.y) > 0)
		{
			m_ptDirOld.y += FORMATION_MOVE_RATE;
			if(m_ptDir.y < m_ptDirOld.y)
				m_ptDirOld.y = m_ptDir.y;
		}
		else if((m_ptDir.y - m_ptDirOld.y) < 0)
		{
			m_ptDirOld.y += -FORMATION_MOVE_RATE;
			if(m_ptDir.y > m_ptDirOld.y)
				m_ptDirOld.y = m_ptDir.y;
		}

		// »óÇĎ
		float fAngle = (float)(FORMATION_UPDOWN_RANGE * m_ptDir.y) / ny;
		if(fAngle > FORMATION_UPDOWN_RANGE)
			fAngle = FORMATION_UPDOWN_RANGE;
		else if(fAngle < -FORMATION_UPDOWN_RANGE)
			fAngle = -FORMATION_UPDOWN_RANGE;

		m_fCurrentAngle = (float)(FORMATION_UPDOWN_RANGE * m_ptDirOld.y) / ny;
		if(m_fCurrentAngle > FORMATION_UPDOWN_RANGE)
			m_fCurrentAngle = FORMATION_UPDOWN_RANGE;
		else if(m_fCurrentAngle < -FORMATION_UPDOWN_RANGE)
			m_fCurrentAngle = -FORMATION_UPDOWN_RANGE;

		D3DXMatrixRotationAxis(&matTemp,&m_vSideVel, m_fMouseRate*m_fCurrentAngle);
		D3DXVec3TransformCoord(&m_vVel,&m_vVel, &matTemp );

		// ÁÂżě
		m_fSideRate = (float)(FORMATION_SIDE_RANGE * m_ptDir.x) / nx;
		if(m_fSideRate > FORMATION_SIDE_RANGE)
			m_fSideRate = FORMATION_SIDE_RANGE;
		else if(m_fSideRate < -FORMATION_SIDE_RANGE)
			m_fSideRate = -FORMATION_SIDE_RANGE;

		m_fCurrentSideRate = (float)(FORMATION_SIDE_RANGE * m_ptDirOld.x) / nx;
		if(m_fCurrentSideRate > FORMATION_SIDE_RANGE)
			m_fCurrentSideRate = FORMATION_SIDE_RANGE;
		else if(m_fCurrentSideRate < -FORMATION_SIDE_RANGE)
			m_fCurrentSideRate = -FORMATION_SIDE_RANGE;

		D3DXMatrixRotationAxis(&matTemp, &m_vUp, m_fMouseRate*m_fCurrentSideRate);
		D3DXVec3TransformCoord(&m_vVel, &m_vVel, &matTemp);
	}
}

// 2006-01-20 by ispark, »čÁ¦
//BOOL CShuttleChild::IsInfluenceWarType(int nEnemyIndex)
//{
//	if( !COMPARE_INFLUENCE(nEnemyIndex, m_myShuttleInfo.InfluenceType) &&
//		!COMPARE_INFLUENCE(nEnemyIndex,		INFLUENCE_TYPE_NORMAL) && 
//		!COMPARE_INFLUENCE(m_myShuttleInfo.InfluenceType,	INFLUENCE_TYPE_NORMAL))
//	{
//		return TRUE;
//	}
//
//	return FALSE;	
//}

///////////////////////////////////////////////////////////////////////////////
/// \fn			CShuttleChild::CheckWearItem()
/// \brief		ŔĺÂř ľĆŔĚĹŰ ĂĽĹ© : şĐ´ç ĂĽĹ©
/// \author		ispark
/// \date		2005-12-29 ~ 2005-12-29
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CShuttleChild::CheckWearItem(float fElapsedTime)
{
	char buf[256];
	m_fCheckWearItemTime += fElapsedTime;
	if(60.0f <= m_fCheckWearItemTime)
	{
		m_fCheckWearItemTime = 0.0f;

		ITEM_GENERAL* pCheckRearWearItem = m_pStoreData->FindItemInInventoryByWindowPos(POS_REAR);
		if(pCheckRearWearItem == NULL)
		{
			wsprintf(buf,STRMSG_C_051203_0001);	// "żŁÁřŔĚ ŔĺÂřµÇľî ŔÖÁö ľĘ˝Ŕ´Ď´Ů."
			g_pD3dApp->m_pChat->CreateChatChild(buf,COLOR_SYSTEM);			
		}
		
		ITEM_GENERAL* pCheckCenterWearItem = m_pStoreData->FindItemInInventoryByWindowPos(POS_CENTER);
		if(pCheckCenterWearItem == NULL)
		{
			wsprintf(buf,STRMSG_C_051229_0100);	// "ľĆ¸Ó°ˇ ŔĺÂřµÇľî ŔÖÁö ľĘ˝Ŕ´Ď´Ů."
			g_pD3dApp->m_pChat->CreateChatChild(buf,COLOR_SYSTEM);			
		}
	
		ITEM_GENERAL* pCheckRadarWearItem = m_pStoreData->FindItemInInventoryByWindowPos(POS_PROW);
		if(pCheckRadarWearItem == NULL)
		{
			wsprintf(buf,STRMSG_C_051229_0101);	// "·ąŔĚ´ő°ˇ ŔĺÂřµÇľî ŔÖÁö ľĘ˝Ŕ´Ď´Ů."
			g_pD3dApp->m_pChat->CreateChatChild(buf,COLOR_SYSTEM);			
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			CShuttleChild::CheckDamageBodyCondition()
/// \brief		±âÁ¸ Damage Ăł¸®¸¦ żˇłĘÁö »óĹÂ¸¦ ąŢŔ¸¸é ±×¶§ ÇŃąř Ăł¸®¸¦ şŻČŻŔĚ ŔÖŔ»¶§ Ăł¸®·Î şŻ°ć
/// \author		ispark
/// \date		2005-12-30 ~ 2005-12-30
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CShuttleChild::CheckDamageBodyCondition()
{
	int nDamageState = 0;
	// 2007-05-31 by bhsohn şÎČ°˝Ă ±âĂĽ şŇĹ¸´Â Çö»ó Ăł¸®
//	if(m_myShuttleInfo.CurrentHP / m_myShuttleInfo.HP < CHARACTER_DAMAGE_STATE_05)
//	{
//		nDamageState = 3;
//	}
//	else if(m_myShuttleInfo.CurrentHP / m_myShuttleInfo.HP < CHARACTER_DAMAGE_STATE_10)
//	{
//		nDamageState = 2;
//	}
//	else if(m_myShuttleInfo.CurrentHP / m_myShuttleInfo.HP < CHARACTER_DAMAGE_STATE_15)
//	{
//		nDamageState = 1;
//	}
//	else
//	{
//		nDamageState = 0;
//	}
	float fCurrentHP = m_fNextHP;

	if(fCurrentHP / m_myShuttleInfo.HP < CHARACTER_DAMAGE_STATE_05)
	{
		nDamageState = 3;
	}
	else if(fCurrentHP / m_myShuttleInfo.HP < CHARACTER_DAMAGE_STATE_10)
	{
		nDamageState = 2;
	}
	else if(fCurrentHP / m_myShuttleInfo.HP < CHARACTER_DAMAGE_STATE_15)
	{
		nDamageState = 1;
	}
	else
	{
		nDamageState = 0;
	}
	// end 2007-05-31 by bhsohn şÎČ°˝Ă ±âĂĽ şŇĹ¸´Â Çö»ó Ăł¸®

	if(m_nCheckDamage != nDamageState)
	{
		m_nCheckDamage = nDamageState;

		if(m_nCheckDamage == 0)
		{
			// µĄąĚÁö şńČ°ĽşČ­
			g_pD3dApp->SendFieldSocketChangeCharacterBodyCondition( BODYCON_DAMAGE1_MASK, FALSE );
			g_pD3dApp->SendFieldSocketChangeCharacterBodyCondition( BODYCON_DAMAGE2_MASK, FALSE );
			g_pD3dApp->SendFieldSocketChangeCharacterBodyCondition( BODYCON_DAMAGE3_MASK, FALSE );
			g_pGameMain->m_pUnitState->OffState(INF_UW_DANGER_MASK);
		}
		else if(m_nCheckDamage == 1)
		{
			// µĄąĚÁö 1ąř Č°ĽşČ­
			g_pD3dApp->SendFieldSocketChangeCharacterBodyCondition( BODYCON_DAMAGE1_MASK, TRUE );
			g_pD3dApp->SendFieldSocketChangeCharacterBodyCondition( BODYCON_DAMAGE2_MASK, FALSE );
			g_pD3dApp->SendFieldSocketChangeCharacterBodyCondition( BODYCON_DAMAGE3_MASK, FALSE );
			g_pGameMain->m_pUnitState->OffState(INF_UW_DANGER_MASK);
		}
		else if(m_nCheckDamage == 2)
		{
			// µĄąĚÁö 1,2ąř Č°ĽşČ­
			g_pD3dApp->SendFieldSocketChangeCharacterBodyCondition( BODYCON_DAMAGE1_MASK, TRUE );
			g_pD3dApp->SendFieldSocketChangeCharacterBodyCondition( BODYCON_DAMAGE2_MASK, TRUE );
			g_pD3dApp->SendFieldSocketChangeCharacterBodyCondition( BODYCON_DAMAGE3_MASK, FALSE );
			g_pGameMain->m_pUnitState->InsertState(INF_UW_DANGER_MASK);
			if(g_pD3dApp->m_bCharacter == FALSE)
				if(g_pD3dApp->m_pSound)
					g_pD3dApp->m_pSound->PlayD3DSound(SOUND_WARNING, D3DXVECTOR3(0,0,0), FALSE);
		}
		else if(m_nCheckDamage == 3)
		{
			// µĄąĚÁö 1,2,3ąř ¸đµÎ Č°ĽşČ­
			g_pD3dApp->SendFieldSocketChangeCharacterBodyCondition( BODYCON_DAMAGE1_MASK, TRUE );
			g_pD3dApp->SendFieldSocketChangeCharacterBodyCondition( BODYCON_DAMAGE2_MASK, TRUE );
			g_pD3dApp->SendFieldSocketChangeCharacterBodyCondition( BODYCON_DAMAGE3_MASK, TRUE );
			g_pGameMain->m_pUnitState->InsertState(INF_UW_DANGER_MASK);
			if(g_pD3dApp->m_bCharacter == FALSE)
				if(g_pD3dApp->m_pSound)
					g_pD3dApp->m_pSound->PlayD3DSound(SOUND_WARNING_HP, D3DXVECTOR3(0,0,0), FALSE);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			CEnemyData::RemoveEnemyTarget()
/// \brief		Ŕű Ĺ¸°Ů »čÁ¦
/// \author		ispark
/// \date		2006-01-13 ~ 2006-01-13
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CShuttleChild::RemoveEnemyTarget()
{
	CMapEnemyIterator it = g_pScene->m_mapEnemyList.begin();
	while(it != g_pScene->m_mapEnemyList.end())
	{
		it->second->m_nTargetIndex = 0;
		it++;
	}
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			void CShuttleChild::InitMissileWarning()
/// \brief		ąĚ»çŔĎ °ć°í ĂĘ±âČ­
/// \author		ispark
/// \date		2006-02-13 ~ 2006-02-13
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CShuttleChild::InitMissileWarning()
{
	m_bMissileWarning = FALSE;
	m_nMissileCount = 0;

	if(g_pD3dApp->m_pSound)
	{
		g_pD3dApp->m_pSound->StopD3DSound(SOUND_MISSILE_WARNNING);
	}
}

void CShuttleChild::SetInvenMesh(int nIndex, CSkinnedMesh* pMesh)
{ 

	if(FALSE == IS_IN_RANGE(0, nIndex, 3))
	{
		return;
	}
	// 2007-11-09 by dgwoo ą«±â ¸Ţ˝¬¸¦ ąčż­żˇĽ­ ş¤ĹÍ·Î şŻ°ć.
	int nSize = m_vectInvenWeaponMesh.size();
	int i=0;
	for(i=0; i < nSize; i++)
	{
		if(m_vectInvenWeaponMesh[i].nMatIndex == nIndex
			&& m_vectInvenWeaponMesh[i].pInvenWeaponMesh == pMesh)
		{
			return;
		}
	}

//	DBGOUT("SetInvenMesh index = %d pMesh = %x\n",nIndex,pMesh);
	INVEN_WEAPON_MESH tmMesh;
	tmMesh.nMatIndex		= nIndex;
	tmMesh.pInvenWeaponMesh	= pMesh;
	m_vectInvenWeaponMesh.push_back(tmMesh);
}

void CShuttleChild::InitInvenMesh()
{
	// 2007-11-09 by dgwoo ą«±â ¸Ţ˝¬¸¦ ąčż­żˇĽ­ ş¤ĹÍ·Î şŻ°ć.
	m_vectInvenWeaponMesh.clear();
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			void CShuttleChild::SendDeadMsg()
/// \brief		±âľî Á×Ŕ» ¶§ ±âş» ¸ŢĽĽÁö
/// \author		ispark
/// \date		2006-04-10 ~ 2006-04-10
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CShuttleChild::SendDeadMsg()
{
	// 2009. 12. 04 by jskim ŔŻ´Ö »ç¸Á˝Ă ŔÎĹÍĆäŔĚ˝ş¸¦ ľřľÖ¸é ±ÍČŻ ¸Ţ˝ĂÁö ąÚ˝şµµ ľřľîÁö´Â ąö±× ĽöÁ¤
	vector<CINFMessageBox*>::iterator it = g_pGameMain->m_pInfWindow->m_vecMessageBox.begin();
	while(it != g_pGameMain->m_pInfWindow->m_vecMessageBox.end())
	{
		if ( (*it)->m_nMsgType == _Q_DEAD_RETURN
			|| (*it)->m_nMsgType == _Q_FUELALLIN_DEAD_RETURN
			|| (*it)->m_nMsgType == _Q_INFLUENCEMAP_DEAD_RETURN
			|| (*it)->m_nMsgType == _Q_REVIVE_WAIT_5SECOND )
			return;
		else
			it++;
	}
	//end 2009. 12. 04 by jskim ŔŻ´Ö »ç¸Á˝Ă ŔÎĹÍĆäŔĚ˝ş¸¦ ľřľÖ¸é ±ÍČŻ ¸Ţ˝ĂÁö ąÚ˝şµµ ľřľîÁö´Â ąö±× ĽöÁ¤

	// 2009. 11. 02 by ckPark ŔÎÇÇ´ĎĆĽ ÇĘµĺ ŔÎ˝şĹĎ˝ş ´řÁŻ ˝Ă˝şĹŰ
	// ŔÎÇÇ »ç¸Á˝Ă 5ĂĘ şÎČ°˝Ă˝şĹŰ
	if( g_pGameMain->m_pInfWindow->IsExistMsgBox( _Q_REVIVE_WAIT_5SECOND )
		|| g_pGameMain->m_pInfWindow->IsExistMsgBox( _Q_ARENA_RESTART ) )
		return;
	// end 2009. 11. 02 by ckPark ŔÎÇÇ´ĎĆĽ ÇĘµĺ ŔÎ˝şĹĎ˝ş ´řÁŻ ˝Ă˝şĹŰ

	MAP_INFO* pMapInfo = g_pDatabase->GetMapInfo(m_myShuttleInfo.MapChannelIndex.MapIndex);
	if(m_dwState == _FUELALLIN)
	{
		g_pGameMain->m_pInfWindow->AddMsgBox(STRMSG_C_CARD_0003, _Q_FUELALLIN_DEAD_RETURN);//"°Ýłł°í·Î ŔĚµżÇŐ´Ď´Ů."		
	}
	// 2006-08-08 by ispark, ŔĚşĄĆ® ¸Ę
	else if(IS_MAP_INFLUENCE_EVENT_AREA(pMapInfo->MapInfluenceType))
	{
		g_pGameMain->m_pInfWindow->AddMsgBox(STRMSG_C_060102_0100, _Q_INFLUENCEMAP_DEAD_RETURN); //"µµ˝Ă·Î ±ÍČŻÇĎ°Ú˝Ŕ´Ď´Ů."		
	}


	// 2009. 11. 02 by ckPark ŔÎÇÇ´ĎĆĽ ÇĘµĺ ŔÎ˝şĹĎ˝ş ´řÁŻ ˝Ă˝şĹŰ

// 	// 2007-05-07 by dgwoo ľĆ·ąłŞ ¸ĘŔĎ°ćżě Ăł¸®.
// 	else if(IS_MAP_INFLUENCE_ARENA(pMapInfo->MapInfluenceType))
// 	{
// 		if(!g_pInterface->m_pGameArena->IsArenaGameState(ARENA_GAME_STATE_RESULT))
// 			g_pGameMain->m_pInfWindow->AddTimeMsgBox(STRMSG_C_070507_0100,_Q_ARENA_RESTART,30,STRMSG_C_070507_0101);// "şÎČ°ÇĎ˝Ă°Ú˝Ŕ´Ď±î?"
// 	}
	else if( IS_MAP_INFLUENCE_ARENA( pMapInfo->MapInfluenceType )
			 || IS_MAP_INFLUENCE_INFINITY( pMapInfo->MapInfluenceType ) )
	{
		// ŔÎÇÇ »ç¸Á˝Ă 5ĂĘ şÎČ°˝Ă˝şĹŰ
		if( !g_pInterface->m_pGameArena->IsArenaGameState( ARENA_GAME_STATE_RESULT ) )
			g_pGameMain->m_pInfWindow->AddTimeMsgBox( " ", _Q_REVIVE_WAIT_5SECOND, 5, STRMSG_C_091103_0313 );// "\\r%dĂĘ\\r ČÄżˇ şÎČ°ŔĚ °ˇ´ÉÇŐ´Ď´Ů."
	}

	// end 2009. 11. 02 by ckPark ŔÎÇÇ´ĎĆĽ ÇĘµĺ ŔÎ˝şĹĎ˝ş ´řÁŻ ˝Ă˝şĹŰ


	// 2007-09-28 by dgwoo ¸đĽ±Ŕü ÁřÇŕÁßŔĚ¸é ĹÚ·ąĆ÷Ć® ż©şÎ¸¦ ąŻ´Â´Ů.
	// 2009. 10. 14 by jskim ÇÁ¸®˝şÄ« Á¦°Ĺ
	//else if(g_pD3dApp->IsWarMotherShip() && IS_OTHER_INFLUENCE_MAP(m_myShuttleInfo.InfluenceType, pMapInfo->MapIndex, pMapInfo->MapInfluenceType))
	else if(g_pD3dApp->IsWarMotherShip() && CAtumSJ::IsOtherInfluenceMap(m_myShuttleInfo.InfluenceType, pMapInfo->MapIndex, pMapInfo->MapInfluenceType))
	//end 2009. 10. 14 by jskim ÇÁ¸®˝şÄ« Á¦°Ĺ
	{
		g_pGameMain->m_pInfWindow->AddMsgBox(STRMSG_C_070928_0100, _Q_DEAD_RETURN);//"µµ˝Ă¸ĘŔ¸·Î ±ÍČŻÇĎ˝Ă°Ú˝Ŕ´Ď±î? (˝ÂŔÎ:µµ˝Ă¸Ę, ĂëĽŇ:ĹÚ·ąĆ÷Ć®)"
	}
	else if(!EnableRevivalMap(m_myShuttleInfo.InfluenceType, m_myShuttleInfo.MapChannelIndex.MapIndex))
	{
		// 2006-01-02 by ispark, ´Ů¸Ą ĽĽ·Â¸ĘŔĚąÇ·Î ŔÚ±â ĽĽ·Â µµ˝Ă¸ĘŔ¸·Î ±ÍČŻ(ÇöŔç ¸Ężˇ şÎČ° ľČÇÔ)
		g_pGameMain->m_pInfWindow->AddMsgBox(STRMSG_C_060102_0100, _Q_INFLUENCEMAP_DEAD_RETURN); //"µµ˝Ă·Î ±ÍČŻÇĎ°Ú˝Ŕ´Ď´Ů."
	}
	// 2006-08-08 by ispark, ´Ů¸Ą ĽĽ·Â ŔĎąÝ¸Ę(ŔĚŔü¸ĘŔ¸·Î şÎČ° °ˇ´É
	// 2009. 10. 14 by jskim ÇÁ¸®˝şÄ« Á¦°Ĺ
	//else if(g_pD3dApp->IsWarMotherShip() && IS_OTHER_INFLUENCE_MAP((m_myShuttleInfo.InfluenceType, pMapInfo->MapInfluenceType))
	//else if(CAtumSJ::IsOtherInfluenceMap(m_myShuttleInfo.InfluenceType, pMapInfo->MapIndex, pMapInfo->MapInfluenceType))
	// 2009. 11. 11 by jskim ÇÁ¸®˝şÄ« Á¦°Ĺ ĽöÁ¤
	else if(g_pD3dApp->IsWarMotherShip() && CAtumSJ::IsOtherInfluenceMap(m_myShuttleInfo.InfluenceType, pMapInfo->MapIndex, pMapInfo->MapInfluenceType))
	//end 2009. 11. 11 by jskim ÇÁ¸®˝şÄ« Á¦°Ĺ ĽöÁ¤
	//end 2009. 10. 14 by jskim ÇÁ¸®˝şÄ« Á¦°Ĺ
	{
		g_pGameMain->m_pInfWindow->AddMsgBox(STRMSG_C_060808_0000, _Q_DEAD_RETURN);//"µµ˝Ă¸ĘŔ¸·Î ±ÍČŻÇĎ˝Ă°Ú˝Ŕ´Ď±î? (˝ÂŔÎ:µµ˝Ă¸Ę, ĂëĽŇ:ŔĚŔü¸Ę)"
	}
	else
	{
		g_pGameMain->m_pInfWindow->AddMsgBox(STRMSG_C_051205_0005, _Q_DEAD_RETURN);//"µµ˝Ă¸ĘŔ¸·Î ±ÍČŻÇĎ˝Ă°Ú˝Ŕ´Ď±î? (Č®ŔÎ:µµ˝Ă¸Ę, ĂëĽŇ:ÇöŔç¸Ę)"
		g_pD3dApp->m_bDeadByP2PPK = FALSE;
	}
	// 2008-06-17 by bhsohn Ćí´ë °ü·Ă Ăł¸®
	// Á×ľúŔ»‹š ş¸ł˝´Ů.
	if(m_pClientParty && m_pClientParty->GetPartyInfo().bPartyType == _PARTYMASTER 
		&& m_pClientParty->IsFormationFlight())
	{		
		m_pClientParty->TempPartyFormation(FLIGHT_FORM_NONE);
	}

}

void CShuttleChild::SetBackView(BOOL bUpDown)
{
	// 2006-05-03 by ispark, ąéşä
	// Ć©Ĺä¸®ľó ¸đµĺżŔ ¤ż˝ĂÁî¸đµĺ˝Ăżˇ´Â ąéşä »çżë ±ÝÁö
	if(g_pTutorial->IsTutorialMode() == FALSE && m_bAttackMode != _SIEGE)
	{
		// DOWN
		if(bUpDown == TRUE)
		{
			m_bTurnCamera = TRUE;
			m_bMouseLock  = TRUE;					
			m_fOldUpRate = 0.0f;							
		}
		// UP
		else if(bUpDown == FALSE)
		{
			m_bTurnCamera = FALSE;
			m_bMouseLock  = FALSE;
			m_fOldUpRate = 0.0f;
			// 2007-03-26 by dgwoo żÉŔúąö ¸đµĺ˝Ă´Â MouseLock Ŕ» °Ç´Ů.
			if(IsObserverMode())
			{
				m_bMouseLock = TRUE;
			}
			// 2014-04-23 by ymjoo 정지 상태에서 백뷰 이후 우클릭 카메라 이동 안되는 버그 수정
			if (m_bUnitStop)
			{
				m_bMouseLock = TRUE;
			}
			// END 2014-04-23 by ymjoo 정지 상태에서 백뷰 이후 우클릭 카메라 이동 안되는 버그 수정
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			CShuttleChild::SetSelectCharacterItem()
/// \brief		Ľ±ĹĂĂ˘żˇ °Ą¶§ ÇöŔç ł» Äł¸ŻĹÍ Á¤ş¸ ŔÔ·Â
/// \author		ispark
/// \date		2006-05-23 ~ 2006-05-23
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CShuttleChild::SetSelectCharacterItem()
{
	// 2007-11-22 by bhsohn ľĆ·ąłŞ ĹëÇŐĽ­ąö
	if(g_pD3dApp->GetArenaState() == ARENA_STATE_ARENA_GAMING )
	{
		// ľĆ·ąłŞ żˇĽ­ °ÔŔÓ Áľ·á˝Ă, Ŕüżˇ ŔÖ´ř Á¤ş¸ °ˇÁ®żČ.		
		CHARACTER* pMainInfo = g_pD3dApp->GetMFSMyShuttleInfo();
		// °ÔŔÓÁ¤ş¸¸¦ ´Ů˝Ă °»˝Ĺ 
		g_pD3dApp->SetMyShuttleInfo(pMainInfo);		

	}
	// end 2007-11-22 by bhsohn ľĆ·ąłŞ ĹëÇŐĽ­ąö	

	FC_CONNECT_LOGIN_INFO info;
	memset(&info, 0x00, sizeof(FC_CONNECT_LOGIN_INFO));
	strcpy(info.CharacterName, g_pD3dApp->m_pShuttleChild->m_myShuttleInfo.CharacterName);
	info.CharacterUniqueNumber = g_pD3dApp->m_pShuttleChild->m_myShuttleInfo.CharacterUniqueNumber;
	info.Gender = g_pD3dApp->m_pShuttleChild->m_myShuttleInfo.Gender;
	info.PilotFace = g_pD3dApp->m_pShuttleChild->m_myShuttleInfo.PilotFace;
	info.Race = g_pD3dApp->m_pShuttleChild->m_myShuttleInfo.Race;
	info.UnitKind = g_pD3dApp->m_pShuttleChild->m_myShuttleInfo.UnitKind;
	// 2009. 11. 3 by jskim Äł˝¬(ŔÎşĄ/Ă˘°í Č®Ŕĺ) ľĆŔĚĹŰ Ăß°ˇ ±¸Çö
	info.RacingPoint = g_pD3dApp->m_pShuttleChild->m_myShuttleInfo.RacingPoint;
	//end 2009. 11. 3 by jskim Äł˝¬(ŔÎşĄ/Ă˘°í Č®Ŕĺ) ľĆŔĚĹŰ Ăß°ˇ ±¸Çö
//	info.CharacterRenderInfo.RI_ProwOut = GetCreateUnitWeapon11(pMsg->Character.UnitKind);
//	info.CharacterRenderInfo.RI_Rear = GetCreateUnitEngine(pMsg->Character.UnitKind);
	CHARACTER_RENDER_INFO CharacterRenderInfo;
	memset(&CharacterRenderInfo, 0x00, sizeof(CHARACTER_RENDER_INFO));
	CItemInfo* pInfo;
	pInfo = g_pShuttleChild->m_pStoreData->FindItemInInventoryByWindowPos(POS_PROWOUT);
	CharacterRenderInfo.RI_ProwOut = 0;


	// 2009. 08. 27 by ckPark ±×·ˇÇČ ¸®ĽŇ˝ş şŻ°ć ˝Ă˝şĹŰ ±¸Çö

	CharacterRenderInfo.RI_ProwOut_ShapeItemNum	 = 0;
	CharacterRenderInfo.RI_ProwOut_EffectItemNum = 0;

	// end 2009. 08. 27 by ckPark ±×·ˇÇČ ¸®ĽŇ˝ş şŻ°ć ˝Ă˝şĹŰ ±¸Çö


	if(pInfo)
	{
		CharacterRenderInfo.RI_ProwOut = pInfo->ItemNum;

		// 2009. 08. 27 by ckPark ±×·ˇÇČ ¸®ĽŇ˝ş şŻ°ć ˝Ă˝şĹŰ ±¸Çö

		CharacterRenderInfo.RI_ProwOut_ShapeItemNum	 = pInfo->GetShapeItemNum();
		CharacterRenderInfo.RI_ProwOut_EffectItemNum = pInfo->GetEffectItemNum();

		// end 2009. 08. 27 by ckPark ±×·ˇÇČ ¸®ĽŇ˝ş şŻ°ć ˝Ă˝şĹŰ ±¸Çö
	}

	pInfo = g_pShuttleChild->m_pStoreData->FindItemInInventoryByWindowPos(POS_WINGOUT);
	CharacterRenderInfo.RI_WingOut = 0;

	// 2009. 08. 27 by ckPark ±×·ˇÇČ ¸®ĽŇ˝ş şŻ°ć ˝Ă˝şĹŰ ±¸Çö

	CharacterRenderInfo.RI_WingOut_ShapeItemNum		= 0;
	CharacterRenderInfo.RI_WingOut_EffectItemNum	= 0;

	// end 2009. 08. 27 by ckPark ±×·ˇÇČ ¸®ĽŇ˝ş şŻ°ć ˝Ă˝şĹŰ ±¸Çö


	if(pInfo)
	{
		CharacterRenderInfo.RI_WingOut = pInfo->ItemNum;

		// 2009. 08. 27 by ckPark ±×·ˇÇČ ¸®ĽŇ˝ş şŻ°ć ˝Ă˝şĹŰ ±¸Çö

		CharacterRenderInfo.RI_WingOut_ShapeItemNum		= pInfo->GetShapeItemNum();
		CharacterRenderInfo.RI_WingOut_EffectItemNum	= pInfo->GetEffectItemNum();

		// end 2009. 08. 27 by ckPark ±×·ˇÇČ ¸®ĽŇ˝ş şŻ°ć ˝Ă˝şĹŰ ±¸Çö
	}

	pInfo = g_pShuttleChild->m_pStoreData->FindItemInInventoryByWindowPos(POS_CENTER);
	CharacterRenderInfo.RI_Center = 0;
	
	// 2009. 08. 27 by ckPark ±×·ˇÇČ ¸®ĽŇ˝ş şŻ°ć ˝Ă˝şĹŰ ±¸Çö

	//CharacterRenderInfo.RI_ArmorColorCode = 0;
	CharacterRenderInfo.RI_Center_ShapeItemNum	= 0;

	// end 2009. 08. 27 by ckPark ±×·ˇÇČ ¸®ĽŇ˝ş şŻ°ć ˝Ă˝şĹŰ ±¸Çö

	if(pInfo)
	{
		CharacterRenderInfo.RI_Center = pInfo->ItemNum;

		// 2009. 08. 27 by ckPark ±×·ˇÇČ ¸®ĽŇ˝ş şŻ°ć ˝Ă˝şĹŰ ±¸Çö

		CharacterRenderInfo.RI_Center_Color = pInfo->ColorCode;
		int nShape = pInfo->GetShapeItemNum(); 
		if((CharacterRenderInfo.RI_Center_Color > 0 && nShape > 0 && nShape != pInfo->ItemNum) || (CharacterRenderInfo.RI_Center_Color == 0 && nShape != pInfo->ItemNum))//29-01-2024 by Inet fix for color over shape in logout menu
			CharacterRenderInfo.RI_Center_ShapeItemNum	= nShape;
		else
			CharacterRenderInfo.RI_Center_ShapeItemNum = 0;

		// end 2009. 08. 27 by ckPark ±×·ˇÇČ ¸®ĽŇ˝ş şŻ°ć ˝Ă˝şĹŰ ±¸Çö
	}

	pInfo = g_pShuttleChild->m_pStoreData->FindItemInInventoryByWindowPos(POS_REAR);
	CharacterRenderInfo.RI_Rear = 0;
	if(pInfo)
	{
		CharacterRenderInfo.RI_Rear = pInfo->ItemNum;
	}

	// 2006-08-14 by dgwoo ·ąŔĚ´Ů Á¤ş¸¸¦ ŔúŔĺ.
	pInfo = g_pShuttleChild->m_pStoreData->FindItemInInventoryByWindowPos(POS_PROWIN);
	CharacterRenderInfo.RI_ProwIn = 0;
	if(pInfo)
	{
		CharacterRenderInfo.RI_ProwIn = pInfo->ItemNum;
	}
	//--------------------------------------------------------------------------//

	pInfo = g_pShuttleChild->m_pStoreData->FindItemInInventoryByWindowPos ( POS_PET );
	CharacterRenderInfo.RI_Pet = 0;
	if ( pInfo )
	{
		tPET_CURRENTINFO *psPetCurInfo = GetPetManager()->GetPtr_PetCurrentData ( pInfo->UniqueNumber );
		tPET_LEVEL_DATA *psPetLvData = NULL;
		
		if ( psPetCurInfo )
			psPetLvData = g_pDatabase->GetPtr_PetLevelData ( psPetCurInfo->PetIndex , psPetCurInfo->PetLevel );
		
		CharacterRenderInfo.RI_Pet = pInfo->ItemNum;

#ifdef _INET_PET
		if (psPetLvData)
			CharacterRenderInfo.RI_Pet_ShapeItemNum = pInfo->GetItemInfo()->SourceIndex;
		else
			CharacterRenderInfo.RI_Pet_ShapeItemNum = 7230000;
#else
		if ( psPetLvData )
			CharacterRenderInfo.RI_Pet_ShapeItemNum = psPetLvData->FieldSourceIndex;
#endif
	}

	info.CharacterRenderInfo = CharacterRenderInfo;

	g_pSelect->SetUnitRenderInfo(g_pSelect->GetCurrentSelectIndex(), info);
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			void CShuttleChild::InitWarpShuttle(MAP_CHANNEL_INDEX &channelIndex = NULL)
/// \brief		żöÇÁ˝Ă ĂĘ±âČ­
/// \author		ispark
/// \date		2006-05-29 ~ 2006-05-29
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CShuttleChild::InitWarpShuttle(MAP_CHANNEL_INDEX channelIndex)
{
	// 2006-12-14 by dgwoo	µµ˝Ă¸ĘżˇĽ± Ĺä±Ű ˝şĹłŔ» ÇŘÁ¦˝ĂĹ˛´Ů.
	if(IS_BAZAAR_MAP_INDEX(channelIndex.MapIndex) 
		|| IS_CITY_MAP_INDEX(channelIndex.MapIndex))
	{
		m_pSkill->ReleaseAllUsingToggleSkill();
	}
	// 2006-12-22 by dgwoo żöÇÁ Ĺë°ú˝Ă ±×¶óżîµĺ˝şĹłŔ» ÇŘÁ¦ÇŃ´Ů.
	if(m_pSkill)
		m_pSkill->ReleaseGroundSkill();
	DeleteItemPoint();// Effect·Î »ýĽşÇŃ ľĆŔĚĹŰĆ÷ŔÎĹÍ¸¦ Áöżî´Ů
	m_bSkillMoveIsUse = FALSE;
	// 2007-01-08 by bhsohn ŔÎşńÁöşíČÄ żöÇÁ Ĺë°ú ˝Ă, °ř°ÝŔĚ µÇ´Â Çö»ó Ăł¸®
	//m_bAttack = TRUE;
	if(m_pSkill)
	{
		if(TRUE == m_pSkill->IsAttackPossibility())
		{
			m_bAttack = TRUE;
		}
	}
	else
	{
		m_bAttack = TRUE;
	}
	// end 2007-01-08 by bhsohn ŔÎşńÁöşíČÄ żöÇÁ Ĺë°ú ˝Ă, °ř°ÝŔĚ µÇ´Â Çö»ó Ăł¸®
	InitMissileWarning();
	SetBackView(FALSE);		// 2006-05-29 by ispark, ąéşä ĂĘ±âČ­
	InitRolling();
}

void CShuttleChild::InitWarpShuttle()
{
	// 2006-12-22 by dgwoo żöÇÁ Ĺë°ú˝Ă ±×¶óżîµĺ˝şĹłŔ» ÇŘÁ¦ÇŃ´Ů.
	if(m_pSkill)
		m_pSkill->ReleaseGroundSkill();
	
	DeleteItemPoint();// Effect·Î »ýĽşÇŃ ľĆŔĚĹŰĆ÷ŔÎĹÍ¸¦ Áöżî´Ů
	m_bSkillMoveIsUse = FALSE;
	// 2007-01-08 by bhsohn ŔÎşńÁöşíČÄ żöÇÁ Ĺë°ú ˝Ă, °ř°ÝŔĚ µÇ´Â Çö»ó Ăł¸®
	//m_bAttack = TRUE;
	if(m_pSkill )
	{
		if(TRUE == m_pSkill->IsAttackPossibility())
		{
			m_bAttack = TRUE;
		}		
	}
	else
	{
		m_bAttack = TRUE;
	}
	// end 2007-01-08 by bhsohn ŔÎşńÁöşíČÄ żöÇÁ Ĺë°ú ˝Ă, °ř°ÝŔĚ µÇ´Â Çö»ó Ăł¸®
	InitMissileWarning();
	SetBackView(FALSE);		// 2006-05-29 by ispark, ąéşä ĂĘ±âČ­
	InitRolling();
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			int CShuttleChild::CheckUnitState()
/// \brief		ÇöŔç ±âľî »óĹÂµéŔ» ż©·Ż°ˇÁö·Î ĹëÇŘĽ­ ĆÇ´Ü
/// \author		ispark
/// \date		2006-09-19 ~ 2006-09-19
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
int CShuttleChild::CheckUnitState()
{
	if(m_dwState == _FALLING ||
		m_dwState == _FALLEN ||
		m_dwState == _EXPLODING ||
		m_dwState == _EXPLODED ||
		m_dwState == _FUELALLIN)
	{
		return BREAKDOWN;
	}

	if(m_dwState == _NORMAL ||
		m_dwState == _WARP ||
		m_dwState == _LANDING ||
		m_dwState == _LANDED ||
		m_dwState == _TAKINGOFF)
	{
		return FLIGHT;
	}

	return DONOTKNOW;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			void CShuttleChild::CheckFormationMoveDelete()
/// \brief		Ćí´ë şńÇŕ ÁßŔĚ¸é ÇŘÁ¦
/// \author		ispark
/// \date		2006-11-02 ~ 2006-11-02
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CShuttleChild::CheckFormationMoveDelete()
{
	if(	m_pClientParty &&
		m_pClientParty->IsFormationFlightMove())
	{
		SendPacket(T_IC_PARTY_CANCEL_FLIGHT_POSITION);
		m_pClientParty->FormationFlightClear();
		g_pD3dApp->m_pChat->CreateChatChild(STRMSG_C_PARTY_0001, COLOR_SYSTEM);

		// ÇŘÁ¦˝Ă ąćÇâ°Ş ĂĘ±âČ­ 
		m_fCurrentSideRate = 0.0f;
		m_fCurrentAngle = 0.0f;
	}
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			CHARACTER* GetShuttleInfo();
/// \brief		ĽĹĆ˛ŔÇ Á¤ş¸¸¦ ľňľîżÂ´Ů.
/// \author		dgwoo
/// \date		2007-01-26 ~ 2007-01-26
/// \warning	
///
/// \param		
/// \return		CHARACTER*
///////////////////////////////////////////////////////////////////////////////
const CHARACTER* CShuttleChild::GetShuttleInfo()
{
	return &m_myShuttleInfo;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			BOOL CShuttleChild::IsMyShuttleRolling()
/// \brief		ĽĹĆ˛ŔÇ Á¤ş¸¸¦ ľňľîżÂ´Ů.
/// \author		// 2007-03-23 by bhsohn ·Ń¸µÇĎ¸éĽ­ °řÁß ˝ĂÁî ľČµÇ´Â ą®Á¦ ÇŘ°á
/// \date		2007-03-23 ~ 2007-03-23
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CShuttleChild::IsMyShuttleRolling()
{
	return m_bRollUsed;	
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			BOOL CShuttleChild::IsObserverMode()
/// \brief		żÉŔúąö ¸đµĺ 
/// \author		// 2007-03-27 by bhsohn żÉŔúąö ¸đµĺ˝Ă ŔÎĹÍĆäŔĚ˝ş Ľű±č
/// \date		2007-03-27 ~ 2007-03-27
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CShuttleChild::IsObserverMode()
{
	if(m_stObserve.ClientIndex)
		return TRUE;
	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			BOOL CShuttleChild::IsOperAndObser()
/// \brief		żÉŔúąö ¸đµĺ ŔĚ°ĹłŞ °üŔü ¸đµĺÁßŔÎ°ˇ?
/// \author		dgwoo
/// \date		2007-06-18 ~ 2007-06-18
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CShuttleChild::IsOperAndObser()
{
	if(m_bObserve || m_bOperation)
		return TRUE;
	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			BOOL ChangeObserveUser(int nKey);
/// \brief		żÉŔúąö ŔŻŔú¸¦ şŻ°ćÇŇ¶§.
/// \author		dgwoo
/// \date		2007-03-29 ~ 2007-03-29
/// \warning	
///
/// \param		´ÜĂŕĹ°ŔÇ ąřČŁ.
/// \return		ŔŻŔú°ˇ Á¸ŔçÇĎ´Â°ˇ Á¸ŔçÇĎÁö ľĘŔş°ˇ.
///////////////////////////////////////////////////////////////////////////////
BOOL CShuttleChild::ChangeObserveUser(int nKey)
{
	// ÁßşąŔ¸·Î Ľ­ąö·Î ŔÎµ¦˝ş°ŞŔ» ş¸ł»´Â°ÍŔ» ¸·±â Ŕ§ÇŃ ĽŇ˝ş
	ClientIndex_t tempIndex = m_stObserve.ClientIndex;
	if(tempIndex == m_ObserveQuick[nKey])
		return FALSE;

	// ±âÁ¸ŔÇ żÉŔúąö ¸đµĺ·Î şŮľîŔÖŔ»˝Ă ÇŘÁ¦¸¦ ¸ŐŔú ÇŃ´Ů.
	if(IsObserverMode())
	{
		ObserveEnd();
	}

	// ´ÜĂŕĹ° ąřČŁżˇ ŔúŔĺµÇľîŔÖ´Â żÉŔúąö·Î şŮŔ»°ÍŔ» żäĂ».
	
	if(m_ObserveQuick[nKey])
	{
		MSG_FC_CHARACTER_OBSERVER_TARGET_CHARACTERINDEX sMsg;
		sMsg.TargetClientIndex = m_ObserveQuick[nKey];
		g_pFieldWinSocket->SendMsg( T_FC_CHARACTER_OBSERVER_START, (char*)&sMsg, sizeof(sMsg) );
		return TRUE;
	}
	DbgOut("Observe Quick[%d].ClientIndex = %d\n",nKey,m_ObserveQuick[nKey]);
	return FALSE;
}

void CShuttleChild::TickOperation(float fElapsedTime)
{
	if(g_pD3dApp->m_bChatMode == FALSE)
	{
		// Ĺ°°Ş Ăł¸®.
		// FĹ°¸¦ ´­·¶Ŕ»°ćżě ŔŻŔú »óĹÂĂ˘Ŕş »ç¶óÁř´Ů.
		if(g_pD3dApp->m_pKeyBoard->GetAsyncKeyState(DIK_F))
		{
			g_pInterface->m_pGameArena->m_bShowOperUserInfo = !g_pInterface->m_pGameArena->m_bShowOperUserInfo;
		}
		// żÉŔúąö ¸đµĺ¸¦ ĂëĽŇÇŇ °ćżě.
		if(g_pD3dApp->m_pKeyBoard->GetAsyncKeyState(DIK_ESCAPE))
		{
			ObserveCancelUpdateInfo();
			ObserveEnd();
		}

	}
}
///////////////////////////////////////////////////////////////////////////////
/// \fn			void TickObserve(float fElapsedTime)
/// \brief		żÉŔúąö¸đµĺ˝Ă ŔĚÇÔĽö¸¦ °ĹÄŁ´Ů.
/// \author		dgwoo
/// \date		2007-03-29 ~ 2007-03-29
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CShuttleChild::TickObserve(float fElapsedTime)
{
	if(g_pD3dApp->m_bChatMode == FALSE)
	{
		// Ĺ°°Ş Ăł¸®.
		for(int i = 0 ; i < OBSERVE_MAX_QUICK ; i++)
		{
			// 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
			//if( g_pD3dApp->m_pKeyBoard->GetAsyncKeyState(DIK_1+i) )
			if( g_pD3dApp->GetAsyncKeyState_DIK_DIJ(DIK_1+i) )
			{
				if(i == 9)
				{
					ChangeObserveUser(0);
				}
				else{
					// żÉŔúąö ¸đµĺżˇĽ­ ÇöŔç °ˇ·Á°íÇĎ´Â ŔŻŔúŔÇ ´ÜĂŕĹ°¸¦ ´­·¶Ŕ»°ćżě.
					ChangeObserveUser(i+1);
				}
				
			}
		}
		// żÉŔúąö ¸đµĺ¸¦ ĂëĽŇÇŇ °ćżě.
		if(g_pD3dApp->m_pKeyBoard->GetAsyncKeyState(DIK_ESCAPE))
		{
			ObserveCancelUpdateInfo();
			ObserveEnd();
		}
	}
}
void CShuttleChild::ObserveCancelUpdateInfo()
{
	// 2007-04-04 by dgwoo ESCĹ°¸¦ ´©¸¦ °ćżěżˇ¸¸ ĽĹĆ˛ŔÇ Á¤ş¸¸¦ ´Ů˝Ă ŔÔ·ÂÇŘÁŘ´Ů.
	g_pShuttleChild->m_myShuttleInfo.DP = g_pShuttleChild->m_stObserve.ClientInfo.DP;
	g_pShuttleChild->m_myShuttleInfo.HP = g_pShuttleChild->m_stObserve.ClientInfo.HP;
	g_pShuttleChild->m_myShuttleInfo.EP = g_pShuttleChild->m_stObserve.ClientInfo.EP;
	g_pShuttleChild->m_myShuttleInfo.SP = g_pShuttleChild->m_stObserve.ClientInfo.SP;
	g_pShuttleChild->m_myShuttleInfo.CurrentDP = g_pShuttleChild->m_stObserve.ClientInfo.CurrentDP;
	g_pShuttleChild->m_myShuttleInfo.CurrentHP = g_pShuttleChild->m_stObserve.ClientInfo.CurrentHP;
	g_pShuttleChild->m_myShuttleInfo.CurrentEP = g_pShuttleChild->m_stObserve.ClientInfo.CurrentEP;
	g_pShuttleChild->m_myShuttleInfo.CurrentSP = g_pShuttleChild->m_stObserve.ClientInfo.CurrentSP;
	g_pGameMain->m_pUnitInfoBar->UpdateAllGage();
	//--------------------------------------------------------------------------//
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			void CShuttleChild::ObserveEnd()
/// \brief		żÉŔúąö Áľ·á˝Ă.
/// \author		dgwoo
/// \date		2007-04-03 ~ 2007-04-03
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CShuttleChild::ObserveEnd()
{
	if(0 == m_stObserve.ClientIndex)
	{
		return;
	}

	MSG_FC_CHARACTER_OBSERVER_END sMsg;
	sMsg.TargetClientIndex = m_stObserve.ClientIndex;
	g_pFieldWinSocket->SendMsg( T_FC_CHARACTER_OBSERVER_END, (char*)&sMsg, sizeof(sMsg) );

	m_bMouseLock = FALSE;
	m_stObserve.ClientIndex = NULL;
	// żÉŔúąö ¸đµĺ°ˇ ˛¨Áö¸éĽ­ şÎ˝şĹÍ ŔĚĆĺĆ® żŞ˝Ă »čÁ¦ÇŃ´Ů.
	CAppEffectData* pEffect =  g_pScene->FindEffect(RC_EFF_BOOSTER);
	if(pEffect)
	{
		pEffect->ChangeBodyCondition(BODYCON_LANDED_MASK); // BODYCON_BOOSTER_OFF
	}
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			void CShuttleChild::IsbAGearFollowEnemy()
/// \brief		
/// \author		// 2007-04-17 by bhsohn °ˇ±îżî °Ĺ¸®żˇĽ­ŔÇ ˝ĂÁî¸đµĺ˝Ă µĄŔĚÁö ľČµéľî°ˇ´Â Çö»ó Ăł¸®
/// \date		2007-04-17 ~ 2007-04-17
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CShuttleChild::IsAGearFollowEnemy()
{
	return m_bAGearFollowEnemy;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		˝şÄµ ľĆŔĚĹŰ »çżë ą× Ăë
/// \author		// 2007-04-19 by bhsohn Ľ­ÄˇľĆŔĚ ľĆŔĚĹŰ Ăß°ˇ
/// \date		2007-04-19 ~ 2007-04-19
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CShuttleChild::UseScanItem(BOOL i_bScan, float i_fCheckEnableTime, float i_fScanRange)
{	
	m_pSkill->SetScanSkill(SCAN_ITEM, i_bScan);
	if(FALSE == i_bScan)
	{
		return;
	}
	m_pSkill->SetScanRange(SCAN_ITEM, i_fScanRange);	

	ClientIndex_t nMyClientIndex = m_myShuttleInfo.ClientIndex;
	CUnitData *pUnit = g_pScene->FindUnitDataByClientIndex(nMyClientIndex);
	if(NULL == pUnit)
	{
		return;
	}	
	D3DXVECTOR3	vVel, vUp, vPos;
	vVel = D3DXVECTOR3(0,0,1);
	vUp = D3DXVECTOR3(0,1,0);
	vPos = pUnit->m_vPos;

	// ˝şÄµ ŔĚĆĺĆ® »ýĽş
	CreateScanItemEffect(pUnit, vPos, vVel, vUp, i_fCheckEnableTime, RC_EFF_SCAN_ITEM);	

	// ˝şÄµ Ŕ§Äˇ ÁöÁ¤ 
	m_pSkill->SetScanPosition(SCAN_ITEM, vPos);
	
	INIT_MSG_WITH_BUFFER(MSG_FC_CHARACTER_SHOW_MAP_EFFECT, T_FC_CHARACTER_SHOW_MAP_EFFECT, pSMode, SendBuf);
	pSMode->avec3Position	= vPos;
	pSMode->avec3Target		= vVel*1000.0f;
	pSMode->avec3Up			= vUp*1000.0f;
	pSMode->nLifetime		= (int)i_fCheckEnableTime;
	pSMode->EffectIndex		= RC_EFF_SCAN_ITEM;
	
	g_pD3dApp->m_pFieldWinSocket->Write(SendBuf, MSG_SIZE(MSG_FC_CHARACTER_SHOW_MAP_EFFECT));	
}
///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		
/// \author		// 2007-04-19 by bhsohn Ľ­ÄˇľĆŔĚ ľĆŔĚĹŰ Ăß°ˇ
/// \date		2007-04-19 ~ 2007-04-19
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CShuttleChild::CreateScanItemEffect(CUnitData *pUnit, 
										D3DXVECTOR3 vPos, D3DXVECTOR3 vVel, D3DXVECTOR3 vUp, 
										float i_fCheckEnableTime,
										UINT i_uEffectId)
{		
	// ŔÚ˝ĹŔĚ ľ´ ˝şÄµŔĎ °ćżě´Â ´«Ŕ» »ýĽşÇĎ°í ľĆ´Ď¶ó¸é Enemy->UseSkillFromServerżˇĽ­ »ýĽşÇŃ´Ů.	
	g_pScene->AddFieldItemScanObject(pUnit, i_fCheckEnableTime, i_uEffectId, _LAY, vVel, vUp, &vPos);	
}

ClientIndex_t CShuttleChild::GetTargetClientIndext()
{
	ClientIndex_t clientIdx = 0;
	if(NULL == m_pOrderTarget)
	{
		return clientIdx;
	}
	
	DWORD dwPartType = m_pOrderTarget->m_dwPartType;
	ClientIndex_t AttackIndex = 0; 
	switch(dwPartType)
	{
	case _ENEMY:
		{
			clientIdx = ((CEnemyData*)m_pOrderTarget)->m_infoCharacter.CharacterInfo.ClientIndex;
		}
		break;
	case _MONSTER:
		{
			clientIdx = ((CMonsterData*)m_pOrderTarget)->m_info.MonsterIndex;
		}
		break;
	}	
	return clientIdx;
}
ClientIndex_t CShuttleChild::GetTargetToClientIdx(CAtumData* pTarget)
{
	ClientIndex_t clientIdx = 0;
	if(NULL == pTarget)
	{
		return clientIdx;
	}
	DWORD dwPartType = pTarget->m_dwPartType;
	switch(dwPartType)
	{
	case _ENEMY:
		{
			clientIdx = ((CEnemyData*)pTarget)->m_infoCharacter.CharacterInfo.ClientIndex;
		}
		break;
	case _MONSTER:
		{
			clientIdx = ((CMonsterData*)pTarget)->m_info.MonsterIndex;
		}
		break;
	}	
	return clientIdx;
}
///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		°˘ ±âľîş° ŔĚ·ú ˝Ă°Ł ĂĽĹ©
/// \author		// 2007-05-15 by bhsohn A±âľî ş¸żĎ
/// \date		2007-05-15 ~ 2007-05-15
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
float CShuttleChild::GetRasingCheckTime()
{
	if(IS_DT(m_myShuttleInfo.UnitKind))
	{
		// A±âľîłÄ?
		return AGEAR_RASING_TIME;
	}
	return 1.0f;

}

///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		Ç®˝şĹČ Á¤ş¸ ľ÷µĄŔĚĆ® 
/// \author		// 2007-05-15 by bhsohn ±âľî ˝şĹČ °ü·Ă Ăł¸®
/// \date		2007-05-15 ~ 2007-05-15
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CShuttleChild::SetMyShuttleFullStatInfo(GEAR_STAT i_MyShuttleFullStatInfo)
{
	m_myShuttleFullStatInfo = i_MyShuttleFullStatInfo;
}
///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		Ç®˝şĹČ Á¤ş¸ ľË·ÁÁÜ. 
/// \author		// 2007-05-15 by bhsohn ±âľî ˝şĹČ °ü·Ă Ăł¸®
/// \date		2007-05-15 ~ 2007-05-15
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
GEAR_STAT CShuttleChild::GetMyShuttleFullStatInfo()
{
	return m_myShuttleFullStatInfo;
}
///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		ĂÖ´ë °ŞŔ» ¸ř łŃ°Ô şŻ°ć 
/// \author		// 2007-05-15 by bhsohn ±âľî ˝şĹČ °ü·Ă Ăł¸®
/// \date		2007-05-15 ~ 2007-05-15
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CShuttleChild::ReformMyShuttleStatInfo(GEAR_STAT* pMyGearStat)
{
	// 2010. 02. 19 by ckPark 100·ąş§ąĚ¸¸ŔĎ˝Ă 300˝şĹČĂĘ°ú·®ŔĚ ŔűżëµÇ´Â ąö±× ĽöÁ¤
	
// 	// °ř°ÝĆÄĆ®
// 	if(pMyGearStat->AttackPart > COUNT_MAX_STAT_POINT)
// 	{
// 		pMyGearStat->AttackPart = COUNT_MAX_STAT_POINT;
// 	}
// 
// 	// ąćľîĆÄĆ®
// 	if(pMyGearStat->DefensePart > COUNT_MAX_STAT_POINT)
// 	{
// 		pMyGearStat->DefensePart = COUNT_MAX_STAT_POINT;
// 	}
// 
// 	// ż¬·áĆÄĆ®
// 	if(pMyGearStat->FuelPart > COUNT_MAX_STAT_POINT)
// 	{
// 		pMyGearStat->FuelPart = COUNT_MAX_STAT_POINT;
// 	}
// 
// 	// °¨ŔŔĆÄĆ®
// 	if(pMyGearStat->SoulPart > COUNT_MAX_STAT_POINT)
// 	{
// 		pMyGearStat->SoulPart = COUNT_MAX_STAT_POINT;
// 	}
// 
// 	// ˝ŻµĺĆÄĆ®
// 	if(pMyGearStat->ShieldPart > COUNT_MAX_STAT_POINT)
// 	{
// 		pMyGearStat->ShieldPart = COUNT_MAX_STAT_POINT;
// 	}
// 
// 	// Č¸ÇÇĆÄĆ®, 0 ~ 255ŔÇ Č®·ü
// 	if(pMyGearStat->DodgePart > COUNT_MAX_STAT_POINT)
// 	{
// 		pMyGearStat->DodgePart = COUNT_MAX_STAT_POINT;
// 	}

	for( int i=0; i<6; ++i )
	{
		Stat_t* pStat;

		switch( i )
		{
		case 0:
			pStat = &(pMyGearStat->AttackPart);
			break;
		case 1:
			pStat = &(pMyGearStat->DefensePart);
			break;
		case 2:
			pStat = &(pMyGearStat->FuelPart);
			break;
		case 3:
			pStat = &(pMyGearStat->SoulPart);
			break;
		case 4:
			pStat = &(pMyGearStat->ShieldPart);
			break;
		case 5:
			pStat = &(pMyGearStat->DodgePart);
			break;
		}

		if( m_myShuttleInfo.Level <= CHARACTER_LEVEL_100
			&& (*pStat) > CHARACTER_LEVEL_100_MAX_STAT_POINT )
		{
			(*pStat) = CHARACTER_LEVEL_100_MAX_STAT_POINT;
		}
		else if( m_myShuttleInfo.Level > CHARACTER_LEVEL_100
				 && (*pStat) > CHARACTER_LEVEL_110_MAX_STAT_POINT )
		{
			(*pStat) = CHARACTER_LEVEL_110_MAX_STAT_POINT;
		}
	}

	// end 2010. 02. 19 by ckPark 100·ąş§ąĚ¸¸ŔĎ˝Ă 300˝şĹČĂĘ°ú·®ŔĚ ŔűżëµÇ´Â ąö±× ĽöÁ¤
}

// 2007-05-28 by bhsohn ľĆŔĚĹŰżˇ ´ëÇŃ ĂĽĹ©Ľ¶ Ăß°ˇ

// 2009. 05. 29 by ckPark SHA256 ĂĽĹ©Ľ¶ ˝Ă˝şĹŰ
// BOOL CShuttleChild::GetCheckSum(UINT *o_puiCheckSum, int *o_pnFileSize, char* pFilePath)
// {
// 	return m_pGameData->GetCheckSum(o_puiCheckSum, o_pnFileSize, pFilePath);
// }

BOOL CShuttleChild::GetCheckSum(BYTE o_byObjCheckSum[32], int *o_pnFileSize, char* pFilePath)
{
	return m_pGameData->GetCheckSum(o_byObjCheckSum, o_pnFileSize, pFilePath);
}
// end 2009. 05. 29 by ckPark SHA256 ĂĽĹ©Ľ¶ ˝Ă˝şĹŰ

///////////////////////////////////////////////////////////////////////////////
/// \fn			BOOL CMonsterData::()
/// \brief		żŔşęÁ§Ć®¸¦ Ĺ¸ÄĎ ŔâŔ»Áö ¸»Áö °áÁ¤
/// \author		// 2007-05-17 by bhsohn żŔşęÁ§Ć® µÚżˇ ĽűľúŔ»˝Ă żˇ ´ëÇŃ Ăł °Ë»ç Ăł¸®	
/// \date		2007-05-17 ~ 2007-05-17
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CShuttleChild::IsCheckObjectColl(CAtumData *pTarget)
{
	BOOL bTargetEnemyInfo = FALSE;
	if(NULL == pTarget)
	{
		return bTargetEnemyInfo;
	}
	switch(pTarget->m_dwPartType)
	{
	case _ENEMY:
	case _MONSTER:
		{
			CUnitData* pEnemyUnit = (CUnitData*)pTarget;			
			bTargetEnemyInfo = pEnemyUnit->IsCheckObjectColl();	// ĂćµąĂĽĹ©
		}
		break;		
	default:
		{
			return bTargetEnemyInfo;
		}
		break;
	}	
	
	return bTargetEnemyInfo;	
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		ÇöŔç ŔÖ´Â ¸Ężˇ ´ëÇŃ Á¤ş¸
/// \author		// 2007-05-17 by bhsohn żŔşęÁ§Ć® µÚżˇ ĽűľúŔ»˝Ă żˇ ´ëÇŃ Ăł °Ë»ç Ăł¸®
/// \date		2007-06-04 ~ 2007-06-04
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
CHARACTER CShuttleChild::GetMyShuttleInfo()
{
	return m_myShuttleInfo;
}


///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		
/// \author		// 2007-06-12 by bhsohn ąĚĽÇ ŔÎĹÍĆäŔĚ˝ş ĽöÁ¤ľČ
/// \date		2007-06-12 ~ 2007-06-12
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
PARTYINFO CShuttleChild::GetPartyInfo()
{
	return m_pClientParty->GetPartyInfo();
}
///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		ąĚĽÇ¸ĘŔ¸·Î ş¸ł»±â
/// \author		// 2007-06-12 by bhsohn ąĚĽÇ ŔÎĹÍĆäŔĚ˝ş ĽöÁ¤ľČ
/// \date		2007-06-12 ~ 2007-06-12
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CShuttleChild::SendGoMissionMap()
{	
	int nMapQuestIndex = g_pGameMain->GetMissionMapQuestIdx();	
	if(nMapQuestIndex)
	{
		INIT_MSG_WITH_BUFFER(MSG_FC_QUEST_MOVE_QUEST_MAP, T_FC_QUEST_MOVE_QUEST_MAP,
			pSQuestMove, SendBuf);
		pSQuestMove->QuestIndex0 = nMapQuestIndex;
		g_pFieldWinSocket->Write(SendBuf, MSG_SIZE(MSG_FC_QUEST_MOVE_QUEST_MAP));
		return TRUE;
	}
	return FALSE;	
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		// ŔŻ´Ö Á¤ş¸ ĂĘ±âČ­ 
/// \author		// 2007-07-04 by bhsohn ·Łµů ¸đĽÇÁß Ć÷Ĺ» ĹŔŔ»˝Ă ąö±× ĽöÁ¤
/// \date		2007-07-04 ~ 2007-07-04
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CShuttleChild::InitUnitState()
{
	m_bOrderMove = FALSE;
	g_pD3dApp->m_vecCharacterOtherInfo.clear();
//	g_pD3dApp->m_vecCharacterOtherInfoRequest.clear();
	g_pD3dApp->m_vecCharacterOtherRenderInfo.clear();
//	g_pD3dApp->m_vecCharacterOtherRenderInfoRequest.clear();
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			BOOL CShuttleChild::IsWeaponeTarget()
/// \brief		1Çü 2Çü ą«±â ąß»ç˝Ă Ĺ¸°ÝŔ» ÁŮĽöŔÖ´Â »óĹÂŔÎ°ˇ?
/// \author		dgwoo
/// \date		2007-07-25 ~ 2007-07-25
/// \warning	
///
/// \param		2Çüą«±âŔÎ°ˇ?
/// \return		°ř°ÝÇŇ°ćżě Ĺ¸°ŮŔĚ ¸ÂŔ¸¸é True Çă°řżˇ ¸ÂŔ¸¸é False
///////////////////////////////////////////////////////////////////////////////
BOOL CShuttleChild::IsWeaponeTarget(BOOL i_bSecondW)
{
	if(i_bSecondW)
	{
		// 2011. 03. 08 by jskim ŔÎÇÇ3Â÷ ±¸Çö - łÍ Ĺ¸°Ů ˝Ă˝şĹŰ
		//if(m_pSecondaryWeapon->m_vecTargetIndex.empty() == FALSE)
		if(m_pSecondaryWeapon->m_vecTargetIndexData.empty() == FALSE)
		// end 2011. 03. 08 by jskim ŔÎÇÇ3Â÷ ±¸Çö - łÍ Ĺ¸°Ů ˝Ă˝şĹŰ
		{
			return TRUE;
		}
	}
	else
	{
		// 2011. 03. 08 by jskim ŔÎÇÇ3Â÷ ±¸Çö - łÍ Ĺ¸°Ů ˝Ă˝şĹŰ
		//if(m_pPrimaryWeapon->m_vecTargetIndex.empty() == FALSE)
		if(m_pPrimaryWeapon->m_vecTargetIndexData.empty() == FALSE)
		// end 2011. 03. 08 by jskim ŔÎÇÇ3Â÷ ±¸Çö - łÍ Ĺ¸°Ů ˝Ă˝şĹŰ
		{
			return TRUE;
		}
	}
	return FALSE;
}
void CShuttleChild::InitWeaponeTarget()
{
	if(m_pSecondaryWeapon)
	{
		// 2011. 03. 08 by jskim ŔÎÇÇ3Â÷ ±¸Çö - łÍ Ĺ¸°Ů ˝Ă˝şĹŰ
		m_pSecondaryWeapon->m_vecTargetIndexData.clear();
		// end 2011. 03. 08 by jskim ŔÎÇÇ3Â÷ ±¸Çö - łÍ Ĺ¸°Ů ˝Ă˝şĹŰ
	}
		if(m_pPrimaryWeapon)
	{
		// 2011. 03. 08 by jskim ŔÎÇÇ3Â÷ ±¸Çö - łÍ Ĺ¸°Ů ˝Ă˝şĹŰ
		m_pPrimaryWeapon->m_vecTargetIndexData.clear();
		// end 2011. 03. 08 by jskim ŔÎÇÇ3Â÷ ±¸Çö - łÍ Ĺ¸°Ů ˝Ă˝şĹŰ
	}
}
void CShuttleChild::RenderInvenBack()
{
	D3DXMATRIX pTemp, pMatrix, pMatBack, pMatOldProj, pMatOldView, pMatPresView,pMatBackProj;
	D3DXMatrixIdentity(&pMatrix);
	D3DXMatrixIdentity(&pMatBack);
	D3DXMatrixIdentity(&pTemp);
	D3DXMatrixIdentity(&pMatOldView);
	D3DXMatrixIdentity(&pMatPresView);
	D3DXMatrixIdentity(&pMatBackProj);
	

	RECT WindowRect;
	GetClientRect(g_pD3dApp->GetHwnd(), &WindowRect);
	D3DXMatrixOrthoLH(&pMatBackProj, 
		             (float)WindowRect.right, 
					 (float)WindowRect.bottom, 
					 0.0f, 100.0f);

	g_pD3dDev->GetTransform( D3DTS_VIEW,		&pMatOldView);
	g_pD3dDev->GetTransform( D3DTS_PROJECTION, &pMatOldProj );	
	g_pD3dDev->SetTransform( D3DTS_VIEW,		&pMatPresView);
	g_pD3dDev->SetTransform( D3DTS_PROJECTION, &pMatBackProj);	
	
	// ąé±×¶óżîµĺ ·»´ő¸µ
	// Restore the modified render states
	g_pD3dDev->SetRenderState( D3DRS_CLIPPLANEENABLE, 0x00 );
	g_pD3dDev->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	
	// Finally, render the mirror itself (as an alpha-blended quad)
	g_pD3dDev->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );
	g_pD3dDev->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
	g_pD3dDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

	g_pD3dDev->SetRenderState( D3DRS_LIGHTING, FALSE );	
	g_pD3dDev->SetRenderState( D3DRS_ZENABLE, FALSE );
	
	int nScreenWidth = -(int)g_pD3dApp->GetBackBufferDesc().Width / 2;
	int nScreenHeight = ((float)g_pD3dApp->GetBackBufferDesc().Height / 2) + 0.5f;
	int nWindowPosY = g_pGameMain->m_nLeftWindowY;

	D3DXMatrixTranslation(&pTemp, 
							nScreenWidth, 
							nScreenHeight - nWindowPosY, 
							0.0f);

	pMatBack = pMatBack * pTemp;

	// ąé±×¶óżîµĺ		
	g_pD3dDev->SetTransform( D3DTS_WORLD, &pMatBack );
	g_pD3dDev->SetRenderState( D3DRS_ZENABLE, TRUE );
	g_pD3dDev->SetRenderState( D3DRS_LIGHTING, TRUE );

	g_pD3dDev->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
	g_pD3dDev->SetTransform( D3DTS_VIEW, &pMatOldView);	
	g_pD3dDev->SetTransform( D3DTS_PROJECTION, &pMatOldProj);	
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			BOOL CShuttleChild::IsRevivalUseItem()
/// \brief		şÎČ° ľĆŔĚĹŰŔ» »çżëÇŇ Á¶°ÇŔĚ µÇ´Â°ˇ?
/// \author		dgwoo
/// \date		2007-10-08 ~ 2007-10-08
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CShuttleChild::IsRevivalUseItem()
{
	MAP_INFO* pMapInfo = g_pDatabase->GetMapInfo(g_pShuttleChild->m_myShuttleInfo.MapChannelIndex.MapIndex);	
	if(IS_MAP_INFLUENCE_EVENT_AREA(pMapInfo->MapInfluenceType) ||   // 2008-04-08 by dgwoo && -> || şŻ°ć.
	IS_MAP_INFLUENCE_ARENA(pMapInfo->MapInfluenceType))
	{
		return FALSE;
	}

	// ŔĎąÝ ĽĽ·ÂŔş ¸Ę°ú »ó°üľřŔĚ »çżë°ˇ´ÉÇĎ´Ů.
	if(IS_NORMAL_INFLUENCE_TYPE(m_myShuttleInfo.InfluenceType))
	{
		return TRUE;
	}

	// 2007-10-08 by dgwoo ľČŔüÁöżŞŔş ¸đµÎ TRUE
	if(MAP_INFLUENCE_NEUTRALITY == pMapInfo->MapInfluenceType					// Áß¸łÁöżŞŔĚ°ĹłŞ
		|| IS_MAP_INFLUENCE_CONFLICT_AREA(pMapInfo->MapInfluenceType)			// şĐŔďÁöżŞŔĚ°ĹłŞ
		|| m_myShuttleInfo.MapChannelIndex.ChannelIndex > 0)					// ľČŔü Ă¤łÎŔĚ¸é »çżë°ˇ´É.
	{
		return TRUE;
	}

	// °ĹÁˇ °ü·Ă.
	if(g_pInterface->m_pINFOpMain->IsRevivalUseItemWarMonster(m_myShuttleInfo.MapChannelIndex.MapIndex))
	{// ÇöŔç ł»°ˇ ŔÖ´Â ¸ĘŔĚ °ĹÁˇŔü°ú °ü·ĂŔĚ ŔÖ´Â ¸ĘŔĚ¸é.
		return FALSE;
	}

	// ŔüÁř±âÁöŔü °ü·Ă.
	if(IS_MAP_INFLUENCE_OUTPOST(pMapInfo->MapInfluenceType))
	{// ŔüÁř±âÁö ¸ĘżˇĽ± şÎČ° ľĆŔĚĹŰ »çżë±ÝÁö.
		return FALSE;
	}

	// ¸đĽ±Ŕü °ü·Ă.
	// 2008-04-04 by bhsohn ¸đĽ±Ŕü °ü¸® Ĺ¬·ˇ˝ş Ăß°ˇ
	if(g_pGameMain->GetSummonMotherShipCnt())
	{
		if(IS_MAP_INFLUENCE_VCN(pMapInfo->MapInfluenceType))
		{
			return g_pGameMain->GetMotherShipInfl(MOTHERSHIP_INFLUENCE_ANI);
		}
		else if(IS_MAP_INFLUENCE_ANI(pMapInfo->MapInfluenceType))
		{
			return g_pGameMain->GetMotherShipInfl(MOTHERSHIP_INFLUENCE_VCN);
		}

	}
	return TRUE;
	
}

CEffectInfo* CShuttleChild::GetEffectWeapon(int nInvenIdx)
{
	char *pObjName = m_vectInvenWeaponMesh[nInvenIdx].pInvenWeaponMesh->GetMeshObjFileName();
	CEffectInfo* pEffectInfo = g_pD3dApp->m_pEffectRender->GetObjEffectInfo(pObjName);
	if(NULL == pEffectInfo)
	{
		return NULL;
	}				
	if(NULL == pEffectInfo->m_pEffect)
	{
		return NULL;
	}
	
	return pEffectInfo;
	
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		
/// \author		// 2007-12-05 by bhsohn ş®żˇ ¶Ő°í µéľî°ˇ´Â Çö»ó Ăł¸®
/// \date		2007-12-05 ~ 2007-12-05
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CShuttleChild::CheckObjLandingColl(D3DXVECTOR3 vNormalVector )
{	
	D3DXVECTOR3 vObjNormalVector = vNormalVector;
	D3DXVECTOR3 vObjSide = vObjNormalVector;
	
	D3DXVec3Normalize(&vObjNormalVector,&vObjNormalVector);
	
	vObjSide.y = 0;
	D3DXVec3Normalize(&vObjSide,&vObjSide);
	
	float fAngleRadian = GetRadianVectorBetweenVector(vObjNormalVector, vObjSide);	
	float fDo = 90.0f - (fAngleRadian*57.29577951f);
	DBGOUT("fDo [%.2f] \n", fDo);	
	// 10µµ łŃŔ¸¸é ľČµČ´Ů. 
	if(fDo > MAX_LADING_DO)
	{
		return FALSE;
	}
	return TRUE;	
}
///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		
/// \author		// 2007-12-05 by bhsohn ş®żˇ ¶Ő°í µéľî°ˇ´Â Çö»ó Ăł¸®
/// \date		2007-12-05 ~ 2007-12-05
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CShuttleChild::CancelLandingMode()
{
	if(g_pTutorial->IsTutorialMode() == FALSE)
	{
		//if(m_fNumpad0keyTime <= 0.0f)
		{
			// Âř·úŔü ˝ĂÁˇŔĚ 1ŔÎÄŞŔĚż´Ŕ¸¸é ´Ů˝Ă 1ŔÎÄŞŔ¸·Î şŻČŻ 
			if(m_bLandingOldCamtypeFPS)
			{
				g_pD3dApp->m_pCamera->ChangeCamType();
				m_bLandingOldCamtypeFPS = FALSE;
			}
			
			SendPacket(T_FC_MOVE_TAKEOFF);
			DBGOUT("T_FC_MOVE_TAKEOFF\n");
			g_pCamera->Init(m_vPos - 10.0f*m_vVel,m_vVel, 3.14f/13.0f, 60.0f);
			m_fNumpad0keyTime = 2.0f;
			ChangeUnitState(_TAKINGOFF);
			// 2007-05-15 by bhsohn A±âľî ş¸żĎ
			//m_fRasingCheckTime = 1.0f;
			m_fRasingCheckTime = GetRasingCheckTime();
		}
		return TRUE;
	}
	return FALSE;
	
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		
/// \author		// 2008-01-16 by bhsohn A±âľî żöÇÁ ˝ÇĆĐ˝Ă Ŕ§Ŕĺ,±×¶óżîµĺ ż˘Ľż ĂëĽŇ ˝ĂĹ´
/// \date		2008-01-16 ~ 2008-01-16
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CShuttleChild::SetShuttleAirMode(BOOL bIsAir)
{
	m_bIsAir = bIsAir;
	if(bIsAir && IS_DT(m_myShuttleInfo.UnitKind) && (m_pSkill))
	{
		// ±âĂĽ°ˇ ¶°ŔÖ´Â »óĹÂżˇĽ­ A±âľî ±×¶óżîµĺ ±âĽúŔ» ˛ű.
		m_pSkill->ReleaseGroundSkill();			
	}
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		
/// \author		// 2007-11-22 by bhsohn ľĆ·ąłŞ ĹëÇŐĽ­ąö
/// \date		2007-11-22 ~ 2007-11-22
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
UID32_t	CShuttleChild::GetCharacterUniqueNumber()
{
	if(g_pD3dApp->GetArenaState() == ARENA_STATE_ARENA_GAMING )
	{
		CHARACTER*	pArenaCharacter = g_pD3dApp->GetArenaCharacterInfo();
		return pArenaCharacter->CharacterUniqueNumber;
	}
	return m_myShuttleInfo.CharacterUniqueNumber;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		
/// \author		// 2008-03-17 by bhsohn Ŕ©µµżě ŔĚµż˝Ă TickľČµéľîżŔ´Â ą®Á¦ Ăł¸®
/// \date		2008-03-17 ~ 2008-03-17
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CShuttleChild::IsUnitStop()
{
	return m_bUnitStop;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		
/// \author		// 2008-03-17 by bhsohn Ŕ©µµżě ŔĚµż˝Ă TickľČµéľîżŔ´Â ą®Á¦ Ăł¸®
/// \date		2008-03-17 ~ 2008-03-17
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CShuttleChild::IsAir()
{
	return m_bIsAir;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		°ř°łĽłÁ¤ żÉĽÇ şŻ°ć
/// \author		// 2008-06-20 by bhsohn EP3 żÉĽÇ°ü·Ă Ăł¸®
/// \date		2008-06-25 ~ 2008-06-25
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CShuttleChild::SetSecretInfoOption(INT i_nSecretInfoOption)
{
	// 2008-12-16 by dgwoo ąĚĽÇ¸¶˝şĹÍ.

	if((m_myShuttleInfo.SecretInfoOption & USER_INFO_OPTION_MISSIONMASTER) != (i_nSecretInfoOption & USER_INFO_OPTION_MISSIONMASTER))
	{
		if(i_nSecretInfoOption & USER_INFO_OPTION_MISSIONMASTER)
		{// ąĚĽÇ¸¶˝şĹÍ·Î µî·Ď.
			g_pD3dApp->m_pChat->CreateChatChild(STRMSG_C_081219_0100,COLOR_SYSTEM);
		}else
		{// ąĚĽÇ¸¶˝şĹÍżˇĽ­ ÇŘÁ¦.
			g_pD3dApp->m_pChat->CreateChatChild(STRMSG_C_081219_0101,COLOR_SYSTEM);
		}
		m_myShuttleInfo.SecretInfoOption = i_nSecretInfoOption;	
		g_pGameMain->m_pMissionMain->UpdateMissionMasterButton();
	}else
	{
		char buf[256];
		wsprintf(buf, STRMSG_C_080609_0200); //"\\yżÉĽÇŔĚ şŻ°ćµÇľú˝Ŕ´Ď´Ů."
		g_pD3dApp->m_pChat->CreateChatChild(buf,COLOR_SYSTEM);	
	}
	m_myShuttleInfo.SecretInfoOption = i_nSecretInfoOption;	
	
}
void CShuttleChild::SetAmorHeight(INT i_nAmorNum)
{
	m_fAmorHeight = GetAmorGearHeight(i_nAmorNum);
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		
/// \author		// 2008-08-22 by bhsohn EP3 ŔÎşĄĹä¸® Ăł¸®
/// \date		2008-09-08 ~ 2008-09-08
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
D3DXMATRIX CShuttleChild::GetMatWeaponPosition(int nIdx)
{
	if(nIdx < 0 || nIdx >= 4)
	{
		D3DXMATRIX MatInven;
		D3DXMatrixIdentity(&MatInven);
		return MatInven;
	}

	return m_pMatWeaponPosition[nIdx];
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		
/// \author		// 2008-08-22 by bhsohn EP3 ŔÎşĄĹä¸® Ăł¸®
/// \date		2008-09-08 ~ 2008-09-08
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
int CShuttleChild::GetInvenWeaponMeshSize()
{
	return m_vectInvenWeaponMesh.size();
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		
/// \author		// 2008-08-22 by bhsohn EP3 ŔÎşĄĹä¸® Ăł¸®
/// \date		2008-09-08 ~ 2008-09-08
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
INVEN_WEAPON_MESH* CShuttleChild::GetInvenWeaponMesh(int nIdx)
{
	if(nIdx < 0 || nIdx>=m_vectInvenWeaponMesh.size())
	{
		return NULL;
	}
	return &m_vectInvenWeaponMesh[nIdx];
}
///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		
/// \author		// 2008-08-22 by bhsohn EP3 ŔÎşĄĹä¸® Ăł¸®
/// \date		2008-09-08 ~ 2008-09-08
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
float CShuttleChild::GetRotationX()
{
	return m_fRotationX;
}
///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		
/// \author		// 2008-08-22 by bhsohn EP3 ŔÎşĄĹä¸® Ăł¸®
/// \date		2008-09-08 ~ 2008-09-08
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
float CShuttleChild::GetRotationZ()
{
	return m_fRotationZ;
}


///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		// şÎ˝şĹÍ ˝Ă°Ł
/// \author		// 2008-09-26 by bhsohn ˝Ĺ±Ô ŔÎĂľĆ® Ăł¸®
/// \date		2008-09-26 ~ 2008-09-26
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
float CShuttleChild::GetEnchantDesParam(CItemInfo* pItemInfo, int nDestParam)
{
	float fBoosterTimeUp = 0.0f;
	
	//ITEM		*pRefITEM = pItemInfo->GetItemInfo();
	RARE_ITEM_INFO *pRefPrefixRareInfo = pItemInfo->GetPrefixRareInfo();
	RARE_ITEM_INFO *pRefSuffixRareInfo = pItemInfo->GetSuffixRareInfo();
	CParamFactor *	pRefEnchant = pItemInfo->GetEnchantParamFactor();
	if(!pRefEnchant)
	{
		return fBoosterTimeUp;
	}
	fBoosterTimeUp = GetParamFactor_DesParam((*pRefEnchant), nDestParam);

	return fBoosterTimeUp;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		// żŁÁř ŔÎĂľ Á¤ş¸ °»˝Ĺ
/// \author		// 2008-09-26 by bhsohn ˝Ĺ±Ô ŔÎĂľĆ® Ăł¸®
/// \date		2008-09-26 ~ 2008-09-26
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CShuttleChild::UpdateEngineInfo()
{
	// żŁÁř ľĆŔĚĹŰ °ˇÁ®żŔ±â
	CItemInfo* pEngineItem = g_pStoreData->FindItemInInventoryByWindowPos( POS_REAR ); 
	if(NULL == pEngineItem)
	{
		return;
	}
	ITEM* pITEM = pEngineItem->GetRealItemInfo();
	
	// şÎ˝şĹÍ Ăß°ˇ ˝Ă°Ł 
	{
		m_fBURN = (float)pITEM->Time/1000.0f;		
		float fTimeUp = GetEnchantDesParam(pEngineItem, DES_ENGINE_BOOSTER_TIME_UP);
		m_fBURN += fTimeUp;		
		// FLOATÇü ŔçÁ¤·Ä ÇĎż© »çżë
		m_fBURN = FloatRangeDecimal(m_fBURN, 2);
	}

	// ĂÖĽŇ ĽÓµµ
	{
		m_fShuttleSpeedMin = pITEM->AbilityMin;
		float fTime = GetEnchantDesParam(pEngineItem, DES_ENGINE_MIN_SPEED_UP);
		m_fShuttleSpeedMin += fTime;

	}
	// ĂÖ´ë ĽÓµµ
	{
		m_fShuttleSpeedMax = pITEM->AbilityMax;
		float fTime = GetEnchantDesParam(pEngineItem, DES_ENGINE_MAX_SPEED_UP);
		m_fShuttleSpeedMax += fTime;		
	}

	// Áö»ó ĽÓµµ
	{
		m_fShuttleGroundSpeedMax = pITEM->Charging;			// 2005-11-26 by ispark
		float fTime = GetEnchantDesParam(pEngineItem, DES_ENGINE_GROUND_SPEED_UP);
		m_fShuttleGroundSpeedMax += fTime;		
	}

	// şÎ˝şĹÍ ĽÓµµ
	{
		m_fShuttleSpeedBoosterOn = (float)pITEM->Range;
		float fTime = GetEnchantDesParam(pEngineItem, DES_ENGINE_BOOSTER_SPEED_UP);
		m_fShuttleSpeedBoosterOn += fTime;		
	}	
	
	// ÇĎŔĚĆŰ ą«şů(Ľ±Č¸°˘)
	{	
		CParamFactor*	pRefEnchant = pEngineItem->GetEnchantParamFactor();		
		if(pRefEnchant)
		{
			m_paramFactor.pfm_ENGINE_ANGLE_UP = pRefEnchant->pfm_ENGINE_ANGLE_UP;
			m_paramFactor.pfm_ENGINE_BOOSTERANGLE_UP = pRefEnchant->pfm_ENGINE_BOOSTERANGLE_UP;
		}
		else
		{
			// ĂĘ±âČ­			
			m_paramFactor.pfm_ENGINE_ANGLE_UP = m_paramFactor.pfm_ENGINE_BOOSTERANGLE_UP = 0.0f;
		}
		
		m_fMouseRate = CAtumSJ::GetEngineRangeAngle(pITEM, &m_paramFactor);
	}

	// żŁÁř UI°»˝Ĺ
	if(g_pGameMain && g_pGameMain->m_pUnitInfoBar)
	{
		g_pGameMain->m_pUnitInfoBar->SetBURN(m_fCurrentBURN, m_fBURN);
		g_pGameMain->m_pUnitInfoBar->SetSpeed(m_fShuttleSpeed, m_fShuttleSpeedMin, m_fShuttleSpeedBoosterOn);
	}

	SetShuttleSpeed(); // ˝şÇÇµĺ °»˝Ĺ
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		// żŁÁř ŔÎĂľ Á¤ş¸ °»˝Ĺ
/// \author		// 2008-09-26 by bhsohn ˝Ĺ±Ô ŔÎĂľĆ® Ăł¸®
/// \date		2008-09-26 ~ 2008-09-26
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CShuttleChild::SetRadarRange(CItemInfo* i_pRadarItemInfo)
{
	CParamFactor*	pRefEnchant = i_pRadarItemInfo->GetEnchantParamFactor();
	
	// 2011. 2. 25 by jskim ş¸±Ţ »óÁˇ Âř·ú˝Ă ·ąŔĚ´ő ĂĘ±âČ­ µÇ´Â ąö±× ĽöÁ¤
 	if(pRefEnchant)
 	{
 		m_paramFactor.pfm_ATTACK_RANGE_01 = pRefEnchant->pfm_ATTACK_RANGE_01;
 		m_paramFactor.pfm_ATTACK_RANGE_02 = pRefEnchant->pfm_ATTACK_RANGE_02;
 	}
 	else
 	{
 		// ĂĘ±âČ­
 		m_paramFactor.pfm_ATTACK_RANGE_01 = m_paramFactor.pfm_ATTACK_RANGE_02 = 0.0f;
 	}
	if(pRefEnchant)
	{
		m_paramFactor.pfm_ATTACK_RANGE_01 = pRefEnchant->pfm_ATTACK_RANGE_01;
		m_paramFactor.pfm_ATTACK_RANGE_02 = pRefEnchant->pfm_ATTACK_RANGE_02;
 	}
	// end 2011. 2. 25 by jskim ş¸±Ţ »óÁˇ Âř·ú˝Ă ·ąŔĚ´ő ĂĘ±âČ­ µÇ´Â ąö±× ĽöÁ¤
	// 2008-11-13 by bhsohn EP3-2 żˇĽ­ ·ąŔĚ´ő »çÁ¤°Ĺ¸® Áő°ˇ Ćę Ŕűżë ľČµÇ´Â ą®Á¦ ÇŘ°á
	// 2009-05-14 by bhsohn ·ąŔĚ´ő şŻ°ć˝Ă, ĆęŔĚ ľĆ´Ń ľĆŔĚĹŰżˇĽ­ »ç°Ĺ¸® Ŕűżë ľČµÇ´Â ą®Á¦ ÇŘ°á	
//	CItemInfo* pPetItem = m_pStoreData->FindItemInInventoryByWindowPos(POS_PET);
//	if(pPetItem)
// 	{
// 		// 2009-04-21 by bhsohn ľĆŔĚĹŰ DesParamĂß°ˇ	
// 		// 	if(m_wearItemParamFactor[POS_PET].DestParameter1 == DES_ATTACK_RANGE_01 
// 		// 			|| m_wearItemParamFactor[POS_PET].DestParameter1 == DES_ATTACK_RANGE_02 )
// 		// 		{
// 		// 			SetParamFactorDesParam(m_paramFactor, m_wearItemParamFactor[POS_PET].DestParameter1, 
// 		// 				m_wearItemParamFactor[POS_PET].ParameterValue1 );
// 		// 		}
// 		// 		if(m_wearItemParamFactor[POS_PET].DestParameter2 == DES_ATTACK_RANGE_01 
// 		// 			|| m_wearItemParamFactor[POS_PET].DestParameter2 == DES_ATTACK_RANGE_02 )
// 		// 		{
// 		// 			SetParamFactorDesParam(m_paramFactor, m_wearItemParamFactor[POS_PET].DestParameter2, 
// 		// 				m_wearItemParamFactor[POS_PET].ParameterValue2 );
// 		// 		}
// 		// 
// 		// 		if(m_wearItemParamFactor[POS_PET].DestParameter3 == DES_ATTACK_RANGE_01 
// 		// 			|| m_wearItemParamFactor[POS_PET].DestParameter3 == DES_ATTACK_RANGE_02 )
// 		// 		{
// 		// 			SetParamFactorDesParam(m_paramFactor, m_wearItemParamFactor[POS_PET].DestParameter3, 
// 		// 				m_wearItemParamFactor[POS_PET].ParameterValue3 );
// 		// 		}
// 		// 
// 		// 		if(m_wearItemParamFactor[POS_PET].DestParameter4 == DES_ATTACK_RANGE_01 
// 		// 			|| m_wearItemParamFactor[POS_PET].DestParameter4 == DES_ATTACK_RANGE_02 )
// 		// 		{
// 		// 			SetParamFactorDesParam(m_paramFactor, m_wearItemParamFactor[POS_PET].DestParameter4, 
// 		// 				m_wearItemParamFactor[POS_PET].ParameterValue4 );
// 		// 		}
// 		int nArrParamCnt = 0;
// 		for(nArrParamCnt = 0; nArrParamCnt < SIZE_MAX_DESPARAM_COUNT_IN_ITEM; nArrParamCnt++)
// 		{
// 			if(m_wearItemParamFactor[POS_PET].ArrDestParameter[nArrParamCnt] == DES_ATTACK_RANGE_01 
// 				|| m_wearItemParamFactor[POS_PET].ArrDestParameter[nArrParamCnt] == DES_ATTACK_RANGE_02 )
// 			{
// 				SetParamFactorDesParam(m_paramFactor, m_wearItemParamFactor[POS_PET].ArrDestParameter[nArrParamCnt], 
// 					m_wearItemParamFactor[POS_PET].ArrParameterValue[nArrParamCnt] );
// 			}
// 			
// 		}				
// 		// end 2009-04-21 by bhsohn ľĆŔĚĹŰ DesParamĂß°ˇ	
// 	}		


	// 2010-06-15 by shcho&hslee Ćę˝Ă˝şĹŰ - ŔĺÂřÇ° ĂÖ´ë °łĽö°Ş şŻ°ć.
	//for(int nEqId=0; nEqId<WEAR_ITEM_NUMBER; nEqId++)
	for( int nEqId=0; nEqId < MAX_EQUIP_POS; nEqId++ )
	{
		CItemInfo* pEqItem = m_pStoreData->FindItemInInventoryByWindowPos(nEqId);
		if(pEqItem)
		{			
			int nArrParamCnt = 0;
			for(nArrParamCnt = 0; nArrParamCnt < SIZE_MAX_DESPARAM_COUNT_IN_ITEM; nArrParamCnt++)
			{
				if(m_wearItemParamFactor[nEqId].ArrDestParameter[nArrParamCnt] == DES_ATTACK_RANGE_01 
					|| m_wearItemParamFactor[nEqId].ArrDestParameter[nArrParamCnt] == DES_ATTACK_RANGE_02 )
				{
					SetParamFactorDesParam(m_paramFactor, m_wearItemParamFactor[nEqId].ArrDestParameter[nArrParamCnt], 
						m_wearItemParamFactor[nEqId].ArrParameterValue[nArrParamCnt] );
				}
				
			}							
		}
	}
	// end 2009-05-14 by bhsohn ·ąŔĚ´ő şŻ°ć˝Ă, ĆęŔĚ ľĆ´Ń ľĆŔĚĹŰżˇĽ­ »ç°Ĺ¸® Ŕűżë ľČµÇ´Â ą®Á¦ ÇŘ°á


	// 2011. 2. 25 by jskim ş¸±Ţ »óÁˇ Âř·ú˝Ă ·ąŔĚ´ő ĂĘ±âČ­ µÇ´Â ąö±× ĽöÁ¤
	// Á˘µÎ, Á˘ąĚ ´É·ÂÄˇ Áß ·ąŔĚ´ő °Ĺ¸® µĄ˝şĆÄ¶÷ Ŕűżë
	for( int nEqId = 0; nEqId < MAX_EQUIP_POS; ++nEqId )
	{
		CItemInfo* pEqItem = m_pStoreData->FindItemInInventoryByWindowPos( nEqId );
		if( pEqItem )
		{			
			for( int nArrParamCnt = 0; nArrParamCnt < SIZE_MAX_DESPARAM_COUNT_IN_ITEM; ++nArrParamCnt )
			{
				if( DES_ATTACK_RANGE_01 == m_wearPreRareItemParamFactor[nEqId].ArrDestParameter[nArrParamCnt]					
				||	DES_ATTACK_RANGE_02 == m_wearPreRareItemParamFactor[nEqId].ArrDestParameter[nArrParamCnt] )					
				{
					SetParamFactorDesParam(m_paramFactor,
						m_wearPreRareItemParamFactor[nEqId].ArrDestParameter[nArrParamCnt],
						m_wearPreRareItemParamFactor[nEqId].ArrParameterValue[nArrParamCnt] );
				}
				
				if( DES_ATTACK_RANGE_01 == m_wearSufRareItemParamFactor[nEqId].ArrDestParameter[nArrParamCnt]
				||	DES_ATTACK_RANGE_02 == m_wearSufRareItemParamFactor[nEqId].ArrDestParameter[nArrParamCnt] )
				{
					SetParamFactorDesParam(m_paramFactor,
						m_wearSufRareItemParamFactor[nEqId].ArrDestParameter[nArrParamCnt],
						m_wearSufRareItemParamFactor[nEqId].ArrParameterValue[nArrParamCnt] );
				}
			}
		}
	}
	//end 2011. 2. 25 by jskim ş¸±Ţ »óÁˇ Âř·ú˝Ă ·ąŔĚ´ő ĂĘ±âČ­ µÇ´Â ąö±× ĽöÁ¤


	// 2009-02-25 by bhsohn ąöÇÁÇü »ç°Ĺ¸® ·ąŔĚ´ő ľČµÇ´Â ąö±× ĽöÁ¤
	if(g_pGameMain && g_pGameMain->m_pInfSkill)
	{
		g_pGameMain->m_pInfSkill->RefreshSkillRader();
	}
	// end 2009-02-25 by bhsohn ąöÇÁÇü »ç°Ĺ¸® ·ąŔĚ´ő ľČµÇ´Â ąö±× ĽöÁ¤
}


///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		// żŁÁř ŔÎĂľ Á¤ş¸ °»˝Ĺ
/// \author		// 2008-09-26 by bhsohn ˝Ĺ±Ô ŔÎĂľĆ® Ăł¸®
/// \date		2008-09-26 ~ 2008-09-26
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
CParamFactor* CShuttleChild::GetShuttleParamFactor()
{
	return &m_paramFactor;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		Á¶ŔĚ˝şĆ˝
/// \author		// 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
/// \date		2008-11-13 ~ 2008-11-13
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CShuttleChild::SetCursorJostickPos()
{	
	CJoyStickInput *pJoyStick = g_pD3dApp->GetJoystickControl();
	if(!pJoyStick)
	{
		// Á¶ŔĚ˝şĆ˝ ŔĺÄˇ ľřŔ˝
		return;
	}
	
	//A±âľî°ˇ ¶Ążˇ ŔÖŔ»¶§
//	if(m_bIsAir == FALSE && IS_DT(m_myShuttleInfo.UnitKind))
//	{
//	}
//	else 
//	{
//		SetAirCursorJostickPos();
//	}
	// 2009-02-25 by bhsohn Á¶ŔĚ˝şĆ˝ ŔĚżë, ¸Ţ´şĂ˘ ¶ěżě¸é A±âľî´Â ¸¶żě˝ş·Î Á¶Ŕý
	if(IS_DT(m_myShuttleInfo.UnitKind))
	{
		if(g_pGameMain)
		{
			GUI_BUILDINGNPC* pCurrentBuildingNpc = NULL;;
			if(g_pInterface->m_pCityBase)
			{
				//ÇöŔç ľó¸° »óÁˇŔĚŔÖłÄ?
				pCurrentBuildingNpc = g_pInterface->m_pCityBase->GetCurrentBuildingNPC();
			}

			if((g_pGameMain->m_pCommunity && g_pGameMain->m_pCommunity->IsShowCommunityWindow())
				|| (g_pGameMain->m_pCharacterInfo && g_pGameMain->m_pCharacterInfo->IsShowWnd())
				|| (g_pGameMain->m_pInven && g_pGameMain->m_pInven->IsShowInven())
				|| (g_pGameMain->m_pSystem && g_pGameMain->m_pSystem->IsOptionShow())
				|| (pCurrentBuildingNpc))
			{
				// Ă˘ ¶ěżě°í ŔÖŔ¸¸é Á¶ŔĚ˝şĆ˝ľČµĘ
				return;
			}
		}
	}
	// end 2009-02-25 by bhsohn Á¶ŔĚ˝şĆ˝ ŔĚżë, ¸Ţ´şĂ˘ ¶ěżě¸é A±âľî´Â ¸¶żě˝ş·Î Á¶Ŕý

	SetAirCursorJostickPos();
	
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		Á¶ŔĚ˝şĆ˝
/// \author		// 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
/// \date		2008-11-13 ~ 2008-11-13
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CShuttleChild::SetAirCursorJostickPos()
{
	if(!IsPossibleJoystickMove())
	{
		return;
	}
	
	FLOG( "CShuttleChild::SetCursorInit()" );
	POINT pt,pt2;
	GetCursorPos(&pt);
	pt2 = pt;
	
	ScreenToClient(g_pD3dApp->GetHwnd(), &pt);
	
	int nMaxX = g_pD3dApp->GetBackBufferDesc().Width/2;
	int nMaxY = g_pD3dApp->GetBackBufferDesc().Height/2;;
	
	int nX = pt2.x - pt.x;
	int nY = pt2.y - pt.y;
	{
		// °ˇżîµĄ żµżŞ
		nX += nMaxX;
		nY += nMaxY;
	}
	
	int nJoyStick = 0;
	int nCurselValue = 0;
	
	nJoyStick = SetCursorJostickLeftRight(JOSTICK_OPTION_MOUSE_LEFT, nMaxX);	// ¸¶żě˝ş żŢÂĘ ĂĽĹ©
	nX += nJoyStick;	
	
	nJoyStick = SetCursorJostickLeftRight(JOSTICK_OPTION_MOUSE_RIGHT, nMaxX);	// ¸¶żě˝ş żŔ¸ĄÂĘ ĂĽĹ©
	nX += nJoyStick;		
	
	nJoyStick = SetCursorJostickUpDown(JOSTICK_OPTION_MOUSE_UP, nMaxY);			// ¸¶żě˝ş Ŕ§·Î ĂĽĹ©
	nY += nJoyStick;	
	
	nJoyStick = SetCursorJostickUpDown(JOSTICK_OPTION_MOUSE_DOWN, nMaxY);			// ¸¶żě˝ş ľĆ·ˇ·Î ĂĽĹ©
	nY += nJoyStick;	
	
	
	SetCursorPos(nX,nY);
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		żŢÂĘ, żŔ¸ĄÂĘ ĂĽĹ©
/// \author		// 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
/// \date		2008-11-13 ~ 2008-11-13
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
int  CShuttleChild::SetCursorJostickLeftRight(int nLeftRightIndex, int nMaxX)
{
	CJoyStickInput *pJoyStick = g_pD3dApp->GetJoystickControl();
	if(!pJoyStick)
	{
		// Á¶ŔĚ˝şĆ˝ ŔĺÄˇ ľřŔ˝
		return 0;
	}
	else if(g_pGameMain && g_pGameMain->IsShowOpJoystick())
	{
		// Á¶ŔĚ˝şĆ˝ żÉĽÇĂ˘ŔĚ ¶°ŔÖ´Ů.
		return 0;
	}

	int nRtnJoyStatus = 0;
	int nJoyStick = 0;
	
	int nKeyIdx, nKeyValue;
	float fTmpVaule = 0.0f;
	nKeyIdx = nKeyValue = 0;

	// ¸¶żě˝ş Á¦ľî´Â ÇŃ±şµĄ ąŰżˇ ¸řłÖ´Â´Ů.	
	g_pInterface->GetJoystickInfo(nLeftRightIndex, 0, &nKeyIdx, &nKeyValue);
	if(-1 == nKeyIdx)
	{
		return nRtnJoyStatus;
	}
		
	// ČŮąć˝ÄŔÎłÄ Ĺ° ąć˝ÄŔĚłÄ¸¦ ĂĽĹ©
	if(pJoyStick->IsJoystickWheel(nKeyIdx))
	{
		// ČŮ˝şĹ©·Ń ąć˝ÄŔĚ´Ů.
		nJoyStick = pJoyStick->GetJoySitckInfo(nKeyIdx);
		if((nKeyValue >= 0 && nJoyStick >= 0)
			|| (nKeyValue <= 0 && nJoyStick <= 0))
		{
			fTmpVaule = ((float)nJoyStick)/JOYSTIC_STEP_MAX_VALUE;	// şńŔ˛°č»ę					
			if(fTmpVaule > 1.0f)
			{
				fTmpVaule = 1.0f;
			}
			else if(fTmpVaule < -1.0f)
			{
				fTmpVaule = -1.0f;
			}		
			fTmpVaule = GetJoystckMoveValue(fTmpVaule);			
		}				
		
		fTmpVaule *= ((float)nMaxX);		
		nRtnJoyStatus = (int)fTmpVaule;
	}
	else
	{
		// Ĺ° Ĺ¬¸Ż ąć˝Ä ŔĚ´Ů.
		nJoyStick = pJoyStick->GetJoySitckInfo(nKeyIdx);
		if(nJoyStick > 0)
		{				
			if(JOSTICK_OPTION_MOUSE_LEFT == nLeftRightIndex)
			{
				nRtnJoyStatus = -1*nMaxX;
			}
			else
			{
				nRtnJoyStatus = nMaxX;
			}				
		}
	}
	
	return nRtnJoyStatus;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		żŢÂĘ, żŔ¸ĄÂĘ ĂĽĹ©
/// \author		// 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
/// \date		2008-11-13 ~ 2008-11-13
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
int  CShuttleChild::SetCursorJostickUpDown(int nUpDownIndex, int nMaxY)
{
	CJoyStickInput *pJoyStick = g_pD3dApp->GetJoystickControl();
	if(!pJoyStick)
	{
		// Á¶ŔĚ˝şĆ˝ ŔĺÄˇ ľřŔ˝
		return 0;
	}
	else if(g_pGameMain && g_pGameMain->IsShowOpJoystick())
	{
		// Á¶ŔĚ˝şĆ˝ żÉĽÇĂ˘ŔĚ ¶°ŔÖ´Ů.
		return 0;
	}
	int nRtnJoyStatus = 0;
	int nJoyStick = 0;
	
	int nKeyIdx, nKeyValue;
	float fTmpVaule = 0.0f;
	nKeyIdx = nKeyValue = 0;

	// ¸¶żě˝ş Á¦ľî´Â ÇŃ±şµĄ ąŰżˇ ¸řłÖ´Â´Ů.	
	g_pInterface->GetJoystickInfo(nUpDownIndex, 0, &nKeyIdx, &nKeyValue);
	if(-1 == nKeyIdx)
	{
		return nRtnJoyStatus;
	}
		
	// ČŮąć˝ÄŔÎłÄ Ĺ° ąć˝ÄŔĚłÄ¸¦ ĂĽĹ©
	if(pJoyStick->IsJoystickWheel(nKeyIdx))
	{
		// ČŮ˝şĹ©·Ń ąć˝ÄŔĚ´Ů.
		nJoyStick = pJoyStick->GetJoySitckInfo(nKeyIdx);
		if((nKeyValue >= 0 && nJoyStick >= 0)
			|| (nKeyValue <= 0 && nJoyStick <= 0))
		{
			fTmpVaule = ((float)nJoyStick)/JOYSTIC_STEP_MAX_VALUE;	// şńŔ˛°č»ę					
			if(fTmpVaule > 1.0f)
			{
				fTmpVaule = 1.0f;
			}			
			else if(fTmpVaule < -1.0f)
			{
				fTmpVaule = -1.0f;
			}			
			fTmpVaule = GetJoystckMoveValue(fTmpVaule);
		}				
		
		fTmpVaule *= ((float)nMaxY);		
		nRtnJoyStatus = (int)fTmpVaule;
	}
	else
	{
		// Ĺ° Ĺ¬¸Ż ąć˝Ä ŔĚ´Ů.
		nJoyStick = pJoyStick->GetJoySitckInfo(nKeyIdx);
		if(nJoyStick > 0)
		{				
			if(JOSTICK_OPTION_MOUSE_UP == nUpDownIndex)
			{
				nRtnJoyStatus = -1*nMaxY;
			}
			else
			{
				nRtnJoyStatus = nMaxY;
			}				
		}
	}
	
	return nRtnJoyStatus;
}


///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		·Ń¸µŔĚ °ˇ´ÉÇŃ »óĹÂ´Ů
/// \author		// 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
/// \date		2008-11-13 ~ 2008-11-13
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CShuttleChild::IsPossibleRolling()
{
	if((g_pTutorial->IsTutorialMode() == FALSE 
		|| (g_pTutorial->IsTutorialMode() == TRUE && g_pTutorial->GetLesson() >= L3)) 
		&&m_bIsAir == TRUE 
		&& m_dwState == _NORMAL)
	{		
		return TRUE;
	}
	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		·Ń¸µŔĚ °ˇ´ÉÇŃ »óĹÂ´Ů
/// \author		// 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
/// \date		2008-11-13 ~ 2008-11-13
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CShuttleChild::StartRolling(int nKeyIndex)
{
	if(!IsPossibleRolling())
	{
		return;
	}
	switch(nKeyIndex)
	{
	case DIK_A:
		{
			/*if(m_fADoubleKeyTime < DOUBLEKEY_TIME 
				&& m_bKeyDownACheck == FALSE 
				// şÎ˝şĹÍ »çżë˝Ăżˇ´Â ·Ń¸µŔ» ŔúŔĺÇĎÁö ľĘ´Â´Ů.
				&& m_nBooster != BOOSTER_TYPE_MAX)
			{
				m_bKeyDownADouble = TRUE;
				m_bKeyDownDDouble = FALSE;
			}
			else
			{
				m_fADoubleKeyTime = 0.0f;
			}*/
			ActiveKeyDownADouble();
		}
		break;
	case DIK_D:
		{
			ActiveKeyDownDDouble();
			//if(m_fDDoubleKeyTime < DOUBLEKEY_TIME 
			//	&& m_bKeyDownDCheck == FALSE
			//	// şÎ˝şĹÍ »çżë˝Ăżˇ´Â ·Ń¸µŔ» ŔúŔĺÇĎÁö ľĘ´Â´Ů.
			//	&& m_nBooster != BOOSTER_TYPE_MAX)
			//{
			//	m_bKeyDownDDouble = TRUE;
			//	m_bKeyDownADouble = FALSE;
			//}
			//else
			//{
			//	m_fDDoubleKeyTime = 0.0f;
			//}
		}
		break;
	}

}
///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		·Ń¸µŔ» ĂĽĹ©ÇŃ´Ů.
/// \author		// 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
/// \date		2008-11-13 ~ 2008-11-13
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CShuttleChild::CheckJoystickKeyDown()
{
	CJoyStickInput *pJoyStick = g_pD3dApp->GetJoystickControl();
	if(!pJoyStick)
	{
		// Á¶ŔĚ˝şĆ˝ ŔĺÄˇ ľřŔ˝
		return;
	}
	else if(!IsPossibleJoystickMove())
	{
		// şńÇŕÁßŔĚ ľĆ´Ď´Ů.
		return;
	}

	int nJoyStickRolling = 0;	
	BOOL bArrowKey = FALSE;

	if( g_pInterface->GetJostickStats(DIK_A) )
	{
		bArrowKey = TRUE;
		nJoyStickRolling = DIK_A;
		if(g_pInterface->IsChangeJoySitckInfo(DIK_A) 
			&& (0 == m_nJoyStickRolling))
		{			
			StartRolling(DIK_A);	// ·Ń¸µ ˝ĂŔŰ ĂĽĹ©
		}
	}
	else if(g_pInterface->GetJostickStats(DIK_D))
	{
		bArrowKey = TRUE;
		nJoyStickRolling = DIK_D;
		if(g_pInterface->IsChangeJoySitckInfo(DIK_D)
			&& (0 == m_nJoyStickRolling))
		{			
			StartRolling(DIK_D);	// ·Ń¸µ ˝ĂŔŰ ĂĽĹ©
		}
	}

	if(g_pInterface->GetJostickStats(DIK_LSHIFT) 
		&& g_pInterface->IsChangeJoySitckInfo(DIK_LSHIFT))
	{
		// Target UnLock
		SetJoystickUnLockOn();
	}

	int nSlotIdx = 0;
	for(nSlotIdx = 0;nSlotIdx < MAX_JOSTICK_OPTION_BEHAVIOR;nSlotIdx++)
	{
		{
			// 1Çüą«±â
			BOOL bWeaponChangeKey1 = FALSE;
			int nKeyIdx, nKeyValue;
			nKeyIdx = nKeyValue = 0;
			
			g_pInterface->GetJoystickInfo(JOSTICK_OPTION_WEAPON_1, nSlotIdx, &nKeyIdx, &nKeyValue);
			
			if(nKeyIdx != -1)
			{
				bWeaponChangeKey1 = pJoyStick->IsChangeJoySitckInfo(nKeyIdx) ;
			}
			
			if(bWeaponChangeKey1)
			{
				int nJoyStickStas = pJoyStick->GetJoySitckInfo(nKeyIdx);
				
				// ±âş»ą«±â
				if(nJoyStickStas)
				{
					if(	m_pSkill && m_pSkill->GetSkillTargetState() == TRUE )
					{
						//	ł»°ˇ Áö±Ý Ĺ¸°Ů ˝şĹłŔ» »çżëÇĎ±â Ŕ§ÇŘ Ĺ¸°ŮŔ» ĂŁ´Â ÁßŔĚ¶ó¸é..
						//	ĂŃľËŔĚ ąß»çµÇÁö ľĘ°í, ´ë˝Ĺżˇ ˝şĹł Ĺ¬·ˇ˝şżˇ ŔÖ´Â Ĺ¸°ŮŔ» ĂŁ´Â ÇÔĽö¸¦ ČŁĂâÇŃ´Ů.
						m_pSkill->FindTargetForSkill();
					}
					// 2009-02-19 by bhsohn Ä«¸Ţ¶ó ˝ĂĽ± şŻ°ćČÄ, ˝±°Ô »çłÉ ąćąý ¸·Ŕ˝					
					//else if(!m_bLButtonState  && (IS_WEAPON_ENABLESTATE(m_dwState, m_nBooster)))
					else if(!m_bLButtonState  && (IS_WEAPON_ENABLESTATE(m_dwState, m_nBooster))&& IsPossibleAttackStat())
					{
						//±âş» ą«±â ąß»ç
						m_bLButtonState = TRUE;
					}
				}			
				else if(m_bLButtonState && !nJoyStickStas)
				{
					// ±âş»ą«±â ąß»ç ĂëĽŇ
					m_bLButtonState = FALSE;
					m_PrimaryAttack.AttackData.TargetInfo.SetNullTarget();
					m_bTargetChange = TRUE;
					
				}
			}
		}
		{
			// 2Çüą«±â
			BOOL bWeaponChangeKey2 = FALSE;
			int nKeyIdx, nKeyValue;
			nKeyIdx = nKeyValue = 0;
			
			g_pInterface->GetJoystickInfo(JOSTICK_OPTION_WEAPON_2, nSlotIdx, &nKeyIdx, &nKeyValue);
			
			if(nKeyIdx != -1)
			{
				bWeaponChangeKey2 = pJoyStick->IsChangeJoySitckInfo(nKeyIdx) ;
			}
			
			if(bWeaponChangeKey2)
			{
				int nJoyStickStas = pJoyStick->GetJoySitckInfo(nKeyIdx);
				// °í±Ţą«±â
				// 2009-02-19 by bhsohn Ä«¸Ţ¶ó ˝ĂĽ± şŻ°ćČÄ, ˝±°Ô »çłÉ ąćąý ¸·Ŕ˝									
				//if(!m_bRButtonState && nJoyStickStas)
				if(!m_bRButtonState && nJoyStickStas && IsPossibleAttackStat())
				{			
					m_bRButtonState = TRUE;
				}
				else if(m_bRButtonState && !nJoyStickStas)
				{			
					m_bRButtonState = FALSE;
					m_SecondaryAttack.AttackData.TargetInfo.SetNullTarget();
					
				}		 
			}	
		}
		{
			// •üşä		
			BOOL bKeyDown = FALSE;	
			BOOL bChangeKey = FALSE;
			int nKeyIdx, nKeyValue;
			int nJoyStickStas =0;
			nKeyIdx = nKeyValue = 0;
			
			g_pInterface->GetJoystickInfo(JOSTICK_OPTION_BACKVIEW, nSlotIdx, &nKeyIdx, &nKeyValue);
			
			if(nKeyIdx != -1)
			{
				bChangeKey = pJoyStick->IsChangeJoySitckInfo(nKeyIdx) ;
			}
			
			if(bChangeKey)
			{
				nJoyStickStas = pJoyStick->GetJoySitckInfo(nKeyIdx);
				if( nJoyStickStas&& !m_bTurnCamera)
				{
					SetBackView(TRUE);
				}
				else if(m_bTurnCamera)
				{
					SetBackView(FALSE);
				}
			}			
		}
	}
	
	

	{
		// ŔŻ´ÖÁ¤Áö
		if(g_pInterface->OnJoystickKeyDown(JOSTICK_OPTION_UNITSTOP))
		{
			OnKeyDownUnitStop();
		}
	}

	{
		// Ăâµż
		if(g_pInterface->OnJoystickKeyDown(JOSTICK_OPTION_TAKEOFF))
		{			
		}
	}

	
	{
		// Ćí´ë ´ëÇü Âüż©
		if(g_pInterface->OnJoystickKeyDown(JOSTICK_OPTION_JOIN_FORMATION))
		{
			OnKeyDownJoinFormation();
		}
	}
	{
		// ˝şĹł ˝˝·Ô şŻ°ć 
		if(g_pInterface->OnJoystickKeyDown(JOSTICK_OPTION_SLOT_CHANGE))
		{
			if(g_pGameMain && g_pGameMain->m_pQuickSlot)
			{
				g_pGameMain->m_pQuickSlot->AddCurrentTab();
			}
			
		}
	}
	// 2009-01-16 by bhsohn Á¶ŔĚ˝şĆ˝ Ăß°ˇ ˝ÂŔÎ/ĂëĽŇ Ăß°ˇ
	{
		// ˝ÂŔÎ ąöĆ°
		if(g_pInterface->OnJoystickKeyDown(JOSTICK_OPTION_OK))
		{
			// if i==0 then ok, if i==1 then cancel
			g_pGameMain->m_pInfWindow->OnButtonClick(0);
		}		
		// ĂëĽŇ ąöĆ°
		else if(g_pInterface->OnJoystickKeyDown(JOSTICK_OPTION_CANCEL))
		{
			g_pGameMain->m_pInfWindow->OnButtonClick(1);
		}
	}
	// end 2009-01-16 by bhsohn Á¶ŔĚ˝şĆ˝ Ăß°ˇ ˝ÂŔÎ/ĂëĽŇ Ăß°ˇ

	m_nJoyStickRolling = nJoyStickRolling;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		·Ń¸µŔ» ĂĽĹ©ÇŃ´Ů.
/// \author		// 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
/// \date		2008-11-13 ~ 2008-11-13
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CShuttleChild::SetJoystickUnLockOn()
{
	SetUnLockOn();
	
	if( IsPkEnableMap(m_myShuttleInfo.MapChannelIndex.MapIndex) )
	{
		SetPKMode( FALSE );
	}	
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		°¨µµ¸¦ °ˇÁ®żÂ´Ů.
/// \author		// 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
/// \date		2008-11-13 ~ 2008-11-13
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
float CShuttleChild::GetJoystckMoveValue(float i_fValue)
{
	float fDstValue = i_fValue;
	CJoyStickInput *pJoyStick = g_pD3dApp->GetJoystickControl();
	
	if(!pJoyStick && !g_pD3dApp->IsUseJoyStick())
	{
		// Á¶ŔĚ˝şĆ˝ ŔĺÄˇ ľřŔ˝
		return fDstValue;
	}	

	fDstValue = pJoyStick->GetSensity(g_pJoysticOp->nJoysticSense, i_fValue);

	return fDstValue;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		ŔŻ´Ö ˝şĹé
/// \author		// 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
/// \date		2008-11-13 ~ 2008-11-13
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CShuttleChild::OnKeyDownUnitStop()
{	
	// ŔŻ´ÖÁ¤Áö ˝Ă˝şĹŰ	
	// ±âľîŔÇ »óĹÂ°ˇ Á×Ŕş »óČ˛ŔĎ¶§´Â BĹ°´Â ¸·´Â´Ů.
	if(CheckUnitState() == BREAKDOWN)
	{
		return;
	}
	else if(m_bIsAir == FALSE && IS_DT(m_myShuttleInfo.UnitKind)			//A±âľî°ˇ ¶Ążˇ ŔÖŔ»¶§
		|| g_pD3dApp->m_dwGameState == _SHOP						//»óÁˇżˇ ŔÖŔ»¶§
		|| m_bAttackMode == _SIEGE)									//˝ĂÁî ¸đµĺŔĎ¶§ 
	{
		return;
	}
	// 2005-10-06 by ispark
	// Ć©Ĺä¸®ľóŔĎ¶§´Â Á¤Áö »çżë ľČÇÔ
	// Ćí´ë şńÇŕŔĎ¶§ Ćí´ëŔĺ ŔĚżÜ´Â Á¤˝Ă »çżë ±ÝÁö
	// ±âľî°ˇ LANDEDŔĎ¶§ Á¤Áö »çżë ľČÇÔ
	// ĆĐĹĎ »çżëÁßżˇ´Â Á¤Áö »çżë ľČÇÔ
	if((g_pTutorial->IsTutorialMode() == FALSE || 
		(g_pTutorial->GetLesson() == L2 && g_pTutorial->GetChapter() == L2_2 && g_pTutorial->GetChapterState() == TUTORIAL_STATE_MOTION)) &&
		m_pClientParty->IsFormationFlightMove() == FALSE &&
		m_bLandedMove == FALSE &&
		m_bSkillMoveIsUse == FALSE)
	{
		if(m_bUnitStop == FALSE)	
		{			
			// ŔŻ´Ö Á¤Áö
			g_pShuttleChild->UnitStop();
		}
		else if(m_bSkillGoMove)	// Ăâąß ÇŇ ¶§
		{
			// 2008-07-03 by bhsohn EP3 ąö±× ¸®Ć÷Ć® ĽöÁ¤
			if(g_pGameMain->m_pCommunity->IsShowCommunityWindow())
			{
				// Ăâąß˝Ă °­Á¦·Î Ă˘´ÝŔ˝
				g_pGameMain->m_pCommunity->ShowCommunityWindow(FALSE);
			}
			if(g_pGameMain->m_pSystem && g_pGameMain->m_pSystem->IsOptionShow())
			{
				g_pGameMain->PopupSystemOptinWnd();
			}
			// end 2008-07-03 by bhsohn EP3 ąö±× ¸®Ć÷Ć® ĽöÁ¤
			
			m_bUnitStop = FALSE;
			m_bPenaltyUnitStop = TRUE;
			m_bMouseMoveLock = FALSE;
			m_nBooster = BOOSTER_TYPE_MIN;
			g_pInterface->m_pGameMain->InitShowWindow();
			// 2010. 03. 30 by jskim ŔŻ´Ö »óĹÂ ąŮµđÄÁµđĽÇ Ľ­ąö Ŕü´Ţ
			g_pShuttleChild->SendFieldSocketChangeBodyCondition( m_myShuttleInfo.ClientIndex, BODYCON_SET_OR_CLEAR_MASK );
			//end 2010. 03. 30 by jskim ŔŻ´Ö »óĹÂ ąŮµđÄÁµđĽÇ Ľ­ąö Ŕü´Ţ
		}						
	}	
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		ŔŻ´Ö ˝şĹé
/// \author		// 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
/// \date		2008-11-13 ~ 2008-11-13
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CShuttleChild::OnKeyDownJoinFormation()
{	
	// 2004-11-26 by jschoi - _GAME »óĹÂ°ˇ ľĆ´Ď°ĹłŞ żöÇÁÁßżˇ´Â ´ëÇüżˇ Âüż©ÇŇ Ľö ľř´Ů.
	if( g_pD3dApp->m_dwGameState != _GAME ||
		m_nCurrentPatternNumber == PATTERN_UNIT_WARP_OUT ||
		m_nCurrentPatternNumber == PATTERN_UNIT_WARP_IN 
		|| IS_CITY_MAP_INDEX(g_pD3dApp->GetMyShuttleMapChannelIndex().MapIndex)) // 2007-06-26 by bhsohn ¸¶Ŕ»żˇĽ­ Ćí´ë şńÇŕ µÇ´Â ąö±× ĽöÁ¤
	{
		DBGOUT("Current Game State (%d) , and PatternType is (%d), So Can't join FormationFlight.\n",g_pD3dApp->m_dwGameState,m_nCurrentPatternNumber);
		return;
	}
	
	if( m_pClientParty->GetPartyInfo().bFormationFlyingType && 
		!m_pClientParty->GetPartyInfo().bFormationFlyingPositionType && 
		m_pClientParty->GetPartyInfo().bPartyType == _PARTYMEMBER &&
		m_dwState == _NORMAL &&
		m_bAttackMode != _SIEGE &&
		(FALSE == IS_DT(m_myShuttleInfo.UnitKind) || m_bIsAir))
	{
		vector<PARTYENEMYINFO*>::iterator itParty = m_pClientParty->m_vecPartyEnemyInfo.begin();
		while(itParty != m_pClientParty->m_vecPartyEnemyInfo.end())
		{
			if(	(*itParty)->m_pEnemyData &&
				(*itParty)->m_bPartyType == _PARTYMASTER)
			{
								
				if( D3DXVec3Length(&((*itParty)->m_pEnemyData->m_vPos - m_vPos)) < 300.0f &&
					(*itParty)->m_pEnemyData->m_dwState == _NORMAL)
				{
					m_pClientParty->ISendPartyGetFlightPosition();
									
					// 2005-10-24 by ispark
					// Ćí´ëşńÇŕ ˝ĹĂ»ŔĎ ¶§ ±âľî Á¤Áö Ç¬´Ů.
					m_bUnitStop = FALSE;
					m_nBooster = BOOSTER_TYPE_MIN;
					m_bPenaltyUnitStop = FALSE;
					m_bMouseLock = FALSE;
					m_bMouseMoveLock = FALSE;
					g_pD3dApp->m_bFixUnitDirMode = FALSE;
					g_pInterface->m_pGameMain->InitShowWindow();
					// 2010. 03. 30 by jskim ŔŻ´Ö »óĹÂ ąŮµđÄÁµđĽÇ Ľ­ąö Ŕü´Ţ	
					SendFieldSocketChangeBodyCondition( m_myShuttleInfo.ClientIndex, BODYCON_SET_OR_CLEAR_MASK );
					//end 2010. 03. 30 by jskim ŔŻ´Ö »óĹÂ ąŮµđÄÁµđĽÇ Ľ­ąö Ŕü´Ţ	
					m_ptDirOld.x = 0;
					m_ptDirOld.y = 0;
				}
				else if((*itParty)->m_pEnemyData->m_dwState != _NORMAL)
				{
					char buf[256];
					wsprintf(buf,STRMSG_C_PARTY_0042);//"Ćí´ëŔĺŔĚ Ćí´ëşńÇŕŔ» ÇŇ Ľö ľř´Â »óĹÂŔÔ´Ď´Ů."
					g_pD3dApp->m_pChat->CreateChatChild(buf,COLOR_SYSTEM);
				}
				else
				{
					char buf[256];
					wsprintf(buf,STRMSG_C_PARTY_0043);//"Ćí´ëŔĺŔ¸·ÎşÎĹÍ °Ĺ¸®°ˇ łĘą« ¸Ö¸® ¶łľîÁ® ŔÖ˝Ŕ´Ď´Ů."
					g_pD3dApp->m_pChat->CreateChatChild(buf,COLOR_SYSTEM);
				}
				return;
			}
			//					itEnemy++;
			itParty++;
		}
	}
	else if(!m_pClientParty->GetPartyInfo().bFormationFlyingType)
	{//
		char buf[256];
		wsprintf(buf,STRMSG_C_PARTY_0044);//"Ćí´ë şńÇŕ ´ëÇü Ľ±ĹĂŔĚ Ŕß¸řµÇľú˝Ŕ´Ď´Ů."
		g_pD3dApp->m_pChat->CreateChatChild(buf,COLOR_SYSTEM);
	}
	else if(m_pClientParty->GetPartyInfo().bFormationFlyingPositionType)
	{
		char buf[256];
		wsprintf(buf,STRMSG_C_PARTY_0045);//"Ćí´ë şńÇŕ Ć÷ÁöĽÇŔĚ Ŕß¸řµÇľú˝Ŕ´Ď´Ů."
		g_pD3dApp->m_pChat->CreateChatChild(buf,COLOR_SYSTEM);
	}
	else if(m_pClientParty->GetPartyInfo().bPartyType != _PARTYMEMBER)
	{
		char buf[256];
		wsprintf(buf,STRMSG_C_PARTY_0046);//"ÇöŔç Ćí´ëżřŔĚ ľĆ´Ő´Ď´Ů. Č®ŔÎÇŘ ÁÖ˝Ă±â ąŮ¶ř´Ď´Ů."
		g_pD3dApp->m_pChat->CreateChatChild(buf,COLOR_SYSTEM);
	}
	else
	{
		char buf[256];
		wsprintf(buf,STRMSG_C_PARTY_0047);//"Ćí´ë şńÇŕŔ» ÇŇ Ľö ľř´Â »óĹÂŔÔ´Ď´Ů."
		g_pD3dApp->m_pChat->CreateChatChild(buf,COLOR_SYSTEM);
	}
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		Á¶ŔĚ˝şĆ˝Ŕ¸·Î żňÁ÷ŔĎĽö ŔÖ´Â »óĹÂłÄ?
/// \author		// 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
/// \date		2008-11-13 ~ 2008-11-13
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CShuttleChild::IsPossibleJoystickMove()
{
	if((TRUE == g_pD3dApp->m_pKeyBoard->GetKeyBoardLostDevice())
		||(FALSE == g_pD3dApp->m_pKeyBoard->GetKeyBoardFocus())) // Ć÷Äż˝ş ľřŔ»˝Ă, °­Á¦ Á¶ŔĚ˝şĆ˝ »çżëľČÇÔ
	{
		return FALSE;
	}
	else if(g_pGameMain && g_pGameMain->IsShowOpJoystick())
	{
		// Á¶ŔĚ˝şĆ˝ żÉĽÇĂ˘ŔĚ ¶°ŔÖ´Ů.
		return FALSE;
	}
	else if(g_pD3dApp->IsPossibleWindowMove())
	{
		if(m_bIsAir == FALSE && IS_DT(m_myShuttleInfo.UnitKind))
		{	
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	return TRUE;	
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		ą«ÇŃŔ¸·Î ĂŃľË łŞ°ˇ´Â Çö»ó Ăł¸®
/// \author		// 2008-12-04 by bhsohn ą«ÇŃŔ¸·Î ĂŃľË łŞ°ˇ´Â Çö»ó Ăł¸®
/// \date		2008-12-04 ~ 2008-12-04
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CShuttleChild::IsWeapongShotting()
{
	if(TRUE == g_pD3dApp->IsMyShuttleCharacter())
	{
		return FALSE;
	}
	else if((TRUE == m_bRButtonState)
		||(TRUE == m_bLButtonState))
	{
		return TRUE;
	}
	return FALSE;

}

///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		Ä«¸Ţ¶ó ˝ĂĽ± şŻ°ćČÄ, ˝±°Ô »çłÉ ąćąý ¸·Ŕ˝
/// \author		// 2009-02-19 by bhsohn Ä«¸Ţ¶ó ˝ĂĽ± şŻ°ćČÄ, ˝±°Ô »çłÉ ąćąý ¸·Ŕ˝
/// \date		2009-02-19 ~ 2009-02-19
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CShuttleChild::TickCheckGearAttackStat()
{
	//float GetOriDistance() {return m_fOriDistance;}// µđĆúĆ® Ä«¸Ţ¶óŔÇ ĽĹĆ˛·ÎşÎĹÍ °Ĺ¸®
	if(IsPossibleAttackStat())
	{
		return;		
	}
	if(m_bLButtonState)
	{
		// ±âş»ą«±â ąß»ç ĂëĽŇ
		m_bLButtonState = FALSE;
		m_PrimaryAttack.AttackData.TargetInfo.SetNullTarget();
		m_bTargetChange = TRUE;
		
	}
	if(m_bRButtonState)
	{			
		m_bRButtonState = FALSE;
		m_SecondaryAttack.AttackData.TargetInfo.SetNullTarget();
		
	}	

}

///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		ÇöŔç °ř°Ý °ˇ´ÉÇŃ »óĹÂłÄ?
/// \author		// 2009-02-19 by bhsohn Ä«¸Ţ¶ó ˝ĂĽ± şŻ°ćČÄ, ˝±°Ô »çłÉ ąćąý ¸·Ŕ˝
/// \date		2009-02-19 ~ 2009-02-19
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CShuttleChild::IsPossibleAttackStat()
{
	if(GetAsyncKeyState(VK_LMENU))
		return FALSE;

	if(NULL == g_pCamera)
	{
		return TRUE;
	}		
	// // µđĆúĆ® Ä«¸Ţ¶óŔÇ ĽĹĆ˛·ÎşÎĹÍ °Ĺ¸®
	if(abs(g_pCamera->GetCamHeight()) <= 1000.0f && g_pCamera->GetCamDistance() <= (g_pCamera->GetOriDistance()+ATTACK_POSSIBLE_DISTANCE)) // ľŕ°ŁŔÇ ż©ąéŔ» µÎŔÚ ż©ąéŔÇ ą¦ąĚ;;;
	{
		return TRUE;
	}
	return FALSE;
}

// 2010. 04. 15 by jskim şÎ˝şĹÍ ĂÖĽŇ ĽÓµµ, ĂÖ°í ĽÓµµ 2ąč µÇ´Â ąö±× ĽöÁ¤
// 2010. 03. 25 by jskim Speed, Angle Ăß°ˇ °č»ę
void CShuttleChild::RecalculateSpeedAndAngle()
{
// 	// żŁÁř ľĆŔĚĹŰ °ˇÁ®żŔ±â
// 	CItemInfo* pEngineItem = g_pStoreData->FindItemInInventoryByWindowPos( POS_REAR ); 
// 	
// 	if(!pEngineItem)
// 	{
// 		m_fShuttleSpeed = 0.0f;
// 		m_fShuttleSpeedMax = 10.0f;
// 		m_fShuttleSpeedBoosterOn = 10.0f;
// 		m_fShuttleSpeedMin = 0.0f;
// 		m_bySpeedPenalty = 0;							// 2005-08-16 by ispark
// 		m_fBURN = 1;
// 		m_fCurrentBURN = 1;
// 		m_fMouseRate = 0.5f;
// 		return;
// 	}
// 
// 	ITEM* pITEM = pEngineItem->GetRealItemInfo();
// 
// 	m_fBURN						= pITEM->Time/1000.0f	+ GetEnchantDesParam(pEngineItem, DES_ENGINE_BOOSTER_TIME_UP);
// 	m_fShuttleSpeedMin			= pITEM->AbilityMin		+ GetEnchantDesParam(pEngineItem, DES_ENGINE_MIN_SPEED_UP);
// 	m_fShuttleSpeedMax			= pITEM->AbilityMax		+ GetEnchantDesParam(pEngineItem, DES_ENGINE_MAX_SPEED_UP);
// 	m_fShuttleGroundSpeedMax	= pITEM->Charging		+ GetEnchantDesParam(pEngineItem, DES_ENGINE_GROUND_SPEED_UP);
// 	m_fShuttleSpeedBoosterOn	= (float)pITEM->Range	+ GetEnchantDesParam(pEngineItem, DES_ENGINE_BOOSTER_SPEED_UP);
// 
// 	if(pEngineItem->GetEnchantParamFactor())
// 	{
// 		m_paramFactor.pfm_ENGINE_ANGLE_UP			= GetParamFactor_DesParam( *pEngineItem->GetEnchantParamFactor(), DES_ENGINE_ANGLE_UP);
// 		m_paramFactor.pfm_ENGINE_BOOSTERANGLE_UP	= GetParamFactor_DesParam( *pEngineItem->GetEnchantParamFactor(), DES_ENGINE_BOOSTERANGLE_UP);
// 	}
// 	else
// 	{
// 		// ĂĘ±âČ­			
// 		m_paramFactor.pfm_ENGINE_ANGLE_UP = m_paramFactor.pfm_ENGINE_BOOSTERANGLE_UP = 0.0f;
// 	}
// 	
// 	// 2010. 04. 14 by jskim ľĆŔĚĹŰ »çżë˝Ă şÎ˝şĹÍ ±â´É Ăß°ˇ
// 	// »çżë·ů ľĆŔĚĹŰ ±â´É °ˇÁ®żŔ±â
// 	vector<ItemFontInfo*> ItemInfo = g_pGameMain->m_pInfSkill->GetItemFontInfo();
// 	vector<ItemFontInfo*>::iterator it = ItemInfo.begin();
// 	while(it != ItemInfo.end())
// 	{
// 		// şÎ˝şĹÍ Ăß°ˇ ˝Ă°Ł 
// 		if((*it)->pItemInfo)
// 		{
// 			m_fBURN += (*it)->pItemInfo->GetParameterValue(DES_ENGINE_BOOSTER_TIME_UP);
// 			// FLOATÇü ŔçÁ¤·Ä ÇĎż© »çżë
// 			m_fBURN = FloatRangeDecimal(m_fBURN, 2);
// 				
// 		// ĂÖĽŇ ĽÓµµ
// 			m_fShuttleSpeedMin += (*it)->pItemInfo->GetParameterValue(DES_ENGINE_MIN_SPEED_UP);
// 
// 		// ĂÖ´ë ĽÓµµ
// 			m_fShuttleSpeedMax += (*it)->pItemInfo->GetParameterValue(DES_ENGINE_MAX_SPEED_UP);
// 		
// 		// Áö»ó ĽÓµµ
// 			m_fShuttleGroundSpeedMax += (*it)->pItemInfo->GetParameterValue(DES_ENGINE_GROUND_SPEED_UP);
// 	
// 		// şÎ˝şĹÍ ĽÓµµ
// 			m_fShuttleSpeedBoosterOn += (*it)->pItemInfo->GetParameterValue(DES_ENGINE_BOOSTER_SPEED_UP);
// 		
// 		// ÇĎŔĚĆŰ ą«şů(Ľ±Č¸°˘)
// 			m_paramFactor.pfm_ENGINE_ANGLE_UP += (*it)->pItemInfo->GetParameterValue(DES_ENGINE_ANGLE_UP);
// 			m_paramFactor.pfm_ENGINE_BOOSTERANGLE_UP += (*it)->pItemInfo->GetParameterValue(DES_ENGINE_BOOSTERANGLE_UP);
// 		}
// 		it++;
// 	}
// 	//end 2010. 04. 14 by jskim ľĆŔĚĹŰ »çżë˝Ă şÎ˝şĹÍ ±â´É Ăß°ˇ
// 	
// 	for(int i = POS_PROW; i <= POS_PET; i++)
// 	{
// 		CItemInfo* pAllItem = g_pStoreData->FindItemInInventoryByWindowPos( i ); 
// 	
// 		if(!pAllItem || i == POS_REAR)
// 			continue;
// 		
// 		// şÎ˝şĹÍ Ăß°ˇ ˝Ă°Ł 
// 		{
// 			float fTime = GetEnchantDesParam(pAllItem, DES_ENGINE_BOOSTER_TIME_UP);
// 			float fTempValue = pAllItem->GetRealItemInfo()->GetParameterValue(DES_ENGINE_BOOSTER_TIME_UP);
// 			m_fBURN += fTime;
// 			m_fBURN += fTempValue;
// 			// FLOATÇü ŔçÁ¤·Ä ÇĎż© »çżë
// 			m_fBURN = FloatRangeDecimal(m_fBURN, 2);
// 		}
// 		
// 		// ĂÖĽŇ ĽÓµµ
// 		{
// 			float fTime = GetEnchantDesParam(pAllItem, DES_ENGINE_MIN_SPEED_UP);
// 			float fTempValue = pAllItem->GetRealItemInfo()->GetParameterValue(DES_ENGINE_MIN_SPEED_UP);
// 			m_fShuttleSpeedMin += fTime;
// 			m_fShuttleSpeedMin += fTempValue;		
// 		}
// 
// 		// ĂÖ´ë ĽÓµµ
// 		{
// 			float fTime = GetEnchantDesParam(pAllItem, DES_ENGINE_MAX_SPEED_UP);
// 			float fTempValue = pAllItem->GetRealItemInfo()->GetParameterValue(DES_ENGINE_MAX_SPEED_UP);
// 			m_fShuttleSpeedMax += fTime;
// 			m_fShuttleSpeedMax += fTempValue;
// 		}
// 		
// 		// Áö»ó ĽÓµµ
// 		{
// 			float fTime = GetEnchantDesParam(pAllItem, DES_ENGINE_GROUND_SPEED_UP);
// 			float fTempValue = pAllItem->GetRealItemInfo()->GetParameterValue(DES_ENGINE_GROUND_SPEED_UP);
// 			m_fShuttleGroundSpeedMax += fTime;
// 			m_fShuttleGroundSpeedMax += fTempValue;
// 		}
// 		
// 		// şÎ˝şĹÍ ĽÓµµ
// 		{
// 			float fTime = GetEnchantDesParam(pAllItem, DES_ENGINE_BOOSTER_SPEED_UP);
// 			float fTempValue = pAllItem->GetRealItemInfo()->GetParameterValue(DES_ENGINE_BOOSTER_SPEED_UP);
// 			m_fShuttleSpeedBoosterOn += fTime;
// 			m_fShuttleSpeedBoosterOn += fTempValue;		
// 		}	
// 		
// 		// ÇĎŔĚĆŰ ą«şů(Ľ±Č¸°˘)
// 		{
// 			if(pAllItem->GetEnchantParamFactor())
// 			{
// 				float fTime = GetParamFactor_DesParam( *pAllItem->GetEnchantParamFactor(), DES_ENGINE_ANGLE_UP);
// 				float fTempValue = pAllItem->GetRealItemInfo()->GetParameterValue(DES_ENGINE_ANGLE_UP);
// 				
// 				m_paramFactor.pfm_ENGINE_ANGLE_UP += fTime;
// 				m_paramFactor.pfm_ENGINE_ANGLE_UP += fTempValue;		
// 	
// 				fTime = GetParamFactor_DesParam( *pAllItem->GetEnchantParamFactor(), DES_ENGINE_BOOSTERANGLE_UP);
// 				fTempValue = pAllItem->GetRealItemInfo()->GetParameterValue(DES_ENGINE_BOOSTERANGLE_UP);
// 				
// 				m_paramFactor.pfm_ENGINE_BOOSTERANGLE_UP += fTime;
// 				m_paramFactor.pfm_ENGINE_BOOSTERANGLE_UP += fTempValue;
// 			}			
// 		}
// 	}
// 	m_fMouseRate = CAtumSJ::GetEngineRangeAngle(pITEM, &m_paramFactor);
// 	
// 	// żŁÁř UI°»˝Ĺ	
// 	if(g_pGameMain && g_pGameMain->m_pUnitInfoBar)
// 	{
// 		g_pGameMain->m_pUnitInfoBar->SetBURN(m_fCurrentBURN, m_fBURN);
// 		g_pGameMain->m_pUnitInfoBar->SetSpeed(m_fShuttleSpeed, m_fShuttleSpeedMin, m_fShuttleSpeedBoosterOn);
// 	}
	
	// ŔĺÂřµČ ľĆŔĚĹŰżˇĽ­ şÎ˝şĹÍ ˝Ă°ˇ °ˇÁ®żŔ±â
	CItemInfo* pEngineItem = g_pStoreData->FindItemInInventoryByWindowPos( POS_REAR );
	
	if(!pEngineItem)
	{
		m_fBURN = 1;
		return;
	}
	m_fBURN	= pEngineItem->GetRealItemInfo()->Time/1000.0f + GetEnchantDesParam(pEngineItem, DES_ENGINE_BOOSTER_TIME_UP);
	m_fBURN = FloatRangeDecimal(m_fBURN, 2);

	for(int i = POS_PROW; i <= POS_PET; i++)
	{
		CItemInfo* pAllItem = g_pStoreData->FindItemInInventoryByWindowPos( i ); 
		
		if(!pAllItem || i == POS_REAR )
			continue;

		// 2010. 04. 21 by jskim ˝Ĺ±Ô ·°Ĺ° ¸Ó˝Ĺ ±¸Çö
		//// şÎ˝şĹÍ Ăß°ˇ ˝Ă°Ł 
		//m_fBURN += GetEnchantDesParam(pAllItem, DES_ENGINE_BOOSTER_TIME_UP) + pAllItem->GetRealItemInfo()->GetParameterValue(DES_ENGINE_BOOSTER_TIME_UP);
		float tempBURN = 0.0f;
		if(pAllItem && i == POS_CENTER)
		{
			CItemInfo* pAllItem = g_pStoreData->FindItemInInventoryByWindowPos( i );
			
			for(int j=0; j <SIZE_DES_PARAM_PER_RARE_ITEM_INFO; j++)
			{
				if(pAllItem->GetPrefixRareInfo())
				{
					if(pAllItem->GetPrefixRareInfo()->DesParameter[j] == DES_ENGINE_BOOSTER_TIME_UP)
					{
						tempBURN += pAllItem->GetPrefixRareInfo()->ParameterValue[j];
					}
				}
				if(pAllItem->GetSuffixRareInfo())
				{
					if(pAllItem->GetSuffixRareInfo()->DesParameter[j] == DES_ENGINE_BOOSTER_TIME_UP)
					{
						tempBURN += pAllItem->GetSuffixRareInfo()->ParameterValue[j];
					}
				}
			}
		}	
		// şÎ˝şĹÍ Ăß°ˇ ˝Ă°Ł 
		m_fBURN += GetEnchantDesParam(pAllItem, DES_ENGINE_BOOSTER_TIME_UP) + pAllItem->GetRealItemInfo()->GetParameterValue(DES_ENGINE_BOOSTER_TIME_UP) + tempBURN;
		//end 2010. 04. 21 by jskim ˝Ĺ±Ô ·°Ĺ° ¸Ó˝Ĺ ±¸Çö
		m_fBURN = FloatRangeDecimal(m_fBURN, 2);
	}

	// »çżë·ů ľĆŔĚĹŰ ±â´É °ˇÁ®żŔ±â
	vector<ItemFontInfo*> ItemInfo = g_pGameMain->m_pInfSkill->GetItemFontInfo();
	vector<ItemFontInfo*>::iterator it = ItemInfo.begin();
	while(it != ItemInfo.end())
	{
		// şÎ˝şĹÍ Ăß°ˇ ˝Ă°Ł 
		if((*it)->pItemInfo)
	 	{
	 		m_fBURN += (*it)->pItemInfo->GetParameterValue(DES_ENGINE_BOOSTER_TIME_UP);
	 		// FLOATÇü ŔçÁ¤·Ä ÇĎż© »çżë
	 		m_fBURN = FloatRangeDecimal(m_fBURN, 2);
 		}
		it++;
 	}
}
//end 2010. 03. 25 by jskim Speed, Angle Ăß°ˇ °č»ę
//end 2010. 04. 15 by jskim şÎ˝şĹÍ ĂÖĽŇ ĽÓµµ, ĂÖ°í ĽÓµµ 2ąč µÇ´Â ąö±× ĽöÁ¤

// 2010. 03. 25 by ckPark »ç¸Á˝Ă 5ĂĘŔĚ»ó Ăß¶ôÇĎ¸é şÎČ°¸ŢĽĽÁö Ăß°ˇ
void	CShuttleChild::SetDeadMessageEnable( BOOL bDeadMessageEnable )
{
	m_bDeadMessageEnable = bDeadMessageEnable;
}
// end 2010. 03. 25 by ckPark »ç¸Á˝Ă 5ĂĘŔĚ»ó Ăß¶ôÇĎ¸é şÎČ°¸ŢĽĽÁö Ăß°ˇ

// 2010-06-08 by dgwoo, Ćę˝Ă˝şĹŰ Ăß°ˇ. 
void CShuttleChild::SetPartner(char* i_szPartner,BOOL i_bCharacter)
{
	int nPartner = atoi(i_szPartner);
	DeleteWearItem(WEAR_ITEM_KIND_PET);
	CreateWearItem( WEAR_ITEM_KIND_PET, nPartner, i_bCharacter);
}
// 2010-06-08 by dgwoo, Ćę˝Ă˝şĹŰ Ăß°ˇ. 

// 2010-06-15 by shcho&hslee Ćę ˝Ă˝şĹŰ - Ćę ĽŇŔŻ Á¤ş¸ ąŢ±â.
BOOL CShuttleChild:: PutPetInfo(MSG_FC_ITEM_PET_BASEDATA* pMsg)
{

	if ( NULL == pMsg )
		return FALSE;

	tPET_CURRENTINFO *pNewPetCurInfo = new tPET_CURRENTINFO;

	if ( NULL == pNewPetCurInfo )
		return FALSE;
#ifdef _INET_PET
	//2011-10-06 by jhahn ĆÄĆ®łĘ ĽşŔĺÇü ˝Ă˝şĹŰ	
	pNewPetCurInfo->CreatedPetUID = pMsg->CreatedPetUID;
	//	pNewPetCurInfo->EnableEditPetName = pMsg->EnableEditPetName;
	//	pNewPetCurInfo->EnableLevelUp = pMsg->EnableLevelUp;
	//end 2011-10-06 by jhahn ĆÄĆ®łĘ ĽşŔĺÇü ˝Ă˝şĹŰ
	pNewPetCurInfo->PetExp = pMsg->PetExp;
	pNewPetCurInfo->PetIndex = pMsg->PetIndex;
	pNewPetCurInfo->PetLevel = pMsg->PetLevel;

	//2011-10-06 by jhahn ĆÄĆ®łĘ ĽşŔĺÇü ˝Ă˝şĹŰ
	pNewPetCurInfo->PetExpRatio = pMsg->PetExpRatio;
	pNewPetCurInfo->PetEnableSocketCount = pMsg->PetEnableSocketCount;

	pNewPetCurInfo->PetKitHP.ItemNum = pMsg->PetKitHP.ItemNum;
	pNewPetCurInfo->PetKitHP.TriggerValue = pMsg->PetKitHP.TriggerValue;

	pNewPetCurInfo->PetKitShield.ItemNum = pMsg->PetKitShield.ItemNum;
	pNewPetCurInfo->PetKitShield.TriggerValue = pMsg->PetKitShield.TriggerValue;

	pNewPetCurInfo->PetKitSP.ItemNum = pMsg->PetKitSP.ItemNum;
	pNewPetCurInfo->PetKitSP.TriggerValue = pMsg->PetKitSP.TriggerValue;




	for (int i = 0; i < SIZE_MAX_PETSOCKET; i++)
	{
		pNewPetCurInfo->PetSocketItemUID[i] = pMsg->PetSocketItemUID[i];
		pNewPetCurInfo->PetAutoSkill.ItemNum[i] = pMsg->PetAutoSkill.ItemNum[i];
	}

	strcpy(pNewPetCurInfo->PetName, pMsg->PetName);
	//end 2011-10-06 by jhahn ĆÄĆ®łĘ ĽşŔĺÇü ˝Ă˝şĹŰ
#else
	pNewPetCurInfo->CreatedPetUID = pMsg->CreatedPetUID;
	pNewPetCurInfo->EnableEditPetName = pMsg->EnableEditPetName;
	pNewPetCurInfo->EnableLevelUp = pMsg->EnableLevelUp;
	pNewPetCurInfo->PetExp = pMsg->PetExp;
	pNewPetCurInfo->PetIndex = pMsg->PetIndex;
	pNewPetCurInfo->PetLevel = pMsg->PetLevel;
	pNewPetCurInfo->SourceIndex_City = pMsg->SourceIndex_City;
	pNewPetCurInfo->SourceIndex_Field = pMsg->SourceIndex_Field;
	pNewPetCurInfo->Stamina = pMsg->Stamina;

	strcpy ( pNewPetCurInfo->szPetName , pMsg->szPetName );

	for ( int i = 0; i < SIZE_MAX_PETSKILLITEM; ++i )
	{
		if ( pMsg->ItemIndex_PetSkill[i] <= 0 )
			continue;

		const ITEM *pItem = g_pDatabase->GetServerItemInfo( pMsg->ItemIndex_PetSkill[i] );

		if ( NULL == pItem )
			continue;

		pNewPetCurInfo->pItem_PetSkill[i] = pItem;
	}
#endif
	GetPetManager()->InsertPetCurrentData( pNewPetCurInfo );

	return TRUE;

}
// End 2010-06-15 by shcho&hslee Ćę ˝Ă˝şĹŰ - Ćę ĽŇŔŻ Á¤ş¸ ąŢ±â.

// 2011-05-31 by jhahn	ŔÎÇÇ3Â÷˝Ăł×¸¶ şŻ°ć
void CShuttleChild::Infinity3Cinema(short bSuccess)
{


	
	if ( TRUE == g_pInterface->m_pInfinity->GetEnableCinemaSkip() )
		return;
	
	// 2011-06-30 by jhahn  ŔÎÇÇ3Â÷ ˝Ăł×¸¶ ˝şĹµ±ÝÁö
	m_InfiState		= FALSE;
	//end 2011-06-30 by jhahn  ŔÎÇÇ3Â÷ ˝Ăł×¸¶ ˝şĹµ±ÝÁö
	

	
	CObjectChild * pCurrentObjEvent;

	if (bSuccess == 1)
	{
		// 2011-06-30 by jhahn  ŔÎÇÇ3Â÷ ˝Ăł×¸¶ ŔÓ˝ĂŔ§Äˇ
		g_pShuttleChild->m_vPos.x = INFINITY_THIRD_X_POS;
		g_pShuttleChild->m_vPos.z = INFINITY_THIRD_Z_POS;
		g_pShuttleChild->m_vPos.y = INFINITY_THIRD_Y_POS;
		//end 2011-06-30 by jhahn  ŔÎÇÇ3Â÷ ˝Ăł×¸¶ ŔÓ˝ĂŔ§Äˇ
		pCurrentObjEvent = g_pScene->FindEventObjectByTypeAndPositionIndex(EVENT_TYPE_INFI_CINEMA_THIRD, EVENT_TYPE_PARAM_INFITHIRD_CINEMA_SUCCESS);
	}	
	else if (bSuccess == 2)
	{
		// 2011-06-30 by jhahn  ŔÎÇÇ3Â÷ ˝Ăł×¸¶ ŔÓ˝ĂŔ§Äˇ
		g_pShuttleChild->m_vPos.x = INFINITY_THIRD_X_POS;
		g_pShuttleChild->m_vPos.z = INFINITY_THIRD_Z_POS;
		g_pShuttleChild->m_vPos.y = INFINITY_THIRD_Y_POS;
		//end 2011-06-30 by jhahn  ŔÎÇÇ3Â÷ ˝Ăł×¸¶ ŔÓ˝ĂŔ§Äˇ
		pCurrentObjEvent = g_pScene->FindEventObjectByTypeAndPositionIndex(EVENT_TYPE_INFI_CINEMA_THIRD, EVENT_TYPE_PARAM_INFITHIRD_CINEMA_FAIL);
	}	
	if(pCurrentObjEvent)
	{
		D3DXVECTOR3 vVel,vUp, vPos;
		vVel = m_vPos - pCurrentObjEvent->m_vPos;
		D3DXVec3Normalize(&vPos, &(pCurrentObjEvent->m_vPos - g_pShuttleChild->m_vPos));
		vUp.x = m_mMatrix._21;
		vUp.y = m_mMatrix._22;
		vUp.z = m_mMatrix._23;
		if(vVel == vUp)
		{// ¸ĹĆ®¸Ż˝ş°ˇ ±úÁöÁö ľĘµµ·Ď
			m_mMatrix._21 += 0.001f;
			m_mMatrix._22 -= 0.001f;
			m_mMatrix._23 += 0.002f;
		}
		m_mMatrix._31 = vVel.x;
		m_mMatrix._32 = vVel.y;
		m_mMatrix._33 = vVel.z;
		int succestype;
		if(bSuccess == 1)
		{
			succestype = PATTERN_CAMERA_SUCCESS_INFINITYTHIRD;
		}
		else if (bSuccess  == 2)
		{
			succestype = PATTERN_CAMERA_FAIL_INFINITYTHIRD;
		}
		
		if(InitCinemaUnit(succestype, FALSE, pCurrentObjEvent->m_vPos, vPos) == TRUE)
		{
		// 2011-06-30 by jhahn ŔÎÇÇ3Â÷    ˝ĂÁˇ 1ŔÎÄŞ şŻ°ć
		//	g_pD3dApp->m_pCamera->SetCamType( CAMERA_TYPE_FPS);
		//end  2011-06-30 by jhahn ŔÎÇÇ3Â÷    ˝ĂÁˇ 1ŔÎÄŞ şŻ°ć
			m_bSkillMoveIsUse = TRUE;
			g_pD3dApp->StartFadeEffect(TRUE,3,D3DCOLOR_ARGB(0,0,0,0));
		if(g_pInterface->m_bShowInterface)
		{						
				// 2011-06-30 by jhahn ŔÎÇÇ3Â÷ ˝Ăł×¸¶ ¶§ °ř°Ý±ÝÁö
				m_bLButtonState = m_bRButtonState = FALSE;
				//end 2011-06-30 by jhahn ŔÎÇÇ3Â÷ ˝Ăł×¸¶ ¶§ °ř°Ý±ÝÁö
			
				SetShowCinema ( TRUE );
			g_pInterface->SetScreenShotMode(TRUE);
				m_bKeyBlock = TRUE;
			}
		}
		else
		{
			DBGOUT("[succestype] : Pattern File Non Exsistent.\n");
			
			g_pInterface->m_pInfinity->SendPacket_SkipEndingCinema();
		}
	}	
}
// End 2011-05-31 by jhahn	ŔÎÇÇ3Â÷˝Ăł×¸¶ şŻ°ć
#ifdef _INET_PET
void CShuttleChild::PetUseItme()
{

	CItemInfo* pAllItem = g_pStoreData->FindItemInInventoryByWindowPos(POS_PET);

	if (pAllItem == NULL)
	{
		return;
	}

	tPET_CURRENTINFO * tempCurrentInfo = g_pShuttleChild->GetPetManager()->GetPtr_PetCurrentData(pAllItem->UniqueNumber);
	if (tempCurrentInfo == NULL)
	{
		return;
	}

	tPET_LEVEL_DATA* tempLevelData = g_pDatabase->GetPtr_PetLevelData(tempCurrentInfo->PetIndex, tempCurrentInfo->PetLevel);
	if (tempLevelData == NULL)
	{
		return;
	}

	// 2013-03-25 by bhsohn ĆÄĆ®łĘ Ĺ°Ć® ŔÚµż »çżë ĽŇÄĎ °ü·Ă ąö±× ĽöÁ¤
	DWORD dwGameCurrentTime = g_pD3dApp->GetGameCurrentTime();
	BOOL bUsePacketSend = FALSE;
	if (max(1, dwGameCurrentTime - m_dwPetUseItemTime_Spell) > MAX_PETUSETITEM_CAP)  // 2013-03-25 by bhsohn ĆÄĆ®łĘ Ĺ°Ć® ŔÚµż »çżë ĽŇÄĎ °ü·Ă ąö±× ĽöÁ¤
	{
		//2011-10-06 by jhahn ĆÄĆ®łĘ ĽşŔĺÇü ˝Ă˝şĹŰ
		for (int i = 0; i < SIZE_MAX_PETSOCKET_AUTOSKILL_SLOT; i++)
		{
			if (tempCurrentInfo->PetAutoSkill.ItemNum[i] && g_pShuttleChild->GetPetManager()->GetSocketOnOff())
			{
				CSkillInfo* tempskill = g_pShuttleChild->m_pSkill->FindItemSkill(tempCurrentInfo->PetAutoSkill.ItemNum[i]);

				if (tempskill && (g_pShuttleChild->m_myShuttleInfo.CurrentSP >= tempskill->ItemInfo->ReqSP) && (tempskill->GetCheckReattackTime() <= 0.0f))
				{
					g_pShuttleChild->m_pSkill->UseSkill(tempskill);
					// 2013-03-25 by bhsohn ĆÄĆ®łĘ Ĺ°Ć® ŔÚµż »çżë ĽŇÄĎ °ü·Ă ąö±× ĽöÁ¤
					bUsePacketSend = TRUE;
					// END 2013-03-25 by bhsohn ĆÄĆ®łĘ Ĺ°Ć® ŔÚµż »çżë ĽŇÄĎ °ü·Ă ąö±× ĽöÁ¤	
				}
			}
		}
	}

	// 2013-03-25 by bhsohn ĆÄĆ®łĘ Ĺ°Ć® ŔÚµż »çżë ĽŇÄĎ °ü·Ă ąö±× ĽöÁ¤
	if (bUsePacketSend)
	{
		m_dwPetUseItemTime_Spell = g_pD3dApp->GetGameCurrentTime();
	}
	// END 2013-03-25 by bhsohn ĆÄĆ®łĘ Ĺ°Ć® ŔÚµż »çżë ĽŇÄĎ °ü·Ă ąö±× ĽöÁ¤

	// 2013-03-25 by bhsohn ĆÄĆ®łĘ Ĺ°Ć® ŔÚµż »çżë ĽŇÄĎ °ü·Ă ąö±× ĽöÁ¤
	CItemInfo* pItemInfo = NULL;
	if (max(1, dwGameCurrentTime - m_dwPetUseItemTime_Hp) > MAX_PETUSETITEM_CAP)
	{
		CItemInfo* pItemInfo = g_pStoreData->FindItemInInventoryByItemNum(tempCurrentInfo->PetKitHP.ItemNum);
		if (pItemInfo != NULL)
		{
			float temp = g_pShuttleChild->m_myShuttleInfo.HP;
			float temp2 = temp / 100;
			HpCharge = FALSE;			// 2014-08-06 by ymjoo 파트너 키트 자동 사용 소켓 설정 수치 까지만 회복되도록 수정
			if (g_pShuttleChild->m_myShuttleInfo.CurrentHP < (temp2 * tempCurrentInfo->PetKitHP.TriggerValue))
			{
				HpCharge = TRUE;
			}

			if (HpCharge)
			{
				if (g_pShuttleChild->m_myShuttleInfo.CurrentHP >= g_pShuttleChild->m_myShuttleInfo.HP)
				{
					HpCharge = FALSE;
				}
				else
				{
					if (tempLevelData->KitLevelHP >= pItemInfo->ItemInfo->ArrParameterValue[1] && g_pShuttleChild->GetPetManager()->GetSocketOnOff())
					{
						g_pGameMain->m_pInven->SendUseItem((ITEM_BASE*)pItemInfo);

						m_dwPetUseItemTime_Hp = dwGameCurrentTime; // 2013-03-25 by bhsohn ĆÄĆ®łĘ Ĺ°Ć® ŔÚµż »çżë ĽŇÄĎ °ü·Ă ąö±× ĽöÁ¤
					}
					else
						pItemInfo = NULL;


				}

			}
		}
	}
	// 2013-03-25 by bhsohn ĆÄĆ®łĘ Ĺ°Ć® ŔÚµż »çżë ĽŇÄĎ °ü·Ă ąö±× ĽöÁ¤
	if (max(1, dwGameCurrentTime - m_dwPetUseItemTime_Dp) > MAX_PETUSETITEM_CAP)
	{
		pItemInfo = g_pStoreData->FindItemInInventoryByItemNum(tempCurrentInfo->PetKitShield.ItemNum);
		if (pItemInfo != NULL)
		{
			float temp = g_pShuttleChild->m_myShuttleInfo.DP;
			float temp2 = temp / 100;
			ShieldCharge = FALSE;			// 2014-08-06 by ymjoo 파트너 키트 자동 사용 소켓 설정 수치 까지만 회복되도록 수정
			if (g_pShuttleChild->m_myShuttleInfo.CurrentDP < (temp2 * tempCurrentInfo->PetKitShield.TriggerValue))
			{
				ShieldCharge = TRUE;
			}

			if (ShieldCharge)
			{
				if (g_pShuttleChild->m_myShuttleInfo.CurrentDP >= g_pShuttleChild->m_myShuttleInfo.DP)
				{
					ShieldCharge = FALSE;
				}
				else
				{
					if (tempLevelData->KitLevelShield >= pItemInfo->ItemInfo->ArrParameterValue[1] && g_pShuttleChild->GetPetManager()->GetSocketOnOff())
					{
						g_pGameMain->m_pInven->SendUseItem((ITEM_BASE*)pItemInfo);

						m_dwPetUseItemTime_Dp = dwGameCurrentTime; // 2013-03-25 by bhsohn ĆÄĆ®łĘ Ĺ°Ć® ŔÚµż »çżë ĽŇÄĎ °ü·Ă ąö±× ĽöÁ¤
					}

				}

			}
		}
	}
	// 2013-03-25 by bhsohn ĆÄĆ®łĘ Ĺ°Ć® ŔÚµż »çżë ĽŇÄĎ °ü·Ă ąö±× ĽöÁ¤
	if (max(1, dwGameCurrentTime - m_dwPetUseItemTime_Sp) > MAX_PETUSETITEM_CAP)
	{
		pItemInfo = g_pStoreData->FindItemInInventoryByItemNum(tempCurrentInfo->PetKitSP.ItemNum);
		if (pItemInfo != NULL)
		{
			float temp = g_pShuttleChild->m_myShuttleInfo.SP;
			float temp2 = temp / 100;
			SpCharge = FALSE;
			//			if ( g_pShuttleChild->m_myShuttleInfo.CurrentSP < (temp2 * tempCurrentInfo->PetKitSP.TriggerValue ))
			// m_myShuttleInfo.CurrentSP : ·Ł´ő¸µµÇ°í ŔÖ´Â SP°Ş
			// g_pShuttleChild->m_fNextSP : ˝ÇÁ¦ Ľ­ąö żˇĽ­ ľË·ÁÁÖ´Â SP°Ş (Áßżä. ŔĚ °ŞŔ¸·Î ÇöŔç ł» ±âĂĽŔÇ SP°ŞŔ» ĂĽĹ©ÇŘľßÇŃ´Ů.!!)
			if (g_pShuttleChild->m_fNextSP < (temp2 * tempCurrentInfo->PetKitSP.TriggerValue))
			{
				//				DBGOUT(" SpCharge = TRUE; #1 \n");
				SpCharge = TRUE;
			}

			if (SpCharge)
			{
				if (g_pShuttleChild->m_fNextSP >= g_pShuttleChild->m_myShuttleInfo.SP)
				{
					SpCharge = FALSE;
				}
				else
				{
					if (tempLevelData->KitLevelSP >= pItemInfo->ItemInfo->ArrParameterValue[1] && g_pShuttleChild->GetPetManager()->GetSocketOnOff())
					{
						g_pGameMain->m_pInven->SendUseItem((ITEM_BASE*)pItemInfo);

						m_dwPetUseItemTime_Sp = dwGameCurrentTime; // 2013-03-25 by bhsohn ĆÄĆ®łĘ Ĺ°Ć® ŔÚµż »çżë ĽŇÄĎ °ü·Ă ąö±× ĽöÁ¤
					}

				}
			}
		}
	}
	//end 2011-10-06 by jhahn ĆÄĆ®łĘ ĽşŔĺÇü ˝Ă˝şĹŰ

}
#endif

void CShuttleChild::ActiveKeyDownADouble()
{
	if((g_pTutorial->IsTutorialMode() == FALSE || 
		(g_pTutorial->IsTutorialMode() == TRUE && g_pTutorial->GetLesson() >= L3)) &&
		m_bIsAir == TRUE &&
		m_dwState == _NORMAL)
	{
		if(m_fADoubleKeyTime < DOUBLEKEY_TIME 
			&& m_bKeyDownACheck == FALSE 
			&& m_nBooster != BOOSTER_TYPE_MAX
			&& !m_bRollUsed)
		{
			DbgOut("CShuttleChild::WndProc() : m_bKeyDownADouble = TRUE\n");
			m_bKeyDownADouble = TRUE;
			m_bKeyDownDDouble = FALSE;
		}
		else
		{
			m_fADoubleKeyTime = 0.0f;
			m_fDDoubleKeyTime = 0.5f;
		}
	}
}
void CShuttleChild::ActiveKeyDownDDouble()
{
	if((g_pTutorial->IsTutorialMode() == FALSE || 
		(g_pTutorial->IsTutorialMode() == TRUE && g_pTutorial->GetLesson() >= L3)) &&
		m_bIsAir == TRUE &&
		m_dwState == _NORMAL)
	{
		if(m_fDDoubleKeyTime < DOUBLEKEY_TIME 
			&& m_bKeyDownDCheck == FALSE
			&& m_nBooster != BOOSTER_TYPE_MAX
			&& !m_bRollUsed)
		{
			DbgOut("CShuttleChild::WndProc() : m_bKeyDownDDouble = TRUE\n");
			m_bKeyDownDDouble = TRUE;
			m_bKeyDownADouble = FALSE;
		}
		else
		{
			m_fDDoubleKeyTime = 0.0f;
			m_fADoubleKeyTime = 0.5f;
		}
	}
}

// 2013-02-14 by mspark, M±âľî ą«Ŕű ˝şĹł żöÇÁČÄżˇ »ó´ëąćżˇ°Ô ş¸ŔĚÁö ľĘ´Â ą®Á¦ ĽöÁ¤
void CShuttleChild::UpdateSkillEffectInfo(int nSkillNum)
// end 2013-02-14 by mspark, M±âľî ą«Ŕű ˝şĹł żöÇÁČÄżˇ »ó´ëąćżˇ°Ô ş¸ŔĚÁö ľĘ´Â ą®Á¦ ĽöÁ¤
{
	vector<CSkillInfo*>::iterator itvecSkillInfo = m_pSkill->m_vecUsingSkill.begin();
	while (itvecSkillInfo != m_pSkill->m_vecUsingSkill.end())
	{
		CSkillInfo* pUsingSkillInfo = (*itvecSkillInfo);
		if (!pUsingSkillInfo)
		{
			itvecSkillInfo++;
			continue;
		}
		else if (!pUsingSkillInfo->ItemInfo)
		{
			itvecSkillInfo++;
			continue;
		}
		else if (SKILLTYPE_TIMELIMIT != pUsingSkillInfo->ItemInfo->SkillType)
		{
			// Ĺ¸ŔÓ ąöÇÁ°ˇ ľĆ´Ď´Ů
			itvecSkillInfo++;
			continue;
		}
		else if (SKILL_STATE_USING != pUsingSkillInfo->GetSkillState())
		{
			// »çżëÁßŔĚ ľĆ´Ď´Ů.
			itvecSkillInfo++;
			continue;
		}

		INT   ItemNum = pUsingSkillInfo->ItemNum;		// ľĆŔĚĹŰ ąřČŁ
		//if(SKILL_BASE_NUM(ItemNum) != AGEAR_SKILL_BASENUM_BARIAL)		
		// 2013-02-14 by mspark, M±âľî ą«Ŕű ˝şĹł żöÇÁČÄżˇ »ó´ëąćżˇ°Ô ş¸ŔĚÁö ľĘ´Â ą®Á¦ ĽöÁ¤
		if (SKILL_BASE_NUM(ItemNum) != nSkillNum)
			// end 2013-02-14 by mspark, M±âľî ą«Ŕű ˝şĹł żöÇÁČÄżˇ »ó´ëąćżˇ°Ô ş¸ŔĚÁö ľĘ´Â ą®Á¦ ĽöÁ¤
		{
			itvecSkillInfo++;
			continue;
		}
		if (m_pSkillEffect->FindSkillEffect(ItemNum))
		{
			DBGOUT("FindSkillEffect ItemNum[%d] \n", ItemNum);
		}
		else
		{
			DBGOUT("~FindSkillEffect ItemNum[%d] \n", ItemNum);
			ITEM* pItem = g_pDatabase->GetServerItemInfo(ItemNum);
			if (pItem)
			{
				// 2013-03-13 by bhsohn ˝şĹł ąřČŁ ±¸Á¶ ĽöÁ¤
//				CAppEffectData* pEff = new CAppEffectData(g_pShuttleChild,SKILL_BASE_NUM(pItem->SourceIndex),D3DXVECTOR3(0,0,0));
				CAppEffectData* pEff = new CAppEffectData(g_pShuttleChild, SKILL_BASE_NUM(pItem->SourceIndex), D3DXVECTOR3(0, 0, 0));

				if (pEff->m_bUsing == TRUE)
				{
					pEff = (CAppEffectData*)g_pD3dApp->m_pEffectList->AddChild(pEff);
				}
				else
				{
					delete pEff;
				}
			}
		}


		itvecSkillInfo++;
	}

}
// 2013-06-25 by ssjung µĄąĚÁö ŔĚĆĺĆ®°ˇ ľČłŞżŔ´Â Çö»ó ĽöÁ¤
BodyCond_t CShuttleChild::ShuttleChangeBodyCondition()
{
	BodyCond_t hyBody = BODYCON_SET_OR_CLEAR_MASK;

	if (m_pCharacterInfo)
	{
		if (COMPARE_BODYCON_BIT(m_pCharacterInfo->m_nCurrentBodyCondition, BODYCON_DAMAGE1_MASK))
		{
			hyBody |= BODYCON_DAMAGE1_MASK;
		}
		if (COMPARE_BODYCON_BIT(m_pCharacterInfo->m_nCurrentBodyCondition, BODYCON_DAMAGE2_MASK))
		{
			hyBody |= BODYCON_DAMAGE2_MASK;
		}
		if (COMPARE_BODYCON_BIT(m_pCharacterInfo->m_nCurrentBodyCondition, BODYCON_DAMAGE3_MASK))
		{
			hyBody |= BODYCON_DAMAGE3_MASK;
		}
	}
	return hyBody;
}
// end 2013-06-25 by ssjung µĄąĚÁö ŔĚĆĺĆ®°ˇ ľČłŞżŔ´Â Çö»ó ĽöÁ¤

// 2013-10-18 by ssjung A±âľî°ˇ ˝ĂÁî¸đµĺ ŔĚ°ĹłŞ żňÁ÷ŔĎ¶§ ŔÎĹÍĆäŔĚ˝ş ¶çżěÁö ľĘµµ·Ď ĽöÁ¤
BOOL CShuttleChild::GetMoving()
{
	float fTempVal = 0.0f;

	fTempVal = abs(m_fShuttleSpeed);
	if (fTempVal >= 0.0f && fTempVal <= 30.0f)
		return FALSE;

	return TRUE;
}
// end 2013-10-18 by ssjung A±âľî°ˇ ˝ĂÁî¸đµĺ ŔĚ°ĹłŞ żňÁ÷ŔĎ¶§ ŔÎĹÍĆäŔĚ˝ş ¶çżěÁö ľĘµµ·Ď ĽöÁ¤

