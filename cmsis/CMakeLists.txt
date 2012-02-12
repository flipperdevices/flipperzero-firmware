PROJECT(stm32cmsis)

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
ENABLE_LANGUAGE(ASM)

IF(NOT STM32_StdPeriphLib_DIR)
    SET(STM32_StdPeriphLib_DIR "/opt/STM32F10x_StdPeriph_Lib_V3.5.0")
    MESSAGE(STATUS "No STM32_StdPeriphLib_DIR specified, using default: " ${STM32_StdPeriphLib_DIR})
ENDIF()

INCLUDE_DIRECTORIES(
    ${STM32_StdPeriphLib_DIR}/Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x/
    ${STM32_StdPeriphLib_DIR}/Libraries/CMSIS/CM3/CoreSupport/
    ${CMAKE_CURRENT_SOURCE_DIR}
)

SET(CMSIS_SOURCES
    ${STM32_StdPeriphLib_DIR}/Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x/system_stm32f10x.c
    ${STM32_StdPeriphLib_DIR}/Libraries/CMSIS/CM3/CoreSupport/core_cm3.c
)

SET(STM32_STARTUP_CL ${STM32_StdPeriphLib_DIR}/Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x/startup/gcc_ride7/startup_stm32f10x_cl.s)
SET(STM32_STARTUP_HD ${STM32_StdPeriphLib_DIR}/Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x/startup/gcc_ride7/startup_stm32f10x_hd.s)
SET(STM32_STARTUP_HD_VL ${STM32_StdPeriphLib_DIR}/Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x/startup/gcc_ride7/startup_stm32f10x_hd_vl.s)
SET(STM32_STARTUP_LD ${STM32_StdPeriphLib_DIR}/Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x/startup/gcc_ride7/startup_stm32f10x_ld.s)
SET(STM32_STARTUP_LD_VL ${STM32_StdPeriphLib_DIR}/Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x/startup/gcc_ride7/startup_stm32f10x_ld_vl.s)
SET(STM32_STARTUP_MD ${STM32_StdPeriphLib_DIR}/Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x/startup/gcc_ride7/startup_stm32f10x_md.s)
SET(STM32_STARTUP_MD_VL ${STM32_StdPeriphLib_DIR}/Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x/startup/gcc_ride7/startup_stm32f10x_md_vl.s)
SET(STM32_STARTUP_XL ${STM32_StdPeriphLib_DIR}/Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x/startup/gcc_ride7/startup_stm32f10x_xl.s)

ADD_LIBRARY(cmsis_cl ${CMSIS_SOURCES})
SET_TARGET_PROPERTIES(cmsis_cl PROPERTIES COMPILE_FLAGS "-DSTM32F10X_CL")

ADD_LIBRARY(cmsis_hd ${CMSIS_SOURCES})
SET_TARGET_PROPERTIES(cmsis_hd PROPERTIES COMPILE_DEFINITIONS "STM32F10X_HD")

ADD_LIBRARY(cmsis_hd_vl ${CMSIS_SOURCES})
SET_TARGET_PROPERTIES(cmsis_hd_vl PROPERTIES COMPILE_DEFINITIONS "STM32F10X_HD_VL")

ADD_LIBRARY(cmsis_ld ${CMSIS_SOURCES})
SET_TARGET_PROPERTIES(cmsis_ld PROPERTIES COMPILE_DEFINITIONS "STM32F10X_LD")

ADD_LIBRARY(cmsis_ld_vl ${CMSIS_SOURCES})
SET_TARGET_PROPERTIES(cmsis_ld_vl PROPERTIES COMPILE_DEFINITIONS "STM32F10X_LD_VL")

ADD_LIBRARY(cmsis_md ${CMSIS_SOURCES})
SET_TARGET_PROPERTIES(cmsis_md PROPERTIES COMPILE_DEFINITIONS "STM32F10X_MD")

ADD_LIBRARY(cmsis_md_vl ${CMSIS_SOURCES})
SET_TARGET_PROPERTIES(cmsis_md_vl PROPERTIES COMPILE_DEFINITIONS "STM32F10X_MD_VL")

ADD_LIBRARY(cmsis_xl ${CMSIS_SOURCES})
SET_TARGET_PROPERTIES(cmsis_xl PROPERTIES COMPILE_DEFINITIONS "STM32F10X_XL")

INSTALL(TARGETS cmsis_cl cmsis_hd cmsis_hd_vl cmsis_ld cmsis_ld_vl cmsis_md cmsis_md_vl cmsis_xl
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

INSTALL(FILES 
    ${STM32_StdPeriphLib_DIR}/Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x/system_stm32f10x.h
    ${STM32_StdPeriphLib_DIR}/Libraries/CMSIS/CM3/CoreSupport/core_cm3.h
    ${STM32_StdPeriphLib_DIR}/Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x/stm32f10x.h
    DESTINATION
    include
)

INSTALL(FILES 
    ${STM32_STARTUP_CL}
    ${STM32_STARTUP_HD}
    ${STM32_STARTUP_HD_VL}
    ${STM32_STARTUP_MD}
    ${STM32_STARTUP_MD_VL}
    ${STM32_STARTUP_LD}
    ${STM32_STARTUP_LD_VL}
    ${STM32_STARTUP_XL}
    ${CMAKE_CURRENT_SOURCE_DIR}/stm32_flash.ld.in
    DESTINATION
    share/cmsis/
)