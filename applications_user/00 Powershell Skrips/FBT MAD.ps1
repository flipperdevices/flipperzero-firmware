 #Beschreibung:
<#
Beschreibung:
Programm zum erstellen von FAP Dateien und flashen auf den Flipper Zero.
 #>
 #Änderungen:
 <#
13.02.2023 
            letzte Befehl wiederholen eingefügt.
16.02.2023 
            faps unter Punkt 4
23.02.2023 
            Variabeln mit {} und.$ geändert.
            Verzeichniswahl hinzugefügt.
24.02.2023 
            Verzeichniswahl mit Platzhalter
            set-Location 'C:\Users\*\OneDrive\Visual Studio 2022'
26.02.2023 
            Beim Start zuerst das Verzeichnis abfragen und ändern.
            Beim Start abfragen ob das Verzeichnis geändert werden soll mit J oder N.
            Letzte Namenseingabe wiederholen.
            Letzten Befehl mit W wiederholen einfügen.
27.02.2023
            Firmware über USB auf Flipper laden. ./fbt flash_usb
28.02.2023
            Manuelle Eingabe hinzufügen.
03.03.2023
            Ordner über eingabe mit Nummern auswählen.
            Firmware in Visual Studio öffnen.

06.03.2023
            Beschreibung zu den Befehlen hinzufügen.

12.03.2023
            V Verzeichnis wie am start mit Nummerneingabe auswählen.
            R Restart hinzufügen.
            Ordner auswählen von denen der Code compiliert werden soll um anschließend das Projekt in VSC zu öffnen.
            Gleiche Befehle als Funktionen ablegen zur leichteren Verständnis.
12.03.2023
            Test:
            01 I.O.
            02 I.O.
            03 I.O.
            04 I.O.
            05 I.O.
            06 I.O.
            07 I.O.
            08 I.O.
            09 I.O.
            00 I.O.
            C  I.O.
            M  I.O.
            W  I.O
            V  I.O.
            R  I.O.
            Q  I.O.

 #>

 <#Offene Punkte: 
 
 Bei der Abfrage der $appname Auswahl mit Nummerneingabe hinzufügen.

 #>

 

 Set-ExecutionPolicy -Scope Process -ExecutionPolicy RemoteSigned
<#
Befehlsliste die geändert werden kann:
#>

$fbt1 = {./fbt} 
$fbt2 = {./fbt fap_$name}#app_name"
$fbt3 = {./fbt launch_app APPSRC=applications_user/$name} #app_name"
$fbt4 = {./fbt fap_dist}
$fbt5 = {./fbt updater_package}
$fbt6 = {./fbt faps}
$fbt7 = {./fbt flash_usb}
$fbt8 = {"Firmware in Visual Studio Code öffnen"}
$fbt9 = {./fbt_options.py}

#Hilfetext

$text1 = "Erstellt komplette Firmware"
$text2 = "Erstellt FAP Datei nach Eingabe von app_name. (app_name unter appid in application.fam zu finden)"
$text3 = "Erstellt eine FAP auf den Flipper und startet diese nach dem Flashen. Der Flipper muss im Ruhezustand sein"
$text4 = "Erstellt ein externen Plug-In und speichert es im dist Ordner"
$text5 = "Erstellt ein Update Paket. Die Minimalversion enthält nur die DFU-Datei der Firmware; Die Vollversion enthält auch einen Radiostack und Ressourcen für die SD-Karte. "
$text6 = "Erstellt ext. Apps und Plug-In als faps."
$text7 = "Erstellt lädt und instaliert eine komplette Firmware auf den Flipper."
$text8 = "Öffnet den ausgewählten Ordner im Visual Studio Code zur Bearbeitung."
$text9 = "Öffnet die ./fbt_opions in Python zur Hilfestellung."



# ./fbt Befehlsliste:
$fbtl1 = "$fbt1 "
$fbtl2 = "$fbt2 "
$fbtl3 = "$fbt3 "
$fbtl4 = "$fbt4 "
$fbtl5 = "$fbt5 "
$fbtl6 = "$fbt6 "
$fbtl7 = "$fbt7 "
$fbtl8 = "$fbt8 "
$fbtl9 = "$fbt9 "

# ./fbt Liste:
$fbtlist ="
##############################Debugen über ./fbt. Muss im Hauptpfad ausgeführt werden##################################
#######################################################################################################################

1 $fbtl1 $text1
2 $fbtl2 $text2
3 $fbtl3 $text3
4 $fbtl4 $text4
5 $fbtl5 $text5
6 $fbtl6 $text6
7 $fbtl7 $text7
8 $fbtl8 $text8
9 $fbtl9 $text9
0 Seite löschen
C Ordner in VSC öffnen
M Manuele Befehlseingabe
W Letzten Befehl wiederholen
V Firmware Verzeichnis wählen
R Reset Skript
Q Beenden

#######################################################################################################################
"

<# Funktionen zur besseren Übersicht:
#>
<# get-firmware-directory: Auswahl des Ordners mit der Firmwareversion über Nummerneingabe 
   get-restart: Neustart des Powershell Skrips.
   get-directory: Auswahl des Ordners über Nummerneingabe
