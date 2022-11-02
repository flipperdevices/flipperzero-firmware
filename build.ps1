function Get-LatestDirectory {
    param (
        $Path
    )

    Get-ChildItem -Path $Path | Where-Object {$_.PSIsContainer} | Sort-Object LastWriteTime -Descending | Select-Object -First 1
}

./fbt u COMPACT=1 DEBUG=0 VERBOSE=0 fap_totp
./fbt o COMPACT=1 DEBUG=0 VERBOSE=0 fap_totp

Push-Location $PSScriptRoot

if (!(Test-Path -PathType Container "build")) {
    New-Item -ItemType Directory -Path "build"
}

$official_latest_dir = Get-LatestDirectory -Path "flipperzero-firmware_official\build\"
Copy-Item "flipperzero-firmware_official\build\$official_latest_dir\.extapps\totp.fap" -Destination "build\totp_official-fw.fap"

$unleashed_latest_dir = Get-LatestDirectory -Path "flipperzero-firmware_unleashed\build\"
Copy-Item "flipperzero-firmware_unleashed\build\$unleashed_latest_dir\.extapps\totp.fap" -Destination "build\totp_unleashed-fw.fap"

Pop-Location