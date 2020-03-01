function(stm32_util_create_family_targets FAMILY)
    if(NOT (TARGET STM32::${FAMILY}))
        add_library(STM32::${FAMILY} INTERFACE IMPORTED)
        target_compile_options(STM32::${FAMILY} INTERFACE 
            --sysroot="${TOOLCHAIN_SYSROOT}"
            -mthumb -mabi=aapcs -Wall -ffunction-sections -fdata-sections -fno-strict-aliasing -fno-builtin -ffast-math
            $<$<CONFIG:Debug>:-Og>
            $<$<CONFIG:Release>:-Os>
        )
        target_link_options(STM32::${FAMILY} INTERFACE 
            --sysroot="${TOOLCHAIN_SYSROOT}"
            -mthumb -mabi=aapcs -Wl,--gc-sections
            $<$<CONFIG:Debug>:-Og>
            $<$<CONFIG:Release>:-Os -s>
        )
        target_compile_definitions(STM32::${FAMILY} INTERFACE 
            STM32${FAMILY}
        )
    endif()
    foreach(TYPE ${STM32_${FAMILY}_TYPES})
        if(NOT (TARGET STM32::${FAMILY}::${TYPE}))
            add_library(STM32::${FAMILY}::${TYPE} INTERFACE IMPORTED)
            target_link_libraries(STM32::${FAMILY}::${TYPE} INTERFACE STM32::${FAMILY})
            target_compile_definitions(STM32::${FAMILY}::${TYPE} INTERFACE 
                STM32F${TYPE}
            )
        endif()
    endforeach()
endfunction()
