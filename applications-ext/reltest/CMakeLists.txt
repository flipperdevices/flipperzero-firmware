# Dirty Cmake file, will be deleted
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)
cmake_minimum_required(VERSION 3.7)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# specify cross compilers and tools
set(TOOLCHAIN_BIN_PATH "C:/work/cmake-stm32/arm_toolchain/gcc-arm-none-eabi-10-2020-q4-major/bin")
set(OPENOCD_PATH "C:/work/cmake-stm32/arm_toolchain/GNU MCU Eclipse/OpenOCD/0.10.0-15")
set(ST_CUBEPROG_PATH "C:/Program Files/STMicroelectronics/STM32Cube/STM32CubeProgrammer/bin")

set(STM32_TARGET_TRIPLET arm-none-eabi)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

find_program(CMAKE_C_COMPILER NAMES ${STM32_TARGET_TRIPLET}-gcc PATHS ${TOOLCHAIN_BIN_PATH})
find_program(CMAKE_CXX_COMPILER NAMES ${STM32_TARGET_TRIPLET}-g++ PATHS ${TOOLCHAIN_BIN_PATH})
find_program(CMAKE_ASM_COMPILER NAMES ${STM32_TARGET_TRIPLET}-gcc PATHS ${TOOLCHAIN_BIN_PATH})
find_program(CMAKE_AR NAMES ${STM32_TARGET_TRIPLET}-ar PATHS ${TOOLCHAIN_BIN_PATH})
find_program(CMAKE_OBJCOPY NAMES ${STM32_TARGET_TRIPLET}-objcopy PATHS ${TOOLCHAIN_BIN_PATH})
find_program(CMAKE_OBJDUMP NAMES ${STM32_TARGET_TRIPLET}-objdump PATHS ${TOOLCHAIN_BIN_PATH})
find_program(CMAKE_STRIP NAMES ${STM32_TARGET_TRIPLET}-strip PATHS ${TOOLCHAIN_BIN_PATH})
find_program(SIZE NAMES ${STM32_TARGET_TRIPLET}-size PATHS ${TOOLCHAIN_BIN_PATH})

# project start
PROJECT("plugin" C CXX ASM)

set(CUBE_DIR "${CMAKE_SOURCE_DIR}/../../lib/STM32CubeWB")

set(INCLUDES 
    "${CMAKE_SOURCE_DIR}"
    "${CMAKE_SOURCE_DIR}/../.."
    "${CMAKE_SOURCE_DIR}/../../core"
    "${CMAKE_SOURCE_DIR}/../../applications"
    "${CMAKE_SOURCE_DIR}/../../applications/input"
    "${CMAKE_SOURCE_DIR}/../../assets/compiled"
    "${CMAKE_SOURCE_DIR}/../../firmware/targets/furi-hal-include"
    "${CMAKE_SOURCE_DIR}/../../lib"
    "${CMAKE_SOURCE_DIR}/../../lib/mlib"
    "${CMAKE_SOURCE_DIR}/../../lib/u8g2"
    "${CMAKE_SOURCE_DIR}/../../lib/ST25RFAL002"
    "${CMAKE_SOURCE_DIR}/../../lib/ST25RFAL002/include"
    "${CMAKE_SOURCE_DIR}/../../lib/ST25RFAL002/source/st25r3916"
    "${CMAKE_SOURCE_DIR}/../../lib/callbackconnector"
    "${CMAKE_SOURCE_DIR}/../../lib/app-template"
    "${CMAKE_SOURCE_DIR}/../../firmware/targets/Inc"
    "${CMAKE_SOURCE_DIR}/../../lib/callback-connector"
    "${CMAKE_SOURCE_DIR}/../../lib/fnv1a-hash"
    "${CMAKE_SOURCE_DIR}/../../lib/onewire"
    "${CMAKE_SOURCE_DIR}/../../lib/littlefs"
    "${CMAKE_SOURCE_DIR}/../../lib/cyfral"
    "${CMAKE_SOURCE_DIR}/../../lib/drivers"
    "${CMAKE_SOURCE_DIR}/../../lib/common-api"
    "${CMAKE_SOURCE_DIR}/../../lib/version"
    "${CMAKE_SOURCE_DIR}/../../lib/irda/encoder_decoder"
    "${CMAKE_SOURCE_DIR}/../../lib/irda/worker"
    "${CMAKE_SOURCE_DIR}/../../lib/args"
    "${CMAKE_SOURCE_DIR}/../../lib/app-scened-template"
    "${CMAKE_SOURCE_DIR}/../../lib/app_scene_template"
    "${CMAKE_SOURCE_DIR}/../../lib/file_reader"
    "${CMAKE_SOURCE_DIR}/../../lib/nfc_protocols"
    "${CMAKE_SOURCE_DIR}/../../lib/nanopb"
    "${CMAKE_SOURCE_DIR}/../../lib/libusb_stm32/inc"
    "${CMAKE_SOURCE_DIR}/../../firmware/targets/f6/Inc"
    "${CMAKE_SOURCE_DIR}/../../firmware/targets/f6/furi-hal"
    "${CUBE_DIR}/Drivers/CMSIS/Device/ST/STM32WBxx/Include"
	"${CUBE_DIR}/Drivers/CMSIS/Device/ST"
	"${CUBE_DIR}/Drivers/CMSIS/Include"
    "${CUBE_DIR}/Drivers/STM32WBxx_HAL_Driver/Inc"
	"${CUBE_DIR}/Drivers/STM32WBxx_HAL_Driver/Inc/Legacy"
    "${CUBE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/include"
    "${CUBE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2"
    "${CUBE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F"
)

