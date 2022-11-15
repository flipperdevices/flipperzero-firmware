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
As of commit f2df88c (11/14/22) I have a pretty much fully working pixel-by-pixel drawing interface. I have decided though that this is not the best approach. Instead I should have specific commands for drawing the face, number of access points, name, and so on. For this reason I will archive this current code in a branch called pixel-by-pixel and then continue development of a new protocol that will require less information transmission over UART.

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
