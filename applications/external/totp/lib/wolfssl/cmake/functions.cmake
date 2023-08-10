function(override_cache VAR VAL)
    get_property(VAR_STRINGS CACHE ${VAR} PROPERTY STRINGS)
    LIST(FIND VAR_STRINGS ${VAL} CK)
    if(-1 EQUAL ${CK})
        message(SEND_ERROR
            "\"${VAL}\" is not valid override value for \"${VAR}\"."
            " Please select value from \"${VAR_STRINGS}\"\n")
    endif()
    set_property(CACHE ${VAR} PROPERTY VALUE ${VAL})
endfunction()

function(add_option NAME HELP_STRING DEFAULT VALUES)
    # Set the default value for the option.
    set(${NAME} ${DEFAULT} CACHE STRING ${HELP_STRING})
    # Set the list of allowed values for the option.
    set_property(CACHE ${NAME} PROPERTY STRINGS ${VALUES})

    if(DEFINED ${NAME})
        list(FIND VALUES ${${NAME}} IDX)
        #
        # If the given value isn't in the list of allowed values for the option,
        # reduce it to yes/no according to CMake's "if" logic:
        # https://cmake.org/cmake/help/latest/command/if.html#basic-expressions
        #
        # This has no functional impact; it just makes the settings in
        # CMakeCache.txt and cmake-gui easier to read.
        #
        if (${IDX} EQUAL -1)
            if(${${NAME}})
                override_cache(${NAME} "yes")
            else()
                override_cache(${NAME} "no")
            endif()
        endif()
    endif()
endfunction()

