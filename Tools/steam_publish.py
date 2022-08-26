import os
import sys
import subprocess
import shutil
import argparse
from sys import platform
from execute_util import execute
from datetime import datetime
from shutil import copyfile
import subprocess
from util import cd_repo_root_path
import platform

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
    contentBuiler_path = os.path.join(tools_path, "Steam/ContentBuilder/")

    vdf_config_name = ""
    depot_config_name = ""
    steamcmd_path = ""
    if platform.system() == "Windows":
        vdf_config_name = "app_build_windows.vdf"
        depot_config_name = "depot_build_windows.vdf"
        steamcmd_path = os.path.join(contentBuiler_path, "builder/steamcmd.exe")
        steamcmd_path = steamcmd_path.replace("/","\\")
    elif platform.system() == "Darwin":
        vdf_config_name = "app_build_mac.vdf"
        depot_config_name = "depot_build_mac.vdf"
        steamcmd_path = os.path.join(contentBuiler_path, "builder_osx/steamcmd")
    elif platform.system() == "Linux":
        vdf_config_name = "app_build_linux.vdf"
        depot_config_name = "depot_build_linux.vdf"
        steamcmd_path = os.path.join(contentBuiler_path, "builder_linux/steamcmd.sh")

    print(f"Set steamCmd path: {steamcmd_path}")

    abs_vdf_path = os.path.join(tools_path,"Steam/steamcmd/" + vdf_config_name)

    if not os.path.isfile(abs_vdf_path):
        print("Incorrect vdf name")
        sys.exit(-1)

    file = open(abs_vdf_path, "r")

    config_content = file.read()
    git_hash = get_git_revision_short_hash().decode("utf-8").replace("\n", "")
    git_commit_text = get_git_commit_text().decode("utf-8").replace("\n", "")
    current_date_time = datetime.now().strftime("%m/%d/%Y, %H:%M:%S")

    build_description = "- git hash: " + git_hash + ", commit: " +  git_commit_text + " - upload datetime: " + current_date_time
    config_content = config_content.replace("{{BUILD_DESCRIPTION}}", build_description)
    config_content = config_content.replace("{{SET_LIVE_ON_BRANCH}}", set_live_branch_name)
    tmp_steam_config_foldername = "tmp_steam_config/"
    tmp_steam_config_dir = os.path.abspath(os.path.join(tools_path,tmp_steam_config_foldername))

    if os.path.isdir(tmp_steam_config_dir):
        shutil.rmtree(tmp_steam_config_dir)

    os.mkdir(tmp_steam_config_dir)

    f = open(os.path.abspath(tmp_steam_config_dir + "/" + vdf_config_name), "w")
    f.write(config_content)
    f.close()

    print(f"Using config:\n {config_content}\n")

    # We also must copy the depot file
    abs_depot_path = os.path.join(tools_path, "Steam/steamcmd/" + depot_config_name)
    copyfile(abs_depot_path, tmp_steam_config_dir + "/" + depot_config_name)

    tmp_steam_config_path = "\"" + os.path.abspath(os.path.join(tmp_steam_config_dir,vdf_config_name) ) +  "\"" 

    print("Execute steamcmd on: " + tmp_steam_config_path)
    execute(f"{steamcmd_path} +login {steam_username} {steam_password} +run_app_build {tmp_steam_config_path} +quit")

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