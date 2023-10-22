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

QT_PATH = path = Path(os.path.join(
    os.path.realpath(__file__), "../../../aqt")).resolve()
QT_VERSION = "6.6.0"
QT_BIN_PATH = QT_PATH.joinpath(f"{QT_VERSION}/{QT_PLATFORM}/bin")
QT_TOOLS_PATH = QT_PATH.joinpath("Tools/")
QT_IFW_VERSION = "4.6"
# 02.06.2023 https://gitlab.com/kelteseth/screenplay-vcpkg :
VCPKG_VERSION = "f06975f46d8c7a1dad916e1e997584f77ae0c34a"
PYTHON_EXECUTABLE = "python" if sys.platform == "win32" else "python3"
FFMPEG_VERSION = "5.0.1"

GODOT_VERSION = "4.1.1"
GODOT_RELEASE_TYPE = "stable"
if sys.platform == "win32":
    SCREENPLAYWALLPAPER_GODOT_EXECUTABLE = "ScreenPlayWallpaperGodot.exe"
    GODOT_EDITOR_EXECUTABLE = f"Godot_v{GODOT_VERSION}-{GODOT_RELEASE_TYPE}_win64.exe"
    GODOT_TEMPLATES_PATH = os.path.join(os.getenv(
        'APPDATA'), f"Godot/templates/{GODOT_VERSION}.{GODOT_RELEASE_TYPE}")
elif sys.platform == "darwin":
    SCREENPLAYWALLPAPER_GODOT_EXECUTABLE = "ScreenPlayWallpaperGodot.app"
    GODOT_EDITOR_EXECUTABLE = f"Godot_v{GODOT_VERSION}-{GODOT_RELEASE_TYPE}_osx.universal"
    GODOT_TEMPLATES_PATH = "TODO"
elif sys.platform == "linux":
    SCREENPLAYWALLPAPER_GODOT_EXECUTABLE = "ScreenPlayWallpaperGodot"
    GODOT_EDITOR_EXECUTABLE = f"Godot_v{GODOT_VERSION}-{GODOT_RELEASE_TYPE}_x11.64"
    # /home/eli/.local/share/godot/templates/
    GODOT_TEMPLATES_PATH = os.path.join(
        Path.home(), f".local/share/godot/templates/{GODOT_VERSION}.{GODOT_RELEASE_TYPE}")
