cmake_minimum_required(VERSION 3.5)

if (DEFINED ESP_PLATFORM)

    # Register component to esp-idf build system
    set(COMPONENT_SRCS
        src/esp_loader.c
        src/esp_targets.c
        src/serial_comm.c
        src/md5_hash.c
        port/esp32_port.c)
    set(COMPONENT_ADD_INCLUDEDIRS "include port")
    set(COMPONENT_PRIV_INCLUDEDIRS "private_include" )
    register_component()
    component_compile_options(-Wstrict-prototypes)
    set(target ${COMPONENT_LIB})

else()

    # Create traditional CMake target
    add_library(flasher 
        src/esp_loader.c
        src/esp_targets.c
        src/serial_comm.c
        src/md5_hash.c)

    target_include_directories(flasher PUBLIC include port PRIVATE private_include)

    if(PORT STREQUAL "STM32")
        target_link_libraries(flasher PUBLIC stm_cube)
        target_sources(flasher PRIVATE port/stm32_port.c)
    elseif(PORT STREQUAL "RASPBERRY_PI")
        find_library(pigpio_LIB pigpio)
        target_link_libraries(flasher PUBLIC ${pigpio_LIB})
        target_sources(flasher PRIVATE port/raspberry_port.c)
    else()
        message(FATAL_ERROR "Selected port is not supported")
    endif()

    set(target flasher)

endif()

if(DEFINED MD5_ENABLED)
    target_compile_definitions(${target} PRIVATE -DMD5_ENABLED=${MD5_ENABLED})
endif()
