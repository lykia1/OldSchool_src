// Interface.cpp: implementation of the CInterface class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RangeTime.h"
#include "INFUnitCreateInfo.h"
#include "Interface.h"
#include "AtumApplication.h"
#include "INFTarget.h"
#include "INFGameMain.h"
#include "INFPilotFace.h"
#include "INFSelect.h"
#include "INFMapLoad.h"
#include "INFCityBase.h"
#include "INFChannelList.h"
#include "ShuttleChild.h"
#include "CharacterChild.h"				// 2005-07-21 by ispark
#include "Camera.h"
#include "SceneData.h"
#include "INFGameMainUnitInfoBar.h"
#include "INFGameMainWisperChat.h"
#include "INFGameMainChat.h"
#include "INFSkill.h"
#include "INFWindow.h"
#include "INFGameMainMinimap.h"
#include "INFFadeEffect.h"
#include "INFStageEffectData.h"
#include "INFGameCountDown.h"
#include "dxutil.h"
#include "INFGameMainFAQ.h"
#include "INFUnitNameInfo.h"
#include "INFAttackDirection.h"

// 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
#include "JoyStickInput.h"	

// 2008-03-19 by bhsohn Ep3żÉĽÇĂ˘
//#include "INFSystem.h"
#include "INFOptionSystem.h"

#include "INFMp3Player.h"
#include "INFItemInfo.h"
#include "TutorialSystem.h"
#include "INFMapName.h"
#include "PkNormalTimer.h"
#include "INFDamageView.h"
#include "INFMissionInfo.h"
#include "GlobalImage.h"
#include "INFUnitState.h"								// 2005-07-18 by ispark
#include "INFCommunityVOIP.h"
#include "INFMenuList.h"
#include "StoreData.h"
#include "INFCityShop.h"
#include "ItemInfo.h"
#include "INFCityBazaar.h"								// 2006-07-25 by ispark
#include "INFInven.h"									// 2006-07-27 by ispark
#include "Skill.h"										// 2006-07-30 by ispark
#include "SkillInfo.h"									// 2006-07-30 by ispark
// 2008-09-22 by bhsohn EP3 Äł¸ŻĹÍ Ă˘
//#include "INFCharacterInfo.h"
#include "INFCharacterInfoExtend.h"
#include "EnemyData.h"									// 2006-08-02 by ispark
#include "INFTrade.h"
#include "Chat.h"
#include "QuestData.h"									// 2006-09-07 by ispark
#include "INFGameArena.h"

// 2009. 11. 02 by ckPark ŔÎÇÇ´ĎĆĽ ÇĘµĺ ŔÎ˝şĹĎ˝ş ´řÁŻ ˝Ă˝şĹŰ
#include "INFInfinity.h"
// end 2009. 11. 02 by ckPark ŔÎÇÇ´ĎĆĽ ÇĘµĺ ŔÎ˝şĹĎ˝ş ´řÁŻ ˝Ă˝şĹŰ

// 2010. 03. 03 by ckPark ŔÎÇÇ´ĎĆĽ ÇĘµĺ 2Â÷(ŔÔŔĺ UIşŻ°ć)
#include "INFCityInfinityField.h"
// end 2010. 03. 03 by ckPark ŔÎÇÇ´ĎĆĽ ÇĘµĺ 2Â÷(ŔÔŔĺ UIşŻ°ć)


// 2007-07-04 by bhsohn żŔĆŰ·ąŔĚĹÍ Ăß°ˇ
#include "INFOpMain.h"
// 2007-07-23 by bhsohn żŔĆŰ·ąŔĚĹÍ Ăß°ˇŔŰľ÷
#include "SetupConfig.h"
#include "INFToolTip.h"
#include "INFGameMainOutPost.h"
#include "INFMotherShipManager.h"

// 2008-04-22 by bhsohn ¸đĽ±/°ĹÁˇŔü ÁřÇŕÁßŔÎ Á¤ş¸µµ ÇĄ˝Ă ÇĎ°Ô şŻ°ć
#include "INFCityLeader.h"
#include "INFCityOutPost.h"		// 2008-10-16 by bhsohn ŔüÁř±âÁö ¸đĽ±Ŕü, °ĹÁˇŔü Á¤ş¸Ă˘

#include "WorldRankManager.h"	// 2009-02-13 by bhsohn żůµĺ ·©Ĺ· ˝Ă˝şĹŰ

// 2008-10-15 by bhsohn ¸®ĽŇ˝ş ¸Ţ¸đ¸® ş¸ČŁ ±â´É Ăß°ˇ
#include "GameDataLast.h"
//29-07-2021 by Inetpub for fix F hiding when party
#include "INFCommunityParty.h"
#include "INFCommunity.h"
#ifdef S_ENABLE_KILLFEED
#include "INFEvoKillfeed.h"
#endif // S_ENABLE_KILLFEED

#include "AtumDatabase.h"

#define OPTION_DEFAULT_HIGH_TERRAINEFFECTRENDER			9
#define OPTION_DEFAULT_HIGH_TERRAINRENDER				9
#define OPTION_DEFAULT_HIGH_SHOWSHADOW					0
#define OPTION_DEFAULT_HIGH_OBJECTVIEW					9
#define OPTION_DEFAULT_HIGH_UNITDETAIL					9
#define OPTION_DEFAULT_HIGH_CONTRAST					0
#define OPTION_DEFAULT_HIGH_FILTER						0

#define OPTION_DEFAULT_MIDDLE_TERRAINEFFECTRENDER		7
#define OPTION_DEFAULT_MIDDLE_TERRAINRENDER				4
#define OPTION_DEFAULT_MIDDLE_SHOWSHADOW				0
#define OPTION_DEFAULT_MIDDLE_OBJECTVIEW				4
#define OPTION_DEFAULT_MIDDLE_UNITDETAIL				4
#define OPTION_DEFAULT_MIDDLE_CONTRAST					0
#define OPTION_DEFAULT_MIDDLE_FILTER					0

#define OPTION_DEFAULT_LOW_TERRAINEFFECTRENDER			0
#define OPTION_DEFAULT_LOW_TERRAINRENDER				0
#define OPTION_DEFAULT_LOW_SHOWSHADOW					0
#define OPTION_DEFAULT_LOW_OBJECTVIEW					0
#define OPTION_DEFAULT_LOW_UNITDETAIL					0
#define OPTION_DEFAULT_LOW_CONTRAST						0
#define OPTION_DEFAULT_LOW_FILTER						0

#define RENDER_MAPNAME_POS_Y							113

// 2007-07-23 by bhsohn żŔĆŰ·ąŔĚĹÍ Ăß°ˇŔŰľ÷
#define	SETUP_INFO_PATH									"setupinfo.ver"

#define	SETUP_INFO_OPERATOR_INFO						"operator"
// 2007-08-07 by bhsohn ˝şÇÇÄż ľĆŔĚĹŰ Ăß°ˇ
#define	SETUP_INFO_SPEAKER_INFO							"speaker"
// 2008-06-20 by bhsohn EP3 żÉĽÇ°ü·Ă Ăł¸®
#define	SETUP_INFO_OPETC_MANTOMAN_INFO					"mantoman"		// 1:1ŔüĹő
#define	SETUP_INFO_OPETC_PARTYWAR_INFO					"partywar"		// Ćí´ëŔüĹő
#define	SETUP_INFO_OPETC_GUILDWAR_INFO					"guildwar"		// ż©´ÜŔüĹő
#define	SETUP_INFO_OPETC_WHISPER_INFO					"whisper"		// ±ÓĽÓ¸»
#define	SETUP_INFO_OPETC_PARTYINVI_INFO					"inparty"		// ĆÄĆĽĂĘ´ë
#define	SETUP_INFO_OPETC_GUILDINVI_INFO					"inguild"		// ż©´ÜĂĘ´ë
#define	SETUP_INFO_OPETC_EXCHANGE_INFO					"exchange"		// ±łČŻ
#define	SETUP_INFO_OPETC_INTERPOS_INFO					"interpos"		// ŔÎĹÍĆäŔĚ˝ş Ŕ§ÄˇŔúŔĺ
#define	SETUP_INFO_OPETC_INTERPOS_X_INFO				"interx"		// ŔÎĹÍĆäŔĚ˝ş X
#define	SETUP_INFO_OPETC_INTERPOS_Y_INFO				"intery"		// ŔÎĹÍĆäŔĚ˝ş Y
#define	SETUP_INFO_OPETC_INTERPOS_W_INFO				"interw"		// ŔÎĹÍĆäŔĚ˝ş WIdth
#define	SETUP_INFO_OPETC_INTERPOS_H_INFO				"interh"		// ŔÎĹÍĆäŔĚ˝ş Height

// 2009. 02. 03 by ckPark Ă¤ĆĂĂ˘ ŔÎĹÍĆäŔĚ˝ş Ĺ©±â ŔúŔĺ
#define	SETUP_INFO_OPETC_CHATPOS_W_INFO					"chatw"			// Ă¤ĆĂĂ˘ W
#define	SETUP_INFO_OPETC_CHATPOS_H_INFO					"chath"			// Ă¤ĆĂĂ˘ H
// end 2009. 02. 03 by ckPark Ă¤ĆĂĂ˘ ŔÎĹÍĆäŔĚ˝ş Ĺ©±â ŔúŔĺ


#define	SETUP_INFO_OPETC_GAME_W_INFO					"gamew"		// ŔÎĹÍĆäŔĚ˝ş WIdth
#define	SETUP_INFO_OPETC_GAME_H_INFO					"gameh"		// ŔÎĹÍĆäŔĚ˝ş Height
#define	SETUP_INFO_OPETC_INTERHIDE_INFO					"interhide"		// ŔÎĹÍĆäŔĚ˝ş Ľű±â±â
#define	SETUP_INFO_OPETC_SAME_ID_HIDE_INFO				"shide"			// °°Ŕş ĽĽ·Â ľĆŔĚµđ Ľű±â±â
#define	SETUP_INFO_OPETC_ENEMY_ID_HIDE_INFO				"ehide"			// ´Ů¸Ą ĽĽ·Â ľĆŔĚµđ Ľű±â±â
#define	SETUP_INFO_OPETC_MOUSE_LOCK_INFO				"mouselock"		// ¸¶żě˝ş °ˇµÎ±â
#define	SETUP_INFO_OPETC_FILTER_INFO					"filter"		// şńĽÓľî ÇĘĹÍ¸µ
// 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
#define	SETUP_INFO_JOYSTICK_USE							"juse"			// Á¶ŔĚ˝şĆ˝ »çżë
#define	SETUP_INFO_JOYSTICK_SELECT						"jselect"		// Á¶ŔĚ˝şĆ˝ Ľ±ĹĂ
#define	SETUP_INFO_JOYSTICK_FEED						"jfeed"			// Á¶ŔĚ˝şĆ˝ Ářµż»çżë
#define	SETUP_INFO_JOYSTICK_SENSE						"jsense"		// Á¶ŔĚ˝şĆ˝ °¨µµ
#define	SETUP_INFO_JOYSTICK_KEYCONFIG					"jkey"			// Ĺ° ĽłÁ¤ °ˇÁö°í żŔ±â
// end 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#include "Background.h"

void TempInterfaceLib() {} // Create Temp Global Folder

// Set debug fog color
extern DWORD g_dwDebugFogColor;
extern float g_pDebugFogStartDistance;
extern float g_pDebugFogEndDistance;
extern D3DXVECTOR3 g_vDebugLightDirection;

#endif // _DEBUG_endif

CInterface::CInterface()
{
	FLOG( "CInterface()" );
	g_pInterface = this;
	m_pTarget = NULL;
//	m_pGageBox = NULL;
	m_pGameMain = NULL;
	m_pPilotFace = NULL;
	m_pSelect = NULL;
	m_pMapLoad = NULL;
	m_pCityBase = NULL;
	m_bShowInterface = TRUE;
	m_pChannelList = NULL;
	m_pFadeEffect = NULL;
	m_pStageEffectData = NULL;
	m_pMapNameImg = NULL;
	m_pGameGlobalImage = NULL;
	m_pBazaarShop = NULL;
	m_pGameArena = NULL;

	// 2010. 03. 03 by ckPark ŔÎÇÇ´ĎĆĽ ÇĘµĺ 2Â÷(ŔÔŔĺ UIşŻ°ć)
	m_pInfinity			= NULL;
	m_pInfinityPopup	= NULL;
	// end 2010. 03. 03 by ckPark ŔÎÇÇ´ĎĆĽ ÇĘµĺ 2Â÷(ŔÔŔĺ UIşŻ°ć)

	m_vecGameWindowWnd.clear();

	// 2007-07-04 by bhsohn żŔĆŰ·ąŔĚĹÍ Ăß°ˇ
	m_pINFOpMain = NULL;
	m_pToolTip = NULL;

	// 2007-07-23 by bhsohn żŔĆŰ·ąŔĚĹÍ Ăß°ˇŔŰľ÷
	m_pSetupConfig = new CSetupConfig;	

	// 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
	m_pJoySticConfig = new CSetupConfig;	

	// 2007-11-28 by bhsohn ˝şĹ©¸° Ľ¦ ¸đµĺ Ăß°ˇ
	m_bScreenShotMode = FALSE;

	// 2008-02-27 by bhsohn ľĆ·ąłŞ ŔĚµż˝Ă Ă¤ĆĂÁ¤ş¸ •üľ÷
	int i = 0;
	for(i = 0; i < CHAT_BUFFER_NUMBER; i++)
	{		
		m_chatBackup[i].pszChatString = NULL;		
		m_chatBackup[i].dwColorType = 0;
		// 2009-05-06 by bhsohn ľĆ·ąłŞ ŔĚµż˝Ă, Ă¤ĆĂ •üľ÷ ľČµÇ´Â ą®Á¦ ÇŘ°á
		m_chatBackup[i].dwType = 0;

	}
	// end 2008-02-27 by bhsohn ľĆ·ąłŞ ŔĚµż˝Ă Ă¤ĆĂÁ¤ş¸ •üľ÷

	// 2008-10-15 by bhsohn ¸®ĽŇ˝ş ¸Ţ¸đ¸® ş¸ČŁ ±â´É Ăß°ˇ
	m_pDummyDataHeader = NULL;
	// end 2008-10-15 by bhsohn ¸®ĽŇ˝ş ¸Ţ¸đ¸® ş¸ČŁ ±â´É Ăß°ˇ

	// 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
	int nCnt = 0;
	for(i=0; i< MAX_JOSTICK_OPTION;i++)
	{
		for(nCnt=0; nCnt< MAX_JOSTICK_OPTION_BEHAVIOR;nCnt++)
		{
			memset(&m_struJoystickOption[i][nCnt], 0x00, sizeof(structJoystickKey));			
			m_struJoystickOption[i][nCnt].nKeyIdx = -1;
			m_struJoystickOption[i][nCnt].nKeyValue = -1;
		}
		
	}
	
	m_pJoystickSetupKeyTable = new structJoystickSetupKeyTable[MAX_JOSTICK_KEY_OPTION];
	for(i=0; i< MAX_JOSTICK_KEY_OPTION;i++)
	{					
		memset(&m_pJoystickSetupKeyTable[i], 0x00, sizeof(structJoystickSetupKeyTable));
		m_pJoystickSetupKeyTable[i].nKeySetup = -1;
		for(nCnt=0; nCnt< MAX_JOSTICK_OPTION_SLOT;nCnt++)
		{
			m_pJoystickSetupKeyTable[i].nBehavior[nCnt] = -1;
		}
	}
	
	// end 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
}

CInterface::~CInterface()
{
	FLOG( "~CInterface()" );
	g_pInterface = NULL;
	SAFE_DELETE(m_pTarget ) ;
//	SAFE_DELETE(m_pGageBox );
	SAFE_DELETE(m_pGameMain);
	SAFE_DELETE(m_pPilotFace);
	SAFE_DELETE(m_pSelect);
	SAFE_DELETE(m_pMapLoad);
	SAFE_DELETE(m_pCityBase);
	SAFE_DELETE(m_pChannelList);
	SAFE_DELETE(m_pFadeEffect);
	SAFE_DELETE(m_pStageEffectData);
	SAFE_DELETE(m_pMapNameImg);
	SAFE_DELETE(m_pGameGlobalImage);
	SAFE_DELETE(m_pBazaarShop);
	SAFE_DELETE(m_pGameArena);

	// 2009. 11. 02 by ckPark ŔÎÇÇ´ĎĆĽ ÇĘµĺ ŔÎ˝şĹĎ˝ş ´řÁŻ ˝Ă˝şĹŰ
	SAFE_DELETE( m_pInfinity );
	// end 2009. 11. 02 by ckPark ŔÎÇÇ´ĎĆĽ ÇĘµĺ ŔÎ˝şĹĎ˝ş ´řÁŻ ˝Ă˝şĹŰ

	// 2010. 03. 03 by ckPark ŔÎÇÇ´ĎĆĽ ÇĘµĺ 2Â÷(ŔÔŔĺ UIşŻ°ć)
	SAFE_DELETE( m_pInfinityPopup );
	// end 2010. 03. 03 by ckPark ŔÎÇÇ´ĎĆĽ ÇĘµĺ 2Â÷(ŔÔŔĺ UIşŻ°ć)

	// 2007-07-04 by bhsohn żŔĆŰ·ąŔĚĹÍ Ăß°ˇ
	SAFE_DELETE(m_pINFOpMain);	 
	// 2007-07-23 by bhsohn żŔĆŰ·ąŔĚĹÍ Ăß°ˇŔŰľ÷
	SAFE_DELETE(m_pSetupConfig);	 

	// 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
	SAFE_DELETE(m_pJoySticConfig);	
	SAFE_DELETE_ARRAY(m_pJoystickSetupKeyTable);
	// end 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷

	// 2008-02-27 by bhsohn ľĆ·ąłŞ ŔĚµż˝Ă Ă¤ĆĂÁ¤ş¸ •üľ÷
	int i = 0;
	for(i = 0; i < CHAT_BUFFER_NUMBER; i++)
	{
		if(m_chatBackup[i].pszChatString != NULL&& 
			strlen(m_chatBackup[i].pszChatString))
		{
			delete [] m_chatBackup[i].pszChatString;
			m_chatBackup[i].pszChatString = NULL;
		}
	}
	// end 2008-02-27 by bhsohn ľĆ·ąłŞ ŔĚµż˝Ă Ă¤ĆĂÁ¤ş¸ •üľ÷

	// 2008-10-15 by bhsohn ¸®ĽŇ˝ş ¸Ţ¸đ¸® ş¸ČŁ ±â´É Ăß°ˇ
	SAFE_DELETE(m_pDummyDataHeader);
	// end 2008-10-15 by bhsohn ¸®ĽŇ˝ş ¸Ţ¸đ¸® ş¸ČŁ ±â´É Ăß°ˇ
}

HRESULT CInterface::InitDeviceObjects()
{
	FLOG( "CInterface::InitDeviceObjects()" );
	m_pMapLoad = new CINFMapLoad() ;

	m_pFadeEffect = new CINFFadeEffect();
	m_pFadeEffect->InitDeviceObjects();
	
	m_pMapNameImg = new CINFMapName() ;
	m_pMapNameImg->InitDeviceObjects("mapname.tex") ;

//	m_pGameGlobalImage = new CGlobalImage() ;	
//	m_pGameGlobalImage->InitDeviceObjects("imagezone.tex") ;
//	g_pGImage = m_pGameGlobalImage;

	// Ă˘żîĽ± ĽřŔ§ ĂĘ±âČ­
	InitWindowsFirstFormat();
	
	// 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
	{
		LoadJoystickInfo(g_pJoysticOp->chJoyKeyFile);
	}

	return S_OK ;
}

HRESULT CInterface::RestoreDeviceObjects()
{
	FLOG( "CInterface::RestoreDeviceObjects()" );
	if(m_pFadeEffect)
		m_pFadeEffect->RestoreDeviceObjects();
	if(m_pMapNameImg )
		m_pMapNameImg->RestoreDeviceObjects() ;
//	if(m_pGameGlobalImage)
//		m_pGameGlobalImage->RestoreDeviceObjects();
	// 2006-07-25 by ispark
	if(m_pBazaarShop)
		m_pBazaarShop->RestoreDeviceObjects();
	// 2008. 12. 05 by ckPark ¸Ţ¸đ¸® ´©Ľö ĽöÁ¤
//	if(m_pGameArena)
//		m_pGameArena->RestoreDeviceObjects();
//	// 2007-07-04 by bhsohn żŔĆŰ·ąŔĚĹÍ Ăß°ˇ
//	if(m_pINFOpMain)
//		m_pINFOpMain->RestoreDeviceObjects();	

	return S_OK ;
}

HRESULT CInterface::InvalidateDeviceObjects()
{
	FLOG( "CInterface::InvalidateDeviceObjects()" );
	InvalidateSelectObjects();
	InvalidateGameObjects();
	InvalidateCityObjects();
	InvalidateChannelListObjects();
	InvalidateSoundScript();
	if(m_pFadeEffect)
		m_pFadeEffect->InvalidateDeviceObjects();
	if(m_pMapNameImg)
		m_pMapNameImg->InvalidateDeviceObjects();
//	if(m_pGameGlobalImage)
//		m_pGameGlobalImage->InvalidateDeviceObjects();
	// 2006-07-25 by ispark
	if(m_pBazaarShop)
		m_pBazaarShop->InvalidateDeviceObjects();
	if(m_pGameArena)
		m_pGameArena->InvalidateDeviceObjects();

	// 2009. 11. 02 by ckPark ŔÎÇÇ´ĎĆĽ ÇĘµĺ ŔÎ˝şĹĎ˝ş ´řÁŻ ˝Ă˝şĹŰ
	if( m_pInfinity )
		m_pInfinity->InvalidateDeviceObjects();
	// end 2009. 11. 02 by ckPark ŔÎÇÇ´ĎĆĽ ÇĘµĺ ŔÎ˝şĹĎ˝ş ´řÁŻ ˝Ă˝şĹŰ

	// 2010. 03. 03 by ckPark ŔÎÇÇ´ĎĆĽ ÇĘµĺ 2Â÷(ŔÔŔĺ UIşŻ°ć)
	if( m_pInfinityPopup )
		m_pInfinityPopup->InvalidateDeviceObjects();
	// end 2010. 03. 03 by ckPark ŔÎÇÇ´ĎĆĽ ÇĘµĺ 2Â÷(ŔÔŔĺ UIşŻ°ć)

	// 2007-07-04 by bhsohn żŔĆŰ·ąŔĚĹÍ Ăß°ˇ
	if(m_pINFOpMain)
		m_pINFOpMain->InvalidateDeviceObjects();

	return S_OK ;
}

HRESULT CInterface::DeleteDeviceObjects()
{
	FLOG( "CInterface::DeleteDeviceObjects()" );
	// 2008-06-20 by bhsohn EP3 żÉĽÇ°ü·Ă Ăł¸®
	if(g_pD3dApp->IsOptionEtc(OPTION_RADIO_INTERFACE_POS))
	{
		// ŔúŔĺ ¸đµĺ¸é żÉĽÇÁ¤ş¸ ŔúŔĺ
		g_pInterface->SaveOptionEtcInfo();		
	}
	// end 2008-06-20 by bhsohn EP3 żÉĽÇ°ü·Ă Ăł¸®

	DeleteSelectObjects();
	// 2007-11-22 by bhsohn ľĆ·ąłŞ ĹëÇŐĽ­ąö
	//DeleteGameObjects();
	DeleteGameObjects(FALSE);

	DeleteCityObjects();
	DeleteChannelListObjects();
	DeleteSoundScript(false);
	SAFE_DELETE(m_pMapLoad);
	if(m_pFadeEffect)
	{
		m_pFadeEffect->DeleteDeviceObjects();
		SAFE_DELETE(m_pFadeEffect);
	}
	if(m_pMapNameImg)
	{
		m_pMapNameImg->DeleteDeviceObjects();
		SAFE_DELETE(m_pMapNameImg);
	}
	// 2006-07-25 by ispark
	if(m_pBazaarShop)
		m_pBazaarShop->DeleteDeviceObjects();
	if(m_pGameArena)
		m_pGameArena->DeleteDeviceObjects();

	// 2009. 11. 02 by ckPark ŔÎÇÇ´ĎĆĽ ÇĘµĺ ŔÎ˝şĹĎ˝ş ´řÁŻ ˝Ă˝şĹŰ
	if( m_pInfinity )
		m_pInfinity->DeleteDeviceObjects();
	// end 2009. 11. 02 by ckPark ŔÎÇÇ´ĎĆĽ ÇĘµĺ ŔÎ˝şĹĎ˝ş ´řÁŻ ˝Ă˝şĹŰ

	// 2010. 03. 03 by ckPark ŔÎÇÇ´ĎĆĽ ÇĘµĺ 2Â÷(ŔÔŔĺ UIşŻ°ć)
	if( m_pInfinityPopup )
		m_pInfinityPopup->DeleteDeviceObjects();
	// end 2010. 03. 03 by ckPark ŔÎÇÇ´ĎĆĽ ÇĘµĺ 2Â÷(ŔÔŔĺ UIşŻ°ć)


	// 2007-07-04 by bhsohn żŔĆŰ·ąŔĚĹÍ Ăß°ˇ
	if(m_pINFOpMain)
		m_pINFOpMain->DeleteDeviceObjects();

	return S_OK;
}
HRESULT CInterface::InitSoundScript(char* szFileName, STAGE_EFFECT_DATA * pStageEffectData /* = NULL*/, BYTE byEndAct/* = NOT_ACTION*/, BOOL bSkip/* = TRUE*/)
{
	m_pStageEffectData = new CINFStageEffectData;
	m_pStageEffectData->SetOldGameState( g_pD3dApp->m_dwGameState );
	m_pStageEffectData->SetData(pStageEffectData, byEndAct, bSkip);
	if(FAILED(m_pStageEffectData->LoadData(szFileName)))
	{
		DeleteSoundScript();
		return E_FAIL;
	}
	if(FAILED(m_pStageEffectData->InitDeviceObjects()))
	{
		DeleteSoundScript();
		return E_FAIL;
	}

	g_pD3dApp->ChangeGameState( _SCRIPT );
	return S_OK;
}
HRESULT CInterface::RestoreSoundScript()
{
	if(m_pStageEffectData)
	{
		m_pStageEffectData->RestoreDeviceObjects();
	}
	return S_OK;
}

HRESULT CInterface::InvalidateSoundScript()
{
	if(m_pStageEffectData)
	{
		m_pStageEffectData->InvalidateDeviceObjects();
	}
	return S_OK;
}

HRESULT CInterface::DeleteSoundScript(bool restore_game_state /*= true*/)
{
	if(m_pStageEffectData)
	{
		if (restore_game_state)
		{
			g_pD3dApp->ChangeGameState(m_pStageEffectData->GetOldGameState());
			ScriptEndAct(m_pStageEffectData->GetEndAct());
		}

		m_pStageEffectData->InvalidateDeviceObjects();
		m_pStageEffectData->DeleteDeviceObjects();
		SAFE_DELETE(m_pStageEffectData);
	}
	return S_OK ;
}

HRESULT CInterface::InitGameObjects()
{
	FLOG( "CInterface::InitGameObjects()" );

	m_pGameMain = new CINFGameMain(this) ;
	m_pGameMain->InitDeviceObjects() ;
	m_pTarget = new CINFTarget();
	m_pTarget->SetGameData(m_pGameMain->m_pGameData);
	m_pTarget->InitDeviceObjects() ;
	m_pPilotFace = new CINFPilotFace() ;
	m_pPilotFace->InitDeviceObjects("charicon.tex") ;

	m_pGameGlobalImage = new CGlobalImage() ;
	m_pGameGlobalImage->SetGameData(m_pGameMain->m_pGameData);
	m_pGameGlobalImage->InitDeviceObjects() ;
	g_pGImage = m_pGameGlobalImage;
	
	// 2007-11-22 by bhsohn ľĆ·ąłŞ ĹëÇŐĽ­ąö
//	m_pGameArena = new CINFGameArena();
//	m_pGameArena->SetGameData(m_pGameMain->m_pGameData);
//	m_pGameArena->InitDeviceObjects();
	if(NULL == m_pGameArena)
	{
		m_pGameArena = new CINFGameArena();		
	}
	m_pGameArena->SetGameData(m_pGameMain->m_pGameData);
	m_pGameArena->InitDeviceObjects();
	// end 2007-11-22 by bhsohn ľĆ·ąłŞ ĹëÇŐĽ­ąö


	// 2009. 11. 02 by ckPark ŔÎÇÇ´ĎĆĽ ÇĘµĺ ŔÎ˝şĹĎ˝ş ´řÁŻ ˝Ă˝şĹŰ
	if( m_pInfinity == NULL )
	{
		m_pInfinity = new CINFInfinity;
	}
	m_pInfinity->SetGameData( m_pGameMain->m_pGameData );
	m_pInfinity->InitDeviceObjects();
	// end 2009. 11. 02 by ckPark ŔÎÇÇ´ĎĆĽ ÇĘµĺ ŔÎ˝şĹĎ˝ş ´řÁŻ ˝Ă˝şĹŰ

	// 2010. 03. 03 by ckPark ŔÎÇÇ´ĎĆĽ ÇĘµĺ 2Â÷(ŔÔŔĺ UIşŻ°ć)
	if( m_pInfinityPopup == NULL )
		m_pInfinityPopup = new CINFCityInfinityFieldPopUp;

	m_pInfinityPopup->SetGameData( m_pGameMain->m_pGameData );
	m_pInfinityPopup->InitDeviceObjects();
	// end 2010. 03. 03 by ckPark ŔÎÇÇ´ĎĆĽ ÇĘµĺ 2Â÷(ŔÔŔĺ UIşŻ°ć)


	// 2007-07-04 by bhsohn żŔĆŰ·ąŔĚĹÍ Ăß°ˇ	
	m_pINFOpMain = new CINFOpMain();
	m_pINFOpMain->SetGameData(m_pGameMain->m_pGameData);
	m_pINFOpMain->InitDeviceObjects();

	m_pToolTip = new CINFToolTip();
	m_pToolTip->SetGameData(m_pGameMain->m_pGameData);
	m_pToolTip->InitDeviceObjects();
	
	// 2009-02-13 by bhsohn żůµĺ ·©Ĺ· ˝Ă˝şĹŰ
	{
		CWorldRankManager*	pWorldRankManager = g_pD3dApp->GetWorldRankManager();
		pWorldRankManager->InitDeviceObjects_ServiceInfoImage();
	}
	// end 2009-02-13 by bhsohn żůµĺ ·©Ĺ· ˝Ă˝şĹŰ
	

	return S_OK ;
}

HRESULT CInterface::RestoreGameObjects()
{
	FLOG( "CInterface::RestoreGameObjects()" );
	if(m_pTarget)
		m_pTarget->RestoreDeviceObjects();
	if(m_pGameMain )
		m_pGameMain->RestoreDeviceObjects() ;
	if(m_pPilotFace )
		m_pPilotFace->RestoreDeviceObjects() ;
	if(m_pGameGlobalImage)
		m_pGameGlobalImage->RestoreDeviceObjects();
	if(m_pGameArena)
		m_pGameArena->RestoreDeviceObjects();

	// 2009. 11. 02 by ckPark ŔÎÇÇ´ĎĆĽ ÇĘµĺ ŔÎ˝şĹĎ˝ş ´řÁŻ ˝Ă˝şĹŰ
	if( m_pInfinity )
		m_pInfinity->RestoreDeviceObjects();
	// end 2009. 11. 02 by ckPark ŔÎÇÇ´ĎĆĽ ÇĘµĺ ŔÎ˝şĹĎ˝ş ´řÁŻ ˝Ă˝şĹŰ


	// 2010. 03. 03 by ckPark ŔÎÇÇ´ĎĆĽ ÇĘµĺ 2Â÷(ŔÔŔĺ UIşŻ°ć)
	if( m_pInfinityPopup )
		m_pInfinityPopup->RestoreDeviceObjects();
	// end 2010. 03. 03 by ckPark ŔÎÇÇ´ĎĆĽ ÇĘµĺ 2Â÷(ŔÔŔĺ UIşŻ°ć)


	// 2007-07-04 by bhsohn żŔĆŰ·ąŔĚĹÍ Ăß°ˇ		
	if(m_pINFOpMain)
		m_pINFOpMain->RestoreDeviceObjects();
	if(m_pToolTip)
		m_pToolTip->RestoreDeviceObjects();

	// 2009-02-13 by bhsohn żůµĺ ·©Ĺ· ˝Ă˝şĹŰ
	{
		CWorldRankManager*	pWorldRankManager = g_pD3dApp->GetWorldRankManager();
		pWorldRankManager->RestoreDeviceObjects_ServiceInfoImage();
	}
	// end 2009-02-13 by bhsohn żůµĺ ·©Ĺ· ˝Ă˝şĹŰ

	return S_OK ;
}

HRESULT CInterface::InvalidateGameObjects()
{
	FLOG( "CInterface::InvalidateGameObjects()" );
	if(m_pTarget)
		m_pTarget->InvalidateDeviceObjects();
	if(m_pGameMain)
		m_pGameMain->InvalidateDeviceObjects();
	if(m_pPilotFace)
		m_pPilotFace->InvalidateDeviceObjects();
	if(m_pGameGlobalImage)
		m_pGameGlobalImage->InvalidateDeviceObjects();
	if(m_pGameArena)
		m_pGameArena->InvalidateDeviceObjects();

	// 2009. 11. 02 by ckPark ŔÎÇÇ´ĎĆĽ ÇĘµĺ ŔÎ˝şĹĎ˝ş ´řÁŻ ˝Ă˝şĹŰ
	if( m_pInfinity )
		m_pInfinity->InvalidateDeviceObjects();
	// end 2009. 11. 02 by ckPark ŔÎÇÇ´ĎĆĽ ÇĘµĺ ŔÎ˝şĹĎ˝ş ´řÁŻ ˝Ă˝şĹŰ


	// 2010. 03. 03 by ckPark ŔÎÇÇ´ĎĆĽ ÇĘµĺ 2Â÷(ŔÔŔĺ UIşŻ°ć)
	if( m_pInfinityPopup )
		m_pInfinityPopup->InvalidateDeviceObjects();
	// end 2010. 03. 03 by ckPark ŔÎÇÇ´ĎĆĽ ÇĘµĺ 2Â÷(ŔÔŔĺ UIşŻ°ć)
	


	// 2007-07-04 by bhsohn żŔĆŰ·ąŔĚĹÍ Ăß°ˇ		
	if(m_pINFOpMain)
		m_pINFOpMain->InvalidateDeviceObjects();
	if(m_pToolTip)
		m_pToolTip->InvalidateDeviceObjects();

	// 2009-02-13 by bhsohn żůµĺ ·©Ĺ· ˝Ă˝şĹŰ
	{
		CWorldRankManager*	pWorldRankManager = g_pD3dApp->GetWorldRankManager();
		pWorldRankManager->InvalidateDeviceObjects_ServiceInfoImage();
	}
	// end 2009-02-13 by bhsohn żůµĺ ·©Ĺ· ˝Ă˝şĹŰ

	return S_OK ;
}

HRESULT CInterface::DeleteGameObjects(BOOL bArenaStart)
{
	FLOG( "CInterface::DeleteGameObjects()" );
	if(m_pTarget)
	{
		m_pTarget->DeleteDeviceObjects();
		SAFE_DELETE(m_pTarget) ;
	}
	if(m_pGameGlobalImage)
	{
		m_pGameGlobalImage->DeleteDeviceObjects();
		SAFE_DELETE(m_pGameGlobalImage);
	}
	if(m_pGameMain )
	{
		m_pGameMain->DeleteDeviceObjects() ;
		SAFE_DELETE(m_pGameMain ) ;
	}
	if(m_pPilotFace )
	{
		m_pPilotFace->DeleteDeviceObjects() ;
		SAFE_DELETE(m_pPilotFace ) ;
	}
	if(m_pGameArena)
	{
		m_pGameArena->DeleteDeviceObjects() ;
		if(FALSE == bArenaStart)
		{
			SAFE_DELETE(m_pGameArena ) ;
		}
	}

	// 2009. 11. 02 by ckPark ŔÎÇÇ´ĎĆĽ ÇĘµĺ ŔÎ˝şĹĎ˝ş ´řÁŻ ˝Ă˝şĹŰ
	if( m_pInfinity )
	{
		m_pInfinity->DeleteDeviceObjects();
		SAFE_DELETE( m_pInfinity );
	}
	// end 2009. 11. 02 by ckPark ŔÎÇÇ´ĎĆĽ ÇĘµĺ ŔÎ˝şĹĎ˝ş ´řÁŻ ˝Ă˝şĹŰ


	// 2010. 03. 03 by ckPark ŔÎÇÇ´ĎĆĽ ÇĘµĺ 2Â÷(ŔÔŔĺ UIşŻ°ć)
	if( m_pInfinityPopup )
	{
		m_pInfinityPopup->DeleteDeviceObjects();
		SAFE_DELETE( m_pInfinityPopup );
	}
	// end 2010. 03. 03 by ckPark ŔÎÇÇ´ĎĆĽ ÇĘµĺ 2Â÷(ŔÔŔĺ UIşŻ°ć)

	// 2007-07-04 by bhsohn żŔĆŰ·ąŔĚĹÍ Ăß°ˇ			
	if(m_pINFOpMain)
	{
		m_pINFOpMain->DeleteDeviceObjects() ;
		SAFE_DELETE(m_pINFOpMain ) ;
	}
	if(m_pToolTip)
	{
		m_pToolTip->DeleteDeviceObjects();
		SAFE_DELETE(m_pToolTip);
	}

	// 2008-10-27 by bhsohn ą«ÇŃŔ¸·Î ş¸±Ţ °ˇ´ÉÇŃ ąö±× ĽöÁ¤
	ReleaseBazaarShop();

	// 2009-02-13 by bhsohn żůµĺ ·©Ĺ· ˝Ă˝şĹŰ
	{
		CWorldRankManager*	pWorldRankManager = g_pD3dApp->GetWorldRankManager();
		pWorldRankManager->DeleteDeviceObjects_ServiceInfoImage();
	}
	// end 2009-02-13 by bhsohn żůµĺ ·©Ĺ· ˝Ă˝şĹŰ

	return S_OK ;
}

HRESULT	CInterface::InitChannelListObjects()
{
	FLOG( "CInterface::InitChannelListObjects()" );

	ASSERT_ASSERT(g_pChannelList == NULL);
	m_pChannelList = new CINFChannelList(this);
	m_pChannelList->SetGameData( g_pGameMain->m_pGameData);
	m_pChannelList->InitDeviceObjects();

	return S_OK ;
}

HRESULT CInterface::RestoreChannelListObjects()
{
	FLOG( "CInterface::RestoreChannelListObjects()" );
	if(m_pChannelList)
	{
		m_pChannelList->RestoreDeviceObjects();
	}
	
	return S_OK ;
}

HRESULT CInterface::InvalidateChannelListObjects()
{
	FLOG( "CInterface::InvalidateChannelListObjects()" );
	if(m_pChannelList)
	{
		m_pChannelList->InvalidateDeviceObjects();
	}

	return S_OK ;
}

HRESULT CInterface::DeleteChannelListObjects()
{
	FLOG( "CInterface::DeleteChannelListObjects()" );
	if(m_pChannelList)
	{
		m_pChannelList->DeleteDeviceObjects();
		SAFE_DELETE(m_pChannelList);
	}

	return S_OK ;
}

