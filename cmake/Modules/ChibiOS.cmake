IF(NOT CHIBIOS_ROOT)
    SET(CHIBIOS_ROOT /usr/src/chibios)
    MESSAGE(STATUS "No CHIBIOS_ROOT specified, using default: ${CHIBIOS_ROOT}")
ENDIF()

IF(STM32_FAMILY STREQUAL "F4")
    SET(CHIBIOS_HAL_PLATFORM_SOURCES
        ${CHIBIOS_ROOT}/os/hal/platforms/STM32F4xx/stm32_dma.c
        ${CHIBIOS_ROOT}/os/hal/platforms/STM32F4xx/hal_lld.c
        ${CHIBIOS_ROOT}/os/hal/platforms/STM32F4xx/adc_lld.c
        ${CHIBIOS_ROOT}/os/hal/platforms/STM32F4xx/ext_lld_isr.c
        ${CHIBIOS_ROOT}/os/hal/platforms/STM32/GPIOv2/pal_lld.c
        ${CHIBIOS_ROOT}/os/hal/platforms/STM32/I2Cv1/i2c_lld.c
        ${CHIBIOS_ROOT}/os/hal/platforms/STM32/OTGv1/usb_lld.c
        ${CHIBIOS_ROOT}/os/hal/platforms/STM32/RTCv2/rtc_lld.c
        ${CHIBIOS_ROOT}/os/hal/platforms/STM32/SPIv1/spi_lld.c
        ${CHIBIOS_ROOT}/os/hal/platforms/STM32/TIMv1/gpt_lld.c
        ${CHIBIOS_ROOT}/os/hal/platforms/STM32/TIMv1/icu_lld.c
        ${CHIBIOS_ROOT}/os/hal/platforms/STM32/TIMv1/pwm_lld.c
        ${CHIBIOS_ROOT}/os/hal/platforms/STM32/USARTv1/serial_lld.c
        ${CHIBIOS_ROOT}/os/hal/platforms/STM32/USARTv1/uart_lld.c
    )
    SET(CHIBIOS_KERNEL_PLATFORM_SOURCES
        ${CHIBIOS_ROOT}/os/ports/GCC/ARMCMx/STM32F4xx/vectors.c
    )  
    SET(CHIBIOS_HAL_PLATFORM_INCLUDE_DIRS
        ${CHIBIOS_ROOT}/os/hal/platforms/STM32F4xx/
        ${CHIBIOS_ROOT}/os/hal/platforms/STM32/
        ${CHIBIOS_ROOT}/os/hal/platforms/STM32/GPIOv2
        ${CHIBIOS_ROOT}/os/hal/platforms/STM32/I2Cv1
        ${CHIBIOS_ROOT}/os/hal/platforms/STM32/OTGv1
        ${CHIBIOS_ROOT}/os/hal/platforms/STM32/RTCv2
        ${CHIBIOS_ROOT}/os/hal/platforms/STM32/SPIv1
        ${CHIBIOS_ROOT}/os/hal/platforms/STM32/TIMv1
        ${CHIBIOS_ROOT}/os/hal/platforms/STM32/USARTv1
    )
    SET(CHIBIOS_KERNEL_PLATFORM_INCLUDE_DIRS
        ${CHIBIOS_ROOT}/os/ports/GCC/ARMCMx/STM32F4xx
    )
ENDIF()

SET(CHIBIOS_HAL_PLATFORM_SOURCES ${CHIBIOS_HAL_PLATFORM_SOURCES}
    ${CHIBIOS_ROOT}/os/hal/platforms/STM32/can_lld.c
    ${CHIBIOS_ROOT}/os/hal/platforms/STM32/ext_lld.c
    ${CHIBIOS_ROOT}/os/hal/platforms/STM32/mac_lld.c
    ${CHIBIOS_ROOT}/os/hal/platforms/STM32/sdc_lld.c
)  

SET(CHIBIOS_KERNEL_PLATFORM_SOURCES ${CHIBIOS_KERNEL_PLATFORM_SOURCES}
    ${CHIBIOS_ROOT}/os/ports/GCC/ARMCMx/crt0.c
    ${CHIBIOS_ROOT}/os/ports/GCC/ARMCMx/chcore.c
    ${CHIBIOS_ROOT}/os/ports/GCC/ARMCMx/chcore_v7m.c
    ${CHIBIOS_ROOT}/os/ports/common/ARMCMx/nvic.c
)

