Push-Location $PSScriptRoot

$firmware_path = ""
$firmware_name = ""
if (('official-dev', 'off-dev', 'od', 'unleashed', 'un', 'u').Contains($args[0])) {
    $firmware_path = "flipperzero-firmware_official_dev"
    $firmware_name = "Official Dev"
}
elseif (('official-stable', 'off-stbl', 'os').Contains($args[0])) {
    $firmware_path = "flipperzero-firmware_official_stable"
    $firmware_name = "Official Stable"
} elseif (('xtreme', 'xt', 'x').Contains($args[0])) {
    $firmware_path = "flipperzero-firmware_xtreme"
    $firmware_name = "Xtreme"
}
else {
    throw "Unable to recognize which firmware to use"
}

Write-Host "Using $firmware_name firmware to run FBT"

$commandline_scons = "$firmware_path\site_scons\commandline.scons"
((Get-Content -Path $commandline_scons -Raw) -replace 'applications_user','..') | Set-Content -Path $commandline_scons -NoNewline

$builtin_totp_path = "$firmware_path\applications\external\totp"
if ((Test-Path -Path $builtin_totp_path) -eq $True) {
    Remove-Item $builtin_totp_path -Recurse
}

Push-Location $firmware_path

./fbt $args[1..($args.Length - 1)]

Pop-Location
Pop-Location