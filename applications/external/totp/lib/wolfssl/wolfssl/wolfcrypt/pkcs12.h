/* pkcs12.h
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


#ifndef WOLF_CRYPT_PKCS12_H
#define WOLF_CRYPT_PKCS12_H

#include <wolfssl/wolfcrypt/types.h>

#ifdef __cplusplus
    extern "C" {
#endif

#ifndef WOLFSSL_TYPES_DEFINED /* do not redeclare from ssl.h */
    typedef struct WC_PKCS12 WC_PKCS12;
#endif

typedef struct WC_DerCertList { /* dereferenced in ssl.c */
    byte* buffer;
    word32 bufferSz;
    struct WC_DerCertList* next;
} WC_DerCertList;

/* default values for creating PKCS12 */
enum {
    WC_PKCS12_ITT_DEFAULT = 2048,
    WC_PKCS12_VERSION_DEFAULT = 3,
    WC_PKCS12_MAC_DEFAULT = 1
};

WOLFSSL_API WC_PKCS12* wc_PKCS12_new(void);
WOLFSSL_API void wc_PKCS12_free(WC_PKCS12* pkcs12);
WOLFSSL_API int wc_d2i_PKCS12(const byte* der, word32 derSz, WC_PKCS12* pkcs12);
#ifndef NO_FILESYSTEM
WOLFSSL_API int wc_d2i_PKCS12_fp(const char* file, WC_PKCS12** pkcs12);
#endif
WOLFSSL_API int wc_i2d_PKCS12(WC_PKCS12* pkcs12, byte** der, int* derSz);
WOLFSSL_API int wc_PKCS12_parse(WC_PKCS12* pkcs12, const char* psw,
        byte** pkey, word32* pkeySz, byte** cert, word32* certSz,
        WC_DerCertList** ca);
WOLFSSL_LOCAL int wc_PKCS12_verify_ex(WC_PKCS12* pkcs12,
        const byte* psw, word32 pswSz);
WOLFSSL_API WC_PKCS12* wc_PKCS12_create(char* pass, word32 passSz,
        char* name, byte* key, word32 keySz, byte* cert, word32 certSz,
        WC_DerCertList* ca, int nidKey, int nidCert, int iter, int macIter,
        int keyType, void* heap);


WOLFSSL_LOCAL int wc_PKCS12_SetHeap(WC_PKCS12* pkcs12, void* heap);
WOLFSSL_LOCAL void* wc_PKCS12_GetHeap(WC_PKCS12* pkcs12);

WOLFSSL_LOCAL void wc_FreeCertList(WC_DerCertList* list, void* heap);

#ifdef __cplusplus
    } /* extern "C" */
#endif

#endif /* WOLF_CRYPT_PKCS12_H */

