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
call "./tools/compile_solution.bat" "./tools/Sharpmake/Sharpmake.sln" "Release" "Any CPU"
if %errorlevel% NEQ 0 (
	echo.
	echo Error: Failed to check compile Sharpmake.
	color 0C
	pause
	exit /b %errorlevel%
)

::==============================================================
:: COMPILE TRACY SERVER
cd "./code/libraries"
call "./build_tracy_server.bat"

