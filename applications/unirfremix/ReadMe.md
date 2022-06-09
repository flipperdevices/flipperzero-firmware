# UniRF Remix App
### Integrated into Flipper Zero Unleashed Firmware 0.58
------------
### Quick Start

- Grab DFU from [Releases](https://github.com/ESurge/flipperzero-firmware-unifxremix/Relases/latest)
- Flash onto Flipper Zero
- Edit [universal\_rf\_map](https://github.com/ESurge/flipperzero-firmware-unirfremix/blob/dev/assets/resources/subghz/assets/universal_rf_map) and upload to SD card under ```/subghz/assets``` folder
------------
### Custom Import

- UniRFRemix folder has the base code. Use ```unirfremix_app``` for ```applications.c```
- Icons used are located at ```assets/icons/UniRFRemix```
- Animated Icon used is located at ```assets/icons/MainMenu/UniRFRemix_14```
- Follow compilation instructions from [Flipper Devices Flipper Zero Firmware GitHub](https://github.com/flipperdevices/flipperzero-firmware)
------------
### Notes
* ##### App Usage
  - Press a button to send the assigned capture file.
  - Press Back button to set how many repeats the app should send. Capped at 5 repeats.
  - Hold Back button to exit app.
  - Only RAW SubGhz captures are supported currently.
  - No skip function.

* ##### Universal RF Map
  - Backwards compatible with [jimilinuxguy Universal RF Remote](https://github.com/jimilinuxguy/flipperzero-universal-rf-remote) map file. You should be able to use the map file as is with both versions.
  - Recommend that you update the map file to the version included in this repo.
  - File path should not have any spaces or special characters (- and _ excluded).
  - Labels are limited to 12 characters.
    - Why? This is to prevent overlapping elements on screen.
    - For example: If you set your label or file to ```WWWWWWWWWWWWWWW``` you'll be over the screen limits.

* ##### Firmware
  - Code based on the Unleashed firmware
  - I modified the wav_player title in applications.c but that's all.

* ##### Issues
  - If you find any issues, report them. I am not sure I can fix them, but I will try.
------------
### Screenshots
#### Main Menu:
![unirfremix_mainmenu](https://user-images.githubusercontent.com/982575/169637623-bc41cfa5-6433-4198-a970-8fce42691ad7.png)
#### Idle Screen:
- Repeat indictator is located at the bottom right

![unirfremix-idle](https://user-images.githubusercontent.com/982575/169639427-daef6274-2e38-4684-816a-14ba915aa051.png)

#### Sending Screen:
- Button pressed will highlight when sending
- LED indicator will flash

![unirfremix-sending](https://user-images.githubusercontent.com/982575/169639435-74bdeb9e-da58-4ada-b613-8c4f8f89ab46.png)

#### Invalid Map File:
- If you don't have at least one valid file path set in your ```universal_rf_map``` file, you'll see this screen

![unirfremix-invalidmap](https://user-images.githubusercontent.com/982575/169639438-f1b96944-42c4-476c-9fe2-233d174c6262.png)

#### Missing Map File:
- If you don't have the ```universal_rf_map``` file, you'll see this screen

![unirfremix-missingmap](https://user-images.githubusercontent.com/982575/169639439-6414c81a-3de9-4817-b9b5-235130fd0e56.png)

------------
### Backstory
I saw jimilinuxguy post an idea of having a TouchTunes/Jukebox remote app, and it was basically one screenshot and a link to a repo. At the time, I was reading the Hello World plugin tutorial and wanted to give coding a plugin a shot.

Using the Music_Player plugin as a starting point (instead of using the Hello World example) I began the journey. By referencing some of the code in Universal RF Remote app, I started to get a basic idea of how the Flipper Zero device works.

After a few days of coding, I managed to get the app running pretty good. I proceed with a layout change which helped me learn how to create/import images (as icons). Also, I implemented a few ideas/suggestions which were brought up by various users in the Flipper Devices Discord, and this project is now released.

------------
### Credits
- [Jimilinuxguy](https://github.com/jimilinuxguy) for code base reference and the initial idea
- "Red_Link2" for creating the D-Pad animation
- "Shitposter Simulator" for the idea of adding a notification message for flashing the LED
- A bunch of others for convincing me to release this and also for just being generally good people.
