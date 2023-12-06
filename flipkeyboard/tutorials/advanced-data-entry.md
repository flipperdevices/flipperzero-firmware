# Tutorial: Advanced data entry

## Overview

In this example we will launch notepad (on Windows) and then type a message.  Between the keystrokes, we will add some delays.  We will make the application launched and message configurable, so we can quickly choose a different application or message.

## Step 1: Open the FlipKeyboard app

- On your Flipper Zero, open `Apps` folder.
- Choose `GPIO` app.
- Choose `Flipboard Keyboard` app.

## Step 2: Configure the Action 1 

- Choose `Config` from the main menu of the FlipKeyboard.
- Choose `Action 1 (button 1)` from the configuration menu.

## Step 3: Windows + R to run an app, then delay

- Choose `Add Keystroke` from the configuration menu.
- Choose `L-WIN` for the Keystroke.  (You can click OK to select the key from the on-screen keyboard; it looks like the 4 square windows logo).
- Choose `Add Keystroke` from the configuration menu.
- Choose `r` for the Keystroke.  (You can click OK to select the key from the on-screen keyboard).
- Choose `Add Keystroke` from the configuration menu.
- Choose `Delay` for the Keystroke.  (This option is NOT in the on-screen keyboard).
- Change the `Count` to `12` (Each delay is 100ms, so this will be a 1.2 second delay).

## Step 4: Type "notepad", press enter, then delay

- Choose `Add Keystroke` from the configuration menu.
- Choose `Msg 1` for the Keystroke.  (You can click OK to select the key from the on-screen keyboard).
- Scroll down to `Message 1` and click OK to edit the message.  Enter the text `notepad` and choose `save` to save the message.
  - NOTE: To clear a message you cannot delete all of the text. Instead, you must enter a space character.  This is a limitation in the current keyboard widget module that is provided by Flipper.
- Choose `Add Keystroke` from the configuration menu.
- Choose `Enter` for the Keystroke.  (You can click OK to select the key from the on-screen keyboard).
- Choose `Add Keystroke` from the configuration menu.
- Choose `Delay` for the Keystroke.  (This option is NOT in the on-screen keyboard).
- Change the `Count` to `20` (Each delay is 100ms, so this will be a 2.0 second delay).

## Step 5: Type "Hello World".

- Choose `Add Keystroke` from the configuration menu.
- Choose `Msg 2` for the Keystroke.  (You can click OK to select the key from the on-screen keyboard).
- Set the `Count` to `5` (This will type the message 5 times).
- Scroll down to `Message 2` and click OK to edit the message.  Enter the text `Hello World!` and choose `save` to save the message.

- NOTE: For more advanced messages, see the [Message tutorial](./message.md).

## Step 6: Save the configuration

- Press the `Back` button to return to the list of actions.
- Press the `Back` button to return to the main menu.

## Step 7: Run the keyboard

- Choose `Flipboard Keyboard` from the main menu.
- Make sure your Flipper Zero is connected to your PC using the USB cable.
- Press the Leftmost button on the FlipBoard to run the `Action 1` that you programmed.
