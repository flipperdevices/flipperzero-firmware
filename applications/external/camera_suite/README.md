<div align="center">
  <img align="center" src=".github/images/camera-suite.png" />
  <h2 align="center">Flipper Zero - Camera Suite</h2>
  <p align="center">
    Firmware and software to run an ESP32-CAM module on your Flipper Zero device.
  </p>
  <a href="https://shop.flipperzero.one/">
    <img src=".github/images/flipper-zero-buy-now.svg" />
  </a>
  <a href="https://docs.flipperzero.one/">
    <img src=".github/images/flipper-zero-docs.svg" />
  </a>
</div>

---

## Table of Contents <a name="index"></a>

- [Introduction](#intro)
- [Videos](#videos)
- [Hardware Requirements](#hardware-requirements)
- [Hardware Installation](#hardware-installation)
- [Firmware Installation](#firmware-installation)
- [Software Installation](#software-installation)
- [Software Guide](#software-guide)
- [Special Thanks](#special-thanks)
- [Contributions](#contributions)
- [Licensing](#licensing)
- [Changelog](fap/docs/CHANGELOG.md)

## Introduction <a name="intro"></a>

<img align="center" src=".github/images/preview.png" />

### Welcome to the ESP32-CAM Suite for Flipper Zero

Discover a new dimension of possibilities by connecting your ESP32-CAM module with your Flipper Zero device. The ESP32-CAM module, a compact ~~powerful~~ cheap camera module, enables you to capture images and stream a live video to your Flipper Zero. With this suite, your Flipper Zero becomes a hub of creativity and utility.

**What You Can Do:**

- **Capture Moments:** This custom Flipper Zero application empowers you to take pictures effortlessly. View real-time image previews on your Flipper Zero screen while you capture high quality blocky and pixelated memories! Hey it's still a memory and we're at least having fun...

- **Personalize Your Experience:** Tailor your camera settings with ease. Adjust camera orientation, experiment with various dithering options, and toggle flash, haptic feedback, sound effects, and LED effects to match your preferences. Feel free to use this as a flashlight too, it's pretty bright and good at blinding yourself unexpectedly!

There will be many more features added in the future! If you have any ideas or suggestions, please let me know by opening an issue [here][issues-link].

<p align="right">[ <a href="#index">Back to top</a> ]</p>

## Videos <a name="videos"></a>

This section is dedicated to all the fans that have made videos of this project! Thank you! 

If you have a video you'd like to share, please let me know by opening an issue [here][issues-link].

<details>

<summary>Expand for videos</summary>

<br />

**Flipper Zero ESP32 CAM Camera Module** - by TAKEAPART

[https://www.youtube.com/watch?v=cEl5UnWH_Ok](https://www.youtube.com/watch?v=cEl5UnWH_Ok)

[![Flipper Zero ESP32 CAM Camera Module - TAKEAPART](https://img.youtube.com/vi/cEl5UnWH_Ok/0.jpg)](https://www.youtube.com/watch?v=cEl5UnWH_Ok)

**Tech With Kids - Flipper Zero ESP32 Kamera** - by @rechtsanwalt.okan.dogan

[https://www.instagram.com/reel/C4DrufKoKrb/](https://www.instagram.com/reel/C4DrufKoKrb/)

[![Flipper Zero ESP32 CAM Camera Module - @rechtsanwalt.okan.dogan](.github/images/video_rechtsanwalt_okan_dogan.png)](https://www.instagram.com/reel/C4DrufKoKrb/)

</details>

<p align="right">[ <a href="#index">Back to top</a> ]</p>

## Hardware Requirements <a name="hardware-requirements"></a>

Requires an ESP32-CAM module. Below are two images of the ESP32-CAM module. You can find these all over Amazon, Ali Express, and other retailers.

<img src=".github/images/esp32-cam-front.png" /><img src=".github/images/esp32-cam-back.png" />

<p align="right">[ <a href="#index">Back to top</a> ]</p>

## Hardware Installation <a name="hardware-installation"></a>

Below is the pinout guide and diagram for the ESP32-CAM module to the Flipper Zero. From the ESP32-CAM module to the Flipper Zero:

```markdown
VCC to 3V3
GND to GND (Be sure to use the right GND, see image below.)
U0R to TX
U0T to RX
```

On the ESP32-CAM module itself you'll also need to connect the `IO0` pin to `GND`. This will place the module into flash mode for installing the firmware later on (see [Firmware Installation](#firmware-installation)). You can do this by connecting a jumper wire, a button, or a switch to do this.

<img align="center" src=".github/images/esp32-cam-pinout-guide.png" />

<p align="right">[ <a href="#index">Back to top</a> ]</p>

## Firmware Installation <a name="firmware-installation"></a>

The firmware is the software that runs on the ESP32-CAM module. It is required to use the camera module with your Flipper Zero. There are two ways to install the firmware, the new, easy way, and the old, hard way. The new way is a script that will do everything for you, the old way is a manual process that requires you to install the Arduino IDE and manually flash the firmware to the ESP32-CAM module. I recommend the new way, but if you have issues with it, try the old way.

<details>

<summary>Simple Installation - Firmware Flashing Utility</summary>

### Firmware Flashing Utility (Windows 10+ batch script)

1. Download/clone this repository to your computer.
2. Run the script found at the root of this directory: `firmware-flash.bat`.
3. Follow the on screen instructions to continue to flash the firmware to your ESP32-CAM module.

That's it, let me know if you have any issues!

</details>

<details>

<summary>Involved Installation - Manually flash with Arduino IDE</summary>

### Arduino IDE

1. Clone/download this repository to your computer.
2. Download and install the Arduino IDE from [here][arduino-ide].
3. Open `flipper-zero-camera-suite\firmware\firmware.ino` with your Arduino IDE.
4. In the Arduino IDE, go to `File > Preferences`.
5. In the `Settings` tab, add the following URL to the `Additional Boards Manager URLs` field:

   ```markdown
   https://dl.espressif.com/dl/package_esp32_index.json
   ```

6. In the Arduino IDE, go to `Tools > Board > Boards Manager`.
7. Search for `esp32` and install `esp32` by `Espressif Systems`.
8. Plug in your Flipper Zero via USB. Make sure qFlipper or something else isn't connected to it already after doing so.
9. On your Flipper Zero, open `GPIO > USB-UART Bridge`.
10. In the Arduino IDE, go to `Tools > Board > esp32 > AI Thinker ESP32-CAM`.
11. In the Arduino IDE, go to `Tools > Port` and select the port that your Flipper Zero is connected to.
12. Plug in the ESP32-CAM module to your Flipper Zero while connecting the `IO0` pin to `GND`. See [Hardware Installation](#hardware-installation) for more information.
13. Press the RST button on the back of the ESP32-CAM module to boot it into flash mode.
14. In the Arduino IDE, go to `Sketch > Upload` to upload the firmware to your ESP32-CAM module. You will see upload progress in % and receive a message on completion if successful.
15. Fin! Now you may use the [Software Installation](#software-installation) section to install the software on your Flipper Zero to take advantage of this hardwares firmware.

Note the upload may fail a few times, this is normal, try again. If it still fails, try pressing the RST button on the back of the ESP32-CAM module again or checking your connections.

On success, your screen should look like this:

<img align="center" src=".github/images/firmware-build-success.png" />

</details>

<p align="right">[ <a href="#index">Back to top</a> ]</p>

## Software Installation <a name="software-installation"></a>

Below are two ways to install the software on your Flipper Zero. The first way is the simple way, the second way is the involved way. The simple way is to install the fap file via the Flipper Zero application catalog. The involved way is to manually install the fap file to your Flipper Zero MicroSD card.

<details>

<summary>Simple Installation - Flipper Zero Application Catalog</summary>

### Flipper Zero Application Catalog

This fap app is available on your phones Flipper Zero application catalog! Simply open the Flipper Zero app on your phone, go to the "Applications" tab, and search for "Camera Suite". Install it and you're good to go! More information about Flipper Zero applications can be found [here][flipper-zero-apps].

</details>

<details>

<summary>Involved Installation - Manually Install Files</summary>

### Manually Install Files

1. Connect your Flipper Zero via USB, or insert your MicroSD.
2. Navigate to the latest GitHub "Build + upload" action [here][github-actions-link].
3. Open the most recent action on that page (top of the list) and download the fap zip for either "dev" or "release" build versions of the Flipper Zero firmware depending on your usage. Generally you'll want to use the "release" build version.
4. Move "camera_suite.fap" into `~\apps\gpio\` on your Flipper Zero MicroSD:

   ```markdown
   .                        # The Flipper Zero MicroSD root.
   ├── apps                 # The Flipper Zero Applications folder.
   | ├── gpio               # The Flipper Zero GPIO folder.
   | | ├── camera_suite.fap # The Camera Suite application.
   ```

5. Reinsert your MicroSD into your Flipper Zero if you took it out.
6. Plug in your ESP32-CAM module to your Flipper Zero.
7. Press the "Power" button on your Flipper Zero to turn it on.
8. Open the application "[ESP32] Camera Suite":

   ```markdown
   Applications > GPIO > [ESP32] Camera Suite
   ```

9. That's it! Follow the on screen instructions to continue.

</details>

<p align="right">[ <a href="#index">Back to top</a> ]</p>

## Software Guide <a name="software-guide"></a>

### Flipper Zero button mappings

| Button | Action |
| :----- | :----- |
| 🔼     | Contrast Up |
| 🔽     | Contrast Down |
| ◀️     | Toggle invert |
| ▶️     | Toggle dithering on/off |
| ↩️     | Go back |
| 🔵     | Take a picture and save to the "DCIM" folder at the root of your SD card. Image will be saved as a bitmap file with a timestamp as the filename ("YYYYMMDD-HHMMSS.bmp"). If flash is on in the settings (enabled by default) the ESP32-CAM onboard LED will light up when the camera is opened. |

### Camera Settings

| Setting | Description |
| :------ | :---------- |
| **Orientation** | Rotate the camera image 90 degrees counter-clockwise starting at zero by default (0, 90, 180, 270). This is useful if you have your camera module mounted in a different orientation than the default. |
| **Dithering Type** | Change between the Cycle Floyd–Steinberg, Jarvis-Judice-Ninke, and Stucki dithering types. |
| **Flash** | Toggle the ESP32-CAM onboard LED on/off while using the camera. |

### Application Settings

| Setting | Description |
| :------ | :---------- |
| **Haptic Effects** | Toggle haptic feedback on/off. |
| **Sound Effects** | Toggle sound effects on/off. |
| **LED Effects** | Toggle LED effects on/off. |

<p align="right">[ <a href="#index">Back to top</a> ]</p>

## Special Thanks <a name="special-thanks"></a>

A huge thanks to the following people and projects for making this possible:

- This project is based on & forked from the [Flipper Zero Camera Application][flipperzero-camera] by [Z4urce][github-profile-z4urce]. Thanks Z4urce for the inspiration and the great work!
- I based this projects application structure on the [Flipper Zero Boilerplate Application][flipper-zero-fap-boilerplate] by [leedave][github-profile-leedave]. Thanks leedave for the great boilerplate application that helped me learn how to structure a Flipper Zero application properly!
- [WillyJL][github-profile-willyjl] for your guidance and wisdom, contributions to the F0 community, and the amazing [Momentum Firmware][flipper-zero-momentum-firmware]!
- [TalkingSasquach][github-profile-talkingsasquach] for your contributions to the F0 community, for all of your helpful [YouTube videos][youtube-talkingsasquach], and the [Discord community][discord-squachtopia]!
- [RogueMaster][github-profile-roguemaster] for your contributions to the F0 community, and the amazing [RogueMaster Firmware][flipper-zero-roguemaster-firmware]!
- The project images were drawn using the a application called "[lopaka][lopaka]" by [sbrin][github-profile-sbrin]. Thanks sbrin for your help in creating the images for this project!
- The Flipper Zero community for all your support and feedback!

<p align="right">[ <a href="#index">Back to top</a> ]</p>

## Contributions <a name="contributions"></a>

To contribute to this project, please follow the steps below:

1. Fork.
2. Create a new branch: `<username>/[<issue-#>]-<feature-or-bug-fix-desc>`
3. Program. Commit changes, push.
4. Request PR [here][pull-request-link], introduce work via your branch.
5. Wait for review and merge.

When developing the firmware, be sure to download the dependencies by running the `firmware-assets.bat` batch script at the root of this directory. This will download the ESP32-CAM firmware dependencies (Arduino Build & Git Assets) to your Windows TEMP folder. You can run the same script over again later to remove the temporary files when you're done using them. Once installed, add these dependencies and their directories to your "Include path" in your IDE of choice. I prefer Visual Studio Code; there's a guide on how to get started with that workflow here:

https://github.com/CodyTolene/Flipper-Zero-Development-Toolkit

Example include path:

```markdown
C:/Users/<username>/AppData/Local/Temp/arduino-cli/**/**
```

Thank you for any and all contributions to this project, I'm looking forward to seeing what you come up with! If you have any questions, please let me know by opening an issue [here][issues-link].

<p align="right">[ <a href="#index">Back to top</a> ]</p>

## Licensing <a name="licensing"></a>

This project is licensed under the BSD 3-Clause license. See the [LICENSE](LICENSE) file for details. Certain files in this project are based on code from Espressif Systems (Shanghai) PTE LTD and are licensed under the Apache License, Version 2.0. See the [APACHE_2_LICENSE](LICENSE.Apache-2.0) file for the pertaining license text.

`SPDX-License-Identifier: BSD 3-Clause, Apache-2.0`

<p align="right">[ <a href="#index">Back to top</a> ]</p>

## Wrapping Up <a name="licensing"></a>

Thanks to all the people and projects that made this possible! I hope you enjoy this project as much as I enjoyed working on it. If you have any questions, please let me know by opening an issue [here][issues-link].

| Type                                                           | Info                                                                               |
| :------------------------------------------------------------- | :--------------------------------------------------------------------------------- |
| <img width="48" src=".github/ng-icons/email.svg" />            | webmaster@codytolene.com                                                           |
| <img width="48" src=".github/simple-icons/buymeacoffee.svg" /> | https://www.buymeacoffee.com/codytolene                                            |
| <img width="48" src=".github/images/bitcoin-btc-logo.svg" />   | [bc1qfx3lvspkj0q077u3gnrnxqkqwyvcku2nml86wmudy7yf2u8edmqq0a5vnt][btc-address-link] |

Fin. Happy programming friend!

Cody Tolene

<!-- LINKS -->

[arduino-ide]: https://www.arduino.cc/en/software
[btc-address-link]: https://explorer.btc.com/btc/address/bc1qfx3lvspkj0q077u3gnrnxqkqwyvcku2nml86wmudy7yf2u8edmqq0a5vnt
[discord-squachtopia]: https://discord.gg/squachtopia
[flipper-zero-apps]: https://docs.flipper.net/apps
[flipper-zero-fap-boilerplate]: https://github.com/leedave/flipper-zero-fap-boilerplate
[flipper-zero-momentum-firmware]: https://github.com/Next-Flip/Momentum-Firmware
[flipper-zero-roguemaster-firmware]: https://github.com/RogueMaster/flipperzero-firmware-wPlugins
[flipperzero-camera]: https://github.com/Z4urce/flipperzero-camera
[github-actions-link]: https://github.com/CodyTolene/Flipper-Zero-Camera-Suite/actions?query=workflow%3A%22Build+%2B+upload.%22
[github-profile-leedave]: https://github.com/leedave
[github-profile-roguemaster]: https://github.com/RogueMaster
[github-profile-sbrin]: https://github.com/sbrin
[github-profile-talkingsasquach]: https://github.com/skizzophrenic
[github-profile-willyjl]: https://github.com/Willy-JL
[github-profile-z4urce]: https://github.com/Z4urce
[issues-link]: https://github.com/CodyTolene/Flipper-Zero-Camera-Suite/issues
[lopaka]: https://github.com/sbrin/lopaka
[pull-request-link]: https://github.com/CodyTolene/Flipper-Zero-Camera-Suite/pulls
[youtube-talkingsasquach]: https://www.youtube.com/@TalkingSasquach
