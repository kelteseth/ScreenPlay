#!/usr/bin/python3
# SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
import os
from pathlib import Path
import util
import argparse

def find_files(path, pattern, file_extensions):
    files = []
    for root, dirnames, filenames in os.walk(path):
        for filename in filenames:
            if any(filename.endswith(ext) for ext in file_extensions):
                file_path = os.path.join(root, filename)
                with open(file_path) as f:
                    contents = f.read()
                    if any(substring in contents for substring in pattern):
                        files.append(file_path)
                        print(f"Found file {file_path}")
    return files


def check_moc_include(filename, add_include=False):
    cppfile = Path(filename)
    # replace cppfile ending with .cpp
    cppfile = cppfile.with_suffix(".cpp").resolve()
    # check if cpp file exists
    if not cppfile.exists():
        # check if cpp file exists but 3 directories up and then in src
        cppfile = cppfile.joinpath(
            "../../../../src/{}".format(cppfile.name)).resolve()
        if not cppfile.exists():
            print(f"FAIL: {cppfile} does not exist")
            return
        print(f"Check {cppfile}")

    with open(cppfile) as f:
        if '#include "moc_{}"'.format(os.path.basename(cppfile)) in f.read():
            print("OK: {} HAS moc include".format(cppfile))
        else:
            print("FAIL: {} MISSES moc include".format(cppfile))
            if add_include:
                with open(cppfile, 'a') as f:
                    f.write("\n#include \"moc_{}\"\n".format(
                        os.path.basename(cppfile)))
                    print("Adding include...")
            else:
                print("Add this line to the end of the file:")
                print("#include \"moc_{}\"".format(os.path.basename(cppfile)))


if __name__ == '__main__':
    parser = argparse.ArgumentParser(
        description="Check if moc includes are present and add them if requested.")
    parser.add_argument('-a', '--addinclude', action='store_true',
                        help='Automatically add missing moc includes')
    args = parser.parse_args()
    repo_root = util.repo_root_path()
    filenames = find_files(repo_root, ['Q_OBJECT', 'Q_GADGET', 'Q_NAMESPACE'], [
                           '.cpp', '.h', "main.cpp"])
    for filename in filenames:
        check_moc_include(filename, args.addinclude)
    if not args.addinclude:
        print("Run this script with the '-a' or '--addinclude' flag to automatically add the moc includes")
