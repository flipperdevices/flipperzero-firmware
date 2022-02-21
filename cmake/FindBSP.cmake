# For information about why and how of this file: https://cmake.org/cmake/help/latest/command/find_package.html

### F0 ###
set(BSP_F0_BOARDS 
    STM32F0xx_Nucleo_32 STM32F0xx-Nucleo STM32F072B-Discovery 
    STM32F0308-Discovery STM32072B_EVAL STM32091C_EVAL
)
set(BSP_F0_COMPONENTS
    hx8347d l3gd20 spfd5408 st7735 stlm75
)
set(BSP_F0_SOURCES_STM32F072B_Discovery eeprom gyroscope)
set(BSP_F0_SOURCES_STM32072B_EVAL eeprom lcd sd tsensor)
set(BSP_F0_SOURCES_STM32091C_EVAL eeprom lcd sd tsensor)
set(BSP_F0_DEVICE_STM32F072B_Discovery F072RB)
set(BSP_F0_DEVICE_STM32F0308_Discovery F030R8)
set(BSP_F0_DEVICE_STM32072B_EVAL F072VB)
set(BSP_F0_DEVICE_STM32091C_EVAL F091VC)

### F1 ###
set(BSP_F1_BOARDS 
    STM32F1xx_Nucleo STM32VL-Discovery STM3210C_EVAL STM3210E_EVAL
)
set(BSP_F1_COMPONENTS
    ak4343 cs43l22 hx8347d ili9320 ili9325 lis302dl spfd5408 st7735 stlm75
    stmpe811
)
set(BSP_F1_SOURCES_STM3210C_EVAL accelerometer audio eeprom io lcd sd ts)
set(BSP_F1_SOURCES_STM3210E_EVAL audio lcd nand nor sd serialflash sram tsensor)
set(BSP_F1_DEVICE_STM32VL_Discovery F100RB)
set(BSP_F1_DEVICE_STM3210C_EVAL F107VC)
set(BSP_F1_DEVICE_STM3210E_EVAL F103ZE)

### F2 ###
set(BSP_F2_BOARDS 
    STM32F2xx_Nucleo_144 STM322xG_EVAL
)
set(BSP_F2_COMPONENTS
    cs43l22 ili9320 ili9325 ili9341 ov2640 st7735 stmpe811
)
set(BSP_F2_SOURCES_STM322xG_EVAL audio camera eeprom io lcd sd sram ts)
set(BSP_F2_DEVICE_STM322xG_EVAL F207IG)

### F3 ###
set(BSP_F3_BOARDS 
    STM32F3-Discovery STM32F3xx_Nucleo_32 STM32F3xx_Nucleo_144 STM32F3xx-Nucleo
    STM32F3348-Discovery STM32303C_EVAL STM32303E_EVAL STM32373C_EVAL
)
set(BSP_F3_COMPONENTS
    cs42l52 cs43l22 hx8347d hx8347g hx8347i ili9320 ili9325 ili9328 l3gd20
    lsm303dlhc spfd5408 st7735 stts751
)
set(BSP_F3_SOURCES_STM32F3_Discovery accelerometer gyroscope)
set(BSP_F3_SOURCES_STM32303C_EVAL audio eeprom lcd sd tsensor)
set(BSP_F3_SOURCES_STM32303E_EVAL audio eeprom lcd sd tsensor)
set(BSP_F3_SOURCES_STM32373C_EVAL audio eeprom lcd sd tsensor)
set(BSP_F3_DEVICE_STM32F3_Discovery F303VC)
set(BSP_F3_DEVICE_STM32F3348_Discovery F334C8)
set(BSP_F3_DEVICE_STM32303C_EVAL F303VC)
set(BSP_F3_DEVICE_STM32303E_EVAL F303VE)
set(BSP_F3_DEVICE_STM32373C_EVAL F373VC)

