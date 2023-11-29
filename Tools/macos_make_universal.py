#!/usr/bin/python3
# SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
from distutils.dir_util import mkpath
import os
import subprocess
from pathlib import Path
import util
from util import run, run_and_capture_output
from sys import stdout

stdout.reconfigure(encoding='utf-8')


def listfiles(path):
    files = []
    ignored = ('.DS_Store',)  # Ignored files
    print(f"WALK: {path}")

    for dirName, _, fileList in os.walk(path):
        for fname in fileList:
            if fname in ignored:
                continue  # Skip ignored files

            file_path = os.path.join(dirName, fname)
            if os.path.isfile(file_path) and (fname.endswith('.a')):
                files.append(file_path)
                if os.path.islink(file_path):
                    print(f"Warning: file {file_path} is a symlink!")
                    print("Symlink target: ", os.readlink(file_path))
    return files

def is_mach_o_binary(file_path):
    """
    Check if a file is a Mach-O binary using the file command.
    """
    try:
        result = subprocess.run(["file", file_path], capture_output=True, text=True)
        return 'Mach-O' in result.stdout
    except Exception as e:
        print(f"Error checking file type of {file_path}: {e}")
        return False

# Merges x64 and arm64 vcpkg build into universal
def run_lipo():
    workspace_path = util.workspace_path()
    vcpkg_installed_path = str(Path(workspace_path).joinpath("vcpkg/installed/").absolute())
    arm64_dir = f"{vcpkg_installed_path}/arm64-osx" 
    x64_dir = f"{vcpkg_installed_path}/x64-osx" 
    universal_dir = f"{vcpkg_installed_path}/64-osx-universal" 
    # Ensure the universal directory is created and empty
    run(f"rm -rf {universal_dir}", workspace_path)
    # Copy the x64 build as a base for the universal build
    run(f"cp -a {arm64_dir} {universal_dir}", workspace_path)

    files = listfiles(str(universal_dir))

    for file in files:
        # Extract the relative path for file
        rel_path = os.path.relpath(file, universal_dir)

        # Construct the corresponding arm64 and x64 file paths
        arm64_file = os.path.join(arm64_dir, rel_path)
        x64_file = os.path.join(x64_dir, rel_path)
        universal_file = file  # file is already in universal_dir
        
            #and is_mach_o_binary(arm64_file)
        if os.path.exists(x64_file) :
            run(f"lipo -create {arm64_file} {x64_file} -output {universal_file}")
            print(f"Processing binary file: {universal_file}")
            run(f"lipo -info {universal_file}")
        else:
            print(f"Skipping non-binary file: {universal_file}")

def check_fat_binary():
    # Ensure the script starts from the correct directory
    workspace_path = Path(util.workspace_path())

    dir = 'vcpkg/installed/64-osx-universal'
    universal_dir = str(workspace_path.joinpath(dir))
    print(f"check_fat_binary {universal_dir}")
    files = listfiles(universal_dir)
    
    for file in files:
        out = run_and_capture_output(f"lipo -info {file}")
        if out.startswith('Non-fat'):
            out = out.replace("\n","")
            print(f"❌ {out}")
        else:
            print(f"✅ {file}")
def execute():
    run_lipo()
    check_fat_binary()

if __name__ == "__main__":
    execute()
