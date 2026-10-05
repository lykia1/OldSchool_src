// LogTabServerIntegration.cpp : implementation file
//

#include "stdafx.h"
#include "atumadmintool.h"
#include "LogTabServerIntegration.h"
#include "SCLogAdminDlg.h"
#include "AtumProtocol.h"
#include "AtumSJ.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLogTabServerIntegration dialog

void MakeCurrencySeparator(char* pDest, char* pSrc, int nSepInterval, char cSepChar)
{
	int        nSrcLen = strlen(pSrc);
	int        nDestLen;
	char* pFindDot = strchr(pSrc, '.');
	int        nDotPos;

	// Error
	if (!nSepInterval) return;

	// Initial
	if (pFindDot == NULL)
	{
		nDotPos = nSrcLen;
		nDestLen = nSrcLen + nSrcLen / nSepInterval;
	}
	else
	{
		nDotPos = pFindDot - pSrc;
		nDestLen = nSrcLen + nDotPos / nSepInterval;
	}

	memset(pDest, NULL, nDestLen + 1);

	// Copy to destination buffer
	int        nCurDestPos = nDestLen - 1;
	int        nCount = 0;

	for (int i = nSrcLen - 1; i >= 0; i--)
	{
		if (!(nCount % nSepInterval) && nCount)
		{
			pDest[nCurDestPos] = cSepChar;
			nCurDestPos--;
		}

		pDest[nCurDestPos] = pSrc[i];

		if (i < nDotPos)    nCount++;
		nCurDestPos--;
	}

	// Adjusting
	if (*pDest == NULL)
	{
		memcpy(pDest, pDest + 1, nDestLen + 1);
	}
}
CLogTabServerIntegration::CLogTabServerIntegration(CDialog *i_pMainDlg, CWnd* pParent /*=NULL*/)
	: CDialog(CLogTabServerIntegration::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLogTabServerIntegration)
	m_szAccountName = _T("");
	m_bCheckAccountName = FALSE;
	m_dateEnd = CTime::GetCurrentTime();
	m_timeEnd = CTime::GetCurrentTime();
	m_dateStart = (CTime::GetCurrentTime() - CTimeSpan(1, 0 , 0, 0));
	m_timeStart = CTime::GetCurrentTime();
	m_bDate = TRUE;
	m_bCheckAccountUID = FALSE;
	m_nAccountUID = 0;
	m_bCheckcharUID = FALSE;
	m_bCheckCharName = FALSE;
	m_bCheckItemUID = FALSE;
	//chkExpired = TRUE;
	//chkSold = TRUE;
	//chkPaidRecall = TRUE;
	//chkOthers = FALSE;
	m_szCharName = _T("");
	m_nCharUID = 0;
	m_nItemUID = 0;
	
	m_nTotalLogCounts = 0;
	//}}AFX_DATA_INIT
	m_pMainDlg = (CSCLogAdminDlg*)i_pMainDlg;
	m_pODBCStmt = &m_pMainDlg->m_ODBCStmt;
}

void CLogTabServerIntegration::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLogTabServerIntegration)
	DDX_Text(pDX, IDC_EDIT_ACCOUNTNAME, m_szAccountName);
	DDX_Check(pDX, IDC_CHECK_AccountName, m_bCheckAccountName);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_END_DATE, m_dateEnd);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_END_TIME, m_timeEnd);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_START_DATE, m_dateStart);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_START_TIME, m_timeStart);
	DDX_Check(pDX, IDC_CHECK_DATE, m_bDate);
	DDX_Check(pDX, IDC_CHECK_AccountUID, m_bCheckAccountUID);
	DDX_Text(pDX, IDC_EDIT_ACCOUNTUID, m_nAccountUID);

	DDX_Check(pDX, IDC_CHECK_AccountUID2, m_bBuyerCheckAccountUID);
	DDX_Text(pDX, IDC_EDIT_ACCOUNTUID2, m_nBuyerAccountUID);

	DDX_Check(pDX, IDC_CHECK_AccountName6, m_bBuyerCheckAccName);
	DDX_Text(pDX, IDC_EDIT_ACCOUNTNAME6, m_szBuyerAccName);

	DDX_Check(pDX, IDC_CHECK_AccountName2, m_bCheckcharUID);
	DDX_Text(pDX, IDC_EDIT_ACCOUNTNAME2, m_nCharUID);

	DDX_Check(pDX, IDC_CHECK_AccountName3, m_bCheckCharName);
	DDX_Text(pDX, IDC_EDIT_ACCOUNTNAME3, m_szCharName);
	
	DDX_Check(pDX, IDC_CHECK_AccountName4, m_bBuyerCheckcharUID);
	DDX_Text(pDX, IDC_EDIT_ACCOUNTNAME4, m_nBuyerCharUID);

	DDX_Check(pDX, IDC_CHECK_AccountName5, m_bBuyerCheckCharName);
	DDX_Text(pDX, IDC_EDIT_ACCOUNTNAME5, m_szBuyerCharName);

	DDX_Check(pDX, IDC_CHECK_SOURCE_DB_NUM, m_bCheckItemUID);
	DDX_Text(pDX, IDC_EDIT_SOURCE_DB_NUM, m_nItemUID);

	DDX_Text(pDX, IDC_EDIT_TOTAL_LOG_COUNTS, m_nTotalLogCounts);


	//DDX_Control(pDX, IDC_CHECK4, chkExpired);
	//DDX_Control(pDX, IDC_CHECK5, chkSold);
	//DDX_Control(pDX, IDC_CHECK6, chkPaidRecall);
	//DDX_Control(pDX, IDC_CHECK7, chkOthers);


	//}}AFX_DATA_MAP
	DDX_GridControl(pDX, IDC_GRID_SERVERINTEGRATION_LOG, m_GridServerIntegrationLog);
}


