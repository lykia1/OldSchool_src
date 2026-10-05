// LogTabUser.cpp : implementation file
//

#include "stdafx.h"
#include "atumadmintool.h"
#include "LogTabUser.h"
#include "SCLogAdminDlg.h"
#include "AtumProtocol.h"
#include "AtumSJ.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
// 2008-08-25 by cmkwon, MySQL 소스 통합(빠진 쿼리 정리) - StringDefineQuery.h로 뺀다.
#define _STRMSG_S_SCAT_0122 QUERY_080825_0010


/////////////////////////////////////////////////////////////////////////////
// CLogTabUser dialog


CLogTabUser::CLogTabUser(CDialog *i_pMainDlg, CWnd* pParent /*=NULL*/)
	: CDialog(CLogTabUser::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLogTabUser)
	m_bCheckAccountName = FALSE;
	m_szAccountName = _T("");
	m_bCheckCharacterName = FALSE;
	m_szCharacterName = _T("");
	m_bCheckDate = FALSE;
	m_dateStart = (CTime::GetCurrentTime() - CTimeSpan(1, 0 , 0, 0));
	m_dateEnd = CTime::GetCurrentTime();
	m_bCheckMaxRowCount = FALSE;
	m_nMaxRowCount = 100;
	m_bCheckUserDead = FALSE;
	m_bCheckUserExpChange = FALSE;
	m_bCheckUserGameEnd = FALSE;
	m_bCheckUserGameStart = FALSE;
	m_bCheckUserLevelChange = FALSE;
	m_bCheckUserPVPLose = FALSE;
	m_bCheckUserPVPWin = FALSE;
	m_bCheckUserQuestEnd = FALSE;
	m_bCheckUserSpeedHack = FALSE;
	m_bCheckUserMemoryHack = FALSE;
	m_bCheckUserStatChange = FALSE;
	m_bCheckUserWarp = FALSE;
	m_bCheckUserCreateCharacter = FALSE;
	m_bCheckUserDeleteCharacter = FALSE;
	m_bCheckUserCreateCharacter = FALSE;
	m_bCheckUserDeleteCharacter = FALSE;
	m_bCheckUserPvELoss			= FALSE;
	m_bCheckUserPvEWin			= FALSE;
	m_TimeEnd = 0;
	m_TimeStart = 0;
	//}}AFX_DATA_INIT
	m_pMainDlg = (CSCLogAdminDlg*)i_pMainDlg;
	m_pODBCStmt = &m_pMainDlg->m_ODBCStmt;
}


void CLogTabUser::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLogTabUser)
	DDX_Check(pDX, IDC_CHECK_ACCOUNT_NAME, m_bCheckAccountName);
	DDX_Text(pDX, IDC_EDIT_ACCOUNT_NAME, m_szAccountName);
	DDX_Check(pDX, IDC_CHECK_CHARACTER, m_bCheckCharacterName);
	DDX_Text(pDX, IDC_EDIT_CHARACTER_NAME, m_szCharacterName);
	DDX_Check(pDX, IDC_CHECK_DATE, m_bCheckDate);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_START_DATE, m_dateStart);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_END_DATE, m_dateEnd);
	DDX_Check(pDX, IDC_CHECK_MAX_ROW_COUNT, m_bCheckMaxRowCount);
	DDX_Text(pDX, IDC_EDIT_MAX_ROW_COUNT, m_nMaxRowCount);
	DDX_Control(pDX, IDC_EDIT_TOTAL_LOG_ROW_COUNT, m_EditTotalLogRowCount);
	DDX_Check(pDX, IDC_CHECK_USER_DEAD, m_bCheckUserDead);
	DDX_Check(pDX, IDC_CHECK_USER_EXP_CHANGE, m_bCheckUserExpChange);
	DDX_Check(pDX, IDC_CHECK_USER_GAMEEND, m_bCheckUserGameEnd);
	DDX_Check(pDX, IDC_CHECK_USER_GAMESTART, m_bCheckUserGameStart);
	DDX_Check(pDX, IDC_CHECK_USER_LEVEL_CHANGE, m_bCheckUserLevelChange);
	DDX_Check(pDX, IDC_CHECK_USER_PVP_LOSE, m_bCheckUserPVPLose);
	DDX_Check(pDX, IDC_CHECK_USER_PVP_WIN, m_bCheckUserPVPWin);
	DDX_Check(pDX, IDC_CHECK_USER_QUEST_END, m_bCheckUserQuestEnd);
	DDX_Check(pDX, IDC_CHECK_USER_SPEED_HACK, m_bCheckUserSpeedHack);
	DDX_Check(pDX, IDC_CHECK_USER_MEMORY_HACK, m_bCheckUserMemoryHack);
	DDX_Check(pDX, IDC_CHECK_USER_STAT_CHANGE, m_bCheckUserStatChange);
	DDX_Check(pDX, IDC_CHECK_USER_WARP, m_bCheckUserWarp);
	DDX_Check(pDX, IDC_CHECK_USER_CREATE_CHARACTER, m_bCheckUserCreateCharacter);
	DDX_Check(pDX, IDC_CHECK_USER_DELETE_CHARACTER, m_bCheckUserDeleteCharacter);
	DDX_Check(pDX, IDC_CHECK_USER_PVE_LOSS, m_bCheckUserPvELoss);
	DDX_Check(pDX, IDC_CHECK_USER_PVE_WIN, m_bCheckUserPvEWin);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_END_TIME, m_TimeEnd);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_START_TIME, m_TimeStart);
	//}}AFX_DATA_MAP
	DDX_GridControl(pDX, IDC_GRID_USER_LOG, m_GridUserLog);
}


BEGIN_MESSAGE_MAP(CLogTabUser, CDialog)
	//{{AFX_MSG_MAP(CLogTabUser)
	ON_BN_CLICKED(IDC_BUTTON_OK, OnButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_RESET, OnButtonReset)
	ON_BN_CLICKED(IDC_CHECK_ACCOUNT_NAME, OnCheckAccountName)
	ON_BN_CLICKED(IDC_CHECK_CHARACTER, OnCheckCharacter)
	ON_BN_CLICKED(IDC_CHECK_DATE, OnCheckDate)
	ON_BN_CLICKED(IDC_CHECK_MAX_ROW_COUNT, OnCheckMaxRowCount)
	ON_BN_CLICKED(IDC_BUTTON_SELECT_ALL, OnButtonSelectAll)
	ON_BN_CLICKED(IDC_BUTTON_DISELECT_ALL, OnButtonDiselectAll)
	ON_BN_CLICKED(IDC_BTN_SAVE_RESULT, OnBtnSaveResult)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_OK2, &CLogTabUser::OnBnClickedButtonOk2)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLogTabUser message handlers

