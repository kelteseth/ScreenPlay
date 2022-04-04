#!/usr/bin/python3

import argparse
import shutil
from platform import system
from pathlib import Path
from execute_util import execute
from download_ffmpeg import download_prebuild_ffmpeg

vcpkg_version = "1085a57da0725c19e19586025438e8c16f34c890"  # Master 31.12.2021

vcpkg_packages_list = [
    "openssl",
    "curl",
    "sentry-native",
    "cpp-httplib"
]

class commands_list():
    def __init__(self):
        self.commands = []

    def add(self, command, cwd=".", ignore_error=False, use_shell=True, print_command=True):
        self.commands.append({
            "command": command,
            "cwd": cwd,
            "ignore_error": ignore_error,
            "use_shell": use_shell,
            "print_command": print_command
        })

    def get_commands(self):
        return self.commands

    def execute_commands(self):
        '''
        This function execute all commands added to the list.
        '''
        for command in self.commands:
            # Check if the command if a string.
            if isinstance(command["command"], str):
                execute(command["command"], command["cwd"], command["ignore_error"], command["use_shell"], command["print_command"])
            else:
                # Function call
                command["command"]()

if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description='Install ScreenPlay dependencies.')
    parser.add_argument(
        '--path', help='Manually set the vcpkg path. The path must be an absolute path \
without the ScreenPlay-vcpkg folder (E.g. py .\setup.py --path "D:/Backup/Code/Qt/")')
    args = parser.parse_args()

    # ScreenPlay source and ScreenPlay-vcpkg have to be on the same file system hierarchy
    project_source_parent_path = ""
    project_source_path = Path.cwd().resolve()
    if project_source_path.name == "Tools":
        project_source_path = project_source_path.parent

    if args.path is not None:
        project_source_parent_path = Path(args.path).resolve()
    else:
        print("No --path provided, using default path.")
        project_source_parent_path = project_source_path.parent

    print("project_source_parent_path: {}".format(project_source_parent_path))
    print("project_source_path: {}".format(project_source_path))

    vcpkg_path = project_source_parent_path.joinpath("ScreenPlay-vcpkg")
    print("vcpkg_path: ", vcpkg_path)
    print("Build vcpkg ", vcpkg_version)

    vcpkg_triplet = ""
    vcpkg_command = ""  
    platform_command = commands_list()

    if system() == "Windows":
        vcpkg_command = "vcpkg.exe"
        vcpkg_packages_list.append("infoware[d3d]")
        if shutil.which("pwsh"):
            print("Using experimental pwsh, may not work properly.")
            platform_command.add("pwsh.exe -NoProfile -ExecutionPolicy Bypass .\scripts\\bootstrap.ps1", vcpkg_path)
        else:
            platform_command.add("bootstrap-vcpkg.bat", vcpkg_path, False)
        platform_command.add("download_ffmpeg.bat", project_source_path.joinpath("Tools"), False)
        vcpkg_triplet = "x64-windows"
    elif system() == "Darwin":
        vcpkg_command = "./vcpkg"
        vcpkg_packages_list.append("infoware[opencl]")
        vcpkg_packages_list.append("curl") # Hidden dependency from sentry
        platform_command.add("chmod +x bootstrap-vcpkg.sh", vcpkg_path)
        platform_command.add("./bootstrap-vcpkg.sh", vcpkg_path, False)
        platform_command.add("chmod +x vcpkg", vcpkg_path)
        vcpkg_triplet = "x64-osx"
        platform_command.add(download_prebuild_ffmpeg)
    elif system() == "Linux":
        vcpkg_command = "./vcpkg"
        #vcpkg_packages_list.append("infoware[opengl]")
        platform_command.add("chmod +x bootstrap-vcpkg.sh", vcpkg_path)
        platform_command.add("./bootstrap-vcpkg.sh", vcpkg_path, False)
        platform_command.add("chmod +x vcpkg", vcpkg_path)
        vcpkg_triplet = "x64-linux"
    else:
        raise NotImplementedError("Unknown system: {}".format(system()))

    execute("git clone https://github.com/microsoft/vcpkg.git ScreenPlay-vcpkg", project_source_parent_path, True)
    execute("git fetch", vcpkg_path)
    execute("git checkout {}".format(vcpkg_version), vcpkg_path)
    platform_command.execute_commands() # Execute platform specific commands.
    vcpkg_packages = " ".join(vcpkg_packages_list)
    execute("{} remove --outdated --recurse".format(vcpkg_command), vcpkg_path, False)
    execute("{} update".format(vcpkg_command), vcpkg_path, False)
    execute("{} upgrade --no-dry-run".format(vcpkg_command),
            vcpkg_path, False)
    execute("{} install {} --triplet {} --recurse".format(vcpkg_command,
            vcpkg_packages, vcpkg_triplet), vcpkg_path, False)
