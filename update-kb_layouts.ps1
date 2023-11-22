# Keyboard layouts file format generator script.

param (
    [string]$outputFile = './totp/assets/kb_layouts.klx',
    [string]$cliHelpFile = './totp/assets/cli/cli_help.txt'
)

function Get-UInt16AsBytes() {
    [OutputType([Array])]
    param (
        [Parameter(Mandatory)][uint16]$Value
    )

    $hi = ($Value -shr 8) -band 0xFF
    $lo = $Value -band 0xFF

    return [Array]@($lo, $hi)
}

function ConvertStringToFixedSizeByteArray {
    param (
        [string]$inputString,
        [int]$fixedSize
    )

    $bytesArray = [System.Text.Encoding]::UTF8.GetBytes($inputString)
    $fixedSizeArray = New-Object byte[] $fixedSize
    [System.Array]::Copy($bytesArray, $fixedSizeArray, [System.Math]::Min($bytesArray.Length, $fixedSizeArray.Length))
    return $fixedSizeArray
}

function AreArraysEqual {
    param (
        [array]$array1,
        [array]$array2
    )

    if ($array1.Count -ne $array2.Count) {
        return $false
    }

    for ($i = 0; $i -lt $array1.Count; $i++) {
        if ($array1[$i] -ne $array2[$i]) {
            return $false
        }
    }

    return $true
}

$kbFiles = Invoke-RestMethod -Uri 'https://api.github.com/repos/Flipper-XFW/Xtreme-Firmware/contents/applications/main/bad_kb/resources/badkb/assets/layouts?ref=dev'

$layoutsFile = New-Item -Name $outputFile -ItemType File -Force
$layoutsFileStream = $layoutsFile.OpenWrite()
$tempFile = New-TemporaryFile
$uniqueMaps = [System.Collections.ArrayList]@()
$layouts = [System.Collections.ArrayList]@()
try {
    foreach ($kbFile in $kbFiles) {
        Write-Host "Processing $($kbFile.name)"
        $layoutName = [System.IO.Path]::GetFileNameWithoutExtension($kbFile.name) -replace 'cz_CS', 'cs-CZ'
        Invoke-WebRequest -Uri $kbFile.download_url -OutFile $tempFile
        try {
            $kbFileStream = $tempFile.OpenRead()
            $buffer = New-Object byte[] 72
            $kbFileStream.Position = 96 # Positioning at symbol '0'
            $kbFileStream.Read($buffer, 0, 20) | Out-Null

            $kbFileStream.Position = 130 # Positioning at symbol 'A'
            $kbFileStream.Read($buffer, 20, 52) | Out-Null
            
            $thereIsMatch = $false
            foreach ($map in $uniqueMaps) {
                if ((AreArraysEqual -array1 $map.Map -array2 $buffer) -eq $true) {
                    $thereIsMatch = $true
                    $map.Aliases.Add($layoutName) | Out-Null
                    break
                }
            }

            if (!$thereIsMatch) {
                $uniqueMaps.Add([PSCustomObject]@{
                        Map     = $buffer;
                        Aliases = [System.Collections.ArrayList]@($layoutName);
                    }) | Out-Null
            }
        }
        finally {
            $kbFileStream.Dispose()
        }
    }
   
    $dataStartOffset = 1

    foreach ($map in $uniqueMaps) {
        if ($map.Aliases -contains 'en-US') {
            $map.Aliases = @('QWERTY')
        }
        elseif ($map.Aliases -contains 'fr-FR') {
            $map.Aliases = @('AZERTY')
        }
        elseif ($map.Aliases -contains 'de-DE') {
            $map.Aliases = @('QWERTZ')
        }
        elseif ($map.Aliases -contains 'dvorak') {
            $map.Aliases = @('Dvorak')
        }
        elseif ($map.Aliases -contains 'hu-HU') {
            $map.Aliases = @('Hungarian')
        }
        
        if ($map.Aliases -contains 'cs-CZ') {
            $map.Aliases.Remove('cs-CZ') | Out-Null
            $map.Aliases.Add('Czech') | Out-Null
        }

        if ($map.Aliases -contains 'sk-SK') {
            $map.Aliases.Remove('sk-SK') | Out-Null
            $map.Aliases.Add('Slovak') | Out-Null
        }

        $dataStartOffset += $map.Aliases.Count * 12
    }

    $uniqueMaps | ForEach-Object {
        Write-Host $_.Aliases
    }

    $i = 0
    $aliasOffsets = @{}
    
    foreach ($map in $uniqueMaps) {
        foreach ($alias in $map.Aliases) {
            $aliasOffsets.Add($alias, $dataStartOffset + $i * 72)
        }
        $i = $i + 1
    }

    $layoutsFileStream.WriteByte($aliasOffsets.Count)

    $layoutsOrder = @('QWERTY', 'AZERTY', 'QWERTZ')
    foreach ($alias in $aliasOffsets.Keys | Sort-Object -Property @{ Exp = { if ($layoutsOrder -contains $_) { $layoutsOrder.IndexOf($_) } else { $_ } }; Desc = $false }) {
        $layoutsFileStream.Write((ConvertStringToFixedSizeByteArray -inputString $alias -fixedSize 10), 0, 10)
        $layoutsFileStream.Write((Get-UInt16AsBytes -Value $aliasOffsets.$alias), 0, 2)
        $layouts.Add($alias) | Out-Null
    }
    foreach ($map in $uniqueMaps) {
        $layoutsFileStream.Write($map.Map, 0, $map.Map.Length)
    }
}
finally {
    $layoutsFileStream.Dispose()
    Remove-Item $tempFile -Force
}

$cliHelpFileContent = Get-Content -Path $cliHelpFile -Raw
$cliHelpFileContent = $cliHelpFileContent -replace '(?m)(-k <layout>.+Must be one of: )(.*)$', ('$1' + ($layouts -join ', ') + "`r")
Set-Content -Path $cliHelpFile -Value $cliHelpFileContent -NoNewline -Force -Encoding Ascii | Out-Null
