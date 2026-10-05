// characterinfodlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "characterinfodlg.h"
#include "atumsj.h"
#include "ODBCStatement.h"
#include "AtumError.h"				// 2009-03-31 by cmkwon, 세력초기화 시스템 구현 - 

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCharacterInfoDlg dialog


CCharacterInfoDlg::CCharacterInfoDlg(BOOL i_bEnableEdit, CODBCStatement *i_pODBCState, CWnd* pParent /*=NULL*/)
	: CDialog(CCharacterInfoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCharacterInfoDlg)
	m_CharacterName = _T("");
	m_DP = 0;
	m_CurDP = 0;
	m_HP = 0;
	m_CurHP = 0;
	m_SP = 0;
	m_CurSP = 0;
	m_EP = 0;
	m_CurEP = 0;
	m_ExpPercentage = 0.0f;
	m_Level = 0;
	m_MapIndex = 0;
	m_nChannelIndex = 0;
	m_PartAttack = 0;
	m_PartDefense = 0;
	m_PartFuel = 0;
	m_PartSoul = 0;
	m_PartShield = 0;
	m_PartDodge = 0;
	m_BonusStat = 0;
	m_BonusStatPoint = 0;
	m_PosX = 0;
	m_PosZ = 0;
	m_Propensity = 0;
	m_RacingPoint = 0;
	m_ctl_doExp = 0.0;
	m_ctl_bInflLeader = FALSE;
	m_ctl_bInflSubLeader = FALSE;
	m_ctl_bInflSub2Leader = FALSE;
	m_PCBangTotalPlayTime = 0;
	m_ArenaCWP = 0;
	m_ArenaDisConnect = 0;
	m_ArenaLose = 0;
	m_ArenaWin = 0;
	m_ArenaWP = 0;
	//}}AFX_DATA_INIT
	nDestInfluence = 0;
	m_bEnableEdit			= i_bEnableEdit;		// 2006-04-15 by cmkwon
	m_pODBCStmt3			= i_pODBCState;			// 2006-05-03 by cmkwon
	m_CharacterUID			= 0;					// 2006-05-03 by cmkwon
	m_usUnitKind			= 0;					// 2007-01-11 by cmkwon
	m_AccountUID			= 0;					// 2007-02-13 by dhjin
	m_bReloadCharacterInfo	= FALSE;				// 2009-03-31 by cmkwon, 세력초기화 시스템 구현 - 
}


void CCharacterInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCharacterInfoDlg)
	DDX_Text(pDX, IDC_EDIT_CHARACTER_NAME, m_CharacterName);
	DDX_Text(pDX, IDC_EDIT_DP, m_DP);
	DDX_Text(pDX, IDC_EDIT_DP_CUR, m_CurDP);
	DDX_Text(pDX, IDC_EDIT_HP, m_HP);
	DDX_Text(pDX, IDC_EDIT_HP_CUR, m_CurHP);
	DDX_Text(pDX, IDC_EDIT_SP, m_SP);
	DDX_Text(pDX, IDC_EDIT_SP_CUR, m_CurSP);
	DDX_Text(pDX, IDC_EDIT_EP, m_EP);
	DDX_Text(pDX, IDC_EDIT_EP_CUR, m_CurEP);
	DDX_Text(pDX, IDC_EDIT_EXP_PER, m_ExpPercentage);
	DDX_Text(pDX, IDC_EDIT_LEVEL, m_Level);
	DDX_Text(pDX, IDC_EDIT_MAP_INDEX, m_MapIndex);
	DDX_Text(pDX, IDC_EDIT_CHANNEL_INDEX, m_nChannelIndex);
	DDX_Text(pDX, IDC_EDIT_PART_ATTACK, m_PartAttack);
	DDX_Text(pDX, IDC_EDIT_PART_DEFENSE, m_PartDefense);
	DDX_Text(pDX, IDC_EDIT_PART_FUEL, m_PartFuel);
	DDX_Text(pDX, IDC_EDIT_PART_SOUL, m_PartSoul);
	DDX_Text(pDX, IDC_EDIT_PART_SHIELD, m_PartShield);
	DDX_Text(pDX, IDC_EDIT_PART_DODGE, m_PartDodge);
	DDX_Text(pDX, IDC_EDIT_BONUS_STAT, m_BonusStat);
	DDX_Text(pDX, IDC_EDIT_BONUS_STAT_POINT, m_BonusStatPoint);
	DDX_Text(pDX, IDC_EDIT_POS_X, m_PosX);
	DDX_Text(pDX, IDC_EDIT_POS_Z, m_PosZ);
	DDX_Text(pDX, IDC_EDIT_PROPENSITY, m_Propensity);
	DDX_Text(pDX, IDC_EDIT_RACING_POINT, m_RacingPoint);
	DDX_Text(pDX, IDC_EDIT_EXP, m_ctl_doExp);
	DDX_Check(pDX, IDC_CHECK_SET_INFL_LEADER, m_ctl_bInflLeader);
	DDX_Check(pDX, IDC_CHECKSubLeader, m_ctl_bInflSubLeader);
	DDX_Check(pDX, IDC_CHECKInflSub2Leader, m_ctl_bInflSub2Leader);
	DDX_Text(pDX, IDC_EDIT_PCBangTotalPlayTime, m_PCBangTotalPlayTime);
	DDX_Text(pDX, IDC_EDIT_ARENA_CWP, m_ArenaCWP);
	DDX_Text(pDX, IDC_EDIT_ARENA_DISCONNECT, m_ArenaDisConnect);
	DDX_Text(pDX, IDC_EDIT_ARENA_LOSE, m_ArenaLose);
	DDX_Text(pDX, IDC_EDIT_ARENA_WIN, m_ArenaWin);
	DDX_Text(pDX, IDC_EDIT_ARENA_WP, m_ArenaWP);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_RADIO1, m_BtnDonatorRank[0]);
	DDX_Control(pDX, IDC_RADIO2, m_BtnDonatorRank[1]);
	DDX_Control(pDX, IDC_RADIO3, m_BtnDonatorRank[2]);
	DDX_Control(pDX, IDC_RADIO4, m_BtnDonatorRank[3]);
	DDX_Control(pDX, IDC_RADIO5, m_BtnDonatorRank[4]);
	DDX_Control(pDX, IDC_RADIO6, m_BtnDonatorRank[5]);
	DDX_Control(pDX, IDC_RADIO7, m_BtnDonatorRank[6]);
	DDX_Control(pDX, IDC_RADIO8, m_BtnDonatorRank[7]);
	DDX_Control(pDX, IDC_RADIO9, m_BtnDonatorRank[8]);
	DDX_Control(pDX, IDC_RADIO10, m_BtnDonatorRank[9]);
	DDX_Control(pDX, IDC_RADIO11, m_BtnDonatorRank[10]);
	DDX_Control(pDX, IDC_RADIO12, m_BtnDonatorRank[11]);
	DDX_Control(pDX, IDC_RADIO13, m_BtnDonatorRank[12]);
	DDX_Control(pDX, IDC_RADIO14, m_BtnDonatorRank[13]);
	DDX_Control(pDX, IDC_RADIO15, m_BtnDonatorRank[14]);
	DDX_Control(pDX, IDC_RADIO16, m_BtnDonatorRank[15]);
	DDX_Control(pDX, IDC_RADIO17, m_BtnDonatorRank[16]);
	DDX_Control(pDX, IDC_RADIO18, m_BtnDonatorRank[17]);
	DDX_Control(pDX, IDC_RADIO19, m_BtnDonatorRank[18]);
	DDX_Control(pDX, IDC_RADIO20, m_BtnDonatorRank[19]);
	DDX_Control(pDX, IDC_RADIO21, m_BtnDonatorRank[20]);
	DDX_Control(pDX, IDC_RADIO22, m_BtnDonatorRank[21]);
	DDX_Control(pDX, IDC_RADIO23, m_BtnDonatorRank[22]);
	DDX_Control(pDX, IDC_RADIO24, m_BtnDonatorRank[23]);
	DDX_Control(pDX, IDC_RADIO25, m_BtnDonatorRank[24]);
	DDX_Control(pDX, IDC_RADIO26, m_BtnDonatorRank[25]);
	DDX_Control(pDX, IDC_RADIO27, m_BtnDonatorRank[26]);
	DDX_Control(pDX, IDC_RADIO28, m_BtnDonatorRank[27]);
	DDX_Control(pDX, IDC_RADIO29, m_BtnDonatorRank[28]);
	DDX_Control(pDX, IDC_RADIO30, m_BtnDonatorRank[29]);
	DDX_Control(pDX, IDC_RADIO31, m_BtnDonatorRank[30]);
	DDX_Control(pDX, IDC_RADIO32, m_BtnDonatorRank[31]);
	DDX_Control(pDX, IDC_RADIO33, m_BtnDonatorRank[32]);
	DDX_Control(pDX, IDC_RADIO34, m_BtnDonatorRank[33]);
	DDX_Control(pDX, IDC_RADIO35, m_BtnDonatorRank[34]);
	DDX_Control(pDX, IDC_RADIO36, m_BtnDonatorRank[35]);
	DDX_Control(pDX, IDC_RADIO37, m_BtnDonatorRank[36]);
	DDX_Control(pDX, IDC_RADIO38, m_BtnDonatorRank[37]);
	DDX_Control(pDX, IDC_RADIO39, m_BtnDonatorRank[38]);
	DDX_Control(pDX, IDC_RADIO40, m_BtnDonatorRank[39]);
	DDX_Control(pDX, IDC_RADIO41, m_BtnDonatorRank[40]);
	DDX_Control(pDX, IDC_RADIO42, m_BtnDonatorRank[41]);
	DDX_Control(pDX, IDC_RADIO43, m_BtnDonatorRank[42]);
	DDX_Control(pDX, IDC_RADIO44, m_BtnDonatorRank[43]);

	DDX_Control(pDX, IDC_SLIDER1, m_EXPSlider);
	DDX_CBIndex(pDX, IDC_COMBO1, m_nColoredName);
}
void CCharacterInfoDlg::OnRadioBtn(UINT radioID)
{
	OnCbnSelchangeCombo1();
}
BEGIN_MESSAGE_MAP(CCharacterInfoDlg, CDialog)
	//{{AFX_MSG_MAP(CCharacterInfoDlg)
	ON_BN_CLICKED(IDOK, OnOk)
	ON_BN_CLICKED(IDC_CHECK_SET_INFL_LEADER, OnCheckSetInflLeader)
	ON_BN_CLICKED(IDC_CHECKSubLeader, OnCHECKSubLeader)
	ON_BN_CLICKED(IDC_CHECKInflSub2Leader, OnCHECKInflSub2Leader)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_INIT_INFLUENCE, OnBtnInitInfluence)
	ON_WM_HSCROLL()
	ON_CONTROL_RANGE( BN_CLICKED, IDC_RADIO1, IDC_RADIO44, OnRadioBtn )
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON2, &CCharacterInfoDlg::OnBnFixQuests)
	ON_EN_UPDATE(IDC_EDIT_EXP, &CCharacterInfoDlg::OnEnUpdateEditExp)
	ON_EN_UPDATE(IDC_EDIT_LEVEL, &CCharacterInfoDlg::OnEnUpdateEditLevel)
	ON_BN_CLICKED(IDC_BUTTON1, &CCharacterInfoDlg::OnBnClickedButton1)
	ON_CBN_SELCHANGE(IDC_COMBO1, &CCharacterInfoDlg::OnCbnSelchangeCombo1)
	ON_BN_CLICKED(IDC_BUTTON8, &CCharacterInfoDlg::OnBnClickedButton8)
	ON_BN_CLICKED(IDC_BUTTON9, &CCharacterInfoDlg::OnBnClickedButton9)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCharacterInfoDlg message handlers