BOOL CLogTabUser::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
	{
		m_pMainDlg->EndDialog(-1);
		return TRUE;
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}

BOOL CLogTabUser::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_GridUserLog.InitGrid(NUM_COLUMNS_USER_LOG_GRID, g_arrUserLogColumnNames);

	ResetVariables();
	ResetControls();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CLogTabUser::ResetVariables()
{
	m_szCharacterName = _T("");
	m_szAccountName = _T("");
	m_bCheckCharacterName = FALSE;
	m_bCheckAccountName = FALSE;

	m_bCheckDate = FALSE;
	m_dateStart = (CTime::GetCurrentTime() - CTimeSpan(1, 0 , 0, 0));
	m_dateEnd = CTime::GetCurrentTime();

	m_TimeStart = CTime(1977, 1, 1, 0, 0, 0);
	m_TimeEnd = CTime(1977, 1, 1, 23, 59, 59);

	m_bCheckMaxRowCount = FALSE;
	m_bCheckUserMemoryHack = TRUE;
	m_nMaxRowCount = 100;

	m_EditTotalLogRowCount.SetWindowText("");

	UpdateData(FALSE);
}

void CLogTabUser::ResetControls()
{
	GetDlgItem(IDC_EDIT_ACCOUNT_NAME)->EnableWindow(m_bCheckAccountName);
	GetDlgItem(IDC_EDIT_CHARACTER_NAME)->EnableWindow(m_bCheckCharacterName);
	GetDlgItem(IDC_DATETIMEPICKER_START_DATE)->EnableWindow(m_bCheckDate);
	GetDlgItem(IDC_DATETIMEPICKER_END_DATE)->EnableWindow(m_bCheckDate);
	GetDlgItem(IDC_DATETIMEPICKER_START_TIME)->EnableWindow(m_bCheckDate);
	GetDlgItem(IDC_DATETIMEPICKER_END_TIME)->EnableWindow(m_bCheckDate);
	GetDlgItem(IDC_EDIT_MAX_ROW_COUNT)->EnableWindow(m_bCheckMaxRowCount);
}

void CLogTabUser::OnCheckAccountName() 
{
	// TODO: Add your control notification handler code here
	UpdateData();

	GetDlgItem(IDC_EDIT_ACCOUNT_NAME)->EnableWindow(m_bCheckAccountName);
}

void CLogTabUser::OnCheckCharacter() 
{
	// TODO: Add your control notification handler code here
	UpdateData();

	GetDlgItem(IDC_EDIT_CHARACTER_NAME)->EnableWindow(m_bCheckCharacterName);
}

void CLogTabUser::OnCheckDate() 
{
	// TODO: Add your control notification handler code here
	UpdateData();

	GetDlgItem(IDC_DATETIMEPICKER_START_DATE)->EnableWindow(m_bCheckDate);
	GetDlgItem(IDC_DATETIMEPICKER_END_DATE)->EnableWindow(m_bCheckDate);
	GetDlgItem(IDC_DATETIMEPICKER_START_TIME)->EnableWindow(m_bCheckDate);
	GetDlgItem(IDC_DATETIMEPICKER_END_TIME)->EnableWindow(m_bCheckDate);
}

void CLogTabUser::OnCheckMaxRowCount() 
{
	// TODO: Add your control notification handler code here
	UpdateData();

	if (!m_bCheckMaxRowCount)
	{
		m_nMaxRowCount = SIZE_MAX_LOG_ROW_COUNT;
		UpdateData(FALSE);
	}
	else
	{
		m_nMaxRowCount = 100;
		UpdateData(FALSE);
	}

	GetDlgItem(IDC_EDIT_MAX_ROW_COUNT)->EnableWindow(m_bCheckMaxRowCount);
}

