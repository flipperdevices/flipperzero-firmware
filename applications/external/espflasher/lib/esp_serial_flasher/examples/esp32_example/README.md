# Flash multiple partitions example

## Overview

Example demonstrates how to flash ESP32/ESP32-S2/ESP8266 from another (host) MCU using esp_serial_flash component API. In this case, ESP32 is also used as host MCU. Binaries to be flashed from host MCU to another Espressif SoC can be found in `binaries` folder and are converted into C-array during build process.

Following steps are performed in order to re-program target's memory:

1. Filesystem is initialized and mounted.
2. UART1 through which new binary will be transfered is initialized.
3. Host puts slave device into boot mode tries to connect by calling `esp_loader_connect()`.
4. Binary file is opened and its size is acquired, as it has to be known before flashing.
5. Then `esp_loader_flash_start()` is called to enter flashing mode and erase amount of memory to be flashed.
6. `esp_loader_flash_write()` function is called repeatedly until the whole binary image is transfered.

Note: In addition, to steps mentioned above, `esp_loader_change_transmission_rate`  is called after connection is established in order to increase flashing speed. This does not apply for ESP8266, as its bootloader does not support this command. However, ESP8266 is capable of detecting baud rate during connection phase, and can be changed before calling `esp_loader_connect`, if necessary.

## SPI pin initialization

In majority of cases `ESP_LOADER_CONNECT_DEFAULT` helper macro is used in order to initialize `loader_connect_args_t` data structure passed to `esp_loader_connect`. Helper macro sets `spi_pin_config` field of the data structure to zero, thus, default SPI pins are used to connect to FLASH memory. In special cases, such as custom design in which FLASH is connected to different pins, `spi_pin_config` field has to be set accordingly. For more detailed information refer to [serial protocol](https://docs.espressif.com/projects/esptool/en/latest/esp32s3/advanced-topics/serial-protocol.html).

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

## Build and flash

To run the example, type the following command:

```CMake
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Configuration

For details about available configuration option, please refer to top level [README.md](../../README.md). 
Compile definitions can be specified on command line when running `idf.py`, for example:

```
idf.py build -DMD5_ENABLED=1
```
Binaries to be flashed are placed in separate folder (binaries.c) for each possible target and converted to C-array. Without explicitly enabling MD5 check, flash integrity verification is disabled by default.

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