void CCharacterInfoDlg::Info2Character(CHARACTER *o_pCharac)
{
	STRNCPY_MEMSET(o_pCharac->CharacterName, m_CharacterName, SIZE_MAX_CHARACTER_NAME);
	o_pCharac->DP							= m_DP;
	o_pCharac->CurrentDP					= m_CurDP;
	o_pCharac->HP							= m_HP;
	o_pCharac->CurrentHP					= m_CurHP;
	o_pCharac->SP							= m_SP;
	o_pCharac->CurrentSP					= m_CurSP;
	o_pCharac->EP							= m_EP;
	o_pCharac->CurrentEP					= m_CurEP;
// 2005-04-21 by cmkwon, 퍼센트지 설정에서 경험치를 직접 설정으로 변경
//	o_pCharac->Experience					= CAtumSJ::GetExperienceFromPercentage(m_ExpPercentage, m_Level);
	o_pCharac->Experience					= m_ctl_doExp;
	o_pCharac->Level						= m_Level;
	o_pCharac->MapChannelIndex.MapIndex		= m_MapIndex;
	o_pCharac->MapChannelIndex.ChannelIndex	= m_nChannelIndex;
	o_pCharac->GearStat.AttackPart			= m_PartAttack;
	o_pCharac->GearStat.DefensePart			= m_PartDefense;
	o_pCharac->GearStat.FuelPart			= m_PartFuel;
	o_pCharac->GearStat.SoulPart			= m_PartSoul;
	o_pCharac->GearStat.ShieldPart			= m_PartShield;
	o_pCharac->GearStat.DodgePart			= m_PartDodge;
	o_pCharac->BonusStat					= m_BonusStat;
	o_pCharac->BonusStatPoint				= m_BonusStatPoint;			// 2007-01-11 by cmkwon
	o_pCharac->PositionVector.x				= m_PosX;
	o_pCharac->PositionVector.z				= m_PosZ;
	o_pCharac->Race							= (m_Race&(~RACE_ACCOUNT_TYPE_MASK)) | (m_RaceAccType&RACE_ACCOUNT_TYPE_MASK);
	o_pCharac->InfluenceType				= m_byInfluenceType;		// 2005-12-13 by cmkwon
	o_pCharac->Propensity					= m_Propensity;
	o_pCharac->RacingPoint					= m_RacingPoint;
	//////////////////////////////////////////////////////////////////////////
	// 2007-06-08 by dhjin, 아레나 정보와 PCBangTotal시간 추가됨
	o_pCharac->PCBangTotalPlayTime			= m_PCBangTotalPlayTime;
	o_pCharac->WarPoint						= m_ArenaWP;
	o_pCharac->CumulativeWarPoint			= m_ArenaCWP;
	o_pCharac->ArenaWin						= m_ArenaWin;
	o_pCharac->ArenaLose					= m_ArenaLose;
	o_pCharac->ArenaDisConnect				= m_ArenaDisConnect;

}

void CCharacterInfoDlg::Character2Info(CHARACTER *i_pCharac, BOOL i_bInflLeader, BOOL i_bInflSub1Leader, BOOL i_bInflSub2Leader,ITEM &iCPU)
{
	m_CharacterName			= i_pCharac->CharacterName;
	m_DP					= i_pCharac->DP;
	m_CurDP					= i_pCharac->CurrentDP;
	m_HP					= i_pCharac->HP;
	m_CurHP					= i_pCharac->CurrentHP;
	m_SP					= i_pCharac->SP;
	m_CurSP					= i_pCharac->CurrentSP;
	m_EP					= i_pCharac->EP;
	m_CurEP					= i_pCharac->CurrentEP;
	m_ctl_doExp				= i_pCharac->Experience;
	m_ExpPercentage			= CAtumSJ::GetPercentageOfExperience(i_pCharac->Experience, i_pCharac->Level);
	m_Level					= i_pCharac->Level;
	m_MapIndex				= i_pCharac->MapChannelIndex.MapIndex;
	m_nChannelIndex			= i_pCharac->MapChannelIndex.ChannelIndex;
	m_PartAttack			= i_pCharac->GearStat.AttackPart;
	m_PartDefense			= i_pCharac->GearStat.DefensePart;
	m_PartFuel				= i_pCharac->GearStat.FuelPart;
	m_PartSoul				= i_pCharac->GearStat.SoulPart;
	m_PartShield			= i_pCharac->GearStat.ShieldPart;
	m_PartDodge				= i_pCharac->GearStat.DodgePart;
	m_BonusStat				= i_pCharac->BonusStat;
	m_BonusStatPoint		= i_pCharac->BonusStatPoint;			// 2007-01-11 by cmkwon
	m_PosX					= i_pCharac->PositionVector.x;
	m_PosZ					= i_pCharac->PositionVector.z;
	m_Race					= i_pCharac->Race&(~RACE_ACCOUNT_TYPE_MASK);
	m_RaceAccType			= i_pCharac->Race&RACE_ACCOUNT_TYPE_MASK;
	m_Propensity			= i_pCharac->Propensity;
	m_RacingPoint			= i_pCharac->RacingPoint;
	m_byInfluenceType		= i_pCharac->InfluenceType;						// 2005-12-13 by cmkwon
	
	///////////////////////////////////////////////////////////////////////////////
	// 2006-05-03 by cmkwon
	m_ctl_bInflLeader		= i_bInflLeader;
	m_ctl_bInflSubLeader	= i_bInflSub1Leader;
	m_ctl_bInflSub2Leader	= i_bInflSub2Leader;
	m_CharacterUID			= i_pCharac->CharacterUniqueNumber;
	m_AccountUID			= i_pCharac->AccountUniqueNumber;
	m_usUnitKind			= i_pCharac->UnitKind;					// 2007-01-11 by cmkwon
	//////////////////////////////////////////////////////////////////////////
	// 2007-06-08 by dhjin, 아레나 정보와 PCBangTotal시간 추가됨
	m_PCBangTotalPlayTime	= i_pCharac->PCBangTotalPlayTime;
	m_ArenaWP				= i_pCharac->WarPoint;
	m_ArenaCWP				= i_pCharac->CumulativeWarPoint;
	m_ArenaWin				= i_pCharac->ArenaWin;
	m_ArenaLose				= i_pCharac->ArenaLose;
	m_ArenaDisConnect		= i_pCharac->ArenaDisConnect;

	m_csAccountName			= i_pCharac->AccountName;			// 2009-03-31 by cmkwon, 세력초기화 시스템 구현 - 
	itmCPU = iCPU;
}


