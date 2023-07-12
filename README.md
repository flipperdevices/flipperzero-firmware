# Flipper Zero Evil Portal

An evil captive portal Wi-Fi access point using the Flipper Zero and Wi-Fi dev board

## About

**This project is a work in progress.**

This project will turn your Wi-Fi dev board into an open access point. When users try to connect to this access point they will be served a fake login screen. User credentials are sent to the Flipper and logged on the SD card.

## Portals

The portal I initially provided is just an (ugly) example, please check out the [community portals folder](https://github.com/bigbrodude6119/flipper-zero-evil-portal/tree/main/portals) for more portals. 
Contributors are welcome and very much needed!

Users, remember to rename the new portal as `index.html` when you drag it on the flipper SD card.

## Disclaimer

I am not a C developer and I am using this project as a way to learn more about esp32, flipper zero and, C programming.

**Contributors are welcome!** Please feel free to open a PR at any time.

This program is for educational purposes only.

# Getting Started

The pre-built fap file is made for the unleashed custom firmware. If you are on a different firmware you can download the `evil_portal.fap` file [at flipc.org](https://flipc.org/bigbrodude6119/flipper-zero-evil-portal?branch=main&root=flipper/flipper-evil-portal&firmware=unleashed) or you can build the `.fap` file yourself by following [these instructions](https://github.com/flipperdevices/flipperzero-firmware/blob/dev/documentation/AppsOnSDCard.md).

**Note** The official Flipper Zero firmware is now supported again thanks to @sboger.

## Install the pre-built app on the flipper

Go to the releases section on this repo. Download and extract the `unleashed-evil_portal.fap.zip` file from the latest release. This file will contain the `evil_portal.fap` file for the Unleashed firmware.

Put the `evil_portal.fap` file into the `apps/GPIO/` folder on your Flipper SD card.

In the releases section you will also need to download and extract the `evil_portal_sd_folder.zip` folder. This .zip file contains a `evil_portal` folder.

Put the `evil_portal` folder into the `apps_data` folder on your SD card.
This is an example of your Flipper SD card if done correctly.

```
apps/
  GPIO/
    evil_portal.fap
apps_data/
  evil_portal/
    ap.config.txt
    index.html
    logs/
      <empty>
```

You should be able to see the `[ESP32] Evil Portal` app on your flipper zero now.

# Installing/flashing the Wi-Fi dev board

There is now an easier method (Option One) of flashing your ESP32 dev board. Thank you to reddit user dellycem for showing me how to do this.

**Note:** the following boards are supported via this method

1. [The official Wifi dev board](https://shop.flipperzero.one/products/wifi-devboard)
2. Alternative ESP32-S2 boards like [this one from AWOK Dynamics](https://www.awokdynamics.com/product/esp32-v4-w-case/20?cp=true&sa=true&sbp=false&q=false)
3. ESP32-WROOM boards

The alternative ESP32-S2 boards can use the same `.bin` files as the official dev board.

The esp32 wroom board has it's own pre-compiled `.bin` files provided in the 0.0.2 release. Please check out the required pin connections bellow.

**If you are not using one of these boards you will have to go with option two.**

## Option One - Official Wi-Fi Dev Board

Starting with version `0.0.2` I will include pre-compiled `.bin` files for the official WiFi Dev board. This will allow users to flash their dev boards via a website instead of through the Arduino IDE.

1. Download and extract the `wifi_dev_board.zip` file that is part of the latest release. This will contain 4 `.bin` files. 
2. Connect your WiFi dev board to your computer while holding the boot button.
3. Go to the website https://esp.huhn.me/ and press the `Connect` button.
4. Select the port associated with your board.
5. Add each of the 4 `.bin` files using the blue `Add` button.
6. Enter the following addresses in the text field to the left of each file.

- 1000 - EvilPortal.ino.bootloader.bin
- 8000 - EvilPortal.ino.partitions.bin
- e000 - boot_app0.bin
- 10000 - EvilPortal.ino.bin

7. Press the `Program` button and wait while the board is being flashed.
8. Assuming you do not have any errors you are good to go.


## Option Two - Other compatible boards

Follow the steps below to flash the other compatible ESP32 boards. You may have to adjust the steps below for your specific board:

1. Download and install the Arduino IDE from [here][link-arduino].
2. Download zip/clone dependency [AsyncTCP][link-asynctcp] to file.
3. Download zip/clone dependency [ESPAsyncWebServer][link-espasyncwebserver] to file.
4. Unzip both dependencies to your Arduino library folder.
   - On Windows this is usually `C:\Users\<username>\Documents\Arduino\libraries`.
5. Go to the releases section on this repo and download the `EvilPortal.ino` file, open it with Arduino IDE.
6. Go to `File > Preferences` and paste the following two URL's into the `Additional Boards Manager URLs` field:
   ```
   https://dl.espressif.com/dl/package_esp32_index.json
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_dev_index.json
   ```
7. Go to `Tools > Board > Boards Manager...` and search for `esp32` and install `esp32 by Espressif Systems`.
8. Go to `Tools > Board` and select `ESP32S2 Dev Module` or the ESP32 board that you are trying to flash.
9. On your ESP32-S2 Wi-Fi module, hold the BOOT button.
10. Connect your board to your computer, keep holding the BOOT button (holding for just 3-5 seconds and releasing may be fine, continuously holding worked better for me).
11. Go to `Tools > Port` and select the port that appeared when you connected your ESP32.
12. Click the "Upload" button in the top left corner of the Arduino IDE.
13. On success, you will see something like:
    ```
    Hash of data verified.
    Leaving...
    WARNING: ESP32-S2 (revision v0.0) chip was placed into download mode...
    ```
14. Plug in the Wi-Fi Dev board to the flipper, press the reset button on the Wi-Fi dev board and you should now see a solid blue light.



# Usage

Plug in the Wi-Fi Dev board to the flipper.

Open the app on the Flipper and press `Start portal` on the main menu. After a few seconds you should start to see logs coming in from your Wi-Fi dev board and the AP will start and the LED will turn green.

The AP will take the name that is in the `ap.config.txt` file located on your Flipper in the `apps_data/evil_portal/` folder.

When you connect to the AP a web page will open after a few seconds. This web page contains the HTML located in the `index.html` file located on your Flipper in the `apps_data/evil_portal/` folder.

You can stop the portal by pressing `Stop portal` on the main menu. The LED should turn blue.

You can manually save logs using the `Save logs` command. Logs will be stored in the `logs` folder that is in your `apps_data/evil_portal/` folder.

Logs will automatically be saved when exiting the app or when the current log reaches 4000 characters.

## Alternative boards

The ESP32 wroom boards will not have the LED indicators in the 0.0.2 release and if you are compiling for a Wroom board you will have to comment out the code dealing with the LEDs.
The pre-compiled `.bin` files for that board already have this change. I plan on making this process easier in the next release.

The following pins are required for the board to work:

- 3.3v
- GND
- TX
- RX

Keep in mind that the TX/RX pins go to the opposite pins on the flipper. So TX on your ESP32 goes to RX on the flipper. For my Wroom board I had to use RX0/TX0, your board may be a little different.

## Troubleshooting

- If you run into any issues make sure that you have the required files set up on the Flipper `apps_data` folder on the Flipper SD card.

- If the AP won't start or you have other issues try pressing reset on the Wi-Fi dev board, waiting a few seconds, and pressing `Start portal` on the main menu.

- It is important to give the dev board some time to load the html files from the Flipper.

- If you have the Marauder firmware on your dev board you **might** need to enable `Erase All Flash Before Sketch Upload` before flashing or follow the `Erasing firmware` instructions below. If you are using the web flasher there is an erase function on the website.

- If you see garbage characters in the AP name you will need to press the reset button on the board.

- Some users are reporting that the captive portal login does not open on some Android phones.

## Erasing firmware <a name="erasing-firmware"></a>

Assuming you have the Flipper Zero Wi-Fi Wrover Development Module (**ESP32-S2**):

1. Install [Python][link-python].
2. Open a command terminal as an administrator:
   - On Windows press ⊞Win+R, type "cmd", and press CTRL+SHIFT+ENTER.
3. In the terminal type the following to install [esptool][link-esptool] via Python package manager:
   ```
   pip install esptool
   ```
4. Install [setuptools][link-setuptools] dependencies:
   ```
   pip install setuptools
   ```
5. Enter the following command into your terminal, do not run it yet:
   ```
   python -m esptool --chip esp32s2 erase_flash
   ```
6. On your ESP32-S2 Wi-Fi module, hold the BOOT button.
7. Connect your ESP32-S2 to your computer, keep holding the BOOT button.
8. In your terminal press enter to run the command from step 5.
9. When successful you will get the message `Chip erase completed successfully in ___s` (time in seconds suffixed with "s").
10. Unplug/reset your board.

## Todo

I plan on working on this in my free time. Here is my todo list.

- Support for multiple portals
- Enter AP name on the Flipper
- Scan nearby APs and clone their SSID (good idea leedave!)
- Add a config file for general app settings
- Create cleaner log files that are easier to read
- Randomize mac address so that the network shows up as a new network each time
- Clean up code & implement best practices

## License

Distributed under the MIT License. See `LICENSE.txt` for more information.

## Acknowledgments

I was only able to create this using the following apps as examples

- [flipperzero-wifi-marauder](https://github.com/0xchocolate/flipperzero-wifi-marauder)
- [UART_Terminal](https://github.com/cool4uma/UART_Terminal)
- [flipper-zero-fap-boilerplate](https://github.com/leedave/flipper-zero-fap-boilerplate)
- [Create Captive Portal Using ESP32](https://iotespresso.com/create-captive-portal-using-esp32/)

## Contact me

You can message me on my reddit account bigbrodude6119

<!-- LINKS -->

[link-arduino]: https://www.arduino.cc/en/software
[link-asynctcp]: https://github.com/me-no-dev/AsyncTCP
[link-espasyncwebserver]: https://github.com/me-no-dev/ESPAsyncWebServer
[link-esptool]: https://pypi.org/project/esptool/
[link-python]: https://www.python.org/downloads/
[link-setuptools]: https://pypi.org/project/setuptools/
