# https://www.github.com/jamisonderek/flipper-zero-tutorials
#
# Pass short 'secret' messages using SSIDs! Tell people your burner phone number while grabbing a coffee.
# Who you gonna call? "5552368" is: AL6fvOPr, BOI5O5Ag, CRU3Cjn7 or DUgPtAbN 
# To reach Jenny      "8675309" is: AOJCghRA, BRVAUlxa, CUhWcF9e or DXtUQJfy
# See the comment near the bottom of this file (line 131) for directions on calculating your own SSID values.
#
# This script will scan for SSIDs that match the pattern of an encoded message & display them in GREEN.
#
# In Marauder:
#  - Clear List, SSID.
#  - SSID, add name.
#  - Enter the SSID name after the "ssid -a -n " prompt.  (For example:  AL6fvOPr)
#  - do List, SSID.  It should show you the SSID that you added.
#  - Beacon Spam, SSID List.
#
# On the PC, Open a PowerShell window and run ./scan.ps1
#

# This is the list of allowed characters in the message.
$dict = "0123456789ABCDEF-"

# This is the list of allowed characters in the SSID.
$cypherLetters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789".ToCharArray()

# This is the list of SSIDs that were found, so we only display them once.
$ssids = [System.Collections.Generic.List[string]]::new()

Function Get-Encoded-SSID {
  Param(
    [Parameter(Mandatory=$true)]
    [String]$Message,
    [Parameter(Mandatory=$false)]
    [String]$Prefix="0"
  )
  $letters = ($Prefix + $Message).ToCharArray()
  $index = 0
  $i = 1
  $result = ""
  foreach($letter in $letters) {
    $ch = $dict.IndexOf($letter)
    if ($ch -eq -1) {
      Write-Host "Invalid character: $letter"
      return
    }
    $index = $index + $ch
    if ($index -ge $cypherLetters.Count) {
      $index = $index - $cypherLetters.Count
    }
    $result = $result + $cypherLetters[$index]
    $i = $i + 1
    $index = $index + (([int][char]$cypherLetters[$index]) * $i)
    $index = $index % $cypherLetters.Count
  }
  return $result
}

Function Get-Decoded-SSID {
  Param(
    [Parameter(Mandatory=$true)]
    [String]$SSID
  )
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

Function Get-SSIDS {
    $networks = (netsh wlan show networks)
    foreach($network in $networks) {
        if($network -match "SSID") {
            $ssid = $network.split(":")[1].trim()
            if ($ssid.Length -gt 0) {
              if ($ssids.Contains($ssid)) {
                continue
              }
              $decoded = (Get-Decoded-SSID $ssid)
              if ($decoded.Length -gt 0) {               
                Write-Host ($ssid + ":                 " + $decoded) -ForegroundColor Green
              } else {
                Write-Host ($ssid + "-         NO MATCH") -ForegroundColor Red
              }
              $ssids.Add($ssid)
            }
        }
    }
}

## Test that Get-Decoded-SSID can get back original string that was passed to Get-Encoded-SSID
# For($i = 0; $i -lt 100000; ++$i) {
#   $r = (""+(Get-Random -Maximum 1000000 -Minimum 0)+(Get-Random -Maximum 1000000 -Minimum 0))
#   $r = $r.Substring((Get-Random) % $r.Length)
#   $p = (""+(Get-Random 9))
#   $d = Get-Decoded-SSID (Get-Encoded-SSID -Message $r -Prefix $p)
#   if ($r -ne $d) {
#     Write-Host ("Fail. r:"+$r+" p:"+$p+" d:"+$d)
#   } else {
# #    Write-Host ("Pass. r:"+$r+" p:"+$p+" d:"+$d)
#   }
# }

# UNCOMMENT THIS LINE TO SEE THE SSID YOU SHOULD USE.
# You can change Message from "5552326" to the desired value using 0-9, 0-F and hyphens.
# The value for Prefix can also be 0-9, A-F.  (So that you can have different SSIDs generated)
#
#   Get-Encoded-SSID -Message "5552368" -Prefix "1"   ## Returns a result of 'BOI5O5Ag'

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