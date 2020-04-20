git submodule update --init
git submodule update --recursive
cd Common
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
git pull
git checkout origin/master
chmod +x bootstrap-vcpkg.sh
./bootstrap-vcpkg.sh
chmod +x vcpkg

if [[ "$OSTYPE" == "darwin"* ]]; then
./vcpkg install libzippp:x64-osx nlohmann-json:x64-osx openssl-unix:x64-osx libzip:x64-osx
else
./vcpkg install libzippp:x64-linux nlohmann-json:x64-linux openssl-unix:x64-linux  	libzip:x64-linux
fi