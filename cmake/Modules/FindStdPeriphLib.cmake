IF(STM32_FAMILY STREQUAL "F1")
    SET(STDPERIPH_HEADERS
        stm32f10x_adc.h
        stm32f10x_bkp.h
        stm32f10x_can.h
        stm32f10x_cec.h
        stm32f10x_crc.h
        stm32f10x_dac.h
        stm32f10x_dbgmcu.h
        stm32f10x_dma.h
        stm32f10x_exti.h
        stm32f10x_flash.h
        stm32f10x_fsmc.h
        stm32f10x_gpio.h
        stm32f10x_i2c.h
        stm32f10x_iwdg.h
        stm32f10x_pwr.h
        stm32f10x_rcc.h
        stm32f10x_rtc.h
        stm32f10x_sdio.h
        stm32f10x_spi.h
        stm32f10x_tim.h
        stm32f10x_usart.h
        stm32f10x_wwdg.h
        stm32f10x_misc.h
    )
ELSEIF(STM32_FAMILY STREQUAL "F4")
    SET(STDPERIPH_HEADERS
        stm32f4xx_adc.h
        stm32f4xx_can.h
        stm32f4xx_crc.h
        stm32f4xx_cryp.h
        stm32f4xx_dac.h
        stm32f4xx_dbgmcu.h
        stm32f4xx_dcmi.h
        stm32f4xx_dma.h
        stm32f4xx_dma2d.h
        stm32f4xx_exti.h
        stm32f4xx_flash.h
        stm32f4xx_fmc.h
        stm32f4xx_fsmc.h
        stm32f4xx_gpio.h
        stm32f4xx_hash.h
        stm32f4xx_i2c.h
        stm32f4xx_iwdg.h
        stm32f4xx_ltdc.h
        stm32f4xx_pwr.h
        stm32f4xx_rcc.h
        stm32f4xx_rng.h
        stm32f4xx_rtc.h
        stm32f4xx_sai.h
        stm32f4xx_sdio.h
        stm32f4xx_spi.h
        stm32f4xx_syscfg.h
        stm32f4xx_tim.h
        stm32f4xx_usart.h
        stm32f4xx_wwdg.h
    )
ENDIF()

STRING(TOLOWER ${STM32_FAMILY} STM32_FAMILY_LOWER)
IF(NOT STM32_CHIP_TYPE)
    FOREACH(CHIP_TYPE ${STM32_CHIP_TYPES})
        STRING(TOLOWER ${CHIP_TYPE} CHIP_TYPE_LOWER)
        LIST(APPEND STDPERIPH_FIND_LIBS stdperiph_${STM32_FAMILY_LOWER}_${CHIP_TYPE_LOWER})
    ENDFOREACH()    
ELSE()
    STRING(TOLOWER ${STM32_CHIP_TYPE} STM32_CHIP_TYPE_LOWER)
    SET(STDPERIPH_FIND_LIBS stdperiph_${STM32_FAMILY_LOWER}_${STM32_CHIP_TYPE_LOWER})
ENDIF()

FIND_PATH(StdPeriphLib_INCLUDE_DIR ${STDPERIPH_HEADERS}
    PATH_SUFFIXES include stm32${STM32_FAMILY_LOWER}
)

FOREACH(STDPERIPH_LIB_NAME ${STDPERIPH_FIND_LIBS})
    SET(StdPeriphLib_LIBRARY STDPERIPH_LIBRARY-NOTFOUND)
    FIND_LIBRARY(StdPeriphLib_LIBRARY
        NAMES ${STDPERIPH_LIB_NAME}
        PATH_SUFFIXES lib
    )
    LIST(APPEND StdPeriphLib_LIBRARIES ${StdPeriphLib_LIBRARY})
ENDFOREACH()

INCLUDE(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(StdPeriphLib DEFAULT_MSG StdPeriphLib_LIBRARIES StdPeriphLib_INCLUDE_DIR) 

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DUSE_STDPERIPH_DRIVER")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_STDPERIPH_DRIVER")
