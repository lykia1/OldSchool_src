// 2005-04-28 by cmkwon

#ifndef _STRING_DEFINE_COMMON_H_
#define _STRING_DEFINE_COMMON_H_

///////////////////////////////////////////////////////////////////////////////
// 1 - FieldServer?? ???? –½—ßŒê
	#define STRCMD_CS_COMMAND_MENT_0					"/ƒRƒƒ“ƒg"
	#define STRCMD_CS_COMMAND_MENT_1					"/ƒRƒƒ“ƒg"
	#define STRCMD_CS_COMMAND_MENT_2					"/ment"
	#define STRCMD_CS_COMMAND_MENT_HELP					"format: /ƒRƒƒ“ƒg [|String] - ƒLƒƒƒ‰ƒNƒ^[ƒRƒƒ“ƒgÝ’è"
	#define STRCMD_CS_COMMAND_MOVE						"/ˆÚ“®"
	#define STRCMD_CS_COMMAND_MOVE_1					"/move"
	#define STRCMD_CS_COMMAND_MOVE_HELP					"format: /ˆÚ“® [MapIndex] [|ChannelIndex] - ŠY“– ƒ}ƒbƒvƒ`ƒƒƒ“ƒlƒ‹‚ÉˆÚ“®"
	#define STRCMD_CS_COMMAND_COORDINATE				"/À•W"
	#define STRCMD_CS_COMMAND_COORDINATE_1				"/coordinate"
	#define STRCMD_CS_COMMAND_COORDINATE_HELP			"format: /À•W [X] [Y] - Œ»Ýƒ}ƒbƒv‚ÌŠY“–À•W‚ÉˆÚ“®"
	#define STRCMD_CS_COMMAND_LIST						"/ƒŠƒXƒg"
	#define STRCMD_CS_COMMAND_LIST_1					"/list"
	#define STRCMD_CS_COMMAND_LIST_HELP					"format: /ƒŠƒXƒg - Œ»Ýƒ}ƒbƒv‚ÌƒvƒŒ[ƒ„[ƒŠƒXƒg‚ðo—Í (Å‘å20l)"
	#define STRCMD_CS_COMMAND_USERSEND					"/ƒLƒƒƒ‰ˆÚ“®"
	#define STRCMD_CS_COMMAND_USERSEND_1				"/senduser"
	#define STRCMD_CS_COMMAND_USERSEND_HELP				"format: /ƒLƒƒƒ‰ˆÚ“® [character name] [map name] - ŠY“–ƒLƒƒƒ‰ƒNƒ^[‚ðŽw’è‚µ‚½ƒ}ƒbƒv‚ÉˆÚ“®"
	#define STRCMD_CS_COMMAND_INFObyNAME				"/î•ñ"
	#define STRCMD_CS_COMMAND_INFObyNAME_1				"/info"
	#define STRCMD_CS_COMMAND_INFObyNAME_HELP			"format: /î•ñ [monster name|item name] | –¼‘O‚ÉŠY“–•¶Žš‚ªŠÜ‚Ü‚ê‚Ä‚¢‚éƒ‚ƒ“ƒXƒ^[‚âƒAƒCƒeƒ€‚Ìî•ñ‚ðo—Í"
	#define STRCMD_CS_COMMAND_QUESTINFO					"/ƒ~ƒbƒVƒ‡ƒ“"
	#define STRCMD_CS_COMMAND_QUESTINFO_1				"/quest"
	#define STRCMD_CS_COMMAND_QUESTINFO_HELP			"format: /ƒ~ƒbƒVƒ‡ƒ“ - ƒLƒƒƒ‰ƒNƒ^[‚Ìƒ~ƒbƒVƒ‡ƒ“î•ñ‚ðo—Í"
	#define STRCMD_CS_COMMAND_QUESTDEL					"/ƒ~ƒbƒVƒ‡ƒ“Á‹Ž"
	#define STRCMD_CS_COMMAND_QUESTDEL_1				"/delQuest"
	#define STRCMD_CS_COMMAND_QUESTDEL_HELP				"format: /ƒ~ƒbƒVƒ‡ƒ“Á‹Ž [ƒ~ƒbƒVƒ‡ƒ“”Ô†]"
	#define STRCMD_CS_COMMAND_ITEMINFObyKIND			"/Ží—Þ"
	#define STRCMD_CS_COMMAND_ITEMINFObyKIND_1			"/itemKind"
	#define STRCMD_CS_COMMAND_ITEMINFObyKIND_HELP		"format: /Ží—Þ [|item kind(0`53)] - ŠY“–Ží—Þ‚ÌƒAƒCƒeƒ€‚ðo—Í"
	#define STRCMD_CS_COMMAND_ITEMINSERTbyKIND			"/Ží—ÞƒAƒCƒeƒ€"
	#define STRCMD_CS_COMMAND_ITEMINSERTbyKIND_1		"/insertItemKind"
	#define STRCMD_CS_COMMAND_ITEMINSERTbyKIND_HELP		"format: /Ží—ÞƒAƒCƒeƒ€[item kind(0`53)] - ŠY“–Ží—Þ‚ÌƒAƒCƒeƒ€‚ð’Ç‰Á"
	#define STRCMD_CS_COMMAND_ITEMINSERTbyITEMNUMRANGE		"/”ÍˆÍƒAƒCƒeƒ€"
	#define STRCMD_CS_COMMAND_ITEMINSERTbyITEMNUMRANGE_1	"/insertItemNumRange"
	#define STRCMD_CS_COMMAND_ITEMINSERTbyITEMNUMRANGE_HELP	"format: /”ÍˆÍƒAƒCƒeƒ€[from itemnum] ` [to itemnum] - ŠY“–ƒAƒCƒeƒ€‚ð’Ç‰Á"
	#define STRCMD_CS_COMMAND_STATINIT					"/ƒXƒe[ƒ^ƒX‰Šú‰»"
	#define STRCMD_CS_COMMAND_STATINIT_1				"/ƒXƒe[ƒ^ƒX‰Šú‰»"
	#define STRCMD_CS_COMMAND_STATINIT_2				"/initStatus"
	#define STRCMD_CS_COMMAND_STATINIT_HELP				"format: /ƒXƒe[ƒ^ƒX‰Šú‰» [|CharacterName]- ‘S‘ÌƒXƒe[ƒ^ƒX‰Šú‰»"
	#define STRCMD_CS_COMMAND_PARTYINFO					"/•Ò‘à"
	#define STRCMD_CS_COMMAND_PARTYINFO_1				"/partyInfo"
	#define STRCMD_CS_COMMAND_PARTYINFO_HELP			"format: /•Ò‘à - •Ò‘àî•ñ‚ðo—Í"
	#define STRCMD_CS_COMMAND_GAMETIME					"/ŽžŠÔ"
	#define STRCMD_CS_COMMAND_GAMETIME_1				"/Time"
	#define STRCMD_CS_COMMAND_GAMETIME_HELP				"format: /ŽžŠÔ [|‰ÁŽZŽžŠÔ(0`23)] - Œ»ÝŽžŠÔ‚ð•ÏX(Ž©•ª‚ÌŽžŠÔ‚Ì‚Ý•ÏX‚³‚ê‚é)"
	#define STRCMD_CS_COMMAND_STRINGLEVEL_0				"/ƒXƒgƒŠƒ“ƒO"
	#define STRCMD_CS_COMMAND_STRINGLEVEL_1				"/ƒXƒgƒŠƒ“ƒO"
	#define STRCMD_CS_COMMAND_STRINGLEVEL_2				"/string"
	#define STRCMD_CS_COMMAND_STRINGLEVEL_HELP			"format: /ƒXƒgƒŠƒ“ƒO [0`5] | ƒfƒoƒbƒOƒƒbƒZ[ƒW‚ðo—Í‚·‚élevel‚ðŒˆ’è"
	#define STRCMD_CS_COMMAND_MONSUMMON					"/¢Š«"
	#define STRCMD_CS_COMMAND_MONSUMMON_1				"/summon"
	#define STRCMD_CS_COMMAND_MONSUMMON_HELP			"format: /¢Š« [monster number|monster name] [# of monsters] - ƒ‚ƒ“ƒXƒ^[‚ð ¢Š«(ƒ‚ƒ“ƒXƒ^[‚Ì–¼‘O‚ÉƒXƒy[ƒX‚ª‚ ‚éê‡‚ÍƒXƒy[ƒX‚Ì‘ã‚í‚è‚Éu_v(ƒAƒ“ƒ_[ƒo[)‚ð“ü—Í)"
	#define STRCMD_CS_COMMAND_SKILLALL					"/‘SƒXƒLƒ‹"
	#define STRCMD_CS_COMMAND_SKILLALL_1				"/allSkill"
	#define STRCMD_CS_COMMAND_SKILLALL_HELP				"format: /‘SƒXƒLƒ‹ [level] - ŠY“–‚Ì‘S‚Ä‚ÌƒXƒLƒ‹‚ð‘}“ü"
	#define STRCMD_CS_COMMAND_ITEMINSERTALL				"/‘SƒAƒCƒeƒ€"
	#define STRCMD_CS_COMMAND_ITEMINSERTALL_1			"/allIte"
	#define STRCMD_CS_COMMAND_ITEMINSERTALL_HELP		"format: /‘SƒAƒCƒeƒ€ - ŠY“–‚·‚é‘S‚Ä‚ÌƒAƒCƒeƒ€‚ð‘}“üAƒXƒLƒ‹‚ÆCountableƒAƒCƒeƒ€‚ðœŠO"
	#define STRCMD_CS_COMMAND_ITEMINSERTWEAPON			"/‘S•Ší"
	#define STRCMD_CS_COMMAND_ITEMINSERTWEAPON_1		"/allWeapon"
	#define STRCMD_CS_COMMAND_ITEMINSERTWEAPON_HELP		"format: /‘S•Ší | ŠY“–ƒMƒA‚Ì‘S‚Ä‚Ì•Ší‚ð’Ç‰Á"
	#define STRCMD_CS_COMMAND_ITEMDELALL_0				"/‘SƒAƒCƒeƒ€‚ðŽÌ‚Ä‚é"
	#define STRCMD_CS_COMMAND_ITEMDELALL_1				"/‘SƒAƒCƒeƒ€‚ðŽÌ‚Ä‚é"
	#define STRCMD_CS_COMMAND_ITEMDELALL_2				"/delAllItem"
	#define STRCMD_CS_COMMAND_ITEMDELALL_HELP			"format: /‘SƒAƒCƒeƒ€‚ðŽÌ‚Ä‚é- ŠY“–‚Ì‘S‚Ä‚Ì‘•”õ•s‰ÂƒAƒCƒeƒ€‚ðŽÌ‚Ä‚éB (ƒXƒLƒ‹œŠO)"
	#define STRCMD_CS_COMMAND_ITEMINSERTbyITEMNUM		"/ƒAƒCƒeƒ€"
	#define STRCMD_CS_COMMAND_ITEMINSERTbyITEMNUM_1		"/item"
	#define STRCMD_CS_COMMAND_ITEMINSERTbyITEMNUM_HELP	"format: /ƒAƒCƒeƒ€ [item number] [# of items] - ƒAƒCƒeƒ€‚ðŽæ“¾‚·‚éBŽæ“¾”[# of items]‚ð“ü—Í‚µ‚È‚¢ê‡A1ŒÂ‚¾‚¯Žæ“¾B"
	#define STRCMD_CS_COMMAND_ITEMDROP					"/ƒhƒƒbƒv"
	#define STRCMD_CS_COMMAND_ITEMDROP_1				"/dropItem"
	#define STRCMD_CS_COMMAND_ITEMDROP_HELP				"format: /ƒhƒƒbƒv [item number] [|# of items] - ƒAƒCƒeƒ€‚ðƒhƒƒbƒv"
	#define STRCMD_CS_COMMAND_USERSINFOTOTAL			"/ƒT[ƒo"
	#define STRCMD_CS_COMMAND_USERSINFOTOTAL_1			"/server"
	#define STRCMD_CS_COMMAND_USERSINFOTOTAL_HELP		"format: /ƒT[ƒo - ƒT[ƒo‚Ìî•ñ‚ðo—Í"
	#define STRCMD_CS_COMMAND_USERSINFOperMAP			"/ƒT[ƒoƒ}ƒbƒv"
	#define STRCMD_CS_COMMAND_USERSINFOperMAP_1			"/serverMap"
	#define STRCMD_CS_COMMAND_USERSINFOperMAP_HELP		"format: /ƒT[ƒoƒ}ƒbƒv | ‘S‚Ä‚Ìƒ}ƒbƒv‚Ìî•ñ‚ðo—Í"
	#define STRCMD_CS_COMMAND_CHANNELINFO				"/ƒ`ƒƒƒ“ƒlƒ‹"
	#define STRCMD_CS_COMMAND_CHANNELINFO_1				"/channelInfo"
	#define STRCMD_CS_COMMAND_CHANNELINFO_HELP			"format: /ƒ`ƒƒƒ“ƒlƒ‹ - Œ»Ýƒ}ƒbƒv‚Ìƒ`ƒƒƒ“ƒlƒ‹î•ñ‚ðo—Í"
	#define STRCMD_CS_COMMAND_DEBUGPRINTDBG				"/DBG"
	#define STRCMD_CS_COMMAND_DEBUGPRINTDBG_1			"/dbg"
	#define STRCMD_CS_COMMAND_DEBUGPRINTDBG_HELP		"format: /DBG - ƒeƒXƒg—p"
	#define STRCMD_CS_COMMAND_DEBUGSETPARAMF			"/testf"
	#define STRCMD_CS_COMMAND_DEBUGSETPARAMF_HELP		"format: /testf [Param1] [Param2] [Param3]"
	#define STRCMD_CS_COMMAND_BULLETCHARGE				"/e’e"
	#define STRCMD_CS_COMMAND_BULLETCHARGE_1			"/chargeBullet"
	#define STRCMD_CS_COMMAND_BULLETCHARGE_HELP			"format: /e’e [|1Œ^e’e”] [|2Œ^e’e”] - e’e‚ð•â‹‹"
	#define STRCMD_CS_COMMAND_REPAIRALL					"/‘S‰ñ•œ"
	#define STRCMD_CS_COMMAND_REPAIRALL_1				"/repairAll"
	#define STRCMD_CS_COMMAND_REPAIRALL_HELP			"format: /‘S‰ñ•œ [|character name] - HP, UTC, FUEL‚ð 100% ‰ñ•œ‚·‚éA[character name]‚ª“ü—Í‚³‚ê‚Ä‚¢‚È‚¢ê‡AŽ©•ª‚ð‰ñ•œ"
	#define STRCMD_CS_COMMAND_REPAIRbyPARAM				"/ŠeŽíŒ¸­"
	#define STRCMD_CS_COMMAND_REPAIRbyPARAM_1			"/repairParam"
	#define STRCMD_CS_COMMAND_REPAIRbyPARAM_HELP		"format: /ŠeŽíŒ¸­ [|Œ¸­’l%] [|character name] - HP, UTC, FUEL‚ð [Œ¸­’l%]‚É‚·‚éA[Œ¸­’l%]‚Ì“ü—Í‚ª‚È‚¢ê‡‚Í50%A [character name]‚ª“ü—Í‚³‚ê‚Ä‚¢‚È‚¢ê‡‚ÍŽ©•ªŽ©g‚ðŒ¸­B"
	#define STRCMD_CS_COMMAND_USERNORMALIZE				"/ˆê”Ê"
	#define STRCMD_CS_COMMAND_USERNORMALIZE_1			"/normal"
	#define STRCMD_CS_COMMAND_USERNORMALIZE_HELP		"format: /ˆê”Ê -‰^‰cŽÒƒQ[ƒ€ƒ}ƒXƒ^[ƒAƒJƒEƒ“ƒg‚ÅƒƒOƒCƒ“‚µ‚½ê‡Aˆê”ÊƒAƒJƒEƒ“ƒg‚Åˆø‚«‘Ö‚¦i‹@ŠB–|–ój"
	#define STRCMD_CS_COMMAND_USERSPECIALIZE			"/“ÁŒ "
	#define STRCMD_CS_COMMAND_USERSPECIALIZE_1			"/specialize"
	#define STRCMD_CS_COMMAND_USERSPECIALIZE_HELP		"format: /“ÁŒ  -‰^‰cŽÒ‚âƒQ[ƒ€ƒ}ƒXƒ^[ƒAƒJƒEƒ“ƒg‚ÅƒƒOƒCƒ“‚µ‚½ê‡Aˆê”ÊƒAƒJƒEƒ“ƒg‚Ü‚½‚Í“ÁŽêƒAƒJƒEƒ“ƒg‚Åˆø‚«‘Ö‚¦i‹@ŠB–|–ój"
	#define STRCMD_CS_COMMAND_USERINVINCIBILITY			"/–³“G"
	#define STRCMD_CS_COMMAND_USERINVINCIBILITY_1		"/invincible"
	#define STRCMD_CS_COMMAND_USERINVINCIBILITY_HELP	"format: /–³“G- ‰^‰cŽÒ‚âƒQ[ƒ€ƒ}ƒXƒ^[ƒAƒJƒEƒ“ƒg‚Ìê‡Aƒ_ƒ[ƒW‚ðŽó‚¯‚È‚¢"
	#define STRCMD_CS_COMMAND_POWERUP					"/Žm‹C"
	#define STRCMD_CS_COMMAND_POWERUP_1					"/powerUp"
	#define STRCMD_CS_COMMAND_POWERUP_HELP				"format: /Žm‹C [UŒ‚—Í ‘‰Á—Ê(%%)]"
	#define STRCMD_CS_COMMAND_VARIABLESET				"/•Ï”"
	#define STRCMD_CS_COMMAND_VARIABLESET_1				"/setVariable"
	#define STRCMD_CS_COMMAND_VARIABLESET_HELP			"format: /•Ï” [•Ï”’l] - (ˆê”Ê)•Ï” ’²®"
	#define STRCMD_CS_COMMAND_LEVELSET					"/ƒŒƒxƒ‹"
	#define STRCMD_CS_COMMAND_LEVELSET_1				"/level"
	#define STRCMD_CS_COMMAND_LEVELSET_HELP				"format: /·¹º§ [|level] [|percentage of exp] [|character name] - ·¹º§À» Á¶Á¤ÇÔ"
	/////////////////////////////////////////////////
	// start 2011-09-05 by hskim, ÆÄÆ®³Ê ½Ã½ºÅÛ 2Â÷
	#define STRCMD_CS_COMMAND_PARTNERLEVELSET			"/ÆÄÆ®³Ê·¹º§"
	#define STRCMD_CS_COMMAND_PARTNERLEVELSET_1			"/partnerlevel"
	#define STRCMD_CS_COMMAND_PARTNERLEVELSET_HELP		"format: /ÆÄÆ®³Ê·¹º§ [|level] [|percentage of exp] - ÆÄÆ®³Ê ·¹º§À» Á¶Á¤ÇÔ"
	// end 2011-09-05 by hskim, ÆÄÆ®³Ê ½Ã½ºÅÛ 2Â÷
	/////////////////////////////////////////////////
	#define STRCMD_CS_COMMAND_USERINVISIABLE			"/Åõ¸í"
	#define STRCMD_CS_COMMAND_USERINVISIABLE_1			"/invisible"
	#define STRCMD_CS_COMMAND_USERINVISIABLE_HELP		"format: /“§–¾ | Ž©•ª‚ª‚Ù‚©‚ÌƒLƒƒƒ‰ƒNƒ^[‚ÉŒ©‚¦‚È‚¢"
	#define STRCMD_CS_COMMAND_DEBUGPRINTMSGF_0			"/messagef"
	#define STRCMD_CS_COMMAND_DEBUGPRINTMSGF_1			"/msgf"
	#define STRCMD_CS_COMMAND_DEBUGPRINTMSGF_HELP		"format: /msgf | ƒeƒXƒg—p"
	#define STRCMD_CS_COMMAND_GAMEEVENT					"/ƒCƒxƒ“ƒg"
	#define STRCMD_CS_COMMAND_GAMEEVENT_1				"/event"
	#define STRCMD_CS_COMMAND_GAMEEVENT_P1EXP			"ŒoŒ±’l"
	#define STRCMD_CS_COMMAND_GAMEEVENT_P1SPI			"SPI"
	#define STRCMD_CS_COMMAND_GAMEEVENT_P1EXPR			"ŒoŒ±’l•œ‹Œ"
	#define STRCMD_CS_COMMAND_GAMEEVENT_P1ITEM			"ƒAƒCƒeƒ€"
	#define STRCMD_CS_COMMAND_GAMEEVENT_P1RARE			"ƒŒƒAƒAƒCƒeƒ€"
	#define STRCMD_CS_COMMAND_GAMEEVENT_P2END			"I—¹"
	#define STRCMD_CS_COMMAND_GAMEEVENT_HELP			"format: /ƒCƒxƒ“ƒg [ŒoŒ±’lbSPIbŒoŒ±’l•œ‹ŒbƒAƒCƒeƒ€bƒŒƒAƒAƒCƒeƒ€bí‘ˆƒ|ƒCƒ“ƒgiWPj] [|”{”(%%f)|I—¹] [ŽžŠÔ(’PˆÊ:•ª)] [|¨—Í(255:ALL,1:Normal,2:VCN,4:ANI)]- ƒCƒxƒ“ƒg Ý’è,‰ðœ"
	#define STRCMD_CS_COMMAND_PREMEUM					"/ƒvƒŒƒ~ƒAƒ€"
	#define STRCMD_CS_COMMAND_PREMEUM_1					"/premium"
	#define STRCMD_CS_COMMAND_PREMEUM_PNORMAL			"ˆê”Ê"
	#define STRCMD_CS_COMMAND_PREMEUM_PSUPER			"ƒX[ƒp["
	#define STRCMD_CS_COMMAND_PREMEUM_PUPGRADE			"ƒAƒbƒvƒOƒŒ[ƒh"
	#define STRCMD_CS_COMMAND_PREMEUM_PEND				"I—¹"
	#define STRCMD_CS_COMMAND_PREMEUM_HELP				"format: /ƒvƒŒƒ~ƒAƒ€ [ˆê”Ê|ƒX[ƒp[|ƒAƒbƒvƒOƒŒ[ƒh|I—¹]"
