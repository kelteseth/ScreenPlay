echo "Updating translations..."
$MSVCPATH = Read-Host 'Type your ABSOLUTE Qt "bin" directory (ex: F:\Qt\5.15.1\msvc2019_64\bin)'
echo "Begin update translations..."
$files = Get-ChildItem ".\translations"
foreach ($f in $files){
    echo "Processing $f"
    $pinfo = New-Object System.Diagnostics.ProcessStartInfo
    $pinfo.FileName = "$MSVCPATH/lupdate.exe"
    $pinfo.RedirectStandardError = $true
    $pinfo.RedirectStandardOutput = $true
    $pinfo.UseShellExecute = $false
    $pinfo.Arguments = "-extensions qml ..\qml -ts $f"
    $pinfo.WorkingDirectory = ".\translations"
    $p = New-Object System.Diagnostics.Process
    $p.StartInfo = $pinfo
    $p.Start() | Out-Null
    $p.WaitForExit()
    $stdout = $p.StandardOutput.ReadToEnd()
    $stderr = $p.StandardError.ReadToEnd()
    Write-Host "$stdout"
    Write-Host "$stderr"
    Write-Host "Exit code:" + $p.ExitCode
    echo "Done"
}
echo "Updated translations."
pause