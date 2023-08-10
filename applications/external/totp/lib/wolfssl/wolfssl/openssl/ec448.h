/* ec448.h
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

/* ec448.h */

#ifndef WOLFSSL_EC448_H_
#define WOLFSSL_EC448_H_

#include <wolfssl/openssl/compat_types.h>

#ifdef __cplusplus
extern "C" {
#endif

WOLFSSL_API
int wolfSSL_EC448_generate_key(unsigned char *priv, unsigned int *privSz,
                               unsigned char *pub, unsigned int *pubSz);

WOLFSSL_API
int wolfSSL_EC448_shared_key(unsigned char *shared, unsigned int *sharedSz,
                             const unsigned char *priv, unsigned int privSz,
                             const unsigned char *pub, unsigned int pubSz);

#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif /* header */
