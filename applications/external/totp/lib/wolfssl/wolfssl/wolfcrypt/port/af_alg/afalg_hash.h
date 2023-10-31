/* afalg_hash.h
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


#ifndef WOLF_CRYPT_AFALG_HASH_H
#define WOLF_CRYPT_AFALG_HASH_H

#include <wolfssl/wolfcrypt/types.h>

#undef  WOLFSSL_NO_HASH_RAW
#define WOLFSSL_NO_HASH_RAW

typedef struct {
    byte*  msg;
    void*  heap;
    word32 used;
    word32 len;
    int    alFd;
    int    rdFd;
} wolfssl_AFALG_Hash;



#if !defined(NO_SHA256) && defined(WOLFSSL_AFALG_HASH)
    typedef wolfssl_AFALG_Hash wc_Sha256;
#endif
#if defined(WOLFSSL_SHA3) && defined(WOLFSSL_AFALG_XILINX_SHA3)
    typedef wolfssl_AFALG_Hash wc_Sha3;
#endif

#endif /* WOLF_CRYPT_AFALG_HASH_H  */

