# Alternative Infrared Remote for Flipperzero

It is a plugin like [UniversalRF Remix](https://github.com/ESurge/flipperzero-firmware-unirfremix) but for infrared files. I do this plugin for convenience, because the main IR app need to navigate for different button abit troublesome (buttons like up,down,left,right,back). I found it useful for TV and TV box.

It supports short press and long press input for different ir remote buttons. Tested on the [unleashed firmware version unlshd-058](https://github.com/DarkFlippers/unleashed-firmware/releases/tag/unlshd-058)


## How to install

1. Update unleashed firmware to the version unlshd-058, then download the `ir_remote.fap` from [releases](https://github.com/Hong5489/ir_remote/tags)

2. Put the `ir_remote.fap` file in your flipper's SD card, under `apps/Infrared` folder

You also can install the unleashed firmware with extra apps included

## How to use

1. Similar to UniRF app, put the path of the ir file and the ir button for each button on flipper (UP,DOWN,LEFT,RIGHT,BACK)

The format With `HOLD` one is long press, without is short press

Example of the configuration file (Update: the string format can be in any order):
```
REMOTE: /ext/infrared/Philips_32PFL4208T.ir
UP: Up
DOWN: Down
LEFT: Left
RIGHT: Right
OK: 
BACK: Back
UPHOLD: VOL+
DOWNHOLD: VOL-
LEFTHOLD: Source
RIGHTHOLD: SmartTV
OKHOLD: POWER
```

Leave it empty for the button you don't need

2. Save it as `.txt` file, then create a new folder in your SD card `ir_remote`, put it inside the folder

3. Lastly, you can open the app, choose the configuration file, then you can try out the ir signal for each buttons

4. Hold the back button to exit the app

## How to disable repeat signal

You can add an extra line in the config txt file: `REPEATSIGNAL: false`, so it will not send repeating IR signal when you press the hold button.

Example:
```
REMOTE: /ext/infrared/Philips_32PFL4208T.ir
REPEATSIGNAL: false
UP: Up
DOWN: Down
LEFT: Left
RIGHT: Right
BACK: Back
UPHOLD: VOL+
DOWNHOLD: VOL-
LEFTHOLD: Source
RIGHTHOLD: SmartTV
OKHOLD: POWER
```

## How to build 

You can clone this repo and put it inside the `applications_user` folder, then build it with the command:
```
./fbt fap_ir_remote
```
Or you can build and run it on your flipper with the command:
```
./fbt launch_app APPSRC=applications_user/ir_remote
```

## Screenshots

Choose config file to map

![image](ir.png)

Show all button name in the config file (If empty will show N/A). Upper part short press, Lower part long press

![image2](ir2.png)

## Update
Thanks to [friebel](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/pull/535), [d4ve10](https://github.com/Hong5489/ir_remote/pull/4) and [polarikus](https://github.com/Hong5489/ir_remote/pull/2)
- Added auto create `ir_remote` directory
- Fixed string format 
- Added sending repeat signal for hold button
- Added config for disable sending repeat signal for hold button

## TODO
- Add menu for GPIO setting
- Mulitiple remote in one config file