HRESULT CInterface::InitMapLoadObjects()
{
	FLOG( "CInterface::InitMapLoadObjects()" );
	m_pMapLoad->InitDeviceObjects() ;

	return S_OK ;
}

HRESULT CInterface::RestoreMapLoadObjects()
{
	FLOG( "CInterface::RestoreMapLoadObjects()" );
	if(m_pMapLoad )
	{
		m_pMapLoad->RestoreDeviceObjects() ;
	}
	return S_OK ;
}

HRESULT CInterface::InvalidateMapLoadObjects()
{
	FLOG( "CInterface::InvalidateMapLoadObjects()" );
	if(m_pMapLoad)
	{
		m_pMapLoad->InvalidateDeviceObjects();
	}
	return S_OK ;
}

HRESULT CInterface::DeleteMapLoadObjects()
{
	FLOG( "CInterface::DeleteMapLoadObjects()" );
	if(m_pMapLoad)
	{
		m_pMapLoad->DeleteDeviceObjects() ;
//		SAFE_DELETE(m_pMapLoad);
	}
	return S_OK ;
}

HRESULT CInterface::InitCityObjects()
{
	FLOG( "CInterface::InitCityObjects()" );
	m_pCityBase = new CINFCityBase(this) ;
	m_pCityBase->SetGameData(m_pGameMain->m_pGameData);
	m_pCityBase->InitDeviceObjects();

	return S_OK ;
}

HRESULT CInterface::RestoreCityObjects()
{
	FLOG( "CInterface::RestoreCityObjects()" );
	if(m_pCityBase )
	{
		m_pCityBase->RestoreDeviceObjects() ;
	}

	return S_OK ;
}

HRESULT CInterface::InvalidateCityObjects()
{
	FLOG( "CInterface::InvalidateCityObjects()" );
	if(m_pCityBase)
	{
		m_pCityBase->InvalidateDeviceObjects();
	}
	return S_OK ;
}

HRESULT CInterface::DeleteCityObjects()
{
	FLOG( "CInterface::DeleteCityObjects()" );
	if(m_pCityBase)
	{
		m_pCityBase->DeleteDeviceObjects() ;
		SAFE_DELETE(m_pCityBase);
	}
	return S_OK ;
}

HRESULT CInterface::InitSelectObjects()
{
	FLOG( "CInterface::InitSelectObjects()" );
	m_pSelect = new CINFSelect(this) ;
	m_pSelect->InitDeviceObjects();
	return S_OK ;
}

HRESULT CInterface::RestoreSelectObjects()
{
	FLOG( "CInterface::RestoreSelectObjects()" );
	if(m_pSelect )
		m_pSelect->RestoreDeviceObjects() ;

	ZeroMemory( &m_light0, sizeof(D3DLIGHT9) );
	m_light0.Type       = D3DLIGHT_DIRECTIONAL;
	m_light0.Diffuse.r  = 0.3f;
	m_light0.Diffuse.g  = 0.3f;
	m_light0.Diffuse.b  = 0.3f;
	m_light0.Ambient.r  = 0.1f;
	m_light0.Ambient.g  = 0.1f;
	m_light0.Ambient.b  = 0.1f;
	m_light0.Specular.r  = 1.0f;
	m_light0.Specular.g  = 1.0f;
	m_light0.Specular.b  = 1.0f;
	m_light0.Direction  = D3DXVECTOR3( -0.5, 0, -1 );
	m_light0.Position   = D3DXVECTOR3( 500, 500, 500 );
	m_light0.Range      = 1000.0f;
	
	ZeroMemory( &m_light1, sizeof(D3DLIGHT9) );
	m_light1.Type       = D3DLIGHT_SPOT;
	m_light1.Diffuse.r  = 1.0f;
	m_light1.Diffuse.g  = 1.0f;
	m_light1.Diffuse.b  = 1.0f;
	m_light1.Ambient.r  = 0.3f;
	m_light1.Ambient.g  = 0.3f;
	m_light1.Ambient.b  = 0.3f;
	m_light1.Specular.r  = 1.0f;
	m_light1.Specular.g  = 1.0f;
	m_light1.Specular.b  = 1.0f;
	m_light1.Direction  = D3DXVECTOR3( 0, -1, 0 );
	m_light1.Position   = D3DXVECTOR3( 0,-10,15 );
	m_light1.Range      = 1000.0f;
//    m_light1.Position     = 2.0f * D3DXVECTOR3( x, y, z );
//    m_light1.Direction    = D3DXVECTOR3( x, y, z );
	m_light1.Theta        = 0.5f;
	m_light1.Phi          = 1.4f;
	m_light1.Falloff      = 1.0f;
	m_light1.Attenuation0 = 1.0f;

	
	return S_OK ;
}

HRESULT CInterface::DeleteSelectObjects()
{
	FLOG( "CInterface::DeleteSelectObjects()" );
	if(m_pSelect )
		m_pSelect->DeleteDeviceObjects() ;
	SAFE_DELETE(m_pSelect ) ;
	return S_OK ;

}

HRESULT CInterface::InvalidateSelectObjects()
{
	FLOG( "CInterface::InvalidateSelectObjects()" );
	if(m_pSelect)
		m_pSelect->InvalidateDeviceObjects();
	return S_OK ;
}
	
void CInterface::Tick()
{
	FLOG( "CInterface::Tick()" );
	switch(g_pD3dApp->m_dwGameState)
	{
	case _SELECT:
	case _CREATE:
	case _OPTION:
		{
			if(m_pSelect)
				m_pSelect->Tick();
		}
		break;
	case _MAPLOAD:
		{
			if(m_pMapLoad)
				m_pMapLoad->Tick();
			// 2007-09-07 by dgwoo ˝Ă°ŁŔ» ¸Ę·Îµĺ˝Ăżˇµµ ĂĽĹ©ÇĎ±âŔ§ÇŘ.
			if(m_pGameMain && m_pGameMain->m_pOutPost)
			{
				m_pGameMain->m_pOutPost->Tick();
			}

		}
		break;
	case _SELECTCHANNEL:
		{
			if(m_pChannelList)
				m_pChannelList->Tick();
			// 2007-09-07 by dgwoo ˝Ă°ŁŔ» ¸Ę·Îµĺ˝Ăżˇµµ ĂĽĹ©ÇĎ±âŔ§ÇŘ.
			if(m_pGameMain && m_pGameMain->m_pOutPost)
			{
				m_pGameMain->m_pOutPost->Tick();
			}
		}
		break;
	case _SHOP:
	case _GAME:
		{
			if(TRUE == m_pMapLoad->GetRenderMapNameImg())
			{	// ¸ĘŔĚ¸§ »ç¶óÁö±â..
				m_pMapLoad->m_dwAlpha = g_pGameMain->GetCurrentColor(m_pMapLoad->m_fMapNameViewTime, ALPHA_DELAY_WAVE, 8.0f,2.0f);
				m_pMapLoad->m_fMapNameViewTime += g_pD3dApp->GetElapsedTime();
				if(m_pMapLoad->m_fMapNameViewTime > 8.0f)
				{
					m_pMapLoad->m_fMapNameViewTime = MAPLOAD_NAME_IMG;
					m_pMapLoad->SetRenderMapNameImg(FALSE);
				}
			}

			// »óÁˇżˇĽ­ »çżëµČ ľĆŔĚĹŰŔĚ ˝ÇÁ¦ °ÔŔÓ»óżˇĽ­µµ »çżëµÇ°í ŔÖ´Â°ˇ?
			// Gear colorŔűżë
			// 2007-08-09 by dgwoo FieldSocketEventLeaveBuildingOk ż©±âĽ­ Ăł¸®.
			//SetShopItemUseSerch();
			
			if( g_pD3dApp->m_dwGameState != _SHOP && m_pTarget)	
				m_pTarget->Tick();
			// µĄąĚÁö şäľî
			if(g_pGameMain->m_pDamageView)
				g_pGameMain->m_pDamageView->Tick();
			if(m_pGameGlobalImage)
				m_pGameGlobalImage->Tick();
			// 2007-06-12 by bhsohn ąĚĽÇ ŔÎĹÍĆäŔĚ˝ş ĽöÁ¤ľČ
//			if(g_pGameMain->m_pMissionInfo)
//				g_pGameMain->m_pMissionInfo->Tick();
			g_pGameMain->TickMissonMain();
			
			// 2007-08-14 by dgwoo Äł˝¬ »óÁˇżˇĽ­ ąĚ·Ż Ăß°ˇ
			if( m_pCityBase )
				m_pCityBase->Tick();
			// 2007-09-19 by dgwoo °ÔŔÓÁß ĹřĆÁ Ăß°ˇ.
			if(m_pToolTip)
				m_pToolTip->Tick();
			// 2007-09-07 by dgwoo ŔüÁř±âÁöŔü Ăß°ˇ.
			if(m_pGameMain->m_pOutPost)
			{
				m_pGameMain->m_pOutPost->Tick();
			}
			// 2007-10-22 by dgwoo Áľ·á˝Ă ˝Ă°ŁŔĚ ľČ°ˇ´Â ąö±×ąß»ý żąąć Â÷żřżˇĽ­ Ŕ§Äˇ ĽöÁ¤.
			if(m_pGameMain->m_pSystem)
			{
				m_pGameMain->m_pSystem->Tick();
			}

			// 2009. 01. 23 by ckPark ŔÎĹÍĆäŔĚ˝ş ˛°Ŕ» ˝Ă Ć÷Ĺ»/˝şĹł »çżëÁöż¬ ĂĽĹ©
			if(m_pGameMain)
			{
				m_pGameMain->TickItemDelay();
				m_pGameMain->TickSkillDealy();
			}			
			// end 2009. 01. 23 by ckPark ŔÎĹÍĆäŔĚ˝ş ˛°Ŕ» ˝Ă Ć÷Ĺ»/˝şĹł »çżëÁöż¬ ĂĽĹ©

			// 2010. 02. 11 by ckPark ąßµż·ů ŔĺÂřľĆŔĚĹŰ
			if( m_pGameMain )
				m_pGameMain->TickInvokeWearItem();
			// end 2010. 02. 11 by ckPark ąßµż·ů ŔĺÂřľĆŔĚĹŰ
			
			if(m_bShowInterface)
			{
				// Á¤ş¸Ă˘ŔĚ ş¸ŔĎ¶§
				if( m_pGameMain )
				{
					m_pGameMain->Tick();
				}
			}
			else
			{
				// ŔÎĹÍĆäŔĚ˝ş Á¤ş¸Ă˘ŔĚ ş¸ŔĚÁö ľĘŔ»¶§.
				if( m_pGameMain )
				{
					// 2007-10-05 by bhsohn FĹ°·Î Ľű°Üµµ ˝şÇÇÄż ˝şĆ®¸µ ş¸ŔĚ°Ô şŻ°ć					
					m_pGameMain->TickSpeakerMsg();
					// end 2007-10-05 by bhsohn FĹ°·Î Ľű°Üµµ ˝şÇÇÄż ˝şĆ®¸µ ş¸ŔĚ°Ô şŻ°ć
					
					// 2005-07-18 by ispark
					// Ă˘ŔĚ ¶çľîÁłŔ»¶§ FĹ°¸¦ ´©¸Ł¸é ŔŻ´ÖŔĚ Á¤ÁöÇŘ ąö¸°´Ů. 
					// ±×°ÍŔş m_pGameMain->Tick()Ŕ» µąÁö ľĘ±â ¶§ą®ŔĚ´Ů.
					// ąŘżˇ ÇŃÁŮ ĽŇ˝ş°ˇ ŔŻ´ÖŔ» °­Á¦·Î Á¤Áö ˝ĂĹ°Áö ľĘ´Â´Ů.
					//g_pD3dApp->m_bFixUnitDirMode = FALSE;

					// 2007-07-24 by dgwoo invenČ°ĽşČ­Áß FĹ°¸¦ ´©¸Ł¸é ą«±â Á¤ş¸Ă˘ŔĚ »ç¶÷Áö¸ç şóĂ˘ŔĚ ş¸ŔÓ.
					m_pGameMain->TickDrawWindowInfo();

					// 2005-07-18 by ispark
					// Ă˘ŔĚ Ľű°Ü ŔÖľîµÎ ŔŻ´Ö »óĹÂ ş¸ż©ÁÖ±â
					m_pGameMain->m_pUnitState->Tick();
					m_pGameMain->m_pQuickSlot->Tick();
					m_pGameMain->m_pChat->Tick();
					m_pGameMain->m_pMiniMap->Tick();
					m_pGameMain->m_pUnitInfoBar->Tick();
					m_pGameMain->m_pGameCountDown->Tick();
					// 2005-08-08 by ispark MP3 °čĽÓ ŔŻÁöÇĎ±â
					if(m_pGameMain->m_pMp3Player && m_pGameMain->m_bMp3PlayFlag == TRUE)
						m_pGameMain->m_pMp3Player->Tick();
					if(m_pGameMain->m_pInfGameMainFaq && m_pGameMain->m_bFAQProsFlag)
						m_pGameMain->m_pInfGameMainFaq->Tick();
					
					for(int i=0;i<INF_AD_DIRECTION_MAX;i++)
					{
						if(m_pGameMain->m_pAttackDirection->tRemainTime[i].Get() > 0.0f)
							m_pGameMain->m_pAttackDirection->tRemainTime[i].Set(0);
					}

					// 2009. 11. 02 by ckPark ŔÎÇÇ´ĎĆĽ ÇĘµĺ ŔÎ˝şĹĎ˝ş ´řÁŻ ˝Ă˝şĹŰ
					if( m_pGameMain->m_pInfWindow )
						m_pGameMain->m_pInfWindow->Tick();
					// end 2009. 11. 02 by ckPark ŔÎÇÇ´ĎĆĽ ÇĘµĺ ŔÎ˝şĹĎ˝ş ´řÁŻ ˝Ă˝şĹŰ

				}
			}
#ifdef S_ENABLE_KILLFEED
            m_pGameMain->m_pINFEvoKillfeed->Tick(); // tick with interface on and off (m_bShowInterface)
#endif
			// 2006-07-30 by ispark, °łŔÎ »óÁˇ
			if(m_pBazaarShop)
			{
				// Áľ·á ĂĽĹ©
				if(m_pBazaarShop->GetbBazaarShopEnd())
				{
					ReadyCloseBazaarShop();
				}
			}

			if(g_pD3dApp->m_bGameShutDown)
			{
				GameShutDownTick();
			}
			if(m_pGameArena)
				m_pGameArena->Tick();


			// 2009. 11. 02 by ckPark ŔÎÇÇ´ĎĆĽ ÇĘµĺ ŔÎ˝şĹĎ˝ş ´řÁŻ ˝Ă˝şĹŰ
			if( m_pInfinity )
				m_pInfinity->Tick();
			// end 2009. 11. 02 by ckPark ŔÎÇÇ´ĎĆĽ ÇĘµĺ ŔÎ˝şĹĎ˝ş ´řÁŻ ˝Ă˝şĹŰ


			// 2007-07-04 by bhsohn żŔĆŰ·ąŔĚĹÍ Ăß°ˇ
			m_pINFOpMain->Tick(g_pD3dApp->m_dwGameState);
		}
		break;
//	case _SHOP:
//		{
//			if( m_pGameMain )
//				m_pGameMain->Tick() ;
//			if( m_pCityBase )
//				m_pCityBase->Tick() ;
//			if(m_pGameGlobalImage)
//				m_pGameGlobalImage->Tick();
//			// µĄąĚÁö şäľî
//			if(g_pGameMain->m_pDamageView)
//				g_pGameMain->m_pDamageView->Tick();
//
//		}
//		break;
	case _WAITING:
		break;
	case _CITY:
		{
			if(TRUE == m_pMapLoad->GetRenderMapNameImg())
			{	// ¸ĘŔĚ¸§ »ç¶óÁö±â..
				m_pMapLoad->m_dwAlpha = g_pGameMain->GetCurrentColor(m_pMapLoad->m_fMapNameViewTime, ALPHA_DELAY_WAVE, 8.0f,2.0f);
				m_pMapLoad->m_fMapNameViewTime += g_pD3dApp->GetElapsedTime();
				if(m_pMapLoad->m_fMapNameViewTime > 8.0f)
				{
					m_pMapLoad->m_fMapNameViewTime = MAPLOAD_NAME_IMG;
					m_pMapLoad->SetRenderMapNameImg(FALSE);
				}
			}
			if(m_pGameGlobalImage)
				m_pGameGlobalImage->Tick();
			if( m_pGameMain )
				m_pGameMain->Tick() ;

			// 2009. 01. 23 by ckPark ŔÎĹÍĆäŔĚ˝ş ˛°Ŕ» ˝Ă Ć÷Ĺ»/˝şĹł »çżëÁöż¬ ĂĽĹ©
			if(m_pGameMain)
			{
				m_pGameMain->TickItemDelay();					
				m_pGameMain->TickSkillDealy();
			}			
			// end 2009. 01. 23 by ckPark ŔÎĹÍĆäŔĚ˝ş ˛°Ŕ» ˝Ă Ć÷Ĺ»/˝şĹł »çżëÁöż¬ ĂĽĹ©

			// 2010. 02. 11 by ckPark ąßµż·ů ŔĺÂřľĆŔĚĹŰ
			if( m_pGameMain )
				m_pGameMain->TickInvokeWearItem();
			// end 2010. 02. 11 by ckPark ąßµż·ů ŔĺÂřľĆŔĚĹŰ

			if( m_pCityBase )
				m_pCityBase->Tick() ;
		}
		break;
	case _SCRIPT:
		{
			if( m_pStageEffectData )
			{
				if( m_pStageEffectData->Tick( g_pD3dApp->GetElapsedTime() ) == FALSE )
				{
					InvalidateSoundScript();
					DeleteSoundScript();
				}
			}
		}
	default:
		{
		}
	}
}


void CInterface::Render()
{
	FLOG( "CInterface::Render()" );

	switch(g_pD3dApp->m_dwGameState)
	{
	case _SELECT:
	case _CREATE:
	case _OPTION:
		{
			if(m_pSelect) m_pSelect->Render() ;
		}
		break;
	case _MAPLOAD:
		{
			if(m_pMapLoad)
			{
				m_pMapLoad->Render();
			}
			if( m_pTarget )
			{
				POINT pt;
				GetCursorPos(&pt);
				ScreenToClient(g_pD3dApp->GetHwnd(), &pt);
				CheckMouseReverse(&pt);				

				m_pTarget->RenderMouse(pt.x, pt.y, MOUSE_TYPE_0);
			}
		}
		break;
	case _SELECTCHANNEL:
		{
			if(m_pChannelList)
				m_pChannelList->Render();
			if( m_pTarget )
			{
				POINT pt;
				GetCursorPos(&pt);
				ScreenToClient(g_pD3dApp->GetHwnd(), &pt);
				CheckMouseReverse(&pt);

				m_pTarget->RenderMouse(pt.x, pt.y, MOUSE_TYPE_0);
			}
		}
		break;
	case _SHOP:
	case _GAME:
		{
			if(	g_pTutorial && 
				g_pTutorial->IsTutorialMode() == TRUE && 
				g_pTutorial->IsRenderInterface() == FALSE )
			{
				break;
			}
			// 2007-11-28 by bhsohn ˝şĹ©¸° Ľ¦ ¸đµĺ Ăß°ˇ
//#ifdef _SCREENSHOT // ˝şĹ©¸° Ľ¦ ¸đµĺ
			if(g_pInterface->IsScreenShotMode())
			{
				// 2008-06-20 by bhsohn EP3 żÉĽÇ°ü·Ă Ăł¸®
//				if(m_bShowInterface)
//				{
//					if( m_pGameMain )
//					{
//						m_pGameMain->Render() ;
//					}
//					if( m_pTarget )
//						m_pTarget->Render();
//				}
				// 2008-06-20 by bhsohn EP3 żÉĽÇ°ü·Ă Ăł¸®				
				// żěĽ±ĽřŔ§ ·»´ő¸µ ĽłÁ¤
				RenderWndIntfaceHideMode();
				// ¸¶żě˝ş ·Ł´ő¸µ
				if(m_pTarget)
				{					
					POINT pt;
					GetCursorPos(&pt);
					ScreenToClient(g_pD3dApp->GetHwnd(), &pt);
					CheckMouseReverse(&pt);
					
					m_pTarget->RenderMouse(pt.x, pt.y ,m_pTarget->m_nMouseType);
				}
				// end 2008-06-20 by bhsohn EP3 żÉĽÇ°ü·Ă Ăł¸®
				// 2009. 11. 02 by ckPark ŔÎÇÇ´ĎĆĽ ÇĘµĺ ŔÎ˝şĹĎ˝ş ´řÁŻ ˝Ă˝şĹŰ
				if (IS_CITY_MAP_INDEX(g_pShuttleChild->m_myShuttleInfo.MapChannelIndex.MapIndex))//possible only in city and causes lag everywhere else - by Inet
				{
					if (m_pInfinity)
						m_pInfinity->RenderResult();
				}
				// end 2009. 11. 02 by ckPark ŔÎÇÇ´ĎĆĽ ÇĘµĺ ŔÎ˝şĹĎ˝ş ´řÁŻ ˝Ă˝şĹŰ
			}			
//#else
			else
			{				
				if( m_pTarget )
				{
					m_pTarget->RenderAttackMeObject();
					m_pTarget->RenderAttackMeMonster();
					m_pTarget->RenderAttackMePKEnemy();
					// 2006-11-09 by ispark, ¸ó˝şĹÍ°ˇ ´Ů¸Ą »ó´ë Ĺ¸°Ů ÇĄ˝Ă
					m_pTarget->RenderOtherTargetMonster();
				}
				
				// µĄąĚÁö şäľî
				if(g_pGameMain->m_pDamageView)
				{	
					g_pGameMain->m_pDamageView->Render();
				}
				m_pTarget->RenderAutoTargetDrow();
				if(g_pGameMain->m_pUnitNameInfo)
					g_pGameMain->m_pUnitNameInfo->Render();
				
				if(m_pToolTip)
					m_pToolTip->Render();
				
				// 2007-03-27 by bhsohn żÉŔúąö ¸đµĺ˝Ă ŔÎĹÍĆäŔĚ˝ş Ľű±č

				if(m_bShowInterface && g_pTutorial->IsTutorialMode() == FALSE && !g_pShuttleChild->IsObserverMode())			
				{				
					if(m_pGameMain && m_pGameMain->m_pUnitInfoBar)
					{
						m_pGameMain->m_pUnitInfoBar->Render();	
					}				
					if(m_pGameMain)	
						m_pGameMain->Render();
					
				}
				else
				{
					if( m_pGameMain )
					{
						//29-07-2021 by Inetpub for fix F hiding when party
						if (g_pShuttleChild->m_pClientParty->GetPartyInfo().PartyID != 0)
							m_pGameMain->m_pCommunity->GetParty()->RenderPartyFrame();
						// 2007-10-05 by bhsohn FĹ°·Î Ľű°Üµµ ˝şÇÇÄż ˝şĆ®¸µ ş¸ŔĚ°Ô şŻ°ć					
						m_pGameMain->RenderSpeakerMsg();
						// end 2007-10-05 by bhsohn FĹ°·Î Ľű°Üµµ ˝şÇÇÄż ˝şĆ®¸µ ş¸ŔĚ°Ô şŻ°ć					
						
						if(m_pGameMain->m_pUnitState)
						{
							m_pGameMain->m_pUnitState->Render();
						}
						if(m_pGameMain->m_pUnitInfoBar)
						{
							m_pGameMain->m_pUnitInfoBar->HideRender();
						}
						
						if(m_pGameMain->m_pChat && g_pTutorial->IsTutorialMode() == FALSE)
						{
							m_pGameMain->m_pChat->Render();
							//m_pGameMain->m_pChat->RenderWisperChat();
						}
						
						if(m_pGameMain->m_pGameCountDown)
						{
							m_pGameMain->m_pGameCountDown->Render();
						}
						
						if(g_pTutorial->IsTutorialMode() == TRUE)
						{
							if(m_pGameMain->m_pInfWindow )
							{
								m_pGameMain->m_pInfWindow->Render() ;
							}
							
							m_pGameMain->RenderLeftRightWindow();
						}
						// 2007-03-27 by bhsohn żÉŔúąö ¸đµĺ˝Ă ŔÎĹÍĆäŔĚ˝ş Ľű±č					
						if(g_pShuttleChild->IsObserverMode() && g_pD3dApp->m_dwGameState != _SHOP)
						{
							// °ÔŔĚÁö ÇĄ˝Ă
							m_pGameMain->Render_AltiMeter();
						}
						// end 2007-03-27 by bhsohn żÉŔúąö ¸đµĺ˝Ă ŔÎĹÍĆäŔĚ˝ş Ľű±č
					}
				}
				
				if(m_pGameMain && m_pGameMain->m_pQuickSlot && g_pTutorial->IsTutorialMode() == FALSE)
				{
					m_pGameMain->m_pQuickSlot->Render();
				}
#ifdef S_ENABLE_KILLFEED
				if(g_pGameMain && m_pGameMain->m_pINFEvoKillfeed)
				{
					m_pGameMain->m_pINFEvoKillfeed->Render();
				}
#endif
				//////////////////////////////////////////////////////////////////////////
				// żěĽ±ĽřŔ§ ·»´ő¸µ ĽłÁ¤
				WindowsRender();
				//
				//////////////////////////////////////////////////////////////////////////	
				// 2011-05-31 by jhahn ŔÎÇÇ´ĎĆĽ1Â÷ °á°úĂ˘ Ăâ·Â 
				if( m_pInfinity )
					m_pInfinity->RenderResult();
				// end 2011-05-31 by jhahn ŔÎÇÇ´ĎĆĽ1Â÷ °á°úĂ˘ Ăâ·Â 
				
				
			}
//#endif
		}
		break;
//	case _SHOP:
//		{
//			// ľĆŔĚµđ, Ă¤ĆĂ
//			if( m_pTarget )
//					m_pTarget->RenderAttackMeMonster();
//			if(g_pGameMain->m_pUnitNameInfo)
//				g_pGameMain->m_pUnitNameInfo->Render();
//			if(m_pGameMain->m_pUnitInfoBar)
//				m_pGameMain->m_pUnitInfoBar->Render();			
//			if(m_pGameMain->m_pQuickSlot)
//				m_pGameMain->m_pQuickSlot->Render();
//			if( m_pCityBase )
//				m_pCityBase->Render() ;
//			if( m_pGameMain )
//				m_pGameMain->Render() ;
//			if( m_pGameMain->m_pInfSkill )
//				m_pGameMain->m_pInfSkill->Render();			
//			// µµ˝Ă,°ÔŔÓ °řĹë
//			if(m_pGameMain->m_bShowHelp)
//			{
//				m_pGameMain->RenderHelp();				
//			}
//			// ąĚĽÇ ŔÎĆ÷
//			if(g_pGameMain->m_pMissionInfo)
//			{
//				g_pGameMain->m_pMissionInfo->Render();
//			}
//			// mp3 ÇĂ·ąŔĚľî
//			if(g_pGameMain->m_pMp3Player && g_pGameMain->m_bMp3PlayFlag == TRUE)
//			{
//				g_pGameMain->m_pMp3Player->Render();
//			}			
//			if(m_pGameMain->m_pInfGameMainFaq && m_pGameMain->m_bFAQProsFlag)
//				m_pGameMain->m_pInfGameMainFaq->Render();
//			// ľĆŔĚĹŰ ŔÎĆ÷
//			if(m_pGameMain->m_pItemInfo)
//				m_pGameMain->m_pItemInfo->Render();
//
//			if(m_pGameMain->m_pInfWindow)
//				m_pGameMain->m_pInfWindow->RenderMessaegBox();
//			// ´ÜĂŕĹ° °ü·Ă ·»´ő¸µ
//			if(m_pGameMain->m_bQSlotIconFlag == TRUE)
//			{
//				m_pGameMain->RenderQSlotIcon();
//			}			
//			// µĄąĚÁö şäľî
//			if(g_pGameMain->m_pDamageView)
//			{	
//				g_pGameMain->m_pDamageView->Render();
//			}
//			// ±Ű·Îąú ŔĚąĚÁö
//			if(m_pGameGlobalImage)
//				m_pGameGlobalImage->Render();
//			if( m_pTarget )
//			{
//				POINT pt;
//				GetCursorPos(&pt);
//				ScreenToClient(g_pD3dApp->GetHwnd(), &pt);
//				CheckMouseReverse(&pt);
//				
//				if(m_pTarget->m_bWisperChatBoxMouseFlag)
//				{
//					m_pTarget->RenderMouse(pt.x, pt.y, m_pTarget->m_nMouseType);
//				}
//				else
//				{
//					m_pTarget->RenderMouse(pt.x, pt.y, MOUSE_TYPE_0);
//				}
//			}
//		}
//		break;
	case _WAITING:
		{
		}
		break;
	case _CITY:
		{
			if(m_pMapLoad->GetRenderMapNameImg())
			{	// ¸ĘŔĚ¸§ ·»´ő¸µ				
#ifdef MAP_EDITOR
				CINFImage* pName = m_pMapNameImg->FindMapNameImage(g_pD3dApp->m_nMapNumber);

#else
				CINFImage* pName = m_pMapNameImg->FindMapNameImage(g_pShuttleChild->m_myShuttleInfo.MapChannelIndex.MapIndex);	
#endif
				if(pName)
				{
					pName->SetColor(m_pMapLoad->m_dwAlpha);
					pName->Move((g_pD3dApp->GetBackBufferDesc().Width/2)-152, RENDER_MAPNAME_POS_Y);
					pName->Render();
				}

				// PK°ˇ´É ¸ĘŔÎ°ˇ
				CINFImage* pName2 = NULL;
				if(g_pShuttleChild->m_pPkNormalTimer->IsCityWar() == TRUE)
				{
					pName2 = m_pMapNameImg->FindMapNameImage(5555);
				}
#ifdef MAP_EDITOR
				else if (IsPkEnableMap(g_pD3dApp->m_nMapNumber))
#else
				else if(IsPkEnableMap(g_pShuttleChild->m_myShuttleInfo.MapChannelIndex.MapIndex))
#endif
				{	// PK °ˇ´É ¸Ę
					pName2 = m_pMapNameImg->FindMapNameImage(1111);	
				}
				else
				{	// PK şŇ°ˇ´É ¸Ę
					pName2 = m_pMapNameImg->FindMapNameImage(9999);	
				}
				if(pName2)
				{
					pName2->SetColor(m_pMapLoad->m_dwAlpha);
					pName2->Move((g_pD3dApp->GetBackBufferDesc().Width/2)-152, RENDER_MAPNAME_POS_Y);
					pName2->Render();
				}
			}
			if(m_pCityBase)
			{
				m_pCityBase->RenderUpBox();
				m_pCityBase->RenderDownBox();
			}
			if(m_pGameMain->m_pUnitInfoBar)
				m_pGameMain->m_pUnitInfoBar->RenderCityUpInfo();			
			if(m_pGameMain->m_pQuickSlot)
				m_pGameMain->m_pQuickSlot->Render();
			if( m_pCityBase )
				m_pCityBase->Render() ;
			if( m_pGameMain )
				m_pGameMain->Render() ;
			if( m_pGameMain->m_pInfSkill )
				m_pGameMain->m_pInfSkill->Render();			
			// µµ˝Ă,°ÔŔÓ °řĹë
			if(m_pGameMain->m_bShowHelp)
			{
				m_pGameMain->RenderHelp();
				
			}
			// 2007-06-12 by bhsohn ąĚĽÇ ŔÎĹÍĆäŔĚ˝ş ĽöÁ¤ľČ
			// ąĚĽÇ ŔÎĆ÷
//			if(g_pGameMain->m_pMissionInfo)
//			{
//				g_pGameMain->m_pMissionInfo->Render();
//			}
			g_pGameMain->RenderMission();
			

			// mp3 ÇĂ·ąŔĚľî
			if(g_pGameMain->m_pMp3Player && g_pGameMain->m_bMp3PlayFlag == TRUE)
			{
				g_pGameMain->m_pMp3Player->Render();
			}			
			if(m_pGameMain->m_pInfGameMainFaq && m_pGameMain->m_bFAQProsFlag)
					m_pGameMain->m_pInfGameMainFaq->Render();
			if(m_pGameMain->m_pInfWindow)
				m_pGameMain->m_pInfWindow->RenderMessaegBox();
			// ´ÜĂŕĹ° °ü·Ă ·»´ő¸µ
			if(m_pGameMain->m_bQSlotIconFlag == TRUE)
			{
				m_pGameMain->RenderQSlotIcon();
			}
			// ľĆŔĚĹŰ ŔÎĆ÷
			// 2009-02-03 by bhsohn ŔĺÂř ľĆŔĚĹŰ şń±ł ĹřĆÁ
// 			if(m_pGameMain->m_pItemInfo)
// 				m_pGameMain->m_pItemInfo->Render();
			if(m_pGameMain)
			{
				m_pGameMain->RenderItemInfo();
			}
			// end 2009-02-03 by bhsohn ŔĺÂř ľĆŔĚĹŰ şń±ł ĹřĆÁ
			// ±Ű·Îąú ŔĚąĚÁö
			if(m_pGameGlobalImage)
				m_pGameGlobalImage->Render();
			if( m_pTarget )
			{
				POINT pt;
				GetCursorPos(&pt);
				ScreenToClient(g_pD3dApp->GetHwnd(), &pt);
				CheckMouseReverse(&pt);
				
				if(m_pTarget->m_bWisperChatBoxMouseFlag)
				{
					m_pTarget->RenderMouse(pt.x, pt.y, m_pTarget->m_nMouseType);
				}
				else
				{
					m_pTarget->RenderMouse(pt.x, pt.y, MOUSE_TYPE_0);
				}
			}
		}
		break;
	case _SCRIPT:
		{
			if( m_pStageEffectData )
			{
				m_pStageEffectData->Render();
			}
		}
		break;
	default:
		{
	}
	}
}

int CInterface::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	FLOG( "CInterface::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)" );
	
	// 2005-03-03 by jschoi
	// 2007-12-17 by bhsohn ľĆŔĚĹŰ Á¤ş¸°ˇ ´Ů ľČżŔ¸é °ÔŔÓ ´ë±â»óĹÂ·Î
	//if( uMsg != WM_MOUSEMOVE && g_pD3dApp->m_bRequestEnable == FALSE)
	if( uMsg != WM_MOUSEMOVE && g_pD3dApp->IsLockMode() == FALSE)
	{
		return 0;	// Ľ­ąöŔÇ ŔŔ´äŔ» ±â´Ů¸®´Â ÁßŔĚ¶ó¸é
	}
	/*
	

Starting in Windows 8, you can use the GetCurrentInputMessageSource() function in your message handler. You can check if the reported originId is IMO_HARDWARE, IMO_INJECTED, or IMO_SYSTEM.
*/
//	GetCurrentInputMessageSource()
	//LPKBDLLHOOKSTRUCT kbdCheck = (LPKBDLLHOOKSTRUCT)lParam;
	//if ((kbdCheck->flags & LLKHF_INJECTED) || (kbdCheck->flags & LLKHF_LOWER_IL_INJECTED)) // injected
	//{
	//	MSG_FC_CHARACTER_GET_REAL_WEAPON_INFO_OK msg_Weapon;
	//	msg_Weapon.ItemUID0 = 111;
	//	msg_Weapon.ReattackTime0 = 1;
	//	msg_Weapon.ShotNum0 = 0;
	//	msg_Weapon.MultiNum0 = 0;
	//	msg_Weapon.nFound = 150001;
	//	if(g_pD3dApp && g_pD3dApp->m_pFieldWinSocket)
	//	g_pD3dApp->m_pFieldWinSocket->SendMsg(T_FC_CHARACTER_GET_REAL_WEAPON_INFO_OK, reinterpret_cast<char*>(&msg_Weapon), sizeof(msg_Weapon));
	////	MessageBox(NULL, "Keyboard macro detected. \r\nCode for support(0xAC150001)", "Inetpub Anti - Cheat", MB_OK | MB_ICONERROR);
	//	FDBG("AntiCheat :: Triggered in skill check!");
	////	SendMessage(g_pD3dApp->GetHwnd(), WM_CLOSE, 0, 0);
	//	return 0;

	//}

	//LPMSLLHOOKSTRUCT detectMacro = (LPMSLLHOOKSTRUCT)lParam;
	//if ((detectMacro->flags & LLMHF_INJECTED) || (detectMacro->flags & LLMHF_LOWER_IL_INJECTED)) // injected
	//{
	//	MSG_FC_CHARACTER_GET_REAL_WEAPON_INFO_OK msg_Weapon;
	//	msg_Weapon.ItemUID0 = 222;
	//	msg_Weapon.ReattackTime0 = 2;
	//	msg_Weapon.ShotNum0 = 0;
	//	msg_Weapon.MultiNum0 = 0;
	//	msg_Weapon.nFound = 150002;
	//	if (g_pD3dApp && g_pD3dApp->m_pFieldWinSocket)
	//		g_pD3dApp->m_pFieldWinSocket->SendMsg(T_FC_CHARACTER_GET_REAL_WEAPON_INFO_OK, reinterpret_cast<char*>(&msg_Weapon), sizeof(msg_Weapon));
	//	//MessageBox(NULL, "Mouse macro detected. \r\nCode for support(0xAC150002)", "Inetpub Anti - Cheat", MB_OK | MB_ICONERROR);
	//	FDBG("AntiCheat :: Triggered in skill check!");
	//	//SendMessage(g_pD3dApp->GetHwnd(), WM_CLOSE, 0, 0);
	//	return 0;
	//}

	// 2007.04.24 by bhsohn China IME Working
#ifdef LANGUAGE_CHINA
	
	if(WM_INPUTLANGCHANGE == uMsg)
	{
		HKL hkl = (HKL)lParam;
		//char szDesc[256];
		//memset(szDesc, 0x00, 256);
		//UINT uRet = ImmGetDescription(hkl, szDesc, 256);
		//DBGOUT("WM_INPUTLANGCHANGE == uMsg [%s]\n", szDesc);

//		HKL hkl = (HKL)lParam;
//		LANGID langId = LOWORD(hkl);	
//		int nSubid = HIWORD(hkl);
//		BOOL bChinaFlag = FALSE;
//		if(LANG_CHINESE == PRIMARYLANGID(langId)) 
//		{			
//			HKL hMspy = g_pD3dApp->m_inputkey.GetMspyHKL();			
//			
//			//if(hMspy != NULL && NULL == pFind && 0x0804 != nSubid)
//			//if(hMspy != NULL && NULL == pFind && nDefaultSubId != nSubid)
//			// 2007.04.24 by bhsohn China IME Working
//			BOOL bPermission = IsPermissionIME(hkl);
//			if(hMspy != NULL && FALSE == bPermission)			
//			{
//				//lParam = (LPARAM)m_hkl[m_nChinaMSPYIdx];
//				ActivateKeyboardLayout(hMspy, KLF_REORDER);
//				//bChinaFlag = TRUE;
//			}
//			else
//			{
//				g_pD3dApp->m_inputkey.UpdateChinaToggle(1);			
//			}
//		}
//		g_input.CheckToggleState(g_pD3dApp->GetHwnd());
	}			
