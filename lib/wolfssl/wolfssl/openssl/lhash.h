/* lhash.h
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

/* lhash.h for openSSL */

#ifndef WOLFSSL_lhash_H_
#define WOLFSSL_lhash_H_

#ifdef __cplusplus
    extern "C" {
#endif

#include <wolfssl/openssl/ssl.h>

#ifdef OPENSSL_ALL
#define IMPLEMENT_LHASH_HASH_FN(name, type) \
    unsigned long wolfSSL_##name##_LHASH_HASH(const void *arg) \
    {                                                          \
        const type *a = arg;                                 \
        return name##_hash(a);                                 \
    }
#define IMPLEMENT_LHASH_COMP_FN(name, type) \
    int wolfSSL_##name##_LHASH_COMP(const void *p1, const void *p2) \
    {                                                               \
        const type *_p1 = p1;                                       \
        const type *_p2 = p2;                                       \
        return name##_cmp(_p1, _p2);                                \
    }

#define LHASH_HASH_FN(name) wolfSSL_##name##_LHASH_HASH
#define LHASH_COMP_FN(name) wolfSSL_##name##_LHASH_COMP

WOLFSSL_API unsigned long wolfSSL_LH_strhash(const char *str);

WOLFSSL_API void *wolfSSL_lh_retrieve(WOLFSSL_STACK *sk, void *data);

#define lh_strhash          wolfSSL_LH_strhash

#endif


#ifdef  __cplusplus
} /* extern "C" */
#endif

#endif /* WOLFSSL_lhash_H_ */
