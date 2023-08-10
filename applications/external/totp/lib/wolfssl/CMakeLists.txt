# CMakeList.txt
#
# Copyright (C) 2006-2023 wolfSSL Inc.
#
# This file is part of wolfSSL. (formerly known as CyaSSL)
#
# Usage:
# $ mkdir build
# $ cd build
# $ cmake ..
# $ cmake --build .
#
# To build with debugging use:
# $ cmake .. -DCMAKE_BUILD_TYPE=Debug
#
# See "Building with CMake" in INSTALL for more.

####################################################
# Project
####################################################

cmake_minimum_required(VERSION 3.16)

if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
    message(FATAL_ERROR "In-source builds are not allowed.\
     Run cmake from a separate directory from where CMakeLists.txt lives.\
     NOTE: cmake will now create CMakeCache.txt and CMakeFiles/*.\
     You must delete them, or cmake will refuse to work.")
endif()

project(wolfssl VERSION 5.6.3 LANGUAGES C ASM)

# shared library versioning
# increment if interfaces have been added, removed or changed
set(LIBTOOL_CURRENT 40)
# increment if source code has changed  set to zero if current is incremented
set(LIBTOOL_REVISION 1)
# increment if interfaces have been added set to zero if interfaces have been
# removed or changed
set(LIBTOOL_AGE 5)

math(EXPR LIBTOOL_SO_VERSION "${LIBTOOL_CURRENT} - ${LIBTOOL_AGE}")
set(LIBTOOL_FULL_VERSION ${LIBTOOL_SO_VERSION}.${LIBTOOL_AGE}.${LIBTOOL_REVISION})

set(WOLFSSL_DEFINITIONS)
set(WOLFSSL_LINK_LIBS)
set(WOLFSSL_INCLUDE_DIRS)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/")
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/functions.cmake)

####################################################
# Compiler
####################################################
# Let CMake choose default compiler

# TODO: See gl_VISIBILITY in visibility.m4. Need to perform
#       the same checks.
# TODO: Turn on warnings.

if(CMAKE_C_COMPILER_ID STREQUAL "AppleClang")
    # Silence ranlib warning "has no symbols"
    set(CMAKE_C_ARCHIVE_CREATE   "<CMAKE_AR> Scr <TARGET> <LINK_FLAGS> <OBJECTS>")
    set(CMAKE_CXX_ARCHIVE_CREATE "<CMAKE_AR> Scr <TARGET> <LINK_FLAGS> <OBJECTS>")
    set(CMAKE_C_ARCHIVE_FINISH   "<CMAKE_RANLIB> -no_warning_for_no_symbols -c <TARGET>")
    set(CMAKE_CXX_ARCHIVE_FINISH "<CMAKE_RANLIB> -no_warning_for_no_symbols -c <TARGET>")
endif()

include(CheckIncludeFile)

check_include_file("arpa/inet.h" HAVE_ARPA_INET_H)
check_include_file("fcntl.h" HAVE_FCNTL_H)
check_include_file("limits.h" HAVE_LIMITS_H)
check_include_file("netdb.h" HAVE_NETDB_H)
check_include_file("netinet/in.h" HAVE_NETINET_IN_H)
check_include_file("stddef.h" HAVE_STDDEF_H)
check_include_file("time.h" HAVE_TIME_H)
check_include_file("sys/ioctl.h" HAVE_SYS_IOCTL_H)
check_include_file("sys/socket.h" HAVE_SYS_SOCKET_H)
check_include_file("sys/time.h" HAVE_SYS_TIME_H)
check_include_file("errno.h" HAVE_ERRNO_H)
check_include_file("dlfcn.h" HAVE_DLFCN_H)
check_include_file("inttypes.h" HAVE_INTTYPES_H)
check_include_file("memory.h" HAVE_MEMORY_H)
check_include_file("stdint.h" HAVE_STDINT_H)
check_include_file("stdlib.h" HAVE_STDLIB_H)
check_include_file("string.h" HAVE_STRING_H)
check_include_file("strings.h" HAVE_STRINGS_H)
check_include_file("sys/stat.h" HAVE_SYS_STAT_H)
check_include_file("sys/types.h" HAVE_SYS_TYPES_H)
check_include_file("unistd.h" HAVE_UNISTD_H)

include(CheckFunctionExists)

# TODO: Also check if these functions are declared by the
#       expected headers. See comments around
#       AC_CHECK_FUNCS/AC_CHECK_DECLS in configure.ac.
check_function_exists("gethostbyname" HAVE_GETHOSTBYNAME)
check_function_exists("getaddrinfo" HAVE_GETADDRINFO)
check_function_exists("gettimeofday" HAVE_GETTIMEOFDAY)
check_function_exists("gmtime_r" HAVE_GMTIME_R)
check_function_exists("inet_ntoa" HAVE_INET_NTOA)
check_function_exists("memset" HAVE_MEMSET)
check_function_exists("socket" HAVE_SOCKET)
check_function_exists("strftime" HAVE_STRFTIME)
check_function_exists("__atomic_fetch_add" HAVE_C___ATOMIC)

include(CheckTypeSize)

check_type_size("__uint128_t" __UINT128_T)
check_type_size("long long" SIZEOF_LONG_LONG)
check_type_size("long" SIZEOF_LONG)
check_type_size("time_t" SIZEOF_TIME_T)

# By default, HAVE___UINT128_T gets defined as TRUE,
# but we want it as 1.
if(HAVE___UINT128_T)
    set(HAVE___UINT128_T "1" CACHE INTERNAL "Result of TRY_COMPILE" FORCE)
endif()

include(TestBigEndian)

test_big_endian(WORDS_BIGENDIAN)

# Thread local storage
include(CheckCSourceCompiles)

set(TLS_KEYWORDS "__thread" "__declspec(thread)")
foreach(TLS_KEYWORD IN LISTS TLS_KEYWORDS)
    set(TLS_CODE "#include <stdlib.h>
                  static void foo(void) {
                     static ${TLS_KEYWORD} int bar\;
                     exit(1)\;
                  }

                  int main() {
                    return 0\;
                  }"
    )
    check_c_source_compiles(${TLS_CODE} THREAD_LS_ON)

    if(THREAD_LS_ON)
        list(APPEND WOLFSSL_DEFINITIONS "-DHAVE_THREAD_LS")
        break()
    else()
        # THREAD_LS_ON is cached after each call to
        # check_c_source_compiles, and the function
        # won't run subsequent times if the variable
        # is in the cache. To make it run again, we
        # need to remove the variable from the cache.
        unset(THREAD_LS_ON CACHE)
    endif()
endforeach()

# TODO: AX_PTHREAD does a lot. Need to implement the
#       rest of its logic.
find_package(Threads)

####################################################
# Cross Compile Example
####################################################

#set(CMAKE_SYSTEM_NAME Linux)
#set(CMAKE_SYSTEM_PROCESSOR arm)
#set(CMAKE_C_COMPILER "/opt/arm-linux-musleabihf-cross/bin/arm-linux-musleabihf-gcc")
#set(CMAKE_CXX_COMPILER "/opt/arm-linux-musleabihf-cross/bin/arm-linux-musleabihf-g++")
#set(CMAKE_SYSROOT "/opt/arm-linux-musleabihf-cross/arm-linux-musleabihf/")
# Example for setting CFLAGS
#set(CMAKE_C_FLAGS "-std=gnu89 ${CMAKE_C_FLAGS}")
# Example for map file and custom linker script
#set(CMAKE_EXE_LINKER_FLAGS " -Xlinker -Map=output.map -T\"${CMAKE_CURRENT_SOURCE_DIR}/linker.ld\"")

if(DEFINED WARNING_C_FLAGS)
set(CMAKE_C_FLAGS "${WARNING_C_FLAGS} ${CMAKE_C_FLAGS}")
elseif(WIN32)
# Windows cl.exe does not support the -Wextra, -Wno-unused and -Werror flags.
set(CMAKE_C_FLAGS "-Wall ${CMAKE_C_FLAGS}")
else()
set(CMAKE_C_FLAGS "-Wall -Wextra -Wno-unused -Werror ${CMAKE_C_FLAGS}")
endif()

####################################################
# Build Options
####################################################
# TODO: - FIPS
#       - Distro
#       - Linux Kernel Module
#       - Single precision math
#       - Enable all
#       - Enable all crypto

# For reproducible build, gate out from the build anything that might
# introduce semantically frivolous jitter, maximizing chance of
# identical object files.
add_option("WOLFSSL_REPRODUCIBLE_BUILD"
    "Enable maximally reproducible build (default: disabled)"
    "no" "yes;no")

if(WOLFSSL_REPRODUCIBLE_BUILD)
    list(APPEND WOLFSSL_DEFINITIONS "-DHAVE_REPRODUCIBLE_BUILD")
    set(CMAKE_C_ARCHIVE_CREATE "<CMAKE_AR> Dqc <TARGET> <LINK_FLAGS> <OBJECTS>")
    set(CMAKE_C_ARCHIVE_APPEND "<CMAKE_AR> Dq  <TARGET> <LINK_FLAGS> <OBJECTS>")
    set(CMAKE_C_ARCHIVE_FINISH "<CMAKE_RANLIB> -D <TARGET>")
endif()

# Support for forcing 32-bit mode
# TODO: detect platform from other options
add_option("WOLFSSL_32BIT"
    "Enables 32-bit support (default: disabled)"
    "no" "yes;no")

# 16-bit compiler support
add_option("WOLFSSL_16BIT"
    "Enables 16-bit support (default: disabled)"
    "no" "yes;no")
if(WOLFSSL_16BIT)
    list(APPEND WOLFSSL_DEFINITIONS "-DWC_16BIT_CPU")
endif()

# Support for disabling all ASM
add_option("WOLFSSL_ASM"
    "Enables option for assembly (default: enabled)"
    "yes" "yes;no")

if(NOT WOLFSSL_ASM)
    list(APPEND WOLFSSL_DEFINITIONS
        "-DTFM_NO_ASM"
        "-DWOLFSSL_NO_ASM")
endif()

# Enable Debugging
add_option("WOLFSSL_DEBUG"
    "Enables option for debug (default: disabled)"
    "no" "yes;no")

if(WOLFSSL_DEBUG)
    set(CMAKE_C_FLAGS "-g ${CMAKE_C_FLAGS}")
    list(APPEND WOLFSSL_DEFINITIONS
        "-DDEBUG_WOLFSSL"
        "-DDEBUG")
endif()


# Single threaded
add_option("WOLFSSL_SINGLE_THREADED"
    "Enable wolfSSL single threaded (default: disabled)"
    "no" "yes;no")

# TODO: Logic here isn't complete, yet (see AX_PTHREAD)
if(NOT WOLFSSL_SINGLE_THREADED)
    if(CMAKE_USE_PTHREADS_INIT)
        list(APPEND WOLFSSL_LINK_LIBS Threads::Threads)
        set(HAVE_PTHREAD 1)
        list(APPEND WOLFSSL_DEFINITIONS
            "-DHAVE_PTHREAD"
            "-D_POSIX_THREADS")
    endif()
endif()


# DTLS
add_option("WOLFSSL_DTLS"
    "Enables wolfSSL DTLS (default: disabled)"
    "no" "yes;no")

if(WOLFSSL_DTLS)
    list(APPEND WOLFSSL_DEFINITIONS
        "-DWOLFSSL_DTLS")
endif()


# TLS v1.3
add_option("WOLFSSL_TLS13"
    "Enable wolfSSL TLS v1.3 (default: enabled)"
    "yes" "yes;no")

if("${FIPS_VERSION}" STREQUAL "v1")
    override_cache(WOLFSSL_TLS13 "no")
endif()

# Post-handshake authentication
add_option("WOLFSSL_POSTAUTH"
    "Enable wolfSSL Post-handshake Authentication (default: disabled)"
    "no" "yes;no")

if(WOLFSSL_POSTAUTH)
    if(NOT WOLFSSL_TLS13)
        message(WARNING "TLS 1.3 is disabled - disabling Post-handshake Authentication")
        override_cache(WOLFSSL_POSTAUTH "no")
    else()
        list(APPEND WOLFSSL_DEFINITIONS
            "-DWOLFSSL_POST_HANDSHAKE_AUTH")
    endif()
endif()

