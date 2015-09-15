 IF(NOT CHIBIOS_PROCESS_STACK_SIZE)
  SET(CHIBIOS_PROCESS_STACK_SIZE 0x400)
  MESSAGE(STATUS "No CHIBIOS_PROCESS_STACK_SIZE specified, using default: ${CHIBIOS_PROCESS_STACK_SIZE}")
ENDIF()

IF(NOT CHIBIOS_MAIN_STACK_SIZE)
  SET(CHIBIOS_MAIN_STACK_SIZE 0x400)
  MESSAGE(STATUS "No CHIBIOS_MAIN_STACK_SIZE specified, using default: ${CHIBIOS_MAIN_STACK_SIZE}")
ENDIF()

SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L\"${CHIBIOS_ROOT}/os/common/ports/ARMCMx/compilers/GCC\"")
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--defsym=__process_stack_size__=${CHIBIOS_PROCESS_STACK_SIZE}")
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--defsym=__main_stack_size__=${CHIBIOS_MAIN_STACK_SIZE}")

# Auto-generate linker script
IF(NOT ChibiOS_LINKER_SCRIPT)
    FILE(WRITE ${CMAKE_BINARY_DIR}/chibios_link.ld.in 
      "MEMORY\n"
      "{\n"
      "  flash : org = 0x08000000, len = \${STM32_FLASH_SIZE}\n"
      "  ram0 : org = 0x20000000, len = \${STM32_RAM_SIZE}\n"
      "  ram1 : org = 0x00000000, len = 0\n"
      "  ram2 : org = 0x00000000, len = 0\n"
      "  ram3 : org = 0x00000000, len = 0\n"
      "  ram4 : org = \${STM32_CCRAM_ORIGIN}, len = \${STM32_CCRAM_SIZE}\n"
      "  ram5 : org = 0x00000000, len = 0\n"
      "  ram6 : org = 0x00000000, len = 0\n"
      "  ram7 : org = 0x00000000, len = 0\n"
      "}\n"
      "REGION_ALIAS(\"MAIN_STACK_RAM\", ram0);\n"
      "REGION_ALIAS(\"PROCESS_STACK_RAM\", ram0);\n"
      "REGION_ALIAS(\"DATA_RAM\", ram0);\n"
      "REGION_ALIAS(\"BSS_RAM\", ram0);\n"
      "REGION_ALIAS(\"HEAP_RAM\", ram0);\n"
      "INCLUDE rules.ld\n"      
    )
    SET(ChibiOS_LINKER_SCRIPT ${CMAKE_BINARY_DIR}/chibios_link.ld.in)
ENDIF()     