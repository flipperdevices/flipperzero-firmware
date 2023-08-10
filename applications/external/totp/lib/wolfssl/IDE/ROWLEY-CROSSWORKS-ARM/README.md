# Rowley CrossWorks ARM Project for wolfSSL and wolfCrypt

This directory contains a CrossWorks solution named wolfssl.hzp.

Inside are three projects:

1. libwolfssl: 
This generates a library file named "libwolfssl_ARM_Debug/libwolfssl_v7em_t_le_eabi.a"
2. benchmark: 
This is a sample benchmark application. It runs the "benchmark_test" suite repeatedly until a failure occurs.
3. test: 
This is a sample test application. It runs "wolfcrypt_test" suite suite repeatedly until a failure occurs.

# Prerequisites

+You will need to install the "Freescale Kinetis CPU Support Package" and "ARM CPU Support Package" in the Rowley Package Manager under Tools -> Package Manager.

# Hardware Support

All hardware functions are defined in `kinetis_hw.c` and are currently setup for a Freescale Kinetis K64 Coretx-M4 microcontroller. This file can be customized to work with other Kinetis microcontrollers by editing the top part of the file. Testing for this project was done with the Freescale Kinetis `MK64FN1M0xxx12` using the `TWR-K64F120M`.  To build for the `TWR-K64F120M` or `FRDM-K64F`, define `WOLFSSL_FRDM_K64` in the Preprocessor Definitions section of CrossStudio, or define it in "user_settings.h".

To create support for a new ARM microcontroller the functions in `hw.h` will need to be implemented.

Also you will need to configure the ARM Architecture and ARM Core Type in the "Solution Properties" -> "ARM". 
Also the "Target Processor" in each of the projects ("Project Properties" -> "Target Processor")

## Hardware Crypto Acceleration

To enable NXP/Freescale MMCAU:

1. [Download the MMCAU library](http://www.freescale.com/products/arm-processors/kinetis-cortex-m/k-series/k7x-glcd-mcus/crypto-acceleration-unit-cau-and-mmcau-software-library:CAUAP).
2. Copy the `lib_mmcau.a` and `cau_api.h` files into the project.
3. Define `USE_NXP_MMCAU` to enable in `user_settings.h`.
4. Add the `lib_mmcau.a` file to `Source Files` in the application project.
5. Open the wolfssl_ltc.hzp CrossWorks project
6. Build and run

To enable the NXP/Freescale MMCAU and/or LTC:

1. [Download the NXP KSDK 2.0](https://nxp.flexnetoperations.com/control/frse/download?agree=Accept&element=7353807)
2. Copy the following folders into IDE/ROWLEY-CROSSWORKS-ARM: drivers, mmcau_2.0.0 and CMSIS.
3. Copy the following files into IDE/ROWLEY-CROSSWORKS-ARM: clock_config.c, clock_config.h, fsl_debug_console.c, fsl_debug_console.h, fsl_device_registers.h, system_MK82F25615.c, system_MK82F25615.h, MK82F25615.h and MK82F25615_features.h.
4. Define `USE_NXP_LTX` to enable in `user_settings.h`.
5. Open the wolfssl_ltc.hzp CrossWorks project
6. Build and run

# Project Files

* `arm_startup.c`: Handles startup from `reset_handler`. Disabled watchdog, initializes sections, initializes heap, starts hardware and starts main.
* `benchmark_main.c`: The main function entrypoint for benchmark application.
* `hw.h`: The hardware API interface. These hardware interface functions are required for all platforms.
* `kinetis_hw.c`: The most basic hardware implementation required for Kinetis.
* `test_main.c`: The main function entrypoint for test application.
* `user_libc.c`: Defines stubs for functions required by libc. It also wraps hardware functions for UART, RTC and Random Number Generator (RNG).
* `user_settings.h`: This is the custom user configuration file for WolfSSL.

# Functions required by the WolfSSL Library

If you are writing your own application, the following functions need to be implemented to support the WolfSSL library:

* `double current_time(int reset)`: Returns a double as seconds.milliseconds.
* `int custom_rand_generate(void)`: Returns a 32-bit randomly generated number.
