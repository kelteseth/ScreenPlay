git submodule update --init
git submodule update --recursive
cd Common
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
git pull
rem  master 10.09.2020 - 18ab4b72a26284f0df28295ce7bf9b21c96f20f4
git checkout  18ab4b72a26284f0df28295ce7bf9b21c96f20f4
call bootstrap-vcpkg.bat

rem Install vcpkg dependencies
vcpkg.exe install openssl  --triplet x64-windows --recurse

rem Donwload ffmpeg
curl.exe -L https://www.gyan.dev/ffmpeg/builds/packages/ffmpeg-4.3.1-full_build.zip --output ffmpeg.zip

rem Extract ffmpeg. Needs Windows 10 build 17063 or higher!
rem We only need the content of the bin folder
rem --strip-components 2 removes folder
tar -xvf ffmpeg.zip  --strip-components 2  ffmpeg-4.3.1-full_build/bin

rem Create ffmpeg folder
mkdir Common/ffmpeg

rem Remove not used ffplay
DEL  ffplay.exe

rem Move ffmpeg into folder
move /Y  ffmpeg.exe  Common/ffmpeg
move /Y  ffprobe.exe  Common/ffmpeg