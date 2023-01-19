#!/usr/bin/python3
# SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
import os
import sys
import subprocess
from sys import stdout

stdout.reconfigure(encoding='utf-8')

def install_requirements():
    print("Set up required python modules")
    script_path = os.path.dirname(os.path.realpath(__file__))
    subprocess.check_call([sys.executable, "-m", "pip", "install", "-r", script_path + "/requirements.txt"])

if __name__ == "__main__":
    install_requirements()