# Hello Retry Request Cookie
add_option("WOLFSSL_HRR_COOKIE"
    "Enable the server to send Cookie Extension in HRR with state (default: disabled)"
    "undefined" "yes;no;undefined")

if("${WOLFSSL_HRR_COOKIE}" STREQUAL "yes")
    if(NOT WOLFSSL_TLS13)
        message(WARNING "TLS 1.3 is disabled - disabling HRR Cookie")
        override_cache(WOLFSSL_HRR_COOKIE "no")
    else()
        list(APPEND WOLFSSL_DEFINITIONS
            "-DWOLFSSL_SEND_HRR_COOKIE")
    endif()
endif()

# DTLS v1.3
add_option("WOLFSSL_DTLS13"
    "Enable wolfSSL DTLS v1.3 (default: disabled)"
    "no" "yes;no")

if(WOLFSSL_DTLS13)
    if (NOT WOLFSSL_DTLS)
      message(FATAL_ERROR "DTLS13 requires DTLS")
    endif()
    if (NOT WOLFSSL_TLS13)
      message(FATAL_ERROR "DTLS13 requires TLS13")
    endif()
    list(APPEND WOLFSSL_DEFINITIONS "-DWOLFSSL_DTLS13")
    list(APPEND WOLFSSL_DEFINITIONS "-DWOLFSSL_W64_WRAPPER")
    if ("${WOLFSSL_HRR_COOKIE}" STREQUAL "undefined")
      message(WARNING "DTLS1.3 is enabled - enabling HRR Cookie")
      override_cache(WOLFSSL_HRR_COOKIE "yes")
      list(APPEND WOLFSSL_DEFINITIONS "-DWOLFSSL_SEND_HRR_COOKIE")
    endif()
    if (WOLFSSL_AES)
      list(APPEND WOLFSSL_DEFINITIONS "-DWOLFSSL_AES_DIRECT")
    endif()
endif()

# DTLS ConnectionID support
add_option("WOLFSSL_DTLS_CID"
    "Enables wolfSSL DTLS CID (default: disabled)"
    "no" "yes;no")

if(WOLFSSL_DTLS_CID)
    if(NOT WOLFSSL_DTLS13)
        message(FATAL_ERROR "CID are supported only for DTLSv1.3")
    endif()
    list(APPEND WOLFSSL_DEFINITIONS "-DWOLFSSL_DTLS_CID")
endif()

# RNG
add_option("WOLFSSL_RNG"
    "Enable compiling and using RNG (default: enabled)"
    "yes" "yes;no")

if(NOT WOLFSSL_RNG)
    list(APPEND WOLFSSL_DEFINITIONS "-DWC_NO_RNG")
endif()

# TODO: - DTLS-SCTP
#       - DTLS multicast
#       - OpenSSH
#       - OpenVPN
#       - Nginx
#       - HAProxy
#       - wpa_supplicant
#       - Fortress
#       - libwebsockets
#       - IP alternative name
#       - Qt
#       - SSL bump
#       - sniffer
#       - Signal
#       - OpenSSL coexist
#       - OpenSSL compatibility all
#       - OpenSSL compatibility extra
#       - Max strength

# Harden, enable Timing Resistance and Blinding by default
add_option("WOLFSSL_HARDEN"
    "Enable Hardened build, Enables Timing Resistance and Blinding (default: enabled)"
    "yes" "yes;no")

if(WOLFSSL_HARDEN)
    list(APPEND WOLFSSL_DEFINITIONS "-DTFM_TIMING_RESISTANT" "-DECC_TIMING_RESISTANT")

    if(WOLFSSL_RNG)
        list(APPEND WOLFSSL_DEFINITIONS "-DWC_RSA_BLINDING")
    endif()
else()
    list(APPEND WOLFSSL_DEFINITIONS "-DWC_NO_HARDEN")
endif()

add_option(WOLFSSL_OPENSSLEXTRA
    "Enable extra OpenSSL API, size+ (default: disabled)"
    "no" "yes;no")

add_option(WOLFSSL_OPENSSLALL
    "Enable all OpenSSL API, size++ (default: disabled)"
    "no" "yes;no")

add_option(WOLFSSL_ASIO
    "Enable asio support (default: disabled)"
    "no" "yes;no")

if (WOLFSSL_ASIO)
    list(APPEND WOLFSSL_DEFINITIONS
    "-DWOLFSSL_ASIO" "-DASIO_USE_WOLFSSL"
    "-DBOOST_ASIO_USE_WOLFSSL" "-DHAVE_EX_DATA"
    "-DSSL_TXT_TLSV1_2" "-DOPENSSL_NO_SSL2" "-DOPENSSL_NO_SSL3"
    "-DHAVE_OCSP" "-DWOLFSSL_KEY_GEN")
    override_cache(WOLFSSL_OPENSSLALL "yes")
    override_cache(WOLFSSL_OPENSSLEXTRA "yes")
endif()

if (WOLFSSL_OPENSSLEXTRA AND NOT WOLFSSL_OPENSSLCOEXIST)
    list(APPEND WOLFSSL_DEFINITIONS
      "-DOPENSSL_EXTRA"
      "-DWOLFSSL_ALWAYS_VERIFY_CB"
      "-DWOLFSSL_VERIFY_CB_ALL_CERTS"
      "-DWOLFSSL_EXTRA_ALERTS"
      "-DHAVE_EXT_CACHE"
      "-DWOLFSSL_FORCE_CACHE_ON_TICKET")
endif()

if (WOLFSSL_OPENSSLALL)
    list(APPEND WOLFSSL_DEFINITIONS
      "-DOPENSSL_ALL" "-DWOLFSSL_EITHER_SIDE" "-DWC_RSA_NO_PADDING"
      "-DWC_RSA_PSS" "-DWOLFSSL_PSS_LONG_SALT" "-DWOLFSSL_TICKET_HAVE_ID"
      "-DWOLFSSL_ERROR_CODE_OPENSSL" "-DWOLFSSL_CERT_NAME_ALL")
endif()


# TODO: - IPv6 test apps

set(WOLFSSL_SLOW_MATH "yes")

# liboqs
add_option(WOLFSSL_OQS
    "Enable integration with the OQS (Open Quantum Safe) liboqs library (default: disabled)"
    "no" "yes;no")

if (WOLFSSL_OQS)
    find_package(OQS)

    if (OQS_FOUND)
        list(APPEND WOLFSSL_LINK_LIBS ${OQS_LIBRARY})
        list(APPEND WOLFSSL_INCLUDE_DIRS ${OQS_INCLUDE_DIR})
        set(HAVE_LIBOQS 1)
        list(APPEND WOLFSSL_DEFINITIONS
          "-DHAVE_TLS_EXTENSIONS"
          "-DHAVE_LIBOQS")
    endif()
endif()

# TODO: - Lean PSK
#       - Lean TLS
#       - Low resource
#       - Titan cache
#       - Huge cache
#       - Big cache
#       - Small cache
#       - Persistent session cache
#       - Persistent cert cache
#       - Write duplicate
#       - Atomic user record layer
#       - Public key callbacks
#       - Microchip/Atmel CryptoAuthLib

# AES-CBC
add_option("WOLFSSL_AESCBC"
    "Enable wolfSSL AES-CBC support (default: enabled)"
    "yes" "yes;no")

if(NOT WOLFSSL_AESCBC)
    list(APPEND WOLFSSL_DEFINITIONS "-DNO_AES_CBC")
endif()

# AES-GCM
add_option("WOLFSSL_AESGCM"
    "Enable wolfSSL AES-GCM support (default: enabled)"
    "yes" "yes;no;table;small;word32;4bit")

# leanpsk and leantls don't need gcm
if(WOLFSSL_LEAN_PSK OR (WOLFSSL_LEAN_TLS AND NOT WOLFSSL_TLS13))
    override_cache(WOLFSSL_AESGCM "no")
endif()

if(WOLFSSL_AESGCM AND NOT WORDS_BIGENDIAN)
    override_cache(WOLFSSL_AESGCM "4bit")
endif()

if(WOLFSSL_AESGCM)
    if("${WOLFSSL_AESGCM}" STREQUAL "word32")
        list(APPEND WOLFSSL_DEFINITIONS "-DGCM_WORD32")
        override_cache(WOLFSSL_AESGCM "yes")
    endif()

    if(("${WOLFSSL_AESGCM}" STREQUAL "small") OR WOLFSSL_LOW_RESOURCE)
        list(APPEND WOLFSSL_DEFINITIONS "-DGCM_SMALL")
        override_cache(WOLFSSL_AESGCM "yes")
    endif()

    if("${WOLFSSL_AESGCM}" STREQUAL "table")
        list(APPEND WOLFSSL_DEFINITIONS "-DGCM_TABLE")
        override_cache(WOLFSSL_AESGCM "yes")
    endif()

    if("${WOLFSSL_AESGCM}" STREQUAL "4bit")
        list(APPEND WOLFSSL_DEFINITIONS "-DGCM_TABLE_4BIT")
        override_cache(WOLFSSL_AESGCM "yes")
    endif()

    list(APPEND WOLFSSL_DEFINITIONS "-DHAVE_AESGCM")
endif()

# AES-SIV
add_option("WOLFSSL_AESSIV"
    "Enable wolfSSL AES-SIV support (default: disabled)"
    "no" "yes;no")

if(WOLFSSL_AESSIV)
    list(APPEND WOLFSSL_DEFINITIONS "-DWOLFSSL_AES_SIV")
endif()

# AES-CTR
add_option("WOLFSSL_AESCTR"
    "Enable wolfSSL AES-CTR support (default: disabled)"
    "no" "yes;no")

if(WOLFSSL_OPENVPN OR
   WOLFSSL_LIBSSH2 OR
   WOLFSSL_AESSIV)
    override_cache(WOLFSSL_AESCTR "yes")
endif()

if(WOLFSSL_AESCTR AND NOT WOLFSSL_FORTRESS)
    list(APPEND WOLFSSL_DEFINITIONS
        "-DWOLFSSL_AES_COUNTER"
        "-DWOLFSSL_AES_DIRECT")
endif()

# AES-CCM
add_option("WOLFSSL_AESCCM"
    "Enable wolfSSL AES-CCM support (default: disabled)"
    "no" "yes;no")

# AES-OFB
add_option("WOLFSSL_AESOFB"
    "Enable wolfSSL AES-OFB support (default: disabled)"
    "no" "yes;no")

# TODO: - AES-GCM stream
#       - AES-ARM
#       - Xilinx hardened crypto
#       - Intel AES-NI
#       - Intel ASM
#       - Intel RDRAND
#       - Linux af_alg
#       - Linux dev crpyto calls
#       - Camellia
#       - MD2
#       - NULL cipher
#       - RIPEMD
#       - BLAKE2

add_option("WOLFSSL_AESCFB"
    "Enable wolfSSL AES-CFB support (default: disabled)"
    "no" "yes;no")

# Align data
add_option("WOLFSSL_ALIGN_DATA"
    "Align data for ciphers (default: enabled)"
    "yes" "yes;no")
if(WOLFSSL_ALIGN_DATA)
    list(APPEND WOLFSSL_DEFINITIONS "-DWOLFSSL_USE_ALIGN")
endif()

