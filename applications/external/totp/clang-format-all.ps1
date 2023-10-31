$clang_format = Join-Path -Path $PSScriptRoot -ChildPath "flipperzero-firmware_official_dev/toolchain/x86_64-windows/bin/clang-format.exe"
Push-Location $PSScriptRoot
Get-ChildItem -Path "totp" -File -Recurse -Include "*.c", "*.h" | ForEach-Object {
    if (!($_.FullName -like '*wolfssl*')) {
        Write-Host "Formatting $($_.FullName)"
        & $clang_format -i -style=file $_.FullName
    }
}
Pop-Location