void CLogTabUser::OnButtonOk() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	m_EditTotalLogRowCount.SetWindowText("");

	// 그리드 정리하기
	m_GridUserLog.InitGrid(NUM_COLUMNS_USER_LOG_GRID, g_arrUserLogColumnNames);

	BOOL bRet = TRUE;
	if (bRet & m_bCheckUserGameStart)
	{
		bRet = GetLogAndInsertToGrid(T1_FL_LOG_GAME_START, "atum_log_user_game_start_end",
			STRMSG_S_SCADMINTOOL_0017);
		if (bRet && m_pMainDlg->m_bCheckArchivesBackup)
		{
			bRet = GetLogAndInsertToGrid(T1_FL_LOG_GAME_START, "atum_backup_log_user_game_start_end",
				STRMSG_S_SCADMINTOOL_0017);
		}
	}

	if (bRet & m_bCheckUserGameEnd)
	{
		bRet = GetLogAndInsertToGrid(T1_FL_LOG_GAME_END, "atum_log_user_game_start_end",
			STRMSG_S_SCADMINTOOL_0017);
		if (bRet && m_pMainDlg->m_bCheckArchivesBackup)
		{
			bRet = GetLogAndInsertToGrid(T1_FL_LOG_GAME_END, "atum_backup_log_user_game_start_end",
				STRMSG_S_SCADMINTOOL_0017);
		}
	}

	if (bRet & m_bCheckUserCreateCharacter)
	{
		bRet = GetLogAndInsertToGrid(T1_FL_LOG_CREATE_CHARACTER, "atum_log_user_game_start_end",
			"dbo.atum_GetGearString(l.PlayTime) + \'-Gear, Lv \' + CAST(l.TotalPlayTime AS VARCHAR(10))");

		if (bRet && m_pMainDlg->m_bCheckArchivesBackup)
		{
			bRet = GetLogAndInsertToGrid(T1_FL_LOG_CREATE_CHARACTER, "atum_backup_log_user_game_start_end",
				"dbo.atum_GetGearString(l.PlayTime) + \'-Gear, Lv \' + CAST(l.TotalPlayTime AS VARCHAR(10))");
		}
	}

	if (bRet & m_bCheckUserDeleteCharacter)
	{
		bRet = GetLogAndInsertToGrid(T1_FL_LOG_DELETE_CHARACTER, "atum_log_user_game_start_end",
			"dbo.atum_GetGearString(l.PlayTime) + \'-Gear, Lv \' + CAST(l.TotalPlayTime AS VARCHAR(10))");

		if (bRet && m_pMainDlg->m_bCheckArchivesBackup)
		{
			bRet = GetLogAndInsertToGrid(T1_FL_LOG_DELETE_CHARACTER, "atum_backup_log_user_game_start_end",
				"dbo.atum_GetGearString(l.PlayTime) + \'-Gear, Lv \' + CAST(l.TotalPlayTime AS VARCHAR(10))");
		}
	}

	if (bRet & m_bCheckUserLevelChange)
	{
		bRet = GetLogAndInsertToGrid(T1_FL_LOG_LEVEL, "atum_log_user_level_stat",
			STRMSG_S_SCADMINTOOL_0018);
	}

	if (bRet & m_bCheckUserExpChange)
	{
		bRet = GetLogAndInsertToGrid(T1_FL_LOG_EXP, "atum_log_user_exp",
			_STRMSG_S_SCAT_0122);
	}

	if (bRet & m_bCheckUserStatChange)
	{
		bRet = GetLogAndInsertToGrid(T1_FL_LOG_STAT, "atum_log_user_level_stat",
			"CAST(l.Param1 AS VARCHAR(10))", "CAST(l.Param2 AS VARCHAR(10))");
	}

	if (bRet & m_bCheckUserQuestEnd)
	{
		bRet = GetLogAndInsertToGrid(T1_FL_LOG_QUEST_COMPLETION, "atum_log_user_quest_pk_dead",
			"CAST(l.Param1 AS VARCHAR(10))");

		if (bRet && m_pMainDlg->m_bCheckArchivesBackup)
		{
			bRet = GetLogAndInsertToGrid(T1_FL_LOG_QUEST_COMPLETION, "atum_backup_log_user_quest_pk_dead",
				"CAST(l.Param1 AS VARCHAR(10))");
		}
	}

	if (bRet & m_bCheckUserPVPWin)
	{
		bRet = GetLogAndInsertToGrid(T1_FL_LOG_PKWIN, "atum_log_user_quest_pk_dead",
			"(select charactername from td_character where uniquenumber like Param1)");

		if (bRet && m_pMainDlg->m_bCheckArchivesBackup)
		{

			bRet = GetLogAndInsertToGrid(T1_FL_LOG_PKWIN, "atum_backup_log_user_quest_pk_dead",
				"(select charactername from td_character where uniquenumber like Param1)");
		}
	}

	if (bRet & m_bCheckUserPVPLose)
	{
		bRet = GetLogAndInsertToGrid(T1_FL_LOG_PKLOSS, "atum_log_user_quest_pk_dead",
			"(select charactername from td_character where uniquenumber like Param1)");
		if (bRet && m_pMainDlg->m_bCheckArchivesBackup)
		{
			bRet = GetLogAndInsertToGrid(T1_FL_LOG_PKLOSS, "atum_backup_log_user_quest_pk_dead",
				"(select charactername from td_character where uniquenumber like Param1)");
		}
	}

	if (bRet & m_bCheckUserDead)
	{
		bRet = GetLogAndInsertToGrid(T1_FL_LOG_DEAD, "atum_log_user_quest_pk_dead",
			"CAST(l.Param1 AS VARCHAR(10))");

		if (bRet && m_pMainDlg->m_bCheckArchivesBackup)
		{
			bRet = GetLogAndInsertToGrid(T1_FL_LOG_DEAD, "atum_backup_log_user_quest_pk_dead",
				"CAST(l.Param1 AS VARCHAR(10))");
		}
	}

	if (bRet & m_bCheckUserWarp)
	{
		bRet = GetLogAndInsertToGrid(T1_FL_LOG_WARP, "atum_log_user_warp",
			"CAST(TargetMapIndex AS VARCHAR(10)) + \
			\' (\' + CAST(TargetPosition_X/40 AS VARCHAR(10)) + \
			\', \' + CAST(TargetPosition_Z/40 AS VARCHAR(10)) + \')\'");
	}

	if (bRet & m_bCheckUserSpeedHack)
	{
		bRet = GetLogAndInsertToGrid(T1_FL_LOG_SPEED_HACK, "atum_log_user_speed_hack",
			" \'CheckType \' + CAST(CAST(CheckType AS INT) AS VARCHAR(10)) + \
			\', ReceivedMoveCounts \' + CAST(CAST(ReceivedMoveCounts AS INT) AS VARCHAR(10)) + \
			\', CurrentSpeed \' + CAST(CAST(CurrentSpeed AS INT) AS VARCHAR(10))");
	}

	if (bRet & m_bCheckUserMemoryHack)
	{
		bRet = GetLogAndInsertToGrid(T1_FL_LOG_MEMORY_HACK, "atum_log_user_memory_hack",
			/*" \'ItemKind(\' + CAST(CAST(ItemKind AS INT) AS VARCHAR(10)) + \
			\'), ItemNum(\' + CAST(CAST(ItemNum AS INT) AS VARCHAR(10)) + \
			\'), ItemUID(\' + CAST(CAST(ItemUniqueNumber AS INT) AS VARCHAR(10)) + \
			\'), Hacking CODE(\' + CASE WHEN LEN(MemoryHackCheckType)<=4 THEN (RIGHT(\'0000\'+ISNULL(CAST(CAST(MemoryHackCheckType AS INT) AS VARCHAR(10)),\'\'),4)) ELSE (RIGHT(\'00000\'+ISNULL(CAST(CAST(MemoryHackCheckType AS INT) AS VARCHAR(10)),\'\'),5)) END+ \
			\'), Param1(\' + CAST(CAST(ValidValue AS FLOAT) AS VARCHAR(20)) + \
			\'), Param2(\' + CAST(CAST(CurrentValue AS FLOAT) AS VARCHAR(20)) + \
			\'), Param3(\' + CAST(CAST(Param1 AS INT) AS VARCHAR(10)) + \')\'");*/
			"CASE MemoryHackCheckType\
			WHEN 70000 THEN 'Illegal process found - code for player(' + CASE WHEN LEN(MemoryHackCheckType) <= 4 THEN(RIGHT('0000' + ISNULL(CAST(CAST(MemoryHackCheckType AS INT) AS VARCHAR(10)), ''), 4)) ELSE(RIGHT('00000' + ISNULL(CAST(CAST(MemoryHackCheckType AS INT) AS VARCHAR(10)), ''), 5)) END + ') ProcessID(' + CAST(CAST(ItemUniqueNumber AS INT) AS VARCHAR(10)) + ') '\
			WHEN 70001 THEN 'Cheat Engine registry check (deleted from 1.11.23) Code for player(' + CASE WHEN LEN(MemoryHackCheckType) <= 4 THEN(RIGHT('0000' + ISNULL(CAST(CAST(MemoryHackCheckType AS INT) AS VARCHAR(10)), ''), 4)) ELSE(RIGHT('00000' + ISNULL(CAST(CAST(MemoryHackCheckType AS INT) AS VARCHAR(10)), ''), 5)) END + ') '\
			WHEN 80000 THEN 'Not allowed DLL injected (RTSS Hook or hack dll) Code for player(' + CASE WHEN LEN(MemoryHackCheckType) <= 4 THEN(RIGHT('0000' + ISNULL(CAST(CAST(MemoryHackCheckType AS INT) AS VARCHAR(10)), ''), 4)) ELSE(RIGHT('00000' + ISNULL(CAST(CAST(MemoryHackCheckType AS INT) AS VARCHAR(10)), ''), 5)) END + ') '\
			WHEN 80001 THEN 'CheckNtGlobalFlag detected Code for player(' + CASE WHEN LEN(MemoryHackCheckType) <= 4 THEN(RIGHT('0000' + ISNULL(CAST(CAST(MemoryHackCheckType AS INT) AS VARCHAR(10)), ''), 4)) ELSE(RIGHT('00000' + ISNULL(CAST(CAST(MemoryHackCheckType AS INT) AS VARCHAR(10)), ''), 5)) END + ') '\
			WHEN 80002 THEN 'Debugger detected Code for player(' + CASE WHEN LEN(MemoryHackCheckType) <= 4 THEN(RIGHT('0000' + ISNULL(CAST(CAST(MemoryHackCheckType AS INT) AS VARCHAR(10)), ''), 4)) ELSE(RIGHT('00000' + ISNULL(CAST(CAST(MemoryHackCheckType AS INT) AS VARCHAR(10)), ''), 5)) END + ') '\
			WHEN 80003 THEN 'HW breakpoint detected Code for player(' + CASE WHEN LEN(MemoryHackCheckType) <= 4 THEN(RIGHT('0000' + ISNULL(CAST(CAST(MemoryHackCheckType AS INT) AS VARCHAR(10)), ''), 4)) ELSE(RIGHT('00000' + ISNULL(CAST(CAST(MemoryHackCheckType AS INT) AS VARCHAR(10)), ''), 5)) END + ') '\
			WHEN 80004 THEN 'Main thread suspended Code for player(' + CASE WHEN LEN(MemoryHackCheckType) <= 4 THEN(RIGHT('0000' + ISNULL(CAST(CAST(MemoryHackCheckType AS INT) AS VARCHAR(10)), ''), 4)) ELSE(RIGHT('00000' + ISNULL(CAST(CAST(MemoryHackCheckType AS INT) AS VARCHAR(10)), ''), 5)) END + ') '\
			WHEN 60000 THEN 'Missing important file from Res-Obj(possible wallhack) Code for player(' + CASE WHEN LEN(MemoryHackCheckType) <= 4 THEN(RIGHT('0000' + ISNULL(CAST(CAST(MemoryHackCheckType AS INT) AS VARCHAR(10)), ''), 4)) ELSE(RIGHT('00000' + ISNULL(CAST(CAST(MemoryHackCheckType AS INT) AS VARCHAR(10)), ''), 5)) END + ') '\
			WHEN 65000 THEN 'Crossfeeding with the same IP Code for player(' + CASE WHEN LEN(MemoryHackCheckType) <= 4 THEN(RIGHT('0000' + ISNULL(CAST(CAST(MemoryHackCheckType AS INT) AS VARCHAR(10)), ''), 4)) ELSE(RIGHT('00000' + ISNULL(CAST(CAST(MemoryHackCheckType AS INT) AS VARCHAR(10)), ''), 5)) END + ') '\
			WHEN 30000 THEN\
			'ItemKind(' + CAST(CAST(ItemKind AS INT) AS VARCHAR(10)) + '), \
			ItemNum(' + CAST(CAST(ItemNum AS INT) AS VARCHAR(10)) + 			'),\
			ItemUID(' + CAST(CAST(ItemUniqueNumber AS INT) AS VARCHAR(10)) + 			'),\
			Hacking CODE(' + CASE WHEN LEN(MemoryHackCheckType)<=4 THEN\
				(RIGHT('0000' + ISNULL(CAST(CAST(MemoryHackCheckType AS INT)\
					AS VARCHAR(10)), ''), 4)) ELSE\
					(RIGHT('00000' + ISNULL(CAST(CAST(MemoryHackCheckType AS INT) AS VARCHAR(10)), ''), 5)) END + '),\
				GambleCode(' + CAST(CAST(ValidValue AS FLOAT) AS VARCHAR(20)) + 			'),\
				IsParamHacked(' + CAST(CAST(CurrentValue AS FLOAT) AS VARCHAR(20)) + 			'),\
				IsValueHacked(' + CAST(CAST(Param1 AS INT) AS VARCHAR(10)) + ')'  \
				WHEN 31000 THEN\
				'ItemKind(' + CAST(CAST(ItemKind AS INT) AS VARCHAR(10)) + '), \
				ItemNum(' + CAST(CAST(ItemNum AS INT) AS VARCHAR(10)) + 			'),\
				ItemUID(' + CAST(CAST(ItemUniqueNumber AS INT) AS VARCHAR(10)) + 			'),\
				Hacking CODE(' + CASE WHEN LEN(MemoryHackCheckType)<=4 THEN\
					(RIGHT('0000' + ISNULL(CAST(CAST(MemoryHackCheckType AS INT)\
						AS VARCHAR(10)), ''), 4)) ELSE\
						(RIGHT('00000' + ISNULL(CAST(CAST(MemoryHackCheckType AS INT) AS VARCHAR(10)), ''), 5)) END + '),\
					GambleCode(' + CAST(CAST(ValidValue AS FLOAT) AS VARCHAR(20)) + 			'),\
					IsParamHacked(' + CAST(CAST(CurrentValue AS FLOAT) AS VARCHAR(20)) + 			'),\
					IsValueHacked(' + CAST(CAST(Param1 AS INT) AS VARCHAR(10)) + ')'  \
					WHEN 50000 THEN\
					' SKILL Hacked  \
					SkillNumber(' + CAST(CAST(ItemUniqueNumber AS INT) AS VARCHAR(10)) + 			'),\
					Hacking CODE(' + CASE WHEN LEN(MemoryHackCheckType)<=4 THEN\
						(RIGHT('0000' + ISNULL(CAST(CAST(MemoryHackCheckType AS INT)\
							AS VARCHAR(10)), ''), 4)) ELSE\
							(RIGHT('00000' + ISNULL(CAST(CAST(MemoryHackCheckType AS INT) AS VARCHAR(10)), ''), 5)) END + '),\
						HackDetected(' + CAST(CAST(ValidValue AS FLOAT) AS VARCHAR(20)) + 			'),\
						IsParamHacked(' + CAST(CAST(CurrentValue AS FLOAT) AS VARCHAR(20)) + 			'),\
						IsValueHacked(' + CAST(CAST(Param1 AS INT) AS VARCHAR(10)) + ')'  \
						ELSE\
						'HACK => ItemKind(' + CAST(CAST(ItemKind AS INT) AS VARCHAR(10)) + '), \
						ItemNum(' + CAST(CAST(ItemNum AS INT) AS VARCHAR(10)) + 			'),\
						ItemUID(' + CAST(CAST(ItemUniqueNumber AS INT) AS VARCHAR(10)) + 			'),\
						Hacking CODE(' + CASE WHEN LEN(MemoryHackCheckType)<=4 THEN\
							(RIGHT('0000' + ISNULL(CAST(CAST(MemoryHackCheckType AS INT)\
								AS VARCHAR(10)), ''), 4)) ELSE\
								(RIGHT('00000' + ISNULL(CAST(CAST(MemoryHackCheckType AS INT) AS VARCHAR(10)), ''), 5)) END + '),\
							Param1(' + CAST(CAST(ValidValue AS FLOAT) AS VARCHAR(20)) + 			'),\
							Param2(' + CAST(CAST(CurrentValue AS FLOAT) AS VARCHAR(20)) + 			'),\
							Param3(' + CAST(CAST(Param1 AS INT) AS VARCHAR(10)) + ')'  \
							END");
	}

	///////////////////////////////////////////////////////////////////////////////
	// 2006-03-06 by cmkwon, 세력전시 로그
	if (bRet & m_bCheckUserPvEWin)
	{
		bRet = GetLogAndInsertToGrid(T1_FL_LOG_PvEWIN, "atum_log_user_quest_pk_dead",
			"(select charactername from td_character where uniquenumber like Param1)");

		if (bRet && m_pMainDlg->m_bCheckArchivesBackup)
		{
			bRet = GetLogAndInsertToGrid(T1_FL_LOG_PvEWIN, "atum_backup_log_user_quest_pk_dead",
				"(select charactername from td_character where uniquenumber like Param1)");
		}
	}
	if (bRet & m_bCheckUserPvELoss)
	{
		bRet = GetLogAndInsertToGrid(T1_FL_LOG_PvELOSS, "atum_log_user_quest_pk_dead",
			"(select charactername from td_character where uniquenumber like Param1)");

		if (bRet && m_pMainDlg->m_bCheckArchivesBackup)
		{
			bRet = GetLogAndInsertToGrid(T1_FL_LOG_PvELOSS, "atum_backup_log_user_quest_pk_dead",
				"(select charactername from td_character where uniquenumber like Param1)");
		}
	}


	m_GridUserLog.SortTextItems(COL_USER_LOG_DATE_TIME, FALSE);

	// 리턴된 로그 수 할당
	char szRowCountBuf[10];
	m_EditTotalLogRowCount.SetWindowText(itoa(m_GridUserLog.GetRowCount() - 1, szRowCountBuf, 10));
}

