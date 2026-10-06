#include "StdAfx.h"
#include "AceTRClientBootstrap.h"
#include <windows.h>
#include <stdio.h>

namespace
{
    enum
    {
        IDC_BOOT_ACCOUNT = 5001,
        IDC_BOOT_PASSWORD,
        IDC_BOOT_REMEMBER,
        IDC_BOOT_WINDOWED,
        IDC_BOOT_64BIT,
        IDC_BOOT_LOGIN,
        IDC_BOOT_CLOSE
    };

    struct BootstrapState
    {
        HWND hwnd;
        HWND account;
        HWND password;
        HWND remember;
        HWND windowed;
        HWND bit64;
        HWND login;
        bool submitted;
        BootstrapState() : hwnd(NULL), account(NULL), password(NULL), remember(NULL),
            windowed(NULL), bit64(NULL), login(NULL), submitted(false) {}
    };

    static BootstrapState g_state;
    static HBRUSH g_bgBrush = NULL;
    static HBRUSH g_editBrush = NULL;
    static HFONT g_font = NULL;
    static HFONT g_titleFont = NULL;

    static void CenterWindow(HWND hwnd)
    {
        RECT rc = {0}, wa = {0};
        GetWindowRect(hwnd, &rc);
        SystemParametersInfo(SPI_GETWORKAREA, 0, &wa, 0);
        const int w = rc.right - rc.left;
        const int h = rc.bottom - rc.top;
        SetWindowPos(hwnd, NULL,
            wa.left + ((wa.right - wa.left) - w) / 2,
            wa.top + ((wa.bottom - wa.top) - h) / 2,
            0, 0, SWP_NOSIZE | SWP_NOZORDER);
    }

    static void SaveRememberedAccount(const char* account, bool remember)
    {
        char iniPath[MAX_PATH] = {0};
        GetModuleFileNameA(NULL, iniPath, MAX_PATH);
        char* slash = strrchr(iniPath, '\\');
        if (slash) *(slash + 1) = 0;
        lstrcatA(iniPath, "AceTRClient.ini");

        WritePrivateProfileStringA("Login", "Account", remember ? account : "", iniPath);
        WritePrivateProfileStringA("Login", "Remember", remember ? "1" : "0", iniPath);
    }

    static void LoadRememberedAccount()
    {
        char iniPath[MAX_PATH] = {0};
        GetModuleFileNameA(NULL, iniPath, MAX_PATH);
        char* slash = strrchr(iniPath, '\\');
        if (slash) *(slash + 1) = 0;
        lstrcatA(iniPath, "AceTRClient.ini");

        if (GetPrivateProfileIntA("Login", "Remember", 0, iniPath) == 1)
        {
            char account[256] = {0};
            GetPrivateProfileStringA("Login", "Account", "", account, sizeof(account), iniPath);
            SetWindowTextA(g_state.account, account);
            SendMessage(g_state.remember, BM_SETCHECK, BST_CHECKED, 0);
        }
    }

