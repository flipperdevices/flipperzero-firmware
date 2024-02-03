write-host "Installation aktuelle Firmware:"
$folder = Read-Host "Gib den Ordnernamen ein."
Write-Host "Der Ordner $folder wird erstellt"
git clone --recursive https://github.com/flipperdevices/flipperzero-firmware.git $folder
Write-Host "Download der Firmware fertig. Ordner $folder wurde erstellt."
cd .\$folder
Write-Host ".\fbt wird gestartet"
.\fbt
Write-Host ".\fbt Fertig."
Write-Host ".\fbt vscode_dist wird gestartet"
.\fbt vscode_dist
Write-Host ".\fbt vscode_dist Fertig"
$read = Read-Host "In VSC öffnen ? Mit j bestätigen!"
if ($read -eq "j"){
    code .\$folder
}
