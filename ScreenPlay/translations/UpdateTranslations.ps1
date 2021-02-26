Function Execute-Command ($commandTitle, $commandPath, $commandArguments)
{
    $pinfo = New-Object System.Diagnostics.ProcessStartInfo
    $pinfo.FileName = "$commandPath"
    $pinfo.RedirectStandardError = $true
    $pinfo.RedirectStandardOutput = $true
    $pinfo.UseShellExecute = $false
    $pinfo.WorkingDirectory = "$PSScriptRoot"
    $pinfo.Arguments = $commandArguments
    $p = New-Object System.Diagnostics.Process
    $p.StartInfo = $pinfo
    $p.Start() | Out-Null
    $p.WaitForExit()
    $exitcodestr = $p.ExitCode.ToString()
    Write-Output $p.StandardOutput.ReadToEnd()
    Write-Output $p.StandardError.ReadToEnd()
    Write-Output "Exit code: $exitcodestr"
}
$MSVCPATH = Read-Host 'Type your ABSOLUTE Qt "bin" directory (default is C:\Qt\5.15.2\msvc2019_64\bin)'
if ($MSVCPATH -eq "") {
    echo "Using default directory..."
    $MSVCPATH = "C:\Qt\5.15.2\msvc2019_64\bin"
}
echo "Testing directory..."
if (((Test-Path -Path "$MSVCPATH\lupdate.exe") -eq $true) -and ((Test-Path -Path "$MSVCPATH\lrelease.exe") -eq $true)) {
    echo "Begin update translations..."
    $files = Get-ChildItem ".\*.ts"
    foreach ($f in $files){
        echo $PSScriptRoot
        echo "Processing $f (1/2)"
        Execute-Command -commandTitle "Update translation file: $f" -commandPath "$MSVCPATH\lupdate.exe" -commandArguments "-extensions qml ..\qml -ts ""$f"""
        echo "Processing $f (2/2)"
        Execute-Command -commandTitle "Compile translation file: $f" -commandPath "$MSVCPATH\lrelease.exe" -commandArguments """$f"""
        echo "Done"
    }
    echo "Updated translations."
}
else
{
    echo "lupdate or lrelease dosen't exist, failed to update translations..."
}
pause