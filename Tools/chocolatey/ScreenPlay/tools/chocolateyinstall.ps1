
$ErrorActionPreference = 'Stop';
$toolsDir   = "$(Split-Path -parent $MyInvocation.MyCommand.Definition)"
$url        = ''
$url64      = 'https://kelteseth.com/releases/0.15.0/ScreenPlay-0.15.0-x64-windows-release.zip' 

$packageArgs = @{
  packageName   = $env:ChocolateyPackageName
  unzipLocation = $toolsDir
  url           = $url
  url64bit      = $url64
  fileType      = 'zip'

  softwareName  = 'ScreenPlay*'
  checksum      = ''
  checksumType  = 'sha256'
  checksum64    = 'f870de3a43fb5572021a2be0134dfa23585e8e56dd8067176ed8afe624c8cb4f'
  checksumType64= 'sha256'
}

Install-ChocolateyZipPackage @packageArgs
