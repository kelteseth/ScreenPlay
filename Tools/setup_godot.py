import requests
import zipfile
import platform
from pathlib import Path
# Assuming defines.py exists and has GODOT_VERSION
from defines import GODOT_VERSION
from util import repo_root_path  # Assuming util.py exists and has repo_root_path()


def execute():
    # Assuming repo_root_path() returns the git repo root path
    root_path = Path(repo_root_path())
    godot_path = root_path / "Tools" / "Apps" / "Godot"

    # Create the directory if it doesn't exist
    godot_path.mkdir(parents=True, exist_ok=True)

    # Check if Godot executable already exists
    for file in godot_path.iterdir():
        if GODOT_VERSION in str(file):
            print(f"Godot v{GODOT_VERSION} already exists.")
            return

    # Determine OS type
    os_type = platform.system().lower()
    if os_type == "windows":
        os_type = "win64.exe"
    elif os_type == "linux":
        os_type = "linux.x86_64"
    elif os_type == "darwin":  # macOS
        os_type = "macos.universal"
    else:
        print("Unsupported OS")
        return

    # Download Godot
    base_url = "https://github.com/godotengine/godot/releases/download"
    file_name = f"Godot_v{GODOT_VERSION}-stable_{os_type}.zip"
    url = f"{base_url}/{GODOT_VERSION}-stable/{file_name}"

    response = requests.get(url)
    if response.status_code == 200:
        download_path = godot_path / file_name
        download_path.write_bytes(response.content)

        # Extract ZIP file
        with zipfile.ZipFile(download_path, 'r') as zip_ref:
            zip_ref.extractall(godot_path)

        # Delete ZIP file
        download_path.unlink()
        print(f"Successfully installed Godot v{GODOT_VERSION}")
    else:
        print(f"Failed to download Godot v{GODOT_VERSION} for {os_type}")


if __name__ == "__main__":
    execute()
