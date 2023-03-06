cmake_minimum_required(VERSION 3.5)
project(serial_flasher_test)

add_executable( ${PROJECT_NAME}
	test_main.cpp
	../src/esp_loader.c
	../src/esp_targets.c
	../src/md5_hash.c
	../src/protocol.c
	../src/slip.c)

target_include_directories(${PROJECT_NAME} PRIVATE ../include ../private_include ../test ../port)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Werror -O3)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 14)

if( QEMU_TEST )
    target_sources(${PROJECT_NAME} PRIVATE serial_io_tcp.cpp qemu_test.cpp)
else()
    target_sources(${PROJECT_NAME} PRIVATE serial_io_mock.cpp test.cpp)
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE -DMD5_ENABLED=1)
