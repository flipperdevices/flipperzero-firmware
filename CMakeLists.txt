# version
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)
cmake_minimum_required(VERSION 3.7)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# specify cross compilers and tools
set(TOOLCHAIN_BIN_PATH "C:/work/cmake-stm32/toolchain/gcc-arm-none-eabi-10-2020-q4-major/bin")
set(OPENOCD_PATH "C:/work/cmake-stm32/toolchain/GNU MCU Eclipse/OpenOCD/0.10.0-15")
set(ST_CUBEPROG_PATH "C:/Program Files/STMicroelectronics/STM32Cube/STM32CubeProgrammer/bin")

set(STM32_TARGET_TRIPLET arm-none-eabi)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(FTARGET 7)
set(FTARGET_PATH "firmware/targets/f${FTARGET}")

find_program(CMAKE_C_COMPILER NAMES ${STM32_TARGET_TRIPLET}-gcc PATHS ${TOOLCHAIN_BIN_PATH})
find_program(CMAKE_CXX_COMPILER NAMES ${STM32_TARGET_TRIPLET}-g++ PATHS ${TOOLCHAIN_BIN_PATH})
find_program(CMAKE_ASM_COMPILER NAMES ${STM32_TARGET_TRIPLET}-gcc PATHS ${TOOLCHAIN_BIN_PATH})
find_program(CMAKE_AR NAMES ${STM32_TARGET_TRIPLET}-ar PATHS ${TOOLCHAIN_BIN_PATH})
find_program(CMAKE_OBJCOPY NAMES ${STM32_TARGET_TRIPLET}-objcopy PATHS ${TOOLCHAIN_BIN_PATH})
find_program(CMAKE_OBJDUMP NAMES ${STM32_TARGET_TRIPLET}-objdump PATHS ${TOOLCHAIN_BIN_PATH})
find_program(SIZE NAMES ${STM32_TARGET_TRIPLET}-size PATHS ${TOOLCHAIN_BIN_PATH})


# project start
PROJECT(STM32 C CXX ASM)

file(TOUCH_NOCREATE "core/version.h")

add_subdirectory(${FTARGET_PATH})

include_directories(${INCLUDE_DIRECTORIES})
include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/core)
include_directories(${CMAKE_SOURCE_DIR}/applications)
include_directories(${CMAKE_SOURCE_DIR}/applications/input)
include_directories(${CMAKE_SOURCE_DIR}/assets/compiled)
include_directories(${CMAKE_SOURCE_DIR}/firmware/targets/furi_hal_include)
include_directories(${CMAKE_SOURCE_DIR}/lib)
include_directories(${CMAKE_SOURCE_DIR}/lib/mlib)
include_directories(${CMAKE_SOURCE_DIR}/lib/u8g2)
include_directories(${CMAKE_SOURCE_DIR}/lib/ST25RFAL002)
include_directories(${CMAKE_SOURCE_DIR}/lib/ST25RFAL002/include)
include_directories(${CMAKE_SOURCE_DIR}/lib/ST25RFAL002/source/st25r3916)
include_directories(${CMAKE_SOURCE_DIR}/lib/callbackconnector)
include_directories(${CMAKE_SOURCE_DIR}/lib/app-template)
include_directories(${CMAKE_SOURCE_DIR}/firmware/targets/Inc)
include_directories(${CMAKE_SOURCE_DIR}/lib/callback-connector)
include_directories(${CMAKE_SOURCE_DIR}/lib/fnv1a-hash)
include_directories(${CMAKE_SOURCE_DIR}/lib/onewire)
#include_directories(${CMAKE_SOURCE_DIR}/lib/one_wire)
include_directories(${CMAKE_SOURCE_DIR}/lib/littlefs)
include_directories(${CMAKE_SOURCE_DIR}/lib/cyfral)
include_directories(${CMAKE_SOURCE_DIR}/lib/drivers)
include_directories(${CMAKE_SOURCE_DIR}/lib/common-api)
include_directories(${CMAKE_SOURCE_DIR}/lib/version)
include_directories(${CMAKE_SOURCE_DIR}/lib/infrared/encoder_decoder)
include_directories(${CMAKE_SOURCE_DIR}/lib/infrared/worker)
include_directories(${CMAKE_SOURCE_DIR}/lib/args)
include_directories(${CMAKE_SOURCE_DIR}/lib/app-scened-template)
include_directories(${CMAKE_SOURCE_DIR}/lib/app_scene_template)
include_directories(${CMAKE_SOURCE_DIR}/lib/file_reader)
include_directories(${CMAKE_SOURCE_DIR}/lib/nfc_protocols)
include_directories(${CMAKE_SOURCE_DIR}/lib/nanopb)
include_directories(${CMAKE_SOURCE_DIR}/lib/libusb_stm32/inc)
include_directories(${CMAKE_SOURCE_DIR}/lib/flipper_file)
include_directories(${CMAKE_SOURCE_DIR}/lib/micro-ecc)
include_directories(${CMAKE_SOURCE_DIR}/lib/microtar/src)

