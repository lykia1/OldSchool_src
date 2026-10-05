// 2005-04-28 by cmkwon
// #include "StringDefineCommon.h"
#ifndef _STRING_DEFINE_COMMON_H_
#define _STRING_DEFINE_COMMON_H_

///////////////////////////////////////////////////////////////////////////////
// 1 - Command handled at the FieldServer
	#define STRCMD_CS_COMMAND_MENT_0					"/ment"
	#define STRCMD_CS_COMMAND_MENT_1					"/ment"
	#define STRCMD_CS_COMMAND_MENT_2					"/ment"
	#define STRCMD_CS_COMMAND_MENT_HELP					"format: /ment [|String] - Setting karakter's ment. ayrıl blank string -e iptal et current ment."
	#define STRCMD_CS_COMMAND_MOVE						"/move"
	#define STRCMD_CS_COMMAND_MOVE_1					"/go"
	#define STRCMD_CS_COMMAND_MOVE_HELP					"format: /hareket et [MapIndex] [|ChannelIndex] - Moves -e specific harita & kanal"
	#define STRCMD_CS_COMMAND_COORDINATE				"/coor"
	#define STRCMD_CS_COMMAND_COORDINATE_1				"/coordinate"
	#define STRCMD_CS_COMMAND_COORDINATE_HELP			"format: /coordinate [X] [Y] - Moves -e specific coordinates harita"
	#define STRCMD_CS_COMMAND_LIST						"/list"
	#define STRCMD_CS_COMMAND_LIST_1					"/list"
	#define STRCMD_CS_COMMAND_LIST_HELP					"format: /list - Lists users içinde hediye harita (maksimum 20 users)"
	#define STRCMD_CS_COMMAND_USERSEND					"/senduser"
	#define STRCMD_CS_COMMAND_USERSEND_1				"/senduser"
	#define STRCMD_CS_COMMAND_USERSEND_HELP				"format: /senduser [karakter isim] [harita isim] - Moves karakter into specified harita"
	#define STRCMD_CS_COMMAND_INFObyNAME				"/info"
	#define STRCMD_CS_COMMAND_INFObyNAME_1				"/info"
	#define STRCMD_CS_COMMAND_INFObyNAME_HELP			"format: /info [canavar isim|eşya isim] - Shows bilgi about monsters veya eşyalar included içinde string"
	#define STRCMD_CS_COMMAND_QUESTINFO					"/quest"
	#define STRCMD_CS_COMMAND_QUESTINFO_1				"/quest"
	#define STRCMD_CS_COMMAND_QUESTINFO_HELP			"format: /görev - Shows characters görev bilgi"
	#define STRCMD_CS_COMMAND_QUESTDEL					"/delQuest"
	#define STRCMD_CS_COMMAND_QUESTDEL_1				"/delQuest"
	#define STRCMD_CS_COMMAND_QUESTDEL_HELP				"format: /delQuest [görev yok.]"
	#define STRCMD_CS_COMMAND_ITEMINFObyKIND			"/itemKind"
	#define STRCMD_CS_COMMAND_ITEMINFObyKIND_1			"/itemKind"
	#define STRCMD_CS_COMMAND_ITEMINFObyKIND_HELP		"format: /itemKind [|eşya kind(0~53)] - Shows specific types eşya"
	#define STRCMD_CS_COMMAND_ITEMINSERTbyKIND			"/insertItemKind"
	#define STRCMD_CS_COMMAND_ITEMINSERTbyKIND_1		"/insertItemKind"
	#define STRCMD_CS_COMMAND_ITEMINSERTbyKIND_HELP		"format: /insertItemKind [eşya kind(0~53)] - Additional specific types eşyalar"
	#define STRCMD_CS_COMMAND_ITEMINSERTbyITEMNUMRANGE		"/insertItemNumRange"
	#define STRCMD_CS_COMMAND_ITEMINSERTbyITEMNUMRANGE_1	"/insertItemNumRange"
	#define STRCMD_CS_COMMAND_ITEMINSERTbyITEMNUMRANGE_HELP	"format: /insertItemNumRange [-den itemnum] ~ [-e itemnum] - Additional specific eşyalar"
	#define STRCMD_CS_COMMAND_STATINIT					"/initStat"
	#define STRCMD_CS_COMMAND_STATINIT_1				"/initStatus"
	#define STRCMD_CS_COMMAND_STATINIT_2				"/initStatus"
	#define STRCMD_CS_COMMAND_STATINIT_HELP				"format: /initStatus - Initiates entire status"
	#define STRCMD_CS_COMMAND_PARTYINFO					"/partyInfo"
	#define STRCMD_CS_COMMAND_PARTYINFO_1				"/partyInfo"
	#define STRCMD_CS_COMMAND_PARTYINFO_HELP			"format: /partyInfo - Shows grup bilgi"
	#define STRCMD_CS_COMMAND_GAMETIME					"/Time"
	#define STRCMD_CS_COMMAND_GAMETIME_1				"/Time"
	#define STRCMD_CS_COMMAND_GAMETIME_HELP				"format: /zaman [|additional zaman(0~23)] - Changes hediye zaman (individual zaman changed sadece)"
	#define STRCMD_CS_COMMAND_STRINGLEVEL_0				"/string"
	#define STRCMD_CS_COMMAND_STRINGLEVEL_1				"/string"
	#define STRCMD_CS_COMMAND_STRINGLEVEL_2				"/string"
	#define STRCMD_CS_COMMAND_STRINGLEVEL_HELP			"format: /string [0~5] - Decides levels shown tarafından debug mesaj"
	#define STRCMD_CS_COMMAND_MONSUMMON					"/summon"
	#define STRCMD_CS_COMMAND_MONSUMMON_1				"/summon"
	#define STRCMD_CS_COMMAND_MONSUMMON_HELP			"format: /çağır [canavar sayı|canavar isim] [# monsters] - Brings canavar ('_' kullanıldı if canavar's isim includes alan içinde between)"
	#define STRCMD_CS_COMMAND_SKILLALL					"/allSkill"
	#define STRCMD_CS_COMMAND_SKILLALL_1				"/allSkill"
	#define STRCMD_CS_COMMAND_SKILLALL_HELP				"format: /allSkill [seviye] - Inserts tüm matching beceriler"
	#define STRCMD_CS_COMMAND_ITEMINSERTALL				"/allItem"
	#define STRCMD_CS_COMMAND_ITEMINSERTALL_1			"/allItem"
	#define STRCMD_CS_COMMAND_ITEMINSERTALL_HELP		"format: /allItem - Inserts tüm specific eşyalar excluding beceriler ve countable eşyalar"
	#define STRCMD_CS_COMMAND_ITEMINSERTWEAPON			"/allWeapon"
	#define STRCMD_CS_COMMAND_ITEMINSERTWEAPON_1		"/allWeapon"
	#define STRCMD_CS_COMMAND_ITEMINSERTWEAPON_HELP		"format: /allWeapon - Additional entire silahlar suitable içiçinde Gear"
	#define STRCMD_CS_COMMAND_ITEMDELALL_0				"/delAllItem"
	#define STRCMD_CS_COMMAND_ITEMDELALL_1				"/delAllItem"
	#define STRCMD_CS_COMMAND_ITEMDELALL_2				"/delAllItem"
	#define STRCMD_CS_COMMAND_ITEMDELALL_HELP			"format: /delAllItem - Eliminates tüm unequipped eşyalar (excluding beceriler)"
	#define STRCMD_CS_COMMAND_ITEMINSERTbyITEMNUM		"/eşya"
	#define STRCMD_CS_COMMAND_ITEMINSERTbyITEMNUM_1		"/eşya"
	#define STRCMD_CS_COMMAND_ITEMINSERTbyITEMNUM_HELP	"format: /eşya [eşya sayı] [# eşyalar] - Acquires eşyalar, treated as 1 eşya when [# eşyalar] değil inputted"
	#define STRCMD_CS_COMMAND_ITEMDROP					"/dropItem"
	#define STRCMD_CS_COMMAND_ITEMDROP_1				"/dropItem"
	#define STRCMD_CS_COMMAND_ITEMDROP_HELP				"format: /dropItem [eşya sayı] [|# eşyalar] - Dropping eşyalar içinde field"
	#define STRCMD_CS_COMMAND_USERSINFOTOTAL			"/sunucu"
	#define STRCMD_CS_COMMAND_USERSINFOTOTAL_1			"/sunucu"
	#define STRCMD_CS_COMMAND_USERSINFOTOTAL_HELP		"format: /sunucu - Listing sunucu bilgi"
	#define STRCMD_CS_COMMAND_USERSINFOperMAP			"/serverMap"
	#define STRCMD_CS_COMMAND_USERSINFOperMAP_1			"/serverMap"
	#define STRCMD_CS_COMMAND_USERSINFOperMAP_HELP		"format: /serverMap - Shows bilgi tüm harita"
	#define STRCMD_CS_COMMAND_CHANNELINFO				"/channelInfo"
	#define STRCMD_CS_COMMAND_CHANNELINFO_1				"/channelInfo"
	#define STRCMD_CS_COMMAND_CHANNELINFO_HELP			"format: /channelInfo - Shows maps kanal bilgi üzerinde hediye harita"
	#define STRCMD_CS_COMMAND_DEBUGPRINTDBG				"/dbg"
	#define STRCMD_CS_COMMAND_DEBUGPRINTDBG_1			"/dbg"
	#define STRCMD_CS_COMMAND_DEBUGPRINTDBG_HELP		"format: /dbg - içiçinde test sadece"
	#define STRCMD_CS_COMMAND_DEBUGSETPARAMF			"/testf"
	#define STRCMD_CS_COMMAND_DEBUGSETPARAMF_HELP		"format: /testf [Param1] [Param2] [Param3]"
	#define STRCMD_CS_COMMAND_BULLETCHARGE				"/bullet"
	#define STRCMD_CS_COMMAND_BULLETCHARGE_1			"/chargeBullet"
	#define STRCMD_CS_COMMAND_BULLETCHARGE_HELP			"format: /chargeBullet [|yok. 1st type bullet] [|yok. 2nd type bullet] - reloading bullets"
	#define STRCMD_CS_COMMAND_REPAIRALL					"/manpi"
	#define STRCMD_CS_COMMAND_REPAIRALL_1				"/repairAll"
	#define STRCMD_CS_COMMAND_REPAIRALL_HELP			"format: /repairAll [|karakter isim] - Healing enerji, kalkan, SP, yakıt -e 100%, Healing yourself without [karakter isim]"
	#define STRCMD_CS_COMMAND_REPAIRbyPARAM				"/banpi"
	#define STRCMD_CS_COMMAND_REPAIRbyPARAM_1			"/repairParam"
	#define STRCMD_CS_COMMAND_REPAIRbyPARAM_HELP		"format: /repairParam [|decreasing quantity %] [|karakter isim] - enerji, kalkan, SP, ve yakıt accounts içiçinde [decreasing quantity %], 50% if [decreasing quantity %] değil inputted, ve if [karakter isim] değil hediye it stands içiçinde individual's karakter"
	#define STRCMD_CS_COMMAND_USERNORMALIZE				"/normal"
	#define STRCMD_CS_COMMAND_USERNORMALIZE_1			"/normal"
	#define STRCMD_CS_COMMAND_USERNORMALIZE_HELP		"format: /normal - değiştir -e normal hesap -den yönetici veya game master mode"
	#define STRCMD_CS_COMMAND_USERSPECIALIZE			"/specialize"
	#define STRCMD_CS_COMMAND_USERSPECIALIZE_1			"/specialize"
	#define STRCMD_CS_COMMAND_USERSPECIALIZE_HELP		"format: /specialize - Changed -den normal hesap -e special yönetici veya game master hesap"
	#define STRCMD_CS_COMMAND_USERINVINCIBILITY			"/godmode"
	#define STRCMD_CS_COMMAND_USERINVINCIBILITY_1		"/invincible"
	#define STRCMD_CS_COMMAND_USERINVINCIBILITY_HELP	"format: /yenilmez - As game master veya manager mode, yok hasar taken"
	#define STRCMD_CS_COMMAND_POWERUP					"/powerUp"
	#define STRCMD_CS_COMMAND_POWERUP_1					"/powerUp"
	#define STRCMD_CS_COMMAND_POWERUP_HELP				"format: /powerUp [artırıldı offensive power(%%)]"
	#define STRCMD_CS_COMMAND_VARIABLESET				"/setVariable"
	#define STRCMD_CS_COMMAND_VARIABLESET_1				"/setVariable"
	#define STRCMD_CS_COMMAND_VARIABLESET_HELP			"format: /setVariable [variable] - adjusts (normal) variables"
	#define STRCMD_CS_COMMAND_LEVELSET					"/level"
	#define STRCMD_CS_COMMAND_LEVELSET_1				"/level"
	#define STRCMD_CS_COMMAND_LEVELSET_HELP				"format: /seviye [|seviye] [|percentage exp] [|karakter isim] - Adjusts seviye ve percentage exp. karakter"
	#define STRCMD_CS_COMMAND_USERINVISIABLE			"/invisible"
	#define STRCMD_CS_COMMAND_USERINVISIABLE_1			"/hide"
	#define STRCMD_CS_COMMAND_USERINVISIABLE_HELP		"format: /görünmez - görünmez -den diğer characters"
	#define STRCMD_CS_COMMAND_DEBUGPRINTMSGF_0			"/messagef"
	#define STRCMD_CS_COMMAND_DEBUGPRINTMSGF_1			"/msgf"
	#define STRCMD_CS_COMMAND_DEBUGPRINTMSGF_HELP		"format: /msgf - içiçinde test sadece"
	#define STRCMD_CS_COMMAND_GAMEEVENT					"/event"
	#define STRCMD_CS_COMMAND_GAMEEVENT_1				"/event"
	#define STRCMD_CS_COMMAND_GAMEEVENT_P1EXP			"exppoint"
	#define STRCMD_CS_COMMAND_GAMEEVENT_P1SPI			"SPI"
	#define STRCMD_CS_COMMAND_GAMEEVENT_P1EXPR			"restoreexppoint"
	#define STRCMD_CS_COMMAND_GAMEEVENT_P1ITEM			"eşya"
	#define STRCMD_CS_COMMAND_GAMEEVENT_P1RARE			"rareitem"
	#define STRCMD_CS_COMMAND_GAMEEVENT_P2END			"finish"
	#define STRCMD_CS_COMMAND_GAMEEVENT_HELP			"format: /etkinlik [exppoint|SPI|restoreexppoint|eşya|rareitem] [|rate %f|finish] [zaman(dakika)] - etkinlik set, iptal et"
	#define STRCMD_CS_COMMAND_PREMEUM					"/premium"
	#define STRCMD_CS_COMMAND_PREMEUM_1					"/premium"
	#define STRCMD_CS_COMMAND_PREMEUM_PNORMAL			"standart"
	#define STRCMD_CS_COMMAND_PREMEUM_PSUPER			"super"
	#define STRCMD_CS_COMMAND_PREMEUM_PUPGRADE			"upgrade"
	#define STRCMD_CS_COMMAND_PREMEUM_PEND				"finish"
	#define STRCMD_CS_COMMAND_PREMEUM_HELP				"format: /premium [standart|süper|upgrade|finish]"
