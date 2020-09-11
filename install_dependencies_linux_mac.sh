git submodule update --init
git submodule update --recursive
cd Common
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
git pull
git checkout 2bc6cd714
chmod +x bootstrap-vcpkg.sh
./bootstrap-vcpkg.sh
chmod +x vcpkg

if [[ "$OSTYPE" == "darwin"* ]]; then
./vcpkg install zlib libzip libzippp openssl-unix libzip sentry-native --triplet x64-osx --recurse
else
./vcpkg install zlib libzip libzippp openssl-unix libzip sentry-native --triplet x64-linux --recurse
fi