# Example of loading the program into RAM through SPI

## Overview

This example demonstrates how to upload an app to RAM of an Espressif MCU with SPI download support from another (host) MCU using the `esp_serial_flash` component API. In this case, another Espressif MCU is used as the host. Binaries to be uploaded to RAM from host MCU to the target MCU can be found in `binaries/RAM_APP` folder and are converted into C-array during build process.

Following steps are performed in order to re-program the target's memory:

1. SPI2 through which the binary will be transfered is initialized.
2. Host puts slave device into SPI download mode tries to connect by calling `esp_loader_connect()`.
3. Then `esp_loader_mem_start()` is called for each segment in RAM.
4. `esp_loader_flash_write()` function is called repeatedly for every segment until the whole binary image is transfered.
5. `esp_loader_mem_finish()` is called with the binary entrypoint, telling the chip to start the uploaded program.
6. UART2 is initialized for the connection to the target
7. Target output is continually read and printed

## Hardware Required

* Two development boards, one with any Espressif MCU (e.g., ESP32-DevKitC, ESP-WROVER-KIT, etc.) and one with an Espressif MCU with SPI download support. Here is a short list of supported MCUs:
1. ESP32-C3
2. ESP32-C2
3. ESP32-S3
4. ESP32-S2
5. ESP32-H2
* One or two USB cables for power supply and programming.

## Hardware connection

Table below shows connection between two Espressif MCUs.

| Host         | Slave         |
|    IO_5      |    RESET      |
|    IO_12     |    CLK        |
|    IO_10     |    CS         |
|    IO_13     |    MISO       |
|    IO_11     |    MOSI       |
|    IO_14     |    QUADWP     |
|    IO_9      |    QUADHD     |
|    IO_13     |    STRAP_B0   |
|    IO_2      |    STRAP_B1   |
|    IO_3      |    STRAP_B2   |
|    IO_4      |    STRAP_B3   |
|    IO_6      |    UART0_RX   |
|    IO_7      |    UART0_TX   |

> Note 1: Strapping bit pins are documented in the TRM for each respective chip

> Note 2: For achieving highest speeds, check which pins go through the IO MUX bypassing the GPIO matrix and use those. Pins chosen here are IO MUX pins for ESP32S3 and ESP32S2 chips.

## Build and flash

To run the example, type the following command:

```CMake
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example output

Here is the example's console output:

```
Connected to target
I (682) spi_ram_loader: Loading app to RAM ...
Start loading
Downloading 7840 bytes at 0x3fc96e00...
Downloading 312 bytes at 0x3fca0020...
Downloading 93164 bytes at 0x40380000...
Finished loading
I (802) spi_ram_loader: ********************************************
I (802) spi_ram_loader: *** Logs below are print from slave .... ***
I (812) spi_ram_loader: ********************************************
Hello world!
Hello world!
...
```