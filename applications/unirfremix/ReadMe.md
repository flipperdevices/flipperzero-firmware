# UniRF Remix App - Minimalized
### Notes
* ##### App Usage
  - Press a button to send the assigned capture file.
  - Press Back button to set how many repeats the app should send. Capped at 5 repeats.
  - Hold Back button to exit app.
  - Only RAW SubGhz captures are supported currently.
  - No skip function.

* ##### Universal RF Map
  - App updated to read map definitions from ```subghz/assets/universal_rf_map.txt```
  - Backwards compatible with [jimilinuxguy Universal RF Remote](https://github.com/jimilinuxguy/flipperzero-universal-rf-remote) map file. You should be able to use the map file as is with both versions.
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
![subremoteremix-mainmenu](https://user-images.githubusercontent.com/982575/174747166-c437a518-4db9-492c-a24d-48586ee751be.gif)

#### Idle Screen:
- Repeat indictator is located at the bottom right

![unirfremix-minimal-mainscreen_idle](https://user-images.githubusercontent.com/982575/174741909-aa1214b2-3d97-4232-ad5c-7e99711b8fa6.png)

#### Sending Screen:
- Button pressed will highlight when sending
- LED indicator will flash

![unirfremix-minimal-mainscreen_send](https://user-images.githubusercontent.com/982575/174741934-ef144ba4-109a-430a-b439-699b863ac61c.png)

#### Invalid Map File:
- If you don't have at least one valid file path set in your ```universal_rf_map``` file, you'll see this screen

![unirfremix-invalidmap](https://user-images.githubusercontent.com/982575/169639438-f1b96944-42c4-476c-9fe2-233d174c6262.png)

#### Missing Map File:
- If you don't have the ```universal_rf_map``` file, you'll see this screen

![unirfremix-missingmap](https://user-images.githubusercontent.com/982575/169639439-6414c81a-3de9-4817-b9b5-235130fd0e56.png)

#### Config Loading:
- If the map file doesn't load properly for some reason, you'll see this screen

![unirfremix-screen-loading](https://user-images.githubusercontent.com/982575/174741703-5632f7d5-ee1e-481e-8d11-7f87bc999c94.png)

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
