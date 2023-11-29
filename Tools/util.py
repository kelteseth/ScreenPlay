#!/usr/bin/python3
# SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
import hashlib
from pathlib import Path
from pathlib import Path
from os import chdir
from concurrent.futures import ThreadPoolExecutor
import os
import re
import subprocess
from sys import stdout
import zipfile
import urllib.request
import sys
import ssl

def progress(count: int, block_size: int, total_size: int):
    percent = int(count * block_size * 100 / total_size)
    sys.stdout.write("\rDownload: {}%".format(percent))
    sys.stdout.flush()

# disable broken progress bar in CI
def download(url: str, filename: str, show_progress: bool = True):
    # This is needed for downloading from https sites
    # see https://programmerah.com/python-error-certificate-verify-failed-certificate-has-expired-40374/
    ssl._create_default_https_context = ssl._create_unverified_context

    if show_progress:
        urllib.request.urlretrieve(url, filename, progress)
    else:
        urllib.request.urlretrieve(url, filename)
        

stdout.reconfigure(encoding='utf-8')


def sftp_exists(sftp, path) -> bool:
    try:
        sftp.stat(path)
        return True
    except FileNotFoundError:
        return False


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


def repo_root_path() -> str:
    # Root dir of the repository
    path = os.path.join(os.path.realpath(__file__), "../../")
    return os.path.realpath(path)


def workspace_path() -> str:
    # One folder above the repo
    path = os.path.join(repo_root_path(), "../")
    return os.path.realpath(path)

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

# Based on https://gist.github.com/l2m2/0d3146c53c767841c6ba8c4edbeb4c2c


def get_vs_env_dict():
    vcvars: str  # We support 2019 or 2022

    # Hardcoded VS path
    # check if vcvars64.bat is available.
    msvc_2019_path = "C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Community\\VC\\Auxiliary\\Build\\vcvars64.bat"
    msvc_2022_path = "C:\\Program Files\\Microsoft Visual Studio\\2022\\Community\\VC\\Auxiliary\\Build\\vcvars64.bat"

    if Path(msvc_2019_path).exists():
        vcvars = msvc_2019_path
    # Prefer newer MSVC and override if exists
    if Path(msvc_2022_path).exists():
        vcvars = msvc_2022_path
    if not vcvars:
        raise RuntimeError(
            "No Visual Studio installation found, only 2019 and 2022 are supported.")

    print(f"\n\nLoading MSVC env variables via {vcvars}\n\n")

    cmd = [vcvars, '&&', 'set']
    popen = subprocess.Popen(
        cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    stdout, stderr = popen.communicate()

    if popen.wait() != 0:
        raise ValueError(stderr.decode("mbcs"))
    output = stdout.decode("mbcs").split("\r\n")
    return dict((e[0].upper(), e[1]) for e in [p.rstrip().split("=", 1) for p in output] if len(e) == 2)


def get_latest_git_tag():
    try:
        tag = subprocess.check_output(
            ["git", "describe", "--tags"]).decode("utf-8").strip()
        return tag
    except subprocess.CalledProcessError:
        print("Error fetching the Git tag.")
        return None


def parse_semver(tag):
    # Regular expression to match semver
    # Like v0.15.1-RC1-305-g18b8c402
    # Do NOT add a - between RC and the version number (1 in this example)
    pattern = r'(?i)^v?(\d+)\.(\d+)\.(\d+)(?:-([a-zA-Z0-9]+))?(?:-(\d+)-g([a-f0-9]+))?$'
    match = re.match(pattern, tag)
    if match:
        major, minor, patch, pre_release, commits_since, commit_hash = match.groups()
        return {
            'major': major,
            'minor': minor,
            'patch': patch,
            'pre_release': pre_release,
            'commits_since': commits_since,
            'commit_hash': commit_hash
        }
    else:
        return None


def semver_to_string(semver_dict):
    version_str = f"v{semver_dict['major']}.{semver_dict['minor']}.{semver_dict['patch']}"
    if semver_dict['pre_release']:
        version_str += f"-{semver_dict['pre_release']}"
    return version_str


def unzip(zip_path: str, destination_path: str, specific_file: str = None):
    with zipfile.ZipFile(zip_path, 'r') as zip_ref:
        if specific_file:
            zip_ref.extract(specific_file, path=destination_path)
        else:
            zip_ref.extractall(path=destination_path)


def listfiles(path):
    files = []
    extensions = ('.dylib', '.so','')
    ignored = ('qmldir')
    print(f"WALK: {path}")
    for dirName, subdirList, fileList in os.walk(path):
        dir = dirName.replace(path, '')
        for fname in fileList:
            if Path(fname).suffix in extensions and not fname in ignored:
                file =  path + os.path.join(dir, fname)
                if(os.path.isfile(file)):
                    files.append(file)
                    if  os.path.islink(file):
                        print(f"Warning: file {file} is a symlink!")
                        print("Symlink target: ", os.readlink(file))
    return files


def check_universal_binary():
    dir = 'build-64-osx-universal-release/bin/ScreenPlay.app/Contents'
    path = Path.joinpath(repo_root_path(), dir).absolute()
    print(f"Checking files at: {path}")
    files = listfiles(str(path))
    none_fat_found = False
    for file in files:
        out = run_and_capture_output(f"lipo -info {file}")
        if out.startswith('Non-fat'):
            print(out)
            none_fat_found = True
    if none_fat_found:
        print("✅ All files are a universal binaries")
    else:
        print("❌ None universal binaries found")