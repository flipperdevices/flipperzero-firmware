# https://www.github.com/jamisonderek/flipper-zero-tutorials
#
# Pass short 'secret' messages using SSIDs!
# For example, burner phone number or YouTube link.
#
# Who you gonna call? "5552368" is: AL6fvOPr, BOI5O5Ag, CRU3Cjn7 or DUgPtAbN 
#
# In PowerShell script, run the ./encode.ps1.
# - Enter the message to encode
# - It will display 8 different SSID values that you can use.
#
# On your Flipper Zero, start Marauder:
#  - Clear List, SSID.
#  - SSID, add name.
#  - Enter the SSID name after the "ssid -a -n " prompt.  (For example:  AL6fvOPr)
#  - do List, SSID.  It should show you the SSID that you added.
#  - Beacon Spam, SSID List.
#
# On the PC, Open a PowerShell window and run ./scan.ps1

# This is the list of allowed characters in the message.
$dict = "0123456789ABCDEF-"

# This is the list of allowed characters in the SSID.
$cypherLetters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789".ToCharArray()

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

$message = Read-Host "Enter the message to encode"
$hex = [System.BitConverter]::ToString([System.Text.Encoding]::ASCII.GetBytes($message)).Replace("-", "")
if ($message -cmatch "[^0-9A-F-]") {
  Write-Host "Using Hex: $hex"
  $message = $hex
}
for ($i = 1; $i -le 8; $i++) {
  $ssid = Get-Encoded-SSID -Message $message -Prefix $i
  Write-Host ("SSID " + $i +": $ssid")
}