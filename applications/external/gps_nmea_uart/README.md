# GPS for Flipper Zero

A simple Flipper Zero application for NMEA 0183 serial GPS modules, such as the
- Adafruit Ultimate GPS Breakout.

Heavy lifting (NMEA parsing) provided by minmea.

## Usage

This is a single-screen app, and a few interactions are provided via the
hardware buttons:

- Long press the up button to change the **baud rate**. The default baud rate
  is 9600, but 19200, 38400, 57600, and 115200 baud are also supported.
- Long press the right button to change **speed units** from knots to
  kilometers per hour.
- Press the OK button to set the **backlight** to always on mode. Press it
  again to disable.
- Long press the back button to **exit** the app.

## Hardware Setup

Connect the GPS module to power and the USART using GPIO pins 9 (3.3V), 11
(GND), 13 (TX), and 14 (RX), as appropriate.


See the tutorial video - https://www.youtube.com/watch?v=5vSGFzEBp-k from
Lab401 by RocketGod - https://github.com/RocketGod-git for a visual guide to
the hardware setup.

### Confirmed Compatible Modules

* Adafruit Ultimate GPS Breakout
* Beitian BN-180
* Royaltek RBT-2100LP
* u-blox NEO-6M

If you have verified this application working with a module not listed here,
please submit a PR adding it to the list.

## Links

Original repo link - https://github.com/ezod/flipperzero-gps
Adafruit Ultimate GPS Breakout: https://www.adafruit.com/product/746
minmea: https://github.com/kosma/minmea
u-blox NEO-6M: https://www.u-blox.com/en/product/neo-6-series