#endif
	// end by bhsohn 2007-03-12 China Working
	
	switch(g_pD3dApp->m_dwGameState)
	{
	case _SELECT:
	case _CREATE:
	case _OPTION:
		{
			if(	m_pSelect && m_pSelect->WndProc(uMsg, wParam, lParam)==INF_MSGPROC_BREAK)
				return INF_MSGPROC_BREAK;
		}
		break;
	case _MAPLOAD:
		{
		}
		break;
	case _SELECTCHANNEL:
		{
			if(	m_pChannelList && m_pChannelList->WndProc(uMsg, wParam, lParam)==INF_MSGPROC_BREAK)
				return INF_MSGPROC_BREAK;
		}
		break;
	case _GAME:
		{
			// 2007-07-04 by bhsohn żŔĆŰ·ąŔĚĹÍ Ăß°ˇ
			if(	WndProcOperator(uMsg, wParam, lParam) == INF_MSGPROC_BREAK)
			{
				return INF_MSGPROC_BREAK;
			}			
			// end 2007-07-04 by bhsohn żŔĆŰ·ąŔĚĹÍ Ăß°ˇ

			if(	WindowsWndProc(uMsg, wParam, lParam) == INF_MSGPROC_BREAK)
					return INF_MSGPROC_BREAK;
		}
		break;
	case _SHOP:
		{
			if(	WindowsWndProc(uMsg, wParam, lParam) == INF_MSGPROC_BREAK)
					return INF_MSGPROC_BREAK;
			// Ăâ°ÝÇĎ±â ą× ąĚĽÇ¸ĘŔĚµż
			// 2007-03-27 by bhsohn żÉŔúąö ¸đµĺ˝Ă ŔÎĹÍĆäŔĚ˝ş Ľű±č
			// 2007-06-12 by bhsohn ąĚĽÇ ŔÎĹÍĆäŔĚ˝ş ĽöÁ¤ľČ
			//if(g_pGameMain->m_pMissionInfo && m_bShowInterface && 
			//if(g_pGameMain->m_pMissionInfo && m_bShowInterface && !g_pShuttleChild->IsObserverMode() &&
			if(m_bShowInterface && !g_pShuttleChild->IsObserverMode() &&
				//g_pGameMain->m_pMissionInfo->WndProc(uMsg, wParam, lParam)==INF_MSGPROC_BREAK)				
				g_pGameMain->WndProcMiss(uMsg, wParam, lParam)==INF_MSGPROC_BREAK)
					return INF_MSGPROC_BREAK;
		}
		break;
	case _CITY:
		{
			// ±Ű·Îąú ŔĚąĚÁö
			if(m_pGameGlobalImage->WndProc(uMsg, wParam, lParam)==INF_MSGPROC_BREAK)
					return INF_MSGPROC_BREAK;
			if(	m_pGameMain && m_pGameMain->WndProc(uMsg, wParam, lParam)==INF_MSGPROC_BREAK)
					return INF_MSGPROC_BREAK;
			if(	m_pCityBase && m_pCityBase->WndProc(uMsg, wParam, lParam)==INF_MSGPROC_BREAK)
					return INF_MSGPROC_BREAK;
			// Ăâ°ÝÇĎ±â ą× ąĚĽÇ¸ĘŔĚµż
			// 2007-03-27 by bhsohn żÉŔúąö ¸đµĺ˝Ă ŔÎĹÍĆäŔĚ˝ş Ľű±č
			// 2007-06-12 by bhsohn ąĚĽÇ ŔÎĹÍĆäŔĚ˝ş ĽöÁ¤ľČ
			//if(g_pGameMain->m_pMissionInfo && m_bShowInterface && 
			//if(g_pGameMain->m_pMissionInfo && m_bShowInterface && !g_pShuttleChild->IsObserverMode() &&
			if(m_bShowInterface && !g_pShuttleChild->IsObserverMode() &&
				//g_pGameMain->m_pMissionInfo->WndProc(uMsg, wParam, lParam)==INF_MSGPROC_BREAK)
				g_pGameMain->WndProcMiss(uMsg, wParam, lParam)==INF_MSGPROC_BREAK)
					return INF_MSGPROC_BREAK;
		}
		break;
	case _SCRIPT:
		{
			if( m_pStageEffectData )
			{
				if( m_pStageEffectData->WndProc(uMsg, wParam, lParam)==INF_MSGPROC_BREAK)
					return INF_MSGPROC_BREAK;
			}
		}
		break;
	default:
		{
		}
	}
#ifdef _DEBUG_MAPSETTING
	if(uMsg == WM_KEYDOWN )
	{
		BYTE nRed, nGreen, nBlue;
		nRed = (BYTE)(g_dwDebugFogColor >> 16);
		nGreen = (BYTE)(g_dwDebugFogColor >> 8);
		nBlue = (BYTE)(g_dwDebugFogColor);
		switch(wParam)
		{
		case '9':
			{
				if(GetAsyncKeyState(VK_SHIFT))
				{
					nRed += 1;
					if(nRed>255)
						nRed = 255;
				}
				else
				{
					nRed -= 1;
					if(nRed<0)
						nRed = 0;
				}
				g_dwDebugFogColor = D3DCOLOR_ARGB(0,nRed,nGreen,nBlue);
			}
			break;
		case '0':
			{
				if(GetAsyncKeyState(VK_SHIFT))
				{
					nGreen += 1;
					if(nGreen>255)
						nGreen = 255;
				}
				else
				{
					nGreen -= 1;
					if(nGreen<0)
						nGreen = 0;
				}
				g_dwDebugFogColor = D3DCOLOR_ARGB(0,nRed,nGreen,nBlue);
			}
			break;
		case 189: // '-'
			{
				if(GetAsyncKeyState(VK_SHIFT))
				{
					nBlue += 1;
					if(nBlue>255)
						nBlue = 255;
				}
				else
				{
					nBlue -= 1;
					if(nBlue<0)
						nBlue = 0;
				}
				g_dwDebugFogColor = D3DCOLOR_ARGB(0,nRed,nGreen,nBlue);
			}
			break;
		case 187: // '='
			{
				if(GetAsyncKeyState(VK_SHIFT))
				{
					g_pDebugFogStartDistance += 10;
				}
				else
				{
					g_pDebugFogStartDistance -= 10;
					if(g_pDebugFogStartDistance==0)
						g_pDebugFogStartDistance = 0;
				}
			}
			break;
		case 220: // '\'
			{
				if(GetAsyncKeyState(VK_SHIFT))
				{
					g_pDebugFogEndDistance += 10;
				}
				else
				{
					g_pDebugFogEndDistance -= 10;
					if(g_pDebugFogEndDistance==0)
						g_pDebugFogEndDistance = 0;
				}
			}
			break;
		case VK_PRIOR:
			{
				g_vDebugLightDirection.y -= 0.05f;
				g_pScene->m_light0.Direction = g_pScene->SetLightDirection();
				g_pD3dDev->SetLight( 0, &g_pScene->m_light0 );
			}
			break;
		case VK_NEXT:
			{
				g_vDebugLightDirection.y += 0.05f;
				g_pScene->m_light0.Direction = g_pScene->SetLightDirection();
				g_pD3dDev->SetLight( 0, &g_pScene->m_light0 );
			}
			break;
		case 'J':
			{
				if(GetAsyncKeyState(VK_SHIFT))
				{
					g_pScene->m_light0.Diffuse.r += 0.01f;
				}
				else
				{
					g_pScene->m_light0.Diffuse.r -= 0.01f;
					if(g_pScene->m_light0.Diffuse.r==0)
						g_pScene->m_light0.Diffuse.r = 0;
				}
			}
			break;
		case 'K':
			{
				if(GetAsyncKeyState(VK_SHIFT))
				{
					g_pScene->m_light0.Diffuse.g += 0.01f;
				}
				else
				{
					g_pScene->m_light0.Diffuse.g -= 0.01f;
					if(g_pScene->m_light0.Diffuse.g==0)
						g_pScene->m_light0.Diffuse.g = 0;
				}
			}
			break;
		case 'L':
			{
				if(GetAsyncKeyState(VK_SHIFT))
				{
					g_pScene->m_light0.Diffuse.b += 0.01f;
				}
				else
				{
					g_pScene->m_light0.Diffuse.b -= 0.01f;
					if(g_pScene->m_light0.Diffuse.b==0)
						g_pScene->m_light0.Diffuse.b = 0;
				}
			}
			break;
		case 'U':
			{
				if(GetAsyncKeyState(VK_SHIFT))
				{
					g_pScene->m_light0.Ambient.r += 0.01f;
				}
				else
				{
					g_pScene->m_light0.Ambient.r -= 0.01f;
					if(g_pScene->m_light0.Ambient.r==0)
						g_pScene->m_light0.Ambient.r = 0;
				}
			}
			break;
		case 'I':
			{
				if(GetAsyncKeyState(VK_SHIFT))
				{
					g_pScene->m_light0.Ambient.g += 0.01f;
				}
				else
				{
					g_pScene->m_light0.Ambient.g -= 0.01f;
					if(g_pScene->m_light0.Ambient.g==0)
						g_pScene->m_light0.Ambient.g = 0;
				}
			}
			break;
		case 'O':
			{
				if(GetAsyncKeyState(VK_SHIFT))
				{
					g_pScene->m_light0.Ambient.b += 0.01f;
				}
				else
				{
					g_pScene->m_light0.Ambient.b -= 0.01f;
					if(g_pScene->m_light0.Ambient.b==0)
						g_pScene->m_light0.Ambient.b = 0;
				}
			}
			break;
		}
		if(g_pScene->m_pGround)
		{
			sprintf( g_pD3dApp->m_strDebug, "F[%d, %d, %d] S:%.0f,E:%.0f,L:%.2f, A:%.2f(%.2f),%.2f(%.2f),%.2f(%.2f), D:%.2f(%.2f),%.2f(%.2f),%.2f(%.2f)", 
				nRed, nGreen, nBlue, g_pDebugFogStartDistance,g_pDebugFogEndDistance, g_vDebugLightDirection.y,
				g_pScene->m_light0.Ambient.r, g_pScene->m_pGround->m_projectInfo.fAmbientR1, 
				g_pScene->m_light0.Ambient.g, g_pScene->m_pGround->m_projectInfo.fAmbientG1, 
				g_pScene->m_light0.Ambient.b, g_pScene->m_pGround->m_projectInfo.fAmbientB1, 
				g_pScene->m_light0.Diffuse.r, g_pScene->m_pGround->m_projectInfo.fDiffuseR1, 
				g_pScene->m_light0.Diffuse.g, g_pScene->m_pGround->m_projectInfo.fDiffuseG1, 
				g_pScene->m_light0.Diffuse.b, g_pScene->m_pGround->m_projectInfo.fDiffuseB1 );
			::SetFogLevel(0,TRUE);
		}
//		g_pScene->m_dwFogColor = D3DCOLOR_ARGB( 0x00,bRed, bGreen, bBlue );
	}
#endif // _DEBUG_endif
	////////////////////////////////////////////////////////////////////////////////////////////
	return INF_MSGPROC_NORMAL;

}
/*
void CInterface::OnButtonClick(int nButton)
{
	FLOG( "CInterface::OnButtonClick(int nButton)" );
	if(m_pGameMain)
		m_pGameMain->OnButtonClick(nButton);

}
*/

void CInterface::InitOptionData()
{
	FLOG( "CInterface::InitOptionData()" );
	g_pSOption->sSoundVolume = OPTION_DEFAULT_SOUNDVOLUME;
	g_pSOption->sMusicVolume = OPTION_DEFAULT_MUSICVOLUME;
	g_pSOption->sGammaCtrl = OPTION_DEFAULT_GAMMA;
	g_pSOption->sLowQuality	= OPTION_DEFAULT_LOWQUALITY;
	g_pSOption->sMouseReversLeft = OPTION_DEFAULT_REVERSE_LEFT;			// ¸¶żě˝ş ÁÂżěąÝŔü
	g_pSOption->sMouseReversUp = OPTION_DEFAULT_REVERSE_UP;				// ¸¶żě˝ş »óÇĎąÝŔü
	g_pSOption->sAutoBalance = OPTION_DEFAULT_AUTO_BALACE;				// ŔÚµż ĽöĆňÁ¶Á¤
	g_pSOption->sMenuBalance = OPTION_DEFAULT_MENU_USE;					// ¸Ţ´ş»çżë˝Ă ŔÚµż ĽöĆňÁ¶Á¤
	// 2010. 10. 26 by jskim Ćę żŔĆŰ·ąŔĚĹÍ żÉĽÇ Ăł¸®
	g_pSOption->sPetOPBalance = OPTION_DEFAULT_PET_OP;					// Ćę żŔĆŰ·ąŔĚĹÍ Ŕűżë
	// end 2010. 10. 26 Ćę żŔĆŰ·ąŔĚĹÍ żÉĽÇ Ăł¸®
	g_pSOption->sMp3Player = OPTION_DEFAULT_MP3_USE;					// ąÂÁ÷ÇĂ·ąŔĚľî
#ifdef _INET_ADDITIONAL_HUD
	g_pSOption->sINETAddAIMInfo = OPTION_DEFAULT_SHOW_ADD_HUD;
	g_pSOption->sINETComboTransp = 5;
#endif
#ifdef _INET_SHOW_GUILD_NAME
	g_pSOption->sINETAddAIMInfo = OPTION_DEFAULT_SHOW_GUILDNAME;
#endif
#ifdef _INET_RANKS
	g_pSOption->bShowRanks = OPTION_DEFAULT_SHOW_RANKS;
#endif
#ifdef _INET_DRANKS
	g_pSOption->bShowDonatorRanks = OPTION_DEFAULT_SHOW_DRANKS;
#endif
#ifdef _INET_DEFAULT_COLOR
	g_pSOption->sINETDefaultColor = 15;
#endif
#ifdef _INET_FOV
	g_pSOption->sFOVangle = OPTION_DEFAULT_FOV;

	g_pSOption->sScalePerc = 100;
#endif
	g_pSOption->nINET_Aim_Type = 0;
	g_pSOption->bChatTimestamp = OPTION_DEFAULT_SHOW_CHAT_TIME;

	g_pSOption->bKillFeed = OPTION_DEFAULT_SHOW_KILLFEED;
	g_pSOption->bMonQuest = OPTION_DEFAULT_SHOW_MONQUEST;
	g_pSOption->bItemLink = OPTION_DEFAULT_SHOW_ITEMLINK;
	g_pSOption->bPing = OPTION_DEFAULT_SHOW_PING;
	g_pSOption->bMMAuto = OPTION_DEFAULT_SHOW_MMAUTO;
	g_pSOption->bExtTarg = OPTION_DEFAULT_SHOW_EXTTARGET;
	g_pSOption->bDisableChatLock = 0;
	
	
	g_pSOption->sHandle = TRUE ;										// ¸¶żě˝ş Ć÷ŔÎĆ® ş¸ŔĚ±â
	switch(g_pD3dApp->m_bDegree)
	{
	case 2:// HIGH
		{
			g_pSOption->sTerrainEffectRender = OPTION_DEFAULT_HIGH_TERRAINEFFECTRENDER;
			g_pSOption->sTerrainRender = OPTION_DEFAULT_HIGH_TERRAINRENDER;
			g_pSOption->sShadowState = OPTION_DEFAULT_HIGH_SHOWSHADOW;
			g_pSOption->sUnitDetail = OPTION_DEFAULT_HIGH_UNITDETAIL;
			g_pSOption->sContrast = OPTION_DEFAULT_HIGH_CONTRAST;
			g_pSOption->sFilter = OPTION_DEFAULT_HIGH_FILTER;
		}
		break;
	case 1:// Middle
		{
			g_pSOption->sTerrainEffectRender = OPTION_DEFAULT_MIDDLE_TERRAINEFFECTRENDER;
			g_pSOption->sTerrainRender = OPTION_DEFAULT_MIDDLE_TERRAINRENDER;
			g_pSOption->sShadowState = OPTION_DEFAULT_MIDDLE_SHOWSHADOW;
			g_pSOption->sUnitDetail = OPTION_DEFAULT_MIDDLE_UNITDETAIL;
			g_pSOption->sContrast = OPTION_DEFAULT_MIDDLE_CONTRAST;
			g_pSOption->sFilter = OPTION_DEFAULT_MIDDLE_FILTER;
		}
		break;
	case 0:// LOW
		{
			g_pSOption->sTerrainEffectRender = OPTION_DEFAULT_LOW_TERRAINEFFECTRENDER;
			g_pSOption->sTerrainRender = OPTION_DEFAULT_LOW_TERRAINRENDER;
			g_pSOption->sShadowState = OPTION_DEFAULT_LOW_SHOWSHADOW;
			g_pSOption->sUnitDetail = OPTION_DEFAULT_LOW_UNITDETAIL;
			g_pSOption->sContrast = OPTION_DEFAULT_LOW_CONTRAST;
			g_pSOption->sFilter = OPTION_DEFAULT_LOW_FILTER;
		}
		break;
	}

	g_pSOption->sEyePt = FALSE;
	g_pSOption->sTarget = TRUE ;
	g_pSOption->sHeight = FALSE ;
	g_pSOption->sPKMode = 0 ;
	g_pSOption->InputType = 0 ;	

	// Ă¤ĆĂżÉĽÇ ŔúŔĺ
	for(int i = 0; i < 12 /*CHAT_MENULIST_NUMBER*/ ; i++)
	{
		g_pSOption->m_bChatCurrentSelect[i] = TRUE;
	}
}

void CInterface::InitCharacterData()
{
	FLOG( "CInterface::InitCharacterData()" );
	g_pSOptionCharacter->sHelpDesk = TRUE;
	memset(g_pSOptionCharacter->UniqueNumber, 0x00, QSLOT_NUMBER*QSLOT_TAB_NUMBER*sizeof(UID64_t));
	memset(g_pSOptionCharacter->ItemNum, 0x00, QSLOT_NUMBER*QSLOT_TAB_NUMBER*sizeof(int));
}
BOOL CInterface::LoadOptionFile()
{
	// 2007-07-23 by bhsohn żŔĆŰ·ąŔĚĹÍ Ăß°ˇŔŰľ÷
	{
		char chMaxPath[MAX_PATH];
		memset(chMaxPath, 0x00, MAX_PATH);
		wsprintf(chMaxPath, SETUP_INFO_PATH);
		if(FALSE == m_pSetupConfig->LoadSetupInfo(chMaxPath))
		{
			// ĂĘ±âČ­
			InitSetupInfo(chMaxPath);
		}
	}	
	// end 2007-07-23 by bhsohn żŔĆŰ·ąŔĚĹÍ Ăß°ˇŔŰľ÷
	// 2008-06-20 by bhsohn EP3 żÉĽÇ°ü·Ă Ăł¸®
	LoadOptionEtcInfo(g_pSOptionEtc);
	// end 2008-06-20 by bhsohn EP3 żÉĽÇ°ü·Ă Ăł¸®

	// 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
	LoadJoysticOptionInfo(g_pJoysticOp);
	
	g_pD3dApp->SetUseJoyStick(g_pJoysticOp->bUseJoystick);	// Á¶ŔĚ˝şĆ˝ »çżëż©şÎ ĽłÁ¤
	g_pD3dApp->SetUseJoyFeedBack(g_pJoysticOp->bUseFeedJoyStick); // Á¶ŔĚ˝şĆ˝ Ářµż »çżëŔŻą«
	// end 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
	
	FLOG( "CInterface::LoadOptionFile()" );
	FILE *	fp;
	int nResult;
	char strFileName[32];
	strcpy(strFileName, "option.sys");
	fp = fopen(strFileName, "rb");
	if(!fp)
	{
		DBGOUT("CINFInterface::LoadOptionFile() Option File(%s) Open(rb) Error, Set Default\n",strFileName);
		InitOptionData();
		SaveOptionFile();
		return FALSE;
	}
	nResult = fseek(fp, 0L, SEEK_END);
	if( nResult != 0)
	{
		DBGOUT("CINFInterface::LoadOptionFile() Option File(%s) fseek(SEEK_END) Error, Set Default\n", strFileName);
		fclose(fp);
		InitOptionData();
		SaveOptionFile();
		return FALSE;
	}
	nResult = ftell(fp);
	// 050202 ĂĘ±â°Ş ŔűżëŔ» Ŕ§ÇŘ ¸®ĽÂ
	if(nResult != sizeof(sOPTION_SYSTEM)/* - (QSLOT_NUMBER*QSLOT_TAB_NUMBER*(sizeof(int)+sizeof(UID64_t)))*/)
	{
		DBGOUT("CINFInterface::LoadOptionFile() Option File(%s) SIZE error, Set Default\n", strFileName);
		fclose(fp);
		InitOptionData();
		SaveOptionFile();
		return FALSE;
	}
	nResult = fseek(fp, 0L, SEEK_SET);
	if( nResult != 0)
	{
		DBGOUT("CINFInterface::LoadOptionFile() Option File(%s) fseek(SEEK_SET) error, Set Default\n", strFileName);
		fclose(fp);
		InitOptionData();
		SaveOptionFile();
		return FALSE;
	}
	
	nResult = fread(g_pSOption, sizeof(sOPTION_SYSTEM)/* - QSLOT_NUMBER*QSLOT_TAB_NUMBER*(sizeof(int)+sizeof(UID64_t))*/, 1, fp);
	if(nResult != 1)
	{
		DBGOUT("CINFInterface::LoadOptionFile() File Read Error(%s), Set Default\n", strFileName);
		fclose(fp);
		InitOptionData();
		SaveOptionFile();
		return FALSE;
	}
	fclose(fp);
#ifdef _INET_FOV
	if (g_pSOption->sFOVangle >= _INET_FOV_MAX)
		g_pSOption->sFOVangle = _INET_FOV_MAX;

	if (g_pSOption->sFOVangle <= _INET_FOV_MIN)
		g_pSOption->sFOVangle = _INET_FOV_MIN;

	if (g_pSOption->sScalePerc >= 100)
		g_pSOption->sScalePerc = 100;

	if (g_pSOption->sScalePerc <= 10)
		g_pSOption->sScalePerc = 10;
#endif
	if(g_pSOption->sSoundVolume > 0) 
		g_pSOption->sSoundVolume = 0;
	if(g_pSOption->sMusicVolume > 100)				// 2006-03-15 by ispark, Ŕ˝ľÇ şĽ·ý°Ş ŔúŔĺ ĽöÄˇ şŻ°ć
		g_pSOption->sMusicVolume = 100;

	if(g_pSOption->sSoundVolume < -10000)
		g_pSOption->sSoundVolume = -10000;
	if(g_pSOption->sMusicVolume < 0)				// 2006-03-15 by ispark, Ŕ˝ľÇ şĽ·ý°Ş ŔúŔĺ ĽöÄˇ şŻ°ć
		g_pSOption->sMusicVolume = 0;	

	// 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
	// ¸¶żě˝ş ÁÂżě ąÝŔü Ç×»ó ˛ô±â·Î
	g_pSOption->sMouseReversLeft = OPTION_DEFAULT_REVERSE_LEFT;
	// end 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
	
	g_pD3dApp->m_pCamera->SetCamType( g_pSOption->sEyePt );
#ifndef MAP_EDITOR
	g_pD3dApp->m_pShuttleChild->m_bAutoLockon = g_pSOption->sTarget;
	g_pD3dApp->m_pShuttleChild->m_bAutoHeightControl = g_pSOption->sHeight;
	g_pD3dApp->m_pShuttleChild->SetShuttleInputType( g_pSOption->InputType);
#endif
	return TRUE;
}

// 2007-07-23 by bhsohn żŔĆŰ·ąŔĚĹÍ Ăß°ˇŔŰľ÷
void CInterface::InitSetupInfo(char* pPath)
{
	// ĆÄŔĎŔĚ ľř´Ů
	char chBuff[512];
	wsprintf(chBuff, "%d", TRUE);
	m_pSetupConfig->AddSetupInfo(SETUP_INFO_OPERATOR_INFO, chBuff);
	m_pSetupConfig->AddSetupInfo(SETUP_INFO_SPEAKER_INFO, chBuff);
	
	// 2008-06-20 by bhsohn EP3 żÉĽÇ°ü·Ă Ăł¸®	
	sOPTION_ETC	stEtcOption;
	memset(&stEtcOption, 0x00, sizeof(sOPTION_ETC));
	g_pD3dApp->InitOptionEtcInfo(&stEtcOption);
	{
		// 1:1ąřĹő
		wsprintf(chBuff, "%d", stEtcOption.bRadioInfo[OPTION_RADIO_MAN_TO_MAN]);
		m_pSetupConfig->AddSetupInfo(SETUP_INFO_OPETC_MANTOMAN_INFO, chBuff);

		// Ćí´ë ŔüĹő
		wsprintf(chBuff, "%d", stEtcOption.bRadioInfo[OPTION_RADIO_FORMATIONWAR]);
		m_pSetupConfig->AddSetupInfo(SETUP_INFO_OPETC_PARTYWAR_INFO, chBuff);

		// ż©´ÜŔüĹő
		wsprintf(chBuff, "%d", stEtcOption.bRadioInfo[OPTION_RADIO_GUILD_WAR]);
		m_pSetupConfig->AddSetupInfo(SETUP_INFO_OPETC_GUILDWAR_INFO, chBuff);

		//±ÓĽÓ¸»
		wsprintf(chBuff, "%d", stEtcOption.bRadioInfo[OPTION_RADIO_WHISPER]);
		m_pSetupConfig->AddSetupInfo(SETUP_INFO_OPETC_WHISPER_INFO, chBuff);

		// Ćí´ë ĂĘ´ë
		wsprintf(chBuff, "%d", stEtcOption.bRadioInfo[OPTION_RADIO_FORMATION_INVITE]);
		m_pSetupConfig->AddSetupInfo(SETUP_INFO_OPETC_PARTYINVI_INFO, chBuff);

		// ż©´ÜĂĘ´ë
		wsprintf(chBuff, "%d", stEtcOption.bRadioInfo[OPTION_RADIO_GUILD_INVITE]);
		m_pSetupConfig->AddSetupInfo(SETUP_INFO_OPETC_GUILDINVI_INFO, chBuff);

		// ±łČŻ
		wsprintf(chBuff, "%d", stEtcOption.bRadioInfo[OPTION_RADIO_EXCHANGE]);
		m_pSetupConfig->AddSetupInfo(SETUP_INFO_OPETC_EXCHANGE_INFO, chBuff);

		//ŔÎĹÍĆäŔĚ˝ş Ŕ§ÄˇŔúŔĺ
		wsprintf(chBuff, "%d", stEtcOption.bRadioInfo[OPTION_RADIO_INTERFACE_POS]);
		m_pSetupConfig->AddSetupInfo(SETUP_INFO_OPETC_INTERPOS_INFO, chBuff);

		//ŔÎĹÍĆäŔĚ˝ş Ŕ§ÄˇŔúŔĺ °Ş
		sprintf(chBuff, "%.2f", stEtcOption.stRateRect.fRateX);
		m_pSetupConfig->AddSetupInfo(SETUP_INFO_OPETC_INTERPOS_X_INFO, chBuff);		
		sprintf(chBuff, "%.2f", stEtcOption.stRateRect.fRateY);
		m_pSetupConfig->AddSetupInfo(SETUP_INFO_OPETC_INTERPOS_Y_INFO, chBuff);		

		sprintf(chBuff, "%.2f", stEtcOption.stRateRect.fRateWidth);
		m_pSetupConfig->AddSetupInfo(SETUP_INFO_OPETC_INTERPOS_W_INFO, chBuff);		
		sprintf(chBuff, "%.2f", stEtcOption.stRateRect.fRateHeight);
		m_pSetupConfig->AddSetupInfo(SETUP_INFO_OPETC_INTERPOS_H_INFO, chBuff);

		
		// 2009. 02. 03 by ckPark Ă¤ĆĂĂ˘ ŔÎĹÍĆäŔĚ˝ş Ĺ©±â ŔúŔĺ

		sprintf(chBuff, "%.2f", stEtcOption.stChatRect.fRateWidth);
		m_pSetupConfig->AddSetupInfo(SETUP_INFO_OPETC_CHATPOS_W_INFO, chBuff);
		sprintf(chBuff, "%.2f", stEtcOption.stChatRect.fRateHeight);
		m_pSetupConfig->AddSetupInfo(SETUP_INFO_OPETC_CHATPOS_H_INFO, chBuff);

		// end 2009. 02. 03 by ckPark Ă¤ĆĂĂ˘ ŔÎĹÍĆäŔĚ˝ş Ĺ©±â ŔúŔĺ

		
		sprintf(chBuff, "%.2f", stEtcOption.stRateRect.fGameWidth);
		m_pSetupConfig->AddSetupInfo(SETUP_INFO_OPETC_GAME_W_INFO, chBuff);		
		sprintf(chBuff, "%.2f", stEtcOption.stRateRect.fGameHeight);
		m_pSetupConfig->AddSetupInfo(SETUP_INFO_OPETC_GAME_H_INFO, chBuff);		

		// ŔÎĹÍĆäŔĚ˝ş ¸đµÎ °¨Ăß±â
		wsprintf(chBuff, "%d", stEtcOption.bRadioInfo[OPTION_RADIO_INTERFACE_HIDE]);
		m_pSetupConfig->AddSetupInfo(SETUP_INFO_OPETC_INTERHIDE_INFO, chBuff);

		// °°Ŕş ĽĽ·Â ľĆŔĚµđ
		wsprintf(chBuff, "%d", stEtcOption.bRadioInfo[OPTION_RADIO_FRIEND_ID_HIDE]);
		m_pSetupConfig->AddSetupInfo(SETUP_INFO_OPETC_SAME_ID_HIDE_INFO, chBuff);

		// ´Ů¸Ą ĽĽ·Â ľĆŔĚµđ
		wsprintf(chBuff, "%d", stEtcOption.bRadioInfo[OPTION_RADIO_ENEMY_ID_HIDE]);
		m_pSetupConfig->AddSetupInfo(SETUP_INFO_OPETC_ENEMY_ID_HIDE_INFO, chBuff);

		// ¸¶żě˝ş °ˇµÎ±â
		wsprintf(chBuff, "%d", stEtcOption.bRadioInfo[OPTION_RADIO_MOUSE_LOCK]);
		m_pSetupConfig->AddSetupInfo(SETUP_INFO_OPETC_MOUSE_LOCK_INFO, chBuff);

		// şńĽÓľî ÇĘĹÍ¸µ		
		wsprintf(chBuff, "%d", stEtcOption.bRadioInfo[OPTION_RADIO_CHAT_FILTER]);
		m_pSetupConfig->AddSetupInfo(SETUP_INFO_OPETC_FILTER_INFO, chBuff);			
	}	
	// 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
	{	

		structJoyStickOptionInfo	stJoysticOp;
		memset(&stJoysticOp, 0x00, sizeof(structJoyStickOptionInfo));
		g_pD3dApp->InitJoyStickOptionInfo(&stJoysticOp);
		// Á¶ŔĚ˝şĆ˝ »çżë		
		{			
			wsprintf(chBuff, "%d", stJoysticOp.bUseJoystick);
			m_pSetupConfig->AddSetupInfo(SETUP_INFO_JOYSTICK_USE, chBuff);
		}
		
		// Á¶ŔĚ˝şĆ˝ Ľ±ĹĂ		
		{				
			m_pSetupConfig->AddSetupInfo(SETUP_INFO_JOYSTICK_SELECT, stJoysticOp.chJoySelJoy);
		}
		
		// Á¶ŔĚ˝şĆ˝ Ářµż»çżë		
		{			
			wsprintf(chBuff, "%d", stJoysticOp.bUseFeedJoyStick);
			m_pSetupConfig->AddSetupInfo(SETUP_INFO_JOYSTICK_FEED, chBuff);
		}
		
		// Á¶ŔĚ˝şĆ˝ °¨µµ		
		{			
			wsprintf(chBuff, "%d", stJoysticOp.nJoysticSense);
			m_pSetupConfig->AddSetupInfo(SETUP_INFO_JOYSTICK_SENSE, chBuff);
		}
		
		// Ĺ° ĽłÁ¤ °ˇÁö°í żŔ±â		
		{				
			m_pSetupConfig->AddSetupInfo(SETUP_INFO_JOYSTICK_KEYCONFIG, stJoysticOp.chJoyKeyFile);
		}		
	}
	// end 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷

	m_pSetupConfig->SaveSetupInfo(pPath);
}
// 2007-08-07 by bhsohn ˝şÇÇÄż ľĆŔĚĹŰ Ăß°ˇ
int CInterface::GetSpeakerMode()
{
	char chBuf[512];		
	if(m_pSetupConfig->GetSetupInfo(SETUP_INFO_SPEAKER_INFO, chBuf))
	{
		return atoi(chBuf);	
	}
	// ľřŔ»¶§´Â ŔúŔĺ ˝Ăµµ
	SetSpeakerMode(1);
	// ŔúŔĺ
	{
		char chMaxPath[MAX_PATH];
		memset(chMaxPath, 0x00, MAX_PATH);
		wsprintf(chMaxPath, SETUP_INFO_PATH);
		m_pSetupConfig->SaveSetupInfo(chMaxPath);		
	}
	return 1;	
}
void CInterface::SetSpeakerMode(int nMode)
{	
	if(nMode != 0 && nMode !=1 )
	{
		return;
	}
	char chBuf[512];
	wsprintf(chBuf, "%d", nMode);
	m_pSetupConfig->SetSetupInfo(SETUP_INFO_SPEAKER_INFO, chBuf);
}


int CInterface::GetOperatorMode()
{	
	char chBuf[512];		
	if(m_pSetupConfig->GetSetupInfo(SETUP_INFO_OPERATOR_INFO, chBuf))
	{
		return atoi(chBuf);	
	}
	return FALSE;	
}
void CInterface::SetOperatorMode(int nMode)
{	
	if(nMode != 0 && nMode !=1 )
	{
		return;
	}
	char chBuf[512];
	wsprintf(chBuf, "%d", nMode);
	m_pSetupConfig->SetSetupInfo(SETUP_INFO_OPERATOR_INFO, chBuf);
}

// 2007-11-22 by bhsohn ľĆ·ąłŞ ĹëÇŐĽ­ąö
BOOL CInterface::LoadCharacterFile(BOOL bArenaRestart)
{
	FLOG( "CInterface::LoadCharacterFile()" );
	//*--------------------------------------------------------------------------*//
	// 2006-09-05 by ispark, 
	// Äü˝˝·Ô Á¤ş¸ Ľ­ąöżˇ·Î şÎĹÍ żäĂ»
	if(!bArenaRestart)
	{
		// ľĆ·ąłŞĽ­ąö·Î ŔĚµż˝Ăżˇ´Â Äü˝˝·Ô Á¤ş¸¸¦ żäĂ»ľČÇÔ.
		g_pD3dApp->m_pFieldWinSocket->SendMsg(T_FC_STORE_REQUEST_QUICKSLOT, NULL, 0);
	}
	//*--------------------------------------------------------------------------*//

	if( g_pD3dApp->m_pShuttleChild->m_myShuttleInfo.CharacterUniqueNumber==0 )
	{
		DBGOUT("CINFInterface::LoadCharacterFile() Character File Name(0) Error\n");
		return FALSE;
	}
	
	// 2008-02-27 by bhsohn ĹëÇŐ ľĆ·ąłŞ ĽöÁ¤
	if(bArenaRestart)
	{
		// ľĆ·ąłŞ »óĹÂżˇĽ­´Â ´Ů˝Ă ·ÎµĺÇŇÇĘżä°ˇ ľř´Ů.
		return TRUE;
	}

	// end 2008-02-27 by bhsohn ĹëÇŐ ľĆ·ąłŞ ĽöÁ¤

	FILE *	fp;
	char strFileName[32];
	int nResult;
	// 2007-11-22 by bhsohn ľĆ·ąłŞ ĹëÇŐĽ­ąö
	//wsprintf(strFileName, "%u.env", g_pD3dApp->m_pShuttleChild->m_myShuttleInfo.CharacterUniqueNumber);
	CHARACTER* pMainInfo = g_pD3dApp->GetMFSMyShuttleInfo();
	wsprintf(strFileName, "%u.env", pMainInfo->CharacterUniqueNumber);
	fp = fopen(strFileName, "r+b");
	if(!fp)
	{
		DBGOUT("CINFInterface::LoadCharacterFile() Character File(%s) Open(w+b) Error:(Normal)\n", strFileName);
		InitCharacterData();
		return FALSE;
	}
	nResult = fseek(fp, 0L, SEEK_END);
	if( nResult != 0)
	{
		DBGOUT("CINFInterface::LoadCharacterFile() Character File(%s) fseek(SEEK_END) Error\n", strFileName);
		fclose(fp);
		InitCharacterData();
		return FALSE;
	}
	nResult = ftell(fp);
//	if(nResult != sizeof(sOPTION_CHARACTER))
	if(nResult != sizeof(BOOL))
	{
		DBGOUT("CINFInterface::LoadCharacterFile() Character File(%s) SIZE Error\n", strFileName);
		fclose(fp);
		InitCharacterData();
		return FALSE;
	}
	nResult = fseek(fp, 0L, SEEK_SET);
	if( nResult != 0)
	{
		DBGOUT("CINFInterface::LoadCharacterFile() Character File(%s) fseek(SEEK_SET) Error\n", strFileName);
		fclose(fp);
		InitCharacterData();
		return FALSE;
	}
//	nResult = fread(g_pSOptionCharacter, sizeof(sOPTION_CHARACTER), 1, fp);
	nResult = fread(&g_pSOptionCharacter->sHelpDesk, sizeof(BOOL), 1, fp);
	if(nResult != 1)
	{
		DBGOUT("CINFInterface::LoadOptionFile() File Read Error(%s), Set Default\n", strFileName);
		fclose(fp);
		InitCharacterData();
		return FALSE;
	}
	
	fclose(fp);
	return TRUE;
}

BOOL CInterface::SaveCharacterFile()
{
	FLOG( "CInterface::SaveCharacterFile()" );

	if( !m_pGameMain )
	{
		DBGOUT("CINFInterface::SaveCharacterFile() Game GUI not Ready State\n");
		return FALSE;
	}
	if( g_pD3dApp->m_pShuttleChild->m_myShuttleInfo.CharacterUniqueNumber==0 )
	{
		DBGOUT("CINFInterface::SaveCharacterFile() Character File name(0) Error\n");
		return FALSE;
	}

	MSG_FC_STORE_SAVE_QUICKSLOT sMsg;
	for(auto i = 0; i < QUICKTABCOUNT; i++)
	{
		auto nTabCount = 0;
		auto nDelTabCount = 0;

		for(auto j = 0; j < QUICKSLOTCOUNT; j++)
		{
			if(g_pSOptionCharacter->ItemNum[i][j] != 0)
			{
				nTabCount++;

				// »čÁ¦µČ »óĹÂ
				if(m_pGameMain->m_pQuickSlot->m_pQSlotInfo[i][j].pItem == nullptr)
				{
					nDelTabCount++;
					g_pSOptionCharacter->ItemNum[i][j] = 0;
				}
				else
				{
					g_pSOptionCharacter->ItemNum[i][j] = m_pGameMain->m_pQuickSlot->m_pQSlotInfo[i][j].pItem->ItemNum;
				}
			}
			else if(m_pGameMain->m_pQuickSlot->m_pQSlotInfo[i][j].pItem)
			{		
				g_pSOptionCharacter->ItemNum[i][j] = m_pGameMain->m_pQuickSlot->m_pQSlotInfo[i][j].pItem->ItemNum;
				nTabCount++;
			}
			
			sMsg.POS[i].arrItemNum_[j] = g_pSOptionCharacter->ItemNum[i][j];
		}

		sMsg.POS[i].Delete = FALSE;

		if(nTabCount != 0 &&
			nTabCount == nDelTabCount)
		{
			sMsg.POS[i].Delete = TRUE;
		}
	}
	
	g_pFieldWinSocket->SendMsg(T_FC_STORE_SAVE_QUICKSLOT, reinterpret_cast<char*>(&sMsg), sizeof(sMsg));
	g_pD3dApp->m_bCheckQuickSlotSave = TRUE;
	
	return TRUE;
}

