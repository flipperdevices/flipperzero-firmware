# I2C DEMO
## Introduction
This is a basic demonstration of reading/writing I2C protocol.
For this demo, we connect a I2C device to pins:
 - 3V3 (3V3, pin 9)  = VCC
 - GND (GND, pin 18) = GND
 - SCL (C0, pin 16)  = SCL
 - SDA (C1, pin 15)  = SDA


## Installation Directions
This project is intended to be overlayed on top of an existing firmware repo.
- Clone, Build & Deploy an existing flipper zero firmware repo.  See this [tutorial](/firmware/updating/README.md) for updating firmware.
- Copy the "i2c_demo" [folder](..) to the \applications\plugins\i2c_demo folder in your firmware.
- Build &amp; deploy the firmware.  See this [tutorial](/firmware/updating/README.md) for updating firmware.
- NOTE: You can also extract the i2c_demo.FAP from resources.tar file and use qFlipper to copy the file to the SD Card/apps/Gpio folder.


## Running the updated firmware
These directions assume you are starting at the flipper desktop.  If not, please press the back button until you are at the desktop.

- Press the OK button on the flipper to pull up the main menu.
- Choose "Applications" from the menu.
- Choose "GPIO" from the sub-menu.
- Choose "I2C Demo"

- The flipper should say "I2C NOT FOUND" if no I2C devices are connected.

- NOTE: If your I2C device already has pull-up resistors, then you do not need to add them to your SCL and SDA lines.

- Connect an I2C device (like a BH1750) to pins 9 (3V3), 16 (SCL), 15 (SDA), 18 (GND).
- The message should change to "FOUND I2C DEVICE"
- The next line should have the address of the I2C device.
- If the device is a BH1750 then you should also see "WRITE/READ SUCCESS" and a value that changes with the brightness on the sensor.

- Press the BACK button to exit.


## How it works
- application.fam
  - specifies the name of our application.
  - specifies the entry point for our application.
  - specifies we use the GUI.
  - specifies our icon is the i2c_demo.png file.
  - specifies our application can be found in the "GPIO" category.

- i2c_demo.png
  - The icon for our application that shows up in the "GPIO" folder.

- i2c_demo_app.c
  - This is the demo application that uses I2C.