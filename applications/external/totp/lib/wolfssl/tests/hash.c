/* hash.c has unit tests
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

#include <stdio.h>

#include <wolfssl/wolfcrypt/md4.h>
#include <wolfssl/wolfcrypt/md5.h>
#include <wolfssl/wolfcrypt/sha.h>
#include <wolfssl/wolfcrypt/sha256.h>
#include <wolfssl/wolfcrypt/sha512.h>
#include <wolfssl/wolfcrypt/ripemd.h>
#include <wolfssl/wolfcrypt/hmac.h>

#include <tests/unit.h>

typedef struct testVector {
    const char*  input;
    const char*  output;
    size_t inLen;
    size_t outLen;
} testVector;

int  md4_test(void);
int  md5_test(void);
int  sha_test(void);
int  sha224_test(void);
int  sha256_test(void);
int  sha512_test(void);
int  sha384_test(void);
int  ripemd_test(void);
int  hmac_md5_test(void);
int  hmac_sha_test(void);
int  hmac_sha224_test(void);
int  hmac_sha256_test(void);
int  hmac_sha384_test(void);

int HashTest(void)
{
    int ret = 0;

    printf(" Begin HASH Tests\n");

#ifndef NO_MD4
    if ( (ret = md4_test()) ) {
        printf( "   MD4      test failed!\n");
        return ret;
    } else
        printf( "   MD4      test passed!\n");
#endif

#ifndef NO_MD5
    if ( (ret = md5_test()) ) {
        printf( "   MD5      test failed!\n");
        return ret;
    } else
        printf( "   MD5      test passed!\n");
#endif

#ifndef NO_SHA
    if ( (ret = sha_test()) ) {
        printf( "   SHA      test failed!\n");
        return ret;
    } else
        printf( "   SHA      test passed!\n");
#endif

#ifdef WOLFSSL_SHA224
    if ( (ret = sha224_test()) ) {
        printf( "   SHA-224  test failed!\n");
        return ret;
    } else
        printf( "   SHA-224  test passed!\n");
#endif

#ifndef NO_SHA256
    if ( (ret = sha256_test()) ) {
        printf( "   SHA-256  test failed!\n");
        return ret;
    } else
        printf( "   SHA-256  test passed!\n");
#endif

#ifdef WOLFSSL_SHA512
    if ( (ret = sha512_test()) ) {
        printf( "   SHA-512  test failed!\n");
        return ret;
    } else
        printf( "   SHA-512  test passed!\n");
#endif

#ifdef WOLFSSL_SHA384
    if ( (ret = sha384_test()) ) {
        printf( "   SHA-384  test failed!\n");
        return ret;
    } else
        printf( "   SHA-384  test passed!\n");
#endif

#ifdef WOLFSSL_RIPEMD
    if ( (ret = ripemd_test()) ) {
        printf( "   RIPEMD   test failed!\n");
        return ret;
    } else
        printf( "   RIPEMD   test passed!\n");
#endif

#ifndef NO_HMAC
    #if !defined(NO_MD5) && !(defined(HAVE_FIPS) && defined(HAVE_FIPS_VERSION) \
                              && (HAVE_FIPS_VERSION >= 5))
        if ( (ret = hmac_md5_test()) ) {
            printf( "   HMAC-MD5 test failed!\n");
            return ret;
        } else
            printf( "   HMAC-MD5 test passed!\n");
    #endif

    #ifndef NO_SHA
    if ( (ret = hmac_sha_test()) ) {
        printf( "   HMAC-SHA test failed!\n");
        return ret;
    } else
        printf( "   HMAC-SHA test passed!\n");
    #endif

    #ifdef WOLFSSL_SHA224
        if ( (ret = hmac_sha224_test()) ) {
            printf( "   HMAC-SHA224 test failed!\n");
            return ret;
        } else
            printf( "   HMAC-SHA224 test passed!\n");
    #endif

    #ifndef NO_SHA256
        if ( (ret = hmac_sha256_test()) ) {
            printf( "   HMAC-SHA256 test failed!\n");
            return ret;
        } else
            printf( "   HMAC-SHA256 test passed!\n");
    #endif

    #ifdef WOLFSSL_SHA384
        if ( (ret = hmac_sha384_test()) ) {
            printf( "   HMAC-SHA384 test failed!\n");
            return ret;
        } else
            printf( "   HMAC-SHA384 test passed!\n");
    #endif
#endif

    printf(" End HASH Tests\n");

    return 0;
}

#ifndef NO_MD4

int md4_test(void)
{
    Md4  md4;
    byte hash[MD4_DIGEST_SIZE];

    testVector a, b, c, d, e, f, g;
    testVector test_md4[7];
    int times = sizeof(test_md4) / sizeof(testVector), i;

    a.input  = "";
    a.output = "\x31\xd6\xcf\xe0\xd1\x6a\xe9\x31\xb7\x3c\x59\xd7\xe0\xc0\x89"
               "\xc0";
    a.inLen  = XSTRLEN(a.input);
    a.outLen = XSTRLEN(a.output);

    b.input  = "a";
    b.output = "\xbd\xe5\x2c\xb3\x1d\xe3\x3e\x46\x24\x5e\x05\xfb\xdb\xd6\xfb"
               "\x24";
    b.inLen  = XSTRLEN(b.input);
    b.outLen = XSTRLEN(b.output);

    c.input  = "abc";
    c.output = "\xa4\x48\x01\x7a\xaf\x21\xd8\x52\x5f\xc1\x0a\xe8\x7a\xa6\x72"
               "\x9d";
    c.inLen  = XSTRLEN(c.input);
    c.outLen = XSTRLEN(c.output);

    d.input  = "message digest";
    d.output = "\xd9\x13\x0a\x81\x64\x54\x9f\xe8\x18\x87\x48\x06\xe1\xc7\x01"
               "\x4b";
    d.inLen  = XSTRLEN(d.input);
    d.outLen = XSTRLEN(d.output);

    e.input  = "abcdefghijklmnopqrstuvwxyz";
    e.output = "\xd7\x9e\x1c\x30\x8a\xa5\xbb\xcd\xee\xa8\xed\x63\xdf\x41\x2d"
               "\xa9";
    e.inLen  = XSTRLEN(e.input);
    e.outLen = XSTRLEN(e.output);

    f.input  = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz012345"
               "6789";
    f.output = "\x04\x3f\x85\x82\xf2\x41\xdb\x35\x1c\xe6\x27\xe1\x53\xe7\xf0"
               "\xe4";
    f.inLen  = XSTRLEN(f.input);
    f.outLen = XSTRLEN(f.output);

    g.input  = "1234567890123456789012345678901234567890123456789012345678"
               "9012345678901234567890";
    g.output = "\xe3\x3b\x4d\xdc\x9c\x38\xf2\x19\x9c\x3e\x7b\x16\x4f\xcc\x05"
               "\x36";
    g.inLen  = XSTRLEN(g.input);
    g.outLen = XSTRLEN(g.output);

    test_md4[0] = a;
    test_md4[1] = b;
    test_md4[2] = c;
    test_md4[3] = d;
    test_md4[4] = e;
    test_md4[5] = f;
    test_md4[6] = g;

    wc_InitMd4(&md4);

    for (i = 0; i < times; ++i) {
        wc_Md4Update(&md4, (byte*)test_md4[i].input, (word32)test_md4[i].inLen);
        wc_Md4Final(&md4, hash);

        if (XMEMCMP(hash, test_md4[i].output, MD4_DIGEST_SIZE) != 0)
            return -205 - i;
    }

    return 0;
}

#endif /* NO_MD4 */

