# GPIO 7-Segment Output
## Introduction
This is a GPIO push-pull output demo application for driving a 7-segment display on the Flipper Zero.  The goal of this project is to show application developers how GPIO works for push-pull output.  This project was derived from the [\plugins\basic](..\..\plugins\basic\README.md) tutorial.


## Installation Directions
This project is intended to be overlayed on top of an existing firmware repo.
- Clone, Build & Deploy an existing flipper zero firmware repo.  See this [tutorial](/firmware/updating/README.md) for updating firmware.
- Copy the "gpio_7segment" [folder](..) to the \applications\plugins\gpio_7segment folder in your firmware.
- Build &amp; deploy the firmware.  See this [tutorial](/firmware/updating/README.md) for updating firmware.
- NOTE: You can also extract the gpio_output_demo.FAP from resources.tar file and use qFlipper to copy the file to the SD Card/apps/Misc folder.


## Running the updated firmware
These directions assume you are starting at the flipper desktop.  If not, please press the back button until you are at the desktop.

- Press the OK button on the flipper to pull up the main menu.
- Choose "Apps" from the menu.
- Choose "Gpio" from the sub-menu.
- Choose "GPIO 7-Segment Output"

- The flipper should say "GPIO 7-Segment".
- Click the "OK" button to display a random number from 1-6.

- Press the BACK button to exit.


## How it works
- application.fam
  - specifies the settings for our application.
  - adds "gpio" to requires.

- gpio_7segment.png
  - The icon for our application that shows up in the "Misc" folder.

- gpio_7segment_app.c
  - Similar to the plugin\basic tutorial.
  - bool digits[70] is defined to store the segments to glow for different digits.
    - each group of 7 booleans represents a number.
    - note: in production code you could replace with a single byte [8-bits] to represent the segments to turn on/off.  Like bit 0 is middle LED, bit 1 is bottom right, etc.
  - gpio_7segment_render_callback(...) callback paints the canvas.
    - Title of application
    - two boxes (looks like a 7-segment display)
    - index is set to the first index with our LED information
      - for example to draw a 2 we index at 7*2 = 14 (which is the 15th bool, since it starts at a count of 0.)
      - digits[index++] ? "A7" : "a7"
        - digits[index] is the bool (true/false) from our digits table.
        - if it is true we will use the text "A7" (CAPS to show on)
        - if it is false we will use the text "a7" (lowercase)
        - index++ will increment index to the next number.
    - data->invert ? "GND" : "3.3v"
      - if invert is set to true we show the text "GND" (pins will GND to glow.)
      - if invert is set to false we show the text "3.3v" (pins will 3.3 to glow.)
    - We show the number we are displaying on the 7-segment display.
  - gpio_7segment_show(...) displays a number.
    - index is set to the first index with our LED information
      - for example to draw a 2 we index at 7*2 = 14 (which is the 15th bool, since it starts at a count of 0.)
    - furi_hal_gpio_write(&gpio_ext_pa7, digits[index++] ^ invert);
      - THIS IS THE CODE THAT CHANGES THE STATE OF THE PIN!
      - gpio_ext_pa7 is the pin (a7).  You can see all pin names at \firmware\targets\f7\furi_hal\furi_hal_resources.c
      - digits[index++] ^ invert
        - digits[index] is the bool (true/false) from our digits table.
        - if invert is false, (digits[index++] ^ false) == digits[index++] (so same as table.)
        - if invert it true, (digits[index++] ^ true) == !digits[index++]  (so inverted from table value.  This is needed for common anode 7-segment LEDs.)
  - gpio_7segment_disconnect_pin(...) disconnects a pin.
    - GpioModeOutputOpenDrain means that when the value is false the pin will be GND, but when the value is true the pin will be disconnected.
    - GpioPullNo means there is no pull-up resistor between 3.3v and pin & there is no pull-down resistor between GND and pin.
    - we write a true, so pin is disconnected.
    - Some people use Input with Push/Pull-No to disconnect pins while others use OutputOpenDrain.  I'm not sure if the pins are really disconnected just because you aren't reading, so I think I would prefer to use OutputOpenDrain.
  - gpio_7segment_app(...) is our entry point
    - furi_hal_gpio_init_simple(&gpio_ext_pa7, GpioModeOutputPushPull);
      - this sets the A7 pin to be OutputPushPull.
        - PushPull means pin will either be GND or 3.3volts.
        - (As opposed to OutputOpenDrain when pin is GND or disconnected.)
        - We use "init_simple" instead of "init", because other params aren't needed.
    - context->data->digit = (furi_hal_random_get() % 6) + 1;
      - when OK button is pressed we use furi_hal_random_get() to get a random number (I think from the secure random number generator).
      - (x % 6) will return the remainder from dividing x by 6.  Which should be a number between 0 and 5.  We then add 1 to the value so we get a value between 1 and 6.
    - gpio_7segment_disconnect_pin(&gpio_ext_pa7);
      - when our app exits, we disconnect the pins.  If we didn't the number would still show.
