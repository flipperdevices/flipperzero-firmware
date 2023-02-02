cmake_minimum_required(VERSION 3.5)

set(srcs
    src/esp_loader.c
    src/esp_targets.c
    src/md5_hash.c
    src/protocol.c
    src/slip.c
)


if (DEFINED ESP_PLATFORM)
    # Register component to esp-idf build system
    list(APPEND srcs port/esp32_port.c)
    if ("${IDF_VERSION_MAJOR}.${IDF_VERSION_MINOR}" VERSION_GREATER "4.0")
        # esp_timer component was introduced in v4.2
        set(priv_requires driver)
        if("${IDF_VERSION_MAJOR}.${IDF_VERSION_MINOR}" VERSION_GREATER "4.1")
            list(APPEND priv_requires esp_timer)
        endif()

        idf_component_register(SRCS ${srcs}
                               INCLUDE_DIRS include port
                               PRIV_INCLUDE_DIRS private_include
                               PRIV_REQUIRES ${priv_requires})

        set(target ${COMPONENT_LIB})
        component_compile_options(-Wstrict-prototypes)
    else()
        # Remove when dropping support for IDF 3.3
        set(COMPONENT_SRCS ${srcs})
        set(COMPONENT_ADD_INCLUDEDIRS include port)
        set(COMPONENT_PRIV_INCLUDEDIRS private_include)
        register_component()
        set(target ${COMPONENT_TARGET})
    endif()

else()
    # Create traditional CMake target
    add_library(flasher ${srcs})

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

if(DEFINED MD5_ENABLED OR CONFIG_SERIAL_FLASHER_MD5_ENABLED)
    target_compile_definitions(${target} PUBLIC -DMD5_ENABLED=1)
endif()

if(DEFINED CONFIG_SERIAL_FLASHER_RESET_HOLD_TIME_MS AND DEFINED CONFIG_SERIAL_FLASHER_BOOT_HOLD_TIME_MS)
    target_compile_definitions(${target}
    PUBLIC
        SERIAL_FLASHER_RESET_HOLD_TIME_MS=${CONFIG_SERIAL_FLASHER_RESET_HOLD_TIME_MS}
        SERIAL_FLASHER_BOOT_HOLD_TIME_MS=${CONFIG_SERIAL_FLASHER_BOOT_HOLD_TIME_MS}
    )
else()
    if(NOT DEFINED SERIAL_FLASHER_RESET_HOLD_TIME_MS)
        target_compile_definitions(${target}
        PUBLIC
            SERIAL_FLASHER_RESET_HOLD_TIME_MS=100
        )
    endif()
    if(NOT DEFINED SERIAL_FLASHER_BOOT_HOLD_TIME_MS)
        target_compile_definitions(${target}
        PUBLIC
            SERIAL_FLASHER_BOOT_HOLD_TIME_MS=50
        )
    endif()
endif()