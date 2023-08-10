/* user_settings_EBSnet.h
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

/* Example wolfSSL user settings for use with EBSnet ports.
 * This file is included with wolfssl/wolfcrypt/settings.h when WOLFSSL_USER_SETTINGS is defined.
 */

#ifndef WOLFSSL_USER_SETTINGS_H
#define WOLFSSL_USER_SETTINGS_H

#ifdef __cplusplus
extern "C" {
#endif

#define OPENSSL_EXTRA_X509_SMALL

#define USE_FAST_MATH
#define TFM_TIMING_RESISTANT
#define WC_RSA_BLINDING
#define ECC_TIMING_RESISTANT

#define HAVE_ECC
#define HAVE_CURVE25519
#define HAVE_AESGCM
#define WOLFSSL_SHA384

#define NO_DSA
#define NO_RC4
#define NO_MD4
#define NO_MD5
#define NO_DES3

#ifdef __cplusplus
}
#endif

#endif /* WOLFSSL_USER_SETTINGS_H */

