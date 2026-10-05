#include "stdafx.h"
#include "..\atumadmintool.h"
#include "AtumAdminToolDlg.h"
#include "DlgMultination.h"
#include "BlockAccountDlg.h"
CDlgMultination::CDlgMultination(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgMultination::IDD, pParent)
{
	m_pMainDlg = (CAtumAdminToolDlg*)AfxGetMainWnd();
	m_bCheckHideBanned = FALSE;
	m_bIsShowWhitelisted = FALSE;
	m_pUserAdminPreSocket = nullptr;
}

CDlgMultination::~CDlgMultination()
{

}

void CDlgMultination::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_GridControl(pDX, IDC_GRID_WPSHOP_LIST, m_Grid);
	DDX_Check(pDX, IDC_CHECK1, m_bCheckHideBanned);
	DDX_Check(pDX, IDC_CHECK3, m_bOnlyOnline);
	DDX_Check(pDX, IDC_CHECK8, m_bIsShowWhitelisted);
}


BEGIN_MESSAGE_MAP(CDlgMultination, CDialog)
	ON_BN_CLICKED(ID_UPDATE_TO_DB, OnSave)
	ON_BN_CLICKED(ID_UPDATE_TO_DB2, &CDlgMultination::OnDoReload)
	ON_BN_CLICKED(IDC_CHECK1, &CDlgMultination::OnBnHideBanned)
	ON_BN_CLICKED(IDC_CHECK3, &CDlgMultination::OnBnOnlyOnline)
	ON_BN_CLICKED(ID_UPDATE_TO_DB3, &CDlgMultination::OnBnClickedUpdateToDb3)
	ON_MESSAGE(WM_PRE_PACKET_NOTIFY, OnSocketNotifyPre)
		ON_MESSAGE(WM_PRE_ASYNC_EVENT, OnAsyncSocketMessage)
	ON_MESSAGE(WM_IM_ASYNC_EVENT, OnAsyncSocketMessage)
	ON_MESSAGE(WM_FIELD_ASYNC_EVENT, OnAsyncSocketMessage)
	ON_BN_CLICKED(IDC_CHECK8, &CDlgMultination::OnBnClickedCheck8)
	ON_BN_CLICKED(IDC_BUTTON1, &CDlgMultination::OnBnClickedButton1)
END_MESSAGE_MAP()

void CDlgMultination::_InitGrid(CGridCtrl* i_pGridCtrl)
{
	i_pGridCtrl->SetBkColor(0xFFFFFF);

	int nRows = 1;
	int nCols = 9;

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

	Item.col = 1;	// itemnum 
	Item.strText.Format("AccountName");
	i_pGridCtrl->SetItem(&Item);

	Item.col = 2;	// itemname
	Item.strText.Format("IP");
	i_pGridCtrl->SetItem(&Item);

	Item.col = 3;	// price 
	Item.strText.Format("Influence");
	i_pGridCtrl->SetItem(&Item);

	Item.col =4;	// price 
	Item.strText.Format("AccountUID");
	i_pGridCtrl->SetItem(&Item);

	Item.col = 5;
	Item.strText.Format("Is Banned");
	i_pGridCtrl->SetItem(&Item);

	Item.col = 6;
	Item.strText.Format("Is Online");
	i_pGridCtrl->SetItem(&Item);

	Item.col = 7;
	Item.strText.Format("Last Login");
	i_pGridCtrl->SetItem(&Item);
	
	Item.col = 8;
	Item.strText.Format("Whitelisted");
	i_pGridCtrl->SetItem(&Item);

	// arrange grid
	i_pGridCtrl->AutoSize();
	i_pGridCtrl->ExpandColumnsToFit();

	// clean all cells
	CCellRange tmpCellRange(1, 0, i_pGridCtrl->GetRowCount() - 1, i_pGridCtrl->GetColumnCount() - 1);
	i_pGridCtrl->ClearCells(tmpCellRange);
}

