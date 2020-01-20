# Serial flash example

## Overview

Example demonstrates how to flash ESP32 from another (host) MCU using esp_serial_flash component API. In this case, ESP32 is also used as host MCU. Binary to be flashed from one ESP32 to another is stored in partition named `spiffs`.

This example is based on spiffsgen example located in IDF. For more information how to use SPIFFS Image Generator, refer to [spiffsgen example](https://github.com/espressif/esp-idf/tree/master/examples/storage/spiffsgen)

Following steps are performed in order to re-program target's memory:

1. UART1 through which new binary will be transfered is initialized.
2. Filesystem is initialized and mounted.
3. Binary file is opened and its size is acquired, as it has to be known before flashing.
4. Host puts slave device into boot mode tries to connect by calling `loader_connect()`.
5. Then `loader_flash_start()` is called to enter flashing mode.
6. `loader_flash_write()` function is called repeatedly until the whole binary image is transfered.

## Hardware Required

* Two development boards with ESP32 SoC (e.g., ESP32-DevKitC, ESP-WROVER-KIT, etc.).
* A USB cable for power supply and programming.

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
