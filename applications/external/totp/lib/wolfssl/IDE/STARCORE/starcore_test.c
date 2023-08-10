/* starcore_test.c
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


#include <prototype.h> 

#include <wolfssl/wolfcrypt/settings.h>
#ifndef WOLFSSL_USER_SETTINGS
    #error "USER SETTINGS not set"
#endif
#include <wolfcrypt/test/test.h>
#include <wolfssl/wolfcrypt/wc_port.h>
#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/wolfcrypt/random.h>
#include <wolfcrypt/benchmark/benchmark.h>
#ifdef HAVE_FIPS
    #include <wolfssl/wolfcrypt/fips_test.h>
    /* #include <wolfacvp/wolfacvp.h> */
  #if 1
    #include <op_test.h>
  #endif
#endif

typedef struct test_func_args {
    int argc;
    char** argv;
    int return_code;
} test_func_args;

#ifdef HAVE_FIPS
    static void appFipsCb(int ok, int err, const char* hash)
    {
        printf("in appFipsCb Fips callback, ok = %d, err = %d\n", ok, err);
        fflush(stdout);
        printf("message = %s\n", wc_GetErrorString(err));
        fflush(stdout);
        printf("hash = %s\n", hash);
        fflush(stdout);

        if (err == IN_CORE_FIPS_E) {
            printf("In core integrity hash check failure, copy above hash\n");
            fflush(stdout);
            printf("into verifyCore[] in fips_test.c and rebuild\n");
            fflush(stdout);
        }
        fflush(stdout);
    }
#endif

int my_rng_generate_seed(unsigned char* output, int sz)
{
    unsigned int i;
    long t;

    time(&t); /* init */
    srand(t); /* init */
    for (i = 0; i < sz; i++) {
        output[i] = (unsigned char) rand();
    }
    return 0;
}

#define SAMPLE_SIZE 32
int simpleRngTest(void)
{
    WC_RNG rng;
    int ret;
    char block[SAMPLE_SIZE] = {0};

    ret = wc_InitRng(&rng);
    if (ret != 0) {
        printf("Failed to init RNG with return %d\n", ret);
        fflush(stdout);
        return ret;
    }

    ret = wc_RNG_GenerateBlock(&rng, (byte*) block, SAMPLE_SIZE);
    /* This should trigger FIPS callback if the hash needs updated */
    printf("ret from wc_RNG_GenerateBlock was %d\n", ret);
    fflush(stdout);
    return ret;
}

#define FILE_PATH_LEN 160
/* The directory used to hold the vector files, update accordingly */
#define FIXED_PATH "../../../../fips/wolfACVP/wolfSSL-14699641-PRODUCTION/"

void reset_path(char* fName) {
    memset(fName, 0, FILE_PATH_LEN);
    strncpy(fName, FIXED_PATH, strlen(FIXED_PATH));
}

#if 0
/* Disable when not processing vector files */
int process_a_file(char* fName)
{
    int ret = 0;
    test_func_args args = {0};
    char* argIn[3] = {0};
    argIn[0] = "wolfacvp_client";
    argIn[1] = "-i";
    argIn[2] = fName;
#if 0
    argIn[3] = "-K"; /* Remove this when processing vectors from the lab,
                              * we don't know the answers to those */
    args.argc = 4; /* change to 3 when processing vectors from the lab */
#else
    args.argc = 3;
#endif
    args.argv = argIn;
    printf("Case: START\n");
    ret = wolfacvp_harness(args.argc, args.argv);
    if (ret != 0) {
        printf("Result of wolfCrypt harness is %d\n", ret); fflush(stdout);
    } else {
        printf("Case: END\n");
    }
    return ret;
}
#endif 

void check_ret(int ret)
{
    if(ret != 0) {
        exit(-1); 
    }
}

