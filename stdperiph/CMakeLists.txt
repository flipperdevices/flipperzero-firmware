PROJECT(stm32stdperiph)

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
ENABLE_LANGUAGE(ASM)

IF(NOT DEFINED CMSIS_INCLUDE_DIR)
    FIND_PACKAGE(CMSIS REQUIRED)
ENDIF()

IF(${STM32_FAMILY} STREQUAL "F1")
    IF(NOT STM32F1_StdPeriphLib_DIR)
        SET(STM32F1_StdPeriphLib_DIR "/opt/STM32F10x_StdPeriph_Lib_V3.5.0")
        MESSAGE(STATUS "No STM32F1_StdPeriphLib_DIR specified, using default: " ${STM32F1_StdPeriphLib_DIR})
    ENDIF()

    INCLUDE_DIRECTORIES(
        ${STM32F1_StdPeriphLib_DIR}/Libraries/STM32F10x_StdPeriph_Driver/inc/
    )
    
    SET(STDPERIPH_SOURCE_DIR ${STM32F1_StdPeriphLib_DIR}/Libraries/STM32F10x_StdPeriph_Driver/src)
    SET(STDPERIPH_HEADER_DIR ${STM32F1_StdPeriphLib_DIR}/Libraries/STM32F10x_StdPeriph_Driver/inc)
    
    SET(STDPERIPH_HEADERS 
        ${STDPERIPH_HEADER_DIR}/stm32f10x_adc.h
        ${STDPERIPH_HEADER_DIR}/stm32f10x_bkp.h
        ${STDPERIPH_HEADER_DIR}/stm32f10x_can.h
        ${STDPERIPH_HEADER_DIR}/stm32f10x_cec.h
        ${STDPERIPH_HEADER_DIR}/stm32f10x_crc.h
        ${STDPERIPH_HEADER_DIR}/stm32f10x_dac.h
        ${STDPERIPH_HEADER_DIR}/stm32f10x_dbgmcu.h
        ${STDPERIPH_HEADER_DIR}/stm32f10x_dma.h
        ${STDPERIPH_HEADER_DIR}/stm32f10x_exti.h
        ${STDPERIPH_HEADER_DIR}/stm32f10x_flash.h
        ${STDPERIPH_HEADER_DIR}/stm32f10x_fsmc.h
        ${STDPERIPH_HEADER_DIR}/stm32f10x_gpio.h
        ${STDPERIPH_HEADER_DIR}/stm32f10x_i2c.h
        ${STDPERIPH_HEADER_DIR}/stm32f10x_iwdg.h
        ${STDPERIPH_HEADER_DIR}/stm32f10x_pwr.h
        ${STDPERIPH_HEADER_DIR}/stm32f10x_rcc.h
        ${STDPERIPH_HEADER_DIR}/stm32f10x_rtc.h
        ${STDPERIPH_HEADER_DIR}/stm32f10x_sdio.h
        ${STDPERIPH_HEADER_DIR}/stm32f10x_spi.h
        ${STDPERIPH_HEADER_DIR}/stm32f10x_tim.h
        ${STDPERIPH_HEADER_DIR}/stm32f10x_usart.h
        ${STDPERIPH_HEADER_DIR}/stm32f10x_wwdg.h
    )
    
    SET(STDPERIPH_SOURCES
        ${STDPERIPH_SOURCE_DIR}/misc.c
        ${STDPERIPH_SOURCE_DIR}/stm32f10x_adc.c
        ${STDPERIPH_SOURCE_DIR}/stm32f10x_bkp.c
        ${STDPERIPH_SOURCE_DIR}/stm32f10x_can.c
        ${STDPERIPH_SOURCE_DIR}/stm32f10x_cec.c
        ${STDPERIPH_SOURCE_DIR}/stm32f10x_crc.c
        ${STDPERIPH_SOURCE_DIR}/stm32f10x_dac.c
        ${STDPERIPH_SOURCE_DIR}/stm32f10x_dbgmcu.c
        ${STDPERIPH_SOURCE_DIR}/stm32f10x_dma.c
        ${STDPERIPH_SOURCE_DIR}/stm32f10x_exti.c
        ${STDPERIPH_SOURCE_DIR}/stm32f10x_flash.c
        ${STDPERIPH_SOURCE_DIR}/stm32f10x_fsmc.c
        ${STDPERIPH_SOURCE_DIR}/stm32f10x_gpio.c
        ${STDPERIPH_SOURCE_DIR}/stm32f10x_i2c.c
        ${STDPERIPH_SOURCE_DIR}/stm32f10x_iwdg.c
        ${STDPERIPH_SOURCE_DIR}/stm32f10x_pwr.c
        ${STDPERIPH_SOURCE_DIR}/stm32f10x_rcc.c
        ${STDPERIPH_SOURCE_DIR}/stm32f10x_rtc.c
        ${STDPERIPH_SOURCE_DIR}/stm32f10x_sdio.c
        ${STDPERIPH_SOURCE_DIR}/stm32f10x_spi.c
        ${STDPERIPH_SOURCE_DIR}/stm32f10x_tim.c
        ${STDPERIPH_SOURCE_DIR}/stm32f10x_usart.c
        ${STDPERIPH_SOURCE_DIR}/stm32f10x_wwdg.c
    )
    
    INSTALL(FILES ${STDPERIPH_HEADER_DIR}/misc.h RENAME stm32f10x_misc.h DESTINATION include/stm32f1)
