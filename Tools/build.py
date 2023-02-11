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
from typing import Tuple
from shutil import copytree
from pathlib import Path
from macos_lipo import run_lipo, check_fat_binary
import macos_sign
from util import sha256, cd_repo_root_path,repo_root_path, zipdir, run, get_vs_env_dict
from sys import stdout

stdout.reconfigure(encoding='utf-8')


def clean_build_dir(build_dir):
    if build_dir.exists():
        print(f"Remove previous build folder: {build_dir}")
        # ignore_errors removes also not empty folders...
        shutil.rmtree(build_dir, ignore_errors=True)
    build_dir.mkdir(parents=True, exist_ok=True)


class BuildResult:       
    # Windows example with absolute paths:
    # [...]/build-x64-windows-release/
    build: Path
    # [...]/build-x64-windows-release/bin
    bin: Path
    # [...]/build-x64-windows-release/ScreenPlay-Installer.exe
    installer: Path
    # [...]/build-x64-windows-release/ScreenPlay-Installer.zip
    installer_zip: Path
    # [...]/build-x64-windows-release/ScreenPlay-0.X.0-RCX-x64-windows-release.zip
    build_zip: Path
    # [...]/build-x64-windows-release/ScreenPlay-0.X.0-RCX-x64-windows-release.txt :sha256, needed for scoop
    build_hash: Path
    # x64, arm64, universal
    build_arch: str

class BuildConfig:
    root_path: str
    cmake_osx_architectures: str
    cmake_target_triplet: str
    package: bool
    osx_bundle: str
    package_command: str
    executable_file_ending: str
    # qt_* use either aqt or from the maintenance tool
    qt_path: str #  C:\Qt
    qt_bin_path: str #  C:\Qt\6.3.2\msvc2019_64
    qt_version: str
    qt_ifw_version: str
    ifw_root_path: str
    cmake_toolchain_file: str
    aqt_install_qt_packages: str
    aqt_install_tool_packages: str
    executable_file_ending: str
    build_folder: str
    bin_dir: str
    screenplay_version: str
    # CMake variables need str: "ON" or "OFF"
    build_steam: str
    build_tests: str
    build_deploy: str
    build_type: str
    build_architecture: str
    create_installer: str
    sign_osx: bool


def execute(
    build_config: BuildConfig
) -> BuildResult:
    start_time = time.time()
    # Make sure the script is always started from the same folder
    build_config.root_path = cd_repo_root_path()

    build_result = BuildResult()

    # Sets all platform spesific paths, arguments etc.
    setup_tuple = setup(build_config, build_result)
    build_config = setup_tuple[0]
    build_result = setup_tuple[1]

    build_result.build = Path(build_config.build_folder)
    build_result.bin = Path(build_config.bin_dir)

    # Make sure to always delete everything first.
    # 3rd party tools like the crashreporter create local
    # temporary files in the build directory.
    clean_build_dir(build_config.build_folder)


    # Runs cmake configure and cmake build
    step_time = time.time()
    build_result = build(build_config, build_result)
    build_duration = time.time() - step_time
    print(f"⏱️ build_duration (for {build_config.build_architecture}): {build_duration}s")

    if platform.system() == "Darwin":
        # Swap the architecture for the second build
        if build_config.build_architecture == "arm64":
            build_config.build_architecture = "x64"
        else:
            build_config.build_architecture = "arm64"

        # Make sure the script is always started from the same folder
        build_config.root_path = cd_repo_root_path()

        build_result = BuildResult()

        # Sets all platform spesific paths, arguments etc.
        setup_tuple = setup(build_config, build_result)
        build_config = setup_tuple[0]
        build_result = setup_tuple[1]

        build_result.build = Path(build_config.build_folder)
        build_result.bin = Path(build_config.bin_dir)

        # Make sure to always delete everything first.
        # 3rd party tools like the crashreporter create local
        # temporary files in the build directory.
        clean_build_dir(build_config.build_folder)
        step_time = time.time()
        build_result = build(build_config, build_result)
        build_duration = time.time() - step_time
        print(f"⏱️ Second build_duration (for {build_config.build_architecture}): {build_duration}s")



    # Copies all needed libraries and assets into the bin folder
    step_time = time.time()
    package(build_config)
    package_duration = time.time() - step_time
    print(f"⏱️ package_duration: {package_duration}s")


    # Creates a Qt InstallerFrameWork (IFW) installer
    if build_config.create_installer == "ON":
        step_time = time.time()
        build_installer(build_config, build_result)
        build_installer_duration = time.time() - step_time
        print(f"⏱️ build_installer_duration: {build_installer_duration}s")

    # Create a zip file for scoop & chocolatey
    if platform.system() == "Windows":
        step_time = time.time()
        build_result = zip(build_config, build_result)
        zip_duration = time.time() - step_time
        print(f"⏱️ zip_duration: {zip_duration}s")

    duration = time.time() - start_time
    print(f"⏱️ Build completed in: {duration}s")

    # Print BuildConfig & BuildResult member for easier debugging
    print("\n🆗 BuildResult:")
    print(' '.join("\n- %s: \t\t%s" % item for item in vars(build_result).items()))
    print("\n⚙️ BuildConfig:")
    print(' '.join("\n- %s: \t\t%s" % item for item in vars(build_config).items()))

    return build_result


