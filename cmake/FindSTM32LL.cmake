SET(STM32LL_HEADER_ONLY_COMPONENTS	bus cortex iwdg system wwdg dmamux)

IF(STM32_FAMILY STREQUAL "F0")
    SET(LL_COMPONENTS	adc bus comp cortex crc crs dac dma exti gpio i2c
						i2s iwdg pwr rcc rtc spi system tim usart utils wwdg)

    SET(LL_REQUIRED_COMPONENTS bus cortex pwr rcc system utils)

    SET(LL_PREFIX stm32f0xx_)

ELSEIF(STM32_FAMILY STREQUAL "F1")
    SET(LL_COMPONENTS	adc bus cortex crc dac dma exti gpio i2c i2s
						iwdg pwr rcc rtc spi system tim usart usb utils wwdg)

    SET(LL_REQUIRED_COMPONENTS bus cortex pwr rcc system utils)

    SET(LL_PREFIX stm32f1xx_)

ELSEIF(STM32_FAMILY STREQUAL "F2")
    SET(LL_COMPONENTS	adc bus cortex crc dac dma exti gpio i2c i2s iwdg pwr 
						rcc rng rtc spi system tim usart usb utils wwdg)

    SET(LL_REQUIRED_COMPONENTS bus cortex pwr rcc system utils)

    SET(LL_PREFIX stm32f2xx_)

ELSEIF(STM32_FAMILY STREQUAL "F3")
    SET(LL_COMPONENTS	adc bus comp cortex crc dac dma exti gpio hrtim i2c i2s
						iwdg opamp pwr rcc rtc spi system tim usart utils wwdg)

    SET(LL_REQUIRED_COMPONENTS bus cortex pwr rcc system utils)

    SET(LL_PREFIX stm32f3xx_)

ELSEIF(STM32_FAMILY STREQUAL "F4")
    SET(LL_COMPONENTS	adc bus cortex crc dac dma2d dma exti fmc gpio i2c i2s iwdg
						lptim pwr rcc rng rtc spi system tim usart usb utils wwdg)

    SET(LL_REQUIRED_COMPONENTS bus cortex pwr rcc system utils)

    SET(LL_PREFIX stm32f4xx_)

ELSEIF(STM32_FAMILY STREQUAL "F7")
    SET(LL_COMPONENTS	adc bus cortex crc dac dma2d dma exti gpio i2c i2s iwdg
						lptim pwr rcc rng rtc spi system tim usart usb utils wwdg)

    SET(LL_REQUIRED_COMPONENTS bus cortex pwr rcc system utils)

    SET(LL_PREFIX stm32f7xx_)

ELSEIF(STM32_FAMILY STREQUAL "L0")
    SET(LL_COMPONENTS	adc bus comp cortex crc crs dac dma exti gpio i2c i2s
						iwdg lptim lpuart pwr rcc rng rtc spi system tim usart
						utils wwdg)

    SET(LL_REQUIRED_COMPONENTS bus cortex pwr rcc system utils)

    SET(LL_PREFIX stm32l0xx_)

ELSEIF(STM32_FAMILY STREQUAL "L1")
    SET(LL_COMPONENTS	adc comp crc dac dma exti fsmc gpio i2c opamp pwr rcc 
						rtc sdmmc spi tim usart utils)
    SET(LL_REQUIRED_COMPONENTS pwr rcc utils)

    SET(LL_PREFIX stm32l1xx_)

ELSEIF(STM32_FAMILY STREQUAL "L4")
    SET(LL_COMPONENTS	adc bus comp cortex crc crs dac dma2d dmamux dma exti 
						gpio i2c iwdg lptim lpuart opamp pwr rcc rng rtc spi 
						system tim usart usb utils wwdg)

    SET(LL_REQUIRED_COMPONENTS bus cortex pwr rcc system utils)

    SET(LL_PREFIX stm32l4xx_)

ENDIF()

ADD_DEFINITIONS(-DUSE_FULL_LL_DRIVER)

FOREACH(cmp ${LL_REQUIRED_COMPONENTS})
	LIST(FIND STM32LL_FIND_COMPONENTS ${cmp} STM32LL_FOUND_INDEX)
	IF(${STM32LL_FOUND_INDEX} LESS 0)
		LIST(APPEND STM32LL_FIND_COMPONENTS ${cmp})
	ENDIF()
ENDFOREACH()

FOREACH(cmp ${STM32LL_FIND_COMPONENTS})
	LIST(FIND LL_COMPONENTS ${cmp} STM32LL_FOUND_INDEX)
	IF(${STM32LL_FOUND_INDEX} LESS 0)
		MESSAGE(FATAL_ERROR "Unknown STM32LL component: ${cmp}. Available components: ${LL_COMPONENTS}")
	ELSE()
		LIST(FIND STM32LL_HEADER_ONLY_COMPONENTS ${cmp} HEADER_ONLY_FOUND_INDEX)
		IF(${HEADER_ONLY_FOUND_INDEX} LESS 0)
			LIST(APPEND LL_SRCS ${LL_PREFIX}ll_${cmp}.c)
		ENDIF()
		LIST(APPEND LL_HEADERS ${LL_PREFIX}ll_${cmp}.h)
	ENDIF()
ENDFOREACH()

LIST(REMOVE_DUPLICATES LL_HEADERS)
LIST(REMOVE_DUPLICATES LL_SRCS)

STRING(TOLOWER ${STM32_FAMILY} STM32_FAMILY_LOWER)

FIND_PATH(STM32LL_INCLUDE_DIR ${LL_HEADERS}
	PATH_SUFFIXES include stm32${STM32_FAMILY_LOWER}
	HINTS ${STM32Cube_DIR}/Drivers/STM32${STM32_FAMILY}xx_HAL_Driver/Inc
	CMAKE_FIND_ROOT_PATH_BOTH
)

FOREACH(LL_SRC ${LL_SRCS})
	STRING(MAKE_C_IDENTIFIER "${LL_SRC}" LL_SRC_CLEAN)
	SET(LL_${LL_SRC_CLEAN}_FILE LL_SRC_FILE-NOTFOUND)
	FIND_FILE(LL_${LL_SRC_CLEAN}_FILE ${LL_SRC}
		PATH_SUFFIXES src stm32${STM32_FAMILY_LOWER}
		HINTS ${STM32Cube_DIR}/Drivers/STM32${STM32_FAMILY}xx_HAL_Driver/Src
		CMAKE_FIND_ROOT_PATH_BOTH
	)
	LIST(APPEND STM32LL_SOURCES ${LL_${LL_SRC_CLEAN}_FILE})
ENDFOREACH()
    
INCLUDE(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(STM32LL DEFAULT_MSG STM32LL_INCLUDE_DIR STM32LL_SOURCES)
