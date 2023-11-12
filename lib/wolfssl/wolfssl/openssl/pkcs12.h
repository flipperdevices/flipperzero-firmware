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

/* pkcs12.h for openssl */


#include <wolfssl/openssl/ssl.h>
#include <wolfssl/wolfcrypt/pkcs12.h>

#ifndef WOLFSSL_PKCS12_COMPAT_H_
#define WOLFSSL_PKCS12_COMPAT_H_

#define NID_pbe_WithSHA1AndDES_CBC             2
#define NID_pbe_WithSHA1And3_Key_TripleDES_CBC 3
#define NID_pbe_WithSHA1And128BitRC4           1

#define PKCS12_DEFAULT_ITER WC_PKCS12_ITT_DEFAULT

/* wolfCrypt level does not make use of ssl.h */
#define PKCS12         WC_PKCS12
#define PKCS12_new     wc_PKCS12_new
#define PKCS12_free    wc_PKCS12_free

/* wolfSSL level using structs from ssl.h and calls down to wolfCrypt */
#define d2i_PKCS12_bio wolfSSL_d2i_PKCS12_bio
#define PKCS12_parse   wolfSSL_PKCS12_parse
#define PKCS12_verify_mac wolfSSL_PKCS12_verify_mac
#define PKCS12_create  wolfSSL_PKCS12_create
#define PKCS12_PBE_add wolfSSL_PKCS12_PBE_add

#endif /* WOLFSSL_PKCS12_COMPAT_H_ */

