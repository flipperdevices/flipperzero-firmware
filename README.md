# Gravity Companion for Flipper Zero

This Flipper Zero application (FAP) is a companion application to ESP32-Gravity.

ESP32-Gravity is a wireless exploration framework written for the ESP32. It
supports a variety of wireless exploration, offensive and defensive features.

ESP32-Gravity can be used without a Flipper, but Flipper-Gravity can't be used
without ESP32-Gravity. You can download it from:
https://github.com/chris-bc/esp32-gravity

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

## TODO
* Bug: Console scrolls to top mid-scroll when text is added to it
    * Replicate: Start scan, Run help and try to read it
* Add space to keyboard
* add channel, mode, count STA+AP on console header

## Menu Structure
* Beacon: target-ssids APs RickRoll Random Infinite
* Probe: Any target-ssids APs
* Sniff: On Off
* target-ssids: add remove list
* scan: on off
* hop: on off default
* view: sta ap sta+ap
* select: sta ap
* selected: sta ap sta+ap
* clear: sta ap
* get/set: <variables>
* deauth: off, frame STA, device STA, spoof STA, frame APs, device APs, spoof APs, frame broadcast, device broadcast, spoof broadcast
* mana: on off clear
* mana verbose: on off
* mana loud: on off
* help: commands help

# UART Terminal for Flipper Zero
[Flipper Zero](https://flipperzero.one/) app to control various devices via UART interface.
## Download fap
| **FW Official** | **FW Unleashed** |
| - | - |
| [![FAP Factory](https://flipc.org/api/v1/cool4uma/UART_Terminal/badge)](https://github.com/chris-bc/Flipper-Gravity) | [![FAP Factory](https://flipc.org/api/v1/cool4uma/UART_Terminal/badge?firmware=unleashed)](https://github.com/chris-bc/Flipper-Gravity) |


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


![1f](https://user-images.githubusercontent.com/122148894/211161450-6d177638-3bfa-42a8-9c73-0cf3af5e5ca7.jpg)


![2f](https://user-images.githubusercontent.com/122148894/211161456-4d2be15b-4a05-4450-a62e-edcaab3772fd.jpg)

