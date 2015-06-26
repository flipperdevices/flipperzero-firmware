PROJECT(stm32hal)

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
        ${STDPERIPH_HEADER_DIR}/misc.h
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
    INSTALL(FILES ${STDPERIPH_SOURCES} DESTINATION share/stdperiph/stm32f1)
ELSEIF(${STM32_FAMILY} STREQUAL "F4")
    IF(NOT STM32Cube_DIR)
        SET(STM32Cube_DIR "/opt/STM32Cube_FW_F4_V1.6.0")
        MESSAGE(STATUS "No STM32Cube_DIR specified, using default: " ${STM32Cube_DIR})
    ENDIF()
    
    SET(HAL_HEADER_DIR ${STM32Cube_DIR}/Drivers/STM32F4xx_HAL_Driver/Inc)
    SET(HAL_SOURCE_DIR ${STM32Cube_DIR}/Drivers/STM32F4xx_HAL_Driver/Src)
    
    INCLUDE_DIRECTORIES(${HAL_HEADER_DIR})
    
    SET(HAL_HEADERS         
        ${HAL_HEADER_DIR}/stm32f4xx_hal.h
        ${HAL_HEADER_DIR}/stm32f4xx_hal_adc.h
        ${HAL_HEADER_DIR}/stm32f4xx_hal_adc_ex.h
        ${HAL_HEADER_DIR}/stm32f4xx_hal_can.h
        ${HAL_HEADER_DIR}/stm32f4xx_hal_cec.h
        ${HAL_HEADER_DIR}/stm32f4xx_hal_conf_template.h
        ${HAL_HEADER_DIR}/stm32f4xx_hal_cortex.h
        ${HAL_HEADER_DIR}/stm32f4xx_hal_crc.h
        ${HAL_HEADER_DIR}/stm32f4xx_hal_cryp.h
        ${HAL_HEADER_DIR}/stm32f4xx_hal_cryp_ex.h
        ${HAL_HEADER_DIR}/stm32f4xx_hal_dac.h
        ${HAL_HEADER_DIR}/stm32f4xx_hal_dac_ex.h
        ${HAL_HEADER_DIR}/stm32f4xx_hal_dcmi.h
        ${HAL_HEADER_DIR}/stm32f4xx_hal_dcmi_ex.h
        ${HAL_HEADER_DIR}/stm32f4xx_hal_def.h
        ${HAL_HEADER_DIR}/stm32f4xx_hal_dma.h
        ${HAL_HEADER_DIR}/stm32f4xx_hal_dma2d.h
        ${HAL_HEADER_DIR}/stm32f4xx_hal_dma_ex.h
        ${HAL_HEADER_DIR}/stm32f4xx_hal_eth.h
        ${HAL_HEADER_DIR}/stm32f4xx_hal_flash.h
        ${HAL_HEADER_DIR}/stm32f4xx_hal_flash_ex.h
        ${HAL_HEADER_DIR}/stm32f4xx_hal_flash_ramfunc.h
        ${HAL_HEADER_DIR}/stm32f4xx_hal_fmpi2c.h
        ${HAL_HEADER_DIR}/stm32f4xx_hal_fmpi2c_ex.h
        ${HAL_HEADER_DIR}/stm32f4xx_hal_gpio.h
        ${HAL_HEADER_DIR}/stm32f4xx_hal_gpio_ex.h
        ${HAL_HEADER_DIR}/stm32f4xx_hal_hash.h
        ${HAL_HEADER_DIR}/stm32f4xx_hal_hash_ex.h
        ${HAL_HEADER_DIR}/stm32f4xx_hal_hcd.h
        ${HAL_HEADER_DIR}/stm32f4xx_hal_i2c.h
        ${HAL_HEADER_DIR}/stm32f4xx_hal_i2c_ex.h
        ${HAL_HEADER_DIR}/stm32f4xx_hal_i2s.h
        ${HAL_HEADER_DIR}/stm32f4xx_hal_i2s_ex.h
        ${HAL_HEADER_DIR}/stm32f4xx_hal_irda.h
        ${HAL_HEADER_DIR}/stm32f4xx_hal_iwdg.h
        ${HAL_HEADER_DIR}/stm32f4xx_hal_ltdc.h
        ${HAL_HEADER_DIR}/stm32f4xx_hal_nand.h
        ${HAL_HEADER_DIR}/stm32f4xx_hal_nor.h
        ${HAL_HEADER_DIR}/stm32f4xx_hal_pccard.h
        ${HAL_HEADER_DIR}/stm32f4xx_hal_pcd.h
        ${HAL_HEADER_DIR}/stm32f4xx_hal_pcd_ex.h
        ${HAL_HEADER_DIR}/stm32f4xx_hal_pwr.h
        ${HAL_HEADER_DIR}/stm32f4xx_hal_pwr_ex.h
        ${HAL_HEADER_DIR}/stm32f4xx_hal_qspi.h
        ${HAL_HEADER_DIR}/stm32f4xx_hal_rcc.h
        ${HAL_HEADER_DIR}/stm32f4xx_hal_rcc_ex.h
        ${HAL_HEADER_DIR}/stm32f4xx_hal_rng.h
        ${HAL_HEADER_DIR}/stm32f4xx_hal_rtc.h
        ${HAL_HEADER_DIR}/stm32f4xx_hal_rtc_ex.h
        ${HAL_HEADER_DIR}/stm32f4xx_hal_sai.h
        ${HAL_HEADER_DIR}/stm32f4xx_hal_sai_ex.h
        ${HAL_HEADER_DIR}/stm32f4xx_hal_sd.h
        ${HAL_HEADER_DIR}/stm32f4xx_hal_sdram.h
        ${HAL_HEADER_DIR}/stm32f4xx_hal_smartcard.h
        ${HAL_HEADER_DIR}/stm32f4xx_hal_spdifrx.h
        ${HAL_HEADER_DIR}/stm32f4xx_hal_spi.h
        ${HAL_HEADER_DIR}/stm32f4xx_hal_sram.h
        ${HAL_HEADER_DIR}/stm32f4xx_hal_tim.h
        ${HAL_HEADER_DIR}/stm32f4xx_hal_tim_ex.h
        ${HAL_HEADER_DIR}/stm32f4xx_hal_uart.h
        ${HAL_HEADER_DIR}/stm32f4xx_hal_usart.h
        ${HAL_HEADER_DIR}/stm32f4xx_hal_wwdg.h
        ${HAL_HEADER_DIR}/stm32f4xx_ll_fmc.h
        ${HAL_HEADER_DIR}/stm32f4xx_ll_fsmc.h
        ${HAL_HEADER_DIR}/stm32f4xx_ll_sdmmc.h
        ${HAL_HEADER_DIR}/stm32f4xx_ll_usb.h
    )
    
    SET(HAL_SOURCES
        ${HAL_SOURCE_DIR}/stm32f4xx_hal.c
        ${HAL_SOURCE_DIR}/stm32f4xx_hal_adc.c
        ${HAL_SOURCE_DIR}/stm32f4xx_hal_adc_ex.c
        ${HAL_SOURCE_DIR}/stm32f4xx_hal_can.c
        ${HAL_SOURCE_DIR}/stm32f4xx_hal_cec.c
        ${HAL_SOURCE_DIR}/stm32f4xx_hal_cortex.c
        ${HAL_SOURCE_DIR}/stm32f4xx_hal_crc.c
        ${HAL_SOURCE_DIR}/stm32f4xx_hal_cryp.c
        ${HAL_SOURCE_DIR}/stm32f4xx_hal_cryp_ex.c
        ${HAL_SOURCE_DIR}/stm32f4xx_hal_dac.c
        ${HAL_SOURCE_DIR}/stm32f4xx_hal_dac_ex.c
        ${HAL_SOURCE_DIR}/stm32f4xx_hal_dcmi.c
        ${HAL_SOURCE_DIR}/stm32f4xx_hal_dcmi_ex.c
        ${HAL_SOURCE_DIR}/stm32f4xx_hal_dma.c
        ${HAL_SOURCE_DIR}/stm32f4xx_hal_dma2d.c
        ${HAL_SOURCE_DIR}/stm32f4xx_hal_dma_ex.c
        ${HAL_SOURCE_DIR}/stm32f4xx_hal_eth.c
        ${HAL_SOURCE_DIR}/stm32f4xx_hal_flash.c
        ${HAL_SOURCE_DIR}/stm32f4xx_hal_flash_ex.c
        ${HAL_SOURCE_DIR}/stm32f4xx_hal_flash_ramfunc.c
        ${HAL_SOURCE_DIR}/stm32f4xx_hal_fmpi2c.c
        ${HAL_SOURCE_DIR}/stm32f4xx_hal_fmpi2c_ex.c
        ${HAL_SOURCE_DIR}/stm32f4xx_hal_gpio.c
        ${HAL_SOURCE_DIR}/stm32f4xx_hal_hash.c
        ${HAL_SOURCE_DIR}/stm32f4xx_hal_hash_ex.c
        ${HAL_SOURCE_DIR}/stm32f4xx_hal_hcd.c
        ${HAL_SOURCE_DIR}/stm32f4xx_hal_i2c.c
        ${HAL_SOURCE_DIR}/stm32f4xx_hal_i2c_ex.c
        ${HAL_SOURCE_DIR}/stm32f4xx_hal_i2s.c
        ${HAL_SOURCE_DIR}/stm32f4xx_hal_i2s_ex.c
        ${HAL_SOURCE_DIR}/stm32f4xx_hal_irda.c
        ${HAL_SOURCE_DIR}/stm32f4xx_hal_iwdg.c
        ${HAL_SOURCE_DIR}/stm32f4xx_hal_ltdc.c
        ${HAL_SOURCE_DIR}/stm32f4xx_hal_msp_template.c
        ${HAL_SOURCE_DIR}/stm32f4xx_hal_nand.c
        ${HAL_SOURCE_DIR}/stm32f4xx_hal_nor.c
        ${HAL_SOURCE_DIR}/stm32f4xx_hal_pccard.c
        ${HAL_SOURCE_DIR}/stm32f4xx_hal_pcd.c
        ${HAL_SOURCE_DIR}/stm32f4xx_hal_pcd_ex.c
        ${HAL_SOURCE_DIR}/stm32f4xx_hal_pwr.c
        ${HAL_SOURCE_DIR}/stm32f4xx_hal_pwr_ex.c
        ${HAL_SOURCE_DIR}/stm32f4xx_hal_qspi.c
        ${HAL_SOURCE_DIR}/stm32f4xx_hal_rcc.c
        ${HAL_SOURCE_DIR}/stm32f4xx_hal_rcc_ex.c
        ${HAL_SOURCE_DIR}/stm32f4xx_hal_rng.c
        ${HAL_SOURCE_DIR}/stm32f4xx_hal_rtc.c
        ${HAL_SOURCE_DIR}/stm32f4xx_hal_rtc_ex.c
        ${HAL_SOURCE_DIR}/stm32f4xx_hal_sai.c
        ${HAL_SOURCE_DIR}/stm32f4xx_hal_sai_ex.c
        ${HAL_SOURCE_DIR}/stm32f4xx_hal_sd.c
        ${HAL_SOURCE_DIR}/stm32f4xx_hal_sdram.c
        ${HAL_SOURCE_DIR}/stm32f4xx_hal_smartcard.c
        ${HAL_SOURCE_DIR}/stm32f4xx_hal_spdifrx.c
        ${HAL_SOURCE_DIR}/stm32f4xx_hal_spi.c
        ${HAL_SOURCE_DIR}/stm32f4xx_hal_sram.c
        ${HAL_SOURCE_DIR}/stm32f4xx_hal_tim.c
        ${HAL_SOURCE_DIR}/stm32f4xx_hal_tim_ex.c
        ${HAL_SOURCE_DIR}/stm32f4xx_hal_uart.c
        ${HAL_SOURCE_DIR}/stm32f4xx_hal_usart.c
        ${HAL_SOURCE_DIR}/stm32f4xx_hal_wwdg.c
        ${HAL_SOURCE_DIR}/stm32f4xx_ll_fmc.c
        ${HAL_SOURCE_DIR}/stm32f4xx_ll_fsmc.c
        ${HAL_SOURCE_DIR}/stm32f4xx_ll_sdmmc.c
        ${HAL_SOURCE_DIR}/stm32f4xx_ll_usb.c
    )
    
    INSTALL(FILES ${HAL_HEADER_DIR}/Legacy/stm32_hal_legacy.h DESTINATION include/stm32f4/Legacy)
    
#     SET(STDPERIPH_SOURCES_40_41xxx
#         ${STDPERIPH_SOURCE_DIR}/stm32f4xx_fsmc.c
#     )
#     SET(STDPERIPH_SOURCES_427_437xx
#         ${STDPERIPH_SOURCE_DIR}/stm32f4xx_fmc.c
#     )
#     SET(STDPERIPH_SOURCES_429_439xx
#         ${STDPERIPH_SOURCE_DIR}/stm32f4xx_fmc.c
#     )
ENDIF()

STRING(TOLOWER ${STM32_FAMILY} STM32_FAMILY_LOWER)

INSTALL(FILES ${HAL_HEADERS} DESTINATION include/stm32${STM32_FAMILY_LOWER})
INSTALL(FILES ${HAL_SOURCES} DESTINATION share/stm32hal/${STM32_FAMILY_LOWER})
