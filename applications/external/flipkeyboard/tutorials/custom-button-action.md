# Tutorial: Custom Button Action

## Prerequisites

- Make sure you have your development environment set up as described in [Getting Started](../../README.md#building-with-vscode-visual-studio-code).
- Make sure you have copied the `flipkeyboard` folder to your firmware `applications_user` folder.

## Overview

When buttons on the FlipBoard are pressed, we can write code to perform **any** action we want.  

In this tutorial, keeping with the theme of a FlipKeyboard, we will write custom code to press and hold `ALT`, press and release `2` on the numeric keypad, press `0` on the numeric keypad, then release both the `0` and `ALT` buttons.  

Typing values on the numeric keypad while holding the Alt button is called an "Alt+code". Using the value of `20` will result in the [Pilcrow](https://en.wikipedia.org/wiki/Pilcrow) `¶` character being typed; which we use as a line-feed in our [Message field](message.md).  

Here is a handy [table of Alt+codes](https://i.pinimg.com/originals/df/cf/09/dfcf09b517a2f161232fbaa9e30c0001.jpg) that you might want to try!

## Editing the code 

- Step 1: In Visual Studio Code, open the `applications_user/flipkeyboard/app.c` file

- Step 2: Scroll down until you find the line that says `void flipboard_debounced_switch(void* context, uint8_t old_button, uint8_t new_button) {`

    ```c
    void flipboard_debounced_switch(void* context, uint8_t old_button, uint8_t new_button) {
        Flipboard* app = (Flipboard*)context;
        FlipboardModel* model = flipboard_get_model(app);
        uint8_t reduced_new_button = flipboard_model_reduce(model, new_button, false);

        FURI_LOG_D(
            TAG, "SW EVENT: old=%d new=%d reduced=%d", old_button, new_button, reduced_new_button);

        flipboard_model_update_gui(model);
        
        ButtonModel* bm = flipboard_model_get_button_model(model, reduced_new_button);
        flipboard_model_set_colors(model, bm, new_button);
        flipboard_model_send_keystrokes(model, bm);
        flipboard_model_send_text(model, bm);
        flipboard_model_play_tone(model, bm);
    }
    ```

  - The first line declares the function named `flipboard_debounced_switch`, which takes 3 parameters.  The first parameter is a `void*` context; which is a pointer to the Flipboard application.  The second parameter is a `uint8_t` named `old_button`.  The `old_button` value is the button state before the button was pressed.  The third parameter is a `uint8_t` named `new_button`.  The `new_button` value is the button state after the button was pressed.  The `{` starts the function body.

  - The next line `Flipboard* app = (Flipboard*)context;` declares a local variable named `app` of type `Flipboard*` (a pointer to a Flipboard application).  The value of `app` is set to the value of the `context` parameter, which is a pointer to the Flipboard application.  We do this because `void*` is a generic pointer type, and we need to cast it to a `Flipboard*` pointer type so we can access the Flipboard application.

  - The next line `FlipboardModel* model = flipboard_get_model(app);` declares a local variable named `model` of type `FlipboardModel*` (a pointer to a FlipboardModel).  The value of `model` is set to the value returned by the `flipboard_get_model` function, which takes a `Flipboard*` pointer as a parameter, and returns a `FlipboardModel*` pointer.  The `flipboard_get_model` function is defined in `common/flipboard.c` and returns the `model` member of the `Flipboard` structure.

  - The next line `uint8_t reduced_new_button = flipboard_model_reduce(model, new_button, false);` declares a local variable named `reduced_new_button` of type `uint8_t` (an unsigned 8-bit integer).  The value of `reduced_new_button` is set to the value returned by the `flipboard_model_reduce` function, which takes a `FlipboardModel*` pointer, a `uint8_t` value (containing the buttons that were pressed), and a `bool` (leftmost button should win) as parameters, and returns a `uint8_t` (reduced new button) value.  If the application is defined as allowing multiple buttons, this will just return the value of `new_button`.  If the application is defined as only allowing a single button, this will return the value of the leftmost button that was pressed (if the third parameter is true) or the rightmost button that was pressed (if the third parameter is false).

  - The next line `FURI_LOG_D(TAG, "SW EVENT: old=%d new=%d reduced=%d", old_button, new_button, reduced_new_button);` logs a debug message to the serial port.  NOTE: One challenge is that since our USB cable is used as a HID keyboard, the CLI LOG command won't be available via the USB cable.

  - The next line `flipboard_model_update_gui(model);` calls the `flipboard_model_update_gui` function, which takes a `FlipboardModel*` pointer as a parameter.  GUI stands for the Graphical User Interface, so this method will request an update of the Flipper's 128x64 screen.

  - The next line `ButtonModel* bm = flipboard_model_get_button_model(model, reduced_new_button);` declares a local variable named `bm` of type `ButtonModel*` (a pointer to a ButtonModel).  The value of `bm` is set to the value returned by the `flipboard_model_get_button_model` function, which takes a `FlipboardModel*` pointer and a `uint8_t` value (containing the reduced new button) as parameters, and returns a `ButtonModel*` pointer.  The ButtonModel contains all of the configuration for a single button combination.

  - The next line `flipboard_model_set_colors(model, bm, new_button);` calls the `flipboard_model_set_colors` function, which takes a `FlipboardModel*` pointer, a `ButtonModel*` pointer, and a `uint8_t` value (containing the new button) as parameters.  This function will set the color of the LEDs on the FlipBoard based on the ButtonModel information.

  - The next line `flipboard_model_send_keystrokes(model, bm);` calls the `flipboard_model_send_keystrokes` function, which takes a `FlipboardModel*` pointer and a `ButtonModel*` pointer as parameters.  This function will send the keystrokes to the computer based on the ButtonModel information.

  - The next line `flipboard_model_send_text(model, bm);` calls the `flipboard_model_send_text` function, which takes a `FlipboardModel*` pointer and a `ButtonModel*` pointer as parameters.  This function will send the text to the computer based on the ButtonModel information.

  - The next line `flipboard_model_play_tone(model, bm);` calls the `flipboard_model_play_tone` function, which takes a `FlipboardModel*` pointer and a `ButtonModel*` pointer as parameters.  This function will play the tone based on the ButtonModel information.

  - The next line `}` ends the `flipboard_debounced_switch` function.

- Step 3: Add our new code immediately after the `flipboard_model_update_gui(model);` line.

```c
    if(new_button == (1 | 2 | 4 | 8)) {
        // Alt+20 (Paragraph symbol/pilcrow)
        FlipboardKeyboard* keyboard = flipboard_model_get_keyboard(model);
        uint32_t delay = 50;
        flipboard_keyboard_send_keycode(keyboard, HID_KEYBOARD_L_ALT | KEY_MOD_LEFT_ALT);
        furi_delay_ms(delay);
        flipboard_keyboard_send_keycode(keyboard, HID_KEYPAD_2 | KEY_MOD_LEFT_ALT);
        furi_delay_ms(delay);
        flipboard_keyboard_release_keycode(keyboard, HID_KEYPAD_2);
        furi_delay_ms(delay);
        flipboard_keyboard_send_keycode(keyboard, HID_KEYPAD_0 | KEY_MOD_LEFT_ALT);
        furi_delay_ms(delay);
        flipboard_keyboard_release_all(keyboard);
        furi_delay_ms(delay);
        return;
    }
```

- Step 4: change our if condition to only happen when the 3rd and 4th button are pressed.

  - Change the text `if(new_button == (1 | 2 | 4 | 8)) {` to instead use the text `if(new_button == (0 | 0 | 4 | 8)) {`

  - The `if (...) {...}` statement will check if the condition inside the parenthesis is true, and if so, execute the code inside the curly braces.  The `new_button` value is a bitfield, so we can use the `|` operator to combine the bits we want to check.  The `0` value is used to indicate that we don't care about the first two buttons, so we use `0` for the first two bits.  The `4` value is used to indicate that we want the 3rd button to be pressed, so we use `4` for the third bit.  The `8` value is used to indicate that we want the 4th button to be pressed, so we use `8` for the fourth bit.  The `if` statement will check if the `new_button` value is equal to the value we specified, and if so, execute the code inside the curly braces.

  - Bitfields are binary (base-2) counting, so they go 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, ...  

  - The `|` operator is the bitwise OR operator, which combines the bits together into a single value.

  - The `==` operator is the equality operator, which checks if the two values are equal.  **NOTE:** Be sure to use `==` and not `=`; the `=` operator is the assignment operator, which sets the value of the variable on the left to the value on the right and then is treated as 'true' if the resulting value is not zero!!!

- Step 5: Understanding the rest of the code we added...

  - The first line `// Alt+20 (Paragraph symbol/pilcrow)` is a comment.  Comments are used to document the code, and are ignored by the compiler.  Comments start with `//` and continue until the end of the line.

  - The next line `FlipboardKeyboard* keyboard = flipboard_model_get_keyboard(model);` declares a local variable named `keyboard` of type `FlipboardKeyboard*` (a pointer to a FlipboardKeyboard).  The value of `keyboard` is set to the value returned by the `flipboard_model_get_keyboard` function, which takes a `FlipboardModel*` pointer as a parameter, and returns a `FlipboardKeyboard*` pointer.  The FlipboardKeyboard contains all of the configuration for the FlipBoard's keyboard.

  - `uint32_t delay = 50;` declares a local variable named `delay` of type `uint32_t` (an unsigned 32-bit integer).  The value of `delay` is set to `50`.  We use this variable later to control the delay between pressing and releasing keystrokes.  Having it in one place makes it easier to change later (instead of having to update all 5 of our `furi_delay_ms` calls).

  - `flipboard_keyboard_send_keycode(keyboard, HID_KEYBOARD_L_ALT | KEY_MOD_LEFT_ALT);` calls the `flipboard_keyboard_send_keycode` function, which takes a `FlipboardKeyboard*` pointer and a `uint8_t` value (containing the key code) as parameters.  This function will send the key code to the computer based on the FlipboardKeyboard information.  The `HID_KEYBOARD_L_ALT` value is the key code for the left Alt key.  The `KEY_MOD_LEFT_ALT` value is the modifier for the left Alt key.  The `|` operator is the bitwise OR operator, which combines the bits together into a single value.  The `flipboard_keyboard_send_keycode` sends a PRESS event, so we will need to send a RELEASE event later.  The `Left Alt` button is being held down.

  - `furi_delay_ms(delay);` calls the `furi_delay_ms` function, which takes a `uint32_t` value (containing the number of milliseconds to delay) as a parameter.  This will delay the 50 milliseconds, since that is what we set our delay to easlier.

  - Next we send `HID_KEYPAD_2 | KEY_MOD_LEFT_ALT`, which is the `2` button on the **numeric keypad**.  We specify that the `Left Alt` button is still being held down.  Then we delay for 50 milliseconds.

  - `flipboard_keyboard_release_keycode(keyboard, HID_KEYPAD_2);` calls the `flipboard_keyboard_release_keycode` function, which takes a `FlipboardKeyboard*` pointer and a `uint8_t` value (containing the key code) as parameters.  The `flipboard_keyboard_release_keycode` sends a RELEASE event, so we are no longer holding down the `2` button.  NOTE: The `Left Alt` button is still being held down.  Then we delay for 50 milliseconds.

  - Next we send `HID_KEYPAD_0 | KEY_MOD_LEFT_ALT`, which is the `0` button on the **numeric keypad**.  We specify that the `Left Alt` button is still being held down.  Then we delay for 50 milliseconds.

  - `flipboard_keyboard_release_all(keyboard);` calls the `flipboard_keyboard_release_all` function, which takes a `FlipboardKeyboard*` pointer as a parameter.  The `flipboard_keyboard_release_all` will release any keys that are still pressed down (so this releases both the `0` and the `Left Alt` keys.  Then we delay for 50 milliseconds.

  - `return;` exits the function early, so we don't do any of the other actions.  This means the button never changed colors, typed the configured text, or made sound when we pressed it.

- Step 6: Save the file

- Step 7: Build the firmware & launch the app

  - Step 7a: Make sure the Flipper is connected and that nothing is using the serial port (close qFlipper, CLI windows, etc.)
  - Step 7b: Open a build task window in Visual Studio Code by pressing `Ctrl+Shift+B`.
  - Step 7c: Choose `[Debug] Launch App on Flipper`
