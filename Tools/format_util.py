import subprocess
import sys
import os
from multiprocessing import cpu_count
from multiprocessing import Pool
from multiprocessing import dummy
from pathlib import Path


def find_all_files(path) -> []:
    file_list = []
    for root, dirnames, files in os.walk(path):
        for filename in files:
            file_list.append(os.path.join(root, filename))
    return file_list


def find_all_git_staged_files() -> []:
    process = subprocess.run("git diff --name-only --staged",
                             capture_output=True, shell=True)
    out = process.stdout.decode("utf-8")
    out = out.splitlines()
    return out


def find_files(file_endings_tuple, path="", git_staged_only=False) -> []:
    file_list = []
    # Get the root folder by moving one up
    root = Path(__file__).parent.absolute()
    root = os.path.abspath(os.path.join(root, "../"))

    root = os.path.join(root, path)
    print(root)

    if git_staged_only:
        file_list = find_all_git_staged_files()
    else:
        file_list = find_all_files(root)

    filtered_file_list = []
    for filename in file_list:
        if filename.endswith(file_endings_tuple):
            filtered_file_list.append(os.path.join(root, filename))

    return filtered_file_list


def execute_threaded(file_list, format_file_function):
    p = Pool(cpu_count())
    p.map(format_file_function, file_list)
    p.close()
    p.join()


def check_git_exit(caller_name):
    # Check if all files are formatter
    process = subprocess.run("git --no-pager diff",
                             capture_output=True, shell=True)

    out = process.stdout.decode("utf-8")

    if out != "":
        print("\n########### %s DONE ###########\n" % caller_name)
        print("Git diff is not empty. This means your files where not correctly formatted!")
        out.replace("\\n", "\n")
        # print(out)
        sys.exit(1)


def find_absolute_qt_path(qt_version) -> os.path:
    compiler = ""
    if os.name != 'nt':
        print("find_absolute_qt_path is only implemented for Windows!")
        sys.exit(1)

    compiler = "msvc2019_64"

    qt_base_path = "C:\\Qt"
    if not os.path.exists(qt_base_path):
        print("No suitable Qt version found! Searching for version %s" % qt_version)
        print("Path searches the root Qt version folder like: C:/Qt/6.0.0/msvc2019_64")
        sys.exit(1)

    absolute_qt_path = os.path.join(qt_base_path, qt_version)
    if os.path.exists(absolute_qt_path):
        return os.path.join(absolute_qt_path, compiler)
    else:
        print("No suitable Qt version found! Searching for version %s" % qt_version)
        print("Path searches the root Qt version folder like: C:/Qt/6.0.0/msvc2019_64")
        sys.exit(1)
