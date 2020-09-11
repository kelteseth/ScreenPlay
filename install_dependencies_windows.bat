git submodule update --init
git submodule update --recursive
cd Common
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
git pull
rem master 10.09.2020 - 18ab4b72a26284f0df28295ce7bf9b21c96f20f4
git checkout 18ab4b72a26284f0df28295ce7bf9b21c96f20f4
call bootstrap-vcpkg.bat
vcpkg.exe install zlib libzip libzippp openssl sentry-native --triplet x64-windows --recurse
