<div  style="display: flex; justify-content:center;">
<img width="100%" height="93" src="https://screen-play.app/images/logo_gitlab_fullwidth.svg">
</div>

# Requirements
### Windows
1. [Download and install MSVC 2017 Community](https://visualstudio.microsoft.com/vs/community/)
2. [Download and install Win 10 SDK (debugging support. Not included via the MSVC installer)](https://developer.microsoft.com/en-us/windows/downloads/windows-10-sdk)
    - Select debugging support during the installation (CDB)
3. [Download and install Qt 5 binary installer from qt.io](https://www.qt.io/download-qt-installer)
    - Install the Maintaince tool
    - Select the following features to install:
        - Qt 5.11.2
            - MSVC 2017 64-bit
            - Qt WebEngine
        - Tools
            - Qt Creator 4.x CDB Debugger Support
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
        - Qt 5.11.2
            - GCC
            - Qt WebEngine
        - Tools
            - Qt Creator 4.x CDB Debugger Support
### OSX
1. Install xcode via the app store
2. [Download and install Qt 5 binary installer from qt.io](https://www.qt.io/download-qt-installer)
    - Install the Maintaince tool
    - Select the following features to install:
        - Qt 5.11.2
            - Qt WebEngine
        - Tools
            - Qt Creator 4.x CDB Debugger Support