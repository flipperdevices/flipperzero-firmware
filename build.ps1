param (
    [Parameter(Mandatory)][string[]]$FbtSwitch,
    [switch]$doNotClearBuildFolder = $false
)

$build_commands = @{
    od = [PSCustomObject]@{
        Name         = "Official Dev";
        ArtifactName = "totp_official-dev_fw{FEATURES_SUFFIX}.zip";
    }
    os = [PSCustomObject]@{
        Name         = "Official Stable";
        ArtifactName = "totp_official-stable_fw{FEATURES_SUFFIX}.zip";
    }
    xs = [PSCustomObject]@{
        Name         = "Xtreme (stable)";
        ArtifactName = "totp_xtreme_fw{FEATURES_SUFFIX}.zip";
    }
    xd = [PSCustomObject]@{
        Name         = "Xtreme (dev)";
        ArtifactName = "totp_xtreme_dev_fw{FEATURES_SUFFIX}.zip";
    }
    ul = [PSCustomObject]@{
        Name         = "Unleashed";
        ArtifactName = "totp_unleashed_fw{FEATURES_SUFFIX}.zip";
    }
}

Push-Location $PSScriptRoot

if (!(Test-Path -PathType Container "build")) {
    New-Item -ItemType Directory -Path "build"
}
elseif (!$doNotClearBuildFolder) {
    Remove-Item "build/*" -Recurse -Force
}

function Build-Run {
    param (
        [string]$FeaturesSuffix,
        [string[]]$CppDefine,
        [string]$Subfolder,
        [string]$FbtSwitch
    )
    $build_command = $build_commands[$FbtSwitch]
    Write-Host "Building $($build_command.Name)"
    $build_path = Join-Path -Path $PSScriptRoot -ChildPath "totp/dist"

    $fbt_args = @($FbtSwitch, "--clean")
    if ($CppDefine.Length -gt 0) {
        $CppDefine | ForEach-Object {
            $fbt_args += '-D'
            $fbt_args += $_
        }
    }

    Invoke-Expression -Command "./ufbt.ps1 $fbt_args"

    $build_output_folder = "build"
    if ($Subfolder -ne $null -and $Subfolder -ne '') {
        $build_output_folder = Join-Path $build_output_folder $Subfolder
        if (!(Test-Path -PathType Container $build_output_folder)) {
            New-Item -ItemType Directory -Path $build_output_folder
        }
    }

    $build_output_artifact = Join-Path $build_output_folder "$($build_command.ArtifactName -replace '{FEATURES_SUFFIX}',$FeaturesSuffix)"

    $zip_folder = Join-Path $build_output_folder ".zip"
    if (!(Test-Path -PathType Container $zip_folder)) {
        New-Item -ItemType Directory -Path $zip_folder
    }
    elseif (!$doNotClearBuildFolder) {
        Remove-Item "$zip_folder/*" -Recurse -Force
    }

    $zip_app_folder = Join-Path $zip_folder "apps/Tools"
    New-Item $zip_app_folder -ItemType Directory -Force

    Copy-Item "$build_path/totp.fap" -Destination $zip_app_folder

    $zip_plugins_folder = Join-Path $zip_folder "apps_data/totp/plugins"
    New-Item $zip_plugins_folder -ItemType Directory -Force

    Copy-Item "$build_path/*.fal" -Destination $zip_plugins_folder

    Compress-Archive -Path "$zip_folder/*" -DestinationPath $build_output_artifact

    Remove-Item $zip_folder -Recurse -Force

    Write-Host "Artifacts for `"$($build_command.Name)`" stored at `"$build_output_artifact`""
}

foreach ($fbts in $FbtSwitch) {
    Write-Information 'Building with all the features enables'
    Build-Run -FeaturesSuffix '' -FbtSwitch $fbts

    Write-Information 'Building without BadBT'
    Build-Run -FeaturesSuffix '_no-badbt' -CppDefine TOTP_NO_BADBT_AUTOMATION -FbtSwitch $fbts
}

Pop-Location
