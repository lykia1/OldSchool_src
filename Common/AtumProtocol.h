/******************************************************************************
	ATUM C/S Protocol 정의

	* protocol header ( size of data + 암호화 정보 + seq. #), 무조건 4 bytes
	----------------------------------------------------
	| size(2B) | encode_flag(1bit)+N(7bit)| seq. # (1B)|
	----------------------------------------------------

	* 암호화된 데이터인 경우 (자세한 사항은 이 파일 아래 부분 참조)
	<------------------- 4B --------------->
	-----------------------------------------------------------------------
	| size | encode_flag +   N    | seq. # | MSGs ... | dummy  | checksum |
	| (2B) |   (1bit)    + (7bit) |  (1B)  |          | (0~3B) |   (1B)   |
	-----------------------------------------------------------------------
                                  <- XOR -><----------- XOR -------------->

	* 암호화되지 않은 데이터인 경우
	---------------------------------------------------
	| size | encode_flag +   N    | seq. # | MSGs ... |
	| (2B) |   (1bit)    + (7bit) |  (1B)  |          |
	---------------------------------------------------
           <----------- 0x0000 ------------>

	* message header
	-------------------------
	| Type1(1B) | Type2(1B) |
	-------------------------

	작성자: 이윤원(ywlee@webcallworld.com)
	작성일: 2002. 9. 23.

******************************************************************************/


/* JUST FOR EDITING
0x00
0x01
0x02
0x03
0x04
0x05
0x06
0x07
0x08
0x09
0x0A
0x0B
0x0C
0x0D
0x0E
0x0F
0x10
0x11
0x12
0x13
0x14
0x15
0x16
0x17
0x18
0x19
0x1A
0x1B
0x1C
0x1D
0x1E
0x1F
0x20
0x21
0x22
0x23
0x24
0x25
0x26
0x27
0x28
0x29
0x2A
0x2B
0x2C
0x2D
0x2E
0x2F
*/

#ifndef __ATUM_PROTOCOL_H__
#define __ATUM_PROTOCOL_H__

#include "windows.h"
#include "DefineGlobal.h"
#include "SocketHeader.h"
#include "LocalizationDefineCommon.h"		// 2009-06-12 by cmkwon, 대만 클라이언트 컴파일 문제 해결 - 
#include "AtumParam.h"
#include "time.h"
#include "ThreadCheck.h"

#if defined(_ATUM_CLIENT)		// 2008-03-25 by cmkwon, 클라이언트 실행 파일
	// 2008-04-03 by cmkwon, 핵쉴드 서버 연동 시스템 수정 - 아래의 헤더 파일로 변경됨
	//#include "AntiCpSvrFunc.h"				// 2008-03-24 by cmkwon, 핵쉴드 2.0 적용 - AntiCpSvrFunc.h 파일의 정의를 그대로 사용하고 클라이언트에도 헤더파일 전달하기
	// 2008-12-19 by cmkwon, 한국 Yedang 핵쉴드 모니터링 서버 설정 추가 - 
	//#include "AntiCpXSvr.h"					// 2008-04-03 by cmkwon, 핵쉴드 서버 연동 시스템 수정 - 
	#include "HShield.h"					// 2008-12-19 by cmkwon, 한국 Yedang 핵쉴드 모니터링 서버 설정 추가 - 
#else
	// 2008-04-03 by cmkwon, 핵쉴드 서버 연동 시스템 수정 - 
	//#include "Security\AntiCpSvrFunc.h"		// 2008-03-24 by cmkwon, 핵쉴드 2.0 적용 - AntiCpSvrFunc.h 파일의 정의를 그대로 사용하고 클라이언트에도 헤더파일 전달하기
	#include "Security\AntiCpXSvr.h"		// 2008-04-03 by cmkwon, 핵쉴드 서버 연동 시스템 수정 - 
#endif



#define PRE_SERVER_PORT			15100
#define IM_SERVER_PORT			15101
#define LOG_SERVER_PORT			15102
#define FIELD_SERVER_PORT		15103
#define NPC_SERVER_PORT			15104
//#define NPC_MONITOR_PORT		N/A

const char* GetProtocolTypeString(MessageType_t msgType);
const char* GetGameLogTypeString(MessageType_t msgType);
const char* GetItemUpdateTypeString(BYTE i_byItemUpdateTy);				// 2006-10-27 by cmkwon
const char* GetItemDeleteTypeString(BYTE i_byItemDeleteTy);				// 2008-01-23 by cmkwon, S_F, S_L: 장착/장착해제 게임 로그에 추가 - GetItemDeleteTypeString() 추가


void PrintExchangeMsg(BYTE SendOrRecv, MessageType_t nType, char *peerIP, ENServerType st = ST_INVALID_TYPE, BYTE printLevel = PRINTLEVEL_NO_MSG);

///////////////////////////////////////////////////////////////////////////////
// Message Type 0 (대분류)
///////////////////////////////////////////////////////////////////////////////
/*
//#define T0_PC_DEFAULT_UPDATE		0x00
//#define T0_PC_CONNECT				0x01
//#define T0_FN_CONNECT				0x02
//#define T0_FC_CONNECT				0x03
//#define T0_FP_CONNECT				0x04
//#define T0_IP_CONNECT				0x05
//#define T0_FI_CONNECT				0x06
//#define T0_IC_CONNECT				0x07
//#define T0_PM_CONNECT				0x08
//#define T0_FM_CONNECT				0x09
//#define T0_LM_CONNECT				0x0A
//#define T0_IM_CONNECT				0x0B
//#define T0_NM_CONNECT				0x0C
//#define T0_PL_CONNECT				0x0D
//#define T0_IL_CONNECT				0x0E
//#define T0_FL_CONNECT				0x0F
//#define T0_NL_CONNECT				0x10
//#define T0_FI_EVENT					0x11
//#define T0_IC_CHAT					0x12
//#define T0_FI_CHAT					0x13
//#define T0_FC_CHAT					0x14
//#define T0_FC_CHARACTER				0x15
//#define T0_FN_CHARACTER				0x16
//#define T0_FC_MOVE					0x17
//#define T0_FN_MOVE					0x18
//#define T0_FC_BATTLE				0x19
//#define T0_FN_BATTLE				0x1A
//#define T0_FC_PARTY					0x1B
//#define T0_FI_PARTY					0x1C
//#define T0_IC_PARTY					0x1D
//#define T0_FC_MONSTER				0x1E
//#define T0_FN_MONSTER				0x1F
//#define T0_FC_EVENT					0x20
//#define T0_FN_EVENT					0x21
//#define T0_FP_EVENT					0x22
//#define T0_FC_STORE					0x23
//#define T0_FC_ITEM					0x24
//#define T0_FC_SHOP					0x25
//#define T0_FC_TRADE					0x26
//#define T0_FC_GUILD					0x27
//#define T0_FI_GUILD					0x28
//#define T0_IC_GUILD					0x29
//#define T0_FC_SKILL					0x2A
//#define T0_FN_SKILL					0x2B
//#define T0_FC_QUEST					0x2C
////#define T0_FC_SYNC					0x2D
//#define T0_FC_INFO					0x2E
//#define T0_FC_REQUEST				0x2F	// 캐릭터간의 요청, 수락, 거절 등에 쓰임, general-purpose
//#define T0_FC_CITY					0x30	// 도시용 프로토콜
//// monitor protocol
//#define T0_FM_MONITOR				0x31
//#define T0_PM_MONITOR				0x32
//#define T0_NM_MONITOR				0x33
//#define T0_IM_MONITOR				0x34
//#define T0_LM_MONITOR				0x35
//// admin protocol
//#define T0_FI_ADMIN					0x36
//#define T0_IC_ADMIN					0x37
//#define T0_FC_ADMIN					0x38
//// SendErrorMessage등에 Type으로 사용하기 위해
//#define T0_PRE						0x40
//#define T0_IM						0x41
//#define T0_FIELD					0x42
//#define T0_NPC						0x43
//#define T0_TIMER					0x44
//#define T0_DB						0x45
//
//#define T0_FC_RACING				0x50	// 레이싱 시스템에서 사용
//#define T0_FC_TIMER					0x51
//
//// VoIP
//#define T0_ATUMVOIP					0x60				// 음성 통화를 위해
//
//// 캐릭터의 정보 전송
//#define T0_FI_CHARACTER				0x90
//// admin tool protocol
//#define T0_PA_ADMIN					0xA0
//#define T0_IA_ADMIN					0xA1
//#define T0_FA_ADMIN					0xA2
//#define T0_NA_ADMIN					0xA3
//// INVALID PROTOCOL
//#define T0_NA						0xD0	// NOT AVAILABLE
//// log protocol
//#define T0_FL_LOG					0xE0
//// debugging protocol
//#define T0_FC_CLIENT_REPORT			0xFC	// C -> F
//#define T0_IC_STRING				0xFD
//#define T0_FC_STRING				0xFE
//// error protocol
//#define T0_ERROR					0xFF
*/
// 기본 업데이트용 프로토콜, 수정 불가
#define T0_PC_DEFAULT_UPDATE  0xE1
#define T0_PC_CONNECT  0x80
#define T0_FN_CONNECT  0x63
#define T0_FC_CONNECT  0x10
#define T0_FP_CONNECT  0x82
#define T0_IP_CONNECT  0x37
#define T0_FI_CONNECT  0xC0
#define T0_IC_CONNECT  0xA4
#define T0_PM_CONNECT  0x64
#define T0_FM_CONNECT  0x17
#define T0_LM_CONNECT  0x47
#define T0_IM_CONNECT  0x34
#define T0_NM_CONNECT  0x39
#define T0_PL_CONNECT  0x4B
#define T0_IL_CONNECT  0x84
#define T0_FL_CONNECT  0x48
#define T0_NL_CONNECT  0x38
#define T0_MF_TO_AF  0xFF
#define T0_PP_CONNECT  0x40
#define T0_FC_ADMIN  0xB2
#define T0_FC_BATTLE  0x67
#define T0_FC_CHARACTER  0x30
#define T0_FC_CHAT  0x50
#define T0_FC_CITY  0x41
#define T0_FC_CLIENT_REPORT  0x3B
#define T0_FC_EVENT  0x1B
#define T0_FC_GUILD  0x43
#define T0_FC_INFO  0x44
#define T0_FC_ITEM  0x4D
#define T0_FC_MONSTER  0x1F
#define T0_FC_MOVE  0x54
#define T0_FC_PARTY  0x18
#define T0_FC_QUEST  0x56
#define T0_FC_RACING  0x33
#define T0_FC_REQUEST  0x58
#define T0_FC_SHOP  0x83
#define T0_FC_SKILL  0x53
#define T0_FC_STORE  0x12
#define T0_FC_STRING  0xC3
#define T0_FC_TIMER  0x61
#define T0_FC_TRADE  0x11
#define T0_FC_COUNTDOWN  0x73
#define T0_FC_OBJECT  0x89
#define T0_FC_AUCTION  0x20
#define T0_FC_CITYWAR  0x52
#define T0_FC_WAR  0x57
#define T0_FC_BAZAAR  0x21
#define T0_FC_ARENA  0x3E
#define T0_FC_TUTORIAL  0x55
#define T0_FC_OUTPOST  0xFE
#define T0_FC_INFINITY  0xB3
#define T0_FI_ADMIN  0x32
#define T0_FI_CHARACTER  0x14
#define T0_FI_CHAT  0x15
#define T0_FI_EVENT  0xB1
#define T0_FI_GUILD  0x81
#define T0_FI_PARTY  0x3C
#define T0_FI_CITYWAR  0x13
#define T0_FI_CASH  0x4C
#define T0_FI_INFO  0x85
#define T0_FN_BATTLE  0xA1
#define T0_FN_CHARACTER  0x71
#define T0_FN_EVENT  0x49
#define T0_FN_MONSTER  0xC2
#define T0_FN_MOVE  0x36
#define T0_FN_SKILL  0x3A
#define T0_FN_CITYWAR  0xD0
#define T0_FN_NPCSERVER  0x99
#define T0_FP_EVENT  0x87
#define T0_FP_MONITOR  0x66
#define T0_FP_CASH  0x16
#define T0_FP_ADMIN  0x3D
#define T0_IC_ADMIN  0xA2
#define T0_IC_CHAT  0x60
#define T0_IC_GUILD  0x1C
#define T0_IC_PARTY  0x19
#define T0_IC_STRING  0x35
#define T0_IC_COUNTDOWN  0x45
#define T0_IC_CITYWAR  0x3F
#define T0_IC_VOIP  0x88
#define T0_IC_CHATROOM  0x90
#define T0_IC_INFO  0x65
#define T0_FL_LOG  0x1D
#define T0_PAUTH_CONNECT  0x4F
#define T0_PM_MONITOR  0x46
#define T0_IM_MONITOR  0x62
#define T0_LM_MONITOR  0x00
#define T0_FM_MONITOR  0x86
#define T0_NM_MONITOR  0x4E
#define T0_PA_ADMIN  0xE0
#define T0_IA_ADMIN  0xC1
#define T0_FA_ADMIN  0x42
#define T0_NA_ADMIN  0x1E
#define T0_PRE  0x70
#define T0_IM  0xA3
#define T0_FIELD  0x31
#define T0_NPC  0x72
#define T0_ATUMVOIP  0x51
#define T0_TIMER  0xA0
#define T0_DB  0xB0
#define T0_ERROR  0x1A
#define T0_NA  0x4A
#define T0_FC_MARKET  0xA5
///////////////////////////////////////////////////////////////////////////////
// Message Type 1 (소분류)
///////////////////////////////////////////////////////////////////////////////
// PC_DEFAULT_UPDATE: Atum.exe를 위해 사용함, Launcher 업데이트용
#define T1_PC_DEFAULT_UPDATE_LAUNCHER_VERSION		0x01
#define T1_PC_DEFAULT_UPDATE_LAUNCHER_UPDATE_INFO	0x02
#define T1_PC_DEFAULT_UPDATE_LAUNCHER_VERSION_OK	0x03
#define T1_PC_DEFAULT_NEW_UPDATE_LAUNCHER_VERSION		0x10		// 2007-01-08 by cmkwon
#define T1_PC_DEFAULT_NEW_UPDATE_LAUNCHER_UPDATE_INFO	0x11		// 2007-01-06 by cmkwon, FTP or HTTP를 이용한 Launcher update


// PC_CONNECT
#define T1_PC_CONNECT					0x00		// 사용하지 않음
#define T1_PC_CONNECT_OK				0x01		// 사용하지 않음
#define T1_PC_CONNECT_CLOSE				0x02
#define T1_PC_CONNECT_ALIVE				0x03
#define T1_PC_CONNECT_VERSION			0x04
#define T1_PC_CONNECT_UPDATE_INFO		0x05
#define T1_PC_CONNECT_VERSION_OK		0x06
#define T1_PC_CONNECT_REINSTALL_CLIENT	0x07
#define T1_PC_CONNECT_LOGIN				0x08
#define T1_PC_CONNECT_LOGIN_OK			0x09
#define T1_PC_CONNECT_SINGLE_FILE_VERSION_CHECK		0x10		// single file에 대한 버전 확인(deletefilelist.txt, notice.txt 등)
#define T1_PC_CONNECT_SINGLE_FILE_VERSION_CHECK_OK	0x11
#define T1_PC_CONNECT_SINGLE_FILE_UPDATE_INFO		0x12
#define T1_PC_CONNECT_GET_SERVER_GROUP_LIST			0x13
#define T1_PC_CONNECT_GET_SERVER_GROUP_LIST_OK		0x14

#define T1_PC_CONNECT_GET_GAME_SERVER_GROUP_LIST		0x20		// 2007-05-02 by cmkwon, C->P
#define T1_PC_CONNECT_GET_GAME_SERVER_GROUP_LIST_OK		0x21		// 2007-05-02 by cmkwon, P->C
#define T1_PC_CONNECT_NETWORK_CHECK						0x22		// 2007-06-18 by cmkwon, C->P, // 2007-06-18 by cmkwon, 네트워크 상태 체크 
#define T1_PC_CONNECT_NETWORK_CHECK_OK					0x23		// 2007-06-18 by cmkwon, P->C, // 2007-06-18 by cmkwon, 네트워크 상태 체크 
#define T1_PC_CONNECT_GET_NEW_GAME_SERVER_GROUP_LIST		0x24	// 2007-09-05 by cmkwon, EXE_1에 로그인 서버 선택 인터페이스 수정 - C->P
#define T1_PC_CONNECT_GET_NEW_GAME_SERVER_GROUP_LIST_OK		0x25
#define T1_PC_CONNECT_LAUNCHER_SESSION				0x26	// P->Launcher, authenticated account-management session	// 2007-09-05 by cmkwon, EXE_1에 로그인 서버 선택 인터페이스 수정 - P->C
#ifdef _INET_MAC_ADDRESS_CHECKER
#define T1_PC_CONNECT_SEND_GET_BLOCKED_MAC_ADDR			0xF1		// 2016-03-06 by inet - for send to p-server mac address
#define T1_PC_CONNECT_SEND_GET_BLOCKED_MAC_ADDR_OK		0xF2		// 2016-03-06 by inet - for send to p-server mac address
#define T1_PC_CONNECT_SEND_GET_BLOCKED_MAC_ADDR_BLOCK	0xF3		// 2016-03-07 by inet - for send to p-server mac address
#endif
#define T1_PC_CONNECT_LOGIN_BLOCKED		0xF0


// FC_CONNECT
#define T1_FC_CONNECT					0x00		// 사용하지 않음
#define T1_FC_CONNECT_OK				0x01		// 사용하지 않음
#define T1_FC_CONNECT_CLOSE				0x02
#define T1_FC_CONNECT_ALIVE				0x03
#define T1_FC_CONNECT_LOGIN				0x04
#define T1_FC_CONNECT_LOGIN_OK			0x05
#define T1_FC_CONNECT_SYNC_TIME			0x06
#define T1_FC_CONNECT_NOTIFY_SERVER_SHUTDOWN	0x07	// No body, 2006-08-04 by cmkwon
#define T1_FC_CONNECT_NETWORK_CHECK				0x09	// C->F, // 2008-02-15 by cmkwon, Client<->FieldServer 간 네트워크 상태 체크 - 
#define T1_FC_CONNECT_NETWORK_CHECK_OK			0x0A	// F->C, // 2008-02-15 by cmkwon, Client<->FieldServer 간 네트워크 상태 체크 - 
#define T1_FC_CONNECT_ARENASERVER_INFO			0x0B	// 2007-12-28 by dhjin, 아레나 통합 -
#define T1_FC_CONNECT_ARENASERVER_LOGIN			0x0C	// 2007-12-28 by dhjin, 아레나 통합 -
#define T1_FC_CONNECT_ARENASERVER_LOGIN_OK		0x0D	// 2007-12-28 by dhjin, 아레나 통합 -
#define T1_FC_CONNECT_ARENASERVER_SSERVER_GROUP_FOR_CLIENT		0x0E	// 2008-02-25 by dhjin, 아레나 통합 -
#define T1_FC_CONNECT_ARENASERVER_TO_IMSERVER		0x0F	// 2008-03-03 by dhjin, 아레나 통합 -
// start 2011-11-03 by shcho, yedang 셧다운제 구현 - 전송 코드 추가
#define T1_FC_CONNECT_SHUTDOWNMINS_USER_ALTER	0x10	// 10분마다 종료 시간 알림 전송
#define T1_FC_CONNECT_SHUTDOWNMINS_USER_ENDGAME	0x11	// 12시이므로 게임 종료 알림
// end 2011-11-03 by shcho, yedang 셧다운제 구현 - 전송 코드 추가

// FP_CONNECT
#define T1_FP_CONNECT					0x00		// 사용하지 않음
#define T1_FP_CONNECT_OK				0x01		// 사용하지 않음
#define T1_FP_CONNECT_CLOSE				0x02
#define T1_FP_CONNECT_ALIVE				0x03
#define T1_FP_CONNECT_AUTH_USER			0x04
#define T1_FP_CONNECT_AUTH_USER_OK		0x05
#define T1_FP_CONNECT_FIELD_CONNECT		0x06
#define T1_FP_CONNECT_FIELD_CONNECT_OK	0x07
#define T1_FP_CONNECT_NOTIFY_CLOSE		0x08
#define T1_FP_CONNECT_NOTIFY_FIELDSERVER_CHANGE		0x09
#define T1_FP_CONNECT_NOTIFY_FIELDSERVER_CHANGE_OK	0x0A
#define T1_FP_CONNECT_PREPARE_SHUTDOWN				0x0B		// No body, 2006-08-04 by cmkwon
#define T1_FP_CONNECT_UPDATE_DBSERVER_GROUP			0x10		// 2008-04-29 by cmkwon, 서버군 정보 DB에 추가(신규 계정 캐릭터 생성 제한 시스템추가) - P->F
#define T1_FP_CONNECT_CHECK_CONNECTABLE_ACCOUNT		0x11		// 2008-04-29 by cmkwon, 서버군 정보 DB에 추가(신규 계정 캐릭터 생성 제한 시스템추가) - P->F
#define T1_FP_CONNECT_CHECK_CONNECTABLE_ACCOUNT_OK	0x12		// 2008-04-29 by cmkwon, 서버군 정보 DB에 추가(신규 계정 캐릭터 생성 제한 시스템추가) - F->P
#define T1_FP_ADMIN_RELOAD_WORLDRANKING				0x13		// 2009-02-12 by cmkwon, EP3-3 월드랭킹시스템 구현 - P->F(n)
#define T1_FP_ADMIN_RELOAD_INFLUENCERATE			0x14		// P->F // 2009-09-16 by cmkwon, 세력 초기화시 어뷰징 방지 구현 - 
#define T1_FP_AUTHENTICATION_SHUTDOWN				0x15		// P->F // 2011-06-22 by hskim, 사설 서버 방지 

// IP_CONNECT
#define T1_IP_CONNECT					0x00		// 사용하지 않음
#define T1_IP_CONNECT_OK				0x01		// 사용하지 않음
#define T1_IP_CONNECT_CLOSE				0x02
#define T1_IP_CONNECT_ALIVE				0x03
#define T1_IP_CONNECT_IM_CONNECT		0x04
#define T1_IP_CONNECT_IM_CONNECT_OK		0x05
#define T1_IP_GET_SERVER_GROUP_INFO		0x06
#define T1_IP_GET_SERVER_GROUP_INFO_ACK	0x07
#define T1_IP_ADMIN_PETITION_SET_PERIOD	0x08	// 2007-11-19 by cmkwon, 진정시스템 업데이트 - P->I(n)
#define T1_IP_ADMIN_RELOAD_ADMIN_NOTICE_SYSTEM		0x09	// 2009-01-14 by cmkwon, 운영자 자동 공지 시스템 구현 - P->I(n)
#define T1_IP_AUTHENTICATION_SHUTDOWN				0x0A	// P->F // 2011-06-22 by hskim, 사설 서버 방지 

// IC_CONNECT
#define T1_IC_CONNECT					0x00		// 사용하지 않음
#define T1_IC_CONNECT_OK				0x01		// 사용하지 않음
#define T1_IC_CONNECT_CLOSE				0x02
#define T1_IC_CONNECT_ALIVE				0x03
#define T1_IC_CONNECT_LOGIN				0x04
#define T1_IC_CONNECT_LOGIN_OK			0x05
#define T1_IC_CONNECT_FM_TO_IM_OK		0x06		// 2008-03-03 by dhjin, 아레나 통합

// FI_CONNECT
#define T1_FI_CONNECT						0x00
#define T1_FI_CONNECT_OK					0x01
#define T1_FI_CONNECT_CLOSE					0x02
#define T1_FI_CONNECT_ALIVE					0x03
#define T1_FI_NOTIFY_FIELDSERVER_IP			0x04
#define T1_FI_NOTIFY_GAMEEND				0x05
//#define T1_FI_CONNECT_NOTIFY_MAP_CHANGE	0x06		// F -> I, check: deprecated, T1_FI_EVENT_NOTIFY_WARP로 대체됨
#define T1_FI_CONNECT_NOTIFY_DEAD			0x07		// F -> I
#define T1_FI_GET_FIELD_USER_COUNTS			0x08
#define T1_FI_GET_FIELD_USER_COUNTS_ACK		0x09
#define T1_FI_CONNECT_NOTIFY_GAMESTART		0x0A	// F->I, 게임 시작했을 때 IM Server에 알림, 파티 정보 확인 요청 등
#define T1_FI_CONNECT_NOTIFY_DEAD_GAMESTART	0x0B
#define T1_FI_CONNECT_PREPARE_SHUTDOWN		0x10	// I->F, // 2007-08-27 by cmkwon, 서버다운준비 명령어 추가(SCAdminTool에서 SCMonitor의 PrepareShutdown을 진행 할 수 있게)

// PM_CONNECT
#define T1_PM_CONNECT					0x00
#define T1_PM_CONNECT_OK				0x01
#define T1_PM_CONNECT_CLOSE				0x02
#define T1_PM_CONNECT_ALIVE				0x03

// FM_CONNECT
#define T1_FM_CONNECT					0x00
#define T1_FM_CONNECT_OK				0x01
#define T1_FM_CONNECT_CLOSE				0x02
#define T1_FM_CONNECT_ALIVE				0x03

#define T1_FM_CONNECT_BACK				0x70
#define T1_FM_CONNECT_OK_BACK			0x71
#define T1_FM_CONNECT_CLOSE_BACK		0x72
#define T1_FM_CONNECT_ALIVE_BACK		0x73

// FN_CONNECT
#define T1_FN_CONNECT					0x00		// 사용하지 않음
#define T1_FN_CONNECT_OK				0x01		// 사용하지 않음
#define T1_FN_CONNECT_CLOSE				0x02
#define T1_FN_CONNECT_ALIVE				0x03
#define T1_FN_CONNECT_INCREASE_CHANNEL	0x04	// F->N
#define T1_FN_CONNECT_SET_CHANNEL_STATE	0x05	// F->N

// LM_CONNECT
#define T1_LM_CONNECT					0x00
#define T1_LM_CONNECT_OK				0x01
#define T1_LM_CONNECT_CLOSE				0x02
#define T1_LM_CONNECT_ALIVE				0x03

// IM_CONNECT
#define T1_IM_CONNECT					0x00
#define T1_IM_CONNECT_OK				0x01
#define T1_IM_CONNECT_CLOSE				0x02
#define T1_IM_CONNECT_ALIVE				0x03

// NM_CONNECT
#define T1_NM_CONNECT					0x00
#define T1_NM_CONNECT_OK				0x01
#define T1_NM_CONNECT_CLOSE				0x02
#define T1_NM_CONNECT_ALIVE				0x03

// PL_CONNECT
#define T1_PL_CONNECT					0x00
#define T1_PL_CONNECT_OK				0x01
#define T1_PL_CONNECT_CLOSE				0x02
#define T1_PL_CONNECT_ALIVE				0x03

// IL_CONNECT
#define T1_IL_CONNECT					0x00
#define T1_IL_CONNECT_OK				0x01
#define T1_IL_CONNECT_CLOSE				0x02
#define T1_IL_CONNECT_ALIVE				0x03

// FL_CONNECT
#define T1_FL_CONNECT					0x00
#define T1_FL_CONNECT_OK				0x01
#define T1_FL_CONNECT_CLOSE				0x02
#define T1_FL_CONNECT_ALIVE				0x03

// NL_CONNECT
#define T1_NL_CONNECT					0x00
#define T1_NL_CONNECT_OK				0x01
#define T1_NL_CONNECT_CLOSE				0x02
#define T1_NL_CONNECT_ALIVE				0x03

// 2008-02-22 by cmkwon, ServerPreServer->MasangPreServer 로 서비스 정보 전송 시스템 추가 - 
// 2008-02-22 by cmkwon, #define T0_PP_CONNECT				0x20	
#define T1_PP_CONNECT					0x00	// 2008-02-22 by cmkwon, ServerPreServer->MasangPreServer 로 서비스 정보 전송 시스템 추가 - 
#define T1_PP_CONNECT_OK				0x01	// 2008-02-22 by cmkwon, ServerPreServer->MasangPreServer 로 서비스 정보 전송 시스템 추가 - 
#define T1_PP_CONNECT_DO_CLOSE			0x02	// 2008-02-22 by cmkwon, ServerPreServer->MasangPreServer 로 서비스 정보 전송 시스템 추가 - 

#define T_PP_CONNECT					(MessageType_t)((T0_PP_CONNECT<<8)|T1_PP_CONNECT)			// 2008-02-22 by cmkwon, ServerPreServer->MasangPreServer 로 서비스 정보 전송 시스템 추가 - 
#define T_PP_CONNECT_OK					(MessageType_t)((T0_PP_CONNECT<<8)|T1_PP_CONNECT_OK)		// 2008-02-22 by cmkwon, ServerPreServer->MasangPreServer 로 서비스 정보 전송 시스템 추가 - 
#define T_PP_CONNECT_DO_CLOSE			(MessageType_t)((T0_PP_CONNECT<<8)|T1_PP_CONNECT_DO_CLOSE)	// 2008-02-22 by cmkwon, ServerPreServer->MasangPreServer 로 서비스 정보 전송 시스템 추가 - 



// FI_EVENT
#define T1_FI_EVENT_NOTIFY_WARP			0x00		// F -> I
#define T1_FI_EVENT_NOTIFY_WARP_OK		0x01		// I -> F
#define T1_FI_EVENT_GET_WARP_INFO		0x02		// F -> I
#define T1_FI_EVENT_GET_WARP_INFO_OK	0x03		// I -> F
#define T1_FI_EVENT_CHAT_BLOCK			0x04		// 2008-12-30 by cmkwon, 지도자 채팅 제한 카드 구현 - F->I
#define T1_FI_MULTICHAT_STEERING		0x05		//19-03-2017 by inetpub - allow fieldserver to disable/enable multichat
// IC_CHAT
#define T1_IC_CHAT_ALL					0x00	// 서버군, 운영자용
#define T1_IC_CHAT_MAP					0x01	// 맵 전체
#define T1_IC_CHAT_REGION				0x02	// 주위의 아그들에게만
#define T1_IC_CHAT_PTOP					0x03	// 1:1
#define T1_IC_CHAT_PARTY				0x04	// 파티
#define T1_IC_CHAT_GUILD				0x05	// 길드
#define T1_IC_CHAT_GET_GUILD			0x06
#define T1_IC_CHAT_GET_GUILD_OK			0x07
#define T1_IC_CHAT_CHANGE_GUILD			0x08
#define T1_IC_CHAT_CHANGE_GUILD_OK		0x09
#define T1_IC_CHAT_CHANGE_PARTY			0x0A
#define T1_IC_CHAT_CHANGE_PARTY_OK		0x0B
#define T1_IC_CHAT_CHANGE_CHAT_FLAG		0x0C
#define T1_IC_CHAT_POSITION				0x0D
#define T1_IC_CHAT_BLOCK				0x0E
#define T1_IC_CHAT_GET_BLOCK			0x0F
#define T1_IC_CHAT_GET_BLOCK_OK			0x10
#define T1_IC_CHAT_BLOCK_YOU			0x11
#define T1_IC_CHAT_FRIENDLIST_AND_REJECTLIST_LOADING	0x12	// No Body
#define T1_IC_CHAT_FRIENDLIST_LOADING_OK	0x13
#define T1_IC_CHAT_FRIENDLIST_INSERT		0x14
#define T1_IC_CHAT_FRIENDLIST_INSERT_OK		0x15
#define T1_IC_CHAT_FRIENDLIST_DELETE		0x16
#define T1_IC_CHAT_FRIENDLIST_DELETE_OK		0x17
#define T1_IC_CHAT_FRIENDLIST_REFRESH		0x18	// No Body
#define T1_IC_CHAT_FRIENDLIST_REFRESH_OK	0x19
#define T1_IC_CHAT_REJECTLIST_LOADING_OK	0x1A
#define T1_IC_CHAT_REJECTLIST_INSERT		0x1B
#define T1_IC_CHAT_REJECTLIST_INSERT_OK		0x1C
#define T1_IC_CHAT_REJECTLIST_DELETE		0x1D
#define T1_IC_CHAT_REJECTLIST_DELETE_OK		0x1E	
#define T1_IC_CHAT_FRIENDLIST_INSERT_NOTIFY	0x1F	// 2009-01-13 by cmkwon, 친구 등록시 상대방에게 알림 시스템 적용 - 

#define T1_IC_CHAT_SELL_ALL					0x30	// 매매 전체 채팅
#define T1_IC_CHAT_CASH_ALL					0x31	// 유료 전체 채팅
#define T1_IC_CHAT_INFLUENCE_ALL			0x32	// 세력 전체 채팅 - 세력지도자만 가능
#define T1_IC_CHAT_ARENA					0x33	// 2007-05-02 by dhjin, 아레나 채팅
#define T1_IC_CHAT_WAR						0x34	// 2008-05-19 by dhjin, EP3 - 채팅 시스템 변경, 전쟁 채팅
#define T1_IC_CHAT_CHATROOM					0x35	// 2008-06-18 by dhjin, EP3 채팅방 - 
#define T1_IC_CHAT_INFINITY					0x36	// 2009-09-09 ~ 2010 by dhjin, 인피니티 - 인피 채팅
#define T1_IC_CHAT_MULTI					0x37	// 2015-02-11 by silver define T1_IC_CHAT_MULTI
// FI_CHAT (IM Server --> Field Server)
#define T1_FI_CHAT_MAP						0x00
#define T1_FI_CHAT_REGION					0x01
#define T1_FI_CHAT_CHANGE_CHAT_FLAG			0x02
#define T1_FI_CHAT_CASH_ALL					0x03	// 유료 전체 채팅
#define	T1_FI_CHAT_ARENA					0x04	// 2007-05-02 by dhjin, 아레나 채팅
#define	T1_FI_CHAT_OUTPOST_GUILD			0x05	// 2007-10-06 by cmkwon, 전진 기지 소유한 여단장 세력 채팅 가능
#define T1_FI_CHAT_INFINITY					0x06	// 2009-09-09 ~ 2010 by dhjin, 인피니티 - 인피 채팅

#define T1_FI_CHAT_ONLINE_EACHOTHER_FRIEND_COUNT_OK	0x10	// 2006-07-18 by cmkwon, 서로 등록한 온라인 친구 카운트
#define T1_IC_CHAT_ONLINE_EACHOTHER_FRIEND_COUNT	0x11	// 2008-07-11 by dhjin, EP3 친구목록 -
// FC_CHAT (Field Server --> Clients)
#define T1_FC_CHAT_MAP								0x00
#define T1_FC_CHAT_REGION							0x01
#define T1_FC_CHAT_CASH_ALL							0x02
#define T1_FC_CHAT_ARENA							0x03			// 2007-05-02 by dhjin	F->C(n)
#define T1_FC_CHAT_INFINITY							0x12			// 2009-09-09 ~ 2010 by dhjin, 인피니티 - 인피 채팅
#define T1_FC_CHAT_ALL_INFLUENCE					0x10			// 2007-08-09 by cmkwon, 모든 세력에 채팅 전송하기 - 프로토콜타입 추가
#define T1_FC_CHAT_OUTPOST_GUILD					0x11			// 2007-10-06 by cmkwon, 전진 기지 소유한 여단장 세력 채팅 가능
#define T1_FC_CHAT_LETTER_REQUEST_TITLE				0x21			// 2008-04-29 by dhjin, EP3 편지 시스템 - 편지 리스트 요청 C->F
#define T1_FC_CHAT_LETTER_REQUEST_TITLE_OK_HEADER	0x22			// 2008-04-29 by dhjin, EP3 편지 시스템 - 편지 리스트 전송 F->C
#define T1_FC_CHAT_LETTER_REQUEST_TITLE_OK			0x23			// 2008-04-29 by dhjin, EP3 편지 시스템 - 편지 리스트 전송 F->C
#define T1_FC_CHAT_LETTER_REQUEST_TITLE_OK_DONE		0x24			// 2008-04-29 by dhjin, EP3 편지 시스템 - 편지 리스트 전송 F->C
#define T1_FC_CHAT_LETTER_READ						0x25			// 2008-04-29 by dhjin, EP3 편지 시스템 - 편지 읽기 요청 C->F
#define T1_FC_CHAT_LETTER_READ_OK					0x26			// 2008-04-29 by dhjin, EP3 편지 시스템 - 편지 읽기 완료 F->C
#define T1_FC_CHAT_LETTER_DELETE					0x27			// 2008-04-29 by dhjin, EP3 편지 시스템 - 편지 지우기 요청 C->F
#define T1_FC_CHAT_LETTER_DELETE_OK					0x28			// 2008-04-29 by dhjin, EP3 편지 시스템 - 편지 지우기 완료 F->C
#define T1_FC_CHAT_LETTER_SEND						0x29			// 2008-05-08 by dhjin, EP3 편지 시스템 - 편지 전송 C->F
#define T1_FC_CHAT_LETTER_SEND_OK					0x2A			// 2008-05-08 by dhjin, EP3 편지 시스템 - 편지 전송 완료 F->C
#define T1_FC_CHAT_LETTER_RECEIVE					0x2B			// 2008-05-08 by dhjin, EP3 편지 시스템 - 편지 전송 받음 F->C
#define T1_FC_CHAT_ALLLETTER_REQUEST_TITLE				0x31			// 2008-05-09 by dhjin, EP3 편지 시스템 - 전체 편지 리스트 요청 C->F
#define T1_FC_CHAT_ALLLETTER_REQUEST_TITLE_OK_HEADER	0x32		// 2008-05-09 by dhjin, EP3 편지 시스템 - 전체 편지 리스트 전송 F->C
#define T1_FC_CHAT_ALLLETTER_REQUEST_TITLE_OK			0x33			// 2008-05-09 by dhjin, EP3 편지 시스템 - 전체 편지 리스트 전송 F->C
#define T1_FC_CHAT_ALLLETTER_REQUEST_TITLE_OK_DONE		0x34			// 2008-05-09 by dhjin, EP3 편지 시스템 - 전체 편지 리스트 전송 F->C
#define T1_FC_CHAT_ALLLETTER_READ						0x35			// 2008-05-09 by dhjin, EP3 편지 시스템 - 전체 편지 읽기 요청 C->F
#define T1_FC_CHAT_ALLLETTER_READ_OK					0x36			// 2008-05-09 by dhjin, EP3 편지 시스템 - 전체 편지 읽기 완료 F->C
#define T1_FC_CHAT_ALLLETTER_DELETE						0x37			// 2008-05-09 by dhjin, EP3 편지 시스템 - 전체 편지 지우기 요청 C->F
#define T1_FC_CHAT_ALLLETTER_DELETE_OK					0x38			// 2008-05-09 by dhjin, EP3 편지 시스템 - 전체 편지 지우기 완료 F->C
#define T1_FC_CHAT_ALLLETTER_SEND						0x39			// 2008-05-09 by dhjin, EP3 편지 시스템 - 전체 편지 전송 C->F
#define T1_FC_CHAT_ALLLETTER_SEND_OK					0x3A			// 2008-05-09 by dhjin, EP3 편지 시스템 - 전체 편지 전송 완료 F->C 
#define T1_FC_CHAT_ALLLETTER_RECEIVE					0x3B			// 2008-05-09 by dhjin, EP3 편지 시스템 - 전체 편지 전송 받음 F->C

// FC_CHARACTER
#define T1_FC_CHARACTER_CREATE						0x00
#define T1_FC_CHARACTER_CREATE_OK					0x01
#define T1_FC_CHARACTER_DELETE						0x02
#define T1_FC_CHARACTER_DELETE_OK					0x03
#define T1_FC_CHARACTER_GET_CHARACTER				0x04
#define T1_FC_CHARACTER_GET_CHARACTER_OK			0x05
#define T1_FC_CHARACTER_GAMESTART					0x06
#define T1_FC_CHARACTER_GAMESTART_OK				0x07
#define T1_FC_CHARACTER_CONNECT_GAMESTART			0x08
#define T1_FC_CHARACTER_CONNECT_GAMESTART_OK		0x09
#define T1_FC_CHARACTER_GAMEEND						0x0A
#define T1_FC_CHARACTER_GAMEEND_OK					0x0B
#define T1_FC_CHARACTER_REPAIR						0x0C
#define T1_FC_CHARACTER_REPAIR_OK					0x0D
#define T1_FC_CHARACTER_REPAIR_ERR					0x0E
#define T1_FC_CHARACTER_DOCKING						0x0F
#define T1_FC_CHARACTER_UNDOCKING					0x10
#define T1_FC_CHARACTER_DOCKING_ERR					0x11
#define T1_FC_CHARACTER_GET_OTHER_INFO				0x12
#define T1_FC_CHARACTER_GET_OTHER_INFO_OK			0x13
#define T1_FC_CHARACTER_GET_MONSTER_INFO_OK			0x14
#define T1_FC_CHARACTER_CHANGE_UNITKIND				0x15
#define T1_FC_CHARACTER_CHANGE_STAT					0x16
#define T1_FC_CHARACTER_CHANGE_TOTALGEAR_STAT		0x17
#define T1_FC_CHARACTER_CHANGE_FRIEND				0x18
#define T1_FC_CHARACTER_CHANGE_EXP					0x19
#define T1_FC_CHARACTER_CHANGE_BODYCONDITION		0x1A
#define T1_FC_CHARACTER_CHANGE_BODYCONDITION_SKILL	0x1B
#define T1_FC_CHARACTER_CHANGE_INFLUENCE_TYPE		0x1C
#define T1_FC_CHARACTER_CHANGE_STATUS				0x1D
#define T1_FC_CHARACTER_CHANGE_PKPOINT				0x1E
#define T1_FC_CHARACTER_CHANGE_CURRENTHPDPSPEP		0x1F
#define T1_FC_CHARACTER_CHANGE_CURRENTHP			0x20
#define T1_FC_CHARACTER_CHANGE_CURRENTDP			0x21
#define T1_FC_CHARACTER_CHANGE_CURRENTSP			0x22
#define T1_FC_CHARACTER_CHANGE_CURRENTEP			0x23
#define T1_FC_CHARACTER_CHANGE_MAPNAME				0x24
#define T1_FC_CHARACTER_CHANGE_PETINFO				0x25
#define T1_FC_CHARACTER_CHANGE_POSITION				0x26
#define T1_FC_CHARACTER_CHANGE_LEVEL				0x27
#define T1_FC_CHARACTER_USE_BONUSSTAT				0x28
// 아래에 정의되어 있음 #define T1_FC_CHARACTER_USE_BONUSSTAT_OK					0x67	// F->C, 2006-09-18 by cmkwon
#define T1_FC_CHARACTER_DEAD_NOTIFY					0x29	// F->C, 캐릭터가 죽은것을 클라이언트에 알림
#define T1_FC_CHARACTER_GET_ACCOUNTUNIQUENUMBER		0x2A
#define T1_FC_CHARACTER_GET_ACCOUNTUNIQUENUMBER_OK	0x2B
#define T1_FC_CHARACTER_APPLY_COLLISION_DAMAGE		0x2C
#define T1_FC_CHARACTER_GET_OTHER_MOVE				0x2D
#define T1_FC_CHARACTER_DELETE_OTHER_INFO			0x2E
#define T1_FC_CHARACTER_DEAD_GAMESTART				0x2F	// C -> F
#define T1_FC_CHARACTER_OTHER_REVIVED				0x30	// F -> C
#define T1_FC_CHARACTER_GET_OTHER_RENDER_INFO		0x31
#define T1_FC_CHARACTER_GET_OTHER_RENDER_INFO_OK	0x32
#define T1_FC_CHARACTER_CHANGE_BODYCONDITION_ALL	0x33
#define T1_FC_CHARACTER_CHANGE_PROPENSITY			0x34	// F->C // 2005-08-22 by cmkwon, 사용하지 않던 것을 변경함
#define T1_FC_CHARACTER_CHANGE_HPDPSPEP				0x35
#define T1_FC_CHARACTER_SHOW_EFFECT					0x36	// C->F
#define T1_FC_CHARACTER_SHOW_EFFECT_OK				0x37	// F->C
#define T1_FC_CHARACTER_GET_OTHER_PARAMFACTOR		0x38	// C->F
#define T1_FC_CHARACTER_GET_OTHER_PARAMFACTOR_OK	0x39	// F->C
#define T1_FC_CHARACTER_SEND_PARAMFACTOR_IN_RANGE	0x3A	// C->F
#define T1_FC_CHARACTER_GET_OTHER_SKILL_INFO		0x3B	// C->F
#define T1_FC_CHARACTER_GET_OTHER_SKILL_INFO_OK		0x3C	// F->C
#define T1_FC_CHARACTER_SPEED_HACK_USER				0x3D	// C->F
#define T1_FC_CHARACTER_CHANGE_CHARACTER_MENT		0x3E	// F->C(n)
#define T1_FC_CHARACTER_GET_CASH_MONEY_COUNT		0x3F	// C->F
#define T1_FC_CHARACTER_GET_CASH_MONEY_COUNT_OK		0x40	// F->C
#define T1_FC_CHARACTER_CASH_PREMIUM_CARD_INFO		0x41	// F->C
#define T1_FC_CHARACTER_TUTORIAL_SKIP				0x42	// C->F
#define T1_FC_CHARACTER_TUTORIAL_SKIP_OK			0x43	// F->C
#define T1_FC_CHARACTER_CHANGE_CHARACTER_MODE		0x50	// C->F
#define T1_FC_CHARACTER_CHANGE_CHARACTER_MODE_OK	0x51	// F->C(n)
#define T1_FC_CHARACTER_FALLING_BY_FUEL_ALLIN		0x52	// C->F
#define T1_FC_CHARACTER_WARP_BY_AGEAR_LANDING_FUEL_ALLIN	0x53	// C->F
#define T1_FC_CHARACTER_GET_REAL_WEAPON_INFO				0x60	// F->C// 2005-12-21 by cmkwon
#define T1_FC_CHARACTER_GET_REAL_WEAPON_INFO_OK				0x61	// C->F// 2005-12-21 by cmkwon
#define T1_FC_CHARACTER_GET_REAL_ENGINE_INFO				0x62	// F->C// 2005-12-21 by cmkwon
#define T1_FC_CHARACTER_GET_REAL_ENGINE_INFO_OK				0x63	// C->F// 2005-12-21 by cmkwon
#define T1_FC_CHARACTER_GET_REAL_TOTAL_WEIGHT				0x64	// F->C// 2005-12-21 by cmkwon
#define T1_FC_CHARACTER_GET_REAL_TOTAL_WEIGHT_OK			0x65	// C->F// 2005-12-21 by cmkwon
#define T1_FC_CHARACTER_MEMORY_HACK_USER					0x66	// C->F// 2005-12-21 by cmkwon
#define T1_FC_CHARACTER_USE_BONUSSTAT_OK					0x67	// F->C, 2006-09-18 by cmkwon
#define T1_FC_CHARACTER_UPDATE_SUBLEADER					0x68	// C->F, 2007-02-14 by dhjin
#define T1_FC_CHARACTER_UPDATE_SUBLEADER_OK					0x6A	// F->C, 2007-10-06 by dhjin

#define T1_FC_CHARACTER_OBSERVER_START						0x69	// C->F, 2007-03-27 by dhjin

#define T1_FC_CHARACTER_OBSERVER_START_OK					0x70	// F->C, 2007-03-27 by dhjin
#define T1_FC_CHARACTER_OBSERVER_END						0x71	// C->F, 2007-03-27 by dhjin
#define T1_FC_CHARACTER_OBSERVER_END_OK						0x72	// F->C, 2007-03-27 by dhjin
#define T1_FC_CHARACTER_OBSERVER_INFO						0x73	// F->C, 2007-03-27 by dhjin
#define T1_FC_CHARACTER_OBSERVER_REG						0x74	// F->C, 2007-03-27 by dhjin
#define T1_FC_CHARACTER_SHOW_MAP_EFFECT						0x75	// C->F, // 2007-04-20 by cmkwon
#define T1_FC_CHARACTER_SHOW_MAP_EFFECT_OK					0x76	// F->C(n), // 2007-04-20 by cmkwon
#define T1_FC_CHARACTER_PAY_WARPOINT						0x77	// F->C(n), // 2007-05-16 by dhjin
#define T1_FC_CHARACTER_WATCH_INFO							0x78	// F->C, // 2007-06-19 by dhjin
#define T1_FC_CHARACTER_READY_GAMESTART_FROM_ARENA_TO_MAINSERVER	0x79	// C->F, // 2008-01-31 by dhjin, 아레나 통합 - 
#define T1_FC_CHARACTER_READY_GAMESTART_FROM_ARENA_TO_MAINSERVER_OK	0x7A	// F->C, // 2008-01-31 by dhjin, 아레나 통합 - 
#define T1_FC_CHARACTER_GAMESTART_FROM_ARENA_TO_MAINSERVER	0x7B	// C->F, // 2008-01-10 by dhjin, 아레나 통합 - 
#define T1_FC_CHARACTER_GET_USER_INFO						0x7C	// C->F, // 2008-06-23 by dhjin, EP3 유저정보옵션 -
#define T1_FC_CHARACTER_GET_USER_INFO_OK					0x7D	// F->C, // 2008-06-23 by dhjin, EP3 유저정보옵션 -
#define T1_FC_CHARACTER_CHANGE_INFO_OPTION_SECRET			0x7E	// C->F, // 2008-06-23 by dhjin, EP3 유저정보옵션 -
#define T1_FC_CHARACTER_CHANGE_INFO_OPTION_SECRET_OK		0x7F	// F->C, // 2008-06-23 by dhjin, EP3 유저정보옵션 -
#define T1_FC_CHARACTER_CHANGE_NICKNAME						0x80	// C->F, // 2009-02-12 by cmkwon, EP3-3 월드랭킹시스템 구현 - 
#define T1_FC_CHARACTER_CHANGE_NICKNAME_OK					0x81	// F->C, // 2009-02-12 by cmkwon, EP3-3 월드랭킹시스템 구현 - 
#define T1_FC_CHARACTER_CHANGE_START_CITY_MAPINDEX			0x82	// C->F, // 2009-10-12 by cmkwon, 프리스카 제거 방안 적용 - 
#define T1_FC_CHARACTER_CHANGE_START_CITY_MAPINDEX_OK		0x83	// F->C, // 2009-10-12 by cmkwon, 프리스카 제거 방안 적용 - 
#define T1_FC_CHARACTER_CHANGE_ADDED_INVENTORY_COUNT		0x84	// F->C, // 2009-11-02 by cmkwon, 캐쉬(인벤/창고 확장) 아이템 추가 구현 - 
#define T1_FC_CHARACTER_DEBUFF_DOT_INFO						0x90	// 2009-09-09 ~ 2010 by dhjin, 인피니티 - C -> F
#define T1_FC_CHARACTER_DEBUFF_DOT_INFO_OK					0x91	// 2009-09-09 ~ 2010 by dhjin, 인피니티 - F -> C
#define T1_FC_CHARACTER_DEBUFF_DOT_APPLYING					0x92	// 2009-09-09 ~ 2010 by dhjin, 인피니티 - F -> C
#define T1_FC_CHARACTER_DEBUFF_DOT_RELEASE					0x93	// 2009-09-09 ~ 2010 by dhjin, 인피니티 - F -> C
#ifdef _INET_LINK_CHAT
#define T1_FC_CHARACTER_GET_USER_ITEM_INFO_OK_DONE					0x94	// 2009-09-09 ~ 2010 by dhjin, 인피니티 - F -> C
#endif

// FN_CHARACTER
#define T1_FN_CHARACTER_CHANGE_UNITKIND				0x01
#define T1_FN_CHARACTER_CHANGE_BODYCONDITION		0x02
#define T1_FN_CHARACTER_CHANGE_HPDPSPEP				0x03
#define T1_FN_CHARACTER_CHANGE_CURRENTHPDPSPEP		0x04
#define T1_FN_CHARACTER_CHANGE_MAPNAME				0x05
#define T1_FN_CHARACTER_CHANGE_POSITION				0x06
#define T1_FN_CHARACTER_CHANGE_STEALTHSTATE			0x07
#define T1_FN_CHARACTER_CHANGE_CHARACTER_MODE_OK	0x08
#define T1_FN_CHARACTER_CHANGE_INFLUENCE_TYPE		0x09	// F->I, 2005-12-03 by cmkwon
#define T1_FN_CHARACTER_CHANGE_INVISIBLE			0x0A	// F->I, 2006-11-27 by dhjin

// FC_MOVE
#define T1_FC_MOVE						0x00
#define T1_FC_MOVE_OK					0x01
#define T1_FC_MOVE_LOCKON				0x02
#define T1_FC_MOVE_LOCKON_OK			0x03
#define T1_FC_MOVE_UNLOCKON				0x04
#define T1_FC_MOVE_UNLOCKON_OK			0x05
#define T1_FC_MOVE_LANDING				0x06
#define T1_FC_MOVE_LANDING_OK			0x07
#define T1_FC_MOVE_LANDING_DONE			0x08	// C->F, 착륙 완료를 알림
#define T1_FC_MOVE_TAKEOFF				0x09
#define T1_FC_MOVE_TAKEOFF_OK			0x0A
#define T1_FC_MISSILE_MOVE_OK			0x0B
#define T1_FC_MOVE_TARGET				0x0C
#define T1_FC_MOVE_WEAPON_VEL			0x0D
#define T1_FC_MOVE_WEAPON_VEL_OK		0x0E
#define T1_FC_MOVE_ROLLING				0x10
#define T1_FC_MOVE_ROLLING_OK			0x11
#define T1_FC_MOVE_HACKSHIELD_GuidReqMsg		0xA0	// 2006-06-05 by cmkwon, Anlab - HackShield
#define T1_FC_MOVE_HACKSHIELD_GuidAckMsg		0xA1	// 2006-06-05 by cmkwon, Anlab - HackShield
#define T1_FC_MOVE_HACKSHIELD_CRCReqMsg			0xA2	// 2006-06-05 by cmkwon, Anlab - HackShield
#define T1_FC_MOVE_HACKSHIELD_CRCAckMsg			0xA3	// 2006-06-05 by cmkwon, Anlab - HackShield
#define T1_FC_MOVE_HACKSHIELD_HACKING_CLIENT	0xA4	// 2006-06-05 by cmkwon, Anlab - HackShield
#define T1_FC_MOVE_XIGNCODE_REQ_SCAN_INIT		0xB0	// 2008-11-28 by cmkwon, 대만 Netpower_Tpe XignCode(게임가드) 적용 - S->C(1)
#define T1_FC_MOVE_XIGNCODE_REQ_SCAN_INIT_OK	0xB1	// 2008-11-28 by cmkwon, 대만 Netpower_Tpe XignCode(게임가드) 적용 - C->S
#define T1_FC_MOVE_XIGNCODE_REQ_SCAN_CHECK		0xB2	// 2008-11-28 by cmkwon, 대만 Netpower_Tpe XignCode(게임가드) 적용 - S->C(1)
#define T1_FC_MOVE_XIGNCODE_REQ_SCAN_CHECK_OK	0xB3	// 2008-11-28 by cmkwon, 대만 Netpower_Tpe XignCode(게임가드) 적용 - C->S
#define T1_FC_MOVE_NPROTECT_REQ_AUTH_DATA		0xB4	// 2009-03-09 by cmkwon, 일본 Arario nProtect에 CS인증 적용하기 - S->C(1)
#define T1_FC_MOVE_NPROTECT_REQ_AUTH_DATA_OK	0xB5	// 2009-03-09 by cmkwon, 일본 Arario nProtect에 CS인증 적용하기 - C->S
#define T1_FC_MOVE_XTRAP_REQ_STEP				0xB6	// 2009-10-06 by cmkwon, 베트남 게임 가드 X-TRAP으로 변경 - S->C(1)
#define T1_FC_MOVE_XTRAP_REQ_STEP_OK			0xB7	// 2009-10-06 by cmkwon, 베트남 게임 가드 X-TRAP으로 변경 - C(1)->S
#define T1_FC_MOVE_APEX_REQ_APEXDATA			0xB8	// 2009-11-04 by cmkwon, 태국 게임가드 Apex로 변경 - S->C(1)
#define T1_FC_MOVE_APEX_REQ_APEXDATA_OK			0xB9	// 2009-11-04 by cmkwon, 태국 게임가드 Apex로 변경 - C(1)->S

// FN_MOVE
#define T1_FN_MONSTER_MOVE				0x00
#define T1_FN_MOVE_OK					0x01
#define T1_FN_MOVE_LOCKON				0x02
#define T1_FN_MOVE_LOCKON_OK			0x03
#define T1_FN_MOVE_LOCKON_AVOID			0x04
#define T1_FN_MOVE_LOCKON_AVOID_OK		0x05
#define T1_FN_MISSILE_MOVE				0x06
#define T1_FN_MONSTER_HPRECOVERY		0x07
#define T1_FN_MONSTER_HIDE				0x08
#define T1_FN_MONSTER_SHOW				0x09

// FC_BATTLE
#define T1_FC_BATTLE_ATTACK						0x00	// C->F
#define T1_FC_BATTLE_ATTACK_OK					0x01	// F->C_in_range
#define T1_FC_BATTLE_ATTACK_FIND				0x02	// C->F
#define T1_FC_BATTLE_ATTACK_FIND_OK				0x03	// F->C_in_range
#define T1_FC_BATTLE_DROP_MINE					0x04	// C->F
#define T1_FC_BATTLE_DROP_MINE_OK				0x05	// F->C_in_range, 아이템 보여주기
#define T1_FC_BATTLE_MINE_ATTACK				0x06	// C->F
#define T1_FC_BATTLE_MINE_ATTACK_OK				0x07	// F->C_in_range
#define T1_FC_BATTLE_MINE_ATTACK_FIND			0x08	// C->F
#define T1_FC_BATTLE_MINE_ATTACK_FIND_OK		0x09	// F->C_in_range
#define T1_FC_BATTLE_REQUEST_PK					0x0A
#define T1_FC_BATTLE_REQUEST_PK_OK				0x0B
#define T1_FC_BATTLE_CANCEL_PK					0x0C
#define T1_FC_BATTLE_REQUEST_P2P_PK				0x0D
#define T1_FC_BATTLE_REQUEST_P2P_PK_OK			0x0E
#define T1_FC_BATTLE_ACCEPT_REQUEST_P2P_PK		0x0F
#define T1_FC_BATTLE_ACCEPT_REQUEST_P2P_PK_OK	0x10
#define T1_FC_BATTLE_REJECT_REQUEST_P2P_PK		0x11
#define T1_FC_BATTLE_REJECT_REQUEST_P2P_PK_OK	0x12
#define T1_FC_BATTLE_SURRENDER_P2P_PK			0x13
#define T1_FC_BATTLE_SURRENDER_P2P_PK_OK		0x14
#define T1_FC_BATTLE_ACCEPT_SURRENDER_P2P_PK	0x15
#define T1_FC_BATTLE_REJECT_SURRENDER_P2P_PK	0x16
#define T1_FC_BATTLE_REJECT_SURRENDER_P2P_PK_OK	0x17
#define T1_FC_BATTLE_END_P2P_PK					0x18
#define T1_FC_BATTLE_ATTACK_EXPLODE_ITEM		0x19
#define T1_FC_BATTLE_ATTACK_HIDE_ITEM			0x1A
#define T1_FC_BATTLE_ATTACK_EXPLODE_ITEM_W_KIND	0x1B
#define T1_FC_BATTLE_ATTACK_HIDE_ITEM_W_KIND	0x1C
#define T1_FC_BATTLE_TOGGLE_SHIELD				0x1D	// No Body
#define T1_FC_BATTLE_TOGGLE_SHIELD_RESULT		0x1E
#define T1_FC_BATTLE_DROP_DUMMY					0x1F
#define T1_FC_BATTLE_DROP_DUMMY_OK				0x20
#define T1_FC_BATTLE_DROP_FIXER					0x21
#define T1_FC_BATTLE_DROP_FIXER_OK				0x22
#define T1_FC_BATTLE_PRI_BULLET_RELOADED		0x23
#define T1_FC_BATTLE_SEC_BULLET_RELOADED		0x24
#define T1_FC_BATTLE_SHIELD_DAMAGE				0x25
#define T1_FC_BATTLE_TOGGLE_DECOY				0x26	// C->F, No Body
#define T1_FC_BATTLE_TOGGLE_DECOY_OK			0x27	// F->C
#define T1_FC_BATTLE_SHOW_DAMAGE				0x28	// F->C
#define T1_FC_BATTLE_ATTACK_EVASION				0x29	// C->F, // 2005-12-12 by cmkwon
#define T1_FC_BATTLE_ATTACK_EVASION_OK			0x2A	// F->C(1), // 2005-12-12 by cmkwon
#define T1_FC_BATTLE_DELETE_DUMMY_OK			0x30	// 2006-12-04 by dhjin, F->C(n)
#define T1_FC_BATTLE_EXPLODE_DUMMY_OK			0x31	// 2006-12-04 by dhjin, F->C(n)
#define T1_FC_BATTLE_MONSTER_BARRIER_USING		0x32	// 2009-09-09 ~ 2010 by dhjin, 인피니티 - F -> C
#define T1_FC_BATTLE_MONSTER_BARRIER_USE		0x33	// 2009-09-09 ~ 2010 by dhjin, 인피니티 - F -> C


// FN_BATTLE
#define T1_FN_BATTLE_ATTACK_PRIMARY				0x00
#define T1_FN_BATTLE_ATTACK_RESULT_PRIMARY		0x01
#define T1_FN_BATTLE_ATTACK_SECONDARY			0x02
#define T1_FN_BATTLE_ATTACK_RESULT_SECONDARY	0x03
#define T1_FN_BATTLE_ATTACK_FIND				0x04
#define T1_FN_BATTLE_ATTACK_FIND_RESULT			0x05
#define T1_FN_BATTLE_SET_ATTACK_CHARACTER		0x06
#define T1_FN_BATTLE_DROP_FIXER					0x07
#define T1_FN_BATTLE_DROP_FIXER_OK				0x08
#define T1_FN_BATTLE_ATTACK_HIDE_ITEM_W_KIND	0x09
#define T1_FN_BATTLE_ATTACK_SKILL				0x0A			// 2009-09-09 ~ 2010 by dhjin, 인피니티 - N -> F
#define T1_FN_BATTLE_ATTACK_SKILL_CANCEL		0x0B			// 2010-03-31 by dhjin, 인피니티(기지방어) - N -> F

// FC_PARTY
#define T1_FC_PARTY_CREATE_OK							0x00
#define T1_FC_PARTY_REQUEST_INVITE						0x01
#define T1_FC_PARTY_REQUEST_INVITE_QUESTION				0x02
#define T1_FC_PARTY_ACCEPT_INVITE						0x03
#define T1_FC_PARTY_REJECT_INVITE						0x04
#define T1_FC_PARTY_REJECT_INVITE_OK					0x05
#define T1_FC_PARTY_GET_MEMBER							0x06
#define T1_FC_PARTY_PUT_MEMBER							0x07
#define T1_FC_PARTY_GET_ALL_MEMBER						0x08
#define T1_FC_PARTY_PUT_ALL_MEMBER						0x09
#define T1_FC_PARTY_UPDATE_MEMBER_INFO_ALL				0x0A
#define T1_FC_PARTY_UPDATE_MEMBER_INFO_LEVEL			0x0B
#define T1_FC_PARTY_UPDATE_MEMBER_INFO_HP				0x0C
#define T1_FC_PARTY_UPDATE_MEMBER_INFO_CURRENT_HP		0x0D
#define T1_FC_PARTY_UPDATE_MEMBER_INFO_DP				0x0E
#define T1_FC_PARTY_UPDATE_MEMBER_INFO_CURRENT_DP		0x0F
#define T1_FC_PARTY_UPDATE_MEMBER_INFO_SP				0x10
#define T1_FC_PARTY_UPDATE_MEMBER_INFO_CURRENT_SP		0x11
#define T1_FC_PARTY_UPDATE_MEMBER_INFO_EP				0x12
#define T1_FC_PARTY_UPDATE_MEMBER_INFO_CURRENT_EP		0x13
#define T1_FC_PARTY_UPDATE_MEMBER_INFO_BODYCONDITION	0x14
#define T1_FC_PARTY_REQUEST_PARTY_WARP					0x15
#define T1_FC_PARTY_REQUEST_PARTY_WARP_WITH_MAP_NAME	0x16
#define T1_FC_PARTY_REQUEST_PARTY_OBJECT_EVENT			0x17
#define T1_FC_PARTY_GET_OTHER_MOVE						0x18
#define T1_FC_PARTY_BATTLE_START						0x19
#define T1_FC_PARTY_BATTLE_END							0x20
#define T1_FC_PARTY_PUT_ITEM_OTHER						0x21	// F->C, 다른 파티원의 아이템 취득 정보 전송
#define T1_FC_PARTY_AUTO_CREATED						0x30	// 2009-09-09 ~ 2010-01-26 by dhjin, 인피니티 - 자동 편대 생성 완료 알림, F -> C

// FI_PARTY
#define T1_FI_PARTY_CREATE_OK							0x01
#define T1_FI_PARTY_ACCEPT_INVITE_OK					0x02
#define T1_FI_PARTY_BAN_MEMBER_OK						0x03
#define T1_FI_PARTY_LEAVE_OK							0x04
#define T1_FI_PARTY_TRANSFER_MASTER_OK					0x05
#define T1_FI_PARTY_DISMEMBER_OK						0x06
#define T1_FI_PARTY_CHANGE_FLIGHT_FORMATION_OK			0x07
#define T1_FI_PARTY_CHANGE_FLIGHT_POSITION				0x08
#define T1_FI_PARTY_CANCEL_FLIGHT_POSITION				0x09
#define T1_FI_PARTY_NOTIFY_BATTLE_PARTY					0x10
#define T1_FI_PARTY_NOTIFY_BATTLE_PARTY_OK				0x12
#define T1_FI_PARTY_ADD_MEMBER							0x13
#define T1_FI_PARTY_DELETE_MEMBER						0x14
#define T1_FI_PARTY_UPDATE_ITEM_POS						0x15	// F->I, 파티원이 아이템 장착을 수정했을 때 전송
#define T1_FI_PARTY_ALL_FLIGHT_POSITION					0x16	// F->I, 파티장이 재 설정한 파티원들의 Position
#define T1_FI_PARTY_UPDATE_PARTY_INFO					0x17
#define T1_FI_PARTY_CHANGE_EXP_DISTRIBUTE_TYPE			0x18	// 2008-06-04 by dhjin, EP3 편대 수정 - 경험치 분배 방식 변경 
#define T1_FI_PARTY_CHANGE_ITEM_DISTRIBUTE_TYPE			0x19	// 2008-06-04 by dhjin, EP3 편대 수정 - 아이템 분배 방식 변경
#define T1_FI_PARTY_CHANGE_FORMATION_SKILL				0x20	// 2009-08-03 by cmkwon, EP3-4 편대 대형 스킬 구현 - 
#define T1_FI_PARTY_AUTO_CREATE							0x30	// 2009-09-09 ~ 2010-01-26 by dhjin, 인피니티 - 자동 편대 생성 요청, F -> I
#define T1_FI_PARTY_AUTO_CREATE_OK						0x31	// 2009-09-09 ~ 2010-01-26 by dhjin, 인피니티 - 자동 편대 생성 완료, I -> F
#define T1_FI_PARTY_DISCONNECT_LEAVE_OK					0x32	// 2009-09-09 ~ 2010-01-27 by dhjin, 인피니티 - 강제 종료 유저 파티에서 탈퇴처리!
#define T1_FI_PARTY_UPDATE_ITEM_TRANSFORMER_OK			0x33	// F->I, // 2010-03-18 by cmkwon, 몬스터변신 구현 - 

// FI_CHARACTER
#define T1_FI_CHARACTER_DELETE_CHARACTER				0x01
#define T1_FI_CHARACTER_CHANGE_LEVEL					0x02
#define T1_FI_CHARACTER_UPDATE_GUILD_INFO				0x03
#define T1_FI_CHARACTER_UPDATE_MAP_CHANNEL				0x04
#define T1_FI_CHARACTER_CHANGE_INFLUENCE_TYPE			0x05	// F->I, 2005-12-03 by cmkwon
#define T1_FI_UPDATE_SUBLEADER							0x06	// F->I, 2005-12-03 by cmkwon
#define T1_FI_CREATE_GUILD_BY_SUBLEADER					0x07	// F->I, 2007-10-06 by dhjin

// IC_PARTY
#define T1_IC_PARTY_CREATE								0x00
#define T1_IC_PARTY_ACCEPT_INVITE_OK					0x01
#define T1_IC_PARTY_GET_MEMBER							0x02
#define T1_IC_PARTY_PUT_MEMBER							0x03
#define T1_IC_PARTY_GET_ALL_MEMBER						0x04
#define T1_IC_PARTY_PUT_ALL_MEMBER						0x05
#define T1_IC_PARTY_BAN_MEMBER							0x06
#define T1_IC_PARTY_BAN_MEMBER_OK						0x07
#define T1_IC_PARTY_LEAVE								0x08
#define T1_IC_PARTY_LEAVE_OK							0x09
#define T1_IC_PARTY_TRANSFER_MASTER						0x0A
#define T1_IC_PARTY_TRANSFER_MASTER_OK					0x0B
#define T1_IC_PARTY_DISMEMBER							0x0C
#define T1_IC_PARTY_DISMEMBER_OK						0x0D
#define T1_IC_PARTY_CHANGE_FLIGHT_FORMATION				0x0E	// Cm -> I
#define T1_IC_PARTY_CHANGE_FLIGHT_FORMATION_OK			0x0F	// I -> C
#define T1_IC_PARTY_GET_FLIGHT_POSITION					0x10	// C -> I -> Cm
#define T1_IC_PARTY_CHANGE_FLIGHT_POSITION				0x11	// Cm -> I -> C
#define T1_IC_PARTY_CANCEL_FLIGHT_POSITION				0x12	// C -> I -> Cm
#define T1_IC_PARTY_PUT_LAST_PARTY_INFO					0x13	// I -> C
#define T1_IC_PARTY_UPDATE_MEMBER_INFO_MAPNAME			0x14	// I -> C, 워프시 맵이름 전송
#define T1_IC_PARTY_MEMBER_INVALIDATED					0x15	// I -> C, 파티원이 비정상적으로 게임에서 튕겼을 때 전송
#define T1_IC_PARTY_MEMBER_REJOINED						0x16	// I -> C, 파티원이 다시 게임을 시작하였을 때 전송
#define T1_IC_PARTY_UPDATE_ITEM_POS						0x17	// I -> C, 파티원이 아이템 장착을 수정했을 때 전송
#define T1_IC_PARTY_ALL_FLIGHT_POSITION					0x18
#define T1_IC_PARTY_REQUEST_PARTYINFO_FROM_A_TO_M		0x19	// 2008-02-28 by dhjin, 아레나 통합 - C->I, 아레나 종료 후 메인 서버에 왔을 때 기존 파티 검사 
#define T1_IC_PARTY_LEAVE_FROM_M_TO_A					0x1A	// 2008-02-28 by dhjin, 아레나 통합 - C->I, 아레나 시작을 위해 메인서버에서 파티 관련 처리
#define T1_IC_PARTY_LEAVE_FROM_A_TO_M					0x1B	// 2008-02-28 by dhjin, 아레나 통합 - C->I, 아레나 서버에서 메인서버로 복귀할때 아레나 서버에서 파티 탈퇴 처리 클라이언트에게 전송하지 않는다.
#define T1_IC_PARTY_LIST_INFO							0x1C	// C -> I, 2008-06-02 by dhjin, EP3 편대 수정 - 편대 정보 리스트 요청
#define T1_IC_PARTY_LIST_INFO_OK						0x1D	// I -> C, 2008-06-02 by dhjin, EP3 편대 수정 - 편대 정보 리스트 요청 OK
#define T1_IC_PARTY_JOIN_FREE							0x1E	// C -> I, 2008-06-03 by dhjin, EP3 편대 수정 - 편대 자유 참여
#define T1_IC_PARTY_JOIN_FREE_OK						0x1F	// I -> C, 2008-06-03 by dhjin, EP3 편대 수정 - 편대 자유 참여 OK
#define	T1_IC_PARTY_CHANGE_INFO							0x20	// C -> I, 2008-06-04 by dhjin, EP3 편대 수정 - 편대 정보 수정
#define T1_IC_PARTY_CHANGE_INFO_OK						0x21	// I -> C, 2008-06-04 by dhjin, EP3 편대 수정 - 편대 정보 수정 OK
#define T1_IC_PARTY_RECOMMENDATION_MEMBER				0x22	// C -> I, 2008-06-04 by dhjin, EP3 편대 수정 - 추천 케릭터 요청
#define T1_IC_PARTY_RECOMMENDATION_MEMBER_OK			0x23	// I -> C, 2008-06-04 by dhjin, EP3 편대 수정 - 추천 케릭터 요청 OK
#define T1_IC_PARTY_INFO								0x24	// I -> C, 2008-06-10 by dhjin, EP3 편대 수정 - 편대 정보 전송
#define T1_IC_PARTY_GET_AUTO_PARTY_INFO					0x30	// 2009-09-09 ~ 2010-01-26 by dhjin, 인피니티 - 자동 편대 정보 요청, C -> I
#define T1_IC_PARTY_GET_AUTO_PARTY_INFO_OK				0x31	// 2009-09-09 ~ 2010-01-26 by dhjin, 인피니티 - 자동 편대 정보 전송, I -> C
#define T1_IC_PARTY_UPDATE_ITEM_TRANSFORMER_OK			0x32	// I -> C(n), // 2010-03-18 by cmkwon, 몬스터변신 구현 - 

// FC_MONSTER
#define T1_FC_MONSTER_CREATED							0x01
#define T1_FC_MONSTER_MOVE_OK							0x02
#define T1_FC_MONSTER_HIDE								0x03
#define T1_FC_MONSTER_SHOW								0x04
#define T1_FC_MONSTER_CHANGE_HP							0x05
#define T1_FC_MONSTER_CHANGE_BODYCONDITION				0x06
#define T1_FC_MONSTER_SKILL_USE_SKILL					0x07
#define T1_FC_MONSTER_SKILL_END_SKILL					0x08
#define T1_FC_MONSTER_SUMMON_MONSTER					0x09	// C->F
#define T1_FC_MONSTER_TUTORIAL_MONSTER_DEAD_NOTIFY		0x0A	// F->C
#define T1_FC_MONSTER_TUTORIAL_MONSTER_DELETE			0x0B	// F->C
#define T1_FC_MONSTER_HPTALK							0x0C	// 2009-09-09 ~ 2010 by dhjin, 인피니티 - , F->C(n)
#define T1_FC_MONSTER_SKILL								0x0D	// 2009-09-09 ~ 2010 by dhjin, 인피니티 - , F->C(n)
#define T1_FC_MONSTER_CREATED_ALIVE_FOR_GAMECLEAR		0x0E	// 2010-03-31 by dhjin, 인피니티(기지방어) -
#define T1_FC_MONSTER_SKILL_CANCEL						0x0F	// 2010-03-31 by dhjin, 인피니티(기지방어) - F->C(n)
#define T1_FC_MONSTER_CHANGE_INDEX						0x10	// 2011-05-17 by hskim, 인피니티 3차 - 시네마 몬스터 교체 기능 , F->C(n)
#define T1_FC_MONSTER_CINEMA_DELETE_NOTIFY				0x11	// 2011-05-30 by hskim, 인피니티 3차 - 시네마에서 몬스터 삭제 클라이언트 반영 F->C(n)

// FN_MONSTER
#define T1_FN_MAPPROJECT_START							0x02
#define T1_FN_MAPPROJECT_START_OK						0x03
#define T1_FN_MAPPROJECT_START_OK_ACK					0x04
#define T1_FN_MONSTER_CREATE							0x05
#define T1_FN_MONSTER_CREATE_OK							0x06
#define T1_FN_MONSTER_DELETE							0x07
#define T1_FN_CLIENT_GAMESTART_OK						0x08
#define T1_FN_CLIENT_GAMEEND_OK							0x09
#define T1_FN_GET_CHARACTER_INFO						0x0A
#define T1_FN_GET_CHARACTER_INFO_OK						0x0B
#define T1_FN_ADMIN_SUMMON_MONSTER						0x0C
#define T1_FN_MONSTER_CHANGE_HP							0x0D
#define T1_FN_MONSTER_CHANGE_BODYCONDITION				0x0E
#define T1_FN_MONSTER_SKILL_USE_SKILL					0x0F
#define T1_FN_MONSTER_SKILL_END_SKILL					0x10
#define T1_FN_MONSTER_AUTO_DESTROYED					0x20	// N->F, 2006-04-17 by cmkwon
#define T1_FN_MONSTER_STRATEGYPOINT_INIT				0x30	// 2006-11-20 by cmkwon, F->N
#define T1_FN_MONSTER_STRATEGYPOINT_SUMMON				0x31	// 2007-03-02 by dhjin, F->N
#define T1_FN_MONSTER_OUTPOST_INIT						0x35	// 2007-08-24 by dhjin, F->N
#define T1_FN_MONSTER_OUTPOST_RESET_SUMMON				0x36	// 2007-08-24 by dhjin, F->N
#define T1_FN_MONSTER_CREATE_IN_MAPCHANNEL_BYVALUE		0x37	// 2007-08-29 by dhjin, F->N
#define T1_FN_MONSTER_TELEPORT_SUMMON					0x38	// 2007-09-05 by dhjin, F->N
////////////////////////////////////////////////////////////////////////////////
// 2009-09-09 ~ 2010 by dhjin, 인피니티 - 
#define T1_FN_MONSTER_HPTALK							0x39	// 2009-09-09 ~ 2010 by dhjin, 인피니티 - , N->F
#define T1_FN_MONSTER_KEYMONSTER_CREATE					0x50	// 2009-09-09 ~ 2010 by dhjin, 인피니티 - 키 몬스터 생성, F -> N
#define T1_FN_MONSTER_CHANGE_OK							0x51	// 2011-05-11 by hskim, 인피니티 3차 - 시네마 관련 기능 추가 - 해당 맵채널 특정 몬스터 변경 기능 추가 N -> F

// T0_FN_NPCSERVER
#define T1_FN_NPCSERVER_START							0x00	// TCP:
#define T1_FN_NPCSERVER_START_OK						0x01	// TCP:
#define T1_FN_NPCSERVER_SUMMON_JACO_MONSTER				0x02	// TCP:F->N, 
#define T1_FN_NPCSERVER_DELETE_MONSTER_IN_MAPCHANNEL	0x10	// TCP:F->N, // 2007-08-22 by cmkwon, 해당 맵채널 몬스터 모두 삭제하기 기능 추가
#define T1_FN_NPCSERVER_CINEMA_MONSTER_CREATE			0x11	// 2010-03-31 by dhjin, 인피니티(기지방어) -	// 2009-09-09 ~ 2010 by dhjin, 인피니티 - 키 몬스터 생성, F -> N
#define T1_FN_NPCSERVER_NEW_CHANGE_OBJECT				0x12		// 2009-09-09 ~ 2010 by dhjin, 인피니티 - 변경 오브젝트를 위해!!!! 
#define T1_FN_NPCSERVER_RESET_CHANGE_OBJECT				0x13		// 2009-09-09 ~ 2010 by dhjin, 인피니티 - 변경 오브젝트를 위해!!!! 
#define T1_FN_NPCSERVER_CINEMA_MONSTER_DESTROY			0x14	// 2011-04-28 by hskim, 인피니티 3차 - 시네마 관련 기능 추가 - 해당 맵채널 특정 몬스터 삭제 기능 추가
#define T1_FN_NPCSERVER_CINEMA_MONSTER_CHANGE			0x15	// 2011-05-11 by hskim, 인피니티 3차 - 시네마 관련 기능 추가 - 해당 맵채널 특정 몬스터 변경 기능 추가
#define T1_FN_NPCSERVER_CINEMA_MONSTER_REGEN			0x16	// 2011-06-02 by hskim, 인피니티 3차 - 스텝 6 - 주기적 소환 기능 제작


// FC_EVENT
#define T1_FC_EVENT_WARP						0x00
#define T1_FC_EVENT_WARP_SAME_MAP				0x01
#define T1_FC_EVENT_WARP_SAME_MAP_DONE			0x02
#define T1_FC_EVENT_WARP_SAME_FIELD_SERVER		0x03
#define T1_FC_EVENT_WARP_SAME_FIELD_SERVER_DONE	0x04
#define T1_FC_EVENT_OTHER_WARPED				0x05
#define T1_FC_EVENT_WARP_CONNECT				0x06
#define T1_FC_EVENT_WARP_CONNECT_OK				0x07
#define T1_FC_EVENT_ENTER_BUILDING				0x08
#define T1_FC_EVENT_ENTER_BUILDING_OK			0x09
#define T1_FC_EVENT_LEAVE_BUILDING				0x0A
#define T1_FC_EVENT_LEAVE_BUILDING_OK			0x0B
#define T1_FC_EVENT_REQUEST_WARP				0x0C
#define T1_FC_EVENT_REJECT_WARP					0x0D
#define T1_FC_EVENT_REQUEST_OBJECT_EVENT		0x0E
#define T1_FC_EVENT_CHANGE_WEATHER				0x0F
#define T1_FC_EVENT_SUGGEST_CHANNELS			0x10
#define T1_FC_EVENT_SELECT_CHANNEL				0x11
#define T1_FC_EVENT_SELECT_CHANNEL_WITH_PARTY	0x12
#define T1_FC_EVENT_REQUEST_RACING_WARP			0x13
#define T1_FC_EVENT_GET_SHOP_WARP_TARGET_MAP_LIST		0x14
#define T1_FC_EVENT_GET_SHOP_WARP_TARGET_MAP_LIST_OK	0x15
#define T1_FC_EVENT_REQUEST_SHOP_WARP					0x16
#define T1_FC_EVENT_CHARACTERMODE_ENTER_BUILDING		0x17	// 2005-07-28 by cmkwon
#define T1_FC_EVENT_CALL_WARP_EVENT_REQUEST				0x20	// 2006-07-21 by cmkwon
#define T1_FC_EVENT_CALL_WARP_EVENT_REQUEST_ACK			0x21	// 2006-07-21 by cmkwon
#define T1_FC_EVENT_CLICK_TELEPORT						0x22	// 2007-09-06 by dhjin
#define T1_FC_EVENT_CLICK_TELEPORT_OK					0x23	// 2008-04-22 by dhjin, 모선전 정보 표시 기획안 - 텔레포트 빌딩 완료 시간
#define T1_FC_EVENT_TELEPORT_BUILDCOMPLETE				0x24	// 2007-09-19 by dhjin
#define T1_FC_EVENT_TELEPORT_DESTROY					0x25	// 2007-09-19 by dhjin

#define T1_FC_EVENT_NOTIFY_MSG_GET						0x30	// 2007-11-28 by cmkwon, 통지시스템 구현 - C->F
#define T1_FC_EVENT_NOTIFY_MSG_GET_OK					0x31	// 2007-11-28 by cmkwon, 통지시스템 구현 - F->C
#define T1_FC_EVENT_NOTIFY_MSG_DELETE					0x32	// 2007-11-28 by cmkwon, 통지시스템 구현 - C->F

#define T1_FC_EVENT_COUPON_EVENT_INFO					0x36	// 2008-01-10 by cmkwon, 아이템 이벤트 시스템에 신 쿠폰 시스템 추가 - F->C
#define T1_FC_EVENT_COUPON_EVENT_USE_COUPON				0x37	// 2008-01-10 by cmkwon, 아이템 이벤트 시스템에 신 쿠폰 시스템 추가 - C->F
#define T1_FC_EVENT_COUPON_EVENT_USE_COUPON_OK			0x38	// 2008-01-10 by cmkwon, 아이템 이벤트 시스템에 신 쿠폰 시스템 추가 - F->C

#define T1_FC_EVENT_INFLUENCEMARK					0x3A	// 2008-08-18 by dhjin, 세력마크이벤트 
#define T1_FC_EVENT_INFLUENCEMARKEND				0x3B	// 2008-08-18 by dhjin, 세력마크이벤트 

// FN_EVENT
#define T1_FN_EVENT_WARP						0x00
#define T1_FN_EVENT_OTHER_WARPED				0x01
#define T1_FN_EVENT_WARP_CONNECT_OK				0x02
////////////////////////////////////////////////////////////////////////////////
// 2009-09-09 ~ 2010 by dhjin, 인피니티 - 변경 오브젝트를 위해!!!! 
#define T1_FN_EVENT_NEW_CHANGE_OBJECT			0x10
#define T1_FN_EVENT_RESET_CHANGE_OBJECT			0x11
	
// FP_EVENT	
#define T1_FP_EVENT_NOTIFY_WARP					0x00
#define T1_FP_EVENT_NOTIFY_WARP_OK				0x01
#define T1_FP_EVENT_ENTER_SELECT_SCREEN			0x02	// F->P, Client가 캐릭터 선택 창에 들어옴
#define T1_FP_EVENT_GAME_STARTED				0x03	// F->P, Client가 게임을 시작함(맵으로 들어옴)
#define T1_FP_EVENT_MAP_CHANGED					0x04	// F->P, Client가 맵을 이동함
#define T1_FP_EVENT_RELOAD_HAPPYEV				0x05	// P->F, No body, All ServerGroup reload HappyHourEvent.
#define T1_FP_EVENT_RELOAD_ITEMEV				0x06	// P->F, No body, All ServerGroup reload ItemEvent.	
#define T1_FP_EVENT_UPDATE_PCBANGLIST			0x07	// P->F, No body,
#define T1_FP_EVENT_UPDATE_STRATEGYPOINT_NOTSUMMONTIM			0x08	// P->F, No body,

// T0_FP_MONITOR	
#define T1_FP_MONITOR_SET_MGAME_EVENT_TYPE		0x01
#define T1_FP_MONITOR_RELOAD_VERSION_INFO_OK	0x02	// P->F(n), No body, // 2008-09-08 by cmkwon, SCMonitor에서 ReloadVersionInfo시에 일부 체크섬파일(.\Res-Tex\*.*)도 리로드하기 - 

// T0_FP_CASH	
#define T1_FP_CASH_CHANGE_CHARACTERNAME			0x00

// #define T0_FP_ADMIN					0x73			// 2008-01-31 by cmkwon, 계정 블럭/해제 명령어로 가능한 시스템 구현 - 
#define T1_FP_ADMIN_BLOCKACCOUNT				0x00	// 2008-01-31 by cmkwon, 계정 블럭/해제 명령어로 가능한 시스템 구현 - 
#define T1_FP_ADMIN_BLOCKACCOUNT_OK				0x01	// 2008-01-31 by cmkwon, 계정 블럭/해제 명령어로 가능한 시스템 구현 - 
#define T1_FP_ADMIN_UNBLOCKACCOUNT				0x02	// 2008-01-31 by cmkwon, 계정 블럭/해제 명령어로 가능한 시스템 구현 - 
#define T1_FP_ADMIN_UNBLOCKACCOUNT_OK			0x03	// 2008-01-31 by cmkwon, 계정 블럭/해제 명령어로 가능한 시스템 구현 - 
#define T1_FP_ADMIN_STRATRGYPOINT_INFO_CHANGE	0x04
// FC_STORE
#define T1_FC_STORE_GET_ITEM					0x00
#define T1_FC_STORE_PUT_ITEM_HEADER				0x01
#define T1_FC_STORE_PUT_ITEM					0x02
#define T1_FC_STORE_PUT_ITEM_DONE				0x03
#define T1_FC_STORE_MOVE_ITEM					0x04
#define T1_FC_STORE_MOVE_ITEM_OK				0x05
#define T1_FC_STORE_INSERT_ITEM					0x06
#define T1_FC_STORE_DELETE_ITEM					0x07
#define T1_FC_STORE_UPDATE_ITEM_COUNT			0x08
#define T1_FC_STORE_UPDATE_ENDURANCE			0x09
#define T1_FC_STORE_UPDATE_POSSESS				0x0A
#define T1_FC_STORE_UPDATE_RARE_FIX				0x0B
#define T1_FC_STORE_INSERT_USINGITEM			0x0C		// F->C
#define T1_FC_STORE_DELETE_USINGITEM			0x0D		// F->C
#define T1_FC_STORE_UPDATE_USINGITEM			0x0E		// F->C, 2006-03-30 by cmkwon
#define T1_FC_STORE_EXPIRE_USINGITEM			0x0F		// C->F, 2006-10-11 by cmkwon, 사용 시간이 끝난것을 서버로 알림
#define T1_FC_STORE_UPDATE_ITEMSTORAGE			0x10			// 2005-12-07 by cmkwon
#define T1_FC_STORE_UPDATE_ITEMNUM				0x11			// 2006-06-14 by cmkwon
#define T1_FC_STORE_REQUEST_QUICKSLOT			0x12		// 2006-09-04 by dhjin
#define T1_FC_STORE_REQUEST_QUICKSLOT_OK		0x13		// 2006-09-04 by dhjin
#define T1_FC_STORE_SAVE_QUICKSLOT				0x14		// 2006-09-04 by dhjin
#define T1_FC_STORE_LOG_GUILD_ITEM				0x15		// 2006-09-27 by dhjin
#define T1_FC_STORE_LOG_GUILD_ITEM_OK			0x16		// 2006-09-27 by dhjin
#define T1_FC_STORE_LOG_GUILD_ITEM_OK_HEADER	0x17		// 2006-09-27 by dhjin
#define T1_FC_STORE_LOG_GUILD_ITEM_OK_DONE		0x18		// 2006-09-27 by dhjin

// FC_ITEM
#define T1_FC_ITEM_SHOW_ITEM							0x00
#define T1_FC_ITEM_HIDE_ITEM							0x01
#define T1_FC_ITEM_GET_ITEM								0x02
#define T1_FC_ITEM_GET_ITEM_OK							0x03
#define T1_FC_ITEM_GET_ITEM_FAIL						0x04
#define T1_FC_ITEM_PUT_ITEM								0x05
// 2005-11-15 by cmkwon, 제거함
//#define T1_FC_ITEM_BONUSSKILLPOINT						0x06
//#define T1_FC_ITEM_BONUSSKILLPOINT_OK					0x07
#define T1_FC_ITEM_CHANGE_WINDOW_POSITION				0x08
#define T1_FC_ITEM_CHANGE_WINDOW_POSITION_OK			0x09
#define T1_FC_ITEM_UPDATE_WINDOW_ITEM_LIST				0x0A
#define T1_FC_ITEM_THROW_AWAY_ITEM						0x0B
#define T1_FC_ITEM_THROW_AWAY_ITEM_OK					0x0C
#define T1_FC_ITEM_USE_ENERGY							0x0D
#define T1_FC_ITEM_USE_ENERGY_OK						0x0E	// F->C
#define T1_FC_ITEM_USE_ITEM								0x0F
#define T1_FC_ITEM_USE_ITEM_OK							0x10	// F->C
#define T1_FC_ITEM_DELETE_ITEM_ADMIN					0x11
#define T1_FC_ITEM_RELOAD_ITEM_INFO						0x12	// 아이템 정보가 업데이트되었음
#define T1_FC_ITEM_USE_ENCHANT							0x13
#define T1_FC_ITEM_USE_ENCHANT_OK						0x14
#define T1_FC_ITEM_PUT_ENCHANT_HEADER					0x15
#define T1_FC_ITEM_PUT_ENCHANT							0x16
#define T1_FC_ITEM_PUT_ENCHANT_DONE						0x17
#define T1_FC_ITEM_DELETE_ALL_ENCHANT					0x18	// F->C, 모든 인챈트를 삭제한다
#define T1_FC_ITEM_DELETE_DROP_ITEM						0x19	// F->C, 자신이 뿌린 아이템(마인등)을 지울 때 쓰임
#define T1_FC_ITEM_UPDATE_ITEM_POS						0x1A	// F->C, 아이템 장착(전)을 갱신함, 아이템 장착을 바꾸면 주위에 전송함
#define T1_FC_ITEM_MIX_ITEMS							0x20	// C->F, 조합할 아이템의 리스트를 전송
#define T1_FC_ITEM_MIX_ITEMS_RESULT						0x21	// F->C, 아이템 조합 결과를 전송
#define T1_FC_ITEM_USE_CARDITEM_GUILDSUMMON				0x22	// 길드 소환 카드
#define T1_FC_ITEM_USE_CARDITEM_GUILDSUMMON_NOTIFY		0x23	// 길드 소환 카드로 소환됨을 알림
#define T1_FC_ITEM_USE_CARDITEM_RESTORE					0x24	// 부활 카드
#define T1_FC_ITEM_USE_CARDITEM_RESTORE_NOTIFY			0x25	// 부활 카드로 부활됨을 알림
#define T1_FC_ITEM_USE_CARDITEM_GUILD					0x26	// C->F, 일반/고급 여단 카드
#define T1_FC_ITEM_USE_CARDITEM_GUILD_NOTIFY			0x27	// F->C, 일반/고급 여단 카드
#define T1_FC_ITEM_USE_CARDITEM_MONSTERSUMMON			0x28	// C->F, 몬스터 소환 카드
#define T1_FC_ITEM_USE_CARDITEM_MONSTERSUMMON_NOTIFY	0x29	// F->C, 몬스터 소환 카드
#define T1_FC_ITEM_USE_CARDITEM_CHANGECHARACTERNAME		0x2A	// C->F, 캐릭터 이름 변경 카드
#define T1_FC_ITEM_USE_CARDITEM_CHANGECHARACTERNAME_NOTIFY	0x2B	// F->C, 캐릭터 이름 변경 카드
#define T1_FC_ITEM_USE_CARDITEM_SKILLINITIALIZE			0x2C	// C->F, 스킬 초기화 카드
#define T1_FC_ITEM_USE_CARDITEM_SKILLINITIALIZE_NOTIFY	0x2D	// F->C, 스킬 초기화 카드
#define T1_FC_ITEM_USE_CARDITEM_CHANGEPILOTFACE			0x2E	// C->F, 얼굴 변경 카드
#define T1_FC_ITEM_USE_CARDITEM_CHANGEPILOTFACE_NOTIFY	0x2F	// F->C, 얼굴 변경 카드
#define T1_FC_ITEM_USE_INFLUENCE_BUFF					0x30	// C->F, 세력 버프		// 2006-04-21 by cmkwon
#define T1_FC_ITEM_USE_INFLUENCE_BUFF_OK				0x31	// F->C					// 2006-04-21 by cmkwon
#define T1_FC_ITEM_USE_INFLUENCE_GAMEEVENT				0x32	// C->F, 세력 이벤트	// 2006-04-21 by cmkwon
#define T1_FC_ITEM_USE_INFLUENCE_GAMEEVENT_OK			0x33	// F->C					 // 2006-04-21 by cmkwon
#define T1_FC_ITEM_USE_RANDOMBOX						0x34	// C->F, 2006-08-10 by cmkwon
#define T1_FC_ITEM_USE_RANDOMBOX_OK						0x35	// F->C(n), 2006-08-10 by cmkwon
#define T1_FC_ITEM_USE_SKILL_SUPPORT_ITEM				0x36	// C->F, 2006-09-29 by cmkwon
#define T1_FC_ITEM_USE_SKILL_SUPPORT_ITEM_OK			0x37	// F->C, 2006-09-29 by cmkwon
#define T1_FC_ITEM_USE_RANDOMBOX_OK_DONE				0x38	// F->C, // 2008-08-26 by cmkwon, ItemAttribute 추가 - 패키지(Package) 아이템, (no body) 클라이언트는 이 메시지를 받고 락상태를 해제 한다.
#define T1_FC_ITEM_USE_LUCKY_ITEM						0x39	// C->F, 2008-11-04 by dhjin, 럭키머신
#define T1_FC_ITEM_USE_LUCKY_ITEM_OK					0x3A	// F->C, 2008-11-04 by dhjin, 럭키머신
#define T1_FC_ITEM_USE_LUCKY_ITEM_WIN					0x3B	// C->F, 2008-11-04 by dhjin, 럭키머신
#define T1_FC_ITEM_USE_LUCKY_ITEM_WIN_OK				0x3C	// F->C, 2008-11-04 by dhjin, 럭키머신
#define T1_FC_ITEM_CHANGED_SHAPEITEMNUM					0x3D	// F->C, // 2009-08-26 by cmkwon, 그래픽 리소스 변경 시스템 구현 - 
#define T1_FC_ITEM_CHANGED_EFFECTITEMNUM				0x3E	// F->C, // 2009-08-26 by cmkwon, 그래픽 리소스 변경 시스템 구현 - 
#define T1_FC_ITEM_USE_INVOKING_WEAR_ITEM				0x40	// C->F, 2009-09-09 ~ 2010-02-10 by dhjin, 인피니티 - 발동류장착아이템 사용
#define T1_FC_ITEM_EXPIRE_TIME_INVOKING_WEAR_ITEM		0x41	// C->F, 2009-09-09 ~ 2010-02-10 by dhjin, 인피니티 - 발동류장착아이템 지속 시간 완료
#define T1_FC_ITEM_END_COOLINGTIME_ITEM					0x42	// C->F, 2009-09-09 ~ 2010-02-10 by dhjin, 인피니티 - 발동류장착아이템, 쿨타임 종료	
#define T1_FC_ITEM_END_COOLINGTIME_ITEM_OK				0x43	// F->C, 2009-09-09 ~ 2010-02-10 by dhjin, 인피니티 - 발동류장착아이템, 쿨타임 종료	
#define T1_FC_ITEM_GET_COOLINGTIME_INFO					0x44	// C->F, // 2009-09-09 ~ 2010-02-10 by dhjin, 인피니티 - 발동류장착아이템, 쿨타임 정보 요청
#define T1_FC_ITEM_GET_COOLINGTIME_INFO_OK				0x45	// F->C, // 2009-09-09 ~ 2010-02-10 by dhjin, 인피니티 - 발동류장착아이템, 쿨타임 정보 요청
#define T1_FC_ITEM_USE_INVOKING_WEAR_ITEM_BUFF			0x46	// F->C, // 2009-09-09 ~ 2010-02-10 by dhjin, 인피니티 - 발동류장착아이템, 버프 올리세요
#define T1_FC_ITEM_UPDATE_TRANSFORMER_OK				0x47	// F->C(n), // 2010-03-18 by cmkwon, 몬스터변신 구현 - 
#define T1_FC_ITEM_HOMEPREMIUM_INFO						0x48	// F->C, // 2010-06-01 by shcho, PC방 권한 획득(캐쉬) 아이템 -

#define T1_FC_ITEM_PET_HEADER							0x49	// 2010-06-15 by shcho&hslee 펫시스템 - 소유 펫 정보 로딩.
#define T1_FC_ITEM_PET									0x50	// 2010-06-15 by shcho&hslee 펫시스템 - 소유 펫 정보 로딩.
#define T1_FC_ITEM_PET_BASEDATA_OK						0x51	// 2010-06-15 by shcho&hslee 펫시스템 - 소유 펫 정보 로딩.
#define T1_FC_ITEM_PET_SKILLDATA_OK						0x52	// 2010-06-15 by shcho&hslee 펫시스템 - 소유 펫 정보 로딩.
#define T1_FC_ITEM_PET_SOCKETDATA_OK					0x53	// 2010-06-15 by shcho&hslee 펫시스템 - 소유 펫 정보 로딩.
#define T1_FC_ITEM_PET_DONE								0x54	// 2010-06-15 by shcho&hslee 펫시스템 - 소유 펫 정보 로딩.
#define T1_FC_ITEM_DISSOLUTIONITEM						0x55	// 2010-08-31 by shcho&jskim 아이템용해 시스템 - 용해 시스템 패킷 처리
#define T1_FC_ITEM_DISSOLUTIONITEM_OK					0x56	// 2010-08-31 by shcho&jskim 아이템용해 시스템 - 용해 시스템 패킷 처리
#ifdef _INET_PET
#define T1_FC_ITEM_PET_SET_NAME							0x57	// C->F, 2011-08-22 by hskim, 파트너 시스템 2차 - 펫 이름 변경 요청
#define T1_FC_ITEM_PET_SET_NAME_OK						0x58	// F->C, 2011-08-22 by hskim, 파트너 시스템 2차 - 펫 이름 변경 요청 결과
#define T1_FC_ITEM_PET_SET_EXP_RATIO					0x59	// C->F, 2011-08-22 by hskim, 파트너 시스템 2차 - 펫 경험치 획득 비율 변경 요청
#define T1_FC_ITEM_PET_SET_EXP_RATIO_OK					0x60	// F->C, 2011-08-22 by hskim, 파트너 시스템 2차 - 펫 경험치 획득 비율 변경 결과
#define T1_FC_ITEM_PET_CHANGE_LEVEL						0x61	// C->F, 2011-08-22 by hskim, 파트너 시스템 2차 - 기능 구현
#define T1_FC_ITEM_PET_CHANGE_EXP						0x62	// C->F, 2011-08-22 by hskim, 파트너 시스템 2차 - 기능 구현

#define T1_FC_ITEM_PET_SET_SOCKET						0x63	// C->F, 2011-08-22 by hskim, 파트너 시스템 2차 - 소켓 아이템 설정
#define T1_FC_ITEM_PET_SET_SOCKET_OK					0x64	// F->C, 2011-08-22 by hskim, 파트너 시스템 2차 - 소켓 아이템 설정 결과
#define T1_FC_ITEM_PET_SET_KIT_SLOT						0x65	// C->F, 2011-08-22 by hskim, 파트너 시스템 2차 - 키트 슬롯 설정
#define T1_FC_ITEM_PET_SET_KIT_SLOT_OK					0x66	// F->C, 2011-08-22 by hskim, 파트너 시스템 2차 - 키트 슬롯 설정 결과
#define T1_FC_ITEM_PET_SET_KIT_AUTOSKILL_SLOT			0x67	// C->F, 2011-08-22 by hskim, 파트너 시스템 2차 - 오토 스킬 슬롯 설정
#define T1_FC_ITEM_PET_SET_KIT_AUTOSKILL_SLOT_OK		0x68	// F->C, 2011-08-22 by hskim, 파트너 시스템 2차 - 오토 스킬 슬롯 설정

#define T1_FC_ITEM_USE_PET_SOCKET_ITEM					0x69	// C->F, // 2011-09-20 by hskim, 파트너 시스템 2차 - 소켓 아이템 사용 (토글)
#define T1_FC_ITEM_USE_PET_SOCKET_ITEM_OK				0x6a	// F->C, // 2011-09-20 by hskim, 파트너 시스템 2차 - 소켓 아이템 사용 결과 (토글)
#define T1_FC_ITEM_CANCEL_PET_SOCKET_ITEM				0x6b	// C->F, // 2011-09-20 by hskim, 파트너 시스템 2차 - 소켓 아이템 사용 중지 (토글)
#define T1_FC_ITEM_CANCEL_PET_SOCKET_ITEM_OK			0x6c	// F->C, // 2011-09-20 by hskim, 파트너 시스템 2차 - 소켓 아이템 사용 중지 결과 (토글)
#define T1_FC_ITEM_NOTIFY_WINDOW_POSITION				0x6d	// F->C, // 2011-09-20 by hskim, 파트너 시스템 2차 - ItemWindowIndex 변경 알림 (F->C) , 아이템 숨김 처리

#define T1_FC_MINIMAP_SET_MARKER						0x70
#endif
#ifdef BONUS_STAT_ITEM
#define T1_FC_ITEM_PUT_BONUS_HEADER						0x6e
#define T1_FC_ITEM_PUT_BONUS							0x6f
#define T1_FC_ITEM_PUT_BONUS_DONE						0x70
#endif
#ifdef _INET_LINK_CHAT
#define T1_FC_GET_ITEM_BYCHARNAME8ITEMUID							0x71
#define T1_FC_GET_ITEM_BYCHARNAME8ITEMUID_OK						0x72
#endif
// FC_SHOP	
#define T1_FC_SHOP_PUT_ITEM_HEADER			0x00
#define T1_FC_SHOP_PUT_ITEM					0x01
#define T1_FC_SHOP_PUT_ITEM_DONE			0x02
#define T1_FC_SHOP_GET_ITEMINFO				0x03
#define T1_FC_SHOP_GET_ITEMINFO_OK			0x04
#define T1_FC_SHOP_BUY_ITEM					0x05
#define T1_FC_SHOP_BUY_ITEM_OK				0x06
#define T1_FC_SHOP_SELL_ITEM				0x07
#define T1_FC_SHOP_SELL_ITEM_OK				0x08
#define T1_FC_SHOP_GET_USED_ITEM_PRICE		0x09
#define T1_FC_SHOP_GET_USED_ITEM_PRICE_OK	0x0A
#define T1_FC_SHOP_GET_SHOP_ITEM_LIST		0x0B
#define T1_FC_SHOP_REQUEST_REPAIR			0x0C	// C->F, 기체 수리 요청
#define T1_FC_SHOP_REQUEST_REPAIR_OK		0x0D	// F->C, 기체 수리 요청 성공
#define T1_FC_SHOP_BUY_CASH_ITEM			0x0E	// C->F, 유료 아이템 구입
#define T1_FC_SHOP_BUY_CASH_ITEM_OK			0x0F	// F->C, // 2007-11-13 by cmkwon, 선물하기 기능 추가 -유료 아이템 구입 성공
#define T1_FC_SHOP_BUY_COLOR_ITEM			0x10	// C->F, 색상 아이템 구입// 2005-12-06 by cmkwon
#define T1_FC_SHOP_BUY_COLOR_ITEM_OK		0x11	// F->C, 색상 아이템 구입 성공// 2005-12-06 by cmkwon
#define T1_FC_SHOP_BUY_WARPOINT_ITEM		0x12	// C->F, WarPoint 아이템 구입 // 2007-06-13 by dhjin
#define T1_FC_SHOP_BUY_WARPOINT_ITEM_OK		0x13	// F->C, // 2007-06-13 by dhjin
#define T1_FC_SHOP_CHECK_GIVE_TARGET		0x14	// C->F, // 2007-11-13 by cmkwon, 선물하기 기능 추가 - 선물받는 캐릭터 체크 요청 프로토콜
#define T1_FC_SHOP_CHECK_GIVE_TARGET_OK		0x15	// F->C, // 2007-11-13 by cmkwon, 선물하기 기능 추가 -
#define T1_FC_SHOP_INFINITY_ITEM_HEADER		0x16	// 2009-09-09 ~ 2010 by dhjin, 인피니티 - 상점
#define T1_FC_SHOP_INFINITY_ITEM			0x17	// 2009-09-09 ~ 2010 by dhjin, 인피니티 - 상점
#define T1_FC_SHOP_INFINITY_ITEM_DONE		0x18	// 2009-09-09 ~ 2010 by dhjin, 인피니티 - 상점
#define T1_FC_SHOP_INFINITY_BUY_ITEM		0x19	// 2009-09-09 ~ 2010 by dhjin, 인피니티 - 상점, 구매

// FC_TRADE
#define T1_FC_TRADE_REQUEST_TRADE			0x00
#define T1_FC_TRADE_REQUEST_TRADE_OK		0x01
#define T1_FC_TRADE_CANCEL_REQUEST			0x02
#define T1_FC_TRADE_CANCEL_REQUEST_OK		0x03
#define T1_FC_TRADE_ACCEPT_TRADE			0x04
#define T1_FC_TRADE_ACCEPT_TRADE_OK			0x05
#define T1_FC_TRADE_REJECT_TRADE			0x06
#define T1_FC_TRADE_REJECT_TRADE_OK			0x07
#define T1_FC_TRADE_REJECT_TRADING			0x08
#define T1_FC_TRADE_SHOW_TRADE_WINDOW		0x09
#define T1_FC_TRADE_TRANS_ITEM				0x0A
#define T1_FC_TRADE_TRANS_ITEM_OK			0x0B
#define T1_FC_TRADE_SEE_ITEM				0x0C
#define T1_FC_TRADE_SEE_ITEM_OK				0x0D
#define T1_FC_TRADE_OK_TRADE				0x0E
#define T1_FC_TRADE_OK_TRADE_OK				0x0F
#define T1_FC_TRADE_CANCEL_TRADE			0x10
#define T1_FC_TRADE_CANCEL_TRADE_OK			0x11
#define T1_FC_TRADE_INSERT_ITEM				0x12
#define T1_FC_TRADE_DELETE_ITEM				0x13
#define T1_FC_TRADE_UPDATE_ITEM_COUNT		0x14
#define T1_FC_TRADE_OK_TRADE_NOTIFY			0x15	// 2008-11-21 by cmkwon, 거래 승인 확인 시스템 구현 - F->C(2)
// 2010-06-15 by shcho&hslee 펫시스템 - 거래 시 펫 정보 전송
#define T1_FC_TRADE_SEE_PET_DATA			0x16	
#define T1_FC_TRADE_DELETE_CURRENT_PET_DATA	0x17 // F->C 트레이드 성공시 데이터 삭제 알림 패킷
#define T1_FC_TRADE_INSERT_CURRENT_PET_DATA	0x18 // F->C 트레이드 성공시 데이터 추가 알림 패킷
// END 2010-06-15 by shcho&hslee 펫시스템 - 거래 시 펫 정보 전송
#ifdef BONUS_STAT_ITEM
#define T1_FC_TRADE_GET_BONUS_ITEM			0x19
#define T1_FC_TRADE_SEND_BONUS				0x20
#endif
// T0_FC_COUNTDOWN
#define T1_FC_COUNTDOWN_START					0x00	// F -> C
#define T1_FC_COUNTDOWN_DONE					0x01	// C -> F

// T0_FC_OBJECT
#define T1_FC_OBJECT_CHANGE_BODYCONDITION		0x00
#define T1_FC_OBJECT_CHANGE_BODYCONDITION_OK	0x01

// T0_FC_AUCTION
#define T1_FC_AUCTION_REGISTER_ITEM			0x00	// C->F, 경매 아이템 등록
#define T1_FC_AUCTION_REGISTER_ITEM_OK		0x01	// F->C, 경매 아이템 등록 결과
#define T1_FC_AUCTION_CANCEL_REGISTER		0x02	// C->F, 경매 아이템 등록 취소
#define T1_FC_AUCTION_CANCEL_REGISTER_OK	0x03	// F->C, 경매 아이템 등록 취소 결과
#define T1_FC_AUCTION_BUY_ITEM				0x04	// C->F, 경매 아이템 구매
#define T1_FC_AUCTION_BUY_ITEM_OK			0x05	// F->C, 경매 아이템 구매 결과
#define T1_FC_AUCTION_GET_ITEM_LIST			0x06	// C->F, 경매 아이템 목록 요청
#define T1_FC_AUCTION_INSERT_ITEM			0x07	// F->C, 경매 아이템 전송용
#define T1_FC_AUCTION_PUT_ENCHANT			0x08	// F->C, 경매 아이템의 인챈트 정보 전송용

// FC_GUILD
#define T1_FC_GUILD_GET_MAP_OWNER_INFO			0x00
#define T1_FC_GUILD_GET_MAP_OWNER_INFO_OK		0x01
#define T1_FC_GUILD_REQUEST_GUILD_WAR			0x02
#define T1_FC_GUILD_REQUEST_GUILD_WAR_RESULT	0x03
#define T1_FC_GUILD_GET_CHALLENGER_GUILD		0x04
#define T1_FC_GUILD_GET_CHALLENGER_GUILD_OK		0x05
#define T1_FC_GUILD_GET_WAR_INFO				0x06	// C->F
#define T1_FC_GUILD_GET_WAR_INFO_OK				0x07	// F->C
#define T1_FC_GUILD_SURRENDER_GUILD_WAR			0x08	// C->F
#define T1_FC_GUILD_SUMMON_MEMBER				0x09	// F->C(n)
#define T1_FC_GUILD_SUMMON_MEMBER_OK			0x0A	// C(n)->F
#define T1_FC_GUILD_DISMEMBER					0x0B	// C->F

// T0_FI_GUILD
#define T1_FI_GUILD_NOTIFY_START_WAR			0x00	// F->I
#define T1_FI_GUILD_NOTIFY_END_WAR				0x01	// I->F
#define T1_FI_GUILD_DELETE_GUILD				0x02	// F->I
#define T1_FI_GUILD_ADD_GUILD_FAME				0x10	// F->I
#define T1_FI_GUILD_REG_DELETE_GUILD			0x11	// I->F
#define T1_FI_GUILD_DISMEMBER					0x12	// F->I
#define T1_FI_GUILD_OUTPOST						0x13	// F->I, // 2008-05-20 by dhjin, EP3 - 여단 수정 사항 - 전진기지 관련

// IC_GUILD
#define T1_IC_GUILD_CREATE					0x00	// C->I, 길드 생성 요청
#define T1_IC_GUILD_CREATE_OK				0x01	// I->C, 길드 생성 결과
#define T1_IC_GUILD_GET_GUILD_INFO			0x02	// C->I, 길드 정보 요청
#define T1_IC_GUILD_GET_GUILD_INFO_OK		0x03	// I->C, 길드 정보 결과
#define T1_IC_GUILD_GET_OTHER_GUILD_INFO	0x04	// C->I, 다른 길드 정보 요청
#define T1_IC_GUILD_GET_OTHER_GUILD_INFO_OK	0x05	// I->C, 다른 길드 정보 결과
#define T1_IC_GUILD_REQUEST_INVITE			0x06	// C->I, 가입 권유, 서버에 요청
#define T1_IC_GUILD_REQUEST_INVITE_QUESTION	0x07	// I->C, 가입 권유, 대상자에게 전송
#define T1_IC_GUILD_ACCEPT_INVITE			0x08	// C->I, 가입 승낙, 서버에 요청
#define T1_IC_GUILD_ACCEPT_INVITE_OK		0x09	// I->C, 가입 승낙, 길드원에게 전송
#define T1_IC_GUILD_REJECT_INVITE			0x0A	// C->I, 가입 거부, 서버에 요청
#define T1_IC_GUILD_REJECT_INVITE_OK		0x0B	// I->C, 가입 거부, 대상자에게 전송
#define T1_IC_GUILD_BAN_MEMBER				0x0C	// C->I, 길드원 추방
#define T1_IC_GUILD_BAN_MEMBER_OK			0x0D	// I->C, 길드원 추방 결과
#define T1_IC_GUILD_LEAVE					0x0E	// C->I, 여단 탈퇴
#define T1_IC_GUILD_LEAVE_OK				0x0F	// I->C, 여단 탈퇴 결과
// #define T1_IC_GUILD_DISMEMBER				0x10	// C->I, 여단 해체
#define T1_IC_GUILD_DISMEMBER_OK			0x11	// I->C, 여단 해체 결과
#define T1_IC_GUILD_SET_MEMBER_STATE		0x12	// I->C, 길드원의 상태 변화(ONLINE, OFFLINE 등)
#define T1_IC_GUILD_CANCEL_DISMEMBER		0x13	// C->I, 여단 해체 취소 요청
#define T1_IC_GUILD_CANCEL_DISMEMBER_OK		0x14	// I->C, 여단 해체 취소 결과
#define T1_IC_GUILD_GET_DISMEMBER_DATE		0x15	// C->I, 길드 해산 시간 요청
#define T1_IC_GUILD_GET_DISMEMBER_DATE_OK	0x16	// I->C, 길드 해산 시간 결과
#define T1_IC_GUILD_CHANGE_GUILD_NAME		0x17	// C->I, 여단 이름 변경 요청
#define T1_IC_GUILD_CHANGE_GUILD_NAME_OK	0x18	// I->C, 여단 이름 변경 결과
#define T1_IC_GUILD_GET_GUILD_MARK			0x19	// C->I, 여단 문양 요청
#define T1_IC_GUILD_GET_GUILD_MARK_OK		0x1A	// I->C, 여단 문양 결과
#define T1_IC_GUILD_SET_GUILD_MARK			0x1B	// C->I, 여단 문양 성정 요청
#define T1_IC_GUILD_SET_GUILD_MARK_OK		0x1C	// I->C, 여단 문양 성정 결과
#define T1_IC_GUILD_SET_RANK				0x1D	// C->I, 계급 설정
#define T1_IC_GUILD_SET_RANK_OK				0x1E	// I->C, 계급 설정 결과
#define T1_IC_GUILD_CHANGE_GUILD_STATE		0x1F	// I->C, 여단 상태 전송
#define T1_IC_GUILD_LOADING_GUILD_DONE		0x20	// I->C, 서버측에서 길드 정보 로딩 완료 알림
#define T1_IC_GUILD_WAR_READY				0x21	// I->C
#define T1_IC_GUILD_START_WAR				0x22	// I->C
#define T1_IC_GUILD_END_WAR					0x23	// I->C
#define T1_IC_GUILD_UPDATE_WAR_POINT		0x24	// I->C
#define T1_IC_GUILD_SURRENDER_GUILD_WAR		0x25	// C->I, 길드전 항복
#define T1_IC_GUILD_CHANGE_MEMBER_CAPACITY	0x26	// I->C, 여단 제한 인원 변경
#define T1_IC_GUILD_GET_GUILD_MEMBER_LIST_OK	0x27	// I->C, 여단원 리스트
#define T1_IC_GUILD_END_WAR_ADMIN_NOTIFY		0x28	// I->C(n), 2006-08-09 by cmkwon
#define T1_IC_GUILD_MEMBER_LEVEL_UP			0x29	// I->C(n), // 2008-05-20 by dhjin, EP3 - 여단 수정 사항 - 여단원 레벨업 관련
#define T1_IC_GUILD_NEW_COMMANDER			0x2A	// C->I, // 2008-05-20 by dhjin, EP3 - 여단 수정 사항 - 여단장 위임
#define T1_IC_GUILD_NOTICE_WRITE			0x2B	// C->I, // 2008-05-20 by dhjin, EP3 - 여단 수정 사항 - 여단 공지 작성
#define T1_IC_GUILD_NOTICE_WRITE_OK			0x2C	// I->C, // 2008-05-20 by dhjin, EP3 - 여단 수정 사항 - 여단 공지 작성 OK
#define T1_IC_GUILD_GET_APPLICANT			0x2D	// C->I, // 2008-05-27 by dhjin, EP3 - 여단 수정 사항 - 여단 지원자 관리
#define T1_IC_GUILD_GET_APPLICANT_OK_HEADER	0x2E	// I->C, // 2008-05-27 by dhjin, EP3 - 여단 수정 사항 - 여단 지원자 관리 OK
#define T1_IC_GUILD_GET_APPLICANT_OK		0x2F	// I->C, // 2008-05-27 by dhjin, EP3 - 여단 수정 사항 - 여단 지원자 관리 OK
#define T1_IC_GUILD_GET_APPLICANT_OK_DONE	0x30	// I->C, // 2008-05-27 by dhjin, EP3 - 여단 수정 사항 - 여단 지원자 관리 OK
#define T1_IC_GUILD_GET_INTRODUCTION		0x31	// C->I, // 2008-05-27 by dhjin, EP3 - 여단 수정 사항 - 여단 소개
#define T1_IC_GUILD_GET_INTRODUCTION_OK		0x32	// I->C, // 2008-05-27 by dhjin, EP3 - 여단 수정 사항 - 여단 소개 ok
#define T1_IC_GUILD_GET_SELF_INTRODUCTION		0x33	// C->I, // 2008-05-27 by dhjin, EP3 - 여단 수정 사항 - 여단 지원자 소개서 
#define T1_IC_GUILD_GET_SELF_INTRODUCTION_OK	0x34	// I->C, // 2008-05-27 by dhjin, EP3 - 여단 수정 사항 - 여단 지원자 소개서 OK
#define T1_IC_GUILD_SEARCH_INTRODUCTION			0x35		// C->I, // 2008-05-27 by dhjin, EP3 - 여단 수정 사항 - 여단 소개 검색  
#define T1_IC_GUILD_SEARCH_INTRODUCTION_OK_HEADER	0x36	// I->C, // 2008-05-27 by dhjin, EP3 - 여단 수정 사항 - 여단 소개 검색 OK 
#define T1_IC_GUILD_SEARCH_INTRODUCTION_OK			0x37	// I->C, // 2008-05-27 by dhjin, EP3 - 여단 수정 사항 - 여단 소개 검색 OK 
#define T1_IC_GUILD_SEARCH_INTRODUCTION_OK_DONE		0x38	// I->C, // 2008-05-27 by dhjin, EP3 - 여단 수정 사항 - 여단 소개 검색 OK 
#define T1_IC_GUILD_UPDATE_INTRODUCTION				0x39	// C->I, // 2008-05-27 by dhjin, EP3 - 여단 수정 사항 - 여단 소개 작성 
#define T1_IC_GUILD_UPDATE_INTRODUCTION_OK			0x3A	// I->C, // 2008-05-27 by dhjin, EP3 - 여단 수정 사항 - 여단 소개 작성 OK
#define T1_IC_GUILD_DELETE_INTRODUCTION				0x3B	// C->I, // 2008-05-27 by dhjin, EP3 - 여단 수정 사항 - 여단 소개 지우기  
#define T1_IC_GUILD_DELETE_INTRODUCTION_OK			0x3C	// I->C, // 2008-05-27 by dhjin, EP3 - 여단 수정 사항 - 여단 소개 지우기 OK
#define T1_IC_GUILD_UPDATE_SELFINTRODUCTION			0x3D	// C->I, // 2008-05-27 by dhjin, EP3 - 여단 수정 사항 - 자기 소개 작성 
#define T1_IC_GUILD_UPDATE_SELFINTRODUCTION_OK		0x3E	// I->C, // 2008-05-27 by dhjin, EP3 - 여단 수정 사항 - 자기 소개 작성 OK
#define T1_IC_GUILD_DELETE_SELFINTRODUCTION			0x3F	// C->I, // 2008-05-27 by dhjin, EP3 - 여단 수정 사항 - 자기 소개 지우기  
#define T1_IC_GUILD_DELETE_SELFINTRODUCTION_OK		0x40	// I->C, // 2008-05-27 by dhjin, EP3 - 여단 수정 사항 - 자기 소개 지우기 OK
#define T1_IC_GUILD_CHANGE_FAME_RANK				0x41	// I->C, // 2008-06-10 by dhjin, EP3 - 여단 수정 사항 - 여단 명성 변경
#define T1_IC_GUILD_APPLICANT_INVITE				0x42	// C->I, // 2008-06-12 by dhjin, EP3 - 여단 수정 사항 - 지원자 관리에서 여단원 초대
#define T1_IC_GUILD_APPLICANT_INVITE_OK				0x43	// I->C, // 2008-06-12 by dhjin, EP3 - 여단 수정 사항 - 지원자 관리에서 여단원 초대 OK
#define T1_IC_GUILD_APPLICANT_REJECT_INVITE			0x44	// C->I, // 2008-06-12 by dhjin, EP3 - 여단 수정 사항 - 지원자 관리에서 여단원 초대 거부 
#define T1_IC_GUILD_APPLICANT_REJECT_INVITE_OK		0x45	// I->C, // 2008-06-12 by dhjin, EP3 - 여단 수정 사항 - 지원자 관리에서 여단원 초대 거부 OK
#define T1_IC_GUILD_CHANGE_MEMBERSHIP				0x46	// I->C, // 2008-06-20 by dhjin, EP3 - 여단 수정 사항 - 여단장 맴버쉽 정보 전송

// FC_SKILL
#define T1_FC_SKILL_USE_SKILLPOINT			0x00
#define T1_FC_SKILL_USE_SKILLPOINT_OK		0x01
#define T1_FC_SKILL_SETUP_SKILL				0x02
#define T1_FC_SKILL_SETUP_SKILL_OK_HEADER	0x03
#define T1_FC_SKILL_SETUP_SKILL_OK			0x04
#define T1_FC_SKILL_SETUP_SKILL_OK_DONE		0x05
#define T1_FC_SKILL_USE_SKILL				0x06
#define T1_FC_SKILL_USE_SKILL_OK			0x07
#define T1_FC_SKILL_CANCEL_SKILL			0x08
#define T1_FC_SKILL_INVALIDATE_SKILL		0x09
#define T1_FC_SKILL_PREPARE_USE				0x0A
#define T1_FC_SKILL_PREPARE_USE_OK			0x0B
#define T1_FC_SKILL_CANCEL_PREPARE			0x0C
#define T1_FC_SKILL_CANCEL_PREPARE_OK		0x0D
#define T1_FC_SKILL_CONFIRM_USE				0x0F		// 2005-12-02 by cmkwon, C->F, F->C
#define T1_FC_SKILL_CONFIRM_USE_ACK			0x10		// 2005-12-02 by cmkwon, C->F, F->C
#define T1_FC_SKILL_CANCEL_SKILL_OK			0x11		// 2006-11-28 by dhjin, F->C

// FN_SKILL
#define T1_FN_SKILL_USE_SKILL				0x00
#define T1_FN_SKILL_USE_SKILL_OK			0x01

// FC_QUEST
#define T1_FC_QUEST_REQUEST_START					0x00
#define T1_FC_QUEST_REQUEST_START_RESULT			0x01
#define T1_FC_QUEST_ACCEPT_QUEST					0x02
#define T1_FC_QUEST_CANCEL_QUEST					0x03
#define T1_FC_QUEST_REQUEST_SUCCESS					0x04
#define T1_FC_QUEST_REQUEST_SUCCESS_RESULT			0x05
#define T1_FC_QUEST_PUT_ALL_QUEST_HEADER			0x06
#define T1_FC_QUEST_PUT_ALL_QUEST					0x07
#define T1_FC_QUEST_PUT_ALL_QUEST_DONE				0x08
#define T1_FC_QUEST_DISCARD_QUEST					0x09
#define T1_FC_QUEST_DISCARD_QUEST_OK				0x0A
#define T1_FC_QUEST_MOVE_QUEST_MAP					0x0B	// C->F
#define T1_FC_QUEST_PUT_ALL_QUEST_MONSTER_COUNT_HEADER		0x0C	// F->C, // 2005-10-25 by cmkwon
#define T1_FC_QUEST_PUT_ALL_QUEST_MONSTER_COUNT		0x0D	// F->C, // 2005-10-25 by cmkwon
#define T1_FC_QUEST_PUT_ALL_QUEST_MONSTER_COUNT_DONE		0x0E	// F->C, // 2005-10-25 by cmkwon
#define T1_FC_QUEST_UPDATE_MONSTER_COUNT			0x0F	// F->C, // 2005-10-25 by cmkwon
#define T1_FC_QUEST_REQUEST_SUCCESS_CHECK			0x10	// C->F, // 2006-03-24 by cmkwon
#define T1_FC_QUEST_REQUEST_SUCCESS_CHECK_RESULT	0x11	// F->C(n), // 2006-03-24 by cmkwon
#define T1_FC_QUEST_REQUEST_PARTY_WARP				0x12	// F->C(n), // 2006-10-16 by cmkwon
#define T1_FC_QUEST_REQUEST_PARTY_WARP_ACK			0x13	// C(n)->F, // 2006-10-16 by cmkwon
#define T1_FC_QUEST_REQUEST_MISSIONMASTER_HELP		0x14	// 2008-12-02 by dhjin, C -> F, 미션마스터 요청
#define T1_FC_QUEST_REQUEST_MISSIONMASTER_HELP_INVITE 0x15	// 2008-12-02 by dhjin, F -> C(n), 랜덤으로 뽑힌 미션마스터에게 요청
#define T1_FC_QUEST_MISSIONMASTER_HELP_INVITE		0x16	// 2008-12-02 by dhjin, C -> F, 미션마스터 요청 승락 
#define T1_FC_QUEST_MISSIONMASTER_HELP_INVITE_OK	0x17	// 2008-12-02 by dhjin, F -> C, 미션마스터 요청 승락
#define T1_FC_QUEST_MISSIONMASTER_HELP_REJECT		0x18	// 2008-12-02 by dhjin, C -> F, 미션마스터 요청 거절 
#define T1_FC_QUEST_MISSIONMASTER_HELP_REJECT_OK	0x19	// 2008-12-02 by dhjin, F -> C, 미션마스터 요청 거절
#define T1_FC_QUEST_INSERT_QUEST					0x20	// 2009-03-31 by cmkwon, 세력초기화 시스템 구현 - F->C
#define T1_FC_QUEST_DELETE_QUEST					0x21	// 2009-03-31 by cmkwon, 세력초기화 시스템 구현 - F->C


// FC_SYNC
//#define T1_FC_SYNC_PRIMARY_REATTACK_OK		0x00
//#define T1_FC_SYNC_SECONDARY_REATTACK_OK	0x01
//#define T1_FC_SYNC_SKILL_REUSE_OK			0x02

// FC_INFO
#define T1_FC_INFO_GET_MONSTER_INFO					0x00
#define T1_FC_INFO_GET_MONSTER_INFO_OK				0x01
#define T1_FC_INFO_GET_MAPOBJECT_INFO				0x02
#define T1_FC_INFO_GET_MAPOBJECT_INFO_OK			0x03
#define T1_FC_INFO_GET_ITEM_INFO					0x04
#define T1_FC_INFO_GET_ITEM_INFO_OK					0x05
#define T1_FC_INFO_GET_RARE_ITEM_INFO				0x06
#define T1_FC_INFO_GET_RARE_ITEM_INFO_OK			0x07
#define T1_FC_INFO_GET_BUILDINGNPC_INFO				0x08
#define T1_FC_INFO_GET_BUILDINGNPC_INFO_OK			0x09
#define T1_FC_INFO_GET_SIMPLE_ITEM_INFO				0x0A
#define T1_FC_INFO_GET_SIMPLE_ITEM_INFO_OK			0x0B
#define T1_FC_INFO_GET_ENCHANT_COST					0x0C
#define T1_FC_INFO_GET_ENCHANT_COST_OK				0x0D
#define T1_FC_INFO_GET_CURRENT_MAP_INFO				0x10
#define T1_FC_INFO_GET_CURRENT_MAP_INFO_OK			0x11
#define T1_FC_INFO_GET_HAPPY_HOUR_EVENT_INFO_OK		0x12
#define T1_FC_INFO_GET_GAME_EVENT_INFO_OK			0x13
#define T1_FC_INFO_GET_SERVER_DATE_TIME				0x14		// 2006-10-11 by cmkwon, C->F
#define T1_FC_INFO_GET_SERVER_DATE_TIME_OK			0x15		// 2006-10-11 by cmkwon, F->C
#define T1_FC_INFO_GET_HAPPY_HOUR_EVENT_INFO		0x16		// 2007-10-30 by cmkwon, 세력별 해피아워 이벤트 구현 - C->F
#define T1_FC_INFO_GET_HAPPY_HOUR_EVENT_INFO_BY_LEVEL	0x17		// 2008-03-14 by dhjin, Level별 해피아워 이벤트 구현 - F->C

#define T1_FC_INFO_CHECK_RESOBJ_CHECKSUM			0x20		// 2007-05-28 by cmkwon, C->F
#define T1_FC_INFO_NOTICE_REQUEST					0x30		// C->F
#define T1_FC_INFO_NOTICE_REQUEST_OK				0x31		// F->C
#define T1_FC_INFO_NOTICE_REG						0x32		// C->F
#define T1_FC_INFO_NOTICE_REG_OK					0x33		// F->C
#define T1_FC_INFO_NOTICE_MODIFY					0x34		// C->F
#define T1_FC_INFO_NOTICE_MODIFY_OK					0x35		// F->C
#define T1_FC_INFO_EXPEDIENCYFUND_REQUEST			0x36		// C->F
#define T1_FC_INFO_EXPEDIENCYFUND_REQUEST_OK		0x37		// F->C
#define T1_FC_INFO_EXPEDIENCYFUND_PAYBACK			0x38		// C->F
#define T1_FC_INFO_EXPEDIENCYFUND_PAYBACK_OK		0x39		// F->C
#define T1_FC_INFO_SECONDARYPASSWORD_UPDATE_PASSWORD	0x40	// 2007-09-12 by cmkwon, 베트남 2차패스워드 구현 - 프로토콜 추가, C->F
#define T1_FC_INFO_SECONDARYPASSWORD_UPDATE_PASSWORD_OK	0x41	// 2007-09-12 by cmkwon, 베트남 2차패스워드 구현 - 프로토콜 추가, F->C
#define T1_FC_INFO_SECONDARYPASSWORD_CHECK_PASSWORD		0x42	// 2007-09-12 by cmkwon, 베트남 2차패스워드 구현 - 프로토콜 추가, C->F
#define T1_FC_INFO_SECONDARYPASSWORD_CHECK_PASSWORD_OK	0x43	// 2007-09-12 by cmkwon, 베트남 2차패스워드 구현 - 프로토콜 추가, F->C
#define T1_FC_INFO_SECONDARYPASSWORD_LOCK				0x44	// 2007-09-12 by cmkwon, 베트남 2차패스워드 구현 - 프로토콜 추가, C->F
#define T1_FC_INFO_SECONDARYPASSWORD_LOCK_OK			0x45	// 2007-09-12 by cmkwon, 베트남 2차패스워드 구현 - 프로토콜 추가, F->C
#define T1_FC_INFO_GET_GUILDMARK						0x46	// 2007-12-07 by dhjin, 여단 마크 C->F
#define T1_FC_INFO_GET_GUILDMARK_OK						0x47	// 2007-12-07 by dhjin, 여단 마크 F->C
#define T1_FC_INFO_MSWARINFO_DISPLAY					0x48	// 2008-03-27 by dhjin, 모선전 정보 표시 기획안 - 모선전 정보 전송 F->C
#define T1_FC_INFO_MSWARINFO_DISPLAY_OPTION				0x49	// 2008-03-27 by dhjin, 모선전 정보 표시 기획안 - 모선전 정보 옵션 전송 C->F
#define T1_FC_INFO_MSWARINFO_DISPLAY_OPTION_OK			0x4A	// 2008-03-27 by dhjin, 모선전 정보 표시 기획안 - 모선전 정보 옵션 전송 F->C
#define T1_FC_INFO_MSWARINFO_RESULT						0x4B	// 2008-04-02 by dhjin, 모선전, 거점전 정보창 기획안 - 모선전 결과 정보 C->F
#define T1_FC_INFO_MSWARINFO_RESULT_OK					0x4C	// 2008-04-02 by dhjin, 모선전, 거점전 정보창 기획안 - 모선전 결과 정보 F->C
#define T1_FC_INFO_SPWARINFO_RESULT						0x4D	// 2008-04-02 by dhjin, 모선전, 거점전 정보창 기획안 - 거점전 결과 정보 C->F
#define T1_FC_INFO_SPWARINFO_RESULT_OK_HEADER			0x4E	// 2008-04-02 by dhjin, 모선전, 거점전 정보창 기획안 - 거점전 결과 정보 F->C
#define T1_FC_INFO_SPWARINFO_RESULT_OK					0x4F	// 2008-04-02 by dhjin, 모선전, 거점전 정보창 기획안 - 거점전 결과 정보 F->C
#define T1_FC_INFO_SPWARINFO_RESULT_OK_DONE				0x50	// 2008-04-02 by dhjin, 모선전, 거점전 정보창 기획안 - 거점전 결과 정보 F->C
#define T1_FC_INFO_DECLARATION_MSWAR_INFO				0x51	// 2009-01-12 by dhjin, 선전 포고 - 선전포고 정보 요청 C->F
#define T1_FC_INFO_DECLARATION_MSWAR_INFO_OK			0x52	// 2009-01-12 by dhjin, 선전 포고 - 선전포고 정보 전송 F->C
#define T1_FC_INFO_DECLARATION_MSWAR_SET				0x53	// 2009-01-12 by dhjin, 선전 포고 - 선전포고 시간 및 포기 설정 C->F
#define T1_FC_INFO_STRATEGICPOINTINFO_DISPLAY_LIST		0x54	//19-04-2016 by Inetpub
#define T1_FC_INFO_STRATEGICPOINTINFO_DISPLAY			0x55	//19-04-2016 by Inetpub

#define T1_FC_INFO_WRK_GET_SERVICE_INFO					0x60	// 2009-02-12 by cmkwon, EP3-3 월드랭킹시스템 구현 - C->F
#define T1_FC_INFO_WRK_GET_SERVICE_INFO_OK				0x61	// 2009-02-12 by cmkwon, EP3-3 월드랭킹시스템 구현 - F->C
#define T1_FC_INFO_WRK_GET_SERVICE_INFO_OK_IMAGE		0x62	// 2009-02-12 by cmkwon, EP3-3 월드랭킹시스템 구현 - F->C
#define T1_FC_INFO_WRK_GET_SERVICE_INFO_OK_DONE			0x63	// 2009-02-12 by cmkwon, EP3-3 월드랭킹시스템 구현 - F->C
#define T1_FC_INFO_WRK_GET_RANKER_LIST					0x64	// 2009-02-12 by cmkwon, EP3-3 월드랭킹시스템 구현 - C->F
#define T1_FC_INFO_WRK_GET_LEVEL_RANKER_LIST_OK			0x65	// 2009-02-12 by cmkwon, EP3-3 월드랭킹시스템 구현 - F->C
#define T1_FC_INFO_WRK_GET_FAME_RANKER_LIST_OK			0x66	// 2009-02-12 by cmkwon, EP3-3 월드랭킹시스템 구현 - F->C
#define T1_FC_INFO_WRK_GET_PVP_RANKER_LIST_OK			0x67	// 2009-02-12 by cmkwon, EP3-3 월드랭킹시스템 구현 - F->C
#define T1_FC_INFO_WRK_GET_SELF_RANKING					0x68	// 2009-02-12 by cmkwon, EP3-3 월드랭킹시스템 구현 - C->F
#define T1_FC_INFO_WRK_GET_SELF_RANKING_OK				0x69	// 2009-02-12 by cmkwon, EP3-3 월드랭킹시스템 구현 - F->C
#define T1_FC_INFO_APPLY_RESISTANCE_ITEM				0x70	// 2009-09-09 ~ 2010 by dhjin, 인피니티 - 저항 아이템 적용 정보 전송, F -> C(n)
#define T1_FC_INFO_APPLY_DESTPARAM						0x71	// 2009-09-09 ~ 2010-02-10 by dhjin, 인피니티 - 발동류장착아이템, F -> C(n)
#define T1_FC_INFO_APPLY_DESTPARAM_LIST					0x72	// 2009-09-09 ~ 2010-02-10 by dhjin, 인피니티 - 발동류장착아이템, F -> C(n)
#define T1_FC_INFO_SP_HP_SILVER							0x73	// 2015-08-11 by silver
#define T1_FC_INFO_SP_AT_LOGIN_SILVER					0x74	// 2015-08-14 by silver
#ifdef _INET_ANTICHEAT
#define T1_FC_INET_GET_ITEM_INFO						0x75
#define T1_FC_INET_GET_ITEM_INFO_OK						0x76
#endif
// FC_REQUEST - 캐릭터간의 요청, 수락, 거절 등에 쓰임, general-purpose
#define T1_FC_REQUEST_REQUEST				0x00
#define T1_FC_REQUEST_REQUEST_OK			0x01
#define T1_FC_REQUEST_ACCEPT_REQUEST		0x02
#define T1_FC_REQUEST_ACCEPT_REQUEST_OK		0x03
#define T1_FC_REQUEST_REJECT_REQUEST		0x04
#define T1_FC_REQUEST_REJECT_REQUEST_OK		0x05
#define T1_FC_REQUEST_CANCEL_REQUEST		0x06


//////////////////////////////////////////////////////////////////////////
// FC_CITY
#define T1_FC_CITY_GET_BUILDING_LIST						0x00
#define T1_FC_CITY_GET_BUILDING_LIST_OK						0x01
#define T1_FC_CITY_GET_WARP_TARGET_MAP_LIST					0x02
#define T1_FC_CITY_GET_WARP_TARGET_MAP_LIST_OK				0x03
#define T1_FC_CITY_REQUEST_ENTER_BUILDING					0x04	// C->F, 상점 들어갈 것을 요청
#define T1_FC_CITY_REQUEST_ENTER_BUILDING_OK				0x05	// F->C, 상점 진입 완료를 알림
#define T1_FC_CITY_REQUEST_WARP								0x06	// C->F, 도시에서 워프해서 나가기 위한 요청
#define T1_FC_CITY_CHECK_WARP_STATE							0x07
#define T1_FC_CITY_CHECK_WARP_STATE_OK						0x08

#define T1_FC_CITY_POLL_REQUEST_LEADER_CANDIDATE_LIST			0x10	// C->F, 지도자 후보 리스트 요청
#define T1_FC_CITY_POLL_REQUEST_LEADER_CANDIDATE_LIST_OK_HEADER	0x11	// F->C, 지도자 후보 리스트 요청
#define T1_FC_CITY_POLL_REQUEST_LEADER_CANDIDATE_LIST_OK		0x12	// F->C, 지도자 후보 리스트 요청
#define T1_FC_CITY_POLL_REQUEST_LEADER_CANDIDATE_LIST_OK_DONE	0x13	// F->C, 지도자 후보 리스트 요청
#define T1_FC_CITY_POLL_REQUEST_LEADER_CANDIDATE_INFO			0x14	// C->F, 지도자 후보 정보 요청
#define T1_FC_CITY_POLL_REQUEST_LEADER_CANDIDATE_INFO_OK		0x15	// F->C, 지도자 후보 정보 요청
#define T1_FC_CITY_POLL_REQUEST_LEADER_CANDIDATE_INFO_OK_GUILDMARK		0x16	// F->C, 지도자 후보 정보 요청 여단 마크
#define T1_FC_CITY_POLL_REG_LEADER_CANDIDATE					0x17	// C->F, 지도자 후보 등록
#define T1_FC_CITY_POLL_REG_LEADER_CANDIDATE_OK					0x18	// F->C, 지도자 후보 등록
#define T1_FC_CITY_POLL_DELETE_LEADER_CANDIDATE					0x19	// C->F, 지도자 후보 탈퇴
#define T1_FC_CITY_POLL_DELETE_LEADER_CANDIDATE_OK				0x1A	// F->C, 지도자 후보 탈퇴
#define T1_FC_CITY_POLL_VOTE									0x1B	// C->F, 지도자 후보에게 투표
#define T1_FC_CITY_POLL_VOTE_OK									0x1C	// F->C, 지도자 후보에게 투표
#define T1_FC_CITY_POLL_REQUEST_POLL_DATE						0x1D	// C->F, 선거 기간 요청
#define T1_FC_CITY_POLL_REQUEST_POLL_DATE_OK					0x1E	// F->C, 선거 기간 요청
#define T1_FC_CITY_POLL_LEADER_ELECTION_INFO					0x1F	// F->C, 선거 결과 전송

#define T1_FC_CITY_WARINFO_INFLUENCE							0x20	// C->F, 세력 정보
#define T1_FC_CITY_WARINFO_INFLUENCE_OK							0x21	// F->C, 세력 정보
#define T1_FC_CITY_WARINFO_OUTPOST								0x22	// C->F, 전진기지 정보
#define T1_FC_CITY_WARINFO_OUTPOST_OK							0x23	// F->C, 전진기지 정보 



//////////////////////////////////////////////////////////////////////////
// FC_TIMER
#define T1_FC_TIMER_START_TIMER					0x00	// F->C, TIMER_EVENT 시작
#define T1_FC_TIMER_STOP_TIMER					0x01	// F->C, TIMER_EVENT 정지
#define T1_FC_TIMER_UPDATE_TIMER				0x02	// F->C, TIMER_EVENT 갱신(시간 연장)
#define T1_FC_TIMER_PAUSE_TIMER					0x03	// F->C, TIMER_EVENT 일시 정지
#define T1_FC_TIMER_CONTINUE_TIMER				0x04	// F->C, TIMER_EVENT 재시작
#define T1_FC_TIMER_TIMEOUT						0x05	// C->F, 시간이 다 됨을 알림

// Debug String
#define T1_FC_CLIENT_REPORT					0x00
#define T1_IC_STRING_128					0x01
#define T1_IC_STRING_256					0x02
#define T1_IC_STRING_512					0x03

#define T1_FC_STRING_128					0x01
#define T1_FC_STRING_256					0x02
#define T1_FC_STRING_512					0x03

// FI_ADMIN
#define T1_FI_ADMIN_GET_CHARACTER_INFO		0x00
#define T1_FI_ADMIN_GET_CHARACTER_INFO_OK	0x01
#define T1_FI_ADMIN_CALL_CHARACTER			0x02	// I -> F
#define T1_FI_ADMIN_MOVETO_CHARACTER		0x03	// I -> F
#define T1_FI_ADMIN_PRINT_DEBUG_MSG			0x04	// I -> F
#define T1_FI_ADMIN_CHANGE_WEATHER			0x05	// I -> F

// IC_ADMIN
#define T1_IC_ADMIN_CALL_CHARACTER			0x01	// C -> I
#define T1_IC_ADMIN_GET_SERVER_STAT			0x02	// C -> I, NO BODY
#define T1_IC_ADMIN_GET_SERVER_STAT_OK		0x03	// I -> C
#define T1_IC_ADMIN_CALL_GUILD				0x04	// C -> I
#define T1_IC_ADMIN_CALLGM_INFO_OK			0x10	// C -> I, // 2006-05-08 by cmkwon
#define T1_IC_ADMIN_CALLGM_VIEW_OK			0x11	// C -> I, // 2006-05-08 by cmkwon
#define T1_IC_ADMIN_CALLGM_BRING_OK			0x12	// C -> I, // 2006-05-08 by cmkwon
#define T1_IC_ADMIN_COMMAND_EVO_WHO_OK		0x13
// FC_ADMIN
#define T1_FC_ADMIN_GET_SERVER_STAT			0x00	// C -> F, NO BODY
#define T1_FC_ADMIN_GET_SERVER_STAT_OK		0x01	// F -> C

// T0_IC_COUNTDOWN
#define T1_IC_COUNTDOWN_START				0x00	// I -> C
#define T1_IC_COUNTDOWN_DONE				0x01	// C -> I

// T0_IC_VOIP
// 2008-06-17 by dhjin, EP3 VOIP -
#define T1_IC_VOIP_SET						0x00		// C -> I, 2008-06-17 by dhjin, EP3 VOIP - 설정 
#define T1_IC_VOIP_SET_OK					0x01		// I -> C, 2008-06-17 by dhjin, EP3 VOIP - 설정 정보 전송

//////////////////////////////////////////////////////////////////////////
// T0_IC_CHATROOM
#define T1_IC_CHATROOM_CREATE					0x00	// C -> I, 2008-06-16 by dhjin, EP3 채팅방 - 채팅방 생성
#define T1_IC_CHATROOM_CREATE_OK				0x01	// I -> C, 2008-06-16 by dhjin, EP3 채팅방 - 채팅방 생성 OK
#define T1_IC_CHATROOM_LIST_INFO				0x02	// C -> I, 2008-06-16 by dhjin, EP3 채팅방 - 채팅방 목록 가져오기
#define T1_IC_CHATROOM_LIST_INFO_OK				0x03	// I -> C, 2008-06-16 by dhjin, EP3 채팅방 - 채팅방 목록 가져오기 OK
#define T1_IC_CHATROOM_REQUEST_INVITE			0x04	// C -> I, 2008-06-16 by dhjin, EP3 채팅방 - 채팅방 초대 요청 
#define T1_IC_CHATROOM_REQUEST_INVITE_QUESTION	0x05	// I -> C, 2008-06-16 by dhjin, EP3 채팅방 - 채팅방 초대 대상에게 전송
#define T1_IC_CHATROOM_JOIN						0x06	// C -> I, 2008-06-16 by dhjin, EP3 채팅방 - 채팅방 참여
#define T1_IC_CHATROOM_JOIN_OK					0x07	// I -> C, 2008-06-16 by dhjin, EP3 채팅방 - 채팅방 참여 OK
#define T1_IC_CHATROOM_ACCEPT_INVITE			0x08	// C -> I, 2008-06-16 by dhjin, EP3 채팅방 - 채팅방 초대 수락
#define T1_IC_CHATROOM_ACCEPT_INVITE_OK			0x09	// I -> C, 2008-06-16 by dhjin, EP3 채팅방 - 채팅방 초대 수락 OK
#define T1_IC_CHATROOM_REJECT_INVITE			0x0A	// C -> I, 2008-06-16 by dhjin, EP3 채팅방 - 채팅방 초대 거절
#define T1_IC_CHATROOM_REJECT_INVITE_OK			0x0B	// I -> C, 2008-06-16 by dhjin, EP3 채팅방 - 채팅방 초대 거절 OK
#define T1_IC_CHATROOM_LEAVE					0x0C	// C -> I, 2008-06-16 by dhjin, EP3 채팅방 - 채팅방 나가기
#define T1_IC_CHATROOM_LEAVE_OK					0x0D	// I -> C, 2008-06-16 by dhjin, EP3 채팅방 - 채팅방 나가기 OK
#define T1_IC_CHATROOM_BAN						0x0E	// C -> I, 2008-06-16 by dhjin, EP3 채팅방 - 채팅방 추방
#define T1_IC_CHATROOM_BAN_OK					0x0F	// I -> C, 2008-06-16 by dhjin, EP3 채팅방 - 채팅방 추방 OK
#define T1_IC_CHATROOM_CHANGE_NAME				0x10	// C -> I, 2008-06-16 by dhjin, EP3 채팅방 - 채팅방 이름 변경
#define T1_IC_CHATROOM_CHANGE_NAME_OK			0x11	// I -> C, 2008-06-16 by dhjin, EP3 채팅방 - 채팅방 이름 변경 OK
#define T1_IC_CHATROOM_CHANGE_MASTER			0x12	// C -> I, 2008-06-16 by dhjin, EP3 채팅방 - 채팅방 방장 변경
#define T1_IC_CHATROOM_CHANGE_MASTER_OK			0x13	// I -> C, 2008-06-16 by dhjin, EP3 채팅방 - 채팅방 방장 변경 OK
#define T1_IC_CHATROOM_CHANGE_LOCK_PW			0x14	// C -> I, 2008-06-16 by dhjin, EP3 채팅방 - 채팅방 비밀번호 변경
#define T1_IC_CHATROOM_CHANGE_LOCK_PW_OK		0x15	// I -> C, 2008-06-16 by dhjin, EP3 채팅방 - 채팅방 비밀번호 변경 OK
#define T1_IC_CHATROOM_CHANGE_MAX_MEMBER		0x16	// C -> I, 2008-06-16 by dhjin, EP3 채팅방 - 채팅방 인원수 변경
#define T1_IC_CHATROOM_CHANGE_MAX_MEMBER_OK		0x17	// I -> C, 2008-06-16 by dhjin, EP3 채팅방 - 채팅방 인원수 변경 OK
#define T1_IC_CHATROOM_MEMBER_INFO				0x18	// C -> I, 2008-06-25 by dhjin, EP3 채팅방 - 채팅방 맴버 정보 전송
#define T1_IC_CHATROOM_MEMBER_INFO_OK			0x19	// I -> C, 2008-06-25 by dhjin, EP3 채팅방 - 채팅방 맴버 정보 전송 OK
#define T1_IC_CHATROOM_OTHER_MEMBER_INFO		0x1A	// C -> I, 2008-06-25 by dhjin, EP3 채팅방 - 채팅방 다른 맴버 정보 전송
#define T1_IC_CHATROOM_OTHER_MEMBER_INFO_OK		0x1B	// I -> C, 2008-06-25 by dhjin, EP3 채팅방 - 채팅방 다른 맴버 정보 전송 OK


// T0_FC_CITYWAR
#define T1_FC_CITYWAR_START_WAR					0x01
#define T1_FC_CITYWAR_MONSTER_CREATED			0x02
#define T1_FC_CITYWAR_MONSTER_DEAD				0x03
#define T1_FC_CITYWAR_END_WAR					0x04
#define T1_FC_CITYWAR_GET_OCCUPYINFO			0x05	// C->F
#define T1_FC_CITYWAR_GET_OCCUPYINFO_OK			0x06	// F->C
#define T1_FC_CITYWAR_SET_SETTINGTIME			0x07	// C->F
#define T1_FC_CITYWAR_SET_TEX					0x08	// C->F
#define T1_FC_CITYWAR_SET_BRIEFING				0x09	// C->F
#define T1_FC_CITYWAR_BRING_SUMOFTEX			0x0A	// C->F
#define T1_FC_CITYWAR_BRING_SUMOFTEX_OK			0x0B	// F->C

// T0_FC_WAR
#define T1_FC_WAR_NOTIFY_INVASION					0x01	// F->C(n)
#define T1_FC_WAR_NOTIFY_INFLUENCE_MONSTER_DEAD		0x02	// F->C(n)
#define T1_FC_WAR_NOTIFY_INFLUENCE_MONSTER_INVASION	0x03	// F->C(n),// 2006-01-20 by cmkwon
#define T1_FC_WAR_NOTIFY_INFLUENCE_MONSTER_AUTO_DESTROYED		0x04	// F->C(n), // 2007-02-14 by dhjin

#define T1_FC_WAR_BOSS_MONSTER_SUMMON_DATA			0x10	// F->C, F->C(n) - 2006-04-14 by cmkwon
#define T1_FC_WAR_JACO_MONSTER_SUMMON				0x11	// F->C(n) // 2006-04-20 by cmkwon
#define T1_FC_WAR_STRATEGYPOINT_MONSTER_SUMMON		0x12	// F->C		// 2007-07-16 by dhjin
#define T1_FC_WAR_SIGN_BOARD_INSERT_STRING			0x20	// F->C(n) - 2006-04-17 by cmkwon
#define T1_FC_WAR_SIGN_BOARD_DELETE_STRING			0x21	// F->C(n) - 2006-04-17 by cmkwon
#define T1_FC_WAR_REQ_SIGN_BOARD_STRING_LIST		0x22	// C->F - 2006-04-17 by cmkwon
#define T1_FC_WAR_REQ_SIGN_BOARD_STRING_LIST_OK		0x23	// F->C - 2006-04-17 by cmkwon
#define T1_FC_WAR_UPDATE_CONTRIBUTION_POINT_OK		0x24	// F->C(n) - 2006-04-19 by cmkwon
#define T1_FC_WAR_INFLUENCE_DATA					0x25	// F->C - 2006-04-21 by cmkwon


#define T1_FC_WAR_MONSTER_CREATED					0x30	// 2006-11-20 by cmkwon, F->C(n)
#define T1_FC_WAR_MONSTER_AUTO_DESTROYED			0x31	// 2006-11-20 by cmkwon, F->C(n)
#define T1_FC_WAR_MONSTER_DEAD						0x32	// 2006-11-20 by cmkwon, F->C(n)
#define T1_FC_WAR_BOSS_CONTRIBUTION_GUILD			0x33	// 2008-12-29 by dhjin, 전쟁 보상 추가안, F->C(n)

///////////////////////////////////////////////////////////////////////////////
// 2006-07-25 by cmkwon
// T0_FC_BAZAAR
#define T1_FC_BAZAAR_CUSTOMER_INFO_OK				0x01	// F->C
#define T1_FC_BAZAAR_INFO_OK						0x02	// F->C(n)
#define T1_FC_BAZAAR_SELL_PUT_ITEM					0x03	// C->F
#define T1_FC_BAZAAR_SELL_PUT_ITEM_OK				0x04	// F->C
#define T1_FC_BAZAAR_SELL_CANCEL_ITEM				0x05	// C->F
#define T1_FC_BAZAAR_SELL_CANCEL_ITEM_OK			0x06	// F->C
#define T1_FC_BAZAAR_SELL_START						0x07	// C->F
#define T1_FC_BAZAAR_SELL_START_OK					0x08	// F->C
#define T1_FC_BAZAAR_SELL_REQUEST_ITEMLIST			0x09	// C->F
#define T1_FC_BAZAAR_SELL_REQUEST_ITEMLIST_OK		0x0A	// F->C
#define T1_FC_BAZAAR_SELL_ITEM_ENCHANTLIST_OK		0x0B	// F->C
#define T1_FC_BAZAAR_SELL_BUY_ITEM					0x0C	// C->F
#define T1_FC_BAZAAR_SELL_BUY_ITEM_OK				0x0D	// F->C(2)

#define T1_FC_BAZAAR_BUY_PUT_ITEM					0x23	// C->F
#define T1_FC_BAZAAR_BUY_PUT_ITEM_OK				0x24	// F->C
#define T1_FC_BAZAAR_BUY_CANCEL_ITEM				0x25	// C->F
#define T1_FC_BAZAAR_BUY_CANCEL_ITEM_OK				0x26	// F->C
#define T1_FC_BAZAAR_BUY_START						0x27	// C->F
#define T1_FC_BAZAAR_BUY_START_OK					0x28	// F->C
#define T1_FC_BAZAAR_BUY_REQUEST_ITEMLIST			0x29	// C->F
#define T1_FC_BAZAAR_BUY_REQUEST_ITEMLIST_OK		0x2A	// F->C
#define T1_FC_BAZAAR_BUY_SELL_ITEM					0x2B	// C->F
#define T1_FC_BAZAAR_BUY_SELL_ITEM_OK				0x2C	// F->C(2)
#ifdef BONUS_STAT_ITEM
#define T1_FC_BAZAAR_SELL_ITEM_BONUSLIST_OK			0x30	// F->C
#endif






// T0_FI_CITYWAR

// T0_FI_CASH
#define T1_FI_CASH_USING_GUILD						0x00
#define T1_FI_CASH_USING_CHANGE_CHARACTERNAME		0x01
#define T1_FI_CASH_PREMIUM_CARD_INFO                0x02
#define T1_FI_GUILD_RANK_OK							0x03
#define T1_FI_GUILD_DELETE_INFO_OK					0x04

// T0_FI_WAR, // 2005-12-27 by cmkwon
#define T1_FI_WAR_ADD_GUILD_FAME					0x00	// F->I

// T0_FI_INFO
#define T1_FI_INFO_DECLARATION_MSWAR_SET_OK			0x00	// F->I		// 2009-01-12 by dhjin, 선전 포고 - 선전포고 시간 및 포기 설정 F->I

// T0_IC_INFO
#define T1_IC_INFO_DECLARATION_MSWAR_SET_OK			0x00	// I->C		// 2009-01-12 by dhjin, 선전 포고 - 선전포고 시간 및 포기 설정 I->C

// T0_IC_CITYWAR

// T0_FN_CITYWAR
#define T1_FN_CITYWAR_START_WAR					0x00	// F->N
#define T1_FN_CITYWAR_END_WAR					0x01	// F->N
#define T1_FN_CITYWAR_CHANGE_OCCUPY_INFO		0x02	// F->N
#define T1_FN_CITYWAR_CHANGE_EVENTMONSTER_PROB  0x03

// SendErrorMessage등에 Type으로 사용하기 위해
#define T1_PRE_IOCP								0x00
#define T1_PRE_DB								0x01

#define T1_IM_IOCP								0x00
#define T1_IM_DB								0x01

#define T1_FIELD_IOCP							0x00
#define T1_FIELD_DB								0x01

#define T1_NPC_IOCP								0x00
#define T1_NPC_DB								0x01

#define T1_TIMER								0x00

#define T1_DB									0x00

// INVALID PROTOCOL
#define T1_NA									0x00

// ERROR
#define T1_ERROR								0x00


// T0_FC_RACING
#define T1_FC_RACING_RACINGLIST_REQUEST			0x00	// No Body
#define T1_FC_RACING_RACINGLIST_REQUEST_ACK		0x01
#define T1_FC_RACING_RACINGINFO_REQUEST			0x02
#define T1_FC_RACING_RACINGINFO_REQUEST_ACK		0x03
#define T1_FC_RACING_RACINGNOTIFY				0x04
#define T1_FC_RACING_JOIN_ENABLE				0x05
#define T1_FC_RACING_JOIN_REQUEST				0x06
#define T1_FC_RACING_JOIN_REQUEST_ACK			0x07
#define T1_FC_RACING_COUNTDOWN					0x08
#define T1_FC_RACING_CHECKPOINT_CHECK			0x09
#define T1_FC_RACING_CHECKPOINT_CHECK_ACK		0x0A
#define T1_FC_RACING_FINISHED					0x0B
#define T1_FC_RACING_OTHER_FINISHED				0x0C
#define T1_FC_RACING_FINALRANKING				0x0D


//////////////////////////////////////////////////////////////////////////
// 2007-04-17 by dhjin
// T0_FC_ARENA
#define T1_FC_ARENA_REQUEST_TEAM				0x00	// C->F
#define T1_FC_ARENA_REQUEST_TEAM_OK				0x01	// F->C
#define T1_FC_ARENA_CREATE_TEAM					0x02	// C->F
#define T1_FC_ARENA_CREATE_TEAM_OK				0x03	// F->C
#define T1_FC_ARENA_ENTER_TEAM					0x04	// C->F
#define T1_FC_ARENA_ENTER_TEAM_OK				0x05	// F->C
#define T1_FC_ARENA_TEAM_MEMBER_LIST			0x06	// F->C(n)
#define T1_FC_ARENA_REAVE_TEAM					0x07	// C->F
#define T1_FC_ARENA_REAVE_TEAM_OK				0x08	// F->C
#define T1_FC_ARENA_TEAM_READY					0x09	// F->C(n)
#define T1_FC_ARENA_TEAM_READY_FINISH			0x0A	// C->F
#define T1_FC_ARENA_TEAM_READY_FINISH_CANCEL	0x0B	// C->F
#define T1_FC_ARENA_TEAM_MATCHING				0x0C	// F->C(n)
#define T1_FC_ARENA_ENTER_ROOM					0x0D	// F->C(n)
#define T1_FC_ARENA_ENTER_ROOM_WARP				0x0E	// C->F
#define T1_FC_ARENA_ROOM_WAR_START				0x0F	// F->C(n)
#define T1_FC_ARENA_ROOM_WAR_INFO				0x10	// F->C(n)	
#define T1_FC_ARENA_ROOM_WAR_FINISH_HEADER		0x11	// F->C(n)
#define T1_FC_ARENA_ROOM_WAR_FINISH				0x12	// F->C(n)
#define T1_FC_ARENA_ROOM_WAR_FINISH_DONE		0x13	// F->C(n)
#define T1_FC_ARENA_ROOM_WAR_FINISH_DRAW		0x14	// F->C(n)
#define T1_FC_ARENA_FINISH_WARP					0x15	// C->F
#define T1_FC_ARENA_FINISH_WARP_OK				0x16	// F->C(n)
#define T1_FC_ARENA_REQUEST_CREATE_TEAMINFO		0x17	// C->F
#define T1_FC_ARENA_REQUEST_CREATE_TEAMINFO_OK	0x18	// F->C
#define T1_FC_ARENA_BEFORE_ROOM_WAR_FINISH		0x19	// F->C
#define T1_FC_ARENA_REQUEST_OTHER_TEAM_INFO		0x1A	// C->F
#define T1_FC_ARENA_REQUEST_OTHER_TEAM_INFO_OK	0x1B	// F->C
#define T1_FC_ARENA_GM_COMMAND_INFO_HEADER		0x1C	// F->C
#define T1_FC_ARENA_GM_COMMAND_INFO				0x1D	// F->C
#define T1_FC_ARENA_GM_COMMAND_INFO_DONE		0x1E	// F->C
#define T1_FC_ARENA_QUICK_ENTER_TEAM			0x1F	// C->F
#define T1_FC_ARENA_WATCH_READY					0x20	// F->C
#define T1_FC_ARENA_WATCH_START					0x21	// F->C
#define T1_FC_ARENA_WATCH_END					0x22	// F->C
#define T1_FC_ARENA_WATCH_REMOVE_USER			0x23	// F->C
// 아레나 통합 -
#define T1_FC_ARENA_POSSIBLE_GAMESTART			0x24	// F->C
#define T1_FC_ARENA_CHARACTER_GAMESTART			0x25	// C->F
#define T1_FC_ARENA_CHARACTER_GAMESTART_OK		0x26	// F->C
#define T1_FC_ARENA_USE_CITYWARPITEM			0x27	// F->C


//////////////////////////////////////////////////////////////////////////
// 2007-07-06 by dhjin, Tutorial
// T0_FC_TUTORIAL
#define T1_FC_TUTORIAL_START					0x00	// C->F
#define T1_FC_TUTORIAL_START_OK					0x01	// F->C
#define T1_FC_TUTORIAL_COMPLETE					0x02	// C->F
#define T1_FC_TUTORIAL_COMPLETE_OK				0x03	// F->C
#define T1_FC_TUTORIAL_END						0x04	// C->F
#define T1_FC_TUTORIAL_END_OK					0x05	// F->C
#define T1_FC_TUTORIAL_WARP						0x06	// C->F


//////////////////////////////////////////////////////////////////////////
// 2007-08-13 by dhjin, OutPost
// T0_FC_OUTPOST
#define T1_FC_OUTPOST_WAR_BEFORE				0x00	// F->C(N)
#define T1_FC_OUTPOST_WAR_START					0x01	// F->C(N)
#define T1_FC_OUTPOST_PROTECTOR_DESTROY			0x02	// F->C(N)
//#define T1_FC_OUTPOST_RESET_CHECK_START			0x03	// C->F
//#define T1_FC_OUTPOST_RESET_CHECK_START_OK		0x04	// F->C
#define T1_FC_OUTPOST_RESET_START				0x05	// C->F
#define T1_FC_OUTPOST_RESET_START_OK			0x06	// F->C
#define T1_FC_OUTPOST_RESET_DESTROY				0x07	// F->C
#define T1_FC_OUTPOST_RESET_SUCCESS				0x08	// F->C
#define T1_FC_OUTPOST_WAR_END					0x09	// F->C(N)
#define T1_FC_OUTPOST_NEXTWAR_INFO_REQUEST		0x0A	// C->F
#define T1_FC_OUTPOST_NEXTWAR_INFO_REQUEST_OK	0x0B	// F->C
#define T1_FC_OUTPOST_NEXTWAR_SET_TIME			0x0C	// C->F
#define T1_FC_OUTPOST_NEXTWAR_SET_TIME_OK		0x0D	// F->C
#define T1_FC_OUTPOST_WAR_INFO					0x0E	// F->C

////////////////////////////////////////////////////////////////////////////////
// 2009-09-09 ~ 2010 by dhjin, 인피니티 - 
// T0_FC_INFINITY
#define T1_FC_INFINITY_MODE_LIST				0x00	// C -> F
#define T1_FC_INFINITY_MODE_LIST_OK				0x01	// F -> C
#define T1_FC_INFINITY_READY_LIST				0x02	// C -> F
#define T1_FC_INFINITY_READY_LIST_OK			0x03	// F -> C
#define T1_FC_INFINITY_CREATE					0x04	// C -> F
#define T1_FC_INFINITY_CREATE_OK				0x05	// F -> C
#define T1_FC_INFINITY_JOIN						0x06	// C -> F
#define T1_FC_INFINITY_JOIN_REQUEST_MASTERUSER	0x07	// F -> C
#define T1_FC_INFINITY_JOIN_REQUEST_MASTERUSER_OK	0x08	// C -> F
#define T1_FC_INFINITY_JOIN_OK					0x09	// F -> C
#define T1_FC_INFINITY_MEMBER_INFO_LIST			0x0A	// C -> F
#define T1_FC_INFINITY_MEMBER_INFO_LIST_OK		0x0B	// F -> C
#define T1_FC_INFINITY_CHANGE_MASTER			0x0C	// C -> F
#define T1_FC_INFINITY_CHANGE_MASTER_OK			0x0D	// F -> C
#define T1_FC_INFINITY_LEAVE					0x0E	// C -> F
#define T1_FC_INFINITY_LEAVE_OK					0x0F	// F -> C
#define T1_FC_INFINITY_BAN						0x10	// C -> F
#define T1_FC_INFINITY_BAN_OK					0x11	// F -> C
#define T1_FC_INFINITY_READY					0x12	// C -> F
#define T1_FC_INFINITY_READY_OK					0x13	// F -> C
#define T1_FC_INFINITY_READY_CANCEL				0x14	// C -> F
#define T1_FC_INFINITY_READY_CANCEL_OK			0x15	// F -> C
#define T1_FC_INFINITY_START					0x16	// C -> F
#define T1_FC_INFINITY_MAP_LOADING				0x17	// F -> C
#define T1_FC_INFINITY_MAP_LOADED				0x18	// C -> F
#define T1_FC_INFINITY_ENTER					0x19	// F -> C
#define T1_FC_INFINITY_CINEMA					0x1A	// F -> C
#define T1_FC_INFINITY_SUCCESS_FIN				0x1B	// F -> C
#define T1_FC_INFINITY_FAIL_FIN					0x1C	// F -> C
#define T1_FC_INFINITY_FIN_OK					0x1D	// C -> F
#define T1_FC_INFINITY_READY_FINISH_MAINSVR_START 0x1E	// F -> C
#define T1_FC_INFINITY_TENDER_DROPITEM_INFO		0x1F	// 2009-09-09 ~ 2010 by dhjin, 인피니티 - 몬스터에게서 드랍된 Tender 아이템, F -> C
#define T1_FC_INFINITY_TENDER_START				0x20	// 2009-09-09 ~ 2010 by dhjin, 인피니티 - Tender 시작, F -> C
#define T1_FC_INFINITY_TENDER_PUT_IN_TENDER		0x21	// 2009-09-09 ~ 2010 by dhjin, 인피니티 - 유저 Tender 버튼 누름, C -> F
#define T1_FC_INFINITY_TENDER_PUT_IN_TENDER_OK  0x22	// 2009-09-09 ~ 2010 by dhjin, 인피니티 - 유저 Tender 버튼 누름 결과, F -> C
#define T1_FC_INFINITY_TENDER_RESULT			0x23	// 2009-09-09 ~ 2010 by dhjin, 인피니티 - 입찰자 결과, F -> C
#define T1_FC_INFINITY_TENDER_TIMEOVER			0x24	// 2009-09-09 ~ 2010 by dhjin, 인피니티 - Tender 시간이 지났지만 입찰자가 없다, F -> C
#define T1_FC_INFINITY_TENDER_ALLGIVEUP			0x25	// 2009-09-09 ~ 2010 by dhjin, 인피니티 - 입찰자 모두 포기, F -> C
#define T1_FC_INFINITY_CHANGE_LIMITTIME			0x26	// 2009-09-09 ~ 2010 by dhjin, 인피니티 - 인피 사망시 패널티 추가, F -> C
#define T1_FC_INFINITY_JOIN_CANCEL				0x27	// 2009-09-09 ~ 2010 by dhjin, 인피니티 - 인피 입장 취소
#define T1_FC_INFINITY_JOIN_CANCEL_REQUEST_MASTERUSER	0x28		// 2009-09-09 ~ 2010 by dhjin, 인피니티 - 인피 입장 취소
#define T1_FC_INFINITY_REQUEST_RESTART_BY_DISCONNECT	0x29	// 2009-09-09 ~ 2010 by dhjin, 인피니티 - 팅긴 유저 재접속 처리
#define T1_FC_INFINITY_RESTART_BY_DISCONNECT	0x2A	// 2009-09-09 ~ 2010 by dhjin, 인피니티 - 팅긴 유저 재접속 처리
#define T1_FC_INFINITY_MAP_LOADED_RESTART_BY_DISCONNECT	0x2B		// 2009-09-09 ~ 2010 by dhjin, 인피니티 - 팅긴 유저 재접속 처리, C -> AFS
#define T1_FC_INFINITY_DELETED_CINEMA_HEADER	0x2C				// 2009-09-09 ~ 2010 by dhjin, 인피니티 - 팅긴 유저 재접속 처리
#define T1_FC_INFINITY_DELETED_CINEMA			0x2D				// 2009-09-09 ~ 2010 by dhjin, 인피니티 - 팅긴 유저 재접속 처리
#define T1_FC_INFINITY_DELETED_CINEMA_DONE		0x2E				// 2009-09-09 ~ 2010 by dhjin, 인피니티 - 팅긴 유저 재접속 처리
#define T1_FC_INFINITY_ENTER_BY_DISCONNECT		0x2F				// 2009-09-09 ~ 2010 by dhjin, 인피니티 - 팅긴 유저 재접속 처리
#define T1_FC_INFINITY_CHANGE_ALIVE_FOR_GAMECLEAR_MONSTERHP			0x30		// 2010-03-31 by dhjin, 인피니티(기지방어) - 인피 사망시 패널티, F -> C

// 2010. 05. 19 by hsLee 인피니티 필드 2차 난이도 조절. (신호처리 + 몬스터 처리(서버) )
//#define T1_FC_INFINITY_DIFFICULTY_LIST			0x31 // 2010-05-04 by shcho, 인피니티 난이도 정보 목록 요청. - C -> AFS
#define T1_FC_INFINITY_DIFFICULTY_LIST_OK		0x31 // 2010-05-04 by shcho, 인피니티 난이도 정보 목록 요청. - AFS -> C

#define T1_FC_INFINITY_CHANGE_DIFFICULTY_LEVEL		0x32	// 2010. 05. 19 by hsLee 인피니티 필드 2차 난이도 조절. (신호처리 + 몬스터 처리(서버) ) - 방의 난이도 변경. C -> AFS
#define T1_FC_INFINITY_CHANGE_DIFFICULTY_LEVEL_OK	0x33	// 2010. 05. 19 by hsLee 인피니티 필드 2차 난이도 조절. (신호처리 + 몬스터 처리(서버) ) - 방의 난이도 변경. AFS -> C
// End 2010. 05. 19 by hsLee 인피니티 필드 2차 난이도 조절. (신호처리 + 몬스터 처리(서버) )

#define T1_FC_INFINITY_READY_CANCEL_ALL_OK		0x34		// 2010. 05. 31 by hsLee 인피니티 필드 2차 난이도 조절. (오브젝트 몬스터 밸런스 적용 문제 수정.) - 방의 모든 맴버 레디 취소 처리. AFS -> C (방의 난이도 변경시 사용.)

// 2010. 07. 27 by hsLee 인피니티 2차 거점 방어 시네마 연출 스킵 처리.
#define T1_FC_INFINITY_SKIP_ENDING_CINEMA		0x35		// 인피니티 거점 방어 엔딩 시네마 연출 스킵 요청. 2010. 07. 27 by hsLee.
#define T1_FC_INFINITY_SKIP_ENDING_CINEMA_OK	0x36		// 인피니티 거점 방어 엔딩 시네마 연출 스킵 결과. 2010. 07. 27 by hsLee.
// End 2010. 07. 27 by hsLee 인피니티 2차 거점 방어 시네마 연출 스킵 처리.

#define T1_FC_INFINITY_MOVIE					0x37		// 2011-05-17 by hskim, 인피니티 3차 - 시네마 연출, F -> C
#define T1_FC_INFINITY_SET_LIMITTIME			0x38		// 2011-05-30 by hskim, 인피니티 3차 - 플레이 시간 재설정 기능, F -> C

// 2007-12-27 by dhjin, 아레나 통합 - 아레나 프로토콜 MainField <=> ArenaField 
#define T1_FtoA_MFSINFO							0x00
#define T1_FtoA_MFSINFO_OK						0x01
#define T1_FtoA_ALIVE							0x02
#define T1_FtoA_AUTH_CHARACTER					0x03
#define T1_FtoA_AUTH_CHARACTER_OK				0x04
#define T1_FtoA_ARENA_STATE_CHANGE				0x05
#define T1_FtoA_ARENA_STATE_CHANGE_OK			0x06
#define T1_FtoA_ARENA_TEAM_MATCHING				0x07
#define T1_FtoA_ARENA_TEAM_MATCHING_OK			0x08
#define T1_FtoA_ARENA_SERVER_PAY				0x09		
#define T1_FtoA_ARENA_CHARACTER_PAY				0x0A
#define T1_FtoA_ARENA_CHARACTER_DISCONNECT		0x0B
// 2009-09-09 ~ 2010 by dhjin, 인피니티 - MFS <-> AFS 인피니티 관련 통신
#define T1_FtoA_INFINITY_IMPUTE_LIST			0x30	//	MFS -> AFS
#define T1_FtoA_INFINITY_START					0x31	//	AFS -> MFS
#define T1_FtoA_INFINITY_START_OK				0x32	//	MFS -> AFS
#define T1_FtoA_INFINITY_IMPUTE					0x33	//	AFS -> MFS
#define T1_FtoA_INFINITY_UPDATE_ALL_ITEM_COUNT	0x34	//	AFS -> MFS
#define T1_FtoA_INFINITY_INSERT_ITEM_HEADER		0x35	//	AFS -> MFS
#define T1_FtoA_INFINITY_INSERT_ITEM			0x36	//	AFS -> MFS
#define T1_FtoA_INFINITY_INSERT_ITEM_DONE		0x37	//	AFS -> MFS
#define T1_FtoA_INFINITY_UPDATE_ITEM_HEADER		0x38	//	AFS -> MFS
#define T1_FtoA_INFINITY_UPDATE_ITEM			0x39	//	AFS -> MFS
#define T1_FtoA_INFINITY_UPDATE_ITEM_DONE		0x3A	//	AFS -> MFS
#define T1_FtoA_INFINITY_DELETE_ITEM			0x3B	//	AFS -> MFS
#define T1_FtoA_INFINITY_UPDATE_USER_INFO		0x3C	//	AFS -> MFS
#define T1_FtoA_INFINITY_READY_FINISH_MAINSVR_START		0x3D		//	MFS -> AFS
#define T1_FtoA_INFINITY_LOG					0x3E	//	AFS -> MFS
#define T1_FtoA_USING_TIME_LIMIT_ITEM			0x3F	//	AFS -> MFS
#define T1_FtoA_INFINITY_STATE_CHANGE			0x40	// 2009-09-09 ~ 2010-01-20 by dhjin, 인피니티 - 인피 상태 값을 메인서버로 전송한다. 인피 상태 체크 //	AFS -> MFS
#define T1_FtoA_INFINITY_START_CHECK			0x41	// AF->MF, // 2010-03-23 by cmkwon, 인피니티 입장 캐쉬 아이템 구현 - 
#define T1_FtoA_INFINITY_START_CHECK_ACK		0x42	// MF->AF, // 2010-03-23 by cmkwon, 인피니티 입장 캐쉬 아이템 구현 - 
#define T1_FtoA_UPDATE_ITEM_NOTI				0x43	//	MFS -> AFS // 2010-03-31 by dhjin, 인피니티 입장 캐쉬 아이템 구현 - 
#define T1_FtoA_INFINITY_UPDATE_USER_MAP_INFO	0x44	//	AFS -> MFS // 2010-04-06 by cmkwon, 인피2차 추가 수정 - 
#define T1_FtoA_LOG_INFINITYUSER_GET_TENDERITEM	0x45	//	AFS -> MFS //// 2010-06-25 by shcho, 인피니티 관련로그 찍기 - 습득 아이템 정보 DB저장 
#ifdef _INET_PET
#define T1_FtoA_INFINITY_UPDATE_ITEM_PET		0x47	//	AFS -> MFS // 2011-09-30 by hskim, 파트너 시스템 2차 - 인피 MainFieldServer로 파트너 아이템 동기화 위해 전송
#endif
// 2011-01-26 by hskim, 인증 서버 구현
#define T1_PAUTH_CONNECT_LOGIN					0x00
#define T1_PAUTH_CONNECT_LOGIN_OK				0x01
#define T1_PAUTH_CONNECT_LOGIN_FAIL				0x02
#define T1_PAUTH_CONNECT_LOGIN_SHUTDOWN			0x03

///////////////////////////////////////////////////////////////////////////////
// Message Type
///////////////////////////////////////////////////////////////////////////////
#define T_PC_DEFAULT_UPDATE_LAUNCHER_VERSION		(MessageType_t)((T0_PC_DEFAULT_UPDATE<<8)|T1_PC_DEFAULT_UPDATE_LAUNCHER_VERSION)
#define T_PC_DEFAULT_UPDATE_LAUNCHER_UPDATE_INFO	(MessageType_t)((T0_PC_DEFAULT_UPDATE<<8)|T1_PC_DEFAULT_UPDATE_LAUNCHER_UPDATE_INFO)
#define T_PC_DEFAULT_UPDATE_LAUNCHER_VERSION_OK		(MessageType_t)((T0_PC_DEFAULT_UPDATE<<8)|T1_PC_DEFAULT_UPDATE_LAUNCHER_VERSION_OK)
#define T_PC_DEFAULT_NEW_UPDATE_LAUNCHER_VERSION			(MessageType_t)((T0_PC_DEFAULT_UPDATE<<8)|T1_PC_DEFAULT_NEW_UPDATE_LAUNCHER_VERSION)		// C->P
#define T_PC_DEFAULT_NEW_UPDATE_LAUNCHER_UPDATE_INFO		(MessageType_t)((T0_PC_DEFAULT_UPDATE<<8)|T1_PC_DEFAULT_NEW_UPDATE_LAUNCHER_UPDATE_INFO)	// P->C

#define T_PC_CONNECT_CLOSE						(MessageType_t)((T0_PC_CONNECT<<8)|T1_PC_CONNECT_CLOSE)
#define T_PC_CONNECT_ALIVE						(MessageType_t)((T0_PC_CONNECT<<8)|T1_PC_CONNECT_ALIVE)
#define T_PC_CONNECT_VERSION					(MessageType_t)((T0_PC_CONNECT<<8)|T1_PC_CONNECT_VERSION)
#define T_PC_CONNECT_UPDATE_INFO				(MessageType_t)((T0_PC_CONNECT<<8)|T1_PC_CONNECT_UPDATE_INFO)
#define T_PC_CONNECT_VERSION_OK					(MessageType_t)((T0_PC_CONNECT<<8)|T1_PC_CONNECT_VERSION_OK)		// no body
#define T_PC_CONNECT_REINSTALL_CLIENT			(MessageType_t)((T0_PC_CONNECT<<8)|T1_PC_CONNECT_REINSTALL_CLIENT)
#define T_PC_CONNECT_LOGIN						(MessageType_t)((T0_PC_CONNECT<<8)|T1_PC_CONNECT_LOGIN)
#define T_PC_CONNECT_LOGIN_OK					(MessageType_t)((T0_PC_CONNECT<<8)|T1_PC_CONNECT_LOGIN_OK)
#define T_PC_CONNECT_SINGLE_FILE_VERSION_CHECK		(MessageType_t)((T0_PC_CONNECT<<8)|T1_PC_CONNECT_SINGLE_FILE_VERSION_CHECK)		// single file들에 대한 버전 확인(deletefilelist.txt, notice.txt 등)
#define T_PC_CONNECT_SINGLE_FILE_VERSION_CHECK_OK	(MessageType_t)((T0_PC_CONNECT<<8)|T1_PC_CONNECT_SINGLE_FILE_VERSION_CHECK_OK)	// No Body
#define T_PC_CONNECT_SINGLE_FILE_UPDATE_INFO		(MessageType_t)((T0_PC_CONNECT<<8)|T1_PC_CONNECT_SINGLE_FILE_UPDATE_INFO)
#define T_PC_CONNECT_GET_SERVER_GROUP_LIST			(MessageType_t)((T0_PC_CONNECT<<8)|T1_PC_CONNECT_GET_SERVER_GROUP_LIST)		// Launcher->P, No Body
#define T_PC_CONNECT_GET_SERVER_GROUP_LIST_OK		(MessageType_t)((T0_PC_CONNECT<<8)|T1_PC_CONNECT_GET_SERVER_GROUP_LIST_OK)	// P->Launcher
#define T_PC_CONNECT_GET_GAME_SERVER_GROUP_LIST		(MessageType_t)((T0_PC_CONNECT<<8)|T1_PC_CONNECT_GET_GAME_SERVER_GROUP_LIST)		// 2007-05-02 by cmkwon, C->P
#define T_PC_CONNECT_GET_GAME_SERVER_GROUP_LIST_OK	(MessageType_t)((T0_PC_CONNECT<<8)|T1_PC_CONNECT_GET_GAME_SERVER_GROUP_LIST_OK)		// 2007-05-02 by cmkwon, P->C
#define T_PC_CONNECT_NETWORK_CHECK					(MessageType_t)((T0_PC_CONNECT<<8)|T1_PC_CONNECT_NETWORK_CHECK)			// 2007-06-18 by cmkwon, C->P, // 2007-06-18 by cmkwon, 네트워크 상태 체크
#define T_PC_CONNECT_NETWORK_CHECK_OK				(MessageType_t)((T0_PC_CONNECT<<8)|T1_PC_CONNECT_NETWORK_CHECK_OK)		// 2007-06-18 by cmkwon, P->C, // 2007-06-18 by cmkwon, 네트워크 상태 체크
#define T_PC_CONNECT_GET_NEW_GAME_SERVER_GROUP_LIST		(MessageType_t)((T0_PC_CONNECT<<8)|T1_PC_CONNECT_GET_NEW_GAME_SERVER_GROUP_LIST)	// 2007-09-05 by cmkwon, EXE_1에 로그인 서버 선택 인터페이스 수정 - C->P
#define T_PC_CONNECT_GET_NEW_GAME_SERVER_GROUP_LIST_OK	(MessageType_t)((T0_PC_CONNECT<<8)|T1_PC_CONNECT_GET_NEW_GAME_SERVER_GROUP_LIST_OK)	// 2007-09-05 by cmkwon, EXE_1에 로그인 서버 선택 인터페이스 수정 - P->C
#ifdef _INET_MAC_ADDRESS_CHECKER
#define T_PC_CONNECT_SEND_GET_BLOCKED_MAC_ADDR			(MessageType_t)((T0_PC_CONNECT<<8)|T1_PC_CONNECT_SEND_GET_BLOCKED_MAC_ADDR) // 2016-03-06 by inet - for send to p-server mac address
#define T_PC_CONNECT_SEND_GET_BLOCKED_MAC_ADDR_BLOCK	(MessageType_t)((T0_PC_CONNECT<<8)|T1_PC_CONNECT_SEND_GET_BLOCKED_MAC_ADDR_BLOCK) // 2016-03-07 by inet - for send to p-server mac address
#define T_PC_CONNECT_SEND_GET_BLOCKED_MAC_ADDR_OK		(MessageType_t)((T0_PC_CONNECT<<8)|T1_PC_CONNECT_SEND_GET_BLOCKED_MAC_ADDR_OK) // 2016-03-06 by inet - for send to p-server mac address
#endif
#define T_PC_CONNECT_LOGIN_BLOCKED					(MessageType_t)((T0_PC_CONNECT<<8)|T1_PC_CONNECT_LOGIN_BLOCKED)				// P->Launcher

#define T_FC_CONNECT_CLOSE						(MessageType_t)((T0_FC_CONNECT<<8)|T1_FC_CONNECT_CLOSE)
#define T_FC_CONNECT_ALIVE						(MessageType_t)((T0_FC_CONNECT<<8)|T1_FC_CONNECT_ALIVE)
#define T_FC_CONNECT_LOGIN						(MessageType_t)((T0_FC_CONNECT<<8)|T1_FC_CONNECT_LOGIN)
#define T_FC_CONNECT_LOGIN_OK					(MessageType_t)((T0_FC_CONNECT<<8)|T1_FC_CONNECT_LOGIN_OK)
#define T_FC_CONNECT_SYNC_TIME					(MessageType_t)((T0_FC_CONNECT<<8)|T1_FC_CONNECT_SYNC_TIME)				// 시간 동기화를 위해
#define T_FC_CONNECT_NOTIFY_SERVER_SHUTDOWN		(MessageType_t)((T0_FC_CONNECT<<8)|T1_FC_CONNECT_NOTIFY_SERVER_SHUTDOWN)	// No body, 2006-08-04 by cmkwon
#define T_FC_CONNECT_NETWORK_CHECK				(MessageType_t)((T0_FC_CONNECT<<8)|T1_FC_CONNECT_NETWORK_CHECK)		// C->F, // 2008-02-15 by cmkwon, Client<->FieldServer 간 네트워크 상태 체크 
#define T_FC_CONNECT_NETWORK_CHECK_OK			(MessageType_t)((T0_FC_CONNECT<<8)|T1_FC_CONNECT_NETWORK_CHECK_OK)	// F->C, // 2008-02-15 by cmkwon, Client<->FieldServer 간 네트워크 상태 체크 
#define T_FC_CONNECT_ARENASERVER_INFO			(MessageType_t)((T0_FC_CONNECT<<8)|T1_FC_CONNECT_ARENASERVER_INFO)		// 2007-12-28 by dhjin, 아레나 통합 - F -> C 
#define T_FC_CONNECT_ARENASERVER_LOGIN			(MessageType_t)((T0_FC_CONNECT<<8)|T1_FC_CONNECT_ARENASERVER_LOGIN)		// 2007-12-28 by dhjin, 아레나 통합 - AF -> C 
#define T_FC_CONNECT_ARENASERVER_LOGIN_OK		(MessageType_t)((T0_FC_CONNECT<<8)|T1_FC_CONNECT_ARENASERVER_LOGIN_OK)		// 2007-12-28 by dhjin, 아레나 통합 - C -> AF 
#define T_FC_CONNECT_ARENASERVER_SSERVER_GROUP_FOR_CLIENT			(MessageType_t)((T0_FC_CONNECT<<8)|T1_FC_CONNECT_ARENASERVER_SSERVER_GROUP_FOR_CLIENT)		// 2008-02-25 by dhjin, 아레나 통합 - AF -> C
#define T_FC_CONNECT_ARENASERVER_TO_IMSERVER	(MessageType_t)((T0_FC_CONNECT<<8)|T1_FC_CONNECT_ARENASERVER_TO_IMSERVER)		// 2008-03-03 by dhjin, 아레나 통합 - C -> F 


#define T_FP_CONNECT_CLOSE						(MessageType_t)((T0_FP_CONNECT<<8)|T1_FP_CONNECT_CLOSE)
#define T_FP_CONNECT_ALIVE						(MessageType_t)((T0_FP_CONNECT<<8)|T1_FP_CONNECT_ALIVE)
#define T_FP_CONNECT_AUTH_USER					(MessageType_t)((T0_FP_CONNECT<<8)|T1_FP_CONNECT_AUTH_USER)
#define T_FP_CONNECT_AUTH_USER_OK				(MessageType_t)((T0_FP_CONNECT<<8)|T1_FP_CONNECT_AUTH_USER_OK)
#define T_FP_CONNECT_FIELD_CONNECT				(MessageType_t)((T0_FP_CONNECT<<8)|T1_FP_CONNECT_FIELD_CONNECT)
#define T_FP_CONNECT_FIELD_CONNECT_OK			(MessageType_t)((T0_FP_CONNECT<<8)|T1_FP_CONNECT_FIELD_CONNECT_OK)
#define T_FP_CONNECT_NOTIFY_CLOSE				(MessageType_t)((T0_FP_CONNECT<<8)|T1_FP_CONNECT_NOTIFY_CLOSE)
#define T_FP_CONNECT_NOTIFY_FIELDSERVER_CHANGE		(MessageType_t)((T0_FP_CONNECT<<8)|T1_FP_CONNECT_NOTIFY_FIELDSERVER_CHANGE)
#define T_FP_CONNECT_NOTIFY_FIELDSERVER_CHANGE_OK	(MessageType_t)((T0_FP_CONNECT<<8)|T1_FP_CONNECT_NOTIFY_FIELDSERVER_CHANGE_OK)
#define T_FP_CONNECT_PREPARE_SHUTDOWN				(MessageType_t)((T0_FP_CONNECT<<8)|T1_FP_CONNECT_PREPARE_SHUTDOWN)				// No body, 2006-08-04 by cmkwon
#define T_FP_CONNECT_UPDATE_DBSERVER_GROUP			(MessageType_t)((T0_FP_CONNECT<<8)|T1_FP_CONNECT_UPDATE_DBSERVER_GROUP)			// 2008-04-29 by cmkwon, 서버군 정보 DB에 추가(신규 계정 캐릭터 생성 제한 시스템추가) - P->F
#define T_FP_CONNECT_CHECK_CONNECTABLE_ACCOUNT		(MessageType_t)((T0_FP_CONNECT<<8)|T1_FP_CONNECT_CHECK_CONNECTABLE_ACCOUNT)		// 2008-04-29 by cmkwon, 서버군 정보 DB에 추가(신규 계정 캐릭터 생성 제한 시스템추가) - P->F
#define T_FP_CONNECT_CHECK_CONNECTABLE_ACCOUNT_OK	(MessageType_t)((T0_FP_CONNECT<<8)|T1_FP_CONNECT_CHECK_CONNECTABLE_ACCOUNT_OK)	// 2008-04-29 by cmkwon, 서버군 정보 DB에 추가(신규 계정 캐릭터 생성 제한 시스템추가) - F->P
#define T_FP_ADMIN_RELOAD_WORLDRANKING				(MessageType_t)((T0_FP_CONNECT<<8)|T1_FP_ADMIN_RELOAD_WORLDRANKING)		// 2009-02-12 by cmkwon, EP3-3 월드랭킹시스템 구현 - P->F(n)
#define T_FP_ADMIN_RELOAD_INFLUENCERATE				(MessageType_t)((T0_FP_CONNECT<<8)|T1_FP_ADMIN_RELOAD_INFLUENCERATE)		// P->F // 2009-09-16 by cmkwon, 세력 초기화시 어뷰징 방지 구현 - 

#define T_IP_CONNECT_CLOSE						(MessageType_t)((T0_IP_CONNECT<<8)|T1_IP_CONNECT_CLOSE)
#define T_IP_CONNECT_ALIVE						(MessageType_t)((T0_IP_CONNECT<<8)|T1_IP_CONNECT_ALIVE)
#define T_IP_CONNECT_IM_CONNECT					(MessageType_t)((T0_IP_CONNECT<<8)|T1_IP_CONNECT_IM_CONNECT)
#define T_IP_CONNECT_IM_CONNECT_OK				(MessageType_t)((T0_IP_CONNECT<<8)|T1_IP_CONNECT_IM_CONNECT_OK)
#define T_IP_GET_SERVER_GROUP_INFO				(MessageType_t)((T0_IP_CONNECT<<8)|T1_IP_GET_SERVER_GROUP_INFO)
#define T_IP_GET_SERVER_GROUP_INFO_ACK			(MessageType_t)((T0_IP_CONNECT<<8)|T1_IP_GET_SERVER_GROUP_INFO_ACK)
#define T_IP_ADMIN_PETITION_SET_PERIOD			(MessageType_t)((T0_IP_CONNECT<<8)|T1_IP_ADMIN_PETITION_SET_PERIOD)	// 2007-11-19 by cmkwon, 진정시스템 업데이트 - P->I(n)
#define T_IP_ADMIN_RELOAD_ADMIN_NOTICE_SYSTEM	(MessageType_t)((T0_IP_CONNECT<<8)|T1_IP_ADMIN_RELOAD_ADMIN_NOTICE_SYSTEM)	// 2009-01-14 by cmkwon, 운영자 자동 공지 시스템 구현 - P->I(n)

#define T_IC_CONNECT_CLOSE						(MessageType_t)((T0_IC_CONNECT<<8)|T1_IC_CONNECT_CLOSE)
#define T_IC_CONNECT_ALIVE						(MessageType_t)((T0_IC_CONNECT<<8)|T1_IC_CONNECT_ALIVE)
#define T_IC_CONNECT_LOGIN						(MessageType_t)((T0_IC_CONNECT<<8)|T1_IC_CONNECT_LOGIN)
#define T_IC_CONNECT_LOGIN_OK					(MessageType_t)((T0_IC_CONNECT<<8)|T1_IC_CONNECT_LOGIN_OK)
#define T_IC_CONNECT_FM_TO_IM_OK				(MessageType_t)((T0_IC_CONNECT<<8)|T1_IC_CONNECT_FM_TO_IM_OK)		// 2008-03-03 by dhjin, 아레나 통합 - I->C

#define T_FI_CONNECT							(MessageType_t)((T0_FI_CONNECT<<8)|T1_FI_CONNECT)
#define T_FI_CONNECT_OK							(MessageType_t)((T0_FI_CONNECT<<8)|T1_FI_CONNECT_OK)
#define T_FI_CONNECT_CLOSE						(MessageType_t)((T0_FI_CONNECT<<8)|T1_FI_CONNECT_CLOSE)
#define T_FI_CONNECT_ALIVE						(MessageType_t)((T0_FI_CONNECT<<8)|T1_FI_CONNECT_ALIVE)
#define T_FI_CONNECT_NOTIFY_FIELDSERVER_IP		(MessageType_t)((T0_FI_CONNECT<<8)|T1_FI_NOTIFY_FIELDSERVER_IP)
#define T_FI_CONNECT_NOTIFY_GAMEEND				(MessageType_t)((T0_FI_CONNECT<<8)|T1_FI_NOTIFY_GAMEEND)
//#define T_FI_CONNECT_NOTIFY_MAP_CHANGE			(MessageType_t)((T0_FI_CONNECT<<8)|T1_FI_CONNECT_NOTIFY_MAP_CHANGE)	// F -> I, check: deprecated, T_FI_EVENT_NOTIFY_WARP로 대체됨
#define T_FI_CONNECT_NOTIFY_DEAD				(MessageType_t)((T0_FI_CONNECT<<8)|T1_FI_CONNECT_NOTIFY_DEAD)		// F -> I
#define T_FI_GET_FIELD_USER_COUNTS				(MessageType_t)((T0_FI_CONNECT<<8)|T1_FI_GET_FIELD_USER_COUNTS)		// F -> I
#define T_FI_GET_FIELD_USER_COUNTS_ACK			(MessageType_t)((T0_FI_CONNECT<<8)|T1_FI_GET_FIELD_USER_COUNTS_ACK)	// F -> I
#define T_FI_CONNECT_NOTIFY_GAMESTART			(MessageType_t)((T0_FI_CONNECT<<8)|T1_FI_CONNECT_NOTIFY_GAMESTART)		// F->I, 게임 시작했을 때 IM Server에 알림, 파티 정보 확인 요청 등
#define T_FI_CONNECT_NOTIFY_DEAD_GAMESTART		(MessageType_t)((T0_FI_CONNECT<<8)|T1_FI_CONNECT_NOTIFY_DEAD_GAMESTART)	// F->I, 죽은후에 게임 시작했을 때 IM Server에 알림
#define T_FI_CONNECT_PREPARE_SHUTDOWN			(MessageType_t)((T0_FI_CONNECT<<8)|T1_FI_CONNECT_PREPARE_SHUTDOWN)		// I->F, // 2007-08-27 by cmkwon, 서버다운준비 명령어 추가(SCAdminTool에서 SCMonitor의 PrepareShutdown을 진행 할 수 있게)

#define T_PM_CONNECT							(MessageType_t)((T0_PM_CONNECT<<8)|T1_PM_CONNECT)
#define T_PM_CONNECT_OK							(MessageType_t)((T0_PM_CONNECT<<8)|T1_PM_CONNECT_OK)
#define T_PM_CONNECT_CLOSE						(MessageType_t)((T0_PM_CONNECT<<8)|T1_PM_CONNECT_CLOSE)
#define T_PM_CONNECT_ALIVE						(MessageType_t)((T0_PM_CONNECT<<8)|T1_PM_CONNECT_ALIVE)

#define T_FM_CONNECT							(MessageType_t)((T0_FM_CONNECT<<8)|T1_FM_CONNECT)
#define T_FM_CONNECT_OK							(MessageType_t)((T0_FM_CONNECT<<8)|T1_FM_CONNECT_OK)
#define T_FM_CONNECT_CLOSE						(MessageType_t)((T0_FM_CONNECT<<8)|T1_FM_CONNECT_CLOSE)
#define T_FM_CONNECT_ALIVE						(MessageType_t)((T0_FM_CONNECT<<8)|T1_FM_CONNECT_ALIVE)

#define T_FM_CONNECT_BACK							(MessageType_t)((T0_FM_CONNECT<<8)|T1_FM_CONNECT_BACK)
#define T_FM_CONNECT_OK_BACK						(MessageType_t)((T0_FM_CONNECT<<8)|T1_FM_CONNECT_OK_BACK)
#define T_FM_CONNECT_CLOSE_BACK						(MessageType_t)((T0_FM_CONNECT<<8)|T1_FM_CONNECT_CLOSE_BACK)
#define T_FM_CONNECT_ALIVE_BACK						(MessageType_t)((T0_FM_CONNECT<<8)|T1_FM_CONNECT_ALIVE_BACK)

#define T_FN_CONNECT_CLOSE						(MessageType_t)((T0_FN_CONNECT<<8)|T1_FN_CONNECT_CLOSE)
#define T_FN_CONNECT_ALIVE						(MessageType_t)((T0_FN_CONNECT<<8)|T1_FN_CONNECT_ALIVE)
#define T_FN_CONNECT_INCREASE_CHANNEL			(MessageType_t)((T0_FN_CONNECT<<8)|T1_FN_CONNECT_INCREASE_CHANNEL)		// F->N
#define T_FN_CONNECT_SET_CHANNEL_STATE			(MessageType_t)((T0_FN_CONNECT<<8)|T1_FN_CONNECT_SET_CHANNEL_STATE)		// F->N

#define T_LM_CONNECT							(MessageType_t)((T0_LM_CONNECT<<8)|T1_LM_CONNECT)
#define T_LM_CONNECT_OK							(MessageType_t)((T0_LM_CONNECT<<8)|T1_LM_CONNECT_OK)
#define T_LM_CONNECT_CLOSE						(MessageType_t)((T0_LM_CONNECT<<8)|T1_LM_CONNECT_CLOSE)
#define T_LM_CONNECT_ALIVE						(MessageType_t)((T0_LM_CONNECT<<8)|T1_LM_CONNECT_ALIVE)

#define T_IM_CONNECT							(MessageType_t)((T0_IM_CONNECT<<8)|T1_IM_CONNECT)
#define T_IM_CONNECT_OK							(MessageType_t)((T0_IM_CONNECT<<8)|T1_IM_CONNECT_OK)
#define T_IM_CONNECT_CLOSE						(MessageType_t)((T0_IM_CONNECT<<8)|T1_IM_CONNECT_CLOSE)
#define T_IM_CONNECT_ALIVE						(MessageType_t)((T0_IM_CONNECT<<8)|T1_IM_CONNECT_ALIVE)

#define T_NM_CONNECT							(MessageType_t)((T0_NM_CONNECT<<8)|T1_NM_CONNECT)
#define T_NM_CONNECT_OK							(MessageType_t)((T0_NM_CONNECT<<8)|T1_NM_CONNECT_OK)
#define T_NM_CONNECT_CLOSE						(MessageType_t)((T0_NM_CONNECT<<8)|T1_NM_CONNECT_CLOSE)
#define T_NM_CONNECT_ALIVE						(MessageType_t)((T0_NM_CONNECT<<8)|T1_NM_CONNECT_ALIVE)

#define T_PL_CONNECT							(MessageType_t)((T0_PL_CONNECT<<8)|T1_PL_CONNECT)
#define T_PL_CONNECT_OK							(MessageType_t)((T0_PL_CONNECT<<8)|T1_PL_CONNECT_OK)
#define T_PL_CONNECT_CLOSE						(MessageType_t)((T0_PL_CONNECT<<8)|T1_PL_CONNECT_CLOSE)
#define T_PL_CONNECT_ALIVE						(MessageType_t)((T0_PL_CONNECT<<8)|T1_PL_CONNECT_ALIVE)

#define T_IL_CONNECT							(MessageType_t)((T0_IL_CONNECT<<8)|T1_IL_CONNECT)
#define T_IL_CONNECT_OK							(MessageType_t)((T0_IL_CONNECT<<8)|T1_IL_CONNECT_OK)
#define T_IL_CONNECT_CLOSE						(MessageType_t)((T0_IL_CONNECT<<8)|T1_IL_CONNECT_CLOSE)
#define T_IL_CONNECT_ALIVE						(MessageType_t)((T0_IL_CONNECT<<8)|T1_IL_CONNECT_ALIVE)

#define T_FL_CONNECT							(MessageType_t)((T0_FL_CONNECT<<8)|T1_FL_CONNECT)
#define T_FL_CONNECT_OK							(MessageType_t)((T0_FL_CONNECT<<8)|T1_FL_CONNECT_OK)
#define T_FL_CONNECT_CLOSE						(MessageType_t)((T0_FL_CONNECT<<8)|T1_FL_CONNECT_CLOSE)
#define T_FL_CONNECT_ALIVE						(MessageType_t)((T0_FL_CONNECT<<8)|T1_FL_CONNECT_ALIVE)

#define T_NL_CONNECT							(MessageType_t)((T0_NL_CONNECT<<8)|T1_NL_CONNECT)
#define T_NL_CONNECT_OK							(MessageType_t)((T0_NL_CONNECT<<8)|T1_NL_CONNECT_OK)
#define T_NL_CONNECT_CLOSE						(MessageType_t)((T0_NL_CONNECT<<8)|T1_NL_CONNECT_CLOSE)
#define T_NL_CONNECT_ALIVE						(MessageType_t)((T0_NL_CONNECT<<8)|T1_NL_CONNECT_ALIVE)

#define T_FI_EVENT_NOTIFY_WARP					(MessageType_t)((T0_FI_EVENT<<8)|T1_FI_EVENT_NOTIFY_WARP)			// F -> I
// 2005-07-27 by cmkwon, 다른 필드서버로의 워프는 없으므로 삭제함
//#define T_FI_EVENT_NOTIFY_WARP_OK				(MessageType_t)((T0_FI_EVENT<<8)|T1_FI_EVENT_NOTIFY_WARP_OK)		// I -> F, 다른 필드 서버로의 워프인 경우만 받는다
//#define T_FI_EVENT_GET_WARP_INFO				(MessageType_t)((T0_FI_EVENT<<8)|T1_FI_EVENT_GET_WARP_INFO)		// F -> I, Party & TimerEvent정보, 다른 필드 서버로의 워프인 경우 정보 요청
//#define T_FI_EVENT_GET_WARP_INFO_OK				(MessageType_t)((T0_FI_EVENT<<8)|T1_FI_EVENT_GET_WARP_INFO_OK)	// I -> F, Party & TimerEvent정보, 다른 필드 서버로의 워프인 경우 정보 주기
#define T_FI_EVENT_CHAT_BLOCK					(MessageType_t)((T0_FI_EVENT<<8)|T1_FI_EVENT_CHAT_BLOCK)		// 2008-12-30 by cmkwon, 지도자 채팅 제한 카드 구현 - F->I

#define T_IC_CHAT_ALL							(MessageType_t)((T0_IC_CHAT<<8)|T1_IC_CHAT_ALL)
#define T_IC_CHAT_MAP							(MessageType_t)((T0_IC_CHAT<<8)|T1_IC_CHAT_MAP)
#define T_IC_CHAT_REGION						(MessageType_t)((T0_IC_CHAT<<8)|T1_IC_CHAT_REGION)
#define T_IC_CHAT_PTOP							(MessageType_t)((T0_IC_CHAT<<8)|T1_IC_CHAT_PTOP)
#define T_IC_CHAT_PARTY							(MessageType_t)((T0_IC_CHAT<<8)|T1_IC_CHAT_PARTY)
#define T_IC_CHAT_GUILD							(MessageType_t)((T0_IC_CHAT<<8)|T1_IC_CHAT_GUILD)
#define T_IC_CHAT_GET_GUILD						(MessageType_t)((T0_IC_CHAT<<8)|T1_IC_CHAT_GET_GUILD)
#define T_IC_CHAT_GET_GUILD_OK					(MessageType_t)((T0_IC_CHAT<<8)|T1_IC_CHAT_GET_GUILD_OK)
#define T_IC_CHAT_CHANGE_GUILD					(MessageType_t)((T0_IC_CHAT<<8)|T1_IC_CHAT_CHANGE_GUILD)
#define T_IC_CHAT_CHANGE_GUILD_OK				(MessageType_t)((T0_IC_CHAT<<8)|T1_IC_CHAT_CHANGE_GUILD_OK)
#define T_IC_CHAT_CHANGE_PARTY					(MessageType_t)((T0_IC_CHAT<<8)|T1_IC_CHAT_CHANGE_PARTY)
#define T_IC_CHAT_CHANGE_PARTY_OK				(MessageType_t)((T0_IC_CHAT<<8)|T1_IC_CHAT_CHANGE_PARTY_OK)
#define T_IC_CHAT_CHANGE_CHAT_FLAG				(MessageType_t)((T0_IC_CHAT<<8)|T1_IC_CHAT_CHANGE_CHAT_FLAG)
#define T_IC_CHAT_POSITION						(MessageType_t)((T0_IC_CHAT<<8)|T1_IC_CHAT_POSITION)
#define T_IC_CHAT_BLOCK							(MessageType_t)((T0_IC_CHAT<<8)|T1_IC_CHAT_BLOCK)
#define T_IC_CHAT_GET_BLOCK						(MessageType_t)((T0_IC_CHAT<<8)|T1_IC_CHAT_GET_BLOCK)
#define T_IC_CHAT_GET_BLOCK_OK					(MessageType_t)((T0_IC_CHAT<<8)|T1_IC_CHAT_GET_BLOCK_OK)
#define T_IC_CHAT_BLOCK_YOU						(MessageType_t)((T0_IC_CHAT<<8)|T1_IC_CHAT_BLOCK_YOU)
#define T_IC_CHAT_FRIENDLIST_AND_REJECTLIST_LOADING		(MessageType_t)((T0_IC_CHAT<<8)|T1_IC_CHAT_FRIENDLIST_AND_REJECTLIST_LOADING)
#define T_IC_CHAT_FRIENDLIST_LOADING_OK			(MessageType_t)((T0_IC_CHAT<<8)|T1_IC_CHAT_FRIENDLIST_LOADING_OK)
#define T_IC_CHAT_FRIENDLIST_INSERT				(MessageType_t)((T0_IC_CHAT<<8)|T1_IC_CHAT_FRIENDLIST_INSERT)
#define T_IC_CHAT_FRIENDLIST_INSERT_OK			(MessageType_t)((T0_IC_CHAT<<8)|T1_IC_CHAT_FRIENDLIST_INSERT_OK)
#define T_IC_CHAT_FRIENDLIST_DELETE				(MessageType_t)((T0_IC_CHAT<<8)|T1_IC_CHAT_FRIENDLIST_DELETE)
#define T_IC_CHAT_FRIENDLIST_DELETE_OK			(MessageType_t)((T0_IC_CHAT<<8)|T1_IC_CHAT_FRIENDLIST_DELETE_OK)
#define T_IC_CHAT_FRIENDLIST_REFRESH			(MessageType_t)((T0_IC_CHAT<<8)|T1_IC_CHAT_FRIENDLIST_REFRESH)
#define T_IC_CHAT_FRIENDLIST_REFRESH_OK			(MessageType_t)((T0_IC_CHAT<<8)|T1_IC_CHAT_FRIENDLIST_REFRESH_OK)
#define T_IC_CHAT_REJECTLIST_LOADING_OK			(MessageType_t)((T0_IC_CHAT<<8)|T1_IC_CHAT_REJECTLIST_LOADING_OK)
#define T_IC_CHAT_REJECTLIST_INSERT				(MessageType_t)((T0_IC_CHAT<<8)|T1_IC_CHAT_REJECTLIST_INSERT)
#define T_IC_CHAT_REJECTLIST_INSERT_OK			(MessageType_t)((T0_IC_CHAT<<8)|T1_IC_CHAT_REJECTLIST_INSERT_OK)
#define T_IC_CHAT_REJECTLIST_DELETE				(MessageType_t)((T0_IC_CHAT<<8)|T1_IC_CHAT_REJECTLIST_DELETE)
#define T_IC_CHAT_REJECTLIST_DELETE_OK			(MessageType_t)((T0_IC_CHAT<<8)|T1_IC_CHAT_REJECTLIST_DELETE_OK)
#define T_IC_CHAT_FRIENDLIST_INSERT_NOTIFY		(MessageType_t)((T0_IC_CHAT<<8)|T1_IC_CHAT_FRIENDLIST_INSERT_NOTIFY)	// 2009-01-13 by cmkwon, 친구 등록시 상대방에게 알림 시스템 적용 - 

#define T_IC_CHAT_SELL_ALL						(MessageType_t)((T0_IC_CHAT<<8)|T1_IC_CHAT_SELL_ALL)	// 매매 전체 채팅
#define T_IC_CHAT_CASH_ALL						(MessageType_t)((T0_IC_CHAT<<8)|T1_IC_CHAT_CASH_ALL)	// 유료 전체 채팅
#define T_IC_CHAT_INFLUENCE_ALL					(MessageType_t)((T0_IC_CHAT<<8)|T1_IC_CHAT_INFLUENCE_ALL)	// 세력 전체 채팅 - 세력지도자만 가능
#define T_IC_CHAT_ARENA							(MessageType_t)((T0_IC_CHAT<<8)|T1_IC_CHAT_ARENA)			// 2007-05-02 by dhjin, 아레나 채팅
#define T_IC_CHAT_WAR							(MessageType_t)((T0_IC_CHAT<<8)|T1_IC_CHAT_WAR)				// 2008-05-19 by dhjin, EP3 - 채팅 시스템 변경, 전쟁 채팅
#define T_IC_CHAT_CHATROOM						(MessageType_t)((T0_IC_CHAT<<8)|T1_IC_CHAT_CHATROOM)		// 2008-06-18 by dhjin, EP3 채팅방 - 
#define T_IC_CHAT_INFINITY						(MessageType_t)((T0_IC_CHAT<<8)|T1_IC_CHAT_INFINITY)		// 2009-09-09 ~ 2010 by dhjin, 인피니티 - 인피 채팅
#define T_IC_CHAT_MULTI							(MessageType_t)((T0_IC_CHAT<<8)|T1_IC_CHAT_MULTI)		// 2015-02-11 by silver multi chat
#define T_FI_CHAT_MAP							(MessageType_t)((T0_FI_CHAT<<8)|T1_FI_CHAT_MAP)
#define T_FI_CHAT_REGION						(MessageType_t)((T0_FI_CHAT<<8)|T1_FI_CHAT_REGION)
#define T_FI_CHAT_CHANGE_CHAT_FLAG				(MessageType_t)((T0_FI_CHAT<<8)|T1_FI_CHAT_CHANGE_CHAT_FLAG)
#define T_FI_CHAT_CASH_ALL						(MessageType_t)((T0_FI_CHAT<<8)|T1_FI_CHAT_CASH_ALL)	// 유료 전체 채팅
#define T_FI_CHAT_ARENA							(MessageType_t)((T0_FI_CHAT<<8)|T1_FI_CHAT_ARENA)		// 2007-05-02 by dhjin
#define	T_FI_CHAT_OUTPOST_GUILD					(MessageType_t)((T0_FI_CHAT<<8)|T1_FI_CHAT_OUTPOST_GUILD)	// 2007-10-06 by cmkwon, 전진 기지 소유한 여단장 세력 채팅 가능
#define T_FI_CHAT_INFINITY						(MessageType_t)((T0_FI_CHAT<<8)|T1_FI_CHAT_INFINITY)		// 2009-09-09 ~ 2010 by dhjin, 인피니티 - 인피 채팅


#define T_FI_CHAT_ONLINE_EACHOTHER_FRIEND_COUNT_OK		(MessageType_t)((T0_FI_CHAT<<8)|T1_FI_CHAT_ONLINE_EACHOTHER_FRIEND_COUNT_OK)	// 2006-07-18 by cmkwon, 온라인 친구 카운트
#define T_IC_CHAT_ONLINE_EACHOTHER_FRIEND_COUNT			(MessageType_t)((T0_FI_CHAT<<8)|T1_IC_CHAT_ONLINE_EACHOTHER_FRIEND_COUNT)		// 2008-07-11 by dhjin, EP3 친구목록 -

#define T_FC_CHAT_MAP							(MessageType_t)((T0_FC_CHAT<<8)|T1_FC_CHAT_MAP)
#define T_FC_CHAT_REGION						(MessageType_t)((T0_FC_CHAT<<8)|T1_FC_CHAT_REGION)
#define T_FC_CHAT_CASH_ALL						(MessageType_t)((T0_FC_CHAT<<8)|T1_FC_CHAT_CASH_ALL)
#define T_FC_CHAT_ARENA							(MessageType_t)((T0_FC_CHAT<<8)|T1_FC_CHAT_ARENA)		// 2007-05-02 by dhjin
#define T_FC_CHAT_ALL_INFLUENCE					(MessageType_t)((T0_FC_CHAT<<8)|T1_FC_CHAT_ALL_INFLUENCE)	// 2007-08-09 by cmkwon, 모든 세력에 채팅 전송하기 - 프로토콜타입 추가
#define T_FC_CHAT_OUTPOST_GUILD					(MessageType_t)((T0_FC_CHAT<<8)|T1_FC_CHAT_OUTPOST_GUILD)	// 2007-10-06 by cmkwon, 전진 기지 소유한 여단장 세력 채팅 가능
#define T_FC_CHAT_INFINITY						(MessageType_t)((T0_FC_CHAT<<8)|T1_FC_CHAT_INFINITY)		// 2009-09-09 ~ 2010 by dhjin, 인피니티 - 인피 채팅

#define T_FC_CHARACTER_CREATE					(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_CREATE)
#define T_FC_CHARACTER_CREATE_OK				(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_CREATE_OK)
#define T_FC_CHARACTER_DELETE					(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_DELETE)
#define T_FC_CHARACTER_DELETE_OK				(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_DELETE_OK)
#define T_FC_CHARACTER_GET_CHARACTER			(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_GET_CHARACTER)
#define T_FC_CHARACTER_GET_CHARACTER_OK			(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_GET_CHARACTER_OK)
#define T_FC_CHARACTER_GAMESTART				(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_GAMESTART)
#define T_FC_CHARACTER_GAMESTART_OK				(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_GAMESTART_OK)
#define T_FC_CHARACTER_CONNECT_GAMESTART		(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_CONNECT_GAMESTART)
#define T_FC_CHARACTER_CONNECT_GAMESTART_OK		(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_CONNECT_GAMESTART_OK)
#define T_FC_CHARACTER_GAMEEND					(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_GAMEEND)
#define T_FC_CHARACTER_GAMEEND_OK				(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_GAMEEND_OK)
#define T_FC_CHARACTER_REPAIR					(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_REPAIR)
#define T_FC_CHARACTER_REPAIR_OK				(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_REPAIR_OK)
#define T_FC_CHARACTER_REPAIR_ERR				(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_REPAIR_ERR)
#define T_FC_CHARACTER_DOCKING					(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_DOCKING)
#define T_FC_CHARACTER_UNDOCKING				(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_UNDOCKING)
#define T_FC_CHARACTER_DOCKING_ERR				(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_DOCKING_ERR)
#define T_FC_CHARACTER_GET_OTHER_INFO			(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_GET_OTHER_INFO)
#define T_FC_CHARACTER_GET_OTHER_INFO_OK		(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_GET_OTHER_INFO_OK)
#define T_FC_CHARACTER_GET_MONSTER_INFO_OK		(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_GET_MONSTER_INFO_OK)
#define T_FC_CHARACTER_CHANGE_UNITKIND			(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_CHANGE_UNITKIND)
#define T_FC_CHARACTER_CHANGE_STAT				(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_CHANGE_STAT)
#define T_FC_CHARACTER_CHANGE_TOTALGEAR_STAT	(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_CHANGE_TOTALGEAR_STAT)
#define T_FC_CHARACTER_CHANGE_FRIEND			(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_CHANGE_FRIEND)
#define T_FC_CHARACTER_CHANGE_EXP				(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_CHANGE_EXP)
#define T_FC_CHARACTER_CHANGE_BODYCONDITION		(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_CHANGE_BODYCONDITION)
#define T_FC_CHARACTER_CHANGE_BODYCONDITION_SKILL	(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_CHANGE_BODYCONDITION_SKILL)
#define T_FC_CHARACTER_CHANGE_INFLUENCE_TYPE		(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_CHANGE_INFLUENCE_TYPE)
#define T_FC_CHARACTER_CHANGE_STATUS			(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_CHANGE_STATUS)
#define T_FC_CHARACTER_CHANGE_PKPOINT			(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_CHANGE_PKPOINT)
#define T_FC_CHARACTER_CHANGE_HPDPSPEP			(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_CHANGE_HPDPSPEP)
#define T_FC_CHARACTER_CHANGE_CURRENTHPDPSPEP	(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_CHANGE_CURRENTHPDPSPEP)
#define T_FC_CHARACTER_CHANGE_CURRENTHP			(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_CHANGE_CURRENTHP)
#define T_FC_CHARACTER_CHANGE_CURRENTDP			(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_CHANGE_CURRENTDP)
#define T_FC_CHARACTER_CHANGE_CURRENTSP			(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_CHANGE_CURRENTSP)
#define T_FC_CHARACTER_CHANGE_CURRENTEP			(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_CHANGE_CURRENTEP)
#define T_FC_CHARACTER_CHANGE_MAPNAME			(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_CHANGE_MAPNAME)
#define T_FC_CHARACTER_CHANGE_PETINFO			(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_CHANGE_PETINFO)
#define T_FC_CHARACTER_CHANGE_POSITION			(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_CHANGE_POSITION)
#define T_FC_CHARACTER_CHANGE_LEVEL				(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_CHANGE_LEVEL)
#define T_FC_CHARACTER_USE_BONUSSTAT			(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_USE_BONUSSTAT)
#define T_FC_CHARACTER_USE_BONUSSTAT_OK			(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_USE_BONUSSTAT_OK)		// 2006-09-18 by cmkwon
#define T_FC_CHARACTER_DEAD_NOTIFY				(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_DEAD_NOTIFY)
#define T_FC_CHARACTER_GET_ACCOUNTUNIQUENUMBER	(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_GET_ACCOUNTUNIQUENUMBER)
#define T_FC_CHARACTER_GET_ACCOUNTUNIQUENUMBER_OK	(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_GET_ACCOUNTUNIQUENUMBER_OK)
#define T_FC_CHARACTER_APPLY_COLLISION_DAMAGE	(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_APPLY_COLLISION_DAMAGE)
#define T_FC_CHARACTER_GET_OTHER_MOVE			(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_GET_OTHER_MOVE)
#define T_FC_CHARACTER_DELETE_OTHER_INFO		(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_DELETE_OTHER_INFO)
#define T_FC_CHARACTER_DEAD_GAMESTART			(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_DEAD_GAMESTART)	// C -> F
#define T_FC_CHARACTER_OTHER_REVIVED			(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_OTHER_REVIVED)		// F -> C, 죽었다 되살아났을 때 다른 캐릭터(현재는 파티원)에게 보냄
#define T_FC_CHARACTER_GET_OTHER_RENDER_INFO	(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_GET_OTHER_RENDER_INFO)
#define T_FC_CHARACTER_GET_OTHER_RENDER_INFO_OK	(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_GET_OTHER_RENDER_INFO_OK)
#define T_FC_CHARACTER_CHANGE_BODYCONDITION_ALL	(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_CHANGE_BODYCONDITION_ALL)	// C->F, 강제로 BodyCondition 세팅 요청
#define T_FC_CHARACTER_CHANGE_PROPENSITY			(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_CHANGE_PROPENSITY)		// F->C, // 2005-08-22 by cmkwon, 변경함
#define T_FC_CHARACTER_SHOW_EFFECT					(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_SHOW_EFFECT)			// C->F, 주위에 자신의 이펙트 전송 요청
#define T_FC_CHARACTER_SHOW_EFFECT_OK				(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_SHOW_EFFECT_OK)		// F->C, 주위에 캐릭들에게 전송
#define T_FC_CHARACTER_GET_OTHER_PARAMFACTOR		(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_GET_OTHER_PARAMFACTOR)		// C->F, 해당 캐릭터의 ParamFactor 정보 요청
#define T_FC_CHARACTER_GET_OTHER_PARAMFACTOR_OK		(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_GET_OTHER_PARAMFACTOR_OK)	// F->C, 해당 캐릭터의 ParamFactor 정보 요청 결과
#define T_FC_CHARACTER_SEND_PARAMFACTOR_IN_RANGE	(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_SEND_PARAMFACTOR_IN_RANGE)		// C->F, 자신의 ParamFactor를 주위에 보내도록 요청
//#define T_FC_CHARACTER_GET_OTHER_SKILL_INFO		(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_GET_OTHER_SKILL_INFO)		// C->F
//#define T_FC_CHARACTER_GET_OTHER_SKILL_INFO_OK	(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_GET_OTHER_SKILL_INFO_OK)	// F->C
#define T_FC_CHARACTER_SPEED_HACK_USER				(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_SPEED_HACK_USER)			// C->F
#define T_FC_CHARACTER_CHANGE_CHARACTER_MENT		(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_CHANGE_CHARACTER_MENT)		// F->C(n)
#define T_FC_CHARACTER_GET_CASH_MONEY_COUNT			(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_GET_CASH_MONEY_COUNT)		// C->F
#define T_FC_CHARACTER_GET_CASH_MONEY_COUNT_OK		(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_GET_CASH_MONEY_COUNT_OK)	// F->C
#define T_FC_CHARACTER_CASH_PREMIUM_CARD_INFO		(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_CASH_PREMIUM_CARD_INFO)	// F->C
#define T_FC_CHARACTER_TUTORIAL_SKIP				(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_TUTORIAL_SKIP)	// C->F
#define T_FC_CHARACTER_TUTORIAL_SKIP_OK				(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_TUTORIAL_SKIP_OK)	// F->C
#define T_FC_CHARACTER_CHANGE_CHARACTER_MODE		(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_CHANGE_CHARACTER_MODE)		// C->F
#define T_FC_CHARACTER_CHANGE_CHARACTER_MODE_OK		(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_CHANGE_CHARACTER_MODE_OK)	// F->C(n)
#define T_FC_CHARACTER_FALLING_BY_FUEL_ALLIN		(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_FALLING_BY_FUEL_ALLIN)					// C->F
#define T_FC_CHARACTER_WARP_BY_AGEAR_LANDING_FUEL_ALLIN	(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_WARP_BY_AGEAR_LANDING_FUEL_ALLIN)	// C->F
#define T_FC_CHARACTER_GET_REAL_WEAPON_INFO			(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_GET_REAL_WEAPON_INFO)	// F->C// 2005-12-21 by cmkwon
#define T_FC_CHARACTER_GET_REAL_WEAPON_INFO_OK		(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_GET_REAL_WEAPON_INFO_OK)	// C->F// 2005-12-21 by cmkwon
#define T_FC_CHARACTER_GET_REAL_ENGINE_INFO			(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_GET_REAL_ENGINE_INFO)		// F->C// 2005-12-21 by cmkwon
#define T_FC_CHARACTER_GET_REAL_ENGINE_INFO_OK		(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_GET_REAL_ENGINE_INFO_OK)	// C->F// 2005-12-21 by cmkwon
#define T_FC_CHARACTER_GET_REAL_TOTAL_WEIGHT		(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_GET_REAL_TOTAL_WEIGHT)		// F->C// 2005-12-21 by cmkwon
#define T_FC_CHARACTER_GET_REAL_TOTAL_WEIGHT_OK		(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_GET_REAL_TOTAL_WEIGHT_OK)	// C->F// 2005-12-21 by cmkwon
#define T_FC_CHARACTER_MEMORY_HACK_USER				(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_MEMORY_HACK_USER)			// C->F// 2005-12-21 by cmkwon
#define T_FC_CHARACTER_UPDATE_SUBLEADER				(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_UPDATE_SUBLEADER)			// C->F// 2007-02-14 by dhjin
#define T_FC_CHARACTER_UPDATE_SUBLEADER_OK			(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_UPDATE_SUBLEADER_OK)		// F->C, 2007-10-06 by dhjin
#define T_FC_CHARACTER_OBSERVER_START				(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_OBSERVER_START)			// C->F, 2007-03-27 by dhjin
#define T_FC_CHARACTER_OBSERVER_START_OK			(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_OBSERVER_START_OK)			// F->C, 2007-03-27 by dhjin
#define T_FC_CHARACTER_OBSERVER_END					(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_OBSERVER_END)				// C->F, 2007-03-27 by dhjin
#define T_FC_CHARACTER_OBSERVER_END_OK				(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_OBSERVER_END_OK)			// F->C, 2007-03-27 by dhjin
#define T_FC_CHARACTER_OBSERVER_INFO				(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_OBSERVER_INFO)			// F->C, 2007-03-27 by dhjin
#define T_FC_CHARACTER_OBSERVER_REG					(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_OBSERVER_REG)			// F->C, 2007-03-27 by dhjin
#define T_FC_CHARACTER_SHOW_MAP_EFFECT				(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_SHOW_MAP_EFFECT)			// C->F, // 2007-04-20 by cmkwon
#define T_FC_CHARACTER_SHOW_MAP_EFFECT_OK			(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_SHOW_MAP_EFFECT_OK)		// F->C(n), // 2007-04-20 by cmkwon
#define T_FC_CHARACTER_PAY_WARPOINT					(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_PAY_WARPOINT)			// F->C(n), // 2007-04-20 by cmkwon
#define T_FC_CHARACTER_WATCH_INFO					(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_WATCH_INFO)			// F->C, 2007-03-27 by dhjin
// 2008-01-10 by dhjin,  아레나 통합 - 아레나 종료 후 다시 필드 서버 게임 시작
#define T_FC_CHARACTER_READY_GAMESTART_FROM_ARENA_TO_MAINSERVER 	(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_READY_GAMESTART_FROM_ARENA_TO_MAINSERVER)			// C->F, // 2008-01-31 by dhjin
#define T_FC_CHARACTER_READY_GAMESTART_FROM_ARENA_TO_MAINSERVER_OK 	(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_READY_GAMESTART_FROM_ARENA_TO_MAINSERVER_OK)			// F->C, // 2008-01-31 by dhjin
#define T_FC_CHARACTER_GAMESTART_FROM_ARENA_TO_MAINSERVER 			(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_GAMESTART_FROM_ARENA_TO_MAINSERVER)			// C->F, // 2008-01-10 by dhjin
#define T_FC_CHARACTER_GET_USER_INFO				(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_GET_USER_INFO)					// C->F, // 2008-06-23 by dhjin, EP3 유저정보옵션 -
#define T_FC_CHARACTER_GET_USER_INFO_OK				(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_GET_USER_INFO_OK)				// F->C, // 2008-06-23 by dhjin, EP3 유저정보옵션 -
#ifdef _INET_LINK_CHAT
#define T_FC_CHARACTER_GET_USER_ITEM_INFO_OK_DONE			(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_GET_USER_ITEM_INFO_OK_DONE)	// F->C
#endif
#define T_FC_CHARACTER_CHANGE_INFO_OPTION_SECRET	(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_CHANGE_INFO_OPTION_SECRET)		// C->F, // 2008-06-23 by dhjin, EP3 유저정보옵션 -
#define T_FC_CHARACTER_CHANGE_INFO_OPTION_SECRET_OK	(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_CHANGE_INFO_OPTION_SECRET_OK)	// F->C, // 2008-06-23 by dhjin, EP3 유저정보옵션 -
#define T_FC_CHARACTER_CHANGE_NICKNAME				(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_CHANGE_NICKNAME)		// C->F, // 2009-02-12 by cmkwon, EP3-3 월드랭킹시스템 구현 - 
#define T_FC_CHARACTER_CHANGE_NICKNAME_OK			(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_CHANGE_NICKNAME_OK)	// F->C, // 2009-02-12 by cmkwon, EP3-3 월드랭킹시스템 구현 - 
#define T_FC_CHARACTER_CHANGE_START_CITY_MAPINDEX		(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_CHANGE_START_CITY_MAPINDEX)	// C->F, // 2009-10-12 by cmkwon, 프리스카 제거 방안 적용 - 
#define T_FC_CHARACTER_CHANGE_START_CITY_MAPINDEX_OK	(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_CHANGE_START_CITY_MAPINDEX_OK)	// F->C, // 2009-10-12 by cmkwon, 프리스카 제거 방안 적용 - 
#define T_FC_CHARACTER_CHANGE_ADDED_INVENTORY_COUNT		(MessageType_t)((T0_FC_CHARACTER<<8)|T1_FC_CHARACTER_CHANGE_ADDED_INVENTORY_COUNT)	// F->C, // 2009-11-02 by cmkwon, 캐쉬(인벤/창고 확장) 아이템 추가 구현 - 

#define T_FN_CHARACTER_CHANGE_UNITKIND			(MessageType_t)((T0_FN_CHARACTER<<8)|T1_FN_CHARACTER_CHANGE_UNITKIND)
#define T_FN_CHARACTER_CHANGE_BODYCONDITION		(MessageType_t)((T0_FN_CHARACTER<<8)|T1_FN_CHARACTER_CHANGE_BODYCONDITION)
#define T_FN_CHARACTER_CHANGE_HPDPSPEP			(MessageType_t)((T0_FN_CHARACTER<<8)|T1_FN_CHARACTER_CHANGE_HPDPSPEP)
#define T_FN_CHARACTER_CHANGE_CURRENTHPDPSPEP	(MessageType_t)((T0_FN_CHARACTER<<8)|T1_FN_CHARACTER_CHANGE_CURRENTHPDPSPEP)
#define T_FN_CHARACTER_CHANGE_MAPNAME			(MessageType_t)((T0_FN_CHARACTER<<8)|T1_FN_CHARACTER_CHANGE_MAPNAME)
#define T_FN_CHARACTER_CHANGE_POSITION			(MessageType_t)((T0_FN_CHARACTER<<8)|T1_FN_CHARACTER_CHANGE_POSITION)
#define T_FN_CHARACTER_CHANGE_STEALTHSTATE		(MessageType_t)((T0_FN_CHARACTER<<8)|T1_FN_CHARACTER_CHANGE_STEALTHSTATE)
#define T_FN_CHARACTER_CHANGE_CHARACTER_MODE_OK	(MessageType_t)((T0_FN_CHARACTER<<8)|T1_FN_CHARACTER_CHANGE_CHARACTER_MODE_OK)
#define T_FN_CHARACTER_CHANGE_INFLUENCE_TYPE	(MessageType_t)((T0_FN_CHARACTER<<8)|T1_FN_CHARACTER_CHANGE_INFLUENCE_TYPE)		// F->I, 2005-12-03 by cmkwon
#define T_FN_CHARACTER_CHANGE_INVISIBLE			(MessageType_t)((T0_FN_CHARACTER<<8)|T1_FN_CHARACTER_CHANGE_INVISIBLE)			// F->N, 2006-11-27 by dhjin

#define T_FC_MOVE								(MessageType_t)((T0_FC_MOVE<<8)|T1_FC_MOVE)
#define T_FC_MOVE_OK							(MessageType_t)((T0_FC_MOVE<<8)|T1_FC_MOVE_OK)
#define T_FC_MOVE_LOCKON						(MessageType_t)((T0_FC_MOVE<<8)|T1_FC_MOVE_LOCKON)
#define T_FC_MOVE_LOCKON_OK						(MessageType_t)((T0_FC_MOVE<<8)|T1_FC_MOVE_LOCKON_OK)
#define T_FC_MOVE_UNLOCKON						(MessageType_t)((T0_FC_MOVE<<8)|T1_FC_MOVE_UNLOCKON)
#define T_FC_MOVE_UNLOCKON_OK					(MessageType_t)((T0_FC_MOVE<<8)|T1_FC_MOVE_UNLOCKON_OK)
#define T_FC_MOVE_LANDING						(MessageType_t)((T0_FC_MOVE<<8)|T1_FC_MOVE_LANDING)
#define T_FC_MOVE_LANDING_OK					(MessageType_t)((T0_FC_MOVE<<8)|T1_FC_MOVE_LANDING_OK)
#define T_FC_MOVE_LANDING_DONE					(MessageType_t)((T0_FC_MOVE<<8)|T1_FC_MOVE_LANDING_DONE)	// C->F, 착륙 완료를 알림
#define T_FC_MOVE_TAKEOFF						(MessageType_t)((T0_FC_MOVE<<8)|T1_FC_MOVE_TAKEOFF)
#define T_FC_MOVE_TAKEOFF_OK					(MessageType_t)((T0_FC_MOVE<<8)|T1_FC_MOVE_TAKEOFF_OK)
#define T_FC_MISSILE_MOVE_OK					(MessageType_t)((T0_FC_MOVE<<8)|T1_FC_MISSILE_MOVE_OK)
#define T_FC_MOVE_TARGET						(MessageType_t)((T0_FC_MOVE<<8)|T1_FC_MOVE_TARGET)
#define T_FC_MOVE_WEAPON_VEL					(MessageType_t)((T0_FC_MOVE<<8)|T1_FC_MOVE_WEAPON_VEL)		// C->F, 무기의 방향의 움직임 전송
#define T_FC_MOVE_WEAPON_VEL_OK					(MessageType_t)((T0_FC_MOVE<<8)|T1_FC_MOVE_WEAPON_VEL_OK)	// F->C_in_range, 무기의 방향의 움직임 전송
#define T_FC_MOVE_ROLLING						(MessageType_t)((T0_FC_MOVE<<8)|T1_FC_MOVE_ROLLING)			// F->C, 롤링 사용 요청
#define T_FC_MOVE_ROLLING_OK					(MessageType_t)((T0_FC_MOVE<<8)|T1_FC_MOVE_ROLLING_OK)		// F->C, 롤링 사용 허가

#define T_FC_MOVE_HACKSHIELD_GuidReqMsg			(MessageType_t)((T0_FC_MOVE<<8)|T1_FC_MOVE_HACKSHIELD_GuidReqMsg)	// F->C, 2006-06-05 by cmkwon, Anlab - HackShield
#define T_FC_MOVE_HACKSHIELD_GuidAckMsg			(MessageType_t)((T0_FC_MOVE<<8)|T1_FC_MOVE_HACKSHIELD_GuidAckMsg)	// C->F, 2006-06-05 by cmkwon, Anlab - HackShield
#define T_FC_MOVE_HACKSHIELD_CRCReqMsg			(MessageType_t)((T0_FC_MOVE<<8)|T1_FC_MOVE_HACKSHIELD_CRCReqMsg)	// F->C, 2006-06-05 by cmkwon, Anlab - HackShield
#define T_FC_MOVE_HACKSHIELD_CRCAckMsg			(MessageType_t)((T0_FC_MOVE<<8)|T1_FC_MOVE_HACKSHIELD_CRCAckMsg)	// C->F, 2006-06-05 by cmkwon, Anlab - HackShield
#define T_FC_MOVE_HACKSHIELD_HACKING_CLIENT		(MessageType_t)((T0_FC_MOVE<<8)|T1_FC_MOVE_HACKSHIELD_HACKING_CLIENT)	// 2006-06-05 by cmkwon, Anlab - HackShield
#define T_FC_MOVE_XIGNCODE_REQ_SCAN_INIT		(MessageType_t)((T0_FC_MOVE<<8)|T1_FC_MOVE_XIGNCODE_REQ_SCAN_INIT)		// 2008-11-28 by cmkwon, 대만 Netpower_Tpe XignCode(게임가드) 적용 - S->C(1)
#define T_FC_MOVE_XIGNCODE_REQ_SCAN_INIT_OK		(MessageType_t)((T0_FC_MOVE<<8)|T1_FC_MOVE_XIGNCODE_REQ_SCAN_INIT_OK)	// 2008-11-28 by cmkwon, 대만 Netpower_Tpe XignCode(게임가드) 적용 - C->S
#define T_FC_MOVE_XIGNCODE_REQ_SCAN_CHECK		(MessageType_t)((T0_FC_MOVE<<8)|T1_FC_MOVE_XIGNCODE_REQ_SCAN_CHECK)		// 2008-11-28 by cmkwon, 대만 Netpower_Tpe XignCode(게임가드) 적용 - S->C(1)
#define T_FC_MOVE_XIGNCODE_REQ_SCAN_CHECK_OK	(MessageType_t)((T0_FC_MOVE<<8)|T1_FC_MOVE_XIGNCODE_REQ_SCAN_CHECK_OK)	// 2008-11-28 by cmkwon, 대만 Netpower_Tpe XignCode(게임가드) 적용 - C->S
#define T_FC_MOVE_NPROTECT_REQ_AUTH_DATA		(MessageType_t)((T0_FC_MOVE<<8)|T1_FC_MOVE_NPROTECT_REQ_AUTH_DATA)		// 2009-03-09 by cmkwon, 일본 Arario nProtect에 CS인증 적용하기 - S->C(1)
#define T_FC_MOVE_NPROTECT_REQ_AUTH_DATA_OK		(MessageType_t)((T0_FC_MOVE<<8)|T1_FC_MOVE_NPROTECT_REQ_AUTH_DATA_OK)	// 2009-03-09 by cmkwon, 일본 Arario nProtect에 CS인증 적용하기 - C->S
#define T_FC_MOVE_XTRAP_REQ_STEP				(MessageType_t)((T0_FC_MOVE<<8)|T1_FC_MOVE_XTRAP_REQ_STEP)			// 2009-10-06 by cmkwon, 베트남 게임 가드 X-TRAP으로 변경 - S->C(1)
#define T_FC_MOVE_XTRAP_REQ_STEP_OK				(MessageType_t)((T0_FC_MOVE<<8)|T1_FC_MOVE_XTRAP_REQ_STEP_OK)		// 2009-10-06 by cmkwon, 베트남 게임 가드 X-TRAP으로 변경 - C(1)->S
#define T_FC_MOVE_APEX_REQ_APEXDATA				(MessageType_t)((T0_FC_MOVE<<8)|T1_FC_MOVE_APEX_REQ_APEXDATA)		// 2009-11-04 by cmkwon, 태국 게임가드 Apex로 변경 - S->C(1)
#define T_FC_MOVE_APEX_REQ_APEXDATA_OK			(MessageType_t)((T0_FC_MOVE<<8)|T1_FC_MOVE_APEX_REQ_APEXDATA_OK)	// 2009-11-04 by cmkwon, 태국 게임가드 Apex로 변경 - C(1)->S

#define T_FN_MONSTER_MOVE						(MessageType_t)((T0_FN_MOVE<<8)|T1_FN_MONSTER_MOVE)
#define T_FN_MOVE_OK							(MessageType_t)((T0_FN_MOVE<<8)|T1_FN_MOVE_OK)
#define T_FN_MOVE_LOCKON						(MessageType_t)((T0_FN_MOVE<<8)|T1_FN_MOVE_LOCKON)
#define T_FN_MOVE_LOCKON_OK						(MessageType_t)((T0_FN_MOVE<<8)|T1_FN_MOVE_LOCKON_OK)
#define T_FN_MOVE_LOCKON_AVOID					(MessageType_t)((T0_FN_MOVE<<8)|T1_FN_MOVE_LOCKON_AVOID)
#define T_FN_MOVE_LOCKON_AVOID_OK				(MessageType_t)((T0_FN_MOVE<<8)|T1_FN_MOVE_LOCKON_AVOID_OK)
#define T_FN_MISSILE_MOVE						(MessageType_t)((T0_FN_MOVE<<8)|T1_FN_MISSILE_MOVE)
#define T_FN_MONSTER_HPRECOVERY					(MessageType_t)((T0_FN_MOVE<<8)|T1_FN_MONSTER_HPRECOVERY)
#define T_FN_MONSTER_HIDE						(MessageType_t)((T0_FN_MOVE<<8)|T1_FN_MONSTER_HIDE)
#define T_FN_MONSTER_SHOW						(MessageType_t)((T0_FN_MOVE<<8)|T1_FN_MONSTER_SHOW)

#define T_FC_BATTLE_ATTACK						(MessageType_t)((T0_FC_BATTLE<<8)|T1_FC_BATTLE_ATTACK)						// C->F
#define T_FC_BATTLE_ATTACK_OK					(MessageType_t)((T0_FC_BATTLE<<8)|T1_FC_BATTLE_ATTACK_OK)					// F->C_in_range
#define T_FC_BATTLE_ATTACK_FIND					(MessageType_t)((T0_FC_BATTLE<<8)|T1_FC_BATTLE_ATTACK_FIND)					// C->F
#define T_FC_BATTLE_ATTACK_FIND_OK				(MessageType_t)((T0_FC_BATTLE<<8)|T1_FC_BATTLE_ATTACK_FIND_OK)				// F->C_in_range
#define T_FC_BATTLE_DROP_MINE					(MessageType_t)((T0_FC_BATTLE<<8)|T1_FC_BATTLE_DROP_MINE)					// C->F
#define T_FC_BATTLE_DROP_MINE_OK				(MessageType_t)((T0_FC_BATTLE<<8)|T1_FC_BATTLE_DROP_MINE_OK)				// F->C_in_range, 아이템 보여주기
#define T_FC_BATTLE_MINE_ATTACK					(MessageType_t)((T0_FC_BATTLE<<8)|T1_FC_BATTLE_MINE_ATTACK)					// C->F
#define T_FC_BATTLE_MINE_ATTACK_OK				(MessageType_t)((T0_FC_BATTLE<<8)|T1_FC_BATTLE_MINE_ATTACK_OK)				// F->C_in_range
#define T_FC_BATTLE_MINE_ATTACK_FIND			(MessageType_t)((T0_FC_BATTLE<<8)|T1_FC_BATTLE_MINE_ATTACK_FIND)			// C->F
#define T_FC_BATTLE_MINE_ATTACK_FIND_OK			(MessageType_t)((T0_FC_BATTLE<<8)|T1_FC_BATTLE_MINE_ATTACK_FIND_OK)			// F->C_in_range
#define T_FC_BATTLE_REQUEST_PK					(MessageType_t)((T0_FC_BATTLE<<8)|T1_FC_BATTLE_REQUEST_PK)					// C->F, client의 PK 요청
#define T_FC_BATTLE_REQUEST_PK_OK				(MessageType_t)((T0_FC_BATTLE<<8)|T1_FC_BATTLE_REQUEST_PK_OK)				// F->C, PK 요청 승낙
#define T_FC_BATTLE_CANCEL_PK					(MessageType_t)((T0_FC_BATTLE<<8)|T1_FC_BATTLE_CANCEL_PK)					// F->C, PK 해제
#define T_FC_BATTLE_REQUEST_P2P_PK				(MessageType_t)((T0_FC_BATTLE<<8)|T1_FC_BATTLE_REQUEST_P2P_PK)				// C->F, 일대일 PK 요청
#define T_FC_BATTLE_REQUEST_P2P_PK_OK			(MessageType_t)((T0_FC_BATTLE<<8)|T1_FC_BATTLE_REQUEST_P2P_PK_OK)			// F->C, 일대일 PK 요청
#define T_FC_BATTLE_ACCEPT_REQUEST_P2P_PK		(MessageType_t)((T0_FC_BATTLE<<8)|T1_FC_BATTLE_ACCEPT_REQUEST_P2P_PK)		// C->F, 일대일 PK 승낙
#define T_FC_BATTLE_ACCEPT_REQUEST_P2P_PK_OK	(MessageType_t)((T0_FC_BATTLE<<8)|T1_FC_BATTLE_ACCEPT_REQUEST_P2P_PK_OK)	// F->C, 일대일 PK 승낙
#define T_FC_BATTLE_REJECT_REQUEST_P2P_PK		(MessageType_t)((T0_FC_BATTLE<<8)|T1_FC_BATTLE_REJECT_REQUEST_P2P_PK)		// C->F, 일대일 PK 거절
#define T_FC_BATTLE_REJECT_REQUEST_P2P_PK_OK	(MessageType_t)((T0_FC_BATTLE<<8)|T1_FC_BATTLE_REJECT_REQUEST_P2P_PK_OK)	// F->C, 일대일 PK 거절
#define T_FC_BATTLE_SURRENDER_P2P_PK			(MessageType_t)((T0_FC_BATTLE<<8)|T1_FC_BATTLE_SURRENDER_P2P_PK)			// C->F, 일대일 PK 항복
#define T_FC_BATTLE_SURRENDER_P2P_PK_OK			(MessageType_t)((T0_FC_BATTLE<<8)|T1_FC_BATTLE_SURRENDER_P2P_PK_OK)			// F->C, 일대일 PK 항복
#define T_FC_BATTLE_ACCEPT_SURRENDER_P2P_PK		(MessageType_t)((T0_FC_BATTLE<<8)|T1_FC_BATTLE_ACCEPT_SURRENDER_P2P_PK)		// C->F, 일대일 PK 항복 승낙
#define T_FC_BATTLE_REJECT_SURRENDER_P2P_PK		(MessageType_t)((T0_FC_BATTLE<<8)|T1_FC_BATTLE_REJECT_SURRENDER_P2P_PK)		// C->F, 일대일 PK 항복 거절
#define T_FC_BATTLE_REJECT_SURRENDER_P2P_PK_OK	(MessageType_t)((T0_FC_BATTLE<<8)|T1_FC_BATTLE_REJECT_SURRENDER_P2P_PK_OK)	// F->C, 일대일 PK 항복 거절
#define T_FC_BATTLE_END_P2P_PK					(MessageType_t)((T0_FC_BATTLE<<8)|T1_FC_BATTLE_END_P2P_PK)					// F->C, PK 종료
#define T_FC_BATTLE_ATTACK_EXPLODE_ITEM			(MessageType_t)((T0_FC_BATTLE<<8)|T1_FC_BATTLE_ATTACK_EXPLODE_ITEM)
#define T_FC_BATTLE_ATTACK_HIDE_ITEM			(MessageType_t)((T0_FC_BATTLE<<8)|T1_FC_BATTLE_ATTACK_HIDE_ITEM)
#define T_FC_BATTLE_ATTACK_EXPLODE_ITEM_W_KIND	(MessageType_t)((T0_FC_BATTLE<<8)|T1_FC_BATTLE_ATTACK_EXPLODE_ITEM_W_KIND)
#define T_FC_BATTLE_ATTACK_HIDE_ITEM_W_KIND		(MessageType_t)((T0_FC_BATTLE<<8)|T1_FC_BATTLE_ATTACK_HIDE_ITEM_W_KIND)
#define T_FC_BATTLE_TOGGLE_SHIELD				(MessageType_t)((T0_FC_BATTLE<<8)|T1_FC_BATTLE_TOGGLE_SHIELD)				// No Body
#define T_FC_BATTLE_TOGGLE_SHIELD_RESULT		(MessageType_t)((T0_FC_BATTLE<<8)|T1_FC_BATTLE_TOGGLE_SHIELD_RESULT)
#define T_FC_BATTLE_DROP_DUMMY					(MessageType_t)((T0_FC_BATTLE<<8)|T1_FC_BATTLE_DROP_DUMMY)
#define T_FC_BATTLE_DROP_DUMMY_OK				(MessageType_t)((T0_FC_BATTLE<<8)|T1_FC_BATTLE_DROP_DUMMY_OK)
#define T_FC_BATTLE_DROP_FIXER					(MessageType_t)((T0_FC_BATTLE<<8)|T1_FC_BATTLE_DROP_FIXER)
#define T_FC_BATTLE_DROP_FIXER_OK				(MessageType_t)((T0_FC_BATTLE<<8)|T1_FC_BATTLE_DROP_FIXER_OK)
#define T_FC_BATTLE_PRI_BULLET_RELOADED			(MessageType_t)((T0_FC_BATTLE<<8)|T1_FC_BATTLE_PRI_BULLET_RELOADED)	// 1형 무기의 탄알이 리로드되었음
#define T_FC_BATTLE_SEC_BULLET_RELOADED			(MessageType_t)((T0_FC_BATTLE<<8)|T1_FC_BATTLE_SEC_BULLET_RELOADED)	// 2형 무기의 탄알이 리로드되었음
#define T_FC_BATTLE_SHIELD_DAMAGE				(MessageType_t)((T0_FC_BATTLE<<8)|T1_FC_BATTLE_SHIELD_DAMAGE)
#define T_FC_BATTLE_TOGGLE_DECOY				(MessageType_t)((T0_FC_BATTLE<<8)|T1_FC_BATTLE_TOGGLE_DECOY)		// C->F, No Body
#define T_FC_BATTLE_TOGGLE_DECOY_OK				(MessageType_t)((T0_FC_BATTLE<<8)|T1_FC_BATTLE_TOGGLE_DECOY_OK)		// F->C
#define T_FC_BATTLE_SHOW_DAMAGE					(MessageType_t)((T0_FC_BATTLE<<8)|T1_FC_BATTLE_SHOW_DAMAGE)			// F->C, 공격 데미지를 표시함
#define T_FC_BATTLE_ATTACK_EVASION				(MessageType_t)((T0_FC_BATTLE<<8)|T1_FC_BATTLE_ATTACK_EVASION)	// C->F, // 2005-12-12 by cmkwon
#define T_FC_BATTLE_ATTACK_EVASION_OK			(MessageType_t)((T0_FC_BATTLE<<8)|T1_FC_BATTLE_ATTACK_EVASION_OK)	// F->C(1), // 2005-12-12 by cmkwon
#define T_FC_BATTLE_DELETE_DUMMY_OK				(MessageType_t)((T0_FC_BATTLE<<8)|T1_FC_BATTLE_DELETE_DUMMY_OK)		// 2006-12-04 by dhjin, F->C(n)
#define T_FC_BATTLE_EXPLODE_DUMMY_OK			(MessageType_t)((T0_FC_BATTLE<<8)|T1_FC_BATTLE_EXPLODE_DUMMY_OK)	// 2006-12-04 by dhjin, F->C(n)


#define T_FN_BATTLE_ATTACK_PRIMARY				(MessageType_t)((T0_FN_BATTLE<<8)|T1_FN_BATTLE_ATTACK_PRIMARY)
#define T_FN_BATTLE_ATTACK_RESULT_PRIMARY		(MessageType_t)((T0_FN_BATTLE<<8)|T1_FN_BATTLE_ATTACK_RESULT_PRIMARY)
#define T_FN_BATTLE_ATTACK_SECONDARY			(MessageType_t)((T0_FN_BATTLE<<8)|T1_FN_BATTLE_ATTACK_SECONDARY)
#define T_FN_BATTLE_ATTACK_RESULT_SECONDARY		(MessageType_t)((T0_FN_BATTLE<<8)|T1_FN_BATTLE_ATTACK_RESULT_SECONDARY)
#define T_FN_BATTLE_ATTACK_FIND					(MessageType_t)((T0_FN_BATTLE<<8)|T1_FN_BATTLE_ATTACK_FIND)
#define T_FN_BATTLE_ATTACK_FIND_RESULT			(MessageType_t)((T0_FN_BATTLE<<8)|T1_FN_BATTLE_ATTACK_FIND_RESULT)
#define T_FN_BATTLE_SET_ATTACK_CHARACTER		(MessageType_t)((T0_FN_BATTLE<<8)|T1_FN_BATTLE_SET_ATTACK_CHARACTER)
#define T_FN_BATTLE_DROP_FIXER					(MessageType_t)((T0_FN_BATTLE<<8)|T1_FN_BATTLE_DROP_FIXER)					// F -> N
#define T_FN_BATTLE_DROP_FIXER_OK				(MessageType_t)((T0_FN_BATTLE<<8)|T1_FN_BATTLE_DROP_FIXER_OK)				// N -> F
#define T_FN_BATTLE_ATTACK_HIDE_ITEM_W_KIND		(MessageType_t)((T0_FN_BATTLE<<8)|T1_FN_BATTLE_ATTACK_HIDE_ITEM_W_KIND)

#define T_FC_PARTY_CREATE_OK						(MessageType_t)((T0_FC_PARTY<<8)|T1_FC_PARTY_CREATE_OK)
#define T_FC_PARTY_REQUEST_INVITE					(MessageType_t)((T0_FC_PARTY<<8)|T1_FC_PARTY_REQUEST_INVITE)
#define T_FC_PARTY_REQUEST_INVITE_QUESTION			(MessageType_t)((T0_FC_PARTY<<8)|T1_FC_PARTY_REQUEST_INVITE_QUESTION)
#define T_FC_PARTY_ACCEPT_INVITE					(MessageType_t)((T0_FC_PARTY<<8)|T1_FC_PARTY_ACCEPT_INVITE)
#define T_FC_PARTY_REJECT_INVITE					(MessageType_t)((T0_FC_PARTY<<8)|T1_FC_PARTY_REJECT_INVITE)
#define T_FC_PARTY_REJECT_INVITE_OK					(MessageType_t)((T0_FC_PARTY<<8)|T1_FC_PARTY_REJECT_INVITE_OK)
#define T_FC_PARTY_GET_MEMBER						(MessageType_t)((T0_FC_PARTY<<8)|T1_FC_PARTY_GET_MEMBER)
#define T_FC_PARTY_PUT_MEMBER						(MessageType_t)((T0_FC_PARTY<<8)|T1_FC_PARTY_PUT_MEMBER)
#define T_FC_PARTY_GET_ALL_MEMBER					(MessageType_t)((T0_FC_PARTY<<8)|T1_FC_PARTY_GET_ALL_MEMBER)
#define T_FC_PARTY_PUT_ALL_MEMBER					(MessageType_t)((T0_FC_PARTY<<8)|T1_FC_PARTY_PUT_ALL_MEMBER)
#define T_FC_PARTY_UPDATE_MEMBER_INFO_ALL			(MessageType_t)((T0_FC_PARTY<<8)|T1_FC_PARTY_UPDATE_MEMBER_INFO_ALL)
#define T_FC_PARTY_UPDATE_MEMBER_INFO_LEVEL			(MessageType_t)((T0_FC_PARTY<<8)|T1_FC_PARTY_UPDATE_MEMBER_INFO_LEVEL)
#define T_FC_PARTY_UPDATE_MEMBER_INFO_HP			(MessageType_t)((T0_FC_PARTY<<8)|T1_FC_PARTY_UPDATE_MEMBER_INFO_HP)
#define T_FC_PARTY_UPDATE_MEMBER_INFO_CURRENT_HP	(MessageType_t)((T0_FC_PARTY<<8)|T1_FC_PARTY_UPDATE_MEMBER_INFO_CURRENT_HP)
#define T_FC_PARTY_UPDATE_MEMBER_INFO_DP			(MessageType_t)((T0_FC_PARTY<<8)|T1_FC_PARTY_UPDATE_MEMBER_INFO_DP)
#define T_FC_PARTY_UPDATE_MEMBER_INFO_CURRENT_DP	(MessageType_t)((T0_FC_PARTY<<8)|T1_FC_PARTY_UPDATE_MEMBER_INFO_CURRENT_DP)
#define T_FC_PARTY_UPDATE_MEMBER_INFO_SP			(MessageType_t)((T0_FC_PARTY<<8)|T1_FC_PARTY_UPDATE_MEMBER_INFO_SP)
#define T_FC_PARTY_UPDATE_MEMBER_INFO_CURRENT_SP	(MessageType_t)((T0_FC_PARTY<<8)|T1_FC_PARTY_UPDATE_MEMBER_INFO_CURRENT_SP)
#define T_FC_PARTY_UPDATE_MEMBER_INFO_EP			(MessageType_t)((T0_FC_PARTY<<8)|T1_FC_PARTY_UPDATE_MEMBER_INFO_EP)
#define T_FC_PARTY_UPDATE_MEMBER_INFO_CURRENT_EP	(MessageType_t)((T0_FC_PARTY<<8)|T1_FC_PARTY_UPDATE_MEMBER_INFO_CURRENT_EP)
#define T_FC_PARTY_UPDATE_MEMBER_INFO_BODYCONDITION	(MessageType_t)((T0_FC_PARTY<<8)|T1_FC_PARTY_UPDATE_MEMBER_INFO_BODYCONDITION)
#define T_FC_PARTY_REQUEST_PARTY_WARP				(MessageType_t)((T0_FC_PARTY<<8)|T1_FC_PARTY_REQUEST_PARTY_WARP)
#define T_FC_PARTY_REQUEST_PARTY_WARP_WITH_MAP_NAME	(MessageType_t)((T0_FC_PARTY<<8)|T1_FC_PARTY_REQUEST_PARTY_WARP_WITH_MAP_NAME)
#define T_FC_PARTY_REQUEST_PARTY_OBJECT_EVENT		(MessageType_t)((T0_FC_PARTY<<8)|T1_FC_PARTY_REQUEST_PARTY_OBJECT_EVENT)
#define T_FC_PARTY_GET_OTHER_MOVE					(MessageType_t)((T0_FC_PARTY<<8)|T1_FC_PARTY_GET_OTHER_MOVE)	// 파티원의 MOVE_OK 요청
#define T_FC_PARTY_BATTLE_START						(MessageType_t)((T0_FC_PARTY<<8)|T1_FC_PARTY_BATTLE_START)
#define T_FC_PARTY_BATTLE_END						(MessageType_t)((T0_FC_PARTY<<8)|T1_FC_PARTY_BATTLE_END)
#define T_FC_PARTY_PUT_ITEM_OTHER					(MessageType_t)((T0_FC_PARTY<<8)|T1_FC_PARTY_PUT_ITEM_OTHER)	// F->C, 다른 파티원의 아이템 취득 정보 전송

#define T_FI_PARTY_CREATE_OK						(MessageType_t)((T0_FI_PARTY<<8)|T1_FI_PARTY_CREATE_OK)
#define T_FI_PARTY_ACCEPT_INVITE_OK					(MessageType_t)((T0_FI_PARTY<<8)|T1_FI_PARTY_ACCEPT_INVITE_OK)
#define T_FI_PARTY_BAN_MEMBER_OK					(MessageType_t)((T0_FI_PARTY<<8)|T1_FI_PARTY_BAN_MEMBER_OK)
#define T_FI_PARTY_LEAVE_OK							(MessageType_t)((T0_FI_PARTY<<8)|T1_FI_PARTY_LEAVE_OK)
#define T_FI_PARTY_TRANSFER_MASTER_OK				(MessageType_t)((T0_FI_PARTY<<8)|T1_FI_PARTY_TRANSFER_MASTER_OK)
#define T_FI_PARTY_DISMEMBER_OK						(MessageType_t)((T0_FI_PARTY<<8)|T1_FI_PARTY_DISMEMBER_OK)
#define T_FI_PARTY_CHANGE_FLIGHT_FORMATION_OK		(MessageType_t)((T0_FI_PARTY<<8)|T1_FI_PARTY_CHANGE_FLIGHT_FORMATION_OK)
#define T_FI_PARTY_CHANGE_FLIGHT_POSITION			(MessageType_t)((T0_FI_PARTY<<8)|T1_FI_PARTY_CHANGE_FLIGHT_POSITION)	// C -> F -> I,All
#define T_FI_PARTY_CANCEL_FLIGHT_POSITION			(MessageType_t)((T0_FI_PARTY<<8)|T1_FI_PARTY_CANCEL_FLIGHT_POSITION)	// C -> F -> I,All
#define T_FI_PARTY_NOTIFY_BATTLE_PARTY				(MessageType_t)((T0_FI_PARTY<<8)|T1_FI_PARTY_NOTIFY_BATTLE_PARTY)		// F->I, 파티전을 알림
#define T_FI_PARTY_NOTIFY_BATTLE_PARTY_OK			(MessageType_t)((T0_FI_PARTY<<8)|T1_FI_PARTY_NOTIFY_BATTLE_PARTY_OK)	// I->F, 파티전을 알림에 대한 ACK
#define T_FI_PARTY_ADD_MEMBER						(MessageType_t)((T0_FI_PARTY<<8)|T1_FI_PARTY_ADD_MEMBER)		// I->F, 파티원을 추가하라고 Field Server 알림
#define T_FI_PARTY_DELETE_MEMBER					(MessageType_t)((T0_FI_PARTY<<8)|T1_FI_PARTY_DELETE_MEMBER)		// I->F, 파티원을 제거하라고 Field Server 알림
#define T_FI_PARTY_UPDATE_ITEM_POS					(MessageType_t)((T0_FI_PARTY<<8)|T1_FI_PARTY_UPDATE_ITEM_POS)	// F->I, 파티원이 아이템 장착을 수정했을 때 전송
#define T_FI_PARTY_ALL_FLIGHT_POSITION				(MessageType_t)((T0_FI_PARTY<<8)|T1_FI_PARTY_ALL_FLIGHT_POSITION)
#define T_FI_PARTY_UPDATE_PARTY_INFO				(MessageType_t)((T0_FI_PARTY<<8)|T1_FI_PARTY_UPDATE_PARTY_INFO)	// I->F, 파티 정보를 업데이트
#define T_FI_PARTY_CHANGE_EXP_DISTRIBUTE_TYPE		(MessageType_t)((T0_FI_PARTY<<8)|T1_FI_PARTY_CHANGE_EXP_DISTRIBUTE_TYPE)	// 2008-06-04 by dhjin, EP3 편대 수정 - 경험치 분배 방식 변경 
#define T_FI_PARTY_CHANGE_ITEM_DISTRIBUTE_TYPE		(MessageType_t)((T0_FI_PARTY<<8)|T1_FI_PARTY_CHANGE_ITEM_DISTRIBUTE_TYPE)	// 2008-06-04 by dhjin, EP3 편대 수정 - 아이템 분배 방식 변경
#define T_FI_PARTY_CHANGE_FORMATION_SKILL			(MessageType_t)((T0_FI_PARTY<<8)|T1_FI_PARTY_CHANGE_FORMATION_SKILL)		// 2009-08-03 by cmkwon, EP3-4 편대 대형 스킬 구현 - 


#define T_IC_PARTY_CREATE							(MessageType_t)((T0_IC_PARTY<<8)|T1_IC_PARTY_CREATE)
#define T_IC_PARTY_ACCEPT_INVITE_OK					(MessageType_t)((T0_IC_PARTY<<8)|T1_IC_PARTY_ACCEPT_INVITE_OK)
#define T_IC_PARTY_GET_MEMBER						(MessageType_t)((T0_IC_PARTY<<8)|T1_IC_PARTY_GET_MEMBER)
#define T_IC_PARTY_PUT_MEMBER						(MessageType_t)((T0_IC_PARTY<<8)|T1_IC_PARTY_PUT_MEMBER)
#define T_IC_PARTY_GET_ALL_MEMBER					(MessageType_t)((T0_IC_PARTY<<8)|T1_IC_PARTY_GET_ALL_MEMBER)
#define T_IC_PARTY_PUT_ALL_MEMBER					(MessageType_t)((T0_IC_PARTY<<8)|T1_IC_PARTY_PUT_ALL_MEMBER)
#define T_IC_PARTY_BAN_MEMBER						(MessageType_t)((T0_IC_PARTY<<8)|T1_IC_PARTY_BAN_MEMBER)
#define T_IC_PARTY_BAN_MEMBER_OK					(MessageType_t)((T0_IC_PARTY<<8)|T1_IC_PARTY_BAN_MEMBER_OK)
#define T_IC_PARTY_LEAVE							(MessageType_t)((T0_IC_PARTY<<8)|T1_IC_PARTY_LEAVE)
#define T_IC_PARTY_LEAVE_OK							(MessageType_t)((T0_IC_PARTY<<8)|T1_IC_PARTY_LEAVE_OK)
#define T_IC_PARTY_TRANSFER_MASTER					(MessageType_t)((T0_IC_PARTY<<8)|T1_IC_PARTY_TRANSFER_MASTER)
#define T_IC_PARTY_TRANSFER_MASTER_OK				(MessageType_t)((T0_IC_PARTY<<8)|T1_IC_PARTY_TRANSFER_MASTER_OK)
#define T_IC_PARTY_DISMEMBER						(MessageType_t)((T0_IC_PARTY<<8)|T1_IC_PARTY_DISMEMBER)
#define T_IC_PARTY_DISMEMBER_OK						(MessageType_t)((T0_IC_PARTY<<8)|T1_IC_PARTY_DISMEMBER_OK)
#define T_IC_PARTY_CHANGE_FLIGHT_FORMATION			(MessageType_t)((T0_IC_PARTY<<8)|T1_IC_PARTY_CHANGE_FLIGHT_FORMATION)		// Cm -> I
#define T_IC_PARTY_CHANGE_FLIGHT_FORMATION_OK		(MessageType_t)((T0_IC_PARTY<<8)|T1_IC_PARTY_CHANGE_FLIGHT_FORMATION_OK)	// I -> C
#define T_IC_PARTY_GET_FLIGHT_POSITION				(MessageType_t)((T0_IC_PARTY<<8)|T1_IC_PARTY_GET_FLIGHT_POSITION)			// C -> I -> Cm
#define T_IC_PARTY_CHANGE_FLIGHT_POSITION			(MessageType_t)((T0_IC_PARTY<<8)|T1_IC_PARTY_CHANGE_FLIGHT_POSITION)		// Cm -> I -> C
#define T_IC_PARTY_CANCEL_FLIGHT_POSITION			(MessageType_t)((T0_IC_PARTY<<8)|T1_IC_PARTY_CANCEL_FLIGHT_POSITION)		// C -> I -> Cm
#define T_IC_PARTY_PUT_LAST_PARTY_INFO				(MessageType_t)((T0_IC_PARTY<<8)|T1_IC_PARTY_PUT_LAST_PARTY_INFO)			// I -> C, 파티원이 다시 게임을 시작하였을 때 전송, 자기 자신에게만 보냄
#define T_IC_PARTY_UPDATE_MEMBER_INFO_MAPNAME		(MessageType_t)((T0_IC_PARTY<<8)|T1_IC_PARTY_UPDATE_MEMBER_INFO_MAPNAME)	// I -> C, 워프시 맵이름 전송
#define T_IC_PARTY_MEMBER_INVALIDATED				(MessageType_t)((T0_IC_PARTY<<8)|T1_IC_PARTY_MEMBER_INVALIDATED)			// I -> C, 파티원이 비정상적으로 게임에서 튕겼을 때 전송
#define T_IC_PARTY_MEMBER_REJOINED					(MessageType_t)((T0_IC_PARTY<<8)|T1_IC_PARTY_MEMBER_REJOINED)				// I -> C, 파티원이 다시 게임을 시작하였을 때 전송, 자신은 제외함
#define T_IC_PARTY_UPDATE_ITEM_POS					(MessageType_t)((T0_IC_PARTY<<8)|T1_IC_PARTY_UPDATE_ITEM_POS)				// I -> C, 파티원이 아이템 장착을 수정했을 때 전송
#define T_IC_PARTY_ALL_FLIGHT_POSITION				(MessageType_t)((T0_IC_PARTY<<8)|T1_IC_PARTY_ALL_FLIGHT_POSITION)			// C->I, I->C, 파티장이 IMServer로 전송하면 IMServer는 모든 파티원들에게 전송
#define T_IC_PARTY_REQUEST_PARTYINFO_FROM_A_TO_M	(MessageType_t)((T0_IC_PARTY<<8)|T1_IC_PARTY_REQUEST_PARTYINFO_FROM_A_TO_M) // 2008-02-28 by dhjin, 아레나 통합 - C->I, 아레나 종료 후 메인 서버에 왔을 때 기존 파티 검사 
#define T_IC_PARTY_LEAVE_FROM_M_TO_A				(MessageType_t)((T0_IC_PARTY<<8)|T1_IC_PARTY_LEAVE_FROM_M_TO_A)				// 2008-02-28 by dhjin, 아레나 통합 - C->I, 아레나 시작을 위해 메인서버에서 파티 관련 처리
#define T_IC_PARTY_LEAVE_FROM_A_TO_M				(MessageType_t)((T0_IC_PARTY<<8)|T1_IC_PARTY_LEAVE_FROM_A_TO_M)				// 2008-02-28 by dhjin, 아레나 통합 - C->I, 아레나 서버에서 메인서버로 복귀할때 아레나 서버에서 파티 탈퇴 처리 클라이언트에게 전송하지 않는다.
#define T_IC_PARTY_LIST_INFO						(MessageType_t)((T0_IC_PARTY<<8)|T1_IC_PARTY_LIST_INFO)						// C -> I, 2008-06-02 by dhjin, EP3 편대 수정 - 편대 정보 리스트 요청
#define T_IC_PARTY_LIST_INFO_OK						(MessageType_t)((T0_IC_PARTY<<8)|T1_IC_PARTY_LIST_INFO_OK)					// I -> C, 2008-06-02 by dhjin, EP3 편대 수정 - 편대 정보 리스트 요청 OK
#define T_IC_PARTY_JOIN_FREE						(MessageType_t)((T0_IC_PARTY<<8)|T1_IC_PARTY_JOIN_FREE)						// C -> I, 2008-06-03 by dhjin, EP3 편대 수정 - 편대 자유 참여
#define T_IC_PARTY_JOIN_FREE_OK						(MessageType_t)((T0_IC_PARTY<<8)|T1_IC_PARTY_JOIN_FREE_OK)					// I -> C, 2008-06-03 by dhjin, EP3 편대 수정 - 편대 자유 참여 OK
#define T_IC_PARTY_CHANGE_INFO						(MessageType_t)((T0_IC_PARTY<<8)|T1_IC_PARTY_CHANGE_INFO)					// C -> I, 2008-06-04 by dhjin, EP3 편대 수정 - 편대 정보 수정
#define T_IC_PARTY_CHANGE_INFO_OK					(MessageType_t)((T0_IC_PARTY<<8)|T1_IC_PARTY_CHANGE_INFO_OK)				// I -> C, 2008-06-04 by dhjin, EP3 편대 수정 - 편대 정보 수정 OK
#define T_IC_PARTY_RECOMMENDATION_MEMBER			(MessageType_t)((T0_IC_PARTY<<8)|T1_IC_PARTY_RECOMMENDATION_MEMBER)			// C -> I, 2008-06-04 by dhjin, EP3 편대 수정 - 추천 케릭터 요청
#define T_IC_PARTY_RECOMMENDATION_MEMBER_OK			(MessageType_t)((T0_IC_PARTY<<8)|T1_IC_PARTY_RECOMMENDATION_MEMBER_OK)		// I -> C, 2008-06-04 by dhjin, EP3 편대 수정 - 추천 케릭터 요청 OK
#define T_IC_PARTY_INFO								(MessageType_t)((T0_IC_PARTY<<8)|T1_IC_PARTY_INFO)							// I -> C, 2008-06-10 by dhjin, EP3 편대 수정 - 편대 정보 전송

// FI_CHARACTER
#define T_FI_CHARACTER_DELETE_CHARACTER				(MessageType_t)((T0_FI_CHARACTER<<8)|T1_FI_CHARACTER_DELETE_CHARACTER)		// F->I, 캐릭터 삭제 전송
#define T_FI_CHARACTER_CHANGE_LEVEL					(MessageType_t)((T0_FI_CHARACTER<<8)|T1_FI_CHARACTER_CHANGE_LEVEL)			// F->I, 캐릭터 레벨 변화 전송
#define T_FI_CHARACTER_UPDATE_GUILD_INFO			(MessageType_t)((T0_FI_CHARACTER<<8)|T1_FI_CHARACTER_UPDATE_GUILD_INFO)		// I->F, 길드 정보 업데이트
#define T_FI_CHARACTER_UPDATE_MAP_CHANNEL			(MessageType_t)((T0_FI_CHARACTER<<8)|T1_FI_CHARACTER_UPDATE_MAP_CHANNEL)	// F->I, 맵채널 정보 업데이트
#define T_FI_CHARACTER_CHANGE_INFLUENCE_TYPE		(MessageType_t)((T0_FI_CHARACTER<<8)|T1_FI_CHARACTER_CHANGE_INFLUENCE_TYPE)	// F->I, 2005-12-03 by cmkwon
#define T_FI_UPDATE_SUBLEADER						(MessageType_t)((T0_FI_CHARACTER<<8)|T1_FI_UPDATE_SUBLEADER)				// F->I, 2007-02-14 by dhjin
#define T_FI_CREATE_GUILD_BY_SUBLEADER				(MessageType_t)((T0_FI_CHARACTER<<8)|T1_FI_CREATE_GUILD_BY_SUBLEADER)		// F->I, 2007-10-06 by dhjin

#define T_FI_MULTICHAT_STEERING						(MessageType_t)((T0_FI_EVENT<<8)|T1_FI_MULTICHAT_STEERING)					//F->I bt inetpub 19-03-2017 - control multichat by field srv for nation war like sp ms

#define T_FC_MONSTER_CREATED						(MessageType_t)((T0_FC_MONSTER<<8)|T1_FC_MONSTER_CREATED)
#define T_FC_MONSTER_MOVE_OK						(MessageType_t)((T0_FC_MONSTER<<8)|T1_FC_MONSTER_MOVE_OK)
#define T_FC_MONSTER_HIDE							(MessageType_t)((T0_FC_MONSTER<<8)|T1_FC_MONSTER_HIDE)
#define T_FC_MONSTER_SHOW							(MessageType_t)((T0_FC_MONSTER<<8)|T1_FC_MONSTER_SHOW)
#define T_FC_MONSTER_CHANGE_HP						(MessageType_t)((T0_FC_MONSTER<<8)|T1_FC_MONSTER_CHANGE_HP)
#define T_FC_MONSTER_CHANGE_BODYCONDITION			(MessageType_t)((T0_FC_MONSTER<<8)|T1_FC_MONSTER_CHANGE_BODYCONDITION)
#define T_FC_MONSTER_SKILL_USE_SKILL				(MessageType_t)((T0_FC_MONSTER<<8)|T1_FC_MONSTER_SKILL_USE_SKILL)
#define T_FC_MONSTER_SKILL_END_SKILL				(MessageType_t)((T0_FC_MONSTER<<8)|T1_FC_MONSTER_SKILL_END_SKILL)
#define T_FC_MONSTER_SUMMON_MONSTER					(MessageType_t)((T0_FC_MONSTER<<8)|T1_FC_MONSTER_SUMMON_MONSTER)			// C->F, 몬스터 생성 명령
#define T_FC_MONSTER_TUTORIAL_MONSTER_DEAD_NOTIFY	(MessageType_t)((T0_FC_MONSTER<<8)|T1_FC_MONSTER_TUTORIAL_MONSTER_DEAD_NOTIFY)	// F->C, 튜토리얼맵에서 몬스터가 죽은것을 클라이언트에 알림
#define T_FC_MONSTER_TUTORIAL_MONSTER_DELETE		(MessageType_t)((T0_FC_MONSTER<<8)|T1_FC_MONSTER_TUTORIAL_MONSTER_DELETE)		// C->F, 튜토리얼맵에서 몬스터 삭제를 서버에 요청
#define T_FC_MONSTER_CHANGE_INDEX					(MessageType_t)((T0_FC_MONSTER<<8)|T1_FC_MONSTER_CHANGE_INDEX)		// F->C, 2011-05-17 by hskim, 인피니티 3차 - 시네마 몬스터 교체 기능
#define T_FC_MONSTER_CINEMA_DELETE_NOTIFY			(MessageType_t)((T0_FC_MONSTER<<8)|T1_FC_MONSTER_CINEMA_DELETE_NOTIFY)		// F->C, 2011-05-30 by hskim, 인피니티 3차 - 몬스터 삭제 클라이언트 반영

#define T_FN_MAPPROJECT_START						(MessageType_t)((T0_FN_MONSTER<<8)|T1_FN_MAPPROJECT_START)
#define T_FN_MAPPROJECT_START_OK					(MessageType_t)((T0_FN_MONSTER<<8)|T1_FN_MAPPROJECT_START_OK)
#define T_FN_MAPPROJECT_START_OK_ACK				(MessageType_t)((T0_FN_MONSTER<<8)|T1_FN_MAPPROJECT_START_OK_ACK)
#define T_FN_MONSTER_CREATE							(MessageType_t)((T0_FN_MONSTER<<8)|T1_FN_MONSTER_CREATE)
#define T_FN_MONSTER_CREATE_OK						(MessageType_t)((T0_FN_MONSTER<<8)|T1_FN_MONSTER_CREATE_OK)
#define T_FN_MONSTER_DELETE							(MessageType_t)((T0_FN_MONSTER<<8)|T1_FN_MONSTER_DELETE)
#define T_FN_MONSTER_CHANGE_OK						(MessageType_t)((T0_FN_MONSTER<<8)|T1_FN_MONSTER_CHANGE_OK)		// 2011-05-11 by hskim, 인피니티 3차 - 시네마 관련 기능 추가 - 해당 맵채널 특정 몬스터 변경 기능 추가
#define T_FN_CLIENT_GAMESTART_OK					(MessageType_t)((T0_FN_MONSTER<<8)|T1_FN_CLIENT_GAMESTART_OK)
#define T_FN_CLIENT_GAMEEND_OK						(MessageType_t)((T0_FN_MONSTER<<8)|T1_FN_CLIENT_GAMEEND_OK)
#define T_FN_GET_CHARACTER_INFO						(MessageType_t)((T0_FN_MONSTER<<8)|T1_FN_GET_CHARACTER_INFO)
#define T_FN_GET_CHARACTER_INFO_OK					(MessageType_t)((T0_FN_MONSTER<<8)|T1_FN_GET_CHARACTER_INFO_OK)
#define T_FN_ADMIN_SUMMON_MONSTER					(MessageType_t)((T0_FN_MONSTER<<8)|T1_FN_ADMIN_SUMMON_MONSTER)
#define T_FN_MONSTER_CHANGE_HP						(MessageType_t)((T0_FN_MONSTER<<8)|T1_FN_MONSTER_CHANGE_HP)
#define T_FN_MONSTER_CHANGE_BODYCONDITION			(MessageType_t)((T0_FN_MONSTER<<8)|T1_FN_MONSTER_CHANGE_BODYCONDITION)
#define T_FN_MONSTER_SKILL_USE_SKILL				(MessageType_t)((T0_FN_MONSTER<<8)|T1_FN_MONSTER_SKILL_USE_SKILL)
#define T_FN_MONSTER_SKILL_END_SKILL				(MessageType_t)((T0_FN_MONSTER<<8)|T1_FN_MONSTER_SKILL_END_SKILL)
#define T_FN_MONSTER_AUTO_DESTROYED					(MessageType_t)((T0_FN_MONSTER<<8)|T1_FN_MONSTER_AUTO_DESTROYED)	// 2006-04-17 by cmkwon
#define T_FN_MONSTER_STRATEGYPOINT_INIT				(MessageType_t)((T0_FN_MONSTER<<8)|T1_FN_MONSTER_STRATEGYPOINT_INIT)	// 2006-11-20 by cmkwon, F->N
#define T_FN_MONSTER_STRATEGYPOINT_SUMMON			(MessageType_t)((T0_FN_MONSTER<<8)|T1_FN_MONSTER_STRATEGYPOINT_SUMMON)	// 2006-11-20 by cmkwon, F->N
#define T_FN_MONSTER_OUTPOST_INIT					(MessageType_t)((T0_FN_MONSTER<<8)|T1_FN_MONSTER_OUTPOST_INIT)		// 2007-08-24 by dhjin, F->N
#define T_FN_MONSTER_OUTPOST_RESET_SUMMON			(MessageType_t)((T0_FN_MONSTER<<8)|T1_FN_MONSTER_OUTPOST_RESET_SUMMON)	// 2007-08-24 by dhjin, F->N
#define T_FN_MONSTER_CREATE_IN_MAPCHANNEL_BYVALUE	(MessageType_t)((T0_FN_MONSTER<<8)|T1_FN_MONSTER_CREATE_IN_MAPCHANNEL_BYVALUE)	// 2007-08-29 by dhjin
#define T_FN_MONSTER_TELEPORT_SUMMON				(MessageType_t)((T0_FN_MONSTER<<8)|T1_FN_MONSTER_TELEPORT_SUMMON)	// 2007-09-05 by dhjin

#define T_FN_NPCSERVER_START						(MessageType_t)((T0_FN_NPCSERVER<<8)|T1_FN_NPCSERVER_START)
#define T_FN_NPCSERVER_START_OK						(MessageType_t)((T0_FN_NPCSERVER<<8)|T1_FN_NPCSERVER_START_OK)
#define T_FN_NPCSERVER_SUMMON_JACO_MONSTER			(MessageType_t)((T0_FN_NPCSERVER<<8)|T1_FN_NPCSERVER_SUMMON_JACO_MONSTER)
#define T_FN_NPCSERVER_DELETE_MONSTER_IN_MAPCHANNEL	(MessageType_t)((T0_FN_NPCSERVER<<8)|T1_FN_NPCSERVER_DELETE_MONSTER_IN_MAPCHANNEL)	// TCP:F->N, // 2007-08-22 by cmkwon, 해당 맵채널 몬스터 모두 삭제하기 기능 추가
#define T_FN_NPCSERVER_CINEMA_MONSTER_CREATE		(MessageType_t)((T0_FN_NPCSERVER<<8)|T1_FN_NPCSERVER_CINEMA_MONSTER_CREATE)	// 2010-03-31 by dhjin, 인피니티(기지방어) -	// 2009-09-09 ~ 2010 by dhjin, 인피니티 - 키 몬스터 생성, F -> N
#define T_FN_NPCSERVER_CINEMA_MONSTER_DESTROY		(MessageType_t)((T0_FN_NPCSERVER<<8)|T1_FN_NPCSERVER_CINEMA_MONSTER_DESTROY)	// 2011-04-28 by hskim, 인피니티 3차 - 시네마 관련 기능 추가 - 해당 맵채널 특정 몬스터 삭제 기능 추가
#define T_FN_NPCSERVER_CINEMA_MONSTER_CHANGE		(MessageType_t)((T0_FN_NPCSERVER<<8)|T1_FN_NPCSERVER_CINEMA_MONSTER_CHANGE)		// 2011-05-11 by hskim, 인피니티 3차 - 시네마 관련 기능 추가 - 해당 맵채널 특정 몬스터 변경 기능 추가
#define T_FN_NPCSERVER_CINEMA_MONSTER_REGEN			(MessageType_t)((T0_FN_NPCSERVER<<8)|T1_FN_NPCSERVER_CINEMA_MONSTER_REGEN)		// 2011-06-02 by hskim, 인피니티 3차 - 스텝 6 - 주기적 소환 기능 제작
#define T_FN_NPCSERVER_NEW_CHANGE_OBJECT			(MessageType_t)((T0_FN_NPCSERVER<<8)|T1_FN_NPCSERVER_NEW_CHANGE_OBJECT)		// 2009-09-09 ~ 2010 by dhjin, 인피니티 - 변경 오브젝트를 위해!!!! 
#define T_FN_NPCSERVER_RESET_CHANGE_OBJECT			(MessageType_t)((T0_FN_NPCSERVER<<8)|T1_FN_NPCSERVER_RESET_CHANGE_OBJECT)		// 2009-09-09 ~ 2010 by dhjin, 인피니티 - 변경 오브젝트를 위해!!!! 


#define T_FC_EVENT_WARP							(MessageType_t)((T0_FC_EVENT<<8)|T1_FC_EVENT_WARP)
#define T_FC_EVENT_WARP_SAME_MAP				(MessageType_t)((T0_FC_EVENT<<8)|T1_FC_EVENT_WARP_SAME_MAP)
#define T_FC_EVENT_WARP_SAME_MAP_DONE			(MessageType_t)((T0_FC_EVENT<<8)|T1_FC_EVENT_WARP_SAME_MAP_DONE)
#define T_FC_EVENT_WARP_SAME_FIELD_SERVER		(MessageType_t)((T0_FC_EVENT<<8)|T1_FC_EVENT_WARP_SAME_FIELD_SERVER)
#define T_FC_EVENT_WARP_SAME_FIELD_SERVER_DONE	(MessageType_t)((T0_FC_EVENT<<8)|T1_FC_EVENT_WARP_SAME_FIELD_SERVER_DONE)
#define T_FC_EVENT_OTHER_WARPED					(MessageType_t)((T0_FC_EVENT<<8)|T1_FC_EVENT_OTHER_WARPED)
// 2004-12-16 by cmkwon, 다른 필드서버로의 워프는 없으므로 삭제함
//#define T_FC_EVENT_WARP_CONNECT					(MessageType_t)((T0_FC_EVENT<<8)|T1_FC_EVENT_WARP_CONNECT)
#define T_FC_EVENT_WARP_CONNECT_OK				(MessageType_t)((T0_FC_EVENT<<8)|T1_FC_EVENT_WARP_CONNECT_OK)
#define T_FC_EVENT_ENTER_BUILDING				(MessageType_t)((T0_FC_EVENT<<8)|T1_FC_EVENT_ENTER_BUILDING)
#define T_FC_EVENT_ENTER_BUILDING_OK			(MessageType_t)((T0_FC_EVENT<<8)|T1_FC_EVENT_ENTER_BUILDING_OK)
#define T_FC_EVENT_LEAVE_BUILDING				(MessageType_t)((T0_FC_EVENT<<8)|T1_FC_EVENT_LEAVE_BUILDING)
#define T_FC_EVENT_LEAVE_BUILDING_OK			(MessageType_t)((T0_FC_EVENT<<8)|T1_FC_EVENT_LEAVE_BUILDING_OK)
#define T_FC_EVENT_REQUEST_WARP					(MessageType_t)((T0_FC_EVENT<<8)|T1_FC_EVENT_REQUEST_WARP)
#define T_FC_EVENT_REJECT_WARP					(MessageType_t)((T0_FC_EVENT<<8)|T1_FC_EVENT_REJECT_WARP)
#define T_FC_EVENT_REQUEST_OBJECT_EVENT			(MessageType_t)((T0_FC_EVENT<<8)|T1_FC_EVENT_REQUEST_OBJECT_EVENT)
#define T_FC_EVENT_CHANGE_WEATHER				(MessageType_t)((T0_FC_EVENT<<8)|T1_FC_EVENT_CHANGE_WEATHER)
#define T_FC_EVENT_SUGGEST_CHANNELS				(MessageType_t)((T0_FC_EVENT<<8)|T1_FC_EVENT_SUGGEST_CHANNELS)	// F->C, 선택 가능한 채널을 제시함
#define T_FC_EVENT_SELECT_CHANNEL				(MessageType_t)((T0_FC_EVENT<<8)|T1_FC_EVENT_SELECT_CHANNEL)	// C->F, 선택한 채널을 전송함
#define T_FC_EVENT_SELECT_CHANNEL_WITH_PARTY	(MessageType_t)((T0_FC_EVENT<<8)|T1_FC_EVENT_SELECT_CHANNEL_WITH_PARTY)	// C->F, 선택한 채널을 전송함, 파티원 리스트 포함
#define T_FC_EVENT_REQUEST_RACING_WARP			(MessageType_t)((T0_FC_EVENT<<8)|T1_FC_EVENT_REQUEST_RACING_WARP)	// C->F, 레이싱맵으로의 워프 요청
#define T_FC_EVENT_GET_SHOP_WARP_TARGET_MAP_LIST	(MessageType_t)((T0_FC_EVENT<<8)|T1_FC_EVENT_GET_SHOP_WARP_TARGET_MAP_LIST)
#define T_FC_EVENT_GET_SHOP_WARP_TARGET_MAP_LIST_OK	(MessageType_t)((T0_FC_EVENT<<8)|T1_FC_EVENT_GET_SHOP_WARP_TARGET_MAP_LIST_OK)
#define T_FC_EVENT_REQUEST_SHOP_WARP				(MessageType_t)((T0_FC_EVENT<<8)|T1_FC_EVENT_REQUEST_SHOP_WARP)
#define T_FC_EVENT_CHARACTERMODE_ENTER_BUILDING		(MessageType_t)((T0_FC_EVENT<<8)|T1_FC_EVENT_CHARACTERMODE_ENTER_BUILDING)	// C->F, 
#define T_FC_EVENT_CALL_WARP_EVENT_REQUEST			(MessageType_t)((T0_FC_EVENT<<8)|T1_FC_EVENT_CALL_WARP_EVENT_REQUEST)		// 2006-07-21 by cmkwon, F->C(n)
#define T_FC_EVENT_CALL_WARP_EVENT_REQUEST_ACK		(MessageType_t)((T0_FC_EVENT<<8)|T1_FC_EVENT_CALL_WARP_EVENT_REQUEST_ACK)	// 2006-07-21 by cmkwon, C(n)->F


#define T_FN_EVENT_WARP							(MessageType_t)((T0_FN_EVENT<<8)|T1_FN_EVENT_WARP)
#define T_FN_EVENT_OTHER_WARPED					(MessageType_t)((T0_FN_EVENT<<8)|T1_FN_EVENT_OTHER_WARPED)
#define T_FN_EVENT_WARP_CONNECT_OK				(MessageType_t)((T0_FN_EVENT<<8)|T1_FN_EVENT_WARP_CONNECT_OK)

// 2005-07-27 by cmkwon, 다른 필드서버로의 워프는 없으므로 삭제함
//#define T_FP_EVENT_NOTIFY_WARP					(MessageType_t)((T0_FP_EVENT<<8)|T1_FP_EVENT_NOTIFY_WARP)
//#define T_FP_EVENT_NOTIFY_WARP_OK				(MessageType_t)((T0_FP_EVENT<<8)|T1_FP_EVENT_NOTIFY_WARP_OK)
#define T_FP_EVENT_ENTER_SELECT_SCREEN			(MessageType_t)((T0_FP_EVENT<<8)|T1_FP_EVENT_ENTER_SELECT_SCREEN)	// F->P, Client가 캐릭터 선택 창에 들어옴
#define T_FP_EVENT_GAME_STARTED					(MessageType_t)((T0_FP_EVENT<<8)|T1_FP_EVENT_GAME_STARTED)			// F->P, Client가 게임을 시작함(맵으로 들어옴)
#define T_FP_EVENT_MAP_CHANGED					(MessageType_t)((T0_FP_EVENT<<8)|T1_FP_EVENT_MAP_CHANGED)			// F->P, Client가 맵을 이동함
#define T_FP_EVENT_RELOAD_HAPPYEV				(MessageType_t)((T0_FP_EVENT<<8)|T1_FP_EVENT_RELOAD_HAPPYEV)		// P->F, No Body, All ServerGroup reload HappyHourEvent.
#define T_FP_EVENT_RELOAD_ITEMEV				(MessageType_t)((T0_FP_EVENT<<8)|T1_FP_EVENT_RELOAD_ITEMEV)			// P->F, No Body, All ServerGroup reload ITEMEvent.
#define T_FP_EVENT_UPDATE_PCBANGLIST			(MessageType_t)((T0_FP_EVENT<<8)|T1_FP_EVENT_UPDATE_PCBANGLIST)		// P->F, No Body
#define T_FP_EVENT_UPDATE_STRATEGYPOINT_NOTSUMMONTIM			(MessageType_t)((T0_FP_EVENT<<8)|T1_FP_EVENT_UPDATE_STRATEGYPOINT_NOTSUMMONTIM)		// P->F, No Body

// T0_FP_MONITOR
#define T_FP_MONITOR_SET_MGAME_EVENT_TYPE		(MessageType_t)((T0_FP_MONITOR<<8)|T1_FP_MONITOR_SET_MGAME_EVENT_TYPE)
#define T_FP_MONITOR_RELOAD_VERSION_INFO_OK		(MessageType_t)((T0_FP_MONITOR<<8)|T1_FP_MONITOR_RELOAD_VERSION_INFO_OK)	// P->F(n), No Body, // 2008-09-08 by cmkwon, SCMonitor에서 ReloadVersionInfo시에 일부 체크섬파일(.\Res-Tex\*.*)도 리로드하기 - 

// T0_FP_CASH
#define T_FP_CASH_CHANGE_CHARACTERNAME			(MessageType_t)((T0_FP_CASH<<8)|T1_FP_CASH_CHANGE_CHARACTERNAME)

struct MSG_FP_CASH_CHANGE_CHARACTERNAME
{
	char		szAccName[SIZE_MAX_ACCOUNT_NAME];
	char		szChangedCharName[SIZE_MAX_CHARACTER_NAME];
};

// #define T0_FP_ADMIN					0x73			// 2008-01-31 by cmkwon, 계정 블럭/해제 명령어로 가능한 시스템 구현 - 
#define T_FP_ADMIN_BLOCKACCOUNT					(MessageType_t)((T0_FP_ADMIN<<8)|T1_FP_ADMIN_BLOCKACCOUNT)	// 2008-01-31 by cmkwon, 계정 블럭/해제 명령어로 가능한 시스템 구현 - 
#define T_FP_ADMIN_BLOCKACCOUNT_OK				(MessageType_t)((T0_FP_ADMIN<<8)|T1_FP_ADMIN_BLOCKACCOUNT_OK)	// 2008-01-31 by cmkwon, 계정 블럭/해제 명령어로 가능한 시스템 구현 - 
#define T_FP_ADMIN_UNBLOCKACCOUNT				(MessageType_t)((T0_FP_ADMIN<<8)|T1_FP_ADMIN_UNBLOCKACCOUNT)	// 2008-01-31 by cmkwon, 계정 블럭/해제 명령어로 가능한 시스템 구현 - 
#define T_FP_ADMIN_UNBLOCKACCOUNT_OK			(MessageType_t)((T0_FP_ADMIN<<8)|T1_FP_ADMIN_UNBLOCKACCOUNT_OK)	// 2008-01-31 by cmkwon, 계정 블럭/해제 명령어로 가능한 시스템 구현 - 
#define T_FP_ADMIN_STRATRGYPOINT_INFO_CHANGE	(MessageType_t)((T0_FP_ADMIN<<8)|T1_FP_ADMIN_STRATRGYPOINT_INFO_CHANGE)
struct MSG_FP_ADMIN_BLOCKACCOUNT		// 2008-01-31 by cmkwon, 계정 블럭/해제 명령어로 가능한 시스템 구현 - 
{
	SBLOCKED_ACCOUNT_INFO blockAccInfo;
};
struct MSG_FP_ADMIN_BLOCKACCOUNT_OK		// 2008-01-31 by cmkwon, 계정 블럭/해제 명령어로 가능한 시스템 구현 - 
{
	Err_t	ErrCode;		// ERR_NO_ERROR 이면 성공
	INT		AdminFieldServerClientIndex;		// 2008-01-31 by cmkwon, 계정 블럭/해제 명령어로 가능한 시스템 구현 - PreServer<->FieldServer 에서만 사용함
	char	AdminAccName[SIZE_MAX_ACCOUNT_NAME];
	char	BlockedAccName[SIZE_MAX_ACCOUNT_NAME];
	ATUM_DATE_TIME	atimeEndTime;				// 블럭 종료 시간
};

struct MSG_FP_ADMIN_UNBLOCKACCOUNT		// 2008-01-31 by cmkwon, 계정 블럭/해제 명령어로 가능한 시스템 구현 - 
{
	SBLOCKED_ACCOUNT_INFO blockAccInfo;
};
struct MSG_FP_ADMIN_UNBLOCKACCOUNT_OK	// 2008-01-31 by cmkwon, 계정 블럭/해제 명령어로 가능한 시스템 구현 - 
{
	Err_t	ErrCode;		// ERR_NO_ERROR 이면 성공
	INT		AdminFieldServerClientIndex;		// 2008-01-31 by cmkwon, 계정 블럭/해제 명령어로 가능한 시스템 구현 - PreServer<->FieldServer 에서만 사용함
	char	AdminAccName[SIZE_MAX_ACCOUNT_NAME];
	char	UnblockedAccName[SIZE_MAX_ACCOUNT_NAME];
};


#define T_FC_STORE_GET_ITEM						(MessageType_t)((T0_FC_STORE<<8)|T1_FC_STORE_GET_ITEM)
#define T_FC_STORE_PUT_ITEM_HEADER				(MessageType_t)((T0_FC_STORE<<8)|T1_FC_STORE_PUT_ITEM_HEADER)
#define T_FC_STORE_PUT_ITEM						(MessageType_t)((T0_FC_STORE<<8)|T1_FC_STORE_PUT_ITEM)
#define T_FC_STORE_PUT_ITEM_DONE				(MessageType_t)((T0_FC_STORE<<8)|T1_FC_STORE_PUT_ITEM_DONE)
#define T_FC_STORE_MOVE_ITEM					(MessageType_t)((T0_FC_STORE<<8)|T1_FC_STORE_MOVE_ITEM)
#define T_FC_STORE_MOVE_ITEM_OK					(MessageType_t)((T0_FC_STORE<<8)|T1_FC_STORE_MOVE_ITEM_OK)
#define T_FC_STORE_INSERT_ITEM					(MessageType_t)((T0_FC_STORE<<8)|T1_FC_STORE_INSERT_ITEM)
#define T_FC_STORE_DELETE_ITEM					(MessageType_t)((T0_FC_STORE<<8)|T1_FC_STORE_DELETE_ITEM)
#define T_FC_STORE_UPDATE_ITEM_COUNT			(MessageType_t)((T0_FC_STORE<<8)|T1_FC_STORE_UPDATE_ITEM_COUNT)
#define T_FC_STORE_UPDATE_ENDURANCE				(MessageType_t)((T0_FC_STORE<<8)|T1_FC_STORE_UPDATE_ENDURANCE)
#define T_FC_STORE_UPDATE_POSSESS				(MessageType_t)((T0_FC_STORE<<8)|T1_FC_STORE_UPDATE_POSSESS)
#define T_FC_STORE_UPDATE_RARE_FIX				(MessageType_t)((T0_FC_STORE<<8)|T1_FC_STORE_UPDATE_RARE_FIX)		// F->C, 접두사, 접미사의 업데이트 전송
#define T_FC_STORE_INSERT_USINGITEM				(MessageType_t)((T0_FC_STORE<<8)|T1_FC_STORE_INSERT_USINGITEM)
#define T_FC_STORE_DELETE_USINGITEM				(MessageType_t)((T0_FC_STORE<<8)|T1_FC_STORE_DELETE_USINGITEM)
#define T_FC_STORE_UPDATE_USINGITEM				(MessageType_t)((T0_FC_STORE<<8)|T1_FC_STORE_UPDATE_USINGITEM)		// F->C, 2006-03-30 by cmkwon
#define T_FC_STORE_EXPIRE_USINGITEM				(MessageType_t)((T0_FC_STORE<<8)|T1_FC_STORE_EXPIRE_USINGITEM)		// C->F, 2006-10-11 by cmkwon, 사용 시간이 끝난것을 서버로 알림
#define T_FC_STORE_UPDATE_ITEMSTORAGE			(MessageType_t)((T0_FC_STORE<<8)|T1_FC_STORE_UPDATE_ITEMSTORAGE)	// 2005-12-07 by cmkwon
#define T_FC_STORE_UPDATE_ITEMNUM				(MessageType_t)((T0_FC_STORE<<8)|T1_FC_STORE_UPDATE_ITEMNUM)		// 2006-06-14 by cmkwon
#define T_FC_STORE_REQUEST_QUICKSLOT			(MessageType_t)((T0_FC_STORE<<8)|T1_FC_STORE_REQUEST_QUICKSLOT)		// 2006-09-04 by dhjin
#define T_FC_STORE_REQUEST_QUICKSLOT_OK			(MessageType_t)((T0_FC_STORE<<8)|T1_FC_STORE_REQUEST_QUICKSLOT_OK)	// 2006-09-04 by dhjin
#define T_FC_STORE_SAVE_QUICKSLOT				(MessageType_t)((T0_FC_STORE<<8)|T1_FC_STORE_SAVE_QUICKSLOT)		// 2006-09-04 by dhjin
#define T_FC_STORE_LOG_GUILD_ITEM				(MessageType_t)((T0_FC_STORE<<8)|T1_FC_STORE_LOG_GUILD_ITEM)			// 2006-09-27 by dhjin
#define T_FC_STORE_LOG_GUILD_ITEM_OK			(MessageType_t)((T0_FC_STORE<<8)|T1_FC_STORE_LOG_GUILD_ITEM_OK)			// 2006-09-27 by dhjin
#define T_FC_STORE_LOG_GUILD_ITEM_OK_HEADER		(MessageType_t)((T0_FC_STORE<<8)|T1_FC_STORE_LOG_GUILD_ITEM_OK_HEADER)	// 2006-09-27 by dhjin
#define T_FC_STORE_LOG_GUILD_ITEM_OK_DONE		(MessageType_t)((T0_FC_STORE<<8)|T1_FC_STORE_LOG_GUILD_ITEM_OK_DONE)	// 2006-09-27 by dhjin

#define T_FC_ITEM_SHOW_ITEM								(MessageType_t)((T0_FC_ITEM<<8)|T1_FC_ITEM_SHOW_ITEM)
#define T_FC_ITEM_HIDE_ITEM								(MessageType_t)((T0_FC_ITEM<<8)|T1_FC_ITEM_HIDE_ITEM)
#define T_FC_ITEM_GET_ITEM								(MessageType_t)((T0_FC_ITEM<<8)|T1_FC_ITEM_GET_ITEM)
#define T_FC_ITEM_GET_ITEM_OK							(MessageType_t)((T0_FC_ITEM<<8)|T1_FC_ITEM_GET_ITEM_OK)
#define T_FC_ITEM_GET_ITEM_FAIL							(MessageType_t)((T0_FC_ITEM<<8)|T1_FC_ITEM_GET_ITEM_FAIL)
#define T_FC_ITEM_PUT_ITEM								(MessageType_t)((T0_FC_ITEM<<8)|T1_FC_ITEM_PUT_ITEM)
// 2005-11-15 by cmkwon, 제거함
//#define T_FC_ITEM_BONUSSKILLPOINT						(MessageType_t)((T0_FC_ITEM<<8)|T1_FC_ITEM_BONUSSKILLPOINT)
//#define T_FC_ITEM_BONUSSKILLPOINT_OK					(MessageType_t)((T0_FC_ITEM<<8)|T1_FC_ITEM_BONUSSKILLPOINT_OK)
#define T_FC_ITEM_CHANGE_WINDOW_POSITION				(MessageType_t)((T0_FC_ITEM<<8)|T1_FC_ITEM_CHANGE_WINDOW_POSITION)
#define T_FC_ITEM_CHANGE_WINDOW_POSITION_OK				(MessageType_t)((T0_FC_ITEM<<8)|T1_FC_ITEM_CHANGE_WINDOW_POSITION_OK)
#define T_FC_ITEM_UPDATE_WINDOW_ITEM_LIST				(MessageType_t)((T0_FC_ITEM<<8)|T1_FC_ITEM_UPDATE_WINDOW_ITEM_LIST)
#define T_FC_ITEM_THROW_AWAY_ITEM						(MessageType_t)((T0_FC_ITEM<<8)|T1_FC_ITEM_THROW_AWAY_ITEM)
#define T_FC_ITEM_THROW_AWAY_ITEM_OK					(MessageType_t)((T0_FC_ITEM<<8)|T1_FC_ITEM_THROW_AWAY_ITEM_OK)
#define T_FC_ITEM_USE_ENERGY							(MessageType_t)((T0_FC_ITEM<<8)|T1_FC_ITEM_USE_ENERGY)
#define T_FC_ITEM_USE_ENERGY_OK							(MessageType_t)((T0_FC_ITEM<<8)|T1_FC_ITEM_USE_ENERGY_OK)
#define T_FC_ITEM_USE_ITEM								(MessageType_t)((T0_FC_ITEM<<8)|T1_FC_ITEM_USE_ITEM)
#define T_FC_ITEM_USE_ITEM_OK							(MessageType_t)((T0_FC_ITEM<<8)|T1_FC_ITEM_USE_ITEM_OK)
#define T_FC_ITEM_DELETE_ITEM_ADMIN						(MessageType_t)((T0_FC_ITEM<<8)|T1_FC_ITEM_DELETE_ITEM_ADMIN)
#define T_FC_ITEM_RELOAD_ITEM_INFO						(MessageType_t)((T0_FC_ITEM<<8)|T1_FC_ITEM_RELOAD_ITEM_INFO)	// 아이템 정보가 업데이트되었음, no body
#define T_FC_ITEM_USE_ENCHANT							(MessageType_t)((T0_FC_ITEM<<8)|T1_FC_ITEM_USE_ENCHANT)
#define T_FC_ITEM_USE_ENCHANT_OK						(MessageType_t)((T0_FC_ITEM<<8)|T1_FC_ITEM_USE_ENCHANT_OK)
#define T_FC_ITEM_PUT_ENCHANT_HEADER					(MessageType_t)((T0_FC_ITEM<<8)|T1_FC_ITEM_PUT_ENCHANT_HEADER)
#define T_FC_ITEM_PUT_ENCHANT							(MessageType_t)((T0_FC_ITEM<<8)|T1_FC_ITEM_PUT_ENCHANT)
#define T_FC_ITEM_PUT_ENCHANT_DONE						(MessageType_t)((T0_FC_ITEM<<8)|T1_FC_ITEM_PUT_ENCHANT_DONE)

#ifdef BONUS_STAT_ITEM
#define T_FC_ITEM_PUT_BONUS_HEADER						(MessageType_t)((T0_FC_ITEM<<8)|T1_FC_ITEM_PUT_BONUS_HEADER)
#define T_FC_ITEM_PUT_BONUS								(MessageType_t)((T0_FC_ITEM<<8)|T1_FC_ITEM_PUT_BONUS)
#define T_FC_ITEM_PUT_BONUS_DONE						(MessageType_t)((T0_FC_ITEM<<8)|T1_FC_ITEM_PUT_BONUS_DONE)
#endif


#define T_FC_ITEM_DELETE_ALL_ENCHANT					(MessageType_t)((T0_FC_ITEM<<8)|T1_FC_ITEM_DELETE_ALL_ENCHANT)	// F->C, 아이템의 모든 인챈트를 삭제한다
#define T_FC_ITEM_DELETE_DROP_ITEM						(MessageType_t)((T0_FC_ITEM<<8)|T1_FC_ITEM_DELETE_DROP_ITEM)	// F->C, 자신이 뿌린 아이템(마인등)을 지울 때 쓰임
#define T_FC_ITEM_UPDATE_ITEM_POS						(MessageType_t)((T0_FC_ITEM<<8)|T1_FC_ITEM_UPDATE_ITEM_POS)		// F->C, 아이템 장착(전)을 갱신함, 아이템 장착을 바꾸면 주위에 전송함
#define T_FC_ITEM_MIX_ITEMS								(MessageType_t)((T0_FC_ITEM<<8)|T1_FC_ITEM_MIX_ITEMS)			// C->F, 조합할 아이템의 리스트를 전송
#define T_FC_ITEM_MIX_ITEMS_RESULT						(MessageType_t)((T0_FC_ITEM<<8)|T1_FC_ITEM_MIX_ITEMS_RESULT)	// F->C, 아이템 조합 결과를 전송
#define T_FC_ITEM_USE_CARDITEM_GUILDSUMMON				(MessageType_t)((T0_FC_ITEM<<8)|T1_FC_ITEM_USE_CARDITEM_GUILDSUMMON)				// 길드 소환 카드
#define T_FC_ITEM_USE_CARDITEM_GUILDSUMMON_NOTIFY		(MessageType_t)((T0_FC_ITEM<<8)|T1_FC_ITEM_USE_CARDITEM_GUILDSUMMON_NOTIFY)			// 길드 소환 카드로 소환됨을 알림
#define T_FC_ITEM_USE_CARDITEM_RESTORE					(MessageType_t)((T0_FC_ITEM<<8)|T1_FC_ITEM_USE_CARDITEM_RESTORE)					// 부활 카드
#define T_FC_ITEM_USE_CARDITEM_RESTORE_NOTIFY			(MessageType_t)((T0_FC_ITEM<<8)|T1_FC_ITEM_USE_CARDITEM_RESTORE_NOTIFY)				// 부활 카드로 부활됨을 알림
#define T_FC_ITEM_USE_CARDITEM_GUILD					(MessageType_t)((T0_FC_ITEM<<8)|T1_FC_ITEM_USE_CARDITEM_GUILD)					// 일반/고급 여단 카드
#define T_FC_ITEM_USE_CARDITEM_GUILD_NOTIFY				(MessageType_t)((T0_FC_ITEM<<8)|T1_FC_ITEM_USE_CARDITEM_GUILD_NOTIFY)			// 일반/고급 여단 카드
#define T_FC_ITEM_USE_CARDITEM_MONSTERSUMMON			(MessageType_t)((T0_FC_ITEM<<8)|T1_FC_ITEM_USE_CARDITEM_MONSTERSUMMON)			// 몬스터 소환 카드
#define T_FC_ITEM_USE_CARDITEM_MONSTERSUMMON_NOTIFY		(MessageType_t)((T0_FC_ITEM<<8)|T1_FC_ITEM_USE_CARDITEM_MONSTERSUMMON_NOTIFY)	// 몬스터 소환 카드
#define T_FC_ITEM_USE_CARDITEM_CHANGECHARACTERNAME		(MessageType_t)((T0_FC_ITEM<<8)|T1_FC_ITEM_USE_CARDITEM_CHANGECHARACTERNAME)	// 캐릭터 이름 변경 카드
#define T_FC_ITEM_USE_CARDITEM_CHANGECHARACTERNAME_NOTIFY	(MessageType_t)((T0_FC_ITEM<<8)|T1_FC_ITEM_USE_CARDITEM_CHANGECHARACTERNAME_NOTIFY)	// 캐릭터 이름 변경 카드
#define T_FC_ITEM_USE_CARDITEM_SKILLINITIALIZE			(MessageType_t)((T0_FC_ITEM<<8)|T1_FC_ITEM_USE_CARDITEM_SKILLINITIALIZE)		// 스킬 초기화 카드
#define T_FC_ITEM_USE_CARDITEM_SKILLINITIALIZE_NOTIFY	(MessageType_t)((T0_FC_ITEM<<8)|T1_FC_ITEM_USE_CARDITEM_SKILLINITIALIZE_NOTIFY)	// 스킬 초기화 카드
#define T_FC_ITEM_USE_CARDITEM_CHANGEPILOTFACE			(MessageType_t)((T0_FC_ITEM<<8)|T1_FC_ITEM_USE_CARDITEM_CHANGEPILOTFACE)		// 얼굴 변경 카드
#define T_FC_ITEM_USE_CARDITEM_CHANGEPILOTFACE_NOTIFY	(MessageType_t)((T0_FC_ITEM<<8)|T1_FC_ITEM_USE_CARDITEM_CHANGEPILOTFACE_NOTIFY)	// 얼굴 변경 카드
#define T_FC_ITEM_USE_INFLUENCE_BUFF					(MessageType_t)((T0_FC_ITEM<<8)|T1_FC_ITEM_USE_INFLUENCE_BUFF)			// C->F, 세력 버프		// 2006-04-21 by cmkwon
#define T_FC_ITEM_USE_INFLUENCE_BUFF_OK					(MessageType_t)((T0_FC_ITEM<<8)|T1_FC_ITEM_USE_INFLUENCE_BUFF_OK)		// F->C					// 2006-04-21 by cmkwon
#define T_FC_ITEM_USE_INFLUENCE_GAMEEVENT				(MessageType_t)((T0_FC_ITEM<<8)|T1_FC_ITEM_USE_INFLUENCE_GAMEEVENT)		// C->F, 세력 이벤트	// 2006-04-21 by cmkwon
#define T_FC_ITEM_USE_INFLUENCE_GAMEEVENT_OK			(MessageType_t)((T0_FC_ITEM<<8)|T1_FC_ITEM_USE_INFLUENCE_GAMEEVENT_OK)	// F->C					 // 2006-04-21 by cmkwon
#define T_FC_ITEM_USE_RANDOMBOX							(MessageType_t)((T0_FC_ITEM<<8)|T1_FC_ITEM_USE_RANDOMBOX)		// C->F, 2006-08-10 by cmkwon
#define T_FC_ITEM_USE_RANDOMBOX_OK						(MessageType_t)((T0_FC_ITEM<<8)|T1_FC_ITEM_USE_RANDOMBOX_OK)	// F->C(n), 2006-08-10 by cmkwon
#define T_FC_ITEM_USE_SKILL_SUPPORT_ITEM				(MessageType_t)((T0_FC_ITEM<<8)|T1_FC_ITEM_USE_SKILL_SUPPORT_ITEM)		// C->F, 2006-09-29 by cmkwon
#define T_FC_ITEM_USE_SKILL_SUPPORT_ITEM_OK				(MessageType_t)((T0_FC_ITEM<<8)|T1_FC_ITEM_USE_SKILL_SUPPORT_ITEM_OK)	// F->C, 2006-09-29 by cmkwon
#define T_FC_ITEM_USE_RANDOMBOX_OK_DONE					(MessageType_t)((T0_FC_ITEM<<8)|T1_FC_ITEM_USE_RANDOMBOX_OK_DONE)		// F->C, // 2008-08-26 by cmkwon, ItemAttribute 추가 - 패키지(Package) 아이템, (no body) 클라이언트는 이 메시지를 받고 락상태를 해제 한다.
#define T_FC_ITEM_USE_LUCKY_ITEM						(MessageType_t)((T0_FC_ITEM<<8)|T1_FC_ITEM_USE_LUCKY_ITEM)			// C->F, 2008-11-04 by dhjin, 럭키머신
#define T_FC_ITEM_USE_LUCKY_ITEM_OK						(MessageType_t)((T0_FC_ITEM<<8)|T1_FC_ITEM_USE_LUCKY_ITEM_OK)		// F->C, 2008-11-04 by dhjin, 럭키머신
#define T_FC_ITEM_CHANGED_SHAPEITEMNUM					(MessageType_t)((T0_FC_ITEM<<8)|T1_FC_ITEM_CHANGED_SHAPEITEMNUM)	// F->C, // 2009-08-26 by cmkwon, 그래픽 리소스 변경 시스템 구현 - 
#define T_FC_ITEM_CHANGED_EFFECTITEMNUM					(MessageType_t)((T0_FC_ITEM<<8)|T1_FC_ITEM_CHANGED_EFFECTITEMNUM)	// F->C, // 2009-08-26 by cmkwon, 그래픽 리소스 변경 시스템 구현 - 
#ifdef _INET_PET
#define T_FC_ITEM_USE_PET_SOCKET_ITEM					(MessageType_t)((T0_FC_ITEM<<8)|T1_FC_ITEM_USE_PET_SOCKET_ITEM)			// C->F, // 2011-09-20 by hskim, 파트너 시스템 2차
#define T_FC_ITEM_USE_PET_SOCKET_ITEM_OK				(MessageType_t)((T0_FC_ITEM<<8)|T1_FC_ITEM_USE_PET_SOCKET_ITEM_OK)		// F->C, // 2011-09-20 by hskim, 파트너 시스템 2차
#define T_FC_ITEM_CANCEL_PET_SOCKET_ITEM				(MessageType_t)((T0_FC_ITEM<<8)|T1_FC_ITEM_CANCEL_PET_SOCKET_ITEM)		// C->F, // 2011-09-20 by hskim, 파트너 시스템 2차
#define T_FC_ITEM_CANCEL_PET_SOCKET_ITEM_OK				(MessageType_t)((T0_FC_ITEM<<8)|T1_FC_ITEM_CANCEL_PET_SOCKET_ITEM_OK)	// F->C, // 2011-09-20 by hskim, 파트너 시스템 2차
#define T_FC_ITEM_NOTIFY_WINDOW_POSITION				(MessageType_t)((T0_FC_ITEM<<8)|T1_FC_ITEM_NOTIFY_WINDOW_POSITION)		// F->C, // 2011-09-20 by hskim, 파트너 시스템 2차
#endif
#ifdef _INET_LINK_CHAT
#define T_FC_GET_ITEM_BYCHARNAME8ITEMUID				(MessageType_t)((T0_FC_ITEM<<8)|T1_FC_GET_ITEM_BYCHARNAME8ITEMUID)
#define T_FC_GET_ITEM_BYCHARNAME8ITEMUID_OK				(MessageType_t)((T0_FC_ITEM<<8)|T1_FC_GET_ITEM_BYCHARNAME8ITEMUID_OK)
#endif
#define T_FC_SHOP_PUT_ITEM_HEADER				(MessageType_t)((T0_FC_SHOP<<8)|T1_FC_SHOP_PUT_ITEM_HEADER)
#define T_FC_SHOP_PUT_ITEM						(MessageType_t)((T0_FC_SHOP<<8)|T1_FC_SHOP_PUT_ITEM)
#define T_FC_SHOP_PUT_ITEM_DONE					(MessageType_t)((T0_FC_SHOP<<8)|T1_FC_SHOP_PUT_ITEM_DONE)
#define T_FC_SHOP_GET_ITEMINFO					(MessageType_t)((T0_FC_SHOP<<8)|T1_FC_SHOP_GET_ITEMINFO)
#define T_FC_SHOP_GET_ITEMINFO_OK				(MessageType_t)((T0_FC_SHOP<<8)|T1_FC_SHOP_GET_ITEMINFO_OK)
#define T_FC_SHOP_BUY_ITEM						(MessageType_t)((T0_FC_SHOP<<8)|T1_FC_SHOP_BUY_ITEM)
#define T_FC_SHOP_BUY_ITEM_OK					(MessageType_t)((T0_FC_SHOP<<8)|T1_FC_SHOP_BUY_ITEM_OK)
#define T_FC_SHOP_SELL_ITEM						(MessageType_t)((T0_FC_SHOP<<8)|T1_FC_SHOP_SELL_ITEM)
#define T_FC_SHOP_SELL_ITEM_OK					(MessageType_t)((T0_FC_SHOP<<8)|T1_FC_SHOP_SELL_ITEM_OK)
#define T_FC_SHOP_GET_USED_ITEM_PRICE			(MessageType_t)((T0_FC_SHOP<<8)|T1_FC_SHOP_GET_USED_ITEM_PRICE)
#define T_FC_SHOP_GET_USED_ITEM_PRICE_OK		(MessageType_t)((T0_FC_SHOP<<8)|T1_FC_SHOP_GET_USED_ITEM_PRICE_OK)
#define T_FC_SHOP_GET_SHOP_ITEM_LIST			(MessageType_t)((T0_FC_SHOP<<8)|T1_FC_SHOP_GET_SHOP_ITEM_LIST)		// C->F, 상점에서 파는 아이템의 리스트를 요청, 응답은 T_FC_SHOP_PUT_ITEM_XXX
#define T_FC_SHOP_REQUEST_REPAIR				(MessageType_t)((T0_FC_SHOP<<8)|T1_FC_SHOP_REQUEST_REPAIR)			// C->F, 기체 수리 요청, 2005-11-08 by cmkwon
#define T_FC_SHOP_REQUEST_REPAIR_OK				(MessageType_t)((T0_FC_SHOP<<8)|T1_FC_SHOP_REQUEST_REPAIR_OK)		// F->C, 기체 수리 요청 성공
#define T_FC_SHOP_BUY_CASH_ITEM					(MessageType_t)((T0_FC_SHOP<<8)|T1_FC_SHOP_BUY_CASH_ITEM)			// C->F, 유료 아이템 구입
#define T_FC_SHOP_BUY_CASH_ITEM_OK				(MessageType_t)((T0_FC_SHOP<<8)|T1_FC_SHOP_BUY_CASH_ITEM_OK)		// F->C, // 2007-11-13 by cmkwon, 선물하기 기능 추가 -유료 아이템 구입 성공
#define T_FC_SHOP_BUY_COLOR_ITEM				(MessageType_t)((T0_FC_SHOP<<8)|T1_FC_SHOP_BUY_COLOR_ITEM)			// C->F, 색상 아이템 구입// 2005-12-06 by cmkwon
#define T_FC_SHOP_BUY_COLOR_ITEM_OK				(MessageType_t)((T0_FC_SHOP<<8)|T1_FC_SHOP_BUY_COLOR_ITEM_OK)			// F->C, 색상 아이템 구입 성공// 2005-12-06 by cmkwon
#define T_FC_SHOP_BUY_WARPOINT_ITEM				(MessageType_t)((T0_FC_SHOP<<8)|T1_FC_SHOP_BUY_WARPOINT_ITEM)			// C->F, WarPoint 아이템 구입 // 2007-06-13 by dhjin
#define T_FC_SHOP_BUY_WARPOINT_ITEM_OK			(MessageType_t)((T0_FC_SHOP<<8)|T1_FC_SHOP_BUY_WARPOINT_ITEM_OK)		// F->C, // 2007-06-13 by dhjin
#define T_FC_SHOP_CHECK_GIVE_TARGET				(MessageType_t)((T0_FC_SHOP<<8)|T1_FC_SHOP_CHECK_GIVE_TARGET)		// C->F, // 2007-11-13 by cmkwon, 선물하기 기능 추가 - 선물받는 캐릭터 체크 요청 프로토콜
#define T_FC_SHOP_CHECK_GIVE_TARGET_OK			(MessageType_t)((T0_FC_SHOP<<8)|T1_FC_SHOP_CHECK_GIVE_TARGET_OK)	// F->C, // 2007-11-13 by cmkwon, 선물하기 기능 추가 -

#define T_FC_TRADE_REQUEST_TRADE				(MessageType_t)((T0_FC_TRADE<<8)|T1_FC_TRADE_REQUEST_TRADE)
#define T_FC_TRADE_REQUEST_TRADE_OK				(MessageType_t)((T0_FC_TRADE<<8)|T1_FC_TRADE_REQUEST_TRADE_OK)
#define T_FC_TRADE_CANCEL_REQUEST				(MessageType_t)((T0_FC_TRADE<<8)|T1_FC_TRADE_CANCEL_REQUEST)
#define T_FC_TRADE_CANCEL_REQUEST_OK			(MessageType_t)((T0_FC_TRADE<<8)|T1_FC_TRADE_CANCEL_REQUEST_OK)
#define T_FC_TRADE_ACCEPT_TRADE					(MessageType_t)((T0_FC_TRADE<<8)|T1_FC_TRADE_ACCEPT_TRADE)
#define T_FC_TRADE_ACCEPT_TRADE_OK				(MessageType_t)((T0_FC_TRADE<<8)|T1_FC_TRADE_ACCEPT_TRADE_OK)
#define T_FC_TRADE_REJECT_TRADE					(MessageType_t)((T0_FC_TRADE<<8)|T1_FC_TRADE_REJECT_TRADE)
#define T_FC_TRADE_REJECT_TRADE_OK				(MessageType_t)((T0_FC_TRADE<<8)|T1_FC_TRADE_REJECT_TRADE_OK)
#define T_FC_TRADE_REJECT_TRADING				(MessageType_t)((T0_FC_TRADE<<8)|T1_FC_TRADE_REJECT_TRADING)
#define T_FC_TRADE_SHOW_TRADE_WINDOW			(MessageType_t)((T0_FC_TRADE<<8)|T1_FC_TRADE_SHOW_TRADE_WINDOW)
#define T_FC_TRADE_TRANS_ITEM					(MessageType_t)((T0_FC_TRADE<<8)|T1_FC_TRADE_TRANS_ITEM)
#define T_FC_TRADE_TRANS_ITEM_OK				(MessageType_t)((T0_FC_TRADE<<8)|T1_FC_TRADE_TRANS_ITEM_OK)
#define T_FC_TRADE_SEE_ITEM						(MessageType_t)((T0_FC_TRADE<<8)|T1_FC_TRADE_SEE_ITEM)
#define T_FC_TRADE_SEE_ITEM_OK					(MessageType_t)((T0_FC_TRADE<<8)|T1_FC_TRADE_SEE_ITEM_OK)
#define T_FC_TRADE_OK_TRADE						(MessageType_t)((T0_FC_TRADE<<8)|T1_FC_TRADE_OK_TRADE)
#define T_FC_TRADE_OK_TRADE_OK					(MessageType_t)((T0_FC_TRADE<<8)|T1_FC_TRADE_OK_TRADE_OK)
#define T_FC_TRADE_CANCEL_TRADE					(MessageType_t)((T0_FC_TRADE<<8)|T1_FC_TRADE_CANCEL_TRADE)
#define T_FC_TRADE_CANCEL_TRADE_OK				(MessageType_t)((T0_FC_TRADE<<8)|T1_FC_TRADE_CANCEL_TRADE_OK)
#define T_FC_TRADE_INSERT_ITEM					(MessageType_t)((T0_FC_TRADE<<8)|T1_FC_TRADE_INSERT_ITEM)
#define T_FC_TRADE_DELETE_ITEM					(MessageType_t)((T0_FC_TRADE<<8)|T1_FC_TRADE_DELETE_ITEM)
#define T_FC_TRADE_UPDATE_ITEM_COUNT			(MessageType_t)((T0_FC_TRADE<<8)|T1_FC_TRADE_UPDATE_ITEM_COUNT)
#define T_FC_TRADE_OK_TRADE_NOTIFY				(MessageType_t)((T0_FC_TRADE<<8)|T1_FC_TRADE_OK_TRADE_NOTIFY)	// 2008-11-21 by cmkwon, 거래 승인 확인 시스템 구현 - F->C(2)
#define T_FC_TRADE_SEE_PET_DATA					(MessageType_t)((T0_FC_TRADE<<8)|T1_FC_TRADE_SEE_PET_DATA)				// 2010-06-15 by shcho&hslee 펫시스템 - 거래 시 펫 정보 전송
#define T_FC_TRADE_DELETE_CURRENT_PET_DATA		(MessageType_t)((T0_FC_TRADE<<8)|T1_FC_TRADE_DELETE_CURRENT_PET_DATA)	// 2010-06-15 by shcho&hslee 펫시스템 - 거래 시 펫 정보 전송
#define T_FC_TRADE_INSERT_CURRENT_PET_DATA 		(MessageType_t)((T0_FC_TRADE<<8)|T1_FC_TRADE_INSERT_CURRENT_PET_DATA)	// 2010-06-15 by shcho&hslee 펫시스템 - 거래 시 펫 정보 전송
#ifdef BONUS_STAT_ITEM
#define T_FC_TRADE_GET_BONUS_ITEM				(MessageType_t)((T0_FC_TRADE<<8)|T1_FC_TRADE_GET_BONUS_ITEM)
#define T_FC_TRADE_SEND_BONUS					(MessageType_t)((T0_FC_TRADE<<8)|T1_FC_TRADE_SEND_BONUS)
#endif
// T0_FC_COUNTDOWN
#define T_FC_COUNTDOWN_START					(MessageType_t)((T0_FC_COUNTDOWN<<8)|T1_FC_COUNTDOWN_START)	// F -> C
#define T_FC_COUNTDOWN_DONE						(MessageType_t)((T0_FC_COUNTDOWN<<8)|T1_FC_COUNTDOWN_DONE)	// C -> F

// T0_FC_OBJECT
#define T_FC_OBJECT_CHANGE_BODYCONDITION		(MessageType_t)((T0_FC_OBJECT<<8)|T1_FC_OBJECT_CHANGE_BODYCONDITION)	// C -> F
#define T_FC_OBJECT_CHANGE_BODYCONDITION_OK		(MessageType_t)((T0_FC_OBJECT<<8)|T1_FC_OBJECT_CHANGE_BODYCONDITION_OK)	// F -> C(n)

#define T_FC_AUCTION_REGISTER_ITEM				(MessageType_t)((T0_FC_AUCTION<<8)|T1_FC_AUCTION_REGISTER_ITEM)			// C->F, 경매 아이템 등록
#define T_FC_AUCTION_REGISTER_ITEM_OK			(MessageType_t)((T0_FC_AUCTION<<8)|T1_FC_AUCTION_REGISTER_ITEM_OK)		// F->C, 경매 아이템 등록 결과
#define T_FC_AUCTION_CANCEL_REGISTER			(MessageType_t)((T0_FC_AUCTION<<8)|T1_FC_AUCTION_CANCEL_REGISTER)		// C->F, 경매 아이템 등록 취소
#define T_FC_AUCTION_CANCEL_REGISTER_OK			(MessageType_t)((T0_FC_AUCTION<<8)|T1_FC_AUCTION_CANCEL_REGISTER_OK)	// F->C, 경매 아이템 등록 취소 결과
#define T_FC_AUCTION_BUY_ITEM					(MessageType_t)((T0_FC_AUCTION<<8)|T1_FC_AUCTION_BUY_ITEM)				// C->F, 경매 아이템 구매
#define T_FC_AUCTION_BUY_ITEM_OK				(MessageType_t)((T0_FC_AUCTION<<8)|T1_FC_AUCTION_BUY_ITEM_OK)			// F->C, 경매 아이템 구매 결과
#define T_FC_AUCTION_GET_ITEM_LIST				(MessageType_t)((T0_FC_AUCTION<<8)|T1_FC_AUCTION_GET_ITEM_LIST)			// C->F, 경매 아이템 목록 요청
#define T_FC_AUCTION_INSERT_ITEM				(MessageType_t)((T0_FC_AUCTION<<8)|T1_FC_AUCTION_INSERT_ITEM)			// F->C, 경매 아이템 목록 전송용
#define T_FC_AUCTION_PUT_ENCHANT				(MessageType_t)((T0_FC_AUCTION<<8)|T1_FC_AUCTION_PUT_ENCHANT)			// F->C, 경매 아이템의 인챈트 정보 전송용

#define T_FC_GUILD_GET_MAP_OWNER_INFO			(MessageType_t)((T0_FC_GUILD<<8)|T1_FC_GUILD_GET_MAP_OWNER_INFO)		// C->F, 맵 소유 정보 요청
#define T_FC_GUILD_GET_MAP_OWNER_INFO_OK		(MessageType_t)((T0_FC_GUILD<<8)|T1_FC_GUILD_GET_MAP_OWNER_INFO_OK)		// F->C, 맵 소유 정보 요청 결과
#define T_FC_GUILD_REQUEST_GUILD_WAR			(MessageType_t)((T0_FC_GUILD<<8)|T1_FC_GUILD_REQUEST_GUILD_WAR)			// C->F, 여단전 요청
#define T_FC_GUILD_REQUEST_GUILD_WAR_RESULT		(MessageType_t)((T0_FC_GUILD<<8)|T1_FC_GUILD_REQUEST_GUILD_WAR_RESULT)	// F->C, 여단전 요청 결과
#define T_FC_GUILD_GET_CHALLENGER_GUILD			(MessageType_t)((T0_FC_GUILD<<8)|T1_FC_GUILD_GET_CHALLENGER_GUILD)		// C->F, 여단전 요청 대기 길드 리스트 요청
#define T_FC_GUILD_GET_CHALLENGER_GUILD_OK		(MessageType_t)((T0_FC_GUILD<<8)|T1_FC_GUILD_GET_CHALLENGER_GUILD_OK)	// F->C, 여단전 요청 대기 길드 리스트 요청 결과
#define T_FC_GUILD_GET_WAR_INFO					(MessageType_t)((T0_FC_GUILD<<8)|T1_FC_GUILD_GET_WAR_INFO)				// C->F, 여단전 정보를 요청
#define T_FC_GUILD_GET_WAR_INFO_OK				(MessageType_t)((T0_FC_GUILD<<8)|T1_FC_GUILD_GET_WAR_INFO_OK)			// F->C, 여단전 정보를 전송
// 2004-12-10 by cmkwon, IM Server에서 처리한다, 프로토콜 삭제함
//#define T_FC_GUILD_SURRENDER_GUILD_WAR			(MessageType_t)((T0_FC_GUILD<<8)|T1_FC_GUILD_SURRENDER_GUILD_WAR)		// C->F, 여단전 항복, No Body
#define T_FC_GUILD_SUMMON_MEMBER				(MessageType_t)((T0_FC_GUILD<<8)|T1_FC_GUILD_SUMMON_MEMBER)	// F->C
#define T_FC_GUILD_SUMMON_MEMBER_OK				(MessageType_t)((T0_FC_GUILD<<8)|T1_FC_GUILD_SUMMON_MEMBER_OK)	// C(n)->F
#define T_FC_GUILD_DISMEMBER					(MessageType_t)((T0_FC_GUILD<<8)|T1_FC_GUILD_DISMEMBER)	// C->F

#define T_FI_GUILD_NOTIFY_START_WAR				(MessageType_t)((T0_FI_GUILD<<8)|T1_FI_GUILD_NOTIFY_START_WAR)			// F->I
#define T_FI_GUILD_NOTIFY_END_WAR				(MessageType_t)((T0_FI_GUILD<<8)|T1_FI_GUILD_NOTIFY_END_WAR)			// I->F
#define T_FI_GUILD_DELETE_GUILD					(MessageType_t)((T0_FI_GUILD<<8)|T1_FI_GUILD_DELETE_GUILD)				// F->I
#define T_FI_GUILD_ADD_GUILD_FAME				(MessageType_t)((T0_FI_GUILD<<8)|T1_FI_GUILD_ADD_GUILD_FAME)		// F->I, // 2005-12-27 by cmkwon
#define T_FI_GUILD_REG_DELETE_GUILD				(MessageType_t)((T0_FI_GUILD<<8)|T1_FI_GUILD_REG_DELETE_GUILD)			// I->F
// 2007-11-12 by dhjin, T_IC_GUILD_DISMEMBER => T_FI_GUILD_DISMEMBER 로 변경
#define T_FI_GUILD_DISMEMBER					(MessageType_t)((T0_FI_GUILD<<8)|T1_FI_GUILD_DISMEMBER)				// F->I, 여단 해체
#define T_FI_GUILD_OUTPOST						(MessageType_t)((T0_FI_GUILD<<8)|T1_FI_GUILD_OUTPOST)				// F->I, // 2008-05-20 by dhjin, EP3 - 여단 수정 사항 - 전진기지 관련


#define T_IC_GUILD_CREATE						(MessageType_t)((T0_IC_GUILD<<8)|T1_IC_GUILD_CREATE)				// C->I, 길드 생성 요청
#define T_IC_GUILD_CREATE_OK					(MessageType_t)((T0_IC_GUILD<<8)|T1_IC_GUILD_CREATE_OK)				// I->C, 길드 생성 결과
#define T_IC_GUILD_GET_GUILD_INFO				(MessageType_t)((T0_IC_GUILD<<8)|T1_IC_GUILD_GET_GUILD_INFO)		// C->I, 길드 정보 요청
#define T_IC_GUILD_GET_GUILD_INFO_OK			(MessageType_t)((T0_IC_GUILD<<8)|T1_IC_GUILD_GET_GUILD_INFO_OK)		// I->C, 길드 정보 결과
#define T_IC_GUILD_GET_OTHER_GUILD_INFO			(MessageType_t)((T0_IC_GUILD<<8)|T1_IC_GUILD_GET_OTHER_GUILD_INFO)		// C->I, 다른 길드 정보 요청
#define T_IC_GUILD_GET_OTHER_GUILD_INFO_OK		(MessageType_t)((T0_IC_GUILD<<8)|T1_IC_GUILD_GET_OTHER_GUILD_INFO_OK)	// I->C, 다른 길드 정보 결과
#define T_IC_GUILD_REQUEST_INVITE				(MessageType_t)((T0_IC_GUILD<<8)|T1_IC_GUILD_REQUEST_INVITE)		// C->I, 가입 권유, 서버에 요청
#define T_IC_GUILD_REQUEST_INVITE_QUESTION		(MessageType_t)((T0_IC_GUILD<<8)|T1_IC_GUILD_REQUEST_INVITE_QUESTION)	// I->C, 가입 권유, 대상자에게 전송
#define T_IC_GUILD_ACCEPT_INVITE				(MessageType_t)((T0_IC_GUILD<<8)|T1_IC_GUILD_ACCEPT_INVITE)			// C->I, 가입 승낙, 서버에 요청
#define T_IC_GUILD_ACCEPT_INVITE_OK				(MessageType_t)((T0_IC_GUILD<<8)|T1_IC_GUILD_ACCEPT_INVITE_OK)		// I->C, 가입 승낙, 길드원에게 전송
#define T_IC_GUILD_REJECT_INVITE				(MessageType_t)((T0_IC_GUILD<<8)|T1_IC_GUILD_REJECT_INVITE)			// C->I, 가입 거부, 서버에 요청
#define T_IC_GUILD_REJECT_INVITE_OK				(MessageType_t)((T0_IC_GUILD<<8)|T1_IC_GUILD_REJECT_INVITE_OK)		// I->C, 가입 거부, 대상자에게 전송
#define T_IC_GUILD_BAN_MEMBER					(MessageType_t)((T0_IC_GUILD<<8)|T1_IC_GUILD_BAN_MEMBER)			// C->I, 길드원 추방
#define T_IC_GUILD_BAN_MEMBER_OK				(MessageType_t)((T0_IC_GUILD<<8)|T1_IC_GUILD_BAN_MEMBER_OK)			// I->C, 길드원 추방 결과
#define T_IC_GUILD_LEAVE						(MessageType_t)((T0_IC_GUILD<<8)|T1_IC_GUILD_LEAVE)					// C->I, 여단 탈퇴
#define T_IC_GUILD_LEAVE_OK						(MessageType_t)((T0_IC_GUILD<<8)|T1_IC_GUILD_LEAVE_OK)				// I->C, 여단 탈퇴 결과
// 2007-11-12 by dhjin, FI로 변경
//#define T_IC_GUILD_DISMEMBER					(MessageType_t)((T0_IC_GUILD<<8)|T1_IC_GUILD_DISMEMBER)				// C->I, 여단 해체
#define T_IC_GUILD_DISMEMBER_OK					(MessageType_t)((T0_IC_GUILD<<8)|T1_IC_GUILD_DISMEMBER_OK)			// I->C, 여단 해체 결과
#define T_IC_GUILD_SET_MEMBER_STATE				(MessageType_t)((T0_IC_GUILD<<8)|T1_IC_GUILD_SET_MEMBER_STATE)		// I->C, 길드원의 상태 변화(ONLINE, OFFLINE 등)
#define T_IC_GUILD_CANCEL_DISMEMBER				(MessageType_t)((T0_IC_GUILD<<8)|T1_IC_GUILD_CANCEL_DISMEMBER)		// C->I, 여단 해체 취소 요청
#define T_IC_GUILD_CANCEL_DISMEMBER_OK			(MessageType_t)((T0_IC_GUILD<<8)|T1_IC_GUILD_CANCEL_DISMEMBER_OK)	// I->C, 여단 해체 취소 결과
#define T_IC_GUILD_GET_DISMEMBER_DATE			(MessageType_t)((T0_IC_GUILD<<8)|T1_IC_GUILD_GET_DISMEMBER_DATE)	// C->I, 길드 해산 시간 요청
#define T_IC_GUILD_GET_DISMEMBER_DATE_OK		(MessageType_t)((T0_IC_GUILD<<8)|T1_IC_GUILD_GET_DISMEMBER_DATE_OK)	// I->C, 길드 해산 시간 결과
#define T_IC_GUILD_CHANGE_GUILD_NAME			(MessageType_t)((T0_IC_GUILD<<8)|T1_IC_GUILD_CHANGE_GUILD_NAME)		// C->I, 여단 이름 변경 요청
#define T_IC_GUILD_CHANGE_GUILD_NAME_OK			(MessageType_t)((T0_IC_GUILD<<8)|T1_IC_GUILD_CHANGE_GUILD_NAME_OK)	// I->C, 여단 이름 변경 결과
#define T_IC_GUILD_GET_GUILD_MARK				(MessageType_t)((T0_IC_GUILD<<8)|T1_IC_GUILD_GET_GUILD_MARK)		// C->I, 여단 문양 요청
#define T_IC_GUILD_GET_GUILD_MARK_OK			(MessageType_t)((T0_IC_GUILD<<8)|T1_IC_GUILD_GET_GUILD_MARK_OK)		// I->C, 여단 문양 결과
#define T_IC_GUILD_SET_GUILD_MARK				(MessageType_t)((T0_IC_GUILD<<8)|T1_IC_GUILD_SET_GUILD_MARK)		// C->I, 여단 문양 성정 요청
#define T_IC_GUILD_SET_GUILD_MARK_OK			(MessageType_t)((T0_IC_GUILD<<8)|T1_IC_GUILD_SET_GUILD_MARK_OK)		// I->C, 여단 문양 성정 결과
#define T_IC_GUILD_SET_RANK						(MessageType_t)((T0_IC_GUILD<<8)|T1_IC_GUILD_SET_RANK)				// C->I, 계급 설정
#define T_IC_GUILD_SET_RANK_OK					(MessageType_t)((T0_IC_GUILD<<8)|T1_IC_GUILD_SET_RANK_OK)			// I->C, 계급 설정 결과
#define T_IC_GUILD_CHANGE_GUILD_STATE			(MessageType_t)((T0_IC_GUILD<<8)|T1_IC_GUILD_CHANGE_GUILD_STATE)	// I->C, 여단 상태 전송
#define T_IC_GUILD_LOADING_GUILD_DONE			(MessageType_t)((T0_IC_GUILD<<8)|T1_IC_GUILD_LOADING_GUILD_DONE)	// I->C, 서버측에서 길드 정보 로딩 완료 알림
#define T_IC_GUILD_WAR_READY					(MessageType_t)((T0_IC_GUILD<<8)|T1_IC_GUILD_WAR_READY)				// I->C, 여단전 시작 대기 상태를 알림
#define T_IC_GUILD_START_WAR					(MessageType_t)((T0_IC_GUILD<<8)|T1_IC_GUILD_START_WAR)				// I->C, 여단전 시작을 알림
#define T_IC_GUILD_END_WAR						(MessageType_t)((T0_IC_GUILD<<8)|T1_IC_GUILD_END_WAR)				// I->C, 여단전 종료를 알림
#define T_IC_GUILD_UPDATE_WAR_POINT				(MessageType_t)((T0_IC_GUILD<<8)|T1_IC_GUILD_UPDATE_WAR_POINT)		// I->C, 여단전 종료후 길드전승패를 DB Update이후에 길드원에게 전송
#define T_IC_GUILD_SURRENDER_GUILD_WAR			(MessageType_t)((T0_IC_GUILD<<8)|T1_IC_GUILD_SURRENDER_GUILD_WAR)	// C->I, 여단전 항복
#define T_IC_GUILD_CHANGE_MEMBER_CAPACITY		(MessageType_t)((T0_IC_GUILD<<8)|T1_IC_GUILD_CHANGE_MEMBER_CAPACITY)	// I->C, 여단 제한 인원 변경
#define T_IC_GUILD_GET_GUILD_MEMBER_LIST_OK		(MessageType_t)((T0_IC_GUILD<<8)|T1_IC_GUILD_GET_GUILD_MEMBER_LIST_OK)	// I->C, 여단원 리스트
#define T_IC_GUILD_END_WAR_ADMIN_NOTIFY			(MessageType_t)((T0_IC_GUILD<<8)|T1_IC_GUILD_END_WAR_ADMIN_NOTIFY)		// I->C(n)
#define T_IC_GUILD_MEMBER_LEVEL_UP				(MessageType_t)((T0_IC_GUILD<<8)|T1_IC_GUILD_MEMBER_LEVEL_UP)			// I->C(n), // 2008-05-20 by dhjin, EP3 - 여단 수정 사항 - 여단원 레벨업 관련
#define T_IC_GUILD_NEW_COMMANDER				(MessageType_t)((T0_IC_GUILD<<8)|T1_IC_GUILD_NEW_COMMANDER)				// C->I, // 2008-05-20 by dhjin, EP3 - 여단 수정 사항 - 여단장 위임
#define T_IC_GUILD_NOTICE_WRITE					(MessageType_t)((T0_IC_GUILD<<8)|T1_IC_GUILD_NOTICE_WRITE)				// C->I, // 2008-05-20 by dhjin, EP3 - 여단 수정 사항 - 여단 공지 작성
#define T_IC_GUILD_NOTICE_WRITE_OK				(MessageType_t)((T0_IC_GUILD<<8)|T1_IC_GUILD_NOTICE_WRITE_OK)			// I->C, // 2008-05-20 by dhjin, EP3 - 여단 수정 사항 - 여단 공지 작성 OK
#define T_IC_GUILD_GET_APPLICANT				(MessageType_t)((T0_IC_GUILD<<8)|T1_IC_GUILD_GET_APPLICANT)				// C->I, // 2008-05-27 by dhjin, EP3 - 여단 수정 사항 - 여단 지원자 관리
#define T_IC_GUILD_GET_APPLICANT_OK_HEADER		(MessageType_t)((T0_IC_GUILD<<8)|T1_IC_GUILD_GET_APPLICANT_OK_HEADER)	// I->C, // 2008-05-27 by dhjin, EP3 - 여단 수정 사항 - 여단 지원자 관리 OK
#define T_IC_GUILD_GET_APPLICANT_OK				(MessageType_t)((T0_IC_GUILD<<8)|T1_IC_GUILD_GET_APPLICANT_OK)			// I->C, // 2008-05-27 by dhjin, EP3 - 여단 수정 사항 - 여단 지원자 관리 OK
#define T_IC_GUILD_GET_APPLICANT_OK_DONE		(MessageType_t)((T0_IC_GUILD<<8)|T1_IC_GUILD_GET_APPLICANT_OK_DONE)		// I->C, // 2008-05-27 by dhjin, EP3 - 여단 수정 사항 - 여단 지원자 관리 OK
#define T_IC_GUILD_GET_INTRODUCTION				(MessageType_t)((T0_IC_GUILD<<8)|T1_IC_GUILD_GET_INTRODUCTION)			// C->I, // 2008-05-27 by dhjin, EP3 - 여단 수정 사항 - 여단 소개
#define T_IC_GUILD_GET_INTRODUCTION_OK			(MessageType_t)((T0_IC_GUILD<<8)|T1_IC_GUILD_GET_INTRODUCTION_OK)		// I->C, // 2008-05-27 by dhjin, EP3 - 여단 수정 사항 - 여단 소개 ok
#define T_IC_GUILD_GET_SELF_INTRODUCTION		(MessageType_t)((T0_IC_GUILD<<8)|T1_IC_GUILD_GET_SELF_INTRODUCTION)		// C->I, // 2008-05-27 by dhjin, EP3 - 여단 수정 사항 - 여단 지원자 소개서 
#define T_IC_GUILD_GET_SELF_INTRODUCTION_OK		(MessageType_t)((T0_IC_GUILD<<8)|T1_IC_GUILD_GET_SELF_INTRODUCTION_OK)	// I->C, // 2008-05-27 by dhjin, EP3 - 여단 수정 사항 - 여단 지원자 소개서 OK
#define T_IC_GUILD_SEARCH_INTRODUCTION				(MessageType_t)((T0_IC_GUILD<<8)|T1_IC_GUILD_SEARCH_INTRODUCTION)			// C->I, // 2008-05-27 by dhjin, EP3 - 여단 수정 사항 - 여단 소개 검색 
#define T_IC_GUILD_SEARCH_INTRODUCTION_OK_HEADER	(MessageType_t)((T0_IC_GUILD<<8)|T1_IC_GUILD_SEARCH_INTRODUCTION_OK_HEADER)	// I->C, // 2008-05-27 by dhjin, EP3 - 여단 수정 사항 - 여단 소개 검색 OK
#define T_IC_GUILD_SEARCH_INTRODUCTION_OK			(MessageType_t)((T0_IC_GUILD<<8)|T1_IC_GUILD_SEARCH_INTRODUCTION_OK)		// I->C, // 2008-05-27 by dhjin, EP3 - 여단 수정 사항 - 여단 소개 검색 OK
#define T_IC_GUILD_SEARCH_INTRODUCTION_OK_DONE		(MessageType_t)((T0_IC_GUILD<<8)|T1_IC_GUILD_SEARCH_INTRODUCTION_OK_DONE)	// I->C, // 2008-05-27 by dhjin, EP3 - 여단 수정 사항 - 여단 소개 검색 OK
#define T_IC_GUILD_UPDATE_INTRODUCTION			(MessageType_t)((T0_IC_GUILD<<8)|T1_IC_GUILD_UPDATE_INTRODUCTION)			// C->I, // 2008-05-27 by dhjin, EP3 - 여단 수정 사항 - 여단 소개 작성 
#define T_IC_GUILD_UPDATE_INTRODUCTION_OK		(MessageType_t)((T0_IC_GUILD<<8)|T1_IC_GUILD_UPDATE_INTRODUCTION_OK)		// I->C, // 2008-05-27 by dhjin, EP3 - 여단 수정 사항 - 여단 소개 작성 OK
#define T_IC_GUILD_DELETE_INTRODUCTION			(MessageType_t)((T0_IC_GUILD<<8)|T1_IC_GUILD_DELETE_INTRODUCTION)			// C->I, // 2008-05-27 by dhjin, EP3 - 여단 수정 사항 - 여단 소개 지우기  
#define T_IC_GUILD_DELETE_INTRODUCTION_OK		(MessageType_t)((T0_IC_GUILD<<8)|T1_IC_GUILD_DELETE_INTRODUCTION_OK)		// I->C, // 2008-05-27 by dhjin, EP3 - 여단 수정 사항 - 여단 소개 지우기 OK
#define T_IC_GUILD_UPDATE_SELFINTRODUCTION		(MessageType_t)((T0_IC_GUILD<<8)|T1_IC_GUILD_UPDATE_SELFINTRODUCTION)		// C->I, // 2008-05-27 by dhjin, EP3 - 여단 수정 사항 - 자기 소개 작성 
#define T_IC_GUILD_UPDATE_SELFINTRODUCTION_OK	(MessageType_t)((T0_IC_GUILD<<8)|T1_IC_GUILD_UPDATE_SELFINTRODUCTION_OK)	// I->C, // 2008-05-27 by dhjin, EP3 - 여단 수정 사항 - 자기 소개 작성 OK
#define T_IC_GUILD_DELETE_SELFINTRODUCTION		(MessageType_t)((T0_IC_GUILD<<8)|T1_IC_GUILD_DELETE_SELFINTRODUCTION)		// C->I, // 2008-05-27 by dhjin, EP3 - 여단 수정 사항 - 자기 소개 지우기  
#define T_IC_GUILD_DELETE_SELFINTRODUCTION_OK	(MessageType_t)((T0_IC_GUILD<<8)|T1_IC_GUILD_DELETE_SELFINTRODUCTION_OK)	// I->C, // 2008-05-27 by dhjin, EP3 - 여단 수정 사항 - 자기 소개 지우기 OK
#define T_IC_GUILD_CHANGE_FAME_RANK				(MessageType_t)((T0_IC_GUILD<<8)|T1_IC_GUILD_CHANGE_FAME_RANK)				// I->C, // 2008-06-10 by dhjin, EP3 - 여단 수정 사항 - 여단 명성 변경
#define T_IC_GUILD_APPLICANT_INVITE				(MessageType_t)((T0_IC_GUILD<<8)|T1_IC_GUILD_APPLICANT_INVITE)				// C->I, // 2008-06-12 by dhjin, EP3 - 여단 수정 사항 - 지원자 관리에서 여단원 초대
#define T_IC_GUILD_APPLICANT_INVITE_OK			(MessageType_t)((T0_IC_GUILD<<8)|T1_IC_GUILD_APPLICANT_INVITE_OK)			// I->C, // 2008-06-12 by dhjin, EP3 - 여단 수정 사항 - 지원자 관리에서 여단원 초대 OK
#define T_IC_GUILD_APPLICANT_REJECT_INVITE		(MessageType_t)((T0_IC_GUILD<<8)|T1_IC_GUILD_APPLICANT_REJECT_INVITE)		// C->I, // 2008-06-12 by dhjin, EP3 - 여단 수정 사항 - 지원자 관리에서 여단원 초대 거부 
#define T_IC_GUILD_APPLICANT_REJECT_INVITE_OK	(MessageType_t)((T0_IC_GUILD<<8)|T1_IC_GUILD_APPLICANT_REJECT_INVITE_OK)	// I->C, // 2008-06-12 by dhjin, EP3 - 여단 수정 사항 - 지원자 관리에서 여단원 초대 거부 OK
#define T_IC_GUILD_CHANGE_MEMBERSHIP			(MessageType_t)((T0_IC_GUILD<<8)|T1_IC_GUILD_CHANGE_MEMBERSHIP)				// I->C, // 2008-06-20 by dhjin, EP3 - 여단 수정 사항 - 여단장 맴버쉽 정보 전송

#define T_FC_SKILL_USE_SKILLPOINT				(MessageType_t)((T0_FC_SKILL<<8)|T1_FC_SKILL_USE_SKILLPOINT)
#define T_FC_SKILL_USE_SKILLPOINT_OK			(MessageType_t)((T0_FC_SKILL<<8)|T1_FC_SKILL_USE_SKILLPOINT_OK)
#define T_FC_SKILL_SETUP_SKILL					(MessageType_t)((T0_FC_SKILL<<8)|T1_FC_SKILL_SETUP_SKILL)
#define T_FC_SKILL_SETUP_SKILL_OK_HEADER		(MessageType_t)((T0_FC_SKILL<<8)|T1_FC_SKILL_SETUP_SKILL_OK_HEADER)
#define T_FC_SKILL_SETUP_SKILL_OK				(MessageType_t)((T0_FC_SKILL<<8)|T1_FC_SKILL_SETUP_SKILL_OK)
#define T_FC_SKILL_SETUP_SKILL_OK_DONE			(MessageType_t)((T0_FC_SKILL<<8)|T1_FC_SKILL_SETUP_SKILL_OK_DONE)
#define T_FC_SKILL_USE_SKILL					(MessageType_t)((T0_FC_SKILL<<8)|T1_FC_SKILL_USE_SKILL)
#define T_FC_SKILL_USE_SKILL_OK					(MessageType_t)((T0_FC_SKILL<<8)|T1_FC_SKILL_USE_SKILL_OK)
#define T_FC_SKILL_CANCEL_SKILL					(MessageType_t)((T0_FC_SKILL<<8)|T1_FC_SKILL_CANCEL_SKILL)
#define T_FC_SKILL_CANCEL_SKILL_OK				(MessageType_t)((T0_FC_SKILL<<8)|T1_FC_SKILL_CANCEL_SKILL_OK)
#define T_FC_SKILL_INVALIDATE_SKILL				(MessageType_t)((T0_FC_SKILL<<8)|T1_FC_SKILL_INVALIDATE_SKILL)
#define T_FC_SKILL_PREPARE_USE					(MessageType_t)((T0_FC_SKILL<<8)|T1_FC_SKILL_PREPARE_USE)
#define T_FC_SKILL_PREPARE_USE_OK				(MessageType_t)((T0_FC_SKILL<<8)|T1_FC_SKILL_PREPARE_USE_OK)
#define T_FC_SKILL_CANCEL_PREPARE				(MessageType_t)((T0_FC_SKILL<<8)|T1_FC_SKILL_CANCEL_PREPARE)
#define T_FC_SKILL_CANCEL_PREPARE_OK			(MessageType_t)((T0_FC_SKILL<<8)|T1_FC_SKILL_CANCEL_PREPARE_OK)
#define T_FC_SKILL_CONFIRM_USE					(MessageType_t)((T0_FC_SKILL<<8)|T1_FC_SKILL_CONFIRM_USE)		// 2005-12-02 by cmkwon, C->F, F->C
#define T_FC_SKILL_CONFIRM_USE_ACK				(MessageType_t)((T0_FC_SKILL<<8)|T1_FC_SKILL_CONFIRM_USE_ACK)	// 2005-12-02 by cmkwon, C->F, F->C

#define T_FN_SKILL_USE_SKILL					(MessageType_t)((T0_FN_SKILL<<8)|T1_FN_SKILL_USE_SKILL)
#define T_FN_SKILL_USE_SKILL_OK					(MessageType_t)((T0_FN_SKILL<<8)|T1_FN_SKILL_USE_SKILL_OK)

#define T_FC_QUEST_REQUEST_START				(MessageType_t)((T0_FC_QUEST<<8)|T1_FC_QUEST_REQUEST_START)			// C->F, Quest 시작을 요청
#define T_FC_QUEST_REQUEST_START_RESULT			(MessageType_t)((T0_FC_QUEST<<8)|T1_FC_QUEST_REQUEST_START_RESULT)	// F->C, Quest 시작을 승인, Client는 이 MSG를 받으면 Pre NPCTalk을 로딩한다
#define T_FC_QUEST_ACCEPT_QUEST					(MessageType_t)((T0_FC_QUEST<<8)|T1_FC_QUEST_ACCEPT_QUEST)			// C->F, Quest를 받아들임
#define T_FC_QUEST_CANCEL_QUEST					(MessageType_t)((T0_FC_QUEST<<8)|T1_FC_QUEST_CANCEL_QUEST)			// C->F, Quest를 거절함(거절할 수 없는 Quest도 존재함)
#define T_FC_QUEST_REQUEST_SUCCESS				(MessageType_t)((T0_FC_QUEST<<8)|T1_FC_QUEST_REQUEST_SUCCESS)		// C->F, Quest 결과 검증을 요청
#define T_FC_QUEST_REQUEST_SUCCESS_RESULT		(MessageType_t)((T0_FC_QUEST<<8)|T1_FC_QUEST_REQUEST_SUCCESS_RESULT)// F->C, Quest 결과(완료)를 알림, Client는 이 MSG를 받으면 quest를 종료하고 After NPCTalk을 로딩한다
#define T_FC_QUEST_PUT_ALL_QUEST_HEADER			(MessageType_t)((T0_FC_QUEST<<8)|T1_FC_QUEST_PUT_ALL_QUEST_HEADER)	// F->C, 완료되거나 진행중인 모든 퀘스트를 전송
#define T_FC_QUEST_PUT_ALL_QUEST				(MessageType_t)((T0_FC_QUEST<<8)|T1_FC_QUEST_PUT_ALL_QUEST)			// F->C, 완료되거나 진행중인 모든 퀘스트를 전송
#define T_FC_QUEST_PUT_ALL_QUEST_DONE			(MessageType_t)((T0_FC_QUEST<<8)|T1_FC_QUEST_PUT_ALL_QUEST_DONE)	// F->C, 완료되거나 진행중인 모든 퀘스트를 전송
#define T_FC_QUEST_DISCARD_QUEST				(MessageType_t)((T0_FC_QUEST<<8)|T1_FC_QUEST_DISCARD_QUEST)			// C->F, 이미 시작된 퀘스트를 포기함
#define T_FC_QUEST_DISCARD_QUEST_OK				(MessageType_t)((T0_FC_QUEST<<8)|T1_FC_QUEST_DISCARD_QUEST_OK)		// F->C, 이미 시작된 퀘스트를 포기함에 대한 결과
#define T_FC_QUEST_MOVE_QUEST_MAP				(MessageType_t)((T0_FC_QUEST<<8)|T1_FC_QUEST_MOVE_QUEST_MAP)		// C->F, 진행중인 미션맵으로 이동
#define T_FC_QUEST_PUT_ALL_QUEST_MONSTER_COUNT_HEADER	(MessageType_t)((T0_FC_QUEST<<8)|T1_FC_QUEST_PUT_ALL_QUEST_MONSTER_COUNT_HEADER)	// F->C, // 2005-10-25 by cmkwon
#define T_FC_QUEST_PUT_ALL_QUEST_MONSTER_COUNT	(MessageType_t)((T0_FC_QUEST<<8)|T1_FC_QUEST_PUT_ALL_QUEST_MONSTER_COUNT)	// F->C, // 2005-10-25 by cmkwon
#define T_FC_QUEST_PUT_ALL_QUEST_MONSTER_COUNT_DONE	(MessageType_t)((T0_FC_QUEST<<8)|T1_FC_QUEST_PUT_ALL_QUEST_MONSTER_COUNT_DONE)	// F->C, // 2005-10-25 by cmkwon
#define T_FC_QUEST_UPDATE_MONSTER_COUNT			(MessageType_t)((T0_FC_QUEST<<8)|T1_FC_QUEST_UPDATE_MONSTER_COUNT)			// F->C, // 2005-10-25 by cmkwon
#define T_FC_QUEST_REQUEST_SUCCESS_CHECK		(MessageType_t)((T0_FC_QUEST<<8)|T1_FC_QUEST_REQUEST_SUCCESS_CHECK)			// C->F, // 2006-03-24 by cmkwon
#define T_FC_QUEST_REQUEST_SUCCESS_CHECK_RESULT	(MessageType_t)((T0_FC_QUEST<<8)|T1_FC_QUEST_REQUEST_SUCCESS_CHECK_RESULT)	// F->C(n), // 2006-03-24 by cmkwon
#define T_FC_QUEST_REQUEST_PARTY_WARP			(MessageType_t)((T0_FC_QUEST<<8)|T1_FC_QUEST_REQUEST_PARTY_WARP)	// F->C(n), // 2006-10-16 by cmkwon
#define T_FC_QUEST_REQUEST_PARTY_WARP_ACK		(MessageType_t)((T0_FC_QUEST<<8)|T1_FC_QUEST_REQUEST_PARTY_WARP_ACK)	// C(n)->F, // 2006-10-16 by cmkwon

//#define T_FC_SYNC_PRIMARY_REATTACK_OK			(MessageType_t)((T0_FC_SYNC<<8)|T1_FC_SYNC_PRIMARY_REATTACK_OK)
//#define T_FC_SYNC_SECONDARY_REATTACK_OK			(MessageType_t)((T0_FC_SYNC<<8)|T1_FC_SYNC_SECONDARY_REATTACK_OK)
//#define T_FC_SYNC_SKILL_REUSE_OK				(MessageType_t)((T0_FC_SYNC<<8)|T1_FC_SYNC_SKILL_REUSE_OK)

#define T_FC_INFO_GET_MONSTER_INFO				(MessageType_t)((T0_FC_INFO<<8)|T1_FC_INFO_GET_MONSTER_INFO)
#define T_FC_INFO_GET_MONSTER_INFO_OK			(MessageType_t)((T0_FC_INFO<<8)|T1_FC_INFO_GET_MONSTER_INFO_OK)
#define T_FC_INFO_GET_MAPOBJECT_INFO			(MessageType_t)((T0_FC_INFO<<8)|T1_FC_INFO_GET_MAPOBJECT_INFO)
#define T_FC_INFO_GET_MAPOBJECT_INFO_OK			(MessageType_t)((T0_FC_INFO<<8)|T1_FC_INFO_GET_MAPOBJECT_INFO_OK)
#ifdef _INET_ANTICHEAT
#define T_FC_INET_GET_ITEM_INFO					(MessageType_t)((T0_FC_INFO<<8)|T1_FC_INET_GET_ITEM_INFO)
#define T_FC_INET_GET_ITEM_INFO_OK				(MessageType_t)((T0_FC_INFO<<8)|T1_FC_INET_GET_ITEM_INFO_OK)
#endif
#define T_FC_INFO_GET_ITEM_INFO					(MessageType_t)((T0_FC_INFO<<8)|T1_FC_INFO_GET_ITEM_INFO)
#define T_FC_INFO_GET_ITEM_INFO_OK				(MessageType_t)((T0_FC_INFO<<8)|T1_FC_INFO_GET_ITEM_INFO_OK)
#define T_FC_INFO_GET_RARE_ITEM_INFO			(MessageType_t)((T0_FC_INFO<<8)|T1_FC_INFO_GET_RARE_ITEM_INFO)
#define T_FC_INFO_GET_RARE_ITEM_INFO_OK			(MessageType_t)((T0_FC_INFO<<8)|T1_FC_INFO_GET_RARE_ITEM_INFO_OK)
#define T_FC_INFO_GET_BUILDINGNPC_INFO			(MessageType_t)((T0_FC_INFO<<8)|T1_FC_INFO_GET_BUILDINGNPC_INFO)
#define T_FC_INFO_GET_BUILDINGNPC_INFO_OK		(MessageType_t)((T0_FC_INFO<<8)|T1_FC_INFO_GET_BUILDINGNPC_INFO_OK)
#define T_FC_INFO_GET_SIMPLE_ITEM_INFO			(MessageType_t)((T0_FC_INFO<<8)|T1_FC_INFO_GET_SIMPLE_ITEM_INFO)
#define T_FC_INFO_GET_SIMPLE_ITEM_INFO_OK		(MessageType_t)((T0_FC_INFO<<8)|T1_FC_INFO_GET_SIMPLE_ITEM_INFO_OK)
#define T_FC_INFO_GET_ENCHANT_COST				(MessageType_t)((T0_FC_INFO<<8)|T1_FC_INFO_GET_ENCHANT_COST)		// C->F, 인챈트 가격을 요청
#define T_FC_INFO_GET_ENCHANT_COST_OK			(MessageType_t)((T0_FC_INFO<<8)|T1_FC_INFO_GET_ENCHANT_COST_OK)		// F->C, 인챈트 가격을 전송

#ifdef _INET_ENCHANT_CHANCE
	#define T_FC_INFO_GET_ENCHANT_CHANCE				(MessageType_t)((T0_FC_INFO<<8)|T1_FC_INFO_GET_ENCHANT_CHANCE) //0xe5
	#define T_FC_INFO_GET_ENCHANT_CHANCE_OK			(MessageType_t)((T0_FC_INFO<<8)|T1_FC_INFO_GET_ENCHANT_CHANCE_OK) //0xe6
#endif

#define T_FC_INFO_GET_CURRENT_MAP_INFO				(MessageType_t)((T0_FC_INFO<<8)|T1_FC_INFO_GET_CURRENT_MAP_INFO)
#define T_FC_INFO_GET_CURRENT_MAP_INFO_OK			(MessageType_t)((T0_FC_INFO<<8)|T1_FC_INFO_GET_CURRENT_MAP_INFO_OK)
#define T_FC_INFO_GET_HAPPY_HOUR_EVENT_INFO_OK		(MessageType_t)((T0_FC_INFO<<8)|T1_FC_INFO_GET_HAPPY_HOUR_EVENT_INFO_OK)	// F->C
#define T_FC_INFO_GET_GAME_EVENT_INFO_OK			(MessageType_t)((T0_FC_INFO<<8)|T1_FC_INFO_GET_GAME_EVENT_INFO_OK)	// F->C
#define T_FC_INFO_GET_SERVER_DATE_TIME				(MessageType_t)((T0_FC_INFO<<8)|T1_FC_INFO_GET_SERVER_DATE_TIME)		// 2006-10-11 by cmkwon, C->F
#define T_FC_INFO_GET_SERVER_DATE_TIME_OK			(MessageType_t)((T0_FC_INFO<<8)|T1_FC_INFO_GET_SERVER_DATE_TIME_OK)		// 2006-10-11 by cmkwon, F->C
#define T_FC_INFO_GET_HAPPY_HOUR_EVENT_INFO			(MessageType_t)((T0_FC_INFO<<8)|T1_FC_INFO_GET_HAPPY_HOUR_EVENT_INFO)	// 2007-10-30 by cmkwon, 세력별 해피아워 이벤트 구현 - C->F
#define T_FC_INFO_GET_HAPPY_HOUR_EVENT_INFO_BY_LEVEL		(MessageType_t)((T0_FC_INFO<<8)|T1_FC_INFO_GET_HAPPY_HOUR_EVENT_INFO_BY_LEVEL)	// 2008-03-14 by dhjin, Level별 해피아워 이벤트 구현 -

#define T_FC_INFO_CHECK_RESOBJ_CHECKSUM				(MessageType_t)((T0_FC_INFO<<8)|T1_FC_INFO_CHECK_RESOBJ_CHECKSUM)		// 2007-05-28 by cmkwon, C->F


#define T_FC_REQUEST_REQUEST					(MessageType_t)((T0_FC_REQUEST<<8)|T1_FC_REQUEST_REQUEST)			// C->F, 요청
#define T_FC_REQUEST_REQUEST_OK					(MessageType_t)((T0_FC_REQUEST<<8)|T1_FC_REQUEST_REQUEST_OK)		// F->C, 요청을 전달
#define T_FC_REQUEST_ACCEPT_REQUEST				(MessageType_t)((T0_FC_REQUEST<<8)|T1_FC_REQUEST_ACCEPT_REQUEST)	// C->F, 승낙
#define T_FC_REQUEST_ACCEPT_REQUEST_OK			(MessageType_t)((T0_FC_REQUEST<<8)|T1_FC_REQUEST_ACCEPT_REQUEST_OK)	// F->C, 승낙을 전달
#define T_FC_REQUEST_REJECT_REQUEST				(MessageType_t)((T0_FC_REQUEST<<8)|T1_FC_REQUEST_REJECT_REQUEST)	// C->F, 거절
#define T_FC_REQUEST_REJECT_REQUEST_OK			(MessageType_t)((T0_FC_REQUEST<<8)|T1_FC_REQUEST_REJECT_REQUEST_OK)	// F->C, 거절을 전달
#define T_FC_REQUEST_CANCEL_REQUEST				(MessageType_t)((T0_FC_REQUEST<<8)|T1_FC_REQUEST_CANCEL_REQUEST)	// C->F, 요청 취소됨

#define T_FC_CITY_GET_BUILDING_LIST				(MessageType_t)((T0_FC_CITY<<8)|T1_FC_CITY_GET_BUILDING_LIST)			// C->F, 도시의 건물(상점 등) 리스트 전송 요청
#define T_FC_CITY_GET_BUILDING_LIST_OK			(MessageType_t)((T0_FC_CITY<<8)|T1_FC_CITY_GET_BUILDING_LIST_OK)		// F->C, 도시의 건물(상점 등) 리스트 전송
//#define T_FC_CITY_GET_WARP_TARGET_MAP_LIST		(MessageType_t)((T0_FC_CITY<<8)|T1_FC_CITY_GET_WARP_TARGET_MAP_LIST)	// C->F, 도시에서 워프해나갈 수 있는 맵의 리스트 전송 요청
//#define T_FC_CITY_GET_WARP_TARGET_MAP_LIST_OK	(MessageType_t)((T0_FC_CITY<<8)|T1_FC_CITY_GET_WARP_TARGET_MAP_LIST_OK)	// F->C, 도시에서 워프해나갈 수 있는 맵의 리스트 전송
#define T_FC_CITY_REQUEST_ENTER_BUILDING		(MessageType_t)((T0_FC_CITY<<8)|T1_FC_CITY_REQUEST_ENTER_BUILDING)		// C->F, 상점 들어갈 것을 요청
#define T_FC_CITY_REQUEST_ENTER_BUILDING_OK		(MessageType_t)((T0_FC_CITY<<8)|T1_FC_CITY_REQUEST_ENTER_BUILDING_OK)	// F->C, 상점 진입 완료를 알림 
//#define T_FC_CITY_REQUEST_WARP					(MessageType_t)((T0_FC_CITY<<8)|T1_FC_CITY_REQUEST_WARP)				// C->F, 도시에서 워프해서 나가기 위한 요청, 응답은 채널이 하나일때와 여러개일때가 다르다
#define T_FC_CITY_CHECK_WARP_STATE				(MessageType_t)((T0_FC_CITY<<8)|T1_FC_CITY_CHECK_WARP_STATE)			// C->F, 워프 가능한 상태인지 확인 요청, No Body
#define T_FC_CITY_CHECK_WARP_STATE_OK			(MessageType_t)((T0_FC_CITY<<8)|T1_FC_CITY_CHECK_WARP_STATE_OK)			// F->C, 워프 가능한 상태인지에 대한 결과

#define T_FC_TIMER_START_TIMER					(MessageType_t)((T0_FC_TIMER<<8)|T1_FC_TIMER_START_TIMER)		// F->C, TIMER_EVENT 시작
#define T_FC_TIMER_STOP_TIMER					(MessageType_t)((T0_FC_TIMER<<8)|T1_FC_TIMER_STOP_TIMER)		// F->C, TIMER_EVENT 정지
#define T_FC_TIMER_UPDATE_TIMER					(MessageType_t)((T0_FC_TIMER<<8)|T1_FC_TIMER_UPDATE_TIMER)		// F->C, TIMER_EVENT 갱신(시간 연장)
#define T_FC_TIMER_PAUSE_TIMER					(MessageType_t)((T0_FC_TIMER<<8)|T1_FC_TIMER_PAUSE_TIMER)		// F->C, TIMER_EVENT 일시 정지
#define T_FC_TIMER_CONTINUE_TIMER				(MessageType_t)((T0_FC_TIMER<<8)|T1_FC_TIMER_CONTINUE_TIMER)	// F->C, TIMER_EVENT 재시작
#define T_FC_TIMER_TIMEOUT						(MessageType_t)((T0_FC_TIMER<<8)|T1_FC_TIMER_TIMEOUT)			// C->F, 시간이 다 됨을 알림

#define T_FC_CLIENT_REPORT						(MessageType_t)((T0_FC_CLIENT_REPORT<<8)|T1_FC_CLIENT_REPORT)
#define T_IC_STRING_128							(MessageType_t)((T0_IC_STRING<<8)|T1_IC_STRING_128)
#define T_IC_STRING_256							(MessageType_t)((T0_IC_STRING<<8)|T1_IC_STRING_256)
#define T_IC_STRING_512							(MessageType_t)((T0_IC_STRING<<8)|T1_IC_STRING_512)

#define T_FC_STRING_128							(MessageType_t)((T0_FC_STRING<<8)|T1_FC_STRING_128)
#define T_FC_STRING_256							(MessageType_t)((T0_FC_STRING<<8)|T1_FC_STRING_256)
#define T_FC_STRING_512							(MessageType_t)((T0_FC_STRING<<8)|T1_FC_STRING_512)

#define T_FI_ADMIN_GET_CHARACTER_INFO			(MessageType_t)((T0_FI_ADMIN<<8)|T1_FI_ADMIN_GET_CHARACTER_INFO)
#define T_FI_ADMIN_GET_CHARACTER_INFO_OK		(MessageType_t)((T0_FI_ADMIN<<8)|T1_FI_ADMIN_GET_CHARACTER_INFO_OK)
#define T_FI_ADMIN_CALL_CHARACTER				(MessageType_t)((T0_FI_ADMIN<<8)|T1_FI_ADMIN_CALL_CHARACTER)		// I -> F
#define T_FI_ADMIN_MOVETO_CHARACTER				(MessageType_t)((T0_FI_ADMIN<<8)|T1_FI_ADMIN_MOVETO_CHARACTER)
#define T_FI_ADMIN_PRINT_DEBUG_MSG				(MessageType_t)((T0_FI_ADMIN<<8)|T1_FI_ADMIN_PRINT_DEBUG_MSG)
#define T_FI_ADMIN_CHANGE_WEATHER				(MessageType_t)((T0_FI_ADMIN<<8)|T1_FI_ADMIN_CHANGE_WEATHER)

#define T_IC_ADMIN_CALL_CHARACTER				(MessageType_t)((T0_IC_ADMIN<<8)|T1_IC_ADMIN_CALL_CHARACTER)		// C -> I
#define T_IC_ADMIN_GET_SERVER_STAT				(MessageType_t)((T0_IC_ADMIN<<8)|T1_IC_ADMIN_GET_SERVER_STAT)		// C -> I, NO BODY
#define T_IC_ADMIN_GET_SERVER_STAT_OK			(MessageType_t)((T0_IC_ADMIN<<8)|T1_IC_ADMIN_GET_SERVER_STAT_OK)	// I -> C
#define T_IC_ADMIN_CALL_GUILD					(MessageType_t)((T0_IC_ADMIN<<8)|T1_IC_ADMIN_CALL_GUILD)			// I -> C
#define T_IC_ADMIN_CALLGM_INFO_OK				(MessageType_t)((T0_IC_ADMIN<<8)|T1_IC_ADMIN_CALLGM_INFO_OK)		// I -> C
#define T_IC_ADMIN_CALLGM_VIEW_OK				(MessageType_t)((T0_IC_ADMIN<<8)|T1_IC_ADMIN_CALLGM_VIEW_OK)		// I -> C
#define T_IC_ADMIN_CALLGM_BRING_OK				(MessageType_t)((T0_IC_ADMIN<<8)|T1_IC_ADMIN_CALLGM_BRING_OK)		// I -> C
#define T_IC_ADMIN_COMMAND_EVO_WHO_OK			(MessageType_t)((T0_IC_ADMIN<<8)|T1_IC_ADMIN_COMMAND_EVO_WHO_OK)

#define T_FC_ADMIN_GET_SERVER_STAT				(MessageType_t)((T0_FC_ADMIN<<8)|T1_FC_ADMIN_GET_SERVER_STAT)		// C -> F, NO BODY
#define T_FC_ADMIN_GET_SERVER_STAT_OK			(MessageType_t)((T0_FC_ADMIN<<8)|T1_FC_ADMIN_GET_SERVER_STAT_OK)	// F -> C


// T0_IC_COUNTDOWN
#define T_IC_COUNTDOWN_START					(MessageType_t)((T0_IC_COUNTDOWN<<8)|T1_IC_COUNTDOWN_START)		// I -> C
#define T_IC_COUNTDOWN_DONE						(MessageType_t)((T0_IC_COUNTDOWN<<8)|T1_IC_COUNTDOWN_DONE)		// C -> I

//////////////////////////////////////////////////////////////////////////
// 2008-06-17 by dhjin, EP3 VOIP -
#define T_IC_VOIP_SET							(MessageType_t)((T0_IC_VOIP<<8)|T1_IC_VOIP_SET)					// C -> I, 2008-06-17 by dhjin, EP3 VOIP - 설정 
#define T_IC_VOIP_SET_OK						(MessageType_t)((T0_IC_VOIP<<8)|T1_IC_VOIP_SET_OK)				// I -> C, 2008-06-17 by dhjin, EP3 VOIP - 설정 정보 전송

//////////////////////////////////////////////////////////////////////////
// T0_IC_CHATROOM
#define T_IC_CHATROOM_CREATE					(MessageType_t)((T0_IC_CHATROOM<<8)|T1_IC_CHATROOM_CREATE)					// C -> I, 2008-06-16 by dhjin, EP3 채팅방 - 채팅방 생성
#define T_IC_CHATROOM_CREATE_OK					(MessageType_t)((T0_IC_CHATROOM<<8)|T1_IC_CHATROOM_CREATE_OK)				// I -> C, 2008-06-16 by dhjin, EP3 채팅방 - 채팅방 생성 OK
#define T_IC_CHATROOM_LIST_INFO					(MessageType_t)((T0_IC_CHATROOM<<8)|T1_IC_CHATROOM_LIST_INFO)				// C -> I, 2008-06-16 by dhjin, EP3 채팅방 - 채팅방 목록 가져오기
#define T_IC_CHATROOM_LIST_INFO_OK				(MessageType_t)((T0_IC_CHATROOM<<8)|T1_IC_CHATROOM_LIST_INFO_OK)			// I -> C, 2008-06-16 by dhjin, EP3 채팅방 - 채팅방 목록 가져오기 OK
#define T_IC_CHATROOM_REQUEST_INVITE			(MessageType_t)((T0_IC_CHATROOM<<8)|T1_IC_CHATROOM_REQUEST_INVITE)			// C -> I, 2008-06-16 by dhjin, EP3 채팅방 - 채팅방 초대 요청 
#define T_IC_CHATROOM_REQUEST_INVITE_QUESTION	(MessageType_t)((T0_IC_CHATROOM<<8)|T1_IC_CHATROOM_REQUEST_INVITE_QUESTION)	// I -> C, 2008-06-16 by dhjin, EP3 채팅방 - 채팅방 초대 대상에게 전송
#define T_IC_CHATROOM_JOIN						(MessageType_t)((T0_IC_CHATROOM<<8)|T1_IC_CHATROOM_JOIN)					// C -> I, 2008-06-16 by dhjin, EP3 채팅방 - 채팅방 참여
#define T_IC_CHATROOM_JOIN_OK					(MessageType_t)((T0_IC_CHATROOM<<8)|T1_IC_CHATROOM_JOIN_OK)					// I -> C, 2008-06-16 by dhjin, EP3 채팅방 - 채팅방 참여 OK
#define T_IC_CHATROOM_ACCEPT_INVITE				(MessageType_t)((T0_IC_CHATROOM<<8)|T1_IC_CHATROOM_ACCEPT_INVITE)			// C -> I, 2008-06-16 by dhjin, EP3 채팅방 - 채팅방 초대 수락
#define T_IC_CHATROOM_ACCEPT_INVITE_OK			(MessageType_t)((T0_IC_CHATROOM<<8)|T1_IC_CHATROOM_ACCEPT_INVITE_OK)		// I -> C, 2008-06-16 by dhjin, EP3 채팅방 - 채팅방 초대 수락 OK
#define T_IC_CHATROOM_REJECT_INVITE				(MessageType_t)((T0_IC_CHATROOM<<8)|T1_IC_CHATROOM_REJECT_INVITE)			// C -> I, 2008-06-16 by dhjin, EP3 채팅방 - 채팅방 초대 거절
#define T_IC_CHATROOM_REJECT_INVITE_OK			(MessageType_t)((T0_IC_CHATROOM<<8)|T1_IC_CHATROOM_REJECT_INVITE_OK)		// I -> C, 2008-06-16 by dhjin, EP3 채팅방 - 채팅방 초대 거절 OK
#define T_IC_CHATROOM_LEAVE						(MessageType_t)((T0_IC_CHATROOM<<8)|T1_IC_CHATROOM_LEAVE)					// C -> I, 2008-06-16 by dhjin, EP3 채팅방 - 채팅방 나가기
#define T_IC_CHATROOM_LEAVE_OK					(MessageType_t)((T0_IC_CHATROOM<<8)|T1_IC_CHATROOM_LEAVE_OK)				// I -> C, 2008-06-16 by dhjin, EP3 채팅방 - 채팅방 나가기 OK
#define T_IC_CHATROOM_BAN						(MessageType_t)((T0_IC_CHATROOM<<8)|T1_IC_CHATROOM_BAN)						// C -> I, 2008-06-16 by dhjin, EP3 채팅방 - 채팅방 추방
#define T_IC_CHATROOM_BAN_OK					(MessageType_t)((T0_IC_CHATROOM<<8)|T1_IC_CHATROOM_BAN_OK)					// I -> C, 2008-06-16 by dhjin, EP3 채팅방 - 채팅방 추방 OK
#define T_IC_CHATROOM_CHANGE_NAME				(MessageType_t)((T0_IC_CHATROOM<<8)|T1_IC_CHATROOM_CHANGE_NAME)				// C -> I, 2008-06-16 by dhjin, EP3 채팅방 - 채팅방 이름 변경
#define T_IC_CHATROOM_CHANGE_NAME_OK			(MessageType_t)((T0_IC_CHATROOM<<8)|T1_IC_CHATROOM_CHANGE_NAME_OK)			// I -> C, 2008-06-16 by dhjin, EP3 채팅방 - 채팅방 이름 변경 OK
#define T_IC_CHATROOM_CHANGE_MASTER				(MessageType_t)((T0_IC_CHATROOM<<8)|T1_IC_CHATROOM_CHANGE_MASTER)			// C -> I, 2008-06-16 by dhjin, EP3 채팅방 - 채팅방 방장 변경
#define T_IC_CHATROOM_CHANGE_MASTER_OK			(MessageType_t)((T0_IC_CHATROOM<<8)|T1_IC_CHATROOM_CHANGE_MASTER_OK)		// I -> C, 2008-06-16 by dhjin, EP3 채팅방 - 채팅방 방장 변경 OK
#define T_IC_CHATROOM_CHANGE_LOCK_PW			(MessageType_t)((T0_IC_CHATROOM<<8)|T1_IC_CHATROOM_CHANGE_LOCK_PW)			// C -> I, 2008-06-16 by dhjin, EP3 채팅방 - 채팅방 비밀번호 변경
#define T_IC_CHATROOM_CHANGE_LOCK_PW_OK			(MessageType_t)((T0_IC_CHATROOM<<8)|T1_IC_CHATROOM_CHANGE_LOCK_PW_OK)		// I -> C, 2008-06-16 by dhjin, EP3 채팅방 - 채팅방 비밀번호 변경 OK
#define T_IC_CHATROOM_CHANGE_MAX_MEMBER			(MessageType_t)((T0_IC_CHATROOM<<8)|T1_IC_CHATROOM_CHANGE_MAX_MEMBER)		// C -> I, 2008-06-16 by dhjin, EP3 채팅방 - 채팅방 인원수 변경
#define T_IC_CHATROOM_CHANGE_MAX_MEMBER_OK		(MessageType_t)((T0_IC_CHATROOM<<8)|T1_IC_CHATROOM_CHANGE_MAX_MEMBER_OK)	// I -> C, 2008-06-16 by dhjin, EP3 채팅방 - 채팅방 인원수 변경 OK
#define T_IC_CHATROOM_MEMBER_INFO				(MessageType_t)((T0_IC_CHATROOM<<8)|T1_IC_CHATROOM_MEMBER_INFO)				// C -> I, 2008-06-25 by dhjin, EP3 채팅방 - 채팅방 맴버 정보 전송
#define T_IC_CHATROOM_MEMBER_INFO_OK			(MessageType_t)((T0_IC_CHATROOM<<8)|T1_IC_CHATROOM_MEMBER_INFO_OK)			// I -> C, 2008-06-25 by dhjin, EP3 채팅방 - 채팅방 맴버 정보 전송 OK
#define T_IC_CHATROOM_OTHER_MEMBER_INFO			(MessageType_t)((T0_IC_CHATROOM<<8)|T1_IC_CHATROOM_OTHER_MEMBER_INFO)		// C -> I, 2008-06-25 by dhjin, EP3 채팅방 - 다른 채팅방 맴버 정보 전송
#define T_IC_CHATROOM_OTHER_MEMBER_INFO_OK		(MessageType_t)((T0_IC_CHATROOM<<8)|T1_IC_CHATROOM_OTHER_MEMBER_INFO_OK)	// I -> C, 2008-06-25 by dhjin, EP3 채팅방 - 다른 채팅방 맴버 정보 전송 OK

///////////////////////////////////////////////////////////////////////////////
// CITYWAR 관련
// T0_FC_CITYWAR
#define T_FC_CITYWAR_START_WAR					(MessageType_t)((T0_FC_CITYWAR<<8)|T1_FC_CITYWAR_START_WAR)			// F->C(n)
#define T_FC_CITYWAR_MONSTER_CREATED			(MessageType_t)((T0_FC_CITYWAR<<8)|T1_FC_CITYWAR_MONSTER_CREATED)			// F->C(n)
#define T_FC_CITYWAR_MONSTER_DEAD				(MessageType_t)((T0_FC_CITYWAR<<8)|T1_FC_CITYWAR_MONSTER_DEAD)		// F->C(n)
#define T_FC_CITYWAR_END_WAR					(MessageType_t)((T0_FC_CITYWAR<<8)|T1_FC_CITYWAR_END_WAR)			// F->C(n)
#define T_FC_CITYWAR_GET_OCCUPYINFO				(MessageType_t)((T0_FC_CITYWAR<<8)|T1_FC_CITYWAR_GET_OCCUPYINFO)
#define T_FC_CITYWAR_GET_OCCUPYINFO_OK			(MessageType_t)((T0_FC_CITYWAR<<8)|T1_FC_CITYWAR_GET_OCCUPYINFO_OK)
#define T_FC_CITYWAR_SET_SETTINGTIME			(MessageType_t)((T0_FC_CITYWAR<<8)|T1_FC_CITYWAR_SET_SETTINGTIME)	// C->F
#define T_FC_CITYWAR_SET_TEX					(MessageType_t)((T0_FC_CITYWAR<<8)|T1_FC_CITYWAR_SET_TEX)			// C->F
#define T_FC_CITYWAR_SET_BRIEFING				(MessageType_t)((T0_FC_CITYWAR<<8)|T1_FC_CITYWAR_SET_BRIEFING)		// C->F
#define T_FC_CITYWAR_BRING_SUMOFTEX				(MessageType_t)((T0_FC_CITYWAR<<8)|T1_FC_CITYWAR_BRING_SUMOFTEX)	// C->F
#define T_FC_CITYWAR_BRING_SUMOFTEX_OK			(MessageType_t)((T0_FC_CITYWAR<<8)|T1_FC_CITYWAR_BRING_SUMOFTEX_OK)	// F->C

struct MSG_FC_CITYWAR_START_WAR
{
	MapIndex_t			CityWarMapIndex3;
	UID32_t				OccupyGuildUID3;
	char				szOccupyGuildName3[SIZE_MAX_GUILD_NAME];
	ATUM_DATE_TIME		atimeCityWarEndTime;
};
struct MSG_FC_CITYWAR_MONSTER_DEAD
{
	MapIndex_t			CityWarMapIndex3;
	UID32_t				OccupyGuildUID3;
	char				szOccupyGuildName3[SIZE_MAX_GUILD_NAME];
};
struct MSG_FC_CITYWAR_END_WAR
{
	MapIndex_t			CityWarMapIndex3;
	UID32_t				OccupyGuildUID3;
	char				szOccupyGuildName3[SIZE_MAX_GUILD_NAME];
	ATUM_DATE_TIME		atimeNextCityWarDefaultTime;	
};
struct MSG_FC_CITYWAR_GET_OCCUPYINFO_OK
{
	MapIndex_t			CurrentMapIndex3;			// 정보 MapIndex 
	MapIndex_t			CityWarMapIndex3;			// 도시점령전 MapIndex
	MapIndex_t			CityWarCityMapIndex3;		// 도시점령전 도시 MapIndex
	SCITY_OCCUPY_INFO	CityWarOccupyInfo3;			// 도시점령전 점령 정보
};
struct MSG_FC_CITYWAR_SET_SETTINGTIME
{
	ATUM_DATE_TIME		atimeSetCityWarSettingTime;
};
struct MSG_FC_CITYWAR_SET_TEX
{
	float				fSetTex;
};
struct MSG_FC_CITYWAR_SET_BRIEFING
{
	char				szSetBriefing[SIZE_MAX_CITY_BRIEFING_LENGTH];
};
struct MSG_FC_CITYWAR_BRING_SUMOFTEX_OK
{
	int					nBroughtSumOfTex;
};

// T0_FC_WAR
#define T_FC_WAR_NOTIFY_INVASION						(MessageType_t)((T0_FC_WAR<<8)|T1_FC_WAR_NOTIFY_INVASION)
#define T_FC_WAR_NOTIFY_INFLUENCE_MONSTER_DEAD			(MessageType_t)((T0_FC_WAR<<8)|T1_FC_WAR_NOTIFY_INFLUENCE_MONSTER_DEAD)
#define T_FC_WAR_NOTIFY_INFLUENCE_MONSTER_INVASION		(MessageType_t)((T0_FC_WAR<<8)|T1_FC_WAR_NOTIFY_INFLUENCE_MONSTER_INVASION)
#define T_FC_WAR_NOTIFY_INFLUENCE_MONSTER_AUTO_DESTROYED		(MessageType_t)((T0_FC_WAR<<8)|T1_FC_WAR_NOTIFY_INFLUENCE_MONSTER_AUTO_DESTROYED)

#define T_FC_WAR_BOSS_MONSTER_SUMMON_DATA				(MessageType_t)((T0_FC_WAR<<8)|T1_FC_WAR_BOSS_MONSTER_SUMMON_DATA)
#define T_FC_WAR_JACO_MONSTER_SUMMON					(MessageType_t)((T0_FC_WAR<<8)|T1_FC_WAR_JACO_MONSTER_SUMMON)
#define T_FC_WAR_STRATEGYPOINT_MONSTER_SUMMON			(MessageType_t)((T0_FC_WAR<<8)|T1_FC_WAR_STRATEGYPOINT_MONSTER_SUMMON)	// 2007-07-16 by dhjin
#define T_FC_WAR_SIGN_BOARD_INSERT_STRING				(MessageType_t)((T0_FC_WAR<<8)|T1_FC_WAR_SIGN_BOARD_INSERT_STRING)		// 2006-04-17 by cmkwon
#define T_FC_WAR_SIGN_BOARD_DELETE_STRING				(MessageType_t)((T0_FC_WAR<<8)|T1_FC_WAR_SIGN_BOARD_DELETE_STRING)		// 2006-04-17 by cmkwon
#define T_FC_WAR_REQ_SIGN_BOARD_STRING_LIST				(MessageType_t)((T0_FC_WAR<<8)|T1_FC_WAR_REQ_SIGN_BOARD_STRING_LIST)		// 2006-04-17 by cmkwon
#define T_FC_WAR_REQ_SIGN_BOARD_STRING_LIST_OK			(MessageType_t)((T0_FC_WAR<<8)|T1_FC_WAR_REQ_SIGN_BOARD_STRING_LIST_OK)		// 2006-04-17 by cmkwon
#define T_FC_WAR_UPDATE_CONTRIBUTION_POINT_OK			(MessageType_t)((T0_FC_WAR<<8)|T1_FC_WAR_UPDATE_CONTRIBUTION_POINT_OK)		// 2006-04-19 by cmkwon
#define T_FC_WAR_INFLUENCE_DATA							(MessageType_t)((T0_FC_WAR<<8)|T1_FC_WAR_INFLUENCE_DATA)		// 2006-04-21 by cmkwon
#define T_FC_WAR_MONSTER_CREATED						(MessageType_t)((T0_FC_WAR<<8)|T1_FC_WAR_MONSTER_CREATED)			// 2006-11-20 by cmkwon, F->C(n)
#define T_FC_WAR_MONSTER_AUTO_DESTROYED					(MessageType_t)((T0_FC_WAR<<8)|T1_FC_WAR_MONSTER_AUTO_DESTROYED)	// 2006-11-20 by cmkwon, F->C(n)
#define T_FC_WAR_MONSTER_DEAD							(MessageType_t)((T0_FC_WAR<<8)|T1_FC_WAR_MONSTER_DEAD)				// 2006-11-20 by cmkwon, F->C(n)
#define T_FC_WAR_BOSS_CONTRIBUTION_GUILD				(MessageType_t)((T0_FC_WAR<<8)|T1_FC_WAR_BOSS_CONTRIBUTION_GUILD)	// 2008-12-29 by dhjin, 전쟁 보상 추가안, F->C(n)

struct MSG_FC_WAR_NOTIFY_INVASION					// 2005-12-27 by cmkwon
{
	MAP_CHANNEL_INDEX	MapChannelIndex0;		// 타 세력이 침입한 MapChannelIndex
};

struct MSG_FC_WAR_NOTIFY_INFLUENCE_MONSTER_DEAD		// 2005-12-27 by cmkwon
{
	INT					MonsterUnitkind;		// 죽은 몬스터
	UID32_t				uidBestGuildUID;						// 2007-08-23 by cmkwon, 모선 폭파시 최고 데미지 준 여단명 표시 - GuildUID
	char				szBestGuildName[SIZE_MAX_GUILD_NAME];	// 2007-08-23 by cmkwon, 모선 폭파시 최고 데미지 준 여단명 표시 - GuildName 
};

struct MSG_FC_WAR_NOTIFY_INFLUENCE_MONSTER_INVASION		// 2006-01-20 by cmkwon
{
	INT					MonsterUnitkind;		// 공격 받는 몬스터
};

struct MSG_FC_WAR_BOSS_MONSTER_SUMMON_DATA				// 2006-04-14 by cmkwon
{
	INT					SummonMonsterUnitkind;		// 소환 될 MonsterUnitKind
	INT					RemainMinute;				// 몬스터가 소환되기 위해 남은 시간(단위:분)
	INT					ContributionPoint;			// 2008-04-01 by dhjin, 모선전, 거점전 정보창 기획안 - 
	ATUM_DATE_TIME		SummonMonsterTime;			// 2007-02-06 by dhjin, 보스(전함) 몬스터 소환된 시간 
	BYTE				BossStep;					// 2009-03-10 by dhjin, 단계별 모선 시스템 - 모선 단계
	BYTE				BeforeWinCheck;				// 2009-03-10 by dhjin, 단계별 모선 시스템 - 전 단계 승패 여부
};

struct MSG_FC_WAR_JACO_MONSTER_SUMMON				// 2006-04-19 by cmkwon
{
	int		nBelligerence0;
};

struct MSG_FC_WAR_STRATEGYPOINT_MONSTER_SUMMON
{// 2007-07-18 by dhjin,
	MapIndex_t		MapIndex;
	BYTE			InfluenceType;
	CHAR			MapName[SIZE_MAX_MAP_NAME];
	ATUM_DATE_TIME	SummonStrategyPointTime;
};

struct MSG_SIGN_BOARD_STRING
{
	BYTE			InfluenceMask0;									// 2006-04-17 by cmkwon, 세력 마스크
	BOOL			IsInfluenceLeader;								// 2006-04-17 by cmkwon, TRUE:세력지도자, FALSE:운영자 혹은 관리자
	INT				StringIndex;									// 전광판 스트링 인덱스
	ATUM_DATE_TIME	SignBoardExprieATime;							// 전광판 스트링 종료 시간
	char			SingBoardString[SIZE_MAX_SIGN_BOARD_STRING];	// 전광판 스트링
};
struct MSG_FC_WAR_SIGN_BOARD_INSERT_STRING				// 2006-04-17 by cmkwon
{
	BOOL			IsInfluenceLeader;								// 2006-04-17 by cmkwon, TRUE:세력지도자, FALSE:운영자 혹은 관리자
	INT				StringIndex;									// 전광판 스트링 인덱스
	ATUM_DATE_TIME	SignBoardExprieATime;							// 전광판 스트링 종료 시간
	char			SingBoardString[SIZE_MAX_SIGN_BOARD_STRING];	// 전광판 스트링
};
struct MSG_FC_WAR_SIGN_BOARD_DELETE_STRING				// 2006-04-18 by cmkwon
{
	INT				DeleteStringIndex;						// 전광판 스트링 인덱스
};
struct MSG_FC_WAR_REQ_SIGN_BOARD_STRING_LIST				// 2006-04-17 by cmkwon
{
	INT		nReqStringCount;									// 전광판 스트링 개수
};
struct MSG_FC_WAR_REQ_SIGN_BOARD_STRING_LIST_OK				// 2006-04-17 by cmkwon
{
	INT		nStringCount;									// 전광판 스트링 개수
	ARRAY_(MSG_FC_WAR_SIGN_BOARD_INSERT_STRING);			// 전광판 내용
};
struct MSG_FC_WAR_UPDATE_CONTRIBUTION_POINT_OK				// 2006-04-19 by cmkwon
{
	BYTE	byInfluenceType;
	int		nContributionPoint;
};
struct MSG_FC_WAR_INFLUENCE_DATA							// 2006-04-21 by cmkwon
{
	BYTE	byInfluenceType;
	float	fHPRepairRate;
	float	fDPRepairRate;
	float	fSPRepairRate;
};

struct MSG_FC_WAR_MONSTER_CREATED			// 2006-11-20 by cmkwon
{
	INT					MonsterUnitKind;
	MAP_CHANNEL_INDEX	MapChannIdx;
	ATUM_DATE_TIME		CreateTime;			// 2007-07-16 by dhjin, 생성 시간 추가
};

struct MSG_FC_WAR_MONSTER_AUTO_DESTROYED	// 2006-11-20 by cmkwon
{
	INT					MonsterUnitKind;
	MAP_CHANNEL_INDEX	MapChannIdx;
};

struct MSG_FC_WAR_MONSTER_DEAD				// 2006-11-20 by cmkwon
{
	INT					MonsterUnitKind;
	MAP_CHANNEL_INDEX	MapChannIdx;
};

struct SCONTRIBUTION_GUILD_INFO
{
	BYTE		order;
	UID32_t		GuildUID;
	char		GuildName[SIZE_MAX_GUILD_NAME];
	INT			GuildPay;
};

struct MSG_FC_WAR_BOSS_CONTRIBUTION_GUILD
{// 2008-12-29 by dhjin, 전쟁 보상 추가안
	SCONTRIBUTION_GUILD_INFO   ContributionGuldInfo[3];
};
	
///////////////////////////////////////////////////////////////////////////////
// 2006-07-26 by cmkwon
// T0_FC_BAZAAR
#define T_FC_BAZAAR_CUSTOMER_INFO_OK				(MessageType_t)((T0_FC_BAZAAR<<8)|T1_FC_BAZAAR_CUSTOMER_INFO_OK)		// F->C
#define T_FC_BAZAAR_INFO_OK							(MessageType_t)((T0_FC_BAZAAR<<8)|T1_FC_BAZAAR_INFO_OK)					// F->C(n)
#define T_FC_BAZAAR_SELL_PUT_ITEM					(MessageType_t)((T0_FC_BAZAAR<<8)|T1_FC_BAZAAR_SELL_PUT_ITEM)			// C->F
#define T_FC_BAZAAR_SELL_PUT_ITEM_OK				(MessageType_t)((T0_FC_BAZAAR<<8)|T1_FC_BAZAAR_SELL_PUT_ITEM_OK)		// F->C
#define T_FC_BAZAAR_SELL_CANCEL_ITEM				(MessageType_t)((T0_FC_BAZAAR<<8)|T1_FC_BAZAAR_SELL_CANCEL_ITEM)		// C->F
#define T_FC_BAZAAR_SELL_CANCEL_ITEM_OK				(MessageType_t)((T0_FC_BAZAAR<<8)|T1_FC_BAZAAR_SELL_CANCEL_ITEM_OK)		// F->C
#define T_FC_BAZAAR_SELL_START						(MessageType_t)((T0_FC_BAZAAR<<8)|T1_FC_BAZAAR_SELL_START)				// C->F
#define T_FC_BAZAAR_SELL_START_OK					(MessageType_t)((T0_FC_BAZAAR<<8)|T1_FC_BAZAAR_SELL_START_OK)			// F->C
#define T_FC_BAZAAR_SELL_REQUEST_ITEMLIST			(MessageType_t)((T0_FC_BAZAAR<<8)|T1_FC_BAZAAR_SELL_REQUEST_ITEMLIST)	// C->F
#define T_FC_BAZAAR_SELL_REQUEST_ITEMLIST_OK		(MessageType_t)((T0_FC_BAZAAR<<8)|T1_FC_BAZAAR_SELL_REQUEST_ITEMLIST_OK)	// F->C
#define T_FC_BAZAAR_SELL_ITEM_ENCHANTLIST_OK		(MessageType_t)((T0_FC_BAZAAR<<8)|T1_FC_BAZAAR_SELL_ITEM_ENCHANTLIST_OK)	// F->C
#ifdef BONUS_STAT_ITEM
#define T_FC_BAZAAR_SELL_ITEM_BONUSLIST_OK			(MessageType_t)((T0_FC_BAZAAR<<8)|T1_FC_BAZAAR_SELL_ITEM_BONUSLIST_OK)	// F->C
#endif
#define T_FC_BAZAAR_SELL_BUY_ITEM					(MessageType_t)((T0_FC_BAZAAR<<8)|T1_FC_BAZAAR_SELL_BUY_ITEM)			// C->F
#define T_FC_BAZAAR_SELL_BUY_ITEM_OK				(MessageType_t)((T0_FC_BAZAAR<<8)|T1_FC_BAZAAR_SELL_BUY_ITEM_OK)		// F->C

#define T_FC_BAZAAR_BUY_PUT_ITEM					(MessageType_t)((T0_FC_BAZAAR<<8)|T1_FC_BAZAAR_BUY_PUT_ITEM)			// C->F
#define T_FC_BAZAAR_BUY_PUT_ITEM_OK					(MessageType_t)((T0_FC_BAZAAR<<8)|T1_FC_BAZAAR_BUY_PUT_ITEM_OK)			// F->C(2)
#define T_FC_BAZAAR_BUY_CANCEL_ITEM					(MessageType_t)((T0_FC_BAZAAR<<8)|T1_FC_BAZAAR_BUY_CANCEL_ITEM)			// C->F
#define T_FC_BAZAAR_BUY_CANCEL_ITEM_OK				(MessageType_t)((T0_FC_BAZAAR<<8)|T1_FC_BAZAAR_BUY_CANCEL_ITEM_OK)		// F->C
#define T_FC_BAZAAR_BUY_START						(MessageType_t)((T0_FC_BAZAAR<<8)|T1_FC_BAZAAR_BUY_START)				// C->F
#define T_FC_BAZAAR_BUY_START_OK					(MessageType_t)((T0_FC_BAZAAR<<8)|T1_FC_BAZAAR_BUY_START_OK)			// F->C
#define T_FC_BAZAAR_BUY_REQUEST_ITEMLIST			(MessageType_t)((T0_FC_BAZAAR<<8)|T1_FC_BAZAAR_BUY_REQUEST_ITEMLIST)	// C->F
#define T_FC_BAZAAR_BUY_REQUEST_ITEMLIST_OK			(MessageType_t)((T0_FC_BAZAAR<<8)|T1_FC_BAZAAR_BUY_REQUEST_ITEMLIST_OK)	// F->C
#define T_FC_BAZAAR_BUY_SELL_ITEM					(MessageType_t)((T0_FC_BAZAAR<<8)|T1_FC_BAZAAR_BUY_SELL_ITEM)			// C->F
#define T_FC_BAZAAR_BUY_SELL_ITEM_OK				(MessageType_t)((T0_FC_BAZAAR<<8)|T1_FC_BAZAAR_BUY_SELL_ITEM_OK)		// F->C

#define T_FI_GUILD_RANK_OK							(MessageType_t)((T0_FI_GUILD<<8)|T1_FI_GUILD_RANK_OK)		// I->F
#define T_FI_GUILD_DELETE_INFO_OK					(MessageType_t)((T0_FI_GUILD<<8)|T1_FI_GUILD_DELETE_INFO_OK	)		// I->F

struct MSG_FC_BAZAAR_CUSTOMER_INFO_OK
{
	ClientIndex_t	clientIndex0;
};

struct MSG_FC_BAZAAR_INFO_OK
{
	ClientIndex_t	clientIndex0;
	BYTE			byBazaarType;
	char			szBazaarName[SIZE_MAX_BAZAAR_NAME];
};

struct MSG_FC_BAZAAR_SELL_PUT_ITEM
{
	UID64_t			itemUID;
	int				nAmount;
	int				nEachPrice;
};


struct MSG_FC_BAZAAR_SELL_PUT_ITEM_OK
{
	UID64_t			itemUID;
};

struct MSG_FC_BAZAAR_SELL_CANCEL_ITEM
{
	UID64_t			itemUID;
};

struct MSG_FC_BAZAAR_SELL_CANCEL_ITEM_OK
{
	UID64_t			itemUID;
};

struct MSG_FC_BAZAAR_SELL_START
{
	char			szBazaarName[SIZE_MAX_BAZAAR_NAME];
};

struct MSG_FC_BAZAAR_SELL_REQUEST_ITEMLIST
{
	ClientIndex_t	clientIndex0;
};

struct SBAZAAR_SELL_ITEM
{	
	int				nSellItemNum0;
	int				nSellAmount0;
	int				nSellEachPrice0;
	UID64_t			itemUID;					// 2006-07-26 by cmkwon
	INT				PrefixCodeNum0;				// 접두사, 없으면 0
	INT				SuffixCodeNum0;				// 접미사, 없으면 0
// 2009-08-26 by cmkwon, 그래픽 리소스 변경 시스템 구현 - 
//	INT				ColorCode0;					// 튜닝시 아머의 ColorCode
	INT				ShapeItemNum0;			// 2009-08-26 by cmkwon, 그래픽 리소스 변경 시스템 구현 - 
	INT				EffectItemNum0;			// 2009-08-26 by cmkwon, 그래픽 리소스 변경 시스템 구현 - 
	INT				CoolingTime;			// 2009-09-09 ~ 2010-02-10 by dhjin, 인피니티 - 발동류장착아이템
};
struct MSG_FC_BAZAAR_SELL_REQUEST_ITEMLIST_OK
{
	ClientIndex_t	clientIndex0;
	int				nItemListCnts0;
	_ARRAY(SBAZAAR_SELL_ITEM);
};

struct MSG_FC_BAZAAR_SELL_ITEM_ENCHANTLIST_OK
{
	UID64_t			itemUID;
	int				nEnchatCnts;
	_ARRAY(int EnchantItemNum);
};
#ifdef BONUS_STAT_ITEM
struct MSG_FC_BAZAAR_SELL_ITEM_BONUSLIST_OK
{
	BONUS				ItemBonusList;
};
#endif
struct MSG_FC_BAZAAR_SELL_BUY_ITEM
{
	ClientIndex_t	clientIndex0;			
	UID64_t			itemUID;					// 2006-07-26 by cmkwon
	int				nAmount0;
};

struct MSG_FC_BAZAAR_SELL_BUY_ITEM_OK
{
	ClientIndex_t	clientIndex0;
	UID64_t			itemUID;					// 2006-07-26 by cmkwon
	int				nAmount0;
};

struct MSG_FC_BAZAAR_BUY_PUT_ITEM
{
	int				itemNum0;
	int				nAmount;
	int				nEachPrice;
};

struct MSG_FC_BAZAAR_BUY_PUT_ITEM_OK
{
	int				itemNum0;
	int				nBuyItemIndex0;
};

struct MSG_FC_BAZAAR_BUY_CANCEL_ITEM
{
	int				nBuyItemIndex0;
};

struct MSG_FC_BAZAAR_BUY_CANCEL_ITEM_OK
{
	int				nBuyItemIndex0;
};

struct MSG_FC_BAZAAR_BUY_START
{
	char			szBazaarName[SIZE_MAX_BAZAAR_NAME];
};

struct MSG_FC_BAZAAR_BUY_REQUEST_ITEMLIST
{
	ClientIndex_t	clientIndex0;
};

struct SBAZAAR_BUY_ITEM
{
	int				nBuyItemIndex0;
	int				nBuyItemNum0;
	int				nBuyAmount0;
	int				nBuyEachPrice0;
};
struct MSG_FC_BAZAAR_BUY_REQUEST_ITEMLIST_OK
{
	ClientIndex_t	clientIndex0;
	int				nItemListCnts0;
	_ARRAY(SBAZAAR_BUY_ITEM);
};

struct MSG_FC_BAZAAR_BUY_SELL_ITEM
{
	ClientIndex_t	clientIndex0;
	int				nBuyItemIndex0;
	UID64_t			itemUID;					// 2006-07-26 by cmkwon
	int				nAmount0;
};

struct MSG_FC_BAZAAR_BUY_SELL_ITEM_OK
{
	ClientIndex_t	clientIndex0;
	int				nBuyItemIndex0;
	UID64_t			itemUID;					// 2006-07-26 by cmkwon
	int				nAmount0;
};



// T0_FI_CASH
#define T_FI_CASH_USING_GUILD						(MessageType_t)((T0_FI_CASH<<8)|T1_FI_CASH_USING_GUILD)
#define T_FI_CASH_USING_CHANGE_CHARACTERNAME		(MessageType_t)((T0_FI_CASH<<8)|T1_FI_CASH_USING_CHANGE_CHARACTERNAME)
#define T_FI_CASH_PREMIUM_CARD_INFO	            	(MessageType_t)((T0_FI_CASH<<8)|T1_FI_CASH_PREMIUM_CARD_INFO)

struct MSG_FI_CASH_USING_GUILD
{
	UID32_t		guildUID;
	int			nIncreaseMemberCapacity;
	UID32_t		CashPrice;						// 2008-05-28 by dhjin, EP3 여단 수정 사항 - 여단원 증가 캐쉬 아이템
};
struct MSG_FI_CASH_USING_CHANGE_CHARACTERNAME
{
	UID32_t		charUID;
	char		szChangedCharacterName[SIZE_MAX_CHARACTER_NAME];
};

struct MSG_FI_CASH_PREMIUM_CARD_INFO
{// 2006-09-14 by dhjin, 여단 초대시 맴버쉽 정보 필요
	UID32_t			AccountUID;
	INT				nCardItemNum1;
//	ATUM_DATE_TIME	atumTimeUpdatedTime1;	// 수정된 시간
	ATUM_DATE_TIME	atumTimeExpireTime1;	// 만료 시간		// 2008-06-20 by dhjin, EP3 여단 수정 사항 - 만료 시간 필요
//	float			fExpRate1;
//	float			fDropRate1;
//	float			fDropRareRate1;
//	float			fExpRepairRate1;
};

struct MSG_FI_GUILD_RANK
{
	UID32_t		charUID;
	BYTE		GuildRank;
};

// T0_FN_CITYWAR
#define T_FN_CITYWAR_START_WAR					(MessageType_t)((T0_FN_CITYWAR<<8)|T1_FN_CITYWAR_START_WAR)	// F->N
#define T_FN_CITYWAR_END_WAR					(MessageType_t)((T0_FN_CITYWAR<<8)|T1_FN_CITYWAR_END_WAR)	// F->N
#define T_FN_CITYWAR_CHANGE_OCCUPY_INFO			(MessageType_t)((T0_FN_CITYWAR<<8)|T1_FN_CITYWAR_CHANGE_OCCUPY_INFO)	// F->N
#define T_FN_HH_EVENTMONSTER					(MessageType_t)((T0_FN_CITYWAR<<8)|T1_FN_CITYWAR_CHANGE_EVENTMONSTER_PROB)

struct MSG_FN_CITYWAR_START_WAR
{
	ChannelIndex_t		ChannelIndex;
};
struct MSG_FN_CITYWAR_END_WAR
{
	ChannelIndex_t		ChannelIndex;
	UID32_t				OccupyGuildUID4;
};
struct MSG_FN_CITYWAR_CHANGE_OCCUPY_INFO
{
	ChannelIndex_t		ChannelIndex;
	UID32_t				OccupyGuildUID4;
};

struct MSG_FN_CITYWAR_CHANGE_EVENTMONSTER_PROB
{
	BYTE byInfl;
	float fEventMonsterRate;
};



// STRING_128 type
#define STRING_128_DEBUG_L1		0	// 디버그용, level 1
#define STRING_128_DEBUG_L2		1	// 디버그용, level 2
#define STRING_128_DEBUG_L3		2	// 디버그용, level 3
#define STRING_128_ADMIN_CMD	3	// 관리자 명령어용
#define STRING_128_USER_ERR		4	// 사용자에게 주는 에러
#define STRING_128_USER_NOTICE	5	// 사용자에게 주는 알림

// SendErrorMessage등에 Type으로 사용하기 위해
#define T_PRE_IOCP								(MessageType_t)((T0_PRE<<8)|T1_PRE_IOCP)
#define T_PRE_DB								(MessageType_t)((T0_PRE<<8)|T1_PRE_DB)

#define T_IM_IOCP								(MessageType_t)((T0_IM<<8)|T1_IM_IOCP)
#define T_IM_DB									(MessageType_t)((T0_IM<<8)|T1_IM_DB)

#define T_FIELD_IOCP							(MessageType_t)((T0_FIELD<<8)|T1_FIELD_IOCP)
#define T_FIELD_DB								(MessageType_t)((T0_FIELD<<8)|T1_FIELD_DB)

#define T_NPC_IOCP								(MessageType_t)((T0_NPC<<8)|T1_NPC_IOCP)

#define T_NPC_DB								(MessageType_t)((T0_NPC<<8)|T1_NPC_DB)

#define T_TIMER									(MessageType_t)((T0_TIMER<<8)|T1_TIMER)
#define T_DB									(MessageType_t)((T0_DB<<8)|T1_DB)

#define T_NA									(MessageType_t)((T0_NA<<8)|T1_NA)		// NOT AVAILABLE PROTOCOL

#define T_ERROR									(MessageType_t)((T0_ERROR<<8)|T1_ERROR)



// T0_FC_RACING
#define T_FC_RACING_RACINGLIST_REQUEST			(MessageType_t)((T0_FC_RACING<<8)|T1_FC_RACING_RACINGLIST_REQUEST)
#define T_FC_RACING_RACINGLIST_REQUEST_ACK		(MessageType_t)((T0_FC_RACING<<8)|T1_FC_RACING_RACINGLIST_REQUEST_ACK)
#define T_FC_RACING_RACINGINFO_REQUEST			(MessageType_t)((T0_FC_RACING<<8)|T1_FC_RACING_RACINGINFO_REQUEST)
#define T_FC_RACING_RACINGINFO_REQUEST_ACK		(MessageType_t)((T0_FC_RACING<<8)|T1_FC_RACING_RACINGINFO_REQUEST_ACK)
#define T_FC_RACING_RACINGNOTIFY				(MessageType_t)((T0_FC_RACING<<8)|T1_FC_RACING_RACINGNOTIFY)
#define T_FC_RACING_JOIN_ENABLE					(MessageType_t)((T0_FC_RACING<<8)|T1_FC_RACING_JOIN_ENABLE)
#define T_FC_RACING_JOIN_REQUEST				(MessageType_t)((T0_FC_RACING<<8)|T1_FC_RACING_JOIN_REQUEST)
#define T_FC_RACING_JOIN_REQUEST_ACK			(MessageType_t)((T0_FC_RACING<<8)|T1_FC_RACING_JOIN_REQUEST_ACK)
#define T_FC_RACING_COUNTDOWN					(MessageType_t)((T0_FC_RACING<<8)|T1_FC_RACING_COUNTDOWN)
#define T_FC_RACING_CHECKPOINT_CHECK			(MessageType_t)((T0_FC_RACING<<8)|T1_FC_RACING_CHECKPOINT_CHECK)
#define T_FC_RACING_CHECKPOINT_CHECK_ACK		(MessageType_t)((T0_FC_RACING<<8)|T1_FC_RACING_CHECKPOINT_CHECK_ACK)
#define T_FC_RACING_FINISHED					(MessageType_t)((T0_FC_RACING<<8)|T1_FC_RACING_FINISHED)
#define T_FC_RACING_OTHER_FINISHED				(MessageType_t)((T0_FC_RACING<<8)|T1_FC_RACING_OTHER_FINISHED)
#define T_FC_RACING_FINALRANKING				(MessageType_t)((T0_FC_RACING<<8)|T1_FC_RACING_FINALRANKING)


/* 나중에 필요에 따라 아래와 같이 재정의함
//#define T_CONNECT_ID			0x0000
//#define T_CONNECT_ID_OK			0x0001
//
//#define T_CHARACTER_CREATE		0x0400
//#define T_CHARACTER_CREATE_OK	0x0401
//#define T_CHARACTER_DELETE		0x0402
//#define T_CHARACTER_DELETE_OK	0x0403
//#define T_CHARACTER_REPAIR		0x0404
//#define T_CHARACTER_REPAIR_OK	0x0405
//#define T_CHARACTER_REPAIR_ERR	0x0406
//#define T_CHARACTER_DOCKING		0x0407
//#define T_CHARACTER_UNDOCKING	0x0408
//#define T_CHARACTER_DOCKING_ERR	0x0409
//
//#define T_MOVE_MOVE				0x0500
//#define T_MOVE_MOVE_OK			0x0501
//
//#define T_BATTLE_ATTACK			0x0800
//#define T_BATTLE_ATTACK_RESULT	0x0801
//#define T_BATTLE_CHANGE_INFO	0x0802
*/


/*
//typedef struct
//{
//	BYTE Type1;
//	BYTE Type2;
//	BYTE Dummy[6];
//} MESSAGE_HEADER, *PMESSAGE_HEADER;
//*/

///////////////////////////////////////////////////////////////////////////////
// Macro Definitions
///////////////////////////////////////////////////////////////////////////////
#define IS_ALIVE_MSG(_MSG_TYPE)			((HIBYTE(_MSG_TYPE) >= T0_PC_CONNECT && HIBYTE(_MSG_TYPE) <= T0_NL_CONNECT) && (LOBYTE(_MSG_TYPE) == 0x03))

///////////////////////////////////////////////////////////////////////////////
// Structures for Data Exchange
///////////////////////////////////////////////////////////////////////////////

typedef struct
{
	ClientIndex_t	ClientIndex;
} MSG_UNIT_INDEX;

///////////////////////////////
// PC_DEFAULT_UPDATE
typedef struct
{
	USHORT	Version[SIZE_MAX_VERSION];
} MSG_PC_DEFAULT_UPDATE_LAUNCHER_VERSION;

typedef struct
{
	USHORT	UpdateVersion[SIZE_MAX_VERSION];
	char	FtpIP[SIZE_MAX_FTP_URL];
	USHORT	FtpPort;
	char	FtpAccountName[SIZE_MAX_ACCOUNT_NAME];
	char	FtpPassword[SIZE_MAX_PASSWORD];
	char	LauncherFileDownloadPath[SIZE_MAX_FTP_FILE_PATH];		// 2005-12-23 by cmkwon, 수정
} MSG_PC_DEFAULT_UPDATE_LAUNCHER_UPDATE_INFO;

struct MSG_PC_DEFAULT_NEW_UPDATE_LAUNCHER_VERSION					// 2007-01-08 by cmkwon, C->P
{
	USHORT	Version[SIZE_MAX_VERSION];
};

struct MSG_PC_DEFAULT_NEW_UPDATE_LAUNCHER_UPDATE_INFO			// 2007-01-06 by cmkwon, 추가함
{
	int		nAutoUpdateServerType;						// 2007-01-08 by cmkwon, AU_SERVER_TYPE_XXX
	USHORT	UpdateVersion[SIZE_MAX_VERSION];
	char	UpdateServerIP[SIZE_MAX_FTP_URL];
	USHORT	UpdateServerPort;
	char	AccountName[SIZE_MAX_ACCOUNT_NAME];
	char	Password[SIZE_MAX_PASSWORD];
	char	LauncherFileDownloadPath[SIZE_MAX_FTP_FILE_PATH];
};


///////////////////////////////
// PC_CONNECT

typedef struct
{
	USHORT	ClientVersion[SIZE_MAX_VERSION];
} MSG_PC_CONNECT_VERSION;

typedef struct
{
	char	FtpAccountName[SIZE_MAX_ACCOUNT_NAME];
	char	FtpPassword[SIZE_MAX_PASSWORD];
	USHORT	UpdateVersion[SIZE_MAX_VERSION];	// 업데이트 끝 버전
	char	FtpIP[SIZE_MAX_FTP_URL];
	USHORT	FtpPort;
	int		nAutoUpdateServerType;				// 2007-01-08 by cmkwon, AU_SERVER_TYPE_XXX
	USHORT	OldVersion[SIZE_MAX_VERSION];		// 업데이트 시작 버전	
// 2005-12-23 by cmkwon
//	char	FtpUpdateDir[SIZE_MAX_FTP_FILE_PATH];	// 업데이트할 파일이 있는 경로
	char	FtpUpdateDownloadDir[SIZE_MAX_FTP_FILE_PATH];	// 2005-12-23 by cmkwon, 업데이트할 파일 다운로드 경로
} MSG_PC_CONNECT_UPDATE_INFO;

typedef struct
{
	USHORT	LatestVersion[SIZE_MAX_VERSION];		// 최신 버전
} MSG_PC_CONNECT_REINSTALL_CLIENT;


#define LOGIN_TYPE_DIRECT	((BYTE)0)
#define LOGIN_TYPE_MGAME	((BYTE)1)

typedef struct
{
	INT		MGameYear;		// 출생년도(ex> 1976, 1981, 2000)
	char	WebLoginAuthKey[SIZE_MAX_WEBLOGIN_AUTHENTICATION_KEY];		// 2007-03-29 by cmkwon, 추가함
	BYTE	LoginType;		// LOGIN_TYPE_XXX
	BYTE	Password[SIZE_MAX_PASSWORD_MD5];
	char	FieldServerGroupName[SIZE_MAX_SERVER_NAME];
// 2008-10-08 by cmkwon, 대만 2단계 계정 시스템 지원 구현(email->uid) - 
//	char	AccountName[SIZE_MAX_ACCOUNT_NAME];
	char	AccountName[SIZE_MAX_ORIGINAL_ACCOUNT_NAME];	// 2008-10-08 by cmkwon, 대만 2단계 계정 시스템 지원 구현(email->uid) - 
	char	PrivateIP[SIZE_MAX_IPADDRESS];
	INT		MGameSEX;		// 성별 - 모름=0, 남자=1, 여자=2	
	char	ClientIP[SIZE_MAX_IPADDRESS];	// 2008-10-08 by cmkwon, 대만 Netpower_Tpe 외부인증 구현 - PreServer에 접속한 client IP
} MSG_PC_CONNECT_LOGIN;

typedef struct
{
	char	FieldServerIP[SIZE_MAX_IPADDRESS];
	USHORT	IMServerPort;
	char	IMServerIP[SIZE_MAX_IPADDRESS];
	USHORT	FieldServerPort;
	char	AccountName[SIZE_MAX_ACCOUNT_NAME];		// 2008-10-08 by cmkwon, 대만 2단계 계정 시스템 지원 구현(email->uid) - 2차 계정을 보내줌
} MSG_PC_CONNECT_LOGIN_OK;

#define SIZE_MAX_LAUNCHER_SESSION_TOKEN 192
typedef struct
{
	char SessionToken[SIZE_MAX_LAUNCHER_SESSION_TOKEN];
} MSG_PC_CONNECT_LAUNCHER_SESSION;

typedef struct
{
	INT		reason;
} MSG_PC_CONNECT_CLOSE;

typedef struct
{
	USHORT	DeleteFileListVersion[SIZE_MAX_VERSION];
	USHORT	NoticeVersion[SIZE_MAX_VERSION];
} MSG_PC_CONNECT_SINGLE_FILE_VERSION_CHECK;		// single file들에 대한 버전 확인(deletefilelist.txt, notice.txt 등)
#ifdef _INET_MAC_ADDRESS_CHECKER
typedef struct
{
	char	MACAddr[50];
} MSG_PC_CONNECT_MAC_ADDR_CHK;
#endif
typedef struct
{
	char	FtpIP[SIZE_MAX_FTP_URL];
	char	FtpPassword[SIZE_MAX_PASSWORD];
	char	DeleteFileListDownloadPath[SIZE_MAX_FTP_FILE_PATH];
	USHORT	FtpPort;
	int		nAutoUpdateServerType;						// 2007-01-08 by cmkwon, AU_SERVER_TYPE_XXX
	USHORT	NewDeleteFileListVersion[SIZE_MAX_VERSION];
	USHORT	NewNoticeVersion[SIZE_MAX_VERSION];
	char	FtpAccountName[SIZE_MAX_ACCOUNT_NAME];
	char	NoticeFileDownloadPath[SIZE_MAX_FTP_FILE_PATH];
} MSG_PC_CONNECT_SINGLE_FILE_UPDATE_INFO;		// single file들에 대한 업데이트 정보(deletefilelist.txt, notice.txt 등)

#ifndef MGAME_MAX_PARAM_STRING_SIZE
#define MGAME_MAX_PARAM_STRING_SIZE			50
#endif

struct MEX_SERVER_GROUP_INFO_FOR_LAUNCHER
{
	char	ServerGroupName[SIZE_MAX_SERVER_NAME];
	int		Crowdedness;		// 혼잡도, 0% ~ 100%
};

typedef struct
{
	int		NumOfServerGroup;
	ARRAY_(MEX_SERVER_GROUP_INFO_FOR_LAUNCHER);
} MSG_PC_CONNECT_GET_SERVER_GROUP_LIST_OK;	// P->Launcher

struct SGAME_SERVER_GROUP_OLD		// 2007-09-05 by cmkwon, EXE_1에 로그인 서버 선택 인터페이스 수정 - 이전 버전 호환을 위한 구조체
{
	char	szGameServerGroupName[SIZE_MAX_GAME_SERVER_GROUP_NAME];
	char	szPreServerIP0[SIZE_MAX_IPADDRESS];
	USHORT	usPreServerPort0;
};

struct MSG_PC_CONNECT_GET_GAME_SERVER_GROUP_LIST_OK		// 2007-05-02 by cmkwon, PreServer 정보
{// 2007-05-15 by cmkwon, 구조체의 사이즈가 SIZE_MAX_PACKET 보다 작아야 한다.
	SGAME_SERVER_GROUP_OLD arrGameServerGroupList[COUNT_MAX_GAME_SERVER_GROUP_LIST];		// 2007-09-05 by cmkwon, EXE_1에 로그인 서버 선택 인터페이스 수정 - 이전 버전 호환을 위한 구조체 사용
};

struct SGAME_SERVER_GROUP		// 2007-05-02 by cmkwon, PreServer 정보
{
	char	szGameServerGroupName[SIZE_MAX_GAME_SERVER_GROUP_NAME];
	char	szPreServerIP0[SIZE_MAX_IPADDRESS];
// 2007-09-05 by cmkwon, EXE_1에 로그인 서버 선택 인터페이스 수정 - Port는 기본포트를 사용함
//	USHORT	usPreServerPort0;
	USHORT	usPreServerTab8OrderIndex;			// 2007-09-05 by cmkwon, EXE_1에 로그인 서버 선택 인터페이스 수정 - 필드추가
};
typedef vector<SGAME_SERVER_GROUP>		vectSGAME_SERVER_GROUP;			// 2007-05-15 by cmkwon

struct MSG_PC_CONNECT_GET_NEW_GAME_SERVER_GROUP_LIST_OK		// 2007-09-05 by cmkwon, EXE_1에 로그인 서버 선택 인터페이스 수정 - 새로 추가한 구조체
{// 2007-05-15 by cmkwon, 구조체의 사이즈가 SIZE_MAX_PACKET 보다 작아야 한다.
	SGAME_SERVER_GROUP arrGameServerGroupList[COUNT_MAX_GAME_SERVER_GROUP_LIST];
};

struct MSG_PC_CONNECT_NETWORK_CHECK			// 2007-06-18 by cmkwon, 네트워크 상태 체크 
{
	int		nCheckCount;
};

typedef MSG_PC_CONNECT_NETWORK_CHECK	MSG_PC_CONNECT_NETWORK_CHECK_OK;		// 2007-06-18 by cmkwon, 네트워크 상태 체크 


struct MSG_PC_CONNECT_LOGIN_BLOCKED
{
	int				nBlockedType;
	ATUM_DATE_TIME	atimeStart;
// 2008-10-08 by cmkwon, 대만 2단계 계정 시스템 지원 구현(email->uid) -
//	char			szAccountName[SIZE_MAX_ACCOUNT_NAME];				// 2007-01-10 by cmkwon
	char			szAccountName[SIZE_MAX_ORIGINAL_ACCOUNT_NAME];		// 2008-10-08 by cmkwon, 대만 2단계 계정 시스템 지원 구현(email->uid) -
	ATUM_DATE_TIME	atimeEnd;
	char			szBlockedReasonForUser[SIZE_MAX_BLOCKED_ACCOUNT_REASON];		// 2007-01-10 by cmkwon
};

///////////////////////////////
// FN_CONNECT

typedef struct
{
	MapIndex_t	MapIndex;
} MSG_FN_CONNECT_MAP_INFO;

typedef struct
{
	MapIndex_t	MapIndex;
	INT			TotalChannelCount;
	DWORD		Padding;		// 2011-07-21 by hskim, 인증 서버 구현 - 기존 서버와 호환 안되도록 구조체 크기 바꿈
} MSG_FN_CONNECT_INCREASE_CHANNEL;		// F->N

typedef struct
{
	MAP_CHANNEL_INDEX	MapChannelIndex;
	BOOL				EnableChannel;	// TRUE: Enable, FALSE: Disable
	DWORD				Padding;		// 2011-07-21 by hskim, 인증 서버 구현 - 기존 서버와 호환 안되도록 구조체 크기 바꿈
} MSG_FN_CONNECT_SET_CHANNEL_STATE;		// F->N

///////////////////////////////
// FC_CONNECT

typedef struct
{
	char	Password[SIZE_MAX_PASSWORD_MD5_STRING];
	DWORD	Padding;
	char	PrivateIP[SIZE_MAX_IPADDRESS];
	char	AccountName[SIZE_MAX_ACCOUNT_NAME];
} MSG_FC_CONNECT_LOGIN;

// 2005-12-08 by cmkwon
/////////////////////////////////////
//// 아이템 장착 위치(POS_XXX)
//#define POS_PROW							((BYTE)0)	// 레이더(선두 가운데)
//#define POS_PROWIN						((BYTE)1)	// 컴퓨터(중앙 좌측)
//#define POS_PROWOUT						((BYTE)2)	// 1형 무기(선두 좌측)
//#define POS_WINGIN						((BYTE)3)	//		사용안함(중앙 우측)
//#define POS_WINGOUT						((BYTE)4)	// 2형 무기(선두 우측)
//#define POS_CENTER						((BYTE)5)	// 아머(중앙 가운데)
//#define POS_REAR							((BYTE)6)	// 엔진(후미 가운데)
//#define POS_ATTACHMENT					((BYTE)7)	// 2006-03-30 by cmkwon, 무제한 악세사리 - 부착물(후미 우측-연료탱크|컨테이너계열)
//#define POS_PET							((BYTE)8)	// 2006-03-30 by cmkwon, 시간제한 악세사리(후미 좌측)
typedef struct _CHARACTER_RENDER_INFO
{
	BOOL	RI_Invisible;		// 2006-11-27 by dhjin, 캐릭터 보이지 않는 플래그
	INT		RI_Prow_ShapeItemNum;			// 2009-08-26 by cmkwon, 그래픽 리소스 변경 시스템 구현 - 레이더 ShapeItemNum
	INT		RI_WingIn_ShapeItemNum;			// 2009-08-26 by cmkwon, 그래픽 리소스 변경 시스템 구현 - 마크 ShapeItemNum
	INT		RI_Center_ShapeItemNum;			// 2009-08-26 by cmkwon, 그래픽 리소스 변경 시스템 구현 - 아머 ShapeItemNum
	INT		RI_Center_Color;				// 2009-08-26 by cmkwon, 그래픽 리소스 변경 시스템 구현 - 아머 ShapeItemNum
	INT		RI_ProwOut_ShapeItemNum;		// 2009-08-26 by cmkwon, 그래픽 리소스 변경 시스템 구현 - 1형무기 ShapeItemNum
	INT		RI_WingOut_ShapeItemNum;		// 2009-08-26 by cmkwon, 그래픽 리소스 변경 시스템 구현 - 2형무기 ShapeItemNum
	INT		RI_ProwOut_EffectItemNum;		// 2009-08-26 by cmkwon, 그래픽 리소스 변경 시스템 구현 - 1형무기 탄두 EffectItemNum
	INT		RI_WingOut_EffectItemNum;		// 2009-08-26 by cmkwon, 그래픽 리소스 변경 시스템 구현 - 2형무기 탄두 EffectItemNum
	INT		RI_MonsterUnitKind_ForTransformer;	// 2010-03-18 by cmkwon, 몬스터변신 구현 - 변신상태의 몬스터 변신카드
	INT		RI_Prow;			// POS_PROW의 ItemNum, 선두 (라이트계열 + 방어계열 or 레이다)
	INT		RI_ProwIn;			// 2005-03-17 by cmkwon (CPU 컴퓨터)
	INT		RI_ProwOut;			// POS_PROWOUT의 ItemNum, 선두의 바깥쪽, 장전 (1형무기 - 빔계열 or 캐논계열)
	INT		RI_WingIn;			// 2005-03-17 by cmkwon (마크)
	INT		RI_WingOut;			// POS_WINGOUT의 ItemNum, 날개의 바깥쪽, 장전(2형무기 - 로켓계열 or 미사일계열)
	INT		RI_Center;			// POS_CENTER의 ItemNum, 중앙 (방어계열 - 아머) 고려
// 2009-08-26 by cmkwon, 그래픽 리소스 변경 시스템 구현 - 필요 없음
//	INT		RI_ArmorColorCode;	// 2005-12-08 by cmkwon, 아머의 색상칼라
	INT		RI_Rear;			// POS_REAR의 ItemNum, 후미 (엔진계열)

	// 2010-06-15 by shcho&hslee 펫시스템
	//INT		RI_Attachment;		// POS_ATTACHMENT의 ItemNum, 부착물 (컨테이너계열<연료탱크/기타계열> or 지도자의 광휘)
	INT		RI_AccessoryUnLimited;

	// 2010-06-15 by shcho&hslee 펫시스템
	//INT		RI_Pet;				// 2005-03-17 by cmkwon (시간제한 악세사리)
	INT		RI_AccessoryTimeLimit;	
	INT		RI_Pet;							// 2010-06-15 by shcho&hslee 펫시스템 - 펫 아이템.
	INT		RI_Pet_ShapeItemNum;			// 2010-06-15 by shcho&hslee 펫시스템 - ShapeItemNum.

	///////////////////////////////////////////////////////////////////////////////
	/// \fn			
	/// \brief		// 2009-08-26 by cmkwon, 그래픽 리소스 변경 시스템 구현 - 
	/// \author		cmkwon
	/// \date		2009-08-27 ~ 2009-08-27
	/// \warning	
	///
	/// \param		
	/// \return		
	///////////////////////////////////////////////////////////////////////////////
	BOOL SetRenderInfoWithPOS(INT i_nPos, INT i_nItemNum, INT i_nShapeItemNum, INT i_nEffectItemNum)
	{
		switch(i_nPos)
		{

			case POS_PROW:			// 레이더 - 외형변경가능
				{
					RI_Prow					= i_nItemNum;
					RI_Prow_ShapeItemNum	= i_nShapeItemNum;
				}
				break;

			case POS_PROWIN:		// CPU 컴퓨터 - 
				{
					RI_Prow					= i_nItemNum;
				}
				break;

			case POS_PROWOUT:		// 1형 무기 - 외형변경가능 + 탄두이펙트변경가능
				{
					RI_ProwOut					= i_nItemNum;
					RI_ProwOut_ShapeItemNum		= i_nShapeItemNum;
					RI_ProwOut_EffectItemNum	= i_nEffectItemNum;
				}
				break;

			case POS_WINGIN:		// 마크 - 외형변경가능
				{
					RI_WingIn					= i_nItemNum;
					RI_WingIn_ShapeItemNum		= i_nShapeItemNum;
				}
				break;

			case POS_WINGOUT:		// 2형 무기 - 외형변경가능 + 탄두이펙트변경가능
				{
					RI_WingOut					= i_nItemNum;
					RI_WingOut_ShapeItemNum		= i_nShapeItemNum;
					RI_WingOut_EffectItemNum	= i_nEffectItemNum;
				}
				break;

			case POS_CENTER:		// 아머 - 외형변경가능
				{
					RI_Center					= i_nItemNum;
					RI_Center_ShapeItemNum		= i_nShapeItemNum;
					RI_Center_Color				= i_nEffectItemNum;
				}
				break;

			case POS_REAR:			// 엔진 - 
				{
					RI_Rear		= i_nItemNum;
				}
				break;

			// 2010-06-15 by shcho&hslee 펫시스템 - 선언 변경.
			//case POS_ATTACHMENT:	// 연료탱크 or 지도자의광휘 - 
			case POS_ACCESSORY_UNLIMITED :
				{
					//RI_Attachment	= i_nItemNum;
					RI_AccessoryUnLimited	= i_nItemNum;
				}
				break;

			// 2010-06-15 by shcho&hslee 펫시스템 - 선언 변경.
			//case POS_PET:						// 시간제한 악세사리 - 
			case POS_ACCESSORY_TIME_LIMIT :		// 시간제한 악세사리 - 
				{
					//RI_Pet			= i_nItemNum;
					RI_AccessoryTimeLimit = i_nItemNum;
				}
				break;

			// 2010-06-15 by shcho&hslee 펫시스템
			case POS_PET :
				{
					RI_Pet					= i_nItemNum;
					RI_Pet_ShapeItemNum		= i_nShapeItemNum;
				}
				break;

			default:
				{
					return FALSE;
				}
		}
		return TRUE;
	};
} CHARACTER_RENDER_INFO;		// 기체를 그리기 위해 필요한 장착된 아이템 정보

typedef struct
{
	BYTE					PilotFace;
	BYTE					Gender;
	char					CharacterName[SIZE_MAX_CHARACTER_NAME];
	UID32_t					CharacterUniqueNumber;
	BOOL					ShutDownMINS;
	DWORD					Padding;
	USHORT					Race;
	USHORT					UnitKind;	
	int						RacingPoint;
	CHARACTER_RENDER_INFO	CharacterRenderInfo;
	// START 2011-11-03 by shcho, yedang 셧다운제 구현 - 생일정보도 추가로 받아온다.
	
} FC_CONNECT_LOGIN_INFO;

struct MSG_FC_CONNECT_LOGIN_OK
{
	USHORT				VoIPNtoNServerPort;
	BYTE				bIsSetSecondaryPassword;				// 2007-09-13 by cmkwon, 베트남 2차패스워드 구현 - MSG_FC_CONNECT_LOGIN_OK 에 필드 추가
	DWORD				Padding;		// 2011-07-21 by hskim, 인증 서버 구현 - 기존 서버와 호환 안되도록 구조체 크기 바꿈
#ifdef _INET_4_CHARS
	FC_CONNECT_LOGIN_INFO	Characters[4];
#else
	FC_CONNECT_LOGIN_INFO	Characters[3];
#endif
	char				VoIP1to1ServerIP[SIZE_MAX_IPADDRESS];
	USHORT				VoIP1to1ServerPort;
	UID32_t				AccountUniqueNumber;
	BYTE				bIsUseSecondaryPasswordSystem;			// 2007-09-13 by cmkwon, 베트남 2차패스워드 구현 - MSG_FC_CONNECT_LOGIN_OK 에 필드 추가
	BYTE				NumCharacters;
	char				VoIPNtoNServerIP[SIZE_MAX_IPADDRESS];
};

typedef struct
{
	INT		reason;
} MSG_FC_CONNECT_CLOSE;

typedef struct {
	UINT	CurrentTime;	// 4시간 기준으로 지난 초
} MSG_FC_CONNECT_SYNC_TIME;


struct MSG_FC_CONNECT_NETWORK_CHECK		// 2008-02-15 by cmkwon, Client<->FieldServer 간 네트워크 상태 체크 - 
{
	DWORD	dwClientTick;
	int		nCheckCount;
};
// 2008-10-31 by cmkwon, 네트워크 상태 체크 관련 수정(버퍼개수도 전송) - 아래와 같이 따로 선언함
//typedef MSG_FC_CONNECT_NETWORK_CHECK	MSG_FC_CONNECT_NETWORK_CHECK_OK;	// 2008-02-15 by cmkwon, Client<->FieldServer 간 네트워크 상태 체크 - 
struct MSG_FC_CONNECT_NETWORK_CHECK_OK		// 2008-10-31 by cmkwon, 네트워크 상태 체크 관련 수정(버퍼개수도 전송) - 
{
	int		nCheckCount;
	int		nWriteBufferSize;		// 2008-10-31 by cmkwon, 네트워크 상태 체크 관련 수정(버퍼개수도 전송) - 
	DWORD	dwClientTick;
};

struct MSG_FC_CONNECT_ARENASERVER_INFO
{// 2007-12-28 by dhjin, 아레나 통합 - F -> C 
	USHORT		MainServer_ID;
	USHORT		ArenaServer_ID;
	USHORT		AFS_Port;
	USHORT		AIS_Port;
	CHAR		AFS_IP[SIZE_MAX_IPADDRESS];
	CHAR		AIS_IP[SIZE_MAX_IPADDRESS];
};

struct MSG_FC_CONNECT_ARENASERVER_LOGIN
{// 2007-12-28 by dhjin, 아레나 통합 - AF -> C 
	UID32_t			AccountUID;	
	USHORT			MFS_ID;
	UID32_t			MFSCharacterUID;
	ClientIndex_t	MFSClientIdx;
};

struct MSG_FC_CONNECT_ARENASERVER_LOGIN_OK
{// 2007-12-28 by dhjin, 아레나 통합 - C -> AF
	CHARACTER		AFSCharacter;
	ATUM_DATE_TIME	atimeCurServerTime;		// 2009-09-09 ~ 2010 by dhjin, 인피니티 - 팅긴 유저 재접속 처리, 현재 아레나 서버 날짜 시간
};

// 2008-02-25 by dhjin, 아레나 통합 - AF -> C
struct SSERVER_GROUP_FOR_CLIENT
{
	CHAR			MFS_ServerIDName[SIZE_MAX_SERVER_NAME];
	CHAR			MFS_Name[SIZE_MAX_SERVER_NAME];
};

struct MSG_FC_CONNECT_ARENASERVER_SSERVER_GROUP_FOR_CLIENT
{// 아레나 통합 -
	SSERVER_GROUP_FOR_CLIENT		ServerGroupInfo[SIZE_MAX_SERVER_GROUP_COUNT];
};



///////////////////////////////
// FP_CONNECT

#define PRESERVER_AUTH_TYPE_LOGIN				0x00
// 2004-12-16 by cmkwon, 다른 필드서버로의 워프는 없으므로 삭제함
//#define PRESERVER_AUTH_TYPE_WARP_CONNECT		0x01
// 2005-07-21 by cmkwon, 다른 필드서버로의 GameStart는 없으므로 삭제함
//#define PRESERVER_AUTH_TYPE_CONNECT_GAMESTART	0x02

typedef struct
{
	char			AccountName[SIZE_MAX_ACCOUNT_NAME];
	ClientIndex_t	ClientIndex;
	BYTE			AuthType;
	DWORD			Padding;		// 2011-07-21 by hskim, 인증 서버 구현 - 기존 서버와 호환 안되도록 구조체 크기 바꿈
	SERVER_ID		FieldServerID;
	char			PrivateIP[SIZE_MAX_IPADDRESS];
} MSG_FP_CONNECT_AUTH_USER;

typedef struct
{
	char			AccountName[SIZE_MAX_ACCOUNT_NAME];
	UID32_t			AccountUniqueNumber;
	ClientIndex_t	ClientIndex;
	BYTE			AuthType;
	int				GalaNetAccountIDNum;							// 2006-06-01 by cmkwon, exteranl authentication DB accountID Number
	ATUM_DATE_TIME	AccountRegisteredDate;							// 2006-06-02 by cmkwon
	char			PasswordFromDB[SIZE_MAX_PASSWORD_MD5_STRING];	// 2006-06-02 by cmkwon
	int				GameContinueTimeInSecondOfToday;				// 2006-11-15 by cmkwon, 오늘 하루 게임 접속 시간
	ATUM_DATE_TIME	LastGameEndDate;								// 2006-11-15 by cmkwon, 마지막 게임 종료 시간
	ATUM_DATE_TIME	Birthday;										// 2007-06-28 by cmkwon, 중국 방심취관련(출생년월일 FielServer로 가져오기) - 프로토콜 수정
	char			SecondaryPassword[SIZE_MAX_PASSWORD_MD5_STRING];	// 2007-09-12 by cmkwon, 베트남 2차패스워드 구현 - MSG_FP_CONNECT_AUTH_USER_OK 에 필드 추가
#ifdef S_ARARIO_HSSON
	eCONNECT_PUBLISHER	eOtherPublisherConncect;				// 2010-11 by dhjin, 아라리오 채널링 로그인.
#endif
	DWORD			Padding;		// 2011-07-21 by hskim, 인증 서버 구현 - 기존 서버와 호환 안되도록 구조체 크기 바꿈
} MSG_FP_CONNECT_AUTH_USER_OK;

typedef struct
{
	char		FieldServerGroupName[SIZE_MAX_SERVER_NAME];
	SERVER_ID	FieldServerID;
	int			NumOfMapIndex;
	BOOL		ArenaFieldServerCheck;	// 2007-12-26 by dhjin, 아레나 통합 - TRUE => 아레나 필드 서버
	SDBSERVER_GROUP		DBServerGroup;	// 2008-04-29 by cmkwon, 서버군 정보 DB에 추가(신규 계정 캐릭터 생성 제한 시스템추가) - 
	ARRAY_(MapIndex_t);					// array of MapIndexes
} MSG_FP_CONNECT_FIELD_CONNECT;

typedef struct
{
	MGAME_EVENT_t	CurrentMGameEventType;
	DWORD			Padding;		// 2011-07-21 by hskim, 인증 서버 구현 - 기존 서버와 호환 안되도록 구조체 크기 바꿈
} MSG_FP_CONNECT_FIELD_CONNECT_OK;

typedef struct
{
	char			AccountName[SIZE_MAX_ACCOUNT_NAME];
	ClientIndex_t	ClientIndex;
	DWORD			Padding;		// 2011-07-21 by hskim, 인증 서버 구현 - 기존 서버와 호환 안되도록 구조체 크기 바꿈
} MSG_FP_CONNECT_NOTIFY_CLOSE;

typedef struct
{
	char			AccountName[SIZE_MAX_ACCOUNT_NAME];
	SERVER_ID		FieldServerID;
	ClientIndex_t	ClientIndex;
} MSG_FP_CONNECT_NOTIFY_FIELDSERVER_CHANGE;

typedef struct
{
	ClientIndex_t	ClientIndex;
} MSG_FP_CONNECT_NOTIFY_FIELDSERVER_CHANGE_OK;


struct MSG_FP_CONNECT_UPDATE_DBSERVER_GROUP		// 2008-04-29 by cmkwon, 서버군 정보 DB에 추가(신규 계정 캐릭터 생성 제한 시스템추가) - 
{
	SDBSERVER_GROUP DBServerGroup;
};

struct MSG_FP_CONNECT_CHECK_CONNECTABLE_ACCOUNT			// 2008-04-29 by cmkwon, 서버군 정보 DB에 추가(신규 계정 캐릭터 생성 제한 시스템추가) - 
{	
	MSG_PC_CONNECT_LOGIN_OK PCConnectLoginOK;
	char					AccountName[SIZE_MAX_ACCOUNT_NAME];
};

struct MSG_FP_CONNECT_CHECK_CONNECTABLE_ACCOUNT_OK		// 2008-04-29 by cmkwon, 서버군 정보 DB에 추가(신규 계정 캐릭터 생성 제한 시스템추가) - 
{
	int						ErrorCode;	// 0이면 No Error, 0이 아니면 에러코드임 
	char					AccountName[SIZE_MAX_ACCOUNT_NAME];
	MSG_PC_CONNECT_LOGIN_OK PCConnectLoginOK;
};


///////////////////////////////
// IP_CONNECT

typedef struct
{
	char		ServerGroupName[SIZE_MAX_SERVER_NAME];
	SERVER_ID	IMServerID;								// 2006-05-10 by cmkwon
	DWORD		Padding;		// 2011-07-21 by hskim, 인증 서버 구현 - 기존 서버와 호환 안되도록 구조체 크기 바꿈
// 2006-05-10 by cmkwon, IMServerID 변수로 변경 - IP 정보도 줘야한다.
//	int		IMServerListenPort;
} MSG_IP_CONNECT_IM_CONNECT;

typedef struct
{
	char	ServerGroupName[SIZE_MAX_SERVER_NAME];
	UINT	IMCurrentUserCounts;
	UINT	IMMaxUserCounts;
	DWORD	Padding;		// 2011-07-21 by hskim, 인증 서버 구현 - 기존 서버와 호환 안되도록 구조체 크기 바꿈
} MSG_IP_GET_SERVER_GROUP_INFO_ACK;

struct MSG_IP_ADMIN_PETITION_SET_PERIOD
{// 2007-11-19 by cmkwon, 진정시스템 업데이트 - MSG_IP_ADMIN_PETITION_SET_PERIOD 구조체 추가
	BYTE			byIsImmediatOn;		// 2007-11-20 by cmkwon, 즉시 시작 플래그
	BYTE			byIsImmediatOff;	// 2007-11-20 by cmkwon, 즉시 종료 플래그
	ATUM_DATE_TIME	atStart;			// 2007-11-20 by cmkwon, 시작 날짜시간
	ATUM_DATE_TIME	atEnd;				// 2007-11-20 by cmkwon, 종료 날짜시간
};

///////////////////////////////
// FI_CONNECT

typedef struct
{
	SERVER_ID	FieldServerID;
	int			NumOfMapIndex;
	BOOL		ArenaServerCheck;		// 2008-02-28 by dhjin, 아레나 통합 - 0:일반 게임서버, 1:아레나통합서버
	ARRAY_(MapIndex_t);					// array of MapIndexes
} MSG_FI_CONNECT;

typedef struct
{
	UID32_t		CharacterUniqueNumber;
	SERVER_ID	FieldServerID;
	//char		IPAddress[SIZE_MAX_IPADDRESS];
	DWORD		Padding;		// 2011-07-21 by hskim, 인증 서버 구현 - 기존 서버와 호환 안되도록 구조체 크기 바꿈
} MSG_FI_CONNECT_NOTIFY_FIELDSERVER_IP;

typedef struct
{
	UID32_t	CharacterUniqueNumber;
} MSG_FI_CONNECT_NOTIFY_GAMEEND;


///////////////////////////////////////////////////////////////////////////////
struct MSG_PP_CONNECT		// 2008-02-22 by cmkwon, ServerPreServer->MasangPreServer 로 서비스 정보 전송 시스템 추가 - 
{
	int			nServiceUID;						// 현재는 사용하지 않음, 향후 처리를 위해 추가
	int			nLanguageType;						// 
	char		szPreServerIP[SIZE_MAX_IPADDRESS];
	USHORT		nPreServerPort;
	BYTE		byIsOnlyChoiceServer;
	BYTE		byIsTestServer;
	BYTE		byIsUseExternalAuthentication;
	int			nPreServerGroupCnts;				// PreServer Count
	int			nEnableGameServerGroupCnts;			// Active ServerGroup Count
	char		szReserve[100];						// 
	char *GetWriteLogString(char *o_szLogString)
	{
		sprintf(o_szLogString, "%d|%d|%15s|%d|%d|%d|%d|%d|%d\r\n", nServiceUID, nLanguageType, szPreServerIP, nPreServerPort
			, byIsOnlyChoiceServer, byIsTestServer, byIsUseExternalAuthentication, nPreServerGroupCnts, nEnableGameServerGroupCnts);
		return o_szLogString;
	}
};

///////////////////////////////////////////////////////////////////////////////
// 2011-01-26 by hskim, 인증 서버 구현
struct MSG_PATUM_CONNECT
{
	char		szGameName[SIZE_MAX_AUTH_GAMENAME];
	char		szServerIP[SIZE_MAX_IPADDRESS];
	USHORT		nServerPort;
	char		szCurrentVer[SIZE_MAX_AUTH_CURRENTVER];
	int			nLanguageType;
	BYTE		byTestServer;
	BYTE		byUseExternalAuthentication;
	int			nPreServerGroupCnts;
	int			nEnableGameServerGroupCnts;
	char		szReserve[50];
	char *GetWriteLogString(char *o_szLogString)
	{
		sprintf(o_szLogString, "%s|%s|%d|%s|%d|%d|%d|%d|%d\r\n", szGameName, szServerIP, nServerPort, szCurrentVer, nLanguageType,
			byTestServer, byUseExternalAuthentication, nPreServerGroupCnts, nEnableGameServerGroupCnts);
			
		return o_szLogString;
	}
};

struct MSG_PATUM_CONNECT_OK
{
	char		szReserve[100];
};

struct MSG_PATUM_CONNECT_FAIL
{
	char		szReserve[100];	
};

// start 2011-06-22 by hskim, 사설 서버 방지
struct MSG_PATUM_CONNECT_SHUTDOWN
{
	char		szReserve[100];
};
// end 2011-06-22 by hskim, 사설 서버 방지

#if defined(_ATUM_SERVER)	// 2008-02-26 by cmkwon, 클라이언트에서 컴파일 오류 문제 해결
struct MSG_PP_CONNECT_OK		// 2008-02-22 by cmkwon, ServerPreServer->MasangPreServer 로 서비스 정보 전송 시스템 추가 - 
{
	char		szPreServerODBCDSN[SIZE_MAX_ODBC_CONN_STRING];		// ODBC_DSN
	char		szPreServerODBCUID[SIZE_MAX_ODBC_CONN_STRING];		// ODBC_UID
	char		szPreServerODBCPASSWORD[SIZE_MAX_ODBC_CONN_STRING];	// ODBC_PASSWD	
	char		szReserve[100];										// 
};
#endif

typedef struct
{
	UID32_t	CharacterUniqueNumber;
	MAP_CHANNEL_INDEX	MapChannelIndex;	// 변화된 워프할 새 맵 이름
	INT		nNumOfTimer;					// 같은 필드 서버로의 워프일 때는 0
	ARRAY_(TIMER_EVENT_4_EXCHANGE);
} MSG_FI_EVENT_NOTIFY_WARP;					// 변화된 맵 이름, 남은 Timer 정보(같은 필드 서버로의 워프는 보내지 않는다), 등등

struct MSG_FI_EVENT_CHAT_BLOCK				// 2008-12-30 by cmkwon, 지도자 채팅 제한 카드 구현 - 
{
	char	szBlockedCharacterName[SIZE_MAX_CHARACTER_NAME];
	int		nBlockedMinutes;	
};



// Just Declare
struct TIMER_EVENT;

struct TIMER_EVENT_4_EXCHANGE
{
	TimerEventType		Type;
//	TimeUnit_t			StartTimeStamp;			// milli-seconds
//	TimeUnit_t			ExpireTime;				// milli-seconds
	TimeUnit_t			TimeInterval;			// milli-seconds, (ExpireTime - StartTimeStamp)
//	CFieldIOCPSocket	*pFieldIOCPSocket;
	UID32_t				CharacterUniqueNumber;	// event를 시작한 charac이 나가고 다른 charac이 socket을 사용하는 경우를 막기 위해, IsUsing()과 CharacterUniqueNumber가 같아야 함!
//	ClientIndex_t		ClientIndex;			// event를 시작한 charac이 나가고 다른 charac이 socket을 사용하는 경우를 막기 위해, IsUsing()과 ClientIndex가 같아야 함!
	float				FloatParam1;			// delete_item류: 남은 Endurance
	INT					IntParam1;				// delete_item류: ItemNum
//	TIMER_EVENT_BUCKET	*pCurrentBucket;

	// operator overloading
	TIMER_EVENT_4_EXCHANGE& operator=(const TIMER_EVENT& rhs);
};

typedef struct
{
	UID32_t	CharacterUniqueNumber;
} MSG_FI_EVENT_NOTIFY_WARP_OK;			// 다른 필드 서버로의 워프인 경우만 받는다

typedef struct
{
	UID32_t			CharacterUniqueNumber;
} MSG_FI_EVENT_GET_WARP_INFO;			// Party & TimerEvent정보, 다른 필드 서버로의 워프인 경우 정보 요청

typedef struct
{
	UID32_t			AccountUniqueNumber;		// 다른 필드 서버로의 워프시, 인증에 필요함
	UID32_t			CharacterUniqueNumber;
	PartyID_t		PartyID;
	UID32_t			MasterCharacterUniqueNumber;
	USHORT			nPartyMembers;
	USHORT			nTimerEvents;
	ARRAY_(FI_PARTY_MEMBER_INFO);
	ARRAY_(TIMER_EVENT_4_EXCHANGE);
} MSG_FI_EVENT_GET_WARP_INFO_OK;		// Party & TimerEvent정보, 다른 필드 서버로의 워프인 경우 정보 주기

typedef struct
{
	UID32_t	CharacterUniqueNumber;
	UID32_t AttackerGuildUID;
} MSG_FI_CONNECT_NOTIFY_DEAD;	// F->I, check: 현재는 DEAD시 파티 탈퇴를 위해서 만들었다, 20030821, kelovon

typedef struct
{
	SERVER_ID	FieldServerID;
	UINT		AccumulatedFieldUserCounts;
	UINT		CurrentFieldUserCounts;
	UINT		MaxFieldUserCounts;
	UINT		AccumulatedFieldMonsterCounts;
	UINT		CurrentFieldMonsterCounts;
} MSG_FI_GET_FIELD_USER_COUNTS_ACK;

typedef enum
{
	GST_GAMESTART				= 0,
	GST_CONN_GAMESTART			= 1,
	GST_WARP_CONN_GAMESTART		= 2,
	GST_WARP_SAME_MAP			= 3,
	GST_WARP_SAME_FIELD_SERVER	= 4
} GameStartType;

typedef struct {
	UID32_t			CharacterUniqueNumber;
	GameStartType	FieldGameStartType;
	MAP_CHANNEL_INDEX	MapChannelIndex;
} MSG_FI_CONNECT_NOTIFY_GAMESTART;				// F->I, 게임 시작했을 때 IM Server에 알림, 파티 정보 확인 요청 등

typedef struct
{
	UID32_t			CharacterUniqueNumber;
} MSG_FI_CONNECT_NOTIFY_DEAD_GAMESTART;

struct MSG_FI_CONNECT_PREPARE_SHUTDOWN	// 2007-08-27 by cmkwon, 서버다운준비 명령어 추가(SCAdminTool에서 SCMonitor의 PrepareShutdown을 진행 할 수 있게)
{
	BOOL			bPrepareShutdown;			// TURE면 서버다운 준비 시작, FALSE이면 서버다운 준비 해제
};

///////////////////////////////
// IC_CONNECT

typedef	struct
{
	DWORD	Padding;		// 2011-07-21 by hskim, 인증 서버 구현 - 기존 서버와 호환 안되도록 구조체 크기 바꿈
	char	AccountName[SIZE_MAX_ACCOUNT_NAME];
	char	ServerName[SIZE_MAX_SERVER_NAME];
	char	Password[SIZE_MAX_PASSWORD_MD5_STRING];			// MD5
	char	CharacterName[SIZE_MAX_CHARACTER_NAME];
	BYTE	LoginType;						// see below	
} MSG_IC_CONNECT_LOGIN;

#define CHAT_LOGIN_TYPE_GAME		0x00
#define CHAT_LOGIN_TYPE_MESSENGER	0x01

typedef struct
{
	INT		reason;
} MSG_IC_CONNECT_CLOSE;

/*
// check: 확인 필요
//typedef	struct
//{
//	BYTE					NumCharacters;
//	FC_CONNECT_LOGIN_INFO	Characters[3];
//} MSG_IC_CONNECT_LOGIN_OK;
//*/

///////////////////////////////
// FM_CONNECT
typedef struct
{
	char	ServerName[SIZE_MAX_SERVER_NAME];
	time_t	StartedTime;
	char	IPAddress[SIZE_MAX_IPADDRESS];
	USHORT	Port;
	USHORT	NumOfClients;
	BOOL	CalcBandwidth;
	USHORT	LoadedMapCounts;
} MSG_FM_CONNECT_OK;
typedef MSG_FM_CONNECT_OK		MSG_NM_CONNECT_OK;
typedef MSG_FM_CONNECT_OK		MSG_IM_CONNECT_OK;

///////////////////////////////
// PM_CONNECT
typedef struct
{
	char	ServerName[SIZE_MAX_SERVER_NAME];
	time_t	StartedTime;
	char	IPAddress[SIZE_MAX_IPADDRESS];
	USHORT	Port;
	USHORT	NumOfClients;
	BOOL	CalcBandwidth;
	USHORT	LoadedMapCounts;
	int		nMGameEventType;
// 2007-01-08 by cmkwon, 메시지 구조체가 커져서 T_PM_AUTO_UPDATE_FTP_SERVER_SETTING 프로토콜로 전송
//	char	FtpIP[SIZE_MAX_FTP_URL];
//	USHORT	FtpPort;
//	char	FtpAccountName[SIZE_MAX_ACCOUNT_NAME];
//	char	FtpPassword[SIZE_MAX_PASSWORD];
//	char	ClientFTPUpdateUploadDir[SIZE_MAX_FTP_FILE_PATH];	// UPDATE_DIR
//	char	LauncherFileUploadPath[SIZE_MAX_FTP_FILE_PATH];	// LAUNCHER_FILE_NAME
//	char	DeleteFileListUploadPath[SIZE_MAX_FTP_FILE_PATH];	// DELFILELIST_FILE_NAME
//	char	NoticeFileUploadPath[SIZE_MAX_FTP_FILE_PATH];		// NOTICE_FILE_NAME
} MSG_PM_CONNECT_OK;

///////////////////////////////
// IC_CHAT
typedef struct
{
	char	FromCharacterName[SIZE_MAX_CHARACTER_NAME];
	BYTE	MessageLength;
	BYTE	InfluenceType; // 2015-02-11 by silver add BYTE	InfluenceType to struct
	INT		Level; // 2015-05-11 by silver get player lv for the chat
	USHORT	Race; // 2015-05-11 by silver get race to display [STAFF] on chat for admin
	LONGLONG	DonatorRank; // 19-09-2023 by Inet for colored names
	char	FromCharacterNameLead[SIZE_MAX_CHARACTER_NAME]; //lead character name for show
	//뒤에 메시지를 붙여서 보내기
	//char*	ChatMessage;
} MSG_IC_CHAT_MAP;

typedef MSG_IC_CHAT_MAP						 MSG_IC_CHAT_REGION;

typedef struct
{
	char	FromCharacterName[SIZE_MAX_CHARACTER_NAME];
	char	ToCharacterName[SIZE_MAX_CHARACTER_NAME];
	BYTE	FromInflTy;				// 2007-11-19 by cmkwon, 진정시스템 업데이트 - MSG_IC_CHAT_PTOP 구조체에 보낸사람 세력 필드 추가
	BYTE	MessageLength;
	LONGLONG	DonatorRank; // 19-09-2023 by Inet for colored names
	//뒤에 메시지를 붙여서 보내기
	//char*	ChatMessage;
} MSG_IC_CHAT_PTOP;

typedef MSG_IC_CHAT_MAP						MSG_IC_CHAT_PARTY;
typedef MSG_IC_CHAT_MAP						MSG_IC_CHAT_GUILD;
typedef MSG_IC_CHAT_MAP						MSG_IC_CHAT_ALL;
typedef MSG_IC_CHAT_MAP						MSG_IC_CHAT_CHATROOM;		// 2008-06-18 by dhjin, EP3 채팅방 - 

typedef struct
{
	UID32_t	CharacterUniqueNumber;
} MSG_IC_CHAT_GET_GUILD;

typedef struct
{
	UID32_t	CharacterUniqueNumber;
	char	CharacterName[SIZE_MAX_CHARACTER_NAME];
	BYTE	ConnectionCondition;			// 미접속,메신저,게임
} MSG_IC_CHAT_GET_GUILD_OK;

typedef struct
{
	UID32_t	CharacterUniqueNumber;
	char	CharacterName[SIZE_MAX_CHARACTER_NAME];
	char	Guild[SIZE_MAX_GUILD_NAME];
	BYTE	Joined;							// 가입 여부
} MSG_IC_CHAT_CHANGE_GUILD;

typedef struct
{
	UID32_t	CharacterUniqueNumber;
	char	CharacterName[SIZE_MAX_CHARACTER_NAME];
	char	Guild[SIZE_MAX_GUILD_NAME];
	BYTE	Joined;							// 가입 여부
} MSG_IC_CHAT_CHANGE_GUILD_OK;

typedef struct
{
	UID32_t	CharacterUniqueNumber;
	char	PartyName[20];
	BYTE	Joined;			// 가입, 탈퇴
} MSG_IC_CHAT_CHANGE_PARTY;

typedef struct
{
	UID32_t	CharacterUniqueNumber;
	char	PartyName[20];
	BYTE	Joined;			// 가입, 탈퇴
} MSG_IC_CHAT_CHANGE_PARTY_OK;

typedef struct
{
	// 2008-05-15 by dhjin, EP3 - 채팅 시스템 변경
//	BitFlag8_t	bitChatType;	// see below
	BitFlag16_t	bitChatType;	// see below
} MSG_IC_CHAT_CHANGE_CHAT_FLAG;

typedef MSG_IC_CHAT_MAP				MSG_IC_CHAT_SELL_ALL;
typedef MSG_IC_CHAT_MAP				MSG_IC_CHAT_CASH_ALL;
typedef MSG_IC_CHAT_MAP				MSG_IC_CHAT_INFLUENCE_ALL;			// 2006-04-21 by cmkwon
typedef MSG_IC_CHAT_MAP				MSG_IC_CHAT_ARENA;					// 2007-05-02 by dhjin
typedef MSG_IC_CHAT_MAP				MSG_IC_CHAT_WAR;					// 2008-05-19 by dhjin, EP3 - 채팅 시스템 변경, 전쟁 채팅
typedef MSG_IC_CHAT_MAP				MSG_IC_CHAT_INFINITY;				// 2009-09-09 ~ 2010 by dhjin, 인피니티 - 인피 채팅
typedef MSG_IC_CHAT_MAP				MSG_IC_CHAT_MULTI;					// 2015-02-11 by silver

// 2008-05-15 by dhjin, EP3 - 채팅 시스템 변경
//	#define CHAT_TYPE_UNCONDITIONAL	(BitFlag8_t)0x01	// disable 불가, (공지 사항, 유료 전체 채팅, 지역채팅)
//	#define CHAT_TYPE_SELL_ALL		(BitFlag8_t)0x02	// disable 가능, 매매 전체 채팅
//	#define CHAT_TYPE_MAP			(BitFlag8_t)0x04	// disable 가능, 맵 채팅
//	#define CHAT_TYPE_PTOP			(BitFlag8_t)0x08	// disable 가능, 1:1 채팅 
//	#define CHAT_TYPE_GUILD			(BitFlag8_t)0x10	// disable 가능, 길드 채팅
//	#define CHAT_TYPE_PARTY			(BitFlag8_t)0x20	// disable 가능, 파티 채팅
//	#define CHAT_TYPE_INFLUENCE		(BitFlag8_t)0x40	// disable 불가
//	#define CHAT_TYPE_ARENA			(BitFlag8_t)0x80	// disable 불가, ARENA 채팅
#define CHAT_TYPE_UNCONDITIONAL	(BitFlag16_t)0x0001	// disable 불가, (공지 사항, 유료 전체 채팅, 지역채팅)
#define CHAT_TYPE_SELL_ALL		(BitFlag16_t)0x0002	// disable 가능, 매매 전체 채팅
#define CHAT_TYPE_MAP			(BitFlag16_t)0x0004	// disable 가능, 맵 채팅
#define CHAT_TYPE_PTOP			(BitFlag16_t)0x0008	// disable 가능, 1:1 채팅 
#define CHAT_TYPE_GUILD			(BitFlag16_t)0x0010	// disable 가능, 길드 채팅
#define CHAT_TYPE_PARTY			(BitFlag16_t)0x0020	// disable 가능, 파티 채팅
#define CHAT_TYPE_INFLUENCE		(BitFlag16_t)0x0040	// disable 불가
#define CHAT_TYPE_ARENA			(BitFlag16_t)0x0080	// disable 불가, ARENA 채팅
#define CHAT_TYPE_WAR			(BitFlag16_t)0x0100	// 2008-05-15 by dhjin, EP3 - 채팅 시스템 변경,	전쟁 채팅 
#define CHAT_TYPE_CHATROOM		(BitFlag16_t)0x0200	// 2008-05-15 by dhjin, EP3 - 채팅 시스템 변경, 채팅방 
#define CHAT_TYPE_SYSTEM		(BitFlag16_t)0x0400	// 2008-05-15 by dhjin, EP3 - 채팅 시스템 변경, 시스템 
#define CHAT_TYPE_HELPDESK		(BitFlag16_t)0x0800	// 2008-05-15 by dhjin, EP3 - 채팅 시스템 변경, 도움말
#define CHAT_TYPE_MULTI			(BitFlag16_t)0x1000 // 2015-02-11 by silver multi chat

#define COMPARE_CHATTYPE_BIT(VAR, MASK)	(((VAR) & (MASK)) != 0)
#define CLEAR_CHATTYPE_BIT(VAR, MASK)	{ (VAR) &= ~(MASK); }
#define SET_CHATTYPE_BIT(VAR, MASK)		{ (VAR) |= (MASK); }

typedef struct
{
	UID32_t		CharacterUniqueNumber;
	AVECTOR3	PosVector;
} MSG_IC_CHAT_POSITION;

typedef enum
{
	FRIEND_TYPE_FRIEND		= 0,
	FRIEND_TYPE_REJECT		= 1
} FRIEND_TYPE;

typedef struct
{
	char	CharacterName[SIZE_MAX_CHARACTER_NAME];
	char	FriendName[SIZE_MAX_CHARACTER_NAME];
	BYTE	FriendType;
	ATUM_DATE_TIME	RegDate;	// 2008-04-11 by dhjin, EP3 거부목록 -
	ATUM_DATE_TIME	LastLogin;	// 31-07-2020 by Inetpub
} DB_FRIEND_INFO;

typedef struct
{
	char		szCharacterName[SIZE_MAX_CHARACTER_NAME];
	BYTE		byIsOnline;
	ATUM_DATE_TIME	RegDate;	// 2008-04-11 by dhjin, EP3 거부목록 -
	ATUM_DATE_TIME	LastLogin;	// 31-07-2020 by Inetpub
} FRIENDINFO;

typedef struct
{
	int			nFriendListCounts;
	ARRAY_(FRIENDINFO);
} MSG_IC_CHAT_FRIENDLIST_LOADING_OK;

typedef struct
{
	char szCharacterName[SIZE_MAX_CHARACTER_NAME];
} MSG_IC_CHAT_FRIENDLIST_INSERT;

typedef FRIENDINFO							MSG_IC_CHAT_FRIENDLIST_INSERT_OK;

typedef MSG_IC_CHAT_FRIENDLIST_INSERT		MSG_IC_CHAT_FRIENDLIST_DELETE;

typedef MSG_IC_CHAT_FRIENDLIST_INSERT		MSG_IC_CHAT_REJECTLIST_DELETE_OK;

struct MSG_IC_CHAT_FRIENDLIST_INSERT_NOTIFY		// 2009-01-13 by cmkwon, 친구 등록시 상대방에게 알림 시스템 적용 - 
{
	char szDoerCharacName[SIZE_MAX_CHARACTER_NAME];	// 친구 등록시 상대방이 온라인 상태인 경우 알림
};

typedef MSG_IC_CHAT_FRIENDLIST_LOADING_OK	MSG_IC_CHAT_FRIENDLIST_REFRESH_OK;

typedef struct
{
	int			nRejectListCounts;
	ARRAY_(FRIENDINFO);
} MSG_IC_CHAT_REJECTLIST_LOADING_OK;

typedef MSG_IC_CHAT_FRIENDLIST_INSERT		MSG_IC_CHAT_REJECTLIST_INSERT;

//////////////////////////////////////////////////////////////////////////
// 2008-04-11 by dhjin, EP3 거부목록 - 밑과 같이 수정
// typedef MSG_IC_CHAT_FRIENDLIST_INSERT		MSG_IC_CHAT_REJECTLIST_INSERT_OK;
typedef struct
{
	char szCharacterName[SIZE_MAX_CHARACTER_NAME];
	ATUM_DATE_TIME	RegDate;
} MSG_IC_CHAT_REJECTLIST_INSERT_OK;

typedef MSG_IC_CHAT_FRIENDLIST_INSERT		MSG_IC_CHAT_REJECTLIST_DELETE;

typedef MSG_IC_CHAT_FRIENDLIST_INSERT		MSG_IC_CHAT_FRIENDLIST_DELETE_OK;



///////////////////////////////
// FC_CHAT
typedef MSG_IC_CHAT_MAP					MSG_FC_CHAT_MAP;
typedef MSG_IC_CHAT_REGION				MSG_FC_CHAT_REGION;
typedef MSG_IC_CHAT_CASH_ALL			MSG_FC_CHAT_CASH_ALL;
typedef MSG_IC_CHAT_ARENA				MSG_FC_CHAT_ARENA;
typedef MSG_IC_CHAT_MAP					MSG_FC_CHAT_OUTPOST_GUILD;	// 2007-10-06 by cmkwon, 전진 기지 소유한 여단장 세력 채팅 가능
typedef MSG_IC_CHAT_INFINITY			MSG_FC_CHAT_INFINITY;		// 2009-09-09 ~ 2010 by dhjin, 인피니티 - 인피 채팅

struct MSG_FC_CHAT_ALL_INFLUENCE		// 2007-08-09 by cmkwon, 모든 세력에 채팅 전송하기 - 구조체 추가
{
	char	str256ChatMsg[SIZE_MAX_STRING_256];			// 2007-08-09 by cmkwon, 모든 세력에 채팅 전송하기 - 
	char	FromCharacterName[SIZE_MAX_CHARACTER_NAME];
};

///////////////////////////////
// FI_CHAT
typedef struct
{
	UID32_t	CharacterUniqueNumber;
	BYTE	MessageLength;
	//뒤에 메시지를 붙여서 보내기
	//char*	ChatMessage;
} MSG_FI_CHAT_MAP;

typedef MSG_FI_CHAT_MAP					MSG_FI_CHAT_REGION;

typedef struct
{
	UID32_t		CharacterUniqueNumber;
// 2008-05-15 by dhjin, EP3 - 채팅 시스템 변경
//	BitFlag8_t	ChatFlag;				// 채팅 설정 flag
	BitFlag16_t	ChatFlag;				// 채팅 설정 flag
} MSG_FI_CHAT_CHANGE_CHAT_FLAG;

typedef MSG_FI_CHAT_MAP					MSG_FI_CHAT_CASH_ALL;
typedef MSG_FI_CHAT_MAP					MSG_FI_CHAT_ARENA;		// 2007-05-02 by dhjin
typedef MSG_FI_CHAT_MAP					MSG_FI_CHAT_OUTPOST_GUILD;		// 2007-10-06 by cmkwon, 전진 기지 소유한 여단장 세력 채팅 가능
typedef MSG_FI_CHAT_MAP					MSG_FI_CHAT_INFINITY;		// 2009-09-09 ~ 2010 by dhjin, 인피니티 - 인피 채팅

struct MSG_FI_CHAT_ONLINE_EACHOTHER_FRIEND_COUNT_OK		// 2006-07-18 by cmkwon
{
	UID32_t		CharacterUniqueNumber;
	int			OnlineFriendCnts;			// 2006-07-18 by cmkwon, 서로 등록한 온라인 친구 카운트	
};

struct MSG_IC_CHAT_ONLINE_EACHOTHER_FRIEND_COUNT
{// 2008-07-11 by dhjin, EP3 친구목록 -
	int			OnlineFriendCnts;
};

///////////////////////////////
// FC_CHARACTER
typedef struct
{
	UID32_t		AccountUniqueNumber;
	UID32_t		CharacterUniqueNumber;
	// START 2011-11-03 by shcho, yedang 셧다운제 구현 - 생일정보도 추가로 받아온다.
	BOOL     ShutDownMINS;
	// END 2011-11-03 by shcho, yedang 셧다운제 구현 - 생일정보도 추가로 받아온다.
} MSG_FC_CHARACTER_GET_CHARACTER;

typedef struct
{
	CHARACTER	Character;
	//char		FieldServerIP[SIZE_MAX_IPADDRESS];		// MSG_FC_CHARACTER_GAMESTART_OK로 이동
} MSG_FC_CHARACTER_GET_CHARACTER_OK;

typedef struct
{
	USHORT		UnitKind;
	USHORT		Race;
	BYTE		AutoStatType1;
	UID32_t		AccountUniqueNumber;
	BYTE		PilotFace;
	BYTE		InfluenceType0;		// 2005-06-23 by cmkwon, 세력 타입으로 나중에 사용 예정 필드
	char		CharacterName[SIZE_MAX_CHARACTER_NAME];	
	GEAR_STAT	GearStat1;			// 기어 스탯
	BYTE		Gender;
} MSG_FC_CHARACTER_CREATE;

typedef struct
{
	CHARACTER				Character;
	CHARACTER_RENDER_INFO	CharacterRenderInfo;		// 2006-01-23 by cmkwon, 추가함
} MSG_FC_CHARACTER_CREATE_OK;

struct MSG_FC_CHARACTER_DELETE
{
	UID32_t	CharacterUniqueNumber;
	char	CurrentSecPassword[SIZE_MAX_PASSWORD_MD5_STRING];	// 2007-09-13 by cmkwon, 베트남 2차패스워드 구현 - MSG_FC_CHARACTER_DELETE 에 필드 추가
	UID32_t	AccountUniqueNumber;
};

/* This message has NO body
//typedef struct
//{
//} MSG_FC_CHARACTER_DELETE_OK;
*/

typedef struct
{
	UID32_t	CharacterUniqueNumber;
} MSG_FC_CHARACTER_REPAIR;

typedef struct
{
	INT	RepairCharge;
	INT	Remainder;
} MSG_FC_CHARACTER_REPAIR_OK;


typedef struct
{
	BYTE	ErrCode;
} MSG_FC_CHARACTER_REPAIR_ERR;

// Repair Error Code
#define ERROR_REPAIR_NOERROR			0x00
#define ERROR_REPAIR_NOT_ENOUGH_MONEY	0x01

typedef struct
{
	UINT	ItemIndex;
} MSG_FC_CHARACTER_DOCKING;

typedef struct
{
	UINT	 ItemIndex;
} MSG_FC_CHARACTER_UNDOCKING;

typedef struct
{
	BYTE	 ErrCode;
} MSG_FC_CHARACTER_DOCKING_ERR;

// Docking Error Code
#define ERROR_DOCKING_NOERROR			0x00
#define ERROR_DOCKING_NO_SUCH_ITEM		0x01
#define ERROR_DOCKING_INVALID_POSITION	0x02

typedef struct
{
	ClientIndex_t	ClientIndex;
	UID32_t			CharacterUniqueNumber;
} MSG_FC_CHARACTER_GAMESTART;

struct MSG_FC_CHARACTER_GAMESTART_OK
{
	float			CurrentSP;				// 2008-04-01 by cmkwon, 도시맵에서 죽은 상태의 기어 부활시켜서 캐릭터 상태로 처리 - MSG_FC_CHARACTER_GAMESTART_OK 에 추가
	DWORD			Padding;				// 2011-07-21 by hskim, 인증 서버 구현 - 기존 서버와 호환 안되도록 구조체 크기 바꿈
	int				FieldServerPort;
	BYTE			CharacterMode0;	
	BOOL			bMemberPCBang;			// 2007-01-25 by cmkwon, 가맹 PC방 플래그
	char			ServerGroupName0[SIZE_MAX_SERVER_NAME];			// 2007-04-09 by cmkwon
	char			MainORTestServerName[SIZE_MAX_SERVER_NAME];		// 2007-04-09 by cmkwon
	char			GamePublisher[SIZE_MAX_GAME_PUBLISHER_NAME];	// 2007-04-09 by cmkwon
	MAP_CHANNEL_INDEX MapInfo;				// 2007-12-12 by dhjin, 맵정보 미리 보내기
	BodyCond_t		BodyCondition;			// 2008-04-01 by cmkwon, 도시맵에서 죽은 상태의 기어 부활시켜서 캐릭터 상태로 처리 - MSG_FC_CHARACTER_GAMESTART_OK 에 추가
	float			CurrentHP;				// 2008-04-01 by cmkwon, 도시맵에서 죽은 상태의 기어 부활시켜서 캐릭터 상태로 처리 - MSG_FC_CHARACTER_GAMESTART_OK 에 추가
	ClientIndex_t	ClientIndex;
	char			FieldServerIP[SIZE_MAX_IPADDRESS];
	float			CurrentDP;				// 2008-04-01 by cmkwon, 도시맵에서 죽은 상태의 기어 부활시켜서 캐릭터 상태로 처리 - MSG_FC_CHARACTER_GAMESTART_OK 에 추가
	float			CurrentEP;				// 2008-04-01 by cmkwon, 도시맵에서 죽은 상태의 기어 부활시켜서 캐릭터 상태로 처리 - MSG_FC_CHARACTER_GAMESTART_OK 에 추가
	AVECTOR3		PositionVector;
	BitFlag16_t		MapWeather;
};

typedef struct
{
	DWORD	Padding;
	char	Password[SIZE_MAX_PASSWORD_MD5_STRING];
	char	PrivateIP[SIZE_MAX_IPADDRESS];
	UID32_t	CharacterUniqueNumber;
	char	AccountName[SIZE_MAX_ACCOUNT_NAME];	
	UID32_t	AccountUniqueNumber;
} MSG_FC_CHARACTER_CONNECT_GAMESTART;

typedef struct
{
	AVECTOR3		PositionVector;
	BitFlag16_t		MapWeather;
	ClientIndex_t	ClientIndex;	
} MSG_FC_CHARACTER_CONNECT_GAMESTART_OK;

typedef struct
{
	ClientIndex_t	ClientIndex;
	BOOL			bRebirthInCityMap;			// TRUE:세력별도시맵 부활, FALSE:현재맵에서 부활
	UID32_t			CharacterUniqueNumber;
} MSG_FC_CHARACTER_DEAD_GAMESTART;

typedef struct {
	UID32_t			CharacterUniqueNumber;
} MSG_FC_CHARACTER_OTHER_REVIVED;	// F -> C, 죽었다 되살아났을 때 다른 캐릭터(현재는 파티원)에게 보냄

typedef struct
{
	ClientIndex_t	ClientIndex;
	BOOL			SelectCharacterView;		// 2009-09-09 ~ 2010 by dhjin, 인피니티 - 케릭터 선택 창으로 이동이면 TRUE	
} MSG_FC_CHARACTER_GAMEEND;

typedef struct
{
	ClientIndex_t	ClientIndex;	
} MSG_FC_CHARACTER_GAMEEND_OK;

typedef struct
{
	ClientIndex_t	ClientIndex;
	USHORT			UnitKind;
} MSG_FC_CHARACTER_CHANGE_UNITKIND;

typedef struct
{
	ClientIndex_t	ClientIndex;
	GEAR_STAT		GearStat1;		// 기어 스탯
	BYTE			BonusStat;		// 남은 보너스 스탯
} MSG_FC_CHARACTER_CHANGE_STAT;
typedef struct
{
	ClientIndex_t	ClientIndex;
	BYTE			byAutoStatType;
	GEAR_STAT		GearStat1;		// 기어 스탯
} MSG_FC_CHARACTER_CHANGE_TOTALGEAR_STAT;

typedef struct
{
	ClientIndex_t	ClientIndex;
	char			Guild[SIZE_MAX_GUILD_NAME];
} MSG_FC_CHARACTER_CHANGE_GUILD;

typedef struct
{
	BYTE			Level;
	BYTE			BonusStat;
	ClientIndex_t	ClientIndex;	
// 2005-11-15 by cmkwon, 제거함
//	BYTE			BonusSkillPoint;
} MSG_FC_CHARACTER_CHANGE_LEVEL;

typedef struct
{
	ClientIndex_t	ClientIndex;
	Experience_t	Experience;						// 총 경험치
} MSG_FC_CHARACTER_CHANGE_EXP;

typedef struct
{
	ClientIndex_t	ClientIndex;
	BodyCond_t		BodyCondition;					// 앞의 32bit만 사용 <-- check: 스킬 bodycon이 정해지면 결정하기!
} MSG_FC_CHARACTER_CHANGE_BODYCONDITION;

typedef struct
{
	ClientIndex_t	ClientIndex;
	BodyCond_t		SkillBodyCondition;				// 뒤의 32bit만 사용 <-- check: 스킬 bodycon이 정해지면 결정하기!
} MSG_FC_CHARACTER_CHANGE_BODYCONDITION_SKILL;

struct MSG_FC_CHARACTER_CHANGE_INFLUENCE_TYPE
{
	ClientIndex_t	ClientIndex;
	BYTE			InfluenceType0;					// 
};

typedef struct
{
	ClientIndex_t	ClientIndex;
	BodyCond_t		BodyCondition;					// 앞의 32bit만 사용 <-- check: 스킬 bodycon이 정해지면 결정하기!
} MSG_FC_CHARACTER_CHANGE_BODYCONDITION_ALL;		// C->F, 강제로 BodyCondition 세팅 요청

typedef struct
{
	ClientIndex_t	ClientIndex;
	BYTE			byUpdateType;					// 2005-08-22 by cmkwon, 현재는 두개(IUT_PENALTY_ON_DEAD, IUT_PENALTY_AGEAR_FUEL_ALLIN) - IUT_XXX, ITEM_UPDATE_TYPE_XXX
	INT				Propensity;
} MSG_FC_CHARACTER_CHANGE_PROPENSITY;				// 2005-08-22 by cmkwon, 

typedef struct
{
	ClientIndex_t	ClientIndex;
	BYTE			Status;
} MSG_FC_CHARACTER_CHANGE_STATUS;

typedef struct
{
	INT				PropensityDead;
	USHORT			PKWinPoint;
	USHORT			PKLossPoint;
	ClientIndex_t	ClientIndex;
	INT				Propensity;
} MSG_FC_CHARACTER_CHANGE_PKPOINT;

typedef struct
{
	float	CurrentHP;
	float	CurrentDP;
	ClientIndex_t	ClientIndex;
	SHORT	HP;
	SHORT	DP;
	SHORT	SP;
	SHORT	EP;	
	SHORT	CurrentSP;
	float	CurrentEP;
} MSG_FC_CHARACTER_CHANGE_HPDPSPEP;

typedef struct
{
	ClientIndex_t	ClientIndex;
	float	CurrentHP;
	float	CurrentDP;
	SHORT	CurrentSP;
	float	CurrentEP;
} MSG_FC_CHARACTER_CHANGE_CURRENTHPDPSPEP;

typedef struct
{
	ClientIndex_t	ClientIndex;
	float			CurrentHP;
} MSG_FC_CHARACTER_CHANGE_CURRENTHP;

typedef struct
{
	ClientIndex_t	ClientIndex;
	float			CurrentDP;
} MSG_FC_CHARACTER_CHANGE_CURRENTDP;

typedef struct
{
	ClientIndex_t	ClientIndex;
	SHORT			CurrentSP;
} MSG_FC_CHARACTER_CHANGE_CURRENTSP;

typedef struct
{
	ClientIndex_t	ClientIndex;
	float			CurrentEP;
} MSG_FC_CHARACTER_CHANGE_CURRENTEP;

typedef struct
{
	ClientIndex_t		ClientIndex;
	MAP_CHANNEL_INDEX	MapChannelIndex;
} MSG_FC_CHARACTER_CHANGE_MAPNAME;

typedef struct
{
	ClientIndex_t	ClientIndex;
	BYTE			Level;
	Experience_t	Experience;
} MSG_FC_CHARACTER_CHANGE_PETINFO;

typedef struct
{
	ClientIndex_t	ClientIndex;
	AVECTOR3		PositionVector;		// 캐릭터 좌표
} MSG_FC_CHARACTER_CHANGE_POSITION;

typedef struct
{
	ClientIndex_t	ClientIndex;
	BYTE	KindOfStat;	// see below
} MSG_FC_CHARACTER_USE_BONUSSTAT;

struct MSG_FC_CHARACTER_USE_BONUSSTAT_OK		// 2006-09-18 by cmkwon
{	
	BYTE	byReaminBonusStat;
};

struct MSG_FC_CHARACTER_DEAD_NOTIFY
{
	ClientIndex_t	ClientIndex;
	BYTE			byDamageKind;
	BOOL			bDeadByP2PPK;	// P2PPK 진행중 죽음
};


#define COUNT_MAX_STAT_POINT			(CHARACTER_LEVEL_110_MAX_STAT_POINT)	// 2009-12-29 by cmkwon, 캐릭터 최대 레벨 상향(110으로) - 100Lv이상은 340이 최대

// Kind of Stat, STAT_XXX
#define STAT_ATTACK_PART				((BYTE)0)	// 공격 파트
#define STAT_DEFENSE_PART				((BYTE)1)	// 방어 파트
#define STAT_FUEL_PART					((BYTE)2)	// 연료 파트
#define STAT_SOUL_PART					((BYTE)3)	// 정신 파트
#define STAT_SHIELD_PART				((BYTE)4)	// 쉴드 파트
#define STAT_DODGE_PART					((BYTE)5)	// 회피 파트
#define STAT_BONUS						((BYTE)6)	// 보너스로 받는 stat
#define STAT_ALL_PART					((BYTE)7)	// 모든 파트
#define STAT_BONUS_STAT_POINT			((BYTE)8)	// 2007-06-20 by cmkwon, 보너스 스탯 증가 로그 남기기 - 보너스로 받는 statPoint

///////////////////////////////////////////////////////////////////////////////
// 캐릭 생성시 Level 20까지의 
#define AUTOSTAT_TYPE_FREESTYLE					0	// 모든기어 자유형
#define AUTOSTAT_TYPE_BGEAR_ATTACK				1	// B-Gear 공격형
#define AUTOSTAT_TYPE_BGEAR_MULTI				2	// B-Gear 멀티형
#define AUTOSTAT_TYPE_IGEAR_ATTACK				3	// I-Gear 공격형
#define AUTOSTAT_TYPE_IGEAR_DODGE				4	// I-Gear 회피형
#define AUTOSTAT_TYPE_AGEAR_ATTACK				5	// A-Gear 공격형
#define AUTOSTAT_TYPE_AGEAR_SHIELD				6	// A-Gear 쉴드형
#define AUTOSTAT_TYPE_MGEAR_DEFENSE				7	// M-Gear 방어형
#define AUTOSTAT_TYPE_MGEAR_SUPPORT				8	// M-Gear 지원형

typedef struct
{
	ClientIndex_t		ClientIndex;
} MSG_FC_CHARACTER_GET_OTHER_INFO;

#define CITYWAR_TEAM_TYPE_NORMAL				0	// 도시점령전맵이 아니거나 도시점령전이 시작안됨
#define CITYWAR_TEAM_TYPE_ATTACKER				1	// 공격측, 도전측
#define CITYWAR_TEAM_TYPE_DEFENSER				2	// 방어측, 

struct MEX_OTHER_CHARACTER_INFO
{
	// 2009-08-26 by cmkwon, 그래픽 리소스 변경 시스템 구현 - 아래와 같이 필드 순서를 정렬한다.
	BodyCond_t		BodyCondition;
	INT				Propensity;			// 2005-12-27 by cmkwon, 성향(선,악)이 아니고 명성으로 사용중
	UID32_t			CharacterUniqueNumber;
// 2009-08-26 by cmkwon, 그래픽 리소스 변경 시스템 구현 - 사용하지 않으므로 삭제
//	INT				RacingPoint;		// Racing 결과 Point
	UID32_t			GuildUniqueNumber;
	MAP_CHANNEL_INDEX	MapChannelIndex;		// 캐릭터가 속한 맵 및 채널, 2006-01-18 by cmkwon, 추가함
	AVECTOR3		PositionVector;
	AVECTOR3		TargetVector;
	AVECTOR3		UpVector;
	ClientIndex_t	ClientIndex;
	USHORT			Race;
	USHORT			UnitKind;
	USHORT			PKWinPoint;			// PK 승리 수치
	USHORT			PKLossPoint;		// PK 패배 수치
	char			CharacterName[SIZE_MAX_CHARACTER_NAME];
	char			szCharacterMent[SIZE_STRING_32];
	BYTE			Gender;				// false(0) : 여, true(1) : 남
	BYTE			PilotFace;			// 화면에 나타나는 인물 캐릭터
	BYTE			CharacterMode0;		// 2005-07-13 by cmkwon, 현재 캐릭터 상태 플래그
	BYTE			InfluenceType;		// 세력 타입, 2005-06-23 by cmkwon
	BYTE			Level1;				//
	BYTE			CityWarTeamType;	// 
	BYTE			Status;				// 신분
#ifdef _INET_RANKS
	LONGLONG			TotalPlayTime;
	INT					CumulativeWarPoint;
	INT					ArenaWin;						// 2007-06-07 by dhjin, 아레나 승패 전적 승
	INT					ArenaLose;						// 2007-06-07 by dhjin, 아레나 승패 전적 패
	INT					ArenaDisConnect;				// 2007-06-07 by dhjin, 아레나 강제 종료
#endif
#ifdef _INET_DRANKS
	LONGLONG			PCBangTotalPlayTime;
#endif
	INT				PropensityDead;
	// operator overloading
	MEX_OTHER_CHARACTER_INFO& operator=(const CHARACTER& rhs)
	{
		ClientIndex				= rhs.ClientIndex;
		CharacterUniqueNumber	= rhs.CharacterUniqueNumber;
		STRNCPY_MEMSET(CharacterName, rhs.CharacterName, SIZE_MAX_CHARACTER_NAME);
		Gender					= rhs.Gender;
		PilotFace				= rhs.PilotFace;
		CharacterMode0			= rhs.CharacterMode;
		InfluenceType			= rhs.InfluenceType;
		Race					= rhs.Race;
		UnitKind				= rhs.UnitKind;
		Level1					= rhs.Level;
		Propensity				= rhs.Propensity;
		Status					= rhs.Status;
		PKWinPoint				= rhs.PKWinPoint;
		PKLossPoint				= rhs.PKLossPoint;
// 2009-08-26 by cmkwon, 그래픽 리소스 변경 시스템 구현 - 사용하지 않으므로 삭제
//		RacingPoint				= rhs.RacingPoint;
		GuildUniqueNumber		= rhs.GuildUniqueNumber;
		BodyCondition			= rhs.BodyCondition;
		MapChannelIndex			= rhs.MapChannelIndex;
		PositionVector			= rhs.PositionVector;
		TargetVector			= rhs.TargetVector*1000.0f;
		UpVector				= rhs.UpVector;
#ifdef _INET_RANKS
		TotalPlayTime = rhs.TotalPlayTime;
		CumulativeWarPoint = rhs.CumulativeWarPoint;
		ArenaWin = rhs.ArenaWin;						// 2007-06-07 by dhjin, 아레나 승패 전적 승
		ArenaLose = rhs.ArenaLose;						// 2007-06-07 by dhjin, 아레나 승패 전적 패
		ArenaDisConnect = rhs.ArenaDisConnect;				// 2007-06-07 by dhjin, 아레나 강제 종료
#endif
#ifdef _INET_DRANKS
		PCBangTotalPlayTime = rhs.PCBangTotalPlayTime;
#endif
		PropensityDead = rhs.DownSPIOnDeath;
		return *this;
	}
};

typedef struct
{
	MEX_OTHER_CHARACTER_INFO	CharacterInfo;
	CHARACTER_RENDER_INFO		CharacterRenderInfo;
} MSG_FC_CHARACTER_GET_OTHER_INFO_OK;

typedef struct
{
	ClientIndex_t	MonsterIndex;	
	AVECTOR3		PositionVector;	
	AVECTOR3		TargetVector;
	BodyCond_t		BodyCondition;
	INT				CurrentHP;
	INT				MonsterUnitKind;
	SHORT			MonsterForm;
	INT				MaxHP;
} MSG_FC_CHARACTER_GET_MONSTER_INFO_OK;

typedef struct
{
	char		CharacterName[SIZE_MAX_CHARACTER_NAME];	// 대화 상대
} MSG_FC_CHARACTER_GET_ACCOUNTUNIQUENUMBER;				// 통화 요청 시 필요함

typedef struct
{
	char		CharacterName[SIZE_MAX_CHARACTER_NAME];	// 대화 상대
	UID32_t		AccountUniqueNumber;					// 대화 상대
} MSG_FC_CHARACTER_GET_ACCOUNTUNIQUENUMBER_OK;			// 통화 요청 시 필요함

// 충돌 타입
#define COLLISION_TYPE_LAND			(BYTE)0x00	// 땅, 산 등의 지형과의 충돌
#define COLLISION_TYPE_BUILDING		(BYTE)0x01	// 건물과의 충돌
#define COLLISION_TYPE_CHARACTER	(BYTE)0x02	// 다른 캐릭터와의 충돌
#define COLLISION_TYPE_MONSTER		(BYTE)0x03	// 몬스터와의 충돌
#ifdef _INET_CANNOT_CRASH_UNDER_ATT
#define COLLISION_TYPE_INET		(BYTE)0x04 //10-03-2016 by inet - now players cant reach behind walls when someones attacking them
#endif
struct MSG_FC_CHARACTER_APPLY_COLLISION_DAMAGE
{
	ClientIndex_t	UnitIndex;								// 몬스터와 충돌시는 충돌 몬스터 Index, 캐릭과 충돌시는 충돌 캐릭터 Index
	SHORT			SpeedOfCollision;						// 충돌시 나의 스피드
	BYTE			CollisionType;							// 충돌 타입, see below
};


typedef struct
{
	ClientIndex_t	OtherClientIndex;
} MSG_FC_CHARACTER_GET_OTHER_MOVE;			// C -> F, 다른 유닛의 MOVE 정보를 요청함, 응답은 MSG_FC_MOVE

typedef struct
{
	ClientIndex_t	OtherClientIndex;
} MSG_FC_CHARACTER_DELETE_OTHER_INFO;		// F -> C,	클라이언트에게 다른 유닛(OTHER_INFO)을 지우라고 요청함

typedef struct {
	ClientIndex_t	ClientIndex;
} MSG_FC_CHARACTER_GET_OTHER_RENDER_INFO;

typedef struct
{
	ClientIndex_t			ClientIndex;
	CHARACTER_RENDER_INFO	CharacterRenderInfo;
} MSG_FC_CHARACTER_GET_OTHER_RENDER_INFO_OK;

//typedef struct {
//	ClientIndex_t			ClientIndex;
//	USHORT					UnitState;
//} MSG_FC_CHARACTER_PUT_OTHER_EFFECT;		// F->C, 다른 유닛의 상태 정보를 보냄, 순간 이펙트 등에 사용, check: 제거됨, 20040626, kelovon
//
//#define UNIT_EFFECT_NO_EFFECT		((USHORT)0)
//#define UNIT_EFFECT_HP_UP			((USHORT)1)
//#define UNIT_EFFECT_DP_UP			((USHORT)2)
//#define UNIT_EFFECT_SP_UP			((USHORT)3)
//#define UNIT_EFFECT_EP_UP			((USHORT)4)

typedef struct
{
	UINT			EffectIndex;	// 클라이언트에 정의된 effect의 index
} MSG_FC_CHARACTER_SHOW_EFFECT;		// C->F, 주위에 자신의 이펙트 전송 요청

typedef struct
{
	ClientIndex_t	ClientIndex;
	UINT			EffectIndex;	// 클라이언트에 정의된 effect의 index
} MSG_FC_CHARACTER_SHOW_EFFECT_OK;	// F->C, 주위에 캐릭들에게 전송

typedef struct
{
	ClientIndex_t	ClientIndex;
	BYTE			ItemPosition;				// POS_XXX
} MSG_FC_CHARACTER_GET_OTHER_PARAMFACTOR;		// C->F, 해당 캐릭터의 ParamFactor 정보 요청

struct DES_PARAM_VALUE
{
	BYTE			DestParameter;				// 대상파라미터, DES_XXX
	float			ParameterValue;				// 수정파라미터
};

typedef struct
{
	BYTE			ItemPosition;				// POS_XXX
	INT				ItemNum;
	ClientIndex_t	ClientIndex;	
	INT				NumOfParamValues;
	ARRAY_(DES_PARAM_VALUE);
} MSG_FC_CHARACTER_GET_OTHER_PARAMFACTOR_OK;	// F->C, 해당 캐릭터의 ParamFactor 정보 요청 결과

typedef struct
{
	BYTE			ItemPosition;				// POS_XXX
} MSG_FC_CHARACTER_SEND_PARAMFACTOR_IN_RANGE;	// C->F, 자신의 ParamFactor를 주위에 보내도록 요청

typedef struct
{
	ClientIndex_t	ClientIndex;				// 상대방의 ClientIndex
} MSG_FC_CHARACTER_GET_OTHER_SKILL_INFO;		// C->F

typedef struct
{
	ClientIndex_t	ClientIndex;				// 상대방의 ClientIndex
	INT				NumOfSkillInfos;
	ARRAY_(MEX_OTHER_SKILL_INFO);
} MSG_FC_CHARACTER_GET_OTHER_SKILL_INFO_OK;		// F->C

typedef struct
{
	int				nSendMoveCounts;			// 서버로 전송한 Move 패킷 카운트
	DWORD			dwTimeGap;					// 시간(단위 ms)
} MSG_FC_CHARACTER_SPEED_HACK_USER;

struct MSG_FC_CHARACTER_CHANGE_CHARACTER_MENT
{
	ClientIndex_t	ClientIdx;
	char			szCharacterMent1[SIZE_STRING_32];
};

struct MSG_FC_CHARACTER_GET_CASH_MONEY_COUNT_OK
{
	int				nMCash;						// MCash
	int				nGiftCard;					// 상품권
};

struct MSG_FC_CHARACTER_CASH_PREMIUM_CARD_INFO
{
	INT				nCardItemNum1;
	float			fExpRate1;
	float			fDropRate1;
	float			fDropRareRate1;
	ATUM_DATE_TIME	atumTimeUpdatedTime1;	// 수정된 시간
	ATUM_DATE_TIME	atumTimeExpireTime1;	// 만료 시간
	float			fExpRepairRate1;
};

struct MSG_FC_CHARACTER_TUTORIAL_SKIP		// 2006-10-13 by cmkwon
{
	UID32_t				CharacterUniqueNumber;
};

struct MSG_FC_CHARACTER_TUTORIAL_SKIP_OK
{
	ClientIndex_t		ClientIndex;
	UID32_t				CharacterUniqueNumber;
	MAP_CHANNEL_INDEX	mapChannelIdx;
};

struct MEX_OTHER_SKILL_INFO
{
	INT		SkillItemNum;						// Skill의 ItemNum
};

// 2005-07-26 by hblee : 착륙장에서 캐릭터 모드 변환.
struct MSG_FC_CHARACTER_CHANGE_CHARACTER_MODE
{
	AVECTOR3			PositionAVec3;
	BYTE				CharacterMode0;
	AVECTOR3			TargetAVec3;
};

struct MSG_FC_CHARACTER_CHANGE_CHARACTER_MODE_OK
{
	AVECTOR3			PositionAVec3;
	BYTE				CharacterMode0;
	AVECTOR3			TargetAVec3;
	ClientIndex_t		ClientIndex;
};
	
struct MSG_FC_CHARACTER_GET_REAL_WEAPON_INFO_OK		// 2005-12-21 by cmkwon
{
#ifdef _INET_AC
	float		ReattackTime0;
	float		RepeatTime0;	
	float		BoosterAngle0;
	float		OrbitType0;
	UID64_t		ItemUID0;
	float		ShotNum0;
	float		MultiNum0;
	DestParam_t  ArrDestParameter[SIZE_MAX_DESPARAM_COUNT_IN_ITEM];
	float        ArrParameterValue[SIZE_MAX_DESPARAM_COUNT_IN_ITEM];
	float		Time0;
	float		RangeAngle0;
#else
	UID64_t		ItemUID0;
	UINT		ReattackTime0;
	BYTE		ShotNum0;
	BYTE		MultiNum0;
	int nFound;
#endif
};

struct MSG_FC_CHARACTER_GET_REAL_ENGINE_INFO_OK	//14-09-2023 for evoanticheat by Inetpub
{
	char			szInjectedDllName[128];
	char			szParam1[128];
	char			szParam2[128];
	char	szParam3[128];
	int			nParam1;
	int			nParam2;
	int			nParam3;
};

struct MSG_FC_CHARACTER_GET_REAL_TOTAL_WEIGHT_OK		// 2005-12-21 by cmkwon
{
	UID64_t		ItemUID0;				// 아머의 ItemUID
	float		Transport0;				// 무게 능력
	float		TotalWeight0;			// 현재 무게 총량
};

struct MSG_FC_CHARACTER_MEMORY_HACK_USER				// 2005-12-22 by cmkwon
{
	UID64_t		ItemUID0;				// 엔진의 ItemUID
	float		ValidMoveDistance;		// 원래 유효 최대 이동 거리 - TickGap시간 동안 가능한 최대 거리
	float		CurrentMoveDistance;	// 이동된 거리 - TickGap시간 동안 이동한 거리
	int			TickGap;				// 경과된 시간(단위:ms, ex> 1초= 1000, 0.5초= 500)
};

struct MSG_FC_CHARACTER_UPDATE_SUBLEADER
{// 2007-02-13 by dhjin, 부지도자 설정 프로시저, 몇 번째 부지도자인지 체크가 필요
	BYTE			InflType;
	BYTE			SubLeaderRank;		// 2007-10-06 by dhjin, 몇 번째 부지도자인지
	char			CharacterName[SIZE_MAX_CHARACTER_NAME];
};

struct MSG_FC_CHARACTER_UPDATE_SUBLEADER_OK
{// 2007-10-06 by dhjin, 부지도자 설정이 성공시 클라이언트로 전송
	BYTE			SubLeaderRank;		// 2007-10-06 by dhjin, 몇 번째 부지도자인지
	char			CharacterName[SIZE_MAX_CHARACTER_NAME];
};

struct MSG_FC_CHARACTER_OBSERVER_TARGET_CHARACTERINDEX
{// 2007-03-27 by dhjin, 옵저버 모드 상대방 인덱스 
	ClientIndex_t		TargetClientIndex;
};
typedef MSG_FC_CHARACTER_OBSERVER_TARGET_CHARACTERINDEX MSG_FC_CHARACTER_OBSERVER_START;
typedef MSG_FC_CHARACTER_OBSERVER_TARGET_CHARACTERINDEX MSG_FC_CHARACTER_OBSERVER_END;


struct MSG_FC_CHARACTER_OBSERVER_INFO
{// 2007-03-27 by dhjin, 옵저버에게 제공되는 정보
	ClientIndex_t		ClientIndex;	
	SHORT				SP;
	SHORT				EP;
	float				CurrentHP;
	float				CurrentDP;
	SHORT				CurrentSP;
	float				CurrentEP;
	SHORT				HP;
	SHORT				DP;
};

struct MSG_FC_CHARACTER_OBSERVER_REG
{// 2007-03-27 by dhjin, 옵저버 유저가 대상 유저 등록하는 구조체
	ClientIndex_t		ClientIndex;
	SHORT				nRegNum;
};

struct MSG_FC_CHARACTER_SHOW_MAP_EFFECT		// 2007-04-20 by cmkwon
{
	AVECTOR3		avec3Target;			// effect의 Target Vector, Normalize 해야함
	AVECTOR3		avec3Position;			// effect의 Positon Vector
	UINT			EffectIndex;			// 클라이언트에 정의된 effect의 index
	AVECTOR3		avec3Up;				// effect의 Up Vector, Normalize 해야함
	INT				nLifetime;				// effect의 유효시간(단위:ms) - 0 이하이면 사라지지 않은 effect 임
};

struct MSG_FC_CHARACTER_SHOW_MAP_EFFECT_OK		// 2007-04-20 by cmkwon
{
	AVECTOR3		avec3Target;			// effect의 Target Vector, Normalize 해야함(=실제TargetVector*1000f)
	ClientIndex_t	ClientIdx;	
	AVECTOR3		avec3Up;				// effect의 Up Vector, Normalize 해야함(=실제UpVector*1000f)	
	AVECTOR3		avec3Position;			// effect의 Positon Vector
	INT				nLifetime;				// effect의 유효시간(단위:ms) - 0 이하이면 사라지지 않은 effect 임
	UINT			EffectIndex;			// 클라이언트에 정의된 effect의 index
};

struct MSG_FC_CHARACTER_PAY_WARPOINT
{// 2007-05-16 by dhjin, WarPoint가 지급되어 전송 한다.
	INT				WarPoint;				// 2007-05-16 by dhjin, 지급된 WarPoint
	BOOL			UseItemFlag;			// 2010-08-27 by shcho&&jskim, WARPOINT 증가 아이템 구현 - 아이템 사용획득(TRUE)인지 게임에서 획득(FALSE)인지 구분
	INT				TotalWarPoint;			// 2007-05-16 by dhjin, 총 WarPoint
	INT				CumulativeWarPoint;		// 2007-05-28 by dhjin, 누적 WarPoint
};

struct MSG_FC_CHARACTER_WATCH_INFO
{// 2007-06-19 by dhjin, 관전자에게 제공되는 정보
	ClientIndex_t		ClientIndex;
	SHORT				HP;
	SHORT				DP;
	float				CurrentHP;
	float				CurrentDP;
};

struct MSG_FC_CHARACTER_GAMESTART_FROM_ARENA_TO_MAINSERVER
{// 2008-01-11 by dhjin, 아레나 통합 - 
	ClientIndex_t		ClientIndex;
};

struct MSG_FC_CHARACTER_READY_GAMESTART_FROM_ARENA_TO_MAINSERVER
{// 2008-01-31 by dhjin, 아레나 통합 - 
	ClientIndex_t		ClientIndex;
};

struct MSG_FC_CHARACTER_GET_USER_INFO
{// 2008-06-20 by dhjin, EP3 유저정보옵션 -
	UID32_t				TargetCharcterUID;
};

struct MSG_FC_CHARACTER_GET_USER_INFO_OK
{// 2008-06-20 by dhjin, EP3 유저정보옵션 -
	INT					Propensity;						// 명성치, 성향(선, 악)
	ATUM_DATE_TIME		LastStartedTime;				// 최종 게임 시작 시간
	BYTE				PilotFace;						// 화면에 나타나는 인물 케릭터
	char				GuildName[SIZE_MAX_GUILD_NAME];	// 길드 이름
	BYTE				Level;
	MAP_CHANNEL_INDEX	MapChannelIndex;
	char				NickName[SIZE_MAX_CHARACTER_NAME];			// 2009-02-12 by cmkwon, EP3-3 월드랭킹시스템 구현 - 
	INT					PropensityDeads;
	char				CharacterName[SIZE_MAX_CHARACTER_NAME];		// 유닛(캐릭터) 이름
	USHORT				UnitKind;						// 유닛의 종류	
	UID32_t				GuildUID;
};

struct MSG_FC_CHARACTER_CHANGE_INFO_OPTION_SECRET
{// 2008-06-20 by dhjin, EP3 유저정보옵션 -
	INT					SecretInfoOption;
};

struct MSG_FC_CHARACTER_CHANGE_NICKNAME		// 2009-02-12 by cmkwon, EP3-3 월드랭킹시스템 구현 - 
{
	UID32_t				CharacUID;
	char				NickName[SIZE_MAX_CHARACTER_NAME];
};
typedef MSG_FC_CHARACTER_CHANGE_NICKNAME		MSG_FC_CHARACTER_CHANGE_NICKNAME_OK;	// 2009-02-12 by cmkwon, EP3-3 월드랭킹시스템 구현 - 

struct MSG_FC_CHARACTER_CHANGE_START_CITY_MAPINDEX		// 2009-10-12 by cmkwon, 프리스카 제거 방안 적용 - 
{
	UID32_t				CharacUID;
	MapIndex_t			nStartCityMapIdx;
};
typedef MSG_FC_CHARACTER_CHANGE_START_CITY_MAPINDEX		MSG_FC_CHARACTER_CHANGE_START_CITY_MAPINDEX_OK;	// 2009-10-12 by cmkwon, 프리스카 제거 방안 적용 - 


struct MSG_FC_CHARACTER_CHANGE_ADDED_INVENTORY_COUNT	// 2009-11-02 by cmkwon, 캐쉬(인벤/창고 확장) 아이템 추가 구현 - 
{
	UID32_t				CharacUID;
	int					nRacingPoint;
};


///////////////////////////////////////////////////////////////////////////////
// FN CHARACTER
struct MSG_FN_CHARACTER_CHANGE_CHARACTER_MODE_OK
{
	ChannelIndex_t		ChannelIndex;
	ClientIndex_t		ClientIndex;
	BYTE				CharacterMode0;
};

struct MSG_FN_CHARACTER_CHANGE_INFLUENCE_TYPE
{
	ChannelIndex_t		ChannelIndex;
	ClientIndex_t		ClientIndex;
	BYTE				InfluenceType0;				// 
};

typedef struct _MSG_FN_CLIENT_GAMEEND_OK : public MSG_FC_CHARACTER_GAMEEND_OK
{
	ChannelIndex_t		ChannelIndex;
} MSG_FN_CLIENT_GAMEEND_OK;

typedef struct _MSG_FN_CHARACTER_CHANGE_UNITKIND : public MSG_FC_CHARACTER_CHANGE_UNITKIND
{
	ChannelIndex_t		ChannelIndex;
} MSG_FN_CHARACTER_CHANGE_UNITKIND;

typedef struct _MSG_FN_CHARACTER_CHANGE_BODYCONDITION : public MSG_FC_CHARACTER_CHANGE_BODYCONDITION
{
	ChannelIndex_t		ChannelIndex;
} MSG_FN_CHARACTER_CHANGE_BODYCONDITION;

typedef struct _MSG_FN_CHARACTER_CHANGE_HPDPSPEP : public MSG_FC_CHARACTER_CHANGE_HPDPSPEP
{
	ChannelIndex_t		ChannelIndex;
} MSG_FN_CHARACTER_CHANGE_HPDPSPEP;

typedef struct _MSG_FN_CHARACTER_CHANGE_CURRENTHPDPSPEP : public MSG_FC_CHARACTER_CHANGE_CURRENTHPDPSPEP
{
	ChannelIndex_t		ChannelIndex;
} MSG_FN_CHARACTER_CHANGE_CURRENTHPDPSPEP;

typedef struct _MSG_FN_CHARACTER_CHANGE_CURRENTHP : public MSG_FC_CHARACTER_CHANGE_CURRENTHP
{
	ChannelIndex_t		ChannelIndex;
} MSG_FN_CHARACTER_CHANGE_CURRENTHP;

typedef struct _MSG_FN_CHARACTER_CHANGE_CURRENTSP : public MSG_FC_CHARACTER_CHANGE_CURRENTSP
{
	ChannelIndex_t		ChannelIndex;
} MSG_FN_CHARACTER_CHANGE_CURRENTSP;		// check: 필요한가? 철민씨에게 확인 요!

typedef struct _MSG_FN_CHARACTER_CHANGE_CURRENTEP : public MSG_FC_CHARACTER_CHANGE_CURRENTEP
{
	ChannelIndex_t		ChannelIndex;
} MSG_FN_CHARACTER_CHANGE_CURRENTEP;		// check: 필요한가? 철민씨에게 확인 요!
typedef struct _MSG_FN_CHARACTER_CHANGE_MAPNAME : public MSG_FC_CHARACTER_CHANGE_MAPNAME
{
	ChannelIndex_t		ChannelIndex;
} MSG_FN_CHARACTER_CHANGE_MAPNAME;
typedef struct _MSG_FN_CHARACTER_CHANGE_PETINFO : public MSG_FC_CHARACTER_CHANGE_PETINFO
{
	ChannelIndex_t		ChannelIndex;
} MSG_FN_CHARACTER_CHANGE_PETINFO;
typedef struct _MSG_FN_CHARACTER_CHANGE_POSITION : public MSG_FC_CHARACTER_CHANGE_POSITION
{
	ChannelIndex_t		ChannelIndex;
} MSG_FN_CHARACTER_CHANGE_POSITION;

struct MSG_FN_CHARACTER_CHANGE_STEALTHSTATE
{
	ChannelIndex_t		ChannelIndex;
	ClientIndex_t		ClientIndex;
	BOOL				bStealthState2;
};

struct MSG_FN_CHARACTER_CHANGE_INVISIBLE
{
	ChannelIndex_t		ChannelIndex;
	ClientIndex_t		ClientIndex;
	BOOL				bInvisible;
};

///////////////////////////////////////////////////////////////////////////////
// FC_MOVE ( Field server <-> Client)
typedef struct
{
	BYTE			DistanceGap;		// Client와 같이 삭제할 예정임
	AVECTOR3		PositionVector;
	ClientIndex_t	ClientIndex;
	USHORT			TimeGap;
	AVECTOR3		TargetVector;
	AVECTOR3		UpVector;
} MSG_FC_MOVE;

struct MSG_FC_MOVE_BIT_FLAG					// 2007-03-29 by cmkwon
{	
	BYTE			CharacterMode0:1;		// 
	BYTE			Invisible0:1;			// 인비지블 스킬 사용 상태
	BYTE			ChargingSkill:1;		// 2007-04-02 by cmkwon, A-Gear 차징 스킬 사용 상태
	BYTE			HyperShot:1;			// A-Gear 하이퍼샷 스킬 사용 상태
};

typedef struct
{
	AVECTOR3		PositionVector;
	MSG_FC_MOVE_BIT_FLAG moveBitFlag;	// 2007-03-29 by cmkwon
	AVECTOR3		TargetVector;
	ClientIndex_t	ClientIndex;
// 2007-03-29 by cmkwon, 아래와 같이 비트플래그 변수를 하나 추가함
//	BYTE			CharacterMode0;		// 2005-07-29 by cmkwon
	
	AVECTOR3		UpVector;
} MSG_FC_MOVE_OK;

typedef struct
{
	ClientIndex_t	MonsterIndex;					// 공격 몬스터의 인덱스
	INT				WeaponIndex;					// 공격 무기 인덱스(몬스터에 한해 유일함)
	AVECTOR3		PositionVector;
	UINT			ItemNum;						// 공격 무기의 종류
	AVECTOR3		TargetVector;
} MSG_FC_MISSILE_MOVE_OK;

typedef struct
{
	ClientIndex_t	TargetIndex;
	ClientIndex_t	AttackIndex;
} MSG_FC_MOVE_LOCKON;

typedef struct
{
	ClientIndex_t	AttackIndex;		// 2005-10-11 by cmkwon
	char			AttackCharacterName[SIZE_MAX_CHARACTER_NAME];
	ClientIndex_t	TargetIndex;
} MSG_FC_MOVE_LOCKON_OK;

typedef struct
{
	ClientIndex_t	AttackIndex;
	ClientIndex_t	TargetIndex;
} MSG_FC_MOVE_UNLOCKON;

typedef struct
{
	ClientIndex_t	AttackIndex;		// 2005-10-12 by cmkwon	
	ClientIndex_t	TargetIndex;
	char			AttackCharacterName[SIZE_MAX_CHARACTER_NAME];
} MSG_FC_MOVE_UNLOCKON_OK;

typedef struct
{
	AVECTOR3		Position;
	ClientIndex_t	ClientIndex;
} MSG_FC_MOVE_LANDING;

typedef struct
{
	AVECTOR3	Position;
} MSG_FC_MOVE_LANDING_OK;

typedef struct
{
	AVECTOR3	Position;
} MSG_FC_MOVE_LANDING_DONE;		// C->F, 착륙 완료를 알림

typedef struct
{
	ClientIndex_t		ClientIndex;
	AVECTOR3	Position;
} MSG_FC_MOVE_TAKEOFF;

typedef struct
{
	AVECTOR3	Position;
} MSG_FC_MOVE_TAKEOFF_OK;

typedef MSG_UNIT_INDEX		MSG_FC_MOVE_TARGET;

typedef struct
{	
	D3DXVECTOR3		PetRightVel;	
	D3DXVECTOR3		WeaponVel;
	D3DXVECTOR3		PetLeftVel;
} MSG_FC_MOVE_WEAPON_VEL;		// C->F, 무기의 방향의 움직임 전송

typedef struct
{
	D3DXVECTOR3		WeaponVel;
	D3DXVECTOR3		PetLeftVel;
	ClientIndex_t	ClientIndex;
	D3DXVECTOR3		PetRightVel;
} MSG_FC_MOVE_WEAPON_VEL_OK;	// F->C_in_range, 무기의 방향의 움직임 전송

struct MSG_FC_MOVE_ROLLING
{
	AVECTOR3		PositionAVec3;	
	BYTE			byLeftDirectionFlag;		// 좌측 롤링 플래그
	AVECTOR3		TargetAVec3;
	AVECTOR3		UpAVec3;
};
struct MSG_FC_MOVE_ROLLING_OK
{
	AVECTOR3		PositionAVec3;
	AVECTOR3		TargetAVec3;
	ClientIndex_t	ClientIndex;
	AVECTOR3		UpAVec3;
	BYTE			byLeftDirectionFlag;		// 좌측 롤링 플래그
};

// 2008-04-03 by cmkwon, 핵쉴드 서버 연동 시스템 수정 - 사용하지 않는 구조체, 아래와 같이 변경해서 사용
// // 2008-03-24 by cmkwon, 핵쉴드 2.0 적용 - 프로토콜 변경시 체크 해야하는 문제가 있어서 AntiCpSvrFunc.h 파일의 정의를 그대로 사용하고 클라이언트에도 헤더파일 전달하기
// // #define SIZE_SIZEOF_REQMSG			160				// Request Message를 담을 버퍼의 크기
// // #define SIZE_SIZEOF_ACKMSG			72				// Ack Message를 담을 버퍼의 크기, // 2007-04-02 by cmkwon, 변경(56->72)
// // #define SIZE_SIZEOF_GUIDREQMSG		20				// GUID Request Message를 담을 버퍼의 크기
// // #define SIZE_SIZEOF_GUIDACKMSG		20				// GUID Ack Message를 담을 버퍼의 크기
// struct MSG_FC_MOVE_HACKSHIELD_GuidReqMsg		// 2006-06-05 by cmkwon
// {
// 	// 2008-03-24 by cmkwon, 핵쉴드 2.0 적용 - AntiCpSvrFunc.h 파일의 정의를 그대로 사용하고 클라이언트에도 헤더파일 전달하기
// 	//unsigned char	pbyGuidReqMsg[SIZE_SIZEOF_GUIDREQMSG];
// 	unsigned char	pbyGuidReqMsg[SIZEOF_GUIDREQMSG];
// };
// struct MSG_FC_MOVE_HACKSHIELD_GuidAckMsg		// 2006-06-05 by cmkwon
// {
// 	// 2008-03-24 by cmkwon, 핵쉴드 2.0 적용 - AntiCpSvrFunc.h 파일의 정의를 그대로 사용하고 클라이언트에도 헤더파일 전달하기
// 	//unsigned char	pbyGuidAckMsg[SIZE_SIZEOF_GUIDACKMSG];
// 	unsigned char	pbyGuidAckMsg[SIZEOF_GUIDACKMSG];
// };
// struct MSG_FC_MOVE_HACKSHIELD_CRCReqMsg			// 2006-06-05 by cmkwon
// {
// 	// 2008-03-24 by cmkwon, 핵쉴드 2.0 적용 - AntiCpSvrFunc.h 파일의 정의를 그대로 사용하고 클라이언트에도 헤더파일 전달하기
// 	//unsigned char	pbyReqMsg[SIZE_SIZEOF_REQMSG];
// 	unsigned char	pbyReqMsg[SIZEOF_REQMSG];
// };
// struct MSG_FC_MOVE_HACKSHIELD_CRCAckMsg			// 2006-06-05 by cmkwon
// {
// 	// 2008-03-24 by cmkwon, 핵쉴드 2.0 적용 - AntiCpSvrFunc.h 파일의 정의를 그대로 사용하고 클라이언트에도 헤더파일 전달하기
// 	//unsigned char	pbyAckMsg[SIZE_SIZEOF_ACKMSG];	
// 	unsigned char	pbyAckMsg[SIZEOF_ACKMSG];	
// };
struct MSG_FC_MOVE_HACKSHIELD_CRCReqMsg			// 2008-04-03 by cmkwon, 핵쉴드 서버 연동 시스템 수정 - 
{
	_AHNHS_TRANS_BUFFER		stRequestBuf;
};
struct MSG_FC_MOVE_HACKSHIELD_CRCAckMsg			// 2008-04-03 by cmkwon, 핵쉴드 서버 연동 시스템 수정 - 
{
	_AHNHS_TRANS_BUFFER		stResponseBuf;
};

struct MSG_FC_MOVE_HACKSHIELD_HACKING_CLIENT	// 2006-06-05 by cmkwon
{
	long			lHackingClinetCode;			// ERR_AHNHS_XXXXXX	
	char			szErrString[SIZE_STRING_128];			// 2006-10-20 by cmkwon, 추가함(해킹 프로그램명)
};
	
	
struct MSG_FC_MOVE_XIGNCODE_REQ_SCAN_INIT	// 2008-11-28 by cmkwon, 대만 Netpower_Tpe XignCode(게임가드) 적용 - 
{
	BYTE	byCurDataIndex;		// 0,1,2,3,...
	BYTE	byDataCount;		// Data Count
	BYTE	byCheckData[MAX_PACKET_SIZE_FOR_XIGNCODE];
};
struct MSG_FC_MOVE_XIGNCODE_REQ_SCAN_INIT_OK	// 2008-11-28 by cmkwon, 대만 Netpower_Tpe XignCode(게임가드) 적용 - 
{
	BYTE	byResultData[MAX_PACKET_SIZE_FOR_XIGNCODE];
};
struct MSG_FC_MOVE_XIGNCODE_REQ_SCAN_CHECK	// 2008-11-28 by cmkwon, 대만 Netpower_Tpe XignCode(게임가드) 적용 - 
{
	BYTE	byCurDataIndex;		// 0,1,2,3,...
	BYTE	byDataCount;		// Data Count
	BYTE	byCheckData[MAX_PACKET_SIZE_FOR_XIGNCODE];
};
struct MSG_FC_MOVE_XIGNCODE_REQ_SCAN_CHECK_OK	// 2008-11-28 by cmkwon, 대만 Netpower_Tpe XignCode(게임가드) 적용 - 
{
	BYTE	byResultData[MAX_PACKET_SIZE_FOR_XIGNCODE];
};

struct MSG_FC_MOVE_NPROTECT_REQ_AUTH_DATA		// 2009-03-09 by cmkwon, 일본 Arario nProtect에 CS인증 적용하기 - 
{
	BYTE 	reqAuthData[MAX_PACKET_SIZE_FOR_NPROTECT];
};
struct MSG_FC_MOVE_NPROTECT_REQ_AUTH_DATA_OK	// 2009-03-09 by cmkwon, 일본 Arario nProtect에 CS인증 적용하기 - 
{
	BYTE	resAuthData[MAX_PACKET_SIZE_FOR_NPROTECT];
};

struct MSG_FC_MOVE_XTRAP_REQ_STEP			// 2009-10-06 by cmkwon, 베트남 게임 가드 X-TRAP으로 변경 - 
{
	BYTE	reqCSStepData[XTRAP_SIZE_MAX_SESSIONBUF];
};

struct MSG_FC_MOVE_XTRAP_REQ_STEP_OK		// 2009-10-06 by cmkwon, 베트남 게임 가드 X-TRAP으로 변경 - 
{
	BYTE	resCSStepData[XTRAP_SIZE_MAX_SESSIONBUF];
};

struct MSG_FC_MOVE_APEX_REQ_APEXDATA		// 2009-11-04 by cmkwon, 태국 게임가드 Apex로 변경 - 
{
	char	cMsgType;
	char	szApexData[SECURITY_APEX_MaxPacketLen];
	int		nApexDataLen;
};
typedef MSG_FC_MOVE_APEX_REQ_APEXDATA		MSG_FC_MOVE_APEX_REQ_APEXDATA_OK;		// 2009-11-04 by cmkwon, 태국 게임가드 Apex로 변경 - 


///////////////////////////////////////////////////////////////////////////////
// FN_MOVE (Field server <-> NPC server)


///////////////////////////////////////////////////////////////////////////////
// FC_BATTLE (Field server <-> Client)
struct MEX_TARGET_INFO
{
	ClientIndex_t	TargetIndex;			// 공격 대상 ClientIndex or MonterIndex, 0이면 ItemFieldIndex만 유효
	UINT			TargetItemFieldIndex;	// 공격 대상에 부착된 아이템이면 TargetIndex 유효, 아니면 TargetIndex는 0
	AVECTOR3		TargetPosition;			// 공격 클라이언트의 화면에서의 타켓 포지션	
	USHORT			MultiTargetIndex;		// 2011-03-21 by hskim, 인피니티 3차 - 몬스터 멀티 타겟팅 기능 추가

public:
	void SetNullTarget()
	{
		TargetIndex = 0;
		TargetItemFieldIndex = 0;
		MultiTargetIndex = 0;				// 2011-03-21 by hskim, 인피니티 3차 - 몬스터 멀티 타겟팅 기능 추가
	}
};

typedef struct
{
	MEX_TARGET_INFO	TargetInfo;
	AVECTOR3		FirePosition;		// 무기의 발사 위치
	BYTE			AttackType;			// 공격 타입, ATT_TYPE_XXX, see below
#ifdef BONUS_STAT_ITEM
	UINT			ItemUniqueNumber;
#endif
	UINT			SkillNum;			// 스킬 사용 시 사용
} MSG_FC_BATTLE_ATTACK;					// C->F

// ATT_TYPE_XXX, 1~100: 1형 공격, 101~200: 2형 공격
#define	ATT_TYPE_NONE			((BYTE)0)	// Attack Type 없음
// 1형 공격
#define	ATT_TYPE_GENERAL_PRI		((BYTE)1)	// 1형 무기 일반 공격
#define	ATT_TYPE_GROUND_BOMBING_PRI	((BYTE)2)	// 1형 지상 폭격 모드
#define	ATT_TYPE_AIR_BOMBING_PRI	((BYTE)3)	// 1형 공중 폭격 모드
#define	ATT_TYPE_SEIGE_PRI			((BYTE)4)	// 1형 시즈 모드
#define	ATT_TYPE_SPLASH_PRI			((BYTE)5)	// 1형 Splash 공격 모드
#define	ATT_TYPE_END_PRI			((BYTE)100)	// 1형 무기 공격 끝, 실제로 안 쓰임
// 2형 공격
#define ATT_TYPE_GENERAL_SEC		((BYTE)101)	// 2형 무기 일반 공격
#define ATT_TYPE_GROUND_BOMBING_SEC	((BYTE)102)	// 2형 지상 폭격 모드
#define ATT_TYPE_AIR_BOMBING_SEC	((BYTE)103)	// 2형 공중 폭격 모드
#define ATT_TYPE_SIEGE_SEC			((BYTE)104)	// 2형 시즈 모드
#define	ATT_TYPE_END_SEC			((BYTE)200)	// 2형 무기 공격 끝, 실제로 안 쓰임

// 2010-06-15 by shcho&hslee 펫시스템 - 펫 공격 처리
#define ATT_TYPE_PET_ATK			ATT_TYPE_PET_GENERAL
#define ATT_TYPE_PET_GENERAL		((BYTE)201) // 펫 공격(일반 공격)
#define ATT_TYPE_PET_END			((BYTE)300)	// 펫 공격 끝

#define IS_PET_ATT_TPYE(_ATT_TYPE)			(IS_IN_RANGE(ATT_TYPE_PET_START, _ATT_TYPE, ATT_TYPE_PET_END))
// END 2010-06-15 by shcho&hslee 펫시스템 - 펫 공격 처리
#define IS_PRIMARY_ATT_TYPE(_ATT_TYPE)		(IS_IN_RANGE(ATT_TYPE_GENERAL_PRI, _ATT_TYPE, ATT_TYPE_END_PRI))
#define IS_SECONDARY_ATT_TYPE(_ATT_TYPE)	(IS_IN_RANGE(ATT_TYPE_GENERAL_SEC, _ATT_TYPE, ATT_TYPE_END_SEC))

///////////////////////////////////////////////////////////////////////////////
// 2007-06-04 by cmkwon
struct SATTACK_PARAMETER
{
	BYTE			AttackType;						// 2010-04-05 by cmkwon, 인피2차 M2M 2형 무기 보완 처리 - 
	USHORT			MultiTargetIndex;				// 2011-04-04 by hskim, 인피니티 3차 - 몬스터 멀티 타겟팅 기능 추가
	UID16_t			WeaponIndex;					// 탄두 고유번호 - 2형 무기만 처리됨
	DWORD			dwShotTick;						// 2007-06-08 by cmkwon, 발사 Tick
	float			fPierceAttackProbability;		// 공격 피어스율
	float			fAttack;						// 공격력
	float			fMaxAttack;						// 최대공격력	
	ITEM* pWeaponItemInfo;				// 공격 무기 아이템 정보
	float			fAttackProbability;				// 공격 확률
	ClientIndex_t	TargetIndex;					// 2010-04-05 by cmkwon, 인피2차 M2M 2형 무기 보완 처리 - 
};
typedef mt_vector<SATTACK_PARAMETER>			mtvectSATTACK_PARAMETER;		// 2007-06-07 by cmkwon
typedef vector<SATTACK_PARAMETER>				vectSATTACK_PARAMETER;			// 2010-04-05 by cmkwon, 인피2차 M2M 2형 무기 보완 처리 - 

typedef struct
{	
	AVECTOR3		FirePosition;		// 무기의 발사 위치
	UINT			ItemNum;			// 무기의 ItemNum
	USHORT			RemainedBulletFuel;	// 남은 총알(혹은 Fuel)의 수
	UINT			SkillNum;			// 스킬 사용 시 사용
	ClientIndex_t	AttackIndex;
	MEX_TARGET_INFO	TargetInfo;
	ClientIndex_t	DelegateClientIdx;	// 2009-09-09 ~ 2010 by dhjin, 인피니티 - 몬스터 간 2형 무기 폭팔 처리 할 위임 클라이언트
	BYTE			AttackType;			// 공격 타입, ATT_TYPE_XXX
	UID16_t			WeaponIndex;		// 클라이언트에서 발사된 총알의 인덱스, 서버에서 생성, CUID16Generator 사용
} MSG_FC_BATTLE_ATTACK_OK;				// F->C_in_range

typedef struct
{
	UINT			ItemNum;			// 무기의 ItemNum, 몬스터의 2형 공격에만 사용됨
	UID16_t			WeaponIndex;		// 클라이언트에서 발사된 총알의 인덱스, 서버에서 생성, CUID16Generator 사용
	ClientIndex_t	AttackIndex;
	MEX_TARGET_INFO	TargetInfo;
	BYTE			AttackType;			// 공격 타입, ATT_TYPE_XXX
} MSG_FC_BATTLE_ATTACK_FIND;			// C->F

typedef struct
{
	ClientIndex_t	AttackIndex;	
	UID16_t			WeaponIndex;		// 클라이언트에서 발사된 총알의 인덱스, 서버에서 생성, CUID16Generator 사용
	BYTE			AttackType;			// 공격 타입, ATT_TYPE_XXX
	ClientIndex_t	TargetIndex;			// 공격 대상 ClientIndex or MonterIndex, 0이면 ItemFieldIndex만 유효
	UINT			TargetItemFieldIndex;	// 공격 대상에 부착된 아이템이면 TargetIndex 유효, 아니면 TargetIndex는 0
} MSG_FC_BATTLE_ATTACK_FIND_OK;			// F->C_in_range

// 마인류 처리
typedef struct
{
	BYTE			NumOfMines;
	ClientIndex_t	TargetIndex;		// Mine을 쏠때 Target이 있으면 설정된다(서버는 클라이언트로 전달만 하면됨)
	ARRAY_(AVECTOR3);					// MINE이 떨어질 위치
} MSG_FC_BATTLE_DROP_MINE;				// C->F

typedef struct
{
	UINT			ItemFieldIndex;		// 습득 전까지 서버가 임시로 관리하는 마인 번호
	INT				ItemNum;			// 클라이언트에 아이템의 종류를 보여주기 위해 보냄
	ClientIndex_t	AttackIndex;		// MINE 공격자
	ClientIndex_t	TargetIndex;		// Mine을 쏠때 Target이 있으면 설정된다(서버는 클라이언트로 전달만 하면됨)
	AVECTOR3		DropPosition;		// MINE이 떨어질 위치
	USHORT			RemainedBulletFuel;	// 남은 총알(혹은 Fuel)의 수
} MSG_FC_BATTLE_DROP_MINE_OK;			// F->C_in_range, 아이템 보여주기

typedef struct
{
	UINT			ItemFieldIndex;		// 서버가 임시로 관리하는 마인 번호
	MEX_TARGET_INFO	TargetInfo;			// 피공격자
} MSG_FC_BATTLE_MINE_ATTACK;			// C->F

typedef struct
{
	UINT			ItemFieldIndex;		// 서버가 임시로 관리하는 마인 번호
	MEX_TARGET_INFO	TargetInfo;			// 피공격자
} MSG_FC_BATTLE_MINE_ATTACK_OK;			// F->C_in_range

typedef struct
{
	UINT			ItemFieldIndex;		// 서버가 임시로 관리하는 마인 번호
	MEX_TARGET_INFO	TargetInfo;			// 피공격자
} MSG_FC_BATTLE_MINE_ATTACK_FIND;		// C->F

typedef struct
{
	UINT			ItemFieldIndex;		// 서버가 임시로 관리하는 마인 번호
	MEX_TARGET_INFO	TargetInfo;			// 피공격자
} MSG_FC_BATTLE_MINE_ATTACK_FIND_OK;	// F->C_in_range

// 2007-08-07 by cmkwon, 1형/2형 무기 총알 충전 아이템 구현 - 아래와 같이 수정함
//typedef struct  {
//	USHORT			BulletCount;		// 무기의 reload된 총알의 최종 개수
//} MSG_FC_BATTLE_PRI_BULLET_RELOADED;
//
//typedef struct  {
//	USHORT			BulletCount;		// 무기의 reload된 총알의 최종 개수
//} MSG_FC_BATTLE_SEC_BULLET_RELOADED;

// 2007-08-07 by cmkwon, 1형/2형 무기 총알 충전 아이템 구현 - BULLET_RECHARGE_TYPE_XXX 정의 추가
#define BULLET_RECHARGE_TYPE_NORMAL			((BYTE)0)
#define BULLET_RECHARGE_TYPE_REPAIR_SHOP	((BYTE)1)
#define BULLET_RECHARGE_TYPE_BULLET_ITEM	((BYTE)2)
#define BULLET_RECHARGE_TYPE_ADMIN_COMMAND	((BYTE)3)
struct  MSG_FC_BATTLE_PRI_BULLET_RELOADED
{
	USHORT			BulletCount;		// 무기의 reload된 총알의 최종 개수
	USHORT			RechargeCount;		// 2007-08-07 by cmkwon, 1형/2형 무기 총알 충전 아이템 구현 - 추가된 필드
	BYTE			RechargeType;		// 2007-08-07 by cmkwon, 1형/2형 무기 총알 충전 아이템 구현 - 추가된 필드(BULLET_RECHARGE_TYPE_XXX)
};
typedef MSG_FC_BATTLE_PRI_BULLET_RELOADED		 MSG_FC_BATTLE_SEC_BULLET_RELOADED;

// Kind of Damages: 이펙트를 표시하기 위해 사용한다. check: 아래분류는 재정의되어야 한다.
// DAMAGEKIND_XXX
#define DAMAGEKIND_NO_DAMAGE	(BYTE)0x00
#define DAMAGEKIND_NORMAL		(BYTE)0x01	// 0 < DAMAGE < 100
#define DAMAGEKIND_CRITICAL		(BYTE)0x02	// 피공격자의 방어력 무시
#define DAMAGEKIND_ADD_DAMAGE	(BYTE)0x03	// 2009-09-09 ~ 2010 by dhjin, 인피니티 - 절대값 추가 타격치 아이템
#define DAMAGEKIND_REFLECTION	(BYTE)0x04	// 2009-09-09 ~ 2010-02-10 by dhjin, 인피니티 - 데미지 반사
#define DAMAGEKIND_PET			(BYTE)0x05	// 2010-11-01 by jskim, 펫 데미지 View 변경

typedef struct
{
	ClientIndex_t	TargetIndex;			// 해당 아이템을 소유한 ClientIndex
	UINT			TargetItemFieldIndex;
} MSG_FC_BATTLE_ATTACK_EXPLODE_ITEM;		// 기체가 달고 있는 아이템(DUMMY 류 등)이 터지는 경우

typedef struct
{
	ClientIndex_t	TargetIndex;			// 해당 아이템을 소유한 ClientIndex
	UINT			TargetItemFieldIndex;
} MSG_FC_BATTLE_ATTACK_HIDE_ITEM;			// 기체가 달고 있는 아이템(DUMMY 류 등)이 그냥 사라지는 경우(Time Out 등로 인해...)

typedef struct
{
	ClientIndex_t	TargetIndex;			// 해당 아이템을 소유한 ClientIndex
	BYTE			ItemKind;
	UINT			TargetItemFieldIndex;
} MSG_FC_BATTLE_ATTACK_EXPLODE_ITEM_W_KIND;	// With KIND, 기체가 달고 있는 아이템(FIXER 류 등)이 터지는 경우

typedef struct
{
	ClientIndex_t	TargetIndex;			// 해당 아이템을 소유한 ClientIndex
	BYTE			ItemKind;
	UINT			TargetItemFieldIndex;
} MSG_FC_BATTLE_ATTACK_HIDE_ITEM_W_KIND;	// With KIND, 기체가 달고 있는 아이템(FIXER 류 등)이 그냥 사라지는 경우(Time Out 등로 인해...)

// 번들류 처리
typedef struct
{
	UID64_t			ItemUniqueNumber;
	ClientIndex_t	AttackClientIndex;
	ClientIndex_t	TargetClientIndex;
	AVECTOR3		DropPosition;
} MSG_FC_BATTLE_DROP_BUNDLE;

typedef struct
{	
	ClientIndex_t	TargetClientIndex;
	INT				ItemNum;			// 클라이언트에 아이템의 종류를 보여주기 위해 보냄
	AVECTOR3		DropPosition;
	UID64_t			ItemUniqueNumber;
	ClientIndex_t	AttackClientIndex;
	USHORT			NumOfBullet;		// 발사할 탄체의 개수
} MSG_FC_BATTLE_DROP_BUNDLE_OK;

typedef struct
{
	UID64_t			BundleItemUniqueNumber;	// 번들의 ItemUniqueNumber
	AVECTOR3		AttackPosition;
	ClientIndex_t	TargetIndex;
	AVECTOR3		TargetPosition;
} MSG_FC_BATTLE_BUNDLE_ATTACK;

typedef struct
{
	INT				ItemNum;				// bundle의 링크 아이템(미사일 등)의 ItemNum
	ClientIndex_t	AttackIndex;
	AVECTOR3		AttackPosition;
	ClientIndex_t	TargetIndex;
	AVECTOR3		TargetPosition;
} MSG_FC_BATTLE_BUNDLE_ATTACK_RESULT;

typedef struct
{
	UID64_t			BundleItemUniqueNumber;	// 번들의 ItemUniqueNumber
	AVECTOR3		AttackPosition;
	ClientIndex_t	TargetIndex;
	UINT			TargetItemFieldIndex;	// 대상 아이템
	AVECTOR3		TargetPosition;
} MSG_FC_BATTLE_BUNDLE_ATTACK_ITEM;

typedef struct
{
	INT				ItemNum;				// bundle의 링크 아이템(미사일 등)의 ItemNum
	ClientIndex_t	AttackIndex;
	AVECTOR3		AttackPosition;
	ClientIndex_t	TargetIndex;
	UINT			TargetItemFieldIndex;	// 대상 아이템
	AVECTOR3		TargetPosition;
} MSG_FC_BATTLE_BUNDLE_ATTACK_ITEM_RESULT;

// check: 필요하면 살림(현재 NO BODY임), kelovon, 20030917
//typedef struct
//{
//} MSG_FC_BATTLE_TOGGLE_SHIELD;

typedef struct
{
	ClientIndex_t	AttackIndex;		// shield를 가동한 캐릭터
	BYTE			IsOn;				// 0(FALSE): off, 1(TRUE): on
	INT				ItemNum;
} MSG_FC_BATTLE_TOGGLE_SHIELD_RESULT;	// F->C, SHIELD류 부착중이면 MSG_FC_CHARACTER_GET_OTHER_INFO_OK보낸 후 이 MSG를 붙여보낸다.

// check: 필요하면 살림(현재 NO BODY임), kelovon, 20040517
//typedef struct
//{
//} MSG_FC_BATTLE_TOGGLE_DECOY;

typedef struct
{
	ClientIndex_t	AttackIndex;	// decoy를 가동한 캐릭터
	BYTE			IsOn;			// 0(FALSE): off, 1(TRUE): on
	INT				ItemNum;
} MSG_FC_BATTLE_TOGGLE_DECOY_OK;	// F->C, DECOY류 부착중이면 MSG_FC_CHARACTER_GET_OTHER_INFO_OK보낸 후 이 MSG를 붙여보낸다.

typedef struct
{
	ClientIndex_t	TargetIndex;		// shield를 가동시키고 있는 캐릭터
	AVECTOR3		CollisionPosition;	// 충돌 위치
} MSG_FC_BATTLE_SHIELD_DAMAGE;

// 더미(DUMMY)류
typedef struct
{
	UID64_t		ItemUniqueNumber;
//	BYTE		NumOfDummies;			// check: 사라짐. 20030930, kelovon with jinking
//	ARRAY_(AVECTOR3);					// DUMMY의 위치(기체에 대한 상대 좌표), check: 사라짐. 20030930, kelovon with jinking
} MSG_FC_BATTLE_DROP_DUMMY;				// 더미는 한번에 다 쏜다.

// 2007-06-21 by cmkwon, 체프 하나의 메시지로 모두 전송 - 아래와 같이 구조체 수정
//typedef struct
//{
//	ClientIndex_t	AttackIndex;		// DUMMY를 발동한 기체
//	UINT			ItemFieldIndex;		// 습득 전까지 서버가 임시로 관리하는 DUMMY 번호
//	INT				ItemNum;			// 클라이언트에 아이템의 종류를 보여주기 위해 보냄
////	AVECTOR3		DropPosition;		// 기체에 대한 상대 좌표, check: 사라짐. 20030930, kelovon with jinking
//} MSG_FC_BATTLE_DROP_DUMMY_OK;			// 아이템 보여주기,  DUMMY류 부착중이면 MSG_FC_CHARACTER_GET_OTHER_INFO_OK보낸 후 이 MSG를 붙여보낸다.
struct MSG_FC_BATTLE_DROP_DUMMY_OK		// 2007-06-21 by cmkwon, 체프 하나의 메시지로 모두 전송 -
{
	ClientIndex_t	AttackIndex;		// DUMMY를 발동한 기체
	INT				ItemNum;			// 클라이언트에 아이템의 종류를 보여주기 위해 보냄
	INT				DummyCounts;		// 2007-06-21 by cmkwon, 체프 하나의 메시지로 모두 전송 -
	_ARRAY(UINT ItemFieldIndex);		// 2007-06-21 by cmkwon, 체프 하나의 메시지로 모두 전송 - DummyCounts 만큼 붙여서 전송
};

// 픽서(FIXER)류
typedef struct
{
	UID64_t			ItemUniqueNumber;
	ClientIndex_t	TargetIndex;
} MSG_FC_BATTLE_DROP_FIXER;				// 한 번에 개수만큼 다 쏨

typedef struct
{
	ClientIndex_t	AttackIndex;		// FIXER를 쏜 기체
	ClientIndex_t	TargetIndex;
	UINT			ItemFieldIndex;		// 습득 전까지 서버가 임시로 관리하는 FIXER류 번호
	INT				ItemNum;			// 클라이언트에 아이템의 종류를 보여주기 위해 보냄
} MSG_FC_BATTLE_DROP_FIXER_OK;			// 각각에 대해 전송, 아이템 보여주기, FIXER류 부착중이면 MSG_FC_CHARACTER_GET_OTHER_INFO_OK보낸 후 이 MSG를 붙여보낸다.

// check: 필요하면 살림(현재 NO BODY임), kelovon, 20030612
//typedef struct
//{
//} MSG_FC_BATTLE_REQUEST_PK;			// C->F, client의 PK 요청

typedef struct
{
	ClientIndex_t	ClientIndex;		// PK를 요청한 client
} MSG_FC_BATTLE_REQUEST_PK_OK;			// F->C, pk 요청 승낙

// check: 필요하면 살림(현재 NO BODY임), kelovon, 20030612
//typedef struct
//{
//} MSG_FC_BATTLE_CANCEL_PK;			// F->C, PK 해제

typedef struct
{
	ClientIndex_t	TargetClientIndex;		// 피요청자
} MSG_FC_BATTLE_REQUEST_P2P_PK;				// C->F, 일대일 PK 요청

typedef struct
{
	ClientIndex_t	SourceClientIndex;		// 요청자
} MSG_FC_BATTLE_REQUEST_P2P_PK_OK;			// F->C, 일대일 PK 요청

typedef struct
{
	ClientIndex_t	SourceClientIndex;		// 요청자
} MSG_FC_BATTLE_ACCEPT_REQUEST_P2P_PK;		// C->F, 일대일 PK 승낙

typedef struct
{
	ClientIndex_t	PeerClientIndex;		// 상대방
} MSG_FC_BATTLE_ACCEPT_REQUEST_P2P_PK_OK;	// F->C, 일대일 PK 승낙, 양 쪽으로 보냄, 클라이언트는 받으면 PK 시작

typedef struct
{
	ClientIndex_t	SourceClientIndex;		// 요청자
} MSG_FC_BATTLE_REJECT_REQUEST_P2P_PK;		// C->F, 일대일 PK 거절

typedef struct
{
	ClientIndex_t	TargetClientIndex;		// 피요청자
} MSG_FC_BATTLE_REJECT_REQUEST_P2P_PK_OK;	// F->C, 일대일 PK 거절

typedef struct
{
	ClientIndex_t	TargetClientIndex;		// 피요청자
} MSG_FC_BATTLE_SURRENDER_P2P_PK;			// C->F, 일대일 PK 항복

typedef struct
{
	ClientIndex_t	SourceClientIndex;		// 요청자
} MSG_FC_BATTLE_SURRENDER_P2P_PK_OK;		// F->C, 일대일 PK 항복

typedef struct
{
	ClientIndex_t	SourceClientIndex;		// 요청자
} MSG_FC_BATTLE_ACCEPT_SURRENDER_P2P_PK;	// C->F, 일대일 PK 항복 승낙, 이에 대한 응답은 MSG_FC_BATTLE_END_P2P_PK로 처리

/*
typedef struct
{
	ClientIndex_t	TargetClientIndex;		// 피요청자
} MSG_FC_BATTLE_ACCEPT_SURRENDER_P2P_PK_OK;	// F->C, 일대일 PK 항복 승낙
*/

typedef struct
{
	ClientIndex_t	SourceClientIndex;		// 요청자
} MSG_FC_BATTLE_REJECT_SURRENDER_P2P_PK;	// C->F, 일대일 PK 항복 거절

typedef struct
{
	ClientIndex_t	TargetClientIndex;		// 피요청자
} MSG_FC_BATTLE_REJECT_SURRENDER_P2P_PK_OK;	// F->C, 일대일 PK 항복 거절

typedef struct
{
	ClientIndex_t	PeerClientIndex;		// 상대방의 ClientIndex
	USHORT			EndType;				// 결투 종료 타입, BATTLE_END_XXX
} MSG_FC_BATTLE_END_P2P_PK;					// PK 종료

// 전투 종료 타입, BATTLE_END_XXX
#define BATTLE_END_WIN			(USHORT)0x0000	// 승리
#define BATTLE_END_DEFEAT		(USHORT)0x0001	// 패배
#define BATTLE_END_TIE			(USHORT)0x0002	// 무승부
#define BATTLE_END_END			(USHORT)0x0003	// 그냥 종료되었습니다(이유불문)
#define BATTLE_END_TIME_LIMITE	(USHORT)0x0004	// 시간제한
#define BATTLE_END_SURRENDER	(USHORT)0x0005	// 항복

typedef struct
{
	ClientIndex_t	TargetIndex;	// TargetIndex
	USHORT			AmountDamage;	// 데미지 량
	BYTE			DamageKind;		// DAMAGEKIND_XXX
	BYTE			byIsPrimaryWeapon;		// 2008-12-03 by cmkwon, 데미지 정보에 1형,2형 정보 추가 - 
	USHORT			MultiTargetIndex;		// 2011-03-21 by hskim, 인피니티 3차 - 몬스터 멀티 타겟팅 기능 추가
} MSG_FC_BATTLE_SHOW_DAMAGE;		// F->C, 공격 데미지량을 표시함



struct MSG_FC_BATTLE_ATTACK_EVASION		// 2005-12-12 by cmkwon
{
	ClientIndex_t	AttackIndex;
	MEX_TARGET_INFO	TargetInfo;
	UINT			ItemNum;			// 무기의 ItemNum, 몬스터의 2형 공격에만 사용됨
	UID16_t			WeaponIndex;		// 클라이언트에서 발사된 총알의 인덱스, 서버에서 생성, CUID16Generator 사용
	BYTE			AttackType;			// 공격 타입, ATT_TYPE_XXX
};

typedef MSG_FC_BATTLE_ATTACK_EVASION		MSG_FC_BATTLE_ATTACK_EVASION_OK;	// 2005-12-12 by cmkwon

struct MSG_FC_BATTLE_DELETE_DUMMY_OK		// 2006-12-04 by dhjin
{
	ClientIndex_t	AttackIndex;
	UINT			ItemFieldIndex;
};

struct MSG_FC_BATTLE_EXPLODE_DUMMY_OK		// 2006-12-04 by dhjin
{
	ClientIndex_t	AttackIndex;
	UINT			ItemFieldIndex;
};


///////////////////////////////////////////////////////////////////////////////
// FN_BATTLE (Field server <-> NPC server)

typedef struct
{
	ChannelIndex_t	ChannelIndex;
	ClientIndex_t	AttackIndex;
	ClientIndex_t	TargetIndex;
	UINT			WeaponItemNumber;	// 무기 타입(각 무기종류,스킬종류)
	USHORT			WeaponIndex;		// 남은 총알(혹은 Fuel)의 수, Fuel인 경우에는 x10하여 계산함
	AVECTOR3		TargetPosition;		// 공격 클라이언트의 화면에서의 타켓 포지션
	USHORT			MultiTargetIndex;	// 2011-03-21 by hskim, 인피니티 3차 - 몬스터 멀티 타겟팅 기능 추가
} MSG_FN_BATTLE_ATTACK_PRIMARY;

typedef struct
{
	ClientIndex_t	AttackIndex;
	ClientIndex_t	TargetIndex;
	UINT			WeaponItemNumber;	// 무기 타입(각 무기종류,스킬종류)
	USHORT			WeaponIndex;		// 남은 총알(혹은 Fuel)의 수, Fuel인 경우에는 x10하여 계산함
	AVECTOR3		TargetPosition;		// 공격 클라이언트의 화면에서의 타켓 포지션
	BYTE			DamageKind;
} MSG_FN_BATTLE_ATTACK_RESULT_PRIMARY;

typedef struct
{
	ChannelIndex_t	ChannelIndex;
	ClientIndex_t	AttackIndex;
	ClientIndex_t	TargetIndex;
	UINT			WeaponItemNumber;	// 무기 타입(각 무기종류,스킬종류)
	USHORT			WeaponIndex;		// 남은 총알(혹은 Fuel)의 수, Fuel인 경우에는 x10하여 계산함
	AVECTOR3		TargetPosition;		// 공격 클라이언트의 화면에서의 타켓 포지션
	USHORT			MultiTargetIndex;	// 2011-03-21 by hskim, 인피니티 3차 - 몬스터 멀티 타겟팅 기능 추가
	BYTE			Distance;
	BYTE			SecAttackType;		// 2형 무기 공격 타입: SEC_ATT_TYPE_XXX, see below
	AVECTOR3		AttackPosition;		// 2009-09-09 ~ 2010 by dhjin, 인피니티 - 몬스터 위치
} MSG_FN_BATTLE_ATTACK_SECONDARY;

typedef struct
{
	ClientIndex_t	AttackIndex;
	ClientIndex_t	TargetIndex;
	UINT			WeaponItemNumber;	// 무기 타입(각 무기종류,스킬종류)
	USHORT			WeaponIndex;		// 남은 총알(혹은 Fuel)의 수, Fuel인 경우에는 x10하여 계산함
	AVECTOR3		TargetPosition;		// 공격 클라이언트의 화면에서의 타켓 포지션
	BYTE			DamageKind;
	BYTE			Distance;
	BYTE			SecAttackType;		// 2형 무기 공격 타입: SEC_ATT_TYPE_XXX
} MSG_FN_BATTLE_ATTACK_RESULT_SECONDARY;

typedef struct
{
	ClientIndex_t	AttackIndex;
	ClientIndex_t	TargetIndex;
	AVECTOR3		TargetPosition;		// 공격 클라이언트의 화면에서의 타켓 포지션
	USHORT			WeaponIndex;		// 클라이언트에서 발사된 총알의 인덱스
	ChannelIndex_t	ChannelIndex;
} MSG_FN_BATTLE_ATTACK_FIND;

typedef struct
{
	ClientIndex_t	AttackIndex;
	ClientIndex_t	TargetIndex;
	USHORT			WeaponIndex;		// 클라이언트에서 발사된 총알의 인덱스
	BYTE			DamageKind;
} MSG_FN_BATTLE_ATTACK_FIND_RESULT;

typedef struct
{
	ChannelIndex_t	ChannelIndex;		// check_cmkwon, 추가하기~, 20040330, kelovon
	ClientIndex_t	AttackIndex;		// Attack Character
	ClientIndex_t	TargetIndex;		// Target Monster
	INT				ItemNum;			// Fixer의 ItemNum
} MSG_FN_BATTLE_DROP_FIXER;				// F -> N

typedef struct
{
	ChannelIndex_t	ChannelIndex;		// check_cmkwon, 추가하기~, 20040330, kelovon
	ClientIndex_t	AttackIndex;		// Attack Character
	ClientIndex_t	TargetIndex;		// Target Monster
	UINT			ItemFieldIndex;
	INT				ItemNum;			// 클라이언트에 아이템의 종류를 보여주기 위해 보냄
} MSG_FN_BATTLE_DROP_FIXER_OK;			// N -> F

typedef struct _MSG_FN_BATTLE_ATTACK_HIDE_ITEM_W_KIND : public MSG_FC_BATTLE_ATTACK_HIDE_ITEM_W_KIND
{
	ChannelIndex_t	ChannelIndex;
} MSG_FN_BATTLE_ATTACK_HIDE_ITEM_W_KIND;

typedef struct
{
	ChannelIndex_t	ChannelIndex;
	ClientIndex_t	AttackIndex;
	ClientIndex_t	TargetIndex;
	float			DamageAmount;
	BYTE			ItemKind;
} MSG_FN_BATTLE_SET_ATTACK_CHARACTER;

///////////////////////////////////////////////////////////////////////////////
// Party(편대, 파티) 관련 프로토쿨
///////////////////////////////////////////////////////////////////////////////

// IMServer에서 FieldServer로 넘겨주어야 하는 정보
struct MEX_FIELD_PARTY_INFO
{
	INT		nTotalPartyMember;			// 총 파티원 수
	BYTE	lowestMemberLevel;			// 최하 파티원 level, check: FieldServer가 IMServer로 사용자의 level 정보 등 update하는 부분 구현해야 함! 20031101, kelovon
};

// 파티 생성
typedef struct
{
	UID32_t		CharacterUniqueNumber;		// 생성자(파티장) 번호
	SPARTY_INFO	PartyInfo;					// 2008-06-02 by dhjin, EP3 편대 수정 - 편대 정보
	DWORD		Padding;					// 2011-07-21 by hskim, 인증 서버 구현 - 기존 서버와 호환 안되도록 구조체 크기 바꿈
} MSG_IC_PARTY_CREATE;

typedef struct
{
	PartyID_t				PartyID;
	UID32_t					CharacterUniqueNumber;	// 생성자(파티장) 번호
	MEX_FIELD_PARTY_INFO	FieldPartyInfo;
	BYTE					ExpDistributeType;		// 2008-06-02 by dhjin, EP3 편대 수정 - 편대 경험치 분배 타잎
	BYTE					ItemDistributeType;		// 2008-06-02 by dhjin, EP3 편대 수정 - 편대 아이템 분배 타잎
} MSG_FI_PARTY_CREATE_OK;

typedef struct
{
	PartyID_t	PartyID;
} MSG_FC_PARTY_CREATE_OK;

// 파티원 초대
typedef struct
{
	char		CharacterName[SIZE_MAX_CHARACTER_NAME];		// 초대할 상대방
} MSG_FC_PARTY_REQUEST_INVITE;

typedef struct
{
	PartyID_t	PartyID;
	char		MasterCharacterName[SIZE_MAX_CHARACTER_NAME];	// 파티장
} MSG_FC_PARTY_REQUEST_INVITE_QUESTION;

typedef struct
{
	PartyID_t	PartyID;
} MSG_FC_PARTY_ACCEPT_INVITE;

typedef struct
{
	PartyID_t				PartyID;
	UID32_t					CharacterUniqueNumber;		// 새로 가입할 파티원
	MEX_FIELD_PARTY_INFO	FieldPartyInfo;				// 이 MSG가 F->I일 때는 무시함
} MSG_FI_PARTY_ACCEPT_INVITE_OK;						// F->I, I->F 둘 다 쓰임

struct IMPartyMember;

typedef struct _IM_PARTY_MEMBER_INFO
{
	UID32_t		CharacterUniqueNumber;
	char		CharacterName[SIZE_MAX_CHARACTER_NAME];
	BYTE		PilotFace;
	USHORT		UnitKind;
	USHORT		Race;
	BYTE		Level;
	//char		MapName[SIZE_MAX_MAP_NAME];	// IS_VALID_MAP_NAME()이 FALSE이면, 파티 소속이나 게임중이 아닌 캐릭임(죽어서 나간 캐릭터 등)
	MAP_CHANNEL_INDEX	MapChannelIndex;	// IsValid()이 FALSE이면, 파티 소속이나 게임중이 아닌 캐릭임(죽어서 나간 캐릭터 등)
	EN_CHECK_TYPE	VoipType;		// 2008-07-16 by dhjin, EP3 - Voip 정보

	// operator overloading
	_IM_PARTY_MEMBER_INFO& operator=(const IMPartyMember& rhs);
	inline _IM_PARTY_MEMBER_INFO& operator=(const CHARACTER& rhs)
	{
		this->CharacterUniqueNumber	= rhs.CharacterUniqueNumber;
		this->PilotFace				= rhs.PilotFace;
		this->UnitKind				= rhs.UnitKind;
		this->Race					= rhs.Race;
		this->Level					= rhs.Level;
		STRNCPY_MEMSET(this->CharacterName, rhs.CharacterName, SIZE_MAX_CHARACTER_NAME);
		this->MapChannelIndex = rhs.MapChannelIndex;
		return *this;
	}
} IM_PARTY_MEMBER_INFO;										// IM Server가 항상 유지해야 하는 정보

typedef struct
{
	PartyID_t				PartyID;
	IM_PARTY_MEMBER_INFO	IMPartyMemberInfo;
} MSG_IC_PARTY_ACCEPT_INVITE_OK;

typedef struct
{
	PartyID_t	PartyID;
} MSG_FC_PARTY_REJECT_INVITE;

typedef struct
{
	PartyID_t	PartyID;
	char		CharacterName[SIZE_MAX_CHARACTER_NAME];		// 초대했던 상대방
} MSG_FC_PARTY_REJECT_INVITE_OK;

// 파티원 정보(from IM Server)
typedef struct
{
	UID32_t		CharacterUniqueNumber;
} MSG_IC_PARTY_GET_MEMBER;					// 같은 파티인지는 서버에서 확인한다

typedef struct
{
	IM_PARTY_MEMBER_INFO	IMPartyMemberInfo;
} MSG_IC_PARTY_PUT_MEMBER;

typedef struct
{
	PartyID_t	PartyID;
} MSG_IC_PARTY_GET_ALL_MEMBER;

typedef struct
{
	PartyID_t	PartyID;
	UID32_t		MasterUniqueNumber;			// 파티장의 CharacterUniqueNumber
	UINT		nNumOfPartyMembers;
	ARRAY_(IM_PARTY_MEMBER_INFO);
} MSG_IC_PARTY_PUT_ALL_MEMBER;

// 파티원 정보(from Field Server)
typedef struct _FIELD_PARTY_MEMBER_INFO
{
	UID32_t			CharacterUniqueNumber;
	ClientIndex_t	ClientIndex;
	char			CharacterName[SIZE_MAX_CHARACTER_NAME];
	//char			MapName[SIZE_MAX_MAP_NAME];	// IS_VALID_MAP_NAME()이 FALSE이면, 파티 소속이나 게임중이 아닌 캐릭임(죽어서 나간 캐릭터 등)
	MAP_CHANNEL_INDEX	MapChannelIndex;	// IsValid()이 FALSE이면, 파티 소속이나 게임중이 아닌 캐릭임(죽어서 나간 캐릭터 등)
	SHORT			HP;
	float			CurrentHP;
	SHORT			DP;
	float			CurrentDP;
	SHORT			SP;
	SHORT			CurrentSP;
	SHORT			EP;
	float			CurrentEP;

	// operator overloading
	inline _FIELD_PARTY_MEMBER_INFO& operator=(const CHARACTER& rhs)
	{
		this->CharacterUniqueNumber	= rhs.CharacterUniqueNumber;
		this->ClientIndex			= rhs.ClientIndex;
		this->HP					= rhs.HP;
		this->CurrentHP				= rhs.CurrentHP;
		this->DP					= rhs.DP;
		this->CurrentDP				= rhs.CurrentDP;
		this->SP					= rhs.SP;
		this->CurrentSP				= rhs.CurrentSP;
		this->EP					= rhs.EP;
		this->CurrentEP				= rhs.CurrentEP;
		STRNCPY_MEMSET(this->CharacterName, rhs.CharacterName, SIZE_MAX_CHARACTER_NAME);
		this->MapChannelIndex		= rhs.MapChannelIndex;
		return *this;
	}
} FIELD_PARTY_MEMBER_INFO;

typedef struct
{
	UID32_t		CharacterUniqueNumber;
} MSG_FC_PARTY_GET_MEMBER;					// 같은 파티인지는 서버에서 확인한다

typedef struct
{
	FIELD_PARTY_MEMBER_INFO	FieldMemberInfo;
} MSG_FC_PARTY_PUT_MEMBER;

typedef struct
{
	PartyID_t	PartyID;
} MSG_FC_PARTY_GET_ALL_MEMBER;

typedef struct
{
	UID32_t		MasterUniqueNumber;			// 파티장의 CharacterUniqueNumber
	UINT		nNumOfPartyMembers;
	ARRAY_(MSG_FC_PARTY_PUT_MEMBER);
} MSG_FC_PARTY_PUT_ALL_MEMBER;

// 파티원 정보 업데이트
typedef struct
{
	UID32_t		CharacterUniqueNumber;
	BYTE		Level;
	SHORT		HP;
	float		CurrentHP;
	SHORT		DP;
	float		CurrentDP;
	SHORT		SP;
	SHORT		CurrentSP;
	SHORT		EP;
	float		CurrentEP;
	BodyCond_t	BodyCondition;
} MSG_FC_PARTY_UPDATE_MEMBER_INFO_ALL;

typedef struct
{
	UID32_t		CharacterUniqueNumber;
	BYTE		Level;
} MSG_FC_PARTY_UPDATE_MEMBER_INFO_LEVEL;

typedef struct
{
	UID32_t		CharacterUniqueNumber;
	SHORT		HP;
	float		CurrentHP;
} MSG_FC_PARTY_UPDATE_MEMBER_INFO_HP;

typedef struct
{
	UID32_t		CharacterUniqueNumber;
	float		CurrentHP;
} MSG_FC_PARTY_UPDATE_MEMBER_INFO_CURRENT_HP;

typedef struct
{
	UID32_t		CharacterUniqueNumber;
	SHORT		DP;
	float		CurrentDP;
} MSG_FC_PARTY_UPDATE_MEMBER_INFO_DP;

typedef struct
{
	UID32_t		CharacterUniqueNumber;
	float		CurrentDP;
} MSG_FC_PARTY_UPDATE_MEMBER_INFO_CURRENT_DP;

typedef struct
{
	UID32_t		CharacterUniqueNumber;
	SHORT		SP;
	SHORT		CurrentSP;
} MSG_FC_PARTY_UPDATE_MEMBER_INFO_SP;

typedef struct
{
	UID32_t		CharacterUniqueNumber;
	SHORT		CurrentSP;
} MSG_FC_PARTY_UPDATE_MEMBER_INFO_CURRENT_SP;

typedef struct
{
	UID32_t		CharacterUniqueNumber;
	SHORT		EP;
	float		CurrentEP;
} MSG_FC_PARTY_UPDATE_MEMBER_INFO_EP;

typedef struct
{
	UID32_t		CharacterUniqueNumber;
	float		CurrentEP;
} MSG_FC_PARTY_UPDATE_MEMBER_INFO_CURRENT_EP;

typedef struct
{
	UID32_t		CharacterUniqueNumber;
	BodyCond_t	BodyCondition;
} MSG_FC_PARTY_UPDATE_MEMBER_INFO_BODYCONDITION;

typedef struct
{
	UID32_t		CharacterUniqueNumber;
	MAP_CHANNEL_INDEX	MapChannelIndex;
} MSG_IC_PARTY_UPDATE_MEMBER_INFO_MAPNAME;		// 워프시 맵 이름 업데이트

// 추방
typedef struct
{
	PartyID_t	PartyID;
	UID32_t		CharacterUniqueNumber;
} MSG_IC_PARTY_BAN_MEMBER;

typedef struct
{
	PartyID_t	PartyID;
	UID32_t		CharacterUniqueNumber;
} MSG_IC_PARTY_BAN_MEMBER_OK;

typedef struct
{
	PartyID_t				PartyID;
	UID32_t					CharacterUniqueNumber;
	MEX_FIELD_PARTY_INFO	FieldPartyInfo;
} MSG_FI_PARTY_BAN_MEMBER_OK;

// 탈퇴
typedef struct
{
	PartyID_t	PartyID;
} MSG_IC_PARTY_LEAVE;

typedef struct
{
	PartyID_t	PartyID;
	UID32_t		CharacterUniqueNumber;
	bool		bMoveToArena;
} MSG_IC_PARTY_LEAVE_OK;

typedef struct
{
	PartyID_t				PartyID;
	UID32_t					CharacterUniqueNumber;
	MEX_FIELD_PARTY_INFO	FieldPartyInfo;
} MSG_FI_PARTY_LEAVE_OK;

// 파티장 권한 양도
typedef struct
{
	PartyID_t	PartyID;
	UID32_t		OldMasterCharacterUniqueNumber;
	UID32_t		NewMasterCharacterUniqueNumber;
} MSG_IC_PARTY_TRANSFER_MASTER;

typedef struct
{
	PartyID_t	PartyID;
	UID32_t		OldMasterCharacterUniqueNumber;
	UID32_t		NewMasterCharacterUniqueNumber;
} MSG_IC_PARTY_TRANSFER_MASTER_OK;

typedef struct
{
	PartyID_t	PartyID;
	UID32_t		OldMasterCharacterUniqueNumber;
	UID32_t		NewMasterCharacterUniqueNumber;
} MSG_FI_PARTY_TRANSFER_MASTER_OK;

// 해산
typedef struct
{
	PartyID_t	PartyID;
} MSG_IC_PARTY_DISMEMBER;

typedef struct
{
	PartyID_t	PartyID;
} MSG_IC_PARTY_DISMEMBER_OK;

typedef struct
{
	PartyID_t	PartyID;
} MSG_FI_PARTY_DISMEMBER_OK;

// 편대 비행 요청
typedef struct
{
	PartyID_t	PartyID;
	BYTE		Formation;				// 편대 비행 형태, see below
} MSG_IC_PARTY_CHANGE_FLIGHT_FORMATION;	// Cm -> I

// 2009-08-03 by cmkwon, EP3-4 편대 대형 스킬 구현 - AtumParam.h로 옮김
// #define FLIGHT_FORM_NONE				(BYTE)0	// 편대 비행 안 함
// #define FLIGHT_FORM_2_COLUMN			(BYTE)1	// 이렬 종대, 이렬 종대 모양으로 두 줄로 나란히 선 모양이다
// #define FLIGHT_FORM_2_LINE				(BYTE)2	// 이렬 횡대, 이렬 횡대 모양으로 두 줄로 나란히 선 모양이다
// #define FLIGHT_FORM_TRIANGLE			(BYTE)3	// 삼각 편대, 삼각형 모양으로 상단부터 1, 2, 3개의 유닛이 위치한다
// #define FLIGHT_FORM_INVERTED_TRIANGLE	(BYTE)4	// 역삼각 편대, 역 삼각형 모양으로 상단부터 3, 2, 1개의 유닛이 위치한다
// #define FLIGHT_FORM_BELL				(BYTE)5	// 종 형태, 종 모양으로 상단부터 1, 3, 2개의 유닛이 위치한다
// #define FLIGHT_FORM_INVERTED_BELL		(BYTE)6	// 역종 형태, 역종 모양으로 상단부터 2, 3, 1개의 유닛이 위치한다
// #define FLIGHT_FORM_X					(BYTE)7 // X자 형태
// #define FLIGHT_FORM_STAR				(BYTE)8	// 별 형태

typedef struct
{
	PartyID_t	PartyID;
	BYTE		Formation;					// 편대 비행 형태
} MSG_IC_PARTY_CHANGE_FLIGHT_FORMATION_OK;	// I -> C, 파티장에게도 보냄

typedef struct
{
	PartyID_t	PartyID;
	BYTE		Formation;					// 편대 비행 형태
} MSG_FI_PARTY_CHANGE_FLIGHT_FORMATION_OK;	// I -> C, 파티장에게도 보냄

typedef struct
{
	UID32_t		CharacterUniqueNumber;
} MSG_IC_PARTY_GET_FLIGHT_POSITION;			// C -> I -> Cm

typedef struct
{
	UID32_t		CharacterUniqueNumber;
	BYTE		FlightPosition;				// 자기 자신의 편대 비행 위치
} MSG_IC_PARTY_CHANGE_FLIGHT_POSITION;		// Cm -> I -> C

typedef struct
{
	PartyID_t	PartyID;
	UID32_t		CharacterUniqueNumber;
	BYTE		FlightPosition;				// 자기 자신의 편대 비행 위치
} MSG_FI_PARTY_CHANGE_FLIGHT_POSITION;		// Cm -> I -> C

// 2011-02-22 by shcho&hsSon, 편대버프 해제 안되는 버그 수정
#define FORMATION_SKILL_NULL	0	// 포메이션 값을 사용하지 않음
#define FORMATION_SKILL_ON		1	// 포메이션 사용
#define FORMATION_SKILL_OFF		2	// 포메이션 사용 중이 아님
// END 2011-02-22 by shcho&hsSon, 편대버프 해제 안되는 버그 수정

typedef struct
{
	// 2011-02-22 by shcho&hsSon, 편대버프 해제 안되는 버그 수정
	BOOL		Formation_On_Off;			
	// end 2011-02-22 by shcho&hsSon, 편대버프 해제 안되는 버그 수정
	UID32_t		CharacterUniqueNumber;
} MSG_IC_PARTY_CANCEL_FLIGHT_POSITION;		// C -> I -> Cm

typedef struct
{
	PartyID_t	PartyID;
	UID32_t		CharacterUniqueNumber;
} MSG_FI_PARTY_CANCEL_FLIGHT_POSITION;		// C -> I -> Cm

typedef struct
{
	UID32_t		CharacterUniqueNumber;
} MSG_IC_PARTY_MEMBER_INVALIDATED;			// I -> C, 파티원이 비정상적으로 게임에서 튕겼을 때 전송

typedef struct
{
	UID32_t				CharacterUniqueNumber;
	MAP_CHANNEL_INDEX	MapChannelIndex;
} MSG_IC_PARTY_MEMBER_REJOINED;				// I -> C, 파티원이 다시 게임을 시작하였을 때 전송, 자신은 제외함

typedef struct {
	UID32_t			CharacterUniqueNumber;	// 아이템 창작이 갱신된 파티원
	BYTE			ItemPosition;			// POS_XXX
	INT				ItemNum;
// 2009-08-26 by cmkwon, 그래픽 리소스 변경 시스템 구현 - 변경
//	INT				ColorCode;				// 2005-12-08 by cmkwon, 아머 색상 튜닝 정보
	INT				nShapeItemNum;		// 2009-08-26 by cmkwon, 그래픽 리소스 변경 시스템 구현 - 
	INT				nEffectItemNum;		// 2009-08-26 by cmkwon, 그래픽 리소스 변경 시스템 구현 - 
} MSG_IC_PARTY_UPDATE_ITEM_POS;				// I->C, 파티원이 아이템 장착을 수정했을 때 전송

typedef struct
{
	UID32_t			AllPartyMemberCharacterUniqueNumber[SIZE_MAX_PARTY_MEMBER-1];
} MSG_IC_PARTY_ALL_FLIGHT_POSITION;

struct MSG_IC_PARTY_LIST_INFO
{// 2008-06-02 by dhjin, EP3 편대 수정 - 편대 정보 리스트 
	PartyID_t		StartNum;
};

struct MSG_IC_PARTY_JOIN_FREE
{// 2008-06-03 by dhjin, EP3 편대 수정 - 편대 자유 참여
	PartyID_t		PartyNum;
	CHAR			PartyPW[SIZE_MAX_TEAM_PW];
};

struct SPARTY_LIST_INFO
{// 2008-06-02 by dhjin, EP3 편대 수정 - 편대 정보 리스트 OK
	PartyID_t	PartyNum;
	CHAR		PartyName[SIZE_MAX_PARTY_NAME];
	BYTE		ExpDistributeType;
	BYTE		ItemDistributeType;
	BOOL		PartyLock;
	CHAR		PartyPW[SIZE_MAX_TEAM_PW];
	INT			MinLevel;
	INT			MaxLevel;
	CHAR		PartyMasterName[SIZE_MAX_CHARACTER_NAME];
	BYTE		Membercount;
	BOOL		VoipCheck;
};

struct MSG_IC_PARTY_LIST_INFO_OK
{// 2008-06-02 by dhjin, EP3 편대 수정 - 편대 정보 리스트 OK
	INT				PartyInfoTotalCount;	// 2008-06-02 by dhjin, 파티 최대 목록
	INT				PartyInfoListCount;		// 2008-06-02 by dhjin,
	_ARRAY(SPARTY_LIST_INFO);
};

struct MSG_IC_PARTY_CHANGE_INFO
{// 2008-06-04 by dhjin, EP3 편대 수정 - 편대 자유 참여
	SPARTY_INFO	PartyInfo;					
};
typedef MSG_IC_PARTY_CHANGE_INFO	MSG_IC_PARTY_INFO;

struct SRECOMMENDATION_MEMBER_INFO
{
	char		CharacterName[SIZE_MAX_CHARACTER_NAME];
	UID32_t		CharacterUID;
};

struct MSG_IC_PARTY_RECOMMENDATION_MEMBER_OK
{// 2008-06-04 by dhjin, EP3 편대 수정 - 추천 케릭터 요청
	UINT		Count;
	ARRAY_(SRECOMMENDATION_MEMBER_INFO);	
};


// check: FI_EVENT_GET_WARP_INFO(파티 정보+이벤트 정보)로 대체한다. 순수 파티 정보만 받아와야할 필요성이 생기면 살린다. 20031006.
// 다른 필드 서버(파티 정보를 가지지 않은)로의 워프시, IM 서버로부터 정보를 받아 파티 정보 생성
//typedef struct
//{
//	UID32_t			CharacterUniqueNumber;
//} MSG_FI_PARTY_GET_PARTY_INFO;
//
//typedef struct
//{
//	UID32_t			AccountUniqueNumber;		// 다른 필드 서버로의 워프시, 인증에 필요함
//	UID32_t			CharacterUniqueNumber;
//	PartyID_t		PartyID;
//	UID32_t			MasterCharacterUniqueNumber;
//	int				NumOfPartyMembers;
//	ARRAY_(FI_PARTY_MEMBER_INFO);
//} MSG_FI_PARTY_GET_PARTY_INFO_OK;

typedef struct
{
	UID32_t			CharacterUniqueNumber;
//	char			CurrentMapName[SIZE_MAX_MAP_NAME];	// check: 다른 필드서버에 있는 파티원은 관리하지 않기로 하면서 제거, 20031010, 확정&구현되면 삭제 가능
} FI_PARTY_MEMBER_INFO;	// Field Server와 IM Server 사이에 사용하는 파티원 정보

// 편대 비행중인 파티원 워프
typedef struct
{
	int				nPartyMembers;				// 같이 워프할 파티원의 수, 편대장 제외
	ARRAY_(UID32_t);							// 파티원 character uniquenumber의 array
} MSG_FC_PARTY_REQUEST_PARTY_WARP;

typedef struct
{
	MAP_CHANNEL_INDEX	MapChannelIndex;
	int					nPartyMembers;				// 같이 워프할 파티원의 수, 편대장 제외
	ARRAY_(UID32_t);								// 파티원 character uniquenumber의 array
} MSG_FC_PARTY_REQUEST_PARTY_WARP_WITH_MAP_NAME;

//typedef struct
//{
//	INT				ObjectIndex;				// Object의 Index
//	int				nPartyMembers;				// 같이 워프할 파티원의 수, 편대장 제외
//	ARRAY_(UID32_t);							// 파티원 character uniquenumber의 array
//} MSG_FC_PARTY_REQUEST_PARTY_OBJECT_EVENT;
typedef struct
{
	DWORD			ObjectType;
	AVECTOR3		ObjectPosition;
	int				nPartyMembers;				// 같이 워프할 파티원의 수, 편대장 제외
	ARRAY_(UID32_t);							// 파티원 character uniquenumber의 array
} MSG_FC_PARTY_REQUEST_PARTY_OBJECT_EVENT;

// 파티원의 MOVE 정보 요청
typedef struct
{
	UID32_t			OtherCharacterUniqueNumber;
} MSG_FC_PARTY_GET_OTHER_MOVE;

// 게임에서 나갔다가 들어왔을 때 편대 유지 관련
typedef struct
{
	PartyID_t	PartyID;				// 최근에 몸담았던 파티가 로긴하였을 때 존재하므로, 그에 대한 준비 요청
} MSG_IC_PARTY_PUT_LAST_PARTY_INFO;		// I -> C, 파티원이 다시 게임을 시작하였을 때 전송, 자기 자신에게만 보냄

// 파티전 관련
typedef struct
{
	PartyID_t		PeerPartyID;				// 상대 PartyID
	ClientIndex_t	PeerPartyMasterClientIndex;	// 파티장의 ClientIndex
	SHORT			nPeerPartyMemberToBattle;	// 파티전에 참여할 상대 파티원의 수
	ARRAY_(PEER_PARTY_MEMBER);
} MSG_FC_PARTY_BATTLE_START;					// F->C, 상대 파티의 정보를 보낸다.

struct PEER_PARTY_MEMBER
{
	ClientIndex_t	ClientIndex;
	UID32_t			CharacterUniqueNumber;
};

typedef struct
{
	PartyID_t		PeerPartyID;				// 상대방의 ClientIndex
	USHORT			EndType;					// 결투 종료 타입, BATTLE_END_XXX
} MSG_FC_PARTY_BATTLE_END;						// F->C, 파티전 결과

typedef struct
{
	PartyID_t		PartyID1;					// 파티 1
	PartyID_t		PeerPartyID1;				// 파티 1의 대상 파티
	PartyID_t		PartyID2;					// 파티 2
	PartyID_t		PeerPartyID2;				// 파티 2의 대상 파티
} MSG_FI_PARTY_NOTIFY_BATTLE_PARTY;				// F->I, 파티전을 알림

typedef struct
{
	PartyID_t		PartyID1;					// 파티 1
	PartyID_t		PeerPartyID1;				// 파티 1의 대상 파티
	PartyID_t		PartyID2;					// 파티 2
	PartyID_t		PeerPartyID2;				// 파티 2의 대상 파티
} MSG_FI_PARTY_NOTIFY_BATTLE_PARTY_OK;			// I->F, 파티전을 알림에 대한 ACK

typedef struct  {
	INT				ItemNum;					// 아이템의 종류
	USHORT			Amount;						// 아이템의 개수
	ClientIndex_t	ClientIndex;
} MSG_FC_PARTY_PUT_ITEM_OTHER;					// F->C, 다른 파티원의 아이템 취득 정보 전송

typedef struct {
	PartyID_t				PartyID;
	UID32_t					CharacterUniqueNumber;	// 추가할 파티원
	MEX_FIELD_PARTY_INFO	FieldPartyInfo;
} MSG_FI_PARTY_ADD_MEMBER;						// I->F, 파티원을 추가하라고 Field Server 알림

typedef struct {
	PartyID_t				PartyID;
	UID32_t					CharacterUniqueNumber;	// 제거할 파티원
	MEX_FIELD_PARTY_INFO	FieldPartyInfo;
} MSG_FI_PARTY_DELETE_MEMBER;					// I->F, 파티원을 제거하라고 Field Server 알림

typedef struct {
	UID32_t			CharacterUniqueNumber;	// 아이템 창작이 갱신된 파티원
	BYTE			ItemPosition;			// POS_XXX
	INT				ItemNum;
// 2009-08-26 by cmkwon, 그래픽 리소스 변경 시스템 구현 - 변경
//	INT				ColorCode;				// 2005-12-08 by cmkwon, 아머 색상 튜닝 정보
	INT				nShapeItemNum;		// 2009-08-26 by cmkwon, 그래픽 리소스 변경 시스템 구현 - 
	INT				nEffectItemNum;		// 2009-08-26 by cmkwon, 그래픽 리소스 변경 시스템 구현 - 
} MSG_FI_PARTY_UPDATE_ITEM_POS;				// F->I, 파티원이 아이템 장착을 수정했을 때 전송


typedef struct
{
	PartyID_t		PartyID;
	UID32_t			AllPartyMemberCharacterUniqueNumber[SIZE_MAX_PARTY_MEMBER-1];
} MSG_FI_PARTY_ALL_FLIGHT_POSITION;

typedef struct
{
	PartyID_t				PartyID;
	MEX_FIELD_PARTY_INFO	FieldPartyInfo;
} MSG_FI_PARTY_UPDATE_PARTY_INFO;	// I->F, 파티 정보를 업데이트

struct MSG_FI_PARTY_CHANGE_EXP_DISTRIBUTE_TYPE
{// 2008-06-04 by dhjin, EP3 편대 수정 - 경험치 분배 방식 변경 
	PartyID_t	PartyID;
	BYTE		ExpDistributeType;
};

struct MSG_FI_PARTY_CHANGE_ITEM_DISTRIBUTE_TYPE
{// 2008-06-04 by dhjin, EP3 편대 수정 - 아이템 분배 방식 변경 
	PartyID_t	PartyID;
	BYTE		ItemDistributeType;
};

struct MSG_FI_PARTY_CHANGE_FORMATION_SKILL		// 2009-08-03 by cmkwon, EP3-4 편대 대형 스킬 구현 - 
{
	UID32_t		PartyMasterCharcUID;	// PartyMaster CharacterUID
	BYTE		Is_On_or_Off;			// TRUE is ON, FALSE is Off
	BYTE		FlightFormation;		// 
};

///////////////////////////////////////////////////////////////////////////////
// FI_CHARACTER
typedef struct
{
	UID32_t	AccountUniqueNumber;
	UID32_t	CharacterUniqueNumber;
	UID32_t	GuildUniqueNumber;
} MSG_FI_CHARACTER_DELETE_CHARACTER;

typedef struct
{
	UID32_t	CharacterUniqueNumber;
	BYTE	Level;
} MSG_FI_CHARACTER_CHANGE_LEVEL;

typedef struct
{
	UID32_t	CharacterUniqueNumber;
	char	GuildName[SIZE_MAX_GUILD_NAME];	// 길드 이름
	UID32_t	GuildUniqueNumber;				// 길드 번호, 0이면 길드 없음
	BOOL	GuildDelete;					// 2006-09-29 by dhjin, 길드 삭제 정보, 1->삭제
} MSG_FI_CHARACTER_UPDATE_GUILD_INFO;


// 2007-10-06 by dhjin, 부지도자가 여단 생성시 InflWarManager 클래스에 부지도자 여단을 설정.
struct MSG_FI_CREATE_GUILD_BY_SUBLEADER
{
	byte	Influence;
	int		SubLeaderRank;
	UID32_t CharacterUID;
};

typedef struct
{
	UID32_t				CharacterUniqueNumber;
	MAP_CHANNEL_INDEX	MapChannel;
} MSG_FI_CHARACTER_UPDATE_MAP_CHANNEL;

struct MSG_FI_CHARACTER_CHANGE_INFLUENCE_TYPE
{
	UID32_t				CharacterUID;
	BYTE				InfluenceType0;					// 
};

struct MSG_FI_UPDATE_SUBLEADER
{// 2007-02-14 by dhjin, 부지도자 설정 시 IM서버로 정보 전송.
	UID32_t				CharacterUID;
	BYTE				SubLeaderNum;
	BYTE				InfluenceType;
};

struct MSG_FI_MULTICHAT_STEERING
{
	BOOL				nEnableChats;
};
///////////////////////////////////////////////////////////////////////////////
// FC_GUILD (Cleint <-> Field Server)
typedef struct
{
	MapIndex_t	MapIndex;
} MSG_FC_GUILD_GET_MAP_OWNER_INFO;			// C->F, 맵 소유 정보 요청

typedef struct
{
	char	DefenderGuildName[SIZE_MAX_GUILD_NAME];	// 점령 길드 이름
	BYTE	NumOfCallengerGuilds;					// 도전 길드 수
} MSG_FC_GUILD_GET_MAP_OWNER_INFO_OK;		// F->C, 맵 소유 정보 요청 결과

typedef struct
{
	char	GuildName[SIZE_MAX_GUILD_NAME];
} MSG_FC_GUILD_REQUEST_GUILD_WAR;			// C->F, 여단전 요청

typedef struct
{
	INT		Order;							// 순번(1부터 시작), 0 이하이면 신청 실패
} MSG_FC_GUILD_REQUEST_GUILD_WAR_RESULT;	// F->C, 여단전 요청 결과

typedef struct
{
	UID32_t	DefenderGuildUniqueNumber;		// 점령 길드 고유 번호
} MSG_FC_GUILD_GET_CHALLENGER_GUILD;		// C->F, 여단전 요청 대기 길드 리스트 요청

typedef struct
{
	BYTE	NumOfCallengerGuilds;			// 도전 길드 이름 개수(우선 순위 순서)
	ARRAY_(char[SIZE_MAX_GUILD_NAME]);
} MSG_FC_GUILD_GET_CHALLENGER_GUILD_OK;		// F->C, 여단전 요청 대기 길드 리스트 요청 결과

typedef struct
{
	UID32_t		GuildUID;				// 자신이 속한 길드의 UID
} MSG_FC_GUILD_GET_WAR_INFO;			// C->F, 여단전 정보를 요청

typedef struct
{
	MAP_CHANNEL_INDEX	MapChannel;		// 여단전이 일어나는 맵
	UID32_t				PeerGuildUID;	// 상대 길드의 길드 번호
} MSG_FC_GUILD_GET_WAR_INFO_OK;			// F->C, 여단전 정보를 전송

struct MSG_FC_GUILD_SUMMON_MEMBER
{
	UID32_t				uidGuildUID;	// 길드 UID
	MAP_CHANNEL_INDEX	MapChannel;		// 여단장이 있는 맵
	AVECTOR3			PositionVector;	// 여단장의 좌표
};
struct MSG_FC_GUILD_SUMMON_MEMBER_OK
{
	UID32_t				uidGuildUID;	// 길드 UID
	MAP_CHANNEL_INDEX	MapChannel;		// 여단장이 있는 맵
	AVECTOR3			PositionVector;	// 여단장의 좌표
};

///////////////////////////////////////////////////////////////////////////////
// FI_GUILD (Field Server <-> IM Server)
typedef struct
{
	UID32_t		DefenderGuildUID;
	UID32_t		ChallengerGuildUID;
	MAP_CHANNEL_INDEX	WarMapChannel;
} MSG_FI_GUILD_NOTIFY_START_WAR;	// F->I

typedef struct
{
	USHORT		WarEndType;			// BATTLE_END_XXX, BATTLE_END_WIN or BATTLE_END_DEFEAT이면 승패 있음
	UID32_t		WinnerGuildUID;
	UID32_t		LoserGuildUID;
	MAP_CHANNEL_INDEX	WarMapChannel;
} MSG_FI_GUILD_NOTIFY_END_WAR;		// F->I

typedef struct
{
	UID32_t		DeleteGuildUID;
} MSG_FI_GUILD_DELETE_GUILD;
typedef		MSG_FI_GUILD_DELETE_GUILD	MSG_FI_GUILD_REG_DELETE_GUILD;		// 2007-11-09 by dhjin

struct MSG_FI_GUILD_ADD_GUILD_FAME	// 2005-12-27 by cmkwon
{
	UID32_t		guildUID;		// 
	int			addValues;		// 1 or 100
};

struct MSG_FI_GUILD_OUTPOST
{// 2008-05-21 by dhjin, EP3 - 여단 수정 사항 - 전진기지 관련
	UID32_t		GuildUID;
	MapIndex_t	MapIndex;
};

///////////////////////////////////////////////////////////////////////////////
// IC_GUILD (Cleint <-> IM Server)
typedef struct
{
	char	GuildName[SIZE_MAX_GUILD_NAME];
} MSG_IC_GUILD_CREATE;						// C->I, 길드 생성 요청

typedef struct
{
	char	GuildName[SIZE_MAX_GUILD_NAME];
	UID32_t	GuildUniqueNumber;				// 길드 고유번호
	char	GuildCommanderUniqueNumber;		// 여단장
} MSG_IC_GUILD_CREATE_OK;					// I->C, 길드 생성 결과

typedef struct
{
	UID32_t	GuildUniqueNumber;				// 길드 고유번호
} MSG_IC_GUILD_GET_GUILD_INFO;				// C->I, 길드 정보 요청

// GUILD_STATE_XXX
#define GUILD_STATE_NORMAL				((BYTE)0)	// 정상 길드 상태
#define GUILD_STATE_DISMEMBER_READY		((BYTE)1)	// 길드 해체 대기 상태
#define GUILD_STATE_IN_GUILD_WAR		((BYTE)2)	// 길드전 상태
#define GUILD_STATE_CITYWAR				((BYTE)3)	// 도시점령전 상태

///////////////////////////////////////////////////////////////////////////////
// 2007-08-02 by cmkwon, 여단 마크 심사 시스템 구현 - GUILD_MARK_STATE_XXX 정의
#define GUILD_MARK_STATE_NONE					((BYTE)0)	// 마크 없는 상태
#define GUILD_MARK_STATE_WAITING_PERMISSION		((BYTE)1)	// 마크 허용을 기다리는 상태, 게임상에서는 사용 불가
#define GUILD_MARK_STATE_NORMAL					((BYTE)2)	// 마크 사용 중인 상태
char *GetStringGuildMarkState(BYTE i_byGuildMarkState, BOOL i_bForUser=FALSE);


typedef struct
{
	char	GuildName[SIZE_MAX_GUILD_NAME];
	UID32_t	GuildUniqueNumber;				// 길드 고유번호
	UID32_t	GuildCommanderUniqueNumber;		// 여단장
	INT		GuildMemberCapacity;			// 길드 인원 제한
	BYTE	NumOfGuildMemberInfo;
	BYTE	GuildState;						// GUILD_STATE_XXX
	UINT	GuildMarkVersion;
	INT		WarWinPoint;					// 길드전 승수
	INT		WarLossPoint;					// 길드전 패수
	char    Notice[SIZE_MAX_NOTICE];		// 2008-06-05 by dhjin, EP3 - 여단 수정 사항
	MapIndex_t GuildOutPostCityMapIndex;	// 2008-06-05 by dhjin, EP3 - 여단 수정 사항
	INT		GuildTotalFame;					// 2008-06-05 by dhjin, EP3 - 여단 수정 사항			
	INT		GuildMonthlyFame;				// 2008-06-05 by dhjin, EP3 - 여단 수정 사항
	INT		GuildTotalFameRank;				// 2008-06-05 by dhjin, EP3 - 여단 수정 사항	
	INT		GuildMonthlyFameRank;			// 2008-06-05 by dhjin, EP3 - 여단 수정 사항
	BOOL    GuildMemberShip;					// 2008-06-20 by dhjin, EP3 - 여단 수정 사항
	ATUM_DATE_TIME GuildMemberShipExpireTime;	// 2008-06-20 by dhjin, EP3 - 여단 수정 사항
	ARRAY_(MEX_GUILD_MEMBER_INFO);
} MSG_IC_GUILD_GET_GUILD_INFO_OK;			// I->C, 길드 정보 결과

// 길드원 계급, GUILD_RANK_XXX, BYTE
#define GUILD_RANK_PRIVATE_NULL			((BYTE)0)	// 무소속 대대원
#define GUILD_RANK_COMMANDER			((BYTE)1)	// 비행여단장, 길드장
#define GUILD_RANK_SUBCOMMANDER			((BYTE)2)	// 부여단장				// 2008-05-20 by dhjin, EP3 - 여단 수정 사항
#define GUILD_RANK_SQUAD_LEADER_1		((BYTE)3)	// 제 1 비행대대장
#define GUILD_RANK_PRIVATE_1			((BYTE)4)	// 제 1 비행대대원
#define GUILD_RANK_SQUAD_LEADER_2		((BYTE)5)	// 제 2 비행대대장
#define GUILD_RANK_PRIVATE_2			((BYTE)6)	// 제 2 비행대대원
#define GUILD_RANK_SQUAD_LEADER_3		((BYTE)7)	// 제 3 비행대대장
#define GUILD_RANK_PRIVATE_3			((BYTE)8)	// 제 3 비행대대원
#define GUILD_RANK_SQUAD_LEADER_4		((BYTE)9)	// 제 4 비행대대장
#define GUILD_RANK_PRIVATE_4			((BYTE)10)	// 제 4 비행대대원
#define GUILD_RANK_SQUAD_LEADER_5		((BYTE)11)	// 제 5 비행대대장
#define GUILD_RANK_PRIVATE_5			((BYTE)12)	// 제 5 비행대대원

// check: MSG_IC_GUILD_GET_GUILD_INFO로 통합, 필요하면 살림, 20040520, kelovon
//typedef struct
//{
//	UID32_t	GuildUniqueNumber;		// 길드 고유번호
//} MSG_IC_GUILD_GET_MEMBER;		// C->I, 길드원 정보 요청

class CGuildMember;

struct MEX_GUILD_MEMBER_INFO
{
	char	MemberName[SIZE_MAX_CHARACTER_NAME];
	UID32_t	MemberUniqueNumber;
	BYTE	GuildRank;				// GUILD_RANK_XXX
	BYTE	IsOnline;				// 1: TRUE, 0: FALSE
	INT		UnitKind;				// 2008-05-20 by dhjin, EP3 - 여단 수정 사항
	BYTE	MemberLevel;			// 2008-05-20 by dhjin, EP3 - 여단 수정 사항	
	EN_CHECK_TYPE	VoipType;		// 2008-07-16 by dhjin, EP3 - Voip 정보

#ifdef _ATUM_IM_SERVER
	// operator overloading
	MEX_GUILD_MEMBER_INFO& operator=(const CGuildMember& rhs);
#endif // _ATUM_IM_SERVER
};

// check: MSG_IC_GUILD_GET_GUILD_INFO_OK로 통합, 필요하면 살림, 20040520, kelovon
//typedef struct
//{
//	UID32_t	GuildUniqueNumber;		// 길드 고유번호
//	BYTE	NumOfGuildMemberInfo;
//	ARRAY_(MEX_GUILD_MEMBER_INFO);
//} MSG_IC_GUILD_GET_MEMBER_OK;		// I->C, 길드원 정보 결과

class CGuild;

struct MEX_OTHER_GUILD_INFO
{
	char	GuildName[SIZE_MAX_GUILD_NAME];
	UID32_t	GuildUniqueNumber;
	UINT	GuildMarkVersion;

#ifdef _ATUM_IM_SERVER
	// operator overloading
	MEX_OTHER_GUILD_INFO& operator=(const CGuild& rhs);
#endif // _ATUM_IM_SERVER
};

typedef struct
{
	UID32_t	GuildUniqueNumber;
} MSG_IC_GUILD_GET_OTHER_GUILD_INFO;

typedef struct
{
	MEX_OTHER_GUILD_INFO		OtherGuildMemberInfo;
} MSG_IC_GUILD_GET_OTHER_GUILD_INFO_OK;

typedef struct
{
	char	InviteeCharacterName[SIZE_MAX_CHARACTER_NAME];		// 초대할 상대방 아이디
} MSG_IC_GUILD_REQUEST_INVITE;		// C->I, 가입 권유, 서버에 요청

typedef struct
{
	UID32_t	GuildUniqueNumber;										// 길드 고유 번호
	char	GuildName[SIZE_MAX_GUILD_NAME];							// 길드 이름
	char	GuildCommanderCharacterName[SIZE_MAX_CHARACTER_NAME];	// 길드장 아이디
} MSG_IC_GUILD_REQUEST_INVITE_QUESTION;		// I->C, 가입 권유, 대상자에게 전송

typedef struct
{
	UID32_t	GuildUniqueNumber;		// 길드 고유 번호, MSG_IC_GUILD_REQUEST_INVITE_QUESTION의 값 
} MSG_IC_GUILD_ACCEPT_INVITE;		// C->I, 가입 승낙, 서버에 요청

typedef struct
{
	MEX_GUILD_MEMBER_INFO	MemberInfo;		// 새 길드원의 정보
} MSG_IC_GUILD_ACCEPT_INVITE_OK;			// I->C, 가입 승낙, 대상자 및 길드원에게 전송

typedef struct
{
	UID32_t	GuildUniqueNumber;		// 길드 고유 번호, MSG_IC_GUILD_REQUEST_INVITE_QUESTION의 값
} MSG_IC_GUILD_REJECT_INVITE;		// C->I, 가입 거부, 서버에 요청

typedef struct
{
	char	CharacterName[SIZE_MAX_CHARACTER_NAME];	// 가입 거부한 character의 이름
} MSG_IC_GUILD_REJECT_INVITE_OK;	// I->C, 가입 거부, 대상자에게 전송

typedef struct
{
	UID32_t	MemberUniqueNumber;
} MSG_IC_GUILD_BAN_MEMBER;			// C->I, 길드원 추방

typedef struct
{
	UID32_t	MemberUniqueNumber;
} MSG_IC_GUILD_BAN_MEMBER_OK;		// I->C, 길드원 추방 결과

typedef struct
{
	UID32_t	GuildUniqueNumber;
} MSG_IC_GUILD_LEAVE;				// C->I, 여단 탈퇴

typedef struct
{
	UID32_t	MemberUniqueNumber;
	DWORD	Padding;				// 2011-07-21 by hskim, 인증 서버 구현 - 기존 서버와 호환 안되도록 구조체 크기 바꿈
} MSG_IC_GUILD_LEAVE_OK;			// I->C, 여단 탈퇴 결과

typedef struct
{
	UID32_t	GuildUniqueNumber;
} MSG_IC_GUILD_DISMEMBER;			// C->I, 여단 해체
typedef MSG_IC_GUILD_DISMEMBER		MSG_FC_GUILD_DISMEMBER;
typedef MSG_IC_GUILD_DISMEMBER		MSG_FI_GUILD_DISMEMBER;


typedef struct
{
	char	GuildName[SIZE_MAX_GUILD_NAME];
} MSG_IC_GUILD_DISMEMBER_OK;		// I->C, 여단 해체 결과

// GUILD_MEMBER_STATE_XXX
#define GUILD_MEMBER_STATE_OFFLINE		((BYTE)0)
#define GUILD_MEMBER_STATE_ONLINE		((BYTE)1)

typedef struct
{
	UID32_t	MemberUniqueNumber;
	BYTE	GuildMemberState;		// GUILD_MEMBER_STATE_XXX
} MSG_IC_GUILD_SET_MEMBER_STATE;	// I->C, 길드원의 상태 변화(ONLINE, OFFLINE 등)

typedef struct
{
	UID32_t	GuildUniqueNumber;
} MSG_IC_GUILD_CANCEL_DISMEMBER;		// C->I, 여단 해체 취소 요청

typedef struct
{
	BYTE	GuildState;					// GUILD_STATE_XXX
} MSG_IC_GUILD_CANCEL_DISMEMBER_OK;		// I->C, 여단 해체 취소 결과

typedef struct
{
	UID32_t	GuildUniqueNumber;
} MSG_IC_GUILD_GET_DISMEMBER_DATE;		// C->I, 길드 해산 시간 요청

typedef struct
{
	ATUM_DATE_TIME	DismemberDateTime;
} MSG_IC_GUILD_GET_DISMEMBER_DATE_OK;	// C->I, 길드 해산 시간 결과

typedef struct
{
	char	NewGuildName[SIZE_MAX_GUILD_NAME];
} MSG_IC_GUILD_CHANGE_GUILD_NAME;		// C->I, 여단 이름 변경 요청

typedef struct
{
	char	NewGuildName[SIZE_MAX_GUILD_NAME];
} MSG_IC_GUILD_CHANGE_GUILD_NAME_OK;	// I->C, 여단 이름 변경 결과

typedef struct
{
	UID32_t	GuildUniqueNumber;
} MSG_IC_GUILD_GET_GUILD_MARK;			// C->I, 여단 문양 요청

// MSG_SEQUENCE_XXX
#define MSG_SEQUENCE_SINGLE		((BYTE)0)	// 단독 MSG
#define MSG_SEQUENCE_BEGIN		((BYTE)1)	// LONG MSG의 시작
#define MSG_SEQUENCE_MIDDLE		((BYTE)2)	// LONG MSG의 중간
#define MSG_SEQUENCE_END		((BYTE)3)	// LONG MSG의 끝

typedef struct
{
	UID32_t	GuildUniqueNumber;
	UINT	GuildMarkVersion;
	UINT	SizeOfGuildMark;			// 길드 문양의 size <= SIZE_MAX_GUILD_MARK_IMAGE
	ARRAY_(char);						// 길드 문양, image(bmp, gif, jpg, ...)
} MSG_IC_GUILD_GET_GUILD_MARK_OK;		// I->C, 여단 문양 결과

typedef struct
{
	UINT	SizeOfGuildMark;			// 길드 문양의 size <= SIZE_MAX_GUILD_MARK_IMAGE
	ARRAY_(char);						// 길드 문양, image(bmp, gif, jpg, ...)
} MSG_IC_GUILD_SET_GUILD_MARK;			// C->I, 여단 문양 성정 요청

typedef struct
{
	UINT	GuildMarkVersion;
	UINT	SizeOfGuildMark;			// 길드 문양의 size <= SIZE_MAX_GUILD_MARK_IMAGE
	ARRAY_(char);						// 길드 문양, image(bmp, gif, jpg, ...)
} MSG_IC_GUILD_SET_GUILD_MARK_OK;		// I->C, 여단 문양 성정 결과

typedef struct
{
	UID32_t	MemberUniqueNumber;
	BYTE	GuildRank;					// GUILD_RANK_XXX
} MSG_IC_GUILD_SET_RANK;				// C->I, 계급 설정

typedef struct
{
	UID32_t	MemberUniqueNumber;
	BYTE	GuildRank;					// GUILD_RANK_XXX
} MSG_IC_GUILD_SET_RANK_OK;				// I->C, 계급 설정 결과

typedef struct
{
	BYTE	GuildState;					// GUILD_STATE_XXX
} MSG_IC_GUILD_CHANGE_GUILD_STATE;		// I->C, 여단 상태 전송

typedef struct
{
	UID32_t	GuildUniqueNumber;
} MSG_IC_GUILD_LOADING_GUILD_DONE;		// I->C, 서버측에서 길드 정보 로딩 완료 알림

typedef struct
{
	UID32_t		PeerGuildUID;			// 상대 길드
	INT			WaitingTime;			// 대기 시간, 여기 명시된 시간 후에 여단전 시작
} MSG_IC_GUILD_WAR_READY;				// I->C, 여단전 시작 대기 상태를 알림

typedef struct
{
	UID32_t		PeerGuildUID;			// 상대 길드
} MSG_IC_GUILD_START_WAR;				// I->C, 여단전 시작을 알림

typedef struct
{
	USHORT		WarEndType;				// 결과, BATTLE_END_XXX
	char		PeerGuildName[SIZE_MAX_GUILD_NAME];	// 상대 길드 이름
} MSG_IC_GUILD_END_WAR;					// I->C, 여단전 종료를 알림

typedef struct
{
	INT			WarWinPoint;			// 길드전 승수
	INT			WarLossPoint;			// 길드전 패수
} MSG_IC_GUILD_UPDATE_WAR_POINT;

struct MSG_IC_GUILD_CHANGE_MEMBER_CAPACITY
{
	UID32_t		guildUID;				// 길드 UID
	INT			nMemberCapacity;		// 여단 제한 인원
	INT			IncreaseCapacity;		// 2008-05-28 by dhjin, EP3 여단 수정 사항 - 여단원 증가 캐쉬 아이템
};

struct MSG_IC_GUILD_GET_GUILD_MEMBER_LIST_OK
{
	UID32_t	GuildUniqueNumber;				// 길드 고유번호
	BYTE	NumOfGuildMemberInfo;
	ARRAY_(MEX_GUILD_MEMBER_INFO);
};

struct MSG_IC_GUILD_END_WAR_ADMIN_NOTIFY		// 2006-08-09 by cmkwon
{
	USHORT		WarEndType;								// 결과, BATTLE_END_XXX
	char		WinerGuildName[SIZE_MAX_GUILD_NAME];	// 승리 길드 이름
	char		LoserGuildName[SIZE_MAX_GUILD_NAME];	// 패배 길드 이름
};

struct MSG_IC_GUILD_MEMBER_LEVEL_UP
{// 2008-05-20 by dhjin, EP3 - 여단 수정 사항 - 여단원 레벨업 관련
	UID32_t		CharacterUID;
	INT			Level;
};

struct MSG_IC_GUILD_NEW_COMMANDER
{// 2008-05-20 by dhjin, EP3 - 여단 수정 사항 - 여단장 위임
	UID32_t		NewCommanderUID;
};

struct MSG_IC_GUILD_NOTICE_WRITE
{// 2008-05-20 by dhjin, EP3 - 여단 수정 사항 - 여단 공지
	char		Notice[SIZE_MAX_NOTICE];
};
typedef MSG_IC_GUILD_NOTICE_WRITE   MSG_IC_GUILD_NOTICE_WRITE_OK;

struct MSG_IC_GUILD_GET_INTRODUCTION_OK
{// 2008-05-27 by dhjin, EP3 - 여단 수정 사항 - 여단 지원자 관리
	char		GuildIntroduction[SIZE_MAX_NOTICE];
};

struct MSG_IC_GUILD_GET_APPLICANT_OK
{// 2008-05-27 by dhjin, EP3 - 여단 수정 사항 - 여단 지원자 관리
	UID32_t		CharacterUID;
	char		CharacterName[SIZE_MAX_CHARACTER_NAME];
	BYTE		UnitKind;
	INT			Level;
};

struct MSG_IC_GUILD_GET_SELF_INTRODUCTION
{// 2008-05-27 by dhjin, EP3 - 여단 수정 사항 - 여단 지원자 소개서 
	UID32_t		CharacterUID;
};

struct MSG_IC_GUILD_GET_SELF_INTRODUCTION_OK
{// 2008-05-27 by dhjin, EP3 - 여단 수정 사항 - 여단 지원자 소개서 
	char		GuildName[SIZE_MAX_GUILD_NAME];
	char		SelfIntroduction[SIZE_MAX_NOTICE];
};

struct MSG_IC_GUILD_SEARCH_INTRODUCTION_OK
{// 2008-05-27 by dhjin, EP3 - 여단 수정 사항 - 여단 소개 검색
	UID32_t			GuildUID;
	char			GuildName[SIZE_MAX_GUILD_NAME];
	ATUM_DATE_TIME	WriteDate;
	char			GuildIntroduction[SIZE_MAX_NOTICE];
	char			CharacterName[SIZE_MAX_CHARACTER_NAME];
};

struct MSG_IC_GUILD_UPDATE_INTRODUCTION
{// 2008-05-28 by dhjin, EP3 - 여단 수정 사항 - 여단 소개 작성 
	char			GuildIntroduction[SIZE_MAX_NOTICE];
};

struct MSG_IC_GUILD_UPDATE_SELFINTRODUCTION
{// 2008-05-28 by dhjin, EP3 - 여단 수정 사항 - 자기 소개 작성
	UID32_t			TargetGuildUID;
	char			SelfIntroduction[SIZE_MAX_NOTICE];	
};

struct MSG_IC_GUILD_CHANGE_FAME_RANK
{// 2008-06-10 by dhjin, EP3 - 여단 수정 사항 - 여단 명성 변경
	INT		GuildTotalFameRank;				
	INT		GuildMonthlyFameRank;			
};

struct MSG_IC_GUILD_APPLICANT_INVITE
{// 2008-06-12 by dhjin, EP3 - 여단 수정 사항 - 지원자 관리에서 여단원 초대
	UID32_t		CharacterUID;
	char		CharacterName[SIZE_MAX_CHARACTER_NAME];
	BYTE		UnitKind;
	INT			Level;
};

struct MSG_IC_GUILD_APPLICANT_REJECT_INVITE
{// 2008-06-12 by dhjin, EP3 - 여단 수정 사항 - 지원자 관리에서 여단원 초대 거부 
	UID32_t		CharacterUID;
	char		GuildName[SIZE_MAX_GUILD_NAME];
};

struct MSG_IC_GUILD_CHANGE_MEMBERSHIP
{// I->C, // 2008-06-20 by dhjin, EP3 - 여단 수정 사항 - 여단장 맴버쉽 정보 전송
	BOOL			MemberShip;
	ATUM_DATE_TIME	MemberShipExpireTime;
};

///////////////////////////////////////////////////////////////////////////////
// FC_QUEST
struct MEX_QUEST_INFO						// Character의 퀘스트 정보
{
	INT				QuestIndex;				// 퀘스트 번호
	BYTE			QuestState;				// 완료, 진행중
	LONGLONG		QuestPlayTimeStamp;		// 퀘스트 시작 시의 TotalPlayTime
	ATUM_DATE_TIME		QuestTimeStamp;		// quest started time
	char			szCityWarServerGroupName[SIZE_MAX_SERVER_NAME];
};

struct MEX_QUEST_MONSTER_COUNT						// Character의 퀘스트 정보
{
	INT				QuestIndex;				// 퀘스트 번호
	INT				MonsterUniqueNumber;
	INT				Count;
};

typedef struct
{
	INT			QuestIndex;
	BYTE		QuestStartType;			// QUEST_START_TYPE_XXX
	BYTE		PassQuest;				// 퀘스트를 그냥 넘김, CQuest::IsDiscardable이 TRUE일 때만 처리함
	AVECTOR3	AVec3QuestPosition;		// 퀘스트 시작시 좌표, 2005-08-29 by cmkwon
	char		szCouponNumber[SIZE_MAX_COUPON_NUMBER];
} MSG_FC_QUEST_REQUEST_START;		// C->F, Quest 시작을 요청

typedef struct
{
	MEX_QUEST_INFO	MexQuestInfo;	// 퀘스트 정보
	float			fVCNInflDistributionPercent;	// 2006-02-08 by cmkwon, 바이제니유 정규군 세력분포 - 세력선택 미션시만 사용됨
	float			fANIInflDistributionPercent;	// 2006-02-08 by cmkwon, 알링턴 정규군 - 세력선택 미션시만 사용됨
} MSG_FC_QUEST_REQUEST_START_RESULT;	// F->C, Quest 시작을 승인, Client는 이 MSG를 받으면 Pre NPCTalk을 로딩한다

typedef struct
{
	INT		QuestIndex;
} MSG_FC_QUEST_ACCEPT_QUEST;	// C->F, Quest를 받아들임

typedef struct
{
	INT		QuestIndex;
} MSG_FC_QUEST_CANCEL_QUEST;	// C->F, Quest를 거절함(거절할 수 없는 Quest도 존재함)

// check: no body, 필요하면 살림
//typedef struct {
//} MSG_FC_QUEST_LEAVE_NPC;		// C->F, NPC를 떠남
//
//typedef struct {
//} MSG_FC_QUEST_LEAVE_NPC_OK;	// F->C, NPC를 떠남에 대한 ACK

typedef struct
{
	INT			QuestIndex;
	INT			QuestResult;			// 퀘스트의 성적 혹은 점수, 0이면 무시, 10~13(A~D)이면 넘겨주기, check: 현재는 받은대로 넘겨주기, 20040224, kelovon
	INT			SpentTimeInSeconds;		// 퀘스트 시작 후 지난 시간, 20041129, kelovon, 서버에서는 클라이언트 시간을 전적으로 믿음
	AVECTOR3	AVec3QuestPosition;		// 퀘스트 완료시 좌표, 2005-08-29 by cmkwon
} MSG_FC_QUEST_REQUEST_SUCCESS;	// C->F, Quest 결과 검증을 요청

struct QUEST_PAY_ITEM_INFO
{
	INT		ItemNum;					// 2007-07-09 by dhjin, 보상 아이템 번호 
	INT		ItemCount;					// 2007-07-09 by dhjin, 보상 아이템 수
};

typedef struct
{
	INT		QuestIndex;
	BOOL	IsSuccessful;
	INT		QuestResult;		// 퀘스트의 성적 혹은 점수, 0이면 무시, 10~13(A~D)이면 넘겨주기, check: 현재는 받은대로 넘겨주기, 20040224, kelovon
	INT		ExpOfCompensation;			// 2007-03-06 by cmkwon, 퀘스트 완료시 실제로 추가된 보상 경험치
	BYTE	BonusStatOfCompensation;	// 2007-07-09 by dhjin, BonusStat 보상 정보 
	INT		QuestPayInfoListCount;		// 2007-07-09 by dhjin, 아이템 보상 수
	ARRAY_(QUEST_PAY_ITEM_INFO);
} MSG_FC_QUEST_REQUEST_SUCCESS_RESULT;	// F->C, Quest 결과(완료)를 알림, Client는 이 MSG를 받으면 quest를 종료하고 After NPCTalk을 로딩한다

typedef struct
{
	float			fVCNInflDistributionPercent;	// 2006-02-08 by cmkwon, 바이제니유 정규군 세력분포 - 세력선택 미션시만 사용됨
	float			fANIInflDistributionPercent;	// 2006-02-08 by cmkwon, 알링턴 정규군 - 세력선택 미션시만 사용됨
	INT		NumOfQuest;
	ARRAY_(MEX_QUEST_INFO);		// QuestIndex의 array, see below
} MSG_FC_QUEST_PUT_ALL_QUEST;			// F->C, 완료되거나 진행중인 모든 퀘스트를 전송, 게임 시작 시 아이템 전송 후 이어서 보냄

typedef struct
{
	INT		QuestIndex;
} MSG_FC_QUEST_DISCARD_QUEST;		// C->F, 이미 시작된 퀘스트를 포기함

typedef struct
{
	INT		QuestIndex;
	BYTE	IsDiscarded;			// 퀘스트 포기 성공 여부, 1: TRUE, 0:FALSE
} MSG_FC_QUEST_DISCARD_QUEST_OK;	// F->C, 이미 시작된 퀘스트를 포기함에 대한 결과

struct MSG_FC_QUEST_MOVE_QUEST_MAP
{
	INT		QuestIndex0;
};

struct MSG_FC_QUEST_PUT_ALL_QUEST_MONSTER_COUNT
{
	INT		NumOfMonsterCount;
	ARRAY_(MEX_QUEST_MONSTER_COUNT);	
};	// 2005-10-25 by cmkwon

typedef MEX_QUEST_MONSTER_COUNT					MSG_FC_QUEST_UPDATE_MONSTER_COUNT; 	// 2005-10-25 by cmkwon

typedef MSG_FC_QUEST_REQUEST_SUCCESS			MSG_FC_QUEST_REQUEST_SUCCESS_CHECK;			// 2006-03-24 by cmkwon

typedef MSG_FC_QUEST_REQUEST_SUCCESS_RESULT		MSG_FC_QUEST_REQUEST_SUCCESS_CHECK_RESULT;	// 2006-03-24 by cmkwon

struct MSG_FC_QUEST_REQUEST_PARTY_WARP			// 2006-10-16 by cmkwon
{
	UID32_t				callerCharacterUID;			// 호출자 CharcterUID
	DWORD				dwRemainTime;				// 단위:ms(ex> 1000 <== 1초)
	MAP_CHANNEL_INDEX	warpMapChannIndex;
};

struct MSG_FC_QUEST_REQUEST_PARTY_WARP_ACK		// 2006-10-16 by cmkwon
{
	UID32_t				callerCharacterUID;
	MAP_CHANNEL_INDEX	warpMapChannIndex;
};

///////////////////////////////////////////////////////////////////////////////
// FC_MONSTER (Client <-> Field server)

typedef MSG_FC_CHARACTER_GET_MONSTER_INFO_OK		MSG_FC_MONSTER_CREATED;

typedef struct
{
	ClientIndex_t	MonsterIndex;
	ClientIndex_t	TargetIndex;
	AVECTOR3		PositionVector;
	AVECTOR3		TargetVector;
} MSG_FC_MONSTER_MOVE_OK;

typedef struct
{
	ClientIndex_t	MonsterIndex;
} MSG_FC_MONSTER_HIDE;

typedef MSG_FC_MONSTER_HIDE							MSG_FC_MONSTER_SHOW;

typedef struct
{
	ClientIndex_t	MonsterIndex;
	INT				CurrentHP;
} MSG_FC_MONSTER_CHANGE_HP;					// F -> C, 몬스터의 현재 HP를 전송함

typedef struct
{
	ClientIndex_t	MonsterIndex;
	BodyCond_t		BodyCondition;					// 앞의 32bit만 사용 <-- check: 스킬 bodycon이 정해지면 결정하기!
} MSG_FC_MONSTER_CHANGE_BODYCONDITION;

typedef struct
{
	ClientIndex_t		MonsterIndex;				// 스킬을 사용한 몬스터
	ClientIndex_t		ClientIndex;				// 몬스터 스킬의 종료를 알려줄 클라이언트 인덱스
	INT					SkillItemNum;				// 스킬의 ItemNum
} MSG_FC_MONSTER_SKILL_USE_SKILL;

// start 2011-05-17 by hskim, 인피니티 3차 - 시네마 몬스터 교체 기능
typedef struct
{
	ClientIndex_t	MonsterIndex;
	INT				ChangeMonsterUnitKind;
} MSG_FC_MONSTER_CHANGE_INDEX;					// F -> C, 몬스터 교체 메시지 전달
// end 2011-05-17 by hskim, 인피니티 3차 - 시네마 몬스터 교체 기능

typedef struct
{
	ClientIndex_t	MonsterIndex;
	AVECTOR3		PositionVector;
	AVECTOR3		TargetVector;
} MSG_FC_MONSTER_SKILL_END_SKILL;

struct MSG_FC_MONSTER_SUMMON_MONSTER
{
	ClientIndex_t	clientIdx;
	INT				SummonMonsterUnitKind;			// 몬스터 고유번호
	INT				nSummonCounts;
	BYTE			byMonsterTargetType2;
	int				nTargetTypeData2;
	AVECTOR3		PositionAVector;
};
struct MSG_FC_MONSTER_TUTORIAL_MONSTER_DEAD_NOTIFY
{
	ClientIndex_t	monsterIdx;	
};
struct MSG_FC_MONSTER_TUTORIAL_MONSTER_DELETE
{
	ClientIndex_t	monsterIdx;	
};

///////////////////////////////////////////////////////////////////////////////
// FN_MONSTER (Field server <-> NPC server)
typedef struct
{
	USHORT		CreateChannelCounts;
	BOOL		bCityWarStarted;
	UID32_t		CityWarOccupyGuildUID;
	ARRAY_(MSG_FN_CONNECT_SET_CHANNEL_STATE);
} MSG_FN_MAPPROJECT_START_OK;

typedef struct
{
	ChannelIndex_t			ChannelIndex;
	ClientIndex_t			MonsterIndex;				// 인덱스
	INT						MonsterUnitKind;				// 몬스터 고유번호
	BYTE					MonsterTargetType1;
	int						TargetTypeData1;
	ClientIndex_t			CltIdxForTargetType1;
	BodyCond_t				BodyCondition;					// 몬스터 BodyCondition
	AVECTOR3				PositionVector;					// 몬스터 좌표
	AVECTOR3				TargetVector;					//
	BYTE					ObjectMonsterType;
	MONSTER_BALANCE_DATA	MonsterBalanceData;				// 2010. 05. 19 by hsLee 인피니티 필드 2차 난이도 조절. (신호처리 + 몬스터 처리(서버) ) - 몬스터 밸런스 정보 추가.
} MSG_FN_MONSTER_CREATE;	// check: 불필요한 정보를 없애야 하지 않을까? 확인 요망!

typedef struct
{
	ChannelIndex_t	ChannelIndex;
	ClientIndex_t	MonsterIndex;
} MSG_FN_MONSTER_CREATE_OK;

typedef struct
{
	ChannelIndex_t	ChannelIndex;
	ClientIndex_t	MonsterIndex;
	BOOL			CinemaDelete;		// 2011-05-30 by hskim, 인피니티 3차 - 몬스터 삭제 클라이언트 반영 - 시네마에서 삭제한경우 TRUE
} MSG_FN_MONSTER_DELETE;

// start 2011-05-11 by hskim, 인피니티 3차 - 시네마 관련 기능 추가 - 해당 맵채널 특정 몬스터 변경 기능 추가
typedef struct
{
	ChannelIndex_t	ChannelIndex;
	ClientIndex_t	MonsterIndex;
	INT				ChangeMonsterUnitKind;
} MSG_FN_MONSTER_CHANGE_OK;
// end 2011-05-11 by hskim, 인피니티 3차 - 시네마 관련 기능 추가 - 해당 맵채널 특정 몬스터 변경 기능 추가

// start 2011-05-30 by hskim, 인피니티 3차 - 몬스터 삭제 클라이언트 반영
struct MSG_FC_MONSTER_CINEMA_DELETE_NOTIFY
{
	ClientIndex_t	MonsterIdx;
};
// end 2011-05-30 by hskim, 인피니티 3차 - 몬스터 삭제 클라이언트 반영

typedef struct
{
	ChannelIndex_t	ChannelIndex;
	ClientIndex_t	ClientIndex;
	ClientIndex_t	TargetIndex;
	AVECTOR3		PositionVector;
	AVECTOR3		TargetVector;
	USHORT			usSendRange;
} MSG_FN_MONSTER_MOVE;

typedef struct
{
	ChannelIndex_t	ChannelIndex;
	ClientIndex_t	ClientIndex;
	AVECTOR3		PositionVector;
	AVECTOR3		TargetVector;
} MSG_FN_MOVE_OK;

struct MEX_CHARACTER_1
{
	USHORT			Race;						// 종족
	USHORT			UnitKind;
	BYTE			InfluenceType0;				// 2005-12-27 by cmkwon
	BYTE			CharacterMode0;				// 2005-07-15 by cmkwon
	UID32_t			CharacterUniqueNumber;
	UID32_t			GuildUID;
	BodyCond_t		BodyCondition;				// 상태, bit flag 사용
	float			CurrentHP;					// 에너지
	AVECTOR3		PositionVector;				// 캐릭터 좌표

	MEX_CHARACTER_1 &operator=(const CHARACTER &rhs)
	{
		Race					= rhs.Race;
		UnitKind				= rhs.UnitKind;
		InfluenceType0			= rhs.InfluenceType;
		CharacterMode0			= rhs.CharacterMode;
		CharacterUniqueNumber	= rhs.CharacterUniqueNumber;
		GuildUID				= rhs.GuildUniqueNumber;
		BodyCondition			= rhs.BodyCondition;
		CurrentHP				= rhs.CurrentHP;
		PositionVector			= rhs.PositionVector;
		return *this;
	};
};
typedef struct
{
	ChannelIndex_t	ChannelIndex;
	ClientIndex_t	ClientIndex;
	MEX_CHARACTER_1	mexCharacter;
	UID32_t			GuildMasterCharUID;
	BOOL			bStealthState1;
	BOOL			bInvisible;			// 2006-11-27 by dhjin, 캐릭터 안보이기 플래그
} MSG_FN_CLIENT_GAMESTART_OK;

typedef struct
{
	ChannelIndex_t	ChannelIndex;
	ClientIndex_t	ClientIndex;
} MSG_FN_GET_CHARACTER_INFO;

typedef MSG_FN_CLIENT_GAMESTART_OK					MSG_FN_GET_CHARACTER_INFO_OK;

typedef struct
{
	ChannelIndex_t			ChannelIndex;
	char					CharacterName[SIZE_MAX_CHARACTER_NAME];
	INT						MonsterUnitKind;
	BYTE					MonsterTargetType1;
	int						TargetTypeData1;
	ClientIndex_t			CltIdxForTargetType1;
	INT						NumOfMonster;
	AVECTOR3				Position;

	// 2010. 06. 08 by hsLee 인피니티 필드 2차 난이도 조절. (아군 동일 밸런스 적용.)
	MONSTER_BALANCE_DATA	MonsterBalanceData;

} MSG_FN_ADMIN_SUMMON_MONSTER;

typedef struct _MSG_FN_MONSTER_CHANGE_HP
{
	ChannelIndex_t		ChannelIndex;
	ClientIndex_t		MonsterIndex;
	float				CurrentHP;				
} MSG_FN_MONSTER_CHANGE_HP;

typedef struct _MSG_FN_MONSTER_CHANGE_BODYCONDITION : MSG_FC_CHARACTER_CHANGE_BODYCONDITION
{
	ChannelIndex_t		ChannelIndex;
} MSG_FN_MONSTER_CHANGE_BODYCONDITION;

typedef struct
{
	ChannelIndex_t		ChannelIndex;
	ClientIndex_t		MonsterIndex;				// 스킬을 사용한 몬스터
	ClientIndex_t		ClientIndex;				// 몬스터 스킬의 종료를 알려줄 클라이언트 인덱스
	INT					SkillItemNum;				// 스킬의 ItemNum
} MSG_FN_MONSTER_SKILL_USE_SKILL;

typedef struct
{
	ChannelIndex_t		ChannelIndex;
	ClientIndex_t		MonsterIndex;
	AVECTOR3			PositionVector;
	AVECTOR3			TargetVector;
} MSG_FN_MONSTER_SKILL_END_SKILL;

struct MSG_FN_MONSTER_AUTO_DESTROYED		// 2006-04-17 by cmkwon
{
	MAP_CHANNEL_INDEX	MapChannIdx;				// 2007-11-26 by cmkwon, 몬스터 자동삭제 메시지 TCP로 전송(N->F) - MSG_FN_MONSTER_AUTO_DESTROYED 에 필드 추가
// 2007-11-26 by cmkwon, 몬스터 자동삭제 메시지 TCP로 전송(N->F) - MapChannIdx 로 추가
//	ChannelIndex_t		ChannelIndex;
	ClientIndex_t		MonsterIndex;
	BodyCond_t			BodyCondition;				// 2007-11-26 by cmkwon, 몬스터 자동삭제 메시지 TCP로 전송(N->F) - MSG_FN_MONSTER_AUTO_DESTROYED 에 필드 추가
};

struct MSG_FN_MONSTER_STRATEGYPOINT_INIT		// 2006-11-20 by cmkwon
{
	ChannelIndex_t		ChannelIndex;
	int					bVCNMapInflTyforInit;	// 2007-09-16 by dhjin, Belligerence 검색으로 수정으로 인하여 오브젝트 Bell값으로 수정 // 2006-11-21 by cmkwon, 초기화 할 맵이 VCN인지 여부(TRUE=IS_MAP_INFLUENCE_VCN, FALSE=IS_MAP_INFLUENCE_ANI)
	BOOL				bCreateFlag;			// 초기화 후에 소환 여부 플래그
	BOOL				bInfluenceBoss;			// 2007-08-30 by dhjin, 모선전시 전략포인트 소환인지 체크 비트 0 : FASLE, 1 : TRUE
};

struct MSG_FN_MONSTER_STRATEGYPOINT_SUMMON
{
	MapIndex_t			MapIndex;
};

struct MSG_FN_MONSTER_OUTPOST_INIT
{// 2007-08-24 by dhjin, 전진기지 맵 몬스터 초기화 맵 소유에 맞게 몬스터를 소환해야함 
	MAP_CHANNEL_INDEX	mapChann;
	int					bell1;			// 기본값이 -1 이어야 함
	int					bell2;			// 기본값이 -1 이어야 함
	int					bell3;			// 기본값이 -1 이어야 함
};

struct MSG_FN_MONSTER_OUTPOST_RESET_SUMMON
{// 2007-08-24 by dhjin, 전진기지 몬스터 소환, 주의~!! 보호막을 파괴한 세력 소유로 소환해야한다.
	MAP_CHANNEL_INDEX	mapChann;
	int					bell1;			// 기본값이 -1 이어야 함
};
typedef MSG_FN_MONSTER_OUTPOST_RESET_SUMMON		MSG_FN_MONSTER_TELEPORT_SUMMON;

struct MSG_MONSTER_SUMMON_BY_BELL
{// 2007-09-19 by cmkwon, 구조체 추가
	int					MonsterBell;	// 기본값이 -1 이어야 함
};



struct MSG_FN_NPCSERVER_SUMMON_JACO_MONSTER		// 2006-04-18 by cmkwon
{
	BYTE			Belligerence0;				// 
	BOOL			IsSummonJacoMonster;		// 
};

struct MSG_FN_NPCSERVER_DELETE_MONSTER_IN_MAPCHANNEL		// TCP:F->N, // 2007-08-22 by cmkwon, 해당 맵채널 몬스터 모두 삭제하기 기능 추가
{
	MAP_CHANNEL_INDEX	mapChann;
	BOOL				bAllFlag;		// FALSE가 아니면 아래의 필드들에 상관없이 모든 몬스터가 삭제됨
	int					bell1;			// 기본값이 -1 이어야 함
	int					bell2;			// 기본값이 -1 이어야 함
	int					excludeBell1;	// 기본값이 -1 이어야 함
	int					excludeBell2;	// 기본값이 -1 이어야 함
	BOOL				bNotCreateMonster;	// 몬스터 생성 금지 0 => False 1 => True
};

struct MSG_FN_MONSTER_CREATE_IN_MAPCHANNEL_BYVALUE		// TCP:F->N, // 2007-08-29 by dhjin, 해당 맵채널 몬스터 모두 생성가능하게 설정
{
	MAP_CHANNEL_INDEX	mapChann;
};


typedef struct
{
	ChannelIndex_t	ChannelIndex;
	UINT			ItemNum;						// 공격 무기의 종류
	ClientIndex_t	MonsterIndex;					// 공격 몬스터의 인덱스
	INT				WeaponIndex;					// 공격 무기 인덱스(몬스터에 한해 유일함)
	AVECTOR3		PositionVector;
	AVECTOR3		TargetVector;
} MSG_FN_MISSILE_MOVE;

typedef struct
{
	ChannelIndex_t	ChannelIndex;
	ClientIndex_t	MonsterIndex;
	UINT			RecoveryHP;
} MSG_FN_MONSTER_HPRECOVERY;


typedef struct
{
	ChannelIndex_t	ChannelIndex;
	ClientIndex_t	ClientIndex;
} MSG_FN_MONSTER_HIDE;
typedef MSG_FN_MONSTER_HIDE MSG_FN_MONSTER_SHOW;

///////////////////////////////////////////////////////////////////////////////
// FC_EVENT (Field server <-> Client)

// 워프 타입: WARP_TYPE_XXX
#define WARP_TYPE_SAME_MAP				(BYTE)0x00	// 같은 맵으로의 워프
#define WARP_TYPE_SAME_FIELD_SERVER		(BYTE)0x01	// 다른 맵(같은 필드 서버)으로의 워프
#define WARP_TYPE_OTHER_FIELD_SERVER	(BYTE)0x02	// 다른 맵(다른 필드 서버)으로의 워프

typedef struct
{
	MAP_CHANNEL_INDEX	MapChannelIndex;
	SERVER_ID			FieldServerID;
	USHORT				WarpAreaIndex;
} MSG_FC_EVENT_WARP;			// F->C, 다른 필드 서버로 워프함

typedef struct
{
	AVECTOR3			PositionVector;
	BYTE				CharacterMode0;		// 2005-07-27 by cmkwon
} MSG_FC_EVENT_WARP_SAME_MAP;				// F->C, 같은 필드 서버 & 같은 맵으로 워프함

typedef struct
{
	MAP_CHANNEL_INDEX	MapChannelIndex;
	BitFlag16_t			MapWeather;
	AVECTOR3			PositionVector;
	BYTE				CharacterMode0;		// 2005-07-27 by cmkwon
} MSG_FC_EVENT_WARP_SAME_FIELD_SERVER;		// F->C, 같은 필드 서버 & 다른 맵으로 워프함

typedef struct
{
	ClientIndex_t		ClientIndex;
	MapIndex_t			WarpMapIndex;	// 2005-11-01 by cmkwon, 이동 할 MapIndex
} MSG_FC_EVENT_OTHER_WARPED;

typedef struct
{
	char	AccountName[SIZE_MAX_ACCOUNT_NAME];
	char	Password[SIZE_MAX_PASSWORD_MD5_STRING];		// MD5
	char	PrivateIP[SIZE_MAX_IPADDRESS];
	UID32_t	AccountUniqueNumber;
	UID32_t	CharacterUniqueNumber;
	UINT	PartyNumber;
	MAP_CHANNEL_INDEX	MapChannelIndex;
	USHORT	WarpAreaIndex;
} MSG_FC_EVENT_WARP_CONNECT;

typedef struct
{
	ClientIndex_t	ClientIndex;
	AVECTOR3		PositionVector;
	AVECTOR3		TargetVector;
	BitFlag16_t		MapWeather;
} MSG_FC_EVENT_WARP_CONNECT_OK;

typedef struct
{
	BUILDINGNPC		BuildingNPCInfo;
// 2006-02-08 by cmkwon
//	float			fCityWarTexRate;				//
	float			fInflDistributionTexPercent;	// 2006-02-08 by cmkwon, 세력분포 세금
} MSG_FC_EVENT_ENTER_BUILDING;				// F->C

typedef struct
{
	UINT		BuildingIndex;
	BOOL		SendShopItemList;
} MSG_FC_EVENT_ENTER_BUILDING_OK;			// C->F

typedef struct
{
	UINT		BuildingIndex;
} MSG_FC_EVENT_LEAVE_BUILDING;				// C->F

typedef struct
{
	AVECTOR3	PositionVector;
	AVECTOR3	TargetVector;
} MSG_FC_EVENT_LEAVE_BUILDING_OK;			// F->C

typedef struct
{
	ClientIndex_t	ClientIndex;
} MSG_FC_EVENT_REQUEST_WARP;				// C->F, 파티가 있으면 파티장만 요청 가능, 나머지는 요청 불가

typedef struct
{
	Err_t			Reason;
} MSG_FC_EVENT_REJECT_WARP;					// F->C, check: 필요하게 되면 구체적인 MSG 내용 정하기!


//typedef struct
//{
//	INT				ObjectIndex;			// Object의 Index
//} MSG_FC_EVENT_REQUEST_OBJECT_EVENT;		// F->C, Object에 의한 Event를 요청

typedef struct
{
	DWORD			ObjectType;
	AVECTOR3		ObjectPosition;
} MSG_FC_EVENT_REQUEST_OBJECT_EVENT;		// F->C, Object에 의한 Event를 요청

typedef struct
{
	BitFlag16_t		MapWeather;
} MSG_FC_EVENT_CHANGE_WEATHER;

#define WEATHER_DEFAULT		(BitFlag16_t)0x0000	// 기본
#define WEATHER_SUNNY		(BitFlag16_t)0x0001	// 맑음
#define WEATHER_RAINY		(BitFlag16_t)0x0002	// 비
#define WEATHER_SNOWY		(BitFlag16_t)0x0004	// 눈
#define WEATHER_CLOUDY		(BitFlag16_t)0x0008	// 흐림
#define WEATHER_FOGGY		(BitFlag16_t)0x0010	// 안개

struct MEX_CHANNEL_INFO {
	ChannelIndex_t	ChannelIndex;
	INT				Crowdedness;		// 혼잡도, 0% ~ 100%??, check: 정확한 방식 결정해야 함
};

typedef struct {
	MapIndex_t		MapIndex;
	INT				WarpTargetIndex;	// 클라이언트는 이 정보를 MSG_FC_EVENT_SELECT_CHANNEL에 그대로 복사해서 서버에 넘겨주어야 함
	INT				NumOfChannels;		// 선택 가능한 채널의 수
	ARRAY_(MEX_CHANNEL_INFO);
} MSG_FC_EVENT_SUGGEST_CHANNELS;		// F->C, 선택 가능한 채널을 제시함

typedef struct {
	ClientIndex_t		ClientIndex;
	INT					WarpTargetIndex;	// 클라이언트는 이 정보를 MSG_FC_EVENT_SUGGEST_CHANNELS로부터 그대로 복사해서 서버에 넘겨주어야 함
	MAP_CHANNEL_INDEX	MapChannelIndex;
} MSG_FC_EVENT_SELECT_CHANNEL;				// C->F, 선택한 채널을 전송함

typedef struct {
	ClientIndex_t		ClientIndex;
	INT					WarpTargetIndex;	// 클라이언트는 이 정보를 MSG_FC_EVENT_SUGGEST_CHANNELS로부터 그대로 복사해서 서버에 넘겨주어야 함
	MAP_CHANNEL_INDEX	MapChannelIndex;
	int					nPartyMembers;		// 같이 워프할 파티원의 수, 편대장 제외
	ARRAY_(UID32_t);						// 파티원 character uniquenumber의 array
} MSG_FC_EVENT_SELECT_CHANNEL_WITH_PARTY;	// C->F, 선택한 채널을 전송함

typedef struct {
	ClientIndex_t		ClientIndex;
	MAP_CHANNEL_INDEX	MapChannelIndex;
} MSG_FC_EVENT_REQUEST_RACING_WARP;	// C->F, 레이싱맵으로의 워프 요청


typedef struct
{
	MAP_CHANNEL_INDEX	MapChannelIndex;
} MSG_FC_EVENT_GET_SHOP_WARP_TARGET_MAP_LIST;

struct WARP_TARGET_MAP_INFO_4_EXCHANGE
{
	MapIndex_t	MapIndex;
	INT			TargetIndex;	// EVENTINFO에 존재하는 TargetIndex
	char		TargetName[SIZE_MAX_WARP_TARGET_NAME];	// 타켓의 이름(ex: 도시상단, 도시하단)
	INT			Fee;			// 워프 요금
};

typedef struct
{
	INT		NumOfTargetMaps;
	ARRAY_(WARP_TARGET_MAP_INFO_4_EXCHANGE);
} MSG_FC_EVENT_GET_SHOP_WARP_TARGET_MAP_LIST_OK;

typedef struct
{
	MapIndex_t	MapIndex;
	INT			TargetIndex;			// EVENTINFO에 존재하는 TargetIndex	
} MSG_FC_EVENT_REQUEST_SHOP_WARP;	

struct MSG_FC_EVENT_CHARACTERMODE_ENTER_BUILDING
{
	INT			nBuildingIndex0;
};

struct MSG_FC_EVENT_CALL_WARP_EVENT_REQUEST			// 2006-07-21 by cmkwon
{
	DWORD				dwCallWarpEventID0;
	DWORD				dwSupportTermTick0;
	char				szUserMessage[SIZE_STRING_128];
};

struct MSG_FC_EVENT_CALL_WARP_EVENT_REQUEST_ACK		// 2006-07-21 by cmkwon
{
	DWORD				dwCallWarpEventID0;
};


///////////////////////////////////////////////////////////////////////////////
// FN_EVENT (Field server <-> NPC server)
typedef MSG_FC_EVENT_OTHER_WARPED	MSG_FN_EVENT_OTHER_WARPED;

///////////////////////////////////////////////////////////////////////////////
// FP_EVENT (Field server <-> Pre server)
typedef struct
{
	char			AccountName[SIZE_MAX_ACCOUNT_NAME];
	SERVER_ID		FieldServerID;
	UID32_t			CharacterUniqueNumber;
} MSG_FP_EVENT_NOTIFY_WARP;		// F --> P

typedef struct
{
	UID32_t			CharacterUniqueNumber;
} MSG_FP_EVENT_NOTIFY_WARP_OK;		// P --> F

typedef struct
{
	char			AccountName[SIZE_MAX_ACCOUNT_NAME];
} MSG_FP_EVENT_ENTER_SELECT_SCREEN;		// F->P, Client가 캐릭터 선택 창에 들어옴

typedef struct
{
	char				AccountName[SIZE_MAX_ACCOUNT_NAME];
	char				CharacterName[SIZE_MAX_CHARACTER_NAME];
	UID32_t				CharacterUniqueNumber;
	MAP_CHANNEL_INDEX	MapChannelIndex;	
} MSG_FP_EVENT_GAME_STARTED;			// F->P, Client가 게임을 시작함(맵으로 들어옴)

typedef struct
{
	char				AccountName[SIZE_MAX_ACCOUNT_NAME];
	char				CharacterName[SIZE_MAX_CHARACTER_NAME];
	UID32_t				CharacterUniqueNumber;
	MAP_CHANNEL_INDEX	MapChannelIndex;	
} MSG_FP_EVENT_MAP_CHANGED;				// F->P, Client가 맵을 이동함

// T0_FP_MONITOR
typedef struct
{
	MGAME_EVENT_t		enMGameEvent;
} MSG_FP_MONITOR_SET_MGAME_EVENT_TYPE;



///////////////////////////////////////////////////////////////////////////////
// FC_STORE
typedef struct
{
	UID32_t	AccountUniqueNumber;
	UID32_t	PossessCharacter;		// CharacterUniqueNumber 
	BYTE	ItemStorage0;			// 0(ITEM_IN_CHARACTER):캐릭터인벤, 1(ITEM_IN_STORE):창고
	BOOL	bSendToClient;			// Client에 전송할지 여부, WARP_CONNECT시는 전송 불필요
} MSG_FC_STORE_GET_ITEM;

typedef struct
{
	UID32_t	PossessCharacter;		// CharacterUniqueNumber
	BYTE	ItemStorage0;			// 2006-01-05 by cmkwon, 0(ITEM_IN_CHARACTER):캐릭터인벤, 1(ITEM_IN_STORE):창고
} MSG_FC_STORE_PUT_ITEM_HEADER;

typedef struct
{
	int		BytesToRead;			// 뒤에 붙는 아이템의 총 바이트수, 주의가 필요함
	ARRAY_(ITEM_XXX);
} MSG_FC_STORE_PUT_ITEM;

typedef struct
{
	UINT	NumOfItem;
	BYTE	ItemStorage0;			// 2006-01-05 by cmkwon, 0(ITEM_IN_CHARACTER):캐릭터인벤, 1(ITEM_IN_STORE):창고
} MSG_FC_STORE_PUT_ITEM_DONE;

typedef struct
{
	UID64_t		ItemUniqueNumber;		// STORE Talbe의 인덱스, DB paramemter는 (STORE_ITEM*)임
	BYTE		FromItemStorage;		// 0(ITEM_IN_CHARACTER):캐릭터인벤, 1(ITEM_IN_STORE):창고
	BYTE		ToItemStorage;			// 0(ITEM_IN_CHARACTER):창고->캐릭터인벤, 1(ITEM_IN_STORE):캐릭터인벤->창고
	INT			Count;					// 동일 아이템의 개수
} MSG_FC_STORE_MOVE_ITEM;

typedef struct
{
	UID64_t		ItemUniqueNumber;
	UID32_t		FromPossessCharacter;	// CharacterUniqueNumber or 0
	UID32_t		ToPossessCharacter;		// CharacterUniqueNumber or 0
	INT			Count;					// 동일 아이템의 개수
} MSG_FC_STORE_MOVE_ITEM_OK;

struct MSG_FC_STORE_LOG_GUILD_ITEM
{// 2006-09-27 by dhjin, C --> F 여단 로그 정보 요청
	UID32_t		AccountUID;
	UID32_t		CharacterUID;
};

struct MSG_FC_STORE_LOG_GUILD_ITEM_OK
{// 2006-09-27 by dhjin, F --> C 여단 로그 정보 전송
	BYTE			LogType;
	ATUM_DATE_TIME	Time;
	UID32_t			CharacterUID;
	CHAR			CharacterName[SIZE_MAX_CHARACTER_NAME];
	INT				ItemNum;
	INT				PrefixCodeNum;
	INT				SuffixCodeNum;
	INT				ItemChangeCount;
};

// 2008-01-23 by cmkwon, S_F, S_L: 장착/장착해제 게임 로그에 추가 - IDT_XXX, ITEM_DELETE_TYPE_XXX
#define IDT_GENERAL						((BYTE)0)	// 일반 용도
#define IDT_EXPIRE_TIME					((BYTE)1)	// 유효 시간/기간 경과
#define IDT_INVALID_ITEMNUM				((BYTE)2)	// 유효하지 않은 ItemNum		// 2009-11-20 by cmkwon, 소유 아이템 로딩시 유효하지 않은 아이템 처리 수정 - 
#define IDT_COUNTABLEITEM_ZERO			((BYTE)3)	// 카운터블 아이템 Count 0		// 2009-11-20 by cmkwon, 소유 아이템 로딩시 유효하지 않은 아이템 처리 수정 - 


// IUT_XXX, ITEM_UPDATE_TYPE_XXX
#define IUT_GENERAL						((BYTE)0)	// 일반 용도
#define IUT_DROP_ITEM					((BYTE)1)	// 드랍 아이템
#define IUT_MIXING						((BYTE)2)	// 아이템 조합
#define IUT_TRADE						((BYTE)3)	// 거래
#define IUT_SHOP						((BYTE)4)	// 상점 구매 아이템, 판매 금액 등
#define IUT_QUEST						((BYTE)5)	// 퀘스트에 의한 삽입
#define IUT_ADMIN						((BYTE)6)	// 관리, 개발용
#define IUT_BULLET						((BYTE)7)	// 총알 업데이트
#define IUT_SKILL						((BYTE)8)	// 스킬 아이템
#define IUT_LOADING						((BYTE)9)	// 게임 시작 시 로딩
#define IUT_AUCTION						((BYTE)10)	// 경매 관련
#define IUT_ENCHANT						((BYTE)11)	// 인챈트
#define IUT_USE_ITEM					((BYTE)12)	// 아이템 사용
#define IUT_MGAME_EVENT					((BYTE)13)	// MGameEvent
#define IUT_USE_ENERGY					((BYTE)14)	// 에너지류 아이템 사용
#define IUT_EXPIRE_CARD_ITEM			((BYTE)15)	// 시간제한 아이템 사용 시간 만료
#define IUT_PENALTY_ON_DEAD				((BYTE)16)	// 캐릭터 죽을 때의 패널티
#define IUT_PENALTY_AGEAR_FUEL_ALLIN	((BYTE)17)	// 캐릭터 죽을 때의 패널티
#define IUT_INFLUENCEWAR_KILLER_BONUS	((BYTE)18)	// 세력전에서 킬러 보너스
#define IUT_BONUS_ITEM					((BYTE)19)	// 2006-04-26 by cmkwon, 보너스 아이템
#define IUT_BAZAAR_SELL					((BYTE)20)	// 개인 판매 상점 거래
#define IUT_BAZAAR_BUY					((BYTE)21)	// 개인 구입 상점 거래
#define IUT_RANDOMBOX					((BYTE)22)	// 2006-08-10 by cmkwon, 랜덤박스 사용
#define IUT_GIVEEVENTITEM				((BYTE)23)	// 2006-08-25 by dhjin, 이벤트 아이템 사용
#define IUT_GUILD_STORE					((BYTE)24)	// 2006-09-25 by cmkwon, 여단 창고
#define IUT_EXPIRE_ITEM					((BYTE)25)	// 2006-09-29 by cmkwon, 아이템 만료
#define IUT_STORE						((BYTE)26)	// 2006-10-26 by cmkwon, 창고 이용
#define IUT_STORE_FEE					((BYTE)27)	// 2006-10-26 by cmkwon, 창고 이용료
#define IUT_ARENA_ITEM					((BYTE)28)	// 2007-06-04 by dhjin, 아레나 전용 아이템
#define IUT_TUTORIAL_PAY_ITEM			((BYTE)29)	// 2007-07-24 by dhjin, Tutorial 지급 아이템
#define IUT_EXPEDIENCYFUND_PAYBACK		((BYTE)30)	// 2007-08-22 by dhjin, 판공비 환급
#define IUT_GIVEEVENTITEM_COUPONEVENT	((BYTE)31)	// 2008-01-10 by cmkwon, 아이템 이벤트 시스템에 신 쿠폰 시스템 추가 - 
#define IUT_LUCKY_ITEM					((BYTE)32)	// 2008-11-10 by dhjin, 럭키머신
#define IUT_WAR_CONTRIBUTION			((BYTE)33)	// 2008-12-23 by dhjin, 전쟁 보상 추가안
#define IUT_WAR_CONTRIBUTION_LEADER		((BYTE)34)	// 2008-12-23 by dhjin, 전쟁 보상 추가안 - 지도자
#define IUT_WAR_CONTRIBUTION_GUILD		((BYTE)35)	// 2008-12-23 by dhjin, 전쟁 보상 추가안 - 길드
#define IUT_DISSOLUTION_ITEM			((BYTE)36)	// 2010-08-31 by shcho&jskim 아이템용해 시스템 - 용해 시스템 패킷 처리
#ifdef _INET_PET
	#define IUT_DEPENDENCY_ITEM			((BYTE)37)
#endif
#define IUT_MARKET						((BYTE)42)
#define IUT_CITYWAR_BRING_SUMOFTEX		((BYTE)100)	// 도시점령전 세금

typedef struct
{
	UID32_t			FromCharacterUniqueNumber;		// From Possess, 혹은 FromCharacterUniqueNumber
	BYTE			ItemInsertionType;				// 아이템 삽입 타입, IUT_XXX
	ITEM_GENERAL	ItemGeneral;
} MSG_FC_STORE_INSERT_ITEM;

typedef struct
{
	UID64_t			ItemUniqueNumber;	// 지울 아이템
	BYTE			ItemDeletionType;	// 아이템 삭제 타입, IUT_XXX
} MSG_FC_STORE_DELETE_ITEM;

typedef struct
{
	UID64_t			ItemUniqueNumber;	// Update할 아이템
	INT				NewCount;			// Count의 새 값
	BYTE			ItemUpdateType;		// 아이템 수정 타입, IUT_XXX
} MSG_FC_STORE_UPDATE_ITEM_COUNT;

typedef struct
{
	UID64_t			ItemUniqueNumber;
	float			Endurance;
} MSG_FC_STORE_UPDATE_ENDURANCE;

typedef struct
{
	UID64_t			ItemUniqueNumber;
	UID32_t			FromPossess;
	UID32_t			ToPossess;
} MSG_FC_STORE_UPDATE_POSSESS;

typedef struct
{
	UID64_t	ItemUID;
	INT		PrefixCodeNum;	// 접두사, 없으면 0
	INT		SuffixCodeNum;	// 접미사, 없으면 0
} MSG_FC_STORE_UPDATE_RARE_FIX;

struct MSG_FC_STORE_INSERT_USINGITEM
{
	INT				ItemNum;
	UID64_t			ItemUID;						// 2006-04-24 by cmkwon
	BYTE			ItemInsertionType;				// 아이템 삽입 타입, IUT_XXX
	INT				nRemainSecond;
};

struct MSG_FC_STORE_DELETE_USINGITEM
{
	INT				ItemNum;
	BYTE			ItemDeletionType;	// 아이템 삭제 타입, IUT_XXX
};

struct MSG_FC_STORE_UPDATE_USINGITEM
{
	UID64_t			ItemUID0;
	INT				UsingTimeStamp0;	// 지금까지 사용된 시간(단위:초)
};

struct MSG_FC_STORE_EXPIRE_USINGITEM		// 2006-10-11 by cmkwon
{
	UID64_t			ItemUID0;
};

struct MSG_FC_STORE_UPDATE_ITEMSTORAGE
{
//	UID32_t			PossessCharacter;		// 2006-09-18 by dhjin, 선택된 케릭터
	UID64_t			ItemUniqueNumber;
	UID32_t			FromItemStorage;
	UID32_t			ToItemStorage;
};

struct MSG_FC_STORE_UPDATE_ITEMNUM		// 2006-06-14 by cmkwon
{
	UID64_t			ItemUniqueNumber;
	INT				ItemNum;
	BYTE			ItemUpdateType;		// 아이템 수정 타입, IUT_XXX
};

///////////////////////////////////////////////////////////////////////////////
// 2006-09-04 by dhjin, 퀵 슬롯 관련 
#define QUICKTABCOUNT		3
#define QUICKSLOTCOUNT		10			// 2008-06-19 by dhjin, EP3 - 8 -> 10으로 수정  

struct SQUICKSLOT_INFO
{
	BOOL		Delete;
	int			arrItemNum_[QUICKSLOTCOUNT];
};

struct MSG_FC_STORE_REQUEST_QUICKSLOT_OK
{
	SQUICKSLOT_INFO		POS[QUICKTABCOUNT];
};

struct MSG_FC_STORE_SAVE_QUICKSLOT
{
	SQUICKSLOT_INFO		POS[QUICKTABCOUNT];
};

///////////////////////////////////////////////////////////////////////////////
// FC_ITEM
typedef struct
{
	UINT			ItemFieldIndex;				// 습득 전까지 서버가 임시로 관리하는 번호
	INT				ItemNum;					// 클라이언트에 아이템의 종류를 보여주기 위해 보냄
	UID32_t			FirstCharacterUID;			// 아이템 습득 가능한 첫번째 캐릭터
	INT				Amount;						// 아이템의 개수
	AVECTOR3		Position;
	BYTE			DropItemType;				// 떨어진 아이템의 종류(습득용 아이템, 공격용(마인류) 아이템 등등)
} MSG_FC_ITEM_SHOW_ITEM;

#define DROP_ITEM_TYPE_PICKUP		(BYTE)0x00		// 습득용 아이템
#define DROP_ITEM_TYPE_ATTACK_MINE	(BYTE)0x01		// 공격용 마인류 아이템

typedef struct
{
	UINT			ItemFieldIndex;				// 습득 전까지 서버가 임시로 관리하는 번호
	ClientIndex_t	ClientIndex;				// 아이템을 먹은 character의 client index
} MSG_FC_ITEM_HIDE_ITEM;

typedef struct
{
	ClientIndex_t	ClientIndex;
	UINT			ItemFieldIndex;					// 습득 전까지 서버가 임시로 관리하는 번호
} MSG_FC_ITEM_GET_ITEM;

typedef struct
{
	ClientIndex_t	ClientIndex;
	UINT			ItemFieldIndex;				// 습득 전까지 서버가 임시로 관리하는 번호
	INT				ItemNum;					// 아이템의 종류
	INT				Amount;						// 아이템의 개수
	INT				SizeOfItem;					// sizeof(해당Item)
	ARRAY_(ITEM_XXX);							// ITEM_XXX가 온다
} MSG_FC_ITEM_GET_ITEM_OK;
#ifdef _INET_LINK_CHAT
typedef struct
{
	char				TargetCharcterName[SIZE_MAX_CHARACTER_NAME];
	UID64_t				nItemUniqueNumber;
} MSG_FC_GET_ITEM_BY_CHAR8UID;
typedef struct
{
	UID64_t				nItemUniqueNumber;
	ITEM_GENERAL		itemGeneral;

} MSG_FC_GET_ITEM_BY_CHAR8UID_OK;
#endif
typedef struct
{
	ClientIndex_t	ClientIndex;
	UINT			ItemFieldIndex;				// 습득 전까지 서버가 임시로 관리하는 번호
} MSG_FC_ITEM_GET_ITEM_FAIL;

typedef struct
{
	UINT			ItemFieldIndex;				// 습득 전까지 서버가 임시로 관리하는 번호
	STRUCT_(ITEM_XXX);							// 이후에 해당 ITEM_XXXX가 온다
} MSG_FC_ITEM_PUT_ITEM;

// 2005-11-15 by cmkwon, 삭제함
//typedef struct
//{
//	ClientIndex_t	ClientIndex;
//	BYTE			KindOfSkill;				// check: 의미 파악 및 이름 정의 필요
//} MSG_FC_ITEM_BONUSSKILLPOINT;
//
//typedef struct
//{
//	ClientIndex_t	ClientIndex;
//	BYTE			OldKindOfSkill;				// check: 의미 파악 및 이름 정의 필요
//	BYTE			NewKindOfSkill;				// check: 의미 파악 및 이름 정의 필요
//	BYTE			RemainedAmountOfSkill;		// check: 변수 type 확인해야 함!
//} MSG_FC_ITEM_BONUSSKILLPOINT_OK;

typedef struct
{
	UID32_t			CharacterUniqueNumber;
	UID64_t			FromItemUniqueNumber;	// 게임 유저가 이동할 아이템
	INT				FromItemWindowIndex;	// 게임 유저가 이동할 아이템의 기존 위치
	UID64_t			ToItemUniqueNumber;		// 이동 위치(destination)에 들어 있는 아이템(존재한다면), 없다면 0
	INT				ToItemWindowIndex;		// 이동 위치(destination)의 ItemWindowIndex
} MSG_FC_ITEM_CHANGE_WINDOW_POSITION;

typedef struct
{
	UID64_t			UniqueNumber;			// 게임 유저가 이동한 아이템
	INT				ItemWindowIndex;		// 게임 유저가 이동한 아이템의 새 위치
	BYTE			Wear;					// 게임 유저가 이동한 아이템의 wear 상태
	UID64_t			UniqueNumberDest;		// 이동 위치(destination)에 들어 있던 기존 아이템(존재한다면), 없다면 0
	INT				ItemWindowIndexDest;	// 이동 위치(destination)에 들어 있던 기존 아이템의 새 위치(존재한다면), 없다면 POS_INVALID_POSITION
	BYTE			WearDest;				// 이동 위치(destination)에 들어 있던 기존 아이템 wear 상태(존재한다면), 없다면 0
} MSG_FC_ITEM_CHANGE_WINDOW_POSITION_OK;
#ifdef _INET_PET
typedef struct
{
	UID64_t			UniqueNumber;
	INT				ItemWindowIndex;
	BYTE			Wear;
} MSG_FC_ITEM_NOTIFY_WINDOW_POSITION;
#endif
typedef struct
{
	INT				NumOfItems;
	ARRAY_(ITEM_WINDOW_INFO);
} MSG_FC_ITEM_UPDATE_WINDOW_ITEM_LIST;

typedef struct
{
	UID64_t			ItemUniqueNumber;
	BYTE			ItemKind;
	BYTE			Wear;
	INT				ItemWindowIndex;
} ITEM_WINDOW_INFO;

typedef struct
{
	ClientIndex_t	ClientIndex;
	UID64_t			ItemUniqueNumber;
	INT				Amount;					// 버리는 개수
} MSG_FC_ITEM_THROW_AWAY_ITEM;

typedef struct
{
	ClientIndex_t	ClientIndex;
	UID64_t			ItemUniqueNumber;
	INT				RemainedNumOfItem;		// 버린 후 남아있는 개수. 0 이면 완전히 버리기.
} MSG_FC_ITEM_THROW_AWAY_ITEM_OK;

typedef struct
{
	ClientIndex_t	ClientIndex;
	UID64_t			ItemUniqueNumber;
	INT				nParam1;				//
} MSG_FC_ITEM_USE_ENERGY;
struct MSG_FC_ITEM_USE_ENERGY_OK
{
	ClientIndex_t	ClientIndex;
	INT				ItemNum;
};

struct MSG_FC_ITEM_USE_ITEM
{
	ClientIndex_t	ClientIndex;
	UID64_t			ItemUniqueNumber;
	char			str256ChatMsg[SIZE_MAX_STRING_256];		// 2007-08-09 by cmkwon, 모든 세력에 채팅 전송하기 - 
};
struct MSG_FC_ITEM_USE_ITEM_OK
{
	ClientIndex_t	ClientIndex;
	INT				ItemNum;
};

typedef struct
{
	UINT			ItemFieldIndex;	// 습득 전까지 서버가 임시로 관리하는 번호
	AVECTOR3		DropPosition;	// 아이템의 위치
} MSG_FC_ITEM_DELETE_ITEM_ADMIN;

typedef struct
{
	UINT			ItemFieldIndex;	// 습득 전까지 서버가 임시로 관리하는 번호
	AVECTOR3		DropPosition;	// 아이템(마인)의 위치
} MSG_FC_ITEM_DELETE_DROP_ITEM;		// F->C, 자신이 뿌린 아이템(마인등)을 지울 때 쓰임

typedef struct {
	ClientIndex_t	ClientIndex;
	BYTE			ItemPosition;	// POS_XXX
	INT				ItemNum;
// 2009-08-26 by cmkwon, 그래픽 리소스 변경 시스템 구현 - 변경
//	INT				ColorCode;		// 2005-12-08 by cmkwon, 아머 색상 튜닝 정보
	INT				nShapeItemNum;		// 2009-08-26 by cmkwon, 그래픽 리소스 변경 시스템 구현 - 
	INT				nEffectItemNum;		// 2009-08-26 by cmkwon, 그래픽 리소스 변경 시스템 구현 - 
	INT				nPetLevel;
} MSG_FC_ITEM_UPDATE_ITEM_POS;		// F->C, 아이템 장착(전)을 갱신함, 아이템 장착을 바꾸면 주위에 전송함

typedef struct
{
	UID64_t			EnchantItemUniqueNumber;	// 인챈트 아이템
	UID64_t			TargetItemUniqueNumber;		// 인챈트를 바를 대상 아이템
	UID64_t			AttachItemUniqueNumber;		// 추가 아이템
	UID64_t			IncreaseProbabilityItemUID;	// 2009-01-19 by dhjin, 인첸트 확률 증가, 10인첸 파방 카드 - 인첸트 확률 증가 카드
	UID64_t			EnchantItemUniqueNumber2;	// 인챈트 아이템2 // 2010-04-20 by cmkwon, 신규 러키 머신 구현 - SuffixCard ItemUID 
} MSG_FC_ITEM_USE_ENCHANT;

typedef struct
{
	// 2010-04-20 by cmkwon, 신규 러키 머신 구현 - (빠진부분추가) - 
	//INT				ItemNum;					// 인챈트 아이템
	INT				EnchantItemNum;			// 2010-04-20 by cmkwon, 신규 러키 머신 구현 - (빠진부분추가) - 
	INT				SuffixRareItemNum;		// 2010-04-20 by cmkwon, 신규 러키 머신 구현 - (빠진부분추가) - 
	BOOL			bSuccessFlag;			// 2010-04-20 by cmkwon, 신규 러키 머신 구현 - (빠진부분추가) - 
} MSG_FC_ITEM_USE_ENCHANT_OK;					// 인챈트 완료를 보냄

typedef struct
{
	ENCHANT			Enchant;
} MSG_FC_ITEM_PUT_ENCHANT;
#ifdef BONUS_STAT_ITEM
typedef struct
{
	BONUS			Bonus;
} MSG_FC_ITEM_PUT_BONUS;
#endif
struct MSG_FC_ITEM_DELETE_ALL_ENCHANT
{
	UID64_t			ItemUniqueNumber;			// 인챈트를 삭제할 아이템	
};

typedef struct
{
	INT				NumOfItems;
	INT				nMixCounts;		// 2008-03-17 by cmkwon, 조합 시스템 기능 추가 - 
	ARRAY_(ITEM_UNIQUE_NUMBER_W_COUNT);
} MSG_FC_ITEM_MIX_ITEMS;		// C->F, 조합할 아이템의 리스트를 전송

typedef struct
{
	Err_t			Err;		// ERR_XXX, ERROR_NO_ERROR이면 성공
} MSG_FC_ITEM_MIX_ITEMS_RESULT;	// F->C, 아이템 조합 결과를 전송

struct MSG_FC_ITEM_USE_CARDITEM_GUILDSUMMON
{
	UID64_t			ItemUniqueNumber;
	char			szCharacterName10[SIZE_MAX_CHARACTER_NAME];
};
struct MSG_FC_ITEM_USE_CARDITEM_GUILDSUMMON_NOTIFY
{
	ClientIndex_t	guildSummonClientIdx;
};

struct MSG_FC_ITEM_USE_CARDITEM_RESTORE
{
	UID64_t			ItemUniqueNumber;
	char			szCharacterName10[SIZE_MAX_CHARACTER_NAME];
};
struct MSG_FC_ITEM_USE_CARDITEM_RESTORE_NOTIFY
{
	ClientIndex_t	restoreClientIdx;
};

struct MSG_FC_ITEM_USE_CARDITEM_GUILD
{
	UID64_t			ItemUniqueNumber;
};
struct MSG_FC_ITEM_USE_CARDITEM_GUILD_NOTIFY
{
	ClientIndex_t	notifyClientIdx;
	UID32_t			guildUID;
};
struct MSG_FC_ITEM_USE_CARDITEM_MONSTERSUMMON
{
	UID64_t			ItemUniqueNumber;
};
struct MSG_FC_ITEM_USE_CARDITEM_MONSTERSUMMON_NOTIFY
{
	ClientIndex_t	notifyClientIdx;
	INT				notifyItemNum;
	INT				summonMonsterUnitKind;
};
struct MSG_FC_ITEM_USE_CARDITEM_CHANGECHARACTERNAME
{
	UID64_t			ItemUniqueNumber;
	char			szChangeCharacterName[SIZE_MAX_CHARACTER_NAME];
};
struct MSG_FC_ITEM_USE_CARDITEM_CHANGECHARACTERNAME_NOTIFY
{
	ClientIndex_t	notifyClientIdx;
	char			szChangedCharacterName[SIZE_MAX_CHARACTER_NAME];
	char			szOriginCharacterName[SIZE_MAX_CHARACTER_NAME];
};
struct MSG_FC_ITEM_USE_CARDITEM_SKILLINITIALIZE
{
	UID64_t			ItemUniqueNumber;
	INT				skillItemNum;
};
struct MSG_FC_ITEM_USE_CARDITEM_SKILLINITIALIZE_NOTIFY
{
	ClientIndex_t	notifyClientIdx;
	INT				notifyItemNum;
};
struct MSG_FC_ITEM_USE_CARDITEM_CHANGEPILOTFACE
{
	UID64_t			ItemUniqueNumber;
	BYTE			pilotFace;
};
struct MSG_FC_ITEM_USE_CARDITEM_CHANGEPILOTFACE_NOTIFY
{
	ClientIndex_t	notifyClientIdx;
	INT				notifyItemNum;
	BYTE			pilotFace;
};

struct MSG_FC_ITEM_USE_INFLUENCE_BUFF				// 2006-04-21 by cmkwon
{
	UID64_t			ItemUniqueNumber;
};
struct MSG_FC_ITEM_USE_INFLUENCE_BUFF_OK			// 2006-04-21 by cmkwon
{
// 2009-01-05 by dhjin, 미션마스터 - 편대 버프 아이템 추가 - 밑과 같이 변경
//	UID64_t			ItemUniqueNumber;
	char			ItemUseCharacterName[SIZE_MAX_CHARACTER_NAME];
	INT				ItemNum;
};
struct MSG_FC_ITEM_USE_INFLUENCE_GAMEEVENT			// 2006-04-21 by cmkwon
{
	UID64_t			ItemUniqueNumber;
};
struct MSG_FC_ITEM_USE_INFLUENCE_GAMEEVENT_OK		// 2006-04-21 by cmkwon
{
	UID64_t			ItemUniqueNumber;
};

// 2006-08-10 by cmkwon
#define RANDOMBOX_RESULT_FAIL				0		// 실패
#define RANDOMBOX_RESULT_ITEM				1		// 아이템
#define RANDOMBOX_RESULT_SPI				2		// Money(SPI)
#define RANDOMBOX_RESULT_EXP				3		// 경험치
#define RANDOMBOX_RESULT_SKILL_SUPPORT_ITEM	4		// 스킬 보조 아이템, 2006-09-29 by cmkwon 추가함, ITEM_ATTR_SKILL_SUPPORT_ITEM
#define RANDOMBOX_RESULT_KIND_COUNT			5		// 랜덤박스 결과 종류 개수

struct MSG_FC_ITEM_USE_RANDOMBOX		// 2006-08-10 by cmkwon
{
	UID64_t			ItemUID;
};
struct MSG_FC_ITEM_USE_RANDOMBOX_OK		// 2006-08-10 by cmkwon
{
	char			szCharacterName0[SIZE_MAX_CHARACTER_NAME];	// 사용자 CharacterName
	INT				nRandomBoxResult;							// RANDOMBOX_RESULT_XXX
	INT				nResultItemNum0;							// 생성된 ItemNum
	INT64			n64ResultCounts;							//
	INT				nPrefixCodeNum0;							// 접두사, 없으면 0
	INT				nSuffixCodeNum0;							// 접미사, 없으면 0
};

typedef MSG_FC_ITEM_USE_ITEM			MSG_FC_ITEM_USE_SKILL_SUPPORT_ITEM;		// 2006-09-29 by cmkwon
typedef MSG_FC_ITEM_USE_ITEM_OK			MSG_FC_ITEM_USE_SKILL_SUPPORT_ITEM_OK;	// 2006-09-29 by cmkwon

#ifdef _INET_PET
///////////////////////////////////////////////////////////////////////////////////////
// start 2011-09-20 by hskim, 파트너 시스템 2차

struct MSG_FC_ITEM_USE_PET_SOCKET_ITEM
{
	ClientIndex_t	ClientIndex;
	UID64_t			ItemUniqueNumber;
};

struct MSG_FC_ITEM_USE_PET_SOCKET_ITEM_OK
{
	ClientIndex_t	ClientIndex;
	UID64_t			ItemUniqueNumber;
};

struct MSG_FC_ITEM_CANCEL_PET_SOCKET_ITEM
{
	ClientIndex_t	ClientIndex;
	UID64_t			ItemUniqueNumber;
};

struct MSG_FC_ITEM_CANCEL_PET_SOCKET_ITEM_OK
{
	ClientIndex_t	ClientIndex;
	UID64_t			ItemUniqueNumber;
};

// end 2011-09-20 by hskim, 파트너 시스템 2차
///////////////////////////////////////////////////////////////////////////////////////
#endif
//////////////////////////////////////////////////////////////////////////
// 2008-11-04 by dhjin, 럭키머신
// 2009-03-03 by dhjin, 럭키머신 수정안 - 밑과 같이 변경
// typedef MSG_FC_ITEM_USE_RANDOMBOX		MSG_FC_ITEM_USE_LUCKY_ITEM;
struct MSG_FC_ITEM_USE_LUCKY_ITEM
{
	UID64_t			ItemUID;
	INT				MachineNum;
};

struct LUCKY_ITEM_SIMPLEINFO
{
	SHORT			SlotNum;
	INT				MysteryItemDropNum;
	INT				Itemnum;
	INT64			n64ResultCounts;							//
	INT				nPrefixCodeNum0;							// 접두사, 없으면 0
	INT				nSuffixCodeNum0;							// 접미사, 없으면 0
};

struct MSG_FC_ITEM_USE_LUCKY_ITEM_OK
{
	LUCKY_ITEM_SIMPLEINFO	LuckyItemSimpleInfo[MaxPayLuckyMachineItem];
};


struct MSG_FC_ITEM_CHANGED_SHAPEITEMNUM			// 2009-08-26 by cmkwon, 그래픽 리소스 변경 시스템 구현 - 
{
	UINT64			nItemUID;
	INT				nShapeItemNum;
};

struct MSG_FC_ITEM_CHANGED_EFFECTITEMNUM		// 2009-08-26 by cmkwon, 그래픽 리소스 변경 시스템 구현 - 
{
	UINT64			nItemUID;
	INT				nEffectItemNum;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FC_SHOP
typedef struct
{
	UINT		BuildingIndex;
} MSG_FC_SHOP_PUT_ITEM_HEADER;

typedef struct
{
	USHORT		BytesToRead;			// 뒤에 붙는 아이템들의 총 바이트수
	ARRAY_(SHOP_ITEM);
} MSG_FC_SHOP_PUT_ITEM;

#define UNLIMITED_REMAIN_COUNT_FOR_LIMITED_EDITION		-1		// 2010-01-26 by cmkwon, 캐쉬 아이템 한정판매 시스템 구현 - 
#define SOLD_OUT_REMAIN_COUNT_FOR_LIMITED_EDITION		0		// 2010-01-26 by cmkwon, 캐쉬 아이템 한정판매 시스템 구현 - 

struct SHOP_ITEM
{
	UINT		ItemNum;
	char		ItemName[SIZE_MAX_ITEM_NAME];
	USHORT		MinTradeQuantity;
	INT			Price;
	BYTE		ItemKind;
	BYTE		CashShopIndex;		// 2009-01-28 by cmkwon, 캐쉬?수정(추천탭,신상품 추가) - SHOP_ITEM에 추가, 하위 4비트는 탭인덱스, 상위 4비트는 비트 플래그로 사용
	INT			RemainCountForLimitedEdition;		// 2010-01-26 by cmkwon, 캐쉬 아이템 한정판매 시스템 구현 - 

	// 2010-01-26 by cmkwon, 캐쉬 아이템 한정판매 시스템 구현 - 매진여부
	BOOL IsSoldOutShopItem(void)
	{
		if(SOLD_OUT_REMAIN_COUNT_FOR_LIMITED_EDITION != RemainCountForLimitedEdition)
		{
			return FALSE;
		}
		return TRUE;
	}
	BOOL IsLimitedEditionShopItem(void)
	{
		if(SOLD_OUT_REMAIN_COUNT_FOR_LIMITED_EDITION > RemainCountForLimitedEdition)
		{
			return FALSE;
		}
		return TRUE;
	}
};

typedef struct
{
	UINT		NumOfItem;
} MSG_FC_SHOP_PUT_ITEM_DONE;

typedef struct
{
	INT			ItemNum;
} MSG_FC_SHOP_GET_ITEMINFO;

typedef struct
{
	ITEM		ItemInfo;
} MSG_FC_SHOP_GET_ITEMINFO_OK;

typedef struct
{
	UINT		BuildingIndex;
	UINT		ItemNum;
	INT			Amount;					// 구매할 아이템의 개수
} MSG_FC_SHOP_BUY_ITEM;

typedef struct
{
	INT			RemainedMoney;			// 구매 후 남은 돈
	INT			SizeOfItem;				// sizeof(해당Item)의 총합
	ARRAY_(ITEM_XXX);						// ITEM_XXX가 온다
} MSG_FC_SHOP_BUY_ITEM_OK;

typedef struct
{
	UINT		BuildingIndex;
	UID64_t		ItemUniqueNumber;
	BYTE		ItemKind;
	INT			Amount;					// 판매할 아이템의 개수
} MSG_FC_SHOP_SELL_ITEM;

typedef struct
{
	UID64_t		ItemUniqueNumber;
	INT			RemainedNumOfItem;		// 판매 후 남아 있는 수량(0이면 삭제, Countable Item 인 경우 의미 가짐)
	INT			RemainedMoney;			// 판매 후 남은 돈
} MSG_FC_SHOP_SELL_ITEM_OK;

typedef struct
{
	UID64_t		ItemUniqueNumber;
	INT			Amount;
} MSG_FC_SHOP_GET_USED_ITEM_PRICE;

typedef struct
{
	UID64_t		ItemUniqueNumber;
	UINT		Price;
} MSG_FC_SHOP_GET_USED_ITEM_PRICE_OK;

typedef struct
{
	INT			BuildingIndex;
} MSG_FC_SHOP_GET_SHOP_ITEM_LIST;	// C->F, 상점에서 파는 아이템의 리스트를 요청, 응답은 T_FC_SHOP_PUT_ITEM_XXX

typedef struct
{
	INT			DesParam;			// 수리할 부분(DES_HP, DES_DP, DES_EP, DES_SP, DES_BULLET_01, DES_BULLET_02)
	INT			Count;				// 어느 양만큼 수리할 것인지
} MSG_FC_SHOP_REQUEST_REPAIR;		// C->F, 기체 수리 요청

struct MSG_FC_SHOP_REQUEST_REPAIR_OK
{
	INT			DesParam;			// 수리할 부분(DES_HP, DES_DP, DES_EP, DES_SP, DES_BULLET_01, DES_BULLET_02)
	INT			Count;				// 수리한 양
	INT			RepairCost;			// 수리 비용
};

struct MSG_FC_SHOP_BUY_CASH_ITEM
{
	UINT		BuildingIndex;
	UINT		ItemNum;
	INT			Amount;					// 구매할 아이템의 개수
	UID32_t		GiveTargetCharacterUID;	// 2007-11-13 by cmkwon, 선물하기 기능 추가 - 0 이면 자신이 구입, 0이 아니면 해당 캐릭터에게 선물하기
};
struct MSG_FC_SHOP_BUY_CASH_ITEM_OK
{// 2007-11-13 by cmkwon, 선물하기 기능 추가 - MSG_FC_SHOP_BUY_CASH_ITEM_OK 구조체 추가
	UINT		ItemNum;
	INT			Amount;					// 구매할 아이템의 개수
	UID32_t		GiveTargetCharacterUID;	// 2007-11-13 by cmkwon, 선물하기 기능 추가 - 0 이면 자신이 구입, 0이 아니면 해당 캐릭터에게 선물하기
	char		GiveTargetCharacterName[SIZE_MAX_CHARACTER_NAME];	// 2007-11-13 by cmkwon, 선물하기 기능 추가 - 
	INT			RemainCountForLimitedEdition;		// 2010-01-26 by cmkwon, 캐쉬 아이템 한정판매 시스템 구현 - 
};

struct MSG_FC_SHOP_BUY_COLOR_ITEM	// 2005-12-06 by cmkwon
{
	UINT		BuildingIndex;
	UINT		ItemNum;
};

struct MSG_FC_SHOP_BUY_COLOR_ITEM_OK	// 2005-12-06 by cmkwon
{
	UID64_t		ItemUID64;
	INT			ColorCode;
};

struct MSG_FC_SHOP_BUY_WARPOINT_ITEM
{// 2007-06-13 by dhjin, WarPoint 샵 
	UINT		BuildingIndex;
	UINT		ItemNum;
	INT			Amount;					// 구매할 아이템의 개수
};

struct MSG_FC_SHOP_BUY_WARPOINT_ITEM_OK
{// 2007-06-13 by dhjin, WarPointItem 구입하고 난 정보.
	INT				PayWarPoint;			// 2007-06-13 by dhjin, 차감된 WarPoint
	INT				WarPoint;				// 2007-06-13 by dhjin, 총 WarPoint
};

struct MSG_FC_SHOP_CHECK_GIVE_TARGET
{// 2007-11-13 by cmkwon, 선물하기 기능 추가 - MSG_FC_SHOP_CHECK_GIVE_TARGET 구조체 추가
	char			GiveTargetCharName[SIZE_MAX_CHARACTER_NAME];	// 선물받는 캐릭터명
	INT				GiveItemNum;
};

struct MSG_FC_SHOP_CHECK_GIVE_TARGET_OK
{// 2007-11-13 by cmkwon, 선물하기 기능 추가 - MSG_FC_SHOP_CHECK_GIVE_TARGET_OK 구조체 추가
	char			GiveTargetCharName[SIZE_MAX_CHARACTER_NAME];	// 선물받는 캐릭터명
	UID32_t			GiveTargetCharUID;
	char			GiveTargetGuildName[SIZE_MAX_GUILD_NAME];
	USHORT			GiveTargetUnitKind;
	BYTE			GiveTargetLevel;
};

struct SGIVE_TARGET_CHARACTER
{// 2007-11-13 by cmkwon, 선물하기 기능 추가 - SGIVE_TARGET_CHARACTER 구조체 추가
	char			AccountName0[SIZE_MAX_ACCOUNT_NAME];
	UID32_t			AccountUID0;
	INT				ConnectingServerGroupID;
	INT				MembershipItemNum;
	ATUM_DATE_TIME	MembershipExpireTime;
	char			CharacterName0[SIZE_MAX_CHARACTER_NAME];
	UID32_t			CharacterUID0;
	BYTE			InfluenceType0;
	char			GuildName0[SIZE_MAX_GUILD_NAME];
	USHORT			UnitKind0;
	BYTE			Level0;
	INT				BuyItemNum;								// 구매 할 ItemNum
};


///////////////////////////////////////////////////////////////////////////////
// FC_TRADE
typedef struct
{
	char			TradeTarget[SIZE_MAX_CHARACTER_NAME];	// 피요청자
} MSG_FC_TRADE_REQUEST_TRADE;

typedef struct
{
	char			TradeSource[SIZE_MAX_CHARACTER_NAME];	// 요청자
	UID32_t			TradeSourceCharacterUniqueNumber;		// 요청자의 character unique number
} MSG_FC_TRADE_REQUEST_TRADE_OK;

typedef struct
{
	char			TradeTarget[SIZE_MAX_CHARACTER_NAME];	// 피요청자
} MSG_FC_TRADE_CANCEL_REQUEST;

typedef struct
{
	char			TradeSource[SIZE_MAX_CHARACTER_NAME];	// 요청자
	UID32_t			TradeSourceCharacterUniqueNumber;		// 요청자의 character unique number
} MSG_FC_TRADE_CANCEL_REQUEST_OK;

typedef struct
{
	UID32_t			TradeSourceCharacterUniqueNumber;		// 요청자의 character unique number
} MSG_FC_TRADE_ACCEPT_TRADE;

typedef struct
{
	char			TradeTarget[SIZE_MAX_CHARACTER_NAME];	// 피요청자
	UID32_t			TradeTargetCharacterUniqueNumber;		// 피요청자의 character unique number
} MSG_FC_TRADE_ACCEPT_TRADE_OK;

typedef struct
{
	UID32_t			TradeSourceCharacterUniqueNumber;		// 요청자의 character unique number
} MSG_FC_TRADE_REJECT_TRADE;

typedef struct
{
	char			TradeTarget[SIZE_MAX_CHARACTER_NAME];	// 피요청자
	UID32_t			TradeTargetCharacterUniqueNumber;		// 피요청자의 character unique number
} MSG_FC_TRADE_REJECT_TRADE_OK;

typedef struct
{
	char			TradeTarget[SIZE_MAX_CHARACTER_NAME];	// 피요청자
} MSG_FC_TRADE_REJECT_TRADING;								// 거래중이므로 거래를 할 수가 없다

typedef struct
{
	UID32_t			PeerTradeCharacterUniqueNumber;			// 거래 상대의 character unique number
} MSG_FC_TRADE_SHOW_TRADE_WINDOW;
#ifdef BONUS_STAT_ITEM
typedef struct
{
	UID64_t			ItemUniqueNumber;
	BOOL			WichVector;
} MSG_FC_TRADE_GET_BONUS_ITEM;

typedef struct
{
	UID64_t			ItemUniqueNumber;
	BONUS			AddingBonus[7];
	BOOL			MyVect;
} MSG_FC_TRADE_SEND_BONUS_ITEM;
#endif
typedef struct
{
	UID32_t			CharacterUniqueNumber;					// 아이템 올린 사람
	UID64_t			ItemUniqueNumber;						// 올린 아이템
	INT				ItemNum;								// 아이템 종류
	INT				Amount;									// 올린 개수(에너지류만 해당, 나머지는 1)
} MSG_FC_TRADE_TRANS_ITEM;

typedef struct
{
	UID32_t			CharacterUniqueNumber;					// 아이템 올린 사람
	ITEM_GENERAL	TradeItem;								// 이 아이템의 count는 최종 개수
	INT				NumOfEnchants;							// 아이템의 총 인챈트 개수
#ifdef BONUS_STAT_ITEM
	BONUS			AddingBonus[7];
#endif
	ARRAY_(INT);											// EnchantItemNum의 Array
} MSG_FC_TRADE_TRANS_ITEM_OK;

typedef struct
{
	UID32_t			CharacterUniqueNumber;					// 아이템 올린 사람
	UID64_t			ItemUniqueNumber;						// 올린 아이템
	INT				ItemNum;								// 아이템 종류
	INT				Amount;									// 올린 개수(countable item은 최종 개수, 나머지는 1)
} MSG_FC_TRADE_SEE_ITEM;

typedef struct
{
	UID32_t			CharacterUniqueNumber;					// 아이템 올린 사람
#ifdef BONUS_STAT_ITEM
	BONUS			AddingBonus[7];
#endif
	STRUCT_(ITEM_XXX);										// 이 아이템의 count는 최종 개수
} MSG_FC_TRADE_SEE_ITEM_OK;

typedef struct
{
	UID32_t			CharacterUniqueNumber;					// 자신
} MSG_FC_TRADE_OK_TRADE;

typedef struct
{
	UID32_t			CharacterUniqueNumber;					// 상대방
} MSG_FC_TRADE_OK_TRADE_OK;

typedef struct
{
	UID32_t			CharacterUniqueNumber;					// 상대방
} MSG_FC_TRADE_CANCEL_TRADE;

typedef struct
{
	UID32_t			CharacterUniqueNumber;					// 상대방
} MSG_FC_TRADE_CANCEL_TRADE_OK;

typedef MSG_FC_STORE_INSERT_ITEM		MSG_FC_TRADE_INSERT_ITEM;
typedef MSG_FC_STORE_DELETE_ITEM		MSG_FC_TRADE_DELETE_ITEM;
typedef MSG_FC_STORE_UPDATE_ITEM_COUNT	MSG_FC_TRADE_UPDATE_ITEM_COUNT;

struct MSG_FC_TRADE_OK_TRADE_NOTIFY		// 2008-11-21 by cmkwon, 거래 승인 확인 시스템 구현 - 
{
	UID32_t			CharacterUniqueNumber;					// 거래 승인 버튼을 실행한 캐릭터
};

typedef enum
{
	COUNTDOWN_TYPE_P2P_WAR		= 0,
	COUNTDOWN_TYPE_PARTY_WAR	= 1,
	COUNTDOWN_TYPE_GUILD_WAR	= 2,
	COUNTDOWN_TYPE_RACING		= 3,
	COUNTDOWN_TYPE_INVALID		= 4
} COUNTDOWN_TYPE;
// T0_FC_COUNTDOWN
typedef struct
{
	int		nWaitingSecond;
	BYTE	byCountdownType;
} MSG_FC_COUNTDOWN_START;

typedef struct
{
	BYTE	byCountdownType;
} MSG_FC_COUNTDOWN_DONE;


// T0_FC_OBJECT
typedef struct
{
	INT			nObjectCodeNum;
	AVECTOR3	AVec3Position;
	BodyCond_t	bodyCondition;
} MSG_FC_OBJECT_CHANGE_BODYCONDITION;

typedef MSG_FC_OBJECT_CHANGE_BODYCONDITION		MSG_FC_OBJECT_CHANGE_BODYCONDITION_OK;

// 2010-06-15 by shcho&hslee 펫시스템 - 거래 시 펫 정보 전송 패킷 추가
typedef struct
{
	UID64_t			CreatedPetUID;
#ifndef _INET_PET
	BOOL			EnableEditPetName;
	BOOL			EnableLevelUp;
	char			szPetName[SIZE_MAX_PET_NAME];
#else
	char			PetName[SIZE_MAX_PET_NAME];
#endif
	
	
	INT				PetIndex;
	INT				PetLevel;
	
	Experience_t	PetExp;

} MSG_TRADE_PET_DATA;
// end 2010-06-15 by shcho&hslee 펫시스템 - 거래 시 펫 정보 전송 패킷 추가

///////////////////////////////////////////////////////////////////////////////
// FC_AUCTION
typedef struct
{
	UID32_t		ItemUID;
	INT			Price;
} MSG_FC_AUCTION_REGISTER_ITEM;			// C->F, 경매 아이템 등록

typedef struct
{
	UID32_t		ItemUID;
	INT			Price;
} MSG_FC_AUCTION_REGISTER_ITEM_OK;		// F->C, 경매 아이템 등록 결과

typedef struct
{
	UID32_t		ItemUID;
} MSG_FC_AUCTION_CANCEL_REGISTER;		// C->F, 경매 아이템 등록 취소

typedef struct
{
	UID32_t		ItemUID;
} MSG_FC_AUCTION_CANCEL_REGISTER_OK;	// F->C, 경매 아이템 등록 취소 결과

typedef struct
{
	UID32_t		ItemUID;
} MSG_FC_AUCTION_BUY_ITEM;				// C->F, 경매 아이템 구매

typedef struct
{
	UID32_t		ItemUID;
} MSG_FC_AUCTION_BUY_ITEM_OK;			// F->C, 경매 아이템 구매 결과

typedef struct
{
	UID32_t		CharacterUID;			// 자신이 등록한 경매 아이템을 요청할 때는 자신의 CharacterUID, 아니면 0
	BYTE		AuctionKind;			// 요청할 경매 아이템 종류, AUCTION_KIND_XXX
} MSG_FC_AUCTION_GET_ITEM_LIST;			// C->F, 경매 아이템 목록 요청

// AUCTION_KIND_XXX
#define AUCTION_KIND_MY_ITEM	((BYTE)0)	// 자신이 등록한 아이템
#define AUCTION_KIND_ATTACH		((BYTE)1)	// 장착류 0 ~ 17, 22, 25 ~ 26
#define AUCTION_KIND_CARD		((BYTE)2)	// 카드류 21, 27
#define AUCTION_KIND_ETC		((BYTE)3)	// 기타류 18 ~ 20, 23 ~ 24

#define IS_AUCTION_KIND_ATTACH(_ITEM_KIND)	\
	(IS_WEAPON(_ITEM_KIND)					\
	|| _ITEM_KIND == ITEMKIND_DEFENSE		\
	|| _ITEM_KIND == ITEMKIND_SUPPORT		\
	|| _ITEM_KIND == ITEMKIND_TANK			\
	|| _ITEM_KIND == ITEMKIND_RADAR			\
	|| _ITEM_KIND == ITEMKIND_COMPUTER)
#define IS_AUCTION_KIND_CARD(_ITEM_KIND)	\
	(_ITEM_KIND == ITEMKIND_ENCHANT			\
	|| _ITEM_KIND == ITEMKIND_GAMBLE)
#define IS_AUCTION_KIND_ETC(_ITEM_KIND)		\
	(_ITEM_KIND == ITEMKIND_ENERGY			\
	|| _ITEM_KIND == ITEMKIND_INGOT			\
	|| _ITEM_KIND == ITEMKIND_CARD			\
	|| _ITEM_KIND == ITEMKIND_BULLET		\
	|| _ITEM_KIND == ITEMKIND_QUEST)

typedef struct
{
	BYTE			AuctionKind;		// AUCTION_KIND_XXX
	char			SellerCharacterName[SIZE_MAX_CHARACTER_NAME];
	ITEM_GENERAL	AuctionItemGeneral;	// 경매 아이템
	int				AuctionPrice;		// 경매 가격
	ATUM_DATE_TIME	AuctionStartDate;	// 경매 시작 시간
} MSG_FC_AUCTION_INSERT_ITEM;			// F->C, 경매 아이템 전송용

typedef struct
{
	BYTE			AuctionKind;		// AUCTION_KIND_XXX
	ENCHANT			AuctionItemEnchant;
} MSG_FC_AUCTION_PUT_ENCHANT;			// F->C, 경매 아이템의 인챈트 정보 전송용

///////////////////////////////////////////////////////////////////////////////
// FC_SKILL
typedef struct
{
	UINT		KindOfSkill;
} MSG_FC_SKILL_USE_SKILL_POINT;

typedef struct
{
	UINT		KindOfSkill;
	BYTE		UsedPoints;
	BYTE		RemainedPoints;
} MSG_FC_SKILL_USE_SKILL_POINT_OK;

typedef struct
{
	INT			ItemNum;
} MSG_FC_SKILL_SETUP_SKILL;

typedef struct
{
	BYTE		ItemUpdateType;		// IUT_SHOP, IUT_LOADING, IUT_SKILL
	ITEM_SKILL	ItemSkill;
} MSG_FC_SKILL_SETUP_SKILL_OK;

typedef struct 
{
	ItemID_t		SkillItemID;
	ClientIndex_t	AttackIndex;
	ClientIndex_t	TargetIndex;	// target이 없는 skill인 경우 0
	UID32_t			TargetCharUID;	// 2005-11-24 by cmkwon, target있지만 TargetIndex가 0일때 사용됨, TargetIndex가 편대원 1명 소환 스킬시 사용한다.
} MSG_FC_SKILL_USE_SKILL;

typedef struct
{
	ItemID_t		SkillItemID;
	ClientIndex_t	AttackIndex;
	ClientIndex_t	TargetIndex;	// target이 없는 skill인 경우 0
	ATUM_DATE_TIME	UseTime;		// 2006-11-17 by dhjin, 2차 스킬 사용 시간
//	BOOL			UseSkillTimeOk;	// 2006-11-17 by dhjin, 0 -> 2차 스킬 사용 대기 시간, 1 -> 2차 스킬 사용할 수 있다.
} MSG_FC_SKILL_USE_SKILL_OK;

typedef struct
{
	ItemID_t		SkillItemID;			// 종료되는 스킬 정보
	INT				AttackSkillItemNum0;	// 2006-12-12 by cmkwon, 현재 스킬을 종료되게 하는 공격스킬 아이템넘버 
} MSG_FC_SKILL_CANCEL_SKILL;

typedef struct
{
	ClientIndex_t	ClientIndex;
	ItemID_t		SkillItemID;
	INT				AttackSkillItemNum0;	// 2006-12-12 by cmkwon, 현재 스킬을 종료되게 하는 공격스킬 아이템넘버
} MSG_FC_SKILL_CANCEL_SKILL_OK;

typedef struct
{
	ItemID_t		SkillItemID;
	ClientIndex_t	ClientIndex;
} MSG_FC_SKILL_INVALIDATE_SKILL;	// skill 사용 중지(시간 제한이 있을 경우)

typedef struct
{
	ItemID_t		SkillItemID;
} MSG_FC_SKILL_PREPARE_USE;

typedef struct
{
	ClientIndex_t	ClientIndex;
	ItemID_t		SkillItemID;
} MSG_FC_SKILL_PREPARE_USE_OK;

typedef struct 
{
	ItemID_t		SkillItemID;
} MSG_FC_SKILL_CANCEL_PREPARE;

typedef struct
{
	ClientIndex_t	ClientIndex;
	ItemID_t		SkillItemID;
} MSG_FC_SKILL_CANCEL_PREPARE_OK;

struct MSG_FC_SKILL_CONFIRM_USE			// 2005-12-02 by cmkwon
{
	char				szAttackCharacterName[SIZE_MAX_CHARACTER_NAME];	// 스킬 사용자의 CharacterName;
	UID32_t				AttackCharacterUID;		// 스킬 사용자 CharacterUID
	UID32_t				TargetCharacterUID;		// 스킬 타겟 CharacterUID
	int					UsingSkillItemNum;		// 사용 스킬 ItemNum
	MAP_CHANNEL_INDEX	MapChannelIndex;		// 스킬 사용자의 MapChannelIndex
	int					SkillConfirmUseUID;		// 2009-04-06 by cmkwon, 콜오브 히어로 스킬 시스템 변경 - 해당 요청의 UID
};

struct MSG_FC_SKILL_CONFIRM_USE_ACK		// 2005-12-02 by cmkwon
{
	BOOL				bYesOrNo;				// 수락 여부
	UID32_t				AttackCharacterUID;		// 스킬 사용자 CharacterUID
	UID32_t				TargetCharacterUID;		// 스킬 타겟 CharacterUID
	int					UsingSkillItemNum;		// 사용 스킬 ItemNum
	int					SkillConfirmUseUID;		// 2009-04-06 by cmkwon, 콜오브 히어로 스킬 시스템 변경 - 해당 요청의 UID
};



///////////////////////////////////////////////////////////////////////////////
// FN_SKILL
typedef MSG_FC_SKILL_USE_SKILL				MSG_FN_SKILL_USE_SKILL;
typedef struct _MSG_FN_SKILL_USE_SKILL_OK : public MSG_FC_SKILL_USE_SKILL_OK
{
	ChannelIndex_t		ChannelIndex;
} MSG_FN_SKILL_USE_SKILL_OK;

///////////////////////////////////////////////////////////////////////////////
// FC_SYNC
//typedef struct
//{
//	INT		ItemNum;
//} MSG_FC_SYNC_SKILL_REUSE_OK;

///////////////////////////////////////////////////////////////////////////////
// FC_INFO
typedef struct
{
	INT				MonsterUnitKind;	// 몬스터 고유 번호
} MSG_FC_INFO_GET_MONSTER_INFO;

struct MEX_MONSTER_INFO
{
	INT				MonsterUnitKind;					// 몬스터 고유번호
	char			MonsterName[SIZE_MAX_MONSTER_NAME];	// 몬스터 이름
	BYTE			Level;								// 몬스터의 Level
	INT				HP;									// 만피
	BYTE			Size;								// 필드 서버는 SizeForClient를 loading,  NPC 서버는 SizeForServer를 loading
	BYTE			Belligerence;						// 호전성, // 2005-12-28 by cmkwon 추가함
	BYTE			AlphaBlending;						// alpha blending 여부, TRUE(1), FALSE(0), client측 사용을 위해 추가, 20030616
	USHORT			RenderIndex;
	float			ScaleValue;
	BYTE			TextureIndex;
	UINT			SourceIndex;
	BitFlag64_t		MPOption;			// 2010-01-11 by cmkwon, 몬스터 MPOption 64bit로 변경 - 기존(BYTE)
	BYTE			ClickEvent;							// 2007-09-05 by dhjin, 몬스터 클릭 이벤트 추가
	char			PortraitFileName[SIZE_MAX_FILE_NAME];	// 2010-03-31 by dhjin, 인피니티(기지방어) - 몬스터 초상화 파일

	// operator overloading
	MEX_MONSTER_INFO& operator=(const MONSTER_INFO& rhs)
	{
		this->MonsterUnitKind	= rhs.MonsterUnitKind;
		STRNCPY_MEMSET(this->MonsterName, rhs.MonsterName, SIZE_MAX_MONSTER_NAME);
		this->Level				= rhs.Level;
		this->HP				= rhs.MonsterHP;
		this->Size				= rhs.Size;
		this->Belligerence		= rhs.Belligerence;				// 호전성
		this->AlphaBlending		= rhs.AlphaBlending;
		this->RenderIndex		= rhs.RenderIndex;
		this->ScaleValue		= rhs.ScaleValue;
		this->TextureIndex		= rhs.TextureIndex;
		this->SourceIndex		= rhs.SourceIndex;
		this->MPOption			= rhs.MPOption;
		this->ClickEvent		= rhs.ClickEvent;				// 2007-09-05 by dhjin
		STRNCPY_MEMSET(this->PortraitFileName, rhs.PortraitFileName, SIZE_MAX_FILE_NAME);	// 2010-03-31 by dhjin, 인피니티(기지방어) - 몬스터 초상화 파일

		return *this;
	}
};

typedef struct
{
	MEX_MONSTER_INFO	MonsterInfo;
} MSG_FC_INFO_GET_MONSTER_INFO_OK;

typedef struct
{
	INT				Code;
} MSG_FC_INFO_GET_MAPOBJECT_INFO;

typedef struct
{
	MAPOBJECTINFO	MapObjectInfo;
} MSG_FC_INFO_GET_MAPOBJECT_INFO_OK;

typedef struct
{
	INT				ItemNum;
} MSG_FC_INFO_GET_ITEM_INFO;

typedef struct
{
	ITEM			ItemInfo;
} MSG_FC_INFO_GET_ITEM_INFO_OK;
#ifdef _INET_ANTICHEAT
typedef struct
{
	ITEM			ItemInfo;
} MSG_FC_INET_GET_ITEM_INFO;
#endif

#ifdef _INET_ANTICHEAT
typedef struct
{
	int			nINET_hacktype;
} MSG_FC_INET_GET_ITEM_INFO_OK;
#endif
typedef struct
{
	INT				CodeNum;
} MSG_FC_INFO_GET_RARE_ITEM_INFO;

typedef struct
{
	RARE_ITEM_INFO	RareItemInfo;
} MSG_FC_INFO_GET_RARE_ITEM_INFO_OK;

typedef struct
{
	INT				BuildingIndex;
} MSG_FC_INFO_GET_BUILDINGNPC_INFO;

typedef struct
{
	BUILDINGNPC		BuildingNPCInfo;
} MSG_FC_INFO_GET_BUILDINGNPC_INFO_OK;

typedef struct
{
	INT				ItemNum;
} MSG_FC_INFO_GET_SIMPLE_ITEM_INFO;

struct MEX_ITEM_INFO
{
	
	
	float		AbilityMax;						// 아이템최대성능
	float		ArrParameterValue[SIZE_MAX_DESPARAM_COUNT_IN_ITEM];	// 2009-04-21 by cmkwon, ITEM에 DesParam 필드 개수 8개로 늘리기 - 
	BYTE		Defense;						// 방어력
	BYTE		SpeedPenalty;					// 스피드페널티, 이동속도에미치는 영향(-:감소)
	USHORT		ReqUnitKind;					// 필요유닛종류
	BYTE		ReqMinLevel;					// 필요 최저 레벨
	INT			ItemNum;						// 아이템 고유번호, 장착 아이템일 때 (ITEM_BASE*)
	float		AbilityMin;						// 아이템최소성능
	BYTE		Kind;							// 아이템 종류(기관포, 빔, 로켓, 스킬.....), ITEMKIND_XXX
	BYTE		ReqMaxLevel;					// 필요 최저 레벨
	float		HitRate;						// 명중확률(0~255), // 2010-07-19 by dhjin, 확률 수식 변경
	
	USHORT		Range;							// 공격범위, 엔진류인 경우에는 부스터 가동 시 속도
	UINT		Price;							// 최소 거래 수량의 가격
	BitFlag64_t	ItemAttribute;					// 아이템의 속성, ITEM_ATTR_XXX
	FLOAT		BoosterAngle;					// 부스터시에 유닛의 회전각, 현재는 엔진에만 사용
	UINT		CashPrice;						// 최소 거래 수량의 현금 가격
// 2009-04-21 by cmkwon, ITEM에 DesParam 필드 개수 8개로 늘리기 - 
// 	BYTE		DestParameter1;					// 대상파라미터1
// 	float		ParameterValue1;				// 수정파라미터1
// 	BYTE		DestParameter2;					// 대상파라미터2
// 	float		ParameterValue2;				// 수정파라미터2
// 	BYTE		DestParameter3;					// 대상파라미터3
// 	float		ParameterValue3;				// 수정파라미터3
// 	BYTE		DestParameter4;					// 대상파라미터4
// 	float		ParameterValue4;				// 수정파라미터4
	float		RangeAngle;						// 범위각도(0 ~ PI)
	BYTE		MultiTarget;					// 동시에 잡을 수 있는 타겟의 수
#ifdef _INET_PET
	DestParam_t		ArrDestParameter[SIZE_MAX_DESPARAM_COUNT_IN_ITEM];
#else
	BYTE		ArrDestParameter[SIZE_MAX_DESPARAM_COUNT_IN_ITEM];	// 2009-04-21 by cmkwon, ITEM에 DesParam 필드 개수 8개로 늘리기 - 
#endif
	

	UINT		ReAttacktime;					// 재 공격시간(ms)
	USHORT		AttackTime;						// 공격시간, 공격을 하기 위해 필요한 시간
	BYTE		ReqSP;							// SP 소모량(스킬)
	BYTE		OrbitType;						// 미사일, 로켓 등의 궤적
	INT			Time;							// 지속 시간(스킬류 등)
	USHORT		RepeatTime;						// 2006-12-08 by cmkwon, 추가함(무기류에서는 남은 총알 수로 사용, 나머지는 개수, 시간형 스킬류에선 남은 시간, 나머지 스킬은 사용 여부)
	
	USHORT		ExplosionRange;					// 폭발반경(폭발 시 데미지의 영향이 미치는 반경)
	USHORT		ReactionRange;					// 반응반경(마인 등이 반응하는 반경)
	BYTE		ShotNum;						// 점사 수,	점사 시 발사 수를 나타낸다.
	BYTE		MultiNum;						// 동시 발사 탄 수,	1번 발사에 몇발이 동시에 나가느냐
	
	

	// operator overloading
	MEX_ITEM_INFO& operator=(const ITEM& rhs);
};
typedef struct
{
	MEX_ITEM_INFO	ItemInfo;
} MSG_FC_INFO_GET_SIMPLE_ITEM_INFO_OK;
typedef struct
{
	INT				EnchantItemNum;
} MSG_FC_INFO_GET_ENCHANT_COST;		// C->F, 인챈트 가격을 요청
typedef struct
{
	INT				Cost;
} MSG_FC_INFO_GET_ENCHANT_COST_OK;	// F->C, 인챈트 가격을 전송
#ifdef _INET_ENCHANT_CHANCE
typedef struct
{
	INT				EnchantItemNum;
	INT				UniqueNumber;
	INT				IncreaseValue;
} MSG_FC_INFO_GET_ENCHANT_CHANCE;
typedef struct
{
	Prob10K_t				Prob;
} MSG_FC_INFO_GET_ENCHANT_CHANCE_OK;
#endif
struct MSG_FC_INFO_GET_CURRENT_MAP_INFO		// 2007-04-06 by cmkwon, 추가함
{
	MAP_CHANNEL_INDEX	mapChannelIdx0;			// 2007-04-06 by cmkwon
	// 2009-05-29 by cmkwon, Hash알고리즘 추가(SHA256) - 
	//UINT				checkSum0;				// 2007-04-06 by cmkwon, map이 checksum 결과
	int					nFileSize;			// 2009-05-29 by cmkwon, Hash알고리즘 추가(SHA256) - 
	BYTE				byDigest[32];		// 2009-05-29 by cmkwon, Hash알고리즘 추가(SHA256) - 
};

struct MSG_FC_INFO_GET_CURRENT_MAP_INFO_OK
{
	BYTE			IsPKMap;				// 2005-02-15 by cmkwon, 아직 사용하지 않음
	BYTE			IsCityWarStarted;		// 도시점령전 시작 플래그
	BYTE			byCityWarTeamType;		//
};

enum
{
	HAPPYEV_STATE_TYPE_END				= 0,			// 일반 해피아워 이벤트 종료됨
	HAPPYEV_STATE_TYPE_START			= 1,			// 일반 해피아워 이벤트 시작됨
	HAPPYEV_STATE_TYPE_STARTING			= 2,			// 일반 해피아워 이벤트 진행중
	PCBANG_HAPPYEV_STATE_TYPE_END		= 10,			// PCBang 해피아워 이벤트 종료됨
	PCBANG_HAPPYEV_STATE_TYPE_START		= 11,			// PCBang 해피아워 이벤트 시작됨
	PCBANG_HAPPYEV_STATE_TYPE_STARTING	= 12,			// PCBang 해피아워 이벤트 진행중
	GAME_EVENT_GROUP_MOTHERSHIP_END			= 20,		// 2008-05-20 by cmkwon, 모든 이벤트(HappyHoure,MotherShip,Item) 그룹 동시에 가능하게 - 
	GAME_EVENT_GROUP_MOTHERSHIP_START		= 21,		// 2008-05-20 by cmkwon, 모든 이벤트(HappyHoure,MotherShip,Item) 그룹 동시에 가능하게 - 
	GAME_EVENT_GROUP_MOTHERSHIP_STARTING	= 22,		// 2008-05-20 by cmkwon, 모든 이벤트(HappyHoure,MotherShip,Item) 그룹 동시에 가능하게 - 
	GAME_EVENT_GROUP_ITEM_END				= 30,		// 2008-05-20 by cmkwon, 모든 이벤트(HappyHoure,MotherShip,Item) 그룹 동시에 가능하게 - 
	GAME_EVENT_GROUP_ITEM_START				= 31,		// 2008-05-20 by cmkwon, 모든 이벤트(HappyHoure,MotherShip,Item) 그룹 동시에 가능하게 - 
	GAME_EVENT_GROUP_ITEM_STARTING			= 32		// 2008-05-20 by cmkwon, 모든 이벤트(HappyHoure,MotherShip,Item) 그룹 동시에 가능하게 - 

	,GAME_EVENT_GROUP_SP_END = 40,		// 26-02-2024 by Inet - added sp hh category to dont stop damn lead hh
	GAME_EVENT_GROUP_SP_START = 41,		// 26-02-2024 by Inet - added sp hh category to dont stop damn lead hh
	GAME_EVENT_GROUP_SP_STARTING = 42		// 26-02-2024 by Inet - added sp hh category to dont stop damn lead hh

};
struct MSG_FC_INFO_GET_HAPPY_HOUR_EVENT_INFO_OK
{
	BYTE			byStateType;			// 0(종료됨), 1(시작됨), 2(진행중)
	BYTE			byInfluenceType4;		// 2007-10-30 by cmkwon, 세력별 해피아워 이벤트 구현 - MSG_FC_INFO_GET_HAPPY_HOUR_EVENT_INFO_OK 구조체에 추가된 필드
	ATUM_DATE_TIME	StartATime;
	ATUM_DATE_TIME	EndATime;
	float			fEXPRate2;
	float			fSPIRate2;
	float			fEXPRepairRate2;
	float			fDropItemRate2;
	float			fDropRareRate2;
	float			fWarPointRate2;			// 2007-06-26 by dhjin, 워포인트 이벤트 관련 추가
	float			fEventMonster;
};
struct MSG_FC_INFO_GET_GAME_EVENT_INFO_OK
{
	BYTE			byStateType;			// 0(종료됨), 1(시작됨), 2(진행중)
	int				nGameEventType;			// 이벤트 타입
	float			fRate;					// 이벤트 Rate
	int				nRemainMinute;			// 남은 시간(단위:분)
};

struct MSG_FC_INFO_GET_SERVER_DATE_TIME_OK		// 2006-10-11 by cmkwon
{
	ATUM_DATE_TIME	atimeCurServerTime;		// 2006-10-11 by cmkwon, 현재 서버 날짜 시간
};

struct MSG_FC_INFO_CHECK_RESOBJ_CHECKSUM		// 2007-05-28 by cmkwon
{
	int			nItemNum0;
	char		szObjFileName[SIZE_MAX_RESOBJ_FILE_NAME];
	// 2009-05-29 by cmkwon, Hash알고리즘 추가(SHA256) - 
	//UINT		uiCheckSum0;
	int			nFileSize0;
	BYTE		byDigest[32];		// 2009-05-29 by cmkwon, Hash알고리즘 추가(SHA256) - 
};


///////////////////////////////////////////////////////////////////////////////
// FC_REQUEST - 캐릭터간의 요청, 수락, 거절 등에 쓰임, general-purpose
// REQUEST_TYPE_XXX
#define REQUEST_TYPE_NULL			((BYTE)0)
#define REQUEST_TYPE_TRADE			((BYTE)1)	// 거래
#define REQUEST_TYPE_PARTY_INVITE	((BYTE)2)	// 파티 초대
#define REQUEST_TYPE_GUILD_INVITE	((BYTE)3)	// 드 초대
#define REQUEST_TYPE_P2P_PK			((BYTE)4)	// 1대 1 결투
#define REQUEST_TYPE_PARTY_BATTLE	((BYTE)5)	// 파티간 결투
#define REQUEST_TYPE_GUILD_WAR		((BYTE)6)	// 여단전 신청, 같은 맵에만 있으면 가능

typedef struct
{
	ClientIndex_t	TargetClientIndex;	// 피요청자
	char			TargetCharacterName[SIZE_MAX_CHARACTER_NAME];	// ClientIndex가 0인 경우 이 필드 참조
	BYTE			RequestType;
} MSG_FC_REQUEST_REQUEST;				// C->F, 요청

typedef struct
{
	ClientIndex_t	SourceClientIndex;	// 요청자
	char			SourceCharacterName[SIZE_MAX_CHARACTER_NAME];	// ClientIndex가 0인 경우 이 필드 참조
	BYTE			RequestType;
} MSG_FC_REQUEST_REQUEST_OK;			// F->C, 요청을 전달

typedef struct
{
	ClientIndex_t	SourceClientIndex;	// 요청자
	char			SourceCharacterName[SIZE_MAX_CHARACTER_NAME];	// ClientIndex가 0인 경우 이 필드 참조
	BYTE			RequestType;
} MSG_FC_REQUEST_ACCEPT_REQUEST;		// C->F, 승낙

typedef struct
{
	ClientIndex_t	PeerClientIndex;	// 상대방
	char			PeerCharacterName[SIZE_MAX_CHARACTER_NAME];	// ClientIndex가 0인 경우 이 필드 참조
	BYTE			RequestType;
} MSG_FC_REQUEST_ACCEPT_REQUEST_OK;		// F->C, 승낙을 전달, 양(혹은 한) 쪽으로 보냄

typedef struct
{
	ClientIndex_t	SourceClientIndex;	// 요청자
	char			SourceCharacterName[SIZE_MAX_CHARACTER_NAME];	// ClientIndex가 0인 경우 이 필드 참조
	BYTE			RequestType;
} MSG_FC_REQUEST_REJECT_REQUEST;		// C->F, 거절

typedef struct
{
	ClientIndex_t	PeerClientIndex;	// 상대방
	char			PeerCharacterName[SIZE_MAX_CHARACTER_NAME];	// ClientIndex가 0인 경우 이 필드 참조
	BYTE			RequestType;
} MSG_FC_REQUEST_REJECT_REQUEST_OK;		// F->C, 거절을 전달, 양(혹은 한) 쪽으로 보냄

typedef struct
{
	ClientIndex_t	PeerClientIndex;	// 상대방
	char			PeerCharacterName[SIZE_MAX_CHARACTER_NAME];	// ClientIndex가 0인 경우 이 필드 참조
	BYTE			RequestType;
} MSG_FC_REQUEST_CANCEL_REQUEST;		// C->F, 요청 취소됨, 양(혹은 한) 쪽으로 보냄

///////////////////////////////////////////////////////////////////////////////
// FC_CITY - 도시용 프로토콜
typedef struct
{
	MAP_CHANNEL_INDEX	MapChannelIndex;
} MSG_FC_CITY_GET_BUILDING_LIST;

struct CITY_BUILDING_INFO_4_EXCHANGE
{
	UINT	BuildingIndex;	// 건물(가게 등) 고유 번호
	BYTE	BuildingKind;
};

typedef struct
{
	INT		NumOfBuildings;
	ARRAY_(CITY_BUILDING_INFO_4_EXCHANGE);
} MSG_FC_CITY_GET_BUILDING_LIST_OK;

typedef struct
{
	MAP_CHANNEL_INDEX	MapChannelIndex;
} MSG_FC_CITY_GET_WARP_TARGET_MAP_LIST;

// 2004-11-10 by cmkwon, 위에서 선언함
//struct WARP_TARGET_MAP_INFO_4_EXCHANGE
//{
//	MapIndex_t	MapIndex;
//	INT			TargetIndex;	// EVENTINFO에 존재하는 TargetIndex
//	char		TargetName[SIZE_MAX_WARP_TARGET_NAME];	// 타켓의 이름(ex: 도시상단, 도시하단)
//	INT			Fee;			// 워프 요금
//};

typedef struct
{
	INT		NumOfTargetMaps;
	ARRAY_(WARP_TARGET_MAP_INFO_4_EXCHANGE);
} MSG_FC_CITY_GET_WARP_TARGET_MAP_LIST_OK;

typedef struct
{
	INT		BuildingIndex;
} MSG_FC_CITY_REQUEST_ENTER_BUILDING;	// C->F, 상점 들어갈 것을 요청

typedef struct
{
	INT		BuildingIndex;
// 2006-02-08 by cmkwon
//	float	fCityWarTexRate;			//
	float	fInflDistributionTexPercent;	// 2006-02-08 by cmkwon, 세력분포 세금
} MSG_FC_CITY_REQUEST_ENTER_BUILDING_OK;	// F->C, 상점 진입 완료를 알림

typedef struct
{
	MapIndex_t	MapIndex;
	INT			TargetIndex;			// EVENTINFO에 존재하는 TargetIndex	
} MSG_FC_CITY_REQUEST_WARP;				// C->F, 도시에서 워프해서 나가기 위한 요청

typedef struct
{
	Err_t		Err;				// ERR_NO_ERROR이면 성공
} MSG_FC_CITY_CHECK_WARP_STATE_OK;	// F->C, 워프 가능한 상태인지에 대한 결과

///////////////////////////////////////////////////////////////////////////////
// FC_TIMER - 타이머 관리용 프로토콜
///////////////////////////////////////////////////////////////////////////////

// TimerEventType 의 종류, TE_TYPE_XXX
#define TE_TYPE_TEST					(TimerEventType)0
//#define TE_TYPE_SYNC_PRI				(TimerEventType)1
//#define TE_TYPE_SYNC_SEC				(TimerEventType)2
//#define TE_TYPE_SYNC_SKILL			(TimerEventType)3
//#define TE_TYPE_END_SKILL				(TimerEventType)4		// check: 클라이언트의 요청에 의해 삭제함, 해당 시간 경과 후 클라이언트에서 CANCEL_SKILL 전송, 20041005, kelovon with jschoi
//#define TE_TYPE_DELETE_MINE			(TimerEventType)5	// check: 마인 지우기는 클라이언트에서 모두 관리하기로 하면서 제거함, 20041118, kelovon with dhkwon
#define TE_TYPE_DELETE_DUMMY			(TimerEventType)6
#define TE_TYPE_DELETE_FIXER			(TimerEventType)7
#define TE_TYPE_DELETE_DECOY			(TimerEventType)8		// 디코이는 TimeOut발생 시 CurrentCount를 줄이고, CurrentCount=0까지 반복함
#define TE_TYPE_GRADUAL_SHIELD_UP		(TimerEventType)9		// 2-2형 무기 쉴드, interval은 무조건 1000 ms
#define TE_TYPE_RECOVER_HP				(TimerEventType)10		// 착륙했을 때만 채움(5초당 3)
#define TE_TYPE_RECOVER_DP				(TimerEventType)11		// 착륙했을 때만 채움
#define TE_TYPE_RECOVER_SP				(TimerEventType)12
#define TE_TYPE_GRADUAL_HP_UP			(TimerEventType)13		// interval은 무조건 1000 ms, ITEM_GENERAL.Endurance가 0이 될 때까지 반복함
#define TE_TYPE_GRADUAL_DP_UP			(TimerEventType)14		// interval은 무조건 1000 ms, ITEM_GENERAL.Endurance가 0이 될 때까지 반복함
#define TE_TYPE_GRADUAL_SP_UP			(TimerEventType)15		// interval은 무조건 1000 ms, ITEM_GENERAL.Endurance가 0이 될 때까지 반복함, not used yet: SP는 자동으로 회복됨, TE_TYPE_RECOVER_SP를 사용
#define TE_TYPE_GRADUAL_EP_UP			(TimerEventType)16		// interval은 무조건 1000 ms, ITEM_GENERAL.Endurance가 0이 될 때까지 반복함
#define TE_TYPE_IMMEDIATE_HP_UP			(TimerEventType)17		// 사용시 바로 채우고 Time 동안 재사용이 안된다
#define TE_TYPE_IMMEDIATE_DP_UP			(TimerEventType)18		// 사용시 바로 채우고 Time 동안 재사용이 안된다
#define TE_TYPE_IMMEDIATE_SP_UP			(TimerEventType)19		// 사용시 바로 채우고 Time 동안 재사용이 안된다
#define TE_TYPE_IMMEDIATE_EP_UP			(TimerEventType)20		// 사용시 바로 채우고 Time 동안 재사용이 안된다
#define TE_TYPE_REQUEST_TIMEOUT			(TimerEventType)21		// 거래, 파티전, 일대일대결 등의 요청에 대한 TimeOut
#define TE_TYPE_DECREASE_SP				(TimerEventType)22		// TOGGLE형 스킬 사용 시, SP를 ReqSP만큼 소모, SP 소진 시 스킬 자동 취소
#define TE_TYPE_DO_MINUTELY_WORK		(TimerEventType)23		// 1분에 한번씩 발행하는 타이머
////////////////////////////////////////////////////////////////////////////////
// 2009-09-09 ~ 2010 by dhjin, 인피니티 - 
#define TE_TYPE_DOTSKILL_STEALING		(TimerEventType)24		// Stealing 스킬 - SP감소 (SP가 소진 되어도 계속 적용 되어야 한다.)
#define TE_TYPE_DOTSKILL_DRAIN			(TimerEventType)25		// Drain 스킬 - 피격대상에 HP감소시켜 공격대상에게 채워준다.
#define TE_TYPE_DOTSKILL_TIME_BOMB		(TimerEventType)26		// TimeBomb 스킬 - 피 대상에게 시간 종료시 현재 피에 %로 데미지를 입힌다.
#define TIMER_DOT_INTERVAL					1000


#define TIMER_DECOY_TIME_CHECK_INTERVAL		1000
#define TIMER_GRADUAL_UP_INTERVAL			1000	// gradual up에 사용되는 interval, DELETE_DECOY에도 사용함
#define _DEFAULT_TIME_GRANULARITY			100		// milli-seconds
#define _MAX_TIMER_INTERVAL_TIME			150000	// maximum 150 seconds, check: 조정해야 함
#define SIZE_MAX_TIMER_EVENT_MEMPOOL		1000
#define _TIMER_TICK							_DEFAULT_TIME_GRANULARITY/3
#define TIMER_REPEAT_INFINITE				MAX_INT_VALUE
#define TIMER_DECREASE_SP_INTERVAL			1000				// 1초
#define TIMER_DO_MINUTELY_WORK				60*1000				// 1분

struct MEX_TIMER_EVENT
{
	TimerEventType		Type;
	UID32_t				TimerUID;				// 타이머 아이디
	TimeUnit_t			StartTimeStamp;			// milli-seconds
	TimeUnit_t			ExpireTime;				// milli-seconds
	TimeUnit_t			TimeInterval;			// milli-seconds, (ExpireTime - StartTimeStamp)
	UID32_t				CharacterUniqueNumber;	// event를 시작한 charac이 나가고 다른 charac이 socket을 사용하는 경우를 막기 위해, IsUsing()과 CharacterUniqueNumber가 같아야 함!
	ClientIndex_t		ClientIndex;			// event를 시작한 charac이 나가고 다른 charac이 socket을 사용하는 경우를 막기 위해, IsUsing()과 ClientIndex가 같아야 함!
	UINT				nGeneralParam1;			// end_skill: ITEM_SKILL* ,	mine: CMapBlock*,	delete_item류: ITEM_GENERAL*, GRADUAL_SHIELD_UP: HI(ItemUID), GRADUAL_XXX_UP: ITEM*, DELETE_DUMMY: FIELD_DUMMY*
	UINT				nGeneralParam2;			// end_skill: ,				mine: DROPMINE*,	delete_item류:              , GRADUAL_SHIELD_UP: LO(ItemUID)
	UINT				nGeneralParam3;			// end_skill: ,				mine: CFieldIOCP*,	delete_item류:              , GRADUAL_SHIELD_UP: ItemNum
	BOOL				bRepeatable;			// repeat해야 하는지 여부
	INT					nRemainedRepeatCount;	// 남은 횟수, ~0(0xFFFFFFFF): 무한 반복, 0이하: 정지, 1이상: 회수만큼 반복
};

typedef struct
{
	MEX_TIMER_EVENT		TimerEvent;
} MSG_FC_TIMER_START_TIMER;			// F->C, TIMER_EVENT 시작

typedef struct
{
	UID32_t				TimerUID;
} MSG_FC_TIMER_STOP_TIMER;			// F->C, TIMER_EVENT 정지

typedef struct
{
	MEX_TIMER_EVENT		TimerEvent;
} MSG_FC_TIMER_UPDATE_TIMER;		// F->C, TIMER_EVENT 갱신(시간 연장)

typedef struct
{
	UID32_t				TimerUID;
} MSG_FC_TIMER_PAUSE_TIMER;			// F->C, TIMER_EVENT 일시 정지

typedef struct
{
	UID32_t				TimerUID;
} MSG_FC_TIMER_CONTINUE_TIMER;		// F->C, TIMER_EVENT 재시작

typedef struct
{
	MEX_TIMER_EVENT		TimerEvent;
} MSG_FC_TIMER_TIMEOUT;				// C->F, 시간이 다 됨을 알림

///////////////////////////////////////////////////////////////////////////////
// FC_CLIENT_REPORT
typedef struct
{