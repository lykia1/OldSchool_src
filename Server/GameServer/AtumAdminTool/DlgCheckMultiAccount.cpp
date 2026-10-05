#include "stdafx.h"
#include "..\atumadmintool.h"
#include "AtumAdminToolDlg.h"
#include "DlgCheckMultiAccount.h"

CDlgMultiaccount::CDlgMultiaccount(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgMultiaccount::IDD, pParent)
{
	m_pMainDlg = (CAtumAdminToolDlg*)AfxGetMainWnd();

	nCheckOverCount = 2;
	m_bCheckShowWhitelist = FALSE;
}

CDlgMultiaccount::~CDlgMultiaccount()
{

}

void CDlgMultiaccount::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_GridControl(pDX, IDC_GRID_MULTIACC_LIST, m_Grid);
	DDX_Control(pDX, IDC_RADIO1, m_Btn1);
	DDX_Control(pDX, IDC_RADIO2, m_Btn2);
	DDX_Control(pDX, IDC_RADIO3, m_Btn3);
	DDX_Check(pDX, IDC_CHECK1, m_bCheckShowWhitelist);
}	
void CDlgMultiaccount::OnRadioBtn(UINT radioID)
{
	if (m_Btn1.GetCheck())
		nCheckOverCount =1;
		
	if (m_Btn2.GetCheck())
		nCheckOverCount = 2;

	if (m_Btn3.GetCheck())
		nCheckOverCount = 3;

	OnReaload();
}

BEGIN_MESSAGE_MAP(CDlgMultiaccount, CDialog)
	ON_BN_CLICKED(ID_UPDATE_TO_DB, OnBtnUpdateToDb)
	ON_BN_CLICKED(ID_UPDATE_TO_DB2, &CDlgMultiaccount::OnReaload)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_RADIO1, IDC_RADIO3, OnRadioBtn)
	ON_BN_CLICKED(IDC_CHECK1, &CDlgMultiaccount::OnCheckboxWhitelist)
END_MESSAGE_MAP()

void CDlgMultiaccount::_InitGrid(CGridCtrl* i_pGridCtrl)
{
	i_pGridCtrl->SetBkColor(0xFFFFFF);

	int nRows = 1;
	int nCols = 6;

	i_pGridCtrl->SetEditable(TRUE);
	i_pGridCtrl->SetListMode(TRUE);
	i_pGridCtrl->SetSingleRowSelection(TRUE);
	i_pGridCtrl->EnableSelection(TRUE);
	i_pGridCtrl->SetFrameFocusCell(FALSE);
	i_pGridCtrl->SetTrackFocusCell(FALSE);

	i_pGridCtrl->SetRowCount(nRows);
	i_pGridCtrl->SetColumnCount(nCols);
	i_pGridCtrl->SetFixedRowCount(1);

	nCols = 0;
	GV_ITEM Item;
	Item.mask = GVIF_TEXT | GVIF_FORMAT;
	Item.row = 0;
	Item.nFormat = GRID_CELL_FORMAT;

	Item.col = nCols++;
	Item.strText.Format("NUM");
	i_pGridCtrl->SetItem(&Item);

	Item.col = 1;
	Item.strText.Format("AccountUID");
	i_pGridCtrl->SetItem(&Item);

	Item.col = 2;
	Item.strText.Format("AccountName");
	i_pGridCtrl->SetItem(&Item);

	Item.col = 3;
	Item.strText.Format("IP");
	i_pGridCtrl->SetItem(&Item);

	Item.col = 4;
	Item.strText.Format("Last Login");
	i_pGridCtrl->SetItem(&Item);

	Item.col = 5;
	Item.strText.Format("Is whitelisted");
	i_pGridCtrl->SetItem(&Item);

	// arrange grid
	i_pGridCtrl->AutoSize();
	i_pGridCtrl->ExpandColumnsToFit();

	// clean all cells
	CCellRange tmpCellRange(1, 0, i_pGridCtrl->GetRowCount() - 1, i_pGridCtrl->GetColumnCount() - 1);
	i_pGridCtrl->ClearCells(tmpCellRange);
}