// 2008-02-14 by cmkwon, “sŽsè—Ìí –½—ßŒê ??
// 	#define STRCMD_CS_COMMAND_CITYWAR					"/“sŽsè—Ìí"
// 	#define STRCMD_CS_COMMAND_CITYWAR_1					"/citywar"
// 	#define STRCMD_CS_COMMAND_CITYWAR_PSTART			"ŠJŽn"
// 	#define STRCMD_CS_COMMAND_CITYWAR_PEND				"I—¹"
// 	#define STRCMD_CS_COMMAND_CITYWAR_HELP				"format: /“sŽsè—Ìí [ŠJŽn|I—¹]"
	#define STRCMD_CS_COMMAND_STEALTH					"/ƒXƒeƒ‹ƒX"
	#define STRCMD_CS_COMMAND_STEALTH_1					"/stealth"
	#define STRCMD_CS_COMMAND_STEALTH_HELP				"format: /ƒXƒeƒ‹ƒX | æ§UŒ‚ƒ‚ƒ“ƒXƒ^[‚ªæU‚µ‚È‚¢"
	#define STRCMD_CS_COMMAND_RETURNALL					"/‘S‚Ä‹AŠÒ"
	#define STRCMD_CS_COMMAND_RETURNALL_1				"/returnAll"
	#define STRCMD_CS_COMMAND_RETURNALL_HELP			"format: /‘S‚Ä‹AŠÒ - ŠY“– ¨—Í•Ê “sŽsƒ}ƒbƒv‚ÉˆÚ“®‚·‚é"

// start 2011-06-22 by hskim, »ç¼³ ¼­¹ö ¹æÁö
	#define STRCMD_CS_COMMAND_SERVERINFO				"/getserverinfo"		// ¼­¹ö Á¤º¸ º¸±â
// end 2011-06-22 by hskim, »ç¼³ ¼­¹ö ¹æÁö

// 2007-10-30 by cmkwon, ¼¼·Âº° ÇØÇÇ¾Æ¿ö ÀÌº¥Æ® ±¸Çö - ¸í·É¾î Çü½Ä ¼öÁ¤µÊ ¾Æ·¡¿¡¼­ ´Ù½Ã Á¤ÀÇ ÇÔ
//	#define STRCMD_CS_COMMAND_HAPPYHOUREVENT			"/HappyHourƒCƒxƒ“ƒg"
//	#define STRCMD_CS_COMMAND_HAPPYHOUREVENT_1			"/happyEvent"
//	#define STRCMD_CS_COMMAND_HAPPYHOUREVENT_PSTART		"ŠJŽn"
//	#define STRCMD_CS_COMMAND_HAPPYHOUREVENT_PEND		"I—¹"
//	#define STRCMD_CS_COMMAND_HAPPYHOUREVENT_HELP		"format: /HappyHourƒCƒxƒ“ƒg [ŠJŽn|I—¹] [isŽžŠÔ(’PˆÊ:•ª)]"

// 1_end
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// 2 - IMServer?? ???? –½—ßŒê, ??? ??? ??? –½—ßŒê? ??? ???
	#define STRCMD_CS_COMMAND_MULTICHAT					"/multichat"
	#define STRCMD_CS_COMMAND_MULTICHAT_1				"/multichat"
	#define STRCMD_CS_COMMAND_MULTICHAT_HELP			"format: /multichat [ | on | off] - Enable or disable multi chat, if no parameter on the command, it send the current statues of the multi chat"

	#define STRCMD_CS_COMMAND_DEBUGSETPARAMI			"/testi"
	#define STRCMD_CS_COMMAND_DEBUGSETPARAMI_HELP		"format: /testi - IMServer ????"
	#define STRCMD_CS_COMMAND_WHO						"/’N"
	#define STRCMD_CS_COMMAND_WHO_1						"/who"
	#define STRCMD_CS_COMMAND_WHO_HELP					"format: /’N [|# of users] - Œ»Ý ƒT[ƒo‚É‚ ‚éƒ†[ƒU[‚ð‘S‚Äo—Í (ƒ}ƒbƒv‚ÆŠÖŒW‚È‚¢)"
	#define STRCMD_CS_COMMAND_REGISTERADMIN				"/‰^‰c“o˜^"
	#define STRCMD_CS_COMMAND_REGISTERADMIN_1			"/registerAdmin"
	#define STRCMD_CS_COMMAND_REGISTERADMIN_HELP		"format: /‰^‰c“o˜^ - ‰^‰cŽÒ‚É“Á’èƒCƒxƒ“ƒg”­¶Žž‚ÌƒƒbƒZ[ƒW‚ðŽó‚¯‚é‚æ‚¤‚ÉƒT[ƒo‚É“o˜^‚·‚é"
	#define STRCMD_CS_COMMAND_DEBUGPRINTMSGI_0			"/messagei"
	#define STRCMD_CS_COMMAND_DEBUGPRINTMSGI_1			"/msgi"
	#define STRCMD_CS_COMMAND_DEBUGPRINTMSGI_HELP		"format: /msgi - ƒNƒ‰ƒCƒAƒ“ƒg‚ÆIM ƒT[ƒo‚Æ‚ÌŠÔ‚ÌƒvƒƒgƒRƒ‹‚ð‘S‚Äo—Í‚·‚é"
	#define STRCMD_CS_COMMAND_SERVERDOWN				"/ƒT[ƒoƒ_ƒEƒ“"
	#define STRCMD_CS_COMMAND_SERVERDOWN_1				"/serverDown"
	#define STRCMD_CS_COMMAND_SERVERDOWN_HELP			"format: /ƒT[ƒoƒ_ƒEƒ“ [”FØ”Žš] - ƒT[ƒo‚ð I—¹‚³‚¹‚é"
	#define STRCMD_CS_COMMAND_WHOAREYOU					"/’N2"
	#define STRCMD_CS_COMMAND_WHOAREYOU_1				"/whoareYou"
	#define STRCMD_CS_COMMAND_WHOAREYOU_HELP			"format: /’N2 [character name]"
	#define STRCMD_CS_COMMAND_GOUSER					"/s‚­"
	#define STRCMD_CS_COMMAND_GOUSER_1					"/go"
	#define STRCMD_CS_COMMAND_GOUSER_HELP				"format: /s‚­ [character name] - ŠY“– ƒLƒƒƒ‰ƒNƒ^[‚ÌêŠ‚ÉˆÚ“®‚·‚é"
	#define STRCMD_CS_COMMAND_COMEON					"/ŒÄ‚Ô"
	#define STRCMD_CS_COMMAND_COMEON_1					"/comeon"
	#define STRCMD_CS_COMMAND_COMEON_HELP				"format: /ŒÄ‚Ô [character name] - ŠY“–ƒLƒƒƒ‰ƒNƒ^[‚ðŒÄ‚Ô"
	#define STRCMD_CS_COMMAND_GUILDCOMEON				"/—·’cŒÄ‚Ô"
	#define STRCMD_CS_COMMAND_GUILDCOMEON_1				"/comeonGuild"
	#define STRCMD_CS_COMMAND_GUILDCOMEON_HELP			"format: /—·’cŒÄ‚Ô [—·’c–¼] - ŠY“– —·’cƒƒ“ƒo[‚ð‘S‚ÄŒÄ‚Ô"
	#define STRCMD_CS_COMMAND_GUILDSEND					"/—·’c‘—‚é"
	#define STRCMD_CS_COMMAND_GUILDSEND_1				"/sendGuild"
	#define STRCMD_CS_COMMAND_GUILDSEND_HELP			"format: /—·’c‘—‚é [—·’c–¼] [map name] - ŠY“– —·’cƒƒ“ƒo[‚ð ƒ}ƒbƒv‚ÉˆÚ“®‚³‚¹‚é"
	#define STRCMD_CS_COMMAND_CHATPTOPFLAG				"/‚³‚³‚â‚«"
	#define STRCMD_CS_COMMAND_CHATPTOPFLAG_1			"/whisperChat"
	#define STRCMD_CS_COMMAND_CHATPTOPFLAG_HELP			"format: /‚³‚³‚â‚« | ‚³‚³‚â‚«‚ðƒuƒƒbƒN‚â‰ðœ‚·‚éiOn/Offj"
	#define STRCMD_CS_COMMAND_GUILDINFO					"/—·’c"
	#define STRCMD_CS_COMMAND_GUILDINFO_1				"/guildInfo"
	#define STRCMD_CS_COMMAND_GUILDINFO_HELP			"format: /—·’c - —·’c î•ñ o—Í"
	#define STRCMD_CS_COMMAND_WEATHERSET				"/“V‹C"
	#define STRCMD_CS_COMMAND_WEATHERSET_1				"/weather"
	#define STRCMD_CS_COMMAND_WEATHERSET_P1NORMAL		"Šî–{"
	#define STRCMD_CS_COMMAND_WEATHERSET_P1FINE			"°‚ê"
	#define STRCMD_CS_COMMAND_WEATHERSET_P1RAIN			"‰J"
	#define STRCMD_CS_COMMAND_WEATHERSET_P1SNOW			"á"
	#define STRCMD_CS_COMMAND_WEATHERSET_P1CLOUDY		"“Ü‚è"
	#define STRCMD_CS_COMMAND_WEATHERSET_P1FOG			"–¶"
	#define STRCMD_CS_COMMAND_WEATHERSET_P2ALL			"‘S‘Ì"
	#define STRCMD_CS_COMMAND_WEATHERSET_P3ON			"on"
	#define STRCMD_CS_COMMAND_WEATHERSET_P3OFF			"off"
	#define STRCMD_CS_COMMAND_WEATHERSET_HELP			"format: /“V‹C [Šî–{|°‚ê|‰J|á|“Ü‚è|–¶] [‘S‘Ì|ƒ}ƒbƒv–¼] [on|off] | “V‹C’²®"
	#define STRCMD_CS_COMMAND_CHATFORBID				"/ƒ`ƒƒƒbƒgƒuƒƒbƒN"
	#define STRCMD_CS_COMMAND_CHATFORBID_1				"/forbidChat"
	#define STRCMD_CS_COMMAND_CHATFORBID_HELP			"format: /ƒ`ƒƒƒbƒgƒuƒƒbƒN [character name] [ŽžŠÔ(•ª)] - ƒ`ƒƒƒbƒg‚ðƒuƒƒbƒN‚·‚é"
	#define STRCMD_CS_COMMAND_CHATFORBIDRELEASE			"/ƒ`ƒƒƒbƒgƒuƒƒbƒN‰ðœ"
	#define STRCMD_CS_COMMAND_CHATFORBIDRELEASE_1		"/releaseChat"
	#define STRCMD_CS_COMMAND_CHATFORBIDRELEASE_HELP	"format: /ƒ`ƒƒƒbƒgƒuƒƒbƒN‰ðœ [character name] - ƒ`ƒƒƒbƒgƒuƒƒbƒN‚ð‰ðœ‚·‚é"
	#define STRCMD_CS_COMMAND_COMMANDLIST_0				"/?"
	#define STRCMD_CS_COMMAND_COMMANDLIST_1				"/help"
	#define STRCMD_CS_COMMAND_COMMANDLIST_2				"/–½—ßŒê"
	#define STRCMD_CS_COMMAND_COMMANDLIST_HELP			"format: /? | –½—ßŒêƒŠƒXƒg‚ðo—Í"

	// 2005-07-20 by cmkwon
	#define STRCMD_CS_COMMAND_BONUSSTAT_0				"/BonusStat"
	#define STRCMD_CS_COMMAND_BONUSSTAT_1				"/ƒ{[ƒiƒXƒXƒe[ƒ^ƒX"
	#define STRCMD_CS_COMMAND_BONUSSTAT_2				"/ƒ{[ƒiƒXƒXƒe[ƒ^ƒX"
	#define STRCMD_CS_COMMAND_BONUSSTAT_HELP			"format: /BonusStat [Bonus Counts] [|character name] - ƒ{[ƒiƒXƒXƒe[ƒ^ƒXƒAƒbƒv"

