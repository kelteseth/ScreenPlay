#!/usr/bin/python3
# SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
import os
import subprocess
import argparse
import sys
import defines
from format_util import find_files
from format_util import check_git_exit
from format_util import execute_threaded
from sys import stdout

stdout.reconfigure(encoding='utf-8')


def format_file_function(file):
    executable = "qmlformat"
    if os.name == 'nt':
        executable = "qmlformat.exe"
    qt_bin_path = defines.QT_BIN_PATH
    executable = qt_bin_path.joinpath(executable)

    process = subprocess.run(
        "%s -i %s" % (executable, file), capture_output=True, shell=True)
    print("Format: %s \t return: %s" % (file, process.returncode))


def main(git_staged_only=False):
    file_list = find_files(('.qml'), os.path.abspath(os.path.join(os.getcwd(), "../")), git_staged_only)
    execute_threaded(file_list, format_file_function)
    if not git_staged_only:
        check_git_exit("QML Format")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('-s', action="store_true", dest="stage_only",
                        default=False)
    args = parser.parse_args()
    main(args.stage_only)
