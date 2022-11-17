function Get-LatestDirectory {
    param (
        $Path
    )

    Get-ChildItem -Path $Path | Where-Object {$_.PSIsContainer} | Sort-Object LastWriteTime -Descending | Select-Object -First 1
}

$build_commands = @(
    [PSCustomObject]@{
        Name = "Official Dev";
        FbtSwitch = "od";
        FirmwarePath = "flipperzero-firmware_official_dev";
        ArtifactName = "totp_official-dev-fw.fap"
    }
    [PSCustomObject]@{
        Name = "Official Stable";
        FbtSwitch = "os";
        FirmwarePath = "flipperzero-firmware_official_stable";
        ArtifactName = "totp_official-stable-fw.fap"
    }
    [PSCustomObject]@{
        Name = "Unleashed";
        FbtSwitch = "u";
        FirmwarePath = "flipperzero-firmware_unleashed";
        ArtifactName = "totp_unleashed-fw.fap"
    }
)

Push-Location $PSScriptRoot

if (!(Test-Path -PathType Container "build")) {
    New-Item -ItemType Directory -Path "build"
} else {
    Remove-Item "build\*" -Recurse -Force
}

foreach ($build_command in $build_commands) {
    Write-Host "Building $($build_command.Name)"
    $build_path = Join-Path -Path $build_command.FirmwarePath -ChildPath "build"
    if (Test-Path -PathType Container $build_path) {
        Remove-Item "$build_path\*" -Recurse -Force
    }

    ./fbt $build_command.FbtSwitch COMPACT=1 DEBUG=0 VERBOSE=0 fap_totp

    $latest_dir = Get-LatestDirectory -Path $build_path
    $build_output_artifact = "build\$($build_command.ArtifactName)"
    Copy-Item "$build_path\$latest_dir\.extapps\totp.fap" -Destination $build_output_artifact

    Write-Host "Artifacts for $($build_command.Name) stored at $build_output_artifact"
}

Pop-Location