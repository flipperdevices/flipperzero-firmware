# Flipper Air Mouse

## What?

The app allows you to turn your Flipper into a USB or Bluetooth air mouse (you do need an extra module, see the Hardware section below)...

Using it is really simple:
 * Connect the Flipper via a USB cable and pick USB, or pick Bluetooth and pair it with your PC;
 * Hold the Flipper in your hand with the buttons pointing towards the screen;
 * Wave your Flipper like you don't care to move the cursor;
 * Up button for Left mouse click;
 * Down button for Right mouse click;
 * Center button for Middle mouse click;
 * Left and Right buttons for scrolling;
 * Use calibration menu option if you notice significant drift (place your Flipper onto a level surface, make sure it doesn't move, run this option, wait 2 seconds, done).

## Hardware

The custom module is using Bosch BMI160 accelerometer/gyroscope chip connected via I2C.

Take a look into the schematic folder for Gerber, BOM and CPL files, so you can order directly from JLCPCB.

## Software

The code is based on the original Bosch driver and an orientation tracking implementation from the Google Cardboard project

## License

TL;DR: Use the code however you want, give credit where it's due, no warranty of any kind is provided.