#ifndef NO_MD5

int md5_test(void)
{
    int ret;
    wc_Md5  md5;
    byte    hash[WC_MD5_DIGEST_SIZE];

    testVector a, b, c, d, e;
    testVector test_md5[5];
    int times = sizeof(test_md5) / sizeof(testVector), i;

    a.input  = "abc";
    a.output = "\x90\x01\x50\x98\x3c\xd2\x4f\xb0\xd6\x96\x3f\x7d\x28\xe1\x7f"
               "\x72";
    a.inLen  = XSTRLEN(a.input);
    a.outLen = XSTRLEN(a.output);

    b.input  = "message digest";
    b.output = "\xf9\x6b\x69\x7d\x7c\xb7\x93\x8d\x52\x5a\x2f\x31\xaa\xf1\x61"
               "\xd0";
    b.inLen  = XSTRLEN(b.input);
    b.outLen = XSTRLEN(b.output);

    c.input  = "abcdefghijklmnopqrstuvwxyz";
    c.output = "\xc3\xfc\xd3\xd7\x61\x92\xe4\x00\x7d\xfb\x49\x6c\xca\x67\xe1"
               "\x3b";
    c.inLen  = XSTRLEN(c.input);
    c.outLen = XSTRLEN(c.output);

    d.input  = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz012345"
               "6789";
    d.output = "\xd1\x74\xab\x98\xd2\x77\xd9\xf5\xa5\x61\x1c\x2c\x9f\x41\x9d"
               "\x9f";
    d.inLen  = XSTRLEN(d.input);
    d.outLen = XSTRLEN(d.output);

    e.input  = "1234567890123456789012345678901234567890123456789012345678"
               "9012345678901234567890";
    e.output = "\x57\xed\xf4\xa2\x2b\xe3\xc9\x55\xac\x49\xda\x2e\x21\x07\xb6"
               "\x7a";
    e.inLen  = XSTRLEN(e.input);
    e.outLen = XSTRLEN(e.output);

    test_md5[0] = a;
    test_md5[1] = b;
    test_md5[2] = c;
    test_md5[3] = d;
    test_md5[4] = e;

    ret = wc_InitMd5(&md5);
    if (ret) {
        return ret;
    }

    for (i = 0; i < times; ++i) {
        ret = wc_Md5Update(&md5, (byte*)test_md5[i].input,
                                                (word32)test_md5[i].inLen);
        if (ret) {
            return ret;
        }

        ret = wc_Md5Final(&md5, hash);
        if (ret) {
            return ret;
        }

        if (XMEMCMP(hash, test_md5[i].output, WC_MD5_DIGEST_SIZE) != 0)
            return -5 - i;
    }
    wc_Md5Free(&md5);

    return 0;
}

