#!/usr/bin/python3
# SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
import subprocess
import sys
import os
import util
from multiprocessing import cpu_count
from multiprocessing import Pool
from multiprocessing import dummy
from pathlib import Path
from sys import stdout

stdout.reconfigure(encoding='utf-8')


def find_all_files(path):
    file_list = []
    for root, dirnames, files in os.walk(path):
        for filename in files:
            file_list.append(os.path.join(root, filename))
    return file_list


def find_all_git_staged_files():
    process = subprocess.run("git diff --name-only --staged",
                             capture_output=True, shell=True)
    out = process.stdout.decode("utf-8")
    out = out.splitlines()
    return out


def find_files(file_endings_tuple, path="", git_staged_only=False, exclude_folders=()):
    file_list = []

    if git_staged_only:
        file_list = find_all_git_staged_files()  # Assuming you've defined this
    else:
        for dirpath, dirnames, filenames in os.walk(path):
            # Normalize the current directory path
            norm_dirpath = os.path.normpath(dirpath)

            # Check if the current directory is to be excluded
            if exclude_folders and any(os.path.normpath(excl_folder) in norm_dirpath for excl_folder in exclude_folders):
                continue

            for filename in filenames:
                if filename.endswith(file_endings_tuple):
                    file_list.append(os.path.join(dirpath, filename))

    return file_list


def execute_threaded(file_list, format_file_function):
    if not file_list:
        return True  # or False, depending on how you want to handle an empty list

    with Pool(cpu_count()) as p:
        results = p.map(format_file_function, file_list)

    # Check if any result is False and return accordingly
    if any(result is False for result in results):
        return False
    return True


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