def setup(build_config: BuildConfig, build_result: BuildResult) -> Tuple[BuildConfig, BuildResult]:

    build_config.qt_path = defines.QT_PATH

    if not build_config.qt_path.exists():
        print(f"Qt path does not exist at {build_config.qt_path}. Please make sure to run setup.py!")
        exit(2)
    build_config.qt_bin_path = Path(build_config.qt_path).joinpath(f"{build_config.qt_version}/{defines.QT_PLATFORM}").resolve()
    build_config.ifw_root_path = Path(f"{build_config.qt_path}/Tools/QtInstallerFramework/{build_config.qt_ifw_version}").resolve()    
    
    # Set default to empty for linux and windows, because it is only used on mac
    build_config.cmake_osx_architectures = ""

    if platform.system() == "Windows":
        build_config.cmake_target_triplet = "x64-windows"
        build_config.executable_file_ending = ".exe"
        env_dict = get_vs_env_dict()
        env_dict["PATH"] = env_dict["PATH"] + \
            ";" + str(Path(build_config.qt_bin_path).joinpath("bin").resolve()) + \
            ";" + str(build_config.ifw_root_path.resolve())
        print(f"Using env_dict:\n{env_dict}")
        os.environ.update(env_dict)
        # NO f string we fill it later!
        build_config.package_command = "windeployqt.exe --{type}  --qmldir ../../{app}/qml {app}{executable_file_ending}"
        build_config.aqt_install_qt_packages = f"windows desktop {build_config.qt_version} win64_msvc2019_64 -m all"
        build_config.aqt_install_tool_packages = "windows desktop tools_ifw"

    elif platform.system() == "Darwin":
        # Defalt to arm64
        if(build_config.build_architecture == ""):
            build_config.cmake_target_triplet = "arm64-osx"
            build_config.cmake_osx_architectures = "-DCMAKE_OSX_ARCHITECTURES=arm64"
        elif(build_config.build_architecture == "x64"):
            build_config.cmake_target_triplet = "x64-osx"
            build_config.cmake_osx_architectures = "-DCMAKE_OSX_ARCHITECTURES=x86_64"
        build_config.executable_file_ending = ".app"
        # NO f string we fill it later!
        #build_config.package_command = "{prefix_path}/bin/macdeployqt ScreenPlay.app  -qmldir=../../{app}/qml -executable=ScreenPlay.app/Contents/MacOS/{app} -appstore-compliant -timestamp -hardened-runtime"
        build_config.aqt_install_qt_packages = f"mac desktop {build_config.qt_version} clang_64 -m all"
        build_config.aqt_install_tool_packages = "mac desktop tools_ifw"
        
    elif platform.system() == "Linux":
        build_config.cmake_target_triplet = "x64-linux"
        build_config.executable_file_ending = ""
        build_config.aqt_install_qt_packages = f"linux desktop {build_config.qt_version} gcc_64 -m all"
        build_config.aqt_install_tool_packages = "linux desktop tools_ifw"
    else:
        raise NotImplementedError(
            "Unsupported platform, ScreenPlay only supports Windows, macOS and Linux.")

    # Prepare
    build_config.cmake_toolchain_file = Path(f"{build_config.root_path}/../vcpkg/scripts/buildsystems/vcpkg.cmake").resolve()
    print(f"cmake_toolchain_file: {build_config.cmake_toolchain_file}")
    print(f"Starting build with type {build_config.build_type}.")
    print(f"Qt Version: {build_config.qt_version}. Root path: {build_config.root_path}")

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
	-DSCREENPLAY_DEPLOY={build_config.build_deploy} \
	-DSCREENPLAY_INSTALLER={build_config.create_installer} \
	-DSCREENPLAY_IFW_ROOT:STRING={build_config.ifw_root_path} \
    -G "CodeBlocks - Ninja" \
	-B.'

    print(f"\n⚙️ CMake configure:\n", cmake_configure_command.replace(
        "-D", "\n-D").replace("-G", "\n-G"), "\n")
    run(cmake_configure_command, cwd=build_config.build_folder)
    print(f"\n\n⚙️ CMake build:\n\n")
    run("cmake --build . --target all", cwd=build_config.build_folder)

    build_result.bin = Path(build_config.bin_dir)

    return build_result


