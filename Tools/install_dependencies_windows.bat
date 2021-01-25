git submodule update --init
git submodule update --recursive
cd ..
cd ..
git clone https://github.com/microsoft/vcpkg.git ScreenPlay-vcpkg
cd ScreenPlay-vcpkg
git pull
rem master 25.01.2021 - fc0d6b28006e0607a6b9871641ec48925274e079
git checkout fc0d6b28006e0607a6b9871641ec48925274e079
call bootstrap-vcpkg.bat

rem Install vcpkg dependencies
vcpkg.exe install openssl  sentry-native  doctest benchmark  --triplet x64-windows --recurse
vcpkg.exe upgrade --no-dry-run 

cd ..
cd ScreenPlay

rem Download 7-zip
curl.exe -L https://www.7-zip.org/a/7z1900.msi --ssl-no-revoke --output 7z.msi
rem Extract 7z
msiexec /a 7z.msi /qb TARGETDIR="%cd%\7z"

rem Donwload ffmpeg
curl.exe -L https://www.gyan.dev/ffmpeg/builds/ffmpeg-release-full-shared.7z --ssl-no-revoke --output ffmpeg.7z
rem Extract ffmpeg
"%cd%\7z\Files\7-Zip\7z.exe" e -y ffmpeg.7z -o"%cd%\Common\ffmpeg" *.exe *.dll -r

rem Remove not used ffplay
DEL Common\ffmpeg\ffplay.exe

rem Deleting FFmpeg temp
DEL ffmpeg.7z
DEL 7z.msi
rmdir 7z /s /q
timeout 5 > NUL