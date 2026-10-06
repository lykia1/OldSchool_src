// 2005-04-28 by cmkwon
// #include "StringDefineCommon.h"
#ifndef _STRING_DEFINE_COMMON_H_
#define _STRING_DEFINE_COMMON_H_

///////////////////////////////////////////////////////////////////////////////
// 1 - Command handled at the FieldServer
	#define STRCMD_CS_COMMAND_MENT_0					"/ment"
	#define STRCMD_CS_COMMAND_MENT_1					"/ment"
	#define STRCMD_CS_COMMAND_MENT_2					"/ment"
	#define STRCMD_CS_COMMAND_MENT_HELP					"kullanim: /ment [|Metin] - Karakterin ment metnini ayarlar. Mevcut menti iptal etmek icin bos birakin."
	#define STRCMD_CS_COMMAND_MOVE						"/move"
	#define STRCMD_CS_COMMAND_MOVE_1					"/go"
	#define STRCMD_CS_COMMAND_MOVE_HELP					"kullanim: /move [MapIndex] [|ChannelIndex] - Belirtilen harita ve kanala tasir"
	#define STRCMD_CS_COMMAND_COORDINATE				"/coor"
	#define STRCMD_CS_COMMAND_COORDINATE_1				"/coordinate"
	#define STRCMD_CS_COMMAND_COORDINATE_HELP			"kullanim: /coordinate [X] [Y] - Haritadaki belirtilen koordinatlara tasir"
	#define STRCMD_CS_COMMAND_LIST						"/list"
	#define STRCMD_CS_COMMAND_LIST_1					"/list"
	#define STRCMD_CS_COMMAND_LIST_HELP					"kullanim: /list - Mevcut haritadaki kullanicilari listeler (en fazla 20 kullanici)"
	#define STRCMD_CS_COMMAND_USERSEND					"/senduser"
	#define STRCMD_CS_COMMAND_USERSEND_1				"/senduser"
	#define STRCMD_CS_COMMAND_USERSEND_HELP				"kullanim: /senduser [karakter adi] [harita adi] - Karakteri belirtilen haritaya tasir"
	#define STRCMD_CS_COMMAND_INFObyNAME				"/info"
	#define STRCMD_CS_COMMAND_INFObyNAME_1				"/info"
	#define STRCMD_CS_COMMAND_INFObyNAME_HELP			"kullanim: /info [canavar adi|esya adi] - Metinle eslesen canavar veya esya bilgisini gosterir"
	#define STRCMD_CS_COMMAND_QUESTINFO					"/quest"
	#define STRCMD_CS_COMMAND_QUESTINFO_1				"/quest"
	#define STRCMD_CS_COMMAND_QUESTINFO_HELP			"kullanim: /quest - Karakterin gorev bilgisini gosterir"
	#define STRCMD_CS_COMMAND_QUESTDEL					"/delQuest"
	#define STRCMD_CS_COMMAND_QUESTDEL_1				"/delQuest"
	#define STRCMD_CS_COMMAND_QUESTDEL_HELP				"kullanim: /delQuest [gorev no.]"
	#define STRCMD_CS_COMMAND_ITEMINFObyKIND			"/itemKind"
	#define STRCMD_CS_COMMAND_ITEMINFObyKIND_1			"/itemKind"
	#define STRCMD_CS_COMMAND_ITEMINFObyKIND_HELP		"kullanim: /itemKind [|esya turu(0~53)] - Belirtilen esya turlerini gosterir"
	#define STRCMD_CS_COMMAND_ITEMINSERTbyKIND			"/insertItemKind"
	#define STRCMD_CS_COMMAND_ITEMINSERTbyKIND_1		"/insertItemKind"
	#define STRCMD_CS_COMMAND_ITEMINSERTbyKIND_HELP		"kullanim: /insertItemKind [esya turu(0~53)] - Belirtilen turde esyalar ekler"
	#define STRCMD_CS_COMMAND_ITEMINSERTbyITEMNUMRANGE		"/insertItemNumRange"
	#define STRCMD_CS_COMMAND_ITEMINSERTbyITEMNUMRANGE_1	"/insertItemNumRange"
	#define STRCMD_CS_COMMAND_ITEMINSERTbyITEMNUMRANGE_HELP	"kullanim: /insertItemNumRange [baslangic esya no] ~ [bitis esya no] - Belirtilen esyalari ekler"
	#define STRCMD_CS_COMMAND_STATINIT					"/initStat"
	#define STRCMD_CS_COMMAND_STATINIT_1				"/initStatus"
	#define STRCMD_CS_COMMAND_STATINIT_2				"/initStatus"
	#define STRCMD_CS_COMMAND_STATINIT_HELP				"kullanim: /initStatus - Tum statlari sifirlar"
	#define STRCMD_CS_COMMAND_PARTYINFO					"/partyInfo"
	#define STRCMD_CS_COMMAND_PARTYINFO_1				"/partyInfo"
	#define STRCMD_CS_COMMAND_PARTYINFO_HELP			"kullanim: /partyInfo - Formasyon bilgisini gosterir"
	#define STRCMD_CS_COMMAND_GAMETIME					"/Time"
	#define STRCMD_CS_COMMAND_GAMETIME_1				"/Time"
	#define STRCMD_CS_COMMAND_GAMETIME_HELP				"kullanim: /Time [|ek sure(0~23)] - Mevcut zamani degistirir (yalnizca bireysel zaman)"
	#define STRCMD_CS_COMMAND_STRINGLEVEL_0				"/string"
	#define STRCMD_CS_COMMAND_STRINGLEVEL_1				"/string"
	#define STRCMD_CS_COMMAND_STRINGLEVEL_2				"/string"
	#define STRCMD_CS_COMMAND_STRINGLEVEL_HELP			"kullanim: /string [0~5] - Debug mesajlarinda gosterilecek seviyeyi belirler"
	#define STRCMD_CS_COMMAND_MONSUMMON					"/summon"
	#define STRCMD_CS_COMMAND_MONSUMMON_1				"/summon"
	#define STRCMD_CS_COMMAND_MONSUMMON_HELP			"kullanim: /summon [canavar numarasi|canavar adi] [canavar sayisi] - Canavar cagirir (adinda bosluk varsa '_' kullanilir)"
	#define STRCMD_CS_COMMAND_SKILLALL					"/allSkill"
	#define STRCMD_CS_COMMAND_SKILLALL_1				"/allSkill"
	#define STRCMD_CS_COMMAND_SKILLALL_HELP				"kullanim: /allSkill [seviye] - Seviyeye uygun tum yetenekleri ekler"
	#define STRCMD_CS_COMMAND_ITEMINSERTALL				"/allItem"
	#define STRCMD_CS_COMMAND_ITEMINSERTALL_1			"/allItem"
	#define STRCMD_CS_COMMAND_ITEMINSERTALL_HELP		"kullanim: /allItem - Yetenekler ve sayilabilir esyalar haric belirtilen tum esyalari ekler"
	#define STRCMD_CS_COMMAND_ITEMINSERTWEAPON			"/allWeapon"
	#define STRCMD_CS_COMMAND_ITEMINSERTWEAPON_1		"/allWeapon"
	#define STRCMD_CS_COMMAND_ITEMINSERTWEAPON_HELP		"kullanim: /allWeapon - GEAR'a uygun tum silahlari ekler"
	#define STRCMD_CS_COMMAND_ITEMDELALL_0				"/delAllItem"
	#define STRCMD_CS_COMMAND_ITEMDELALL_1				"/delAllItem"
	#define STRCMD_CS_COMMAND_ITEMDELALL_2				"/delAllItem"
	#define STRCMD_CS_COMMAND_ITEMDELALL_HELP			"kullanim: /delAllItem - Takili olmayan tum esyalari siler (yetenekler haric)"
	#define STRCMD_CS_COMMAND_ITEMINSERTbyITEMNUM		"/item"
	#define STRCMD_CS_COMMAND_ITEMINSERTbyITEMNUM_1		"/item"
	#define STRCMD_CS_COMMAND_ITEMINSERTbyITEMNUM_HELP	"kullanim: /item [esya numarasi] [esya sayisi] - Esya verir; sayi girilmezse 1 adet kabul edilir"
	#define STRCMD_CS_COMMAND_ITEMDROP					"/dropItem"
	#define STRCMD_CS_COMMAND_ITEMDROP_1				"/dropItem"
	#define STRCMD_CS_COMMAND_ITEMDROP_HELP				"kullanim: /dropItem [esya numarasi] [|esya sayisi] - Esyayi alana birakir"
	#define STRCMD_CS_COMMAND_USERSINFOTOTAL			"/server"
	#define STRCMD_CS_COMMAND_USERSINFOTOTAL_1			"/server"
	#define STRCMD_CS_COMMAND_USERSINFOTOTAL_HELP		"kullanim: /server - Sunucu bilgisini listeler"
	#define STRCMD_CS_COMMAND_USERSINFOperMAP			"/serverMap"
	#define STRCMD_CS_COMMAND_USERSINFOperMAP_1			"/serverMap"
	#define STRCMD_CS_COMMAND_USERSINFOperMAP_HELP		"kullanim: /serverMap - Tum haritalarin bilgisini gosterir"
	#define STRCMD_CS_COMMAND_CHANNELINFO				"/channelInfo"
	#define STRCMD_CS_COMMAND_CHANNELINFO_1				"/channelInfo"
	#define STRCMD_CS_COMMAND_CHANNELINFO_HELP			"kullanim: /channelInfo - Mevcut haritanin kanal bilgilerini gosterir"
	#define STRCMD_CS_COMMAND_DEBUGPRINTDBG				"/dbg"
	#define STRCMD_CS_COMMAND_DEBUGPRINTDBG_1			"/dbg"
	#define STRCMD_CS_COMMAND_DEBUGPRINTDBG_HELP		"kullanim: /dbg - Yalnizca test icin"
	#define STRCMD_CS_COMMAND_DEBUGSETPARAMF			"/testf"
	#define STRCMD_CS_COMMAND_DEBUGSETPARAMF_HELP		"format: /testf [Param1] [Param2] [Param3]"
	#define STRCMD_CS_COMMAND_BULLETCHARGE				"/bullet"
	#define STRCMD_CS_COMMAND_BULLETCHARGE_1			"/chargeBullet"
	#define STRCMD_CS_COMMAND_BULLETCHARGE_HELP			"kullanim: /chargeBullet [|1. tur mermi sayisi] [|2. tur mermi sayisi] - Mermileri yeniden doldurur"
	#define STRCMD_CS_COMMAND_REPAIRALL					"/manpi"
	#define STRCMD_CS_COMMAND_REPAIRALL_1				"/repairAll"
	#define STRCMD_CS_COMMAND_REPAIRALL_HELP			"kullanim: /repairAll [|karakter adi] - Enerji, Kalkan, SP ve Yakiti %%100 yeniler; karakter adi girilmezse kendinizi yeniler"
	#define STRCMD_CS_COMMAND_REPAIRbyPARAM				"/banpi"
	#define STRCMD_CS_COMMAND_REPAIRbyPARAM_1			"/repairParam"
	#define STRCMD_CS_COMMAND_REPAIRbyPARAM_HELP		"kullanim: /repairParam [|azalma miktari %%] [|karakter adi] - Enerji, Kalkan, SP ve Yakit icin verilen orani uygular; oran girilmezse %%50, karakter adi yoksa kendi karakteriniz kullanilir"
	#define STRCMD_CS_COMMAND_USERNORMALIZE				"/normal"
	#define STRCMD_CS_COMMAND_USERNORMALIZE_1			"/normal"
	#define STRCMD_CS_COMMAND_USERNORMALIZE_HELP		"kullanim: /normal - Yonetici veya GM modundan normal hesaba doner"
	#define STRCMD_CS_COMMAND_USERSPECIALIZE			"/specialize"
	#define STRCMD_CS_COMMAND_USERSPECIALIZE_1			"/specialize"
	#define STRCMD_CS_COMMAND_USERSPECIALIZE_HELP		"kullanim: /specialize - Normal hesabi ozel yonetici veya GM hesabina cevirir"
	#define STRCMD_CS_COMMAND_USERINVINCIBILITY			"/godmode"
	#define STRCMD_CS_COMMAND_USERINVINCIBILITY_1		"/invincible"
	#define STRCMD_CS_COMMAND_USERINVINCIBILITY_HELP	"kullanim: /invincible - GM veya yonetici modunda hasar almaz"
	#define STRCMD_CS_COMMAND_POWERUP					"/powerUp"
	#define STRCMD_CS_COMMAND_POWERUP_1					"/powerUp"
	#define STRCMD_CS_COMMAND_POWERUP_HELP				"kullanim: /powerUp [Saldiri gucu artisi(%%)]"
	#define STRCMD_CS_COMMAND_VARIABLESET				"/setVariable"
	#define STRCMD_CS_COMMAND_VARIABLESET_1				"/setVariable"
	#define STRCMD_CS_COMMAND_VARIABLESET_HELP			"kullanim: /setVariable [degisken] - Normal degiskenleri ayarlar"
	#define STRCMD_CS_COMMAND_LEVELSET					"/level"
	#define STRCMD_CS_COMMAND_LEVELSET_1				"/level"
	#define STRCMD_CS_COMMAND_LEVELSET_HELP				"kullanim: /level [|seviye] [|EXP yuzdesi] [|karakter adi] - Karakterin seviye ve EXP yuzdesini ayarlar"
	#define STRCMD_CS_COMMAND_USERINVISIABLE			"/invisible"
	#define STRCMD_CS_COMMAND_USERINVISIABLE_1			"/hide"
	#define STRCMD_CS_COMMAND_USERINVISIABLE_HELP		"kullanim: /invisible - Diger karakterlere gorunmez olur"
	#define STRCMD_CS_COMMAND_DEBUGPRINTMSGF_0			"/messagef"
	#define STRCMD_CS_COMMAND_DEBUGPRINTMSGF_1			"/msgf"
	#define STRCMD_CS_COMMAND_DEBUGPRINTMSGF_HELP		"kullanim: /msgf - Yalnizca test icin"
	#define STRCMD_CS_COMMAND_GAMEEVENT					"/event"
	#define STRCMD_CS_COMMAND_GAMEEVENT_1				"/event"
	#define STRCMD_CS_COMMAND_GAMEEVENT_P1EXP			"exppoint"
	#define STRCMD_CS_COMMAND_GAMEEVENT_P1SPI			"SPI"
	#define STRCMD_CS_COMMAND_GAMEEVENT_P1EXPR			"restoreexppoint"
	#define STRCMD_CS_COMMAND_GAMEEVENT_P1ITEM			"item"
	#define STRCMD_CS_COMMAND_GAMEEVENT_P1RARE			"rareitem"
	#define STRCMD_CS_COMMAND_GAMEEVENT_P2END			"finish"
	#define STRCMD_CS_COMMAND_GAMEEVENT_HELP			"kullanim: /event [exppoint|SPI|restoreexppoint|item|rareitem] [|oran %f|finish] [sure(dakika)] - Etkinligi ayarlar veya iptal eder"
	#define STRCMD_CS_COMMAND_PREMEUM					"/premium"
	#define STRCMD_CS_COMMAND_PREMEUM_1					"/premium"
	#define STRCMD_CS_COMMAND_PREMEUM_PNORMAL			"standard"
	#define STRCMD_CS_COMMAND_PREMEUM_PSUPER			"super"
	#define STRCMD_CS_COMMAND_PREMEUM_PUPGRADE			"upgrade"
	#define STRCMD_CS_COMMAND_PREMEUM_PEND				"finish"
	#define STRCMD_CS_COMMAND_PREMEUM_HELP				"kullanim: /premium [standard|super|upgrade|finish]"
