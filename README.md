Tama P1 Emulator for Flipper Zero
=======================================

This is a tama P1 Emulator app for Flipper Zero, based on [TamaLIB](https://github.com/jcrona/tamalib/).

![Alt Text](tama.gif)

How to play
-----------
Create a `tama_p1` folder in your microSD card, and put the ROM as `rom.bin`.
Use a search engine to find the Tamagotchi ROM. There is a file named `a`. 
Rename this to `rom.bin`. 

- Left button is A.
- OK is B. 
- Right button is C. 
- Holding the Down button functions the same as press both A and C, which mutes the volume. 
- Hold the Back button to save and exit.


Building
--------
Move this folder into flippers applications/plugins/tama_p1. 


Launching the app, directly from console to flipper: 
`./fbt launch_app APPSRC=applications\plugins\tama_p1`

Run the following to compile icons:
```
scripts/assets.py icons applications/tama_p1/icons applications/tama_p1/compiled
```

Note: you may also need to add `-Wno-unused-parameter` to `CCFLAGS` in
`site_cons/cc.scons` to suppress unused parameter errors in TamaLIB.

Debugging
---------
Using the serial script from [FlipperScripts](https://github.com/DroomOne/FlipperScripts/blob/main/serial_logger.py) 
it is easy to add direct logging after running the application: 
`python .\serial_logger.py`

`./fbt launch_app APPSRC=applications\plugins\tama_p1;  python .\serial_logger.py`


Implemented
-----------
- Basic emulation
- Input
- Sound
- Saving/Loading emulator state (stored in `/ext/tama_p1/save.bin`)
- Mute button combo shortcut (down = A+C)

To-do
-----
- more than one save slot
- In-game reset
- Test mode?
- Volume adjustment
