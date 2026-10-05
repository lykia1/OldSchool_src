// InetConfiguratorDLG.cpp : implementation file
//

#include "stdafx.h"
#include "AtumLauncher.h"
#include "InetConfiguratorDLG.h"

struct SINET_CFG
{
	char	*szOptName;
	int		Value;
	int		nOptKind; // 0 - anti-aliasing level /  1 - anti-aliasing quality / 2 - texture filter / 3 - anisotrophic level
	int		nIdxInStr;
};
SINET_CFG g_pInetOptionList[] =
{
	{ "MSAA  off", 0, 0, 0 },
	{ "MSAA  x1", 1, 0, 1 },
	{ "MSAA  x2", 2, 0, 2 },
	{ "MSAA  x4", 4, 0, 3 },
	{ "MSAA  x8", 8, 0, 4 },
	{ "MSAA x16", 16, 0, 5 },
	{ "0", 0, 1, 0 },
	{ "1", 1, 1, 1 },
	{ "2", 2, 1, 2 },
	{ "3", 3, 1, 3 },
	{ "4", 4, 1, 4 },
	{ "5", 5, 1, 5 },
	{ "6", 6, 1, 6 },
	{ "7", 7, 1, 7 },
	{ "None", 0, 2, 0 },
	{ "Point", 1, 2, 1 },
	{ "Bilinear", 2, 2, 2 },
	{ "Trilinear", 3, 2, 3 },
	{ "Anisotropic", 4, 2 ,4 },
	{ "1", 1, 3, 0 },
	{ "2", 2, 3, 1 },
	{ "3", 3, 3, 2 },
	{ "4", 4, 3, 3 },
	{ "5", 5, 3, 4 },
	{ "6", 6, 3, 5 },
	{ "7", 7, 3, 6 },
	{ "8", 8, 3, 7 },
	{ "9", 9, 3, 8 },
	{ "10", 10, 3, 9 },
	{ "11", 11, 3, 10 },
	{ "12", 12, 3, 11 },
	{ "13", 13, 3, 12 },
	{ "14", 14, 3, 13 },
	{ "15", 15, 3, 14 },
	{ "16", 16, 3, 15 },
};
InetConfiguratorDLG::InetConfiguratorDLG(CWnd* pParent /*=NULL*/)
	: CDialog(InetConfiguratorDLG::IDD, pParent)
{
 //nothing to do here
}
int find(SINET_CFG arr[], unsigned int from, unsigned int optIDX, UINT seek)
{
	for (int i = from; arr[i].nIdxInStr <= optIDX; i++)
	{
		if (arr[i].Value == seek) return arr[i].nIdxInStr;
	}
	return -1;
}
void InetConfiguratorDLG::ReadINI()
{
	//select combo etc like its setted up in INI file
	m_ctlEnableVsync.SetCheck(GetPrivateProfileInt(_GRAPH_SEC, "VSync", 0, _INI_FILE_NAME));
	m_ctlRememberPass.SetCheck(GetPrivateProfileInt(_GRAPH_MISC, "RememberPass", 1, _INI_FILE_NAME));

	int AliasLvSel = find(g_pInetOptionList, 0, 5, GetPrivateProfileInt(_GRAPH_SEC, "AntiAliasLv", 1, _INI_FILE_NAME));
	pAALv->SetCurSel((AliasLvSel == -1) ? 0 : AliasLvSel);

	int AliasQtySel = find(g_pInetOptionList, 6, 7, GetPrivateProfileInt(_GRAPH_SEC, "AntiAliasQuality", 1, _INI_FILE_NAME));
	pAAQty->SetCurSel((AliasQtySel == -1) ? 0 : AliasQtySel);

	int FTypeSel = find(g_pInetOptionList, 14, 4, GetPrivateProfileInt(_GRAPH_SEC, "TexFilterType", 1, _INI_FILE_NAME));
	pFtype->SetCurSel((FTypeSel == -1) ? 0 : FTypeSel);

	int AnisoSel = find(g_pInetOptionList, 19, 15, GetPrivateProfileInt(_GRAPH_SEC, "AnisothropicLv", 1, _INI_FILE_NAME));
	pAnisoLv->SetCurSel((AnisoSel == -1) ? 0 : AnisoSel);

	m_ctlDisableParallel.SetCheck(GetPrivateProfileInt(_GRAPH_SEC, "DisableParallel", 1, _INI_FILE_NAME));
	m_FPSLimit = GetPrivateProfileInt(_GRAPH_SEC, "LimitFPS", 0, _INI_FILE_NAME);
	char szTm[10];
	sprintf(szTm,"%d",m_FPSLimit);
	m_EditFPS.SetWindowText(szTm);
}

