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
