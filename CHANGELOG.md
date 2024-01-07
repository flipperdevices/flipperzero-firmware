## New changes
* NFC: Various NFC Mifare classic Read fixes (was caused by wrong logic in parsers) (mifare classic 4k, and others) (by @Leptopt1los)
* Apps: Fixed Unitemp and ESP32 Camera suite
* Apps: **Check out Apps updates by following** [this link](https://github.com/xMasterX/all-the-plugins/commits/dev)
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
