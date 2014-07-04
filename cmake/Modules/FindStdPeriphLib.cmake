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
    SET(STDPERIPH_SRCS
        misc.c
        stm32f10x_adc.c
        stm32f10x_bkp.c
        stm32f10x_can.c
        stm32f10x_cec.c
        stm32f10x_crc.c
        stm32f10x_dac.c
        stm32f10x_dbgmcu.c
        stm32f10x_dma.c
        stm32f10x_exti.c
        stm32f10x_flash.c
        stm32f10x_fsmc.c
        stm32f10x_gpio.c
        stm32f10x_i2c.c
        stm32f10x_iwdg.c
        stm32f10x_pwr.c
        stm32f10x_rcc.c
        stm32f10x_rtc.c
        stm32f10x_sdio.c
        stm32f10x_spi.c
        stm32f10x_tim.c
        stm32f10x_usart.c
        stm32f10x_wwdg.c
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
    
    SET(STDPERIPH_SRCS
        misc.c
        stm32f4xx_adc.c
        stm32f4xx_can.c
        stm32f4xx_crc.c
        stm32f4xx_cryp.c
        stm32f4xx_cryp_aes.c
        stm32f4xx_cryp_des.c
        stm32f4xx_cryp_tdes.c
        stm32f4xx_dac.c
        stm32f4xx_dbgmcu.c
        stm32f4xx_dcmi.c
        stm32f4xx_dma.c
        stm32f4xx_dma2d.c
        stm32f4xx_exti.c
        stm32f4xx_flash.c
        stm32f4xx_gpio.c
        stm32f4xx_hash.c
        stm32f4xx_hash_md5.c
        stm32f4xx_hash_sha1.c
        stm32f4xx_i2c.c
        stm32f4xx_iwdg.c
        stm32f4xx_ltdc.c
        stm32f4xx_pwr.c
        stm32f4xx_rcc.c
        stm32f4xx_rng.c
        stm32f4xx_rtc.c
        stm32f4xx_sai.c
        stm32f4xx_sdio.c
        stm32f4xx_spi.c
        stm32f4xx_syscfg.c
        stm32f4xx_tim.c
        stm32f4xx_usart.c
        stm32f4xx_wwdg.c
    )
    
    SET(STDPERIPH_SRCS_40_41xxx
        stm32f4xx_fsmc.c
    )
    SET(STDPERIPH_SRCS_427_437xx
        stm32f4xx_fmc.c
    )
    SET(STDPERIPH_SRCS_429_439xx
        stm32f4xx_fmc.c
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

STRING(TOLOWER ${STM32_FAMILY} STM32_FAMILY_LOWER)

FOREACH(STDPERIPH_SRC ${STDPERIPH_SRCS})
    SET(STDPERIPH_SRC_FILE STDPERIPH_SRC_FILE-NOTFOUND)
    FIND_FILE(STDPERIPH_SRC_FILE NAMES ${STDPERIPH_SRC} PATHS ${CMAKE_FIND_ROOT_PATH} PATH_SUFFIXES share/stdperiph/stm32${STM32_FAMILY_LOWER} stdperiph/stm32${STM32_FAMILY_LOWER})
    LIST(APPEND StdPeriphLib_SOURCES ${STDPERIPH_SRC_FILE})
ENDFOREACH()

IF(NOT STM32_CHIP_TYPE)
    FOREACH(CHIP_TYPE ${STM32_CHIP_TYPES})
        IF(STDPERIPH_SRCS_${CHIP_TYPE})
            FOREACH(STDPERIPH_SRC ${STDPERIPH_SRCS_${CHIP_TYPE}})
                SET(STDPERIPH_SRC_FILE STDPERIPH_SRC_FILE-NOTFOUND)
                FIND_FILE(STDPERIPH_SRC_FILE NAMES ${STDPERIPH_SRC} PATHS ${CMAKE_FIND_ROOT_PATH} PATH_SUFFIXES share/stdperiph/stm32${STM32_FAMILY_LOWER} stdperiph/stm32${STM32_FAMILY_LOWER})
                LIST(APPEND StdPeriphLib_SOURCES_${CHIP_TYPE} ${STDPERIPH_SRC_FILE})
                LIST(APPEND STDPERIPH_SRCSRC StdPeriphLib_SOURCES_${CHIP_TYPE})
            ENDFOREACH()
        ENDIF()
    ENDFOREACH()    
ELSE()
    IF(STDPERIPH_SRCS_${STM32_CHIP_TYPE})
        FOREACH(STDPERIPH_SRC ${STDPERIPH_SRCS_${STM32_CHIP_TYPE}})
            SET(STDPERIPH_SRC_FILE STDPERIPH_SRC_FILE-NOTFOUND)
            FIND_FILE(STDPERIPH_SRC_FILE NAMES ${STDPERIPH_SRC} PATHS ${CMAKE_FIND_ROOT_PATH} PATH_SUFFIXES share/stdperiph/stm32${STM32_FAMILY_LOWER} stdperiph/stm32${STM32_FAMILY_LOWER})
            LIST(APPEND StdPeriphLib_SOURCES ${STDPERIPH_SRC_FILE})
        ENDFOREACH()
    ENDIF()
ENDIF()

IF(STDPERIPH_USE_ASSERT)
    ADD_DEFINITIONS("-D\"assert_param(expr)\"=\"((expr) ? (void)0 : assert_failed((uint8_t *)__FILE__, __LINE__))\"")
ELSE()
    ADD_DEFINITIONS("-D\"assert_param(expr)\"=\"((void)0)\"")
ENDIF()

INCLUDE(FindPackageHandleStandardArgs)

IF(NOT STM32_CHIP_TYPE)
    FIND_PACKAGE_HANDLE_STANDARD_ARGS(StdPeriphLib DEFAULT_MSG StdPeriphLib_LIBRARIES StdPeriphLib_INCLUDE_DIR StdPeriphLib_SOURCES ${STDPERIPH_SRCSRC}) 
ELSE()
    FIND_PACKAGE_HANDLE_STANDARD_ARGS(StdPeriphLib DEFAULT_MSG StdPeriphLib_LIBRARIES StdPeriphLib_INCLUDE_DIR StdPeriphLib_SOURCES) 
ENDIF()

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DUSE_STDPERIPH_DRIVER")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_STDPERIPH_DRIVER")