BEGIN_MESSAGE_MAP(CLogTabServerIntegration, CDialog)
	//{{AFX_MSG_MAP(CLogTabServerIntegration)
	ON_BN_CLICKED(IDC_BUTTON_SEARCH, OnButtonSearch)
	ON_BN_CLICKED(IDC_CHECK_AccountName, OnCHECKAccountName)
	ON_BN_CLICKED(IDC_CHECK_DATE, OnCheckDate)
	ON_BN_CLICKED(IDC_CHECK_AccountUID, OnCHECKAccountUID)
	/*ON_BN_CLICKED(IDC_CHECK_SOURCE_CHARACTER_NAME, OnCheckSourceCharacterName)
	ON_BN_CLICKED(IDC_CHECK_SOURCE_CHARACTER_UID, OnCheckSourceCharacterUid)
	ON_BN_CLICKED(IDC_CHECK_SOURCE_DB_NUM, OnCheckSourceDbNum)
	ON_BN_CLICKED(IDC_CHECK_TARGET_CHARACTER_NAME, OnCheckTargetCharacterName)
	ON_BN_CLICKED(IDC_CHECK_TARGET_CHARACTER_UID, OnCheckTargetCharacterUid)
	ON_BN_CLICKED(IDC_CHECK_TARGET_DB_NUM, OnCheckTargetDbNum)*/
	ON_BN_CLICKED(IDC_BUTTON_RESET, OnButtonReset)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnButtonSave)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_RADIO1, &CLogTabServerIntegration::OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, &CLogTabServerIntegration::OnBnClickedRadio2)
	ON_BN_CLICKED(IDC_RADIO3, &CLogTabServerIntegration::OnBnClickedRadio3)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLogTabServerIntegration message handlers

BOOL CLogTabServerIntegration::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
	{
		m_pMainDlg->EndDialog(-1);
		return TRUE;
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}
 
