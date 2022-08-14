#!/usr/bin/python3
import platform
import os
import subprocess
import shutil
import argparse
import time
import zipfile
from shutil import copytree
from pathlib import Path
from concurrent.futures import ThreadPoolExecutor
from datetime import datetime
from util import run, run_and_capture_output

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
                    #print(file)
                    files.append(file)
    return files


def create_fat_binary():
    # Make sure the script is always started from the same folder
    root_path = Path.cwd()
    if root_path.name == "Tools":
        root_path = root_path.parent
    print(f"Change root directory to: {root_path}")
    os.chdir(root_path)

    arm64_dir = 'build-arm64-osx-release/bin/ScreenPlay.app'
    x64_dir = "build-x64-osx-release/bin/ScreenPlay.app"
    arm64_files = listfiles(
        str(Path.joinpath(root_path, arm64_dir)))
    x64_files = listfiles(str(Path.joinpath(root_path, x64_dir)))

    for file in arm64_files:
        run(f"lipo -info {file}")

    # print(arm64_files)
    # print(x64_files)

def run_lipo():
      # Make sure the script is always started from the same folder
    root_path = Path.cwd()
    if root_path.name == "Tools":
        root_path = root_path.parent
    print(f"Change root directory to: {root_path}")
    os.chdir(root_path)

    shutil.copytree(str(Path.joinpath(root_path, "build-arm64-osx-release/bin/")) , 
                    str(Path.joinpath(root_path, "build-universal-osx-release/bin/")) )

    apps = ["ScreenPlay","ScreenPlayWallpaper", "ScreenPlayWidget"]
    for app in apps:
        arm64_dir = str(Path.joinpath(root_path, f"build-arm64-osx-release/bin/{app}.app/Contents/MacOS/{app}")) 
        x64_dir = str(Path.joinpath(root_path,   f"build-x64-osx-release/bin/{app}.app/Contents/MacOS/{app}")) 
        universal_dir = str(Path.joinpath(root_path,   f"build-universal-osx-release/bin/{app}.app/Contents/MacOS/{app}")) 
        run(f"lipo -create {arm64_dir} {x64_dir} -output {universal_dir}")
        run(f"lipo -info {universal_dir}")
       
def check_fat_binary():
    # Make sure the script is always started from the same folder
    root_path = Path.cwd()
    if root_path.name == "Tools":
        root_path = root_path.parent
    print(f"Change root directory to: {root_path}")
    os.chdir(root_path)

    dir = 'build-universal-osx-release/bin/'
    files = listfiles(str(Path.joinpath(root_path, dir)))

    for file in files:
        out = run_and_capture_output(f"lipo -info {file}")
        if out.startswith('Non-fat'):
            print(out)
        


if __name__ == "__main__":
    #run_lipo()
    check_fat_binary()
    #create_fat_binary()
