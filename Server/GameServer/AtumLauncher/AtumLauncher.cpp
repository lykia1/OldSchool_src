// AtumLauncher.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "AtumLauncher.h"
#include "AtumLauncherDlg.h"
#include "MGameDecryption.h"
#include "AtumError.h"				// 2006-10-02 by cmkwon
#include "RegistryControl.h"	// 2008-10-16 by cmkwon, Gameforge4D(Eng,Deu) Launcher Registry event ±¸Çö - 


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CAtumLauncherApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

CAtumLauncherApp::CAtumLauncherApp()
:CWinApp(REGISTRY_BASE_PATH)
{
	m_hMutexMonoInstance	= NULL;
}

CAtumLauncherApp theApp;

BOOL CAtumLauncherApp::InitInstance()
{

	m_hMutexMonoInstance = CreateMutex(NULL, TRUE, "AtumLauncher");
	if(NULL == m_hMutexMonoInstance)
	{
		MessageBox(NULL, "CreateMutex Error", "AtumLauncher", MB_OK);
		return FALSE;
	}

	if(ERROR_ALREADY_EXISTS == ::GetLastError())
	{
		MessageBox(NULL, "ERROR : \n  Application is running already...", "AtumLauncher", MB_OK);
		return FALSE;
	}

	srand(timeGetTime());
	AfxEnableControlContainer();
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	SetRegistryKey(STRMSG_REG_STRING_REGISTRYKEY_NAME);

	Err_t errCode = GSetExcuteParameterList(__argc, __argv);
	if(ERR_NO_ERROR != errCode)
	{
		DbgOut("Set Excute Parameter error !! Error = %s(0x%X)\r\n", GetErrorString(errCode), errCode);
		return FALSE;
	}

	SEXCUTE_PARAMETER exeParam;		MEMSET_ZERO(&exeParam, sizeof(exeParam));
	exeParam.i_nExcuteFileType		= EXCUTE_FILE_TYPE_SC_LAUNCHER_ATM;
	errCode = GCheckExcuteParameterList(&exeParam);
	if(ERR_NO_ERROR != errCode)
	{
		DbgOut("Check Excute Parameter error !! Error = %s(0x%X)\r\n", GetErrorString(errCode), errCode);
		return FALSE;
	}
	
	CAtumLauncherDlg dlg;
	m_pMainWnd = &dlg;
	ReadProfile();
	ReadCrocessProfile();
	if(0 != strcmp(exeParam.o_szPreServerIP0, ""))
	{// 2007-05-15 by cmkwon, PreServer IP¸¦ ˝ÇÇŕŔÎŔÚ·Î ąŢŔ˝
		dlg.m_szPreServerIPReg	= exeParam.o_szPreServerIP0;
	}

	dlg.m_nBirthYear			= exeParam.o_nBirthYear;
	dlg.m_ctlbWindowMode		= (dlg.m_nWindowModeReg == GAME_MODE_WINDOW) ? TRUE : FALSE;
	//dlg.m_n64Bit


	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

void CAtumLauncherApp::DisableXPServicePack2()
{
}

void CAtumLauncherApp::WriteProfile(CAtumLauncherDlg& instance)
{
	instance.m_szAccountNameReg.MakeLower();

	SREG_DATA_EXE_2 regDataExe2;
	regDataExe2.resetREG_DATA_EXE_2();
	regDataExe2.ClientVersion = instance.m_CurrentVersion;
	regDataExe2.NVersion = instance.m_CurrentNoticeVersion;
	regDataExe2.DVersion = instance.m_CurrentDelFileListVersion;

	STRNCPY_MEMSET(regDataExe2.WindowDegree, LPCSTR(instance.m_csWindowsResolutionReg), SIZE_MAX_WINDOW_DEGREE_NAME);	// 2007-12-27 by cmkwon, À©µµ¿ìÁî ¸ðµå ±â´É Ãß°¡ -

	if (instance.m_ctlBtnRememberID.GetCheck())
		STRNCPY_MEMSET(regDataExe2.BeforeAccountName, instance.m_szAccountNameReg, SIZE_MAX_ACCOUNT_NAME);
	
	STRNCPY_MEMSET(regDataExe2.BeforePassword, instance.m_szPasswordReg, SIZE_MAX_PASSWORD);
	
	STRNCPY_MEMSET(regDataExe2.SelectedServerGroupName, LPCSTR(instance.m_strServerGroupName), SIZE_MAX_SERVER_NAME);


	regDataExe2.IsWindowMode = instance.m_ctlbWindowMode;

	regDataExe2.Is64Bit = instance.m_ctrl64Bit.GetCheck();

	GSaveExe2VersionInfo(&regDataExe2, STRMSG_VERSION_INFO_FILE_NAME);
}

void CAtumLauncherApp::ReadProfile()
{
	// registry path: HKEY_CURRENT_USER\Software\Atum Online\Atum Launcher\Configuration
	CAtumLauncherDlg *pDlg = (CAtumLauncherDlg*)AfxGetMainWnd();

	if(pDlg)
	{
		SREG_DATA_EXE_2 regDataExe2;
		regDataExe2.resetREG_DATA_EXE_2();

		Err_t errCode = GLoadExe2VersionInfo(&regDataExe2, STRMSG_VERSION_INFO_FILE_NAME);
		if(ERR_NO_ERROR != errCode
			|| FALSE == regDataExe2.ClientVersion.IsValidVersionInfo())
		{
			regDataExe2.ClientVersion.SetVersion((char*)(LPCSTR)GetProfileString( _T("Configuration"), STRMSG_REG_STRING_CLIENT_VERSION, "0.0.0.0"));
		}

		if(ERR_NO_ERROR != errCode
			|| 0 == strncmp(regDataExe2.WindowDegree, "", SIZE_MAX_WINDOW_DEGREE_NAME))
		{
			CString csWDegree = GetProfileString(_T("Configuration"), STRMSG_REG_KEY_NAME_WINDOWDEGREE_NEW, STRMSG_WINDOW_DEGREE_1024x768_MEDIUM);
			STRNCPY_MEMSET(regDataExe2.WindowDegree, (char*)(LPCSTR)csWDegree, SIZE_MAX_WINDOW_DEGREE_NAME);
		}

		if(ERR_NO_ERROR != errCode
			|| 0 == strncmp(regDataExe2.BeforeAccountName, "", SIZE_MAX_ACCOUNT_NAME))
		{
			CString strTm;
			strTm = GetProfileString( _T("Configuration"), _T("AccountName"), NULL);
			strTm.MakeLower();
			sprintf(regDataExe2.BeforeAccountName, "%s", strTm);
		}
#ifdef _INET_CONFIGURATOR
		if (ERR_NO_ERROR != errCode
			|| 0 == strncmp(regDataExe2.BeforePassword, "", SIZE_MAX_PASSWORD))
		{
			CString strTm;
			strTm = GetProfileString(_T("Configuration"), _T("Password"), NULL);
			sprintf(regDataExe2.BeforePassword, "%s", strTm);
		}
#endif
		
		if(ERR_NO_ERROR != errCode
			|| 0 == strncmp(regDataExe2.SelectedServerGroupName, "", SIZE_MAX_SERVER_NAME))
		{
			CString strTm;
			strTm = GetProfileString( _T("Configuration"), _T("ServerGroupName"), "");
			sprintf(regDataExe2.SelectedServerGroupName, "%s", strTm);
		}

		pDlg->m_CurrentVersion				= regDataExe2.ClientVersion;
		pDlg->m_csWindowsResolutionReg		= regDataExe2.WindowDegree;
		pDlg->m_szAccountNameReg			= regDataExe2.BeforeAccountName;
#ifdef _INET_CONFIGURATOR
		pDlg->m_szPasswordReg = regDataExe2.BeforePassword;
#endif
		pDlg->m_strServerGroupName			= regDataExe2.SelectedServerGroupName;
		pDlg->m_CurrentDelFileListVersion.SetVersion("0.0.0.0");
		pDlg->m_CurrentNoticeVersion.SetVersion("0.0.0.0");

		///////////////////////////////////////////////////////////////////////////////
		// 2008-01-03 by cmkwon, Ŕ©µµżě¸đµĺ »óĹÂ ŔúŔĺÇĎ±â - 
		if(regDataExe2.IsWindowMode)
		{
			pDlg->m_nWindowModeReg = GAME_MODE_WINDOW;					// 2008-01-03 by cmkwon, Ŕ©µµżě¸đµĺ »óĹÂ ŔúŔĺÇĎ±â - 
		}
		else
		{
			pDlg->m_nWindowModeReg = GAME_MODE_FULLSCREEN;					// 2007-05-09 by cmkwon, Ç×»ó FullScreenMode
		}
		pDlg->m_n64Bit = regDataExe2.Is64Bit;
		
		char szPreServer[1024];
		MEMSET_ZERO(szPreServer, 1024);
		XOR::XORDecrypt(szPreServer, CHOICE_PRE_SERVER_IP_OR_DOMAIN_IN_XOR, STR_XOR_KEY_STRING_PRE_SERVER_ADDRESS);
		pDlg->m_szPreServerIPReg	= szPreServer;

		char szExecute[512];
		sprintf(szExecute, ".\\%s", CLIENT_EXEUTE_FILE_NAME);

		pDlg->m_bGuardAgreementReg	= GetProfileInt(_T("Configuration"), "GameGuardAgreement", FALSE);
	}
}

void CAtumLauncherApp::ReadCrocessProfile()
{
	CAtumLauncherDlg *pDlg = (CAtumLauncherDlg*)AfxGetMainWnd();

	if(pDlg)
	{
		pDlg->m_szCrocessSuffix = "";
	}
	else
	{
		return;
	}

	HKEY hKey = NULL;
    TCHAR szSuffix[16];
    DWORD dwBufLen = 16;
    LONG lRet;

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("software\\Gameinfinity\\Install game info.\\c34"), 0, KEY_QUERY_VALUE,
		&hKey) == ERROR_SUCCESS)
	{
		lRet = RegQueryValueEx(hKey,
							TEXT("identifier"),
							NULL,
							NULL,
							(LPBYTE)szSuffix,
							&dwBufLen);

		if (lRet == ERROR_SUCCESS)
		{
			pDlg->m_szCrocessSuffix = szSuffix;
		}
		else
		{
			pDlg->m_szCrocessSuffix = "";
		}

		RegCloseKey(hKey);
	}
	else
	{
		pDlg->m_szCrocessSuffix = "";
		return;
	}
}

int CAtumLauncherApp::ExitInstance() 
{
	// TODO: Add your specialized code here and/or call the base class
	if(m_hMutexMonoInstance)
	{
		::CloseHandle(m_hMutexMonoInstance);
		m_hMutexMonoInstance = NULL;
	}
	
	return CWinApp::ExitInstance();
}