// 2008-02-14 by cmkwon,   
//	#define STRCMD_CS_COMMAND_CITYWAR					"/citywar"
//	#define STRCMD_CS_COMMAND_CITYWAR_1					"/citywar"
//	#define STRCMD_CS_COMMAND_CITYWAR_PSTART			"start"
//	#define STRCMD_CS_COMMAND_CITYWAR_PEND				"finish"
//	#define STRCMD_CS_COMMAND_CITYWAR_HELP				"format: /citywar [start|finish]"
	#define STRCMD_CS_COMMAND_STEALTH					"/stealth"
	#define STRCMD_CS_COMMAND_STEALTH_1					"/stealth"
	#define STRCMD_CS_COMMAND_STEALTH_HELP				"format: /stealth"
	#define STRCMD_CS_COMMAND_RETURNALL					"/returnAll"
	#define STRCMD_CS_COMMAND_RETURNALL_1				"/returnAll"
	#define STRCMD_CS_COMMAND_RETURNALL_HELP			"kullanim: /returnAll [MapIndex] - Haritadaki tum uyeleri ulus baskentine tasir"

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
	#define STRCMD_CS_COMMAND_DEBUGSETPARAMI_HELP		"kullanim: /testi - IMServer debug icin"
	#define STRCMD_CS_COMMAND_WHO						"/who"
	#define STRCMD_CS_COMMAND_WHO_1						"/who"
	#define STRCMD_CS_COMMAND_WHO_HELP					"kullanim: /who [|kullanici sayisi] - Sunucudaki tum kullanicilari listeler (haritadan bagimsiz)"
	#define STRCMD_CS_COMMAND_REGISTERADMIN				"/registerAdmin"
	#define STRCMD_CS_COMMAND_REGISTERADMIN_1			"/registerAdmin"
	#define STRCMD_CS_COMMAND_REGISTERADMIN_HELP		"kullanim: /registerAdmin - Etkinlik sirasinda yoneticiye mesaj gondermek icin sunucuyu kaydeder"
	#define STRCMD_CS_COMMAND_DEBUGPRINTMSGI_0			"/messagei"
	#define STRCMD_CS_COMMAND_DEBUGPRINTMSGI_1			"/msgi"
	#define STRCMD_CS_COMMAND_DEBUGPRINTMSGI_HELP		"kullanim: /msgi - Istemci ile IM sunucusu arasindaki tum protokolleri gosterir"
	#define STRCMD_CS_COMMAND_SERVERDOWN				"/serverDown"
	#define STRCMD_CS_COMMAND_SERVERDOWN_1				"/serverDown"
	#define STRCMD_CS_COMMAND_SERVERDOWN_HELP			"kullanim: /serverDown [onay no.] - Sunucuyu kapatir"
	#define STRCMD_CS_COMMAND_WHOAREYOU					"/donttrytousethiscommandorelseyouwillgetbannedohyeahwewillfindyouguyseventuallyandthisisapermaban"
	#define STRCMD_CS_COMMAND_WHOAREYOU_1				"/donttrytousethiscommandorelseyouwillgetbannedohyeahwewillfindyouguyseventuallyandthisisapermaban"
	#define STRCMD_CS_COMMAND_WHOAREYOU_HELP			"kullanim: /whoareYou [karakter adi] - Devre disi"
	#define STRCMD_CS_COMMAND_GOUSER					"/go"
	#define STRCMD_CS_COMMAND_GOUSER_1					"/go"
	#define STRCMD_CS_COMMAND_GOUSER_HELP				"kullanim: /go [karakter adi] - Belirtilen karakterin konumuna tasir"
	#define STRCMD_CS_COMMAND_COMEON					"/comeon"
	#define STRCMD_CS_COMMAND_COMEON_1					"/comeon"
	#define STRCMD_CS_COMMAND_COMEON_HELP				"kullanim: /comeon [karakter adi] - Belirtilen karakteri yaniniza cagirir"
	#define STRCMD_CS_COMMAND_GUILDCOMEON				"/comeonGuild"
	#define STRCMD_CS_COMMAND_GUILDCOMEON_1				"/comeonGuild"
	#define STRCMD_CS_COMMAND_GUILDCOMEON_HELP			"kullanim: /comeonGuild [tugay adi] - Tum tugayi yaniniza cagirir"
	#define STRCMD_CS_COMMAND_GUILDSEND					"/sendGuild"
	#define STRCMD_CS_COMMAND_GUILDSEND_1				"/sendGuild"
	#define STRCMD_CS_COMMAND_GUILDSEND_HELP			"kullanim: /sendGuild [tugay adi] [harita adi] - Tugayi belirtilen haritaya gonderir"
	#define STRCMD_CS_COMMAND_CHATPTOPFLAG				"/whisperChat"
	#define STRCMD_CS_COMMAND_CHATPTOPFLAG_1			"/whisperChat"
	#define STRCMD_CS_COMMAND_CHATPTOPFLAG_HELP			"kullanim: /whisperChat - Fisiltilari engelleme/izin verme durumunu degistirir"
	#define STRCMD_CS_COMMAND_GUILDINFO					"/guildInfo"
	#define STRCMD_CS_COMMAND_GUILDINFO_1				"/guildInfo"
	#define STRCMD_CS_COMMAND_GUILDINFO_HELP			"kullanim: /guildInfo - Tugay bilgisini gosterir"
	#define STRCMD_CS_COMMAND_WEATHERSET				"/weather"
	#define STRCMD_CS_COMMAND_WEATHERSET_1				"/weather"
	#define STRCMD_CS_COMMAND_WEATHERSET_P1NORMAL		"standard"
	#define STRCMD_CS_COMMAND_WEATHERSET_P1FINE			"clear"
	#define STRCMD_CS_COMMAND_WEATHERSET_P1RAIN			"rain"
	#define STRCMD_CS_COMMAND_WEATHERSET_P1SNOW			"snow"
	#define STRCMD_CS_COMMAND_WEATHERSET_P1CLOUDY		"cloudy"
	#define STRCMD_CS_COMMAND_WEATHERSET_P1FOG			"foggy"
	#define STRCMD_CS_COMMAND_WEATHERSET_P2ALL			"whole"
	#define STRCMD_CS_COMMAND_WEATHERSET_P3ON			"on"
	#define STRCMD_CS_COMMAND_WEATHERSET_P3OFF			"off"
	#define STRCMD_CS_COMMAND_WEATHERSET_HELP			"kullanim: /weather [standard|clear|rain|snow|cloudy|foggy] [whole|mapname] [on|off] - Hava durumunu kontrol eder"
	#define STRCMD_CS_COMMAND_CHATFORBID				"/mute"
	#define STRCMD_CS_COMMAND_CHATFORBID_1				"/forbidChat"
	#define STRCMD_CS_COMMAND_CHATFORBID_HELP			"kullanim: /forbidChat [karakter adi] [sure(dk.)] - Sohbeti yasaklar"
	#define STRCMD_CS_COMMAND_CHATFORBIDRELEASE			"/unmute"
	#define STRCMD_CS_COMMAND_CHATFORBIDRELEASE_1		"/releaseChat"
	#define STRCMD_CS_COMMAND_CHATFORBIDRELEASE_HELP	"kullanim: /releaseChat [sure(dk.)] - Sohbet yasagini kaldirir"
	#define STRCMD_CS_COMMAND_COMMANDLIST_0				"/?"
	#define STRCMD_CS_COMMAND_COMMANDLIST_1				"/help"
	#define STRCMD_CS_COMMAND_COMMANDLIST_2				"/command"
	#define STRCMD_CS_COMMAND_COMMANDLIST_HELP			"kullanim: /? - Komut listesini gosterir"

	// 2005-07-20 by cmkwon
	#define STRCMD_CS_COMMAND_BONUSSTAT_0				"/BonusStat"
	#define STRCMD_CS_COMMAND_BONUSSTAT_1				"/BonusStat"
	#define STRCMD_CS_COMMAND_BONUSSTAT_2				"/BonusStat"
	#define STRCMD_CS_COMMAND_BONUSSTAT_HELP			"kullanim: /BonusStat [Bonus Sayisi] [|karakter adi] - BonusStat artirir"