file(GLOB CORE_SOURCES 
    "core/*.c"
    "core/*.cpp"
    "lib/u8g2/u8x8_d_st7565.c"
    "lib/u8g2/u8g2_d_setup.c"
    "lib/u8g2/u8g2_intersection.c"
    "lib/u8g2/u8g2_setup.c"
    "lib/u8g2/u8g2_d_memory.c"
    "lib/u8g2/u8x8_cad.c"
    "lib/u8g2/u8x8_byte.c"
    "lib/u8g2/u8x8_gpio.c"
    "lib/u8g2/u8x8_display.c"
    "lib/u8g2/u8x8_setup.c"
    "lib/u8g2/u8g2_hvline.c"
    "lib/u8g2/u8g2_ll_hvline.c"
    "lib/u8g2/u8g2_circle.c"
    "lib/u8g2/u8g2_box.c"
    "lib/u8g2/u8g2_buffer.c"
    "lib/u8g2/u8g2_font.c"
    "lib/u8g2/u8g2_fonts.c"
    "lib/u8g2/u8x8_8x8.c"
    "lib/u8g2/u8g2_bitmap.c"
    "lib/u8g2/u8g2_line.c"
    "lib/u8g2/u8g2_glue.c"
    "lib/app-template/app-template.cpp"

    "lib/FatFs/*.c"
    "lib/FatFs/option/syscall.c"
    "lib/FatFs/option/unicode.c"

    "lib/ST25RFAL002/*.c"
    "lib/ST25RFAL002/source/*.c"
    "lib/ST25RFAL002/source/st25r3916/*.c"
    "lib/fnv1a-hash/*.c"

    "lib/one_wire/*.c"
    "lib/one_wire/*/*.c"
    "lib/one_wire/*/*/*.c"
    "lib/onewire/*.c"
    "lib/onewire/*.cpp"
    "lib/littlefs/*.c"
    "lib/drivers/*.c"
    "lib/version/*.c"
    "lib/nfc_protocols/*.c"
    "lib/args/*.c"
    "lib/app-scened-template/*.c"
    "lib/app-scened-template/*.cpp"
    "lib/app-scened-template/*/*.cpp"
    "lib/file_reader/*.cpp"
    "lib/common-api/*.c"
    "lib/subghz/*.c"
    "lib/subghz/*/*.c"
    "lib/nanopb/*.c"

    "lib/infrared/encoder_decoder/*.c"
    "lib/infrared/encoder_decoder/*/*.c"
    "lib/infrared/worker/*.c"
    "lib/toolbox/*.c"
    "lib/toolbox/*/*.c"
    "lib/toolbox/*.cpp"
    "lib/toolbox/*/*.cpp"

    "applications/*.c"
    "applications/*/*.c"
    "applications/*/*/*.c"
    "applications/*/*/*/*.c"
    "applications/*/*.cpp"
    "applications/*/*/*.cpp"
    "applications/*/*/*/*.cpp"

    "core/furi_hal/*.c"
    "core/furi/*.c"

    "assets/compiled/*.c"

    "lib/libusb_stm32/src/*.c"
    "lib/nanopb/*.c"
    "lib/heatshrink/*.c"
    "lib/flipper_file/*.c"
    "lib/flipper_format/*.c"
    "lib/micro-ecc/*.c"
    "lib/microtar/*/*.c"
    "lib/update_util/*.c"

)

set(SOURCES 
    ${SOURCES} 
    ${CORE_SOURCES} 
)

add_executable(${PROJECT_NAME}.elf ${SOURCES} ${LINKER_SCRIPT})

target_compile_options(${PROJECT_NAME}.elf PRIVATE 
    -mcpu=cortex-m4
    -Og
    -g
    -Wall
    -Wno-address-of-packed-member
    #-Wswitch
    -ffunction-sections
    -fdata-sections
    -fstack-usage
    -fno-math-errno
    -MMD
    -MP
    --specs=nano.specs
    -mfpu=fpv4-sp-d16
    -mfloat-abi=hard
    -mthumb
    -u _printf_float
    #-Werror
    -fcallgraph-info=su
    #-Wextra
    $<$<COMPILE_LANGUAGE:C>:-std=gnu17>
    $<$<COMPILE_LANGUAGE:C>:-Wenum-conversion>
    $<$<COMPILE_LANGUAGE:CXX>:-std=gnu++17>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-threadsafe-statics>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-use-cxa-atexit>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions>
    #-ffixed-r9
)

