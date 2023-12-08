@echo off

echo Ensuring submodules are cloned..
git submodule update --init --recursive
if %errorlevel% NEQ 0 (
	pause>nul
	exit
)

@REM IF BUILD TOOLS ARE TO BE USED.
@REM PLEASE ENSURE IT WORKS WHEN FINDING THE PATH BELOW.

@REM echo Ensuring Visual Studio 2022 Build Tools are installed..
@REM echo May ask you to accept.
@REM winget install --id=Microsoft.VisualStudio.2022.BuildTools -e
@REM if %errorlevel% NEQ 0 (
@REM 	@REM Make sure the error wasn't that it was already installed.
@REM 	if %errorlevel% NEQ -1978335189 (
@REM 		pause>nul
@REM 		exit
@REM 	)
@REM )

@REM pause>nul

echo Checking possible known directories for MSBuild.exe..

set msBuildPath="C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe"
if exist %msBuildPath% goto :msbuild_found

echo Error: Was unable to find MSBuild.exe.
echo %msBuildPath%
pause>nul
exit

:msbuild_found
echo Using %msBuildPath%

echo Compiling Sharpmake...
call %msBuildPath% ".\Tools\Sharpmake\Sharpmake.sln" /t:Restore;Build /p:Configuration=Release /v:minimal
if %errorlevel% NEQ 0 (
	pause>nul
	exit
)

echo Generating project solution..
cd ".\Code\"
call "generate_solution.bat"
cd ..

color 0A
echo.
echo.
echo Repository has been set up. You should now have the solution generated in the Code folder.

pause>nul
