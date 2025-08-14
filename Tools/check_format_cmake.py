#!/usr/bin/python3
# SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
import os
import argparse
import util
from format_util import find_files
from format_util import execute_threaded
from sys import stdout

stdout.reconfigure(encoding='utf-8')


def format_file_function(file):
    executable = "cmake-format"
    if os.name == 'nt':
        executable += ".exe"
    config_path = os.path.join(util.repo_root_path(), ".cmake-format.py")
    os.system(" %s -c %s -i %s" % (executable, config_path, file))
    print("Format: ", file)


def check_format_file_function(file):
    executable = "cmake-format"
    if os.name == 'nt':
        executable += ".exe"
    config_path = os.path.join(util.repo_root_path(), ".cmake-format.py")
    result = os.system(" %s -c %s --check %s" %
                       (executable, config_path, file))

    # If the return code is non-zero, the file isn't formatted correctly
    if result != 0:
        print(f"{file} is not correctly formatted.")
        return False
    return True


def main(git_staged_only=False, check_only=False):
    exclude_folders = ("ThirdParty","Build","ScreenPlayWallpaper/Godot/GDExtention/extern/godot-cpp")
    file_list = find_files(
        ('CMakeLists.txt', '*.cmake'), util.repo_root_path(), git_staged_only, exclude_folders)

    if check_only:
        result = execute_threaded(file_list, check_format_file_function)
        if not result:  # Since result is a single boolean, you can directly check its value
            print("Some files are not correctly formatted!")
            exit(1)
    else:
        execute_threaded(file_list, format_file_function)


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('-s', '--stage-only', action="store_true", dest="stage_only", default=False,
                        help="Check/format only staged files")
    parser.add_argument('-c', '--check', action="store_true", dest="check_only", default=False,
                        help="Only check if files are correctly formatted without actually formatting them")

    args = parser.parse_args()
    main(args.stage_only, args.check_only)
