set(COMPONENT_SRCS 
    "src/esp_loader.c"
    "src/serial_comm.c"
    "port/esp32_uart.c")

set(COMPONENT_ADD_INCLUDEDIRS "include")

set(COMPONENT_PRIV_INCLUDEDIRS "private_include" )

if (NOT ESP_PLATFORM)
    message(FATAL_ERROR  "This CMakeLists.txt file is only for use inside ESP-IDF. 
                          It will need modifying to work standalone or in another project.")
endif()


register_component()

component_compile_options(-Wstrict-prototypes)