void InetConfiguratorDLG::WriteINI()
{

	char AALvI[5], AAQty[5], TexFT[5], AniLV[5], VSync1[5], RPas1[5], DPar[5],FPSLi[5];
	//chack if setting are valid if not correct them by one void
	//CheckIfSettingsAreValid(g_pInetOptionList[pAALv->GetCurSel()].Value, g_pInetOptionList[pAAQty->GetCurSel() + 4].Value);
	//end of check
	sprintf(AALvI, "%d", g_pInetOptionList[pAALv->GetCurSel()].Value);
	sprintf(AAQty, "%d", g_pInetOptionList[pAAQty->GetCurSel() + 6].Value);
	sprintf(TexFT, "%d", g_pInetOptionList[pFtype->GetCurSel() + 14].Value);
	sprintf(AniLV, "%d", g_pInetOptionList[pAnisoLv->GetCurSel() + 19].Value);
	sprintf(VSync1, "%d", m_ctlEnableVsync.GetCheck());
	sprintf(RPas1, "%d", m_ctlRememberPass.GetCheck());
	sprintf(DPar, "%d", m_ctlDisableParallel.GetCheck());
	CString tmTextFPSLim;
	m_EditFPS.GetWindowText(tmTextFPSLim);
	sprintf(FPSLi, "%s", (LPCTSTR)tmTextFPSLim);
	WritePrivateProfileStringW(NULL, NULL, NULL, (LPCWSTR)_INI_FILE_NAME);
	WritePrivateProfileString(_GRAPH_SEC, "VSync", VSync1 != NULL ? VSync1 : "0", _INI_FILE_NAME);
	WritePrivateProfileString(_GRAPH_SEC, "AntiAliasLv", AALvI != NULL ? AALvI : "0", _INI_FILE_NAME);
	WritePrivateProfileString(_GRAPH_SEC, "AntiAliasQuality", AAQty != NULL ? AAQty : "0", _INI_FILE_NAME);
	WritePrivateProfileString(_GRAPH_SEC, "TexFilterType", TexFT != NULL ? TexFT : "0", _INI_FILE_NAME);
	WritePrivateProfileString(_GRAPH_SEC, "AnisothropicLv", AniLV != NULL ? AniLV : "0", _INI_FILE_NAME);
	WritePrivateProfileString(_GRAPH_MISC, "RememberPass", RPas1 != NULL ? RPas1 : "0", _INI_FILE_NAME);
	WritePrivateProfileString(_GRAPH_SEC, "DisableParallel", DPar != NULL ? DPar : "0", _INI_FILE_NAME);
	WritePrivateProfileString(_GRAPH_SEC, "LimitFPS", FPSLi != NULL ? FPSLi : "0", _INI_FILE_NAME);

	WritePrivateProfileString(_GRAPH_SEC, "UseTrailMod", m_radMinimum.GetCheck()? "1":"0", _INI_FILE_NAME); //use trail mod

	MessageBox("Settings successfully saved!", "Info", MB_OK | MB_ICONINFORMATION);
	CDialog::EndDialog(0);
}
void InetConfiguratorDLG::RandomMoveWindow(void)
{
	int nCXScreen = GetSystemMetrics(SM_CXSCREEN);
	int nCYScreen = GetSystemMetrics(SM_CYSCREEN);

	RECT rt;
	GetWindowRect(&rt);
	int nWidth = rt.right - rt.left;
	int nHeight = rt.bottom - rt.top;

	RECT rtParent;
	GetParent()->GetWindowRect(&rtParent);

	int nLeftMargin = 150;
	int nRightMargin = 0;
	int nDownMargin = 140;
	if (nCXScreen > nWidth)
	{
		int	nRandRange = max(1, (rtParent.right - rtParent.left - (nLeftMargin + nRightMargin)) - nWidth);
		rt.left = rtParent.left + nLeftMargin;
		rt.right = rt.left + nWidth;
	}

	if (nCYScreen > nHeight)
	{
		rt.top = max(0, rtParent.bottom - nHeight - nDownMargin);
		rt.bottom = rt.top + nHeight;
	}

	MoveWindow(&rt);
}

