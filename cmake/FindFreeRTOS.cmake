# For information about why and how of this file: https://cmake.org/cmake/help/latest/command/find_package.html
set(FreeRTOS_PORTS ARM_CM0 ARM_CM3 ARM_CM3_MPU ARM_CM4_MPU ARM_CM4F ARM_CM7 ARM_CM7_MPU ARM_CM23 ARM_CM23_NTZ ARM_CM33 ARM_CM33_NTZ)
set(FreeRTOS_armv8_PORTS ARM_CM23_NTZ ARM_CM33_NTZ ARM_CM23 ARM_CM33)
set(FreeRTOS_armv8_trustZone_PORTS ARM_CM23 ARM_CM33)

if(NOT FreeRTOS_FIND_COMPONENTS)
    set(FreeRTOS_FIND_COMPONENTS ${FreeRTOS_PORTS})
endif()
list(REMOVE_DUPLICATES FreeRTOS_FIND_COMPONENTS)

if((NOT FREERTOS_PATH) AND (DEFINED ENV{FREERTOS_PATH}))
    set(FREERTOS_PATH $ENV{FREERTOS_PATH} CACHE PATH "Path to FreeRTOS")
    message(STATUS "ENV FREERTOS_PATH specified, using FREERTOS_PATH: ${FREERTOS_PATH}")
endif()

if(NOT FREERTOS_PATH)
    set(DEFAULT_FREERTOS_PATH "/opt/FreeRTOS")
    if(EXISTS ${DEFAULT_FREERTOS_PATH})
        set(FREERTOS_PATH ${DEFAULT_FREERTOS_PATH} CACHE PATH "Path to FreeRTOS")
        message(STATUS "No FREERTOS_PATH specified using default: ${DEFAULT_FREERTOS_PATH}")
    else()
        message(STATUS
            "No FreeRTOS folder found at default location ${DEFAULT_FREERTOS_PATH}. "
            "Leaving empty.."
        )
    endif()
endif()

if(STM32H7 IN_LIST FreeRTOS_FIND_COMPONENTS)
    list(REMOVE_ITEM FreeRTOS_FIND_COMPONENTS STM32H7)
    list(APPEND FreeRTOS_FIND_COMPONENTS STM32H7_M7 STM32H7_M4)
endif()

if(STM32WB IN_LIST BSP_FIND_COMPONENTS)
    list(REMOVE_ITEM FreeRTOS_FIND_COMPONENTS STM32WB)
    list(APPEND FreeRTOS_FIND_COMPONENTS STM32WB_M4)
endif()

if(STM32WL IN_LIST BSP_FIND_COMPONENTS)
    list(REMOVE_ITEM FreeRTOS_FIND_COMPONENTS STM32WL)
    list(APPEND FreeRTOS_FIND_COMPONENTS STM32WL_M4 STM32WL_M0PLUS)
endif()

# This section fills the family and ports components list
foreach(COMP ${FreeRTOS_FIND_COMPONENTS})
    string(TOUPPER ${COMP} COMP)
    string(REGEX MATCH "^STM32([FGHLMUW]P?[0-9BL])([0-9A-Z][0-9M][A-Z][0-9A-Z])?_?(M0PLUS|M4|M7)?.*$" FAMILY_COMP ${COMP})
    # Valid family component, so add it (e.g. STM32H7)
    if(CMAKE_MATCH_1)
        list(APPEND FreeRTOS_FIND_COMPONENTS_FAMILIES ${FAMILY_COMP})
        continue()
    endif()
    # Was not a family component, so add it to the port list
    list(APPEND FreeRTOS_FIND_COMPONENTS_PORTS ${COMP})
endforeach()

if(NOT FreeRTOS_FIND_COMPONENTS_PORTS)
    set(FreeRTOS_FIND_COMPONENTS_PORTS ${FreeRTOS_PORTS})
endif()

