# Marauder SSID secret messages

## Overview
*secret_ssid* allows you to pass short *secret messages* using SSIDs.  For example, you can tell someone nearby your burner phone number or YouTube link.

## Typical usage
Let's say you needed to tell a friend the A-Team's mobile phone number is "5556162".  Rather than write it on a napkin at a hotdog stand, you can walk into the coffee shop and use Marauder on your Flipper zero to do a *Beacon spam* with any of the following SSIDs: AL6fvOPr, BOI5O5Ag, CRU3Cjn7 or DUgPtAbN.  If you think someone is carefully watching the SSIDs, can you use an expanded ssid format like "ATTCRU3Cjn7", so it looks just like an AT&T router. 

Your friend just needs to run the PowerShell script on their laptop to get your message.  They don't even need to buy a Flipper -- which might help them save enough to _hire the A-Team_.

## Encoding the ssid

In PowerShell script, run the ./encode.ps1 file.
- Enter the message to encode
- It will display 8 different SSID values that you can use.

## Sending the SSID on the Flipper Zero
On your Flipper Zero, start Wifi Marauder:
- Clear List, SSID. (You may need to use left/right to choose SSID)
- SSID, add name. (You may need to use left/right to choose add name)
  - The "ssid -a -n " prompt should appear.
  - Enter the name of the SSID, for example: AL6fvOPr
  - The prompt should be "ssid -a -b AL6fvOPr"
  - Click OK
  - Click BACK
  - Click BACK
- List, SSID. (You may need to use left/right to choose SSID)
  - It should show you the SSID that you added.
- Beacon Spam, SSID List. (You may need to use left/right to choose SSID)

Make sure you are in the coffee shop long enough for the laptop to pick up the signal.

## Decoding the SSID on the PC
On the PC:
- Start a PowerShell window. (You can search for "Windows PowerShell" app) or in Visual Studio code, you can right click and choose "Open in Integrated Terminal".
- Make sure you are in the secret_ssid directory.
  - use *dir* or *ls* to see the directory.
  - use cd ..  to go up one directory
  - use cd secret_ssid to go into secret_ssid.
- Run the following command:
   ```
   ./scan.ps1
   ```
- The script will start dumping out nearby SSIDs
  - If the SSID is not a secret message, it will be shown in red.
  - If the SSID is a secret message, it will be shown in green.
    - The decoded message will displayed next to the SSID.

## How it works
If your message is a hex value, then it will be encoded directly.  If you message is ASCII text, then it will first be encoded into hex. For example, "Hello" becomes "48656C6C6F".

For each SSID we prefix the SSID number to the front, so SSID 1 is
"148656C6C6F".

Encoding "148656C6C6F" should give "BNH135GWLhO".

The following encoding bar is used: "0123456789ABCDEF-"
The following encoding strip is used:
 "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"

Step 1. Initially, the bar and the strip are aligned.

```
  0123456789ABCDEF-
  ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789
```

Step 2. We find the first value to encode ("1" is the first value in "148656C6C6F")in the encoding bar & write down the letter in the encoding strip (which is 'B').

```
   v
  0123456789ABCDEF-
  ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789
```

Step 3. Move the encoding strip so it is aligned with the letter we encoded.

```
   v
   0123456789ABCDEF-
  ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789
```

Step 4. We take the ASCII value of the encoding strip ('B' is 66) and multiply it by how many letters we have encoded plus 1. = 66*(1+1) = 132. We move the encoding strip forward that many spaces (wrapping as needed.)  ASCII: "A"=64+1=65, "a"=65+32=97, "0"=48.

You can divide by 62 (the encoding strip length) and use the remainder (e.g. keep subtracting 62 until you get a small value.)  So 132%62 = 8, so we advance 8 spaces (so our encoding bar starts over the J).  NOTE: The encoding bar wraps around to the front if needed.

```
           0123456789ABCDEF-
  ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789
```

Step 5. (Repeat steps 2-4). We find the value to encode ("4" is the next value in "148656C6C6F")in the encoding bar & write down the letter in the encoding strip (which is 'N').

```
               v
           0123456789ABCDEF-
  ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789
```

Move the strip:
```
               v
               0123456789ABCDEF-
  ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789
```

N is 78.  78*(2+1)=234.  234%62 = 48
Slide the strip forward 48 spots, wrapping the 1 around:
```
  123456789ABCDEF-                                             0
  ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789
```

8 is next value in 148656C6C6F.  So we have an H.
```
         v
  123456789ABCDEF-                                             0
  ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789
```

Repeat until we have created the full SSID:
BNH135GWLhO


Decoding is a similar process.  If the encoding bar is over the encoding strip and the SSID has a letter not there, we know the SSID is not a secret message.  For example if the SSID was "BNT..." we know it's not valid, since the third character needs to have the encoding bar over it.  We can see from the previous step, the only valid 3rd character for an SSID starting with "BN" are (9 and A-P).

## Security
Using these scripts to encode data is probably similar security to using a [Master Lock(R) lock](https://www.youtube.com/@lockpickinglawyer/videos) to secure a door.  Data encoded with a [Ceasar cipher](https://en.wikipedia.org/wiki/Caesar_cipher) can be decrypted by someone that wants to.