@REM @echo off

mkdir "%~dp0tracy_build"
cd "%~dp0tracy_build"

call "%~dp0../tools/compile_cmake.bat" "%~dp0tracy/profiler/CMakeLists.txt"
call "%~dp0../tools/compile_solution.bat" "%~dp0tracy_build/tracy-profiler.sln" "Release" "x64"

move "%~dp0tracy_build\Release\tracy-profiler.exe" "%~dp0..\tools\"
