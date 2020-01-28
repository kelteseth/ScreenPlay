
git submodule update --recursive
cd Common
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
git pull
git checkout origin/master
chmod +x bootstrap-vcpkg.sh
./bootstrap-vcpkg.sh
chmod +x vcpkg
./vcpkg install libzippp:x64-linux nlohmann-json:x64-linux openssl:x64-linux
