git submodule update --init
git submodule update --recursive
cd ..
cd ..
git clone https://github.com/microsoft/vcpkg.git ScreenPlay-vcpkg
cd ScreenPlay-vcpkg
git pull
# master 25.01.2021 - fc0d6b28006e0607a6b9871641ec48925274e079
git checkout fc0d6b28006e0607a6b9871641ec48925274e079
chmod +x bootstrap-vcpkg.sh
./bootstrap-vcpkg.sh
chmod +x vcpkg

if [[ "$OSTYPE" == "darwin"* ]]; then
./vcpkg install  openssl-unix  sentry-native doctest benchmark --triplet x64-osx --recurse
else
./vcpkg install  openssl-unix  sentry-native doctest benchmark  --triplet x64-linux --recurse
fi

./vcpkg upgrade --no-dry-run 