// 2_end
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// 3 - AtumMonitor?? ???? –½—ßŒê, ??? ??? ??? –½—ßŒê? ??? ???
	#define STRCMD_CS_COMMAND_PASSWORDSET				"/ƒpƒXƒ[ƒhÝ’è"
	#define STRCMD_CS_COMMAND_PASSWORDSET_1				"/setPassword"
	#define STRCMD_CS_COMMAND_PASSWORDSET_HELP			"format: /ƒpƒXƒ[ƒhÝ’è [AccountName] [Password]"
	#define STRCMD_CS_COMMAND_PASSWORDROLLBACK			"/ƒpƒXƒ[ƒh•œ‹Œ"
	#define STRCMD_CS_COMMAND_PASSWORDROLLBACK_1		"/rollbackPassword"
	#define STRCMD_CS_COMMAND_PASSWORDROLLBACK_HELP		"format: /ƒpƒXƒ[ƒh•œ‹Œ [AccountName]"
	#define STRCMD_CS_COMMAND_PASSWORDLIST				"/ƒpƒXƒ[ƒhƒŠƒXƒg"
	#define STRCMD_CS_COMMAND_PASSWORDLIST_1			"/passwordList"
	#define STRCMD_CS_COMMAND_PASSWORDLIST_HELP			"format: /ƒpƒXƒ[ƒhƒŠƒXƒg"
	#define STRCMD_CS_COMMAND_PASSWORDENCRYPT			"/ƒpƒXƒ[ƒh‰»"
	#define STRCMD_CS_COMMAND_PASSWORDENCRYPT_1			"/encrypt"
	#define STRCMD_CS_COMMAND_PASSWORDENCRYPT_HELP		"format: /ƒpƒXƒ[ƒh‰» [ƒpƒXƒ[ƒh‰»‚·‚éƒXƒgƒŠƒ“ƒO]"
	#define STRCMD_CS_COMMAND_ACCOUNTBLOCK				"/ƒAƒJƒEƒ“ƒgƒuƒƒbƒN"
	#define STRCMD_CS_COMMAND_ACCOUNTBLOCK_1			"/blockAccount"
	#define STRCMD_CS_COMMAND_ACCOUNTBLOCKT_HELP		"format: /ƒAƒJƒEƒ“ƒgƒuƒƒbƒN [AccountName]"
	#define STRCMD_CS_COMMAND_ACCOUNTBLOCKRELEASE		"/ƒAƒJƒEƒ“ƒgƒuƒƒbƒN‰ðœ"
	#define STRCMD_CS_COMMAND_ACCOUNTBLOCKRELEASE_1		"/releaseAccount"
	#define STRCMD_CS_COMMAND_ACCOUNTBLOCKRELEASE_HELP	"format: /ƒAƒJƒEƒ“ƒgƒuƒƒbƒN‰ðœ [AccountName]"
	#define STRCMD_CS_COMMAND_ACCOUNTBLOCKLIST			"/ƒuƒƒbƒN‚³‚ê‚½ƒAƒJƒEƒ“ƒg"
	#define STRCMD_CS_COMMAND_ACCOUNTBLOCKLIST_1		"/blockedList"
	#define STRCMD_CS_COMMAND_ACCOUNTBLOCKLIST_HELP		"format: /ƒuƒƒbƒN‚³‚ê‚½ƒAƒJƒEƒ“ƒg"
// 3_end
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// 4 - CommonGameServer
	#define STRCMD_CS_COMMON_DB_0000 "MƒQ[ƒ€ƒT[ƒoê—p!!!!\r\n"
	#define STRCMD_CS_COMMON_DB_0001 "ƒT[ƒoƒƒOƒCƒ“ID‚ð“ü—Í‚µ‚Ä‚­‚¾‚³‚¢"
	#define STRCMD_CS_COMMON_DB_0002 "ƒT[ƒoƒƒOƒCƒ“PW‚ð“ü—Í‚µ‚Ä‚­‚¾‚³‚¢"
	#define STRCMD_CS_COMMON_DB_0003 "DBƒƒOƒCƒ“ID‚ð“ü—Í‚µ‚Ä‚­‚¾‚³‚¢B"
	#define STRCMD_CS_COMMON_DB_0004 "DBƒƒOƒCƒ“PW‚ð“ü—Í‚µ‚Ä‚­‚¾‚³‚¢B"

	#define STRCMD_CS_COMMON_MAP_0000 "ƒn[ƒhƒR[ƒfƒBƒ“ƒO•”•ª: 0101ƒ}ƒbƒv‚Å1”Ôƒ[ƒvƒ^[ƒQƒbƒg‚ð–³Ž‹Aƒ}ƒbƒvƒGƒfƒBƒ^‚©œ‹Ž\r\n"
	#define STRCMD_CS_COMMON_MAP_0001 "MAP: %04d, m_DefaltWarpTargetIndex: %d\r\n"
	#define STRCMD_CS_COMMON_MAP_0002 "ƒn[ƒhƒR[ƒfƒBƒ“ƒO•”•ª: 0101ƒ}ƒbƒv‚Å1”Ôƒ[ƒvƒ^[ƒQƒbƒg‚ð–³Ž‹Aƒ}ƒbƒvƒGƒfƒBƒ^‚©œ‹Ž\r\n "
	#define STRCMD_CS_COMMON_MAP_0003 "MAP: %04d, m_DefaltWarpTargetIndex: %d\r\n"
	#define STRCMD_CS_COMMON_MAP_0004 "    ObjMon ==> ObjNum[%8d] EvType[%d] EvIndex[%3d] ¢Š«ƒ‚ƒ“ƒXƒ^[[%8d] ¢Š«ŽžŠÔ[%6d•b]A Pos(%4d, %4d, %4d)\r\n"
	#define STRCMD_CS_COMMON_MAP_0005 "[ERROR] ObjectMonster EventParam1 Index d•¡ Error ==> ObjectNum[%8d] EventType[%d] EventIndex[%3d] ¢Š« ƒ‚ƒ“ƒXƒ^[[%8d] ¢Š«ŽžŠÔ[%6d•b], Pos(%4d, %4d, %4d)\r\n"
	#define STRCMD_CS_COMMON_MAP_0006 "Tatal Monster Count:[%4d]<==ƒIƒuƒWƒFƒNƒgƒ‚ƒ“ƒXƒ^[‚ðŠÜ‚Þ\r\n"

	#define STRCMD_CS_COMMON_DOWNLOAD_0000 "ƒ_ƒEƒ“ƒ[ƒhƒtƒ@ƒCƒ‹‚ªŒ©‚Â‚©‚è‚Ü‚¹‚ñB"
	#define STRCMD_CS_COMMON_DOWNLOAD_0001 "ƒtƒ@ƒCƒ‹ì¬ƒGƒ‰[‚Å‚·B"
	#define STRCMD_CS_COMMON_DOWNLOAD_0002 "ƒ_ƒEƒ“ƒ[ƒhƒtƒ@ƒCƒ‹‚Ì“Ç‚Ýž‚ÝƒGƒ‰[‚Å‚·B"

	#define STRCMD_CS_COMMON_DATETIME_0000 "%d“ú%dŽžŠÔ%d•ª%d•b"

	#define STRCMD_CS_COMMON_RACE_NORMAL		"ˆê”Ê"
	#define STRCMD_CS_COMMON_RACE_BATTALUS		"ƒoƒ^ƒ‰ƒX"
	#define STRCMD_CS_COMMON_RACE_DECA			"ƒfƒJ"
	#define STRCMD_CS_COMMON_RACE_PHILON		"ƒpƒCƒƒ“"
	#define STRCMD_CS_COMMON_RACE_SHARRINE		"ƒVƒ…ƒŠƒ“"
	#define STRCMD_CS_COMMON_RACE_MONSTER1		"—\”õ"
	#define STRCMD_CS_COMMON_RACE_MONSTER2		"—\”õ"
	#define STRCMD_CS_COMMON_RACE_NPC			"NPC"
	#define STRCMD_CS_COMMON_RACE_OPERATION		"ŠÇ—ŽÒ"
	#define STRCMD_CS_COMMON_RACE_GAMEMASTER	"GM"
	#define STRCMD_CS_COMMON_RACE_MONITOR		"ƒ‚ƒjƒ^["
	#define STRCMD_CS_COMMON_RACE_GUEST			"ƒQƒXƒg"
	#define STRCMD_CS_COMMON_RACE_DEMO			"ƒfƒ‚—p"
	#define STRCMD_CS_COMMON_RACE_ALL			"‘SŽí‘°"
	#define STRCMD_CS_COMMON_RACE_UNKNOWN		"•s–¾‚ÈŽí‘°"

	#define STRCMD_CS_COMMON_MAPNAME_UNKNOWN	"–¼‘O‚È‚µ"

	#define STRCMD_CS_STATUS_BEGINNER_AIRMAN		"ƒgƒŒ[ƒjƒ“ƒOƒGƒA[ƒ}ƒ“"
	#define STRCMD_CS_STATUS_3RD_CLASS_AIRMAN		"3rdƒGƒA[ƒ}ƒ“"
	#define STRCMD_CS_STATUS_2ND_CLASS_AIRMAN		"2ndƒGƒA[ƒ}ƒ“"
	#define STRCMD_CS_STATUS_1ST_CLASS_AIRMAN		"1stƒGƒA[ƒ}ƒ“"
	#define STRCMD_CS_STATUS_3RD_CLASS_WINGMAN		"3rdƒEƒBƒ“ƒOƒ}ƒ“"
	#define STRCMD_CS_STATUS_2ND_CLASS_WINGMAN		"2ndƒEƒBƒ“ƒOƒ}ƒ“"
	#define STRCMD_CS_STATUS_1ST_CLASS_WINGMAN		"1stƒEƒBƒ“ƒOƒ}ƒ“"
	#define STRCMD_CS_STATUS_3RD_CLASS_LEADER		"3rdƒŠ[ƒ_["
	#define STRCMD_CS_STATUS_2ND_CLASS_LEADER		"2ndƒŠ[ƒ_["
	#define STRCMD_CS_STATUS_1ST_CLASS_LEADER		"1stƒŠ[ƒ_["
	#define STRCMD_CS_STATUS_3RD_CLASS_ACE			"3rdƒG[ƒX"
	#define STRCMD_CS_STATUS_2ND_CLASS_ACE			"2ndƒG[ƒX"
	#define STRCMD_CS_STATUS_1ST_CLASS_ACE			"1stƒG[ƒX"
	#define STRCMD_CS_STATUS_COPPER_CLASS_GENERAL	"y«"
	#define STRCMD_CS_STATUS_SILVER_CLASS_GENERAL	"­«"
	#define STRCMD_CS_STATUS_GOLD_CLASS_GENERAL		"’†«"
	#define STRCMD_CS_STATUS_MASTER_GENERAL			"‘å«"

	#define STRCMD_CS_ITEMKIND_AUTOMATIC			"ƒI[ƒgƒ}ƒ`ƒbƒN—Þ"
	#define STRCMD_CS_ITEMKIND_VULCAN				"ƒoƒ‹ƒJƒ“—Þ"
	#define STRCMD_CS_ITEMKIND_DUALIST				"ƒfƒ…ƒAƒ‹ƒŠƒXƒg—Þ"		// 2005-08-01 by hblee : GRENADE -> DUALIST ‚É •ÏX.
	#define STRCMD_CS_ITEMKIND_CANNON				"ƒLƒƒƒmƒ“—Þ"
	#define STRCMD_CS_ITEMKIND_RIFLE				"ƒ‰ƒCƒtƒ‹—Þ"
	#define STRCMD_CS_ITEMKIND_GATLING				"ƒKƒgƒŠƒ“ƒO—Þ"
	#define STRCMD_CS_ITEMKIND_LAUNCHER				"ƒ‰ƒ“ƒ`ƒƒ[—Þ"
	#define STRCMD_CS_ITEMKIND_MASSDRIVE			"ƒ}ƒXƒhƒ‰ƒCƒu—Þ"
	#define STRCMD_CS_ITEMKIND_ROCKET				"ƒƒPƒbƒg—Þ"
	#define STRCMD_CS_ITEMKIND_MISSILE				"ƒ~ƒTƒCƒ‹—Þ"
	#define STRCMD_CS_ITEMKIND_BUNDLE				"ƒoƒ“ƒhƒ‹—Þ"

	#define STRCMD_CS_ITEMKIND_MINE					"ƒ}ƒCƒ“—Þ"
	#define STRCMD_CS_ITEMKIND_SHIELD				"ƒV[ƒ‹ƒh—Þ"
	#define STRCMD_CS_ITEMKIND_DUMMY				"ƒ_ƒ~[—Þ"
	#define STRCMD_CS_ITEMKIND_FIXER				"ƒsƒNƒT[—Þ"
	#define STRCMD_CS_ITEMKIND_DECOY				"ƒfƒRƒC—Þ"
	#define STRCMD_CS_ITEMKIND_DEFENSE				"ƒA[ƒ}[—Þ"
	#define STRCMD_CS_ITEMKIND_SUPPORT				"ƒGƒ“ƒWƒ“—Þ"
	#define STRCMD_CS_ITEMKIND_ENERGY				"ƒGƒlƒ‹ƒM[—Þ"
	#define STRCMD_CS_ITEMKIND_INGOT				"zÎ—Þ"
	#define STRCMD_CS_ITEMKIND_CARD					"ˆê”ÊƒJ[ƒh—Þ"
	#define STRCMD_CS_ITEMKIND_ENCHANT				"ƒGƒ“ƒ`ƒƒƒ“ƒgƒJ[ƒh—Þ"
	#define STRCMD_CS_ITEMKIND_TANK					"ƒ^ƒ“ƒN—Þ"
	#define STRCMD_CS_ITEMKIND_BULLET				"’eŠÛ—Þ"
	#define STRCMD_CS_ITEMKIND_QUEST				"ƒ~ƒbƒVƒ‡ƒ“ƒAƒCƒeƒ€—Þ"
	#define STRCMD_CS_ITEMKIND_RADAR				"ƒŒ[ƒ_[—Þ"
	#define STRCMD_CS_ITEMKIND_COMPUTER				"ƒRƒ“ƒsƒ…[ƒ^[—Þ"
	#define STRCMD_CS_ITEMKIND_GAMBLE				"ƒMƒƒƒ“ƒuƒ‹ƒJ[ƒh—Þ"
	#define STRCMD_CS_ITEMKIND_PREVENTION_DELETE_ITEM	"ƒGƒ“ƒ`ƒƒƒ“ƒg”j‰ó–hŽ~ƒJ[ƒh—Þ"
	#define STRCMD_CS_ITEMKIND_BLASTER				"ƒuƒ‰ƒXƒ^[—Þ"	// 2005-08-01 by hblee : ƒuƒ‰ƒXƒ^[—Þ’Ç‰Á.
	#define STRCMD_CS_ITEMKIND_RAILGUN				"ƒŒ[ƒ‹ƒKƒ“—Þ"		// 2005-08-01 by hblee : ƒŒ[ƒ‹ƒKƒ“—Þ’Ç‰Á.
	#define STRCMD_CS_ITEMKIND_ACCESSORY_UNLIMITED	"–³§ŒÀƒAƒNƒZƒTƒŠ"		// 2006-03-17 by cmkwon, Žg—pŽžŠÔ‚ª <‰i‹v>‚Ì ƒAƒNƒZƒTƒŠ ƒAƒCƒeƒ€
	#define STRCMD_CS_ITEMKIND_ACCESSORY_TIMELIMIT	"ŽžŠÔ§ŒÀƒAƒNƒZƒTƒŠ"		// 2006-03-17 by cmkwon, ŽžŠÔ§ŒÀ‚Ì‚ ‚éƒAƒNƒZƒTƒŠƒAƒCƒeƒ€
	#define STRCMD_CS_ITEMKIND_ALL_WEAPON			"‘S•Ší‘S•Ší"
	#define STRCMD_CS_ITEMKIND_PRIMARY_WEAPON_ALL	"ƒƒCƒ“•Ší"
	#define STRCMD_CS_ITEMKIND_PRIMARY_WEAPON_1		"e’eŒ^ƒƒCƒ“•Ší"
	#define STRCMD_CS_ITEMKIND_PRIMARY_WEAPON_2		"”R—¿Œ^ƒƒCƒ“•Ší"
	#define STRCMD_CS_ITEMKIND_SECONDARY_WEAPON_ALL	"ƒTƒu•Ší"
	#define STRCMD_CS_ITEMKIND_SECONDARY_WEAPON_1	"’e“ªŒ^ƒTƒu•Ší"
	#define STRCMD_CS_ITEMKIND_SECONDARY_WEAPON_2	"–hŒäŒ^ƒTƒu•Ší"
	#define STRCMD_CS_ITEMKIND_SKILL_ATTACK			"UŒ‚ƒXƒLƒ‹"
	#define STRCMD_CS_ITEMKIND_SKILL_DEFENSE		"–hŒäƒXƒLƒ‹"
	#define STRCMD_CS_ITEMKIND_SKILL_SUPPORT		"•â•ƒXƒLƒ‹"
	#define STRCMD_CS_ITEMKIND_SKILL_ATTRIBUTE		"‘®«ƒXƒLƒ‹"
	#define STRCMD_CS_ITEMKIND_FOR_MON_PRIMARY		"1Œ^ƒ‚ƒ“ƒXƒ^[—pƒAƒCƒeƒ€"
	#define STRCMD_CS_ITEMKIND_FOR_MON_GUN			"ƒ‚ƒ“ƒXƒ^[ƒ}ƒVƒ“ƒKƒ“(1-1Œ^)"
	#define STRCMD_CS_ITEMKIND_FOR_MON_BEAM			"ƒ‚ƒ“ƒXƒ^[ƒr[ƒ€—Þ(1-2Œ^)"
	#define STRCMD_CS_ITEMKIND_FOR_MON_ALLATTACK	"ƒ‚ƒ“ƒXƒ^[‘S‘ÌUŒ‚"
	#define STRCMD_CS_ITEMKIND_FOR_MON_SECONDARY	"2Œ^ƒ‚ƒ“ƒXƒ^[—pƒAƒCƒeƒ€"
	#define STRCMD_CS_ITEMKIND_FOR_MON_ROCKET		"ƒ‚ƒ“ƒXƒ^[ƒƒPƒbƒg(2-1Œ^)"
	#define STRCMD_CS_ITEMKIND_FOR_MON_MISSILE		"ƒ‚ƒ“ƒXƒ^[ƒ~ƒTƒCƒ‹—Þ(2-1Œ^)"
	#define STRCMD_CS_ITEMKIND_FOR_MON_BUNDLE		"ƒ‚ƒ“ƒXƒ^[ƒoƒ“ƒhƒ‹—Þ(2-1Œ^)"
	#define STRCMD_CS_ITEMKIND_FOR_MON_MINE			"ƒ‚ƒ“ƒXƒ^[ƒ}ƒCƒ“—Þ(2-1Œ^)"
	#define STRCMD_CS_ITEMKIND_FOR_MON_SHIELD		"ƒ‚ƒ“ƒXƒ^[ƒV[ƒ‹ƒh—Þ(2-2Œ^)"
	#define STRCMD_CS_ITEMKIND_FOR_MON_DUMMY		"ƒ‚ƒ“ƒXƒ^[ƒ_ƒ~[—Þ(2-2Œ^)"
	#define STRCMD_CS_ITEMKIND_FOR_MON_FIXER		"ƒ‚ƒ“ƒXƒ^[ƒsƒNƒT[—Þ(2-2Œ^)"
	#define STRCMD_CS_ITEMKIND_FOR_MON_DECOY		"ƒ‚ƒ“ƒXƒ^[ƒfƒRƒC—Þ(2-2Œ^)"
	#define STRCMD_CS_ITEMKIND_FOR_MON_FIRE			"ƒ‚ƒ“ƒXƒ^[ƒtƒ@ƒCƒ„[[—Þ"
	#define STRCMD_CS_ITEMKIND_FOR_MON_OBJBEAM		"ƒ‚ƒ“ƒXƒ^[Õ“Ë‰Â”\ƒr[ƒ€—Þ"
	#define STRCMD_CS_ITEMKIND_FOR_MON_STRAIGHTBOOM	"ƒ‚ƒ“ƒXƒ^[’¼i”š’e—Þ"
	#define STRCMD_CS_ITEMKIND_UNKNOWN				"•s–¾‚ÈƒAƒCƒeƒ€"

	#define STRCMD_CS_UNITKIND_UNKNOWN				"•s–¾‚È‹@‘Ì"

	#define STRCMD_CS_STAT_ATTACK_PART				"UŒ‚"
	#define STRCMD_CS_STAT_DEFENSE_PART				"–hŒä"
	#define STRCMD_CS_STAT_FUEL_PART				"”R—¿"
	#define STRCMD_CS_STAT_SOUL_PART				"¸_"
	#define STRCMD_CS_STAT_SHIELD_PART				"ƒV[ƒ‹ƒh"
	#define STRCMD_CS_STAT_DODGE_PART				"‰ñ”ð"
	#define STRCMD_CS_STAT_BONUS					"ƒ{[ƒiƒXƒXƒe[ƒ^ƒX"
	#define STRCMD_CS_STAT_ALL_PART					"‘SƒXƒe[ƒ^ƒX"
	#define STRCMD_CS_STAT_UNKNOWN					"•s–¾‚ÈƒXƒe[ƒ^ƒX"

	#define STRCMD_CS_AUTOSTAT_TYPE_FREESTYLE		"Ž©—RŒ^"
	#define STRCMD_CS_AUTOSTAT_TYPE_BGEAR_ATTACK	"UŒ‚Œ^"
	#define STRCMD_CS_AUTOSTAT_TYPE_BGEAR_MULTI		"ƒ}ƒ‹ƒ`Œ^"
	#define STRCMD_CS_AUTOSTAT_TYPE_IGEAR_ATTACK	"UŒ‚Œ^"
	#define STRCMD_CS_AUTOSTAT_TYPE_IGEAR_DODGE		"‰ñ”ðŒ^"
	#define STRCMD_CS_AUTOSTAT_TYPE_AGEAR_ATTACK	"UŒ‚Œ^"
	#define STRCMD_CS_AUTOSTAT_TYPE_AGEAR_SHIELD	"ƒV[ƒ‹ƒhŒ^"
	#define STRCMD_CS_AUTOSTAT_TYPE_MGEAR_DEFENSE	"–hŒäŒ^"
	#define STRCMD_CS_AUTOSTAT_TYPE_MGEAR_SUPPORT	"Žx‰‡Œ^"
	#define STRCMD_CS_AUTOSTAT_TYPE_UNKNOWN			"UNKNOWN_AUTOSTAT_TYPE"

