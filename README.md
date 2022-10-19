# Pwnagotchi-Flipper interface
This program will interface the pwnagotchi with the flipper. This will be accomplished using custom code on the pwnagotchi's end to give the flipper simple rendering instructions over UART.

## Layout
This project is broken down into two parts: pwn_zero and TBD
```
pwnagotchi-flipper
 |--> pwn_zero/
 |--> TBD/
```
- pwn_zero is the Flipper-side application
- TBD is the pwnagotchi-side application

## Current state
FUNCTIONALITY IS CURRENTLY IN DEVELOPMENT: IT IS INCOMPLETE!
As of commit 9e5f6b7 (10/18/22) on rxBuffer branch the Pwnagotchi app for the flipper has a test function in the main function. This basically updates the screen when it receives a message. At the moment this causes the pwnagotchi to dereference a null pointer when it receives data. At least this means data is being received!

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
