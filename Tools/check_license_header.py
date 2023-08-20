#!/usr/bin/python3
# SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
import argparse
from format_util import find_files
from format_util import execute_threaded
from sys import stdout
import util

stdout.reconfigure(encoding='utf-8')


def check_license_header_file_function(file_path):
    """
    Check if the given file has a license header in its first or second line.

    Parameters:
    - file_path (str): Path to the file to be checked.

    Returns:
    - bool: True if the license header is present, otherwise False.
    """
    try:
        with open(file_path, 'r') as f:
            # Read the first two lines of the file
            first_line = f.readline().strip()
            second_line = f.readline().strip()

            # Check if either of the two lines has the required header
            if "SPDX-License-Identifier:" in first_line or "SPDX-License-Identifier:" in second_line:
                return True
            else:
                print(f"INVALID: {file_path}")
                return False
    except Exception as e:
        print(f"Error reading the file: {e}")
        return False


def main(git_staged_only=False):

    # Setting exclude_folders before calling the function
    exclude_folders = (
        "ThirdParty", "ScreenPlayWorkshop/SteamSDK", "build-x64-windows-release")
    file_endings = ('.cpp', '.h', '.qml')
    file_list = find_files(file_endings_tuple=file_endings,
                           path=util.repo_root_path(),
                           git_staged_only=git_staged_only,
                           exclude_folders=exclude_folders)
    result = execute_threaded(file_list, check_license_header_file_function)
    if result is False:
        print("At least one file is missing the license header!")
        # Depending on your requirement, you can exit the script here
        exit(1)


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--s", '--stage-only', action="store_true", dest="stage_only",
                        default=False)
    args = parser.parse_args()
    main(args.stage_only)
