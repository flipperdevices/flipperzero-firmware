# Serial flasher

## Overview

Serial flasher component provides portable library for flashing `ESP32`, `ESP32-S2`, `ESP8266` from other host microcontroller. Espressif SoCs are normally programmed via serial interface (UART) and port layer for given host microcontroller has to be implemented, if not available. Details can be found in section below.


## Supporting new host target

In order to support new target, following function has to be implemented by user:

* loader_port_serial_read()
* loader_port_serial_write()
* loader_port_enter_bootloader()
* loader_port_delay_ms()
* loader_port_start_timer()
* loader_port_remaining_time()

Following functions are part of serial_io.h header for convenience, however, user does not have to strictly follow function signatures, as there are not called directly from library. 

* loader_port_serial_init()
* loader_port_serial_deinit()
* loader_port_change_baudrate()
* loader_port_reset_target()
* loader_port_debug_print()

Prototypes of all function mentioned above can be found in [serial_io.h](include/serial_io.h).
Please refer to ports in `port` directory. Currently, only [ESP32 port](port/esp32_uart.c) is available.

## Component integration

Apart from writing port (if not available), user has to provide `loader_config_user.h` header file and add its include path into build system. By defining appropriate macros, default configuration can be overwritten. As for now, slave target to be flashed (one of Espressif SoCs) and flash integrity verification can be configured. In case header file remains empty, `esp-serial-flasher` is compiled for ESP32 target by default.

## Known limitations

At this point, component can only be integrated with IDF. Proper CMakeList.txt file is to be done.   

Size of new binary image has to be known before flashing.