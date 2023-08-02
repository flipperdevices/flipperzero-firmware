# MEMSIC 2125

## Introduction

This is a demostration of using GPIO interrupts to interpret data from the Memsic 2125 (Mx2125) Dual-Axis Accelerometer.

## Installation Directions

This project is intended to be overlayed on top of an existing firmware repo.

- Clone, Build & Deploy an existing flipper zero firmware repo. See this [tutorial](/firmware/updating/README.md) for updating firmware.
- Copy the "memsic_2125" [folder](..) to the \applications\plugins\memsic_2125 folder in your firmware.
- Build &amp; deploy the firmware. See this [tutorial](/firmware/updating/README.md) for updating firmware.
- NOTE: You can also extract the memsic_2125.FAP from resources.tar file and use qFlipper to copy the file to the SD Card/apps/Misc folder.

## Connecting the hardware

The Memsic Mx2125 is a 6-pin IC. From the top view the pins look like the following:
<br/>
<img alt="Mx2125 chip with pin 1 on top-left" src="./memsic_2125_chip.jpg" height=150/>
<br/>

The pins should be connected to the Flipper Zero as follows:

| Memsic | Name | Purpose                                    | Flipper       |
| ------ | ---- | ------------------------------------------ | ------------- |
| Pin 1  | Tout | Temperature Out                            | not connected |
| Pin 2  | Yout | Y-axis PWM Out (100Hz, duty cycle = value) | C0            |
| Pin 3  | GND  | Ground                                     | GND           |
| Pin 4  | GND  | Ground                                     | GND           |
| Pin 5  | Xout | X-axis PWM Out (100Hz, duty cycle = value) | C1            |
| Pin 6  | Vdd  | Drain voltage (3.3V to 5V DC)              | 3v3           |

## Running the updated firmware

These directions assume you are starting at the flipper desktop. If not, please press the back button until you are at the desktop.

- Press the OK button on the flipper to pull up the main menu.
- Choose "Apps" from the menu.
- Choose "Misc" from the sub-menu.
- Choose "Memsic 2125"

- The flipper should say "Memsic 2125 C1:X C0:Y". This is a reminder that the X-Axis (pin 5) connects to pin C1 and Y-Axis (pin 2) connects to pin C0.
- A circle should show in the middle of the screen when the Mx2125 is on a level surface.
- Tilting along the X and Y axis should move the circle.
- The values at the bottom of the screen show the X and Y duty cycle amounts.

  - NOTE: A value of 50% means that the device is level.

- Press the BACK button to exit.

## How it works

- application.fam

  - See [basic demo](../../plugins/basic/README.md) for explanation.

- basic_demo.png

  - See [basic demo](../../plugins/basic/README.md) for explanation.

- basic_demo_app.c

  - See [basic demo](../../plugins/basic/README.md) for explanation.
  - GPIO specific:

    - AxisData is a structure to hold information about an axis.
      - pin is the pin we are monitoring, so we can read the state later.
      - high is the timestamp when the pin last transitioned to high.
      - low is the timestamp when the pin last transitioned to low.
      - value is the duty cycle (which represents the amount of tilt.)
      - reading is set to true when the UI is reading this structure. If this value is true, then in our callback we will not modify value. This is used to ensure access to value is thread safe.
    - DemoData has two AxisData references (for x-axis data and y-axis data.)
    - Our entry point initializes xData with the c1 pin & yData with the c0 pin.
    - furi_hal_gpio_init(xData->pin, GpioModeInterruptRiseFall, GpioPullNo, GpioSpeedVeryHigh) initializes an interrupt to happen on the Rise or Fall of the c1 pin. We don't need a pull-up since the pin will be set to GND or 3.3V by the Mx2125.
    - furi_hal_gpio_add_int_callback(xData->pin, pulse_callback, xData) sets the callback routine to pulse_callback and will pass our xData whenever the pin changes state.
    - We do the same process as the xData for the yData using the c0 pin.
    - pulse_callback(void \*) is our GPIO interrupt callback.
      - The input parameter is actually a pointer to AxisData.
      - furi_hal_cortex_timer_get(0) gets the current timestamp.
      - furi_hal_gpio_read(d->pin) gets the state of the pin (true=HIGH/false=LOW).
      - if we transitioned from high to low to high & our render_callback is not currently reading the value, then we update the value with the new duty cycle number.
    - When exiting we remove the callback routine using furi_hal_gpio_remove_int_callback.
    - In our render_callback(Canvas*, void*) we render the UI
      - We set xData->reading to true before accessing xData->value and then set it to false once we are done accessing the data.
      - We calcuate the X and Y coordinate to draw a circle based on xData->value and yData->value.
