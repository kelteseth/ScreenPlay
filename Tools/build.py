from sys import platform
from concurrent.futures import ThreadPoolExecutor
import os
import sys
import subprocess
import shutil
import argparse
from execute_util import execute

# Based on https://gist.github.com/l2m2/0d3146c53c767841c6ba8c4edbeb4c2c


def vs_env_dict():
    vsvar32 = 'C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Community\\VC\\Auxiliary\\Build\\vcvars64.bat'
    cmd = [vsvar32, '&&', 'set']
    popen = subprocess.Popen(
        cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    stdout, stderr = popen.communicate()

    if popen.wait() != 0:
        raise ValueError(stderr.decode("mbcs"))
    output = stdout.decode("mbcs").split("\r\n")
    return dict((e[0].upper(), e[1]) for e in [p.rstrip().split("=", 1) for p in output] if len(e) == 2)

# Based on  https://stackoverflow.com/questions/7207309/how-to-run-functions-in-parallel
def run_io_tasks_in_parallel(tasks):
    with ThreadPoolExecutor() as executor:
        running_tasks = [executor.submit(task) for task in tasks]
        for running_task in running_tasks:
            running_task.result()



# MAIN
parser = argparse.ArgumentParser(description='Build and Package ScreenPlay')
parser.add_argument('-t', action="store", dest="build_type",
                    help="Build type. This is either debug or release.")
parser.add_argument('-s', action="store", dest="sign_build",
                    help="Enable if you want to sign the apps. This is macos only for now.")
args = parser.parse_args()

if not args.build_type:
    print("Build type argument is missing (release,debug). Example: python build.py -t release -s=True")
    sys.exit(1)

qt_version = "6.2.0"
print("Starting build with type %s. Qt Version: %s" %
      (args.build_type, qt_version))


cmake_prefix_path = ""
cmake_target_triplet = ""
cmake_build_type = ""
executable_file_ending = ""
deploy_command = ""


if platform == "win32":
    print("Loading MSVC env variables via vsvars32.bat")
    cmake_build_type = args.build_type
    windows_msvc = "msvc2019_64"
    executable_file_ending = ".exe"
    deploy_command = "windeployqt.exe --{type}  --qmldir ../../{app}/qml {app}{executable_file_ending}"
    dict = vs_env_dict()
    dict["PATH"] = dict["PATH"] + ";C:\\Qt\\" + qt_version + \
        "\\" + windows_msvc + "\\bin;C:\\Qt\\Tools\\QtCreator\\bin"
    os.environ.update(dict)
    cmake_prefix_path = "c:/Qt/" + qt_version + "/" + windows_msvc
    cmake_target_triplet = "x64-windows"
elif platform == "darwin":
    cmake_prefix_path = "~/Qt/" + qt_version + "/clang_64"
    deploy_command = "{prefix_path}/bin/macdeployqt {app}.app  -qmldir=../../{app}/qml -executable={app}.app/Contents/MacOS/{app}"
    cmake_target_triplet = "x64-osx"
elif platform == "linux":
    deploy_command = "cqtdeployer -qmldir ../../{app}/qml -bin {app}"
    cmake_prefix_path = "~/Qt/" + qt_version + "/gcc_64"
    cmake_target_triplet = "x64-linux"

# REMOVE OLD BUILD FOLDER
cwd = os.getcwd()
root_path = os.path.abspath((cwd+"/../"))
os.chdir(root_path)
cmake_toolchain_file = (
    "'{root_path}/../ScreenPlay-vcpkg/scripts/buildsystems/vcpkg.cmake'").format(root_path=root_path)
print("cmake_toolchain_file: %s " % cmake_toolchain_file)

build_folder = root_path + "/build-" + cmake_target_triplet + "-" + args.build_type

if os.path.isdir(build_folder):
    print("Remove previous build folder: " + build_folder)
    shutil.rmtree(build_folder)


os.mkdir(build_folder)
os.chdir(build_folder)

cmake_configure_command = """cmake ../
 -DCMAKE_PREFIX_PATH={prefix_path}
 -DCMAKE_BUILD_TYPE={type}
 -DCMAKE_TOOLCHAIN_FILE={toolchain}
 -DVCPKG_TARGET_TRIPLET={triplet}
 -DTESTS_ENABLED=OFF
 -DSCREENPLAY_STEAM_DEPLOY=ON
 -DSCREENPLAY_STEAM=ON
 -G "CodeBlocks - Ninja"
 -B.
  """.format(
    type=args.build_type,
    prefix_path=cmake_prefix_path,
    triplet=cmake_target_triplet,
    toolchain=cmake_toolchain_file).replace("\n", "")

execute(cmake_configure_command)
execute("cmake --build . --target all")
os.chdir("bin")

execute(deploy_command.format(
    type=cmake_build_type,
    prefix_path=cmake_prefix_path,
    app="ScreenPlay",
    executable_file_ending=executable_file_ending))

execute(deploy_command.format(
    type=cmake_build_type,
    prefix_path=cmake_prefix_path,
    app="ScreenPlayWidget",
    executable_file_ending=executable_file_ending))

execute(deploy_command.format(
    type=cmake_build_type,
    prefix_path=cmake_prefix_path,
    app="ScreenPlayWallpaper",
    executable_file_ending=executable_file_ending))

if platform == "darwin" and args.sign_build:
    print("Remove workshop build folder (macos only).")
    shutil.rmtree(build_folder + "/bin/workshop")

    execute("codesign --deep -f -s \"Developer ID Application: Elias Steurer (V887LHYKRH)\" --timestamp --options \"runtime\" -f --entitlements \"../../ScreenPlay/entitlements.plist\"  --deep \"ScreenPlay.app/\"")
    execute("codesign --deep -f -s \"Developer ID Application: Elias Steurer (V887LHYKRH)\" --timestamp --options \"runtime\" -f --deep \"ScreenPlayWallpaper.app/\"")
    execute("codesign --deep -f -s \"Developer ID Application: Elias Steurer (V887LHYKRH)\" --timestamp --options \"runtime\" -f --deep \"ScreenPlayWidget.app/\"")

    execute("codesign --verify --verbose=4  \"ScreenPlay.app/\"")
    execute("codesign --verify --verbose=4  \"ScreenPlayWallpaper.app/\"")
    execute("codesign --verify --verbose=4  \"ScreenPlayWidget.app/\"")

    run_io_tasks_in_parallel([
         lambda: execute("xcnotary notarize ScreenPlay.app -d kelteseth@gmail.com -k ScreenPlay"),
         lambda: execute("xcnotary notarize ScreenPlayWallpaper.app -d kelteseth@gmail.com -k ScreenPlay"),
         lambda: execute("xcnotary notarize ScreenPlayWidget.app -d kelteseth@gmail.com -k ScreenPlay")
    ])

    execute("spctl --assess --verbose  \"ScreenPlay.app/\"")
    execute("spctl --assess --verbose  \"ScreenPlayWallpaper.app/\"")
    execute("spctl --assess --verbose  \"ScreenPlayWidget.app/\"")


file_endings = [".ninja_deps", ".ninja", ".ninja_log", ".lib", ".a", ".dylib", ".exp",
                ".manifest", ".cmake", ".cbp", "CMakeCache.txt"]

for filename in os.listdir(os.getcwd()):
    for ending in file_endings:
        if filename.endswith(ending):
            full_file_path = os.path.join(os.getcwd(), filename)
            print("Remove: %s" % full_file_path)
            os.remove(full_file_path)