// 2007-10-30 by cmkwon, ¨—Í? HappyHour ƒCƒxƒ“ƒg ?? - ???? ?? ?? ?
//	#define STRCMD_CS_INFLUENCE_TYPE_NORMAL			"ƒoƒCƒWƒFƒjƒ…[ˆê”ÊŒR"			// 2005-12-20 by cmkwon
//	#define STRCMD_CS_INFLUENCE_TYPE_VCN			"ƒoƒCƒWƒFƒjƒ…[³‹KŒR"
//	#define STRCMD_CS_INFLUENCE_TYPE_ANI			"ƒA[ƒŠƒ“ƒgƒ“³‹KŒR"
	#define STRCMD_CS_INFLUENCE_TYPE_RRP			"ƒoƒ^ƒ‰ƒX˜A–MŒR"

	#define STRCMD_CS_POS_PROW						"ƒŒ[ƒ_[ˆÊ’u(æ“ª‚Ì’†‰›)"
	#define STRCMD_CS_POS_PROWIN					"ƒRƒ“ƒsƒ…[ƒ^[(’†‰›‚Ì¶)"
	#define STRCMD_CS_POS_PROWOUT					"ƒƒCƒ“•Ší(æ“ª‚Ì¶)"
	#define STRCMD_CS_POS_WINGIN					"Žg—p‚µ‚È‚¢(’†‰›‚Ì‰E)"
	#define STRCMD_CS_POS_WINGOUT					"ƒTƒu•Ší(æ“ª‚Ì‰E)"
	#define STRCMD_CS_POS_CENTER					"ƒA[ƒ}[(’†‰›‚Ì’†‰›)"
	#define STRCMD_CS_POS_REAR						"ƒGƒ“ƒWƒ“(Œã”ö‚Ì’†‰›)"
	    
	// 2010-06-15 by shcho&hslee Æê½Ã½ºÅÛ
    //#define STRCMD_CS_POS_ATTACHMENT				"•t’…•¨(Œã”ö‚Ì‰E-”R—¿ƒ^ƒ“ƒN|ƒhƒ‰ƒCƒo[Œn—ñ)"
	#define STRCMD_CS_POS_ACCESSORY_UNLIMITED		"•t’…•¨(Œã”ö‚Ì‰E-”R—¿ƒ^ƒ“ƒN|ƒhƒ‰ƒCƒo[Œn—ñ)"

	// 2010-06-15 by shcho&hslee Æê½Ã½ºÅÛ
	//#define STRCMD_CS_POS_PET						"»ç¿ë¾ÈÇÔ(ÈÄ¹Ì ÁÂÃø)"
	#define STRCMD_CS_POS_ACCESSORY_TIME_LIMIT		"»ç¿ë¾ÈÇÔ(ÈÄ¹Ì ÁÂÃø)"

	#define STRCMD_CS_POS_PET						"ÆÄÆ®³Ê"

	#define STRCMD_CS_HIDDEN_ITEM					"¼û°ÜÁø À§Ä¡"		// 2011-09-20 by hskim, ÆÄÆ®³Ê ½Ã½ºÅÛ 2Â÷ - ¼û°ÜÁø ¾ÆÀÌÅÛ

	#define STRCMD_CS_POS_INVALID_POSITION			"¹ÌÈ®Á¤ À§Ä¡"
	#define STRCMD_CS_POS_ITEMWINDOW_OFFSET			"ÀÎº¥Åä¸® À§Ä¡"

	// 2005-12-07 by cmkwon
	#define STRCMD_CS_COMMAND_QUESTCOMPLETION_0		"/ƒ~ƒbƒVƒ‡ƒ“Š®—¹"
	#define STRCMD_CS_COMMAND_QUESTCOMPLETION_1		"/QuestCom"
	#define STRCMD_CS_COMMAND_QUESTCOMPLETION_HELP	"format: /ƒ~ƒbƒVƒ‡ƒ“Š®—¹[|QuesIndex] - is’†ƒ~ƒbƒVƒ‡ƒ“‚âŽw’è‚µ‚½ƒ~ƒbƒVƒ‡ƒ“‚ªŠ®—¹ˆ—"

	// 2006-02-08 by cmkwon
	#define STRCMD_CS_COMMAND_INFLDITRIBUTION_0		"/¨—Í•ª•z"
	#define STRCMD_CS_COMMAND_INFLDITRIBUTION_1		"/InflDist"
	#define STRCMD_CS_COMMAND_INFLDITRIBUTION_HELP	"format: /¨—Í•ª•z"
	#define STRCMD_CS_COMMAND_CHANGEINFL_0			"/¨—Í•ÏX"
	#define STRCMD_CS_COMMAND_CHANGEINFL_1			"/InflChange"
	#define STRCMD_CS_COMMAND_CHANGEINFL_HELP		"format:/¨—Í•ÏX [|1(Normal)|2(BCU)|4(ANI)]"

	// 2006-03-02 by cmkwon
	#define STRCMD_CS_COMMAND_GOMONSTER_0			"/ƒ‚ƒ“ƒXƒ^[‚És‚­"
	#define STRCMD_CS_COMMAND_GOMONSTER_1			"/GoMonster"
	#define STRCMD_CS_COMMAND_GOMONSTER_HELP		"format:/ƒ‚ƒ“ƒXƒ^[‚És‚­ [MonsterName|MonsterNumber]"

	//////////////////////////////////////////////////////////////////////////
	// 2008-05-20 by dhjin, EP3 - —·’c ?? ?? - ?? ?? ??? ˆÚ“®
	// 2006-03-07 by cmkwon
