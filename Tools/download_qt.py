#!/usr/bin/python3

import argparse
import subprocess
from platform import platform, system
from pathlib import Path
from execute_util import execute


def run(cmd, cwd=Path.cwd()):
	result = subprocess.run(cmd, shell=True, cwd=cwd)
	if result.returncode != 0:
		raise RuntimeError(f"Failed to execute {cmd}")

def download_qt():
    root_path = Path.cwd()
    qt_version = "6.4.0"
    qt_installer_version = "4.4"
    qt_path = " "
    operating_system =  " "
    device_type = "desktop"
    qt_bin = "  "
    aqt_path =  Path(f"{root_path}/../../aqt/").resolve()

    parser = argparse.ArgumentParser(description='Installs qt')
    parser.add_argument('--path', help='')
    args = parser.parse_args()


    if system() == "Windows":
        operating_system = "windows"
        qt_bin = "win64_msvc2019_64"
    elif system() == "Darwin":
        operating_system = "mac"
        qt_bin = "clang_64"
    elif system() == "Linux":
        operating_system = "linux"
        qt_bin = "gcc_64"
        
    print(f"Download destination: {aqt_path} ")
    run("python3 -m pip install -U pip wheel")
    run("python3 -m pip install aqtinstall")
    run(f"python3 -m aqt install-qt -O {aqt_path} {operating_system} {device_type} {qt_version} {qt_bin} -m all")
    run(f"python3 -m aqt install-tool -O {aqt_path} {operating_system} {device_type} tools_ifw ")

if __name__ == "__main__":
    download_qt()
