# GPIO_BLINK
This is the classic "Hello World" of GPIO, where we blink an LED.  You will need an LED (any color is fine) and a resistor (220 ohms - 1 kilohm should be fine).  Connect the long side of the LED into pin A7.  Connect the short size of the LED to one side of the resistor (doesn't matter which side).  Connect the other side of the resistor to any of the pin GND.

Run the application and the LED should start blinking.  Hold the back button to exit.

YouTube video that demonstrates blinking the LED:

[![YouTube video that demonstrates blinking the LED](https://img.youtube.com/vi/27RtJAxY2RY/0.jpg)](https://youtu.be/27RtJAxY2RY)
- [https://youtu.be/27RtJAxY2RY](https://youtu.be/27RtJAxY2RY)

## How it works
- For list of pins see [/firmware/targets/f7/furi_hal/furi_hal_resources.c](https://github.com/flipperdevices/flipperzero-firmware/blob/dev/firmware/targets/f7/furi_hal/furi_hal_resources.c) in your firmware repo.

- We define a variable for our LED...
  ```c
  const GpioPin* const pin_led = &gpio_ext_pa7;
  ```

- We initialize the pin for output.
  - ``GpioModeOutputPushPull`` means true = 3.3 volts, false = 0 volts.
  - ``GpioModeOutputOpenDrain`` means true = floating, false = 0 volts.
  ```c
  furi_hal_gpio_init_simple(pin_led, GpioModeOutputPushPull);
  ```

- We turn the LED on (3.3 volts on the pin).
  ```c
  furi_hal_gpio_write(pin_led, true);
  ```

- We wait 500 ms.
  ```c
  furi_delay_ms(500);
  ```

- We turn the LED off (0 volts on the pin).
  ```c
  furi_hal_gpio_write(pin_led, false);
  ```

- We wait 500 ms.
  ```c
  furi_delay_ms(500);
  ```

- We loop if the back button is not currently pressed.
  ```c
  while(furi_hal_gpio_read(pin_back))
  ```

- Typically when a pin is no longer in use, it is set to analog mode.  This causes the pin to be a floating input.
  ```c
  furi_hal_gpio_init_simple(pin_led, GpioModeAnalog);
  ```

## Other ideas
- We could connect the long side of LED (anode) to +3.3volts and then the resistor to pin A7.  We could then use `GpioModeOutputOpenDrain` and then when the A7 pin is pulled to GND the LED would turn on; this is called "active low".