@echo off

"../tools/Sharpmake/Sharpmake.Application/bin/Release/net6.0/Sharpmake.Application.exe" /sources('./example/core-only/sharpmake.cs')
if %errorlevel% NEQ 0 (
	exit /b %errorlevel%
)

"../tools/Sharpmake/Sharpmake.Application/bin/Release/net6.0/Sharpmake.Application.exe" /sources('./example/engine/sharpmake.cs')
exit /b %errorlevel%
