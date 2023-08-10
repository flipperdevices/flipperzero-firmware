/* nrf51.h
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


#ifndef WOLFSSL_NRF51_PORT_H
#define WOLFSSL_NRF51_PORT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <wolfssl/wolfcrypt/types.h>

/* Public Functions */
int nrf51_random_generate(byte* output, word32 sz);

int nrf51_aes_set_key(const byte* key);
int nrf51_aes_encrypt(const byte* in, const byte* key, word32 rounds, byte* out);

double current_time(int reset);

#ifdef __cplusplus
}
#endif

#endif /* WOLFSSL_NRF51_PORT_H */
