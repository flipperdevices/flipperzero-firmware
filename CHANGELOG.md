## New changes
* NFC: **EMV parser** added (by @Leptopt1los and @wosk | PR #700)
* NFC: Metromoney parser balance fix (by @Leptopt1los | PR #699)
* NFC/LFRFID: Stop emulation after 5 mins to avoid antenna damage (by @Leptopt1los)
* Archive: Fix two filebrowser bugs
* SubGHz: **Programming mode for Dea Mio** (right arrow button)
* SubGHz: **Keeloq fix emulation for multiple systems and extend add manually support** for 2 of them (Dea Mio, Genius Bravo, GSN, Normstahl)
* SubGHz: Fixed hopper state when entering Read via Freq analyzer
* SubGHz: Raw erase fixes (by @Willy-JL)
* SubGHz: Subghz save files with receive time (by @Willy-JL)
* NFC: Fix NFC V dumps with v3 (pre refactor saves) crashing at info page
* NFC: Zolotaya Korona Online parser added (by @Leptopt1los)
* NFC: Add NFC **NDEF parser** (by @Willy-JL)
* LF RFID: **Write T5577 with random and custom password** added (clear password via Extra actions) (by @Leptopt1los)
* SubGHz: Update honeywell protocol (by @Willy-JL)
* System: More contrast values for replacement displays (up to +8 or -8)
* USB/BLE HID: Add macOS Music app volume control
* Apps: **Check out Apps updates by following** [this link](https://github.com/xMasterX/all-the-plugins/commits/dev)
* OFW PR 3384: NFC: Display unread Mifare Classic bytes as question marks - by TollyH
* OFW PR 3396: NFC: **fix application opening from browser** - by RebornedBrain (+ fix for leftover issues)
* OFW PR 3382: NFC UI refactor - by RebornedBrain
* OFW PR 3391: Rework more info scene for Ultralight cards - by RebornedBrain
* OFW PR 3401: it-IT-mac layout - by nminaylov
* OFW: Fix expansion protocol crash when fed lots of garbage
* OFW: 0.98.0-rc various fixes
* OFW: RFID CLI: better usage
* OFW: **Mf DESFire fixes**
* OFW: NFC UI refactor
* OFW: **Expansion module protocol** (+ expansion settings read and store in ram by @Willy-JL) 
* OFW: Bugfix: Strip last parity bit from decoded FDX-B data
* OFW: FuriHal: interrupt priorities and documentation
* OFW: FuriHal: **UART refactoring**
* OFW: SubGhz: add `subghz tx_from_file` CLI cmd, major TX flow refactoring, various improvements and bug fixes
* OFW: Furi_hal_rtc: new function
* OFW: NFC UI refactor 
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