#endif /* NO_MD5 */

#ifndef NO_SHA
int sha_test(void)
{
    wc_Sha sha;
    byte   hash[WC_SHA_DIGEST_SIZE];

    testVector a, b, c, d;
    testVector test_sha[4];
    int ret   = 0;
    int times = sizeof(test_sha) / sizeof(struct testVector), i;

    a.input  = "abc";
    a.output = "\xA9\x99\x3E\x36\x47\x06\x81\x6A\xBA\x3E\x25\x71\x78\x50\xC2"
               "\x6C\x9C\xD0\xD8\x9D";
    a.inLen  = XSTRLEN(a.input);
    a.outLen = XSTRLEN(a.output);

    b.input  = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
    b.output = "\x84\x98\x3E\x44\x1C\x3B\xD2\x6E\xBA\xAE\x4A\xA1\xF9\x51\x29"
               "\xE5\xE5\x46\x70\xF1";
    b.inLen  = XSTRLEN(b.input);
    b.outLen = XSTRLEN(b.output);

    c.input  = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaa";
    c.output = "\x00\x98\xBA\x82\x4B\x5C\x16\x42\x7B\xD7\xA1\x12\x2A\x5A\x44"
               "\x2A\x25\xEC\x64\x4D";
    c.inLen  = XSTRLEN(c.input);
    c.outLen = XSTRLEN(c.output);

    d.input  = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaa";
    d.output = "\xAD\x5B\x3F\xDB\xCB\x52\x67\x78\xC2\x83\x9D\x2F\x15\x1E\xA7"
               "\x53\x99\x5E\x26\xA0";
    d.inLen  = XSTRLEN(d.input);
    d.outLen = XSTRLEN(d.output);

    test_sha[0] = a;
    test_sha[1] = b;
    test_sha[2] = c;
    test_sha[3] = d;

    ret = wc_InitSha(&sha);
    if (ret != 0)
        return ret;

    for (i = 0; i < times; ++i) {
        wc_ShaUpdate(&sha, (byte*)test_sha[i].input, (word32)test_sha[i].inLen);
        wc_ShaFinal(&sha, hash);

        if (XMEMCMP(hash, test_sha[i].output, WC_SHA_DIGEST_SIZE) != 0)
            return -10 - i;
    }
    wc_ShaFree(&sha);

    return 0;
}
#endif /* NO_SHA */

#ifdef WOLFSSL_SHA224
int sha224_test(void)
{
    wc_Sha224 sha;
    byte      hash[WC_SHA224_DIGEST_SIZE];

    testVector a, b;
    testVector test_sha[2];
    int ret;
    int times = sizeof(test_sha) / sizeof(struct testVector), i;

    a.input  = "abc";
    a.output = "\x23\x09\x7d\x22\x34\x05\xd8\x22\x86\x42\xa4\x77\xbd\xa2\x55"
               "\xb3\x2a\xad\xbc\xe4\xbd\xa0\xb3\xf7\xe3\x6c\x9d\xa7";
    a.inLen  = XSTRLEN(a.input);
    a.outLen = WC_SHA224_DIGEST_SIZE;

    b.input  = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
    b.output = "\x75\x38\x8b\x16\x51\x27\x76\xcc\x5d\xba\x5d\xa1\xfd\x89\x01"
               "\x50\xb0\xc6\x45\x5c\xb4\xf5\x8b\x19\x52\x52\x25\x25";
    b.inLen  = XSTRLEN(b.input);
    b.outLen = WC_SHA224_DIGEST_SIZE;

    test_sha[0] = a;
    test_sha[1] = b;

    ret = wc_InitSha224(&sha);
    if (ret != 0)
        return -4005;

    for (i = 0; i < times; ++i) {
        ret = wc_Sha224Update(&sha, (byte*)test_sha[i].input,(word32)test_sha[i].inLen);
        if (ret != 0)
            return ret;
        ret = wc_Sha224Final(&sha, hash);
        if (ret != 0)
            return ret;

        if (XMEMCMP(hash, test_sha[i].output, WC_SHA224_DIGEST_SIZE) != 0)
            return -10 - i;
    }
    wc_Sha224Free(&sha);

    return 0;
}
#endif

