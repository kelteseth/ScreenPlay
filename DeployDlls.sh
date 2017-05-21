#!/bin/bash

Msys2Path='C:\\msys64\\mingw64\\bin'

array=(QtAVWidgets1.dll QtAV1.dll zlib1.dll libbz2-1.dll libfreetype-6.dll libgcc_s_seh-1.dll libglib-2.0-0.dll libgraphite2.dll libharfbuzz-0.dll libiconv-2.dll libicudt57.dll libicuin57.dll libicuuc57.dll libintl-8.dll libpcre-1.dll libpcre16-0.dll libpng16-16.dll libpng16-config libpng-config libstdc++-6.dll libwinpthread-1.dl libvorbisenc-2.dll libvorbisfile-3.dll libvorbis-0.dll xvidcore.dll libschroedinger-1.0-0.dll libtheora-0.dll libtheoradec-1.dll libtheoraenc-1.dll libmodplug-1.dll libcaca++-0.dll libcaca-0.dll libmp3lame-0.dll libspeexdsp-1.dll libcelt0-2.dll libogg-0.dll libgmodule-2.0-0.dll libgobject-2.0-0.dll libgthread-2.0-0.dll libgio-2.0-0.dll libglib-2.0-0.dll swresample-2.dll swscale-4.dll postproc-54.dll avutil-55.dll avformat-57.dll avresample-3.dll avcodec-57.dll avdevice-57.dll avfilter-6.dll libssp-0.dll libstdc++-6.dll libgomp-1.dll libquadmath-0.dll libatomic-1.dll libgcc_s_seh-1.dll libfreetype-6.dll libwinpthread-1.dll libopencore-amrwb-0.dll libopencore-amrnb-0.dll libopus-0.dll libwavpack-1.dll libbluray-2.dll libvpx-1.dll libx265.dll libx265_main10.dll libtiffxx-5.dll libtiff-5.dll libopenmj2-7.dll libopenjpip-7.dll libopenjpwl-7.dll libopenjp2-7.dll libopenjp3d-7.dll libpng16-16.dll libwebpdecoder-3.dll libwebpdemux-2.dll libwebpmux-3.dll libwebp-7.dll edit.dll libharfbuzz-icu-0.dll libharfbuzz-0.dll libharfbuzz-gobject-0.dll libp11-kit-0.dll libgnutlsxx-28.dll libgnutls-30.dll libcharset-1.dll libiconv-2.dll liblzo2-2.dll libsystre-0.dll libtre-5.dll librtmp-1.dll libintl-8.dll libgettextpo-0.dll libgettextsrc-0-19-8-1.dll libasprintf-0.dll libgettextlib-0-19-8-1.dll libcppunit-1-13-0.dll libpcre32-0.dll libpcrecpp-0.dll libpcreposix-0.dll libpcre-1.dll libpcre16-0.dll zlib1.dll libminizip-1.dll libbz2-1.dll libass-9.dll libspeex-1.dll libcairo-script-interpreter-2.dll libcairo-2.dll libcairo-gobject-2.dll libunistring-2.dll liblzma-5.dll libgmp-10.dll libgmpxx-4.dll libexpat-1.dll libfontconfig-1.dll libxml2-2.dll libgraphite2.dll SDL2.dll libnettle-6.dll libhogweed-4.dll libturbojpeg-0.dll libjpeg-8.dll libx264-148.dll liblsmash.dll liblsmash-2.dll liblcms2-2.dll libpixman-1-0.dll liborc-0.4-0.dll liborc-test-0.4-0.dll libtasn1-6.dll libidn-11.dll libffi-6.dll libgsm.dll libgif-7.dll libopenal-1.dll libfribidi-0.dll libquazip.dll)

echo 'Attention! This shell script must' 
echo 'be executed in the release folder!'
echo ''
echo Is your default msys2 path $Msys2Path ? [Y/N]
read input

if [ "$input" == "N" ] || [ "$input" == "n" ]
then
    echo Pleas input you msys path like $Msys2Path
    read customMsys2Path
    Msys2Path=customMsys2Path
fi

for item in ${array[*]}
do
	printf $Msys2Path'\\'$item"\t to "$PWD"\n"
    cp $Msys2Path'\\'$item $PWD
done

echo 'press any button to exit <3'
read sads