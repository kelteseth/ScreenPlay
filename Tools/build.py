from sys import platform
import os
import sys
import subprocess
import shutil
import argparse

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



# MAIN
parser = argparse.ArgumentParser(description='Build and Package ScreenPlay')
parser.add_argument('-t', action="store", dest="build_type")
args = parser.parse_args()

if not args.build_type:
    print("Build type argument is missing (release,debug). Example: python build.py -t release")
    sys.exit(1)

qt_version = "5.15.1"
print("Starting build with type %s. Qt Version: %s" %
      (args.build_type, qt_version))


cmake_prefix_path = ""
cmake_target_triplet = ""
executable_file_ending = ""
deploy_executable = ""

if platform == "win32":
    print("Loading MSVC env variables via vsvars32.bat")
    windows_msvc =  "msvc2019_64"
    executable_file_ending = ".exe"
    deploy_executable = "windeployqt.exe"
    dict = vs_env_dict()
    dict["PATH"] = dict["PATH"] + ";C:\\Qt\\" + qt_version + "\\" + windows_msvc + "\\bin;C:\\Qt\\Tools\\QtCreator\\bin"
    os.environ.update(dict)
    cmake_prefix_path = "c:/Qt/" + qt_version + "/" + windows_msvc
    cmake_target_triplet = "x64-windows"
    os.system("install_dependencies_windows.bat")
elif platform == "darwin":
    cmake_prefix_path = "~/Qt/" + qt_version + "/clang_64"
    deploy_executable = "macdeployqt"
    cmake_target_triplet = "x64-osx"
    print("Executing install_dependencies_linux_mac.sh")
    os.system("chmod +x install_dependencies_linux_mac.sh")
    os.system("./install_dependencies_linux_mac.sh")
elif platform == "linux":
    deploy_executable = "linuxdeployqt"
    cmake_prefix_path = "~/Qt/"
    cmake_target_triplet = "x64-linux"
    print("Executing install_dependencies_linux_mac.sh")
    os.system("chmod +x install_dependencies_linux_mac.sh")
    os.system("./install_dependencies_linux_mac.sh")

# REMOVE OLD BUILD FOLDER
cwd = os.getcwd()
root_path = os.path.abspath((cwd+"/../"))
os.chdir(root_path)
cmake_toolchain_file = ("'{root_path}/../ScreenPlay-vcpkg/scripts/buildsystems/vcpkg.cmake'").format(root_path=root_path)
print("cmake_toolchain_file: %s " % cmake_toolchain_file)

build_folder = "build-" + cmake_target_triplet + "-" + args.build_type

if os.path.isdir(build_folder):
    print("Remove previous build folder")
    shutil.rmtree(build_folder)


os.mkdir(build_folder)
os.chdir(root_path + "/" + build_folder)

cmake_configure_command = """cmake ../
 -DCMAKE_PREFIX_PATH={prefix_path}
 -DCMAKE_BUILD_TYPE={type}
 -DCMAKE_TOOLCHAIN_FILE={toolchain}
 -DVCPKG_TARGET_TRIPLET={triplet}
 -G "CodeBlocks - Ninja"
 -B.
  """.format(
    type=args.build_type,
    prefix_path=cmake_prefix_path,
    triplet=cmake_target_triplet,
    toolchain=cmake_toolchain_file).replace("\n", "")

print("cmake_configure_command: %s" % cmake_configure_command)

process = subprocess.run(cmake_configure_command,  capture_output=True)

if process.returncode != 0:
    sys.exit(process.returncode)

os.system("cmake --build . --target all")
os.chdir("bin")

os.system(("{deploy_executable} --{type}  --qmldir ../../ScreenPlay/qml ScreenPlay{executable_file_ending}").format(
  type=args.build_type,
  executable_file_ending=executable_file_ending,
  deploy_executable=deploy_executable))

os.system(("{deploy_executable} --{type}  --qmldir ../../ScreenPlayWidget ScreenPlayWidget{executable_file_ending}").format(  
  type=args.build_type,
  executable_file_ending=executable_file_ending,
  deploy_executable=deploy_executable))

os.system(("{deploy_executable} --{type}  --qmldir ../../ScreenPlayWallpaper ScreenPlayWallpaper{executable_file_ending}").format(  
  type=args.build_type,
  executable_file_ending=executable_file_ending,
  deploy_executable=deploy_executable))


file_endings = [".ninja_deps", ".ninja", ".ninja_log", ".lib", ".exp",
                ".manifest", ".cmake", ".cbp", "CMakeCache.txt"]

for filename in os.listdir(os.getcwd()):
    for ending in file_endings:
        if filename.endswith(ending):
            full_file_path = os.path.join(os.getcwd(), filename)
            print("Remove: %s" % full_file_path)
            os.remove(full_file_path)
