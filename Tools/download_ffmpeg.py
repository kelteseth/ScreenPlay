import zipfile
from urllib.request import urlopen
import os


def download_and_extract(file_base_path, name):
    print("Download: ", name)

    download_server_base_url = 'https://evermeet.cx/ffmpeg/'
    filedata = urlopen(download_server_base_url + name)
    datatowrite = filedata.read()
    path_and_filename = os.path.join(file_base_path, name)

    print("Save tmp file: ", path_and_filename)
    with open(path_and_filename, 'wb') as f:
        f.write(datatowrite)

    extraction_path = os.path.join(file_base_path, "../Common/ffmpeg")

    print("Extract to:", extraction_path)
    with zipfile.ZipFile(path_and_filename,"r") as zip_ref:
        zip_ref.extractall(extraction_path)

    print("Delete tmp file: ", path_and_filename)
    os.remove(path_and_filename) 

def download_prebuild_ffmpeg():
    ffmpeg_7zip_name  = 'ffmpeg-4.4.zip'
    ffprobe_7zip_name = 'ffprobe-4.4.zip'
    current_path = os.path.join(os.path.abspath(os.getcwd()),"")
    extraction_path = os.path.abspath(os.path.join(current_path, "../Common/ffmpeg"))

    if not os.path.exists(extraction_path):
        os.makedirs(extraction_path)

    download_and_extract(current_path, ffmpeg_7zip_name)
    download_and_extract(current_path, ffprobe_7zip_name)


if __name__ == "__main__":
    download_prebuild_ffmpeg()

