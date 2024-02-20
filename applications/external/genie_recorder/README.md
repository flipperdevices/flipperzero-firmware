# Genie garage door recorder

## Overview

Version 3.x no longer requires changes to the firmware! The application includes the Genie protocol encoder/decoder, thanks to @MMX for the suggestion.

- [Warnings](#warnings)
- [Connecting to remote](#connecting-to-remote)
- [Capture codes](#capture-codes)
- [Sending signals](#sending-signals)
- [.GNE file format](#gne-file-format)

This program was written to allow the Flipper Zero to press a button on a Genie garage door remote and record the rolling code. The goal is to capture all 65,536 signals (which hopefully repeats when it gets to the end). Our click speed is current 2000ms per click + however long it takes to get the signal. In practice, it typically takes 2 days to record all of your signals.

The application also has the ability to send the next code to the garage door opener. This starts from the first captured signal.

This application is intended to **replace** the remote you captured. You should not use your original remote again once you are using the Flipper Zero or they will get out of sync. It is recommended that you capture a backup remote you are not using.

The files are stored in `SD Card/apps_data/genie`.  You can copy the `007F1991.gne` file to your Flipper if you want to use a default remote.  On your receiver you will need to sync to the Flipper.


## Warnings

WARNING: For my remote, the codes wrapped after 65,536 codes were sent. I'm not sure if this is the case for ALL Genie brand remotes. If it doesn't wrap, it's possible that the remote could stop working (if the manufacture uses OVR bits).

WARNING: This could desync your remote from the receiver. Be sure you know the process to sync the remote back to the receiver. For my remote, I had to press the learn button on the receiver, then press the button on the remote.

WARNING: Don't run this app near your garage. There is no reason to open the physical garage door & you will likely burn out the motor.

## Connecting to remote

<img src="wiring.png" width="45%"/>
<img src="wiring-2.jpg" width="40%"/>
<p/>

WARNING: Do this **at your own risk**. You could damage your remote or Flipper Zero if done improperly.

- Step 1. Remove the case of your garage door remote.
- Step 2. Connect GND from Flipper to GND pin on the remote (Shown in green traces for my remote).
- Step 3. Connect A7 from Flipper to the signal pin on the remote (Shown in orange traces - top button, cyan - middle button for my remote).
- Step 4. Put in a fresh battery.
  - Risky Option: Remove the battery and connect 3V3 from Flipper to the battery positive bar on the remote. Be 100% sure that GND on the Flipper is going to GND on the remote (and not the signal pin) and that no wires are shorting. If you are not 100% sure, then DON'T DO THIS! You could damage the remote and the Flipper Zero.

## Capture codes

- Step 1. Be sure you have followed the steps in the [Connecting to remote](#connecting-to-remote) section.
- Step 2. Be sure you are not near your garage door (or that the door is unplugged). The application will be pushing the button many times and could burn out the motor!
- Step 3. Run the Genie Recorder application.
- Step 4. Choose `Config` and set the frequency to the frequency your remote runs at. If you don't know, start with `315MHz` then use `390MHz` if it doesn't work. NOTE: You do not need to do anything with the `Genie File` setting at this time.
- Step 5. Press `Back` button to return to the main application.
- Step 6. Choose `Learn` to start capturing and learning signals.
  - `Remaining code` - how many codes still need to be captured.
  - `Click` - how many times a click has been sent to the remote.
  - `Prev` - how many codes were previously captured (no codes captured this session yet).
  - `Got` - total number of codes captured.
  - 16 digit hex code - the last code that was captured.
- Step 7. If you get the message `NO SIGNAL FROM REMOTE?` see the [troubleshooting](#troubleshooting) section.
- Step 8. Let it run for 2-3 days (the goal is to capture at least 50,000 signals). If you capture less, it will still work but when it wraps back to the beginning those codes will be rejected by your garage door opener and you would have to resync. To successfully wrap, the remote needs 50,000 button presses. It is recommended you capture all of the codes.
- Step 9. Press the BACK button to return to the main application.

## Sending signals

- Step 1. Be sure you have followed the steps in the [Capturing codes](#capture-codes) section.
- Step 2. Run the Genie Recorder application.
- Step 3. Choose `Config` and set the frequency to the frequency your remote runs at.
- Step 4. Choose `Genie File` and select the .GNE file that was saved during the [Capture codes](#capture-codes) section steps.
- Step 5. Press `Back` button to return to the main application.
- Step 6. Choose `Send` to load the send screen.
  - `NO .GNE FILE LOADED` - Confirm you selected a valid file in step 4.
  - `Remote not fully captured` - You should continue to [capture signals](#capture-codes) but you can use the application.
  - `Press OK to send signal` - Press the OK button to send the next code to the garage door opener.
  - `Long press OK to reset code.` - This will desync the Flipper from the receiver. It will start over, transmitting the first code.

## Troubleshooting

- If the LED on the remote is not blinking, be sure you have followed the steps in the [Connecting to remote](#connecting-to-remote) section.

- If the application is not detecting the remote, but the LED on the remote is blinking, then ensure the frequency is correct.

- If sending signal is not working, then ensure the frequency is correct.

## .GNE file format

The file format is as follows:

- 2 bytes: "G\*" (0x47 0x2A)
- 1 byte: Major version (0x02)
- 1 byte: Minor version (e.g. 0x05)
- 4 bytes: Fix code from remote (matches filename.GNE)
- 2 bytes: Last sent index
- 2 bytes: Last saved index (0xFFFF and contents at 0xFFFF indicates full capture)
- 4 bytes: Reserved for future use.
- 4 bytes x 65536: The dynamic part of the code (0x00000000 if no code has been received)

The file can be found in the `SD Card\apps_data\genie` folder on your Flipper Zero. The name of the file will match the ending 8 characters of the fix code.
