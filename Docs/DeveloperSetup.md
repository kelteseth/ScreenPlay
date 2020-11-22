# Developer Setup
1. Install latest [git + git-lfs](https://git-scm.com/)
2. Clone ScreenPlay
``` bash
git clone --recursive https://gitlab.com/kelteseth/ScreenPlay.git ScreenPlay
```
3. Download the latest [__Qt 5.15__](https://www.qt.io/download-qt-installer). Earlier versions are not supported!
4. Start install-dependencies.bat to download dependencies. ATTENTION: This will create a ScreenPlay-vcpkg folder in the same directory as your ScreenPlay source folder. 
``` bash
//Windows
.\Tools\install-dependencies.bat

//Linux
sudo apt install git gcc cmake build-essential libgl1-mesa-dev
chmod +x install-dependencies.sh
.\Tools\install-dependencies.sh
```
   * This will install these dependencies via __vcpkg__
      * openSSL 1.1.1d
      * sentry-native
   * Download these dependencies via __git submodules__
      * qt-google-analytics
   * Download ffmpeg binaries
5. **Follow the steps below for your OS**. 
6. Open the CMakeLists.txt via QtCreator. **This can take some time until QtCreator parses all files!**

7. Add CMake variables
    * Add CMAKE_TOOLCHAIN_FILE and VCPKG_TARGET_TRIPLET
       * Extras -> Tools -> Kits -> <Your Kit> -> CMake Configuration -> Append this:
       * CMAKE_TOOLCHAIN_FILE:STRING=%{CurrentProject:Path}/../ScreenPlay-vcpkg/vcpkg/scripts/buildsystems/vcpkg.cmake
       * VCPKG_TARGET_TRIPLET:STRING=x64-windows
       * or  Linux: x64-linux MacOSX: x64-osx

<div>
<img width="100%" height="auto" src="../.gitlab/media/QtCreator_kit.png">
</div>

8. Check if Ninja is selected
    * Extras -> Tools -> Kits -> <Your Kit> -> CMakeGenerator -> Change to:
        * Generator: Ninja
        * Extra Generator: CodeBlocks
9. Save and close the settings.
10. Press build (the big green play button). This will compile the project and copy all necessary files into your Qt installation.

### Windows
1. [Download and install the most recent MSVC 2019 Community](https://visualstudio.microsoft.com/vs/community/)
    - Select "Desktop development with C++"
2. [Download and install Qt 5 binary installer from qt.io](https://www.qt.io/download-qt-installer)
    - Install the Maintaince tool
    - Select the following features to install:
        - Qt 5.15.1
            - MSVC 2019 64-bit
            - Qt WebEngine
        - Developer and Designer Tools
            - Cmake
            - Ninja

### Linux
1. Install dependencies for your distro:
``` bash
# Debian/Ubuntu
sudo apt install build-essential libgl1-mesa-dev lld ninja-build cmake

# Fedora/RHEL/CentOS (yum)
sudo yum groupinstall "C Development Tools and Libraries"
sudo yum install mesa-libGL-devel

# openSUSE (zypper)
sudo zypper install -t pattern devel_basis
```
2. [Download and install Qt 5 binary installer from qt.io](https://www.qt.io/download-qt-installer)
    - Install the Maintaince tool
    - Select the following features to install:
        - Qt 5.15.1
            - GCC
            - Qt WebEngine
### OSX
1. [Download and install Qt 5 binary installer from qt.io](https://www.qt.io/download-qt-installer)
    - Install the Maintaince tool
    - Select the following features to install:
        - Qt 5.15.1
            - Qt WebEngine
        - Developer and Designer Tools
            - OpenSSL 1.1.1.c Toolkit
                - OpenSSL 64-bit binaries
            - Cmake
            - Ninja
2. Install [homebrew](https://brew.sh/)
     - Open a terminal and install clang -> brew install llvm cmake ninja
3. Change your default kit: QtCreator -> Options -> Kits -> Select your default kit (Desktop Qt 5.13.0) -> Change c and c++ Compiler to Apple Clang (x86_64)


# Developer docs 
We use qdoc to generate documentation. Qt6 is the minimum version qdoc, because of vastly improved c++ parsing support. 
* qdoc.exe configWindows.qdocconf
If you have installed Qt in a different directory, change the path to your Qt location.

Some useful links:
* [Introduction to QDoc](https://doc.qt.io/qt-5/01-qdoc-manual.html)
* [Writing qdoc comments](https://doc.qt.io/qt-5/qdoc-guide-writing.html)