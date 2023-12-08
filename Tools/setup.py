#!/usr/bin/python3
# SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
from platform import system
from pathlib import Path
from execute_util import execute
import download_ffmpeg
import defines
import argparse
import util
import shutil
import macos_make_universal
import datetime
import setup_godot
from sys import stdout

stdout.reconfigure(encoding='utf-8')


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
                execute(command["command"], command["cwd"], command["ignore_error"],
                        command["use_shell"], command["print_command"])
            else:
                # Function call
                command["command"]()


def download(aqt_path: Path, qt_platform: Path):
    qt_packages =  ""
    if system() == "Windows":
        os = "windows"
    elif system() == "Darwin":
        os = "mac"
    elif system() == "Linux":
        qt_packages =  "qtwaylandcompositor "
        os = "linux"

    qt_packages += "qt3d qtquick3d qtconnectivity qt5compat qtimageformats qtmultimedia qtshadertools qtwebchannel qtwebengine qtwebsockets qtwebview qtpositioning"
    # Windows: python -m aqt list-qt windows desktop --modules 6.6.1 win64_msvc2019_64
    # Linux: python3 -m aqt list-qt linux  desktop --modules 6.6.1 gcc_64
    print(f"Downloading: {qt_packages} to {aqt_path}")
    execute(f"{defines.PYTHON_EXECUTABLE} -m aqt install-qt -O  {aqt_path} {os} desktop {defines.QT_VERSION} {qt_platform} -m {qt_packages}")

    # Tools can only be installed one at the time:
    # see:  python -m aqt list-tool windows desktop
    tools = ["tools_ifw"]
    for tool in tools:
        execute(
            f"{defines.PYTHON_EXECUTABLE} -m aqt install-tool -O {aqt_path} {os} desktop {tool}")


def setup_qt():

    aqt_path = defines.QT_PATH

    print(f"Setup Qt via aqt at {aqt_path}")

    if system() == "Windows":
        qt_platform = "win64_msvc2019_64"
    elif system() == "Darwin":
        qt_platform = "clang_64"
    elif system() == "Linux":
        qt_platform = "gcc_64"

    qt_base_path = aqt_path.joinpath(defines.QT_VERSION).resolve()
    qt_path = qt_base_path.joinpath(qt_platform).resolve()

    if not qt_path.exists():
        download(aqt_path, qt_platform)
    else:
        # Betas & RCs are technically the same version. So limit download to x days
        days = 30
        folder_creation_date: datetime = datetime.datetime.fromtimestamp(
            qt_base_path.stat().st_mtime, tz=datetime.timezone.utc)
        now: datetime = datetime.datetime.now(tz=datetime.timezone.utc)
        two_weeks_ago: datetime = now - datetime.timedelta(days=days)
        if (folder_creation_date < two_weeks_ago):
            print(
                f"qt version at `{qt_base_path}` older than {days} days ({folder_creation_date}), redownload!")
            download(aqt_path, qt_platform)
        else:
            print(f"Qt {defines.QT_VERSION} is up to date and ready ")


def main():
    parser = argparse.ArgumentParser(
        description='Build and Package ScreenPlay')
    parser.add_argument('--skip-aqt', action="store_true", dest="skip_aqt",
                        help="Downloads needed Qt binaries Windows")
    args = parser.parse_args()

    root_path = Path(util.cd_repo_root_path())
    project_source_parent_path = root_path.joinpath("../").resolve()
    vcpkg_path = project_source_parent_path.joinpath("vcpkg").resolve()
    vcpkg_packages_list = defines.VCPKG_BASE_PACKAGES

    if system() != "Darwin":
        if not setup_godot.execute():
            raise RuntimeError("Unable to download godot")

    if not download_ffmpeg.execute():
         raise RuntimeError("Unable to download ffmpeg")
    

    if system() == "Windows":
        vcpkg_command = "vcpkg.exe"
        vcpkg_packages_list.append("infoware[d3d]")
        vcpkg_packages_list.append("sentry-native[transport]")
        platform_command = commands_list()
        platform_command.add("bootstrap-vcpkg.bat", vcpkg_path, False)
        vcpkg_triplet = ["x64-windows"]
    elif system() == "Darwin":
        vcpkg_command = "./vcpkg"
        # vcpkg_packages_list.append("infoware[opencl]") does not work with arm
        vcpkg_packages_list.append("curl")  # Hidden dependency from sentry
        platform_command = commands_list()
        platform_command.add("chmod +x bootstrap-vcpkg.sh", vcpkg_path)
        platform_command.add("./bootstrap-vcpkg.sh", vcpkg_path, False)
        platform_command.add("chmod +x vcpkg", vcpkg_path)
        vcpkg_triplet = ["x64-osx","arm64-osx"]
    elif system() == "Linux":
        vcpkg_command = "./vcpkg"
        # vcpkg_packages_list.append("infoware[opengl]")
        platform_command = commands_list()
        platform_command.add("chmod +x bootstrap-vcpkg.sh", vcpkg_path)
        platform_command.add("./bootstrap-vcpkg.sh", vcpkg_path, False)
        platform_command.add("chmod +x vcpkg", vcpkg_path)
        vcpkg_triplet = ["x64-linux"]
    else:
        raise NotImplementedError("Unknown system: {}".format(system()))
    
    if vcpkg_path.exists():
        print(f"Deleting exisitng vcpkg: {vcpkg_path}")
        shutil.rmtree(str(vcpkg_path))
        
    print(f"Clone into {vcpkg_path}")
    execute("git clone --depth 1  https://github.com/microsoft/vcpkg vcpkg",
            project_source_parent_path, True)
    execute("git fetch", vcpkg_path)
    execute(f"git checkout {defines.VCPKG_VERSION}", vcpkg_path)

    # Setup vcpkg via boostrap script first
    platform_command.execute_commands()  # Execute platform specific commands.

    execute(f"{vcpkg_command} remove --outdated --recurse", vcpkg_path, False)

    for triplet in vcpkg_triplet:
        vcpkg_packages = " ".join(vcpkg_packages_list)
        execute(
            f"{vcpkg_command} install {vcpkg_packages} --triplet {triplet} --recurse", vcpkg_path, False)
        
    # Combine x64 and arm
    if system() == "Darwin":
        macos_make_universal.execute()

    if not args.skip_aqt:
        setup_qt()



if __name__ == "__main__":
    main()
