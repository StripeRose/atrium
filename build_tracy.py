
from tools import compile
from tools import terminal_subprocess

import os
from pathlib import Path
import shutil

buildPath = str(Path(__file__).parent() / "tracy_build")
if not buildPath.exists():
    buildPath.mkdir()

os.chdir(str(buildPath))

compile.cmake(str(
    buildPath / "../libraries/tracy/profiler/CMakeLists.txt"
))
compile.buildsolution(
    str(buildPath / "tracy-profiler.sln"),
    "Release",
    "x64"
)

shutil.copyfile(
    str(buildPath / "Release/tracy-profiler.exe"),
    str(buildPath / "..")
)

buildPath.rmdir()