// 2_end
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// 3 - Command used in AtumMonitor, some are used with the same command as the above
	#define STRCMD_CS_COMMAND_PASSWORDSET				"/setPassword"
	#define STRCMD_CS_COMMAND_PASSWORDSET_1				"/setPassword"
	#define STRCMD_CS_COMMAND_PASSWORDSET_HELP			"kullanim: /setPassword [HesapAdi] [Sifre]"
	#define STRCMD_CS_COMMAND_PASSWORDROLLBACK			"/rollbackPassword"
	#define STRCMD_CS_COMMAND_PASSWORDROLLBACK_1		"/rollbackPassword"
	#define STRCMD_CS_COMMAND_PASSWORDROLLBACK_HELP		"kullanim: /rollbackPassword [HesapAdi]"
	#define STRCMD_CS_COMMAND_PASSWORDLIST				"/passwordList"
	#define STRCMD_CS_COMMAND_PASSWORDLIST_1			"/passwordList"
	#define STRCMD_CS_COMMAND_PASSWORDLIST_HELP			"kullanim: /passwordList"
	#define STRCMD_CS_COMMAND_PASSWORDENCRYPT			"/encrypt"
	#define STRCMD_CS_COMMAND_PASSWORDENCRYPT_1			"/encrypt"
	#define STRCMD_CS_COMMAND_PASSWORDENCRYPT_HELP		"kullanim: /encrypt [sifrelenecek metin]"
	#define STRCMD_CS_COMMAND_ACCOUNTBLOCK				"/blockAccount"
	#define STRCMD_CS_COMMAND_ACCOUNTBLOCK_1			"/blockAccount"
	#define STRCMD_CS_COMMAND_ACCOUNTBLOCKT_HELP		"kullanim: /blockAccount [HesapAdi]"
	#define STRCMD_CS_COMMAND_ACCOUNTBLOCKRELEASE		"/releaseAccount"
	#define STRCMD_CS_COMMAND_ACCOUNTBLOCKRELEASE_1		"/releaseAccount"
	#define STRCMD_CS_COMMAND_ACCOUNTBLOCKRELEASE_HELP	"kullanim: /releaseAccount [HesapAdi]"
	#define STRCMD_CS_COMMAND_ACCOUNTBLOCKLIST			"/blockedList"
	#define STRCMD_CS_COMMAND_ACCOUNTBLOCKLIST_1		"/blockedList"
	#define STRCMD_CS_COMMAND_ACCOUNTBLOCKLIST_HELP		"kullanim: /blockedList"
// 3_end
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// 4 - CommonGameServer
	#define STRCMD_CS_COMMON_DB_0000 "Only related to Mgame server only!!!!\r\n"
	#define STRCMD_CS_COMMON_DB_0001 "Please enter server login name"
	#define STRCMD_CS_COMMON_DB_0002 "Please enter server login password"
	#define STRCMD_CS_COMMON_DB_0003 "Please enter DB log in name"
	#define STRCMD_CS_COMMON_DB_0004 "Please enter DB log in password"

	#define STRCMD_CS_COMMON_MAP_0000 "Hard coded part: Ignore number 1 warp target at 0101 map, easy way to delete from map editor need to be found and needs to be deleted!\r\n"
	#define STRCMD_CS_COMMON_MAP_0001 "MAP: %04d, m_DefaltWarpTargetIndex: %d\r\n"
	#define STRCMD_CS_COMMON_MAP_0002 "Hard coded part: Ignore number 1 warp target at 0101 map, easy way to delete from map editor need to be found and needs to be deleted!\r\n"
	#define STRCMD_CS_COMMON_MAP_0003 "MAP: %04d, m_DefaltWarpTargetIndex: %d\r\n"
	#define STRCMD_CS_COMMON_MAP_0004 "    ObjMon ==> ObjNum[%8d] EvType[%d] EvIndex[%3d] summon monster[%8d] summon time[%6dsecond], Pos(%4d, %4d, %4d)\r\n"
	#define STRCMD_CS_COMMON_MAP_0005 "[ERROR] ObjectMonster EventParam1 Index overlap Error ==> ObjectNum[%8d] EventType[%d] EventIndex[%3d] summon monster[%8d] summon time[%6dsecond], Pos(%4d, %4d, %4d)\r\n"
	#define STRCMD_CS_COMMON_MAP_0006 "  Tatal Monster Count : [%4d] <== Including object monster\r\n"

	#define STRCMD_CS_COMMON_DOWNLOAD_0000 "Download file error"
	#define STRCMD_CS_COMMON_DOWNLOAD_0001 "File creation error"
	#define STRCMD_CS_COMMON_DOWNLOAD_0002 "Download file read error"

	#define STRCMD_CS_COMMON_DATETIME_0000 "%dday%dhour%dminute%dsecond"

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
	#define STRCMD_CS_COMMON_RACE_DEMO			"For demo"
	#define STRCMD_CS_COMMON_RACE_ALL			"All race"
	#define STRCMD_CS_COMMON_RACE_UNKNOWN		"Unknown race"

	#define STRCMD_CS_COMMON_MAPNAME_UNKNOWN	"No name"

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
	#define STRCMD_CS_ITEMKIND_SHIELD				"Shield type"
	#define STRCMD_CS_ITEMKIND_DUMMY				"Dummy type"			
	#define STRCMD_CS_ITEMKIND_FIXER				"Fixer type"
	#define STRCMD_CS_ITEMKIND_DECOY				"Decoy type"
	#define STRCMD_CS_ITEMKIND_DEFENSE				"Armor type"
	#define STRCMD_CS_ITEMKIND_SUPPORT				"Engine type"
	#define STRCMD_CS_ITEMKIND_ENERGY				"Consumable type"
	#define STRCMD_CS_ITEMKIND_INGOT				"Mineral type"
	#define STRCMD_CS_ITEMKIND_CARD					"Standard Card type"
	#define STRCMD_CS_ITEMKIND_ENCHANT				"Enchant Card type"
	#define STRCMD_CS_ITEMKIND_TANK					"Tank type"
	#define STRCMD_CS_ITEMKIND_BULLET				"Bullet type"
	#define STRCMD_CS_ITEMKIND_QUEST				"Quest item type"
	#define STRCMD_CS_ITEMKIND_RADAR				"Radar type"
	#define STRCMD_CS_ITEMKIND_COMPUTER				"Computer type"
	#define STRCMD_CS_ITEMKIND_GAMBLE				"Gamble card type"
	#define STRCMD_CS_ITEMKIND_PREVENTION_DELETE_ITEM	"Enchant Delete Prevention Card type"
	#define STRCMD_CS_ITEMKIND_BLASTER				"Blaster type"	// 2005-08-01 by hblee : Blaster type added.
	#define STRCMD_CS_ITEMKIND_RAILGUN				"Rail gun type"		// 2005-08-01 by hblee : Rail gun type added.
	#define STRCMD_CS_ITEMKIND_ACCESSORY_UNLIMITED	"Unlimited Accessory"		// 2006-03-17 by cmkwon, 사용시간이 <영원>인 액세서리 아이템
	#define STRCMD_CS_ITEMKIND_ACCESSORY_TIMELIMIT	"Time limit Accessory"		// 2006-03-17 by cmkwon, 사용시간에 시간 제한이 있는 액세서리 아이템
	#define STRCMD_CS_ITEMKIND_ALL_WEAPON			"All weapons"
	#define STRCMD_CS_ITEMKIND_PRIMARY_WEAPON_ALL	"Standard Weapon"
	#define STRCMD_CS_ITEMKIND_PRIMARY_WEAPON_1		"Bullet type Standard Weapon"
	#define STRCMD_CS_ITEMKIND_PRIMARY_WEAPON_2		"Fuel type Standard Weapon"
	#define STRCMD_CS_ITEMKIND_SECONDARY_WEAPON_ALL	"Advanced Weapon"
	#define STRCMD_CS_ITEMKIND_SECONDARY_WEAPON_1	"Bullet type Advanced Weapon"
	#define STRCMD_CS_ITEMKIND_SECONDARY_WEAPON_2	"Shield type Advanced Weapon"
	#define STRCMD_CS_ITEMKIND_SKILL_ATTACK			"Attack skill"
	#define STRCMD_CS_ITEMKIND_SKILL_DEFENSE		"Defense skill"
	#define STRCMD_CS_ITEMKIND_SKILL_SUPPORT		"Support skill"
	#define STRCMD_CS_ITEMKIND_SKILL_ATTRIBUTE		"Attribute skill"
	#define STRCMD_CS_ITEMKIND_FOR_MON_PRIMARY		"Item for 1 type monster"
	#define STRCMD_CS_ITEMKIND_FOR_MON_GUN			"Monster machine gun type(1-1type)"
	#define STRCMD_CS_ITEMKIND_FOR_MON_BEAM			"Monster beam type(1-2type)"
	#define STRCMD_CS_ITEMKIND_FOR_MON_ALLATTACK	"Attack all monster"
	#define STRCMD_CS_ITEMKIND_FOR_MON_SECONDARY	"Item for 2 type monster"
	#define STRCMD_CS_ITEMKIND_FOR_MON_ROCKET		"Monster rocket(2-1type)"
	#define STRCMD_CS_ITEMKIND_FOR_MON_MISSILE		"Monster Missile type(2-1type)"
	#define STRCMD_CS_ITEMKIND_FOR_MON_BUNDLE		"Monster Bundle type(2-1type)"
	#define STRCMD_CS_ITEMKIND_FOR_MON_MINE			"Monster Mine type(2-1type)"
	#define STRCMD_CS_ITEMKIND_FOR_MON_SHIELD		"Monster shield type(2-2type)"
	#define STRCMD_CS_ITEMKIND_FOR_MON_DUMMY		"Monster dummy type(2-2type)"
	#define STRCMD_CS_ITEMKIND_FOR_MON_FIXER		"Monster Fixer type(2-2type)"
	#define STRCMD_CS_ITEMKIND_FOR_MON_DECOY		"Monster Decoy type(2-2type)"
	#define STRCMD_CS_ITEMKIND_FOR_MON_FIRE			"Monster Fire type"
	#define STRCMD_CS_ITEMKIND_FOR_MON_OBJBEAM		"Monster crash possible beam type"
	#define STRCMD_CS_ITEMKIND_FOR_MON_STRAIGHTBOOM	"Monster straight bomb type"
	#define STRCMD_CS_ITEMKIND_UNKNOWN				"Unknown item"

	#define STRCMD_CS_UNITKIND_UNKNOWN				"Unknown airframe"

	#define STRCMD_CS_STAT_ATTACK_PART				"Attack"
	#define STRCMD_CS_STAT_DEFENSE_PART				"Defense"
	#define STRCMD_CS_STAT_FUEL_PART				"Fuel"
	#define STRCMD_CS_STAT_SOUL_PART				"Spirit"
	#define STRCMD_CS_STAT_SHIELD_PART				"Shield"
	#define STRCMD_CS_STAT_DODGE_PART				"Agility"
	#define STRCMD_CS_STAT_BONUS					"Bonus stat"
	#define STRCMD_CS_STAT_ALL_PART					"All stat"
	#define STRCMD_CS_STAT_UNKNOWN					"Unknown stat"

	#define STRCMD_CS_AUTOSTAT_TYPE_FREESTYLE		"Free type"
	#define STRCMD_CS_AUTOSTAT_TYPE_BGEAR_ATTACK	"Attack type"
	#define STRCMD_CS_AUTOSTAT_TYPE_BGEAR_MULTI		"Multi-type"	
	#define STRCMD_CS_AUTOSTAT_TYPE_IGEAR_ATTACK	"Attack type"
	#define STRCMD_CS_AUTOSTAT_TYPE_IGEAR_DODGE		"Agility type"
	#define STRCMD_CS_AUTOSTAT_TYPE_AGEAR_ATTACK	"Attack type"
	#define STRCMD_CS_AUTOSTAT_TYPE_AGEAR_SHIELD	"Shield type"
	#define STRCMD_CS_AUTOSTAT_TYPE_MGEAR_DEFENSE	"Defense type"
	#define STRCMD_CS_AUTOSTAT_TYPE_MGEAR_SUPPORT	"Support type"
	#define STRCMD_CS_AUTOSTAT_TYPE_UNKNOWN			"UNKNOWN_AUTOSTAT_TYPE"

