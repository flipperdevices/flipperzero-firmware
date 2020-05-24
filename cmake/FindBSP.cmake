set(BSP_F0_BOARDS 
    STM32F0xx_Nucleo_32 STM32F0xx-Nucleo STM32F072B-Discovery 
    STM32F0308-Discovery STM32072B_EVAL STM32091C_EVAL
)
set(BSP_F0_COMPONENTS
    hx8347d l3gd20 spfd5408 st7735 stlm75
)
set(BSP_F0_SOURCES_STM32F0xx_Nucleo_32 stm32f0xx_nucleo_32.c)
set(BSP_F0_SOURCES_STM32F0xx_Nucleo stm32f0xx_nucleo.c)
set(BSP_F0_SOURCES_STM32F072B_Discovery stm32f072b_discovery_eeprom.c stm32f072b_discovery_gyroscope.c stm32f072b_discovery.c)
set(BSP_F0_SOURCES_STM32F0308_Discovery stm32f0308_discovery.c)
set(BSP_F0_SOURCES_STM32072B_EVAL stm32072b_eval_eeprom.c stm32072b_eval_lcd.c stm32072b_eval_sd.c stm32072b_eval_tsensor.c stm32072b_eval.c)
set(BSP_F0_SOURCES_STM32091C_EVAL stm32091c_eval_eeprom.c stm32091c_eval_lcd.c stm32091c_eval_sd.c stm32091c_eval_tsensor.c stm32091c_eval.c)

if(NOT BSP_FIND_COMPONENTS)
    set(BSP_FIND_COMPONENTS 
        STM32F0 STM32F1 STM32F2 STM32F3 STM32F4 STM32F7
        STM32G0 STM32G4 
        STM32H7
        STM32L0 STM32L1 STM32L4 
    )
endif()

foreach(COMP ${BSP_FIND_COMPONENTS})
    string(TOLOWER ${COMP} COMP_L)
    string(TOUPPER ${COMP} COMP_U)
    
    string(REGEX MATCH "^STM32([FGHL][0-9]).*$" COMP_U ${COMP_U})
    
    if(NOT CMAKE_MATCH_1)
        message(FATAL_ERROR "Unknown HAL component: ${COMP}")
    endif()
        
    set(FAMILY ${CMAKE_MATCH_1})
    string(TOLOWER ${FAMILY} FAMILY_L)
    
    if(NOT STM32_CUBE_${FAMILY}_PATH)
        set(STM32_CUBE_${FAMILY}_PATH /opt/STM32Cube${FAMILY} CACHE PATH "Path to STM32Cube${FAMILY}")
        message(STATUS "No STM32_CUBE_${FAMILY}_PATH specified using default: ${STM32_CUBE_${FAMILY}_PATH}")
    endif()
        
    find_path(BSP_${FAMILY}_PATH
        NAMES Components/Common/io.h
        PATHS "${STM32_CUBE_${FAMILY}_PATH}/Drivers/BSP"
        NO_DEFAULT_PATH
    )
    if (NOT BSP_${FAMILY}_PATH)
        continue()
    endif()
    
    set(BSP_${FAMILY}_INCLUDE "${BSP_${FAMILY}_PATH}/Components/Common")
    
    add_library(BSP::STM32::${FAMILY} INTERFACE IMPORTED)
    target_link_libraries(BSP::STM32::${FAMILY} INTERFACE STM32::${FAMILY})
    target_include_directories(BSP::STM32::${FAMILY} INTERFACE "${BSP_${FAMILY}_PATH}/Components/Common")
        
    foreach(BOARD ${BSP_${FAMILY}_BOARDS})
        string(REPLACE "-" "_" BOARD_CANONICAL ${BOARD})
        
        add_library(BSP::STM32::${BOARD_CANONICAL} INTERFACE IMPORTED)
        target_link_libraries(BSP::STM32::${BOARD_CANONICAL} INTERFACE BSP::STM32::${FAMILY} CMSIS::STM32::${FAMILY})
        target_include_directories(BSP::STM32::${BOARD_CANONICAL} INTERFACE "${BSP_${FAMILY}_PATH}/${BOARD}")
        foreach(SRC ${BSP_${FAMILY}_SOURCES_${BOARD_CANONICAL}})
            target_sources(BSP::STM32::${BOARD_CANONICAL} INTERFACE "${BSP_${FAMILY}_PATH}/${BOARD}/${SRC}")
        endforeach()
    endforeach()
    
    foreach(BCOMP ${BSP_${FAMILY}_COMPONENTS}) 
        string(TOLOWER ${BCOMP} BCOMP_L)
        string(TOUPPER ${BCOMP} BCOMP_U)
        
        add_library(BSP::STM32::${FAMILY}::${BCOMP_U} INTERFACE IMPORTED)
        target_link_libraries(BSP::STM32::${FAMILY}::${BCOMP_U} INTERFACE BSP::STM32::${FAMILY} CMSIS::STM32::${FAMILY})
        target_include_directories(BSP::STM32::${FAMILY}::${BCOMP_U} INTERFACE "${BSP_${FAMILY}_PATH}/Components/${BCOMP}")
        target_sources(BSP::STM32::${FAMILY}::${BCOMP_U} INTERFACE "${BSP_${FAMILY}_PATH}/Components/${BCOMP}/${BCOMP}.c")
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

