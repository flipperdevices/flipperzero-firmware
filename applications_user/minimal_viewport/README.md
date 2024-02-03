# BASIC DEMO
## Introduction
This is a minimal Hello World application for the Flipper Zero using the Viewport.  The goal of this project is to use it as a starting point for other applications.


## Installation Directions
This project is intended to be overlayed on top of an existing firmware repo.
- Clone, Build & Deploy an existing flipper zero firmware repo.  See this [tutorial](/firmware/updating/README.md) for updating firmware.
- Copy the "minimal_viewport" [folder](..) to the \applications\plugins\minimal_viewport folder in your firmware.
- Build &amp; deploy the firmware.  See this [tutorial](/firmware/updating/README.md) for updating firmware.
- NOTE: You can also extract the basic_demo.FAP from resources.tar file and use qFlipper to copy the file to the SD Card/apps/Misc folder.


## Running the updated firmware
These directions assume you are starting at the flipper desktop.  If not, please press the back button until you are at the desktop.

- Press the OK button on the flipper to pull up the main menu.
- Choose "Applications" from the menu.
- Choose "Misc" from the sub-menu.
- Choose "Minimal Viewport Demo"

- The flipper should say "Hello World!"

- Press the BACK button to exit.


## How it works
- application.fam
  - specifies the name of our application.
  - specifies the entry point for our application.
  - specifies we use the GUI.
  - specifies our icon is the minimal_viewport.png file.
  - specifies our application can be found in the "Misc" category.

- minimal_viewport.png
  - The icon for our application.

- minimal_viewport_app.c
  - We #include the libraries we referece.
    