//	#define STRCMD_CS_GUILD_RANK_PRIVATE_NULL		"‘àˆõ"
//	#define STRCMD_CS_GUILD_RANK_COMMANDER			"—·’c’·"
//	#define STRCMD_CS_GUILD_RANK_SQUAD_LEADER_1		"1‘å‘à’·"
//	#define STRCMD_CS_GUILD_RANK_PRIVATE_1			"1‘å‘àˆõ"
//	#define STRCMD_CS_GUILD_RANK_SQUAD_LEADER_2		"2‘å‘à’·"
//	#define STRCMD_CS_GUILD_RANK_PRIVATE_2			"2‘å‘àˆõ"
//	#define STRCMD_CS_GUILD_RANK_SQUAD_LEADER_3		"3‘å‘à’·"
//	#define STRCMD_CS_GUILD_RANK_PRIVATE_3			"3‘å‘àˆõ"
//	#define STRCMD_CS_GUILD_RANK_SQUAD_LEADER_4		"4‘å‘à’·"
//	#define STRCMD_CS_GUILD_RANK_PRIVATE_4			"4‘å‘àˆõ"
//	#define STRCMD_CS_GUILD_RANK_SQUAD_LEADER_5		"5‘å‘à’·"
//	#define STRCMD_CS_GUILD_RANK_PRIVATE_5			"5‘å‘àˆõ"

	// 2006-04-17 by cmkwon
	#define STRCMD_CS_COMMAND_SIGNBOARD_0			"/“dŒõ”Â"
	#define STRCMD_CS_COMMAND_SIGNBOARD_1			"/Noticeboard"
	#define STRCMD_CS_COMMAND_SIGNBOARD_HELP		"format:/“dŒõ”Â[|Ž‘±ŽžŠÔ(’PˆÊ:•ª)] [‚¨’m‚ç‚¹“à—e] | Žw“±ŽÒ‚Ì‚ÝŽg—p‰Â”\A“dŒõ”Â‚É‚¨’m‚ç‚¹‚ð’Ç‰Á‚·‚é‚©ƒŠƒXƒg‚ðŒ©‚é"
	#define STRCMD_CS_COMMAND_SIGNBOARD_DEL_0		"/“dŒõ”Âíœ"
	#define STRCMD_CS_COMMAND_SIGNBOARD_DEL_1		"/DeleteNoticeboard"
	#define STRCMD_CS_COMMAND_SIGNBOARD_DEL_HELP	"format: /“dŒõ”Âíœ[íœ‚·‚é‚¨’m‚ç‚¹‚ÌƒCƒ“ƒfƒbƒNƒX] | Žw“±ŽÒ‚Ì‚ÝŽg—p‰Â”\A“dŒõ”Â‚ÌŽw’è‚µ‚½‚¨’m‚ç‚¹‚ðíœ‚·‚é"

	// 2006-04-20 by cmkwon
	#define STRCMD_CS_COMMON_RACE_INFLUENCE_LEADER	"¨—ÍíƒŠ[ƒ_["
	#define STRCMD_CS_COMMON_RACE_INFLUENCE_SUBLEADER	"¨—Íí•›ƒŠ[ƒ_["
	// 2006-04-21 by cmkwon
	#define STRCMD_CS_ITEMKIND_INFLUENCE_BUFF		"¨—ÍŽx‰‡Œø‰Ê"
	#define STRCMD_CS_ITEMKIND_INFLUENCE_GAMEEVENT	"¨—ÍƒCƒxƒ“ƒg"

	// 2006-04-24 by cmkwon
	#define STRCMD_CS_COMMAND_CONPOINT_0			"/Šñ—^“x"
	#define STRCMD_CS_COMMAND_CONPOINT_1			"/ContributionPoint"
	#define STRCMD_CS_COMMAND_CONPOINT_HELP			"format: /Šñ—^“x [¨—Í(2:BCU, 4:ANI)] [‘‰Á•ª] | Žw’è‚µ‚½¨—Í‚ÌŠñ—^“x‚ð‘‰Á‚³‚¹‚é"

	// 2006-05-08 by cmkwon
	#define STRCMD_CS_COMMAND_CALLGM_0				"/CallGM"
	#define STRCMD_CS_COMMAND_CALLGM_1				"/ƒwƒ‹ƒv"
	#define STRCMD_CS_COMMAND_CALLGM_2				"/ƒwƒ‹ƒv"
	#define STRCMD_CS_COMMAND_CALLGM_HELP			"format: /ƒwƒ‹ƒv [‘Š’k“à—e] | GM‚É‘Š’k‚ð\‚µž‚Ý‚·‚éB"
	#define STRCMD_CS_COMMAND_VIEWCALLGM_0			"/ViewCallGM"
	#define STRCMD_CS_COMMAND_VIEWCALLGM_1			"/ƒwƒ‹ƒv‚ðŒ©‚é"
	#define STRCMD_CS_COMMAND_VIEWCALLGM_2			"/ƒwƒ‹ƒv‚ðŒ©‚é"
	#define STRCMD_CS_COMMAND_VIEWCALLGM_HELP		"format: /ƒwƒ‹ƒv‚ðŒ©‚é[|Œ”(1`10)] | GM‘Š’k\‚µž‚ÝƒŠƒXƒg‚ðŽw’è‚µ‚½Œ”‚ðo—Í‚·‚é"
	#define STRCMD_CS_COMMAND_BRINGCALLGM_0			"/BringCallGM"
	#define STRCMD_CS_COMMAND_BRINGCALLGM_1			"/ƒwƒ‹ƒv‚ð“Ç‚Ýž‚Þ"
	#define STRCMD_CS_COMMAND_BRINGCALLGM_2			"/ƒwƒ‹ƒv‚ð“Ç‚Ýž‚Þ"
	#define STRCMD_CS_COMMAND_BRINGCALLGM_HELP		"format: /ƒwƒ‹ƒv‚ð“Ç‚Ýž‚Þ[|Œ”(1`10)] | GM‘Š’k\‚µž‚ÝƒŠƒXƒg‚ðŽw’è‚µ‚½Œ”“Ç‚Ýž‚Þ (ƒT[ƒo‚©‚çíœ‚³‚ê‚é)"

	// 2006-07-18 by cmkwon
	#define STRCMD_CS_COMMAND_COMEONINFL_0			"/ComeOnInfl"
	#define STRCMD_CS_COMMAND_COMEONINFL_1			"/¨—Í¢Š«"
	#define STRCMD_CS_COMMAND_COMEONINFL_2			"/¨—Í¢Š«"
// 2008-09-09 by cmkwon, /¨—Í¢Š« –½—ßŒê ?? ƒŠƒXƒg? ??ƒ^ƒCƒv ’Ç‰Á - commented
//	#define STRCMD_CS_COMMAND_COMEONINFL_HELP		"format: /ComeOnInfl [1(Normal)|2(VCN)|4(ANI)|255(All)] [Å‘ålˆõ] [0|Å¬ƒŒƒxƒ‹] [0|Å‘åƒŒƒxƒ‹] [ƒ†[ƒU[‚É‘—‚éƒƒbƒZ[ƒW] | ”CˆÓ‚ÉŽw’è‚µ‚½¨—ÍAŽw’è‚µ‚½ƒŒƒxƒ‹‚Ìƒ†[ƒU[‚ÉƒCƒxƒ“ƒgƒ}ƒbƒv‚Ö‚ÌˆÚ“®‚ð—v¿‚·‚é"

	// 2006-07-24 by cmkwon
	#define STRCMD_CS_COMMAND_ITEMINMAP_0			"/InsertItemInMap"
	#define STRCMD_CS_COMMAND_ITEMINMAP_1			"/ƒAƒCƒeƒ€ƒ}ƒbƒv"
	#define STRCMD_CS_COMMAND_ITEMINMAP_2			"/ƒAƒCƒeƒ€’Ç‰Áƒ}ƒbƒv"
	#define STRCMD_CS_COMMAND_ITEMINMAP_HELP		"format: /InsertItemInMap [1(Normal)|2(VCN)|4(ANI)|255(All)] [Item Number] [# of items] - Œ»Ýƒ}ƒbƒv‚ÌŽw’è‚µ‚½¨—Í‚Ìƒ†[ƒU[‚ÉŽw’è‚µ‚½ƒAƒCƒeƒ€‚ðŽx‹‹‚·‚é"

	// 2006-07-28 by cmkwon
	#define STRCMD_CS_ITEMKIND_COLOR_ITEM			"Color Item"

	// 2006-08-03 by cmkwon, ??? ?? ?? ??? ???
	// ??(Korea):		YYYY-MM-DD HH:MM:SS
	// ??(English):	MM-DD-YYYY HH:MM:SS
	// ???(Vietnam):	DD-MM-YYYY HH:MM:SS
	#define NATIONAL_ATUM_DATE_TIME_STRING_FORMAT(Y, M, D, h, m, s)				"%04d-%02d-%02d %02d:%02d:%02d", Y, M, D, h, m, s
	#define NATIONAL_ATUM_DATE_TIME_STRING_FORMAT_EXCLUDE_SECOND(Y, M, D, h, m)	"%04d-%02d-%02d %02d:%02d", Y, M, D, h, m

	// 2006-08-08 by dhjin, ƒŒƒxƒ‹•ª•z
	#define STRCMD_CS_COMMAND_DISTRIBUTIONLEVEL_0		"/LevelDistribution"		// 2006-08-08 by dhjin
	#define STRCMD_CS_COMMAND_DISTRIBUTIONLEVEL_1		"/ƒŒƒxƒ‹•ª•z"					// 2006-08-08 by dhjin
	#define STRCMD_CS_COMMAND_DISTRIBUTIONLEVEL_HELP	"format: /ƒŒƒxƒ‹•ª•z - ƒQ[ƒ€‚ÉÚ‘±‚µ‚Ä‚¢‚éƒ†[ƒU[‚ÌƒŒƒxƒ‹•ª•z‚ð‚Ý‚é"	// 2006-08-08 by dhjin

	// 2006-08-10 by cmkwon
	#define STRCMD_CS_ITEMKIND_RANDOMBOX				"K‰^‚Ì” "

	// 2006-08-21 by cmkwon
	#define STRCMD_CS_ITEMKIND_MARK						"ƒ}[ƒN"

///////////////////////////////////////////////////////////////////////////////
// 2006-08-24 by cmkwon
// ƒNƒ‰ƒCƒAƒ“ƒg??? ???? –½—ßŒê(Just command for client)
	#define STRCMD_C_COMMAND_CALL						"/ƒ{ƒCƒXƒ`ƒƒƒbƒg"
	#define STRCMD_C_COMMAND_CALL_HELP					"format: /ƒ{ƒCƒXƒ`ƒƒƒbƒg [CharacterName] | Žw’è‚µ‚½ƒLƒƒƒ‰ƒNƒ^[‚É 1:1 ƒ{ƒCƒXƒ{ƒCƒXƒ`ƒƒƒbƒg‚ð —v¿‚·‚é"
	#define STRCMD_C_COMMAND_PARTYCALL					"/•Ò‘àƒ{ƒCƒXƒ`ƒƒƒbƒg"
	#define STRCMD_C_COMMAND_PARTYCALL_HELP				"format: /•Ò‘àƒ{ƒCƒXƒ`ƒƒƒbƒg - •Ò‘à’·‚Ì‚ÝŽg—p‰Â”\B•Ò‘àƒ{ƒCƒXƒ{ƒCƒXƒ`ƒƒƒbƒg‚ðŠJŽn‚·‚é"
	#define STRCMD_C_COMMAND_PARTYCALLEND				"/•Ò‘àƒ{ƒCƒXƒ`ƒƒƒbƒgI—¹"
	#define STRCMD_C_COMMAND_PARTYCALLEND_HELP			"foramt: /•Ò‘àƒ{ƒCƒXƒ`ƒƒƒbƒgI—¹ - •Ò‘à’·‚Ì‚ÝŽg—p‰Â”\B•Ò‘àƒ{ƒCƒXƒ{ƒCƒXƒ`ƒƒƒbƒg‚ðI—¹‚·‚é"
	#define STRCMD_C_COMMAND_GUILDCALL					"/—·’cƒ{ƒCƒXƒ`ƒƒƒbƒg"
	#define STRCMD_C_COMMAND_GUILDCALL_HELP				"format: /—·’cƒ{ƒCƒXƒ`ƒƒƒbƒg - —·’c’·‚Ì‚ÝŽg—p‰Â”\A—·’cƒ{ƒCƒXƒ`ƒƒƒbƒg‚ðŠJŽn‚·‚éB"
	#define STRCMD_C_COMMAND_GUILDCALLEND				"/—·’cƒ{ƒCƒXƒ`ƒƒƒbƒgI—¹"
	#define STRCMD_C_COMMAND_GUILDCALLEND_HELP			"format: /—·’cƒ{ƒCƒXƒ`ƒƒƒbƒgI—¹ - —·’c’·‚Ì‚ÝŽg—p‰Â”\A—·’cƒ{ƒCƒXƒ{ƒCƒXƒ`ƒƒƒbƒg‚ðI—¹‚·‚éB"
	#define STRCMD_C_COMMAND_CALLEND					"/ƒ{ƒCƒXƒ`ƒƒƒbƒgI—¹"
	#define STRCMD_C_COMMAND_CALLEND_HELP				"format: /ƒ{ƒCƒXƒ`ƒƒƒbƒgI—¹ - 1:1 ƒ{ƒCƒXƒ{ƒCƒXƒ`ƒƒƒbƒg‚â•Ò‘àƒ{ƒCƒXƒ`ƒƒƒbƒg‚â—·’cƒ{ƒCƒXƒ`ƒƒƒbƒg‚ðI—¹‚·‚éB"
	#define STRCMD_C_COMMAND_COMBAT						"/‘ÎŒˆ"
	#define STRCMD_C_COMMAND_BATTLE						"/Œˆ“¬"
	#define STRCMD_C_COMMAND_BATTLE_HELP				"format: /Œˆ“¬ [CharacterName] | Žw’è‚µ‚½ƒLƒƒƒ‰ƒNƒ^[‚É1:1 Œˆ“¬‚ð\‚µž‚Ý‚·‚éB"
	#define STRCMD_C_COMMAND_SURRENDER					"/ƒMƒuƒAƒbƒv"
	#define STRCMD_C_COMMAND_SURRENDER_HELP				"format: /ƒMƒuƒAƒbƒv [CharacterName] | Žw’è‚µ‚½ƒLƒƒƒ‰ƒNƒ^[‚Æ1:1 Œˆ“¬Žž‚ÉŽ©•ª‚ªƒMƒuƒAƒbƒv‚ð\‚µž‚Ý‚·‚éB"
	#define STRCMD_C_COMMAND_PARTYBATTLE				"/•Ò‘àŒˆ“¬"
	#define STRCMD_C_COMMAND_PARTYBATTLE_HELP			"format: /•Ò‘àŒˆ“¬ [CharacterName] - •Ò‘à’·‚Ì‚ÝŽg—p‰Â”\BŽw’è‚µ‚½ƒLƒƒƒ‰ƒNƒ^[ (•Ò‘à’·)‚É•Ò‘àŒˆ“¬‚ð\‚µž‚Ý‚·‚éB"
	#define STRCMD_C_COMMAND_PARTYCOMBAT				"/•Ò‘à‘ÎŒˆ"
	#define STRCMD_C_COMMAND_PARTYWAR					"/•Ò‘àí“¬"
	#define STRCMD_C_COMMAND_GUILDBATTLE				"/—·’cí“¬"
	#define STRCMD_C_COMMAND_GUILDCOMBAT				"/—·’c‘ÎŒˆ"
	#define STRCMD_C_COMMAND_GUILDCOMBAT_HELP			"format: /—·’c‘ÎŒˆ [CharacterName] |—·’c’·‚Ì‚ÝŽg—p‰Â”\BŽw’è‚µ‚½ƒLƒƒƒ‰ƒNƒ^[ (—·’c’·)‚É—·’cŒˆ“¬‚ð\‚µž‚Ý‚·‚éB"
	#define STRCMD_C_COMMAND_GUILDWAR					"/—·’cí‘ˆ"
	#define STRCMD_C_COMMAND_GUILDSURRENDER				"/—·’cíƒMƒuƒAƒbƒv"
	#define STRCMD_C_COMMAND_GUILDSURRENDER_HELP		"format: /—·’cíƒMƒuƒAƒbƒv - —·’c’·‚Ì‚ÝŽg—p‰Â”\B—·’c‘ÎŒˆisŽžAƒMƒuƒAƒbƒv‚·‚éB"
	#define STRCMD_C_COMMAND_NAME						"/ŒÄÌ"
	#define STRCMD_C_COMMAND_NAME_HELP					"format: /ŒÄÌ [CharacterName] [ŠK‹‰(2 ` 11)] - —·’c’·‚Ì‚ÝŽg—p‰Â”\BŽw’è‚µ‚½ƒLƒƒƒ‰ƒNƒ^[‚ðŽw’è‚µ‚½ŠK‹‰‚É•ÏX‚·‚é"
	#define STRCMD_C_COMMAND_WARP						"/ƒ[ƒv"
	#define STRCMD_C_COMMAND_CANCELSKILL				"/ƒXƒLƒ‹ƒLƒƒƒ“ƒZƒ‹"
	#define STRCMD_C_COMMAND_INITCHAT					"/ƒ`ƒƒƒbƒgƒEƒBƒ“ƒhƒE‰Šú‰»"
	#define STRCMD_C_COMMAND_INITCHAT_HELP				"format: /ƒ`ƒƒƒbƒgƒEƒBƒ“ƒhƒE‰Šú‰» - ƒ`ƒƒƒbƒgƒEƒBƒ“ƒhƒE‚ð ‰Šú‰» ‚·‚é"
	#define STRCMD_C_COMMAND_REFUSEBATTLE				"/Œˆ“¬ƒuƒƒbƒN"
	#define STRCMD_C_COMMAND_REFUSEBATTLE_HELP			"format: /Œˆ“¬ƒuƒƒbƒN - 1:1 Œˆ“¬ƒuƒƒbƒN Ý’è‚ð On/Off ‚·‚éB"
	#define STRCMD_C_COMMAND_REFUSETRADE				"/Žæ‚èˆø‚«ƒuƒƒbƒN"
	#define STRCMD_C_COMMAND_REFUSETRADE_HELP			"format: /Žæ‚èˆø‚«ƒuƒƒbƒN - Žæ‚èˆø‚«ƒuƒƒbƒN Ý’è‚ð On/Off ‚·‚éB"
	#define STRMSG_C_050810_0001						"/windowclose"
	#define STRMSG_C_050810_0001_HELP					"format: /windowclose | “Á’è ƒƒbƒZ[ƒWƒ{ƒbƒNƒX‚ðŠJ‚©‚È‚¢‚æ‚¤‚É‚·‚éBŽ©“®ƒLƒƒƒ“ƒZƒ‹‚³‚ê‚éB"
	#define STRMSG_C_050810_0002						"/windowopen"
	#define STRMSG_C_050810_0002_HELP					"format: /windowopen | ‘SƒƒbƒZ[ƒWƒ{ƒbƒNƒX‚ªŽg—p‚Å‚«‚éB"

