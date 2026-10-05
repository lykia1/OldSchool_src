#if !defined(AFX_LOGTABSERVERINTEGRATION_H__C09E8001_E80E_4E70_9E06_2C346E0583AF__INCLUDED_)
#define AFX_LOGTABSERVERINTEGRATION_H__C09E8001_E80E_4E70_9E06_2C346E0583AF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LogTabServerIntegration.h : header file
//
#include "SCGridHelper.h"
#include "ODBCStatement.h"

class CSCLogAdminDlg;



struct MARKET_BUY_LOG_FOR_AT
{
	BYTE byLogType;
	ATUM_DATE_TIME	Time;
	char szCharacterName[SIZE_MAX_CHARACTER_NAME];
	char szAccountName[SIZE_MAX_ACCOUNT_NAME];
	UID32_t CharacterUID;
	UID32_t AccountUID;
	INT ChargePrice;
	INT ItemNum;
	char szItemName[140];
	INT ItemCount;
	INT Price;
	char szMoneyType[SIZE_MAX_ACCOUNT_NAME];
	UID64_t	MarketUID;
	UID64_t	MarketItemUID;
	char szBuyerCharName[SIZE_MAX_CHARACTER_NAME];
	char szBuyerAccountName[SIZE_MAX_ACCOUNT_NAME];
	UID32_t BuyerCharacterUID;
	UID32_t BuyerAccountUID;
	BYTE byMarketStatus;
};

typedef vector<MARKET_BUY_LOG_FOR_AT>		vectMARKET_BUY_LOG_FOR_AT;
/////////////////////////////////////////////////////////////////////////////
// CLogTabServerIntegration dialog

class CLogTabServerIntegration : public CDialog
{
// Construction
public:
	CLogTabServerIntegration(CDialog *i_pMainDlg, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CLogTabServerIntegration)
	enum { IDD = IDD_LOG_TAB_SERVERINTEGRATION };
	CString	m_szAccountName;
	BOOL	m_bCheckAccountName;
	CString	m_szBuyerAccName;
	BOOL	m_bBuyerCheckAccName;
	CTime	m_dateEnd;
	CTime	m_timeEnd;
	CTime	m_dateStart;
	CTime	m_timeStart;
	BOOL	m_bDate;
	BOOL	m_bCheckAccountUID;
	int		m_nAccountUID;	
	
	BOOL	m_bBuyerCheckAccountUID;
	int		m_nBuyerAccountUID;

	BOOL	m_bCheckcharUID;
	int		m_nCharUID;

	BOOL	m_bCheckCharName;
	CString	m_szCharName;

	BOOL	m_bCheckItemUID;
	UID64_t		m_nItemUID;


	BOOL	m_bBuyerCheckcharUID;
	int		m_nBuyerCharUID;

	BOOL	m_bBuyerCheckCharName;
	CString	m_szBuyerCharName;

	int m_nTotalLogCounts;
	//}}AFX_DATA
	CButton* radioRegister;
	CButton* radioRecall;
	CButton* radioTraded;

	CButton* chkExpired;
	CButton* chkSold;
	CButton* chkPaidRecall;
	CButton* chkOthers;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLogTabServerIntegration)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	CSCGridCtrl		m_GridServerIntegrationLog;
	CSCLogAdminDlg	*m_pMainDlg;
	CODBCStatement	*m_pODBCStmt;
	void ArrangeControls();
	vectMARKET_BUY_LOG_FOR_AT m_vectMARKET_BUY_LOG_FOR_AT;

	void	InitGrid();
	void	ResetVariables();
	void	ResetControls();

	BOOL	MakeSzQueryForComplete();					// 2007-01-30 by dhjin, 쿼리완성을 위한 함수 

protected:
	CString		SzQuery;

	// Generated message map functions
	//{{AFX_MSG(CLogTabServerIntegration)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonSearch();
	afx_msg void OnCHECKAccountName();
	afx_msg void OnCheckDate();
	afx_msg void OnCHECKAccountUID();
	afx_msg void OnCheckSourceCharacterName();
	afx_msg void OnCheckSourceCharacterUid();
	afx_msg void OnCheckSourceDbNum();
	afx_msg void OnCheckTargetCharacterName();
	afx_msg void OnCheckTargetCharacterUid();
	afx_msg void OnCheckTargetDbNum();
	afx_msg void OnButtonReset();
	afx_msg void OnButtonSave();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();
	afx_msg void OnBnClickedRadio3();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOGTABSERVERINTEGRATION_H__C09E8001_E80E_4E70_9E06_2C346E0583AF__INCLUDED_)