// 2008-02-14 by cmkwon,   
//	#define STRCMD_CS_COMMAND_CITYWAR					"/citywar"
//	#define STRCMD_CS_COMMAND_CITYWAR_1					"/citywar"
//	#define STRCMD_CS_COMMAND_CITYWAR_PSTART			"start"
//	#define STRCMD_CS_COMMAND_CITYWAR_PEND				"finish"
//	#define STRCMD_CS_COMMAND_CITYWAR_HELP				"format: /citywar [start|finish]"
	#define STRCMD_CS_COMMAND_STEALTH					"/stealth"
	#define STRCMD_CS_COMMAND_STEALTH_1					"/stealth"
	#define STRCMD_CS_COMMAND_STEALTH_HELP				"format: /gizlilik"
	#define STRCMD_CS_COMMAND_RETURNALL					"/returnAll"
	#define STRCMD_CS_COMMAND_RETURNALL_1				"/returnAll"
	#define STRCMD_CS_COMMAND_RETURNALL_HELP			"format: /returnAll [MapIndex] - tüm üyeler içinde harita oluyor moved -e ülke capital"

// start 2011-06-22 by hskim, 사설 서버 방지
	#define STRCMD_CS_COMMAND_SERVERINFO				"/getserverinfo"		// 서버 정보 보기
// end 2011-06-22 by hskim, 사설 서버 방지
			   
// 2007-10-30 by cmkwon, 세력별 해피아워 이벤트 구현 - 명령어 형식 수정됨 아래에서 다시 정의 함
//	#define STRCMD_CS_COMMAND_HAPPYHOUREVENT			"/happyEvent"
//	#define STRCMD_CS_COMMAND_HAPPYHOUREVENT_1			"/happyEvent"
//	#define STRCMD_CS_COMMAND_HAPPYHOUREVENT_PSTART		"start"
//	#define STRCMD_CS_COMMAND_HAPPYHOUREVENT_PEND		"end"
//	#define STRCMD_CS_COMMAND_HAPPYHOUREVENT_HELP		"format: /happyEvent [start|end] [progress time(unit:min.)]"

// 1_end
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// 2 - Command used in IMServer, some are used with the same command as the above
	#define STRCMD_CS_COMMAND_DEBUGSETPARAMI			"/testi"
	#define STRCMD_CS_COMMAND_DEBUGSETPARAMI_HELP		"format: /testi - IMServer içiçinde debug"
	#define STRCMD_CS_COMMAND_WHO						"/who"
	#define STRCMD_CS_COMMAND_WHO_1						"/who"
	#define STRCMD_CS_COMMAND_WHO_HELP					"format: /who [|# users] - Listing tüm users presently içinde sunucu (unrelated -e harita)"
	#define STRCMD_CS_COMMAND_REGISTERADMIN				"/registerAdmin"
	#define STRCMD_CS_COMMAND_REGISTERADMIN_1			"/registerAdmin"
	#define STRCMD_CS_COMMAND_REGISTERADMIN_HELP		"format: /registerAdmin - Registers sunucu -e send mesaj -e yönetici içinde process etkinlik"
	#define STRCMD_CS_COMMAND_DEBUGPRINTMSGI_0			"/messagei"
	#define STRCMD_CS_COMMAND_DEBUGPRINTMSGI_1			"/msgi"
	#define STRCMD_CS_COMMAND_DEBUGPRINTMSGI_HELP		"format: /msgi - Shows tüm protocol şu istemci ve IM sunucu sends ve receives"
	#define STRCMD_CS_COMMAND_SERVERDOWN				"/serverDown"
	#define STRCMD_CS_COMMAND_SERVERDOWN_1				"/serverDown"
	#define STRCMD_CS_COMMAND_SERVERDOWN_HELP			"format: /serverDown [certified yok.] - sunucu shutdown"
	#define STRCMD_CS_COMMAND_WHOAREYOU					"/donttrytousethiscommandorelseyouwillgetbannedohyeahwewillfindyouguyseventuallyandthisisapermaban"
	#define STRCMD_CS_COMMAND_WHOAREYOU_1				"/donttrytousethiscommandorelseyouwillgetbannedohyeahwewillfindyouguyseventuallyandthisisapermaban"
	#define STRCMD_CS_COMMAND_WHOAREYOU_HELP			"format: /whoareYou [karakter isim] - devre dışı"
	#define STRCMD_CS_COMMAND_GOUSER					"/go"
	#define STRCMD_CS_COMMAND_GOUSER_1					"/go"
	#define STRCMD_CS_COMMAND_GOUSER_HELP				"format: /go [karakter isim] - Moves -e specific position karakter"
	#define STRCMD_CS_COMMAND_COMEON					"/comeon"
	#define STRCMD_CS_COMMAND_COMEON_1					"/comeon"
	#define STRCMD_CS_COMMAND_COMEON_HELP				"format: /comeon [karakter isim] - Summons specific karakter"
	#define STRCMD_CS_COMMAND_GUILDCOMEON				"/comeonGuild"
	#define STRCMD_CS_COMMAND_GUILDCOMEON_1				"/comeonGuild"
	#define STRCMD_CS_COMMAND_GUILDCOMEON_HELP			"format: /comeonGuild [tugay isim] - Summons whole tugay"
	#define STRCMD_CS_COMMAND_GUILDSEND					"/sendGuild"
	#define STRCMD_CS_COMMAND_GUILDSEND_1				"/sendGuild"
	#define STRCMD_CS_COMMAND_GUILDSEND_HELP			"format: /sendGuild [tugay isim] [harita isim] - Sends tugay into specific harita"
	#define STRCMD_CS_COMMAND_CHATPTOPFLAG				"/whisperChat"
	#define STRCMD_CS_COMMAND_CHATPTOPFLAG_1			"/whisperChat"
	#define STRCMD_CS_COMMAND_CHATPTOPFLAG_HELP			"format: /whisperChat - Toggles between blocking ve allowing whispers"
	#define STRCMD_CS_COMMAND_GUILDINFO					"/guildInfo"
	#define STRCMD_CS_COMMAND_GUILDINFO_1				"/guildInfo"
	#define STRCMD_CS_COMMAND_GUILDINFO_HELP			"format: /guildInfo - Shows tugay bilgi"
	#define STRCMD_CS_COMMAND_WEATHERSET				"/weather"
	#define STRCMD_CS_COMMAND_WEATHERSET_1				"/weather"
	#define STRCMD_CS_COMMAND_WEATHERSET_P1NORMAL		"standart"
	#define STRCMD_CS_COMMAND_WEATHERSET_P1FINE			"clear"
	#define STRCMD_CS_COMMAND_WEATHERSET_P1RAIN			"rain"
	#define STRCMD_CS_COMMAND_WEATHERSET_P1SNOW			"snow"
	#define STRCMD_CS_COMMAND_WEATHERSET_P1CLOUDY		"cloudy"
	#define STRCMD_CS_COMMAND_WEATHERSET_P1FOG			"foggy"
	#define STRCMD_CS_COMMAND_WEATHERSET_P2ALL			"whole"
	#define STRCMD_CS_COMMAND_WEATHERSET_P3ON			"on"
	#define STRCMD_CS_COMMAND_WEATHERSET_P3OFF			"off"
	#define STRCMD_CS_COMMAND_WEATHERSET_HELP			"format: /hava durumu [standart|clear|yağmur|kar|cloudy|foggy] [whole|mapname] [üzerinde|off] - Controls hava durumu"
	#define STRCMD_CS_COMMAND_CHATFORBID				"/mute"
	#define STRCMD_CS_COMMAND_CHATFORBID_1				"/forbidChat"
	#define STRCMD_CS_COMMAND_CHATFORBID_HELP			"format: /forbidChat [karakter isim] [zaman(min.)] - Prohibiting sohbet"
	#define STRCMD_CS_COMMAND_CHATFORBIDRELEASE			"/unmute"
	#define STRCMD_CS_COMMAND_CHATFORBIDRELEASE_1		"/releaseChat"
	#define STRCMD_CS_COMMAND_CHATFORBIDRELEASE_HELP	"format: /releaseChat [zaman(min.)] - Cancelling prohibited sohbet"
	#define STRCMD_CS_COMMAND_COMMANDLIST_0				"/?"
	#define STRCMD_CS_COMMAND_COMMANDLIST_1				"/help"
	#define STRCMD_CS_COMMAND_COMMANDLIST_2				"/command"
	#define STRCMD_CS_COMMAND_COMMANDLIST_HELP			"format: /? - command list shown"

	// 2005-07-20 by cmkwon
	#define STRCMD_CS_COMMAND_BONUSSTAT_0				"/BonusStat"
	#define STRCMD_CS_COMMAND_BONUSSTAT_1				"/BonusStat"
	#define STRCMD_CS_COMMAND_BONUSSTAT_2				"/BonusStat"
	#define STRCMD_CS_COMMAND_BONUSSTAT_HELP			"format: /BonusStat [Bonus Counts] [|karakter isim] - BonusStat artış"
