# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(zephyr_flasher)

zephyr_compile_definitions_ifdef(CONFIG_SERIAL_FLASHER_MD5_ENABLED MD5_ENABLED)

zephyr_library_sources(
  ../common/example_common.c
  )

zephyr_library_include_directories(
  ../common
  ../binaries
  )

# Embed binaries into the app.
include(${CMAKE_CURRENT_LIST_DIR}/../common/bin2array.cmake)
create_resources(${CMAKE_CURRENT_LIST_DIR}/../binaries/Hello-world ${CMAKE_BINARY_DIR}/binaries.c)
set_property(SOURCE ${CMAKE_BINARY_DIR}/binaries.c PROPERTY GENERATED 1)

target_sources(app PRIVATE src/main.c  ${CMAKE_BINARY_DIR}/binaries.c)
