## New changes
* NFC: CharlieCard parser (by @zacharyweiss)
* SubGHz: Add Manually - Sommer FM fixes
* SubGHz: Enabled tx-rx state on unused gpio pin by default (**external amp option was removed and is enabled by default now**)
* SubGHz: **Status output !TX/RX on the GDO2 CC1101 pin** (by @quen0n | PR #742)
* SubGHz: Reworked saved settings (by @xMasterX and @Willy-JL)
* Desktop: Fixes for animation unload (by @Willy-JL)
* Misc: Added `void` due to `-Wstrict-prototypes`
* Misc: Some code cleanup and proper log levels in nfc parsers
* Infrared: Allow external apps to use infrared settings (by @Willy-JL)
* JS & HAL: Various fixes and FURI_HAL_RANDOM_MAX define added (by @Willy-JL)
* JS: **BadUSB layout support** (by @Willy-JL)
* JS: Module `widget` and path globals (by @jamisonderek)
* Apps: NFC Magic - **Gen2 writing support, Gen4 NTAG password and PACK fixes** (by @Astrrra)
* Apps: MFKey - **fixed crashes** (by @noproto)
* Apps: **Check out Apps updates by following** [this link](https://github.com/xMasterX/all-the-plugins/commits/dev)
* OFW: **Felica poller** (NFC-F)
* OFW: Desktop/Loader: Unload animations before loading FAPs
* OFW: JS Documentation
* OFW: **Update radio stack to v1.19.0**
* OFW: **Move crypto1 to helpers, add it to the public API**
* OFW: Explain RNG differences, add FURI_HAL_RANDOM_MAX
* OFW: Furi: Add "out of memory" and "malloc(0)" crash messages
* OFW: IR: Fix crash on duty_cycle=1
* OFW: **Desktop: ensure that animation is unloaded before app start (fixes some out of memory crashes)**
* OFW: Hide unlock with reader for MFU-C 
* OFW: fbt: fixed missing FBT_FAP_DEBUG_ELF_ROOT to dist env
* OFW: fbt: added -Wstrict-prototypes for main firmware
* OFW: Mifare Ultralight naming fix 
* OFW: IR: Remember OTG state
* OFW: Bad USB: fix crash when selecting a keyboard layout
* OFW: L1_Mods animation update : adding VGM visual 
* OFW: RFID Improvements 
* OFW: Fixed plugins and UI 
* OFW: **NFC: Fix mf desfire detect**
* OFW: infrared_transmit.h was missing `#pragma once`
* OFW: Show the wrong PIN Attempt count on the login screen
* OFW: SavedStruct: Introduce saved_struct_get_metadata
* OFW: JS CLI command
* OFW: Add ChromeOS Bad USB demo
* OFW: **Configurable Infrared TX output** (previous UL version is replaced with OFW version, new features added "AutoDetect" and saving settings)
* OFW: BadUSB: BLE, media keys, Fn/Globe key commands
* OFW: NFC: Slix privacy password reveal ->(was included in previous UL release) and **Desfire detect fix**
* OFW: github: additional pre-upload checks for doxygen workflow
* OFW: NFC UI fixes
* OFW: Gui: unicode support, new canvas API
* OFW: **Api Symbols: replace asserts with checks**
<br><br>
#### Known NFC post-refactor regressions list: 
- Mifare Mini clones reading is broken (original mini working fine) (OFW)
- NFC CLI was removed with refactoring (OFW) (will be back soon)
- Current list of affected apps: https://github.com/xMasterX/all-the-plugins/tree/dev/apps_broken_by_last_refactors
- Also in app **Enhanced Sub-GHz Chat** - NFC part was temporarily removed to make app usable, NFC part of the app requires remaking it with new nfc stack

### Thanks to the UNLEASHED sponsors for supporting UNLEASHED changes!

### THANKS TO ALL RM SPONSORS FOR BEING AWESOME!

# MOST OF ALL, THANK YOU TO THE FLIPPER ZERO COMMUNITY THAT KEEPS GROWING OUR PROJECT!
