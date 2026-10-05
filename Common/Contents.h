#pragma once

//----------------------------------------------------------------------------------------------------------//
// 국가별 define                                                                                            //
//----------------------------------------------------------------------------------------------------------//
// _DEBUG
// TEST140
// YEDANG_KOR
// WIKIGAMES_ENG
// YETIME_CHN
// INTECOM_VIET
// GAMEFORGE4D_ENG
// GAMEFORGE4D_DEU
// GAMEFORGE4D_TURKISH
// GAMEFORGE4D_ITALIAN
// GAMEFORGE4D_FRANCE
// GAMEFORGE4D_POL
// GAMEFORGE4D_ESP
// WINNERONLINE_THA
// WINNERONLINE_SGP
// WINNERONLINE_IDN
// INNOVA_RUS
// NETPOWER_TPE
// ARARIO_JPN
// LIN_ARG
//
//----------------------------------------------------------------------------------------------------------//
// define 사용 규약                                                                                         //
//----------------------------------------------------------------------------------------------------------//
//
// 1. 국가별 define 사용해서 묶을 때는 위의 순서 대로 묶고, 공백 역시 정확하게 삽입
// ex1> #if defined(TEST140) || defined(YEDANG_KOR) ( o ) 딩동댕!
// ex2> #if defined(TEST140) ||defined(YEDANG_KOR)  ( x ) 땡!
// ex3> #if defined(YEDANG_KOR) || defined(TEST140) ( x ) 땡!
//
// 2. 전처리기 사용 시 #ifndef 사용을 불허 (대신 #else를 사용)
// ex1> #ifndef TEST140 ( x ) 땡!
//
// 3. define을 분류 할 때 다른 국가에서 사용 가능성이 농후 할 경우 아래로 분리하고, 아닐 경우 위쪽에 분리
//
// 4. define을 생성 시 국가별 특성을 가질 경우 앞쪽에 국가 이니셜 삽입
//
// 5. define을 생성 시 자신의 이니셜을 끝에 삽입
//
// 6. define을 생성 시 서버, 클라에 따라서 C나 S를 삽입, 둘다 사용 할 경우 SC로 삽입
//
// 7. 동일한 define을 절대 2번 선언하지 말 것
//
// 8. 빌드 선택 시 "Win32 Debug"는 폐기된 빌드이며, 기본으로 "Win32 D_Test140" 사용
//
// 9. 국가별 define(ex> TEST140, YEDANG_KOR)은 절대 이 헤더파일(Contents.h) 이외에는 쓰지 말 것
//
//----------------------------------------------------------------------------------------------------------//
// 국가별로 차별화를 두고 다시는 변화하지 않을 작업                                                         //
//----------------------------------------------------------------------------------------------------------//
//#define ARENA
#if defined(_DEBUG) || defined(TEST140)
#define CUSTOM_OPTIMIZER_HSSON					// ini 파일 읽어서 게임에 적용
#define MULTI_LOADER_HSSON						// 클라이언트를 여러개 띄울 수 있도록 수정
#define GAMEGUARD_NOT_EXECUTE_HSSON				// 게임 가드 실행 안함
#endif

#if defined(TEST140) || defined(YEDANG_KOR)
#define KOR_INPUT_LANGUAGE_HSSON				// 입력 언어 변환
#define KOR_HANGEUL_START_HSSON					// 한글 모드로 시작
#define KOR_CASHSHOP_INTERFACE_HSSON			// 캐쉬샵 인터페이스
#define KOR_GAME_RATINGS_HSSON					// 게임 등급물 위원회 아이콘 추가
#define KOR_CHARACTER_INTERFACE_POS_HSSON		// 언어에 따라 위치 수정
#endif

#if defined(YEDANG_KOR)
#define KOR_YEDANG_WEB_LAUNCHER_HSSON			// 예당 웹런쳐
#endif

#if defined(_DEBUG)
#define _ATUM_DEVELOP							// 서버용) 서버소스상 디버깅일 때 활성화 해줘야 함
#endif

#if defined(INNOVA_RUS)
#define _USING_INNOVA_FROST_					// 서버용) 러시아의 경우 게임가드로 포레스트 사용
#endif

#if defined(TEST140)
#define S_140_SERVER_SETTING_HSSON				// 접속할 서버, 파일명 등 국가별 세팅
#endif

