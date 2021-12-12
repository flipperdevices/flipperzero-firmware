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
find_program(SIZE NAMES ${STM32_TARGET_TRIPLET}-size PATHS ${TOOLCHAIN_BIN_PATH})


# project start
PROJECT("plugin" C CXX ASM)

include_directories(${CMAKE_SOURCE_DIR})

set(SOURCES 
    "main.c"
    "start.c"
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
)

target_link_options(${PROJECT_NAME}.elf PRIVATE 
    -r
    -Bsymbolic
    -nostartfiles
	-mcpu=cortex-m3
    -mthumb
    -mlong-calls
    -fno-common
	-T "${CMAKE_SOURCE_DIR}/../application-ext.ld"
)

# # Modify compile flags to change optimization level from O3 to O0
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

target_compile_definitions(${PROJECT_NAME}.elf PRIVATE 
    ${COMMON_FLAGS}
)

set(HEX_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.hex)
set(BIN_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.bin)

set_target_properties(${PROJECT_NAME}.elf PROPERTIES LINKER_LANGUAGE C)

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