BOOL CLogTabServerIntegration::OnInitDialog() 
{
	CDialog::OnInitDialog();

	radioRegister = (CButton*)GetDlgItem(IDC_RADIO1);
	radioRecall = (CButton*)GetDlgItem(IDC_RADIO2);
	radioTraded = (CButton*)GetDlgItem(IDC_RADIO3);

	chkExpired = (CButton*)GetDlgItem(IDC_CHECK4);
	chkSold = (CButton*)GetDlgItem(IDC_CHECK5);
	chkPaidRecall = (CButton*)GetDlgItem(IDC_CHECK6);
	chkOthers = (CButton*)GetDlgItem(IDC_CHECK7);

	chkExpired->SetCheck(TRUE);
	chkSold->SetCheck(TRUE);
	chkPaidRecall->SetCheck(TRUE);
	chkOthers->SetCheck(FALSE);

	this->InitGrid();
	this->ResetVariables();
	this->ResetControls();



	radioRegister->SetCheck(1);

	ArrangeControls();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CLogTabServerIntegration::ResetVariables()
{
	m_nAccountUID = 0;
	m_bCheckAccountUID = FALSE;
	m_szAccountName = _T("");
	m_bCheckAccountName = FALSE;
	m_dateEnd = CTime::GetCurrentTime();
	m_timeEnd = CTime::GetCurrentTime();
	m_dateStart = (CTime::GetCurrentTime() - CTimeSpan(1, 0 , 0, 0));
	m_timeStart = CTime::GetCurrentTime();
	m_bDate = TRUE;
	m_bCheckAccountUID = FALSE;
	m_nAccountUID = 0;
	m_bCheckcharUID = FALSE;
	m_bCheckCharName = FALSE;
	m_bCheckItemUID = FALSE;

	m_szCharName = _T("");
	m_nCharUID = 0;
	m_nItemUID = 0;
	m_nTotalLogCounts = 0;

	UpdateData(FALSE);
}

void CLogTabServerIntegration::ResetControls()
{
	GetDlgItem(IDC_EDIT_ACCOUNTUID)->EnableWindow(m_bCheckAccountUID);
	GetDlgItem(IDC_EDIT_ACCOUNTNAME)->EnableWindow(m_bCheckAccountName);
	GetDlgItem(IDC_DATETIMEPICKER_START_DATE)->EnableWindow(m_bDate);
	GetDlgItem(IDC_DATETIMEPICKER_END_DATE)->EnableWindow(m_bDate);
	GetDlgItem(IDC_DATETIMEPICKER_START_TIME)->EnableWindow(m_bDate);
	GetDlgItem(IDC_DATETIMEPICKER_END_TIME)->EnableWindow(m_bDate);	
}

void CLogTabServerIntegration::InitGrid()
{
	m_GridServerIntegrationLog.SetBkColor(0xFFFFFF);

	int m_nRows = 1;
	int m_nCols = 10;
	if(radioRecall->GetCheck()) {
		m_nCols = 11;
	}

	m_GridServerIntegrationLog.SetEditable(FALSE);
	m_GridServerIntegrationLog.SetListMode(TRUE);
	m_GridServerIntegrationLog.SetSingleRowSelection(TRUE);
	m_GridServerIntegrationLog.EnableSelection(TRUE);
	m_GridServerIntegrationLog.SetFrameFocusCell(FALSE);
	m_GridServerIntegrationLog.SetTrackFocusCell(FALSE);

	m_GridServerIntegrationLog.SetRowCount(m_nRows);
	m_GridServerIntegrationLog.SetColumnCount(m_nCols);
	m_GridServerIntegrationLog.SetFixedRowCount(1);
// 2008-03-03 by cmkwon, 서버군 통합 로그 타입 보여주기 - 
//	m_GridServerIntegrationLog.SetColumnWidth(0,0);

	// 칼럼 만들기
	m_nCols = 0;
	GV_ITEM Item;
	Item.mask = GVIF_TEXT|GVIF_FORMAT;
	Item.row = 0;
	Item.nFormat = DT_LEFT|DT_VCENTER|DT_SINGLELINE;

// 2008-03-03 by cmkwon, 서버군 통합 로그 타입 보여주기 - 
// 	Item.col = m_nCols++;
// 	Item.strText.Format("Number");
// 	m_GridServerIntegrationLog.SetItem(&Item);
 	Item.col = m_nCols++;
 	Item.strText.Format("Log Type");
 	m_GridServerIntegrationLog.SetItem(&Item);

	Item.col = m_nCols++;
	Item.strText.Format("ItemUID");
	m_GridServerIntegrationLog.SetItem(&Item);
	if (radioTraded->GetCheck()) {
		Item.col = m_nCols++;
		Item.strText.Format("Seller name");
		m_GridServerIntegrationLog.SetItem(&Item);

		Item.col = m_nCols++;
		Item.strText.Format("Buyer name");
		m_GridServerIntegrationLog.SetItem(&Item);

		Item.col = m_nCols++;
		Item.strText.Format("Item");
		m_GridServerIntegrationLog.SetItem(&Item);

		Item.col = m_nCols++;
		Item.strText.Format("Price");
		m_GridServerIntegrationLog.SetItem(&Item);

		Item.col = m_nCols++;
		Item.strText.Format("Seller UID");
		m_GridServerIntegrationLog.SetItem(&Item);

		Item.col = m_nCols++;
		Item.strText.Format("Buyer UID");
		m_GridServerIntegrationLog.SetItem(&Item);
	}
	else
	{
		Item.col = m_nCols++;
		Item.strText.Format("Character name");
		m_GridServerIntegrationLog.SetItem(&Item);

		Item.col = m_nCols++;
		Item.strText.Format("Account name");
		m_GridServerIntegrationLog.SetItem(&Item);

		Item.col = m_nCols++;
		Item.strText.Format("Item");
		m_GridServerIntegrationLog.SetItem(&Item);

		Item.col = m_nCols++;
		Item.strText.Format("Price");
		m_GridServerIntegrationLog.SetItem(&Item);

		Item.col = m_nCols++;
		Item.strText.Format("Character UID");
		m_GridServerIntegrationLog.SetItem(&Item);

		Item.col = m_nCols++;
		Item.strText.Format("Account UID");
		m_GridServerIntegrationLog.SetItem(&Item);
	}
	Item.col = m_nCols++;
	Item.strText.Format("AuctionUID");
	m_GridServerIntegrationLog.SetItem(&Item);

	Item.col = m_nCols++;
	Item.strText.Format("Time");
	m_GridServerIntegrationLog.SetItem(&Item);

	if (radioRecall->GetCheck()) {
		Item.col = m_nCols++;
		Item.strText.Format("MarketStatus");
		m_GridServerIntegrationLog.SetItem(&Item);
	}

	// arrange grid
	m_GridServerIntegrationLog.AutoSize();
	m_GridServerIntegrationLog.ExpandColumnsToFit();

	// clean all cells
	CCellRange tmpCellRange(1, 0, m_GridServerIntegrationLog.GetRowCount()-1, m_GridServerIntegrationLog.GetColumnCount()-1);
	m_GridServerIntegrationLog.ClearCells(tmpCellRange);	
}

BOOL CLogTabServerIntegration::MakeSzQueryForComplete()
{
	SzQuery.Empty();

	if (TRUE == m_bCheckAccountUID)
	{
		if(FALSE == SzQuery.IsEmpty())
		{
			SzQuery += " and";
		}
		CString	TempSzQuery;
		TempSzQuery.Format(" AccountUID = %d", m_nAccountUID);
		SzQuery += TempSzQuery;
	}
	if (TRUE == m_bCheckcharUID)
	{
		if (FALSE == SzQuery.IsEmpty())
		{
			SzQuery += " and";
		}
		CString	TempSzQuery;
		TempSzQuery.Format(" CharUID = %d", m_nCharUID);
		SzQuery += TempSzQuery;
	}
	if (TRUE == m_bCheckCharName)
	{
		if (FALSE == SzQuery.IsEmpty())
		{
			SzQuery += " and";
		}
		CString	TempSzQuery;
		TempSzQuery.Format(" CharName = '%s'", m_szCharName);
		SzQuery += TempSzQuery;
	}
	if (TRUE == m_bBuyerCheckcharUID)
	{
		if (FALSE == SzQuery.IsEmpty())
		{
			SzQuery += " and";
		}
		CString	TempSzQuery;
		TempSzQuery.Format(" BuyerCharUID = %d", m_nBuyerCharUID);
		SzQuery += TempSzQuery;
	}
	if (TRUE == m_bBuyerCheckCharName)
	{
		if (FALSE == SzQuery.IsEmpty())
		{
			SzQuery += " and";
		}
		CString	TempSzQuery;
		TempSzQuery.Format(" BuyerCharName = '%s'", m_szBuyerCharName);
		SzQuery += TempSzQuery;
	}
	if (TRUE == m_bCheckItemUID)
	{
		if (FALSE == SzQuery.IsEmpty())
		{
			SzQuery += " and";
		}
		CString	TempSzQuery;
		TempSzQuery.Format(" MarketItemUID = %I64d", m_nItemUID);
		SzQuery += TempSzQuery;
	}
	if (TRUE == m_bCheckAccountName)
	{
		if(FALSE == SzQuery.IsEmpty())
		{
			SzQuery += " and";
		}
		CString	TempSzQuery;
		TempSzQuery.Format(" AccName = '%s'", m_szAccountName);
		SzQuery += TempSzQuery;
	}
	if (TRUE == m_bBuyerCheckAccName)
	{
		if(FALSE == SzQuery.IsEmpty())
		{
			SzQuery += " and";
		}
		CString	TempSzQuery;
		TempSzQuery.Format(" BuyerAccName = '%s'", m_szBuyerAccName);
		SzQuery += TempSzQuery;
	}
	if (TRUE == m_bBuyerCheckAccountUID)
	{
		if (FALSE == SzQuery.IsEmpty())
		{
			SzQuery += " and";
		}
		CString	TempSzQuery;
		TempSzQuery.Format(" BuyerAccountUID = %d", m_nBuyerAccountUID);
		SzQuery += TempSzQuery;
	}
	
	if (TRUE == m_bDate)
	{
		if(FALSE == SzQuery.IsEmpty())
		{
			SzQuery += " and";
		}
		CString	TempSzQuery;
		TempSzQuery.Format(" Time >= '%d-%d-%d %d:%d:%d' and Time <= '%d-%d-%d %d:%d:%d'", 
			m_dateStart.GetYear(), m_dateStart.GetMonth(), m_dateStart.GetDay(),
			m_timeStart.GetHour(), m_timeStart.GetMinute(), m_timeStart.GetSecond(),
			m_dateEnd.GetYear(), m_dateEnd.GetMonth(), m_dateEnd.GetDay(),
			m_timeEnd.GetHour(), m_timeEnd.GetMinute(), m_timeEnd.GetSecond());
		SzQuery += TempSzQuery;
	}

	if (TRUE == SzQuery.IsEmpty())
	{
		return FALSE;
	}

	return TRUE;
}

void CLogTabServerIntegration::OnButtonSearch() 
{
	// TODO: Add your control notification handler code here
	this->InitGrid();
	UpdateData(TRUE);
	char szAccName[SIZE_MAX_SZQUERY];
	MEMSET_ZERO(szAccName,SIZE_MAX_SZQUERY);
	/************************************************************************
	atum_log_select_market_trade
	atum_log_select_market_recall
	atum_log_select_market_register
	************************************************************************/
	if(TRUE == this->MakeSzQueryForComplete())
	{
		strcpy(szAccName, SzQuery);
	}
	
	SQLBindParameter(m_pODBCStmt->m_hstmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, SIZE_MAX_SZQUERY, 0, szAccName, 0, NULL);
	RETCODE ret;
	if(radioRegister->GetCheck())
		ret = m_pODBCStmt->ExecuteQuery((UCHAR*)"{call dbo.atum_log_select_market_register (?)}");
	else if (radioRecall->GetCheck())
		ret = m_pODBCStmt->ExecuteQuery((UCHAR*)"{call dbo.atum_log_select_market_recall (?)}");
	else if (radioTraded->GetCheck())
		ret = m_pODBCStmt->ExecuteQuery((UCHAR*)"{call dbo.atum_log_select_market_trade (?)}");

	if (FALSE == ret)
	{
		SQLFreeStmt(m_pODBCStmt->m_hstmt, SQL_CLOSE);
		return;
	}

	SQLINTEGER arrCB[19] = {SQL_NTS,SQL_NTS,SQL_NTS,SQL_NTS,SQL_NTS, SQL_NTS,SQL_NTS,SQL_NTS,SQL_NTS,SQL_NTS
							,SQL_NTS,SQL_NTS,SQL_NTS,SQL_NTS,SQL_NTS, SQL_NTS,SQL_NTS,SQL_NTS,SQL_NTS };

	MARKET_BUY_LOG_FOR_AT	tmRetMarket;
	MEMSET_ZERO(&tmRetMarket, sizeof(tmRetMarket));
	BYTE byLogType = 0;
	SQL_TIMESTAMP_STRUCT tmpTime;

	SQLBindCol(m_pODBCStmt->m_hstmt, 1, SQL_C_TINYINT, &tmRetMarket.byLogType, 0,									&arrCB[1]);
	SQLBindCol(m_pODBCStmt->m_hstmt, 2, SQL_C_TIMESTAMP, &tmpTime, 0, &arrCB[2]);
	SQLBindCol(m_pODBCStmt->m_hstmt, 3, SQL_C_CHAR, tmRetMarket.szCharacterName, SIZE_MAX_CHARACTER_NAME, &arrCB[3]);
	SQLBindCol(m_pODBCStmt->m_hstmt, 4, SQL_C_CHAR, tmRetMarket.szAccountName, SIZE_MAX_ACCOUNT_NAME, &arrCB[4]);
	SQLBindCol(m_pODBCStmt->m_hstmt, 5, SQL_C_LONG, &tmRetMarket.CharacterUID, 0, &arrCB[5]);
	SQLBindCol(m_pODBCStmt->m_hstmt, 6, SQL_C_LONG, &tmRetMarket.AccountUID, 0, &arrCB[6]);

	if (radioRegister->GetCheck())
		SQLBindCol(m_pODBCStmt->m_hstmt, 7, SQL_C_LONG, &tmRetMarket.MarketItemUID, 0, &arrCB[7]);
	else
		SQLBindCol(m_pODBCStmt->m_hstmt, 7, SQL_C_SBIGINT, &tmRetMarket.ChargePrice, 0, &arrCB[7]);

	SQLBindCol(m_pODBCStmt->m_hstmt, 8, SQL_C_LONG, &tmRetMarket.ItemNum, 0, &arrCB[8]);
	SQLBindCol(m_pODBCStmt->m_hstmt, 9, SQL_C_CHAR, tmRetMarket.szItemName, 140, &arrCB[9]);
	SQLBindCol(m_pODBCStmt->m_hstmt, 10, SQL_C_LONG, &tmRetMarket.ItemCount, 0, &arrCB[10]);
	SQLBindCol(m_pODBCStmt->m_hstmt, 11, SQL_C_LONG, &tmRetMarket.Price, 0, &arrCB[11]);
	SQLBindCol(m_pODBCStmt->m_hstmt, 12, SQL_C_CHAR, &tmRetMarket.szMoneyType, 20, &arrCB[12]);
	SQLBindCol(m_pODBCStmt->m_hstmt, 13, SQL_C_SBIGINT, &tmRetMarket.MarketUID, 0, &arrCB[13]);
	SQLBindCol(m_pODBCStmt->m_hstmt, 14, SQL_C_SBIGINT, &tmRetMarket.MarketItemUID, 0, &arrCB[14]);
	if (radioTraded->GetCheck()) {
		SQLBindCol(m_pODBCStmt->m_hstmt, 15, SQL_C_CHAR, tmRetMarket.szBuyerCharName, SIZE_MAX_CHARACTER_NAME, &arrCB[15]);
		SQLBindCol(m_pODBCStmt->m_hstmt, 16, SQL_C_CHAR, tmRetMarket.szBuyerAccountName, SIZE_MAX_ACCOUNT_NAME, &arrCB[16]);
		SQLBindCol(m_pODBCStmt->m_hstmt, 17, SQL_C_LONG, &tmRetMarket.BuyerCharacterUID, 0, &arrCB[17]);
		SQLBindCol(m_pODBCStmt->m_hstmt, 18, SQL_C_LONG, &tmRetMarket.BuyerAccountUID, 0, &arrCB[18]);
	}
	if (radioRecall->GetCheck()) {
		SQLBindCol(m_pODBCStmt->m_hstmt, 15, SQL_C_SBIGINT, &tmRetMarket.byMarketStatus, 0, &arrCB[15]);
		
	}
	m_vectMARKET_BUY_LOG_FOR_AT.clear();
	// DB에 값이 없을때까지 loop를 돈다
	while ( (ret = SQLFetch(m_pODBCStmt->m_hstmt)) != SQL_NO_DATA)
	{
		tmRetMarket.Time		= tmpTime;
		m_vectMARKET_BUY_LOG_FOR_AT.push_back(tmRetMarket);

		MEMSET_ZERO(&tmRetMarket, sizeof(tmRetMarket));
	}
	m_pODBCStmt->FreeStatement();	// clean up

	m_nTotalLogCounts = m_vectMARKET_BUY_LOG_FOR_AT.size();

	GV_ITEM Item;
	Item.mask = GVIF_TEXT|GVIF_FORMAT;
	Item.row = 1;
	Item.nFormat = GRID_CELL_FORMAT;

	for(int i=0; i < m_nTotalLogCounts; i++)
	{
		if(m_vectMARKET_BUY_LOG_FOR_AT[i].byMarketStatus == MARKET_STATE_SELL && !chkPaidRecall->GetCheck())
			continue;

		if (m_vectMARKET_BUY_LOG_FOR_AT[i].byMarketStatus == MARKET_STATE_SELL_DONE && !chkSold->GetCheck())
			continue;

		if (m_vectMARKET_BUY_LOG_FOR_AT[i].byMarketStatus == MARKET_STATE_TIME_OUT && !chkExpired->GetCheck())
			continue;

		if (m_vectMARKET_BUY_LOG_FOR_AT[i].byMarketStatus > MARKET_STATE_TIME_OUT && !chkOthers->GetCheck())
			continue;

		int nNewRowIdx = m_GridServerIntegrationLog.GetRowCount();
		m_GridServerIntegrationLog.SetRowCount(nNewRowIdx+1);

		//select된 값을 GridDetail GridCtrl에 넣어준다.
		Item.row		= nNewRowIdx;
		Item.col		= 0;
		// 2008-03-03 by cmkwon, 서버군 통합 로그 타입 보여주기 - 
		//Item.strText.Format("%d", "Number");
		Item.strText.Format("%s", GetGameLogTypeString(m_vectMARKET_BUY_LOG_FOR_AT[i].byLogType));
		m_GridServerIntegrationLog.SetItem(&Item);
					
		Item.col		= 1;
		Item.strText.Format("%I64d", m_vectMARKET_BUY_LOG_FOR_AT[i].MarketItemUID);
		m_GridServerIntegrationLog.SetItem(&Item);
		
		char szTmpRecallType[50];
		char cOut[48];
		char cOut2[48];
		char cOut3[48];
		char cTotalPriceChar[48];
		char cChargePriceChar[48];
		char cTotalPriceChar1[48];
		sprintf(cTotalPriceChar, "%d", m_vectMARKET_BUY_LOG_FOR_AT[i].Price - m_vectMARKET_BUY_LOG_FOR_AT[i].ChargePrice);
		MakeCurrencySeparator(cOut, cTotalPriceChar, 3, ',');

		sprintf(cChargePriceChar, "%d", m_vectMARKET_BUY_LOG_FOR_AT[i].ChargePrice);
		MakeCurrencySeparator(cOut2, cChargePriceChar, 3, ',');
		
		sprintf(cTotalPriceChar1, "%d", m_vectMARKET_BUY_LOG_FOR_AT[i].Price);
		MakeCurrencySeparator(cOut3, cTotalPriceChar1, 3, ',');

		if (radioTraded->GetCheck()) {
			Item.col = 2;
			Item.strText.Format("%s(%s)", m_vectMARKET_BUY_LOG_FOR_AT[i].szCharacterName, m_vectMARKET_BUY_LOG_FOR_AT[i].szAccountName);
			m_GridServerIntegrationLog.SetItem(&Item);

			Item.col = 3;
			Item.strText.Format("%s(%s)", m_vectMARKET_BUY_LOG_FOR_AT[i].szBuyerCharName, m_vectMARKET_BUY_LOG_FOR_AT[i].szBuyerAccountName);
			m_GridServerIntegrationLog.SetItem(&Item);

			Item.col = 4;
			Item.strText.Format("%s(%d) x %d", m_vectMARKET_BUY_LOG_FOR_AT[i].szItemName, m_vectMARKET_BUY_LOG_FOR_AT[i].ItemNum, m_vectMARKET_BUY_LOG_FOR_AT[i].ItemCount);
			m_GridServerIntegrationLog.SetItem(&Item);

			Item.col = 5;
			Item.strText.Format("%s %s (-%s)", cOut3, m_vectMARKET_BUY_LOG_FOR_AT[i].szMoneyType, cOut2);
			m_GridServerIntegrationLog.SetItem(&Item);

			Item.col = 6;
			Item.strText.Format("%d(%d)", m_vectMARKET_BUY_LOG_FOR_AT[i].CharacterUID, m_vectMARKET_BUY_LOG_FOR_AT[i].AccountUID);
			m_GridServerIntegrationLog.SetItem(&Item);

			Item.col = 7;
			Item.strText.Format("%d(%d)", m_vectMARKET_BUY_LOG_FOR_AT[i].BuyerCharacterUID, m_vectMARKET_BUY_LOG_FOR_AT[i].BuyerAccountUID);
			m_GridServerIntegrationLog.SetItem(&Item);

		}
		else
		{
			Item.col = 2;
			Item.strText.Format("%s", m_vectMARKET_BUY_LOG_FOR_AT[i].szCharacterName);
			m_GridServerIntegrationLog.SetItem(&Item);

			Item.col = 3;
			Item.strText.Format("%s", m_vectMARKET_BUY_LOG_FOR_AT[i].szAccountName);
			m_GridServerIntegrationLog.SetItem(&Item);

			Item.col = 4;
			Item.strText.Format("%s(%d) x %d", m_vectMARKET_BUY_LOG_FOR_AT[i].szItemName, m_vectMARKET_BUY_LOG_FOR_AT[i].ItemNum, m_vectMARKET_BUY_LOG_FOR_AT[i].ItemCount);
			m_GridServerIntegrationLog.SetItem(&Item);

			if (radioRegister->GetCheck())
			{
				Item.col = 5;
				Item.strText.Format("%s %s", cOut3, m_vectMARKET_BUY_LOG_FOR_AT[i].szMoneyType);
				m_GridServerIntegrationLog.SetItem(&Item);
			}
			else
			{
				Item.col = 5;
				Item.strText.Format("%s %s (-%s)", cOut3, m_vectMARKET_BUY_LOG_FOR_AT[i].szMoneyType, cOut2);
				m_GridServerIntegrationLog.SetItem(&Item);
			}

			Item.col = 6;
			Item.strText.Format("%d", m_vectMARKET_BUY_LOG_FOR_AT[i].CharacterUID);
			m_GridServerIntegrationLog.SetItem(&Item);

			Item.col = 7;
			Item.strText.Format("%d", m_vectMARKET_BUY_LOG_FOR_AT[i].AccountUID);
			m_GridServerIntegrationLog.SetItem(&Item);

			
		}
		Item.col = 8;
		Item.strText.Format("%I64d", m_vectMARKET_BUY_LOG_FOR_AT[i].MarketUID);
		m_GridServerIntegrationLog.SetItem(&Item);

		Item.col = 9;
		Item.strText.Format("%s", m_vectMARKET_BUY_LOG_FOR_AT[i].Time.GetDateTimeString(STRNBUF(SIZE_MAX_ATUM_DATE_TIME_STRING)));
		m_GridServerIntegrationLog.SetItem(&Item);
		bool bCode = false;
		Item.col = 10;
		if (radioRecall->GetCheck()) {

			switch (m_vectMARKET_BUY_LOG_FOR_AT[i].byMarketStatus)
			{
				case MARKET_STATE_SELL: //paid
				{
					sprintf(szTmpRecallType,"Paid %s %s for recall", cOut2, m_vectMARKET_BUY_LOG_FOR_AT[i].szMoneyType);
				}
				break;
				case MARKET_STATE_TIME_OUT: //7d passed
				{
					sprintf(szTmpRecallType, "7Day passed - free recall");
				}
				break;
				case MARKET_STATE_SELL_DONE: //sold
				{
					sprintf(szTmpRecallType, "Sold %s %s withdrawed", cOut, m_vectMARKET_BUY_LOG_FOR_AT[i].szMoneyType);
				}
				break;
				default:
				{
					sprintf(szTmpRecallType, "- ");
					bCode = true;
				}
				break;
			}

			if(bCode)
				Item.strText.Format("Status code: %d",m_vectMARKET_BUY_LOG_FOR_AT[i].byMarketStatus);
			else
				Item.strText.Format("%s", szTmpRecallType);

			m_GridServerIntegrationLog.SetItem(&Item);
		}
	}
	
	m_GridServerIntegrationLog.UpdateData();
	m_GridServerIntegrationLog.AutoSize();

	UpdateData(FALSE);
}

void CLogTabServerIntegration::OnCHECKAccountUID() 
{
	// TODO: Add your control notification handler code here
	UpdateData();

	GetDlgItem(IDC_EDIT_ACCOUNTUID)->EnableWindow(m_bCheckAccountUID);	
}

void CLogTabServerIntegration::OnCHECKAccountName() 
{
	// TODO: Add your control notification handler code here
	UpdateData();

	GetDlgItem(IDC_EDIT_ACCOUNTNAME)->EnableWindow(m_bCheckAccountName);
}

void CLogTabServerIntegration::OnCheckDate() 
{
	// TODO: Add your control notification handler code here
	UpdateData();

	GetDlgItem(IDC_DATETIMEPICKER_START_DATE)->EnableWindow(m_bDate);
	GetDlgItem(IDC_DATETIMEPICKER_END_DATE)->EnableWindow(m_bDate);
	GetDlgItem(IDC_DATETIMEPICKER_START_TIME)->EnableWindow(m_bDate);
	GetDlgItem(IDC_DATETIMEPICKER_END_TIME)->EnableWindow(m_bDate);	
}

void CLogTabServerIntegration::OnCheckSourceCharacterName() 
{
	// TODO: Add your control notification handler code here
	UpdateData();

	//GetDlgItem(IDC_EDIT_SOURCE_CHARACTER_NAME)->EnableWindow(m_bCheckSourceCharacterName);
}

void CLogTabServerIntegration::OnCheckSourceCharacterUid() 
{
	// TODO: Add your control notification handler code here
	UpdateData();

	//GetDlgItem(IDC_EDIT_SOURCE_CHARACTER_UID)->EnableWindow(m_bCheckSourceCharacterUID);	
}

void CLogTabServerIntegration::OnCheckSourceDbNum() 
{
	// TODO: Add your control notification handler code here
	UpdateData();

	//GetDlgItem(IDC_EDIT_SOURCE_DB_NUM)->EnableWindow(m_bCheckSourceDBNum);
}

void CLogTabServerIntegration::OnCheckTargetCharacterName() 
{
	// TODO: Add your control notification handler code here
	UpdateData();

	//GetDlgItem(IDC_EDIT_TARGET_CHARACTER_NAME)->EnableWindow(m_bCheckTargetCharacterName);	
}

void CLogTabServerIntegration::OnCheckTargetCharacterUid() 
{
	// TODO: Add your control notification handler code here
	UpdateData();

	//GetDlgItem(IDC_EDIT_TARGET_CHARACTER_UID)->EnableWindow(m_bCheckTargetCharacterUID);		
}

void CLogTabServerIntegration::OnCheckTargetDbNum() 
{
	// TODO: Add your control notification handler code here
	UpdateData();

	//GetDlgItem(IDC_EDIT_TARGET_DB_NUM)->EnableWindow(m_bCheckTargetDBNum);		
}

void CLogTabServerIntegration::OnButtonReset() 
{
	// TODO: Add your control notification handler code here
	this->ResetVariables();
	this->ResetControls();
}

void CLogTabServerIntegration::OnButtonSave() 
{
	// TODO: Add your control notification handler code here
	int nRowCount = m_GridServerIntegrationLog.GetRowCount();
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
			m_GridServerIntegrationLog.GetItemText(i, 0), m_GridServerIntegrationLog.GetItemText(i, 1), m_GridServerIntegrationLog.GetItemText(i, 2),
			m_GridServerIntegrationLog.GetItemText(i, 3), m_GridServerIntegrationLog.GetItemText(i, 4), m_GridServerIntegrationLog.GetItemText(i, 5));
		resultLog.WriteSystemLog(szResult, FALSE);
	}
	MessageBox("Save success !!");
}