#if defined(YEDANG_KOR)
#define S_KOR_SERVER_SETTING_HSSON				// 접속할 서버, 파일명 등 국가별 세팅
#define YEDANG_RELEASE							// 핵쉴드, 몇가지 기능 한국만 수행
#endif

#if defined(ARARIO_JPN)
#define S_JPN_SERVER_SETTING_HSSON				// 접속할 서버, 파일명 등 국가별 세팅
#endif

#if defined(WIKIGAMES_ENG)
#define S_CAN_SERVER_SETTING_HSSON				// 접속할 서버, 파일명 등 국가별 세팅
#endif

#if defined(INNOVA_RUS)
#define S_RUS_SERVER_SETTING_HSSON				// 접속할 서버, 파일명 등 국가별 세팅
#endif

#if defined(INTECOM_VIET)
#define S_VIE_SERVER_SETTING_HSSON				// 접속할 서버, 파일명 등 국가별 세팅
#endif

#if defined(TEST140)
//#define _DEBUG_MAPSETTING						// 키로 색상 바꾸기
#endif

#if defined(TEST140) || defined(YEDANG_KOR) || defined(ARARIO_JPN)
#define S_ARARIO_HSSON							// 아라리오 채널링
#endif

#if defined(ARARIO_JPN)
#define LANGUAGE_JAPAN							// 일본에 기존에 사용하던 디파인
#endif

#if defined(TEST140) || defined(YEDANG_KOR)
#include "Str_KOR/StringDefineCommon.h"
#include "Str_KOR/StringDefineServer.h"
#include "Str_KOR/StringDefineClient.h"
#endif

#if defined(ARARIO_JPN)
#include "Str_JPN/StringDefineCommon.h"
#include "Str_JPN/StringDefineServer.h"
#include "Str_JPN/StringDefineClient.h"
#endif

#if defined(WIKIGAMES_ENG)
	#if defined(WIKI_TOOLS)
#include "Str_CAN/StringDefineCommon.h"
#include "Str_CAN/StringDefineServer.h"
#include "Str_CAN/StringDefineClient.h"	
	#else
//#include "Str_RUS/StringDefineCommon.h"
//#include "Str_RUS/StringDefineServer.h"
//#include "Str_RUS/StringDefineClient.h"


#include "Str_CAN/StringDefineCommon.h"
#include "Str_CAN/StringDefineServer.h"
#include "Str_CAN/StringDefineClient.h"

	#endif

#endif

#if defined(INNOVA_RUS)
#include "Str_RUS/StringDefineCommon.h"
#include "Str_RUS/StringDefineServer.h"
#include "Str_RUS/StringDefineClient.h"
#endif

#if defined(INTECOM_VIET)
#include "Str_VIE/StringDefineCommon.h"
#include "Str_VIE/StringDefineServer.h"
#include "Str_VIE/StringDefineClient.h"
#endif



//----------------------------------------------------------------------------------------------------------//
// 국가별로 차별화를 두지만 컨텐츠 레벨(진도) 또는 요구(특성)에 따라서 변화 될 작업                         //
//----------------------------------------------------------------------------------------------------------//
#if defined(INTECOM_VIET) || defined(LIN_ARG)
#define ONLY_FULL_WINDOW_HSSON					// 무조건 전체화면으로 게임 진행, 알트 + 탭하면 팅구기
#endif

#if defined(TEST140) || defined(YEDANG_KOR) 
#define GUILD_WAREHOUSE_ACCESS_HSSON			// 멤버쉽이 아니더라도 여단창고 이용가능
#endif

#if defined(TEST140) || defined(YEDANG_KOR)
#define NEW_CASHSHOP_INTERFACE_HSSON			// 신규 캐쉬샵 작업
#endif

#if defined(TEST140) || defined(YEDANG_KOR)
#define KOR_CASHSHOP_REFILL_WEB_HSSON			// 캐쉬샵 충전 웹페이지 추가
#endif

#if defined(TEST140) || defined(YEDANG_KOR)
#define WAREHOUSE_SHARES_HSSON					// 캐릭터 간에 창고를 공유 할 수 있음
#endif

#if defined(TEST140)
#define SC_GROWING_PARTNER_HSKIM_JHAHN			// 캐릭터 정보 창에서 파트너UI 활성화
#endif

