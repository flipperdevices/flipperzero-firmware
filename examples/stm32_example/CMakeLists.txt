cmake_minimum_required(VERSION 3.5)

# Creates C resources file from files in given directory
function(create_resources dir output)
    # Create empty output file
    file(WRITE ${output} "")
    # Collect input files
    file(GLOB bins ${dir}/*)
    # Iterate through input files
    foreach(bin ${bins})
        # Get short filenames
        string(REGEX MATCH "([^/]+)$" filename ${bin})
        message(STATUS "filename: " ${filename})
        # Replace filename spaces & extension separator for C compatibility
        string(REGEX REPLACE "\\.| |-" "_" filename ${filename})
        # Read hex data from file
        file(READ ${bin} filedata HEX)
        # Convert hex data for C compatibility
        string(REGEX REPLACE "([0-9a-f][0-9a-f])" "0x\\1," filedata ${filedata})
        # Append data to output file
        file(APPEND ${output} "const unsigned char ${filename}[] = {${filedata}};\nconst unsigned ${filename}_size = sizeof(${filename});\n")
    endforeach()
endfunction()

set(TOOLCHAIN_PREFIX / CACHE PATH "Path to arm toolchain")
set(STM32Cube_DIR / CACHE PATH "Path to STM32 hal library")

set(FLASHER_DIR          ${CMAKE_CURRENT_LIST_DIR}/../..)
set(CMAKE_TOOLCHAIN_FILE ${FLASHER_DIR}/submodules/stm32-cmake/cmake/gcc_stm32.cmake)
set(STM32_CHIP           STM32F407VG)
set(PORT                 STM32)

project(stm32_flasher)
    
enable_language(ASM)

create_resources(images Src/binaries.c)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} Inc)
 
add_executable(${CMAKE_PROJECT_NAME} 
    Src/main.c
    Src/stm32f4xx_hal_msp.c
    Src/stm32f4xx_it.c
    Src/libc_compat.c
    Src/binaries.c)

add_subdirectory(${FLASHER_DIR}/submodules ${CMAKE_BINARY_DIR}/submodules)
add_subdirectory(${FLASHER_DIR} ${CMAKE_BINARY_DIR}/flasher)

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE stm_cube flasher)
 
STM32_ADD_HEX_BIN_TARGETS(${CMAKE_PROJECT_NAME})

add_custom_target(BuildBinary ALL)
add_dependencies(BuildBinary ${CMAKE_PROJECT_NAME}.bin)