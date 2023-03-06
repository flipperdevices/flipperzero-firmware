/**
 * Copyright (c) 2013-2021 SatoshiLabs
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

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "bignum.h"
#include "bip32.h"
#include "cardano.h"
#include "curves.h"
#include "hasher.h"
#include "hmac.h"
#include "memzero.h"
#include "options.h"
#include "pbkdf2.h"
#include "sha2.h"

#if USE_CARDANO

#define CARDANO_MAX_NODE_DEPTH 1048576

const curve_info ed25519_cardano_info = {
    .bip32_name = ED25519_CARDANO_NAME,
    .params = NULL,
    .hasher_base58 = HASHER_SHA2D,
    .hasher_sign = HASHER_SHA2D,
    .hasher_pubkey = HASHER_SHA2_RIPEMD,
    .hasher_script = HASHER_SHA2,
};

static void scalar_multiply8(const uint8_t *src, int bytes, uint8_t *dst) {
  uint8_t prev_acc = 0;
  for (int i = 0; i < bytes; i++) {
    dst[i] = (src[i] << 3) + (prev_acc & 0x7);
    prev_acc = src[i] >> 5;
  }
  dst[bytes] = src[bytes - 1] >> 5;
}

static void scalar_add_256bits(const uint8_t *src1, const uint8_t *src2,
                               uint8_t *dst) {
  uint16_t r = 0;
  for (int i = 0; i < 32; i++) {
    r = r + (uint16_t)src1[i] + (uint16_t)src2[i];
    dst[i] = r & 0xff;
    r >>= 8;
  }
}

static void cardano_ed25519_tweak_bits(uint8_t private_key[32]) {
  private_key[0] &= 0xf8;
  private_key[31] &= 0x1f;
  private_key[31] |= 0x40;
}

int hdnode_private_ckd_cardano(HDNode *inout, uint32_t index) {
  if (inout->curve != &ed25519_cardano_info) {
    return 0;
  }

  if (inout->depth >= CARDANO_MAX_NODE_DEPTH) {
    return 0;
  }

  // checks for hardened/non-hardened derivation, keysize 32 means we are
  // dealing with public key and thus non-h, keysize 64 is for private key
  int keysize = 32;
  if (index & 0x80000000) {
    keysize = 64;
  }

  static CONFIDENTIAL uint8_t data[1 + 64 + 4];
  static CONFIDENTIAL uint8_t z[32 + 32];
  static CONFIDENTIAL uint8_t priv_key[64];
  static CONFIDENTIAL uint8_t res_key[64];

  write_le(data + keysize + 1, index);

  memcpy(priv_key, inout->private_key, 32);
  memcpy(priv_key + 32, inout->private_key_extension, 32);

  if (keysize == 64) {  // private derivation
    data[0] = 0;
    memcpy(data + 1, inout->private_key, 32);
    memcpy(data + 1 + 32, inout->private_key_extension, 32);
  } else {  // public derivation
    if (hdnode_fill_public_key(inout) != 0) {
      return 0;
    }
    data[0] = 2;
    memcpy(data + 1, inout->public_key + 1, 32);
  }

  static CONFIDENTIAL HMAC_SHA512_CTX ctx;
  hmac_sha512_Init(&ctx, inout->chain_code, 32);
  hmac_sha512_Update(&ctx, data, 1 + keysize + 4);
  hmac_sha512_Final(&ctx, z);

  static CONFIDENTIAL uint8_t zl8[32];
  memzero(zl8, 32);

  /* get 8 * Zl */
  scalar_multiply8(z, 28, zl8);
  /* Kl = 8*Zl + parent(K)l */
  scalar_add_256bits(zl8, priv_key, res_key);

  /* Kr = Zr + parent(K)r */
  scalar_add_256bits(z + 32, priv_key + 32, res_key + 32);

  memcpy(inout->private_key, res_key, 32);
  memcpy(inout->private_key_extension, res_key + 32, 32);

  if (keysize == 64) {
    data[0] = 1;
  } else {
    data[0] = 3;
  }
  hmac_sha512_Init(&ctx, inout->chain_code, 32);
  hmac_sha512_Update(&ctx, data, 1 + keysize + 4);
  hmac_sha512_Final(&ctx, z);

  memcpy(inout->chain_code, z + 32, 32);
  inout->depth++;
  inout->child_num = index;
  memzero(inout->public_key, sizeof(inout->public_key));

  // making sure to wipe our memory
  memzero(z, sizeof(z));
  memzero(data, sizeof(data));
  memzero(priv_key, sizeof(priv_key));
  memzero(res_key, sizeof(res_key));
  return 1;
}

