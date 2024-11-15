#!/usr/bin/python3
# SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
from fileinput import filename
import sys
from zipfile import ZipFile
import platform
from urllib.request import urlopen
import subprocess
import os
import defines
from shutil import move, rmtree
from util import cd_repo_root_path
from sys import stdout
from defines import FFMPEG_VERSION_MAC, FFMPEG_VERSION_WIN

stdout.reconfigure(encoding='utf-8')

def download(
        download_server_base_url,
        extraction_path,
        name) -> str:

    url = download_server_base_url + name
    print(f"Downloading {name} from: {url}")
    filedata = urlopen(url)
    datatowrite = filedata.read()
    path_and_filename = os.path.join(extraction_path, name)

    print("Save tmp file: ", path_and_filename)
    with open(path_and_filename, 'wb') as f:
        f.write(datatowrite)

    return path_and_filename


def extract_zip(extraction_path, path_and_filename):
    print(f"Extract {path_and_filename} to {extraction_path}")

    with ZipFile(path_and_filename, "r") as zip_ref:
        zip_ref.extractall(extraction_path)

    print(f"Delete tmp file: {path_and_filename}")
    os.remove(path_and_filename)


def extract_zip_executables(extraction_path, path_and_filename):
    files = []
    listOfFileNames = []
    with ZipFile(path_and_filename, 'r') as zipObj:
        # Get a list of all archived file names from the zip
        listOfFileNames = zipObj.namelist()
        # Iterate over the file names
        for fileName in listOfFileNames:
            # Check filename endswith csv
            if fileName.endswith('.exe'):
                # Extract a single file from zip
                zipObj.extract(fileName, extraction_path)
                print(f"Extract {fileName} to {extraction_path}")
                files.append(fileName)

    for file in files:
        abs_file_path = os.path.join(extraction_path, file)
        move(abs_file_path, extraction_path)
        print(f"Move {abs_file_path} to {extraction_path}")

    print(f"Delete tmp zip file: {path_and_filename}")
    os.remove(path_and_filename)
    # We need the first folder name of a file:
    # ffmpeg-5.0.1-essentials_build/bin/ffmpeg.exe 
    empty_ffmpeg_folder = os.path.join(extraction_path,os.path.dirname(os.path.dirname(files[0])))
    ffplay = os.path.join(extraction_path,"ffplay.exe")
    print(f"Delete not needed {ffplay}")
    os.remove(ffplay)
    print(f"Delete empty folder: {empty_ffmpeg_folder}")
    rmtree(empty_ffmpeg_folder)


def download_prebuild_ffmpeg_mac(extraction_path: str):

    print("Setup ffmpeg mac")
    ffmpeg_zip_name = f'ffmpeg-{defines.FFMPEG_VERSION_MAC}.zip'
    ffprobe_zip_name = f'ffprobe-{defines.FFMPEG_VERSION_MAC}.zip'
    download_server_base_url = 'https://evermeet.cx/ffmpeg/'

    ffmpeg_path_and_filename = download(
        download_server_base_url, extraction_path, ffmpeg_zip_name)
    extract_zip(extraction_path, ffmpeg_path_and_filename)

    ffprobe_path_and_filename = download(
        download_server_base_url, extraction_path, ffprobe_zip_name)
    extract_zip(extraction_path, ffprobe_path_and_filename)


def download_prebuild_ffmpeg_windows(extraction_path: str):

    print("Setup ffmpeg Windows")
    ffmpeg_shared_7zip_name = "ffmpeg-release-essentials.zip"
    download_server_base_url = 'https://www.gyan.dev/ffmpeg/builds/'

    ffmpeg_path_and_filename = download(
        download_server_base_url, extraction_path, ffmpeg_shared_7zip_name)

    extract_zip_executables(extraction_path, ffmpeg_path_and_filename)

def execute() ->bool:
    # Make sure the script is always started from the same folder
    root_path = cd_repo_root_path()
    extraction_path = os.path.join(root_path, "ThirdParty/ffmpeg")
    ffmpeg_binary_path = os.path.join(extraction_path, "ffmpeg")  # Adjust this if FFmpeg binary is inside another subdirectory
    
    if sys.platform == "win32":
        ffmpeg_binary_path += ".exe"

    # Check if ffmpeg is already installed and matches the required version
    if os.path.isfile(ffmpeg_binary_path):
        result = subprocess.run([ffmpeg_binary_path, "-version"], capture_output=True, text=True)
        output = result.stdout
        version_line = next((line for line in output.split('\n') if 'ffmpeg version' in line), None)
        if version_line:
            installed_version = version_line.split(' ')[2].split('-')[0]
            
            if sys.platform == "win32":
                if installed_version == FFMPEG_VERSION_WIN:
                    print(f"FFmpeg version {installed_version} is already installed.")
                    return True
            elif platform.system() == "Darwin":
                if installed_version == FFMPEG_VERSION_MAC:
                    print(f"FFmpeg version {installed_version} is already installed.")
                    return True
                
            else:
                print(f"FFmpeg version {installed_version} found, but version {FFMPEG_VERSION} is required.")

    try:
        if os.path.exists(extraction_path):
            rmtree(extraction_path)
            print("Directory removed successfully.")
    except Exception as e:
        print(f"An error occurred while trying to remove the directory: {str(e)}")
        return False

    try:
        if os.path.exists(extraction_path):
            rmtree(extraction_path)
            print("Directory removed successfully.")
    except Exception as e:
        print(f"An error occurred while trying to remove the directory: {str(e)}")
        return False

        
    os.makedirs(extraction_path)
    
    if platform.system() == "Windows":
        download_prebuild_ffmpeg_windows(extraction_path)
    elif platform.system() == "Darwin":
        download_prebuild_ffmpeg_mac(extraction_path)
    
    return True


if __name__ == "__main__":
    execute()
