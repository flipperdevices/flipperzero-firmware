# Supporting new families

## Write device file

a file <family>.cmake must be added to the folder cmake/stm32
This file containes the differents devices and the regex used to parse them.
It also give information on the RAM and CCRAM available

## Add family to list of know families

Update the list `STM32_SUPPORTED_FAMILIES_LONG_NAME` located in the cmake/stm32/common.cmake file


## Add family to CI

Update the cmake.yml workflow file to include the new family