### F4 ###
set(BSP_F4_BOARDS 
    STM32F4-Discovery STM32F4xx_Nucleo_144 STM32F4xx-Nucleo STM32F401-Discovery
    STM32F411E-Discovery STM32F413H-Discovery STM32F429I-Discovery 
    STM324x9I_EVAL STM324xG_EVAL STM32412G-Discovery STM32446E_EVAL 
    STM32469I_EVAL STM32469I-Discovery
)
set(BSP_F4_COMPONENTS
    ampire480272 ampire640480 cs43l22 exc7200 ft6x06 ili9325 ili9341 l3gd20
    lis3dsh lis302dl ls016b8uy lsm303dlhc mfxstm32l152 n25q128a n25q256a 
    n25q512a otm8009a ov2640 s5k5cag s25fl512s st7735 st7789h2 stmpe811 
    stmpe1600 ts3510 wm8994
)
set(BSP_F4_SOURCES_STM32F4_Discovery accelerometer audio)
set(BSP_F4_SOURCES_STM32F401_Discovery accelerometer audio gyroscope)
set(BSP_F4_SOURCES_STM32F411E_Discovery accelerometer audio gyroscope)
set(BSP_F4_SOURCES_STM32F413H_Discovery audio lcd psram qspi sd ts)
set(BSP_F4_SOURCES_STM32F429I_Discovery eeprom gyroscope io lcd sdram ts)
set(BSP_F4_SOURCES_STM324x9I_EVAL audio camera eeprom io lcd nor sd sdram sram ts)
set(BSP_F4_SOURCES_STM324xG_EVAL audio camera eeprom io lcd sd sram ts)
set(BSP_F4_SOURCES_STM32412G_Discovery audio eeprom lcd qspi sd ts)
set(BSP_F4_SOURCES_STM3232446E_EVAL audio camera eeprom io lcd qspi sd sdram ts)
set(BSP_F4_SOURCES_STM32469I_EVAL audio camera eeprom io lcd nor qspi sd sdram sram ts)
set(BSP_F4_SOURCES_STM32469I_Discovery audio eeprom lcd qspi sd sdram ts)
set(BSP_F4_DEVICE_STM32F4_Discovery F407VG)
set(BSP_F4_DEVICE_STM32F401_Discovery F401VC)
set(BSP_F4_DEVICE_STM32F411E_Discovery F411VE)
set(BSP_F4_DEVICE_STM32F413H_Discovery F413ZH)
set(BSP_F4_DEVICE_STM32F429I_Discovery F429ZI)
set(BSP_F4_DEVICE_STM324x9I_EVAL F429NI)
set(BSP_F4_DEVICE_STM324xG_EVAL F407IG)
set(BSP_F4_DEVICE_STM32412G_Discovery F412ZG)
set(BSP_F4_DEVICE_STM32446E_EVAL F446ZE)
set(BSP_F4_DEVICE_STM32469I_EVAL F469NI)
set(BSP_F4_DEVICE_STM32469I_Discovery F469NI)

