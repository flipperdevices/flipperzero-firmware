# BASIC DEMO
## Introduction
This is a basic demo of using GPIO interrupts.

## Installation Directions
This project is intended to be overlayed on top of an existing firmware repo.
- Clone, Build & Deploy an existing flipper zero firmware repo.  See this [tutorial](/firmware/updating/README.md) for updating firmware.
- Copy the "gpio_interrupt_demo" [folder](..) to the \applications\plugins\gpio_interrupt_demo folder in your firmware.
- Build &amp; deploy the firmware.  See this [tutorial](/firmware/updating/README.md) for updating firmware.
- NOTE: You can also extract the gpio_interrupt_demo.FAP from resources.tar file and use qFlipper to copy the file to the SD Card/apps/Misc folder.


## Connecting the hardware
GPIO pin C3 is our interrupt pin (with internal pull-up resistor).  I connect pin 
to a 220 ohm resistor and then the other side of the resistor to one side of a
momentary switch.  Other side of the switch connects to our GND pin.

GPIO A7, A6 and A4 connect to 220ohm resistors then the + or - pins of LED... 
LED1: +A7  -A6
LED2: +A6  -A7
LED3: +A7  -A4
LED4: +A4  -A7
LED5: +A6  -A4
LED6: +A4  -A6

## Running the updated firmware
These directions assume you are starting at the flipper desktop.  If not, please press the back button until you are at the desktop.

- Press the OK button on the flipper to pull up the main menu.
- Choose "Applications" from the menu.
- Choose "GPIO" from the sub-menu.
- Choose "GPIO Interrupt Demo"

- Message "GND pin C3 to stop." should display
- LEDs should blink in order.
- press switch (or connect wire/resistor between C3 and GND)
- LEDs should stop and display should say "Stopped on LED 4." (or whatever LED was lit.)
- Press DOWN button on Flipper Zero to start over.

- Press the BACK button to exit.

## How it works
- application.fam
  - See [basic demo](../../plugins/basic/README.md) for explanation.

- basic_demo.png
  - See [basic demo](../../plugins/basic/README.md) for explanation.

- basic_demo_app.c
  - GPIO specific code:
    - furi_hal_gpio_init(&gpio_ext_pc3, GpioModeInterruptFall, GpioPullUp, GpioSpeedVeryHigh);
      - gpio_ext_pc3 is the GPIO pin we are using for interrupt input.
      - GpioModeInterruptFall - means trigger callback on falling edge (VCC->GND)
      - GpioModeInterruptRise - means trigger callback on rising edge (GND->VCC)
      - GpioModeInterruptRiseFall - means trigger callback on either edge
      - GpioPullUp - Means enable internal pull-up resistor between VCC and pin.
      - GpioPullDown - Means enable internal pull-down resistor between GND and pin.
      - GpioPullNo - Means do not enable internal pull-up/pull-down resistor.
      - GpioSpeedVeryHigh - priority of input?
    - furi_hal_gpio_add_int_callback(&gpio_ext_pc3, demo_gpio_fall_callback, NULL);
      - gpio_ext_pc3 is the GPIO pin we are using for interrupt input.
      - second parameter is callback with signature: "void fnName(void* ctx)"
      - third parameter is object to pass into callback for context (or NULL if no object).
      - once furi_hal_gpio_add_int_callback is invoked, the callback is enabled.
    - demo_gpio_fall_callback(void* ctx)
      - this method gets invoked by our callback (because of previous add_int_callback) wheneven pin C3 gets pulled to GND.
      - interrupt callbacks should try to do mininal amount of processing.
    - furi_hal_gpio_remove_int_callback(&gpio_ext_pc3);
      - removes the callback, so method is no longer invoked on pin transistion.
    