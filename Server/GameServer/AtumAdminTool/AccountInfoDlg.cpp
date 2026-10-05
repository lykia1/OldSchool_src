// AccountInfoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "atumadmintool.h"
#include "AccountInfoDlg.h"
#include "AtumParam.h"
#include "md5_lib_src.h"
#include "SCUserAdminDlg.h"		// 2008-01-31 by cmkwon, °čÁ¤ şí·°/ÇŘÁ¦ ¸í·Éľî·Î °ˇ´ÉÇŃ ˝Ă˝şĹŰ ±¸Çö - 

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAccountInfoDlg dialog


CAccountInfoDlg::CAccountInfoDlg(BOOL i_bEnableEdit, CWnd* pParent /*=NULL*/)
	: CDialog(CAccountInfoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAccountInfoDlg)
	m_szAccountName = _T("");
	m_szPassword = _T("");
	m_szRegisterdDate = _T("");
	m_szLastLoginDate = _T("");
	m_bAccountBlocked = FALSE;
	m_bChattingBlocked = FALSE;
	m_ctlcsSecPassword = _T("");
	//}}AFX_DATA_INIT

	m_nAcountType		= 0;
	m_bEnableEdit		= i_bEnableEdit;	// 2006-04-15 by cmkwon

	m_szEmail = _T("");
	m_bActive = FALSE;
	m_nCashPoint = 0;
	m_nTotalDonatedEur = 0;
}


void CAccountInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAccountInfoDlg)
	DDX_Control(pDX, IDC_COMBO_RACE_ACC_TYPE2, m_ComboAccountType);
	DDX_Text(pDX, IDC_EDIT_CHARACTER_NAME, m_szAccountName);
	DDX_Text(pDX, IDC_EDIT_PASSWORD, m_szPassword);

	DDX_Text(pDX, IDC_EDIT_REG_DATE, m_szRegisterdDate);
	DDX_Text(pDX, IDC_EDIT_LAST_LOGIN_DATE, m_szLastLoginDate);
	DDX_Check(pDX, IDC_CHECK_ACCOUNT_BLOCKED, m_bAccountBlocked);
	DDX_Check(pDX, IDC_CHECK_CHATTING_BLOCKED, m_bChattingBlocked);
	DDX_Text(pDX, IDC_EDIT_SECPASSWORD, m_ctlcsSecPassword);

	DDX_Text(pDX, IDC_EDIT_EMAIL, m_szEmail);
	DDX_Check(pDX, IDC_CHECK_ACTIVE_ACC, m_bActive);
	DDX_Text(pDX, IDC_EDIT_CASHPOINT, m_nCashPoint);
	DDX_Text(pDX, IDC_EDIT_CASHPOINT2, m_nTotalDonatedEur);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAccountInfoDlg, CDialog)
	//{{AFX_MSG_MAP(CAccountInfoDlg)
	ON_BN_CLICKED(IDOK, OnOk)
	ON_BN_CLICKED(IDC_BUTTON_MD5_PWD, OnButtonMd5Pwd)
	ON_BN_CLICKED(IDC_BUTTON_MD5_SECPWD, OnButtonMd5Secpwd)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAccountInfoDlg message handlers

#define RACE_ACC_TYPE_COMBO_NO_TYPE			0 // (ŔĎąÝ)
#define RACE_ACC_TYPE_COMBO_OPERATION		1 // (°ü¸®ŔÚ)
#define RACE_ACC_TYPE_COMBO_GAMEMASTER		2 // (°ÔŔÓ¸¶˝şĹÍ)
#define RACE_ACC_TYPE_COMBO_MONITOR			3 // (¸đ´ĎĹÍ)
#define RACE_ACC_TYPE_COMBO_GUEST			4 // (°Ô˝şĆ®)
#define RACE_ACC_TYPE_COMBO_DEMO			5 // (µĄ¸đ)


