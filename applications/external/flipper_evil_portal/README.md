# Flipper Zero Evil Portal

An evil captive portal Wi-Fi access point using the Flipper Zero and Wi-Fi dev board

## About

**This project is a work in progress.**

This project will turn your Wi-Fi dev board into an open access point. When users try to connect to this access point they will be served a fake login screen. User credentials are sent to the Flipper and logged on the SD card.

## Disclaimer

I am not a C developer and I am using this project as a way to learn more about esp32, flipper zero and, C programming.

This program is for educational purposes only.

## Getting Started

There are pre-built .fap files for the official FW (untested) as well as unleashed FW (tested).

You will need to manually flash the Wi-Fi dev board.

### Install pre-built app on the flipper

Go to the releases section on this repo and download and extract either the `ofw-evil_portal.fap.zip` file or the `unleashed-evil_portal.fap.zip` file depending on if you are using the official firmware (ofw) or the unleashed firmware. These files will contain the `evil_portal.fap` file for your firmware.

You will also need to download and extract the `evil_portal_sd_folder.zip` folder. This will contain necessary files for the app to run.

Put the `evil_portal.fap` file into the `apps/GPIO/` folder on your Flipper SD card.

Put the `evil_portal` folder into the `apps_data` folder.
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

If you want to create your own `index.html` file keep in mind that there is a limit of 4000 characters for the file. I plan to increase this later but I ran into some issues with larger files.

## Installing/flashing the Wi-Fi dev board

If you've already flashed your Wi-Fi dev board with the Marauder firmware or something else you will need to erase it before installing the new firmware here. Follow [the guide here](#erasing-firmware) for that.

Follow the steps below to flash the Wi-Fi dev board with the evil portal firmware via Windows. The instructions below are for the Flipper Zero Wi-Fi Wrover Development Module (**ESP32-S2**), you may have to adjust the steps for your specific board:

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
8. Go to `Tools > Board` and select `ESP32S2 Dev Module`.
9. On your ESP32-S2 Wi-Fi module, hold the BOOT button.
10. Connect your ESP32-S2 to your computer, keep holding the BOOT button (holding for just 3-5 seconds and releasing may be fine, continuously holding worked better for me).
11. Go to `Tools > Port` and select the port that appeared when you connected your ESP32-S2.
12. Click the "Upload" button in the top left corner of the Arduino IDE.
13. On success, you will see:
    ```
    Hash of data verified.
    Leaving...
    WARNING: ESP32-S2 (revision v0.0) chip was placed into download mode...
    ```
14. Plug in the Wi-Fi Dev board to the flipper, press the reset button on the Wi-Fi dev board and you should now see a solid blue light.

## Installing/flashing an ESP32 Wroom board

Reddit user dellycem [compiled binaries](https://old.reddit.com/r/flipperzero/comments/14ni93r/i_made_a_evil_portal_app_for_the_fz_wifi_dev_board/jqd42fi/?context=3) for the ESP32 Wroom board if you would like to use that instead of the Wi-Fi devboard.

1. Download the bin files from [this link](https://wetransfer.com/downloads/4d3dd914f2df43dc6c84efa452043f4220230702094742/33a478)
2. Go to [ESPWebTool](https://esp.huhn.me/) and get your board connected.
3. Add each of the bin files at the following locations
   ```
   EvilPortal.bootloader.bin - 0x1000
   EvilPortal.partitions.bin - 0x8000
   boot_app0.bin - 0xe000
   EvilPortal.bin - 0x10000
   ```
4. Press the program button and wait while your board is flashed.
5. Once complete, hook up the 3.3v, GND, RX0, and TX0 pins to the flipper zero. Remember that the RX/TX pins should go to the opposite pins on the flipper zero. RX -> TX, TX -> RX.

## Usage

Plug in the Wi-Fi Dev board to the flipper.

Open the app on the Flipper and press `Start portal` on the main menu. After a few seconds you should start to see logs coming in from your Wi-Fi dev board and the AP will start and the LED will turn green.

The AP will take the name that is in the `ap.config.txt` file located on your Flipper in the `apps_data/evil_portal/` folder.

When you connect to the AP a web page will open after a few seconds. This web page contains the HTML located in the `index.html` file located on your Flipper in the `apps_data/evil_portal/` folder.

You can stop the portal by pressing `Stop portal` on the main menu. The LED should turn blue.

You can manually save logs using the `Save logs` command. Logs will be stored in the `logs` folder that is in your `apps_data/evil_portal/` folder.

Logs will automatically be saved when exiting the app or when the current log reaches 4000 characters.

## Building for different firmware

If you are not using the official flipper zero firmware or the unleashed firmware you can build the .fap file yourself by following [these instructions](https://github.com/flipperdevices/flipperzero-firmware/blob/dev/documentation/AppsOnSDCard.md).

Note that you will need to use the firmware repo that you wish to build for.

You can also download builds for each firmware via [flipc](https://flipc.org/bigbrodude6119/flipper-zero-evil-portal?branch=main&root=flipper%2Fflipper-evil-portal).

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

## Issues

If you run into any issues make sure that you have the required files set up on the Flipper `apps_data` folder on the Flipper SD card.

Logs will not be saved if there is no `logs` folder in `apps_data/evil_portal/`.

If the AP won't start or you have other issues try pressing reset on the Wi-Fi dev board, waiting a few seconds, and pressing `Start portal` on the main menu.

It is important to give the devboard some time to load the html files from the Flipper.

If you have the Marauder firmware on your dev board you may need to enable `Erase All Flash Before Sketch Upload` before flashing.

Some users are reporting that the captive portal login does not open on some Android phones.

## Todo

I plan on working on this in my free time. Here is my todo list.

- Support for multiple portals
- Enter AP name on the Flipper
- Add a config file for general app settings
- Create cleaner log files that are easier to read
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
