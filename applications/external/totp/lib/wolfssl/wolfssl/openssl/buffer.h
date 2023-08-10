/* buffer.h
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


#ifndef WOLFSSL_BUFFER_H_
#define WOLFSSL_BUFFER_H_

#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/openssl/ssl.h>

#ifdef __cplusplus
    extern "C" {
#endif


WOLFSSL_API WOLFSSL_BUF_MEM* wolfSSL_BUF_MEM_new(void);
WOLFSSL_API int wolfSSL_BUF_MEM_grow(WOLFSSL_BUF_MEM* buf, size_t len);
WOLFSSL_API int wolfSSL_BUF_MEM_grow_ex(WOLFSSL_BUF_MEM* buf, size_t len,
        char zeroFill);
WOLFSSL_API int wolfSSL_BUF_MEM_resize(WOLFSSL_BUF_MEM* buf, size_t len);
WOLFSSL_API void wolfSSL_BUF_MEM_free(WOLFSSL_BUF_MEM* buf);


#define BUF_MEM_new  wolfSSL_BUF_MEM_new
#define BUF_MEM_grow wolfSSL_BUF_MEM_grow
#define BUF_MEM_free wolfSSL_BUF_MEM_free

#define BUF_strdup strdup
#define BUF_strlcpy wc_strlcpy
#define BUF_strlcat wc_strlcat

#ifdef __cplusplus
    }  /* extern "C" */
#endif

#endif /* WOLFSSL_BUFFER_H_ */
