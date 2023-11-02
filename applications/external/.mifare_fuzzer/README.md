# Flipperzero Mifare Fuzzer

### What
This app allows you to fake Mifare UIDs.  
It emulates only the UID of the card, it does not emulate the full card, but it seems to be enough...

Currently it does support this kinds of card:
- Classic 1k
- Classic 4k
- Ultralight

### Install
To compile you must be familiar with the Flipperzero firmware.
1. Checkout the Flipperzero firmware
2. go to `applications/plugins/`
3. `git clone` this repo inside directory
4. return to main firmware directory with `cd ../..`
5. run `fbt fap_mifare_fuzzer` to compile

### License

https://en.wikipedia.org/wiki/WTFPL

    DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
    Version 2, December 2004