void CLogTabServerIntegration::ArrangeControls()
{
	bool bEnable = false;

	if (radioRecall->GetCheck())
	{
		chkExpired->EnableWindow(TRUE);
		chkSold->EnableWindow(TRUE);
		chkPaidRecall->EnableWindow(TRUE);
		chkOthers->EnableWindow(TRUE);
	}
	else
	{
		chkExpired->EnableWindow(FALSE);
		chkSold->EnableWindow(FALSE);
		chkPaidRecall->EnableWindow(FALSE);
		chkOthers->EnableWindow(FALSE);
	}

	if (!radioTraded->GetCheck())
		bEnable = false;
	else
		bEnable = true;

	//buyer stuff
	GetDlgItem(IDC_EDIT_ACCOUNTUID2)->EnableWindow(bEnable);
	GetDlgItem(IDC_EDIT_ACCOUNTUID2)->SetWindowText("0");
	GetDlgItem(IDC_CHECK_AccountUID2)->EnableWindow(bEnable);

	GetDlgItem(IDC_EDIT_ACCOUNTNAME4)->EnableWindow(bEnable);
	GetDlgItem(IDC_EDIT_ACCOUNTNAME4)->SetWindowText("0");
	GetDlgItem(IDC_CHECK_AccountName4)->EnableWindow(bEnable);
	
	GetDlgItem(IDC_CHECK_AccountName5)->EnableWindow(bEnable);	
	GetDlgItem(IDC_EDIT_ACCOUNTNAME5)->EnableWindow(bEnable);

	GetDlgItem(IDC_CHECK_AccountName6)->EnableWindow(bEnable);
	GetDlgItem(IDC_EDIT_ACCOUNTNAME6)->EnableWindow(bEnable);

	if(!bEnable)
	{
		auto* el = (CButton*)GetDlgItem(IDC_CHECK_AccountName4);
		el->SetCheck(0);

		el = (CButton*)GetDlgItem(IDC_CHECK_AccountUID2);
		el->SetCheck(0);
		
		el = (CButton*)GetDlgItem(IDC_CHECK_AccountName5);
		el->SetCheck(0);
		
		el = (CButton*)GetDlgItem(IDC_CHECK_AccountName6);
		el->SetCheck(0);
	}
}
void CLogTabServerIntegration::OnBnClickedRadio1()
{
	ArrangeControls();
}


void CLogTabServerIntegration::OnBnClickedRadio2()
{
	ArrangeControls();
}


void CLogTabServerIntegration::OnBnClickedRadio3()
{
	ArrangeControls();
}
