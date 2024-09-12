
import os
from pathlib import Path

# Change working directory to the file's own.
os.chdir(Path(__file__).parent.absolute())

import atrium
if not atrium.generate_solution("sharpmake.cs"):
    print("======================================")
    print("FAILED TO GENERATE CHART-GAME SOLUTION")
    print("======================================")
    input("Press Return to continue..")