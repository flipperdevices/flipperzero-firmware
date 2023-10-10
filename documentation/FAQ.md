# FAQ

## I bought Flipper Zero and I don't know what I can do with it, pls help
- Start with reading official main page: https://flipperzero.one/
- Then check out official docs where you can find answers to most questions: https://docs.flipper.net/

## How to use SubGHz Remote app?
1. Open app, press Back button, select New map file
2. Configure signal files and their names for every button (also you can add only one signal and make other buttons empty - just don't select any files for them in config)
3. Save new map file
4. Open map file and select your previously created file
5. Use buttons to send subghz signal files that you selected in map config at step 2

## My desktop (pin, favourites, etc..) (or other) settings was reset to default after update, what to do?
Just configure that settings again, all is fine, and make sure you seen changelogs for the releases that came out after your previous version, when settings struct is changed, settings file are reset after update, this happens only when struct changes is required, so don't assume that settings will be reset in every release, this will happen only in specific ones

## Why is flipper not connecting to chrome?
The most common cause of the flipper not connecting to google chrome is having qFlipper open while trying to connect your flipper. Or having second flipper lab page open at same time.<br>

You must close qFlipper (or other flipper lab web pages) before attempting to connect your flipper to chrome.
    
## Flipper doesn't work! How to restore firmware???

Follow this guide:<br>
https://docs.flipper.net/basics/firmware-update/firmware-recovery


## Useful links and files
Flipper Awesome - place where you can find almost all links that you might need:<br>
https://github.com/djsime1/awesome-flipperzero

## How do I copy files from Github to my Flipper Zero?
Follow this detailed guide: <br>
https://github.com/wrenchathome/flipperfiles/blob/main/_Guides/How2Flipper.pdf


## Where can I find “This file” or “That file” for my flipper?
    
These 2 repos will cover most(99.9%) of your needs:<br>
https://github.com/UberGuidoZ/Flipper/tree/main
<br>
https://github.com/UberGuidoZ/Flipper-IRDB/tree/main

## External Radio: How to connect CC1101 module
https://github.com/quen0n/flipperzero-ext-cc1101

## How to add extra Sub-GHz frequencies
https://github.com/DarkFlippers/unleashed-firmware/blob/dev/documentation/SubGHzSettings.md

## How to use Flipper as new remote (Nice FlorS, BFT Mitto, Somfy Telis, Aprimatic, AN-Motors, etc..)
https://github.com/DarkFlippers/unleashed-firmware/blob/dev/documentation/SubGHzRemoteProg.md

## Can I clone a car key fob for my own car to use flipper as a key?
No, and trying to do so with Read RAW will lead to key desync or unpair with blacklist which means re-pair is very hard and requires service tools

## Will Unleashed FW support car keyfobs decoding, cloning, emulating?
No, never

## Where can I find jamming files?
Nowhere, this is illegal in almost every country in the world

## Your Flipper feels slow and unresponsive?
1. Make sure you using good microSD card from known brand, flipper works with microSD via SPI that means not any microSD will work good even if it works ok with other devices
2. Go into **Settings -> System** and make sure that you have
`Log Level = None`
`Debug = OFF`
`Heap Trace = None`
If some of that settings is set to something different - change it to `None` / `OFF` 
3. Make sure your battery is charged, that can affect performance too

## Flipper crashed, stuck, frozen ?
Reboot it by holding Left + Back buttons

![how to reboot flipper gif, shows how to hold left and back button](https://media.tenor.com/eUbBDDEzmwMAAAAC/flipper-zero-flipper-zero-reboot.gif)


## How to reset forgotten Flipper pin code?

**Disconnect USB Cable if it was connected**
1. Turn off the device - hold back button -> Turn Off 
**If you can't turn it off, try next step but hold buttons for 30-40 seconds)**
2. Hold Up + Back for ~5 sec -> You will see reset screen -> Hold Right to reset (and down arrow to exit if you don't want to reset pin code)
3. Done, internal memory (dolphin level, settings, pin code, is erased to default settings)



## Is there a correct way to capturing Infrared signals?

There is indeed especially with AC units, a new documentation has been released with some notes and steps on capturing infrared signals correctly along with some example data so you are able to understand the difference visually between the two.

https://github.com/DarkFlippers/unleashed-firmware/blob/dev/documentation/InfraredCaptures.md


# NFC/RFID FAQ
From our good friend `@Equip` and  `@np0` <br>
**------------------------------------------------------**

### MIFARE Ultralight

- Scan the card, hold the Flipper Zero up to the reader to get the password to unlock the rest of the sectors, then scan the card again.

### MIFARE DESFire/MIFARE Ultralight C

- The Flipper Zero has no available attacks for this card currently.
 
### Bank cards

- You cannot clone bank cards
- The Flipper Zero cannot emulate bank cards
- The Flipper Zero cannot pretend to be a point of sale machine
 
### Amiibos

- NTAG215. that's it. It's not going on a MIFARE Classic. 
- Currently, you cannot write Amiibos to new physical tags. yet.
 
### HID/iClass

- Picopass iClass can be read using the Picopass reader plugin
- 26bit Picopass can be downgraded to H10301 RFID credentials (note, it is not guaranteed to work if the reader is not configured to read low frequency)
- Readers will need to be configured and have an LF RFID antenna in order to be read. Certain iClass readers are HF only, and do not have the ability to have LF configured
- **Emulation for Picopass** was added on July 26th, and the updated version can be found in latest releases of Unleashed firmware with apps preinstalled, or in official Apps Hub via Flipper Mobile app
- Write support for personalization mode cards is doable with app
- The Seader app and a SAM expansion board < https://www.redteamtools.com/nard-sam-expansion-board-for-flipper-zero-with-hid-seos-iclass-sam/ > will allow reading more secure HID cards, which may be helpful in downgrade attacks
 
### LF-RFID

If you're wanting to make clones of low frequency RFID chips you need to write to T5577's. "Blanks" do not exist. All of the chips the Flipper Zero can interact with are read-only and cannot be overwritten or purchased blank. 
T5577s are multiemulator chips that the Flipper Zero can program to be other tags
 
### Unknown Card/Fob

If you have exhausted all options of scanning via NFC/RFID/PICOPASS then take a photo of:

- The front and back of your credential 
- The reader you use with the credential 
- If your credential is a card, hold it up to a very bright light source e.g. a lightbulb and take a photo of the exposed antenna. This is useful for identification, post it for us to identify!

**------------------------------------------------------**

## How do I access the CLI/Logs?
<blockquote>
  To access the Serial CLI, click one of the following based on your platform.
  <details>
    <summary>Desktop web browser*</summary>
    <em>*Chromium browsers only, such as: Google Chrome, Microsoft Edge, Opera/Opera GX, Brave, and Vivaldi.</em>
    <ul>
      <li>Connect your Flipper via USB.</li>
      <li>Ensure qFlipper and any other serial terminals are closed.</li>
      <li>Open <a href="https://my.flipp.dev/">my.flipp.dev</a> in one of the aforementioned browsers.</li>
      <li>Click <kbd>CONNECT</kbd> and select "USB Serial Device" from the list.</li>
      <li>Wait until you can see your device details on screen.</li>
      <li>Select the 💻 CLI item from the left sidebar.</li>
      <li><strong>Done!</strong></li>
    </ul>
  </details>
  <details>
    <summary>Windows</summary>
    <ul>
      <li>Install <a href="https://www.chiark.greenend.org.uk/~sgtatham/putty/latest.html">PuTTY</a> if it isn't already.</li>
      <li>Connect your Flipper via USB.</li>
      <li>Open qFlipper and look for the COM port next to the Flipper's name. <em>(Should say COM followed by a number, like COM1)</em></li>
      <li>Take note of the COM port number.</li>
      <li><strong>CLOSE qFlipper</strong>, otherwise the next steps won't work.</li>
      <li>Open PuTTY and ensure you're on the Session screen.</li>
      <li>Select "Serial" under connection type.</li>
      <li>Set serial line to the COM port. <em>(Just COM followed by the number, like COM1)</em></li>
      <li>Set speed to <code>115200</code></li>
      <li><em>Optional: Save the session settings for easy connection later.</em></li>
      <li>Finally, click <kbd>Open</kbd> to enter the CLI.</li>
      <li><strong>Done!</strong></li>
      <li>If you get an "Access Denied" error, make sure qFlipper isn't running!</li>
    </ul>
  </details>
  <details>
    <summary>MacOS/Linux</summary>
    <em>Note: I'm a filthy Windows user without any way to verify this procedure. Let me know if it's wrong!</em>
    <ul>
      <li>Install <a href="https://www.gnu.org/software/screen/">GNU Screen</a> if it isn't already.</li>
      <li>Connect your Flipper via USB.</li>
      <li>Open qFlipper and look for the device path next to the Flipper's name. <em>(Starts with /dev/tty)</em></li>
      <li><em>Alternatively: Run <code>ls /dev/tty.*</code> in a terminal.</em></li>
      <li>Take note of the full device path.</li>
      <li><strong>CLOSE qFlipper</strong>, otherwise the next steps won't work.</li>
      <li>Open a terminal.</li>
      <li>Run <code>screen PATH 115200</code>, replacing PATH with the device path from earlier.</li>
      <li><strong>Done!</strong></li>
    </ul>
  </details>
  <details>
    <summary>Android</summary>
    <ul>
      <li>Install <a href="https://play.google.com/store/apps/details?id=de.kai_morich.serial_usb_terminal">Serial USB Terminal</a> if it isn't already.</li>
      <li>Open the app and go to the Connections screen in the hamburger menu <em>(3 bars icon)</em></li>
      <li>Connect your Flipper via USB.</li>
      <li>Click the refresh icon if it doesn't automatically show up.</li>
      <li>Allow Serial USB Terminal to access Flipper if prompted.</li>
      <li>If it doesn't automatically connect, click the connect icon in the upper right. <em>(2 plugs icon)</em></li>
      <li><strong>Done!</strong></li>
      <li><em>Note: To exit log mode, you'll have to disconnect and reconnect using the icon.</em></li>
    </ul>
  </details>
  <details>
    <summary>iPhone</summary>
    Unfortunately, iOS is incapable of accessing a serial terminal over USB; try one of the other methods.
  </details>
  On the Flipper, open the settings, go to System, and set Log Level to Debug. <em>(You can keep Debug set to off unless someone asks you to turn it on)</em>
  Once you have the CLI open, type <code>log</code> and press enter to start watching logs. Press <code>Ctrl-C</code> or <code>Cmd-C</code> to exit log mode.
</blockquote>

<br>
<br>

**CLI FAQ Source + Check out this FAQ for more info:**
https://github.com/djsime1/awesome-flipperzero/blob/main/FAQ.md
