git submodule update --init
git submodule update --recursive
cd ..
cd ..
git clone https://github.com/microsoft/vcpkg.git ScreenPlay-vcpkg
cd ScreenPlay-vcpkg
git pull
# master 12.05.2021 - 7b1016e10ef0434fe7045d320a9ee05b63e0efe9
git checkout 7b1016e10ef0434fe7045d320a9ee05b63e0efe9
chmod +x bootstrap-vcpkg.sh
./bootstrap-vcpkg.sh
chmod +x vcpkg

if [[ "$OSTYPE" == "darwin"* ]]; then
./vcpkg install  openssl-unix  sentry-native doctest benchmark infoware[opencl] --triplet x64-osx --recurse
else
./vcpkg install  openssl-unix  sentry-native doctest benchmark infoware[opencl] --triplet x64-linux --recurse
fi

./vcpkg upgrade --no-dry-run 