int hdnode_from_secret_cardano(const uint8_t secret[CARDANO_SECRET_LENGTH],
                               HDNode *out) {
  memzero(out, sizeof(HDNode));
  out->depth = 0;
  out->child_num = 0;
  out->curve = &ed25519_cardano_info;
  memcpy(out->private_key, secret, 32);
  memcpy(out->private_key_extension, secret + 32, 32);
  memcpy(out->chain_code, secret + 64, 32);

  cardano_ed25519_tweak_bits(out->private_key);

  out->public_key[0] = 0;
  if (hdnode_fill_public_key(out) != 0) {
    return 0;
  }

  return 1;
}

// Derives the root Cardano secret from a master secret, aka seed, as defined in
// SLIP-0023.
int secret_from_seed_cardano_slip23(const uint8_t *seed, int seed_len,
                                    uint8_t secret_out[CARDANO_SECRET_LENGTH]) {
  static CONFIDENTIAL uint8_t I[SHA512_DIGEST_LENGTH];
  static CONFIDENTIAL HMAC_SHA512_CTX ctx;

  hmac_sha512_Init(&ctx, (const uint8_t *)ED25519_CARDANO_NAME,
                   strlen(ED25519_CARDANO_NAME));
  hmac_sha512_Update(&ctx, seed, seed_len);
  hmac_sha512_Final(&ctx, I);

  sha512_Raw(I, 32, secret_out);

  memcpy(secret_out + SHA512_DIGEST_LENGTH, I + 32, 32);
  cardano_ed25519_tweak_bits(secret_out);

  memzero(I, sizeof(I));
  memzero(&ctx, sizeof(ctx));
  return 1;
}

// Derives the root Cardano secret from a BIP-32 master secret via the Ledger
// derivation:
// https://github.com/cardano-foundation/CIPs/blob/09d7d8ee1bd64f7e6b20b5a6cae088039dce00cb/CIP-0003/Ledger.md
int secret_from_seed_cardano_ledger(const uint8_t *seed, int seed_len,
                                    uint8_t secret_out[CARDANO_SECRET_LENGTH]) {
  static CONFIDENTIAL uint8_t chain_code[SHA256_DIGEST_LENGTH];
  static CONFIDENTIAL uint8_t root_key[SHA512_DIGEST_LENGTH];
  static CONFIDENTIAL HMAC_SHA256_CTX ctx;
  static CONFIDENTIAL HMAC_SHA512_CTX sctx;

  const uint8_t *intermediate_result = seed;
  int intermediate_result_len = seed_len;
  do {
    // STEP 1: derive a master secret like in BIP-32/SLIP-10
    hmac_sha512_Init(&sctx, (const uint8_t *)ED25519_SEED_NAME,
                     strlen(ED25519_SEED_NAME));
    hmac_sha512_Update(&sctx, intermediate_result, intermediate_result_len);
    hmac_sha512_Final(&sctx, root_key);

    // STEP 2: check that the resulting key does not have a particular bit set,
    // otherwise iterate like in SLIP-10
    intermediate_result = root_key;
    intermediate_result_len = sizeof(root_key);
  } while (root_key[31] & 0x20);

  // STEP 3: calculate the chain code as a HMAC-SHA256 of "\x01" + seed,
  // key is "ed25519 seed"
  hmac_sha256_Init(&ctx, (const unsigned char *)ED25519_SEED_NAME,
                   strlen(ED25519_SEED_NAME));
  hmac_sha256_Update(&ctx, (const unsigned char *)"\x01", 1);
  hmac_sha256_Update(&ctx, seed, seed_len);
  hmac_sha256_Final(&ctx, chain_code);

  // STEP 4: extract information into output
  _Static_assert(
      SHA512_DIGEST_LENGTH + SHA256_DIGEST_LENGTH == CARDANO_SECRET_LENGTH,
      "Invalid configuration of Cardano secret size");
  memcpy(secret_out, root_key, SHA512_DIGEST_LENGTH);
  memcpy(secret_out + SHA512_DIGEST_LENGTH, chain_code, SHA256_DIGEST_LENGTH);

  // STEP 5: tweak bits of the private key
  cardano_ed25519_tweak_bits(secret_out);

  memzero(&ctx, sizeof(ctx));
  memzero(&sctx, sizeof(sctx));
  memzero(root_key, sizeof(root_key));
  memzero(chain_code, sizeof(chain_code));
  return 1;
}

