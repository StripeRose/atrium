
# Tools for compiling solutions.

from atrium.tools import terminal_subprocess

import os
from pathlib import Path
import subprocess
import sys

# Path to CMakeLists.txt
def cmake(aListspath):
    terminal_subprocess.run([
        _getcmakepath(),
        aListspath
    ])

# Target normally should be "Debug" or "Release".
# Platform should normally be "Any CPU" for sln and "AnyCPU" for csproj.
def buildsolution(aSolutionPath, aTarget, aPlatform):
    terminal_subprocess.run([
        str(_getmsbuildpath()),
        "-clp:Summary",
        "-t:rebuild",
        "-restore",
        aSolutionPath,
        "/nologo",
        "/verbosity:m",
        "/p:Configuration=" + str(aTarget),
        "/p:Platform=" + str(aPlatform)
    ])

def _getvspath():
    vswhere = Path(os.getenv("ProgramFiles(x86)"), "Microsoft Visual Studio", "Installer", "vswhere.exe")
    if vswhere.is_file():
        return Path(subprocess.check_output([
            str(vswhere),
            "-latest",
            "-products", "*",
            "-property", "installationPath",
            "-requires", "Microsoft.VisualStudio.Component.VC.Tools.x86.x64",
            "-utf8",
        ], encoding="utf-8", errors="strict").strip())

    raise RuntimeError("Unable to locate Visual Studio.")

def _getmsbuildpath():
    vspath = _getvspath()
    if out.is_dir():
        exe = out / "MSBuild/Current/Bin/msbuild.exe"
        if exe.is_file():
            return exe

def _getcmakepath():
    vspath = _getvspath()
    if out.is_dir():
        exe = out / "Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/bin/cmake.exe"
        if exe.is_file():
            return exe