SET(CHIBIOS_KERNEL_PLATFORM_INCLUDE_DIRS ${CHIBIOS_KERNEL_PLATFORM_INCLUDE_DIRS}
    ${CHIBIOS_ROOT}/os/ports/common/ARMCMx
    ${CHIBIOS_ROOT}/os/ports/GCC/ARMCMx
)

SET(CHIBIOS_HAL_INCLUDE_DIRS
    ${CHIBIOS_ROOT}/os/hal/include
)

SET(CHIBIOS_HAL_SOURCES
    ${CHIBIOS_ROOT}/os/hal/src/hal.c
    ${CHIBIOS_ROOT}/os/hal/src/adc.c
    ${CHIBIOS_ROOT}/os/hal/src/can.c
    ${CHIBIOS_ROOT}/os/hal/src/ext.c
    ${CHIBIOS_ROOT}/os/hal/src/gpt.c
    ${CHIBIOS_ROOT}/os/hal/src/i2c.c
    ${CHIBIOS_ROOT}/os/hal/src/icu.c
    ${CHIBIOS_ROOT}/os/hal/src/mac.c
    ${CHIBIOS_ROOT}/os/hal/src/mmc_spi.c
    ${CHIBIOS_ROOT}/os/hal/src/mmcsd.c
    ${CHIBIOS_ROOT}/os/hal/src/pal.c
    ${CHIBIOS_ROOT}/os/hal/src/pwm.c
    ${CHIBIOS_ROOT}/os/hal/src/rtc.c
    ${CHIBIOS_ROOT}/os/hal/src/sdc.c
    ${CHIBIOS_ROOT}/os/hal/src/serial.c
    ${CHIBIOS_ROOT}/os/hal/src/serial_usb.c
    ${CHIBIOS_ROOT}/os/hal/src/spi.c
    ${CHIBIOS_ROOT}/os/hal/src/tm.c
    ${CHIBIOS_ROOT}/os/hal/src/uart.c
    ${CHIBIOS_ROOT}/os/hal/src/usb.c
)

SET(CHIBIOS_KERNEL_INCLUDE_DIRS
    ${CHIBIOS_ROOT}/os/kernel/include
)
  
SET(CHIBIOS_KERNEL_SOURCES  
    ${CHIBIOS_ROOT}/os/kernel/src/chsys.c
    ${CHIBIOS_ROOT}/os/kernel/src/chdebug.c
    ${CHIBIOS_ROOT}/os/kernel/src/chlists.c
    ${CHIBIOS_ROOT}/os/kernel/src/chvt.c
    ${CHIBIOS_ROOT}/os/kernel/src/chschd.c
    ${CHIBIOS_ROOT}/os/kernel/src/chthreads.c
    ${CHIBIOS_ROOT}/os/kernel/src/chdynamic.c
    ${CHIBIOS_ROOT}/os/kernel/src/chregistry.c
    ${CHIBIOS_ROOT}/os/kernel/src/chsem.c
    ${CHIBIOS_ROOT}/os/kernel/src/chmtx.c
    ${CHIBIOS_ROOT}/os/kernel/src/chcond.c
    ${CHIBIOS_ROOT}/os/kernel/src/chevents.c
    ${CHIBIOS_ROOT}/os/kernel/src/chmsg.c
    ${CHIBIOS_ROOT}/os/kernel/src/chmboxes.c
    ${CHIBIOS_ROOT}/os/kernel/src/chqueues.c
    ${CHIBIOS_ROOT}/os/kernel/src/chmemcore.c
    ${CHIBIOS_ROOT}/os/kernel/src/chheap.c
    ${CHIBIOS_ROOT}/os/kernel/src/chmempools.c
)

SET(CHIBIOS_VARIOUS_INCLUDE_DIRS
    ${CHIBIOS_ROOT}/os/various/
)

SET(CHIBIOS_SOURCES ${CHIBIOS_KERNEL_PLATFORM_SOURCES} ${CHIBIOS_KERNEL_SOURCES} ${CHIBIOS_HAL_PLATFORM_SOURCES} ${CHIBIOS_HAL_SOURCES})
SET(CHIBIOS_INCLUDE_DIRS ${CHIBIOS_KERNEL_PLATFORM_INCLUDE_DIRS} ${CHIBIOS_KERNEL_INCLUDE_DIRS} ${CHIBIOS_HAL_PLATFORM_INCLUDE_DIRS} ${CHIBIOS_HAL_INCLUDE_DIRS} ${CHIBIOS_VARIOUS_INCLUDE_DIRS})