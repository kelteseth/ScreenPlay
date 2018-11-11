setlocal EnableExtensions

set PATH=%PATH%;C:\Qt\Tools\QtCreator\bin
set PATH=%PATH%;C:\Qt\5.11.2\msvc2017_64\bin
set root=%cd%

call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsx86_amd64.bat"

echo "Begin packaging"

mkdir PACKAGE
cd BUILD

xcopy /s  ScreenPlay\release ..\PACKAGE
xcopy /s  ScreenPlaySDK\release ..\PACKAGE
xcopy /s  ScreenPlayWidget\release ..\PACKAGE
xcopy /s  ScreenPlayWindow\release ..\PACKAGE
xcopy /s  ScreenPlayWorkshop\release ..\PACKAGE

cd ..

windeployqt.exe  --release  --qmldir ScreenPlay/qml PACKAGE/ScreenPlay.exe

cd PACKAGE

del /f *.cpp
del /f *.h
del /f *.obj
del /f *.res

