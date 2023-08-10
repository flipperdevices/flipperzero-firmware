#
#  Copyright (C) 2006-2022 wolfSSL Inc.
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
set(WOLFSSL_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(INCLUDE_PATH ${WOLFSSL_ROOT})

set(COMPONENT_SRCDIRS "./src/"
                      "./wolfcrypt/src/"
                      "./wolfcrypt/src/port/Espressif/"
                      "./wolfcrypt/src/port/atmel/"
                      "./wolfcrypt/benchmark/"
                      "./wolfcrypt/test/"
#                      "$ENV{IDF_PATH}/components/soc/esp32s3/include/soc"
                      )

set(COMPONENT_REQUIRES lwip)


# RTOS_IDF_PATH is typically:
# "/Users/{username}/Desktop/esp-idf/components/freertos/include/freertos"
# depending on the environment, we may need to swap backslashes with forward slashes
string(REPLACE "\\" "/" RTOS_IDF_PATH "$ENV{IDF_PATH}/components/freertos/include/freertos")
# ESP-IDF after version 4.4x has a different RTOS directory structure
string(REPLACE "\\" "/" RTOS_IDF_PATH5 "$ENV{IDF_PATH}/components/freertos/FreeRTOS-Kernel/include/freertos")

if(IS_DIRECTORY ${IDF_PATH}/components/freertos/FreeRTOS-Kernel/)
    set(COMPONENT_ADD_INCLUDEDIRS
        "."
        "./include"
       "${RTOS_IDF_PATH5}"
       "${WOLFSSL_ROOT}"
       )
else()

   set(COMPONENT_ADD_INCLUDEDIRS
       "."
       "./include"
       "${RTOS_IDF_PATH}"
       "${WOLFSSL_ROOT}"
      )
endif()

if(IS_DIRECTORY ${IDF_PATH}/components/cryptoauthlib)
    list(APPEND COMPONENT_ADD_INCLUDEDIRS "../cryptoauthlib/lib")
endif()

set(COMPONENT_SRCEXCLUDE
    "./src/bio.c"
    "./src/conf.c"
    "./src/misc.c"
    "./src/pk.c"
    "./src/ssl_misc.c" # included by ssl.c
    "./src/x509.c"
    "./src/x509_str.c"
    "./wolfcrypt/src/evp.c"
    "./wolfcrypt/src/misc.c"
    )

register_component()

# check to see if there's both a local copy and EDP-IDF copy of the wolfssl components
if( EXISTS "${CMAKE_HOME_DIRECTORY}/components/wolfssl/" AND EXISTS "$ENV{IDF_PATH}/components/wolfssl/" )
    message(STATUS  "")
    message(STATUS  "WARNING: Found components/wolfssl in both local project and IDF_PATH")
    message(STATUS  "")
endif()
# end multiple component check
