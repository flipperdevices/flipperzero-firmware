# STM32 example

## Overview

Example demonstrates how to flash ESP32 from another STM32 (host MCU) using esp_serial_flash component API. WeActStudio [MiniSTM32H7xx](https://github.com/WeActStudio/MiniSTM32H7xx) board is used in this example, as STM32H743VIT has FLASH memory large enough to fit all the example binaries to be flashed from host MCU to ESP32 which can be found in `binaries` directory and converted into C-arrays during build process. USART1 is dedicated for communication with ESP32, whereas, USART2 can be used for debug purposes by attaching UART-to-USB bridge.

Following steps are performed in order to re-program target's memory:

1. Peripherals are initialized.
2. Host puts slave device into boot mode tries to connect by calling `esp_loader_connect()`.
3. Then `esp_loader_flash_start()` is called to enter flashing mode and erase amount of memory to be flashed.
4. `esp_loader_flash_write()` function is called repeatedly until the whole binary image is transfered.

Note: In addition, to steps mentioned above, `esp_loader_change_transmission_rate`  is called after connection is established in order to increase flashing speed. Bootloader is also capable of detecting baud rate during connection phase, and can be changed before calling `esp_loader_connect`. However, it is recommended to start at lower speed and then use dedicated command to increase baud rate. This does not apply for ESP8266, as its bootloader does not support this command, therefore, baud rate can only be changed before connection phase in this case.

## Hardware Required

* WeActStudio [MiniSTM32H7xx](https://github.com/WeActStudio/MiniSTM32H7xx) board with the STM32H743VIT chip
* A development board with ESP32 SoC (e.g. ESP-WROVER-KIT, ESP32-DevKitC, etc.).
* One or two USB cables for power supply and programming.

## Hardware connection

Table below shows connection between STM32 and ESP32.

| STM32 (host) | ESP32 (slave) |
|:------------:|:-------------:|
|    PB12      |      IO0      |
|    PB13      |      RST      |
|    PB14      |      RX0      |
|    PB15      |      TX0      |

Optionally, UART-to-USB bridge can be connected to PA3(RX) and PA2(TX) for debug purposes.

## Build and flash

To compile the example:

Create and navigate to `build` directory:
```
mkdir build && cd build
```
Run cmake (with appropriate parameters) and build: 
```
cmake -DSTM32_TOOLCHAIN_PATH="path_to_toolchain" -DSTM32_CUBE_H7_PATH="path_to_cube_libraries" .. && cmake --build .
```

Binaries to be flashed are placed in separate folder (binaries.c) for each possible target and converted to C-array. Flash integrity verification is enabled by default.

For more details regarding to esp_serial_flasher configuration and STM32 support, please refer to top level [README.md](../../README.md).

> Note: CMake 3.16 or later is required.
