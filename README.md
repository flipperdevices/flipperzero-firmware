# Apple BLE Spam for OFW

![](https://thumb.tildacdn.com/tild3332-3839-4061-b663-363464303432/-/resize/214x/-/format/webp/noroot.png)

## What

This Flipper application ("FAP") spams broadcast packets to Apple devices, which may be up to 5 meters away.

More information: https://techryptic.github.io/2023/09/01/Annoying-Apple-Fans/

## Builds

OFW: App Hub PR pending: https://github.com/flipperdevices/flipper-application-catalog/pull/149

Published (options):
* [FlipC.org](https://flipc.org/noproto/apple_ble_spam_ofw?branch=master)
  * Download the FAP at the above link then copy the FAP to its respective apps/ directory (Bluetooth) on your Flipper Zero device (using qFlipper or manually copying it to the SD)
* [RogueMaster firmware](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/releases/latest)
  * Built in to every release, you're good to go!

## Credit

[Original app by WillyJL](https://github.com/Flipper-XFW/Xtreme-Firmware/tree/dev/applications/external/apple_ble_spam)

Extensive testing and research on behavior and parameters by WillyJL and ECTO-1A

Structures docs and Nearby Action IDs from https://github.com/furiousMAC/continuity/

Proximity Pair IDs from https://github.com/ECTO-1A/AppleJuice/

Airtag ID from https://techryptic.github.io/2023/09/01/Annoying-Apple-Fans/
