#!/usr/bin/python3
# SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
import steam_publish
import sys
import argparse
import os
import subprocess
from pathlib import Path
import platform
from util import repo_root_path
from sys import stdout

stdout.reconfigure(encoding='utf-8')

if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description='Build and Package ScreenPlay')
    parser.add_argument('--skip_steam_publish', '-skstp', action="store_true",
                        dest="skip_steam_publish", default=False,  help="skip publish")
    parser.add_argument('--skip_build', '-skb', action="store_true", dest="skip_build",
                        default=False, help="skip build. If we already have a build and only want to upload it")
    parser.add_argument('--steam_password', '-sp', action="store",
                        dest="steam_password",  help="Steam password")
    args = parser.parse_args()

    if not args.skip_steam_publish and args.steam_password is None:
        print("Steam password is required.")
        sys.exit(1)

    if args.skip_build:
        print("Skip build step.")
    elif platform.system() != "Windows":
        print(f"Skip build: unsupported platform {platform.system()} for Steam deploy.")
    else:
        build_cmd = ["uv", "run", "python", "Tools/build.py", "--preset=windows-deploy-release"]
        print(f"Running build: {' '.join(build_cmd)}")
        subprocess.run(build_cmd, check=True, cwd=repo_root_path())

    if args.skip_steam_publish:
        print("Skip steam publishing.")
        sys.exit(0)

    if not steam_publish.check_steam_login("tachiom", args.steam_password):
        print("Failed to login to Steam!")
        exit(1)

    # Make sure to reset to tools path
    os.chdir(path=Path(repo_root_path()).joinpath("Tools"))
    steam_publish.publish(
        steam_username="tachiom",
        steam_password=args.steam_password,
        set_live_branch_name="internal"
    )