BOOL CDlgMultiaccount::DBQueryLoadData(vectMULTI_ACC_ROW* o_pVectRows)
{
	/*
	* USE [atum2_db_1]
GO

	SET ANSI_NULLS ON
		GO
		SET QUOTED_IDENTIFIER ON
		GO
		-- ============================================ =
		--Author:		<Author, , Name>
		--Create date : <Create Date, , >
		--Description : <Description, , >
		-- ============================================ =
		alter PROCEDURE[dbo].[inet_multiaccount_check]
		@MODE INT
		AS
		BEGIN
		SET NOCOUNT ON;

	With RankedMeasurements As
	(
		Select M.AccountName
		, M.Time
		, M.IPAddress
		, Row_Number() Over(Partition By M.AccountUniqueNumber
			Order By M.time Desc) As Rnk
		From atum2_db_account.dbo.atum_log_connection As M
		Where Exists(
			Select 1
			From atum2_db_account.dbo.td_account As FA1
			Where FA1.AccountUniqueNumber = M.AccountUniqueNumber
			And FA1.ConnectingServerGroupID > 0
			and DATEDIFF(second, M.Time, FA1.LastLoginDate) < 10 and logtype = 0)
	)
		select  distinct l.AccountUniQueNumber, r.AccountName, CAST(CAST(substring(r.IPAddress, 1, 1) AS int) AS nvarchar(3)) + '.' +
		CAST(CAST(substring(r.IPAddress, 2, 1) AS int) AS nvarchar(3)) + '.' +
		CAST(CAST(substring(r.IPAddress, 3, 1) AS int) AS nvarchar(3)) + '.' +
		CAST(CAST(substring(r.IPAddress, 4, 1) AS int) AS nvarchar(3)) as IP
		, MAX(r.Time) as Date,
		CASE WHEN EXISTS(select Accountname from atum2_db_account.dbo.td_Account_Whitelist e where e.AccountName = r.AccountName)
		THEN 'WHITELISTED'  ELSE '-' END as 'IsOnWhitelist'
		from atum2_db_account.dbo.atum_log_connection l, RankedMeasurements r where l.IPAddress in(Select
			RM.IPAddress
			From RankedMeasurements As RM
			Where Rnk = 1

			group by RM.IPAddress
			HAVING(COUNT(RM.IPAddress) > @MODE)) and l.AccountName = r.AccountName
		group by l.AccountUniQueNumber, r.AccountName, r.IPAddress



		END



*/
	SQLINTEGER arrCB[6] = { SQL_NTS,SQL_NTS,SQL_NTS,SQL_NTS,SQL_NTS,SQL_NTS };
	SQLHSTMT hstmt = m_odbcStmt2.GetSTMTHandle();
	SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_ULONG, SQL_INTEGER, 0, 0, &nCheckOverCount, 0, &arrCB[1]);
	BOOL bRet = m_odbcStmt2.ExecuteQuery((UCHAR*)"{call dbo.inet_multiaccount_check(?)}");
	if (!bRet)
	{
		m_odbcStmt2.FreeStatement();		// cleanup		
		AfxMessageBox("DBQueryLoad error !!");
		return FALSE;
	}
	
	MULTI_ACC_ROW tmRow;
	SQL_TIMESTAMP_STRUCT sqlTime;
	SQLBindCol(hstmt,1, SQL_C_ULONG, &tmRow.AccUID, 0, &arrCB[1]);
	SQLBindCol(hstmt, 2, SQL_C_CHAR, tmRow.AccountName, SIZE_MAX_ACCOUNT_NAME, &arrCB[2]);
	SQLBindCol(hstmt, 3, SQL_C_CHAR, tmRow.szIP, SIZE_MAX_IPADDRESS, &arrCB[3]);
	SQLBindCol(hstmt, 4, SQL_C_TIMESTAMP, &sqlTime, 30, &arrCB[4]);
	SQLBindCol(hstmt, 5, SQL_C_CHAR, tmRow.isWihteList, SIZE_MAX_ACCOUNT_NAME, &arrCB[5]);
	MEMSET_ZERO(&tmRow, sizeof(tmRow));
	while (SQLFetch(m_odbcStmt2.m_hstmt) != SQL_NO_DATA)
	{
		string tmpWL(tmRow.isWihteList);
		if (m_bCheckShowWhitelist && tmpWL.find("WHITELISTED") != string::npos)
			continue; //hide whitelisted

		tmRow.LastLogin = sqlTime;

		o_pVectRows->push_back(tmRow);
		MEMSET_ZERO(&tmRow, sizeof(tmRow));
	}

	m_odbcStmt2.FreeStatement();		// cleanup
	return TRUE;
}