def package(build_config: BuildConfig):

    if platform.system() == "Darwin":
        universal_build_dir = Path(os.path.join(repo_root_path(), "build-universal-osx-release"))
        if universal_build_dir.exists():
            print(f"Remove previous build folder: {universal_build_dir}")
            # ignore_errors removes also not empty folders...
            shutil.rmtree(universal_build_dir, ignore_errors=True)

        # Make sure to reset to tools path
        os.chdir(repo_root_path())
        # Create universal (fat) binary
        run_lipo()
        check_fat_binary()

        cmd_raw = "{qt_bin_path}/macdeployqt {build_bin_dir}/ScreenPlay.app  -qmldir={repo_root_path}/{app}/qml -executable={build_bin_dir}/ScreenPlay.app/Contents/MacOS/{app} -verbose=1 -appstore-compliant -timestamp -hardened-runtime " # -sign-for-notarization=\"Developer ID Application: Elias Steurer (V887LHYKRH)\"
        build_bin_dir = Path(repo_root_path()).joinpath("build-universal-osx-release/bin/")
        cwd = Path(repo_root_path()).joinpath("build-universal-osx-release/bin/ScreenPlay.app/Contents/MacOS/")
        qt_bin_path = Path(defines.QT_BIN_PATH).resolve()
        source_path = Path(repo_root_path()).resolve()

        run(cmd=cmd_raw.format(qt_bin_path=qt_bin_path,repo_root_path=source_path, build_bin_dir=build_bin_dir, app="ScreenPlay"), cwd=cwd)
        run(cmd=cmd_raw.format(qt_bin_path=qt_bin_path,repo_root_path=source_path, build_bin_dir=build_bin_dir, app="ScreenPlayWallpaper"), cwd=cwd)
        run(cmd=cmd_raw.format(qt_bin_path=qt_bin_path,repo_root_path=source_path, build_bin_dir=build_bin_dir, app="ScreenPlayWidget"), cwd=cwd)

        if(build_config.sign_osx):
            build_config.bin_dir = os.path.join(repo_root_path(),'build-universal-osx-release/bin/')
            print(f"Sign binary at: {build_config.bin_dir}")
            macos_sign.sign(build_config=build_config)


    if platform.system() == "Windows":
        print("Executing deploy commands...")
        run(build_config.package_command.format(
            type=build_config.build_type,
            prefix_path=build_config.qt_bin_path,
            app="ScreenPlay",
            executable_file_ending=build_config.executable_file_ending), cwd=build_config.bin_dir)

        run(build_config.package_command.format(
            type=build_config.build_type,
            prefix_path=build_config.qt_bin_path,
            app="ScreenPlayWidget",
            executable_file_ending=build_config.executable_file_ending), cwd=build_config.bin_dir)

        run(build_config.package_command.format(
            type=build_config.build_type,
            prefix_path=build_config.qt_bin_path,
            app="ScreenPlayWallpaper",
            executable_file_ending=build_config.executable_file_ending), cwd=build_config.bin_dir)

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



    # Some dlls like openssl do no longer get copied automatically.
    # Lets just copy all of them into bin.
    if platform.system() == "Windows":
        vcpkg_bin_path = Path(
            f"{build_config.root_path}/../vcpkg/installed/x64-windows/bin").resolve()
        print(f"Copy dlls from vcpkg bin path: {vcpkg_bin_path}")
        for file in vcpkg_bin_path.iterdir():
            if file.suffix == ".dll" and file.is_file():
                print(file, build_config.bin_dir)
                shutil.copy2(file, build_config.bin_dir)

        # ⚠️WORKAROUND. REMOVE ME WHEN FIXED: 
        # https://bugreports.qt.io/browse/QTBUG-110937
        qt_bin_path = defines.QT_BIN_PATH
        qt6Widgets_path = Path(qt_bin_path).joinpath("Qt6Widgets.dll").resolve()
        print(f"⚠️WORKAROUND: Copy Qt6Widgets.dll from: {qt6Widgets_path}")
        shutil.copy2(qt6Widgets_path, build_config.bin_dir)


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
        build_result.installer_zip = Path(build_result.build).joinpath(build_result.installer.stem + ".zip")
        print(f"Create zip from installer: {build_result.installer_zip}")
        zipfile.ZipFile(build_result.installer_zip, 'w').write(build_result.installer, build_result.installer.name)

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
    parser.add_argument('-steam', action="store_true", dest="build_steam",
                        help="Enable if you want to build the Steam workshop plugin.")
    parser.add_argument('-tests', action="store_true", dest="build_tests",
                        help="Build tests.")
    parser.add_argument('-installer', action="store_true", dest="create_installer",
                        help="Create a installer.")
    parser.add_argument('-sign_osx', action="store_true", dest="sign_osx", default=False,
                        help="Signs the executable on macOS. This requires a valid Apple Developer ID set up.")
    parser.add_argument('-deploy-version', action="store_true", dest="build_deploy",
                        help="Create a deploy version of ScreenPlay for sharing with the world. A not deploy version is for local development only!")
    parser.add_argument('-architecture', action="store", dest="build_architecture", default="",
                        help="Sets the build architecture. Used to build x86 and ARM osx versions. Currently only works with x86_64 and arm64")
    args = parser.parse_args()

    qt_version = defines.QT_VERSION
    screenplay_version = defines.SCREENPLAY_VERSION
    qt_ifw_version = defines.QT_IFW_VERSION  # Not yet used.
    qt_version_overwrite: str
    use_aqt = False

    if args.qt_version_overwrite:
        qt_version = args.qt_version_overwrite
        print("Using Qt version {qt_version}")

    if args.qt_installer_version_overwrite:
        qt_ifw_version = args.qt_installer_version_overwrite
        print("Using Qt installer framework version {qt_ifw_version}")

    if not args.build_type:
        parser.print_help()
        print("\n\nBuild type argument is missing (release, debug). E.g: python build.py -type release -steam\n\n")
        exit(1)

    build_type = args.build_type

    build_steam = "OFF"
    if args.build_steam:
        build_steam = "ON"

    build_tests = "OFF"
    if args.build_tests:
        build_tests = "ON"

    build_deploy = "OFF"
    if args.build_deploy:
        build_deploy = "ON"

    create_installer = "OFF"
    if args.create_installer:
        create_installer = "ON"

    if args.use_aqt:
        use_aqt = True

    build_config = BuildConfig()
    build_config.qt_version = qt_version
    build_config.qt_ifw_version = qt_ifw_version
    build_config.build_steam = build_steam
    build_config.build_tests = build_tests
    build_config.build_deploy = build_deploy
    build_config.create_installer = create_installer
    build_config.build_type = build_type
    build_config.screenplay_version = screenplay_version
    build_config.build_architecture = args.build_architecture
    build_config.sign_osx = args.sign_osx

    execute(build_config)