int testharness_main()
{
    int ret = -1;
    int* CpuidAddr=(int*) 0xFFF28028;

    printf("The CPU ID is stored at 0xFFF28028 and it's value is %04x\n", *CpuidAddr);

#ifdef HAVE_FIPS
        wolfCrypt_SetCb_fips(appFipsCb);
        /* fipsEntry(); */ /* Prior to Vortec Scheduler was called here, call is
                            * now at the vortec Scheduler level during Core Init
                            */
#endif

    ret = simpleRngTest();

    if (ret == 0) {

    /* uncomment to see performance */
    #if 0 /* Benchmark */
        printf("Kicking off the benchmark\n"); fflush(stdout);
        ret = benchmark_test(NULL);
        printf("Result of wolfCrypt benchmark is %d\n", ret); fflush(stdout);
    #endif

#if 0 /* File-based Harness */
    #ifdef HAVE_FIPS
        char* testFN = "grep-for-this-file.txt";
        char* testStr = "This is my test string, hello from STARCORE!\n";
        char path[FILE_PATH_LEN] = {0};
        FILE* fstream = fopen(testFN, "wb");

        strncpy(path, FIXED_PATH, strlen(FIXED_PATH));

    #if 0 /* Crypto Tests */
        printf("Running testwolfcrypt on starcore\n"); fflush(stdout);
        wolfcrypt_test(NULL);
    #endif

        if (fstream == NULL) {
            printf("Failed to open testFN\n");
        } else {
            ret = fwrite(testStr, 1, strlen(testStr), fstream);
            printf("Wrote %d bytes to fstream\n", ret);
            fclose(fstream);
        }
#if 0 /* PRODUCTION VECTORS */
        //process_a_file(strcat(path, "acvp-v1-testSessions-19392-vectorSets-215826.json")); reset_path(path); /* AES-ECB */
        //process_a_file(strcat(path, "acvp-v1-testSessions-19392-vectorSets-215827.json")); reset_path(path); /* AES-CBC */
        //process_a_file(strcat(path, "acvp-v1-testSessions-19392-vectorSets-215828.json")); reset_path(path); /* AES-CTR */
        //process_a_file(strcat(path, "acvp-v1-testSessions-19397-vectorSets-215833.json")); reset_path(path); /* ECDSA keyGen */
        //process_a_file(strcat(path, "acvp-v1-testSessions-19397-vectorSets-215834.json")); reset_path(path); /* ECDSA keyVer */
        //process_a_file(strcat(path, "acvp-v1-testSessions-19397-vectorSets-215835.json")); reset_path(path); /* ECDSA sigGen */
        //process_a_file(strcat(path, "acvp-v1-testSessions-19397-vectorSets-215836.json")); reset_path(path); /* ECDSA sigVer */
        //process_a_file(strcat(path, "acvp-v1-testSessions-19400-vectorSets-215845.json")); reset_path(path); /* HMAC-SHA1 */
        //process_a_file(strcat(path, "acvp-v1-testSessions-19400-vectorSets-215846.json")); reset_path(path); /* HMAC-SHA2-224 */
        //process_a_file(strcat(path, "acvp-v1-testSessions-19400-vectorSets-215847.json")); reset_path(path); /* HMAC-SHA2-256 */
        //process_a_file(strcat(path, "acvp-v1-testSessions-19400-vectorSets-215848.json")); reset_path(path); /* HMAC-SHA2-384 */
        //process_a_file(strcat(path, "acvp-v1-testSessions-19400-vectorSets-215849.json")); reset_path(path); /* HMAC-SHA2-512 */
        //process_a_file(strcat(path, "acvp-v1-testSessions-19400-vectorSets-215850.json")); reset_path(path); /* HMAC-SHA3-224 */
        //process_a_file(strcat(path, "acvp-v1-testSessions-19400-vectorSets-215851.json")); reset_path(path); /* HMAC-SHA3-256 */
        //process_a_file(strcat(path, "acvp-v1-testSessions-19400-vectorSets-215852.json")); reset_path(path); /* HMAC-SHA3-384 */
        //process_a_file(strcat(path, "acvp-v1-testSessions-19400-vectorSets-215853.json")); reset_path(path); /* HMAC-SHA3-512 */
        process_a_file(strcat(path, "acvp-v1-testSessions-19404-vectorSets-215857.json")); reset_path(path); /* RSA keyGen */
        //process_a_file(strcat(path, "acvp-v1-testSessions-19404-vectorSets-215858.json")); reset_path(path); /* RSA sigGen */
        //process_a_file(strcat(path, "acvp-v1-testSessions-19404-vectorSets-215859.json")); reset_path(path); /* RSA sigVer */
        //process_a_file(strcat(path, "acvp-v1-testSessions-19404-vectorSets-215860.json")); reset_path(path); /* RSA decPrim */
        //process_a_file(strcat(path, "acvp-v1_testSessions_19395_vectorSets_215831.json")); reset_path(path); /* hashDRBG */
        //process_a_file(strcat(path, "acvp-v1_testSessions_19396_vectorSets_215832.json")); reset_path(path); /* DSA */
        //process_a_file(strcat(path, "acvp-v1_testSessions_19401_vectorSets_215854.json")); reset_path(path); /* KAS-ECC */
        //process_a_file(strcat(path, "acvp-v1_testSessions_19402_vectorSets_215855.json")); reset_path(path); /* KAS-ECC-SSC */
        //process_a_file(strcat(path, "acvp-v1_testSessions_19403_vectorSets_215856.json")); reset_path(path); /* KAS-FFC-SSC */
        //process_a_file(strcat(path, "acvp-v1_testSessions_19407_vectorSets_215870.json")); reset_path(path); /* TDES */
        //process_a_file(strcat(path, "acvp-v1_testSessions_19394_vectorSets_215830.json")); reset_path(path); /* CMAC_AES */
        //process_a_file(strcat(path, "acvp-v1_testSessions_19393_vectorSets_215829.json")); reset_path(path); /* AES-CCM */
        //process_a_file(strcat(path, "acvp-v1-testSessions-19398-vectorSets-215837.json")); reset_path(path); /* AES-GCM */
        //process_a_file(strcat(path, "acvp-v1-testSessions-19398-vectorSets-215838.json")); reset_path(path); /* AES-GCM */
        //process_a_file(strcat(path, "acvp-v1-testSessions-19398-vectorSets-215839.json")); reset_path(path); /* AES-GCM */
        //process_a_file(strcat(path, "acvp-v1-testSessions-19398-vectorSets-215840.json")); reset_path(path); /* AES-GCM */
        //process_a_file(strcat(path, "acvp-v1-testSessions-19399-vectorSets-215841.json")); reset_path(path); /* AES-GMAC */
        //process_a_file(strcat(path, "acvp-v1-testSessions-19399-vectorSets-215842.json")); reset_path(path); /* AES-GMAC */
        //process_a_file(strcat(path, "acvp-v1-testSessions-19399-vectorSets-215843.json")); reset_path(path); /* AES-GMAC */
        //process_a_file(strcat(path, "acvp-v1-testSessions-19399-vectorSets-215844.json")); reset_path(path); /* AES-GMAC */
        //process_a_file(strcat(path, "acvp-v1-testSessions-19405-vectorSets-215861.json")); reset_path(path); /* SHA-1 */
        //process_a_file(strcat(path, "acvp-v1-testSessions-19406-vectorSets-215866.json")); reset_path(path); /* SHA3-224 */
        //process_a_file(strcat(path, "acvp-v1-testSessions-19406-vectorSets-215867.json")); reset_path(path); /* SHA3-256 */
        //process_a_file(strcat(path, "acvp-v1-testSessions-19406-vectorSets-215868.json")); reset_path(path); /* SHA3-384 */
        //process_a_file(strcat(path, "acvp-v1-testSessions-19406-vectorSets-215869.json")); reset_path(path); /* SHA3-512 */
        //process_a_file(strcat(path, "acvp-v1-testSessions-19405-vectorSets-215862.json")); reset_path(path); /* SHA2-224*/
        //process_a_file(strcat(path, "acvp-v1-testSessions-19405-vectorSets-215863.json")); reset_path(path); /* SHA2-256 */
        //process_a_file(strcat(path, "acvp-v1-testSessions-19405-vectorSets-215864.json")); reset_path(path); /* SHA2-384 */
        //process_a_file(strcat(path, "acvp-v1-testSessions-19405-vectorSets-215865.json")); reset_path(path); /* SHA2-512 */
#endif
#if 0 /* DEMO VECTORS */
        //process_a_file(strcat(path, "acvp-v1-testSessions-365458-vectorSets-1479140.json")); reset_path(path);
        //process_a_file(strcat(path, "acvp-v1-testSessions-365458-vectorSets-1479141.json")); reset_path(path);
        //process_a_file(strcat(path, "acvp-v1-testSessions-365458-vectorSets-1479142.json")); reset_path(path);
        //process_a_file(strcat(path, "acvp-v1-testSessions-365465-vectorSets-1479149.json")); reset_path(path);
        //process_a_file(strcat(path, "acvp-v1-testSessions-365465-vectorSets-1479150.json")); reset_path(path);
        //process_a_file(strcat(path, "acvp-v1-testSessions-365465-vectorSets-1479151.json")); reset_path(path);
        //process_a_file(strcat(path, "acvp-v1-testSessions-365465-vectorSets-1479152.json")); reset_path(path);
        //process_a_file(strcat(path, "acvp-v1-testSessions-365467-vectorSets-1479154.json")); reset_path(path);
        //process_a_file(strcat(path, "acvp-v1-testSessions-365467-vectorSets-1479155.json")); reset_path(path);
        //process_a_file(strcat(path, "acvp-v1-testSessions-365467-vectorSets-1479156.json")); reset_path(path);
        //process_a_file(strcat(path, "acvp-v1-testSessions-365467-vectorSets-1479157.json")); reset_path(path);
        //process_a_file(strcat(path, "acvp-v1-testSessions-365469-vectorSets-1479161.json")); reset_path(path);
        //process_a_file(strcat(path, "acvp-v1-testSessions-365469-vectorSets-1479162.json")); reset_path(path);
        //process_a_file(strcat(path, "acvp-v1-testSessions-365469-vectorSets-1479163.json")); reset_path(path);
        //process_a_file(strcat(path, "acvp-v1-testSessions-365469-vectorSets-1479164.json")); reset_path(path);
        //process_a_file(strcat(path, "acvp-v1-testSessions-365473-vectorSets-1479170.json")); reset_path(path);
        //process_a_file(strcat(path, "acvp-v1-testSessions-365473-vectorSets-1479171.json")); reset_path(path);
        //process_a_file(strcat(path, "acvp-v1-testSessions-365473-vectorSets-1479172.json")); reset_path(path);
        //process_a_file(strcat(path, "acvp-v1-testSessions-365473-vectorSets-1479173.json")); reset_path(path);
        //process_a_file(strcat(path, "acvp-v1-testSessions-365473-vectorSets-1479174.json")); reset_path(path);
        //process_a_file(strcat(path, "acvp-v1-testSessions-365473-vectorSets-1479175.json")); reset_path(path);
        //process_a_file(strcat(path, "acvp-v1-testSessions-365473-vectorSets-1479176.json")); reset_path(path);
        //process_a_file(strcat(path, "acvp-v1-testSessions-365473-vectorSets-1479177.json")); reset_path(path);
        //process_a_file(strcat(path, "acvp-v1-testSessions-365473-vectorSets-1479178.json")); reset_path(path);
        //process_a_file(strcat(path, "acvp-v1-testSessions-365485-vectorSets-1479194.json")); reset_path(path);
        //process_a_file(strcat(path, "acvp-v1-testSessions-365485-vectorSets-1479195.json")); reset_path(path);
        //process_a_file(strcat(path, "acvp-v1-testSessions-365485-vectorSets-1479196.json")); reset_path(path);
        //process_a_file(strcat(path, "acvp-v1-testSessions-365485-vectorSets-1479197.json")); reset_path(path);
        //process_a_file(strcat(path, "acvp-v1-testSessions-365488-vectorSets-1479202.json")); reset_path(path);
        //process_a_file(strcat(path, "acvp-v1-testSessions-365488-vectorSets-1479203.json")); reset_path(path);
        //process_a_file(strcat(path, "acvp-v1-testSessions-365488-vectorSets-1479204.json")); reset_path(path);
        //process_a_file(strcat(path, "acvp-v1-testSessions-365488-vectorSets-1479205.json")); reset_path(path);
        //process_a_file(strcat(path, "acvp-v1-testSessions-365488-vectorSets-1479206.json")); reset_path(path);
        //process_a_file(strcat(path, "acvp-v1-testSessions-365490-vectorSets-1479211.json")); reset_path(path);
        //process_a_file(strcat(path, "acvp-v1-testSessions-365490-vectorSets-1479212.json")); reset_path(path);
        //process_a_file(strcat(path, "acvp-v1-testSessions-365490-vectorSets-1479213.json")); reset_path(path);
        //process_a_file(strcat(path, "acvp-v1-testSessions-365490-vectorSets-1479214.json")); reset_path(path);
        //process_a_file(strcat(path, "acvp-v1_testSessions_365459_vectorSets_1479143.json")); reset_path(path);
        //process_a_file(strcat(path, "acvp-v1_testSessions_365461_vectorSets_1479145.json")); reset_path(path);
        //process_a_file(strcat(path, "acvp-v1_testSessions_365462_vectorSets_1479146.json")); reset_path(path);
        //process_a_file(strcat(path, "acvp-v1_testSessions_365463_vectorSets_1479147.json")); reset_path(path);
        //process_a_file(strcat(path, "acvp-v1_testSessions_365481_vectorSets_1479190.json")); reset_path(path);
        //process_a_file(strcat(path, "acvp-v1_testSessions_365483_vectorSets_1479192.json")); reset_path(path);
        //process_a_file(strcat(path, "acvp-v1_testSessions_365484_vectorSets_1479193.json")); reset_path(path);
        //process_a_file(strcat(path, "acvp-v1_testSessions_365491_vectorSets_1479215.json")); reset_path(path);
#endif

#if 0 /* VECTORS that passed previously (sanity check before asking FIPS lab
         to send vectors) */

/* ------------------------------------------------------------------- */
        process_a_file(strcat(path, "hashDRBG_47251.json"));// Success
        process_a_file(strcat(path, "AES_CBC_47242.json")); // Failed to allocate buffer large enough for file, fixed by chopping into smaller sections
        process_a_file(strcat(path, "AES_CCM_47247.json")); // Failed, increase stack/heap
        process_a_file(strcat(path, "AES_CCM_47247-part1.json")); // Failed to write out the entire response (got 104 of 370 tgId's in the response)
        process_a_file(strcat(path, "AES_CCM_47247-part2.json")); // Failed to write out the entire response (started at 371 and got to 429 of 741 tgId's in the response) looks like alloc failures, increase HEAP
        process_a_file(strcat(path, "DSA_keyGen_47253.json")); // Success
        process_a_file(strcat(path, "RSA_DecPrim_47306.json")); // Success
        process_a_file(strcat(path, "ECDSA_sigVer_47258.json")); // Success
        process_a_file(strcat(path, "AES_CMAC_47249.json")); // Success
        process_a_file(strcat(path, "AES_CTR_47243.json")); // Success
        process_a_file(strcat(path, "AES_ECB_47241.json")); // Success
        process_a_file(strcat(path, "AES_GCM_external_8_2_1_47263.json")); // Success
        process_a_file(strcat(path, "AES_GCM_external_8_2_2_47265.json")); // Success
        process_a_file(strcat(path, "AES_GCM_internal_8_2_1_47267.json")); // Success
        process_a_file(strcat(path, "AES_GCM_internal_8_2_2_47269.json")); // Success
        process_a_file(strcat(path, "AES_GMAC_external_8_2_1_47271.json")); // Failed to alloc response (passed but couldn't output), use smaller chunks
        process_a_file(strcat(path, "AES_CMAC_47249.json")); // Success
        process_a_file(strcat(path, "AES_CTR_47243.json")); // Success
        process_a_file(strcat(path, "AES_CBC_47242.json")); // Success
        process_a_file(strcat(path, "AES_CCM_47247.json")); // Success
        process_a_file(strcat(path, "AES_CMAC_47249.json")); // Success
        process_a_file(strcat(path, "AES_CTR_47243.json")); // Success
        process_a_file(strcat(path, "AES_ECB_47241.json")); // Success
        process_a_file(strcat(path, "AES_GCM_external_8_2_1_47263.json")); // Success
        process_a_file(strcat(path, "AES_GCM_external_8_2_2_47265.json")); // Success
        process_a_file(strcat(path, "AES_GCM_internal_8_2_1_47267.json")); // Success
        process_a_file(strcat(path, "AES_GCM_internal_8_2_2_47269.json")); // Success
        process_a_file(strcat(path, "AES_GMAC_external_8_2_1_47271.json")); // Success
        process_a_file(strcat(path, "AES_GMAC_external_8_2_2_47273.json")); // Success
        process_a_file(strcat(path, "AES_GMAC_internal_8_2_1_47275.json")); // Success
        process_a_file(strcat(path, "AES_GMAC_internal_8_2_2_47277.json")); // Success
        process_a_file(strcat(path, "ECDSA_keyGen_47255.json")); // Success
        process_a_file(strcat(path, "ECDSA_keyVer_47256.json")); // Success
        process_a_file(strcat(path, "ECDSA_sigGen_47257.json")); // Success
        process_a_file(strcat(path, "HMAC_SHA1_47279.json")); // Success
        process_a_file(strcat(path, "HMAC_SHA2_224_47280.json")); // Success
        process_a_file(strcat(path, "HMAC_SHA2_256_47281.json")); // Success
        process_a_file(strcat(path, "HMAC_SHA2_384_47282.json")); // Success
        process_a_file(strcat(path, "HMAC_SHA2_512_47283.json")); // Success
        process_a_file(strcat(path, "HMAC_SHA3_224_47284.json")); // Success
        process_a_file(strcat(path, "HMAC_SHA3_256_47285.json")); // Success
        process_a_file(strcat(path, "HMAC_SHA3_384_47286.json")); // Success
        process_a_file(strcat(path, "HMAC_SHA3_512_47287.json")); // Success
        process_a_file(strcat(path, "KAS_ECC_47299.json")); // Success
        process_a_file(strcat(path, "KAS_ECC_CDH_Component_47297.json")); // Success
        process_a_file(strcat(path, "KAS_ECC_SSC_652956.json")); // Success
        process_a_file(strcat(path, "KAS_FFC_47301.json")); // Success
        process_a_file(strcat(path, "KAS_FFC_SSC_652957.json")); // Success
        process_a_file(strcat(path, "RSA_keyGen_47303.json")); // Success
        process_a_file(strcat(path, "RSA_sigGen_47304.json")); // Success
        process_a_file(strcat(path, "RSA_sigVer_47305.json")); // Success
        process_a_file(strcat(path, "SHA1_47311.json")); // Success
        process_a_file(strcat(path, "SHA2_224_47312.json")); // Success
        process_a_file(strcat(path, "SHA2_256_47313.json")); // Success
        process_a_file(strcat(path, "SHA2_384_47314.json")); // Success
        process_a_file(strcat(path, "SHA2_512_47315.json")); // Success
        process_a_file(strcat(path, "SHA3_224_47321.json")); // Success
        process_a_file(strcat(path, "SHA3_256_47322.json")); // Success
        process_a_file(strcat(path, "SHA3_384_47323.json")); // Success
        process_a_file(strcat(path, "SHA3_512_47324.json")); // Success
#endif

    #endif /* HAVE_FIPS */
#endif /* Harness if */

    #if 1 /* optest */
    #ifdef HAVE_FIPS
#define VALUES_LEN 15
        int i;

        printf("Kicking off the operational test app\n"); fflush(stdout);
        {
            int values[VALUES_LEN] = { 0, -203, -204, -205, -206, -207, -208, -209, -210, -242, -256, -257,
                                       -258, -259, 0 };
            for (i = 0; i < VALUES_LEN; i++) {
                ret = op_test(values[i], 0);
                printf("ret from running case %d = %d\n", values[i], ret); fflush(stdout);
            }
        }
    #endif /* HAVE_FIPS */
    #endif /* optest if */
    } else {
        printf("Skipping test until hash is updated\n"); fflush(stdout);
    }
    wolfCrypt_Cleanup();
    return 0;
}

