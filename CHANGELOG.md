## New changes
* NFC: EMV Fixes and imporvements (by @wosk & @Leptopt1los | PR #702)
* NFC: Parsers refactoring (by @Leptopt1los)
* NFC: Kazan parser improved - token parse option added (by @Leptopt1los)
* NFC: Update ndef parser, mf classic dict changes (by @Willy-JL)
* Infrared: Update universal remote assets (by @amec0e | PR #718 #719)
* SubGHz: Add 430.50 mhz (by @MizumasuShoichi | PR #721)
* SubGHz: Magellan Event Code Update (by @wooferguy | PR #713)
* SubGHz: Reduce subghz add manually scene flash size (by @Willy-JL)
* SubGHz: Fix led blink on decode raw > signal info (by @Willy-JL)
* HID App: apply fix for ms teams on macos (by @cpressland)
* HID App: merge official fw hid app keyboard changes
* Expansion `is_connected` API to check for VGM (by @HaxSam)
* New JavaScript Modules `UsbDisk`,`badusb.quit()`,`SubGHz`,`Submenu`,`BleBeacon`,`Keyboard`,`Math` (by @Willy-JL, @Spooks4576, @Sil333033)
* Apps: **Check out Apps updates by following** [this link](https://github.com/xMasterX/all-the-plugins/commits/dev)
* OFW: Fix troika 4K keys
* OFW: Archive: Fix item focus after aborting the Delete operation
* OFW: Troyka parser improvements (by UL Team)
* OFW: NFC: Fix washcity plugin verify function being to greedy
* OFW: Parser for Santiago, Chile BIP transit card
* OFW: WiFi board: fixed update script on Windows (unfortunately also Mac and Linux)
* OFW: Gui: reset canvas orientation and frame when entering direct draw mode
* OFW: FBT/uFBT: Enable C++20/GNU23 in VSCode IntelliSense
* OFW: Toolchain fixes
* OFW: Quote $FBT_TOOLCHAIN_PATH to avoid splitting
* OFW: ble: profile rework
* OFW: lfrfid/em4100: added support for different bit rates (16clk was added back into UL, still not reading properly)
* OFW: T5577 lib: write with mask function added
* OFW: Archive: fixed Apps tab ext filter
* OFW: FuriHalRtc refactor: new datetime lib (by UL Team)
* OFW: bit_lib and nfc_util refactor (by UL Team)
* OFW: Gui text box: fix formatted string memory reservation
* OFW: JS debug disabled, archive and file browser fixes
* OFW: VSCode integration fixes for new toolchain
* OFW: FIX ISO15693 emulation
* OFW: JS serial module renamed, uart channel selection
* OFW: mjs: minor fixes
* OFW: **JavaScript runner**
* OFW: Fixed MyKey check LockID
* OFW: Check universal remote files before loading
* OFW: NFC: fix retry scene navigation logic 
* OFW: Expansion module service improvements
* OFW: New toolchain with gcc 12 (+ aarch64 support!)
* OFW: HID app: keyboard modifiers fix
* OFW: CLI: cat command crash workaround
* OFW: NFC: Custom UID entry when adding manually
* OFW: Added NFC plugin; Some parser
* OFW: **Slix disable privacy** (Unlock SLIX-L)
* OFW: NFC: Add support for Gallagher access control (MIFARE Classic only)
* OFW: furi/core/timer: resolve timer handle use-after-free post deletion
* OFW: FuriHal: various GPIO improvements
* OFW: GUI: canvas commit callback has been moved to canvas. Direct Draw apps can now be streamed via RPC.
* OFW: nfc app: fix incorrect protocol detection in save scene (by UL Team)
* OFW: NFC: MFC Unlock with Dictionary
* OFW: ITSO Parser (UK)
* OFW: NFC: fix application opening from browser
* OFW: Rework more info scene for Ultralight cards
* OFW: NFC UI refactor 
* OFW: Add an NFC parser for the San Francisco Bay Area "Clipper" transit card.
* OFW: Fix nfc_protocol_support_scene_save_name_on_event crash
* OFW: NFC: Display unread Mifare Classic bytes as question marks 
* OFW: Troika layout fixes
* OFW: NFC: MF Classic parsers read() fix (dictionary attack skip)
<br><br>
#### Known NFC post-refactor regressions list: 
- Mifare Mini clones reading is broken (original mini working fine) (OFW)
- NFC CLI was removed with refactoring (OFW) (will be back soon)
- Current list of affected apps: https://github.com/xMasterX/all-the-plugins/tree/dev/apps_broken_by_last_refactors
- Also in app **Enhanced Sub-GHz Chat** - NFC part was temporarily removed to make app usable, NFC part of the app requires remaking it with new nfc stack

### Thanks to the UNLEASHED sponsors for supporting UNLEASHED changes!

### THANKS TO ALL RM SPONSORS FOR BEING AWESOME!

# MOST OF ALL, THANK YOU TO THE FLIPPER ZERO COMMUNITY THAT KEEPS GROWING OUR PROJECT!