    static bool StartHiddenLauncherBackend()
    {
        char exeDir[MAX_PATH] = {0};
        GetModuleFileNameA(NULL, exeDir, MAX_PATH);
        char* slash = strrchr(exeDir, '\\');
        if (slash) *(slash + 1) = 0;

        char launcherPath[MAX_PATH] = {0};
        lstrcpyA(launcherPath, exeDir);
        lstrcatA(launcherPath, "Launcher.exe");

        DWORD attr = GetFileAttributesA(launcherPath);
        if (attr == INVALID_FILE_ATTRIBUTES || (attr & FILE_ATTRIBUTE_DIRECTORY))
        {
            MessageBoxA(g_state.hwnd,
                "Launcher.exe oyun klasorunde bulunamadi.\n\n"
                "AtumLauncher projesini R_Wikigames_Eng Win32 olarak derleyip "
                "Launcher.exe dosyasini oyun klasorune koyun.",
                "AceTR", MB_OK | MB_ICONERROR);
            return false;
        }

        char account[256] = {0};
        char password[256] = {0};
        GetWindowTextA(g_state.account, account, sizeof(account));
        GetWindowTextA(g_state.password, password, sizeof(password));

        if (!account[0] || !password[0])
        {
            MessageBoxA(g_state.hwnd, "Kullanici adi ve sifre gerekli.", "AceTR", MB_OK | MB_ICONWARNING);
            return false;
        }

        const bool remember = SendMessage(g_state.remember, BM_GETCHECK, 0, 0) == BST_CHECKED;
        const bool windowed = SendMessage(g_state.windowed, BM_GETCHECK, 0, 0) == BST_CHECKED;
        const bool bit64 = SendMessage(g_state.bit64, BM_GETCHECK, 0, 0) == BST_CHECKED;

        SaveRememberedAccount(account, remember);

        SetEnvironmentVariableA("ACETR_MODERN_LAUNCHER", "1");
        SetEnvironmentVariableA("ACETR_ACCOUNT", account);
        SetEnvironmentVariableA("ACETR_PASSWORD", password);
        SetEnvironmentVariableA("ACETR_REMEMBER", remember ? "1" : "0");
        SetEnvironmentVariableA("ACETR_WINDOWED", windowed ? "1" : "0");
        SetEnvironmentVariableA("ACETR_64BIT", bit64 ? "1" : "0");

        STARTUPINFOA si;
        PROCESS_INFORMATION pi;
        ZeroMemory(&si, sizeof(si));
        ZeroMemory(&pi, sizeof(pi));
        si.cb = sizeof(si);
        si.dwFlags = STARTF_USESHOWWINDOW;
        si.wShowWindow = SW_HIDE;

        char cmdLine[MAX_PATH + 4] = {0};
        wsprintfA(cmdLine, "\"%s\"", launcherPath);

        BOOL ok = CreateProcessA(
            launcherPath,
            cmdLine,
            NULL,
            NULL,
            FALSE,
            CREATE_DEFAULT_ERROR_MODE,
            NULL,
            exeDir,
            &si,
            &pi);

        SetEnvironmentVariableA("ACETR_PASSWORD", NULL);

        if (!ok)
        {
            char msg[256] = {0};
            wsprintfA(msg, "Launcher backend baslatilamadi. Windows hata kodu: %lu", GetLastError());
            MessageBoxA(g_state.hwnd, msg, "AceTR", MB_OK | MB_ICONERROR);
            return false;
        }

        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
        SecureZeroMemory(password, sizeof(password));
        g_state.submitted = true;
        return true;
    }

    static void DrawTechFrame(HDC hdc, const RECT& r, COLORREF border)
    {
        HPEN pen = CreatePen(PS_SOLID, 1, border);
        HGDIOBJ oldPen = SelectObject(hdc, pen);
        HGDIOBJ oldBrush = SelectObject(hdc, GetStockObject(HOLLOW_BRUSH));
        RoundRect(hdc, r.left, r.top, r.right, r.bottom, 12, 12);

        RECT inner = { r.left + 4, r.top + 4, r.right - 4, r.bottom - 4 };
        RoundRect(hdc, inner.left, inner.top, inner.right, inner.bottom, 9, 9);

        SelectObject(hdc, oldBrush);
        SelectObject(hdc, oldPen);
        DeleteObject(pen);
    }

