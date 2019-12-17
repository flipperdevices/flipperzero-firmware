# Serial flasher

## Overview

Serial flasher component provides portable library for flashing ESP32 from other host microcontroller. ESP32 is normally programmed via serial interface (UART) and port layer for given host microcontroller has to be implemented, if not available. Details can be found in section bolew.


## Supporting new target

In order to support new target, following function has to be implemented by user:

* serial_read()
* serial_write()
* enter_bootloader()
* delay_miliseconds()

Following functions are part of serial_io.h header for convenience, however, user does not have to strictly follow function signatures, as there are not called directly from library. 

* serial_init()
* serial_deinit()
* reset_target()
* debug_print()

Prototypes of all function mentioned above can be found in [serial_io.h](include/serial_io.h).
Please refer to ports in `port` directory. Currently, only [ESP32 port](port/esp32_uart.c) is available.

## Component integration

At this point, this component can only be integrated with IDF. Improvement of existing CMakeList.txt file is to be done.   

## Known limitations

Size of new binary image has to be known before flashing.