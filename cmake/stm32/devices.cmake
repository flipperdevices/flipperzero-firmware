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
        # Set compiler flags for target
        # -Wall: all warnings activated
        # -ffunction-sections -fdata-sections: remove unused code
        target_compile_options(STM32::${FAMILY}${CORE_C} INTERFACE 
            -mthumb -Wall -ffunction-sections -fdata-sections
        )
        # Set linker flags
        # -mthumb: Generate thumb code
        # -Wl,--gc-sections: Remove unused code
        target_link_options(STM32::${FAMILY}${CORE_C} INTERFACE 
            -mthumb -Wl,--gc-sections
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

set(STM32_ALL_DEVICES "")
set(STM32_SUPPORTED_FAMILIES_LONG_NAME "")

include(stm32/c0)
include(stm32/f0)
include(stm32/f1)
include(stm32/f2)
include(stm32/f3)
include(stm32/f4)
include(stm32/f7)
include(stm32/g0)
include(stm32/g4)
include(stm32/h5)
include(stm32/h7)
include(stm32/l0)
include(stm32/l1)
include(stm32/l4)
include(stm32/l5)
include(stm32/u0)
include(stm32/u5)
include(stm32/wb)
include(stm32/wl)
include(stm32/mp1)


# Store a list of devices into a given STM_DEVICES list.
# You can also specify multiple device families. Examples:
# Get list of all devices for H7 family: stm32_get_devices_by_family(STM_DEVICES FAMILY H7)
# Get list of all devices: stm32_get_devices_by_family(STM_DEVICES)
function(stm32_get_devices_by_family STM_DEVICES)
    # Specify keywords for argument parsing here
    set(ARG_OPTIONS "")
    set(ARG_SINGLE "")
    set(ARG_MULTIPLE FAMILY)

    # Parse arguments. Multiple families can be specified and will be stored in ARG_<KeywordName>
    cmake_parse_arguments(PARSE_ARGV 1 ARG "${ARG_OPTIONS}" "${ARG_SINGLE}" "${ARG_MULTIPLE}")
    stm32_dev_parser_check()

    # Build a list of families by filtering the whole list with the specified families
    if(ARG_FAMILY)
        set(RESULTING_DEV_LIST "")
        foreach(FAMILY ${ARG_FAMILY})
            set(STM_DEVICE_LIST ${STM32_ALL_DEVICES})
            list(FILTER STM_DEVICE_LIST INCLUDE REGEX "^${FAMILY}")
            list(APPEND RESULTING_DEV_LIST ${STM_DEVICE_LIST})
            if(NOT STM_DEVICE_LIST)
                message(WARNING "No devices found for given family ${FAMILY}")
            endif()
        endforeach()
    else()
        # No family argument, so get list of all devices
        set(RESULTING_DEV_LIST ${STM32_ALL_DEVICES})
    endif()

    set(${STM_DEVICES} ${RESULTING_DEV_LIST} PARENT_SCOPE)
endfunction()

# Print the devices for a given family. You can also specify multiple device families.
# Example usage:
# Print devices for H7 family: stm32_print_devices_by_family(FAMILY H7)
# Print all devices: stm32_print_devices_by_family()
function(stm32_print_devices_by_family)
    # Specify keywords for argument parsing here
    set(ARG_OPTIONS "")
    set(ARG_SINGLE "")
    set(ARG_MULTIPLE FAMILY)

    # Parse arguments. Multiple families can be specified and will be stored in ARG_<KeywordName>
    cmake_parse_arguments(PARSE_ARGV 0 ARG "${ARG_OPTIONS}" "${ARG_SINGLE}" "${ARG_MULTIPLE}")
    stm32_dev_parser_check()

    if(ARG_FAMILY)
        # print devices one family per line
        foreach(FAMILY ${ARG_FAMILY})
            stm32_get_devices_by_family(STM_DEVICES FAMILY ${FAMILY})
            stm32_pretty_print_dev_list(${FAMILY} "${STM_DEVICES}")
        endforeach()
    else()
        # print all devices
        stm32_get_devices_by_family(STM_DEVICES)
        stm32_pretty_print_dev_list("all" "${STM_DEVICES}")
    endif()

endfunction()

# The arguments checked in this macro are filled by cmake_parse_argument
macro(stm32_dev_parser_check)
    # contains unexpected arguments (unknown keywords beofre ARG_MULTIPLE)
    if(ARG_UNPARSED_ARGUMENTS)
        message(WARNING "Unknown keyword(s) ${ARG_UNPARSED_ARGUMENTS} will be ignored")
    endif()
    # is populated if ARG_SINGLE or ARG_MULTIPLE is used without values
    if(ARG_KEYWORDS_MISSING_VALUES)
        message(FATAL_ERROR "Keyword ${ARG_KEYWORDS_MISSING_VALUES} expects values")
    endif()
endmacro()

# Pretty printer to limit amount of list entries printed per line
macro(stm32_pretty_print_dev_list FAMILIES STM_DEVICES)
    if(${FAMILIES} STREQUAL "all")
        message(STATUS  "Devices for all families")
    else()
        message(STATUS "Devices for ${FAMILIES} family")
    endif()
    set(TMP_LIST "")
    foreach(STM_DEVICE ${STM_DEVICES})
        list(APPEND TMP_LIST ${STM_DEVICE})
        list(LENGTH TMP_LIST CURR_LEN)
        if(CURR_LEN EQUAL 10)
            message(STATUS "${TMP_LIST}")
            set(TMP_LIST "")
        endif()
    endforeach()
    if(TMP_LIST)
        message(STATUS "${TMP_LIST}")
   endif()
endmacro()



include(FetchContent)

FetchContent_Declare(
    STM32-CMSIS
    GIT_REPOSITORY https://github.com/STMicroelectronics/cmsis_core/
    GIT_TAG        v5.6.0
    GIT_PROGRESS   TRUE
)

foreach(FAMILY ${STM32_FETCH_FAMILIES})
    string(TOLOWER ${FAMILY} FAMILY_L)
	
	FetchContent_Declare(
		STM32Cube${FAMILY}
		GIT_REPOSITORY https://github.com/STMicroelectronics/STM32Cube${FAMILY}/
		GIT_TAG        ${CUBE_${FAMILY}_VERSION}
		GIT_PROGRESS   TRUE
	)

    if(CMSIS_${FAMILY}_VERSION STREQUAL cube)
        set(STM32_USE_CMSIS_FROM_CUBE_${FAMILY} ON)
    else()
        FetchContent_Declare(
            STM32-CMSIS-${FAMILY}
            GIT_REPOSITORY https://github.com/STMicroelectronics/cmsis_device_${FAMILY_L}/
            GIT_TAG        ${CMSIS_${FAMILY}_VERSION}
            GIT_PROGRESS   TRUE
        )
    endif()
    
    if(HAL_${FAMILY}_VERSION STREQUAL cube)
        set(STM32_USE_HAL_FROM_CUBE_${FAMILY} ON)
    else()
        FetchContent_Declare(
            STM32-HAL-${FAMILY}
            GIT_REPOSITORY https://github.com/STMicroelectronics/stm32${FAMILY_L}xx_hal_driver/
            GIT_TAG        ${HAL_${FAMILY}_VERSION}
            GIT_PROGRESS   TRUE
        )
    endif()

endforeach()

function(stm32_fetch_cube)
    foreach(FAMILY ${ARGV})
        set(CUBE_NAME STM32Cube${FAMILY})
        string(TOLOWER ${CUBE_NAME} CUBE_NAME_L)
        
        if(STM32_CUBE_${FAMILY}_PATH)
            message(VERBOSE "STM32_CUBE_${FAMILY}_PATH specified, skipping fetch for ${CUBE_NAME}")
            continue()
        endif()
        
		FetchContent_MakeAvailable(${CUBE_NAME})
        set(STM32_CUBE_${FAMILY}_PATH ${${CUBE_NAME_L}_SOURCE_DIR} PARENT_SCOPE)
    endforeach()
endfunction()

function(stm32_fetch_cmsis)
	if(NOT STM32_CMSIS_PATH)
        FetchContent_MakeAvailable(STM32-CMSIS)
        set(STM32_CMSIS_PATH ${stm32-cmsis_SOURCE_DIR} PARENT_SCOPE)
	else()
		message(INFO "STM32_CMSIS_PATH specified, skipping fetch for STM32-CMSIS")
	endif()

    foreach(FAMILY ${ARGV})
        if(STM32_USE_CMSIS_FROM_CUBE_${FAMILY})
            stm32_fetch_cube(${FAMILY})
            message(STATUS "Cube fetched for ${FAMILY} at ${STM32_CUBE_${FAMILY}_PATH}")
            set(STM32_CMSIS_${FAMILY}_PATH ${STM32_CUBE_${FAMILY}_PATH}/Drivers/CMSIS/Device/ST/STM32${FAMILY}xx PARENT_SCOPE)
        else()
            set(CMSIS_NAME STM32-CMSIS-${FAMILY})
            string(TOLOWER ${CMSIS_NAME} CMSIS_NAME_L)
            
            if(STM32_CMSIS_${FAMILY}_PATH)
                message(INFO "STM32_CMSIS_${FAMILY}_PATH specified, skipping fetch for ${CMSIS_NAME}")
                continue()
            endif()
            
            FetchContent_MakeAvailable(${CMSIS_NAME})            
            set(STM32_CMSIS_${FAMILY}_PATH ${${CMSIS_NAME_L}_SOURCE_DIR} PARENT_SCOPE)
        endif()
    endforeach()
endfunction()

function(stm32_fetch_hal)
    foreach(FAMILY ${ARGV})
        if(STM32_USE_HAL_FROM_CUBE_${FAMILY})
            stm32_fetch_cube(${FAMILY})
            message(STATUS "Cube fetched for ${FAMILY} at ${STM32_CUBE_${FAMILY}_PATH}")
            set(STM32_HAL_${FAMILY}_PATH ${STM32_CUBE_${FAMILY}_PATH}/Drivers/STM32${FAMILY}xx_HAL_Driver PARENT_SCOPE)
        else()
            set(HAL_NAME STM32-HAL-${FAMILY})
            string(TOLOWER ${HAL_NAME} HAL_NAME_L)
            
            if(STM32_HAL_${FAMILY}_PATH)
                message(INFO "STM32_HAL_${FAMILY}_PATH specified, skipping fetch for ${HAL_NAME}")
                continue()
            endif()
            
            FetchContent_MakeAvailable(${HAL_NAME})
            set(STM32_HAL_${FAMILY}_PATH ${${HAL_NAME_L}_SOURCE_DIR} PARENT_SCOPE)
        endif()
    endforeach()
endfunction()
