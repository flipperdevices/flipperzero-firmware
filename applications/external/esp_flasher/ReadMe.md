[![FAP Build](https://github.com/0xchocolate/flipperzero-esp-flasher/actions/workflows/build.yml/badge.svg)](https://github.com/0xchocolate/flipperzero-esp-flasher/actions/workflows/build.yml)

# ESP Flasher app for Flipper Zero

Flipper Zero app to flash ESP chips from the device (no computer connection needed!). Uses espressif's [esp-serial-flasher](https://github.com/espressif/esp-serial-flasher) library. Requires an ESP chip connected to the flipper's UART pins (e.g. Wi-Fi devboard). For information on how to connect, see the ESP pinout/wiring details on UberGuidoZ's GPIO section: https://github.com/UberGuidoZ/Flipper/tree/main/GPIO

<img src="https://github.com/0xchocolate/flipperzero-esp-flasher/blob/main/screenshots/esp-flasher-browse.png?raw=true" width=30% height=30% /> <img src="https://github.com/0xchocolate/flipperzero-esp-flasher/blob/main/screenshots/esp-flasher-flashing.png?raw=true" width=30% height=30% />

Supported targets: ESP32
ESP8266
ESP32-S2
ESP32-S3
ESP32-C3
ESP32-C2
ESP32-H2

## Get the app
1. Make sure you're logged in with  github account (otherwise the downloads in step 2 won't work)
2. Navigate to the [FAP Build](https://github.com/0xchocolate/flipperzero-esp-flasher/actions/workflows/build.yml)
   GitHub action workflow, and select the most recent run, scroll down to artifacts.
3. The FAP is built for the `dev` and `release` channels of both official and unleashed
   firmware. Download the artifact corresponding to your firmware version.
4. Extract `esp_flasher.fap` from the ZIP file downloaded in step 3 to your Flipper Zero SD card, preferably under Apps/GPIO along with the rest of the GPIO apps. (If you're using qFlipper to transfer files you need to extract the content of the ZIP file to your computer before you drag it to qFlipper, as qFlipper does not support direct dragging from a ZIP file (at least on Windows)).

From a local clone of this repo, you can also build the app yourself using ufbt.

## Using the app
Guide by [@francis2054](https://github.com/francis2054)

Use at your own risk. This hardcodes addresses for ESP chips.

Example of how to flash marauder using this app:
1. Make sure you follow the instructions for how to get the ESP flasher app on your Flipper Zero, they can be found on the top of this page. Latest release needs to be downloaded and installed.
2. Go to [Justcallmekoko's firmware page](https://github.com/justcallmekoko/ESP32Marauder/wiki/update-firmware#using-spacehuhn-web-updater) (or alternative ESP firmware) and download all files necessary for the board you are flashing, most boards will want all 4 files but for the Wi-Fi Devboard you want to download these 3 files: `0x1000` (Bootloader), `0x8000` (partitions), `0x10000` (Firmware). The `Boot App` is not needed for the Wi-Fi Devboard with this method. The Firmware one will redirect you to the releases page where you'll need to pick the one relevant to the board you're flashing, if you are using the official Wi-Fi Devboard you want to pick the one ending in `_flipper_sd_serial.bin`. 
3. Place all files downloaded in step 2 in a new folder on your desktop, the name does not matter. Rename the `_flipper_sd_serial.bin` file you downloaded in step 2 to `Firmware.bin`.
4. Now for transferring the files to the Flipper Zero, drag all the files from the folder on your desktop to the "esp_flasher" folder inside "apps_data" folder on the Flipper Zero SD card. Preferred method to transfer these files is plugging the SD card into your computer with an adapter, but qFlipper works as well. Insert the Flipper Zero SD Card back into the Flipper before proceeding to the next step.
5. Plug your Wi-Fi Devboard into the Flipper.
6. Press and keep holding the boot button while you press the reset button once, release the boot button after 2 seconds.
7. Open the ESP Flasher app on your Flipper Zero, it should be located under Apps->GPIO from the main menu if you followed the instructions for how to install the app further up on this page. (You might get an API mismatch error if the Flipper firmware you are running doesn't match the files you've downloaded, you can try "Continue" anyway, otherwise the app needs to be rebuilt or you might need to update the firmware on your Flipper).
8. Select "Flash ESP".
9. For "Bootloader" scroll down in the list and select `esp32_marauder.ino.bootloader.bin`, for "Paritition table" select `esp32_marauder.ino.partitions.bin` and for "Firmware" select `Firmware.bin`.
10. Scroll down and click "[>] FLASH" and wait for it to complete. (If you get errors here, press back button once and repeat step 6 then try "[>] FLASH" again).
11. Once it says "Done flashing" on the screen, restart the Flipper and you are done :)

## For future updates, just repeat from step 2 and only download the new "Firmware" bin

This process will improve with future updates! :)

## Support

For app feedback, bugs, and feature requests, please [create an issue here](https://github.com/0xchocolate/flipperzero-esp-flasher/issues).

You can find me (0xchocolate) on discord as @cococode.

If you'd like to donate to the app development effort:  
**ETH**: `0xf32A1F0CD6122C97d8953183E53cB889cc087C9b`  
**BTC**: `bc1qtw7s25cwdkuaups22yna8sttfxn0usm2f35wc3`

If you found the app preinstalled in a firmware release, consider supporting the maintainers!
