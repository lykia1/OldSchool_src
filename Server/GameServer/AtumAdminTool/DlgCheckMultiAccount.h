#pragma once

#if _MSC_VER > 1000
#pragma once
#endif 

#include "SCGridHelper.h"
#include "ODBCStatement.h"

struct MULTI_ACC_ROW
{
	int		AccUID;
	char	AccountName[SIZE_MAX_ACCOUNT_NAME];
	char	szIP[SIZE_MAX_IPADDRESS];
	ATUM_DATE_TIME	LastLogin;
	char	isWihteList[SIZE_MAX_ACCOUNT_NAME];
};
typedef vector<MULTI_ACC_ROW>		vectMULTI_ACC_ROW;
struct WHITE_LIST_ROW
{
	int		AccUID;
	char	AccountName[SIZE_MAX_ACCOUNT_NAME];
};
class CAtumAdminToolDlg;
class CDlgMultiaccount : public CDialog
{
public:
	CDlgMultiaccount(CWnd* pParent = NULL);
	virtual ~CDlgMultiaccount();
	enum { IDD = IDD_DLG_MULTIACCOUNT
	};
protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	void _InitGrid(CGridCtrl* i_pGridCtrl);
	void _AddRow(CGridCtrl* i_pGridCtrl, MULTI_ACC_ROW* i_pShopItem);
	void ViewGrid(CGridCtrl* i_pGridCtrl);
	BOOL DBQueryLoadData(vectMULTI_ACC_ROW* o_pVectItemList);
	INT nCheckOverCount;
	CButton m_Btn1;
	CButton m_Btn2;
	CButton m_Btn3;
	INT AddWhiteList(const char* szAccName);
protected:
	CGridCtrl				m_Grid;
	vectMULTI_ACC_ROW		m_vectResults;
	BOOL	m_bCheckShowWhitelist;
	CODBCStatement			m_odbcStmt2;
	CAtumAdminToolDlg* m_pMainDlg;

	virtual BOOL OnInitDialog();
	afx_msg void OnBtnUpdateToDb();
	DECLARE_MESSAGE_MAP()
	void OnRadioBtn(UINT radioID);
public:
	afx_msg void OnReaload();
	afx_msg void OnCheckboxWhitelist();
};