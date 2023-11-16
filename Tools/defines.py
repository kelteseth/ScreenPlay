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

REPO_PATH =  Path(__file__, "../../").resolve()
THIRDPATH_PATH =  Path(REPO_PATH, "ThirdParty").resolve()
QT_PATH = path = Path(REPO_PATH, "../aqt").resolve()
QT_VERSION = "6.6.0"
QT_BIN_PATH = QT_PATH.joinpath(f"{QT_VERSION}/{QT_PLATFORM}/bin")
QT_TOOLS_PATH = QT_PATH.joinpath("Tools/")
QT_IFW_VERSION = "4.6"
# 02.06.2023 https://gitlab.com/kelteseth/screenplay-vcpkg :
VCPKG_VERSION = "f06975f46d8c7a1dad916e1e997584f77ae0c34a"
VCPKG_BASE_PACKAGES =  [
        "curl",
        "cpp-httplib",
        "libarchive",
        "fmt",
        "catch2"
    ]
PYTHON_EXECUTABLE = "python" if sys.platform == "win32" else "python3"
FFMPEG_VERSION = "6.0"
GODOT_VERSION = "4.2"
GODOT_RELEASE_TYPE = "beta6"
GODOT_DOWNLOAD_SERVER = "https://github.com/godotengine/godot-builds/releases/download"
if sys.platform == "win32":
    SCREENPLAYWALLPAPER_GODOT_EXECUTABLE = "ScreenPlayWallpaperGodot.exe"
    GODOT_EDITOR_EXECUTABLE = f"Godot_v{GODOT_VERSION}-{GODOT_RELEASE_TYPE}_win64.exe"
    GODOT_EDITOR_DOWNLOAD_NAME = GODOT_EDITOR_EXECUTABLE  + ".zip"
    GODOT_TEMPLATES_PATH = os.path.join(os.getenv(
        'APPDATA'), f"Godot/templates/{GODOT_VERSION}.{GODOT_RELEASE_TYPE}")
elif sys.platform == "darwin":
    SCREENPLAYWALLPAPER_GODOT_EXECUTABLE = "ScreenPlayWallpaperGodot.app"
    # Godot_v4.2-beta6_macos.universal.zip 
    GODOT_EDITOR_EXECUTABLE = "Godot.app"
    GODOT_EDITOR_DOWNLOAD_NAME =  f"Godot_v{GODOT_VERSION}-{GODOT_RELEASE_TYPE}_macos.universal.zip"
    GODOT_TEMPLATES_PATH = "TODO"
elif sys.platform == "linux":
    SCREENPLAYWALLPAPER_GODOT_EXECUTABLE = "ScreenPlayWallpaperGodot"
    # Godot_v4.2-beta6_linux.x86_64
    GODOT_EDITOR_EXECUTABLE = f"Godot_v{GODOT_VERSION}-{GODOT_RELEASE_TYPE}_linux.x86_64"
    GODOT_EDITOR_DOWNLOAD_NAME = GODOT_EDITOR_EXECUTABLE + ".zip"
    # /home/eli/.local/share/godot/templates/
    GODOT_TEMPLATES_PATH = os.path.join(
        Path.home(), f".local/share/godot/templates/{GODOT_VERSION}.{GODOT_RELEASE_TYPE}")

