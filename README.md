<div>
<img width="100%" height="93" src="https://screen-play.app/images/logo_gitlab_fullwidth.svg">
</div>

<div align="center">
ScreenPlay is an open source cross plattform app for displaying Wallpaper, Widgets and AppDrawer. It is written in modern C++17/Qt5/QML. Binaries with workshop support are available for Windows/Linux/MacOSX via <a href="https://store.steampowered.com/about/">Steam</a>. 
Join our community: <a href="https://screen-play.app/">Homepage</a>, <a href="https://forum.screen-play.app/">Forum</a>
<br> Visit our <a href="https://kelteseth.gitlab.io/ScreenPlayDocs/"> Developer Documentation</a> and our <a href="https://kelteseth.gitlab.io/ScreenPlayDocs/"> Getting Started Guide</a>!
<br>
<h3><a href="https://steamcommunity.com/app/672870/">Download ScreenPlay!</a></h3>

</div>
<br>

## Contributing

Everyone can contribute with code, design, documentation or translation. Visit our [contributing guide](https://gitlab.com/kelteseth/ScreenPlay/blob/dev/CONTRIBUTING.md) for more informations.

# Getting started

### Basic
1. Install latest git + git-lfs
2. Download ScreenPlay with  __all submodules__
``` bash
# HTTPS
git clone --recursive https://gitlab.com/kelteseth/ScreenPlay.git
```
### Windows
1. [Download and install MSVC 2019 Community](https://visualstudio.microsoft.com/vs/community/)
2. [Download and install Win 10 SDK (debugging support. Not included via the MSVC installer)](https://developer.microsoft.com/en-us/windows/downloads/windows-10-sdk)
    - Select debugging support during the installation (CDB)
3. [Download and install Qt 5 binary installer from qt.io](https://www.qt.io/download-qt-installer)
    - Install the Maintaince tool
    - Select the following features to install:
        - Qt 5.13.0
            - MSVC 2017 64-bit
            - Qt WebEngine
        - Tools
            - Qt Creator 4.x CDB Debugger Support
4. Start the vcpkg-install-dependencies.bat for the automatic dependencies management.


### Linux
1. Install dependencies for your distro:
``` bash
# Debian/Ubuntu
sudo apt install build-essential libgl1-mesa-dev

# Fedora/RHEL/CentOS (yum)
sudo yum groupinstall "C Development Tools and Libraries"
sudo yum install mesa-libGL-devel

# openSUSE (zypper)
sudo zypper install -t pattern devel_basis
```
2. [Download and install Qt 5 binary installer from qt.io](https://www.qt.io/download-qt-installer)
    - Install the Maintaince tool
    - Select the following features to install:
        - Qt 5.13.0 
            - GCC
            - Qt WebEngine
### OSX
1. Install XCode via the app store
2. Launch XCode and install the XCode command line tools. (This may take a few minutes)
    - If in doubt or unsure, execute 'xcode-select --install'
3. [Download and install Qt 5 binary installer from qt.io](https://www.qt.io/download-qt-installer)
    - Install the Maintaince tool
    - Select the following features to install:
        - Qt 5.13.0 
            - Qt WebEngine
4. Change your default kit: QtCreator -> Options -> Kits -> Select your default kit (Desktop Qt 5.13.0) -> Change c and c++ Compiler to Apple Clang (x86_64)