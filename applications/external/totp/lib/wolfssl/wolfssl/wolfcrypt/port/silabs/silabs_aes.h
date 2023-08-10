/* silabs_aes.h
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

#ifndef _SILABS_AES_H_
#define _SILABS_AES_H_

#include <em_device.h>

#if defined(WOLFSSL_SILABS_SE_ACCEL)

#include <wolfssl/wolfcrypt/types.h>

#include <sl_se_manager.h>
#include <sl_se_manager_cipher.h>

typedef struct {
  sl_se_command_context_t cmd_ctx;
  sl_se_key_descriptor_t  key;
} silabs_aes_t;

typedef struct Aes Aes;

#ifdef HAVE_AESGCM
int wc_AesGcmEncrypt_silabs (Aes* aes, byte* out, const byte* in, word32 sz,
                             const byte* iv, word32 ivSz,
                             byte* authTag, word32 authTagSz,
                             const byte* authIn, word32 authInSz);
int wc_AesGcmDecrypt_silabs (Aes* aes, byte* out, const byte* in, word32 sz,
                             const byte* iv, word32 ivSz,
                             const byte* authTag, word32 authTagSz,
                             const byte* authIn, word32 authInSz);

#endif /* HAVE_AESGCM */

#ifdef HAVE_AESCCM
int wc_AesCcmEncrypt_silabs (Aes* aes, byte* out, const byte* in, word32 sz,
                             const byte* iv, word32 ivSz,
                             byte* authTag, word32 authTagSz,
                             const byte* authIn, word32 authInSz);
int wc_AesCcmDecrypt_silabs (Aes* aes, byte* out, const byte* in, word32 sz,
                             const byte* iv, word32 ivSz,
                             const byte* authTag, word32 authTagSz,
                             const byte* authIn, word32 authInSz);

#endif /* HAVE_AESCCM */

#endif /* defined(WOLFSSL_SILABS_SE_ACCEL) */

#endif /* _SILABS_AES_H_ */
