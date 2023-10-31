# Font file format generator script.

param (
    [Parameter(Mandatory)][string]$inputFile,
    [Parameter(Mandatory)][string]$outputFile
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

$inputFileContent = Get-Content -Path $inputFile -Raw

$fontFile = New-Item -Name $outputFile -ItemType File -Force
$fileStream = $fontFile.OpenWrite()
try {
    # Font info
    if ($inputFileContent -match "FontInfo\s*=\s*{\s*`"(.+)`",\s*(\d+).*\s*('.').*\s*('.').*\s*(\d+)") {
        $fileStream.WriteByte([Byte]$Matches[1].Length) # Font name length
        $fileStream.Write([System.Text.Encoding]::ASCII.GetBytes($Matches[1]), 0, $Matches[1].Length) # Font name
        $fileStream.WriteByte([Byte]$Matches[2]) # Character height
        $fileStream.WriteByte([Byte][Char]$Matches[3][1]) # Start character
        $fileStream.WriteByte([Byte][Char]$Matches[4][1]) # End character
        $fileStream.WriteByte([Byte]$Matches[5]) # Width, in pixels, of space character
    } else {
        throw "Unable to find FontInfo data"
    }

    # Font bitmaps
    if ($inputFileContent -match 'Bitmaps\[\]\s*=\s*\{([^}]+)\}') {
        $bitmapsRaw = $Matches[1];
        $bitmapsMatch = $bitmapsRaw | Select-String -Pattern '0x[0-9a-zA-Z]{2}' -AllMatches
        $bitmapBytes = $bitmapsMatch.Matches.Value | ForEach({ [byte]$_ })
        $length = [uint16]$bitmapBytes.Length
        $fileStream.Write((Get-UInt16AsBytes -Value $length), 0, 2) # Bitmap array length
        $fileStream.Write($bitmapBytes, 0, $length) # Bitmap data
    } else {
        throw "Unable to find Bitmap data"
    }

    # Font descriptors
    if ($inputFileContent -match 'Descriptors\[\]\s*=\s*\{([\w\W]+?)\};') {
        $descriptorsRaw = $Matches[1];
        $descriptorsMatch = $descriptorsRaw | Select-String -Pattern '\{\s*(\d+),\s*(\d+)\s*\}' -AllMatches
        $fileStream.WriteByte([Byte]$descriptorsMatch.Matches.Length) # Descriptors array length
        foreach ($match in $descriptorsMatch.Matches) {
            $fileStream.WriteByte([Byte]$match.Groups[1].Value) # Character width
            $offset = [uint16]$match.Groups[2].Value
            $fileStream.Write((Get-UInt16AsBytes -Value $offset), 0, 2) # Character offset
        }
    } else {
        throw "Unable to find Descriptors data"
    }
} finally {
    $fileStream.Dispose()
}