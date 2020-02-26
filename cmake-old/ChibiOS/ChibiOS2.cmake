SET(CHIBIOS_HAL_MODULES adc can ext gpt i2c icu mac mmc_spi mmcsd pal pwm rtc sdc serial serial_usb spi tm uart usb)
SET(CHIBIOS_VARIOUS_MODULES chprintf chrtclib evtimer memstreams shell syscalls fatfs lwip)
SET(CHIBIOS_COMPONENTS kernel hal ${CHIBIOS_HAL_MODULES} ${CHIBIOS_VARIOUS_MODULES})

IF(NOT ChibiOS_FIND_COMPONENTS) 
    SET(ChibiOS_FIND_COMPONENTS kernel)
    MESSAGE(STATUS "No ChibiOS components specified, using default: ${ChibiOS_FIND_COMPONENTS}")
    MESSAGE(STATUS "Supported ChibiOS components: ${CHIBIOS_COMPONENTS}")
ENDIF()

SET(CHIBIOS_HAL_LLD_MODULES adc can ext gpt i2c icu mac pal pwm rtc sdc serial spi uart usb)

SET(CHIBIOS_kernel_SEARCH_PATH 
    ${CHIBIOS_ROOT}/os/kernel/include
    ${CHIBIOS_ROOT}/os/ports/common/ARMCMx
    ${CHIBIOS_ROOT}/os/ports/GCC/ARMCMx
    ${CHIBIOS_ROOT}/os/kernel/src
)
SET(CHIBIOS_kernel_SEARCH_HEADERS
    ch.h
    nvic.h
    chcore.h
)
SET(CHIBIOS_kernel_SOURCES  
    chsys.c
    chdebug.c
    chlists.c
    chvt.c
    chschd.c
    chthreads.c
    chdynamic.c
    chregistry.c
    chsem.c
    chmtx.c
    chcond.c
    chevents.c
    chmsg.c
    chmboxes.c
    chqueues.c
    chmemcore.c
    chheap.c
    chmempools.c
    crt0.c
    chcore.c
    chcore_v7m.c
    nvic.c
)

SET(CHIBIOS_hal_SEARCH_PATH ${CHIBIOS_ROOT}/os/hal/include ${CHIBIOS_ROOT}/os/hal/src)
SET(CHIBIOS_hal_SEARCH_HEADERS hal.h)
SET(CHIBIOS_hal_SOURCES hal.c)

FOREACH(module ${CHIBIOS_HAL_MODULES})
    SET(CHIBIOS_${module}_SEARCH_PATH ${CHIBIOS_ROOT}/os/hal/src)
    SET(CHIBIOS_${module}_SOURCES ${module}.c)
ENDFOREACH()

FOREACH(module ${CHIBIOS_VARIOUS_MODULES})
    IF(${module} STREQUAL fatfs)
        SET(CHIBIOS_${module}_SEARCH_PATH ${CHIBIOS_ROOT}/os/various/fatfs_bindings ${CHIBIOS_ROOT}/ext/fatfs/src/ ${CHIBIOS_ROOT}/ext/fatfs/src/option)
        SET(CHIBIOS_${module}_SOURCES
            fatfs_diskio.c
            fatfs_syscall.c
            ff.c
            ccsbcs.c
        )
        SET(CHIBIOS_${module}_SEARCH_HEADERS ff.h)
    ELSEIF(${module} STREQUAL lwip)
        SET(CHIBIOS_${module}_SEARCH_PATH 
            ${CHIBIOS_ROOT}/os/various/lwip_bindings 
            ${CHIBIOS_ROOT}/os/various/lwip_bindings/arch
            ${CHIBIOS_ROOT}/ext/lwip/src/
            ${CHIBIOS_ROOT}/ext/lwip/src/include/
            ${CHIBIOS_ROOT}/ext/lwip/src/include/ipv4/
        )
        SET(CHIBIOS_${module}_SOURCES
            lwipthread.c
            sys_arch.c
            netif/etharp.c
            core/dhcp.c 
            core/dns.c 
            core/init.c 
            core/mem.c 
            core/memp.c 
            core/netif.c 
            core/pbuf.c 
            core/raw.c 
            core/stats.c 
            core/sys.c 
            core/tcp.c 
            core/tcp_in.c 
            core/tcp_out.c 
            core/udp.c
            core/ipv4/autoip.c 
            core/ipv4/icmp.c 
            core/ipv4/igmp.c 
            core/ipv4/inet.c 
            core/ipv4/inet_chksum.c 
            core/ipv4/ip.c 
            core/ipv4/ip_addr.c 
            core/ipv4/ip_frag.c 
            core/def.c 
            core/timers.c
            api/api_lib.c 
            api/api_msg.c 
            api/err.c 
            api/netbuf.c 
            api/netdb.c 
            api/netifapi.c 
            api/sockets.c 
            api/tcpip.c
        )
        SET(CHIBIOS_${module}_SEARCH_HEADERS 
            lwipthread.h 
            sys_arch.h
            lwip/api.h
            lwip/ip.h
        )
    ELSE()
        SET(CHIBIOS_${module}_SEARCH_PATH ${CHIBIOS_ROOT}/os/various/)
        SET(CHIBIOS_${module}_SOURCES ${module}.c)
        IF(NOT (${module} STREQUAL syscalls))
            SET(CHIBIOS_${module}_SEARCH_HEADERS ${module}.h)
        ENDIF()
    ENDIF()
