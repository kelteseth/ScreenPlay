### How-to Update Translations?

1. Open PowerShell (in this Directory)
2. Check if you have execution policy set to bypass by executing `Get-ExecutionPolicy`
    + If you have "Bypass" or "Unrestricted" then you are good.
    + If not then execute **in Administrator PowerShell** `Set-ExecutionPolicy Bypass -Scope Process` to set current PowerShell session to Bypass or `Set-ExecutionPolicy Bypass` to set All PowerShell session to Bypass.
3. Execute `./UpdateTranslations.ps1` and follow the instruction printed on the screen.