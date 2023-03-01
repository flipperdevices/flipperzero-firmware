/**
 * Copyright (c) SatoshiLabs
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
#include <stdbool.h>
#include <string.h>

#include "memzero.h"
#include "secp256k1.h"
#include "zkp_context.h"

#include "vendor/secp256k1-zkp/include/secp256k1.h"
#include "vendor/secp256k1-zkp/include/secp256k1_extrakeys.h"
#include "vendor/secp256k1-zkp/include/secp256k1_preallocated.h"
#include "vendor/secp256k1-zkp/include/secp256k1_recovery.h"

#include "zkp_ecdsa.h"

static bool is_zero_digest(const uint8_t *digest) {
  const uint8_t zeroes[32] = {0};
  return memcmp(digest, zeroes, 32) == 0;
}

// ECDSA compressed public key derivation
// curve has to be &secp256k1
// private_key_bytes has 32 bytes
// public_key_bytes has 33 bytes
// returns 0 on success
int zkp_ecdsa_get_public_key33(const ecdsa_curve *curve,
                               const uint8_t *private_key_bytes,
                               uint8_t *public_key_bytes) {
  assert(curve == &secp256k1);
  if (curve != &secp256k1) {
    return 1;
  }

  int result = 0;

  secp256k1_context *context_writable = NULL;
  if (result == 0) {
    context_writable = zkp_context_acquire_writable();
    if (context_writable == NULL) {
      result = 1;
    }
  }
  if (result == 0) {
    if (secp256k1_context_writable_randomize(context_writable) != 0) {
      result = 1;
    }
  }

  secp256k1_pubkey public_key = {0};
  if (result == 0) {
    if (secp256k1_ec_pubkey_create(context_writable, &public_key,
                                   private_key_bytes) != 1) {
      result = 1;
    }
  }

  if (context_writable) {
    zkp_context_release_writable();
    context_writable = NULL;
  }

  if (result == 0) {
    size_t written = 33;
    const secp256k1_context *context_read_only = zkp_context_get_read_only();
    int returned = secp256k1_ec_pubkey_serialize(
        context_read_only, public_key_bytes, &written, &public_key,
        SECP256K1_EC_COMPRESSED);

    if (returned != 1 || written != 33) {
      result = 1;
    }
  }

  memzero(&public_key, sizeof(public_key));
  return result;
}

// ECDSA uncompressed public key derivation
// curve has to be &secp256k1
// private_key_bytes has 32 bytes
// public_key_bytes has 65 bytes
// returns 0 on success
int zkp_ecdsa_get_public_key65(const ecdsa_curve *curve,
                               const uint8_t *private_key_bytes,
                               uint8_t *public_key_bytes) {
  assert(curve == &secp256k1);
  if (curve != &secp256k1) {
    return 1;
  }

  int result = 0;

  secp256k1_context *context_writable = NULL;
  if (result == 0) {
    context_writable = zkp_context_acquire_writable();
    if (context_writable == NULL) {
      result = 1;
    }
  }
  if (result == 0) {
    if (secp256k1_context_writable_randomize(context_writable) != 0) {
      result = 1;
    }
  }

  secp256k1_pubkey public_key = {0};
  if (result == 0) {
    if (secp256k1_ec_pubkey_create(context_writable, &public_key,
                                   private_key_bytes) != 1) {
      result = 1;
    }
  }

  if (context_writable) {
    zkp_context_release_writable();
    context_writable = NULL;
  }

  if (result == 0) {
    size_t written = 65;
    const secp256k1_context *context_read_only = zkp_context_get_read_only();
    int returned = secp256k1_ec_pubkey_serialize(
        context_read_only, public_key_bytes, &written, &public_key,
        SECP256K1_EC_UNCOMPRESSED);

    if (returned != 1 || written != 65) {
      result = 1;
    }
  }

  memzero(&public_key, sizeof(public_key));
  return result;
}

// ECDSA signing
// curve has to be &secp256k1
// private_key_bytes has 32 bytes
// digest has 32 bytes
// signature_bytes has 64 bytes
// pby is one byte
// is_canonical has to be NULL
// returns 0 on success
int zkp_ecdsa_sign_digest(
    const ecdsa_curve *curve, const uint8_t *private_key_bytes,
    const uint8_t *digest, uint8_t *signature_bytes, uint8_t *pby,
    int (*is_canonical)(uint8_t by, uint8_t signature_bytes[64])) {
  assert(curve == &secp256k1);
  assert(is_canonical == NULL);
  if (curve != &secp256k1 || is_canonical != NULL) {
    return 1;
  }

  int result = 0;

  if (result == 0) {
    if (is_zero_digest(digest)) {
      // The probability of the digest being all-zero by chance is
      // infinitesimal, so this is most likely an indication of a bug.
      // Furthermore, the signature has no value, because in this case it can be
      // easily forged for any public key, see zkp_ecdsa_verify_digest().
      result = 1;
    }
  }

  secp256k1_context *context_writable = NULL;
  if (result == 0) {
    context_writable = zkp_context_acquire_writable();
    if (context_writable == NULL) {
      result = 1;
    }
  }
  if (result == 0) {
    if (secp256k1_context_writable_randomize(context_writable) != 0) {
      result = 1;
    }
  }

  secp256k1_ecdsa_recoverable_signature recoverable_signature = {0};
  if (result == 0) {
    if (secp256k1_ecdsa_sign_recoverable(context_writable,
                                         &recoverable_signature, digest,
                                         private_key_bytes, NULL, NULL) != 1) {
      result = 1;
    }
  }

  if (context_writable) {
    zkp_context_release_writable();
    context_writable = NULL;
  }

  if (result == 0) {
    int recid = 0;
    const secp256k1_context *context_read_only = zkp_context_get_read_only();
    if (secp256k1_ecdsa_recoverable_signature_serialize_compact(
            context_read_only, signature_bytes, &recid,
            &recoverable_signature) != 1) {
      result = 1;
    }
    if (pby != NULL) {
      *pby = (uint8_t)recid;
    }
  }

  memzero(&recoverable_signature, sizeof(recoverable_signature));

  return result;
}

// ECDSA public key recovery
// public_key_bytes has 65 bytes
// signature_bytes has 64 bytes
// digest has 32 bytes
// recid is 0, 1, 2 or 3
// returns 0 on success
int zkp_ecdsa_recover_pub_from_sig(const ecdsa_curve *curve,
                                   uint8_t *public_key_bytes,
                                   const uint8_t *signature_bytes,
                                   const uint8_t *digest, int recid) {
  assert(curve == &secp256k1);
  if (curve != &secp256k1) {
    return 1;
  }

  int result = 0;

  const secp256k1_context *context_read_only = zkp_context_get_read_only();
  secp256k1_ecdsa_recoverable_signature recoverable_signature = {0};

  if (result == 0) {
    if (secp256k1_ecdsa_recoverable_signature_parse_compact(
            context_read_only, &recoverable_signature, signature_bytes,
            recid) != 1) {
      result = 1;
    }
  }

  secp256k1_pubkey public_key = {0};

  if (result == 0) {
    if (secp256k1_ecdsa_recover(context_read_only, &public_key,
                                &recoverable_signature, digest) != 1) {
      result = 1;
    }
  }

  memzero(&recoverable_signature, sizeof(recoverable_signature));

  if (result == 0) {
    size_t written = 65;
    int returned = secp256k1_ec_pubkey_serialize(
        context_read_only, public_key_bytes, &written, &public_key,
        SECP256K1_EC_UNCOMPRESSED);
    if (returned != 1 || written != 65) {
      result = 1;
    }
  }

  memzero(&public_key, sizeof(public_key));

  return result;
}

// ECDSA verification
// curve has to be &secp256k1
// public_key_bytes has 33 or 65 bytes
// signature_bytes has 64 bytes
// digest has 32 bytes
// returns 0 if verification succeeded
int zkp_ecdsa_verify_digest(const ecdsa_curve *curve,
                            const uint8_t *public_key_bytes,
                            const uint8_t *signature_bytes,
                            const uint8_t *digest) {
  assert(curve == &secp256k1);
  if (curve != &secp256k1) {
    return 1;
  }

  int result = 0;

  int public_key_length = 0;

  if (result == 0) {
    if (public_key_bytes[0] == 0x04) {
      public_key_length = 65;
    } else if (public_key_bytes[0] == 0x02 || public_key_bytes[0] == 0x03) {
      public_key_length = 33;
    } else {
      result = 1;
    }
  }

  if (result == 0) {
    if (is_zero_digest(digest)) {
      // The digest was all-zero. The probability of this happening by chance is
      // infinitesimal, but it could be induced by a fault injection. In this
      // case the signature (r,s) can be forged by taking r := (t * Q).x mod n
      // and s := r * t^-1 mod n for any t in [1, n-1]. We fail verification,
      // because there is no guarantee that the signature was created by the
      // owner of the private key.
      result = 3;
    }
  }

  const secp256k1_context *context_read_only = zkp_context_get_read_only();
  secp256k1_pubkey public_key = {0};

  if (result == 0) {
    if (secp256k1_ec_pubkey_parse(context_read_only, &public_key,
                                  public_key_bytes, public_key_length) != 1) {
      result = 1;
    }
  }

  secp256k1_ecdsa_signature signature = {0};

  if (result == 0) {
    if (secp256k1_ecdsa_signature_parse_compact(context_read_only, &signature,
                                                signature_bytes) != 1) {
      result = 2;
    }
  }

  if (result == 0) {
    secp256k1_ecdsa_signature_normalize(context_read_only, &signature,
                                        &signature);

    if (secp256k1_ecdsa_verify(context_read_only, &signature, digest,
                               &public_key) != 1) {
      result = 5;
    }
  }

  memzero(&public_key, sizeof(public_key));
  memzero(&signature, sizeof(signature));

  return result;
}

// ECDSA verification
// curve has to be &secp256k1
// public_key_bytes has 33 or 65 bytes
// signature_bytes has 64 bytes
// returns 0 if verification succeeded
int zkp_ecdsa_verify(const ecdsa_curve *curve, HasherType hasher_type,
                     const uint8_t *public_key_bytes,
                     const uint8_t *signature_bytes, const uint8_t *message,
                     uint32_t message_length) {
  assert(curve == &secp256k1);
  if (curve != &secp256k1) {
    return 1;
  }

  uint8_t hash[32] = {0};
  hasher_Raw(hasher_type, message, message_length, hash);
  int result =
      zkp_ecdsa_verify_digest(curve, public_key_bytes, signature_bytes, hash);
  memzero(hash, sizeof(hash));
  return result;
}
