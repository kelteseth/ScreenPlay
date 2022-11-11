import sys
from pathlib import Path
from sys import stdout

stdout.reconfigure(encoding='utf-8')

VCPKG_VERSION = "2871ddd"  # Master 11.11.2022
QT_VERSION = "6.3.2"
QT_IFW_VERSION = "4.5"
SCREENPLAY_VERSION = "0.15.0-RC3"
AQT_PATH = Path("C:/aqt") if sys.platform == "win32" else Path().home().joinpath("aqt")
PYTHON_EXECUTABLE = "python" if sys.platform == "win32" else "python3"
FFMPEG_VERSION = "5.0.1"