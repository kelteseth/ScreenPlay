#!/usr/bin/python3
# SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
import sys
from zipfile import ZipFile
import platform
import subprocess
import os
import defines
from shutil import move, rmtree
from util import cd_repo_root_path
from sys import stdout
from defines import FFMPEG_VERSION_MAC, FFMPEG_VERSION_WIN
from rich_console import console
from rich.panel import Panel
from download_util import stream_download

stdout.reconfigure(encoding='utf-8')


def download(download_server_base_url, extraction_path, name,
             progress=None, outer_task=None) -> str:
    url = download_server_base_url + name
    dest = os.path.join(extraction_path, name)
    return stream_download(url, dest, name, progress, outer_task)


def extract_zip(extraction_path, path_and_filename):
    console.print(f"[bold cyan]Extracting[/] {path_and_filename}")
    with ZipFile(path_and_filename, "r") as zip_ref:
        zip_ref.extractall(extraction_path)
    os.remove(path_and_filename)
    console.print(f"  [green]Done[/] — removed tmp file")


def extract_zip_executables(extraction_path, path_and_filename):
    files = []
    with ZipFile(path_and_filename, "r") as zipObj:
        for fileName in zipObj.namelist():
            if fileName.endswith(".exe"):
                zipObj.extract(fileName, extraction_path)
                console.print(f"  [green]Extracted[/] {fileName}")
                files.append(fileName)

    for file in files:
        abs_file_path = os.path.join(extraction_path, file)
        move(abs_file_path, extraction_path)
        console.print(f"  [green]Moved[/] {os.path.basename(file)} → {extraction_path}")

    os.remove(path_and_filename)
    console.print(f"  [dim]Removed tmp zip[/]")

    empty_ffmpeg_folder = os.path.join(
        extraction_path, os.path.dirname(os.path.dirname(files[0]))
    )
    ffplay = os.path.join(extraction_path, "ffplay.exe")
    if os.path.isfile(ffplay):
        os.remove(ffplay)
        console.print(f"  [dim]Removed ffplay.exe[/]")
    rmtree(empty_ffmpeg_folder)
    console.print(f"  [dim]Removed empty folder: {empty_ffmpeg_folder}[/]")


def download_prebuild_ffmpeg_mac(extraction_path: str, progress=None, outer_task=None):
    ffmpeg_zip_name = f"ffmpeg-{defines.FFMPEG_VERSION_MAC}.zip"
    ffprobe_zip_name = f"ffprobe-{defines.FFMPEG_VERSION_MAC}.zip"
    base_url = "https://evermeet.cx/ffmpeg/"

    extract_zip(extraction_path, download(base_url, extraction_path, ffmpeg_zip_name, progress, outer_task))
    extract_zip(extraction_path, download(base_url, extraction_path, ffprobe_zip_name, progress, outer_task))


def download_prebuild_ffmpeg_windows(extraction_path: str, progress=None, outer_task=None):
    base_url = "https://www.gyan.dev/ffmpeg/builds/"
    name = "ffmpeg-release-essentials.zip"

    extract_zip_executables(extraction_path, download(base_url, extraction_path, name, progress, outer_task))


def execute(progress=None, outer_task=None) -> bool:
    root_path = cd_repo_root_path()
    extraction_path = os.path.join(root_path, "ThirdParty/ffmpeg")
    ffmpeg_binary_path = os.path.join(extraction_path, "ffmpeg")

    if sys.platform == "win32":
        ffmpeg_binary_path += ".exe"

    if os.path.isfile(ffmpeg_binary_path):
        result = subprocess.run(
            [ffmpeg_binary_path, "-version"], capture_output=True, text=True
        )
        version_line = next(
            (l for l in result.stdout.split("\n") if "ffmpeg version" in l), None
        )
        if version_line:
            installed_version = version_line.split(" ")[2].split("-")[0]
            if sys.platform == "win32" and installed_version == FFMPEG_VERSION_WIN:
                console.print(
                    f"[green]✔[/] FFmpeg [bold]{installed_version}[/] already installed."
                )
                return True
            elif platform.system() == "Darwin" and installed_version == FFMPEG_VERSION_MAC:
                console.print(
                    f"[green]✔[/] FFmpeg [bold]{installed_version}[/] already installed."
                )
                return True
            else:
                console.print(
                    f"[yellow]![/] Found FFmpeg {installed_version}, updating..."
                )

    try:
        if os.path.exists(extraction_path):
            rmtree(extraction_path)
            console.print(f"[dim]Removed existing directory: {extraction_path}[/]")
    except Exception as e:
        console.print(f"[red]Error removing directory:[/] {e}")
        return False

    os.makedirs(extraction_path)

    if platform.system() == "Windows":
        download_prebuild_ffmpeg_windows(extraction_path, progress, outer_task)
    elif platform.system() == "Darwin":
        download_prebuild_ffmpeg_mac(extraction_path, progress, outer_task)

    console.print(Panel("[bold green]FFmpeg setup complete![/]", style="green"))
    return True


if __name__ == "__main__":
    execute()
