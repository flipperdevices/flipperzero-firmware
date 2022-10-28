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
<<<<<<< HEAD
As of commit 23858cc (10/28/22) on rxBuffer branch the I have begun structuring a message queue which will ultimately leverage built in furi streams to process uart input. After restructuring and getting the receive to work I will merge into main and begin working on getting the screen to color as per the protocol's transmission.
=======
Currently I am working on getting the rxBuffer system to work. Right now that just involves experimenting until I find the best solution. Going forward will be referencing the uart_echo tool included with debug in the firmware. Hopefully will have a buffering version soon and then from there just processing the protocol to draw on the screen.
>>>>>>> 7f88c85c33412c7424c05dfa0eae445f3c5e9acb

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
