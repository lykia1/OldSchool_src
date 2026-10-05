#pragma once

#define _INI_FILE_NAME ".\\EvoConfig.ini"
#define _GRAPH_SEC	   "Graphics"
#define _GRAPH_MISC	   "Misc"
class InetConfiguratorDLG : public CDialog
{
public:
	InetConfiguratorDLG(CWnd* pParent = NULL);   // standard constructor
	virtual ~InetConfiguratorDLG();

// Dialog Data
	enum { IDD = IDD_INET_CONFIG };
protected:
	CBitmapButton		m_bmpbtnX;
	CBitmapButton		m_bmpbtnSave;
	CBitmap t1;
	CBitmap tmBitmap;
	CBitmap *pTmOldBitmap;
	CDC tmMemDC;
	CBitmap	*pOldBitmapBackGround;
	CDC		memDCBackGround;
	CDC *pDC;
	CBitmap				m_BackGround;
	CBrush				m_StaticBrushBlack;
	CButton	m_ctlEnableVsync;
	//radios
	CButton	m_radMinimum;
	CButton	m_radMedium;
	CButton	m_radMax;
	CEdit	m_EditFPS;
	//CMyCheck	m_ctlEnableVsync;
	INT		m_nAntiAliasLv;
	INT		m_nAntiAliasQuality;
	INT		m_nTexFilterType;
	INT		m_nAnisothropicLv;
	CComboBox *pAALv;
	
	CComboBox *pAAQty;
	CComboBox *pFtype;
	CComboBox *pAnisoLv;
	
	CButton m_ctlDisableParallel;
	int m_FPSLimit;
	BOOL BitmapRgn(UINT resource, COLORREF TansColor, int nx, int ny);
	BOOL BitmapRgn(LPCTSTR resource, COLORREF TansColor, int nx, int ny);
	HRGN BitmapToRegion(HBITMAP hBmp, COLORREF cTransparentColor/* = 0*/, COLORREF cTolerance/* = 0x101010*/, int nx, int ny);
	CButton	m_ctlRememberPass;
	//CMyCheck m_ctlRememberPass;
protected:
	afx_msg void OnRadioBtn(UINT radioID);
	void ReadINI();
	void RandomMoveWindow(void);
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnClosePopup();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void WriteINI();
	afx_msg void OnPaint();
	afx_msg void OnNCPaint();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	DECLARE_MESSAGE_MAP()
};