///////////////////////////////////////////////////////////////////////////////
/// \fn			BOOL CCharacterInfoDlg::StoreInflLeader(BYTE i_byInflTy, UID32_t i_LeaderCharUID)
/// \brief		
/// \author		cmkwon
/// \date		2006-05-03 ~ 2006-05-03
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CCharacterInfoDlg::StoreInflLeader(BYTE i_byInflTy, UID32_t i_LeaderCharUID)
{
	CString szQuery;
// 2008-07-02 by cmkwon, MySQL 지원 구현 - 
// 	szQuery.Format("UPDATE td_InfluenceWarData \
// 		SET InflLeaderCharacterUID = %d \
// 		WHERE InfluenceType = %d", i_LeaderCharUID, i_byInflTy);

// 2008-08-27 by cmkwon, 날쿼리를 Procedure로 수정 - 
//	szQuery.Format(QUERY_080702_0004, i_LeaderCharUID, i_byInflTy);
//	BOOL bRet = m_pODBCStmt3->ExecuteQuery(szQuery);
	SQLHSTMT hstmt = m_pODBCStmt3->GetSTMTHandle();
	SQLINTEGER arrCB2[3] = {SQL_NTS,SQL_NTS,SQL_NTS};
	SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_UTINYINT, SQL_TINYINT, 0, 0, &i_byInflTy, 0,			&arrCB2[1]);
	SQLBindParameter(hstmt, 2, SQL_PARAM_INPUT, SQL_C_ULONG, SQL_INTEGER, 0, 0, &i_LeaderCharUID, 0,		&arrCB2[2]);
	BOOL bRet = m_pODBCStmt3->ExecuteQuery((char*)(PROCEDURE_080827_0004));
	if (!bRet)
	{
		MessageBox("DB Execute Error !!(CCharacterInfoDlg::StoreInflLeader_)", "Error", MB_OK | MB_ICONERROR);
		m_pODBCStmt3->FreeStatement();
		return FALSE;
	}
	m_pODBCStmt3->FreeStatement();				// free statement	

	/*[Stored Query Definition]************************************************
	--------------------------------------------------------------------------------
	-- TABLE NAME: dbo.atum_InsertLeaderItem
	-- DESC      : 2007-02-13 by dhjin, 지도자 전용 아이템 추가하는 프로시저
	--				7001320	지도자의 광휘(바이제니유)
	--				7001330	지도자의 광휘(알링턴)
	--				7001090	세력의 가호
	--				7001110	성장의 시간
	--------------------------------------------------------------------------------
	**************************************************************************/
	if (i_LeaderCharUID != 0)
	{// 2007-02-13 by dhjin, 지도자 체크란에 체크가 되어야만 업데이트 진행
		RETCODE	ret;
		
		SQLBindParameter(m_pODBCStmt3->m_hstmt, 1, SQL_PARAM_INPUT, SQL_C_UTINYINT, SQL_TINYINT, 0, 0, &i_byInflTy, 0, NULL);
		SQLBindParameter(m_pODBCStmt3->m_hstmt, 2, SQL_PARAM_INPUT, SQL_C_ULONG, SQL_INTEGER, 0, 0, &m_AccountUID, 0, NULL);
		SQLBindParameter(m_pODBCStmt3->m_hstmt, 3, SQL_PARAM_INPUT, SQL_C_ULONG, SQL_INTEGER, 0, 0, &i_LeaderCharUID, 0, NULL);

		ret = SQLExecDirect(m_pODBCStmt3->m_hstmt, PROCEDURE_080822_0001, SQL_NTS);
		if(ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO && ret != SQL_NO_DATA)
		{
			MessageBox("DB Execute Error !!(CCharacterInfoDlg::InsertLeaderItem_)", "Error", MB_OK | MB_ICONERROR);
			m_pODBCStmt3->FreeStatement();
			return FALSE;
		}

		m_pODBCStmt3->FreeStatement();	
	}

	/*[Stored Query Definition]************************************************
	--------------------------------------------------------------------------------
	-- PROCEDURE NAME: dbo.atum_UpdateCityLeaderByLeaderSet
	-- DESC			 : 지도자 설정 시 td_CityLeaderInfo에 정보 설정
	-- 2007-09-14 by dhjin,
	--------------------------------------------------------------------------------
	CREATE PROCEDURE dbo.atum_UpdateCityLeaderByLeaderSet
		@i_Infltype				TINYINT,
		@i_Possess				INT
	AS
		DECLARE @CountInfl INT
		SET @CountInfl = (SELECT count(*) FROM dbo.td_CityLeaderInfo WHERE Influence = @i_Infltype)

		IF @i_Infltype = 2 -- 바이제니유
		BEGIN
			UPDATE dbo.td_CityLeaderInfo
			SET	Influence = 2, CharacterUID = @i_Possess, ExpediencyFundCumulative = 0, ExpediencyFund = 0, ExpediencyFundRate = 10 + (@CountInfl * 5), Notice = NULL
			WHERE MapIndex = 2001
		END

		ELSE IF @i_Infltype = 4 -- 알링턴
		BEGIN
			UPDATE dbo.td_CityLeaderInfo
			SET	Influence = 4, CharacterUID = @i_Possess, ExpediencyFundCumulative = 0, ExpediencyFund = 0, ExpediencyFundRate = 10 + (@CountInfl * 5), Notice = NULL
			WHERE MapIndex = 2002
		END
	GO
	**************************************************************************/	
	if (i_LeaderCharUID != 0)
	{// 2007-02-13 by dhjin, 지도자 체크란에 체크가 되어야만 업데이트 진행
		RETCODE	ret;
		
		SQLBindParameter(m_pODBCStmt3->m_hstmt, 1, SQL_PARAM_INPUT, SQL_C_UTINYINT, SQL_TINYINT, 0, 0, &i_byInflTy, 0, NULL);
		SQLBindParameter(m_pODBCStmt3->m_hstmt, 2, SQL_PARAM_INPUT, SQL_C_ULONG, SQL_INTEGER, 0, 0, &i_LeaderCharUID, 0, NULL);

		ret = SQLExecDirect(m_pODBCStmt3->m_hstmt, PROCEDURE_080822_0002, SQL_NTS);
		if(ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO && ret != SQL_NO_DATA)
		{
			MessageBox("DB Execute Error !!(CCharacterInfoDlg::atum_UpdateCityLeaderByLeaderSet)", "Error", MB_OK | MB_ICONERROR);
			m_pODBCStmt3->FreeStatement();
			return FALSE;
		}

		m_pODBCStmt3->FreeStatement();	
	}

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			BOOL CCharacterInfoDlg::StoreInflSubLeader(BYTE i_byInflTy, UID32_t i_SubLeaderCharUID)
/// \brief		
/// \author		dhjin
/// \date		2006-12-08 ~ 2006-12-08
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CCharacterInfoDlg::StoreInflSubLeader(BYTE i_byInflTy, UID32_t i_SubLeaderCharUID)
{
	CString szQuery;
// 2008-07-02 by cmkwon, MySQL 지원 구현 - 
// 	szQuery.Format("UPDATE td_InfluenceWarData \
// 		SET InflSub1LeaderCharacterUID = %d \
// 		WHERE InfluenceType = %d", i_SubLeaderCharUID, i_byInflTy);

// 2008-08-27 by cmkwon, 날쿼리를 Procedure로 수정 - 
//	szQuery.Format(QUERY_080702_0005, i_SubLeaderCharUID, i_byInflTy);
//	BOOL bRet = m_pODBCStmt3->ExecuteQuery(szQuery);
	SQLHSTMT hstmt = m_pODBCStmt3->GetSTMTHandle();
	SQLINTEGER arrCB2[3] = {SQL_NTS,SQL_NTS,SQL_NTS};
	SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_UTINYINT, SQL_TINYINT, 0, 0, &i_byInflTy, 0,			&arrCB2[1]);
	SQLBindParameter(hstmt, 2, SQL_PARAM_INPUT, SQL_C_ULONG, SQL_INTEGER, 0, 0, &i_SubLeaderCharUID, 0,		&arrCB2[2]);
	BOOL bRet = m_pODBCStmt3->ExecuteQuery((char*)(PROCEDURE_080827_0005));
	if (!bRet)
	{
		MessageBox("DB Execute Error !!(CCharacterInfoDlg::StoreInflSubLeader_)", "Error", MB_OK | MB_ICONERROR);
		m_pODBCStmt3->FreeStatement();
		return FALSE;
	}
	m_pODBCStmt3->FreeStatement();				// free statement	
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			BOOL CCharacterInfoDlg::StoreInflSub2Leader(BYTE i_byInflTy, UID32_t i_Sub2LeaderCharUID)
/// \brief		
/// \author		dhjin
/// \date		2006-12-08 ~ 2006-12-08
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CCharacterInfoDlg::StoreInflSub2Leader(BYTE i_byInflTy, UID32_t i_Sub2LeaderCharUID)
{	
	CString szQuery;
// 2008-07-02 by cmkwon, MySQL 지원 구현 - 
// 	szQuery.Format("UPDATE td_InfluenceWarData \
// 		SET InflSub2LeaderCharacterUID = %d \
// 		WHERE InfluenceType = %d", i_Sub2LeaderCharUID, i_byInflTy);

// 2008-08-27 by cmkwon, 날쿼리를 Procedure로 수정 - 
//	szQuery.Format(QUERY_080702_0006, i_Sub2LeaderCharUID, i_byInflTy);
//	BOOL bRet = m_pODBCStmt3->ExecuteQuery(szQuery);
	SQLHSTMT hstmt = m_pODBCStmt3->GetSTMTHandle();
	SQLINTEGER arrCB2[3] = {SQL_NTS,SQL_NTS,SQL_NTS};
	SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_UTINYINT, SQL_TINYINT, 0, 0, &i_byInflTy, 0,			&arrCB2[1]);
	SQLBindParameter(hstmt, 2, SQL_PARAM_INPUT, SQL_C_ULONG, SQL_INTEGER, 0, 0, &i_Sub2LeaderCharUID, 0,	&arrCB2[2]);
	BOOL bRet = m_pODBCStmt3->ExecuteQuery((char*)(PROCEDURE_080827_0006));
	if (!bRet)
	{
		MessageBox("DB Execute Error !!(CCharacterInfoDlg::StoreInflSubLeader_)", "Error", MB_OK | MB_ICONERROR);
		m_pODBCStmt3->FreeStatement();
		return FALSE;
	}
	m_pODBCStmt3->FreeStatement();				// free statement	
	return TRUE;
}

#define RACE_COMBO_BATTALUS		0 // (바탈러스)
#define RACE_COMBO_DECA			1 // (데카)
#define RACE_COMBO_PHILON		2 // (필론)
#define RACE_COMBO_SHARRINE		3 // (샤린)

#define RACE_ACC_TYPE_COMBO_NO_TYPE			0 // (일반)
#define RACE_ACC_TYPE_COMBO_OPERATION		1 // (관리자)
#define RACE_ACC_TYPE_COMBO_GAMEMASTER		2 // (게임마스터)
#define RACE_ACC_TYPE_COMBO_MONITOR			3 // (모니터)
#define RACE_ACC_TYPE_COMBO_GUEST			4 // (게스트)
#define RACE_ACC_TYPE_COMBO_DEMO			5 // (데모)

