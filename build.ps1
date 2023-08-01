param (
    [switch]$doNotClearBuildFolder = $false
)

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
        ArtifactName = "totp_official-dev_fw{FEATURES_SUFFIX}.fap";
    }
    [PSCustomObject]@{
        Name = "Official Stable";
        FbtSwitch = "os";
        FirmwarePath = "flipperzero-firmware_official_stable";
        ArtifactName = "totp_official-stable_fw{FEATURES_SUFFIX}.fap";
    }
    [PSCustomObject]@{
        Name = "Xtreme \ Unleashed";
        FbtSwitch = "x";
        FirmwarePath = "flipperzero-firmware_xtreme";
        ArtifactName = "totp_xtreme_unleashed_fw{FEATURES_SUFFIX}.fap";
    }
)

Push-Location $PSScriptRoot

if (!(Test-Path -PathType Container "build")) {
    New-Item -ItemType Directory -Path "build"
} elseif (!$doNotClearBuildFolder) {
    Remove-Item "build/*" -Recurse -Force
}

function Build-Run {
    param (
        [string]$FeaturesSuffix,
        [string[]]$CppDefine,
        [string]$Subfolder
    )

    foreach ($build_command in $build_commands) {
        Write-Host "Building $($build_command.Name)"
        $build_path = Join-Path -Path $build_command.FirmwarePath -ChildPath "build"

        $fbt_args = @($build_command.FbtSwitch, "COMPACT=1", "DEBUG=0", "VERBOSE=0", "fap_totp", "--clean")
        if ($CppDefine.Length -gt 0) {
            $CppDefine | ForEach-Object {
                $fbt_args += '-D'
                $fbt_args += $_
            }
        }

        Invoke-Expression -Command "./fbt.ps1 $fbt_args"

        $latest_dir = (Get-LatestDirectory -Path $build_path).Name
        $build_output_folder = "build"
        if ($Subfolder -ne $null -and $Subfolder -ne '') {
            $build_output_folder = Join-Path $build_output_folder $Subfolder
            if (!(Test-Path -PathType Container $build_output_folder)) {
                New-Item -ItemType Directory -Path $build_output_folder
            }
        }

        $build_output_artifact = Join-Path $build_output_folder "$($build_command.ArtifactName -replace '{FEATURES_SUFFIX}',$FeaturesSuffix)"

        Copy-Item "$build_path/$latest_dir/.extapps/totp.fap" -Destination $build_output_artifact

        Write-Host "Artifacts for $($build_command.Name) stored at $build_output_artifact"
    }
}

Write-Information 'Building with all the features enables'
Build-Run -FeaturesSuffix ''

Write-Information 'Building without BadBT'
Build-Run -FeaturesSuffix '_no-badbt' -CppDefine TOTP_NO_BADBT_AUTOMATION

$checksum_file = 'build/checksums.sha256'
New-Item $checksum_file -ItemType File -Force
Get-ChildItem -Path 'build/*.fap' | ForEach-Object {
    $checksum = (Get-FileHash $_ -Algorithm SHA256).Hash.ToLower()
    $filename = $_.Name
    "$checksum  $filename" >> $checksum_file
}

Pop-Location