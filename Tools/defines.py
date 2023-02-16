#!/usr/bin/python3
# SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
import sys
from pathlib import Path
from sys import stdout
import os

stdout.reconfigure(encoding='utf-8')

# Defined by Qt
if sys.platform == "win32":
    OS = "windows"
    QT_PLATFORM = "msvc2019_64"
elif sys.platform == "darwin":
    OS = "mac"
    QT_PLATFORM = "macos"
elif sys.platform == "linux":
    OS = "linux"
    QT_PLATFORM = "gcc_64"

SCREENPLAY_VERSION = "0.15.0-RC5"
QT_PATH =  path = Path(os.path.join(os.path.realpath(__file__), "../../../aqt")).resolve()
QT_VERSION = "6.5.0"
QT_BIN_PATH = QT_PATH.joinpath(f"{QT_VERSION}/{QT_PLATFORM}/bin")
QT_TOOLS_PATH = QT_PATH.joinpath("Tools/")
QT_IFW_VERSION = "4.5"
VCPKG_VERSION = "1cc9525"  # Master 15.02.2023
PYTHON_EXECUTABLE = "python" if sys.platform == "win32" else "python3"
FFMPEG_VERSION = "5.0.1"
