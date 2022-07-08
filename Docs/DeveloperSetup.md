# Developer Setup
1. Install latest [git + git-lfs](https://git-scm.com/)
2. Install [python 3](https://www.python.org/)
3. Clone ScreenPlay
``` bash
git clone --recursive https://gitlab.com/kelteseth/ScreenPlay.git ScreenPlay
```
4. Download the latest __Qt 6.3.x__ for you platform. Earlier versions are not supported!
    1. [Install instructions Windows](#windows)
    1. [Install instructions Linux](#linux)
    1. [Install instructions MacOSX](#macosx)
5. Start the following script to download all needed dependencies automatically. This will create a ScreenPlay-vcpkg folder in the same directory as your ScreenPlay source folder. 
``` bash
cd Tools
py setup.py
```
   * This will install these dependencies via __vcpkg__
      * openSSL
      * sentry-native
      * doctest
      * infoware

<div>
<img width="100%" height="auto" src="../.gitlab/media/QtCreator_kit.png">
</div>

5. Open __QtCreator__ and open the settings `Tools -> Options`
6. Clone an existing kit like `Qt 6.3.0 MSVC2019 64bit` and add `ScreenPlay` to the new kit name
5. Edit CMake variables amd add CMAKE_TOOLCHAIN_FILE and VCPKG_TARGET_TRIPLET
    * `Kits -> <Your_Kit> -> CMake Configuration`
    
Append this:
``` bash
-DCMAKE_TOOLCHAIN_FILE:STRING=%{CurrentProject:Path}/../ScreenPlay-vcpkg/scripts/buildsystems/vcpkg.cmake
# Only _one_ of these lines that match your OS:
-DVCPKG_TARGET_TRIPLET:STRING=x64-windows
-DVCPKG_TARGET_TRIPLET:STRING=x64-linux
-DVCPKG_TARGET_TRIPLET:STRING=x64-osx
-DVCPKG_TARGET_TRIPLET:STRING=arm64-osx
```


6. Save and close the settings.
7. Open Project via `File -> Open File or Project` and select your `CMakeLists.txt`. Then select __our created kit__, press `Configure Project`
<div>
<img  height="auto" src="../.gitlab/media/QtCreator_kit_select.png">
</div>
8. Press build (the big green play button on the bottom left). This will compile and start ScreenPlay and copy all necessary files into your Qt installation.

## Windows
1. It is recommended (but not necessary) to use an easy git UI like [gitextensions](https://gitextensions.github.io/).
1. [Download and install the most recent MSVC 2019 Community](https://visualstudio.microsoft.com/vs/community/)
    - Select "Desktop development with C++"
1. [Download and install Qt binary installer from qt.io](https://www.qt.io/download-qt-installer)
    - Install the Maintaince tool
    - Select the following features to install:
        - Qt 6.3.0
            - MSVC 2019 64-bit
            - **ALL Additional Libraries**
            - Qt Quick 3d
            - Qt 5 Compatibility Module
            - Qt Shader Tools
        - Developer and Designer Tools
            - Cmake
            - Ninja
## Linux via qt.io account
1. Install dependencies for your distro:
``` bash
# Debian/Ubuntu
sudo apt install build-essential  git gpg   ffmpeg mesa-common-dev libxkbcommon-* libfontconfig curl zip unzip tar git pkg-config apt-transport-https ca-certificates gnupg software-properties-common wget software-properties-common python3-pip  libgl1-mesa-dev lld ninja-build cmake  qml-module-qt-websockets qtwebengine5-* -y
```
1. [Download and install Qt binary installer from qt.io](https://www.qt.io/download-qt-installer)
    - Install the Maintaince tool
    - Select the following features to install:
        - Qt 6.3.0
            - GCC
            - **ALL Additional Libraries**
            - Qt Quick 3d
            - Qt 5 Compatibility Module
            - Qt Shader Tools
        - Developer and Designer Tools
            - OpenSSL 1.1.1.c Toolkit
                - OpenSSL 64-bit binaries
            - Cmake
            - Ninja
## Linux via aqt command line & VSCode
1. Alternativly download via aqt
```
pip3 install -U pip
pip3 install aqtinstall
aqt install-qt -O ~/aqt linux desktop 6.3.0 gcc_64 -m all
```
1. Open VSCode and install the `CMake Tools`
    1. ctrl + p: `CMake: Select Configure Preset`
    2. Select `ScreenPlay 64bit Debug Linux`
    3. ctrl + p: `CMake: Configure`
    4. ctrl + p: `CMake: Build`
### Fix python scripts not in path:
```
vim ~/.bashrc
```
Add at the end of the file and restart the console/terminal:
```
export PATH="~/.local/bin:$PATH"
```

## MacOSX
1. Install [brew](https://brew.sh) that is needed by some third party vcpkg packages. Do not forget to add brew to your path as outlined at the on of the installation!
    - `brew install pkg-config git llvm cmake ninja`
1. [Download and install Qt binary installer from qt.io](https://www.qt.io/download-qt-installer)
    - Install the Maintaince tool
    - Select the following features to install:
        - Qt 6.3.0
            - Qt WebEngine
            - **ALL Additional Libraries**
            - Qt Quick 3d
            - Qt 5 Compatibility Module
            - Qt Shader Tools
        - Developer and Designer Tools
            - OpenSSL 1.1.1.c Toolkit
                - OpenSSL 64-bit binaries
            - Cmake
            - Ninja
1. Change your default kit: 
    - `QtCreator -> Options -> Kits -> Select your default kit (Desktop Qt 6.3.0) -> Change c and c++ Compiler to Apple Clang (x86_64)`


# Developer docs 
We use qdoc to generate documentation. Qt6 is the minimum version qdoc, because of vastly improved c++ parsing support. 
* qdoc.exe configWindows.qdocconf
If you have installed Qt in a different directory, change the path to your Qt location.

Some useful links:
* [Introduction to QDoc](https://doc.qt.io/qt-5/01-qdoc-manual.html)
* [Writing qdoc comments](https://doc.qt.io/qt-5/qdoc-guide-writing.html)
