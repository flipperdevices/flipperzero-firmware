# https://www.github.com/jamisonderek/flipper-zero-tutorials
#
# Pass short 'secret' messages using only SSID names! 
# For example, burner phone number or YouTube link.
#
# This script will scan the network for SSIDs.  If any
# SSIDs are encoded message, they will be displayed
# in GREEN.  Non-matching are shown in RED.
#
# For creating SSIDs, please see encode.ps1
#
# On the PC, Open a PowerShell window and run ./scan.ps1
#

# This is the list of allowed characters in the message.
$dict = "0123456789ABCDEF-"

# This is the list of allowed characters in the SSID.
$cypherLetters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789".ToCharArray()

# This is the list of SSIDs that were found, so we only display them once.
$ssids = [System.Collections.Generic.List[string]]::new()

Function Get-Decoded-SSID {
  Param(
    [Parameter(Mandatory=$true)]
    [String]$SSID
  )
  # If the SSID starts with "ATT" skip that part of the SSID.  :)
  if ($SSID.StartsWith("ATT")) {
    $SSID = $SSID.Substring(3)
  }

  $letters = $SSID.ToCharArray()
  $index = 0
  $i = 1
  $result = ""
  foreach($letter in $letters) {
    $ch = $cypherLetters.IndexOf($letter)
    if ($ch -lt $index) {
      $ch = $ch + $cypherLetters.Count
    }
    if (($ch - $index) -gt $dict.Length) {
#      echo "Invalid character: $letter"
      return
    }
    $result = $result + $dict[$ch - $index]
    $i = $i + 1
    $index = $ch
    if ($index -ge $cypherLetters.Count) {
      $index = $index - $cypherLetters.Count
    }
    $index = $index + (([int][char]$cypherLetters[$index]) * $i)
    $index = $index % $cypherLetters.Count
  }
  return $result.Substring(1)
}

Function Show-Available-Networks-UI {
  explorer.exe ms-availablenetworks:
  Start-Sleep 3
}

Function Get-Text-From-Hex {
  Param(
    [Parameter(Mandatory=$true)]
    [String]$hex
  )
  if ($hex -cmatch "[^0-9A-F]") {
    return
  }
  return [System.Text.Encoding]::ASCII.GetString(($hex -split '(..)' | Where-Object { $_.Length -eq 2 } | ForEach-Object { [Convert]::ToByte($_, 16) }))
}

Function Get-SSIDS {
    $networks = (netsh wlan show networks)
    foreach($network in $networks) {
        if($network -match "SSID") {
            $ssid = $network.split(":")[1].trim()
            if ($ssid.Length -gt 0) {
              if ($ssids.Contains($ssid)) {
                continue
              }
              $decodedHex = (Get-Decoded-SSID $ssid)
              if ($decodedHex.Length -gt 0) {                
                $decodedText = Get-Text-From-Hex($decodedHex)
                Write-Host ($ssid + ":                 " + $decodedHex + "   " + $decodedText) -ForegroundColor Green
              } else {
                Write-Host ($ssid + "-         NO MATCH") -ForegroundColor Red
              }
              $ssids.Add($ssid)
            }
        }
    }
}

while ($true) {
  Show-Available-Networks-UI
  For( $i = 0; $i -le 12; ++$i ) {
    Get-SSIDS
    Write-Host "Sleeping..." -ForegroundColor Yellow -NoNewline
    Start-Sleep 9
    Write-Host "`b`b`b`b`b`b`b`b`b`b`b" -NoNewline
    Write-Host "           " -NoNewline
    Write-Host "`b`b`b`b`b`b`b`b`b`b`b" -NoNewline
    Write-Host "Scanning..." -ForegroundColor Yellow -NoNewline
    Start-Sleep 1
    Write-Host "`b`b`b`b`b`b`b`b`b`b`b" -NoNewline
  }
}