BOOL CDlgMultination::DBQueryLoadData(vectMULTI_NATION_ROW* o_pVectOutResults)
{
	/*USE [atum2_db_1]
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
		ALTER PROCEDURE[dbo].[inet_multination_check]
		AS
		BEGIN
		SET NOCOUNT ON;
	--use atum2_db_account
		--go

		DECLARE @DaysBack INT
		SET @DaysBack = 30 --set for how many days back to search

		delete from atum2_db_account.dbo.inet_multination_check
		insert into atum2_db_account.dbo.inet_multination_check select distinct(l.AccountUniqueNumber), CAST(CAST(substring(l.IPAddress, 1, 1) AS int) AS nvarchar(3)) + '.' +
		CAST(CAST(substring(l.IPAddress, 2, 1) AS int) AS nvarchar(3)) + '.' +
		CAST(CAST(substring(l.IPAddress, 3, 1) AS int) AS nvarchar(3)) + '.' +
		CAST(CAST(substring(l.IPAddress, 4, 1) AS int) AS nvarchar(3)) as IP,
		c.SelectableInfluenceMask

		from atum2_db_account.dbo.atum_log_connection l, atum2_db_1.dbo.td_character c
	where l.logtype = 1 and l.AccountUniqueNumber = c.AccountUniqueNumber and c.Race = 2 and c.SelectableInfluenceMask < > 6 and DATEDIFF(day, l.Time, GetDate()) < @DaysBack and c.Accountname group by  l.IPAddress, l.AccountUniqueNumber, c.SelectableInfluenceMask order by ip desc

	select a.AccountName, t.ip as 'IP',
		CASE WHEN c.SelectableInfluenceMask = 2 THEN 'BCU' WHEN c.SelectableInfluenceMask = 4 THEN 'ANI' WHEN c.SelectableInfluenceMask = 6 THEN 'FreeSKA' END as infl,
		a.AccountUniqueNumber as 'Account UID',
		CASE WHEN EXISTS(SELECT * FROM atum2_db_account.dbo.td_BlockedAccounts AS e
			WHERE e.EndDate > GetDate() and e.AccountName = a.AccountName)
		THEN 'BANNED'  ELSE '-' END as 'IsBanned',

		CASE WHEN  a.ConnectingServerGroupID > 0 THEN 'ONLINE' ELSE 'off' END as 'IsOn'
		, a.lastlogindate,
		CASE WHEN EXISTS(select Accountname from atum2_db_account.dbo.td_Account_Whitelist e where e.AccountName = r.AccountName)
		THEN 'WHITELISTED'  ELSE '-' END as 'IsOnWhitelist'
		from atum2_db_account.dbo.inet_multination_check t, atum2_db_account.dbo.td_account a, atum2_db_1.dbo.td_character  c

	where exists(
		select 1 from atum2_db_account.dbo.inet_multination_check
	where ip = t.ip and infl <> t.infl
		) and t.accuid = a.AccountUniqueNumber and c.AccountUniqueNumber = a.AccountUniqueNumber

		group by a.AccountName, t.ip, c.SelectableInfluenceMask, a.AccountUniqueNumber, a.ConnectingServerGroupID, a.lastlogindate
		order by IP Asc
		END



*/
	SQLHSTMT hstmt = m_odbcStmt2.GetSTMTHandle();
	BOOL bRet = m_odbcStmt2.ExecuteQuery((UCHAR*)"{call dbo.inet_multination_check}");
	if (!bRet)
	{
		m_odbcStmt2.FreeStatement();		// cleanup		
		AfxMessageBox("DBQueryLoad error !!");
		return FALSE;
	}
	SQLINTEGER arrCB[9] = { SQL_NTS,SQL_NTS,SQL_NTS,SQL_NTS,SQL_NTS,SQL_NTS,SQL_NTS,SQL_NTS,SQL_NTS };
	MULTI_NATION_ROW tmRow;
	SQL_TIMESTAMP_STRUCT sqlTime;
	SQLBindCol(hstmt, 1, SQL_C_CHAR, tmRow.AccountName, SIZE_MAX_ACCOUNT_NAME, &arrCB[1]);
	SQLBindCol(hstmt, 2, SQL_C_CHAR, tmRow.szIP, SIZE_MAX_IPADDRESS, &arrCB[2]);
	SQLBindCol(hstmt, 3, SQL_C_CHAR, tmRow.Influence, SIZE_MAX_ACCOUNT_NAME, &arrCB[3]);
	SQLBindCol(hstmt, 4, SQL_C_ULONG, &tmRow.AccUID, 0, &arrCB[4]);

	SQLBindCol(hstmt, 5, SQL_C_CHAR, tmRow.isBanned, SIZE_MAX_ACCOUNT_NAME, &arrCB[5]);
	SQLBindCol(hstmt, 6, SQL_C_CHAR, tmRow.isOnline, SIZE_MAX_ACCOUNT_NAME, &arrCB[6]);
	SQLBindCol(hstmt, 7, SQL_C_TIMESTAMP, &sqlTime, 30, &arrCB[7]);
	SQLBindCol(hstmt, 8, SQL_C_CHAR, tmRow.isWhitelisted, SIZE_MAX_ACCOUNT_NAME, &arrCB[7]);
	MEMSET_ZERO(&tmRow, sizeof(tmRow));
	while (SQLFetch(m_odbcStmt2.m_hstmt) != SQL_NO_DATA)
	{
		string tmpBan(tmRow.isBanned);
		string tmpOn(tmRow.isOnline);
		string tmpWL(tmRow.isWhitelisted);
		tmRow.LastLogin = sqlTime;

		if (m_bCheckHideBanned && tmpBan.find("BANNED") != string::npos)
			continue;

		if (m_bOnlyOnline && tmpOn.find("off") != string::npos)
			continue;
		
		if (!m_bIsShowWhitelisted && tmpWL.find("WHITELISTED") != string::npos)
			continue;
		
		o_pVectOutResults->push_back(tmRow);
		MEMSET_ZERO(&tmRow, sizeof(tmRow));
	}

	m_odbcStmt2.FreeStatement();		// cleanup
	return TRUE;
}


