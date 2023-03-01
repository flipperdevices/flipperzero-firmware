/**
 * Copyright (c) 2013-2014 Tomas Dzetkulic
 * Copyright (c) 2013-2014 Pavol Rusnak
 * Copyright (c)      2015 Jochen Hoenicke
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES
 * OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */
#include <assert.h>

#include "hmac_drbg.h"
#include "memzero.h"
#include "rfc6979.h"

void init_rfc6979(const uint8_t *priv_key, const uint8_t *hash,
                  const ecdsa_curve *curve, rfc6979_state *state) {
  if (curve) {
    bignum256 hash_bn = {0};
    bn_read_be(hash, &hash_bn);

    // Make sure hash is partly reduced modulo order
    assert(bn_bitcount(&curve->order) >= 256);
    bn_mod(&hash_bn, &curve->order);

    uint8_t hash_reduced[32] = {0};
    bn_write_be(&hash_bn, hash_reduced);
    memzero(&hash_bn, sizeof(hash_bn));
    hmac_drbg_init(state, priv_key, 32, hash_reduced, 32);
    memzero(hash_reduced, sizeof(hash_reduced));
  } else {
    hmac_drbg_init(state, priv_key, 32, hash, 32);
  }
}

// generate next number from deterministic random number generator
void generate_rfc6979(uint8_t rnd[32], rfc6979_state *state) {
  hmac_drbg_generate(state, rnd, 32);
}

// generate K in a deterministic way, according to RFC6979
// http://tools.ietf.org/html/rfc6979
void generate_k_rfc6979(bignum256 *k, rfc6979_state *state) {
  uint8_t buf[32] = {0};
  generate_rfc6979(buf, state);
  bn_read_be(buf, k);
  memzero(buf, sizeof(buf));
}
