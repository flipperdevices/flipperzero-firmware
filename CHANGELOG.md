### New changes
* If you have copied any apps manually into `apps` folder - remove `apps` folder or that specific apps you copied on your microSD before installing this release to avoid issues due to OFW API version update! If you using regular builds or extra pack builds (e) without your manually added apps, all included apps will be installed automatically, no extra actions needed!
* Settings->LCD and Notifications will be resetted to default due to new Contrast setting from OFW
* Core2 (Crash in idle) issues was reduced to current possible minimum, you can try using DeepSleep again (Sleep Method = Default) (+ more checks was added, if you get `Slow HSE/PLL startup` message more than one time, create issue with steps what to do to reproduce it again)
-----
* Plugins: **New RFID 125KHz and iButton Fuzzers (remake from scratch + new features)** (by @gid9798 | PR #507)
* Plugins: SubGHz Bruteforcer -> Time delay (between signals) setting (hold Up in main screen(says Up to Save)) + allow more repeats (by @gid9798 & @xMasterX)
* Plugins: Update TOTP (Authenticator) [(by akopachov)](https://github.com/akopachov/flipper-zero_authenticator)
* Plugins: Unitemp SCD30 support (PR in unitemp repo by @divinebird / fixed by @xMasterX)
* Plugins: Fix ProtoView issue #503 -> (Broken saved files with custom modulation)
* SubGHz: Added 430, 431 MHz to default list
* SubGHz: Remove broken modulation that was causing buffer overrun (fixes issue #506)
* SubGHz: Notifications fixes (by @wosk | PR #464)
* GUI: `Byte input` new feature: editor without keyboard (press Up until you get into new input, then use up/down to input values) (by @gid9798 | PR #509)
* CI/CD: Provide builds with RGB patch for modded flippers (with special led board installed)
* Infrared: `RCA` protocol support
* Infrared: Update universal remote assets - add new ACs and TCL TV
* API: Add furi_hal_version_uid_default (+ Fix TOTP) (by @ClaraCrazy | PR #502)
* OFW PR 2760: NFC: Improvements to NFC Magic app (by AloneLiberty)
* OFW PR 2756: fix: make dialog_file_browser_set_basic_options initialize all fields (by JarvisCraft)
* OFW: Fix reading Mifare Classic cards with unusual access conditions and fix emulation of unknown keys
* OFW: fbt: stable build dates
* OFW: weather_station: add oregon3 with THGR221
* OFW: Services: simplify api (DOLPHIN_DEED->dolphin_deed - function instead of macros + remake all apps in extra pack and main fw to use new API) -> **Breaking API change, API version was changed from 29.x to 30.x**
* OFW: Core2, SRAM2: provide safety gap
* OFW: FuriHal: always clock SMPS from HSI
* OFW: ble: refactored bt gatt characteristics setup (+ remake of BT HID Led descriptor in new way to work with this changes)
* OFW: Scripts: WiFi board updater
* OFW: github: re-enabled f18 build
* OFW: added ISO15693 (NfcV) (was already added before, so we just updated it with latest changes)
* OFW: fbt: added Flipper selection when multiple are connected over USB
* OFW: fbt, ufbt: added checks for appid in app manifests
* OFW: Fix core2 permisions
* OFW: SubGhz: add subghz_protocol_registry external API (was already in our API but in different way)
* OFW: Furi: smaller critical enter and critical exit macro
* OFW: Serial_CLI: Fixing serial cli logger error so it sounds more concise
* OFW: Remove unused resources
* OFW: Dolphin: new animation
* OFW: f7: add PB9 to debug pins
* OFW: Settings: add contrast adjustment -> **Settings->LCD and Notifications will be resetted to default values one time after installing**
* OFW: FuriHal: add system setting to device info, bump device info version

#### [🎲 Download latest extra apps pack](https://github.com/xMasterX/all-the-plugins/archive/refs/heads/main.zip)

[-> How to install firmware](https://github.com/DarkFlippers/unleashed-firmware/blob/dev/documentation/HowToInstall.md)

[-> Download qFlipper (official link)](https://flipperzero.one/update)

### Thanks to our sponsors:
callmezimbra, Quen0n, MERRON, grvpvl (lvpvrg), art_col, ThurstonWaffles, Moneron, UterGrooll, LUCFER, Northpirate, zloepuzo, T.Rat, Alexey B., ionelife, ...
and all other great people who supported our project and me (xMasterX), thanks to you all!

**Note: To avoid issues with .dfu, prefer installing using .tgz with qFlipper, web updater or by self update package, all needed assets will be installed**

**Recommended option - Web Updater**

### What `n`, `r`, `e` means? What I need to download if I don't want to use Web updater?
What means `n` or `e` in - `flipper-z-f7-update-(version)(n / r / e).tgz` ? - `n` means this build comes without our custom animations, only official flipper animations, 
`e` means build has extra apps pack preinstalled,
`r` means RGB patch (+ extra apps) for flippers with rgb backlight mod (this is hardware mod!) (Works only on modded flippers!)

Self-update package (update from microSD) - `flipper-z-f7-update-(version).zip` or download `.tgz` for mobile app / qFlipper / web


