idf_component_register(SRCS "main.c" "binaries.c" "../../common/example_common.c"
                       INCLUDE_DIRS ".""../../common")

set_property(SOURCE binaries.c PROPERTY GENERATED 1)

include(${CMAKE_CURRENT_LIST_DIR}/../../common/bin2array.cmake)

if(TARGET_SOC STREQUAL "ESP32_S2")
    set(binary_path ${CMAKE_CURRENT_LIST_DIR}/../../binaries/ESP32_S2)
elseif(TARGET_SOC STREQUAL "ESP8266")
    set(binary_path ${CMAKE_CURRENT_LIST_DIR}/../../binaries/ESP8266)
else()
    set(binary_path ${CMAKE_CURRENT_LIST_DIR}/../../binaries/ESP32)
endif()

create_resources(${binary_path} binaries.c)