BOOL CLogTabUser::GetLogAndInsertToGrid(BYTE T1_LogType, LPCSTR szTableName, LPCSTR szContentString1, LPCSTR szContentString2 /* = NULL */)
{
	// max row count
	CString szQuery;
	if (m_bCheckMaxRowCount)
	{
		// 2008-07-02 by cmkwon, MySQL 지원 구현 - 
		//szQuery.Format("select top %d ", min(m_nMaxRowCount, SIZE_MAX_LOG_ROW_COUNT));
		szQuery.Format(QUERY_080702_0013_1_A, min(m_nMaxRowCount, SIZE_MAX_LOG_ROW_COUNT));		// 2008-07-02 by cmkwon, MySQL 지원 구현 - 
	}
	else
	{
		// 2008-07-02 by cmkwon, MySQL 지원 구현 - 
		//szQuery = "select ";
		szQuery = QUERY_080702_0013_1_B;		// 2008-07-02 by cmkwon, MySQL 지원 구현 - 
	}

	if (stricmp("atum_log_user_game_start_end", szTableName) == 0
		|| stricmp("atum_backup_log_user_game_start_end", szTableName) == 0)
	{
		// 2008-07-02 by cmkwon, MySQL 지원 구현 - 
		//szQuery += " l.accountname, l.charactername, l.Time, i.LogStringKO, l.MapIndex, l.Position_X, l.Position_Y, l.Position_Z, ";
		szQuery += QUERY_080702_0013_2_A;		// 2008-07-02 by cmkwon, MySQL 지원 구현 - 
	}
	else
	{
		// 2008-07-02 by cmkwon, MySQL 지원 구현 - 
		//szQuery += " c.accountname, c.charactername, l.Time, i.LogStringKO, l.MapIndex, l.Position_X, l.Position_Y, l.Position_Z, ";
		szQuery += QUERY_080702_0013_2_B;		// 2008-07-02 by cmkwon, MySQL 지원 구현 - 
	}

	szQuery += szContentString1;
	szQuery += " , ";
	if (szContentString2 != NULL)
	{
		szQuery += szContentString2;
	}
	else
	{
		szQuery += "\'\'";
	}

	// 2008-07-02 by cmkwon, MySQL 지원 구현 - 
	//szQuery += " from atum_log_type_info i, ";
	szQuery += QUERY_080702_0013_3;		// 2008-07-02 by cmkwon, MySQL 지원 구현 - 

	szQuery += szTableName;
	if (stricmp("atum_log_user_game_start_end", szTableName) == 0
		|| stricmp("atum_backup_log_user_game_start_end", szTableName) == 0)
	{
		// 2008-07-02 by cmkwon, MySQL 지원 구현 - 
		//szQuery += " l WITH (NOLOCK) where i.logtype = l.logtype ";
		szQuery += QUERY_080702_0013_4_A;		// 2008-07-02 by cmkwon, MySQL 지원 구현 - 
	}
	else
	{
		// 2008-07-02 by cmkwon, MySQL 지원 구현 - 
		//szQuery += " l, td_character c WITH (NOLOCK) where i.logtype = l.logtype and c.uniquenumber = l.characteruniquenumber ";
		szQuery += QUERY_080702_0013_4_B;		// 2008-07-02 by cmkwon, MySQL 지원 구현 - 
	}

	// log type
	CString szLogTypeCond;
	// 2008-07-02 by cmkwon, MySQL 지원 구현 - 
	//szLogTypeCond.Format(" and l.logtype = %d ", T1_LogType);
	szLogTypeCond.Format(QUERY_080702_0013_5, T1_LogType);		// 2008-07-02 by cmkwon, MySQL 지원 구현 - 
	szQuery += szLogTypeCond;

	// account
	if (m_bCheckAccountName)
	{
		CString szAccountCond;
		if (stricmp("atum_log_user_game_start_end", szTableName) == 0
			|| stricmp("atum_backup_log_user_game_start_end", szTableName) == 0)
		{
			// 2008-07-02 by cmkwon, MySQL 지원 구현 - 
			//szAccountCond.Format(" and l.accountname = \'%s\' ", m_szAccountName);
			szAccountCond.Format(QUERY_080702_0013_6_A, m_szAccountName);		// 2008-07-02 by cmkwon, MySQL 지원 구현 - 
		}
		else
		{
			// 2008-07-02 by cmkwon, MySQL 지원 구현 - 
			//szAccountCond.Format(" and c.accountname = \'%s\' ", m_szAccountName);
			szAccountCond.Format(QUERY_080702_0013_6_B, m_szAccountName);		// 2008-07-02 by cmkwon, MySQL 지원 구현 - 
		}
		szQuery += szAccountCond;
	}

	// character
	if (m_bCheckCharacterName)
	{
		CString szCharacterCond;
		if (stricmp("atum_log_user_game_start_end", szTableName) == 0
			|| stricmp("atum_backup_log_user_game_start_end", szTableName) == 0)
		{
			// 2008-07-02 by cmkwon, MySQL 지원 구현 - 
			//szCharacterCond.Format(" and l.charactername = \'%s\' ", m_szCharacterName);
			szCharacterCond.Format(QUERY_080702_0013_7_A, m_szCharacterName);		// 2008-07-02 by cmkwon, MySQL 지원 구현 - 
		}
		else
		{
			// 2008-07-02 by cmkwon, MySQL 지원 구현 - 
			//szCharacterCond.Format(" and c.charactername = \'%s\' ", m_szCharacterName);
			szCharacterCond.Format(QUERY_080702_0013_7_B, m_szCharacterName);	// 2008-07-02 by cmkwon, MySQL 지원 구현 - 
		}
		szQuery += szCharacterCond;
	}

	// date
	if (m_bCheckDate)
	{
		CString szDateCond;
		// 2008-07-02 by cmkwon, MySQL 지원 구현 - 
		//szDateCond.Format(" and l.time > \'%s\' and l.time < \'%s\' ",
		//				(LPCSTR)(m_dateStart.Format("%Y/%m/%d") + m_TimeStart.Format(" %H:%M:%S.0")),
		//				(LPCSTR)(m_dateEnd.Format("%Y/%m/%d") + m_TimeEnd.Format(" %H:%M:%S.0")));
		szDateCond.Format(QUERY_080702_0013_8,		// 2008-07-02 by cmkwon, MySQL 지원 구현 - 
			(LPCSTR)(m_dateStart.Format("%Y/%m/%d") + m_TimeStart.Format(" %H:%M:%S.0")),
			(LPCSTR)(m_dateEnd.Format("%Y/%m/%d") + m_TimeEnd.Format(" %H:%M:%S.0")));
		szQuery += szDateCond;
	}

	// ordering
	// 2008-07-02 by cmkwon, MySQL 지원 구현 - 
	//szQuery += " order by l.time desc ";
	szQuery += QUERY_080702_0013_9;		// 2008-07-02 by cmkwon, MySQL 지원 구현 - 

#if defined(DB_SERVER_MYSQL)
	// 2008-07-08 by cmkwon, MySQL 지원 구현 - 
	if(m_bCheckMaxRowCount)
	{
		char szTm[1024];
		sprintf(szTm, QUERY_080702_0013_10_A, min(m_nMaxRowCount, SIZE_MAX_LOG_ROW_COUNT));
		szQuery += szTm;
	}
	else
	{
		szQuery += QUERY_080702_0013_10_B;
	}
#endif
	
	USER_LOG_INFO UserLog;

	SQLINTEGER cb1, cb2, cb3, cb4, cb5, cb6;
	char AccountName[SIZE_MAX_ACCOUNT_NAME];
	char CharacterName[SIZE_MAX_CHARACTER_NAME];
	SQL_TIMESTAMP_STRUCT sqlTime;
	char LogTypeString[30];
	SHORT MapIndex, PositionX, PositionY, PositionZ;
	char Contents[1024]; memset(Contents, 0, 1024);
	char Contents2[1024]; memset(Contents2, 0, 1024);

	SQLRETURN sqlReturn;

#ifdef _DEBUG
	DBGOUT("%s\r\n", szQuery);
#endif
	
// 2008-08-27 by cmkwon, 날쿼리를 Procedure로 수정 - 
//	if (!m_pODBCStmt->ExecuteQuery(szQuery))
	SQLHSTMT hstmt = m_pODBCStmt->GetSTMTHandle();
	SQLINTEGER arrCB2[3] = {SQL_NTS,SQL_NTS,SQL_NTS};
	SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, SIZE_MAX_TABLE_NAME, 0, (LPSTR)(LPCSTR)szTableName, 0,		&arrCB2[1]);
	SQLBindParameter(hstmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 6000, 0, (LPSTR)(LPCSTR)szQuery, 0,			&arrCB2[2]);	
	BOOL bRet = m_pODBCStmt->ExecuteQuery((char*)(PROCEDURE_080827_0010));
	if(FALSE == bRet)
	{
		m_pODBCStmt->FreeStatement();
		return FALSE;
	}

	cb1 = SQL_NTS;
	SQLBindCol(m_pODBCStmt->m_hstmt, 1, SQL_C_CHAR, AccountName, SIZE_MAX_ACCOUNT_NAME, &cb1);
	cb2 = SQL_NTS;
	SQLBindCol(m_pODBCStmt->m_hstmt, 2, SQL_C_CHAR, CharacterName, SIZE_MAX_CHARACTER_NAME, &cb2);
	SQLBindCol(m_pODBCStmt->m_hstmt, 3, SQL_C_TIMESTAMP, &sqlTime, 0, &cb3);
	cb4 = SQL_NTS;
	SQLBindCol(m_pODBCStmt->m_hstmt, 4, SQL_C_CHAR, LogTypeString, 30, &cb4);
	SQLBindCol(m_pODBCStmt->m_hstmt, 5, SQL_C_USHORT, &MapIndex, 0, NULL);
	SQLBindCol(m_pODBCStmt->m_hstmt, 6, SQL_C_SHORT, &PositionX, 0, NULL);
	SQLBindCol(m_pODBCStmt->m_hstmt, 7, SQL_C_SHORT, &PositionY, 0, NULL);
	SQLBindCol(m_pODBCStmt->m_hstmt, 8, SQL_C_SHORT, &PositionZ, 0, NULL);
	cb5 = SQL_NTS;
	SQLBindCol(m_pODBCStmt->m_hstmt, 9, SQL_C_CHAR, Contents, 1024, &cb5);
	cb5 = SQL_NTS;
	SQLBindCol(m_pODBCStmt->m_hstmt, 10, SQL_C_CHAR, Contents2, 1024, &cb6);

	// reset
	memset(Contents, 0, 200);
	memset(Contents2, 0, 200);
	while ((sqlReturn = SQLFetch(m_pODBCStmt->m_hstmt)) != SQL_NO_DATA)
	{
		if (sqlReturn != SQL_SUCCESS)
		{
			m_pODBCStmt->ProcessLogMessagesForStmt(PROCEDURE_080827_0010);
		}
		UserLog.AccountName = AccountName;
		UserLog.CharacterName = CharacterName;
		UserLog.Position.Format("%d (%d, %d)", (INT)MapIndex, (INT)PositionX/SIZE_MAP_TILE_SIZE, (INT)PositionZ/SIZE_MAP_TILE_SIZE);
		UserLog.Time = sqlTime;
		UserLog.LogType = LogTypeString;
		SetContentsByLogType(T1_LogType, Contents, Contents2, UserLog.Contents);
		// 2011-04-20 by shcho, 러시아 로그에서 MemoryHackCheckType(14) 제거 - 러시아도 4번 추가 제거
#if defined(SERVICE_TYPE_ENGLISH_SERVER_1) || defined(SERVICE_TYPE_GERMAN_SERVER_1) || defined(SERVICE_TYPE_RUSSIAN_SERVER_1)
		
		//size_t ret1 = strpos(Contents, "MemoryHackCheckType(14)");
		//size_t ret2 = strpos(Contents, "MemoryHackCheckType(4)");
		
		//if( !(NULL == ret1 || NULL == ret2) )
		//{
			m_GridUserLog.InsertGridUserLog(&UserLog);	
		//}
		
#else
		m_GridUserLog.InsertGridUserLog(&UserLog);
#endif
		//end 2011-04-20 by shcho, 러시아 로그에서 MemoryHackCheckType(14

		int nRowCount = m_GridUserLog.GetRowCount() - 1;
		if (nRowCount >= max(100, (m_bCheckMaxRowCount?m_nMaxRowCount:SIZE_MAX_LOG_ROW_COUNT)))
		{
			MessageBox(STRERR_S_SCADMINTOOL_0012);
			m_GridUserLog.ArrangeGrid();
			m_pODBCStmt->FreeStatement();
			return FALSE;
		}

		// reset
		memset(Contents, 0, 200);
		memset(Contents2, 0, 200);
	}

	m_GridUserLog.ArrangeGrid();
	m_pODBCStmt->FreeStatement();
	return TRUE;
}

