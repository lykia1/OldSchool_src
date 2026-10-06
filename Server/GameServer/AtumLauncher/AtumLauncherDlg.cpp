// AtumLauncherDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AtumLauncher.h"
#include "AtumLauncherDlg.h"
#include <iostream>
#include <strstream>
#include <fstream>
#include "dbgout_c.h"
#include "ftpdownload.h"
#include <direct.h>
#include "md5_lib_src.h"
#include "Wininet.h"
#include "FTPManager.h"
#include "ZipArchive/ZipArchive.h"
#ifdef _INET_MAC_ADDRESS_CHECKER
//#include <lm.h>
//#include <assert.h>
//#pragma comment(lib, "Netapi32.lib")
#pragma comment(lib, "iphlpapi.lib")
//#include <rpc.h>
//#include <rpcdce.h>
#pragma comment(lib, "rpcrt4.lib")
#endif
// ysw
#include <afxhtml.h>
#include "MGameDecryption.h"
#include "AtumError.h"
#include "HttpManager.h"					// 2007-01-08 by cmkwon
//#include "screenkeyboarddlg.h"				// 2007-09-10 by cmkwon, şŁĆ®ł˛ Č­¸éĹ°ş¸µĺ ±¸Çö -
//#include "DlgGuardAgreement.h"				// 2009-08-31 by cmkwon, Gameforge4D °ÔŔÓ°ˇµĺ µżŔÇĂ˘ ¶çżě±â - 
#ifdef _INET_CONFIGURATOR
#include "InetConfiguratorDLG.h"
#endif

//07-03-2020 by Inetpub for libcurl dl
#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <shellapi.h>
//end of inet
//26-04-2020 by Inetpub:: Launcher would make dmp at crash
//#include "dbgHelp.h"
//#pragma comment(lib, "dbghelp.lib")
//LONG __stdcall exception_minidump(_EXCEPTION_POINTERS* p_exception_info)
//{
//	char fileName[MAX_PATH];
//	GetModuleFileName(nullptr, fileName, sizeof(fileName));
//	char* ext = strrchr(fileName, '.');
//	strcpy(ext ? ext : fileName + strlen(fileName), ".dmp");
//
//	char temp[256];
//	wsprintf(temp, "Exception 0x%08x arised !!", p_exception_info->ExceptionRecord->ExceptionCode);
//	MessageBox(nullptr, temp, fileName, MB_OK);
//
//	auto* const h_process = GetCurrentProcess();
//	auto const dw_process_id = GetCurrentProcessId();
//	auto* const h_file = CreateFile(fileName, GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
//
//	MINIDUMP_EXCEPTION_INFORMATION eInfo;
//	eInfo.ThreadId = GetCurrentThreadId();
//	eInfo.ExceptionPointers = p_exception_info;
//	eInfo.ClientPointers = FALSE;
//
//	MiniDumpWriteDump(h_process, dw_process_id, h_file, MiniDumpNormal, p_exception_info ? &eInfo : nullptr, nullptr, nullptr);
//
//	return EXCEPTION_EXECUTE_HANDLER;
//}
//end of inet's exceptions

using namespace std;
CAtumLauncherDlg* cal = nullptr;
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

constexpr auto NOTICE_FILE_NAME = "notice.txt";
constexpr auto DELFILELIST_FILE_NAME = "deletefilelist.txt";

constexpr auto STRING_SERVER_GROUP_NAME_DELIMIT = " ";
constexpr auto TICKGAP_NETWORK_STATE_WORST_PING_TICK = 1500;

static const CRect ACETR_NAV_HOME_RECT(18, 150, 230, 207);
static const CRect ACETR_NAV_NEWS_RECT(18, 207, 230, 264);
static const CRect ACETR_NAV_EVENTS_RECT(18, 264, 230, 321);
static const CRect ACETR_NAV_WEB_RECT(18, 321, 230, 378);
static const CRect ACETR_NAV_DISCORD_RECT(18, 378, 230, 435);

static size_t LauncherApiWriteCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
	const size_t total = size * nmemb;
	std::string* output = reinterpret_cast<std::string*>(userp);
	output->append(reinterpret_cast<const char*>(contents), total);
	return total;
}

static CString GetLauncherApiBaseUrl()
{
	char baseUrl[1024] = {0};
	GetPrivateProfileString("LauncherApi", "BaseUrl", "http://127.0.0.1:5080",
		baseUrl, sizeof(baseUrl), _INI_FILE_NAME);
	CString result(baseUrl);
	while (!result.IsEmpty() && result.Right(1) == "/")
		result = result.Left(result.GetLength() - 1);
	return result;
}

static void OpenLauncherConfiguredUrl(LPCSTR key)
{
	char defaultUrl[1024] = {0};
	lstrcpyn(defaultUrl, STRMSG_S_GAMEHOMEPAGE_DOMAIN, sizeof(defaultUrl));

	// Web should use the main site by default. Discord intentionally has
	// no hard-coded invite; set it in [LauncherLinks] to enable it.
	if (lstrcmpi(key, "Discord") == 0)
		defaultUrl[0] = 0;

	char url[1024] = {0};
	GetPrivateProfileString("LauncherLinks", key, defaultUrl,
		url, sizeof(url), _INI_FILE_NAME);

	if (url[0] == 0)
		return;

	ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
}		// 2007-06-21 by cmkwon, Ćň±Ő Ping ĽÓµµ¸¦ ¸®ĹĎÇĎµµ·Ď ĽöÁ¤ÇÔ

struct SWINDOW_DEGREE
{
	char	*szWindowDegreeName;
	int		nCX;
	int		nCY;
	int		nDegree;
};

// 2007-12-27 by cmkwon, Ŕ©µµżěÁî ¸đµĺ ±â´É Ăß°ˇ - Ăß°ˇÇÔ
SWINDOW_DEGREE g_pWindowDegreeList[] =
{
	{STRMSG_WINDOW_DEGREE_1024x768_LOW,		1024, 768, 0},
	{STRMSG_WINDOW_DEGREE_1024x768_MEDIUM,	1024, 768, 1},
	{STRMSG_WINDOW_DEGREE_1024x768_HIGH,	1024, 768, 2},

	{STRMSG_WINDOW_DEGREE_W1280x720_LOW,	1280, 720, 0},
	{STRMSG_WINDOW_DEGREE_W1280x720_MEDIUM,	1280, 720, 1},
	{STRMSG_WINDOW_DEGREE_W1280x720_HIGH,	1280, 720, 2},
	
	{STRMSG_WINDOW_DEGREE_W1280x800_LOW,	1280, 800, 0},
	{STRMSG_WINDOW_DEGREE_W1280x800_MEDIUM,	1280, 800, 1},
	{STRMSG_WINDOW_DEGREE_W1280x800_HIGH,	1280, 800, 2},
	
	{STRMSG_WINDOW_DEGREE_1280x960_LOW,		1280, 960, 0},
	{STRMSG_WINDOW_DEGREE_1280x960_MEDIUM,	1280, 960, 1},
	{STRMSG_WINDOW_DEGREE_1280x960_HIGH,	1280, 960, 2},
	
	{STRMSG_WINDOW_DEGREE_1280x1024_LOW,	1280, 1024, 0},
	{STRMSG_WINDOW_DEGREE_1280x1024_MEDIUM,	1280, 1024, 1},
	{STRMSG_WINDOW_DEGREE_1280x1024_HIGH,	1280, 1024, 2},
	//by Inetpub
	{ "1366x768 L", 1366, 768, 0 },
	{ "1366x768 M", 1366, 768, 1 },
	{ "1366x768 H", 1366, 768, 2 },
	//end of inet
	// 2008-02-11 by cmkwon, ÇŘ»óµµ Ăß°ˇ(1440x900) - 
	{STRMSG_WINDOW_DEGREE_1440x900_LOW,		1440, 900, 0},
	{STRMSG_WINDOW_DEGREE_1440x900_MEDIUM,	1440, 900, 1},
	{STRMSG_WINDOW_DEGREE_1440x900_HIGH,	1440, 900, 2},

	{STRMSG_WINDOW_DEGREE_W1600x900_LOW,	1600, 900, 0},
	{STRMSG_WINDOW_DEGREE_W1600x900_MEDIUM,	1600, 900, 1},
	{STRMSG_WINDOW_DEGREE_W1600x900_HIGH,	1600, 900, 2},
	
	{STRMSG_WINDOW_DEGREE_1600x1200_LOW,	1600, 1200, 0},
	{STRMSG_WINDOW_DEGREE_1600x1200_MEDIUM,	1600, 1200, 1},
	{STRMSG_WINDOW_DEGREE_1600x1200_HIGH,	1600, 1200, 2},

	{STRMSG_WINDOW_DEGREE_1680x1050_LOW		,	1680, 1050, 0},		// 2009-10-16 by cmkwon, Áöżř ÇŘ»óµµ Ăß°ˇ(1680x1050,1920x1080,1920x1200) - 
	{STRMSG_WINDOW_DEGREE_1680x1050_MEDIUM	,	1680, 1050, 1},		// 2009-10-16 by cmkwon, Áöżř ÇŘ»óµµ Ăß°ˇ(1680x1050,1920x1080,1920x1200) - 
	{STRMSG_WINDOW_DEGREE_1680x1050_HIGH	,	1680, 1050, 2},		// 2009-10-16 by cmkwon, Áöżř ÇŘ»óµµ Ăß°ˇ(1680x1050,1920x1080,1920x1200) - 

	{STRMSG_WINDOW_DEGREE_1920x1080_LOW		,	1920, 1080, 0},		// 2009-10-16 by cmkwon, Áöżř ÇŘ»óµµ Ăß°ˇ(1680x1050,1920x1080,1920x1200) - 
	{STRMSG_WINDOW_DEGREE_1920x1080_MEDIUM	,	1920, 1080, 1},		// 2009-10-16 by cmkwon, Áöżř ÇŘ»óµµ Ăß°ˇ(1680x1050,1920x1080,1920x1200) - 
	{STRMSG_WINDOW_DEGREE_1920x1080_HIGH	,	1920, 1080, 2},		// 2009-10-16 by cmkwon, Áöżř ÇŘ»óµµ Ăß°ˇ(1680x1050,1920x1080,1920x1200) - 

	{STRMSG_WINDOW_DEGREE_1920x1200_LOW		,	1920, 1200, 0},		// 2009-10-16 by cmkwon, Áöżř ÇŘ»óµµ Ăß°ˇ(1680x1050,1920x1080,1920x1200) - 
	{STRMSG_WINDOW_DEGREE_1920x1200_MEDIUM	,	1920, 1200, 1},		// 2009-10-16 by cmkwon, Áöżř ÇŘ»óµµ Ăß°ˇ(1680x1050,1920x1080,1920x1200) - 
	{STRMSG_WINDOW_DEGREE_1920x1200_HIGH	,	1920, 1200, 2},		// 2009-10-16 by cmkwon, Áöżř ÇŘ»óµµ Ăß°ˇ(1680x1050,1920x1080,1920x1200) - 
	
	//by Inetpub
	{ "2560x1440 L", 2560, 1440, 0 },
	{ "2560x1440 M", 2560, 1440, 1 },
	{ "2560x1440 H", 2560, 1440, 2 },
	//end of inet
	
	{ "Borderless (low)", 0, 0, 0 },
	{ "Borderless (medium)", 0, 0, 1 },
	{ "Borderless (high)", 0, 0, 2 },
	{nullptr, 0, 0, 0}		// 2007-12-28 by cmkwon, łˇŔ» ±¸şĐÇĎ±â Ŕ§ÇŘ
};

	#define ICON_IMAGE_ID		IDB_ICON_VTC

bool IsRuntimeInstalled()
{
	HINSTANCE hinstVCDll = LoadLibrary("vcomp142.dll"); //check for redistributable package
	bool bRet = hinstVCDll != nullptr ? true : false;
	FreeLibrary(hinstVCDll);
	return bRet;
}
void ExecuteApplication(LPCSTR lpApplicationName)
{
	STARTUPINFOA startup;
	PROCESS_INFORMATION procInf;

	ZeroMemory(&startup, sizeof(startup));
	startup.cb = sizeof(startup);
	ZeroMemory(&procInf, sizeof(procInf));


	CreateProcessA
	(
		lpApplicationName,   // the path
		nullptr,                // Command line
		nullptr,                   // Process handle not inheritable
		nullptr,                   // Thread handle not inheritable
		FALSE,                  // Set handle inheritance to FALSE
		CREATE_NEW_CONSOLE,     // Opens file in a separate console
		nullptr,           // Use parent's environment block
		nullptr,           // Use parent's starting directory 
		&startup,            // Pointer to STARTUPINFO structure
		&procInf           // Pointer to PROCESS_INFORMATION structure
	);
 
	CloseHandle(procInf.hProcess);
	CloseHandle(procInf.hThread);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){   return 0;}
CAtumLauncherDlg::CAtumLauncherDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAtumLauncherDlg::IDD, pParent), m_ctrlServerList(IDB_SERVERUSER_VTC, IDB_LISTBG_VTC, ICON_IMAGE_ID)
{
//	SetUnhandledExceptionFilter(exception_minidump);
#ifdef _DEBUG
#define DBGOUT printf
#define DbgOut DBGOUT
	AllocConsole();
	freopen("CONIN$", "r", stdin);
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);
	system("pause");
#endif
	cal = this;
	ptmpMsg = __nullptr;
	//{{AFX_DATA_INIT(CAtumLauncherDlg)
	m_nServer = -1;
	m_szAccountName = _T("");
	m_szPassword = _T("");
	m_staticNumFileCtrl = _T("");
	m_nWindowDegree = 0;
	m_ctlbWindowMode = FALSE;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_pUpdateWinsocket = nullptr;
	m_nOldSel = 0;
//	m_pFieldWinsocket = 0;
	m_bControlEnabled = TRUE;
	m_nModernNavHover = 0;
	m_bModernNavTracking = FALSE;
	m_bLauncherLoggedIn = FALSE;
	m_nLauncherAccountPage = 0;
	m_nLauncherMainPage = 0;
	m_szLauncherSessionToken.Empty();
	m_szLauncherCharacterData.Empty();
	MEMSET_ZERO(m_szLaunchCmdLine, sizeof(m_szLaunchCmdLine));
	MEMSET_ZERO(m_szLaunchAppPath, sizeof(m_szLaunchAppPath));
	MEMSET_ZERO(m_szLaunchCmdParam, sizeof(m_szLaunchCmdParam));

	m_StaticBrushBlack.CreateSolidBrush(RGB(22, 25, 34));
	m_StaticBrushGray.CreateSolidBrush(RGB(27, 30, 40));
	m_ListBrushGray.CreateSolidBrush(RGB(212, 208, 200));
	
	m_listBrush.CreateSolidBrush(RGB(17,20,27));

	m_progressBk.CreateSolidBrush(RGB(79,79,79));	
	m_progressBar.CreateSolidBrush(RGB(99, 123, 246));	

	m_pUpdateFTPManager	= nullptr;
	m_pHttpManager		= nullptr;				// 2007-01-08 by cmkwon
	SetFTPUpdateState(UPDATE_STATE_INIT);


	strcpy(m_szLocalIP, "127.0.0.1");

	m_nBirthYear		= 0;				// 2007-06-05 by cmkwon

	///////////////////////////////////////////////////////////////////////////////
	// 2007-06-18 by cmkwon, ł×Ć®żöĹ© »óĹÂ ĂĽĹ© 
	m_nMaxNetworkCheckCount		= 5;
	m_nCurNetworkCheckCount		= -1;
	m_dwNetworkCheckSendTick	= 0;
	m_dwSumPacketTickGap		= 0;

	///////////////////////////////////////////////////////////////////////////////
	// 2007-09-07 by cmkwon, şŁĆ®ł˛ ·±Ăł ŔÎĹÍĆäŔĚ˝ş ĽöÁ¤ - ĂĘ±âČ­ Ŕ§Äˇ şŻ°ć
	m_Cur_Percent				= 0;
	m_SelectFlag				= FALSE;	
	m_bCancelFlag				= FALSE;
	m_bProcessingVersionUpdate	= FALSE;
	m_bShowPreServerIPDlg		= TRUE;
#ifdef _INET_CONFIGURATOR
	m_pInetCFG = nullptr;
#endif
	// 2007-09-11 by cmkwon, şŁĆ®ł˛ Č­¸éĹ°ş¸µĺ ±¸Çö -
	//m_pScreenKeyboardDlg			= nullptr;
	m_pInputEditFromScreenKeyboard	= nullptr;
	m_bHideScreenKeyboardByScreenKeyboardWindow	= FALSE; // 2007-09-18 by cmkwon, Č­»óĹ°ş¸µĺ ĽöÁ¤ - 

	m_vectServerGroupList.clear();		// 2008-02-14 by cmkwon, ·±ĂłżˇĽ­ Ľ­ąö±×·ě ¸íŔĚ ±úÁ®µµ °ÔŔÓ ˝ÇÇŕżˇ ą®Á¦°ˇ ľřµµ·Ď ĽöÁ¤ - 

	m_bGuardAgreementReg		= FALSE;	// 2009-08-31 by cmkwon, Gameforge4D °ÔŔÓ°ˇµĺ µżŔÇĂ˘ ¶çżě±â - 

	bDownloadedNoticeFile = false;
}

void CAtumLauncherDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAtumLauncherDlg)
	
	
	// 2008-12-17 by ckPark ·Ż˝ĂľĆ ·±ĂÄ
	// ·Ż˝ĂľĆ Ŕüżë ÄÁĆ®·Ńµé
#if defined(SERVICE_TYPE_RUSSIAN_SERVER_1)		// ·Ż˝ĂľĆ ·±Ăł ŔÎĹÍĆäŔĚ˝ş ĽöÁ¤
	DDX_Control(pDX, IDC_COMBO_WINDOW_DEGREE_LAUNCHER, m_ctrlComboWindowDegree);
	DDX_Control(pDX, IDC_CHECK_WINDOWS_MODE, m_ctrlCheckWindowMode);

	DDX_Control(pDX, IDC_EDIT_PASSWORD, m_ctrlEditPassword);
#endif
	// end 2008-12-17 by ckPark ·Ż˝ĂľĆ ·±ĂÄ



	DDX_Control(pDX, IDC_CHECK_REMEMBER_ID, m_ctlBtnRememberID);
	DDX_Control(pDX, IDC_COMBO_SERVER_LIST, m_comboServerList);
	DDX_Control(pDX, IDC_EDIT_ACCOUNT, m_ctrlEditAccount);
	DDX_Control(pDX, IDC_LIST, m_ctrlServerList);					// 2006-05-02 by ispark
	DDX_Control(pDX, IDC_PROGRESS1, m_progressCtrl);
#ifdef _INET_CONFIGURATOR
	DDX_Control(pDX, IDC_BTN_VIEW_INET_CFG, m_ctlINETCfgBtn);
#endif
	DDX_Text(pDX, IDC_EDIT_ACCOUNT, m_szAccountName);
	DDV_MaxChars(pDX, m_szAccountName, 21);
	DDX_Text(pDX, IDC_EDIT_PASSWORD, m_szPassword);
	DDV_MaxChars(pDX, m_szPassword, 50);
	DDX_Text(pDX, IDC_DOWNLOAD_FILENUM, m_staticNumFileCtrl);
	DDX_CBIndex(pDX, IDC_COMBO_WINDOW_DEGREE_LAUNCHER, m_nWindowDegree);
	DDX_Check(pDX, IDC_CHECK_WINDOWS_MODE, m_ctlbWindowMode);
	DDX_Control(pDX, IDC_CHECK_64_BIT, m_ctrl64Bit);
	DDX_Control(pDX, IDGO, m_KbcGO);
	DDX_Control(pDX, IDCAN, m_INET_CLOSE);
	DDX_Control(pDX, IDMIN, m_INET_MINIM);
	DDX_Control(pDX, IDJOIN, m_kbcBtnJoin);
	DDX_Control(pDX, IDC_BTN_HOMEPAGE, m_bmpBtnHomepage);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAtumLauncherDlg, CDialog)
	//{{AFX_MSG_MAP(CAtumLauncherDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDOK, OnOk)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDJOIN, OnJoin)
	ON_BN_CLICKED(IDSECRET, OnSecret)
	ON_BN_CLICKED(IDEND, OnEnd)
	ON_BN_CLICKED(IDDECA, OnDeca)
	ON_BN_CLICKED(IDBATTAL, OnBattal)
	ON_BN_CLICKED(IDSHARIN, OnSharin)
	ON_BN_CLICKED(IDPHILON, OnPhilon)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SETCURSOR()
	ON_MESSAGE(WM_MOUSELEAVE, OnModernNavMouseLeave)
	ON_WM_CTLCOLOR()
	ON_LBN_SELCHANGE(IDC_LIST, OnSelchangeList)
	ON_BN_CLICKED(IDCAN, OnCan)
	ON_BN_CLICKED(IDC_CHECK_64_BIT, OnBtnCheck64b)
	ON_BN_CLICKED(IDMIN, OnMin)
	ON_EN_SETFOCUS(IDC_EDIT_ACCOUNT, OnSetfocusEditAccount)
	ON_EN_SETFOCUS(IDC_EDIT_PASSWORD, OnSetfocusEditPassword)
	ON_BN_CLICKED(IDC_BTN_VIEW_SCREEN_KEYBOARD, OnBtnViewScreenKeyboard)
#ifdef _INET_CONFIGURATOR
	ON_BN_CLICKED(IDC_BTN_VIEW_INET_CFG, OnBtnViewInetConfigurator)
#endif
	ON_BN_CLICKED(IDC_BTN_HOMEPAGE, OnBtnHomepage)
	ON_BN_CLICKED(IDGO, OnOk)
	ON_BN_CLICKED(IDC_CHECK_WINDOWS_MODE, OnCheckWindowsMode)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_PACKET_NOTIFY, OnSocketNotify)
	ON_MESSAGE(WM_ASYNC_EVENT, OnAsyncSocketMessage)
	ON_MESSAGE(WM_DOWNLOAD_GAMEFILES_DONE, OnDownLoadGamefilesDone)

	ON_MESSAGE(WM_UPDATEFILE_DOWNLOAD_ERROR, OnUpdateFileDownloadError)
	ON_MESSAGE(WM_UPDATEFILE_DOWNLOAD_INIT, OnUpdateFileDownloadInit)
	ON_MESSAGE(WM_UPDATEFILE_DOWNLOAD_PROGRESS, OnUpdateFileDownloadProgress)
	ON_MESSAGE(WM_UPDATEFILE_DOWNLOAD_OK, OnUpdateFileDownloadOK)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAtumLauncherDlg message handlers


BOOL CAtumLauncherDlg::ReadNoticeFile()
{
	char	szLine[10000];
	CString strTemp;

	m_NoticeEdit = "";

	FILE		*fp;
#ifdef _ATUM_DEVELOP
	fp = fopen(m_szExecutePathReg + NOTICE_FILE_NAME, "r");
#else
	fp = fopen(NOTICE_FILE_NAME, "r");
#endif

	if (fp == nullptr)
	{
		m_NoticeEdit = "No new notifications";
		//szLine[0] = '\0';
		//strcpy(temp, m_NoticeEdit);
		//m_NoticeEdit.Format("%s%s",temp, szLine);
	}
	else
	{
		while(!feof(fp))
		{
			memset(szLine, 0x00, sizeof(szLine));
			if(fgets(szLine, 10000, fp)== nullptr)
			{
				break;
			}

			//szLine[strlen(szLine)] = 0;
			strTemp = m_NoticeEdit;
			m_NoticeEdit.Format("%s\r\n%s", strTemp, szLine);
		}
		fclose(fp);
	}

	// 2007-09-07 by cmkwon, »çżëÇĎÁö ľĘ´Â °ÍŔĚąÇ·Î ÄÁĆ®·ŃŔ» »čÁ¦ÇÔ
	//GetDlgItem(IDC_NOTICE)->SetWindowText(m_NoticeEdit);
	SetNoticeTxt(m_NoticeEdit);
	return TRUE;
}

	#define EXE2_LAUNCHER_BG_SIZE_X                 1280
#define EXE2_LAUNCHER_BG_SIZE_Y                 720

#define EXE2_BG_TITLE_BAR_SIZE_X                EXE2_LAUNCHER_BG_SIZE_X
#define EXE2_BG_TITLE_BAR_SIZE_Y                64

#define EXE2_BG_BACKGROUND_IMAGE_SIZE_X         EXE2_LAUNCHER_BG_SIZE_X
#define EXE2_BG_BACKGROUND_IMAGE_SIZE_Y         EXE2_LAUNCHER_BG_SIZE_Y

// AceTR modern launcher - right login panel
#define EXE2_BG_RESOLUTION_COMBOBOX_POS_X       970
#define EXE2_BG_RESOLUTION_COMBOBOX_POS_Y       455
#define EXE2_BG_RESOLUTION_COMBOBOX_WIDTH       270
#define EXE2_BG_RESOLUTION_COMBOBOX_HEIGHT      180

#define EXE2_BG_WINDOWSMODE_CHECKBOX_POS_X      885
#define EXE2_BG_WINDOWSMODE_CHECKBOX_POS_Y      515
#define EXE2_BG_WINDOWSMODE_CHECKBOX_WIDTH      16
#define EXE2_BG_WINDOWSMODE_CHECKBOX_HEIGHT     16

#define EXE2_BG_ACCOUNTNAME_EDIT_POS_X          985
#define EXE2_BG_ACCOUNTNAME_EDIT_POS_Y          260
#define EXE2_BG_ACCOUNTNAME_EDIT_WIDTH          245
#define EXE2_BG_ACCOUNTNAME_EDIT_HEIGHT         34

#define EXE2_BG_PASSWORD_EDIT_POS_X             985
#define EXE2_BG_PASSWORD_EDIT_POS_Y             322
#define EXE2_BG_PASSWORD_EDIT_WIDTH              245
#define EXE2_BG_PASSWORD_EDIT_HEIGHT             34

// Server list
#define EXE2_BG_SERVERLIST_BOX_POS_X            970
#define EXE2_BG_SERVERLIST_BOX_POS_Y            375
#define EXE2_BG_SERVERLIST_BOX_WIDTH            270
#define EXE2_BG_SERVERLIST_BOX_HEIGHT           48

#define EXE2_BG_SERVERLIST_ITEM_BG_WIDTH        640
#define EXE2_BG_SERVERLIST_ITEM_BG_HEIGHT       32
#define EXE2_BG_SERVERLIST_ITEM_ICON_POS_X      12
#define EXE2_BG_SERVERLIST_ITEM_ICON_POS_Y      10
#define EXE2_BG_SERVERLIST_ITEM_ICON_WIDTH      19
#define EXE2_BG_SERVERLIST_ITEM_ICON_HEIGHT     10

// Window chrome
#define EXE2_BG_MINIMIZED_BTN_POS_X             1200
#define EXE2_BG_MINIMIZED_BTN_POS_Y             18
#define EXE2_BG_MINIMIZED_BTN_WIDTH             26
#define EXE2_BG_MINIMIZED_BTN_HEIGHT            26
#define EXE2_BG_CANCEL_BTN_POS_X                1236
#define EXE2_BG_CANCEL_BTN_POS_Y                18
#define EXE2_BG_CANCEL_BTN_WIDTH                26
#define EXE2_BG_CANCEL_BTN_HEIGHT               26

// Primary action
#define EXE2_BG_GAMESTART_BTN_POS_X             970
#define EXE2_BG_GAMESTART_BTN_POS_Y             545
#define EXE2_BG_GAMESTART_BTN_WIDTH             270
#define EXE2_BG_GAMESTART_BTN_HEIGHT            82

// Patch/update area
#define EXE2_BG_UPDATE_PROGRESS_BAR_POS_X       260
#define EXE2_BG_UPDATE_PROGRESS_BAR_POS_Y       685
#define EXE2_BG_UPDATE_PROGRESS_BAR_WIDTH       680
#define EXE2_BG_UPDATE_PROGRESS_BAR_HEIGHT      10

#define EXE2_BG_DOWNLOAD_FILE_STATIC_POS_X      260
#define EXE2_BG_DOWNLOAD_FILE_STATIC_POS_Y      660
#define EXE2_BG_DOWNLOAD_FILE_STATIC_WIDTH      300
#define EXE2_BG_DOWNLOAD_FILE_STATIC_HEIGHT     20
#define EXE2_BG_DOWNLOAD_FILE_FONT_SIZE         15
#define EXE2_BG_DOWNLOAD_FILE_FONT_WEIGHT       FW_NORMAL
#define EXE2_BG_DOWNLOAD_FILE_FONT_COLOR        RGB(205,205,215)

#define EXE2_BG_UPDATE_INFO_STATIC_POS_X        570
#define EXE2_BG_UPDATE_INFO_STATIC_POS_Y        660
#define EXE2_BG_UPDATE_INFO_STATIC_WIDTH        370
#define EXE2_BG_UPDATE_INFO_STATIC_HEIGHT       20
#define EXE2_BG_UPDATE_INFO_FONT_SIZE           15
#define EXE2_BG_UPDATE_INFO_FONT_WEIGHT         FW_NORMAL
#define EXE2_BG_UPDATE_INFO_FONT_COLOR          RGB(205,205,215)

#define EXE2_BG_REMEMBERID_CHECKBOX_POS_X       985
#define EXE2_BG_REMEMBERID_CHECKBOX_POS_Y       374
#define EXE2_BG_REMEMBERID_CHECKBOX_WIDTH       16
#define EXE2_BG_REMEMBERID_CHECKBOX_HEIGHT      16

#define PLAYERCNT_Y 0
#define PLAYERCNT_X 0
#define PLAYERCNT1_Y 0
#define PLAYERCNT1_X 0

BOOL CAtumLauncherDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	// Add "About..." menu item to system menu.
	if (GetPrivateProfileInt(_GRAPH_MISC, "Run64BitClient", 0, _INI_FILE_NAME) == 1)
		m_ctrl64Bit.SetCheck(TRUE);

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	///////////////////////////////////////////////////////////////////////////////
	// 2009-08-31 by cmkwon, Gameforge4D °ÔŔÓ°ˇµĺ µżŔÇĂ˘ ¶çżě±â - 
#if defined(_DEFINED_GAMEFORGE4D_)
	if(FALSE == this->m_bGuardAgreementReg)
	{
		CDlgGuardAgreement GuardDlg;
		if(IDOK != GuardDlg.DoModal())
		{
			OnOK();
			return FALSE;
		}
		this->m_bGuardAgreementReg	= TRUE;
		((CAtumLauncherApp*)AfxGetApp())->WriteProfile(*this);
	}
#endif
#ifdef _INET_CONFIGURATOR
	m_pInetCFG = new InetConfiguratorDLG(this);
	m_pInetCFG->Create(IDD_INET_CONFIG, this);
	m_pInetCFG->ShowWindow(SW_HIDE);
#endif
#if defined(SERVICE_TYPE_VIETNAMESE_SERVER_1)
	///////////////////////////////////////////////////////////////////////////////
	// 2007-09-10 by cmkwon, şŁĆ®ł˛ Č­¸éĹ°ş¸µĺ ±¸Çö -
	m_pScreenKeyboardDlg	= new CScreenKeyboardDlg(this);
	m_pScreenKeyboardDlg->Create(IDD_DLG_SCREEN_KEYBOARD, this);
	// 2007-09-18 by cmkwon, Č­»óĹ°ş¸µĺ ĽöÁ¤ - ĆĐ˝şżöµĺ ŔÔ·Â˝Ăżˇ¸¸ Č°ĽşČ­ µÇµµ·Ď ŔĎ´Ü Ľű±ä´Ů
	//m_pScreenKeyboardDlg->ShowWindow(SW_SHOW);
	m_pScreenKeyboardDlg->ShowWindow(SW_HIDE);
#endif

	// 2007-09-11 by cmkwon, şŁĆ®ł˛ Č­¸éĹ°ş¸µĺ ±¸Çö - żäĂ»˝Ă±îÁö ąöĆ°Ŕş ş¸ż©ÁöÁö ľČ´Â´Ů
	//GetDlgItem(IDC_BTN_VIEW_SCREEN_KEYBOARD)->ShowWindow(SW_SHOW);		


	// ŔŰľ÷ ÇĄ˝ĂÁŮżˇ ÇÁ·Î±×·Ą¸í ÇĄ˝Ă
	AfxGetApp()->m_pMainWnd->SetWindowText(STRMSG_WINDOW_TEXT);
	// ŔŰľ÷ ÇĄ˝ĂÁŮżˇ ľĆŔĚÄÜ ÇĄ˝Ă
	ModifyStyle(0,WS_SYSMENU);

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	///////////////////////////////////////////////////////////////////////////
	// TODO: Add extra initialization here

	CWinSocket::SocketInit();		// Initialize winsock 2.0

	// AceTR modern launcher: resize the actual MFC window in pixels.
	SetWindowPos(nullptr, 0, 0,
		EXE2_LAUNCHER_BG_SIZE_X,
		EXE2_LAUNCHER_BG_SIZE_Y,
		SWP_NOMOVE | SWP_NOZORDER);

	this->MoveWindow2Center();	// 2007-09-07 by cmkwon, şŁĆ®ł˛ ·±Ăł ŔÎĹÍĆäŔĚ˝ş ĽöÁ¤ - Č­¸é ÁßľÓŔ¸·Î Ŕ§Äˇ
	RECT rtBG;
	this->GetClientRect(&rtBG);

	// 2008-01-03 by cmkwon, Ŕ©µµżě¸đµĺ »óĹÂ ŔúŔĺÇĎ±â - Áöżř ÇŘ»óµµ ¸®˝şĆ® ĂĘ±âČ­
	this->InitSupportedWindowResolutionList();		
	
	///////////////////////////////////////////////////////////////////////////////
	// 2007-05-07 by cmkwon, ÇŘ»óµµ Á¤ş¸¸¦ °˘ łŞ¶óş° ľđľî·Î ĽłÁ¤ÇĎ±â Ŕ§ÇŘ
	CComboBox *pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_WINDOW_DEGREE_LAUNCHER);
	if(pComboBox)
	{
		this->InsertWindowDegreeList(pComboBox, m_ctlbWindowMode);

		int nIdx = this->FindWindowDegreeComboBoxIndex(pComboBox, (LPSTR)(LPCSTR)m_csWindowsResolutionReg);
		nIdx = max(0, nIdx);
		pComboBox->SetCurSel(nIdx);
	}
	
    m_fontServerGroupListBox.CreateFont(16, 0, 0, 0, SG_BOX_FONT_WEIGHT, 0, FALSE, FALSE, SG_BOX_FONT_CHARSET, OUT_DEFAULT_PRECIS,
							  CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH , SG_BOX_FONT_FACENAME);    // "System" Font´Â ´ëÇĄŔűŔÎ Fixed FontŔÓ´Ů.
    GetDlgItem(IDC_LIST)->SetFont(&m_fontServerGroupListBox);

	m_KbcGO.SetModernButton("GİRİŞ YAP", RGB(47, 211, 255));
	m_KbcGO.SetToolTipText("Giris Yap");
	m_kbcBtnJoin.SetBmpButtonImage(IDB_JOINBTN, RGB(0,0,255));
	m_kbcBtnJoin.SetToolTipText("Join");

	GetDlgItem(IDC_COMBO_WINDOW_DEGREE_LAUNCHER)->MoveWindow(EXE2_BG_RESOLUTION_COMBOBOX_POS_X, EXE2_BG_RESOLUTION_COMBOBOX_POS_Y, EXE2_BG_RESOLUTION_COMBOBOX_WIDTH, EXE2_BG_RESOLUTION_COMBOBOX_HEIGHT);

	m_fontModernSmall.CreateFont(15, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, "Segoe UI");
	GetDlgItem(IDC_COMBO_WINDOW_DEGREE_LAUNCHER)->SetFont(&m_fontModernSmall);
	GetDlgItem(IDC_CHECK_WINDOWS_MODE)->SetFont(&m_fontModernSmall);
	m_ctlBtnRememberID.SetFont(&m_fontModernSmall);
	m_ctrl64Bit.SetFont(&m_fontModernSmall);
	GetDlgItem(IDC_CHECK_WINDOWS_MODE)->MoveWindow(EXE2_BG_WINDOWSMODE_CHECKBOX_POS_X, EXE2_BG_WINDOWSMODE_CHECKBOX_POS_Y, EXE2_BG_WINDOWSMODE_CHECKBOX_WIDTH, EXE2_BG_WINDOWSMODE_CHECKBOX_HEIGHT);
	GetDlgItem(IDC_COMBO_WINDOW_DEGREE_LAUNCHER)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_CHECK_WINDOWS_MODE)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_CHECK_64_BIT)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_CHARACTER_NAME)->ShowWindow(SW_HIDE);

	// AccountName Edit Box, Password Edit Box
	GetDlgItem(IDC_EDIT_ACCOUNT)->MoveWindow(EXE2_BG_ACCOUNTNAME_EDIT_POS_X, EXE2_BG_ACCOUNTNAME_EDIT_POS_Y, EXE2_BG_ACCOUNTNAME_EDIT_WIDTH, EXE2_BG_ACCOUNTNAME_EDIT_HEIGHT);
	GetDlgItem(IDC_EDIT_PASSWORD)->MoveWindow(EXE2_BG_PASSWORD_EDIT_POS_X, EXE2_BG_PASSWORD_EDIT_POS_Y, EXE2_BG_PASSWORD_EDIT_WIDTH, EXE2_BG_PASSWORD_EDIT_HEIGHT);

	m_fontModernInput.CreateFont(18, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, "Segoe UI");
	GetDlgItem(IDC_EDIT_ACCOUNT)->SetFont(&m_fontModernInput);
	GetDlgItem(IDC_EDIT_PASSWORD)->SetFont(&m_fontModernInput);
	GetDlgItem(IDC_EDIT_ACCOUNT)->SendMessage(EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN, MAKELPARAM(10, 10));
	GetDlgItem(IDC_EDIT_PASSWORD)->SendMessage(EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN, MAKELPARAM(10, 10));

