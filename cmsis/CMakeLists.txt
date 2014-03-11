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
    IF(NOT STM32F4_StdPeriphLib_DIR)
        SET(STM32F4_StdPeriphLib_DIR "/opt/STM32F4xx_DSP_StdPeriph_Lib_V1.3.0")
        MESSAGE(STATUS "No STM32F4_StdPeriphLib_DIR specified, using default: " ${STM32F4_StdPeriphLib_DIR})
    ENDIF()

    INCLUDE_DIRECTORIES(
        ${STM32F4_StdPeriphLib_DIR}/Libraries/CMSIS/Device/ST/STM32F4xx/Include/
        ${STM32F4_StdPeriphLib_DIR}/Libraries/CMSIS/Include/
    )
    
    SET(CMSIS_HEADERS 
        ${STM32F4_StdPeriphLib_DIR}/Libraries/CMSIS/Device/ST/STM32F4xx/Include/system_stm32f4xx.h
        ${STM32F4_StdPeriphLib_DIR}/Libraries/CMSIS/Device/ST/STM32F4xx/Include/stm32f4xx.h
        ${STM32F4_StdPeriphLib_DIR}/Libraries/CMSIS/Include/core_cm4.h
        ${STM32F4_StdPeriphLib_DIR}/Libraries/CMSIS/Include/core_cm4_simd.h
        ${STM32F4_StdPeriphLib_DIR}/Libraries/CMSIS/Include/core_cmFunc.h
        ${STM32F4_StdPeriphLib_DIR}/Libraries/CMSIS/Include/core_cmInstr.h
    )
    
    SET(CMSIS_SOURCES 
        ${STM32F4_StdPeriphLib_DIR}/Libraries/CMSIS/Device/ST/STM32F4xx/Source/Templates/system_stm32f4xx.c
    )
    
    SET(CMSIS_LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/stm32f4_flash.ld.in)
    SET(STARTUP_PREFIX ${STM32F4_StdPeriphLib_DIR}/Libraries/CMSIS/Device/ST/STM32F4xx/Source/Templates/gcc_ride7/startup_stm32f)
ENDIF()

INCLUDE_DIRECTORIES(
    ${CMAKE_CURRENT_SOURCE_DIR}
)

STRING(TOLOWER ${STM32_FAMILY} STM32_FAMILY_LOWER)
ADD_CUSTOM_TARGET(cmsis_${STM32_FAMILY_LOWER})

FOREACH(CHIP_TYPE ${STM32_CHIP_TYPES})
    STRING(TOLOWER ${CHIP_TYPE} CHIP_TYPE_LOWER)
    
    LIST(APPEND CMSIS_STARTUP_FILES ${STARTUP_PREFIX}${CHIP_TYPE_LOWER}.s)
    LIST(APPEND CMSIS_LIBRARIES cmsis_${STM32_FAMILY_LOWER}_${CHIP_TYPE_LOWER})
    SET(TARGET_NAME cmsis_${STM32_FAMILY_LOWER}_${CHIP_TYPE_LOWER})
    ADD_LIBRARY(${TARGET_NAME} ${CMSIS_SOURCES})
    STM32_SET_CHIP_DEFINITIONS(${TARGET_NAME} ${CHIP_TYPE})
    ADD_DEPENDENCIES(cmsis_${STM32_FAMILY_LOWER} ${TARGET_NAME})
ENDFOREACH()

INSTALL(TARGETS ${CMSIS_LIBRARIES} RUNTIME DESTINATION bin LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
INSTALL(FILES ${CMSIS_HEADERS} DESTINATION include/stm32${STM32_FAMILY_LOWER})
INSTALL(FILES ${CMSIS_STARTUP_FILES} ${CMSIS_LINKER_SCRIPT} DESTINATION share/cmsis/)
