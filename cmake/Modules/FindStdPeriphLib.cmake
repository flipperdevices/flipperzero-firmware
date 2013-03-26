SET(StdPeriphLib_LIB_NAME_HD "stdperiph_hd")
SET(StdPeriphLib_LIB_NAME_HD_VL "stdperiph_hd_vl")
SET(StdPeriphLib_LIB_NAME_MD "stdperiph_md")
SET(StdPeriphLib_LIB_NAME_MD_VL "stdperiph_md_vl")
SET(StdPeriphLib_LIB_NAME_LD "stdperiph_ld")
SET(StdPeriphLib_LIB_NAME_LD_VL "stdperiph_ld_vl")
SET(StdPeriphLib_LIB_NAME_CL "stdperiph_cl")
SET(StdPeriphLib_LIB_NAME_XL "stdperiph_xl")

IF(NOT STM32_CHIP_TYPE)
    SET(StdPeriphLib_FIND_LIBS ${StdPeriphLib_LIB_NAME_HD} ${StdPeriphLib_LIB_NAME_HD_VL} ${StdPeriphLib_LIB_NAME_MD} ${StdPeriphLib_LIB_NAME_MD_VL} ${StdPeriphLib_LIB_NAME_LD} ${StdPeriphLib_LIB_NAME_LD_VL} ${StdPeriphLib_LIB_NAME_CL} ${StdPeriphLib_LIB_NAME_XL})
ELSE()
    IF(STM32_CHIP_TYPE STREQUAL "HD")
        SET(StdPeriphLib_FIND_LIBS ${StdPeriphLib_LIB_NAME_HD})
    ELSEIF(STM32_CHIP_TYPE STREQUAL "HD_VL")
        SET(StdPeriphLib_FIND_LIBS ${StdPeriphLib_LIB_NAME_HD_VL})
    ELSEIF(STM32_CHIP_TYPE STREQUAL "MD")
        SET(StdPeriphLib_FIND_LIBS ${StdPeriphLib_LIB_NAME_MD})
    ELSEIF(STM32_CHIP_TYPE STREQUAL "MD_VL")
        SET(StdPeriphLib_FIND_LIBS ${StdPeriphLib_LIB_NAME_MD_VL})
    ELSEIF(STM32_CHIP_TYPE STREQUAL "LD")
        SET(StdPeriphLib_FIND_LIBS ${StdPeriphLib_LIB_NAME_LD})
    ELSEIF(STM32_CHIP_TYPE STREQUAL "LD_VL")
        SET(StdPeriphLib_FIND_LIBS ${StdPeriphLib_LIB_NAME_LD_VL})
    ELSEIF(STM32_CHIP_TYPE STREQUAL "XL")
        SET(StdPeriphLib_FIND_LIBS ${StdPeriphLib_LIB_NAME_XL})
    ELSEIF(STM32_CHIP_TYPE STREQUAL "CL")
        SET(StdPeriphLib_FIND_LIBS ${StdPeriphLib_LIB_NAME_CL})
    ELSE()
        MESSAGE(FATAL_ERROR "Invalid stm32 chip type.")
    ENDIF()
ENDIF()

FIND_PATH(StdPeriphLib_INCLUDE_DIR 
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
    PATH_SUFFIXES include stm32
)

FOREACH(StdPeriphLib_LIB_NAME ${StdPeriphLib_FIND_LIBS})
    SET(StdPeriphLib_LIBRARY StdPeriphLib_LIBRARY-NOTFOUND)
    FIND_LIBRARY(StdPeriphLib_LIBRARY
        NAMES ${StdPeriphLib_LIB_NAME}
        PATH_SUFFIXES lib
    )
    SET(StdPeriphLib_LIBRARIES ${StdPeriphLib_LIBRARIES} ${StdPeriphLib_LIBRARY})
ENDFOREACH()

INCLUDE(FindPackageHandleStandardArgs)
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DUSE_STDPERIPH_DRIVER")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_STDPERIPH_DRIVER")
FIND_PACKAGE_HANDLE_STANDARD_ARGS(StdPeriphLib DEFAULT_MSG StdPeriphLib_LIBRARIES StdPeriphLib_INCLUDE_DIR) 
