#!/usr/bin/python3
# SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
import subprocess
import os
import shutil
from sys import stdout
import defines

stdout.reconfigure(encoding='utf-8')

def main():

    shutil.rmtree('../Docs/html/')
    os.mkdir('../Docs/html/')
    
    qt_bin_path = defines.QT_BIN_PATH
    executable = "qdoc"
    if os.name == 'nt':
        executable = "qdoc.exe"
    executable = qt_bin_path.joinpath(executable)

    process = subprocess.Popen(f"{executable} ../Docs/config.qdocconf", stdout=subprocess.PIPE)

if __name__ == "__main__":
    main()
