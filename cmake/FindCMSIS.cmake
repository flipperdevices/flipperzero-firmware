if(NOT CMSIS_FIND_COMPONENTS)
    set(CMSIS_FIND_COMPONENTS STM32F4)
endif()

include(stm32/devices)
    
function(cmsis_generate_default_linker_script FAMILY DEVICE)    
    if(${FAMILY} STREQUAL "F4")
        stm32f4_memory_info(${DEVICE} 
            FLASH_SIZE RAM_SIZE CCRAM_SIZE STACK_SIZE HEAP_SIZE 
            FLASH_ORIGIN RAM_ORIGIN CCRAM_ORIGIN
        )
    endif()
    add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${DEVICE}.ld"
        COMMAND ${CMAKE_COMMAND} 
            -DFLASH_ORIGIN="${FLASH_ORIGIN}" 
            -DRAM_ORIGIN="${RAM_ORIGIN}" 
            -DCCRAM_ORIGIN="${CCRAM_ORIGIN}" 
            -DFLASH_SIZE="${FLASH_SIZE}" 
            -DRAM_SIZE="${RAM_SIZE}" 
            -DCCRAM_SIZE="${CCRAM_SIZE}" 
            -DSTACK_SIZE="${STACK_SIZE}" 
            -DHEAP_SIZE="${HEAP_SIZE}" 
            -DLINKER_SCRIPT="${CMAKE_CURRENT_BINARY_DIR}/${DEVICE}.ld"
            -P "${STM32_CMAKE_DIR}/stm32/linker_ld.cmake"
    )
    add_custom_target(CMSIS_LD_${DEVICE} DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${DEVICE}.ld)
    add_dependencies(CMSIS::STM32::${DEVICE} CMSIS_LD_${DEVICE})
    target_link_options(CMSIS::STM32::${DEVICE} INTERFACE -T "${CMAKE_CURRENT_BINARY_DIR}/${DEVICE}.ld")
endfunction() 

foreach(COMP ${CMSIS_FIND_COMPONENTS})
    string(TOLOWER ${COMP} COMP_L)
    string(TOUPPER ${COMP} COMP)
    
    string(REGEX MATCH "^STM32([A-Z][0-9])([0-9][0-9][A-Z][0-9A-Z])?.*$" COMP ${COMP})
    
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
    string(TOLOWER ${FAMILY} FAMILY_L)
    
    if(NOT STM32_CUBE_${FAMILY}_PATH)
        set(STM32_CUBE_${FAMILY}_PATH /opt/STM32Cube_FW_${FAMILY} CACHE PATH "Path to STM32Cube_FW_${FAMILY}")
        message(STATUS "No STM32_CUBE_${FAMILY}_PATH specified using default: ${STM32_CUBE_${FAMILY}_PATH}")
    endif()
    
    set(CMSIS_PATH "${STM32_CUBE_${FAMILY}_PATH}/Drivers/CMSIS")
    
    find_path(CMSIS_${FAMILY}_COMMON_INCLUDE
        NAMES cmsis_version.h
        PATHS "${CMSIS_PATH}/Include"
        NO_DEFAULT_PATH
    )
    list(APPEND CMSIS_INCLUDE_DIRS "${CMSIS_${FAMILY}_COMMON_INCLUDE}")
    
    find_path(CMSIS_${FAMILY}_INCLUDE
        NAMES stm32${FAMILY_L}xx.h
        PATHS "${CMSIS_PATH}/Device/ST/STM32${FAMILY}xx/Include"
        NO_DEFAULT_PATH
    )
    list(APPEND CMSIS_INCLUDE_DIRS "${CMSIS_${FAMILY}_INCLUDE}")
    
    find_file(CMSIS_${FAMILY}_SOURCE
        NAMES system_stm32${FAMILY_L}xx.c
        PATHS "${CMSIS_PATH}/Device/ST/STM32${FAMILY}xx/Source/Templates"
        NO_DEFAULT_PATH
    )
    list(APPEND CMSIS_SOURCES "${CMSIS_${FAMILY}_SOURCE}")
	
	if ((NOT CMSIS_${FAMILY}_COMMON_INCLUDE) OR 
	    (NOT CMSIS_${FAMILY}_INCLUDE) OR 
	    (NOT CMSIS_${FAMILY}_SOURCE))
		continue()
	endif()
	
    if(NOT (TARGET CMSIS::STM32::${FAMILY}))
		add_library(CMSIS::STM32::${FAMILY} INTERFACE IMPORTED)
		target_link_libraries(CMSIS::STM32::${FAMILY} INTERFACE STM32::${FAMILY})
		target_include_directories(CMSIS::STM32::${FAMILY} INTERFACE "${CMSIS_${FAMILY}_COMMON_INCLUDE}")
		target_include_directories(CMSIS::STM32::${FAMILY} INTERFACE "${CMSIS_${FAMILY}_INCLUDE}")
        target_sources(CMSIS::STM32::${FAMILY} INTERFACE "${CMSIS_${FAMILY}_SOURCE}")
	endif()

    set(DEVICES_FOUND TRUE)
    foreach(DEVICE ${DEVICES})
        stm32_get_chip_type(${FAMILY} ${DEVICE} TYPE)
        string(TOLOWER ${DEVICE} DEVICE_L)
        string(TOLOWER ${TYPE} TYPE_L)
        
        find_file(CMSIS_${FAMILY}_${TYPE}_STARTUP
            NAMES startup_stm32f${TYPE_L}.s
            PATHS "${CMSIS_PATH}/Device/ST/STM32${FAMILY}xx/Source/Templates/gcc"
            NO_DEFAULT_PATH
        )
        list(APPEND CMSIS_SOURCES "${CMSIS_${FAMILY}_${TYPE}_STARTUP}")
        if(NOT CMSIS_${FAMILY}_${TYPE}_STARTUP)
            set(DEVICES_FOUND FALSE)
            break()
        endif()
		
		if(NOT (TARGET CMSIS::STM32::${FAMILY}::${TYPE}))
			add_library(CMSIS::STM32::${FAMILY}::${TYPE} INTERFACE IMPORTED)
			target_link_libraries(CMSIS::STM32::${FAMILY}::${TYPE} INTERFACE CMSIS::STM32::${FAMILY} STM32::${FAMILY}::${TYPE})
			target_sources(CMSIS::STM32::${FAMILY}::${TYPE} INTERFACE "${CMSIS_${FAMILY}_${TYPE}_STARTUP}")
		endif()
        
        add_library(CMSIS::STM32::${DEVICE} INTERFACE IMPORTED)
        target_link_libraries(CMSIS::STM32::${DEVICE} INTERFACE CMSIS::STM32::${FAMILY}::${TYPE})
        cmsis_generate_default_linker_script(${FAMILY} ${DEVICE})
    endforeach()
        
    if(CMSIS_${FAMILY}_COMMON_INCLUDE AND 
       CMSIS_${FAMILY}_INCLUDE AND 
       CMSIS_${FAMILY}_SOURCE AND
       DEVICES_FOUND)
       set(CMSIS_${COMP}_FOUND TRUE)
    else()
       set(CMSIS_${COMP}_FOUND FALSE)
    endif()
endforeach()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(CMSIS
    REQUIRED_VARS CMSIS_INCLUDE_DIRS CMSIS_SOURCES
    FOUND_VAR CMSIS_FOUND
    HANDLE_COMPONENTS
)
