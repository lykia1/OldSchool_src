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

where dotnet.exe >nul 2>nul
if errorlevel 1 (
  echo [ERROR] .NET SDK bulunamadi.
  echo AceTR Account API icin .NET 8 SDK kurulmasi gerekiyor.
  echo.
  pause
  exit /b 1
)

echo [OK] PowerShell bulundu.
for /f "delims=" %%v in ('dotnet --version 2^>nul') do set DOTNET_VERSION=%%v
echo [OK] dotnet version: %DOTNET_VERSION%
echo.

if not exist "%~dp0appsettings.json" (
  echo [INFO] appsettings.json henuz yok.
  echo Ilk calistirmada olusturulacak ve pencere acik kalacak.
  echo.
)

powershell.exe -NoLogo -NoProfile -ExecutionPolicy Bypass -NoExit -File "%~dp0run-local.ps1"

echo.
echo PowerShell kapandi.
pause