# SHA224
set(SHA224_DEFAULT "no")
if(("${CMAKE_SYSTEM_PROCESSOR}" MATCHES "x86_64|AMD64|arm64") OR
    ("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "aarch64"))
    if(NOT WOLFSSL_AFALG AND NOT WOLFSSL_DEVCRYPTO AND
       (NOT WOLFSSL_FIPS OR ("${FIPS_VERSION}" STREQUAL "v2")))
        set(SHA224_DEFAULT "yes")
    endif()
endif()

add_option("WOLFSSL_SHA224"
    "Enable wolfSSL SHA-224 support (default: enabled on x86_64/aarch64)"
    ${SHA224_DEFAULT} "yes;no")

# SHA3
set(SHA3_DEFAULT "no")
if(("${CMAKE_SYSTEM_PROCESSOR}" MATCHES "x86_64|AMD64|arm64") OR
    ("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "aarch64"))
    if(NOT WOLFSSL_FIPS OR ("${FIPS_VERSION}" STREQUAL "v2"))
        set(SHA3_DEFAULT "yes")
    endif()
endif()

add_option("WOLFSSL_SHA3"
    "Enable wolfSSL SHA-3 support (default: enabled on x86_64/aarch64)"
     ${SHA3_DEFAULT} "yes;no;small")

# SHAKE256
add_option("WOLFSSL_SHAKE256"
    "Enable wolfSSL SHAKE256 support (default: enabled on x86_64/aarch64)"
    "no" "yes;no;small")

# SHAKE128
add_option("WOLFSSL_SHAKE128"
    "Enable wolfSSL SHAKE128 support (default: enabled on x86_64/aarch64)"
    "no" "yes;no;small")

# SHA512
add_option("WOLFSSL_SHA512"
    "Enable wolfSSL SHA-512 support (default: enabled)"
    "yes" "yes;no")

# options that don't require sha512
if(WOLFSSL_LEAN_PSK OR
   WOLFSSL_LEAN_TLS OR
   WOLFSSL_32BIT OR
   WOLFSSL_16BIT)
    override_cache(WOLFSSL_SHA512 "no")
endif()

# options that require sha512
if(WOLFSSL_OPENSSH OR
   WOLFSSL_WPAS OR
   WOLFSSL_FORTRESS)
    override_cache(WOLFSSL_SHA512 "yes")
endif()

if(WOLFSSL_SHA512)
    list(APPEND WOLFSSL_DEFINITIONS "-DWOLFSSL_SHA512")
endif()

# SHA384
add_option("WOLFSSL_SHA384"
    "Enable wolfSSL SHA-384 support (default: enabled)"
    "yes" "yes;no")

# options that don't require sha384
if(WOLFSSL_LEAN_PSK OR
   WOLFSSL_LEAN_TLS OR
   WOLFSSL_32BIT OR
   WOLFSSL_16BIT)
    override_cache(WOLFSSL_SHA384 "no")
endif()

# options that require sha384
if(WOLFSSL_OPENSSH OR
   WOLFSSL_WPAS OR
   WOLFSSL_FORTRESS)
    override_cache(WOLFSSL_SHA384 "yes")
endif()

if(WOLFSSL_SHA384)
    list(APPEND WOLFSSL_DEFINITIONS "-DWOLFSSL_SHA384")
endif()

# TODO: - Session certs
#       - SEP

add_option("WOLFSSL_KEYGEN"
    "Enable key generation (default: disabled)])"
    "no" "yes;no")

add_option("WOLFSSL_CERTGEN"
    "Enable cert generation (default: disabled)"
    "no" "yes;no")

add_option("WOLFSSL_CERTREQ"
    "Enable cert request generation (default: disabled)"
    "no" "yes;no")

add_option("WOLFSSL_CERTEXT"
    "Enable cert request extensions (default: disabled)"
    "no" "yes;no")

add_option("WOLFSSL_CERTGENCACHE"
    "Enable decoded cert caching (default: disabled)"
    "no" "yes;no")

# HKDF
add_option("WOLFSSL_HKDF"
    "Enable HKDF (HMAC-KDF) support (default: disabled)"
    "no" "yes;no")

if(WOLFSSL_TLS13)
    override_cache(WOLFSSL_HKDF "yes")
endif()

if(WOLFSSL_HKDF)
    list(APPEND WOLFSSL_DEFINITIONS "-DHAVE_HKDF")
endif()

# DSA
add_option("WOLFSSL_DSA"
    "Enable DSA (default: disabled)"
    "no" "yes;no")

if(NOT WOLFSSL_DSA AND NOT WOLFSSL_OPENSSH)
    list(APPEND WOLFSSL_DEFINITIONS "-DNO_DSA")
endif()

# ECC Shamir
add_option("WOLFSSL_ECCSHAMIR"
    "Enable ECC Shamir (default: enabled)"
    "yes" "yes;no")

# ECC
add_option("WOLFSSL_ECC"
    "Enable ECC (default: enabled)"
    "yes" "yes;no;nonblock")

# lean psk doesn't need ecc
if(WOLFSSL_LEAN_PSK)
    override_cache(WOLFSSL_ECC "no")
endif()

if(WOLFSSL_OPENSSH OR
   WOLFSSL_NGINX OR
   WOLFSSL_SIGNAL)
    override_cache(WOLFSSL_ECC "yes")
endif()

if(WOLFSSL_ECC)
    list(APPEND WOLFSSL_DEFINITIONS "-DHAVE_ECC" "-DTFM_ECC256")

    if(WOLFSSL_ECCSHAMIR AND NOT WOLFSSL_LOW_RESOURCE)
        list(APPEND WOLFSSL_DEFINITIONS "-DECC_SHAMIR")
    endif()

    if("${WOLFSSL_ECC}" STREQUAL "nonblock")
        list(APPEND WOLFSSL_DEFINITIONS "-DWC_ECC_NONBLOCK")
    endif()
endif()

# TODO: - ECC custom curves
#       - Compressed key
#       - FP ECC, fixed point cache ECC
#       - ECC encrypt
#       - PSK
#       - Single PSK identity

# CURVE25519
set(WOLFSSL_CURVE25519_SMALL "no")
add_option("WOLFSSL_CURVE25519"
    "Enable Curve25519 (default: disabled)"
    "no" "yes;no;small;no128bit")

if(WOLFSSL_OPENSSH)
    override_cache(WOLFSSL_CURVE25519 "yes")
endif()

if(WOLFSSL_CURVE25519)
    if("${WOLFSSL_CURVE25519}" STREQUAL "small" OR WOLFSSL_LOW_RESOURCE)
        list(APPEND WOLFSSL_DEFINITIONS "-DCURVE25519_SMALL")
        set(WOLFSSL_CURVE25519_SMALL "yes")
    endif()

    if("${WOLFSSL_CURVE25519}" STREQUAL "no128bit" OR WOLFSSL_32BIT)
        list(APPEND WOLFSSL_DEFINITIONS "-DNO_CURVED25519_128BIT")
    endif()

    list(APPEND WOLFSSL_DEFINITIONS "-DHAVE_CURVE25519")
    set(WOLFSSL_FEMATH "yes")
endif()

# ED25519
set(WOLFSSL_ED25519_SMALL "no")
add_option("WOLFSSL_ED25519"
    "Enable ED25519 (default: disabled)"
    "no" "yes;no")

if(WOLFSSL_OPENSSH)
    override_cache(WOLFSSL_ED25519 "yes")
endif()

if(WOLFSSL_ED25519 AND NOT WOLFSSL_32BIT)
    if("${WOLFSSL_ED25519}" STREQUAL "small" OR WOLFSSL_LOW_RESOURCE)
        list(APPEND WOLFSSL_DEFINITIONS "-DED25519_SMALL")
        set(WOLFSSL_ED25519_SMALL "yes")
        set(WOLFSSL_CURVE25519_SMALL "yes")
    endif()

    if(NOT WOLFSSL_SHA512)
        message(FATAL_ERROR "cannot enable ed25519 without enabling sha512.")
    endif()

    set(WOLFSSL_FEMATH "yes")
    set(WOLFSSL_GEMATH "yes")
    list(APPEND WOLFSSL_DEFINITIONS "-DHAVE_ED25519")
endif()

# CURVE448
set(WOLFSSL_CURVE448_SMALL "no")
add_option("WOLFSSL_CURVE448"
    "Enable Curve448 (default: disabled)"
    "no" "yes;no;small")

if(WOLFSSL_CURVE448)
    if("${WOLFSSL_CURVE448}" STREQUAL "small" OR WOLFSSL_LOW_RESOURCE)
        list(APPEND WOLFSSL_DEFINITIONS "-DCURVE448_SMALL")
        set(WOLFSSL_CURVE448_SMALL "yes")
    endif()

    if("${WOLFSSL_CURVE448}" STREQUAL "no128bit" OR WOLFSSL_32BIT)
        list(APPEND WOLFSSL_DEFINITIONS "-DNO_CURVED448_128BIT")
    endif()

    list(APPEND WOLFSSL_DEFINITIONS "-DHAVE_CURVE448")
    set(WOLFSSL_FE448 "yes")
endif()

# ED448
set(WOLFSSL_ED448_SMALL "no")
add_option("WOLFSSL_ED448"
    "Enable ED448 (default: disabled)"
    "no" "yes;no;small")

if(WOLFSSL_ED448 AND NOT WOLFSSL_32BIT)
    if("${WOLFSSL_ED448}" STREQUAL "small" OR WOLFSSL_LOW_RESOURCE)
        list(APPEND WOLFSSL_DEFINITIONS "-DED448_SMALL")
        set(WOLFSSL_ED448_SMALL "yes")
        set(WOLFSSL_CURVE448_SMALL "yes")
    endif()

    if(NOT WOLFSSL_SHA512)
        message(FATAL_ERROR "cannot enable ed448 without enabling sha512.")
    endif()

    set(WOLFSSL_FE448 "yes")
    set(WOLFSSL_GE448 "yes")
    list(APPEND WOLFSSL_DEFINITIONS "-DHAVE_ED448")

    # EdDSA448 requires SHAKE256 which requires SHA-3
    override_cache(WOLFSSL_SHAKE256 "yes")
endif()

# Error strings
add_option("WOLFSSL_ERROR_STRINGS"
    "Enable error strings table (default: enabled)"
    "yes" "yes;no")

if(NOT WOLFSSL_ERROR_STRINGS)
    list(APPEND WOLFSSL_DEFINITIONS "-DNO_ERROR_STRINGS")
else()
    # turn off error strings if leanpsk or leantls on
    if(WOLFSSL_LEAN_PSK OR WOLFSSL_LEAN_TLS)
        list(APPEND WOLFSSL_DEFINITIONS "-DNO_ERROR_STRINGS")
        override_cache(WOLFSSL_ERROR_STRINGS "no")
    endif()
endif()

# Error queue
add_option("WOLFSSL_ERROR_QUEUE"
    "Enables adding nodes to error queue when compiled with OPENSSL_EXTRA (default: enabled)"
    "yes" "yes;no")

if(NOT WOLFSSL_ERROR_QUEUE)
    list(APPEND WOLFSSL_DEFINITIONS "-DNO_ERROR_QUEUE")
endif()

# Old TLS
add_option("WOLFSSL_OLD_TLS"
    "Enable old TLS versions < 1.2 (default: enabled)"
    "yes" "yes;no")

if(NOT WOLFSSL_OLD_TLS)
    list(APPEND WOLFSSL_DEFINITIONS "-DNO_OLD_TLS")
else()
    # turn off old if leanpsk or leantls on
    if(WOLFSSL_LEAN_PSK OR WOLFSSL_LEAN_TLS)
        list(APPEND WOLFSSL_DEFINITIONS "-DNO_OLD_TLS")
        override_cache(WOLFSSL_OLD_TLS "no")
    endif()
endif()

# TLSv1.2
add_option("WOLFSSL_TLSV12"
    "Enable TLS versions 1.2 (default: enabled)"
    "yes" "yes;no")

if(NOT WOLFSSL_TLSV12)
    list(APPEND WOLFSSL_DEFINITIONS
        "-DWOLFSSL_NO_TLS12"
        "-DNO_OLD_TLS")
endif()

# TODO: - TLSv1.0
#       - SSLv3
#       - Stack size
#       - Stack size verbose

# Memory
add_option("WOLFSSL_MEMORY"
    "Enable memory callbacks (default: enabled)"
    "yes" "yes;no")

if(NOT WOLFSSL_MEMORY)
    list(APPEND WOLFSSL_DEFINITIONS "-DNO_WOLFSSL_MEMORY")
else()
    # turn off memory cb if leanpsk or leantls on
    if(WOLFSSL_LEAN_PSK OR WOLFSSL_LEAN_TLS)
        # but don't turn on NO_WOLFSSL_MEMORY because using own
        override_cache(WOLFSSL_MEMORY "no")
    endif()
endif()

# TODO: - Track memory
#       - Memory log
#       - Stack log

# RSA
add_option("WOLFSSL_RSA"
    "Enable RSA (default: enabled)"
    "yes" "yes;no")

if(NOT WOLFSSL_RSA)
    list(APPEND WOLFSSL_DEFINITIONS "-DNO_RSA")
else()
    if(WOLFSSL_LEAN_PSK OR WOLFSSL_LEAN_TLS)
        list(APPEND WOLFSSL_DEFINITIONS "-DNO_RSA")
        override_cache(WOLFSSL_RSA "no")
    endif()
endif()

# OAEP
add_option("WOLFSSL_OAEP"
    "Enable RSA OAEP (default: enabled)"
    "yes" "yes;no")

if(NOT WOLFSSL_OAEP)
    list(APPEND WOLFSSL_DEFINITIONS "-DWC_NO_RSA_OAEP")
endif()

# TODO: - RSA public only
#       - RSA verify inline only

# RSA-PSS
add_option("WOLFSSL_RSA_PSS"
    "Enable RSA-PSS (default: disabled)"
    "no" "yes;no")

if(NOT WOLFSSL_RSA)
    override_cache(WOLFSSL_RSA_PSS "no")
else()
    if(WOLFSSL_TLS13)
        override_cache(WOLFSSL_RSA_PSS "yes")
    endif()
endif()
if(WOLFSSL_RSA_PSS)
    list(APPEND WOLFSSL_DEFINITIONS "-DWC_RSA_PSS")
    list(APPEND WOLFSSL_DEFINITIONS "-DWOLFSSL_PSS_LONG_SALT")
endif()

# DH
add_option("WOLFSSL_DH"
    "Enable DH (default: enabled)"
    "yes" "yes;no;const")

if(WOLFSSL_OPENSSH)
    override_cache(WOLFSSL_DH "yes")
endif()

if(NOT WOLFSSL_DH)
    list(APPEND WOLFSSL_DEFINITIONS "-DNO_DH")
else()
    if(WOLFSSL_LEAN_PSK OR WOLFSSL_LEAN_TLS)
      list(APPEND WOLFSSL_DEFINITIONS "-DNO_DH")
      override_cache(WOLFSSL_DH "no")
    endif()
endif()

if("${WOLFSSL_DH}" STREQUAL "const")
    list(APPEND WOLFSSL_DEFINITIONS "-DWOLFSSL_DH_CONST")
    set(WOLFSSL_DH_CONST "yes")
endif()

# TODO: - Anonymous

# ASN
# turn off asn, which means no certs, no rsa, no dsa, no ecc,
# and no big int (unless dh is on)
add_option("WOLFSSL_ASN"
    "Enable ASN (default: enabled)"
    "yes" "yes;no")

if(NOT WOLFSSL_ASN)
    list(APPEND WOLFSSL_DEFINITIONS "-DNO_ASN" "-DNO_CERTS")

    if(NOT WOLFSSL_DH AND NOT WOLFSSL_ECC)
        # DH and ECC need bigint
        list(APPEND WOLFSSL_DEFINITIONS "-DNO_BIG_INT")
    endif()
else()
    # turn off ASN if leanpsk on
    if(WOLFSSL_LEAN_PSK)
        list(APPEND WOLFSSL_DEFINITIONS
            "-DNO_ASN"
            "-DNO_CERTS"
            "-DNO_BIG_INT")
        override_cache(WOLFSSL_ASN "no")
    else()
        if("${WOLFSSL_ASN}" STREQUAL "nocrypt")
            list(APPEND WOLFSSL_DEFINITIONS "-DNO_ASN_CRYPT")
            # TODO: verify that this is correct
            override_cache(WOLFSSL_PWDBASED "no")
        endif()
    endif()
endif()

if(WOLFSSL_RSA AND NOT WOLFSSL_RSA_VFY AND NOT WOLFSSL_ASN)
    message(FATAL_ERROR "please disable rsa if disabling asn.")
endif()

if(WOLFSSL_DSA AND NOT WOLFSSL_ASN)
    message(FATAL_ERROR "please disable dsa if disabling asn.")
endif()

# DH and ECC need bigint
if(NOT WOLFSSL_ASN AND
   NOT WOLFSSL_DH AND
   NOT WOLFSSL_ECC AND
   NOT WOLFSSL_RSA)
    override_cache(WOLFSSL_FAST_MATH "no")
    set(WOLFSSL_SLOWMATH "no")
endif()

# AES
add_option("WOLFSSL_AES"
    "Enable AES (default: enabled)"
    "yes" "yes;no")

if(NOT WOLFSSL_AES)
    list(APPEND WOLFSSL_DEFINITIONS "-DNO_AES")

    if(WOLFSSL_FORTRESS)
        message(FATAL_ERROR "fortress requires aes")
    endif()
    if(WOLFSSL_ECC_ENCRYPT)
        message(FATAL_ERROR "cannot enable eccencrypt and hkdf without aes.")
    endif()
    if(WOLFSSL_AESGCM)
        message(FATAL_ERROR "AESGCM requires AES.")
    endif()
    if(WOLFSSL_AESCCM)
        message(FATAL_ERROR "AESCCM requires AES.")
    endif()
    if(WOLFSSL_AESCTR)
        message(FATAL_ERROR "AESCTR requires AES.")
    endif()
else()
    if(WOLFSSL_LEAN_PSK)
        list(APPEND WOLFSSL_DEFINITIONS "-DNO_AES")
        override_cache(WOLFSSL_AES "no")
    endif()
endif()

# Coding
add_option("WOLFSSL_CODING"
    "Enable coding base 16/64 (default: enabled)"
    "yes" "yes;no")

if(NOT WOLFSSL_CODING)
    list(APPEND WOLFSSL_DEFINITIONS "-DNO_CODING")
else()
    # turn off CODING if leanpsk on
    if(WOLFSSL_LEAN_PSK)
        list(APPEND WOLFSSL_DEFINITIONS "-DNO_CODING")
        override_cache(WOLFSSL_CODING "no")
    endif()
endif()

# Base64
set(BASE64_ENCODE_DEFAULT "no")
if("${CMAKE_SYSTEM_PROCESSOR}" MATCHES "x86_64|AMD64|arm64")
    set(BASE64_ENCODE_DEFAULT "yes")
endif()

set(WOLFSSL_BASE64_ENCODE_HELP_STRING "Enable Base64 encoding (default: enabled on x86_64)")
add_option("WOLFSSL_BASE64_ENCODE" ${WOLFSSL_BASE64_ENCODE_HELP_STRING} ${BASE64_ENCODE_DEFAULT} "yes;no")

if(WOLFSSL_BASE64_ENCODE)
    list(APPEND WOLFSSL_DEFINITIONS "-DWOLFSSL_BASE64_ENCODE")
endif()

# TODO: - Base16

# DES3
set(WOLFSSL_DES3_HELP_STRING "Enable DES3 (default: disabled)")
add_option("WOLFSSL_DES3" ${WOLFSSL_DES3_HELP_STRING} "no" "yes;no")

if(WOLFSSL_OPENSSH OR
   WOLFSSL_QT OR
   WOLFSSL_OPENVPN OR
   WOLFSSL_WPAS OR
   WOLFSSL_ASIO)
    override_cache(WOLFSSL_DES3 "yes")
endif()

# ARC4
set(WOLFSSL_ARC4_HELP_STRING "Enable ARC4 (default: disabled)")
add_option("WOLFSSL_ARC4" ${WOLFSSL_ARC4_HELP_STRING} "no" "yes;no")

if(WOLFSSL_OPENSSH OR WOLFSSL_WPAS)
    override_cache(WOLFSSL_ARC4 "yes")
endif()

# MD5
set(WOLFSSL_MD5_HELP_STRING "Enable MD5 (default: enabled)")
add_option("WOLFSSL_MD5" ${WOLFSSL_MD5_HELP_STRING} "yes" "yes;no")

if(NOT WOLFSSL_MD5)
    list(APPEND WOLFSSL_DEFINITIONS "-DNO_MD5" "-DNO_OLD_TLS")
else()
    # turn off MD5 if leanpsk or leantls on
    if(WOLFSSL_LEAN_PSK OR WOLFSSL_LEAN_TLS)
        list(APPEND WOLFSSL_DEFINITIONS "-DNO_MD5" "-DNO_OLD_TLS")
        override_cache(WOLFSSL_MD5 "no")
    endif()
endif()

# SHA
add_option("WOLFSSL_SHA"
    "Enable SHA (default: enabled)"
    "yes" "yes;no")

if(NOT WOLFSSL_SHA)
    list(APPEND WOLFSSL_DEFINITIONS "-DNO_SHA" "-DNO_OLD_TLS")
else()
    # turn off SHA if leanpsk or leantls on
    if(WOLFSSL_LEAN_PSK OR WOLFSSL_LEAN_TLS)
        list(APPEND WOLFSSL_DEFINITIONS "-DNO_SHA" "-DNO_OLD_TLS")
        override_cache(WOLFSSL_SHA "no")
    endif()
endif()

# TODO: - AES-XTS
#       - Web server
#       - Web client
add_option("WOLFSSL_CMAC"
    "Enable CMAC (default: disabled)"
    "no" "yes;no")

if(WOLFSSL_WPAS OR
   WOLFSSL_NTP OR
   WOLFSSL_AESSIV)
    override_cache(WOLFSSL_CMAC "yes")
endif()

if(WOLFSSL_CMAC)
    if (NOT WOLFSSL_AES)
        message(FATAL_ERROR "Cannot use CMAC without AES.")
    else()
        list(APPEND WOLFSSL_DEFINITIONS
            "-DWOLFSSL_CMAC"
            "-DWOLFSSL_AES_DIRECT")
    endif()
endif()

# TODO: - RC2
#       - FIPS, again (there's more logic for FIPS in configure.ac)
#       - Selftest

# SHA224
if(WOLFSSL_SHA224)
    list(APPEND WOLFSSL_DEFINITIONS "-DWOLFSSL_SHA224")
endif()

# SHA3
if("${WOLFSSL_SHA3}" STREQUAL "small")
    list(APPEND WOLFSSL_DEFINITIONS "-DWOLFSSL_SHA3_SMALL")
    override_cache(WOLFSSL_SHA3 "yes")
endif()

if(WOLFSSL_SHA3 AND NOT WOLFSSL_32BIT)
    list(APPEND WOLFSSL_DEFINITIONS "-DWOLFSSL_SHA3")
endif()

# SHAKE256
if(WOLFSSL_SHAKE256)
    if(NOT WOLFSSL_32BIT)
        list(APPEND WOLFSSL_DEFINITIONS "-DWOLFSSL_SHAKE256")

        if(NOT WOLFSSL_SHA3)
            message(FATAL_ERROR "Must have SHA-3 enabled: --enable-sha3")
        endif()
    endif()
else()
    list(APPEND WOLFSSL_DEFINITIONS "-DWOLFSSL_NO_SHAKE256")
endif()

# SHAKE128
if(WOLFSSL_SHAKE128)
    if(NOT WOLFSSL_32BIT)
        list(APPEND WOLFSSL_DEFINITIONS "-DWOLFSSL_SHAKE128")

        if(NOT WOLFSSL_SHA3)
            message(FATAL_ERROR "Must have SHA-3 enabled: --enable-sha3")
        endif()
    endif()
else()
    list(APPEND WOLFSSL_DEFINITIONS "-DWOLFSSL_NO_SHAKE128")
endif()

# POLY1305
set(POLY1305_DEFAULT "yes")
if(WOLFSSL_FIPS)
    set(POLY1305_DEFAULT "no")
endif()

set(WOLFSSL_POLY1305_HELP_STRING "Enable wolfSSL POLY1305 support (default: enabled)")
add_option("WOLFSSL_POLY1305" ${WOLFSSL_POLY1305_HELP_STRING} ${POLY1305_DEFAULT} "yes;no")

# leanpsk and leantls don't need poly1305
if(WOLFSSL_LEAN_PSK OR WOLFSSL_LEAN_TLS)
    override_cache(WOLFSSL_POLY1305 "no")
endif()

if(WOLFSSL_POLY1305)
    list(APPEND WOLFSSL_DEFINITIONS
        "-DHAVE_POLY1305"
        "-DHAVE_ONE_TIME_AUTH")
endif()

# CHACHA
set(CHACHA_DEFAULT "yes")
if(WOLFSSL_FIPS)
    set(CHACHA_DEFAULT "no")
endif()

add_option("WOLFSSL_CHACHA"
    "Enable CHACHA (default: enabled). Use `=noasm` to disable ASM AVX/AVX2 speedups"
    ${CHACHA_DEFAULT} "yes;no;noasm")

# leanpsk and leantls don't need chacha
if(WOLFSSL_LEAN_PSK OR WOLFSSL_LEAN_TLS)
    override_cache(WOLFSSL_CHACHA "no")
endif()

if(("${WOLFSSL_CHACHA}" STREQUAL "noasm") OR NOT WOLFSSL_ASM)
    list(APPEND WOLFSSL_DEFINITIONS "-DNO_CHACHA_ASM")
endif()

if(NOT ("${WOLFSSL_CHACHA}" STREQUAL "noasm") AND WOLFSSL_CHACHA)
    list(APPEND WOLFSSL_DEFINITIONS "-DHAVE_CHACHA")
endif()

# TODO: - XCHACHA

# Hash DRBG
add_option("WOLFSSL_HASH_DRBG"
    "Enable Hash DRBG support (default: enabled)"
    "yes" "yes;no")

if(WOLFSSL_HASH_DRBG)
    list(APPEND WOLFSSL_DEFINITIONS "-DHAVE_HASHDRBG")
else()
    # turn on Hash DRBG if FIPS is on
    if(WOLFSSL_FIPS)
        list(APPEND WOLFSSL_DEFINITIONS "-DHAVE_HASHDRBG")
        override_cache(WOLFSSL_HASH_DRBG "yes")
    else()
        list(APPEND WOLFSSL_DEFINITIONS "-DWC_NO_HASHDRBG")
    endif()
endif()

# Filesystem
if(WOLFSSL_LINUX_KM)
    set(FILESYSTEM_DEFAULT "no")
else()
    set(FILESYSTEM_DEFAULT "yes")
endif()

add_option("WOLFSSL_FILESYSTEM"
    "Enable Filesystem support (default: enabled)"
    ${FILESYSTEM_DEFAULT} "yes;no")

if(NOT WOLFSSL_FILESYSTEM)
    list(APPEND WOLFSSL_DEFINITIONS "-DNO_FILESYSTEM")
else()
    if(WOLFSSL_LEAN_PSK OR WOLFSSL_LEAN_TLS)
        list(APPEND WOLFSSL_DEFINITIONS "-DNO_FILESYSTEM")
        override_cache(WOLFSSL_FILESYSTEM "no")
    endif()
endif()

# Inline function support
add_option("WOLFSSL_INLINE"
    "Enable inline functions (default: enabled)"
    "yes" "yes;no")

if(NOT WOLFSSL_INLINE)
    list(APPEND WOLFSSL_DEFINITIONS "-DNO_INLINE")
endif()

# TODO:
#       - CRL monitor
#       - User crypto
#       - Whitewood netRandom client library
#       - Max fragment length
#       - ALPN
#       - Trusted CA indication
#       - Truncated HMAC
#       - Renegotiation indication
#       - Secure renegotiation
#       - Fallback SCSV

add_option(WOLFSSL_OCSP "Enable OCSP (default: disabled)" "no" "yes;no")
add_option(WOLFSSL_OCSPSTAPLING "Enable OCSP Stapling (default: disabled)" "no" "yes;no")
add_option(WOLFSSL_OCSPSTAPLING_V2 "Enable OCSP Stapling v2 (default: disabled)" "no" "yes;no")
add_option(WOLFSSL_CRL
    "Enable CRL (Use =io for inline CRL HTTP GET) (default: disabled)"
    "no" "yes;no;io")


set(SNI_DEFAULT "no")
if(("${CMAKE_SYSTEM_PROCESSOR}" MATCHES "x86_64|x86|AMD64|arm64") OR
    ("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "aarch64"))
    set(SNI_DEFAULT "yes")
endif()
set(WOLFSSL_SNI_HELP_STRING "Enable SNI (default: disabled)")
add_option(WOLFSSL_SNI ${WOLFSSL_SNI_HELP_STRING} ${SNI_DEFAULT} "yes;no")

set(WOLFSSL_TLSX_HELP_STRING "Enable all TLS Extensions (default: disabled)")
add_option(WOLFSSL_TLSX ${WOLFSSL_TLSX_HELP_STRING} "no" "yes;no")

# Supported elliptic curves extensions
add_option("WOLFSSL_SUPPORTED_CURVES"
    "Enable Supported Elliptic Curves (default: enabled)"
    "yes" "yes;no")

if(WOLFSSL_SUPPORTED_CURVES)
    if(NOT WOLFSSL_ECC AND NOT WOLFSSL_CURVE25519 AND NOT WOLFSSL_CURVE448)
        override_cache(WOLFSSL_SUPPORTED_CURVES "no")
    else()
        list(APPEND WOLFSSL_DEFINITIONS
            "-DHAVE_TLS_EXTENSIONS"
            "-DHAVE_SUPPORTED_CURVES")
    endif()
endif()

# Diffie-Hellman
if(WOLFSSL_DH)
    if(WOLFSSL_TLS13 OR WOLFSSL_SUPPORTED_CURVES)
        list(APPEND WOLFSSL_DEFINITIONS "-DHAVE_FFDHE_2048")
    endif()
endif()

# TODO: - FFDHE params only

# TLS 1.3 Requires either ECC or (RSA/DH), or CURVE25519/ED25519 or CURVE448/ED448
if (NOT WOLFSSL_ECC AND
    (NOT WOLFSSL_RSA OR NOT WOLFSSL_DH) AND
    (NOT WOLFSSL_CURVE25519 OR NOT WOLFSSL_ED25519) AND
    (NOT WOLFSSL_CURVE448 AND NOT WOLFSSL_ED448))
    override_cache(WOLFSSL_TLS13 "no")
endif()
if (WOLFSSL_TLS13)
    list(APPEND WOLFSSL_DEFINITIONS
        "-DHAVE_SUPPORTED_CURVES"
        "-DWOLFSSL_TLS13"
        "-DHAVE_TLS_EXTENSIONS"
        )
endif()

# Session Ticket Extension
add_option("WOLFSSL_SESSION_TICKET"
    "Enable Session Ticket (default: disabled)"
    "no" "yes;no")

if(WOLFSSL_NGINX OR WOLFSSL_WPAS OR WOLFSSL_HAPROXY OR WOLFSSL_LIGHTY)
    override_cache(WOLFSSL_SESSION_TICKET "yes")
endif()

if(WOLFSSL_SESSION_TICKET)
    list(APPEND WOLFSSL_DEFINITIONS
        "-DHAVE_TLS_EXTENSIONS"
        "-DHAVE_SESSION_TICKET")
endif()

add_option("WOLFSSL_TICKET_NONCE_MALLOC"
    "Enable dynamic allocation of ticket nonces (default: disabled)"
    "no" "yes;no")

if(WOLFSSL_TICKET_NONCE_MALLOC)
    list(APPEND WOLFSSL_DEFINITIONS
        "-DWOLFSSL_TICKET_NONCE_MALLOC")
endif()

# Extended master secret extension
add_option("WOLFSSL_EXTENDED_MASTER"
    "Enable Extended Master Secret (default: enabled)"
    "yes" "yes;no")

if(WOLFSSL_EXTENDED_MASTER)
    list(APPEND WOLFSSL_DEFINITIONS "-DHAVE_EXTENDED_MASTER")
endif()


if(NOT WOLFSSL_ARC4)
    list(APPEND WOLFSSL_DEFINITIONS "-DNO_RC4")
else()
    # turn off ARC4 if leanpsk or leantls on
    if(WOLFSSL_LEAN_PSK OR WOLFSSL_LEAN_TLS)
        list(APPEND WOLFSSL_DEFINITIONS "-DNO_RC4")
        override_cache(WOLFSSL_ARC4 "no")
    endif()
endif()

# TODO: - TLS extensions
#       - Early data handshake
#       - wolfSSH options
#       - SCEP
#       - Secure remote password
#       - Indefinite length encoded messages
#       - Small stack cache
#       - Small stack
#       - Valgrind
#       - Test certs
#       - I/O pool example
#       - Certificate service
#       - wolfSSL JNI
#       - lighttpd/lighty
#       - Asio
#       - Apache HTTPD

set(WOLFSSL_PKCS7_HELP_STRING "Enable PKCS7 (default: disabled)")
add_option(WOLFSSL_PKCS7 ${WOLFSSL_PKCS7_HELP_STRING} "no" "yes;no")

set(WOLFSSL_TPM_HELP_STRING "Enable wolfTPM options (default: disabled)")
add_option(WOLFSSL_TPM ${WOLFSSL_TPM_HELP_STRING} "no" "yes;no")

set(WOLFSSL_AESKEYWRAP_HELP_STRING "Enable AES key wrap support (default: disabled)")
add_option(WOLFSSL_AESKEYWRAP ${WOLFSSL_AESKEYWRAP_HELP_STRING} "no" "yes;no")

set(WOLFSSL_X963KDF_HELP_STRING "Enable X9.63 KDF support (default: disabled)")
add_option(WOLFSSL_X963KDF ${WOLFSSL_X963KDF_HELP_STRING} "no" "yes;no")


# Encrypt-then-mac
add_option("WOLFSSL_ENC_THEN_MAC"
    "Enable Encryptr-Then-Mac extension (default: enabled)"
    "yes" "yes;no")

if(WOLFSSL_APACHE_HTTPD)
    override_cache(WOLFSSL_ENC_THEN_MAC "no")
endif()

if(WOLFSSL_TLSX)
    override_cache(WOLFSSL_ENC_THEN_MAC "yes")
endif()

if(WOLFSSL_SNIFFER)
    override_cache(WOLFSSL_ENC_THEN_MAC "no")
endif()

# stunnel Support
# TODO: rest of stunnel support
add_option("WOLFSSL_STUNNEL"
    "Enable stunnel (default: disabled)"
    "no" "yes;no")

if(WOLFSSL_ENC_THEN_MAC)
    list(APPEND WOLFSSL_DEFINITIONS "-DHAVE_ENCRYPT_THEN_MAC")
endif()

if(NOT WOLFSSL_PSK AND
   NOT WOLFSSL_LEAN_PSK AND
   NOT WOLFSSL_STUNNEL)
    list(APPEND WOLFSSL_DEFINITIONS "-DNO_PSK")
endif()

# MD4
set(WOLFSSL_MD4_HELP_STRING "Enable MD4 (default: disabled)")
add_option("WOLFSSL_MD4" ${WOLFSSL_MD4_HELP_STRING} "no" "yes;no")

if(NOT WOLFSSL_MD4)
    # turn on MD4 if using stunnel
    if(WOLFSSL_STUNNEL OR WOLFSSL_WPAS)
        override_cache(WOLFSSL_MD4 "yes")
    else()
        list(APPEND WOLFSSL_DEFINITIONS "-DNO_MD4")
    endif()
endif()

#  Encrypted keys
add_option("WOLFSSL_ENCKEYS"
    "Enable PEM encrypted key support (default: disabled)"
    "no" "yes;no")

if(NOT WOLFSSL_ENCKEYS)
    if(WOLFSSL_OPENSSLEXTRA OR
        WOLFSSL_WEBSERVER OR
        WOLFSSL_WPAS)
        # opensslextra, webserver, and WPAS  needs enckeys
        override_cache(WOLFSSL_ENCKEYS "yes")
    endif()
endif()

if(WOLFSSL_ENCKEYS)
    list(APPEND WOLFSSL_DEFINITIONS "-DWOLFSSL_ENCRYPTED_KEYS")
endif()

# PKCS#12
set(WOLFSSL_PKCS12_HELP_STRING "Enable pkcs12 (default: enabled)")
add_option("WOLFSSL_PKCS12" ${WOLFSSL_PKCS12_HELP_STRING} "yes" "yes;no")
if(NOT WOLFSSL_ASN)
  override_cache(WOLFSSL_PKCS12 "no")
endif()

if(NOT WOLFSSL_PKCS12)
  list(APPEND WOLFSSL_DEFINITIONS "-DNO_PKCS12")
endif()


# PWDBASED has to come after certservice since we want it on w/o explicit on
# PWDBASED
add_option("WOLFSSL_PWDBASED"
    "Enable PWDBASED (default: disabled)"
    "no" "yes;no")

if(NOT WOLFSSL_PWDBASED)
    if(WOLFSSL_OPENSSLEXTRA OR
        WOLFSSL_OPENSSLALL OR
        WOLFSSL_WEBSERVER OR
        WOLFSSL_ENC_KEYS OR
        WOLFSSL_PKCS12)
        # opensslextra, opensslall, webserver, and enckeys needs pwdbased
        override_cache(WOLFSSL_PWDBASED "yes")
    else()
        list(APPEND WOLFSSL_DEFINITIONS "-DNO_PWDBASED")
    endif()
endif()

# TODO: - SCRYPT
#       - wolfCrypt only

# fastmath
add_option("WOLFSSL_FAST_MATH"
    "Enable fast math ops (default: disabled)"
    "no" "yes;no")

if(WOLFSSL_FAST_MATH)
    # turn off fastmath if leanpsk on or asn off (w/o DH and ECC)
    if(WOLFSSL_LEAN_PSK OR NOT WOLFSSL_ASN)
        if(NOT WOLFSSL_DH AND
           NOT WOLFSSL_ECC AND
           NOT WOLFSSL_RSA)
            override_cache(WOLFSSL_FAST_MATH "no")
        else()
            list(APPEND WOLFSSL_DEFINITIONS "-DUSE_FAST_MATH")
            set(WOLFSSL_SLOWMATH "no")
        endif()
    else()
        list(APPEND WOLFSSL_DEFINITIONS "-DUSE_FAST_MATH")
        set(WOLFSSL_SLOWMATH "no")
    endif()

    if("${CMAKE_SYSTEM_PROCESSOR}" MATCHES "x86_64|AMD64")
        # Have settings.h set FP_MAX_BITS higher if user didn't set directly
        list(APPEND WOLFSSL_DEFINITIONS "-DWOLFSSL_X86_64_BUILD")
    endif()
endif()

# TODO: - Fast huge math

if("${CMAKE_SYSTEM_PROCESSOR}" MATCHES "x86_64|AMD64")
    list(APPEND WOLFSSL_DEFINITIONS "-DWOLFSSL_X86_64_BUILD")
endif()

# SP math all
add_option("WOLFSSL_SP_MATH_ALL"
    "Enable Single Precision math implementation for full algorithm suite (default: enabled)"
    "yes" "yes;no")

# Enable examples, used to disable examples
if(WOLFSSL_LINUX_KM)
    set(EXAMPLES_DEFAULT "no")
else()
    set(EXAMPLES_DEFAULT "yes")
endif()

add_option("WOLFSSL_EXAMPLES"
    "Enable examples (default: enabled)"
    ${EXAMPLES_DEFAULT} "yes;no")

if(NOT WOLFSSL_FILESYSTEM OR
   NOT WOLFSSL_INLINE OR
   WOLFSSL_CRYPT_ONLY)
    override_cache(WOLFSSL_EXAMPLES "no")
endif()

# Enable wolfCrypt test and benchmark
if(WOLFSSL_LINUX_KM)
    set(CRYPT_TESTS_DEFAULT "no")
else()
    set(CRYPT_TESTS_DEFAULT "yes")
endif()

add_option("WOLFSSL_CRYPT_TESTS"
    "Enable Crypt Bench/Test (default: enabled)"
    ${CRYPT_TESTS_DEFAULT} "yes;no")

add_option("WOLFSSL_CRYPT_TESTS_LIBS"
    "Build static libraries from the wolfCrypt test and benchmark sources (default: disabled)"
    "no" "yes;no")

add_option("WOLFSSL_CRYPT_TESTS_HELP"
    "Add help text to wolfCrypt test (default: disabled)"
    "no" "yes;no")

# TODO: - LIBZ
#       - PKCS#11
#       - Cavium
#       - Cavium V
#       - Cavium Octeon
#       - Intel QuickAssist
#       - SP ASM (and other SP logic)
#       - Fast RSA
#       - Static memory use
#       - Microchip API
#       - Asynchronous crypto

# Asynchronous threading
add_option("WOLFSSL_ASYNC_THREADS"
    "Enable Asynchronous Threading (default: enabled)"
    "yes" "yes;no")

if(WOLFSSL_ASYNC_CRYPT AND WOLFSSL_ASYNC_THREADS)
    if(CMAKE_USE_PTHREADS_INIT)
        override_cache(WOLFSSL_ASYNC_THREADS "yes")
    else()
        override_cache(WOLFSSL_ASYNC_THREADS "no")
    endif()
else()
    override_cache(WOLFSSL_ASYNC_THREADS "no")
endif()

if(WOLFSSL_ASYNC_THREADS)
    list(APPEND WOLFSSL_LINK_LIBS Threads::Threads)
    list(APPEND WOLFSSL_DEFINITIONS "-D_GNU_SOURCE")
else()
    list(APPEND WOLFSSL_DEFINITIONS "-DWC_NO_ASYNC_THREADING")
endif()

# TODO: - cryptodev
#       - Session export

add_option("WOLFSSL_CRYPTOCB"
    "Enable crypto callbacks (default: disabled)"
    "no" "yes;no")


add_option("WOLFSSL_OLD_NAMES"
    "Keep backwards compat with old names (default: enabled)"
    "yes" "yes;no")

if(NOT WOLFSSL_OLD_NAMES AND NOT WOLFSSL_OPENSSL_COEXIST)
    list(APPEND WOLFSSL_DEFINITIONS
        "-DNO_OLD_RNGNAME"
        "-DNO_OLD_WC_NAMES"
        "-DNO_OLD_SSL_NAMES"
        "-DNO_OLD_SHA_NAMES")
endif()

# TODO: - Memory tests
#       - Hash flags

# Support for enabling setting default DH parameters
add_option("WOLFSSL_DH_DEFAULT_PARAMS"
    "Enables option for default dh parameters (default: disabled)"
    "no" "yes;no")

if(WOLFSSL_DH_DEFAULT_PARAMS OR NOT WOLFSSL_QT)
    override_cache(WOLFSSL_DH_DEFAULT_PARAMS "yes")
    list(APPEND WOLFSSL_DEFINITIONS "-DHAVE_DH_DEFAULT_PARAMS")
endif()

if(NOT WOLFSSL_DES3)
    list(APPEND WOLFSSL_DEFINITIONS "-DNO_DES3")
else()
    # turn off DES3 if leanpsk or leantls on
    if(WOLFSSL_LEAN_PSK OR WOLFSSL_LEAN_TLS)
        list(APPEND WOLFSSL_DEFINITIONS "-DNO_DES3")
        override_cache(WOLFSSL_DES3 "no")
    endif()
endif()

add_option("WOLFSSL_USER_SETTINGS"
    "Use your own user_settings.h and do not add Makefile CFLAGS (default: disabled)"
    "no" "yes;no")

add_option("WOLFSSL_USER_SETTINGS_ASM"
    "Enable use of user_settings_asm.h in assembly files (default: disabled)"
    "no" "yes;no")

add_option("WOLFSSL_OPTFLAGS"
    "Enable default optimization CFLAGS for the compiler (default: enabled)"
    "yes" "yes;no")

add_option("WOLFSSL_SYS_CA_CERTS"
    "Enable ability to load CA certs from OS (default: enabled)"
    "yes" "yes;no")

if(WOLFSSL_SYS_CA_CERTS)
    if(NOT WOLFSSL_FILESYSTEM)
        message("Can't enable system CA certs without a filesystem.")
        override_cache(WOLFSSL_SYS_CA_CERTS "no")
    elseif(APPLE)
        check_include_file("Security/SecTrustSettings.h" HAVE_SECURITY_SECTRUSTSETTINGS_H)
        if(NOT HAVE_SECURITY_SECTRUSTSETTINGS_H)
            message("Can't enable system CA certs without Security/SecTrustSettings.h.")
            override_cache(WOLFSSL_SYS_CA_CERTS "no")
        else()
            find_library(CORE_FOUNDATION_FRAMEWORK CoreFoundation)
            if(NOT CORE_FOUNDATION_FRAMEWORK)
                message("Can't enable system CA certs without CoreFoundation framework.")
                override_cache(WOLFSSL_SYS_CA_CERTS "no")
            else()
                find_library(SECURITY_FRAMEWORK Security)
                if(NOT SECURITY_FRAMEWORK)
                    message("Can't enable system CA certs without Security framework.")
                    override_cache(WOLFSSL_SYS_CA_CERTS "no")
                endif()
            endif()
        endif()
    endif()


    if(WOLFSSL_SYS_CA_CERTS)
        list(APPEND WOLFSSL_DEFINITIONS "-DWOLFSSL_SYS_CA_CERTS")
    endif()
endif()

# FLAGS operations

if(WOLFSSL_AESCCM)
    list(APPEND WOLFSSL_DEFINITIONS "-DHAVE_AESCCM")
endif()

if(WOLFSSL_AESOFB)
    list(APPEND WOLFSSL_DEFINITIONS "-DWOLFSSL_AES_OFB" "-DWOLFSSL_AES_DIRECT")
endif()

if(WOLFSSL_TPM)
    override_cache(WOLFSSL_KEYGEN   "yes")
    override_cache(WOLFSSL_CERTGEN  "yes")
    override_cache(WOLFSSL_CRYPTOCB "yes")
    override_cache(WOLFSSL_CERTREQ  "yes")
    override_cache(WOLFSSL_CERTEXT  "yes")
    override_cache(WOLFSSL_PKCS7    "yes")
    override_cache(WOLFSSL_AESCFB   "yes")
endif()

if(WOLFSSL_AESCFB)
    list(APPEND WOLFSSL_DEFINITIONS "-DWOLFSSL_AES_CFB")
endif()


if(WOLFSSL_PKCS7)
    list(APPEND WOLFSSL_DEFINITIONS "-DHAVE_PKCS7")
    override_cache(WOLFSSL_AESKEYWRAP "yes")
    # Enable prereqs if not already enabled
    if(WOLFSSL_ECC)
        override_cache(WOLFSSL_X963KDF "yes")
    endif()
endif()

if(WOLFSSL_X963KDF)
    list(APPEND WOLFSSL_DEFINITIONS "-DHAVE_X963_KDF")
endif()

if(WOLFSSL_AESKEYWRAP)
    list(APPEND WOLFSSL_DEFINITIONS
        "-DHAVE_AES_KEYWRAP"
        "-DWOLFSSL_AES_DIRECT"
        )
endif()

# Hybrid Public Key Encryption (RFC9180)
add_option("WOLFSSL_HPKE"
        "Enable wolfSSL hybrid public key encryption (default: disabled)"
        "no" "yes;no")

# Encrypted Client Hello (ECH)
add_option("WOLFSSL_ECH"
        "Enable wolfSSL encrypted client hello (default: disabled)"
        "no" "yes;no")

# Keying Material Exporter / TLS Exporter
add_option("WOLFSSL_KEYING_MATERIAL"
        "Enable wolfSSL keying material export (default: disabled)"
        "no" "yes;no")

if(WOLFSSL_HPKE)
    if(NOT WOLFSSL_ECC)
        message(FATAL_ERROR "HPKE supported only with ECC (WOLFSSL_ECC)")
    endif()
    list(APPEND WOLFSSL_DEFINITIONS "-DHAVE_HPKE")
    override_cache(WOLFSSL_HKDF "yes")
endif()

if(WOLFSSL_ECH)
    if(NOT WOLFSSL_HPKE)
        message(FATAL_ERROR "ECH supported only with HPKE (WOLFSSL_HPKE)")
    endif()
    if(NOT WOLFSSL_SNI)
        message(FATAL_ERROR "ECH supported only with SNI (WOLFSSL_SNI)")
    endif()
    list(APPEND WOLFSSL_DEFINITIONS "-DHAVE_ECH")
endif()

if(WOLFSSL_KEYING_MATERIAL)
    list(APPEND WOLFSSL_DEFINITIONS "-DHAVE_KEYING_MATERIAL")
endif()

if(WOLFSSL_KEYGEN)
    list(APPEND WOLFSSL_DEFINITIONS "-DWOLFSSL_KEY_GEN")
endif()
if(WOLFSSL_CERTGEN)
    list(APPEND WOLFSSL_DEFINITIONS "-DWOLFSSL_CERT_GEN")
endif()
if(WOLFSSL_CERTREQ)
    list(APPEND WOLFSSL_DEFINITIONS "-DWOLFSSL_CERT_REQ")
endif()
if(WOLFSSL_CERTEXT)
    list(APPEND WOLFSSL_DEFINITIONS "-DWOLFSSL_CERT_EXT")
endif()
if(WOLFSSL_CERTGENCACHE)
    list(APPEND WOLFSSL_DEFINITIONS "-DWOLFSSL_CERT_GEN_CACHE")
endif()

if(WOLFSSL_CRYPTOCB)
    list(APPEND WOLFSSL_DEFINITIONS "-DWOLF_CRYPTO_CB")
endif()

if(WOLFSSL_OCSPSTAPLING)
    list(APPEND WOLFSSL_DEFINITIONS "-DHAVE_CERTIFICATE_STATUS_REQUEST" "-DHAVE_TLS_EXTENSIONS")
    override_cache(WOLFSSL_OCSP "yes")
endif()

if(WOLFSSL_OCSPSTAPLING_V2)
    list(APPEND WOLFSSL_DEFINITIONS "-DHAVE_CERTIFICATE_STATUS_REQUEST_V2" "-DHAVE_TLS_EXTENSIONS")
    override_cache(WOLFSSL_OCSP "yes")
endif()

# must be below OCSP stapling options to allow override
if (WOLFSSL_OCSP)
   list(APPEND WOLFSSL_DEFINITIONS "-DHAVE_OCSP")
endif()

if (WOLFSSL_CRL STREQUAL "yes")
    list(APPEND WOLFSSL_DEFINITIONS "-DHAVE_CRL")
elseif(WOLFSSL_CRL STREQUAL "io")
    list(APPEND WOLFSSL_DEFINITIONS "-DHAVE_CRL" "-DHAVE_CRL_IO")
endif()

if (WOLFSSL_SNI)
   list(APPEND WOLFSSL_DEFINITIONS "-DHAVE_TLS_EXTENSIONS" "-DHAVE_SNI")
endif()

if (WOLFSSL_TLSX)
  list(APPEND WOLFSSL_DEFINITIONS
    "-DHAVE_TLS_EXTENSIONS"
    "-DHAVE_SNI"
    "-DHAVE_MAX_FRAGMENT"
    "-DHAVE_TRUNCATED_HMAC"
    "-DHAVE_ALPN"
    "-DHAVE_TRUSTED_CA")
    if (WOLFSSL_ECC OR WOLFSSL_CURVE25519 OR WOLFSSL_CURVE448 OR WOLFSSL_TLS13)
       list(APPEND WOLFSSL_DEFINITIONS  "-DHAVE_SUPPORTED_CURVES")
    endif()
endif()


add_option("WOLFSSL_CAAM"
    "Enable use of CAAM with NXP (default: disabled)"
    "no" "yes;no")
if (WOLFSSL_CAAM)
   list(APPEND WOLFSSL_DEFINITIONS  "-DWOLFSSL_CAAM")
endif()


# Generates the BUILD_* flags. These control what source files are included in
# the library. A series of AM_CONDITIONALs handle this in configure.ac.
generate_build_flags()

# TODO: - Bit of logic after optimization flags option (above)
#       - Check for build-type conflicts section

# USER SETTINGS
if(WOLFSSL_USER_SETTINGS)
    # Replace all options and just use WOLFSSL_USER_SETTINGS
    set(WOLFSSL_DEFINITIONS "-DWOLFSSL_USER_SETTINGS")
endif()

if(WOLFSSL_USER_SETTINGS_ASM)
    if(WOLFSSL_USER_SETTINGS)
        list(APPEND WOLFSSL_DEFINITIONS "-DWOLFSSL_USER_SETTINGS_ASM")
        # Create user_settings_asm.h for use in assembly files (e.g. .S files).
        if(WIN32)
            execute_process(COMMAND
            $ENV{SHELL} ${CMAKE_CURRENT_SOURCE_DIR}/scripts/user_settings_asm.sh
            "${CMAKE_C_FLAGS} ${CMAKE_CXX_FLAGS}"
            RESULT_VARIABLE USER_SETTINGS_ASM_RET)
        else()
            execute_process(COMMAND
            ${CMAKE_CURRENT_SOURCE_DIR}/scripts/user_settings_asm.sh
            "${CMAKE_C_FLAGS} ${CMAKE_CXX_FLAGS}"
            RESULT_VARIABLE USER_SETTINGS_ASM_RET)
        endif()
        if (NOT USER_SETTINGS_ASM_RET EQUAL 0)
            message(FATAL_ERROR
            "${CMAKE_CURRENT_SOURCE_DIR}/scripts/user_settings_asm.sh failed.")
        endif()
    else()
        message(FATAL_ERROR
        "Must have WOLFSSL_USER_SETTINGS to enable WOLFSSL_USER_SETTINGS_ASM.")
    endif()
endif()

add_option("WOLFSSL_CONFIG_H"
    "Enable generation of config.h and define HAVE_CONFIG_H (default: enabled)"
    "yes" "yes;no")

if(WOLFSSL_CONFIG_H)
    add_definitions("-DHAVE_CONFIG_H")
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/config.in"
                   "${CMAKE_CURRENT_BINARY_DIR}/config.h" )
    set(abs_top_srcdir ${CMAKE_CURRENT_SOURCE_DIR})
    set(abs_top_builddir ${CMAKE_CURRENT_BINARY_DIR})
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/wolfcrypt/test/test_paths.h.in"
                   "${CMAKE_CURRENT_BINARY_DIR}/wolfcrypt/test/test_paths.h" )
endif()

# If config.h or wolfssl/options.h exists, delete it to avoid
# a mixup with build/wolfssl/options.h.
if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/wolfssl/options.h")
    file(REMOVE "${CMAKE_CURRENT_SOURCE_DIR}/wolfssl/options.h")
endif()

if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/config.h")
    file(REMOVE "${CMAKE_CURRENT_SOURCE_DIR}/config.h")
endif()

# Suppress some warnings about separate compilation, inlining
add_definitions("-DWOLFSSL_IGNORE_FILE_WARN")
# Generate user options header
message("Generating user options header...")
if (${CMAKE_DISABLE_SOURCE_CHANGES})
    set(WOLFSSL_BUILD_OUT_OF_TREE_DEFAULT "${CMAKE_DISABLE_SOURCE_CHANGES}")
else()
    set(WOLFSSL_BUILD_OUT_OF_TREE_DEFAULT "yes")
endif()
add_option("WOLFSSL_BUILD_OUT_OF_TREE"
    "Don't generate files in the source tree (default: ${WOLFSSL_BUILD_OUT_OF_TREE_DEFAULT})"
    "${WOLFSSL_BUILD_OUT_OF_TREE_DEFAULT}" "yes;no")
if (${WOLFSSL_BUILD_OUT_OF_TREE})
   set(WOLFSSL_OUTPUT_BASE ${CMAKE_CURRENT_BINARY_DIR})
else()
   set(WOLFSSL_OUTPUT_BASE ${CMAKE_CURRENT_SOURCE_DIR})
endif()
set(OPTION_FILE "${WOLFSSL_OUTPUT_BASE}/wolfssl/options.h")

file(REMOVE ${OPTION_FILE})

file(APPEND ${OPTION_FILE} "/* wolfssl options.h\n")
file(APPEND ${OPTION_FILE} " * generated from configure options\n")
file(APPEND ${OPTION_FILE} " *\n")
file(APPEND ${OPTION_FILE} " * Copyright (C) 2006-2023 wolfSSL Inc.\n")
file(APPEND ${OPTION_FILE} " *\n")
file(APPEND ${OPTION_FILE} " * This file is part of wolfSSL. (formerly known as CyaSSL)\n")
file(APPEND ${OPTION_FILE} " *\n")
file(APPEND ${OPTION_FILE} " */\n\n")
file(APPEND ${OPTION_FILE} "#ifndef WOLFSSL_OPTIONS_H\n")
file(APPEND ${OPTION_FILE} "#define WOLFSSL_OPTIONS_H\n\n\n")
file(APPEND ${OPTION_FILE} "#ifdef __cplusplus\n")
file(APPEND ${OPTION_FILE} "extern \"C\" {\n")
file(APPEND ${OPTION_FILE} "#endif\n\n")

add_to_options_file("${WOLFSSL_DEFINITIONS}" "${OPTION_FILE}")
# CMAKE_C_FLAGS is just a string of space-separated flags to pass to the C
# compiler. We need to replace those spaces with semicolons in order to treat it
# as a CMake list.
string(REPLACE " " ";" CMAKE_C_FLAGS_LIST "${CMAKE_C_FLAGS}")
add_to_options_file("${CMAKE_C_FLAGS_LIST}" "${OPTION_FILE}")

file(APPEND ${OPTION_FILE} "\n#ifdef __cplusplus\n")
file(APPEND ${OPTION_FILE} "}\n")
file(APPEND ${OPTION_FILE} "#endif\n\n\n")
file(APPEND ${OPTION_FILE} "#endif /* WOLFSSL_OPTIONS_H */\n\n")

####################################################
# Library Target
####################################################

# TODO: - Build shared/static libs based on enables. Check CMake
#         global flag BUILD_SHARED_LIBS.
option(BUILD_SHARED_LIBS "Build shared libraries (.dll/.so) instead of static ones (.lib/.a)" ON)

set(LIB_SOURCES "")
# Generates a list of sources to include in the library.
# Corresponds to the instances of "src_libwolfssl_la_SOURCES += ..."
# in the *.am files.
generate_lib_src_list("${LIB_SOURCES}")
if(BUILD_SHARED_LIBS)
    add_library(wolfssl SHARED ${LIB_SOURCES})
else()
    add_library(wolfssl STATIC ${LIB_SOURCES})
endif()

add_library(wolfssl::wolfssl ALIAS wolfssl)

set_target_properties(wolfssl
    PROPERTIES
        SOVERSION ${LIBTOOL_SO_VERSION}
        VERSION ${LIBTOOL_FULL_VERSION}
)

target_compile_definitions(wolfssl PRIVATE "BUILDING_WOLFSSL")
if(${BUILD_SHARED_LIBS})
    target_compile_definitions(wolfssl PUBLIC "WOLFSSL_DLL")
endif()
target_compile_definitions(wolfssl PUBLIC ${WOLFSSL_DEFINITIONS})

####################################################
# Include Directories
####################################################

target_include_directories(wolfssl
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        ${WOLFSSL_INCLUDE_DIRS}
)

####################################################
# Link Libraries
####################################################

target_link_libraries(wolfssl PUBLIC ${WOLFSSL_LINK_LIBS})

if(WIN32)
    # For Windows link ws2_32
    target_link_libraries(wolfssl PUBLIC
        $<$<PLATFORM_ID:Windows>:ws2_32 crypt32>)
elseif(APPLE)
    if(WOLFSSL_SYS_CA_CERTS)
        target_link_libraries(wolfssl PUBLIC
            ${CORE_FOUNDATION_FRAMEWORK}
            ${SECURITY_FRAMEWORK})
    endif()
else()
    if(WOLFSSL_DH AND NOT WOLFSSL_DH_CONST)
        # DH requires math (m) library
        target_link_libraries(wolfssl
            PUBLIC
                m)
    endif()
endif()

####################################################
# Tests and Examples
####################################################

if(WOLFSSL_EXAMPLES)
    # Build wolfSSL client example
    add_executable(client
        ${CMAKE_CURRENT_SOURCE_DIR}/examples/client/client.c)
    target_link_libraries(client wolfssl)
    set_property(TARGET client
                 PROPERTY RUNTIME_OUTPUT_DIRECTORY
                 ${WOLFSSL_OUTPUT_BASE}/examples/client)

    # Build wolfSSL server example
    add_executable(server
        ${CMAKE_CURRENT_SOURCE_DIR}/examples/server/server.c)
    target_link_libraries(server wolfssl)
    set_property(TARGET server
                 PROPERTY RUNTIME_OUTPUT_DIRECTORY
                 ${WOLFSSL_OUTPUT_BASE}/examples/server)

    # Build echo client example
    add_executable(echoclient
        ${CMAKE_CURRENT_SOURCE_DIR}/examples/echoclient/echoclient.c)
    target_include_directories(echoclient PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR})
    target_link_libraries(echoclient wolfssl)
    set_property(TARGET echoclient
                 PROPERTY RUNTIME_OUTPUT_DIRECTORY
                 ${WOLFSSL_OUTPUT_BASE}/examples/echoclient)

    # Build echo server example
    add_executable(echoserver
        ${CMAKE_CURRENT_SOURCE_DIR}/examples/echoserver/echoserver.c)
    target_include_directories(echoserver PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR})
    target_link_libraries(echoserver wolfssl)
    set_property(TARGET echoserver
                 PROPERTY RUNTIME_OUTPUT_DIRECTORY
                 ${WOLFSSL_OUTPUT_BASE}/examples/echoserver)

    if(NOT WIN32)
        # Build TLS benchmark example
        add_executable(tls_bench
            ${CMAKE_CURRENT_SOURCE_DIR}/examples/benchmark/tls_bench.c)
        target_link_libraries(tls_bench wolfssl)
        target_link_libraries(tls_bench Threads::Threads)
        set_property(TARGET tls_bench
                 PROPERTY RUNTIME_OUTPUT_DIRECTORY
                 ${WOLFSSL_OUTPUT_BASE}/examples/benchmark)
    endif()

    # Build unit tests
    add_executable(unit_test
        tests/api.c
        tests/hash.c
        tests/srp.c
        tests/suites.c
        tests/w64wrapper.c
        tests/unit.c
        examples/server/server.c
        examples/client/client.c)
    target_include_directories(unit_test PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR})
    target_compile_options(unit_test PUBLIC "-DNO_MAIN_DRIVER")
    target_link_libraries(unit_test wolfssl)
    target_link_libraries(unit_test Threads::Threads)
    set_property(TARGET unit_test
                 PROPERTY RUNTIME_OUTPUT_DIRECTORY
                 ${WOLFSSL_OUTPUT_BASE}/tests/)
    set_property(TARGET unit_test
                 PROPERTY RUNTIME_OUTPUT_NAME
                 unit.test)
endif()

if(WOLFSSL_CRYPT_TESTS)
    if(WOLFSSL_CRYPT_TESTS_LIBS)
        # Build wolfCrypt test as a library. This will compile test.c and make
        # its functions available as a CMake target that other CMake targets can
        # pull in, in addition to producing the library itself. Note that this
        # feature is not enabled by default, and the API of this library and
        # wofcryptbench_lib should NOT be treated as stable.
        add_library(wolfcrypttest_lib
            ${CMAKE_CURRENT_SOURCE_DIR}/wolfcrypt/test/test.c)
        set_target_properties(wolfcrypttest_lib PROPERTIES OUTPUT_NAME "wolfcrypttest")
        target_link_libraries(wolfcrypttest_lib wolfssl)
        target_compile_options(wolfcrypttest_lib PRIVATE "-DNO_MAIN_DRIVER")
        if(WOLFSSL_CRYPT_TESTS_HELP)
            target_compile_options(wolfcrypttest_lib PRIVATE "-DHAVE_WOLFCRYPT_TEST_OPTIONS")
        endif()

        # Make another library for the wolfCrypt benchmark code.
        add_library(wolfcryptbench_lib
            ${CMAKE_CURRENT_SOURCE_DIR}/wolfcrypt/benchmark/benchmark.c)
        set_target_properties(wolfcryptbench_lib PROPERTIES OUTPUT_NAME "wolfcryptbench")
        target_link_libraries(wolfcryptbench_lib wolfssl)
        target_compile_options(wolfcryptbench_lib PRIVATE "-DNO_MAIN_DRIVER")
    endif()

    # Build wolfCrypt test executable.
    add_executable(wolfcrypttest
        ${CMAKE_CURRENT_SOURCE_DIR}/wolfcrypt/test/test.c)
    target_link_libraries(wolfcrypttest wolfssl)
    set_property(TARGET wolfcrypttest
                 PROPERTY RUNTIME_OUTPUT_DIRECTORY
                 ${WOLFSSL_OUTPUT_BASE}/wolfcrypt/test)
    set_property(TARGET wolfcrypttest
                 PROPERTY RUNTIME_OUTPUT_NAME
                 testwolfcrypt)
    if(WOLFSSL_CRYPT_TESTS_HELP)
        target_compile_options(wolfcrypttest PRIVATE "-DHAVE_WOLFCRYPT_TEST_OPTIONS")
    endif()

    # Build wolfCrypt benchmark executable.
    add_executable(wolfcryptbench
        ${CMAKE_CURRENT_SOURCE_DIR}/wolfcrypt/benchmark/benchmark.c)
    target_include_directories(wolfcryptbench PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR})
    target_link_libraries(wolfcryptbench wolfssl)
    set_property(TARGET wolfcryptbench
                 PROPERTY RUNTIME_OUTPUT_DIRECTORY
                 ${WOLFSSL_OUTPUT_BASE}/wolfcrypt/benchmark)
    set_property(TARGET wolfcryptbench
                 PROPERTY RUNTIME_OUTPUT_NAME
                 benchmark)