### F7 ###
set(BSP_F7_BOARDS 
    STM32F7xx_Nucleo_144 STM32F723E-Discovery STM32F769I_EVAL 
    STM32F769I-Discovery STM32F7308-Discovery STM32F7508-Discovery
    STM32746G-Discovery STM32756G_EVAL
)
set(BSP_F7_COMPONENTS
    adv7533 ampire480272 ampire640480 exc7200 ft6x06 ft5336 mfxstm32l152 
    mx25l512 n25q128a n25q512a otm8009a ov5640 ov9655 rk043fn48h s5k5cag st7735
    st7789h2 stmpe811 ts3510 wm8994
)
set(BSP_F7_SOURCES_STM32F723E_Discovery audio lcd psram qspi ts)
set(BSP_F7_SOURCES_STM32F769I_EVAL audio camera eeprom io lcd nor qspi sd sdram sram ts)
set(BSP_F7_SOURCES_STM32F769I_Discovery audio eeprom lcd qspi sd sdram ts)
set(BSP_F7_SOURCES_STM32F7308_Discovery audio lcd psram qspi ts)
set(BSP_F7_SOURCES_STM32F7508_Discovery audio camera eeprom lcd qspi sd sdram ts)
set(BSP_F7_SOURCES_STM32746G_Discovery audio camera eeprom lcd qspi sd sdram ts)
set(BSP_F7_SOURCES_STM32756G_EVAL audio camera eeprom io lcd nor qspi sd sdram sram ts)
set(BSP_F7_DEVICE_STM32F723E_Discovery F723IE)
set(BSP_F7_DEVICE_STM32F769I_EVAL F769NI)
set(BSP_F7_DEVICE_STM32F769I_Discovery F769NI)
set(BSP_F7_DEVICE_STM32F7308_Discovery F730I8)
set(BSP_F7_DEVICE_STM32F7508_Discovery F750N8)
set(BSP_F7_DEVICE_STM32746G_Discovery F746NG)
set(BSP_F7_DEVICE_STM32756G_EVAL F756NG)

### G0 ###
set(BSP_G0_BOARDS
    STM32G0xx_Nucleo STM32G0xx_Nucleo_32 STM32G071B-Discovery STM32G081B_EVAL 
    STM32G0316-Discovery
)
set(BSP_G0_COMPONENTS 
    hx8347d ina230 sn65dp141 ssd1315 st7735 stlm75 tusb546
)
set(BSP_G0_DIR_STM32G0316_Discovery STM32G0316-DISCO)
set(BSP_G0_SOURCES_STM32G071B_Discovery lcd pwr pwrmon)
set(BSP_G0_SOURCES_STM32G081B_EVAL lcd mux pwr sd tsensor)
set(BSP_G0_DEVICE_STM32G071B_Discovery G071RB)
set(BSP_G0_DEVICE_STM32G081B_EVAL G081RB)
set(BSP_G0_DEVICE_STM32G0316_Discovery G031J6)

### G4 ###
set(BSP_G4_BOARDS
    B-G474E-DPOW1 STM32G4xx_Nucleo STM32G474E-EVAL
)
set(BSP_G4_COMPONENTS 
    hx8347d mfxstm32l152 mt25ql512abb st7735 stts751 wm8994
)
set(BSP_G4_SOURCES_B-G474E-DPOW1 usbpd_pwr)
set(BSP_G4_SOURCES_STM32G481B_EVAL audio bus env_sensor idd io lcd qspi sd smartcard sram usbpd_pwr)
set(BSP_G4_DEVICE_B_G474E_DPOW1 G474RE)
set(BSP_G4_DEVICE_STM32G474E_EVAL G474QE)

