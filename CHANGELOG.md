## New changes
* Apps: **Mifare Nested - ported to latest API** using old nfc lib (by @xMasterX) (original app made by @AloneLiberty) (+ mem management fix by @Willy-JL) - [Python app running on PC is required](https://github.com/AloneLiberty/FlipperNestedRecovery)
* LFRFID: **Electra fix** non-initialized encoded epilogue on render (by @Leptopt1los)
* JS: Move examples to subfolder `js_examples`
* Apps: HID app improvements and fixes<br>
`- Move new mouse jiggler into mouse jiggler stealth and bring back previous version of mouse jiggler too`<br>
`- Set stealth jiggler max time default value to 4 min and min value to 1 min`<br>
`- Merge OFW changes`<br>
`- More OFW merge fixes` (by @Willy-JL | PR #753)<br>
* Apps: **Check out more Apps updates and fixes by following** [this link](https://github.com/xMasterX/all-the-plugins/commits/dev)
* OFW (TLSF branch): SubGHz: fix memory corrupt in read raw view
* OFW: **NFC App: fix changing UID**
* OFW: Replaced obsolete-format delay
* OFW: **Archive: fix condition race on exit**
* OFW: Text Box: fix displaying text with end text focus
* OFW: FuriHal: add flash ops stats, workaround bug in SHCI_C2_SetSystemClock
* OFW: Icons: compression fixes & larger dimension support
* OFW: **Text Box rework**
* OFW: Fix calling both `view_free_model()` and `view_free()`
* OFW: JS: Add textbox module
* OFW: JS: Add math module
* OFW: **NFC: add Slix capabilities**
* OFW: Settings refactor fixes
* OFW: JS: Add submenu module
* OFW: **Skylanders plugin**
* OFW: Settings menu refactoring 
* OFW: NFC: Mf Desfire fix reading big files 
* OFW: Infrared: Add Toshiba RAS-2518D 
* OFW: **vscode: config fixes**
* OFW: Ble: new connection parameters negotiation scheme
* OFW: FuriHal: move version init to early stage 
* OFW: Add support for R_ARM_REL32 relocations.
* OFW: Remove unused DolphinWait_61x59 icon
* OFW: Add the Akira animation
* OFW: **Desktop: fix crash on autolock after restart in locked state**
<br><br>
#### Known NFC post-refactor regressions list: 
- Mifare Mini clones reading is broken (original mini working fine) (OFW)
- NFC CLI was removed with refactoring (OFW) (will be back soon)

### THANKS TO ALL RM SPONSORS FOR BEING AWESOME!

# MOST OF ALL, THANK YOU TO THE FLIPPER ZERO COMMUNITY THAT KEEPS GROWING OUR PROJECT!
