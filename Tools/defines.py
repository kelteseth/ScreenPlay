import sys
from pathlib import Path

VCPKG_VERSION = "e2667a41fc2fc578474e9521d7eb90b769569c83"  # Master 07.10.2022
QT_VERSION = "6.3.2"
QT_IFW_VERSION = "4.4"
SCREENPLAY_VERSION = "0.15.0-RC1"
AQT_PATH = Path("C:/aqt") if sys.platform == "win32" else Path("~/aqt")
PYTHON_EXECUTABLE = "python" if sys.platform == "win32" else "python3"