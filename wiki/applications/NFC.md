# NFC 


## Card detector (High Frequency)

Кард детектор позволяет определить тип незивестной карты в диапазоне 13,56 MHz (HF). Этот тест не дает точный результат, а позволяет только предположить примерно. В конце теста, если было найдено совпадение, будет предложено перейти в приложение, которое может работать с найденным типом карты. 

Для этого используется рутина:

1. Check if card `ISO-14443` `A` or `B` or FeliCa
3. Check if ATQA(?) last byte indicate ISO-balba compatible card or not
2. Match UID, SAK, ATQA, ATS, ATR from database
3. Try to authenticate as Mifare, EMV, etc..
3. Return founded type and suggest suitable application or return error


## Reader detector (High Frequency)

Unknown reader on the wall можно попробовать определить. Для этого мы можем сэмулировать карту и передавать команды на экран, поняв какую карту ожидает ридер.

1. Silently read WUPA (0x52) or REQA (0x26) without triggering SELECT on reader
2. Emulate card
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
![USB NFC Reader](https://github.com/Flipper-Zero/flipperzero-firmware-community/raw/master/wiki_static/NFC/usb-nfc-reader.png)

There are many use cases that impossible to run directly on Flipper Zero. Most of these cases require powerful CPU for cryptographic attacks:  

- Mifare classic attacks: `mfoc` (Nested), `mfcuk` (Dark Side)
- Mifare Plus attack: Hard Nested

We can use Flipper Zero as a regular USB NFC adapter along with `LibNFC` library, so all existing software will work out of the box without any modifications. This mode must be run from menu `NFC -> USB NFC adapter`. In this mode all commands from PC should be forwarded directly to NFC chip `ST25R3916` via USB serial interface. 

### TODO (USB NFC Reader)

* Write `LibNFC` driver for Flipper's NFC chip `ST25R3916`


# UI
<!--- Menu structure -->
# NFC

## Detect card

- 

<!--- Menu structure end -->

### NFC
* Reader
* Emulate