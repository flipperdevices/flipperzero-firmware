cmake_minimum_required(VERSION 3.3)

enable_testing()

project(minmea)

find_package(Threads REQUIRED) # Workaround for https://github.com/libcheck/check/issues/48#issuecomment-322965461
find_package(PkgConfig)
pkg_check_modules(CHECK REQUIRED check)
link_directories(${CHECK_LIBRARY_DIRS})

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall -Wextra -Werror -std=c99")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_POSIX_C_SOURCE=199309L -D_BSD_SOURCE -D_DEFAULT_SOURCE -D_DARWIN_C_SOURCE")

set(minmea_SRCS minmea.c minmea.h)
add_library(minmea ${minmea_SRCS})

add_executable(example example.c)
target_link_libraries(example minmea)

add_executable(tests tests.c)
target_link_libraries(tests minmea ${CHECK_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
target_include_directories(tests PUBLIC ${CHECK_INCLUDE_DIRS})
target_compile_options(tests PUBLIC ${CHECK_CFLAGS_OTHER})

add_test(NAME tests COMMAND $<TARGET_FILE:tests>)
list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")

find_program(SCAN_FOUND scan-build)

if (SCAN_FOUND)
    add_test(
        NAME clang_static_analysis
        COMMAND scan-build make
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
else()
    MESSAGE(STATUS "scan-build not found, not scanning code")
endif()
