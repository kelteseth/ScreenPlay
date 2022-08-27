
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
  checksum64    = '0502e549ff4f10ec8b9090eb1fc35a9fe6297ea64f435aaefd87a77e5f70190f'
  checksumType64= 'sha256'
}

Install-ChocolateyZipPackage @packageArgs
