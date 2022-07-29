#!/usr/bin/python3
import platform
import os
import subprocess
import platform
import shutil
import argparse
import time
from typing import Tuple
import zipfile
from shutil import copytree
from pathlib import Path
from datetime import datetime
from util import sha256, cd_repo_root_path, zipdir, run

# Based on https://gist.github.com/l2m2/0d3146c53c767841c6ba8c4edbeb4c2c


def get_vs_env_dict():
    vcvars: str  # We support 2019 or 2022

    # Hardcoded VS path
    # check if vcvars64.bat is available.
    msvc_2019_path = "C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Community\\VC\\Auxiliary\\Build\\vcvars64.bat"
    msvc_2022_path = "C:\\Program Files\\Microsoft Visual Studio\\2022\\Community\\VC\\Auxiliary\\Build\\vcvars64.bat"

    if Path(msvc_2019_path).exists():
        vcvars = msvc_2019_path
    # Prefer newer MSVC and override if exists
    if Path(msvc_2022_path).exists():
        vcvars = msvc_2022_path
    if not vcvars:
        raise RuntimeError(
            "No Visual Studio installation found, only 2019 and 2022 are supported.")

    print(f"\n\nLoading MSVC env variables via {vcvars}\n\n")

    cmd = [vcvars, '&&', 'set']
    popen = subprocess.Popen(
        cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    stdout, stderr = popen.communicate()

    if popen.wait() != 0:
        raise ValueError(stderr.decode("mbcs"))
    output = stdout.decode("mbcs").split("\r\n")
    return dict((e[0].upper(), e[1]) for e in [p.rstrip().split("=", 1) for p in output] if len(e) == 2)


def clean_build_dir(build_dir):
    if isinstance(build_dir, str):
        build_dir = Path(build_dir)
    if build_dir.exists():
        print(f"Remove previous build folder: {build_dir}")
        # ignore_errors removes also not empty folders...
        shutil.rmtree(build_dir, ignore_errors=True)
    build_dir.mkdir(parents=True, exist_ok=True)


class BuildResult:       # Windows example with absolute paths:
    build: Path          # [...]/build-x64-windows-release/
    binary: Path         # [...]/build-x64-windows-release/bin
    # [...]/build-x64-windows-release/ScreenPlay-Installer.exe
    installer: Path
    # [...]/build-x64-windows-release/ScreenPlay-Installer.zip
    installer_zip: Path
    # [...]/build-x64-windows-release/ScreenPlay-0.15.0-RC1-x64-windows-release.zip
    build_zip: Path
    # [...]/build-x64-windows-release/ScreenPlay-0.15.0-RC1-x64-windows-release.txt
    build_hash: Path     # sha256, needed for scoop
    build_arch: str      # x64, arm64, universal


class BuildConfig:
    root_path: str
    cmake_osx_architectures: str
    cmake_target_triplet: str
    deploy_command: str
    executable_file_ending: str
    qt_path: str
    qt_bin_path: str
    qt_version: str
    qt_ifw_version: str
    ifw_root_path: str
    cmake_toolchain_file: str
    use_aqt: bool
    aqt_install_qt_packages: str
    aqt_install_tool_packages: str
    aqt_path: str
    executable_file_ending: str
    build_folder: str
    bin_dir: str
    sign_build: bool
    screenplay_version: str
    # CMake variables need str: "ON" or "OFF"
    build_steam: str
    build_tests: str
    build_release: str
    build_type: str
    build_architecture: str
    create_installer: str


def execute(
    build_config: BuildConfig
) -> BuildResult:

    # Make sure the script is always started from the same folder
    build_config.root_path = cd_repo_root_path()

    build_result = BuildResult()

    # Sets all platform spesific paths, arguments etc.
    setup_tuple = setup(build_config, build_result)
    build_config = setup_tuple[0]
    build_result = setup_tuple[1]

    build_result.build = Path(build_config.build_folder)
    build_result.binary = Path(build_config.bin_dir)

    # Make sure to always delete everything first.
    # 3rd party tools like the crashreporter create local
    # temporary files in the build directory.
    clean_build_dir(build_config.build_folder)

    start_time = time.time()

    # Runs cmake configure and cmake build
    build_result = build(build_config, build_result)

    # Copies all needed libraries and assets into the bin folder
    deploy(build_config)

    # Creates a Qt InstallerFrameWork (IFW) installer
    if build_config.create_installer == "ON":
        build_installer(build_config, build_result)

    # Mac needs signed builds for user to run the app
    if platform.system() == "Darwin" and build_config.sign_build:
        sign(build_config)

    # Create a zip file for scoop
    build_result = zip(build_config, build_result)

    print("Time taken: {}s".format(time.time() - start_time))

    # Print BuildConfig & BuildResult member for easier debugging
    print("BuildResult:\n")
    print(' '.join("- %s: \t\t%s\n" % item for item in vars(build_result).items()))
    print("BuildConfig:\n")
    print(' '.join("- %s: \t\t%s\n" % item for item in vars(build_config).items()))

    return build_result


def setup(build_config: BuildConfig, build_result: BuildResult) -> Tuple[BuildConfig, BuildResult]:

    if build_config.use_aqt:
        build_config.aqt_path = Path(
            f"{build_config.root_path}/../aqt/").resolve()

        if not Path(build_config.aqt_path).exists():
            print(
                f"aqt path does not exist at {build_config.aqt_path}. Please make sure you have installed aqt.")
            exit(2)

    # Set default to empty, because it is only used on mac
    build_config.cmake_osx_architectures = ""

    if platform.system() == "Windows":
        build_config.cmake_target_triplet = "x64-windows"
        build_config.executable_file_ending = ".exe"
        build_config.qt_path = build_config.aqt_path if build_config.use_aqt else Path(
            "C:/Qt")
        windows_msvc = "msvc2019_64"  # This will change once prebuild Qt bins change
        build_config.qt_bin_path = build_config.aqt_path.joinpath(build_config.qt_version).joinpath(
            windows_msvc) if build_config.use_aqt else Path(f"C:/Qt/{build_config.qt_version}/{windows_msvc}")
        vs_env_dict = get_vs_env_dict()
        vs_env_dict["PATH"] = vs_env_dict["PATH"] + \
            ";" + str(build_config.qt_bin_path) + "\\bin"
        os.environ.update(vs_env_dict)
        # NO f string we fill it later!
        build_config.deploy_command = "windeployqt.exe --{type}  --qmldir ../../{app}/qml {app}{executable_file_ending}"

        build_config.aqt_install_qt_packages = f"windows desktop {build_config.qt_version} win64_msvc2019_64 -m all"
        build_config.aqt_install_tool_packages = "windows desktop tools_ifw"

    elif platform.system() == "Darwin":
        if(build_config.build_architecture == "arm64"):
            build_config.cmake_target_triplet = "arm64-osx"
            build_config.cmake_osx_architectures = "-DCMAKE_OSX_ARCHITECTURES=arm64"
        elif(build_config.build_architecture == "x64"):
            build_config.cmake_target_triplet = "x64-osx"
            build_config.cmake_osx_architectures = "-DCMAKE_OSX_ARCHITECTURES=x86_64"
        else:
            print("MISSING BUILD ARCH: SET arm64 or x64")
            exit(1)
        build_config.executable_file_ending = ".app"
        build_config.qt_path = build_config.aqt_path if build_config.use_aqt else Path(
            "~/Qt/")
        build_config.qt_bin_path = build_config.aqt_path.joinpath(
            f"{build_config.qt_version}/macos") if build_config.use_aqt else Path(f"~/Qt/{build_config.qt_version}/macos")
        # NO f string we fill it later!
        build_config.deploy_command = "{prefix_path}/bin/macdeployqt {app}.app  -qmldir=../../{app}/qml -executable={app}.app/Contents/MacOS/{app}"

        build_config.aqt_install_qt_packages = f"mac desktop {build_config.qt_version} clang_64 -m all"
        build_config.aqt_install_tool_packages = "mac desktop tools_ifw"

    elif platform.system() == "Linux":
        build_config.cmake_target_triplet = "x64-linux"
        build_config.executable_file_ending = ""
        build_config.qt_path = build_config.aqt_path if build_config.use_aqt else Path(
            "~/Qt/")
        build_config.qt_bin_path = build_config.aqt_path.joinpath(
            f"{build_config.qt_version}/gcc_64") if build_config.use_aqt else Path(f"~/Qt/{build_config.qt_version}/gcc_64")
        build_config.aqt_install_qt_packages = f"linux desktop {build_config.qt_version} gcc_64 -m all"
        build_config.aqt_install_tool_packages = "linux desktop tools_ifw"
        if shutil.which("cqtdeployer"):
            # NO f string we fill it later!
            build_config.deploy_command = "cqtdeployer -qmlDir ../../{app}/qml -bin {app}"
        else:
            print("cqtdeployer not available, build may be incomplete and incompatible with some distro (typically Ubuntu)")
        home_path = str(Path.home())
        build_config.qt_bin_path = build_config.aqt_path.joinpath(
            f"{build_config.qt_version}/gcc_64") if build_config.use_aqt else Path(f"{home_path}/Qt/{build_config.qt_version}/gcc_64")
    else:
        raise NotImplementedError(
            "Unsupported platform, ScreenPlay only supports Windows, macOS and Linux.")

    # Default to QtMaintainance installation.
    if build_config.use_aqt:
        print(f"qt_bin_path: {build_config.qt_bin_path}.")
        if not Path(build_config.aqt_path).exists():
            print(
                f"aqt path does not exist at {build_config.aqt_path}. Installing now into...")
            run(f"aqt install-qt -O ../aqt {build_config.aqt_install_qt_packages}",
                cwd=build_config.root_path)
            run(
                f"aqt install-tool -O ../aqt {build_config.aqt_install_tool_packages}", cwd=build_config.root_path)

    # Prepare
    build_config.cmake_toolchain_file = f"'{build_config.root_path}/../ScreenPlay-vcpkg/scripts/buildsystems/vcpkg.cmake'"
    build_config.ifw_root_path = f"{build_config.qt_path}/Tools/QtInstallerFramework/{build_config.qt_ifw_version}"
    print(f"cmake_toolchain_file: {build_config.cmake_toolchain_file}")
    print(
        f"Starting build with type {build_config.build_type}. Qt Version: {build_config.qt_version}. Root path: {build_config.root_path}")

    # Remove old build folder to before configuring to get rid of
    # all cmake chaches
    build_config.build_folder = build_config.root_path.joinpath(
        f"build-{build_config.cmake_target_triplet}-{build_config.build_type}")
    build_config.bin_dir = build_config.build_folder.joinpath("bin")

    if platform.system() == "Windows":
        build_result.installer = Path(build_config.build_folder).joinpath(
            "ScreenPlay-Installer.exe")
    elif platform.system() == "Darwin":
        build_result.installer = Path(build_config.build_folder).joinpath(
            "ScreenPlay-Installer.dmg")
    elif platform.system() == "Linux":
        build_result.installer = Path(build_config.build_folder).joinpath(
            "ScreenPlay-Installer.run")

    return build_config, build_result


def build(build_config: BuildConfig, build_result: BuildResult) -> BuildResult:
    cmake_configure_command = f'cmake ../ \
	{build_config.cmake_osx_architectures} \
	-DCMAKE_PREFIX_PATH={build_config.qt_bin_path} \
	-DCMAKE_BUILD_TYPE={build_config.build_type} \
	-DVCPKG_TARGET_TRIPLET={build_config.cmake_target_triplet} \
	-DCMAKE_TOOLCHAIN_FILE={build_config.cmake_toolchain_file} \
	-DSCREENPLAY_STEAM={build_config.build_steam} \
	-DSCREENPLAY_TESTS={build_config.build_tests} \
	-DSCREENPLAY_RELEASE={build_config.build_release} \
	-DSCREENPLAY_INSTALLER={build_config.create_installer} \
	-DSCREENPLAY_IFW_ROOT:STRING={build_config.ifw_root_path} \
    -G "CodeBlocks - Ninja" \
	-B.'

    print(f"CMake configure:\n{cmake_configure_command}\n\n")
    run(cmake_configure_command, cwd=build_config.build_folder)
    print(f"\nCMake build:\n")
    run("cmake --build . --target all", cwd=build_config.build_folder)

    build_result.binary = Path(build_config.bin_dir)

    return build_result


def deploy(build_config: BuildConfig):

    if build_config.deploy_command:  # Only deploy if we have the dependencies
        print("Executing deploy commands...")
        run(build_config.deploy_command.format(
            type=build_config.build_type,
            prefix_path=build_config.qt_bin_path,
            app="ScreenPlay",
            executable_file_ending=build_config.executable_file_ending), cwd=build_config.bin_dir)

        run(build_config.deploy_command.format(
            type=build_config.build_type,
            prefix_path=build_config.qt_bin_path,
            app="ScreenPlayWidget",
            executable_file_ending=build_config.executable_file_ending), cwd=build_config.bin_dir)

        run(build_config.deploy_command.format(
            type=build_config.build_type,
            prefix_path=build_config.qt_bin_path,
            app="ScreenPlayWallpaper",
            executable_file_ending=build_config.executable_file_ending), cwd=build_config.bin_dir)
    else:
        # just copy the folders and be done with it
        if platform.system() == "Linux":
            # Copy  all .so files from the qt_bin_path lib folder into bin_dir
            qt_lib_path = build_config.qt_bin_path
            for file in qt_lib_path.joinpath("lib").glob("*.so"):
                shutil.copy(str(file), str(build_config.bin_dir))

            # Copy qt_qml_path folder content into bin_dir
            qt_qml_path = build_config.qt_bin_path
            for folder in qt_qml_path.joinpath("qml").iterdir():
                if not folder.is_file():
                    shutil.copytree(str(folder), str(
                        build_config.bin_dir.joinpath(folder.name)))
                    print("Copied %s" % folder)

            # Copy all plugin folder from qt_bin_path plugins subfolder into bin_dir
            qt_plugins_path = build_config.qt_bin_path
            for folder in build_config.qt_bin_path.joinpath("plugins").iterdir():
                if not folder.is_file():
                    shutil.copytree(str(folder), str(
                        build_config.bin_dir.joinpath(folder.name)))
                    print("Copied %s" % folder)

            # Copy all folder from qt_bin_path translation files into bin_dir translation folder
            qt_translations_path = build_config.qt_bin_path
            for folder in qt_translations_path.joinpath("translations").iterdir():
                if not folder.is_file():
                    shutil.copytree(str(folder), str(
                        build_config.bin_dir.joinpath("translations").joinpath(folder.name)))
                    print("Copied %s" % folder)

            # Copy all  filesfrom qt_bin_path resources folder into bin_dir folder
            qt_resources_path = build_config.qt_bin_path
            for file in build_config.qt_bin_path.joinpath("resources").glob("*"):
                shutil.copy(str(file), str(build_config.bin_dir))
                print("Copied %s" % file)

        else:
            print("Not executing deploy commands due to missing dependencies.")

    # Copy qml dir into all .app/Contents/MacOS/
    if platform.system() == "Darwin":
        copytree(Path.joinpath(build_config.bin_dir, "qml"), Path.joinpath(
            build_config.bin_dir, "ScreenPlay.app/Contents/MacOS/qml"))
        copytree(Path.joinpath(build_config.bin_dir, "qml"), Path.joinpath(
            build_config.bin_dir, "ScreenPlayWallpaper.app/Contents/MacOS/qml"))
        copytree(Path.joinpath(build_config.bin_dir, "qml"), Path.joinpath(
            build_config.bin_dir, "ScreenPlayWidget.app/Contents/MacOS/qml"))

    # Some dlls like openssl do no longer get copied automatically.
    # Lets just copy all of them into bin.
    if platform.system() == "Windows":
        vcpkg_bin_path = Path(
            f"{build_config.root_path}/../ScreenPlay-vcpkg/installed/x64-windows/bin").resolve()
        print(f"Copy dlls from vcpkg bin path: {vcpkg_bin_path}")
        for file in vcpkg_bin_path.iterdir():
            if file.suffix == ".dll" and file.is_file():
                print(file, build_config.bin_dir)
                shutil.copy2(file, build_config.bin_dir)
    if not platform.system() == "Darwin":
        file_endings = [".ninja_deps", ".ninja", ".ninja_log", ".lib", ".a", ".exp",
                        ".manifest", ".cmake", ".cbp", "CMakeCache.txt"]
        for file_ending in file_endings:
            for file in build_config.bin_dir.rglob("*" + file_ending):
                if file.is_file():
                    print("Remove: %s" % file.resolve())
                    file.unlink()


def build_installer(build_config: BuildConfig, build_result: BuildResult):
    os.chdir(build_result.build)
    print("Running cpack at: ", os.getcwd())
    run("cpack", cwd=build_config.build_folder)


def sign(build_config: BuildConfig):

    run("codesign --deep -f -s \"Developer ID Application: Elias Steurer (V887LHYKRH)\" --timestamp --options \"runtime\" -f --entitlements \"../../ScreenPlay/entitlements.plist\"  --deep \"ScreenPlay.app/\"", cwd=build_config.bin_dir)
    run("codesign --deep -f -s \"Developer ID Application: Elias Steurer (V887LHYKRH)\" --timestamp --options \"runtime\" -f --deep \"ScreenPlayWallpaper.app/\"", cwd=build_config.bin_dir)
    run("codesign --deep -f -s \"Developer ID Application: Elias Steurer (V887LHYKRH)\" --timestamp --options \"runtime\" -f --deep \"ScreenPlayWidget.app/\"", cwd=build_config.bin_dir)

    run("codesign --verify --verbose=4  \"ScreenPlay.app/\"", cwd=build_config.bin_dir)
    run("codesign --verify --verbose=4  \"ScreenPlayWallpaper.app/\"",
        cwd=build_config.bin_dir)
    run("codesign --verify --verbose=4  \"ScreenPlayWidget.app/\"",
        cwd=build_config.bin_dir)

    run("xcnotary notarize ScreenPlay.app -d kelteseth@gmail.com -k ScreenPlay",
        cwd=build_config.bin_dir),
    run("xcnotary notarize ScreenPlayWallpaper.app -d kelteseth@gmail.com -k ScreenPlay",
        cwd=build_config.bin_dir),
    run("xcnotary notarize ScreenPlayWidget.app -d kelteseth@gmail.com -k ScreenPlay",
        cwd=build_config.bin_dir)

    run("spctl --assess --verbose  \"ScreenPlay.app/\"", cwd=build_config.bin_dir)
    run("spctl --assess --verbose  \"ScreenPlayWallpaper.app/\"",
        cwd=build_config.bin_dir)
    run("spctl --assess --verbose  \"ScreenPlayWidget.app/\"",
        cwd=build_config.bin_dir)

    # We also need to sign the installer in osx:
    if build_config.create_installer == "ON":
        run("codesign --deep -f -s \"Developer ID Application: Elias Steurer (V887LHYKRH)\" --timestamp --options \"runtime\" -f --deep \"ScreenPlay-Installer.dmg/ScreenPlay-Installer.app/Contents/MacOS/ScreenPlay-Installer\"", cwd=build_config.build_folder)
        run("codesign --verify --verbose=4  \"ScreenPlay-Installer.dmg/ScreenPlay-Installer.app/Contents/MacOS/ScreenPlay-Installer\"",
            cwd=build_config.build_folder)
        run("xcnotary notarize ScreenPlay-Installer.dmg/ScreenPlay-Installer.app -d kelteseth@gmail.com -k ScreenPlay",
            cwd=build_config.build_folder)
        run("spctl --assess --verbose  \"ScreenPlay-Installer.dmg/ScreenPlay-Installer.app/\"",
            cwd=build_config.build_folder)

        run("codesign --deep -f -s \"Developer ID Application: Elias Steurer (V887LHYKRH)\" --timestamp --options \"runtime\" -f --deep \"ScreenPlay-Installer.dmg/\"", cwd=build_config.build_folder)
        run("codesign --verify --verbose=4  \"ScreenPlay-Installer.dmg/\"",
            cwd=build_config.build_folder)
        run("xcnotary notarize ScreenPlay-Installer.dmg -d kelteseth@gmail.com -k ScreenPlay",
            cwd=build_config.build_folder)
        run("spctl --assess --verbose  \"ScreenPlay-Installer.dmg/\"",
            cwd=build_config.build_folder)


def zip(build_config: BuildConfig, build_result: BuildResult) -> BuildResult:
    zipName = f"ScreenPlay-{build_config.screenplay_version}-{build_config.cmake_target_triplet}-{build_config.build_type}.zip"
    build_result.build_zip = Path(build_config.bin_dir).joinpath(zipName)
    print(f"Creating scoop zip file")
    os.chdir(build_config.build_folder)
    with zipfile.ZipFile(zipName, 'w', zipfile.ZIP_DEFLATED) as zipf:
        zipdir(build_config.bin_dir, zipf)

    # Create sha256 hash from zip file
    zip_file_path = os.path.join(build_result.build, zipName)
    build_hash = sha256(zip_file_path)
    build_result.build_hash = Path(
        build_result.build).joinpath(zipName + ".sha256.txt")
    f = open(build_result.build_hash, "a")
    f.write(build_hash)
    f.close()

    # Some weird company firewalls do not allow direct .exe downloads
    # lets just zip the installer lol
    installer_zip = build_result.installer.stem + ".zip"
    build_result.installer_zip = Path(build_result.build).joinpath(build_result.installer.stem + ".zip")
    zipfile.ZipFile(installer_zip, mode='w').write(build_result.installer)

    return build_result


if __name__ == "__main__":

    parser = argparse.ArgumentParser(
        description='Build and Package ScreenPlay')

    parser.add_argument('-qt-version', action="store", dest="qt_version_overwrite",
                        help="Overwrites the default Qt version")
    parser.add_argument('-qt-installer-version', action="store", dest="qt_installer_version_overwrite",
                        help="Overwrites the default Qt installer framework version")
    parser.add_argument('-type', action="store", dest="build_type",
                        help="Build type. This is either debug or release.")
    parser.add_argument('-use-aqt', action="store_true", dest="use_aqt",
                        help="Absolute qt path. If not set the default path is used\Windows: C:\Qt\nLinux & macOS:~/Qt/.")
    parser.add_argument('-sign', action="store_true", dest="sign_build",
                        help="Enable if you want to sign the apps. This is macOS only for now.")
    parser.add_argument('-steam', action="store_true", dest="build_steam",
                        help="Enable if you want to build the Steam workshop plugin.")
    parser.add_argument('-tests', action="store_true", dest="build_tests",
                        help="Build tests.")
    parser.add_argument('-installer', action="store_true", dest="create_installer",
                        help="Create a installer.")
    parser.add_argument('-release', action="store_true", dest="build_release",
                        help="Create a release version of ScreenPlay for sharing with the world. This is not about debug/release build, but the c++ define SCREENPLAY_RELEASE.")
    parser.add_argument('-architecture', action="store", dest="build_architecture",
                        help="Sets the build architecture. Used to build x86 and ARM osx versions. Currently only works with x86_64 and arm64")
    args = parser.parse_args()

    qt_version = "6.3.1"
    screenplay_version = "0.15.0-RC1"
    qt_ifw_version = "4.4"  # Not yet used.
    qt_version_overwrite: str
    use_aqt = False

    if args.qt_version_overwrite:
        qt_version = args.qt_version_overwrite
        print("Using Qt version {qt_version}")

    if args.qt_installer_version_overwrite:
        qt_ifw_version = args.qt_installer_version_overwrite
        print("Using Qt installer framework version {qt_ifw_version}")

    if not args.build_type:
        print("Build type argument is missing (release, debug). E.g: python build_config.py -type release -steam")
        print(
            f"Defaulting to Qt version: {qt_version}. This can be overwritten via -qt-version 6.5.0")
        exit(1)
    build_type = args.build_type

    build_steam = "OFF"
    if args.build_steam:
        build_steam = "ON"

    build_tests = "OFF"
    if args.build_tests:
        build_tests = "ON"

    build_release = "OFF"
    if args.build_release:
        build_release = "ON"

    create_installer = "OFF"
    if args.create_installer:
        create_installer = "ON"

    sign_build = False
    if args.sign_build:
        sign_build = True

    if args.use_aqt:
        use_aqt = True

    build_config = BuildConfig()
    build_config.qt_version = qt_version
    build_config.qt_ifw_version = qt_ifw_version
    build_config.build_steam = build_steam
    build_config.build_tests = build_tests
    build_config.build_release = build_release
    build_config.create_installer = create_installer
    build_config.build_type = build_type
    build_config.sign_build = args.sign_build
    build_config.use_aqt = use_aqt
    build_config.screenplay_version = screenplay_version
    build_config.build_architecture = args.build_architecture

    execute(build_config)