#ifndef NO_SHA256
int sha256_test(void)
{
    wc_Sha256 sha;
    byte      hash[WC_SHA256_DIGEST_SIZE];

    testVector a, b;
    testVector test_sha[2];
    int ret;
    int times = sizeof(test_sha) / sizeof(struct testVector), i;

    a.input  = "abc";
    a.output = "\xBA\x78\x16\xBF\x8F\x01\xCF\xEA\x41\x41\x40\xDE\x5D\xAE\x22"
               "\x23\xB0\x03\x61\xA3\x96\x17\x7A\x9C\xB4\x10\xFF\x61\xF2\x00"
               "\x15\xAD";
    a.inLen  = XSTRLEN(a.input);
    a.outLen = XSTRLEN(a.output);

    b.input  = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
    b.output = "\x24\x8D\x6A\x61\xD2\x06\x38\xB8\xE5\xC0\x26\x93\x0C\x3E\x60"
               "\x39\xA3\x3C\xE4\x59\x64\xFF\x21\x67\xF6\xEC\xED\xD4\x19\xDB"
               "\x06\xC1";
    b.inLen  = XSTRLEN(b.input);
    b.outLen = XSTRLEN(b.output);

    test_sha[0] = a;
    test_sha[1] = b;

    ret = wc_InitSha256(&sha);
    if (ret != 0)
        return ret;

    for (i = 0; i < times; ++i) {
        ret = wc_Sha256Update(&sha, (byte*)test_sha[i].input,(word32)test_sha[i].inLen);
        if (ret != 0)
            return ret;

        ret = wc_Sha256Final(&sha, hash);
        if (ret != 0)
            return ret;

        if (XMEMCMP(hash, test_sha[i].output, WC_SHA256_DIGEST_SIZE) != 0)
            return -10 - i;
    }
    wc_Sha256Free(&sha);

    return 0;
}
#endif

#ifdef WOLFSSL_SHA512
int sha512_test(void)
{
    wc_Sha512 sha;
    byte      hash[WC_SHA512_DIGEST_SIZE];

    testVector a, b;
    testVector test_sha[2];
    int times = sizeof(test_sha) / sizeof(struct testVector), i;
    int ret;

    a.input  = "abc";
    a.output = "\xdd\xaf\x35\xa1\x93\x61\x7a\xba\xcc\x41\x73\x49\xae\x20\x41"
               "\x31\x12\xe6\xfa\x4e\x89\xa9\x7e\xa2\x0a\x9e\xee\xe6\x4b\x55"
               "\xd3\x9a\x21\x92\x99\x2a\x27\x4f\xc1\xa8\x36\xba\x3c\x23\xa3"
               "\xfe\xeb\xbd\x45\x4d\x44\x23\x64\x3c\xe8\x0e\x2a\x9a\xc9\x4f"
               "\xa5\x4c\xa4\x9f";
    a.inLen  = XSTRLEN(a.input);
    a.outLen = XSTRLEN(a.output);

    b.input  = "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhi"
               "jklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu";
    b.output = "\x8e\x95\x9b\x75\xda\xe3\x13\xda\x8c\xf4\xf7\x28\x14\xfc\x14"
               "\x3f\x8f\x77\x79\xc6\xeb\x9f\x7f\xa1\x72\x99\xae\xad\xb6\x88"
               "\x90\x18\x50\x1d\x28\x9e\x49\x00\xf7\xe4\x33\x1b\x99\xde\xc4"
               "\xb5\x43\x3a\xc7\xd3\x29\xee\xb6\xdd\x26\x54\x5e\x96\xe5\x5b"
               "\x87\x4b\xe9\x09";
    b.inLen  = XSTRLEN(b.input);
    b.outLen = XSTRLEN(b.output);

    test_sha[0] = a;
    test_sha[1] = b;

    ret = wc_InitSha512(&sha);
    if (ret != 0)
        return ret;

    for (i = 0; i < times; ++i) {
        ret = wc_Sha512Update(&sha, (byte*)test_sha[i].input,(word32)test_sha[i].inLen);
        if (ret != 0)
            return ret;

        ret = wc_Sha512Final(&sha, hash);
        if (ret != 0)
            return ret;

        if (XMEMCMP(hash, test_sha[i].output, WC_SHA512_DIGEST_SIZE) != 0)
            return -10 - i;
    }
    wc_Sha512Free(&sha);

    return 0;
}
#endif

