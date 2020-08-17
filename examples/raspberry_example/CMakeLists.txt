cmake_minimum_required(VERSION 3.5)

set(FLASHER_DIR ${CMAKE_CURRENT_LIST_DIR}/../..)
set(PORT RASPBERRY_PI)

project(raspberry_flasher)
    
add_executable(${CMAKE_PROJECT_NAME} Src/main.c ../common/example_common.c)

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ../common/)

add_subdirectory(${FLASHER_DIR} ${CMAKE_BINARY_DIR}/flasher)

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE flasher)