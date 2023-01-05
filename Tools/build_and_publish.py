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
import defines
from util import sftp_exists
from sys import stdout

stdout.reconfigure(encoding='utf-8')

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Build and Package ScreenPlay')
    parser.add_argument('-skip_publish', '-skp', action="store_true", dest="skip_publish", default=False,  help="skip publish")
    parser.add_argument('-skip_build', '-skb', action="store_true", dest="skip_build", default=False, help="skip build. If we already have a build and only want to upload it")
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
    build_config.screenplay_version = defines.SCREENPLAY_VERSION
    build_config.qt_version = defines.QT_VERSION
    build_config.qt_ifw_version = defines.QT_IFW_VERSION
    build_config.build_steam = "ON"
    build_config.build_tests = "OFF"
    build_config.build_deploy = "ON"
    build_config.create_installer = "ON"
    build_config.build_type = "release"

    if platform.system() == "Darwin" and not args.skip_build:
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

    if platform.system() == "Windows" and not args.skip_build:
        build_config.build_architecture = "x64"
        
        if not args.skip_publish:
            # Steamless version first
            build_config.build_steam = "OFF"
            build_result = build.execute(build_config)
            ssh = paramiko.SSHClient() 
            ssh.load_system_host_keys()
            ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
            ssh.connect('kelteseth.com', username=args.hosting_username, password=args.hosting_password)
            sftp = ssh.open_sftp()
            release_folder = "/kelteseth_com/public/releases/" + build_config.screenplay_version + "/"
            if sftp_exists(sftp,release_folder):
                remoteFiles = sftp.listdir(path=release_folder)
                for file in remoteFiles:
                    print(f"Delte old: {release_folder+file}")
                    sftp.remove(release_folder+file)
            else:
                sftp.mkdir(release_folder)
            print("Uploading files...")

            sftp.put(build_result.build_zip,    release_folder + str(build_result.build_zip.name))
            sftp.put(build_result.installer,    release_folder + str(build_result.installer.name))
            sftp.put(build_result.installer_zip,release_folder + str(build_result.installer_zip.name))
            sftp.put(build_result.build_hash,   release_folder + str(build_result.build_hash.name))
            sftp.close()
            ssh.close()

        # Now build the steam version
        os.chdir(tools_path)
        build_config.build_steam = "ON"
        build_config.create_installer = "OFF"
        build_result = build.execute(build_config)

    if args.skip_publish:
        print("Skip publishing.")
        sys.exit(0)

    # Make sure to reset to tools path
    os.chdir(tools_path)
    steam_publish.publish(
        steam_username="tachiom",
        steam_password=args.steam_password,
        set_live_branch_name="internal"
    )