#if defined(TEST140) || defined(YEDANG_KOR) || defined(ARARIO_JPN) || defined(WIKIGAMES_ENG) || defined(INTECOM_VIET) || defined(INNOVA_RUS)
#define S_INFINITY3_HSKIM						// 인피니티 3차 적용 (서버)
#define INFI_QUEST_JSKIM					    // 인피니티 퀘스트
#define MULTI_TARGET_JSKIM						// 멀티타켓
#define S_AUTHENTICATION_SERVER_HSKIM			// 인증 서버 적용 (서버)
#define SC_SECURITY_COMMAND_HSKIM				// 비밀키 적용 (클라 서버) [선행조건 (인증 서버 적용)]
//#define C_SECURITY_COMMAND_JHAHN				// 비밀키 클라이언트 확인 - 절대 주석 풀지 마세요
#endif

#if defined(TEST140) || defined(YEDANG_KOR) || defined(WIKIGAMES_ENG) || defined(YETIME_CHN) || defined(INTECOM_VIET) || defined(GAMEFORGE4D_ENG) || defined(GAMEFORGE4D_DEU) || defined(GAMEFORGE4D_TURKISH) || defined(GAMEFORGE4D_ITALIAN) || defined(GAMEFORGE4D_FRANCE) || defined(GAMEFORGE4D_POL) || defined(GAMEFORGE4D_ESP) || defined(WINNERONLINE_THA) || defined(WINNERONLINE_SGP) || defined(WINNERONLINE_IDN) || defined(INNOVA_RUS) || defined(NETPOWER_TPE) || defined(ARARIO_JPN) || defined(LIN_ARG)
#define S_ACCESS_INTERNAL_SERVER_HSSON		// 접속하는 IP를 내부 테스트 서버로 설정
#endif

#if defined(TEST140)
#define SC_DARK_CRASH_HSSON						// 클라이언트 크래쉬 날때 서버로 클라 정보 전송
#endif

#if defined(TEST140)
#define SC_DARK_CRASH_FILE_OUTPUT_HSSON			// 클라이언트 크래쉬 날때 클라 정보를 txt파일로 남길지 여부
#endif

#if defined(TEST140)
#define S_GLOG_HSSON						    // GLog 기능
#endif

#if defined(TEST140) || defined(YEDANG_KOR) || defined(ARARIO_JPN) || defined(WIKIGAMES_ENG) || defined(INTECOM_VIET) || defined(INNOVA_RUS)
#define S_LOGIN_ITEM_EVENT_SHCHO			    // 로그인시 아이템 증정
#endif

#if defined(TEST140) || defined(YEDANG_KOR) || defined(ARARIO_JPN) || defined(INTECOM_VIET)
#define S_AUTO_UPDATE_VERSION_BY_PRESVR_HSSON	// 자동 업데이트 버전
#endif

#if defined(INTECOM_VIET) || defined(INNOVA_RUS)
#define S_ARENA_NOT_INFO						// 아레나에서는 할 수 없게 함
#endif

#if defined(TEST140) || defined(YEDANG_KOR)
#define SC_SHUT_DOWNMIN_SHCHO					// 셧다운제(만 16세 미만 접속 제한)
#endif
#if defined(INTECOM_VIET)
#define S_MANAGER_ADMIN_HSSON					// 게임 마스터도 계정 블럭/언블럭 시킬 수 있도록 수정
#endif


