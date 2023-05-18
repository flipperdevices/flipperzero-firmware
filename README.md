# Serial flasher

## Overview

Serial flasher component provides portable library for flashing or loading ram loadble app to Espressif SoCs from other host microcontroller. Espressif SoCs are normally programmed via serial interface (UART). Port layer for given host microcontroller has to be implemented, if not available. Details can be found in section below.

Supported **host** microcontrollers:

- STM32
- Raspberry Pi
- ESP32
- MCU running Zephyr OS

Supported **target** microcontrollers:

- ESP32
- ESP8266
- ESP32-S2
- ESP32-S3
- ESP32-C3
- ESP32-C2
- ESP32-H2

## Supporting new host target

In order to support new target, following function has to be implemented by user:

- loader_port_read()
- loader_port_write()
- loader_port_enter_bootloader()
- loader_port_delay_ms()
- loader_port_start_timer()
- loader_port_remaining_time()

Following functions are part of io.h header for convenience, however, user does not have to strictly follow function signatures, as there are not called directly from library.

- loader_port_change_transmission_rate()
- loader_port_reset_target()
- loader_port_debug_print()

Prototypes of all function mentioned above can be found in [io.h](include/io.h).
Please refer to ports in `port` directory. Currently, ports for [ESP32](port/esp32_port.c), [STM32](port/stm32_port.c), and [Zephyr](port/zephyr_port.c) are available.

## Configuration

These are the configuration toggles available to the user:
* MD5_ENABLED

If enabled, serial flasher is capable of verifying flash integrity after writing to memory.

Default: Enabled
> Warning: As ROM bootloader of ESP8266 does not support MD5_CHECK, this option has to be disabled!

* SERIAL_FLASHER_RESET_HOLD_TIME_MS

This is the time for which the reset pin is asserted when doing a hard reset in milliseconds.

Default: 100

* SERIAL_FLASHER_BOOT_HOLD_TIME_MS

This is the time for which the boot pin is asserted when doing a hard reset in milliseconds.

Default: 50

Configuration can be passed to `cmake` via command line:

```
cmake -DMD5_ENABLED=1 .. && cmake --build .
```

### STM32 support

STM32 port make use of STM32 HAL libraries, that does not come with CMake support. In order to compile the project, `stm32-cmake` (`cmake` support package) has to be pulled as submodule.

```
git clone --recursive https://github.com/espressif/esp-serial-flasher.git
```

If you have cloned this repository without the `--recursive` flag, you can initialize the submodule using the following command:

```
git submodule update --init
```

In addition to configuration parameters mentioned above, following definitions has to be set:

- TOOLCHAIN_PREFIX: path to arm toolchain (i.e /home/user/gcc-arm-none-eabi-9-2019-q4-major)
- STM32Cube_DIR: path to STM32 Cube libraries (i.e /home/user/STM32Cube/Repository/STM32Cube_FW_F4_V1.25.0)
- STM32_CHIP: name of STM32 for which project should be compiled (i.e STM32F407VG)
- PORT: STM32

This can be achieved by passing definition to command line, such as:

```
cmake -DTOOLCHAIN_PREFIX="/path_to_toolchain" -DSTM32Cube_DIR="path_to_stm32Cube" -DSTM32_CHIP="STM32F407VG" -DPORT="STM32" .. && cmake --build .
```

Alternatively, set those variables in top level `cmake` directory:

```
set(TOOLCHAIN_PREFIX    path_to_toolchain)
set(STM32Cube_DIR       path_to_stm32_HAL)
set(STM32_CHIP          STM32F407VG)
set(PORT                STM32)
```

### Zephyr support

The Zephyr port is ready to be integrated into your Zephyr app as a Zephyr module. In the manifest file (west.yml), add:

```
    - name: esp-flasher
      url: https://github.com/espressif/esp-serial-flasher
      revision: master
      path: modules/lib/esp_flasher
```

And add

```
CONFIG_ESP_SERIAL_FLASHER=y
CONFIG_CONSOLE_GETCHAR=y
CONFIG_SERIAL_FLASHER_MD5_ENABLED=y
```

to your project configuration `prj.conf`.

For your C/C++ source code, you can use the example code provided in `examples/zephyr_example` as a starting point.

## Licence

Code is distributed under Apache 2.0 license.

## Known limitations

Size of new binary image has to be known before flashing.