// 2_end
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// 3 - Command used in AtumMonitor, some are used with the same command as the above
	#define STRCMD_CS_COMMAND_PASSWORDSET				"/setPassword"
	#define STRCMD_CS_COMMAND_PASSWORDSET_1				"/setPassword"
	#define STRCMD_CS_COMMAND_PASSWORDSET_HELP			"format: /setPassword [AccountName] [şifre]"
	#define STRCMD_CS_COMMAND_PASSWORDROLLBACK			"/rollbackPassword"
	#define STRCMD_CS_COMMAND_PASSWORDROLLBACK_1		"/rollbackPassword"
	#define STRCMD_CS_COMMAND_PASSWORDROLLBACK_HELP		"format: /rollbackPassword [AccountName]"
	#define STRCMD_CS_COMMAND_PASSWORDLIST				"/passwordList"
	#define STRCMD_CS_COMMAND_PASSWORDLIST_1			"/passwordList"
	#define STRCMD_CS_COMMAND_PASSWORDLIST_HELP			"format: /passwordList"
	#define STRCMD_CS_COMMAND_PASSWORDENCRYPT			"/encrypt"
	#define STRCMD_CS_COMMAND_PASSWORDENCRYPT_1			"/encrypt"
	#define STRCMD_CS_COMMAND_PASSWORDENCRYPT_HELP		"format: /encrypt [string şu olacak encrypted]"
	#define STRCMD_CS_COMMAND_ACCOUNTBLOCK				"/blockAccount"
	#define STRCMD_CS_COMMAND_ACCOUNTBLOCK_1			"/blockAccount"
	#define STRCMD_CS_COMMAND_ACCOUNTBLOCKT_HELP		"format: /blockAccount [AccountName]"
	#define STRCMD_CS_COMMAND_ACCOUNTBLOCKRELEASE		"/releaseAccount"
	#define STRCMD_CS_COMMAND_ACCOUNTBLOCKRELEASE_1		"/releaseAccount"
	#define STRCMD_CS_COMMAND_ACCOUNTBLOCKRELEASE_HELP	"format: /releaseAccount [AccountName]"
	#define STRCMD_CS_COMMAND_ACCOUNTBLOCKLIST			"/blockedList"
	#define STRCMD_CS_COMMAND_ACCOUNTBLOCKLIST_1		"/blockedList"
	#define STRCMD_CS_COMMAND_ACCOUNTBLOCKLIST_HELP		"format: /blockedList"
// 3_end
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// 4 - CommonGameServer
	#define STRCMD_CS_COMMON_DB_0000 "sadece ile ilgili -e Mgame sunucu sadece!!!!\r\n"
	#define STRCMD_CS_COMMON_DB_0001 "lütfen girin sunucu giriş isim"
	#define STRCMD_CS_COMMON_DB_0002 "lütfen girin sunucu giriş şifre"
	#define STRCMD_CS_COMMON_DB_0003 "lütfen girin DB log içinde isim"
	#define STRCMD_CS_COMMON_DB_0004 "lütfen girin DB log içinde şifre"

	#define STRCMD_CS_COMMON_MAP_0000 "Hard coded part: Ignore sayı 1 ışınlan hedef 0101 harita, easy way -e sil -den harita editor need -e be found ve needs -e be silindi!\r\n"
	#define STRCMD_CS_COMMON_MAP_0001 "harita: %04d, m_DefaltWarpTargetIndex: %d\r\n"
	#define STRCMD_CS_COMMON_MAP_0002 "Hard coded part: Ignore sayı 1 ışınlan hedef 0101 harita, easy way -e sil -den harita editor need -e be found ve needs -e be silindi!\r\n"
	#define STRCMD_CS_COMMON_MAP_0003 "harita: %04d, m_DefaltWarpTargetIndex: %d\r\n"
	#define STRCMD_CS_COMMON_MAP_0004 "ObjMon ==> ObjNum[%8d] EvType[%d] EvIndex[%3d] çağır canavar[%8d] çağır zaman[%6dsaniye], Pos(%4d, %4d, %4d)\r\n"
	#define STRCMD_CS_COMMON_MAP_0005 "[hata] ObjectMonster EventParam1 Index overlap hata ==> ObjectNum[%8d] EventType[%d] EventIndex[%3d] çağır canavar[%8d] çağır zaman[%6dsaniye], Pos(%4d, %4d, %4d)\r\n"
	#define STRCMD_CS_COMMON_MAP_0006 "Tatal canavar Count: [%4d] <== Including nesne canavar\r\n"

	#define STRCMD_CS_COMMON_DOWNLOAD_0000 "Download dosya hata"
	#define STRCMD_CS_COMMON_DOWNLOAD_0001 "dosya creation hata"
	#define STRCMD_CS_COMMON_DOWNLOAD_0002 "Download dosya read hata"

	#define STRCMD_CS_COMMON_DATETIME_0000 "%dgün%dsaat%ddakika%dsaniye"

	#define STRCMD_CS_COMMON_RACE_NORMAL		"normal"
	#define STRCMD_CS_COMMON_RACE_BATTALUS		"Vatallus"
	#define STRCMD_CS_COMMON_RACE_DECA			"DECA"
	#define STRCMD_CS_COMMON_RACE_PHILON		"Phillon"
	#define STRCMD_CS_COMMON_RACE_SHARRINE		"Shrine"
	#define STRCMD_CS_COMMON_RACE_MONSTER1		"reserve"
	#define STRCMD_CS_COMMON_RACE_MONSTER2		"reserve"
	#define STRCMD_CS_COMMON_RACE_NPC			"NPC"
	#define STRCMD_CS_COMMON_RACE_OPERATION		"Administrator"
	#define STRCMD_CS_COMMON_RACE_GAMEMASTER	"Gamemaster"
	#define STRCMD_CS_COMMON_RACE_MONITOR		"Monitor"
	#define STRCMD_CS_COMMON_RACE_GUEST			"Guest"
	#define STRCMD_CS_COMMON_RACE_DEMO			"içiçinde demo"
	#define STRCMD_CS_COMMON_RACE_ALL			"tüm race"
	#define STRCMD_CS_COMMON_RACE_UNKNOWN		"bilinmeyen race"

	#define STRCMD_CS_COMMON_MAPNAME_UNKNOWN	"yok isim"

	#define STRCMD_CS_STATUS_BEGINNER_AIRMAN		"Training Airman"
	#define STRCMD_CS_STATUS_3RD_CLASS_AIRMAN		"3rd Airman"
	#define STRCMD_CS_STATUS_2ND_CLASS_AIRMAN		"2nd Airman"
	#define STRCMD_CS_STATUS_1ST_CLASS_AIRMAN		"1st Airman"
	#define STRCMD_CS_STATUS_3RD_CLASS_WINGMAN		"3rd Wingman"
	#define STRCMD_CS_STATUS_2ND_CLASS_WINGMAN		"2nd Wingman"
	#define STRCMD_CS_STATUS_1ST_CLASS_WINGMAN		"1st Wingman"
	#define STRCMD_CS_STATUS_3RD_CLASS_LEADER		"3rd Leader"
	#define STRCMD_CS_STATUS_2ND_CLASS_LEADER		"2nd Leader"
	#define STRCMD_CS_STATUS_1ST_CLASS_LEADER		"1st Leader"
	#define STRCMD_CS_STATUS_3RD_CLASS_ACE			"3rd Ace"
	#define STRCMD_CS_STATUS_2ND_CLASS_ACE			"2nd Ace"
	#define STRCMD_CS_STATUS_1ST_CLASS_ACE			"1st Ace"
	#define STRCMD_CS_STATUS_COPPER_CLASS_GENERAL	"Air-Commodore"
	#define STRCMD_CS_STATUS_SILVER_CLASS_GENERAL	"Air Vice-Marshal"
	#define STRCMD_CS_STATUS_GOLD_CLASS_GENERAL		"Lieutenant General"
	#define STRCMD_CS_STATUS_MASTER_GENERAL			"General"

	#define STRCMD_CS_ITEMKIND_AUTOMATIC			"Automatic type"
	#define STRCMD_CS_ITEMKIND_VULCAN				"Vulcan type"
	#define STRCMD_CS_ITEMKIND_DUALIST				"Dualist type"		// 2005-08-01 by hblee : changed from GRENADE to DUALIST.
	#define STRCMD_CS_ITEMKIND_CANNON				"Cannon type"
	#define STRCMD_CS_ITEMKIND_RIFLE				"Rifle type"
	#define STRCMD_CS_ITEMKIND_GATLING				"Gatling type"
	#define STRCMD_CS_ITEMKIND_LAUNCHER				"Launcher type"
	#define STRCMD_CS_ITEMKIND_MASSDRIVE			"Mass drive type"
	#define STRCMD_CS_ITEMKIND_ROCKET				"Rocket type"
	#define STRCMD_CS_ITEMKIND_MISSILE				"Missile type"
	#define STRCMD_CS_ITEMKIND_BUNDLE				"Bundle type"

	#define STRCMD_CS_ITEMKIND_MINE					"Mine type"
	#define STRCMD_CS_ITEMKIND_SHIELD				"kalkan type"
	#define STRCMD_CS_ITEMKIND_DUMMY				"Dummy type"			
	#define STRCMD_CS_ITEMKIND_FIXER				"Fixer type"
	#define STRCMD_CS_ITEMKIND_DECOY				"Decoy type"
	#define STRCMD_CS_ITEMKIND_DEFENSE				"zırh type"
	#define STRCMD_CS_ITEMKIND_SUPPORT				"motor type"
	#define STRCMD_CS_ITEMKIND_ENERGY				"Consumable type"
	#define STRCMD_CS_ITEMKIND_INGOT				"Mineral type"
	#define STRCMD_CS_ITEMKIND_CARD					"standart kart type"
	#define STRCMD_CS_ITEMKIND_ENCHANT				"büyüle kart type"
	#define STRCMD_CS_ITEMKIND_TANK					"Tank type"
	#define STRCMD_CS_ITEMKIND_BULLET				"Bullet type"
	#define STRCMD_CS_ITEMKIND_QUEST				"görev eşya type"
	#define STRCMD_CS_ITEMKIND_RADAR				"Radar type"
	#define STRCMD_CS_ITEMKIND_COMPUTER				"Computer type"
	#define STRCMD_CS_ITEMKIND_GAMBLE				"şans denemesi kart type"
	#define STRCMD_CS_ITEMKIND_PREVENTION_DELETE_ITEM	"büyüle sil Prevention kart type"
	#define STRCMD_CS_ITEMKIND_BLASTER				"Blaster type"	// 2005-08-01 by hblee : Blaster type added.
	#define STRCMD_CS_ITEMKIND_RAILGUN				"Rail gun type"		// 2005-08-01 by hblee : Rail gun type added.
	#define STRCMD_CS_ITEMKIND_ACCESSORY_UNLIMITED	"Unlimited Accessory"		// 2006-03-17 by cmkwon, 사용시간이 <영원>인 액세서리 아이템
	#define STRCMD_CS_ITEMKIND_ACCESSORY_TIMELIMIT	"zaman limit Accessory"		// 2006-03-17 by cmkwon, 사용시간에 시간 제한이 있는 액세서리 아이템
	#define STRCMD_CS_ITEMKIND_ALL_WEAPON			"tüm silahlar"
	#define STRCMD_CS_ITEMKIND_PRIMARY_WEAPON_ALL	"standart silah"
	#define STRCMD_CS_ITEMKIND_PRIMARY_WEAPON_1		"Bullet type standart silah"
	#define STRCMD_CS_ITEMKIND_PRIMARY_WEAPON_2		"yakıt type standart silah"
	#define STRCMD_CS_ITEMKIND_SECONDARY_WEAPON_ALL	"gelişmiş silah"
	#define STRCMD_CS_ITEMKIND_SECONDARY_WEAPON_1	"Bullet type gelişmiş silah"
	#define STRCMD_CS_ITEMKIND_SECONDARY_WEAPON_2	"kalkan type gelişmiş silah"
	#define STRCMD_CS_ITEMKIND_SKILL_ATTACK			"saldırı beceri"
	#define STRCMD_CS_ITEMKIND_SKILL_DEFENSE		"savunma beceri"
	#define STRCMD_CS_ITEMKIND_SKILL_SUPPORT		"Support beceri"
	#define STRCMD_CS_ITEMKIND_SKILL_ATTRIBUTE		"Attribute beceri"
	#define STRCMD_CS_ITEMKIND_FOR_MON_PRIMARY		"eşya içiçinde 1 type canavar"
	#define STRCMD_CS_ITEMKIND_FOR_MON_GUN			"canavar machine gun type(1-1type)"
	#define STRCMD_CS_ITEMKIND_FOR_MON_BEAM			"canavar beam type(1-2type)"
	#define STRCMD_CS_ITEMKIND_FOR_MON_ALLATTACK	"saldırı tüm canavar"
	#define STRCMD_CS_ITEMKIND_FOR_MON_SECONDARY	"eşya içiçinde 2 type canavar"
	#define STRCMD_CS_ITEMKIND_FOR_MON_ROCKET		"canavar rocket(2-1type)"
	#define STRCMD_CS_ITEMKIND_FOR_MON_MISSILE		"canavar Missile type(2-1type)"
	#define STRCMD_CS_ITEMKIND_FOR_MON_BUNDLE		"canavar Bundle type(2-1type)"
	#define STRCMD_CS_ITEMKIND_FOR_MON_MINE			"canavar Mine type(2-1type)"
	#define STRCMD_CS_ITEMKIND_FOR_MON_SHIELD		"canavar kalkan type(2-2type)"
	#define STRCMD_CS_ITEMKIND_FOR_MON_DUMMY		"canavar dummy type(2-2type)"
	#define STRCMD_CS_ITEMKIND_FOR_MON_FIXER		"canavar Fixer type(2-2type)"
	#define STRCMD_CS_ITEMKIND_FOR_MON_DECOY		"canavar Decoy type(2-2type)"
	#define STRCMD_CS_ITEMKIND_FOR_MON_FIRE			"canavar Fire type"
	#define STRCMD_CS_ITEMKIND_FOR_MON_OBJBEAM		"canavar crash possible beam type"
	#define STRCMD_CS_ITEMKIND_FOR_MON_STRAIGHTBOOM	"canavar straight bomb type"
	#define STRCMD_CS_ITEMKIND_UNKNOWN				"bilinmeyen eşya"

	#define STRCMD_CS_UNITKIND_UNKNOWN				"bilinmeyen airframe"

	#define STRCMD_CS_STAT_ATTACK_PART				"Attack"
	#define STRCMD_CS_STAT_DEFENSE_PART				"Defense"
	#define STRCMD_CS_STAT_FUEL_PART				"Fuel"
	#define STRCMD_CS_STAT_SOUL_PART				"Spirit"
	#define STRCMD_CS_STAT_SHIELD_PART				"Shield"
	#define STRCMD_CS_STAT_DODGE_PART				"Agility"
	#define STRCMD_CS_STAT_BONUS					"Bonus stat"
	#define STRCMD_CS_STAT_ALL_PART					"tüm stat"
	#define STRCMD_CS_STAT_UNKNOWN					"bilinmeyen stat"

	#define STRCMD_CS_AUTOSTAT_TYPE_FREESTYLE		"Free type"
	#define STRCMD_CS_AUTOSTAT_TYPE_BGEAR_ATTACK	"saldırı type"
	#define STRCMD_CS_AUTOSTAT_TYPE_BGEAR_MULTI		"Multi-type"	
	#define STRCMD_CS_AUTOSTAT_TYPE_IGEAR_ATTACK	"saldırı type"
	#define STRCMD_CS_AUTOSTAT_TYPE_IGEAR_DODGE		"çeviklik type"
	#define STRCMD_CS_AUTOSTAT_TYPE_AGEAR_ATTACK	"saldırı type"
	#define STRCMD_CS_AUTOSTAT_TYPE_AGEAR_SHIELD	"kalkan type"
	#define STRCMD_CS_AUTOSTAT_TYPE_MGEAR_DEFENSE	"savunma type"
	#define STRCMD_CS_AUTOSTAT_TYPE_MGEAR_SUPPORT	"Support type"
	#define STRCMD_CS_AUTOSTAT_TYPE_UNKNOWN			"UNKNOWN_AUTOSTAT_TYPE"