endif()

####################################################
# Installation
####################################################

include(GNUInstallDirs)

set(HEADER_EXCLUDE
  "internal.h"
  "wolfssl/wolfcrypt/port/nrf51.h"
  "wolfssl/wolfcrypt/port/arm"
  "wolfssl/wolfcrypt/port/cypress"
  "wolfssl/wolfcrypt/port/Espressif"
  "wolfssl/wolfcrypt/port/iotsafe"
  "wolfssl/wolfcrypt/port/nxp"
  "wolfssl/wolfcrypt/port/pic"
  "wolfssl/wolfcrypt/port/Renesas"
  "wolfssl/wolfcrypt/port/silabs"
  "wolfssl/wolfcrypt/port/st"
  "wolfssl/wolfcrypt/port/ti"
  "wolfssl/wolfcrypt/port/xilinx"
  )

# TODO: add support for the various ports

# For distro build don't install options.h.
# It depends on the architecture and conflicts with Multi-Arch.
if(BUILD_DISTRO)
  list(APPEND HEADER_EXCLUDE
    "options.h")
endif()

if(NOT BUILD_CRYPTOAUTHLIB)
  list(APPEND HEADER_EXCLUDE
    "wolfssl/wolfcrypt/port/atmel")
endif()

if(NOT BUILD_AFALG)
  list(APPEND HEADER_EXCLUDE
    "wolfssl/wolfcrypt/port/af_alg")
