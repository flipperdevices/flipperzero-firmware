# Flipperzero-SD-SPI
Flipper Zero FAP for Lock and Unlock SD card / Micro SD card through SPI protocol (CMD42).

<p align="center">
<img src="SDSPI.gif" />
</p>

## Pinout ##

Without Flipper Zero SDBoard the SD card it must be connected as in the table below

Flipper Zero  | SD Card
------------- | -------------
9/3.3V  | +3.3V
8/GND  | GND
2/A7  | Mosi
3/A6  | Miso
4/A4  | CS
5/B3  | SCK

<p align="center">
<img src="scheme.png" />
</p>

## Usage ##

Whenever an sd card is connected it is required make a "Init", if the operation is successul in the "status" tab R1 is "NO ERROR" and it is possible execute other commands.

"Lock" and "Unlock" work with password set in namesake tab.

Force Erase allow the removal of unknown password from SD but erases all content.

After the first save, the password is stored in apps_data/sdspi/pwd.txt. you can change it to use characters not found on the flipper keyboard.
