SET(CHIBIOS_HAL_LIB_MODULES chprintf memstreams nullstreams)
SET(CHIBIOS_HAL_MODULES adc buffers can channels dac ext files gpt i2c i2s icu ioblock mac mii mmc_spi mmcsd pal pwm qspi queues rtc sdc serial serial_usb spi st streams uart usb_cdc usb wdg)

IF(${CHIBIOS_KERNEL} STREQUAL nil)
  SET(CHIBIOS_OSAL_PATH ${CHIBIOS_ROOT}/os/hal/osal/nil)
ELSE()
  SET(CHIBIOS_OSAL_PATH ${CHIBIOS_ROOT}/os/hal/osal/rt)
ENDIF()

SET(CHIBIOS_hal_SEARCH_PATH 
  ${CHIBIOS_ROOT}/os/hal/include 
  ${CHIBIOS_ROOT}/os/hal/src/
  ${CHIBIOS_ROOT}/os/hal/osal/lib
  ${CHIBIOS_OSAL_PATH}
)
SET(CHIBIOS_hal_SEARCH_HEADERS 
  hal.h 
  osal.h
)
SET(CHIBIOS_hal_SOURCES 
  hal.c 
  osal.c
)

FOREACH(module ${CHIBIOS_HAL_MODULES})
  SET(CHIBIOS_${module}_SEARCH_PATH ${CHIBIOS_ROOT}/os/hal/include ${CHIBIOS_ROOT}/os/hal/src)
  SET(CHIBIOS_${module}_SOURCES hal_${module}.c)
  SET(CHIBIOS_${module}_SEARCH_HEADERS hal_${module}.h)
ENDFOREACH()

IF(STM32_FAMILY STREQUAL "F0")
    SET(CHIBIOS_HAL_PLATFORM_MODULES adc can dac ext gpt i2c i2s icu mac pal pwm rtc sdc serial spi st uart usb wdg)
    SET(CHIBIOS_HAL_PLATFORM_MODULES_PATHES
            LLD/ADCv1
            LLD/CANv1
            LLD/DACv1
            LLD/EXTIv1
            LLD/TIMv1
            LLD/I2Cv2
            LLD/SPIv1
            LLD/TIMv1
            LLD/MACv1
            LLD/GPIOv2
            LLD/TIMv1
            LLD/RTCv2
            LLD/SDMMCv1
            LLD/USARTv2
            LLD/SPIv2
            LLD/TIMv1
            LLD/USARTv2
            LLD/USBv1
            LDD/DMAv1
            LDD/xWDGv1
            )

    SET(CHIBIOS_hal_PLATFORM_SEARCH_PATH
            ${CHIBIOS_ROOT}/os/hal/ports/common/ARMCMx
            ${CHIBIOS_ROOT}/os/hal/ports/STM32/STM32F0xx
            ${CHIBIOS_ROOT}/os/hal/ports/STM32/LLD/DMAv1
            )
    SET(CHIBIOS_hal_PLATFORM_SEARCH_HEADERS
            hal_lld.h
            stm32_isr.h
            stm32_rcc.h
            stm32_registry.h
            nvic.h
            stm32_dma.h
            )
    SET(CHIBIOS_hal_PLATFORM_SOURCES
            hal_lld.c
            nvic.c
            stm32_dma.c
            )
    SET(INDEX 0)
    FOREACH(module ${CHIBIOS_HAL_PLATFORM_MODULES})
        LIST(GET CHIBIOS_HAL_PLATFORM_MODULES_PATHES ${INDEX} path)
        SET(CHIBIOS_${module}_PLATFORM_SEARCH_PATH ${CHIBIOS_ROOT}/os/hal/ports/STM32/${path})
        SET(CHIBIOS_${module}_PLATFORM_SEARCH_HEADERS hal_${module}_lld.h)
        SET(CHIBIOS_${module}_PLATFORM_SOURCES hal_${module}_lld.c)

        IF(${module} STREQUAL ext)
            SET(CHIBIOS_${module}_PLATFORM_SEARCH_PATH ${CHIBIOS_ext_PLATFORM_SEARCH_PATH} ${CHIBIOS_ROOT}/os/hal/ports/STM32/STM32F0xx)
            SET(CHIBIOS_${module}_PLATFORM_SEARCH_HEADERS ${CHIBIOS_ext_PLATFORM_SEARCH_HEADERS} hal_ext_lld_isr.h)
            SET(CHIBIOS_${module}_PLATFORM_SOURCES ${CHIBIOS_ext_PLATFORM_SOURCES} hal_ext_lld_isr.c)
        ENDIF()

        MATH(EXPR INDEX "${INDEX} + 1")
    ENDFOREACH()