///////////////////////////////////////////////////////////////////////////////
// 2005-12-13 by cmkwon
#define COMBOIndex_INFLUENCETypeNormal		0 // (바이제니유-일반)
#define COMBOIndex_INFLUENCETypeVCN			1 // (바이제니유-정규군)
#define COMBOIndex_INFLUENCETypeANI			2 // (바이제니유-반란군)
#define COMBOIndex_INFLUENCETypeGM			3 // (바이제니유-반란군)
#define COMBOIndex_INFLUENCETypeRRP			4 // (바탈러스-연방군)
#define COMBOIndex_INFLUENCETypeUnknown		5 // (알수없음)

BOOL CCharacterInfoDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_EXPSlider.SetRange(0, 999);
	m_EXPSlider.SetPos(m_ExpPercentage * 10);
	// TODO: Add extra initialization here
	CComboBox *pComboRace = (CComboBox*)GetDlgItem(IDC_COMBO_RACE);

	///////////////////////////////////////////////////////////////////////////////
	// 2008-09-02 by cmkwon, AdminTool에서 계정 권한 설정을 번역 한것을 사용함.
	pComboRace->AddString(STRCMD_CS_COMMON_RACE_BATTALUS);
	pComboRace->AddString(STRCMD_CS_COMMON_RACE_DECA);
	pComboRace->AddString(STRCMD_CS_COMMON_RACE_PHILON);
	pComboRace->AddString(STRCMD_CS_COMMON_RACE_SHARRINE);

	if (m_Race == RACE_BATTALUS) pComboRace->SetCurSel(RACE_COMBO_BATTALUS);
	else if (m_Race == RACE_DECA) pComboRace->SetCurSel(RACE_COMBO_DECA);
	else if (m_Race == RACE_PHILON) pComboRace->SetCurSel(RACE_COMBO_PHILON);
	else if (m_Race == RACE_SHARRINE) pComboRace->SetCurSel(RACE_COMBO_SHARRINE);

	CComboBox *pComboRaceAccType = (CComboBox*)GetDlgItem(IDC_COMBO_RACE_ACC_TYPE);

	///////////////////////////////////////////////////////////////////////////////
	// 2008-09-02 by cmkwon, AdminTool에서 계정 권한 설정을 번역 한것을 사용함.
	pComboRaceAccType->AddString(STRCMD_CS_COMMON_RACE_NORMAL);
	pComboRaceAccType->AddString(STRCMD_CS_COMMON_RACE_OPERATION);
	pComboRaceAccType->AddString(STRCMD_CS_COMMON_RACE_GAMEMASTER);
	pComboRaceAccType->AddString(STRCMD_CS_COMMON_RACE_MONITOR);
	pComboRaceAccType->AddString(STRCMD_CS_COMMON_RACE_GUEST);
	pComboRaceAccType->AddString(STRCMD_CS_COMMON_RACE_DEMO);

	if (m_RaceAccType == 0) pComboRaceAccType->SetCurSel(RACE_ACC_TYPE_COMBO_NO_TYPE);
	else if (m_RaceAccType == RACE_OPERATION) pComboRaceAccType->SetCurSel(RACE_ACC_TYPE_COMBO_OPERATION);
	else if (m_RaceAccType == RACE_GAMEMASTER) pComboRaceAccType->SetCurSel(RACE_ACC_TYPE_COMBO_GAMEMASTER);
	else if (m_RaceAccType == RACE_MONITOR) pComboRaceAccType->SetCurSel(RACE_ACC_TYPE_COMBO_MONITOR);
	else if (m_RaceAccType == RACE_GUEST) pComboRaceAccType->SetCurSel(RACE_ACC_TYPE_COMBO_GUEST);
	else if (m_RaceAccType == RACE_DEMO) pComboRaceAccType->SetCurSel(RACE_ACC_TYPE_COMBO_DEMO);

	CComboBox *pComboInfluenceType = (CComboBox*)GetDlgItem(IDC_COMBO_INFLUENCETYPE);
	pComboInfluenceType->AddString(/*CAtumSJ::GetInfluenceTypeString(INFLUENCE_TYPE_NORMAL)*/"FreeSKA");
	pComboInfluenceType->AddString(/*CAtumSJ::GetInfluenceTypeString(INFLUENCE_TYPE_VCN)*/"BCU");
	pComboInfluenceType->AddString(/*CAtumSJ::GetInfluenceTypeString(INFLUENCE_TYPE_ANI)*/"ANI");
	pComboInfluenceType->AddString(/*CAtumSJ::GetInfluenceTypeString(INFLUENCE_TYPE_RRP)*/"STAFF");
	pComboInfluenceType->AddString(/*CAtumSJ::GetInfluenceTypeString(INFLUENCE_TYPE_RRP)*/"RRP");
	pComboInfluenceType->AddString(/*CAtumSJ::GetInfluenceTypeString(INFLUENCE_TYPE_UNKNOWN)*/"Unknown");
	if(COMPARE_INFLUENCE(m_byInfluenceType, INFLUENCE_TYPE_NORMAL)){	pComboInfluenceType->SetCurSel(COMBOIndex_INFLUENCETypeNormal); nDestInfluence=1;}
	else if(COMPARE_INFLUENCE(m_byInfluenceType, INFLUENCE_TYPE_VCN)){	pComboInfluenceType->SetCurSel(COMBOIndex_INFLUENCETypeVCN); nDestInfluence = 2;
	}
	else if(COMPARE_INFLUENCE(m_byInfluenceType, INFLUENCE_TYPE_ANI)){	pComboInfluenceType->SetCurSel(COMBOIndex_INFLUENCETypeANI); nDestInfluence = 4;
	}
	else if(COMPARE_INFLUENCE(m_byInfluenceType, INFLUENCE_TYPE_GM)){	pComboInfluenceType->SetCurSel(COMBOIndex_INFLUENCETypeGM); nDestInfluence = 1;
	}
	else if(COMPARE_INFLUENCE(m_byInfluenceType, INFLUENCE_TYPE_RRP)){	pComboInfluenceType->SetCurSel(COMBOIndex_INFLUENCETypeRRP); nDestInfluence = 1;
	}
	else {																pComboInfluenceType->SetCurSel(COMBOIndex_INFLUENCETypeUnknown); nDestInfluence = 1;
	}

	if(INFLUENCE_TYPE_VCN != m_byInfluenceType
		&& INFLUENCE_TYPE_ANI != m_byInfluenceType)
	{// 2006-05-03 by cmkwon, 일반군은 세력리더로 설정 불가
		GetDlgItem(IDC_CHECK_SET_INFL_LEADER)->EnableWindow(FALSE);
	}
	if(m_ctl_bInflLeader
		|| m_ctl_bInflSubLeader
		|| m_ctl_bInflSub2Leader)	// 2006-12-08 by dhjin, 부지도자
	{// 2006-05-03 by cmkwon, 세력리더는 세력 변경 불가
		pComboInfluenceType->EnableWindow(FALSE);
	}


	if(FALSE == m_bEnableEdit)
	{
		GetDlgItem(IDC_EDIT_CHARACTER_NAME)->EnableWindow(m_bEnableEdit);
		GetDlgItem(IDC_EDIT_MAP_INDEX)->EnableWindow(m_bEnableEdit);
		GetDlgItem(IDC_EDIT_CHANNEL_INDEX)->EnableWindow(m_bEnableEdit);
		GetDlgItem(IDC_EDIT_POS_X)->EnableWindow(m_bEnableEdit);
		GetDlgItem(IDC_EDIT_POS_Z)->EnableWindow(m_bEnableEdit);
		GetDlgItem(IDC_COMBO_RACE)->EnableWindow(m_bEnableEdit);
		GetDlgItem(IDC_COMBO_RACE_ACC_TYPE)->EnableWindow(m_bEnableEdit);
		GetDlgItem(IDC_COMBO_INFLUENCETYPE)->EnableWindow(m_bEnableEdit);
		GetDlgItem(IDC_EDIT_LEVEL)->EnableWindow(m_bEnableEdit);
		GetDlgItem(IDC_EDIT_EXP)->EnableWindow(m_bEnableEdit);
		GetDlgItem(IDC_EDIT_HP_CUR)->EnableWindow(m_bEnableEdit);
		GetDlgItem(IDC_EDIT_HP)->EnableWindow(m_bEnableEdit);
		GetDlgItem(IDC_EDIT_DP_CUR)->EnableWindow(m_bEnableEdit);
		GetDlgItem(IDC_EDIT_DP)->EnableWindow(m_bEnableEdit);
		GetDlgItem(IDC_EDIT_SP_CUR)->EnableWindow(m_bEnableEdit);
		GetDlgItem(IDC_EDIT_SP)->EnableWindow(m_bEnableEdit);
		GetDlgItem(IDC_EDIT_EP_CUR)->EnableWindow(m_bEnableEdit);
		GetDlgItem(IDC_EDIT_EP)->EnableWindow(m_bEnableEdit);
		GetDlgItem(IDC_EDIT_PART_ATTACK)->EnableWindow(m_bEnableEdit);
		GetDlgItem(IDC_EDIT_PART_DEFENSE)->EnableWindow(m_bEnableEdit);
		GetDlgItem(IDC_EDIT_PART_SOUL)->EnableWindow(m_bEnableEdit);
		GetDlgItem(IDC_EDIT_PART_SHIELD)->EnableWindow(m_bEnableEdit);
		GetDlgItem(IDC_EDIT_PART_DODGE)->EnableWindow(m_bEnableEdit);
		GetDlgItem(IDC_EDIT_PART_FUEL)->EnableWindow(m_bEnableEdit);
		GetDlgItem(IDC_EDIT_BONUS_STAT)->EnableWindow(m_bEnableEdit);
		GetDlgItem(IDC_EDIT_BONUS_STAT_POINT)->EnableWindow(m_bEnableEdit);
		GetDlgItem(IDC_EDIT_PROPENSITY)->EnableWindow(m_bEnableEdit);
		GetDlgItem(IDC_EDIT_RACING_POINT)->EnableWindow(m_bEnableEdit);
		GetDlgItem(IDC_CHECK_SET_INFL_LEADER)->EnableWindow(FALSE);				// 2006-05-04 by cmkwon
		//////////////////////////////////////////////////////////////////////////
		// 2007-06-08 by dhjin, 아레나 정보와 PCBangTotal시간 추가됨		
		GetDlgItem(IDC_EDIT_PCBangTotalPlayTime)->EnableWindow(m_bEnableEdit);
		GetDlgItem(IDC_EDIT_ARENA_CWP)->EnableWindow(m_bEnableEdit);
		GetDlgItem(IDC_EDIT_ARENA_DISCONNECT)->EnableWindow(m_bEnableEdit);
		GetDlgItem(IDC_EDIT_ARENA_LOSE)->EnableWindow(m_bEnableEdit);
		GetDlgItem(IDC_EDIT_ARENA_WIN)->EnableWindow(m_bEnableEdit);
		GetDlgItem(IDC_EDIT_ARENA_WP)->EnableWindow(m_bEnableEdit);

		GetDlgItem(IDC_CHECKSubLeader)->EnableWindow(m_bEnableEdit);		// 2007-10-02 by cmkwon, SCAdminTool 수정 권한 처리 - 캐릭터 정보창
		GetDlgItem(IDC_CHECKInflSub2Leader)->EnableWindow(m_bEnableEdit);	// 2007-10-02 by cmkwon, SCAdminTool 수정 권한 처리 - 캐릭터 정보창

		GetDlgItem(IDC_BTN_INIT_INFLUENCE)->EnableWindow(m_bEnableEdit);	// 2009-03-31 by cmkwon, 세력초기화 시스템 구현 - 
	}

	// 2009-03-31 by cmkwon, 세력초기화 시스템 구현 - 세력변경은 불가능하고 이제 해당 계정의 세력 초기화만 가능하다
	GetDlgItem(IDC_COMBO_INFLUENCETYPE)->EnableWindow(TRUE);

	m_pColoredNameCtrl = (CComboBox*)GetDlgItem(IDC_COMBO1);
	m_pColoredNameCtrl->ResetContent();
	m_pColoredNameCtrl->AddString("predefined colors");
	m_pColoredNameCtrl->AddString("violet RGB(144, 67, 210)");//1000
	m_pColoredNameCtrl->AddString("ltblue RGB(67, 154, 210)");
	m_pColoredNameCtrl->AddString("green RGB(117, 200, 35)");
	m_pColoredNameCtrl->AddString("dev green RGB(65, 232, 55)");
	m_pColoredNameCtrl->AddString("orange RGB(232, 155, 55)");
	m_pColoredNameCtrl->AddString("red owner RGB(255, 0, 0)");
	m_pColoredNameCtrl->AddString("staff RGB(150, 127, 127)");//7000

	m_pColoredNameCtrl->SetCurSel(0);

	if (m_PCBangTotalPlayTime > 1000)
	{
		CAtumSJ::DRankColorRet color = CAtumSJ::GetDRankColor(m_PCBangTotalPlayTime);
		if (m_BtnDonatorRank[color.nRankNum])
			m_BtnDonatorRank[color.nRankNum].SetCheck(1);

		char szTmp[128];
		RGBTRIPLE rgb;
		rgb.rgbtRed = GetRValue(color.color);
		rgb.rgbtGreen = GetGValue(color.color);
		rgb.rgbtBlue = GetBValue(color.color);
		sprintf(szTmp, "%03d,%03d,%03d", rgb.rgbtRed, rgb.rgbtGreen, rgb.rgbtBlue);
		GetDlgItem(IDC_EDIT3)->SetWindowText(szTmp);
	}
	else
	{
		if (m_BtnDonatorRank[m_PCBangTotalPlayTime])
			m_BtnDonatorRank[m_PCBangTotalPlayTime].SetCheck(1);
	}


	int nAttDivider, nDefDivider, nEvaDivider, nFuelDivider, nSoulDivider, nShieldDivider = 1;

	switch (m_usUnitKind)
	{
	case 1:
	{
		nAttDivider = 3;
		nDefDivider = 3;
		nFuelDivider = 3;
		nSoulDivider = 3;
		nShieldDivider = 3;
		nEvaDivider = 3;
	}
	break;
	case 16:
	{
		nAttDivider = 2;
		nDefDivider = 4;
		nFuelDivider = 3;
		nSoulDivider = 4;
		nShieldDivider = 3;
		nEvaDivider = 2;
	}
	break;
	case 256:
	{
		nAttDivider = 4;
		nDefDivider = 3;
		nFuelDivider = 3;
		nSoulDivider = 3;
		nShieldDivider = 4;
		nEvaDivider = 1;
	}
	break;
	case 4096:
	{
		nAttDivider = 4;
		nDefDivider = 2;
		nFuelDivider = 3;
		nSoulDivider = 3;
		nShieldDivider = 2;
		nEvaDivider = 4;
	}
	break;
	}
	char szTmpStat[20]; *szTmpStat ='\0';
	sprintf(szTmpStat,"%dx%d+%.f", nAttDivider, m_PartAttack/nAttDivider-1, itmCPU.GetParameterValue(DES_ATTACK_PART));
	GetDlgItem(IDC_EDIT_PART_ATTACK2)->SetWindowTextA(szTmpStat);
	sprintf(szTmpStat, "%dx%d+%.f", nDefDivider, m_PartDefense/ nDefDivider-1, itmCPU.GetParameterValue(DES_DEFENSE_PART));
	GetDlgItem(IDC_EDIT_PART_ATTACK3)->SetWindowTextA(szTmpStat);
	sprintf(szTmpStat, "%dx%d+%.f", nFuelDivider, m_PartFuel/ nFuelDivider-1, itmCPU.GetParameterValue(DES_FUEL_PART));
	GetDlgItem(IDC_EDIT_PART_ATTACK4)->SetWindowTextA(szTmpStat);
	sprintf(szTmpStat, "%dx%d+%.f", nSoulDivider, m_PartSoul/ nSoulDivider-1, itmCPU.GetParameterValue(DES_SOUL_PART));
	GetDlgItem(IDC_EDIT_PART_ATTACK5)->SetWindowTextA(szTmpStat);
	sprintf(szTmpStat, "%dx%d+%.f", nShieldDivider, m_PartShield/ nShieldDivider-1, itmCPU.GetParameterValue(DES_SHIELD_PART));
	GetDlgItem(IDC_EDIT_PART_ATTACK6)->SetWindowTextA(szTmpStat);
	sprintf(szTmpStat, "%dx%d+%.f", nEvaDivider, m_PartDodge/ nEvaDivider-1, itmCPU.GetParameterValue(DES_DODGE_PART));
	GetDlgItem(IDC_EDIT_PART_ATTACK7)->SetWindowTextA(szTmpStat);

