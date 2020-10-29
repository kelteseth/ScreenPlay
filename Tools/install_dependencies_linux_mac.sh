git submodule update --init
git submodule update --recursive
cd ..
cd Common
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
git pull
# master 10.09.2020 - 18ab4b72a26284f0df28295ce7bf9b21c96f20f4
git checkout 18ab4b72a26284f0df28295ce7bf9b21c96f20f4
chmod +x bootstrap-vcpkg.sh
./bootstrap-vcpkg.sh
chmod +x vcpkg

if [[ "$OSTYPE" == "darwin"* ]]; then
./vcpkg install  openssl-unix  sentry-native --triplet x64-osx --recurse
else
./vcpkg install  openssl-unix  sentry-native --triplet x64-linux --recurse
fi