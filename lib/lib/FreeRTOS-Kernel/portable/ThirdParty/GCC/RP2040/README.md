## Overview

This directory provides a FreeRTOS-Kernel port that can be used with the Raspberry Pi Pico SDK. It supports:

 * Simple CMake INTERFACE libraries, to provide the FreeRTOS-Kernel and also the individual allocator types, without copying code into the user's project.
 * Running the FreeRTOS-Kernel and tasks on either core 0 or core 1
 * Use of SDK synchronization primitives (such as mutexes, semaphores, queues from pico_sync) between FreeRTOS tasks and code executing on the other core, or in IRQ handlers.

Note that a FreeRTOS SMP version of this port is also available in the FreeRTOS-Kernel smp branch, which additionally supports utilizing both RP2040 CPU cores for FreeRTOS tasks simultaneously.

## Using this port

Copy [FreeRTOS-Kernel-import.cmake](FreeRTOS-Kernel-import.cmake) into your project, and
add:

```cmake
import(FreeRTOS_Kernel_import.cmake)
```

below the usual import of `pico_sdk_import.cmake`

This will find the FreeRTOS kernel if it is a direct sub-module of your project, or if you provide the `FREERTOS_KERNEL_PATH` variable in your environment or via `-DFREERTOS_KERNEL_PATH=/path/to/FreeRTOS-Kernel` on the CMake command line.

## Advanced Configuration

Some additional `config` options are defined [here](include/rp2040_config.h) which control some low level implementation details.

## Known Limitations

- Tickless idle has not currently been tested, and is likely non-functional