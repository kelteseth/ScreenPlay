import sys
from pathlib import Path
from sys import stdout

stdout.reconfigure(encoding='utf-8')

VCPKG_VERSION = "2871ddd"  # Master 11.11.2022
QT_VERSION = "6.4.2" if sys.platform != "darwin" else "6.3.2"
QT_IFW_VERSION = "4.5"
SCREENPLAY_VERSION = "0.15.0-RC3"
QT_PATH = Path.cwd().parent.parent.joinpath("aqt")
# Defined by Qt
if sys.platform == "win32":
    OS = "windows"
    QT_PLATFORM = "msvc2019_64"
elif sys.platform == "darwin":
    OS = "mac"
    QT_PLATFORM = "macos"
elif sys.platform ==  "linux":
    OS = "linux"
    QT_PLATFORM = "gcc_64"

PYTHON_EXECUTABLE = "python" if sys.platform == "win32" else "python3"
FFMPEG_VERSION = "5.0.1"