#!/usr/bin/python3
# SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
from platform import system
from pathlib import Path
from execute_util import execute
import download_ffmpeg
import defines
import argparse
import util
import macos_make_universal
import datetime
import setup_godot
from sys import stdout

from rich.panel import Panel
from rich_console import console

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
            if isinstance(command["command"], str):
                execute(command["command"], command["cwd"], command["ignore_error"],
                        command["use_shell"], command["print_command"])
            else:
                command["command"]()


def download(aqt_path: Path, qt_platform: Path):
    extra_qt_packages = ""
    if system() == "Windows":
        os = "windows"
        extra_qt_packages = "debug_info qtactiveqt"
    elif system() == "Darwin":
        os = "mac"
    elif system() == "Linux":
        os = "linux"
        extra_qt_packages = "qtwaylandcompositor debug_info "

    qt_packages = "qt3d qtquick3d qtconnectivity qt5compat qtimageformats qtmultimedia qtshadertools qtwebchannel qtwebengine qtwebsockets qtwebview qtpositioning "
    qt_packages += extra_qt_packages

    console.print(f"Downloading: {qt_packages} to {aqt_path}")
    execute(f"{defines.PYTHON_EXECUTABLE} -m aqt install-qt -O  {aqt_path} {os} desktop {defines.QT_VERSION} {qt_platform} -m {qt_packages}")

    source_modules = " ".join(defines.QT6_SOURCE_MODULES)
    console.print(f"Downloading Qt {defines.QT_VERSION} sources for debugging: {source_modules}")
    execute(f"{defines.PYTHON_EXECUTABLE} -m aqt install-src -O {aqt_path} {os} desktop {defines.QT_VERSION} --archives {source_modules}")

    tools = ["tools_ifw"]
    for tool in tools:
        execute(
            f"{defines.PYTHON_EXECUTABLE} -m aqt install-tool -O {aqt_path} {os} desktop {tool}")


def is_qt_installation_complete(qt_path: Path) -> bool:
    """Check whether an aqt installation is complete by verifying qmake exists.
    aqt writes no completion marker, but qmake is always present in a finished install."""
    qmake = qt_path / "bin" / ("qmake.exe" if system() == "Windows" else "qmake")
    return qmake.is_file()


def setup_qt():
    aqt_path = defines.QT_PATH
    console.print(f"Setup Qt via aqt at {aqt_path}")

    if system() == "Windows":
        qt_platform = "win64_msvc2022_64"
    elif system() == "Darwin":
        qt_platform = "clang_64"
    elif system() == "Linux":
        qt_platform = "linux_gcc_64"

    qt_base_path = aqt_path.joinpath(defines.QT_VERSION).resolve()
    qt_path = qt_base_path.joinpath(qt_platform).resolve()

    if not qt_path.exists():
        console.print(f"[yellow]Qt path not found, downloading…[/]")
        download(aqt_path, qt_platform)
    elif not is_qt_installation_complete(qt_path):
        console.print(f"[yellow]Qt installation appears incomplete (qmake missing), re-downloading…[/]")
        download(aqt_path, qt_platform)
    else:
        days = 30
        folder_creation_date: datetime = datetime.datetime.fromtimestamp(
            qt_base_path.stat().st_mtime, tz=datetime.timezone.utc)
        now: datetime = datetime.datetime.now(tz=datetime.timezone.utc)
        cutoff: datetime = now - datetime.timedelta(days=days)
        if folder_creation_date < cutoff:
            console.print(
                f"Qt version at `{qt_base_path}` older than {days} days ({folder_creation_date}), redownloading!")
            download(aqt_path, qt_platform)
        else:
            console.print(f"[green]✔[/] Qt {defines.QT_VERSION} is up to date.")


def run_step(step_num: int, total_steps: int, label: str, fn):
    """Print a numbered rule then run fn()."""
    console.rule(f"[dim]{step_num}/{total_steps}[/]  [bold]{label}[/]")
    result = fn()
    if isinstance(result, int) and result != 0:
        raise RuntimeError(f"Step '{label}' failed with exit code {result}")


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

    if system() == "Windows":
        vcpkg_command = "vcpkg.exe"
        vcpkg_packages_list.append("infoware[d3d]")
        vcpkg_packages_list.append("sentry-native[transport]")
        platform_command = commands_list()
        platform_command.add("bootstrap-vcpkg.bat", vcpkg_path, False)
        vcpkg_triplet = ["x64-windows"]
    elif system() == "Darwin":
        vcpkg_command = "./vcpkg"
        vcpkg_packages_list.append("curl")
        platform_command = commands_list()
        platform_command.add("chmod +x bootstrap-vcpkg.sh", vcpkg_path)
        platform_command.add("./bootstrap-vcpkg.sh", vcpkg_path, False)
        platform_command.add("chmod +x vcpkg", vcpkg_path)
        vcpkg_triplet = ["x64-osx", "arm64-osx"]
    elif system() == "Linux":
        vcpkg_command = "./vcpkg"
        platform_command = commands_list()
        platform_command.add("chmod +x bootstrap-vcpkg.sh", vcpkg_path)
        platform_command.add("./bootstrap-vcpkg.sh", vcpkg_path, False)
        platform_command.add("chmod +x vcpkg", vcpkg_path)
        vcpkg_triplet = ["x64-linux"]
    else:
        raise NotImplementedError("Unknown system: {}".format(system()))

    # --- Build step list -------------------------------------------------------
    steps: list[tuple[str, callable]] = []

    def _step_godot():
        if not setup_godot.execute():
            raise RuntimeError("Unable to download godot")

    def _step_ffmpeg():
        if not download_ffmpeg.execute():
            raise RuntimeError("Unable to download ffmpeg")

    if system() != "Darwin":
        steps.append(("Download Godot", _step_godot))

    steps.append(("Download FFmpeg", _step_ffmpeg))

    steps.append(("Clone vcpkg", lambda: (
        execute("git clone https://github.com/microsoft/vcpkg vcpkg",
                project_source_parent_path, True),
        execute("git fetch", vcpkg_path),
        execute(f"git checkout {defines.VCPKG_VERSION}", vcpkg_path),
    )))

    steps.append(("Bootstrap vcpkg", lambda: platform_command.execute_commands()))

    steps.append(("vcpkg: remove outdated packages",
                  lambda: execute(f"{vcpkg_command} remove --outdated --recurse", vcpkg_path, False)))

    for triplet in vcpkg_triplet:
        vcpkg_packages = " ".join(vcpkg_packages_list)
        # capture triplet in default arg to avoid late-binding closure
        steps.append((
            f"vcpkg: install packages [{triplet}]",
            lambda t=triplet, p=vcpkg_packages: execute(
                f"{vcpkg_command} install {p} --triplet {t} --recurse",
                vcpkg_path, False
            )
        ))

    if system() == "Darwin":
        steps.append(("macOS: make universal binaries",
                      lambda: macos_make_universal.execute()))

    if not args.skip_aqt:
        steps.append(("Download Qt via aqt", setup_qt))

    # --- Run all steps -------------------------------------------------------
    total_steps = len(steps)

    console.print(Panel(
        f"[bold]ScreenPlay Setup[/]  •  [dim]{system()}[/]  •  [dim]{total_steps} steps[/]",
        style="blue",
    ))

    for i, (label, fn) in enumerate(steps):
        run_step(i + 1, total_steps, label, fn)

    console.print(Panel("[bold green]\u2714  Setup complete![/]", style="green"))


if __name__ == "__main__":
    main()

