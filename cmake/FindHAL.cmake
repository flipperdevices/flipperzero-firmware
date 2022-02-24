# For information about why and how of this file: https://cmake.org/cmake/help/latest/command/find_package.html

set(HAL_DRIVERS_F0
    adc can cec comp cortex crc dac dma exti flash gpio i2c i2s irda iwdg pcd 
    pwr rcc rtc smartcard smbus spi tim tsc uart usart wwdg
)
set(HAL_EX_DRIVERS_F0
    adc crc dac flash i2c pcd pwr rcc rtc smartcard spi tim uart usart
)
set(HAL_LL_DRIVERS_F0
    adc comp crc crs dac dma exti gpio i2c pwr rcc rtc spi tim usart usb utils
)

set(HAL_DRIVERS_F1
    adc can cec cortex crc dac dma eth exti flash gpio hcd i2c i2s irda iwdg 
    mmc nand nor pccard pcd pwr rcc rtc sd smartcard spi sram tim uart usart 
    wwdg
)
set(HAL_EX_DRIVERS_F1
    adc dac flash gpio pcd rcc rtc tim
)
set(HAL_LL_DRIVERS_F1
    adc crc dac dma exti fsmc gpio i2c pwr rcc rtc sdmmc spi tim usart usb utils
)

set(HAL_DRIVERS_F2
    adc can cortex crc cryp dac dcmi dma eth exti flash gpio hash hcd i2c i2s 
    irda iwdg mmc nand nor pccard pcd pwr rcc rng rtc sd smartcard spi sram tim 
    uart usart wwdg
)
set(HAL_EX_DRIVERS_F2
    adc dac dcmi dma flash pcd pwr rcc rtc tim
)
set(HAL_LL_DRIVERS_F2
    adc crc dac dma exti fsmc gpio i2c pwr rcc rng rtc sdmmc spi tim usart usb 
    utils
)

set(HAL_DRIVERS_F3
    adc can cec comp cortex crc dac dma exti flash gpio hrtim i2c i2s irda iwdg 
    nand nor opamp pccard pcd pwr rcc rtc sdadc smartcard smbus spi sram tim tsc 
    uart usart wwdg
)
set(HAL_EX_DRIVERS_F3
    adc crc dac flash i2c i2s opamp pcd pwr rcc rtc smartcard spi tim uart usart
)
set(HAL_LL_DRIVERS_F3
    adc comp crc dac dma exti fmc gpio hrtim i2c opamp pwr rcc rtc spi tim usart 
    usb utils
)

set(HAL_DRIVERS_F4
    adc can cec cortex crc cryp dac dcmi dfsdm dma dma2d dsi eth exti flash
    flash_ramfunc fmpi2c gpio hash hcd i2c i2s irda iwdg lptim ltdc mmc nand nor 
    pccard pcd pwr qspi rcc rng rtc sai sd sdram smartcard smbus spdifrx spi 
    sram tim uart usart wwdg
)
set(HAL_EX_DRIVERS_F4
    adc cryp dac dcmi dma flash fmpi2c hash i2c i2s ltdc pcd pwr rcc rtc sai tim 
)
set(HAL_LL_DRIVERS_F4
    adc crc dac dma dma2d exti fmc fsmc gpio i2c lptim pwr rcc rng rtc sdmmc spi 
    tim usart usb utils 
)

set(HAL_DRIVERS_F7
    adc can cec cortex crc cryp dac dcmi dfsdm dma dma2d dsi eth exti flash 
    gpio hash hcd i2c i2s irda iwdg jpeg lptim ltdc mdios mmc nand nor pcd pwr 
    qspi rcc rng rtc sai sd sdram smartcard smbus spdifrx spi sram tim uart 
    usart wwdg
)
set(HAL_EX_DRIVERS_F7
    adc crc cryp dac dcmi dma flash hash i2c ltdc pcd pwr rcc rtc sai smartcard 
    spi tim uart
)
set(HAL_LL_DRIVERS_F7
    adc crc dac dma dma2d exti fmc gpio i2c lptim pwr rcc rng rtc sdmmc spi tim 
    usart usb utils
)

