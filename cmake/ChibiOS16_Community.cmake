SET(CHIBIOS_COMMUNITY_MODULES community nand eicu usbh timcap qei onewire crc eeprom usb_hid usb_msd)

SET(CHIBIOS_community_SEARCH_HEADERS hal_community.h)
SET(CHIBIOS_community_SOURCES hal_community.c)

FOREACH(module ${CHIBIOS_COMMUNITY_MODULES})
    SET(CHIBIOS_${module}_SEARCH_PATH ${CHIBIOS_ROOT}/community/os/hal/include ${CHIBIOS_ROOT}/community/os/hal/src)
    SET(CHIBIOS_${module}_SOURCES hal_${module}.c)
    SET(CHIBIOS_${module}_SEARCH_HEADERS hal_${module}.h)
ENDFOREACH()

SET(CHIBIOS_HAL_PLATFORM_MODULES nand eicu usbh timcap qei crc)
SET(CHIBIOS_HAL_PLATFORM_MODULES_PATHES
    LLD/FSMCv1
    LLD/TIMv1
    LLD/USBHv1
    LLD/TIMv1
    LLD/TIMv1
    LLD/CRCv1
)

SET(INDEX 0)
FOREACH(module ${CHIBIOS_HAL_PLATFORM_MODULES})
    LIST(GET CHIBIOS_HAL_PLATFORM_MODULES_PATHES ${INDEX} path)
    SET(CHIBIOS_${module}_PLATFORM_SEARCH_PATH ${CHIBIOS_ROOT}/community/os/hal/ports/STM32/${path})
    SET(CHIBIOS_${module}_PLATFORM_SEARCH_HEADERS hal_${module}_lld.h)
    SET(CHIBIOS_${module}_PLATFORM_SOURCES hal_${module}_lld.c)
    MATH(EXPR INDEX "${INDEX} + 1")
ENDFOREACH()
