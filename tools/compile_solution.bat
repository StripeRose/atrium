@echo off
:: Batch arguments:
:: %~1: Project/Solution to build
:: %~2: Target(Normally should be Debug or Release)
:: %~3: Platform(Normally should be "Any CPU" for sln and AnyCPU for a csproj)

setlocal enabledelayedexpansion

set VSWHERE="%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist %VSWHERE% (
    echo Error: Cannot determine the location of the VSWhere tool.
    exit /b 1
)

set VSMSBUILDCMD=
for /f "usebackq delims=" %%i in (`%VSWHERE% -latest -products * -property installationPath`) do (
  if exist "%%i\Common7\Tools\VsMSBuildCmd.bat" (
    set VSMSBUILDCMD="%%i\Common7\Tools\VsMSBuildCmd.bat"
  )
)

if not defined VSMSBUILDCMD (
    echo Error: Cannot determine the location of MSBuild.
    exit /b 1
)

echo MSBuild batch path: !VSMSBUILDCMD!
call !VSMSBUILDCMD!
if %errorlevel% NEQ 0 (
	exit /b 1
)

echo Compiling %~1 in "%~2|%~3"...

set MSBUILD_CMD=msbuild -clp:Summary -t:rebuild -restore "%~1" /nologo /verbosity:m /p:Configuration="%~2" /p:Platform="%~3" /maxcpucount /p:CL_MPCount=%NUMBER_OF_PROCESSORS%
echo %MSBUILD_CMD%
%MSBUILD_CMD%
if %errorlevel% NEQ 0 (
    echo ERROR: Failed to compile %~1 in "%~2|%~3".
    color 0C
)

exit /b %errorlevel%