#define CARDANO_ICARUS_STEPS 32
_Static_assert(
    CARDANO_ICARUS_PBKDF2_ROUNDS % CARDANO_ICARUS_STEPS == 0,
    "CARDANO_ICARUS_STEPS does not divide CARDANO_ICARUS_PBKDF2_ROUNDS");
#define CARDANO_ICARUS_ROUNDS_PER_STEP \
  (CARDANO_ICARUS_PBKDF2_ROUNDS / CARDANO_ICARUS_STEPS)

// Derives the root Cardano HDNode from a passphrase and the entropy encoded in
// a BIP-0039 mnemonic using the Icarus derivation scheme, aka V2 derivation
// scheme:
// https://github.com/cardano-foundation/CIPs/blob/09d7d8ee1bd64f7e6b20b5a6cae088039dce00cb/CIP-0003/Icarus.md
int secret_from_entropy_cardano_icarus(
    const uint8_t *pass, int pass_len, const uint8_t *entropy, int entropy_len,
    uint8_t secret_out[CARDANO_SECRET_LENGTH],
    void (*progress_callback)(uint32_t, uint32_t)) {
  static CONFIDENTIAL PBKDF2_HMAC_SHA512_CTX pctx;
  static CONFIDENTIAL uint8_t digest[SHA512_DIGEST_LENGTH];
  uint32_t progress = 0;

  // PASS 1: first 64 bytes
  pbkdf2_hmac_sha512_Init(&pctx, pass, pass_len, entropy, entropy_len, 1);
  if (progress_callback) {
    progress_callback(progress, CARDANO_ICARUS_PBKDF2_ROUNDS * 2);
  }
  for (int i = 0; i < CARDANO_ICARUS_STEPS; i++) {
    pbkdf2_hmac_sha512_Update(&pctx, CARDANO_ICARUS_ROUNDS_PER_STEP);
    if (progress_callback) {
      progress += CARDANO_ICARUS_ROUNDS_PER_STEP;
      progress_callback(progress, CARDANO_ICARUS_PBKDF2_ROUNDS * 2);
    }
  }
  pbkdf2_hmac_sha512_Final(&pctx, digest);

  memcpy(secret_out, digest, SHA512_DIGEST_LENGTH);

  // PASS 2: remaining 32 bytes
  pbkdf2_hmac_sha512_Init(&pctx, pass, pass_len, entropy, entropy_len, 2);
  if (progress_callback) {
    progress_callback(progress, CARDANO_ICARUS_PBKDF2_ROUNDS * 2);
  }
  for (int i = 0; i < CARDANO_ICARUS_STEPS; i++) {
    pbkdf2_hmac_sha512_Update(&pctx, CARDANO_ICARUS_ROUNDS_PER_STEP);
    if (progress_callback) {
      progress += CARDANO_ICARUS_ROUNDS_PER_STEP;
      progress_callback(progress, CARDANO_ICARUS_PBKDF2_ROUNDS * 2);
    }
  }
  pbkdf2_hmac_sha512_Final(&pctx, digest);

  memcpy(secret_out + SHA512_DIGEST_LENGTH, digest,
         CARDANO_SECRET_LENGTH - SHA512_DIGEST_LENGTH);

  cardano_ed25519_tweak_bits(secret_out);

  memzero(&pctx, sizeof(pctx));
  memzero(digest, sizeof(digest));
  return 1;
}

#endif  // USE_CARDANO
