# esp32-gameboy

This is a port of https://github.com/zid/gameboy to the Espressif ESP32 chip. and this project based on Arduino,and the SPI LCD the low speed.There's a better repo named Espeon(https://github.com/Ryuzaki-MrL/Espeon) used for M5STACK.

# What do I need to use this?

You will need:
* A board containg an ESP32 chip and at least 4MB (32Mbit) of SPI flash, plus the tools to program it.
* A backup of a GameBoy ROM game cartridge
* A 320x240 ILI9341 display, controllable by a 4-wire SPI interface. You can find modules with this LCD by
looking for '2.2 inch SPI 320 240 ILI9341' on eBay or other shopping sites.
* A GPIO button

# How do I hoop up my board?

**LCD:**

    =====  =======================
    Pin    GPIO
    =====  =======================
    MISO   25
    MOSI   23
    CLK    19
    CS     22
    DC     21
    RST    18
    BCKL   5
    =====  =======================

(BCKL = backlight enable)

(Make sure to also wire up the backlight and power pins.)

# How do I program the chip?

rename GB ROM to gb.gb

run rom2h.bat

run gameboy.ino

update!
