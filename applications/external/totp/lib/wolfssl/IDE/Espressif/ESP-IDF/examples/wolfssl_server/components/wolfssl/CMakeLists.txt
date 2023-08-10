#
#  Copyright (C) 2006-2023 wolfSSL Inc.
#
#  This file is part of wolfSSL.
#
#  wolfSSL is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  wolfSSL is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1335, USA
#
# cmake for wolfssl
#
cmake_minimum_required(VERSION 3.5)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DWOLFSSL_USER_SETTINGS")
set(CMAKE_CURRENT_SOURCE_DIR ".")

# We are currently in [root]/IDE/Espressif/ESP-IDF/examples/wolfssl_test/components/wolfssl
# The root of wolfSSL is 7 directories up from here:
get_filename_component(WOLFSSL_ROOT  "../../../../../../../" ABSOLUTE)

# Espressif may take several passes through this makefile. Check to see if we found IDF
string(COMPARE EQUAL "${PROJECT_SOURCE_DIR}" "" WOLFSSL_FOUND_IDF)

if($WOLFSSL_FOUND_IDF)
    message(STATUS "IDF_PATH = $ENV{IDF_PATH}")
    message(STATUS "WOLFSSL_ROOT = ${WOLFSSL_ROOT}")
    message(STATUS "PROJECT_SOURCE_DIR = ${PROJECT_SOURCE_DIR}")
endif()

# get a list of all wolfcrypt assembly files; we'll exclude them as they don't target Xtensa
FILE(GLOB EXCLUDE_ASM *.S)
file(GLOB_RECURSE EXCLUDE_ASM ${CMAKE_SOURCE_DIR} "${WOLFSSL_ROOT}/wolfcrypt/src/*.S")

if(NOT CMAKE_BUILD_EARLY_EXPANSION)
    message(STATUS "EXCLUDE_ASM = ${EXCLUDE_ASM}")
endif()

set(INCLUDE_PATH ${WOLFSSL_ROOT})

set(COMPONENT_SRCDIRS "${WOLFSSL_ROOT}/src/"
                        "${WOLFSSL_ROOT}/wolfcrypt/src/"
                        "${WOLFSSL_ROOT}/wolfcrypt/src/port/Espressif/"
                        "${WOLFSSL_ROOT}/wolfcrypt/src/port/atmel/"
                        "${WOLFSSL_ROOT}/wolfcrypt/benchmark/"
                        "${WOLFSSL_ROOT}/wolfcrypt/test/"
                        )

set(COMPONENT_REQUIRES lwip)


# check to see if there's both a local copy and EDP-IDF copy of the wolfssl and/or wolfssh components
if( EXISTS "${CMAKE_HOME_DIRECTORY}/components/wolfssl/" AND EXISTS "$ENV{IDF_PATH}/components/wolfssl/" )
    #
    # wolfSSL found in both ESP-IDF and local project - needs to be resolved by user
    #
    message(STATUS "")
    message(STATUS "**************************************************************************************")
    message(STATUS "")
    message(STATUS "Error: Found components/wolfssl in both local project and IDF_PATH")
    message(STATUS "")
    message(STATUS "To proceed: ")
    message(STATUS "")
    message(STATUS "Remove either the local project component: ${CMAKE_HOME_DIRECTORY}/components/wolfssl/ ")
    message(STATUS "or the Espressif shared component installed at: $ENV{IDF_PATH}/components/wolfssl/ ")
    message(STATUS "")
    message(FATAL_ERROR "Please use wolfSSL in either local project or Espressif components, but not both.")
    message(STATUS "")
    message(STATUS "**************************************************************************************")
    message(STATUS "")

    # Optional: if you change the above FATAL_ERROR to STATUS you can warn at runtime with this macro definition:
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DWOLFSSL_MULTI_INSTALL_WARNING")

