<div>
<img width="100%" height="93" src=".gitlab/media/logo_gitlab_fullwidth.svg">
</div>

<div align="center">
ScreenPlay is an open source cross plattform app for displaying Wallpaper, Widgets and AppDrawer. It is written in modern C++17/Qt5/QML. Binaries with workshop support are available for Windows and soon Linux & MacOSX via <a href="https://store.steampowered.com/about/">Steam</a>.  Join our community: 
<br> <h4><a href="https://kelteseth.gitlab.io/ScreenPlayDeveloperDocs/"> Developer C++ Classes Documentation</a>  and  
 <a href="https://kelteseth.gitlab.io/ScreenPlayDocs/">Wallpaper And Widgets Guide</a></h4>
<h3><a href="https://screen-play.app/">Homepage</a> - <a href="https://forum.screen-play.app/">Forum</a> - <a href="https://discord.gg/3RygPHZ">🎉 Discord (Free Steam Key Here!)</a> - <a href="https://www.reddit.com/r/ScreenPlayApp/">Reddit</a></h3>
<!--<h4><a href="https://steamcommunity.com/app/672870/">Download ScreenPlay!</a></h4>-->
<br>

![Preview](.gitlab/media/preview.mp4)
</div>

# Platform support

* ❌ Not working/Not implemented
* ❓ Only partical implemented/Not tested

<div align="center">

| Feature                	    | Windows 	    | Linux 	        | MacOS 	|
|------------------------	    |---------	    |-------	        |-------	|
| __ScreenPlay Main App__       | ✔       	    | ✔     	        | ✔     	|
| __Steam Binaries__       | ✔       	    | ❌     	        | ❌     	|
| __Wallpaper__       | ✔       	    | ❌ Help Needed for Gnome/KDE/etc!               | ❓ (Basic implementation) Help Needed!     	|
| __Widgets__        | ✔       	    | ❓   	| ❓ Help Needed!    	|
| __Multilanguage (EN,DE,RU,FR,ES)__              | ✔            	| ✔     	        |  ✔     	|

</div>

Because every operating system has his own version of desktop environment we need to adapt the ScreenPlayWindow for every platform seperate. The most feature complete for now is Windows 10. Windows 7 works but the wallpaper have the [worng coordinates](https://gitlab.com/kelteseth/ScreenPlay/issues/34). MacOS has some basic wallpaper functionality but no maintainer. For Linux we sadly have no support for any desktops environments at the moment.

__If you want to help and add new desktop environments look at ScreenPlayWallpaper/src folder__

* [BaseWindow](https://gitlab.com/kelteseth/ScreenPlay/blob/dev/ScreenPlayWallpaper/src/basewindow.h) baseclass for:
    * [LinuxWindow](https://gitlab.com/kelteseth/ScreenPlay/blob/dev/ScreenPlayWallpaper/src/linuxwindow.h)
    * [WinWindow](https://gitlab.com/kelteseth/ScreenPlay/blob/dev/ScreenPlayWallpaper/src/winwindow.h)
    * [MacWindow](https://gitlab.com/kelteseth/ScreenPlay/blob/dev/ScreenPlayWallpaper/src/macwindow.h)

<div align="center">

| Plattform                	    | Windows 7	       | Windows 8/8.1 	    | Windows 10  |   Gnome	         | KDE 	            | XFCE  	        | MacOS  	|
|------------------------	    |---------	       |-------	            |-------	  |---------	     |-------	        |-------	        | -------	|
| __Wallpaper__                 | ❓ Help Needed!   | ❓ Help Needed!    | ✔      	| ❌ Help Needed!  | ❌ Help Needed!  |❌ Help Needed!   | ❓ (Basic implementation) Help Needed!     	|


</div>


## Contributing

Everyone can contribute with code, design, documentation or translation. Visit our [contributing guide](https://gitlab.com/kelteseth/ScreenPlay/blob/dev/CONTRIBUTING.md) for more informations.

* If you want to help [translate](https://gitlab.com/kelteseth/ScreenPlay/blob/dev/CONTRIBUTING.md#translation)
* If you are a [programmer](https://gitlab.com/kelteseth/ScreenPlay/blob/dev/CONTRIBUTING.md#development)
* If you are a [designer](https://gitlab.com/kelteseth/ScreenPlay/blob/dev/CONTRIBUTING.md#design)


# Getting started

### Basic
1. Install latest [git + git-lfs](https://git-scm.com/)
2. Clone ScreenPlay
``` bash
git clone https://gitlab.com/kelteseth/ScreenPlay.git
```
3. Download the latest [__Qt 5.14__](https://www.qt.io/download-qt-installer). Earlier versions are not supported!
4. Start install-dependencies.bat to download dependencies into the Common/ folder
``` bash
//Windows
.\install-dependencies.bat

//Linux
chmod +x install-dependencies.sh
.\install-dependencies.sh
```
   * This will install these dependencies via __vcpkg__
      * libzippp
      * nlohmann-json
      * openSSL 1.1.1d
   * Download these dependencies via __git submodules__
      * stomt-qt-sdk
      * qt-google-analytics
5. Follow the steps below for your OS. Then Open The ScreenPlay.pro via QtCreator.
6. Add install to the build steps. Projects -> Build -> Add Build Step -> Select Make -> Insert "install" into Make arguments.
7. Press build (the green play button). This will compile the project and copy all necessary files into your Qt installation.

### Windows
1. [Download and install MSVC 2019 Community](https://visualstudio.microsoft.com/vs/community/)
2. [Download and install Qt 5 binary installer from qt.io](https://www.qt.io/download-qt-installer)
    - Install the Maintaince tool
    - Select the following features to install:
        - Qt 5.14.0
            - MSVC 2017 64-bit
            - Qt WebEngine

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
        - Qt 5.14.0 
            - GCC
            - Qt WebEngine
### OSX
1. Install XCode via the app store
2. Launch XCode and install the XCode command line tools. (This may take a few minutes)
    - If in doubt or unsure, execute 'xcode-select --install'
3. [Download and install Qt 5 binary installer from qt.io](https://www.qt.io/download-qt-installer)
    - Install the Maintaince tool
    - Select the following features to install:
        - Qt 5.14.0 
            - Qt WebEngine
        - Developer and Designer Tools
            - OpenSSL 1.1.1.c Toolkit
                - OpenSSL 64-bit binaries
4. Change your default kit: QtCreator -> Options -> Kits -> Select your default kit (Desktop Qt 5.13.0) -> Change c and c++ Compiler to Apple Clang (x86_64)
