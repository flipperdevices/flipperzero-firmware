function(stm32_util_create_family_targets FAMILY)
    set(CORES ${ARGN})
    list(LENGTH CORES NUM_CORES)
    if(${NUM_CORES} EQUAL 0)
        set(CORE "")
        set(CORE_C "")
    elseif(${NUM_CORES} EQUAL 1)
        set(CORE "_${CORES}")
        set(CORE_C "::${CORES}")
    else()
        message(FATAL_ERROR "Expected at most one core for family ${FAMILY}: ${CORES}")
    endif()

    if(NOT (TARGET STM32::${FAMILY}${CORE_C}))
        add_library(STM32::${FAMILY}${CORE_C} INTERFACE IMPORTED)
        target_compile_options(STM32::${FAMILY}${CORE_C} INTERFACE 
            --sysroot="${TOOLCHAIN_SYSROOT}"
            -mthumb -mabi=aapcs -Wall -ffunction-sections -fdata-sections -fno-strict-aliasing -fno-builtin -ffast-math
            $<$<CONFIG:Debug>:-Og>
            $<$<CONFIG:Release>:-Os>
        )
        target_link_options(STM32::${FAMILY}${CORE_C} INTERFACE 
            --sysroot="${TOOLCHAIN_SYSROOT}"
            -mthumb -mabi=aapcs -Wl,--gc-sections
            $<$<CONFIG:Debug>:-Og>
            $<$<CONFIG:Release>:-Os -s>
        )
        target_compile_definitions(STM32::${FAMILY}${CORE_C} INTERFACE 
            STM32${FAMILY}
        )
    endif()
    foreach(TYPE ${STM32_${FAMILY}_TYPES})
        if(NOT (TARGET STM32::${TYPE}${CORE_C}))
            add_library(STM32::${TYPE}${CORE_C} INTERFACE IMPORTED)
            target_link_libraries(STM32::${TYPE}${CORE_C} INTERFACE STM32::${FAMILY}${CORE_C})
            target_compile_definitions(STM32::${TYPE}${CORE_C} INTERFACE 
                STM32${TYPE}
            )
        endif()
    endforeach()
endfunction()

include(FetchContent)

FetchContent_Declare(
    STM32CubeF0
    GIT_REPOSITORY https://github.com/STMicroelectronics/STM32CubeF0/
    GIT_TAG        v1.11.1
    GIT_PROGRESS   TRUE
)
FetchContent_Declare(
    STM32CubeF1
    GIT_REPOSITORY https://github.com/STMicroelectronics/STM32CubeF1/
    GIT_TAG        v1.8.1
    GIT_PROGRESS   TRUE
)
FetchContent_Declare(
    STM32CubeF2
    GIT_REPOSITORY https://github.com/STMicroelectronics/STM32CubeF2/
    GIT_TAG        v1.9.0
    GIT_PROGRESS   TRUE
)
FetchContent_Declare(
    STM32CubeF3
    GIT_REPOSITORY https://github.com/STMicroelectronics/STM32CubeF3/
    GIT_TAG        v1.11.1
    GIT_PROGRESS   TRUE
)
FetchContent_Declare(
    STM32CubeF4
    GIT_REPOSITORY https://github.com/STMicroelectronics/STM32CubeF4/
    GIT_TAG        v1.25.1
    GIT_PROGRESS   TRUE
)
FetchContent_Declare(
    STM32CubeF7
    GIT_REPOSITORY https://github.com/STMicroelectronics/STM32CubeF7/
    GIT_TAG        v1.16.0
    GIT_PROGRESS   TRUE
)
FetchContent_Declare(
    STM32CubeG0
    GIT_REPOSITORY https://github.com/STMicroelectronics/STM32CubeG0/
    GIT_TAG        v1.3.0
    GIT_PROGRESS   TRUE
)
FetchContent_Declare(
    STM32CubeG4
    GIT_REPOSITORY https://github.com/STMicroelectronics/STM32CubeG4/
    GIT_TAG        v1.3.0
    GIT_PROGRESS   TRUE
)
FetchContent_Declare(
    STM32CubeH7
    GIT_REPOSITORY https://github.com/STMicroelectronics/STM32CubeH7/
    GIT_TAG        v1.8.0
    GIT_PROGRESS   TRUE
)
FetchContent_Declare(
    STM32CubeL0
    GIT_REPOSITORY https://github.com/STMicroelectronics/STM32CubeL0/
    GIT_TAG        v1.11.3
    GIT_PROGRESS   TRUE
)
FetchContent_Declare(
    STM32CubeL1
    GIT_REPOSITORY https://github.com/STMicroelectronics/STM32CubeL1/
    GIT_TAG        v1.10.0
    GIT_PROGRESS   TRUE
)
FetchContent_Declare(
    STM32CubeL4
    GIT_REPOSITORY https://github.com/STMicroelectronics/STM32CubeL4/
    GIT_TAG        v1.16.0
    GIT_PROGRESS   TRUE
)

function(stm32_fetch_cube)
    foreach(FAMILY ${ARGV})
        set(CUBE_NAME STM32Cube${FAMILY})
        string(TOLOWER ${CUBE_NAME} CUBE_NAME_L)
        
        if(${STM32_CUBE_${FAMILY}_PATH})
            message(INFO "STM32_CUBE_${FAMILY}_PATH specified, skipping fetch for ${CUBE_NAME}")
            continue()
        endif()
        
        if(NOT ${CUBE_NAME}_POPULATED)
            set(FETCHCONTENT_QUIET FALSE) # To see progress
            FetchContent_Populate(${CUBE_NAME})
        endif()
        
        set(STM32_CUBE_${FAMILY}_PATH ${${CUBE_NAME_L}_SOURCE_DIR} PARENT_SCOPE)
    endforeach()
endfunction()

