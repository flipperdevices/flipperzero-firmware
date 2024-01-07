## Warning!!! Please read this before installing!!!
**This release has some unresolved issues, if any of those affects your daily usage, stay at 065 release or wait for next releases:** <br>
**Issues from this list will be fixed in next releases**
### Known NFC app regressions and issues: 
- Mifare Mini clones reading is broken (original mini working fine) (OFW)
- EMV simple data parser was removed with protocol with refactoring (OFW)
- Option to unlock Slix-L (NFC V) with preset or custom password was removed with refactoring (OFW)
- NFC CLI was removed with refactoring (OFW)
### Some apps that was made for old nfc stack is now not compatible with the new API and require complete remake:
**If you want to help with making this apps work again please send PR to the repo at link below**
- Current list of affected apps: https://github.com/xMasterX/all-the-plugins/tree/dev/apps_broken_by_last_refactors
- Also in app **Enhanced Sub-GHz Chat** - NFC part was temporarily removed to make app usable, NFC part of the app requires remaking it with new nfc stack <br>
**API was updated to v50.x** 
## New changes
* NFC: Skip system dict bug fixed (by @Leptopt1los)
* NFC: Set ATQA scene bit numbering changed (by @Leptopt1los)
* NFC: Added plugin to read WashCity card balance (by @yaba | PR #683)
* NFC: Add manually MF Classic with custom UID (by @Leptopt1los | PR #690)
* NFC: Fix MyKey production date parsing by [@augustozanellato](https://github.com/flipperdevices/flipperzero-firmware/pull/3332/files)
* Apps: Move hid and snake apps into main repo (will be included in `c` builds)
* Docs: Remove weird newline in applications/ReadMe.md (by @Eczbek | PR #688)
* SubGHz: Proper fix for subghz keyboard lock display issue (thanks @Willy-JL)
* SubGHz: Use long press to exit transmitter (to avoid unwanted 2 buttons hold condition, holding arrow button and exit causes default button change, which is stays as hidden feature, but this change makes it harder to call it accidentally)
* Apps: **Check out Apps updates by following** [this link](https://github.com/xMasterX/all-the-plugins/commits/dev)
* OFW: Desktop: fix rpc unlock on pin input screen 
* OFW: UI refactor
* OFW: MFC emulation fixes
* OFW: Scripts: fix incorrect handling of storage stress test count option
* OFW: Add Samsung AC remotes DB93 and AR-EH04 
* OFW: Update mf_classic_dict.nfc
* OFW: Nfc: HID MFC Plugin
* OFW: RPC: reverse input 
* Update slideshow pictures by @Svaarich

### Thanks to the UNLEASHED sponsors for supporting UNLEASHED changes!

### THANKS TO ALL RM SPONSORS FOR BEING AWESOME!

# MOST OF ALL, THANK YOU TO THE FLIPPER ZERO COMMUNITY THAT KEEPS GROWING OUR PROJECT!