// 2007-10-30 by cmkwon, 세력별 해피아워 이벤트 구현 - 아래에서 다시 정의 함
//	#define STRCMD_CS_INFLUENCE_TYPE_NORMAL			"Bygeniou city general army"
//	#define STRCMD_CS_INFLUENCE_TYPE_VCN			"Bygeniou city regular army"
//	#define STRCMD_CS_INFLUENCE_TYPE_ANI			"Arlington city regular army"
	#define STRCMD_CS_INFLUENCE_TYPE_RRP			"Vattalus federation army"

	#define STRCMD_CS_POS_PROW						"Radar location(Top center)"
	#define STRCMD_CS_POS_PROWIN					"Computer(Center left)"
	#define STRCMD_CS_POS_PROWOUT					"standart silah(Top left)"
	#define STRCMD_CS_POS_WINGIN					"değil being kullanıldı(Center right)"
	#define STRCMD_CS_POS_WINGOUT					"gelişmiş silah(Top right)"
	#define STRCMD_CS_POS_CENTER					"zırh(Center middle)"
	#define STRCMD_CS_POS_REAR						"motor(Bottom middle)"

	// 2010-06-15 by shcho&hslee 펫시스템
	//#define STRCMD_CS_POS_ATTACHMENT				"부착물(후미 우측-연료탱크|컨테이너계열)"
	#define STRCMD_CS_POS_ACCESSORY_UNLIMITED		"Accessory(Right side rear-Fueltank|container type)"

	// 2010-06-15 by shcho&hslee 펫시스템
	//#define STRCMD_CS_POS_PET						"사용안함(후미 좌측)"
	#define STRCMD_CS_POS_ACCESSORY_TIME_LIMIT		"değil -e kullan(Left side rear)"

	#define STRCMD_CS_POS_PET						"Partner"
//_INET_PET
	#define STRCMD_CS_HIDDEN_ITEM					"Hidden Position"
//endof _INET_PET
	#define STRCMD_CS_POS_INVALID_POSITION			"Pending location"
	#define STRCMD_CS_POS_ITEMWINDOW_OFFSET			"envanter location"

	// 2005-12-07 by cmkwon
	#define STRCMD_CS_COMMAND_QUESTCOMPLETION_0		"/QuestComplete"
	#define STRCMD_CS_COMMAND_QUESTCOMPLETION_1		"/QuestCom"
	#define STRCMD_CS_COMMAND_QUESTCOMPLETION_HELP	"format: /QuestComplete [|QuestIndex]"

	// 2006-02-08 by cmkwon
	#define STRCMD_CS_COMMAND_INFLDITRIBUTION_0		"/NationRatio"
	#define STRCMD_CS_COMMAND_INFLDITRIBUTION_1		"/InflDist"
	#define STRCMD_CS_COMMAND_INFLDITRIBUTION_HELP	"format: /NationRatio"
	#define STRCMD_CS_COMMAND_CHANGEINFL_0			"/ChangeNation"
	#define STRCMD_CS_COMMAND_CHANGEINFL_1			"/Nation"
	#define STRCMD_CS_COMMAND_CHANGEINFL_HELP		"format: /ChangeNation [|1(Normal)|2(BCU)|4(ANI)]"

	// 2006-03-02 by cmkwon
	#define STRCMD_CS_COMMAND_GOMONSTER_0			"/GoMon"
	#define STRCMD_CS_COMMAND_GOMONSTER_1			"/GoMonster"
	#define STRCMD_CS_COMMAND_GOMONSTER_HELP		"format: /GoMonster [MonsterName|MonsterNumber]"

	//////////////////////////////////////////////////////////////////////////
	// 2008-05-20 by dhjin, EP3 - 여단 수정 사항 - 주석 처리 밑으로 이동
	// 2006-03-07 by cmkwon
//	#define STRCMD_CS_GUILD_RANK_PRIVATE_NULL		"Member"
//	#define STRCMD_CS_GUILD_RANK_COMMANDER			"Flight brigade commander"
//	#define STRCMD_CS_GUILD_RANK_SQUAD_LEADER_1		"1st battalion commander"
//	#define STRCMD_CS_GUILD_RANK_PRIVATE_1			"1st battalion member"
//	#define STRCMD_CS_GUILD_RANK_SQUAD_LEADER_2		"2nd battalion commander"
//	#define STRCMD_CS_GUILD_RANK_PRIVATE_2			"2nd battalion member"
//	#define STRCMD_CS_GUILD_RANK_SQUAD_LEADER_3		"3rd battalion commander"
//	#define STRCMD_CS_GUILD_RANK_PRIVATE_3			"3rd battalion member"
//	#define STRCMD_CS_GUILD_RANK_SQUAD_LEADER_4		"4th battalion commander"
//	#define STRCMD_CS_GUILD_RANK_PRIVATE_4			"4th battalion member"
//	#define STRCMD_CS_GUILD_RANK_SQUAD_LEADER_5		"5th battalion commander"
//	#define STRCMD_CS_GUILD_RANK_PRIVATE_5			"5th battalion member"

	// 2006-04-17 by cmkwon
	#define STRCMD_CS_COMMAND_SIGNBOARD_0			"/Noticeboard"
	#define STRCMD_CS_COMMAND_SIGNBOARD_1			"/Noticeboard"
	#define STRCMD_CS_COMMAND_SIGNBOARD_HELP		"format: /Noticeboard [|Duration(unit:min) [Notice details] - Adds notice -e public electronic notice board içiçinde given duration."
	#define STRCMD_CS_COMMAND_SIGNBOARD_DEL_0		"/DeleteNoticeboard"
	#define STRCMD_CS_COMMAND_SIGNBOARD_DEL_1		"/DeleteNoticeboard"
	#define STRCMD_CS_COMMAND_SIGNBOARD_DEL_HELP	"format: /DeleteNoticeboard [index notice -e be silindi] - Deletes certain notice üzerinde electronic notice board."
	
	// 2006-04-20 by cmkwon
	#define STRCMD_CS_COMMON_RACE_INFLUENCE_LEADER	"Influence Leader"
	#define STRCMD_CS_COMMON_RACE_INFLUENCE_SUBLEADER	"Influence Subleader"
	// 2006-04-21 by cmkwon
	#define STRCMD_CS_ITEMKIND_INFLUENCE_BUFF		"Influence Buff"
	#define STRCMD_CS_ITEMKIND_INFLUENCE_GAMEEVENT	"Influence etkinlik"

	// 2006-04-24 by cmkwon
	#define STRCMD_CS_COMMAND_CONPOINT_0			"/ContributionPoint"
	#define STRCMD_CS_COMMAND_CONPOINT_1			"/ContributionPoint"
	#define STRCMD_CS_COMMAND_CONPOINT_HELP			"format: /ContributionPoint [Influence(2:BCU, 4:ANI)] [artış] - artış contribution certain ülke"

	// 2006-05-08 by cmkwon
	#define STRCMD_CS_COMMAND_CALLGM_0				"/CallGM"
	#define STRCMD_CS_COMMAND_CALLGM_1				"/CallGM"  // Helper
	#define STRCMD_CS_COMMAND_CALLGM_2				"/CallGM"  // Help
	#define STRCMD_CS_COMMAND_CALLGM_HELP			"format: /CallGM [Details request] - Request consultation ile GM. - devre dışı"
	#define STRCMD_CS_COMMAND_VIEWCALLGM_0			"/ViewCallGM"
	#define STRCMD_CS_COMMAND_VIEWCALLGM_1			"/ViewCallGM"   // See helper
	#define STRCMD_CS_COMMAND_VIEWCALLGM_2			"/ViewCallGM"   // See help
	#define STRCMD_CS_COMMAND_VIEWCALLGM_HELP		"format: /ViewCallGM [|sayı(1~10)] - Check sayı consultation request list -e GM - devre dışı"
	#define STRCMD_CS_COMMAND_BRINGCALLGM_0			"/BringCallGM"
	#define STRCMD_CS_COMMAND_BRINGCALLGM_1			"/BringCallGM"   // Bring helper
	#define STRCMD_CS_COMMAND_BRINGCALLGM_2			"/BringCallGM"   // Bring help
	#define STRCMD_CS_COMMAND_BRINGCALLGM_HELP		"format: /BringCallGM [|sayı(1~10)] - Bring requested sayı consultation request list -e GM (silindi -den sunucu)"

	// 2006-07-18 by cmkwon
	#define STRCMD_CS_COMMAND_COMEONINFL_0			"/ComeOnInfl"
	#define STRCMD_CS_COMMAND_COMEONINFL_1			"/ComeOnNation"
	#define STRCMD_CS_COMMAND_COMEONINFL_2			"/ComeOnNation"