list(REMOVE_DUPLICATES FreeRTOS_FIND_COMPONENTS)
list(REMOVE_DUPLICATES FreeRTOS_FIND_COMPONENTS_PORTS)
list(REMOVE_DUPLICATES FreeRTOS_FIND_COMPONENTS_FAMILIES)

set(FreeRTOS_HEAPS 1 2 3 4 5)

macro(stm32_find_freertos FreeRTOS_NAMESPACE FREERTOS_PATH)
    find_path(FreeRTOS_COMMON_INCLUDE
        NAMES FreeRTOS.h
        PATHS "${FREERTOS_PATH}" "${FREERTOS_PATH}/FreeRTOS" 
        PATH_SUFFIXES  "Source/include" "include"
        NO_DEFAULT_PATH
    )

    if(NOT FreeRTOS_COMMON_INCLUDE)
        message(WARNING "FreeRTOS common include path not found, build might fail")
    endif()

    list(APPEND FreeRTOS_INCLUDE_DIRS "${FreeRTOS_COMMON_INCLUDE}")

    find_path(FreeRTOS_SOURCE_DIR
        NAMES tasks.c
        PATHS "${FREERTOS_PATH}" "${FREERTOS_PATH}/FreeRTOS"
        PATH_SUFFIXES  "Source"
        NO_DEFAULT_PATH
    )
    if(NOT (TARGET FreeRTOS))
        add_library(FreeRTOS INTERFACE IMPORTED)
        target_sources(FreeRTOS INTERFACE 
            "${FreeRTOS_SOURCE_DIR}/tasks.c"
            "${FreeRTOS_SOURCE_DIR}/list.c"
            "${FreeRTOS_SOURCE_DIR}/queue.c"
        )
        target_include_directories(FreeRTOS INTERFACE "${FreeRTOS_COMMON_INCLUDE}")
    endif()

    if(NOT (TARGET ${FreeRTOS_NAMESPACE}::Coroutine))
        add_library(${FreeRTOS_NAMESPACE}::Coroutine INTERFACE IMPORTED)
        target_sources(${FreeRTOS_NAMESPACE}::Coroutine INTERFACE "${FreeRTOS_SOURCE_DIR}/croutine.c")
        target_link_libraries(${FreeRTOS_NAMESPACE}::Coroutine INTERFACE FreeRTOS)
    endif()

    if(NOT (TARGET ${FreeRTOS_NAMESPACE}::EventGroups))
        add_library(${FreeRTOS_NAMESPACE}::EventGroups INTERFACE IMPORTED)
        target_sources(${FreeRTOS_NAMESPACE}::EventGroups INTERFACE "${FreeRTOS_SOURCE_DIR}/event_groups.c")
        target_link_libraries(${FreeRTOS_NAMESPACE}::EventGroups INTERFACE FreeRTOS)
    endif()

    if(NOT (TARGET ${FreeRTOS_NAMESPACE}::StreamBuffer))
        add_library(${FreeRTOS_NAMESPACE}::StreamBuffer INTERFACE IMPORTED)
        target_sources(${FreeRTOS_NAMESPACE}::StreamBuffer INTERFACE "${FreeRTOS_SOURCE_DIR}/stream_buffer.c")
        target_link_libraries(${FreeRTOS_NAMESPACE}::StreamBuffer INTERFACE FreeRTOS)
    endif()

    if(NOT (TARGET ${FreeRTOS_NAMESPACE}::Timers))
        add_library(${FreeRTOS_NAMESPACE}::Timers INTERFACE IMPORTED)
        target_sources(${FreeRTOS_NAMESPACE}::Timers INTERFACE "${FreeRTOS_SOURCE_DIR}/timers.c")
        target_link_libraries(${FreeRTOS_NAMESPACE}::Timers INTERFACE FreeRTOS)
    endif()

    foreach(HEAP ${FreeRTOS_HEAPS})
        if(NOT (TARGET ${FreeRTOS_NAMESPACE}::Heap::${HEAP}))
            add_library(${FreeRTOS_NAMESPACE}::Heap::${HEAP} INTERFACE IMPORTED)
            target_sources(${FreeRTOS_NAMESPACE}::Heap::${HEAP} INTERFACE "${FreeRTOS_SOURCE_DIR}/portable/MemMang/heap_${HEAP}.c")
            target_link_libraries(${FreeRTOS_NAMESPACE}::Heap::${HEAP} INTERFACE FreeRTOS)
        endif()
    endforeach()

    foreach(PORT ${FreeRTOS_FIND_COMPONENTS_PORTS})
        if(${PORT} IN_LIST FreeRTOS_armv8_trustZone_PORTS)
            set(ARMv8_NON_SECURE "::NON_SECURE")
        endif()
        
        find_path(FreeRTOS_${PORT}_PATH
            NAMES portmacro.h
            PATHS "${FREERTOS_PATH}" "${FREERTOS_PATH}/FreeRTOS" 
            PATH_SUFFIXES
                "portable/GCC/${PORT}"
                "portable/GCC/${PORT}/r0p1"
                "portable/GCC/${PORT}/non_secure"
                "Source/portable/GCC/${PORT}"
                "Source/portable/GCC/${PORT}/r0p1"
                "Source/portable/GCC/${PORT}/non_secure"
            NO_DEFAULT_PATH
        )

        if(NOT FreeRTOS_${PORT}_PATH)
            message(WARNING "FreeRTOS port path not found, build might fail")
        endif()

        list(APPEND FreeRTOS_INCLUDE_DIRS "${FreeRTOS_${PORT}_PATH}")
        
        find_file(FreeRTOS_${PORT}_SOURCE
            NAMES port.c
            PATHS "${FreeRTOS_${PORT}_PATH}"
            NO_DEFAULT_PATH
        )
        if(NOT (TARGET ${FreeRTOS_NAMESPACE}::${PORT}${ARMv8_NON_SECURE}))
            add_library(${FreeRTOS_NAMESPACE}::${PORT}${ARMv8_NON_SECURE} INTERFACE IMPORTED)
            target_link_libraries(${FreeRTOS_NAMESPACE}::${PORT}${ARMv8_NON_SECURE} INTERFACE FreeRTOS)
            target_sources(${FreeRTOS_NAMESPACE}::${PORT}${ARMv8_NON_SECURE} INTERFACE "${FreeRTOS_${PORT}_SOURCE}")
            target_include_directories(${FreeRTOS_NAMESPACE}::${PORT}${ARMv8_NON_SECURE} INTERFACE "${FreeRTOS_${PORT}_PATH}")
            message(trace "FindFreeRTOS: creating target ${FreeRTOS_NAMESPACE}::${PORT}${ARMv8_NON_SECURE}")
            
            # armv8-m needs additional file even if using "No Trust Zone" port
            if(${PORT} IN_LIST FreeRTOS_armv8_PORTS)
                target_sources(${FreeRTOS_NAMESPACE}::${PORT}${ARMv8_NON_SECURE} INTERFACE "${FreeRTOS_${PORT}_PATH}/portasm.c")
            endif()

            if(${PORT} IN_LIST FreeRTOS_armv8_trustZone_PORTS)
                # create the secure target
                add_library(${FreeRTOS_NAMESPACE}::${PORT}::SECURE INTERFACE IMPORTED)
                # ::SECURE doesn't link FreeRTOS like ::NON_SECURE does
                target_sources(${FreeRTOS_NAMESPACE}::${PORT}::SECURE INTERFACE "${FreeRTOS_${PORT}_PATH}/../secure/secure_context.c"
                                                                                "${FreeRTOS_${PORT}_PATH}/../secure/secure_context_port.c"
                                                                                "${FreeRTOS_${PORT}_PATH}/../secure/secure_heap.c"
                                                                                "${FreeRTOS_${PORT}_PATH}/../secure/secure_init.c")
                message(trace "FindFreeRTOS: creating target ${FreeRTOS_NAMESPACE}::${PORT}::SECURE")

                # non-secure part needs declaratation from secure includes
                target_include_directories(${FreeRTOS_NAMESPACE}::${PORT}${ARMv8_NON_SECURE} INTERFACE "${FreeRTOS_${PORT}_PATH}/../secure")
                # secure part needs declaratation from non-secure includes and common freeRTOS includes
                target_include_directories(${FreeRTOS_NAMESPACE}::${PORT}::SECURE INTERFACE "${FreeRTOS_${PORT}_PATH}"
                                                                                            "${FreeRTOS_COMMON_INCLUDE}")
            endif()
        endif()
        
        if(FreeRTOS_${PORT}_PATH AND 
           FreeRTOS_${PORT}_SOURCE AND 
           FreeRTOS_COMMON_INCLUDE AND
           FreeRTOS_SOURCE_DIR)
           set(FreeRTOS_${PORT}_FOUND TRUE)
        else()
           set(FreeRTOS_${PORT}_FOUND FALSE)
        endif()
    endforeach()
