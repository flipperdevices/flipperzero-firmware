## New changes
* NFC: Zolotaya Korona Online parser added (by @Leptopt1los)
* NFC: Add NFC NDEF parser (by @Willy-JL)
* LF RFID: Write T5577 with random password added (clear password via Extra actions) (by @Leptopt1los)
* SubGHz: Update honeywell protocol (by @Willy-JL)
* Apps: **Check out Apps updates by following** [this link](https://github.com/xMasterX/all-the-plugins/commits/dev)
* OFW: assets: checking limits on image size; ufbt: cdb target
* OFW: NFC: system dict skip when user dict is skipped fix (replaces our fix)
* OFW: FuriHal: fix start duration furi_hal_subghz_async_tx
* OFW: NFC: parsers minor cleanup
* OFW: NFC Ntag success write freeze when not saved card
* OFW: ufbt: fixed generated project paths on Windows 
<br><br>
#### Known NFC post-refactor regressions list: 
- Mifare Mini clones reading is broken (original mini working fine) (OFW)
- EMV simple data parser was removed with protocol with refactoring (OFW)
- Option to unlock Slix-L (NFC V) with preset or custom password was removed with refactoring (OFW)
- NFC CLI was removed with refactoring (OFW)
- Current list of affected apps: https://github.com/xMasterX/all-the-plugins/tree/dev/apps_broken_by_last_refactors
- Also in app **Enhanced Sub-GHz Chat** - NFC part was temporarily removed to make app usable, NFC part of the app requires remaking it with new nfc stack

### Thanks to the UNLEASHED sponsors for supporting UNLEASHED changes!

### THANKS TO ALL RM SPONSORS FOR BEING AWESOME!

# MOST OF ALL, THANK YOU TO THE FLIPPER ZERO COMMUNITY THAT KEEPS GROWING OUR PROJECT!