#ifdef WOLFSSL_SHA384
int sha384_test(void)
{
    wc_Sha384 sha;
    byte      hash[WC_SHA384_DIGEST_SIZE];

    testVector a, b;
    testVector test_sha[2];
    int times = sizeof(test_sha) / sizeof(struct testVector), i;
    int ret;

    a.input  = "abc";
    a.output = "\xcb\x00\x75\x3f\x45\xa3\x5e\x8b\xb5\xa0\x3d\x69\x9a\xc6\x50"
               "\x07\x27\x2c\x32\xab\x0e\xde\xd1\x63\x1a\x8b\x60\x5a\x43\xff"
               "\x5b\xed\x80\x86\x07\x2b\xa1\xe7\xcc\x23\x58\xba\xec\xa1\x34"
               "\xc8\x25\xa7";
    a.inLen  = XSTRLEN(a.input);
    a.outLen = XSTRLEN(a.output);

    b.input  = "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhi"
               "jklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu";
    b.output = "\x09\x33\x0c\x33\xf7\x11\x47\xe8\x3d\x19\x2f\xc7\x82\xcd\x1b"
               "\x47\x53\x11\x1b\x17\x3b\x3b\x05\xd2\x2f\xa0\x80\x86\xe3\xb0"
               "\xf7\x12\xfc\xc7\xc7\x1a\x55\x7e\x2d\xb9\x66\xc3\xe9\xfa\x91"
               "\x74\x60\x39";
    b.inLen  = XSTRLEN(b.input);
    b.outLen = XSTRLEN(b.output);

    test_sha[0] = a;
    test_sha[1] = b;

    ret = wc_InitSha384(&sha);
    if (ret != 0)
        return ret;

    for (i = 0; i < times; ++i) {
        ret = wc_Sha384Update(&sha, (byte*)test_sha[i].input,(word32)test_sha[i].inLen);
        if (ret != 0)
            return ret;

        ret = wc_Sha384Final(&sha, hash);
        if (ret != 0)
            return ret;

        if (XMEMCMP(hash, test_sha[i].output, WC_SHA384_DIGEST_SIZE) != 0)
            return -10 - i;
    }
    wc_Sha384Free(&sha);

    return 0;
}
#endif

#ifdef WOLFSSL_RIPEMD
int ripemd_test(void)
{
    RipeMd  ripemd;
    int ret;
    byte hash[RIPEMD_DIGEST_SIZE];

    testVector a, b, c, d;
    testVector test_ripemd[4];
    int times = sizeof(test_ripemd) / sizeof(struct testVector), i;

    a.input  = "abc";
    a.output = "\x8e\xb2\x08\xf7\xe0\x5d\x98\x7a\x9b\x04\x4a\x8e\x98\xc6"
               "\xb0\x87\xf1\x5a\x0b\xfc";
    a.inLen  = XSTRLEN(a.input);
    a.outLen = XSTRLEN(a.output);

    b.input  = "message digest";
    b.output = "\x5d\x06\x89\xef\x49\xd2\xfa\xe5\x72\xb8\x81\xb1\x23\xa8"
               "\x5f\xfa\x21\x59\x5f\x36";
    b.inLen  = XSTRLEN(b.input);
    b.outLen = XSTRLEN(b.output);

    c.input  = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
    c.output = "\x12\xa0\x53\x38\x4a\x9c\x0c\x88\xe4\x05\xa0\x6c\x27\xdc"
               "\xf4\x9a\xda\x62\xeb\x2b";
    c.inLen  = XSTRLEN(c.input);
    c.outLen = XSTRLEN(c.output);

    d.input  = "12345678901234567890123456789012345678901234567890123456"
               "789012345678901234567890";
    d.output = "\x9b\x75\x2e\x45\x57\x3d\x4b\x39\xf4\xdb\xd3\x32\x3c\xab"
               "\x82\xbf\x63\x32\x6b\xfb";
    d.inLen  = XSTRLEN(d.input);
    d.outLen = XSTRLEN(d.output);

    test_ripemd[0] = a;
    test_ripemd[1] = b;
    test_ripemd[2] = c;
    test_ripemd[3] = d;

    ret = wc_InitRipeMd(&ripemd);
    if (ret) {
        return ret;
    }

    for (i = 0; i < times; ++i) {
        ret = wc_RipeMdUpdate(&ripemd, (byte*)test_ripemd[i].input,
                             (word32)test_ripemd[i].inLen);
        if (ret) {
            return ret;
        }

        ret = wc_RipeMdFinal(&ripemd, hash);
        if (ret) {
            return ret;
        }

        if (XMEMCMP(hash, test_ripemd[i].output, RIPEMD_DIGEST_SIZE) != 0)
            return -10 - i;
    }

    return 0;
}
#endif /* WOLFSSL_RIPEMD */

