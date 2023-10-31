/* wolfcaam_ecdsa.h
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

#ifndef WOLFCAAM_ECDSA_H
#define WOLFCAAM_ECDSA_H

#if defined(HAVE_ECC) && defined(WOLFSSL_CAAM)

#include <wolfssl/wolfcrypt/ecc.h>

WOLFSSL_LOCAL int wc_CAAM_EccSign(const byte* in, int inlen, byte* out,
        word32* outlen, WC_RNG *rng, ecc_key *key, int devId);
WOLFSSL_LOCAL int wc_CAAM_EccVerify(const byte* sig, word32 siglen,
        const byte* hash, word32 hashlen, int* res, ecc_key* key, int devId);
WOLFSSL_LOCAL int wc_CAAM_Ecdh(ecc_key* private_key, ecc_key* public_key,
        byte* out, word32* outlen, int devId);
WOLFSSL_LOCAL int wc_CAAM_MakeEccKey(WC_RNG* rng, int keySize, ecc_key* key,
        int curveId, int devId);
WOLFSSL_LOCAL int wc_CAAM_EccCheckPrivKey(ecc_key* key, const byte* pubKey,
        word32 pubKeySz);
#endif /* HAVE_ECC && WOLFSSL_QNX_CAAM */
#endif /* WOLFCAAM_ECDSA_H */
