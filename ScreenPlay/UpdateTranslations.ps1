echo "Updating translations..."
$MSVCPATH = Read-Host 'Type your ABSOLUTE Qt "bin" directory (ex: F:\Qt\5.15.1\msvc2019_64\bin)'
echo "Begin update translations..."
$files = Get-ChildItem ".\translations"
Function Execute-Command ($commandTitle, $commandPath, $commandArguments, $commandWorkDir)
{
    $resolvedWorkDir = Resolve-Path -Path $commandWorkDir
    $pinfo = New-Object System.Diagnostics.ProcessStartInfo
    $pinfo.FileName = $commandPath
    $pinfo.RedirectStandardError = $true
    $pinfo.RedirectStandardOutput = $true
    $pinfo.UseShellExecute = $false
    $pinfo.WorkingDirectory = $resolvedWorkDir
    $pinfo.Arguments = $commandArguments
    $p = New-Object System.Diagnostics.Process
    $p.StartInfo = $pinfo
    $p.Start() | Out-Null
    $p.WaitForExit()
    [pscustomobject]@{
        stdout = $p.StandardOutput.ReadToEnd()
        stderr = $p.StandardError.ReadToEnd()
        ExitCode = $p.ExitCode
    }
}
foreach ($f in $files){
    echo "Processing $f (1/2)"
    Execute-Command -commandTitle "Update translation file: $f" -commandPath "$MSVCPATH\lupdate.exe" -commandArguments "-extensions qml ..\qml -ts $f" -commandWorkDir "translations"
    echo "Processing $f (2/2)"
    Execute-Command -commandTitle "Process translation file: $f" -commandPath "$MSVCPATH\lrelease.exe" -commandArguments "$f" -commandWorkDir "translations"
    echo "Done"
}
echo "Updated translations."
pause