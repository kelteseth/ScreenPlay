setlocal EnableExtensions

call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsx86_amd64.bat"

echo "Begin packaging"


mkdir package
cd package

xcopy "Shared" "package\Launcher" /s /y

C:\Qt\5.11.2\msvc2017_64\bin\windeployqt.exe  --release  --qmldir Launcher/qml package/Launcher/DC-Launcher.exe


del /f package\PatchFileGenerator\*.cpp
del /f package\PatchFileGenerator\*.h
del /f package\PatchFileGenerator\*.obj
del /f package\PatchFileGenerator\*.res