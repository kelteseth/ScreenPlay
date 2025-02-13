#!/usr/bin/python3
# SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
import platform
import os
import platform
import shutil
import argparse
import time
import zipfile
import defines
import re
import subprocess
from build_result import BuildResult
from build_config import BuildConfig
from typing import Tuple
from pathlib import Path
import macos_sign
import build_godot
from util import sha256, cd_repo_root_path, repo_root_path, zipdir, run, get_vs_env_dict, get_latest_git_tag, parse_semver, semver_to_string, check_universal_binary
from sys import stdout

stdout.reconfigure(encoding='utf-8')


def get_preset_info(preset_name, cwd):
    result = subprocess.run(['cmake', '--preset', preset_name, '-N'], 
                          capture_output=True, text=True, cwd=cwd)
    
    if result.returncode != 0:
        raise RuntimeError(f"Failed to get preset info: {result.stderr}")
    
    output = result.stdout
    
    # Parse different sections
    cmake_vars = {}
    env_vars = {}
    
    #print("############ Extract CMAKE variables:\n")
    # Extract CMAKE variables
    cmake_section = re.search(r'Preset CMake variables:\s*\n(.*?)(?:\n\s*\n|\Z)', 
                            output, re.DOTALL)
    if cmake_section:
        for line in cmake_section.group(1).strip().split('\n'):
            if '=' in line:
                key, value = line.split('=', 1)
                cmake_vars[key.strip()] = value.strip().strip('"')
                print(key,value)

    #print("############ Extract environment variables:\n")
    # Extract environment variables
    env_section = re.search(r'Preset environment variables:\s*\n(.*?)(?:\n\s*\n|\Z)', 
                          output, re.DOTALL)
    if env_section:
        for line in env_section.group(1).strip().split('\n'):
            if '=' in line:
                key, value = line.split('=', 1)
                env_vars[key.strip()] = value.strip().strip('"')
                #print(key,value)
    
    return {
        'cmake_variables': cmake_vars,
        'environment_variables': env_vars
    }

def clean_build_dir(build_config: BuildConfig):
    build_path = Path(build_config.root_path).joinpath("Build") 
    if build_path.exists():
        print(f"Remove previous build folder: {build_path}")
        shutil.rmtree(str(build_path), ignore_errors=True)


def execute(
    build_config: BuildConfig
) -> BuildResult:
    start_time = time.time()
    # Make sure the script is always started from the same folder
    build_config.root_path = cd_repo_root_path()


    # Sets all platform spesific paths, arguments etc.
    [build_config, build_result] = setup(build_config)

    # build_result.build = Path(build_config.build_folder)
    # build_result.bin = Path(build_config.bin_dir)

    # Make sure to always delete everything first.
    # 3rd party tools like the crashreporter create local
    # temporary files in the build directory.
    clean_build_dir(build_config)

    # Runs cmake configure and cmake build
    step_time = time.time()
    build_result = build(build_config, build_result)
    build_duration = time.time() - step_time
    print(f"⏱️ build_duration: {build_duration}s")
    if platform.system() == "Windows":
        copy_vcpkg_libraries(build_config)
    return
    
    # Build Godot Wallpaper
    # Note: This must happen after building ScreenPlay!
    # if platform.system() == "Windows":
    #     build_godot.build_godot(str(build_config.build_folder), build_config.build_type)

    if platform.system() == "Darwin":
        if (build_config.sign_osx):
            print(f"Sign binary at: {build_config.bin_dir}")
            macos_sign.sign(build_config=build_config)

    # Creates a Qt InstallerFrameWork (IFW) installer
    # if build_config.create_installer == "ON":
    #     step_time = time.time()
    #     build_installer(build_config, build_result)
    #     build_installer_duration = time.time() - step_time
    #     print(f"⏱️ build_installer_duration: {build_installer_duration}s")

    #     if platform.system() == "Darwin":
    #         # TODO FIX installer signing
    #         return
    #         if (build_config.sign_osx):
    #             # Base directory
    #             base_dir = Path(build_config.build_folder)

    #             # Paths for the original and new filenames
    #             original_file = base_dir / 'ScreenPlay-Installer-ScreenPlayComponent.dmg'
    #             new_file = base_dir / 'ScreenPlay-Installer.dmg'

    #             # Renaming the file
    #             try:
    #                 original_file.rename(new_file)
    #                 print(f"File renamed successfully to {new_file}")
    #             except OSError as error:
    #                 print(f"Error: {error}")

    #             print(
    #                 f"Sign ScreenPlay-installer.dmg at: {new_file}")
    #             macos_sign.sign_dmg(build_config)

    # Create a zip file of the build
    if platform.system() != "Darwin":
        step_time = time.time()
        build_result = zip(build_config, build_result)
        zip_duration = time.time() - step_time
        print(f"⏱️ zip_duration: {zip_duration}s")

    duration = time.time() - start_time
    print(f"⏱️ Build completed in: {duration}s")

    # Print BuildConfig & BuildResult member for easier debugging
    print("\n🆗 BuildResult:")
    print(' '.join("\n- %s: \t\t%s" %
          item for item in vars(build_result).items()))
    print("\n⚙️ BuildConfig:")
    print(' '.join("\n- %s: \t\t%s" %
          item for item in vars(build_config).items()))

    return build_result


