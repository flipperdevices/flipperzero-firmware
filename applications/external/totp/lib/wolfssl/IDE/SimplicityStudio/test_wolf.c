/* test_wolf.c
 *
 * Copyright (C) 2006-2023 wolfSSL Inc.
 *
 * This file is part of wolfSSL.
 *
 * wolfSSL is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * wolfSSL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1335, USA
 */

/* Example for running wolfCrypt test and benchmark from
 * SiLabs Simplicity Studio's CLI example */

#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/wolfcrypt/signature.h>
#include <wolfssl/wolfcrypt/ecc.h>
#include <wolfssl/wolfcrypt/asn_public.h>
#include <wolfssl/wolfcrypt/logging.h>
#include <wolfssl/wolfcrypt/wc_port.h>
#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfcrypt/test/test.h>
#include <wolfcrypt/benchmark/benchmark.h>
#include <stdio.h>

#include "sl_cli.h"
#include "sl_cli_instances.h"
#include "sl_cli_arguments.h"
#include "sl_cli_handles.h"

#ifndef NO_CRYPT_TEST
typedef struct func_args {
    int    argc;
    char** argv;
    int    return_code;
} func_args;

static func_args args = { 0 };
#endif

void wolf_test(sl_cli_command_arg_t *arguments)
{
    int ret;
#ifndef NO_CRYPT_TEST
    wolfCrypt_Init();

    printf("\nCrypt Test\n");
    wolfcrypt_test(&args);
    ret = args.return_code;
    printf("Crypt Test: Return code %d\n", ret);

    wolfCrypt_Cleanup();
#else
    ret = NOT_COMPILED_IN;
#endif
    (void)arguments;
    (void)ret;
}

void wolf_bench(sl_cli_command_arg_t *arguments)
{
    int ret;
#ifndef NO_CRYPT_BENCHMARK
    wolfCrypt_Init();

    printf("\nBenchmark Test\n");
    benchmark_test(&args);
    ret = args.return_code;
    printf("Benchmark Test: Return code %d\n", ret);

    wolfCrypt_Cleanup();
#else
    ret = NOT_COMPILED_IN;
#endif
    (void)arguments;
    (void)ret;
}

/* ecc key gen, sign and verify examples */
#define TEST_ECC_KEYSZ    32
#define TEST_DATA_SIZE    128
#define TEST_KEYGEN_TRIES 100
#define TEST_ECDSA_TRIES  100
/* #define TEST_VERIFY_ONLY */
/* #define TEST_PRIVATE_IMPORT */