set(HAL_DRIVERS_G0
    adc cec comp cortex crc cryp dac dma exti flash gpio i2c i2s irda iwdg lptim 
    pwr rcc rng rtc smartcard smbus spi tim uart usart wwdg
)
set(HAL_EX_DRIVERS_G0
    adc crc cryp dac dma flash i2c pwr rcc rtc smartcard spi tim uart usart
)
set(HAL_LL_DRIVERS_G0
    adc comp crc dac dma exti gpio i2c lptim lpuart pwr rcc rng rtc spi tim ucpd 
    usart utils
)

set(HAL_DRIVERS_G4
    adc comp cordic cortex crc cryp dac dma exti fdcan flash flash_ramfunc fmac 
    gpio hrtim i2c i2s irda iwdg lptim nand nor opamp pcd pwr qspi rcc rng rtc 
    sai smartcard smbus spi sram tim uart usart wwdg
)
set(HAL_EX_DRIVERS_G4
    adc crc cryp dac dma flash i2c opamp pcd pwr rcc rtc sai smartcard spi tim 
    uart usart
)
set(HAL_LL_DRIVERS_G4
    adc comp cordic crc crs dac dma exti fmac fmc gpio hrtim i2c lptim lpuart 
    opamp pwr rcc rng rtc spi tim ucpd usart usb utils
)

set(HAL_DRIVERS_H7
    adc cec comp cordic cortex crc cryp dac dcmi dfsdm dma dma2d dsi dts eth exti fdcan
    flash fmac gfxmmu gpio hash hcd hrtim hsem i2c i2s irda iwdg jpeg lptim ltdc mdios
    mdma mmc nand nor opamp ospi otfdec pcd pssi pwr qspi ramecc rcc rng rtc sai 
    sd sdram smartcard smbus spdifrx spi sram swpmi tim uart usart wwdg
)
set(HAL_EX_DRIVERS_H7
    adc crc cryp dac dfsdm dma eth flash hash i2c i2s ltdc mmc opamp pcd pwr rcc
    rng rtc sai sd smartcard spi tim uart usart
)
set(HAL_LL_DRIVERS_H7
    adc bdma comp cordic crc crs dac delayblock dma dma2d exti fmac fmc gpio hrtim i2c lptim
    lpuart mdma opamp pwr rcc rng rtc sdmmc spi swpmi tim usart usb utils
)

set(HAL_DRIVERS_L0
    adc comp cortex crc cryp dac dma firewall flash flash_ramfunc gpio i2c i2s 
    irda iwdg lcd lptim pcd pwr rcc rng rtc smartcard smbus spi tim tsc uart 
    usart wwdg
)
set(HAL_EX_DRIVERS_L0
    adc comp crc cryp dac flash i2c pcd pwr rcc rtc smartcard tim uart
)
set(HAL_LL_DRIVERS_L0
    adc comp crc crs dac dma exti gpio i2c lptim lpuart pwr rcc rng rtc spi tim 
    usart usb utils
)

set(HAL_DRIVERS_L1
    adc comp cortex crc cryp dac dma flash flash_ramfunc gpio i2c i2s irda iwdg 
    lcd nor opamp pcd pwr rcc rtc sd smartcard spi sram tim uart usart wwdg
)
set(HAL_EX_DRIVERS_L1
    adc cryp dac flash opamp pcd pcd pwr rcc rtc tim
)
set(HAL_LL_DRIVERS_L1
    adc comp crc dac dma exti fsmc gpio i2c opamp pwr rcc rtc sdmmc spi tim 
    usart usb utils
)

set(HAL_DRIVERS_L4
    adc can comp cortex crc cryp dac dcmi dfsdm dma dma2d dsi exti firewall 
    flash flash_ramfunc gfxmmu gpio hash hcd i2c irda iwdg lcd lptim ltdc mmc 
    nand nor opamp ospi pcd pka pssi pwr qspi rcc rng rtc sai sd smartcard smbus 
    spi sram swpmi tim tsc uart usart wwdg
)
set(HAL_EX_DRIVERS_L4
    adc crc cryp dac dfsdm dma flash hash i2c ltdc mmc opamp pcd pwr rcc rng rtc 
    sai sd smartcard spi tim uart usart
)
set(HAL_LL_DRIVERS_L4
    adc comp crc crs dac dma dma2d exti fmc gpio i2c lptim lpuart opamp pka pwr 
    rcc rng rtc sdmmc spi swpmi tim usart usb utils
)