void CDlgMultiaccount::_AddRow(CGridCtrl* i_pGridCtrl, MULTI_ACC_ROW* i_pData)
{
	GV_ITEM Item;
	printf("added %s \r\n", i_pData->AccountName);
	Item.mask = GVIF_TEXT | GVIF_FORMAT;
	Item.nFormat = DT_LEFT | DT_VCENTER | DT_SINGLELINE;

	int nNewRowIdx = i_pGridCtrl->GetRowCount();
	i_pGridCtrl->SetRowCount(nNewRowIdx + 1);

	Item.row = nNewRowIdx;
	Item.col = 0;
	Item.strText.Format("%d", nNewRowIdx);
	i_pGridCtrl->SetItem(&Item);


	Item.col = 1;
	Item.strText.Format("%d", i_pData->AccUID);
	i_pGridCtrl->SetItem(&Item);

	Item.col = 2;
	Item.strText.Format("%s", i_pData->AccountName);
	i_pGridCtrl->SetItem(&Item);

	Item.col = 3;
	Item.strText.Format("%s", i_pData->szIP);
	i_pGridCtrl->SetItem(&Item);

	Item.col = 4;
	Item.strText.Format("%s", i_pData->LastLogin.GetDateTimeString(STRNBUF(SIZE_MAX_ATUM_DATE_TIME_STRING)));
	i_pGridCtrl->SetItem(&Item);

	Item.col = 5;
	Item.strText.Format("%s", i_pData->isWihteList);
	i_pGridCtrl->SetItem(&Item);

}

void CDlgMultiaccount::ViewGrid(CGridCtrl* i_pGridCtrl)
{
	vectMULTI_ACC_ROW::iterator itr(m_vectResults.begin());
	for (; itr != m_vectResults.end(); itr++)
	{
		MULTI_ACC_ROW* pShopItem = &*itr;
		_AddRow(i_pGridCtrl, pShopItem);
	}
	i_pGridCtrl->UpdateData();
	i_pGridCtrl->AutoSize();
}

