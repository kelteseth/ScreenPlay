
git submodule update --recursive
cd Common
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
git pull
git checkout origin/master
call bootstrap-vcpkg.bat
vcpkg.exe install libzippp:x64-windows nlohmann-json:x64-windows
pause