import os
import subprocess
import argparse
from format_util import find_files
from format_util import check_git_exit
from format_util import execute_threaded


def format_file_function(file):
    executable = "clang-format"
    if os.name == 'nt':
        executable = "clang-format.exe"
    process = subprocess.run(" %s -style=file -i %s" %
                             (executable, file), capture_output=True, shell=True)
    print("Format: %s \t return: %s" % (file, process.returncode))


def main(git_staged_only=False):
    file_list = find_files(('.cpp', '.h'), "", git_staged_only)
    execute_threaded(file_list, format_file_function)
    if not git_staged_only:
        check_git_exit("Clang Format")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('-s', action="store_true", dest="stage_only",
                        default=False)
    args = parser.parse_args()
    main(args.stage_only)
