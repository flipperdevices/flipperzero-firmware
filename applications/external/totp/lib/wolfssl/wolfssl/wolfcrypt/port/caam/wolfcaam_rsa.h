/* wolfcaam_rsa.h
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

#include <wolfssl/wolfcrypt/settings.h>

#ifndef WOLFCAAM_RSA_H
#define WOLFCAAM_RSA_H

#if !defined(NO_RSA) && defined(WOLFSSL_CAAM)

#include <wolfssl/wolfcrypt/rsa.h>

WOLFSSL_LOCAL int wc_CAAM_Rsa(const byte* in, word32 inLen, byte* out,
    word32* outLen, int type, RsaKey* key, WC_RNG* rng);
#ifdef WOLFSSL_KEY_GEN
WOLFSSL_LOCAL int wc_CAAM_MakeRsaKey(RsaKey* key, int size, long e,
    WC_RNG* rng);
#endif

#endif /* !NO_RSA && WOLFSSL_CAAM */
#endif /* WOLFCAAM_RSA_H */