endif()

if(NOT BUILD_KCAPI)
  list(APPEND HEADER_EXCLUDE
    "wolfssl/wolfcrypt/port/kcapi"
    )
endif()

if(NOT BUILD_DEVCRYPTO)
  list(APPEND HEADER_EXCLUDE
    "wolfssl/wolfcrypt/port/devcrypto")
endif()

if(NOT BUILD_ASYNCCRYPT)
  list(APPEND HEADER_EXCLUDE
    "wolfssl/wolfcrypt/async.h")
endif()

if(NOT BUILD_PKCS11)
    list(APPEND HEADER_EXCLUDE
      "wolfssl/wolfcrypt/wc_pkcs11.h"
      "wolfssl/wolfcrypt/pkcs11.h"
      )
endif()

if(NOT BUILD_CAVIUM AND NOT BUILD_OCTEON_SYNC)
  list(APPEND HEADER_EXCLUDE
    "wolfssl/wolfcrypt/port/cavium")
else()
  if(NOT BUILD_CAVIUM)
    list(APPEND HEADER_EXCLUDE
      "wolfssl/wolfcrypt/port/cavium/cavium_nitrox.h")
  endif()

  if(NOT BUILD_OCTEON_SYNC)
    list(APPEND HEADER_EXCLUDE
      "wolfssl/wolfcrypt/port/cavium/cavium_octeon_sync.h"
      )
  endif()