#if !defined(NO_HMAC) && !defined(NO_MD5)
int hmac_md5_test(void)
{
    Hmac hmac;
    byte hash[WC_MD5_DIGEST_SIZE];

    const char* keys[]=
    {
        "\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b",
        "Jefe",
        "\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA"
    };

    testVector a, b, c;
    testVector test_hmac[3];

    int ret;
    int times = sizeof(test_hmac) / sizeof(testVector), i;

    a.input  = "Hi There";
    a.output = "\x92\x94\x72\x7a\x36\x38\xbb\x1c\x13\xf4\x8e\xf8\x15\x8b\xfc"
               "\x9d";
    a.inLen  = XSTRLEN(a.input);
    a.outLen = XSTRLEN(a.output);

    b.input  = "what do ya want for nothing?";
    b.output = "\x75\x0c\x78\x3e\x6a\xb0\xb5\x03\xea\xa8\x6e\x31\x0a\x5d\xb7"
               "\x38";
    b.inLen  = XSTRLEN(b.input);
    b.outLen = XSTRLEN(b.output);

    c.input  = "\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD"
               "\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD"
               "\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD"
               "\xDD\xDD\xDD\xDD\xDD\xDD";
    c.output = "\x56\xbe\x34\x52\x1d\x14\x4c\x88\xdb\xb8\xc7\x33\xf0\xe8\xb3"
               "\xf6";
    c.inLen  = XSTRLEN(c.input);
    c.outLen = XSTRLEN(c.output);

    test_hmac[0] = a;
    test_hmac[1] = b;
    test_hmac[2] = c;

    ret = wc_HmacInit(&hmac, NULL, INVALID_DEVID);
    if (ret != 0)
        return -20009;

    for (i = 0; i < times; ++i) {
#if defined(HAVE_FIPS)
        if (i == 1)
            continue; /* fips not allowed */
#endif
        ret = wc_HmacSetKey(&hmac, WC_MD5, (byte*)keys[i], (word32)XSTRLEN(keys[i]));
        if (ret != 0)
            return -4014;
        ret = wc_HmacUpdate(&hmac, (byte*)test_hmac[i].input,
                   (word32)test_hmac[i].inLen);
        if (ret != 0)
            return -4015;
        ret = wc_HmacFinal(&hmac, hash);
        if (ret != 0)
            return -4016;

        if (XMEMCMP(hash, test_hmac[i].output, WC_MD5_DIGEST_SIZE) != 0)
            return -20 - i;
    }

    wc_HmacFree(&hmac);

    return 0;
}
#endif

#if !defined(NO_HMAC) && !defined(NO_SHA)
int hmac_sha_test(void)
{
    Hmac hmac;
    byte hash[WC_SHA_DIGEST_SIZE];

    const char* keys[]=
    {
        "\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b"
                                                                "\x0b\x0b\x0b",
        "Jefe",
        "\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA"
                                                                "\xAA\xAA\xAA"
    };

    testVector a, b, c;
    testVector test_hmac[3];

    int ret;
    int times = sizeof(test_hmac) / sizeof(testVector), i;

    a.input  = "Hi There";
    a.output = "\xb6\x17\x31\x86\x55\x05\x72\x64\xe2\x8b\xc0\xb6\xfb\x37\x8c"
               "\x8e\xf1\x46\xbe\x00";
    a.inLen  = XSTRLEN(a.input);
    a.outLen = XSTRLEN(a.output);

    b.input  = "what do ya want for nothing?";
    b.output = "\xef\xfc\xdf\x6a\xe5\xeb\x2f\xa2\xd2\x74\x16\xd5\xf1\x84\xdf"
               "\x9c\x25\x9a\x7c\x79";
    b.inLen  = XSTRLEN(b.input);
    b.outLen = XSTRLEN(b.output);

    c.input  = "\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD"
               "\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD"
               "\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD"
               "\xDD\xDD\xDD\xDD\xDD\xDD";
    c.output = "\x12\x5d\x73\x42\xb9\xac\x11\xcd\x91\xa3\x9a\xf4\x8a\xa1\x7b"
               "\x4f\x63\xf1\x75\xd3";
    c.inLen  = XSTRLEN(c.input);
    c.outLen = XSTRLEN(c.output);

    test_hmac[0] = a;
    test_hmac[1] = b;
    test_hmac[2] = c;

    ret = wc_HmacInit(&hmac, NULL, INVALID_DEVID);
    if (ret != 0)
        return -20009;

    for (i = 0; i < times; ++i) {
#if defined(HAVE_FIPS)
        if (i == 1)
            continue; /* fips not allowed */
#endif
        ret = wc_HmacSetKey(&hmac, WC_SHA, (byte*)keys[i], (word32)XSTRLEN(keys[i]));
        if (ret != 0)
            return -4017;
        ret = wc_HmacUpdate(&hmac, (byte*)test_hmac[i].input,
                   (word32)test_hmac[i].inLen);
        if (ret != 0)
            return -4018;
        ret = wc_HmacFinal(&hmac, hash);
        if (ret != 0)
            return -4019;

        if (XMEMCMP(hash, test_hmac[i].output, WC_SHA_DIGEST_SIZE) != 0)
            return -20 - i;
    }

    wc_HmacFree(&hmac);

    return 0;
}
#endif