ELSEIF(${STM32_FAMILY} STREQUAL "F4")
    IF(NOT STM32F4_StdPeriphLib_DIR)
        SET(STM32F4_StdPeriphLib_DIR "/opt/STM32F4xx_DSP_StdPeriph_Lib_V1.3.0")
        MESSAGE(STATUS "No STM32F4_StdPeriphLib_DIR specified, using default: " ${STM32F4_StdPeriphLib_DIR})
    ENDIF()

    INCLUDE_DIRECTORIES(
        ${STM32F4_StdPeriphLib_DIR}/Libraries/STM32F4xx_StdPeriph_Driver/inc/
    )
    
    SET(STDPERIPH_SOURCE_DIR ${STM32F4_StdPeriphLib_DIR}/Libraries/STM32F4xx_StdPeriph_Driver/src)
    SET(STDPERIPH_HEADER_DIR ${STM32F4_StdPeriphLib_DIR}/Libraries/STM32F4xx_StdPeriph_Driver/inc)
    
    SET(STDPERIPH_HEADERS         
        ${STDPERIPH_HEADER_DIR}/stm32f4xx_adc.h
        ${STDPERIPH_HEADER_DIR}/stm32f4xx_can.h
        ${STDPERIPH_HEADER_DIR}/stm32f4xx_crc.h
        ${STDPERIPH_HEADER_DIR}/stm32f4xx_cryp.h
        ${STDPERIPH_HEADER_DIR}/stm32f4xx_dac.h
        ${STDPERIPH_HEADER_DIR}/stm32f4xx_dbgmcu.h
        ${STDPERIPH_HEADER_DIR}/stm32f4xx_dcmi.h
        ${STDPERIPH_HEADER_DIR}/stm32f4xx_dma.h
        ${STDPERIPH_HEADER_DIR}/stm32f4xx_dma2d.h
        ${STDPERIPH_HEADER_DIR}/stm32f4xx_exti.h
        ${STDPERIPH_HEADER_DIR}/stm32f4xx_flash.h
        ${STDPERIPH_HEADER_DIR}/stm32f4xx_fmc.h
        ${STDPERIPH_HEADER_DIR}/stm32f4xx_fsmc.h
        ${STDPERIPH_HEADER_DIR}/stm32f4xx_gpio.h
        ${STDPERIPH_HEADER_DIR}/stm32f4xx_hash.h
        ${STDPERIPH_HEADER_DIR}/stm32f4xx_i2c.h
        ${STDPERIPH_HEADER_DIR}/stm32f4xx_iwdg.h
        ${STDPERIPH_HEADER_DIR}/stm32f4xx_ltdc.h
        ${STDPERIPH_HEADER_DIR}/stm32f4xx_pwr.h
        ${STDPERIPH_HEADER_DIR}/stm32f4xx_rcc.h
        ${STDPERIPH_HEADER_DIR}/stm32f4xx_rng.h
        ${STDPERIPH_HEADER_DIR}/stm32f4xx_rtc.h
        ${STDPERIPH_HEADER_DIR}/stm32f4xx_sai.h
        ${STDPERIPH_HEADER_DIR}/stm32f4xx_sdio.h
        ${STDPERIPH_HEADER_DIR}/stm32f4xx_spi.h
        ${STDPERIPH_HEADER_DIR}/stm32f4xx_syscfg.h
        ${STDPERIPH_HEADER_DIR}/stm32f4xx_tim.h
        ${STDPERIPH_HEADER_DIR}/stm32f4xx_usart.h
        ${STDPERIPH_HEADER_DIR}/stm32f4xx_wwdg.h

    )
    
    SET(STDPERIPH_SOURCES
        ${STDPERIPH_SOURCE_DIR}/misc.c
        ${STDPERIPH_SOURCE_DIR}/stm32f4xx_adc.c
        ${STDPERIPH_SOURCE_DIR}/stm32f4xx_can.c
        ${STDPERIPH_SOURCE_DIR}/stm32f4xx_crc.c
        ${STDPERIPH_SOURCE_DIR}/stm32f4xx_cryp.c
        ${STDPERIPH_SOURCE_DIR}/stm32f4xx_cryp_aes.c
        ${STDPERIPH_SOURCE_DIR}/stm32f4xx_cryp_des.c
        ${STDPERIPH_SOURCE_DIR}/stm32f4xx_cryp_tdes.c
        ${STDPERIPH_SOURCE_DIR}/stm32f4xx_dac.c
        ${STDPERIPH_SOURCE_DIR}/stm32f4xx_dbgmcu.c
        ${STDPERIPH_SOURCE_DIR}/stm32f4xx_dcmi.c
        ${STDPERIPH_SOURCE_DIR}/stm32f4xx_dma.c
        ${STDPERIPH_SOURCE_DIR}/stm32f4xx_dma2d.c
        ${STDPERIPH_SOURCE_DIR}/stm32f4xx_exti.c
        ${STDPERIPH_SOURCE_DIR}/stm32f4xx_flash.c
        ${STDPERIPH_SOURCE_DIR}/stm32f4xx_gpio.c
        ${STDPERIPH_SOURCE_DIR}/stm32f4xx_hash.c
        ${STDPERIPH_SOURCE_DIR}/stm32f4xx_hash_md5.c
        ${STDPERIPH_SOURCE_DIR}/stm32f4xx_hash_sha1.c
        ${STDPERIPH_SOURCE_DIR}/stm32f4xx_i2c.c
        ${STDPERIPH_SOURCE_DIR}/stm32f4xx_iwdg.c
        ${STDPERIPH_SOURCE_DIR}/stm32f4xx_ltdc.c
        ${STDPERIPH_SOURCE_DIR}/stm32f4xx_pwr.c
        ${STDPERIPH_SOURCE_DIR}/stm32f4xx_rcc.c
        ${STDPERIPH_SOURCE_DIR}/stm32f4xx_rng.c
        ${STDPERIPH_SOURCE_DIR}/stm32f4xx_rtc.c
        ${STDPERIPH_SOURCE_DIR}/stm32f4xx_sai.c
        ${STDPERIPH_SOURCE_DIR}/stm32f4xx_sdio.c
        ${STDPERIPH_SOURCE_DIR}/stm32f4xx_spi.c
        ${STDPERIPH_SOURCE_DIR}/stm32f4xx_syscfg.c
        ${STDPERIPH_SOURCE_DIR}/stm32f4xx_tim.c
        ${STDPERIPH_SOURCE_DIR}/stm32f4xx_usart.c
        ${STDPERIPH_SOURCE_DIR}/stm32f4xx_wwdg.c
    )
    
    SET(STDPERIPH_SOURCES_40_41xxx
        ${STDPERIPH_SOURCE_DIR}/stm32f4xx_fsmc.c
    )
    SET(STDPERIPH_SOURCES_427_437xx
        ${STDPERIPH_SOURCE_DIR}/stm32f4xx_fmc.c
    )
    SET(STDPERIPH_SOURCES_429_439xx
        ${STDPERIPH_SOURCE_DIR}/stm32f4xx_fmc.c
    )
        
    INSTALL(FILES ${STDPERIPH_HEADER_DIR}/misc.h RENAME stm32f4xx_misc.h DESTINATION include/stm32f4)
