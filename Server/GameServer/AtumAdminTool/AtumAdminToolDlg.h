// AtumAdminToolDlg.h : header file
//

#if !defined(AFX_ATUMADMINTOOLDLG_H__FEBE4557_6076_4041_AB75_004F385A2854__INCLUDED_)
#define AFX_ATUMADMINTOOLDLG_H__FEBE4557_6076_4041_AB75_004F385A2854__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SCUserAdminDlg.h"
#include "SCServerAdminDlg.h"
#include "SCBadUserAdminDlg.h"
#include "SCLogAdminDlg.h"
#include "TrayIcon.h"
#include "Localization.h"
#include "SCHappyHourEventAdminDlg.h"
#include "SCItemEventDlg.h"
#include "SCStrategyPointAdminDlg.h"
#include "SCOutPostDlg.h"

#include "RenewalStrategyPointAdminDlg.h"
#include "AdminAutoNoticeDlg.h"			// 2009-01-14 by cmkwon, 운영자 자동 공지 시스템 구현 - 

#define WM_ICON_NOTIFY		WM_USER+20

/////////////////////////////////////////////////////////////////////////////
// CAtumAdminToolDlg dialog

struct PERMISSIONS_CHECK
{
	BOOL		RequirePasswordsAllTime;
	BOOL		OpenUserManager;
	BOOL		OpenGuildManager;
	BOOL		OpenLogManager;
	BOOL		OpenHHManager;
	BOOL		OpenItemEvent;
	BOOL		OpenPetitionManager;
	BOOL		OpenSPManager;
	BOOL		OpenOutpostManager;
	BOOL		OpenMonsterEvent;
	BOOL		OpenServerManager;
	BOOL		OpenAutoNotice;
	BOOL		OpenItemShop;
	BOOL		OpenRankingManager;
	BOOL		OpenIFinitialize;
	BOOL		OpenSSViever;
	BOOL		OpenEXPViever;
	BOOL		OpenXOREncode;
	BOOL		OpenXORDecode;
	BOOL		OpenMultinationCheck;
	BOOL		UM_ViewInventory;
	BOOL		UM_BanAccount;
	BOOL		UM_UnbanAccount;
	BOOL		UM_DeletedCharacter;
	BOOL		UM_EditBlockInfo;
	BOOL		UM_EditPremium;
	BOOL		UM_EditCharacter;
	BOOL		UM_EditAccount;
	BOOL		UM_BlockedAccounts;
	BOOL		UM_InflWarList;
	BOOL		UM_InitializeInfluence;
	BOOL		UM_SetInfluenceRate;
	BOOL		UM_InfinityInfoInit;
	BOOL		OpenMultiaccountCheck;

	BOOL		CreateLogBackup;
};
class CExpViewerDlg;
class CSCGuildAdminDlg;
class CXORTestDlg;			// 2007-10-24 by cmkwon, 서버 정보 암호화 - 
class CPetitionManagementDlg;		// 2007-11-19 by cmkwon, 진정시스템 업데이트 - 
class CEventMonsterManagementDlg;	// 2008-04-16 by cmkwon, 몬스터 사망 시 몬스터 소환 이벤트 시스템 구현 - 
class CCashShopManagementDlg;		// 2009-01-28 by cmkwon, 캐쉬샾 수정(추천탭,신상품 추가) - 
class CWRankingManagement;			// 2009-02-12 by cmkwon, EP3-3 월드랭킹시스템 구현 -
class CDlgMultination;
class CDlgMultiaccount;
class CBalanceChartDlg;
class CAtumAdminToolDlg : public CDialog
{
// Construction
public:
	CAtumAdminToolDlg(CWnd* pParent = NULL);	// standard constructor
	virtual ~CAtumAdminToolDlg();

//	CTrayIcon		m_TrayIcon;	

// Dialog Data
	//{{AFX_DATA(CAtumAdminToolDlg)
	enum { IDD = IDD_DIALOG_ATUM_ADMIN_TOOL };
	CComboBox	m_ComboSelectServer;
	CString	m_UID;
	CString	m_PWD;
	CString	m_ctl_strLanguageString;
	//}}AFX_DATA
	PERMISSIONS_CHECK permCheck;
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAtumAdminToolDlg)
	public:
	virtual BOOL DestroyWindow();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL ConnectServer();

	void EnableToolControls(BOOL i_bEnable);

	void EnableToolControls ( USHORT usAccountType );		// 계정 타입별 메뉴 권한 부여.

	void SetLanguageString(int i_nLanguageType);

	BOOL IsManagerAdministrator(void);

	// 2007-11-19 by cmkwon, 진정시스템 업데이트 - 
	BOOL SendMsgToPreServer(BYTE *i_pbyData, int i_nDataLen);
	BOOL SendMsgTypeToPreServer(MessageType_t i_nMsgTy);

