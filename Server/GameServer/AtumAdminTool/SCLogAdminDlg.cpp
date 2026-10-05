// SCLogAdminDlg.cpp : implementation file
//

#include "stdafx.h"
#include "atumadmintool.h"
#include "SCLogAdminDlg.h"
#include "AtumAdminToolDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSCLogAdminDlg dialog


CSCLogAdminDlg::CSCLogAdminDlg(CAtumAdminToolDlg *i_pMainDlg, CWnd* pParent /* = NULL */)
	: CDialog(CSCLogAdminDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSCLogAdminDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_pMainDlg = i_pMainDlg;
	m_bCheckArchivesBackup = FALSE;
}


void CSCLogAdminDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSCLogAdminDlg)
	DDX_Control(pDX, IDC_TAB_LOG_ADMIN_TOOL, m_TabLogTool);
	DDX_Check(pDX, IDC_CHECK1, m_bCheckArchivesBackup);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSCLogAdminDlg, CDialog)
	//{{AFX_MSG_MAP(CSCLogAdminDlg)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, &CSCLogAdminDlg::OnBnClickedDOLogBackup)
	ON_BN_CLICKED(IDC_CHECK1, &CSCLogAdminDlg::OnBnClickedCheck1)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSCLogAdminDlg message handlers

BOOL CSCLogAdminDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	// Connect DB
	if (!m_ODBCStmt.Init(m_pMainDlg->m_pServerInfo4Admin->DBIP, m_pMainDlg->m_pServerInfo4Admin->DBPort, m_pMainDlg->m_pServerInfo4Admin->DBName,
							m_pMainDlg->m_pServerInfo4Admin->DBUID, m_pMainDlg->m_pServerInfo4Admin->DBPWD, GetSafeHwnd())
	)
	{
		MessageBox(STRERR_S_SCADMINTOOL_0013);
		EndDialog(-1);
		return FALSE;
	}

	m_TabLogTool.InsertItem(0, "Connection Log");
	m_TabLogTool.InsertItem(1, "User Log");
	m_TabLogTool.InsertItem(2, "CharacterItem Log");
	m_TabLogTool.InsertItem(3, "GuildItem Log");
	m_TabLogTool.InsertItem(4, "OSR Market Log");
	m_TabLogTool.InsertItem(5, "BlockAccount Log");			// 2008-01-30 by cmkwon, 계정 블럭 로그 남기기 구현 - 

	m_TabLogTool.Init(this);

	if (m_pMainDlg && m_pMainDlg->permCheck.CreateLogBackup)
		GetDlgItem(IDC_BUTTON1)->EnableWindow(TRUE);
	else
		GetDlgItem(IDC_BUTTON1)->EnableWindow(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

#define PROCEDURE_BACKUP_LOGS (UCHAR*)"{call dbo.atum_backup_logs}"
void CSCLogAdminDlg::OnBnClickedDOLogBackup()
{
	BOOL bRet = m_ODBCStmt.ExecuteQuery(PROCEDURE_BACKUP_LOGS);
	if (!bRet) {
		MessageBox("Cannot backup logs!", "Error", MB_OK | MB_ICONERROR);
		return;
	}
	m_ODBCStmt.FreeStatement();
	//success
	MessageBox("Successfully created log backups - now all logs untill now are moved to backup tables so tool would search much faster!\r\nTo search older logs tick \"Check also logs from backup (before last server start)\"", "Info", MB_OK | MB_ICONINFORMATION);
}


void CSCLogAdminDlg::OnBnClickedCheck1()
{
	m_bCheckArchivesBackup = !m_bCheckArchivesBackup;
}
