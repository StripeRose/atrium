@echo off
:: Batch arguments:
:: %~1: sharpmake.cs file to generate.

:: If Sharpmake isn't built yet, build it first.

if not exist "%~dp0Sharpmake/Sharpmake.Application/bin/Release/net6.0/Sharpmake.Application.exe" (
	call "%~dp0compile_solution.bat" "%~dp0Sharpmake/Sharpmake.sln" "Release" "Any CPU"
	if %errorlevel% NEQ 0 (
		echo.
		echo Error: Failed to compile Sharpmake.
		color 0C
		pause
		exit /b %errorlevel%
	)
)

:: Sharpmake isn't happy with back-slashes, so we just replace them here.
set "path=%~1"
set "sanitizedPath=%path:\=/%"
"%~dp0Sharpmake/Sharpmake.Application/bin/Release/net6.0/Sharpmake.Application.exe" "/sources('%sanitizedPath%')"