#if !defined(SERVICE_TYPE_KOREAN_SERVER_2) || defined(_DEBUG)
	GetDlgItem(IDC_EDIT_ACCOUNT)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_EDIT_PASSWORD)->ShowWindow(SW_SHOW);
#endif
	GetDlgItem(IDC_CHECK_REMEMBER_ID)->ShowWindow(SW_SHOW);

	GetDlgItem(IDC_LIST)->MoveWindow(EXE2_BG_SERVERLIST_BOX_POS_X, EXE2_BG_SERVERLIST_BOX_POS_Y, EXE2_BG_SERVERLIST_BOX_WIDTH, EXE2_BG_SERVERLIST_BOX_HEIGHT);
	GetDlgItem(IDC_LIST)->ShowWindow(SW_SHOW);		
	m_ctrlServerList.SetItemHeight(26);
	m_ctrlServerList.SetItemBGImageSize(EXE2_BG_SERVERLIST_ITEM_BG_WIDTH, EXE2_BG_SERVERLIST_ITEM_BG_HEIGHT);
	m_ctrlServerList.SetItemIconImage(EXE2_BG_SERVERLIST_ITEM_ICON_POS_X, EXE2_BG_SERVERLIST_ITEM_ICON_POS_Y, EXE2_BG_SERVERLIST_ITEM_ICON_WIDTH, EXE2_BG_SERVERLIST_ITEM_ICON_HEIGHT);
	m_ServerList = (CListBoxEBX *)GetDlgItem(IDC_LIST);

	m_INET_CLOSE.SetToolTipText("Kapat");
	m_INET_CLOSE.SetModernButton("X", RGB(45, 48, 58));

	m_INET_MINIM.SetToolTipText("Kucult");
	m_INET_MINIM.SetModernButton("-", RGB(45, 48, 58));

	// Minimized Button, Cancel Button
	GetDlgItem(IDMIN)->MoveWindow(EXE2_BG_MINIMIZED_BTN_POS_X, EXE2_BG_MINIMIZED_BTN_POS_Y, EXE2_BG_MINIMIZED_BTN_WIDTH, EXE2_BG_MINIMIZED_BTN_HEIGHT);
	GetDlgItem(IDCAN)->MoveWindow(EXE2_BG_CANCEL_BTN_POS_X, EXE2_BG_CANCEL_BTN_POS_Y, EXE2_BG_CANCEL_BTN_WIDTH, EXE2_BG_CANCEL_BTN_HEIGHT);

	// Game Start Button
	GetDlgItem(IDGO)->MoveWindow(EXE2_BG_GAMESTART_BTN_POS_X, EXE2_BG_GAMESTART_BTN_POS_Y, EXE2_BG_GAMESTART_BTN_WIDTH, EXE2_BG_GAMESTART_BTN_HEIGHT);
#ifdef _INET_CONFIGURATOR
	m_ctlINETCfgBtn.SetToolTipText("Oyun Ayarlari");
	m_ctlINETCfgBtn.SetModernButton("AYAR", RGB(38, 42, 52));

	GetDlgItem(IDC_BTN_VIEW_INET_CFG)->MoveWindow(1168, 660, 72, 30);
#endif	
	// 2007-09-07 by cmkwon, »çżëÇĎÁö ľĘ´Â ąöĆ°ŔÓ
	//// Join Button
	//GetDlgItem(IDJOIN)->MoveWindow(nPosX, nPosY, IMAGE_JOIN_BUTTON_X_SIZE, IMAGE_JOIN_BUTTON_Y_SIZE);

	// Update Progress Bar
	m_progressCtrl.MoveWindow(EXE2_BG_UPDATE_PROGRESS_BAR_POS_X, EXE2_BG_UPDATE_PROGRESS_BAR_POS_Y, EXE2_BG_UPDATE_PROGRESS_BAR_WIDTH, EXE2_BG_UPDATE_PROGRESS_BAR_HEIGHT);
	GetDlgItem(IDC_PLAYER_CNT)->MoveWindow(985, 195, 220, 22);
	GetDlgItem(IDC_PLAYER_CNT)->ShowWindow(SW_SHOW);



	// 2008-12-17 by ckPark ·Ż˝ĂľĆ ·±ĂÄ
#if defined(SERVICE_TYPE_RUSSIAN_SERVER_1)		// ·Ż˝ĂľĆ ·±Ăł ŔÎĹÍĆäŔĚ˝ş ĽöÁ¤
	// ·Ż˝ĂľĆ´Â ÇÁ·Î±×·ˇ˝ş ąŮ »ö±ňŔĚ ´Ů¸Ł´Ů
	m_progressCtrl.SetBkColor(RGB(0, 0, 0));
	m_progressCtrl.SetGradientColors(RGB(18, 236, 218), RGB(18, 236, 218));


	// 2008-12-23 by ckPark ŔĎş» ·±ĂÄ
#elif defined(SERVICE_TYPE_JAPANESE_SERVER_1)
	// ŔĎş» ÇÁ·Î±×·ˇ˝şąŮ »ö±ň ąé±×¶óżîµĺ ÄĂ·Ż
	m_progressCtrl.SetBkColor(RGB(0, 186, 215));
	m_progressCtrl.SetGradientColors(RGB(255, 102, 0), RGB(255, 102, 0));
#else
	// end 2008-12-23 by ckPark ŔĎş» ·±ĂÄ
	
	//BitmapRgn(IDC_PROGRESS1, RGB(255, 255, 255), 0, 0);

	//m_progressCtrl.SetBorders(0);
	m_progressCtrl.SetBkColor(RGB(0, 0, 0));
	m_progressCtrl.SetGradientColors(RGB(225, 82, 35), RGB(225, 82, 35));
#endif
	// end 2008-12-17 by ckPark ·Ż˝ĂľĆ ·±ĂÄ



	// Download File Version Num
	GetDlgItem(IDC_DOWNLOAD_FILENUM)->MoveWindow(EXE2_BG_DOWNLOAD_FILE_STATIC_POS_X, EXE2_BG_DOWNLOAD_FILE_STATIC_POS_Y, EXE2_BG_DOWNLOAD_FILE_STATIC_WIDTH, EXE2_BG_DOWNLOAD_FILE_STATIC_HEIGHT);
	long wndStyle = ::GetWindowLong(GetDlgItem(IDC_DOWNLOAD_FILENUM)->m_hWnd, GWL_STYLE);
    ::SetWindowLong(GetDlgItem(IDC_DOWNLOAD_FILENUM)->m_hWnd, GWL_STYLE, wndStyle | SS_CENTERIMAGE);	// ĂëµćÇŃ Ŕ©µµżě ĽöÁ÷ÁßľÓ(SS_CENTERIMAGE)ĽÓĽşŔ» Ăß°ˇ
	m_fontDownloadFileNum.CreateFont(EXE2_BG_DOWNLOAD_FILE_FONT_SIZE, 0, 0, 0, EXE2_BG_DOWNLOAD_FILE_FONT_WEIGHT, 0, FALSE, FALSE, SG_BOX_FONT_CHARSET, OUT_DEFAULT_PRECIS,
							  CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH , SG_BOX_FONT_FACENAME);    // "System" Font´Â ´ëÇĄŔűŔÎ Fixed FontŔÓ´Ů.
	GetDlgItem(IDC_DOWNLOAD_FILENUM)->SetFont(&m_fontDownloadFileNum);

	// Download File Info
	GetDlgItem(IDC_FILE_INFO)->MoveWindow(EXE2_BG_UPDATE_INFO_STATIC_POS_X, EXE2_BG_UPDATE_INFO_STATIC_POS_Y, EXE2_BG_UPDATE_INFO_STATIC_WIDTH, EXE2_BG_UPDATE_INFO_STATIC_HEIGHT);
	wndStyle = ::GetWindowLong(GetDlgItem(IDC_FILE_INFO)->m_hWnd, GWL_STYLE);
    ::SetWindowLong(GetDlgItem(IDC_FILE_INFO)->m_hWnd, GWL_STYLE, wndStyle | SS_CENTERIMAGE);	// ĂëµćÇŃ Ŕ©µµżě ĽöÁ÷ÁßľÓ(SS_CENTERIMAGE)ĽÓĽşŔ» Ăß°ˇ
	m_fontFileInfo.CreateFont(EXE2_BG_UPDATE_INFO_FONT_SIZE, 0, 0, 0, EXE2_BG_UPDATE_INFO_FONT_WEIGHT, 0, FALSE, FALSE, SG_BOX_FONT_CHARSET, OUT_DEFAULT_PRECIS,
							  CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH , SG_BOX_FONT_FACENAME);    // "System" Font´Â ´ëÇĄŔűŔÎ Fixed FontŔÓ´Ů.
	GetDlgItem(IDC_FILE_INFO)->SetFont(&m_fontFileInfo);

	GetDlgItem(IDC_NOTICE2)->MoveWindow(108, 405, 180, 34);
	wndStyle = ::GetWindowLong(GetDlgItem(IDC_NOTICE2)->m_hWnd, GWL_STYLE);
	::SetWindowLong(GetDlgItem(IDC_NOTICE2)->m_hWnd, GWL_STYLE, wndStyle | SS_CENTERIMAGE);	// ĂëµćÇŃ Ŕ©µµżě ĽöÁ÷ÁßľÓ(SS_CENTERIMAGE)ĽÓĽşŔ» Ăß°ˇ
	m_fontNotice.CreateFont(13, 0, 0, 0, EXE2_BG_UPDATE_INFO_FONT_WEIGHT, 0, FALSE, FALSE, SG_BOX_FONT_CHARSET, OUT_DEFAULT_PRECIS,
							CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH, "Verdana");    // "System" Font´Â ´ëÇĄŔűŔÎ Fixed FontŔÓ´Ů.
	GetDlgItem(IDC_NOTICE2)->SetFont(&m_fontNotice);

	GetDlgItem(IDC_PLAYER_CNT)->MoveWindow(898, 178, 220, 22);
	wndStyle = ::GetWindowLong(GetDlgItem(IDC_PLAYER_CNT)->m_hWnd, GWL_STYLE);
	::SetWindowLong(GetDlgItem(IDC_PLAYER_CNT)->m_hWnd, GWL_STYLE, wndStyle | SS_CENTERIMAGE);	// ĂëµćÇŃ Ŕ©µµżě ĽöÁ÷ÁßľÓ(SS_CENTERIMAGE)ĽÓĽşŔ» Ăß°ˇ
	m_fontPlayers.CreateFont(13, 0, 0, 0, EXE2_BG_UPDATE_INFO_FONT_WEIGHT, 0, FALSE, FALSE, SG_BOX_FONT_CHARSET, OUT_DEFAULT_PRECIS,
							  CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH, "Verdana");    // "System" Font´Â ´ëÇĄŔűŔÎ Fixed FontŔÓ´Ů.
	GetDlgItem(IDC_PLAYER_CNT)->SetFont(&m_fontPlayers);
	

#if defined(SERVICE_TYPE_VIETNAMESE_SERVER_1)
	///////////////////////////////////////////////////////////////////////////////
	// 2007-09-27 by cmkwon, Homepage°ˇ±â ąöĆ° Ăß°ˇ(şŁĆ®ł˛ VTC-Intecom żäĂ») - 
	GetDlgItem(IDC_BTN_HOMEPAGE)->MoveWindow(645, 38, 115, 18);
	GetDlgItem(IDC_BTN_HOMEPAGE)->ShowWindow(SW_SHOW);
	m_bmpBtnHomepage.LoadBitmaps(IDB_HOMEPAGE_UP, IDB_HOMEPAGE_DOWN, IDB_HOMEPAGE_FOCUS);
#endif

// 2008-10-20 by cmkwon, Gameforge4D(Engilsh, German)żˇ Á˘ĽÓ °čÁ¤ ±âľďÇĎ±â ±â´É Ăß°ˇ - ľĆ·ˇżÍ °°ŔĚ ĽöÁ¤
//#if defined(SERVICE_TYPE_KOREAN_SERVER_1) || defined(SERVICE_TYPE_THAI_SERVER_1)	// 2008-06-17 by cmkwon, WinnerOnline_Tha LauncherżˇĽ­ ŔĚŔü Á˘ĽÓ °čÁ¤ ±âľďÇĎ±â(K0000243) - 
// 2008-12-22 by cmkwon, Áöżř Ľ­şń˝ş Ăß°ˇ(Gameforge4D ĹÍĹ°ľĆ, şŇľî, ŔĚĹ»¸®ľĆľî) - ľĆ·ˇżÍ °°ŔĚ 3°ł Ľ­şń˝ş Ăß°ˇ
//#if defined(SERVICE_TYPE_KOREAN_SERVER_1) || defined(SERVICE_TYPE_THAI_SERVER_1) || defined(SERVICE_TYPE_ENGLISH_SERVER_2) || defined(SERVICE_TYPE_GERMAN_SERVER_1)	// 2008-06-17 by cmkwon, WinnerOnline_Tha LauncherżˇĽ­ ŔĚŔü Á˘ĽÓ °čÁ¤ ±âľďÇĎ±â(K0000243) - 
// 2009-06-04 by cmkwon, Áöżř Ľ­şń˝ş Ăß°ˇ(Gameforge4D Ćú¶őµĺľî, ˝şĆäŔÎľî) - 
//#if defined(SERVICE_TYPE_KOREAN_SERVER_1) || defined(SERVICE_TYPE_THAI_SERVER_1) || defined(SERVICE_TYPE_ENGLISH_SERVER_2) || defined(SERVICE_TYPE_GERMAN_SERVER_1) || defined(SERVICE_TYPE_TURKISH_SERVER_1) || defined(SERVICE_TYPE_FRENCH_SERVER_1) || defined(SERVICE_TYPE_ITALIAN_SERVER_1)
// 2010-11-01 by shcho,	 Áöżř Ľ­şń˝ş Ăß°ˇ(Gameforge4D ˝şĆäŔÎľî, ľĆ¸ŁÇîĆĽłŞľî) -  
//#if defined(SERVICE_TYPE_KOREAN_SERVER_1) || defined(SERVICE_TYPE_THAI_SERVER_1) || defined(SERVICE_TYPE_ENGLISH_SERVER_2) || defined(SERVICE_TYPE_GERMAN_SERVER_1) || defined(SERVICE_TYPE_TURKISH_SERVER_1) || defined(SERVICE_TYPE_FRENCH_SERVER_1) || defined(SERVICE_TYPE_ITALIAN_SERVER_1) || defined(SERVICE_TYPE_POLISH_SERVER_1) || defined(SERVICE_TYPE_SPANISH_SERVER_1)
#if defined(SERVICE_TYPE_KOREAN_SERVER_1) || defined(SERVICE_TYPE_THAI_SERVER_1) || defined(SERVICE_TYPE_ENGLISH_SERVER_2) || defined(SERVICE_TYPE_GERMAN_SERVER_1) || defined(SERVICE_TYPE_TURKISH_SERVER_1) || defined(SERVICE_TYPE_FRENCH_SERVER_1) || defined(SERVICE_TYPE_ITALIAN_SERVER_1) || defined(SERVICE_TYPE_POLISH_SERVER_1) || defined(SERVICE_TYPE_SPANISH_SERVER_1) || defined(SERVICE_TYPE_ARGENTINA_SERVER_1) || defined(SERVICE_TYPE_SINGAPORE_1) || defined(SERVICE_TYPE_INDONESIA_SERVER_1) || defined (SERVICE_TYPE_VIETNAMESE_SERVER_1)  || defined(SERVICE_TYPE_ENGLISH_SERVER_1)
	///////////////////////////////////////////////////////////////////////////////
	// 2008-06-17 by cmkwon, WinnerOnline_Tha LauncherżˇĽ­ ŔĚŔü Á˘ĽÓ °čÁ¤ ±âľďÇĎ±â(K0000243) - 
	m_ctlBtnRememberID.ShowWindow(SW_SHOW);	
	m_ctlBtnRememberID.MoveWindow(EXE2_BG_REMEMBERID_CHECKBOX_POS_X, EXE2_BG_REMEMBERID_CHECKBOX_POS_Y, EXE2_BG_REMEMBERID_CHECKBOX_WIDTH, EXE2_BG_REMEMBERID_CHECKBOX_HEIGHT);
	

	if(FALSE == m_szAccountNameReg.IsEmpty())
	{
		m_ctlBtnRememberID.SetCheck(TRUE);
	}

#endif

	m_ctrl64Bit.ShowWindow(SW_SHOW);
	m_ctrl64Bit.MoveWindow(EXE2_BG_REMEMBERID_CHECKBOX_POS_X, 430, 16, 16);
	//m_ctrl64Bit.SetCheck(m_n64Bit);
	
	// 2008-12-23 by ckPark ŔĎş» ·±ĂÄ
#if defined(SERVICE_TYPE_JAPANESE_SERVER_1)
	// ŔĎş» ·±ĂÄżˇ ÇĘżäľř´Â uiµé ŔüşÎ Ľű±č
	GetDlgItem(IDC_LIST)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDIT_ACCOUNT)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDIT_PASSWORD)->ShowWindow(SW_HIDE);
	GetDlgItem(IDGO)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_COMBO_WINDOW_DEGREE_LAUNCHER)->ShowWindow(SW_HIDE);
	GetDlgItem(IDMIN)->ShowWindow(SW_HIDE);
	GetDlgItem(IDCAN)->ShowWindow(SW_HIDE);
	GetDlgItem(IDOK)->ShowWindow(SW_HIDE);
	GetDlgItem(IDCANCEL)->ShowWindow(SW_HIDE);
	GetDlgItem(IDJOIN)->ShowWindow(SW_HIDE);
	GetDlgItem(IDSECRET)->ShowWindow(SW_HIDE);
	GetDlgItem(IDEND)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_CHARACTER_NAME)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_COMBO_SERVER_LIST)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_BTN_VIEW_SCREEN_KEYBOARD)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_BTN_HOMEPAGE)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_CHECK_WINDOWS_MODE)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_CHECK_REMEMBER_ID)->ShowWindow(SW_HIDE);
#endif
	// end 2008-12-23 by ckPark ŔĎş» ·±ĂÄ





	// ÄÁĆ®·Ń Ŕ§Äˇ ĽłÁ¤ÇĎ±â ----> End
	///////////////////////////////////////////////////////////////////////////////


	
	///////////////////////////////////////////////////////////////////////////////
	// ąč°ć Č­¸é ¸¸µé±â ----> Start
	
	CDC *pDC = GetDC();
	int nXScreen = EXE2_LAUNCHER_BG_SIZE_X;
	int nYScreen = EXE2_LAUNCHER_BG_SIZE_Y;
	m_BackGround.CreateCompatibleBitmap(pDC, nXScreen, nYScreen);
		
	CDC		memDCBackGround;	
	memDCBackGround.CreateCompatibleDC(pDC);
	CBitmap	*pOldBitmapBackGround = memDCBackGround.SelectObject(&m_BackGround);
	memDCBackGround.PatBlt(0,0,nXScreen,nYScreen,BLACKNESS);						// ąč°ćŔ» °ËŔş»öŔ¸·Î ĂĘ±âČ­
	
	CDC tmMemDC;
	tmMemDC.CreateCompatibleDC(pDC);

	CBitmap tmBitmap;
	CBitmap *pTmOldBitmap;





	// 2008-12-23 by ckPark ŔĎş» ·±ĂÄ

// 	// Ĺ¸ŔĚĆ˛ąŮ ±×¸®±â(Title Bar)
// 	tmBitmap.LoadBitmap(IDB_TITLE);
// 	pTmOldBitmap = tmMemDC.SelectObject(&tmBitmap);
// 	memDCBackGround.BitBlt(0, 0, EXE2_BG_TITLE_BAR_SIZE_X, EXE2_BG_TITLE_BAR_SIZE_Y, &tmMemDC, 0, 0, SRCCOPY);
// 	tmMemDC.SelectObject(pTmOldBitmap);
// 	tmBitmap.DeleteObject();
// 	
// 	// ąč°ć Č­¸é ±×¸®±â(Background)
// 	tmBitmap.LoadBitmap(IDB_BG_VTC);
// 	pTmOldBitmap = tmMemDC.SelectObject(&tmBitmap);
// 	memDCBackGround.BitBlt(0, EXE2_BG_TITLE_BAR_SIZE_Y,EXE2_BG_BACKGROUND_IMAGE_SIZE_X, EXE2_BG_BACKGROUND_IMAGE_SIZE_Y,&tmMemDC,0,0,SRCCOPY);
// 	tmMemDC.SelectObject(pTmOldBitmap);
// 	tmBitmap.DeleteObject();
// 	
// 	memDCBackGround.SelectObject(pOldBitmapBackGround);

#if defined(SERVICE_TYPE_JAPANESE_SERVER_1)
	// ŔĎş» ·±ĂÄ´Â Ĺ¸ŔĚĆ˛ ąŮ°ˇ ľř´Ů
	// ąč°ć Č­¸é ±×¸®±â(Background)
	tmBitmap.LoadBitmap(IDB_BG_VTC);
	pTmOldBitmap = tmMemDC.SelectObject(&tmBitmap);
	memDCBackGround.BitBlt(0, 0,EXE2_LAUNCHER_BG_SIZE_X, EXE2_LAUNCHER_BG_SIZE_Y,&tmMemDC,0,0,SRCCOPY);
	tmMemDC.SelectObject(pTmOldBitmap);
	tmBitmap.DeleteObject();
	
	memDCBackGround.SelectObject(pOldBitmapBackGround);

#else

	// Ĺ¸ŔĚĆ˛ąŮ ±×¸®±â(Title Bar)
	/*tmBitmap.LoadBitmap(IDB_TITLE);
	pTmOldBitmap = tmMemDC.SelectObject(&tmBitmap);
	memDCBackGround.BitBlt(0, 0, EXE2_BG_TITLE_BAR_SIZE_X, EXE2_BG_TITLE_BAR_SIZE_Y, &tmMemDC, 0, 0, SRCCOPY);
	tmMemDC.SelectObject(pTmOldBitmap);
	tmBitmap.DeleteObject();*/

	// ąč°ć Č­¸é ±×¸®±â(Background)

	// AceTR modern launcher shell - native GDI, no external image dependency.
	CBrush brushBase(RGB(10, 12, 18));
	CBrush brushTop(RGB(18, 21, 29));
	CBrush brushPanel(RGB(24, 27, 37));
	CBrush brushPanel2(RGB(18, 21, 29));
	CBrush brushCard(RGB(30, 34, 46));
	CBrush brushAccent(RGB(225, 82, 35));
	CBrush brushAccentSoft(RGB(109, 48, 31));
	CBrush brushOnline(RGB(42, 190, 108));
	CPen penBorder(PS_SOLID, 1, RGB(52, 58, 74));
	CPen penSoft(PS_SOLID, 1, RGB(42, 47, 61));

	memDCBackGround.FillRect(CRect(0, 0, 1200, 700), &brushBase);
	memDCBackGround.FillRect(CRect(0, 0, 1200, 64), &brushTop);
	memDCBackGround.FillRect(CRect(0, 62, 1200, 64), &brushAccent);

	CPen* pOldPen = memDCBackGround.SelectObject(&penBorder);
	CBrush* pOldBrush = memDCBackGround.SelectObject(&brushPanel);

	// Hero / news area
	memDCBackGround.RoundRect(CRect(55, 82, 805, 472), CPoint(18, 18));
	memDCBackGround.SelectObject(&brushCard);
	memDCBackGround.RoundRect(CRect(75, 132, 785, 360), CPoint(16, 16));

	// Faux cinematic banner layers
	memDCBackGround.SelectObject(&brushAccentSoft);
	memDCBackGround.RoundRect(CRect(90, 150, 770, 344), CPoint(14, 14));
	memDCBackGround.SelectObject(&brushCard);
	memDCBackGround.RoundRect(CRect(110, 170, 750, 324), CPoint(12, 12));

	// News cards
	memDCBackGround.SelectObject(&brushCard);
	memDCBackGround.RoundRect(CRect(92, 370, 305, 448), CPoint(12, 12));
	memDCBackGround.RoundRect(CRect(315, 370, 528, 448), CPoint(12, 12));
	memDCBackGround.RoundRect(CRect(538, 370, 770, 448), CPoint(12, 12));

	// Login panel
	memDCBackGround.SelectObject(&brushPanel2);
	memDCBackGround.RoundRect(CRect(840, 82, 1160, 625), CPoint(18, 18));

	// Server status card
	memDCBackGround.SelectObject(&brushPanel2);
	memDCBackGround.RoundRect(CRect(55, 485, 805, 602), CPoint(16, 16));

	// Bottom updater
	memDCBackGround.SelectObject(&brushPanel2);
	memDCBackGround.RoundRect(CRect(55, 615, 1160, 682), CPoint(14, 14));

	memDCBackGround.SetBkMode(TRANSPARENT);
	memDCBackGround.SetTextColor(RGB(245, 247, 251));

	CFont brandFont;
	brandFont.CreateFont(30, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, "Segoe UI");
	CFont* pOldFont = memDCBackGround.SelectObject(&brandFont);
	memDCBackGround.TextOut(28, 17, "AceTR");

	CFont navFont;
	navFont.CreateFont(15, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, 0,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, "Segoe UI");
	memDCBackGround.SelectObject(&navFont);
	memDCBackGround.SetTextColor(RGB(174, 180, 194));

	CFont sectionFont;
	sectionFont.CreateFont(17, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, "Segoe UI");
	memDCBackGround.SelectObject(&sectionFont);
	memDCBackGround.SetTextColor(RGB(215, 219, 229));
	memDCBackGround.TextOut(75, 100, "GUNCEL");
	memDCBackGround.TextOut(865, 108, "HESABINLA GİRİŞ YAP");
	memDCBackGround.TextOut(75, 494, "SUNUCU DURUMU");

	CFont heroFont;
	heroFont.CreateFont(31, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, "Segoe UI");
	memDCBackGround.SelectObject(&heroFont);
	memDCBackGround.SetTextColor(RGB(255, 255, 255));
	memDCBackGround.TextOut(145, 210, "ACE TR");
	memDCBackGround.SetTextColor(RGB(245, 132, 88));
	memDCBackGround.TextOut(145, 182, "REKABETCI HAVA SAVASI");

	CFont heroSubFont;
	heroSubFont.CreateFont(17, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, "Segoe UI");
	memDCBackGround.SelectObject(&heroSubFont);
	memDCBackGround.SetTextColor(RGB(220, 224, 232));
	memDCBackGround.TextOut(146, 250, "Gökyüzündeki savaş yeniden başlıyor.");
	memDCBackGround.SetTextColor(RGB(245, 132, 88));
	memDCBackGround.TextOut(146, 285, "SEZON • ETKİNLİK • NATION WAR");

	CFont labelFont;
	labelFont.CreateFont(14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, "Segoe UI");

	CFont cardTitleFont;
	cardTitleFont.CreateFont(14, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, "Segoe UI");
	memDCBackGround.SelectObject(&cardTitleFont);
	memDCBackGround.SetTextColor(RGB(245, 247, 251));
	memDCBackGround.TextOut(108, 381, "SON DUYURU");
	memDCBackGround.TextOut(331, 381, "ETKİNLİK");
	memDCBackGround.TextOut(554, 381, "TOPLULUK");

	memDCBackGround.SelectObject(&labelFont);
	memDCBackGround.SetTextColor(RGB(150, 157, 174));
	memDCBackGround.TextOut(331, 408, "Haftalık etkinlikleri");
	memDCBackGround.TextOut(331, 425, "kaçırma.");
	memDCBackGround.TextOut(554, 408, "Web ve Discord'da");
	memDCBackGround.TextOut(554, 425, "bize katıl.");
	memDCBackGround.SetTextColor(RGB(255, 255, 255));
	memDCBackGround.TextOut(620, 285, "ACE TR");

	
	labelFont.CreateFont(14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, "Segoe UI");
	memDCBackGround.SelectObject(&labelFont);
	memDCBackGround.SetTextColor(RGB(150, 157, 174));
	memDCBackGround.TextOut(885, 236, "KULLANICI ADI");
	memDCBackGround.TextOut(885, 306, "ŞİFRE");
	memDCBackGround.TextOut(910, 392, "Beni hatirla");
	memDCBackGround.TextOut(910, 427, "64-bit istemci");
	memDCBackGround.TextOut(885, 447, "ÇÖZÜNÜRLÜK");
	memDCBackGround.TextOut(910, 512, "Pencere modu");

	memDCBackGround.SetTextColor(RGB(150, 157, 174));
	memDCBackGround.TextOut(82, 512, "Sunucu baglantisi ve gecikme bilgisi");
	memDCBackGround.SetTextColor(RGB(108, 115, 132));
	memDCBackGround.TextOut(82, 580, "Baglanti hazir oldugunda OYNA aktif olur.");

	// Online indicator
	memDCBackGround.SelectObject(&brushOnline);
	memDCBackGround.Ellipse(CRect(865, 173, 875, 183));
	memDCBackGround.SetTextColor(RGB(115, 224, 164));
	memDCBackGround.TextOut(885, 168, "ONLINE OYUNCU");

	memDCBackGround.SelectObject(pOldFont);
	memDCBackGround.SelectObject(pOldBrush);
	memDCBackGround.SelectObject(pOldPen);
	memDCBackGround.SelectObject(pOldBitmapBackGround);

#endif
	
	// end 2008-12-23 by ckPark ŔĎş» ·±ĂÄ







	// ąč°ć Č­¸é ¸¸µé±â ----> End
	///////////////////////////////////////////////////////////////////////////////

	DisableControls();
	SetPrivateIP();

	// timeoutŔĚ ąß»ýÇĎ¸é ÇÁ¸® Ľ­ąöżˇ ż¬°á
	SetTimer(TIMERID_CONNECT_PRESERVER, 200, nullptr);
	SetTimer(TIMERID_SEND_ALIVE_PACKET, 30000, nullptr);
	SetTimer(TIMERID_NETWORK_STATE_CHECK, 1000, nullptr);		// 2007-06-18 by cmkwon, ł×Ć®żöĹ© »óĹÂ ĂĽĹ©

#ifndef _DEBUG
	DeleteFile("AtumLauncher_dbg.exe");
#endif

	ShowWindow(SW_SHOW);
	this->GetPublicLocalIP(m_szLocalIP);		// 2006-05-07 by cmkwon
	return TRUE;  // return TRUE  unless you set the focus to a control

}

