## New changes
* !!! **Warning! After installing, Desktop settings (Favoutite apps, PIN Code, AutoLock time..) will be resetted to default due to settings changes, Please set your PIN code, Favourite apps again in Settings->Desktop** !!!
* Desktop: **New way to set favourite apps and fully configurable dummy mode** (now you can set up to 4 favourite apps!) (port of OFW PR 2972 by nminaylov) (by @gid9798 | PR #578)
* Desktop: Fix lock timer after rebooting (by @gid9798 | PR #578)
* Infrared: Updated universal assets (by @amec0e | PR #581)
* Core: Added proper error message on out of memory crash (by @Willy-JL)
* SubGHz: Fix FAAC SLH add manually issues and fix sending signals with unknown seed
* SubGHz: Temporarily reverted changes from OFW PR 2984: SubGhz: fix todo (by Skorpionm) - Fixes Enhanced Sub-GHz Chat app and various issues related to receiving signals that was found in 061 release

[-> How to install firmware](https://github.com/DarkFlippers/unleashed-firmware/blob/dev/documentation/HowToInstall.md)

[-> Download qFlipper (official link)](https://flipperzero.one/update)

### Thanks to our sponsors:
callmezimbra, Quen0n, MERRON, grvpvl (lvpvrg), art_col, ThurstonWaffles, Moneron, UterGrooll, LUCFER, Northpirate, zloepuzo, T.Rat, Alexey B., ionelife, ...
and all other great people who supported our project and me (xMasterX), thanks to you all!


## **Recommended update option - Web Updater**

### What `n`, `r`, `e`, ` `, `c` means? What I need to download if I don't want to use Web updater?
What build I should download and what this name means - `flipper-z-f7-update-(version)(n / r / e / c).tgz` ? <br>
`flipper-z` = for Flipper Zero device<br>
`f7` = Hardware version - same for all flipper zero devices<br>
`update` = Update package, contains updater, all assets (plugins, IR libs, etc.), and firmware itself<br>
`(version)` = Firmware version<br>
` ` = this build comes with 3 custom animations, and default apps preinstalled (base pack)<br>
`c` = this build comes with 3 custom animations, and only main apps (Clean build like latest OFW)<br>
`n` = this build comes without our custom animations (we have only 3 of them), only official flipper animations, and base pack apps<br>
`e` = build has 🎲 [extra apps pack](https://github.com/xMasterX/all-the-plugins) preinstalled, our custom animations, and base pack apps too<br>
`r` = RGB patch (+ extra apps) for flippers with rgb backlight mod (this is hardware mod!) (Works only on modded flippers!) (do not install on non modded device!)

Firmware Self-update package (update from microSD) - `flipper-z-f7-update-(version).tgz` for mobile app / qFlipper / web<br>
Archive of `scripts` folder (contains scripts for FW/plugins development) - `flipper-z-any-scripts-(version).tgz`<br>
SDK files for plugins development and uFBT - `flipper-z-f7-sdk-(version).zip`



