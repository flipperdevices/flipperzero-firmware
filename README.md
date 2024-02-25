# FlipperApp-TuLlave

This is a project to add the capability of reading TuLlave cards to the Flipper Zero device. TuLlave is a contactless smart card used to pay for public transport services in Bogotá, Colombia. It operates on the MIFARE Classic protocol, which is supported by the Flipper Zero NFC module.

## Features

1. Read TuLlave card balance from your Flipper Zero. 
2. Read TuLlave transaction history.

## Installation.

1. Download the latest FAP binary from Releases.
2. Power-Off your Flipper Zero and extract the SD Card. Then, put the FAP file into the apps/NFC directory.
3. Insert your SD Card again in the Flipper Zero. You can Find TuLlave into: Apps > NFC > TuLlave, on the main menu.

## Build

### Requirements

* Flipper Zero device.
* [Flipper Zero Firmware](https://github.com/flipperdevices/flipperzero-firmware) in order to compile this project.
* Firmware version. It is compatible with versions greater than or equal to 0.98.2.
* Visual Studio Code.
* ST-Link V2 for debugging purposes. Highly recommended.

### Steps to build

* Clone the [Flipper Zero Firmware](https://github.com/flipperdevices/flipperzero-firmware).
* Go to the folder flipperzero-firmware/applications_user.
* Clone the FlipperApp-TuLlave in that folder. 
* In the Flipper Zero Firmware root path, run the below command:

```bash
./fbt vscode_dist
```
* Compile the Flipper Zero Firmware in Debug mode, then flash your Flipper device.
* First, ensure your Flipper Device is properly connected through USB or Serial.
* Open the vscode and go to the flipperzero-firmware folder. In the user_applications search: "tullave.c" file. Open the file. then press Ctrl+Shift+B and run "Launch App on Flipper" task. 

### Debugging

This method utilizes the ST-Link V2 USB, which is considered cheap compared to other gadgets. Also, it is the only method that I have used to debug the Flipper :3

Drew has created a great article which explains know how to debug the Flipper Zero Firmware using the GPIO and ST-Link V2 device. Visit the below link for more details. 

https://www.drewgreen.net/debugging-flipper-zero-firmware-with-st-link-v2/

After required connections are set you can execute the task "Attach FW (ST-Link)" which was generated from the ./ftb vscode_dist command.

In order to reach the breakpoints set in the FlipperApp-TuLlave you will need to manually navigate in the Flipper device to: applications -> NFC -> TuLlave.

#### Known Issues when Debugging

I have to say that debugging Flipper Zero with ST-Link V2 is a little tricky. Sometimes I found myself restarting the Flipper Zero often because of unexpected exceptions. Here are some exceptions tha I found during my development.

1. Error: Unable to set adapter speed. This error may indicate the ST-Link V2 driver was not installed properly or the firmware is not compatible with the OS host. First download the latest version for the ST-Link V2 firmware. [Download Firmware here](https://www.st.com/en/development-tools/stsw-link007.html). Then install the latest version of the driver. [Download driver here](https://www.st.com/en/development-tools/stsw-link009.html).

2. Error: init mode failed (unable to connect to the target). This error may occur due to privileges when accessing the ST-Link V2 device. To solve it, just execute the vscode as administrator and try to execute the debug task again. Usually, running the vscode task as admin is required only the first time, you can omit this step for further debugging processes. Also, ensure the Debug mode is enabled in Settings -> System -> Debug on the Flipper Zero. 

2. Flipper Zero freezes. You should restart your Flipper by pressing down Power Button + Left Arrow maintain the keys pressed by a few seconds. 

## Understanding NFC Architecture

See the below video to know how the NFC Architecture is approached by the Flipper Zero.

https://www.youtube.com/watch?v=cTCp4IVq9g0

## TODO

* Recover the UI Reading state to inform user that do not move the card during Transaction History Reading.