void CLogTabUser::SetContentsByLogType(BYTE T1_LogType, LPCSTR i_szContents1, LPCSTR i_szContents2, CString &o_szContents)
{
	if (T1_LogType == T1_FL_LOG_DEAD)
	{
		switch(atoi(i_szContents1))
		{
		case DAMAGE_BY_COLLISION:		o_szContents = "Collision";		break;
		case DAMAGE_BY_MONSTER:			o_szContents = "Monster";		break;
		case DAMAGE_BY_PK:				o_szContents = "Character";		break;
		default:
			{
				o_szContents = "Unknown";
			}
		}
	}
	else if (T1_LogType == T1_FL_LOG_STAT)
	{
		o_szContents.Format("%s, Remained Stat: %s", CAtumSJ::GetStatString(atoi(i_szContents1)), i_szContents2);
	}
	else
	{
		o_szContents = i_szContents1;
	}
}

void CLogTabUser::OnButtonReset() 
{
	// TODO: Add your control notification handler code here
	m_GridUserLog.InitGrid(NUM_COLUMNS_USER_LOG_GRID, g_arrUserLogColumnNames);
	ResetVariables();
	ResetControls();
}

void CLogTabUser::OnButtonSelectAll() 
{
	UpdateData(TRUE);

	m_bCheckUserDead = TRUE;
	m_bCheckUserExpChange = TRUE;
	m_bCheckUserGameEnd = TRUE;
	m_bCheckUserGameStart = TRUE;
	m_bCheckUserCreateCharacter = TRUE;
	m_bCheckUserDeleteCharacter = TRUE;
	m_bCheckUserLevelChange = TRUE;
	m_bCheckUserPVPLose = TRUE;
	m_bCheckUserPVPWin = TRUE;
	m_bCheckUserQuestEnd = TRUE;
	m_bCheckUserSpeedHack = TRUE;
	m_bCheckUserMemoryHack = TRUE;
	m_bCheckUserStatChange = TRUE;
	m_bCheckUserWarp = TRUE;
	m_bCheckUserPvELoss			= TRUE;
	m_bCheckUserPvEWin			= TRUE;

	UpdateData(FALSE);
}

