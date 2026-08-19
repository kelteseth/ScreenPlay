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
import json
import hashlib
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


# Marker file written into the installed Qt folder after a successful aqt run.
# It records (a) what we asked aqt for and (b) a size+BLAKE2b hash of every DLL/exe
# in the install. The next setup run compares both against the on-disk files, so a
# pinned Qt is only refetched when the request changes or the bytes no longer match
# (partial, corrupt, or tampered install) — never merely because time passed.
QT_INSTALL_MARKER = ".screenplay_qt_install.json"


def qt_module_set() -> list[str]:
    """The full aqt module list this repo installs: the shared base plus the
    per-OS extras. Sorted so it can be compared against the install marker."""
    modules = [
        "qt3d", "qtquick3d", "qtconnectivity", "qt5compat", "qtimageformats",
        "qtmultimedia", "qtshadertools", "qtwebchannel", "qtwebengine",
        "qtwebsockets", "qtwebview", "qtpositioning",
    ]
    if system() == "Windows":
        modules += ["debug_info", "qtactiveqt"]
    elif system() == "Linux":
        modules += ["qtwaylandcompositor", "debug_info"]
    return sorted(modules)


def qt_request_signature() -> dict:
    """What we asked aqt for. A change here (version bump, a new module) means the
    install is the wrong *shape* and must be refetched."""
    return {
        "version": defines.QT_VERSION,
        "platform": defines.QT_PLATFORM,
        "modules": qt_module_set(),
        "sources": sorted(defines.QT6_SOURCE_MODULES),
    }


def blake2b_file(path: Path, chunk: int = 1024 * 1024) -> str:
    """BLAKE2b digest of a file. BLAKE2b ships in hashlib (no extra dependency) and
    is fast; a 128-bit digest is ample for integrity. Swap in blake3 if that package
    is ever added and the extra speed is wanted."""
    digest = hashlib.blake2b(digest_size=16)
    with open(path, "rb") as handle:
        for block in iter(lambda: handle.read(chunk), b""):
            digest.update(block)
    return digest.hexdigest()


def qt_content_manifest(qt_path: Path) -> dict:
    """{relpath: {size, blake2b}} for every DLL/exe under the install — the runtime
    binaries (bin/, plugins/, qml/). Keyed by POSIX relative path so the marker is
    stable across machines."""
    manifest = {}
    for pattern in ("*.dll", "*.exe"):
        for binary in qt_path.rglob(pattern):
            if not binary.is_file():
                continue
            manifest[binary.relative_to(qt_path).as_posix()] = {
                "size": binary.stat().st_size,
                "blake2b": blake2b_file(binary),
            }
    return manifest


def download(aqt_path: Path, qt_platform: str):
    os = {"Windows": "windows", "Darwin": "mac", "Linux": "linux"}[system()]

    qt_packages = " ".join(qt_module_set())
    console.print(f"Downloading: {qt_packages} to {aqt_path}")
    execute(f"{defines.PYTHON_EXECUTABLE} -m aqt install-qt -O  {aqt_path} {os} desktop {defines.QT_VERSION} {qt_platform} -m {qt_packages}")

    source_modules = " ".join(defines.QT6_SOURCE_MODULES)
    console.print(f"Downloading Qt {defines.QT_VERSION} sources for debugging: {source_modules}")
    execute(f"{defines.PYTHON_EXECUTABLE} -m aqt install-src -O {aqt_path} {os} desktop {defines.QT_VERSION} --archives {source_modules}")

    tools = ["tools_ifw"]
    for tool in tools:
        execute(
            f"{defines.PYTHON_EXECUTABLE} -m aqt install-tool -O {aqt_path} {os} desktop {tool}")

    # Record what we just installed so subsequent runs can skip redownloading.
    write_qt_marker(aqt_path.joinpath(defines.QT_VERSION, defines.QT_PLATFORM))


def is_qt_installation_complete(qt_path: Path) -> bool:
    """Check whether an aqt installation is complete by verifying qmake exists.
    aqt writes no completion marker, but qmake is always present in a finished install."""
    qmake = qt_path / "bin" / ("qmake.exe" if system() == "Windows" else "qmake")
    return qmake.is_file()


def write_qt_marker(qt_path: Path):
    console.print("  [dim]Hashing installed Qt binaries for the install marker…[/]")
    marker = {"request": qt_request_signature(), "files": qt_content_manifest(qt_path)}
    try:
        (qt_path / QT_INSTALL_MARKER).write_text(
            json.dumps(marker, indent=2), encoding="utf-8")
    except OSError as error:
        console.print(f"[yellow]Could not write Qt install marker:[/] {error}")


