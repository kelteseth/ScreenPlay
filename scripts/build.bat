setlocal EnableExtensions

set PATH=%PATH%;C:\Qt\Tools\QtCreator\bin
set PATH=%PATH%;C:\Qt\5.13.0\msvc2017_64\bin
set root=%cd%

call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsx86_amd64.bat"

mkdir BUILD
cd BUILD

echo "Begin build of ScreenPlay "
qmake.exe ../ScreenPlay.pro -spec win32-msvc "CONFIG+=release"
jom.exe qmake_all
jom.exe  -j8
jom.exe  install
echo "Build ScreenPlay finished!"

