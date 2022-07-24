import steam_publish
import argparse
import os
import build
from pathlib import Path
from macos_lipo import run_lipo, check_fat_binary
import platform
import ftplib


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Build and Package ScreenPlay')
    parser.add_argument('-steam_password', action="store", dest="steam_password", required=True, help="Steam password")
    parser.add_argument('-hosting_password', action="store", dest="hosting_password", required=True, help="FTPS password")
    args = parser.parse_args()

    # Script needs to run in the tools folder
    tools_path = Path.cwd()
    os.chdir(tools_path)
    root_path = tools_path.parent
    print(f"Set root directory to: {root_path}")

    build_result = build.BuildResult()

    build_config = build.BuildConfig()
    build_config.screenplay_version = "0.15.0-RC1"
    build_config.qt_version = "6.3.1"
    build_config.qt_ifw_version = "4.4"
    build_config.build_steam = "ON"
    build_config.build_tests = "OFF"
    build_config.build_release = "ON"
    build_config.create_installer = "ON"
    build_config.build_type = "release"
    build_config.sign_build = True
    build_config.use_aqt = False


    if platform.system() == "Darwin":
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

    # Make sure to reset to tools path
    #os.chdir(tools_path)
    #steam_publish.publish(
    #    steam_username="tachiom",
    #    steam_password=args.steam_password,
    #    set_live_branch_name="internal"
    #)

    #session = ftplib.FTP('wallpaper.software','screenplay_releases',args.hosting_password)

    #file = open('kitten.jpg','rb')                  # file to send
    #session.storbinary('STOR kitten.jpg', file)     # send the file
    #file.close()                                    # close file and FTP
    #session.quit()