function(generate_build_flags)
    set(BUILD_DISTRO ${WOLFSSL_DISTRO} PARENT_SCOPE)
    set(BUILD_ALL ${WOLFSSL_ALL} PARENT_SCOPE)
    if(WOLFSSL_TLS13 OR WOLFSSL_USER_SETTINGS)
        set(BUILD_TLS13 "yes" PARENT_SCOPE)
    endif()
    if(WOLFSSL_DTLS13 OR WOLFSSL_USER_SETTINGS)
        set(BUILD_DTLS13 "yes" PARENT_SCOPE)
    endif()
    if(WOLFSSL_QUIC)
        set(BUILD_QUIC "yes" PARENT_SCOPE)
    endif()
    if(WOLFSSL_RNG OR WOLFSSL_USER_SETTINGS)
        set(BUILD_RNG "yes" PARENT_SCOPE)
    endif()
    if(WOLFSSL_SCTP OR WOLFSSL_USER_SETTINGS)
        set(BUILD_SCTP "yes" PARENT_SCOPE)
    endif()
    if(WOLFSSL_DTLS_CID OR WOLFSSL_USER_SETTINGS)
        set(BUILD_DTLS_COMMON "yes" PARENT_SCOPE)
    endif()
    set(BUILD_MCAST ${WOLFSSL_MCAST} PARENT_SCOPE)
    set(BUILD_IPV6 ${WOLFSSL_IPV6} PARENT_SCOPE)
    set(BUILD_LEAN_PSK ${WOLFSSL_LEAN_PSK} PARENT_SCOPE)
    set(BUILD_LEAN_TLS ${WOLFSSL_LEAN_TLS} PARENT_SCOPE)
    set(BUILD_LOWMEM ${WOLFSSL_LOWRESOURCE} PARENT_SCOPE)
    set(BUILD_PKCALLBACKS ${WOLFSSL_PKCALLBACKS} PARENT_SCOPE)
    set(BUILD_CRYPTOAUTHLIB ${WOLFSSL_CRYPTOAUTHLIB} PARENT_SCOPE)
    if(WOLFSSL_SNIFFER OR WOLFSSL_USER_SETTINGS)
        set(BUILD_SNIFFER "yes" PARENT_SCOPE)
    endif()
    set(BUILD_SNIFFTEST ${WOLFSSL_SNIFFTEST} PARENT_SCOPE)
    if(WOLFSSL_AESGCM OR WOLFSSL_USER_SETTINGS)
        set(BUILD_AESGCM "yes" PARENT_SCOPE)
    endif()
    if(WOLFSSL_AESCCM OR WOLFSSL_USER_SETTINGS)
        set(BUILD_AESCCM "yes" PARENT_SCOPE)
    endif()
    set(BUILD_ARM_ASM ${WOLFSSL_ARM_ASM} PARENT_SCOPE)
    set(BUILD_XILINX ${WOLFSSL_XILINX} PARENT_SCOPE)
    set(BUILD_AESNI ${WOLFSSL_AESNI} PARENT_SCOPE)
    set(BUILD_INTEL_ASM ${WOLFSSL_INTEL_ASM} PARENT_SCOPE)
    set(BUILD_AFALG ${WOLFSSL_AFALG} PARENT_SCOPE)
    set(BUILD_DEVCRYPTO ${WOLFSSL_DEVCRYPTO} PARENT_SCOPE)
    if(WOLFSSL_CAMELLIA OR WOLFSSL_USER_SETTINGS)
        set(BUILD_CAMELLIA "yes" PARENT_SCOPE)
    endif()
    if(WOLFSSL_MD2 OR WOLFSSL_USER_SETTINGS)
        set(BUILD_MD2 "yes" PARENT_SCOPE)
    endif()
    if(WOLFSSL_RIPEMD OR WOLFSSL_USER_SETTINGS)
        set(BUILD_RIPEMD "yes" PARENT_SCOPE)
    endif()
    if(WOLFSSL_BLAKE2 OR WOLFSSL_USER_SETTINGS)
        set(BUILD_BLAKE2 "yes" PARENT_SCOPE)
    endif()
    if(WOLFSSL_BLAKE2S OR WOLFSSL_USER_SETTINGS)
        set(BUILD_BLAKE2S "yes" PARENT_SCOPE)
    endif()
    if(WOLFSSL_SHA512 OR WOLFSSL_SHA384 OR WOLFSSL_USER_SETTINGS)
        set(BUILD_SHA512 "yes" PARENT_SCOPE)
    endif()
    if(WOLFSSL_DSA OR WOLFSSL_USER_SETTINGS)
        set(BUILD_DSA "yes" PARENT_SCOPE)
    endif()
    if(WOLFSSL_ECC OR WOLFSSL_USER_SETTINGS)
        set(BUILD_ECC "yes" PARENT_SCOPE)
    endif()
    if(WOLFSSL_ED25519 OR WOLFSSL_USER_SETTINGS)
        set(BUILD_ED25519 "yes" PARENT_SCOPE)
    endif()
    set(BUILD_ED25519_SMALL ${WOLFSSL_ED25519_SMALL} PARENT_SCOPE)
    if(WOLFSSL_FEMATH OR WOLFSSL_USER_SETTINGS)
        set(BUILD_FEMATH "yes" PARENT_SCOPE)
    endif()
    if(WOLFSSL_GEMATH OR WOLFSSL_USER_SETTINGS)
        set(BUILD_GEMATH "yes" PARENT_SCOPE)
    endif()
    if(WOLFSSL_CURVE25519 OR WOLFSSL_USER_SETTINGS)
        set(BUILD_CURVE25519 "yes" PARENT_SCOPE)
    endif()
    set(BUILD_CURVE25519_SMALL ${WOLFSSL_CURVE25519_SMALL} PARENT_SCOPE)
    if(WOLFSSL_ED448 OR WOLFSSL_USER_SETTINGS)
        set(BUILD_ED448 "yes" PARENT_SCOPE)
    endif()
    set(BUILD_ED448_SMALL ${WOLFSSL_ED448_SMALL} PARENT_SCOPE)
    if(WOLFSSL_FE448 OR WOLFSSL_USER_SETTINGS)
        set(BUILD_FE448 "yes" PARENT_SCOPE)
    endif()
    if(WOLFSSL_GE448 OR WOLFSSL_USER_SETTINGS)
        set(BUILD_GE448 "yes" PARENT_SCOPE)
    endif()
    if(WOLFSSL_CURVE448 OR WOLFSSL_USER_SETTINGS)
        set(BUILD_CURVE448 "yes" PARENT_SCOPE)
    endif()
    if(WOLFSSL_CURVE448_SMALL OR WOLFSSL_USER_SETTINGS)
        set(BUILD_CURVE448_SMALL "yes" PARENT_SCOPE)
    endif()
    if(WOLFSSL_MEMORY OR WOLFSSL_USER_SETTINGS)
        set(BUILD_MEMORY "yes" PARENT_SCOPE)
    endif()
    if(WOLFSSL_RSA OR WOLFSSL_USER_SETTINGS)
        set(BUILD_RSA "yes" PARENT_SCOPE)
    endif()
    if(WOLFSSL_DH OR WOLFSSL_USER_SETTINGS)
        set(BUILD_DH "yes" PARENT_SCOPE)
    endif()
    if(WOLFSSL_ASN OR WOLFSSL_USER_SETTINGS)
        set(BUILD_ASN "yes" PARENT_SCOPE)
    endif()
    if(WOLFSSL_AES OR WOLFSSL_USER_SETTINGS)
        set(BUILD_AES "yes" PARENT_SCOPE)
    endif()
    if(WOLFSSL_CODING OR WOLFSSL_USER_SETTINGS)
        set(BUILD_CODING "yes" PARENT_SCOPE)
    endif()
    if(WOLFSSL_ARC4 OR WOLFSSL_USER_SETTINGS)
        set(BUILD_RC4 "yes" PARENT_SCOPE)
    endif()
    if(WOLFSSL_MD5 OR WOLFSSL_USER_SETTINGS)
        set(BUILD_MD5 "yes" PARENT_SCOPE)
    endif()
    if(WOLFSSL_SHA OR WOLFSSL_USER_SETTINGS)
        set(BUILD_SHA "yes" PARENT_SCOPE)
    endif()
    set(BUILD_FIPS ${WOLFSSL_FIPS} PARENT_SCOPE)
    if("${FIPS_VERSION}" STREQUAL "v1")
        set(BUILD_FIPS_V1 "yes" PARENT_SCOPE)
    endif()
    if("${FIPS_VERSION}" STREQUAL "v2")
        set(BUILD_FIPS_V2 "yes" PARENT_SCOPE)
    endif()
    if("${FIPS_VERSION}" STREQUAL "rand")
        set(BUILD_FIPS_RAND "yes" PARENT_SCOPE)
    endif()
    if("${FIPS_VERSION}" STREQUAL "v5")
        set(BUILD_FIPS_V5 "yes" PARENT_SCOPE)
    endif()
    set(BUILD_FIPS_READY ${FIPS_READY} PARENT_SCOPE)
    if(WOLFSSL_CMAC OR WOLFSSL_USER_SETTINGS)
        set(BUILD_CMAC "yes" PARENT_SCOPE)
    endif()
    set(BUILD_SELFTEST ${WOLFSSL_SELFTEST} PARENT_SCOPE)
    if(WOLFSSL_SHA224 OR WOLFSSL_USER_SETTINGS)
        set(BUILD_SHA224 "yes" PARENT_SCOPE)
    endif()
    if(WOLFSSL_SHA3 OR WOLFSSL_USER_SETTINGS)
        set(BUILD_SHA3 "yes" PARENT_SCOPE)
    endif()
    if(WOLFSSL_POLY1305 OR WOLFSSL_USER_SETTINGS)
        set(BUILD_POLY1305 "yes" PARENT_SCOPE)
    endif()
    if(WOLFSSL_CHACHA OR WOLFSSL_USER_SETTINGS)
        set(BUILD_CHACHA "yes" PARENT_SCOPE)
    endif()
    if(WOLFSSL_XCHACHA OR WOLFSSL_USER_SETTINGS)
        set(BUILD_XCHACHA "yes" PARENT_SCOPE)
    endif()
    if(WOLFSSL_OQS OR WOLFSSL_USER_SETTINGS)
        set(BUILD_FALCON "yes" PARENT_SCOPE)
        set(BUILD_SPHINCS "yes" PARENT_SCOPE)
        set(BUILD_DILITHIUM "yes" PARENT_SCOPE)
        set(BUILD_EXT_KYBER "yes" PARENT_SCOPE)
    endif()
    set(BUILD_INLINE ${WOLFSSL_INLINE} PARENT_SCOPE)
    if(WOLFSSL_OCSP OR WOLFSSL_USER_SETTINGS)
        set(BUILD_OCSP "yes" PARENT_SCOPE)
    endif()
    set(BUILD_OCSP_STAPLING ${WOLFSSL_OCSPSTAPLING} PARENT_SCOPE)
    set(BUILD_OCSP_STAPLING_V2 ${WOLFSSL_OCSPSTAPLING_V2} PARENT_SCOPE)
    if(WOLFSSL_CRL OR WOLFSSL_USER_SETTINGS)
        set(BUILD_CRL "yes" PARENT_SCOPE)
    endif()
    if(WOLFSSL_CRL_MONITOR)
        set(BUILD_CRL_MONITOR "yes" PARENT_SCOPE)
    endif()
    set(BUILD_USER_RSA ${WOLFSSL_USER_RSA} PARENT_SCOPE)
    set(BUILD_USER_CRYPTO ${WOLFSSL_USER_CRYPTO} PARENT_SCOPE)
    set(BUILD_WNR ${WOLFSSL_WNR} PARENT_SCOPE)
    if(WOLFSSL_SRP OR WOLFSSL_USER_SETTINGS)
        set(BUILD_SRP "yes" PARENT_SCOPE)
    endif()
    set(USE_VALGRIND ${WOLFSSL_VALGRIND})
    if(WOLFSSL_MD4 OR WOLFSSL_USER_SETTINGS)
        set(BUILD_MD4 "yes" PARENT_SCOPE)
    endif()
    if(WOLFSSL_PWDBASED OR WOLFSSL_USER_SETTINGS)
        set(BUILD_PWDBASED "yes" PARENT_SCOPE)
    endif()
    if(WOLFSSL_SCRYPT OR WOLFSSL_USER_SETTINGS)
        set(BUILD_SCRYPT "yes" PARENT_SCOPE)
    endif()
    if(WOLFSSL_CRYPT_ONLY AND NOT WOLFSSL_OPENSSL_EXTRA)
        set(BUILD_CRYPTONLY "yes" PARENT_SCOPE)
    endif()
    if(WOLFSSL_FAST_MATH OR WOLFSSL_USER_SETTINGS)
        set(BUILD_FAST_MATH "yes" PARENT_SCOPE)
    endif()
    if(WOLFSSL_SLOW_MATH OR WOLFSSL_USER_SETTINGS)
        set(BUILD_SLOW_MATH "yes" PARENT_SCOPE)
    endif()
    if(WOLFSSL_EXAMPLES AND NOT WOLFSSL_LEAN_TLS)
        set(BUILD_EXAMPLE_SERVERS "yes" PARENT_SCOPE)
    endif()
    set(BUILD_EXAMPLE_CLIENTS ${WOLFSSL_EXAMPLES} PARENT_SCOPE)
    set(BUILD_TESTS ${WOLFSSL_EXAMPLES} PARENT_SCOPE)
    if(NOT WOLFSSL_SINGLETHREADED AND WOLFSSL_EXAMPLES AND NOT WOLFSSL_LEAN_TLS)
        set(BUILD_THREADED_EXAMPLES "yes" PARENT_SCOPE)
    endif()
    set(BUILD_WOLFCRYPT_TESTS ${WOLFSSL_CRYPT_TESTS} PARENT_SCOPE)
    set(BUILD_LIBZ ${WOLFSSL_LIBZ} PARENT_SCOPE)
    if(WOLFSSL_PKCS11 OR WOLFSSL_USER_SETTINGS)
        set(BUILD_PKCS11 "yes" PARENT_SCOPE)
    endif()
    if(WOLFSSL_PKCS12 OR WOLFSSL_USER_SETTINGS)
        set(BUILD_PKCS12 "yes" PARENT_SCOPE)
    endif()
    set(BUILD_CAVIUM ${WOLFSSL_CAVIUM} PARENT_SCOPE)
    set(BUILD_CAVIUM_V ${WOLFSSL_CAVIUM_V} PARENT_SCOPE)
    set(BUILD_OCTEON_SYNC ${WOLFSSL_OCTEON_SYNC} PARENT_SCOPE)
    set(BUILD_INTEL_QA ${WOLFSSL_INTEL_QA} PARENT_SCOPE)
    set(BUILD_INTEL_QA_SYNC ${WOLFSSL_INTEL_QA_SYNC} PARENT_SCOPE)
    if(WOLFSSL_SP OR WOLFSSL_USER_SETTINGS)
        set(BUILD_SP "yes" PARENT_SCOPE)
    endif()
    if((WOLFSSL_SP AND NOT WOLFSSL_SP_ASM) OR WOLFSSL_USER_SETTINGS)
        set(BUILD_SP_C "yes" PARENT_SCOPE)
    endif()
    if(WOLFSSL_SP_ARM64_ASM OR WOLFSSL_USER_SETTINGS)
        set(BUILD_SP_ARM64 "yes" PARENT_SCOPE)
    endif()
    if(WOLFSSL_SP_ARM32_ASM OR WOLFSSL_USER_SETTINGS)
        set(BUILD_SP_ARM32 "yes" PARENT_SCOPE)
    endif()
    if(WOLFSSL_SP_ARM_THUMB_ASM OR WOLFSSL_USER_SETTINGS)
        set(BUILD_SP_ARM_THUMB "yes" PARENT_SCOPE)
    endif()
    if(WOLFSSL_SP_ARM_CORTEX_ASM OR WOLFSSL_USER_SETTINGS)
        set(BUILD_SP_ARM_CORTEX "yes" PARENT_SCOPE)
    endif()
    if(WOLFSSL_SP_X86_64_ASM OR WOLFSSL_USER_SETTINGS)
        set(BUILD_SP_X86_64 "yes" PARENT_SCOPE)
    endif()
    if(WOLFSSL_SP_MATH OR WOLFSSL_SP_MATH_ALL OR WOLFSSL_USER_SETTINGS)
        set(BUILD_SP_INT "yes" PARENT_SCOPE)
    endif()
    set(BUILD_FAST_RSA ${WOLFSSL_FAST_RSA} PARENT_SCOPE)
    set(BUILD_MCAPI ${WOLFSSL_MCAPI} PARENT_SCOPE)
    set(BUILD_ASYNCCRYPT ${WOLFSSL_ASYNCCRYPT} PARENT_SCOPE)
    set(BUILD_WOLFEVENT ${WOLFSSL_ASYNCCRYPT} PARENT_SCOPE)
    if(WOLFSSL_CRYPTOCB OR WOLFSSL_USER_SETTINGS)
        set(BUILD_CRYPTOCB "yes" PARENT_SCOPE)
    endif()
    set(BUILD_PSK ${WOLFSSL_PSK} PARENT_SCOPE)
    set(BUILD_TRUST_PEER_CERT ${WOLFSSL_TRUSTED_PEER_CERT} PARENT_SCOPE)
    set(BUILD_PKI ${WOLFSSL_PKI} PARENT_SCOPE)
    if(WOLFSSL_DES3 OR WOLFSSL_USER_SETTINGS)
        set(BUILD_DES3 "yes" PARENT_SCOPE)
    endif()
    if(WOLFSSL_PKCS7 OR WOLFSSL_USER_SETTINGS)
        set(BUILD_PKCS7 "yes" PARENT_SCOPE)
    endif()
    set(BUILD_HASHFLAGS ${WOLFSSL_HASHFLAGS} PARENT_SCOPE)
    set(BUILD_LINUX_KM ${WOLFSSL_LINUX_KM} PARENT_SCOPE)
    set(BUILD_NO_LIBRARY ${WOLFSSL_NO_LIBRARY} PARENT_SCOPE)
    if(WOLFSSL_DEBUG OR WOLFSSL_STACK_SIZE)
        set(BUILD_DEBUG "yes" PARENT_SCOPE)
    endif()
    set(BUILD_RC2 ${WOLFSSL_RC2} PARENT_SCOPE)
    if(WOLFSSL_CAAM)
        set(BUILD_CAAM "yes" PARENT_SCOPE)
    endif()
    if(WOLFSSL_HPKE OR WOLFSSL_USER_SETTINGS)
        set(BUILD_HPKE "yes" PARENT_SCOPE)
    endif()

    set(BUILD_FLAGS_GENERATED "yes" PARENT_SCOPE)
