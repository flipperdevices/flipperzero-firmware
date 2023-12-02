# Flipboard Common Code

This repository contains common code used by Flipboard projects.  Please let us know if you have any questions or comments!

File names are prefixed with the component name, e.g. `app_menu_i.h` is the private header for the `app_menu` component, while `app_menu.h` is the public header.

## app_menu

The AppMenu module is used to create and show the main application menu.

## backlight

The Backlight module is responsible for controlling the backlight.  You can turn the backlight on, off, or force it off.

## button_config

The ButtonConfig module is used to configure the buttons on the flipboard.

## button_model

This ButtonModel  type is used to store the settings for a button. For example the color, frequency, message, and keystrokes to use when a button is pressed.

- _TODO: Should this allow for extra data to be stored with the button?_

## button_monitor

The ButtonMonitor module is used to monitor the buttons on the flipboard. When a button is pressed, the ButtonMonitor will call the callback function.

## config_colors

config_colors.h contains the configuration of the LED colors. Feel free to add more colors, but be sure to add them to the color_names and color_values arrays.

## config_keystroke

config_keystroke.h contains the configuration of the keystrokes.  You can add new keystrokes here, and they will be available in the config.  There is a limit of 256 keystrokes.  Be sure to update the keystroke_names array with the name of the keystroke.  The keystroke_values array contains the HID values of the keystrokes.  The index of the keystroke in this array is the same as the index of the keystroke in the keystroke_names array.  The keystroke_count_names array contains the number of times to repeat the keystroke.  This array should start at value 0, and increment by 1s.

## config_tones

config_tones.h contains the configuration of the tones.  The tones are a set of frequencies that can be played on the buzzer.  The tones (in Hz) are defined in the tone_values array.  The index of the tone in this array is the same as the index of the tone in the tone_names array.

## flipboard

Typically you will create a Flipboard application in your main function like this:
```c
     Flipboard* app = flipboard_alloc(...);
     view_dispatcher_run(flipboard_get_view_dispatcher(app));
     flipboard_free(app);
```
  
If you have custom configuration, you can use `flipboard_override_config_view`.  If you have custom data, you can use `flipboard_model_set_custom_data`.

## flipboard_file

The FlipboardFile module is used to read and write configuration files for flipboard apps.

## flipboard_model

The FlipboardModel contains all the data needed for the flipboard application.  There are methods to get and set the data & to perform actions with the data, like sending text, playing tones, etc.

## flipboard_model_ref

FlipboardModelRef is a reference to a FlipboardModel, used to pass a FlipboardModel to UI components that cant take a pointer to an existing FlipboardModel.

## keyboard

A Keyboard module is used to send key codes to the host using the USB cable connected to the Flipper Zero.

## keystroke_selector

The KeystrokeSelector module is used to select a keystroke. The view will display a grid of keys. The user can scroll through the keys using the dpad buttons. The user can select a key by pressing the OK button. The view will call a callback when a key is selected.

## leds

The Leds module is used to control the addressable LEDs on the flipboard, and also the status LED.

## menu_callback

The MenuCallback module is used to return a specific view id from ViewNavigationCallback. The id is matched by a large switch so it can only handle a limited number of values (if to large of id is requested, the 0 view will be returned).

## speaker

The Speaker module is used to play tones on the internal Flipper Zero speaker.