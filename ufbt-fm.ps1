param ([Parameter(Mandatory)]$command, [Parameter(Mandatory)]$arg1, [switch]$prune)

Push-Location $PSScriptRoot
$ErrorActionPreference = "Stop"

function Get-UfbtState {
    $ufbt_state_file = Join-Path (Resolve-Path "~/") ".ufbt/current/ufbt_state.json"
    if (Test-Path $ufbt_state_file) {
        return Get-Content $ufbt_state_file -Raw | ConvertFrom-Json
    }

    return $null
}

if ($command -eq 'use') {
    $firmwares_raw_text = Get-Content (Resolve-Path './firmwares.json') -Raw
    $firmwares = $firmwares_raw_text | ConvertFrom-Json
    $matching_firmware = ($firmwares.firmwares | Where-Object { $_.alias -contains $arg1 })[0]
    $ufbt_command = ''
    if (!$matching_firmware) {
        throw "Unable to recognize which firmware to use"
    }
    
    if ($matching_firmware.type -eq 'index') {
        $ufbt_command = "ufbt update --index-url=$($matching_firmware.index_url) --channel=$($matching_firmware.channel)"
    }
    elseif ($matching_firmware.type -eq 'git-action') {
        $workflow_runs_uri = "https://api.github.com/repos/$($matching_firmware.git_repo)/actions/workflows/$($matching_firmware.git_action)/runs?per_page=1&status=success"
        if ($matching_firmware.git_branch) {
            $workflow_runs_uri += "&branch=$($matching_firmware.git_branch)"
        }
        if ($matching_firmware.git_event) {
            $workflow_runs_uri += "&event=$($matching_firmware.git_event)"
        }
 
        $workflow_runs = Invoke-RestMethod -Uri $workflow_runs_uri
        $last_success_run = $workflow_runs.workflow_runs | Select-Object -Index 0
        $artifact_dir = Join-Path (Resolve-Path "~/") ".ufbt/.ufbt_fm/$($matching_firmware.git_repo)/$($matching_firmware.git_action)/$($matching_firmware.git_branch)"
        $sdk_zip_path = Join-Path $artifact_dir "sdk-$($last_success_run.id).zip"
        if (-not (Test-Path -Path $sdk_zip_path -PathType Leaf)) {
            New-Item -ItemType Directory -Path $artifact_dir -Force
            $artifacts = Invoke-RestMethod -Uri $last_success_run.artifacts_url
            $artifact = $artifacts.artifacts | Where-Object { $_.name -match $matching_firmware.git_artifact } | Select-Object -Index 0
            Write-Host "New Github action artifacts found ($($last_success_run.html_url))"
            if ($prune) {
                Remove-Item -Path "$artifact_dir/*" -Recurse -Force
            }
            $artifact_archive_path = Join-Path $artifact_dir "artifact.zip"
            Write-Host "Downloading Github action artifacts"
            $artifact_download_uri = "https://nightly.link/$($matching_firmware.git_repo)/suites/$($last_success_run.check_suite_id)/artifacts/$($artifact.id)"
            try {
                $process = Start-Process -FilePath "curl" -ArgumentList $artifact_download_uri, "-L", "-o", $artifact_archive_path -Wait -NoNewWindow -PassThru
                if ($process.ExitCode -ne 0) {
                    throw "cURL exited with non-zero code"
                }
            }
            catch {
                $ProgressPreference = 'SilentlyContinue'
                Invoke-WebRequest -Uri $artifact_download_uri -OutFile $artifact_archive_path -UseBasicParsing
            }
            Write-Host "Extracting Github action artifacts"
            $sdk_filename_pattern = $matching_firmware.sdk_filename_pattern
            if (-not ($sdk_filename_pattern)) { 
                $sdk_filename_pattern = "sdk.*\.zip$" 
            }
            $zip_archive = [System.IO.Compression.ZipFile]::OpenRead($artifact_archive_path)
            try {
                $sdk_zip_entry = $zip_archive.Entries | Where-Object { $_.Name -match $sdk_filename_pattern } | Select-Object -Index 0
                [System.IO.Compression.ZipFileExtensions]::ExtractToFile($sdk_zip_entry, $sdk_zip_path, $true)
            }
            finally {
                $zip_archive.Dispose()
            }
            Remove-Item $artifact_archive_path -Force
        }

        $ufbt_state = Get-UfbtState
        if ($ufbt_state.file_path -ne $sdk_zip_path) {
            $ufbt_command = "ufbt update `"--local=$sdk_zip_path`" --hw-target=f7"
        }
    }
    else {
        $sdk_uri = ''
        if ($matching_firmware.type -eq 'git-release') {
            $sdk_filename_pattern = $matching_firmware.sdk_filename_pattern
            if (-not ($sdk_filename_pattern)) { 
                $sdk_filename_pattern = "sdk.*\.zip$" 
            }
            $release_info = Invoke-RestMethod -Uri "https://api.github.com/repos/$($matching_firmware.git_repo)/releases/$($matching_firmware.git_release)"
            $sdk_uri = ($release_info.assets | Where-Object { $_.name -match $sdk_filename_pattern } | Select-Object -Index 0).browser_download_url
        }
        elseif ($matching_firmware.type -eq 'direct-uri') {
            $sdk_uri = $matching_firmware.uri
        }

        if ($sdk_uri) {       
            $ufbt_state = Get-UfbtState
            if ($ufbt_state.url -ne $sdk_uri) {
                $ufbt_command = "ufbt update `"--url=$sdk_uri`" --hw-target=f7"
            }
        }
    }

    if ($ufbt_command) {
        Invoke-Expression $ufbt_command
    }

    Write-Host "Updated `"$($matching_firmware.description)`" firmware for uFBT"
}
else {
    throw "Unknown command"
}

Pop-Location