set(HAL_DRIVERS_L5
    adc comp cortex crc cryp dac dfsdm dma exti fdcan flash flash_ramfunc gpio 
    gtzc hash i2c icache irda iwdg lptim mmc nand nor opamp ospi pcd pka pwr rcc 
    rng rtc sai sd smartcard smbus spi sram tim tsc uart usart wwdg
)
set(HAL_EX_DRIVERS_L5
    adc crc cryp dac dfsdm dma flash hash i2c mmc opamp pcd pwr rcc
    rng rtc sai sd smartcard spi tim uart usart
)
set(HAL_LL_DRIVERS_L5
    adc comp crc crs dac dma exti fmc gpio i2c lptim lpuart opamp pka pwr rcc 
    rng rtc sdmmc spi tim ucpd usart usb utils
)

set(HAL_DRIVERS_MP1
    adc cec cortex crc cryp dac dcmi dfsdm dma exti fdcan gpio hash hsem i2c
    ipcc lptim mdios mdma pwr qspi rcc rng rtc sai sd smartcard smbus spdifrx
    spi sram tim uart usart wwdg
)
set(HAL_EX_DRIVERS_MP1
    adc crc cryp dac dfsdm dma hash i2c pwr rcc rtc sai sd smartcard spi tim
    uart usart
)
set(HAL_LL_DRIVERS_MP1
    adc delayblock dma exti fmc gpio i2c lptim pwr rcc rtc sdmmc spi tim usart
    utils
)

set(HAL_DRIVERS_U5
    adc comp cordic cortex crc cryp dac dcache dcmi dma dma2d exti fdcan flash fmac gpio 
    gtzc hash hcd i2c icache irda iwdg lptim mdf mmc nand nor opamp ospi otfdec pcd pka pssi pwr ramcfg rcc
    rng rtc sai sd smartcard smbus spi sram tim tsc uart usart wwdg
)
set(HAL_EX_DRIVERS_U5
    adc crc cryp dac dma flash hash i2c mmc opamp pcd pwr rcc
    rng rtc sai sd smartcard smbus spi tim uart usart
)
set(HAL_LL_DRIVERS_U5
    adc comp cordic crc crs dac dlyb dma dma2d exti fmac fmc gpio i2c icache lpgpio lptim lpuart opamp pka pwr rcc 
    rng rtc sdmmc spi tim ucpd usart usb utils
)

set(HAL_DRIVERS_WB
    adc comp cortex crc cryp dma exti flash gpio hsem 
    i2c ipcc irda iwdg lcd lptim pcd pka pwr qspi rcc 
    rng rtc sai smartcard smbus spi tim tsc uart usart wwdg
)
set(HAL_EX_DRIVERS_WB
    adc crc cryp dma flash i2c pcd pwr rcc
    rtc sai smartcard spi tim uart usart
)
set(HAL_LL_DRIVERS_WB
    adc comp crc crs dma exti gpio i2c lptim lpuart pka pwr rcc 
    rng rtc spi tim usart usb utils
)

set(HAL_DRIVERS_WL
    adc comp cortex crc cryp dac dma exti flash gpio gtzc hsem 
    i2c i2s ipcc irda iwdg lptim pka pwr rcc 
    rng rtc smartcard smbus spi subghz tim uart usart wwdg
)
set(HAL_EX_DRIVERS_WL
    adc crc cryp dma flash i2c pwr rcc
    rng rtc smartcard spi tim uart usart
)
set(HAL_LL_DRIVERS_WL
    adc comp crc dac dma exti gpio i2c lptim lpuart pka pwr rcc 
    rng rtc spi tim usart utils
)

