#!/usr/bin/python3
# SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
import os
import subprocess
import argparse
import util
import defines
import hashlib
from format_util import find_files
from format_util import execute_threaded
from sys import stdout, exit
import hashlib
import shutil

stdout.reconfigure(encoding='utf-8')


def format_qml_file(file):
    executable = "qmlformat"
    if os.name == 'nt':
        executable = "qmlformat.exe"
    qt_bin_path = defines.QT_BIN_PATH
    executable = qt_bin_path.joinpath(executable)

    # Add -i for formatting in place
    process = subprocess.run(
        [executable, "-i", file], capture_output=True, shell=True)
    print("Format: %s \t return: %s" % (file, process.returncode))


def compute_md5(file_path):
    """Compute MD5 hash of the content of the given file."""
    with open(file_path, 'rb') as f:
        return hashlib.md5(f.read()).hexdigest()


# Instead of comparing the direct outputs (which was leading to ambiguous results),
# this function uses a workaround that checks the file's MD5 hash before and after formatting.
def check_format_qml_file(file):
    executable = "qmlformat"
    if os.name == 'nt':
        executable = "qmlformat.exe"
    qt_bin_path = defines.QT_BIN_PATH
    executable = qt_bin_path.joinpath(executable)

    # Step 1: Copy the original file
    backup_file = file + ".backup"
    shutil.copy(file, backup_file)

    # Step 2: Run qmlformat with in-place editing
    subprocess.run([executable, "-i", file], check=True)

    # Step 3: Compare MD5 hash of the original (backup) and the formatted file
    original_hash = compute_md5(backup_file)
    formatted_hash = compute_md5(file)

    # Step 4: Remove the backup copy
    os.remove(backup_file)

    # Step 5: Return the comparison result
    if original_hash != formatted_hash:
        print(f"{file} is not correctly formatted.")
        return False
    return True


def main(git_staged_only=False, check_only=False):
    exclude_folders = ("ThirdParty", "build-x64-windows-release")
    file_list = find_files(('.qml'), util.repo_root_path(),
                           git_staged_only, exclude_folders)

    if check_only:
        result = execute_threaded(file_list, check_format_qml_file)
        if not result:
            print("Some files are not correctly formatted!")
            exit(1)
    else:
        execute_threaded(file_list, format_qml_file)


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('--s', '--stage-only', action="store_true", dest="stage_only", default=False,
                        help="Check/format only staged files")
    parser.add_argument('--c', '--check', action="store_true", dest="check_only", default=False,
                        help="Only check if files are correctly formatted without actually formatting them")

    args = parser.parse_args()
    main(args.stage_only, args.check_only)
