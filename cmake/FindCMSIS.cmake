if(NOT CMSIS_FIND_COMPONENTS)
    set(CMSIS_FIND_COMPONENTS
        STM32F0 STM32F1 STM32F2 STM32F3 STM32F4 STM32F7
        STM32G0 STM32G4
        STM32H7
        STM32L0 STM32L1 STM32L4
    )
endif()

include(stm32/devices)
    
function(cmsis_generate_default_linker_script FAMILY DEVICE CORE)
    string(REPLACE "_" "::" CORE_C ${CORE})
    stm32_get_memory_info(${FAMILY} ${DEVICE} ${CORE}
        FLASH_SIZE RAM_SIZE CCRAM_SIZE STACK_SIZE HEAP_SIZE 
        FLASH_ORIGIN RAM_ORIGIN CCRAM_ORIGIN
    )
    add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${DEVICE}${CORE}.ld"
        COMMAND ${CMAKE_COMMAND} 
            -DFLASH_ORIGIN="${FLASH_ORIGIN}" 
            -DRAM_ORIGIN="${RAM_ORIGIN}" 
            -DCCRAM_ORIGIN="${CCRAM_ORIGIN}" 
            -DFLASH_SIZE="${FLASH_SIZE}" 
            -DRAM_SIZE="${RAM_SIZE}" 
            -DCCRAM_SIZE="${CCRAM_SIZE}" 
            -DSTACK_SIZE="${STACK_SIZE}" 
            -DHEAP_SIZE="${HEAP_SIZE}" 
            -DLINKER_SCRIPT="${CMAKE_CURRENT_BINARY_DIR}/${DEVICE}${CORE}.ld"
            -P "${STM32_CMAKE_DIR}/stm32/linker_ld.cmake"
    )
    add_custom_target(CMSIS_LD_${DEVICE}${CORE} DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${DEVICE}${CORE}.ld)
    add_dependencies(CMSIS::STM32::${DEVICE}${CORE_C} CMSIS_LD_${DEVICE}${CORE})
    stm32_add_linker_script(CMSIS::STM32::${DEVICE}${CORE_C} INTERFACE "${CMAKE_CURRENT_BINARY_DIR}/${DEVICE}${CORE}.ld")
endfunction() 

