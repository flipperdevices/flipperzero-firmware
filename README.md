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
TODO: Explain setup process
NOTE: Interface is still being developed, when I have a working version I will explain the setup procedure.

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