BOOL CAtumLauncherDlg::ConnectPreServer()
{
// Pre Server IP Ă˘ ¶çżě±â
//	char preServerIP[SIZE_MAX_IPADDRESS];

// 2009-01-15 by cmkwon, PreServer, DBServer Á¤ş¸ DNS·Î ĽłÁ¤ °ˇ´ÉÇĎ°Ô ĽöÁ¤ - ąöĆŰ Ĺ©Ĺ°°ˇ ´ő ÇĘżä ÇĎ´Ů.
//	char preServerIP[128];
	char preServerIP[1024];		// 2009-01-15 by cmkwon, PreServer, DBServer Á¤ş¸ DNS·Î ĽłÁ¤ °ˇ´ÉÇĎ°Ô ĽöÁ¤ - 
#ifdef _ATUM_DEVELOP
	char ExecutableBinary[256];
	char ExecutePath[256];
	CPreServerIPDlg	dlg(m_szPreServerIPReg, m_szExecutableBinaryReg, m_szExecutePathReg,
						m_szPreServerIPHistoryReg, m_szExecuteBinHistoryReg, m_szExecutePathHistoryReg);
	dlg.m_bWindowMode = ((m_nWindowModeReg==GAME_MODE_WINDOW)?TRUE:FALSE);

// 2007-12-27 by cmkwon, Ŕ©µµżěÁî ¸đµĺ ±â´É Ăß°ˇ - Ăł¸® ÇĘżäÇĎÁö ľĘŔ˝
//	// window modeŔĚ¸é ÇŘ»óµµ combo box¸¦ disable˝ĂĹ´
//	if (m_nWindowModeReg == GAME_MODE_WINDOW)
//	{
//		GetDlgItem(IDC_COMBO_WINDOW_DEGREE_LAUNCHER)->EnableWindow(FALSE);
//	}
//	else
//	{
//		GetDlgItem(IDC_COMBO_WINDOW_DEGREE_LAUNCHER)->EnableWindow();
//	}

	if (m_bShowPreServerIPDlg)
	{
		if (dlg.DoModal() != IDOK)
		{
			return FALSE;
		}
	}

	STRNCPY_MEMSET(preServerIP, dlg.m_preserver_ip, 128);
	m_szPreServerIPReg = dlg.m_preserver_ip;
	STRNCPY_MEMSET(ExecutableBinary, dlg.m_executable_bin, 256);
	m_szExecutableBinaryReg = dlg.m_executable_bin;
	STRNCPY_MEMSET(ExecutePath, dlg.m_execute_path, 256);
	m_szExecutePathReg = dlg.m_execute_path;
	m_szPreServerIPHistoryReg = dlg.m_szPreServerIPHistory;
	m_szExecuteBinHistoryReg = dlg.m_szExecuteBinHistory;
	m_szExecutePathHistoryReg = dlg.m_szExecutePathHistory;
	m_nWindowModeReg = ((dlg.m_bWindowMode==TRUE)?GAME_MODE_WINDOW:GAME_MODE_FULLSCREEN);
	///////////////////////////////////////////////////////////////////////////////
	// 2007-12-27 by cmkwon, Ŕ©µµżěÁî ¸đµĺ ±â´É Ăß°ˇ - Ăß°ˇÇŃ°Í
	if(GAME_MODE_WINDOW == m_nWindowModeReg)
	{
		
		
		
		// 2008-12-17 by ckPark ·Ż˝ĂľĆ ·±ĂÄ
		// ·Ż˝ĂľĆ´Â ĂĽĹ©ąÚ˝ş »óĹÂ¸¦ şŻĽöżˇĽ­ Á÷Á˘ ľňľîżÂ´Ů
#if defined(SERVICE_TYPE_RUSSIAN_SERVER_1)		// ·Ż˝ĂľĆ ·±Ăł ŔÎĹÍĆäŔĚ˝ş ĽöÁ¤
		m_ctrlCheckWindowMode.SetCheck(TRUE);
#else
		this->m_ctlbWindowMode		= TRUE;
#endif
		// end 2008-12-17 by ckPark ·Ż˝ĂľĆ ·±ĂÄ





		this->CheckDlgButton(IDC_CHECK_WINDOWS_MODE, 1);
	}
	((CAtumLauncherApp*)AfxGetApp())->WriteProfile(*this);

	m_bUpdateClientFile = dlg.m_bUpdateClientFile;

#else
	// 2009-01-15 by cmkwon, PreServer, DBServer Á¤ş¸ DNS·Î ĽłÁ¤ °ˇ´ÉÇĎ°Ô ĽöÁ¤ - domain Ŕ¸·Î ĽłÁ¤µÇ¸é ±ćŔĚ°ˇ ±ćľîÁúĽö ŔÖ´Ů.
	//STRNCPY_MEMSET(preServerIP, m_szPreServerIPReg, SIZE_MAX_IPADDRESS);
	STRNCPY_MEMSET(preServerIP, m_szPreServerIPReg, 1024);		// 2009-01-15 by cmkwon, PreServer, DBServer Á¤ş¸ DNS·Î ĽłÁ¤ °ˇ´ÉÇĎ°Ô ĽöÁ¤ - 
	m_szPreServerIPReg = m_szPreServerIPReg;
#endif


	// Make socket instance & connect
	m_pUpdateWinsocket = new CUpdateWinSocket(GetSafeHwnd());
	// 2004-11-19 by cmkwon
	//if (!m_pUpdateWinsocket->Connect(GGetIPByName(preServerIP, preServerIP), PRE_SERVER_PORT))

	int nPreServerPort = PRE_SERVER_PORT;		// 2009-01-30 by cmkwon, ·Ż˝ĂľĆ Innova ·±Ăł ˝Ă˝şĹŰ(ÇÁ·Î˝şĆ®) ĽöÁ¤ - Ć÷Ć®µµ şŻ°á ÇŇ Ľö ŔÖ°Ô ĽöÁ¤

// 2009-04-23 by cmkwon, ·Ż˝ĂľĆ Innova Frost ŔÓ˝Ă Á¦°ĹÇĎ±â - ľĆ·ˇżÍ °°ŔĚ define Ŕ¸·Î Ăł¸® ÇÔ.
//#if defined(SERVICE_TYPE_RUSSIAN_SERVER_1)
#if defined(SERVICE_TYPE_RUSSIAN_SERVER_1) && defined(_USING_INNOVA_FROST_)

// 2009-07-10 by cmkwon, ·Ż˝ĂľĆ Frost ˝Ĺ±Ô Lib·Î ĽöÁ¤ - 
// 	// 2009-01-30 by cmkwon, ·Ż˝ĂľĆ Innova ·±Ăł ˝Ă˝şĹŰ(ÇÁ·Î˝şĆ®) ĽöÁ¤ - ReadLoginAddr() ČŁĂâ
// 	char ip[AYYO_SECURITY_BUF_SIZE] = {0};
// 	int port = 0;
// 	try
// 	{
// 		ReadLoginAddr(ip, port);
// 	}
// 	catch (std::runtime_error runErr)
// 	{
// 		CString strErr;
// 		strErr.Format("[Error] call ReadLoginAddr() error !! %s", runErr.what());
// 		AfxMessageBox(strErr);
// 		return FALSE;
// 	}
// 	catch(...)
// 	{
// 		CString strErr;
// 		strErr.Format("[Error] call ReadLoginAddr() error !! unknown error(%d)", GetLastError());
// 		AfxMessageBox(strErr);
// 		return FALSE;
// 	}	
// 	STRNCPY_MEMSET(preServerIP, ip, 128);	// IP şŻ°ć
// 	nPreServerPort		= port;				// Port şŻ°ć
	///////////////////////////////////////////////////////////////////////////////
	// 2009-07-10 by cmkwon, ·Ż˝ĂľĆ Frost ˝Ĺ±Ô Lib·Î ĽöÁ¤ - 
	//DbgOut("[TEMP] 090710 frostInitialize 2-1 \r\n");
	frostInitialize(".\\Frost\\gameShieldDll.dll");
	//DbgOut("[TEMP] 090710 frostInitialize 2-2 \r\n");
#endif // END - #if defined(SERVICE_TYPE_RUSSIAN_SERVER_1)

// 2009-01-30 by cmkwon, ·Ż˝ĂľĆ Innova ·±Ăł ˝Ă˝şĹŰ(ÇÁ·Î˝şĆ®) ĽöÁ¤ - ľĆ·ˇżÍ °°ŔĚ ÁöÁ¤µČ Ć÷Ć®·Î ż¬°á ˝Ăµµ
//	if (!m_pUpdateWinsocket->Connect(preServerIP, PRE_SERVER_PORT))
	if (!m_pUpdateWinsocket->Connect(preServerIP, nPreServerPort))
	{
 		int err = GetLastError();
//		AtumMessageBox("Ľ­ąö°ˇ ˝ÇÇŕµÇľî ŔÖÁö ľĘ°ĹłŞ Áˇ°Ë ÁßŔÔ´Ď´Ů.\n\nĹ×˝şĆ® ˝Ă°ŁŔş ÁÖ¸» 24˝Ă°Ł,\n\nĆňŔĎ żŔŔü10˝ĂşÎĹÍ żŔČÄ24˝Ă±îÁöŔÔ´Ď´Ů.");
 		AtumMessageBox(STRERR_S_ATUMLAUNCHER_0004);
		SAFE_DELETE(m_pUpdateWinsocket);
		EndDialog(-1);
		return FALSE;
	}
	m_PreSD = m_pUpdateWinsocket->GetSocketDescriptor();

	return TRUE;
}

void CAtumLauncherDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if(SC_MAXIMIZE == nID)
		return;
	
	
	CDialog::OnSysCommand(nID, lParam);	
}

void CAtumLauncherDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this);
		SendMessage(WM_ICONERASEBKGND, (WPARAM)dc.GetSafeHdc(), 0);
		CRect rect; GetClientRect(&rect);
		dc.DrawIcon((rect.Width() - GetSystemMetrics(SM_CXICON)) / 2,
			(rect.Height() - GetSystemMetrics(SM_CYICON)) / 2, m_hIcon);
		return;
	}

	CPaintDC dc(this);
	CDC mem;
	BITMAP bm = {0};
	mem.CreateCompatibleDC(&dc);
	CBitmap* oldBmp = mem.SelectObject(&m_BackGround);
	m_BackGround.GetObject(sizeof(BITMAP), &bm);
	dc.SetStretchBltMode(HALFTONE);
	dc.StretchBlt(0, 0, EXE2_LAUNCHER_BG_SIZE_X, EXE2_LAUNCHER_BG_SIZE_Y,
		&mem, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
	mem.SelectObject(oldBmp);

	// AceTR dark glass frame.
	CBrush shell(RGB(7, 14, 24));
	CBrush panel(RGB(10, 22, 34));
	CBrush panelLight(RGB(15, 31, 46));
	CBrush card(RGB(18, 37, 54));
	CBrush cyan(RGB(47, 211, 255));
	CBrush green(RGB(67, 224, 132));
	CPen framePen(PS_SOLID, 1, RGB(32, 103, 137));
	CPen cyanPen(PS_SOLID, 2, RGB(47, 211, 255));

	CBrush* oldBrush=dc.SelectObject(&shell);
	CPen* oldPen=dc.SelectObject(&framePen);
	dc.RoundRect(CRect(8, 8, 1272, 712), CPoint(14, 14));

	// Header.
	dc.SelectObject(&panel);
	dc.RoundRect(CRect(18, 18, 1258, 108), CPoint(12, 12));
	dc.FillSolidRect(CRect(18, 106, 1258, 108), RGB(26, 123, 158));

	// Main columns.
	dc.RoundRect(CRect(18, 124, 238, 642), CPoint(12, 12));
	dc.SelectObject(&panelLight);
	dc.RoundRect(CRect(250, 124, 950, 642), CPoint(12, 12));
	dc.SelectObject(&panel);
	dc.RoundRect(CRect(962, 124, 1258, 642), CPoint(12, 12));

	// Bottom patch strip.
	dc.SelectObject(&panel);
	dc.RoundRect(CRect(18, 650, 1258, 704), CPoint(10, 10));

	dc.SetBkMode(TRANSPARENT);
	CFont logoFont, logoSubFont, navFont, sectionFont, heroFont, bodyFont, smallFont, tinyFont;
	logoFont.CreateFont(34,0,0,0,FW_BOLD,FALSE,FALSE,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH|FF_SWISS,"Segoe UI");
	logoSubFont.CreateFont(14,0,0,0,FW_SEMIBOLD,FALSE,FALSE,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH|FF_SWISS,"Segoe UI");
	navFont.CreateFont(18,0,0,0,FW_SEMIBOLD,FALSE,FALSE,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH|FF_SWISS,"Segoe UI");
	sectionFont.CreateFont(16,0,0,0,FW_BOLD,FALSE,FALSE,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH|FF_SWISS,"Segoe UI");
	heroFont.CreateFont(29,0,0,0,FW_BOLD,FALSE,FALSE,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH|FF_SWISS,"Segoe UI");
	bodyFont.CreateFont(15,0,0,0,FW_NORMAL,FALSE,FALSE,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH|FF_SWISS,"Segoe UI");
	smallFont.CreateFont(13,0,0,0,FW_SEMIBOLD,FALSE,FALSE,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH|FF_SWISS,"Segoe UI");
	tinyFont.CreateFont(12,0,0,0,FW_NORMAL,FALSE,FALSE,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH|FF_SWISS,"Segoe UI");

	CFont* oldFont=dc.SelectObject(&logoFont);
	dc.SetTextColor(RGB(238,250,255));
	dc.TextOut(38,32,"ACE TR");
	dc.SelectObject(&logoSubFont);
	dc.SetTextColor(RGB(64,214,255));
	dc.TextOut(40,74,"ACE ONLINE TURKIYE");

	// Decorative top status line.
	dc.SelectObject(&smallFont);
	dc.SetTextColor(RGB(126,151,169));
	dc.TextOut(780,47,"LAUNCHER");
	dc.SetTextColor(RGB(67,224,132));
	dc.TextOut(857,47,"HAZIR");
	dc.SetTextColor(RGB(126,151,169));
	dc.TextOut(922,47,"|  SURUM 1.0");

	// Navigation.
	struct NavItem { CRect r; LPCSTR text; int id; };
	NavItem navItems[] = {
		{ACETR_NAV_HOME_RECT,"ANA SAYFA",1},
		{ACETR_NAV_NEWS_RECT,"HABERLER",2},
		{ACETR_NAV_EVENTS_RECT,"SIRALAMA",3},
		{ACETR_NAV_WEB_RECT,"MARKET",4},
		{ACETR_NAV_DISCORD_RECT,"TOPLULUK",5}
	};
	dc.SelectObject(&navFont);
	for(int i=0;i<5;++i)
	{
		const bool hot=(m_nModernNavHover==navItems[i].id);
		const bool active=(m_nLauncherMainPage==(navItems[i].id-1));
		CRect r=navItems[i].r;
		if(hot||active)
		{
			dc.FillSolidRect(r,RGB(15,55,76));
			dc.FillSolidRect(CRect(r.left,r.top,r.left+4,r.bottom),RGB(47,211,255));
		}
		dc.SetTextColor((hot||active)?RGB(244,252,255):RGB(162,187,201));
		CRect textRect=r; textRect.left+=24;
		dc.DrawText(navItems[i].text,textRect,DT_LEFT|DT_VCENTER|DT_SINGLELINE);
	}

	// Left utility card.
	dc.SelectObject(&card);
	dc.RoundRect(CRect(34,475,222,615),CPoint(10,10));
	dc.SelectObject(&sectionFont);
	dc.SetTextColor(RGB(70,214,255));
	dc.TextOut(52,492,"ACE TR");
	dc.SelectObject(&tinyFont);
	dc.SetTextColor(RGB(150,173,188));
	dc.TextOut(52,520,"Turkiye sunucusu");
	dc.TextOut(52,542,"Modern launcher");
	dc.TextOut(52,564,"Otomatik guncelleme");
	dc.SetTextColor(RGB(67,224,132));
	dc.TextOut(52,588,"Sistem aktif");

	// Featured news artwork container.
	dc.SelectObject(&card);
	dc.RoundRect(CRect(275,154,925,484),CPoint(12,12));
	// Header accent and fake visual depth strips, so the layout looks finished even
	// before the final bitmap pack replaces the legacy background.
	dc.FillSolidRect(CRect(275,154,925,158),RGB(47,211,255));
	dc.FillSolidRect(CRect(292,176,908,330),RGB(10,43,62));
	dc.FillSolidRect(CRect(310,194,892,314),RGB(12,56,78));
	dc.SelectObject(&sectionFont);
	dc.SetTextColor(RGB(76,222,255));
	dc.TextOut(300,170,"ONE CIKAN HABER");
	dc.SelectObject(&heroFont);
	dc.SetTextColor(RGB(244,250,253));
	dc.TextOut(305,352,"YENI SEZON BASLIYOR!");
	dc.SelectObject(&bodyFont);
	dc.SetTextColor(RGB(190,207,219));
	dc.TextOut(307,397,"Daha buyuk savaslar, yeni etkinlikler ve surpriz oduller seni bekliyor.");
	dc.SetTextColor(RGB(82,217,255));
	dc.TextOut(307,433,"DETAYLARI GOR  >");

	// Three compact story cards.
	for(int i=0;i<3;++i)
	{
		CRect c(275+i*217,502,478+i*217,615);
		dc.SelectObject(&panel);
		dc.RoundRect(c,CPoint(10,10));
		dc.FillSolidRect(CRect(c.left,c.top,c.right,c.top+3), i==0?RGB(47,211,255):RGB(40,105,133));
	}
	dc.SelectObject(&smallFont);
	dc.SetTextColor(RGB(228,239,245));
	dc.TextOut(292,523,"SEZON 1");
	dc.TextOut(509,523,"ETKINLIK");
	dc.TextOut(726,523,"GUNCELLEME");
	dc.SelectObject(&tinyFont);
	dc.SetTextColor(RGB(142,166,181));
	dc.TextOut(292,551,"Yeni oduller ve");
	dc.TextOut(292,570,"rekabet seni bekliyor");
	dc.TextOut(509,551,"Haftalik etkinlik");
	dc.TextOut(509,570,"takvimi yayinlandi");
	dc.TextOut(726,551,"Launcher ve istemci");
	dc.TextOut(726,570,"dosyalari guncel");

	// Right server panel.
	dc.SelectObject(&sectionFont);
	dc.SetTextColor(RGB(131,158,175));
	dc.TextOut(986,150,"SUNUCU DURUMU");
	dc.SelectObject(&green);
	dc.Ellipse(CRect(987,181,999,193));
	dc.SelectObject(&sectionFont);
	dc.SetTextColor(RGB(67,224,132));
	dc.TextOut(1008,178,"CEVRIMICI");

	dc.SelectObject(&tinyFont);
	dc.SetTextColor(RGB(128,151,166));
	dc.TextOut(986,231,"BAGLANTI");
	const int pingMs=NTGetPingAverageTime();
	CString pingText;
	if(pingMs>0) pingText.Format("%d ms",pingMs); else pingText="olculuyor...";
	dc.SetTextColor(RGB(225,239,246));
	dc.TextOut(1080,231,pingText);

	dc.SelectObject(&sectionFont);
	dc.SetTextColor(RGB(74,215,255));
	dc.TextOut(986,214,"HESAP GIRISI");
	dc.SelectObject(&card);
	dc.RoundRect(CRect(982,238,1238,386),CPoint(8,8));
	dc.SelectObject(&bodyFont);
	dc.SetTextColor(RGB(224,239,246));
	dc.TextOut(1000,246,"Kullanici Adi");
	dc.TextOut(1000,308,"Sifre");

	dc.SelectObject(&tinyFont);
	dc.SetTextColor(RGB(132,155,170));
	dc.TextOut(986,410,"SUNUCU");
	dc.SetTextColor(RGB(218,235,243));
	dc.TextOut(1080,410,"AceTR");
	dc.SetTextColor(RGB(132,155,170));
	dc.TextOut(986,437,"OYUN DURUMU");
	dc.SetTextColor(RGB(218,235,243));
	dc.TextOut(1080,437,"Hazir");
	dc.SetTextColor(RGB(132,155,170));
	dc.TextOut(986,461,"DOSYALAR");
	dc.SetTextColor(RGB(218,235,243));
	dc.TextOut(1080,461,"Kontrol edildi");
	dc.SetTextColor(RGB(132,155,170));
	dc.TextOut(986,485,"BOLGE");
	dc.SetTextColor(RGB(218,235,243));
	dc.TextOut(1080,485,"Turkiye");

	// Text directly above the real owner-draw OYNA button.
	dc.SelectObject(&tinyFont);
	dc.SetTextColor(RGB(111,145,164));
	dc.DrawText("GIRIS SONRASI OYNA BUTONUNA DONUSUR",CRect(982,514,1238,536),DT_CENTER|DT_VCENTER|DT_SINGLELINE);

	// Patch strip.
	dc.SelectObject(&smallFont);
	dc.SetTextColor(RGB(72,216,255));
	dc.TextOut(34,664,"GUNCELLEME");
	dc.SelectObject(&tinyFont);
	dc.SetTextColor(RGB(139,162,177));
	dc.TextOut(34,684,"AceTR dosyalari otomatik kontrol edilir");

	dc.SelectObject(oldFont);
	dc.SelectObject(oldPen);
	dc.SelectObject(oldBrush);
}

HCURSOR CAtumLauncherDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}


struct sort_MEX_SERVER_GROUP_INFO_FOR_LAUNCHER_By_Crowdedness
{
	bool operator()(MEX_SERVER_GROUP_INFO_FOR_LAUNCHER op1, MEX_SERVER_GROUP_INFO_FOR_LAUNCHER op2)
	{
		return op1.Crowdedness < op2.Crowdedness;				// żŔ¸ĄÂ÷Ľř Á¤·Ä
	}
};
#ifdef _INET_MAC_ADDRESS_CHECKER
void CAtumLauncherDlg::PrintMACaddress(unsigned char MACData[])
{
	char szTemp[256];
	sprintf(szTemp, "%02X-%02X-%02X-%02X-%02X-%02X",
		MACData[0], MACData[1], MACData[2], MACData[3], MACData[4], MACData[5]);

#ifdef _DEBUG
	printf(szTemp);
#endif

	string szTempMAC(szTemp);

	m_szTempMAC = szTempMAC;
}
void CAtumLauncherDlg::GetMACaddress(void)
{

	return;
	//unsigned char MACData[8];

	//WKSTA_TRANSPORT_INFO_0 *pwkti;
	//DWORD dwEntriesRead;
	//DWORD dwTotalEntries;
	//BYTE *pbBuffer;

	//// Get MAC address via NetBios's enumerate function
	//NET_API_STATUS dwStatus = NetWkstaTransportEnum(nullptr, 0, &pbBuffer, MAX_PREFERRED_LENGTH, &dwEntriesRead, &dwTotalEntries, nullptr);
	//assert(dwStatus == NERR_Success);

	//pwkti = (WKSTA_TRANSPORT_INFO_0 *)pbBuffer;

	//for (DWORD i = 1; i< dwEntriesRead; i++)
	//{
	//	swscanf((wchar_t *)pwkti[i].wkti0_transport_address, L"%2hx%2hx%2hx%2hx%2hx%2hx",
	//		&MACData[0], &MACData[1], &MACData[2], &MACData[3], &MACData[4], &MACData[5]);
	//	PrintMACaddress(MACData);
	//}

	//dwStatus = NetApiBufferFree(pbBuffer);
	//assert(dwStatus == NERR_Success);
}
void CAtumLauncherDlg::GetMACaddress2(void)
{
	IP_ADAPTER_INFO AdapterInfo[16];
	DWORD dwBufLen = sizeof(AdapterInfo);

	DWORD dwStatus = GetAdaptersInfo(AdapterInfo, &dwBufLen);
	assert(dwStatus == ERROR_SUCCESS);

	PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;
	do {
		PrintMACaddress(pAdapterInfo->Address);
		pAdapterInfo = pAdapterInfo->Next;
	} while (pAdapterInfo);
}
void CAtumLauncherDlg::GetMACaddress3(void)
{
	//simplest method but not rly so bad
	unsigned char MACData[6];

	UUID uuid;
	UuidCreateSequential(&uuid);

	for (int i = 2; i<8; i++)
		MACData[i - 2] = uuid.Data4[i];

	PrintMACaddress(MACData);
}
#endif