ENDIF()

INCLUDE_DIRECTORIES(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMSIS_INCLUDE_DIR}
)

IF(USE_ASSERT)
    ADD_DEFINITIONS("-D\"assert_param(expr)\"=\"((expr) ? (void)0 : assert_failed((uint8_t *)__FILE__, __LINE__))\"")
ELSE()
    ADD_DEFINITIONS("-D\"assert_param(expr)\"=\"((void)0)\"")
ENDIF()

STRING(TOLOWER ${STM32_FAMILY} STM32_FAMILY_LOWER)
ADD_CUSTOM_TARGET(stdperiph_${STM32_FAMILY_LOWER})

FOREACH(CHIP_TYPE ${STM32_CHIP_TYPES})
    STRING(TOLOWER ${CHIP_TYPE} CHIP_TYPE_LOWER)
    
    LIST(APPEND STDPERIPH_LIBRARIES stdperiph_${STM32_FAMILY_LOWER}_${CHIP_TYPE_LOWER})
    SET(TARGET_NAME stdperiph_${STM32_FAMILY_LOWER}_${CHIP_TYPE_LOWER})
    ADD_LIBRARY(${TARGET_NAME} ${STDPERIPH_SOURCES} ${STDPERIPH_SOURCES_${CHIP_TYPE}})
    STM32_SET_CHIP_DEFINITIONS(${TARGET_NAME} ${CHIP_TYPE})
    ADD_DEPENDENCIES(stdperiph_${STM32_FAMILY_LOWER} ${TARGET_NAME})
ENDFOREACH()

INSTALL(TARGETS ${STDPERIPH_LIBRARIES} RUNTIME DESTINATION bin LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)

INSTALL(FILES ${STDPERIPH_HEADERS} DESTINATION include/stm32${STM32_FAMILY_LOWER})
