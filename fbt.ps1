Push-Location $PSScriptRoot

$firmware_path = ""
$firmware_name = ""
$FW_CDEF = ""
if (('official-dev', 'off-dev', 'od').Contains($args[0])) {
    $firmware_path = "flipperzero-firmware_official_dev"
    $firmware_name = "Official Dev"
    $FW_CDEF = "TOTP_FIRMWARE_OFFICIAL_DEV"
}
elseif (('official-stable', 'off-stbl', 'os').Contains($args[0])) {
    $firmware_path = "flipperzero-firmware_official_stable"
    $firmware_name = "Official Stable"
    $FW_CDEF = "TOTP_FIRMWARE_OFFICIAL_STABLE"
} elseif (('xtreme', 'xt', 'x', 'unleashed', 'un', 'u').Contains($args[0])) {
    $firmware_path = "flipperzero-firmware_xtreme"
    $firmware_name = "Xtreme \ Unleashed"
    $FW_CDEF = "TOTP_FIRMWARE_XTREME_UL"
}
else {
    throw "Unable to recognize which firmware to use"
}

Write-Host "Using $firmware_name firmware to run FBT"

$commandline_scons = "$firmware_path/site_scons/commandline.scons"
((Get-Content -Path $commandline_scons -Raw) -replace 'applications_user','..') | Set-Content -Path $commandline_scons -NoNewline

$i = 1
$args_ls = @()
$cpp_defines = @("(`"TOTP_TARGET_FIRMWARE`", `"$FW_CDEF`")")
while ($i -lt $args.Count) {
    if ($args[$i] -eq '-D') {
        $define = $args[$i + 1].Split('=')
        if ($define.Length -gt 1) {
            $cpp_defines += "(`"$($define[0])`", `"$($define[1])`")"
        } else {
            $cpp_defines += "`"$($define[0])`""
        }
        $i = $i + 2
    } elseif ($args[$i] -eq '--clean') {
        $build_path = "$firmware_path/build"
        if (Test-Path -PathType Container $build_path) {
            Remove-Item "$build_path/*" -Recurse -Force
        }
        $i = $i + 1
    } else {
        $args_ls += $args[$i]
        $i = $i + 1
    }
}

if ($cpp_defines.Length -gt 0) {
    $cc_scons = "$firmware_path/site_scons/cc.scons"
    (Get-Content -Path $cc_scons -Raw) -replace "(CPPDEFINES\s*=\s*\[)(([^]]\r?\n?)*)(\])", "`$1`"_GNU_SOURCE`", $($cpp_defines -join ", ")`$4" | Set-Content -Path $cc_scons -NoNewline
}

$builtin_totp_path = "$firmware_path/applications/external/totp"
if ((Test-Path -Path $builtin_totp_path) -eq $True) {
    Remove-Item $builtin_totp_path -Recurse -Force
}

Push-Location $firmware_path

./fbt $args_ls

Pop-Location
Pop-Location