#!/bin/bash

QtPath='C:/Qt/5.11.1/msvc2017_64/bin'
QtCreatorPath='C:\Qt\Tools\QtCreator\bin'
ReleasePath='C:/Users/Eli/Code/Qt/build-ScreenPlay-Desktop_Qt_5_11_1_MSVC2017_64bit-Release/ScreenPlay/release/'
DeployPath='C:/Users/Eli/Code/Qt/ScreenPlay-Deploy/'
SourcePath=$PWD
SteamSDK='ScreenPlay/ThirdParty/steam/redistributable_bin/win64/'


echo 'This is script will now copy' 
echo 'all depenecies into the release folder'
echo ''

echo 'Generating folder structure'
cd $ReleasePath
if  [ ! -d $DeployPath ] 
then
	mkdir $DeployPath
fi
if [ ! -d  $DeployPath'ScreenPlay' ] 
then
	mkdir $DeployPath'ScreenPlay'
fi
if [ ! -d  $DeployPath'ScreenPlaySDK' ] 
then
	mkdir $DeployPath'ScreenPlaySDK'
fi

rm *.obj
rm *.cpp
rm *.o

echo 'Copying files from Release to Deploy path'
cp -r ./* $DeployPath'ScreenPlay'

cd ..
cd ..
cd 'ScreenPlaySDK'

cp 'qmldir' $DeployPath'ScreenPlaySDK'
cd 'release'
rm *.obj
rm *.cpp
rm *.o
cp -r ./* $DeployPath'ScreenPlaySDK'

cd ..
cd ..
cd 'ScreenPlayWindow'
cd 'release'
rm *.obj
rm *.cpp
rm *.o
cp -r ./* $DeployPath'ScreenPlay'

cd ..
cd ..
cd 'ScreenPlayWidget'
cd 'release'
rm *.obj
rm *.cpp
rm *.o
cp -r ./* $DeployPath'ScreenPlay'


echo ''
echo 'Deploying Steam SDK'
cp $SourcePath'/'$SteamSDK'steam_api64.dll' $DeployPath'ScreenPlay'
cp $SourcePath'/'$SteamSDK'steam_api64.lib' $DeployPath'ScreenPlay'
cp $SourcePath'/ScreenPlay/''steam_appid.txt' $DeployPath'ScreenPlay'
cp -R $SourcePath'/ScreenPlay/legal/' $DeployPath'ScreenPlay/'

echo 'Copying OpenSSL from QtCreator'
cp $QtCreatorPath'/libeay32.dll' $DeployPath'ScreenPlay' 
cp $QtCreatorPath'/ssleay32.dll' $DeployPath'ScreenPlay'


echo ''
echo 'Deploying QtDLLs via windeployqt.exe'
qtDeployPath=' --release  --qmldir  '$SourcePath' '$DeployPath'/ScreenPlay/ScreenPlay.exe'
echo 'Loading please wait!'
exec $QtPath'/'windeployqt.exe $qtDeployPath

echo 'press any button to exit <3'
read sads