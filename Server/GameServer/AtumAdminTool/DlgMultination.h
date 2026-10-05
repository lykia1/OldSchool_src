#pragma once

#if _MSC_VER > 1000
#pragma once
#endif
#include "SCGridHelper.h"
#include "ODBCStatement.h"
struct WHITE_LIST_ROW_MN
{
	int		AccUID;
	char	AccountName[SIZE_MAX_ACCOUNT_NAME];
};
struct MULTI_NATION_ROW
{
	char	AccountName[SIZE_MAX_ACCOUNT_NAME];
	char	szIP[SIZE_MAX_IPADDRESS];
	char	Influence[SIZE_MAX_ACCOUNT_NAME];
	int		AccUID;
	char	isBanned[SIZE_MAX_ACCOUNT_NAME];
	char	isOnline[SIZE_MAX_ACCOUNT_NAME];
	char	isWhitelisted[SIZE_MAX_ACCOUNT_NAME];
	ATUM_DATE_TIME	LastLogin;
};
typedef vector<MULTI_NATION_ROW>		vectMULTI_NATION_ROW;

class CAtumAdminToolDlg;
class CDlgMultination : public CDialog
{
public:
	CDlgMultination(CWnd* pParent = NULL);
	virtual ~CDlgMultination();
	enum { IDD = IDD_DLG_MULTINATION};

protected:
	CSCAdminPreWinSocket * m_pUserAdminPreSocket;
	virtual void DoDataExchange(CDataExchange* pDX);
	void _InitGrid(CGridCtrl* i_pGridCtrl);
	void _AddRow(CGridCtrl* i_pGridCtrl, MULTI_NATION_ROW* i_pShopItem);
	void ViewGrid(CGridCtrl* i_pGridCtrl);
	BOOL DBQueryLoadData(vectMULTI_NATION_ROW* o_pVectItemList);

protected:
	CGridCtrl				m_Grid;
	vectMULTI_NATION_ROW		m_vectResults;
	BOOL	m_bCheckHideBanned;
	BOOL	m_bOnlyOnline;
	BOOL	m_bIsShowWhitelisted;
	CODBCStatement			m_odbcStmt2;
	CAtumAdminToolDlg* m_pMainDlg;

	virtual BOOL OnInitDialog();
	INT AddWhiteList(const char* szAccName);
	afx_msg void OnSave();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDoReload();
	afx_msg void OnBnHideBanned();
	afx_msg void OnBnOnlyOnline();
	afx_msg void OnBnClickedUpdateToDb3();
	afx_msg LONG OnSocketNotifyPre(WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnAsyncSocketMessage(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedCheck8();
	afx_msg void OnBnClickedButton1();
};
