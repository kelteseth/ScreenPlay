# Requirements
 - [msys2 x86_x64] (http://www.msys2.org/)
    - Run: ``` bash pacman -Syu ``` then restart terminal
    - Run: ``` pacman -Su ```
    - Run: ``` pacman -S  mingw64/mingw-w64-x86_64-gdb mingw64/mingw-w64-x86_64-qt5 mingw64/mingw-w64-x86_64-qt-creator mingw64/mingw-w64-x86_64-quazip mingw64/mingw-w64-x86_64-zlib mingw64/mingw-w64-x86_64-cmake mingw64/mingw-w64-x86_64-gcc ```
 - [QtAV for video encoding] (https://github.com/wang-bin/QtAV) 
    - Open QtAV project via QtCreator in the Thirdparty folder 
    - Fix 3 dll names in \lib_win_x86\ via changing *.dll.a to *.a  Bug: https://github.com/wang-bin/QtAV/issues/878
    - After compiling dont forget to copy QtVA via QtAV_buildDir/sdk_install.bat

# Release
 - Compile QtAV and Screenplay as release version
 - Auto copy dependencies via  windeployqt.exe --release --qmldir **qml-dir-location** **exe-location**
    - Example : _C:\Qt\5.9\mingw53_32\bin\windeployqt.exe_  --release --qmldir **C:\Users\Eli\Code\Qt\ScreenPlay\qml** **C:\Users\Eli\Code\Qt\build-ScreenPlay-Qt_5_9_mingw-Release\release**
 - MinGW extra step: Copy  _C:\Qt\5.9\mingw53_32\bin\libgcc_s_dw2-1.dll_ into the release folder!