#define _INET_CHANGED_ATTR_DESC					//17-04-2016 changed descriptions of items (if can be enchanted, moved etc.)
#define _INET_SKILL_DOUBLECLICK_BUY				//17-04-2016 double click to skill buy
#define _INET_C_SUPSH_AFTER_RESUPLY				//17-04-2016 close automatically suply shop after resuply
#define _INET_LEFT_TRGET_FIX					//17-04-2016 fix for left arrow of targeting monster (now its not on monstername)
#define _INET_CHANGE_QUIT_MESSAGE				//17-04-2016 change quit/logout messagebox
#define _INET_ENCHANT_CHANCE					//17-04-2016 dynamic enchant chance check in lab
#define _INET_4_CHARS							//17-04-2016 4 characteers per account
#define _INET_ADDITIONAL_HUD					//19-04-2016 additional hud near aim (hp,ammo etc)
#define _INET_SHOW_GUILD_NAME					//19-04-2016 guild name under nickname
//#define _INET_NO_ENUM							//24-04-2016 uncomment this to use old predefinitions instead of simple enumerators by me (changes for now only in combo boxes in sys option)
#define _INET_DEFAULT_COLOR						//24-04-2016 players can setup their favorite color to always use on chat - no need to write this by player it will add automatically
#define _INET_TRADE_INTERNATIONAL				//24-04-2016 international chat for trade chat with command to disable it *DEFAULT :: ENABLED AFTER SERVER START*
#define _INET_WHISPER_WHEN_INTERNATIONAL		//25-04-2016 when international trade chat is enabled by command then players can whisper other nation - avoid spam on trade chat // you have 2 international chats - trade and overall both can be used by command and enabled/disabled
#define _INET_BLACK_MARKET						//24-04-2016 allow users to warp to freeska map for trade
#define _INET_LE_FIX							//26-04-2016 fix for lucky enchant
#define _INET_PET								//30-04-2016 adding PET's with sockets and leveling up allowed (growing Partner system)
#define S_WAR_SYSTEM_RENEWAL_STRATEGYPOINT_JHSEOL //14-05-2016 mod from ep4 - better sp system
#define _INET_MAC_ADDRESS_CHECKER		//2016-03-06 by inet - get mac address and check if its not banned
#ifdef _INET_MAC_ADDRESS_CHECKER
	#define _BERGI_MAC_CHECK // 2017-06-18 by bergi - deny login if macaddress not received
#endif
#define _INET_CONFIGURATOR				//2016-03-12 by inet - another program to configure graphic engine (not enough space in launcher)
#define _INET_FACTORY8LAB_DB_CLICK		//2016-09-03 by inet - double click to put item in factory or laboratory

#define _INET_FEATURE_CFG				//03-12-2016 by inet - enable Features.cfg file with config credits per hour and wp per kill
///FOR _INET_FEATURE_CFG//////
#if defined(_INET_FEATURE_CFG) //don't touch
	#define _INET_CREDITS_PER_HOUR		//03-12-2016 by inet - enable credit per hour giving to player
	#define _INET_WP_PER_KILL			//03-12-2016 by inet - giving wp per kill to player (setup in config if it should be predefined or dynamic)
	#define _INET_COMMAND_FOR_RELOAD_FEATURES //03-12-2016 by inet - command for reload features.cfg
#endif
/////////////////////////////////

//#define BONUS_STAT_ITEM					//Sami bonus stat - 04-12-2016 implemented by inet on 3.5
//#define BONUS_STAT_INVERTED				//04-12-216 by inet - inversion o fSmai bonus stat
#define _INET_LINK_CHAT					//04-12-2016 by inet - link item on chat
#define SILVER_DISPLAY_SP_WAR_INFO		//strategypoint show by silver

#define _INET_RES_MODS_DIR				//11-12-2016 by Inet :: res-mods dir for put mods
///FOR _INET_RES_MODS_DIR//////
#ifdef _INET_RES_MODS_DIR
	#undef CHECK_SUM_ON_RES_OBJ			//allow users to put objects mods
#endif
/////////////////////////////////

#define _BERGI_FILTER_PACKETS			//2016-11-01 by Bergi9 - filter packets like a packetfilter //copied from 4.3 by inet
#define _INET_ANTICHEAT					//12-12-2016 by inet - anti cheat - check for running programs and server side item check improoved 17-09-2023 as anticheat 2.0
#define _INET_RANKS						//13-12-2016 by inet - reworked ranks near player name (silver mod) 

//#define _INET_NOMOREAGTROLL				//by Inetpub - ag cannot fall down from objects and using groundskills if not at ground - enables/disables whole mod do not comment other lines
///FOR _INET_NOMOREAGTROLL//////
#ifdef _INET_NOMOREAGTROLL
	#define _INET_NOMOREAGTROLL_REACTION_HEIGHT 50.0f //minimum 0.2f - ag height over ground when he will be taked off or moved rly fast down
	#define _INET_NOMOREAGTROLL_MODE 1	//1- fall like a stone when height is more than _INET_NOMOREAGTROLL_REACTION_HEIGHT, 0- takeof when height is more than _INET_NOMOREAGTROLL_REACTION_HEIGHT
