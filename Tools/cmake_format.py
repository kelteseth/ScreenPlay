import os
import argparse
from format_util import find_files
from format_util import check_git_exit
from format_util import execute_threaded

def format_file_function(file):
    executable = "cmake-format"
    if os.name == 'nt':
        executable = "cmake-format.exe"
    os.system(" %s -c ../.cmake-format.py -i %s" % (executable, file))
    print("Format: ", file)

def main(git_staged_only=False):
    file_list = find_files(('CMakeLists.txt'), "", git_staged_only)
    execute_threaded(file_list, format_file_function)
    if not git_staged_only:
        check_git_exit("CMake Format")

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('-s', action="store_true", dest="stage_only",
                        default=False)
    args = parser.parse_args()
    main(args.stage_only)
