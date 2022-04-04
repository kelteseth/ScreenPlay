#!/usr/bin/python3
import platform
import os
import subprocess
import shutil
import argparse
import time
from pathlib import Path
from concurrent.futures import ThreadPoolExecutor


# Based on https://gist.github.com/l2m2/0d3146c53c767841c6ba8c4edbeb4c2c
def get_vs_env_dict():
	vcvars = "" # We support 2019 or 2022

	# Hardcoded VS path
	# check if vcvars64.bat is available.
	msvc_2019_path = "C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Community\\VC\\Auxiliary\\Build\\vcvars64.bat"
	msvc_2022_path = "C:\\Program Files\\Microsoft Visual Studio\\2022\\Community\\VC\\Auxiliary\\Build\\vcvars64.bat"

	if Path(msvc_2019_path).exists():
		vcvars = msvc_2019_path
	# Prefer newer MSVC and override if exists
	if Path(msvc_2022_path).exists():
		vcvars = msvc_2022_path
	else:
		raise RuntimeError("No Visual Studio installation found, only 2019 and 2022 are supported.")

	print(f"\n\nLoading MSVC env variables via {vcvars}\n\n")

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
	parser.add_argument('-steam', action="store_true", dest="build_steam",
						help="Enable if you want to build the Steam workshop plugin.")
	parser.add_argument('-tests', action="store_true", dest="build_tests",
                    help="Build tests.")             
	parser.add_argument('-installer', action="store_true", dest="build_installer",
                    help="Create a installer.")
	args = parser.parse_args()

	if not args.build_type:
		print("Build type argument is missing (release, debug). E.g: python build.py -type release -steam")
		exit(1)

	root_path = Path.cwd()
	qt_version = "6.3.0"
	qt_path = ""
	build_steam = "OFF"
	build_tests = "OFF"
	build_installer = "OFF"
	cmake_target_triplet = ""
	cmake_build_type = ""
	executable_file_ending = ""
	deploy_command = ""
	aqt_path = ""
	cmake_bin_path = ""

	remove_files_from_build_folder = [
		".ninja_deps",
		".ninja", 
		".ninja_log",
		".lib", 
		".a", 
		".dylib",
		".exp",
		".manifest", 
		".cmake",
		".cbp", 
		"CMakeCache.txt", 
		"steam_appid.txt" # This file is only needed for testing. It must not be in a release version!
		]

	if root_path.name == "Tools":
		root_path = root_path.parent

	if args.use_aqt:
		aqt_path =  Path(f"{root_path}/../aqt/").resolve()

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
		home_path = str(Path.home())
		qt_path = aqt_path.joinpath(f"{qt_version}/gcc_64") if args.use_aqt else Path(f"{home_path}/Qt/{qt_version}/gcc_64")
	else:
		raise NotImplementedError("Unsupported platform, ScreenPlay only supports Windows, macOS and Linux.")
		
	# Prepare
	cmake_toolchain_file = f"'{root_path}/../ScreenPlay-vcpkg/scripts/buildsystems/vcpkg.cmake'"
	print("cmake_toolchain_file: %s " % cmake_toolchain_file)
	print("Starting build with type %s. Qt Version: %s. Root path: %s" % (args.build_type, qt_version, root_path))
	
	if args.build_steam:
		build_steam =  "OFF"
	if args.build_tests:
		build_tests =  "OFF"
	if args.build_installer:
		build_installer =  "OFF"

	cmake_configure_command = f'cmake ../ \
-DCMAKE_PREFIX_PATH={qt_path} \
-DCMAKE_BUILD_TYPE={args.build_type} \
-DVCPKG_TARGET_TRIPLET={cmake_target_triplet} \
-DCMAKE_TOOLCHAIN_FILE={cmake_toolchain_file} \
-DSCREENPLAY_STEAM={build_steam} \
-DSCREENPLAY_TESTS={build_tests} \
-DSCREENPLAY_INSTALLER={build_installer} \
-G "CodeBlocks - Ninja" \
-B.'

	build_folder = root_path.joinpath(f"build-{cmake_target_triplet}-{args.build_type}")
	clean_build_dir(build_folder)

	# Build
	start_time = time.time()
	print(cmake_configure_command)
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
		# just copy the folders and be done with it
		if platform.system() == "Linux":
			# Copy  all .so files from the qt_path lib folder into bin_dir
			qt_lib_path = qt_path
			for file in qt_lib_path.joinpath("lib").glob("*.so"):
				shutil.copy(str(file), str(bin_dir))
			
			# Copy qt_qml_path folder content into bin_dir
			qt_qml_path = qt_path
			for folder in qt_qml_path.joinpath("qml").iterdir():
				if not folder.is_file():
					shutil.copytree(str(folder), str(bin_dir.joinpath(folder.name)))
					print("Copied %s" % folder)


			# Copy all plugin folder from qt_path plugins subfolder into bin_dir
			qt_plugins_path = qt_path
			for folder in qt_path.joinpath("plugins").iterdir():
				if not folder.is_file():
					shutil.copytree(str(folder), str(bin_dir.joinpath(folder.name)))
					print("Copied %s" % folder)

			# Copy all folder from qt_path translation files into bin_dir translation folder
			qt_translations_path = qt_path
			for folder in qt_translations_path.joinpath("translations").iterdir():
				if not folder.is_file():
					shutil.copytree(str(folder), str(bin_dir.joinpath("translations").joinpath(folder.name)))
					print("Copied %s" % folder)

			# Copy all  filesfrom qt_path resources folder into bin_dir folder
			qt_resources_path = qt_path
			for file in qt_path.joinpath("resources").glob("*"):
				shutil.copy(str(file), str(bin_dir))
				print("Copied %s" % file)

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
		vcpkg_bin_path = Path(f"{root_path}/../ScreenPlay-vcpkg/installed/x64-windows/bin").resolve()
		print(vcpkg_bin_path)
		for file in vcpkg_bin_path.iterdir():
			if file.suffix == ".dll" and file.is_file():
				print(file, bin_dir)
				shutil.copy2(file, bin_dir)

	for file_ending in remove_files_from_build_folder:
		for file in bin_dir.rglob("*" + file_ending):
			if file.is_file():
				print("Remove: %s" % file.resolve())
				file.unlink()

	if args.build_installer:
		os.chdir(build_folder)
		print("Running cpack at: ", os.getcwd())
		run("cpack", cwd=build_folder)

	print("Time taken: {}s".format(time.time() - start_time))
	
