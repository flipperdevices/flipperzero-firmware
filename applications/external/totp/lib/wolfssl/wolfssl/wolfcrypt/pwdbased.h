/* pwdbased.h
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

/*!
    \file wolfssl/wolfcrypt/pwdbased.h
*/

#ifndef WOLF_CRYPT_PWDBASED_H
#define WOLF_CRYPT_PWDBASED_H

#include <wolfssl/wolfcrypt/types.h>

#ifndef NO_PWDBASED


#ifdef __cplusplus
    extern "C" {
#endif

/*
 * hashType renamed to typeH to avoid shadowing global declaration here:
 * wolfssl/wolfcrypt/asn.h line 173 in enum Oid_Types
 */
WOLFSSL_API int wc_PBKDF1_ex(byte* key, int keyLen, byte* iv, int ivLen,
                      const byte* passwd, int passwdLen,
                      const byte* salt, int saltLen, int iterations,
                      int hashType, void* heap);
WOLFSSL_API int wc_PBKDF1(byte* output, const byte* passwd, int pLen,
                      const byte* salt, int sLen, int iterations, int kLen,
                      int typeH);
WOLFSSL_API int wc_PBKDF2_ex(byte* output, const byte* passwd, int pLen,
                    const byte* salt, int sLen, int iterations, int kLen,
                    int typeH, void* heap, int devId);
WOLFSSL_API int wc_PBKDF2(byte* output, const byte* passwd, int pLen,
                      const byte* salt, int sLen, int iterations, int kLen,
                      int typeH);
WOLFSSL_API int wc_PKCS12_PBKDF(byte* output, const byte* passwd, int pLen,
                            const byte* salt, int sLen, int iterations,
                            int kLen, int typeH, int purpose);
WOLFSSL_API int wc_PKCS12_PBKDF_ex(byte* output, const byte* passwd,int passLen,
                       const byte* salt, int saltLen, int iterations, int kLen,
                       int hashType, int id, void* heap);

#ifdef HAVE_SCRYPT
WOLFSSL_API int wc_scrypt(byte* output, const byte* passwd, int passLen,
                          const byte* salt, int saltLen, int cost,
                          int blockSize, int parallel, int dkLen);
WOLFSSL_API int wc_scrypt_ex(byte* output, const byte* passwd, int passLen,
                             const byte* salt, int saltLen, word32 iterations,
                             int blockSize, int parallel, int dkLen);
#endif


#ifdef __cplusplus
    } /* extern "C" */
#endif

#endif /* NO_PWDBASED */
#endif /* WOLF_CRYPT_PWDBASED_H */
