param ([Parameter(Mandatory)]$command, [Parameter(Mandatory)]$arg1)

Push-Location $PSScriptRoot

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
    elseif ($matching_firmware.type -eq 'git-release') {
        $release_info = Invoke-RestMethod -Uri "https://api.github.com/repos/$($matching_firmware.git_repo)/releases/$($matching_firmware.git_release)"
        $sdk_uri = ($release_info.assets | Where-Object { $_.name.EndsWith("-sdk.zip") } | Select-Object -Index 0).browser_download_url
        $ufbt_state_file = Join-Path (Resolve-Path "~/") ".ufbt/current/ufbt_state.json"
        $need_update = $true
        if (Test-Path $ufbt_state_file) {
            $ufbt_state = Get-Content $ufbt_state_file -Raw | ConvertFrom-Json
            if ($ufbt_state.url -eq $sdk_uri) {
                $need_update = $false
            }
        }
    
        if ($need_update) {
            $ufbt_command = "ufbt update --url=$sdk_uri --hw-target=f7"
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
