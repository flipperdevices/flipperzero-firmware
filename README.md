# FlipperApp-TuLlave

This repository contains a simple personal project to add the capability of reading TuLlave Smartcards. This uses the NFC modules for Flipper firmware.

## Build

### Requirements

* Flipper Zero device.
* [Flipper Zero Firmware](https://github.com/flipperdevices/flipperzero-firmware) in order to compile this project.
* Visual Studio Code.

### Steps to build

* Clone the [Flipper Zero Firmware](https://github.com/flipperdevices/flipperzero-firmware).
* Go to the folder flipperzero-firmware/applications_user.
* Clone this repository in that folder. 
* In the Flipper Zero Firmware root path, run the below command:

```bash
./fbt vscode_dist
```
* Open the vscode application and locate in the file: "tullave_co.c", then press Ctrl+Shift+B and run "Launch App on Flipper" task. Ensure first your Flipper Device is properly connected through USB or Serial.

## TODO

* Read Blance and Card Info from TuLlave (**Done**).
* Implement TuLlave_Analyzer to read the Transaction History.

## Bugs

* Limit readings only for TuLlave. Other NFC-A Tags are read showing garbage in the main window.