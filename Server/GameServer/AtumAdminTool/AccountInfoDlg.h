#if !defined(AFX_ACCOUNTINFODLG_H__7AC50441_DB62_4ADF_BFB7_4048EDABEBAC__INCLUDED_)
#define AFX_ACCOUNTINFODLG_H__7AC50441_DB62_4ADF_BFB7_4048EDABEBAC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AccountInfoDlg.h : header file
//
#include "ODBCStatement.h"

/////////////////////////////////////////////////////////////////////////////
// CAccountInfoDlg dialog

class CAccountInfoDlg : public CDialog
{
// Construction
public:
	CAccountInfoDlg(BOOL i_bEnableEdit, CWnd* pParent = NULL);   // standard constructor

	enum { IDD = IDD_DIALOG_EDIT_ACCOUNT };
	CComboBox	m_ComboAccountType;
	CString	m_szAccountName;
	CString	m_szPassword;
	CString	m_szRegisterdDate;
	CString	m_szLastLoginDate;
	BOOL	m_bAccountBlocked;
	BOOL	m_bChattingBlocked;
	CString	m_ctlcsSecPassword;
	USHORT			m_nAcountType;
	BOOL			m_bEnableEdit;
//07-05-2023 by Inet
	CString	m_szEmail;
	BOOL	m_bActive;
	INT		m_nCashPoint;
	INT		m_nTotalDonatedEur;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);

public:
	CODBCStatement *m_pODBCStmt;
	virtual BOOL OnInitDialog();
	afx_msg void OnOk();
	virtual void OnCancel();
	afx_msg void OnButtonMd5Pwd();
	afx_msg void OnButtonMd5Secpwd();
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_ACCOUNTINFODLG_H__7AC50441_DB62_4ADF_BFB7_4048EDABEBAC__INCLUDED_)
