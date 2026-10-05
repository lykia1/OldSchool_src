// helloworldDlg.h : header file
//

#pragma once

#include "ChartViewer.h"
#include "afxbutton.h"
#define SIZE_DATE 30
#define INFLUENCE_TYPE_COUNT 4
#define INFLUENCE_TYPE_UNKNOWN				(BYTE)0x0000	// 알수 없음
#define INFLUENCE_TYPE_NORMAL				(BYTE)0x0001	// 2005-12-20 by cmkwon, 바이제니유 일반군
#define INFLUENCE_TYPE_VCN					(BYTE)0x0002	// 2005-12-20 by cmkwon, 바이제니유 정규군, 이전(V.C.U: Vijuenill City United.)
#define INFLUENCE_TYPE_ANI					(BYTE)0x0004	// 2005-12-20 by cmkwon, 알링턴 정규군, 이전(반 민족주의 연합 -알링턴 시티 반란군- (A.N.I: Anti Nationalism Influence))
#define INFLUENCE_TYPE_ALL_MASK				(BYTE)0x00FF
#define SQL_DATETIME_STRING_FORMAT					"%04d%02d%02d %02d:%02d:%02d.000"
#include "SCGridHelper.h"
#include "ODBCStatement.h"
struct SDATE_USER_COUNT
{
	char	Date[SIZE_DATE];
	int		arrUserCnts[INFLUENCE_TYPE_COUNT];
};
struct SDATE_EXP
{
	char	Date[SIZE_DATE];
	long long		EXP;
	float		fChangeVal;
};
struct SDATE_MARKET_TRANSFERS_COUNTS
{
	int nType;
	int		nXfersCount;
	char	Date[SIZE_DATE];
};
struct SDATE_MARKET_COMPUTED_3_VEC
{
	int		nXfersCountRegist;
	int		nXfersCountRecall;
	int		nXfersCountTrade;
	char	Date[SIZE_DATE];
};
struct SDATE_MARKET_COUNTS
{
	int		nCount;
	char	Date[SIZE_DATE];
};
struct SDATE_MARKET_TAX
{
	int		nTransfersCount;
	int		nTax;
	long long		nTotal;
	char	Date[SIZE_DATE];
};
typedef vector<SDATE_USER_COUNT>			vectSDATE_USER_COUNT;
typedef vector<SDATE_EXP>			vectSDATE_EXP;

typedef vector<SDATE_MARKET_COUNTS>			vectSDATE_MARKET_COUNTS;
typedef vector<SDATE_MARKET_TAX>			vectSDATE_MARKET_TAX;

typedef vector<SDATE_MARKET_TRANSFERS_COUNTS>			vectSDATE_MARKET_TRANSFERS_COUNTS;
typedef vector<SDATE_MARKET_COMPUTED_3_VEC>			vectSDATE_MARKET_COMPUTED_3_VEC;


struct find_if_SDATE_USER_COUNT_BY_Date
{
	find_if_SDATE_USER_COUNT_BY_Date(char* i_szDate)
	{
		strncpy(m_szDate, i_szDate, SIZE_DATE);
	};
	bool operator()(const SDATE_USER_COUNT dateUserCnt)
	{
		if (0 != strncmp(dateUserCnt.Date, m_szDate, SIZE_DATE))
		{
			return false;
		}

		return true;
	}
	char m_szDate[SIZE_DATE];
};

// CHelloworldDlg dialog
class CBalanceChartDlg : public CDialog
{
// Construction
public:
	CBalanceChartDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_BALANCECHART
	};
	CChartViewer	m_ChartViewer;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	BOOL DBConn();
	BOOL GetExpChart();
	BOOL GetMarketChart();


	BOOL DrawGraph();
	BOOL DrawGraph2();
	BOOL DrawMarketCntGraph();
	CODBCStatement			m_odbcStmt2;
	CAtumAdminToolDlg* m_pMainDlg;
	vectSDATE_USER_COUNT	vectDateUserCntList;
	vectSDATE_EXP	vectDateExpList;

	vectSDATE_MARKET_COUNTS	vectMarketCountsXfers;
	vectSDATE_MARKET_TAX	vectTaxMoney;

	vectSDATE_MARKET_TRANSFERS_COUNTS	vectTransfersCounts;
	vectSDATE_MARKET_COMPUTED_3_VEC	vectComputedTransfers;
// Implementation
protected:
	HICON m_hIcon;
	CBrush mMainBrush;
	CMFCButton  m_RldBtn;
	CMFCButton  m_LoadExpBtn;
	CMFCButton  m_LoadMarketBtn;
	CTime m_FromDate;
	CTime m_FromTime;
	CTime m_ToDate;
	CTime m_ToTime;

	CButton* chkRegistered;
	CButton* chkRecalled;
	CButton* chkTraded;

	CButton* chkTaxSPI;
	CButton* chkTaxWP;
	CButton* chkTaxCR;

	CButton* chkMoneySPI;
	CButton* chkMoneyWP;
	CButton* chkMoneyCR;

	CButton* chkTransactions;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnDtnDatetimechangeDatetimepicker1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedButtonChartRld2();
	afx_msg void OnBnClickedButtonChartRld3();
};
