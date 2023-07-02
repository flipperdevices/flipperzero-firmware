[![FAP Build](https://github.com/0xchocolate/flipperzero-wifi-marauder/actions/workflows/build.yml/badge.svg)](https://github.com/0xchocolate/flipperzero-wifi-marauder/actions/workflows/build.yml)

# WiFi Marauder companion app for Flipper Zero

Requires a connected dev board running Marauder FW. [See install instructions from UberGuidoZ here.](https://github.com/UberGuidoZ/Flipper/tree/main/Wifi_DevBoard#marauder-install-information)

## Get the app
1. Make sure you're logged in with a github account (otherwise the downloads in step 2 won't work)
2. Navigate to the [FAP Build](https://github.com/0xchocolate/flipperzero-wifi-marauder/actions/workflows/build.yml)
   GitHub action workflow, and select the most recent run, scroll down to artifacts.
3. The FAP is built for the `dev` and `release` channels of both official and unleashed
   firmware. Download the artifact corresponding to your firmware version.
4. Extract `ESP32_WiFi_Marauder.fap` from the ZIP file to your Flipper
   Zero SD card, preferably under Apps/GPIO along with the rest of the GPIO apps.

From a local clone of this repo, you can also build the app yourself using ufbt.

## In-app ESP32 flasher (WIP)
The app now contains a work-in-progress of an ESP32 flasher (close to the bottom of the marauder menu). Use at your own risk. This hardcodes addresses for non-S3 ESP32 chip, and you'll need to source your own bootloader, partition table, and application bin files (as described below).

If you use it, make sure you put your board into reflash mode first (generally by pressing RESET while holding BOOT).

To use this method:
1. Make sure you follow the instructions for how to get the Marauder app on your Flipper Zero, they can be found on the top of this page. Latest release needs to be downloaded and installed.
2. Go to [Justcallmekoko's firmware page](https://github.com/justcallmekoko/ESP32Marauder/wiki/update-firmware#using-spacehuhn-web-updater) and download all 4 files necessary for the board you are flashing. The fourth one called Firmware will redirect you to the releases page where you'll need to pick the one relevant to the board you're flashing, if you are using the official Wi-Fi Devboard you want to pick the one ending in `_flipper_sd_serial.bin`.
3. Place all files downloaded in step 2 in a new folder on your desktop, extract `esp32_wifi_marauder.fap` from the `flipperzero-wifi-marauder-blablabla.zip` ZIP file.
4. Now for transferring the files to the Flipper Zero, drag all the files from the folder on your desktop to the "Marauder" folder inside "apps_data" folder on the Flipper Zero SD card. Preferred method to transfer these files is plugging the SD card into your computer, but qFlipper works as well. Plug the Flipper Zero SD Card back into the Flipper before proceeding to the next step.
5. Plug your Wi-Fi Devboard into the Flipper.
6. Press and keep holding the boot button while you press the reset button once, release the boot button after 2 seconds.
7. Open the Marauder app, it should be named "esp32_wifi_marauder" and be located under Apps->GPIO from the main menu if you followed the instructions for how to install the app further up on this page. (You might get a API mismatch error if the Flipper firmware you are running doesn't match the files you've downloaded, you can try "Continue" anyway, otherwise the app needs to be rebuilt or you might need to update the firmware on your Flipper.
8. Press the up arrow on the Flipper three times to get to "Reflash ESP32 (WIP)" and open it.
9. For "Bootloader" scroll down in the list and select `esp32_marauder.ino.bootloader.bin`, for "Paritition table" select `esp32_marauder.ino.partitions.bin` and for "Application" select `boot_app0.bin`.
10. Scroll down and click "[>] FLASH" and wait for it to complete. (If you get errors here, press back button once and repeat step 6 then try "[>] FLASH" again).
11. Once it says "Done flashing" on the screen, restart the Flipper and you are done :)

## For future updates of the Marauder firmware, just replace the `esp32_wifi_marauder.fap` in the "Marauder" under "app_data" and follow the instructions from step 7.

This process will improve with future updates! :)

## Support

For app feedback, bugs, and feature requests, please [create an issue here](https://github.com/0xchocolate/flipperzero-firmware-with-wifi-marauder-companion/issues).

You can find me (0xchocolate) on discord as @cococode#6011.

If you'd like to donate to the app development effort:  
**ETH**: `0xf32A1F0CD6122C97d8953183E53cB889cc087C9b`  
**BTC**: `bc1qtw7s25cwdkuaups22yna8sttfxn0usm2f35wc3`

Find more info about Marauder and support its developer (justcallmekoko aka WillStunForFood) here: https://github.com/justcallmekoko/ESP32Marauder

If you found the app preinstalled in a firmware release, consider supporting the maintainers!