// 2007-10-30 by cmkwon, 세력별 해피아워 이벤트 구현 - 아래에서 다시 정의 함
//	#define STRCMD_CS_INFLUENCE_TYPE_NORMAL			"Bygeniou city general army"
//	#define STRCMD_CS_INFLUENCE_TYPE_VCN			"Bygeniou city regular army"
//	#define STRCMD_CS_INFLUENCE_TYPE_ANI			"Arlington city regular army"
	#define STRCMD_CS_INFLUENCE_TYPE_RRP			"Vattalus federation army"

	#define STRCMD_CS_POS_PROW						"Radar location(Top center)"
	#define STRCMD_CS_POS_PROWIN					"Computer(Center left)"
	#define STRCMD_CS_POS_PROWOUT					"Standard Weapon(Top left)"
	#define STRCMD_CS_POS_WINGIN					"Not being used(Center right)"
	#define STRCMD_CS_POS_WINGOUT					"Advanced Weapon(Top right)"
	#define STRCMD_CS_POS_CENTER					"Armor(Center middle)"
	#define STRCMD_CS_POS_REAR						"Engine(Bottom middle)"

	// 2010-06-15 by shcho&hslee 펫시스템
	//#define STRCMD_CS_POS_ATTACHMENT				"부착물(후미 우측-연료탱크|컨테이너계열)"
	#define STRCMD_CS_POS_ACCESSORY_UNLIMITED		"Accessory(Right side of rear-Fueltank|container type)"

	// 2010-06-15 by shcho&hslee 펫시스템
	//#define STRCMD_CS_POS_PET						"사용안함(후미 좌측)"
	#define STRCMD_CS_POS_ACCESSORY_TIME_LIMIT		"Not to use(Left side of rear)"

	#define STRCMD_CS_POS_PET						"Partner"
//_INET_PET
	#define STRCMD_CS_HIDDEN_ITEM					"Hidden Position"
