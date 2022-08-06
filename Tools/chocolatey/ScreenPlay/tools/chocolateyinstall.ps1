
$ErrorActionPreference = 'Stop';
$toolsDir   = "$(Split-Path -parent $MyInvocation.MyCommand.Definition)"
$url        = ''
$url64      = 'https://kelteseth.com/screenplay/ScreenPlay-0.15.0-RC1-x64-windows-release.zip' 

$packageArgs = @{
  packageName   = $env:ChocolateyPackageName
  unzipLocation = $toolsDir
  url           = $url
  url64bit      = $url64
  fileType      = 'zip'

  softwareName  = 'ScreenPlay*'
  checksum      = ''
  checksumType  = 'sha256'
  checksum64    = ''
  checksumType64= 'sha256'
}

Install-ChocolateyZipPackage @packageArgs