// 2008-09-09 by cmkwon, /세력소환 명령어 인자 리스트에 기어타입 추가 - commented
//	#define STRCMD_CS_COMMAND_COMEONINFL_HELP		"format: /ComeOnNation [1(Normal)|2(BCU)|4(ANI)|3|5|6|7] [MAX_USER] [0|Min-level] [0|Max-level] [message to users] - Summons all specified level users in nation with a message."

	// 2006-07-24 by cmkwon
	#define STRCMD_CS_COMMAND_ITEMINMAP_0			"/InsertItemInMap"
	#define STRCMD_CS_COMMAND_ITEMINMAP_1			"/SendItem"
	#define STRCMD_CS_COMMAND_ITEMINMAP_2			"/SendItem"
	#define STRCMD_CS_COMMAND_ITEMINMAP_HELP		"format: /SendItem [1(Normal)|2(BCU)|4(ANI)|3|5|6|7] [eşya sayı] [# eşyalar] - Gives specific eşya -e tüm ülke users current harita."

	// 2006-07-28 by cmkwon
	#define STRCMD_CS_ITEMKIND_COLOR_ITEM			"Color eşya"

	// 2006-08-03 by cmkwon, 나라별 날짜 표현 방식이 다르다
	// 한국(Korea):		YYYY-MM-DD HH:MM:SS
	// 미국(English):	MM-DD-YYYY HH:MM:SS
	// 베트남(Vietnam):	DD-MM-YYYY HH:MM:SS
	#define NATIONAL_ATUM_DATE_TIME_STRING_FORMAT(Y, M, D, h, m, s)				"%02d-%02d-%04d %02d:%02d:%02d", M, D, Y, h, m, s
	#define NATIONAL_ATUM_DATE_TIME_STRING_FORMAT_EXCLUDE_SECOND(Y, M, D, h, m)	"%02d-%02d-%04d %02d:%02d", M, D, Y, h, m

	// 2006-08-08 by dhjin, 레벨분포
	#define STRCMD_CS_COMMAND_DISTRIBUTIONLEVEL_0		"/LevelDistribution"		// 2006-08-08 by dhjin
	#define STRCMD_CS_COMMAND_DISTRIBUTIONLEVEL_1		"/LevelDist"				// 2006-08-08 by dhjin
	#define STRCMD_CS_COMMAND_DISTRIBUTIONLEVEL_HELP	"format: /LevelDistribution - It shows concurrent users seviye distribution."	// 2006-08-08 by dhjin

	// 2006-08-10 by cmkwon
	#define STRCMD_CS_ITEMKIND_RANDOMBOX				"olasılık eşya"

	// 2006-08-21 by cmkwon
	#define STRCMD_CS_ITEMKIND_MARK						"Mark"

	///////////////////////////////////////////////////////////////////////////////
	// 2006-08-24 by cmkwon
	// 클라이언트에서만 사용하는 명령어(Just command for client)
	#define STRCMD_C_COMMAND_CALL						"/call"
	#define STRCMD_C_COMMAND_CALL_HELP					"format: /call [CharacterName] - Request 1:1 voice sohbet -e specific karakter."
	#define STRCMD_C_COMMAND_PARTYCALL					"/formcall"
	#define STRCMD_C_COMMAND_PARTYCALL_HELP				"format: /formcall - Start voice sohbet between filo üyeler. sadece usable tarafından filo leader."
	#define STRCMD_C_COMMAND_PARTYCALLEND				"/formcallend"
	#define STRCMD_C_COMMAND_PARTYCALLEND_HELP			"foramt: /formcallend - End voice sohbet between filo üyeler. sadece usable tarafından filo leader."
	#define STRCMD_C_COMMAND_GUILDCALL					"/brigcall"
	#define STRCMD_C_COMMAND_GUILDCALL_HELP				"format: /brigcall - Start voice sohbet between tugay üyeler. sadece usable tarafından tugay leader."
	#define STRCMD_C_COMMAND_GUILDCALLEND				"/brigcallend"
	#define STRCMD_C_COMMAND_GUILDCALLEND_HELP			"format: /brigcallend - End voice sohbet between tugay üyeler. sadece usable tarafından tugay leader."
	#define STRCMD_C_COMMAND_CALLEND					"/endcall"
	#define STRCMD_C_COMMAND_CALLEND_HELP				"format: /endcall - Ends tugay, filo, veya normal voice sohbet."
	#define STRCMD_C_COMMAND_COMBAT						"/confront"
	#define STRCMD_C_COMMAND_BATTLE						"/fight"
	#define STRCMD_C_COMMAND_BATTLE_HELP				"format: /savaş [CharacterName] - Request PvP -e specific karakter."
	#define STRCMD_C_COMMAND_SURRENDER					"/surrender"
	#define STRCMD_C_COMMAND_SURRENDER_HELP				"format: /surrender [CharacterName] - Surrenders PvP savaş ile specified karakter."
	#define STRCMD_C_COMMAND_PARTYBATTLE				"/formfight"
	#define STRCMD_C_COMMAND_PARTYBATTLE_HELP			"format: /formfight [CharacterName] - Request filo PvP -e specific karakter(filo leader). sadece usable tarafından filo leader."
	#define STRCMD_C_COMMAND_PARTYCOMBAT				"/formconfront"
	#define STRCMD_C_COMMAND_PARTYWAR					"/formbattle"
	#define STRCMD_C_COMMAND_GUILDBATTLE				"/brigfight"
	#define STRCMD_C_COMMAND_GUILDCOMBAT				"/brigconfront"
	#define STRCMD_C_COMMAND_GUILDCOMBAT_HELP			"format: /brigconfront [CharacterName] - Request tugay PvP -e specific karakter(tugay leader). sadece usable tarafından tugay leader."
	#define STRCMD_C_COMMAND_GUILDWAR					"/brigbattle"
	#define STRCMD_C_COMMAND_GUILDSURRENDER				"/brigsurrender"
	#define STRCMD_C_COMMAND_GUILDSURRENDER_HELP		"format: /brigsurrender - Surrender tugay PvP savaş. sadece usable tarafından tugay leader."
	#define STRCMD_C_COMMAND_NAME						"/name"
	#define STRCMD_C_COMMAND_NAME_HELP					"format: /isim [CharacterName] [class(2 ~ 11)] - değiştir appointment class specific karakter. sadece usable tarafından tugay leader."
	#define STRCMD_C_COMMAND_WARP						"/warp"
	#define STRCMD_C_COMMAND_CANCELSKILL				"/cancelskill"
	#define STRCMD_C_COMMAND_INITCHAT					"/initchat"
	#define STRCMD_C_COMMAND_INITCHAT_HELP				"format: /initchat - Resets sohbet window"
	#define STRCMD_C_COMMAND_REFUSEBATTLE				"/refusefight"
	#define STRCMD_C_COMMAND_REFUSEBATTLE_HELP			"format: /refusefight - üzerinde/Off - Toggles PvP denial setting"
	#define STRCMD_C_COMMAND_REFUSETRADE				"/refusetrade"
	#define STRCMD_C_COMMAND_REFUSETRADE_HELP			"format: /refusetrade - üzerinde/Off - Toggles trading denial setting"
	#define STRMSG_C_050810_0001						"/CloseWindow"
	#define STRMSG_C_050810_0001_HELP					"format: /Closewindow - Prevents mesaj popups. Popup messages oluyor automatically iptal edildi."
	#define STRMSG_C_050810_0002						"/OpenWindow"
	#define STRMSG_C_050810_0002_HELP					"format: /Openwindow - Allows mesaj popups."

// 2006-09-29 by cmkwon
#define STRCMD_CS_ITEMKIND_SKILL_SUPPORT_ITEM			"Support beceri eşya"

// 2010-06-15 by shcho&hslee 펫시스템 - 펫 아이템.
#define STRCMD_CS_ITEMKIND_PET_ITEM						"Partner eşya"

// 2006-11-17 by cmkwon, 베트남 하루 게임 시간 관련
#define STRCMD_CS_COMMAND_TIMELIMITSYSTEM_0			"/TimeLimitSystem"
#define STRCMD_CS_COMMAND_TIMELIMITSYSTEM_1			"/TimeLimitSystem"
#define STRCMD_CS_COMMAND_TIMELIMITSYSTEM_P2ON		"on"
#define STRCMD_CS_COMMAND_TIMELIMITSYSTEM_P2OFF		"off"
#define STRCMD_CS_COMMAND_TIMELIMITSYSTEM_HELP		"format: /TimeLimitSystem [üzerinde|off] - bu command -e turn zaman limit sistem turn üzerinde/off."
#define STRCMD_CS_COMMAND_PLAYTIME_0				"/PlayTime"
#define STRCMD_CS_COMMAND_PLAYTIME_1				"/PlayTime"
#define STRCMD_CS_COMMAND_PLAYTIME_HELP				"format: /PlayTime - bu command shows todays played zaman."

// 2007-10-06 by cmkwon, 부지도자 2명의 호칭을 다르게 설정 - 아래에 세력별로 다르게 정의함
//// 2006-12-13 by cmkwon
//#define STRCMD_CS_COMMON_INFLUENCE_LEADER			"Leader"
//#define STRCMD_CS_COMMON_INFLUENCE_SUBLEADER		"Subleader"

// 2007-01-08 by dhjin
#define STRCMD_CS_COMMAND_BONUSSTAT_POINT_0			"/BonusStatPoint"
#define STRCMD_CS_COMMAND_BONUSSTAT_POINT_1			"/BonusStatPoint"
#define STRCMD_CS_COMMAND_BONUSSTAT_POINT_2			"/BonusStatPoint"
#define STRCMD_CS_COMMAND_BONUSSTAT_POINT_HELP		"format: /BonusStatPoint [BonusStatPoint Counts] [|karakter isim] - BonusStatPoint update -e DB"

// 2007-01-25 by dhjin
#define STRCMD_CS_COMMAND_PCBANGUSERCOUNT_0			"/PCBang"
#define STRCMD_CS_COMMAND_PCBANGUSERCOUNT_1			"/PCBang"
#define STRCMD_CS_COMMAND_PCBANGUSERCOUNT_HELP		"format: /PCBang - PCBang kullanıcı counts"

// 2007-10-06 by dhjin, 부지도자 선출 방법 변경으로 수정
// 2007-02-13 by dhjin, 부지도자
//#define STRCMD_CS_COMMAND_SUBLEADER_0				"/Subleader"
//#define STRCMD_CS_COMMAND_SUBLEADER_1				"/Subleader"
//#define STRCMD_CS_COMMAND_SUBLEADER_HELP			"format: /Subleader [CharacterName] - Setting subleader"
//#define STRCMD_CS_COMMAND_SUBLEADER_RESULT_ERROR		"Subleader setting has been cancelled."
//#define STRCMD_CS_COMMAND_SUBLEADER_RESULT_0			"No more subleaders can be set."
//#define STRCMD_CS_COMMAND_SUBLEADER_RESULT_1			"%s has become the first subleader."
//#define STRCMD_CS_COMMAND_SUBLEADER_RESULT_2			"%s has become the second subleader."
//#define STRCMD_CS_COMMAND_SUBLEADER_RESULT_10			"%s does not exist."
//#define STRCMD_CS_COMMAND_SUBLEADER_RESULT_20			"%s is already a subleader."

// 2007-02-23 by dhjin, 거점정보
#define STRCMD_CS_COMMAND_STRATEGYPOINTINFO_0			"/StrategyPointInfo"
#define STRCMD_CS_COMMAND_STRATEGYPOINTINFO_1			"/StrategyPointInfo"
#define STRCMD_CS_COMMAND_STRATEGYPOINTINFO_HELP		"format: /StrategyPointInfo - bu shows status current progress üzerinde strategypoint."
#define STRCMD_CS_COMMAND_STRATEGYPOINTINFO_EMPTY		"There yok strategypoint war devam ediyor."
#define STRCMD_CS_COMMAND_STRATEGYPOINTINFO_EXIST		"Strategypoint war devam ediyor."
#define STRCMD_CS_COMMAND_STRATEGYPOINTINFO_ZONE		"Progress location"
#define STRCMD_CS_COMMAND_STRATEGYPOINTINFO_STARTTIME		"başlatılıyor zaman"

