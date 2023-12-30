# FlipSignal

- [Overview](#overview)
- [Running the app](#running-the-app)
- [Configuration](#configuration)
- [Using the app](#using-the-app)
- [Tutorials](#tutorials)
  - [Video tutorials](#video-tutorials)
  - [Written tutorials](#written-tutorials)
- [Updates](#updates)

## Overview
The goal is to create an application that allows you to quickly send signals using a FlipBoard.

Currently the application sends a sub-ghz signal (RAW or Protocol), and then it sends an IR signal.  The configuration of signals is done via files rather than using the Config menu option.

## Running the app

Make sure your FlipBoard is connected to your Flipper Zero.  On your Flipper Zero, press the `Ok` button.  Scroll down to `Apps` and click the `Ok` button.  Select the `GPIO` folder.  Select the `FlipBoard Signal` application. You should see the FlipBoard LEDs do their start-up sequence.

## Configuration
You need to copy your Sub-GHz files into your `SD Card/subghz` folder.

You need to copy your `flipboard.ir` file into your `SD Card/infrared` folder.  The `name:` field should be `Flip1`, `Flip2`, `Flip4` & `Flip8` for the 4 actions!!!  The actions are case-sensitive; so they must start with a capital F and then lowercase the remaining letters.  If the file is not created or no matching name exists, then a fallback file of `SD Card/infrared/assets/tv.ir` will be used along with default names (Power, Mute, Ch_prev and Ch_next).

- Action 1 will run `Flip1.sub` and then send IR signal `Flip1`.  If IR signal does not exist, it will send IR TV signal "Power".
- Action 2 will run `Flip2.sub` and then send IR signal `Flip2`.  If IR signal does not exist, it will send IR TV signal "Mute".
- Action 4 will run `Flip4.sub` and then send IR signal `Flip4`.  If IR signal does not exist, it will send IR TV signal "Ch_prev".
- Action 8 will run `Flip8.sub` and then send IR signal `Flip8`.  If IR signal does not exist, it will send IR TV signal "Ch_next".

The first time you are running the app go to `Config`.  For each Action, configure the `Press color`, `Release color` and `Music note`.  The `Press color` is the color of the LEDs when the button is pressed.  The `Release color` is the color of the LEDs when the button is released.  The `Music note` is the note that will be played when the button is pressed.  The `Music note` is optional.  If you don't want a note to be played, then leave the field set to `Off`.

## Using the app

Choose the `Flipboard Signal` option to enable the FlipBoard.  Press the leftmost button and it should send the Flip1 signal.  If you did not configure any signals, then it will send the IR TV signal "Power".  The green status LED in the FlipBoard will blink as it sends the various signals.  Once every signal has been sent, the green status LED will turn off.

## Tutorials

### Video tutorials
[![Flipper Zero: FlipSignal send IR/SUB with your FlipBoard](https://img.youtube.com/vi/uDaISCAcKpk/0.jpg)](https://youtu.be/uDaISCAcKpk)
<br/>[Flipper Zero: FlipSignal send IR/SUB with your FlipBoard](https://youtu.be/uDaISCAcKpk)

### Written tutorials


## Updates
- V3.1 : Fix bug with second button not working.
- V3.0 : Renamed files AGAIN.  :(   `Flip1.sub`, `Flip2.sub`, `Flip4.sub`, `Flip8.sub` and the IR entries to `Flip1`, `Flip2`, `Flip4`, `Flip8`.
- V2.0 (was called v1.1): Flipper keyboard doesn't have a dash, so renamed the files to `Flip1.sub`, `Flip2.sub`, `Flip3.sub`, `Flip4.sub` and the IR entries to `Flip1`, `Flip2`, `Flip3`, `Flip4`.