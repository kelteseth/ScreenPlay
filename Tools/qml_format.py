import os
import subprocess
import argparse
import sys
from format_util import find_files
from format_util import check_git_exit
from format_util import execute_threaded
from format_util import find_absolute_qt_path


def format_file_function(file):
    executable = "qmlformat"
    if os.name == 'nt':
        executable = "qmlformat.exe"
    qt_bin_path = os.path.join(find_absolute_qt_path("5.15.2"), "bin")
    executable = os.path.join(qt_bin_path, executable)

    process = subprocess.run(
        "%s -n -i %s" % (executable, file), capture_output=True, shell=True)
    print("Format: %s \t return: %s" % (file, process.returncode))


def main(git_staged_only=False):
    if "" == find_absolute_qt_path("5.15.2"):
        print("No suitable qt version found!")
        sys.exit(1)
    
    file_list = find_files(('.qml'), "Apps/Client/qml", git_staged_only)
    execute_threaded(file_list, format_file_function)
    if not git_staged_only:
        check_git_exit("QML Format")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('-s', action="store_true", dest="stage_only",
                        default=False)
    args = parser.parse_args()
    main(args.stage_only)
