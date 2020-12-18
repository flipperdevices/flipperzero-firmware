if(NOT CMSIS_FIND_COMPONENTS)
    set(CMSIS_FIND_COMPONENTS ${STM32_SUPPORTED_FAMILIES_LONG_NAME})
endif()
if(STM32H7 IN_LIST CMSIS_FIND_COMPONENTS)
    list(REMOVE_ITEM CMSIS_FIND_COMPONENTS STM32H7)
    list(APPEND CMSIS_FIND_COMPONENTS STM32H7_M7 STM32H7_M4)
endif()
list(REMOVE_DUPLICATES CMSIS_FIND_COMPONENTS)

include(stm32/devices)

function(cmsis_generate_default_linker_script FAMILY DEVICE CORE)
    if(CORE)
        set(CORE_C "::${CORE}")
        set(CORE_U "_${CORE}")
    endif()
    
    set(OUTPUT_LD_FILE "${CMAKE_CURRENT_BINARY_DIR}/${DEVICE}${CORE_U}.ld")
    
    stm32_get_memory_info(FAMILY ${FAMILY} DEVICE ${DEVICE} CORE ${CORE} FLASH SIZE FLASH_SIZE ORIGIN FLASH_ORIGIN)
    stm32_get_memory_info(FAMILY ${FAMILY} DEVICE ${DEVICE} CORE ${CORE} RAM SIZE RAM_SIZE ORIGIN RAM_ORIGIN)
    stm32_get_memory_info(FAMILY ${FAMILY} DEVICE ${DEVICE} CORE ${CORE} CCRAM SIZE CCRAM_SIZE ORIGIN CCRAM_ORIGIN)
    stm32_get_memory_info(FAMILY ${FAMILY} DEVICE ${DEVICE} CORE ${CORE} HEAP SIZE HEAP_SIZE)
    stm32_get_memory_info(FAMILY ${FAMILY} DEVICE ${DEVICE} CORE ${CORE} STACK SIZE STACK_SIZE)
    
    add_custom_command(OUTPUT "${OUTPUT_LD_FILE}"
        COMMAND ${CMAKE_COMMAND} 
            -DFLASH_ORIGIN="${FLASH_ORIGIN}" 
            -DRAM_ORIGIN="${RAM_ORIGIN}" 
            -DCCRAM_ORIGIN="${CCRAM_ORIGIN}" 
            -DFLASH_SIZE="${FLASH_SIZE}" 
            -DRAM_SIZE="${RAM_SIZE}" 
            -DCCRAM_SIZE="${CCRAM_SIZE}" 
            -DSTACK_SIZE="${STACK_SIZE}" 
            -DHEAP_SIZE="${HEAP_SIZE}" 
            -DLINKER_SCRIPT="${OUTPUT_LD_FILE}"
            -P "${STM32_CMAKE_DIR}/stm32/linker_ld.cmake"
    )
    add_custom_target(CMSIS_LD_${DEVICE}${CORE_U} DEPENDS "${OUTPUT_LD_FILE}")
    add_dependencies(CMSIS::STM32::${DEVICE}${CORE_C} CMSIS_LD_${DEVICE}${CORE_U})
    stm32_add_linker_script(CMSIS::STM32::${DEVICE}${CORE_C} INTERFACE "${OUTPUT_LD_FILE}")
endfunction() 

