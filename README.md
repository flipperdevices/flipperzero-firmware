# flipper-enigma

[![Build](https://github.com/xtruan/flipper-enigma/actions/workflows/build.yml/badge.svg?branch=main)](https://github.com/xtruan/flipper-enigma/actions/workflows/build.yml)

`HOW ABOUT A NICE GAME OF ENIGMA?`

![FLIPR](https://github.com/xtruan/flipper-enigma/blob/main/icons/FLIPR_128x64.png)

## Enigma game for Flipper Zero
- Built against `0.101.2` Flipper Zero firmware release
- Uses [smallenigmalib](https://codeberg.org/drummyfish/smallenigmalib)

### Installation

- Download [last release fap file](https://github.com/xtruan/flipper-enigma/releases/latest)
- Copy fap file to the apps folder of your Flipper SD card

### Usage

- Start "Enigma" plugin

### Build

- Recursively clone your base firmware (official or not)
- Clone this repository in `applications_user`
- Build with `./fbt fap_dist APPSRC=applications_user/flipper-enigma`
- Retrieve build fap in dist subfolders

(More info about build tool [here](https://github.com/flipperdevices/flipperzero-firmware/blob/dev/documentation/fbt.md))

### 


