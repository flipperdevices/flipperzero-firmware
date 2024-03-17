## v0.7.1

Bug fixes and improvements!

- Back to working on latest official firmware ( @GeorgeBotsev )
- Now the Evil Portal and wardrive dumps will be saved to a new directory apps_data/marauder/dumps, and the "-serial" argument for all commands will respect your "save pcaps" app setting ( @Willy-JL )
- You'll also notice an apps_data/marauder/html directory, to support migrating from the standalone Evil Portal ( @RogueMaster )
- Fixes an issue with loading scripts due to upstream toolchain changes ( @Willy-JL , @vollukas )

As always, make sure you're running the latest version of Marauder FW on your wifi board: https://github.com/justcallmekoko/ESP32Marauder/releases/latest

<3 @0xchocolate (cococode)


## v0.7.0

Unified serial update! No more complicated wiring of 2x UART connections - all your boards are supported now, with PCAPs and text both going over just ONE, SINGLE serial channel! Thanks to @Willy-JL for adding this amazing feature and fixing up the app for the new APIs.

Requires Marauder **FW v0.13.7** (or newer) flashed to your wifi board: https://github.com/justcallmekoko/ESP32Marauder/releases/latest

<3 @0xchocolate (cococode)


## v0.6.6

Bug fixes and new BLE spam commands! Thanks to @justcallmekoko for adding the new commands! I also fixed the script user input bug where it would hang after selecting "save".

The BLE spam command format has changed and requires at least **FW v0.13.6**. Make sure you flash the latest release of Marauder FW to your wifi board: https://github.com/justcallmekoko/ESP32Marauder/releases/latest

Note: if you're flashing directly from your flipper using the [ESP Flasher app](https://github.com/0xchocolate/flipperzero-esp-flasher), grab the bin that corresponds to your hardware and only reflash the Firmware partition. (If you have dual boot set up, then choose the slot where you want to install it. Otherwise, reflash FirmwareA only.)

By the way, want to support app development by buying me a coffee? You finally have that option! https://ko-fi.com/cococode

<3 @0xchocolate (cococode)


## v0.6.5

New option to load Evil Portal HTML files from the flipper sd card!


## v0.6.4

swiftpair spam and ESP32 sd card commands: sethtml, ls. Requires a board with BT and sd card support. Thanks to @justcallmekoko for adding the new commands!


## v0.6.3

BT wardriving and Sour Apple! Requires a board with BT support. Thanks to @justcallmekoko for adding the new commands!


## v0.6.2

Happy wardriving! Thanks to @justcallmekoko for adding the gps and wardrive commands!

Instructions to install a GPS module: https://github.com/justcallmekoko/ESP32Marauder/wiki/gps-modification


## v0.6.1

Thanks to @superhedge22 for adding a channel hop option to the sniffpmkid script stage!


## v0.6.0

Evil Portal and LED control! Thanks @justcallmekoko for adding the commands! See Evil Portal workflow here: https://github.com/justcallmekoko/ESP32Marauder/wiki/evil-portal-workflow

This version also removes the in-app ESP flasher, but don't worry! It's grown up into its own app, where development will continue separately: https://github.com/0xchocolate/flipperzero-esp-flasher


## v0.4.0

Added Signal Monitor (thanks @justcallmekoko!) to support new sigmon command in Marauder v0.10.5: https://github.com/justcallmekoko/ESP32Marauder/releases

Added keyboard and +5V support from unleashed (thanks @xMasterX!)
