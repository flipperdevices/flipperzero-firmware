# Notes on the wolfssl-fips project

First, if you did not get the FIPS files with your archive, you must contact
wolfSSL to obtain them.

The IDE/WIN10/wolfssl-fips.sln solution is for the FIPS 140-3 certificate or later.

# Building the wolfssl-fips project

The wolfCrypt FIPS library for Windows is a part of the wolfSSL library. It
must be built as a static library, for the moment.

The library project is built with Whole Program Optimization disabled. This is
required so that necessary components of the library are not optimized away.
There are two functions added to the library that are used as markers in
memory for the in-core memory check of the code. WPO consolidates them into a
single function. WPO also optimizes away the automatic FIPS entry function.

Each of the source files inside the FIPS boundary defines their own code and
constant section. The code section names start with ".fipsA$" and the constant
section names start with ".fipsB$". Each subsection has a letter to organize
them in a specific order. This specific ordering puts marker functions and
constants on either end of the boundary so it can be hashed.


# In Core Memory Test

The In Core Memory test calculates a checksum (HMAC-SHA256) of the wolfCrypt
FIPS library code and constant data and compares it with a known value in
the code.

The Randomized Base Address setting needs to be disabled on all builds as the 
feature throws off the in-core memory calculation causing the test to fail.

The "verifyCore" check value in the source fips_test.c needs to be updated when
building the code. The POS performs this check and the default failure callback
will print out the calculated checksum. When developing your code, copy this
value and paste it back into your code in the verifyCore initializer then
rebuild the code. When statically linking, you may have to recalculate your
check value when changing your application.


# Build Options

The default build options should be the proper default set of options:

 * HAVE_FIPS
 * HAVE_FIPS_VERSION=5
 * HAVE_FIPS_VERSION_MINOR=1 (Also for FIPS Ready)
 * HAVE_THREAD_LS
 * WOLFSSL_KEY_GEN
 * HAVE_AESGCM
 * HAVE_HASHDRBG
 * WOLFSSL_SHA384
 * WOLFSSL_SHA512
 * NO_PSK
 * NO_RC4
 * NO_DSA
 * NO_MD4
 * WOLFSSL_SHA224
 * WOLFSSL_SHA3
 * WC_RSA_PSS
 * WC_RSA_NO_PADDING
 * HAVE_ECC
 * ECC_SHAMIR
 * HAVE_ECC_CDH
 * ECC_TIMING_RESISTANT
 * TFM_TIMING_RESISTANT
 * WOLFSSL_AES_COUNTER
 * WOLFSSL_AES_DIRECT
 * HAVE_AES_ECB
 * HAVE_AESCCM
 * WOLFSSL_CMAC
 * HAVE_HKDF
 * WOLFSSL_VALIDATE_ECC_IMPORT
 * WOLFSSL_VALIDATE_FFC_IMPORT
 * HAVE_FFDHE_Q
 * NO_DES
 * NO_DES3
 * NO_MD5
 * NO_OLD_TLS
 * WOLFSSL_TLS13
 * HAVE_TLS_EXTENSIONS
 * HAVE_SUPPORTED_CURVES
 * GCM_TABLE_4BIT
 * WOLFSSL_NO_SHAKE256
 * WOLFSSL_VALIDATE_ECC_KEYGEN
 * WOLFSSL_ECDSA_SET_K
 * WOLFSSL_WOLFSSH
 * WOLFSSL_PUBLIC_MP
 * WC_RNG_SEED_CB
 * TFM_ECC256
 * ECC_USER_CURVES
 * HAVE_ECC192
 * HAVE_ECC224
 * HAVE_ECC256
 * HAVE_ECC384
 * HAVE_ECC521
 * HAVE_FFDHE_2048
 * HAVE_FFDHE_3072
 * HAVE_FFDHE_4096
 * HAVE_FFDHE_6144
 * HAVE_FFDHE_8192
 * FP_MAX_BITS 16384

The "NO" options explicitly disable algorithms that are not allowed in
FIPS mode.

Additionally one may enable:

 * WOLFSSL_AESNI
 * OPENSSL_EXTRA

These settings are defined in IDE/WIN10/user_settings.h.
