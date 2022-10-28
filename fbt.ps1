Push-Location $PSScriptRoot

$firmware_path = ""
$firmware_name = ""
if (('official', 'off', 'o').Contains($args[0])) {
    $firmware_path = "flipperzero-firmware_official"
    $firmware_name = "Official"
}
elseif (('unleashed', 'un', 'u').Contains($args[0])) {
    $firmware_path = "flipperzero-firmware_unleashed"
    $firmware_name = "Unleashed"
}
else {
    throw "Unable to recognize which firmware to use"
}

Write-Host "Using $firmware_name firmware to run FBT"

$commandline_scons = "$firmware_path\site_scons\commandline.scons"
((Get-Content -Path $commandline_scons -Raw) -replace 'applications_user','..') | Set-Content -Path $commandline_scons

$builtin_totp_path = "$firmware_path\applications\plugins\totp"
if ((Test-Path -Path $builtin_totp_path) -eq $True) {
    Remove-Item $builtin_totp_path -Recurse
}

Push-Location $firmware_path

./fbt $args[1..($args.Length - 1)]

Pop-Location
Pop-Location