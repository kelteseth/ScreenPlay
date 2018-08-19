# Requirements
 - [Download and install Qt 5] (https://www.qt.io/download-qt-installer)
    - Install Maintaince tool
    - Select the following features to install:
        - Qt 5.11.1
            - MSVC 2017 64-bit
            - Qt WebEngine
        - Tools
            - Qt Creator 4.x CDB Debugger Support

# Optional
This is only needed for a standalone Steam release build to run WinDeploy.sh!
- [Download and install msys2 x86_x64] (http://www.msys2.org/)
   - Start msys2
   - Run: ``` pacman -Syu ``` then restart terminal
   - Run: ``` pacman -Su ```

# Release
- Change in WinDeploy.sh the paths in Line 4 - 5 according to your setup
    - ReleasePath='C:\\Users\\Eli\\Code\\Qt\\build-ScreenPlay-Desktop-Release\\ScreenPlay\\release\\'
    - DeployPath='C:\\Users\\Eli\\Code\\Qt\\ScreenPlay-Deploy\\'
- Run WinDeploy.sh

