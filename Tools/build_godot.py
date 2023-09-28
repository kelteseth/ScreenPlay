#!/usr/bin/python3
import os
import util
import defines
from pathlib import Path
from execute_util import execute
import argparse


def main():
    # Parse build folder as arugment

    parser = argparse.ArgumentParser(description='Build K3000Map to the bin build folder: D:/Backup/Code/Qt/build_ScreenPlay_Qt_6.5.2_MSVC_Debug/bin')
    parser.add_argument('--build_path', dest="build_path", type=str,  help='Build folder')
    parser.add_argument('--skip_if_exists', dest="skip_if_exists", default=False, action="store_true",   help='Skips the build if the index.html file exists. This is used for faster CMake configure')
 
    args = parser.parse_args()

    if not args.build_path:
        print("ERROR: Please specify the build folder")
        print("py build_godot.py --build_path D:/Backup/Code/Qt/build_ScreenPlay_Qt_6.5.2_MSVC_Debug/bin/")
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
    
    build_godot(abs_build_path)
    
def build_godot(abs_build_path: str):
    project_path = Path(util.repo_root_path()).joinpath("ScreenPlayWallpaper/Godot/ScreenPlayGodot").resolve()
    apps_path = os.path.join(util.repo_root_path(),"Tools/Apps/Godot")
    godot_executable = os.path.join(apps_path, defines.GODOT_EDITOR_EXECUTABLE)
    screenPlayWallpaperGodot_executable = Path(abs_build_path).joinpath(defines.SCREENPLAYWALLPAPER_GODOT_EXECUTABLE).resolve()
    export_command = f'"{godot_executable}" -v --headless --export-release "Windows Desktop" "{screenPlayWallpaperGodot_executable}"'

    # We get random error on successful export, so lets ignore it
    execute(command=export_command,workingDir=project_path,ignore_error=True)

if __name__ == "__main__":
     main()
