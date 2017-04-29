# Requirements
 - [Qt 5.9] (https://www.qt.io/download-open-source/)
 - [QtAV for video encoding] (https://github.com/wang-bin/QtAV)
    - [Follow these build instructions] (https://github.com/wang-bin/QtAV/wiki/Build-QtAV)
    - [Download Windows dependencies] (http://sourceforge.net/projects/qtav/files/depends/QtAV-depends-windows-x86%2Bx64.7z/download)
    - After compiling dont forget to copy QtVA via QtAV_buildDir/sdk_install.bat

# Release
 - Compile QtAV and Screenplay as release version
 - Auto copy dependencies via  windeployqt.exe --release --qmldir qml-dir-location exe-location
    - Example : _C:\Qt\5.9\mingw53_32\bin\windeployqt.exe_  --release --qmldir _C:\Users\Eli\Code\Qt\ScreenPlay\qml_ _C:\Users\Eli\Code\Qt\build-ScreenPlay-Qt_5_9_mingw-Release\release_
 - MinGW extra step: Copy  _C:\Qt\5.9\mingw53_32\bin\libgcc_s_dw2-1.dll_ into the release folder!