BOOL CInterface::SaveOptionFile()
{
	// 2008-06-20 by bhsohn EP3 żÉĽÇ°ü·Ă Ăł¸®
	SaveOptionEtcInfo();		
	// end 2008-06-20 by bhsohn EP3 żÉĽÇ°ü·Ă Ăł¸®	

	FLOG( "CInterface::SaveOptionFile()");
	FILE *	fp;
	char strFileName[32];
	strcpy(strFileName, "option.sys");
	fp = fopen(strFileName, "w+b");
	if(!fp)
	{
		DBGOUT("CINFInterface::SaveOptionFile() Option File(%s) open(w+b) Error\n",strFileName);
		return FALSE;
	}
	int nSize = fwrite(g_pSOption, sizeof(sOPTION_SYSTEM)/* - QSLOT_NUMBER*QSLOT_TAB_NUMBER*(sizeof(int)+sizeof(UID64_t))-sizeof(BOOL)*/, 1, fp);

	if(nSize != 1)
	{
		DBGOUT("CINFInterface::SaveOptionFile() File write Error(%s)\n", strFileName);
		fclose(fp);
		fp = fopen(strFileName, "w+b");  // size=0 ĆÄŔĎ »ýĽş
		fclose(fp);
		return FALSE;
	}
	fclose(fp);
	return TRUE;

}

void CInterface::SetWindowOrder(int nWndIndex)
{
	// ĂĘ±â ĽłÁ¤ Á¶°Ç
	if(g_pGameMain == NULL)
		return;

	if(nWndIndex != WNDInfWindow)
		g_pGameMain->m_pInfWindow->ClearAllMessageString();
//	else if(g_pD3dApp->m_dwGameState != _SHOP)
//		g_pGameMain->m_pInfWindow->ResetMessageString();
	if(nWndIndex != WNDCommunityVOIP)
		g_pGameMain->m_pCommunityVOIP->ClearWindowString();
	// 2007-07-20 by dgwoo ĂÖÁ¶ FAQżˇ ÁřŔÔ˝Ă ÇŃąřŔş Ăł¸®ÇŘľßÇŇ »çÇ×.
	if(nWndIndex == WNDGameMainFaq)
		g_pGameMain->m_pInfGameMainFaq->SetInit();

	// Á¤ąćÇâ ĽřĽ­ ŔçÁöÁ¤
	m_vecGameWindowWndReverse.clear();
	vector<GameWindowWnd>::iterator itWnd = m_vecGameWindowWnd.begin();
	while(itWnd != m_vecGameWindowWnd.end())
	{
		if((*itWnd).nWindowIndex == nWndIndex)
		{
			int nNum = (*itWnd).nWindowOrder;
			(*itWnd).nWindowOrder = WNDIndexEnd-1;
			vector<GameWindowWnd>::iterator itWndRe = m_vecGameWindowWnd.begin();
			while(itWndRe != m_vecGameWindowWnd.end())
			{
				if((*itWndRe).nWindowOrder > nNum && (*itWnd).nWindowIndex != (*itWndRe).nWindowIndex)
				{
					(*itWndRe).nWindowOrder--;
				}
				itWndRe++;
			}
		}
		itWnd++;
	}

	// żŞąćÇâ ŔúŔĺ
	GameWindowWnd vecTemp;
	vector<GameWindowWnd>::iterator itWndR = m_vecGameWindowWnd.begin();
	while(itWndR != m_vecGameWindowWnd.end())
	{
		int nNum = WNDIndexEnd - (*itWndR).nWindowOrder;
		vecTemp.nWindowIndex = (*itWndR).nWindowIndex;
		vecTemp.nWindowOrder = nNum;
		m_vecGameWindowWndReverse.push_back(vecTemp);

		itWndR++;
	}
}

void CInterface::InitWindowsFirstFormat()
{
	GameWindowWnd	vecGameWindowWnd;

	for(int nCount=0; nCount<WNDIndexEnd; nCount++)
	{
		vecGameWindowWnd.nWindowIndex = nCount;
		vecGameWindowWnd.nWindowOrder = nCount;
		m_vecGameWindowWnd.push_back(vecGameWindowWnd);
		
		vecGameWindowWnd.nWindowOrder = WNDIndexEnd-nCount;
		m_vecGameWindowWndReverse.push_back(vecGameWindowWnd);
	}	
}

//////////////////////////////////////////////////////////////////////////
// ŔĚÇĎ żěĽ±ĽřŔ§ Ăł¸® ĽłÁ¤ 
// 2005.10.31 ydkim
//////////////////////////////////////////////////////////////////////////
int CInterface::WindowsWndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// 2005-10-12 by ispark Ć©Ĺä¸®ľó ·ą˝Ľ2´Â Çăżë BĹ°·Î Á¤Áö ¸ŢĽĽÁö ¶§ą®żˇ...
	if(g_pTutorial->IsTutorialMode() == TRUE){
		if(g_pTutorial->GetLesson() != L2){
			return INF_MSGPROC_NORMAL;
		}
	}

	// 2005-11-16 by ispark żŢÂĘ ąöĆ° Ĺ¬¸Ż˝Ă »óŔ§żˇĽ­ Ăł¸®ÇŘľß ÇĎ´Â °Íµé
	if(uMsg == WM_LBUTTONDOWN)
	{
		POINT pt;
		pt.x = LOWORD(lParam);
		pt.y = HIWORD(lParam);
		CheckMouseReverse(&pt);

		// Ă¤ĆĂ ŔĚżÜżˇ Ĺ¬¸Ż˝Ă ą«Čż
		if( !(pt.x > 25 && 
			pt.x < 245 &&
			pt.y < g_pD3dApp->GetBackBufferDesc().Height - 6 && 
			pt.y > g_pD3dApp->GetBackBufferDesc().Height - 28))
		{
			m_pGameMain->m_pChat->SetCleanChat();
		}
	}
	
	// ÇĘµĺ»óżˇĽ­ Äł˝¬ »óÁˇżˇ µéľî°¬Ŕ»¶§
	BOOL bEnterCash = FALSE;
//	if(m_pCityBase && m_pCityBase->GetCurrentBuildingNPC()){
//		bEnterCash = TRUE;
//		g_pGameMain->m_bEnterBuilingCashShop = TRUE;
//		m_bShowInterface = TRUE;
//	}
//	else{
	// Äł˝¬ »óÁˇŔĚ ľĆ´Ň¶§
		g_pGameMain->m_bEnterBuilingCashShop = FALSE;

		if(uMsg == WM_KEYDOWN && g_pD3dApp->m_dwGameState != _SHOP)
		{
			// 2006-09-08 by dgwoo 1:1°Ĺ·ˇÁßżŁ FĹ°¸¦ ¸·´Â´Ů.
			if(g_pD3dApp->m_bChatMode == FALSE && g_pGameMain->m_bVOIPFlag == FALSE && !g_pGameMain->m_pTrade->m_bTrading)
			{
				switch(lParam)
				{
				case WM_KEYDOWN_LPARAM_F:
					{
						// 2007-11-28 by bhsohn ˝şĹ©¸° Ľ¦ ¸đµĺ Ăß°ˇ
						CHARACTER myShuttleInfo = g_pShuttleChild->GetMyShuttleInfo();
						// ľĆŔĚĹŰ °ü¸®ŔÚ, żîżµŔÚ¸¸ ˝şĹ©¸°Ľ¦ ¸đµĺ »çżë
						if((COMPARE_RACE(myShuttleInfo.Race,RACE_OPERATION|RACE_GAMEMASTER))
							&& GetAsyncKeyState(VK_CONTROL))
						{
							BOOL bShotMode = IsScreenShotMode();
							bShotMode ^= TRUE;
							SetScreenShotMode(bShotMode);

							return INF_MSGPROC_BREAK;
						}
						// end 2007-11-28 by bhsohn ˝şĹ©¸° Ľ¦ ¸đµĺ Ăß°ˇ

						m_bShowInterface = !m_bShowInterface;
						
						// 2009. 12. 04 by jskim ŔŻ´Ö »ç¸Á˝Ă ŔÎĹÍĆäŔĚ˝ş¸¦ ľřľÖ¸é ±ÍČŻ ¸Ţ˝ĂÁö ąÚ˝şµµ ľřľîÁö´Â ąö±× ĽöÁ¤
						if(m_bShowInterface && g_pShuttleChild->m_dwState == _FALLEN)
						{
							g_pShuttleChild->SendDeadMsg();
						}
						//end 2009. 12. 04 by jskim ŔŻ´Ö »ç¸Á˝Ă ŔÎĹÍĆäŔĚ˝ş¸¦ ľřľÖ¸é ±ÍČŻ ¸Ţ˝ĂÁö ąÚ˝şµµ ľřľîÁö´Â ąö±× ĽöÁ¤
						
						g_pGameMain->m_pChat->m_pSystemMsgW->SetInterface(m_bShowInterface);
						g_pGameMain->m_pChat->SetWisperChatShowInterface(m_bShowInterface);

						if(m_bShowInterface == FALSE)
						{
							g_pGameMain->RightWindowShow(FALSE,g_pGameMain->m_nRightWindowInfo);
							g_pGameMain->LeftWindowShow(FALSE,g_pGameMain->m_nLeftWindowInfo);
						}

						// 2008-06-20 by bhsohn EP3 żÉĽÇ°ü·Ă Ăł¸®
						if(m_pGameMain && m_pGameMain->m_pSystem)
						{
							m_pGameMain->m_pSystem->RefreshShowInterface(!GetShowInterface());
						}
						
						// end 2008-06-20 by bhsohn EP3 żÉĽÇ°ü·Ă Ăł¸®
						
						return INF_MSGPROC_BREAK;
					}
				case WM_KEYDOWN_LPARAM_R:
					{
						// 2008-08-11 by bhsohn ´Üµ¶Ŕ¸·Î ˝ÇÇŕ µÇ°Ô˛ű
						CHARACTER myShuttleInfo = g_pShuttleChild->GetMyShuttleInfo();
						// ľĆŔĚĹŰ °ü¸®ŔÚ, żîżµŔÚ¸¸ ˝şĹ©¸°Ľ¦ ¸đµĺ »çżë
						if((COMPARE_RACE(myShuttleInfo.Race,RACE_OPERATION))
							&& GetAsyncKeyState(VK_CONTROL))
						{
							g_pD3dApp->SaveSinglePlayInfo();
							return INF_MSGPROC_BREAK;
						}
						// end 2008-08-11 by bhsohn ´Üµ¶Ŕ¸·Î ˝ÇÇŕ µÇ°Ô˛ű
					}
					break;
				case WM_KEYDOWN_LPARAM_Q:
					{
						// 2007-03-27 by bhsohn żÉŔúąö ¸đµĺ˝Ă ŔÎĹÍĆäŔĚ˝ş Ľű±č
						if(g_pShuttleChild->IsObserverMode())
						{
							return INF_MSGPROC_NORMAL;
						}
						// end 2007-03-27 by bhsohn żÉŔúąö ¸đµĺ˝Ă ŔÎĹÍĆäŔĚ˝ş Ľű±č
						
						// 2006-12-01 by ispark, Á×Ŕş »óĹÂ°ˇ ľĆ´Ď¸é GetItem ş¸ł˝´Ů.
						if(g_pShuttleChild->CheckUnitState() != BREAKDOWN)
						{
							g_pScene->SendItemGetItemAll();
						}
					}
					break;
				}
			}
			if(!m_bShowInterface && wParam == VK_ESCAPE)
			{
				m_bShowInterface = TRUE;
				return INF_MSGPROC_BREAK;
			}
		}
//	}
	if(m_pGameArena && 
		m_pGameArena->WndProc(uMsg,wParam,lParam) == INF_MSGPROC_BREAK)
	{
		return INF_MSGPROC_BREAK;
	}


	// 2009. 11. 02 by ckPark ŔÎÇÇ´ĎĆĽ ÇĘµĺ ŔÎ˝şĹĎ˝ş ´řÁŻ ˝Ă˝şĹŰ
	if( m_pInfinity
		&& m_pInfinity->WndProc( uMsg, wParam, lParam ) == INF_MSGPROC_BREAK )
	{
		return INF_MSGPROC_BREAK;
	}
	// end 2009. 11. 02 by ckPark ŔÎÇÇ´ĎĆĽ ÇĘµĺ ŔÎ˝şĹĎ˝ş ´řÁŻ ˝Ă˝şĹŰ


	// 2010. 03. 03 by ckPark ŔÎÇÇ´ĎĆĽ ÇĘµĺ 2Â÷(ŔÔŔĺ UIşŻ°ć)
	if( m_pInfinityPopup
		&& m_pInfinityPopup->WndProc( uMsg, wParam, lParam ) == INF_MSGPROC_BREAK )
	{
		return INF_MSGPROC_BREAK;
	}
	// end 2010. 03. 03 by ckPark ŔÎÇÇ´ĎĆĽ ÇĘµĺ 2Â÷(ŔÔŔĺ UIşŻ°ć)


	// 2008-04-08 by dgwoo ¸đĽ± żÉĽÇĂ˘Ŕ» ¶çľúŔ»°ćżě ´Ů¸Ą ŔĚşĄĆ®¸¦ ¸·´Â´Ů.
	if(g_pGameMain->GetINFMotherShipManager() &&
		g_pGameMain->GetINFMotherShipManager()->WndProcOption(uMsg,wParam,lParam) == INF_MSGPROC_BREAK)
	{
		return INF_MSGPROC_BREAK;
	}
	for (int nLoopWndProcOrder = 0; nLoopWndProcOrder < WNDIndexEnd; nLoopWndProcOrder++) {
		auto window = std::find_if(m_vecGameWindowWndReverse.begin(), m_vecGameWindowWndReverse.end(), [nLoopWndProcOrder](const GameWindowWnd& wnd) {return wnd.nWindowOrder == nLoopWndProcOrder; });
		if (window == m_vecGameWindowWndReverse.end()) {
			continue;
		}

		switch ((*window).nWindowIndex) {
			case WNDGameGlobalImage:
				if (WndProcGlob(uMsg, wParam, lParam) == INF_MSGPROC_BREAK)
					return INF_MSGPROC_BREAK;
				break;
			case WNDInfWindow:
				if (bEnterCash)
					break;
				if (WndProcInfW(uMsg, wParam, lParam) == INF_MSGPROC_BREAK)
					return INF_MSGPROC_BREAK;
				break;
			case WNDCityBase:
				if (bEnterCash)
					break;
				if (WndProcCity(uMsg, wParam, lParam) == INF_MSGPROC_BREAK)
					return INF_MSGPROC_BREAK;
				break;
			case WNDBazaarShop:
				if (bEnterCash || !m_pBazaarShop)
					break;
				if (WndProcBaza(uMsg, wParam, lParam) == INF_MSGPROC_BREAK)
					return INF_MSGPROC_BREAK;
				break;
			case WNDGameMain:
				if (bEnterCash == TRUE)
					break;
				if (WndProcGame(uMsg, wParam, lParam) == INF_MSGPROC_BREAK)
					return INF_MSGPROC_BREAK;
				break;
			case WNDMissionInfo:
				if (!bEnterCash && !m_bShowInterface && g_pTutorial->IsTutorialMode() == TRUE)
					break;
				// 2007-03-27 by bhsohn 옵저버 모드시 인터페이스 숨김
				if (g_pShuttleChild->IsObserverMode()) {
					break;
				}
				// end 2007-03-27 by bhsohn 옵저버 모드시 인터페이스 숨김
				if (WndProcMiss(uMsg, wParam, lParam) == INF_MSGPROC_BREAK)
					return INF_MSGPROC_BREAK;
				break;
			case WNDGameMainFaq:
				if (m_pGameMain->m_bFAQProsFlag == FALSE)
					break;
				if (WndProcFAQs(uMsg, wParam, lParam) == INF_MSGPROC_BREAK)
					return INF_MSGPROC_BREAK;
				break;
			case WNDCommunityVOIP:
				if (g_pGameMain->m_pCommunityVOIP && g_pGameMain->m_bVOIPFlag)
					if (WndProcVoIP(uMsg, wParam, lParam) == INF_MSGPROC_BREAK)
						return INF_MSGPROC_BREAK;
				break;
			case WNDMiniMap:
				if (WndProcMini(uMsg, wParam, lParam) == INF_MSGPROC_BREAK)
					return INF_MSGPROC_BREAK;
				break;
			case WNDMenuList:
				if (g_pGameMain->m_bMenuListFlag == TRUE && g_pInterface->m_pTarget->GetShowTargetCodition() == FALSE)
					if (WndProcMenu(uMsg, wParam, lParam) == INF_MSGPROC_BREAK)
						return INF_MSGPROC_BREAK;
				break;
			case WNDLeftRightWindow:
				// 					if(g_pGameMain->m_pInfWindow->WndProc(uMsg, wParam, lParam) == INF_MSGPROC_BREAK)
				// 						return INF_MSGPROC_BREAK;
				/*	else */if (g_pGameMain->WndProcLeftRightWindow(uMsg, wParam, lParam) == INF_MSGPROC_BREAK)
					return INF_MSGPROC_BREAK;
				break;
			case WNDMp3Player:
#ifndef C_EPSODE4_UI_CHANGE_JSKIM					        // 2011. 10. 10 by jskim UI시스템 변경
				// 2011. 10. 10 by jskim UI시스템 변경
				if (g_pGameMain->m_pMp3Player && g_pGameMain->m_bMp3PlayFlag == TRUE &&		// 2011. 10. 10 by jskim UI시스템 변경
					// 2009. 12. 04 by jskim 마우스 포인터 안보이기 선택시 music player를 사용할 수 없게 되는 버그 수정
					//m_pTarget->GetShowTargetCodition() == FALSE &&
					//end 2009. 12. 04 by jskim 마우스 포인터 안보이기 선택시 music player를 사용할 수 없게 되는 버그 수정
					m_bShowInterface == TRUE &&
					!g_pShuttleChild->IsObserverMode())		// 2007-03-27 by bhsohn 옵저버 모드시 인터페이스 숨김
				{
					if (WndProcMp3P(uMsg, wParam, lParam) == INF_MSGPROC_BREAK)
						return INF_MSGPROC_BREAK;
				}
				// end 2011. 10. 10 by jskim UI시스템 변경
#endif
				break;
				// 2008-04-04 by bhsohn Ep3 커뮤니티 창
			case WNDCommunityWnd:
				{
					if (g_pGameMain && g_pGameMain->WndProcCommunityWnd(uMsg, wParam, lParam) == INF_MSGPROC_BREAK) {
						return INF_MSGPROC_BREAK;
					}
				}
				break;
				// end end 2008-04-04 by bhsohn Ep3 커뮤니티 창
				// 2008-11-13 by bhsohn 조이스틱 작업
			case WNDOpJoystick:
				{
					if (g_pGameMain && g_pGameMain->WndProcOpJoystickWnd(uMsg, wParam, lParam) == INF_MSGPROC_BREAK) {
						return INF_MSGPROC_BREAK;
					}
				}
				break;
				// end 2008-11-13 by bhsohn 조이스틱 작업
				// 2008-08-22 by bhsohn EP3 인벤토리 처리
			case WNDInvenWnd:
				{
					if (g_pGameMain && g_pGameMain->WndProcInvenWnd(uMsg, wParam, lParam) == INF_MSGPROC_BREAK) {
						return INF_MSGPROC_BREAK;
					}
				}
				break;
				// end 2008-08-22 by bhsohn EP3 인벤토리 처리
				// 2008-09-22 by bhsohn EP3 캐릭터 창
			case WNDCharacterWnd:
				{
					if (m_pGameMain
						&& m_pGameMain->m_pCharacterInfo
						&& (m_pGameMain->m_pCharacterInfo->WndProc(uMsg, wParam, lParam) == INF_MSGPROC_BREAK)) {
						return INF_MSGPROC_BREAK;
					}
				}
				break;
				// end 2008-09-22 by bhsohn EP3 캐릭터 창
				// 2008-07-10 by bhsohn 시스템 창 우선순위 추가
			case WNDSysWnd:
				{
					if (WndProcSystemMsg(uMsg, wParam, lParam) == INF_MSGPROC_BREAK) {
						return INF_MSGPROC_BREAK;
					}
				}
				break;
			case WNDVoiceWnd:
				{
					if (m_pGameMain && m_pGameMain->m_pChat && m_pGameMain->m_pChat->WndProcVoiceOption(uMsg, wParam, lParam) == INF_MSGPROC_BREAK) {
						return INF_MSGPROC_BREAK;
					}
				}
				break;
			case WNDMacroWnd:
				{
					if (m_pGameMain && m_pGameMain->m_pChat && m_pGameMain->m_pChat->WndProcMacro(uMsg, wParam, lParam) == INF_MSGPROC_BREAK) {
						return INF_MSGPROC_BREAK;
					}
				}
				break;

				// 2009. 01. 12 by ckPark 선전 포고 시스템
			case WNDWarDecalre:
				{
					if (m_pGameMain && m_pGameMain->GetINFMotherShipManager() && m_pGameMain->GetINFMotherShipManager()->GetWarDeclareWindow()->WndProcWarDeclare(uMsg, wParam, lParam) == INF_MSGPROC_BREAK) {
						return INF_MSGPROC_BREAK;
					}
				}
				break;
				// end 2009. 01. 12 by ckPark 선전 포고 시스템
					 // 2009-02-13 by bhsohn 월드 랭킹 시스템
			case WNDWorldRank: // 월드 랭킹 창 
				{
					if (m_pGameMain && m_pGameMain->WndProcWorldRank(uMsg, wParam, lParam) == INF_MSGPROC_BREAK) {
						return INF_MSGPROC_BREAK;
					}
				}
				break;
				// end 2009-02-13 by bhsohn 월드 랭킹 시스템
		}
	}

	if (m_pGameArena &&
		m_pGameArena->WndProc(uMsg, wParam, lParam) == INF_MSGPROC_BREAK) {
		return INF_MSGPROC_BREAK;
	}

	return INF_MSGPROC_NORMAL;
}

int CInterface::WndProcGlob(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return m_pGameGlobalImage->WndProc(uMsg, wParam, lParam);
}
int CInterface::WndProcGame(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return m_pGameMain->WndProc(uMsg, wParam, lParam);
}
int CInterface::WndProcCity(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return m_pCityBase->WndProc(uMsg, wParam, lParam);
}
int CInterface::WndProcInfW(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return g_pGameMain->m_pInfWindow->WndProc(uMsg, wParam, lParam);
}
int CInterface::WndProcMiss(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// 2007-06-12 by bhsohn ąĚĽÇ ŔÎĹÍĆäŔĚ˝ş ĽöÁ¤ľČ
	//return g_pGameMain->m_pMissionInfo->WndProc(uMsg, wParam, lParam);
	return g_pGameMain->WndProcMiss(uMsg, wParam, lParam);
}
int CInterface::WndProcFAQs(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return g_pGameMain->m_pInfGameMainFaq->WndProc(uMsg, wParam, lParam);
}
int CInterface::WndProcVoIP(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return g_pGameMain->m_pCommunityVOIP->WndProc(uMsg, wParam, lParam);
}
int CInterface::WndProcMini(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return m_pGameMain->m_pMiniMap->WndProc(uMsg, wParam, lParam);
}
int CInterface::WndProcMenu(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return g_pGameMain->m_pMenuList->WndProc(uMsg, wParam, lParam);
}
int CInterface::WndProcMp3P(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return g_pGameMain->m_pMp3Player->WndProc(uMsg, wParam, lParam);
}
int CInterface::WndProcBaza(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if ( m_pBazaarShop->GetbBazaarShopEnd() )
		return INF_MSGPROC_NORMAL;

	// 2006-08-29 by dgwoo ĂÖĽŇČ­˝Ă Inven ProcŔ¸·Î µéľî°ˇÁö ľĘµµ·Ď. 
	if((m_pBazaarShop->WndProc(uMsg, wParam, lParam) == INF_MSGPROC_BREAK))
		return INF_MSGPROC_BREAK;

	if(m_pBazaarShop->GetBazaarDisable() == FALSE &&
		m_pGameMain->m_pInven->WndProcOnCityBase(uMsg, wParam, lParam) == INF_MSGPROC_BREAK)
		return INF_MSGPROC_BREAK;

	return INF_MSGPROC_NORMAL;
}

// 2008-07-10 by bhsohn ˝Ă˝şĹŰ Ă˘ żěĽ±ĽřŔ§ Ăß°ˇ
int CInterface::WndProcSystemMsg(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return m_pGameMain->WndProcSystemMsg(uMsg, wParam, lParam);
}
//////////////////////////////////////////////////////////////////////////
// ŔĚÇĎ żěĽ±ĽřŔ§ ·»´ő¸µ ĽłÁ¤ 
// 2005.10.31 ydkim
//////////////////////////////////////////////////////////////////////////
void CInterface::WindowsRender()
{	
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////	
	//»óÁˇ °ü·Ă ·»´ő¸µ
//	if( m_pCityBase && m_pCityBase->GetCurrentBuildingNPC())
//	{
//		m_pCityBase->Render();	
//	}
	
	// 2006-07-25 by ispark, °łŔÎ»óÁˇ
	if(m_pBazaarShop)
		m_pBazaarShop->Render();
	//////////////////////////////////////////////////////////////////////////
	// ¸ĘŔĚ¸§ ·»´ő¸µ
	RenderProcMapName();


	// 2009. 11. 02 by ckPark ŔÎÇÇ´ĎĆĽ ÇĘµĺ ŔÎ˝şĹĎ˝ş ´řÁŻ ˝Ă˝şĹŰ
	if( m_pInfinity )
		m_pInfinity->Render();
	// end 2009. 11. 02 by ckPark ŔÎÇÇ´ĎĆĽ ÇĘµĺ ŔÎ˝şĹĎ˝ş ´řÁŻ ˝Ă˝şĹŰ

	// 2010. 04. 21 by jskim ˝Ĺ±Ô ·°Ĺ° ¸Ó˝Ĺ ±¸Çö
	// 2008-08-22 by bhsohn EP3 ŔÎşĄĹä¸® Ăł¸®
	//if( g_pGameMain )
	//	g_pGameMain->RenderInvenWnd();
	// end 2008-08-22 by bhsohn EP3 ŔÎşĄĹä¸® Ăł¸®
	//end 2010. 04. 21 by jskim ˝Ĺ±Ô ·°Ĺ° ¸Ó˝Ĺ ±¸Çö

	// żěĽ±ĽřŔ§
	RenderOrderWindows(0);
	// 2006-04-17 by ispark, µµżň¸», ŔÓ˝Ă·Î łůŔ˝
	if(m_pGameMain->m_bShowHelp)
		m_pGameMain->RenderHelp();
	

	// 2006-08-11 by ispark, ąĚĽÇĹ¸°Ů ł»żë ·ąŔĚ´ő ´ŮŔ˝Ŕ¸·Î ÂďŔ˝
	//if(g_pGameMain->m_pMissionInfo &&
	if(g_pTutorial->IsTutorialMode() == FALSE &&
		g_pD3dApp->m_dwGameState != _SHOP &&
        // 2011. 03. 08 by jskim ŔÎÇÇ3Â÷ ±¸Çö - Äů˝şĆ® ˝Ă˝şĹŰ		
		!IS_MAP_INFLUENCE_ARENA(g_pD3dApp->GetMyShuttleMapInfo()->MapInfluenceType) &&
		!IS_MAP_INFLUENCE_INFINITY(g_pD3dApp->GetMyShuttleMapInfo()->MapInfluenceType))
		// end 2011. 03. 08 by jskim ŔÎÇÇ3Â÷ ±¸Çö - Äů˝şĆ® ˝Ă˝şĹŰ
	{
		// 2007-06-12 by bhsohn ąĚĽÇ ŔÎĹÍĆäŔĚ˝ş ĽöÁ¤ľČ
		//g_pGameMain->m_pMissionInfo->RenderMissionTarget();
		g_pGameMain->RenderMissionINGInfo();
	}
	// 2006-08-01 by ispark ±Ó¸»
	//m_pGameMain->m_pChat->RenderWisperChat();
	// ľĆŔĚĹŰ ŔÎĆ÷
	// 2009-02-03 by bhsohn ŔĺÂř ľĆŔĚĹŰ şń±ł ĹřĆÁ
// 	if(m_pGameMain->m_pItemInfo) 
// 		m_pGameMain->m_pItemInfo->Render();
	if(m_pGameMain) 
	{
		m_pGameMain->RenderItemInfo();
	}
	// end 2009-02-03 by bhsohn ŔĺÂř ľĆŔĚĹŰ şń±ł ĹřĆÁ

	// 2007-05-08 by dgwoo ľĆ·ąłŞ °ü·Ă UI´Â °ˇŔĺ Ŕ§żˇ ±×¸°´Ů.
	if(m_pGameArena)
		m_pGameArena->Render();

		
	// 2010. 03. 03 by ckPark ŔÎÇÇ´ĎĆĽ ÇĘµĺ 2Â÷(ŔÔŔĺ UIşŻ°ć)
	if( m_pInfinityPopup )
		m_pInfinityPopup->Render();
	// end 2010. 03. 03 by ckPark ŔÎÇÇ´ĎĆĽ ÇĘµĺ 2Â÷(ŔÔŔĺ UIşŻ°ć)


	// ĹřĆÁ °ü·Ă ·»´ő¸µ
	m_pGameMain->RenderToolTip();

	// 2010. 04. 28 by hsLee ŔÎÇÇ´ĎĆĽ ÇĘµĺ 2Â÷ ł­ŔĚµµ Á¶Ŕý.
	// ĹřĆÁ ·Ł´ő¸µ. (°łÇŕ Áöżř.)
	m_pGameMain->RenderToolTipEx();
	// End. 2010. 04. 28 by hsLee ŔÎÇÇ´ĎĆĽ ÇĘµĺ 2Â÷ ł­ŔĚµµ Á¶Ŕý.

	// 2006-07-27 by ispark, Ľ±ĹĂ ľĆŔĚĹŰ ·»´ő¸µ		
	m_pGameMain->RenderSelectItem();

	// 2007-07-30 by bhsohn żŔĆŰ·ąŔĚĹÍ ·Ł´ő¸µ ĽřĽ­ şŻ°ć
	
	if(m_pINFOpMain)
	{
		// żŔĆŰ·ąŔĚĹÍ ·Ł´ő¸µ
		m_pINFOpMain->RenderOpHelper(g_pD3dApp->m_dwGameState);
		m_pINFOpMain->RenderOpWnd();
	}
	// endn 2007-07-30 by bhsohn żŔĆŰ·ąŔĚĹÍ ·Ł´ő¸µ ĽřĽ­ şŻ°ć

	// 2008-04-04 by dgwoo ¸đĽ±Ŕü Á¤ş¸ żÉĽÇĂ˘ Á© Ŕ§żˇ ±×¸˛.
	if(m_pGameMain && m_pGameMain->GetINFMotherShipManager())
	{
		m_pGameMain->GetINFMotherShipManager()->RenderOption();
	}


	// Ĺ¸°Ů °ü·Ă 
	if( m_pTarget )	
		m_pTarget->Render();

	// Ć©Ĺä¸®ľó
	// 2007-07-26 by dgwoo
	if(g_pTutorial && g_pTutorial->IsTutorialMode() == TRUE)
	{
//		g_pTutorial->RenderETC();
		g_pTutorial->Render();
	}
	// 2007-07-26 by dgwoo Ć©Ĺä¸®ľó ¸ŢŔÎČ­¸éżˇĽ­ Ĺ¸°ŮĆĂ ŔĚąĚÁö ľČş¸ŔĚ°Ô ĽöÁ¤.
	if(m_pTarget)
	{
		// 2007-06-21 by dgwoo ¸¶żě˝ş Ć÷ŔÎĹÍ´Â ş¸ŔĚ°Ô şŻ°ć.
		POINT pt;
		GetCursorPos(&pt);
		ScreenToClient(g_pD3dApp->GetHwnd(), &pt);
		CheckMouseReverse(&pt);

		m_pTarget->RenderMouse(pt.x, pt.y ,m_pTarget->m_nMouseType);
	}


	// Ľ±ĹĂ ľĆŔĚĹŰ ľĆŔĚÄÜ ·»´ő¸µ
	if(m_pGameMain->m_pSelectIcon)
	{
        POINT ptCursor;
        GetCursorPos( &ptCursor );
        ScreenToClient( g_pD3dApp->GetHwnd(), &ptCursor );
		CheckMouseReverse(&ptCursor);

		m_pGameMain->m_pSelectIcon->Move(ptCursor.x-m_pGameMain->m_nIconPosX, ptCursor.y-m_pGameMain->m_nIconPosX);
		m_pGameMain->m_pSelectIcon->Render();
	}

}

void CInterface::RenderProcMapName()
{
	if(m_pMapLoad->GetRenderMapNameImg())
	{	// ¸ĘŔĚ¸§ ·»´ő¸µ				
#ifdef MAP_EDITOR
		CINFImage* pName = m_pMapNameImg->FindMapNameImage(g_pD3dApp->m_nMapNumber);
#else
		CINFImage* pName = m_pMapNameImg->FindMapNameImage(g_pShuttleChild->m_myShuttleInfo.MapChannelIndex.MapIndex);	
#endif
		if(pName)
		{
			pName->SetColor(m_pMapLoad->m_dwAlpha);
			pName->Move((g_pD3dApp->GetBackBufferDesc().Width/2)-152, RENDER_MAPNAME_POS_Y);
			pName->Render();
		}
		
		// PK°ˇ´É ¸ĘŔÎ°ˇ
		CINFImage* pName2 = NULL;
		if(g_pShuttleChild->m_pPkNormalTimer->IsCityWar() == TRUE)
		{
			pName2 = m_pMapNameImg->FindMapNameImage(5555);
		}
#ifdef MAP_EDITOR
		else if (IsPkEnableMap(g_pD3dApp->m_nMapNumber))
#else
		else if(IsPkEnableMap(g_pShuttleChild->m_myShuttleInfo.MapChannelIndex.MapIndex))
#endif
		{	// PK °ˇ´É ¸Ę
			pName2 = m_pMapNameImg->FindMapNameImage(1111);	
		}
		else
		{	// PK şŇ°ˇ´É ¸Ę
			pName2 = m_pMapNameImg->FindMapNameImage(9999);	
		}
		if(pName2)
		{
			pName2->SetColor(m_pMapLoad->m_dwAlpha);
			pName2->Move((g_pD3dApp->GetBackBufferDesc().Width/2)-152, RENDER_MAPNAME_POS_Y);
			pName2->Render();
		}
	}
}