def setup(build_config: BuildConfig) -> Tuple[BuildConfig, BuildResult]:

    # Get preset info
    preset_info = get_preset_info(build_config.preset_name, build_config.root_path)
    cmake_vars = preset_info['cmake_variables']
    env_vars = preset_info['environment_variables']
    
    # FILL BUILD CONFIG from preset info
    # CMake ON/OFF variables
    build_config.create_installer = cmake_vars.get('SCREENPLAY_CREATE_INSTALLER', 'OFF')
    build_config.build_steam = cmake_vars.get('SCREENPLAY_BUILD_STEAM', 'OFF')
    build_config.build_tests = cmake_vars.get('SCREENPLAY_BUILD_TESTS', 'OFF')
    build_config.build_deploy = cmake_vars.get('SCREENPLAY_BUILD_DEPLOY', 'OFF')
    build_config.build_godot = cmake_vars.get('SCREENPLAY_BUILD_GODOT', 'OFF')
    build_config.package = cmake_vars.get('SCREENPLAY_PACKAGE', 'OFF') == 'ON'
    build_config.osx_bundle = cmake_vars.get('SCREENPLAY_OSX_BUNDLE', 'OFF')

    # vcpkg
    build_config.build_architecture = cmake_vars.get('VCPKG_OSX_ARCHITECTURES', '')
    build_config.cmake_target_triplet = cmake_vars.get('VCPKG_TARGET_TRIPLET', '')

    # Build configuration, toolchain and paths
    build_config.build_type = cmake_vars.get('CMAKE_BUILD_TYPE', 'Release')
    build_config.build_folder = cmake_vars.get('CMAKE_BINARY_DIR', None)
    build_config.install_folder = cmake_vars.get('CMAKE_INSTALL_PREFIX', None)
    build_config.cmake_toolchain_file = cmake_vars.get('CMAKE_TOOLCHAIN_FILE', '')
    
    build_config.ifw_root_path = defines.QT_TOOLS_PATH
    build_config.qt_path = defines.QT_PATH
    build_config.qt_version = defines.QT_VERSION
    build_config.qt_ifw_version = defines.QT_IFW_VERSION
    build_config.qt_bin_path = defines.QT_BIN_PATH
    build_config.bin_dir = build_config.build_folder # TODO
    
    print(build_config)

    if platform.system() == "Windows":
        build_config.executable_file_ending = ".exe"
        env_dict = get_vs_env_dict()
        env_dict["PATH"] = env_dict["PATH"] + \
            ";" + str(Path(build_config.qt_bin_path).joinpath("bin").resolve()) + \
            ";" + str(build_config.ifw_root_path.resolve())
        os.environ.update(env_dict)


    elif platform.system() == "Linux":
        build_config.executable_file_ending = ""
    else:
        raise NotImplementedError(
            "Unsupported platform, ScreenPlay only supports Windows, macOS and Linux.")


    build_result = BuildResult()
    if platform.system() == "Windows":
        build_result.installer = Path(build_config.build_folder).joinpath(
            "ScreenPlay-Installer.exe")
    elif platform.system() == "Darwin":
        build_result.installer = Path(
            build_config.build_folder).joinpath("ScreenPlay.dmg")
    elif platform.system() == "Linux":
        build_result.installer = Path(build_config.build_folder).joinpath(
            "ScreenPlay-Installer.run")

    return build_config, build_result