void CLogTabUser::OnButtonDiselectAll() 
{
	UpdateData(TRUE);

	m_bCheckUserDead = FALSE;
	m_bCheckUserExpChange = FALSE;
	m_bCheckUserGameEnd = FALSE;
	m_bCheckUserGameStart = FALSE;
	m_bCheckUserCreateCharacter = FALSE;
	m_bCheckUserDeleteCharacter = FALSE;
	m_bCheckUserLevelChange = FALSE;
	m_bCheckUserPVPLose = FALSE;
	m_bCheckUserPVPWin = FALSE;
	m_bCheckUserQuestEnd = FALSE;
	m_bCheckUserSpeedHack = FALSE;
	m_bCheckUserMemoryHack = FALSE;
	m_bCheckUserStatChange = FALSE;
	m_bCheckUserWarp = FALSE;
	m_bCheckUserPvELoss			= FALSE;
	m_bCheckUserPvEWin			= FALSE;

	UpdateData(FALSE);
}

void CLogTabUser::OnBtnSaveResult() 
{
	// TODO: Add your control notification handler code here

	int nRowCount = m_GridUserLog.GetRowCount();
	if(1 == nRowCount)
	{
		MessageBox("No data !!");
		return;
	}

	CSystemLogManager resultLog;
	if(FALSE == resultLog.InitLogManger(TRUE, "UserLog", "./resultLog/"))
	{
		return;
	}

	for(int i=0; i < nRowCount; i++)
	{
		char szResult[2048];
		MEMSET_ZERO(szResult, 2048);
		sprintf(szResult, "%s;%s;%s;%s;%s;%s\r\n",
			m_GridUserLog.GetItemText(i, 0), m_GridUserLog.GetItemText(i, 1), m_GridUserLog.GetItemText(i, 2),
			m_GridUserLog.GetItemText(i, 3), m_GridUserLog.GetItemText(i, 4), m_GridUserLog.GetItemText(i, 5));
		resultLog.WriteSystemLog(szResult, FALSE);
	}
	MessageBox("Save success !!");
}