#endif
/////////////////////////////////

#define _INET_FOV						//by Inetpub - field of view feature
///FOR _INET_FOV//////
#define _INET_FOV_MIN	60				//minimum fov angle
#define _INET_FOV_MAX   90				//maximum fov angle
//////////////////////

#define _INET_CREDITS_IN_MAP			//04-03-2017 by inet - possible to add credits at specified map (usage for sp, events etc)

#define	_INET_CREDITS_PER_SP			//enable credits per sp
#define	_INET_CREDITS_PER_MS			//enable credits per ms
#define	_INET_CREDITS_PER_AB			//enable credits per ab

#define _INET_SORT_INV							//04~05-03-2017 by inet - sort inventory option

#define _INET_TM
								// 30~31.03.2017 by inet - advanced Team ranks with custom privilages
//#define _INET_CANNOT_CRASH_UNDER_ATT	//2016-03-03 by inet - cannot crash your gear while youre under attack

#define _INET_PREVENT_COH_UNDER_HP				//20-05-2017 by Inet - forbid coh if hp is lower than setted up 
#ifdef _INET_PREVENT_COH_UNDER_HP				//cfg for mod with coh prevention - values are in percent
	#define _INET_COH_MIN_HP_IG	25
	#define _INET_COH_MIN_HP_AG 25
	#define _INET_COH_MIN_HP_MG 25
	#define _INET_COH_MIN_HP_BG 25
#endif

#define _INET_MAPWARP_INFO			//12-08-2017 by Inet - add info about selected map while warping there from city
#define _INET_DEFAULT_WEATHER		//26-08-2017 by Inet - set weather on selected maps by default
#define _INET_DEBUG			//used for test new features before they will get own name in contents.h
#define _INET_PARTY_GEAR	//27-08-2017 by Inet - shop party members gear

#define _INET_DRANKS	//12-10-2017 by Inet - ranks for donators instead of color names
#define _INET_RANKBUFF  //15-10-2017 by Inet - ranks (normal) will give some bonus

#define _INET_LOG_ALL //19-12-2017 by Inet - every player action is logged (no more mss disabled logs)
#define _INET_FIX_DUPLICATE_ITEM //19-12-2017 by Inet - fix duplicate item by guild wh - change delete item cycle from 1h to 26,5
#define _INET_BAZAR_BUGFIX	//19-12-2017 by Inet - fix for bazar hangs (if client get hard lag during bazar open it should release it and close bazar)
#define _INET_FIX_MOUSE //19-12-2017 by Inet - Fixed not reset mouse focus when game is in background
#define _INET_SERVER_DMP //19-12-2017 by Inet - Server binaries will create DMP files now

#ifdef _ATUM_CLIENT
	//#define _OLD_SHADOW_SYSTEM
	#define _BERGI_PARALLEL
	#ifdef _BERGI_PARALLEL
		#define COLLISION_CHECK_THREADS 2
	#endif
	#define _BERGI_NEW_SKINNED_MESH_LIB
	//#define _BERGI_CLIENT_DEBUG_LOG
	#ifdef _BERGI_CLIENT_DEBUG_LOG
		//#define _BERGI_CLIENT_DEBUG_LOG_EXT
	#endif
#endif

#define _BERGI_SP_DISTRIBUTE // 12-04-2021 by bergi - truely distribute the SP along the day

#define _INET_FRIEND_EXP	0.025f //18-02-2018 by Inet - easy modify friend bonus exp (ex. 0.005f = 0,5%)
#define _INET_PARTY_MAP				//27-02-2018 by Inet - show formation member location (when mouse over name shows little popup)

#define _INET_RESTRICT_QSLOT_PORTALS //15-09-2018 by inet - define to disallow players to put express return portals at quickbar
#define _INET_ALLOW_SPAWN_SP_AT_MSDAY	//15/09/18 by Inet - allow sp spawning at ms day
#define _INET_LEVELING_SP				//16/09/18 by Inet - spawn sp by nPC point difference (9 levels)

#define _INET_DISCORD_BOT_PACKETS		//19/09.18 by Inet - added packets for discord bot notifications (spawn sp/ms/bases)

#define _INET_EVENTMONSTER_CHANGE		//22-09-2018 by Inetpub - added possibility to restrict goldies spawn to killed npc types only (ex. you kill Sandmall you got ONLY gold Sandmall)

