PROJECT(stm32cmsis)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
ENABLE_LANGUAGE(ASM)

IF(${STM32_FAMILY} STREQUAL "F1")
IF(NOT STM32Cube_DIR)
        SET(STM32Cube_DIR "/opt/STM32Cube_FW_F1_V1.1.0")
        MESSAGE(STATUS "No STM32Cube_DIR specified, using default: " ${STM32Cube_DIR})
    ENDIF()
    
    SET(CMSIS_HEADERS        
        ${STM32Cube_DIR}/Drivers/CMSIS/Device/ST/STM32F1xx/Include/stm32f1xx.h
        ${STM32Cube_DIR}/Drivers/CMSIS/Device/ST/STM32F1xx/Include/stm32f100xb.h
        ${STM32Cube_DIR}/Drivers/CMSIS/Device/ST/STM32F1xx/Include/stm32f100xe.h
        ${STM32Cube_DIR}/Drivers/CMSIS/Device/ST/STM32F1xx/Include/stm32f101x6.h
        ${STM32Cube_DIR}/Drivers/CMSIS/Device/ST/STM32F1xx/Include/stm32f101xb.h
        ${STM32Cube_DIR}/Drivers/CMSIS/Device/ST/STM32F1xx/Include/stm32f101xe.h
        ${STM32Cube_DIR}/Drivers/CMSIS/Device/ST/STM32F1xx/Include/stm32f101xg.h
        ${STM32Cube_DIR}/Drivers/CMSIS/Device/ST/STM32F1xx/Include/stm32f102x6.h
        ${STM32Cube_DIR}/Drivers/CMSIS/Device/ST/STM32F1xx/Include/stm32f102xb.h
        ${STM32Cube_DIR}/Drivers/CMSIS/Device/ST/STM32F1xx/Include/stm32f103x6.h
        ${STM32Cube_DIR}/Drivers/CMSIS/Device/ST/STM32F1xx/Include/stm32f103xb.h
        ${STM32Cube_DIR}/Drivers/CMSIS/Device/ST/STM32F1xx/Include/stm32f103xe.h
        ${STM32Cube_DIR}/Drivers/CMSIS/Device/ST/STM32F1xx/Include/stm32f103xg.h
        ${STM32Cube_DIR}/Drivers/CMSIS/Device/ST/STM32F1xx/Include/stm32f105xc.h
        ${STM32Cube_DIR}/Drivers/CMSIS/Device/ST/STM32F1xx/Include/stm32f107xc.h
        ${STM32Cube_DIR}/Drivers/CMSIS/Device/ST/STM32F1xx/Include/system_stm32f1xx.h

        
        ${STM32Cube_DIR}/Drivers/CMSIS/Include/arm_common_tables.h
        ${STM32Cube_DIR}/Drivers/CMSIS/Include/arm_const_structs.h
        ${STM32Cube_DIR}/Drivers/CMSIS/Include/arm_math.h
        ${STM32Cube_DIR}/Drivers/CMSIS/Include/core_cm3.h
        ${STM32Cube_DIR}/Drivers/CMSIS/Include/core_cmFunc.h
        ${STM32Cube_DIR}/Drivers/CMSIS/Include/core_cmInstr.h
        ${STM32Cube_DIR}/Drivers/CMSIS/Include/core_cmSimd.h
    )
    
    SET(CMSIS_SOURCES 
        ${STM32Cube_DIR}/Drivers/CMSIS/Device/ST/STM32F1xx/Source/Templates/system_stm32f1xx.c
    )
    
    SET(CMSIS_LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/stm32f1.ld.in)
    SET(STARTUP_PREFIX ${STM32Cube_DIR}/Drivers/CMSIS/Device/ST/STM32F1xx/Source/Templates/gcc/startup_stm32f)
ELSEIF(${STM32_FAMILY} STREQUAL "F4")
    IF(NOT STM32Cube_DIR)
        SET(STM32Cube_DIR "/opt/STM32Cube_FW_F4_V1.6.0")
        MESSAGE(STATUS "No STM32Cube_DIR specified, using default: " ${STM32Cube_DIR})
    ENDIF()
    
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
