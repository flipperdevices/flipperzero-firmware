# For information about why and how of this file: https://cmake.org/cmake/help/latest/command/find_package.html

# This function gets a list of hal_driver using a given prefix and suffix
#
# out_list_hal_drivers   list of hal_drivers found
# hal_drivers_path       path to the hal's drivers
# hal_driver_type        hal_driver type to find (hal/ll/ex)
function(get_list_hal_drivers out_list_hal_drivers hal_drivers_path hal_driver_type)
    #The pattern to retrieve a driver from a file name depends on the hal_driver_type field
    if(${hal_driver_type} STREQUAL "hal" OR ${hal_driver_type} STREQUAL "ll")
        #This regex match and capture a driver type (stm32xx_hal_(rcc).c or stm32xx_ll_(rcc).c => catches rcc) 
        set(file_pattern ".+_${hal_driver_type}_([a-z0-9]+)\\.c$")
    elseif(${hal_driver_type} STREQUAL "ex")
        #This regex match and capture a driver type (stm32xx_hal_(rcc)_ex.c => catches rcc) 
        set(file_pattern ".+_hal_([a-z0-9]+)_ex\\.c$")
    else()
        message(FATAL_ERROR "the inputed hal_driver_type(${hal_driver_type}) is not valid.")
    endif()

    #Retrieving all the .c files from hal_drivers_path
    file(GLOB filtered_files
        RELATIVE "${hal_drivers_path}/Src"
        "${hal_drivers_path}/Src/*.c")
    # For all matched .c files keep only those with a driver name pattern (e.g. stm32xx_hal_rcc.c)
    list(FILTER filtered_files INCLUDE REGEX ${file_pattern})
    # From the files names keep only the driver type part using the regex (stm32xx_hal_(rcc).c or stm32xx_ll_(rcc).c => catches rcc)
    list(TRANSFORM filtered_files REPLACE ${file_pattern} "\\1")
    #Making a return by reference by seting the output variable to PARENT_SCOPE
    set(${out_list_hal_drivers} ${filtered_files} PARENT_SCOPE)
endfunction()

################################################################################
# Checking the parameters provided to the find_package(HAL ...) call
# The expected parameters are families and or drivers in *any orders*
# Families are valid if on the list of known families.
# Drivers are valid if on the list of valid driver of any family. For this
# reason the requested families must be processed in two steps
#  - Step 1 : Checking all the requested families
#  - Step 2 : Generating all the valid drivers from requested families
#  - Step 3 : Checking the other requested components (Expected to be drivers)
################################################################################
# Step 1 : Checking all the requested families
foreach(COMP ${HAL_FIND_COMPONENTS})
    string(TOUPPER ${COMP} COMP_U)
    string(REGEX MATCH "^STM32([FGHLMUW]P?[0-9BL])([0-9A-Z][0-9M][A-Z][0-9A-Z])?_?(M0PLUS|M4|M7)?.*$" COMP_U ${COMP_U})
    if(CMAKE_MATCH_1) #Matches the family part of the provided STM32<FAMILY>[..] component
        list(APPEND HAL_FIND_COMPONENTS_FAMILIES ${COMP})
        message(TRACE "FindHAL: append COMP ${COMP} to HAL_FIND_COMPONENTS_FAMILIES")
    else()
        list(APPEND HAL_FIND_COMPONENTS_UNHANDLED ${COMP})
    endif()
endforeach()

# If no family requested look for all families
if(NOT HAL_FIND_COMPONENTS_FAMILIES)
    set(HAL_FIND_COMPONENTS_FAMILIES ${STM32_SUPPORTED_FAMILIES_LONG_NAME})
endif()

# Step 2 : Generating all the valid drivers from requested families
foreach(family_comp ${HAL_FIND_COMPONENTS_FAMILIES})
    string(TOUPPER ${family_comp} family_comp)
    string(REGEX MATCH "^STM32([FGHLMUW]P?[0-9BL])([0-9A-Z][0-9M][A-Z][0-9A-Z])?_?(M0PLUS|M4|M7)?.*$" family_comp ${family_comp})
    if(CMAKE_MATCH_1) #Matches the family part of the provided STM32<FAMILY>[..] component
        set(FAMILY ${CMAKE_MATCH_1})
        string(TOLOWER ${FAMILY} FAMILY_L)
    endif()
    find_path(HAL_${FAMILY}_PATH
        NAMES Inc/stm32${FAMILY_L}xx_hal.h
        PATHS "${STM32_HAL_${FAMILY}_PATH}" "${STM32_CUBE_${FAMILY}_PATH}/Drivers/STM32${FAMILY}xx_HAL_Driver"
        NO_DEFAULT_PATH
        )
    if(NOT HAL_${FAMILY}_PATH)
        message(FATAL_ERROR "could not find HAL for family ${FAMILY}")
    else()
        set(HAL_${family_comp}_FOUND TRUE)
    endif()
    if(CMAKE_MATCH_1) #Matches the family part of the provided STM32<FAMILY>[..] component
        get_list_hal_drivers(HAL_DRIVERS_${FAMILY} ${HAL_${FAMILY}_PATH} "hal")
        get_list_hal_drivers(HAL_EX_DRIVERS_${FAMILY} ${HAL_${FAMILY}_PATH}  "ex")
        get_list_hal_drivers(HAL_LL_DRIVERS_${FAMILY} ${HAL_${FAMILY}_PATH} "ll")
        list(APPEND HAL_DRIVERS ${HAL_DRIVERS_${FAMILY}})
        list(APPEND HAL_LL_DRIVERS ${HAL_LL_DRIVERS_${FAMILY}})
    else()
    endif()
endforeach()
list(REMOVE_DUPLICATES HAL_DRIVERS)
list(REMOVE_DUPLICATES HAL_LL_DRIVERS)

# Step 3 : Checking the other requested components (Expected to be drivers)
foreach(COMP ${HAL_FIND_COMPONENTS_UNHANDLED})
    string(TOLOWER ${COMP} COMP_L)
    
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
