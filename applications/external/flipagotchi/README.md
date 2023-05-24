# Pwnagotchi-Flipper interface
This program will interface the pwnagotchi with the flipper. This will be accomplished using custom code on the pwnagotchi's end to give the flipper simple rendering instructions over UART.

<img src='doc/attachments/PwnZeroBaseWFace.png' alt='Flipper screen showing base Pwnagotchi display' height="128" width="256"/>

## Layout
This project is broken down into two parts: flipagotchi and pwnzero
```
pwnagotchi-flipper
 |--> flipagotchi/
 |--> pwnzero/
```
- flipagotchi is the Flipper-side application
- pwnzero is the pwnagotchi-side application

## Current state
FUNCTIONALITY IS CURRENTLY IN DEVELOPMENT: IT IS INCOMPLETE!
As of commit 7e9a9dc (1/21/23) The non-pixel-by-pixel version of the Flipper app now can receive and properly process all protocol v2 commands. The plugin was built and it "works" at a basic level. There are many bugs that need to be fixed sometime in the future. Attached is a GIF of the app in action!

<img src="doc/attachments/PwnInAction.gif" width="256" height="160"/>

## Setup
### Flipagotchi Setup (Flipper side)
This procedure explains the setup of the Flipagotchi app on the Flipper Zero. This will involve the manual compile as the firmware is constantly being updated and releasing a FAP for each firmware change is currently not feasible.
1. Connect your Flipper to your computer
2. Clone the Flipper Zero firmware onto your machine
3. Place the ```flipagotchi/``` directory into the ```applications_user/```
4. Open a terminal and navigate to the root of the firmware
5. Execute the following command to compile the app and launch it on the Flipper:<br>
    ```./fbt launch_app APPSRC=applications_user/flipagotchi```
6. This will now compile and load the app onto your Flipper

### PwnZero Setup (Pwnagotchi side)
This procedure will explain how to configure the Pwnagotchi to use the PwnZero plugin to communicate with the Flipper.
1. Disable Bluetooth on the Pi by adding ```dt-overlay=disable-bt``` at the bottom of the ```/boot/config.txt``` file<br>
    -- This needs to be disabled so that the full UART is directed to ```/dev/serial0```
2. Place the PwnZero.py file somewhere on the Pi in either its own folder or a folder with other plugins
3. Edit ```/etc/pwnagotchi/config.toml``` file and set ```main.custom_plugins = "/path/to/plugin/folder"```
4. Follow hardware setup shown in the doc/HardwareSetup.md to connect the devices
5. Restart the Pwnagotchi and open the Flipagotchi app on the Flipper Zero

## Development stages
### Stage 1: Simple display rendering
- Stage 1 will focus on getting the Pwnagotchi display to render on the Flipper's display

### Stage 2: App interaction
- Stage 2 will allow the user to interact and control the pwnagotchi using the Flipper's interface

## Contributing
If you would like to contribute, you may make a pull request. It will be helpful if you first open an issue describing the change that you are interested in contributing.

## License
[MIT](https://choosealicense.com/licenses/mit/)

## Disclaimer
<b>This program is meant for educational purposes ONLY. I disclaim any and all responsibility for the usage of this program by external parties (not me).</b>
