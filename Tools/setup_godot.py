#!/usr/bin/python3
# SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
import os
import sys
import shutil
import util
from pathlib import Path
import defines
import util

def download_godot(exe_zip_filename: str, export_templates: str, download_destination_path: str) -> bool:
    # https://github.com/godotengine/godot-builds/releases/download/4.2-beta6/Godot_v4.2-beta6_win64.exe.zip
    subfolder = f"{defines.GODOT_VERSION}-{defines.GODOT_RELEASE_TYPE}"
    base_url = f"{defines.GODOT_DOWNLOAD_SERVER}/{subfolder}"
    
    download_export_templates = f"{base_url}/{export_templates}"
    exe_destination_filepath = os.path.join(
        download_destination_path, exe_zip_filename)
    export_templates_destination_path = os.path.join(
        download_destination_path, export_templates)

    # Godot adds ".stable" to the folder names for full releases: "AppData/Roaming/Godot/templates/4.2.stable":
    download_editor = f"{base_url}/{exe_zip_filename}"
    print(f"⬇️  Downloading Godot editor {download_editor}")
    util.download(download_editor, exe_destination_filepath, False)
    print(f"⬇️  Downloading Godot export templates {download_export_templates}")
    util.download(download_export_templates,
                            export_templates_destination_path, False)

    return True


def unzip_godot(exe_zip_filepath: str, export_templates_filepath: str, destination_path: str) -> bool:
    print("🪛 Unzip Godot")
    util.unzip(exe_zip_filepath, destination_path)

    # The export templates contain a templates subfolder in which the content is. This is bad because it clashes
    # with the folder structure where the version comes after: AppData\Roaming\Godot\templates\3.3.4.stable
    # Rename: AppData\Roaming\Godot\templates\templates
    # to    : AppData\Roaming\Godot\templates\3.4.stable
    godot_templates_dir = ""
    if sys.platform == "win32":
        godot_templates_dir = os.path.join(
            os.getenv('APPDATA'), "Godot/templates")
    elif sys.platform == "linux":
        godot_templates_dir = os.path.join(
            str(Path.home()), ".local/share/godot/templates")
    os.makedirs(godot_templates_dir, exist_ok=True)
    export_templates_destination_version = f"{godot_templates_dir}/{defines.GODOT_VERSION}.{defines.GODOT_RELEASE_TYPE}"

    # Remove previous folder
    if os.path.exists(export_templates_destination_version):
        print(f"Remove previous export templates folder: {export_templates_destination_version}")
        shutil.rmtree(export_templates_destination_version)

    util.unzip(export_templates_filepath, godot_templates_dir)
    os.rename(os.path.join(godot_templates_dir, "templates"),
              export_templates_destination_version)

    print(f"🚮 Remove {exe_zip_filepath}")
    try:
        os.remove(exe_zip_filepath)
    except OSError as error:
        print(f"Error deleting file: {error}")
        return False
    
    print(f"🚮 Remove {export_templates_filepath}")
    try:
        os.remove(export_templates_filepath)
    except OSError as error:
        print(f"Error deleting file: {error}")
        return False

    return True


def setup_godot() -> bool:
    print(f"🚀 Set up GODOT version {defines.GODOT_VERSION} {defines.GODOT_RELEASE_TYPE}")
    destination_path = os.path.join(defines.THIRDPATH_PATH, "Godot")
    export_templates = f"Godot_v{defines.GODOT_VERSION}-{defines.GODOT_RELEASE_TYPE}_export_templates.tpz"
    export_templates_filepath = os.path.join(
        destination_path, export_templates)
    exe_zip_filepath = os.path.join(destination_path, defines.GODOT_EDITOR_DOWNLOAD_NAME)
    download_godot( defines.GODOT_EDITOR_DOWNLOAD_NAME,
                   export_templates, destination_path)
    if not unzip_godot(exe_zip_filepath, export_templates_filepath, destination_path):
        return False
    
    # Linux needs to change file permission to be able to run godot
    if sys.platform == "linux":
        command = f"chmod +x {defines.GODOT_EDITOR_EXECUTABLE}"
        print(f"Make editor executable: {command} at {destination_path}")
        util.run(command,destination_path)

    return True


def execute() -> bool:
    # Assuming repo_root_path() returns the git repo root path
    repo_path = Path(util.repo_root_path())
    godot_path = repo_path / "ThirdParty" / "Godot"

    # Create the directory if it doesn't exist
    godot_path.mkdir(parents=True, exist_ok=True)

    # Check if Godot executable already exists
    for file in godot_path.iterdir():
        if defines.GODOT_EDITOR_EXECUTABLE in str(file):
            print(f"Godot v{defines.GODOT_EDITOR_EXECUTABLE} already exists.")
            return True
        
    return setup_godot()

if __name__ == "__main__":
    execute()
