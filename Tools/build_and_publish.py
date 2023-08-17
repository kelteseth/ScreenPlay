#!/usr/bin/python3
# SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
import steam_publish
import sys
import argparse
import os
import build
from pathlib import Path
import platform
import paramiko
import defines
from util import sftp_exists, get_latest_git_tag, parse_semver, semver_to_string
from sys import stdout

stdout.reconfigure(encoding='utf-8')

if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description='Build and Package ScreenPlay')
    parser.add_argument('--skip_steam_publish', '-skstp', action="store_true",
                        dest="skip_publish", default=False,  help="skip publish")
    parser.add_argument('--skip_getspde_publish', '-skgsp', action="store_true",
                        dest="skip_getspde_publish", default=False,  help="skip getsp.de publish")
    parser.add_argument('--skip_build', '-skb', action="store_true", dest="skip_build",
                        default=False, help="skip build. If we already have a build and only want to upload it")
    parser.add_argument('--steam_password', '-sp', action="store",
                        dest="steam_password",  help="Steam password")
    parser.add_argument('--hosting_username', '-hu', action="store",
                        dest="hosting_username", help="ssh username")
    parser.add_argument('--hosting_password', '-hp', action="store",
                        dest="hosting_password",  help="ssh password")
    args = parser.parse_args()

    # Script needs to run in the tools folder
    tools_path = Path.cwd()
    os.chdir(tools_path)
    root_path = tools_path.parent
    print(f"Set root directory to: {root_path}")

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

    build_result = build.BuildResult()

    build_config = build.BuildConfig()
    build_config.qt_version = defines.QT_VERSION
    build_config.qt_ifw_version = defines.QT_IFW_VERSION
    build_config.build_steam = "ON"
    build_config.build_tests = "OFF"
    build_config.build_deploy = "ON"
    build_config.create_installer = "ON"
    build_config.build_type = "release"
    build_config.screenplay_version = screenplay_version

    if platform.system() == "Darwin" and not args.skip_build:
        # We do not yet support a standalone osx installer
        build_config.create_installer = "OFF"

        # We need to manually package here at the end after
        build_config.package = True
        build_config.sign_osx = True

        # This will build both arm64 and x64 and sign the unversal binary
        build_result = build.execute(build_config)

    if platform.system() == "Windows" and not args.skip_build:
        build_config.build_architecture = "x64"

        if not args.skip_getspde_publish:
            # Steamless version first
            build_config.build_steam = "OFF"
            build_result = build.execute(build_config)
            ssh = paramiko.SSHClient()
            ssh.load_system_host_keys()
            ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
            ssh.connect('kelteseth.com', username=args.hosting_username,
                        password=args.hosting_password)
            sftp = ssh.open_sftp()
            release_folder = "/getsp.de/" + build_config.screenplay_version + "/"
            if sftp_exists(sftp, release_folder):
                remoteFiles = sftp.listdir(path=release_folder)
                for file in remoteFiles:
                    print(f"Delte old: {release_folder+file}")
                    sftp.remove(release_folder+file)
            else:
                sftp.mkdir(release_folder)
            print("Uploading files...")

            sftp.put(build_result.build_zip,    release_folder +
                     str(build_result.build_zip.name))
            sftp.put(build_result.installer,    release_folder +
                     str(build_result.installer.name))
            sftp.put(build_result.installer_zip, release_folder +
                     str(build_result.installer_zip.name))
            sftp.put(build_result.build_hash,   release_folder +
                     str(build_result.build_hash.name))
            sftp.close()
            ssh.close()

        # Now build the steam version
        os.chdir(tools_path)
        build_config.build_steam = "ON"
        build_config.create_installer = "OFF"
        build_result = build.execute(build_config)

    if args.skip_steam_publish:
        print("Skip steam publishing.")
        sys.exit(0)

    if args.steam_password is None:
        print("Steam password is required.")
        sys.exit(1)

    # Make sure to reset to tools path
    os.chdir(tools_path)
    steam_publish.publish(
        steam_username="tachiom",
        steam_password=args.steam_password,
        set_live_branch_name="internal"
    )
