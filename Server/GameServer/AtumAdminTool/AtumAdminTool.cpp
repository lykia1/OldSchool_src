// AtumAdminTool.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "AtumAdminTool.h"
#include "AtumAdminToolDlg.h"
#include "VMemPool.h"
#include "dbgHelp.h"
#pragma comment(lib, "dbghelp.lib")
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
LONG __stdcall ExceptionHandler(_EXCEPTION_POINTERS* pExceptionInfo)
{
	char fileName[MAX_PATH];
	GetModuleFileName(NULL, fileName, sizeof(fileName));
	char* ext = strrchr(fileName, '.');


	ATUM_DATE_TIME	CurrentTime(TRUE);
	sprintf(ext ? ext : fileName + strlen(fileName), "_%s.dmp", CurrentTime.GetFileDateTimeString(STRNBUF(SIZE_MAX_SQL_DATETIME_STRING)));


	char szTemp[256];
	wsprintf(szTemp, "AdminTool Crash !! : Create dump file (Exception 0x%08x arised)", pExceptionInfo->ExceptionRecord->ExceptionCode);

	HANDLE hProcess = GetCurrentProcess();
	DWORD dwProcessID = GetCurrentProcessId();
	HANDLE hFile = CreateFile(fileName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	MINIDUMP_EXCEPTION_INFORMATION eInfo;
	eInfo.ThreadId = GetCurrentThreadId();
	eInfo.ExceptionPointers = pExceptionInfo;
	eInfo.ClientPointers = FALSE;

	MiniDumpWriteDump(hProcess, dwProcessID, hFile, MiniDumpWithFullMemory, pExceptionInfo ? &eInfo : NULL, NULL, NULL);

	
	MessageBox(NULL, szTemp, "ERROR", MB_TOPMOST | MB_ICONSTOP);


	return EXCEPTION_EXECUTE_HANDLER;
}

/////////////////////////////////////////////////////////////////////////////
// CAtumAdminToolApp

BEGIN_MESSAGE_MAP(CAtumAdminToolApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAtumAdminToolApp construction

CAtumAdminToolApp::CAtumAdminToolApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CAtumAdminToolApp object

CAtumAdminToolApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CAtumAdminToolApp initialization

BOOL CAtumAdminToolApp::InitInstance()
{
	SetUnhandledExceptionFilter(&ExceptionHandler);
	AfxEnableControlContainer();

	///////////////////////////////////////////////////////////////////////////////
	// 2006-04-10 by cmkwon, Initialize winsock 2.0
	CWinSocket::SocketInit();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	CVMemPool::vmPoolAddObject(sizeof(CSendPacket), 1000);
	CVMemPool::vmPoolAddObject(sizeof(CRecvPacket), 100);	

	if(CVMemPool::vmPoolInit() == FALSE) {
		return FALSE;
	}

	SetRegistryKey(STRMSG_REG_STRING_REGISTRYKEY_NAME);

	GDecryptGameServerInfoByXOR();

	CAtumAdminToolDlg dlg;
	m_pMainWnd = &dlg;
	ReadProfile();
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

void CAtumAdminToolApp::WriteProfile()
{
	// registry path: HKEY_CURRENT_USER\Software\SpaceCowboy\AtumAdminTool\Configuration
	CAtumAdminToolDlg *pDlg = (CAtumAdminToolDlg*)AfxGetMainWnd();

	if(pDlg)
	{
#ifndef _ATUM_ADMIN_RELEASE
		WriteProfileString(_T("Configuration"), _T("UID"), pDlg->m_UID);
		WriteProfileString(_T("Configuration"), _T("PWD"), pDlg->m_PWD);
#endif// _ATUM_ADMIN_RELEASE_end_ifndef
		WriteProfileString(_T("Configuration"), _T("Server"), pDlg->m_szServerName);
		WriteProfileInt(_T("Configuration"), _T("LanguageType"), pDlg->m_nLanguageType);
		WriteProfileString(_T("Configuration"), _T("LocalizationDirectory"), pDlg->m_strLocalizationDirectoryPath);

		// CSCUserAdminToolDlg
		WriteProfileString(_T("Configuration"), _T("AccountNameInput"), m_szAccountNameInputReg);
	}
}

void CAtumAdminToolApp::ReadProfile()
{
	// registry path: HKEY_CURRENT_USER\Software\Atum Online\AtumAdminTool\Configuration
	CAtumAdminToolDlg *pDlg = (CAtumAdminToolDlg*)AfxGetMainWnd();

	if(pDlg)
	{
#ifndef _ATUM_ADMIN_RELEASE
		pDlg->m_UID = GetProfileString( _T("Configuration"), _T("UID"), "");
		pDlg->m_PWD = GetProfileString( _T("Configuration"), _T("PWD"), "");
#endif// _ATUM_ADMIN_RELEASE_end_ifndef
		pDlg->m_szServerName = GetProfileString( _T("Configuration"), _T("Server"), "");
		pDlg->m_nLanguageType	= GetProfileInt( _T("Configuration"), _T("LanguageType"), LANGUAGE_TYPE_KOREAN);		
		pDlg->m_strLocalizationDirectoryPath = GetProfileString( _T("Configuration"), _T("LocalizationDirectory"), "./localization");

		// CSCUserAdminToolDlg
		m_szAccountNameInputReg = GetProfileString( _T("Configuration"), _T("AccountNameInput"), "");
	}
}


///////////////////////////////////////////////////////////////////////////////
/// \fn			char *CAtumAdminToolApp::GetPublicLocalIP(char *o_szLocalIP)
/// \brief		
/// \author		cmkwon
/// \date		2006-04-10 ~ 2006-04-10
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
char *CAtumAdminToolApp::GetPublicLocalIP(char *o_szLocalIP)
{
	GGetLocalIP(o_szLocalIP, IP_TYPE_PUBLIC);
	return o_szLocalIP;
}