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

if ((Test-Path -Path "$firmware_path\applications_user\totp") -ne $True) {
    $mklink_command = "mklink /D $firmware_path\applications_user\totp ..\..\totp"
    if (!(New-Object Security.Principal.WindowsPrincipal([Security.Principal.WindowsIdentity]::GetCurrent())).IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)) {
        Write-Host $PSScriptRoot
        Start-Process -FilePath 'cmd' -ArgumentList ( "/C", "cd $PSScriptRoot & $mklink_command") -Verb RunAs
    } else {
        Invoke-Expression -Command $mklink_command
    }    
}

Push-Location $firmware_path

./fbt $args[1..($args.Length - 1)]

Pop-Location
Pop-Location