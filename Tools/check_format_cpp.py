#!/usr/bin/python3
# SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
import os
import subprocess
import argparse
import util
from format_util import find_files
from format_util import execute_threaded
from sys import stdout

stdout.reconfigure(encoding='utf-8')


def format_file_function(file):
    executable = "clang-format"
    if os.name == 'nt':
        executable = "clang-format.exe"
    process = subprocess.run(" %s -style=file -i %s" %
                             (executable, file), capture_output=True, shell=True)
    print("Format: %s \t return: %s" % (file, process.returncode))


def check_format_file_function(file):
    executable = "clang-format"
    if os.name == 'nt':
        executable += ".exe"
    result = subprocess.run(" %s -style=file --output-replacements-xml %s" %
                            (executable, file), capture_output=True, shell=True, text=True)

    # Check for opening replacement tag with attributes (a space after it indicates attributes)
    if "<replacement " in result.stdout:
        print(f"{file} is not correctly formatted.")
        print(f"{result.stdout} ")
        return False
    return True


def main(git_staged_only=False, check_only=False):
    exclude_folders = ("ScreenPlayWorkshop/SteamSDK",
                       "ThirdParty","Build", "ScreenPlayWallpaper/Godot/GDExtention/extern/godot-cpp")
    file_list = find_files(
        ('.cpp', '.h'), util.repo_root_path(), git_staged_only, exclude_folders)

    if check_only:
        result = execute_threaded(file_list, check_format_file_function)
        if not result:  # Since result is a single boolean, you can directly check its value
            print("Some files are not correctly formatted!")
            exit(1)
    else:
        execute_threaded(file_list, format_file_function)


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('--s', '--stage-only', action="store_true", dest="stage_only", default=False,
                        help="Check/format only staged files")
    parser.add_argument('--c', '--check', action="store_true", dest="check_only", default=False,
                        help="Only check if files are correctly formatted without actually formatting them")

    args = parser.parse_args()
    main(args.stage_only, args.check_only)
