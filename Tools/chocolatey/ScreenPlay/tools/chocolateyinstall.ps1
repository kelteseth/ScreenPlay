
$ErrorActionPreference = 'Stop';
$toolsDir   = "$(Split-Path -parent $MyInvocation.MyCommand.Definition)"
$url        = ''
$url64      = 'https://kelteseth.com/releases/0.15.0-RC2/ScreenPlay-0.15.0-RC2-x64-windows-release.zip' 

$packageArgs = @{
  packageName   = $env:ChocolateyPackageName
  unzipLocation = $toolsDir
  url           = $url
  url64bit      = $url64
  fileType      = 'zip'

  softwareName  = 'ScreenPlay*'
  checksum      = ''
  checksumType  = 'sha256'
  checksum64    = 'e7dc40f99d681db2146a5f48e6bb1c0d8b46af40c9dde896975117b82c7e535f'
  checksumType64= 'sha256'
}

Install-ChocolateyZipPackage @packageArgs