//ingame COUNT_MAX_STAT_POINT
	sprintf(szTmpStat, "%.f", m_PartAttack + itmCPU.GetParameterValue(DES_ATTACK_PART));
	GetDlgItem(IDC_EDIT_PART_ATTACK8)->SetWindowTextA(szTmpStat);
	sprintf(szTmpStat, "%.f", m_PartDefense + itmCPU.GetParameterValue(DES_DEFENSE_PART));
	GetDlgItem(IDC_EDIT_PART_DEFENSE2)->SetWindowTextA(szTmpStat);
	sprintf(szTmpStat, "%.f", m_PartFuel +itmCPU.GetParameterValue(DES_FUEL_PART));
	GetDlgItem(IDC_EDIT_PART_SOUL2)->SetWindowTextA(szTmpStat);
	sprintf(szTmpStat, "%.f", m_PartSoul + itmCPU.GetParameterValue(DES_SOUL_PART));
	GetDlgItem(IDC_EDIT_PART_SHIELD2)->SetWindowTextA(szTmpStat);
	sprintf(szTmpStat, "%.f", m_PartShield + itmCPU.GetParameterValue(DES_SHIELD_PART));
	GetDlgItem(IDC_EDIT_PART_DODGE2)->SetWindowTextA(szTmpStat);
	sprintf(szTmpStat, "%.f", m_PartDodge + itmCPU.GetParameterValue(DES_DODGE_PART));
	GetDlgItem(IDC_EDIT_PART_FUEL2)->SetWindowTextA(szTmpStat);
	memset(szTmpStat,0x00,20);

	

	

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCharacterInfoDlg::OnOk()
{
	// TODO: Add your control notification handler code here
	if (FALSE == m_bEnableEdit)
	{// 2006-04-15 by cmkwon, 수정 권한 체크
		MessageBox("Not enabled edit", "Error", MB_OK | MB_ICONERROR);
		CDialog::OnCancel();
		return;
	}
	UpdateData();

	CComboBox* pComboRace = (CComboBox*)GetDlgItem(IDC_COMBO_RACE);
	CComboBox* pComboRaceAccType = (CComboBox*)GetDlgItem(IDC_COMBO_RACE_ACC_TYPE);
	
	if (pComboRace->GetCurSel() == RACE_COMBO_BATTALUS) m_Race = RACE_BATTALUS;
	else if (pComboRace->GetCurSel() == RACE_COMBO_DECA) m_Race = RACE_DECA;
	else if (pComboRace->GetCurSel() == RACE_COMBO_PHILON) m_Race = RACE_PHILON;
	else if (pComboRace->GetCurSel() == RACE_COMBO_SHARRINE) m_Race = RACE_SHARRINE;
	if (RACE_DECA != m_Race)
	{// 2008-09-02 by cmkwon, 체크 추가
		MessageBox("Invalid Race !!", "Error", MB_OK | MB_ICONERROR);
		return;
	}

	if (pComboRaceAccType->GetCurSel() == RACE_ACC_TYPE_COMBO_NO_TYPE) m_RaceAccType = 0;
	else if (pComboRaceAccType->GetCurSel() == RACE_ACC_TYPE_COMBO_OPERATION) m_RaceAccType = RACE_OPERATION;
	else if (pComboRaceAccType->GetCurSel() == RACE_ACC_TYPE_COMBO_GAMEMASTER) m_RaceAccType = RACE_GAMEMASTER;
	else if (pComboRaceAccType->GetCurSel() == RACE_ACC_TYPE_COMBO_MONITOR) m_RaceAccType = RACE_MONITOR;
	else if (pComboRaceAccType->GetCurSel() == RACE_ACC_TYPE_COMBO_GUEST) m_RaceAccType = RACE_GUEST;
	else if (pComboRaceAccType->GetCurSel() == RACE_ACC_TYPE_COMBO_DEMO) m_RaceAccType = RACE_DEMO;

	///////////////////////////////////////////////////////////////////////////////
	// 2005-12-13 by cmkwon, 세력
	CComboBox* pComboInfluenceTy = (CComboBox*)GetDlgItem(IDC_COMBO_INFLUENCETYPE);
	if (COMBOIndex_INFLUENCETypeNormal == pComboInfluenceTy->GetCurSel()) { m_byInfluenceType = INFLUENCE_TYPE_NORMAL; nDestInfluence = 1;}
	else if (COMBOIndex_INFLUENCETypeVCN == pComboInfluenceTy->GetCurSel()) { m_byInfluenceType = INFLUENCE_TYPE_VCN; nDestInfluence = 2;
	}
	else if (COMBOIndex_INFLUENCETypeANI == pComboInfluenceTy->GetCurSel()) { m_byInfluenceType = INFLUENCE_TYPE_ANI; nDestInfluence = 4;
	}
	else if (COMBOIndex_INFLUENCETypeGM == pComboInfluenceTy->GetCurSel()) { m_byInfluenceType = INFLUENCE_TYPE_GM; nDestInfluence = 1;
	}
	else if (COMBOIndex_INFLUENCETypeRRP == pComboInfluenceTy->GetCurSel()) { m_byInfluenceType = INFLUENCE_TYPE_RRP; nDestInfluence = 1;
	}
	else { m_byInfluenceType = INFLUENCE_TYPE_NORMAL; }

	if (FALSE == IS_VALID_INFLUENCE_TYPE(m_byInfluenceType))
	{// 2008-09-02 by cmkwon, 체크 추가
		MessageBox("Invalid Influence !!", "Error", MB_OK | MB_ICONERROR);
		return;
	}

	Experience_t exp1 = CAtumSJ::GetInitialExperienceOfLevel(m_Level);
	Experience_t exp2 = CAtumSJ::GetInitialExperienceOfLevel(m_Level + 1) - 1;

	// 2010-03-11 by cmkwon, 지원레벨 110 상항 처리 빠진것 수정 - 아래와 같이 최대지원 레벨처리 수정
	if (CHARACTER_LEVEL_110 <= m_Level)
	{
		m_Level = CHARACTER_LEVEL_110;
		m_ctl_doExp = exp1;
		UpdateData(FALSE);
	}
	else if (m_ctl_doExp < exp1
		|| m_ctl_doExp >= exp2)
	{
		char szTemp[512];
		sprintf(szTemp, STRERR_S_SCADMINTOOL_0011
			, m_Level, exp1, exp2);
		MessageBox(szTemp, "Error", MB_OK | MB_ICONERROR);
		return;
	}

	if (m_CharacterName.GetLength() >= SIZE_MAX_CHARACTER_NAME)
	{
		char szTemp[512];
		sprintf(szTemp, "Invalid CharacterName length !!(Max Length: %dBytes)", SIZE_MAX_CHARACTER_NAME);
		MessageBox(szTemp, "Error", MB_OK | MB_ICONERROR);
		return;
	}
	
	CDialog::OnOK();
}