foreach(COMP ${CMSIS_FIND_COMPONENTS})
    string(TOLOWER ${COMP} COMP_L)
    string(TOUPPER ${COMP} COMP)
    
    string(REGEX MATCH "^STM32([A-Z][0-9])([0-9A-Z][0-9][A-Z][0-9A-Z])?_?(M[47])?.*$" COMP ${COMP})
    
    if((NOT CMAKE_MATCH_1) AND (NOT CMAKE_MATCH_2))
        message(FATAL_ERROR "Unknown CMSIS component: ${COMP}")
    endif()
    
    if(CMAKE_MATCH_2)
        set(FAMILY ${CMAKE_MATCH_1})
        set(DEVICES "${CMAKE_MATCH_1}${CMAKE_MATCH_2}")
    else()
        set(FAMILY ${CMAKE_MATCH_1})
        stm32_get_devices_by_family(DEVICES FAMILY ${FAMILY} CORE ${CORE})
    endif()
    
    if(CMAKE_MATCH_3)
        set(CORE ${CMAKE_MATCH_3})
        set(CORE_C "::${CORE}")
        set(CORE_U "_${CORE}")
    else()
        unset(CORE)
        unset(CORE_C)
        unset(CORE_U)
    endif()
    
    string(TOLOWER ${FAMILY} FAMILY_L)
    
    if((NOT STM32_CMSIS_${FAMILY}_PATH) AND (NOT STM32_CUBE_${FAMILY}_PATH))
        set(STM32_CUBE_${FAMILY}_PATH /opt/STM32Cube${FAMILY} CACHE PATH "Path to STM32Cube${FAMILY}")
        message(STATUS "Neither STM32_CUBE_${FAMILY}_PATH nor STM32_CMSIS_${FAMILY}_PATH specified using default  STM32_CUBE_${FAMILY}_PATH: ${STM32_CUBE_${FAMILY}_PATH}")
    endif()
        
    find_path(CMSIS_${FAMILY}${CORE_U}_CORE_PATH
        NAMES Include/cmsis_gcc.h
        PATHS "${STM32_CMSIS_PATH}" "${STM32_CUBE_${FAMILY}_PATH}/Drivers/CMSIS"
        NO_DEFAULT_PATH
    )
    if (NOT CMSIS_${FAMILY}${CORE_U}_CORE_PATH)
        continue()
    endif()
	
    find_path(CMSIS_${FAMILY}${CORE_U}_PATH
        NAMES Include/stm32${FAMILY_L}xx.h
        PATHS "${STM32_CMSIS_${FAMILY}_PATH}" "${STM32_CUBE_${FAMILY}_PATH}/Drivers/CMSIS/Device/ST/STM32${FAMILY}xx"
        NO_DEFAULT_PATH
    )
    if (NOT CMSIS_${FAMILY}${CORE_U}_PATH)
        continue()
    endif()
    list(APPEND CMSIS_INCLUDE_DIRS "${CMSIS_${FAMILY}${CORE_U}_CORE_PATH}/Include" "${CMSIS_${FAMILY}${CORE_U}_PATH}/Include")

    if(NOT CMSIS_${FAMILY}${CORE_U}_VERSION)
        find_file(CMSIS_${FAMILY}${CORE_U}_PDSC
            NAMES ARM.CMSIS.pdsc
            PATHS "${CMSIS_${FAMILY}${CORE_U}_CORE_PATH}"
            NO_DEFAULT_PATH
        )
        if (NOT CMSIS_${FAMILY}${CORE_U}_PDSC)
            set(CMSIS_${FAMILY}${CORE_U}_VERSION "0.0.0")
        else()
            file(STRINGS "${CMSIS_${FAMILY}${CORE_U}_PDSC}" VERSION_STRINGS REGEX "<release version=\"([0-9]*\\.[0-9]*\\.[0-9]*)\" date=\"[0-9]+\\-[0-9]+\\-[0-9]+\">")
            list(GET VERSION_STRINGS 0 STR)
            string(REGEX MATCH "<release version=\"([0-9]*)\\.([0-9]*)\\.([0-9]*)\" date=\"[0-9]+\\-[0-9]+\\-[0-9]+\">" MATCHED ${STR})
            set(CMSIS_${FAMILY}${CORE_U}_VERSION "${CMAKE_MATCH_1}.${CMAKE_MATCH_2}.${CMAKE_MATCH_3}" CACHE INTERNAL "CMSIS STM32${FAMILY}${CORE_U} version")
        endif()
    endif()
    
    set(CMSIS_${COMP}_VERSION ${CMSIS_${FAMILY}${CORE_U}_VERSION})
    set(CMSIS_VERSION ${CMSIS_${COMP}_VERSION})
        
    find_file(CMSIS_${FAMILY}${CORE_U}_SOURCE
        NAMES system_stm32${FAMILY_L}xx.c
        PATHS "${CMSIS_${FAMILY}${CORE_U}_PATH}/Source/Templates"
        NO_DEFAULT_PATH
    )
    list(APPEND CMSIS_SOURCES "${CMSIS_${FAMILY}${CORE_U}_SOURCE}")
    
    if (NOT CMSIS_${FAMILY}${CORE_U}_SOURCE)
        continue()
    endif()

    if(NOT (TARGET CMSIS::STM32::${FAMILY}${CORE_C}))
        add_library(CMSIS::STM32::${FAMILY}${CORE_C} INTERFACE IMPORTED)
        target_link_libraries(CMSIS::STM32::${FAMILY}${CORE_C} INTERFACE STM32::${FAMILY}${CORE_C})
        target_include_directories(CMSIS::STM32::${FAMILY}${CORE_C} INTERFACE "${CMSIS_${FAMILY}${CORE_U}_CORE_PATH}/Include")
        target_include_directories(CMSIS::STM32::${FAMILY}${CORE_C} INTERFACE "${CMSIS_${FAMILY}${CORE_U}_PATH}/Include")
        target_sources(CMSIS::STM32::${FAMILY}${CORE_C} INTERFACE "${CMSIS_${FAMILY}${CORE_U}_SOURCE}")
    endif()

    set(DEVICES_FOUND TRUE)
    foreach(DEVICE ${DEVICES})
        stm32_get_cores(DEV_CORES FAMILY ${FAMILY} DEVICE ${DEVICE})
        if(CORE AND (NOT ${CORE} IN_LIST DEV_CORES))
            continue()
        endif()
                
        stm32_get_chip_type(${FAMILY} ${DEVICE} TYPE)
        string(TOLOWER ${DEVICE} DEVICE_L)
        string(TOLOWER ${TYPE} TYPE_L)
        
        find_file(CMSIS_${FAMILY}${CORE_U}_${TYPE}_STARTUP
            NAMES startup_stm32${TYPE_L}.s
            PATHS "${CMSIS_${FAMILY}${CORE_U}_PATH}/Source/Templates/gcc"
            NO_DEFAULT_PATH
        )
        list(APPEND CMSIS_SOURCES "${CMSIS_${FAMILY}${CORE_U}_${TYPE}_STARTUP}")
        if(NOT CMSIS_${FAMILY}${CORE_U}_${TYPE}_STARTUP)
            set(DEVICES_FOUND FALSE)
            break()
        endif()
        
        if(NOT (TARGET CMSIS::STM32::${TYPE}${CORE_C}))
            add_library(CMSIS::STM32::${TYPE}${CORE_C} INTERFACE IMPORTED)
            target_link_libraries(CMSIS::STM32::${TYPE}${CORE_C} INTERFACE CMSIS::STM32::${FAMILY}${CORE_C} STM32::${TYPE}${CORE_C})
            target_sources(CMSIS::STM32::${TYPE}${CORE_C} INTERFACE "${CMSIS_${FAMILY}${CORE_U}_${TYPE}_STARTUP}")
        endif()
        
        add_library(CMSIS::STM32::${DEVICE}${CORE_C} INTERFACE IMPORTED)
        target_link_libraries(CMSIS::STM32::${DEVICE}${CORE_C} INTERFACE CMSIS::STM32::${TYPE}${CORE_C})
        cmsis_generate_default_linker_script(${FAMILY} ${DEVICE} "${CORE}")
    endforeach()

    if(DEVICES_FOUND)
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