else()
    if( EXISTS "$ENV{IDF_PATH}/components/wolfssl/" )
        #
        # wolfSSL found in ESP-IDF components and is assumed to be already configured in user_settings.h via setup.
        #
        message(STATUS "")
        message(STATUS "Using components/wolfssl in IDF_PATH = $ENV{IDF_PATH}")
        message(STATUS "")
    else()
        #
        # wolfSSL is not an ESP-IDF component. We need to now determine if it is local and if so if it is part of the wolfSSL repo
        # or if  wolfSSL is simply installed as a local component.
        #
        if( EXISTS "${CMAKE_HOME_DIRECTORY}/components/wolfssl/" )
            #
            # wolfSSL found in local project.
            #
            if( EXISTS "${CMAKE_HOME_DIRECTORY}/components/wolfssl/include/" )
                message(STATUS "")
                message(STATUS "Using installed project ./components/wolfssl in CMAKE_HOME_DIRECTORY = $ENV{CMAKE_HOME_DIRECTORY}")
                message(STATUS "")
                #
                # Note we already checked above and confirmed there's not another wolfSSL installed in the ESP-IDF components.
                #
                # We won't do anything else here, as it will be assumed the original install completed successfully.
                #
            else()
                #
                # This is the developer repo mode. wolfSSL will be assume to be not installed to ESP-IDF nor local project
                # In this configuration, we are likely running a wolfSSL example found directly in the repo.
                #
                message(STATUS "")
                message(STATUS "Using developer repo ./components/wolfssl in CMAKE_HOME_DIRECTORY = $ENV{CMAKE_HOME_DIRECTORY}")
                message(STATUS "")

                message(STATUS "************************************************************************************************")
                # When in developer mode, we are typically running wolfSSL examples such as benchmark or test directories.
                # However, the as-cloned or distributed wolfSSL does not have the ./include/ directory, so we'll add it as needed.
                #
                # first check if there's a [root]/include/user_settings.h
                if( EXISTS "${WOLFSSL_ROOT}/include/user_settings.h" )
                    # we won't overwrite an existing user settings file, just note that we already have one:
                    message(STATUS "Found wolfSSL user_settings.h in ${WOLFSSL_ROOT}/include/user_settings.h")
                else()
                    message(STATUS "Installing wolfSSL user_settings.h to ${WOLFSSL_ROOT}/include/user_settings.h")
                    file(COPY "${WOLFSSL_ROOT}/IDE/Espressif/ESP-IDF/user_settings.h" DESTINATION "${WOLFSSL_ROOT}/include/")
                endif() # user_settings.h

                # next check if there's a [root]/include/config.h
                if( EXISTS "${WOLFSSL_ROOT}/include/config.h" )
                    message(STATUS "Found wolfSSL config.h in ${WOLFSSL_ROOT}/include/config.h")
                else()
                    message(STATUS "Installing wolfSSL config.h to ${WOLFSSL_ROOT}/include/config.h")
                    file(COPY "${WOLFSSL_ROOT}/IDE/Espressif/ESP-IDF/dummy_config_h" DESTINATION "${WOLFSSL_ROOT}/include/")
                    file(RENAME "${WOLFSSL_ROOT}/include/dummy_config_h" "${WOLFSSL_ROOT}/include/config.h")
                endif() # config.h
                message(STATUS "************************************************************************************************")
                message(STATUS "")
            endif()

        else()
            # we did not find a ./components/wolfssl/include/ directory from this pass of cmake.
            if($WOLFSSL_FOUND_IDF)
                message(STATUS "")
                message(STATUS "WARNING: wolfSSL not found.")
                message(STATUS "")
            else()
                # probably needs to be re-parsed by Espressif
                message(STATUS "wolfSSL found IDF. Project Source:${PROJECT_SOURCE_DIR}")
            endif() # else we have not found ESP-IDF yet
        endif() # else not a local wolfSSL component

    endif() #else not an ESP-IDF component
endif() # else not local copy and EDP-IDF wolfSSL


# RTOS_IDF_PATH is typically:
# "/Users/{username}/Desktop/esp-idf/components/freertos/include/freertos"
# depending on the environment, we may need to swap backslashes with forward slashes
string(REPLACE "\\" "/" RTOS_IDF_PATH "$ENV{IDF_PATH}/components/freertos/include/freertos")

# ESP-IDF after version 4.4x has a different RTOS directory structure
string(REPLACE "\\" "/" RTOS_IDF_PATH5 "$ENV{IDF_PATH}/components/freertos/FreeRTOS-Kernel/include/freertos")

if(IS_DIRECTORY ${IDF_PATH}/components/freertos/FreeRTOS-Kernel/)
    set(COMPONENT_ADD_INCLUDEDIRS
        "."
       "${WOLFSSL_ROOT}/include"
       "${RTOS_IDF_PATH5}"
       "${WOLFSSL_ROOT}"
       )
else()

   set(COMPONENT_ADD_INCLUDEDIRS
       "."
       "${WOLFSSL_ROOT}/include"
       "${RTOS_IDF_PATH}"
       "${WOLFSSL_ROOT}"
      )
endif()

if(IS_DIRECTORY ${IDF_PATH}/components/cryptoauthlib)
    list(APPEND COMPONENT_ADD_INCLUDEDIRS "../cryptoauthlib/lib")
endif()

set(COMPONENT_SRCEXCLUDE
    "${WOLFSSL_ROOT}/src/bio.c"
    "${WOLFSSL_ROOT}/src/conf.c"
    "${WOLFSSL_ROOT}/src/misc.c"
    "${WOLFSSL_ROOT}/src/pk.c"
    "${WOLFSSL_ROOT}/src/ssl_misc.c" # included by ssl.c
    "${WOLFSSL_ROOT}/src/x509.c"
    "${WOLFSSL_ROOT}/src/x509_str.c"
    "${WOLFSSL_ROOT}/wolfcrypt/src/evp.c"
    "${WOLFSSL_ROOT}/wolfcrypt/src/misc.c"
    "${EXCLUDE_ASM}"
    )

register_component()

# some optional diagnostics
if (0)
    get_cmake_property(_variableNames VARIABLES)
    list (SORT _variableNames)
    message(STATUS "")
    message(STATUS "ALL VARIABLES BEGIN")
    message(STATUS "")
    foreach (_variableName ${_variableNames})
        message(STATUS "${_variableName}=${${_variableName}}")
    endforeach()
    message(STATUS "")
    message(STATUS "ALL VARIABLES END")
    message(STATUS "")
endif()

# check to see if there's both a local copy and EDP-IDF copy of the wolfssl components
if( EXISTS "${CMAKE_HOME_DIRECTORY}/components/wolfssl/" AND EXISTS "$ENV{IDF_PATH}/components/wolfssl/" )
    message(STATUS "")
    message(STATUS "")
    message(STATUS "********************************************************************")
    message(STATUS "WARNING: Found components/wolfssl in both local project and IDF_PATH")
    message(STATUS "********************************************************************")
    message(STATUS "")
endif()
# end multiple component check