foreach(FAMILY_SUFFIX ${STM32_SUPPORTED_FAMILIES_SHORT_NAME})
    list(APPEND HAL_DRIVERS ${HAL_DRIVERS_${FAMILY_SUFFIX}})
    list(APPEND HAL_LL_DRIVERS ${HAL_LL_DRIVERS_${FAMILY_SUFFIX}})
endforeach()
list(REMOVE_DUPLICATES HAL_DRIVERS)
list(REMOVE_DUPLICATES HAL_LL_DRIVERS)

foreach(COMP ${HAL_FIND_COMPONENTS})
    string(TOLOWER ${COMP} COMP_L)
    string(TOUPPER ${COMP} COMP_U)
    
    string(REGEX MATCH "^STM32([FGHLMUW]P?[0-9BL])([0-9A-Z][0-9M][A-Z][0-9A-Z])?_?(M0PLUS|M4|M7)?.*$" COMP_U ${COMP_U})
    if(CMAKE_MATCH_1)
        list(APPEND HAL_FIND_COMPONENTS_FAMILIES ${COMP})
        message(TRACE "FindHAL: append COMP ${COMP} to HAL_FIND_COMPONENTS_FAMILIES")
        continue()
    endif()
    if(${COMP_L} IN_LIST HAL_DRIVERS)
        list(APPEND HAL_FIND_COMPONENTS_DRIVERS ${COMP})
        message(TRACE "FindHAL: append COMP ${COMP} to HAL_FIND_COMPONENTS_DRIVERS")
        continue()
    endif()
    string(REGEX REPLACE "^ll_" "" COMP_L ${COMP_L})
    if(${COMP_L} IN_LIST HAL_LL_DRIVERS)
        list(APPEND HAL_FIND_COMPONENTS_DRIVERS_LL ${COMP})
        message(TRACE "FindHAL: append COMP ${COMP} to HAL_FIND_COMPONENTS_DRIVERS_LL")
        continue()
    endif()
    message(FATAL_ERROR "FindHAL: unknown HAL component: ${COMP}")
endforeach()

if(NOT HAL_FIND_COMPONENTS_FAMILIES)
    set(HAL_FIND_COMPONENTS_FAMILIES ${STM32_SUPPORTED_FAMILIES_LONG_NAME})
endif()

if(STM32H7 IN_LIST HAL_FIND_COMPONENTS_FAMILIES)
    list(REMOVE_ITEM HAL_FIND_COMPONENTS_FAMILIES STM32H7)
    list(APPEND HAL_FIND_COMPONENTS_FAMILIES STM32H7_M7 STM32H7_M4)
endif()

if(STM32WB IN_LIST HAL_FIND_COMPONENTS_FAMILIES)
    list(REMOVE_ITEM HAL_FIND_COMPONENTS_FAMILIES STM32WB)
    list(APPEND HAL_FIND_COMPONENTS_FAMILIES STM32WB_M4)
endif()

if(STM32WL IN_LIST HAL_FIND_COMPONENTS_FAMILIES)
    list(REMOVE_ITEM HAL_FIND_COMPONENTS_FAMILIES STM32WL)
    list(APPEND HAL_FIND_COMPONENTS_FAMILIES STM32WL_M4 STM32WL_M0PLUS)
endif()

if(STM32MP1 IN_LIST HAL_FIND_COMPONENTS_FAMILIES)
    list(REMOVE_ITEM HAL_FIND_COMPONENTS_FAMILIES STM32MP1)
    list(APPEND HAL_FIND_COMPONENTS_FAMILIES STM32MP1_M4)
endif()

list(REMOVE_DUPLICATES HAL_FIND_COMPONENTS_FAMILIES)

# when no explicit driver and driver_ll is given to find_component(HAL )
# then search for all supported driver and driver_ll
if((NOT HAL_FIND_COMPONENTS_DRIVERS) AND (NOT HAL_FIND_COMPONENTS_DRIVERS_LL))
    set(HAL_FIND_COMPONENTS_DRIVERS ${HAL_DRIVERS})
    set(HAL_FIND_COMPONENTS_DRIVERS_LL ${HAL_LL_DRIVERS})
endif()
list(REMOVE_DUPLICATES HAL_FIND_COMPONENTS_DRIVERS)
list(REMOVE_DUPLICATES HAL_FIND_COMPONENTS_DRIVERS_LL)

