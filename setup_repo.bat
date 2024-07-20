@echo off

::==============================================================
:: INIT SUBMODULES
git submodule update --init --recursive

if %errorlevel% NEQ 0 (
	echo.
	echo Error: Failed to check out all submodules.
	color 0C
	pause
	exit /b %errorlevel%
)

::==============================================================
:: COMPILE SHARPMAKE
if not exist "./tools/Sharpmake/Sharpmake.Application/bin/Release/net6.0/Sharpmake.Application.exe" (
	call "./tools/compile_solution.bat" "./tools/Sharpmake/Sharpmake.sln" "Release" "Any CPU"
	if %errorlevel% NEQ 0 (
		echo.
		echo Error: Failed to compile Sharpmake.
		color 0C
		pause
		exit /b %errorlevel%
	)
)
