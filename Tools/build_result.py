#!/usr/bin/python3
# SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
from pathlib import Path

class BuildResult:
    def __init__(self):
        # CMAKE_BINARY_DIR - where CMake builds
        # Example: C:/Code/cpp/ScreenPlay/ScreenPlay/Build/MSVC_Release
        self.build: Path
        
        # CMAKE_INSTALL_PREFIX - where CMake installs (for distribution)
        # Example: C:/Code/cpp/ScreenPlay/ScreenPlay/Build/Deploy
        self.install: Path
        
        # Binary directory within install folder
        # Example: C:/Code/cpp/ScreenPlay/ScreenPlay/Build/Deploy/bin
        self.bin: Path
        
        # Distribution artifacts (all relative to install folder)
        # Example: C:/Code/cpp/ScreenPlay/ScreenPlay/Build/Deploy/ScreenPlay-Installer.exe
        self.installer: Path
        # Example: C:/Code/cpp/ScreenPlay/ScreenPlay/Build/Deploy/ScreenPlay-Installer.zip
        self.installer_zip: Path
        # Example: C:/Code/cpp/ScreenPlay/ScreenPlay/Build/Deploy/ScreenPlay-0.X.0-RCX-x64-windows-release.zip
        self.build_zip: Path
        # Example: C:/Code/cpp/ScreenPlay/ScreenPlay/Build/Deploy/ScreenPlay-0.X.0-RCX-x64-windows-release.txt
        self.build_hash: Path
        
        # Architecture info
        self.build_arch: str