LONG CAtumLauncherDlg::OnSocketNotify(WPARAM wParam, LPARAM lParam)
{
	if(nullptr == m_pUpdateWinsocket)
	{
		return 0L;
	}

	switch(LOWORD(wParam))
	{
	case CUpdateWinSocket::WS_ERROR:
		{
		}
		break;
	case CUpdateWinSocket::WS_CONNECTED:
		{
			if (HIWORD(wParam) == TRUE)
			{
				m_pUpdateWinsocket->WriteMessageType(T_PC_CONNECT_GET_SERVER_GROUP_LIST);

				INIT_MSG_WITH_BUFFER(MSG_PC_CONNECT_SINGLE_FILE_VERSION_CHECK, T_PC_CONNECT_SINGLE_FILE_VERSION_CHECK, msgSFVer, msgSFVerBuf);
				// delete file list version
				msgSFVer->DeleteFileListVersion[0] = m_CurrentDelFileListVersion.GetVersion()[0];
				msgSFVer->DeleteFileListVersion[1] = m_CurrentDelFileListVersion.GetVersion()[1];
				msgSFVer->DeleteFileListVersion[2] = m_CurrentDelFileListVersion.GetVersion()[2];
				msgSFVer->DeleteFileListVersion[3] = m_CurrentDelFileListVersion.GetVersion()[3];
				// notice version
				msgSFVer->NoticeVersion[0] = m_CurrentNoticeVersion.GetVersion()[0];
				msgSFVer->NoticeVersion[1] = m_CurrentNoticeVersion.GetVersion()[1];
				msgSFVer->NoticeVersion[2] = m_CurrentNoticeVersion.GetVersion()[2];
				msgSFVer->NoticeVersion[3] = m_CurrentNoticeVersion.GetVersion()[3];

				m_pUpdateWinsocket->Write((char*)msgSFVerBuf, MSG_SIZE(MSG_PC_CONNECT_SINGLE_FILE_VERSION_CHECK));
#ifdef _INET_MAC_ADDRESS_CHECKER
				//GetMACaddress();
				HW_PROFILE_INFO hwProfileInfo;
						
				if (GetCurrentHwProfile(&hwProfileInfo))
				{
					INIT_MSG_WITH_BUFFER(MSG_PC_CONNECT_MAC_ADDR_CHK, T_PC_CONNECT_SEND_GET_BLOCKED_MAC_ADDR, msgMACAdd, msgMACAddBuf);
					STRNCPY_MEMSET(msgMACAdd->MACAddr, hwProfileInfo.szHwProfileGuid, 50);
					m_pUpdateWinsocket->Write((char*)msgMACAddBuf, MSG_SIZE(MSG_PC_CONNECT_MAC_ADDR_CHK));
				}
				else
				{
					GetMACaddress2();
					if (!m_szTempMAC.empty())
					{
						INIT_MSG_WITH_BUFFER(MSG_PC_CONNECT_MAC_ADDR_CHK, T_PC_CONNECT_SEND_GET_BLOCKED_MAC_ADDR, msgMACAdd, msgMACAddBuf);
						STRNCPY_MEMSET(msgMACAdd->MACAddr, (LPCSTR)m_szTempMAC.c_str(), 50);
						m_pUpdateWinsocket->Write((char*)msgMACAddBuf, MSG_SIZE(MSG_PC_CONNECT_MAC_ADDR_CHK));
					}
					else
					{
						GetMACaddress3();
						if (!m_szTempMAC.empty())
						{
							INIT_MSG_WITH_BUFFER(MSG_PC_CONNECT_MAC_ADDR_CHK, T_PC_CONNECT_SEND_GET_BLOCKED_MAC_ADDR, msgMACAdd, msgMACAddBuf);
							STRNCPY_MEMSET(msgMACAdd->MACAddr, (LPCSTR)m_szTempMAC.c_str(), 50);
							m_pUpdateWinsocket->Write((char*)msgMACAddBuf, MSG_SIZE(MSG_PC_CONNECT_MAC_ADDR_CHK));
						}
						else
						{
#ifdef _DEBUG
							printf("cannot get MAC totally! -.-");
#endif
						}
					}
					//MessageBox("Cannot receive MAC adderess. Closing application!", "Error");
					//exit(0);

				}
#endif
			}
			else
			{
//				AtumMessageBox("Ľ­ąö°ˇ ˝ÇÇŕµÇľî ŔÖÁö ľĘ°ĹłŞ Áˇ°Ë ÁßŔÔ´Ď´Ů.\n\nĹ×˝şĆ® ˝Ă°ŁŔş ÁÖ¸» 24˝Ă°Ł,\n\nĆňŔĎ żŔŔü10˝ĂşÎĹÍ żŔČÄ24˝Ă±îÁöŔÔ´Ď´Ů.");
				AtumMessageBox(STRERR_S_ATUMLAUNCHER_0005);
				EndDialog(-1);
			}
		}
		break;
	case CUpdateWinSocket::WS_CLOSED:
		{
			SAFE_DELETE(m_pUpdateWinsocket);
#ifdef _DEBUG
			AtumMessageBox(STRERR_S_ATUMLAUNCHER_0006);
#endif

			OnCancel();
		}
		break;
	case CUpdateWinSocket::WS_RECEIVED:
		{
			char * pPacket = nullptr;
			int len,nType;
			m_pUpdateWinsocket->Read(&pPacket, len);

			if(pPacket)
			{
				nType = 0;
				memcpy(&nType, pPacket, SIZE_FIELD_TYPE_HEADER);

				switch(nType)
				{
#ifdef _INET_MAC_ADDRESS_CHECKER
				case T_PC_CONNECT_SEND_GET_BLOCKED_MAC_ADDR_BLOCK:
				{
					MessageBox("Sorry, you're not allowed to connect our server (Permanent BAN!)\r\nOldSchoolRivals Team", "Information", MB_OK);
#ifdef _DEBUG
					printf("[MAC] T_PC_CONNECT_SEND_GET_BLOCKED_MAC_ADDR_BLOCK ==> You're NOT allowed to connect server!");
#endif
					exit(0);
				}
				break;
				case T_PC_CONNECT_SEND_GET_BLOCKED_MAC_ADDR_OK:
				{
#ifdef _DEBUG
					printf("[MAC] T_PC_CONNECT_SEND_GET_BLOCKED_MAC_ADDR_OK ==> You're allowed to connect server!");
#endif
				}
				break;
#endif
				case T_PC_CONNECT_SINGLE_FILE_VERSION_CHECK_OK:
					{
// 2006-05-03 by cmkwon, °řÁö»çÇ×Ŕş ·±ĂłŔÇ ŔĄČ­¸éżˇĽ­ Ăł¸®ÇŃ´Ů.
// 						// Notice ·Îµů
 						if (!ReadNoticeFile())
 						{
							SetNoticeTxt("Welcome to OldSchoolRivals ! No new notifications to be displayed!");
// 							AtumMessageBox(STRERR_S_ATUMLAUNCHER_0007);
// 							EndDialog(-1);
// 							return FALSE;
 						}
//						DbgOut("	2007-07-13 by cmkwon, T_PC_CONNECT_SINGLE_FILE_VERSION_CHECK_OK\r\n");	// 2007-07-13 by cmkwon, Ĺ×˝şĆ® 

						/////////////////////////////////////////////
						// Send Client Version
						INIT_MSG_WITH_BUFFER(MSG_PC_CONNECT_VERSION, T_PC_CONNECT_VERSION, msgVersion, msgVersionBuff);
						msgVersion->ClientVersion[0] = m_CurrentVersion.GetVersion()[0];
						msgVersion->ClientVersion[1] = m_CurrentVersion.GetVersion()[1];
						msgVersion->ClientVersion[2] = m_CurrentVersion.GetVersion()[2];
						msgVersion->ClientVersion[3] = m_CurrentVersion.GetVersion()[3];
						m_pUpdateWinsocket->Write(msgVersionBuff, MSG_SIZE(MSG_PC_CONNECT_VERSION));

						// ŔĎ´Ü ÂďľîµÎ±â
						SetProgressGroupText(STRMSG_S_ATUMLAUNCHER_0000);
					}
					break;
				case T_PC_CONNECT_SINGLE_FILE_UPDATE_INFO:
					{
						MSG_PC_CONNECT_SINGLE_FILE_UPDATE_INFO *pMsgUpdate
							= (MSG_PC_CONNECT_SINGLE_FILE_UPDATE_INFO*)(pPacket + SIZE_FIELD_TYPE_HEADER);
						// ¸±¸®Áîżë
						VersionInfo TmpDelVerion(pMsgUpdate->NewDeleteFileListVersion);
						if (TmpDelVerion != m_CurrentDelFileListVersion)
						{
							// delete file Ăł¸®
							if (m_bUpdateClientFile)
							{
								///////////////////////////////////////////////////////////////////////////////
								// 2007-01-08 by cmkwon, Http Update Ăł¸® Ăß°ˇ
								switch(pMsgUpdate->nAutoUpdateServerType)
								{
								case AU_SERVER_TYPE_HTTP:
									{
										ProcessDeleteFileListByHttp(pMsgUpdate);
									}
									break;
								default:
									{
										if (!ProcessDeleteFileList(pMsgUpdate))
										{
											// ąćČ­ş® µîŔÇ ŔĚŔŻ·Î FTPżˇ Á˘±ŮÇĎÁö ¸řÇĎ´Â ŔŻŔú°ˇ Á¸ŔçÇŇ Ľö ŔÖŔ¸ąÇ·Î,
											// ˝ÇĆĐÇŘµµ ±×łÉ łŃ±ä´Ů. 20041030, kelovon
											//OnCancel();
										}
									}
								}
							}
							m_CurrentDelFileListVersion = TmpDelVerion;
						}
// 2006-05-03 by cmkwon, °řÁö»çÇ×Ŕş ·±ĂłŔÇ ŔĄČ­¸éżˇĽ­ Ăł¸®ÇŃ´Ů.
 						VersionInfo TmpNoticeVerion(pMsgUpdate->NewNoticeVersion);
						if (TmpNoticeVerion != m_CurrentNoticeVersion)
 						{
 							// notice.txt¸¦ ´Ůżî·ÎµĺąŢŔ˝
 							if (!ProcessNoticeFile(pMsgUpdate))
 							{
								SetNoticeTxt("Welcome to OldSchoolRivals ! No new notifications to be displayed.");
 								// ąćČ­ş® µîŔÇ ŔĚŔŻ·Î FTPżˇ Á˘±ŮÇĎÁö ¸řÇĎ´Â ŔŻŔú°ˇ Á¸ŔçÇŇ Ľö ŔÖŔ¸ąÇ·Î,
 								// ˝ÇĆĐÇŘµµ ±×łÉ łŃ±ä´Ů. 20041030, kelovon
 								//OnCancel();
 							}
 							m_CurrentNoticeVersion = TmpNoticeVerion;
 						}
						
						// ·ąÁö˝şĆ®¸®żˇ ľ˛±â
						((CAtumLauncherApp*)AfxGetApp())->WriteProfile(*this);

// 2006-05-03 by cmkwon, °řÁö»çÇ×Ŕş ·±ĂłŔÇ ŔĄČ­¸éżˇĽ­ Ăł¸®ÇŃ´Ů.
// 						// Notice ·Îµů
						if (!ReadNoticeFile())
 						{
							SetNoticeTxt("Welcome to OldSchoolRivals! No new notifications to be displayed.");
 							//AtumMessageBox(STRERR_S_ATUMLAUNCHER_0007);
// 							EndDialog(-1);
// 							return FALSE;
 						}

//						DbgOut("	2007-07-13 by cmkwon, T_PC_CONNECT_SINGLE_FILE_UPDATE_INFO\r\n");	// 2007-07-13 by cmkwon, Ĺ×˝şĆ® 
						/////////////////////////////////////////////
						// Send Client Version
						INIT_MSG_WITH_BUFFER(MSG_PC_CONNECT_VERSION, T_PC_CONNECT_VERSION, msgVersion, msgVersionBuff);
						msgVersion->ClientVersion[0] = m_CurrentVersion.GetVersion()[0];
						msgVersion->ClientVersion[1] = m_CurrentVersion.GetVersion()[1];
						msgVersion->ClientVersion[2] = m_CurrentVersion.GetVersion()[2];
						msgVersion->ClientVersion[3] = m_CurrentVersion.GetVersion()[3];
						m_pUpdateWinsocket->Write(msgVersionBuff, MSG_SIZE(MSG_PC_CONNECT_VERSION));
						// ŔĎ´Ü ÂďľîµÎ±â
						SetProgressGroupText(STRMSG_S_ATUMLAUNCHER_0000);
					}
					break;
				case T_PC_CONNECT_VERSION_OK:
					{
//						DbgOut("	2007-07-13 by cmkwon, T_PC_CONNECT_VERSION_OK\r\n");	// 2007-07-13 by cmkwon, Ĺ×˝şĆ® 
						m_bProcessingVersionUpdate = TRUE;
						EnableControls();
						NTStartNetworkCheck();		// 2007-06-18 by cmkwon, ł×Ć®żöĹ© »óĹÂ ĂĽĹ©



#ifndef _DEBUG
						DeleteFile("AtumLauncher_dbg.exe");
#endif

#if defined(SERVICE_TYPE_JAPANESE_SERVER_1)
						// 2008-12-18 by cmkwon, ŔĎş» Arario ·±Ăł ĽöÁ¤ - Á˘ĽÓ ąöĆ° Ăł¸®
						this->OnOk();
#endif
					}
					break;
				case T_PC_CONNECT_REINSTALL_CLIENT:
					{
						m_bProcessingVersionUpdate = TRUE;
						MSG_PC_CONNECT_REINSTALL_CLIENT *pReinstall
							= (MSG_PC_CONNECT_REINSTALL_CLIENT*)(pPacket + SIZE_FIELD_TYPE_HEADER);
						VersionInfo currentVersion;
						currentVersion.SetVersion(pReinstall->LatestVersion[0]
													, pReinstall->LatestVersion[1]
													, pReinstall->LatestVersion[2]
													, pReinstall->LatestVersion[3]);

#ifdef _ATUM_DEVELOP
						if (!m_bUpdateClientFile)
						{

							/////////////////////////////////////////////
							// Send Client Version
							INIT_MSG_WITH_BUFFER(MSG_PC_CONNECT_VERSION, T_PC_CONNECT_VERSION, msgVersion, msgVersionBuff);
							msgVersion->ClientVersion[0] = pReinstall->LatestVersion[0];
							msgVersion->ClientVersion[1] = pReinstall->LatestVersion[1];
							msgVersion->ClientVersion[2] = pReinstall->LatestVersion[2];
							msgVersion->ClientVersion[3] = pReinstall->LatestVersion[3];
							m_pUpdateWinsocket->Write(msgVersionBuff, MSG_SIZE(MSG_PC_CONNECT_VERSION));
							EnableControls();
							NTStartNetworkCheck();		// 2007-06-18 by cmkwon, ł×Ć®żöĹ© »óĹÂ ĂĽĹ©
							SetProgressGroupText("update completed");
							break;
						}
#endif
						CString tmpMsg;

						tmpMsg.Format("Client need to be reinstalled\nVisit our website for download links:%s\nNewest version:", STRMSG_S_GAMEHOMEPAGE_DOMAIN);
						//tmpMsg.Format("Client need to be reinstalled or repaired.\n Automatic repair tool would be started!\n Visit our website for more info:%s\nNewest version:", STRMSG_S_GAMEHOMEPAGE_DOMAIN);
						tmpMsg += currentVersion.GetVersionString();
						AtumMessageBox(tmpMsg);
						/*
							if (WinExec("ClientRepairTool.exe", SW_SHOW) == ERROR_FILE_NOT_FOUND)
							{
								MessageBox("The ClientRepairTool was not found, it will be downloaded now.", "OldSchoolRivals Repair Client");
								HRESULT hr = URLDownloadToFile(nullptr, _T("https://patch.oldschoolrivals.com/fullclient/ClientRepairTool.exe"), _T("ClientRepairTool.exe"), 0, nullptr);
								WinExec("ClientRepairTool.exe", SW_SHOW);
							}*/
// 2005-09-30 by cmkwon
//						string tmpMsg = STRERR_S_ATUMLAUNCHER_0008;
//						tmpMsg += currentVersion.GetVersionString();
//						AtumMessageBox(tmpMsg.c_str());
						OnCancel();
					}
					break;
				case T_PC_CONNECT_UPDATE_INFO:
					{
						MSG_PC_CONNECT_UPDATE_INFO *pMsgUpdateInfo
							= (MSG_PC_CONNECT_UPDATE_INFO*)(pPacket + SIZE_FIELD_TYPE_HEADER);

#ifdef _ATUM_DEVELOP
						if (!m_bUpdateClientFile)
						{
							/////////////////////////////////////////////
							// Send Client Version
							INIT_MSG_WITH_BUFFER(MSG_PC_CONNECT_VERSION, T_PC_CONNECT_VERSION, msgVersion, msgVersionBuff);
							msgVersion->ClientVersion[0] = pMsgUpdateInfo->UpdateVersion[0];
							msgVersion->ClientVersion[1] = pMsgUpdateInfo->UpdateVersion[1];
							msgVersion->ClientVersion[2] = pMsgUpdateInfo->UpdateVersion[2];
							msgVersion->ClientVersion[3] = pMsgUpdateInfo->UpdateVersion[3];
							m_pUpdateWinsocket->Write(msgVersionBuff, MSG_SIZE(MSG_PC_CONNECT_VERSION));
							EnableControls();
							NTStartNetworkCheck();		// 2007-06-18 by cmkwon, ł×Ć®żöĹ© »óĹÂ ĂĽĹ©
							SetProgressGroupText(STRMSG_S_ATUMLAUNCHER_0000);
							break;
						}
#endif
						ptmpMsg = pMsgUpdateInfo;
						// download update file
						switch(pMsgUpdateInfo->nAutoUpdateServerType)
						{
						case AU_SERVER_TYPE_HTTP:
							{
								if(FALSE == DownloadUpdateFileByHttp(pMsgUpdateInfo))
								{
									AtumMessageBox("Auto update failed. Remember to run game as administrator! \r\n Code for support: (0x01200)","OSR Launcher",MB_OK);
									/*if (WinExec("ClientRepairTool.exe", SW_SHOW) == ERROR_FILE_NOT_FOUND)
									{
										MessageBox("The ClientRepairTool was not found, it will be downloaded now.", "OldSchoolRivals Repair Client");
										HRESULT hr = URLDownloadToFile(NULL, _T("https://patch.oldschoolrivals.com/fullclient/ClientRepairTool.exe"), _T("ClientRepairTool.exe"), 0, NULL);
										WinExec("ClientRepairTool.exe", SW_SHOW);
									}*/
									//ShellExecute(NULL, "open", STRMSG_S_GAMEHOMEPAGE_DOMAIN, NULL, NULL, SW_SHOWNORMAL);
									exit(0);
								}
							}
							break;
						default:
							{
								if(FALSE == DownloadUpdateFile(pMsgUpdateInfo))
								{
									AtumMessageBox("Auto update failed. Remember to run game as administrator! \r\n Code for support: (0x01400)", "OSR Launcher", MB_OK);
								/*	if (WinExec("ClientRepairTool.exe", SW_SHOW) == ERROR_FILE_NOT_FOUND)
									{
										MessageBox("The ClientRepairTool was not found, it will be downloaded now.", "OldSchoolRivals Repair Client");
										HRESULT hr = URLDownloadToFile(NULL, _T("https://patch.oldschoolrivals.com/fullclient/ClientRepairTool.exe"), _T("ClientRepairTool.exe"), 0, NULL);
										WinExec("ClientRepairTool.exe", SW_SHOW);
									}*/
									//ShellExecute(NULL, "open", STRMSG_S_GAMEHOMEPAGE_DOMAIN, NULL, NULL, SW_SHOWNORMAL);
									exit(0);
								}
							}
						}
						m_msg_PC_CONNECT_UPDATE_INFO = *pMsgUpdateInfo;

// 2004-07-06, cmkwon OnUpdateFileDownloadOK()ÇÔĽö·Î ŔĚµż						
//						// extract update file
//						ExtractUpdateFile(pMsgUpdateInfo);
//
//						// update completed
//						VersionInfo OldVersion(pMsgUpdateInfo->OldVersion);
//						VersionInfo UpdateVersion(pMsgUpdateInfo->UpdateVersion);
//						char strBuffer[256];
//						sprintf(strBuffer, "update completed(%s -> %s)", OldVersion.GetVersionString(),
//							UpdateVersion.GetVersionString());
//						SetProgressGroupText(strBuffer);
//
//						// set current cliet version
//						m_CurrentVersion = UpdateVersion;
//
//						// On DownLoadGamefilesDone
//						OnDownLoadGamefilesDone(NULL, NULL);

					}
					break;
				case T_PC_CONNECT_LOGIN_OK:
					{
						MSG_PC_CONNECT_LOGIN_OK *pConnOK;
						pConnOK = (MSG_PC_CONNECT_LOGIN_OK*)(pPacket + SIZE_FIELD_TYPE_HEADER);

// 2009-04-23 by cmkwon, ·Ż˝ĂľĆ Innova Frost ŔÓ˝Ă Á¦°ĹÇĎ±â - ľĆ·ˇżÍ °°ŔĚ define Ŕ¸·Î Ăł¸® ÇÔ.
//#if defined(SERVICE_TYPE_RUSSIAN_SERVER_1)
#if defined(SERVICE_TYPE_RUSSIAN_SERVER_1) && defined(_USING_INNOVA_FROST_)
// 2009-07-10 by cmkwon, ·Ż˝ĂľĆ Frost ˝Ĺ±Ô Lib·Î ĽöÁ¤ - ±âÁ¸ ĽŇ˝ş ÁÖĽ® Ăł¸®
// 						// 2009-01-30 by cmkwon, ·Ż˝ĂľĆ Innova ·±Ăł ˝Ă˝şĹŰ(ÇÁ·Î˝şĆ®) ĽöÁ¤ - SetUserName() ČŁĂâ Ăł¸®
// 						try
// 						{
// 							SetUserName((LPCSTR)m_szAccountName);
// 						}
// 						catch (std::runtime_error runErr)
// 						{
// 							CString strErr;
// 							strErr.Format("[Error] call SetUserName() error !! %s", runErr.what());
// 							AfxMessageBox(strErr);
// 							exit(0);
// 						}
// 						catch(...)
// 						{
// 							CString strErr;
// 							strErr.Format("[Error] call SetUserName() error !! unknown error(%d)", GetLastError());
// 							exit(0);
// 						}
// 						DbgOut("[090130] call SetUserName(%s)\r\n", m_szAccountName);
// 										
// 
// 						// 2009-01-30 by cmkwon, ·Ż˝ĂľĆ Innova ·±Ăł ˝Ă˝şĹŰ(ÇÁ·Î˝şĆ®) ĽöÁ¤ - ChangeGameAddr() ČŁĂâ Ăł¸®
// 						struct in_addr laddr;
// 						int GameFServIP, GameFServPort, LocalFServIP, LocalFServPort;
// 						GameFServIP		= inet_addr(pConnOK->FieldServerIP);
// 						GameFServPort	= pConnOK->FieldServerPort;
// 
// 						try
// 						{
// 							ChangeGameAddr(GameFServIP, GameFServPort, LocalFServIP, LocalFServPort);
// 						}
// 						catch (std::runtime_error runErr)
// 						{
// 							CString strErr;
// 							strErr.Format("[Error] call FieldServer ChangeGameAddr() error !! %s", runErr.what());
// 							AfxMessageBox(strErr);
// 							exit(0);
// 						}
// 						catch(...)
// 						{
// 							CString strErr;
// 							strErr.Format("[Error] call FieldServer ChangeGameAddr() error !! unknown error(%d)", GetLastError());
// 							AfxMessageBox(strErr);
// 							exit(0);
// 						}
// 
// 						laddr.s_addr	= LocalFServIP;		
// 						STRNCPY_MEMSET(pConnOK->FieldServerIP, inet_ntoa(laddr), SIZE_MAX_IPADDRESS);
// 						pConnOK->FieldServerPort	= LocalFServPort;
// 
// 						int GameIMServIP, GameIMServPort, LocalIMServIP, LocalIMServPort;
// 						GameIMServIP	= inet_addr(pConnOK->IMServerIP);
// 						GameIMServPort	= pConnOK->IMServerPort;
// 
// 						try
// 						{
// 							ChangeGameAddr(GameIMServIP, GameIMServPort, LocalIMServIP, LocalIMServPort);
// 						}
// 						catch (std::runtime_error runErr)
// 						{
// 							CString strErr;
// 							strErr.Format("[Error] call IMServer ChangeGameAddr() error !! %s", runErr.what());
// 							AfxMessageBox(strErr);
// 							exit(0);
// 						}
// 						catch(...)
// 						{
// 							CString strErr;
// 							strErr.Format("[Error] call IMServer ChangeGameAddr() error !! unknown error(%d)", GetLastError());
// 							AfxMessageBox(strErr);
// 							exit(0);
// 						}
// 		
// 						laddr.s_addr	= LocalIMServIP;		
// 						STRNCPY_MEMSET(pConnOK->IMServerIP, inet_ntoa(laddr), SIZE_MAX_IPADDRESS);
// 						pConnOK->IMServerPort	= LocalIMServPort;						
						///////////////////////////////////////////////////////////////////////////////
						// 2009-07-10 by cmkwon, ·Ż˝ĂľĆ Frost ˝Ĺ±Ô Lib·Î ĽöÁ¤ - 
						//DbgOut("[TEMP] 090710 frostSetUserName 1 \r\n");
						frostSetUserName((LPCSTR)m_szAccountName);
						//DbgOut("[TEMP] 090710 frostSetUserName 2 \r\n");
#endif // END - #if defined(SERVICE_TYPE_RUSSIAN_SERVER_1)

						char szEncodedString[SIZE_MAX_PASSWORD_MD5_STRING];
						BYTE byPass[SIZE_MAX_PASSWORD_MD5];
						MD5 MD5_instance;
						
						char szTmPassword[1024];			// 2006-05-22 by cmkwon, şńąřżˇ Ăß°ˇ ˝şĆ®¸µ Ŕűżë
						MEMSET_ZERO(szTmPassword, 1024);
						wsprintf(szTmPassword, "%s%s", MD5_PASSWORD_ADDITIONAL_STRING, m_szPassword);
						MD5_instance.MD5Encode(szTmPassword, byPass);
						MD5_instance.MD5Binary2String(byPass, szEncodedString);

						// 2008-10-08 by cmkwon, ´ë¸¸ 2´Ü°č °čÁ¤ ˝Ă˝şĹŰ Áöżř ±¸Çö(email->uid) - 2´Ü°č °čÁ¤Ŕ» ĽłÁ¤ÇŃ´Ů.
						m_szAccountName	= pConnOK->AccountName;

						char cmdLine[1024];		MEMSET_ZERO(cmdLine, 1024);
						//_chdir(".\\down");
						CString szTmpWindowDegree;
// 2007-12-27 by cmkwon, Ŕ©µµżěÁî ¸đµĺ ±â´É Ăß°ˇ - ľĆ·ˇżÍ °°ŔĚ ĽöÁ¤ÇÔ
//						switch(m_nWindowDegreeReg)
//						{
//// 2007-07-24 by cmkwon, ·±ĂłżˇĽ­ 800*600 ÇŘ»óµµ »čÁ¦ - 
////						case WINDOW_DEGREE_800x600_LOW:			szTmpWindowDegree = "800 600 0";		break;
////						case WINDOW_DEGREE_800x600_MEDIUM:		szTmpWindowDegree = "800 600 1";		break;
////						case WINDOW_DEGREE_800x600_HIGH:		szTmpWindowDegree = "800 600 2";		break;
//						case WINDOW_DEGREE_1024x768_LOW:		szTmpWindowDegree = "1024 768 0";		break;
//						case WINDOW_DEGREE_1024x768_MEDIUM:		szTmpWindowDegree = "1024 768 1";		break;
//						case WINDOW_DEGREE_1024x768_HIGH:		szTmpWindowDegree = "1024 768 2";		break;
//						case WINDOW_DEGREE_1280x960_LOW:		szTmpWindowDegree = "1280 960 0";		break;
//						case WINDOW_DEGREE_1280x960_MEDIUM:		szTmpWindowDegree = "1280 960 1";		break;
//						case WINDOW_DEGREE_1280x960_HIGH:		szTmpWindowDegree = "1280 960 2";		break;
//						case WINDOW_DEGREE_1280x1024_LOW:		szTmpWindowDegree = "1280 1024 0";		break;
//						case WINDOW_DEGREE_1280x1024_MEDIUM:	szTmpWindowDegree = "1280 1024 1";		break;
//						case WINDOW_DEGREE_1280x1024_HIGH:		szTmpWindowDegree = "1280 1024 2";		break;
//						case WINDOW_DEGREE_1600x1200_LOW:		szTmpWindowDegree = "1600 1200 0";		break;
//						case WINDOW_DEGREE_1600x1200_MEDIUM:	szTmpWindowDegree = "1600 1200 1";		break;
//						case WINDOW_DEGREE_1600x1200_HIGH:		szTmpWindowDegree = "1600 1200 2";		break;
//						case WINDOW_DEGREE_W1280x800_LOW:		szTmpWindowDegree = "1280 800 0";		break;
//						case WINDOW_DEGREE_W1280x800_MEDIUM:	szTmpWindowDegree = "1280 800 1";		break;
//						case WINDOW_DEGREE_W1280x800_HIGH:		szTmpWindowDegree = "1280 800 2";		break;
//						case WINDOW_DEGREE_W1600x900_LOW:		szTmpWindowDegree = "1600 900 0";		break;
//						case WINDOW_DEGREE_W1600x900_MEDIUM:	szTmpWindowDegree = "1600 900 1";		break;
//						case WINDOW_DEGREE_W1600x900_HIGH:		szTmpWindowDegree = "1600 900 2";		break;
//
//						// 2007-08-23 by cmkwon, Wide ÇŘ»óµµ 1280x720(16:9) Ăß°ˇ - 
//						case WINDOW_DEGREE_W1280x720_LOW:		szTmpWindowDegree = "1280 720 0";		break;
//						case WINDOW_DEGREE_W1280x720_MEDIUM:	szTmpWindowDegree = "1280 720 1";		break;
//						case WINDOW_DEGREE_W1280x720_HIGH:		szTmpWindowDegree = "1280 720 2";		break;
//						default:
//							{
//								szTmpWindowDegree = "1024 768 2";
//							}
//						}

						// 2007-12-27 by cmkwon, Ŕ©µµżěÁî ¸đµĺ ±â´É Ăß°ˇ - ĽöÁ¤ÇÔ
						int nCX, nCY, nDegree;
						if(FALSE == this->FindWindowResolutionByWindowDegree(&nCX, &nCY, &nDegree, (LPSTR)(LPCSTR)m_csWindowsResolutionReg))
						{
							nCX		= 1024;		nCY		= 768;		nDegree		= 2;	// 2007-12-28 by cmkwon, ±âş»°Ş
						}
						szTmpWindowDegree.Format("%d %d %d", nCX, nCY, nDegree);

						DBGOUT(" Resolution ==> %s\r\n", szTmpWindowDegree);
						if (FALSE == this->IsDlgButtonChecked(IDC_CHECK_WINDOWS_MODE)) //disable full scr
						{
							m_nWindowModeReg = GAME_MODE_WINDOW;//GAME_MODE_FULLSCREEN;
							nCX = 0;		nCY = 0;		nDegree =2; //high borderless

							szTmpWindowDegree.Format("%d %d %d", nCX, nCY, nDegree);
						}
						///////////////////////////////////////////////////////////////////////////////
						// 2009-01-30 by cmkwon, ·Ż˝ĂľĆ Innova ·±Ăł ˝Ă˝şĹŰ(ÇÁ·Î˝şĆ®) ĽöÁ¤ - ľĆ·ˇżÍ °°ŔĚ ĽöÁ¤ÇÔ.
						char szAppPath[1024];		MEMSET_ZERO(szAppPath, 1024);
						char szCmdParam[1024];		MEMSET_ZERO(szCmdParam, 1024);
						if (TRUE == m_ctrl64Bit.GetCheck()) {
							STRNCPY_MEMSET(szAppPath, "Client64.exe", 1024);
						}
						else {
							STRNCPY_MEMSET(szAppPath, "Client.exe", 1024);
						}

#ifndef _ATUM_DEVELOP
						// Releaseżë
						CString szAccountNameLow = g_szMGameID;
						szAccountNameLow.MakeLower();
// 2009-01-30 by cmkwon, ·Ż˝ĂľĆ Innova ·±Ăł ˝Ă˝şĹŰ(ÇÁ·Î˝şĆ®) ĽöÁ¤ - ľĆ·ˇżÍ °°ŔĚ ĽöÁ¤ÇÔ.
// 						sprintf(cmdLine, "%s %s %d %s %d %s %s %d %s INET %s %s %s %s"
// 							, CLIENT_EXEUTE_FILE_NAME
// 							, pConnOK->FieldServerIP, pConnOK->FieldServerPort
// 							, pConnOK->IMServerIP, pConnOK->IMServerPort
// 							, szAccountNameLow, szEncodedString
// 							, m_nWindowModeReg, szTmpWindowDegree,
// 							(LPCSTR)g_szMGameSeed, (LPCSTR)g_szMGameID, (LPCSTR)g_szMGameEncryptedID, (LPCSTR)g_szMGameEncryptedPWD);
						///////////////////////////////////////////////////////////////////////////////
						// 2009-01-30 by cmkwon, ·Ż˝ĂľĆ Innova ·±Ăł ˝Ă˝şĹŰ(ÇÁ·Î˝şĆ®) ĽöÁ¤ - ľĆ·ˇżÍ °°ŔĚ ĽöÁ¤ÇÔ.
						sprintf(szCmdParam, "%s %d %s %d %s %s %d %s INET %s %s %s %s"
							, pConnOK->FieldServerIP, pConnOK->FieldServerPort
							, pConnOK->IMServerIP, pConnOK->IMServerPort
							, szAccountNameLow, szEncodedString
							, m_nWindowModeReg, szTmpWindowDegree,
							(LPCSTR)g_szMGameSeed, (LPCSTR)g_szMGameID, (LPCSTR)g_szMGameEncryptedID, (LPCSTR)g_szMGameEncryptedPWD);
						sprintf(cmdLine, "%s %s", szAppPath, szCmdParam);

	#if defined(SERVICE_TYPE_KOREAN_SERVER_2)		// 2006-10-02 by cmkwon

// 2008-01-22 by cmkwon, S_Exe2: żą´ç ş»Ľ· ·±Ăłżˇ  Ŕ©µµżěÁî¸đµĺ Ŕűżë
// 		#if defined(SERVICE_TYPE_KOREAN_SERVER_2) && !defined(_TEST_SERVER)
// 						// 2008-01-09 by cmkwon, Yedang_Kor_Main Ŕ©µµżěÁî¸đµĺ şńČ°ĽşČ­ - Ç×»ó Ç®¸đµĺ·Î ˝ÇÇŕ
// 						m_nWindowModeReg	= GAME_MODE_FULLSCREEN;
// 		#endif

						// 2008-01-08 by cmkwon, WindowMode Ăß°ˇÇÔ - ľĆ·ˇżÍ °°ŔĚ ĽöÁ¤
						//sprintf(cmdLine, "%s %s %d %s %d %s",
						//	CLIENT_EXEUTE_FILE_NAME,
						//	pConnOK->FieldServerIP, pConnOK->FieldServerPort,
						//	pConnOK->IMServerIP, pConnOK->IMServerPort, szTmpWindowDegree);

// 2009-01-30 by cmkwon, ·Ż˝ĂľĆ Innova ·±Ăł ˝Ă˝şĹŰ(ÇÁ·Î˝şĆ®) ĽöÁ¤ - ľĆ·ˇżÍ °°ŔĚ ĽöÁ¤ÇÔ.
// 						sprintf(cmdLine, "%s %s %d %s %d %s %d", CLIENT_EXEUTE_FILE_NAME, pConnOK->FieldServerIP, pConnOK->FieldServerPort,
// 							pConnOK->IMServerIP, pConnOK->IMServerPort, szTmpWindowDegree, m_nWindowModeReg);
//
//						// 2007-05-17 by cmkwon, forą®Ŕ» ĽöÁ¤ÇÔ
//						sprintf(&(cmdLine[strlen(cmdLine)]), " %s %s %s %s %s %s"
//							, g_szArrargv[1], g_szArrargv[2], g_szArrargv[3]
//							, g_szArrargv[4], g_szArrargv[5], g_szArrargv[6]);

						///////////////////////////////////////////////////////////////////////////////
						// 2009-01-30 by cmkwon, ·Ż˝ĂľĆ Innova ·±Ăł ˝Ă˝şĹŰ(ÇÁ·Î˝şĆ®) ĽöÁ¤ - ľĆ·ˇżÍ °°ŔĚ ĽöÁ¤ÇÔ.
						sprintf(szCmdParam, "%s %d %s %d %s %d", pConnOK->FieldServerIP, pConnOK->FieldServerPort, pConnOK->IMServerIP, pConnOK->IMServerPort, szTmpWindowDegree, m_nWindowModeReg);
						sprintf(&(szCmdParam[strlen(szCmdParam)]), " %s %s %s %s %s %s"
							, g_szArrargv[1], g_szArrargv[2], g_szArrargv[3]
							, g_szArrargv[4], g_szArrargv[5], g_szArrargv[6]);
						sprintf(cmdLine, "%s %s", szAppPath, szCmdParam);

	#else

// 2009-01-30 by cmkwon, ·Ż˝ĂľĆ Innova ·±Ăł ˝Ă˝şĹŰ(ÇÁ·Î˝şĆ®) ĽöÁ¤ - ľĆ·ˇżÍ °°ŔĚ ĽöÁ¤ÇÔ.
// 						sprintf(cmdLine, "%s %s %d %s %d %s %s %d %s",
// 							CLIENT_EXEUTE_FILE_NAME,
// 							pConnOK->FieldServerIP, pConnOK->FieldServerPort,
// 							pConnOK->IMServerIP, pConnOK->IMServerPort,
// 							m_szAccountName, szEncodedString,
// 							m_nWindowModeReg, szTmpWindowDegree);
						///////////////////////////////////////////////////////////////////////////////
						// 2009-01-30 by cmkwon, ·Ż˝ĂľĆ Innova ·±Ăł ˝Ă˝şĹŰ(ÇÁ·Î˝şĆ®) ĽöÁ¤ - ľĆ·ˇżÍ °°ŔĚ ĽöÁ¤ÇÔ.
 						sprintf(szCmdParam, "%s %d %s %d %s %s %d %s", pConnOK->FieldServerIP, pConnOK->FieldServerPort, pConnOK->IMServerIP, pConnOK->IMServerPort, m_szAccountName, szEncodedString, m_nWindowModeReg, szTmpWindowDegree);
						sprintf(cmdLine, "%s %s", szAppPath, szCmdParam);

	#endif // END - #if defined(SERVICE_TYPE_KOREAN_SERVER_2)		// 2006-10-02 by cmkwon
#else
						// °łąßżë
// 2009-01-30 by cmkwon, ·Ż˝ĂľĆ Innova ·±Ăł ˝Ă˝şĹŰ(ÇÁ·Î˝şĆ®) ĽöÁ¤ - ľĆ·ˇżÍ °°ŔĚ ĽöÁ¤ÇÔ.
// 						sprintf(cmdLine, "%s %s %d %s %d %s %s %d %s DEVELOP",
// 							m_szExecutableBinaryReg,
// 							pConnOK->FieldServerIP, pConnOK->FieldServerPort,
// 							pConnOK->IMServerIP, pConnOK->IMServerPort,
// 							m_szAccountName + m_szCrocessSuffix, szEncodedString,
// 							m_nWindowModeReg, szTmpWindowDegree);
						///////////////////////////////////////////////////////////////////////////////
						// 2009-01-30 by cmkwon, ·Ż˝ĂľĆ Innova ·±Ăł ˝Ă˝şĹŰ(ÇÁ·Î˝şĆ®) ĽöÁ¤ - ľĆ·ˇżÍ °°ŔĚ ĽöÁ¤ÇÔ.
						sprintf(szCmdParam, "%s %d %s %d %s %s %d %s DEVELOP", pConnOK->FieldServerIP, pConnOK->FieldServerPort, pConnOK->IMServerIP, pConnOK->IMServerPort, m_szAccountName + m_szCrocessSuffix, szEncodedString, m_nWindowModeReg, szTmpWindowDegree);
						sprintf(cmdLine, "%s %s", szAppPath, szCmdParam);

#endif// end_#ifndef _ATUM_DEVELOP


#ifdef _ATUM_DEVELOP
						if (m_szExecutableBinaryReg.IsEmpty())
						{
							char buf[128];
							sprintf(buf, "%s %d %s %d %s %s %d %s DEVELOP",
												pConnOK->FieldServerIP, pConnOK->FieldServerPort,
												pConnOK->IMServerIP, pConnOK->IMServerPort,
												m_szAccountName, szEncodedString, m_nWindowModeReg, szTmpWindowDegree);
							CLoginSuccessDlg ldlg(buf);
							if (ldlg.DoModal() != IDOK)
							{
								return FALSE;
							}
							m_bProcessingVersionUpdate = TRUE;
							OnCancel();
							break;
						}
#endif
						// AceTR: successful launcher login starts the game immediately.
						// Keep the original authenticated command line/parameters produced
						// by the PreServer so the client receives the exact legacy launch data.
						STRNCPY_MEMSET(m_szLaunchCmdLine, cmdLine, sizeof(m_szLaunchCmdLine));
						STRNCPY_MEMSET(m_szLaunchAppPath, szAppPath, sizeof(m_szLaunchAppPath));
						STRNCPY_MEMSET(m_szLaunchCmdParam, szCmdParam, sizeof(m_szLaunchCmdParam));
						m_bLauncherLoggedIn = TRUE;

						SetProgressGroupText("Giris basarili. Oyun baslatiliyor...");

						// The authenticated command line already contains the derived password.
						// Clear the plaintext UI value before launching.
						m_szPassword.Empty();
						GetDlgItem(IDC_EDIT_PASSWORD)->SetWindowText("");

						if (m_szCrocessSuffix == "")
							ExecGame(m_szLaunchCmdLine, m_szLaunchAppPath, m_szLaunchCmdParam);
						else
							ExecGameCrocess(m_szLaunchCmdLine);

						OnCancel();
						return 0;
					}
					break;
				case T_PC_CONNECT_LAUNCHER_SESSION:
					{
						MSG_PC_CONNECT_LAUNCHER_SESSION* pSession =
							(MSG_PC_CONNECT_LAUNCHER_SESSION*)(pPacket + SIZE_FIELD_TYPE_HEADER);
						m_szLauncherSessionToken = pSession->SessionToken;
						if (!m_szLauncherSessionToken.IsEmpty())
							SetProgressGroupText("Launcher hesabı doğrulandı. Hesap servisleri hazır.");
					}
					break;

				case T_PC_CONNECT_GET_SERVER_GROUP_LIST_OK:
					{
						MSG_PC_CONNECT_GET_SERVER_GROUP_LIST_OK *pServerListOK
							= (MSG_PC_CONNECT_GET_SERVER_GROUP_LIST_OK*)(pPacket+SIZE_FIELD_TYPE_HEADER);

						if (pServerListOK->NumOfServerGroup <= 0)
						{
							AtumMessageBox(STRERR_S_ATUMLAUNCHER_0010);
							OnCancel();
							return 0;
						}

						MEX_SERVER_GROUP_INFO_FOR_LAUNCHER *pArrMexServerGroup
							= ((MEX_SERVER_GROUP_INFO_FOR_LAUNCHER*)(pPacket + MSG_SIZE(MSG_PC_CONNECT_GET_SERVER_GROUP_LIST_OK)));

// 2005-01-01 by cmkwon, Ľ­ąö±ş Ă¤łÎŔ» Ăß°ˇÇĎ¸éĽ­ sortingŔ» Á¦°ĹÇÔ						
//						///////////////////////////////////////////////////////////////////////////////
//						// Ľ­ąö±şŔĚ 2°łŔĚ»óŔĚ¸é ČĄŔâµµ ĽřŔ¸·Î Á¤·Ä ÇŃ´Ů
//						if(pServerListOK->NumOfServerGroup >= 2)
//						{							
//							sort(&pArrMexServerGroup[0], &pArrMexServerGroup[pServerListOK->NumOfServerGroup]
//								, sort_MEX_SERVER_GROUP_INFO_FOR_LAUNCHER_By_Crowdedness());
//						}

						BOOL	bSelectedServerGroupName = FALSE;
						int		nIndexMinCrowdednessServerGroup = -1;
						int		nMinCrowdedness = 100;
						int		nCrowdedIndex = -1;
						///////////////////////////////////////////////////////////////////////////////
						// Ľ­ąö±şŔ» ¸®˝şĆ®żˇ Ăß°ˇÇŃ´Ů
						for (int i = 0; i < pServerListOK->NumOfServerGroup; i++)
						{
							MEX_SERVER_GROUP_INFO_FOR_LAUNCHER *pMexServerGroup
								= &pArrMexServerGroup[i];

							if (strcmp(pMexServerGroup->ServerGroupName, "ARENA") == 0) continue;

							///////////////////////////////////////////////////////////////////////////////
							// ČĄŔâµµ Á¤ş¸¸¦ ServerGroupNameżˇ Ăß°ˇÇŃ´Ů
							CString strTemp;
							COLORREF	fontColor;

							///////////////////////////////////////////////////////////////////////////////
							// ČĄŔâµµ Á¤ş¸°ˇ Ăß°ˇµČ ServerGroupNameŔ» Listżˇ Ăß°ˇÇŃ´Ů
							// 2006-05-02 by ispark, ÄŢş¸·Î şŻ°ć
							if(0 >= pMexServerGroup->Crowdedness)
							{
								strTemp.Format(STRERR_S_ATUMLAUNCHER_0011, pMexServerGroup->ServerGroupName, STRING_SERVER_GROUP_NAME_DELIMIT);
								fontColor = RGB(180, 180, 180);
								nCrowdedIndex = -1; 
							}
							else
							{
								nCrowdedIndex = min(9, (int)(pMexServerGroup->Crowdedness)/10);
#if defined(SERVICE_TYPE_THAI_SERVER_1) || defined(SERVICE_TYPE_SINGAPORE_1) || defined(SERVICE_TYPE_INDONESIA_SERVER_1)
								// 2008-07-24 by cmkwon, WinnerOnline_Tha CCU°ˇ 80% ŔĚ»óŔĚ¸é 100%·Î ş¸ż©ÁÖ±â(K0000369) - 
								nCrowdedIndex = (nCrowdedIndex < 7) ? nCrowdedIndex : 9;
#endif
								strTemp.Format("%-16s%s", pMexServerGroup->ServerGroupName, STRING_SERVER_GROUP_NAME_DELIMIT);
								fontColor = RGB(255, 255, 255);
							}

							///////////////////////////////////////////////////////////////////////////////
							// 2008-02-14 by cmkwon, ·±ĂłżˇĽ­ Ľ­ąö±×·ě ¸íŔĚ ±úÁ®µµ °ÔŔÓ ˝ÇÇŕżˇ ą®Á¦°ˇ ľřµµ·Ď ĽöÁ¤ - m_vectServerGroupList żˇ Ăß°ˇÇŃ´Ů.
							SSERVER_GROUP_FOR_LAUNCHER tmSerGroup;
							tmSerGroup.nIndex	= i;
							STRNCPY_MEMSET(tmSerGroup.szServerGroupName, pMexServerGroup->ServerGroupName, SIZE_MAX_SERVER_NAME);
							m_vectServerGroupList.push_back(tmSerGroup);
							
#ifdef SERVICE_TYPE_CHINESE_SERVER_1	// 2007-06-22 by cmkwon, Áß±ą ł×Ć®żöĹ© »óĹÂ ş¸ż©ÁÖ±â - Áß±ąŔş ČĄŔâµµ ŔĚąĚÁö¸¦ ±×¸®Áö ľĘ°í ł×Ć®żöĹ© Ping ĽÓµµ¸¦ ş¸ż©ÁŘ´Ů
							m_ServerList->InsertItem(i, strTemp, pMexServerGroup->Crowdedness, DEF_COL, fontColor, nCrowdedIndex, this->NTGetPingAverageTime(), TRUE);
#else
							// 7/13/2006 by dgwoo
							m_ServerList->InsertItem(i, strTemp, pMexServerGroup->Crowdedness, DEF_COL, fontColor, nCrowdedIndex);
#endif
							///////////////////////////////////////////////////////////////////////////////
							// ŔĚŔüżˇ Á˘ĽÓÇŃ ServerGroupNameżˇ şń±łÇŃ´Ů
							if(FALSE == m_strServerGroupName.IsEmpty()
								&& 0 == strnicmp((LPSTR)(LPCSTR)m_strServerGroupName, pMexServerGroup->ServerGroupName, SIZE_MAX_SERVER_NAME)
								&& pMexServerGroup->Crowdedness > -1)
							{// Crowdedness°ˇ 0ŔĚ¸é Ľ­ąö Áˇ°Ë »óĹÂŔÓ

								// 7/13/2006 by dgwoo
								m_ServerList->SetCurSel(i);
								m_nOldSel					= i;		// 2007-06-21 by cmkwon, Ľ­ąö±ş Ľ±ĹĂ ąö±× ĽöÁ¤ - Ľ±ĹĂµČ index¸¦ ĽłÁ¤ÇŃ´Ů.
								bSelectedServerGroupName = TRUE;
							}

							if(pMexServerGroup->Crowdedness > 0
								&& pMexServerGroup->Crowdedness < nMinCrowdedness)
							{
								nIndexMinCrowdednessServerGroup = i;
								nMinCrowdedness = pMexServerGroup->Crowdedness;
							}

							CString strNetState;					
							int nPlayerCnt = pMexServerGroup->Crowdedness == -1 ? 0 : pMexServerGroup->Crowdedness;
							strNetState.Format(_T("%d"), nPlayerCnt);
							SetPlayerCntTxt(strNetState);
						}
						
						if(FALSE == bSelectedServerGroupName)
						{
#if defined(SERVICE_TYPE_JAPANESE_SERVER_1)
							// 2008-12-18 by cmkwon, ŔĎş» Arario ·±Ăł ĽöÁ¤ - Ľ­ąö±ş¸íŔĚ ŔŻČżÇĎÁö ľĘŔ¸¸é ±×łÉ Áľ·á
							AtumMessageBox(STRERR_S_ATUMLAUNCHER_0012);
							OnCancel();
							return 0;
#endif
							if(nIndexMinCrowdednessServerGroup == -1)
							{
								AtumMessageBox(STRERR_S_ATUMLAUNCHER_0012);
								OnCancel();
							}
							// 7/13/2006 by dgwoo
							m_ServerList->SetCurSel(nIndexMinCrowdednessServerGroup);
							m_nOldSel					= nIndexMinCrowdednessServerGroup;		// 2007-06-21 by cmkwon, Ľ­ąö±ş Ľ±ĹĂ ąö±× ĽöÁ¤ - Ľ±ĹĂµČ index¸¦ ĽłÁ¤ÇŃ´Ů.
						}						
					}
					break;

				case T_PC_CONNECT_NETWORK_CHECK_OK:		// 2007-06-18 by cmkwon, ł×Ć®żöĹ© »óĹÂ ĂĽĹ©
					{
						MSG_PC_CONNECT_NETWORK_CHECK_OK *pRMsg = (MSG_PC_CONNECT_NETWORK_CHECK_OK*)(pPacket + SIZE_FIELD_TYPE_HEADER);
						this->NTOnReceivedNetworkCheckOK(pRMsg->nCheckCount);
						m_ServerList->UpdateNetworkState(this->NTGetPingAverageTime());		// 2007-06-22 by cmkwon, Áß±ą ł×Ć®żöĹ© »óĹÂ ş¸ż©ÁÖ±â -
						m_ServerList->Invalidate();											// 2007-06-22 by cmkwon, Áß±ą ł×Ć®żöĹ© »óĹÂ ş¸ż©ÁÖ±â -
					}
					break;

				case T_PC_CONNECT_LOGIN_BLOCKED:
					{
						MSG_PC_CONNECT_LOGIN_BLOCKED *pLoginBlocked
							= (MSG_PC_CONNECT_LOGIN_BLOCKED*)(pPacket+SIZE_FIELD_TYPE_HEADER);

						char szTemp[1024];
						wsprintf(szTemp, STRMSG_S_050506
							, pLoginBlocked->szAccountName, pLoginBlocked->szBlockedReasonForUser
							, pLoginBlocked->atimeStart.GetDateTimeString(STRNBUF(SIZE_MAX_ATUM_DATE_TIME_STRING))
							, pLoginBlocked->atimeEnd.GetDateTimeString(STRNBUF(SIZE_MAX_ATUM_DATE_TIME_STRING)));
						AtumMessageBox(szTemp);
						OnCancel();
					}
					break;
				case T_ERROR:
					{
						MSG_ERROR *pRecvMsg;
						pRecvMsg = (MSG_ERROR*)(pPacket + SIZE_FIELD_TYPE_HEADER);

						char buf[1024];
						Err_t error = pRecvMsg->ErrorCode;

						DBGOUT(STRERR_S_ATUMLAUNCHER_0013, GetErrorString(pRecvMsg->ErrorCode), pRecvMsg->ErrorCode, "ST_PRE_SERVER", m_pUpdateWinsocket->m_szPeerIP);

						switch (error)
						{
						case ERR_COMMON_LOGIN_FAILED:
							{
								AtumMessageBox(STRMSG_060824_0000);
								GetDlgItem(IDC_EDIT_PASSWORD)->SetFocus();
								((CEdit*)GetDlgItem(IDC_EDIT_PASSWORD))->SetSel(0, -1);
							}
							break;
						case ERR_PROTOCOL_INVALID_PRESERVER_CLIENT_STATE:
							{
								AtumMessageBox(STRERR_S_ATUMLAUNCHER_0015);
								OnCancel();
							}
							break;
						case ERR_PROTOCOL_EMPTY_ACCOUNTNAME:
							{
								AtumMessageBox(STRERR_S_ATUMLAUNCHER_0016);
								GetDlgItem(IDC_EDIT_ACCOUNT)->SetFocus();
								((CEdit*)GetDlgItem(IDC_EDIT_ACCOUNT))->SetSel(0, -1);
							}
							break;
						case ERR_PROTOCOL_DUPLICATE_LOGIN:
							{
								AtumMessageBox(STRERR_S_ATUMLAUNCHER_0017);
								OnCancel();
							}
							break;
						case ERR_PROTOCOL_ALL_FIELD_SERVER_NOT_ALIVE:
							{
								AtumMessageBox(STRERR_S_ATUMLAUNCHER_0018);
								//OnCancel();
							}
							break;
						case ERR_PROTOCOL_IM_SERVER_NOT_ALIVE:
							{
								AtumMessageBox(STRERR_S_ATUMLAUNCHER_0019);
								//OnCancel();
							}
							break;
						case ERR_COMMON_SERVICE_TEMPORARILY_PAUSED:
							{
								static BOOL bMsgBox = FALSE;
								if(FALSE == bMsgBox)
								{
									bMsgBox = TRUE;
									AtumMessageBox(STRERR_S_ATUMLAUNCHER_0020);
									OnCancel();
								}
							}
							break;
						case ERR_PROTOCOL_LIMIT_GROUP_USER_COUNT:
							{
								AtumMessageBox(STRERR_S_ATUMLAUNCHER_0021);
								OnCancel();
							}
							break;
						case ERR_PROTOCOL_ACCOUNT_BLOCKED:
							{
								char szErrString[512];
								memset(szErrString, 0x00, 512);
								memcpy(szErrString, ((BYTE*)pRecvMsg) + sizeof(MSG_ERROR), pRecvMsg->StringLength);
								sprintf(buf, STRERR_S_ATUMLAUNCHER_0022, szErrString);
								AtumMessageBox(buf);
								OnCancel();
							}
							break;
						case ERR_COMMON_INVALID_CLIENT_VERSION:
							{
								//AtumMessageBox(STRERR_061011_0000);		// 2006-10-11 by cmkwon, ĽöÁ¤(STRERR_S_ATUMLAUNCHER_0023-->STRERR_061011_0000)
								AtumMessageBox("Auto update failed. Remember to run game as administrator! \r\n Code for support: (0x01300)", "OSR Launcher", MB_OK);
							/*	if (WinExec("ClientRepairTool.exe", SW_SHOW) == ERROR_FILE_NOT_FOUND)
								{
									MessageBox("The ClientRepairTool was not found, it will be downloaded now.", "OldSchoolRivals Repair Client");
									HRESULT hr = URLDownloadToFile(NULL, _T("https://patch.oldschoolrivals.com/fullclient/ClientRepairTool.exe"), _T("ClientRepairTool.exe"), 0, NULL);
									WinExec("ClientRepairTool.exe", SW_SHOW);
								}*/
								OnCancel();
							}
							break;
						case ERR_DB_NO_SUCH_ACCOUNT:		// 2006-09-19 by cmkwon, µî·ĎµÇÁö ľĘŔş °čÁ¤ŔÓ
							{
								AtumMessageBox(STRMSG_060824_0000);
							}
							break;
						case ERR_PERMISSION_DENIED:			// 2006-09-27 by cmkwon, Á˘±Ů ±ÇÇŃŔĚ ľř˝Ŕ´Ď´Ů.
							{
								AtumMessageBox(STRMSG_060927_0000);		// 2006-09-27 by cmkwon, "ÇöŔç Ľ­şń˝ş Áˇ°Ë ÁßŔÔ´Ď´Ů. ŔÚĽĽÇŃ »çÇ×Ŕş Č¨ĆäŔĚÁö¸¦ Âü°íÇĎĽĽżä."
								OnCancel();
							}
							break;
						case ERR_NO_SEARCH_CHARACTER:		// 2008-04-29 by cmkwon, Ľ­ąö±ş Á¤ş¸ DBżˇ Ăß°ˇ(˝Ĺ±Ô °čÁ¤ Äł¸ŻĹÍ »ýĽş Á¦ÇŃ ˝Ă˝şĹŰĂß°ˇ) - 
						case ERR_DB_EXECUTION_FAILED:		// 2008-04-29 by cmkwon, Ľ­ąö±ş Á¤ş¸ DBżˇ Ăß°ˇ(˝Ĺ±Ô °čÁ¤ Äł¸ŻĹÍ »ýĽş Á¦ÇŃ ˝Ă˝şĹŰĂß°ˇ) - 
							{
								AtumMessageBox(STRMSG_080430_0001);		// 2008-04-30 by cmkwon, "Ľ±ĹĂÇĎ˝Ĺ Ľ­ąö´Â ˝Ĺ±Ô Äł¸ŻĹÍ »ýĽşŔĚ Á¦ÇŃµČ Ľ­ąö ŔÔ´Ď´Ů."
							}
							break;
						case	ERR_OVER_COUNT:
						{
							char szTmp[1024]; *szTmp = '\0';
							sprintf(szTmp, "You have already to many accounts logged in! You're allowed to have only %d accounts ingame per IP.\r\nIf you're playing with family please contact OSR Team on Discord\r\nOfficial website : www.oldschoolrivals.com", _INET_MAX_ACC_PER_IP);
							MessageBox(szTmp,"Multiaccount - error",MB_OK|MB_ICONERROR);
							OnCancel();
						}
						break;
						default:
							{
								sprintf(buf, STRERR_S_ATUMLAUNCHER_0024, GetErrorString(pRecvMsg->ErrorCode), pRecvMsg->ErrorCode);
								AtumMessageBox(buf);
								OnCancel();
							}
							break;
						}
					}
				default:
					{
					}
				}

				SAFE_DELETE(pPacket);
			}
		}
		break;
	}

	return 0;
}

