from build import build
import steam_publish
import argparse
import os
from pathlib import Path
from macos_lipo import run_lipo, check_fat_binary
import platform

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Build and Package ScreenPlay')
    parser.add_argument('-steam_password', action="store", dest="steam_password", required=True, help="Steam password")
    args = parser.parse_args()

    # Script needs to run in the tools folder
    tools_path = Path.cwd()
    os.chdir(tools_path)
    root_path = tools_path.parent
    print(f"Set root directory to: {root_path}")

    if platform.system() == "Darwin":
        # OSX builds needs to build for x86 and arm
        # and also be signed!
        build(
            qt_version="6.4.0",
            qt_ifw_version="4.4", 
            build_steam="ON",
            build_tests="OFF",
            build_release="ON",
            create_installer="ON",
            build_type="release",
            sign_build=True,
            use_aqt=False,
            build_architecture = "arm64"
        )
        build(
            qt_version="6.4.0",
            qt_ifw_version="4.4", 
            build_steam="ON",
            build_tests="OFF",
            build_release="ON",
            create_installer="OFF",
            build_type="release",
            sign_build=True,
            use_aqt=False,
            build_architecture = "x64"
        )
        # Make sure to reset to tools path
        os.chdir(root_path)
        # Create universal (fat) binary
        run_lipo()
        check_fat_binary()
    else:
        build(
            qt_version="6.4.0",
            qt_ifw_version="4.4", 
            build_steam="ON",
            build_tests="OFF",
            build_release="ON",
            create_installer="OFF",
            build_type="release",
            sign_build=False,
            use_aqt=False
        )
    # Make sure to reset to tools path
    os.chdir(tools_path)
    #steam_publish(
    #    steam_username="tachiom",
    #    steam_password=args.steam_password,
    #    set_live_branch_name="internal"
    #)
