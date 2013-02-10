# GCC toolchain prefix
IF(NOT TOOLCHAIN_PREFIX)
     SET(TOOLCHAIN_PREFIX "/opt/arm-2011.09")
     MESSAGE(STATUS "No TOOLCHAIN_PREFIX specified, using default: " ${TOOLCHAIN_PREFIX})
ENDIF()

IF(NOT TARGET_TRIPLET)
    SET(TARGET_TRIPLET "arm-none-eabi")
    MESSAGE(STATUS "No TARGET_TRIPLET specified, using default: " ${TARGET_TRIPLET})
ENDIF()

SET(TOOLCHAIN_BIN_DIR ${TOOLCHAIN_PREFIX}/bin)
SET(TOOLCHAIN_INC_DIR ${TOOLCHAIN_PREFIX}/${TARGET_TRIPLET}/include)
SET(TOOLCHAIN_LIB_DIR ${TOOLCHAIN_PREFIX}/${TARGET_TRIPLET}/lib)

SET(CMAKE_SYSTEM_NAME Generic CACHE INTERNAL "system name")
SET(CMAKE_SYSTEM_PROCESSOR arm CACHE INTERNAL "processor")

SET(CMAKE_C_COMPILER ${TOOLCHAIN_BIN_DIR}/${TARGET_TRIPLET}-gcc CACHE INTERNAL "c compiler")
SET(CMAKE_CXX_COMPILER ${TOOLCHAIN_BIN_DIR}/${TARGET_TRIPLET}-g++ CACHE INTERNAL "cxx compiler")
SET(CMAKE_ASM_COMPILER ${TOOLCHAIN_BIN_DIR}/${TARGET_TRIPLET}-as CACHE INTERNAL "asm compiler")

SET(CMAKE_OBJCOPY ${TOOLCHAIN_BIN_DIR}/${TARGET_TRIPLET}-objcopy CACHE INTERNAL "objcopy")
SET(CMAKE_OBJDUMP ${TOOLCHAIN_BIN_DIR}/${TARGET_TRIPLET}-objdump CACHE INTERNAL "objdump")

SET(CMAKE_C_FLAGS "-isystem ${TOOLCHAIN_INC_DIR} -mthumb -mcpu=cortex-m3 -fno-builtin -Wall -std=gnu99 -fdata-sections -ffunction-sections" CACHE INTERNAL "c compiler flags")
SET(CMAKE_CXX_FLAGS "-isystem ${TOOLCHAIN_INC_DIR} -mthumb -mcpu=cortex-m3 -fno-builtin -Wall  -fdata-sections -ffunction-sections" CACHE INTERNAL "cxx compiler flags")
SET(CMAKE_ASM_FLAGS "-mthumb -mcpu=cortex-m3" CACHE INTERNAL "asm compiler flags")

SET(CMAKE_C_FLAGS_DEBUG "-O0 -g -gstabs+" CACHE INTERNAL "c debug compiler flags")
SET(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -gstabs+" CACHE INTERNAL "cxx debug compiler flags")
SET(CMAKE_ASM_FLAGS_DEBUG "-g -gstabs+" CACHE INTERNAL "asm debug compiler flags")

SET(CMAKE_C_FLAGS_RELEASE "-Os -flto" CACHE INTERNAL "c release compiler flags")
SET(CMAKE_CXX_FLAGS_RELEASE "-Os -flto" CACHE INTERNAL "cxx release compiler flags")
SET(CMAKE_ASM_FLAGS_RELEASE "" CACHE INTERNAL "asm release compiler flags")

SET(CMAKE_EXE_LINKER_FLAGS "-nostartfiles -Wl,--gc-sections -mthumb -mcpu=cortex-m3 -flto" CACHE INTERNAL "exe link flags")
SET(CMAKE_MODULE_LINKER_FLAGS "-L${TOOLCHAIN_LIB_DIR} -mthumb -mcpu=cortex-m3 -flto" CACHE INTERNAL "module link flags")
SET(CMAKE_SHARED_LINKER_FLAGS "-L${TOOLCHAIN_LIB_DIR} -mthumb -mcpu=cortex-m3 -flto" CACHE INTERNAL "shared link flags")

SET(CMAKE_FIND_ROOT_PATH ${TOOLCHAIN_PREFIX}/${TARGET_TRIPLET} CACHE INTERNAL "cross root directory")
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM BOTH CACHE INTERNAL "")
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY CACHE INTERNAL "")
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY CACHE INTERNAL "")
