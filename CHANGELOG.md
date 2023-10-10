## New changes
**Summary: BLE Core2 (Copro) crashes should be fixed with this update, Apple BLE Spam app and other apps updated to the latest versions, NFC file sort crashes has been fixed, other fixes and improvements see below:**
* SubGHz: Nice Flor S - added custom button code 0x3
* NFC: Fixes out of memory crash if we open folder with more than 300 files in it
* LF RFID: Fixed logic in `t5577_write_with_pass` (by @baugp | PR #612)
* Infrared: Updated universal assets (by @amec0e | PR #607 #619)
* **Apple BLE Spam app** updated to latest version (by @Willy-JL) -> (app can be found in builds ` `, `e`, `n`, `r`)
* OFW: Fix spelling across some project files
* OFW: CCID: Support PC To Reader Transfer Block data
* OFW: Firmware: bigger thread name storage. Notification app: better BacklightEnforce edge cases handling. 
* OFW: Lib: update stm32wb_copro to 1.17.3 release
* OFW: FuriHal ble: length fix for fw version prop 
* OFW: add documentation SubGhz Bin_RAW file format
* OFW: fbt: glob improvements
* OFW: HEX input UI improvements
* OFW: Ble: fix null-ptr dereference in bt_change_profile
* OFW: Add the coding in the shell animation
* OFW: FuriHal,BleGlue: prevent sleep while HCI command executed, proper bt api rpc locking. Fixes random system lockups.
* OFW: fbt: reworked tool path handling
* OFW: Gui: handle view port lockup and notify developer about it
* OFW: Added `fal_embedded` parameter for PLUGIN apps
* OFW: Fix multiline aligned text going out of bounds (again)
* OFW: Add Initial CCID support 
* OFW: Add confirmation before exiting USB-UART
* OFW: Add extended I2C HAL functions -> **Breaking API change 38.x -> 39.x**
* OFW: New clock switch schema, **fixes random core2 crashes**

[-> How to install firmware](https://github.com/DarkFlippers/unleashed-firmware/blob/dev/documentation/HowToInstall.md)

[-> Download qFlipper (official link)](https://flipperzero.one/update)

### Thanks to the UNLEASHED sponsors for our UNLEASHED changes:
callmezimbra, Quen0n, MERRON, grvpvl (lvpvrg), art_col, ThurstonWaffles, Moneron, UterGrooll, LUCFER, Northpirate, zloepuzo, T.Rat, Alexey B., ionelife, ...
and all other great people who supported our project and me (xMasterX), thanks to you all!


### THANKS TO ALL RM SPONSORS FOR BEING AWESOME! THANK YOU TO THE COMMUNITY THAT KEEPS GROWING OUR PROJECT!
