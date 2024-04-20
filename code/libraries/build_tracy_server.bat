@echo off

mkdir "tracy_build"
cd "./tracy_build"

call "../../../tools/compile_cmake.bat" "../tracy/profiler/CMakeLists.txt"
call "../../../tools/compile_solution.bat" "./tracy-profiler.sln" "Release" "x64"

cd "Release"
move "tracy-profiler.exe" "../../../../tools/"

cd "../../"
rd /s /q "./tracy_build"
