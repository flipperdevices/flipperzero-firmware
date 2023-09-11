## New changes
* **Apple BLE Spam app** (by @Willy-JL | Plus research from ECTO-1A, xMasterX and techryptic) -> (app can be found in builds ` `, `e`, `n`, `r`)
* SubGHz: **FAAC SLH - Programming mode** (by @xMasterX & @Eng1n33r (full research and PoC by @Skorpionm)| PR #585) -> [How to use](https://github.com/DarkFlippers/unleashed-firmware/blob/dev/documentation/SubGHzRemoteProg.md)
* SubGHz: FAAC SLH -> Add manually new options
* SubGHz: **Ignore Princeton** option
* SubGHz: **Save all settings, option to reset config to default** (by @derskythe | PR #590)
* SubGHz: Fix 0xFFFF counter value being skipped
* SubGHz: Fix path reset on save name scene exit
* SubGHz: Various fixes
* SubGHz Remote: New design (by @Svaarich) - Implemented by @gid9798
* SubGHz Remote: Fix Sub-GHz Remote folder name (by @OperKH | PR #583)
* SubGHz Remote: submodule (by @gid9798 | PR #592)
* Infrared: Updated universal assets (by @amec0e | PR #594)
* Infrared: Remake custom universal remotes to use new design (New icons by @Svaarich)
* UI: Keyboard ok to toggle select all in cursor mode (by @Willy-JL)
* CI/CD: CodeQL for internal usage
* CI/CD: Fixed regular builds having `c` in version name in the device info while not being actual `c` build
* Docs: New FAAC SLH instructions
* Docs: Readme & Changelog fixes (by @gid9798 | PR #586 #600)
* OFW: Sub-GHz: fix incorrect key parsing crash
* OFW: fbt: added FW_CFG_name with build configuration
* OFW: SD-Card: proper HAL -> **Breaking API change, API 37.x -> API 38.x** - **Update your apps!**
* OFW: Various Fixes and Improvements -> **Breaking API change, API 36.x -> API 37.x** - **Update your apps!**
* OFW: iButton: Return to the file selection if file is corrupted
* OFW: Account for the "-" in line carry-over
* OFW: github: workflow improvements
* OFW: Storage: force mount
* OFW: Add File Naming setting for more detailed naming -> **Breaking API change, API 35.x -> API 36.x** - **Update your apps!**
* OFW: Disconnect from BLE on protobuf error
* OFW: Add support for Mifare Classic 4k SAK 0x38 ATQA 0x02, 0x04, 0x08
* OFW: Undo some TODO
* OFW: Check the filetype of the update manifest
* OFW: StorageListRequest: size filter 
* OFW: SubGhz: heap overflow text error 
* OFW: nfc: add rfal wrong state error handling
* OFW: Rfid: fix crash on broken key launch from archive (fix was already done in UL in similar way)
* OFW: AC OFF button
* OFW: New IR universal remote graphics
* OFW: Intelligent probing with warnings for fwflash.py
* OFW: FuriHal: explicitly pull display pins at early init stage, move PUPD config to early stage 
* OFW: Fix display last symbol in multiline text
* OFW: Properly reset the NFC device data
* OFW: fbt: various improvements and bug fixes
* OFW: Littlefs updated to v2.7.0
* OFW: loader: restored support for debug apps
* OFW: Removed explicit dependency on scons for external scripting

[-> How to install firmware](https://github.com/DarkFlippers/unleashed-firmware/blob/dev/documentation/HowToInstall.md)

[-> Download qFlipper (official link)](https://flipperzero.one/update)

### Thanks to the UNLEASHED sponsors for our UNLEASHED changes:
callmezimbra, Quen0n, MERRON, grvpvl (lvpvrg), art_col, ThurstonWaffles, Moneron, UterGrooll, LUCFER, Northpirate, zloepuzo, T.Rat, Alexey B., ionelife, ...
and all other great people who supported our project and me (xMasterX), thanks to you all!


### THANKS TO ALL RM SPONSORS FOR BEING AWESOME! THANK YOU TO THE COMMUNITY THAT KEEPS GROWING OUR PROJECT!