#>
    function get-firmware-directory {
    set-Location 'C:\Users\*\OneDrive\Visual Studio 2022'
    $Ordner = Get-ChildItem -Directory
# Füge die Zeilennummerierung hinzu
    for ($i = 0; $i -lt $Ordner.Count; $i++) {
    $Ordner[$i] | Add-Member -MemberType NoteProperty -Name 'Zeile' -Value ($i + 1)
    }

# Gib die Tabelle aus
    $Ordner | Format-Table Zeile, Name
    $Auswahl = Read-Host "Wählen Sie einen Ordner aus (1-$($Ordner.Count))"
    $Ordner[$Auswahl -1] # lädt den gewählten Ordner aus der Variablen
    Set-Location -LiteralPath $Ordner[$auswahl -1]
    $Ordner = Get-Location
    Write-Host "Aktuelle pfad $Ordner" -ForegroundColor Green
    }

    function get-restart {
    Write-Host "Skript neu starten? J/N" -ForegroundColor Yellow
    $read = Read-Host
    if ($read -eq "j"){
    & $PSCommandPath
    exit}
    }

    function Get-Directory {
    $Ordner = Get-ChildItem -Directory
    # Füge die Zeilennummerierung hinzu
    for ($i = 0; $i -lt $Ordner.Count; $i++) {
        $Ordner[$i] | Add-Member -MemberType NoteProperty -Name 'Zeile' -Value ($i + 1)
    }

    # Gib die Tabelle aus
    $Ordner | Format-Table Zeile, Name

    # Wähle einen Ordner aus und zeige seine Unterordner an
    do {
        $Auswahl = Read-Host "Wählen Sie einen Ordner aus (1-$($Ordner.Count)) oder geben Sie 'B' ein, um zu bestätigen"
        if ($Auswahl -ne "B") {
            Set-Location -LiteralPath $Ordner[$Auswahl -1].FullName

            # Zeige nur Ordner an und nummeriere sie durch
            $Unterordner = Get-ChildItem -Directory
            for ($i = 0; $i -lt $Unterordner.Count; $i++) {
                $Unterordner[$i] | Add-Member -MemberType NoteProperty -Name 'Zeile' -Value ($i + 1)
            }
            $Unterordner | Format-Table Zeile, Name

            # Setze die Auswahl zurück für den nächsten Durchlauf
            $Auswahl = ""
            
            # Aktualisiere die Liste der Ordner für den nächsten Durchlauf
            $Ordner = Get-ChildItem -Directory
        }
    } while ($Auswahl -ne "B")
}

# Select-Folder zu testzwecken:
<#
    function Select-Folder {
    param(
        [string]$Path = (Get-Location).Path
    )
    $folders = Get-ChildItem -Path $Path | Where-Object { $_.PSIsContainer }
    Write-Host "Wählen Sie einen Ordner aus:"
    for ($i = 0; $i -lt $folders.Count; $i++) {
        Write-Host "$($i + 1): $($folders[$i].Name)"
    }
    Write-Host "Z: Zurück"
    $input = Read-Host
    if ($input -eq 'Z') {
        Set-Location ..
        Select-Folder
    } elseif ([int]::TryParse($input, [ref]$null)) {
        Set-Location (Join-Path -Path $Path -ChildPath $folders[[int]$input - 1].Name)
        Select-Folder
    } else {
        Write-Error "Ungültige Eingabe"
        Select-Folder
    }
    
    return (Get-Location).Path
}

   # $selectedFolder = Select-Folder
    #>

    <# Starte das Programm:#>
    cls
    Write-Host "Wähle das Arbeitsverzeichis" -ForegroundColor Green
    get-firmware-directory
    

while(($inp = Read-Host -Prompt "$fbtlist") -ne "Q"){
    switch ($inp) {
    
    1 {
    Write-Host "$fbtl1" -ForegroundColor Green
    Write-Host "Komplettes Firmware wird erstellt." -ForegroundColor Green
    & $fbt1
    $rpt = $fbt1
    }

    2 {
    Write-Host "Letzten Befehl wiederholen? J" -ForegroundColor Yellow
    $read = read-host
    if($read -eq "J"){
    & $fbt2
    $rpt = $fbt2}
    else {
    Write-Host "$fbtl2" -ForegroundColor Green
    Write-Host "Gib den Namen der App ein" -ForegroundColor Yellow
    $name = Read-Host
    & $fbt2
    $rpt = $fbt2}
    }

    3 {
    Write-Host "Letzten Befehl wiederholen? J" -ForegroundColor Yellow
    $read = read-host
    if($read -eq "J"){
    & $fbt3
    $rpt = $fbt3}
    else {
    Write-Host "fbtl3" -ForegroundColor Green
    Write-Host "Gib den Namen des Ordners ein, von der App die gestartet werden soll." -ForegroundColor Yellow
    $name = Read-Host
    & $fbt3
    $rpt = $fbt3
    }
    }

    4 {
    Write-Host "$fptl4" -ForegroundColor Green
    & $fbt4
    $rpt = $fbt4
    }

    5 {
    & $fbt5
    $rpt = $fbt5
    }

    6 {
    & $fbt6
    $rpt = $fbt6
    }

    7 {
    & $fbt7
    $rpt = $fbt7
    }

    8 {
    & $fbt8
    $rpt = $fbt8
    code $ordner
    }

    9 {
    & $fbt9
    $rpt = $fbt9}
    
    0 {
    cls
    }

    c {
    Write-Host "Verzeichnis wählen das in Visual Studio Code geöffnet werden soll" -ForegroundColor Green
    Set-Location 'C:\Users\*\OneDrive\Visual Studio 2022'
    Get-Directory
    Write-Host "$(Get-Location) `nMit beliebiger Taste in VSC öffnen." -ForegroundColor Green
    Write-Host "Mit Taste A Abbrechen." -ForegroundColor Red
    $read = Read-Host

    if ($read.ToUpper() -ne "A"){
    code (Get-Location)
    }
    }
    m {
    Write-Host "Gib den Befehl ein der Manuel ausgeführt werden soll" -ForegroundColor Green
    $eingabe = Read-Host
    Invoke-Expression $eingabe
    }
    w {
    & $rpt
    }

    V {
    get-firmware-directory
    }

    R {
    get-restart
    }

    Q {
    "Beenden"
    }
    }
    }
    
    