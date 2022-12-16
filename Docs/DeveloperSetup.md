# Setup
1. Download and install the tools for your platform __first__.
   1. [Windows](#windows)
   1. [Linux](#linux)
   1. [MacOS](#macos)
2. __Then__ see the instruction of [how to setup QtCreator and compile ScreenPlay](#setup-qtcreator-and-compile-screenplay)

## Windows
1. Download and install:
   1. Microsoft Visual Studio Community 2022 [Download](https://visualstudio.microsoft.com/de/vs/community/)
   2. Python 3.11+ and select `Add to Path` during the installation. [Download](https://www.python.org/downloads/)
   3. Optional if you do not have tools like `git` or `cmake` installed, we recommend `Chocolatey`:
      1. Chocolatey via Powershell (Administrator) [Download](https://chocolatey.org/install)
      2. Run `choco.exe install git cmake -y`

## Linux
1. Install dependencies for Debian/Ubuntu:
``` bash
sudo apt install build-essential git gpg ffmpeg mesa-common-dev libxkbcommon-* libfontconfig curl zip unzip tar cmake pkg-config apt-transport-https ca-certificates gnupg software-properties-common wget software-properties-common python3 python3-pip  libgl1-mesa-dev lld ninja-build qml-module-qt-websockets qtwebengine5-* -y
```

## MacOS
1. Install XCode 14+ , open and restart your device.
1. Install [brew](https://brew.sh) that is needed by some third party vcpkg packages. Do not forget to add brew to your path as outlined at the on of the installation!
    - `brew install pkg-config git llvm cmake`

# Setup QtCreator and compile ScreenPlay
1. Clone ScreenPlay
``` bash
git clone --recursive https://gitlab.com/kelteseth/ScreenPlay.git ScreenPlay/ScreenPlay
```
1. Start the following script to download _all_ needed dependencies automatically. 
``` bash
cd Tools
# Windows defaults to python
python setup.py
# Linux and macOS uses python3
python3 setup.py
```
1. This will:
   1. Download `Qt` and `QtCreator` into:
      1. Windows: `C:\aqt`
      2. MacOS and Linux: `~/aqt`
   2. Create a `vcpkg` folder for dependencies
   3. Compile the dependencies
   4. Download ffmpeg
1. Open QtCreator
      1. Windows: `C:\aqt\Tools\QtCreator\bin\qtcreator.exe`
      2. Linux: `~/aqt/Tools/QtCreator/bin/qtcreator`
      3. MacOS: `~/aqt/qtcreator`
1. Select the imported temporary kit like `ScreenPlay 64bit Debug Windows MSVC 2022 Community`
2. Compile!

# Developer docs 
We use qdoc to generate documentation. Qt6 is the minimum version qdoc, because of vastly improved c++ parsing support. 
* qdoc.exe configWindows.qdocconf
If you have installed Qt in a different directory, change the path to your Qt location.

Some useful links:
* [Introduction to QDoc](https://doc.qt.io/qt-5/01-qdoc-manual.html)
* [Writing qdoc comments](https://doc.qt.io/qt-5/qdoc-guide-writing.html)

