/* silabs_ecc.h
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


#ifndef _SILABS_ECC_H_
#define _SILABS_ECC_H_


#if defined(WOLFSSL_SILABS_SE_ACCEL)

#include <wolfssl/wolfcrypt/types.h>

#include <sl_se_manager.h>
#include <sl_se_manager_defines.h>
#include <sl_se_manager_key_derivation.h>
#include <sl_se_manager_signature.h>

typedef enum ecc_curve_id ecc_curve_id;
typedef struct ecc_key ecc_key;

int silabs_ecc_sign_hash (const byte* in, word32 inlen,
                          byte* out, word32 *outlen,
                          ecc_key* key);
int silabs_ecc_verify_hash (const byte* sig, word32 siglen,
                            const byte* hash, word32 hashlen,
                            int* stat, ecc_key* key);


int silabs_ecc_make_key(ecc_key* key, int keysize);

int silabs_ecc_import(ecc_key* key, word32 keysize, int pub, int priv);
int silabs_ecc_export_public(ecc_key* key, sl_se_key_descriptor_t* seKey);

int silabs_ecc_shared_secret(ecc_key* private_key, ecc_key* public_key,
                             byte* out, word32* outlen);

#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
int silabs_ecc_load_vault(ecc_key* key);
#endif


#endif /* WOLFSSL_SILABS_SE_ACCEL */

#endif /* _SILABS_ECC_H_ */
