import hashlib
from pathlib import Path
from pathlib import Path
from os import chdir
from concurrent.futures import ThreadPoolExecutor
import os
import subprocess
import zipfile

def run(cmd, cwd=Path.cwd()):
    result = subprocess.run(cmd, shell=True, cwd=cwd)
    if result.returncode != 0:
        raise RuntimeError(f"Failed to execute {cmd}")

def run_and_capture_output(cmd, cwd=Path.cwd()) -> str:
    result = subprocess.run(cmd, shell=True, cwd=cwd, stdout=subprocess.PIPE)
    if result.returncode != 0:
        print(f"Failed to execute {cmd}")
    if result.stdout is not None:
        return str(result.stdout.decode('utf-8'))
    return ""

def cd_repo_root_path() -> str:
    # Make sure the script is always started from the same 
    # ScreenPlay root folder
    root_path = Path.cwd()
    if root_path.name == "Tools":
        root_path = root_path.parent
        print(f"Change root directory to: {root_path}")
        chdir(root_path)
    return root_path

def sha256(fname) -> str:
    hash_sha256 = hashlib.sha256()
    with open(fname, "rb") as f:
        for chunk in iter(lambda: f.read(4096), b""):
            hash_sha256.update(chunk)
    return hash_sha256.hexdigest()



def zipdir(path, ziph):
    # ziph is zipfile handle
    for root, dirs, files in os.walk(path):
        for file in files:
            ziph.write(os.path.join(root, file),
                       os.path.relpath(os.path.join(root, file),
                                       os.path.join(path, '..')))


def run_io_tasks_in_parallel(tasks):
    with ThreadPoolExecutor() as executor:
        running_tasks = [executor.submit(task) for task in tasks]
        for running_task in running_tasks:
            running_task.result()