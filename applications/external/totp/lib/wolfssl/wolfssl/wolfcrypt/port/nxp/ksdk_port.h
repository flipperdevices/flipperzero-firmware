/* ksdk_port.h
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

#ifndef _KSDK_PORT_H_
#define _KSDK_PORT_H_

#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/wolfcrypt/wolfmath.h>
#include <wolfssl/wolfcrypt/ecc.h>
#include <wolfssl/wolfcrypt/curve25519.h>
#include <wolfssl/wolfcrypt/ed25519.h>


/* API to init required hardware */
int ksdk_port_init(void);

/* software algorithm, by wolfcrypt */
#if defined(FREESCALE_LTC_TFM)
	int wolfcrypt_mp_mul(mp_int *A, mp_int *B, mp_int *C);
	int wolfcrypt_mp_mod(mp_int *a, mp_int *b, mp_int *c);
	int wolfcrypt_mp_mulmod(mp_int *a, mp_int *b, mp_int *c, mp_int *d);
	int wolfcrypt_mp_mod(mp_int *a, mp_int *b, mp_int *c);
	int wolfcrypt_mp_invmod(mp_int *a, mp_int *b, mp_int *c);
	int wolfcrypt_mp_exptmod(mp_int *G, mp_int *X, mp_int *P, mp_int *Y);
	int wolfcrypt_mp_prime_is_prime_ex(mp_int* a, int t, int* result, WC_RNG* rng);

    /* Exported mp_mulmod function */
    int mp_mulmod(mp_int *a, mp_int *b, mp_int *c, mp_int *d);

#endif /* FREESCALE_LTC_TFM */

#if defined(FREESCALE_LTC_ECC)
	#include "fsl_ltc.h"

	typedef enum _fsl_ltc_ecc_coordinate_system
	{
	    kLTC_Weierstrass = 0U, /*< Point coordinates on an elliptic curve in Weierstrass form */
	    kLTC_Curve25519 = 1U,  /*< Point coordinates on an Curve25519 elliptic curve in Montgomery form */
	    kLTC_Ed25519 = 2U,     /*< Point coordinates on an Ed25519 elliptic curve in twisted Edwards form */
	} fsl_ltc_ecc_coordinate_system_t;

	int wc_ecc_point_add(ecc_point *mG, ecc_point *mQ, ecc_point *mR, mp_int *m);

	#ifdef HAVE_CURVE25519
		int nxp_ltc_curve25519(ECPoint *q, const byte *n, const ECPoint *p, fsl_ltc_ecc_coordinate_system_t type);
		const ECPoint *nxp_ltc_curve25519_GetBasePoint(void);
		status_t LTC_PKHA_Curve25519ToWeierstrass(const ltc_pkha_ecc_point_t *ltcPointIn, ltc_pkha_ecc_point_t *ltcPointOut);
		status_t LTC_PKHA_WeierstrassToCurve25519(const ltc_pkha_ecc_point_t *ltcPointIn, ltc_pkha_ecc_point_t *ltcPointOut);
		status_t LTC_PKHA_Curve25519ComputeY(ltc_pkha_ecc_point_t *ltcPoint);
	#endif

	#ifdef HAVE_ED25519
		status_t LTC_PKHA_Ed25519ToWeierstrass(const ltc_pkha_ecc_point_t *ltcPointIn, ltc_pkha_ecc_point_t *ltcPointOut);
		status_t LTC_PKHA_WeierstrassToEd25519(const ltc_pkha_ecc_point_t *ltcPointIn, ltc_pkha_ecc_point_t *ltcPointOut);
		status_t LTC_PKHA_Ed25519_PointMul(const ltc_pkha_ecc_point_t *ltcPointIn,
		                                   const uint8_t *N,
		                                   size_t sizeN,
		                                   ltc_pkha_ecc_point_t *ltcPointOut,
		                                   fsl_ltc_ecc_coordinate_system_t typeOut);
		const ltc_pkha_ecc_point_t *LTC_PKHA_Ed25519_BasePoint(void);
		status_t LTC_PKHA_Ed25519_PointDecompress(const uint8_t *pubkey, size_t pubKeySize, ltc_pkha_ecc_point_t *ltcPointOut);
		status_t LTC_PKHA_sc_reduce(uint8_t *a);
		status_t LTC_PKHA_sc_muladd(uint8_t *s, const uint8_t *a, const uint8_t *b, const uint8_t *c);
		status_t LTC_PKHA_SignatureForVerify(uint8_t *rcheck, const unsigned char *a, const unsigned char *b, ed25519_key *key);
		status_t LTC_PKHA_Ed25519_Compress(const ltc_pkha_ecc_point_t *ltcPointIn, uint8_t *p);
	#endif

#endif /* FREESCALE_LTC_ECC */

#endif /* _KSDK_PORT_H_ */
