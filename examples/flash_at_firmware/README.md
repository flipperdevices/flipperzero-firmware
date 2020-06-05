# Flash AT firmware example

## Overview

Example demonstrates how to flash AT command firmware from one ESP32 to another using esp_serial_flash component API. Binary to be flashed has to be stored on SD card with `fat` filesystem. AT command firmware to copy onto SD card can be found in `image` folder. When other AT command firmware is stored on SD card, file name has to be changed in example accordingly.

Following steps are performed in order to re-program target's memory:

1. Filesystem is initialized and mounted.
2. UART1 through which new binary will be transfered is initialized.
3. Host puts slave device into boot mode tries to connect by calling `esp_loader_connect()`.
4. Binary file is opened and its size is acquired, as it has to be known before flashing.
5. Then `esp_loader_flash_start()` is called to enter flashing mode and erase amount of memory to be flashed.
6. `esp_loader_flash_write()` function is called repeatedly until the whole binary image is transfered.

## Hardware Required

* `ESP_WROVER_KIT` (with SD card connector)
* Another board based on WROWER module.
* One or two USB cables for power supply and programming.
* microSD card

Note: WROWER module(ESP32) to be flashed can be substituted with any other Espressif SoC as long as appropriate AT firmware version is used.

## Hardware connection

Table below shows connection between two ESP32 devices.

| ESP32 (host) | ESP32 (slave) |
|:------------:|:-------------:|
|    IO26      |      IO0      |
|    IO25      |     RESET     |
|    IO4       |      RX0      |
|    IO5       |      TX0      |

### Build and flash

To run the example, type the following command:

```CMake
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Configuration

For details about available configuration option, please refer to top level [README.md](../../README.md). Compile definitions can be specified on command line when running `idf.py`, for example:

```
idf.py build -DTARGET_SOC=ESP32_S2 -DMD5_ENABLED=1
```
By default, example is compiled for ESP32 target with MD5 check disabled.
When target is not ESP32, provide appropriate AT command firmware binary.

For available AT command firmwares refer to [AT firmwares](https://www.espressif.com/en/support/download/at)

## Example output

Here is the example's console output:

```
...
I (342) example: Initializing SPIFFS
I (482) example: Image size: 144672
I (902) example: Connected to target
I (1732) example: Start programming
I (1832) example: packet: 0  written: 1024 B
I (1932) example: packet: 1  written: 1024 B
...
I (16052) example: packet: 140  written: 1024 B
I (16152) example: packet: 141  written: 288 B
I (16152) example: Finished programming
```