ELSEIF(STM32_FAMILY STREQUAL "F1")
    SET(CHIBIOS_HAL_PLATFORM_MODULES adc can dac ext gpt i2c icu pal pwm rtc sdc serial spi st uart usb)
    SET(CHIBIOS_HAL_PLATFORM_MODULES_PATHES
      STM32F1xx
      LLD/CANv1
      LLD/DACv1
      LLD/EXTIv1
      LLD/TIMv1
      LLD/I2Cv1
      LLD/TIMv1
      LLD/GPIOv1
      LLD/TIMv1
      LLD/RTCv1
      LLD/SDIOv1
      LLD/USARTv1
      LLD/SPIv1
      LLD/TIMv1
      LLD/USARTv1
      LLD/USBv1
    )

    SET(CHIBIOS_hal_PLATFORM_SEARCH_PATH
        ${CHIBIOS_ROOT}/os/hal/ports/common/ARMCMx
        ${CHIBIOS_ROOT}/os/hal/ports/STM32/STM32F1xx
        ${CHIBIOS_ROOT}/os/hal/ports/STM32/LLD/DMAv1
    )
    SET(CHIBIOS_hal_PLATFORM_SEARCH_HEADERS
        hal_lld.h
        stm32_isr.h
        stm32_rcc.h
        stm32_registry.h
        nvic.h
        stm32_dma.h
    )
    SET(CHIBIOS_hal_PLATFORM_SOURCES
        hal_lld.c
        nvic.c
        stm32_dma.c
    )
    SET(INDEX 0)
    FOREACH(module ${CHIBIOS_HAL_PLATFORM_MODULES})
        LIST(GET CHIBIOS_HAL_PLATFORM_MODULES_PATHES ${INDEX} path)
        SET(CHIBIOS_${module}_PLATFORM_SEARCH_PATH ${CHIBIOS_ROOT}/os/hal/ports/STM32/${path})
        SET(CHIBIOS_${module}_PLATFORM_SEARCH_HEADERS hal_${module}_lld.h)
        SET(CHIBIOS_${module}_PLATFORM_SOURCES hal_${module}_lld.c)

        IF(${module} STREQUAL ext)
            SET(CHIBIOS_${module}_PLATFORM_SEARCH_PATH ${CHIBIOS_ext_PLATFORM_SEARCH_PATH} ${CHIBIOS_ROOT}/os/hal/ports/STM32/STM32F1xx)
            SET(CHIBIOS_${module}_PLATFORM_SEARCH_HEADERS ${CHIBIOS_ext_PLATFORM_SEARCH_HEADERS} hal_ext_lld_isr.h)
            SET(CHIBIOS_${module}_PLATFORM_SOURCES ${CHIBIOS_ext_PLATFORM_SOURCES} hal_ext_lld_isr.c)
        ENDIF()

        MATH(EXPR INDEX "${INDEX} + 1")
    ENDFOREACH()