void CCharacterInfoDlg::OnCheckSetInflLeader() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	
	if(INFLUENCE_TYPE_VCN != m_byInfluenceType
		&& INFLUENCE_TYPE_ANI != m_byInfluenceType)
	{
		m_ctl_bInflLeader	= FALSE;
		UpdateData(FALSE);
		return;
	}

	CString str;
	str.Format("IMServer and FieldServer must be shutdown if you want to change Influence Leader setting change.\n\n  IMServer and FieldServer is shutdowned?");
	int nRet = MessageBox(str, "Question", MB_YESNO|MB_ICONQUESTION);
	if(IDNO == nRet)
	{// 2006-05-03 by cmkwon, IMServer와 FieldServer가 종료 상태에만 변경해야 한다.
		m_ctl_bInflLeader	= !m_ctl_bInflLeader;
		UpdateData(FALSE);
		return;
	}

	if(FALSE == m_ctl_bInflLeader)
	{// 2006-05-03 by cmkwon, 세력리더 설정 해제
		str.Format("This character set a \'%s\' Influence Leader.\n\n  Would you cancel this setting? (Will be changed immediately)", CAtumSJ::GetInfluenceTypeString(m_byInfluenceType));
	}
	else
	{// 2006-05-03 by cmkwon, 세력리더로 설정
		str.Format("Would you set this characer to Influence Leader of \'%s\'? (Will be changed immediately)", CAtumSJ::GetInfluenceTypeString(m_byInfluenceType));
	}
	nRet = MessageBox(str, "Question", MB_YESNO|MB_ICONQUESTION);
	if(IDNO == nRet)
	{
		m_ctl_bInflLeader	= !m_ctl_bInflLeader;
		UpdateData(FALSE);
		return;
	}

	m_ctl_bInflSubLeader	= FALSE;
	m_ctl_bInflSub2Leader	= FALSE;
	UpdateData(FALSE);
	StoreInflLeader(m_byInfluenceType, (FALSE==m_ctl_bInflLeader)?0:m_CharacterUID);
}

void CCharacterInfoDlg::OnCHECKSubLeader() 
{
	// TODO: Add your control notification handler code here
	CString str;
	// 2006-12-13 by dhjin, 40레벨 미만은 부지도자가 될 수 없다.
	if (40 > m_Level)
	{
		str.Format("Required Level 40");
		MessageBox(str, "Error", MB_OK|MB_ICONERROR);
		UpdateData(FALSE);
		return;
	}	

	UpdateData();

	if(INFLUENCE_TYPE_VCN != m_byInfluenceType
		&& INFLUENCE_TYPE_ANI != m_byInfluenceType)
	{
		m_ctl_bInflLeader	= FALSE;
		MessageBox("Character in neutral influence!","Error",MB_OK|MB_ICONERROR);
		UpdateData(FALSE);
		return;
	}

//	CString str;
	str.Format("IMServer and FieldServer must be shutdown if you want to change Influence SubLeader setting change.\n\n  IMServer and FieldServer is shutdowned?");
	int nRet = MessageBox(str, "Question", MB_YESNO|MB_ICONQUESTION);
	if(IDNO == nRet)
	{// 2006-05-03 by cmkwon, IMServer와 FieldServer가 종료 상태에만 변경해야 한다.
		m_ctl_bInflSubLeader	= !m_ctl_bInflSubLeader;
		UpdateData(FALSE);
		return;
	}

	if(FALSE == m_ctl_bInflSubLeader)
	{// 2006-05-03 by cmkwon, 세력리더 설정 해제
		str.Format("This character set a \'%s\' Influence SubLeader.\n\n  Would you cancel this setting? (Will be change immediately)", CAtumSJ::GetInfluenceTypeString(m_byInfluenceType));
	}
	else
	{// 2006-05-03 by cmkwon, 세력리더로 설정
		str.Format("Would you set this characer to Influence SubLeader of \'%s\'? (Will be change immediately)", CAtumSJ::GetInfluenceTypeString(m_byInfluenceType));
	}
	nRet = MessageBox(str, "Question", MB_YESNO|MB_ICONQUESTION);
	if(IDNO == nRet)
	{
		m_ctl_bInflSubLeader	= !m_ctl_bInflSubLeader;
		UpdateData(FALSE);
		return;
	}

	m_ctl_bInflLeader	= FALSE;
	m_ctl_bInflSub2Leader	= FALSE;
	UpdateData(FALSE);
	StoreInflSubLeader(m_byInfluenceType, (FALSE==m_ctl_bInflSubLeader)?0:m_CharacterUID);	
}