foreach(COMP ${CMSIS_FIND_COMPONENTS})
    string(TOLOWER ${COMP} COMP_L)
    string(TOUPPER ${COMP} COMP)
    
    string(REGEX MATCH "^STM32([A-Z][0-9])([0-9A-Z][0-9][A-Z][0-9A-Z])?(_M[47])?.*$" COMP ${COMP})
    
    if((NOT CMAKE_MATCH_1) AND (NOT CMAKE_MATCH_2))
        message(FATAL_ERROR "Unknown CMSIS component: ${COMP}")
    endif()
    
    if(CMAKE_MATCH_2)
        set(FAMILY ${CMAKE_MATCH_1})
        set(DEVICES "${CMAKE_MATCH_1}${CMAKE_MATCH_2}")
    else()
        set(FAMILY ${CMAKE_MATCH_1})
        stm32_get_devices_by_family(${FAMILY} DEVICES)
    endif()
    if(CMAKE_MATCH_3)
        set(CORE ${CMAKE_MATCH_3})
        string(REGEX REPLACE "_" "::" CORE_C ${CORE})
    endif()
    string(TOLOWER ${FAMILY} FAMILY_L)
    
    if(NOT STM32_CUBE_${FAMILY}_PATH)
        set(STM32_CUBE_${FAMILY}_PATH /opt/STM32Cube${FAMILY} CACHE PATH "Path to STM32Cube${FAMILY}")
        message(STATUS "No STM32_CUBE_${FAMILY}_PATH specified using default: ${STM32_CUBE_${FAMILY}_PATH}")
    endif()
        
    find_path(CMSIS_${FAMILY}${CORE}_PATH
        NAMES Include/cmsis_gcc.h
        PATHS "${STM32_CUBE_${FAMILY}_PATH}/Drivers/CMSIS"
        NO_DEFAULT_PATH
    )
    if (NOT CMSIS_${FAMILY}${CORE}_PATH)
        continue()
    endif()
    
    if(NOT CMSIS_${FAMILY}${CORE}_VERSION)
        find_file(CMSIS_${FAMILY}${CORE}_PDSC
            NAMES ARM.CMSIS.pdsc
            PATHS "${CMSIS_${FAMILY}${CORE}_PATH}"
            NO_DEFAULT_PATH
        )
        if (NOT CMSIS_${FAMILY}${CORE}_PDSC)
            set(CMSIS_${FAMILY}${CORE}_VERSION "0.0.0")
        else()
            file(STRINGS "${CMSIS_${FAMILY}${CORE}_PDSC}" VERSION_STRINGS REGEX "<release version=\"([0-9]*\\.[0-9]*\\.[0-9]*)\" date=\"[0-9]+\\-[0-9]+\\-[0-9]+\">")
            list(GET VERSION_STRINGS 0 STR)
            string(REGEX MATCH "<release version=\"([0-9]*)\\.([0-9]*)\\.([0-9]*)\" date=\"[0-9]+\\-[0-9]+\\-[0-9]+\">" MATCHED ${STR})
            set(CMSIS_${FAMILY}${CORE}_VERSION "${CMAKE_MATCH_1}.${CMAKE_MATCH_2}.${CMAKE_MATCH_3}" CACHE INTERNAL "CMSIS STM32${FAMILY}${CORE} version")
        endif()
    endif()
    
    set(CMSIS_${COMP}_VERSION ${CMSIS_${FAMILY}${CORE}_VERSION})
    set(CMSIS_VERSION ${CMSIS_${COMP}_VERSION})
    
    find_path(CMSIS_${FAMILY}${CORE}_COMMON_INCLUDE
        NAMES cmsis_gcc.h
        PATHS "${CMSIS_${FAMILY}${CORE}_PATH}/Include"
        NO_DEFAULT_PATH
    )
    list(APPEND CMSIS_INCLUDE_DIRS "${CMSIS_${FAMILY}${CORE}_COMMON_INCLUDE}")
    
    find_path(CMSIS_${FAMILY}${CORE}_INCLUDE
        NAMES stm32${FAMILY_L}xx.h
        PATHS "${CMSIS_${FAMILY}${CORE}_PATH}/Device/ST/STM32${FAMILY}xx/Include"
        NO_DEFAULT_PATH
    )
    list(APPEND CMSIS_INCLUDE_DIRS "${CMSIS_${FAMILY}${CORE}_INCLUDE}")
    
    find_file(CMSIS_${FAMILY}${CORE}_SOURCE
        NAMES system_stm32${FAMILY_L}xx.c
        PATHS "${CMSIS_${FAMILY}${CORE}_PATH}/Device/ST/STM32${FAMILY}xx/Source/Templates"
        NO_DEFAULT_PATH
    )
    list(APPEND CMSIS_SOURCES "${CMSIS_${FAMILY}${CORE}_SOURCE}")
    
    if ((NOT CMSIS_${FAMILY}${CORE}_COMMON_INCLUDE) OR 
        (NOT CMSIS_${FAMILY}${CORE}_INCLUDE) OR 
        (NOT CMSIS_${FAMILY}${CORE}_SOURCE))
        continue()
    endif()

    if(NOT (TARGET CMSIS::STM32::${FAMILY}${CORE_C}))
        add_library(CMSIS::STM32::${FAMILY}${CORE_C} INTERFACE IMPORTED)
        target_link_libraries(CMSIS::STM32::${FAMILY}${CORE_C} INTERFACE STM32::${FAMILY}${CORE_C})
        target_include_directories(CMSIS::STM32::${FAMILY}${CORE_C} INTERFACE "${CMSIS_${FAMILY}${CORE}_COMMON_INCLUDE}")
        target_include_directories(CMSIS::STM32::${FAMILY}${CORE_C} INTERFACE "${CMSIS_${FAMILY}${CORE}_INCLUDE}")
        target_sources(CMSIS::STM32::${FAMILY}${CORE_C} INTERFACE "${CMSIS_${FAMILY}${CORE}_SOURCE}")
    endif()

    set(DEVICES_FOUND TRUE)
    foreach(DEVICE ${DEVICES})
        stm32_get_chip_type(${FAMILY} ${DEVICE} TYPE)
        string(TOLOWER ${DEVICE} DEVICE_L)
        string(TOLOWER ${TYPE} TYPE_L)
        
        find_file(CMSIS_${FAMILY}${CORE}_${TYPE}_STARTUP
            NAMES startup_stm32${TYPE_L}.s
            PATHS "${CMSIS_${FAMILY}${CORE}_PATH}/Device/ST/STM32${FAMILY}xx/Source/Templates/gcc"
            NO_DEFAULT_PATH
        )
        list(APPEND CMSIS_SOURCES "${CMSIS_${FAMILY}${CORE}_${TYPE}_STARTUP}")
        if(NOT CMSIS_${FAMILY}${CORE}_${TYPE}_STARTUP)
            set(DEVICES_FOUND FALSE)
            break()
        endif()
        
        if(NOT (TARGET CMSIS::STM32::${TYPE}${CORE}))
            add_library(CMSIS::STM32::${TYPE}${CORE_C} INTERFACE IMPORTED)
            target_link_libraries(CMSIS::STM32::${TYPE}${CORE_C} INTERFACE CMSIS::STM32::${FAMILY}${CORE_C} STM32::${TYPE}${CORE_C})
            target_sources(CMSIS::STM32::${TYPE}${CORE_C} INTERFACE "${CMSIS_${FAMILY}${CORE}_${TYPE}_STARTUP}")
        endif()
        
        add_library(CMSIS::STM32::${DEVICE}${CORE_C} INTERFACE IMPORTED)
        target_link_libraries(CMSIS::STM32::${DEVICE}${CORE_C} INTERFACE CMSIS::STM32::${TYPE}${CORE_C})
        cmsis_generate_default_linker_script(${FAMILY} ${DEVICE} ${CORE})
    endforeach()
        
    if(CMSIS_${FAMILY}${CORE}_COMMON_INCLUDE AND 
       CMSIS_${FAMILY}${CORE}_INCLUDE AND 
       CMSIS_${FAMILY}${CORE}_SOURCE AND
       DEVICES_FOUND)
       set(CMSIS_${COMP}_FOUND TRUE)
    else()
       set(CMSIS_${COMP}_FOUND FALSE)
    endif()
    list(REMOVE_DUPLICATES CMSIS_INCLUDE_DIRS)
    list(REMOVE_DUPLICATES CMSIS_SOURCES)
endforeach()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(CMSIS
    REQUIRED_VARS CMSIS_INCLUDE_DIRS CMSIS_SOURCES
    FOUND_VAR CMSIS_FOUND
    VERSION_VAR CMSIS_VERSION
    HANDLE_COMPONENTS
)
