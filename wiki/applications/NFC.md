# NFC 

## Card detector

<img width="800" src="./../../wiki_static/NFC/nfc-card-detector.png" />

Card type is often unknown. Card detector runs tests against the unknown card to determine it's type. These tests are not 100% accurate, but they help to start exploring. If the test finished successfully, it can recommend to run a suitable application for the card type.

### Card detector routine:

1. Check if card `ISO-14443` `A` or `B` or `FeliCa`
2. Check the 6 byte of `SAK` to determine if `ISO-14443-4` compliant
3. Match combination of UID, SAK, ATQA, ATS, ATR from database
4. Try to authenticate as Mifare, EMV, etc..
5. Return founded type and suggest suitable application or return error

## Reader detector

<img width="800" src="./../../wiki_static/NFC/nfc-wall-reader.png" />

Wall readers usually looks the same, but may accept various types type of cards. With reader detector feature we can emulate dummy card on Flipper and sniff commands that reader send to card.  


### Reader detector

1. Silently read WUPA (0x52) or REQA (0x26) without triggering SELECT on reader
2. Emulate dummy card
    2.1 Answer on ATQA
    2.2 Answer on SELECT 
    2.3 ...
3. Consistently emulate popular cards before valid authentication from reader found 

## Reader mode

* Mifare classic reader  _How to select dictionary?_
  * Brute force
  * Save dump
  * Write dump to SD-card
* Mifare Ultralight reader
  * Save dump
  * Write dump?
* EMV reader

## Write mode

## Emulation 

# USB NFC Reader [Not implemented]
<img width="800" src="./../../wiki_static/NFC/usb-nfc-reader.png" />

There are many use cases that impossible to run directly on Flipper Zero. Most of these cases require powerful CPU for cryptographic attacks:  

- Mifare classic attacks: `mfoc` (Nested), `mfcuk` (Dark Side)
- Mifare Plus attack: Hard Nested

We can use Flipper Zero as a regular USB NFC adapter along with `LibNFC` library, so all existing software will work out of the box without any modifications. This mode must be run from menu `NFC -> USB NFC adapter`. In this mode all commands from PC should be forwarded directly to NFC chip `ST25R3916` via USB serial interface. 

# Schematic

<img src="./../../wiki_static/NFC/ST25R3916-schematic.png" />


# UI
<!--- Menu structure -->
# NFC

## Detect card

- 
<!--- Menu structure end -->
