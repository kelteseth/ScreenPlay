setlocal EnableExtensions

set PATH=%PATH%;C:\Qt\Tools\QtCreator\bin
set PATH=%PATH%;C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Tools\MSVC\14.15.26726\bin\Hostx64\x64
set PATH=%PATH%;C:\Qt\5.11.2\msvc2017_64\bin
set root=%cd%

call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsx86_amd64.bat"

echo "Begin build of ScreenPlay "
cd ..
qmake.exe ScreenPlay.pro -spec win32-msvc "CONFIG+=release"
jom.exe  -j8
echo "Build ScreenPlay finished!"

