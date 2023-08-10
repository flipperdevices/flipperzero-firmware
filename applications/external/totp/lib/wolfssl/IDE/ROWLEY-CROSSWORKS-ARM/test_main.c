/* test_main.c
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
#include <wolfcrypt/test/test.h>
#include <stdio.h>
#include "hw.h"

typedef struct func_args {
    int    argc;
    char** argv;
    int    return_code;
} func_args;

static func_args args = { 0 } ;


void main(void)
{
    int test_num = 0;

    wolfCrypt_Init(); /* required for ksdk_port_init */
    do
    {
        /* Used for testing, must have a delay so no data is missed while serial is initializing */
        #ifdef WOLFSSL_FRDM_K64_JENKINS
            /* run twice */
            if(test_num == 2){
                printf("\n&&&&&&&&&&&&& done &&&&&&&&&&&&&&&");
                delay_us(1000000);
                break;
            }
            delay_us(1000000); /* 1 second */
        #endif

        printf("\nCrypt Test %d:\n", test_num);
        wolfcrypt_test(&args);
        printf("Crypt Test %d: Return code %d\n", test_num, args.return_code);

        test_num++;
    } while(args.return_code == 0);

    /* Print this again for redundancy */
    #ifdef WOLFSSL_FRDM_K64_JENKINS
        printf("\n&&&&&&&&&&&&&& done &&&&&&&&&&&&&\n");
        delay_us(1000000);
    #endif

    wolfCrypt_Cleanup();
}


/* SAMPLE OUTPUT:
Crypt Test 0:
SHA      test passed!
SHA-256  test passed!
SHA-384  test passed!
SHA-512  test passed!
HMAC-SHA test passed!
HMAC-SHA256 test passed!
HMAC-SHA384 test passed!
HMAC-SHA512 test passed!
GMAC     test passed!
Chacha   test passed!
POLY1305 test passed!
ChaCha20-Poly1305 AEAD test passed!
AES      test passed!
AES-GCM  test passed!
AES-CCM  test passed!
RANDOM   test passed!
RSA      test passed!
ECC      test passed!
CURVE25519 test passed!
ED25519  test passed!
Crypt Test 0: Return code 0
*/
