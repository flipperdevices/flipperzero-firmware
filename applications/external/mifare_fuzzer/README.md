# Flipperzero Mifare Fuzzer

### What
This app allows you to fake Mifare UIDs.  
It can emulate either UIDs or full cards from your Flipper's storage.

Currently it supports this kinds of card:
- Classic 1k
- Classic 4k
- Ultralight

### Install
#### FBT

1. Checkout the Flipperzero firmware
2. Go to `applications/plugins/`
3. `git clone` this repo inside directory
4. Return to main firmware directory with `cd ../..`
5. Run `fbt fap_mifare_fuzzer` to compile

#### uFBT (no firmware required)

1. [Install uFBT](https://pypi.org/project/ufbt/)
2. Open your app's directory in Terminal
3. Deploy the app to your Flipper with `ufbt launch`


### License

https://en.wikipedia.org/wiki/WTFPL

    DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
    Version 2, December 2004
