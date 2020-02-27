function(stm32_add_cmsis TARGET)
    stm32_get_chip(${TARGET} FAMILY DEVICE) 
    stm32_get_type(${FAMILY} ${DEVICE} TYPE)
    string(TOLOWER ${FAMILY} FAMILY_L)
    string(TOLOWER ${DEVICE} DEVICE_L)
    string(TOLOWER ${TYPE} TYPE_L)
    
    if(NOT STM32_CUBE_${FAMILY}_PATH)
        set(STM32_CUBE_${FAMILY}_PATH /opt/STM32Cube_FW_${FAMILY} CACHE PATH "Path to STM32Cube_FW_${FAMILY}")
        message(STATUS "No STM32_CUBE_${FAMILY}_PATH specified using default: ${STM32_CUBE_${FAMILY}_PATH}")
    endif()
    
    set(CMSIS_PATH "${STM32_CUBE_${FAMILY}_PATH}/Drivers/CMSIS")
    
    find_path(STM32_CMSIS_${FAMILY}_COMMON_INCLUDE
        NAMES cmsis_version.h
        PATHS "${CMSIS_PATH}/Include"
        NO_DEFAULT_PATH
    )
    find_path(STM32_CMSIS_${FAMILY}_INCLUDE
        NAMES stm32${FAMILY_L}xx.h
        PATHS "${CMSIS_PATH}/Device/ST/STM32${FAMILY}xx/Include"
        NO_DEFAULT_PATH
    )
    find_file(STM32_CMSIS_${FAMILY}_SOURCE
        NAMES system_stm32${FAMILY_L}xx.c
        PATHS "${CMSIS_PATH}/Device/ST/STM32${FAMILY}xx/Source/Templates"
        NO_DEFAULT_PATH
    )
    find_file(STM32_CMSIS_${FAMILY}_${TYPE}_STARTUP
        NAMES startup_stm32f${TYPE_L}.s
        PATHS "${CMSIS_PATH}/Device/ST/STM32${FAMILY}xx/Source/Templates/gcc"
        NO_DEFAULT_PATH
    )
    if((NOT STM32_CMSIS_${FAMILY}_COMMON_INCLUDE) OR
       (NOT STM32_CMSIS_${FAMILY}_INCLUDE) OR
       (NOT STM32_CMSIS_${FAMILY}_SOURCE) OR
       (NOT STM32_CMSIS_${FAMILY}_${TYPE}_STARTUP))
        message(FATAL_ERROR "Cannot find CMSIS. Make sure STM32_CUBE_${FAMILY}_PATH contains path to STM32Cube_FW_${FAMILY}.")
    endif()
            
    target_include_directories(${TARGET} PRIVATE 
        ${STM32_CMSIS_${FAMILY}_COMMON_INCLUDE} 
        ${STM32_CMSIS_${FAMILY}_INCLUDE}
    )
    target_sources(${TARGET} PRIVATE 
        ${STM32_CMSIS_${FAMILY}_${TYPE}_STARTUP}
        ${STM32_CMSIS_${FAMILY}_SOURCE}
    )    
endfunction()
