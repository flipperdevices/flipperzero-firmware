function Get-LatestDirectory {
    param (
        $Path
    )

    Get-ChildItem -Path $Path | Where-Object {$_.PSIsContainer} | Sort-Object LastWriteTime -Descending | Select-Object -First 1
}

Push-Location $PSScriptRoot

$official_build_path = "flipperzero-firmware_official\build"
$unleashed_build_path = "flipperzero-firmware_unleashed\build"

Remove-Item "$official_build_path\*" -Recurse -Force
Remove-Item "$unleashed_build_path\*" -Recurse -Force

./fbt u COMPACT=1 DEBUG=0 VERBOSE=0 fap_totp
./fbt o COMPACT=1 DEBUG=0 VERBOSE=0 fap_totp

if (!(Test-Path -PathType Container "build")) {
    New-Item -ItemType Directory -Path "build"
} else {
    Remove-Item "build\*" -Recurse -Force
}

$official_latest_dir = Get-LatestDirectory -Path $official_build_path
Copy-Item "$official_build_path\$official_latest_dir\.extapps\totp.fap" -Destination "build\totp_official-fw.fap"

$unleashed_latest_dir = Get-LatestDirectory -Path $unleashed_build_path
Copy-Item "$unleashed_build_path\$unleashed_latest_dir\.extapps\totp.fap" -Destination "build\totp_unleashed-fw.fap"

Pop-Location