    static LRESULT CALLBACK BootstrapWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        switch (msg)
        {
        case WM_CREATE:
        {
            g_state.hwnd = hwnd;
            g_bgBrush = CreateSolidBrush(RGB(3, 10, 18));
            g_editBrush = CreateSolidBrush(RGB(6, 23, 37));

            g_font = CreateFontA(19, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Segoe UI");
            g_titleFont = CreateFontA(34, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Segoe UI");

            g_state.account = CreateWindowExA(0, "EDIT", "", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
                360, 238, 300, 44, hwnd, (HMENU)IDC_BOOT_ACCOUNT, GetModuleHandle(NULL), NULL);
            g_state.password = CreateWindowExA(0, "EDIT", "", WS_CHILD | WS_VISIBLE | ES_PASSWORD | ES_AUTOHSCROLL,
                360, 304, 300, 44, hwnd, (HMENU)IDC_BOOT_PASSWORD, GetModuleHandle(NULL), NULL);

            g_state.remember = CreateWindowExA(0, "BUTTON", "Beni Hatirla",
                WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
                360, 365, 140, 28, hwnd, (HMENU)IDC_BOOT_REMEMBER, GetModuleHandle(NULL), NULL);
            g_state.windowed = CreateWindowExA(0, "BUTTON", "Pencere Modu",
                WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
                515, 365, 145, 28, hwnd, (HMENU)IDC_BOOT_WINDOWED, GetModuleHandle(NULL), NULL);
            g_state.bit64 = CreateWindowExA(0, "BUTTON", "64 Bit",
                WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
                360, 402, 100, 28, hwnd, (HMENU)IDC_BOOT_64BIT, GetModuleHandle(NULL), NULL);

            g_state.login = CreateWindowExA(0, "BUTTON", "GIRIS YAP",
                WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
                360, 458, 300, 58, hwnd, (HMENU)IDC_BOOT_LOGIN, GetModuleHandle(NULL), NULL);

            HWND closeBtn = CreateWindowExA(0, "BUTTON", "X",
                WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
                738, 18, 42, 34, hwnd, (HMENU)IDC_BOOT_CLOSE, GetModuleHandle(NULL), NULL);

            SendMessage(g_state.account, WM_SETFONT, (WPARAM)g_font, TRUE);
            SendMessage(g_state.password, WM_SETFONT, (WPARAM)g_font, TRUE);
            SendMessage(g_state.remember, WM_SETFONT, (WPARAM)g_font, TRUE);
            SendMessage(g_state.windowed, WM_SETFONT, (WPARAM)g_font, TRUE);
            SendMessage(g_state.bit64, WM_SETFONT, (WPARAM)g_font, TRUE);
            SendMessage(g_state.login, WM_SETFONT, (WPARAM)g_font, TRUE);
            SendMessage(closeBtn, WM_SETFONT, (WPARAM)g_font, TRUE);

            SendMessage(g_state.account, EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN, MAKELPARAM(12, 12));
            SendMessage(g_state.password, EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN, MAKELPARAM(12, 12));

            LoadRememberedAccount();
            CenterWindow(hwnd);
            return 0;
        }

        case WM_CTLCOLORSTATIC:
        {
            HDC hdc = (HDC)wParam;
            SetTextColor(hdc, RGB(190, 225, 240));
            SetBkColor(hdc, RGB(3, 10, 18));
            return (LRESULT)g_bgBrush;
        }

        case WM_CTLCOLOREDIT:
        {
            HDC hdc = (HDC)wParam;
            SetTextColor(hdc, RGB(232, 248, 255));
            SetBkColor(hdc, RGB(6, 23, 37));
            return (LRESULT)g_editBrush;
        }

        case WM_DRAWITEM:
        {
            DRAWITEMSTRUCT* dis = (DRAWITEMSTRUCT*)lParam;
            if (dis->CtlID == IDC_BOOT_LOGIN || dis->CtlID == IDC_BOOT_CLOSE)
            {
                const bool pressed = (dis->itemState & ODS_SELECTED) != 0;
                RECT r = dis->rcItem;
                HBRUSH b = CreateSolidBrush(dis->CtlID == IDC_BOOT_LOGIN
                    ? (pressed ? RGB(0, 91, 145) : RGB(0, 142, 210))
                    : RGB(8, 27, 40));
                HPEN p = CreatePen(PS_SOLID, 2, RGB(34, 211, 255));
                HGDIOBJ ob = SelectObject(dis->hDC, b);
                HGDIOBJ op = SelectObject(dis->hDC, p);
                RoundRect(dis->hDC, r.left, r.top, r.right, r.bottom, 8, 8);

                SetBkMode(dis->hDC, TRANSPARENT);
                SetTextColor(dis->hDC, RGB(255, 255, 255));
                char txt[64] = {0};
                GetWindowTextA(dis->hwndItem, txt, sizeof(txt));
                DrawTextA(dis->hDC, txt, -1, &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

                SelectObject(dis->hDC, op);
                SelectObject(dis->hDC, ob);
                DeleteObject(p);
                DeleteObject(b);
                return TRUE;
            }
            break;
        }

        case WM_COMMAND:
            if (LOWORD(wParam) == IDC_BOOT_LOGIN)
            {
                if (StartHiddenLauncherBackend())
                    DestroyWindow(hwnd);
                return 0;
            }
            if (LOWORD(wParam) == IDC_BOOT_CLOSE)
            {
                DestroyWindow(hwnd);
                return 0;
            }
            break;

        case WM_LBUTTONDOWN:
            ReleaseCapture();
            SendMessage(hwnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
            return 0;

        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            RECT client;
            GetClientRect(hwnd, &client);
            FillRect(hdc, &client, g_bgBrush);

            RECT top = {0, 0, client.right, 96};
            HBRUSH topBrush = CreateSolidBrush(RGB(5, 20, 33));
            FillRect(hdc, &top, topBrush);
            DeleteObject(topBrush);

            RECT left = {24, 118, 322, 540};
            RECT loginPanel = {338, 118, 686, 540};
            DrawTechFrame(hdc, left, RGB(15, 95, 132));
            DrawTechFrame(hdc, loginPanel, RGB(20, 178, 230));

            SetBkMode(hdc, TRANSPARENT);
            SelectObject(hdc, g_titleFont);
            SetTextColor(hdc, RGB(238, 250, 255));
            TextOutA(hdc, 38, 27, "ACE", 3);
            SetTextColor(hdc, RGB(0, 194, 255));
            TextOutA(hdc, 116, 27, "TR", 2);

            SelectObject(hdc, g_font);
            SetTextColor(hdc, RGB(126, 174, 198));
            TextOutA(hdc, 194, 38, "ACE ONLINE TURKIYE", 18);

            SetTextColor(hdc, RGB(222, 242, 250));
            TextOutA(hdc, 58, 154, "TEK ISTEMCI", 11);
            SetTextColor(hdc, RGB(96, 149, 175));
            TextOutA(hdc, 58, 190, "Guncelleme, giris ve oyun", 25);
            TextOutA(hdc, 58, 216, "artik ProjectAtum icinden", 24);
            TextOutA(hdc, 58, 242, "baslatiliyor.", 13);

            SetTextColor(hdc, RGB(61, 225, 139));
            TextOutA(hdc, 58, 306, "● SUNUCU HAZIR", 15);

            SetTextColor(hdc, RGB(222, 242, 250));
            TextOutA(hdc, 360, 156, "KULLANICI GIRISI", 16);
            SetTextColor(hdc, RGB(106, 158, 182));
            TextOutA(hdc, 360, 214, "KULLANICI ADI", 12);
            TextOutA(hdc, 360, 280, "SIFRE", 5);

            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_DESTROY:
            if (g_font) { DeleteObject(g_font); g_font = NULL; }
            if (g_titleFont) { DeleteObject(g_titleFont); g_titleFont = NULL; }
            if (g_bgBrush) { DeleteObject(g_bgBrush); g_bgBrush = NULL; }
            if (g_editBrush) { DeleteObject(g_editBrush); g_editBrush = NULL; }
            PostQuitMessage(0);
            return 0;
        }

        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}

bool RunAceTRClientBootstrap(HINSTANCE hInstance)
{
    const char* className = "AceTRClientBootstrapWindow";

    WNDCLASSEXA wc;
    ZeroMemory(&wc, sizeof(wc));
    wc.cbSize = sizeof(wc);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = BootstrapWndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = className;

    RegisterClassExA(&wc);

    HWND hwnd = CreateWindowExA(
        WS_EX_APPWINDOW,
        className,
        "AceTR",
        WS_POPUP,
        CW_USEDEFAULT, CW_USEDEFAULT,
        800, 580,
        NULL, NULL, hInstance, NULL);

    if (!hwnd)
        return false;

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return g_state.submitted;
}
