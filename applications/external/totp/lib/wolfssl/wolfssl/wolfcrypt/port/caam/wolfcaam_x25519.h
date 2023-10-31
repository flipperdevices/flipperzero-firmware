/* wolfcaam_x25519.h
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

#ifndef WOLFCAAM_X25519_H
#define WOLFCAAM_X25519_H

#if defined(HAVE_CURVE25519) && defined(WOLFSSL_CAAM)

#include <wolfssl/wolfcrypt/curve25519.h>

WOLFSSL_LOCAL int wc_CAAM_MakeCurve25519Key(curve25519_key* key, int keySize,
    WC_RNG* rng);
WOLFSSL_LOCAL int wc_CAAM_Curve25519(byte* out, word32* outlen,
    curve25519_key* k, curve25519_key* pubKey, int endian);

#endif /* !NO_RSA && WOLFSSL_CAAM */
#endif /* WOLFCAAM_X25519_H */