endif()


if(NOT BUILD_INTEL_QA AND NOT BUILD_INTEL_QA_SYNC)
  list(APPEND HEADER_EXCLUDE
    "wolfssl/wolfcrypt/port/intel")
else()
  if(NOT BUILD_INTEL_QA)
    list(APPEND HEADER_EXCLUDE
      "wolfssl/wolfcrypt/port/intel/quickassist.h"
      "wolfssl/wolfcrypt/port/intel/quickassist_mem.h"
      )
  endif()

  if(NOT BUILD_INTEL_QA_SYNC)
    list(APPEND HEADER_EXCLUDE
      "wolfssl/wolfcrypt/port/intel/quickassist_sync.h")
  endif()
endif()

if(NOT BUILD_SP)
  list(APPEND HEADER_EXCLUDE
    "wolfssl/wolfcrypt/sp.h")
endif()

if(NOT BUILD_SP_INT)
  list(APPEND HEADER_EXCLUDE
    "wolfssl/wolfcrypt/sp_int.h")
endif()

if(NOT BUILD_SELFTEST)
  list(APPEND HEADER_EXCLUDE
    "wolfssl/wolfcrypt/selftest.h")
endif()

if(NOT BUILD_FIPS OR BUILD_FIPS_V1)
  list(APPEND HEADER_EXCLUDE
    "wolfssl/wolfcrypt/fips.h")
