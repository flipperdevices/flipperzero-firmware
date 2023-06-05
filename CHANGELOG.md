### New changes
* If you have copied any apps manually into `apps` folder - remove `apps` folder or that specific apps you copied on your microSD before installing this release to avoid issues due to OFW API version update! If you using regular builds or extra pack builds (e) without your manually added apps, all included apps will be installed automatically, no extra actions needed!
-----
* Only in release 052 -> **Multiple Extra pack apps was fixed!** -> TAMA P1, Flizzer Tracker, Video Player, Music Tracker
* NFC V: Remove delay from emulation loop. This improves compatibility when the reader is Android.
* Plugins: iButton Fuzzer -> Fix v2 key files load (all new saved files)
### Previous changes
* SubGHz Remote: Fixed BinRAW support, + many other fixes (by @gid9798 | PR #492)
* SubGHz: Fix KL: Stilmatic support + add manually support
* SubGHz: Keeloq mfname refactoring (by @gid9798 | PR #479)
* Desktop Clock: Some improvements and fixes (by @gid9798 | PR #490)
* LF RFID: Cleanup duplicated code (by @gid9798 | PR #493)
* NFC V: Code review fixes + some GUI rework (by @nvx & @xMasterX)
* NFC V: Fixed crash when exiting emulation and starting it again
* Infrared: Use Universal AC Remote from OFW, same for Audio remote, and rename buttons in OFW naming scheme
* Infrared: Update universal remote assets (by @amec0e)
* GUI Keyboard: Fix crash when renaming files with long file name (Fixed issue #489)
* Misc: Fix APP_IDs to match new regex (regex check will be added in OFW soon)
* Plugins: Protoview, WAV Player, DTMF Dolphin - fixed all known crashes, ported to latest hal bus system
* Plugins: ESP8266 Deauther - Crash fix (Fixed issue #497)
* Plugins: Update -> Mifare Nested [(by AloneLiberty)](https://github.com/AloneLiberty/FlipperNested)
* Plugins: Update -> TOTP (Authenticator) [(by akopachov)](https://github.com/akopachov/flipper-zero_authenticator)
* Plugins: Update -> ESP32: WiFi Marauder companion plugin [(by 0xchocolate)](https://github.com/0xchocolate/flipperzero-wifi-marauder)
* Plugins: Update -> UART Terminal [(by cool4uma)](https://github.com/cool4uma/UART_Terminal/tree/main)
* OFW: FuriHal: disable bus re-initialization on early init and extra asserts for AHB1,AHB2,AHB3 which must be left intact on entering to FUS -> **Fixes updater error 1-100**
* OFW: NFC: fix MFC timings -> **Fixes issues with Mifare Classic emulation that could happen after unlshd-049 release**
* OFW: Update dolphin.py
* OFW: NFC Magic: Fix gen1 writing with invalid BCC (lost fix from PR 2511)
* OFW: SubGhz: fix flipper crashes after exiting broadcast blocking message and crash cli
* OFW: Dolphin: new animation
* OFW: fbt: added hooks for build & dist environments; added FW_ORIGIN_* macro for apps & SDK 
* OFW: FuriHal: add bus abstraction -> **Breaking API change, API version was changed from 27.x to 28.x** 
* OFW: Implement support for reading Opal card (Sydney, Australia) 
* OFW: BadUSB: script execution pause
* OFW: IR Universal AC: Add Carrier 42QHB12D8S

#### [🎲 Download latest extra apps pack](https://github.com/xMasterX/all-the-plugins/archive/refs/heads/main.zip)

[-> How to install firmware](https://github.com/DarkFlippers/unleashed-firmware/blob/dev/documentation/HowToInstall.md)

[-> Download qFlipper (official link)](https://flipperzero.one/update)

### Thanks to our sponsors:
callmezimbra, Quen0n, MERRON, grvpvl (lvpvrg), art_col, ThurstonWaffles, Moneron, UterGrooll, LUCFER, Northpirate, zloepuzo, T.Rat, Alexey B., ionelife, ...
and all other great people who supported our project and me (xMasterX), thanks to you all!

**Note: To avoid issues with .dfu, prefer installing using .tgz with qFlipper, web updater or by self update package, all needed assets will be installed**

**Recommended option - Web Updater**

What means `n` or `e` in - `flipper-z-f7-update-(version)(n / e).tgz` ? - `n` means this build comes without our custom animations, only official flipper animations, 
`e` means build has extra apps pack preinstalled

Self-update package (update from microSD) - `flipper-z-f7-update-(version).zip` or download `.tgz` for mobile app / qFlipper