// 2007-03-29 by cmkwon
#define STRCMD_CS_UNITKIND_BGEAR					"B-GEAR"
#define STRCMD_CS_UNITKIND_MGEAR					"M-GEAR"
#define STRCMD_CS_UNITKIND_AGEAR					"A-GEAR"
#define STRCMD_CS_UNITKIND_IGEAR					"I-GEAR"
#define STRCMD_CS_UNITKIND_BGEAR_ALL				"B-GEAR tüm"
#define STRCMD_CS_UNITKIND_MGEAR_ALL				"M-GEAR tüm"
#define STRCMD_CS_UNITKIND_AGEAR_ALL				"A-GEAR tüm"
#define STRCMD_CS_UNITKIND_IGEAR_ALL				"I-GEAR tüm"
#define STRCMD_CS_UNITKIND_GEAR_ALL					"GEAR tüm"

// 2007-03-30 by dhjin, 옵저버 모드 유저 등록
#define STRCMD_CS_COMMAND_OBSERVER_REG_START_0  		"/Observerstart"  // 2007-03-30 by dhjin, Client only
#define STRCMD_CS_COMMAND_OBSERVER_REG_START_1  		"/Observerstart"   // 2007-03-30 by dhjin, Client only
#define STRCMD_CS_COMMAND_OBSERVER_REG_END_0  			"/Observerend"   // 2007-03-30 by dhjin, Client only 
#define STRCMD_CS_COMMAND_OBSERVER_REG_END_1  			"/Observerend"   // 2007-03-30 by dhjin, Client only
#define STRCMD_CS_COMMAND_OBSERVER_REG_0   			"/Observer"
#define STRCMD_CS_COMMAND_OBSERVER_REG_1   			"/Observer"
#define STRCMD_CS_COMMAND_OBSERVER_REG_HELP   			"format: /Observer [n] [CharacterName] - CharacterName kaydet kullanıcı n sayı"

// 2007-04-10 by cmkwon, Jamboree server 군 관련
#define STRCMD_CS_COMMAND_JAMBOREE_INIT_0   			"/InitJamboree"   
#define STRCMD_CS_COMMAND_JAMBOREE_INIT_1   			"/InitJamboree"  
#define STRCMD_CS_COMMAND_JAMBOREE_INIT_HELP  			"format: /InitJamboree [validation sayı] - Initialize jamboree sunucu DB(atum2_db_20)."
#define STRCMD_CS_COMMAND_JAMBOREE_ENTRANTS_0  			"/EntrantJamboree"
#define STRCMD_CS_COMMAND_JAMBOREE_ENTRANTS_1  			"/EntrantJamboree"  
#define STRCMD_CS_COMMAND_JAMBOREE_ENTRANTS_HELP 		"format: /EntrantJamboree [CharacterName] - Designated karakter olacak duplicated -e jamboree sunucu DB(atum2_db_20)."
#define STRCMD_CS_JAMBOREE_PREADD_CHARACTER_NAME_1  "1_"
#define STRCMD_CS_JAMBOREE_PREADD_CHARACTER_NAME_2  "2_"
#define STRCMD_CS_JAMBOREE_PREADD_CHARACTER_NAME_3  "3_"
#define STRCMD_CS_JAMBOREE_PREADD_CHARACTER_NAME_4  "4_"
#define STRCMD_CS_JAMBOREE_PREADD_CHARACTER_NAME_5  "5_"
#define STRCMD_CS_JAMBOREE_PREADD_CHARACTER_NAME_6  "6_"
#define STRCMD_CS_JAMBOREE_PREADD_CHARACTER_NAME_7  "7_"
#define STRCMD_CS_JAMBOREE_PREADD_CHARACTER_NAME_8  "8_"
#define STRCMD_CS_JAMBOREE_PREADD_CHARACTER_NAME_9  "9_"
#define STRCMD_CS_JAMBOREE_PREADD_CHARACTER_NAME_10  "10_"
#define STRCMD_CS_JAMBOREE_PREADD_CHARACTER_NAME_11  "11_"
#define STRCMD_CS_JAMBOREE_PREADD_CHARACTER_NAME_12  "12_"
#define STRCMD_CS_JAMBOREE_PREADD_CHARACTER_NAME_13  "13_"
#define STRCMD_CS_JAMBOREE_PREADD_CHARACTER_NAME_14  "14_"
#define STRCMD_CS_JAMBOREE_PREADD_CHARACTER_NAME_15  "15_"
#define STRCMD_CS_JAMBOREE_PREADD_CHARACTER_NAME_16  "16_"
#define STRCMD_CS_JAMBOREE_PREADD_CHARACTER_NAME_17  "17_"
#define STRCMD_CS_JAMBOREE_PREADD_CHARACTER_NAME_18  "18_"
#define STRCMD_CS_JAMBOREE_PREADD_CHARACTER_NAME_19  "19_"

// 2007-04-17 by dhjin, 레벨 랭크에 관한 등급
#define STRCMD_CS_CHARACTER_12_LEVEL_RANK  "Trainee"
#define STRCMD_CS_CHARACTER_22_LEVEL_RANK  "Junior"
#define STRCMD_CS_CHARACTER_32_LEVEL_RANK  "Airman"
#define STRCMD_CS_CHARACTER_42_LEVEL_RANK  "Wingman"
#define STRCMD_CS_CHARACTER_52_LEVEL_RANK  "Ace"
#define STRCMD_CS_CHARACTER_62_LEVEL_RANK  "Veteran"
#define STRCMD_CS_CHARACTER_72_LEVEL_RANK  "Top Gun"
#define STRCMD_CS_CHARACTER_82_LEVEL_RANK  "Wing Cmdr"
#define STRCMD_CS_CHARACTER_92_LEVEL_RANK  "Hero"

// 2007-05-09 by cmkwon, 
#define STRMSG_VERSION_INFO_FILE_NAME				"VersionInfo.ver"
#define STRMSG_REG_KEY_NAME_LAUNCHER_VERSION		"LauncherVersion"
#define STRMSG_REG_KEY_NAME_CLIENT_VERSION			"ClientVersion"
// 2007-12-27 by cmkwon, 윈도우즈 모드 기능 추가 -
//#define STRMSG_REG_KEY_NAME_WINDOWDEGREE			"WindowDegree"
#define STRMSG_REG_KEY_NAME_ACCOUNT_NAME			"AccountName"
#define STRMSG_REG_KEY_NAME_SERVER_GROUP_NAME		"ServerGroupName"

// 2007-05-23 by dhjin, ARENA 팀 출력 관련 스트링
#define STRMSG_CS_STRING_ARENA_NOT_SEARCH   "bulunamıyor arena team."
#define STRMSG_CS_COMMAND_ARENA_TEAM_INFO_0   "/ARENA"
#define STRMSG_CS_COMMAND_ARENA_TEAM_INFO_1   "/ARENA"
#define STRMSG_CS_COMMAND_ARENA_TEAM_INFO_HELP  "format: /arena [2(BCU)|4(ANI)]- Shows hediye progress arena."

// 2010. 06. 04 by hsLee ARENA 인피니티 관련. - 
// 2010. 06. 04 by hsLee 인티피니 필드 2차 난이도 조절. (GM 명령어 추가. /nextscene(다음 시네마 씬 호출.) )
#define STRCMD_CS_COMMAND_INFINITY_NEXT_SCENE		"/nextscene"
// End 2010. 06. 04 by hsLee 인티피니 필드 2차 난이도 조절. (GM 명령어 추가. /nextscene(다음 시네마 씬 호출.) )

// 2007-06-15 by dhjin, 관전
#define STRMSG_CS_COMMAND_WATCH_START_INFO_0		"/WatchStart"
#define STRMSG_CS_COMMAND_WATCH_START_INFO_1		"/WatchStart"
#define STRMSG_CS_COMMAND_WATCH_START_INFO_HELP	"format:/WatchStart-Start watch."
#define STRMSG_CS_COMMAND_WATCH_END_INFO_0			"/WatchEnd"
#define STRMSG_CS_COMMAND_WATCH_END_INFO_1			"/WatchEnd"
#define STRMSG_CS_COMMAND_WATCH_END_INFO_HELP		"format: /WatchEnd? Ends watch."

// 2007-06-22 by dhjin, WarPoint 추가
#define STRMSG_CS_COMMAND_WARPOINT_0    "/WarPoint"
#define STRMSG_CS_COMMAND_WARPOINT_1    "/WarPoint"
#define STRMSG_CS_COMMAND_WARPOINT_HELP    "format: /WarPoint [sayı 1~1000000] [|Username]? Adds savaş puanları."

// 2007-06-26 by dhjin, 워포인트 이벤트 관련 추가
#define STRCMD_CS_COMMAND_GAMEEVENT_P1WARPOINT		"WarPoint"

// 2007-07-11 by cmkwon, Arena block system materialization - Add command(/forbidAreana, /releaseArena)
#define STRCMD_CS_COMMAND_ARENAFORBID_0    "/forbidArena"
#define STRCMD_CS_COMMAND_ARENAFORBID_1    "/forbidArena"
#define STRCMD_CS_COMMAND_ARENAFORBID_2    "/forbidArena"
#define STRCMD_CS_COMMAND_ARENAFORBID_HELP   "format: /forbidArena [karakter isim] [|zaman(dakika)] - Forbid entering Arena"
#define STRCMD_CS_COMMAND_ARENAFORBIDRelease_0  "/releaseArena"
#define STRCMD_CS_COMMAND_ARENAFORBIDRelease_1  "/releaseArena"
#define STRCMD_CS_COMMAND_ARENAFORBIDRelease_2  "/releaseArena"
#define STRCMD_CS_COMMAND_ARENAFORBIDRelease_HELP "format: /releaseArena [karakter isim] - Release prohibition Arena"

///////////////////////////////////////////////////////////////////////////////
// 2007-08-02 by cmkwon, Brigade mark screening system materialization - added string
#define STRMSG_070802_0001    "tugay mark has been başarıyla kaydoldu."
#define STRMSG_070802_0002    "Registration olacak completed after screening process."
#define STRMSG_070802_0003    "Will siz accept %d seçildi tugay mark?"
#define STRMSG_070802_0004    "yok tugay mark status"
#define STRMSG_070802_0005    "tugay mark içinde waiting status"
#define STRMSG_070802_0006    "tugay mark içinde normal status"
#define STRMSG_070802_0007    "tugay mark hata status"

// 2007-08-24 by cmkwon, 스피커아이템 사용 가능/금지 설정 기능 추가 - 명령어 추가
#define STRCMD_CS_COMMAND_UsableSpeakerItem_0			"/UseSpeaker"
#define STRCMD_CS_COMMAND_UsableSpeakerItem_1			"/UseSpeaker"
#define STRCMD_CS_COMMAND_UsableSpeakerItem_2			"/UseSpeaker"
#define STRCMD_CS_COMMAND_UsableSpeakerItem_P1Able		"Enable"
#define STRCMD_CS_COMMAND_UsableSpeakerItem_P1Forbid	"Forbid"
#define STRCMD_CS_COMMAND_UsableSpeakerItem_HELP		"format: /[UseSpeaker|UseSpeaker|UseSpeaker] [Enable|Forbid] - Enable/Forbid kullan Speaker eşya"

// 2007-08-27 by cmkwon, PrepareShutdown command(GM can shutdown game server in SCAdminTool)
#define STRCMD_CS_COMMAND_PrepareShutdown_0				"/PrepareShutdown"
#define STRCMD_CS_COMMAND_PrepareShutdown_1				"/PrepareShutdown"
#define STRCMD_CS_COMMAND_PrepareShutdown_2				"/PrepareShutdown"
#define STRCMD_CS_COMMAND_PrepareShutdown_P1Start		"Start"
#define STRCMD_CS_COMMAND_PrepareShutdown_P1Release		"Release"
#define STRCMD_CS_COMMAND_PrepareShutdown_HELP			"format: /[PrepareShutdown|PrepareShutdown|PrepareShutdown] [Start|Release] - Prepare sunucu shut down, disconnect tüm users."

