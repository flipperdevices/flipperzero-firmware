cmake_minimum_required(VERSION 3.3)

project(minmea)

set(core_SRCS minmea.c minmea.h)
add_library(core ${core_SRCS})
add_executable(exe example.c)
target_link_libraries(exe core)
