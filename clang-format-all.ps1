Get-ChildItem -Path "totp" -Directory -Recurse | ForEach-Object {
    if ((Test-Path "$($_.FullName)/*.c") -or (Test-Path "$($_.FullName)/*.h")) {
        Push-Location $_.FullName
        Write-Host "Processing $($_.FullName)"
        &clang-format -i -style=file *.c *.h
        Pop-Location
    }
    
}