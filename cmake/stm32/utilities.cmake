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
            --sysroot="${TOOLCHAIN_SYSROOT}"
            -mthumb -Wall -ffunction-sections -fdata-sections
        )
        # Set linker flags
        # -mthumb: Generate thumb code
        # -Wl,--gc-sections: Remove unused code
        target_link_options(STM32::${FAMILY}${CORE_C} INTERFACE 
            --sysroot="${TOOLCHAIN_SYSROOT}"
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

include(FetchContent)

set(STM32_FETCH_FAMILIES       F0      F1      F2      F3      F4       F7      G0      G4      H7       L0      L1      L4      L5     U5     WB      WL    )
set(STM32_FETCH_CUBE_VERSIONS  v1.11.2 v1.8.4  v1.9.3  v1.11.2 v1.26.1  v1.16.1 v1.4.1  v1.4.0  v1.9.0   v1.12.0 v1.10.3 v1.17.0 v1.4.0 v1.0.0 v1.12.0 v1.1.0)
set(STM32_FETCH_CMSIS_VERSIONS v2.3.5  v4.3.3  v2.2.5  v2.3.5  v2.6.6   v1.2.6  v1.4.0  v1.2.1  v1.10.0  v1.9.1  v2.3.2  v1.7.1  v1.0.4 v1.0.0 v1.9.0  v1.1.0)
set(STM32_FETCH_HAL_VERSIONS   v1.7.5  v1.1.8  v1.2.7  v1.5.5  v1.7.12  v1.2.9  v1.4.1  v1.2.1  v1.10.0  v1.10.4 v1.4.4  v1.13.0 v1.0.4 v1.0.0 v1.9.0  v1.1.0)

FetchContent_Declare(
    STM32-CMSIS
    GIT_REPOSITORY https://github.com/STMicroelectronics/cmsis_core/
    GIT_TAG        v5.6.0
    GIT_PROGRESS   TRUE
)

set(IDX 0)
foreach(FAMILY ${STM32_FETCH_FAMILIES})
    string(TOLOWER ${FAMILY} FAMILY_L)
	list(GET STM32_FETCH_CUBE_VERSIONS ${IDX} CUBE_VERSION)
	list(GET STM32_FETCH_CMSIS_VERSIONS ${IDX} CMSIS_VERSION)
	list(GET STM32_FETCH_HAL_VERSIONS ${IDX} HAL_VERSION)
	
	FetchContent_Declare(
		STM32Cube${FAMILY}
		GIT_REPOSITORY https://github.com/STMicroelectronics/STM32Cube${FAMILY}/
		GIT_TAG        ${CUBE_VERSION}
		GIT_PROGRESS   TRUE
	)
	FetchContent_Declare(
		STM32-CMSIS-${FAMILY}
		GIT_REPOSITORY https://github.com/STMicroelectronics/cmsis_device_${FAMILY_L}/
		GIT_TAG        ${CMSIS_VERSION}
		GIT_PROGRESS   TRUE
	)
	FetchContent_Declare(
		STM32-HAL-${FAMILY}
		GIT_REPOSITORY https://github.com/STMicroelectronics/stm32${FAMILY_L}xx_hal_driver/
		GIT_TAG        ${HAL_VERSION}
		GIT_PROGRESS   TRUE
	)
	math(EXPR IDX "${IDX} + 1")
endforeach()

function(stm32_fetch_cube)
    foreach(FAMILY ${ARGV})
        set(CUBE_NAME STM32Cube${FAMILY})
        string(TOLOWER ${CUBE_NAME} CUBE_NAME_L)
        
        if(STM32_CUBE_${FAMILY}_PATH)
            message(INFO "STM32_CUBE_${FAMILY}_PATH specified, skipping fetch for ${CUBE_NAME}")
            continue()
        endif()
        
		FetchContent_GetProperties(${CUBE_NAME} POPULATED CUBE_POPULATED)
        if(NOT CUBE_POPULATED)
            set(FETCHCONTENT_QUIET FALSE) # To see progress
            FetchContent_Populate(${CUBE_NAME})
        endif()
        
        set(STM32_CUBE_${FAMILY}_PATH ${${CUBE_NAME_L}_SOURCE_DIR} PARENT_SCOPE)
    endforeach()
endfunction()

function(stm32_fetch_cmsis)
	if(NOT STM32_CMSIS_PATH)
        if(NOT STM32-CMSIS_POPULATED)
            set(FETCHCONTENT_QUIET FALSE) # To see progress
            FetchContent_Populate(STM32-CMSIS)
        endif()
        
        set(STM32_CMSIS_PATH ${stm32-cmsis_SOURCE_DIR} PARENT_SCOPE)
	else()
		message(INFO "STM32_CMSIS_PATH specified, skipping fetch for STM32-CMSIS")
	endif()
    foreach(FAMILY ${ARGV})
        set(CMSIS_NAME STM32-CMSIS-${FAMILY})
        string(TOLOWER ${CMSIS_NAME} CMSIS_NAME_L)
        
        if(STM32_CMSIS_${FAMILY}_PATH)
            message(INFO "STM32_CMSIS_${FAMILY}_PATH specified, skipping fetch for ${CMSIS_NAME}")
            continue()
        endif()
        
		FetchContent_GetProperties(${CMSIS_NAME_L} POPULATED CMSIS_POPULATED)
        if(NOT CMSIS_POPULATED)
            set(FETCHCONTENT_QUIET FALSE) # To see progress
            FetchContent_Populate(${CMSIS_NAME})
        endif()
        
        set(STM32_CMSIS_${FAMILY}_PATH ${${CMSIS_NAME_L}_SOURCE_DIR} PARENT_SCOPE)
    endforeach()
endfunction()

function(stm32_fetch_hal)
    foreach(FAMILY ${ARGV})
        set(HAL_NAME STM32-HAL-${FAMILY})
        string(TOLOWER ${HAL_NAME} HAL_NAME_L)
        
        if(STM32_HAL_${FAMILY}_PATH)
            message(INFO "STM32_HAL_${FAMILY}_PATH specified, skipping fetch for ${HAL_NAME}")
            continue()
        endif()
        
		FetchContent_GetProperties(${HAL_NAME} POPULATED HAL_POPULATED)
        if(NOT HAL_POPULATED)
            set(FETCHCONTENT_QUIET FALSE) # To see progress
            FetchContent_Populate(${HAL_NAME})
        endif()
        
        set(STM32_HAL_${FAMILY}_PATH ${${HAL_NAME_L}_SOURCE_DIR} PARENT_SCOPE)
    endforeach()
endfunction()
