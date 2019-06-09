setlocal EnableExtensions

cd Common
cd vcpkg
git pull
call bootstrap-vcpkg.bat
vcpkg.exe install openssl:x64-windows
