#!/usr/bin/python3
# SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
from pathlib import Path

class BuildResult:
    # Windows example with absolute paths:
    # [...]/build-x64-windows-release/
    build: Path
    # [...]/build-x64-windows-release/bin
    bin: Path
    # [...]/build-x64-windows-release/ScreenPlay-Installer.exe
    installer: Path
    # [...]/build-x64-windows-release/ScreenPlay-Installer.zip
    installer_zip: Path
    # [...]/build-x64-windows-release/ScreenPlay-0.X.0-RCX-x64-windows-release.zip
    build_zip: Path
    # [...]/build-x64-windows-release/ScreenPlay-0.X.0-RCX-x64-windows-release.txt :sha256, needed for scoop
    build_hash: Path
    # x64, arm64, universal
    build_arch: str
