#!/usr/bin/python3

from platform import system
from pathlib import Path
from execute_util import execute
import download_ffmpeg
import defines
import argparse
import util
import datetime

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

def download(aqt_path: Path, qt_platform: Path):
    
    # aqt list-qt windows desktop --modules 6.2.0 win64_msvc2019_64
    qt_packages = "qt3d qt5compat qtimageformats qtmultimedia qtshadertools qtquick3d qtwebengine qtwebsockets qtwebview qtpositioning"
    print(f"Downloading: {qt_packages} to {aqt_path}")
    execute(f"{defines.PYTHON_EXECUTABLE} -m aqt install-qt -O  {aqt_path} windows desktop {defines.QT_VERSION} {qt_platform} -m {qt_packages}")

    # Tools can only be installed one at the time:
    # see: aqt list-tool windows desktop
    tools = ["tools_ifw", "tools_qtcreator", "tools_ninja" ,"tools_cmake"]
    for tool in tools:
        execute(f"{defines.PYTHON_EXECUTABLE} -m aqt install-tool -O {aqt_path} windows desktop {tool}")

def setup_qt():

    aqt_path = defines.AQT_PATH

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
        folder_creation_date: datetime = datetime.datetime.fromtimestamp(qt_base_path.stat().st_mtime, tz=datetime.timezone.utc)
        now: datetime = datetime.datetime.now(tz=datetime.timezone.utc) 
        two_weeks_ago: datetime = now - datetime.timedelta(days=days)
        if(folder_creation_date < two_weeks_ago):
            print(f"qt version at `{qt_base_path}` older than {days} days ({folder_creation_date}), redownload!")
            download(aqt_path, qt_platform)
        else:
            print(f"Qt {defines.QT_VERSION} is up to date and ready ")

def main():
    parser = argparse.ArgumentParser(
        description='Build and Package ScreenPlay')
    parser.add_argument('-skip-aqt', action="store_true", dest="skip_aqt",
                        help="Downloads QtCreator and needed binaries \Windows: C:\aqt\nLinux & macOS:~/aqt/.")
    args = parser.parse_args()

    root_path = Path(util.cd_repo_root_path())
    project_source_parent_path = root_path.joinpath("../").resolve()
    vcpkg_path = project_source_parent_path.joinpath("ScreenPlay-vcpkg").resolve()
    vcpkg_packages_list = [
    "openssl",
    "curl",
    "cpp-httplib",
    "libarchive"
]
    if not args.skip_aqt:
        setup_qt()

    download_ffmpeg.execute()

    if system() == "Windows":
        vcpkg_command = "vcpkg.exe"
        vcpkg_packages_list.append("infoware[d3d]")
        vcpkg_packages_list.append("sentry-native")
        platform_command = commands_list()
        platform_command.add("bootstrap-vcpkg.bat", vcpkg_path, False)
        vcpkg_triplet = ["x64-windows"]
    elif system() == "Darwin":
        vcpkg_command = "./vcpkg"
        #vcpkg_packages_list.append("infoware[opencl]") does not work with arm
        vcpkg_packages_list.append("curl") # Hidden dependency from sentry
        platform_command = commands_list()
        platform_command.add("chmod +x bootstrap-vcpkg.sh", vcpkg_path)
        platform_command.add("./bootstrap-vcpkg.sh", vcpkg_path, False)
        platform_command.add("chmod +x vcpkg", vcpkg_path)
        vcpkg_triplet = ["x64-osx", "arm64-osx"]
    elif system() == "Linux":
        vcpkg_command = "./vcpkg"
        #vcpkg_packages_list.append("infoware[opengl]")
        platform_command = commands_list()
        platform_command.add("chmod +x bootstrap-vcpkg.sh", vcpkg_path)
        platform_command.add("./bootstrap-vcpkg.sh", vcpkg_path, False)
        platform_command.add("chmod +x vcpkg", vcpkg_path)
        vcpkg_triplet = ["x64-linux"]
    else:
        raise NotImplementedError("Unknown system: {}".format(system()))

    print(f"Clone into {vcpkg_path}")
    execute("git clone https://github.com/microsoft/vcpkg.git ScreenPlay-vcpkg", project_source_parent_path, True)
    execute("git fetch", vcpkg_path)
    execute("git checkout {}".format(defines.VCPKG_VERSION), vcpkg_path)
    
    # Setup vcpkg via boostrap script first
    platform_command.execute_commands() # Execute platform specific commands.
    
    execute(f"{vcpkg_command} remove --outdated --recurse", vcpkg_path, False)

    for triplet in vcpkg_triplet:
        vcpkg_packages = " ".join(vcpkg_packages_list)
        execute(f"{vcpkg_command} install {vcpkg_packages} --triplet {triplet} --recurse", vcpkg_path, False)

if __name__ == "__main__":
    main()