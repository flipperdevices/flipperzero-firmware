cmake_minimum_required(VERSION 3.0.0)
project(HIDMinimalClient VERSION 0.1.0)

set(CMAKE_C_FLAGS "-Os -s")
set(CMAKE_CXX_FLAGS "-Os -s")

set(CMAKE_BUILD_TYPE Release)

option(BUILD_WIN "Cross compile for windows." OFF)
if (BUILD_WIN)
    set(CMAKE_TOOLCHAIN_FILE "./mingw.cmake")
endif()

set(CMAKE_TOOLCHAIN_FILE "./mingw.cmake")

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
        set(BUILD_FOR_WIN TRUE)
        message("Windows")
endif()
option(STATIC_BUILD "Build a static binary." ${BUILD_FOR_WIN})

if (STATIC_BUILD)
        set(CMAKE_EXE_LINKER_FLAGS "-static")
        set(CMAKE_FIND_LIBRARY_SUFFIXES ".a" CONFIG)
        set(BUILD_SHARED_LIBS OFF)
endif()

include(CTest)
enable_testing()

add_subdirectory(lib/hidapi)
add_executable(HIDMinimalClient src/main.c)

#find_package(hidapi REQUIRED)
target_link_libraries(HIDMinimalClient PRIVATE hidapi::hidapi)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
