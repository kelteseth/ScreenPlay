setlocal EnableExtensions

cd Common
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
git pull
call bootstrap-vcpkg.bat
vcpkg.exe install openssl:x64-windows 
