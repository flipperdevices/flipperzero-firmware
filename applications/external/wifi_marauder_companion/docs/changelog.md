## v0.6.2

Happy wardriving! Thanks to @justcallmekoko for adding the gps and wardrive commands!

Instructions to install a GPS module: https://github.com/justcallmekoko/ESP32Marauder/wiki/gps-modification

Make sure you flash the latest release of Marauder FW to your wifi board: https://github.com/justcallmekoko/ESP32Marauder/releases/tag/v0.11.1

Note: if you're flashing directly from your flipper using the ESP Flasher app, grab the bin that corresponds to your hardware and only reflash the Firmware partition. (If you have dual boot set up, then choose the slot where you want to install it. Otherwise, reflash FirmwareA only.)


## v0.6.1

Thanks to @superhedge22 for adding a channel hop option to the sniffpmkid script stage!


## v0.6.0

Evil Portal and LED control! Thanks @justcallmekoko for adding the commands! See Evil Portal workflow here: https://github.com/justcallmekoko/ESP32Marauder/wiki/evil-portal-workflow

This version also removes the in-app ESP flasher, but don't worry! It's grown up into its own app, where development will continue separately: https://github.com/0xchocolate/flipperzero-esp-flasher


## v0.4.0

Added Signal Monitor (thanks @justcallmekoko!) to support new sigmon command in Marauder v0.10.5: https://github.com/justcallmekoko/ESP32Marauder/releases

Added keyboard and +5V support from unleashed (thanks @xMasterX!)