//endof _INET_PET
	#define STRCMD_CS_POS_INVALID_POSITION			"Pending location"
	#define STRCMD_CS_POS_ITEMWINDOW_OFFSET			"Inventory location"

	// 2005-12-07 by cmkwon
	#define STRCMD_CS_COMMAND_QUESTCOMPLETION_0		"/QuestComplete"
	#define STRCMD_CS_COMMAND_QUESTCOMPLETION_1		"/QuestCom"
	#define STRCMD_CS_COMMAND_QUESTCOMPLETION_HELP	"kullanim: /QuestComplete [|QuestIndex]"

	// 2006-02-08 by cmkwon
	#define STRCMD_CS_COMMAND_INFLDITRIBUTION_0		"/NationRatio"
	#define STRCMD_CS_COMMAND_INFLDITRIBUTION_1		"/InflDist"
	#define STRCMD_CS_COMMAND_INFLDITRIBUTION_HELP	"kullanim: /NationRatio"
	#define STRCMD_CS_COMMAND_CHANGEINFL_0			"/ChangeNation"
	#define STRCMD_CS_COMMAND_CHANGEINFL_1			"/Nation"
	#define STRCMD_CS_COMMAND_CHANGEINFL_HELP		"kullanim: /ChangeNation [|1(Normal)|2(BCU)|4(ANI)]"

	// 2006-03-02 by cmkwon
	#define STRCMD_CS_COMMAND_GOMONSTER_0			"/GoMon"
	#define STRCMD_CS_COMMAND_GOMONSTER_1			"/GoMonster"
	#define STRCMD_CS_COMMAND_GOMONSTER_HELP		"kullanim: /GoMonster [CanavarAdi|CanavarNumarasi]"

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
	#define STRCMD_CS_COMMAND_SIGNBOARD_HELP		"kullanim: /Noticeboard [|Sure(birim:dk) [Duyuru ayrintisi] - Belirtilen sure boyunca genel elektronik duyuru panosuna duyuru ekler."
	#define STRCMD_CS_COMMAND_SIGNBOARD_DEL_0		"/DeleteNoticeboard"
	#define STRCMD_CS_COMMAND_SIGNBOARD_DEL_1		"/DeleteNoticeboard"
	#define STRCMD_CS_COMMAND_SIGNBOARD_DEL_HELP	"kullanim: /DeleteNoticeboard [silinecek duyuru indeksi] - Elektronik duyuru panosundan belirtilen duyuruyu siler."
	
	// 2006-04-20 by cmkwon
	#define STRCMD_CS_COMMON_RACE_INFLUENCE_LEADER	"Influence Leader"
	#define STRCMD_CS_COMMON_RACE_INFLUENCE_SUBLEADER	"Influence Subleader"
	// 2006-04-21 by cmkwon
	#define STRCMD_CS_ITEMKIND_INFLUENCE_BUFF		"Influence Buff"
	#define STRCMD_CS_ITEMKIND_INFLUENCE_GAMEEVENT	"Influence Event"

	// 2006-04-24 by cmkwon
	#define STRCMD_CS_COMMAND_CONPOINT_0			"/ContributionPoint"
	#define STRCMD_CS_COMMAND_CONPOINT_1			"/ContributionPoint"
	#define STRCMD_CS_COMMAND_CONPOINT_HELP			"kullanim: /ContributionPoint [Ulus(2:BCU, 4:ANI)] [Artis] - Belirtilen ulusun katki puanini artirir"

	// 2006-05-08 by cmkwon
	#define STRCMD_CS_COMMAND_CALLGM_0				"/CallGM"
	#define STRCMD_CS_COMMAND_CALLGM_1				"/CallGM"  // Helper
	#define STRCMD_CS_COMMAND_CALLGM_2				"/CallGM"  // Help
	#define STRCMD_CS_COMMAND_CALLGM_HELP			"kullanim: /CallGM [Talep ayrintisi] - GM ile destek talebi olusturur. - Devre disi"
	#define STRCMD_CS_COMMAND_VIEWCALLGM_0			"/ViewCallGM"
	#define STRCMD_CS_COMMAND_VIEWCALLGM_1			"/ViewCallGM"   // See helper
	#define STRCMD_CS_COMMAND_VIEWCALLGM_2			"/ViewCallGM"   // See help
	#define STRCMD_CS_COMMAND_VIEWCALLGM_HELP		"kullanim: /ViewCallGM [|Numara(1~10)] - GM destek talebi listesini kontrol eder - Devre disi"
	#define STRCMD_CS_COMMAND_BRINGCALLGM_0			"/BringCallGM"
	#define STRCMD_CS_COMMAND_BRINGCALLGM_1			"/BringCallGM"   // Bring helper
	#define STRCMD_CS_COMMAND_BRINGCALLGM_2			"/BringCallGM"   // Bring help
	#define STRCMD_CS_COMMAND_BRINGCALLGM_HELP		"kullanim: /BringCallGM [|Numara(1~10)] - Belirtilen destek talebini GM'e getirir (sunucudan silinir)"

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
	#define STRCMD_CS_COMMAND_ITEMINMAP_HELP		"kullanim: /SendItem [1(Normal)|2(BCU)|4(ANI)|3|5|6|7] [Esya Numarasi] [esya sayisi] - Mevcut haritadaki secili ulusun tum kullanicilarina esya verir."

	// 2006-07-28 by cmkwon
	#define STRCMD_CS_ITEMKIND_COLOR_ITEM			"Color Item"

	// 2006-08-03 by cmkwon, 나라별 날짜 표현 방식이 다르다
	// 한국(Korea):		YYYY-MM-DD HH:MM:SS
	// 미국(English):	MM-DD-YYYY HH:MM:SS
	// 베트남(Vietnam):	DD-MM-YYYY HH:MM:SS
	#define NATIONAL_ATUM_DATE_TIME_STRING_FORMAT(Y, M, D, h, m, s)				"%02d-%02d-%04d %02d:%02d:%02d", M, D, Y, h, m, s
	#define NATIONAL_ATUM_DATE_TIME_STRING_FORMAT_EXCLUDE_SECOND(Y, M, D, h, m)	"%02d-%02d-%04d %02d:%02d", M, D, Y, h, m

	// 2006-08-08 by dhjin, 레벨분포
	#define STRCMD_CS_COMMAND_DISTRIBUTIONLEVEL_0		"/LevelDistribution"		// 2006-08-08 by dhjin
	#define STRCMD_CS_COMMAND_DISTRIBUTIONLEVEL_1		"/LevelDist"				// 2006-08-08 by dhjin
	#define STRCMD_CS_COMMAND_DISTRIBUTIONLEVEL_HELP	"kullanim: /LevelDistribution - Cevrimici kullanicilarin seviye dagilimini gosterir."	// 2006-08-08 by dhjin

	// 2006-08-10 by cmkwon
	#define STRCMD_CS_ITEMKIND_RANDOMBOX				"Chance Item"

	// 2006-08-21 by cmkwon
	#define STRCMD_CS_ITEMKIND_MARK						"Mark"

	///////////////////////////////////////////////////////////////////////////////
	// 2006-08-24 by cmkwon
	// 클라이언트에서만 사용하는 명령어(Just command for client)
	#define STRCMD_C_COMMAND_CALL						"/call"
	#define STRCMD_C_COMMAND_CALL_HELP					"kullanim: /call [KarakterAdi] - Belirtilen karaktere 1:1 sesli sohbet istegi gonderir."
	#define STRCMD_C_COMMAND_PARTYCALL					"/formcall"
	#define STRCMD_C_COMMAND_PARTYCALL_HELP				"kullanim: /formcall - Formasyon uyeleri arasinda sesli sohbet baslatir. Yalnizca formasyon lideri kullanabilir."
	#define STRCMD_C_COMMAND_PARTYCALLEND				"/formcallend"
	#define STRCMD_C_COMMAND_PARTYCALLEND_HELP			"kullanim: /formcallend - Formasyon uyeleri arasindaki sesli sohbeti bitirir. Yalnizca formasyon lideri kullanabilir."
	#define STRCMD_C_COMMAND_GUILDCALL					"/brigcall"
	#define STRCMD_C_COMMAND_GUILDCALL_HELP				"kullanim: /brigcall - Tugay uyeleri arasinda sesli sohbet baslatir. Yalnizca tugay lideri kullanabilir."
	#define STRCMD_C_COMMAND_GUILDCALLEND				"/brigcallend"
	#define STRCMD_C_COMMAND_GUILDCALLEND_HELP			"kullanim: /brigcallend - Tugay uyeleri arasindaki sesli sohbeti bitirir. Yalnizca tugay lideri kullanabilir."
	#define STRCMD_C_COMMAND_CALLEND					"/endcall"
	#define STRCMD_C_COMMAND_CALLEND_HELP				"kullanim: /endcall - Tugay, formasyon veya normal sesli sohbeti bitirir."
	#define STRCMD_C_COMMAND_COMBAT						"/confront"
	#define STRCMD_C_COMMAND_BATTLE						"/fight"
	#define STRCMD_C_COMMAND_BATTLE_HELP				"kullanim: /fight [KarakterAdi] - Belirtilen karaktere PvP istegi gonderir."
	#define STRCMD_C_COMMAND_SURRENDER					"/surrender"
	#define STRCMD_C_COMMAND_SURRENDER_HELP				"kullanim: /surrender [KarakterAdi] - Belirtilen karakterle PvP savasinda teslim olur."
	#define STRCMD_C_COMMAND_PARTYBATTLE				"/formfight"
	#define STRCMD_C_COMMAND_PARTYBATTLE_HELP			"kullanim: /formfight [KarakterAdi] - Belirtilen formasyon liderine formasyon PvP istegi gonderir. Yalnizca formasyon lideri kullanabilir."
	#define STRCMD_C_COMMAND_PARTYCOMBAT				"/formconfront"
	#define STRCMD_C_COMMAND_PARTYWAR					"/formbattle"
	#define STRCMD_C_COMMAND_GUILDBATTLE				"/brigfight"
	#define STRCMD_C_COMMAND_GUILDCOMBAT				"/brigconfront"
	#define STRCMD_C_COMMAND_GUILDCOMBAT_HELP			"kullanim: /brigconfront [KarakterAdi] - Belirtilen tugay liderine tugay PvP istegi gonderir. Yalnizca tugay lideri kullanabilir."
	#define STRCMD_C_COMMAND_GUILDWAR					"/brigbattle"
	#define STRCMD_C_COMMAND_GUILDSURRENDER				"/brigsurrender"
	#define STRCMD_C_COMMAND_GUILDSURRENDER_HELP		"kullanim: /brigsurrender - Tugay PvP savasinda teslim olur. Yalnizca tugay lideri kullanabilir."
	#define STRCMD_C_COMMAND_NAME						"/name"
	#define STRCMD_C_COMMAND_NAME_HELP					"kullanim: /name [KarakterAdi] [sinif(2 ~ 11)] - Belirtilen karakterin gorev/rutbe sinifini degistirir. Yalnizca tugay lideri kullanabilir."
	#define STRCMD_C_COMMAND_WARP						"/warp"
	#define STRCMD_C_COMMAND_CANCELSKILL				"/cancelskill"
	#define STRCMD_C_COMMAND_INITCHAT					"/initchat"
	#define STRCMD_C_COMMAND_INITCHAT_HELP				"kullanim: /initchat - Sohbet penceresini sifirlar"
	#define STRCMD_C_COMMAND_REFUSEBATTLE				"/refusefight"
	#define STRCMD_C_COMMAND_REFUSEBATTLE_HELP			"kullanim: /refusefight - On/Off - PvP reddetme ayarini degistirir"
	#define STRCMD_C_COMMAND_REFUSETRADE				"/refusetrade"
	#define STRCMD_C_COMMAND_REFUSETRADE_HELP			"kullanim: /refusetrade - On/Off - Ticaret reddetme ayarini degistirir"
	#define STRMSG_C_050810_0001						"/CloseWindow"
	#define STRMSG_C_050810_0001_HELP					"kullanim: /Closewindow - Mesaj pencerelerini engeller. Acilir mesajlar otomatik olarak iptal edilir."
	#define STRMSG_C_050810_0002						"/OpenWindow"
	#define STRMSG_C_050810_0002_HELP					"kullanim: /Openwindow - Mesaj pencerelerine izin verir."

// 2006-09-29 by cmkwon
#define STRCMD_CS_ITEMKIND_SKILL_SUPPORT_ITEM			"Support Skill Item"

// 2010-06-15 by shcho&hslee 펫시스템 - 펫 아이템.
#define STRCMD_CS_ITEMKIND_PET_ITEM						"Partner Item"

// 2006-11-17 by cmkwon, 베트남 하루 게임 시간 관련
#define STRCMD_CS_COMMAND_TIMELIMITSYSTEM_0			"/TimeLimitSystem"
#define STRCMD_CS_COMMAND_TIMELIMITSYSTEM_1			"/TimeLimitSystem"
#define STRCMD_CS_COMMAND_TIMELIMITSYSTEM_P2ON		"on"
#define STRCMD_CS_COMMAND_TIMELIMITSYSTEM_P2OFF		"off"
#define STRCMD_CS_COMMAND_TIMELIMITSYSTEM_HELP		"kullanim: /TimeLimitSystem [on|off] - Zaman siniri sistemini acip kapatir."
#define STRCMD_CS_COMMAND_PLAYTIME_0				"/PlayTime"
#define STRCMD_CS_COMMAND_PLAYTIME_1				"/PlayTime"
#define STRCMD_CS_COMMAND_PLAYTIME_HELP				"kullanim: /PlayTime - Bugunku oynama suresini gosterir."

// 2007-10-06 by cmkwon, 부지도자 2명의 호칭을 다르게 설정 - 아래에 세력별로 다르게 정의함
//// 2006-12-13 by cmkwon
//#define STRCMD_CS_COMMON_INFLUENCE_LEADER			"Leader"
//#define STRCMD_CS_COMMON_INFLUENCE_SUBLEADER		"Subleader"

// 2007-01-08 by dhjin
#define STRCMD_CS_COMMAND_BONUSSTAT_POINT_0			"/BonusStatPoint"
#define STRCMD_CS_COMMAND_BONUSSTAT_POINT_1			"/BonusStatPoint"
#define STRCMD_CS_COMMAND_BONUSSTAT_POINT_2			"/BonusStatPoint"
#define STRCMD_CS_COMMAND_BONUSSTAT_POINT_HELP		"kullanim: /BonusStatPoint [BonusStatPoint Sayisi] [|karakter adi] - BonusStatPoint degerini DB'de gunceller"

// 2007-01-25 by dhjin
#define STRCMD_CS_COMMAND_PCBANGUSERCOUNT_0			"/PCBang"
#define STRCMD_CS_COMMAND_PCBANGUSERCOUNT_1			"/PCBang"
#define STRCMD_CS_COMMAND_PCBANGUSERCOUNT_HELP		"kullanim: /PCBang - PCBang kullanici sayisini gosterir"

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
#define STRCMD_CS_COMMAND_STRATEGYPOINTINFO_HELP		"kullanim: /StrategyPointInfo - Mevcut Stratejik Nokta ilerleme durumunu gosterir."
#define STRCMD_CS_COMMAND_STRATEGYPOINTINFO_EMPTY		"There is no strategypoint war in progress."
#define STRCMD_CS_COMMAND_STRATEGYPOINTINFO_EXIST		"Strategypoint war is in progress."
#define STRCMD_CS_COMMAND_STRATEGYPOINTINFO_ZONE		"Progress location"
#define STRCMD_CS_COMMAND_STRATEGYPOINTINFO_STARTTIME		"Starting time"