endmacro()

message(STATUS "Search for FreeRTOS ports: ${FreeRTOS_FIND_COMPONENTS_PORTS}")

if(NOT FreeRTOS_FIND_COMPONENTS_FAMILIES)
    stm32_find_freertos(FreeRTOS ${FREERTOS_PATH})
else()
    message(STATUS "Search for FreeRTOS families: ${FreeRTOS_FIND_COMPONENTS_FAMILIES}")

    foreach(COMP ${FreeRTOS_FIND_COMPONENTS_FAMILIES})
        string(TOLOWER ${COMP} COMP_L)
        string(TOUPPER ${COMP} COMP)
        
        string(REGEX MATCH "^STM32([FGHLMUW]P?[0-9BL])([0-9A-Z][0-9M][A-Z][0-9A-Z])?_?(M0PLUS|M4|M7)?.*$" COMP ${COMP})
        
        if((NOT CMAKE_MATCH_1) AND (NOT CMAKE_MATCH_2))
            message(FATAL_ERROR "Unknown FreeRTOS component: ${COMP}")
        endif()
        
        if(CMAKE_MATCH_2)
            set(FAMILY ${CMAKE_MATCH_1})
            set(STM_DEVICES "${CMAKE_MATCH_1}${CMAKE_MATCH_2}")
        else()
            set(FAMILY ${CMAKE_MATCH_1})
            stm32_get_devices_by_family(STM_DEVICES FAMILY ${FAMILY})
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
        
        string(TOLOWER ${FAMILY} FAMILY_L)
        if(NOT STM32_CUBE_${FAMILY}_PATH)
            set(STM32_CUBE_${FAMILY}_PATH /opt/STM32Cube${FAMILY} CACHE PATH "Path to STM32Cube${FAMILY}")
            message(STATUS "Did not specify STM32_CMSIS_${FAMILY}_PATH, using default STM32_CUBE_${FAMILY}_PATH: ${STM32_CUBE_${FAMILY}_PATH}")
        endif()
        
        stm32_find_freertos(FreeRTOS::STM32::${FAMILY}${CORE_C} ${STM32_CUBE_${FAMILY}_PATH}/Middlewares/Third_Party/FreeRTOS)
        foreach(PORT_COMP ${FreeRTOS_FIND_COMPONENTS_PORTS})
            if(FreeRTOS_${PORT_COMP}_PATH AND 
               FreeRTOS_${PORT_COMP}_SOURCE AND 
               FreeRTOS_COMMON_INCLUDE AND
               FreeRTOS_SOURCE_DIR)
               set(FreeRTOS_${COMP}_FOUND TRUE)
            else()
               set(FreeRTOS_${COMP}_FOUND FALSE)
            endif()
        endforeach()
    endforeach()
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(FreeRTOS
    REQUIRED_VARS FreeRTOS_INCLUDE_DIRS
    FOUND_VAR FreeRTOS_FOUND
    HANDLE_COMPONENTS
)
