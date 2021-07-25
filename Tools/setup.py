import os
import sys
import argparse
from execute_util import execute

if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description='Installs ScreenPlay dependencies.')
    parser.add_argument(
        '--path', help='You can manually set the vcpkg path via -p. This path must be an absolute path and without the ScreenPlay-vcpkg name! py.exe .\setup.py --path "D:/Backup/Code/Qt/"   ')
    args = parser.parse_args()

    # ScreenPlay source and ScreenPlay-vcpkg have to be on the same file system hierarchy
    project_source_parent_path = ""

    project_source_path = os.path.abspath(os.path.join(os.getcwd(), "../"))
    if args.path is not None:
        project_source_parent_path = os.path.abspath(args.path)
    else:
        print("No --path provided!")
        project_source_parent_path = os.path.abspath(
            os.path.join(os.getcwd(), "../../"))

    print("\nproject_source_parent_path: ", project_source_parent_path,
          "\nproject_source_path: ", project_source_path,
          "\n")

    execute("git clone https://github.com/microsoft/vcpkg.git ScreenPlay-vcpkg",
            project_source_parent_path, True)

    vcpkg_path = os.path.join(project_source_parent_path, "ScreenPlay-vcpkg")
    print("vcpkg_path: ", vcpkg_path)
    vcpkg_version = "9172179c513aa84308e48b8dd0e3df90acec7204"  # Master 25.06.2021
    print("Build vcpkg ", vcpkg_version)
    execute("git fetch", vcpkg_path)
    execute("git checkout {}".format(vcpkg_version), vcpkg_path)

    vcpkg_packages_list = [
        "openssl-unix",
        "curl",
        "sentry-native",
        "doctest",
    ]

    vcpkg_triplet = ""
    vcpkg_command = ""

    if sys.platform == "win32":
        vcpkg_command = "vcpkg.exe"
        vcpkg_packages_list.append("infoware[d3d]")
        execute("download_ffmpeg.bat", project_source_path + "/Tools", False)
        execute("bootstrap-vcpkg.bat", vcpkg_path, False)
        vcpkg_triplet = "x64-windows"
    elif sys.platform == "darwin":
        vcpkg_command = "./vcpkg"
        vcpkg_packages_list.append("infoware[opencl]")
        vcpkg_packages_list.append("curl") # Hidden dependency from sentry
        execute("chmod +x bootstrap-vcpkg.sh", vcpkg_path)
        execute("./bootstrap-vcpkg.sh", vcpkg_path, False)
        execute("chmod +x vcpkg", vcpkg_path)
        vcpkg_triplet = "x64-osx"
    elif sys.platform == "linux":
        vcpkg_command = "./vcpkg"
        #vcpkg_packages_list.append("infoware[opengl]")
        execute("chmod +x bootstrap-vcpkg.sh", vcpkg_path)
        execute("./bootstrap-vcpkg.sh", vcpkg_path, False)
        execute("chmod +x vcpkg", vcpkg_path)
        vcpkg_triplet = "x64-linux"

    vcpkg_packages = " ".join(vcpkg_packages_list)
    execute("{} update".format(vcpkg_command), vcpkg_path, False)
    execute("{} upgrade --no-dry-run".format(vcpkg_command),
            vcpkg_path, False)
    execute("{} install {} --triplet {} --recurse".format(vcpkg_command,
            vcpkg_packages, vcpkg_triplet), vcpkg_path, False)