BOOL CAccountInfoDlg::OnInitDialog() 
{
	/*
	 * USE [atum2_db_1]
	GO

	SET ANSI_NULLS ON
	GO
	SET QUOTED_IDENTIFIER ON
	GO
	ALTER PROCEDURE [dbo].[atum_PROCEDURE_080827_0001]
		@i_AccName			VARCHAR(20)
	AS
		SELECT AccountName, Password, AccountType, RegisteredDate, LastLoginDate,
				IsBlocked, ChattingBlocked, SecondaryPassword
				--07-05-2023 by Inetpub
				,Email,Active,CashPoint
		FROM atum2_db_account.dbo.td_account WITH (NOLOCK)
		where accountname = @i_AccName;

	 */
	CDialog::OnInitDialog();
	m_ComboAccountType.AddString(STRCMD_CS_COMMON_RACE_NORMAL);
	m_ComboAccountType.AddString(STRCMD_CS_COMMON_RACE_OPERATION);
	m_ComboAccountType.AddString(STRCMD_CS_COMMON_RACE_GAMEMASTER);
	m_ComboAccountType.AddString(STRCMD_CS_COMMON_RACE_MONITOR);
	m_ComboAccountType.AddString(STRCMD_CS_COMMON_RACE_GUEST);
	m_ComboAccountType.AddString(STRCMD_CS_COMMON_RACE_DEMO);

	CString szQuery;
	SQLHSTMT hstmt = m_pODBCStmt->GetSTMTHandle();
	SQLINTEGER arrCB2[2] = {SQL_NTS,SQL_NTS};
	SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, SIZE_MAX_ACCOUNT_NAME, 0, (LPSTR)(LPCSTR)m_szAccountName, 0, &arrCB2[1]);
	BOOL bRet = m_pODBCStmt->ExecuteQuery((char*)(PROCEDURE_080827_0001));
	if (!bRet)
	{
		MessageBox("Cannot exec procedure");
		m_pODBCStmt->FreeStatement();
		OnCancel();
		return TRUE;
	}

	// 2007-09-13 by cmkwon, şŁĆ®ł˛ 2Â÷ĆĐ˝şżöµĺ ˝Ă˝şĹŰ ±¸Çö - SCAdminTool ĽöÁ¤
	SQLINTEGER arrCB[12] = {SQL_NTS,SQL_NTS,SQL_NTS,SQL_NTS,SQL_NTS, SQL_NTS,SQL_NTS,SQL_NTS,SQL_NTS,SQL_NTS,
							SQL_NTS,SQL_NTS };

	char szAccountName[SIZE_MAX_ACCOUNT_NAME];
	char szPassword[SIZE_MAX_PASSWORD_MD5_STRING]; memset(szPassword, 0, SIZE_MAX_PASSWORD_MD5_STRING);

	SQL_TIMESTAMP_STRUCT RegDate, LastLoginDate;
	memset(&RegDate, 0, sizeof(SQL_TIMESTAMP_STRUCT));
	memset(&LastLoginDate, 0, sizeof(SQL_TIMESTAMP_STRUCT));
	BYTE nIsBlocked, nChattingBlocked, bIsActive;
	ATUM_DATE_TIME tmpDateTime;
	char szSecPassword[SIZE_MAX_PASSWORD_MD5_STRING]; MEMSET_ZERO(szSecPassword, SIZE_MAX_PASSWORD_MD5_STRING);	// 2007-09-13 by cmkwon, şŁĆ®ł˛ 2Â÷ĆĐ˝şżöµĺ ˝Ă˝şĹŰ ±¸Çö - SCAdminTool ĽöÁ¤
	char szEmail[50]; MEMSET_ZERO(szEmail,50);
	INT nCashPoint,nTotalEur;

	SQLBindCol(m_pODBCStmt->m_hstmt, 1, SQL_C_CHAR, szAccountName, SIZE_MAX_ACCOUNT_NAME,			&arrCB[1]);
	SQLBindCol(m_pODBCStmt->m_hstmt, 2, SQL_C_CHAR, szPassword, SIZE_MAX_PASSWORD_MD5_STRING,		&arrCB[2]);
	SQLBindCol(m_pODBCStmt->m_hstmt, 3, SQL_C_USHORT, &m_nAcountType, 0,							&arrCB[3]);
	SQLBindCol(m_pODBCStmt->m_hstmt, 4, SQL_C_TIMESTAMP, &RegDate, 0,								&arrCB[4]);
	SQLBindCol(m_pODBCStmt->m_hstmt, 5, SQL_C_TIMESTAMP, &LastLoginDate, 0,							&arrCB[5]);
	SQLBindCol(m_pODBCStmt->m_hstmt, 6, SQL_C_TINYINT, &nIsBlocked, 0,								&arrCB[6]);
	SQLBindCol(m_pODBCStmt->m_hstmt, 7, SQL_C_TINYINT, &nChattingBlocked, 0,						&arrCB[7]);
	SQLBindCol(m_pODBCStmt->m_hstmt, 8, SQL_C_CHAR, szSecPassword, SIZE_MAX_PASSWORD_MD5_STRING,	&arrCB[8]);	// 2007-09-13 by cmkwon, şŁĆ®ł˛ 2Â÷ĆĐ˝şżöµĺ ˝Ă˝şĹŰ ±¸Çö - SCAdminTool ĽöÁ¤, °ˇÁ®żŔ±â ÇĘµĺ Ăß°ˇ

	//07-05-2023 by Inet - added email, activation and cashpoint
	SQLBindCol(m_pODBCStmt->m_hstmt, 9, SQL_C_CHAR, szEmail, 50,									&arrCB[9]);
	SQLBindCol(m_pODBCStmt->m_hstmt, 10, SQL_C_TINYINT, &bIsActive, 0,									&arrCB[10]);
	SQLBindCol(m_pODBCStmt->m_hstmt, 11, SQL_C_ULONG, &nCashPoint, 0,									&arrCB[11]);
	SQLBindCol(m_pODBCStmt->m_hstmt, 12, SQL_C_ULONG, &nTotalEur, 0,									&arrCB[12]);


	if (SQL_NO_DATA == SQLFetch(m_pODBCStmt->GetSTMTHandle()))
	{
		MessageBox(STRERR_S_SCADMINTOOL_0000);
		m_pODBCStmt->FreeStatement();
		OnCancel();
		return TRUE;
	}

	// free statement
	m_pODBCStmt->FreeStatement();

	m_szPassword = szPassword;
	///////////////////////////////////////////////////////////////////////////////
	// 2005-12-13 by cmkwon, ±ÇÇŃ ą× ÁľÁ·
	if (m_nAcountType == 0) m_ComboAccountType.SetCurSel(RACE_ACC_TYPE_COMBO_NO_TYPE);
	else if (COMPARE_RACE(m_nAcountType, RACE_OPERATION)) m_ComboAccountType.SetCurSel(RACE_ACC_TYPE_COMBO_OPERATION);
	else if (COMPARE_RACE(m_nAcountType, RACE_GAMEMASTER)) m_ComboAccountType.SetCurSel(RACE_ACC_TYPE_COMBO_GAMEMASTER);
	else if (COMPARE_RACE(m_nAcountType, RACE_MONITOR)) m_ComboAccountType.SetCurSel(RACE_ACC_TYPE_COMBO_MONITOR);
	else if (COMPARE_RACE(m_nAcountType, RACE_GUEST)) m_ComboAccountType.SetCurSel(RACE_ACC_TYPE_COMBO_GUEST);
	else if (COMPARE_RACE(m_nAcountType, RACE_DEMO)) m_ComboAccountType.SetCurSel(RACE_ACC_TYPE_COMBO_DEMO);

	tmpDateTime = RegDate;
	m_szRegisterdDate = tmpDateTime.GetDateTimeString(STRNBUF(SIZE_MAX_ATUM_DATE_TIME_STRING));
	tmpDateTime = LastLoginDate;
	m_szLastLoginDate = tmpDateTime.GetDateTimeString(STRNBUF(SIZE_MAX_ATUM_DATE_TIME_STRING));
	m_bAccountBlocked = nIsBlocked;
	m_bChattingBlocked = nChattingBlocked;
	m_ctlcsSecPassword	= szSecPassword;		// 2007-09-13 by cmkwon, şŁĆ®ł˛ 2Â÷ĆĐ˝şżöµĺ ˝Ă˝şĹŰ ±¸Çö - SCAdminTool ĽöÁ¤

	m_szEmail = szEmail;
	m_bActive = bIsActive;
	m_nCashPoint = nCashPoint;
	m_nTotalDonatedEur = nTotalEur;

	UpdateData(FALSE);

	if(FALSE == m_bEnableEdit)
	{// 2006-04-15 by cmkwon, ĽöÁ¤ ±ÇÇŃ Ăł¸®
		GetDlgItem(IDC_EDIT_PASSWORD)->EnableWindow(m_bEnableEdit);
		GetDlgItem(IDC_BUTTON_MD5_PWD)->EnableWindow(m_bEnableEdit);
		GetDlgItem(IDC_COMBO_RACE_ACC_TYPE2)->EnableWindow(m_bEnableEdit);
		GetDlgItem(IDC_EDIT_SECPASSWORD)->EnableWindow(m_bEnableEdit);			// 2007-10-02 by cmkwon, SCAdminTool ĽöÁ¤ ±ÇÇŃ Ăł¸® - ĽöÁ¤ ±ÇÇŃŔĚ ľřŔ»¶§ şńČ°ĽşČ­
		GetDlgItem(IDC_BUTTON_MD5_SECPWD)->EnableWindow(m_bEnableEdit);			// 2007-10-02 by cmkwon, SCAdminTool ĽöÁ¤ ±ÇÇŃ Ăł¸® - ĽöÁ¤ ±ÇÇŃŔĚ ľřŔ»¶§ şńČ°ĽşČ­
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAccountInfoDlg::OnOk() 
{
	/*
	 *
	 * USE [atum2_db_1]
	GO

	SET ANSI_NULLS ON
	GO
	SET QUOTED_IDENTIFIER ON
	GO

	ALTER PROCEDURE [dbo].[atum_PROCEDURE_080827_0002]
		@i_AccName			VARCHAR(20),
		@i_AccType			SMALLINT,
		@i_Password			VARCHAR(33),
		@i_SecPassword		VARCHAR(33),
	--07-05-2023 by Inet
		@i_Email			VARCHAR(50),
		@b_Active			BIT,
		@n_CashPoint		INT
	AS
		UPDATE atum2_db_account.dbo.td_account 
		SET AccountType = @i_AccType, Password = @i_Password, SecondaryPassword = @i_SecPassword
		, email = @i_Email, Active = @b_Active, CashPoint = @n_CashPoint
		WHERE AccountName = @i_AccName;

	 */
	if(FALSE == m_bEnableEdit) {
		CDialog::OnCancel();
		return;
	}

	CSCUserAdminDlg *pSCUserAdminDlg = (CSCUserAdminDlg *)this->GetParent();
	if(FALSE == pSCUserAdminDlg->IsEnabledEdit()) {
		AfxMessageBox("Now, you can't update AccountInfo !! Retry");
		CDialog::OnCancel();
		return;
	}

	// update data
	UpdateData();

	// Account Type ĽłÁ¤
	if (m_ComboAccountType.GetCurSel() == RACE_ACC_TYPE_COMBO_NO_TYPE) m_nAcountType = 0;
	else if (m_ComboAccountType.GetCurSel() == RACE_ACC_TYPE_COMBO_OPERATION) m_nAcountType = RACE_OPERATION;
	else if (m_ComboAccountType.GetCurSel() == RACE_ACC_TYPE_COMBO_GAMEMASTER) m_nAcountType = RACE_GAMEMASTER;
	else if (m_ComboAccountType.GetCurSel() == RACE_ACC_TYPE_COMBO_MONITOR) m_nAcountType = RACE_MONITOR;
	else if (m_ComboAccountType.GetCurSel() == RACE_ACC_TYPE_COMBO_GUEST) m_nAcountType = RACE_GUEST;
	else if (m_ComboAccountType.GetCurSel() == RACE_ACC_TYPE_COMBO_DEMO) m_nAcountType = RACE_DEMO;

	CString szQuery;

	SQLHSTMT hstmt = m_pODBCStmt->GetSTMTHandle();
	SQLINTEGER arrCB2[8] = {SQL_NTS,SQL_NTS,SQL_NTS,SQL_NTS,SQL_NTS,SQL_NTS,SQL_NTS,SQL_NTS};
	SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, SIZE_MAX_ACCOUNT_NAME, 0, (LPSTR)(LPCSTR)m_szAccountName, 0,			&arrCB2[1]);
	SQLBindParameter(hstmt, 2, SQL_PARAM_INPUT, SQL_C_USHORT, SQL_SMALLINT, 0, 0, &m_nAcountType, 0,											&arrCB2[2]);
	SQLBindParameter(hstmt, 3, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, SIZE_MAX_PASSWORD_MD5_STRING, 0, (LPSTR)(LPCSTR)m_szPassword, 0,		&arrCB2[3]);
	SQLBindParameter(hstmt, 4, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, SIZE_MAX_PASSWORD_MD5_STRING, 0, (LPSTR)(LPCSTR)m_ctlcsSecPassword, 0,	&arrCB2[4]);

	SQLBindParameter(hstmt, 5, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 50, 0, (LPSTR)(LPCSTR)m_szEmail, 0,	&arrCB2[5]);
	SQLBindParameter(hstmt, 6, SQL_PARAM_INPUT, SQL_C_BIT, SQL_SMALLINT, 0, 0, &m_bActive, 0,	&arrCB2[6]);
	SQLBindParameter(hstmt, 7, SQL_PARAM_INPUT, SQL_C_ULONG, SQL_INTEGER, 0, 0, &m_nCashPoint, 0,	&arrCB2[7]);

	BOOL bRet = m_pODBCStmt->ExecuteQuery((char*)(PROCEDURE_080827_0002));
	if (!bRet) {
		MessageBox(STRERR_S_SCADMINTOOL_0001);
	}

	CDialog::OnOK();
}

void CAccountInfoDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

void CAccountInfoDlg::OnButtonMd5Pwd() 
{
	// TODO: Add your control notification handler code here
	UpdateData();

	if(m_szPassword.IsEmpty())
	{
		AfxMessageBox("Input Password !!");
		return;
	}

	MD5 MD5_instance;
	unsigned char md5_string[16];
	char szEncodedString[33];

	MD5_instance.MD5Encode((char*)(LPCSTR)m_szPassword, md5_string);
	MD5_instance.MD5Binary2String(md5_string, szEncodedString);

	// MD5 ľĎČŁ ĽłÁ¤
	m_szPassword = szEncodedString;

	UpdateData(FALSE);
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			void CAccountInfoDlg::OnButtonMd5Secpwd() 
/// \brief		
/// \author		cmkwon
/// \date		2007-09-13 ~ 2007-09-13
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CAccountInfoDlg::OnButtonMd5Secpwd() 
{
	// TODO: Add your control notification handler code here
	UpdateData();

	if(m_ctlcsSecPassword.IsEmpty())
	{
		AfxMessageBox("Input SecondaryPassword !!");
		return;
	}

	MD5 MD5_instance;
	unsigned char md5_string[16];
	char szEncodedString[33];

	MD5_instance.MD5Encode((char*)(LPCSTR)m_ctlcsSecPassword, md5_string);
	MD5_instance.MD5Binary2String(md5_string, szEncodedString);

	// MD5 ľĎČŁ ĽłÁ¤
	m_ctlcsSecPassword = szEncodedString;

	UpdateData(FALSE);
	
}
