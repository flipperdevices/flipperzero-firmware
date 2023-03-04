# FlipBIP - BIP32/39/44 Tool
Crypto toolkit for Flipper Zero

## Background

The goal of this project is to see how much crypto functionality can be brought to the Flipper Zero.

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
  - Similar features to: https://iancoleman.io/bip39/

### Work in Progress

- Support for custom BIP32 wallet paths
  - Currently hardcoded to `m/44'/0'/0'/0`

### (FAR) Future

- More coin types
- Saving wallets to disk
- USB/Bluetooth wallet functionality