include_directories(${INCLUDES})

set(SOURCES 
    "main.c"
    "start.c"
    "snake-game.c"
)

add_executable(${PROJECT_NAME}.elf ${SOURCES} ${LINKER_SCRIPT})

target_compile_options(${PROJECT_NAME}.elf PRIVATE
    -mcpu=cortex-m3
    -mthumb
    -O0
    -ggdb3
	-mword-relocations
    -mlong-calls
    -fno-common
    -nostdlib
)

target_link_options(${PROJECT_NAME}.elf PRIVATE 
    -r
    -Bsymbolic
    -nostartfiles
	-mcpu=cortex-m3
    -mthumb
    -mlong-calls
    -fno-common
    -nostdlib
	-T "${CMAKE_SOURCE_DIR}/../application-ext.ld"
)

# Modify compile flags to change optimization level from O3 to O0
string(REGEX REPLACE "([\\/\\-]O)3" "\\10"
  CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
string(REGEX REPLACE "([\\/\\-]O)3" "\\10"
  CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
string(REGEX REPLACE "([\\/\\-]O)3" "\\10"
  CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL}")
string(REGEX REPLACE "([\\/\\-]O)3" "\\10"
  CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")

# string(REGEX REPLACE "([\\/\\-]O)3" "\\1g"
#   CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
# string(REGEX REPLACE "([\\/\\-]O)3" "\\1g"
#   CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
# string(REGEX REPLACE "([\\/\\-]O)3" "\\1g"
#   CMAKE_C_FLAGS_MINSIZEREL "${CMAKE_C_FLAGS_MINSIZEREL}")
# string(REGEX REPLACE "([\\/\\-]O)3" "\\1g"
#   CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO}")

set(COMMON_FLAGS
    NDEBUG
)

set(OTHER_FLAGS
    __STM32WBxx_FAMILY 
    __STM32WB55_SUBFAMILY 
    ARM_MATH_CM4 -DSTM32WB 
    USB_PMASIZE=0x400 
    STM32WB55xx 
    USE_FULL_LL_DRIVER 
    USE_HAL_DRIVER 
    HAVE_FREERTOS 
    PB_ENABLE_MALLOC 
    PB_WITHOUT_64BIT 
    _GNU_SOURCE
)

target_compile_definitions(${PROJECT_NAME}.elf PRIVATE 
    ${COMMON_FLAGS}
    ${OTHER_FLAGS}
)

set(HEX_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.hex)
set(BIN_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.bin)

set_target_properties(${PROJECT_NAME}.elf PROPERTIES LINKER_LANGUAGE C)

# Print size
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND ${SIZE} ${PROJECT_NAME}.elf)

# Make stripped elf file
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_STRIP} -g -o ${PROJECT_NAME}-striped.elf $<TARGET_FILE:${PROJECT_NAME}.elf>
    COMMENT "Building ${HEX_FILE}
Building ${BIN_FILE}")

# Generate assembly listing.
add_custom_command(
    TARGET ${PROJECT_NAME}.elf
    COMMAND rm ${PROJECT_NAME}.lst -f
    COMMAND ${CMAKE_OBJDUMP}
    ARGS "-S" "${PROJECT_NAME}.elf" ">>" "${PROJECT_NAME}.lst")