target_link_options(${PROJECT_NAME}.elf PRIVATE 
    -mcpu=cortex-m4
    -T ${LINKER_SCRIPT}
    --specs=nosys.specs
    -Wl,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map
    -Wl,--gc-sections -static
    -Wl,--undefined=uxTopUsedPriority
    --specs=nano.specs
    -mfpu=fpv4-sp-d16
    -mfloat-abi=hard
    -mthumb
    -fno-math-errno
    -u _printf_float
    -fstack-usage
    -Wl,--start-group -lstdc++ -lsupc++
    -Wl,--end-group
    -Wl,--print-memory-usage -T ${LINKER_SCRIPT}
    -Wl,--wrap,_malloc_r
    -Wl,--wrap,_free_r
    -Wl,--wrap,_calloc_r
    -Wl,--wrap,_realloc_r
    #-ffixed-r9
)

# # Modify compile flags to change optimization level from O3 to Og
string(REGEX REPLACE "([\\/\\-]O)3" "\\1g"
  CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
string(REGEX REPLACE "([\\/\\-]O)3" "\\1g"
  CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
string(REGEX REPLACE "([\\/\\-]O)3" "\\1g"
  CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL}")
string(REGEX REPLACE "([\\/\\-]O)3" "\\1g"
  CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")

string(REGEX REPLACE "([\\/\\-]O)3" "\\1g"
  CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
string(REGEX REPLACE "([\\/\\-]O)3" "\\1g"
  CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
string(REGEX REPLACE "([\\/\\-]O)3" "\\1g"
  CMAKE_C_FLAGS_MINSIZEREL "${CMAKE_C_FLAGS_MINSIZEREL}")
string(REGEX REPLACE "([\\/\\-]O)3" "\\1g"
  CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO}")

# Modify compile flags to change optimization level from O3 to Os
# string(REGEX REPLACE "([\\/\\-]O)3" "\\1s"
#   CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
# string(REGEX REPLACE "([\\/\\-]O)3" "\\1s"
#   CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
# string(REGEX REPLACE "([\\/\\-]O)3" "\\1s"
#   CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL}")
# string(REGEX REPLACE "([\\/\\-]O)3" "\\1s"
#   CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")

# string(REGEX REPLACE "([\\/\\-]O)3" "\\1s"
#   CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
# string(REGEX REPLACE "([\\/\\-]O)3" "\\1s"
#   CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
# string(REGEX REPLACE "([\\/\\-]O)3" "\\1s"
#   CMAKE_C_FLAGS_MINSIZEREL "${CMAKE_C_FLAGS_MINSIZEREL}")
# string(REGEX REPLACE "([\\/\\-]O)3" "\\1s"
#   CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO}")

message(STATUS "Target flags: ${TARGET_FLAGS}")

set(COMMON_FLAGS
    # USE_HAL_DRIVER
    USE_FULL_LL_DRIVER
    HAVE_FREERTOS
    LFS_NO_ASSERT
    # NO_BOOTLOADER
    _GNU_SOURCE
    NDEBUG
    FURI_DEBUG
)

execute_process(
    COMMAND git rev-parse --short HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT
    OUTPUT_STRIP_TRAILING_WHITESPACE)

execute_process(
    COMMAND git rev-parse --abbrev-ref HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_BRANCH
    OUTPUT_STRIP_TRAILING_WHITESPACE)

execute_process(
    COMMAND git rev-list --count HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_BRANCH_NUM
    OUTPUT_STRIP_TRAILING_WHITESPACE)

execute_process(
    COMMAND date /T
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE BUILD_DATE
    OUTPUT_STRIP_TRAILING_WHITESPACE)

execute_process(
    COMMAND git describe --tags --abbrev=0 --exact-match
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE)

# set(EX_INFO_FLAGS
#     -DGIT_BRANCH="${GIT_BRANCH}"
#     -DGIT_COMMIT="${GIT_COMMIT}"
#     -DGIT_BRANCH_NUM="${GIT_BRANCH_NUM}"
#     -DVERSION="${VERSION}"
#     -DBUILD_DATE="${BUILD_DATE}"
# )

set(EX_INFO_FLAGS
    -DGIT_BRANCH="branch"
    -DGIT_COMMIT="commit"
    -DGIT_BRANCH_NUM="666"
    -DVERSION="666"
    -DBUILD_DATE="000"
    -DBUILD_DIRTY=1
)

set(INFO_FLAGS
    TARGET=${FTARGET}
)

set_property(SOURCE "lib/toolbox/version.c" APPEND PROPERTY COMPILE_OPTIONS ${EX_INFO_FLAGS})
set_property(SOURCE "firmware/targets/f6/ble_glue/dev_info_service.c" APPEND PROPERTY COMPILE_OPTIONS ${EX_INFO_FLAGS})
set_property(SOURCE "firmware/targets/f7/ble_glue/dev_info_service.c" APPEND PROPERTY COMPILE_OPTIONS ${EX_INFO_FLAGS})
set_property(SOURCE "firmware/targets/f8/ble_glue/dev_info_service.c" APPEND PROPERTY COMPILE_OPTIONS ${EX_INFO_FLAGS})
set_property(SOURCE "firmware/targets/f9/ble_glue/dev_info_service.c" APPEND PROPERTY COMPILE_OPTIONS ${EX_INFO_FLAGS})

message(STATUS "Info flags: ${INFO_FLAGS}")

set(RELEASE_APP_FLAGS
    SRV_BT
    SRV_CLI
    SRV_DIALOGS
    SRV_DOLPHIN
    SRV_GUI
    SRV_INPUT
    SRV_LOADER
    SRV_NOTIFICATION
    SRV_POWER
    SRV_POWER_OBSERVER
    SRV_RPC
    SRV_STORAGE
    SRV_DESKTOP
    
    # Apps
    APP_ARCHIVE
    #APP_NFC
    #APP_GPIO
    #APP_IBUTTON
    #APP_INFRARED
    #APP_LF_RFID
    APP_SUBGHZ
    APP_ABOUT
    
    # APP_BAD_USB
    # APP_U2F
    # APP_UPDATER

    # Plugins
    # APP_MUSIC_PLAYER
    # APP_SNAKE_GAME

    # Debug
    # APP_ACCESSOR
    # APP_BLINK
    # APP_IRDA_MONITOR
    # APP_KEYPAD_TEST
    # APP_SD_TEST
    # APP_VIBRO_DEMO
    # APP_USB_TEST
    # APP_DISPLAY_TEST
    # APP_USB_MOUSE
    # APP_BAD_USB
    # APP_UART_ECHO

    #
    # APP_UNIT_TESTS
    # APP_MUSIC_MIDI
    # APP_ELF_LOADER
    # LOADER_AUTOSTART="Elf Loader"
    # HEAP_PRINT_DEBUG
    # APP_MASS_STORAGE
)

set(PROTOBUF_FLAGS
    PB_ENABLE_MALLOC
)

set(MINIMAL_APP_FLAGS
    SRV_MENU
    SRV_GUI
    SRV_INPUT
    SRV_CLI
)

target_compile_definitions(${PROJECT_NAME}.elf PRIVATE 
    ${TARGET_FLAGS}
    ${PROTOBUF_FLAGS}
    ${COMMON_FLAGS}
    ${INFO_FLAGS}
    ${RELEASE_APP_FLAGS}
    # ${MINIMAL_APP_FLAGS}
)

set(HEX_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.hex)
set(BIN_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.bin)

set_target_properties(${PROJECT_NAME}.elf PROPERTIES LINKER_LANGUAGE CXX)

# Print size
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND ${SIZE} ${PROJECT_NAME}.elf)

# Make binary file.
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${HEX_FILE}
    COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}.elf> ${BIN_FILE}
    COMMENT "Building ${HEX_FILE}
Building ${BIN_FILE}")

# Generate assembly listing.
add_custom_command(
    TARGET ${PROJECT_NAME}.elf
    COMMAND rm ${PROJECT_NAME}.lst -f
    COMMAND ${CMAKE_OBJDUMP}
    ARGS "-S" "${PROJECT_NAME}.elf" ">>" "${PROJECT_NAME}.lst")

# Flash firmware to target.
add_custom_target(flash
    ${ST_CUBEPROG_PATH}/STM32_Programmer_CLI --connect port=swd freq=24000 --write ${BIN_FILE} 0x08000000 --verify --start 0x08000000
    DEPENDS ${PROJECT_NAME}.elf
    COMMENT "Flashing target hardware")

# Run OpenOCD as a GDB host.
add_custom_target(gdbhost
    ${OPENOCD_PATH}/bin/openocd.exe -f ${CMAKE_SOURCE_DIR}/.vscode/stlink.cfg -c "transport select hla_swd"
    -f ${CMAKE_SOURCE_DIR}/.vscode/stm32wbx.cfg -c "init" -c "reset halt"
    DEPENDS ${PROJECT_NAME}.elf
    COMMENT "Running OpenOCD as a GDB host.")
