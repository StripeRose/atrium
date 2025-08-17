Start-Job -ScriptBlock {
	$PSScriptRoot = "$using:PSScriptRoot"
	& "$PSScriptRoot/rose-common/tools/Launch-VsDevShell.ps1" -Latest

	Push-Location "$PSScriptRoot"

	if (-not (Test-Path "./tracy_build")) {
		New-Item -Name "./tracy_build" -ItemType "Directory"
	}

	Push-Location "./tracy_build"

	cmake -G "Visual Studio 17 2022" "$PSScriptRoot/tracy/profiler/CMakeLists.txt"
	msbuild -clp:Summary -t:rebuild -restore "./tracy-profiler.sln" /nologo /verbosity:m /p:Configuration="Release" /p:Platform="x64" /maxcpucount
	Start-Process -FilePath "./Release/tracy-profiler.exe"

	Pop-Location
	Pop-Location

} | Receive-Job -Wait -AutoRemoveJob