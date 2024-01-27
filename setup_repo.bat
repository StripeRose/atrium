@echo off
call :CloneSubmodules
call "./tools/compile_sharpmake.bat"
call :GenerateProject
exit /b 0

::==============================================================
:: INIT SUBMODULES
:CloneSubmodules
echo Cloning submodules...

git submodule update --init --recursive

if %errorlevel% NEQ 0 (
	echo.
	echo Error: Failed to check out all submodules.
	color 0C
	exit /b %errorlevel%
)

exit /b 0

::==============================================================
:: GENERATE PROJECT
:GenerateProject
echo Generating project solution..
cd ".\Code\"
call "generate_solution.bat"

if %errorlevel% NEQ 0 (
	cd ..
	echo.
	echo Error: Failed to generate Visual Studio solutions.
	color 0C
	exit /b %errorlevel%
)

cd ..

color 0A
echo.
echo.
echo Repository has been set up. You should now have the solution generated in the Code folder.
pause

exit /b 0
