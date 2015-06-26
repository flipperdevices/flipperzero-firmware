PROJECT(stm32cmsis)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
ENABLE_LANGUAGE(ASM)

IF(${STM32_FAMILY} STREQUAL "F1")
    IF(NOT STM32F1_StdPeriphLib_DIR)
        SET(STM32F1_StdPeriphLib_DIR "/opt/STM32F10x_StdPeriph_Lib_V3.5.0")
        MESSAGE(STATUS "No STM32F1_StdPeriphLib_DIR specified, using default: " ${STM32F1_StdPeriphLib_DIR})
    ENDIF()

    INCLUDE_DIRECTORIES(
        ${STM32F1_StdPeriphLib_DIR}/Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x/
        ${STM32F1_StdPeriphLib_DIR}/Libraries/CMSIS/CM3/CoreSupport/
    )
    
    SET(CMSIS_HEADERS 
        ${STM32F1_StdPeriphLib_DIR}/Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x/system_stm32f10x.h
        ${STM32F1_StdPeriphLib_DIR}/Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x/stm32f10x.h
        ${STM32F1_StdPeriphLib_DIR}/Libraries/CMSIS/CM3/CoreSupport/core_cm3.h
    )
    
    SET(CMSIS_SOURCES 
        ${STM32F1_StdPeriphLib_DIR}/Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x/system_stm32f10x.c
    )
    
    SET(CMSIS_LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/stm32f1_flash.ld.in)
    SET(STARTUP_PREFIX ${STM32F1_StdPeriphLib_DIR}/Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x/startup/gcc_ride7/startup_stm32f10x_)
ELSEIF(${STM32_FAMILY} STREQUAL "F4")
    IF(NOT STM32Cube_DIR)
        SET(STM32Cube_DIR "/opt/STM32Cube_FW_F4_V1.6.0")
        MESSAGE(STATUS "No STM32Cube_DIR specified, using default: " ${STM32Cube_DIR})
    ENDIF()

    INCLUDE_DIRECTORIES(
        ${STM32Cube_DIR}/Drivers/CMSIS/Device/ST/STM32F4xx/Include
        ${STM32Cube_DIR}/Drivers/CMSIS/Include
    )
    
    SET(CMSIS_HEADERS 
        ${STM32Cube_DIR}/Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f4xx.h
        ${STM32Cube_DIR}/Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f401xc.h
        ${STM32Cube_DIR}/Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f401xe.h
        ${STM32Cube_DIR}/Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f405xx.h
        ${STM32Cube_DIR}/Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f407xx.h
        ${STM32Cube_DIR}/Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f411xe.h
        ${STM32Cube_DIR}/Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f415xx.h
        ${STM32Cube_DIR}/Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f417xx.h
        ${STM32Cube_DIR}/Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f427xx.h
        ${STM32Cube_DIR}/Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f429xx.h
        ${STM32Cube_DIR}/Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f437xx.h
        ${STM32Cube_DIR}/Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f439xx.h
        ${STM32Cube_DIR}/Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f446xx.h
        ${STM32Cube_DIR}/Drivers/CMSIS/Device/ST/STM32F4xx/Include/system_stm32f4xx.h
        
        ${STM32Cube_DIR}/Drivers/CMSIS/Include/arm_common_tables.h
        ${STM32Cube_DIR}/Drivers/CMSIS/Include/arm_const_structs.h
        ${STM32Cube_DIR}/Drivers/CMSIS/Include/arm_math.h
        ${STM32Cube_DIR}/Drivers/CMSIS/Include/core_cm4.h
        ${STM32Cube_DIR}/Drivers/CMSIS/Include/core_cmFunc.h
        ${STM32Cube_DIR}/Drivers/CMSIS/Include/core_cmInstr.h
        ${STM32Cube_DIR}/Drivers/CMSIS/Include/core_cmSimd.h
    )
    
    SET(CMSIS_SOURCES 
        ${STM32Cube_DIR}/Drivers/CMSIS/Device/ST/STM32F4xx/Source/Templates/system_stm32f4xx.c
    )
    
    SET(CMSIS_LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/stm32f4.ld.in)
    SET(STARTUP_PREFIX ${STM32Cube_DIR}/Drivers/CMSIS/Device/ST/STM32F4xx/Source/Templates/gcc/startup_stm32f)
ENDIF()

FOREACH(CHIP_TYPE ${STM32_CHIP_TYPES})
    STRING(TOLOWER ${CHIP_TYPE} CHIP_TYPE_LOWER)
    LIST(APPEND CMSIS_STARTUP_FILES ${STARTUP_PREFIX}${CHIP_TYPE_LOWER}.s)
ENDFOREACH()

INSTALL(FILES ${CMSIS_HEADERS} DESTINATION include/stm32${STM32_FAMILY_LOWER})
INSTALL(FILES ${CMSIS_STARTUP_FILES} ${CMSIS_LINKER_SCRIPT} DESTINATION share/cmsis/)
INSTALL(FILES ${CMSIS_SOURCES} DESTINATION share/cmsis/)
