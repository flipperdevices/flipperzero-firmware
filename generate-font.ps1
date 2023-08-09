# Font file format generator script.

param (
    [Parameter(Mandatory)][string]$inputFile
)

$inputFileContent = Get-Content -Path $inputFile -Raw

$fontFile = New-Item -Name "$inputFile.font" -ItemType File -Force
$fileStream = $fontFile.OpenWrite()
try {
    # Font info
    if ($inputFileContent -match "FontInfo\s*=\s*{\s*(\d+).*\s*('.').*\s*('.').*\s*(\d+)") {
        $fileStream.WriteByte([Byte]$Matches[1]) # Character height
        $fileStream.WriteByte([Byte][Char]$Matches[2][1]) # Start character
        $fileStream.WriteByte([Byte][Char]$Matches[3][1]) # End character
        $fileStream.WriteByte([Byte]$Matches[4]) # Width, in pixels, of space character
    } else {
        throw "Unable to find FontInfo data"
    }

    # Font bitmaps
    if ($inputFileContent -match 'Bitmaps\[\]\s*=\s*\{([^}]+)\}') {
        $bitmapsRaw = $Matches[1];
        $bitmapsMatch = $bitmapsRaw | Select-String -Pattern '0x[0-9a-zA-Z]{2}' -AllMatches
        $bitmapBytes = $bitmapsMatch.Matches.Value | ForEach({ [byte]$_ })
        $fileStream.Write([System.BitConverter]::GetBytes([Int16]$bitmapBytes.Length), 0, 2) # Bitmap array length
        $fileStream.Write($bitmapBytes, 0, $bitmapBytes.Length) # Bitmap data
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
            $fileStream.Write([System.BitConverter]::GetBytes([Int16]$match.Groups[2].Value), 0, 2) # Character offset
        }
    } else {
        throw "Unable to find Descriptors data"
    }
} finally {
    $fileStream.Dispose()
}