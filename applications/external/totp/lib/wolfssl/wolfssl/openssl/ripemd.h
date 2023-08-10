/* ripemd.h
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

/* ripemd.h for openssl */


#ifndef WOLFSSL_RIPEMD_H_
#define WOLFSSL_RIPEMD_H_

#include <wolfssl/wolfcrypt/settings.h>

#ifdef __cplusplus
    extern "C" {
#endif


typedef struct WOLFSSL_RIPEMD_CTX {
    int holder[32];   /* big enough to hold wolfcrypt, but check on init */
} WOLFSSL_RIPEMD_CTX;

WOLFSSL_API void wolfSSL_RIPEMD_Init(WOLFSSL_RIPEMD_CTX*);
WOLFSSL_API void wolfSSL_RIPEMD_Update(WOLFSSL_RIPEMD_CTX*, const void*,
                                     unsigned long);
WOLFSSL_API void wolfSSL_RIPEMD_Final(unsigned char*, WOLFSSL_RIPEMD_CTX*);


typedef WOLFSSL_RIPEMD_CTX RIPEMD_CTX;

#define RIPEMD_Init   wolfSSL_RIPEMD_Init
#define RIPEMD_Update wolfSSL_RIPEMD_Update
#define RIPEMD_Final  wolfSSL_RIPEMD_Final


#ifdef __cplusplus
    }  /* extern "C" */
#endif


#endif /* WOLFSSL_MD5_H_ */

