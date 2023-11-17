# FlipBlinky

<img src="./gallery/banner.png">

The FlipKeyboard application turns your Flipper Zero into a blinky badge for wearing at conferences.  The application is written in C and uses the flipboard common code for the majority of the functionality.

## Running the app

<img src="./gallery/01-blinky-main-menu.png" width="200px">

Make sure your FlipBoard is connected to your Flipper Zero.  Choose the "Flipboard Blinky" option to launch the app.  You should see the FlipBoard LEDs light up and start animating.

Press the leftmost button to decrease the delay between frames.  Press the adjacent button to increase the delay between frames.  The speed in milliseconds is displayed in the bottom right corner when you press a button.

Press the rightmost button to change the animation.  Press the adjacent button to decrease the animation id.  Press both buttons to run the off animation.  The animation id is displayed in the bottom right corner when you press a button.  

## Customizing the app

### Adding a new nametag
Currently the `nametag.png` is hardcoded into the app.  You can replace the [assets/nametag.png](./assets/nametag.png) file with your own 128x64 black and white image, but you will need to recompile the app.  In a future release, the nametag image will be loaded from the SD card.

### Adding new colors
The [common/config_colors.h](../common/config_colors.h) file contains the color definitions for the app.  You can add new colors by adding a new entry to the `color_names` array.  Then in the `LedColors` enum, add a new color entry; the hex value is the amount of red [00-FF], green [00-FF] & blue [00-FF].  Finally, add a new entry to the `color_values` array.  NOTE: The order of the color_values must match the same order as the `color_names`.

Once you have added the colors, you are ready to edit the [app.c](./app.c) file.  Find the `flipboard_reset_effect` function and replace the colors for the various effects with your new colors!

### Adding a new effect
You will need to edit the [app.c](./app.c) file.  First find the `flipboard_blinky_model_alloc` and increase the `max_effect_id` value, so that it contains all of the effects you want to add.  NOTE: The `max_effect_id` will be the effect to turn off all of the LEDs.

Find `flipboard_reset_effect` and scroll to the bottom of the function, just before the `default:` statement.  You will want to add your new case statement here.  The case number should be for one bigger than the previous case number.  You can set colors[0..3], which are the LED colors that will scroll by default for this effect. colors[4 and 5] are additional colors that can be used if you create your own effect code.

If you choose to have a different effect than just scrolling, find `flipboard_do_effect` and scroll the the bottom of the function, just before the `default:` statement.  You will want to add your new case statement here.  The case number **must match** the number you used in `flipboard_reset_effect`.  If you need to define variables, be sure to wrap your code in `{ ... }`.  Your routine should set fbm->colors[0..3] to the correct hex RGB values.

