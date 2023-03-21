function Get-LatestDirectory {
    param (
        $Path
    )

    Get-ChildItem -Path $Path | Where-Object {$_.PSIsContainer} | Sort-Object LastWriteTime -Descending | Select-Object -First 1
}

$build_commands = @(
    [PSCustomObject]@{
        Name = "Official Dev \ Unleashed";
        FbtSwitch = "od";
        FirmwarePath = "flipperzero-firmware_official_dev";
        ArtifactName = "totp_official-dev_unleashed_fw{FEATURES_SUFFIX}.fap"
    }
    [PSCustomObject]@{
        Name = "Official Stable";
        FbtSwitch = "os";
        FirmwarePath = "flipperzero-firmware_official_stable";
        ArtifactName = "totp_official-stable_fw{FEATURES_SUFFIX}.fap"
    }
)

Push-Location $PSScriptRoot

if (!(Test-Path -PathType Container "build")) {
    New-Item -ItemType Directory -Path "build"
} else {
    Remove-Item "build\*" -Recurse -Force
}

function Features-Configure {
    param (
        [string[]]$enable,
        [string[]]$disable
    )

    $featuresConfigContent = Get-Content "totp/features_config.h" -Raw
    $appManifestContent = Get-Content "totp/application.fam" -Raw

    foreach ($feature in $enable) {
        $featuresConfigContent = $featuresConfigContent -replace "(#undef)(\s+$feature(\s|$)+)", '#define$2'
        [regex]$appManifestFeaturePattern="(#ifdef $feature\r?\n)((.+\r?\n)+)(#\s*endif)"
        $appManifestContent = $appManifestFeaturePattern.Replace($appManifestContent, { param($match)
            return $match.Groups[1].Value + 
            ($match.Groups[2].Value -replace '^(\s*)#(.+)$', '$1$2') + 
            $match.Groups[4].Value
        })
    }

    foreach ($feature in $disable) {
        $featuresConfigContent = $featuresConfigContent -replace "(#define)(\s+$feature(\s|$)+)", '#undef$2'
        [regex]$appManifestFeaturePattern="(#ifdef $feature\r?\n)((.+\r?\n)+)(#\s*endif)"
        $appManifestContent = $appManifestFeaturePattern.Replace($appManifestContent, { param($match)
            return $match.Groups[1].Value + 
            ($match.Groups[2].Value -replace '^(\s*)(.+)$', '#$1$2') + 
            $match.Groups[4].Value
        })
    }

    Set-Content -Path "totp/features_config.h" -NoNewline -Value $featuresConfigContent
    Set-Content -Path "totp/application.fam" -NoNewline -Value $appManifestContent
}

function Build-Run {
    param (
        [string]$FeaturesSuffix
    )

    foreach ($build_command in $build_commands) {
        Write-Host "Building $($build_command.Name)"
        $build_path = Join-Path -Path $build_command.FirmwarePath -ChildPath "build"
        if (Test-Path -PathType Container $build_path) {
            Remove-Item "$build_path\*" -Recurse -Force
        }

        ./fbt $build_command.FbtSwitch COMPACT=1 DEBUG=0 VERBOSE=0 fap_totp

        $latest_dir = Get-LatestDirectory -Path $build_path
        $build_output_artifact = "build\$($build_command.ArtifactName -replace '{FEATURES_SUFFIX}',$FeaturesSuffix)"
        Copy-Item "$build_path\$latest_dir\.extapps\totp.fap" -Destination $build_output_artifact

        Write-Host "Artifacts for $($build_command.Name) stored at $build_output_artifact"
    }
}

Write-Information 'Building with all the features enables'
Features-Configure -enable TOTP_BADBT_TYPE_ENABLED,TOTP_BADBT_TYPE_ICON_ENABLED
Build-Run -FeaturesSuffix ''

Write-Information 'Building with BadBT but without BadBT icon'
Features-Configure -disable TOTP_BADBT_TYPE_ICON_ENABLED
Build-Run -FeaturesSuffix '_badbt-wo-icon'

Write-Information 'Building without BadBT'
Features-Configure -disable TOTP_BADBT_TYPE_ENABLED,TOTP_BADBT_TYPE_ICON_ENABLED
Build-Run -FeaturesSuffix '_no-badbt'

Features-Configure -enable TOTP_BADBT_TYPE_ENABLED,TOTP_BADBT_TYPE_ICON_ENABLED

Pop-Location