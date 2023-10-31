# AVR ISP Programmer

The AVR ISP Programmer application allows you to do various things with AVR microcontrollers using the ISP interface. It can autonomously Dump and Flash the microcontroller using files on the SD card. It can also act as an ISP programmer for programming from a desktop IDE via USB. It is compatible with AVR-based microcontrollers, such as the ATmega328P used in the Arduino Uno, ATTiny85, and many others.

## Wiring

The AVR ISP Programmer includes a wiring pinout diagram in the app. You can also find it here:

FLIPPER PIN | AVR PIN
------------|--------
1           | 5V (optional)
2           | MOSI
3           | MISO
4           | CLOCK (optional)
5           | SCK
6           | RESET
7           | -
8           | GND
9           | 3.3V
10          | -
11          | GND
12-18       | -

# Features

## Dump

The Dump feature allows you to dump the contents of the microcontroller's flash memory to a file on your Flipper Zero's SD card. Along with the EEPROM memory, the dump files include the fuses and lock bits of the microcontroller.

This can be used to backup the contents of the microcontroller's flash memory, or to extract the firmware from a device.

## Flash

The Flash feature allows you to flash the contents of a file on your Flipper Zero's SD card to the microcontroller's flash memory. It also flashes the fuses and lock bits of the microcontroller.

This can help you quickly flash multiple devices with the same firmware, or to restore a device to its original firmware if it has been bricked.

## ISP

The ISP feature allows you to use your Flipper Zero as an ISP programmer for your desktop computer. After connecting your Flipper Zero to the AVR microcontroller, you can flash it via USB from your desktop computer using avrdude.

Example command: "avrdude.exe -p m328p -c stk500v1 -P COMxx -U flash:r:X:\\sketch_sample.hex:i"

Where:

* -p m328p is the brand of your chip,
* -P COMxx is the com port number of the ISP programmer

## Supported Microcontrollers

* ATtinyXXXX
* ATmegaXXXX
* AT43Uxxx
* AT76C711
* AT86RF401
* AT90xxxxx
* AT94K
* ATAxxxxx
* ATA664251
* M3000
* LGT8F88P
* LGT8F168P
* LGT8F328P

Along with any other AVR microcontroller that is supported by avrdude.
