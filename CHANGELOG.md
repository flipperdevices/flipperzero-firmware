## Warning!!! Please read this before installing!!!
**This release has some unresolved issues, if any of those affects your daily usage, stay at 065 release or wait for next releases:** <br>
**Issues from this list will be fixed in next releases**
### Known NFC app regressions and issues: 
- Mifare Classic with custom UID add manually option was temporarily removed (Unleashed)
- Mifare Mini clones reading is broken (original mini working fine) (OFW)
- Mifare Classic dict attack fast skip (multiple presses on OK button) causes glitches/incorrect reading (OFW)
- EMV simple data parser was removed with protocol with refactoring (OFW)
- Option to unlock Slix-L (NFC V) with preset or custom password was removed with refactoring (OFW)
- NFC CLI was removed with refactoring (OFW)
### Some apps that was made for old nfc stack is now not compatible with the new API and require complete remake:
**If you want to help with making this apps work again please send PR to the repo at link below**
- Current list of affected apps: https://github.com/xMasterX/all-the-plugins/tree/dev/apps_broken_by_last_refactors
- Also in app **Enhanced Sub-GHz Chat** - NFC part was temporarily removed to make app usable, NFC part of the app requires remaking it with new nfc stack <br>
**API was updated to v50.x** 
## New changes
* IR: Updated infrared assets (by @amec0e | PR #677)
* NFC: Fix Saflok edge case 0.5% of UIDs got wrong result (by @noproto | PR #668)
* NFC: Zolotaya Korona transport card parser added (by @Leptopt1los)
* NFC: Parsers cleanup for new api (by @Leptopt1los)
* SubGHz: Temp fix for subghz keyboard lock display issue (furi_timer is not working properly)
* SubGHz: Added new option to delete old signals on full memory
* SubGHz: Faac rc/xt add manually (unverified)
* SubGHz: Better subghz history element removal (by @Willy-JL)
* SubGHz: Fix key display newline issue in came atomo
* Apps: **Check out Apps updates by following** [this link](https://github.com/xMasterX/all-the-plugins/commits/dev)
* OFW: USART Bridge: added support for software control of DE/RE pins
* OFW: ufbt: changed toolchain environment invocation; updated .gitignore for app template
* OFW: Keys Dict: fix PVS warnings
* OFW: NfcDict Refactoring
* OFW: Add AC's Carrier 42QG5A580SC and AUX YKR-H/006E
* OFW: NFC Plugins loading rework
* OFW: MFC emulation fix
* OFW: nfc_util: little endian bytes2num functions added
* OFW: Add MyKey parser
* OFW: Update CLI MOTD
* OFW: NFC NTAG and ISO14443-3b reading fix
* OFW: FuriHal: RTC register reset API. New factory reset routine that wipes all RTC backup registers content.
* OFW: FuriHal: various GPIO improvements
* OFW: SubGhz: changed the name of the button when sending RAW to SubGHz

### Thanks to the UNLEASHED sponsors for supporting UNLEASHED changes!

### THANKS TO ALL RM SPONSORS FOR BEING AWESOME!

# MOST OF ALL, THANK YOU TO THE FLIPPER ZERO COMMUNITY THAT KEEPS GROWING OUR PROJECT!