void CDlgMultination::_AddRow(CGridCtrl* i_pGridCtrl, MULTI_NATION_ROW* i_pData)
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
	Item.strText.Format("%s", i_pData->AccountName);
	i_pGridCtrl->SetItem(&Item);

	Item.col = 2;
	Item.strText.Format("%s", i_pData->szIP);
	i_pGridCtrl->SetItem(&Item);

	Item.col = 3;
	Item.strText.Format("%s", i_pData->Influence);
	i_pGridCtrl->SetItem(&Item);

	Item.col = 4;
	Item.strText.Format("%d", i_pData->AccUID);
	i_pGridCtrl->SetItem(&Item);

	Item.col = 5;
	Item.strText.Format("%s", i_pData->isBanned);
	i_pGridCtrl->SetItem(&Item);

	Item.col = 6;
	Item.strText.Format("%s", i_pData->isOnline);
	i_pGridCtrl->SetItem(&Item);

	Item.col = 7;
	Item.strText.Format("%s", i_pData->LastLogin.GetDateTimeString(STRNBUF(SIZE_MAX_ATUM_DATE_TIME_STRING)));
	i_pGridCtrl->SetItem(&Item);
	
	Item.col = 8;
	Item.strText.Format("%s", i_pData->isWhitelisted);
	i_pGridCtrl->SetItem(&Item);

}