### H7 ###
set(BSP_H7_BOARDS
    STM32H7B3I-Discovery STM32H7B3I-EVAL STM32H7xx_Nucleo STM32H743I-EVAL
    STM32H745I-Discovery STM32H747I-Discovery STM32H747I-EVAL 
    STM32H750B-Discovery
)
set(BSP_H7_COMPONENTS 
    adv7533 ampire480272 ampire640480 cs42l51 es_wifi exc7200 ft6x06 ft5336 
    is42s16800j is42s32800g is42s32800j lan8742 m24lr64 mfxstm32l152 
    mt25tl01g mt48lc4m32b2 mx25lm51245g otm8009a ov5640 ov9655 rk043fn48h 
    rk070er9427 s5k5cag st7735 stmpe811 ts3510 wm8994
)
set(BSP_H7_DIR_STM32H7B3I_Discovery STM32H7B3I-DK)
set(BSP_H7_DIR_STM32H745I_Discovery STM32H745I-DISCO)
set(BSP_H7_DIR_STM32H747I_Discovery STM32H747I-DISCO)
set(BSP_H7_DIR_STM32H750B_Discovery STM32H750B-DK)
set(BSP_H7_SOURCES_STM32H7B3I_Discovery audio bus camera eeprom lcd ospi sd sdram ts)
set(BSP_H7_SOURCES_STM32H7B3I_EVAL audio bus camera eeprom io lcd nor ospi sd sdram sram ts)
set(BSP_H7_SOURCES_STM32H743I_EVAL audio bus eeprom io lcd nor qspi sd sdram sram ts)
set(BSP_H7_SOURCES_STM32H745I_Discovery audio bus lcd mmc qspi sdram ts)
set(BSP_H7_SOURCES_STM32H747I_Discovery audio bus camera lcd qspi sd sdram ts)
set(BSP_H7_SOURCES_STM32H747I_EVAL audio bus eeprom io lcd nor qspi sd sdram sram ts)
set(BSP_H7_SOURCES_STM32H750B_Discovery audio bus lcd mmc qspi sdram ts)
set(BSP_H7_DEVICE_STM32H7B3I_Discovery H7B3LI)
set(BSP_H7_DEVICE_STM32H7B3I_EVAL H7B3LI)
set(BSP_H7_DEVICE_STM32H743I_EVAL H743XI)
set(BSP_H7_DEVICE_STM32H745I_Discovery H745XI)
set(BSP_H7_DEVICE_STM32H747I_Discovery H747XI)
set(BSP_H7_DEVICE_STM32H747I_EVAL H743XI)
set(BSP_H7_DEVICE_STM32H750B_Discovery H750XB)

### L0 ###
set(BSP_L0_BOARDS
    STM32L0xx_Nucleo STM32L0xx_Nucleo_32 STM32L073Z_EVAL STM32L0538-Discovery
)
set(BSP_L0_COMPONENTS 
    gde021a1 hx8347d mfxstm32l152 st7735 stlm75
)
set(BSP_L0_SOURCES_STM32L073Z_EVAL eeprom glass_lcd idd io lcd sd tsensor)
set(BSP_L0_SOURCES_STM32L0538_Discovery epd)
set(BSP_L0_DEVICE_STM32L073Z_EVAL L073VZ)
set(BSP_L0_DEVICE_STM32L0538_Discovery L053C8)

### L1 ###
set(BSP_L1_BOARDS
    STM32L1xx_Nucleo STM32L100C-Discovery STM32L152C-Discovery STM32L152D_EVAL
)
set(BSP_L1_COMPONENTS 
    cs43l22 hx8347d ili9320 ili9325 spfd5408 st7735 stlm75
)
set(BSP_L1_SOURCES_STM32L152C_Discovery glass_lcd)
set(BSP_L1_SOURCES_STM32L152D_EVAL audio eeprom audio glass_lcd lcd nor sd sram tsensor)
set(BSP_L1_DEVICE_STM32L100C_Discovery L100RC)
set(BSP_L1_DEVICE_STM32L152C_Discovery L152RC)
set(BSP_L1_DEVICE_STM32L152D_EVAL L152ZD)