InetConfiguratorDLG::~InetConfiguratorDLG()
{
}

void InetConfiguratorDLG::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX); 
	DDX_Control(pDX, IDC_CHECK_FPS, m_ctlEnableVsync);
	DDX_Control(pDX, IDC_CHECK_PASS, m_ctlRememberPass);
	DDX_Control(pDX, IDC_DIS_PARAL, m_ctlDisableParallel);
	DDX_CBIndex(pDX, IDC_AA_LV, m_nAntiAliasLv);
	DDX_CBIndex(pDX, IDC_AA_QTY, m_nAntiAliasQuality);
	DDX_CBIndex(pDX, IDC_TEX_FLT_TYPE, m_nTexFilterType);
	DDX_CBIndex(pDX, IDC_ANISO_LV, m_nAnisothropicLv);
	DDX_Control(pDX, IDC_RADIO1, m_radMinimum);
	DDX_Control(pDX, IDC_RADIO2, m_radMedium);
	DDX_Control(pDX, IDC_RADIO3, m_radMax);
	DDX_Control(pDX, IDC_EDIT1, m_EditFPS);
}
void InetConfiguratorDLG::OnRadioBtn(UINT radioID)
{
	if (m_radMinimum.GetCheck())
	{
		m_ctlEnableVsync.SetCheck(1);
		m_ctlDisableParallel.SetCheck(0);
		pAALv->SetCurSel(0);
		pAAQty->SetCurSel(0);
		pFtype->SetCurSel(4);
		pAnisoLv->SetCurSel(0);
	}
	if (m_radMedium.GetCheck())
	{
		m_ctlEnableVsync.SetCheck(1);
		m_ctlDisableParallel.SetCheck(1);
		pAALv->SetCurSel(3);
		pAAQty->SetCurSel(0);
		pFtype->SetCurSel(4);
		pAnisoLv->SetCurSel(5);
	}
	if (m_radMax.GetCheck())
	{
		m_ctlEnableVsync.SetCheck(0);
		m_ctlDisableParallel.SetCheck(1);
		pAALv->SetCurSel(5);
		pAAQty->SetCurSel(8);
		pFtype->SetCurSel(4);
		pAnisoLv->SetCurSel(15);
	}
}
BOOL InetConfiguratorDLG::OnInitDialog()
{
	CDialog::OnInitDialog();
	pDC = GetDC();
	int nXScreen = 500;
	int nYScreen = 300;

	/*m_ctlRememberPass.SetBitMap(IDB_CB_UNCHECK, IDB_CB_CHECK);
	m_ctlRememberPass.MoveWindow(130, 32, 13, 13);

	m_ctlEnableVsync.SetBitMap(IDB_CB_UNCHECK, IDB_CB_CHECK);
	m_ctlEnableVsync.MoveWindow(21, 32, 13, 13);*/

	m_BackGround.CreateCompatibleBitmap(pDC, nXScreen, nYScreen);


	memDCBackGround.CreateCompatibleDC(pDC);
	pOldBitmapBackGround = memDCBackGround.SelectObject(&m_BackGround);



	BitmapRgn(IDB_CFG_BK, RGB(255, 255, 255), 0, 0); //transparency mask


	tmBitmap.LoadBitmap(IDB_CFG_BK);
	tmMemDC.CreateCompatibleDC(pDC);
	pTmOldBitmap = tmMemDC.SelectObject(&tmBitmap);
	int	nPosX = 0;
	int nPosY = 0;
	memDCBackGround.BitBlt(nPosX, nPosY, nXScreen, nYScreen, &tmMemDC, 0, 0, SRCCOPY);
	tmMemDC.SelectObject(pTmOldBitmap);
	tmBitmap.DeleteObject();
	//CustomProgressUpdate(aaa, 0);
	memDCBackGround.SelectObject(pOldBitmapBackGround);


	pAALv = (CComboBox*)GetDlgItem(IDC_AA_LV);
	pAAQty = (CComboBox*)GetDlgItem(IDC_AA_QTY);
	pFtype = (CComboBox*)GetDlgItem(IDC_TEX_FLT_TYPE);
	pAnisoLv = (CComboBox*)GetDlgItem(IDC_ANISO_LV);
	pAALv->ResetContent();
	pAAQty->ResetContent();
	pFtype->ResetContent();
	pAnisoLv->ResetContent();
	

	for (int i = 0; g_pInetOptionList[i].nOptKind == 0; i++)
	{
		pAALv->AddString(g_pInetOptionList[i].szOptName);
	}
	for (int i = 6; g_pInetOptionList[i].nOptKind == 1; i++)
	{
		pAAQty->AddString(g_pInetOptionList[i].szOptName);
	}
	for (int i = 14; g_pInetOptionList[i].nOptKind == 2; i++)
	{
		pFtype->AddString(g_pInetOptionList[i].szOptName);
	}
	for (int i = 19; g_pInetOptionList[i].nOptKind == 3; i++)
	{
		pAnisoLv->AddString(g_pInetOptionList[i].szOptName);
	}

	ReadINI();
	//for button image
	m_bmpbtnX.AutoLoad(IDCANCEL_POPUP, this);
	m_bmpbtnX.LoadBitmaps(IDCANCEL_POPUP, RGB(0, 0, 0));
	//end of close image
	RandomMoveWindow();
	return TRUE;
}
void InetConfiguratorDLG::OnClosePopup()
{
	CDialog::EndDialog(0);
}
BEGIN_MESSAGE_MAP(InetConfiguratorDLG, CDialog)
	ON_BN_CLICKED(IDCANCEL_POPUP, OnClosePopup)
	ON_WM_PAINT()
	ON_WM_SHOWWINDOW()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(ID_SAVE, WriteINI)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_RADIO1, IDC_RADIO3, OnRadioBtn)
