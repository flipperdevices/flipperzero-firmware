# flipper-chess

[![Build](https://github.com/xtruan/flipper-chess/actions/workflows/build.yml/badge.svg?branch=main)](https://github.com/xtruan/flipper-chess/actions/workflows/build.yml)

`HOW ABOUT A NICE GAME OF CHESS?`

![FLIPR](https://github.com/xtruan/flipper-chess/blob/main/icons/FLIPR_128x64.png)

## Chess game for Flipper Zero
- Built against `0.86.1` Flipper Zero firmware release
- Uses [smallchesslib](https://codeberg.org/drummyfish/smallchesslib)
- Included in [RogueMaster Custom Firmware](https://github.com/RogueMaster/flipperzero-firmware-wPlugins)

### Installation

- Download [last release fap file](https://github.com/xtruan/flipper-chess/releases/latest)
- Copy fap file to the apps folder of your Flipper SD card

### Usage

- Start "Chess" plugin

### Build

- Recursively clone your base firmware (official or not)
- Clone this repository in `applications_user`
- Build with `./fbt fap_dist APPSRC=applications_user/flipper-chess`
- Retrieve build fap in dist subfolders

(More info about build tool [here](https://github.com/flipperdevices/flipperzero-firmware/blob/dev/documentation/fbt.md))

### 