BOOL CAtumLauncherDlg::DownloadUpdateFile(MSG_PC_CONNECT_UPDATE_INFO *pMsgUpdateInfo)
{
	if(m_pUpdateFTPManager){		return FALSE;}

	VersionInfo OldVersion(pMsgUpdateInfo->OldVersion);
	VersionInfo UpdateVersion(pMsgUpdateInfo->UpdateVersion);
	char szDownLoadFileName[256];
// 2005-12-23 by cmkwon
//	sprintf(szDownLoadFileName, "%s/%s_%s.zip",	pMsgUpdateInfo->FtpUpdateDir, OldVersion.GetVersionString(), UpdateVersion.GetVersionString());
	sprintf(szDownLoadFileName, "%s/%s_%s.zip",	pMsgUpdateInfo->FtpUpdateDownloadDir, OldVersion.GetVersionString(), UpdateVersion.GetVersionString());

	DbgOut("Updating game data file(%s) From %s:%d by FTP\r\n", szDownLoadFileName, pMsgUpdateInfo->FtpIP, pMsgUpdateInfo->FtpPort);

	m_pUpdateFTPManager = new CFTPManager;
	
	BOOL bRet = m_pUpdateFTPManager->ConnectToServer(pMsgUpdateInfo->FtpIP, pMsgUpdateInfo->FtpPort,
									pMsgUpdateInfo->FtpAccountName, pMsgUpdateInfo->FtpPassword);
	if (!bRet)
	{
		AtumMessageBox(STRERR_S_ATUMLAUNCHER_0025);		
		return FALSE;
	}

	HINTERNET hFile;
	int nFileSize = m_pUpdateFTPManager->GetFileSize(szDownLoadFileName, hFile);
	if (nFileSize == -1)
	{
		DbgOut(STRERR_S_ATUMLAUNCHER_0026, szDownLoadFileName);
// 2006-05-26 by cmkwon, FTP Server żÉĽÇżˇ List ş¸ż©ÁÖ±â ĽłÁ¤ŔĚ ľČµÇľî ŔÖŔ» Ľöµµ ŔÖ´Ů
// 		AtumMessageBox(STRERR_S_ATUMLAUNCHER_0027);
// 		return FALSE;
		nFileSize		= 1000*1024*1024;			// 2008-11-27 by cmkwon, ±âş» 1000MBytes·Î ĽłÁ¤ÇÔ. // 2006-05-26 by cmkwon, ±âş» 100MBytes·Î ĽłÁ¤ÇÔ
	}

	m_progressCtrl.SetRange32(0, nFileSize);
	SetProgressGroupText((LPCSTR)(CString("updating from ") + OldVersion.GetVersionString()
							+ " to " + UpdateVersion.GetVersionString()));
	
	SetFTPUpdateState(UPDATE_STATE_DOWNLOADING);
	bRet = m_pUpdateFTPManager->DownloadFile(szDownLoadFileName, nullptr, nullptr, GetSafeHwnd());
	if (!bRet)
	{
		delete m_pUpdateFTPManager;
		m_pUpdateFTPManager = nullptr;
		this->SetFTPUpdateState(UPDATE_STATE_INIT);
		AtumMessageBox(STRERR_S_ATUMLAUNCHER_0028);
		return FALSE;
	}

	return TRUE;
}

void CAtumLauncherDlg::ExtractUpdateFile(MSG_PC_CONNECT_UPDATE_INFO *pMsgUpdateInfo)
{	
	VersionInfo OldVersion(pMsgUpdateInfo->OldVersion);
	VersionInfo UpdateVersion(pMsgUpdateInfo->UpdateVersion);
	char szUpdateFileName[256]; *szUpdateFileName = '\0';
	sprintf_s(szUpdateFileName, "%s_%s.zip", OldVersion.GetVersionString(),
		UpdateVersion.GetVersionString());

	// extract zip file
	CZipArchive ZipOut;
	
	FILE* fp = nullptr;	
	auto const opened = fopen_s(&fp, szUpdateFileName, "rb");
	if (opened != 0) {
		fclose(fp);
		AtumMessageBox(STRERR_S_ATUMLAUNCHER_0029);
		exit(0);
	}	
	fclose(fp);
	
	ZipOut.Open(szUpdateFileName, CZipArchive::zipOpen);

	SetProgressGroupText(static_cast<LPCSTR>(CString("extracting ") + szUpdateFileName));
	m_progressCtrl.SetRange32(0, ZipOut.GetCount());
	for (auto i = 0; i < ZipOut.GetCount(); i++) {
		
		if (!(ZipOut.GetFileInfo(i))->IsDirectory()) {
			CZipFileHeader zipFileHeader;
			ZipOut.GetFileInfo(zipFileHeader, i);
			auto targetFileName = zipFileHeader.GetFileName();
			auto fileAttr = GetFileAttributes(targetFileName);
			if (fileAttr & FILE_ATTRIBUTE_READONLY != 0) {
				fileAttr = fileAttr & ~FILE_ATTRIBUTE_READONLY;
				SetFileAttributes(targetFileName, fileAttr);
			}
		}
		try {
			ZipOut.ExtractFile(i, "./", true);
		}
		catch (CZipException & ex)
		{
			char szTemp[1024]; *szTemp = '\0';
			sprintf_s(szTemp, "Error while processing an archive: %s", static_cast<LPCTSTR>(ex.GetErrorDescription()));
			MessageBox(szTemp, "Error", MB_OK | MB_ICONERROR);
			ZipOut.Close(CZipArchive::afAfterException);
		}
		m_progressCtrl.SetPos(i + 1);
	}

	ZipOut.Close();
	DeleteFile(szUpdateFileName);

	return;
}


///////////////////////////////////////////////////////////////////////////////
/// \fn			BOOL CAtumLauncherDlg::DownloadUpdateFileByHttp(MSG_PC_CONNECT_UPDATE_INFO *pMsgUpdateInfo)
/// \brief		
/// \author		cmkwon
/// \date		2007-01-08 ~ 2007-01-08
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
static size_t WriteCallback(void* ptr, size_t size, size_t nmemb, FILE* data)
{
	size_t written = fwrite(ptr, size, nmemb, data);
	return written;
}
int progress_func(void* ptr, double TotalToDownload, double NowDownloaded,
	double TotalToUpload, double NowUploaded)
{
	if (TotalToDownload <= 0.0) {
		return 0;
	}

	int totaldotz = 40;
	double fractiondownloaded = NowDownloaded / TotalToDownload;
	int dotz = round(fractiondownloaded * totaldotz);
	if (cal) {
		cal->SetProgressPos((int)(fractiondownloaded * 100));
		char szTempMsg[1024];
		sprintf(szTempMsg, " %3.0f%%", fractiondownloaded * 100);

		cal->SetProgressGroupText((LPCSTR)(CString("Updater #2 => Updating from ") + cal->tmOldVersion.GetVersionString() + " to " + cal->tmUpdateVersion.GetVersionString() + szTempMsg));

		memset(szTempMsg, 0x00, sizeof(szTempMsg));
	}
	int ii = 0;
	DBGOUT("%3.0f%% [", fractiondownloaded * 100);

	for (; ii < dotz; ii++) {
		DBGOUT("=");
	}
	for (; ii < totaldotz; ii++) {
		DBGOUT(" ");
	}

	DBGOUT("]\r");
	fflush(stdout);

	return 0;
}
UINT CAtumLauncherDlg::HTTPUpdateThread(LPVOID Param)
{
	CAtumLauncherDlg* dlg = (CAtumLauncherDlg*)Param;
	if (dlg)
	{
		char szTemp[1024];
		sprintf(szTemp, "https://%s%s", dlg->tmpFtpIP, dlg->tmpszDownLoadFileFullPath);
		DBGOUT("\r\n-------------------------------------------------\n");
		DBGOUT(szTemp);
		CURL* curl_handle;
		CURLcode res;
		const char* url = szTemp;
		FILE* pagefile;
		DBGOUT("\r\n-------------------------------------------------\n");
		time_t t = time(nullptr);
		DBGOUT("Localtime: %s", ctime(&t));

		pagefile = fopen(dlg->tmpszDownLoadFileName, "wb");
		curl_handle = curl_easy_init();

		curl_easy_setopt(curl_handle, CURLOPT_URL, url);
		curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, FALSE);
		curl_easy_setopt(curl_handle, CURLOPT_PROGRESSFUNCTION, progress_func);
		curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, pagefile);
		res = curl_easy_perform(curl_handle);
		DBGOUT("\r\n-------------------------------------------------\n");
		if (CURLE_OK == res) {
			curl_off_t val;

			res = curl_easy_getinfo(curl_handle, CURLINFO_SIZE_DOWNLOAD_T, &val);
			if ((CURLE_OK == res) && (val > 0))
				DBGOUT("Data downloaded: %" CURL_FORMAT_CURL_OFF_T " bytes.\n", val);

			res = curl_easy_getinfo(curl_handle, CURLINFO_TOTAL_TIME_T, &val);
			if ((CURLE_OK == res) && (val > 0))
				DBGOUT("Total download time: %" CURL_FORMAT_CURL_OFF_T ".%06ld sec.\n",
				(val / 1000000), (long)(val % 1000000));

			res = curl_easy_getinfo(curl_handle, CURLINFO_SPEED_DOWNLOAD_T, &val);
			if ((CURLE_OK == res) && (val > 0))
				DBGOUT("Average download speed: %" CURL_FORMAT_CURL_OFF_T
					" kbyte/sec.\n", val / 1024);

			res = curl_easy_getinfo(curl_handle, CURLINFO_NAMELOOKUP_TIME_T, &val);
			if ((CURLE_OK == res) && (val > 0))
				DBGOUT("Name lookup time: %" CURL_FORMAT_CURL_OFF_T ".%06ld sec.\n",
				(val / 1000000), (long)(val % 1000000));

			res = curl_easy_getinfo(curl_handle, CURLINFO_CONNECT_TIME_T, &val);
			if ((CURLE_OK == res) && (val > 0))
				DBGOUT("Connect time: %" CURL_FORMAT_CURL_OFF_T ".%06ld sec.\n",
				(val / 1000000), (long)(val % 1000000));

			curl_easy_cleanup(curl_handle);
			fclose(pagefile);
			dlg->OnUpdateFileDownloadOK(0, 0);
		}
		else {
			curl_easy_cleanup(curl_handle);
			fclose(pagefile);
			dlg->SetFTPUpdateState(UPDATE_STATE_INIT);
			DBGOUT("Error while fetching");
			dlg->AtumMessageBox("Autoupdate has failed.\n1. Remember to run game as administrator (Right click-> Run as Administrator)! \n2. Check if firewall or Anti-Virus is not blocking this process\n3. Install latest VC++ redistributable & DirectX - setup file in Common direcotry. \n If this does not help contact STAFF Team!");
			exit(0);
		}

		AfxEndThread(0);
		return TRUE;
	}
	return NULL;
}
BOOL CAtumLauncherDlg::DownloadUpdateFileByHttp(MSG_PC_CONNECT_UPDATE_INFO *pMsgUpdateInfo, bool b_MssWayFailed/* = false*/)
{
	bDownloadedNoticeFile = false;
	VersionInfo OldVersion(pMsgUpdateInfo->OldVersion);
	VersionInfo UpdateVersion(pMsgUpdateInfo->UpdateVersion);
	tmOldVersion = OldVersion;
	tmUpdateVersion = UpdateVersion;
	char szDownLoadFileName[SIZE_MAX_FTP_FILE_PATH];
	char szDownLoadFileFullPath[SIZE_MAX_FTP_FILE_PATH];
	memset(tmpFtpIP, 0x00, sizeof(tmpFtpIP));
	memset(tmpszDownLoadFileName, 0x00, sizeof(tmpszDownLoadFileName));
	memset(tmpszDownLoadFileFullPath, 0x00, sizeof(tmpszDownLoadFileFullPath));

	sprintf(tmpFtpIP, "%s", pMsgUpdateInfo->FtpIP);
	sprintf(tmpszDownLoadFileName, "%s_%s.zip", tmOldVersion.GetVersionString(), tmUpdateVersion.GetVersionString());
	sprintf(tmpszDownLoadFileFullPath, "%s/%s", pMsgUpdateInfo->FtpUpdateDownloadDir, tmpszDownLoadFileName);
	sprintf(szDownLoadFileName, "%s_%s.zip", OldVersion.GetVersionString(), UpdateVersion.GetVersionString());
	sprintf(szDownLoadFileFullPath, "%s/%s", pMsgUpdateInfo->FtpUpdateDownloadDir, szDownLoadFileName);
	/*if(m_pHttpManager){						return FALSE;}

	VersionInfo OldVersion(pMsgUpdateInfo->OldVersion);
	VersionInfo UpdateVersion(pMsgUpdateInfo->UpdateVersion);
	char szDownLoadFileName[SIZE_MAX_FTP_FILE_PATH];
	char szDownLoadFileFullPath[SIZE_MAX_FTP_FILE_PATH];

	sprintf(szDownLoadFileName, "%s_%s.zip", OldVersion.GetVersionString(), UpdateVersion.GetVersionString());
	sprintf(szDownLoadFileFullPath, "%s/%s", pMsgUpdateInfo->FtpUpdateDownloadDir, szDownLoadFileName);

	DbgOut("Updating game data file(%s) From %s:%d by Http\r\n", szDownLoadFileName, pMsgUpdateInfo->FtpIP, pMsgUpdateInfo->FtpPort);

	m_pHttpManager		= new CHttpManager;

	SetFTPUpdateState(UPDATE_STATE_DOWNLOADING);
	Err_t errCode = m_pHttpManager->DownloadFileByHttp(pMsgUpdateInfo->FtpIP, pMsgUpdateInfo->FtpPort, szDownLoadFileFullPath, szDownLoadFileName, TRUE, this->GetSafeHwnd());
	if (ERR_NO_ERROR != errCode)
	{
		SAFE_DELETE(m_pHttpManager);
		this->SetFTPUpdateState(UPDATE_STATE_INIT);
		AtumMessageBox(STRERR_S_ATUMLAUNCHER_0028);
		return FALSE;
	}

	SetProgressGroupText((LPCSTR)(CString("updating from ") + OldVersion.GetVersionString()	+ " to " + UpdateVersion.GetVersionString()));
	return TRUE;*/
	if (!b_MssWayFailed)
	{
		if (m_pHttpManager != nullptr) return FALSE;

		DbgOut("Updating game data file(%s) From %s:%d by Http\r\n", szDownLoadFileName, pMsgUpdateInfo->FtpIP, pMsgUpdateInfo->FtpPort);

		m_pHttpManager = new CHttpManager;

		SetFTPUpdateState(UPDATE_STATE_DOWNLOADING);
		Err_t errCode = m_pHttpManager->DownloadFileByHttp(pMsgUpdateInfo->FtpIP, pMsgUpdateInfo->FtpPort, szDownLoadFileFullPath, szDownLoadFileName, TRUE, this->GetSafeHwnd());
		if (ERR_NO_ERROR != errCode)
		{
			SAFE_DELETE(m_pHttpManager);
			this->SetFTPUpdateState(UPDATE_STATE_INIT);
			AtumMessageBox(STRERR_S_ATUMLAUNCHER_0028, STRMSG_WINDOW_TEXT, MB_OK | MB_ICONERROR);
			return FALSE;
		}

		SetProgressGroupText((LPCSTR)(CString("Updater #1 => Updating from ") + OldVersion.GetVersionString() + " to " + UpdateVersion.GetVersionString()));
	}
	else
	{
		m_progressCtrl.SetRange(0, 100);
		m_progressCtrl.SetPos(0);

		SetProgressGroupText((LPCSTR)(CString("Updater #2 => Updating from ") + tmOldVersion.GetVersionString() + " to " + tmUpdateVersion.GetVersionString()));
		SetFTPUpdateState(UPDATE_STATE_DOWNLOADING);
		AfxBeginThread(HTTPUpdateThread, this, THREAD_PRIORITY_NORMAL, 0, 0, nullptr);
	}
	//SAFE_DELETE(m_pHttpManager);
	return TRUE;
}

// WPARAM: Socket descriptor
LONG CAtumLauncherDlg::OnAsyncSocketMessage(WPARAM wParam, LPARAM lParam)
{
	if ((SOCKET)wParam == m_PreSD)
		m_pUpdateWinsocket->OnAsyncEvent(lParam);
/*	else if ((SOCKET)wParam == m_FieldSD)
		m_pFieldWinsocket->OnAsyncEvent(lParam);*/

	return 0;
}

LONG CAtumLauncherDlg::OnDownLoadGamefilesDone(WPARAM wParam, LPARAM lParam)
{
	if (m_bCancelFlag)
	{
		return -1;
	}

	CString cstrVersion = m_CurrentVersion.GetVersionString();
	SetFileNumText((char*)(LPCTSTR)("VER " + cstrVersion));

	///////////////////////////////////////////////////////////////////////////
	// ´Ůżî·Îµĺ żĎ·áČÄ ąöŔü ĽöÁ¤ ČÄ ´Ů˝Ă CONNECT_VERSION ŔüĽŰ
	((CAtumLauncherApp*)AfxGetApp())->WriteProfile(*this);

	/////////////////////////////////////////////
	// Send Version
	INIT_MSG_WITH_BUFFER(MSG_PC_CONNECT_VERSION, T_PC_CONNECT_VERSION, msgVersion, msgVersionBuff);
	msgVersion->ClientVersion[0] = m_CurrentVersion.GetVersion()[0];
	msgVersion->ClientVersion[1] = m_CurrentVersion.GetVersion()[1];
	msgVersion->ClientVersion[2] = m_CurrentVersion.GetVersion()[2];
	msgVersion->ClientVersion[3] = m_CurrentVersion.GetVersion()[3];
	m_pUpdateWinsocket->Write(msgVersionBuff, MSG_SIZE(MSG_PC_CONNECT_VERSION));

	return 0;
}

void CAtumLauncherDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: Add your message handler code here

// 2009-11-19 by cmkwon, ·Ż˝ĂľĆ AdminToolżˇ FrostLib ŔűżëÇĎ±â - 
#if defined(SERVICE_TYPE_RUSSIAN_SERVER_1) && defined(_USING_INNOVA_FROST_)
	///////////////////////////////////////////////////////////////////////////////
	// 2009-11-19 by cmkwon, ·Ż˝ĂľĆ AdminToolżˇ FrostLib ŔűżëÇĎ±â - 
	frostFinalize();
#endif // END - #if defined(SERVICE_TYPE_RUSSIAN_SERVER_1)

	SAFE_DELETE(m_pUpdateWinsocket);

	CWinSocket::SocketClean();
}

void CAtumLauncherDlg::OnOk()
{
	// TODO: Add your control notification handler code here
	if(m_SelectFlag)
	{
		UpdateData();
		// 7/13/2006 by dgwoo
		if (m_ServerList->GetCurSel() == LB_ERR
			|| m_ServerList->GetCount() < 1)
		{
			AtumMessageBox(STRMSG_S_ATUMLAUNCHER_0001);
			return;
		}

		// 2007-12-27 by cmkwon, Ŕ©µµżěÁî ¸đµĺ ±â´É Ăß°ˇ -
		CString csWDegree;
		GetDlgItem(IDC_COMBO_WINDOW_DEGREE_LAUNCHER)->GetWindowText(csWDegree);
		if(csWDegree.IsEmpty())
		{
			AtumMessageBox(STRMSG_071228_0001);
			return;
		}



		// 2008-12-17 by ckPark ·Ż˝ĂľĆ ·±ĂÄ
		// 2007-12-27 by cmkwon, Ŕ©µµżěÁî ¸đµĺ ±â´É Ăß°ˇ - ĂĽĹ© ąöĆ°Ŕ» Č®ŔÎ ÇŃ´Ů.		
#if defined(SERVICE_TYPE_RUSSIAN_SERVER_1)		// ·Ż˝ĂľĆ ·±Ăł ŔÎĹÍĆäŔĚ˝ş ĽöÁ¤
		// ·Ż˝ĂľĆ´Â ĂĽĹ©ąÚ˝ş »óĹÂ¸¦ ÄÁĆ®·ŃżˇĽ­ ľňľîżÂ´Ů
		if(FALSE == m_ctrlCheckWindowMode.GetCheck())
#else
		if(FALSE == this->IsDlgButtonChecked(IDC_CHECK_WINDOWS_MODE))
#endif
		// end 2008-12-17 by ckPark ·Ż˝ĂľĆ ·±ĂÄ


		{
			m_nWindowModeReg		= GAME_MODE_FULLSCREEN;
		}
		else
		{
			m_nWindowModeReg		= GAME_MODE_WINDOW;
		}

		SendLogin(LOGIN_TYPE_DIRECT);
// 2007-03-06 by cmkwon, żĄ°ÔŔÓ ĽŇ˝ş Á¦°Ĺ·Î ÇĘżä ľřŔ˝
//		//////////////////////////////////////////////////////////////////////
//		// Send Login
//#if defined(_ATUM_DEVELOP) || defined(_MASANG15_SERVER) || defined(_MASANG51_SERVER) || defined(_GLOBAL_ENG_SERVER) || defined(_VTC_VIET_SERVER) || defined(_KOREA_SERVER_2)
//		SendLogin(LOGIN_TYPE_DIRECT);
//#else
//		SendLogin(LOGIN_TYPE_MGAME);
//#endif// end_ATUM_DEVELOP

//		m_nServerGroupReg = m_nServer;
		m_szAccountName.MakeLower();
		m_szAccountNameReg = m_szAccountName;
		m_szPasswordReg = m_szPassword;
// 2007-12-27 by cmkwon, Ŕ©µµżěÁî ¸đµĺ ±â´É Ăß°ˇ - ÇĘżä ľřŔ˝
//		m_nWindowDegreeReg = m_nWindowDegree+3;		// 2007-07-24 by cmkwon, ·±ĂłżˇĽ­ 800*600 ÇŘ»óµµ »čÁ¦ - ±âÁ¸ ·ąÁö˝şĆ®¸®°ŞŔ» »çżëÇĎ±â Ŕ§ÇŘĽ­




		// 2008-12-17 by ckPark ·Ż˝ĂľĆ ·±ĂÄ
		// ·Ż˝ĂľĆ´Â ˝şĆ®¸µŔ» ÄÁĆ®·ŃżˇĽ­ Á÷Á˘ ľňľîżÂ´Ů
#if defined(SERVICE_TYPE_RUSSIAN_SERVER_1)		// ·Ż˝ĂľĆ ·±Ăł ŔÎĹÍĆäŔĚ˝ş ĽöÁ¤
		m_csWindowsResolutionReg	= reinterpret_cast<char*>(m_ctrlComboWindowDegree.GetItemData(m_ctrlComboWindowDegree.GetCurSel()));
#else
		m_csWindowsResolutionReg	= csWDegree;	// 2007-12-27 by cmkwon, Ŕ©µµżěÁî ¸đµĺ ±â´É Ăß°ˇ -
#endif
		// end 2008-12-17 by ckPark ·Ż˝ĂľĆ ·±ĂÄ




		((CAtumLauncherApp*)AfxGetApp())->WriteProfile(*this);

		// °ÔŔÓ ˝ĂŔŰ ąöĆ° ż©·Żąř ´©¸Ł´Â °Ĺ ąćÁö
		DisableControls();
		SetTimer(TIMERID_ENABLE_CONTROL, 2000, NULL);
	}
	else
	{
		AfxMessageBox(STRMSG_S_ATUMLAUNCHER_0001);
	}
}

