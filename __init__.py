
# Provides an interface for setting up and controlling Atrium workspaces.

from atrium.tools import compile
from atrium.tools import terminal_subprocess

import os
from pathlib import Path
import sys

# Generate a solution from a sharpmake folder or file.
# If folder is given, will assume the file name is "sharpmake.cs".
def generate_solution(aSharpmakePath):
    if aSharpmakePath.endswith(".cs"):
        return _sharpmake("./" + aSharpmakePath)
    else:
        return _sharpmake("./" + aSharpmakePath + "/sharpmake.cs")

def _sharpmakePath():
    return Path(__file__).parent.absolute() / "tools/Sharpmake/Sharpmake.Application/bin/Release/net6.0/Sharpmake.Application.exe"

def _sharpmakeBuild():
    compile.buildsolution(
        str(Path(__file__).parent.absolute() / "tools/Sharpmake/Sharpmake.sln"),
        "Release",
        "Any CPU"
    )

def _sharpmake(aSharpmakeFilePath):
    if not _sharpmakePath().exists():
        _sharpmakeBuild()

    return terminal_subprocess.run([
        str(_sharpmakePath()),
        "/sources('" + aSharpmakeFilePath + "')"
    ]) == 0
