PROJECT(stm32stdperiph)

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
ENABLE_LANGUAGE(ASM)

FIND_PACKAGE(CMSIS REQUIRED)

IF(NOT STM32_StdPeriphLib_DIR)
    SET(STM32_StdPeriphLib_DIR "/opt/STM32F10x_StdPeriph_Lib_V3.5.0")
    MESSAGE(STATUS "No STM32_StdPeriphLib_DIR specified, using default: " ${STM32_StdPeriphLib_DIR})
ENDIF()

INCLUDE_DIRECTORIES(
    ${STM32_StdPeriphLib_DIR}/Libraries/STM32F10x_StdPeriph_Driver/inc/
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMSIS_INCLUDE_DIR}
)

SET(LIB_SOURCES
    ${STM32_StdPeriphLib_DIR}/Libraries/STM32F10x_StdPeriph_Driver/src/misc.c
    ${STM32_StdPeriphLib_DIR}/Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_adc.c
    ${STM32_StdPeriphLib_DIR}/Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_bkp.c
    ${STM32_StdPeriphLib_DIR}/Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_can.c
    ${STM32_StdPeriphLib_DIR}/Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_cec.c
    ${STM32_StdPeriphLib_DIR}/Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_crc.c
    ${STM32_StdPeriphLib_DIR}/Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_dac.c
    ${STM32_StdPeriphLib_DIR}/Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_dbgmcu.c
    ${STM32_StdPeriphLib_DIR}/Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_dma.c
    ${STM32_StdPeriphLib_DIR}/Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_exti.c
    ${STM32_StdPeriphLib_DIR}/Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_flash.c
    ${STM32_StdPeriphLib_DIR}/Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_fsmc.c
    ${STM32_StdPeriphLib_DIR}/Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_gpio.c
    ${STM32_StdPeriphLib_DIR}/Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_i2c.c
    ${STM32_StdPeriphLib_DIR}/Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_iwdg.c
    ${STM32_StdPeriphLib_DIR}/Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_pwr.c
    ${STM32_StdPeriphLib_DIR}/Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_rcc.c
    ${STM32_StdPeriphLib_DIR}/Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_rtc.c
    ${STM32_StdPeriphLib_DIR}/Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_sdio.c
    ${STM32_StdPeriphLib_DIR}/Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_spi.c
    ${STM32_StdPeriphLib_DIR}/Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_tim.c
    ${STM32_StdPeriphLib_DIR}/Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_usart.c
    ${STM32_StdPeriphLib_DIR}/Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_wwdg.c
)

IF(USE_ASSERT)
    ADD_DEFINITIONS("-D\"assert_param(expr)\"=\"((expr) ? (void)0 : assert_failed((uint8_t *)__FILE__, __LINE__))\"")
ELSE()
    ADD_DEFINITIONS("-D\"assert_param(expr)\"=\"((void)0)\"")
ENDIF()

ADD_LIBRARY(stdperiph_cl ${LIB_SOURCES})
SET_TARGET_PROPERTIES(stdperiph_cl PROPERTIES COMPILE_DEFINITIONS ${STM32_CHIP_DEF_CL})

ADD_LIBRARY(stdperiph_hd ${LIB_SOURCES})
SET_TARGET_PROPERTIES(stdperiph_hd PROPERTIES COMPILE_DEFINITIONS ${STM32_CHIP_DEF_HD})

ADD_LIBRARY(stdperiph_hd_vl ${LIB_SOURCES})
SET_TARGET_PROPERTIES(stdperiph_hd_vl PROPERTIES COMPILE_DEFINITIONS ${STM32_CHIP_DEF_HD_VL})

ADD_LIBRARY(stdperiph_ld ${LIB_SOURCES})
SET_TARGET_PROPERTIES(stdperiph_ld PROPERTIES COMPILE_DEFINITIONS ${STM32_CHIP_DEF_LD})

ADD_LIBRARY(stdperiph_ld_vl ${LIB_SOURCES})
SET_TARGET_PROPERTIES(stdperiph_ld_vl PROPERTIES COMPILE_DEFINITIONS ${STM32_CHIP_DEF_LD_VL})

ADD_LIBRARY(stdperiph_md ${LIB_SOURCES})
SET_TARGET_PROPERTIES(stdperiph_md PROPERTIES COMPILE_DEFINITIONS ${STM32_CHIP_DEF_MD})