endfunction()

function(generate_lib_src_list LIB_SOURCES)
    if(NOT BUILD_FLAGS_GENERATED)
        message(FATAL_ERROR "Cannot call generate_lib_src_list without calling generate_build_flags first.")
    endif()

    # Corresponds to src/include.am
    if(BUILD_FIPS)
         if(BUILD_FIPS_V2)
              # FIPSv2 first file
              list(APPEND LIB_SOURCES wolfcrypt/src/wolfcrypt_first.c)

              list(APPEND LIB_SOURCES
                wolfcrypt/src/hmac.c
                wolfcrypt/src/random.c
                wolfcrypt/src/sha256.c)

              if(BUILD_RSA)
                   list(APPEND LIB_SOURCES wolfcrypt/src/rsa.c)
              endif()

              if(BUILD_ECC)
                   list(APPEND LIB_SOURCES wolfcrypt/src/ecc.c)
              endif()

              if(BUILD_AES)
                   list(APPEND LIB_SOURCES wolfcrypt/src/aes.c)

                   if(BUILD_ARMASM AND BUILD_FIPS_READY)
                        list(APPEND LIB_SOURCES wolfcrypt/src/port/arm/armv8-aes.c)
                   endif()
              endif()

              if(BUILD_AESNI)
                   list(APPEND LIB_SOURCES wolfcrypt/src/aes_asm.S)

                   if(BUILD_INTELASM)
                        list(APPEND LIB_SOURCES wolfcrypt/src/aes_gcm_asm.S)
                   endif()
              endif()

              if(BUILD_DES3)
                   list(APPEND LIB_SOURCES wolfcrypt/src/des3.c)
              endif()

              if(BUILD_SHA)
                   list(APPEND LIB_SOURCES wolfcrypt/src/sha.c)
              endif()

              if(BUILD_ARMASM AND BUILD_FIPS_READY)
                   list(APPEND LIB_SOURCES wolfcrypt/src/port/arm/armv8-sha256.c)
              endif()

              if(BUILD_INTELASM)
                   list(APPEND LIB_SOURCES wolfcrypt/src/sha256_asm.S)
              endif()

              if(BUILD_SHA512)
                   list(APPEND LIB_SOURCES wolfcrypt/src/sha512.c)

                   if(BUILD_ARMASM AND BUILD_FIPS_READY)
                        list(APPEND LIB_SOURCES
                             wolfcrypt/src/port/arm/armv8-sha512.c
                             wolfcrypt/src/port/arm/armv8-sha512-asm.S
                             wolfcrypt/src/port/arm/armv8-32-sha512-asm.S)
                   endif()

                   if(BUILD_INTELASM)
                        list(APPEND LIB_SOURCES wolfcrypt/src/sha512_asm.S)
                   endif()
              endif()

              if(BUILD_SHA3)
                   list(APPEND LIB_SOURCES wolfcrypt/src/sha3.c)
              endif()

              if(BUILD_DH)
                   list(APPEND LIB_SOURCES wolfcrypt/src/dh.c)
              endif()

              if(BUILD_CMAC)
                   list(APPEND LIB_SOURCES wolfcrypt/src/cmac.c)
              endif()

              list(APPEND LIB_SOURCES
                   wolfcrypt/src/fips.c
                   wolfcrypt/src/fips_test.c)

              # fips last file
              list(APPEND LIB_SOURCES wolfcrypt/src/wolfcrypt_last.c)
         endif()

         if(BUILD_FIPS_V5)
              list(APPEND LIB_SOURCES wolfcrypt/src/wolfcrypt_first.c)

              list(APPEND LIB_SOURCES
                wolfcrypt/src/hmac.c
                wolfcrypt/src/random.c
                wolfcrypt/src/sha256.c)

              list(APPEND LIB_SOURCES
                wolfcrypt/src/kdf.c)

              if(BUILD_RSA)
                   list(APPEND LIB_SOURCES wolfcrypt/src/rsa.c)
              endif()
         endif()

         if(BUILD_FIPS_RAND)
              list(APPEND LIB_SOURCES
                   wolfcrypt/src/wolfcrypt_first.c
                   wolfcrypt/src/hmac.c
                   wolfcrypt/src/random.c
                   wolfcrypt/src/sha256.c
                   wolfcrypt/src/sha256_asm.S
                   wolfcrypt/src/fips.c
                   wolfcrypt/src/fips_test.c
                   wolfcrypt/src/wolfcrypt_last.c)
         endif()
    endif()

    # For wolfRand, exclude everything else.
    if(NOT BUILD_FIPS_RAND)
         # For FIPSV2, exclude the wolfCrypt files included above.
         # For wolfRand, exclude just a couple files.
         # For old FIPS, keep the wolfCrypt versions of the
         # CtaoCrypt files included above.
         if(NOT BUILD_FIPS_V2)
              list(APPEND LIB_SOURCES wolfcrypt/src/hmac.c)
         endif()

         # CAVP self test
         if(BUILD_SELFTEST)
              list(APPEND LIB_SOURCES wolfcrypt/src/selftest.c)
         endif()
    endif()

    list(APPEND LIB_SOURCES
         wolfcrypt/src/hash.c
         wolfcrypt/src/cpuid.c)

    if(NOT BUILD_FIPS_RAND)
         if(NOT BUILD_FIPS_V5)
              list(APPEND LIB_SOURCES wolfcrypt/src/kdf.c)
         endif()

         if(NOT BUILD_FIPS_V2 AND BUILD_RNG)
              list(APPEND LIB_SOURCES wolfcrypt/src/random.c)
         endif()

         if(NOT BUILD_FIPS_V2)
              if(BUILD_ARMASM)
                   list(APPEND LIB_SOURCES wolfcrypt/src/port/arm/armv8-sha256.c)
              else()
                   list(APPEND LIB_SOURCES wolfcrypt/src/sha256.c)

                   if(BUILD_INTELASM)
                        list(APPEND LIB_SOURCES wolfcrypt/src/sha256_asm.S)
                   endif()
              endif()
         endif()

         if(BUILD_AFALG)
              list(APPEND LIB_SOURCES wolfcrypt/src/port/af_alg/afalg_hash.c)
         endif()

         if(BUILD_WOLFEVENT)
              list(APPEND LIB_SOURCES wolfcrypt/src/wolfevent.c)
         endif()

         if(BUILD_ASYNCCRYPT)
              list(APPEND LIB_SOURCES wolfcrypt/src/async.c)
         endif()

         if(NOT BUILD_USER_RSA AND BUILD_RSA)
              if(BUILD_FAST_RSA)
                   list(APPEND LIB_SOURCES wolfcrypt/user-crypto/src/rsa.c)
              else()
                   if(NOT BUILD_FIPS_V2)
                        list(APPEND LIB_SOURCES wolfcrypt/src/rsa.c)
                   endif()
              endif()
         endif()

         if(BUILD_SP)
              if(BUILD_SP_C)
                   list(APPEND LIB_SOURCES
                        wolfcrypt/src/sp_c32.c
                        wolfcrypt/src/sp_c64.c)
              endif()

              if(BUILD_SP_X86_64)
                   list(APPEND LIB_SOURCES
                        wolfcrypt/src/sp_x86_64.c
                        wolfcrypt/src/sp_x86_64_asm.S)
              endif()

              if(NOT BUILD_FIPS_V2 AND BUILD_SP_ARM32)
                   list(APPEND LIB_SOURCES wolfcrypt/src/sp_arm32.c)
              endif()

              if(BUILD_SP_ARM_THUMB)
                   list(APPEND LIB_SOURCES wolfcrypt/src/sp_armthumb.c)
              endif()

              if(BUILD_SP_ARM64)
                   list(APPEND LIB_SOURCES wolfcrypt/src/sp_arm64.c)
              endif()

              if(BUILD_SP_ARM_CORTEX)
                   list(APPEND LIB_SOURCES wolfcrypt/src/sp_cortexm.c)
              endif()
         endif()
         if(BUILD_SP_INT)
              list(APPEND LIB_SOURCES wolfcrypt/src/sp_int.c)
         endif()

         if(NOT BUILD_FIPS_V2)
              if(BUILD_AES)
                   list(APPEND LIB_SOURCES wolfcrypt/src/aes.c)

                   if(BUILD_ARMASM)
                        list(APPEND LIB_SOURCES wolfcrypt/src/port/arm/armv8-aes.c)
                   endif()

                   if(BUILD_AFALG)
                        list(APPEND LIB_SOURCES wolfcrypt/src/port/af_alg/afalg_aes.c)
                   endif()
              endif()
         endif()

         if(NOT BUILD_FIPS_V2 AND BUILD_CMAC)
              list(APPEND LIB_SOURCES wolfcrypt/src/cmac.c)
         endif()

         if(NOT BUILD_FIPS_V2 AND BUILD_DES3)
              list(APPEND LIB_SOURCES wolfcrypt/src/des3.c)
         endif()

         if(NOT BUILD_FIPS_V2 AND BUILD_SHA)
              list(APPEND LIB_SOURCES wolfcrypt/src/sha.c)
         endif()

         if(NOT BUILD_FIPS_V2 AND BUILD_SHA512)
              if(BUILD_ARMASM)
                   list(APPEND LIB_SOURCES
                        wolfcrypt/src/port/arm/armv8-sha512.c
                        wolfcrypt/src/port/arm/armv8-sha512-asm.S
                        wolfcrypt/src/port/arm/armv8-32-sha512-asm.S)
              else()
                   list(APPEND LIB_SOURCES wolfcrypt/src/sha512.c)

                   if(BUILD_INTELASM)
                        list(APPEND LIB_SOURCES wolfcrypt/src/sha512_asm.S)
                   endif()
              endif()
         endif()

         if(NOT BUILD_FIPS_V2 AND BUILD_SHA3)
              list(APPEND LIB_SOURCES wolfcrypt/src/sha3.c)
         endif()
    endif()

    list(APPEND LIB_SOURCES
         wolfcrypt/src/logging.c
         wolfcrypt/src/wc_port.c
         wolfcrypt/src/error.c)


    if(NOT BUILD_FIPS_RAND)
         list(APPEND LIB_SOURCES
              wolfcrypt/src/wc_encrypt.c
              wolfcrypt/src/signature.c
              wolfcrypt/src/wolfmath.c)
    endif()

    if(BUILD_MEMORY)
         list(APPEND LIB_SOURCES wolfcrypt/src/memory.c)
    endif()

    if(NOT BUILD_FIPS_RAND)
         if(NOT BUILD_FIPS_V2 AND BUILD_DH)
              list(APPEND LIB_SOURCES wolfcrypt/src/dh.c)
         endif()

         if(BUILD_ASN)
              list(APPEND LIB_SOURCES wolfcrypt/src/asn.c)
         endif()
    endif()

    if(BUILD_CODING)
         list(APPEND LIB_SOURCES wolfcrypt/src/coding.c)
    endif()

    if(NOT BUILD_FIPS_RAND)
         if(BUILD_POLY1305)
              if(BUILD_ARMASM)
                   list(APPEND LIB_SOURCES wolfcrypt/src/port/arm/armv8-poly1305.c)
              endif()

              list(APPEND LIB_SOURCES wolfcrypt/src/poly1305.c)

              if(BUILD_INTELASM)
                   list(APPEND LIB_SOURCES wolfcrypt/src/poly1305_asm.S)
              endif()
         endif()

         if(BUILD_RC4)
              list(APPEND LIB_SOURCES wolfcrypt/src/arc4.c)
         endif()

         if(BUILD_MD4)
              list(APPEND LIB_SOURCES wolfcrypt/src/md4.c)
         endif()

         if(BUILD_MD5)
              list(APPEND LIB_SOURCES wolfcrypt/src/md5.c)
         endif()

         if(BUILD_PWDBASED)
              list(APPEND LIB_SOURCES
                   wolfcrypt/src/pwdbased.c
                   wolfcrypt/src/pkcs12.c)
         endif()

         if(BUILD_DSA)
              list(APPEND LIB_SOURCES wolfcrypt/src/dsa.c)
         endif()

         if(NOT BUILD_FIPS_V2 AND BUILD_AESNI)
              list(APPEND LIB_SOURCES
                   wolfcrypt/src/aes_asm.S
                   wolfcrypt/src/aes_gcm_asm.S)
         endif()

         if(BUILD_CAMELLIA)
              list(APPEND LIB_SOURCES wolfcrypt/src/camellia.c)
         endif()

         if(BUILD_MD2)
              list(APPEND LIB_SOURCES wolfcrypt/src/md2.c)
         endif()

         if(BUILD_RIPEMD)
              list(APPEND LIB_SOURCES wolfcrypt/src/ripemd.c)
         endif()

         if(BUILD_BLAKE2)
              list(APPEND LIB_SOURCES wolfcrypt/src/blake2b.c)
         endif()

         if(BUILD_BLAKE2S)
              list(APPEND LIB_SOURCES wolfcrypt/src/blake2s.c)
         endif()

         if(BUILD_CHACHA)
              if(BUILD_ARMASM)
                   list(APPEND LIB_SOURCES wolfcrypt/src/port/arm/armv8-chacha.c)
              else()
                   list(APPEND LIB_SOURCES wolfcrypt/src/chacha.c)

                   if(BUILD_INTELASM)
                        list(APPEND LIB_SOURCES wolfcrypt/src/chacha_asm.S)
                   endif()
              endif()

              if(BUILD_POLY1305)
                   list(APPEND LIB_SOURCES wolfcrypt/src/chacha20_poly1305.c)
              endif()
         endif()

         if(NOT BUILD_INLINE)
              list(APPEND LIB_SOURCES wolfcrypt/src/misc.c)
         endif()

         if(BUILD_FAST_MATH)
              list(APPEND LIB_SOURCES wolfcrypt/src/tfm.c)
         endif()

         if(BUILD_SLOW_MATH)
              list(APPEND LIB_SOURCES wolfcrypt/src/integer.c)
         endif()

         if(NOT BUILD_FIPS_V2 AND BUILD_ECC)
              list(APPEND LIB_SOURCES wolfcrypt/src/ecc.c)
         endif()

         if(BUILD_CURVE25519)
              list(APPEND LIB_SOURCES wolfcrypt/src/curve25519.c)
         endif()

         if(BUILD_ED25519)
              list(APPEND LIB_SOURCES wolfcrypt/src/ed25519.c)
         endif()

         if(BUILD_FEMATH)
              if(BUILD_CURVE25519_SMALL)
                   list(APPEND LIB_SOURCES wolfcrypt/src/fe_low_mem.c)
              else()
                   if(BUILD_INTELASM)
                        list(APPEND LIB_SOURCES wolfcrypt/src/fe_x25519_asm.S)
                   else()
                        if(BUILD_ARMASM)
                             list(APPEND LIB_SOURCES
                                  wolfcrypt/src/port/arm/armv8-32-curve25519.S
                                  wolfcrypt/src/port/arm/armv8-curve25519.S)
                        else()
                             list(APPEND LIB_SOURCES wolfcrypt/src/fe_operations.c)
                        endif()
                   endif()
              endif()
         endif()

         if(BUILD_GEMATH)
              if(BUILD_ED25519_SMALL)
                   list(APPEND LIB_SOURCES wolfcrypt/src/ge_low_mem.c)
              else()
                   list(APPEND LIB_SOURCES wolfcrypt/src/ge_operations.c)

                   if(NOT BUILD_FEMATH)
                        if(BUILD_INTELASM)
                             list(APPEND LIB_SOURCES wolfcrypt/src/fe_x25519_asm.S)
                        else()
                             if(BUILD_ARMASM)
                                  list(APPEND LIB_SOURCES wolfcrypt/src/port/arm/armv8-curve25519.S)
                             else()
                                  list(APPEND LIB_SOURCES wolfcrypt/src/fe_operations.c)
                             endif()
                        endif()
                   endif()
              endif()
         endif()

         if(BUILD_CURVE448)
              list(APPEND LIB_SOURCES wolfcrypt/src/curve448.c)
         endif()

         if(BUILD_ED448)
              list(APPEND LIB_SOURCES wolfcrypt/src/ed448.c)
         endif()

         if(BUILD_FE448)
              list(APPEND LIB_SOURCES wolfcrypt/src/fe_448.c)
         endif()

         if(BUILD_GE448)
              list(APPEND LIB_SOURCES wolfcrypt/src/ge_448.c)

              if(NOT BUILD_FE448)
                   list(APPEND LIB_SOURCES wolfcrypt/src/fe_448.c)
              endif()
         endif()

         if(BUILD_FALCON)
              list(APPEND LIB_SOURCES wolfcrypt/src/falcon.c)
         endif()

         if(BUILD_SPHINCS)
              list(APPEND LIB_SOURCES wolfcrypt/src/sphincs.c)
         endif()

         if(BUILD_DILITHIUM)
              list(APPEND LIB_SOURCES wolfcrypt/src/dilithium.c)
         endif()

         if(BUILD_EXT_KYBER)
              list(APPEND LIB_SOURCES wolfcrypt/src/ext_kyber.c)
         endif()

         if(BUILD_LIBZ)
              list(APPEND LIB_SOURCES wolfcrypt/src/compress.c)
         endif()

         if(BUILD_PKCS7)
              list(APPEND LIB_SOURCES wolfcrypt/src/pkcs7.c)
         endif()

         if(BUILD_SRP)
              list(APPEND LIB_SOURCES wolfcrypt/src/srp.c)
         endif()

         if(BUILD_AFALG)
              list(APPEND LIB_SOURCES wolfcrypt/src/port/af_alg/wc_afalg.c)
         endif()

         if(NOT BUILD_CRYPTONLY)
              # ssl files
              list(APPEND LIB_SOURCES
                   src/internal.c
                   src/wolfio.c
                   src/keys.c
                   src/ssl.c
                   src/ocsp.c
                   src/tls.c)

              if(BUILD_TLS13)
                   list(APPEND LIB_SOURCES src/tls13.c)
              endif()

              if(BUILD_DTLS13)
                   list(APPEND LIB_SOURCES src/dtls13.c)
              endif()

              if(BUILD_QUIC)
                   list(APPEND LIB_SOURCES src/quic.c)
              endif()

              if(BUILD_OCSP)
                   list(APPEND LIB_SOURCES src/ocsp.c)
              endif()

              if(BUILD_CRL)
                   list(APPEND LIB_SOURCES src/crl.c)
              endif()

              if(BUILD_SNIFFER)
                   list(APPEND LIB_SOURCES src/sniffer.c)
              endif()

              if(BUILD_DTLS_COMMON)
                   list(APPEND LIB_SOURCES src/dtls.c)
              endif()
         endif()
    endif()

    # Corresponds to wolfcrypt/src/include.am
    if(BUILD_CRYPTOCB)
           list(APPEND LIB_SOURCES wolfcrypt/src/cryptocb.c)
    endif()

    if(BUILD_PKCS11)
           list(APPEND LIB_SOURCES wolfcrypt/src/wc_pkcs11.c)
    endif()

    if(BUILD_DEVCRYPTO)
        list(APPEND LIB_SOURCES
            wolfcrypt/src/port/devcrypto/devcrypto_hash.c
            wolfcrypt/src/port/devcrypto/devcrypto_aes.c
            wolfcrypt/src/port/devcrypto/wc_devcrypto.c)
    endif()

    if(BUILD_CAVIUM)
        list(APPEND LIB_SOURCES wolfcrypt/src/port/cavium/cavium_nitrox.c)
    endif()

    if(BUILD_OCTEON_SYNC)
        list(APPEND LIB_SOURCES wolfcrypt/src/port/cavium/cavium_octeon_sync.c)
    endif()

    if(BUILD_INTEL_QA)
        list(APPEND LIB_SOURCES
            wolfcrypt/src/port/intel/quickassist.c
            wolfcrypt/src/port/intel/quickassist_mem.c)
    endif()

    if(BUILD_INTEL_QA_SYNC)
        list(APPEND LIB_SOURCES wolfcrypt/src/port/intel/quickassist_sync.c)
    endif()

    if(BUILD_CRYPTOAUTHLIB)
        list(APPEND LIB_SOURCES wolfcrypt/src/port/atmel/atmel.c)
    endif()

    if(BUILD_CAAM)
        list(APPEND LIB_SOURCES
            wolfcrypt/src/port/caam/wolfcaam_init.c
            wolfcrypt/src/port/caam/wolfcaam_qnx.c
            wolfcrypt/src/port/caam/wolfcaam_seco.c
            wolfcrypt/src/port/caam/wolfcaam_fsl_nxp.c
            wolfcrypt/src/port/caam/wolfcaam_x25519.c
            wolfcrypt/src/port/caam/wolfcaam_ecdsa.c
            wolfcrypt/src/port/caam/wolfcaam_cmac.c
            wolfcrypt/src/port/caam/wolfcaam_aes.c
            wolfcrypt/src/port/caam/wolfcaam_hash.c
            wolfcrypt/src/port/caam/wolfcaam_rsa.c
            wolfcrypt/src/port/caam/wolfcaam_hmac.c)
    endif()

    if(BUILD_HPKE)
         list(APPEND LIB_SOURCES wolfcrypt/src/hpke.c)
    endif()

    set(LIB_SOURCES ${LIB_SOURCES} PARENT_SCOPE)