void CInterface::RenderOrderWindows(int nRenderNum)
{
	for(int nLoopWndProcOrder = 0; nLoopWndProcOrder<WNDIndexEnd; nLoopWndProcOrder++)
	{
		vector<GameWindowWnd>::iterator itWnd = m_vecGameWindowWnd.begin();
		while(itWnd != m_vecGameWindowWnd.end())
		{
			if((*itWnd).nWindowOrder == nLoopWndProcOrder)
			{
				switch((*itWnd).nWindowIndex)
				{
				case WNDGameGlobalImage:
					{
						// ±Ű·Îąú ŔĚąĚÁö
						if(m_pGameGlobalImage)
							m_pGameGlobalImage->Render();
					}
					break;
				case WNDInfWindow:
					{
						// ˝Ă˝şĹŰ ¸ŢĽĽÁö ąÚ˝ş 
						if(m_pGameMain->m_pInfWindow)
							m_pGameMain->m_pInfWindow->RenderMessaegBox();
					}
					break;
				case WNDCityBase:
					{
						// 2008-07-14 by bhsohn EP3 °ü·Ă Ăł¸®
						//»óÁˇ °ü·Ă ·»´ő¸µ
						if( m_pCityBase && m_pCityBase->GetCurrentBuildingNPC())
						{
							m_pCityBase->Render();	
						}
						// end 2008-07-14 by bhsohn EP3 °ü·Ă Ăł¸®
					}
					break;
				case WNDGameMain:
						// 2008-07-10 by bhsohn ˝Ă˝şĹŰ Ă˘ żěĽ±ĽřŔ§ Ăß°ˇ
//						if(m_pGameMain->m_pChat)
//							m_pGameMain->m_pChat->RenderWisperChat();
					break;
				case WNDMissionInfo:
					{
						// 2007-06-12 by bhsohn ąĚĽÇ ŔÎĹÍĆäŔĚ˝ş ĽöÁ¤ľČ
						// ąĚĽÇ ŔÎĆ÷
						//if(g_pGameMain->m_pMissionInfo && 
						if(g_pTutorial->IsTutorialMode() == FALSE &&
							g_pD3dApp->m_dwGameState != _SHOP)
						{
							// 2007-06-12 by bhsohn ąĚĽÇ ŔÎĹÍĆäŔĚ˝ş ĽöÁ¤ľČ
							//g_pGameMain->m_pMissionInfo->Render();
							g_pGameMain->RenderMission();
						}
					}
					break;
				case WNDMiniMap:
					{
						// ąĚ´Ď¸Ę
						if(g_pD3dApp->m_dwGameState == _GAME || 
							g_pD3dApp->m_dwGameState == _SHOP )
						{
							if(m_pGameMain->m_pMiniMap)
								m_pGameMain->m_pMiniMap->Render();
						}
					}
					break;
				case WNDMp3Player:
					{
						// mp3 ÇĂ·ąŔĚľî
						if(	g_pGameMain->m_pMp3Player && 
							g_pGameMain->m_bMp3PlayFlag == TRUE && 
							m_bShowInterface && 
							!g_pShuttleChild->IsObserverMode() &&	// 2007-03-27 by bhsohn żÉŔúąö ¸đµĺ˝Ă ŔÎĹÍĆäŔĚ˝ş Ľű±č
							g_pTutorial->IsTutorialMode() == FALSE)
						{
							g_pGameMain->m_pMp3Player->Render();
						}
					}
					break;
				case WNDRenderHelp:
					{
						// 2006-04-17 by ispark
//						// µµ˝Ă,°ÔŔÓ °řĹë
//						if(m_pGameMain->m_bShowHelp)
//							m_pGameMain->RenderHelp();
					}
					break;
				case WNDInfSkill:
					{
						if( m_pGameMain->m_pInfSkill )
							m_pGameMain->m_pInfSkill->Render();
					}
					break;
				case WNDGameMainFaq:
					{
						// FAQ °ü·Ă
						if(m_pGameMain->m_pInfGameMainFaq && m_pGameMain->m_bFAQProsFlag)
							m_pGameMain->m_pInfGameMainFaq->Render();
					}
					break;
				case WNDCommunityVOIP:
					{
						// VOIP
						if(m_pGameMain->m_pCommunityVOIP && m_pGameMain->m_bVOIPFlag)
							m_pGameMain->m_pCommunityVOIP->Render();
					}
					break;
				case WNDQSlotIconFlag:
					{
						// ´ÜĂŕĹ° °ü·Ă ·»´ő¸µ
						if(m_pGameMain->m_bQSlotIconFlag == TRUE)
							m_pGameMain->RenderQSlotIcon();
					}
					break;
				case WNDItemInfo:
					{						
					}
					break;
				case WNDMenuList:
					{
						if(g_pGameMain->m_pMenuList && g_pGameMain->m_bMenuListFlag)
							g_pGameMain->m_pMenuList->Render();
					}
					break;
				case WNDMainChatStartMenu:
					{
						// Ăß°ˇ Ă¤ĆĂ ¸®˝şĆ®
						if(g_pGameMain->m_pChat->m_bMenuListUse)
							g_pGameMain->m_pChat->ViewChatMenuList();
					}
					break;
				case WNDLeftRightWindow:
					{						
//						if(g_pD3dApp->m_dwGameState != _SHOP)
						
						g_pGameMain->m_pInfWindow->Render() ;
						g_pGameMain->RenderLeftRightWindow();
					}
					break;
					// 2008-04-04 by bhsohn Ep3 ÄżąÂ´ĎĆĽ Ă˘
				case WNDCommunityWnd:
					{						
						// ÄżąÂ´ĎĆĽĂ˘ ·Ł´ő¸µ
						g_pGameMain->RenderCommunity();	
					}
					break;
					// end 2008-04-04 by bhsohn Ep3 ÄżąÂ´ĎĆĽ Ă˘
				// 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
				 case WNDOpJoystick:
					 {
						 if(g_pGameMain )
						 {
							 g_pGameMain->RenderOpJoystickWnd();
						 }							 
					 }
					 break;
					 // end 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
// 2010. 04. 21 by jskim ˝Ĺ±Ô ·°Ĺ° ¸Ó˝Ĺ ±¸Çö
// 					// 2008-08-22 by bhsohn EP3 ŔÎşĄĹä¸® Ăł¸®
				 case WNDInvenWnd:
 					 {
						 g_pGameMain->RenderInvenWnd();
				 }
 					 break;
// 					 // end 2008-08-22 by bhsohn EP3 ŔÎşĄĹä¸® Ăł¸®
// 					// 2008-09-22 by bhsohn EP3 Äł¸ŻĹÍ Ă˘
//end 2010. 04. 21 by jskim ˝Ĺ±Ô ·°Ĺ° ¸Ó˝Ĺ ±¸Çö
				 case WNDCharacterWnd:
					 {
						 if(g_pGameMain)
						 {
							 g_pGameMain->RenderCharacterWnd();
						 }						 
					 }
					 break;
					 // end 2008-09-22 by bhsohn EP3 Äł¸ŻĹÍ Ă˘
					// 2008-07-10 by bhsohn ˝Ă˝şĹŰ Ă˘ żěĽ±ĽřŔ§ Ăß°ˇ
				 case WNDSysWnd:
					 {

						 // 2009. 12. 04 by jskim Ć©Ĺä¸®ľó ˝ÇÇŕ˝Ă ˝Ă˝şĹŰ ¸Ţ˝ĂÁö Ă˘ ´ÝÇôÁö°Ô ĽöÁ¤
// 						if(m_pGameMain->m_pChat)
// 						{
// 							m_pGameMain->m_pChat->RenderSystemMsg();
// 						}
// 						
// 						if(m_pGameMain->m_pChat)
// 						{
// 							m_pGameMain->m_pChat->RenderWisperChat();
// 						}
						if(g_pD3dApp->m_pTutorial->IsTutorialMode() != TRUE)
						{
							if (m_pGameMain->m_pChat &&
								!m_pGameMain->m_pUnitInfoBar->m_bRenderEvent && !m_pGameMain->m_pUnitInfoBar->GetRenderPremiumToolTipTextOnOff()
								&& !m_pGameMain->m_pUnitInfoBar->m_bRenderEventRecovery)
						 {
							 m_pGameMain->m_pChat->RenderSystemMsg();
						 }
						 
						if(m_pGameMain->m_pChat)
						{
							m_pGameMain->m_pChat->RenderWisperChat();
						}
						}
						//end 2009. 12. 04 by jskim Ć©Ĺä¸®ľó ˝ÇÇŕ˝Ă ˝Ă˝şĹŰ ¸Ţ˝ĂÁö Ă˘ ´ÝÇôÁö°Ô ĽöÁ¤
					 }
					 break;
				 case WNDVoiceWnd:
					 {
						 if(m_pGameMain && m_pGameMain->m_pChat)
						 {
							 m_pGameMain->m_pChat->RenderVoiceChatOption();
						 }
					 }
					 break;
				case WNDMacroWnd:
					 {
						 if(m_pGameMain && m_pGameMain->m_pChat)
						 {							 
							 m_pGameMain->m_pChat->RenderMacro();
						 }
					 }
					 break;

				// 2009. 01. 12 by ckPark Ľ±Ŕü Ć÷°í ˝Ă˝şĹŰ
				case WNDWarDecalre:
					if(m_pGameMain && m_pGameMain->GetINFMotherShipManager())
						m_pGameMain->GetINFMotherShipManager()->GetWarDeclareWindow()->Render();
					break;
				// end 2009. 01. 12 by ckPark Ľ±Ŕü Ć÷°í ˝Ă˝şĹŰ
					// 2009-02-13 by bhsohn żůµĺ ·©Ĺ· ˝Ă˝şĹŰ
				case WNDWorldRank: // żůµĺ ·©Ĺ· Ă˘ 
					{
						if(m_pGameMain)
						{
							m_pGameMain->RenderWorldRank();
						}
					}
					break;
					// end 2009-02-13 by bhsohn żůµĺ ·©Ĺ· ˝Ă˝şĹŰ
					// end 2008-07-10 by bhsohn ˝Ă˝şĹŰ Ă˘ żěĽ±ĽřŔ§ Ăß°ˇ
				}
			}			
			itWnd++;
		}
	}
}

int CInterface::GetWindowFirstOrder()
{
	vector<GameWindowWnd>::iterator itWnd = m_vecGameWindowWnd.begin();
	while(itWnd != m_vecGameWindowWnd.end())
	{
		if((*itWnd).nWindowOrder == WNDIndexEnd-1)
		{
			return (*itWnd).nWindowIndex;
		}
		itWnd++;
	}

	return -1;
}

void CInterface::SetShopItemUseSerch()
{
// 2007-08-09 by dgwoo »óÁˇŔĚ ´ÝČú¶§¸¸ ČŁĂâÇĎ´Â °É·Î ľĆ·ˇ°ˇ ÇĘżäľřŔ˝.
//	 if(g_pD3dApp->m_dwGameState == _SHOP)
//		 return;

	//////////////////////////////////////////////////////////////////////////			
	// »óÁˇ ľĆŔĚĹŰ Âřżë°Ë»ç (ş°·Î ČżŔ˛ŔűŔĚÁř ľĘŔ¸łŞ żĎş®ÇŃ °Ë»ç¸¦ Ŕ§ÇŘ »çżë)
	// ĂßČÄ ĽöÁ¤ żä¸Á
	// ydkim.05.12.1
	if(g_pGameMain->m_bUseShopItem == TRUE)
	{
//		if(m_pGameMain->m_nLeaveBuildingIndex == BUILDINGKIND_TUNING_COLOR)
//			return;

		CItemInfo* pTempItem = NULL;
		for(int i=0; i<SIZE_MAX_POS; i++)
		{
			if(g_pGameMain->m_nItemSourceNum[i] != 0)
			{
				// 2006-08-24 by ispark, ŔŻ´ĎĹ© łŃąö·Î şŻ°ć
//				pTempItem = g_pStoreData->FindItemInInventoryByItemNum( g_pGameMain->m_nItemSourceNum[i] );
				pTempItem = g_pStoreData->FindItemInInventoryByUniqueNumber( g_pGameMain->m_nItemSourceNum[i] );
				if(pTempItem)
				{
					g_pStoreData->UpdateWearItemData( pTempItem );
					if(i == POS_CENTER)
					{
						// 2006-02-17 by ispark
						int nArmorColor = max(1, pTempItem->ColorCode%100);
						g_pShuttleChild->SetInvenAmorColor(nArmorColor);
						g_pD3dApp->SetUnitArmorColorNum(nArmorColor);

	//					g_pD3dApp->SetUnitArmorColorNum(g_pGameMain->m_nArmorColor);					
					}
				}
			}
		}
		g_pGameMain->m_bUseShopItem = FALSE;
		g_pGameMain->ResetWearItem();
	}	
	//
	//////////////////////////////////////////////////////////////////////////
}

