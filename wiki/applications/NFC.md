The NFC standard (ISO-14443) operates at 13.56MHz. We are going to use the ST25R3916 chip, the same chip as used in the HydraNFC project.

Supported cards:

* ISO-14443 tag (mifare?) reading & writing & emulation  
* Emulation — Mifare Classic & Ultralight


## Reader mode


## Write mode


## Emulate mode

# USB NFC Reader [Not implemented]
![USB NFC Reader](https://github.com/Flipper-Zero/flipperzero-firmware-community/raw/master/wiki_static/NFC/usb-nfc-reader.png)

There are many use cases that impossible to run directly on Flipper Zero. Most of these cases require powerful CPU for cryptographic attacks:  

- Mifare classic attacks: `mfoc` (Nested), `mfcuk` (Dark Side)
- Mifare Plus attack: Hard Nested

We can use Flipper Zero as a regular USB NFC adapter along with `LibNFC` library, so all existing software will work out of the box without any modifications. This mode must be run from menu `NFC -> USB NFC adapter`. In this mode all commands from PC should be forwarded directly to NFC chip `ST25R3916` via USB serial interface. 

### TODO (USB NFC Reader)

* Write `LibNFC` driver for Flipper's NFC chip `ST25R3916`


# UI

### NFC
* Reader
* Emulate