// 2007-08-30 by cmkwon, 회의룸 시스템 구현 - 명령어 추가
#define STRCMD_CS_COMMAND_EntrancePermission_0                                     "/EntrancePermission"
#define STRCMD_CS_COMMAND_EntrancePermission_1                                     "/EntrancePermission"
#define STRCMD_CS_COMMAND_EntrancePermission_2                                     "/EntrancePermission"
#define STRCMD_CS_COMMAND_EntrancePermission_HELP                      "format: /[EntrancePermission] [|CharacterName]? -ebilir sadece be kullanıldı tarafından leader. bu allows ilgili karakter -e be put üzerinde conference oda giriş list."
#define STRCMD_CS_COMMAND_EntrancePermissionDeny_0                               "/EntrancePermissionDeny"
#define STRCMD_CS_COMMAND_EntrancePermissionDeny_1                               "/EntrancePermissionDeny"
#define STRCMD_CS_COMMAND_EntrancePermissionDeny_2                               "/EntrancePermissionDeny"
#define STRCMD_CS_COMMAND_EntrancePermissionDeny_HELP                "format: /[ EntrancePermissionDeny] [CharacterName]? -ebilir sadece be kullanıldı tarafından leader. bu deletes ilgili kullanıcı -den conference oda giriş list."

// 2007-10-05 by cmkwon, different each nations.
#define STRCMD_071005_0000					"%d\\rgün\\r %dsaat(s) %ddakika(s) %dsaniye(s)", Day, Hour, Minute, Second  // Day, Hour, Minute, Second is parameter name. No need to translate.
#define STRCMD_071005_0001					"%dYear %dmonth %dgün", Year, Month, Day //Year, Month, Day is parameter name. No need to translate.
#define STRCMD_071005_0002					"%dYear %dmonth", Year, Month // Year, Month is parameter name. No need to translate.
#define STRCMD_071005_0003					"%dMonth %dgün", Month, Day // Month, Day is parameter name. No need to translate.


// 2007-10-06 by cmkwon, Set name 2 sub-leader each nations
#define STRCMD_CS_COMMON_RACE_INFLUENCE_SUBLEADER_1		"Sub leader 1" // this is common both nations.
#define STRCMD_CS_COMMON_RACE_INFLUENCE_SUBLEADER_2		"Sub leader 2" // this is common both nations.
#define STRCMD_VCN_INFLUENCE_LEADER						"General Commander"
#define STRCMD_VCN_INFLUENCE_SUBLEADER_1				"Deputy Commander"
#define STRCMD_VCN_INFLUENCE_SUBLEADER_2				"Chief Staff"
#define STRCMD_ANI_INFLUENCE_LEADER						"Chairman"
#define STRCMD_ANI_INFLUENCE_SUBLEADER_1				"Vice-Chairman"
#define STRCMD_ANI_INFLUENCE_SUBLEADER_2				"Strategy Director"
#define STRCMD_OUTPOST_GUILD_MASTER						"Cpt. %s"

// 2007-10-06 by dhjin, command to set 2 sub-leader
#define STRCMD_CS_COMMAND_SUBLEADER1_0				"/appointment1"
#define STRCMD_CS_COMMAND_SUBLEADER1_1				"/appointment1"
#define STRCMD_CS_COMMAND_SUBLEADER1_HELP			"format: /appointment1 [CharacterName] - BCU: set BCU ülke 1st sub-leader isim, ANI: set ANI ülke 1st sub-leader isim"
#define STRCMD_CS_COMMAND_SUBLEADER2_0				"/appointment2"
#define STRCMD_CS_COMMAND_SUBLEADER2_1				"/appointment2"
#define STRCMD_CS_COMMAND_SUBLEADER2_HELP			"format: /appointment2 [CharacterName] - BCU: set BCU ülke 2nd sub-leader isim, ANI: set ANI ülke 2nd sub-leader isim"

// 2007-10-30 by cmkwon, each nation happy hour event system - Command system is changed.
#define STRCMD_CS_COMMAND_HAPPYHOUREVENT				"/HappyEvent"
#define STRCMD_CS_COMMAND_HAPPYHOUREVENT_1			"/HappyEvent"
#define STRCMD_CS_COMMAND_HAPPYHOUREVENT_PSTART	"Start"
#define STRCMD_CS_COMMAND_HAPPYHOUREVENT_PEND		"End"
#define STRCMD_CS_COMMAND_HAPPYHOUREVENT_HELP		"format: /HappyEvent [255(AllNation)|0(NormalNation)|2(BCU)|4(ANI)] [Start|End] [Progress zaman(Unit:dakika)]"
 
// 2007-10-30 by cmkwon, each nation happy hour event system - Changed with old system.
#define STRCMD_CS_INFLUENCE_TYPE_NORMAL					"Neutral"
#define STRCMD_CS_INFLUENCE_TYPE_VCN						"Bygeniou"
#define STRCMD_CS_INFLUENCE_TYPE_ANI							"Arlington"
#define STRCMD_CS_INFLUENCE_TYPE_GM							"\\mStaff\\m"
#define STRCMD_CS_INFLUENCE_TYPE_ALL_MASK					"AllNation"   // 2007-10-30 by cmkwon, each nation happy hour event system - added
 
// 2007-11-05 by cmkwon, WP award command - added
#define STRCMD_CS_COMMAND_WAPPOINTINMAP_0			"/AddWarPointInMap"
#define STRCMD_CS_COMMAND_WAPPOINTINMAP_1			"/WPAddedMap"
#define STRCMD_CS_COMMAND_WAPPOINTINMAP_2			"/WPAddedMap"
#define STRCMD_CS_COMMAND_WAPPOINTINMAP_HELP		"format: /WPAddedMap [1(Normal)|2(BCU)|4(ANI)|3|5|6|7] [AddWarPoint(1~)] - Users seçildi ülke hediye harita olacak awarded WarPoint."

// 2007-11-19 by cmkwon, CallGM system - new command
#define STRCMD_CS_COMMAND_STARTCALLGM_0			"/StartCallGM"
#define STRCMD_CS_COMMAND_STARTCALLGM_1			"/StartHelper"
#define STRCMD_CS_COMMAND_STARTCALLGM_2			"/StartHelp"
#define STRCMD_CS_COMMAND_STARTCALLGM_HELP		"format: /StartHelper [|zaman(Unit: dakika)] - Start appeal sistem"
#define STRCMD_CS_COMMAND_ENDCALLGM_0				"/EndCallGM"
#define STRCMD_CS_COMMAND_ENDCALLGM_1				"/EndHelper"
#define STRCMD_CS_COMMAND_ENDCALLGM_2				"/EndHelp"
#define STRCMD_CS_COMMAND_ENDCALLGM_HELP			"format: /EndHelper - End appeal sistem"

// 2007-12-27 by cmkwon, 윈도우즈 모드 기능 추가 - STRMSG_REG_KEY_NAME_WINDOWDEGREE_NEW 추가
#define STRMSG_REG_KEY_NAME_WINDOWDEGREE_NEW		"WindowDegreeNew"

// 2008-01-03 by cmkwon, 윈도우모드 상태 저장하기 - 
#define STRMSG_REG_KEY_NAME_WINDOWMODE				"WindowMode"
#define STRMSG_REG_KEY_NAME_64BIT				"Is64Bit"

// 2008-01-31 by cmkwon, 계정 블럭/해제 명령어로 가능한 시스템 구현 - 명령어 추가
#define STRCMD_CS_COMMAND_NEWACCOUNTBLOCK_0					"/Block"
#define STRCMD_CS_COMMAND_NEWACCOUNTBLOCK_1					"/Block"
#define STRCMD_CS_COMMAND_NEWACCOUNTBLOCK_2					"/Ban"
#define STRCMD_CS_COMMAND_NEWACCOUNTBLOCK_HELP				"format: /Block [AccountName] [BlockType(1:Normal|2:ile ilgili Money|3:ile ilgili eşya|4:ile ilgili SpeedHack|5:ile ilgili sohbet|6:ile ilgili GameBug)] [Period:gün] [Block Reason içiçinde kullanıcı] / [Block Reason içiçinde sadece yönetici]"
#define STRCMD_CS_COMMAND_NEWACCOUNTUNBLOCK_0				"/Unblock"
#define STRCMD_CS_COMMAND_NEWACCOUNTUNBLOCK_1				"/Unblock"
#define STRCMD_CS_COMMAND_NEWACCOUNTUNBLOCK_2				"/Unban"
#define STRCMD_CS_COMMAND_NEWACCOUNTUNBLOCK_HELP			"format: /Unblock [AccountName]"

// 2008-02-20 by cmkwon, 명령어추가(접속중인유저모두에게아이템지급- 
#define STRCMD_CS_COMMAND_ITEMALLUSER_0                                    "/ItemAllUser"
#define STRCMD_CS_COMMAND_ITEMALLUSER_1                                    "/ItemAllUser"
#define STRCMD_CS_COMMAND_ITEMALLUSER_2                                    "/ItemAllUser"
#define STRCMD_CS_COMMAND_ITEMALLUSER_HELP                               "format: /ItemAllUser [1(Normal)|2(BCU)|4(ANI)|255(tüm)] [eşya sayı] [# eşyalar] - Logged üzerinde kullanıcı seçildi ülke will receive designated eşya"

// 2008-02-21 by dhjin, 아레나통합- 아레나추가명령어
#define STRCMD_CS_COMMAND_ARENAMOVE_0                                                         "/ArenaMove"
#define STRCMD_CS_COMMAND_ARENAMOVE_1                                                         "/ArenaMove"
#define STRCMD_CS_COMMAND_TEAMARENALEAVE_0                                                  "/TeamArenaLeave"
#define STRCMD_CS_COMMAND_TEAMARENALEAVE_1                                                  "/TeamServerReturn"
#define STRCMD_CS_COMMAND_TEAMARENALEAVE_HELP                                   "format: /TeamArenaLeave [2(BLUE)|4(RED)|6(BLUE ve RED)]"
#define STRCMD_CS_COMMAND_TARGETARENALEAVE_0                                     "/TargetArenaLeave"
#define STRCMD_CS_COMMAND_TARGETARENALEAVE_1                                     "/TargetArenaLeave"
#define STRCMD_CS_COMMAND_TARGETARENALEAVE_HELP                                 "format: /TargetArenaLeave [Charactername]"
#define STRCMD_CS_COMMAND_ARENAEVENT_0                                                         "/ArenaEvent"
#define STRCMD_CS_COMMAND_ARENAEVENT_1                                                         "/ArenaEvent"
#define STRCMD_CS_COMMAND_ARENAEVENT_2                                                         "/ArenaEvent"
#define STRCMD_CS_COMMAND_ARENAEVENT_HELP                                                    "format: /ArenaEvent [RoomNumber]"
#define STRCMD_CS_COMMAND_ARENAEVENTRELEASE_0                                    "/ArenaEventRelease"
#define STRCMD_CS_COMMAND_ARENAEVENTRELEASE_1                                    "/CancelArenaEvent"
#define STRCMD_CS_COMMAND_ARENAEVENTRELEASE_2                                    "/CancelArenaEvent"
#define STRCMD_CS_COMMAND_ARENAEVENTRELEASE_HELP                                "format: /ArenaEventRelease [RoomNumber]"

// 2008-06-03 by cmkwon, AdminTool, DBTool 사용시 아이템 검색시 콤보박스에서 검색 기능 추가(K0000143) - 
#define STRCMD_CS_ITEMKIND_ALL_ITEM							"tüm Kind"

