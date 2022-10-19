# RC2014 ColecoVision Controller for Flipper Zero

A Flipper Zero application and RC2014 module allowing the Flipper to be used as a controller for ColecoVision games on the RC2014.

## Building the FAP

1. Clone the [flipperzero-firmware] repository.
2. Create a symbolic link in `applications_user` named `coleco`, pointing to this repository.
3. Compile with `./fbt fap_coleco`. This will create the `coleco.fap` binary.

[flipperzero-firmware]: https://github.com/flipperdevices/flipperzero-firmware