### L4 ###
set(BSP_L4_BOARDS 
    STM32L475E-IOT01 STM32L4P5G-Discovery STM32L4R9I_EVAL STM32L4R9I-Discovery 
    STM32L4xx_Nucleo STM32L4xx_Nucleo_32 STM32L4xx_Nucleo_144 STM32L476G_EVAL 
    STM32L476G-Discovery STM32L496G-Discovery
)
set(BSP_L4_COMPONENTS
    cs42l51 cs43l22 cy8c4014lqi ft3x67 ft6x06 ft5336 hts221 hx8347g hx8347i 
    iss66wvh8m8 l3gd20 lis3mdl lps22hb ls016b8uy lsm6dsl lsm303c lsm303dlhc 
    m24sr mfxstm32l152 mx25lm51245g mx25r6435f n25q128a n25q256a ov9655 
    rk043fn48h st7735 st7789h2 stmpe811 stmpe1600 wm8994
)
set(BSP_L4_SOURCES_B_L475E_IOT01 accelerometer gyro hsensor magneto psensor qspi tsensor iot01)
set(BSP_L4_SOURCES_STM32L4P5G_Discovery idd io lcd mmc ospi_nor psram ts)
set(BSP_L4_SOURCES_STM32L4R9I_EVAL audio dsi_lcd dsi_ts eeprom idd io nor ospi_nor ospi_ram rgb_ts sd sram)
set(BSP_L4_SOURCES_STM32L4R9I_Discovery audio camera idd io lcd ospi_nor psram sd ts)
set(BSP_L4_SOURCES_STM32L476G_EVAL audio eeprom glass_lcd idd io lcd nor qspi sd sram ts)
set(BSP_L4_SOURCES_STM32L476G_Discovery audio compass glass_lcd gyroscope qspi)
set(BSP_L4_SOURCES_STM32L496G_Discovery audio camera idd io lcd qspi sd sram ts)
set(BSP_L4_DIR_STM32L475E_IOT01 B-L475E-IOT01)
set(BSP_L4_DEVICE_STM32L475E_IOT01 L475VG)
set(BSP_L4_DEVICE_STM32L4P5G_Discovery L4P5AG)
set(BSP_L4_DEVICE_STM32L4R9I_EVAL L4R9AI)
set(BSP_L4_DEVICE_STM32L4R9I_Discovery L4R9AI)
set(BSP_L4_DEVICE_STM32L476G_EVAL L476ZG)
set(BSP_L4_DEVICE_STM32L476G_Discovery L476VG)
set(BSP_L4_DEVICE_STM32L496G_Discovery L496AG)

### L5 ###
set(BSP_L5_BOARDS 
    STM32L552E_EVAL STM32L562E-Discovery STM32L5xx_Nucleo_144
)
set(BSP_L5_COMPONENTS
    cs42l51 ft6x06 hx8347i iss66wvh8m8 lsm6dso mfxstm32l152 mx25lm51245g
    st7789h2 stmpe811
)
set(BSP_L5_SOURCES_STM32L562E_Discovery audio bus idd lcd motion_sensor ospi sd ts usbpd_pwr)
set(BSP_L5_SOURCES_STM32L552E_EVAL audio bus idd io lcd ospi sd sram ts usbpd_pwr)
set(BSP_L5_DEVICE_STM32L562E_Discovery L562QE)
set(BSP_L5_DEVICE_STM32L552E_EVAL L552ZE)

### MP1 ###
set(BSP_MP1_BOARDS
    STM32MP15xx_DISCO STM32MP15xx_EVAL)
set(BSP_MP1_COMPONENTS )
set(BSP_MP1_SOURCES_STM32MP15xx_DISCO bus stpmic1)
set(BSP_MP1_SOURCES_STM32MP15xx_EVAL bus stpmic1)

### WB ###
set(BSP_WB_BOARDS
    STM32WB15CC_Nucleo STM32WB55_Nucleo STM32WB55_USBDongle STM32WB5MM_Discovery
)
set(BSP_WB_COMPONENTS 
    ism330dhcx s25fl128s ssd1315 stts22h
)
set(BSP_WB_DIR_STM32WB5MM_Discovery STM32WB5MM-DK)
set(BSP_WB_SOURCES_STM32WB5MM_Discovery audio bus env_sensors env_sensors_ex lcd motion_sensors motion_sensors_ex qspi)
set(BSP_WB_DEVICE_STM32WB5MM_Discovery WB5MMG)

### WL ###
set(BSP_WL_BOARDS
    STM32WLxx_Nucleo
)



if(NOT BSP_FIND_COMPONENTS)
    set(BSP_FIND_COMPONENTS ${STM32_SUPPORTED_FAMILIES_LONG_NAME})
endif()