END_MESSAGE_MAP()
HBRUSH InetConfiguratorDLG::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO: Change any attributes of the DC here
	switch (nCtlColor)
	{
#define _INET_LBL_RE_COLOR RGB(212,244,255)
	case CTLCOLOR_STATIC:
	{
		if (GetDlgItem(IDC_STATIC1)->m_hWnd == pWnd->m_hWnd)
		{
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(_INET_LBL_RE_COLOR);
			return (HBRUSH)GetStockObject(NULL_BRUSH);
		}
		else if (GetDlgItem(IDC_CHECK_FPS)->m_hWnd == pWnd->m_hWnd)
		{
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(_INET_LBL_RE_COLOR);
			return (HBRUSH)GetStockObject(NULL_BRUSH);
		}
		else if (GetDlgItem(IDC_CHECK_FPS)->m_hWnd == pWnd->m_hWnd)
		{
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(_INET_LBL_RE_COLOR);
			return (HBRUSH)GetStockObject(NULL_BRUSH);
		}
		else if (GetDlgItem(IDC_CHECK_PASS)->m_hWnd == pWnd->m_hWnd)
		{
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(_INET_LBL_RE_COLOR);
			return (HBRUSH)GetStockObject(NULL_BRUSH);
		}
		else if (GetDlgItem(IDC_DIS_PARAL)->m_hWnd == pWnd->m_hWnd)
		{
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(_INET_LBL_RE_COLOR);
			return (HBRUSH)GetStockObject(NULL_BRUSH);
		}
		else if (GetDlgItem(IDC_STATIC2)->m_hWnd == pWnd->m_hWnd)
		{
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(_INET_LBL_RE_COLOR);
			return (HBRUSH)GetStockObject(NULL_BRUSH);
		}
		else if (GetDlgItem(IDC_STATIC3)->m_hWnd == pWnd->m_hWnd)
		{
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(_INET_LBL_RE_COLOR);
			return (HBRUSH)GetStockObject(NULL_BRUSH);
		}
		else if (GetDlgItem(IDC_STATIC7)->m_hWnd == pWnd->m_hWnd)
		{
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(_INET_LBL_RE_COLOR);
			return (HBRUSH)GetStockObject(NULL_BRUSH);
		}
		else if (GetDlgItem(IDC_STATIC4)->m_hWnd == pWnd->m_hWnd)
		{
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(_INET_LBL_RE_COLOR);
			return (HBRUSH)GetStockObject(NULL_BRUSH);
		}
		else if (GetDlgItem(IDC_STATIC5)->m_hWnd == pWnd->m_hWnd)
		{
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(_INET_LBL_RE_COLOR);
			return (HBRUSH)GetStockObject(NULL_BRUSH);
		}
		else if (GetDlgItem(IDC_RADIO1)->m_hWnd == pWnd->m_hWnd)
		{
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(_INET_LBL_RE_COLOR);
			return (HBRUSH)GetStockObject(NULL_BRUSH);
		}else if (GetDlgItem(IDC_RADIO2)->m_hWnd == pWnd->m_hWnd)
		{
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(_INET_LBL_RE_COLOR);
			return (HBRUSH)GetStockObject(NULL_BRUSH);
		}else if (GetDlgItem(IDC_RADIO3)->m_hWnd == pWnd->m_hWnd)
		{
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(_INET_LBL_RE_COLOR);
			return (HBRUSH)GetStockObject(NULL_BRUSH);
		}
		else if (GetDlgItem(IDC_STATIC6)->m_hWnd == pWnd->m_hWnd)
		{
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(_INET_LBL_RE_COLOR);
			return (HBRUSH)GetStockObject(NULL_BRUSH);
		}
	}
	break;
	if((GetDlgItem(IDC_RADIO1)->m_hWnd == pWnd->m_hWnd)
				|| (GetDlgItem(IDC_RADIO2)->m_hWnd == pWnd->m_hWnd)|| (GetDlgItem(IDC_RADIO3)->m_hWnd == pWnd->m_hWnd))
			{
				pDC->SetBkMode(TRANSPARENT);
				pDC->SetBkColor(RGB(101, 103,117));
				pDC->SetTextColor(RGB(255, 255, 255));
				return m_StaticBrushBlack;
			}
	case CTLCOLOR_BTN:
	{
		if (GetDlgItem(ID_SAVE)->m_hWnd == pWnd->m_hWnd)
		{
			pDC->SetBkMode(TRANSPARENT);
				pDC->SetBkColor(RGB(101, 103,117));
			pDC->SetTextColor(RGB(189, 194, 198));
			return m_StaticBrushBlack;
		}
	}
	break;
	default:
	{
	}
	}

	// TODO: Return a different brush if the default is not desired
	return hbr;
}
void InetConfiguratorDLG::OnNCPaint()
{
	CPaintDC PaintDC(this);
	CDC		dcMem;
	BITMAP	stBitmap;

	dcMem.CreateCompatibleDC(&PaintDC);
	CBitmap *OldBitmap = dcMem.SelectObject(&m_BackGround);

	m_BackGround.GetObject(sizeof(BITMAP), &stBitmap);
	PaintDC.BitBlt(0, 0, stBitmap.bmWidth, stBitmap.bmHeight, &dcMem, 0, 0, SRCCOPY);
}
void InetConfiguratorDLG::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM)dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		//dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CPaintDC PaintDC(this);
		CDC		dcMem;
		BITMAP	stBitmap;

		dcMem.CreateCompatibleDC(&PaintDC);
		CBitmap *OldBitmap = dcMem.SelectObject(&m_BackGround);

		m_BackGround.GetObject(sizeof(BITMAP), &stBitmap);
		PaintDC.BitBlt(0, 0, stBitmap.bmWidth, stBitmap.bmHeight, &dcMem, 0, 0, SRCCOPY);


		//CDialog::OnPaint();
	}
}
void InetConfiguratorDLG::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);
	RandomMoveWindow();
}
BOOL InetConfiguratorDLG::BitmapRgn(LPCTSTR resource, COLORREF TansColor, int nx, int ny)
{
	HBITMAP			m_hBack;
	HINSTANCE hInstance = AfxGetInstanceHandle();

	HANDLE handle = ::LoadImage(hInstance, resource, IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS | LR_LOADFROMFILE);

	if (!handle) return FALSE;

	m_hBack = (HBITMAP)handle;
	::SetWindowRgn(m_hWnd, BitmapToRegion(m_hBack, TansColor, TansColor, nx, ny), TRUE);

	return TRUE;
}
BOOL InetConfiguratorDLG::BitmapRgn(UINT resource, COLORREF TansColor, int nx, int ny)
{
	HBITMAP			m_hBack;
	HINSTANCE hInstance = AfxGetInstanceHandle();
	m_hBack = (HBITMAP)LoadBitmap(hInstance, MAKEINTRESOURCE(resource));
	::SetWindowRgn(m_hWnd, BitmapToRegion(m_hBack, TansColor, TansColor, nx, ny), TRUE);

	return TRUE;
}
HRGN InetConfiguratorDLG::BitmapToRegion(HBITMAP hBmp, COLORREF cTransparentColor/* = 0*/, COLORREF cTolerance/* = 0x101010*/, int nx, int ny)
{
	HRGN hRgn = NULL;

	if (hBmp)
	{
		// Create a memory DC inside which we will scan the bitmap content
		HDC hMemDC = CreateCompatibleDC(NULL);
		if (hMemDC)
		{
			// Get bitmap size
			BITMAP bm;
			GetObject(hBmp, sizeof(bm), &bm);

			// Create a 32 bits depth bitmap and select it into the memory DC 
			BITMAPINFOHEADER RGB32BITSBITMAPINFO = {
				sizeof(BITMAPINFOHEADER),	// biSize 
				bm.bmWidth,					// biWidth; 
				bm.bmHeight,				// biHeight; 
				1,							// biPlanes; 
				32,							// biBitCount 
				BI_RGB,						// biCompression; 
				0,							// biSizeImage; 
				0,							// biXPelsPerMeter; 
				0,							// biYPelsPerMeter; 
				0,							// biClrUsed; 
				0							// biClrImportant; 
			};
			VOID * pbits32;
			HBITMAP hbm32 = CreateDIBSection(hMemDC, (BITMAPINFO *)&RGB32BITSBITMAPINFO, DIB_RGB_COLORS, &pbits32, NULL, 0);
			if (hbm32)
			{
				HBITMAP holdBmp = (HBITMAP)SelectObject(hMemDC, hbm32);

				// Create a DC just to copy the bitmap into the memory DC
				HDC hDC = CreateCompatibleDC(hMemDC);
				if (hDC)
				{
					// Get how many bytes per row we have for the bitmap bits (rounded up to 32 bits)
					BITMAP bm32;
					GetObject(hbm32, sizeof(bm32), &bm32);
					while (bm32.bmWidthBytes % 4)
						bm32.bmWidthBytes++;

					// Copy the bitmap into the memory DC
					HBITMAP holdBmp = (HBITMAP)SelectObject(hDC, hBmp);
					BitBlt(hMemDC, nx, ny, bm.bmWidth, bm.bmHeight, hDC, 0, 0, SRCCOPY);

					// For better performances, we will use the ExtCreateRegion() function to create the
					// region. This function take a RGNDATA structure on entry. We will add rectangles by
					// amount of ALLOC_UNIT number in this structure.
#define ALLOC_UNIT	100
					DWORD maxRects = ALLOC_UNIT;
					HANDLE hData = GlobalAlloc(GMEM_MOVEABLE, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects));
					RGNDATA *pData = (RGNDATA *)GlobalLock(hData);
					pData->rdh.dwSize = sizeof(RGNDATAHEADER);
					pData->rdh.iType = RDH_RECTANGLES;
					pData->rdh.nCount = pData->rdh.nRgnSize = 0;
					SetRect(&pData->rdh.rcBound, MAXLONG, MAXLONG, 0, 0);

					// Keep on hand highest and lowest values for the "transparent" pixels
					BYTE lr = GetRValue(cTransparentColor);
					BYTE lg = GetGValue(cTransparentColor);
					BYTE lb = GetBValue(cTransparentColor);
					BYTE hr = min(0xff, lr + GetRValue(cTolerance));
					BYTE hg = min(0xff, lg + GetGValue(cTolerance));
					BYTE hb = min(0xff, lb + GetBValue(cTolerance));

					// Scan each bitmap row from bottom to top (the bitmap is inverted vertically)
					BYTE *p32 = (BYTE *)bm32.bmBits + (bm32.bmHeight - 1) * bm32.bmWidthBytes;
					for (int y = 0; y < bm.bmHeight; y++)
					{
						// Scan each bitmap pixel from left to right
						for (int x = 0; x < bm.bmWidth; x++)
						{
							// Search for a continuous range of "non transparent pixels"
							int x0 = x;
							LONG *p = (LONG *)p32 + x;
							while (x < bm.bmWidth)
							{
								BYTE b = GetRValue(*p);
								if (b >= lr && b <= hr)
								{
									b = GetGValue(*p);
									if (b >= lg && b <= hg)
									{
										b = GetBValue(*p);
										if (b >= lb && b <= hb)
											// This pixel is "transparent"
											break;
									}
								}
								p++;
								x++;
							}

							if (x > x0)
							{
								// Add the pixels (x0, y) to (x, y+1) as a new rectangle in the region
								if (pData->rdh.nCount >= maxRects)
								{
									GlobalUnlock(hData);
									maxRects += ALLOC_UNIT;
									hData = GlobalReAlloc(hData, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects), GMEM_MOVEABLE);
									pData = (RGNDATA *)GlobalLock(hData);
								}
								RECT *pr = (RECT *)&pData->Buffer;
								SetRect(&pr[pData->rdh.nCount], x0, y, x, y + 1);
								if (x0 < pData->rdh.rcBound.left)
									pData->rdh.rcBound.left = x0;
								if (y < pData->rdh.rcBound.top)
									pData->rdh.rcBound.top = y;
								if (x > pData->rdh.rcBound.right)
									pData->rdh.rcBound.right = x;
								if (y + 1 > pData->rdh.rcBound.bottom)
									pData->rdh.rcBound.bottom = y + 1;
								pData->rdh.nCount++;

								// On Windows98, ExtCreateRegion() may fail if the number of rectangles is too
								// large (ie: > 4000). Therefore, we have to create the region by multiple steps.
								if (pData->rdh.nCount == 2000)
								{
									HRGN h = ExtCreateRegion(NULL, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects), pData);
									if (hRgn)
									{
										CombineRgn(hRgn, hRgn, h, RGN_OR);
										DeleteObject(h);
									}
									else
										hRgn = h;
									pData->rdh.nCount = 0;
									SetRect(&pData->rdh.rcBound, MAXLONG, MAXLONG, 0, 0);
								}
							}
						}

						// Go to next row (remember, the bitmap is inverted vertically)
						p32 -= bm32.bmWidthBytes;
					}

					// Create or extend the region with the remaining rectangles
					HRGN h = ExtCreateRegion(NULL, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects), pData);
					if (hRgn)
					{
						CombineRgn(hRgn, hRgn, h, RGN_OR);
						DeleteObject(h);
					}
					else
						hRgn = h;

					// Clean up
					GlobalFree(hData);
					SelectObject(hDC, holdBmp);
					DeleteDC(hDC);
				}

				DeleteObject(SelectObject(hMemDC, holdBmp));
			}

			DeleteDC(hMemDC);
		}
	}

	return hRgn;
}