// 2007-03-29 by cmkwon
#define STRCMD_CS_UNITKIND_BGEAR					"B-GEAR"
#define STRCMD_CS_UNITKIND_MGEAR					"M-GEAR"
#define STRCMD_CS_UNITKIND_AGEAR					"A-GEAR"
#define STRCMD_CS_UNITKIND_IGEAR					"I-GEAR"
#define STRCMD_CS_UNITKIND_BGEAR_ALL				"B-GEAR All"
#define STRCMD_CS_UNITKIND_MGEAR_ALL				"M-GEAR All"
#define STRCMD_CS_UNITKIND_AGEAR_ALL				"A-GEAR All"
#define STRCMD_CS_UNITKIND_IGEAR_ALL				"I-GEAR All"
#define STRCMD_CS_UNITKIND_GEAR_ALL					"GEAR All"

// 2007-03-30 by dhjin, 옵저버 모드 유저 등록
#define STRCMD_CS_COMMAND_OBSERVER_REG_START_0  		"/Observerstart"  // 2007-03-30 by dhjin, Client only
#define STRCMD_CS_COMMAND_OBSERVER_REG_START_1  		"/Observerstart"   // 2007-03-30 by dhjin, Client only
#define STRCMD_CS_COMMAND_OBSERVER_REG_END_0  			"/Observerend"   // 2007-03-30 by dhjin, Client only 
#define STRCMD_CS_COMMAND_OBSERVER_REG_END_1  			"/Observerend"   // 2007-03-30 by dhjin, Client only
#define STRCMD_CS_COMMAND_OBSERVER_REG_0   			"/Observer"
#define STRCMD_CS_COMMAND_OBSERVER_REG_1   			"/Observer"
#define STRCMD_CS_COMMAND_OBSERVER_REG_HELP   			"kullanim: /Observer [n] [KarakterAdi] - Karakteri n numarali gozlemci listesine kaydeder"

// 2007-04-10 by cmkwon, Jamboree server 군 관련
#define STRCMD_CS_COMMAND_JAMBOREE_INIT_0   			"/InitJamboree"   
#define STRCMD_CS_COMMAND_JAMBOREE_INIT_1   			"/InitJamboree"  
#define STRCMD_CS_COMMAND_JAMBOREE_INIT_HELP  			"kullanim: /InitJamboree [dogrulama numarasi] - Jamboree sunucu DB'sini (atum2_db_20) baslatir."
#define STRCMD_CS_COMMAND_JAMBOREE_ENTRANTS_0  			"/EntrantJamboree"
#define STRCMD_CS_COMMAND_JAMBOREE_ENTRANTS_1  			"/EntrantJamboree"  
#define STRCMD_CS_COMMAND_JAMBOREE_ENTRANTS_HELP 		"kullanim: /EntrantJamboree [KarakterAdi] - Belirtilen karakteri jamboree sunucu DB'sine (atum2_db_20) kopyalar."
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
#define STRMSG_CS_STRING_ARENA_NOT_SEARCH   "Arena takimi bulunamadi."
#define STRMSG_CS_COMMAND_ARENA_TEAM_INFO_0   "/ARENA"
#define STRMSG_CS_COMMAND_ARENA_TEAM_INFO_1   "/ARENA"
#define STRMSG_CS_COMMAND_ARENA_TEAM_INFO_HELP  "kullanim: /arena [2(BCU)|4(ANI)] - Arena'nin mevcut ilerleme durumunu gosterir."

// 2010. 06. 04 by hsLee ARENA 인피니티 관련. - 
// 2010. 06. 04 by hsLee 인티피니 필드 2차 난이도 조절. (GM 명령어 추가. /nextscene(다음 시네마 씬 호출.) )
#define STRCMD_CS_COMMAND_INFINITY_NEXT_SCENE		"/nextscene"
// End 2010. 06. 04 by hsLee 인티피니 필드 2차 난이도 조절. (GM 명령어 추가. /nextscene(다음 시네마 씬 호출.) )

// 2007-06-15 by dhjin, 관전
#define STRMSG_CS_COMMAND_WATCH_START_INFO_0		"/WatchStart"
#define STRMSG_CS_COMMAND_WATCH_START_INFO_1		"/WatchStart"
#define STRMSG_CS_COMMAND_WATCH_START_INFO_HELP	"kullanim: /WatchStart - Izlemeyi baslatir."
#define STRMSG_CS_COMMAND_WATCH_END_INFO_0			"/WatchEnd"
#define STRMSG_CS_COMMAND_WATCH_END_INFO_1			"/WatchEnd"
#define STRMSG_CS_COMMAND_WATCH_END_INFO_HELP		"kullanim: /WatchEnd - Izlemeyi bitirir."

// 2007-06-22 by dhjin, WarPoint 추가
#define STRMSG_CS_COMMAND_WARPOINT_0    "/WarPoint"
#define STRMSG_CS_COMMAND_WARPOINT_1    "/WarPoint"
#define STRMSG_CS_COMMAND_WARPOINT_HELP    "kullanim: /WarPoint [Sayi 1~1000000] [|KullaniciAdi] - Savas puani ekler."

// 2007-06-26 by dhjin, 워포인트 이벤트 관련 추가
#define STRCMD_CS_COMMAND_GAMEEVENT_P1WARPOINT		"WarPoint"

// 2007-07-11 by cmkwon, Arena block system materialization - Add command(/forbidAreana, /releaseArena)
#define STRCMD_CS_COMMAND_ARENAFORBID_0    "/forbidArena "
#define STRCMD_CS_COMMAND_ARENAFORBID_1    "/forbidArena"
#define STRCMD_CS_COMMAND_ARENAFORBID_2    "/forbidArena"
#define STRCMD_CS_COMMAND_ARENAFORBID_HELP   "kullanim: /forbidArena [karakter adi] [|Sure(dakika)] - Arena'ya girisi yasaklar"
#define STRCMD_CS_COMMAND_ARENAFORBIDRelease_0  "/releaseArena "
#define STRCMD_CS_COMMAND_ARENAFORBIDRelease_1  "/releaseArena"
#define STRCMD_CS_COMMAND_ARENAFORBIDRelease_2  "/releaseArena"
#define STRCMD_CS_COMMAND_ARENAFORBIDRelease_HELP "kullanim: /releaseArena [karakter adi] - Arena yasagini kaldirir"

///////////////////////////////////////////////////////////////////////////////
// 2007-08-02 by cmkwon, Brigade mark screening system materialization - added string
#define STRMSG_070802_0001    "Tugay amblemi basariyla kaydedildi."
#define STRMSG_070802_0002    "Kayit inceleme islemi tamamlandiktan sonra etkinlesecek."
#define STRMSG_070802_0003    "Secilen %d numarali tugay amblemini kabul ediyor musunuz?"
#define STRMSG_070802_0004    "Tugay amblemi durumu yok"
#define STRMSG_070802_0005    "Tugay amblemi bekleme durumunda"
#define STRMSG_070802_0006    "Tugay amblemi normal durumda"
#define STRMSG_070802_0007    "Tugay amblemi hata durumunda"

// 2007-08-24 by cmkwon, 스피커아이템 사용 가능/금지 설정 기능 추가 - 명령어 추가
#define STRCMD_CS_COMMAND_UsableSpeakerItem_0			"/UseSpeaker"
#define STRCMD_CS_COMMAND_UsableSpeakerItem_1			"/UseSpeaker"
#define STRCMD_CS_COMMAND_UsableSpeakerItem_2			"/UseSpeaker"
#define STRCMD_CS_COMMAND_UsableSpeakerItem_P1Able		"Enable"
#define STRCMD_CS_COMMAND_UsableSpeakerItem_P1Forbid	"Forbid"
#define STRCMD_CS_COMMAND_UsableSpeakerItem_HELP		"kullanim: /[UseSpeaker|UseSpeaker|UseSpeaker] [Enable|Forbid] - Hoparlor esyasinin kullanimina izin verir/yasaklar"

// 2007-08-27 by cmkwon, PrepareShutdown command(GM can shutdown game server in SCAdminTool)
#define STRCMD_CS_COMMAND_PrepareShutdown_0				"/PrepareShutdown"
#define STRCMD_CS_COMMAND_PrepareShutdown_1				"/PrepareShutdown"
#define STRCMD_CS_COMMAND_PrepareShutdown_2				"/PrepareShutdown"
#define STRCMD_CS_COMMAND_PrepareShutdown_P1Start		"Start"
#define STRCMD_CS_COMMAND_PrepareShutdown_P1Release		"Release"
#define STRCMD_CS_COMMAND_PrepareShutdown_HELP			"kullanim: /[PrepareShutdown|PrepareShutdown|PrepareShutdown] [Start|Release] - Sunucu kapanisini hazirlar, tum kullanicilarin baglantisini keser."

// 2007-08-30 by cmkwon, 회의룸 시스템 구현 - 명령어 추가
#define STRCMD_CS_COMMAND_EntrancePermission_0                                     "/EntrancePermission"
#define STRCMD_CS_COMMAND_EntrancePermission_1                                     "/EntrancePermission"
#define STRCMD_CS_COMMAND_EntrancePermission_2                                     "/EntrancePermission"
#define STRCMD_CS_COMMAND_EntrancePermission_HELP                      "kullanim: /[EntrancePermission] [|CharacterName] - Yalnizca lider kullanabilir. Ilgili karakteri konferans odasi giris listesine ekler."
#define STRCMD_CS_COMMAND_EntrancePermissionDeny_0                               "/EntrancePermissionDeny"
#define STRCMD_CS_COMMAND_EntrancePermissionDeny_1                               "/EntrancePermissionDeny"
#define STRCMD_CS_COMMAND_EntrancePermissionDeny_2                               "/EntrancePermissionDeny"
#define STRCMD_CS_COMMAND_EntrancePermissionDeny_HELP                "kullanim: /[EntrancePermissionDeny] [CharacterName] - Yalnizca lider kullanabilir. Ilgili kullaniciyi konferans odasi giris listesinden siler."

// 2007-10-05 by cmkwon, different each nations.
#define STRCMD_071005_0000					"%d\\rdays\\r %dhour(s) %dminute(s) %dsecond(s)", Day, Hour, Minute, Second  // Day, Hour, Minute, Second is parameter name. No need to translate.
#define STRCMD_071005_0001					"%dYear %dmonth %dday", Year, Month, Day //Year, Month, Day is parameter name. No need to translate.
#define STRCMD_071005_0002					"%dYear %dmonth", Year, Month // Year, Month is parameter name. No need to translate.
#define STRCMD_071005_0003					"%dMonth %dday", Month, Day // Month, Day is parameter name. No need to translate.