void CDlgMultination::ViewGrid(CGridCtrl* i_pGridCtrl)
{
	vectMULTI_NATION_ROW::iterator itr(m_vectResults.begin());
	for (; itr != m_vectResults.end(); itr++)
	{
		MULTI_NATION_ROW* pShopItem = &*itr;
		_AddRow(i_pGridCtrl, pShopItem);
	}
	i_pGridCtrl->UpdateData();
	i_pGridCtrl->AutoSize();
}

BOOL CDlgMultination::OnInitDialog()
{
	CDialog::OnInitDialog();
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



	m_pUserAdminPreSocket = new CSCAdminPreWinSocket("CSCUserAdminDlg's PreServer Socket", this, GetSafeHwnd());
	if (!m_pUserAdminPreSocket->Connect(m_pMainDlg->m_pServerInfo4Admin->ServerIP, PRE_SERVER_PORT))
	{
		int err = GetLastError();
		MessageBox(STRERR_S_SCADMINTOOL_0005);
		SAFE_DELETE(m_pUserAdminPreSocket);
		EndDialog(-1);
		return FALSE;
	}

	return TRUE;
}

void CDlgMultination::OnSave()
{
	AfxMessageBox("Save not possible yet");
	return;

	m_vectResults.clear();
	this->DBQueryLoadData(&m_vectResults);
	this->ViewGrid(&m_Grid);

	AfxMessageBox("Been successfully applied to the database.!! (Applies to restart the server)");
}



void CDlgMultination::OnDoReload()
{
	m_vectResults.clear();
	this->DBQueryLoadData(&m_vectResults);
	this->_InitGrid(&m_Grid);
	this->ViewGrid(&m_Grid);
	MessageBox("Reloaded successfully!","Info",MB_OK|MB_ICONINFORMATION);
}


void CDlgMultination::OnBnHideBanned()
{
	m_bCheckHideBanned = !m_bCheckHideBanned;
	OnDoReload();
}


void CDlgMultination::OnBnOnlyOnline()
{
	m_bOnlyOnline = !m_bOnlyOnline;
	OnDoReload();
}