// 2006-09-29 by cmkwon	
#define STRCMD_CS_ITEMKIND_SKILL_SUPPORT_ITEM			"•â•ƒXƒLƒ‹ƒAƒCƒeƒ€"

// 2010-06-15 by shcho&hslee Æê½Ã½ºÅÛ - Æê ¾ÆÀÌÅÛ.
#define STRCMD_CS_ITEMKIND_PET_ITEM						"ƒp[ƒgƒiƒAƒCƒeƒ€"
#define STRCMD_CS_ITEMKIND_PET_SOCKET_ITEM				"ÆÄÆ®³Ê ¼ÒÄÏ ¾ÆÀÌÅÛ"		// 2011-09-01 by hskim, ÆÄÆ®³Ê ½Ã½ºÅÛ 2Â÷

// 2006-11-17 by cmkwon, ??? ?? ƒQ[ƒ€ ?? ??
#define STRCMD_CS_COMMAND_TIMELIMITSYSTEM_0			"/TimeLimitSyste"
#define STRCMD_CS_COMMAND_TIMELIMITSYSTEM_1			"/ŽžŠÔ§ŒÀƒVƒXƒeƒ€"
#define STRCMD_CS_COMMAND_TIMELIMITSYSTEM_P2ON		"on"
#define STRCMD_CS_COMMAND_TIMELIMITSYSTEM_P2OFF		"off"
#define STRCMD_CS_COMMAND_TIMELIMITSYSTEM_HELP		"format: /ŽžŠÔ§ŒÀƒVƒXƒeƒ€ [on|off] | ŽžŠÔ§ŒÀƒVƒXƒeƒ€ on/off‚ð Ý’è‚·‚éB"
#define STRCMD_CS_COMMAND_PLAYTIME_0				"/PlayTime"
#define STRCMD_CS_COMMAND_PLAYTIME_1				"/—˜—pŽžŠÔ"
#define STRCMD_CS_COMMAND_PLAYTIME_HELP				"format: /—˜—pŽžŠÔ | ¡“úˆê“ú‚ÌƒQ[ƒ€Ú‘±ŽžŠÔ‚ð•\Ž¦‚·‚é"

// 2007-10-06 by cmkwon, •›Žw“±ŽÒ 2?? ??‚ð ??? Ý’è - ??? ¨—Í?? ??? ???
//// 2006-12-13 by cmkwon	
//#define STRCMD_CS_COMMON_INFLUENCE_LEADER			"Žw“±ŽÒ"
//#define STRCMD_CS_COMMON_INFLUENCE_SUBLEADER		"•›Žw“±ŽÒ"

// 2007-01-08 by dhjin	
#define STRCMD_CS_COMMAND_BONUSSTAT_POINT_0			"/BonusStatPoint"
#define STRCMD_CS_COMMAND_BONUSSTAT_POINT_1			"/ƒ{[ƒiƒXƒXƒe[ƒ^ƒXƒ|ƒCƒ“ƒg"
#define STRCMD_CS_COMMAND_BONUSSTAT_POINT_2			"/ƒ{[ƒiƒXƒXƒe[ƒ^ƒXƒ|ƒCƒ“ƒg"
#define STRCMD_CS_COMMAND_BONUSSTAT_POINT_HELP		"format: /BonusStatPoint [BonusStatPoint Counts] [|character name] - BonusStatPoint‚ð DB‚É UPDATE"

// 2007-01-25 by dhjin	
#define STRCMD_CS_COMMAND_PCBANGUSERCOUNT_0			"/ƒlƒbƒgƒJƒtƒF"
#define STRCMD_CS_COMMAND_PCBANGUSERCOUNT_1			"/ƒlƒbƒgƒJƒtƒF"
#define STRCMD_CS_COMMAND_PCBANGUSERCOUNT_HELP		"format: /ƒlƒbƒgƒJƒtƒF - Œ»ÝÚ‘±‚µ‚Ä‚¢‚éƒlƒbƒgƒJƒtƒF‚Ìƒ†[ƒU[”‚ð•\Ž¦‚·‚é"

// 2007-10-06 by dhjin, •›Žw“±ŽÒ ?? ?? •ÏX?? ??
// 2007-02-13 by dhjin, •›Žw“±ŽÒ
//#define STRCMD_CS_COMMAND_SUBLEADER_0				"/Subleader"
//#define STRCMD_CS_COMMAND_SUBLEADER_1				"/•›Žw“±ŽÒ"
//#define STRCMD_CS_COMMAND_SUBLEADER_HELP			"format: /•›Žw“±ŽÒ [CharacterName] - •›Žw“±ŽÒ Ý’è"
//#define STRCMD_CS_COMMAND_SUBLEADER_RESULT_ERROR	"•›Žw“±ŽÒ Ý’è‚ª ƒLƒƒƒ“ƒZƒ‹ ‚³‚ê‚Ü‚µ‚½B"
//#define STRCMD_CS_COMMAND_SUBLEADER_RESULT_0		"•›Žw“±ŽÒ‚ð ‚±‚êˆÈãÝ’è‚·‚é‚±‚Æ‚ª‚Å‚«‚Ü‚¹‚ñB"
//#define STRCMD_CS_COMMAND_SUBLEADER_RESULT_1		"%s‚ð 1”Ô–Ú‚Ì•›Žw“±ŽÒ‚É Ý’è‚µ‚Ü‚µ‚½B"
//#define STRCMD_CS_COMMAND_SUBLEADER_RESULT_2		"%s‚ð 2”Ô–Ú‚Ì•›Žw“±ŽÒ‚ÉÝ’è‚µ‚Ü‚µ‚½B"
//#define STRCMD_CS_COMMAND_SUBLEADER_RESULT_10		"%s‚Ì–¼‘O‚Í‘¶Ý‚µ‚Ä‚¢‚Ü‚¹‚ñB"
//#define STRCMD_CS_COMMAND_SUBLEADER_RESULT_20		"%s‚Í•›Žw“±ŽÒ‚É Ý’è ‚³‚ê‚Ä‚¢‚Ü‚·B"

// 2007-02-23 by dhjin, ??î•ñ	
#define STRCMD_CS_COMMAND_STRATEGYPOINTINFO_0		"/StrategyPointInfo"
#define STRCMD_CS_COMMAND_STRATEGYPOINTINFO_1		"/‹’“_î•ñ"
#define STRCMD_CS_COMMAND_STRATEGYPOINTINFO_HELP	"format: /‹’“_î•ñ - Œ»Ý ‹’“_isî•ñ‚ð•\Ž¦‚·‚éB"
#define STRCMD_CS_COMMAND_STRATEGYPOINTINFO_EMPTY	"is’†‚Ì‹’“_í‚ª‚ ‚è‚Ü‚¹‚ñB"
#define STRCMD_CS_COMMAND_STRATEGYPOINTINFO_EXIST	"‹’“_í‚ªis’†‚Å‚·B"
#define STRCMD_CS_COMMAND_STRATEGYPOINTINFO_ZONE	"is’nˆæ"
#define STRCMD_CS_COMMAND_STRATEGYPOINTINFO_STARTTIME	"ŠJŽnŽžŠÔ"

// 2007-03-29 by cmkwon	
#define STRCMD_CS_UNITKIND_BGEAR					"B-Gear"
#define STRCMD_CS_UNITKIND_MGEAR					"M-Gear"
#define STRCMD_CS_UNITKIND_AGEAR					"A-Gear"
#define STRCMD_CS_UNITKIND_IGEAR					"I-Gear"
#define STRCMD_CS_UNITKIND_BGEAR_ALL				"B-Gear All"
#define STRCMD_CS_UNITKIND_MGEAR_ALL				"M-Gear All"
#define STRCMD_CS_UNITKIND_AGEAR_ALL				"A-Gear All"
#define STRCMD_CS_UNITKIND_IGEAR_ALL				"I-Gear All"
#define STRCMD_CS_UNITKIND_GEAR_ALL					"Gear All"

// 2007-03-30 by dhjin, ƒIƒuƒU[ƒo[ ƒ‚[ƒh ƒ†[ƒU[ “o˜^
#define STRCMD_CS_COMMAND_OBSERVER_REG_START_0		"/Observerstart"		// 2007-03-30 by dhjin, ƒNƒ‰ƒCƒAƒ“ƒg??? ??
#define STRCMD_CS_COMMAND_OBSERVER_REG_START_1		"/ƒIƒuƒU[ƒo[ŠJŽn"			// 2007-03-30 by dhjin, ƒNƒ‰ƒCƒAƒ“ƒg??? ??
#define STRCMD_CS_COMMAND_OBSERVER_REG_END_0		"/Observerend"			// 2007-03-30 by dhjin, ƒNƒ‰ƒCƒAƒ“ƒg??? ??
#define STRCMD_CS_COMMAND_OBSERVER_REG_END_1		"/ƒIƒuƒU[ƒo[I—¹"			// 2007-03-30 by dhjin, ƒNƒ‰ƒCƒAƒ“ƒg??? ??
#define STRCMD_CS_COMMAND_OBSERVER_REG_0			"/Observer"
#define STRCMD_CS_COMMAND_OBSERVER_REG_1			"/ƒIƒuƒU[ƒo["
#define STRCMD_CS_COMMAND_OBSERVER_REG_HELP			"format: /ƒIƒuƒU[ƒo[ [n] [CharacterName] | CharacterNameƒ†[ƒU[‚ð n‚Æ‚¢‚¤”Ô†‚ÉƒZ[ƒu‚·‚é"

// 2007-04-10 by cmkwon, ‘å‰ïƒT[ƒo? ??
#define STRCMD_CS_COMMAND_JAMBOREE_INIT_0			"/InitJamboree"
#define STRCMD_CS_COMMAND_JAMBOREE_INIT_1			"/‘å‰ïƒT[ƒo‰Šú‰»"
#define STRCMD_CS_COMMAND_JAMBOREE_INIT_HELP		"format: /InitJamboree [”FØ”Žš] - ‘å‰ïƒT[ƒoŒQ DB(atum2_db_20)‚ð ‰Šú‰» ‚·‚éB"
#define STRCMD_CS_COMMAND_JAMBOREE_ENTRANTS_0		"/EntrantJamboree"
#define STRCMD_CS_COMMAND_JAMBOREE_ENTRANTS_1		"/‘å‰ïƒT[ƒoŽQ‰ÁŽÒ"
// 2008-04-15 by cmkwon, ‘å‰ïƒT[ƒo(JamboreeServer)? ƒVƒXƒeƒ€ ?? - ??? ?? ?? ???
//#define STRCMD_CS_COMMAND_JAMBOREE_ENTRANTS_HELP	"format: /EntrantJamboree [CharacterName] | Žw’è‚µ‚½ƒLƒƒƒ‰ƒNƒ^[‚Ìƒf[ƒ^‚ð ‘å‰ïƒT[ƒoŒQ DB(atum2_db_20)‚ÉƒRƒs[‚·‚éB"
#define STRCMD_CS_COMMAND_JAMBOREE_ENTRANTS_HELP	"format: /EntrantJamboree [CharacterName] [1(Normal)|2(BCU)|4(ANI)] | Žw’è‚µ‚½ƒLƒƒƒ‰ƒNƒ^[‚Ìƒf[ƒ^‚ð‘å‰ïƒT[ƒoŒQ DB(atum2_db_20)‚Ì ŠY“– ¨—ÍƒRƒs[‚·‚éB"
#define STRCMD_CS_JAMBOREE_PREADD_CHARACTER_NAME_1		"1_"
#define STRCMD_CS_JAMBOREE_PREADD_CHARACTER_NAME_2		"2_"
#define STRCMD_CS_JAMBOREE_PREADD_CHARACTER_NAME_3		"3_"
#define STRCMD_CS_JAMBOREE_PREADD_CHARACTER_NAME_4		"4_"
#define STRCMD_CS_JAMBOREE_PREADD_CHARACTER_NAME_5		"5_"
#define STRCMD_CS_JAMBOREE_PREADD_CHARACTER_NAME_6		"6_"
#define STRCMD_CS_JAMBOREE_PREADD_CHARACTER_NAME_7		"7_"
#define STRCMD_CS_JAMBOREE_PREADD_CHARACTER_NAME_8		"8_"
#define STRCMD_CS_JAMBOREE_PREADD_CHARACTER_NAME_9		"9_"
#define STRCMD_CS_JAMBOREE_PREADD_CHARACTER_NAME_10		"10_"
#define STRCMD_CS_JAMBOREE_PREADD_CHARACTER_NAME_11		"11_"
#define STRCMD_CS_JAMBOREE_PREADD_CHARACTER_NAME_12		"12_"
#define STRCMD_CS_JAMBOREE_PREADD_CHARACTER_NAME_13		"13_"
#define STRCMD_CS_JAMBOREE_PREADD_CHARACTER_NAME_14		"14_"
#define STRCMD_CS_JAMBOREE_PREADD_CHARACTER_NAME_15		"15_"
#define STRCMD_CS_JAMBOREE_PREADD_CHARACTER_NAME_16		"16_"
#define STRCMD_CS_JAMBOREE_PREADD_CHARACTER_NAME_17		"17_"
#define STRCMD_CS_JAMBOREE_PREADD_CHARACTER_NAME_18		"18_"
#define STRCMD_CS_JAMBOREE_PREADD_CHARACTER_NAME_19		"19_"

// 2007-04-17 by dhjin, ƒŒƒxƒ‹ƒ‰ƒ“ƒN? ?? ??
#define	STRCMD_CS_CHARACTER_12_LEVEL_RANK		"ŒP—û¶"
#define	STRCMD_CS_CHARACTER_22_LEVEL_RANK		"ƒWƒ…ƒjƒA"
#define	STRCMD_CS_CHARACTER_32_LEVEL_RANK		"ƒGƒA[ƒ}ƒ“"
#define	STRCMD_CS_CHARACTER_42_LEVEL_RANK		"ƒEƒBƒ“ƒOƒ}ƒ“"
#define	STRCMD_CS_CHARACTER_52_LEVEL_RANK		"ƒG[ƒX"
#define	STRCMD_CS_CHARACTER_62_LEVEL_RANK		"ƒxƒeƒ‰ƒ“"
#define	STRCMD_CS_CHARACTER_72_LEVEL_RANK		"ƒgƒbƒvƒKƒ“"
#define	STRCMD_CS_CHARACTER_82_LEVEL_RANK		"ƒEƒCƒ“ƒOƒRƒ}ƒ“ƒh"
#define	STRCMD_CS_CHARACTER_92_LEVEL_RANK		"ƒq[ƒ["

// 2007-05-09 by cmkwon, 
#define STRMSG_VERSION_INFO_FILE_NAME				"VersionInfo.ver"
#define STRMSG_REG_KEY_NAME_LAUNCHER_VERSION		"LauncherVersion"
#define STRMSG_REG_KEY_NAME_CLIENT_VERSION			"ClientVersion"
// 2007-12-27 by cmkwon, ƒEƒBƒ“ƒhƒEƒ‚[ƒh‹@”\’Ç‰Á -
//#define STRMSG_REG_KEY_NAME_WINDOWDEGREE			"WindowDegree"
#define STRMSG_REG_KEY_NAME_ACCOUNT_NAME			"AccountName"
#define STRMSG_REG_KEY_NAME_SERVER_GROUP_NAME		"ServerGroupName"

