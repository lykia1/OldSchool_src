@echo off
setlocal
cd /d "%~dp0"

echo ==========================================
echo   AceTR Account API Launcher
echo ==========================================
echo.

where powershell.exe >nul 2>nul
if errorlevel 1 (
  echo [ERROR] powershell.exe bulunamadi.
  echo.
  pause
  exit /b 1
)

powershell.exe -NoLogo -NoProfile -ExecutionPolicy Bypass -NoExit -File "%~dp0run-local.ps1"

echo.
echo PowerShell kapandi.
pause
