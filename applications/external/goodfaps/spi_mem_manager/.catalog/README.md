# SPI Memory Manager

This application allows you to read, erase, and flash SPI memory chips. It can be used to backup the contents of SPI memory chips, or to restore them to their original state.

## Wiring

The SPI Memory Manager includes a wiring pinout diagram in the app. You can also find it here:

| FLIPPER PIN | SPI PIN |
|-------------|---------|
| 1           | -       |
| 2           | MOSI    |
| 3           | MISO    |
| 4           | CS      |
| 5           | CLK     |
| 6           | -       |
| 7           | -       |
| 8           | GND     |
| 9           | 3.3V    |
| 10          | -       |
| 11          | GND     |
| 12-17       | -       |
| 18          | GND     |

# Features

## Read

To read the contents of an SPI memory chip, connect it to your Flipper Zero and press the Read button. The chip type will be detected automatically, and, if it is supported, the contents of the chip will be read and saved to a file on your Flipper Zero's SD card.

## Erase

To erase the contents of an SPI memory chip, connect it to your Flipper Zero and press the Erase button. If the chip type is supported, the chip will be erased.

## Flash

To flash the contents of a file on your Flipper Zero's SD card to an SPI memory chip, connect it to your Flipper Zero, select the Saved menu option in the app, and select the file you want to flash. If the chip type is supported, the file will be flashed to the chip.
