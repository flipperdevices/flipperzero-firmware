# Gravity for Flipper Zero
## The Unseen Force

This Flipper Zero application (FAP) is a companion application to ESP32-Gravity.

ESP32-Gravity is a wireless exploration framework written for the ESP32. It
supports a variety of wireless exploration, offensive and defensive features.

ESP32-Gravity can be used without a Flipper, but Flipper-Gravity can't be used
without ESP32-Gravity. You can download it from the [esp32-gravity GitHub repo](https://github.com/chris-bc/esp32-gravity).

## Download Flipper-Gravity

### 1. From Source

1. Clone the RogueMaster repository using a command such as
    * You may prefer to `checkout` - or even download - a release version of the code. You do you :)
```c
gh repo clone RogueMaster/flipperzero-firmware-wPlugins
```
2. Change to the RogueMaster directory: `cd flipperzero-firmware-wPlugins`
3. Install the Flipper compilation toolchain: `./fbt`
4. Configure your environment variables: `` . `./fbt -s env` `` (note the leading 'dot space')
5. Change to the applications_user directory: `cd applications_user`
6. Clone this repository using a command such as 
```c
gh repo clone chris-bc/Flipper-Gravity
```
7. Change back to the firmware root directory: `cd ..`

Unless you've already flashed the firmware being used you'll need to upgrade the firmware on your Flipper so that it uses the same version as the FAP.

To build and flash the complete firmware bundle make sure your
Flipper Zero is on the home screen (no applications or menus open),
connect it to your computer, and run
```c
./fbt firmware_all flash_usb_full
```

However, if you want to install just the application, you have two main options:
1. `./fbt launch_app APPSRC=esp32_gravity` will build, install and start the application;
2. `./fbt flash_usb fap_esp32_gravity` will build the application and package it into a firmware update package;
3. Copy the update directory, which will be reported towards the end of the above command and will be similar to `dist/f7-C/f7-update-RM420FAP`, to the Flipper Zero's SD card;
4. Open Browser on the Flipper by pressing `Down`, then `Left` repeatedly until you reach a screen labelled **Browser**;
5. Navigate to the update directory you copied;
6. Push `OK` on `update.fuf`, then select *Run in App*;
7. Once the firmware update completes Gravity will be installed under `Apps/GPIO/ESP32/[ESP32] Gravity`


### 2. Binaries

Simply download this `FAP` (Flipper Application Package) and copy it to your
preferred applications folder on your Flipper Zero's SD card.

Now you just need to flash and connect an ESP32 and you'll be ready to go!

[![FAP Factory](https://flipc.org/api/v1/cool4uma/UART_Terminal/badge?firmware=unleashed)](https://flipc.org/chris-bc/Flipper-Gravity?firmware=roguemaster)


## Reduced Support for ESP32-C6

Good news for all the Flipper tinkerers who have a bucket of ESP32s but
no ESP32-C6s - Espressif's development environment, even trying the
bleeding edge from GitHub, doesn't yet have full Bluetooth support
for the ESP32-C6.

Gravity now targets the regular ESP32. I'm using a Wroom during development,
and hope to verify it working on ESP32-Cam cam at some point, but I think
this makes ESP32 the only Espressif microcontroller with full-stack Bluetooth
support. For now, at least.

For more information, including a thorough overview of all features,
check out the [readme doc over at esp-32-gravity](https://github.com/chris-bc/esp32-gravity/blob/main/README.md)


## Version Compatibility

Since Gravity has been included with RogueMaster Firmware I thought I should
make a couple of changes to make it easy for people other than me to know
which esp32-Gravity is compatible with which Flipper-Gravity.

* From here on out versions will be numbered `(major).(minor).(release)`;
* For example `1.2.1`;
* Different **release** versions (such as `1.2.1` and `1.2.9`) will always be compatible with each other, although the addition or significant modification of some features may result in a small number of features not working;
* Changes in minor version, such as `1.2.1` and `1.3.1`, are unlikely to be compatible. A change in minor version represents a noteable change to the platform or a breaking change to the platform;
* Changes in major verson, such as `1.4.9` to `2.0.0`, represent substantial changes to the application and how it runs. Different major versions *will not* be compatible with each other.


## Gravity In Action

![Gravity Main Menu](https://github.com/chris-bc/flipper-gravity/blob/main/flip-grav-mainmenu.png)

![Gravity Mana Attack](https://github.com/chris-bc/flipper-gravity/blob/main/flip-grav-mana.jpg)

![Gravity About Page](https://github.com/chris-bc/flipper-gravity/blob/main/flip-grav-about.png)


## TODO
* Bug: Console scrolls to top mid-scroll when text is added to it
    * Replicate: Start scan, Run help and try to read it
* Add space to keyboard
* add channel, mode, count STA+AP on console header

## Menu Structure
* Beacon: target-ssids, APs, RickRoll, Random, Infinite
* Probe: Any, target-ssids, APs
* Sniff: On, Off
* target-ssids: add, remove, list
* scan: on, off
* hop: on, off, sequential, random, default, set
* view: sta, ap, sta+ap
* select: sta, ap
* selected: sta, ap, sta+ap
* clear: sta, ap
* get/set: <variables>
* deauth: off, frame STA, device STA, spoof STA, frame APs, device APs, spoof APs, frame broadcast, device broadcast, spoof broadcast
* mana: on, off, clear
* mana verbose: on, off
* mana loud: on, off
* DOS AP: on, off
* help: commands help info


Info: If possible, do not power your devices from 3V3 (pin 9) Flipper Zero. It does not support hot plugging.

## Keyboard
UART_terminal uses its own special keyboard for work, which has all the symbols necessary for working in the console.

To accommodate more characters on a small display, some characters are called up by holding.

![kbf](https://user-images.githubusercontent.com/122148894/212286637-7063f1ee-c6ff-46b9-8dc5-79a5f367fab1.png)

## How to install
Copy the contents of the repository to the applications_user/uart_terminal folder Flipper Zero firmware and build app with the command ./fbt fap_uart_terminal.

Or use the tool [uFBT](https://github.com/flipperdevices/flipperzero-ufbt) for building applications for Flipper Zero.

Download ready [fap](https://github.com/playmean/fap-list)

## How it works

