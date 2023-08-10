/* benchmark_main.c
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


#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/wolfcrypt/wc_port.h>
#include <wolfcrypt/benchmark/benchmark.h>
#include <stdio.h>
#include "hw.h" /* Needed for 'delay_us' prototype */

typedef struct func_args {
    int    argc;
    char** argv;
    int    return_code;
} func_args;

static func_args args = { 0 } ;

extern double current_time(int reset);

void main(void)
{
    int test_num = 0;

    wolfCrypt_Init(); /* required for ksdk_port_init */
    do
    {
        /* Used for testing, must have a delay so no data is missed while serial is initializing */
        #ifdef WOLFSSL_FRDM_K64_JENKINS
            /* run once */
            if(test_num == 1){
                printf("\n&&&&&&&&&&&&& done &&&&&&&&&&&&&&&");
                delay_us(1000000);
                break;
            }
            delay_us(1000000); /* 1 second */
        #endif


        printf("\nBenchmark Test %d:\n", test_num);
        benchmark_test(&args);
        printf("Benchmark Test %d: Return code %d\n", test_num, args.return_code);

        test_num++;
    } while(args.return_code == 0);

    /*Print this again for redundancy*/
    #ifdef WOLFSSL_FRDM_K64_JENKINS
        printf("\n&&&&&&&&&&&&&& done &&&&&&&&&&&&&\n");
        delay_us(1000000);
    #endif

    wolfCrypt_Cleanup();
}

/*
SAMPLE OUTPUT: Freescale K64 running at 96MHz with no MMCAU:
Benchmark Test 0:
AES      25 kB took 0.073 seconds,    0.334 MB/s
ARC4     25 kB took 0.033 seconds,    0.740 MB/s
RABBIT   25 kB took 0.027 seconds,    0.904 MB/s
3DES     25 kB took 0.375 seconds,    0.065 MB/s
MD5      25 kB took 0.016 seconds,    1.526 MB/s
SHA      25 kB took 0.044 seconds,    0.555 MB/s
SHA-256  25 kB took 0.119 seconds,    0.205 MB/s
RSA 1024 encryption took 91.000 milliseconds, avg over 1 iterations
RSA 1024 decryption took 573.000 milliseconds, avg over 1 iterations
DH  1024 key generation  253.000 milliseconds, avg over 1 iterations
DH  1024 key agreement   311.000 milliseconds, avg over 1 iterations
Benchmark Test 0: Return code 0

SAMPLE OUTPUT: Freescale K64 running at 96MHz with MMCAU enabled:
Benchmark Test 0:
AES      25 kB took 0.019 seconds,    1.285 MB/s
ARC4     25 kB took 0.033 seconds,    0.740 MB/s
RABBIT   25 kB took 0.028 seconds,    0.872 MB/s
3DES     25 kB took 0.026 seconds,    0.939 MB/s
MD5      25 kB took 0.005 seconds,    4.883 MB/s
SHA      25 kB took 0.008 seconds,    3.052 MB/s
SHA-256  25 kB took 0.013 seconds,    1.878 MB/s
RSA 1024 encryption took 89.000 milliseconds, avg over 1 iterations
RSA 1024 decryption took 573.000 milliseconds, avg over 1 iterations
DH  1024 key generation  250.000 milliseconds, avg over 1 iterations
DH  1024 key agreement   308.000 milliseconds, avg over 1 iterations
Benchmark Test 0: Return code 0


SAMPLE OUTPUT: NXP K82 running at 150Mhz w/MMCAU and LTC
Benchmark Test 0:
RNG      25 kB took 0.026 seconds,    0.939 MB/s
AES enc  25 kB took 0.002 seconds,   12.207 MB/s
AES dec  25 kB took 0.002 seconds,   12.207 MB/s
AES-GCM  25 kB took 0.002 seconds,   12.207 MB/s
AES-CTR  25 kB took 0.003 seconds,    8.138 MB/s
AES-CCM  25 kB took 0.004 seconds,    6.104 MB/s
CHACHA   25 kB took 0.008 seconds,    3.052 MB/s
CHA-POLY 25 kB took 0.013 seconds,    1.878 MB/s

POLY1305 25 kB took 0.003 seconds,    8.138 MB/s
SHA      25 kB took 0.006 seconds,    4.069 MB/s
SHA-256  25 kB took 0.009 seconds,    2.713 MB/s
SHA-384  25 kB took 0.032 seconds,    0.763 MB/s
SHA-512  25 kB took 0.035 seconds,    0.698 MB/s

RSA 2048 public          12.000 milliseconds, avg over 1 iterations
RSA 2048 private         135.000 milliseconds, avg over 1 iterations

ECC  256 key generation  17.400 milliseconds, avg over 5 iterations
EC-DHE   key agreement   15.200 milliseconds, avg over 5 iterations
EC-DSA   sign   time     20.200 milliseconds, avg over 5 iterations
EC-DSA   verify time     33.000 milliseconds, avg over 5 iterations

CURVE25519 256 key generation 14.400 milliseconds, avg over 5 iterations
CURVE25519 key agreement      14.400 milliseconds, avg over 5 iterations

ED25519  key generation  14.800 milliseconds, avg over 5 iterations
ED25519  sign   time     16.800 milliseconds, avg over 5 iterations
ED25519  verify time     30.400 milliseconds, avg over 5 iterations
Benchmark Test 0: Return code 0

SAMPLE OUTPUT: NXP K82 running at 150Mhz software only
Benchmark Test 0:
RNG      25 kB took 0.035 seconds,    0.698 MB/s
AES enc  25 kB took 0.038 seconds,    0.642 MB/s
AES dec  25 kB took 0.036 seconds,    0.678 MB/s
AES-GCM  25 kB took 0.485 seconds,    0.050 MB/s
AES-CTR  25 kB took 0.038 seconds,    0.642 MB/s
AES-CCM  25 kB took 0.077 seconds,    0.317 MB/s
CHACHA   25 kB took 0.009 seconds,    2.713 MB/s
CHA-POLY 25 kB took 0.013 seconds,    1.878 MB/s

POLY1305 25 kB took 0.003 seconds,    8.138 MB/s
SHA      25 kB took 0.006 seconds,    4.069 MB/s
SHA-256  25 kB took 0.014 seconds,    1.744 MB/s
SHA-384  25 kB took 0.032 seconds,    0.763 MB/s
SHA-512  25 kB took 0.034 seconds,    0.718 MB/s

RSA 1024 encryption took 18.000 milliseconds, avg over 1 iterations
RSA 1024 decryption took 123.000 milliseconds, avg over 1 iterations
RSA 2048 encryption took 63.000 milliseconds, avg over 1 iterations
RSA 2048 decryption took 1011.000 milliseconds, avg over 1 iterations

ECC  256 key generation  180.800 milliseconds, avg over 5 iterations
EC-DHE   key agreement   178.600 milliseconds, avg over 5 iterations
EC-DSA   sign   time     184.600 milliseconds, avg over 5 iterations
EC-DSA   verify time     130.200 milliseconds, avg over 5 iterations

CURVE25519 256 key generation 41.800 milliseconds, avg over 5 iterations
CURVE25519 key agreement      41.600 milliseconds, avg over 5 iterations

ED25519  key generation  14.800 milliseconds, avg over 5 iterations
ED25519  sign   time     16.600 milliseconds, avg over 5 iterations
ED25519  verify time     48.000 milliseconds, avg over 5 iterations
Benchmark Test 0: Return code 0
*/
