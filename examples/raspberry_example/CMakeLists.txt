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

set(FLASHER_DIR ${CMAKE_CURRENT_LIST_DIR}/../..)
set(PORT RASPBERRY_PI)

project(raspberry_flasher)
    
create_resources(images Src/binaries.c)

add_executable(${CMAKE_PROJECT_NAME} Src/main.c Src/binaries.c)

add_subdirectory(${FLASHER_DIR} ${CMAKE_BINARY_DIR}/flasher)

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE flasher)