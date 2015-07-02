# About
This project is used to develop applications for stm32 - ST's ARM Cortex-M3(4) MCUs, using cmake, GCC, newlib (libc), STM32CubeMX package or ChibiOS.

Requirements:
* cmake >= 2.8
* GCC toolchain with newlib (optionally).
* STM32CubeMX package for STM32F1 or STM32F4 families.
Project contains:
* CMake common toolchain file, that configures cmake to use arm toolchain.
* CMake family-specific toolchain file, that configures family-specific parameters.
* CMake modules to find and configure CMSIS ans STM32HAL components.
* CMake modules to find and configure ChibiOS components.
* CMake project template.
* Example projects:
  * stm32-blinky - blink LED using timers and PWM.
  * stm32-newlib - show date using uart and libc functions from newlib
  * stm32-chibios - blink led using ChibiOS/NIL

# Usage
First of all you need to configure toolchain and libraries, you can do this by editing gcc_stm32.cmake or (better way) pass it throught command line.
Variables for toolchain:
* TOOLCHAIN_PREFIX - where toolchain is located, **default**: /usr
* TARGET_TRIPLET - toolchain target triplet, **default**: arm-none-eabi
* STM32_CHIP - STM32 device code, e.g. STM32F407VG or STM32F103VG
* STM32_FAMILY - STM32 family (F0, F1, F4, etc.) currently, F1 and F4 family are supported. **Note:** If STM32_CHIP variable is set, STM32_FAMILY is optional.
* STM32Cube_DIR - path to STM32CubeMX directory **default**: /opt/STM32Cube_FW_F1_V1.1.0 /opt/STM32Cube_FW_F4_V1.6.0

For using scripts you'll need to copy contents of cmake folder into cmake's modules path, or use CMAKE_MODULE_PATH variable. 
Template project can be found in stm32-template folder.

## Configure
Common usage:

 ```cmake -DSTM32_CHIP=<chip> -DCMAKE_TOOLCHAIN_FILE=<path_to_gcc_stm32.cmake> -DCMAKE_BUILD_TYPE=Debug <path_to_source_dir>```

Where <chip> - stm32 chip name (e.g. STM32F100C8, STM32F407IG). 
This command will generate Makefile for project.
Scripts will try to detected chip parameters (type, flash/ram size) from chip name. 
You can set this parameters directly using following cmake variables:
* STM32_CHIP_TYPE - family-depended chip type. Global variable STM32_CHIP_TYPES contains list of valid types for current family
* STM32_FLASH_SIZE - chip flash size (e.g. 64K)
* STM32_RAM_SIZE - chip RAM size (e.g. 4K)

For using with Eclipse CDT:

 ```cmake -DSTM32_CHIP=<chip> -DCMAKE_TOOLCHAIN_FILE=<path_to_gcc_stm32.cmake> -DCMAKE_BUILD_TYPE=Debug -G "Eclipse CDT4 - Unix Makefiles" <path_to_source_dir>```

For release build:

 ```cmake -DSTM32_CHIP=<chip> -DCMAKE_TOOLCHAIN_FILE=<path_to_gcc_stm32.cmake> -DCMAKE_BUILD_TYPE=Release <path_to_source_dir>```

## Build

 ```make```
 
To build .hex:

 ```make <project name>.hex```
 
or .bin:

 ```make <project name>.bin```

## Linker script variables
Next cmake variables are useful for linker tuning:
* STM32_LINKER_SCRIPT - Path to custom linker script. You can use cmake variables (listed below) in itd.
* STM32_FLASH_ORIGIN - Start address of flash (**default**: 0x08000000)
* STM32_RAM_ORIGIN - Start address of RAM (**default**: 0x20000000)
* STM32_CCRAM_ORIGIN - Start address of Core-Coupled RAM (only for F4 family) (**default**: 0x10000000)
* STM32_FLASH_SIZE - Flash size (**default**: from chip name)
* STM32_RAM_SIZE - RAM size (**default**: from chip name)
* STM32_CCRAM_SIZE - Core-Coupled RAM size (only for F4 family) (**default**: 64 KiB)
* STM32_MIN_STACK_SIZE - Minimum stack size for error detection at link-time (**default**: 512 bytes)
* STM32_MIN_HEAP_SIZE - Minimum heap size for error detection at link-time (**default**: 0 bytes)

## Useful cmake macros
* STM32_GET_CHIP_TYPE(CHIP CHIP_TYPE) - gets chip type from chip name.
* STM32_GET_CHIP_PARAMETERS(CHIP FLASH_SIZE RAM_SIZE) - gets chip ram/flash size from chip name.
* STM32_SET_CHIP_DEFINITIONS(TARGET CHIP_TYPE) - sets chip family and type-specific compiler flags for target.
* STM32_SET_FLASH_PARAMS(TARGET ...) - sets chip flash/ram parameters for targer.
* STM32_SET_TARGET_PROPERTIES(TARGET) - sets all needed parameters and compiler flags for target. 
* STM32_GENERATE_LIBRARIES(NAME SOURCES LIBRARIES) - generates libraries for all chip types in family. Resulting libraries stored in LIBRARIES and have names in ${NAME}_${FAMILY}_${CHIP_TYPE} format.

## ChibiOS Support
This projects also supports ChibiOS v3.x.x (both nil and rt kernels). 
CMake modules for ChibiOS can find specified ChibiOS components using COMPONENTS directive. 
See project stm32-chibios for example usage. 