BOOL CDlgMultiaccount::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_Btn2.SetCheck(1);
	nCheckOverCount = 2;
	m_bCheckShowWhitelist = FALSE;
	CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK1);
	pBtn->SetCheck(0);

	// Connect DB
	if (FALSE == m_odbcStmt2.Init(m_pMainDlg->m_pServerInfo4Admin->DBIP, m_pMainDlg->m_pServerInfo4Admin->DBPort, m_pMainDlg->m_pServerInfo4Admin->DBName,
		m_pMainDlg->m_pServerInfo4Admin->DBUID, m_pMainDlg->m_pServerInfo4Admin->DBPWD, GetSafeHwnd()))
	{
		char szTemp[1024];
		sprintf(szTemp, "Can not connect DBServer<%s(%s:%d)> !!"
			, m_pMainDlg->m_pServerInfo4Admin->DBName, m_pMainDlg->m_pServerInfo4Admin->DBIP
			, m_pMainDlg->m_pServerInfo4Admin->DBPort);
		MessageBox(szTemp);
		EndDialog(-1);
		return FALSE;
	}

	m_vectResults.clear();
	this->DBQueryLoadData(&m_vectResults);
	this->_InitGrid(&m_Grid);
	this->ViewGrid(&m_Grid);

	return TRUE;
}
INT CDlgMultiaccount::AddWhiteList(const char* szAccName)
{
	/*
	 USE [atum2_db_1]
	GO

	SET ANSI_NULLS ON
	GO
	SET QUOTED_IDENTIFIER ON
	GO

	create PROCEDURE [dbo].[inet_multiaccount_whitelist_add]
		@i_AccName			VARCHAR(20)
	AS
	DECLARE @AccountUID INT
	SET @AccountUID = -1
	SET @AccountUID = (Select Top 1 AccountUniqueNumber from atum2_db_account.dbo.td_Account where AccountName = @i_AccName)

	IF(@AccountUID = -1)
	BEGIN
		SELECT 2
		RETURN
	END

	IF EXISTS (SELECT * from atum2_db_account.dbo.td_Account_Whitelist where AccountName =@i_AccName)
		BEGIN
			SELECT 1
		END
	ELSE
		BEGIN
			INSERT INTO atum2_db_account.dbo.td_Account_Whitelist VALUES(@AccountUID,@i_AccName)
			SELECT *,0 from atum2_db_account.dbo.td_Account_Whitelist where AccountName =@i_AccName
		END
*/

	SQLINTEGER arrCB[4] = { SQL_NTS,SQL_NTS,SQL_NTS,SQL_NTS };
	SQLHSTMT hstmt = m_odbcStmt2.GetSTMTHandle();

	SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, SIZE_MAX_ACCOUNT_NAME, 0, (char*)szAccName, 0, &arrCB[1]);
	BOOL bRet = m_odbcStmt2.ExecuteQuery((UCHAR*)"{call dbo.inet_multiaccount_whitelist_add(?)}");
	if (!bRet)
	{
		m_odbcStmt2.FreeStatement();		// cleanup	

		MessageBox("[DB Error] fail to execure Qry {call dbo.inet_multiaccount_whitelist_add }", "Error", MB_OK | MB_ICONERROR);
		return 3;
	}
	WHITE_LIST_ROW tmRow;
	MEMSET_ZERO(&tmRow, sizeof(tmRow));

	int nResult = 0;

	SQLBindCol(hstmt, 1, SQL_C_ULONG, &tmRow.AccUID, 0, &arrCB[1]);
	SQLBindCol(hstmt, 2, SQL_C_CHAR, tmRow.AccountName, SIZE_MAX_ACCOUNT_NAME, &arrCB[2]);
	SQLBindCol(hstmt, 3, SQL_C_ULONG, &nResult, 0, &arrCB[3]);
	SQLFetch(m_odbcStmt2.m_hstmt);

	if (nResult == 1)
	{
		MessageBox("[Error] Already on whitelist !\r\n", "Error",MB_OK|MB_ICONERROR);
		m_odbcStmt2.FreeStatement();
		return 1;
	}
	if (nResult == 2)
	{
		MessageBox("[Error] Account does not exist in account table !\r\n","Error", MB_OK | MB_ICONERROR);
		m_odbcStmt2.FreeStatement();
		return 2;
	}

	MEMSET_ZERO(&tmRow, sizeof(tmRow));
	m_odbcStmt2.FreeStatement();
	MessageBox("Added to white list successfully, REMEMBER to reload white list!\r\nI.e. use command /reloadWL in game from admin account\r\nTIP: You can use this command from Server&&Char live by AdminTool too!", "Info", MB_OK | MB_ICONINFORMATION);
	return 0;
}
void CDlgMultiaccount::OnBtnUpdateToDb()
{
	CString szAccNm;
	GetDlgItem(IDC_EDIT1)->GetWindowText(szAccNm);
	const char* c = szAccNm;
	if(AddWhiteList(c)==0)
		OnReaload();
}



void CDlgMultiaccount::OnReaload()
{
	m_vectResults.clear();
	this->DBQueryLoadData(&m_vectResults);
	this->_InitGrid(&m_Grid);
	this->ViewGrid(&m_Grid);
	MessageBox("Reloaded successfully!", "Info", MB_OK | MB_ICONINFORMATION);
}


void CDlgMultiaccount::OnCheckboxWhitelist()
{
	m_bCheckShowWhitelist = !m_bCheckShowWhitelist;
	OnReaload();
}
