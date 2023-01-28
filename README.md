# GPS for Flipper Zero

A simple Flipper Zero application for NMEA 0183 serial GPS modules, such as the
[Adafruit Ultimate GPS Breakout].

![ui](ui.png)

Heavy lifting (NMEA parsing) provided by [minmea], which is included in this
repository.

## Hardware Setup

Connect the GPS module to power and the USART using GPIO pins 9 (3.3V), 11
(GND), 13 (TX), and 14 (RX), as appropriate.

![wiring](wiring.png)

See the [tutorial video](https://www.youtube.com/watch?v=5vSGFzEBp-k) from
Lab401 by [RocketGod](https://github.com/RocketGod-git) for a visual guide to
the hardware setup.

### Confirmed Compatible Modules

* [Adafruit Ultimate GPS Breakout]
* Beitian BN-180
* [u-blox NEO-6M]

If you have verified this application working with a module not listed here,
please submit a PR adding it to the list.

## Building the FAP

1. Clone the [flipperzero-firmware] repository.
2. Create a symbolic link in `applications_user` named `gps`, pointing to this
   repository.
3. Compile with `./fbt fap_gps`.
4. Copy `build/f7-firmware-D/.extapps/gps.fap` to `apps/Tools` on the SD card
   (directly or using [qFlipper]).

## Contributing

This project was a learning exercise and is more or less "complete" from my
perspective, but I will happily accept pull requests that improve and enhance
the functionality for others.

Currently, the app only parses RMC and GGA sentences, and displays a subset of
the data that fits on the screen. The UART is also hard-coded to 9600 baud.
These limitations are largely driven by the GPS module I have to work with. A
more elaborate UI with scrolling or multiple screens, as well as a configurable
baud rate, may be useful for other GPS modules.

[Adafruit Ultimate GPS Breakout]: https://www.adafruit.com/product/746
[minmea]: https://github.com/kosma/minmea
[flipperzero-firmware]: https://github.com/flipperdevices/flipperzero-firmware
[qFlipper]: https://flipperzero.one/update
[u-blox NEO-6M]: https://www.u-blox.com/en/product/neo-6-series
