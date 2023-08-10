/* unit.c API unit tests driver
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


/* Name change compatibility layer no longer need to be included here */

#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/wolfcrypt/types.h>

#include <stdio.h>
#include <tests/unit.h>
#include <wolfssl/wolfcrypt/fips_test.h>


int allTesting = 1;
int apiTesting = 1;
int myoptind = 0;
char* myoptarg = NULL;
int unit_test(int argc, char** argv);

#ifndef NO_TESTSUITE_MAIN_DRIVER
int main(int argc, char** argv)
{
    return unit_test(argc, argv);
}
#endif

/* Print usage options for unit test.
 */
static void UnitTest_Usage(void)
{
    printf("Usage: ./tests/unit.test <options>\n");
    printf(" -?, --help     Display this usage information.\n");
    printf(" --list         List the API tests.\n");
    printf(" --api          Only perform API tests.\n");
    printf(" -<number>      Run the API test identified by number.\n");
    printf("                Can be specified multiple times.\n");
    printf(" -<string>      Run the API test identified by name.\n");
    printf("                Can be specified multiple times.\n");
    printf(" <filename>     Name of cipher suite testing file.\n");
}

int unit_test(int argc, char** argv)
{
    int ret = 0;

    (void)argc;
    (void)argv;

#ifdef WOLFSSL_FORCE_MALLOC_FAIL_TEST
    if (argc > 1) {
        int memFailCount = atoi(argv[1]);
        fprintf(stderr, "\n--- SET RNG MALLOC FAIL AT %d---\n", memFailCount);
        wolfSSL_SetMemFailCount(memFailCount);
    }
#endif

    printf("starting unit tests...\n");
    fflush(stdout);

#if defined(DEBUG_WOLFSSL) && !defined(HAVE_VALGRIND)
    wolfSSL_Debugging_ON();
#endif

#ifdef WC_RNG_SEED_CB
    wc_SetSeed_Cb(wc_GenerateSeed);
#endif
#ifdef HAVE_WNR
    if (wc_InitNetRandom(wnrConfig, NULL, 5000) != 0)
        err_sys("Whitewood netRandom global config failed");
#endif /* HAVE_WNR */

#ifndef WOLFSSL_TIRTOS
    ChangeToWolfRoot();
#endif

#if defined(HAVE_FIPS) && defined(HAVE_FIPS_VERSION) && (HAVE_FIPS_VERSION == 5)
#if !defined(NO_AES) && !defined(NO_AES_CBC)
    if (wc_RunCast_fips(FIPS_CAST_AES_CBC) != 0) {
        err_sys("AES-CBC CAST failed");
    }
#endif
#ifdef HAVE_AESGCM
    if (wc_RunCast_fips(FIPS_CAST_AES_GCM) != 0) {
        err_sys("AES-GCM CAST failed");
    }
#endif
#ifndef NO_SHA
    if (wc_RunCast_fips(FIPS_CAST_HMAC_SHA1) != 0) {
        err_sys("HMAC-SHA1 CAST failed");
    }
#endif
    /* the only non-optional CAST */
    if (wc_RunCast_fips(FIPS_CAST_HMAC_SHA2_256) != 0) {
        err_sys("HMAC-SHA2-256 CAST failed");
    }
#ifdef WOLFSSL_SHA512
    if (wc_RunCast_fips(FIPS_CAST_HMAC_SHA2_512) != 0) {
        err_sys("HMAC-SHA2-512 CAST failed");
    }
#endif
#ifdef WOLFSSL_SHA3
    if (wc_RunCast_fips(FIPS_CAST_HMAC_SHA3_256) != 0) {
        err_sys("HMAC-SHA3-256 CAST failed");
    }
#endif
#ifdef HAVE_HASHDRBG
    if (wc_RunCast_fips(FIPS_CAST_DRBG) != 0) {
        err_sys("Hash_DRBG CAST failed");
    }
#endif
#ifndef NO_RSA
    if (wc_RunCast_fips(FIPS_CAST_RSA_SIGN_PKCS1v15) != 0) {
        err_sys("RSA sign CAST failed");
    }
#endif
#if defined(HAVE_ECC_CDH) && defined(HAVE_ECC_CDH_CAST)
    if (wc_RunCast_fips(FIPS_CAST_ECC_CDH) != 0) {
        err_sys("RSA sign CAST failed");
    }
#endif
#ifdef HAVE_ECC_DHE
    if (wc_RunCast_fips(FIPS_CAST_ECC_PRIMITIVE_Z) != 0) {
        err_sys("ECC Primitive Z CAST failed");
    }
#endif
#ifdef HAVE_ECC
    if (wc_RunCast_fips(FIPS_CAST_ECDSA) != 0) {
        err_sys("ECDSA CAST failed");
    }
#endif
#ifndef NO_DH
    if (wc_RunCast_fips(FIPS_CAST_DH_PRIMITIVE_Z) != 0) {
        err_sys("DH Primitive Z CAST failed");
    }
#endif
#ifdef WOLFSSL_HAVE_PRF
    if (wc_RunCast_fips(FIPS_CAST_KDF_TLS12) != 0) {
        err_sys("KDF TLSv1.2 CAST failed");
    }
#endif
#if defined(WOLFSSL_HAVE_PRF) && defined(WOLFSSL_TLS13)
    if (wc_RunCast_fips(FIPS_CAST_KDF_TLS13) != 0) {
        err_sys("KDF TLSv1.3 CAST failed");
    }
#endif
#ifdef WOLFSSL_WOLFSSH
    if (wc_RunCast_fips(FIPS_CAST_KDF_SSH) != 0) {
        err_sys("KDF SSHv2.0 CAST failed");
    }
#endif
#endif /* HAVE_FIPS && HAVE_FIPS_VERSION == 5 */

    while (argc > 1) {
        if (argv[1][0] != '-') {
            break;
        }

        if (XSTRCMP(argv[1], "-?") == 0 || XSTRCMP(argv[1], "--help") == 0) {
            UnitTest_Usage();
            goto exit;
        }
        else if (XSTRCMP(argv[1], "--list") == 0) {
            ApiTest_PrintTestCases();
            goto exit;
        }
        else if (XSTRCMP(argv[1], "--api") == 0) {
            allTesting = 0;
        }
        else if (XSTRCMP(argv[1], "--no-api") == 0) {
            apiTesting = 0;
        }
        else if (argv[1][1] >= '0' && argv[1][1] <= '9') {
            ret = ApiTest_RunIdx(atoi(argv[1] + 1));
            if (ret != 0) {
                goto exit;
            }
            allTesting = 0;
        }
        else {
            ret = ApiTest_RunName(argv[1] + 1);
            if (ret != 0) {
                goto exit;
            }
            allTesting = 0;
        }

        argc--;
        argv++;
    }

#ifdef WOLFSSL_ALLOW_SKIP_UNIT_TESTS
    if (argc == 1)
#endif
    {
        if (apiTesting) {
            ret = ApiTest();
            if (ret != 0)
                goto exit;
        }

        if (!allTesting) {
            goto exit;
        }

        if ((ret = HashTest()) != 0) {
            fprintf(stderr, "hash test failed with %d\n", ret);
            goto exit;
        }

    #ifdef WOLFSSL_W64_WRAPPER
        if ((ret = w64wrapper_test()) != 0) {
            fprintf(stderr, "w64wrapper test failed with %d\n", ret);
            goto exit;
        }
    #endif /* WOLFSSL_W64_WRAPPER */

    #ifdef WOLFSSL_QUIC
        if ((ret = QuicTest()) != 0) {
            printf("quic test failed with %d\n", ret);
            goto exit;
        }
    #endif

        SrpTest();
    }

#ifndef NO_WOLFSSL_CIPHER_SUITE_TEST
#if !defined(NO_WOLFSSL_CLIENT) && !defined(NO_WOLFSSL_SERVER)
#ifndef SINGLE_THREADED
    if ((ret = SuiteTest(argc, argv)) != 0) {
        fprintf(stderr, "suite test failed with %d\n", ret);
        goto exit;
    }
#endif
#endif
#endif /* NO_WOLFSSL_CIPHER_SUITE_TEST */

exit:
#ifdef HAVE_WNR
    if (wc_FreeNetRandom() < 0)
        err_sys("Failed to free netRandom context");
#endif /* HAVE_WNR */

    if (ret == 0) {
        puts("\nunit_test: Success for all configured tests.");
        fflush(stdout);
    }

    return ret;
}