public :	// Inline Process.

	inline USHORT GetManagerAccountType ( void ) { return m_usManagerAccountType; };

public:
	// dialogs
	CSCUserAdminDlg				*m_pUserAdminDlg;
	CSCServerAdminDlg			*m_pServerAdminDlg;
	CSCBadUserAdminDlg			*m_pBadUserAdminDlg;
	CSCLogAdminDlg				*m_pLogAdminDlg;
	CSCHappyHourEventAdminDlg	*m_pHappyHourEventAdminDlg;
	CExpViewerDlg				*m_pExpViewer;					// 2005-08-08 by cmkwon
	CSCGuildAdminDlg			*m_pGuildAdminDlg;				// 2006-03-07 by cmkwon
	CSCItemEventDlg				*m_pItemEventDlg;				// 2006-08-29 by dhjin
	CRenewalStrategyPointAdminDlg	*m_pRenewalStrategyPointDlg;
	CSCStrategyPointAdminDlg	*m_pStrategyPointDlg;			// 2007-03-05 by dhjin
	CXORTestDlg					*m_pXOREncodeDlg;				// 2007-10-24 by cmkwon, 서버 정보 암호화
	CXORTestDlg					*m_pXORDecodeDlg; //inetpub
	CPetitionManagementDlg		*m_pPetitionDlg;				// 2007-11-19 by cmkwon, 진정시스템 업데이트 - 
	CSCOutPostDlg				*m_pOutPostDlg;					// 2008-01-28 by dhjin, OutPst
	CEventMonsterManagementDlg	*m_pEventMonsterDlg;			// 2008-04-16 by cmkwon, 몬스터 사망 시 몬스터 소환 이벤트 시스템 구현 - 
	CDlgMultination *	m_pDlgMultination;
	CDlgMultiaccount*	m_pDlgMultiacc;
	CBalanceChartDlg*	m_pDlgBalance;
	
	
	// 2009-01-14 by cmkwon, 운영자 자동 공지 시스템 구현 - Admin Auto Notice Management
	CAdminAutoNoticeDlg*		m_pAdminAutoNoticeDlg;

	// 2009-01-28 by cmkwon, 캐쉬샾 수정(추천탭,신상품 추가) - 
	CCashShopManagementDlg *	m_pCashShopManagementDlg;	

	// 2009-02-12 by cmkwon, EP3-3 월드랭킹시스템 구현 - 
	CWRankingManagement *		m_pWRankingManagementDlg;	
	BOOL IsConnectedToPreServer(void);

	// data
	GAME_SERVER_INFO_FOR_ADMIN	*m_pServerInfo4Admin;
	CSCAdminPreWinSocket		*m_pAdminPreSocket;

	CString						m_szServerName;
	
	// 현지화 관련
	CLocalization				m_Localization;
	CString						m_strLocalizationDirectoryPath;
	INT							m_nLanguageType;

	// 2006-04-15 by cmkwon
	char						m_szManagerAccountName[SIZE_MAX_ACCOUNT_NAME];	// 2006-04-15 by cmkwon, SCAdminTool 접속자 계정
	USHORT						m_usManagerAccountType;							// 2006-04-15 by cmkwon, 접속자 계정 권한
	INT n_ATVersion;
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CAtumAdminToolDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButtonUserTool();
	afx_msg void OnButtonConnect();
	afx_msg void OnButtonDisconnect();
	afx_msg void OnButtonServerTool();
	afx_msg void OnButtonLogTool();
	afx_msg void OnButtonBadUserTool();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnButtonScreenShotViewer();
	afx_msg void OnMenuTrayExit();
	afx_msg void OnMenuTrayOpen();
	virtual void OnCancel();
	afx_msg void OnDestroy();
	afx_msg void OnBtnLocalization();
	afx_msg void OnButtonHappyHourEventTool();
	afx_msg void OnBtnExpViewer();
	afx_msg void OnBtnGuildTool();
	afx_msg void OnButtonItemEvent();
	afx_msg void OnButtonStrategypoint();
	afx_msg void OnBtnXorEncode();
	afx_msg void OnBtnXorDecode();
	afx_msg void OnBtnPetitionSytem();
	afx_msg void OnButtonOutpost();
	afx_msg void OnButtonEventMonster();
	afx_msg void OnButtonAutoNotice();
	afx_msg void OnBtnCashshopManagement();
	afx_msg void OnBtnWrankingManagement();
	afx_msg void OnBtnMultination();
	afx_msg void OnBtnInfinity();
	//}}AFX_MSG
	afx_msg LONG OnSocketNotifyPre(WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnAsyncSocketMessage(WPARAM wParam, LPARAM lParam);
//	afx_msg LRESULT OnTrayNotification(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnMultinationdlg2();
	afx_msg void OnBnClickedButton1();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ATUMADMINTOOLDLG_H__FEBE4557_6076_4041_AB75_004F385A2854__INCLUDED_)
