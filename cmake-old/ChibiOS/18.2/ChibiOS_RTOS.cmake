FOREACH (FAMILY F0 L0 L4)
    SET (CHIBIOS_SOURCES_${CHIBIOS_KERNEL}_${FAMILY}
        os/common/startup/ARMCMx/compilers/GCC/crt0_v6m.S
        os/common/ports/ARMCMx/chcore.c
        os/common/ports/ARMCMx/chcore_v6m.c
        os/common/ports/ARMCMx/compilers/GCC/chcoreasm_v6m.S
    )
ENDFOREACH()

FOREACH (FAMILY F1 F2 F3 F4 F7)
    SET (CHIBIOS_SOURCES_${CHIBIOS_KERNEL}_${FAMILY}
        os/common/startup/ARMCMx/compilers/GCC/crt0_v7m.S
        os/common/ports/ARMCMx/chcore.c
        os/common/ports/ARMCMx/chcore_v7m.c
        os/common/ports/ARMCMx/compilers/GCC/chcoreasm_v7m.S
    )
ENDFOREACH()

FOREACH (FAMILY F0 F1 F2 F3 F4 F7 L0 L1)
    SET (CHIBIOS_INCLUDES_${CHIBIOS_KERNEL}_${FAMILY}
        os/common/startup/ARMCMx/devices/STM32${FAMILY}xx
        os/common/ext/ST/STM32${FAMILY}xx
        os/common/oslib/include
        os/common/ports/ARMCMx
        os/common/ports/ARMCMx/compilers/GCC
    )
ENDFOREACH()

SET (CHIBIOS_SOURCES_${CHIBIOS_KERNEL}
    os/common/startup/ARMCMx/compilers/GCC/crt1.c
    os/common/startup/ARMCMx/compilers/GCC/vectors.S
)

SET (CHIBIOS_INCLUDES_${CHIBIOS_KERNEL}
    os/license
    os/common/portability/GCC
    os/common/startup/ARMCMx/compilers/GCC
    os/common/ext/ARM/CMSIS/Core/Include
)

SET (CHIBIOS_SOURCES_${CHIBIOS_KERNEL}_MAILBOXES  os/common/oslib/src/chmboxes.c)
SET (CHIBIOS_SOURCES_${CHIBIOS_KERNEL}_MEMCORE    os/common/oslib/src/chmemcore.c)
SET (CHIBIOS_SOURCES_${CHIBIOS_KERNEL}_HEAP       os/common/oslib/src/chheap.c)
SET (CHIBIOS_SOURCES_${CHIBIOS_KERNEL}_MEMPOOLS   os/common/oslib/src/chmempools.c)
SET (CHIBIOS_SOURCES_${CHIBIOS_KERNEL}_FACTORY    os/common/oslib/src/chfactory.c)

SET (CHIBIOS_SOURCES_rt_TM          os/rt/src/chtm.c)
SET (CHIBIOS_SOURCES_rt_REGISTRY    os/rt/src/chregistry.c)
SET (CHIBIOS_SOURCES_rt_SEMAPHORES  os/rt/src/chsem.c)
SET (CHIBIOS_SOURCES_rt_MUTEXES     os/rt/src/chmtx.c)
SET (CHIBIOS_SOURCES_rt_CONDVARS    os/rt/src/chcond.c)
SET (CHIBIOS_SOURCES_rt_EVENTS      os/rt/src/chevents.c)
SET (CHIBIOS_SOURCES_rt_MESSAGES    os/rt/src/chmsg.c)
SET (CHIBIOS_SOURCES_rt_DYNAMIC     os/rt/src/chdynamic.c)

LIST (APPEND CHIBIOS_SOURCES_nil     os/nil/src/ch.c)
LIST (APPEND CHIBIOS_INCLUDES_nil    os/nil/include)

LIST (APPEND CHIBIOS_SOURCES_rt
    os/rt/src/chsys.c
    os/rt/src/chdebug.c
    os/rt/src/chtrace.c
    os/rt/src/chvt.c
    os/rt/src/chschd.c
    os/rt/src/chthreads.c
)

LIST (APPEND CHIBIOS_INCLUDES_rt     os/rt/include)

IF (CHIBIOS_SOURCES_${CHIBIOS_KERNEL}_${STM32_FAMILY})
    LIST(APPEND CHIBIOS_SOURCES_${CHIBIOS_KERNEL} ${CHIBIOS_SOURCES_${CHIBIOS_KERNEL}_${STM32_FAMILY}})
ENDIF()

IF (CHIBIOS_INCLUDES_${CHIBIOS_KERNEL}_${STM32_FAMILY})
    LIST(APPEND CHIBIOS_INCLUDES_${CHIBIOS_KERNEL} ${CHIBIOS_INCLUDES_${CHIBIOS_KERNEL}_${STM32_FAMILY}})
ENDIF()

FOREACH (COMP ${CHIBIOS_RTOS_COMPONENTS})
    IF (CHIBIOS_SOURCES_${CHIBIOS_KERNEL}_${COMP})
        LIST(APPEND CHIBIOS_SOURCES_${CHIBIOS_KERNEL} ${CHIBIOS_SOURCES_${CHIBIOS_KERNEL}_${COMP}})
    ENDIF()
    IF (CHIBIOS_INCLUDES_${CHIBIOS_KERNEL}_${COMP})
        LIST(APPEND CHIBIOS_INCLUDES_${CHIBIOS_KERNEL} ${CHIBIOS_INCLUDES_${CHIBIOS_KERNEL}_${COMP}})
    ENDIF()

    IF (CHIBIOS_SOURCES_${CHIBIOS_KERNEL}_${COMP}_${STM32_FAMILY})
        LIST(APPEND CHIBIOS_SOURCES_${CHIBIOS_KERNEL} ${CHIBIOS_SOURCES_${CHIBIOS_KERNEL}_${COMP}_${STM32_FAMILY}})
    ENDIF()
    IF (CHIBIOS_INCLUDES_${CHIBIOS_KERNEL}_${COMP}_${STM32_FAMILY})
        LIST(APPEND CHIBIOS_INCLUDES_${CHIBIOS_KERNEL} ${CHIBIOS_INCLUDES_${CHIBIOS_KERNEL}_${COMP}_${STM32_FAMILY}})
    ENDIF()
ENDFOREACH()