#if !defined(NO_HMAC) && defined(WOLFSSL_SHA224)
int hmac_sha224_test(void)
{
    Hmac hmac;
    byte hash[WC_SHA224_DIGEST_SIZE];

    const char* keys[]=
    {
        "\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b"
                                                                "\x0b\x0b\x0b",
        "Jefe",
        "\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA"
                                                                "\xAA\xAA\xAA"
    };

    testVector a, b, c;
    testVector test_hmac[3];

    int ret;
    int times = sizeof(test_hmac) / sizeof(testVector), i;

    a.input  = "Hi There";
    a.output = "\x89\x6f\xb1\x12\x8a\xbb\xdf\x19\x68\x32\x10\x7c\xd4\x9d\xf3"
               "\x3f\x47\xb4\xb1\x16\x99\x12\xba\x4f\x53\x68\x4b\x22";
    a.inLen  = XSTRLEN(a.input);
    a.outLen = WC_SHA224_DIGEST_SIZE;

    b.input  = "what do ya want for nothing?";
    b.output = "\xa3\x0e\x01\x09\x8b\xc6\xdb\xbf\x45\x69\x0f\x3a\x7e\x9e\x6d"
               "\x0f\x8b\xbe\xa2\xa3\x9e\x61\x48\x00\x8f\xd0\x5e\x44";
    b.inLen  = XSTRLEN(b.input);
    b.outLen = WC_SHA224_DIGEST_SIZE;

    c.input  = "\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD"
               "\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD"
               "\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD"
               "\xDD\xDD\xDD\xDD\xDD\xDD";
    c.output = "\x7f\xb3\xcb\x35\x88\xc6\xc1\xf6\xff\xa9\x69\x4d\x7d\x6a\xd2"
               "\x64\x93\x65\xb0\xc1\xf6\x5d\x69\xd1\xec\x83\x33\xea";
    c.inLen  = XSTRLEN(c.input);
    c.outLen = WC_SHA224_DIGEST_SIZE;

    test_hmac[0] = a;
    test_hmac[1] = b;
    test_hmac[2] = c;

    ret = wc_HmacInit(&hmac, NULL, INVALID_DEVID);
    if (ret != 0)
        return -20009;

    for (i = 0; i < times; ++i) {
#if defined(HAVE_FIPS) || defined(HAVE_CAVIUM)
        if (i == 1)
            continue; /* cavium can't handle short keys, fips not allowed */
#endif
        ret = wc_HmacSetKey(&hmac, WC_SHA224, (byte*)keys[i],(word32)XSTRLEN(keys[i]));
        if (ret != 0)
            return -4021;
        ret = wc_HmacUpdate(&hmac, (byte*)test_hmac[i].input,
                   (word32)test_hmac[i].inLen);
        if (ret != 0)
            return -4022;
        ret = wc_HmacFinal(&hmac, hash);
        if (ret != 0)
            return -4023;

        if (XMEMCMP(hash, test_hmac[i].output, WC_SHA224_DIGEST_SIZE) != 0)
            return -20 - i;
    }

    wc_HmacFree(&hmac);

    return 0;
}
#endif


#if !defined(NO_HMAC) && !defined(NO_SHA256)
int hmac_sha256_test(void)
{
    Hmac hmac;
    byte hash[WC_SHA256_DIGEST_SIZE];

    const char* keys[]=
    {
        "\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b"
                                                                "\x0b\x0b\x0b",
        "Jefe",
        "\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA"
                                                                "\xAA\xAA\xAA"
    };

    testVector a, b, c;
    testVector test_hmac[3];

    int ret;
    int times = sizeof(test_hmac) / sizeof(testVector), i;

    a.input  = "Hi There";
    a.output = "\xb0\x34\x4c\x61\xd8\xdb\x38\x53\x5c\xa8\xaf\xce\xaf\x0b\xf1"
               "\x2b\x88\x1d\xc2\x00\xc9\x83\x3d\xa7\x26\xe9\x37\x6c\x2e\x32"
               "\xcf\xf7";
    a.inLen  = XSTRLEN(a.input);
    a.outLen = XSTRLEN(a.output);

    b.input  = "what do ya want for nothing?";
    b.output = "\x5b\xdc\xc1\x46\xbf\x60\x75\x4e\x6a\x04\x24\x26\x08\x95\x75"
               "\xc7\x5a\x00\x3f\x08\x9d\x27\x39\x83\x9d\xec\x58\xb9\x64\xec"
               "\x38\x43";
    b.inLen  = XSTRLEN(b.input);
    b.outLen = XSTRLEN(b.output);

    c.input  = "\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD"
               "\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD"
               "\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD"
               "\xDD\xDD\xDD\xDD\xDD\xDD";
    c.output = "\x77\x3e\xa9\x1e\x36\x80\x0e\x46\x85\x4d\xb8\xeb\xd0\x91\x81"
               "\xa7\x29\x59\x09\x8b\x3e\xf8\xc1\x22\xd9\x63\x55\x14\xce\xd5"
               "\x65\xfe";
    c.inLen  = XSTRLEN(c.input);
    c.outLen = XSTRLEN(c.output);

    test_hmac[0] = a;
    test_hmac[1] = b;
    test_hmac[2] = c;

    ret = wc_HmacInit(&hmac, NULL, INVALID_DEVID);
    if (ret != 0)
        return -20009;

    for (i = 0; i < times; ++i) {
#if defined(HAVE_FIPS)
        if (i == 1)
            continue; /* fips not allowed */
#endif
        ret = wc_HmacSetKey(&hmac, WC_SHA256, (byte*)keys[i],
            (word32)XSTRLEN(keys[i]));
        if (ret != 0)
            return -4020;
        ret = wc_HmacUpdate(&hmac, (byte*)test_hmac[i].input,
                   (word32)test_hmac[i].inLen);
        if (ret != 0)
            return -4021;
        ret = wc_HmacFinal(&hmac, hash);
        if (ret != 0)
            return -4022;

        if (XMEMCMP(hash, test_hmac[i].output, WC_SHA256_DIGEST_SIZE) != 0)
            return -20 - i;
    }

    wc_HmacFree(&hmac);

    return 0;
}
#endif