endif()

if(NOT BUILD_QNXCAAM OR BUILD_CAAM)
  list(APPEND HEADER_EXCLUDE
    "wolfssl/wolfcrypt/port/caam"
    )
endif()

list(JOIN HEADER_EXCLUDE "|" EXCLUDED_HEADERS_REGEX)

string(PREPEND EXCLUDED_HEADERS_REGEX "(")
string(APPEND  EXCLUDED_HEADERS_REGEX ")")

set(INSTALLED_EXAMPLES
    ${CMAKE_CURRENT_SOURCE_DIR}/examples/echoserver/echoserver.c
    ${CMAKE_CURRENT_SOURCE_DIR}/examples/sctp/sctp-server.c
    ${CMAKE_CURRENT_SOURCE_DIR}/examples/sctp/sctp-client-dtls.c
    ${CMAKE_CURRENT_SOURCE_DIR}/examples/sctp/sctp-client.c
    ${CMAKE_CURRENT_SOURCE_DIR}/examples/sctp/sctp-server-dtls.c
    ${CMAKE_CURRENT_SOURCE_DIR}/examples/echoclient/echoclient.c
    ${CMAKE_CURRENT_SOURCE_DIR}/examples/server/server.c
    ${CMAKE_CURRENT_SOURCE_DIR}/examples/benchmark/tls_bench.c
    ${CMAKE_CURRENT_SOURCE_DIR}/examples/client/client.c)