void CCharacterInfoDlg::OnCHECKInflSub2Leader() 
{
	// TODO: Add your control notification handler code here
	CString str;
	// 2006-12-13 by dhjin, 40레벨 미만은 부지도자가 될 수 없다.
	if (40 > m_Level)
	{
		str.Format("Required Level 40");
		MessageBox(str, "Error", MB_OK|MB_ICONERROR);
		UpdateData(FALSE);
		return;
	}	

	UpdateData();
	
	if(INFLUENCE_TYPE_VCN != m_byInfluenceType
		&& INFLUENCE_TYPE_ANI != m_byInfluenceType)
	{
		m_ctl_bInflLeader	= FALSE;
		UpdateData(FALSE);
		return;
	}

//	CString str;
	str.Format("IMServer and FieldServer must be shutdown if you want to change Influence SubLeader setting change.\n\n  IMServer and FieldServer is shutdowned?");
	int nRet = MessageBox(str, "Question", MB_YESNO|MB_ICONQUESTION);
	if(IDNO == nRet)
	{// 2006-05-03 by cmkwon, IMServer와 FieldServer가 종료 상태에만 변경해야 한다.
		m_ctl_bInflSub2Leader= !m_ctl_bInflSub2Leader;
		UpdateData(FALSE);
		return;
	}

	if(FALSE == m_ctl_bInflSub2Leader)
	{// 2006-05-03 by cmkwon, 세력리더 설정 해제
		str.Format("This character set a \'%s\' Influence SubLeader.\n\n  Would you cancel this setting? (Will be change immediately)", CAtumSJ::GetInfluenceTypeString(m_byInfluenceType));
	}
	else
	{// 2006-05-03 by cmkwon, 세력리더로 설정
		str.Format("Would you set this characer to Influence SubLeader of \'%s\'? (Will be change immediately)", CAtumSJ::GetInfluenceTypeString(m_byInfluenceType));
	}
	nRet = MessageBox(str, "Question", MB_YESNO|MB_ICONQUESTION);
	if(IDNO == nRet)
	{
		m_ctl_bInflSub2Leader	= !m_ctl_bInflSub2Leader;
		UpdateData(FALSE);
		return;
	}

	m_ctl_bInflSubLeader	= FALSE;
	m_ctl_bInflLeader	= FALSE;
	UpdateData(FALSE);
	StoreInflSub2Leader(m_byInfluenceType, (FALSE==m_ctl_bInflSub2Leader)?0:m_CharacterUID);	
}


///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		// 2009-03-31 by cmkwon, 세력초기화 시스템 구현 - 
/// \author		cmkwon
/// \date		2009-04-02 ~ 2009-04-02
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CCharacterInfoDlg::DBQ_CheckEnableChangeInfluence(void)
{
	CString szQuery;
	SQLHSTMT hstmt = m_pODBCStmt3->GetSTMTHandle();
	SQLINTEGER arrCB2[2] = {SQL_NTS,SQL_NTS};
	SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_ULONG, SQL_INTEGER, 0, 0, &m_AccountUID, 0,	&arrCB2[1]);
	BOOL bRet = m_pODBCStmt3->ExecuteQuery(PROCEDURE_090401_0347);
	if (!bRet)
	{
		MessageBox("DB Execute Error !!(CCharacterInfoDlg::DBQ_CheckEnableChangeInfluence#)","Error",MB_OK|MB_ICONERROR);
		m_pODBCStmt3->FreeStatement();
		return FALSE;
	}

	int nErrorCode = ERR_COMMON_UNKNOWN_ERROR;
	arrCB2[1] = SQL_NTS;
	SQLBindCol(hstmt, 1, SQL_C_LONG, &nErrorCode, 0,				&arrCB2[1]);
	SQLFetch(m_pODBCStmt3->m_hstmt);
	m_pODBCStmt3->FreeStatement();				// free statement	

	if(ERR_NO_ERROR != nErrorCode)
	{
		CString csMsg;
		switch (nErrorCode)
		{
		case ERR_DELETE_CHARACTER_GUILDCOMMANDER:
			csMsg.Format("Account can't change influece because you had a character of guild master !!");
			break;
		case ERR_PROTOCOL_SUBLEADER_SET_ALEADY:
			csMsg.Format("Account can't change influece because you had a character of leader or sub leader !!");
			break;
		case ERR_REQ_REG_LEADER_CANDIDATE:
			csMsg.Format("Account can't change influece because you had a character of leader candidate !!");
			break;
		default:
			csMsg.Format("Account can't change influece !!");
		}
		MessageBox(csMsg,"Error", MB_OK|MB_ICONERROR);
		return FALSE;
	}

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		// 2009-03-31 by cmkwon, 세력초기화 시스템 구현 - 
/// \author		cmkwon
/// \date		2009-04-02 ~ 2009-04-02
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CCharacterInfoDlg::DBQ_InitializeInfluenceAccount(UID32_t i_AccUID)
{
	/*[Stored Query Definition]************************************************
	--------------------------------------------------------------------------------
	-- PROCEDURE NAME	: atum_Init_InfluenceType_ByAccountUID
	-- DESC				: // 2009-03-31 by cmkwon, 세력초기화 시스템 구현 - 
	--						// 2009-10-12 by cmkwon, 프리스카 제거 방안 적용 - 
	--						
	--						#define QUEST_STATE_NONE			(BYTE)0		// 시작하지 않은 상태
	--						#define QUEST_STATE_IN_PROGRESS		(BYTE)1		// 진행중
	--						#define QUEST_STATE_COMPLETED		(BYTE)2		// 완료
	--
	--						#define QUEST_INDEX_OF_SELECT_INFLUENCE				112
	--------------------------------------------------------------------------------
	**************************************************************************/
	SQLBindParameter(m_pODBCStmt3->m_hstmt, 1, SQL_PARAM_INPUT, SQL_C_ULONG, SQL_INTEGER, 0, 0, &i_AccUID, 0, NULL);
	BOOL bRet = m_pODBCStmt3->ExecuteQuery(PROCEDURE_090401_0345);
	m_pODBCStmt3->FreeStatement();
	if (!bRet)
	{		
		return FALSE;
	}

	return TRUE;

}

///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		// 2009-03-31 by cmkwon, 세력초기화 시스템 구현 - 
/// \author		cmkwon
/// \date		2009-04-02 ~ 2009-04-02
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CCharacterInfoDlg::OnBtnInitInfluence() 
{
	// TODO: Add your control notification handler code here

	///////////////////////////////////////////////////////////////////////////////
	// 2009-04-02 by cmkwon, 세력 변경이 가능한지 체크
	if(FALSE == DBQ_CheckEnableChangeInfluence())
	{
		MessageBox("Fail to update character infomation !!", "Error", MB_OK | MB_ICONERROR);
		return;	
	}

	if(FALSE == this->DBQ_InitializeInfluenceAccount(m_AccountUID))
	{
		MessageBox("Fail to change influence account !!", "Error", MB_OK | MB_ICONERROR);
		return;
	}

	m_byInfluenceType		= INFLUENCE_TYPE_NORMAL;
	m_bReloadCharacterInfo	= TRUE;						// 2009-03-31 by cmkwon, 세력초기화 시스템 구현 - 초기화 처리를 진행함.

	CComboBox *pComboInfluenceType = (CComboBox*)GetDlgItem(IDC_COMBO_INFLUENCETYPE);
	if(COMPARE_INFLUENCE(m_byInfluenceType, INFLUENCE_TYPE_NORMAL)){	pComboInfluenceType->SetCurSel(COMBOIndex_INFLUENCETypeNormal);}
	else if(COMPARE_INFLUENCE(m_byInfluenceType, INFLUENCE_TYPE_VCN)){	pComboInfluenceType->SetCurSel(COMBOIndex_INFLUENCETypeVCN);}
	else if(COMPARE_INFLUENCE(m_byInfluenceType, INFLUENCE_TYPE_ANI)){	pComboInfluenceType->SetCurSel(COMBOIndex_INFLUENCETypeANI);}
	else if(COMPARE_INFLUENCE(m_byInfluenceType, INFLUENCE_TYPE_GM)){	pComboInfluenceType->SetCurSel(COMBOIndex_INFLUENCETypeGM);}
	else if(COMPARE_INFLUENCE(m_byInfluenceType, INFLUENCE_TYPE_RRP)){	pComboInfluenceType->SetCurSel(COMBOIndex_INFLUENCETypeRRP);}
	else {																pComboInfluenceType->SetCurSel(COMBOIndex_INFLUENCETypeUnknown);}

	MessageBox("Success !!","Info",MB_OK|MB_ICONINFORMATION);

	OnCancel();		// 2009-04-30 by cmkwon, 세력 초기화 관련 버그 수정 - 여기에서 캔슬로 기존 캐릭터 정보가 업데이트 되면 안된다.
}

#define PROCEDURE_FIX_QUEST (UCHAR*)"{call dbo.atum_admin_FixXFERQuests(?,?)}"
void CCharacterInfoDlg::OnBnFixQuests()
{
	SQLINTEGER arrCB[9] = { SQL_NTS,SQL_NTS,SQL_NTS};
	SQLBindParameter(m_pODBCStmt3->m_hstmt, 1, SQL_PARAM_INPUT, SQL_C_ULONG, SQL_INTEGER, 0, 0, &m_CharacterUID, 0, &arrCB[1]);
	SQLBindParameter(m_pODBCStmt3->m_hstmt, 2, SQL_PARAM_INPUT, SQL_C_ULONG, SQL_INTEGER, 0, 0, &nDestInfluence, 0, &arrCB[2]);
	BOOL bRet = m_pODBCStmt3->ExecuteQuery(PROCEDURE_FIX_QUEST);
	if (!bRet) {
		MessageBox("Cannot fix quests!", "Error", MB_OK | MB_ICONERROR);
		return;
	}
	
//success
		MessageBox("Successfully fixed quests!", "Info", MB_OK | MB_ICONINFORMATION);
}
void CCharacterInfoDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CSliderCtrl* pSlider = reinterpret_cast<CSliderCtrl*>(pScrollBar);
	char tmpStr[128]; *tmpStr = '\0';
	// Check which slider sent the notification  
	if (pSlider == &m_EXPSlider)
	{
		sprintf(tmpStr, "%.2f%%", m_EXPSlider.GetPos()/10.0f);
		GetDlgItem(IDC_EDIT_EXP_PER)->SetWindowText(tmpStr);

		Experience_t exp1 = CAtumSJ::GetInitialExperienceOfLevel(m_Level);
		Experience_t exp2 = CAtumSJ::GetInitialExperienceOfLevel(m_Level + 1) - 1;

		if (CHARACTER_LEVEL_110 <= m_Level)
		{
			m_Level = CHARACTER_LEVEL_110;
			m_ctl_doExp = exp1;
			UpdateData(FALSE);
		}

		Experience_t fCurExp = ((exp2- exp1)* m_EXPSlider.GetPos()/1000.0f) + exp1;
		m_ctl_doExp = fCurExp;
		sprintf(tmpStr, "%.2f", m_ctl_doExp);
		GetDlgItem(IDC_EDIT_EXP)->SetWindowText(tmpStr);
	}
	
	memset(tmpStr, 0x00, sizeof tmpStr);
}