BOOL CAtumLauncherDlg::SendLogin(BYTE i_nLoginType)
{
	INIT_MSG_WITH_BUFFER(MSG_PC_CONNECT_LOGIN, T_PC_CONNECT_LOGIN, msgLogin, msgLoginBuf);

	msgLogin->LoginType = i_nLoginType;
	if (i_nLoginType == LOGIN_TYPE_DIRECT)
	{
		// Ĺ©·ÎĽĽ˝ş ŔÎÁőżë
		// 2008-10-08 by cmkwon, ´ë¸¸ 2´Ü°č °čÁ¤ ˝Ă˝şĹŰ Áöżř ±¸Çö(email->uid) - 
		//STRNCPY_MEMSET(msgLogin->AccountName, m_szAccountName + m_szCrocessSuffix, SIZE_MAX_ACCOUNT_NAME);
		STRNCPY_MEMSET(msgLogin->AccountName, m_szAccountName + m_szCrocessSuffix, SIZE_MAX_ORIGINAL_ACCOUNT_NAME);
	}
	else if (i_nLoginType == LOGIN_TYPE_MGAME)
	{		
		CString szTmpMGameID	= g_szMGameID;
		szTmpMGameID.MakeLower();
		m_szAccountName			= szTmpMGameID;
		// 2008-10-08 by cmkwon, ´ë¸¸ 2´Ü°č °čÁ¤ ˝Ă˝şĹŰ Áöżř ±¸Çö(email->uid) - 
		//STRNCPY_MEMSET(msgLogin->AccountName, szTmpMGameID, SIZE_MAX_ACCOUNT_NAME);
		STRNCPY_MEMSET(msgLogin->AccountName, szTmpMGameID, SIZE_MAX_ORIGINAL_ACCOUNT_NAME);
	}
	else
	{
		return FALSE;
	}

// 2009-06-09 by cmkwon, ŔĎş» Arario dbg·Î Á˘ĽÓ °ˇ´ÉÇĎ°Ô ĽöÁ¤ - ĆĐ˝şżöµĺµµ 1111·Î °íÁ¤
#if defined(SERVICE_TYPE_JAPANESE_SERVER_1) && defined(_DEBUG)
	m_szPassword			= "1111";
#endif // END - #if defined(SERVICE_TYPE_JAPANESE_SERVER_1) && defined(_DEBUG)

	if (i_nLoginType == LOGIN_TYPE_DIRECT)
	{
		MD5 MD5_instance;
		char szTmPassword[1024];			// 2006-05-22 by cmkwon, şńąřżˇ Ăß°ˇ ˝şĆ®¸µ Ŕűżë
		MEMSET_ZERO(szTmPassword, 1024);
		wsprintf(szTmPassword, "%s%s", MD5_PASSWORD_ADDITIONAL_STRING, m_szPassword);
		MD5_instance.MD5Encode(szTmPassword, msgLogin->Password);
	}
	else if (i_nLoginType == LOGIN_TYPE_MGAME)
	{
		MEMSET_ZERO(msgLogin->Password, SIZE_MAX_PASSWORD_MD5);
	}
	else
	{
		return FALSE;
	}

///////////////////////////////////////////////////////////////////////////////
// 2006-10-02 by cmkwon
#if defined(SERVICE_TYPE_KOREAN_SERVER_2) && !defined(_DEBUG)
	// 2008-10-08 by cmkwon, ´ë¸¸ 2´Ü°č °čÁ¤ ˝Ă˝şĹŰ Áöżř ±¸Çö(email->uid) - 
	//STRNCPY_MEMSET(msgLogin->AccountName, g_szArrargv[1], SIZE_MAX_ACCOUNT_NAME);
	STRNCPY_MEMSET(msgLogin->AccountName, g_szArrargv[1], SIZE_MAX_ORIGINAL_ACCOUNT_NAME);
	MD5 MD5_instance;
	MD5_instance.MD5String2Binary(g_szArrargv[2], msgLogin->Password);

	///////////////////////////////////////////////////////////////////////////////
	// 2007-03-29 by cmkwon, ŔĄŔÎÁőĹ° ĽłÁ¤
	STRNCPY_MEMSET(msgLogin->WebLoginAuthKey, g_szArrargv[5], SIZE_MAX_WEBLOGIN_AUTHENTICATION_KEY);
#endif

	// 7/13/2006 by dgwoo
	if (m_ServerList->GetCrowdedness(m_ServerList->GetCurSel()) == -1) {
		AtumMessageBox(STRERR_S_ATUMLAUNCHER_0030, STRMSG_WINDOW_TEXT, MB_OK | MB_ICONINFORMATION);
		return FALSE;
	}

	CString szSelectedServerName;
// 2008-02-14 by cmkwon, ·±ĂłżˇĽ­ Ľ­ąö±×·ě ¸íŔĚ ±úÁ®µµ °ÔŔÓ ˝ÇÇŕżˇ ą®Á¦°ˇ ľřµµ·Ď ĽöÁ¤ - ľĆ·ˇżÍ °°ŔĚ ĽöÁ¤ÇÔ
// 	///////////////////////////////////////////////////////////////////////////////
// 	// Ľ±ĹĂÇŃ ServerGroupNameŔ» °ˇÁ®żÂ´Ů(ČĄŔâµµ°ˇ Ć÷ÇÔµÇľîŔÖ´Ů)
// 	// 7/13/2006 by dgwoo
// 	m_ServerList->GetTextString(m_ServerList->GetCurSel(), szSelectedServerName);
// 	///////////////////////////////////////////////////////////////////////////////
// 	// °ˇÁ®żÂ ServerGroupNameżˇĽ­ ČĄŔâµµ Á¤ş¸¸¦ Á¦°ĹÇŃ´Ů
// 	int nServerNameEndIndex = szSelectedServerName.FindOneOf(STRING_SERVER_GROUP_NAME_DELIMIT);
// 	szSelectedServerName.Delete(nServerNameEndIndex, szSelectedServerName.GetLength() - nServerNameEndIndex);
	if(FALSE == this->FindServerGroupName(&szSelectedServerName, m_ServerList->GetCurSel()))
	{// 2008-02-14 by cmkwon, ·±ĂłżˇĽ­ Ľ­ąö±×·ě ¸íŔĚ ±úÁ®µµ °ÔŔÓ ˝ÇÇŕżˇ ą®Á¦°ˇ ľřµµ·Ď ĽöÁ¤ - ŔÎµ¦˝ş·Î Ľ­ąö±×·ě ŔĚ¸§Ŕ» °Ë»öÇŃ´Ů.
		AtumMessageBox("Invalid ServerGroup");
		return FALSE;
	}
	
	///////////////////////////////////////////////////////////////////////////////
	// ServerGroupNameŔ» ĽłÁ¤ÇŃ´Ů
	m_strServerGroupName = szSelectedServerName;
	STRNCPY_MEMSET(msgLogin->FieldServerGroupName, szSelectedServerName, SIZE_MAX_SERVER_NAME);
	STRNCPY_MEMSET(msgLogin->PrivateIP, m_PrivateIP, SIZE_MAX_IPADDRESS);
// 2007-06-05 by cmkwon, ÇĘżäľřŔ˝ - ľĆ·ˇżÍ °°ŔĚ ĽöÁ¤ÇÔ
//	if (i_nLoginType == LOGIN_TYPE_DIRECT)
//	{
//		msgLogin->MGameSEX = 0;
//		msgLogin->MGameYear = 0;
//	}
//	else if (i_nLoginType == LOGIN_TYPE_MGAME)
//	{
//		msgLogin->MGameSEX = atoi(g_szMGameSEX);
//		msgLogin->MGameYear = atoi(g_szMGameYear);
//	}
//	else
//	{
//		return FALSE;
//	}
	msgLogin->MGameSEX		= 0;							// 2007-06-05 by cmkwon, Ľşş° - ¸đ¸§=0, ł˛ŔÚ=1, ż©ŔÚ=2
	msgLogin->MGameYear		= this->m_nBirthYear;			// 2007-06-05 by cmkwon

// 2008-02-11 by cmkwon, Yedang_Global_Eng żÜşÎ ŔÎÁő Ăł¸® - Yedang_Global_Eng żˇ ĆĐ˝şżöµĺ ŔÎÁőŔ» WebLoginAuthKey ¸¦ »çżëÇÔ, Password ´Â ¸¶»óżˇĽ­¸¸ »çżëÇÔ
#if defined(SERVICE_TYPE_CHINESE_SERVER_1) || defined(SERVICE_TYPE_ENGLISH_SERVER_1)
	// 2007-07-03 by cmkwon, Áß±ą Yetime şńąĐ ąřČŁ MD5 ÇĎÁö ľĘŔ˝ - ŔÓ˝Ă·Î WebLoginAuthKey ÇĘµĺ »çżëÇÔ
	STRNCPY_MEMSET(msgLogin->WebLoginAuthKey, (LPCSTR)m_szPassword, SIZE_MAX_WEBLOGIN_AUTHENTICATION_KEY);	
#endif
//	DbgOut(" Password:%s", msgLogin->WebLoginAuthKey);

#if defined(SERVICE_TYPE_JAPANESE_SERVER_1)
	// 2008-12-18 by cmkwon, ŔĎş» Arario ·±Ăł ĽöÁ¤ - MSG_PC_CONNECT_LOGINżˇ SesstionKey¸¦ ĽłÁ¤ÇŃ´Ů.
	STRNCPY_MEMSET(msgLogin->WebLoginAuthKey, (LPCSTR)m_csSessionKey, SIZE_MAX_WEBLOGIN_AUTHENTICATION_KEY);
#endif

#if defined(SERVICE_TYPE_RUSSIAN_SERVER_1)
	///////////////////////////////////////////////////////////////////////////////
	// 2010-04-26 by cmkwon, ·Ż˝ĂľĆ Innva ŔÎÁő/şô¸µ ˝Ă˝şĹŰ şŻ°ć - ľĎČŁČ­ ÇĎÁö ľĘŔş ĆĐ˝şżöµĺ¸¦ łŃ±ä´Ů.
	STRNCPY_MEMSET(msgLogin->WebLoginAuthKey, (LPCSTR)m_szPassword, SIZE_MAX_WEBLOGIN_AUTHENTICATION_KEY);
#endif

	return m_pUpdateWinsocket->Write(msgLoginBuf, MSG_SIZE(MSG_PC_CONNECT_LOGIN));
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			int CAtumLauncherDlg::AtumMessageBox(LPCTSTR lpszText, LPCTSTR lpszCaption/*=NULL*/, UINT nType/*=MB_OK*/)
/// \brief		
/// \author		cmkwon
/// \date		2006-04-20 ~ 2006-04-20
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
int CAtumLauncherDlg::AtumMessageBox(LPCTSTR lpszText, LPCTSTR lpszCaption/*=NULL*/, UINT nType/*=MB_OK*/)
{
// 2008-05-09 by cmkwon, EXE1, EXE2 żˇĽ­ ĹÂ±ąľî ±úÁö´Â ą®Á¦ ÇŘ°á
//#ifdef SERVICE_TYPE_VIETNAMESE_SERVER_1
#if defined(SERVICE_TYPE_VIETNAMESE_SERVER_1) || defined(SERVICE_TYPE_THAI_SERVER_1) || defined(SERVICE_TYPE_SINGAPORE_1) || defined(SERVICE_TYPE_INDONESIA_SERVER_1)
	WCHAR wcText[1024];
	MEMSET_ZERO(wcText, sizeof(wcText));
	WCHAR wcCaption[1024];
	MEMSET_ZERO(wcCaption, sizeof(wcCaption));
	// 2008-05-09 by cmkwon, CodePage Á¤ŔÇ Ăß°ˇ - 
	//MultiByteToWideChar(1258, 0, lpszText, strlen(lpszText)+1, wcText, sizeof(wcText)/sizeof(wcText[0]));
	MultiByteToWideChar(CODE_PAGE, 0, lpszText, strlen(lpszText)+1, wcText, sizeof(wcText)/sizeof(wcText[0]));

	char	szCaption[1024] = STRMSG_WINDOW_TEXT;
	if(lpszCaption)
	{
		strncpy(szCaption, lpszCaption, 1023);
	}
	// 2008-05-09 by cmkwon, CodePage Á¤ŔÇ Ăß°ˇ - 
	//MultiByteToWideChar(1258, 0, szCaption, strlen(szCaption)+1, wcCaption, sizeof(wcCaption)/sizeof(wcCaption[0]));
	MultiByteToWideChar(CODE_PAGE, 0, szCaption, strlen(szCaption)+1, wcCaption, sizeof(wcCaption)/sizeof(wcCaption[0]));
	return ::MessageBoxW(GetSafeHwnd(), wcText, wcCaption, nType);
#else

	if(lpszCaption)
	{
		return MessageBox(lpszText, lpszCaption, nType);
	}
	return MessageBox(lpszText, STRMSG_WINDOW_TEXT, nType);	
#endif	
}


///////////////////////////////////////////////////////////////////////////////
/// \fn			BOOL CAtumLauncherDlg::ProcessDeleteFileListByHttp(MSG_PC_CONNECT_SINGLE_FILE_UPDATE_INFO *pUpdateInfo)
/// \brief		
/// \author		cmkwon
/// \date		2007-01-08 ~ 2007-01-08
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CAtumLauncherDlg::ProcessDeleteFileListByHttp(MSG_PC_CONNECT_SINGLE_FILE_UPDATE_INFO *pUpdateInfo)
{
	char			szDelFileName[SIZE_MAX_FTP_FILE_PATH];
	CHttpManager	httpMan;
	Err_t			errCode;

	///////////////////////////////////////////////////////////////////////////////
	// 2007-01-08 by cmkwon, ŔÓ˝Ă ĆÄŔĎ ŔĚ¸§
	STRNCPY_MEMSET(szDelFileName, "tmDelete.txt", SIZE_MAX_FTP_FILE_PATH);
	errCode = httpMan.DownloadFileByHttp(pUpdateInfo->FtpIP, pUpdateInfo->FtpPort, pUpdateInfo->DeleteFileListDownloadPath, szDelFileName);
	if(ERR_NO_ERROR != errCode)
	{
		return FALSE;
	}

#ifdef _DEBUG
		char	szDir[512];
		GetCurrentDirectory(512, szDir);
#endif

	///////////////////////////////////////////////////////////////////////////////		
	// Ăł¸®
	ifstream isDel;
	isDel.open(szDelFileName);
	if(false == isDel.is_open())
	{
		return FALSE;
	}
	char delFileLine[SIZE_MAX_FTP_FILE_PATH];
	bool bFileEndFlag = false;
	do
	{
		isDel.getline(delFileLine, SIZE_MAX_FTP_FILE_PATH);
		bFileEndFlag = isDel.eof();
		if(bFileEndFlag)
		{
			break;
		}

		// 0x0D(CR)¸¦ Á¦°ĹÇĎÁö ¸řÇŃ °ćżě Á¦°ĹÇÔ
		if (delFileLine[strlen(delFileLine)-1] == 0x0D)
		{
			delFileLine[strlen(delFileLine)-1] = '\0';
		}

		if (delFileLine[0] == '#')
		{// commentŔÓ

			continue;
		}

		::DeleteFile(delFileLine);
		SetLastError(0);				// 2007-01-08 by cmkwon
	}while(false == bFileEndFlag);
	isDel.close();

	///////////////////////////////////////////////////////////////////////////////
	// 2007-01-08 by cmkwon, ŔÓ˝Ă ĆÄŔĎŔ» »čÁ¦ÇŃ´Ů
	::DeleteFile(szDelFileName);
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			void CAtumLauncherDlg::MoveWindow2Center(void)
/// \brief		// 2007-09-07 by cmkwon, şŁĆ®ł˛ ·±Ăł ŔÎĹÍĆäŔĚ˝ş ĽöÁ¤ -
/// \author		cmkwon
/// \date		2007-09-07 ~ 2007-09-07
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CAtumLauncherDlg::MoveWindow2Center(void)
{
	int nWidth	= EXE2_LAUNCHER_BG_SIZE_X + 6;
	int nHeight	= EXE2_LAUNCHER_BG_SIZE_Y + 6;

	LONG x,y;
	x=LONG((GetSystemMetrics(SM_CXSCREEN)-nWidth)/2); 
	y=LONG((GetSystemMetrics(SM_CYSCREEN)-nHeight)/2);
	
	MoveWindow(x, y, nWidth, nHeight, TRUE);
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			void CAtumLauncherDlg::PushCharFromScreenKeyboard(char i_cPushChar)
/// \brief		// 2007-09-11 by cmkwon, şŁĆ®ł˛ Č­¸éĹ°ş¸µĺ ±¸Çö -
/// \author		cmkwon
/// \date		2007-09-11 ~ 2007-09-11
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CAtumLauncherDlg::PushCharFromScreenKeyboard(char i_cPushChar)
{
	/*if(nullptr == m_pScreenKeyboardDlg)
	{
		return;
	}*/

	if(nullptr == m_pInputEditFromScreenKeyboard)
	{
		m_pInputEditFromScreenKeyboard = GetDlgItem(IDC_EDIT_ACCOUNT);
	}

	CString strCurText, strResultText;
	m_pInputEditFromScreenKeyboard->GetWindowText(strCurText);
	strResultText.Format("%s%c", strCurText, i_cPushChar);
	m_pInputEditFromScreenKeyboard->SetWindowText(strResultText);
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			void CAtumLauncherDlg::DeleteCharFromScreenKeyboard(void)
/// \brief		// 2007-09-11 by cmkwon, şŁĆ®ł˛ Č­¸éĹ°ş¸µĺ ±¸Çö -
/// \author		cmkwon
/// \date		2007-09-11 ~ 2007-09-11
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CAtumLauncherDlg::DeleteCharFromScreenKeyboard(void)
{
	/*if(nullptr == m_pScreenKeyboardDlg)
	{
		return;
	}*/

	if(nullptr == m_pInputEditFromScreenKeyboard)
	{
		m_pInputEditFromScreenKeyboard = GetDlgItem(IDC_EDIT_ACCOUNT);
	}

	CString strCurText;
	m_pInputEditFromScreenKeyboard->GetWindowText(strCurText);
	if(strCurText.IsEmpty())
	{
		return;
	}
	strCurText.Delete(strCurText.GetLength()-1);
	m_pInputEditFromScreenKeyboard->SetWindowText(strCurText);
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			void CAtumLauncherDlg::HideScreenKeyboardByScreenKeyboardWindow(void)
/// \brief		// 2007-09-18 by cmkwon, Č­»óĹ°ş¸µĺ ĽöÁ¤ - 
/// \author		cmkwon
/// \date		2007-09-18 ~ 2007-09-18
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CAtumLauncherDlg::HideScreenKeyboardByScreenKeyboardWindow(void)
{
	m_bHideScreenKeyboardByScreenKeyboardWindow	= TRUE;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			BOOL CAtumLauncherDlg::FindWindowResolutionByWindowDegree(int *o_pnCX, int *o_pnCY, int *o_pnDegree, char *i_szWindowDegreeName)
/// \brief		// 2007-12-27 by cmkwon, Ŕ©µµżěÁî ¸đµĺ ±â´É Ăß°ˇ - CAtumLauncherDlg::FindWindowResolutionByWindowDegree() Ăß°ˇ
/// \author		cmkwon
/// \date		2007-12-28 ~ 2007-12-28
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CAtumLauncherDlg::FindWindowResolutionByWindowDegree(int *o_pnCX, int *o_pnCY, int *o_pnDegree, char *i_szWindowDegreeName)
{
	*o_pnCX		= 0;
	*o_pnCY		= 0;
	*o_pnDegree	= 0;

	for(int i=0; g_pWindowDegreeList[i].szWindowDegreeName != nullptr; i++)
	{
		if(0 == strncmp(g_pWindowDegreeList[i].szWindowDegreeName, i_szWindowDegreeName, SIZE_MAX_WINDOW_DEGREE_NAME))
		{
			*o_pnCX		= g_pWindowDegreeList[i].nCX;
			*o_pnCY		= g_pWindowDegreeList[i].nCY;
			*o_pnDegree	= g_pWindowDegreeList[i].nDegree;
			return TRUE;
		}
	}

	return FALSE;
}


///////////////////////////////////////////////////////////////////////////////
/// \fn			int CAtumLauncherDlg::InsertWindowDegreeList(CComboBox *i_pComboBox, BOOL i_bWindowsMode)
/// \brief		// 2007-12-27 by cmkwon, Ŕ©µµżěÁî ¸đµĺ ±â´É Ăß°ˇ - CAtumLauncherDlg::InsertWindowDegreeList() Ăß°ˇ
/// \author		cmkwon
/// \date		2007-12-28 ~ 2007-12-28
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
int CAtumLauncherDlg::InsertWindowDegreeList(CComboBox *i_pComboBox, BOOL i_bWindowsMode)
{
	i_pComboBox->ResetContent();	// 2007-12-28 by cmkwon, ¸đµç ľĆŔĚĹŰ ĂĘ±âČ­

	int nCXScreen = GetSystemMetrics(SM_CXSCREEN);
	int nCYScreen = GetSystemMetrics(SM_CYSCREEN);

	int nInsertedCnts = 0;
	for(int i=0; g_pWindowDegreeList[i].szWindowDegreeName != nullptr; i++)
	{
		if(FALSE == this->IsSupportedResolution(g_pWindowDegreeList[i].nCX, g_pWindowDegreeList[i].nCY))
		{// 2008-01-03 by cmkwon, ÁöżřÇĎ´Â ÇŘ»óµµ ¸®˝şĆ®¸¸ ş¸ż©ÁÖ±â - ÁöżřÇĎÁö ľĘ´Â ÇŘ»óµµ´Â ş¸ż©ÁÖÁö ľČ´Â´Ů
			continue;
		}

		if( FALSE == i_bWindowsMode
			|| (nCXScreen >= g_pWindowDegreeList[i].nCX	&& nCYScreen >= g_pWindowDegreeList[i].nCY) )
		{
			i_pComboBox->AddString(g_pWindowDegreeList[i].szWindowDegreeName);



			// 2008-12-17 by ckPark ·Ż˝ĂľĆ ·±ĂÄ
			// ·Ż˝ĂľĆ´Â ˝şĆ®¸µŔ» ÄÁĆ®·ŃżˇĽ­ Á÷Á˘ ĽÂĆĂ
#if defined(SERVICE_TYPE_RUSSIAN_SERVER_1)		// ·Ż˝ĂľĆ ·±Ăł ŔÎĹÍĆäŔĚ˝ş ĽöÁ¤
			i_pComboBox->SetItemData(i, (DWORD)(g_pWindowDegreeList[i].szWindowDegreeName));
#endif
			// end 2008-12-17 by ckPark ·Ż˝ĂľĆ ·±ĂÄ



			nInsertedCnts++;
		}
	}



	// 2008-12-17 by ckPark ·Ż˝ĂľĆ ·±ĂÄ
	// ÄŢş¸ąÚ˝ş °»˝Ĺ˝Ă ´Ů˝Ă ÇŃąř ±×¸°´Ů
#if defined(SERVICE_TYPE_RUSSIAN_SERVER_1)		// ·Ż˝ĂľĆ ·±Ăł ŔÎĹÍĆäŔĚ˝ş ĽöÁ¤
	i_pComboBox->Invalidate(TRUE);
#endif
	// end 2008-12-17 by ckPark ·Ż˝ĂľĆ ·±ĂÄ




	return nInsertedCnts;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			int CAtumLauncherDlg::FindWindowDegreeComboBoxIndex(CComboBox *i_pComboBox, char *i_szWindowDegreeName)
/// \brief		// 2007-12-27 by cmkwon, Ŕ©µµżěÁî ¸đµĺ ±â´É Ăß°ˇ - CAtumLauncherDlg::FindWindowDegreeComboBoxIndex() Ăß°ˇ
/// \author		cmkwon
/// \date		2007-12-28 ~ 2007-12-28
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
int CAtumLauncherDlg::FindWindowDegreeComboBoxIndex(CComboBox *i_pComboBox, char *i_szWindowDegreeName)
{
	int nItemCnts = i_pComboBox->GetCount();
	for(int i=0; i < nItemCnts; i++)
	{
		CString csWDegree;

		
		
		// 2008-12-17 by ckPark ·Ż˝ĂľĆ ·±ĂÄ
#if defined(SERVICE_TYPE_RUSSIAN_SERVER_1)		// ·Ż˝ĂľĆ ·±Ăł ŔÎĹÍĆäŔĚ˝ş ĽöÁ¤
		csWDegree = (char*)(i_pComboBox->GetItemDataPtr(i));
#else
		i_pComboBox->GetLBText(i, csWDegree);
#endif
		// end 2008-12-17 by ckPark ·Ż˝ĂľĆ ·±ĂÄ




		if(0 == csWDegree.Compare(i_szWindowDegreeName))
		{
			return i;
		}
	}

	return -1;
}


///////////////////////////////////////////////////////////////////////////////
/// \fn			int CAtumLauncherDlg::InitSupportedWindowResolutionList(void)
/// \brief		// 2008-01-03 by cmkwon, ÁöżřÇĎ´Â ÇŘ»óµµ ¸®˝şĆ®¸¸ ş¸ż©ÁÖ±â - CAtumLauncherDlg::InitSupportedWindowResolutionList() Ăß°ˇ
/// \author		cmkwon
/// \date		2008-01-03 ~ 2008-01-03
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
int CAtumLauncherDlg::InitSupportedWindowResolutionList(void)
{
 	///////////////////////////////////////////////////////////////////////////////
 	// 2008-06-12 by cmkwon, ŔÓ˝Ă ĂĽĹ©żë, Áöżř ÇŘ»óµµ ·Î±× ĆÄŔĎ·Î ŔúŔĺ - ÇĘżä ľřŔ»°ćżě ÁÖĽ®Ŕ¸·Î Ăł¸®
// 	BOOL bSaveFile	= TRUE;
// 	CSystemLogManager resultLog;
// 	if(FALSE == resultLog.InitLogManger(TRUE, "LauncherLog", "./"))
// 	{
// 		bSaveFile	= FALSE;
// 	}

	DEVMODE devMode;
	INT32 modeExist;
	for (int i=0; ;i++) 
	{
		modeExist = EnumDisplaySettings(nullptr, i, &devMode);
		if (!modeExist) 
		{
			break;
		}

		
// 2008-06-12 by cmkwon, ŔÓ˝Ă ĂĽĹ©żë, Áöżř ÇŘ»óµµ ·Î±× ĆÄŔĎ·Î ŔúŔĺ
// 		if(bSaveFile)
// 		{
// 			// 2008-05-23 by cmkwon, ĂĽĹ©żë
// 			//DbgOut("Resolution Idx(%3d) %4d x %4d , BitsPerPel(%2d) Frequency(%3d)\r\n", i, devMode.dmPelsWidth, devMode.dmPelsHeight, devMode.dmBitsPerPel, devMode.dmDisplayFrequency);
// 			char szResult[2048];
// 			MEMSET_ZERO(szResult, 2048);
// 			sprintf(szResult, "Resolution Idx(%3d) %4d x %4d , BitsPerPel(%2d) Frequency(%3d)\r\n", i, devMode.dmPelsWidth, devMode.dmPelsHeight, devMode.dmBitsPerPel, devMode.dmDisplayFrequency);
// 			resultLog.WriteSystemLog(szResult);
// 		}

// 2008-10-31 by cmkwon, ·±Ăł(Luncher)żˇĽ­ ÇŘ»óµµ ĂĽĹ©˝Ă ÇČĽż¸¸ ĂĽĹ©·Î ĽöÁ¤(»ö»óşńĆ®´Â ĂĽĹ©ÇĎÁö ľĘŔ˝) - ĂĽĹ© ÇĘżä ľřŔ˝
//		// 2008-06-12 by cmkwon, Win98, Win98ME żˇĽ­ ÇŘ»óµµ 1°łµµ łŞżŔÁö ľĘ´Â ą®Á¦ ĽöÁ¤(K0000227) - win98, win98ME żˇĽ­´Â dmDisplayFrequency ŔĚ 0Ŕ¸·Î ¸®ĹĎµÇ°í ŔÖŔ˝.
//		if( 32 != devMode.dmBitsPerPel
//			|| (0 != devMode.dmDisplayFrequency && 60 != devMode.dmDisplayFrequency) )
//		{
//			continue;
//		}

		m_vectSupportedResolutionList.push_back(devMode);
	}

	return m_vectSupportedResolutionList.size();
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			BOOL CAtumLauncherDlg::IsSupportedResolution(int i_nWidth, int i_nHeight)
/// \brief		// 2008-01-03 by cmkwon, ÁöżřÇĎ´Â ÇŘ»óµµ ¸®˝şĆ®¸¸ ş¸ż©ÁÖ±â - CAtumLauncherDlg::IsSupportedResolution() Ăß°ˇ
/// \author		cmkwon
/// \date		2008-01-03 ~ 2008-01-03
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CAtumLauncherDlg::IsSupportedResolution(int i_nWidth, int i_nHeight)
{
	int nCnts = m_vectSupportedResolutionList.size();
	if (i_nWidth == 0 && i_nHeight == 0)
	{
		return TRUE;//for borderless
	}
	for(int i=0; i < nCnts; i++)
	{
		DEVMODE *pDevMode = &(m_vectSupportedResolutionList[i]);
		if(pDevMode->dmPelsWidth == i_nWidth
			&& pDevMode->dmPelsHeight == i_nHeight)
		{
			return TRUE;
		}
	}

	return FALSE;
}


///////////////////////////////////////////////////////////////////////////////
/// \fn			BOOL CAtumLauncherDlg::FindServerGroupName(CString *o_pcsServerGroupName, int i_nFindIndex)
/// \brief		// 2008-02-14 by cmkwon, ·±ĂłżˇĽ­ Ľ­ąö±×·ě ¸íŔĚ ±úÁ®µµ °ÔŔÓ ˝ÇÇŕżˇ ą®Á¦°ˇ ľřµµ·Ď ĽöÁ¤ - CAtumLauncherDlg::FindServerGroupName() Ăß°ˇ
/// \author		cmkwon
/// \date		2008-02-14 ~ 2008-02-14
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CAtumLauncherDlg::FindServerGroupName(CString *o_pcsServerGroupName, int i_nFindIndex)
{
	o_pcsServerGroupName->Empty();
	int i = 0;
	for(i=0; i < m_vectServerGroupList.size(); i++)
	{
		if(i_nFindIndex == m_vectServerGroupList[i].nIndex)
		{
			*o_pcsServerGroupName = m_vectServerGroupList[i].szServerGroupName;
			return TRUE;
		}
	}
	return FALSE;
}





/*
BOOL CAtumLauncherDlg::SetCurrentClientVersion()
{
	const int SIZE_BUFF = 512;
	char buff[SIZE_BUFF];
	char *token;
	char seps[] = " \t";

	ifstream fin;

	CString cstrVersionFile = CLIENT_DOWNLOAD_DIR;
	cstrVersionFile += CLIENT_VERSION_FILE_NAME;
	fin.open((LPCTSTR)cstrVersionFile);

	if (! fin.is_open())
	{
#ifdef _ATUM_DEVELOP
		m_CurrentVersion.SetVersion(0, 0, 0, 0);
		return TRUE;
#else // _ATUM_DEVELOP
		return FALSE;
#endif // _ATUM_DEVELOP
	}

	fin.getline(buff, SIZE_BUFF);

	token = strtok(buff, seps);

	m_CurrentVersion.SetVersion(token);

	fin.close();

	return TRUE;
}
*/


///////////////////////////////////////////////////////////////////////////////
/// \fn			BOOL CAtumLauncherDlg::NTStartNetworkCheck(void)
/// \brief		// 2007-06-18 by cmkwon, ł×Ć®żöĹ© »óĹÂ ĂĽĹ© 
/// \author		cmkwon
/// \date		2007-06-18 ~ 2007-06-18
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CAtumLauncherDlg::NTStartNetworkCheck(void)
{
	if(nullptr == m_pUpdateWinsocket
		|| FALSE == m_pUpdateWinsocket->IsConnected()
		|| -1 != m_nCurNetworkCheckCount)
	{
		return FALSE;
	}


	// 2007-06-18 by cmkwon, ĂĘ±âČ­
	m_dwSumPacketTickGap		= 0;
	m_dwNetworkCheckSendTick	= timeGetTime();

	INIT_MSG_WITH_BUFFER(MSG_PC_CONNECT_NETWORK_CHECK, T_PC_CONNECT_NETWORK_CHECK, pNetCheck, SendBuf);
	pNetCheck->nCheckCount		= m_nCurNetworkCheckCount;	
	m_pUpdateWinsocket->Write(SendBuf, MSG_SIZE(MSG_PC_CONNECT_NETWORK_CHECK));
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			int CAtumLauncherDlg::NTOnReceivedNetworkCheckOK(int i_nCheckCount)
/// \brief		// 2007-06-18 by cmkwon, ł×Ć®żöĹ© »óĹÂ ĂĽĹ©
/// \author		cmkwon
/// \date		2007-06-18 ~ 2007-06-18
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
int CAtumLauncherDlg::NTOnReceivedNetworkCheckOK(int i_nCheckCount)
{
	if(nullptr == m_pUpdateWinsocket
		|| FALSE == m_pUpdateWinsocket->IsConnected()
		|| m_nCurNetworkCheckCount != i_nCheckCount)
	{
		return NTGetPingAverageTime();
	}
	
	DWORD dwCurTick				= timeGetTime();

	m_nCurNetworkCheckCount++;
	m_dwSumPacketTickGap		+= dwCurTick - m_dwNetworkCheckSendTick;

	Sleep(500);		// 2007-06-19 by cmkwon, ľŕ°ŁŔÇ ˝Ă°ŁÂ÷¸¦ µĐ´Ů

	m_dwNetworkCheckSendTick	= timeGetTime();

	if(m_nMaxNetworkCheckCount <= m_nCurNetworkCheckCount)
	{
		return NTGetPingAverageTime();
	}
	INIT_MSG_WITH_BUFFER(MSG_PC_CONNECT_NETWORK_CHECK, T_PC_CONNECT_NETWORK_CHECK, pNetCheck, SendBuf);
	pNetCheck->nCheckCount		= m_nCurNetworkCheckCount;	
	m_pUpdateWinsocket->Write(SendBuf, MSG_SIZE(MSG_PC_CONNECT_NETWORK_CHECK));

	return NTGetPingAverageTime();
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			int CAtumLauncherDlg::NTGetPingAverageTime(void)
/// \brief		// 2007-06-18 by cmkwon, ł×Ć®żöĹ© »óĹÂ ĂĽĹ©
/// \author		cmkwon
/// \date		2007-06-19 ~ 2007-06-19
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
int CAtumLauncherDlg::NTGetPingAverageTime(void)
{
	if(0 >= m_nCurNetworkCheckCount)
	{// 2007-06-19 by cmkwon, ˝ĂŔŰ»óĹÂ°ˇ ľĆ´Ď°ĹłŞ ĂłŔ˝ ĆĐĹ¶Ŕ» ąŢÁö ¸řÇŃ »óĹÂ ŔĎ°ćżě

		return 0;		// 2007-06-21 by cmkwon, Ćň±Ő Ping ĽÓµµ¸¦ ¸®ĹĎÇĎµµ·Ď ĽöÁ¤ÇÔ - 
	}

	DWORD dwAverageTick = m_dwSumPacketTickGap/m_nCurNetworkCheckCount;

// 2007-06-21 by cmkwon, Ćň±Ő Ping ĽÓµµ¸¦ ¸®ĹĎÇĎµµ·Ď ĽöÁ¤ÇÔ
//	const int nBestStateTick	= 100;
//	const int nWorstStateTick	= 300;
//	if(dwAverageTick <= nBestStateTick)
//	{
//		nRetState = 100;
//	}
//	else if(dwAverageTick >= nWorstStateTick)
//	{
//		nRetState = 1;
//	}
//	else
//	{
//		nRetState = 100 - ( 100 * (dwAverageTick-nBestStateTick) )/(nWorstStateTick - nBestStateTick);
//	}
//	

// 2007-07-13 by cmkwon, ÇĘżä ľřŔ˝
//	DbgOut("	 AverageTickGap[%8d] <== Count(%d), SumTickGap(%8d)\r\n"
//		, dwAverageTick, m_nCurNetworkCheckCount, m_dwSumPacketTickGap);

	return dwAverageTick;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			BOOL CAtumLauncherDlg::NTCheckTimeOver(DWORD *o_pdwPingTime)
/// \brief		// 2007-06-18 by cmkwon, ł×Ć®żöĹ© »óĹÂ ĂĽĹ©
/// \author		cmkwon
/// \date		2007-06-19 ~ 2007-06-19
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CAtumLauncherDlg::NTCheckTimeOver(DWORD *o_pdwPingTime)
{
	if(0 > m_nCurNetworkCheckCount
		|| m_nMaxNetworkCheckCount <= m_nCurNetworkCheckCount)
	{
		return FALSE;
	}

	DWORD dwCurTick				= timeGetTime();
	if(TICKGAP_NETWORK_STATE_WORST_PING_TICK > dwCurTick - m_dwNetworkCheckSendTick)
	{// 2007-06-19 by cmkwon, Ăł¸®°ˇ ÇĘżä ľřŔ¸ąÇ·Î ±×łÉ ÇöŔç »óĹÂ¸¦ ¸®ĹĎ
		*o_pdwPingTime = NTGetPingAverageTime();
	}
	else
	{
		// 2007-06-19 by cmkwon, ÇöŔç ĆĐĹ¶Ŕ» ĂĘ±âČ­ ÇĎ°í ´ŮŔ˝ ĆĐĹ¶ ŔüĽŰ
		*o_pdwPingTime = NTOnReceivedNetworkCheckOK(m_nCurNetworkCheckCount);
	}

	return TRUE;
}


char* CAtumLauncherDlg::GetFileName(char *szFullPathName, char *szFileName)
{
	char *token;
	char seps[] = "/";
	int len = strlen(szFullPathName);

	strcpy(szFileName, szFullPathName);

	token = strtok(szFileName, seps);
	len -= strlen(token);

	while( len > 7 )
	{
		token = strtok(nullptr, seps);
		len -= strlen(token);
	}

	STRNCPY_MEMSET(szFileName, token, SIZE_MAX_FTP_FILE_PATH);

	return szFileName;
}

// devideą®ŔÚ¸¦ °ć°č·Î ¸®˝şĆ®żˇ łÖ´Â´Ů.
BOOL CAtumLauncherDlg::Tokenizer(CList <CString, CString&>& lsString, CString strMsg, TCHAR devide)
{
	if( strMsg.IsEmpty() ) return FALSE;
	int nStart = 0;
	int pos;
	if( strMsg[strMsg.GetLength()-1] != devide )
		strMsg += devide;
	pos = strMsg.Find( devide, nStart );
	if( pos == -1)
		lsString.AddTail( strMsg );
	while( (pos = strMsg.Find( devide, nStart )) != -1 )	{
		lsString.AddTail( strMsg.Mid( nStart, pos - nStart ) );
		nStart = pos+1;
	}
	return TRUE;
}

int CAtumLauncherDlg::CreateDirectory(CString strParent, CList<CString, CString&> &lsDir)
{
	POSITION pos = lsDir.GetHeadPosition();
	strParent += lsDir.GetNext(pos);
	if(pos)// ¸¶Áö¸·Ŕş µđ·şĹä¸®°ˇ ľĆ´Ń şą»çÇŇ ĆÄŔĎ
	{
		CFileFind finder;
		if(finder.FindFile(strParent))
		{

		    finder.FindNextFile();
			if(!finder.IsDirectory())
			{
				DeleteFile(strParent); // ĆÄŔĎ·Î Á¸ŔçÇĎ¸é Áöżî´Ů.
				if(!::CreateDirectory(strParent, nullptr))
					return -1;
			}
		} else
			if(!::CreateDirectory(strParent, nullptr))
				return -1;

		lsDir.GetPrev(pos);
		lsDir.RemoveAt(pos);
		strParent += '/';
		if(CreateDirectory(strParent,lsDir)==-1)
			return -1;
	}
	return 0;

}

void CAtumLauncherDlg::SetProgressRange(int up)
{
	m_progressCtrl.SetRange32(0, up);
}

void CAtumLauncherDlg::SetProgressPos(int pos)
{
	m_progressCtrl.SetPos(pos);
}

void CAtumLauncherDlg::SetFileNumText(char* str)
{
	CWnd *pWndStatus = GetDlgItem(IDC_DOWNLOAD_FILENUM);
// 2007-09-11 by cmkwon, ľĆ·ˇżÍ °°ŔĚ ĽöÁ¤ÇÔ
//	InvalidateRect(CRect(90,579,190,592),true);
	InvalidateRect(CRect(EXE2_BG_DOWNLOAD_FILE_STATIC_POS_X,EXE2_BG_DOWNLOAD_FILE_STATIC_POS_Y,EXE2_BG_DOWNLOAD_FILE_STATIC_POS_X + EXE2_BG_DOWNLOAD_FILE_STATIC_WIDTH,EXE2_BG_DOWNLOAD_FILE_STATIC_POS_Y+EXE2_BG_DOWNLOAD_FILE_STATIC_HEIGHT),true);
	
	pWndStatus->SetWindowText(str);
}
void CAtumLauncherDlg::SetPlayerCntTxt(const char* str)
{
	CWnd* pWndStatus = GetDlgItem(IDC_PLAYER_CNT);
	InvalidateRect(CRect(EXE2_BG_UPDATE_INFO_STATIC_POS_X + PLAYERCNT_X, EXE2_BG_UPDATE_INFO_STATIC_POS_Y +PLAYERCNT_Y, EXE2_BG_UPDATE_INFO_STATIC_POS_X + PLAYERCNT_X + EXE2_BG_UPDATE_INFO_STATIC_WIDTH, EXE2_BG_UPDATE_INFO_STATIC_POS_Y+ PLAYERCNT_Y + EXE2_BG_UPDATE_INFO_STATIC_HEIGHT), true);

	CPaintDC dc(this);
	CBitmap* oldbitmap;
	CDC memDC;
	memDC.CreateCompatibleDC(&dc);
	oldbitmap = (CBitmap*)memDC.SelectObject(&m_BackGround);
	dc.BitBlt(EXE2_BG_UPDATE_INFO_STATIC_POS_X + PLAYERCNT_X, EXE2_BG_UPDATE_INFO_STATIC_POS_Y+PLAYERCNT_Y, EXE2_BG_UPDATE_INFO_STATIC_WIDTH, EXE2_BG_UPDATE_INFO_STATIC_HEIGHT, &memDC, EXE2_BG_UPDATE_INFO_STATIC_POS_X + PLAYERCNT_X, EXE2_BG_UPDATE_INFO_STATIC_POS_Y+ PLAYERCNT_Y, SRCCOPY);
	memDC.SelectObject(oldbitmap);
	memDC.DeleteDC();

	WCHAR wcText[1024];
	MEMSET_ZERO(wcText, sizeof(wcText));
	// 2008-05-09 by cmkwon, CodePage Á¤ŔÇ Ăß°ˇ - 
	//MultiByteToWideChar(1258, 0, str, strlen(str)+1, wcText, sizeof(wcText)/sizeof(wcText[0])); 
	MultiByteToWideChar(CODE_PAGE, 0, str, strlen(str) + 1, wcText, sizeof(wcText) / sizeof(wcText[0]));
	::SetWindowTextW(pWndStatus->GetSafeHwnd(), wcText);
}
void CAtumLauncherDlg::SetNoticeTxt(const char* str)
{
	CWnd* pWndStatus = GetDlgItem(IDC_NOTICE2);
	InvalidateRect(CRect(EXE2_BG_UPDATE_INFO_STATIC_POS_X + PLAYERCNT1_X, EXE2_BG_UPDATE_INFO_STATIC_POS_Y + PLAYERCNT1_Y, EXE2_BG_UPDATE_INFO_STATIC_POS_X + PLAYERCNT1_X + EXE2_BG_UPDATE_INFO_STATIC_WIDTH, EXE2_BG_UPDATE_INFO_STATIC_POS_Y + PLAYERCNT1_Y + EXE2_BG_UPDATE_INFO_STATIC_HEIGHT), true);

	CPaintDC dc(this);
	CBitmap* oldbitmap;
	CDC memDC;
	memDC.CreateCompatibleDC(&dc);
	oldbitmap = (CBitmap*)memDC.SelectObject(&m_BackGround);
	dc.BitBlt(EXE2_BG_UPDATE_INFO_STATIC_POS_X + PLAYERCNT1_X, EXE2_BG_UPDATE_INFO_STATIC_POS_Y + PLAYERCNT1_Y, EXE2_BG_UPDATE_INFO_STATIC_WIDTH, EXE2_BG_UPDATE_INFO_STATIC_HEIGHT, &memDC, EXE2_BG_UPDATE_INFO_STATIC_POS_X + PLAYERCNT1_X, EXE2_BG_UPDATE_INFO_STATIC_POS_Y + PLAYERCNT1_Y, SRCCOPY);
	memDC.SelectObject(oldbitmap);
	memDC.DeleteDC();

	WCHAR wcText[1024];
	MEMSET_ZERO(wcText, sizeof(wcText));
	MultiByteToWideChar(CODE_PAGE, 0, str, strlen(str) + 1, wcText, sizeof(wcText) / sizeof(wcText[0]));
	::SetWindowTextW(pWndStatus->GetSafeHwnd(), wcText);
}
void CAtumLauncherDlg::SetProgressGroupText(const char* str)
{
	//ysw : »čÁ¦
	CWnd *pWndStatus = GetDlgItem(IDC_FILE_INFO);
	// 7/10/2006 by dgwoo ±Űľľżˇ ŔÜ»óŔĚ ł˛´Â°ÍŔ» ¸·±âŔ§ÇŃ şÎşĐ.

// 2007-09-11 by cmkwon, ľĆ·ˇżÍ °°ŔĚ ĽöÁ¤ÇÔ
//	InvalidateRect(CRect(190,579,480, 592),true);
	InvalidateRect(CRect(EXE2_BG_UPDATE_INFO_STATIC_POS_X,EXE2_BG_UPDATE_INFO_STATIC_POS_Y,EXE2_BG_UPDATE_INFO_STATIC_POS_X+EXE2_BG_UPDATE_INFO_STATIC_WIDTH, EXE2_BG_UPDATE_INFO_STATIC_POS_Y+EXE2_BG_UPDATE_INFO_STATIC_HEIGHT),true);
	
	CPaintDC dc(this);
	CBitmap *oldbitmap;
	CDC memDC;
	memDC.CreateCompatibleDC(&dc);
	oldbitmap = (CBitmap *)memDC.SelectObject(&m_BackGround);
	dc.BitBlt(EXE2_BG_UPDATE_INFO_STATIC_POS_X,EXE2_BG_UPDATE_INFO_STATIC_POS_Y,EXE2_BG_UPDATE_INFO_STATIC_WIDTH,EXE2_BG_UPDATE_INFO_STATIC_HEIGHT, &memDC,EXE2_BG_UPDATE_INFO_STATIC_POS_X,EXE2_BG_UPDATE_INFO_STATIC_POS_Y,SRCCOPY);
	memDC.SelectObject(oldbitmap);
	memDC.DeleteDC();
	
	WCHAR wcText[1024]; 
	MEMSET_ZERO(wcText, sizeof(wcText)); 
	// 2008-05-09 by cmkwon, CodePage Á¤ŔÇ Ăß°ˇ - 
	//MultiByteToWideChar(1258, 0, str, strlen(str)+1, wcText, sizeof(wcText)/sizeof(wcText[0])); 
	MultiByteToWideChar(CODE_PAGE, 0, str, strlen(str)+1, wcText, sizeof(wcText)/sizeof(wcText[0])); 
	::SetWindowTextW(pWndStatus->GetSafeHwnd(), wcText);
}


void CAtumLauncherDlg::OnCancel()
{
	// TODO: Add extra cleanup here
	m_bCancelFlag = TRUE;
	if (!m_bProcessingVersionUpdate)
	{
//		AtumMessageBox("Operation Canceled!");
	}

	CDialog::OnCancel();
}

void CAtumLauncherDlg::SetLauncherMainPage(int page)
{
	if (page < 0 || page > 4)
		page = 0;

	m_nLauncherMainPage = page;

	CWnd* notice = GetDlgItem(IDC_NOTICE2);
	CWnd* serverList = GetDlgItem(IDC_LIST);

	if (page == 0)
	{
		if (notice) notice->ShowWindow(SW_SHOW);
		if (serverList)
			serverList->ShowWindow(m_bLauncherLoggedIn ? SW_HIDE : SW_SHOW);
	}
	else
	{
		if (notice) notice->ShowWindow(SW_HIDE);
		if (serverList) serverList->ShowWindow(SW_HIDE);
	}

	InvalidateRect(CRect(50, 70, 815, 610), FALSE);
	InvalidateRect(CRect(470, 8, 990, 65), FALSE);
}

BOOL CAtumLauncherDlg::LoadLauncherCharacters()
{
	m_szLauncherCharacterData.Empty();

	if (m_szLauncherSessionToken.IsEmpty())
	{
		SetProgressGroupText("Launcher oturumu hazir degil. PreServer session ayarini kontrol et.");
		return FALSE;
	}

	CString url = GetLauncherApiBaseUrl() + "/api/launcher/characters";
	CURL* curl = curl_easy_init();
	if (!curl)
	{
		SetProgressGroupText("Hesap servisi baslatilamadi.");
		return FALSE;
	}

	std::string response;
	struct curl_slist* headers = NULL;
	CString sessionHeader;
	sessionHeader.Format("X-AceTR-Session: %s", (LPCSTR)m_szLauncherSessionToken);
	headers = curl_slist_append(headers, (LPCSTR)sessionHeader);
	headers = curl_slist_append(headers, "Accept: text/plain");

	curl_easy_setopt(curl, CURLOPT_URL, (LPCSTR)url);
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, LauncherApiWriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 4L);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 8L);
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

	CURLcode code = curl_easy_perform(curl);
	long httpCode = 0;
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);

	curl_slist_free_all(headers);
	curl_easy_cleanup(curl);

	if (code != CURLE_OK)
	{
		CString err;
		err.Format("Hesap servisine ulaşılamadı: %s", curl_easy_strerror(code));
		SetProgressGroupText(err);
		return FALSE;
	}

	if (httpCode == 401)
	{
		SetProgressGroupText("Launcher oturumunun suresi doldu. Tekrar giris yap.");
		return FALSE;
	}

	if (httpCode != 200)
	{
		CString err;
		err.Format("Hesap servisi hata verdi (HTTP %ld).", httpCode);
		SetProgressGroupText(err);
		return FALSE;
	}

	m_szLauncherCharacterData = response.c_str();
	if (m_szLauncherCharacterData.IsEmpty())
		m_szLauncherCharacterData = "Bu hesapta karakter bulunamadi.";

	SetProgressGroupText("Karakter bilgileri guncellendi.");
	return TRUE;
}

void CAtumLauncherDlg::LogoutLauncherAccount()
{
	m_bLauncherLoggedIn = FALSE;
	m_nLauncherAccountPage = 0;
	MEMSET_ZERO(m_szLaunchCmdLine, sizeof(m_szLaunchCmdLine));
	MEMSET_ZERO(m_szLaunchAppPath, sizeof(m_szLaunchAppPath));
	MEMSET_ZERO(m_szLaunchCmdParam, sizeof(m_szLaunchCmdParam));
	m_szPassword.Empty();

	GetDlgItem(IDC_EDIT_ACCOUNT)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_EDIT_PASSWORD)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_CHECK_REMEMBER_ID)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_CHECK_64_BIT)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_COMBO_WINDOW_DEGREE_LAUNCHER)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_CHECK_WINDOWS_MODE)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_PLAYER_CNT)->ShowWindow(SW_SHOW);

	m_KbcGO.SetModernButton("GİRİŞ YAP", RGB(225, 82, 35));
	m_KbcGO.SetToolTipText("Giris Yap");
	EnableControls();

	GetDlgItem(IDC_EDIT_PASSWORD)->SetWindowText("");
	GetDlgItem(IDC_EDIT_PASSWORD)->SetFocus();
	SetProgressGroupText("Oturum kapatildi.");
	Invalidate(FALSE);
}

void CAtumLauncherDlg::DisableControls()
{
	m_bControlEnabled = FALSE;

	GetDlgItem(IDC_EDIT_ACCOUNT)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_PASSWORD)->EnableWindow(FALSE);
	GetDlgItem(IDOK)->EnableWindow(FALSE);
	GetDlgItem(IDGO)->EnableWindow(FALSE);
	m_KbcGO.SetButtonDisable();
	GetDlgItem(IDC_COMBO_WINDOW_DEGREE_LAUNCHER)->EnableWindow(FALSE);
	GetDlgItem(IDC_LIST)->EnableWindow(FALSE);

	GetDlgItem(IDC_COMBO_WINDOW_DEGREE_LAUNCHER)->EnableWindow(FALSE);	// 2008-01-03 by cmkwon, Ŕ©µµżě¸đµĺ »óĹÂ ŔúŔĺÇĎ±â - ąö±× ĽöÁ¤
	GetDlgItem(IDC_CHECK_WINDOWS_MODE)->EnableWindow(FALSE);			// 2008-01-17 by cmkwon, Ŕ©µµżě¸đµĺ »óĹÂ ŔúŔĺÇĎ±â - ąö±× ĽöÁ¤

	m_ctlBtnRememberID.EnableWindow(FALSE);			// 2008-06-17 by cmkwon, WinnerOnline_Tha LauncherżˇĽ­ ŔĚŔü Á˘ĽÓ °čÁ¤ ±âľďÇĎ±â(K0000243) - ĂĽĹ©ąÚ˝ş Ŕ§Äˇ
	m_ctrl64Bit.EnableWindow(FALSE);			// 2008-06-17 by cmkwon, WinnerOnline_Tha LauncherżˇĽ­ ŔĚŔü Á˘ĽÓ °čÁ¤ ±âľďÇĎ±â(K0000243) - ĂĽĹ©ąÚ˝ş Ŕ§Äˇ

	//ysw : »čÁ¦
//	GetDlgItem(IDC_DECA_SERVER)->EnableWindow(FALSE);
//	GetDlgItem(IDC_BATTALUS_SERVER)->EnableWindow(FALSE);
//	GetDlgItem(IDC_SHARRINE_SERVER)->EnableWindow(FALSE);
//	GetDlgItem(IDC_PHILON_SERVER)->EnableWindow(FALSE);


	SendDlgItemMessage(IDOK, BM_SETSTYLE, BS_PUSHBUTTON, (LONG)TRUE);
	SendDlgItemMessage(IDCANCEL, BM_SETSTYLE, BS_DEFPUSHBUTTON, (LONG)TRUE);
	GetDlgItem(IDCANCEL)->SetFocus();

	SetProgressGroupText("Connecting server...");
}

void CAtumLauncherDlg::EnableControls()
{
//	DbgOut("	2007-07-13 by cmkwon, 111\r\n");	// 2007-07-13 by cmkwon, Ĺ×˝şĆ® 
	m_bControlEnabled = TRUE;

	int nLower, nUpper;
	m_progressCtrl.GetRange(nLower, nUpper);
	m_progressCtrl.SetPos(nUpper);

	GetDlgItem(IDC_EDIT_ACCOUNT)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_PASSWORD)->EnableWindow(TRUE);
	GetDlgItem(IDOK)->EnableWindow(TRUE);
	GetDlgItem(IDGO)->EnableWindow(TRUE);
	m_KbcGO.SetButtonEnable();
// 2008-01-03 by cmkwon, Ŕ©µµżě¸đµĺ »óĹÂ ŔúŔĺÇĎ±â - ąö±× ĽöÁ¤
//	if (m_nWindowModeReg == GAME_MODE_WINDOW)
//	{
//		GetDlgItem(IDC_COMBO_WINDOW_DEGREE_LAUNCHER)->EnableWindow(FALSE);
//	}
//	else
//	{
//		GetDlgItem(IDC_COMBO_WINDOW_DEGREE_LAUNCHER)->EnableWindow(TRUE);
//	}
	GetDlgItem(IDC_COMBO_WINDOW_DEGREE_LAUNCHER)->EnableWindow(TRUE);	// 2008-01-03 by cmkwon, Ŕ©µµżě¸đµĺ »óĹÂ ŔúŔĺÇĎ±â - ąö±× ĽöÁ¤
	GetDlgItem(IDC_CHECK_WINDOWS_MODE)->EnableWindow(TRUE);				// 2008-01-17 by cmkwon, Ŕ©µµżě¸đµĺ »óĹÂ ŔúŔĺÇĎ±â - ąö±× ĽöÁ¤
	GetDlgItem(IDC_LIST)->EnableWindow(TRUE);

	m_ctlBtnRememberID.EnableWindow();			// 2008-06-17 by cmkwon, WinnerOnline_Tha LauncherżˇĽ­ ŔĚŔü Á˘ĽÓ °čÁ¤ ±âľďÇĎ±â(K0000243) - ĂĽĹ©ąÚ˝ş Ŕ§Äˇ
	m_ctrl64Bit.EnableWindow();			// 2008-06-17 by cmkwon, WinnerOnline_Tha LauncherżˇĽ­ ŔĚŔü Á˘ĽÓ °čÁ¤ ±âľďÇĎ±â(K0000243) - ĂĽĹ©ąÚ˝ş Ŕ§Äˇ

	GetDlgItem(IDC_EDIT_ACCOUNT)->GetWindowText(m_szAccountNameReg);
	if (m_szAccountNameReg == "")
	{
		GetDlgItem(IDC_EDIT_ACCOUNT)->SetFocus();
	}
	else
	{
		GetDlgItem(IDC_EDIT_PASSWORD)->SetFocus();
		((CEdit*)GetDlgItem(IDC_EDIT_PASSWORD))->SetSel(0, -1);
	}

	// reset default button
	SendDlgItemMessage(IDCANCEL, BM_SETSTYLE, BS_PUSHBUTTON, (LONG)TRUE);
	SendDlgItemMessage(IDOK, BM_SETSTYLE, BS_DEFPUSHBUTTON, (LONG)TRUE);
//	DbgOut("	2007-07-13 by cmkwon, 222\r\n");	// 2007-07-13 by cmkwon, Ĺ×˝şĆ®
	char szTemp[1024];
	sprintf(szTemp, "Client version: %s", m_CurrentVersion.GetVersionString() );
	SetProgressGroupText(szTemp);

	/*char strWebAddress[1024];
	STRNCPY_MEMSET(strWebAddress, "patch.oldschoolrivals.com/", 1024);
	RECT rt;
	rt.left = EXE2_BG_WEBCONTROL_POS_X;
	rt.top = EXE2_BG_WEBCONTROL_POS_Y;
	rt.right = EXE2_BG_WEBCONTROL_POS_X + EXE2_BG_WEBCONTROL_WIDTH;
	rt.bottom = EXE2_BG_WEBCONTROL_POS_Y + EXE2_BG_WEBCONTROL_HEIGHT;
	m_pHost = new Host(m_hWnd, strWebAddress, nullptr, nullptr, nullptr, &rt);*/
}