// 2011-04-20 by shcho, 러시아 로그에서 MemoryHackCheckType(14) 제거
size_t CLogTabUser::strpos(const string &haystack, const string &needle)
{
    int sleng = haystack.length();
    int nleng = needle.length();
	
    if (sleng==0 || nleng==0)
        return string::npos;
	
    for(int i=0, j=0; i<sleng; j=0, i++ )
    {
        while (i+j<sleng && j<nleng && haystack[i+j]==needle[j])
            j++;
        if (j==nleng)
            return NULL;
    }
    return string::npos;
}
//end 2011-04-20 by shcho, 러시아 로그에서 MemoryHackCheckType(14) 제거


void CLogTabUser::OnBnClickedButtonOk2()
{
	MessageBox("Help with hacking codes: \r \
 TIP: the codes are adding up opcode-like for one hack type i.e. 0001 + 0010 + 1000 = 1011 means that 3/4 checks were positive for detect hacking \r \
 ------------------------- \r \
 Code: 0001 / 0010 / 0100 / 1000 => Reattack \r \
 Code: 0002 / 0020 / 0200 / 2000 => Shot number \r \
 Code: 0003 / 0030 / 0300 / 3000 => Multi number \r \
 Code: 0004 / 0040 / 0400 / 4000 => Explosion range \r \
 Code: 0005 / 0050 / 05000 / 5000 => Engine speed max (non boosted) \r \
 Code: 0006 / 0060 / 0600 / 6000 => Engine speed min (non boosted) \r \
 Code: 0007 / 0070 / 0700 / 7000 => Engine rate of turn (booster turnrate) \r \
 Code: 0008 / 0080 / 0800 / 8000 => Engine on booster speed \r \
 Code: 10001 / 10010 / 10100 / 11000 => Engine valid angle \r \
 Code: 10002 / 10020 / 10200 / 12000 => Engine lateral speed \r \
 Code: 10003 / 10030 / 10300 / 13000 => Engine booster time \r \
 Code: 11000 => Engine des parameter added \r \
 Code: 20001 => Hacked Des parameter value in equipied item (Param1 - parameter num , Param2 - position in EQ) \r \
 Code: 20010 => Hacked Des parameter type in equipied item (Param1 - parameter num , Param2 - position in EQ)  \r \
 Code: 20011 => Hacked Des parameter type and value in equipied item (Param1 - parameter num , Param2 - position in EQ)  \r \
 Code: 30000 => Hacked gamble in primary weapon (Param1 - gamble code Param2- HackedParam(1/0) Param3- HackedValue(1/0)  \r \
 Code: 31000 => Hacked gamble in secondary weapon (Param1 - gamble code Param2- HackedParam(1/0) Param3- HackedValue(1/0)  \r \
 Code: 40000 => Client does not send heartbeat - probably replaced!\r \
 Code: 50000 => Hacked skill (ItemUID - SkillNumber Param1 - HackDetect(1/0) Param2- HackedParam(1/0) Param3- HackedValue(1/0)  \r \
 Code: 60000 => Missing ressource obligatory check file (Res-Obj) \r \
 Code: 65000 => Crossfeeding with the same IP player \r \
 Code: 70000 => Illegal program found (Cheat Engine, Macroo, RTSS, ArtMoney, MouseRecorder, AI Robot, AutoHotkey) \r \
 Code: 70001 => Check by registry for 'Cheat Engine' positive \r \
 Code: 80000 => Check by advanced anticheat not allowed DLL injection\r \
 Code: 80001 => Check by advanced anticheat - CheckNtGlobalFlag\r \
 Code: 80002 => Check by advanced anticheat - CheckIsDebuggerPresentFlag\r \
 Code: 80003 => Check by advanced anticheat - CheckHWBreakpoint\r \
 Code: 80004 => Check by advanced anticheat - CheckMainThreadSuspended\r \
 ------------------------- \r \
		", "Hacking codes",MB_OK|MB_ICONINFORMATION);
}
