#!/usr/bin/python3
# SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
class BuildConfig:
    root_path: str
    cmake_target_triplet: str
    package: bool
    osx_bundle: str
    package_command: str
    executable_file_ending: str
    # qt_* use either aqt or from the maintenance tool
    qt_path: str  # C:\Qt
    qt_bin_path: str  # C:\Qt\6.8.2\msvc2022_64
    qt_version: str
    qt_ifw_version: str
    ifw_root_path: str
    cmake_toolchain_file: str
    aqt_install_qt_packages: str
    aqt_install_tool_packages: str
    executable_file_ending: str
    build_folder: str
    bin_dir: str
    screenplay_version: str
    # CMake variables need str: "ON" or "OFF"
    build_steam: str
    build_tests: str
    build_deploy: str
    build_type: str
    build_architecture: str
    build_godot: str
    create_installer: str
    sign_osx: bool