ELSEIF(STM32_FAMILY STREQUAL "F4")
    SET(CHIBIOS_HAL_PLATFORM_MODULES adc can dac ext gpt i2c i2s icu mac pal pwm rtc sdc serial spi st uart usb)
    SET(CHIBIOS_HAL_PLATFORM_MODULES_PATHES
      LLD/ADCv2
      LLD/CANv1
      LLD/DACv1
      LLD/EXTIv1
      LLD/TIMv1
      LLD/I2Cv1
      LLD/SPIv1
      LLD/TIMv1
      LLD/MACv1
      LLD/GPIOv2
      LLD/TIMv1
      LLD/RTCv2
      LLD/SDIOv1
      LLD/USARTv1
      LLD/SPIv1
      LLD/TIMv1
      LLD/USARTv1
      LLD/OTGv1
    )

    SET(CHIBIOS_hal_PLATFORM_SEARCH_PATH
        ${CHIBIOS_ROOT}/os/hal/ports/common/ARMCMx
        ${CHIBIOS_ROOT}/os/hal/ports/STM32/STM32F4xx
        ${CHIBIOS_ROOT}/os/hal/ports/STM32/LLD/DMAv2
    )
    SET(CHIBIOS_hal_PLATFORM_SEARCH_HEADERS
        hal_lld.h
        stm32_isr.h
        stm32_rcc.h
        stm32_registry.h
        nvic.h
        stm32_dma.h
    )
    SET(CHIBIOS_hal_PLATFORM_SOURCES
        hal_lld.c
        nvic.c
        stm32_dma.c
    )
    SET(INDEX 0)
    FOREACH(module ${CHIBIOS_HAL_PLATFORM_MODULES})
        LIST(GET CHIBIOS_HAL_PLATFORM_MODULES_PATHES ${INDEX} path)
        SET(CHIBIOS_${module}_PLATFORM_SEARCH_PATH ${CHIBIOS_ROOT}/os/hal/ports/STM32/${path})
        SET(CHIBIOS_${module}_PLATFORM_SEARCH_HEADERS hal_${module}_lld.h)
        SET(CHIBIOS_${module}_PLATFORM_SOURCES hal_${module}_lld.c)

        IF(${module} STREQUAL ext)
            SET(CHIBIOS_${module}_PLATFORM_SEARCH_PATH ${CHIBIOS_ext_PLATFORM_SEARCH_PATH} ${CHIBIOS_ROOT}/os/hal/ports/STM32/STM32F4xx)
            SET(CHIBIOS_${module}_PLATFORM_SEARCH_HEADERS ${CHIBIOS_ext_PLATFORM_SEARCH_HEADERS} hal_ext_lld_isr.h)
            SET(CHIBIOS_${module}_PLATFORM_SOURCES ${CHIBIOS_ext_PLATFORM_SOURCES} hal_ext_lld_isr.c)
        ENDIF()

        MATH(EXPR INDEX "${INDEX} + 1")
    ENDFOREACH()
ELSEIF(STM32_FAMILY STREQUAL "L0")
    SET(CHIBIOS_HAL_PLATFORM_MODULES adc can dac ext gpt i2c icu pal pwm rtc serial spi st uart usb wdg)
    SET(CHIBIOS_HAL_PLATFORM_MODULES_PATHES
      LLD/ADCv1
      LLD/CANv1
      LLD/DACv1
      LLD/EXTIv1
      LLD/TIMv1
      LLD/I2Cv2
      LLD/TIMv1
      LLD/GPIOv2
      LLD/TIMv1
      LLD/RTCv2
      LLD/USARTv2
      LLD/SPIv1
      LLD/TIMv1
      LLD/USARTv2
      LLD/USBv1
      LLD/xWDGv1
    )


    SET(CHIBIOS_hal_PLATFORM_SEARCH_PATH
        ${CHIBIOS_ROOT}/os/hal/ports/common/ARMCMx
        ${CHIBIOS_ROOT}/os/hal/ports/STM32/STM32L0xx
        ${CHIBIOS_ROOT}/os/hal/ports/STM32/LLD/DMAv1
    )
    SET(CHIBIOS_hal_PLATFORM_SEARCH_HEADERS
        hal_lld.h
        stm32_isr.h
        stm32_rcc.h
        stm32_registry.h
        nvic.h
        stm32_dma.h
    )
    SET(CHIBIOS_hal_PLATFORM_SOURCES
        hal_lld.c
        nvic.c
        stm32_dma.c
    )
    SET(INDEX 0)
    FOREACH(module ${CHIBIOS_HAL_PLATFORM_MODULES})
        LIST(GET CHIBIOS_HAL_PLATFORM_MODULES_PATHES ${INDEX} path)
        SET(CHIBIOS_${module}_PLATFORM_SEARCH_PATH ${CHIBIOS_ROOT}/os/hal/ports/STM32/${path})
        SET(CHIBIOS_${module}_PLATFORM_SEARCH_HEADERS hal_${module}_lld.h)
        SET(CHIBIOS_${module}_PLATFORM_SOURCES hal_${module}_lld.c)

        IF(${module} STREQUAL ext)
            SET(CHIBIOS_${module}_PLATFORM_SEARCH_PATH ${CHIBIOS_ext_PLATFORM_SEARCH_PATH} ${CHIBIOS_ROOT}/os/hal/ports/STM32/STM32L0xx)
            SET(CHIBIOS_${module}_PLATFORM_SEARCH_HEADERS ${CHIBIOS_ext_PLATFORM_SEARCH_HEADERS} hal_ext_lld_isr.h)
            SET(CHIBIOS_${module}_PLATFORM_SOURCES ${CHIBIOS_ext_PLATFORM_SOURCES} hal_ext_lld_isr.c)
        ENDIF()

        MATH(EXPR INDEX "${INDEX} + 1")
    ENDFOREACH()
ENDIF()
