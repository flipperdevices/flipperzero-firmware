# BASIC DEMO
## Introduction
This project is to learn how to build a custom component (an up-down knob) for a scene for the Flipper Zero.

TODO: WRITE TUTORIAL ON CREATING THE KNOB COMPONENT.

## Installation Directions
This project is intended to be overlayed on top of an existing firmware repo.
- Clone, Build & Deploy an existing flipper zero firmware repo.  See this [tutorial](/firmware/updating/README.md) for updating firmware.
- Copy the "knob_component" [folder](..) to the \applications\plugins\knob_component folder in your firmware.
- Build &amp; deploy the firmware.  See this [tutorial](/firmware/updating/README.md) for updating firmware.
- NOTE: You can also extract the knob_component.FAP from resources.tar file and use qFlipper to copy the file to the SD Card/apps/Misc folder.


## Running the updated firmware
These directions assume you are starting at the flipper desktop.  If not, please press the back button until you are at the desktop.

- Press the OK button on the flipper to pull up the main menu.
- Choose "Applications" from the menu.
- Choose "Misc" from the sub-menu.
- Choose "Knob Demo"

- The flipper should say "VOLUME" and "Knob demo 50".
- Press UP/DOWN buttons to change value.
- Press OK button to switch to "FREQUENCY" and "Knob demo 440".
- Press UP/DOWN buttons to change value.
- Press OK button to go to "VOLUME".
- Press BACK button for last page.
- Keep pressing BACK button until exit.


## How it works
TODO: WRITE TUTORIAL ON CREATING THE KNOB COMPONENT.