#!/usr/bin/python3
import platform
import os
import subprocess
import shutil
import argparse
import time
from pathlib import Path
from concurrent.futures import ThreadPoolExecutor

qt_version = "6.2.2"
steam_build = "OFF"
build_tests = "OFF"
create_installer = "OFF"

qt_path = ""
cmake_target_triplet = ""
cmake_build_type = ""
executable_file_ending = ""
deploy_command = ""
aqt_path = ""
ifw_root_path = ""
cmake_bin_path = ""

file_endings = [".ninja_deps", ".ninja", ".ninja_log", ".lib", ".a", ".dylib", ".exp",
				".manifest", ".cmake", ".cbp", "CMakeCache.txt"]

vcvars = "" # We support 2019 or 2022

# Based on https://gist.github.com/l2m2/0d3146c53c767841c6ba8c4edbeb4c2c
def get_vs_env_dict():
	# Hardcoded VS path
	# check if vcvars64.bat is available
	msvc_2019_path = "C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Community\\VC\\Auxiliary\\Build\\vcvars64.bat"
	msvc_2022_path = "C:\\Program Files\\Microsoft Visual Studio\\2022\\Community\\VC\\Auxiliary\\Build\\vcvars64.bat"

	if Path(msvc_2019_path).exists():
		vcvars = msvc_2019_path
	elif Path(msvc_2022_path).exists():
		vcvars = msvc_2022_path
	else:
		raise RuntimeError("No Visual Studio installation found, only 2019 and 2022 are supported.")

	print("Loading MSVC env variables via {vcvars}".format(vcvars=vcvars))

	cmd = [vcvars, '&&', 'set']
	popen = subprocess.Popen(
		cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
	stdout, stderr = popen.communicate()

	if popen.wait() != 0:
		raise ValueError(stderr.decode("mbcs"))
	output = stdout.decode("mbcs").split("\r\n")
	return dict((e[0].upper(), e[1]) for e in [p.rstrip().split("=", 1) for p in output] if len(e) == 2)

def run_io_tasks_in_parallel(tasks):
	with ThreadPoolExecutor() as executor:
		running_tasks = [executor.submit(task) for task in tasks]
		for running_task in running_tasks:
			running_task.result()

def clean_build_dir(build_dir):
	if isinstance(build_dir, str):
		build_dir = Path(build_dir)
	if build_dir.exists():
		print(f"Remove previous build folder: {build_dir}")
		shutil.rmtree(build_dir)
	build_dir.mkdir(parents=True, exist_ok=True)

def run(cmd, cwd = Path.cwd()):
	result = subprocess.run(cmd, shell=True, cwd=cwd)
	if result.returncode != 0:
		raise RuntimeError(f"Failed to execute {cmd}")

if __name__ == "__main__":
	parser = argparse.ArgumentParser(description='Build and Package ScreenPlay')
	parser.add_argument('-type', action="store", dest="build_type",
						help="Build type. This is either debug or release.")
	parser.add_argument('-use-aqt', action="store_true", dest="use_aqt",
						help="Absolute qt path. If not set the default path is used\Windows: C:\Qt\nLinux & macOS:~/Qt/.")
	parser.add_argument('-sign', action="store_true", dest="sign_build",
					help="Enable if you want to sign the apps. This is macOS only for now.")
	parser.add_argument('-steam', action="store_true", dest="steam_build",
						help="Enable if you want to build the Steam workshop plugin.")
	parser.add_argument('-tests', action="store_true", dest="build_tests",
                    help="Build tests.")             
	parser.add_argument('-installer', action="store_true", dest="create_installer",
                    help="Create a installer.")
	args = parser.parse_args()

	if not args.build_type:
		print("Build type argument is missing (release, debug). E.g: python build.py -t release -steam=True")
		exit(1)

	root_path = Path.cwd()

	if root_path.name == "Tools":
		root_path = root_path.parent

	if args.use_aqt:
		aqt_path =  Path(("{root_path}/../aqt/").format(root_path=root_path)).resolve()

		if not Path(aqt_path).exists():
			print("aqt path does not exist at %s. Please make sure you have installed aqt." % aqt_path)
			exit(2)

	if platform.system() == "Windows":
		cmake_target_triplet = "x64-windows"
		cmake_build_type = args.build_type
		windows_msvc = "msvc2019_64"
		executable_file_ending = ".exe"
		qt_path = aqt_path.joinpath(qt_version).joinpath(windows_msvc) if args.use_aqt else Path(f"C:/Qt/{qt_version}/{windows_msvc}")
		vs_env_dict = get_vs_env_dict()
		vs_env_dict["PATH"] = vs_env_dict["PATH"] + ";" + str(qt_path) + "\\bin"
		os.environ.update(vs_env_dict)
		deploy_command = "windeployqt.exe --{type}  --qmldir ../../{app}/qml {app}{executable_file_ending}"

	elif platform.system() == "Darwin":
		cmake_target_triplet = "x64-osx"
		qt_path = aqt_path.joinpath(f"{qt_version}/macos") if args.use_aqt else Path(f"~/Qt/{qt_version}/macos")
		deploy_command = "{prefix_path}/bin/macdeployqt {app}.app  -qmldir=../../{app}/qml -executable={app}.app/Contents/MacOS/{app}"

	elif platform.system() == "Linux":
		cmake_target_triplet = "x64-linux"
		qt_path = aqt_path.joinpath(f"{qt_version}/gcc_64") if args.use_aqt else Path(f"~/Qt/{qt_version}/gcc_64")
		if shutil.which("cqtdeployer"):
			deploy_command = "cqtdeployer -qmlDir ../../{app}/qml -bin {app}"
		else:
			print("cqtdeployer not available, build may be incomplete and incompatible with some distro (typically Ubuntu)")
	else:
		raise NotImplementedError("Unsupported platform, ScreenPlay only supports Windows, macOS and Linux.")
		
	# Prepare
	cmake_toolchain_file = ("'{root_path}/../ScreenPlay-vcpkg/scripts/buildsystems/vcpkg.cmake'").format(root_path=root_path)
	print("cmake_toolchain_file: %s " % cmake_toolchain_file)
	print("Starting build with type %s. Qt Version: %s. Root path: %s" % (args.build_type, qt_version, root_path))
	
	if args.steam_build:
		steam_build =  "ON"
	if args.build_tests:
		build_tests =  "ON"
	if args.create_installer:
		create_installer =  "ON"
		ifw_root_path =  ("{aqt_path}\\Tools\\QtInstallerFramework\\4.2").format(aqt_path=aqt_path)

	cmake_configure_command = 'cmake ../ \
-DCMAKE_PREFIX_PATH={prefix_path} \
-DCMAKE_BUILD_TYPE={type} \
-DVCPKG_TARGET_TRIPLET={triplet} \
-DCMAKE_TOOLCHAIN_FILE={toolchain} \
-DSCREENPLAY_STEAM={steam} \
-DSCREENPLAY_TESTS={tests} \
-DSCREENPLAY_CREATE_INSTALLER={installer} \
-DSCREENPLAY_IFW_ROOT:STRING={ifw} \
-G "CodeBlocks - Ninja" \
-B.'.format(
		prefix_path=qt_path,
		type=args.build_type,
		triplet=cmake_target_triplet,
		toolchain=cmake_toolchain_file,
		steam=steam_build,
		tests = build_tests,
		installer= create_installer,
		ifw= ifw_root_path)

	build_folder = root_path.joinpath(f"build-{cmake_target_triplet}-{args.build_type}")
	clean_build_dir(build_folder)

	# Build
	start_time = time.time()
	run(cmake_configure_command, cwd=build_folder)
	run("cmake --build . --target all", cwd=build_folder)

	bin_dir = build_folder.joinpath("bin")

	# Deploy dependencies
	if deploy_command: # Only deploy if we have the dependencies
		print("Executing deploy commands...")
		run(deploy_command.format(
			type=cmake_build_type,
			prefix_path=qt_path,
			app="ScreenPlay",
			executable_file_ending=executable_file_ending), cwd=bin_dir)

		run(deploy_command.format(
			type=cmake_build_type,
			prefix_path=qt_path,
			app="ScreenPlayWidget",
			executable_file_ending=executable_file_ending), cwd=bin_dir)

		run(deploy_command.format(
			type=cmake_build_type,
			prefix_path=qt_path,
			app="ScreenPlayWallpaper",
			executable_file_ending=executable_file_ending), cwd=bin_dir)
	else:
		print("Not executing deploy commands due to missing dependencies.")
	
	# Post-build
	if platform.system() == "Darwin" and args.sign_build:
		print("Remove workshop build folder (macos only).")
		shutil.rmtree(build_folder + "/bin/workshop")

		run("codesign --deep -f -s \"Developer ID Application: Elias Steurer (V887LHYKRH)\" --timestamp --options \"runtime\" -f --entitlements \"../../ScreenPlay/entitlements.plist\"  --deep \"ScreenPlay.app/\"", cwd=bin_dir)
		run("codesign --deep -f -s \"Developer ID Application: Elias Steurer (V887LHYKRH)\" --timestamp --options \"runtime\" -f --deep \"ScreenPlayWallpaper.app/\"", cwd=bin_dir)
		run("codesign --deep -f -s \"Developer ID Application: Elias Steurer (V887LHYKRH)\" --timestamp --options \"runtime\" -f --deep \"ScreenPlayWidget.app/\"", cwd=bin_dir)

		run("codesign --verify --verbose=4  \"ScreenPlay.app/\"", cwd=bin_dir)
		run("codesign --verify --verbose=4  \"ScreenPlayWallpaper.app/\"", cwd=bin_dir)
		run("codesign --verify --verbose=4  \"ScreenPlayWidget.app/\"", cwd=bin_dir)

		run_io_tasks_in_parallel([
			lambda: run("xcnotary notarize ScreenPlay.app -d kelteseth@gmail.com -k ScreenPlay", cwd=bin_dir),
			lambda: run("xcnotary notarize ScreenPlayWallpaper.app -d kelteseth@gmail.com -k ScreenPlay", cwd=bin_dir),
			lambda: run("xcnotary notarize ScreenPlayWidget.app -d kelteseth@gmail.com -k ScreenPlay", cwd=bin_dir)
		])

		run("spctl --assess --verbose  \"ScreenPlay.app/\"", cwd=bin_dir)
		run("spctl --assess --verbose  \"ScreenPlayWallpaper.app/\"", cwd=bin_dir)
		run("spctl --assess --verbose  \"ScreenPlayWidget.app/\"", cwd=bin_dir)

	# Some dlls like openssl do no longer get copied automatically.
	# Lets just copy all of them into bin.
	if platform.system() == "Windows":
		vcpkg_bin_path = Path(("{root_path}/../ScreenPlay-vcpkg/installed/x64-windows/bin").format(root_path=root_path)).resolve()
		print(vcpkg_bin_path)
		for file in vcpkg_bin_path.iterdir():
			if file.suffix == ".dll" and file.is_file():
				print(file, bin_dir)
				shutil.copy2(file, bin_dir)

	for file_ending in file_endings:
		for file in bin_dir.rglob("*" + file_ending):
			if file.is_file():
				print("Remove: %s" % file.resolve())
				file.unlink()

	if args.create_installer:
		os.chdir(build_folder)
		print("Running cpack at: ", os.getcwd())
		print("SKIP INSTALLER UNTIL CMAKE OUT OF SPACE ERROR IS FIXED IN 3.23")
		print("https://cmake.org/cmake/help/git-stage/cpack_gen/ifw.html#variable:CPACK_IFW_ARCHIVE_FORMAT")
		#run("cpack", cwd=build_folder)

	print("Time taken: {}s".format(time.time() - start_time))
	
