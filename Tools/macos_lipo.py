#!/usr/bin/python3
from distutils.dir_util import mkpath
import os
from pathlib import Path
from util import run, run_and_capture_output, cd_repo_root_path

def listfiles(path):
    files = []
    extensions = ('.dylib', '.so','')
    ignored = ('qmldir')
    print(f"WALK: {path}")
    for dirName, subdirList, fileList in os.walk(path):
        dir = dirName.replace(path, '')
        for fname in fileList:
            if Path(fname).suffix in extensions and not fname in ignored:
                file =  path + os.path.join(dir, fname)
                if(os.path.isfile(file)):
                    files.append(file)
                    if  os.path.islink(file):
                        print(f"Warning: file {file} is a symlink!")
                        print("Symlink target: ", os.readlink(file))
    return files

# Merges x64 and arm64 build into universal
def run_lipo() :
    root_path = cd_repo_root_path()

    # Looks like it is ok the contain symlinks otherwise we get these errors for qml plugins:
    # bundle format is ambiguous (could be app or framework)
    # https://bugreports.qt.io/browse/QTBUG-101338
    run("cp -a build-x64-osx-release build-universal-osx-release",root_path)

    apps = ["ScreenPlay","ScreenPlayWallpaper", "ScreenPlayWidget"]
    for app in apps:
        arm64_dir = str(Path.joinpath(root_path, f"build-arm64-osx-release/bin/{app}.app/Contents/MacOS/{app}")) 
        x64_dir = str(Path.joinpath(root_path,   f"build-x64-osx-release/bin/{app}.app/Contents/MacOS/{app}")) 
        universal_dir = str(Path.joinpath(root_path,   f"build-universal-osx-release/bin/{app}.app/Contents/MacOS/{app}")) 
        run(f"lipo -create {arm64_dir} {x64_dir} -output {universal_dir}")
        run(f"lipo -info {universal_dir}")
       
def check_fat_binary():
    # Make sure the script is always started from the same folder
    root_path = Path.cwd()
    if root_path.name == "Tools":
        root_path = root_path.parent
    print(f"Change root directory to: {root_path}")
    os.chdir(root_path)

    dir = 'build-universal-osx-release/bin/'
    files = listfiles(str(Path.joinpath(root_path, dir)))

    for file in files:
        out = run_and_capture_output(f"lipo -info {file}")
        if out.startswith('Non-fat'):
            print(out)

if __name__ == "__main__":
    run_lipo()
    check_fat_binary()
