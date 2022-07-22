

from pathlib import Path
import os
import subprocess

def run(cmd, cwd=Path.cwd()):
    result = subprocess.run(cmd, shell=True, cwd=cwd)
    if result.returncode != 0:
        raise RuntimeError(f"Failed to execute {cmd}")