#if !defined(NO_HMAC) && defined(WOLFSSL_SHA384)
int hmac_sha384_test(void)
{
    Hmac hmac;
    byte hash[WC_SHA384_DIGEST_SIZE];

    const char* keys[]=
    {
        "\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b"
                                                                "\x0b\x0b\x0b",
        "Jefe",
        "\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA"
                                                                "\xAA\xAA\xAA"
    };

    testVector a, b, c;
    testVector test_hmac[3];

    int ret;
    int times = sizeof(test_hmac) / sizeof(testVector), i;

    a.input  = "Hi There";
    a.output = "\xaf\xd0\x39\x44\xd8\x48\x95\x62\x6b\x08\x25\xf4\xab\x46\x90"
               "\x7f\x15\xf9\xda\xdb\xe4\x10\x1e\xc6\x82\xaa\x03\x4c\x7c\xeb"
               "\xc5\x9c\xfa\xea\x9e\xa9\x07\x6e\xde\x7f\x4a\xf1\x52\xe8\xb2"
               "\xfa\x9c\xb6";
    a.inLen  = XSTRLEN(a.input);
    a.outLen = XSTRLEN(a.output);

    b.input  = "what do ya want for nothing?";
    b.output = "\xaf\x45\xd2\xe3\x76\x48\x40\x31\x61\x7f\x78\xd2\xb5\x8a\x6b"
               "\x1b\x9c\x7e\xf4\x64\xf5\xa0\x1b\x47\xe4\x2e\xc3\x73\x63\x22"
               "\x44\x5e\x8e\x22\x40\xca\x5e\x69\xe2\xc7\x8b\x32\x39\xec\xfa"
               "\xb2\x16\x49";
    b.inLen  = XSTRLEN(b.input);
    b.outLen = XSTRLEN(b.output);

    c.input  = "\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD"
               "\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD"
               "\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD"
               "\xDD\xDD\xDD\xDD\xDD\xDD";
    c.output = "\x88\x06\x26\x08\xd3\xe6\xad\x8a\x0a\xa2\xac\xe0\x14\xc8\xa8"
               "\x6f\x0a\xa6\x35\xd9\x47\xac\x9f\xeb\xe8\x3e\xf4\xe5\x59\x66"
               "\x14\x4b\x2a\x5a\xb3\x9d\xc1\x38\x14\xb9\x4e\x3a\xb6\xe1\x01"
               "\xa3\x4f\x27";
    c.inLen  = XSTRLEN(c.input);
    c.outLen = XSTRLEN(c.output);

    test_hmac[0] = a;
    test_hmac[1] = b;
    test_hmac[2] = c;

    ret = wc_HmacInit(&hmac, NULL, INVALID_DEVID);
    if (ret != 0)
        return -20009;

    for (i = 0; i < times; ++i) {
#if defined(HAVE_FIPS)
        if (i == 1)
            continue; /* fips not allowed */
#endif
        ret = wc_HmacSetKey(&hmac, WC_SHA384, (byte*)keys[i],
            (word32)XSTRLEN(keys[i]));
        if (ret != 0)
            return -4023;
        ret = wc_HmacUpdate(&hmac, (byte*)test_hmac[i].input,
                   (word32)test_hmac[i].inLen);
        if (ret != 0)
            return -4024;
        ret = wc_HmacFinal(&hmac, hash);
        if (ret != 0)
            return -4025;

        if (XMEMCMP(hash, test_hmac[i].output, WC_SHA384_DIGEST_SIZE) != 0)
            return -20 - i;
    }

    wc_HmacFree(&hmac);

    return 0;
}
#endif

