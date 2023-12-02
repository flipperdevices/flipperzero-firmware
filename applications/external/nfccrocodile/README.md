# NFC Crocodile

Simple "Guess the word" game based on NFC technology and Flipper Zero.


## Requirements

- Flipper Zero
- FW 0.84.2 

## Build

1. Clone [flipperdevices/flipperzero-firmware](https://github.com/flipperdevices/flipperzero-firmware) as described in it's documentation.
1. Checkout `flipperzero-firmware` on tag corresponding FW on your device.
1. Clone this repository into `flipperzero-firmware/application_user`
1. Deploy on flipper: `fbt launch_app APPSRC=nfc_crocodile`

## Known issues

Word for game is sent as NFC NDEF Type T record. This type of record is not suitable for iOS-based smartphones.