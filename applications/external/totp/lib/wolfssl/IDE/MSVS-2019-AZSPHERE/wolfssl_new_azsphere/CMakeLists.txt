# CMakeList.txt
#
# Copyright (C) 2006-2022 wolfSSL Inc.
#
# This file is part of wolfSSL. (formerly known as CyaSSL)
#
# Usage:
# In MSVS 2019 with Azure Sphere plugin installed choose option
# "Open Local Folder" and browse to wolfssl-root/IDE/MSVS-2019-AZSPHERE"
# highlight wolfssl_new_azsphere and select "Open"
#
# To build:
# go to MSVS menu option "Build" and select "Build All"
#
# See https://docs.microsoft.com/en-us/azure-sphere/install/overview for how
# to get started with an Azure sphere device. Once you have completed the step
# to build/flash and run the Blinky example you will be ready to build/flash
# and run this example!

cmake_minimum_required (VERSION 3.4.1)

project (wolfssl_new_azsphere C)

azsphere_configure_tools(TOOLS_REVISION "22.02")
azsphere_configure_api(TARGET_API_SET "12")

# add wolfSSL preprocessor directives
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DWOLFSSL_USER_SETTINGS -Wno-conversion -Wno-sign-conversion")

# add a shared object for SSL/TLS and CRYPTO
set( SSL_SRC_DIR "../../../src" )
set( CRYPTO_SRC_DIR "../../../wolfcrypt/src" )
set( INCLUDE_DIR "../../.." )
aux_source_directory( ${SSL_SRC_DIR} SSL_SOURCES )
list( REMOVE_ITEM SSL_SOURCES ../../../src/bio.c )
list( REMOVE_ITEM SSL_SOURCES ../../../src/conf.c )
list( REMOVE_ITEM SSL_SOURCES ../../../src/x509.c )
list( REMOVE_ITEM SSL_SOURCES ../../../src/x509_str.c )
list( REMOVE_ITEM SSL_SOURCES ../../../src/pk.c )
list( REMOVE_ITEM SSL_SOURCES ../../../src/ssl_misc.c )
aux_source_directory( ${CRYPTO_SRC_DIR} CRYPTO_SOURCES )
list( REMOVE_ITEM CRYPTO_SOURCES ../../../wolfcrypt/src/evp.c )
list( REMOVE_ITEM CRYPTO_SOURCES ../../../wolfcrypt/src/misc.c )
add_library( wolfssl STATIC ${SSL_SOURCES} ${CRYPTO_SOURCES} )

# Create executable that uses library
set( SERVER_APP_DIR "../server" )
set( CLIENT_APP_DIR "../client" )
set( USER_SETTINGS_DIR ".." )
set( APP_INCLUDE_DIRS ${SERVER_APP_DIR} ${CLIENT_APP_DIR} ${USER_SETTINGS_DIR} )

add_executable (${PROJECT_NAME} main.c ${SERVER_APP_DIR}/server.c ${CLIENT_APP_DIR}/client.c )
target_link_libraries (${PROJECT_NAME} applibs pthread gcc_s c wolfssl)
include_directories( ${INCLUDE_DIR} ${APP_INCLUDE_DIRS})

# azsphere_target_hardware_definition(${PROJECT_NAME} TARGET_DIRECTORY "HardwareDefinitions/mt3620_rdb" TARGET_DEFINITION "template_appliance.json")
azsphere_target_hardware_definition(${PROJECT_NAME} TARGET_DIRECTORY "HardwareDefinitions/seeed_mt3620_mdb" TARGET_DEFINITION "template_appliance.json")

azsphere_target_add_image_package(${PROJECT_NAME})
