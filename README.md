# GPS for Flipper Zero

[![FAP Build](https://github.com/ezod/flipperzero-gps/actions/workflows/build.yml/badge.svg)](https://github.com/ezod/flipperzero-gps/actions/workflows/build.yml)

A simple Flipper Zero application for NMEA 0183 serial GPS modules.

![ui](ui.png)

Heavy lifting (NMEA parsing) provided by [minmea].

## Installation

1. Navigate to the [FAP Build](https://github.com/ezod/flipperzero-gps/actions/workflows/build.yml)
   GitHub action workflow, and select the most recent run.
2. The FAP is built for both the `dev` and `release` channels of the official
   firmware. Download the artifact corresponding to your firmware version.
3. Extract `gps_nmea.fap` from the ZIP file to `apps/GPIO` on your Flipper
   Zero SD card.

## Usage

This is a single-screen app, and a few interactions are provided via the
hardware buttons:

- Long press the up button to change the **baud rate**. The default baud rate
  is 9600, but 4800, 19200, 38400, 57600, and 115200 baud are also supported.
- Long press the right button to change **speed units** from knots to
  kilometers per hour.
- Press the OK button to set the **backlight** to always on mode. Press it
  again to disable.
- Long press the back button to **exit** the app.

## Hardware Setup

Connect the GPS module to power and the USART using GPIO pins 9 (3.3V), 11
(GND), 13 (TX), and 14 (RX), as appropriate.

![wiring](wiring.png)

See the [tutorial video](https://www.youtube.com/watch?v=5vSGFzEBp-k) from
Lab401 by [RocketGod](https://github.com/RocketGod-git) for a visual guide to
the hardware setup.

### Confirmed Compatible Modules

* [Adafruit Ultimate GPS Breakout]
* ATGM336H
* Beitian BN-180
* Beitian BN-220
* Beitian BN-280
* Beitian BN-880
* Beitian BK-250
* Beitian BK-357
* Beitian BK-880Q
* Beitian BE-280
* Beitian BN-280ZF
* Beitian BN-357ZF
* Royaltek RBT-2100LP
* [u-blox NEO-6M]
* [u-blox NEO-7M]
* [Uputronics u-blox MAX-M8C Pico]

If you have verified this application working with a module not listed here,
please submit a PR adding it to the list.

## Building

This application can be compiled using [uFBT]. Run `ufbt` in the root directory
of the repository.

[Adafruit Ultimate GPS Breakout]: https://www.adafruit.com/product/746
[minmea]: https://github.com/kosma/minmea
[qFlipper]: https://flipperzero.one/update
[u-blox NEO-6M]: https://www.u-blox.com/en/product/neo-6-series
[u-blox NEO-7M]: https://www.u-blox.com/en/product/neo-7-series
[uFBT]: https://github.com/flipperdevices/flipperzero-ufbt
[Uputronics u-blox MAX-M8C Pico]: https://store.uputronics.com/index.php?route=product/product&product_id=72
