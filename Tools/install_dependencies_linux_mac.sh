git submodule update --init
git submodule update --recursive
cd ..
cd ..
git clone https://github.com/microsoft/vcpkg.git ScreenPlay-vcpkg
cd ScreenPlay-vcpkg
git pull
# master 27.03.2021 - 9f6157a
git checkout 9f6157a
chmod +x bootstrap-vcpkg.sh
./bootstrap-vcpkg.sh
chmod +x vcpkg

if [[ "$OSTYPE" == "darwin"* ]]; then
./vcpkg install  openssl-unix  sentry-native doctest benchmark --triplet x64-osx --recurse
else
./vcpkg install  openssl-unix  sentry-native doctest benchmark  --triplet x64-linux --recurse
fi

./vcpkg upgrade --no-dry-run 