ENDFOREACH()

IF(STM32_FAMILY STREQUAL "F4")
    SET(CHIBIOS_kernel_PLATFORM_SEARCH_PATH 
        ${CHIBIOS_ROOT}/os/ports/common/ARMCMx/CMSIS/include
        ${CHIBIOS_ROOT}/os/ports/GCC/ARMCMx/STM32F4xx
    )
    SET(CHIBIOS_kernel_PLATFORM_SOURCES
        vectors.c
    ) 
    SET(CHIBIOS_kernel_PLATFORM_SEARCH_HEADERS
        core_cm4.h
        cmparams.h
    )
    
    SET(CHIBIOS_hal_PLATFORM_SEARCH_PATH 
        ${CHIBIOS_ROOT}/os/hal/platforms/STM32F4xx
        ${CHIBIOS_ROOT}/os/hal/platforms/STM32
    )
    SET(CHIBIOS_hal_PLATFORM_SEARCH_HEADERS
        hal_lld.h
        stm32.h
        stm32f4xx.h
    )
    SET(CHIBIOS_hal_PLATFORM_SOURCES  
        hal_lld.c
        stm32_dma.c
    )
    
    SET(CHIBIOS_HAL_PLATFORM_MODULE_PATHES
        STM32F4xx
        STM32
        STM32
        STM32/TIMv1
        STM32/I2Cv1
        STM32/TIMv1
        STM32
        STM32/GPIOv2
        STM32/TIMv1
        STM32/RTCv2
        STM32
        STM32/USARTv1
        STM32/SPIv1
        STM32/USARTv1
        STM32/OTGv1
    )
    
    IF(NOT ChibiOS_LINKER_SCRIPT)
        IF(NOT STM32_CHIP_TYPE)
            STM32_GET_CHIP_TYPE(${STM32_CHIP} STM32_CHIP_TYPE)
        ENDIF()
        IF(NOT STM32_FLASH_SIZE)
            STM32_GET_CHIP_PARAMETERS(${STM32_CHIP} STM32_FLASH_SIZE STM32_RAM_SIZE)
        ENDIF()
        IF(${STM32_CHIP_TYPE} STREQUAL 40_41xxx)
            IF(${STM32_FLASH_SIZE} STREQUAL 1024K)
                FIND_FILE(ChibiOS_LINKER_SCRIPT NAMES STM32F405xG.ld PATHS ${CHIBIOS_ROOT}/os/ports/GCC/ARMCMx/STM32F4xx/ld NO_DEFAULT_PATH CMAKE_FIND_ROOT_PATH_BOTH)
            ENDIF()
        ELSEIF(${STM32_CHIP_TYPE} STREQUAL 429_439xx)
            IF(${STM32_FLASH_SIZE} STREQUAL 2048K)
                FIND_FILE(ChibiOS_LINKER_SCRIPT NAMES STM32F429xI.ld PATHS ${CHIBIOS_ROOT}/os/ports/GCC/ARMCMx/STM32F4xx/ld NO_DEFAULT_PATH CMAKE_FIND_ROOT_PATH_BOTH)
            ENDIF()
        ENDIF()
        
        IF(NOT ChibiOS_LINKER_SCRIPT)
            MESSAGE(STATUS "ChibiOS doesn't have linker script for your chip, please specify it directly using ChibiOS_LINKER_SCRIPT variable.")
        ENDIF()
    ENDIF()
      
