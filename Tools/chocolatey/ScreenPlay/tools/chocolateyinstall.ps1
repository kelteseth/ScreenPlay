
$ErrorActionPreference = 'Stop';
$toolsDir   = "$(Split-Path -parent $MyInvocation.MyCommand.Definition)"
$url        = ''
$url64      = 'https://kelteseth.com/releases/0.15.0-RC3/ScreenPlay-0.15.0-RC3-x64-windows-release.zip' 

$packageArgs = @{
  packageName   = $env:ChocolateyPackageName
  unzipLocation = $toolsDir
  url           = $url
  url64bit      = $url64
  fileType      = 'zip'

  softwareName  = 'ScreenPlay*'
  checksum      = ''
  checksumType  = 'sha256'
  checksum64    = '278f86a888810bab63af17309dd153a59eae35831232cd813c93e3ddb4e32b99'
  checksumType64= 'sha256'
}

Install-ChocolateyZipPackage @packageArgs
