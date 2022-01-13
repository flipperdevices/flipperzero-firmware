set(srcs  main.c
          binaries.c
          ../../common/example_common.c
)

set(include_dirs . ../../common)

if ("${IDF_VERSION_MAJOR}.${IDF_VERSION_MINOR}" VERSION_GREATER "4.0")
    idf_component_register(SRCS ${srcs}
                           INCLUDE_DIRS ${include_dirs})
else()
    # remove when dropping IDF 3.3 support
    set(COMPONENT_SRCS ${srcs})
    set(COMPONENT_ADD_INCLUDEDIRS ${include_dirs})
    register_component()
endif()

set_property(SOURCE binaries.c PROPERTY GENERATED 1)

include(${CMAKE_CURRENT_LIST_DIR}/../../common/bin2array.cmake)

create_resources(${CMAKE_CURRENT_LIST_DIR}/../../binaries/Hello-world binaries.c)