ELSEIF(STM32_FAMILY STREQUAL "F1")
    
    SET(CHIBIOS_kernel_PLATFORM_SEARCH_PATH 
        ${CHIBIOS_ROOT}/os/ports/common/ARMCMx/CMSIS/include
        ${CHIBIOS_ROOT}/os/ports/GCC/ARMCMx/STM32F1xx
    )
    SET(CHIBIOS_kernel_PLATFORM_SOURCES
        vectors.c
    ) 
    SET(CHIBIOS_kernel_PLATFORM_SEARCH_HEADERS
        core_cm3.h
        cmparams.h
    )
    
    SET(CHIBIOS_hal_PLATFORM_SEARCH_PATH 
        ${CHIBIOS_ROOT}/os/hal/platforms/STM32F1xx
        ${CHIBIOS_ROOT}/os/hal/platforms/STM32
    )
    SET(CHIBIOS_hal_PLATFORM_SEARCH_HEADERS
        hal_lld.h
        stm32.h
        stm32f10x.h
    )
    SET(CHIBIOS_hal_PLATFORM_SOURCES  
        hal_lld.c
        stm32_dma.c
    )
    
    SET(CHIBIOS_HAL_PLATFORM_MODULE_PATHES
        STM32F1xx
        STM32
        STM32
        STM32/TIMv1
        STM32/I2Cv1
        STM32/TIMv1
        STM32
        STM32/GPIOv1
        STM32/TIMv1
        STM32/RTCv1
        STM32
        STM32/USARTv1
        STM32/SPIv1
        STM32/USARTv1
        STM32/USBv1
    )
            
    IF(NOT ChibiOS_LINKER_SCRIPT)
        IF(NOT STM32_CHIP_TYPE)
            STM32_GET_CHIP_TYPE(${STM32_CHIP} STM32_CHIP_TYPE)
        ENDIF()
        IF(NOT STM32_FLASH_SIZE)
            STM32_GET_CHIP_PARAMETERS(${STM32_CHIP} STM32_FLASH_SIZE STM32_RAM_SIZE)
        ENDIF()
        IF(${STM32_CHIP_TYPE} STREQUAL MD_VL)
            IF(${STM32_FLASH_SIZE} STREQUAL 128K)
                FIND_FILE(ChibiOS_LINKER_SCRIPT NAMES STM32F100xB.ld PATHS ${CHIBIOS_ROOT}/os/ports/GCC/ARMCMx/STM32F1xx/ld NO_DEFAULT_PATH CMAKE_FIND_ROOT_PATH_BOTH)
            ENDIF()
        ELSEIF(${STM32_CHIP_TYPE} STREQUAL MD)
            IF(${STM32_FLASH_SIZE} STREQUAL 128K)
                FIND_FILE(ChibiOS_LINKER_SCRIPT NAMES STM32F103xB.ld PATHS ${CHIBIOS_ROOT}/os/ports/GCC/ARMCMx/STM32F1xx/ld NO_DEFAULT_PATH CMAKE_FIND_ROOT_PATH_BOTH)
            ENDIF()
        ELSEIF(${STM32_CHIP_TYPE} STREQUAL HD)
            IF(${STM32_FLASH_SIZE} STREQUAL 384K)
                FIND_FILE(ChibiOS_LINKER_SCRIPT NAMES STM32F103xD.ld PATHS ${CHIBIOS_ROOT}/os/ports/GCC/ARMCMx/STM32F1xx/ld NO_DEFAULT_PATH CMAKE_FIND_ROOT_PATH_BOTH)
            ELSEIF(${STM32_FLASH_SIZE} STREQUAL 512K)
                FIND_FILE(ChibiOS_LINKER_SCRIPT NAMES STM32F103xE.ld PATHS ${CHIBIOS_ROOT}/os/ports/GCC/ARMCMx/STM32F1xx/ld NO_DEFAULT_PATH CMAKE_FIND_ROOT_PATH_BOTH)
            ENDIF()
        ELSEIF(${STM32_CHIP_TYPE} STREQUAL XL)
            IF(${STM32_FLASH_SIZE} STREQUAL 1024K)
                FIND_FILE(ChibiOS_LINKER_SCRIPT NAMES STM32F103xG.ld PATHS ${CHIBIOS_ROOT}/os/ports/GCC/ARMCMx/STM32F1xx/ld NO_DEFAULT_PATH CMAKE_FIND_ROOT_PATH_BOTH)
            ENDIF()
        ENDIF()
    ENDIF()            
ENDIF()

IF(NOT ChibiOS_LINKER_SCRIPT)
    MESSAGE(STATUS "ChibiOS doesn't have linker script for your chip, please specify it directly using ChibiOS_LINKER_SCRIPT variable.")
ENDIF()