message(STATUS "Search for HAL families: ${HAL_FIND_COMPONENTS_FAMILIES}")
message(STATUS "Search for HAL drivers: ${HAL_FIND_COMPONENTS_DRIVERS}")
message(STATUS "Search for HAL LL drivers: ${HAL_FIND_COMPONENTS_DRIVERS_LL}")

foreach(COMP ${HAL_FIND_COMPONENTS_FAMILIES})
    string(TOUPPER ${COMP} COMP_U)
    
    string(REGEX MATCH "^STM32([FGHLMUW]P?[0-9BL])([0-9A-Z][0-9M][A-Z][0-9A-Z])?_?(M0PLUS|M4|M7)?.*$" COMP_U ${COMP_U})    
    if(CMAKE_MATCH_3)
        set(CORE ${CMAKE_MATCH_3})
        set(CORE_C "::${CORE}")
        set(CORE_U "_${CORE}")
    else()
        unset(CORE)
        unset(CORE_C)
        unset(CORE_U)
    endif()
        
    set(FAMILY ${CMAKE_MATCH_1})
    string(TOLOWER ${FAMILY} FAMILY_L)

    if((NOT STM32_HAL_${FAMILY}_PATH) AND (NOT STM32_CUBE_${FAMILY}_PATH) AND (DEFINED ENV{STM32_CUBE_${FAMILY}_PATH}))
        set(STM32_CUBE_${FAMILY}_PATH $ENV{STM32_CUBE_${FAMILY}_PATH} CACHE PATH "Path to STM32Cube${FAMILY}")
        message(STATUS "ENV STM32_CUBE_${FAMILY}_PATH specified, using STM32_CUBE_${FAMILY}_PATH: ${STM32_CUBE_${FAMILY}_PATH}")
    endif()

    if((NOT STM32_HAL_${FAMILY}_PATH) AND (NOT STM32_CUBE_${FAMILY}_PATH))
        set(STM32_CUBE_${FAMILY}_PATH /opt/STM32Cube${FAMILY} CACHE PATH "Path to STM32Cube${FAMILY}")
        message(STATUS "Neither STM32_CUBE_${FAMILY}_PATH nor STM32_HAL_${FAMILY}_PATH specified using default STM32_CUBE_${FAMILY}_PATH: ${STM32_CUBE_${FAMILY}_PATH}")
    endif()

    #Checking HAL patch or release version
    unset(VERSION_INFO)
    find_file(PACKAGE_FILE NAMES package.xml PATHS ${STM32_CUBE_${FAMILY}_PATH})
    if(PACKAGE_FILE)
        file(READ ${PACKAGE_FILE} PACKAGE_FILE_CONTENT)
        string(REGEX MATCH "PackDescription Release=\"FW.${FAMILY}.([0-9.]+)\"( Patch=\"FW.${FAMILY}.([0-9.]+)\")?" VERSION_INFO ${PACKAGE_FILE_CONTENT})
        if(CMAKE_MATCH_3) # This is the "Patch" revision
            set(HAL_${COMP}_VERSION ${CMAKE_MATCH_3})
            set(HAL_VERSION ${CMAKE_MATCH_3})
        else(CMAKE_MATCH_1) #This is the "Release" version 
            set(HAL_${COMP}_VERSION ${CMAKE_MATCH_1})
            set(HAL_VERSION ${CMAKE_MATCH_1})
        endif()
    endif()
    if(NOT VERSION_INFO)
        message(STATUS "Could not read the HAL version from package.xml for ${COMP}")
    endif()

    find_path(HAL_${FAMILY}_PATH
        NAMES Inc/stm32${FAMILY_L}xx_hal.h
        PATHS "${STM32_HAL_${FAMILY}_PATH}" "${STM32_CUBE_${FAMILY}_PATH}/Drivers/STM32${FAMILY}xx_HAL_Driver"
        NO_DEFAULT_PATH
    )
    if (NOT HAL_${FAMILY}_PATH)
        message(DEBUG "Missing HAL_${FAMILY}_PATH path")
        continue()
    endif()
    
    find_path(HAL_${FAMILY}${CORE_U}_INCLUDE
        NAMES stm32${FAMILY_L}xx_hal.h
        PATHS "${HAL_${FAMILY}_PATH}/Inc"
        NO_DEFAULT_PATH
    )
    find_file(HAL_${FAMILY}${CORE_U}_SOURCE
        NAMES stm32${FAMILY_L}xx_hal.c
        PATHS "${HAL_${FAMILY}_PATH}/Src"
        NO_DEFAULT_PATH
    )
    
    if ((NOT HAL_${FAMILY}${CORE_U}_INCLUDE) OR (NOT HAL_${FAMILY}${CORE_U}_SOURCE))
        set(HAL_${COMP}_FOUND FALSE)
        message(DEBUG "FindHAL: did not find path to HAL /src or /inc dir")
        continue()
    endif()

    if(NOT (TARGET HAL::STM32::${FAMILY}${CORE_C}))
        message(TRACE "FindHAL: creating library HAL::STM32::${FAMILY}${CORE_C}")
        add_library(HAL::STM32::${FAMILY}${CORE_C} INTERFACE IMPORTED)
        target_link_libraries(HAL::STM32::${FAMILY}${CORE_C} INTERFACE 
                                                    STM32::${FAMILY}${CORE_C} 
                                                    CMSIS::STM32::${FAMILY}${CORE_C})
        target_include_directories(HAL::STM32::${FAMILY}${CORE_C} INTERFACE "${HAL_${FAMILY}${CORE_U}_INCLUDE}")
        target_sources(HAL::STM32::${FAMILY}${CORE_C} INTERFACE "${HAL_${FAMILY}${CORE_U}_SOURCE}")
    endif()
    
    foreach(DRV_COMP ${HAL_FIND_COMPONENTS_DRIVERS})
        string(TOLOWER ${DRV_COMP} DRV_L)
        string(TOUPPER ${DRV_COMP} DRV)
        
        if(NOT (DRV_L IN_LIST HAL_DRIVERS_${FAMILY}))
            continue()
        endif()
        
        find_file(HAL_${FAMILY}${CORE_U}_${DRV}_SOURCE
            NAMES stm32${FAMILY_L}xx_hal_${DRV_L}.c
            PATHS "${HAL_${FAMILY}_PATH}/Src"
            NO_DEFAULT_PATH
        )
        list(APPEND HAL_${FAMILY}${CORE_U}_SOURCES "${HAL_${FAMILY}_${DRV}_SOURCE}")
        if(NOT HAL_${FAMILY}${CORE_U}_${DRV}_SOURCE)
            message(WARNING "Cannot find ${DRV} driver for ${FAMILY}${CORE_U}")
            set(HAL_${DRV_COMP}_FOUND FALSE)
            continue()
        endif()
                
        set(HAL_${DRV_COMP}_FOUND TRUE)
        if(HAL_${FAMILY}${CORE_U}_${DRV}_SOURCE AND (NOT (TARGET HAL::STM32::${FAMILY}::${DRV})))
            message(TRACE "FindHAL: creating library HAL::STM32::${FAMILY}${CORE_C}::${DRV}")
            add_library(HAL::STM32::${FAMILY}${CORE_C}::${DRV} INTERFACE IMPORTED)
            target_link_libraries(HAL::STM32::${FAMILY}${CORE_C}::${DRV} INTERFACE HAL::STM32::${FAMILY}${CORE_C})
            target_sources(HAL::STM32::${FAMILY}${CORE_C}::${DRV} INTERFACE "${HAL_${FAMILY}${CORE_U}_${DRV}_SOURCE}")
        endif()
                
        if(HAL_${FAMILY}${CORE_U}_${DRV}_SOURCE AND (${DRV_L} IN_LIST HAL_EX_DRIVERS_${FAMILY}))
            find_file(HAL_${FAMILY}${CORE_U}_${DRV}_EX_SOURCE
                NAMES stm32${FAMILY_L}xx_hal_${DRV_L}_ex.c
                PATHS "${HAL_${FAMILY}_PATH}/Src"
                NO_DEFAULT_PATH
            )
            list(APPEND HAL_${FAMILY}${CORE_U}_SOURCES "${HAL_${FAMILY}${CORE_U}_${DRV}_EX_SOURCE}")
            if(NOT HAL_${FAMILY}${CORE_U}_${DRV}_EX_SOURCE)
                message(WARNING "Cannot find ${DRV}Ex driver for ${FAMILY}${CORE_U}")
            endif()
                        
            if((TARGET HAL::STM32::${FAMILY}${CORE_C}::${DRV}) AND (NOT (TARGET HAL::STM32::${FAMILY}${CORE_C}::${DRV}Ex)))
                message(TRACE "FindHAL: creating library HAL::STM32::${FAMILY}${CORE_C}::${DRV}Ex")
                add_library(HAL::STM32::${FAMILY}${CORE_C}::${DRV}Ex INTERFACE IMPORTED)
                target_link_libraries(HAL::STM32::${FAMILY}${CORE_C}::${DRV}Ex INTERFACE HAL::STM32::${FAMILY}${CORE_C}::${DRV})
                target_sources(HAL::STM32::${FAMILY}${CORE_C}::${DRV}Ex INTERFACE "${HAL_${FAMILY}${CORE_U}_${DRV}_EX_SOURCE}")
            endif()
        endif()
    endforeach()
    
    foreach(DRV_COMP ${HAL_FIND_COMPONENTS_DRIVERS_LL})
        string(TOLOWER ${DRV_COMP} DRV_L)
        string(REGEX REPLACE "^ll_" "" DRV_L ${DRV_L})
        string(TOUPPER ${DRV_L} DRV)
        
        if(NOT (DRV_L IN_LIST HAL_LL_DRIVERS_${FAMILY}))
            continue()
        endif()
        
        find_file(HAL_${FAMILY}${CORE_U}_${DRV}_LL_SOURCE
            NAMES stm32${FAMILY_L}xx_ll_${DRV_L}.c
            PATHS "${HAL_${FAMILY}_PATH}/Src"
            NO_DEFAULT_PATH
        )
        list(APPEND HAL_${FAMILY}${CORE_U}_SOURCES "${HAL_${FAMILY}_${DRV}_LL_SOURCE}")
        if(NOT HAL_${FAMILY}${CORE_U}_${DRV}_LL_SOURCE)
            message(WARNING "Cannot find LL_${DRV} driver for ${FAMILY}${CORE_U}")
            set(HAL_${DRV_COMP}_FOUND FALSE)
            continue()
        endif()
    
        set(HAL_${DRV_COMP}_FOUND TRUE)
        if(HAL_${FAMILY}${CORE_U}_${DRV}_LL_SOURCE AND (NOT (TARGET HAL::STM32::${FAMILY}${CORE_C}::LL_${DRV})))
            message(TRACE "FindHAL: creating library HAL::STM32::${FAMILY}${CORE_C}::LL_${DRV}")
            add_library(HAL::STM32::${FAMILY}${CORE_C}::LL_${DRV} INTERFACE IMPORTED)
            target_include_directories(HAL::STM32::${FAMILY}${CORE_C}::LL_${DRV} INTERFACE "${HAL_${FAMILY}${CORE_U}_INCLUDE}")
            target_sources(HAL::STM32::${FAMILY}${CORE_C}::LL_${DRV} INTERFACE "${HAL_${FAMILY}${CORE_U}_${DRV}_LL_SOURCE}")
        endif()
    endforeach()
    
    set(HAL_${COMP}_FOUND TRUE)
    list(APPEND HAL_INCLUDE_DIRS "${HAL_${FAMILY}${CORE_U}_INCLUDE}")
    list(APPEND HAL_SOURCES "${HAL_${FAMILY}${CORE_U}_SOURCES}")
endforeach()

list(REMOVE_DUPLICATES HAL_INCLUDE_DIRS)
list(REMOVE_DUPLICATES HAL_SOURCES)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(HAL
    REQUIRED_VARS HAL_INCLUDE_DIRS HAL_SOURCES
    FOUND_VAR HAL_FOUND
    HANDLE_COMPONENTS
)
