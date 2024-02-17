
# Flipper Zero F-Com

A Flipper Zero app that allows the flipper to communicate with Digimon V-Pets.

Currently tested:
- DM20
- DMX
- PenZ
- DMC unsupported

Untested:
- All other pets (Pen20, Original Pets, etc)
- Listen Mode

Based on:
- The DMComm project by BladeSabre: https://github.com/dmcomm/dmcomm-project
- The Flipper Zero Boilerplate App: https://github.com/leedave/flipper-zero-fap-boilerplate
- The Flipper Zero Firmware: https://github.com/flipperdevices/flipperzero-firmware

## Overview

You will need to construct a circuit similar to the A-Com circuit described in teh dmcomm-project github.

Pin C3 - 2k resistor - Pin A3 - 6k resistor - Pin GND

Pin A3 - VPet+

Pin GND - VPet-

## Menu options

### Listen

Listen is currently untested, as I have not created a jig to make it function!!

Listen allows you to connect 2 vpets to each other and eavesdrop on the data they send. Both codes will show up on the flipper after communication completes, and you can save either code to the flipper for later use. Leaving this screen will pause dmcomm.

### Saved

Saved will open a file picker, allowing you to choose a previously saved digirom code. After selection, the flipper will begin attempting to repeatedly send the code. After the code is sent, the received response will display on the screen, and you can optionally save the response to another file on the flipper. Leaving this screen will pause dmcomm.

### Add Manually

Add manually will open a dialog allowing you to enter a digimon manually on the flipper itself. The flipper keyboard does not currently support special characters so ^ and @ characters cannot yet be entered. After entering the code a second text entry screen will display allowing you to enter a file name. After saving, it will return to the main menu. If you wish to send the code you can enter 'Saved' and select it.

### USB A-Com

Tested with the Alpha-Wiki Android App.

The flipper will change the USB port from CLI mode into Serial mode and behave as an A-Com. Serial data transferred in both directions will appear in the flipper's text box on screen. Pressing the back button will return the USB port to CLI mode and pause dmcomm.

## Other Notes / Future stuff

Dmcomm supports a voltage test. However, the flipper zero firmware does not yet have ADC support implemented. This is still possible as demonstrated by the flipper zero oscilloscope project. At some point I may implement this. A-Com's themselves use analog input in order to support a wider range of devices logic levels. This may limit the flipper app's compatibility for now.

DMC support isn't included in the dmcomm-project ino, but has been added to the updated https://github.com/dmcomm/dmcomm-arduino-lib project. At some point I may update this to port the CPP code onto the flipper.

Debug mode support could be added at some point.