// 2007-05-23 by dhjin, ARENA ? o—Í ?? ???
#define STRMSG_CS_STRING_ARENA_NOT_SEARCH			"ƒAƒŠ[ƒi ƒ`[ƒ€‚ªŒ©‚Â‚©‚è‚Ü‚¹‚ñB"
#define STRMSG_CS_COMMAND_ARENA_TEAM_INFO_0			"/ARENA"
#define STRMSG_CS_COMMAND_ARENA_TEAM_INFO_1			"/ƒAƒŠ[ƒi"
#define STRMSG_CS_COMMAND_ARENA_TEAM_INFO_HELP		"format: /ƒAƒŠ[ƒi [2(BCU)|4(ANI)]- Œ»Ý ƒAƒŠ[ƒi is î•ñ‚ð •\Ž¦‚·‚éB"

// 2010. 06. 04 by hsLee ARENA ÀÎÇÇ´ÏÆ¼ °ü·Ã. - 
// 2010. 06. 04 by hsLee ÀÎÆ¼ÇÇ´Ï ÇÊµå 2Â÷ ³­ÀÌµµ Á¶Àý. (GM ¸í·É¾î Ãß°¡. /nextscene(´ÙÀ½ ½Ã³×¸¶ ¾À È£Ãâ.) )
#define STRCMD_CS_COMMAND_INFINITY_NEXT_SCENE		"/nextscene"
// End 2010. 06. 04 by hsLee ÀÎÆ¼ÇÇ´Ï ÇÊµå 2Â÷ ³­ÀÌµµ Á¶Àý. (GM ¸í·É¾î Ãß°¡. /nextscene(´ÙÀ½ ½Ã³×¸¶ ¾À È£Ãâ.) )

// 2007-06-15 by dhjin, ŠÏí
#define STRMSG_CS_COMMAND_WATCH_START_INFO_0		"/ŠÏíŠJŽn"
#define STRMSG_CS_COMMAND_WATCH_START_INFO_1		"/WatchStart"
#define STRMSG_CS_COMMAND_WATCH_START_INFO_HELP		"format: /ŠÏíŠJŽn - ŠÏí‚ð ŠJŽn‚·‚éB"
#define STRMSG_CS_COMMAND_WATCH_END_INFO_0			"/ŠÏíI—¹"
#define STRMSG_CS_COMMAND_WATCH_END_INFO_1			"/WatchEnd"
#define STRMSG_CS_COMMAND_WATCH_END_INFO_HELP		"format: /ŠÏíI—¹ - ŠÏí‚ð I—¹‚·‚éB"

// 2007-06-22 by dhjin, WarPoint ’Ç‰Á
#define STRMSG_CS_COMMAND_WARPOINT_0				"/í‘ˆƒ|ƒCƒ“ƒgiWPj"
#define STRMSG_CS_COMMAND_WARPOINT_1				"/WarPoint"
#define STRMSG_CS_COMMAND_WARPOINT_HELP				"format: /í‘ˆƒ|ƒCƒ“ƒgiWPj [”’l 1`1000000] [|ƒ†[ƒU[ƒl[ƒ€] - í‘ˆƒ|ƒCƒ“ƒgiWPj‚ð ’Ç‰Á‚·‚éB"

// 2007-06-26 by dhjin, ?ƒ|ƒCƒ“ƒg ƒCƒxƒ“ƒg ?? ’Ç‰Á
#define STRCMD_CS_COMMAND_GAMEEVENT_P1WARPOINT		"í‘ˆƒ|ƒCƒ“ƒgiWPj"

// 2007-07-11 by cmkwon, ƒAƒŠ[ƒiƒuƒƒbƒNƒVƒXƒeƒ€ ?? - –½—ßŒê ’Ç‰Á(/ƒAƒŠ[ƒiƒuƒƒbƒN, /ƒAƒŠ[ƒiƒuƒƒbƒN‰ðœ)
#define STRCMD_CS_COMMAND_ARENAFORBID_0				"/ƒAƒŠ[ƒiƒuƒƒbƒN"
#define STRCMD_CS_COMMAND_ARENAFORBID_1				"/forbidArena"
#define STRCMD_CS_COMMAND_ARENAFORBID_2				"/forbidArena"
#define STRCMD_CS_COMMAND_ARENAFORBID_HELP			"format: /ƒAƒŠ[ƒiƒuƒƒbƒN [character name] [|ŽžŠÔ(•ª)] - Arena ƒuƒƒbƒN ‚·‚é"
#define STRCMD_CS_COMMAND_ARENAFORBIDRelease_0		"/ƒAƒŠ[ƒiƒuƒƒbƒN‰ðœ"
#define STRCMD_CS_COMMAND_ARENAFORBIDRelease_1		"/releaseArena"
#define STRCMD_CS_COMMAND_ARENAFORBIDRelease_2		"/releaseArena"
#define STRCMD_CS_COMMAND_ARENAFORBIDRelease_HELP	"format: /ƒAƒŠ[ƒiƒuƒƒbƒN‰ðœ [character name] - Arena ƒuƒƒbƒN ‰ðœ ‚·‚é"

///////////////////////////////////////////////////////////////////////////////
// 2007-08-02 by cmkwon, —·’c ƒ}[ƒN ?? ƒVƒXƒeƒ€ ?? - ’Ç‰Á? ???
#define STRMSG_070802_0001				"—·’cƒ}[ƒN“o˜^\‚µž‚Ý‚ªŠ®—¹‚µ‚Ü‚µ‚½B"
#define STRMSG_070802_0002				"R¸‚ÌŒãA“o˜^‚ªŠ®—¹‚µ‚Ü‚·B"
#define STRMSG_070802_0003				"‘I‘ð‚µ‚½%dŒÂ‚Ì—·’cƒ}[ƒN‚ð‹–‰Â‚µ‚Ü‚·‚©H"
#define STRMSG_070802_0004				"—·’cƒ}[ƒN‚È‚µ"
#define STRMSG_070802_0005				"—·’cƒ}[ƒN‘Ò‹@"
#define STRMSG_070802_0006				"—·’cƒ}[ƒN³í"
#define STRMSG_070802_0007				"—·’cƒ}[ƒNƒGƒ‰["

// 2007-08-24 by cmkwon, ƒXƒs[ƒJ[ƒAƒCƒeƒ€ Žg—p‰Â”\/ƒuƒƒbƒN Ý’è ?? ’Ç‰Á - –½—ßŒê ’Ç‰Á
#define STRCMD_CS_COMMAND_UsableSpeakerItem_0			"/ƒXƒs[ƒJ[Žg—p"
#define STRCMD_CS_COMMAND_UsableSpeakerItem_1			"/UseSpeaker"
#define STRCMD_CS_COMMAND_UsableSpeakerItem_2			"/UseSpeaker"
#define STRCMD_CS_COMMAND_UsableSpeakerItem_P1Able		"‰Â”\"
#define STRCMD_CS_COMMAND_UsableSpeakerItem_P1Forbid	"‹ÖŽ~"
#define STRCMD_CS_COMMAND_UsableSpeakerItem_HELP		"format:/[ƒXƒs[ƒJ[Žg—p|UseSpeaker|UseSpeaker][‰Â”\|‹ÖŽ~]-ƒXƒs[ƒJ[ƒAƒCƒeƒ€ Žg—p‚ð ƒuƒƒbƒN/‰ðœ ‚·‚é"

// 2007-08-27 by cmkwon, ƒT[ƒo???? –½—ßŒê ’Ç‰Á(SCAdminTool?? SCMonitor? PrepareShutdown‚ð ?? ? ? ??)
#define STRCMD_CS_COMMAND_PrepareShutdown_0				"/ƒT[ƒoƒ_ƒEƒ“€”õ"
#define STRCMD_CS_COMMAND_PrepareShutdown_1				"/PrepareShutdown"
#define STRCMD_CS_COMMAND_PrepareShutdown_2				"/PrepareShutdown"
#define STRCMD_CS_COMMAND_PrepareShutdown_P1Start		"Start"
#define STRCMD_CS_COMMAND_PrepareShutdown_P1Release		"Release"
#define STRCMD_CS_COMMAND_PrepareShutdown_HELP			"format: /[ƒT[ƒoƒ_ƒEƒ“€”õ|PrepareShutdown|PrepareShutdown] [Start|Release] - ƒT[ƒo I—¹ €”õA‘S‚Ä‚Ìƒ†[ƒU[‚ÌÚ‘±‚ð‹­§I—¹‚·‚é"

// 2007-08-30 by cmkwon, ‰ï‹cŽº ƒVƒXƒeƒ€ ?? - –½—ßŒê ’Ç‰Á
#define STRCMD_CS_COMMAND_EntrancePermission_0				"/“üê‹–‰Â"
#define STRCMD_CS_COMMAND_EntrancePermission_1				"/EntrancePermission"
#define STRCMD_CS_COMMAND_EntrancePermission_2				"/EntrancePermission"
#define STRCMD_CS_COMMAND_EntrancePermission_HELP			"format: /[“üê‹–‰Â] [|CharacterName] | Žw“±ŽÒ‚Ì‚ÝŽg—p‰Â”\BŠY“– ƒLƒƒƒ‰ƒNƒ^[‚ð“üê‹–‰Â ƒŠƒXƒg‚É’Ç‰Á‚·‚éB"
#define STRCMD_CS_COMMAND_EntrancePermissionDeny_0			"/“üê•s‰Â"
#define STRCMD_CS_COMMAND_EntrancePermissionDeny_1			"/EntrancePermissionDeny"
#define STRCMD_CS_COMMAND_EntrancePermissionDeny_2			"/EntrancePermissionDeny"
#define STRCMD_CS_COMMAND_EntrancePermissionDeny_HELP		"format: /[“üê•s‰Â] [CharacterName] | Žw“±ŽÒ‚Ì‚ÝŽg—p‰Â”\AŠY“– ƒLƒƒƒ‰ƒNƒ^[‚ð ‰ï‹cŽº “üê‹–‰Â ƒŠƒXƒg‚©‚çíœ‚·‚éB"

// 2007-10-05 by cmkwon, ???‚ð ???? ????? ??
#define STRCMD_071005_0000					"%d“ú%dŽžŠÔ%d•ª%d•b", Day, Hour, Minute, Second
#define STRCMD_071005_0001					"%d”N%dŒŽ%d“ú", Year, Month, Day
#define STRCMD_071005_0002					"%d”N%dŒŽ", Year, Month
#define STRCMD_071005_0003					"%dŒŽ%d“ú", Month, Day


// 2007-10-06 by cmkwon, •›Žw“±ŽÒ 2?? ??‚ð ??? Ý’è -
#define STRCMD_CS_COMMON_RACE_INFLUENCE_SUBLEADER_1		"¨—Íí •›Žw“±ŽÒ 1"
#define STRCMD_CS_COMMON_RACE_INFLUENCE_SUBLEADER_2		"¨—Íí •›Žw“±ŽÒ 2"
#define STRCMD_VCN_INFLUENCE_LEADER						"‘Ži—ßŠ¯"
#define STRCMD_VCN_INFLUENCE_SUBLEADER_1				"•›Ži—ßŠ¯"
#define STRCMD_VCN_INFLUENCE_SUBLEADER_2				"ŽQ–d‘’·"
#define STRCMD_ANI_INFLUENCE_LEADER						"‹c’·"
#define STRCMD_ANI_INFLUENCE_SUBLEADER_1				"•›‹c’·"
#define STRCMD_ANI_INFLUENCE_SUBLEADER_2				"ìí–{•”’·"
#define STRCMD_OUTPOST_GUILD_MASTER						"%s ŠÍ’·"

// 2007-10-06 by dhjin, •›Žw“±ŽÒ ?? ?? •ÏX?? ??
#define STRCMD_CS_COMMAND_SUBLEADER1_0				"/appointment1"
#define STRCMD_CS_COMMAND_SUBLEADER1_1				"/EˆÊ•t—^1"
#define STRCMD_CS_COMMAND_SUBLEADER1_HELP			"format: /EˆÊ•t—^1 [CharacterName] - BCU : •›Ži—ßŠ¯A ANI : •›‹c’· Ý’è"
#define STRCMD_CS_COMMAND_SUBLEADER2_0				"/appointment2"
#define STRCMD_CS_COMMAND_SUBLEADER2_1				"/EˆÊ•t—^2"
#define STRCMD_CS_COMMAND_SUBLEADER2_HELP			"format: /EˆÊ•t—^2 [CharacterName] - BCU : ŽQ–d‘’·AANI : ìí–{•”’·Ý’è"

// 2007-10-30 by cmkwon, ¨—Í? HappyHour ƒCƒxƒ“ƒg ?? - –½—ßŒê ?? ??? ??? ?? ?? ?? ?
#define STRCMD_CS_COMMAND_HAPPYHOUREVENT			"/HappyHour"
#define STRCMD_CS_COMMAND_HAPPYHOUREVENT_1			"/happyEvent"
#define STRCMD_CS_COMMAND_HAPPYHOUREVENT_PSTART		"ŠJŽn"
#define STRCMD_CS_COMMAND_HAPPYHOUREVENT_PEND		"I—¹"
#define STRCMD_CS_COMMAND_HAPPYHOUREVENT_HELP		"format: /HappyHourƒCƒxƒ“ƒg [1(Normal)|2(BCU)|4(ANI)|255(All)] [ŠJŽn|I—¹] [isŽžŠÔ(’PˆÊ:•ª)]"

// 2007-10-30 by cmkwon, ¨—Í? HappyHour ƒCƒxƒ“ƒg ?? - ?? ?‚ð ??? ?
#define STRCMD_CS_INFLUENCE_TYPE_NORMAL			"ˆê”Ê¨—Í"
#define STRCMD_CS_INFLUENCE_TYPE_VCN			"ƒoƒCƒWƒFƒjƒ…[¨—Í"
#define STRCMD_CS_INFLUENCE_TYPE_ANI			"ƒA[ƒŠƒ“ƒgƒ“¨—Í"
#define STRCMD_CS_INFLUENCE_TYPE_ALL_MASK		"‘S‘Ì‘ÎÛ"			// 2007-10-30 by cmkwon, ¨—Í? HappyHour ƒCƒxƒ“ƒg ?? - ’Ç‰Á? ?

// 2007-11-05 by cmkwon, WP ?? –½—ßŒê ?? - –½—ßŒê ’Ç‰Á
#define STRCMD_CS_COMMAND_WAPPOINTINMAP_0			"/AddWarPointInMap"
#define STRCMD_CS_COMMAND_WAPPOINTINMAP_1			"/WP’Ç‰Áƒ}ƒbƒv"
#define STRCMD_CS_COMMAND_WAPPOINTINMAP_2			"/í‘ˆƒ|ƒCƒ“ƒgiWPj’Ç‰Áƒ}ƒbƒv"
#define STRCMD_CS_COMMAND_WAPPOINTINMAP_HELP		"format: /WP’Ç‰Áƒ}ƒbƒv [1(Normal)|2(BCU)|4(ANI)|255(All)] [AddWarPoint(1`)] - Œ»Ýƒ}ƒbƒv‚ÌŽw’è‚µ‚½¨—Í‚Ìƒ†[ƒU[‚Éí‘ˆƒ|ƒCƒ“ƒgiWPj‚ðŽx‹‹‚·‚éB"

