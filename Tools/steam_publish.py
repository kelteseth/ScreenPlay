import os
import sys
import subprocess
import shutil
import argparse
from sys import platform
from execute_util import execute
from datetime import datetime
import subprocess


# Executes steamcmd with username and password. Changes the content of the config
# for better readability in the steam builds tab
# https://partner.steamgames.com/apps/builds/672870

def get_git_revision_short_hash():
    return subprocess.check_output(['git', 'rev-parse', '--short', 'HEAD'])


def get_git_branch_hash():
    return subprocess.check_output(['git', 'branch', '--show-current'])


# MAIN
parser = argparse.ArgumentParser(description='Publish ScreenPlay to Steam')
parser.add_argument('-u', action="store", dest="steam_username", required=True,
                    help="Steam Username.")
parser.add_argument('-p', action="store", dest="steam_password", required=True,
                    help="Steam Password.")
parser.add_argument('-c', action="store", dest="vdf_config_name",
                    help="Name to the .vdf config. This will use the .vdf files from the steamcmd folder.")
args = parser.parse_args()

vdf_config_name = ""
if not args.vdf_config_name:
    if platform == "win32":
        vdf_config_name = "app_build_windows.vdf"
    elif platform == "darwin":
        vdf_config_name = "app_build_mac.vdf"
    elif platform == "linux":
        vdf_config_name = "app_build_linux.vdf"
    print("No vdf config provided, using default: " + vdf_config_name)
else:
    vdf_config_name = args.vdf_config_name

abs_vdf_path = os.path.abspath("steamcmd/" + vdf_config_name)

if not os.path.isfile(abs_vdf_path):
    print("Incorrect vdf name")
    sys.exit(-1)

file = open(abs_vdf_path, "r")

config_content = file.read()
git_hash = get_git_revision_short_hash().decode("utf-8").replace("\n", "")
git_branch = get_git_branch_hash().decode("utf-8").replace("\n", "")
current_date_time = datetime.now().strftime("%m/%d/%Y, %H:%M:%S")

build_description = "- git hash: " + git_hash + ", branch: " +  git_branch + " - upload datetime: " + current_date_time
config_content = config_content.replace("{{BUILD_DESCRIPTION}}", build_description)
tmp_steam_config_foldername = "tmp_steam_config/"
tmp_steam_config_dir = os.path.abspath(tmp_steam_config_foldername)

if os.path.isdir(tmp_steam_config_dir):
    shutil.rmtree(tmp_steam_config_dir)

os.mkdir(tmp_steam_config_dir)

f = open(os.path.abspath(tmp_steam_config_dir + "/" + vdf_config_name), "w")
f.write(config_content)
f.close()

tmp_steam_config_path = "\"" + os.path.abspath(tmp_steam_config_foldername + vdf_config_name) +  "\"" 

print("Execute steamcmd on: " + tmp_steam_config_path)
execute("steamcmd +login {username} {password} +run_app_build {config} +quit".format(username=steam_username, password=steam_password, config=tmp_steam_config_path))

print("Deleting tmp config")
shutil.rmtree(tmp_steam_config_dir)