// 2007-10-06 by cmkwon, Set name 2 sub-leader each nations
#define STRCMD_CS_COMMON_RACE_INFLUENCE_SUBLEADER_1		"Sub leader 1" // this is common both nations.
#define STRCMD_CS_COMMON_RACE_INFLUENCE_SUBLEADER_2		"Sub leader 2" // this is common both nations.
#define STRCMD_VCN_INFLUENCE_LEADER						"General Commander"
#define STRCMD_VCN_INFLUENCE_SUBLEADER_1				"Deputy Commander"
#define STRCMD_VCN_INFLUENCE_SUBLEADER_2				"Chief of Staff"
#define STRCMD_ANI_INFLUENCE_LEADER						"Chairman"
#define STRCMD_ANI_INFLUENCE_SUBLEADER_1				"Vice-Chairman"
#define STRCMD_ANI_INFLUENCE_SUBLEADER_2				"Strategy Director"
#define STRCMD_OUTPOST_GUILD_MASTER						"Cpt. %s"

// 2007-10-06 by dhjin, command to set 2 sub-leader
#define STRCMD_CS_COMMAND_SUBLEADER1_0				"/appointment1"
#define STRCMD_CS_COMMAND_SUBLEADER1_1				"/appointment1"
#define STRCMD_CS_COMMAND_SUBLEADER1_HELP			"kullanim: /appointment1 [CharacterName] - BCU: 1. ulus lider yardimcisini, ANI: 1. ulus komutan yardimcisini ayarlar"
#define STRCMD_CS_COMMAND_SUBLEADER2_0				"/appointment2"
#define STRCMD_CS_COMMAND_SUBLEADER2_1				"/appointment2"
#define STRCMD_CS_COMMAND_SUBLEADER2_HELP			"kullanim: /appointment2 [CharacterName] - BCU: 2. ulus lider yardimcisini, ANI: 2. ulus komutan yardimcisini ayarlar"

// 2007-10-30 by cmkwon, each nation happy hour event system - Command system is changed.
#define STRCMD_CS_COMMAND_HAPPYHOUREVENT				"/HappyEvent"
#define STRCMD_CS_COMMAND_HAPPYHOUREVENT_1			"/HappyEvent"
#define STRCMD_CS_COMMAND_HAPPYHOUREVENT_PSTART	"Start"
#define STRCMD_CS_COMMAND_HAPPYHOUREVENT_PEND		"End"
#define STRCMD_CS_COMMAND_HAPPYHOUREVENT_HELP		"kullanim: /HappyEvent [255(AllNation)|0(NormalNation)|2(BCU)|4(ANI)] [Start|End] [Sure(Birim:Dakika)]"
 
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
#define STRCMD_CS_COMMAND_WAPPOINTINMAP_HELP		"kullanim: /WPAddedMap [1(Normal)|2(BCU)|4(ANI)|3|5|6|7] [AddWarPoint(1~)] - Mevcut haritadaki secili ulus kullanicilarina Savas Puani verir."

// 2007-11-19 by cmkwon, CallGM system - new command
#define STRCMD_CS_COMMAND_STARTCALLGM_0			"/StartCallGM"
#define STRCMD_CS_COMMAND_STARTCALLGM_1			"/StartHelper"
#define STRCMD_CS_COMMAND_STARTCALLGM_2			"/StartHelp"
#define STRCMD_CS_COMMAND_STARTCALLGM_HELP		"kullanim: /StartHelper [|sure(Birim: Dakika)] - Destek sistemini baslatir"
#define STRCMD_CS_COMMAND_ENDCALLGM_0				"/EndCallGM"
#define STRCMD_CS_COMMAND_ENDCALLGM_1				"/EndHelper"
#define STRCMD_CS_COMMAND_ENDCALLGM_2				"/EndHelp"
#define STRCMD_CS_COMMAND_ENDCALLGM_HELP			"kullanim: /EndHelper - Destek sistemini bitirir"

// 2007-12-27 by cmkwon, 윈도우즈 모드 기능 추가 - STRMSG_REG_KEY_NAME_WINDOWDEGREE_NEW 추가
#define STRMSG_REG_KEY_NAME_WINDOWDEGREE_NEW		"WindowDegreeNew"

// 2008-01-03 by cmkwon, 윈도우모드 상태 저장하기 - 
#define STRMSG_REG_KEY_NAME_WINDOWMODE				"WindowMode"
#define STRMSG_REG_KEY_NAME_64BIT				"Is64Bit"

// 2008-01-31 by cmkwon, 계정 블럭/해제 명령어로 가능한 시스템 구현 - 명령어 추가
#define STRCMD_CS_COMMAND_NEWACCOUNTBLOCK_0					"/Block"
#define STRCMD_CS_COMMAND_NEWACCOUNTBLOCK_1					"/Block"
#define STRCMD_CS_COMMAND_NEWACCOUNTBLOCK_2					"/Ban"
#define STRCMD_CS_COMMAND_NEWACCOUNTBLOCK_HELP				"kullanim: /Block [AccountName] [BlockType(1:Normal|2:Related Money|3:Related Item|4:Related SpeedHack|5:Related Chatting|6:Related GameBug)] [Sure:Gun] [Kullanici Engelleme Nedeni] / [Yalnizca Yonetici Nedeni]"
#define STRCMD_CS_COMMAND_NEWACCOUNTUNBLOCK_0				"/Unblock"
#define STRCMD_CS_COMMAND_NEWACCOUNTUNBLOCK_1				"/Unblock"
#define STRCMD_CS_COMMAND_NEWACCOUNTUNBLOCK_2				"/Unban"
#define STRCMD_CS_COMMAND_NEWACCOUNTUNBLOCK_HELP			"kullanim: /Unblock [AccountName]"

// 2008-02-20 by cmkwon, 명령어추가(접속중인유저모두에게아이템지급- 
#define STRCMD_CS_COMMAND_ITEMALLUSER_0                                    "/ItemAllUser"
#define STRCMD_CS_COMMAND_ITEMALLUSER_1                                    "/ItemAllUser"
#define STRCMD_CS_COMMAND_ITEMALLUSER_2                                    "/ItemAllUser"
#define STRCMD_CS_COMMAND_ITEMALLUSER_HELP                               "kullanim: /ItemAllUser [1(Normal)|2(BCU)|4(ANI)|255(All)] [Esya Numarasi] [esya sayisi] - Secili ulusun oyundaki kullanicilarina belirtilen esyayi verir"

// 2008-02-21 by dhjin, 아레나통합- 아레나추가명령어
#define STRCMD_CS_COMMAND_ARENAMOVE_0                                                         "/ArenaMove"
#define STRCMD_CS_COMMAND_ARENAMOVE_1                                                         "/ArenaMove"
#define STRCMD_CS_COMMAND_TEAMARENALEAVE_0                                                  "/TeamArenaLeave"
#define STRCMD_CS_COMMAND_TEAMARENALEAVE_1                                                  "/TeamServerReturn"
#define STRCMD_CS_COMMAND_TEAMARENALEAVE_HELP                                   "kullanim: /TeamArenaLeave [2(BLUE)|4(RED)|6(BLUE AND RED)]"
#define STRCMD_CS_COMMAND_TARGETARENALEAVE_0                                     "/TargetArenaLeave"
#define STRCMD_CS_COMMAND_TARGETARENALEAVE_1                                     "/TargetArenaLeave"
#define STRCMD_CS_COMMAND_TARGETARENALEAVE_HELP                                 "kullanim: /TargetArenaLeave [Charactername]"
#define STRCMD_CS_COMMAND_ARENAEVENT_0                                                         "/ArenaEvent"
#define STRCMD_CS_COMMAND_ARENAEVENT_1                                                         "/ArenaEvent"
#define STRCMD_CS_COMMAND_ARENAEVENT_2                                                         "/ArenaEvent"
#define STRCMD_CS_COMMAND_ARENAEVENT_HELP                                                    "kullanim: /ArenaEvent [RoomNumber]"
#define STRCMD_CS_COMMAND_ARENAEVENTRELEASE_0                                    "/ArenaEventRelease"
#define STRCMD_CS_COMMAND_ARENAEVENTRELEASE_1                                    "/CancelArenaEvent"
#define STRCMD_CS_COMMAND_ARENAEVENTRELEASE_2                                    "/CancelArenaEvent"
#define STRCMD_CS_COMMAND_ARENAEVENTRELEASE_HELP                                "kullanim: /ArenaEventRelease [RoomNumber]"

// 2008-06-03 by cmkwon, AdminTool, DBTool 사용시 아이템 검색시 콤보박스에서 검색 기능 추가(K0000143) - 
#define STRCMD_CS_ITEMKIND_ALL_ITEM							"All Kind"

//////////////////////////////////////////////////////////////////////////
// 2008-05-20 by dhjin, EP3 - 여단 수정 사항	// 2006-03-07 by cmkwon
#define STRCMD_CS_GUILD_RANK_PRIVATE_NULL		"Member"
#define STRCMD_CS_GUILD_RANK_COMMANDER			"Flight Brigade Commander"
#define STRCMD_CS_GUILD_RANK_SUBCOMMANDER		"Deputy Brigade Commander"				// 2008-05-20 by dhjin, EP3 - 여단 수정 사항
#define STRCMD_CS_GUILD_RANK_SQUAD_LEADER_1		"1st Battalion Commander"
#define STRCMD_CS_GUILD_RANK_PRIVATE_1			"1st Battalion Member"
#define STRCMD_CS_GUILD_RANK_SQUAD_LEADER_2		"2nd Battalion Commander"
#define STRCMD_CS_GUILD_RANK_PRIVATE_2			"2nd Battalion Member"
#define STRCMD_CS_GUILD_RANK_SQUAD_LEADER_3		"3rd Battalion Commander"
#define STRCMD_CS_GUILD_RANK_PRIVATE_3			"3rd Battalion Member"
#define STRCMD_CS_GUILD_RANK_SQUAD_LEADER_4		"4th Battalion Commander"
#define STRCMD_CS_GUILD_RANK_PRIVATE_4			"4th Battalion Member"
#define STRCMD_CS_GUILD_RANK_SQUAD_LEADER_5		"5th Battalion Commander"
#define STRCMD_CS_GUILD_RANK_PRIVATE_5			"5th Battalion Member"

//////////////////////////////////////////////////////////////////////////
// 2008-06-19 by dhjin, EP3 - 전장정보
#define STRCMD_COMMAND_WAR_OPTION_0					"/MotherShipInfoOption"
#define STRCMD_COMMAND_WAR_OPTION_1					"/MotherShipInfoOption"

