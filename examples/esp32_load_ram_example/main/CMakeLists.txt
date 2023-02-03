set(srcs main.c ../../common/example_common.c)
set(include_dirs . ../../common)

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${include_dirs})
set(target ${COMPONENT_LIB})

# Embed binaries into the app.
# In ESP-IDF this can also be done using EMBED_FILES option of idf_component_register.
# Here an external tool is used to make file embedding similar with other ports.
include(${CMAKE_CURRENT_LIST_DIR}/../../common/bin2array.cmake)
create_resources(${CMAKE_CURRENT_LIST_DIR}/../../binaries/RAM_APP ${CMAKE_BINARY_DIR}/binaries.c)
set_property(SOURCE ${CMAKE_BINARY_DIR}/binaries.c PROPERTY GENERATED 1)
target_sources(${target} PRIVATE ${CMAKE_BINARY_DIR}/binaries.c)
