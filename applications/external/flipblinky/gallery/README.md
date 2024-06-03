# FlipBlinky

## Overview

The FlipBlinky application turns your Flipper Zero + FlipBoard into a blinky badge for wearing at conferences.

You can order your FlipBoard from [@MakeItHackin](https://www.youtube.com/makeithackin) using [tindie](https://www.tindie.com/products/32844/) or [Etsy](https://www.etsy.com/listing/1601295558/).  Be sure to also join our [Discord server](https://discord.gg/KTThkQHj5B).

## Running the app
Make sure your FlipBoard is connected to your Flipper Zero. When you start the app, you should see startup splash screen for a few seconds reminding you to connect the FlipBoard and then the FlipBoard LEDs will do their start-up sequence.
  - Start application : starts light show on the FlipBoard.
  - Configuration application : change what is display on the Flipper Zero screen.
  - About : version and ordering information.
  - Instructions QR code : links to GitHub for detailed instructions (including customizing app).

## Using the app
The FlipBoard has 4 buttons (leftmost is 1, rightmost is 4).  The buttons will glow in different patterns when the application is running
  - Button 1 - lights run quicker.
  - Button 2 - lights run slower.
  - Button 3 - previous animation.
  - Button 4 - next animation.
  - Button 3 **and** Button 4 - Turn off animation.

## Configuration

From the main application menu, choose "Configure application" to change the configuration of the app.  The "Display Source" can be either 'Assets', 'FXBM' or 'Text'.  The "Justification" can be either 'Left', 'Center' or 'Right'.

The Display Source of 'Assets' will use the PNG file that is part of the application. If you want a different image, it is recommended to use FXBM.

The Display Source of 'FXBM' will use a file located on the SD card.  The file should be 'SD Card/apps_data/flipboard/blinky.fxbm'. See below for steps to create your own FXBM file.

The Display Source of 'TEXT' will use the text that was set in the 'Line 1'-'Line 4' fields.  You can change the text to whatever you want.  Choose the line to edit and click OK to be prompted for the new text. This uses your default Flipper Zero keyboard.  In some firmware, long-pressing the 'Ok' button will change the case of the letters.  You cannot save when the text is empty (but you can select a single space; next to the save button; and the line will appear empty).

## Creating an FXBM file
Instructions for generating and copying the 'blinky.fxbm' file to the SD card are below:
- Copy the [blinky_png_to_fxbm.py](https://github.com/jamisonderek/flipboard/blob/main/flipblinky/blinky_png_to_fxbm.py) script to your computer.
- Copy a 128x64 (or smaller) black and white image to your computer named 'blinky.png'.
- Run the python script: "python blinky_png_to_fxbm.py"  (you may need "python3" instead of "python" depending on your system).
- Copy the resulting 'blinky.fxbm' file to the SD card in the 'SD Card/apps_data/flipboard' folder.

## Support

If you have need help, we are here for you.  Also, we would love your feedback on cool ideas for future FlipBoard applications!
Please visit my Discord server [Flipper Zero Tutorials (Unofficial)](https://discord.gg/KTThkQHj5B) and ask your questions in the '#flipboard' or '#general' channel.
