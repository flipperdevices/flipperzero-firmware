# About

![Tests](https://github.com/ObKo/stm32-cmake/workflows/Tests/badge.svg)

This project is used to develop applications for the STM32 - ST's ARM Cortex-Mx MCUs.
It uses cmake and GCC, along with newlib (libc), STM32Cube. Supports C0 F0 F1 F2 F3 F4 F7 G0 G4 H7 L0 L1 L4 L5 U0 U5 WB WL device families.

## Requirements

* cmake >= 3.16
* GCC toolchain with newlib (optional).
* STM32Cube package for appropriate STM32 family.

## Project contains

* CMake toolchain file, that configures cmake to use the arm toolchain: [cmake/stm32_gcc.cmake](cmake/stm32_gcc.cmake).
* CMake module that contains useful functions: [cmake/stm32/common.cmake](cmake/stm32/common.cmake)
* CMake modules that contains information about each family - RAM/flash sizes, CPU types, device types and device naming (e.g. it can tell that STM32F407VG is F4 family with 1MB flash, 128KB RAM with CMSIS type F407xx)
* CMake toolchain file that can generate a tunable linker script [cmake/stm32/linker_ld.cmake](cmake/stm32/linker_ld.cmake)
* CMake module to find and configure CMSIS library [cmake/FindCMSIS.cmake](cmake/FindCMSIS.cmake)
* CMake module to find and configure STM32 HAL library [cmake/FindHAL.cmake](cmake/FindHAL.cmake)
* CMake modules for various libraries/RTOSes
* CMake project template and [examples](examples)
* Some testing project to check cmake scripts working properly [tests](tests)

## Examples

* `template` ([examples/template](examples/template)) - project template, empty source linked compiled with CMSIS.
* `custom-linker-script` ([examples/custom-linker-script](examples/custom-linker-script)) - similar to `template` but using custom linker script.
* `fetch-cube` ([examples/fetch-cube](examples/fetch-cube)) - example of using FetchContent for fetching STM32Cube from ST's git.
* `fetch-cmsis-hal` ([examples/fetch-cmsis-hal](examples/fetch-cmsis-hal)) - example of using FetchContent for fetching STM32 CMSIS and HAL from ST's git.
* `blinky` ([examples/blinky](examples/blinky)) - blink led using STM32 HAL library and SysTick.
   It will compile a project for the `F4` family by default, but you can also compile for the
   `L0` and `F1` family by passing `BLINKY_L0_EXAMPLE=ON` or `BLINKY_F1_EXAMPLE=ON` to the CMake
   generation call. Using C++ instead of C is possible using `USE_CPP_FILE=ON`.
* `freertos` ([examples/freertos](examples/freertos)) - blink led using STM32 HAL library and FreeRTOS.
   You need to specify at least one board by passing `FREERTOS_<BOARD>_EXAMPLE=ON` to CMake.
   Currently, the example can be built for the `H743ZI` and `F407VG` board targets.
   You can opt to use the FreeRTOS CMSIS implementation provided by the Cube repository by supplying
   `USE_CMSIS_RTOS=ON` or `USE_CMSIS_RTOS_V2` to CMake.

# Usage

First of all you need to configure toolchain and library paths using CMake variables. There are
generally three ways to do this:

1. Pass the variables through command line during cmake run with passed to CMake with
   `-D<VAR_NAME>=...`
2. Set the variables inside your `CMakeLists.txt`
3. Pass these variables to CMake by setting them as environmental variables.

The most important set of variables which needs to be set can be found in the following section.

## Configuration

These configuration options need to be set for the build process to work properly:

* `STM32_CUBE_<FAMILY>_PATH` - path to STM32Cube directory, where `<FAMILY>` is one
   of `C0 F0 F1 F2 F3 F4 F7 G0 G4 H7 L0 L1 L4 L5 U0 U5 WB WL` **default**: `/opt/STM32Cube<FAMILY>`

These configuration variables are optional:

* `STM32_TOOLCHAIN_PATH` - where toolchain is located, **default**: `/usr`. Alternatively
  you can add the folder containing the toolchain binaries to the system path. If both are given,
  the `STM32_TOOLCHAIN_PATH` setting takes precedence
* `STM32_TARGET_TRIPLET` - toolchain target triplet, **default**: `arm-none-eabi`
* `FREERTOS_PATH` - Path to the FreeRTOS kernel when compiling with a RTOS. Does not need to be
   specified when using CMSIS

### Helper script on Unix shells

If you have access to a Unix shell, which is the default terminal on Linux, or tools like
`MinGW64` or `git bash` on Windows, you can write a small `path_helper.sh` script like this:

```sh
export STM32_TOOLCHAIN_PATH="<ToolchainPath>"
export STM32_TARGET_TRIPLET=arm-none-eabi
export STM32_CUBE_<FAMILY>_PATH="<PathToCubeRoot>"
```

and then use `. path_helper.sh` to set up the environment for the local terminal instance in one go.

### Helper script in Powershell

On Windows, you can use a Powershell script `path_helper.ps1`to set up the environment:

```sh
$env:STM32_TOOLCHAIN_PATH = "<ToolchainPath>"
$env:STM32_TARGET_TRIPLET = arm-none-eabi
$env:STM32_CUBE_<FAMILY>_PATH="<PathToCubeRoot>"
```

## Common usage

First thing that you need to do after toolchain configuration in your `CMakeLists.txt` script is to find CMSIS package:
```cmake
find_package(CMSIS [CMSIS_version] COMPONENTS STM32F4 REQUIRED)
```
You can specify STM32 family or even specific device (`STM32F407VG`) in `COMPONENTS` or omit `COMPONENTS` totally - in that case stm32-cmake will find ALL sources for ALL families and ALL chips (you'll need ALL STM32Cube packages somewhere).

[CMSIS_version] is an optional version requirement. See [find_package documentation](https://cmake.org/cmake/help/v3.13/command/find_package.html?highlight=find%20package#id4). This parameter does not make sense if multiple STM32 families are requested.

Each STM32 device can be categorized into family and device type groups, for example STM32F407VG is device from `F4` family, with type `F407xx`.

***Note**: Some devices have two different cores (e.g. STM32H7 has Cortex-M7 and Cortex-M4).
For those devices the name used must include the core name e.g STM32H7_M7 and STM32H7_M4.
STM32WB is a multi-cores device even if the second core is not accessible by end user.

CMSIS consists of three main components:

* Family-specific headers, e.g. `stm32f4xx.h`
* Peripheral access layer header and source, e.g. `system_stm32f4xx.[c|h]`
* Device type-specific startup sources (e.g. `startup_stm32f407xx.s`) (if ASM language is enabled)
* Device-specific linker scripts which requires information about memory sizes (if ASM language is enabled)

stm32-cmake uses modern CMake features notably imported targets and target properties.
Every CMSIS component is CMake's target (aka library), which defines compiler definitions, compiler flags, include dirs, sources, etc. to build and propagate them as dependencies. So in a simple use-case all you need is to link your executable with library `CMSIS::STM32::<device>`:
```cmake
add_executable(stm32-template main.c)
target_link_libraries(stm32-template CMSIS::STM32::F407VG)
```
That will add include directories, peripheral layer files, startup source, linker script and compiler flags to your executable.

CMSIS creates the following targets:

* `CMSIS::STM32::<FAMILY>` (e.g. `CMSIS::STM32::F4`) - common includes, compiler flags and defines for family
* `CMSIS::STM32::<TYPE>` (e.g. `CMSIS::STM32::F407xx`) - common startup source for device type and peripheral access layer files, depends on `CMSIS::STM32::<FAMILY>`
* `CMSIS::STM32::<DEVICE>` (e.g. `CMSIS::STM32::F407VG`) - linker script for device, depends on `CMSIS::STM32::<TYPE>`

So, if you don't need linker script, you can link only `CMSIS::STM32::<TYPE>` library and provide your own script using `stm32_add_linker_script` function

***Note**: Because of some families multi-cores architecture, all targets also have a suffix (e.g. STM32H7 has ::M7 or ::M4).
For example, targets created for STM32H747BI will look like `CMSIS::STM32::H7::M7`,
`CMSIS::STM32::H7::M4`, `CMSIS::STM32::H747BI::M7`, `CMSIS::STM32::H747BI::M4`, etc.*

The GCC C/C++ standard libraries are added by linking the library `STM32::NoSys`. This will add
the `--specs=nosys.specs` to compiler and linker flags.
If you want to use C++ on MCUs with little flash, you might instead want to link the newlib-nano to
reduce the code size. You can do so by linking `STM32::Nano`, which will add the
`--specs=nano.specs` flags to both compiler and linker.
Keep in mind that when using `STM32::Nano`, by default you cannot use floats in printf/scanf calls,
and you have to provide implementations for several OS interfacing
functions (`_sbrk`, `_close`, `_fstat`, and others). You can enable printf/scanf floating point support with
newlib-nano by linking against `STM32::Nano::FloatPrint` and/or `STM32::Nano::FloatScan`.
It is also possible to combine `STM32::Nano` and `STM32::NoSys`
to have the benefits of reduced code size while not being forced to implement system calls.

## HAL

STM32 HAL can be used similar to CMSIS.
```cmake
find_package(HAL [HAL_version] COMPONENTS STM32F4 REQUIRED)
set(CMAKE_INCLUDE_CURRENT_DIR TRUE)
```
*`CMAKE_INCLUDE_CURRENT_DIR` here because HAL requires `stm32<family>xx_hal_conf.h` file being in include headers path.*

[HAL_version] is an optional version requirement. See [find_package documentation](https://cmake.org/cmake/help/v3.13/command/find_package.html?highlight=find%20package#id4). This parameter does not make sense if multiple STM32 families are requested.

HAL module will search all drivers supported by family and create the following targets:

* `HAL::STM32::<FAMILY>` (e.g. `HAL::STM32::F4`) - common HAL source, depends on `CMSIS::STM32::<FAMILY>`
* `HAL::STM32::<FAMILY>::<DRIVER>` (e.g. `HAL::STM32::F4::GPIO`) - HAL driver <DRIVER>, depends on `HAL::STM32::<FAMILY>`
* `HAL::STM32::<FAMILY>::<DRIVER>Ex` (e.g. `HAL::STM32::F4::ADCEx`) - HAL Extension driver , depends on `HAL::STM32::<FAMILY>::<DRIVER>`
* `HAL::STM32::<FAMILY>::LL_<DRIVER>` (e.g. `HAL::STM32::F4::LL_ADC`) - HAL LL (Low-Level) driver , depends on `HAL::STM32::<FAMILY>`

***Note**: Targets for multi-cores devices will look like `HAL::STM32::<FAMILY>::<CORE>`, `HAL::STM32::<FAMILY>::<CORE>::<DRIVER>`, etc.*

Here is typical usage:

```cmake
add_executable(stm32-blinky-f4 blinky.c stm32f4xx_hal_conf.h)
target_link_libraries(stm32-blinky-f4
    HAL::STM32::F4::RCC
    HAL::STM32::F4::GPIO
    HAL::STM32::F4::CORTEX
    CMSIS::STM32::F407VG
    STM32::NoSys
)
```

### Building

```
    $ cmake -DCMAKE_TOOLCHAIN_FILE=<path_to_gcc_stm32.cmake> -DCMAKE_BUILD_TYPE=Debug <path_to_sources>
    $ make
```

## Linker script & variables

CMSIS package will generate linker script for your device automatically (target `CMSIS::STM32::<DEVICE>`). To specify a custom linker script, use `stm32_add_linker_script` function.

## Useful CMake functions

* `stm32_get_chip_info(<chip> [FAMILY <family>] [TYPE <type>] [DEVICE <device>])` - classify device using name, will return device family (into `<family>` variable), type (`<type>`) and canonical name (`<device>`, uppercase without any package codes)
* `stm32_get_memory_info((CHIP <chip>)|(DEVICE <device> TYPE <type>) [FLASH|RAM|CCRAM|STACK|HEAP] [SIZE <size>] [ORIGIN <origin>])` - get information about device memories (into `<size>` and `<origin>`). Linker script generator uses values from this function
* `stm32_print_size_of_target(<target>)` - Print the application sizes for all formats
* `stm32_generate_binary_file(<target>)` - Generate the binary file for the given target
* `stm32_generate_srec_file(<target>)` - Generate the srec file for the given target
* `stm32_generate_hex_file(<target>)` - Generate the hex file for the given target


In the following functions, you can also specify mutiple families.

* `stm32_get_devices_by_family(STM_DEVICES [FAMILY families...])` - return into `STM_DEVICES` all
  supported devices by family (or all devices if `FAMILY` is omitted)
* `stm32_print_devices_by_family([FAMILY families...])` - Print all supported devices by family
  (or all devices if `FAMILY` is omitted)

# Additional CMake modules

stm32-cmake contains additional CMake modules for finding and configuring various libraries and RTOSes used in the embedded world.

## <a id="freertos"></a> FreeRTOS

[cmake/FindFreeRTOS](cmake/FindFreeRTOS.cmake) - finds FreeRTOS sources in location specified by
`FREERTOS_PATH` (*default*: `/opt/FreeRTOS`) variable and format them as `IMPORTED` targets.
`FREERTOS_PATH` can be either the path to the whole
[FreeRTOS/FreeRTOS](https://github.com/FreeRTOS/FreeRTOS) github repo, or the path to
FreeRTOS-Kernel (usually located in the subfolder `FreeRTOS` on a downloaded release).
`FREERTOS_PATH` can be supplied as an environmental variable as well.

It is possible to either use the FreeRTOS kernel provided in the Cube repositories, or a separate
FreeRTOS kernel. The Cube repository also provides the CMSIS RTOS and CMSIS RTOS V2 implementations.
If the CMSIS implementations is used, it is recommended to also use the FreeRTOS sources
provided in the Cube repository because the CMSIS port might be incompatible to newer kernel
versions. The FreeRTOS port to use is specified as a `FreeRTOS` component. A list of available
ports can be found below. If the FreeRTOS sources provided in the Cube repository are used, the
device family also has to be specified as a component for the `FreeRTOS` package.

CMSIS RTOS can be used by specifying a `CMSIS` target and by finding the CMSIS `RTOS` package.
The following section will show a few example configurations for the H7 and F4 family.
You can also find example code for several devices in the [examples](https://github.com/ObKo/stm32-cmake/tree/master/examples/freertos)
folder.

Typical usage for a H7 device when using the M7 core, using an external kernel without CMSIS
support. The FreeRTOS namespace is set to `FreeRTOS` and the `ARM_CM7` port is used:

```cmake
find_package(CMSIS COMPONENTS STM32H743ZI STM32H7_M7 REQUIRED)
find_package(FreeRTOS ARM_CM7 REQUIRED)
target_link_libraries(${TARGET_NAME} PRIVATE
    ...
    FreeRTOS::ARM_CM7
)
```

Typical usage for a F4 device, using an external kernel without CMSIS support.
The FreeRTOS namespace is set to `FreeRTOS` and the `ARM_CM4F` port is used:

```cmake
find_package(FreeRTOS COMPONENTS ARM_CM4F REQUIRED)
target_link_libraries(${TARGET_NAME} PRIVATE
    ...
    FreeRTOS::ARM_CM4F
)
```

For ARMv8-M architecture (CM23 and CM33) you can choose "No Trust Zone" port:

```cmake
find_package(FreeRTOS COMPONENTS ARM_CM33_NTZ REQUIRED)
target_link_libraries(${TARGET_NAME} PRIVATE
    ...
    FreeRTOS::ARM_CM33_NTZ
)
```

Or you can use the trust zone with:

```cmake
find_package(FreeRTOS COMPONENTS ARM_CM33 REQUIRED)
target_link_libraries(${SECURE_TARGET_NAME} PRIVATE
    ...
    FreeRTOS::ARM_CM33::SECURE
)
target_link_libraries(${NON_SECURE_TARGET_NAME} PRIVATE
    ...
    FreeRTOS::ARM_CM33::NON_SECURE
)
```

Another typical usage using the FreeRTOS provided in the Cube repository and the CMSIS support.
The FreeRTOS namespace is set to `FreeRTOS::STM32::<FAMILY>`, the `ARM_CM7` port is used and
the device family is specified as a `FreeRTOS` component with `STM32H7`:

```cmake
find_package(CMSIS COMPONENTS STM32H743ZI STM32H7_M7 RTOS REQUIRED)
find_package(FreeRTOS COMPONENTS ARM_CM7 STM32H7 REQUIRED)
target_link_libraries(${TARGET_NAME} PRIVATE
    ...
    FreeRTOS::STM32::H7::M7::ARM_CM7
    CMSIS::STM32::H7::M7::RTOS
)
```

The following CMSIS targets are available in general:

* `CMSIS::STM32::<Family>::RTOS`
* `CMSIS::STM32::<Family>::RTOS_V2`

The following additional FreeRTOS targets are available in general to use the FreeRTOS provided
in the Cube repository

* `FreeRTOS::STM32::<Family>`

For the multi-core architectures, both family and core need to be specified like shown in the
example above.

The following FreeRTOS ports are supported in general: `ARM_CM0`, `ARM_CM3`,
`ARM_CM3_MPU`, `ARM_CM4F`, `ARM_CM4_MPU`, `ARM_CM7`, `ARM_CM7_MPU`, 
`ARM_CM23`, `ARM_CM23_NTZ`, `ARM_CM33`, `ARM_CM33_NTZ`.

Other FreeRTOS libraries, with `FREERTOS_NAMESPACE` being set as specified in the examples above:

* `${FREERTOS_NAMESPACE}::Coroutine` - co-routines (`croutines.c`)
* `${FREERTOS_NAMESPACE}::EventGroups` - event groups (`event_groups.c`)
* `${FREERTOS_NAMESPACE}::StreamBuffer` - stream buffer (`stream_buffer.c`)
* `${FREERTOS_NAMESPACE}::Timers` - timers (`timers.c`)
* `${FREERTOS_NAMESPACE}::Heap::<N>` - heap implementation (`heap_<N>.c`), `<N>`: [1-5]