endfunction()

function(add_to_options_file DEFINITIONS OPTION_FILE)
    list(REMOVE_DUPLICATES DEFINITIONS)
    foreach(DEF IN LISTS DEFINITIONS)
        if(DEF MATCHES "^-D")
            if(DEF MATCHES "^-D(N)?DEBUG(=.+)?")
                message("not outputting (N)DEBUG to ${OPTION_FILE}")
            endif()

            # allow user to ignore system options
            if(DEF MATCHES "^-D_.*")
                file(APPEND ${OPTION_FILE} "#ifndef WOLFSSL_OPTIONS_IGNORE_SYS\n")
            endif()

            string(REGEX REPLACE "^-D" "" DEF_NO_PREFIX ${DEF})
            string(REGEX REPLACE "=.*$" "" DEF_NO_EQUAL_NO_VAL ${DEF_NO_PREFIX})
            string(REPLACE "=" " " DEF_NO_EQUAL ${DEF_NO_PREFIX})

            file(APPEND ${OPTION_FILE} "#undef  ${DEF_NO_EQUAL_NO_VAL}\n")
            file(APPEND ${OPTION_FILE} "#define ${DEF_NO_EQUAL}\n")

            if(DEF MATCHES "^-D_.*")
                file(APPEND ${OPTION_FILE} "#endif\n")
            endif()

            file(APPEND ${OPTION_FILE} "\n")
        endif()
    endforeach()
endfunction()