void CAtumLauncherDlg::SetPrivateIP()
{
	// ŔÚ˝ĹŔÇ IP Address¸¦ ±¸ÇŃ´Ů
	char	host[100];
	HOSTENT	*p;
	char	ip[SIZE_MAX_IPADDRESS];

	gethostname(host, 100);
	if(p = gethostbyname(host))
	{
		sprintf(ip, "%d.%d.%d.%d", (BYTE)p->h_addr_list[0][0], (BYTE)p->h_addr_list[0][1],(BYTE)p->h_addr_list[0][2], (BYTE)p->h_addr_list[0][3]);
		STRNCPY_MEMSET(m_PrivateIP, ip, SIZE_MAX_IPADDRESS);
	}
}

BOOL CAtumLauncherDlg::DestroyWindow()
{
	// TODO: Add your specialized code here and/or call the base class
	return CDialog::DestroyWindow();
}

void CAtumLauncherDlg::OnTimer(UINT nIDEvent)
{
	// TODO: Add your message handler code here and/or call default


	if (nIDEvent == TIMERID_CONNECT_PRESERVER)
	{
		KillTimer(TIMERID_CONNECT_PRESERVER);

		BOOL ret = ConnectPreServer();

		if (ret)
		{
			// succeeded
			m_SelectFlag = TRUE;
//			m_nServerGroupReg = SERVER_DECA;
			m_nServer = SERVER_DECA;

//ysw : »čÁ¦
			// Set default radio button
/*			int ServerGroup = IDC_DECA_SERVER;
			if ( m_nServerGroupReg == SERVER_DECA )
			{
				ServerGroup = IDC_DECA_SERVER;
			}
			else if ( m_nServerGroupReg == SERVER_BATTALUS )
			{
				ServerGroup = IDC_BATTALUS_SERVER;
			}
			else if ( m_nServerGroupReg == SERVER_SHARRINE )
			{
				ServerGroup = IDC_SHARRINE_SERVER;
			}
			else if ( m_nServerGroupReg == SERVER_PHILON )
			{
				ServerGroup = IDC_PHILON_SERVER;
			}
*/
		#ifdef _DEBUG
//			CButton* pBtn= (CButton* )GetDlgItem(ServerGroup);
//			pBtn->SetCheck( TRUE );
		#else
//			CButton* pBtn= (CButton* )GetDlgItem(IDC_BATTALUS_SERVER);
//			pBtn->SetCheck( TRUE );

		//ysw : »čÁ¦
//			GetDlgItem(IDC_DECA_SERVER)->EnableWindow(FALSE);
//			GetDlgItem(IDC_BATTALUS_SERVER)->EnableWindow(FALSE);
//			GetDlgItem(IDC_SHARRINE_SERVER)->EnableWindow(FALSE);
//			GetDlgItem(IDC_PHILON_SERVER)->EnableWindow(FALSE);
		#endif

			CString txt = m_CurrentVersion.GetVersionString();
			SetFileNumText((char*)(LPCTSTR)("VER " + txt));

			// set account name
// 2005-08-22 by cmkwon, for jpn alpha test
// 2007-03-06 by cmkwon, for debug
//#if defined(_DEBUG) || (  !defined(_MASANG15_SERVER) && (!defined(_MASANG51_SERVER)) && (!defined(_GLOBAL_ENG_SERVER)) && (!defined(_VTC_VIET_SERVER)) && (!defined(_KOREA_SERVER_2))  )

// 2008-06-17 by cmkwon, WinnerOnline_Tha LauncherżˇĽ­ ŔĚŔü Á˘ĽÓ °čÁ¤ ±âľďÇĎ±â(K0000243) - ľĆ·ˇżÍ °°ŔĚ ĽöÁ¤ ÇÔ
// #if defined(_DEBUG)
// 			GetDlgItem(IDC_EDIT_ACCOUNT)->SetWindowText(m_szAccountNameReg);
// 			GetDlgItem(IDC_EDIT_PASSWORD)->SetWindowText(m_szPasswordReg);
// 			((CEdit*)GetDlgItem(IDC_EDIT_ACCOUNT))->SetSel(0, -1);
// #endif

#if defined(_DEBUG)
			// 2008-06-17 by cmkwon, WinnerOnline_Tha LauncherżˇĽ­ ŔĚŔü Á˘ĽÓ °čÁ¤ ±âľďÇĎ±â(K0000243) - 
			GetDlgItem(IDC_EDIT_ACCOUNT)->SetWindowText(m_szAccountNameReg);
			GetDlgItem(IDC_EDIT_PASSWORD)->SetWindowText(m_szPasswordReg);

// 2008-10-20 by cmkwon, Gameforge4D(Engilsh, German)żˇ Á˘ĽÓ °čÁ¤ ±âľďÇĎ±â ±â´É Ăß°ˇ - ľĆ·ˇżÍ °°ŔĚ ĽöÁ¤
//#elif defined(SERVICE_TYPE_KOREAN_SERVER_1) || defined(SERVICE_TYPE_THAI_SERVER_1)		// 2008-06-17 by cmkwon, WinnerOnline_Tha LauncherżˇĽ­ ŔĚŔü Á˘ĽÓ °čÁ¤ ±âľďÇĎ±â(K0000243) - 
// 2008-12-22 by cmkwon, Áöżř Ľ­şń˝ş Ăß°ˇ(Gameforge4D ĹÍĹ°ľĆ, şŇľî, ŔĚĹ»¸®ľĆľî) - ľĆ·ˇżÍ °°ŔĚ 3°ł Ľ­şń˝ş Ăß°ˇ
//#elif defined(SERVICE_TYPE_KOREAN_SERVER_1) || defined(SERVICE_TYPE_THAI_SERVER_1) || defined(SERVICE_TYPE_ENGLISH_SERVER_2) || defined(SERVICE_TYPE_GERMAN_SERVER_1)		// 2008-06-17 by cmkwon, WinnerOnline_Tha LauncherżˇĽ­ ŔĚŔü Á˘ĽÓ °čÁ¤ ±âľďÇĎ±â(K0000243) - 
// 2009-06-04 by cmkwon, Áöżř Ľ­şń˝ş Ăß°ˇ(Gameforge4D Ćú¶őµĺľî, ˝şĆäŔÎľî) -  
//#elif defined(SERVICE_TYPE_KOREAN_SERVER_1) || defined(SERVICE_TYPE_THAI_SERVER_1) || defined(SERVICE_TYPE_ENGLISH_SERVER_2) || defined(SERVICE_TYPE_GERMAN_SERVER_1) || defined(SERVICE_TYPE_TURKISH_SERVER_1) || defined(SERVICE_TYPE_FRENCH_SERVER_1) || defined(SERVICE_TYPE_ITALIAN_SERVER_1)
// 2010-11-01 by shcho,	 Áöżř Ľ­şń˝ş Ăß°ˇ(Gameforge4D ˝şĆäŔÎľî, ľĆ¸ŁÇîĆĽłŞľî) -  
//#elif defined(SERVICE_TYPE_KOREAN_SERVER_1) || defined(SERVICE_TYPE_THAI_SERVER_1) || defined(SERVICE_TYPE_ENGLISH_SERVER_2) || defined(SERVICE_TYPE_GERMAN_SERVER_1) || defined(SERVICE_TYPE_TURKISH_SERVER_1) || defined(SERVICE_TYPE_FRENCH_SERVER_1) || defined(SERVICE_TYPE_ITALIAN_SERVER_1) || defined(SERVICE_TYPE_POLISH_SERVER_1) || defined(SERVICE_TYPE_SPANISH_SERVER_1)
#elif defined(SERVICE_TYPE_KOREAN_SERVER_1)|| defined(SERVICE_TYPE_ENGLISH_SERVER_1) || defined(SERVICE_TYPE_THAI_SERVER_1) || defined(SERVICE_TYPE_ENGLISH_SERVER_2) || defined(SERVICE_TYPE_GERMAN_SERVER_1) || defined(SERVICE_TYPE_TURKISH_SERVER_1) || defined(SERVICE_TYPE_FRENCH_SERVER_1) || defined(SERVICE_TYPE_ITALIAN_SERVER_1) || defined(SERVICE_TYPE_POLISH_SERVER_1) || defined(SERVICE_TYPE_SPANISH_SERVER_1) || defined(SERVICE_TYPE_ARGENTINA_SERVER_1) || defined(SERVICE_TYPE_SINGAPORE_1) || defined(SERVICE_TYPE_INDONESIA_SERVER_1)
			// 2008-06-17 by cmkwon, WinnerOnline_Tha LauncherżˇĽ­ ŔĚŔü Á˘ĽÓ °čÁ¤ ±âľďÇĎ±â(K0000243) - ŔúŔĺµČ AccountName Ŕ» ĽłÁ¤ÇŃ´Ů.
			GetDlgItem(IDC_EDIT_ACCOUNT)->SetWindowText(m_szAccountNameReg);
#ifdef _INET_CONFIGURATOR
			if (GetPrivateProfileInt(_GRAPH_MISC, "RememberPass", 0, _INI_FILE_NAME) == 1)
				GetDlgItem(IDC_EDIT_PASSWORD)->SetWindowText(m_szPasswordReg);
#endif
#endif
			
#if defined(SERVICE_TYPE_JAPANESE_SERVER_1)
			// 2008-12-18 by cmkwon, ŔĎş» Arario ·±Ăł ĽöÁ¤ - °čÁ¤ ĽłÁ¤ÇĎ±â
			GetDlgItem(IDC_EDIT_ACCOUNT)->SetWindowText(m_szAccountNameReg);
#endif 
	//		m_szAccountNameReg = "°řÁö»çÇ×\r\nTestÁß...\r\nľ÷µĄŔĚĆ® Á¤ş¸\r\nżěÁÖ¸Ę Ăß°ˇ\r\nľĆŔĚĹŰ Ăß°ˇ\r\n66\r\n77\r\n88\r\n99\r\n00\r\n00\r\n11\r\n22\r\n33\r\n44\r\n55\r\n";
	//		GetDlgItem(IDC_NOTICE)->SetWindowText(m_szAccountNameReg);


		}
		else
		{
			// failed
			EndDialog(-1);
			return;
		}
	}
	else if (nIDEvent == TIMERID_ENABLE_CONTROL)
	{
		KillTimer(nIDEvent);
		EnableControls();
	}
	else if(nIDEvent == TIMERID_SEND_ALIVE_PACKET)
	{
		if(m_pUpdateWinsocket
			&& m_pUpdateWinsocket->IsConnected())
		{
			m_pUpdateWinsocket->WriteMessageType(T_PC_CONNECT_ALIVE);
		}
	}
	else if(TIMERID_NETWORK_STATE_CHECK == nIDEvent)
	{// 2007-06-18 by cmkwon, ł×Ć®żöĹ© »óĹÂ ĂĽĹ©

		DWORD dwPingGapTick = 0;
		if(this->NTCheckTimeOver(&dwPingGapTick))
		{
			// 2007-06-18 by cmkwon, ł×Ć®żöĹ© »óĹÂ ĂĽĹ© - Č­¸éżˇ ÇĄ˝Ă
			m_ServerList->UpdateNetworkState(this->NTGetPingAverageTime());		// 2007-06-22 by cmkwon, Áß±ą ł×Ć®żöĹ© »óĹÂ ş¸ż©ÁÖ±â -
			m_ServerList->Invalidate();											// 2007-06-22 by cmkwon, Áß±ą ł×Ć®żöĹ© »óĹÂ ş¸ż©ÁÖ±â -
		}
	}

	CDialog::OnTimer(nIDEvent);
}


//ysw : Ăß°ˇ
void CAtumLauncherDlg::OnJoin()
{
	// TODO: Add your control notification handler code here
	//CString strTmp;
	//strTmp.Format("%s/%s", STRMSG_S_GAMEHOMEPAGE_DOMAIN, URL_REGISTER_PAGE);
	//ShellExecute(NULL, "open", strTmp, NULL, NULL, SW_SHOWNORMAL);
}

void CAtumLauncherDlg::OnSecret()
{
	// TODO: Add your control notification handler code here
	//WinExec("explorer.exe http://www.atumonline.com/7_members/login.asp ", SW_SHOW);
}

void CAtumLauncherDlg::OnEnd()
{
	// TODO: Add your control notification handler code here
	CDialog::DestroyWindow();
}

void CAtumLauncherDlg::OnDeca()
{
	// TODO: Add your control notification handler code here
	m_SelectFlag = TRUE;
	m_nServer = SERVER_DECA;
}

void CAtumLauncherDlg::OnBattal()
{
	// TODO: Add your control notification handler code here

}

void CAtumLauncherDlg::OnSharin()
{
	// TODO: Add your control notification handler code here

}

void CAtumLauncherDlg::OnPhilon()
{
	// TODO: Add your control notification handler code here

}

#define CUR_BAR_X		170
#define CUR_BAR_Y		41
#define CUR_BAR_WIDTH	258
#define CUR_BAR_HEIGHT	20

/*--------------------------------------------------------------------
	 DrawProgressBar
--------------------------------------------------------------------*/
int CAtumLauncherDlg::DrawProgressBar()
{
	CClientDC	dc(this);
	CDC			MemDC;
	CBitmap		* pOldBmp;
	CBrush		BlueBrush(RGB(0, 84, 166));
	CBrush		GrayBrush(RGB(109, 207, 246));

	CRect		ProRect;
	CBitmap		Percent;

	char		 Junk[10];
	TEXTMETRIC	 TextMetric;

	MemDC.CreateCompatibleDC(&dc);
	Percent.CreateCompatibleBitmap(&dc, CUR_BAR_WIDTH, CUR_BAR_HEIGHT);
	pOldBmp = (CBitmap *)MemDC.SelectObject(&Percent);

	ProRect.SetRect(0, 0, CUR_BAR_WIDTH, CUR_BAR_HEIGHT);
	MemDC.FillRect(&ProRect, &GrayBrush);
	ProRect.SetRect(0, 0, m_Cur_Rect , CUR_BAR_HEIGHT);
	MemDC.FillRect(&ProRect, &BlueBrush);

	MemDC.SetBkMode(TRANSPARENT);
	MemDC.SetTextColor(RGB(0, 0, 0));
	MemDC.GetTextMetrics(&TextMetric);

	wsprintf(Junk, "%d%%", m_Cur_Percent);
	MemDC.TextOut(CUR_BAR_WIDTH/2 - (TextMetric.tmAveCharWidth * lstrlen(Junk)/2),
				CUR_BAR_HEIGHT/2 - (TextMetric.tmHeight/2), (LPCSTR)Junk, lstrlen(Junk));

	dc.BitBlt(CUR_BAR_X, CUR_BAR_Y, CUR_BAR_WIDTH, CUR_BAR_HEIGHT, &MemDC, 0, 0, SRCCOPY);

	MemDC.SelectObject(&pOldBmp);

	return	true;
}


/*----------------------------------------------------------------------------*
*			Set_Cur_Percent
*-----------------------------------------------------------------------------*/
void CAtumLauncherDlg::Set_Cur_Percent(DWORD CurSize)
{
	m_Cur_Percent = CurSize;
	if(m_Cur_Percent > 100) m_Cur_Percent = 100;

	m_Cur_Rect = (int)( (m_Cur_Percent*CUR_BAR_WIDTH)/100 );
}

void CAtumLauncherDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (ACETR_NAV_HOME_RECT.PtInRect(point) || ACETR_NAV_NEWS_RECT.PtInRect(point) ||
		ACETR_NAV_EVENTS_RECT.PtInRect(point) || ACETR_NAV_WEB_RECT.PtInRect(point) ||
		ACETR_NAV_DISCORD_RECT.PtInRect(point))
		return;

	CDialog::OnLButtonDown(nFlags, point);
	PostMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x, point.y));
}

void CAtumLauncherDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (ACETR_NAV_HOME_RECT.PtInRect(point)) { SetLauncherMainPage(0); return; }
	if (ACETR_NAV_NEWS_RECT.PtInRect(point)) { SetLauncherMainPage(1); return; }
	if (ACETR_NAV_EVENTS_RECT.PtInRect(point)) { SetLauncherMainPage(2); return; }
	if (ACETR_NAV_WEB_RECT.PtInRect(point)) { SetLauncherMainPage(3); return; }
	if (ACETR_NAV_DISCORD_RECT.PtInRect(point)) { SetLauncherMainPage(4); return; }
	CDialog::OnLButtonUp(nFlags, point);
}

void CAtumLauncherDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	int hover=0;
	if (ACETR_NAV_HOME_RECT.PtInRect(point)) hover=1;
	else if (ACETR_NAV_NEWS_RECT.PtInRect(point)) hover=2;
	else if (ACETR_NAV_EVENTS_RECT.PtInRect(point)) hover=3;
	else if (ACETR_NAV_WEB_RECT.PtInRect(point)) hover=4;
	else if (ACETR_NAV_DISCORD_RECT.PtInRect(point)) hover=5;

	if (hover!=m_nModernNavHover)
	{
		m_nModernNavHover=hover;
		InvalidateRect(CRect(10,125,245,445),FALSE);
	}

	if (!m_bModernNavTracking)
	{
		TRACKMOUSEEVENT tme={0};
		tme.cbSize=sizeof(tme);
		tme.dwFlags=TME_LEAVE;
		tme.hwndTrack=m_hWnd;
		if (_TrackMouseEvent(&tme)) m_bModernNavTracking=TRUE;
	}
	CDialog::OnMouseMove(nFlags,point);
}