//////////////////////////////////////////////////////////////////////////
// 2008-05-20 by dhjin, EP3 - 여단 수정 사항	// 2006-03-07 by cmkwon
#define STRCMD_CS_GUILD_RANK_PRIVATE_NULL		"Member"
#define STRCMD_CS_GUILD_RANK_COMMANDER			"Flight tugay Commander"
#define STRCMD_CS_GUILD_RANK_SUBCOMMANDER		"Deputy tugay Commander"				// 2008-05-20 by dhjin, EP3 - 여단 수정 사항
#define STRCMD_CS_GUILD_RANK_SQUAD_LEADER_1		"1st Battalion Commander"
#define STRCMD_CS_GUILD_RANK_PRIVATE_1			"1st Battalion üye"
#define STRCMD_CS_GUILD_RANK_SQUAD_LEADER_2		"2nd Battalion Commander"
#define STRCMD_CS_GUILD_RANK_PRIVATE_2			"2nd Battalion üye"
#define STRCMD_CS_GUILD_RANK_SQUAD_LEADER_3		"3rd Battalion Commander"
#define STRCMD_CS_GUILD_RANK_PRIVATE_3			"3rd Battalion üye"
#define STRCMD_CS_GUILD_RANK_SQUAD_LEADER_4		"4th Battalion Commander"
#define STRCMD_CS_GUILD_RANK_PRIVATE_4			"4th Battalion üye"
#define STRCMD_CS_GUILD_RANK_SQUAD_LEADER_5		"5th Battalion Commander"
#define STRCMD_CS_GUILD_RANK_PRIVATE_5			"5th Battalion üye"

//////////////////////////////////////////////////////////////////////////
// 2008-06-19 by dhjin, EP3 - 전장정보
#define STRCMD_COMMAND_WAR_OPTION_0					"/MotherShipInfoOption"
#define STRCMD_COMMAND_WAR_OPTION_1					"/MotherShipInfoOption"

// 2008-08-18 by dhjin, 세력마크이벤트 
#define STRCMD_CS_COMMAND_INFLUENCEMARKEVENT_0				"/influencemarkevent"
#define STRCMD_CS_COMMAND_INFLUENCEMARKEVENT_1				"/influencemarkevent"
#define STRCMD_CS_COMMAND_INFLUENCEMARKEVENT_2				"/influencemarkevent"
#define STRCMD_CS_COMMAND_INFLUENCEMARKEVENT_HELP			"format: /influencemarkevent [ülke 2(BCU)|4(ANI)]"
#define STRCMD_CS_COMMAND_INFLUENCEMARKEVENTEND_0			"/influencemarkeventend"
#define STRCMD_CS_COMMAND_INFLUENCEMARKEVENTEND_1			"/influencemarkeventend"
#define STRCMD_CS_COMMAND_INFLUENCEMARKEVENTEND_2			"/influencemarkeventend"
#define STRCMD_CS_COMMAND_INFLUENCEMARKEVENTEND_HELP		"format: /influencemarkeventend"

//////////////////////////////////////////////////////////////////////////
// 2008-08-25 by dhjin, 태국 PC방 IP정보 로딩
#define STRCMD_CS_COMMAND_PCBANGRELOADTIME_0				"/PCBangReloadTime"
#define STRCMD_CS_COMMAND_PCBANGRELOADTIME_1				"/PCBangReloadTime"
#define STRCMD_CS_COMMAND_PCBANGRELOADTIME_HELP				"format: /PCBangreloadtime [dakika] - 10 Min ~ 1440 Min"


// 2008-08-21 by dhjin, 일반, 특수 계정의 부지도자 임명 제한
#define STRMSG_080821_0001				"Cannot appoint seçildi karakter ile rütbe."


// 2008-09-09 by cmkwon, /세력소환 명령어 인자 리스트에 기어타입 추가 - 
#define STRCMD_CS_COMMAND_COMEONINFL_HELP2		"format: /ComeOnInfl [1(Normal)|2(BCU)|4(ANI)|255(tüm)] [maksimum people] [0|minimum seviye] [0|maksimum seviye] [1(B)|16(M)|256(A)|4096(I)|4369(tüm)] [mesaj -e kullanıcı] - Request -e certain ülke, seviye users -e hareket et -e sizin position."

// 2008-09-09 by cmkwon, "/kick" 명령어 추가 - 
#define STRCMD_CS_COMMAND_KICK_0							"/Kick"
#define STRCMD_CS_COMMAND_KICK_1							"/Kick"
#define STRCMD_CS_COMMAND_KICK_HELP							"format: /Kick [CharacterName] - Terminate designated karakter -den game."


// 2008-09-12 by cmkwon, "/명성" 명령어 추가 - 
#define STRCMD_CS_COMMAND_ADD_FAME_0							"/Fame"
#define STRCMD_CS_COMMAND_ADD_FAME_1							"/Fame"
#define STRCMD_CS_COMMAND_ADD_FAME_HELP							"format: /fame [personal fame] [tugay fame] - Raises personal ve tugay fame karakter."

// 2008-12-30 by cmkwon, 지도자 채팅 제한 카드 구현 - 
#define STRCMD_CS_COMMAND_CHATFORBIDRELEASE_LEADER_0			"/ReleaseLeaderChatBlock"
#define STRCMD_CS_COMMAND_CHATFORBIDRELEASE_LEADER_1			"/ReleaseLeaderChatBlock"
#define STRCMD_CS_COMMAND_CHATFORBIDRELEASE_LEADER_HELP			"format: /ReleaseLeaderChatBlock [CharacterName] - Leader sohbet restriction released."

// 2009-10-12 by cmkwon, 프리스카 제거 방안 적용 - 
#define STRCMD_CS_COMMAND_CHANGE_StartCityMapIndex_0                               "/StartCityMap"
#define STRCMD_CS_COMMAND_CHANGE_StartCityMapIndex_1                               "/StartCity"
#define STRCMD_CS_COMMAND_CHANGE_StartCityMapIndex_HELP                           "format: /StartCity [2001|2002] [|CharacterName] - Set karakter`s beginning şehir -e 2001 veya 2002 when their ülke initialized."


///////////////////////////////////////////////////////////////////////////////
// 2010-01-08 by cmkwon, 최대 레벨 상향에 따른 추가 사항(레벨별 계급) - 계급뜻(백부장, 대령, 장군, 총독, 정복자, 수호자, 전설적인)
#define       STRCMD_CS_CHARACTER_96_LEVEL_RANK             "Centurion"
#define       STRCMD_CS_CHARACTER_100_LEVEL_RANK            "Colonel"
#define       STRCMD_CS_CHARACTER_104_LEVEL_RANK            "General"
#define       STRCMD_CS_CHARACTER_108_LEVEL_RANK            "Governer"
#define       STRCMD_CS_CHARACTER_112_LEVEL_RANK            "Conqueror"
#define       STRCMD_CS_CHARACTER_116_LEVEL_RANK            "Guardian"
#define       STRCMD_CS_CHARACTER_120_LEVEL_RANK            "Legendary"
#define       STRCMD_CS_CHARACTER_XX_LEVEL_RANK             "temel"

//30-03-2017 by Inetpub - advanced Team ranks
#define		TMRANK_TGM		"tGM"
#define		TMRANK_GM		"GM"
#define		TMRANK_GA		"GA"
#define		TMRANK_SGA		"SGA"
#define		TMRANK_EM		"EM"
#define		TMRANK_DEV		"Dev"
#define		TMRANK_ADMIN	"Admin"
//end of inet

#define STRCMD_CS_COMMAND_EVO_WHO						"/evo_who"


#define STRMSG_C_131205_0001	"Entirety"
#define STRMSG_C_131205_0002	"Weapon"
#define STRMSG_C_131205_0003	"standart silah"
#define STRMSG_C_131205_0004	"Vulcan"
#define STRMSG_C_131205_0005	"Cannon"
#define STRMSG_C_131205_0006	"Gatling"
#define STRMSG_C_131205_0007	"Rifle"
#define STRMSG_C_131205_0008	"Automatic"
#define STRMSG_C_131205_0009	"Dualist"
#define STRMSG_C_131205_0010	"Mess Drive"
#define STRMSG_C_131205_0011	"gelişmiş silah"
#define STRMSG_C_131205_0012	"Missile"
#define STRMSG_C_131205_0013	"Bundle"
#define STRMSG_C_131205_0014	"Armour"
#define STRMSG_C_131205_0015	"Veil"
#define STRMSG_C_131205_0016	"Defender"
#define STRMSG_C_131205_0017	"Guarder"
#define STRMSG_C_131205_0018	"Binder"
#define STRMSG_C_131205_0019	"Radar"
#define STRMSG_C_131205_0020	"Auxiliary ekipman"
#define STRMSG_C_131205_0021	"Infinite Accessory"
#define STRMSG_C_131205_0022	"zaman limit Accessory"
#define STRMSG_C_131205_0023	"Computer"
#define STRMSG_C_131205_0024	"Engine"
#define STRMSG_C_131205_0025	"Expendables"
#define STRMSG_C_131205_0026	"iyileştirme kiti"
#define STRMSG_C_131205_0027	"şans denemesi Kit"
#define STRMSG_C_131205_0028	"büyüle kart"
#define STRMSG_C_131205_0029	"Normal kart"
#define STRMSG_C_131205_0030	"Lucky Box"
#define STRMSG_C_131205_0031	"Etc."
#define STRMSG_C_131205_0032	"Ore"
#define STRMSG_C_131205_0033	"siz've chosen wrong eşya veya eşya mevcut değil."
#define STRMSG_C_131205_0034	"+%d %s ister misiniz satın al? \\n(fiyat: %s SPI)"
#define STRMSG_C_131205_0035	"+%d %s ister misiniz satın al? \\n(fiyat: %s WP)"
#define STRMSG_C_131205_0036	"( sayı eşyalar: %d)"
#define STRMSG_C_131205_0037	"It -ebilir't be kaydoldu due -e blank."
#define STRMSG_C_131205_0038	"ister misiniz kaydet eşya ticaret mağaza?"
#define STRMSG_C_131205_0039	"eşya şimdi başarıyla kaydoldu ticaret mağaza."
#define STRMSG_C_131205_0040	"It's expired."
#define STRMSG_C_131205_0041	"%dgün"
#define STRMSG_C_131205_0042	"%dzaman"
#define STRMSG_C_131205_0043	"%ddakika"
#define STRMSG_C_131205_0044	"daha az than dakika"
#define STRMSG_C_131205_0045	"\\ySold\\y"
#define STRMSG_C_131205_0046	"\\rExpired\\r"
#define STRMSG_C_131205_0047	"ister misiniz recall eşya kaydoldu ticaret mağaza?"
#define STRMSG_C_131205_0048	"Would like -e recall eşya which has expired?"
#define STRMSG_C_131205_0049	"ister misiniz withdraw money sold eşya?\\n(Sales Charge: %s %s)"
#define STRMSG_C_131205_0050	"lütfen bekleyin few saniye ve tekrar deneyin."
#define STRMSG_C_131205_0051	"Selling eşyalar oluyor recalled."
#define STRMSG_C_131205_0052	"Expired eşyalar oluyor recalled."
#define STRMSG_C_131205_0053	"Selling fiyat withdrawn."
#define STRMSG_C_131205_0054	"başarısız oldu: recall. lütfen tekrar deneyin."
#define STRMSG_C_131205_0055	"Recalled"
#define STRMSG_C_131205_0056	"Money withdrawn"
// END 2013-12-05 by ymjoo 거래소 구현 스트링

// 2013-12-05 by ymjoo 거래소 구현 스트링
#define STRMSG_C_131206_0001	"siz bought eşya başarıyla.\\n eşya olacak sent -e sizin envanter."

#define STRMSG_C_131206_0002	"\\y %d savaş puanı has azaltıldı."

// 2013-11-29 by ssjung 거래소 구현
#define STRMSG_C_131217_0001	"siz have exceeded sayı eşyalar -e be kaydoldu."
#define STRMSG_C_131217_0002	"It's sold out."
#define STRMSG_C_131217_0003	"siz -ebilir't satın al due -e yetersiz funds."
#define STRMSG_C_131217_0004	"siz -ebilir't satın al due -e maksimum sayı eşyalar has exceeded."
#define STRMSG_C_131217_0005	"siz -ebilir't satın al because yetersiz envanter alan."
#define STRMSG_C_131217_0006	"siz -ebilir't recall due -e maksimum sayı eşyalar has exceeded."
#define STRMSG_C_131217_0007	"siz -ebilir't recall because yetersiz envanter alan."
#define STRMSG_C_131217_0008	"Recall fees: SPI(%.1f%%), WP(%.1f%%)"
#define STRMSG_C_131217_0009	"Please update list eşyalar ticaret mağaza."
// end 2013-11-29 by ssjung 거래소 구현


#endif // end_#ifndef _STRING_DEFINE_COMMON_H_





