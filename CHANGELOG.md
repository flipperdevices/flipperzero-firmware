### New changes
* BLE: Revert BLE gatt characteristics refactoring temporarily -> **Should fix HID issues on older iOS, and maybe some issues with android app**
* SubGHz: Added external cc1101 module at CLI (by @Sil333033 & @xMasterX | PR #513)
* SubGHz: Remove unused global var
* Plugins: Fix ProtoView issue #503 again -> (Broken saved files with custom modulation)
* OFW: furi_hal_nfc: fix rfalTransceiveBitsBlockingTx's 4th argument to bits count rather than bytes count
* OFW: FuriHal: remove clock startup time tracking from clean builds

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


