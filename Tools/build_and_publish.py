import steam_publish
import argparse
import os
import build
from pathlib import Path
from macos_lipo import run_lipo, check_fat_binary
import platform
import paramiko
from util import sftp_exists

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
    build_config.sign_build = True
    build_config.use_aqt = False


    if platform.system() == "Darwin":
        # We do not yet support a standalone osx installer
        build_config.create_installer = "OFF"
        # OSX builds needs to build for x86 and arm
        # and also be signed!
        
        build_config.build_architecture = "arm64"
        build_result = build.execute(build_config)

        build_config.build_architecture = "x64"
        build_result = build.execute(build_config)

        # Make sure to reset to tools path
        os.chdir(root_path)
        # Create universal (fat) binary
        run_lipo()
        check_fat_binary()
    else:
        build_config.build_architecture = "x64"
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

    # Make sure to reset to tools path
    os.chdir(tools_path)
    steam_publish.publish(
        steam_username="tachiom",
        steam_password=args.steam_password,
        set_live_branch_name="internal"
    )
