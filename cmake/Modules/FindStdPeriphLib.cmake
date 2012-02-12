IF(NOT STM32_CHIP_TYPE)
    MESSAGE(FATAL_ERROR "Select your stm32 chip using STM32_CHIP_TYPE variable. (HD, HD_VL, MD, MD_VL, LD, LD_VL, XD, CL)")
ENDIF()

IF(STM32_CHIP_TYPE STREQUAL "HD")
    SET(StdPeriphLib_LIB_NAME stdperiph_hd)
ELSEIF(STM32_CHIP_TYPE STREQUAL "HD_VL")
    SET(StdPeriphLib_LIB_NAME stdperiph_hd_vl)
ELSEIF(STM32_CHIP_TYPE STREQUAL "MD")
    SET(StdPeriphLib_LIB_NAME stdperiph_md)
ELSEIF(STM32_CHIP_TYPE STREQUAL "MD_VL")
    SET(StdPeriphLib_LIB_NAME stdperiph_md_vl)
ELSEIF(STM32_CHIP_TYPE STREQUAL "LD")
    SET(StdPeriphLib_LIB_NAME stdperiph_ld)
ELSEIF(STM32_CHIP_TYPE STREQUAL "LD_VL")
    SET(StdPeriphLib_LIB_NAME stdperiph_ld_vl)
ELSEIF(STM32_CHIP_TYPE STREQUAL "XD")
    SET(StdPeriphLib_LIB_NAME stdperiph_xd)
ELSEIF(STM32_CHIP_TYPE STREQUAL "CL")
    SET(StdPeriphLib_LIB_NAME stdperiph_cl)
ELSE()
    MESSAGE(FATAL_ERROR "Invalid stm32 chip type.")
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

FIND_LIBRARY(StdPeriphLib_LIBRARIES
    NAMES ${StdPeriphLib_LIB_NAME}
    PATH_SUFFIXES lib
)

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DUSE_STDPERIPH_DRIVER")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_STDPERIPH_DRIVER")

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(StdPeriphLib DEFAULT_MSG StdPeriphLib_LIBRARIES StdPeriphLib_INCLUDE_DIR) 
