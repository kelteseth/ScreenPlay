#!/usr/bin/python3
# SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
import os
import sys
import shutil
import util
from pathlib import Path
import defines
from rich_console import console
from download_util import stream_download


def download_godot(exe_zip_filename: str, export_templates: str,
                   download_destination_path: str,
                   progress=None, outer_task=None) -> bool:
    subfolder = f"{defines.GODOT_VERSION}-{defines.GODOT_RELEASE_TYPE}"
    base_url = f"{defines.GODOT_DOWNLOAD_SERVER}/{subfolder}"

    editor_url = f"{base_url}/{exe_zip_filename}"
    templates_url = f"{base_url}/{export_templates}"
    exe_dest = os.path.join(download_destination_path, exe_zip_filename)
    tpl_dest = os.path.join(download_destination_path, export_templates)

    stream_download(editor_url, exe_dest, exe_zip_filename, progress, outer_task)
    stream_download(templates_url, tpl_dest, export_templates, progress, outer_task)
    return True


def unzip_godot(exe_zip_filepath: str, export_templates_filepath: str, destination_path: str) -> bool:
    console.print("[bold cyan]Extracting[/] Godot editor")
    util.unzip(exe_zip_filepath, destination_path)

    godot_templates_dir = ""
    if sys.platform == "win32":
        godot_templates_dir = os.path.join(
            os.getenv('APPDATA'), "Godot/export_templates")
    elif sys.platform == "linux":
        godot_templates_dir = os.path.join(
            str(Path.home()), ".local/share/godot/export_templates")
    os.makedirs(godot_templates_dir, exist_ok=True)
    export_templates_destination_version = f"{godot_templates_dir}/{defines.GODOT_VERSION}.{defines.GODOT_RELEASE_TYPE}"

    if os.path.exists(export_templates_destination_version):
        console.print(f"  [dim]Removing previous export templates: {export_templates_destination_version}[/]")
        shutil.rmtree(export_templates_destination_version)

    console.print("[bold cyan]Extracting[/] Godot export templates")
    util.unzip(export_templates_filepath, godot_templates_dir)
    os.rename(os.path.join(godot_templates_dir, "templates"),
              export_templates_destination_version)

    for path in (exe_zip_filepath, export_templates_filepath):
        try:
            os.remove(path)
            console.print(f"  [dim]Removed {path}[/]")
        except OSError as error:
            console.print(f"[red]Error deleting file:[/] {error}")
            return False

    return True


def setup_godot(progress=None, outer_task=None) -> bool:
    console.print(f"Setting up Godot {defines.GODOT_VERSION} {defines.GODOT_RELEASE_TYPE}")
    destination_path = os.path.join(defines.THIRDPATH_PATH, "Godot")
    export_templates = f"Godot_v{defines.GODOT_VERSION}-{defines.GODOT_RELEASE_TYPE}_export_templates.tpz"
    export_templates_filepath = os.path.join(destination_path, export_templates)
    exe_zip_filepath = os.path.join(destination_path, defines.GODOT_EDITOR_DOWNLOAD_NAME)

    download_godot(defines.GODOT_EDITOR_DOWNLOAD_NAME, export_templates,
                   destination_path, progress, outer_task)
    if not unzip_godot(exe_zip_filepath, export_templates_filepath, destination_path):
        return False

    if sys.platform == "linux":
        command = f"chmod +x {defines.GODOT_EDITOR_EXECUTABLE}"
        console.print(f"  [dim]Make executable: {command}[/]")
        util.run(command, destination_path)

    return True


def execute(progress=None, outer_task=None) -> bool:
    repo_path = Path(util.repo_root_path())
    godot_path = repo_path / "ThirdParty" / "Godot"
    godot_path.mkdir(parents=True, exist_ok=True)

    for file in godot_path.iterdir():
        if defines.GODOT_EDITOR_EXECUTABLE in str(file):
            console.print(f"[green]✔[/] Godot [bold]{defines.GODOT_EDITOR_EXECUTABLE}[/] already exists.")
            return True

    return setup_godot(progress, outer_task)


if __name__ == "__main__":
    execute()