SET(INDEX 0)
FOREACH(module ${CHIBIOS_HAL_LLD_MODULES})
    LIST(GET CHIBIOS_HAL_PLATFORM_MODULE_PATHES ${INDEX} path)
        
    SET(CHIBIOS_${module}_PLATFORM_SEARCH_PATH ${CHIBIOS_ROOT}/os/hal/platforms/${path})
    SET(CHIBIOS_${module}_PLATFORM_SEARCH_HEADERS ${module}_lld.h)
    SET(CHIBIOS_${module}_PLATFORM_SOURCES ${module}_lld.c)
        
    IF(${module} STREQUAL ext)
       SET(CHIBIOS_${module}_PLATFORM_SEARCH_HEADERS ${CHIBIOS_ext_PLATFORM_SEARCH_HEADERS} ext_lld_isr.h)
       SET(CHIBIOS_${module}_PLATFORM_SOURCES ${CHIBIOS_ext_PLATFORM_SOURCES} ext_lld_isr.c)
    ENDIF()
        
    MATH(EXPR INDEX "${INDEX} + 1")
ENDFOREACH()

IF(STM32_FAMILY STREQUAL "F4")
    SET(CHIBIOS_ext_PLATFORM_SEARCH_PATH ${CHIBIOS_ext_PLATFORM_SEARCH_PATH} ${CHIBIOS_ROOT}/os/hal/platforms/STM32F4xx)
ELSEIF(STM32_FAMILY STREQUAL "F1")
    SET(CHIBIOS_ext_PLATFORM_SEARCH_PATH ${CHIBIOS_ext_PLATFORM_SEARCH_PATH} ${CHIBIOS_ROOT}/os/hal/platforms/STM32F1xx)
ENDIF()

FOREACH(comp ${ChibiOS_FIND_COMPONENTS}) 
    LIST(FIND CHIBIOS_COMPONENTS ${comp} INDEX)
    IF(INDEX EQUAL -1)
        MESSAGE(FATAL_ERROR "Unknown ChibiOS component: ${comp}\nSupported ChibiOS components: ${CHIBIOS_COMPONENTS}")
    ENDIF()
    FOREACH(source ${CHIBIOS_${comp}_SOURCES})
        FIND_FILE(CHIBIOS_${comp}_${source} NAMES ${source} PATHS ${CHIBIOS_${comp}_SEARCH_PATH} NO_DEFAULT_PATH CMAKE_FIND_ROOT_PATH_BOTH)
        LIST(APPEND ChibiOS_SOURCES ${CHIBIOS_${comp}_${source}})
    ENDFOREACH()
    IF(CHIBIOS_${comp}_SEARCH_HEADERS)
        FOREACH(header ${CHIBIOS_${comp}_SEARCH_HEADERS})
            FIND_PATH(CHIBIOS_${comp}_${header}_INCLUDE_DIR NAMES ${header} PATHS ${CHIBIOS_${comp}_SEARCH_PATH} NO_DEFAULT_PATH CMAKE_FIND_ROOT_PATH_BOTH)
            LIST(APPEND ChibiOS_INCLUDE_DIRS ${CHIBIOS_${comp}_${header}_INCLUDE_DIR})
        ENDFOREACH()
    ENDIF()
    IF(CHIBIOS_${comp}_PLATFORM_SEARCH_HEADERS)
        FOREACH(header ${CHIBIOS_${comp}_PLATFORM_SEARCH_HEADERS})
            FIND_PATH(CHIBIOS_${comp}_PLATFORM_${header}_INCLUDE_DIR NAMES ${header} PATHS ${CHIBIOS_${comp}_PLATFORM_SEARCH_PATH} NO_DEFAULT_PATH CMAKE_FIND_ROOT_PATH_BOTH)
            LIST(APPEND ChibiOS_INCLUDE_DIRS ${CHIBIOS_${comp}_PLATFORM_${header}_INCLUDE_DIR})
        ENDFOREACH()
    ENDIF()
    IF(CHIBIOS_${comp}_PLATFORM_SOURCES)
        FOREACH(source ${CHIBIOS_${comp}_PLATFORM_SOURCES})
            FIND_FILE(CHIBIOS_${comp}_PLATFORM_${source} NAMES ${source} PATHS ${CHIBIOS_${comp}_PLATFORM_SEARCH_PATH} NO_DEFAULT_PATH CMAKE_FIND_ROOT_PATH_BOTH)
            LIST(APPEND ChibiOS_SOURCES ${CHIBIOS_${comp}_PLATFORM_${source}})
        ENDFOREACH()
    ENDIF()
ENDFOREACH()

