/* renesas-rx64-hw-crypt.h
 *
 * Contributed by Johnson Controls Tyco IP Holdings LLP.
 *
 * Use of this Software is subject to the GPLv2 License
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1335, USA
 */
#ifndef __RENESAS_RX64_HW_CRYPT_H__
#define __RENESAS_RX64_HW_CRYPT_H__

#include <renesas/security/sha/r_sha.h>
#include <wolfssl/wolfcrypt/logging.h>

#ifdef __cplusplus
extern "C" {
#endif

int rx64_hw_Open(void);
void rx64_hw_Close(void);
int rx64_hw_lock(void);
void rx64_hw_unlock(void);

#if (!defined(NO_SHA) || defined(WOLFSSL_SHA224) || !defined(NO_SHA256))

typedef enum
{
    RX64_SHA1 = 0,
    RX64_SHA224 = 1,
    RX64_SHA256 = 2,
    NUM_RX64_SHA_TYPES = 3,
} RX64_SHA_TYPE;

typedef struct
{
    byte*  msg;
    void*  heap;
    word32 used;
    word32 len;
    word32 sha_type;
} wolfssl_RX64_HW_Hash;

#if !defined(NO_SHA)
    typedef wolfssl_RX64_HW_Hash wc_Sha;
#endif

#if !defined(NO_SHA256)
    typedef wolfssl_RX64_HW_Hash wc_Sha256;
#endif

#if defined(WOLFSSL_SHA224)
    typedef wolfssl_RX64_HW_Hash wc_Sha224;
    #define WC_SHA224_TYPE_DEFINED
#endif

WOLFSSL_LOCAL int RX64_ShaCalc(byte* data, word32 len, byte* out, word32 sha_type);

#endif /* !NO_SHA || WOLFSSL_SHA224 || !NO_SHA256 */

#ifdef __cplusplus
}
#endif

#endif  /* __RENESAS_RX64_HW_CRYPT_H__ */
