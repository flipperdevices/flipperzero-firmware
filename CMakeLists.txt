cmake_minimum_required(VERSION 3.5)

if (DEFINED ESP_PLATFORM)

    # Register component to esp-idf build system
    set(COMPONENT_SRCS src/esp_loader.c src/serial_comm.c src/md5_hash.c port/esp32_port.c)
    set(COMPONENT_ADD_INCLUDEDIRS "include")
    set(COMPONENT_PRIV_INCLUDEDIRS "private_include" )
    register_component()
    component_compile_options(-Wstrict-prototypes)
    set(target ${COMPONENT_LIB})

else()

    # Create traditional CMake target
    add_library(flasher src/esp_loader.c src/serial_comm.c src/md5_hash.c)

    target_include_directories(flasher PUBLIC include PRIVATE private_include)

    if(PORT STREQUAL "STM32")
        target_link_libraries(flasher PUBLIC stm_cube)
        target_sources(flasher PRIVATE port/stm32_port.c)
    else()
        message(FATAL_ERROR "Selected port is not supported")
    endif()

    set(target flasher)

endif()


if(TARGET_SOC STREQUAL "ESP32")
    target_compile_definitions(${target} PUBLIC -DTARGET_ESP32)
elseif(TARGET_SOC STREQUAL "ESP32_S2")
    target_compile_definitions(${target} PUBLIC -DTARGET_ESP32_S2)
elseif(TARGET_SOC STREQUAL "ESP8266")
    target_compile_definitions(${target} PUBLIC -DTARGET_ESP8266)
endif()

if(DEFINED MD5_ENABLED)
    target_compile_definitions(${target} PUBLIC -DMD5_ENABLED=${MD5_ENABLED})
endif()
