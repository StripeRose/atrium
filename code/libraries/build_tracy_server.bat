@echo off

call "./tracy/vcpkg/install_vcpkg_dependencies.bat"
call "../../tools/compile_solution.bat" "./tracy/profiler/build/win32/Tracy.sln" "Release" "x64"