ADD_LIBRARY(stdperiph_md_vl ${LIB_SOURCES})
SET_TARGET_PROPERTIES(stdperiph_md_vl PROPERTIES COMPILE_DEFINITIONS ${STM32_CHIP_DEF_MD_VL})

ADD_LIBRARY(stdperiph_xl ${LIB_SOURCES})
SET_TARGET_PROPERTIES(stdperiph_xl PROPERTIES COMPILE_DEFINITIONS ${STM32_CHIP_DEF_XL})

INSTALL(TARGETS stdperiph_cl stdperiph_hd stdperiph_hd_vl stdperiph_ld stdperiph_ld_vl stdperiph_md stdperiph_md_vl stdperiph_xl
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

INSTALL(FILES 
    ${STM32_StdPeriphLib_DIR}/Libraries/STM32F10x_StdPeriph_Driver/inc/stm32f10x_adc.h
    ${STM32_StdPeriphLib_DIR}/Libraries/STM32F10x_StdPeriph_Driver/inc/stm32f10x_bkp.h
    ${STM32_StdPeriphLib_DIR}/Libraries/STM32F10x_StdPeriph_Driver/inc/stm32f10x_can.h
    ${STM32_StdPeriphLib_DIR}/Libraries/STM32F10x_StdPeriph_Driver/inc/stm32f10x_cec.h
    ${STM32_StdPeriphLib_DIR}/Libraries/STM32F10x_StdPeriph_Driver/inc/stm32f10x_crc.h
    ${STM32_StdPeriphLib_DIR}/Libraries/STM32F10x_StdPeriph_Driver/inc/stm32f10x_dac.h
    ${STM32_StdPeriphLib_DIR}/Libraries/STM32F10x_StdPeriph_Driver/inc/stm32f10x_dbgmcu.h
    ${STM32_StdPeriphLib_DIR}/Libraries/STM32F10x_StdPeriph_Driver/inc/stm32f10x_dma.h
    ${STM32_StdPeriphLib_DIR}/Libraries/STM32F10x_StdPeriph_Driver/inc/stm32f10x_exti.h
    ${STM32_StdPeriphLib_DIR}/Libraries/STM32F10x_StdPeriph_Driver/inc/stm32f10x_flash.h
    ${STM32_StdPeriphLib_DIR}/Libraries/STM32F10x_StdPeriph_Driver/inc/stm32f10x_fsmc.h
    ${STM32_StdPeriphLib_DIR}/Libraries/STM32F10x_StdPeriph_Driver/inc/stm32f10x_gpio.h
    ${STM32_StdPeriphLib_DIR}/Libraries/STM32F10x_StdPeriph_Driver/inc/stm32f10x_i2c.h
    ${STM32_StdPeriphLib_DIR}/Libraries/STM32F10x_StdPeriph_Driver/inc/stm32f10x_iwdg.h
    ${STM32_StdPeriphLib_DIR}/Libraries/STM32F10x_StdPeriph_Driver/inc/stm32f10x_pwr.h
    ${STM32_StdPeriphLib_DIR}/Libraries/STM32F10x_StdPeriph_Driver/inc/stm32f10x_rcc.h
    ${STM32_StdPeriphLib_DIR}/Libraries/STM32F10x_StdPeriph_Driver/inc/stm32f10x_rtc.h
    ${STM32_StdPeriphLib_DIR}/Libraries/STM32F10x_StdPeriph_Driver/inc/stm32f10x_sdio.h
    ${STM32_StdPeriphLib_DIR}/Libraries/STM32F10x_StdPeriph_Driver/inc/stm32f10x_spi.h
    ${STM32_StdPeriphLib_DIR}/Libraries/STM32F10x_StdPeriph_Driver/inc/stm32f10x_tim.h
    ${STM32_StdPeriphLib_DIR}/Libraries/STM32F10x_StdPeriph_Driver/inc/stm32f10x_usart.h
    ${STM32_StdPeriphLib_DIR}/Libraries/STM32F10x_StdPeriph_Driver/inc/stm32f10x_wwdg.h
    DESTINATION
    include
)

INSTALL(FILES 
    ${STM32_StdPeriphLib_DIR}/Libraries/STM32F10x_StdPeriph_Driver/inc/misc.h
    RENAME
    stm32f10x_misc.h
    DESTINATION
    include
)