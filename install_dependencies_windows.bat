git submodule update --init
git submodule update --recursive
cd Common
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
git pull
git checkout 2bc6cd714
call bootstrap-vcpkg.bat
vcpkg.exe install zlib libzip libzippp nlohmann-json openssl breakpad  --triplet x64-windows --recurse
