# Requirements
 - [Download and install msys2 x86_x64] (http://www.msys2.org/)
    - Run: ``` bash pacman -Syu ``` then restart terminal
    - Run: ``` pacman -Su ```
    - Run: ``` pacman -S  mingw64/mingw-w64-x86_64-gdb mingw64/mingw-w64-x86_64-qt5 mingw64/mingw-w64-x86_64-qt-creator mingw64/mingw-w64-x86_64-quazip mingw64/mingw-w64-x86_64-zlib mingw64/mingw-w64-x86_64-cmake mingw64/mingw-w64-x86_64-gcc mingw64/mingw-w64-x86_64-ffmpeg ```
 - [QtAV for video encoding via git submodule] (https://github.com/wang-bin/QtAV) 
    - Open QtAV project via QtCreator in the Thirdparty folder and copile (use -j16 as make argument to use more cores)
    - Fix 3 dll names in \lib_win_x86\ via changing *.dll.a to *.a  Bug: https://github.com/wang-bin/QtAV/issues/878
    - After compiling dont forget to copy QtVA via QtAV_buildDir/sdk_install.bat

# Release
 - Compile QtAV (plus copy the created dlls via QtAV_buildDir/sdk_install.bat) and Screenplay as release version
 - Auto copy dependencies via  windeployqt.exe --release --qmldir **/qml-dir-location** **/exe-location**
    - Example : **C:\msys64\mingw64\bin\windeployqt.exe**  --release --qmldir **C:\Users\Eli\Code\Qt\ScreenPlay\qml** **C:\Users\Eli\Code\Qt\build-ScreenPlay-Desktop_Qt_MinGW_w64_64bit_MSYS2-Release**
 - Extra step: Copy ``` zlib1.dll libbz2-1.dll libfreetype-6.dll libgcc_s_seh-1.dll libglib-2.0-0.dll libgraphite2.dll libharfbuzz-0.dll libiconv-2.dll libicudt57.dll libicuin57.dll libicuuc57.dll libintl-8.dll libpcre-1.dll libpcre16-0.dll libpng16-16.dll libpng16-config libpng-config libstdc++-6.dll libwinpthread-1.dll libjpeg-8.dll``` from **C:\msys64\mingw64\bin** into the release folder!