# FlipBIP - BIP39 Tool
Crypto toolkit for Flipper Zero

## Background

The goal of this project is to see how much crypto functionality can be brought to the Flipper Zero.

## Status

### Complete

- Trezor crypto C code ported into `crypto` subfolder
  - Adapted to use Flipper hardware RNG (see `crypto/rand.c`)
  - Imports and some C library functions modified for compatibility with FBT
- Basic navigation and UI adapted from FAP Boilerplate app
- BIP39 mnemonic generation supported!
  - 24, 18, or 12 words
- BIP39 mnemonic to BIP39 seed generation

### Work in Progress

- WIP: BIP39 seed passphrase support
- WIP: BIP39 to BIP32 HDNode
  - possible memory issue

### (Far) Future

- Airgapped wallet functionality
- USB/Bluetooth wallet functionality