def read_qt_marker(qt_path: Path):
    try:
        return json.loads((qt_path / QT_INSTALL_MARKER).read_text(encoding="utf-8"))
    except (OSError, ValueError):
        return None


def qt_marker_status(qt_path: Path, verify_hashes: bool = False) -> tuple[str, str]:
    """Compare the on-disk install against its marker.
    Returns one of: ('missing', ''), ('stale', reason), ('corrupt', reason),
    ('ok', ''). The routine check is presence+size (instant); pass
    verify_hashes=True (setup.py --verify-qt) to also re-hash every binary."""
    marker = read_qt_marker(qt_path)
    if marker is None:
        return ("missing", "")
    if marker.get("request") != qt_request_signature():
        return ("stale", "version or module set changed")

    stored = marker.get("files", {})
    for rel, meta in stored.items():
        binary = qt_path / rel
        if not binary.is_file():
            return ("corrupt", f"missing {rel}")
        if binary.stat().st_size != meta.get("size"):
            return ("corrupt", f"size changed for {rel}")
    if verify_hashes:
        console.print("  [dim]Verifying Qt binary hashes…[/]")
        for rel, meta in stored.items():
            if blake2b_file(qt_path / rel) != meta.get("blake2b"):
                return ("corrupt", f"hash mismatch for {rel}")
    return ("ok", "")


def setup_qt(verify_hashes: bool = False):
    aqt_path = defines.QT_PATH
    console.print(f"Setup Qt via aqt at {aqt_path}")

    if system() == "Windows":
        qt_platform = "win64_msvc2022_64"
    elif system() == "Darwin":
        qt_platform = "clang_64"
    elif system() == "Linux":
        qt_platform = "linux_gcc_64"

    # NOTE: qt_platform is aqt's download architecture id (e.g. win64_msvc2022_64);
    # aqt installs it into a differently named folder (defines.QT_PLATFORM, e.g.
    # msvc2022_64). Use the install-folder name to detect an existing install.
    qt_base_path = aqt_path.joinpath(defines.QT_VERSION).resolve()
    qt_path = qt_base_path.joinpath(defines.QT_PLATFORM).resolve()

    if not qt_path.exists():
        console.print(f"[yellow]Qt path not found, downloading…[/]")
        download(aqt_path, qt_platform)
        return
    if not is_qt_installation_complete(qt_path):
        console.print(f"[yellow]Qt installation appears incomplete (qmake missing), re-downloading…[/]")
        download(aqt_path, qt_platform)
        return

    status, reason = qt_marker_status(qt_path, verify_hashes=verify_hashes)
    if status == "ok":
        how = "binary hashes verified" if verify_hashes else "size check"
        console.print(f"[green]✔[/] Qt {defines.QT_VERSION} is up to date ({how}).")
    elif status == "missing":
        # A complete install from before markers existed: adopt it by recording its
        # current file hashes instead of redownloading a pinned version we have.
        console.print(f"[green]✔[/] Qt {defines.QT_VERSION} found; writing install marker.")
        write_qt_marker(qt_path)
    else:  # "stale" or "corrupt"
        console.print(f"[yellow]Qt install needs refresh ({reason}), re-downloading…[/]")
        download(aqt_path, qt_platform)


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
    parser.add_argument('--verify-qt', action="store_true", dest="verify_qt",
                        help="Re-hash (BLAKE2b) every installed Qt binary against the "
                             "install marker instead of the fast size+presence check")
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

    # We use our own vcpkg fork (kelteseth/screenplay-vcpkg) so we can ship the
    # patched godot-cpp port (GODOTCPP_TARGET fix, see godot-cpp#1882) and the
    # custom 64-osx-universal triplet until the fix lands upstream. remote set-url
    # keeps an existing microsoft-origin checkout working after the URL switch.
    vcpkg_repo = "https://gitlab.com/kelteseth/screenplay-vcpkg"
    steps.append(("Clone vcpkg", lambda: (
        execute(f"git clone {vcpkg_repo} vcpkg",
                project_source_parent_path, True),
        execute(f"git remote set-url origin {vcpkg_repo}", vcpkg_path, True),
        execute("git fetch origin", vcpkg_path),
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
        steps.append(("Download Qt via aqt",
                      lambda: setup_qt(verify_hashes=args.verify_qt)))

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

