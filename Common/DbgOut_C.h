// File: DbgOut.h
//

#include "windows.h"
#include "tchar.h"
#include "Contents.h"
#include <time.h>
#include "AtumDateTime.h"
#ifdef _ATUM_CLIENT
#include "stdio.h"

#ifdef _BERGI_CLIENT_DEBUG_LOG_EXT
#define DBGOUT_EFFECT(...) printf("%-30s:%05d - %10d  ---  ", __FILE__, __LINE__, GetTickCount()); printf(__VA_ARGS__);
#else
	#ifdef _DEBUG
		#define DBGOUT_EFFECT printf
	#else
		#define DBGOUT_EFFECT __noop
	#endif
#endif
#endif
#ifndef DBGOUT_H_D7274660_0C36_11d2_9253_0000C06932AE__INCLUDED_
#define DBGOUT_H_D7274660_0C36_11d2_9253_0000C06932AE__INCLUDED_

static  LPCTSTR    g_dbgOut                = _T("Client DebugOutput 20020805");
static  LPCTSTR    g_dbgOutwindowClassName = _T("Client DebugOut Window");


inline void DbgOutA (LPCSTR p)
{
    COPYDATASTRUCT cd;
    HWND hWnd = ::FindWindow (g_dbgOutwindowClassName, g_dbgOut);
    if (hWnd)
    {
        cd.dwData = 0;
        cd.cbData = (strlen(p)+1)*sizeof(char);
        cd.lpData = (void *)p;
        ::SendMessage (hWnd, WM_COPYDATA, 0, (LPARAM)&cd);
    }
	else
	{
		SetLastError(0);
	}
}


// DBGOUTA
#ifdef _DEBUG
#define DBGOUTA(p) DbgOutA(p);
#else
#define DBGOUTA __noop
#endif


inline void DbgOutW (LPCWSTR p)
{
    COPYDATASTRUCT cd;
    HWND hWnd = ::FindWindow (g_dbgOutwindowClassName, g_dbgOut);
    if (hWnd)
    {
        cd.dwData = 0xFEFF;
        cd.cbData = (wcslen(p)+1)*sizeof(wchar_t);
        cd.lpData = (void *)p;
        ::SendMessage (hWnd, WM_COPYDATA, 0, (LPARAM)&cd);
    }
	else
	{
		SetLastError(0);
	}
}

// DBGOUTW
#ifdef _DEBUG
#define DBGOUTW(p) DbgOutW(p);
#else
#define DBGOUTW __noop
#endif

#define FDBG ::InetDbgToFile
inline bool IsUserAdmin()
{
    BOOL b;
    SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
    PSID AdministratorsGroup;

    b = AllocateAndInitializeSid(
        &NtAuthority,
        2,
        SECURITY_BUILTIN_DOMAIN_RID,
        DOMAIN_ALIAS_RID_ADMINS,
        0, 0, 0, 0, 0, 0,
        &AdministratorsGroup);

    if (b == TRUE)
    {
        if (CheckTokenMembership(NULL, AdministratorsGroup, &b) == FALSE)
        {
            b = FALSE;
        }
        FreeSid(AdministratorsGroup);
    }

    return (b == TRUE);
}
inline void InetDbgToFile(LPCTSTR pFormat, ...)
{
#ifdef _DEBUG
    return;
#endif
    va_list args;
    va_start(args, pFormat);

    _TCHAR buffer[1024 * sizeof(_TCHAR)];
    vsprintf(buffer, pFormat, args);


    ATUM_DATE_TIME now(true);
    tm tmTM;
    now.Convert(tmTM);
    char bufTime[128]; *bufTime = '\0';
    char szTmpOutLog[1024]; *szTmpOutLog = '\0';
    strftime(bufTime, 128, "%d/%m/%y %H:%M:%S", &tmTM);
    if(IsUserAdmin())
        sprintf(szTmpOutLog, "[Admin][%s] %s\n", bufTime, buffer);
    else
        sprintf(szTmpOutLog, "[%s] %s\n", bufTime, buffer);
   

    FILE* pFile = fopen("client_errorlog.txt", "a");
    fprintf(pFile, "%s", szTmpOutLog);
    fclose(pFile);

    memset(bufTime, 0x00, sizeof(bufTime));
    memset(szTmpOutLog, 0x00, sizeof(szTmpOutLog));
    memset(buffer, 0x00, sizeof(buffer));

    va_end(args);
}


inline void DbgOut (LPCTSTR pFormat, ...)
{
	va_list args;
	va_start(args, pFormat);

    _TCHAR buffer [1024*sizeof(_TCHAR)];
	//vsprintf(buffer, pFormat, args);

#ifdef UNICODE
	vswprintf(buffer, pFormat, args);
	DbgOutW (buffer);
#else
	vsprintf(buffer, pFormat, args);
	DbgOutA(buffer);
#endif

    va_end(args);
}

// DBGOUT
#ifdef _DEBUG
#define DBGOUT ::DbgOut
#else
#ifdef _BERGI_CLIENT_DEBUG_LOG
#define DBGOUT(...) printf("%-30s:%05d - %10d  ---  ", __FILE__, __LINE__, GetTickCount()); printf(__VA_ARGS__);
#else
#define DBGOUT __noop
#endif
#endif



inline DWORD DbgOutLastError (LPCTSTR pFormat, ...)
{
   if (::GetLastError() == 0)
        return 0;

	va_list args;
	va_start(args, pFormat);

    _TCHAR buffer [1024*sizeof(_TCHAR)];
	//vsprintf(buffer, pFormat, args);
#ifdef UNICODE
	vswprintf(buffer, pFormat, args);
#else
	vsprintf(buffer, pFormat, args);
#endif

    LPVOID pMessage;
    DWORD  result;
    result = ::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                             NULL,
                             GetLastError(),
                             MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
                             (LPTSTR)&pMessage,
                             0,
                             NULL);

    lstrcat (buffer, _T(" : "));
    lstrcat (buffer, (TCHAR*)pMessage);

    DBGOUT (buffer);

    if(result)
        ::LocalFree(pMessage);

    va_end(args);
    return result;
}

// DBGOUT_LASTERROR
#ifdef _DEBUG
#define DBGOUT_LASTERROR ::DbgOutLastError
#else
#define DBGOUT_LASTERROR __noop
#endif



#endif//  DBGOUT_H_D7274660_0C36_11d2_9253_0000C06932AE__INCLUDED_
