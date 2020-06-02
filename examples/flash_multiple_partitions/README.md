# Flash multiple partitions example

## Overview

Example demonstrates how to flash ESP32/ESP32-S2/ESP8266 from another (host) MCU using esp_serial_flash component API. In this case, ESP32 is also used as host MCU. Binary to be flashed from host MCU to ESP* is stored in partition named `spiffs`. Binaries are placed in separate folder for each target.

This example is based on spiffsgen example located in IDF. For more information how to use SPIFFS Image Generator, refer to [spiffsgen example](https://github.com/espressif/esp-idf/tree/master/examples/storage/spiffsgen)

Following steps are performed in order to re-program target's memory:

1. Filesystem is initialized and mounted.
2. UART1 through which new binary will be transfered is initialized.
3. Host puts slave device into boot mode tries to connect by calling `esp_loader_connect()`.
4. Binary file is opened and its size is acquired, as it has to be known before flashing.
5. Then `esp_loader_flash_start()` is called to enter flashing mode and erase amount of memory to be flashed.
6. `esp_loader_flash_write()` function is called repeatedly until the whole binary image is transfered.

Note: In addition, to steps mentioned above, `esp_loader_change_baudrate`  is called after connection is established in order to increase flashing speed. This does not apply for ESP8266, as its bootloader does not support this command. However, ESP8266 is capable of detecting baud rate during connection phase, and can be changed before calling `esp_loader_connect`, if necessary.

## Target selection

User can configure targets and `loader_config_user.h` header.
Currently, three targets (ESP32, ESP32-S2, ESP8266) are supported.
By default, example is compiled for ESP32 target.

## Hardware Required

* Two development boards with ESP32 SoC (e.g., ESP32-DevKitC, ESP-WROVER-KIT, etc.).
* One or two USB cables for power supply and programming.

## Hardware connection

Table below shows connection between two ESP32 devices.

| ESP32 (host) | ESP32 (slave) |
|:------------:|:-------------:|
|    IO26      |      IO0      |
|    IO25      |     RESET     |
|    IO4       |      RX0      |
|    IO5       |      TX0      |

Note: interconnection is the same for all three targets (slaves). 

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
