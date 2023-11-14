cmake_minimum_required(VERSION 3.5)

include(${CMAKE_CURRENT_LIST_DIR}/../common/bin2array.cmake)

set(TOOLCHAIN_PREFIX / CACHE PATH "Path to arm toolchain")
set(STM32Cube_DIR / CACHE PATH "Path to STM32 hal library")

set(FLASHER_DIR          ${CMAKE_CURRENT_LIST_DIR}/../..)
set(CMAKE_TOOLCHAIN_FILE ${FLASHER_DIR}/submodules/stm32-cmake/cmake/gcc_stm32.cmake)
set(STM32_CHIP           STM32F407VG)
set(PORT                 STM32)

project(stm32_flasher)
    
enable_language(ASM)

create_resources(${CMAKE_CURRENT_LIST_DIR}/../binaries/Hello-world Src/binaries.c)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} Inc)
 
add_executable(${CMAKE_PROJECT_NAME} 
    ../common/example_common.c
    Src/main.c
    Src/stm32f4xx_hal_msp.c
    Src/stm32f4xx_it.c
    Src/libc_compat.c
    Src/binaries.c)

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ../common)

add_subdirectory(${FLASHER_DIR}/submodules ${CMAKE_BINARY_DIR}/submodules)
add_subdirectory(${FLASHER_DIR} ${CMAKE_BINARY_DIR}/flasher)

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE stm_cube flasher)
 
STM32_ADD_HEX_BIN_TARGETS(${CMAKE_PROJECT_NAME})

add_custom_target(BuildBinary ALL)
add_dependencies(BuildBinary ${CMAKE_PROJECT_NAME}.bin)