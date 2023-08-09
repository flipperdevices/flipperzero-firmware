Push-Location $PSScriptRoot
git submodule update --remote --force flipperzero-firmware_xtreme
git submodule update --remote --force flipperzero-firmware_official_dev
git submodule update --remote --force flipperzero-firmware_official_stable
Pop-Location