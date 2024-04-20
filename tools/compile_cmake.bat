@echo off
:: Batch arguments:
:: %~1: CMakeLists.txt to build.

setlocal enabledelayedexpansion

set VSWHERE="%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist %VSWHERE% (
    echo Error: Cannot determine the location of the VSWhere tool.
    exit /b 1
)

for /f "usebackq delims=" %%i in (`%VSWHERE% -latest -products * -property installationPath`) do (
  if exist "%%i\Common7\Tools\VsDevCmd.bat" (
    call "%%i\Common7\Tools\VsDevCmd.bat"
    goto :vsdevcmd_found
  )
)

echo Error: Cannot determine the location of Developer Command Prompt for Visual Studio.
exit /b 1

:vsdevcmd_found

:: Build with CMake

cmake -G "Visual Studio 17 2022" "%~1"