// 2008-08-18 by dhjin, 세력마크이벤트 
#define STRCMD_CS_COMMAND_INFLUENCEMARKEVENT_0				"/influencemarkevent"
#define STRCMD_CS_COMMAND_INFLUENCEMARKEVENT_1				"/influencemarkevent"
#define STRCMD_CS_COMMAND_INFLUENCEMARKEVENT_2				"/influencemarkevent"
#define STRCMD_CS_COMMAND_INFLUENCEMARKEVENT_HELP			"kullanim: /influencemarkevent [ulus 2(BCU)|4(ANI)]"
#define STRCMD_CS_COMMAND_INFLUENCEMARKEVENTEND_0			"/influencemarkeventend"
#define STRCMD_CS_COMMAND_INFLUENCEMARKEVENTEND_1			"/influencemarkeventend"
#define STRCMD_CS_COMMAND_INFLUENCEMARKEVENTEND_2			"/influencemarkeventend"
#define STRCMD_CS_COMMAND_INFLUENCEMARKEVENTEND_HELP		"kullanim: /influencemarkeventend"

//////////////////////////////////////////////////////////////////////////
// 2008-08-25 by dhjin, 태국 PC방 IP정보 로딩
#define STRCMD_CS_COMMAND_PCBANGRELOADTIME_0				"/PCBangReloadTime"
#define STRCMD_CS_COMMAND_PCBANGRELOADTIME_1				"/PCBangReloadTime"
#define STRCMD_CS_COMMAND_PCBANGRELOADTIME_HELP				"kullanim: /PCBangreloadtime [Dakika] - 10 Dk ~ 1440 Dk"


// 2008-08-21 by dhjin, 일반, 특수 계정의 부지도자 임명 제한
#define STRMSG_080821_0001				"Secili karakter bu rutbeye atanamaz."


// 2008-09-09 by cmkwon, /세력소환 명령어 인자 리스트에 기어타입 추가 - 
#define STRCMD_CS_COMMAND_COMEONINFL_HELP2		"format: /ComeOnInfl [1(Normal)|2(BCU)|4(ANI)|255(All)] [maximum people] [0|minimum level] [0|maximum level] [1(B)|16(M)|256(A)|4096(I)|4369(ALL)] [Message to user] - Request to certain nation, level users to move to your position."

// 2008-09-09 by cmkwon, "/kick" 명령어 추가 - 
#define STRCMD_CS_COMMAND_KICK_0							"/Kick"
#define STRCMD_CS_COMMAND_KICK_1							"/Kick"
#define STRCMD_CS_COMMAND_KICK_HELP							"kullanim: /Kick [CharacterName] - Belirtilen karakteri oyundan atar."


// 2008-09-12 by cmkwon, "/명성" 명령어 추가 - 
#define STRCMD_CS_COMMAND_ADD_FAME_0							"/Fame"
#define STRCMD_CS_COMMAND_ADD_FAME_1							"/Fame"
#define STRCMD_CS_COMMAND_ADD_FAME_HELP							"kullanim: /fame [kisisel sohret] [tugay sohreti] - Karakterin kisisel ve tugay sohretini artirir."

// 2008-12-30 by cmkwon, 지도자 채팅 제한 카드 구현 - 
#define STRCMD_CS_COMMAND_CHATFORBIDRELEASE_LEADER_0			"/ReleaseLeaderChatBlock"
#define STRCMD_CS_COMMAND_CHATFORBIDRELEASE_LEADER_1			"/ReleaseLeaderChatBlock"
#define STRCMD_CS_COMMAND_CHATFORBIDRELEASE_LEADER_HELP			"kullanim: /ReleaseLeaderChatBlock [CharacterName] - Lider sohbet kisitlamasini kaldirir."

// 2009-10-12 by cmkwon, 프리스카 제거 방안 적용 - 
#define STRCMD_CS_COMMAND_CHANGE_StartCityMapIndex_0                               "/StartCityMap"
#define STRCMD_CS_COMMAND_CHANGE_StartCityMapIndex_1                               "/StartCity"
#define STRCMD_CS_COMMAND_CHANGE_StartCityMapIndex_HELP                           "kullanim: /StartCity [2001|2002] [|CharacterName] - Ulus baslatilirken karakterin baslangic sehrini 2001 veya 2002 olarak ayarlar."


///////////////////////////////////////////////////////////////////////////////
// 2010-01-08 by cmkwon, 최대 레벨 상향에 따른 추가 사항(레벨별 계급) - 계급뜻(백부장, 대령, 장군, 총독, 정복자, 수호자, 전설적인)
#define       STRCMD_CS_CHARACTER_96_LEVEL_RANK             "Centurion"
#define       STRCMD_CS_CHARACTER_100_LEVEL_RANK            "Colonel"
#define       STRCMD_CS_CHARACTER_104_LEVEL_RANK            "General"
#define       STRCMD_CS_CHARACTER_108_LEVEL_RANK            "Governer"
#define       STRCMD_CS_CHARACTER_112_LEVEL_RANK            "Conqueror"
#define       STRCMD_CS_CHARACTER_116_LEVEL_RANK            "Guardian"
#define       STRCMD_CS_CHARACTER_120_LEVEL_RANK            "Legendary"
#define       STRCMD_CS_CHARACTER_XX_LEVEL_RANK             "Basic"

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


#define STRMSG_C_131205_0001	"Tumu"
#define STRMSG_C_131205_0002	"Silah"
#define STRMSG_C_131205_0003	"Standart Silah"
#define STRMSG_C_131205_0004	"Vulkan"
#define STRMSG_C_131205_0005	"Top"
#define STRMSG_C_131205_0006	"Gatling"
#define STRMSG_C_131205_0007	"Tufek"
#define STRMSG_C_131205_0008	"Otomatik"
#define STRMSG_C_131205_0009	"Duellocu"
#define STRMSG_C_131205_0010	"Mass Drive"
#define STRMSG_C_131205_0011	"Gelismis Silah"
#define STRMSG_C_131205_0012	"Fuze"
#define STRMSG_C_131205_0013	"Paket"
#define STRMSG_C_131205_0014	"Zirh"
#define STRMSG_C_131205_0015	"Ortu"
#define STRMSG_C_131205_0016	"Savunucu"
#define STRMSG_C_131205_0017	"Koruyucu"
#define STRMSG_C_131205_0018	"Baglayici"
#define STRMSG_C_131205_0019	"Radar"
#define STRMSG_C_131205_0020	"Yardimci Ekipman"
#define STRMSG_C_131205_0021	"Sinirsiz Aksesuar"
#define STRMSG_C_131205_0022	"Sureli Aksesuar"
#define STRMSG_C_131205_0023	"Bilgisayar"
#define STRMSG_C_131205_0024	"Motor"
#define STRMSG_C_131205_0025	"Tuketilebilirler"
#define STRMSG_C_131205_0026	"Iyilestirme Kiti"
#define STRMSG_C_131205_0027	"Gamble Kiti"
#define STRMSG_C_131205_0028	"Enchant Karti"
#define STRMSG_C_131205_0029	"Normal Kart"
#define STRMSG_C_131205_0030	"Sans Kutusu"
#define STRMSG_C_131205_0031	"Diger"
#define STRMSG_C_131205_0032	"Maden"
#define STRMSG_C_131205_0033	"Yanlis esyayi sectiniz veya esya kullanilabilir degil. "
#define STRMSG_C_131205_0034	"+%d %s satin almak istiyor musunuz? \\n(Fiyat : %s SPI) "
#define STRMSG_C_131205_0035	"+%d %s satin almak istiyor musunuz? \\n(Fiyat : %s WP) "
#define STRMSG_C_131205_0036	"(Esya sayisi : %d)"
#define STRMSG_C_131205_0037	"Bos alan nedeniyle kaydedilemiyor. "
#define STRMSG_C_131205_0038	"Esyayi ticaret dukkanina kaydetmek istiyor musunuz?"
#define STRMSG_C_131205_0039	"Esya ticaret dukkanina basariyla kaydedildi. "
#define STRMSG_C_131205_0040	"Suresi doldu."
#define STRMSG_C_131205_0041	"%dgun"
#define STRMSG_C_131205_0042	"%dkez"
#define STRMSG_C_131205_0043	"%ddakika"
#define STRMSG_C_131205_0044	"Bir dakikadan az"
#define STRMSG_C_131205_0045	"\\ySatildi\\y"
#define STRMSG_C_131205_0046	"\\rSuresi Doldu\\r"
#define STRMSG_C_131205_0047	"Ticaret dukkanina kayitli esyayi geri almak istiyor musunuz?"
#define STRMSG_C_131205_0048	"Suresi dolan esyayi geri almak istiyor musunuz?"
#define STRMSG_C_131205_0049	"Satilan esyanin parasini cekmek istiyor musunuz?\\n(Satis Kesintisi : %s %s) "
#define STRMSG_C_131205_0050	"Lutfen birkac saniye bekleyip tekrar deneyin. "
#define STRMSG_C_131205_0051	"Satistaki esyalar geri alindi. "
#define STRMSG_C_131205_0052	"Suresi dolan esyalar geri alindi. "
#define STRMSG_C_131205_0053	"Satis bedeli cekildi. "
#define STRMSG_C_131205_0054	"Geri alma basarisiz. Lutfen tekrar deneyin. "
#define STRMSG_C_131205_0055	"Geri Alindi"
#define STRMSG_C_131205_0056	"Para Cekildi"
// END 2013-12-05 by ymjoo 거래소 구현 스트링

// 2013-12-05 by ymjoo 거래소 구현 스트링
#define STRMSG_C_131206_0001	"Esyayi basariyla satin aldiniz.\\nEsya envanterinize gonderilecek. "

#define STRMSG_C_131206_0002	"\\y %d Savas Puani azaldi."

// 2013-11-29 by ssjung 거래소 구현
#define STRMSG_C_131217_0001	"Kaydedilebilecek esya sayisini astiniz."
#define STRMSG_C_131217_0002	"Stok tukenmis."
#define STRMSG_C_131217_0003	"Yetersiz bakiye nedeniyle satin alamazsiniz."
#define STRMSG_C_131217_0004	"Maksimum esya sayisi asildigi icin satin alamazsiniz."
#define STRMSG_C_131217_0005	"Envanterde yeterli yer olmadigi icin satin alamazsiniz."
#define STRMSG_C_131217_0006	"Maksimum esya sayisi asildigi icin geri alamazsiniz."
#define STRMSG_C_131217_0007	"Envanterde yeterli yer olmadigi icin geri alamazsiniz."
#define STRMSG_C_131217_0008	"Geri alma ucretleri : SPI(%.1f%%), WP(%.1f%%)"
#define STRMSG_C_131217_0009	"Lutfen ticaret dukkanindaki esya listesini guncelleyin."
// end 2013-11-29 by ssjung 거래소 구현


#endif // end_#ifndef _STRING_DEFINE_COMMON_H_





