setlocal EnableExtensions

cd Common
cd vcpkg
git pull
git checkout origin/master
call bootstrap-vcpkg.bat
vcpkg.exe install libzippp:x64-windows nlohmann-json:x64-windows
pause