//
// Created by Dusan Klinec on 10/05/2018.
//

#ifndef TREZOR_CRYPTO_XMR_H
#define TREZOR_CRYPTO_XMR_H

#include "ed25519-donna/ed25519-donna.h"
#include "hasher.h"

extern const ge25519 ALIGN(16) xmr_h;

typedef unsigned char xmr_key_t[32];

typedef struct xmr_ctkey {
  xmr_key_t dest;
  xmr_key_t mask;
} xmr_ctkey_t;

/* sets H point to r */
void ge25519_set_xmr_h(ge25519 *r);

/* random scalar value */
void xmr_random_scalar(bignum256modm m);

/* cn_fast_hash */
void xmr_fast_hash(uint8_t *hash, const void *data, size_t length);

/* incremental hashing wrappers */
void xmr_hasher_init(Hasher *hasher);
void xmr_hasher_update(Hasher *hasher, const void *data, size_t length);
void xmr_hasher_final(Hasher *hasher, uint8_t *hash);
void xmr_hasher_copy(Hasher *dst, const Hasher *src);

/* H_s(buffer) */
void xmr_hash_to_scalar(bignum256modm r, const void *data, size_t length);

/* H_p(buffer) */
void xmr_hash_to_ec(ge25519 *P, const void *data, size_t length);

/* derivation to scalar value */
void xmr_derivation_to_scalar(bignum256modm s, const ge25519 *p,
                              uint32_t output_index);

/* derivation */
void xmr_generate_key_derivation(ge25519 *r, const ge25519 *A,
                                 const bignum256modm b);

/* H_s(derivation || varint(output_index)) + base */
void xmr_derive_private_key(bignum256modm s, const ge25519 *deriv, uint32_t idx,
                            const bignum256modm base);

/* H_s(derivation || varint(output_index))G + base */
void xmr_derive_public_key(ge25519 *r, const ge25519 *deriv, uint32_t idx,
                           const ge25519 *base);

/* aG + bB, G is basepoint */
void xmr_add_keys2(ge25519 *r, const bignum256modm a, const bignum256modm b,
                   const ge25519 *B);
void xmr_add_keys2_vartime(ge25519 *r, const bignum256modm a,
                           const bignum256modm b, const ge25519 *B);

/* aA + bB */
void xmr_add_keys3(ge25519 *r, const bignum256modm a, const ge25519 *A,
                   const bignum256modm b, const ge25519 *B);
void xmr_add_keys3_vartime(ge25519 *r, const bignum256modm a, const ge25519 *A,
                           const bignum256modm b, const ge25519 *B);

/* subaddress secret */
void xmr_get_subaddress_secret_key(bignum256modm r, uint32_t major,
                                   uint32_t minor, const bignum256modm m);

/* Generates Pedersen commitment C = aG + bH */
void xmr_gen_c(ge25519 *r, const bignum256modm a, uint64_t amount);

#endif  // TREZOR_CRYPTO_XMR_H
