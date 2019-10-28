setlocal EnableExtensions

set PATH=%PATH%;C:\Qt\Tools\QtCreator\bin
set PATH=%PATH%;C:\Qt\5.14.0\msvc2017_64\bin
set root=%cd%

call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsx86_amd64.bat"

echo "Begin packaging"

mkdir PACKAGE
cd BUILD

echo "Copy ScreenPlay"
xcopy /s /y ScreenPlay\release ..\PACKAGE
echo "Copy ScreenPlaySDK"
xcopy /s /y ScreenPlaySDK\release ..\PACKAGE
echo "Copy ScreenPlayWidget"
xcopy /s /y ScreenPlayWidget\release ..\PACKAGE
echo "Copy ScreenPlayWallpaper"
xcopy /s /y ScreenPlayWallpaper\release ..\PACKAGE

cd ..

windeployqt.exe  --release  --qmldir ScreenPlay/qml PACKAGE/ScreenPlay.exe
windeployqt.exe  --release  --qmldir ScreenPlayWallpaper/qml PACKAGE/ScreenPlayWallpaper.exe

cd PACKAGE

del /f *.cpp
del /f *.moc
del /f *.h
del /f *.obj
del /f *.res
del /f *.exp
del /f *.lib

