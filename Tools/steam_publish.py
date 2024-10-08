#!/usr/bin/python3
# SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
import os
import sys
import subprocess
import shutil
import argparse
from pathlib import Path
from sys import platform
from execute_util import execute
from datetime import datetime
from shutil import copyfile
import subprocess
from util import cd_repo_root_path
import platform
from sys import stdout

stdout.reconfigure(encoding='utf-8')


class PublishConfig:
    vdf_config_name: str
    depot_config_name: str
    steamcmd_path: Path


def init_publish_config():
    config = PublishConfig()

    root_path = cd_repo_root_path()
    tools_path = os.path.join(root_path, "Tools")
    contentBuiler_path = os.path.join(tools_path, "Steam/ContentBuilder/")

    if platform.system() == "Windows":
        config.vdf_config_name = "app_build_windows.vdf"
        config.depot_config_name = "depot_build_windows.vdf"
        config.steamcmd_path = os.path.join(
            contentBuiler_path, "builder/steamcmd.exe")
    elif platform.system() == "Darwin":
        config.vdf_config_name = "app_build_mac.vdf"
        config.depot_config_name = "depot_build_mac.vdf"
        config.steamcmd_path = os.path.join(
            contentBuiler_path, "builder_osx/steamcmd")
        execute(f"chmod +x {config.steamcmd_path}")
    elif platform.system() == "Linux":
        config.vdf_config_name = "app_build_linux.vdf"
        config.depot_config_name = "depot_build_linux.vdf"
        config.steamcmd_path = os.path.join(
            contentBuiler_path, "builder_linux/steamcmd.sh")
        execute(f"chmod +x {config.steamcmd_path}")

    return config


def check_steam_login(username: str, password: str):
    config = init_publish_config()
    cmd = [config.steamcmd_path, "+login", username, password, "+quit"]

    try:
        with subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True) as process:
            try:
                stdout, stderr = process.communicate(
                    timeout=60)  # 1 minute timeout
                for line in stdout.splitlines():
                    print(line)  # Print the line for debugging purposes

                    if "Logging in user" in line:
                        return True
                    elif "Steam Guard code" in line:
                        process.terminate()
                        return False
            except subprocess.TimeoutExpired:
                process.kill()
                print("Steam login check timed out after 1 minute.")
                return False
    except Exception as e:
        print(f"Error during Steam login check: {e}")
        return False


# Executes steamcmd with username and password. Changes the content of the config
# for better readability in the steam builds tab
# https://partner.steamgames.com/apps/builds/672870

def get_git_revision_short_hash():
    return subprocess.check_output(['git', 'rev-parse', '--short', 'HEAD'])


def get_git_commit_text():
    return subprocess.check_output(['git', 'log', '-1', '--pretty=%B'])


def publish(
    steam_username,
    steam_password,
    set_live_branch_name
):

    # Make sure the script is always started from the same folder
    root_path = cd_repo_root_path()
    tools_path = os.path.join(root_path, "Tools")

    config = init_publish_config()

    print(f"Set steamCmd path: {config.steamcmd_path}")

    abs_vdf_path = os.path.join(
        tools_path, "Steam/steamcmd/" + config.vdf_config_name)

    if not os.path.isfile(abs_vdf_path):
        print("Incorrect vdf name")
        sys.exit(-1)

    file = open(abs_vdf_path, "r")

    config_content = file.read()
    git_hash = get_git_revision_short_hash().decode("utf-8").replace("\n", "")
    git_commit_text = get_git_commit_text().decode("utf-8").replace("\n", "")
    # Remove ' and " that can occour it is a merge commit
    git_commit_text = git_commit_text.replace('\"', '')
    git_commit_text = git_commit_text.replace('\'', '')
    current_date_time = datetime.now().strftime("%d/%m/%Y, %H:%M:%S")

    build_description = "- git hash: " + git_hash + ", commit: " + \
        git_commit_text + " - upload datetime: " + current_date_time
    config_content = config_content.replace(
        "{{BUILD_DESCRIPTION}}", build_description)
    config_content = config_content.replace(
        "{{SET_LIVE_ON_BRANCH}}", set_live_branch_name)
    tmp_steam_config_foldername = "tmp_steam_config/"
    tmp_steam_config_dir = os.path.abspath(
        os.path.join(tools_path, tmp_steam_config_foldername))

    if os.path.isdir(tmp_steam_config_dir):
        print(f"Deleting tmp config {tmp_steam_config_dir}")
        shutil.rmtree(tmp_steam_config_dir)

    os.mkdir(tmp_steam_config_dir)

    f = open(os.path.abspath(tmp_steam_config_dir +
             "/" + config.vdf_config_name), "w")
    f.write(config_content)
    f.close()

    print(f"Using config:\n {config_content}\n")

    # We also must copy the depot file
    abs_depot_path = os.path.join(
        tools_path, "Steam/steamcmd/" + config.depot_config_name)
    copyfile(abs_depot_path, tmp_steam_config_dir +
             "/" + config.depot_config_name)

    tmp_steam_config_path = "\"" + \
        os.path.abspath(os.path.join(
            tmp_steam_config_dir, config.vdf_config_name)) + "\""

    print("Execute steamcmd on: " + tmp_steam_config_path)
    execute(f"{config.steamcmd_path} +login {steam_username} {steam_password} +run_app_build {tmp_steam_config_path} +quit")

    print("Deleting tmp config")
    shutil.rmtree(tmp_steam_config_dir)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Publish ScreenPlay to Steam')
    parser.add_argument('-steam_username', action="store", dest="steam_username", required=True,
                        help="Steam Username.")
    parser.add_argument('-steam_password', action="store", dest="steam_password", required=True,
                        help="Steam Password.")
    parser.add_argument('-set_live_branch_name', action="store", dest="set_live_branch_name",
                        help="Branch name. Defaults to internal")
    parser.add_argument('-steam_config', action="store", dest="vdf_config_name",
                        help="Name to the .vdf config. This will use the .vdf files from the steamcmd folder.")
    args = parser.parse_args()

    if not args.steam_username or not args.steam_password:
        exit(1)

    if args.set_live_branch_name:
        set_live_branch_name = args.set_live_branch_name
    else:
        set_live_branch_name = "internal"

    publish(
        steam_username=args.steam_username,
        steam_password=args.steam_password,
        set_live_branch_name=set_live_branch_name
    )
