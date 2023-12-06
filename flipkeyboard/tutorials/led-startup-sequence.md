# Tutorial: LED startup sequence

## Overview

When the FlipKeyboard app starts, the LEDs blink in a specific startup sequence.  This tutorial will show you how to customize the startup sequence.

## Step 1: Find the startup sequence code

Open the `flipkeyboard/app.c` file. Whenever the main application menu is displayed, the `CustomEventAppMenuEnter` custom event is triggered.  The FlipKeyboard application registers the `custom_event_handler` function as the handler for custom events from the event dispatcher.  The `custom_event_handler` will invoke the `loaded_app_menu(FlipboardModel* model)` function when the event matches `CustomEventAppMenuEnter`.

Find the `loaded_app_menu(FlipboardModel* model)` code in the `flipboard/app.c` file.

## Step 2: Understand the current startup sequence

```c
static void loaded_app_menu(FlipboardModel* model) {
    static bool initial_load = true;
    FlipboardLeds* leds = flipboard_model_get_leds(model);
    UNUSED(color_names);
    UNUSED(color_values);
    if(initial_load) {
        for(int i = 0; i < 7; i++) {
            flipboard_leds_set(leds, LedId1, (1 << (16 + i)));
            flipboard_leds_set(leds, LedId2, (1 << (0 + i)));
            flipboard_leds_set(leds, LedId3, (1 << (8 + i)));
            flipboard_leds_set(leds, LedId4, (1 << (0 + i)) | (1 << (8 + i)));
            flipboard_leds_update(leds);
            furi_delay_ms(100);
        }
        for(int i = 7; i > 0; i--) {
            flipboard_leds_set(leds, LedId1, (1 << (16 + i)));
            flipboard_leds_set(leds, LedId2, (1 << (0 + i)));
            flipboard_leds_set(leds, LedId3, (1 << (8 + i)));
            flipboard_leds_set(leds, LedId4, (1 << (0 + i)) | (1 << (8 + i)));
            flipboard_leds_update(leds);
            furi_delay_ms(100);
        }
        initial_load = false;
    }

    flipboard_leds_reset(leds);
    flipboard_leds_update(leds);
}
```

- The `static bool initial_load = true;` line is a static variable that is used to track if the startup sequence has been run before.  The startup sequence is only run once, when the application is first loaded.  Once the sequence loads, we will set `initial_load` to `false` so that the sequence doesn't run again.

- The `FlipboardLeds* leds = flipboard_model_get_leds(model);` line is used to get the LEDs from the FlipboardModel, so we can change the colors of the LEDs.

- The `UNUSED(color_names);` and `UNUSED(color_values);` lines are used to suppress compiler warnings about unused variables.  We don't use the color names or values in this routine, but they are part of the common code, so we need to state that the variables are unused.

