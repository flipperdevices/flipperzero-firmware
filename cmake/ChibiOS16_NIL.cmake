SET(CHIBIOS_nil_SEARCH_PATH 
  ${CHIBIOS_ROOT}/os/nil/src
  ${CHIBIOS_ROOT}/os/nil/include
  ${CHIBIOS_ROOT}/os/nil/ports/ARMCMx
  ${CHIBIOS_ROOT}/os/nil/ports/ARMCMx/compilers/GCC
  ${CHIBIOS_ROOT}/os/common/ports/ARMCMx/compilers/GCC
  ${CHIBIOS_ROOT}/os/nil/src
)
SET(CHIBIOS_nil_SEARCH_HEADERS
  nil.h
  nilcore.h
  niltypes.h
)
SET(CHIBIOS_nil_SOURCES  
  crt1.c
  vectors.c
  nilcore.c
  nil.c
)

IF(STM32_FAMILY STREQUAL "F0")
  SET(CHIBIOS_nil_SOURCES  ${CHIBIOS_nil_SOURCES} crt0_v6m.s nilcore_v6m.c nilcoreasm_v6m.s)
  SET(CHIBIOS_nil_PLATFORM_SEARCH_PATH
    ${CHIBIOS_ROOT}/os/common/ports/ARMCMx/devices/STM32F0xx
    ${CHIBIOS_ROOT}/os/ext/CMSIS/include
    ${CHIBIOS_ROOT}/os/ext/CMSIS/ST/STM32F0xx/
  )
  SET(CHIBIOS_nil_PLATFORM_SEARCH_HEADERS
    core_cm0.h
    stm32f0xx.h
    cmparams.h
  )
ELSEIF(STM32_FAMILY STREQUAL "F1")
  SET(CHIBIOS_nil_SOURCES  ${CHIBIOS_nil_SOURCES} crt0_v7m.s nilcore_v7m.c nilcoreasm_v7m.s)
  SET(CHIBIOS_nil_PLATFORM_SEARCH_PATH
    ${CHIBIOS_ROOT}/os/common/ports/ARMCMx/devices/STM32F1xx
    ${CHIBIOS_ROOT}/os/ext/CMSIS/include
    ${CHIBIOS_ROOT}/os/ext/CMSIS/ST/STM32F1xx/
  )
  SET(CHIBIOS_nil_PLATFORM_SEARCH_HEADERS
    core_cm3.h
    stm32f1xx.h
    cmparams.h
  )
ELSEIF(STM32_FAMILY STREQUAL "F4")
  SET(CHIBIOS_nil_SOURCES  ${CHIBIOS_nil_SOURCES} crt0_v7m.s nilcore_v7m.c nilcoreasm_v7m.s)
  SET(CHIBIOS_nil_PLATFORM_SEARCH_PATH
    ${CHIBIOS_ROOT}/os/common/ports/ARMCMx/devices/STM32F4xx
    ${CHIBIOS_ROOT}/os/ext/CMSIS/include
    ${CHIBIOS_ROOT}/os/ext/CMSIS/ST/STM32F4xx/
  )
  SET(CHIBIOS_nil_PLATFORM_SEARCH_HEADERS
    core_cm4.h
    stm32f4xx.h
    cmparams.h
  )
ELSEIF(STM32_FAMILY STREQUAL "L0")
  SET(CHIBIOS_nil_SOURCES  ${CHIBIOS_nil_SOURCES} crt0_v6m.s nilcore_v6m.c nilcoreasm_v6m.s)
  SET(CHIBIOS_nil_PLATFORM_SEARCH_PATH
    ${CHIBIOS_ROOT}/os/common/ports/ARMCMx/devices/STM32L0xx
    ${CHIBIOS_ROOT}/os/ext/CMSIS/include
    ${CHIBIOS_ROOT}/os/ext/CMSIS/ST/STM32L0xx/
  )
  SET(CHIBIOS_nil_PLATFORM_SEARCH_HEADERS
    core_cm0plus.h
    stm32l0xx.h
    cmparams.h
  )
ENDIF()
