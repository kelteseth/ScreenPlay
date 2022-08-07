#!/usr/bin/python3
import os
import sys
import subprocess

def install_requirements():
    print("Set up required python modules")
    script_path = os.path.dirname(os.path.realpath(__file__))
    subprocess.check_call([sys.executable, "-m", "pip", "install", "-r", script_path + "/requirements.txt"])

if __name__ == "__main__":
    install_requirements()
