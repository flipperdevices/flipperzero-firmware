# IR-Blaster

## Overview
I created a YouTube video to explain how to modify official firmware to use an external IR Blaster.  You can find the video [here](https://www.youtube.com/watch?v=o_Tz68ju4Dg)
[![Flipper Zero: IR Blaster on Official firmware](https://img.youtube.com/vi/o_Tz68ju4Dg/0.jpg)](https://youtu.be/o_Tz68ju4Dg)

In the comments Jeff-ss6qt posted a question about if we can detect the IR hardware and switch automatically.  It turns out that we can!  This tutorial will show you how!

## How to use this patch
You can apply this patch to your official firmware...
1. Recursively clone the repo like in the video.
2. Instead of manually making the edits, 4:40-7:00.
  - 2a. Copy this [ir-blaster.patch](./ir-blaster.patch) file into the root folder of your firmware (same folder as where the fbt file is).
  - 2b. In VS Code, right click on `ir-blaster.patch` and choose `Open in Integrated Terminal`
  - 2c. In the terminal window type: `git apply ir-blaster.patch`
3. 7:00-7:50 - In the source control pane, you should see a bunch of edits to `furi_hal_infrared.c`.  You will not have any edits to `infrared_app.c`.
4. 7:50 - Ctrl+Shift+B then [Release] Flash (USB, with resources)

Your Flipper Zero will now automatically detect if you have an IR Blaster connected and use it instead of the internal IR LED.  If you do not have an IR Blaster connected, it will use the internal IR LED!

## How does it work?
When the IR Blaster is attached and use that instead of the built-in IR.  When you remove the module, it reverts back to the built-in IR -- no setting changes required!  

It automatically provides the +5 volts during the transmission, so no need to go into GPIO settings and change with that either (unless you want the status LED always on).

The detection is just based on "something" being present on pin A7, so other modules may also cause the Flipper Zero to no longer use the built-in IR port but typically you should be using your IR Blaster.  We use an internal pull-up resistor on pin A7 and then read the value.  If it is `high` we assume something nothing is connected.  If it is `low` we assume the IR Blaster is connected (although all we really know is some module is pulling the pin toward ground.)

This is really just for those times when you are needing to adjust your TV and don't have the accessory nearby -- simply unplug whatever is connected to your Flipper and do a quick IR transmit using the built-in LED.  In general, I would always recommend using the IR Blaster module.

You can find more information about the IR Blaster module I used in the description of the YouTube video.

## More information
If you are interested in learning more about the Flipper Zero, check out my YouTube channel: https://youtube.com/@MrDerekJamison

I also have a Discord server where you can ask questions and share your projects: https://discord.com/invite/NsjCvqwPAd