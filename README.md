# FlipBIP - BIP32/39/44
## Crypto toolkit for Flipper Zero
- Built against `0.78.1` Flipper Zero firmware release
- Using Trezor crypto libs from `core/v2.5.3` release
- Included in [RogueMaster Custom Firmware](https://github.com/RogueMaster/flipperzero-firmware-wPlugins)

### DONATE IF YOU FIND THE APP USEFUL
  - ETH (or ERC-20): `xtruan.eth` or `0xa9Ad79502cdaf4F6881f3C2ef260713e5B771CE2`
  - BTC: `16RP5Ui5QrWrVh2rR7NKAPwE5A4uFjCfbs`

## Background

The goal of this project is to see how much crypto functionality can be brought to the Flipper Zero.

## How to install on Flipper Zero
- If you do not have one, download a Flipper Zero firmware to get the `fbt` build tool
- Plug your Flipper Zero in via USB
- Copy the contents of this folder into the `applications_user` folder of your firmware
- Modify the `site_scons/cc.scons` file in the Flipper Zero firmware to add the `"-Os"` flag

Then run the command: 
 ```
./fbt COMPACT=1 DEBUG=0 launch_app APPSRC=applications_user/FlipBIP
 ```
The application will be compiled and copied onto your device

## Status

### Complete

- Trezor crypto C code ported into `crypto` subfolder
  - Adapted to use Flipper hardware RNG (see `crypto/rand.c`)
  - Imports and some C library functions modified for compatibility with FBT
- Navigation and UI adapted from FAP Boilerplate app
- BIP39 mnemonic generation
  - 24, 18, or 12 words
- BIP39 mnemonic to BIP39 seed generation
- Hierarchical Deterministic (HD) wallet generation from seed
  - Generation of offline `m/44'/0'/0'/0` BTC wallet
  - Generation of offline `m/44'/60'/0'/0` ETH wallet (coded from the $SPORK Castle of ETHDenver 2023!)
  - Similar features to: https://iancoleman.io/bip39/
- Saving wallets to SD card
  - Wallets are saved to SD card upon creation in `apps_data/flipbip`
      - NOTE: `apps_data` folder must already exist on SD card!
  - Saved wallets can be viewed between app runs
  - Wallets are encrypted with a randomly generated key, and that key is also encrypted
      - `.flipbip.dat` and `.flipbip.key` files are both required to be in `apps_data/flipbip`
      - Backups of both these files `.flipbip.dat.bak` and `.flipbip.key.bak` are also maintained
      - If you want to externally back up your wallet, I recommend copying all these files, and storing the `key` and `dat` files seperately
  - NOTE: The wallets should be decently tough to crack off of a Flipper, however any Flipper with the app installed can load a wallet in the `apps_data/flipbip` directory if both the `key` and `dat` file are present

### Work in Progress

- More coin types
  - Support for more custom BIP32 wallet paths
  - Currently hardcoded to `m/44'/0'/0'/0` and `m/44'/60'/0'/0`

### (FAR) Future

- Custom wallet security
  - User specified password
- Support for BIP39 passphrase
  - Currently blank
- USB/Bluetooth wallet functionality