# Install the library
install(TARGETS wolfssl
        EXPORT wolfssl-targets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        )
# Install the headers
install(DIRECTORY ${WOLFSSL_OUTPUT_BASE}/wolfssl/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/wolfssl
        FILES_MATCHING PATTERN "*.h"
        REGEX ${EXCLUDED_HEADERS_REGEX} EXCLUDE)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/wolfssl/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/wolfssl
        FILES_MATCHING PATTERN "*.h"
        REGEX ${EXCLUDED_HEADERS_REGEX} EXCLUDE)

# Install the examples
install(FILES ${INSTALLED_EXAMPLES}
        DESTINATION ${CMAKE_INSTALL_DOCDIR}/example)
# Install README.txt and taoCert.txt
install(FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/doc/README.txt
        ${CMAKE_CURRENT_SOURCE_DIR}/certs/taoCert.txt
        DESTINATION ${CMAKE_INSTALL_DOCDIR})
# Install the export set
install(EXPORT wolfssl-targets
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/wolfssl
        FILE wolfssl-targets.cmake
        NAMESPACE wolfssl::)

# TODO: Distro build + rules for what to include in the distro.
#       See various include.am files.

set(prefix ${CMAKE_INSTALL_PREFIX})
set(exec_prefix "\${prefix}")
set(libdir "\${exec_prefix}/lib")
set(includedir "\${prefix}/include")
set(VERSION ${PROJECT_VERSION})

configure_file(support/wolfssl.pc.in ${CMAKE_CURRENT_BINARY_DIR}/support/wolfssl.pc @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/support/wolfssl.pc
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)

include(CMakePackageConfigHelpers)
configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Config.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/wolfssl-config.cmake"
  INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/wolfssl"
  NO_SET_AND_CHECK_MACRO
  NO_CHECK_REQUIRED_COMPONENTS_MACRO
)

export(EXPORT wolfssl-targets
  FILE "${CMAKE_CURRENT_BINARY_DIR}/wolfssl-targets.cmake"
  NAMESPACE wolfssl::
)

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/wolfssl-config-version.cmake"
  VERSION "${wolfssl_VERSION_MAJOR}.${wolfssl_VERSION_MINOR}"
  COMPATIBILITY AnyNewerVersion
)

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/wolfssl-config.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/wolfssl-config-version.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/wolfssl
)