if(STM32H7 IN_LIST BSP_FIND_COMPONENTS)
    list(REMOVE_ITEM BSP_FIND_COMPONENTS STM32H7)
    list(APPEND BSP_FIND_COMPONENTS STM32H7_M7 STM32H7_M4)
endif()

if(STM32WB IN_LIST BSP_FIND_COMPONENTS)
    list(REMOVE_ITEM BSP_FIND_COMPONENTS STM32WB)
    list(APPEND BSP_FIND_COMPONENTS STM32WB_M4)
endif()

if(STM32WL IN_LIST BSP_FIND_COMPONENTS)
    list(REMOVE_ITEM BSP_FIND_COMPONENTS STM32WL)
    list(APPEND BSP_FIND_COMPONENTS STM32WL_M4 STM32WL_M0PLUS)
endif()

if(STM32MP1 IN_LIST BSP_FIND_COMPONENTS)
    list(REMOVE_ITEM BSP_FIND_COMPONENTS STM32MP1)
    list(APPEND BSP_FIND_COMPONENTS STM32MP1_M4)
endif()

list(REMOVE_DUPLICATES BSP_FIND_COMPONENTS)

foreach(COMP ${BSP_FIND_COMPONENTS})
    string(TOLOWER ${COMP} COMP_L)
    string(TOUPPER ${COMP} COMP_U)
    
    string(REGEX MATCH "^STM32([FGHLMUW]P?[0-9BL])([0-9A-Z][0-9M][A-Z][0-9A-Z])?_?(M0PLUS|M4|M7)?.*$" COMP_U ${COMP_U})
    if(NOT CMAKE_MATCH_1)
        message(FATAL_ERROR "Unknown BSP component: ${COMP}")
    endif()
    
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
    
    if((NOT STM32_CUBE_${FAMILY}_PATH) AND (DEFINED ENV{STM32_CUBE_${FAMILY}_PATH))
        set(STM32_CUBE_${FAMILY}_PATH $ENV{STM32_CUBE_${FAMILY}_PATH} CACHE PATH "Path to STM32Cube${FAMILY}")
        message(STATUS "ENV STM32_CUBE_${FAMILY}_PATH specified, using STM32_CUBE_${FAMILY}_PATH: ${STM32_CUBE_${FAMILY}_PATH}")
    endif()

    if(NOT STM32_CUBE_${FAMILY}_PATH)
        set(STM32_CUBE_${FAMILY}_PATH /opt/STM32Cube${FAMILY} CACHE PATH "Path to STM32Cube${FAMILY}")
        message(STATUS "No STM32_CUBE_${FAMILY}_PATH specified using default: ${STM32_CUBE_${FAMILY}_PATH}")
    endif()

    set(BSP_${FAMILY}_PATH "${STM32_CUBE_${FAMILY}_PATH}/Drivers/BSP")
    if(NOT EXISTS ${BSP_${FAMILY}_PATH})
        continue()
    endif()
    
    set(BSP_${FAMILY}_INCLUDE "${BSP_${FAMILY}_PATH}/Components/Common")
    
    add_library(BSP::STM32::${FAMILY}${CORE_C} INTERFACE IMPORTED)
    target_link_libraries(BSP::STM32::${FAMILY}${CORE_C} INTERFACE STM32::${FAMILY}${CORE_C})
    target_include_directories(BSP::STM32::${FAMILY}${CORE_C} INTERFACE "${BSP_${FAMILY}_PATH}/Components/Common")
        
    foreach(BOARD ${BSP_${FAMILY}_BOARDS})
        string(REPLACE "-" "_" BOARD_CANONICAL ${BOARD})
        string(TOLOWER ${BOARD_CANONICAL} BOARD_CANONICAL_L)
        set(BOARD_DEVICE ${BSP_${FAMILY}_DEVICE_${BOARD_CANONICAL}})
        
        stm32_get_cores(DEV_CORES FAMILY ${FAMILY} DEVICE ${BOARD_DEVICE})
        if(CORE AND (NOT ${CORE} IN_LIST DEV_CORES))
            continue()
        endif()

        find_path(BSP_${BOARD_CANONICAL}_PATH
            NAMES ${BOARD_CANONICAL_L}.h
            PATHS "${BSP_${FAMILY}_PATH}/${BOARD}" "${BSP_${FAMILY}_PATH}/${BSP_${FAMILY}_DIR_${BOARD_CANONICAL}}"
            NO_DEFAULT_PATH
        )
        if (NOT EXISTS ${BSP_${BOARD_CANONICAL}_PATH})
            continue()
        endif()
        
        add_library(BSP::STM32::${BOARD_CANONICAL}${CORE_C} INTERFACE IMPORTED)
        target_link_libraries(BSP::STM32::${BOARD_CANONICAL}${CORE_C} INTERFACE BSP::STM32::${FAMILY}${CORE_C} CMSIS::STM32::${FAMILY}${CORE_C})
        target_include_directories(BSP::STM32::${BOARD_CANONICAL}${CORE_C} INTERFACE "${BSP_${BOARD_CANONICAL}_PATH}")
        target_sources(BSP::STM32::${BOARD_CANONICAL}${CORE_C} INTERFACE "${BSP_${BOARD_CANONICAL}_PATH}/${BOARD_CANONICAL_L}.c")
        
        foreach(SRC ${BSP_${FAMILY}_SOURCES_${BOARD_CANONICAL}})
            target_sources(BSP::STM32::${BOARD_CANONICAL}${CORE_C} INTERFACE "${BSP_${BOARD_CANONICAL}_PATH}/${BOARD_CANONICAL_L}_${SRC}.c")
        endforeach()
        if(BSP_${FAMILY}_DEVICE_${BOARD_CANONICAL})
            target_link_libraries(BSP::STM32::${BOARD_CANONICAL}${CORE_C} INTERFACE CMSIS::STM32::${BSP_${FAMILY}_DEVICE_${BOARD_CANONICAL}}${CORE_C})
        endif()
    endforeach()
    
    foreach(BCOMP ${BSP_${FAMILY}_COMPONENTS}) 
        string(TOLOWER ${BCOMP} BCOMP_L)
        string(TOUPPER ${BCOMP} BCOMP_U)
        
        add_library(BSP::STM32::${FAMILY}${CORE_C}::${BCOMP_U} INTERFACE IMPORTED)
        target_link_libraries(BSP::STM32::${FAMILY}${CORE_C}::${BCOMP_U} INTERFACE BSP::STM32::${FAMILY}${CORE_C} CMSIS::STM32::${FAMILY}${CORE_C})
        target_include_directories(BSP::STM32::${FAMILY}${CORE_C}::${BCOMP_U} INTERFACE "${BSP_${FAMILY}_PATH}/Components/${BCOMP}")
        
        find_file(BSP_${BOARD_CANONICAL}_${BCOMP}_SOURCE
            NAMES ${BCOMP}.c
            PATHS "${BSP_${FAMILY}_PATH}/Components/${BCOMP}"
            NO_DEFAULT_PATH
        )
        if (BSP_${BOARD_CANONICAL}_${BCOMP}_SOURCE)
            target_sources(BSP::STM32::${FAMILY}${CORE_C}::${BCOMP_U} INTERFACE "${BSP_${BOARD_CANONICAL}_${BCOMP}_SOURCE}")
        endif()
    endforeach()
    
    set(BSP_${COMP}_FOUND TRUE)
    
    if(BSP_${COMP}_FOUND)
         list(APPEND BSP_INCLUDE_DIRS "${BSP_${FAMILY}_INCLUDE}")
    endif()
endforeach()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(BSP
    REQUIRED_VARS BSP_INCLUDE_DIRS
    FOUND_VAR BSP_FOUND
    HANDLE_COMPONENTS
)