- The `if (initial_load) {...}` will run the code inside the curly braces if `initial_load` is `true`.  This is the code that runs the startup sequence when we first show the menu!

  - The `for(int i = 0; i < 7; i++) {...}` line is a for loop that will run 7 times.  The `i` variable will start at 0 and increment by 1 each time through the loop.  The loop will run as long as `i` is less than 7.
    - The `flipboard_leds_set(leds, LedId1, (1 << (16 + i)));` line will set the LED with ID `LedId1` to the color value `(1 << (16 + i))`.  The color value is a 32-bit integer that represents the color of the LED in the lower 24-bits (the lowest 8-bits are blue, the next 8-bits are green, the next 8-bits are red).  (1 << 16) is a `00000001b` in the red area.  (1 << 17) is a `00000010b` in the red area.  (1 << 18) is a `00000100b` in the red area.  (1 << 19) is a `00001000b` in the red area.  (1 << 20) is a `00010000b` in the red area.  (1 << 21) is a `00100000b` in the red area.  (1 << 22) is a `01000000b` in the red area.  (1 << 23) is a `10000000b` in the red area.  So this means the LED will be turned on brighter red each time throug the loop.
    - The `flipboard_leds_set(leds, LedId2, (1 << (0 + i)));` line will set the LED with ID `LedId2` to the color value `(1 << (0 + i))`.  The color value is a 32-bit integer that represents the color of the LED in the lower 24-bits (the lowest 8-bits are blue, the next 8-bits are green, the next 8-bits are red).  (1 << 0) is a `00000001b` in the blue area. (1 << 1) is a `00000010b` in the blue area. (1 << 2) is a `00000100b` in the blue area. (1 << 3) is a `00001000b` in the blue area. (1 << 4) is a `00010000b` in the blue area. (1 << 5) is a `00100000b` in the blue area. (1 << 6) is a `01000000b` in the blue area. (1 << 7) is a `10000000b` in the blue area. So this means the LED will be turned on brighter blue each time throug the loop.
    - The `flipboard_leds_set(leds, LedId3, (1 << (8 + i)));` line will set the LED with ID `LedId3` to the color value `(1 << (8 + i))`.  The color value is a 32-bit integer that represents the color of the LED in the lower 24-bits (the lowest 8-bits are blue, the next 8-bits are green, the next 8-bits are red).  (1 << 8) is a `00000001b` in the green area. (1 << 9) is a `00000010b` in the green area. (1 << 10) is a `00000100b` in the green area. (1 << 11) is a `00001000b` in the green area. (1 << 12) is a `00010000b` in the green area. (1 << 13) is a `00100000b` in the green area. (1 << 14) is a `01000000b` in the green area. (1 << 15) is a `10000000b` in the green area. So this means the LED will be turned on brighter green each time throug the loop.
    - The `flipboard_leds_set(leds, LedId4, (1 << (0 + i)) | (1 << (8 + i)));` line will set the LED with ID `LedId4` to the color value `(1 << (0 + i)) | (1 << (8 + i))`.  The color value is a 32-bit integer that represents the color of the LED in the lower 24-bits (the lowest 8-bits are blue, the next 8-bits are green, the next 8-bits are red).  ((1 << 0) | (1 << 8)) will have a `00000001` in the blue and  a `00000001` in the green area; resulting an a cyan color.  So this means the LED will be turned on brighter cyan each time throug the loop.
    - The `flipboard_leds_update(leds);` line will update the LEDs with the new colors.
    - The `furi_delay_ms(100)` line will delay the code for 100 milliseconds.  This will make the LEDs stay on for 100 milliseconds before the next time through the loop.

  - The `for(int i = 7; i > 0; i--) {...}` line is a for loop that will run 7 times.  The `i` variable will start at 7 and decrement by 1 each time through the loop.  The loop will run as long as `i` is greater than 0.
    - The code inside the for loop is the same as the code inside the first for loop, but the `i` variable is decreasing instead of increasing.  This means the LEDs will be more dim each time through the loop.

- Finally we reset the LEDs to off and update the LEDs.  If this isn't the first time we loaded the menu, then only this code runs (which turns off the LEDs).

## Step 3: Replace the startup sequence with something different

Replace the existing function with a new function that does something different inside of the `if (initial_load)` block.
In this example, we speed up the speed of the updates to 50ms, change the loop to 20, and change the colors to random colors.

```c
static void loaded_app_menu(FlipboardModel* model) {
    static bool initial_load = true;
    FlipboardLeds* leds = flipboard_model_get_leds(model);
    UNUSED(color_names);
    UNUSED(color_values);
    if(initial_load) {
        for(int i = 0; i < 20; i++) {
            flipboard_leds_set(leds, LedId1, rand() & 0x00FF0000);
            flipboard_leds_set(leds, LedId2, rand() & 0x0000FF00);
            flipboard_leds_set(leds, LedId3, rand() & 0x000000FF);
            flipboard_leds_set(leds, LedId4, rand() & 0x00FFFFFF);
            flipboard_leds_update(leds);
            furi_delay_ms(50);
        }
        initial_load = false;
    }

    flipboard_leds_reset(leds);
    flipboard_leds_update(leds);
}
```

- rand() is a function that returns a random number.  
- We use `& 0x00FF0000` to clear all the non-red colors (resulting in some random red color)
- We use `& 0x0000FF00` to clear all the non-green colors (resulting in some random green color)
- We use `& 0x000000FF` to clear all the non-blue colors (resulting in some random blue color)
- We use `& 0x00FFFFFF` to clear all the non-colors (resulting in some random color)

- You can use `&`, `|`, `<<` and other operators to create your own effects.

## Step 4: Build the firmware & launch the app

  - Step 4a: Make sure the Flipper is connected and that nothing is using the serial port (close qFlipper, CLI windows, etc.)
  - Step 4b: Open a build task window in Visual Studio Code by pressing `Ctrl+Shift+B`.
  - Step 4c: Choose `[Debug] Launch App on Flipper`

## Step 5: Your new startup sequence should happen when the application first starts!

- You can exit the application, and re-enter the application using `Apps` -> `GPIO` -> `Flipboard Keyboard` on your Flipper Zero to see the startup sequence again.
- For debugging, it may be faster to comment out the `initial_load = false;` line so that the startup sequence runs every time you enter the application menu (go to About page and click `Back` button to see animation again).

## Step 6: Create your startup sequence and share it with us!

- [Join Discord Server](https://discord.gg/H89Jzjty6m)

- Share your startup sequence in the `#flipboard` channel.