void CCharacterInfoDlg::OnEnUpdateEditExp()
{
	CString csTmp;
	char tmpStr[128]; *tmpStr = '\0';
	GetDlgItem(IDC_EDIT_EXP)->GetWindowText(csTmp);
	Experience_t expTmp = atof(csTmp);

	Experience_t exp1 = CAtumSJ::GetInitialExperienceOfLevel(m_Level);
	Experience_t exp2 = CAtumSJ::GetInitialExperienceOfLevel(m_Level + 1) - 1;

	float fPerc = (expTmp-exp1)/(exp2-exp1)*100.0f;
	sprintf(tmpStr, "%.2f%%", fPerc);
	GetDlgItem(IDC_EDIT_EXP_PER)->SetWindowText(tmpStr);
	m_EXPSlider.SetPos(fPerc * 10);
	memset(tmpStr, 0x00, sizeof tmpStr);	

	m_ctl_doExp = expTmp;
}


void CCharacterInfoDlg::OnEnUpdateEditLevel()
{
	CString csTmp;
	char tmpStr[128]; *tmpStr = '\0';
	GetDlgItem(IDC_EDIT_LEVEL)->GetWindowText(csTmp);
	INT lvTmp = atoi(csTmp);
	
	Experience_t exp1 = CAtumSJ::GetInitialExperienceOfLevel(lvTmp);
	Experience_t exp2 = CAtumSJ::GetInitialExperienceOfLevel(lvTmp + 1) - 1;

	sprintf(tmpStr, "%.2f", exp1);
	GetDlgItem(IDC_EDIT_EXP)->SetWindowText(tmpStr);

	GetDlgItem(IDC_EDIT_EXP)->GetWindowText(csTmp);
	Experience_t expTmp = atof(csTmp);
	float fPerc = (expTmp - exp1) / (exp2 - exp1) * 100.0f;
	sprintf(tmpStr, "%.2f%%", fPerc);
	GetDlgItem(IDC_EDIT_EXP_PER)->SetWindowText(tmpStr);
	m_EXPSlider.SetPos(fPerc * 10);
	memset(tmpStr, 0x00, sizeof tmpStr);

	m_Level = lvTmp;
	m_ctl_doExp = expTmp;
}


void CCharacterInfoDlg::OnBnClickedButton1()
{
	m_CurDP = m_DP;
	m_CurHP = m_HP;
	m_CurSP = m_SP;
	m_CurEP = m_EP;

	CString csTmp;
	GetDlgItem(IDC_EDIT_HP)->GetWindowText(csTmp);
	GetDlgItem(IDC_EDIT_HP_CUR)->SetWindowTextA(csTmp);

	GetDlgItem(IDC_EDIT_DP)->GetWindowText(csTmp);
	GetDlgItem(IDC_EDIT_DP_CUR)->SetWindowTextA(csTmp);

	GetDlgItem(IDC_EDIT_SP)->GetWindowText(csTmp);
	GetDlgItem(IDC_EDIT_SP_CUR)->SetWindowTextA(csTmp);

	GetDlgItem(IDC_EDIT_EP)->GetWindowText(csTmp);
	GetDlgItem(IDC_EDIT_EP_CUR)->SetWindowTextA(csTmp);
}



void CCharacterInfoDlg::OnCbnSelchangeCombo1()
{
	for (int i = 0; i < DRANK_COUNT; i++)
	{
		if(m_BtnDonatorRank[0].GetCheck())
		{
			OnBnClickedButton9();
		}
		if (m_BtnDonatorRank[i].GetCheck())
		{
			CString c_out;
			char szTmpId[128];
			GetDlgItem(IDC_EDIT3)->GetWindowText(c_out);
			if (c_out.GetLength() > 2 && m_pColoredNameCtrl->GetCurSel() == 0)
			{
				c_out.Remove(',');
				sprintf(szTmpId, "%s%03d", c_out,i);
				GetDlgItem(IDC_EDIT_PCBangTotalPlayTime)->SetWindowText(szTmpId);
				m_PCBangTotalPlayTime = atoll(szTmpId);
				return;
			}
			
			switch (m_pColoredNameCtrl->GetCurSel())
			{
				case 0://nocolor
				{
					sprintf(szTmpId, "%d", i);
					GetDlgItem(IDC_EDIT_PCBangTotalPlayTime)->SetWindowText(szTmpId);
					m_PCBangTotalPlayTime = i;
					GetDlgItem(IDC_EDIT3)->SetWindowText("");
				}
					break;
				case 1: 
				{
					sprintf(szTmpId, "144067210%03d", i);
					GetDlgItem(IDC_EDIT_PCBangTotalPlayTime)->SetWindowText(szTmpId);
					m_PCBangTotalPlayTime = atoll(szTmpId);
					sprintf(szTmpId, "144,67,210");
					GetDlgItem(IDC_EDIT3)->SetWindowText(szTmpId);
				}
					break;
				case 2:
				{
					sprintf(szTmpId, "67154210%03d", i);
					GetDlgItem(IDC_EDIT_PCBangTotalPlayTime)->SetWindowText(szTmpId);
					m_PCBangTotalPlayTime = atoll(szTmpId);
					sprintf(szTmpId, "067,154,210");
					GetDlgItem(IDC_EDIT3)->SetWindowText(szTmpId);
				}
				break;
				case 3:
				{
					sprintf(szTmpId, "117200035%03d", i);
					GetDlgItem(IDC_EDIT_PCBangTotalPlayTime)->SetWindowText(szTmpId);
					m_PCBangTotalPlayTime = atoll(szTmpId);
					sprintf(szTmpId, "117,200,035");
					GetDlgItem(IDC_EDIT3)->SetWindowText(szTmpId);
				}
				break;
				case 4:
				{
					sprintf(szTmpId, "65232055%03d", i);
					GetDlgItem(IDC_EDIT_PCBangTotalPlayTime)->SetWindowText(szTmpId);
					m_PCBangTotalPlayTime = atoll(szTmpId);
					sprintf(szTmpId, "065,232,055");
					GetDlgItem(IDC_EDIT3)->SetWindowText(szTmpId);
				}
				break;
				case 5:
				{
					sprintf(szTmpId, "232155055%03d", i);
					GetDlgItem(IDC_EDIT_PCBangTotalPlayTime)->SetWindowText(szTmpId);
					m_PCBangTotalPlayTime = atoll(szTmpId);
					sprintf(szTmpId, "232,155,055");
					GetDlgItem(IDC_EDIT3)->SetWindowText(szTmpId);
				}
				break;
				case 6:
				{
					sprintf(szTmpId, "255000000%03d", i);
					GetDlgItem(IDC_EDIT_PCBangTotalPlayTime)->SetWindowText(szTmpId);
					m_PCBangTotalPlayTime = atoll(szTmpId);
					sprintf(szTmpId, "255,000,000");
					GetDlgItem(IDC_EDIT3)->SetWindowText(szTmpId);
				}
				break;
				case 7:
				{
					sprintf(szTmpId, "150127127%03d", i);
					GetDlgItem(IDC_EDIT_PCBangTotalPlayTime)->SetWindowText(szTmpId);
					m_PCBangTotalPlayTime = atoll(szTmpId);
					sprintf(szTmpId, "150,127,127");
					GetDlgItem(IDC_EDIT3)->SetWindowText(szTmpId);
				}
				break;
			}

		}
	}

}


void CCharacterInfoDlg::OnBnClickedButton8()
{
	COLORREF crFontColor;
	CColorDialog dlg;
	if ((dlg.DoModal() == IDOK))
	{
		m_pColoredNameCtrl->SetCurSel(0);
		crFontColor = dlg.GetColor();

		char tmpStr[128]; *tmpStr = '\0';
		RGBTRIPLE rgb;
		rgb.rgbtRed = GetRValue(crFontColor);
		rgb.rgbtGreen = GetGValue(crFontColor);
		rgb.rgbtBlue = GetBValue(crFontColor);
		sprintf(tmpStr, "%03d,%03d,%03d", rgb.rgbtRed, rgb.rgbtGreen, rgb.rgbtBlue);
		GetDlgItem(IDC_EDIT3)->SetWindowText(tmpStr);
		for (int i = 0; i < DRANK_COUNT; i++)
		{
			if (m_BtnDonatorRank[i].GetCheck()) {
				sprintf(tmpStr, "%03d%03d%03d%03d", rgb.rgbtRed, rgb.rgbtGreen, rgb.rgbtBlue, i);
				GetDlgItem(IDC_EDIT_PCBangTotalPlayTime)->SetWindowText(tmpStr);
				m_PCBangTotalPlayTime = atoll(tmpStr);
				break;
			}
		}
		memset(tmpStr, 0x00, sizeof tmpStr);
	}
}


void CCharacterInfoDlg::OnBnClickedButton9()
{
	for (int i = 0; i < DRANK_COUNT; i++)
	{
		if (m_BtnDonatorRank[i].GetCheck())
		{
			char tmpStr[128]; *tmpStr = '\0';
			m_pColoredNameCtrl->SetCurSel(0);
			sprintf(tmpStr, "%d", i);
			GetDlgItem(IDC_EDIT_PCBangTotalPlayTime)->SetWindowText(tmpStr);
			m_PCBangTotalPlayTime = i;
			GetDlgItem(IDC_EDIT3)->SetWindowText("");
		}
	}
}