void CInterface::SetLeaveShopKind(int nLeaveBuildingIndex)
{
	m_pGameMain->m_nLeaveBuildingIndex = nLeaveBuildingIndex;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			void CInterface::OpenBazaarShop(ITEM* pSkillItem)
/// \brief		°łŔÎ »óÁˇ żŔÇÂ
/// \author		ispark
/// \date		2006-07-28 ~ 2006-07-28
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CInterface::OpenBazaarShop(ITEM* pSkillItem)
{
	int nBazaarShopType = 0;
	//if(IS_BAZAAR_SELL_SKILL(pSkillItem))
	// 2009-04-21 by bhsohn ľĆŔĚĹŰ DesParamĂß°ˇ
// 	if(IS_EXIST_DES_PARAM((pSkillItem), DES_BAZAAR_SELL))
// 	{
// 		nBazaarShopType = 1;
// 	}
// 	//else if(IS_BAZAAR_BUY_SKILL(pSkillItem))
// 	else if(IS_EXIST_DES_PARAM((pSkillItem), DES_BAZAAR_BUY))
// 	{
// 		nBazaarShopType = 2;
// 	}
	if(pSkillItem->IsExistDesParam(DES_BAZAAR_SELL))
	{
		nBazaarShopType = 1;
	}	
	else if(pSkillItem->IsExistDesParam(DES_BAZAAR_BUY))
	{
		nBazaarShopType = 2;
	}
	// end 2009-04-21 by bhsohn ľĆŔĚĹŰ DesParamĂß°ˇ
	g_pD3dApp->ChangeGameState(_SHOP);
	m_pBazaarShop = new CINFCityBazaarOpen(this, m_pGameMain->m_pGameData, nBazaarShopType);
	m_pBazaarShop->InitDeviceObjects();
	m_pBazaarShop->RestoreDeviceObjects();
	g_pGameMain->InitShowWindow();

	// by bhsohn 2007-03-12 China Working
#ifdef LANGUAGE_CHINA	
	//g_pD3dApp->DxSetDialogBoxMode(TRUE);			
#endif
	
	// 2008-08-22 by bhsohn EP3 ŔÎşĄĹä¸® Ăł¸®dd
	{
		POINT ptItem, ptEq;
		ptItem.x = ptItem.y = ptEq.x = ptEq.y = 0;

		ptItem.x = (CITY_BASE_NPC_BOX_START_X);
		ptItem.y = (CITY_BASE_NPC_BOX_START_Y-SIZE_NORMAL_WINDOW_Y);
		ptEq.x = (CITY_BASE_NPC_BOX_START_X+426);
		ptEq.y = (CITY_BASE_NPC_BOX_START_Y-2*SIZE_NORMAL_WINDOW_Y);

		m_pGameMain->m_pInven->ShowInven(&ptItem, &ptEq);	
	}
	// end 2008-08-22 by bhsohn EP3 ŔÎşĄĹä¸® Ăł¸®

}

///////////////////////////////////////////////////////////////////////////////
/// \fn			void CInterface::CloseBazaarShop()
/// \brief		°łŔÎ»óÁˇ Ĺ¬·ÎÁî
/// \author		ispark
/// \date		2006-07-28 ~ 2006-07-28
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CInterface::CloseBazaarShop()
{
	if( _SHOP == g_pD3dApp->GetGameState() )
	{
		g_pD3dApp->ChangeGameState(_GAME);
	}
	//g_pD3dApp->ChangeGameState(_GAME);

	if(m_pBazaarShop)
	{
		m_pBazaarShop->InvalidateDeviceObjects();
		m_pBazaarShop->DeleteDeviceObjects();
		SAFE_DELETE(m_pBazaarShop);
	}

	g_pCharacterChild->m_nSelBazaarType = 0;
	g_pCharacterChild->m_ShopEnemyClientIndex = 0;

	// by bhsohn 2007-03-12 China Working
#ifdef LANGUAGE_CHINA	
	//g_pD3dApp->DxSetDialogBoxMode(FALSE);
#endif
	// 2008-08-22 by bhsohn EP3 ŔÎşĄĹä¸® Ăł¸®
	g_pGameMain->m_pInven->ShowInven(NULL, NULL);
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			void CInterface::VisitBazaarShop()
/// \brief		ąćą®ŔÚ 
/// \author		ispark
/// \date		2006-07-29 ~ 2006-07-29
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CInterface::VisitBazaarShop()
{
	if(m_pBazaarShop)
	{
		// ąćą®ŔÚ ľĆŔĚĹŰ °»˝Ĺ
		m_pBazaarShop->InitVisitData();
	}
	else
	{
		// ąćą®ŔÚ »óÁˇ ż­±â
		g_pD3dApp->ChangeGameState(_SHOP);
		m_pBazaarShop = new CINFCityBazaarVisit(this, m_pGameMain->m_pGameData, g_pCharacterChild->m_nSelBazaarType, g_pCharacterChild->m_ShopEnemyClientIndex);
		m_pBazaarShop->InitDeviceObjects();
		m_pBazaarShop->RestoreDeviceObjects();
		g_pGameMain->InitShowWindow();
	}
	// 2008-08-22 by bhsohn EP3 ŔÎşĄĹä¸® Ăł¸®dd
	{
		POINT ptItem, ptEq;
		ptItem.x = ptItem.y = ptEq.x = ptEq.y = 0;

		ptItem.x = (CITY_BASE_NPC_BOX_START_X);
		ptItem.y = (CITY_BASE_NPC_BOX_START_Y-SIZE_NORMAL_WINDOW_Y);
		ptEq.x = (CITY_BASE_NPC_BOX_START_X+426);
		ptEq.y = (CITY_BASE_NPC_BOX_START_Y-2*SIZE_NORMAL_WINDOW_Y);

		m_pGameMain->m_pInven->ShowInven(&ptItem, &ptEq);	
	}
	// end 2008-08-22 by bhsohn EP3 ŔÎşĄĹä¸® Ăł¸®
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			void CInterface::SetBazaarLogInfo(BYTE byLogState)
/// \brief		·Î±× Á¤ş¸ ŔÔ·Â
/// \author		ispark
/// \date		2006-07-29 ~ 2006-07-29
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CInterface::SetBazaarLogInfo(BYTE byLogState)
{
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			void CInterface::ReadyCloseBazaarShop(BOOL bErr)
/// \brief		
/// \author		ispark
/// \date		2006-07-30 ~ 2006-07-30
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CInterface::ReadyCloseBazaarShop(BOOL bErr)
{
	vector<CSkillInfo*>::iterator itSkillInfo = g_pShuttleChild->m_pSkill->m_vecUsingSkill.begin();
	while(itSkillInfo != g_pShuttleChild->m_pSkill->m_vecUsingSkill.end())
	{
		// ˝şĹłŔ» »çżëÇŃ »óÁˇ żŔÇÂÇüŔĚ¸é Ľ­ąöżˇ°Ô ˝şĹł ĂëĽŇ¸¦ ş¸ł˝´Ů.
		if(IS_BAZAAR_SKILL((*itSkillInfo)->ItemInfo))
		{
			m_pGameMain->m_pCharacterInfo->SendUseSkill((ITEM_BASE*)(*itSkillInfo));
			return;
		}

		itSkillInfo++;
	}

	// Ŕ§ÂĘŔş »óÁˇ żŔÇÂŔĚ¸é ŔĚ şÎşĐŔş »óÁˇ ąćą®ŔÚŔĚ´Ů.
	// »ó´ëąćŔĚ żˇ·Żżˇ ŔÇÇŃ °ÍŔĚ¶ó¸é ¸»ÇłĽ± »čÁ¦
	if(bErr == TRUE)
	{
		map<INT,CEnemyData *>::iterator itEnemy = g_pScene->m_mapEnemyList.find(g_pCharacterChild->m_ShopEnemyClientIndex);

		if(itEnemy != g_pScene->m_mapEnemyList.end())
		{
			itEnemy->second->DeleteChatMoveShop();
		}
	}
	
	CloseBazaarShop();

}

///////////////////////////////////////////////////////////////////////////////
/// \fn			void CInterface::CloseVisitShop()
/// \brief		ąŮŔÚ »óÁˇ ´Ý±â
/// \author		// 2007-11-01 by bhsohn »óÁˇ ŔĚżëÁßżˇ ´ëÇŃ Ăł¸®
/// \date		2007-11-01 ~ 2007-11-01
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CInterface::CloseVisitShop()
{
	if(NULL == m_pBazaarShop)
	{
		return;
	}
	m_pBazaarShop->CloseVisitShop();
}


///////////////////////////////////////////////////////////////////////////////
/// \fn			void CInterface::ErrBazaarItemList()
/// \brief		ľĆŔĚĹŰ ¸®˝şĆ® °»˝Ĺ ÁŘşń
/// \author		ispark
/// \date		2006-08-02 ~ 2006-08-02
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CInterface::ErrBazaarItemList()
{
	map<INT,CEnemyData *>::iterator itEnemy = g_pScene->m_mapEnemyList.find(g_pCharacterChild->m_ShopEnemyClientIndex);

	if(itEnemy != g_pScene->m_mapEnemyList.end() )
	{
		SendBazaarRequestItemList(itEnemy->second, TRUE);
	}
	else
	{
		// »ó´ëĆíŔĚ Á¸Ŕç ÇĎÁö ľĘŔ˝
		ReadyCloseBazaarShop();
	}
}
///////////////////////////////////////////////////////////////////////////////
/// \fn			BOOL CInterface::SendBazaarRequestItemList(CEnemyData * pEnemy, BOOL bFlag)
/// \brief		
/// \author		ispark
/// \date		2006-08-02 ~ 2006-08-02
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CInterface::SendBazaarRequestItemList(CEnemyData * pEnemy, BOOL bFlag)
{
	if(pEnemy->m_byBazaarType == 1)
	{
		// 2008-07-07 by bhsohn 2Â÷ ĆĐ˝şżöµĺ Č°ĽşČ­˝Ă, ŔÎşĄ ľČż­¸®°Ô ÇÔ
		if(g_pGameMain->IsUseSecondPassword() && bFlag)
		{
			// Lock»óĹÂżˇĽ­ ąŮŔÚ »óÁˇ ľČż­¸®°ÔÇÔ.
			char ErrorMsgMissionList[256];
			wsprintf(ErrorMsgMissionList, STRMSG_C_070917_0213);
			g_pD3dApp->m_pChat->CreateChatChild(ErrorMsgMissionList, COLOR_ERROR); //"\\yş¸ľČ ˝Ă˝şĹŰŔĚ Č°ĽşČ­ µÇľú˝Ŕ´Ď´Ů."
			return TRUE;
		}
		// end 2008-07-07 by bhsohn 2Â÷ ĆĐ˝şżöµĺ Č°ĽşČ­˝Ă, ŔÎşĄ ľČż­¸®°Ô ÇÔ

		g_pCharacterChild->m_pSelBazaarShop = pEnemy;
		if(bFlag)
		{
			// 2006-07-29 by ispark, °łŔÎ»óÁˇ ¸®˝şĆ® żäĂ»
			MSG_FC_BAZAAR_SELL_REQUEST_ITEMLIST sMsg;
			sMsg.clientIndex0 = pEnemy->m_infoCharacter.CharacterInfo.ClientIndex;
			g_pFieldWinSocket->SendMsg(T_FC_BAZAAR_SELL_REQUEST_ITEMLIST, (char*)&sMsg, sizeof(sMsg));

			g_pCharacterChild->m_nSelBazaarType = pEnemy->m_byBazaarType;
			g_pCharacterChild->m_ShopEnemyClientIndex = pEnemy->m_infoCharacter.CharacterInfo.ClientIndex;
			g_pD3dApp->m_bRequestEnable = FALSE;			// ŔŔ´ä ¸ŢĽĽÁö ±â´Ů¸˛
			return TRUE;
		}
	}
	if(pEnemy->m_byBazaarType == 2)
	{
		// 2008-07-07 by bhsohn 2Â÷ ĆĐ˝şżöµĺ Č°ĽşČ­˝Ă, ŔÎşĄ ľČż­¸®°Ô ÇÔ
		if(g_pGameMain->IsUseSecondPassword()&&bFlag)
		{
			// Lock»óĹÂżˇĽ­ ąŮŔÚ »óÁˇ ľČż­¸®°ÔÇÔ.
			char ErrorMsgMissionList[256];
			wsprintf(ErrorMsgMissionList, STRMSG_C_070917_0213);
			g_pD3dApp->m_pChat->CreateChatChild(ErrorMsgMissionList, COLOR_ERROR); //"\\yş¸ľČ ˝Ă˝şĹŰŔĚ Č°ĽşČ­ µÇľú˝Ŕ´Ď´Ů."
			return TRUE;
		}
		// end 2008-07-07 by bhsohn 2Â÷ ĆĐ˝şżöµĺ Č°ĽşČ­˝Ă, ŔÎşĄ ľČż­¸®°Ô ÇÔ
		g_pCharacterChild->m_pSelBazaarShop = pEnemy;
		if(bFlag)
		{
			// 2006-07-29 by ispark, °łŔÎ»óÁˇ ¸®˝şĆ® żäĂ»
			MSG_FC_BAZAAR_BUY_REQUEST_ITEMLIST sMsg;
			sMsg.clientIndex0 = pEnemy->m_infoCharacter.CharacterInfo.ClientIndex;
			g_pFieldWinSocket->SendMsg(T_FC_BAZAAR_BUY_REQUEST_ITEMLIST, (char*)&sMsg, sizeof(sMsg));

			g_pCharacterChild->m_nSelBazaarType = pEnemy->m_byBazaarType;
			g_pCharacterChild->m_ShopEnemyClientIndex = pEnemy->m_infoCharacter.CharacterInfo.ClientIndex;
			g_pD3dApp->m_bRequestEnable = FALSE;			// ŔŔ´ä ¸ŢĽĽÁö ±â´Ů¸˛
			return TRUE;
		}
	}

	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			void CInterface::SetShutDownStart()
/// \brief		°ÔŔÓ °­Á¦ Áľ·á ˝ĂŔŰ
/// \author		ispark
/// \date		2006-08-04 ~ 2006-08-04
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CInterface::SetShutDownStart()
{
	g_pD3dApp->m_bGameShutDown			= TRUE;
	g_pD3dApp->m_fGameShutDownTime		= GAME_SHUTDOWN_TIME;

	char msgShutDown[1024] = {0,};
	wsprintf(msgShutDown, STRMSG_C_060804_0000, (int)g_pD3dApp->m_fGameShutDownTime);		// "%dĂĘ ČÄ Ľ­ąöżÍ Á˘ĽÓŔ» Áľ·á ÇŐ´Ď´Ů."
	g_pD3dApp->m_pChat->CreateChatChild(msgShutDown, COLOR_CHAT_ALL);
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			void CInterface::GameShutDownTick()
/// \brief		
/// \author		ispark
/// \date		2006-08-04 ~ 2006-08-04
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CInterface::GameShutDownTick()
{
	char msgShutDown[1024] = {0,};

	int nTimeFirst = (int)g_pD3dApp->m_fGameShutDownTime;
	g_pD3dApp->m_fGameShutDownTime -= g_pD3dApp->GetElapsedTime();
	int nTimeLast = (int)g_pD3dApp->m_fGameShutDownTime;

	if(g_pD3dApp->m_fGameShutDownTime <= 0)
	{
		// Áľ·á Ăł¸®
		// »óÁˇ
		if( m_pCityBase && m_pCityBase->GetCurrentBuildingNPC())	
		{
			m_pCityBase->CloseCurrentEnterBuilding();
		}
		// 1:1 °Ĺ·ˇ
		if(m_pGameMain && m_pGameMain->m_pTrade && m_pGameMain->m_nLeftWindowInfo == LEFT_WINDOW_TRANS)
		{
			m_pGameMain->m_pTrade->SendTradeCancel();
		}
		// °łŔÎ »óÁˇ
		if(m_pBazaarShop)
		{
			vector<CSkillInfo*>::iterator itSkillInfo = g_pShuttleChild->m_pSkill->m_vecUsingSkill.begin();
			while(itSkillInfo != g_pShuttleChild->m_pSkill->m_vecUsingSkill.end())
			{
				// ˝şĹłŔ» »çżëÇŃ »óÁˇ żŔÇÂÇüŔĚ¸é Áľ·á Çă¶ôŔ» ÇŃ´Ů.
				if(IS_BAZAAR_SKILL((*itSkillInfo)->ItemInfo))
				{
					(*itSkillInfo)->SetbApprovalEnd(TRUE);
					break;
				}

				itSkillInfo++;
			}			

			ReadyCloseBazaarShop();
		}

		if( g_pShuttleChild->InitCinemaCamera(PATTERN_CAMERA_GAME_END) == TRUE )
		{
			g_pD3dApp->StartFadeEffect(FALSE,1,D3DCOLOR_ARGB(0,0,0,0));
			g_pShuttleChild->m_nEventType = EVENT_GAME_END;
			g_pShuttleChild->ChangeUnitState( _WARP );
			g_pD3dApp->m_bGameShutDown = FALSE;			
			// 2007-08-27 by bhsohn Ľ­ąö Áľ·á˝Ă °ÔŔÓ Áľ·á
			g_pD3dApp->m_bClientQuit = TRUE;
			// end 2007-08-27 by bhsohn Ľ­ąö Áľ·á˝Ă °ÔŔÓ Áľ·á
		}
		return;
	}

	if(nTimeFirst - nTimeLast)
	{
		wsprintf(msgShutDown, STRMSG_C_INTERFACE_0037, nTimeLast);		// "°ÔŔÓÁľ·á±îÁö \\r%dĂĘ\\r ł˛ľŇ˝Ŕ´Ď´Ů."
		g_pD3dApp->m_pChat->CreateChatChild(msgShutDown, COLOR_CHAT_ALL);
	}
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			void CInterface::ScriptEndAct(BYTE byEndAct)
/// \brief		˝şĹ×ŔĚÁö ŔĚĆĺĆ® łˇłŞ°í ÇŘľßÇĎ´Â Çŕµż
/// \author		ispark
/// \date		2006-09-07 ~ 2006-09-07
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CInterface::ScriptEndAct(BYTE byEndAct)
{
	switch(byEndAct)
	{
	case TUTORIAL_SKIP:
		{
			if(m_pSelect)
			{
				m_pSelect->SendTutorialSkip();
			}
		}
		break;
	case TUTORIAL_COMPLETE:
		{
			if(g_pShuttleChild)
			{
				g_pD3dApp->ChangeGameState(_PROLOGUE);
				g_pShuttleChild->SendFieldSocketRequestEventObjectWarpIn((CObjectChild *)m_pStageEffectData->GetStageEffectData()->pWarpObject);
			}
		}
		break;
	case MISSION_PRE:
		{
			if(g_pQuestData && m_pStageEffectData->GetStageEffectData()->nMissionIndex != -1)
			{
				g_pQuestData->SendFieldSocketQuestRequestStart(m_pStageEffectData->GetStageEffectData()->nMissionIndex,0);	
			}
		}
		break;
		// 2007-10-01 by bhsohn ĂłŔ˝ Ć©Ĺä¸®ľó Áľ·á˝Ă ÇÁ·Ń·Î±× ľČłŞżŔ´Â Çö»ó Ăł¸®
	case TUTORIAL_CHART_END:
		{
			// ¸Ţ´ş ąöĆ°Ŕ» ´­·Ż Ć©Ĺä¸®ľó Áľ·á
			SendTutorialChartEnd();
		}
		break;
		// end 2007-10-01 by bhsohn ĂłŔ˝ Ć©Ĺä¸®ľó Áľ·á˝Ă ÇÁ·Ń·Î±× ľČłŞżŔ´Â Çö»ó Ăł¸®
	}
}

// 2007.04.24 by bhsohn China IME Working
BOOL CInterface::IsPermissionIME(HKL hkl)
{
#ifdef LANGUAGE_CHINA
	char szDesc[256];
	memset(szDesc, 0x00, 256);
	UINT uRet = ImmGetDescription(hkl, szDesc, 256);

	// Default
	if(strlen(szDesc) < 1)
	{
		return TRUE;
	}
			
	// MSPY 3.0
	char* pFind = strstr(szDesc, STRMSG_C_070313_0201);
	if(NULL != pFind)
	{
		return TRUE;
	}

	// 2007-06-15 by bhsohn China IME Working
	// MSPY 2007
//	pFind = strstr(szDesc, STRMSG_C_070503_0201);
//	if(NULL != pFind)
//	{
//		return TRUE;
//	}
	// 2007-06-18 by bhsohn China IME Working
	pFind = strstr(szDesc, STRMSG_C_070618_0201);
	if(NULL != pFind)
	{
		return TRUE;
	}
	
	pFind = strstr(szDesc, STRMSG_C_070503_0202);
	if(NULL != pFind)
	{
		return TRUE;
	}
	pFind = strstr(szDesc, STRMSG_C_070503_0203);
	if(NULL != pFind)
	{
		return TRUE;
	}
#endif
	return FALSE;

}

// 2007-05-11 by bhsohn ľĆ·ąłŞ ąö±×ĽöÁ¤
BOOL CInterface::IsArenaStart()
{
	// 2007-05-21 by bhsohn ľĆ·ąłŞ ¸ĘżˇĽ± ąĚĽÇĂ˘ Ĺ¬¸Ż ľČµÇ°Ô şŻ°ć	
	MAP_INFO* pMapInfo = g_pD3dApp->GetMyShuttleMapInfo();
	if(!IS_MAP_INFLUENCE_ARENA(pMapInfo->MapInfluenceType))
	{
		return FALSE;
	}
	return TRUE;
}
///////////////////////////////////////////////////////////////////////////////
/// \fn			BOOL IsBazarOpen();
/// \brief		
/// \author		// 2007-07-09 by bhsohn Ăâ°Ý°ú ąŮŔÚ»óÁˇ µż˝Ă »çżë˝Ă ą®Á¦Áˇ Ăł¸®
/// \date		2007-07-09 ~ 2007-07-09
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CInterface::IsBazarOpen()
{
	return m_pGameMain->m_pQuickSlot->IsBazarOpen();
	
}

// 2007-07-04 by bhsohn żŔĆŰ·ąŔĚĹÍ Ăß°ˇ
DataHeader * CInterface::FindResource_LoadOp(char* szRcName)
{
	if(NULL == m_pINFOpMain)
	{
		return NULL;
	}
	return m_pINFOpMain->FindResource(szRcName);
}
///////////////////////////////////////////////////////////////////////////////
/// \fn			BOOL IsBazarOpen();
/// \brief		
/// \author		// 2007-09-12 by bhsohn 2Â÷ ľĎČŁ ˝Ă˝şĹŰ ±¸Çö
/// \date		2007-09-12 ~ 2007-09-12
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
DataHeader * CInterface::FindResource_LoadSelect(char* szRcName)
{
	if(NULL == m_pSelect)
	{
		return NULL;
	}
	return m_pSelect->FindResource(szRcName);
}

// żŔĆä·ąŔĚĹÍ ¸Ţ˝ĂÁö
int CInterface::WndProcOperator(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(NULL == m_pINFOpMain)
	{
		return INF_MSGPROC_NORMAL;
	}
	return m_pINFOpMain->WndProc(uMsg, wParam, lParam);
}
void CInterface::ShowOpFirstInfluenceQuest(BYTE InfluenceType)
{
	if(INFLUENCE_TYPE_VCN == InfluenceType)
	{
		ShowOpUserHelper(TRUE, OPERATOR_USER_SEL_VCU, NULL);			
	}
	else if(INFLUENCE_TYPE_ANI == InfluenceType)
	{
		ShowOpUserHelper(TRUE, OPERATOR_USER_SEL_ANI, NULL);			
	}
}


void CInterface::ShowOpUserHelper(BOOL bShow, int nHelpMode, char* pTxt)
{
	if(NULL == m_pINFOpMain)
	{
		return ;
	}	
	m_pINFOpMain->ShowOpUserHelper(bShow, nHelpMode, pTxt);	
}
void CInterface::CloseFirstUserExplain()
{
	if(NULL == m_pINFOpMain)
	{
		return ;
	}	
	m_pINFOpMain->CloseFirstUserExplain();

}



// 2009. 01. 12 by ckPark Ľ±Ŕü Ć÷°í ˝Ă˝şĹŰ
void CInterface::ShowOpWarDeclare(BYTE Influence, ATUM_DATE_TIME MSWarStartTime, BOOL GiveUp, BYTE SelectCount)
{
	if(NULL == m_pINFOpMain)
	{
		return ;
	}

	m_pINFOpMain->ShowOpWarDeclare(Influence, MSWarStartTime, GiveUp, SelectCount);
}
// end 2009. 01. 12 by ckPark Ľ±Ŕü Ć÷°í ˝Ă˝şĹŰ



void CInterface::ShowOpMissionComplete(INT i_nExpOfCompensation, BYTE i_byBonusStatOfCompensation, vector<QUEST_PAY_ITEM_INFO> vecQuestPay)
{
	if(NULL == m_pINFOpMain)
	{
		return ;
	}	
	m_pINFOpMain->ShowOpMissionComplete(i_nExpOfCompensation, i_byBonusStatOfCompensation, vecQuestPay);

}

// 2007-07-04 by bhsohn żŔĆŰ·ąŔĚĹÍ Ăß°ˇ
void CInterface::AddInflWarMonster(char *pTxt, MapIndex_t	MapIndex, SHORT	MapInfluenceType, ATUM_DATE_TIME		CreateTime)
{
	// ĽĽ·ÂŔü ąßąß
	if(NULL == m_pINFOpMain)
	{
		return ;
	}	
	m_pINFOpMain->AddStrategyPoint(pTxt, MapIndex, MapInfluenceType, CreateTime);

}

void CInterface::ShowOpBossMonsterSummon(BYTE byBelligerence, int nHour, int nMinute)
{
	// ĽĽ·ÂŔü ąßąß
	if(NULL == m_pINFOpMain)
	{
		return ;
	}	
	m_pINFOpMain->ShowOpBossMonsterSummon(byBelligerence, nHour, nMinute);

}

void CInterface::DetroySummonMonster(MapIndex_t	MapIndex)
{
	if(NULL == m_pINFOpMain)
	{
		return;
	}
	m_pINFOpMain->DetroySummonMonster(MapIndex);
}
VOID CInterface::ShowOpWndTxt(char* pTxt)
{
	if(NULL == m_pINFOpMain)
	{
		return;
	}
	m_pINFOpMain->ShowOpWndTxt(pTxt);
}
void CInterface::ShowOpWnd(BOOL bShow)
{
	if(NULL == m_pINFOpMain)
	{
		return;
	}
	m_pINFOpMain->ShowOpWnd(bShow);
}

// ÇöŔç »óÁˇŔĚ ¶ě°í ŔÖłÄ
BOOL CInterface::IsBuildingShow()
{
	GUI_BUILDINGNPC* pBuild = m_pCityBase->GetCurrentBuildingNPC();
	if(NULL == pBuild)
	{
		return FALSE;
	}
	return TRUE;
}
// ¸Ţ´ş ąöĆ°Ŕ» ´­·Ż Ć©Ĺä¸®ľó Áľ·á
// 2007-10-01 by bhsohn ĂłŔ˝ Ć©Ĺä¸®ľó Áľ·á˝Ă ÇÁ·Ń·Î±× ľČłŞżŔ´Â Çö»ó Ăł¸®
void CInterface::SendTutorialChartEnd()
{
	g_pFieldWinSocket->SendMsg(T_FC_TUTORIAL_END, NULL, 0);
	g_pD3dApp->StartFadeEffect(TRUE,1,D3DCOLOR_ARGB(0,0,0,0));
}

///////////////////////////////////////////////////////////////////////////////
/// \fn   VOID CINFOpMain::ShowOpWndMultiTxt(char* pTxt)
/// \brief  
/// \author  // 2007-11-19 by bhsohn żŔĆŰ·ąŔĚĹÍ ż©·ŻÁŮ ĹŘ˝şĆ® Ăł¸®
/// \date  2007-11-19 ~ 2007-11-19
/// \warning 
///
/// \param  
/// \return  
///////////////////////////////////////////////////////////////////////////////
VOID CInterface::ShowOpWndMultiTxt(vector<string> *pvecEditText)
{
	if(NULL == m_pINFOpMain)
	{
		return;
	}
	m_pINFOpMain->ShowOpWndMultiTxt(pvecEditText);
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		// 2007-11-28 by bhsohn ˝şĹ©¸° Ľ¦ ¸đµĺ Ăß°ˇ
/// \author  
/// \date		2007-11-28 ~ 2007-11-28
/// \warning 
///
/// \param  
/// \return  
///////////////////////////////////////////////////////////////////////////////
BOOL CInterface::IsScreenShotMode()
{	
	return m_bScreenShotMode;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		// 2007-11-28 by bhsohn ˝şĹ©¸° Ľ¦ ¸đµĺ Ăß°ˇ
/// \author  
/// \date		2007-11-28 ~ 2007-11-28
/// \warning 
///
/// \param  
/// \return  
///////////////////////////////////////////////////////////////////////////////
void CInterface::SetScreenShotMode(BOOL bScreenShotMode)
{
	m_bScreenShotMode = bScreenShotMode;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			BOOL CInterface::SetGuildMark(UID32_t i_nGuildUID)
/// \brief		ż©´Ü ¸¶Ĺ©°ˇ °»˝ĹµÇľúŔ» °ćżě ĽÂĆĂ.
/// \author		dgwoo
/// \date		2007-12-07 ~ 2007-12-07
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CInterface::SetGuildMark(UID32_t i_nGuildUID)
{
	// ¸¶Ŕ» ŔÎĹÍĆäŔĚ˝şżˇĽ­ Ăł¸®µÇ´Â ÇÔĽö.
	if(m_pCityBase != NULL)
	{
		m_pCityBase->SetCityGuildMark(i_nGuildUID);
	}
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		ľĆ·ąłŞ ·ąµÚ »óĹÂ ĂĽĹ©
/// \author		// 2007-11-22 by bhsohn ľĆ·ąłŞ ĹëÇŐĽ­ąö
/// \date		2007-12-17 ~ 2007-12-17
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CInterface::IsArenaReady()
{
	if(NULL == m_pGameArena)
	{
		return FALSE;
	}
	return m_pGameArena->IsArenaReady();
}


///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		ľĆ·ąłŞ ·ąµÚ »óĹÂ ĂĽĹ©
/// \author		// 2007-11-22 by bhsohn ľĆ·ąłŞ ĹëÇŐĽ­ąö
/// \date		2007-12-17 ~ 2007-12-17
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CInterface::IsArenaGameState(int nState)
{
	if(NULL == m_pGameArena)
	{
		return FALSE;
	}
	return m_pGameArena->IsArenaGameState(nState);
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		ľĆ·ąłŞ ŔĚµż˝Ă Ă¤ĆĂÁ¤ş¸ •üľ÷
/// \author		// 2008-02-27 by bhsohn ľĆ·ąłŞ ŔĚµż˝Ă Ă¤ĆĂÁ¤ş¸ •üľ÷
/// \date		2008-02-27 ~ 2008-02-27
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CInterface::BackupChatInfo()
{
	if(m_pGameMain)
	{
		m_pGameMain->GetAllChatList(m_chatBackup);
	}
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		ľĆ·ąłŞ ŔĚµż˝Ă Ă¤ĆĂÁ¤ş¸ •üľ÷
/// \author		// 2008-02-27 by bhsohn ľĆ·ąłŞ ŔĚµż˝Ă Ă¤ĆĂÁ¤ş¸ •üľ÷
/// \date		2008-02-27 ~ 2008-02-27
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CInterface::RefreshChatInfo()
{
	int nCnt = 0;
	for(nCnt = 0; nCnt < CHAT_BUFFER_NUMBER;nCnt++)
	{
		if(NULL == m_chatBackup[nCnt].pszChatString)
		{
			continue;
		}
		// 2009-05-06 by bhsohn ľĆ·ąłŞ ŔĚµż˝Ă, Ă¤ĆĂ •üľ÷ ľČµÇ´Â ą®Á¦ ÇŘ°á
		//g_pD3dApp->m_pChat->CreateChatChild(m_chatBackup[nCnt].pszChatString, m_chatBackup[nCnt].dwColorType);
		g_pD3dApp->m_pChat->CreateChatChild(m_chatBackup[nCnt].pszChatString, 
													m_chatBackup[nCnt].dwColorType,
													m_chatBackup[nCnt].dwType);
		
		delete [] m_chatBackup[nCnt].pszChatString;
		m_chatBackup[nCnt].pszChatString = NULL;
		m_chatBackup[nCnt].dwColorType = 0;

		// 2009-05-06 by bhsohn ľĆ·ąłŞ ŔĚµż˝Ă, Ă¤ĆĂ •üľ÷ ľČµÇ´Â ą®Á¦ ÇŘ°á
		m_chatBackup[nCnt].dwType = 0;
	}	

}

///////////////////////////////////////////////////////////////////////////////
/// \fn			BOOL IsDoingStrategyWar()
/// \brief		°ĹÁˇŔü ÁßŔĚłÄ?
/// \author		// 2008-03-07 by bhsohn ĽĽ·ÂŔü˝Ă, ľĆ·ąłŞ ŔĚżë ¸řÇĎ°Ô şŻ°ć
/// \date		2008-03-07 ~ 2008-03-07
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CInterface::IsDoingStrategyWar()
{
	if(NULL == m_pINFOpMain)
	{
		return FALSE;
	}
	return m_pINFOpMain->IsDoingStrategyWar();

}

///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		ÇöŔç ÁřÇŕÁßŔÎ ¸đĽ±Ŕü Á¤ş¸ ľ÷µĄŔĚĆ®
/// \author		// 2008-04-22 by bhsohn ¸đĽ±/°ĹÁˇŔü ÁřÇŕÁßŔÎ Á¤ş¸µµ ÇĄ˝Ă ÇĎ°Ô şŻ°ć
/// \date		2008-04-22 ~ 2008-04-22
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CInterface::UpdateShoptPlayingSPWarInfo()
{
	if(NULL == m_pINFOpMain)
	{
		return;
	}
	GUI_BUILDINGNPC* pBuilding = g_pInterface->m_pCityBase->GetCurrentBuildingNPC();
	if(NULL == pBuilding)
	{
		return;
	}
	int nBuildingKind = pBuilding->buildingInfo.BuildingKind;
	// 2008-10-16 by bhsohn ŔüÁř±âÁö ¸đĽ±Ŕü, °ĹÁˇŔü Á¤ş¸Ă˘
//	if(BUILDINGKIND_CITYLEADER_LEADER != nBuildingKind)
//	{
//		return;
//	}
	if((BUILDINGKIND_CITYLEADER_LEADER != nBuildingKind)
		&&(BUILDINGKIND_CITYLEADER_OUTPOST != nBuildingKind))	
	{
		return;
	}
	
	CMapCityShopIterator it = g_pInterface->m_pCityBase->m_mapCityShop.find(nBuildingKind);
	if(it == g_pInterface->m_pCityBase->m_mapCityShop.end() || NULL == it->second)
	{
		return;
	}
	
	// 2008-10-16 by bhsohn ŔüÁř±âÁö ¸đĽ±Ŕü, °ĹÁˇŔü Á¤ş¸Ă˘
	//CINFCityLeader * pCityLeader = ((CINFCityLeader*)it->second);
	int nCnt =0;

	MapIndex_t	MapIndex;
	ATUM_DATE_TIME timeSummon, timeSummonEnd;
	short MapInfluenceType;
	BYTE AttInfluence;

	char buf[256];
	memset(buf, 0x00, 256);	

	MapIndex = 0;
	memset(&timeSummon, 0x00, sizeof(ATUM_DATE_TIME));
	memset(&timeSummonEnd, 0x00, sizeof(ATUM_DATE_TIME));
	MapInfluenceType = 0;

	for(nCnt =0;nCnt < m_pINFOpMain->GetStrategyWarCnt();nCnt++)
	{
		m_pINFOpMain->GetStrategyWarInfo(nCnt, &MapIndex, &timeSummon, &MapInfluenceType);

		// ¸ĘŔĚ¸§ 
		memset(buf, 0x00, 256);
		char* pMapName = g_pGameMain->GetMapNameByIndex(MapIndex);	
		if(pMapName)
		{		
			STRNCPY_MEMSET(buf,pMapName,SIZE_MAX_MAP_NAME);
			
		}

		// °ř°Ý ĽĽ·Â
		AttInfluence = INFLUENCE_TYPE_VCN;
		if(IS_MAP_INFLUENCE_VCN(MapInfluenceType))
		{
			// ¸ĘĽĽ·Â°ú ąÝ´ë°ˇ °ř°Ý ĽĽ·ÂŔĚ´Ů.
			AttInfluence = INFLUENCE_TYPE_ANI;
		}
		if(BUILDINGKIND_CITYLEADER_LEADER == nBuildingKind)
		{
			CINFCityLeader * pCityLeader = ((CINFCityLeader*)it->second);
			pCityLeader->AddWarInfoPointWar(AttInfluence,	// °ř°ÝĽĽ·Â
						0,		// ˝Â¸®ĽĽ·Â(0:ÁřÇŕÁß)
						buf,				// ¸ĘŔĚ¸§											
						timeSummon,	// ˝ĂŔŰ˝Ă°Ł
						timeSummonEnd);	//Áľ·á˝Ă°Ł	
		}
		else if(BUILDINGKIND_CITYLEADER_OUTPOST == nBuildingKind)
		{
			CINFCityOutPost * pOutPost = ((CINFCityOutPost*)it->second);
			pOutPost->AddWarInfoPointWar(AttInfluence,	// °ř°ÝĽĽ·Â
						0,		// ˝Â¸®ĽĽ·Â(0:ÁřÇŕÁß)
						buf,				// ¸ĘŔĚ¸§											
						timeSummon,	// ˝ĂŔŰ˝Ă°Ł
						timeSummonEnd);	//Áľ·á˝Ă°Ł	

		}
	}
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		
/// \author		// 2008-04-22 by bhsohn ¸đĽ±/°ĹÁˇŔü ÁřÇŕÁßŔÎ Á¤ş¸µµ ÇĄ˝Ă ÇĎ°Ô şŻ°ć
/// \date		2008-04-22 ~ 2008-04-22
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CInterface::RefreshWarSPWarInfo()
{
	GUI_BUILDINGNPC* pBuilding = g_pInterface->m_pCityBase->GetCurrentBuildingNPC();
	if((NULL != pBuilding)
		&&(BUILDINGKIND_CITYLEADER_LEADER == pBuilding->buildingInfo.BuildingKind))
	{
		// »óÁˇŔĚ ¶°ŔÖ°í °ĹÁˇŔĚ şÎĽĹÁłŔ¸ąÇ·Î ´Ů˝Ă żäĂ»ÇŃ´Ů.
		CMapCityShopIterator it = g_pInterface->m_pCityBase->m_mapCityShop.find(pBuilding->buildingInfo.BuildingKind);
		CINFCityLeader * pCityOutPost = ((CINFCityLeader*)it->second);
		if(pCityOutPost)
		{
			pCityOutPost->InitWarInfoPointWar();
			pCityOutPost->RqPointWarDB();		
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		
/// \author		// 2008-04-22 by bhsohn ¸đĽ±/°ĹÁˇŔü ÁřÇŕÁßŔÎ Á¤ş¸µµ ÇĄ˝Ă ÇĎ°Ô şŻ°ć
/// \date		2008-04-22 ~ 2008-04-22
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CInterface::RefreshWarMotherWarInfo()
{
	GUI_BUILDINGNPC* pBuilding = g_pInterface->m_pCityBase->GetCurrentBuildingNPC();
	if((NULL != pBuilding)
		&&(BUILDINGKIND_CITYLEADER_LEADER == pBuilding->buildingInfo.BuildingKind))
	{
		// »óÁˇŔĚ ¶°ŔÖ°í °ĹÁˇŔĚ şÎĽĹÁłŔ¸ąÇ·Î ´Ů˝Ă żäĂ»ÇŃ´Ů.
		CMapCityShopIterator it = g_pInterface->m_pCityBase->m_mapCityShop.find(pBuilding->buildingInfo.BuildingKind);
		CINFCityLeader * pCityOutPost = ((CINFCityLeader*)it->second);
		if(pCityOutPost)
		{
			pCityOutPost->InitWarInfoMothership();
			pCityOutPost->RqMotherShipDB();					
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		
/// \author		// 2008-06-20 by bhsohn EP3 żÉĽÇ°ü·Ă Ăł¸®
/// \date		2008-06-20 ~ 2008-06-20
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
int CInterface::GetEtcOptionInfo(int nIdx)
{
	if(nIdx < 0 || nIdx >= MAX_OPTION_RADIO)
	{
		return 0;
	}
	char chSetupInfo[256], chBuf[512];		
	memset(chSetupInfo, 0x00, 256);
	memset(chBuf, 0x00 ,512);		
	
	GetEtcOptionIdx_To_String(nIdx, chSetupInfo);

	if(m_pSetupConfig->GetSetupInfo(chSetupInfo, chBuf))
	{
		return atoi(chBuf);	
	}
	
	// ľřŔ»¶§´Â ŔúŔĺ ˝Ăµµ
	sOPTION_ETC	stpEtcoption;
	memset(&stpEtcoption, 0x00, sizeof(sOPTION_ETC));
	g_pD3dApp->InitOptionEtcInfo(&stpEtcoption);

	SetEtcOptionInfo(chSetupInfo, stpEtcoption.bRadioInfo[nIdx]);

	// ŔúŔĺ
	{
		char chMaxPath[MAX_PATH];
		memset(chMaxPath, 0x00, MAX_PATH);
		wsprintf(chMaxPath, SETUP_INFO_PATH);
		m_pSetupConfig->SaveSetupInfo(chMaxPath);		
	}
	return stpEtcoption.bRadioInfo[nIdx];	
}
///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		
/// \author		// 2008-06-20 by bhsohn EP3 żÉĽÇ°ü·Ă Ăł¸®
/// \date		2008-06-20 ~ 2008-06-20
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////


// 2009. 02. 03 by ckPark Ă¤ĆĂĂ˘ ŔÎĹÍĆäŔĚ˝ş Ĺ©±â ŔúŔĺ
// void CInterface::GetEtcOptionInterPosValue(float* fRateX, float* fRateY, 
// 										   float* fRateWidth, float* fRateHeight, 
// 										   float* fGameWidth, float* fGameHeight)
void CInterface::GetEtcOptionInterPosValue(float* fRateX, float* fRateY,
										   float* fRateWidth, float* fRateHeight,
										   float* fGameWidth, float* fGameHeight,
										   float* fChatWidth, float* fChatHeight)
// end 2009. 02. 03 by ckPark Ă¤ĆĂĂ˘ ŔÎĹÍĆäŔĚ˝ş Ĺ©±â ŔúŔĺ

{	
	char chBuf[512];			
	memset(chBuf, 0x00 ,512);			

	BOOL bDefault = FALSE;
	// ľřŔ»¶§´Â ŔúŔĺ ˝Ăµµ
	sOPTION_ETC	stpEtcoption;
	memset(&stpEtcoption, 0x00, sizeof(sOPTION_ETC));
	g_pD3dApp->InitOptionEtcInfo(&stpEtcoption);

	// X°Ş
	if(m_pSetupConfig->GetSetupInfo(SETUP_INFO_OPETC_INTERPOS_X_INFO, chBuf))
	{
		(*fRateX) = atof(chBuf);	
	}	
	else
	{
		bDefault = TRUE;
		(*fRateX) = stpEtcoption.stRateRect.fRateX;	
		
		sprintf(chBuf, "%.2f", stpEtcoption.stRateRect.fRateX);
		m_pSetupConfig->SetSetupInfo(SETUP_INFO_OPETC_INTERPOS_X_INFO, chBuf);
	}

	// Y°Ş
	if(m_pSetupConfig->GetSetupInfo(SETUP_INFO_OPETC_INTERPOS_Y_INFO, chBuf))
	{
		(*fRateY) = atof(chBuf);	
	}	
	else
	{
		bDefault = TRUE;
		(*fRateY) = stpEtcoption.stRateRect.fRateY;	

		sprintf(chBuf, "%.2f", stpEtcoption.stRateRect.fRateY);
		m_pSetupConfig->SetSetupInfo(SETUP_INFO_OPETC_INTERPOS_Y_INFO, chBuf);
	}

	// Width
	if(m_pSetupConfig->GetSetupInfo(SETUP_INFO_OPETC_INTERPOS_W_INFO, chBuf))
	{
		(*fRateWidth) = atof(chBuf);	
	}	
	else
	{
		bDefault = TRUE;
		(*fRateWidth) = stpEtcoption.stRateRect.fRateWidth;	

		sprintf(chBuf, "%.2f", stpEtcoption.stRateRect.fRateWidth);
		m_pSetupConfig->SetSetupInfo(SETUP_INFO_OPETC_INTERPOS_W_INFO, chBuf);
	}

	// Height
	if(m_pSetupConfig->GetSetupInfo(SETUP_INFO_OPETC_INTERPOS_H_INFO, chBuf))
	{
		(*fRateHeight) = atof(chBuf);	
	}	
	else
	{
		bDefault = TRUE;
		(*fRateHeight) = stpEtcoption.stRateRect.fRateHeight;	

		sprintf(chBuf, "%.2f", stpEtcoption.stRateRect.fRateHeight);
		m_pSetupConfig->SetSetupInfo(SETUP_INFO_OPETC_INTERPOS_H_INFO, chBuf);
	}


	// 2009. 02. 03 by ckPark Ă¤ĆĂĂ˘ ŔÎĹÍĆäŔĚ˝ş Ĺ©±â ŔúŔĺ
	
	// Width
	if(m_pSetupConfig->GetSetupInfo(SETUP_INFO_OPETC_CHATPOS_W_INFO, chBuf))
	{
		(*fChatWidth) = atof(chBuf);	
	}	
	else
	{
		bDefault = TRUE;
		(*fChatWidth) = stpEtcoption.stChatRect.fRateWidth;	
		
		sprintf(chBuf, "%.2f", stpEtcoption.stChatRect.fRateWidth);
		m_pSetupConfig->SetSetupInfo(SETUP_INFO_OPETC_CHATPOS_W_INFO, chBuf);
	}
	
	// Height
	if(m_pSetupConfig->GetSetupInfo(SETUP_INFO_OPETC_CHATPOS_H_INFO, chBuf))
	{
		(*fChatHeight) = atof(chBuf);	
	}	
	else
	{
		bDefault = TRUE;
		(*fChatHeight) = stpEtcoption.stChatRect.fRateHeight;	
		
		sprintf(chBuf, "%.2f", stpEtcoption.stChatRect.fRateHeight);
		m_pSetupConfig->SetSetupInfo(SETUP_INFO_OPETC_CHATPOS_H_INFO, chBuf);
	}

	// end 2009. 02. 03 by ckPark Ă¤ĆĂĂ˘ ŔÎĹÍĆäŔĚ˝ş Ĺ©±â ŔúŔĺ


	// GameWidth
	if(m_pSetupConfig->GetSetupInfo(SETUP_INFO_OPETC_GAME_W_INFO, chBuf))
	{
		(*fGameWidth) = atof(chBuf);
	}	
	else
	{
		bDefault = TRUE;
		(*fGameWidth) = stpEtcoption.stRateRect.fGameWidth;	

		sprintf(chBuf, "%.2f", stpEtcoption.stRateRect.fGameWidth);
		m_pSetupConfig->SetSetupInfo(SETUP_INFO_OPETC_GAME_W_INFO, chBuf);
	}

	// GameHeight
	if(m_pSetupConfig->GetSetupInfo(SETUP_INFO_OPETC_GAME_H_INFO, chBuf))
	{
		(*fGameHeight) = atof(chBuf);	
	}	
	else
	{
		bDefault = TRUE;
		(*fGameHeight) = stpEtcoption.stRateRect.fGameHeight;	

		sprintf(chBuf, "%.2f", stpEtcoption.stRateRect.fGameHeight);
		m_pSetupConfig->SetSetupInfo(SETUP_INFO_OPETC_GAME_H_INFO, chBuf);
	}
	
		
	// ŔúŔĺ
	if(bDefault)
	{
		char chMaxPath[MAX_PATH];
		memset(chMaxPath, 0x00, MAX_PATH);
		wsprintf(chMaxPath, SETUP_INFO_PATH);
		m_pSetupConfig->SaveSetupInfo(chMaxPath);		
	}	
}
///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		
/// \author		// 2008-06-20 by bhsohn EP3 żÉĽÇ°ü·Ă Ăł¸®
/// \date		2008-06-20 ~ 2008-06-20
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CInterface::SetEtcOptionInfo(char* pSetupInfo, int nMode)
{	
	if(nMode != 0 && nMode !=1 )
	{
		return;
	}
	char chBuf[512];
	wsprintf(chBuf, "%d", nMode);
	m_pSetupConfig->SetSetupInfo(pSetupInfo, chBuf);
}
///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		
/// \author		// 2008-06-20 by bhsohn EP3 żÉĽÇ°ü·Ă Ăł¸®
/// \date		2008-06-20 ~ 2008-06-20
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CInterface::GetEtcOptionIdx_To_String(int nIdx, char* pDst)
{
	switch(nIdx)
	{
	case OPTION_RADIO_MAN_TO_MAN:	// 1:1ŔüĹő
		{
			wsprintf(pDst, SETUP_INFO_OPETC_MANTOMAN_INFO);
		}
		break;
	case OPTION_RADIO_FORMATIONWAR:	// Ćí´ëŔüĹő
		{
			wsprintf(pDst, SETUP_INFO_OPETC_PARTYWAR_INFO);
		}
		break;
	case OPTION_RADIO_GUILD_WAR:	// ż©´ÜŔüĹő
		{
			wsprintf(pDst, SETUP_INFO_OPETC_GUILDWAR_INFO);
		}
		break;
	case OPTION_RADIO_WHISPER:		// ±ÓĽÓ¸»
		{
			wsprintf(pDst, SETUP_INFO_OPETC_WHISPER_INFO);
		}
		break;
	case OPTION_RADIO_FORMATION_INVITE:	// Ćí´ëĂĘ´ë
		{
			wsprintf(pDst, SETUP_INFO_OPETC_PARTYINVI_INFO);
		}
		break;
	case OPTION_RADIO_GUILD_INVITE:		// ż©´ÜĂĘ´ë
		{
			wsprintf(pDst, SETUP_INFO_OPETC_GUILDINVI_INFO);
		}
		break;
	case OPTION_RADIO_EXCHANGE:			// ±łČŻ
		{
			wsprintf(pDst, SETUP_INFO_OPETC_EXCHANGE_INFO);
		}
		break;
	case OPTION_RADIO_INTERFACE_POS:	// ŔÎĹÍĆäŔĚ˝ş Ŕ§Äˇ ŔúŔĺ
		{
			wsprintf(pDst, SETUP_INFO_OPETC_INTERPOS_INFO);
		}
		break;
	case OPTION_RADIO_INTERFACE_HIDE:	// ŔÎĹÍĆäŔĚ˝ş ¸đµÎ °¨Ăß±â
		{
			wsprintf(pDst, SETUP_INFO_OPETC_INTERHIDE_INFO);
		}
		break;
	case OPTION_RADIO_FRIEND_ID_HIDE:	// °°Ŕş ĽĽ·Â ľĆŔĚµđ °¨Ăß±â
		{
			wsprintf(pDst, SETUP_INFO_OPETC_SAME_ID_HIDE_INFO);
		}
		break;
	case OPTION_RADIO_ENEMY_ID_HIDE:	// ´Ů¸Ą ĽĽ·Â ľĆŔĚµđ °¨Ăß±â
		{
			wsprintf(pDst, SETUP_INFO_OPETC_ENEMY_ID_HIDE_INFO);
		}
		break;
	case OPTION_RADIO_MOUSE_LOCK:		// ¸¶żě˝ş °ˇµÎ±â(Ŕ©µµżě ¸đµĺ˝Ă)
		{
			wsprintf(pDst, SETUP_INFO_OPETC_MOUSE_LOCK_INFO);
		}
		break;
	case OPTION_RADIO_CHAT_FILTER:		// żĺĆŢĹÍ
		{
			wsprintf(pDst, SETUP_INFO_OPETC_FILTER_INFO);
		}
		break;
	default:
		{
			wsprintf(pDst, "");
		}
		break;
	}

}

///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		
/// \author		// 2008-06-20 by bhsohn EP3 żÉĽÇ°ü·Ă Ăł¸®
/// \date		2008-06-20 ~ 2008-06-20
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CInterface::LoadOptionEtcInfo(sOPTION_ETC	*pEtcOption)
{
	// 1:1ąřĹő
	pEtcOption->bRadioInfo[OPTION_RADIO_MAN_TO_MAN] = GetEtcOptionInfo(OPTION_RADIO_MAN_TO_MAN);	
	
	// Ćí´ë ŔüĹő
	pEtcOption->bRadioInfo[OPTION_RADIO_FORMATIONWAR] = GetEtcOptionInfo(OPTION_RADIO_FORMATIONWAR);		
	
	// ż©´ÜŔüĹő
	pEtcOption->bRadioInfo[OPTION_RADIO_GUILD_WAR] = GetEtcOptionInfo(OPTION_RADIO_GUILD_WAR);			
	
	//±ÓĽÓ¸»
	pEtcOption->bRadioInfo[OPTION_RADIO_WHISPER] = GetEtcOptionInfo(OPTION_RADIO_WHISPER);			
	
	// Ćí´ë ĂĘ´ë
	pEtcOption->bRadioInfo[OPTION_RADIO_FORMATION_INVITE] = GetEtcOptionInfo(OPTION_RADIO_FORMATION_INVITE);				
	
	// ż©´ÜĂĘ´ë
	pEtcOption->bRadioInfo[OPTION_RADIO_GUILD_INVITE] = GetEtcOptionInfo(OPTION_RADIO_GUILD_INVITE);					
	
	// ±łČŻ
	pEtcOption->bRadioInfo[OPTION_RADIO_EXCHANGE] = GetEtcOptionInfo(OPTION_RADIO_EXCHANGE);						
	
	//ŔÎĹÍĆäŔĚ˝ş Ŕ§ÄˇŔúŔĺ
	pEtcOption->bRadioInfo[OPTION_RADIO_INTERFACE_POS] = GetEtcOptionInfo(OPTION_RADIO_INTERFACE_POS);						


	// 2009. 02. 03 by ckPark Ă¤ĆĂĂ˘ ŔÎĹÍĆäŔĚ˝ş Ĺ©±â ŔúŔĺ
// 	GetEtcOptionInterPosValue(&pEtcOption->stRateRect.fRateX, &pEtcOption->stRateRect.fRateY, 
// 								&pEtcOption->stRateRect.fRateWidth, &pEtcOption->stRateRect.fRateHeight,
// 								&pEtcOption->stRateRect.fGameWidth, &pEtcOption->stRateRect.fGameHeight);
	GetEtcOptionInterPosValue(&pEtcOption->stRateRect.fRateX, &pEtcOption->stRateRect.fRateY, 
								&pEtcOption->stRateRect.fRateWidth, &pEtcOption->stRateRect.fRateHeight,
								&pEtcOption->stRateRect.fGameWidth, &pEtcOption->stRateRect.fGameHeight,
								&pEtcOption->stChatRect.fRateWidth, &pEtcOption->stChatRect.fRateHeight);
	// end 2009. 02. 03 by ckPark Ă¤ĆĂĂ˘ ŔÎĹÍĆäŔĚ˝ş Ĺ©±â ŔúŔĺ
	
	
	// ŔÎĹÍĆäŔĚ˝ş ¸đµÎ °¨Ăß±â
	pEtcOption->bRadioInfo[OPTION_RADIO_INTERFACE_HIDE] = GetEtcOptionInfo(OPTION_RADIO_INTERFACE_HIDE);
	
	// °°Ŕş ĽĽ·Â ľĆŔĚµđ
	pEtcOption->bRadioInfo[OPTION_RADIO_FRIEND_ID_HIDE] = GetEtcOptionInfo(OPTION_RADIO_FRIEND_ID_HIDE);	
	
	// ´Ů¸Ą ĽĽ·Â ľĆŔĚµđ
	pEtcOption->bRadioInfo[OPTION_RADIO_ENEMY_ID_HIDE] = GetEtcOptionInfo(OPTION_RADIO_ENEMY_ID_HIDE);		
	
	// ¸¶żě˝ş °ˇµÎ±â
	pEtcOption->bRadioInfo[OPTION_RADIO_MOUSE_LOCK] = GetEtcOptionInfo(OPTION_RADIO_MOUSE_LOCK);			
	
	// şńĽÓľî ÇĘĹÍ¸µ		
	pEtcOption->bRadioInfo[OPTION_RADIO_CHAT_FILTER] = GetEtcOptionInfo(OPTION_RADIO_CHAT_FILTER);			
	
	
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		
/// \author		// 2008-06-20 by bhsohn EP3 żÉĽÇ°ü·Ă Ăł¸®
/// \date		2008-06-20 ~ 2008-06-20
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CInterface::SetOptionEtcInfo(sOPTION_ETC	*pEtcOption)
{
	char chSetupInfo[256], chBuf[512];		
	memset(chSetupInfo, 0x00, 256);
	memset(chBuf, 0x00 ,512);		
	
	// 1:1ŔüĹő
	GetEtcOptionIdx_To_String(OPTION_RADIO_MAN_TO_MAN, chSetupInfo);
	wsprintf(chBuf, "%d", pEtcOption->bRadioInfo[OPTION_RADIO_MAN_TO_MAN]);
	m_pSetupConfig->SetSetupInfo(chSetupInfo, chBuf);	
	
	// Ćí´ë ŔüĹő
	GetEtcOptionIdx_To_String(OPTION_RADIO_FORMATIONWAR, chSetupInfo);
	wsprintf(chBuf, "%d", pEtcOption->bRadioInfo[OPTION_RADIO_FORMATIONWAR]);
	m_pSetupConfig->SetSetupInfo(chSetupInfo, chBuf);		
	
	// ż©´ÜŔüĹő
	GetEtcOptionIdx_To_String(OPTION_RADIO_GUILD_WAR, chSetupInfo);
	wsprintf(chBuf, "%d", pEtcOption->bRadioInfo[OPTION_RADIO_GUILD_WAR]);
	m_pSetupConfig->SetSetupInfo(chSetupInfo, chBuf);			
	
	//±ÓĽÓ¸»
	GetEtcOptionIdx_To_String(OPTION_RADIO_WHISPER, chSetupInfo);
	wsprintf(chBuf, "%d", pEtcOption->bRadioInfo[OPTION_RADIO_WHISPER]);
	m_pSetupConfig->SetSetupInfo(chSetupInfo, chBuf);				
	
	// Ćí´ë ĂĘ´ë
	GetEtcOptionIdx_To_String(OPTION_RADIO_FORMATION_INVITE, chSetupInfo);
	wsprintf(chBuf, "%d", pEtcOption->bRadioInfo[OPTION_RADIO_FORMATION_INVITE]);
	m_pSetupConfig->SetSetupInfo(chSetupInfo, chBuf);					
	
	// ż©´ÜĂĘ´ë
	GetEtcOptionIdx_To_String(OPTION_RADIO_GUILD_INVITE, chSetupInfo);
	wsprintf(chBuf, "%d", pEtcOption->bRadioInfo[OPTION_RADIO_GUILD_INVITE]);
	m_pSetupConfig->SetSetupInfo(chSetupInfo, chBuf);
	
	// ±łČŻ
	GetEtcOptionIdx_To_String(OPTION_RADIO_EXCHANGE, chSetupInfo);
	wsprintf(chBuf, "%d", pEtcOption->bRadioInfo[OPTION_RADIO_EXCHANGE]);
	m_pSetupConfig->SetSetupInfo(chSetupInfo, chBuf);	
	
	//ŔÎĹÍĆäŔĚ˝ş Ŕ§ÄˇŔúŔĺ
	GetEtcOptionIdx_To_String(OPTION_RADIO_INTERFACE_POS, chSetupInfo);
	wsprintf(chBuf, "%d", pEtcOption->bRadioInfo[OPTION_RADIO_INTERFACE_POS]);
	m_pSetupConfig->SetSetupInfo(chSetupInfo, chBuf);	
	
	// ŔÎĹÍĆäŔĚ˝ş Ŕ§Äˇ 
	sprintf(chBuf, "%.2f", pEtcOption->stRateRect.fRateX);
	m_pSetupConfig->SetSetupInfo(SETUP_INFO_OPETC_INTERPOS_X_INFO, chBuf);		
	sprintf(chBuf, "%.2f", pEtcOption->stRateRect.fRateY);
	m_pSetupConfig->SetSetupInfo(SETUP_INFO_OPETC_INTERPOS_Y_INFO, chBuf);		
	
	sprintf(chBuf, "%.2f", pEtcOption->stRateRect.fRateWidth);
	m_pSetupConfig->SetSetupInfo(SETUP_INFO_OPETC_INTERPOS_W_INFO, chBuf);		
	sprintf(chBuf, "%.2f", pEtcOption->stRateRect.fRateHeight);
	m_pSetupConfig->SetSetupInfo(SETUP_INFO_OPETC_INTERPOS_H_INFO, chBuf);

	
	// 2009. 02. 03 by ckPark Ă¤ĆĂĂ˘ ŔÎĹÍĆäŔĚ˝ş Ĺ©±â ŔúŔĺ

	sprintf(chBuf, "%.2f", pEtcOption->stChatRect.fRateWidth);
	m_pSetupConfig->SetSetupInfo(SETUP_INFO_OPETC_CHATPOS_W_INFO, chBuf);
	sprintf(chBuf, "%.2f", pEtcOption->stChatRect.fRateHeight);
	m_pSetupConfig->SetSetupInfo(SETUP_INFO_OPETC_CHATPOS_H_INFO, chBuf);

	// end 2009. 02. 03 by ckPark Ă¤ĆĂĂ˘ ŔÎĹÍĆäŔĚ˝ş Ĺ©±â ŔúŔĺ


	sprintf(chBuf, "%.2f", pEtcOption->stRateRect.fGameWidth);
	m_pSetupConfig->SetSetupInfo(SETUP_INFO_OPETC_GAME_W_INFO, chBuf);		
	sprintf(chBuf, "%.2f", pEtcOption->stRateRect.fGameHeight);
	m_pSetupConfig->SetSetupInfo(SETUP_INFO_OPETC_GAME_H_INFO, chBuf);			
	
	// ŔÎĹÍĆäŔĚ˝ş ¸đµÎ °¨Ăß±â
	GetEtcOptionIdx_To_String(OPTION_RADIO_INTERFACE_HIDE, chSetupInfo);
	wsprintf(chBuf, "%d", pEtcOption->bRadioInfo[OPTION_RADIO_INTERFACE_HIDE]);
	m_pSetupConfig->SetSetupInfo(chSetupInfo, chBuf);			
	
	// °°Ŕş ĽĽ·Â ľĆŔĚµđ
	GetEtcOptionIdx_To_String(OPTION_RADIO_FRIEND_ID_HIDE, chSetupInfo);
	wsprintf(chBuf, "%d", pEtcOption->bRadioInfo[OPTION_RADIO_FRIEND_ID_HIDE]);
	m_pSetupConfig->SetSetupInfo(chSetupInfo, chBuf);				
	
	// ´Ů¸Ą ĽĽ·Â ľĆŔĚµđ
	GetEtcOptionIdx_To_String(OPTION_RADIO_ENEMY_ID_HIDE, chSetupInfo);
	wsprintf(chBuf, "%d", pEtcOption->bRadioInfo[OPTION_RADIO_ENEMY_ID_HIDE]);
	m_pSetupConfig->SetSetupInfo(chSetupInfo, chBuf);					
	
	// ¸¶żě˝ş °ˇµÎ±â
	GetEtcOptionIdx_To_String(OPTION_RADIO_MOUSE_LOCK, chSetupInfo);
	wsprintf(chBuf, "%d", pEtcOption->bRadioInfo[OPTION_RADIO_MOUSE_LOCK]);
	m_pSetupConfig->SetSetupInfo(chSetupInfo, chBuf);					
	
	// şńĽÓľî ÇĘĹÍ¸µ		
	GetEtcOptionIdx_To_String(OPTION_RADIO_CHAT_FILTER, chSetupInfo);
	wsprintf(chBuf, "%d", pEtcOption->bRadioInfo[OPTION_RADIO_CHAT_FILTER]);
	m_pSetupConfig->SetSetupInfo(chSetupInfo, chBuf);			
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		option ETCÁ¤ş¸ ŔúŔĺ
/// \author		// 2008-06-20 by bhsohn EP3 żÉĽÇ°ü·Ă Ăł¸®
/// \date		2008-06-20 ~ 2008-06-20
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CInterface::SaveOptionEtcInfo()
{
	SetOptionEtcInfo(g_pSOptionEtc);	

	// 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
	SaveJoystickOptionInfo(g_pJoysticOp);
	// end 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷

	{
		char chMaxPath[MAX_PATH];
		memset(chMaxPath, 0x00, MAX_PATH);
		wsprintf(chMaxPath, SETUP_INFO_PATH);
		m_pSetupConfig->SaveSetupInfo(chMaxPath);		
	}
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		UIÁöżě±âżˇĽ­ ·Ł´ő¸µÇŇ °Íµé¸¸ ·Ł´ő¸µ
/// \author		// 2008-06-20 by bhsohn EP3 żÉĽÇ°ü·Ă Ăł¸®
/// \date		2008-06-20 ~ 2008-06-20
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CInterface::RenderWndIntfaceHideMode()
{
	for(int nLoopWndProcOrder = 0; nLoopWndProcOrder<WNDIndexEnd; nLoopWndProcOrder++)
	{
		vector<GameWindowWnd>::iterator itWnd = m_vecGameWindowWnd.begin();
		while(itWnd != m_vecGameWindowWnd.end())
		{
			if((*itWnd).nWindowOrder == nLoopWndProcOrder)
			{
				switch((*itWnd).nWindowIndex)
				{
//				case WNDGameGlobalImage:
//					{
//						// ±Ű·Îąú ŔĚąĚÁö
//						if(m_pGameGlobalImage)
//							m_pGameGlobalImage->Render();
//					}
//					break;
				case WNDInfWindow:
					{
						// ˝Ă˝şĹŰ ¸ŢĽĽÁö ąÚ˝ş 
						if(m_pGameMain->m_pInfWindow)
							m_pGameMain->m_pInfWindow->RenderMessaegBox();
					}
					break;
				case WNDCityBase:
					{
						// 2008-07-14 by bhsohn EP3 °ü·Ă Ăł¸®
						//»óÁˇ °ü·Ă ·»´ő¸µ
						if( m_pCityBase && m_pCityBase->GetCurrentBuildingNPC())
						{
							m_pCityBase->Render();	
						}
						// end 2008-07-14 by bhsohn EP3 °ü·Ă Ăł¸®
					}
					break;
				case WNDGameMain:
						// 2008-07-10 by bhsohn ˝Ă˝şĹŰ Ă˘ żěĽ±ĽřŔ§ Ăß°ˇ
//						if(m_pGameMain->m_pChat)
//							m_pGameMain->m_pChat->RenderWisperChat();
					break;
//				case WNDMissionInfo:
//					{
//						// 2007-06-12 by bhsohn ąĚĽÇ ŔÎĹÍĆäŔĚ˝ş ĽöÁ¤ľČ
//						// ąĚĽÇ ŔÎĆ÷
//						//if(g_pGameMain->m_pMissionInfo && 
//						if(g_pTutorial->IsTutorialMode() == FALSE &&
//							g_pD3dApp->m_dwGameState != _SHOP)
//						{
//							// 2007-06-12 by bhsohn ąĚĽÇ ŔÎĹÍĆäŔĚ˝ş ĽöÁ¤ľČ
//							//g_pGameMain->m_pMissionInfo->Render();
//							g_pGameMain->RenderMission();
//						}
//					}
//					break;
//				case WNDMiniMap:
//					{
//						// ąĚ´Ď¸Ę
//						if(g_pD3dApp->m_dwGameState == _GAME || 
//							g_pD3dApp->m_dwGameState == _SHOP )
//						{
//							if(m_pGameMain->m_pMiniMap)
//								m_pGameMain->m_pMiniMap->Render();
//						}
//					}
//					break;
//				case WNDMp3Player:
//					{
//						// mp3 ÇĂ·ąŔĚľî
//						if(	g_pGameMain->m_pMp3Player && 
//							g_pGameMain->m_bMp3PlayFlag == TRUE && 
//							m_bShowInterface && 
//							!g_pShuttleChild->IsObserverMode() &&	// 2007-03-27 by bhsohn żÉŔúąö ¸đµĺ˝Ă ŔÎĹÍĆäŔĚ˝ş Ľű±č
//							g_pTutorial->IsTutorialMode() == FALSE)
//						{
//							g_pGameMain->m_pMp3Player->Render();
//						}
//					}
//					break;
				case WNDRenderHelp:
					{
						// 2006-04-17 by ispark
//						// µµ˝Ă,°ÔŔÓ °řĹë
//						if(m_pGameMain->m_bShowHelp)
//							m_pGameMain->RenderHelp();
					}
					break;
				case WNDInfSkill:
					{
						// 2010. 05. 27 by jskim ˝Ăł×¸¶ Ŕűżë Ä«¸Ţ¶ó ±¸Çö
 						//if( m_pGameMain->m_pInfSkill )
						if( m_pGameMain->m_pInfSkill && !g_pShuttleChild->IsShowCinema() )
 							m_pGameMain->m_pInfSkill->Render();
						//end 2010. 05. 27 by jskim ˝Ăł×¸¶ Ŕűżë Ä«¸Ţ¶ó ±¸Çö
					}
					break;
				case WNDGameMainFaq:
					{
						// FAQ °ü·Ă
						if(m_pGameMain->m_pInfGameMainFaq && m_pGameMain->m_bFAQProsFlag)
							m_pGameMain->m_pInfGameMainFaq->Render();
					}
					break;
				case WNDCommunityVOIP:
					{
						// VOIP
						if(m_pGameMain->m_pCommunityVOIP && m_pGameMain->m_bVOIPFlag)
							m_pGameMain->m_pCommunityVOIP->Render();
					}
					break;
				case WNDQSlotIconFlag:
					{
						// ´ÜĂŕĹ° °ü·Ă ·»´ő¸µ
						if(m_pGameMain->m_bQSlotIconFlag == TRUE)
							m_pGameMain->RenderQSlotIcon();
					}
					break;
				case WNDItemInfo:
					{						
					}
					break;
				case WNDMenuList:
					{
						if(g_pGameMain->m_pMenuList && g_pGameMain->m_bMenuListFlag)
							g_pGameMain->m_pMenuList->Render();
					}
					break;
				case WNDMainChatStartMenu:
					{
						// Ăß°ˇ Ă¤ĆĂ ¸®˝şĆ®
						if(g_pGameMain->m_pChat->m_bMenuListUse)
							g_pGameMain->m_pChat->ViewChatMenuList();
					}
					break;
				case WNDLeftRightWindow:
					{						
//						if(g_pD3dApp->m_dwGameState != _SHOP)
						
						g_pGameMain->m_pInfWindow->Render() ;
						g_pGameMain->RenderLeftRightWindow();
					}
					break;
					// 2008-04-04 by bhsohn Ep3 ÄżąÂ´ĎĆĽ Ă˘
				case WNDCommunityWnd:
					{						
						// ÄżąÂ´ĎĆĽĂ˘ ·Ł´ő¸µ
						g_pGameMain->RenderCommunity();	
					}
					break;
					// end 2008-04-04 by bhsohn Ep3 ÄżąÂ´ĎĆĽ Ă˘
				// 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
				 case WNDOpJoystick:
					 {
						 if(g_pGameMain )
						 {
							 g_pGameMain->RenderOpJoystickWnd();
						 }							 
					 }
					 break;
					 // end 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
					// 2008-08-22 by bhsohn EP3 ŔÎşĄĹä¸® Ăł¸®
				 case WNDInvenWnd:
					 {
						 g_pGameMain->RenderInvenWnd();
					 }
					 break;
					 // end 2008-08-22 by bhsohn EP3 ŔÎşĄĹä¸® Ăł¸®
					// 2008-09-22 by bhsohn EP3 Äł¸ŻĹÍ Ă˘
				 case WNDCharacterWnd:
					 {
						 if(g_pGameMain)
						 {
							 g_pGameMain->RenderCharacterWnd();
						 }						 
					 }
					 break;
					 // end 2008-09-22 by bhsohn EP3 Äł¸ŻĹÍ Ă˘
				// 2008-07-10 by bhsohn ˝Ă˝şĹŰ Ă˘ żěĽ±ĽřŔ§ Ăß°ˇ
				 case WNDSysWnd:
					 {
						 if(m_pGameMain->m_pChat)
						 {
							 m_pGameMain->m_pChat->RenderSystemMsg();
						 }
						 if(m_pGameMain->m_pChat)
						 {
							 m_pGameMain->m_pChat->RenderWisperChat();
						 }
							 
					 }
					 break;
				case WNDVoiceWnd:
					 {
						 if(m_pGameMain && m_pGameMain->m_pChat)
						 {
							 m_pGameMain->m_pChat->RenderVoiceChatOption();
						 }
					 }
					 break;
				case WNDMacroWnd:
					 {
						 if(m_pGameMain && m_pGameMain->m_pChat)
						 {							 
							 m_pGameMain->m_pChat->RenderMacro();
						 }
					 }
					 break;

				// 2009. 01. 12 by ckPark Ľ±Ŕü Ć÷°í ˝Ă˝şĹŰ
				case WNDWarDecalre:
					if(m_pGameMain && m_pGameMain->GetINFMotherShipManager())
						m_pGameMain->GetINFMotherShipManager()->GetWarDeclareWindow()->Render();
					break;
				// end 2009. 01. 12 by ckPark Ľ±Ŕü Ć÷°í ˝Ă˝şĹŰ
					// 2009-02-13 by bhsohn żůµĺ ·©Ĺ· ˝Ă˝şĹŰ
				case WNDWorldRank: // żůµĺ ·©Ĺ· Ă˘ 
					{
						if(m_pGameMain)
						{
							m_pGameMain->RenderWorldRank();				
						}						
					}
					break;
					// end 2009-02-13 by bhsohn żůµĺ ·©Ĺ· ˝Ă˝şĹŰ
					 // end 2008-07-10 by bhsohn ˝Ă˝şĹŰ Ă˘ żěĽ±ĽřŔ§ Ăß°ˇ
				}
			}			
			itWnd++;
		}
	}
}
///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		ŔÎĹÍĆäŔĚ˝ş Ľű±â±â
/// \author		// 2008-06-20 by bhsohn EP3 żÉĽÇ°ü·Ă Ăł¸®
/// \date		2008-06-20 ~ 2008-06-20
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CInterface::GetShowInterface()
{
	return m_bShowInterface;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		
/// \author		// 2008-06-20 by bhsohn EP3 żÉĽÇ°ü·Ă Ăł¸®
/// \date		2008-06-20 ~ 2008-06-20
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CInterface::SetShowInterface(BOOL bShowInterface)
{
	m_bShowInterface = bShowInterface;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		
/// \author		// 2008-10-15 by bhsohn ¸®ĽŇ˝ş ¸Ţ¸đ¸® ş¸ČŁ ±â´É Ăß°ˇ
/// \date		2008-10-15 ~ 2008-10-15
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CInterface::SetDummyDataHeader(DataHeader *pDataHeader)
{
	if(m_pDummyDataHeader)
	{
		// ŔĚąĚ ·ÎµĺÇß´Ů.
		return;
	}
	
	m_pDummyDataHeader = new DataHeader;

	m_pDummyDataHeader->m_EncodeNum = pDataHeader->m_EncodeNum;		// m_EncodeNum
	m_pDummyDataHeader->m_DataSize = pDataHeader->m_DataSize;		// m_DataSize
	m_pDummyDataHeader->m_Parity = pDataHeader->m_Parity;			// m_Parity
	strncpy(m_pDummyDataHeader->m_FileName, pDataHeader->m_FileName, 10);	// m_FileName

	//m_pDummyDataHeader->m_pData 
	m_pDummyDataHeader->m_pData = new char[m_pDummyDataHeader->m_DataSize+1];
	memset(m_pDummyDataHeader->m_pData, 0x00, m_pDummyDataHeader->m_DataSize+1);
	memcpy(m_pDummyDataHeader->m_pData, pDataHeader->m_pData, pDataHeader->m_DataSize );
}
	
///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		
/// \author		// 2008-10-15 by bhsohn ¸®ĽŇ˝ş ¸Ţ¸đ¸® ş¸ČŁ ±â´É Ăß°ˇ
/// \date		2008-10-15 ~ 2008-10-15
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
DataHeader *CInterface::GetDummyDataHeader(char* i_pFileName)
{
	CHARACTER myShuttleInfo = g_pShuttleChild->GetMyShuttleInfo();		
	if(COMPARE_RACE(myShuttleInfo.Race,RACE_OPERATION|RACE_GAMEMASTER))
	{
		// °ü¸®ŔÚ¸¸ Âď´Â´Ů.
		DBGOUT(".Tex File Error(%s)\n", i_pFileName);	//¸®ĽŇ˝ş ĆÄŔĎ żˇ·Ż
		char ErrorMsgMissionList[256];
		wsprintf(ErrorMsgMissionList, ".Tex File Error(%s)", i_pFileName);
		if(g_pGameMain)
		{
			g_pGameMain->CreateChatChild_OperationMode(ErrorMsgMissionList, COLOR_ERROR);
		}
	}

	return m_pDummyDataHeader;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		
/// \author		// 2008-10-27 by bhsohn ą«ÇŃŔ¸·Î ş¸±Ţ °ˇ´ÉÇŃ ąö±× ĽöÁ¤
/// \date		2008-10-27 ~ 2008-10-27
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CInterface::ReleaseBazaarShop()
{
	if(m_pBazaarShop)
	{
		m_pBazaarShop->InvalidateDeviceObjects();
		m_pBazaarShop->DeleteDeviceObjects();
		SAFE_DELETE(m_pBazaarShop);
		if(g_pCharacterChild)
		{
			g_pCharacterChild->m_nSelBazaarType = 0;
			g_pCharacterChild->m_ShopEnemyClientIndex = 0;
		}
	}
	
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		Á¶ŔĚ˝şĆ˝ Á¤ş¸ ŔĐľîżČ
/// \author		// 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
/// \date		2008-11-13 ~ 2008-11-13
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CInterface::LoadJoystickInfo(char* pFilePath)
{	
	m_pJoySticConfig->ResetSetupInfo();	// Á¤ş¸µé ĂĘ±âČ­
	if(m_pJoySticConfig->LoadSetupInfo(pFilePath))
	{		
		LoadFileJoysticInfo(m_pJoySticConfig, m_pJoystickSetupKeyTable);
	}
	else
	{
		// ĂĘ±âČ­
		InitJoystickInfo(pFilePath);
	}
	// ˝ÇÁ¦ ŔűżëÇĎ°Ô˛ű Ŕűżë
	RefreshJoystickOption();
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		Á¶ŔĚ˝şĆ˝ Á¤ş¸ ŔĐľîżČ
/// \author		// 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
/// \date		2008-11-13 ~ 2008-11-13
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CInterface::LoadFileJoysticInfo(CSetupConfig*	i_pJoySticConfig, structJoystickSetupKeyTable	*o_pJoystickSetupKeyTable)
{
	int i, nCnt;
	i = nCnt = 0;
	for(i=0; i< MAX_JOSTICK_KEY_OPTION;i++)
	{					
		memset(&o_pJoystickSetupKeyTable[i], 0x00, sizeof(structJoystickSetupKeyTable));
		o_pJoystickSetupKeyTable[i].nKeySetup = -1;
		for(nCnt=0; nCnt< MAX_JOSTICK_OPTION_SLOT;nCnt++)
		{
			o_pJoystickSetupKeyTable[i].nBehavior[nCnt] = -1;
		}
	}

	vector<SetupConfig*>	*pJoyStickVector = i_pJoySticConfig->GetSetupInfoIterator();
	vector<SetupConfig*>::iterator it = pJoyStickVector->begin();
	while(it != pJoyStickVector->end())
	{
		SetupConfig* pJoysticConfig = (*it);

		int nJoyIndex = GetJoysticInfoString_To_Idx(pJoysticConfig->chTitle);
		if(nJoyIndex != -1)
		{			
			int nPos = 0;
			nCnt = 0;
			char chTmp[64];
			memset(chTmp, 0x00, 64);
			
			o_pJoystickSetupKeyTable[nJoyIndex].nKeySetup = nJoyIndex;		// Ĺ°ŔÎµ¦˝ş					
			
			for(nCnt = 0;nCnt < MAX_JOSTICK_OPTION_SLOT;nCnt++)
			{
				memset(chTmp, 0x00, 64);
				nPos = GetCullingString(nPos, pJoysticConfig->chString, ',', chTmp);
				if(strlen(chTmp) > 0)
				{
					o_pJoystickSetupKeyTable[nJoyIndex].nBehavior[nCnt]= atoi(chTmp);		// ˝˝·Ô°Şµé
				}
				else
				{
					o_pJoystickSetupKeyTable[nJoyIndex].nBehavior[nCnt]= -1;
				}
			}
		}	
		it++;
	}	
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		Á¶ŔĚ˝şĆ˝ Á¤ş¸ ŔĐľîżČ
/// \author		// 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
/// \date		2008-11-13 ~ 2008-11-13
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CInterface::RefreshJoystickOption()
{
	int i = 0;
	int nCnt = 0 ;
	for(i=0; i< MAX_JOSTICK_OPTION;i++)
	{
		for(nCnt=0; nCnt< MAX_JOSTICK_OPTION_BEHAVIOR;nCnt++)
		{
			memset(&m_struJoystickOption[i][nCnt], 0x00, sizeof(structJoystickKey));			
			m_struJoystickOption[i][nCnt].nKeyIdx = -1;
			m_struJoystickOption[i][nCnt].nKeyValue = -1;
		}		
	}
	
	CJoyStickInput *pJoyStick = g_pD3dApp->GetJoystickControl();
	if(!pJoyStick)
	{
		return;
	}
	
	
	int nBehaviorPos = 0;
	int nSlot = 0;
	int	nJoyKeyIndex, nJoyKeyValue, nBehaviorIdx;
	nJoyKeyIndex = nJoyKeyValue = nBehaviorIdx = -1;
	for(nCnt = 0;nCnt < MAX_JOSTICK_KEY_OPTION;nCnt++)
	{
		nJoyKeyIndex = nJoyKeyValue = -1;

		pJoyStick->GetKeyKeyValue_To_JoysticInfo(m_pJoystickSetupKeyTable[nCnt].nKeySetup, 
				&nJoyKeyIndex, &nJoyKeyValue);

		for(nSlot = 0; nSlot< MAX_JOSTICK_OPTION_SLOT;nSlot++)
		{
			nBehaviorIdx = -1;
			if(-1 != m_pJoystickSetupKeyTable[nCnt].nBehavior[nSlot])
			{
				nBehaviorIdx = m_pJoystickSetupKeyTable[nCnt].nBehavior[nSlot];
				SetJoysticInfoBehavior(nBehaviorIdx, nJoyKeyIndex, nJoyKeyValue);							
			}
		}			
	}
}
///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		Á¶ŔĚ˝şĆ˝ Á¤ş¸ ŔÎµ¦˝ş
/// \author		// 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
/// \date		2008-11-13 ~ 2008-11-13
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CInterface::SetJoysticInfoBehavior(int i_nBehaviorIdx, int i_nJoyKeyIndex, int i_nJoyKeyValue)
{
	if( i_nBehaviorIdx < 0 || i_nBehaviorIdx >= MAX_JOSTICK_OPTION )
	{
		return;
	}	
	int nBehaviorPos = 0;
	for(nBehaviorPos = 0;nBehaviorPos < MAX_JOSTICK_OPTION_BEHAVIOR;nBehaviorPos++)
	{
		if(-1 == m_struJoystickOption[i_nBehaviorIdx][nBehaviorPos].nKeyIdx)
		{
			// ľĆłŻ·Î±× ´ŮŔĚľó·Î±×´Â Ŕüżˇ °ŞŔĚ ŔÖŔ¸¸é ľČµČ´Ů.
			m_struJoystickOption[i_nBehaviorIdx][nBehaviorPos].nKeyIdx = i_nJoyKeyIndex;
			m_struJoystickOption[i_nBehaviorIdx][nBehaviorPos].nKeyValue = i_nJoyKeyValue;					
			return;
		}
	}

}
///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		Á¶ŔĚ˝şĆ˝ Á¤ş¸ ŔÎµ¦˝ş
/// \author		// 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
/// \date		2008-11-13 ~ 2008-11-13
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
int CInterface::GetJoysticInfoString_To_Idx(char* i_pTitle)
{
	int nIdx = -1;
	
	if(!stricmp(i_pTitle,JOY_SETUP_OP_XAXIS_MINUS))
	{
		//"X Axis(-)"
		nIdx = JOY_SETUP_XAXIS_MINUS;
	}
	else if(!stricmp(i_pTitle,JOY_SETUP_OP_XAXIS_PLUS))
	{
		//"X Axis(+)"
		nIdx = JOY_SETUP_XAXIS_PLUS;
	}
	else if(!stricmp(i_pTitle,JOY_SETUP_OP_YAXIS_MINUS))
	{
		//"Y Axis(-)"
		nIdx = JOY_SETUP_YAXIS_MINUS;
	}
	else if(!stricmp(i_pTitle,JOY_SETUP_OP_YAXIS_PLUS))
	{
		//"Y Axis(+)"
		nIdx = JOY_SETUP_YAXIS_PLUS;
	}
	else if(!stricmp(i_pTitle,JOY_SETUP_OP_ZAXIS_MINUS))
	{
		//"Z Axis(-)"
		nIdx = JOY_SETUP_ZAXIS_MINUS;
	}
	else if(!stricmp(i_pTitle,JOY_SETUP_OP_ZAXIS_PLUS))
	{
		//"Z Axis(+)"
		nIdx = JOY_SETUP_ZAXIS_PLUS;
	}
	else if(!stricmp(i_pTitle,JOY_SETUP_OP_RXAXIS_MINUS))
	{
		//"RX Axis(-)"
		nIdx = JOY_SETUP_RXAXIS_MINUS;
	}
	else if(!stricmp(i_pTitle,JOY_SETUP_OP_RXAXIS_PLUS))
	{
		//"RX Axis(+)"
		nIdx = JOY_SETUP_RXAXIS_PLUS;
	}
	else if(!stricmp(i_pTitle,JOY_SETUP_OP_RYAXIS_MINUS))
	{
		//"RY Axis(-)"
		nIdx = JOY_SETUP_RYAXIS_MINUS;
	}	
	else if(!stricmp(i_pTitle,JOY_SETUP_OP_RYAXIS_PLUS))
	{
		//"RY Axis(+)"
		nIdx = JOY_SETUP_RYAXIS_PLUS;
	}
	else if(!stricmp(i_pTitle,JOY_SETUP_OP_RZAXIS_MINUS))
	{
		//"RZ Axis(-)"
		nIdx = JOY_SETUP_RZAXIS_MINUS;
	}
	else if(!stricmp(i_pTitle,JOY_SETUP_OP_RZAXIS_PLUS))
	{
		//"RZ Axis(+)"
		nIdx = JOY_SETUP_RZAXIS_PLUS;
	}
	else if(!stricmp(i_pTitle,JOY_SETUP_OP_POW_LEFT))
	{
		//"˝ĘŔÚąöĆ°-ÁÂ"
		nIdx = JOY_SETUP_POW_LEFT;
	}
	else if(!stricmp(i_pTitle,JOY_SETUP_OP_POW_RIGHT))
	{
		//"˝ĘŔÚąöĆ°-żě"
		nIdx = JOY_SETUP_POW_RIGHT;
	}
	else if(!stricmp(i_pTitle,JOY_SETUP_OP_POW_UP))
	{
		//"˝ĘŔÚąöĆ°-»ó"
		nIdx = JOY_SETUP_POW_UP;
	}
	else if(!stricmp(i_pTitle,JOY_SETUP_OP_POW_DOWN))
	{
		//"˝ĘŔÚąöĆ°-ÇĎ"
		nIdx = JOY_SETUP_POW_DOWN;
	}
	else if(!stricmp(i_pTitle,JOY_SETUP_OP_BTN0))
	{
		// BTN1
		nIdx = JOY_SETUP_BTN0;
	}
	else if(!stricmp(i_pTitle,JOY_SETUP_OP_BTN1))
	{
		// BTN1
		nIdx = JOY_SETUP_BTN1;
	}
	else if(!stricmp(i_pTitle,JOY_SETUP_OP_BTN2))
	{
		// BTN1
		nIdx = JOY_SETUP_BTN2;
	}
	else if(!stricmp(i_pTitle,JOY_SETUP_OP_BTN3))
	{
		// BTN1
		nIdx = JOY_SETUP_BTN3;
	}
	else if(!stricmp(i_pTitle,JOY_SETUP_OP_BTN4))
	{
		// BTN1
		nIdx = JOY_SETUP_BTN4;
	}
	else if(!stricmp(i_pTitle,JOY_SETUP_OP_BTN5))
	{
		// BTN1
		nIdx = JOY_SETUP_BTN5;
	}
	else if(!stricmp(i_pTitle,JOY_SETUP_OP_BTN6))
	{
		// BTN1
		nIdx = JOY_SETUP_BTN6;
	}
	else if(!stricmp(i_pTitle,JOY_SETUP_OP_BTN7))
	{
		// BTN1
		nIdx = JOY_SETUP_BTN7;
	}
	else if(!stricmp(i_pTitle,JOY_SETUP_OP_BTN8))
	{
		// BTN1
		nIdx = JOY_SETUP_BTN8;
	}
	else if(!stricmp(i_pTitle,JOY_SETUP_OP_BTN9))
	{
		// BTN1
		nIdx = JOY_SETUP_BTN9;
	}
	else if(!stricmp(i_pTitle,JOY_SETUP_OP_BTN10))
	{
		// BTN1
		nIdx = JOY_SETUP_BTN10;
	}
	else if(!stricmp(i_pTitle,JOY_SETUP_OP_BTN11))
	{
		// BTN1
		nIdx = JOY_SETUP_BTN11;
	}
	else if(!stricmp(i_pTitle,JOY_SETUP_OP_BTN12))
	{
		// BTN1
		nIdx = JOY_SETUP_BTN12;
	}
	else if(!stricmp(i_pTitle,JOY_SETUP_OP_BTN13))
	{
		// BTN1
		nIdx = JOY_SETUP_BTN13;
	}
	else if(!stricmp(i_pTitle,JOY_SETUP_OP_BTN14))
	{
		// BTN1
		nIdx = JOY_SETUP_BTN14;
	}
	else if(!stricmp(i_pTitle,JOY_SETUP_OP_BTN15))
	{
		// BTN1
		nIdx = JOY_SETUP_BTN15;
	}
	else if(!stricmp(i_pTitle,JOY_SETUP_OP_BTN16))
	{
		// BTN1
		nIdx = JOY_SETUP_BTN16;
	}
	else if(!stricmp(i_pTitle,JOY_SETUP_OP_BTN17))
	{
		// BTN1
		nIdx = JOY_SETUP_BTN17;
	}
	else if(!stricmp(i_pTitle,JOY_SETUP_OP_BTN18))
	{
		// BTN1
		nIdx = JOY_SETUP_BTN18;
	}
	else if(!stricmp(i_pTitle,JOY_SETUP_OP_BTN19))
	{
		// BTN1
		nIdx = JOY_SETUP_BTN19;
	}	
	return nIdx;

}
///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		Á¶ŔĚ˝şĆ˝ Á¤ş¸ ŔÎµ¦˝ş
/// \author		// 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
/// \date		2008-11-13 ~ 2008-11-13
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CInterface::GetJoysticInfoIdx_To_String(int nIdx, char* o_pDst)
{
	switch(nIdx)
	{
	case JOY_SETUP_XAXIS_MINUS:
		{
			//"X Axis(-)"
			wsprintf(o_pDst, JOY_SETUP_OP_XAXIS_MINUS);
		}
		break;
	case JOY_SETUP_XAXIS_PLUS:
		{
			//"X Axis(+)"
			wsprintf(o_pDst, JOY_SETUP_OP_XAXIS_PLUS);
		}
		break;
	case JOY_SETUP_YAXIS_MINUS:
		{
			//"Y Axis(-)"
			wsprintf(o_pDst, JOY_SETUP_OP_YAXIS_MINUS);
		}
		break;
	case JOY_SETUP_YAXIS_PLUS:
		{
			//"Y Axis(+)"
			wsprintf(o_pDst, JOY_SETUP_OP_YAXIS_PLUS);
		}
		break;
	case JOY_SETUP_ZAXIS_MINUS:
		{
			//"Z Axis(-)"
			wsprintf(o_pDst, JOY_SETUP_OP_ZAXIS_MINUS);
		}
		break;
	case JOY_SETUP_ZAXIS_PLUS:
		{
			//"Z Axis(+)"
			wsprintf(o_pDst, JOY_SETUP_OP_ZAXIS_PLUS);
		}
		break;
	case JOY_SETUP_RXAXIS_MINUS:
		{
			//"RX Axis(-)"
			wsprintf(o_pDst, JOY_SETUP_OP_RXAXIS_MINUS);
		}
		break;
	case JOY_SETUP_RXAXIS_PLUS:
		{
			//"RX Axis(+)"
			wsprintf(o_pDst, JOY_SETUP_OP_RXAXIS_PLUS);
		}
		break;
	case JOY_SETUP_RYAXIS_MINUS:
		{
			//"RY Axis(-)"
			wsprintf(o_pDst, JOY_SETUP_OP_RYAXIS_MINUS);
		}
		break;
	case JOY_SETUP_RYAXIS_PLUS:
		{
			//"RY Axis(+)"
			wsprintf(o_pDst, JOY_SETUP_OP_RYAXIS_PLUS);
		}
		break;
	case JOY_SETUP_RZAXIS_MINUS:
		{
			//"RZ Axis(-)"
			wsprintf(o_pDst, JOY_SETUP_OP_RZAXIS_MINUS);
		}
		break;
	case JOY_SETUP_RZAXIS_PLUS:
		{
			//"RZ Axis(+)"
			wsprintf(o_pDst, JOY_SETUP_OP_RZAXIS_PLUS);
		}
		break;
	case JOY_SETUP_POW_LEFT:
		{
			//"˝ĘŔÚąöĆ°-ÁÂ"
			wsprintf(o_pDst, JOY_SETUP_OP_POW_LEFT);
		}
		break;
	case JOY_SETUP_POW_RIGHT:
		{
			//"˝ĘŔÚąöĆ°-żě"
			wsprintf(o_pDst, JOY_SETUP_OP_POW_RIGHT);
		}
		break;
	case JOY_SETUP_POW_UP:
		{
			//"˝ĘŔÚąöĆ°-»ó"
			wsprintf(o_pDst, JOY_SETUP_OP_POW_UP);
		}
		break;
	case JOY_SETUP_POW_DOWN:
		{
			//"˝ĘŔÚąöĆ°-ÇĎ"
			wsprintf(o_pDst, JOY_SETUP_OP_POW_DOWN);
		}
		break;
	case JOY_SETUP_BTN0:
		{
			// BTN1
			wsprintf(o_pDst, JOY_SETUP_OP_BTN0);
		}
		break;
	case JOY_SETUP_BTN1:
		{
			// BTN2
			wsprintf(o_pDst, JOY_SETUP_OP_BTN1);
		}
		break;
	case JOY_SETUP_BTN2:
		{
			// BTN2
			wsprintf(o_pDst, JOY_SETUP_OP_BTN2);
		}
		break;
	case JOY_SETUP_BTN3:
		{
			// BTN2
			wsprintf(o_pDst, JOY_SETUP_OP_BTN3);
		}
		break;
	case JOY_SETUP_BTN4:
		{
			// BTN2
			wsprintf(o_pDst, JOY_SETUP_OP_BTN4);
		}
		break;
	case JOY_SETUP_BTN5:
		{
			// BTN2
			wsprintf(o_pDst, JOY_SETUP_OP_BTN5);
		}
		break;
	case JOY_SETUP_BTN6:
		{
			// BTN2
			wsprintf(o_pDst, JOY_SETUP_OP_BTN6);
		}
		break;
	case JOY_SETUP_BTN7:
		{
			// BTN2
			wsprintf(o_pDst, JOY_SETUP_OP_BTN7);
		}
		break;
	case JOY_SETUP_BTN8:
		{
			// BTN2
			wsprintf(o_pDst, JOY_SETUP_OP_BTN8);
		}
		break;
	case JOY_SETUP_BTN9:
		{
			// BTN2
			wsprintf(o_pDst, JOY_SETUP_OP_BTN9);
		}
		break;
	case JOY_SETUP_BTN10:
		{
			// BTN2
			wsprintf(o_pDst, JOY_SETUP_OP_BTN10);
		}
		break;
	case JOY_SETUP_BTN11:
		{
			// BTN2
			wsprintf(o_pDst, JOY_SETUP_OP_BTN11);
		}
		break;
	case JOY_SETUP_BTN12:
		{
			// BTN2
			wsprintf(o_pDst, JOY_SETUP_OP_BTN12);
		}
		break;
	case JOY_SETUP_BTN13:
		{
			// BTN2
			wsprintf(o_pDst, JOY_SETUP_OP_BTN13);
		}
		break;
	case JOY_SETUP_BTN14:
		{
			// BTN2
			wsprintf(o_pDst, JOY_SETUP_OP_BTN14);
		}
		break;
	case JOY_SETUP_BTN15:
		{
			// BTN2
			wsprintf(o_pDst, JOY_SETUP_OP_BTN15);
		}
		break;
	case JOY_SETUP_BTN16:
		{
			// BTN2
			wsprintf(o_pDst, JOY_SETUP_OP_BTN16);
		}
		break;
	case JOY_SETUP_BTN17:
		{
			// BTN2
			wsprintf(o_pDst, JOY_SETUP_OP_BTN17);
		}
		break;
	case JOY_SETUP_BTN18:
		{
			// BTN2
			wsprintf(o_pDst, JOY_SETUP_OP_BTN18);
		}
		break;
	case JOY_SETUP_BTN19:
		{
			// BTN2
			wsprintf(o_pDst, JOY_SETUP_OP_BTN19);
		}
		break;
	default:
		{
			wsprintf(o_pDst, "");
		}
		break;
	}			
}


///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		Á¶ŔĚ˝şĆ˝ Á¤ş¸ ŔĐľîżČ
/// \author		// 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
/// \date		2008-11-13 ~ 2008-11-13
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CInterface::InitJoystickInfo(char* pFilePath)
{
	InitJoystickSetupInfo(m_pJoystickSetupKeyTable);	// Á¶ŔĚ˝şĆ˝ Á¤ş¸ ĂĘ±âČ­
	
	char chKeyBuff[MAX_PATH], chBuff[MAX_PATH];
	char chTmpBuff[MAX_PATH];
	memset(chKeyBuff, 0x00, MAX_PATH);	
	memset(chTmpBuff, 0x00, MAX_PATH);	
	
	int nCnt = 0;
	int nBehavior = 0;

	for(nCnt = 0;nCnt < MAX_JOSTICK_KEY_OPTION;nCnt++)
	{		
		GetJoysticInfoIdx_To_String(nCnt, chKeyBuff);
		if(strlen(chKeyBuff) <= 0)
		{
			continue;
		}
		memset(chBuff, 0x00, MAX_PATH);	
		
		for(nBehavior=0; nBehavior< MAX_JOSTICK_OPTION_SLOT;nBehavior++)
		{
			if(m_pJoystickSetupKeyTable[nCnt].nBehavior[nBehavior] != -1)
			{
				wsprintf(chTmpBuff, "%d,", m_pJoystickSetupKeyTable[nCnt].nBehavior[nBehavior]);
				strcat(chBuff, chTmpBuff);	// µÚżˇ´Ů şŮŔĚŔÚ
			}
		}			
		m_pJoySticConfig->AddSetupInfo(chKeyBuff, chBuff);
		
	}	
	m_pJoySticConfig->SaveSetupInfo(pFilePath); // ±âş»°ŞŔúŔĺ
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		Á¶ŔĚ˝şĆ˝ Á¤ş¸ ŔĐľîżČ
/// \author		// 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
/// \date		2008-11-13 ~ 2008-11-13
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CInterface::GetJoystickInfo(int i_nOptionIdx, int i_nSlotIdx, int*	o_nKeyIdx, int* o_nKeyValue)
{
	if(i_nOptionIdx >= MAX_JOSTICK_OPTION
		|| (i_nOptionIdx < 0))
	{
		(*o_nKeyIdx) = 0;
		(*o_nKeyValue) = 0;
		return;
	}
	else if(i_nSlotIdx >= MAX_JOSTICK_OPTION_BEHAVIOR
		|| (i_nSlotIdx < 0))
	{
		(*o_nKeyIdx) = 0;
		(*o_nKeyValue) = 0;
		return;
	}

	(*o_nKeyIdx) = m_struJoystickOption[i_nOptionIdx][i_nSlotIdx].nKeyIdx;
	(*o_nKeyValue) = m_struJoystickOption[i_nOptionIdx][i_nSlotIdx].nKeyValue;

}

///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		Á¶ŔĚ˝şĆ˝ Á¤ş¸ ŔĐľîżČ
/// \author		// 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
/// \date		2008-11-13 ~ 2008-11-13
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
int CInterface::GetCullingString(int nPos, char* i_pString, char i_chFind, char* o_pDst)
{
	int nCnt = 0;
	int nDstPos = 0;
	BOOL bFind = FALSE;
	int len = strlen(i_pString)+1;
	for(nCnt = nPos;nCnt < len; nCnt++)
	{
		if(i_pString[nCnt] == i_chFind)
		{
			o_pDst[nDstPos] = NULL;
			bFind = TRUE;
			break;
		}
		else
		{
			o_pDst[nDstPos] = i_pString[nCnt];
			nDstPos++;
		}
	}	
	if(bFind)
	{
		nCnt++;
	}
	return nCnt;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		
/// \author		// 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
/// \date		2008-11-13 ~ 2008-11-13
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CInterface::GetJostickStats(int code)
{
	if(!g_pD3dApp->IsUseJoyStick())
	{
		return FALSE;
	}
	else if(g_pGameMain && g_pGameMain->IsShowOpJoystick())
	{
		// Á¶ŔĚ˝şĆ˝ żÉĽÇĂ˘ŔĚ ¶°ŔÖ´Ů.
		return 0;
	}

	CJoyStickInput *pJoyStick = g_pD3dApp->GetJoystickControl();
	if(NULL == pJoyStick)
	{
		return FALSE;
	}

	BOOL bKeyDown = FALSE;
	int nBehaviorPos = 0;
	for(nBehaviorPos = 0;nBehaviorPos < MAX_JOSTICK_OPTION_BEHAVIOR;nBehaviorPos++)
	{
		bKeyDown = FALSE;
		switch(code)
		{
		case DIK_LSHIFT:
			{
				// Lock OnÇŘÁ¦
				int nKeyIdx = 0;
				int nKeyValue = 0;
				
				// şÎ˝şĹÍ
				GetJoystickInfo(JOSTICK_OPTION_TARGET_LOCKON, nBehaviorPos, &nKeyIdx, &nKeyValue);				
				
				if((-1 != nKeyIdx) && nKeyValue == pJoyStick->GetJoySitckInfo(nKeyIdx))
				{
					bKeyDown = TRUE;					
				}
			}
			break;
		case DIK_SPACE:
			{
				int nKeyIdx = 0;
				int nKeyValue = 0;
				
				// şÎ˝şĹÍ
				GetJoystickInfo(JOSTICK_OPTION_BOSTER, nBehaviorPos, &nKeyIdx, &nKeyValue);
				
				
				if((-1 != nKeyIdx) && nKeyValue == pJoyStick->GetJoySitckInfo(nKeyIdx))
				{
					bKeyDown = TRUE;					
				}
			}
			break;
		case DIK_C:
			{
				// Ăâ°Ý ąöĆ°
				int nKeyIdx = 0;
				int nKeyValue = 0;
				
				GetJoystickInfo(JOSTICK_OPTION_TAKEOFF, nBehaviorPos, &nKeyIdx, &nKeyValue);				
				
				if((-1 != nKeyIdx) && nKeyValue == pJoyStick->GetJoySitckInfo(nKeyIdx))
				{
					bKeyDown = TRUE;					
				}
			}
			break;
		case DIK_S:
			{
				int nMinKeyIdx, nMinKeyValue;
				int nLMinKeyIdx, nLMinKeyValue;
				int nRMinKeyIdx, nRMinKeyValue;
				
				nMinKeyIdx = nMinKeyValue = nLMinKeyIdx = nLMinKeyValue = nRMinKeyIdx = nRMinKeyValue = 0;
				GetJoystickInfo(JOSTICK_OPTION_UNIT_MIN_SPEED, nBehaviorPos, &nMinKeyIdx, &nMinKeyValue);
				GetJoystickInfo(JOSTICK_OPTION_UNIT_LEFT, nBehaviorPos, &nLMinKeyIdx, &nLMinKeyValue);			
				GetJoystickInfo(JOSTICK_OPTION_UNIT_RIGHT, nBehaviorPos, &nRMinKeyIdx, &nRMinKeyValue);			
				
				int nPow = pJoyStick->GetJoySitckInfo(nMinKeyIdx);
				if((-1 != nMinKeyIdx) && nMinKeyValue == nPow)
				{
					bKeyDown = TRUE;					
				}
				
				if((-1 != nMinKeyIdx) && (-1 != nLMinKeyIdx) && (-1 != nRMinKeyIdx) 
					&& pJoyStick->IsJoystickPOV(nMinKeyIdx)		// ąćÇâĹ°ŔÎÁö ĂĽĹ©
					&& pJoyStick->IsJoystickPOV(nLMinKeyIdx) 
					&& pJoyStick->IsJoystickPOV(nRMinKeyIdx))
				{				
					if((nLMinKeyValue > nMinKeyValue) && (nMinKeyValue > nRMinKeyValue))
					{
						if((nLMinKeyValue > nPow) && (nPow > nRMinKeyValue))
						{
							bKeyDown = TRUE;							
						}
					}
					else if((nLMinKeyValue < nMinKeyValue) && (nMinKeyValue < nRMinKeyValue))
					{
						if((nLMinKeyValue < nPow) && (nPow < nRMinKeyValue))
						{
							bKeyDown = TRUE;							
						}
					}				
				}			
			}
			break;
		case DIK_A:
			{
				int nLKeyIdx, nLKeyValue;
				int nLMinKeyIdx, nLMinKeyValue;
				
				nLKeyIdx = nLKeyValue = nLMinKeyIdx = nLMinKeyValue = 0;
				
				GetJoystickInfo(JOSTICK_OPTION_UNIT_LEFT, nBehaviorPos, &nLKeyIdx, &nLKeyValue);
				GetJoystickInfo(JOSTICK_OPTION_UNIT_MIN_SPEED, nBehaviorPos, &nLMinKeyIdx, &nLMinKeyValue);
				
				int nPow = pJoyStick->GetJoySitckInfo(nLKeyIdx);
				if((-1 != nLKeyIdx) && nLKeyValue == nPow)
				{
					bKeyDown = TRUE;					
				}
				
				if((-1 != nLKeyIdx) && (-1 != nLMinKeyIdx)  
					&& pJoyStick->IsJoystickPOV(nLKeyIdx)		// ąćÇâĹ°ŔÎÁö ĂĽĹ©
					&& pJoyStick->IsJoystickPOV(nLMinKeyIdx))
				{
					if(nLKeyValue > nLMinKeyValue)
					{
						if((nLKeyValue > nPow) && (nPow > nLMinKeyValue))
						{
							bKeyDown = TRUE;							
						}
					}
					else if(nLKeyValue < nLMinKeyValue)
					{
						if((nLKeyValue < nPow) && (nPow < nLMinKeyValue))
						{
							bKeyDown = TRUE;							
						}
					}
				}
				// 2008-12-23 by bhsohn Á¶ŔĚ˝şĆ˝ Ăß°ˇ żäĂ» Ăł¸®
				// ľŐżˇ ąćÇâ
				int nGoKeyIdx, nGoKeyValue;
				nGoKeyIdx = nGoKeyValue = 0;				

				GetJoystickInfo(JOSTICK_OPTION_UNIT_GO, nBehaviorPos, &nGoKeyIdx, &nGoKeyValue);
				
				if((-1 != nPow) && (-1 != nLKeyIdx) && (-1 != nGoKeyIdx)  
					&& pJoyStick->IsJoystickPOV(nLKeyIdx)		// ąćÇâĹ°ŔÎÁö ĂĽĹ©
					&& pJoyStick->IsJoystickPOV(nGoKeyIdx))
				{
					if(0 == nGoKeyValue)
					{
						nGoKeyValue = 36000;
					}
					if(nLKeyValue > nGoKeyValue)
					{
						if((nLKeyValue > nPow) && (nPow > nGoKeyValue))
						{
							bKeyDown = TRUE;							
						}
					}
					else if(nLKeyValue < nGoKeyValue)
					{
						if((nLKeyValue < nPow) && (nPow < nGoKeyValue))
						{
							bKeyDown = TRUE;							
						}
					}
				}
				
				// end 2008-12-23 by bhsohn Á¶ŔĚ˝şĆ˝ Ăß°ˇ żäĂ» Ăł¸®
				
			}
			break;
		case DIK_D:
			{
				int nRKeyIdx, nRKeyValue;
				int nRMinKeyIdx, nRMinKeyValue;
				
				nRKeyIdx = nRKeyValue = nRMinKeyIdx = nRMinKeyValue = 0;
				
				GetJoystickInfo(JOSTICK_OPTION_UNIT_RIGHT, nBehaviorPos, &nRKeyIdx, &nRKeyValue);
				GetJoystickInfo(JOSTICK_OPTION_UNIT_MIN_SPEED, nBehaviorPos, &nRMinKeyIdx, &nRMinKeyValue);
				
				int nPow = pJoyStick->GetJoySitckInfo(nRKeyIdx);
				if((-1 != nRKeyIdx) && nRKeyValue == nPow)
				{
					bKeyDown = TRUE;					
				}
				if((-1 != nRKeyIdx) && (-1 != nRMinKeyIdx)  
					&& pJoyStick->IsJoystickPOV(nRKeyIdx)		// ąćÇâĹ°ŔÎÁö ĂĽĹ©
					&& pJoyStick->IsJoystickPOV(nRMinKeyIdx))
				{
					if(nRKeyValue > nRMinKeyValue)
					{
						if((nRKeyValue > nPow) && (nPow > nRMinKeyValue))
						{
							bKeyDown = TRUE;							
						}
					}
					else if(nRKeyValue < nRMinKeyValue)
					{
						if((nRKeyValue < nPow) && (nPow < nRMinKeyValue))
						{
							bKeyDown = TRUE;							
						}
					}
				}
				// 2008-12-23 by bhsohn Á¶ŔĚ˝şĆ˝ Ăß°ˇ żäĂ» Ăł¸®
				// ľŐżˇ ąćÇâ
				int nGoKeyIdx, nGoKeyValue;
				nGoKeyIdx = nGoKeyValue = 0;				
				
				GetJoystickInfo(JOSTICK_OPTION_UNIT_GO, nBehaviorPos, &nGoKeyIdx, &nGoKeyValue);
				
				if((-1 != nPow)&& (-1 != nRKeyIdx) && (-1 != nGoKeyIdx)  
					&& pJoyStick->IsJoystickPOV(nRKeyIdx)		// ąćÇâĹ°ŔÎÁö ĂĽĹ©
					&& pJoyStick->IsJoystickPOV(nGoKeyIdx))
				{					
					if(nRKeyValue > nGoKeyValue)
					{
						if((nRKeyValue > nPow) && (nPow > nGoKeyValue))
						{
							bKeyDown = TRUE;							
						}
					}
					else if(nRKeyValue < nGoKeyValue)
					{
						if((nRKeyValue < nPow) && (nPow < nGoKeyValue))
						{
							bKeyDown = TRUE;							
						}
					}
				}				
				// end 2008-12-23 by bhsohn Á¶ŔĚ˝şĆ˝ Ăß°ˇ żäĂ» Ăł¸®
				
			}
			break;
		case DIK_W:
			{
				int nKeyIdx, nKeyValue;
				nKeyIdx = nKeyValue = 0;
				
				GetJoystickInfo(JOSTICK_OPTION_UNIT_GO, nBehaviorPos, &nKeyIdx, &nKeyValue);
				
				int nPow = pJoyStick->GetJoySitckInfo(nKeyIdx);
				if((-1 != nKeyIdx) && nKeyValue == nPow)
				{
					bKeyDown = TRUE;					
				}
				// 2008-12-23 by bhsohn Á¶ŔĚ˝şĆ˝ Ăß°ˇ żäĂ» Ăł¸®
				// ľŐżˇ ąćÇâ
				int nLMinKeyIdx, nLMinKeyValue;
				int nRMinKeyIdx, nRMinKeyValue;
				
				nLMinKeyIdx = nLMinKeyValue = nRMinKeyIdx = nRMinKeyValue = 0;				
				GetJoystickInfo(JOSTICK_OPTION_UNIT_LEFT, nBehaviorPos, &nLMinKeyIdx, &nLMinKeyValue);			
				GetJoystickInfo(JOSTICK_OPTION_UNIT_RIGHT, nBehaviorPos, &nRMinKeyIdx, &nRMinKeyValue);

				if((-1 != nPow)
					&& (-1 != nKeyIdx) && (-1 != nLMinKeyIdx) && (-1 != nRMinKeyIdx) 
					&& pJoyStick->IsJoystickPOV(nKeyIdx)		// ąćÇâĹ°ŔÎÁö ĂĽĹ©
					&& pJoyStick->IsJoystickPOV(nLMinKeyIdx) 
					&& pJoyStick->IsJoystickPOV(nRMinKeyIdx))
				{				
					if(0 == nKeyValue)
					{
						// ±âş»Ŕ§°ŞŔş 0ŔĚ´Ů.
						if(nLMinKeyValue < nPow)
						{
							bKeyDown = TRUE;
						}
						else if(nRMinKeyValue > nPow)
						{
							bKeyDown = TRUE;
						}

					}
					else
					{
						if((nLMinKeyValue > nKeyValue) && (nKeyValue > nRMinKeyValue))
						{
							if((nLMinKeyValue > nPow) && (nPow > nRMinKeyValue))
							{
								bKeyDown = TRUE;							
							}
						}
						else if((nLMinKeyValue < nKeyValue) && (nKeyValue < nRMinKeyValue))
						{
							if((nLMinKeyValue < nPow) && (nPow < nRMinKeyValue))
							{
								bKeyDown = TRUE;							
							}
						}
					}
					
				}
				// end 2008-12-23 by bhsohn Á¶ŔĚ˝şĆ˝ Ăß°ˇ żäĂ» Ăł¸®
			}
			break;
		case DIK_1:
			{
				int nKeyIdx, nKeyValue;
				nKeyIdx = nKeyValue = 0;
				
				GetJoystickInfo(JOSTICK_OPTION_SLOT_0, nBehaviorPos, &nKeyIdx, &nKeyValue);
				
				int nPow = pJoyStick->GetJoySitckInfo(nKeyIdx);
				
				if((-1 != nKeyIdx) && nKeyValue == nPow)
				{
					bKeyDown = TRUE;					
				}
			}
			break;
		case DIK_2:
			{
				int nKeyIdx, nKeyValue;
				nKeyIdx = nKeyValue = 0;
				
				GetJoystickInfo(JOSTICK_OPTION_SLOT_1, nBehaviorPos, &nKeyIdx, &nKeyValue);
				
				int nPow = pJoyStick->GetJoySitckInfo(nKeyIdx);
				
				if((-1 != nKeyIdx) && nKeyValue == nPow)
				{
					bKeyDown = TRUE;					
				}
			}
			break;
		case DIK_3:
			{
				int nKeyIdx, nKeyValue;
				nKeyIdx = nKeyValue = 0;
				
				GetJoystickInfo(JOSTICK_OPTION_SLOT_2, nBehaviorPos, &nKeyIdx, &nKeyValue);
				
				int nPow = pJoyStick->GetJoySitckInfo(nKeyIdx);
				
				if((-1 != nKeyIdx) && nKeyValue == nPow)
				{
					bKeyDown = TRUE;
					
				}
			}
			break;
		case DIK_4:
			{
				int nKeyIdx, nKeyValue;
				nKeyIdx = nKeyValue = 0;
				
				GetJoystickInfo(JOSTICK_OPTION_SLOT_3, nBehaviorPos, &nKeyIdx, &nKeyValue);
				
				int nPow = pJoyStick->GetJoySitckInfo(nKeyIdx);
				
				if((-1 != nKeyIdx) && nKeyValue == nPow)
				{
					bKeyDown = TRUE;
					
				}
			}
			break;
		case DIK_5:
			{
				int nKeyIdx, nKeyValue;
				nKeyIdx = nKeyValue = 0;
				
				GetJoystickInfo(JOSTICK_OPTION_SLOT_4, nBehaviorPos, &nKeyIdx, &nKeyValue);
				
				int nPow = pJoyStick->GetJoySitckInfo(nKeyIdx);
				
				if((-1 != nKeyIdx) && nKeyValue == nPow)
				{
					bKeyDown = TRUE;
					
				}
			}
			break;
		case DIK_6:
			{
				int nKeyIdx, nKeyValue;
				nKeyIdx = nKeyValue = 0;
				
				GetJoystickInfo(JOSTICK_OPTION_SLOT_5, nBehaviorPos, &nKeyIdx, &nKeyValue);
				
				int nPow = pJoyStick->GetJoySitckInfo(nKeyIdx);
				
				if((-1 != nKeyIdx) && nKeyValue == nPow)
				{
					bKeyDown = TRUE;
				}
			}
			break;
		case DIK_7:
			{
				int nKeyIdx, nKeyValue;
				nKeyIdx = nKeyValue = 0;
				
				GetJoystickInfo(JOSTICK_OPTION_SLOT_6, nBehaviorPos, &nKeyIdx, &nKeyValue);
				
				int nPow = pJoyStick->GetJoySitckInfo(nKeyIdx);
				
				if((-1 != nKeyIdx) && nKeyValue == nPow)
				{
					bKeyDown = TRUE;
				}
			}
			break;
		case DIK_8:
			{
				int nKeyIdx, nKeyValue;
				nKeyIdx = nKeyValue = 0;
				
				GetJoystickInfo(JOSTICK_OPTION_SLOT_7, nBehaviorPos, &nKeyIdx, &nKeyValue);
				
				int nPow = pJoyStick->GetJoySitckInfo(nKeyIdx);
				
				if((-1 != nKeyIdx) && nKeyValue == nPow)
				{
					bKeyDown = TRUE;
					
				}
			}
			break;
		case DIK_9:
			{
				int nKeyIdx, nKeyValue;
				nKeyIdx = nKeyValue = 0;
				
				GetJoystickInfo(JOSTICK_OPTION_SLOT_8, nBehaviorPos, &nKeyIdx, &nKeyValue);
				
				int nPow = pJoyStick->GetJoySitckInfo(nKeyIdx);
				
				if((-1 != nKeyIdx) && nKeyValue == nPow)
				{
					bKeyDown = TRUE;
					
				}
			}
			break;
		case DIK_0:
			{
				int nKeyIdx, nKeyValue;
				nKeyIdx = nKeyValue = 0;
				
				GetJoystickInfo(JOSTICK_OPTION_SLOT_9, nBehaviorPos, &nKeyIdx, &nKeyValue);
				
				int nPow = pJoyStick->GetJoySitckInfo(nKeyIdx);
				
				if((-1 != nKeyIdx) && nKeyValue == nPow)
				{
					bKeyDown = TRUE;
					
				}
			}
			break;			
			
		}
		if(bKeyDown)
		{
			return bKeyDown;
		}
	}
	
	
	return bKeyDown;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		
/// \author		// 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
/// \date		2008-11-13 ~ 2008-11-13
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CInterface::IsChangeJoySitckInfo(int code)
{
	if(!g_pD3dApp->IsUseJoyStick())
	{
		return FALSE;
	}
	else if(g_pGameMain && g_pGameMain->IsShowOpJoystick())
	{
		// Á¶ŔĚ˝şĆ˝ żÉĽÇĂ˘ŔĚ ¶°ŔÖ´Ů.
		return 0;
	}

	CJoyStickInput *pJoyStick = g_pD3dApp->GetJoystickControl();
	if(NULL == pJoyStick)
	{
		return FALSE;
	}

	BOOL bChangeKey = FALSE;
	int nBehaviorPos = 0;
	for(nBehaviorPos = 0;nBehaviorPos < MAX_JOSTICK_OPTION_BEHAVIOR;nBehaviorPos++)
	{
		switch(code)
		{
		case DIK_A:
			{
				int nLKeyIdx, nLKeyValue;
				int nLMinKeyIdx, nLMinKeyValue;
				nLKeyIdx = nLKeyValue = 0;
				nLMinKeyIdx = nLMinKeyValue= 0;
				
				GetJoystickInfo(JOSTICK_OPTION_UNIT_LEFT, nBehaviorPos, &nLKeyIdx, &nLKeyValue);
				GetJoystickInfo(JOSTICK_OPTION_UNIT_MIN_SPEED, nBehaviorPos,&nLMinKeyIdx, &nLMinKeyValue);
				
				if(nLKeyIdx != -1)
				{
					bChangeKey = pJoyStick->IsChangeJoySitckInfo(nLKeyIdx) ;
				}			
				
				if(!bChangeKey
					&& (-1 != nLKeyIdx) && (-1 != nLMinKeyIdx)  
					&& pJoyStick->IsJoystickPOV(nLKeyIdx)		// ąćÇâĹ°ŔÎÁö ĂĽĹ©
					&& pJoyStick->IsJoystickPOV(nLMinKeyIdx))
				{
					BOOL bKeyDown = FALSE;
					int nPow = pJoyStick->GetJoySitckInfo(nLKeyIdx);	// Ĺ°°Ş
					
					if(nLKeyValue > nLMinKeyValue)
					{
						if((nLKeyValue > nPow) && (nPow > nLMinKeyValue))
						{
							bKeyDown = TRUE;
						}
					}
					else if(nLKeyValue < nLMinKeyValue)
					{
						if((nLKeyValue < nPow) && (nPow < nLMinKeyValue))
						{
							bKeyDown = TRUE;
						}
					}
					if(bKeyDown)
					{
						bChangeKey = pJoyStick->IsChangeJoySitckInfo(nLMinKeyIdx) ;
					}
				}
				
				
			}
			break;
		case DIK_D:
			{
				int nRKeyIdx, nRKeyValue;
				int nRMinKeyIdx, nRMinKeyValue;
				nRKeyIdx = nRKeyValue = 0;
				nRMinKeyIdx = nRMinKeyValue = 0;
				
				GetJoystickInfo(JOSTICK_OPTION_UNIT_RIGHT, nBehaviorPos, &nRKeyIdx, &nRKeyValue);
				GetJoystickInfo(JOSTICK_OPTION_UNIT_MIN_SPEED, nBehaviorPos, &nRMinKeyIdx, &nRMinKeyValue);
				
				if(nRKeyIdx != -1)
				{
					bChangeKey = pJoyStick->IsChangeJoySitckInfo(nRKeyIdx) ;
				}			
				if((-1 != nRKeyIdx) && (-1 != nRMinKeyIdx)  
					&& pJoyStick->IsJoystickPOV(nRKeyIdx)		// ąćÇâĹ°ŔÎÁö ĂĽĹ©
					&& pJoyStick->IsJoystickPOV(nRMinKeyIdx))
				{
					BOOL bKeyDown = FALSE;
					int nPow = pJoyStick->GetJoySitckInfo(nRKeyIdx);	// Ĺ°°Ş
					
					if(nRKeyValue > nRMinKeyValue)
					{
						if((nRKeyValue > nPow) && (nPow > nRMinKeyValue))
						{
							bKeyDown = TRUE;
						}
					}
					else if(nRKeyValue < nRMinKeyValue)
					{
						if((nRKeyValue < nPow) && (nPow < nRMinKeyValue))
						{
							bKeyDown = TRUE;
						}
					}
					if(bKeyDown)
					{
						bChangeKey = pJoyStick->IsChangeJoySitckInfo(nRMinKeyIdx) ;
					}
				}
				
			}
			break;
		case DIK_LSHIFT:
			{
				// Lock OnÇŘÁ¦
				int nKeyIdx = 0;
				int nKeyValue = 0;
				
				// şÎ˝şĹÍ
				GetJoystickInfo(JOSTICK_OPTION_TARGET_LOCKON, nBehaviorPos, &nKeyIdx, &nKeyValue);
				
				if(nKeyIdx != -1)
				{
					bChangeKey = pJoyStick->IsChangeJoySitckInfo(nKeyIdx) ;
				}			
			}
			break;
		}
		if(bChangeKey)
		{
			return bChangeKey;
		}
	}

	return bChangeKey;

}
///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		
/// \author		// 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
/// \date		2008-11-13 ~ 2008-11-13
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CInterface::InitJoystickSetupInfo(structJoystickSetupKeyTable		*o_pJoystickSetupKeyTable)
{	
	int nCnt = 0;
	int i =0;
	for(i=0; i< MAX_JOSTICK_KEY_OPTION;i++)
	{					
		memset(&o_pJoystickSetupKeyTable[i], 0x00, sizeof(structJoystickSetupKeyTable));
		o_pJoystickSetupKeyTable[i].nKeySetup = -1;
		for(nCnt=0; nCnt< MAX_JOSTICK_OPTION_SLOT;nCnt++)
		{
			o_pJoystickSetupKeyTable[i].nBehavior[nCnt] = -1;
		}
	}

	//////////////////////////////////  ¸¶żě˝ş ąćÇâĹ¸ //////////////////////////////////  
	{
		//"RX Axis(-)"
		o_pJoystickSetupKeyTable[JOY_SETUP_RXAXIS_MINUS].nKeySetup = JOY_SETUP_RXAXIS_MINUS;	
		o_pJoystickSetupKeyTable[JOY_SETUP_RXAXIS_MINUS].nBehavior[0] = JOSTICK_OPTION_MOUSE_LEFT;		// Çŕµżş° ˝˝·Ô

	}
	{
		//"RX Axis(+)"
		o_pJoystickSetupKeyTable[JOY_SETUP_RXAXIS_PLUS].nKeySetup = JOY_SETUP_RXAXIS_PLUS;	
		o_pJoystickSetupKeyTable[JOY_SETUP_RXAXIS_PLUS].nBehavior[0] = JOSTICK_OPTION_MOUSE_RIGHT;		// Çŕµżş° ˝˝·Ô

	}
	{
		//"RY Axis(-)"
		o_pJoystickSetupKeyTable[JOY_SETUP_RYAXIS_MINUS].nKeySetup = JOY_SETUP_RYAXIS_MINUS;	
		o_pJoystickSetupKeyTable[JOY_SETUP_RYAXIS_MINUS].nBehavior[0] = JOSTICK_OPTION_MOUSE_UP;		// Çŕµżş° ˝˝·Ô

	}
	{
		//"RY Axis(+)"
		o_pJoystickSetupKeyTable[JOY_SETUP_RYAXIS_PLUS].nKeySetup = JOY_SETUP_RYAXIS_PLUS;	
		o_pJoystickSetupKeyTable[JOY_SETUP_RYAXIS_PLUS].nBehavior[0] = JOSTICK_OPTION_MOUSE_DOWN;		// Çŕµżş° ˝˝·Ô

	}

	//////////////////////////////////  ąćÇâ  //////////////////////////////////  
	{
		//"˝ĘŔÚąöĆ°-ÁÂ"
		o_pJoystickSetupKeyTable[JOY_SETUP_POW_LEFT].nKeySetup = JOY_SETUP_POW_LEFT;	
		o_pJoystickSetupKeyTable[JOY_SETUP_POW_LEFT].nBehavior[0] = JOSTICK_OPTION_UNIT_LEFT;		

	}
	{
		//"˝ĘŔÚąöĆ°-żě"
		o_pJoystickSetupKeyTable[JOY_SETUP_POW_RIGHT].nKeySetup = JOY_SETUP_POW_RIGHT;	
		o_pJoystickSetupKeyTable[JOY_SETUP_POW_RIGHT].nBehavior[0] = JOSTICK_OPTION_UNIT_RIGHT;		

	}
	{
		//"˝ĘŔÚąöĆ°-»ó"
		o_pJoystickSetupKeyTable[JOY_SETUP_POW_UP].nKeySetup = JOY_SETUP_POW_UP;	
		o_pJoystickSetupKeyTable[JOY_SETUP_POW_UP].nBehavior[0] = JOSTICK_OPTION_UNIT_GO;		

	}
	{
		//"˝ĘŔÚąöĆ°-ÇĎ"
		o_pJoystickSetupKeyTable[JOY_SETUP_POW_DOWN].nKeySetup = JOY_SETUP_POW_DOWN;	
		o_pJoystickSetupKeyTable[JOY_SETUP_POW_DOWN].nBehavior[0] = JOSTICK_OPTION_UNIT_MIN_SPEED;
	}

	////////////////////////////////// ±âĹ¸ ˝şĹł//////////////////////////////////  
	{
		// Ĺ¸ÄĎ ¶ôżÂ
		o_pJoystickSetupKeyTable[JOY_SETUP_BTN4].nKeySetup = JOY_SETUP_BTN4;	
		o_pJoystickSetupKeyTable[JOY_SETUP_BTN4].nBehavior[0] = JOSTICK_OPTION_TARGET_LOCKON;
	}
	{
		// şÎ˝şĹÍ
		o_pJoystickSetupKeyTable[JOY_SETUP_BTN5].nKeySetup = JOY_SETUP_BTN5;	
		o_pJoystickSetupKeyTable[JOY_SETUP_BTN5].nBehavior[0] = JOSTICK_OPTION_BOSTER;
	}
	{
		// 1Çüą«±â
		o_pJoystickSetupKeyTable[JOY_SETUP_BTN10].nKeySetup = JOY_SETUP_BTN10;	
		o_pJoystickSetupKeyTable[JOY_SETUP_BTN10].nBehavior[0] = JOSTICK_OPTION_WEAPON_1;
	}
	{
		// 2Çüą«±â
		o_pJoystickSetupKeyTable[JOY_SETUP_BTN11].nKeySetup = JOY_SETUP_BTN11;	
		o_pJoystickSetupKeyTable[JOY_SETUP_BTN11].nBehavior[0] = JOSTICK_OPTION_WEAPON_2;
	}

	{
		// •üşä
		o_pJoystickSetupKeyTable[JOY_SETUP_BTN7].nKeySetup = JOY_SETUP_BTN7;	
		o_pJoystickSetupKeyTable[JOY_SETUP_BTN7].nBehavior[0] = JOSTICK_OPTION_BACKVIEW;
	}
	{
		// ŔŻ´ÖÁ¤Áö
		o_pJoystickSetupKeyTable[JOY_SETUP_BTN6].nKeySetup = JOY_SETUP_BTN6;	
		o_pJoystickSetupKeyTable[JOY_SETUP_BTN6].nBehavior[0] = JOSTICK_OPTION_UNITSTOP;
	}
	////////////////////////////////// ±âĹ¸ ˝şĹł//////////////////////////////////  
	{
		// ˝˝·Ô 1
		o_pJoystickSetupKeyTable[JOY_SETUP_BTN0].nKeySetup = JOY_SETUP_BTN0;	
		o_pJoystickSetupKeyTable[JOY_SETUP_BTN0].nBehavior[0] = JOSTICK_OPTION_SLOT_0;
	}
	{
		// ˝˝·Ô 2
		o_pJoystickSetupKeyTable[JOY_SETUP_BTN1].nKeySetup = JOY_SETUP_BTN1;	
		o_pJoystickSetupKeyTable[JOY_SETUP_BTN1].nBehavior[0] = JOSTICK_OPTION_SLOT_1;
	}
	{
		// ˝˝·Ô 3
		o_pJoystickSetupKeyTable[JOY_SETUP_BTN2].nKeySetup = JOY_SETUP_BTN2;	
		o_pJoystickSetupKeyTable[JOY_SETUP_BTN2].nBehavior[0] = JOSTICK_OPTION_SLOT_2;
	}
	{
		// ˝˝·Ô 4
		o_pJoystickSetupKeyTable[JOY_SETUP_BTN3].nKeySetup = JOY_SETUP_BTN3;	
		o_pJoystickSetupKeyTable[JOY_SETUP_BTN3].nBehavior[0] = JOSTICK_OPTION_SLOT_3;
	}
}


///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		
/// \author		// 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
/// \date		2008-11-13 ~ 2008-11-13
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CInterface::OnJoystickKeyDown(int i_nCode)
{
	if(!g_pD3dApp->IsUseJoyStick())
	{
		return FALSE;
	}
	else if(g_pGameMain && g_pGameMain->IsShowOpJoystick())
	{
		// Á¶ŔĚ˝şĆ˝ żÉĽÇĂ˘ŔĚ ¶°ŔÖ´Ů.
		return 0;
	}

	CJoyStickInput *pJoyStick = g_pD3dApp->GetJoystickControl();
	if(!pJoyStick)
	{
		// Á¶ŔĚ˝şĆ˝ ŔĺÄˇ ľřŔ˝
		return FALSE;
	}

	BOOL bKeyDown = FALSE;	
	BOOL bChangeKey = FALSE;
	int nKeyIdx, nKeyValue;
	int nJoyStickStas =0;

	int nBehaviorPos = 0;
	for(nBehaviorPos = 0;nBehaviorPos < MAX_JOSTICK_OPTION_BEHAVIOR;nBehaviorPos++)
	{
		nKeyIdx = nKeyValue = 0;
		
		g_pInterface->GetJoystickInfo(i_nCode, nBehaviorPos, &nKeyIdx, &nKeyValue);
		
		if(nKeyIdx != -1)
		{
			bChangeKey = pJoyStick->IsChangeJoySitckInfo(nKeyIdx) ;
		}
		
		if(bChangeKey)
		{
			nJoyStickStas = pJoyStick->GetJoySitckInfo(nKeyIdx);
		}
		if(nKeyValue == nJoyStickStas)
		{
			bKeyDown = TRUE;
			return bKeyDown;
		}	
	}
	

	return bKeyDown;
}


///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		
/// \author		// 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
/// \date		2008-11-13 ~ 2008-11-13
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CInterface::LoadJoysticOptionInfo(structJoyStickOptionInfo* pJoysticOp)
{
	BOOL bSaveSetup = FALSE;
	char chBuf[512];			
	memset(chBuf, 0x00 ,512);			

	memset(pJoysticOp, 0x00, sizeof(structJoyStickOptionInfo));
	g_pD3dApp->InitJoyStickOptionInfo(pJoysticOp);	
	
	// Á¶ŔĚ˝şĆ˝ »çżë
	if(m_pSetupConfig->GetSetupInfo(SETUP_INFO_JOYSTICK_USE, chBuf))
	{
		if(atoi(chBuf))
		{
			pJoysticOp->bUseJoystick = TRUE;
		}
		else
		{
			pJoysticOp->bUseJoystick = FALSE;

		}
	}
	else
	{
		bSaveSetup = TRUE;
		wsprintf(chBuf, "%d", pJoysticOp->bUseJoystick);
		m_pSetupConfig->SetSetupInfo(SETUP_INFO_JOYSTICK_USE, chBuf);
	}

	// Á¶ŔĚ˝şĆ˝ Ľ±ĹĂ
	if(m_pSetupConfig->GetSetupInfo(SETUP_INFO_JOYSTICK_SELECT, chBuf))
	{
		strncpy(pJoysticOp->chJoySelJoy, chBuf, MAX_PATH-1);
	}
	else
	{		
		bSaveSetup = TRUE;
		m_pSetupConfig->SetSetupInfo(SETUP_INFO_JOYSTICK_SELECT, pJoysticOp->chJoySelJoy);
	}

	// Á¶ŔĚ˝şĆ˝ Ářµż»çżë
	if(m_pSetupConfig->GetSetupInfo(SETUP_INFO_JOYSTICK_FEED, chBuf))
	{
		if(atoi(chBuf))
		{
			pJoysticOp->bUseFeedJoyStick = TRUE;		
		}
		else
		{
			pJoysticOp->bUseFeedJoyStick = FALSE;		
		}
	}
	else
	{
		bSaveSetup = TRUE;
		wsprintf(chBuf, "%d", pJoysticOp->bUseFeedJoyStick);
		m_pSetupConfig->SetSetupInfo(SETUP_INFO_JOYSTICK_FEED, chBuf);
	}

	// Á¶ŔĚ˝şĆ˝ °¨µµ
	if(m_pSetupConfig->GetSetupInfo(SETUP_INFO_JOYSTICK_SENSE, chBuf))
	{
		pJoysticOp->nJoysticSense = atoi(chBuf);				
	}
	else
	{
		bSaveSetup = TRUE;
		wsprintf(chBuf, "%d", pJoysticOp->nJoysticSense);
		m_pSetupConfig->SetSetupInfo(SETUP_INFO_JOYSTICK_SENSE, chBuf);
	}

	// Ĺ° ĽłÁ¤ °ˇÁö°í żŔ±â
	if(m_pSetupConfig->GetSetupInfo(SETUP_INFO_JOYSTICK_KEYCONFIG, chBuf))
	{
		strncpy(pJoysticOp->chJoyKeyFile, chBuf, MAX_PATH-1);		
	}
	else
	{		
		bSaveSetup = TRUE;
		m_pSetupConfig->SetSetupInfo(SETUP_INFO_JOYSTICK_KEYCONFIG, pJoysticOp->chJoyKeyFile);
	}


	if(bSaveSetup)
	{
		char chMaxPath[MAX_PATH];
		memset(chMaxPath, 0x00, MAX_PATH);
		wsprintf(chMaxPath, SETUP_INFO_PATH);
		m_pSetupConfig->SaveSetupInfo(chMaxPath);		
	}	
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		
/// \author		// 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
/// \date		2008-11-13 ~ 2008-11-13
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CInterface::SaveJoystickOptionInfo(structJoyStickOptionInfo* pJoysticOp)
{
	char chBuf[512];			
	memset(chBuf, 0x00 ,512);			

	// Á¶ŔĚ˝şĆ˝ »çżë	
	{		
		wsprintf(chBuf, "%d", pJoysticOp->bUseJoystick);
		m_pSetupConfig->SetSetupInfo(SETUP_INFO_JOYSTICK_USE, chBuf);
	}

	// Á¶ŔĚ˝şĆ˝ Ľ±ĹĂ	
	{			
		m_pSetupConfig->SetSetupInfo(SETUP_INFO_JOYSTICK_SELECT, pJoysticOp->chJoySelJoy);
	}

	// Á¶ŔĚ˝şĆ˝ Ářµż»çżë	
	{		
		wsprintf(chBuf, "%d", pJoysticOp->bUseFeedJoyStick);
		m_pSetupConfig->SetSetupInfo(SETUP_INFO_JOYSTICK_FEED, chBuf);
	}

	// Á¶ŔĚ˝şĆ˝ °¨µµ	
	{		
		wsprintf(chBuf, "%d", pJoysticOp->nJoysticSense);
		m_pSetupConfig->SetSetupInfo(SETUP_INFO_JOYSTICK_SENSE, chBuf);
	}

	// Ĺ° ĽłÁ¤ °ˇÁö°í żŔ±â	
	{				
		m_pSetupConfig->SetSetupInfo(SETUP_INFO_JOYSTICK_KEYCONFIG, pJoysticOp->chJoyKeyFile);
	}
	
}
///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		
/// \author		// 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
/// \date		2008-11-13 ~ 2008-11-13
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
structJoystickSetupKeyTable	 *CInterface::GetJoystickSetupKeyTable(int nKeyIndex)
{
	if(nKeyIndex < 0 || nKeyIndex >= MAX_JOSTICK_KEY_OPTION)
	{
		return NULL;
	}
	return &m_pJoystickSetupKeyTable[nKeyIndex];
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		Á¶ŔĚ˝şĆ˝ Á¤ş¸ ŔúŔĺ
/// \author		// 2008-11-13 by bhsohn Á¶ŔĚ˝şĆ˝ ŔŰľ÷
/// \date		2008-11-13 ~ 2008-11-13
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CInterface::SaveJoysticSetupInfo(char* pJoyKeyFile, structJoyStickOptionInfo	*pstruJoyStickOptionInfo, structJoystickSetupKeyTable	*i_pJoystickSetupKeyTable)
{
	if(0 == strlen(pJoyKeyFile))
	{
		return;
	}
	if((g_pD3dApp && g_pD3dApp->m_pChat) && (g_pJoysticOp->bUseJoystick != pstruJoyStickOptionInfo->bUseJoystick))
	{		 
		char buf[256];
		memset(buf, 0x00, 256);
		if(pstruJoyStickOptionInfo->bUseJoystick)
		{
			wsprintf(buf, STRMSG_C_081120_0202);//"Á¶ŔĚ˝şĆ˝Ŕ» »çżëÇŐ´Ď´Ů."
		}
		else
		{
			wsprintf(buf, STRMSG_C_081120_0203);//"Á¶ŔĚ˝şĆ˝Ŕ» »çżëŔ» Áľ·áÇŐ´Ď´Ů."
		}
		
		g_pD3dApp->m_pChat->CreateChatChild(buf,COLOR_SYSTEM);	
		
		// 2008-12-23 by bhsohn Á¶ŔĚ˝şĆ˝ Ăß°ˇ żäĂ» Ăł¸®
		if(pstruJoyStickOptionInfo->bUseJoystick)
		{
			wsprintf(buf, STRMSG_C_081223_0201);//"\\yÄÁĆ®·Ń·Ż°ˇ ĽłÁ¤µČ »óĹÂżˇĽ­ ¸¶żě˝ş¸¦ »çżëÇĎ±â Ŕ§ÇŘĽ­´Â ±âľî°ˇ Á¤Áö»óĹÂ°ˇ µÇľîľß ÇŐ´Ď´Ů."
			g_pD3dApp->m_pChat->CreateChatChild(buf,COLOR_SYSTEM);	
		}
		// end 2008-12-23 by bhsohn Á¶ŔĚ˝şĆ˝ Ăß°ˇ żäĂ» Ăł¸®
		
	}
	CJoyStickInput *pJoyStick = g_pD3dApp->GetJoystickControl();
	if(pJoyStick)
	{		
		if(0 != stricmp(pJoyStick->GetSelJoystickCtrlDevice(), pstruJoyStickOptionInfo->chJoySelJoy))
		{
			// ŔĺÄˇ şŻ°ć µÇľú´Ů.
			g_pD3dApp->InitJoyStickDevice(TRUE, TRUE, pstruJoyStickOptionInfo->chJoySelJoy);
		}
	}	
	else
	{
		// ŔĺÄˇ°ˇ ľř´Ů
		g_pD3dApp->InitJoyStickDevice(TRUE, TRUE, pstruJoyStickOptionInfo->chJoySelJoy);
	}

	memcpy(g_pJoysticOp, pstruJoyStickOptionInfo, sizeof(structJoyStickOptionInfo));
	
	
	g_pD3dApp->SetUseJoyStick(g_pJoysticOp->bUseJoystick);// Á¶ŔĚ˝şĆ˝ »çżë
	g_pD3dApp->SetUseJoyFeedBack(g_pJoysticOp->bUseFeedJoyStick); // Á¶ŔĚ˝şĆ˝ Ářµż »çżëŔŻą«

	int nCnt = 0;
	int nBehavior = 0;
	char chKeyBuff[MAX_PATH], chBuff[MAX_PATH], chTmpBuff[MAX_PATH];

	memset(chTmpBuff, 0x00, MAX_PATH);	

	for(nCnt = 0;nCnt< MAX_JOSTICK_KEY_OPTION;nCnt++)
	{
		memset(chKeyBuff, 0x00, MAX_PATH);
		GetJoysticInfoIdx_To_String(nCnt, chKeyBuff);
		if(strlen(chKeyBuff) <= 0)
		{
			continue;
		}		
		memset(chBuff, 0x00, MAX_PATH);

		for(nBehavior=0; nBehavior< MAX_JOSTICK_OPTION_SLOT;nBehavior++)
		{
			if(i_pJoystickSetupKeyTable[nCnt].nBehavior[nBehavior] != -1)
			{
				wsprintf(chTmpBuff, "%d,", i_pJoystickSetupKeyTable[nCnt].nBehavior[nBehavior]);
				strcat(chBuff, chTmpBuff);	// µÚżˇ´Ů şŮŔĚŔÚ
			}
		}			
		m_pJoySticConfig->SetSetupInfo(chKeyBuff, chBuff);
	}

	m_pJoySticConfig->SaveSetupInfo(pJoyKeyFile); // ĽÂľ÷°ŞŔúŔĺ

	LoadFileJoysticInfo(m_pJoySticConfig, m_pJoystickSetupKeyTable);		// ĽÂľ÷ ±¸Á¶ĂĽżˇĽ­ ´Ů˝Ă ·Îµĺ
	RefreshJoystickOption();	// ˝ÇÁ¦ ŔűżëÇĎ°Ô˛ű Ŕűżë

	// ĽÂľ÷Á¤ş¸ ŔúŔĺ
	SaveOptionEtcInfo();	
}



// 2010-06-15 by shcho&hslee Ćę˝Ă˝şĹŰ - Ćę Ĺ¸ŔÔ String°Ş ¸®ĹĎ.
/**********************************************************
**
**	Ćę˝Ă˝şĹŰ - Ćę Ĺ¸ŔÔ String°Ş ¸®ĹĎ.
**
**	Create Info :	2010-06-15 by shcho&hslee
**
***********************************************************/
char *CInterface :: GetString_PetType ( const INT a_nPetType )
{

	switch ( a_nPetType )
	{
		case PARTNER_TYPE_NORMAL :	return STRMSG_C_100709_0100; //"±âş»Çü"
#ifdef _INET_PET
		case PARTNER_TYPE_GROWTH:	return STRMSG_C_110902_0001;
#else
		case PARTNER_TYPE_DEF :		return STRMSG_C_100608_0404; //"ąćľîÇü"
		case PARTNER_TYPE_ATT :		return STRMSG_C_100608_0405; //"°ř°ÝÇü"
		case PARTNER_TYPE_BUFF :	return STRMSG_C_100608_0406; //"ąöÇÁÇü"
		case PARTNER_TYPE_STORE :	return STRMSG_C_100608_0407; //"°łŔÎ»óÁˇ"
#endif

		default :
			break;
	}

	return "";

}


// 2010. 05. 12 by hsLee ŔÎÇÇ´ĎĆĽ ÇĘµĺ 2Â÷ UI Ăß°ˇ ĽöÁ¤. (ŔÎ°ÔŔÓ °ĹÁˇ ąćľî ´Ü°č ÇĄ˝Ă.)

/**********************************************************
**
**	Ć˝ ŔĚşĄĆ® °č»ę.
**
**	Create Info :	2010. 05. 12.	by hsLee.
**
***********************************************************/
BOOL IsElapsedTime ( DWORD dwTime , DWORD *p_dwElapsedTime )
{

	if ( dwTime == 0 || NULL == p_dwElapsedTime )
		return FALSE;
	
	if ( *p_dwElapsedTime == 0 )
	{
		*p_dwElapsedTime = timeGetTime();
		return FALSE;
	}
	else
	{
		if ( timeGetTime() - (DWORD)(*p_dwElapsedTime) >= dwTime )
		{
			*p_dwElapsedTime = timeGetTime();

			return TRUE;
		}
	}

	return FALSE;

}
// End 2010. 05. 12 by hsLee ŔÎÇÇ´ĎĆĽ ÇĘµĺ 2Â÷ UI Ăß°ˇ ĽöÁ¤. (ŔÎ°ÔŔÓ °ĹÁˇ ąćľî ´Ü°č ÇĄ˝Ă.)


// 2010. 05. 17 by hsLee ŔÎÇÇ´ĎĆĽ ÇĘµĺ 2Â÷ UI Ăß°ˇ ĽöÁ¤. ´Ü°č ÇĄ˝Ă ąö±× ĽöÁ¤.
BOOL GetStrPara ( char *pStrBuff , INT iNum , char pKey , char *pGetPara )
{

	if ( NULL == pStrBuff || pStrBuff[0] == 0 || NULL == &pGetPara || iNum < 0 )
		return FALSE;

	INT iLoop = 0;
	INT iBuffLen = strlen ( pStrBuff );

	INT iChkNum = 0;
	INT iCur_St = 0;
	INT iCur_Ed = 0;

	for ( iLoop = 0; iLoop < iBuffLen; ++iLoop )
	{
		if ( pStrBuff[iLoop] == pKey )
		{
			iChkNum++;

			if ( iNum == iChkNum )
			{
				iCur_St = iCur_Ed = iLoop+1;
			}
			else if ( iChkNum > iNum )
			{
				iCur_Ed = iLoop;
				break;
			}
		}
	}

	if ( iCur_St >= iBuffLen || iChkNum == 0 )
		return FALSE;

	if ( iChkNum > 0 && iCur_Ed <= iCur_St )
		iCur_Ed = iLoop;

	strncpy ( pGetPara , &pStrBuff[iCur_St] , max ( 1 , iCur_Ed - iCur_St ) );

	return TRUE;

}
// End 2010. 05. 17 by hsLee ŔÎÇÇ´ĎĆĽ ÇĘµĺ 2Â÷ UI Ăß°ˇ ĽöÁ¤. ´Ü°č ÇĄ˝Ă ąö±× ĽöÁ¤.