void CDlgMultination::OnBnClickedUpdateToDb3()
{//m_pMainDlg->getaccou
	CBlockAccountDlg dlg{};
	if (dlg.DoModal() != IDOK)
	{
		return;
	}
	auto tmBlInfo = dlg.GetBlockedInfo();
	if(tmBlInfo){
		INIT_MSG_WITH_BUFFER(MSG_PA_ADMIN_BLOCK_ACCOUNT, T_PA_ADMIN_BLOCK_ACCOUNT, pBlock, SendBuf);
		STRNCPY_MEMSET(pBlock->szBlockedAccountName, tmBlInfo->szBlockedAccountName, SIZE_MAX_ACCOUNT_NAME);
		pBlock->enBlockedType			= (EN_BLOCKED_TYPE)tmBlInfo->enBlockedType;
		pBlock->atimeStartTime			= tmBlInfo->atimeStartTime;
		pBlock->atimeEndTime			= tmBlInfo->atimeEndTime;
		STRNCPY_MEMSET(pBlock->szBlockAdminAccountName, tmBlInfo->szBlockAdminAccountName, SIZE_MAX_ACCOUNT_NAME);
		STRNCPY_MEMSET(pBlock->szBlockedReasonForUser, tmBlInfo->szBlockedReasonForUser, SIZE_MAX_BLOCKED_ACCOUNT_REASON);
		STRNCPY_MEMSET(pBlock->szBlockedReasonForOnlyAdmin, tmBlInfo->szBlockedReasonForOnlyAdmin, SIZE_MAX_BLOCKED_ACCOUNT_REASON);
		m_pUserAdminPreSocket->Write(SendBuf, MSG_SIZE(MSG_PA_ADMIN_BLOCK_ACCOUNT));
	}
}
LONG CDlgMultination::OnSocketNotifyPre(WPARAM wParam, LPARAM lParam)
{
	CSCAdminWinSocket *pSCAdminWinSocket = (CSCAdminWinSocket*)lParam;

	switch(LOWORD(wParam))
	{
	case CWinSocket::WS_ERROR:
		{
		}
		break;
	case CWinSocket::WS_CONNECTED:
		{
			if (HIWORD(wParam) == TRUE)
			{
				// 연결 성공
//				MessageBox("Pre Server에 연결하였습니다.");
				INIT_MSG_WITH_BUFFER(MSG_PA_ADMIN_CONNECT, T_PA_ADMIN_CONNECT, msgConnect, msgConnectBuf);
				STRNCPY_MEMSET(msgConnect->UID, m_pMainDlg->m_UID, SIZE_MAX_ACCOUNT_NAME);
				STRNCPY_MEMSET(msgConnect->PWD, m_pMainDlg->m_PWD, SIZE_MAX_PASSWORD);
				msgConnect->Padding = m_pMainDlg->n_ATVersion;
				m_pUserAdminPreSocket->Write(msgConnectBuf, MSG_SIZE(MSG_PA_ADMIN_CONNECT));
			}
			else
			{
				// 연결 실패
				MessageBox(STRERR_S_SCADMINTOOL_0006);
				OnClose();
			}
		}
		break;
	case CWinSocket::WS_RECEIVED:
		{
			MessageType_t	msgType;

			char			*pPacket = NULL;
			int				len;
			pSCAdminWinSocket->Read(&pPacket, len);

			if (pPacket)
			{
				msgType = *(MessageType_t*)(pPacket);

				switch(msgType)
				{
				case T_PA_ADMIN_CONNECT_OK:
					{
						MSG_PA_ADMIN_CONNECT_OK *msgConnectOK
							= (MSG_PA_ADMIN_CONNECT_OK*)(pPacket+SIZE_FIELD_TYPE_HEADER);

// 2006-04-15 by cmkwon
//						if (!msgConnectOK->AuthOK)
						if (0 == msgConnectOK->AccountType0)
						{
							if (msgConnectOK->Padding == 1) {
								MessageBox("You're using outdated AdminTool, Contact Inetpub to get new one!", "Error", MB_OK | MB_ICONERROR);
								EndDialog(-1);
							}

							MessageBox("Fail PreServer2 certification!!");
							OnCancel();
						}
					}
					break;
				case T_PA_ADMIN_BLOCK_ACCOUNT_OK:
					{
						MSG_PA_ADMIN_BLOCK_ACCOUNT_OK *pRMsg = (MSG_PA_ADMIN_BLOCK_ACCOUNT_OK*)(pPacket+SIZE_FIELD_TYPE_HEADER);
						char szTmp[1024];
						sprintf(szTmp,"Blocked account %s!",pRMsg->szBlockedAccountName);
						MessageBox(szTmp,"Info",MB_OK|MB_ICONINFORMATION);
					}
					break;
				case T_PA_ADMIN_UNBLOCK_ACCOUNT_OK:
					{
						
					}
					break;
				default:
					{
					}
					break;
				}
			}

			SAFE_DELETE(pPacket);
		}
		break;
	}	

	return 0;
}

LONG CDlgMultination::OnAsyncSocketMessage(WPARAM wParam, LPARAM lParam)
{
	m_pUserAdminPreSocket->OnAsyncEvent(lParam);

	return 0;
}


void CDlgMultination::OnBnClickedCheck8()
{
	m_bIsShowWhitelisted = !m_bIsShowWhitelisted;
	OnDoReload();
}


void CDlgMultination::OnBnClickedButton1()
{
	CString szAccNm;
	GetDlgItem(IDC_EDIT1)->GetWindowText(szAccNm);
	const char* c = szAccNm;
	if(AddWhiteList(c)==0)
		OnDoReload();
}
INT CDlgMultination::AddWhiteList(const char* szAccName)
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
	WHITE_LIST_ROW_MN tmRow;
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