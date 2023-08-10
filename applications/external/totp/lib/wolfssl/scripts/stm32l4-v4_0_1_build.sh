#!/bin/bash
WOLF_ROOT=$(eval "pwd")
echo "WOLF_ROOT set to: \"$WOLF_ROOT\""
cd ../ || exit 5
APP_ROOT=$(eval "pwd")
echo "APP_ROOT set to: \"$APP_ROOT\""
cd ../../ || exit 5
FIRMWARE_ROOT=$(eval "pwd")
echo "FIRMWARE_ROOT set to: \"$FIRMWARE_ROOT\""
cd "$WOLF_ROOT"  || exit 5

WOLFCRYPT_SRC_LIST=(wolfcrypt_first hmac random sha256 rsa ecc aes des3 sha
                    sha512 sha3 dh cmac fips fips_test wolfcrypt_last asn coding
                    dsa error hash logging md5 memory signature tfm wc_encrypt
                    wc_port wolfmath
                   )

assumptions(){
    printf '%s\n' "ASSUMPTIONS:"
    printf '%s\n' "It is assumed that the firmware directory layout is as follows:"
    printf '%s\n' "firmware-root/"
    printf '%s\n' "firmware-root/dir1/"
    printf '%s\n' "firmware-root/dir1/app-root/"
    printf '%s\n' "firmware-root/dir1/app-root/wolfssl-x.x.x-commercial-fips-stm32l4-v2/"
    printf '\n\n%s\n' "It is also assumed this script will be run from the directory:"
    printf '%s\n' "firmware-root/dir1/app-root/wolfssl-x.x.x-commercial-fips-stm32l4-v2/"
    printf '%s\n' "with the command:"
    printf '%s\n' "./scripts/stm32l4-v4_0_2_build.sh"
}

flatten_wolfcrypt_sources(){
    if [ -d "$WOLF_ROOT" ]; then

        for TARGET_FILE in "${WOLFCRYPT_SRC_LIST[@]}"
        do
            if [ -f "$APP_ROOT/$TARGET_FILE".c ]; then
                printf '%s\n' "Removing: APP_ROOT/$TARGET_FILE.c"
                rm "$APP_ROOT/$TARGET_FILE".c
            fi
            printf '%s\n' "WOLF_ROOT/wolfcrypt/src/$TARGET_FILE.c --> APP_ROOT/$TARGET_FILE.c"
            cp "$WOLF_ROOT/wolfcrypt/src/$TARGET_FILE".c "$APP_ROOT/"
        done

        # uncomment to copy over the test app for testing purposes
        #cp "$WOLF_ROOT/wolfcrypt/test/test.c" ./
    else
        printf '%s\n' "Please update the MY_WOLF_RELEASE_VARIABLE to the name"
        printf '%s\n' "of the release you received most recently. Currently"
        printf '%s\n' "it is set to \"$MY_WOLF_RELEASE_NAME\""
    fi
}

