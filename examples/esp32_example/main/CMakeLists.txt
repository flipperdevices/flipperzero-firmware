idf_component_register(SRCS "main.c" "binaries.c" "../../common/example_common.c"
                       INCLUDE_DIRS "." "../../common")

set_property(SOURCE binaries.c PROPERTY GENERATED 1)

include(${CMAKE_CURRENT_LIST_DIR}/../../common/bin2array.cmake)

create_resources(${CMAKE_CURRENT_LIST_DIR}/../../binaries/Hello-world binaries.c)

