# FlipBoard

![FlipBoard](png/flipboard.png)

The FlipBoard is an MacroPad from [MakeItHackin](https://github.com/MakeItHackin/FlipBoard) with software from @CodeAllNight ([https://youtube.com/@MrDerekJamison]([https://youtube.com/@MrDerekJamison)). It connects to the Flipper Zero's GPIO pins and is a 4 button keypad with addressable LEDs.  You can purchase it on [tindie](https://www.tindie.com/products/32844/) or [Etsy](https://www.etsy.com/listing/1601295558/).

This tutorial is a guide to controlling the FlipBoard using JavaScript on Flipper Zero.  This [video](https://www.youtube.com/@MrDerekJamison) also describes the process.

## Requirements

- [Flipper Zero](https://flipperzero.one/how-to-buy)
- [FlipBoard](https://github.com/MakeItHackin/FlipBoard)
- Latest custom firmware for Flipper Zero (requires **dev branch** for most firmware, due to JavaScript being so new for the Flipper Zero).

## Software Installation

- Load qFlipper.
- Click the `File Manager` tab.
- Navigate to `SD Card`/`apps`/`Scripts` folder.
- Right click and choose `New Folder` and name it `flipboard`.
- Navigate into the `flipboard` folder.
- Copy the files from the [sub](./sub) folder into the `SD Card`/`subghz` folder; or capture your own Sub-GHz signals and save them as `Light_on.sub` and `Light_off.sub`.  Alternately, you can edit the [subghz_fb.js](./scripts/subghz_fb.js) file to use different signals.
- Copy all of the files from the [scripts](./scripts/) into the `flipboard` folder.
- Navigate to the `SD Card`/`apps_data`/`js_app`/`plugins` folder.
- Copy the files from the [fal](./fal) folder that matches your firmware (for example if you are on Momentum dev branch from mid-April [mntm-dev-2024-04-19-a2fc553](./fal/mntm-dev-2024-04-19-a2fc553/)) into the `plugins` folder.

## Running the Software

- Connect the FlipBoard to the Flipper Zero.
- On the Flipper, press `OK` to open the main menu.
- Select `Apps`, `Scripts`, `flipboard` then `a_runner.js` and press `OK`.
- Select the flipboard script (ends with `fb.js`) that you want to run.
- Press buttons on your FlipBoard to interact with the script.

## Building on custom firmware

The steps above in Software Installation have you overlay the fal files on top of your existing firmware.  If you want to build the fal files into your firmware, you can follow these steps instead.

Clone your firmware.  The following [wiki](https://github.com/jamisonderek/flipper-zero-tutorials/wiki/Install-Firmware-and-Apps#clone--deploy-firmware) describes the process.

Copy the files from the [modules](./modules) folder into the `applications\system\js_app\modules` folder of your custom firmware source code.

Edit your `applications\system\js_app\application.fam` file to include the following:

```c
App(
    appid="js_rgbleds",
    apptype=FlipperAppType.PLUGIN,
    entry_point="js_rgbleds_ep",
    requires=["js_app"],
    sources=["modules/js_rgbleds/*.c"],
)

App(
    appid="js_infrared",
    apptype=FlipperAppType.PLUGIN,
    entry_point="js_infrared_ep",
    requires=["js_app"],
    sources=["modules/js_infrared.c"],
)

App(
    appid="js_speaker",
    apptype=FlipperAppType.PLUGIN,
    entry_point="js_speaker_ep",
    requires=["js_app"],
    sources=["modules/js_speaker.c"],
)
```

Rebuild your firmware and flash it to your Flipper Zero.  NOTE: In some cases, you may be able to rebuild just the FAL modules by first opening the `applications\system\js_app\js_app.c` file and then choosing `Terminal`, `Run Task...`, `[Release] Launch app on Flipper`.

## Support
If you have any questions, please ask in my [Flipper Zero Discord](https://discord.com/invite/NsjCvqwPAd) server.  There are also giveaways and other fun things happening there.

Support my work: 
- Option 1. [Like, Subscribe and click the Bell (to get notified)](https://youtu.be/DAUQGeG4pc4) 
- Option 2. [https://ko-fi.com/codeallnight](https://ko-fi.com/codeallnight) (donate $3 via PayPal or Venmo) 
- Option 3. Click the "Thanks" button on [YouTube](https://youtu.be/DAUQGeG4pc4). 
- Option 4. Purchase a [FlipBoard](https://github.com/MakeItHackin/FlipBoard) (I get a portion of the sale).
