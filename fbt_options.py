import os

# Variables starting with _ are not consumed by scons and are introduced only
# for ease of editing. If you want to override them on the commandline,
# use final variables (without _)

# Default hardware target
TARGET_HW = 7

# Optimization flags
## Optimize for size
COMPACT = 0
## Optimize for debugging experience
DEBUG = 1

DIST_SUFFIX = "local"

# Coprocessor firmware
COPRO_OB_DATA = "scripts/ob.data"

# Must match lib/STM32CubeWB version
COPRO_CUBE_VERSION = "1.13.3"

COPRO_MCU_FAMILY = "STM32WB5x"
COPRO_CUBE_DIR = "lib/STM32CubeWB"

# Default radio stack
COPRO_STACK_BIN = "stm32wb5x_BLE_Stack_light_fw.bin"
# Firmware also supports "ble_full", but it might not fit into debug builds
COPRO_STACK_TYPE = "ble_light"

# Leave 0 to lets scripts automatically calculate it
COPRO_STACK_ADDR = "0x0"

COPRO_STACK_BIN_DIR = os.path.join(
    COPRO_CUBE_DIR,
    "Projects",
    "STM32WB_Copro_Wireless_Binaries",
    COPRO_MCU_FAMILY,
)
