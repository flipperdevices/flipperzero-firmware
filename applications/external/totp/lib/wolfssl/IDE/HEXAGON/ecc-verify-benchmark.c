/* ecc-verify-benchmark.c
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

#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/wolfcrypt/sha256.h>
#include <wolfssl/wolfcrypt/random.h>
#include <wolfssl/wolfcrypt/ecc.h>
#include <wolfssl/wolfcrypt/asn_public.h>
#include <pthread.h>

#define USE_CERT_BUFFERS_256
#include <wolfssl/certs_test.h>

#define MAX_TIMES 5000
#define MAX_BLOCK_SIZE 1024

#include <sys/time.h>                                                       

static double get_time()                                              
{                                                                           
    struct timeval tv;                                                      
    gettimeofday(&tv, 0);                                                   
    return (double)tv.tv_sec + (double)tv.tv_usec / 1000000;                
} 


/* software version */
void* hash_firmware_verify(void* key)
{
    int ret, i;
    int verify;
    const byte hash[] = {
0XFB, 0XBA, 0XB2, 0X89, 0XF7, 0XF9, 0X4B, 0X25, 0X73, 0X6C, 0X58, 0XBE, 0X46, 0XA9, 0X94, 0XC4, 0X41, 0XFD, 0X02, 0X55, 0X2C, 0XC6, 0X02, 0X23, 0X52, 0XE3, 0XD8, 0X6D, 0X2F, 0XAB, 0X7C, 0X83
};
    const byte sigBuf[] = {
0X30, 0X44, 0X02, 0X20, 0X05, 0X38, 0XBC, 0X16, 0XC7, 0X67, 0X18, 0XEC, 0XE6, 0X1E, 0X43, 0X7B, 0X29, 0X8F, 0X85, 0X01, 0X33, 0XA8, 0X9B, 0XDD, 0X91, 0X32, 0X1F, 0XEC, 0XF7, 0X91, 0X18, 0X72, 0X9C, 0XE2, 0X6F, 0X31, 0X02, 0X20, 0X3E, 0X31, 0XD6, 0X40, 0XF7, 0X38, 0X3C, 0X1B, 0X6D, 0XAD, 0XE3, 0X93, 0X20, 0XE8, 0XB1, 0XBD, 0X3C, 0X59, 0XF2, 0XD2, 0X7C, 0X46, 0X1B, 0XE5, 0XE1, 0XE3, 0XAB, 0X5E, 0X76, 0X73, 0X6F, 0XFB
    };
    word32 sigLen = (word32)sizeof(sigBuf);
    word32 hashLen = (word32)sizeof(hash);

    for (i = 0; i < MAX_TIMES; i++) {
        ret = wc_ecc_verify_hash((byte*)sigBuf, sigLen, hash, hashLen, &verify, (ecc_key*)key);
        if (ret < 0 || verify != 1) {
            printf("failed on try %d\n", i);
            break;
        }
    }

    if (ret < 0 || verify != 1) {
	printf("unable to verify, ret = %d verify = %d\n", ret, verify);
    }

    return NULL;
}


/* when flag is set then try to use software only if DSP is built in */
static int hash_firmware_verify_default(int numThreads)
{
    int ret, i;
    word32 idx;
    double t;
    pthread_t threads[numThreads];
    ecc_key eccKey[numThreads];

    for (i = 0; i < numThreads; i++) {
        wc_ecc_init(&(eccKey[i]));
        idx = 0;
        ret = wc_EccPrivateKeyDecode(ecc_clikey_der_256, &idx, &(eccKey[i]), sizeof_ecc_clikey_der_256);
        if (ret < 0)
            return ret;

    }

    t = get_time();
    for (i = 0; i < numThreads; i++) {
        pthread_create(&threads[i], NULL, hash_firmware_verify, (void*)&(eccKey[i]));
    }

    for (i = 0; i < numThreads; i++) {
        pthread_join(threads[i], NULL);
    }
    t = get_time() - t;
    printf("%d verifies on %d threads took %f seconds\n", MAX_TIMES * numThreads, numThreads, t);

    return 0;
}

#ifdef WOLFSSL_DSP
/* domain 0 = cDSP 1 = aDSP */
static int hash_firmware_verify_dsp(int numThreads, int domain)
{
    int ret, i;
    word32 idx;
    double t;
    remote_handle64 handle[numThreads];
    char *sp_URI_value;
    pthread_t threads[numThreads];
    ecc_key eccKey[numThreads];

    if (domain == 0) {
        sp_URI_value = wolfSSL_URI "&_dom=cdsp";
    }
    else {
        sp_URI_value = wolfSSL_URI "&_dom=adsp";
    }

    for (i = 0; i < numThreads; i++) {
        wc_ecc_init(&(eccKey[i]));
        idx = 0;
        ret = wc_EccPrivateKeyDecode(ecc_clikey_der_256, &idx, &(eccKey[i]), sizeof_ecc_clikey_der_256);
        if (ret < 0)
            return ret;

        ret = wolfSSL_open(sp_URI_value, &(handle[i]));
        if (ret != 0) {
            printf("unable to open CDSP? retVal = %d\n", ret);
            return -1;
        }
        wc_ecc_set_handle(&(eccKey[i]), handle[i]);
    }

    t = get_time();
    for (i = 0; i < numThreads; i++) {
        pthread_create(&threads[i], NULL, hash_firmware_verify, (void*)&(eccKey[i]));
    }

    for (i = 0; i < numThreads; i++) {
        pthread_join(threads[i], NULL);
        wolfSSL_close(handle[i]);
    }
    t = get_time() - t;
    printf("%d verifies on %d threads took %f seconds\n", MAX_TIMES * numThreads, numThreads, t);

    return 0;
}
#endif /* WOLFSSL_DSP */


int main(int argc, char* argv[])
{
    wolfCrypt_Init();

    printf("benchmarking using default (locks on handle for aDSP)\n");
    hash_firmware_verify_default(1);
    hash_firmware_verify_default(2);

    printf("\nbenchmarking using software (+NEON if built in)\n");
#ifdef WOLFSSL_DSP
    wolfSSL_SetHandleCb(NULL); /* remove calls to DSP by default */
#endif
    hash_firmware_verify_default(1);
    hash_firmware_verify_default(2);

#ifdef WOLFSSL_DSP
    printf("\nbenchmarking using threads on aDSP\n");
    hash_firmware_verify_dsp(1, 1);
    hash_firmware_verify_dsp(2, 1);
    hash_firmware_verify_dsp(3, 1);
    hash_firmware_verify_dsp(4, 1);

    printf("\nbenchmarking 1 thread on cDSP\n");
    hash_firmware_verify_dsp(1, 0);
#endif /* WOLFSSL_DSP */
    wolfCrypt_Cleanup();
    return 0;
}