def build(build_config: BuildConfig, build_result: BuildResult) -> BuildResult:
    # First configure with configure preset
    cmake_configure = f'cmake --preset={build_config.preset_name}'
    print(f"cmake configure: {cmake_configure} at {build_config.root_path}")
    run(cmake_configure, cwd=build_config.root_path)

    # Then build using build preset with same name
    cmake_build = f'cmake --build --preset={build_config.preset_name}'
    print(f"cmake build: {cmake_build} at {build_config.root_path}")
    run(cmake_build, cwd=build_config.root_path)

    build_result.bin = Path(build_config.bin_dir)
    return build_result


def copy_vcpkg_libraries(build_config: BuildConfig):

    # Some dlls like openssl do no longer get copied automatically.
    # Lets just copy all of them into bin.
    if platform.system() == "Windows":
        vcpkg_bin_path = Path(
            f"{build_config.root_path}/../vcpkg/installed/x64-windows/bin").resolve()
        for file in vcpkg_bin_path.iterdir():
            if file.suffix == ".dll" and file.is_file():
                print("Copy: ", file, build_config.install_folder)
                shutil.copy2(file, build_config.install_folder + "/bin")

# TODO probaply not needed anymore
# def cleanup_macos(build_config: BuildConfig):
#     if not platform.system() == "Darwin":
#         file_endings = [".ninja_deps", ".ninja", ".ninja_log", ".lib", ".a", ".exp",
#                         ".manifest", ".cmake", ".cbp", "CMakeCache.txt"]
#         for file_ending in file_endings:
#             for file in build_config.bin_dir.rglob("*" + file_ending):
#                 if file.is_file():
#                     print("Remove: %s" % file.resolve())
#                     file.unlink()


def build_installer(build_config: BuildConfig, build_result: BuildResult):
    os.chdir(build_result.build)
    print("Running cpack at: ", os.getcwd())
    run("cpack", cwd=build_config.build_folder)


def zip(build_config: BuildConfig, build_result: BuildResult) -> BuildResult:
    zipName = f"ScreenPlay-{build_config.screenplay_version}-{build_config.cmake_target_triplet}-{build_config.build_type}.zip"
    build_result.build_zip = Path(build_result.build).joinpath(zipName)
    print(f"Creating bin folder zip file: {build_result.build_zip}")
    os.chdir(build_config.build_folder)
    with zipfile.ZipFile(zipName, 'w', zipfile.ZIP_DEFLATED) as zipf:
        zipdir(build_config.bin_dir, zipf)

    zip_file_path = os.path.join(build_result.build, zipName)
    build_hash = sha256(zip_file_path)
    build_result.build_hash = Path(
        build_result.build).joinpath(zipName + ".sha256.txt")
    print(f"Create sha256 hash: {build_result.build_hash}")
    f = open(build_result.build_hash, "a")
    f.write(build_hash)
    f.close()

    # Some weird company firewalls do not allow direct .exe downloads
    # lets just zip the installer lol
    if build_config.create_installer == "ON":
        build_result.installer_zip = Path(build_result.build).joinpath(
            build_result.installer.stem + ".zip")
        print(f"Create zip from installer: {build_result.installer_zip}")
        zipfile.ZipFile(build_result.installer_zip, 'w').write(
            build_result.installer, build_result.installer.name)

    return build_result


if __name__ == "__main__":

    parser = argparse.ArgumentParser(
        description='Build and Package ScreenPlay')
    parser.add_argument('--preset', action="store", dest="preset",
                        help="Sets the used CMake preset")
    parser.add_argument('--sign_osx', action="store_true", dest="sign_osx", default=False,
                        help="Signs the executable on macOS. This requires a valid Apple Developer ID set up.")
    args = parser.parse_args()

    tag = get_latest_git_tag()
    if tag:
        print(f"Latest Git tag: {tag}")
        semver = parse_semver(tag)
        if semver:
            print(f"Parsed SemVer: {semver}")
            screenplay_version = semver_to_string(semver)
        else:
            print("Failed to parse SemVer.")
            exit(-1)
    else:
        print("No git tags found.")
        exit(-1)

    build_config = BuildConfig()
    build_config.screenplay_version = screenplay_version
    build_config.preset_name = args.preset
    build_config.sign_osx = args.sign_osx


    execute(build_config)
