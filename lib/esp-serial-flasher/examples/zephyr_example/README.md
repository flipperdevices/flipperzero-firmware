# ESP32 Zephyr example

## Overview

This sample code demonstrates how to flash ESP32/ESP32-S2/ESP8266 from another (host) MCU using
esp_serial_flash component API. In this case, ESP32 is also used as host MCU.
Binaries to be flashed from host MCU to another Espressif SoC can be found in `binaries` folder
and are converted into C-array during build process.

Following steps are performed in order to re-program target's memory:

1. Peripherals are initialized (GPIO for BOOT and EN pins and UART).
2. UART1 (can be changed) through which new binary will be transfered is initialized.
3. Host puts slave device into boot mode tries to connect by calling `esp_loader_connect()`.
4. Binary file is opened and its size is acquired, as it has to be known before flashing.
5. Then `esp_loader_flash_start()` is called to enter flashing mode and erase amount of memory to be flashed.
6. `esp_loader_flash_write()` function is called repeatedly until the whole binary image is transfered.

Note: In addition, to steps mentioned above, `esp_loader_change_transmission_rate`  is called after connection
is established in order to increase flashing speed. This does not apply for ESP8266, as its bootloader
does not support this command. However, ESP8266 is capable of detecting baud rate during connection
phase, and can be changed before calling `esp_loader_connect`, if necessary.

## Hardware Required

* Two development boards with ESP32 SoC (e.g., ESP32-DevKitC, ESP-WROVER-KIT, etc.).
* One or two USB cables for power supply and programming.

## Hardware connection

Table below shows connection between two ESP32 devices.

| ESP32 (host) | ESP32 (slave) |
|:------------:|:-------------:|
|    IO4       |      IO0      |
|    IO2       |     RESET     |
|    IO9       |      TX0      |
|    IO10      |      RX0      |

Note: interconnection is the same for all three targets (slaves).

## Build and flash

To run the example, type the following command:

```c
west build -p -b esp32
west flash
west espressif monitor
```

(To exit the serial monitor, type ``ctrl-c``.)

For more information, check [Zephyr's Getting Started](https://docs.zephyrproject.org/latest/develop/getting_started/index.html)

## Configuration

For details about available configuration option, please refer to top level [README.md](../../README.md).
Compile definitions can be specified in `prj.conf` file.

Binaries to be flashed are placed in separate folder (binaries.c) for each possible target and converted to C-array. Without explicitly enabling MD5 check, flash integrity verification is disabled by default.

## Example output

Here is the example's console output:

```
*** Booting Zephyr OS build zephyr-v3.2.0-3548-ga1bb9c9d1736 ***
Running ESP Flasher from Zephyr
Connected to target
Baudrate changed
Erasing flash (this may take a while)...
Start programming
Progress: 100 %
Finished programming
Erasing flash (this may take a while)...
Start programming
Progress: 100 %
Finished programming
Erasing flash (this may take a while)...
Start programming
Progress: 100 %
Finished programming
```
