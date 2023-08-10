/* key_data.h
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





#ifndef KEY_DATA_H_
#define KEY_DATA_H_

#include <wolfssl/wolfcrypt/settings.h>


#if defined(WOLFSSL_RENESAS_TSIP)

#include "r_tsip_rx_if.h"

typedef struct st_key_block_data
{
    /* encrypted provisioning key */
    uint8_t encrypted_provisioning_key[R_TSIP_AES_CBC_IV_BYTE_SIZE * 2];
    /* iv */
    uint8_t iv[R_TSIP_AES_CBC_IV_BYTE_SIZE];
    /* RSA2048 public key for RootCA sign verification */
    uint8_t encrypted_user_rsa2048_ne_key[R_TSIP_RSA2048_NE_KEY_BYTE_SIZE + 16];
    /* update key (not used) */
    uint8_t encrypted_user_update_key[R_TSIP_AES256_KEY_BYTE_SIZE + 16];
    /* wrapped client RSA2048bit public key */
    uint8_t encrypted_user_rsa2048_public_key[R_TSIP_RSA2048_NE_KEY_BYTE_SIZE + 16];
    /* wrapped client RSA2048bit private key */
    uint8_t encrypted_user_rsa2048_private_key[R_TSIP_RSA2048_ND_KEY_BYTE_SIZE + 16];
    /* wrapped client ECC P256 public key */
    uint8_t encrypted_user_ecc256_public_key[R_TSIP_ECC_PUBLIC_KEY_BYTE_SIZE + 16];
    /* wrapped client ECC P256 private key */
    uint8_t encrypted_user_ecc256_private_key[R_TSIP_ECC_PRIVATE_KEY_BYTE_SIZE + 16];
} st_key_block_data_t;


extern const uint32_t               encrypted_user_key_type;
extern const st_key_block_data_t    g_key_block_data;

extern const unsigned char          ca_cert_der_sig[];
extern const unsigned char          ca_ecc_cert_der_sig[];
extern const unsigned char          client_cert_der_sign[];
extern const int                    sizeof_ca_cert_der;


#endif /* WOLFSSL_RENESAS_TSIP */
#endif /* KEY_DATA_H_ */

