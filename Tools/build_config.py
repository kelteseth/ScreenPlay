#!/usr/bin/python3
# SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
from termcolor import colored

class BuildConfig:
    def __init__(self):
        self.preset_name = None
        self.root_path = None
        self.cmake_target_triplet = None
        self.package = False
        self.osx_bundle = None
        self.package_command = None
        self.executable_file_ending = None
        self.qt_path = None
        self.qt_bin_path = None
        self.qt_version = None
        self.qt_ifw_version = None
        self.ifw_root_path = None
        self.cmake_toolchain_file = None
        self.aqt_install_qt_packages = None
        self.aqt_install_tool_packages = None
        self.build_folder = None
        self.install_folder = None
        self.bin_dir = None
        self.screenplay_version = None
        self.build_steam = "OFF"
        self.build_tests = "OFF"
        self.build_deploy = "OFF"
        self.build_type = "Release"
        self.build_godot = "OFF"
        self.create_installer = "OFF"
        self.sign_osx = False

    def __str__(self) -> str:
        sections = {
            "🔧 Build Configuration": {
                "Preset": self.preset_name,
                "Type": self.build_type,
                "Target Triplet": self.cmake_target_triplet,
                "Version": self.screenplay_version,
                "Toolchain": self.cmake_toolchain_file
            },
            "🎯 Build Targets": {
                "Steam": self.build_steam,
                "Tests": self.build_tests,
                "Deploy": self.build_deploy,
                "Godot": self.build_godot,
                "Installer": self.create_installer,
                "Package": self.package,
                "OSX Bundle": self.osx_bundle,
                "Sign OSX": self.sign_osx
            },
            "📁 Paths": {
                "Root": self.root_path,
                "Build": self.build_folder,
                "Install" : self.install_folder,
                "Binary": self.bin_dir
            },
            "🔷 Qt Configuration": {
                "Qt Path": self.qt_path,
                "Qt Binary": self.qt_bin_path,
                "Qt Version": self.qt_version,
                "IFW Version": self.qt_ifw_version,
                "IFW Root": self.ifw_root_path
            },
            "📦 AQT Settings": {
                "Qt Packages": self.aqt_install_qt_packages,
                "Tool Packages": self.aqt_install_tool_packages
            }
        }

        output = ["\n📋 Build Configuration:\n"]
        
        for section_name, items in sections.items():
            output.append(colored(f"\n{section_name}:", "cyan", attrs=["bold"]))
            for key, value in items.items():
                # Color ON/OFF values
                if isinstance(value, str) and value.upper() in ["ON", "OFF"]:
                    value_str = colored("ON", "green", attrs=["bold"]) if value.upper() == "ON" else colored("OFF", "red")
                # Color boolean values
                elif isinstance(value, bool):
                    value_str = colored("True", "green", attrs=["bold"]) if value else colored("False", "red")
                # Handle None values
                elif value is None:
                    value_str = colored("Not Set", "yellow")
                # Handle empty strings
                elif isinstance(value, str) and not value.strip():
                    value_str = colored("Not Set", "yellow")
                else:
                    value_str = str(value)
                
                output.append(f"  {colored(key.ljust(15), 'white')}: {value_str}")

        return "\n".join(output)