LRESULT CAtumLauncherDlg::OnModernNavMouseLeave(WPARAM, LPARAM)
{
	m_bModernNavTracking=FALSE;
	if(m_nModernNavHover!=0)
	{
		m_nModernNavHover=0;
		InvalidateRect(CRect(10,125,245,445),FALSE);
	}
	return 0;
}

BOOL CAtumLauncherDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	POINT pt; ::GetCursorPos(&pt); ScreenToClient(&pt); CPoint point(pt);
	if (ACETR_NAV_HOME_RECT.PtInRect(point) || ACETR_NAV_NEWS_RECT.PtInRect(point) ||
		ACETR_NAV_EVENTS_RECT.PtInRect(point) || ACETR_NAV_WEB_RECT.PtInRect(point) ||
		ACETR_NAV_DISCORD_RECT.PtInRect(point))
	{
		::SetCursor(::LoadCursor(NULL,IDC_HAND));
		return TRUE;
	}
	return CDialog::OnSetCursor(pWnd,nHitTest,message);
}

UINT CAtumLauncherDlg::OnNcHitTest(CPoint point)
{
     UINT nHitTest = CDialog::OnNcHitTest( point );
     // also fake windows out, but this maximizes the window
     // when you double click on it.
     return (nHitTest == HTCLIENT) ? HTCAPTION : nHitTest;
}


HBRUSH CAtumLauncherDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO: Change any attributes of the DC here
	switch(nCtlColor)
	{
	case CTLCOLOR_EDIT:
		{		
			if((GetDlgItem(IDC_EDIT_ACCOUNT)->m_hWnd == pWnd->m_hWnd)
				|| (GetDlgItem(IDC_EDIT_PASSWORD)->m_hWnd == pWnd->m_hWnd))
			{
				pDC->SetBkMode(OPAQUE);
				pDC->SetBkColor(RGB(22, 25, 34));
				pDC->SetTextColor(RGB(245, 247, 251));
				return m_StaticBrushBlack;
			}
		}
		break;
		
	case CTLCOLOR_STATIC:
		{
			if(GetDlgItem(IDC_FILE_INFO)->m_hWnd == pWnd->m_hWnd)
			{
				pDC->SetBkMode(TRANSPARENT);
				pDC->SetTextColor(EXE2_BG_UPDATE_INFO_FONT_COLOR);		
				return (HBRUSH)GetStockObject(NULL_BRUSH);
			}
			else if(GetDlgItem(IDC_DOWNLOAD_FILENUM)->m_hWnd == pWnd->m_hWnd)
			{
				pDC->SetBkMode(TRANSPARENT);
				pDC->SetTextColor(EXE2_BG_DOWNLOAD_FILE_FONT_COLOR);		
				return (HBRUSH)GetStockObject(NULL_BRUSH);
			}
			else if(GetDlgItem(IDC_CHARACTER_NAME)->m_hWnd == pWnd->m_hWnd )
			{
				pDC->SetBkMode(TRANSPARENT);
				pDC->SetBkColor(RGB(0, 0, 0));
				pDC->SetTextColor(RGB(255, 255, 255));		
				return m_StaticBrushGray;
			}
			else if (GetDlgItem(IDC_PLAYER_CNT)->m_hWnd == pWnd->m_hWnd) {
				pDC->SetBkMode(TRANSPARENT);
				pDC->SetTextColor(RGB(255, 255, 255));
				return (HBRUSH)GetStockObject(NULL_BRUSH);
			}
			else if (GetDlgItem(IDC_NOTICE2)->m_hWnd == pWnd->m_hWnd) {
				pDC->SetBkMode(TRANSPARENT);
				pDC->SetTextColor(RGB(255, 255, 255));
				return (HBRUSH)GetStockObject(NULL_BRUSH);
			}
		}
		break;
	case CTLCOLOR_LISTBOX:
		{
			if(GetDlgItem(IDC_LIST)->m_hWnd == pWnd->m_hWnd )
			{
				pDC->SetBkMode(TRANSPARENT);
				pDC->SetBkColor(RGB(29, 29, 40));
				pDC->SetTextColor(RGB(189,194,198));
				return m_listBrush;
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

// 2009-01-30 by cmkwon, ·Ż˝ĂľĆ Innova ·±Ăł ˝Ă˝şĹŰ(ÇÁ·Î˝şĆ®) ĽöÁ¤ - ľĆ·ˇżÍ °°ŔĚ ĽöÁ¤ÇÔ.
//void CAtumLauncherDlg::ExecGame(char *cmdLine)
void CAtumLauncherDlg::ExecGame(char *cmdLine, char *i_szAppPath/*=NULL*/, char *i_szCmdParam/*=NULL*/)
{
#ifdef _ATUM_DEVELOP
	SetCurrentDirectory(m_szExecutePathReg);
#endif

// 2009-07-10 by cmkwon, ·Ż˝ĂľĆ Frost ˝Ĺ±Ô Lib·Î ĽöÁ¤ - ±âÁ¸ ĽŇ˝ş ÁÖĽ® Ăł¸®
// // 2009-01-30 by cmkwon, ·Ż˝ĂľĆ Innova ·±Ăł ˝Ă˝şĹŰ(ÇÁ·Î˝şĆ®) ĽöÁ¤ - ľĆ·ˇżÍ °°ŔĚ ·Ż˝ĂľĆ¸¸ µű·Î Ăł¸®, AyyoCreateProcess() ČŁĂâ Ăł¸®
// // 2009-04-23 by cmkwon, ·Ż˝ĂľĆ Innova Frost ŔÓ˝Ă Á¦°ĹÇĎ±â - ľĆ·ˇżÍ °°ŔĚ define Ŕ¸·Î Ăł¸® ÇÔ.
// //#if defined(SERVICE_TYPE_RUSSIAN_SERVER_1)
// #if defined(SERVICE_TYPE_RUSSIAN_SERVER_1) && defined(_USING_INNOVA_FROST_)
// 	if(NULL == i_szAppPath
// 		|| NULL == i_szCmdParam)
// 	{
// 		AtumMessageBox(STRERR_S_ATUMLAUNCHER_0032);
// 		return;
// 	}
// 
// 
// 	char szFullParm[MAX_PATH];
// 	MEMSET_ZERO(szFullParm, MAX_PATH);
// 	_snprintf(szFullParm, MAX_PATH-1, "%s %s", i_szAppPath, i_szCmdParam);
// 	BOOL bReted = AyyoCreateProcess(i_szAppPath, szFullParm);
// 	if(FALSE == bReted)
// 	{
// 		AtumMessageBox(STRERR_S_ATUMLAUNCHER_0033);
// 		return;
// 	}
// 	return;
// #endif

#if defined(SERVICE_TYPE_VIETNAMESE_SERVER_1) || defined(SERVICE_TYPE_ARGENTINA_SERVER_1)
	///////////////////////////////////////////////////////////////////////////////
	// 2009-10-29 by cmkwon, şŁĆ®ł˛ X-TRAP ŔÚµżľ÷µĄŔĚĆ® ±â´É Ŕűżë - CAtumLauncherDlg::ExecGame#,
	XTrap_L_Patch(XTRAP_KEY_STRING, NULL, 60);
#endif

	UINT ret = WinExec(cmdLine, SW_SHOWNORMAL);
	DbgOut("EXE 2 CommandLine : %s, RetCode(%d)\r\n", cmdLine, ret);		// 2007-05-16 by cmkwon
	if ( ret <= 31 )	// exec failed...
	{
		switch (ret)
		{
		case 0:						// The system is out of memory or resources.
			AtumMessageBox(STRERR_S_ATUMLAUNCHER_0031);
			break;
		case ERROR_BAD_FORMAT:		// The .exe file is invalid.
			AtumMessageBox(STRERR_S_ATUMLAUNCHER_0032);
			break;
		case ERROR_FILE_NOT_FOUND:	// The specified file was not found.
#ifdef _ATUM_DEVELOP
			AtumMessageBox(CString("File not Found!\nbin: ") + cmdLine
				+ "\npath: " + m_szExecutePathReg);
#else
			AtumMessageBox(STRERR_S_ATUMLAUNCHER_0033);
#endif
			break;
		case ERROR_PATH_NOT_FOUND:	// The specified path was not found.
			AtumMessageBox(STRERR_S_ATUMLAUNCHER_0034);
			break;
		default:
			break;
		}
	}
}

void CAtumLauncherDlg::ExecGameCrocess(char *cmdLine)
{
	DWORD dwExitCode;
	PROCESS_INFORMATION pi;

	STARTUPINFO si = {sizeof(si)};
	ZeroMemory(&si,sizeof(si));

	CreateProcess(
		nullptr,				// name of executable module
		cmdLine,			// command line string
		nullptr,				//LPSECURITY_ATTRIBUTES lpProcessAttributes, // SD
		nullptr,				//LPSECURITY_ATTRIBUTES lpThreadAttributes,  // SD
		0,					//BOOL bInheritHandles,                      // handle inheritance option
		0,					//DWORD dwCreationFlags,                     // creation flags
		nullptr,				//LPVOID lpEnvironment,                      // new environment block
		nullptr,				//LPCTSTR lpCurrentDirectory,                // current directory name
		&si,				//LPSTARTUPINFO lpStartupInfo,               // startup information
		&pi					//LPPROCESS_INFORMATION lpProcessInformation // process information
		);


	GetExitCodeProcess(pi.hProcess, &dwExitCode);
	if (WaitForSingleObject(pi.hProcess, INFINITE) == WAIT_OBJECT_0) {
		// Process Áľ·á
		//AtumMessageBox("Process Terminated!");
	}

	CloseHandle( pi.hThread );
	CloseHandle( pi.hProcess );
}

BOOL CAtumLauncherDlg::ProcessDeleteFileList(MSG_PC_CONNECT_SINGLE_FILE_UPDATE_INFO *pUpdateInfo)
{
#ifdef _DEBUG
	VersionInfo DVersion(pUpdateInfo->NewDeleteFileListVersion);
	DBGOUT(STRMSG_S_ATUMLAUNCHER_0002, DVersion.GetVersionString());
#endif

	HINTERNET hInternet;
	HINTERNET hFtpConnect;
	HINTERNET hDir;
	HINTERNET hDelFile;
	WIN32_FIND_DATA pDirInfo;
	int nFileSize;

	hInternet = InternetOpen("Atum Pre Server", INTERNET_OPEN_TYPE_DIRECT, nullptr, nullptr, 0);
	if (hInternet == nullptr)
	{
		// check: error
		// 2005-01-13 by cmkwon, DeleteFile, NoticeFile FTP ż¬°á °ü·Ă żŔ·ů´Â ¸Ţ˝ĂÁöąÚ˝ş »čÁ¦
		//AtumMessageBox("InternetOpen ERROR!");
		DbgOut("InternetOpen ERROR: %d", GetLastError());
		return FALSE;
	}

	DbgOut("Updating delete file list(%s) From %s:%d \r\n", pUpdateInfo->DeleteFileListDownloadPath, pUpdateInfo->FtpIP, pUpdateInfo->FtpPort);

	hFtpConnect = InternetConnect(hInternet, pUpdateInfo->FtpIP, pUpdateInfo->FtpPort, pUpdateInfo->FtpAccountName, pUpdateInfo->FtpPassword, INTERNET_SERVICE_FTP, INTERNET_FLAG_PASSIVE, 0);
	if (hFtpConnect == nullptr)
	{
		// check: error
		// 2005-01-13 by cmkwon, DeleteFile, NoticeFile FTP ż¬°á °ü·Ă żŔ·ů´Â ¸Ţ˝ĂÁöąÚ˝ş »čÁ¦
		//AtumMessageBox("InternetConnect ERROR!");
		DbgOut("InternetConnect ERROR: %d\r\n", GetLastError());
		return FALSE;
	}

	if ( !(hDir = FtpFindFirstFile (hFtpConnect, pUpdateInfo->DeleteFileListDownloadPath, &pDirInfo, 0, 0) ) )
	{
		DWORD dwLastErr = GetLastError();
		if (dwLastErr == ERROR_NO_MORE_FILES)
		{
			DbgOut("No more files here\r\n");
			return FALSE;
		}
		else if (dwLastErr == ERROR_INTERNET_EXTENDED_ERROR)
		{
			DWORD dwError;
			TCHAR szErr[2048]; DWORD dwErrLen = 2048;
			if (InternetGetLastResponseInfo(&dwError, szErr, &dwErrLen))
			{
				CString szErrStr;
				szErrStr.Format("DFile Error: %s\r\n", szErr);
				// 2005-01-13 by cmkwon, DeleteFile, NoticeFile FTP ż¬°á °ü·Ă żŔ·ů´Â ¸Ţ˝ĂÁöąÚ˝ş »čÁ¦
				//AtumMessageBox(szErrStr);
				DbgOut("%s\n", szErrStr);
			}
			return FALSE;
		}
		else
		{
			// check: error
			CString szErrStr;
			szErrStr.Format("DFile Error: %d", GetLastError());
			// 2005-01-13 by cmkwon, DeleteFile, NoticeFile FTP ż¬°á °ü·Ă żŔ·ů´Â ¸Ţ˝ĂÁöąÚ˝ş »čÁ¦
			//AtumMessageBox(szErrStr);
			DbgOut("%s\n", szErrStr);
			return FALSE;
		}
	}
	else
	{
		nFileSize = pDirInfo.nFileSizeLow;
	}

	// GUI ĽłÁ¤
	SetProgressGroupText(STRMSG_S_ATUMLAUNCHER_0003);
	m_progressCtrl.SetRange32(0, nFileSize);

	////////////////////////////
	// file buffer
	string delFileBuffer = "";

	////////////////////////////
	// open remote file
	hDelFile = FtpOpenFile(hFtpConnect, pUpdateInfo->DeleteFileListDownloadPath, GENERIC_READ, FTP_TRANSFER_TYPE_BINARY, NULL);
	char *buffer = new char[DOWNLOAD_BUFFER_SIZE];
	DWORD amount_read = DOWNLOAD_BUFFER_SIZE;
	UINT total_read = 0;

	while (TRUE)
	{
		if (!InternetReadFile (hDelFile, buffer, DOWNLOAD_BUFFER_SIZE, &amount_read))
		{
			// error
			// 2005-01-13 by cmkwon, DeleteFile, NoticeFile FTP ż¬°á °ü·Ă żŔ·ů´Â ¸Ţ˝ĂÁöąÚ˝ş »čÁ¦
			//AtumMessageBox("Reading file failed");
			return FALSE;
		}

		if(0 == amount_read)
		{// 2006-06-30 by cmkwon
			break;
		}

		delFileBuffer.append(buffer, amount_read);

		total_read += amount_read;
		m_progressCtrl.SetPos(total_read);
	}

	delFileBuffer += "\r\n";

	///////////////////////////
	// Ăł¸®
	istrstream isDel(delFileBuffer.c_str());
	char delFileLine[SIZE_MAX_FTP_FILE_PATH];
	int isOffset = 0;
	while(isOffset < nFileSize)
	{
		if (isDel.getline(delFileLine, SIZE_MAX_FTP_FILE_PATH).eof())
		{
			break;
		}

		isOffset += (strlen(delFileLine) + 1);

		// 0x0D¸¦ Á¦°ĹÇĎÁö ¸řÇŃ °ćżě Á¦°ĹÇÔ
		if (delFileLine[strlen(delFileLine)-1] == 0x0D)
		{
			delFileLine[strlen(delFileLine)-1] = '\0';
		}

		if (delFileLine[0] == '#')
		{
			// commentŔÓ
			continue;
		}

#ifdef _DEBUG
		char	szDir[512];
		GetCurrentDirectory(512, szDir);
#endif

		DeleteFile(delFileLine);
	}

	// delete buffer
	delete buffer;

	// close resources
	InternetCloseHandle(hDelFile);
	InternetCloseHandle(hDir);
	InternetCloseHandle(hFtpConnect);
	InternetCloseHandle(hInternet);

	return TRUE;
}

BOOL CAtumLauncherDlg::ProcessNoticeFile(MSG_PC_CONNECT_SINGLE_FILE_UPDATE_INFO *pUpdateInfo)
{
	char szDownLoadFileName[SIZE_MAX_FTP_FILE_PATH];
	char szDownLoadFileFullPath[SIZE_MAX_FTP_FILE_PATH];

	memset(szDownLoadFileName, 0x00, sizeof(szDownLoadFileName));
	memset(szDownLoadFileFullPath, 0x00, sizeof(szDownLoadFileFullPath));

	sprintf(szDownLoadFileName, "notice.txt");
	sprintf(szDownLoadFileFullPath, "%s%s", pUpdateInfo->NoticeFileDownloadPath, szDownLoadFileName);

	if (m_pHttpManager != nullptr) return FALSE;

	DbgOut("Updating notice file(%s) From %s:%d by Http, %s\r\n", szDownLoadFileName, pUpdateInfo->FtpIP, pUpdateInfo->FtpPort, szDownLoadFileFullPath);

	m_pHttpManager = new CHttpManager;

	SetFTPUpdateState(UPDATE_STATE_DOWNLOADING);
	Err_t errCode = m_pHttpManager->DownloadFileByHttp(pUpdateInfo->FtpIP, pUpdateInfo->FtpPort, szDownLoadFileFullPath, szDownLoadFileName, TRUE, this->GetSafeHwnd());
	if (ERR_NO_ERROR != errCode) {
		SAFE_DELETE(m_pHttpManager);
		this->SetFTPUpdateState(UPDATE_STATE_INIT);
		AtumMessageBox("Cannot download notice update!", STRMSG_WINDOW_TEXT, MB_OK | MB_ICONERROR);
		return FALSE;
	}
	bDownloadedNoticeFile = true;
	SetProgressGroupText((LPCSTR)(CString("Updater #1 => Updating notice file ")));
	m_pHttpManager->ThreadEnd(1000);
	SAFE_DELETE(m_pHttpManager);
	this->SetFTPUpdateState(UPDATE_STATE_INIT);

	return TRUE;

	BOOL bRet = FALSE;
	CFTPManager noticeFTPManager;

	DbgOut("Updating notice file(%s) From %s:%d \r\n", pUpdateInfo->NoticeFileDownloadPath, pUpdateInfo->FtpIP, pUpdateInfo->FtpPort);

	bRet = noticeFTPManager.ConnectToServer(pUpdateInfo->FtpIP, pUpdateInfo->FtpPort, pUpdateInfo->FtpAccountName, pUpdateInfo->FtpPassword);
	if (!bRet)
	{
		return FALSE;
	}

	// Find file info
	WIN32_FIND_DATA fileInfo;
	HINTERNET hFile = noticeFTPManager.GetFileInfo(pUpdateInfo->NoticeFileDownloadPath, &fileInfo);
	if (hFile == nullptr)
	{
		return FALSE;
	}

	// set file length
	int nFileSize = fileInfo.nFileSizeLow;

	// GUI ĽłÁ¤
	SetProgressGroupText(STRMSG_S_ATUMLAUNCHER_0004);
	m_progressCtrl.SetRange32(0, nFileSize);

	// ´Ůżî·Îµĺ
	bRet = noticeFTPManager.DownloadFile(pUpdateInfo->NoticeFileDownloadPath, "notice.txt", &m_progressCtrl);
	if (!bRet)
	{
		return FALSE;
	}

	noticeFTPManager.CloseConnection();

	return TRUE;
}

void CAtumLauncherDlg::OnSelchangeList()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE) ;
	int nSel = m_ctrlServerList.GetCurSel();
	if(m_ctrlServerList.GetServerCheck(nSel))
	{
		m_ctrlServerList.SetCurSel(nSel);
		m_nOldSel = nSel;
	}
	else
	{
		m_ctrlServerList.SetCurSel(m_nOldSel);
	}
}
void CAtumLauncherDlg::OnBtnCheck64b()
{
	CString t;
	t.Format(_T("%d"), m_ctrl64Bit.GetCheck());
	WritePrivateProfileString(_GRAPH_MISC, "Run64BitClient",t, _INI_FILE_NAME);		
}
void CAtumLauncherDlg::OnCan()
{
	// TODO: Add your control notification handler code here

	if(UPDATE_STATE_DOWNLOADING == m_FTPUpdateState
		|| UPDATE_STATE_DOWNLOADED == m_FTPUpdateState)
	{
		if(m_pUpdateFTPManager)
		{
			m_pUpdateFTPManager->m_bDownloadThreadCancelFlag = TRUE;
			if(m_pUpdateFTPManager->m_hDownloadThread)
			{
				DWORD dwRet;
				dwRet = WaitForSingleObject(m_pUpdateFTPManager->m_hDownloadThread, INFINITE);
				if(WAIT_OBJECT_0 != dwRet)
				{
					// ¸®ĹĎŔĚ WAIT_FAILEDŔÓ
					int nError = GetLastError();
					SetLastError(0);
				}
				CloseHandle(m_pUpdateFTPManager->m_hDownloadThread);
				m_pUpdateFTPManager->m_hDownloadThread = nullptr;				
				Sleep(100);
			}
			//SAFE_DELETE(m_pHost);
			delete m_pUpdateFTPManager;
			m_pUpdateFTPManager = nullptr;
			this->SetFTPUpdateState(UPDATE_STATE_INIT);
		}

		if(m_pHttpManager)
		{// 2007-01-08 by cmkwon
			m_pHttpManager->ThreadEnd(1000);
			SAFE_DELETE(m_pHttpManager);
			this->SetFTPUpdateState(UPDATE_STATE_INIT);
		}

		VersionInfo OldVersion(m_msg_PC_CONNECT_UPDATE_INFO.OldVersion);
		VersionInfo UpdateVersion(m_msg_PC_CONNECT_UPDATE_INFO.UpdateVersion);
		char szUpdateFileName[256];
		sprintf(szUpdateFileName, "%s_%s.zip", OldVersion.GetVersionString(),
			UpdateVersion.GetVersionString());
		DeleteFile(szUpdateFileName);
	}
	CDialog::OnCancel();
}

BOOL CAtumLauncherDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		if (m_bControlEnabled)
		{
			OnOk();
		}
		return TRUE;
	}

///////////////////////////////////////////////////////////////////////////////
// 2009-07-08 by cmkwon, ·Ż˝ĂľĆ ·±Ăł ĽöÁ¤ żäĂ»(¸¶żě˝şżěĹ¬¸Ż ¸·±â) - 	
#if defined(SERVICE_TYPE_RUSSIAN_SERVER_1)
	if(WM_RBUTTONDOWN == pMsg->message)
	{
		return TRUE;
	}
#endif

	return CDialog::PreTranslateMessage(pMsg);
}



LONG CAtumLauncherDlg::OnUpdateFileDownloadError(WPARAM wParam, LPARAM lParam)
{
	if(UPDATE_STATE_DOWNLOADING != m_FTPUpdateState){		return FALSE;}

	char szErrString[1024];
	char szErrAddStr[1024];
	MEMSET_ZERO(szErrString, 1024);
	MEMSET_ZERO(szErrAddStr, 1024);
// 2007-01-05 by cmkwon, ľĆ·ˇżÍ °°ŔĚ ERR_XXX·Î ĽöÁ¤ÇÔ
//	switch(wParam)
//	{
//	case DOWNLOAD_ERR_FTP_CONNECT:
//		{
//			strcpy(szErrAddStr, STRERR_S_ATUMEXE_0006);
//		}
//		break;
//	case DOWNLOAD_ERR_FTP_OPENFILE:
//		{
//			strcpy(szErrAddStr, STR_DOWNLOAD_ERR_FTPCONNECT);
//		}
//		break;
//	case DOWNLOAD_ERR_CREATE_LOCAL_FILE:
//		{
//			strcpy(szErrAddStr, STR_DOWNLOAD_ERR_CREATE_LOCAL_FILE);
//		}
//		break;
//	case DOWNLOAD_ERR_READ_REMOTE_FILE:
//		{
//			strcpy(szErrAddStr, STR_DOWNLOAD_ERR_READ_REMOTE_FILE);
//		}
//		break;
//	default:
//		{
//			wsprintf(szErrAddStr, "Nomal download error(%d) !!", wParam);
//		}
//	}
	switch(wParam)
	{
	case ERR_CANNOT_CONNECT_AUTO_UPDATE_SERVER:
		{
			strcpy(szErrAddStr, STRERR_S_ATUMEXE_0006);
		}
		break;
	case ERR_LOCAL_FILE_CREATE_FAIL:
		{
			strcpy(szErrAddStr, STRCMD_CS_COMMON_DOWNLOAD_0001);
		}
		break;
	case ERR_UPDATE_FILE_NOT_FOUND:
		{
			strcpy(szErrAddStr, STRCMD_CS_COMMON_DOWNLOAD_0000);
		}
		break;
	case ERR_UPDATE_FILE_DOWNLOADING_FAIL:	
		{
			strcpy(szErrAddStr, STRCMD_CS_COMMON_DOWNLOAD_0002);
		}
		break;
	default:
		{
			wsprintf(szErrAddStr, "Nomal download error(%d) !!", wParam);
		}
	}

	wsprintf(szErrString, STRMSG_060526_0001
		, STRMSG_S_GAMEHOMEPAGE_DOMAIN, szErrAddStr);
	SetProgressGroupText("Updater #1 => failed to autoupdate!");
	DownloadUpdateFileByHttp(ptmpMsg, true);
	/*AtumMessageBox("Auto update failed. Automatic client repair tool would be started to fix it!", "OSR Repair Client", MB_OK);
	if (WinExec("ClientRepairTool.exe", SW_SHOW) == ERROR_FILE_NOT_FOUND)
	{
		MessageBox("The ClientRepairTool was not found, it will be downloaded now.", "OldSchoolRivals Repair Client");
		HRESULT hr = URLDownloadToFile(NULL, _T("https://patch.oldschoolrivals.com/fullclient/ClientRepairTool.exe"), _T("ClientRepairTool.exe"), 0, NULL);
		WinExec("ClientRepairTool.exe", SW_SHOW);
	}*/
	//AtumMessageBox(szErrString);
	//OnCancel();
	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			LONG CAtumLauncherDlg::OnUpdateFileDownloadInit(WPARAM wParam, LPARAM lParam)
/// \brief		
/// \author		cmkwon
/// \date		2007-01-08 ~ 2007-01-08
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
LONG CAtumLauncherDlg::OnUpdateFileDownloadInit(WPARAM wParam, LPARAM lParam)
{
	m_progressCtrl.SetRange32(0, wParam);
	m_progressCtrl.SetPos(0);
	return TRUE;
}

LONG CAtumLauncherDlg::OnUpdateFileDownloadProgress(WPARAM wParam, LPARAM lParam)
{
	if(UPDATE_STATE_DOWNLOADING != m_FTPUpdateState){		return FALSE;}

	m_progressCtrl.SetPos(wParam);
	return TRUE;
}

LONG CAtumLauncherDlg::OnUpdateFileDownloadOK(WPARAM wParam, LPARAM lParam)
{
	if(UPDATE_STATE_DOWNLOADING != m_FTPUpdateState){		return FALSE;}
	this->SetFTPUpdateState(UPDATE_STATE_DOWNLOADED);

	if(m_pUpdateFTPManager)
	{
		if(m_pUpdateFTPManager->m_hDownloadThread)
		{
			DWORD dwRet;
			dwRet = WaitForSingleObject(m_pUpdateFTPManager->m_hDownloadThread, 1000);
			if(WAIT_OBJECT_0 != dwRet)
			{
				// ¸®ĹĎŔĚ WAIT_FAILEDŔÓ
				int nError = GetLastError();
				SetLastError(0);
			}
			CloseHandle(m_pUpdateFTPManager->m_hDownloadThread);
			m_pUpdateFTPManager->m_hDownloadThread = nullptr;

			Sleep(100);
		}

		delete m_pUpdateFTPManager;
		m_pUpdateFTPManager = nullptr;
		this->SetFTPUpdateState(UPDATE_STATE_INIT);
	}
	if(m_pHttpManager)
	{
		m_pHttpManager->ThreadEnd(1000);
		SAFE_DELETE(m_pHttpManager);		
		this->SetFTPUpdateState(UPDATE_STATE_INIT);
	}
	if (bDownloadedNoticeFile) {
		bDownloadedNoticeFile = false;
		return TRUE;
	}
	// extract update file
	ExtractUpdateFile(&m_msg_PC_CONNECT_UPDATE_INFO);
	
	// update completed
	VersionInfo OldVersion(m_msg_PC_CONNECT_UPDATE_INFO.OldVersion);
	VersionInfo UpdateVersion(m_msg_PC_CONNECT_UPDATE_INFO.UpdateVersion);
	char strBuffer[256];
	sprintf(strBuffer, STRMSG_S_ATUMLAUNCHER_0005, OldVersion.GetVersionString(),
		UpdateVersion.GetVersionString());
	SetProgressGroupText(strBuffer);
	
	// set current cliet version
	m_CurrentVersion = UpdateVersion;
	
	// On DownLoadGamefilesDone
	OnDownLoadGamefilesDone(NULL, NULL);

	memset(&m_msg_PC_CONNECT_UPDATE_INFO, 0x00, sizeof(MSG_PC_CONNECT_UPDATE_INFO));

	return TRUE;
}

void CAtumLauncherDlg::OnMin() 
{
	// TODO: Add your control notification handler code here
	this->ShowWindow(SW_MINIMIZE);
}


///////////////////////////////////////////////////////////////////////////////
/// \fn			void CAtumLauncherDlg::OnBtnHomepage() 
/// \brief		// 2007-09-27 by cmkwon, Homepage°ˇ±â ąöĆ° Ăß°ˇ(şŁĆ®ł˛ VTC-Intecom żäĂ») - 
/// \author		cmkwon
/// \date		2007-09-27 ~ 2007-09-27
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CAtumLauncherDlg::OnBtnHomepage() 
{
	OpenLauncherConfiguredUrl("Home");
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			char *CAtumLauncherDlg::GetPublicLocalIP(char *o_szLocalIP)
/// \brief		
/// \author		cmkwon
/// \date		2006-04-10 ~ 2006-04-10
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
char *CAtumLauncherDlg::GetPublicLocalIP(char *o_szLocalIP)
{
	GGetLocalIP(o_szLocalIP, IP_TYPE_PUBLIC);
	return o_szLocalIP;
}

void CAtumLauncherDlg::OnSetfocusEditAccount() 
{
	// TODO: Add your control notification handler code here
	/*if(nullptr == m_pScreenKeyboardDlg)
	{
		return;
	}*/

	m_pInputEditFromScreenKeyboard = GetDlgItem(IDC_EDIT_ACCOUNT);	
}

void CAtumLauncherDlg::OnSetfocusEditPassword() 
{
	// TODO: Add your control notification handler code here
	
	/*if(nullptr == m_pScreenKeyboardDlg)
	{
		return;
	}*/

	m_pInputEditFromScreenKeyboard = GetDlgItem(IDC_EDIT_PASSWORD);	

	///////////////////////////////////////////////////////////////////////////////
	// 2007-09-18 by cmkwon, Č­»óĹ°ş¸µĺ ĽöÁ¤ - Č­»óĹ°ş¸µĺ Ŕ©µµżěżˇĽ­ Hide ˝ĂĹł°ćżě Ăł¸®¸¦ Ŕ§ÇŘ
	//if(FALSE == m_bHideScreenKeyboardByScreenKeyboardWindow)
	//{
	//	// 2007-09-18 by cmkwon, Č­»óĹ°ş¸µĺ ĽöÁ¤ - ĆĐ˝şżöµĺ ŔÔ·Â˝Ăżˇ¸¸ Č°ĽşČ­ µČ´Ů.
	//	m_pScreenKeyboardDlg->ShowWindow(SW_SHOW);
	//}
	//m_bHideScreenKeyboardByScreenKeyboardWindow	= FALSE;
}
#ifdef _INET_CONFIGURATOR
void CAtumLauncherDlg::OnBtnViewInetConfigurator()
{
	if (nullptr == m_pInetCFG)
	{
		return;
	}

	m_pInetCFG->ShowWindow(SW_SHOW);
}
#endif
void CAtumLauncherDlg::OnBtnViewScreenKeyboard() 
{
	// TODO: Add your control notification handler code here

	//if(nullptr == m_pScreenKeyboardDlg)
	//{
	//	return;
	//}
	//
	//// 2007-09-11 by cmkwon, şŁĆ®ł˛ Č­¸éĹ°ş¸µĺ ±¸Çö - Č­¸é Ĺ°ş¸µĺ¸¦ ş¸ż©ÁŘ´Ů
	//m_pScreenKeyboardDlg->ShowWindow(SW_SHOW);
}

void CAtumLauncherDlg::OnCheckWindowsMode() 
{
	// TODO: Add your control notification handler code here
	UpdateData();

	CComboBox *pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_WINDOW_DEGREE_LAUNCHER);
	if(nullptr == pComboBox)
	{
		return;
	}

	CString csBeforeWDegree;
	pComboBox->GetWindowText(csBeforeWDegree);

	
	
	// 2008-12-17 by ckPark ·Ż˝ĂľĆ ·±ĂÄ
	// // ·Ż˝ĂľĆ´Â ĂĽĹ©ąÚ˝ş
#if defined(SERVICE_TYPE_RUSSIAN_SERVER_1)		// ·Ż˝ĂľĆ ·±Ăł ŔÎĹÍĆäŔĚ˝ş ĽöÁ¤
	this->InsertWindowDegreeList(pComboBox, m_ctrlCheckWindowMode.GetCheck());
#else
	this->InsertWindowDegreeList(pComboBox, m_ctlbWindowMode);
#endif
	// end 2008-12-17 by ckPark ·Ż˝ĂľĆ ·±ĂÄ




	int nIdx = this->FindWindowDegreeComboBoxIndex(pComboBox, (LPSTR)(LPCSTR)csBeforeWDegree);
	nIdx = max(0, nIdx);
	pComboBox->SetCurSel(nIdx);
}

BOOL CAtumLauncherDlg::BitmapRgn(LPCTSTR resource, COLORREF TansColor, int nx, int ny)
{
	HBITMAP			m_hBack;
	HINSTANCE hInstance = AfxGetInstanceHandle();

	HANDLE handle = ::LoadImage(hInstance, resource, IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS | LR_LOADFROMFILE);

	if (!handle) return FALSE;

	m_hBack = (HBITMAP)handle;
	::SetWindowRgn(m_hWnd, BitmapToRegion(m_hBack, TansColor, TansColor, nx, ny), TRUE);

	return TRUE;
}

BOOL CAtumLauncherDlg::BitmapRgn(UINT resource, COLORREF TansColor, int nx, int ny)
{
	HBITMAP			m_hBack;
	HINSTANCE hInstance = AfxGetInstanceHandle();
	m_hBack = (HBITMAP)LoadBitmap(hInstance, MAKEINTRESOURCE(resource));
	::SetWindowRgn(m_hWnd, BitmapToRegion(m_hBack, TansColor, TansColor, nx, ny), TRUE);

	return TRUE;
}
HRGN CAtumLauncherDlg::BitmapToRegion(HBITMAP hBmp, COLORREF cTransparentColor/* = 0*/, COLORREF cTolerance/* = 0x101010*/, int nx, int ny)
{
	HRGN hRgn = nullptr;

	if (hBmp)
	{
		// Create a memory DC inside which we will scan the bitmap content
		HDC hMemDC = CreateCompatibleDC(nullptr);
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
			HBITMAP hbm32 = CreateDIBSection(hMemDC, (BITMAPINFO *)&RGB32BITSBITMAPINFO, DIB_RGB_COLORS, &pbits32, nullptr, 0);
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
									HRGN h = ExtCreateRegion(nullptr, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects), pData);
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
					HRGN h = ExtCreateRegion(nullptr, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects), pData);
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