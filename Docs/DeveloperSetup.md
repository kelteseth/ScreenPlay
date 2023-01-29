# Setup
1. Download and install the tools for your platform __first__.
   1. [Windows](#windows)
   1. [Linux](#linux)
   1. [MacOS](#macos)
2. __Then__ see the instruction of [how to setup QtCreator and compile ScreenPlay](#setup-qtcreator-and-compile-screenplay)

## Windows
1. Download and install:
   1. Microsoft Visual Studio Community 2022 [Download](https://visualstudio.microsoft.com/de/vs/community/)
      1. Make sure you have the exact Windows SDK and MSVC version installed we use in `CMakePresets.json`
         1. `"VCToolsVersion": "14.34.31933"` aka Visual Studio `17.4.3`
         2. `"WindowsSDKVersion" : "10.0.22621.0"`  
   2. Python 3.11+ and select `Add to Path` during the installation. [Download](https://www.python.org/downloads/)
   3. Optional if you do not have tools like `git` or `cmake` installed, we recommend `Chocolatey`:
      1. Chocolatey via Powershell (Administrator) [Download](https://chocolatey.org/install)
      2. Run `choco.exe install git vscode cmake --installargs 'ADD_CMAKE_TO_PATH=System' -y`

## Linux
1. Install dependencies for Debian/Ubuntu:
``` bash
sudo apt install build-essential git gpg ffmpeg mesa-common-dev libxkbcommon-* libfontconfig curl zip unzip tar cmake pkg-config apt-transport-https ca-certificates gnupg software-properties-common wget software-properties-common python3 python3-pip  libgl1-mesa-dev lld ninja-build qml-module-qt-websockets qtwebengine5-* -y
# Only needed if we want x11 support
sudo apt-get install libx11-dev xserver-xorg-dev xorg-dev
```

## MacOS
1. Install XCode 14+ , open and restart your device.
1. Install [brew](https://brew.sh) that is needed by some third party vcpkg packages. Do not forget to add brew to your path as outlined at the on of the installation!
    - `brew install pkg-config git llvm cmake`

# Download ScreenPlay and dependencies
``` bash
git clone --recursive https://gitlab.com/kelteseth/ScreenPlay.git ScreenPlay/ScreenPlay
cd ScreenPlay
```
Downloading dependencies is 100% automated. Simply run the `setup.py` script
``` bash
cd Tools
# Windows defaults to python, linux and macOS uses python3
python -m pip install -r requirements.txt
# This scrit will:
# 1. Downloads `Qt` and `QtCreator`
# 2. Create a `vcpkg` folder for dependencies
# 3. Compiles the dependencies
# 4. Downloads ffmpeg
python setup.py
```
Now you can either use VSCode or QtCreator:
# Option 1: Setup VSCode and compile ScreenPlay
1. Open VSCode
1. Install these extentions:
   1. [C/C++ for Visual Studio Code](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools)
   1. [CMake Tools](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools)
2. Press: `CTRL` + `SHIFT` + `P` for every command:
   1. `CMake: Select Configure Preset`. Select one of the listed presets like `MSVC SP Qt 6.4.2 Debug`
   2. `CMake: Configure`
3. Press `F7` to Compile and `F5` to run!
# Option 2: Setup QtCreator and compile ScreenPlay
1. Open QtCreator at:
      1. Windows: `..\aqt\Tools\QtCreator\bin\qtcreator.exe`
      2. Linux: `../aqt/Tools/QtCreator/bin/qtcreator`
      3. MacOS: `../aqt/Qt Creator`
2. Select the imported temporary kit like `MSVC SP Qt 6.4.2 Debug`
3. Press `CTRL` + `B` to Compile and `F5` to run!

# Developer docs 
We use qdoc to generate documentation. Qt6 is the minimum version qdoc, because of vastly improved c++ parsing support. 
* qdoc.exe configWindows.qdocconf
If you have installed Qt in a different directory, change the path to your Qt location.

Some useful links:
* [Introduction to QDoc](https://doc.qt.io/qt-5/01-qdoc-manual.html)
* [Writing qdoc comments](https://doc.qt.io/qt-5/qdoc-guide-writing.html)