// 2007-11-19 by cmkwon, ??ƒVƒXƒeƒ€ ƒAƒbƒvƒf[ƒg - –½—ßŒê ’Ç‰Á
#define STRCMD_CS_COMMAND_STARTCALLGM_0			"/StartCallGM"
#define STRCMD_CS_COMMAND_STARTCALLGM_1			"/ƒwƒ‹ƒvŠJŽn"
#define STRCMD_CS_COMMAND_STARTCALLGM_2			"/ƒwƒ‹ƒvŠJŽn"
#define STRCMD_CS_COMMAND_STARTCALLGM_HELP		"format: /ƒwƒ‹ƒvŠJŽn [|ŽžŠÔ(’PˆÊ:•ª)] | ’ÁÃ ƒVƒXƒeƒ€ ŠJŽn"
#define STRCMD_CS_COMMAND_ENDCALLGM_0			"/EndCallGM"
#define STRCMD_CS_COMMAND_ENDCALLGM_1			"/ƒwƒ‹ƒvI—¹"
#define STRCMD_CS_COMMAND_ENDCALLGM_2			"/ƒwƒ‹ƒvI—¹"
#define STRCMD_CS_COMMAND_ENDCALLGM_HELP		"format: /ƒwƒ‹ƒvI—¹ | ’ÁÃ ƒVƒXƒeƒ€ I—¹"

// 2007-12-27 by cmkwon, ƒEƒBƒ“ƒhƒEƒ‚[ƒh‹@”\’Ç‰Á- STRMSG_REG_KEY_NAME_WINDOWDEGREE_NEW ’Ç‰Á
#define STRMSG_REG_KEY_NAME_WINDOWDEGREE_NEW		"WindowDegreeNew"

// 2008-01-03 by cmkwon, ???ƒ‚[ƒh ?? ???? -
#define STRMSG_REG_KEY_NAME_WINDOWMODE				"WindowMode"

// 2008-01-31 by cmkwon, ƒAƒJƒEƒ“ƒgƒuƒƒbƒN/‰ðœ –½—ßŒê? ??? ƒVƒXƒeƒ€ ?? - –½—ßŒê ’Ç‰Á
#define STRCMD_CS_COMMAND_NEWACCOUNTBLOCK_0					"/Block"
#define STRCMD_CS_COMMAND_NEWACCOUNTBLOCK_1					"/BlockAccount"
#define STRCMD_CS_COMMAND_NEWACCOUNTBLOCK_2					"/ƒuƒƒbƒN"
#define STRCMD_CS_COMMAND_NEWACCOUNTBLOCK_HELP				"format: /Block [AccountName] [BlockType(1:Normal|2:Related Money|3:Related Item|4:Related SpeedHack|5:Related Chatting|6:Related GameBug)] [Period:Days] [Block Reason for User] / [Block Reason for only Admin]"
#define STRCMD_CS_COMMAND_NEWACCOUNTUNBLOCK_0				"/Unblock"
#define STRCMD_CS_COMMAND_NEWACCOUNTUNBLOCK_1				"/UnblockAccount"
#define STRCMD_CS_COMMAND_NEWACCOUNTUNBLOCK_2				"/ƒuƒƒbƒN‰ðœ"
#define STRCMD_CS_COMMAND_NEWACCOUNTUNBLOCK_HELP			"format: /Unblock [AccountName]"

// 2008-02-20 by cmkwon, –½—ßŒê ’Ç‰Á(?? ?? ?? ???? ƒAƒCƒeƒ€ ?? -
#define STRCMD_CS_COMMAND_ITEMALLUSER_0				"/ItemAllUser"
#define STRCMD_CS_COMMAND_ITEMALLUSER_1				"/ƒAƒCƒeƒ€‘S•”"
#define STRCMD_CS_COMMAND_ITEMALLUSER_2				"/ƒAƒCƒeƒ€’Ç‰Á‘S•”"
#define STRCMD_CS_COMMAND_ITEMALLUSER_HELP			"format: /ItemAllUser [1(Normal)|2(BCU)|4(ANI)|255(All)] [Item Number] [# of items] | Ú‘±‚µ‚½ƒ†[ƒU[‚Ì‚È‚©‚ÅŽw’è‚µ‚½¨—Í‚Ìƒ†[ƒU[‚ÉŽw’è‚µ‚½ƒAƒCƒeƒ€‚ðŽx‹‹‚·‚éB"

// 2008-02-21 by dhjin, ƒAƒŠ[ƒi ?? - ƒAƒŠ[ƒi ’Ç‰Á –½—ßŒê
#define STRCMD_CS_COMMAND_ARENAMOVE_0						"/ArenaMove"
#define STRCMD_CS_COMMAND_ARENAMOVE_1						"/ƒAƒŠ[ƒiˆÚ“®"
#define STRCMD_CS_COMMAND_TEAMARENALEAVE_0					"/TeamArenaLeave"
#define STRCMD_CS_COMMAND_TEAMARENALEAVE_1					"/ƒ`[ƒ€ƒT[ƒo•œ‹A"
#define STRCMD_CS_COMMAND_TEAMARENALEAVE_HELP				"format: /TeamArenaLeave [2(BLUE)|4(RED)|6(BLUE AND RED)]"
#define STRCMD_CS_COMMAND_TARGETARENALEAVE_0				"/TargetArenaLeave"
#define STRCMD_CS_COMMAND_TARGETARENALEAVE_1				"/‘ÎÛƒT[ƒo•œ‹A"
#define STRCMD_CS_COMMAND_TARGETARENALEAVE_HELP				"format: /TargetArenaLeave [Charactername]"
#define STRCMD_CS_COMMAND_ARENAEVENT_0						"/ArenaEvent"
#define STRCMD_CS_COMMAND_ARENAEVENT_1						"/ƒAƒŠ[ƒiƒCƒxƒ“ƒg"
#define STRCMD_CS_COMMAND_ARENAEVENT_2						"/ƒAƒŠ[ƒiƒCƒxƒ“ƒg"
#define STRCMD_CS_COMMAND_ARENAEVENT_HELP					"format: /ArenaEvent [RoomNumber]"
#define STRCMD_CS_COMMAND_ARENAEVENTRELEASE_0				"/ArenaEventRelease"
#define STRCMD_CS_COMMAND_ARENAEVENTRELEASE_1				"/ƒAƒŠ[ƒiƒCƒxƒ“ƒg‰ðœ"
#define STRCMD_CS_COMMAND_ARENAEVENTRELEASE_2				"/ƒAƒŠ[ƒiƒCƒxƒ“ƒg‰ðœ"
#define STRCMD_CS_COMMAND_ARENAEVENTRELEASE_HELP			"format: /ArenaEventRelease [RoomNumber]"

// 2008-06-03 by cmkwon, AdminTool, DBTool ??? ƒAƒCƒeƒ€ ŒŸõ? ?????? ŒŸõ ?? ’Ç‰Á(K0000143) -
#define STRCMD_CS_ITEMKIND_ALL_ITEM							"‘SƒAƒCƒeƒ€—Þ"

//////////////////////////////////////////////////////////////////////////
// 2008-05-20 by dhjin, EP3 - —·’c ?? ??	// 2006-03-07 by cmkwon
#define STRCMD_CS_GUILD_RANK_PRIVATE_NULL		"‘àˆõ"
#define STRCMD_CS_GUILD_RANK_COMMANDER			"—·’c’·"
#define STRCMD_CS_GUILD_RANK_SUBCOMMANDER		"•›—·’c’·"				// 2008-05-20 by dhjin, EP3 - —·’c ?? ??
#define STRCMD_CS_GUILD_RANK_SQUAD_LEADER_1		"1‘å‘à’·"
#define STRCMD_CS_GUILD_RANK_PRIVATE_1			"1‘å‘àˆõ"
#define STRCMD_CS_GUILD_RANK_SQUAD_LEADER_2		"2‘å‘à’·"
#define STRCMD_CS_GUILD_RANK_PRIVATE_2			"2‘å‘àˆõ"
#define STRCMD_CS_GUILD_RANK_SQUAD_LEADER_3		"3‘å‘à’·"
#define STRCMD_CS_GUILD_RANK_PRIVATE_3			"3‘å‘àˆõ"
#define STRCMD_CS_GUILD_RANK_SQUAD_LEADER_4		"4‘å‘à’·"
#define STRCMD_CS_GUILD_RANK_PRIVATE_4			"4‘å‘àˆõ"
#define STRCMD_CS_GUILD_RANK_SQUAD_LEADER_5		"5‘å‘à’·"
#define STRCMD_CS_GUILD_RANK_PRIVATE_5			"5‘å‘àˆõ"

//////////////////////////////////////////////////////////////////////////
// 2008-06-19 by dhjin, EP3 - ??î•ñ
#define STRCMD_COMMAND_WAR_OPTION_0					"/•ê‘Díî•ñƒIƒvƒVƒ‡ƒ“"
#define STRCMD_COMMAND_WAR_OPTION_1					"/MotherShipInfoOption"

// 2008-08-18 by dhjin, ¨—Íƒ}[ƒNƒCƒxƒ“ƒg
#define STRCMD_CS_COMMAND_INFLUENCEMARKEVENT_0				"/influencemarkevent"
#define STRCMD_CS_COMMAND_INFLUENCEMARKEVENT_1				"/¨—Íƒ}[ƒNƒCƒxƒ“ƒg"
#define STRCMD_CS_COMMAND_INFLUENCEMARKEVENT_2				"/¨—Íƒ}[ƒNƒCƒxƒ“ƒg"
#define STRCMD_CS_COMMAND_INFLUENCEMARKEVENT_HELP			"format: /influencemarkevent [¨—Í 2(BCU)|4(ANI)]"
#define STRCMD_CS_COMMAND_INFLUENCEMARKEVENTEND_0			"/influencemarkeventend"
#define STRCMD_CS_COMMAND_INFLUENCEMARKEVENTEND_1			"/¨—Íƒ}[ƒNƒCƒxƒ“ƒgI—¹"
#define STRCMD_CS_COMMAND_INFLUENCEMARKEVENTEND_2			"/¨—Íƒ}[ƒNƒCƒxƒ“ƒgI—¹"
#define STRCMD_CS_COMMAND_INFLUENCEMARKEVENTEND_HELP		"format: /influencemarkeventend"

//////////////////////////////////////////////////////////////////////////
// 2008-08-25 by dhjin, ?? PC? IPî•ñ ??
#define STRCMD_CS_COMMAND_PCBANGRELOADTIME_0				"/PCBangReloadTime"
#define STRCMD_CS_COMMAND_PCBANGRELOADTIME_1				"/ƒlƒbƒgƒJƒtƒFƒŠƒ[ƒhƒ^ƒCƒ€"
#define STRCMD_CS_COMMAND_PCBANGRELOADTIME_HELP				"format: /PCBangreloadtime [Minute] - 10 Min ` 1440 Min"


// 2008-08-21 by dhjin, ˆê”Ê, ?? ƒAƒJƒEƒ“ƒg? •›Žw“±ŽÒ ?? ??
#define STRMSG_080821_0001				"‘I‘ð‚µ‚½ƒLƒƒƒ‰ƒNƒ^[‚ÉŠK‹‰‚ð”C–½‚Å‚«‚Ü‚¹‚ñB"


// 2008-09-09 by cmkwon, /¨—Í¢Š« –½—ßŒê ?? ƒŠƒXƒg? ??ƒ^ƒCƒv ’Ç‰Á -
#define STRCMD_CS_COMMAND_COMEONINFL_HELP2		"format: /ComeOnInfl [1(Normal)|2(VCN)|4(ANI)|255(All)] [Å‘ålˆõ] [0|Å¬ƒŒƒxƒ‹] [0|Å‘åƒŒƒxƒ‹] [1(B)|16(M)|256(A)|4096(I)] [ƒ†[ƒU[‚É‘—‚éƒƒbƒZ[ƒW] | ”CˆÓ‚ÌŽw’è‚µ‚½¨—ÍAŽw’è‚µ‚½ƒŒƒxƒ‹‚Ìƒ†[ƒU[‚ÉƒCƒxƒ“ƒgƒ}ƒbƒv‚Ö‚ÌˆÚ“®‚ð—v¿‚·‚é"

// 2008-09-09 by cmkwon, "/kick" –½—ßŒê ’Ç‰Á - 
#define STRCMD_CS_COMMAND_KICK_0							"/Kick"
#define STRCMD_CS_COMMAND_KICK_1							"/ƒLƒbƒN"
#define STRCMD_CS_COMMAND_KICK_HELP							"format: /Kick [CharacterName] - ŠY“– ƒLƒƒƒ‰ƒNƒ^[‚ð ƒQ[ƒ€ I—¹‚³‚¹‚éB"


// 2008-09-12 by cmkwon, "/–¼º" –½—ßŒê ’Ç‰Á -
#define STRCMD_CS_COMMAND_ADD_FAME_0							"/Fame"
#define STRCMD_CS_COMMAND_ADD_FAME_1							"/–¼º"
#define STRCMD_CS_COMMAND_ADD_FAME_HELP							"format: /–¼º [ŒÂl–¼º] [—·’c–¼º] | Ú‘± ƒLƒƒƒ‰ƒNƒ^[‚Ì–¼ºAƒLƒƒƒ‰ƒNƒ^[ —·’c –¼º‚ð ã‚°‚éB"

// 2008-12-30 by cmkwon, Žw“±ŽÒƒ`ƒƒƒbƒg§ŒÀƒJ[ƒh -
#define STRCMD_CS_COMMAND_CHATFORBIDRELEASE_LEADER_0			"/ReleaseLeaderChatBlock"
#define STRCMD_CS_COMMAND_CHATFORBIDRELEASE_LEADER_1			"/Žw“±ŽÒƒ`ƒƒƒbƒg§ŒÀ‰ðœ"
#define STRCMD_CS_COMMAND_CHATFORBIDRELEASE_LEADER_HELP			"format: /Žw“±ŽÒƒ`ƒƒƒbƒg§ŒÀ‰ðœ[CharacterName]|Žw“±ŽÒƒ`ƒƒƒbƒg§ŒÀ‚Ì‰ðœB"

// 2009-10-12 by cmkwon, ƒtƒŠ[ƒXƒJ‚ÌíœˆÄ“K—p - 
#define STRCMD_CS_COMMAND_CHANGE_StartCityMapIndex_0                       "/StartCityMap"
#define STRCMD_CS_COMMAND_CHANGE_StartCityMapIndex_1                       "/ƒXƒ^[ƒg“sŽsƒ}ƒbƒv"
#define STRCMD_CS_COMMAND_CHANGE_StartCityMapIndex_HELP                              "format: /ƒXƒ^[ƒg“sŽsƒ}ƒbƒv [2001|2002] [|CharacterName] - ŠY“–ƒLƒƒƒ‰ƒNƒ^[‚Ìˆê”Ê¨—ÍŽžAƒXƒ^[ƒg‚·‚é“sŽs‚Ìƒ}ƒbƒv‚ðŠY“–ƒ}ƒbƒv(2001|2002)‚É•ÏX‚·‚éB"


///////////////////////////////////////////////////////////////////////////////
// 2010-01-08 by cmkwon, ÃÖ´ë ·¹º§ »óÇâ¿¡ µû¸¥ Ãß°¡ »çÇ×(·¹º§º° °è±Þ) - °è±Þ¶æ(¹éºÎÀå, ´ë·É, Àå±º, ÃÑµ¶, Á¤º¹ÀÚ, ¼öÈ£ÀÚ, Àü¼³ÀûÀÎ)
#define STRCMD_CS_CHARACTER_96_LEVEL_RANK "ƒZƒ“ƒ`ƒ…ƒŠƒAƒ“"
#define STRCMD_CS_CHARACTER_100_LEVEL_RANK "ƒJ[ƒlƒ‹"
#define STRCMD_CS_CHARACTER_104_LEVEL_RANK "ƒWƒFƒlƒ‰ƒ‹"
#define STRCMD_CS_CHARACTER_108_LEVEL_RANK "ƒKƒoƒi["
#define STRCMD_CS_CHARACTER_112_LEVEL_RANK "ƒRƒ“ƒNƒGƒXƒ^"
#define STRCMD_CS_CHARACTER_116_LEVEL_RANK "ƒK[ƒfƒBƒAƒ“"
#define STRCMD_CS_CHARACTER_120_LEVEL_RANK "ƒŒƒWƒFƒ“ƒ_ƒŠ["
#define STRCMD_CS_CHARACTER_XX_LEVEL_RANK "Šî–{"

#endif // end_#ifndef _STRING_DEFINE_COMMON_H_





