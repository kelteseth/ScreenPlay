import steam_publish
import shutil
import sys
import macos_sign
import argparse
import os
import build
from pathlib import Path
from macos_lipo import run_lipo, check_fat_binary
import platform
import paramiko

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Build and Package ScreenPlay')
    parser.add_argument('-steam_password', '-sp', action="store", dest="steam_password",  help="Steam password")
    parser.add_argument('-hosting_username','-hu', action="store", dest="hosting_username", help="ssh username")
    parser.add_argument('-hosting_password', '-hp', action="store", dest="hosting_password",  help="ssh password")
    args = parser.parse_args()

    # Script needs to run in the tools folder
    tools_path = Path.cwd()
    os.chdir(tools_path)
    root_path = tools_path.parent
    print(f"Set root directory to: {root_path}")

    build_result = build.BuildResult()

    build_config = build.BuildConfig()
    build_config.screenplay_version = "0.15.0-RC2"
    build_config.qt_version = "6.3.1"
    build_config.qt_ifw_version = "4.4"
    build_config.build_steam = "ON"
    build_config.build_tests = "OFF"
    build_config.build_deploy = "ON"
    build_config.create_installer = "ON"
    build_config.build_type = "release"
    build_config.use_aqt = False

    if platform.system() == "Darwin":
        # We do not yet support a standalone osx installer
        build_config.create_installer = "OFF"
        # OSX builds needs to build for x86 and arm
        # and also be signed!

        # We need to manually package here at the end after
        # we run 
        build_config.package = True

        # Remove old build-universal-osx-release dir that does not automatically
        # deleted because it is not build directly but generated from x64 and arm64
        universal_build_dir = Path(os.path.join(root_path, "build-universal-osx-release"))
        if universal_build_dir.exists():
            print(f"Remove previous build folder: {universal_build_dir}")
            # ignore_errors removes also not empty folders...
            shutil.rmtree(universal_build_dir, ignore_errors=True)
        
        build_config.build_architecture = "arm64"
        build_result = build.execute(build_config)

        build_config.build_architecture = "x64"
        build_result = build.execute(build_config)

        # Make sure to reset to tools path
        os.chdir(root_path)
        # Create universal (fat) binary
        run_lipo()
        check_fat_binary()

        build_config.bin_dir = os.path.join(build_config.root_path,'build-universal-osx-release/bin/')
        print(f"Change binary dir to: {build_config.bin_dir}")
        macos_sign.sign(build_config=build_config)
    else:
        build_config.build_architecture = "x64"
        build_result = build.execute(build_config)

        ssh = paramiko.SSHClient() 
        ssh.load_host_keys(os.path.expanduser(os.path.join("~", ".ssh", "known_hosts")))
        ssh.connect('kelteseth.com', username=args.hosting_username, password=args.hosting_password)
        sftp = ssh.open_sftp()
        release_folder = "/kelteseth_com/public/releases/" + build_config.screenplay_version + "/"
        sftp.mkdir(release_folder)
        print("Uploading files...")

        sftp.put(build_result.build_zip,    release_folder + str(build_result.build_zip.name))
        sftp.put(build_result.installer,    release_folder + str(build_result.installer.name))
        sftp.put(build_result.installer_zip,release_folder + str(build_result.installer_zip.name))
        sftp.put(build_result.build_hash,   release_folder + str(build_result.build_hash.name))
        sftp.close()
        ssh.close()

        

    # Make sure to reset to tools path
    os.chdir(tools_path)
    steam_publish.publish(
        steam_username="tachiom",
        steam_password=args.steam_password,
        set_live_branch_name="internal"
    )
