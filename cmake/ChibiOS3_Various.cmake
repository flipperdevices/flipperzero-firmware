SET(CHIBIOS_VARIOUS_MODULES evtimer shell syscalls) 

SET(CHIBIOS_evtimer_SEARCH_PATH ${CHIBIOS_ROOT}/os/various)
SET(CHIBIOS_evtimer_SOURCES evtimer.c)
SET(CHIBIOS_evtimer_SEARCH_HEADERS evtimer.h)

SET(CHIBIOS_shell_SEARCH_PATH ${CHIBIOS_ROOT}/os/various)
SET(CHIBIOS_shell_SOURCES shell.c)
SET(CHIBIOS_shell_SEARCH_HEADERS shell.h)

SET(CHIBIOS_syscalls_SEARCH_PATH ${CHIBIOS_ROOT}/os/various)
SET(CHIBIOS_syscalls_SOURCES syscalls.c)
