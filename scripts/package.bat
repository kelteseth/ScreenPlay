setlocal EnableExtensions

set PATH=%PATH%;C:\Qt\Tools\QtCreator\bin
set PATH=%PATH%;C:\Qt\5.11.2\msvc2017_64\bin
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
echo "Copy ScreenPlayWindow"
xcopy /s /y ScreenPlayWindow\release ..\PACKAGE
echo "Copy ScreenPlayWorkshop"
xcopy /s /y ScreenPlayWorkshop\release ..\PACKAGE

xcopy  C:\Qt\Tools\QtCreator\bin\libeay32.dll ..\PACKAGE
xcopy  C:\Qt\Tools\QtCreator\bin\ssleay32.dll ..\PACKAGE


cd ..

windeployqt.exe  --release  --qmldir ScreenPlay/qml PACKAGE/ScreenPlay.exe

cd PACKAGE

del /f *.cpp
del /f *.moc
del /f *.h
del /f *.obj
del /f *.res

