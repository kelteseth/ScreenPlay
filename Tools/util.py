

from pathlib import Path
from pathlib import Path
from os import chdir
import subprocess

def run(cmd, cwd=Path.cwd()):
    result = subprocess.run(cmd, shell=True, cwd=cwd)
    if result.returncode != 0:
        raise RuntimeError(f"Failed to execute {cmd}")



def cd_repo_root_path() -> str:
    # Make sure the script is always started from the same 
    # ScreenPlay root folder
    root_path = Path.cwd()
    if root_path.name == "Tools":
        root_path = root_path.parent
        print(f"Change root directory to: {root_path}")
        chdir(root_path)
    return root_path
