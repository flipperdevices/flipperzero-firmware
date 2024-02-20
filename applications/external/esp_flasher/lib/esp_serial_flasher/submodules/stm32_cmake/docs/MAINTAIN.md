# Supporting new families

## Write device file

a file <family>.cmake must be added to the folder cmake/stm32
This file containes the differents devices and the regex used to parse them.
It also give information on the RAM and CCRAM available

## Add family to list of know families

Update the list `STM32_SUPPORTED_FAMILIES_LONG_NAME` located in the cmake/stm32/common.cmake file

## Add devices to list of devices in devices.cmake

Add all known devices to the list of all devices

## Update cube, cmsis and hal version in utilities.cmake

The versions to use can be found as follow:
 - Cube version : a valid tag from the repo https://github.com/STMicroelectronics/STM32Cube${FAMILY}
 - Cmsis version : a valid tag from the repo https://github.com/STMicroelectronics/cmsis_device_${FAMILY_L}
 - Hal version : a valid tag from the repo https://github.com/STMicroelectronics/stm32${FAMILY_L}xx_hal_driver
 
## Add family to CI

Add the file stm32${FAMILY_L}xx_hal_conf.h file to the root folder of each test. (A template is provided in the corresponding HAL)
Update the cmake.yml workflow file to include the new family