void wolf_ecc_test(sl_cli_command_arg_t *arguments)
{
    int ret = 0, i, j;
    byte data[TEST_DATA_SIZE];
    word32 dataLen = (word32)sizeof(data);
    WC_RNG rng;
    ecc_key eccKey;
    word32 inOutIdx;
#ifdef TEST_PRIVATE_IMPORT
    /* ./certs/ecc-client-key.der, ECC */
    const unsigned char ecc_clikey_der_256[] = {
            0x30, 0x77, 0x02, 0x01, 0x01, 0x04, 0x20, 0xF8, 0xCF, 0x92,
            0x6B, 0xBD, 0x1E, 0x28, 0xF1, 0xA8, 0xAB, 0xA1, 0x23, 0x4F,
            0x32, 0x74, 0x18, 0x88, 0x50, 0xAD, 0x7E, 0xC7, 0xEC, 0x92,
            0xF8, 0x8F, 0x97, 0x4D, 0xAF, 0x56, 0x89, 0x65, 0xC7, 0xA0,
            0x0A, 0x06, 0x08, 0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x03, 0x01,
            0x07, 0xA1, 0x44, 0x03, 0x42, 0x00, 0x04, 0x55, 0xBF, 0xF4,
            0x0F, 0x44, 0x50, 0x9A, 0x3D, 0xCE, 0x9B, 0xB7, 0xF0, 0xC5,
            0x4D, 0xF5, 0x70, 0x7B, 0xD4, 0xEC, 0x24, 0x8E, 0x19, 0x80,
            0xEC, 0x5A, 0x4C, 0xA2, 0x24, 0x03, 0x62, 0x2C, 0x9B, 0xDA,
            0xEF, 0xA2, 0x35, 0x12, 0x43, 0x84, 0x76, 0x16, 0xC6, 0x56,
            0x95, 0x06, 0xCC, 0x01, 0xA9, 0xBD, 0xF6, 0x75, 0x1A, 0x42,
            0xF7, 0xBD, 0xA9, 0xB2, 0x36, 0x22, 0x5F, 0xC7, 0x5D, 0x7F,
            0xB4
    };
    const int sizeof_ecc_clikey_der_256 = sizeof(ecc_clikey_der_256);
#endif
#ifdef TEST_VERIFY_ONLY
    /* ./certs/ecc-client-keyPub.der, ECC */
    const unsigned char ecc_clikeypub_der_256[] = {
        0x30, 0x59, 0x30, 0x13, 0x06, 0x07, 0x2A, 0x86, 0x48, 0xCE,
        0x3D, 0x02, 0x01, 0x06, 0x08, 0x2A, 0x86, 0x48, 0xCE, 0x3D,
        0x03, 0x01, 0x07, 0x03, 0x42, 0x00, 0x04, 0x55, 0xBF, 0xF4,
        0x0F, 0x44, 0x50, 0x9A, 0x3D, 0xCE, 0x9B, 0xB7, 0xF0, 0xC5,
        0x4D, 0xF5, 0x70, 0x7B, 0xD4, 0xEC, 0x24, 0x8E, 0x19, 0x80,
        0xEC, 0x5A, 0x4C, 0xA2, 0x24, 0x03, 0x62, 0x2C, 0x9B, 0xDA,
        0xEF, 0xA2, 0x35, 0x12, 0x43, 0x84, 0x76, 0x16, 0xC6, 0x56,
        0x95, 0x06, 0xCC, 0x01, 0xA9, 0xBD, 0xF6, 0x75, 0x1A, 0x42,
        0xF7, 0xBD, 0xA9, 0xB2, 0x36, 0x22, 0x5F, 0xC7, 0x5D, 0x7F,
        0xB4
    };
    const int sizeof_ecc_clikeypub_der_256 = sizeof(ecc_clikeypub_der_256);

    /* Signature for hash of 128-bytes data 0,1,2.... */
    const byte sig[] = {
        0x30, 0x45,
            0x02, 0x20,
                0x5e, 0x01, 0x3d, 0xfb, 0x18, 0x61, 0xe3, 0xe3,
                0xa0, 0xff, 0x9b, 0x99, 0xb5, 0x2f, 0x89, 0xba,
                0xb3, 0xbb, 0x62, 0x08, 0x9b, 0x94, 0x0e, 0x61,
                0x9c, 0x00, 0x7b, 0x43, 0x9c, 0x28, 0xcb, 0xf6,
            0x02, 0x21,
                0x00,
                0xb4, 0x72, 0x2f, 0x22, 0x92, 0xa2, 0x8d, 0x01,
                0x6b, 0x01, 0x35, 0x5c, 0xcb, 0x34, 0xa2, 0x21,
                0x32, 0xe2, 0xe6, 0x66, 0xd0, 0x9e, 0x3e, 0x8e,
                0x84, 0x5d, 0xa4, 0x69, 0x54, 0x52, 0xfc, 0xef
    };
    word32 sigLen = (word32)sizeof(sig);
#else
    byte sig[ECC_MAX_SIG_SIZE];
    word32 sigLen;
#endif

    memset(&rng, 0, sizeof(rng));
    memset(&eccKey, 0, sizeof(eccKey));

    wolfSSL_Debugging_ON();

    wolfCrypt_Init();

    /* test data */
    for (i=0; i<(int)dataLen; i++) {
        data[i] = (byte)i;
    }

    ret = wc_InitRng(&rng);
    if (ret != 0) {
        goto exit;
    }

    for (i=0; i<TEST_KEYGEN_TRIES; i++) {
        ret = wc_ecc_init_ex(&eccKey, NULL, 0);
        if (ret == 0) {
            inOutIdx = 0;
#ifdef TEST_VERIFY_ONLY
            ret = wc_EccPublicKeyDecode(ecc_clikeypub_der_256, &inOutIdx,
                &eccKey, sizeof_ecc_clikeypub_der_256);
#else
    #if (defined(_SILICON_LABS_SECURITY_FEATURE) && \
        _SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
            /* Load ecc_key with vault's public key.
             * When only the public area of a key is loaded silabs_ecc.c
             * (silabs_ecc_sign_hash) will use the vault key to sign */
            ret = silabs_ecc_load_vault(&eccKey);
    #elif defined(TEST_PRIVATE_IMPORT)
            ret = wc_EccPrivateKeyDecode(ecc_clikey_der_256, &inOutIdx,
                &eccKey, sizeof_ecc_clikey_der_256);
    #else
            ret = wc_ecc_make_key(&rng, TEST_ECC_KEYSZ, &eccKey);
    #endif
#endif
        }

        for (j=0; j<TEST_ECDSA_TRIES; j++) {
#ifndef TEST_VERIFY_ONLY
            if (ret == 0) {
                /* generate signature using ecc key */
                sigLen = (word32)sizeof(sig);

                ret = wc_SignatureGenerate(
                    WC_HASH_TYPE_SHA256, WC_SIGNATURE_TYPE_ECC,
                    data, dataLen,
                    sig, &sigLen,
                    &eccKey, (word32)sizeof(eccKey),
                    &rng);
            }
#endif
            if (ret == 0) {
                ret = wc_SignatureVerify(
                    WC_HASH_TYPE_SHA256, WC_SIGNATURE_TYPE_ECC,
                    data, dataLen,
                    sig, sigLen,
                    &eccKey, (word32)sizeof(eccKey));
            }

            if (ret == 0) {
                fprintf(stderr, "Verification Passed %d %d\n", i, j);
            }
            else {
                fprintf(stderr, "Verification failed!! (ret %d) %d %d\n",
                    ret, i, j);
                break;
            }
        } /* sign/verify tries */

        wc_ecc_free(&eccKey);
        if (ret != 0)
            break;
    } /* key gen tries */

exit:
    wc_FreeRng(&rng);

    wolfCrypt_Cleanup();

    (void)arguments;
}
