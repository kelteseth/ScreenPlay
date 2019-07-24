setlocal EnableExtensions

cd Common
cd vcpkg
git pull
git checkout origin/master
call bootstrap-vcpkg.bat
vcpkg.exe install openssl:x64-windows libzippp:x64-windows

pause