update_user_settings(){
    if [ -f user_settings.h ]; then
        printf '%s\n' "Removing old user_settings.h"
        rm user_settings.h
    fi

    printf '%s\n' "Generating new user_settings.h..."
    touch user_settings.h

    printf '%s\n' "#ifndef STM32L4_V_4_0_1_USER_SETTINGS_H" > user_settings.h
    {
        printf '%s\n' "#define STM32L4_V_4_0_1_USER_SETTINGS_H";
        printf '%s\n' "";
        printf '%s\n' "/* FIPS SETTINGS - BEGIN */";
        printf '%s\n' "#define HAVE_FIPS";
        printf '%s\n' "#define HAVE_FIPS_VERSION 2";
        printf '%s\n' "#define NO_THREAD_LS";
        printf '%s\n' "#define NO_STRICT_ECDSA_LEN";
        printf '%s\n' "#define HAVE_ECC";
        printf '%s\n' "#define HAVE_HKDF";
        printf '%s\n' "#define HAVE_AESCCM";
        printf '%s\n' "#define HAVE_AES_ECB";
        printf '%s\n' "#define HAVE_ECC_CDH";
        printf '%s\n' "#define HAVE_FFDHE_Q";
        printf '%s\n' "#define HAVE_FFDHE_2048"; # NEW
        printf '%s\n' "#define HAVE_HASHDRBG";
        printf '%s\n' "#define WOLFSSL_SHA3";
        printf '%s\n' "#define WOLFSSL_CMAC";
        printf '%s\n' "#define WOLFSSL_SHA224";
        printf '%s\n' "#define WOLFSSL_SHA384";
        printf '%s\n' "#define WOLFSSL_SHA512";
        printf '%s\n' "#define WOLFSSL_KEY_GEN";
        printf '%s\n' "#define WOLFSSL_PUBLIC_MP";
        printf '%s\n' "#define WOLFSSL_AES_DIRECT";
        printf '%s\n' "#define WOLFSSL_AES_COUNTER";
        printf '%s\n' "#define WOLFSSL_BASE64_ENCODE";
        printf '%s\n' "#define WOLFSSL_VALIDATE_FFC_IMPORT";
        printf '%s\n' "#define WOLFSSL_VALIDATE_ECC_IMPORT";
        printf '%s\n' "#define WC_RSA_PSS";
        printf '%s\n' "#define WC_RSA_NO_PADDING";
# NEW        printf '%s\n' "#define WC_RSA_BLINDING";
        printf '%s\n' "#define FP_MAX_BITS 8192";
        printf '%s\n' "";
        printf '%s\n' "/* For operational testing use only in validation effort */";
#        printf '%s\n' "/* #define HAVE_FORCE_FIPS_FAILURE */";
        printf '%s\n' "#define HAVE_FORCE_FIPS_FAILURE";
        printf '%s\n' "/* FIPS SETTINGS - END */";
        printf '%s\n' "";
        printf '%s\n' "/* Debugging */";
        printf '%s\n' "/* #define WOLFSSL_DEBUG_MEMORY */";
        printf '%s\n' "/* #define WOLFSSL_TRACK_MEMORY */";
        printf '%s\n' "/* #define WOLFSSL_DEBUG_MEMORY_PRINT */";
        printf '%s\n' "/* Debugging */";
        printf '%s\n' "";
        printf '%s\n' "/* Environment settings */";
        printf '%s\n' "#define NO_FILESYSTEM";
        printf '%s\n' "#define USE_FAST_MATH";
        printf '%s\n' "#define NO_MAIN_DRIVER";
        printf '%s\n' "#define WOLFCRYPT_ONLY";
        printf '%s\n' "#define WC_RSA_BLINDING";
        printf '%s\n' "#define SINGLE_THREADED";
        printf '%s\n' "#define TFM_TIMING_RESISTANT";
        printf '%s\n' "#define ECC_TIMING_RESISTANT";
        printf '%s\n' "#define USE_CERT_BUFFERS_256";
        printf '%s\n' "#define USE_CERT_BUFFERS_2048";
        printf '%s\n' "#define WOLFSSL_STM32L4";
        printf '%s\n' "#define WOLFSSL_STM32_CUBEMX";
        printf '%s\n' "#define WOLFSSL_CUBEMX_USE_LL";
        printf '%s\n' "#define STM32_RNG";
        printf '%s\n' "#define NO_STM32_CRYPTO";
        printf '%s\n' "#define NO_STM32_HASH";
        printf '%s\n' "#define NO_OLD_RNGNAME";
        printf '%s\n' "/* Environment settings */";
        printf '%s\n' "";
        printf '%s\n' "/* Tuning options */";
        printf '%s\n' "#define ALT_ECC_SIZE";
        printf '%s\n' "#define NO_RC4";
        printf '%s\n' "#define NO_MD4";
        printf '%s\n' "#define NO_PSK";
        printf '%s\n' "#define GCM_SMALL";
        printf '%s\n' "#define TFM_ECC256";
        printf '%s\n' "#define ECC_SHAMIR";
        printf '%s\n' "#define HAVE_AESGCM";
        printf '%s\n' "#define NO_PWDBASED";
        printf '%s\n' "/* Tuning options */";
        printf '%s\n' "";
        printf '%s\n' "/* Non-FIPS related settings */";
        printf '%s\n' "#define HAVE_TLS_EXTENSIONS";
        printf '%s\n' "#define HAVE_EXTENDED_MASTER";
        printf '%s\n' "#define HAVE_SUPPORTED_CURVES";
        printf '%s\n' "/* Non-FIPS related settings */";
        printf '%s\n' "";
        printf '%s\n' "/* Agent harness settings */";
        printf '%s\n' "#define USE_NORMAL_PRINTF";
        printf '%s\n' "#define STM32L4R9I_DISCO";
        printf '%s\n' "#define USE_NORMAL_SCAN";
        printf '%s\n' "#define HAVE_FIPS";
        printf '%s\n' "#define HAVE_FIPS_VERSION 2";
        printf '%s\n' "#define VERIFY_GENERATED_PSS_SIGS";
        printf '%s\n' "/* Agent harness settings */";
        printf '%s\n' "";
        printf '%s\n' "#endif /* STM32L4_V_4_0_1_USER_SETTINGS_H */";
        printf '%s\n' "";
    } >> user_settings.h

    printf '%s\n' "new user_settings.h has been created"
}

assumptions

if [ -f wolfssl/ssl.h ]; then
    if [ -f "$FIRMWARE_ROOT"/project.mk ]; then
        printf '%s\n' "Found ../../../project.mk, wolfSSL properly placed in"
        printf '%s\n' "application root directory"
    else
        printf '%s\n' "Failed to locate ../../../project.mk, wolfSSL in wrong"
        printf '%s\n' "location or assumptions need updated."
    fi
else
    printf '%s\n' "Run this script from the wolfSSL root directory"
    exit 1
fi

flatten_wolfcrypt_sources
# optional test application, remove if not testing
if [ -f "$APP_ROOT/test.c" ]; then
    printf '%s\n' "Removing: $APP_ROOT/test.c"
    rm "$APP_ROOT/test.c"
fi
printf '%s\n' "WOLF_ROOT/wolfcrypt/test/test.c --> APP_ROOT/test.c"
cp "$WOLF_ROOT/wolfcrypt/test/test.c" "$APP_ROOT/"
# optional test application section end

# used during fips validation only, these will not be in final distribution
#./scripts/flatten-agent-sources.sh
#./scripts/flatten-op-test.sh
# used during fips validation only, these will not be in final distribution

update_user_settings

cd "$FIRMWARE_ROOT"
make clean
make -j 1
cd "$APP_ROOT"
make install-target