#define _INET_PING						//02-01-2019 by Inet - show latency (ping)

#define _INET_AUTO_LEADER_SET			//13-08-2019 by Inet - auto infl leader set

//23-05-2020 by Inetpub - quickslot will save automatically like ep4
#define QUICKSLOT_SAVE_CHECK_TIME		(60.0f)

#define _INET_ANTI_MACRO				//13-06-2020 by Inet - prevention before using macro in laboratory

#define _CLOSE_STONE_RUINS_AT_MS		//close SR at ms

//09-08-2020 restrict maps entering by level
#define _RESTRICTED_MAP_IDX_1			3042 //BL
#define _RESTRICTED_MAP_LV_1			100
#define _RES_EVENT_IDX_1_1				9103 //tp at ssb->bl
#define _RES_EVENT_IDX_1_2				9102 //tp2 at ssb->bl

#define _RESTRICTED_MAP_IDX_2			3041 //Robenia City
#define _RESTRICTED_MAP_LV_2			105
#define _RES_EVENT_IDX_2_1				9201 //tp at bl->rc

#define _RESTRICTED_MAP_IDX_3			3043 //Vatalus Lab
#define _RESTRICTED_MAP_LV_3			110
#define _RES_EVENT_IDX_3_1				9401 //tp at rc -> vl

#define _RESTRICTED_MAP_IDX_4			3044 //Robenia City Orbit
#define _RESTRICTED_MAP_LV_4			117
#define _RES_EVENT_IDX_4_1				9301 //tp at rc -> rco

#define _RESTRICTED_MAP_IDX_5			3086 //J-ARK Hangar
#define _RESTRICTED_MAP_LV_5			118
#define _RES_EVENT_IDX_5_1				9762 //tp rco->jh

#define _RESTRICTED_MAP_IDX_6			3087 //J-ARK Core
#define _RESTRICTED_MAP_LV_6			119
#define _RES_EVENT_IDX_6_1				9782 //tp jh->jer

#define _IS_ADMIN_RESTRICTED_MAP(__MAP_INDEX)	(/* (_RES_EVENT_IDX_2_1==(__MAP_INDEX))||*/ (_RES_EVENT_IDX_3_1==(__MAP_INDEX)) || (_RES_EVENT_IDX_4_1==(__MAP_INDEX)) || (_RES_EVENT_IDX_5_1==(__MAP_INDEX)) || (_RES_EVENT_IDX_6_1==(__MAP_INDEX)) \
													/*|| (_RESTRICTED_MAP_IDX_2==(__MAP_INDEX))*/|| (_RESTRICTED_MAP_IDX_3==(__MAP_INDEX)) || (_RESTRICTED_MAP_IDX_4==(__MAP_INDEX)) || (_RESTRICTED_MAP_IDX_5==(__MAP_INDEX)) || (_RESTRICTED_MAP_IDX_6==(__MAP_INDEX)) )	

#define _INET_SOCIAL_BUTTONS //buttons for FB and Discord open

#define _INET_BOSS_DB_CONTROL //17-09-2020 control bossess spawn by DB

#define S_ENABLE_KILLFEED                                        // classical mapwide killfeed

#define _INET_EXTENDED_OPTIONS					//26-01-2022 more options in opt window

#define _INET_INVEN_RIGHT_CLICK_MENU	//04-11-2022 right click menu from ep4 in inventory

#define _REMOVE_ITEMS_AT_MAP

#define _INET_AT_ALL_CHATS//17-0-2023 by Inet - admin tool will listen to all chats

//07-08-2023 by Inet - check for multiple account logged in from this IP and allow only defined count
#define _INET_MULTIACCOUNT_CHECK
#define _INET_MAX_ACC_PER_IP  2

#define _INET_COLORED_NAMES_DONATOR_RANK //17-09-2023 by Inet colored names for donator wings

#define _INET_SELECT_ITEM_WITH_SHIFT //04-10-2023 by Inet - select inven items with shift for select range of items

#define _INET_DISABLE_BOSS_DROPS //disable boss armors drop

#define _INET_BREAK_FORM_FLIGHT_WHEN_SIEGE //18.12.2025 Inet - when using siege mode exclude from form flight