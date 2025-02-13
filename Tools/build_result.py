#!/usr/bin/python3
# SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
from pathlib import Path

class BuildResult:
    def __init__(self):
        # Windows example with absolute paths:
        # [...]/build-x64-windows-release/
        self.build: Path
        # [...]/build-x64-windows-release/bin
        self.bin: Path
        # [...]/build-x64-windows-release/ScreenPlay-Installer.exe
        self.installer: Path
        # [...]/build-x64-windows-release/ScreenPlay-Installer.zip
        self.installer_zip: Path
        # [...]/build-x64-windows-release/ScreenPlay-0.X.0-RCX-x64-windows-release.zip
        self.build_zip: Path
        # [...]/build-x64-windows-release/ScreenPlay-0.X.0-RCX-x64-windows-release.txt :sha256, needed for scoop
        self.build_hash: Path
        # x64, arm64, universal
        self.build_arch: str
