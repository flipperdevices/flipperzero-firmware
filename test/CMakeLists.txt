cmake_minimum_required(VERSION 3.5)
project(serial_flasher_test)

set( CMAKE_CXX_FLAGS "-Wall -Werror -O3 -std=c++14" )

include_directories( ../include ../private_include ../test )

set( TEST_SOURCES
  test_main.cpp 
  ../src/serial_comm.c
  ../src/esp_loader.c
)

# Set -DQEMU_TEST=True to run qemu tests. Host tests are run otherwise.
if( QEMU_TEST ) 
    message("Qemu test ")
    set(TEST_SOURCES ${TEST_SOURCES} serial_io_tcp.cpp qemu_test.cpp)
else()
    message("Host test")
    set(TEST_SOURCES ${TEST_SOURCES} serial_io_mock.cpp test.cpp )
endif()

add_executable( ${PROJECT_NAME} ${TEST_SOURCES} )
