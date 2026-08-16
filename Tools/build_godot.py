#!/usr/bin/python3
# SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
import os
import util
import shutil
import defines
import sys
import glob
from pathlib import Path
from execute_util import execute
import argparse


def main():
    # Parse build folder as arugment

    parser = argparse.ArgumentParser(description='Build SP to the bin build folder: D:/Backup/Code/Qt/build_ScreenPlay_Qt_6.11.1_MSVC_Debug/bin')
    parser.add_argument('--build_path', dest="build_path", type=str,  help='Build folder')
    parser.add_argument('--skip_if_exists', dest="skip_if_exists", default=False, action="store_true",   help='Skips the build if the index.html file exists. This is used for faster CMake configure')
 
    args = parser.parse_args()

    if not args.build_path:
        print("ERROR: Please specify the build folder")
        print("py build_godot.py --build_path D:/Backup/Code/Qt/build_ScreenPlay_Qt_6.11.1_MSVC_Debug/bin/")
        exit()

    # if build path exists and contains a index.html file, skip the build
    if args.skip_if_exists:
        screenPlayWallpaperGodot_executable = Path(args.build_path).joinpath(defines.SCREENPLAYWALLPAPER_GODOT_EXECUTABLE)
        if screenPlayWallpaperGodot_executable.exists:
            print(f"Skipping build, because {defines.SCREENPLAYWALLPAPER_GODOT_EXECUTABLE} exists")
            exit(1)

    abs_build_path = args.build_path
    if not os.path.isabs(args.build_path):
        abs_build_path = os.path.abspath(os.path.join(os.getcwd(), args.build_path))

    if 'Debug' in abs_build_path:
        build_type = "debug"
    else:
        build_type = "release"
    
    build_godot(abs_build_path, build_type)
    
def build_godot(abs_build_path: str,  build_type: str):
    project_path = Path(util.repo_root_path()).joinpath("ScreenPlayWallpaper/Godot/ScreenPlayGodot").resolve()
    apps_path = os.path.join(defines.THIRDPATH_PATH,"Godot")
    godot_executable = os.path.join(apps_path, defines.GODOT_EDITOR_EXECUTABLE)
    screenPlayWallpaperGodot_executable = Path(abs_build_path).joinpath(defines.SCREENPLAYWALLPAPER_GODOT_EXECUTABLE).resolve()
    
    # Determine platform-specific export target and library folder
    is_debug = 'debug' in build_type.lower()
    if sys.platform == "win32":
        platform_folder = "Windows-AMD64"
        export_target = "Windows Desktop MSVC Debug" if is_debug else "Windows Desktop MSVC Release"
        lib_extensions = ["*.dll"]
    elif sys.platform == "darwin":
        platform_folder = "Darwin-x86_64"  # Could also be Darwin-arm64 for Apple Silicon
        export_target = "macOS"
        lib_extensions = ["*.dylib", "*.so"]
    elif sys.platform == "linux":
        platform_folder = "Linux-x86_64"
        export_target = "Linux"
        lib_extensions = ["*.so"]
    else:
        raise Exception(f"Unsupported platform: {sys.platform}")
    
    if is_debug:
        export_type = " --export-debug"
    else:
        export_type = " --export-release"
    export_command = f'"{godot_executable}" -v --headless {export_type} "{export_target}" "{screenPlayWallpaperGodot_executable}"'

    # We get random error on successful export, so lets ignore it
    execute(command=export_command,workingDir=project_path,ignore_error=True)

    # Construct the source path for the libraries
    lib_source_folder = project_path.joinpath(f"ScreenPlayGodotWallpaper/lib/{platform_folder}")
    
    if not lib_source_folder.exists():
        print(f"⚠️ Library folder {lib_source_folder} does not exist, checking for alternative naming...")
        
        # Try alternative naming conventions
        lib_base_folder = project_path.joinpath("ScreenPlayGodotWallpaper/lib")
        if lib_base_folder.exists():
            available_folders = [f.name for f in lib_base_folder.iterdir() if f.is_dir()]
            print(f"Available library folders: {available_folders}")
            
            # Try to find a suitable folder based on platform
            if sys.platform == "win32" and any("Windows" in folder for folder in available_folders):
                platform_folder = next(folder for folder in available_folders if "Windows" in folder)
            elif sys.platform == "darwin" and any("Darwin" in folder or "macOS" in folder for folder in available_folders):
                platform_folder = next(folder for folder in available_folders if "Darwin" in folder or "macOS" in folder)
            elif sys.platform == "linux" and any("Linux" in folder for folder in available_folders):
                platform_folder = next(folder for folder in available_folders if "Linux" in folder)
            
            lib_source_folder = lib_base_folder.joinpath(platform_folder)
    
    if not lib_source_folder.exists():
        print(f"❌ Error: Library folder {lib_source_folder} does not exist")
        return

    print(f"📁 Copying libraries from {lib_source_folder} to {abs_build_path}")
    
    # Copy all libraries matching the platform's extensions
    copied_files = []
    for extension in lib_extensions:
        for lib_file in lib_source_folder.glob(extension):
            # Skip temporary files and debug symbols we don't need in release
            if lib_file.name.startswith('~') or lib_file.suffix in ['.tmp', '.ilk']:
                continue
            
            # Skip debug PDB files in release builds
            if 'release' in build_type.lower() and lib_file.suffix == '.pdb':
                continue
                
            dest_path = Path(abs_build_path) / lib_file.name
            print(f"  📄 Copying {lib_file.name}")
            shutil.copy2(lib_file, dest_path)
            copied_files.append(lib_file.name)
    
    if copied_files:
        print(f"✅ Successfully copied {len(copied_files)} library files: {', '.join(copied_files)}")
    else:
        print(f"⚠️ No library files found to copy from {lib_source_folder}")





if __name__ == "__main__":
     main()
