/**
 * Copyright (c) 2017 Saleem Rashid
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
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, E1PRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES
 * OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#include "nem.h"

#include <string.h>

#include "base32.h"
#include "ed25519-donna/ed25519-keccak.h"
#include "memzero.h"
#include "ripemd160.h"
#include "sha3.h"

#define CAN_WRITE(NEEDED) ((ctx->offset + (NEEDED)) <= ctx->size)

#define SERIALIZE_U32(DATA)                        \
  do {                                             \
    if (!nem_write_u32(ctx, (DATA))) return false; \
  } while (0)
#define SERIALIZE_U64(DATA)                        \
  do {                                             \
    if (!nem_write_u64(ctx, (DATA))) return false; \
  } while (0)
#define SERIALIZE_TAGGED(DATA, LENGTH)                          \
  do {                                                          \
    if (!nem_write_tagged(ctx, (DATA), (LENGTH))) return false; \
  } while (0)

const char *nem_network_name(uint8_t network) {
  switch (network) {
    case NEM_NETWORK_MAINNET:
      return "NEM Mainnet";
    case NEM_NETWORK_TESTNET:
      return "NEM Testnet";
    case NEM_NETWORK_MIJIN:
      return "Mijin";
    default:
      return NULL;
  }
}

static inline bool nem_write_checked(nem_transaction_ctx *ctx,
                                     const uint8_t *data, uint32_t length) {
  if (!CAN_WRITE(length)) {
    return false;
  }

  memcpy(&ctx->buffer[ctx->offset], data, length);
  ctx->offset += length;
  return true;
}

static inline bool nem_write_u32(nem_transaction_ctx *ctx, uint32_t data) {
  if (!CAN_WRITE(4)) {
    return false;
  }

  ctx->buffer[ctx->offset++] = (data >> 0) & 0xff;
  ctx->buffer[ctx->offset++] = (data >> 8) & 0xff;
  ctx->buffer[ctx->offset++] = (data >> 16) & 0xff;
  ctx->buffer[ctx->offset++] = (data >> 24) & 0xff;

  return true;
}

static inline bool nem_write_u64(nem_transaction_ctx *ctx, uint64_t data) {
  SERIALIZE_U32((data >> 0) & 0xffffffff);
  SERIALIZE_U32((data >> 32) & 0xffffffff);

  return true;
}

static inline bool nem_write_tagged(nem_transaction_ctx *ctx,
                                    const uint8_t *data, uint32_t length) {
  SERIALIZE_U32(length);

  return nem_write_checked(ctx, data, length);
}

static inline bool nem_write_mosaic_str(nem_transaction_ctx *ctx,
                                        const char *name, const char *value) {
  uint32_t name_length = strlen(name);
  uint32_t value_length = strlen(value);

  SERIALIZE_U32(sizeof(uint32_t) + name_length + sizeof(uint32_t) +
                value_length);
  SERIALIZE_TAGGED((const uint8_t *)name, name_length);
  SERIALIZE_TAGGED((const uint8_t *)value, value_length);

  return true;
}

static inline bool nem_write_mosaic_bool(nem_transaction_ctx *ctx,
                                         const char *name, bool value) {
  return nem_write_mosaic_str(ctx, name, value ? "true" : "false");
}

static inline bool nem_write_mosaic_u64(nem_transaction_ctx *ctx,
                                        const char *name, uint64_t value) {
  char buffer[21] = {0};

  if (bn_format_uint64(value, NULL, NULL, 0, 0, false, 0, buffer,
                       sizeof(buffer)) == 0) {
    return false;
  }

  return nem_write_mosaic_str(ctx, name, buffer);
}

void nem_get_address_raw(const ed25519_public_key public_key, uint8_t version,
                         uint8_t *address) {
  uint8_t hash[SHA3_256_DIGEST_LENGTH] = {0};

  /* 1.  Perform 256-bit Sha3 on the public key */
  keccak_256(public_key, sizeof(ed25519_public_key), hash);

  /* 2.  Perform 160-bit Ripemd of hash resulting from step 1. */
  ripemd160(hash, SHA3_256_DIGEST_LENGTH, &address[1]);

  /* 3.  Prepend version byte to Ripemd hash (either 0x68 or 0x98) */
  address[0] = version;

  /* 4.  Perform 256-bit Sha3 on the result, take the first four bytes as a
   * checksum */
  keccak_256(address, 1 + RIPEMD160_DIGEST_LENGTH, hash);

  /* 5.  Concatenate output of step 3 and the checksum from step 4 */
  memcpy(&address[1 + RIPEMD160_DIGEST_LENGTH], hash, 4);

  memzero(hash, sizeof(hash));
}

bool nem_get_address(const ed25519_public_key public_key, uint8_t version,
                     char *address) {
  uint8_t pubkeyhash[NEM_ADDRESS_SIZE_RAW] = {0};

  nem_get_address_raw(public_key, version, pubkeyhash);

  char *ret = base32_encode(pubkeyhash, sizeof(pubkeyhash), address,
                            NEM_ADDRESS_SIZE + 1, BASE32_ALPHABET_RFC4648);

  memzero(pubkeyhash, sizeof(pubkeyhash));
  return (ret != NULL);
}

bool nem_validate_address_raw(const uint8_t *address, uint8_t network) {
  if (!nem_network_name(network) || address[0] != network) {
    return false;
  }

  uint8_t hash[SHA3_256_DIGEST_LENGTH] = {0};

  keccak_256(address, 1 + RIPEMD160_DIGEST_LENGTH, hash);
  bool valid = (memcmp(&address[1 + RIPEMD160_DIGEST_LENGTH], hash, 4) == 0);

  memzero(hash, sizeof(hash));
  return valid;
}

bool nem_validate_address(const char *address, uint8_t network) {
  uint8_t pubkeyhash[NEM_ADDRESS_SIZE_RAW] = {0};

  if (strlen(address) != NEM_ADDRESS_SIZE) {
    return false;
  }

  uint8_t *ret = base32_decode(address, NEM_ADDRESS_SIZE, pubkeyhash,
                               sizeof(pubkeyhash), BASE32_ALPHABET_RFC4648);
  bool valid = (ret != NULL) && nem_validate_address_raw(pubkeyhash, network);

  memzero(pubkeyhash, sizeof(pubkeyhash));
  return valid;
}

void nem_transaction_start(nem_transaction_ctx *ctx,
                           const ed25519_public_key public_key, uint8_t *buffer,
                           size_t size) {
  memcpy(ctx->public_key, public_key, sizeof(ctx->public_key));

  ctx->buffer = buffer;
  ctx->offset = 0;
  ctx->size = size;
}

size_t nem_transaction_end(nem_transaction_ctx *ctx,
                           const ed25519_secret_key private_key,
                           ed25519_signature signature) {
  if (private_key != NULL && signature != NULL) {
    ed25519_sign_keccak(ctx->buffer, ctx->offset, private_key, signature);
  }

  return ctx->offset;
}

bool nem_transaction_write_common(nem_transaction_ctx *ctx, uint32_t type,
                                  uint32_t version, uint32_t timestamp,
                                  const ed25519_public_key signer, uint64_t fee,
                                  uint32_t deadline) {
  SERIALIZE_U32(type);
  SERIALIZE_U32(version);
  SERIALIZE_U32(timestamp);
  SERIALIZE_TAGGED(signer, sizeof(ed25519_public_key));
  SERIALIZE_U64(fee);
  SERIALIZE_U32(deadline);

  return true;
}

bool nem_transaction_create_transfer(nem_transaction_ctx *ctx, uint8_t network,
                                     uint32_t timestamp,
                                     const ed25519_public_key signer,
                                     uint64_t fee, uint32_t deadline,
                                     const char *recipient, uint64_t amount,
                                     const uint8_t *payload, uint32_t length,
                                     bool encrypted, uint32_t mosaics) {
  if (!signer) {
    signer = ctx->public_key;
  }

  if (!payload) {
    length = 0;
  }

  bool ret =
      nem_transaction_write_common(ctx, NEM_TRANSACTION_TYPE_TRANSFER,
                                   (uint32_t)network << 24 | (mosaics ? 2 : 1),
                                   timestamp, signer, fee, deadline);
  if (!ret) return false;

  SERIALIZE_TAGGED((const uint8_t *)recipient, NEM_ADDRESS_SIZE);
  SERIALIZE_U64(amount);

  if (length) {
    SERIALIZE_U32(sizeof(uint32_t) + sizeof(uint32_t) + length);
    SERIALIZE_U32(encrypted ? 0x02 : 0x01);
    SERIALIZE_TAGGED(payload, length);
  } else {
    SERIALIZE_U32(0);
  }

  if (mosaics) {
    SERIALIZE_U32(mosaics);
  }

  return true;
}

bool nem_transaction_write_mosaic(nem_transaction_ctx *ctx,
                                  const char *namespace, const char *mosaic,
                                  uint64_t quantity) {
  size_t namespace_length = strlen(namespace);
  size_t mosaic_length = strlen(mosaic);
  size_t identifier_length =
      sizeof(uint32_t) + namespace_length + sizeof(uint32_t) + mosaic_length;

  SERIALIZE_U32(sizeof(uint32_t) + sizeof(uint64_t) + identifier_length);
  SERIALIZE_U32(identifier_length);
  SERIALIZE_TAGGED((const uint8_t *)namespace, namespace_length);
  SERIALIZE_TAGGED((const uint8_t *)mosaic, mosaic_length);
  SERIALIZE_U64(quantity);

  return true;
}

bool nem_transaction_create_multisig(nem_transaction_ctx *ctx, uint8_t network,
                                     uint32_t timestamp,
                                     const ed25519_public_key signer,
                                     uint64_t fee, uint32_t deadline,
                                     const nem_transaction_ctx *inner) {
  if (!signer) {
    signer = ctx->public_key;
  }

  bool ret = nem_transaction_write_common(ctx, NEM_TRANSACTION_TYPE_MULTISIG,
                                          (uint32_t)network << 24 | 1,
                                          timestamp, signer, fee, deadline);
  if (!ret) return false;

  SERIALIZE_TAGGED(inner->buffer, inner->offset);

  return true;
}

bool nem_transaction_create_multisig_signature(
    nem_transaction_ctx *ctx, uint8_t network, uint32_t timestamp,
    const ed25519_public_key signer, uint64_t fee, uint32_t deadline,
    const nem_transaction_ctx *inner) {
  if (!signer) {
    signer = ctx->public_key;
  }

  bool ret = nem_transaction_write_common(
      ctx, NEM_TRANSACTION_TYPE_MULTISIG_SIGNATURE, (uint32_t)network << 24 | 1,
      timestamp, signer, fee, deadline);
  if (!ret) return false;

  char address[NEM_ADDRESS_SIZE + 1] = {0};
  nem_get_address(inner->public_key, network, address);

  uint8_t hash[SHA3_256_DIGEST_LENGTH] = {0};
  keccak_256(inner->buffer, inner->offset, hash);

  SERIALIZE_U32(sizeof(uint32_t) + SHA3_256_DIGEST_LENGTH);
  SERIALIZE_TAGGED(hash, SHA3_256_DIGEST_LENGTH);
  SERIALIZE_TAGGED((const uint8_t *)address, NEM_ADDRESS_SIZE);

  return true;
}

bool nem_transaction_create_provision_namespace(
    nem_transaction_ctx *ctx, uint8_t network, uint32_t timestamp,
    const ed25519_public_key signer, uint64_t fee, uint32_t deadline,
    const char *namespace, const char *parent, const char *rental_sink,
    uint64_t rental_fee) {
  if (!signer) {
    signer = ctx->public_key;
  }

  bool ret = nem_transaction_write_common(
      ctx, NEM_TRANSACTION_TYPE_PROVISION_NAMESPACE,
      (uint32_t)network << 24 | 1, timestamp, signer, fee, deadline);
  if (!ret) return false;

  if (parent) {
    SERIALIZE_TAGGED((const uint8_t *)rental_sink, NEM_ADDRESS_SIZE);
    SERIALIZE_U64(rental_fee);
    SERIALIZE_TAGGED((const uint8_t *)namespace, strlen(namespace));
    SERIALIZE_TAGGED((const uint8_t *)parent, strlen(parent));
  } else {
    SERIALIZE_TAGGED((const uint8_t *)rental_sink, NEM_ADDRESS_SIZE);
    SERIALIZE_U64(rental_fee);
    SERIALIZE_TAGGED((const uint8_t *)namespace, strlen(namespace));
    SERIALIZE_U32(0xffffffff);
  }

  return true;
}

bool nem_transaction_create_mosaic_creation(
    nem_transaction_ctx *ctx, uint8_t network, uint32_t timestamp,
    const ed25519_public_key signer, uint64_t fee, uint32_t deadline,
    const char *namespace, const char *mosaic, const char *description,
    uint32_t divisibility, uint64_t supply, bool mutable_supply,
    bool transferable, uint32_t levy_type, uint64_t levy_fee,
    const char *levy_address, const char *levy_namespace,
    const char *levy_mosaic, const char *creation_sink, uint64_t creation_fee) {
  if (!signer) {
    signer = ctx->public_key;
  }

  bool ret = nem_transaction_write_common(
      ctx, NEM_TRANSACTION_TYPE_MOSAIC_CREATION, (uint32_t)network << 24 | 1,
      timestamp, signer, fee, deadline);
  if (!ret) return false;

  size_t namespace_length = strlen(namespace);
  size_t mosaic_length = strlen(mosaic);
  size_t identifier_length =
      sizeof(uint32_t) + namespace_length + sizeof(uint32_t) + mosaic_length;

  // This length will be rewritten later on
  nem_transaction_ctx state = {0};
  memcpy(&state, ctx, sizeof(state));

  SERIALIZE_U32(0);
  SERIALIZE_TAGGED(signer, sizeof(ed25519_public_key));
  SERIALIZE_U32(identifier_length);
  SERIALIZE_TAGGED((const uint8_t *)namespace, namespace_length);
  SERIALIZE_TAGGED((const uint8_t *)mosaic, mosaic_length);
  SERIALIZE_TAGGED((const uint8_t *)description, strlen(description));
  SERIALIZE_U32(4);  // Number of properties

  if (!nem_write_mosaic_u64(ctx, "divisibility", divisibility)) return false;
  if (!nem_write_mosaic_u64(ctx, "initialSupply", supply)) return false;
  if (!nem_write_mosaic_bool(ctx, "supplyMutable", mutable_supply))
    return false;
  if (!nem_write_mosaic_bool(ctx, "transferable", transferable)) return false;

  if (levy_type) {
    size_t levy_namespace_length = strlen(levy_namespace);
    size_t levy_mosaic_length = strlen(levy_mosaic);
    size_t levy_identifier_length = sizeof(uint32_t) + levy_namespace_length +
                                    sizeof(uint32_t) + levy_mosaic_length;

    SERIALIZE_U32(sizeof(uint32_t) + sizeof(uint32_t) + NEM_ADDRESS_SIZE +
                  sizeof(uint32_t) + levy_identifier_length + sizeof(uint64_t));
    SERIALIZE_U32(levy_type);
    SERIALIZE_TAGGED((const uint8_t *)levy_address, NEM_ADDRESS_SIZE);
    SERIALIZE_U32(levy_identifier_length);
    SERIALIZE_TAGGED((const uint8_t *)levy_namespace, levy_namespace_length);
    SERIALIZE_TAGGED((const uint8_t *)levy_mosaic, levy_mosaic_length);
    SERIALIZE_U64(levy_fee);
  } else {
    SERIALIZE_U32(0);
  }

  // Rewrite length
  nem_write_u32(&state, ctx->offset - state.offset - sizeof(uint32_t));

  SERIALIZE_TAGGED((const uint8_t *)creation_sink, NEM_ADDRESS_SIZE);
  SERIALIZE_U64(creation_fee);

  return true;
}

bool nem_transaction_create_mosaic_supply_change(
    nem_transaction_ctx *ctx, uint8_t network, uint32_t timestamp,
    const ed25519_public_key signer, uint64_t fee, uint32_t deadline,
    const char *namespace, const char *mosaic, uint32_t type, uint64_t delta) {
  if (!signer) {
    signer = ctx->public_key;
  }

  bool ret = nem_transaction_write_common(
      ctx, NEM_TRANSACTION_TYPE_MOSAIC_SUPPLY_CHANGE,
      (uint32_t)network << 24 | 1, timestamp, signer, fee, deadline);
  if (!ret) return false;

  size_t namespace_length = strlen(namespace);
  size_t mosaic_length = strlen(mosaic);
  size_t identifier_length =
      sizeof(uint32_t) + namespace_length + sizeof(uint32_t) + mosaic_length;

  SERIALIZE_U32(identifier_length);
  SERIALIZE_TAGGED((const uint8_t *)namespace, namespace_length);
  SERIALIZE_TAGGED((const uint8_t *)mosaic, mosaic_length);
  SERIALIZE_U32(type);
  SERIALIZE_U64(delta);

  return true;
}

bool nem_transaction_create_aggregate_modification(
    nem_transaction_ctx *ctx, uint8_t network, uint32_t timestamp,
    const ed25519_public_key signer, uint64_t fee, uint32_t deadline,
    uint32_t modifications, bool relative_change) {
  if (!signer) {
    signer = ctx->public_key;
  }

  bool ret = nem_transaction_write_common(
      ctx, NEM_TRANSACTION_TYPE_AGGREGATE_MODIFICATION,
      (uint32_t)network << 24 | (relative_change ? 2 : 1), timestamp, signer,
      fee, deadline);
  if (!ret) return false;

  SERIALIZE_U32(modifications);

  return true;
}

bool nem_transaction_write_cosignatory_modification(
    nem_transaction_ctx *ctx, uint32_t type,
    const ed25519_public_key cosignatory) {
  SERIALIZE_U32(sizeof(uint32_t) + sizeof(uint32_t) +
                sizeof(ed25519_public_key));
  SERIALIZE_U32(type);
  SERIALIZE_TAGGED(cosignatory, sizeof(ed25519_public_key));

  return true;
}

bool nem_transaction_write_minimum_cosignatories(nem_transaction_ctx *ctx,
                                                 int32_t relative_change) {
  SERIALIZE_U32(sizeof(uint32_t));
  SERIALIZE_U32((uint32_t)relative_change);

  return true;
}

bool nem_transaction_create_importance_transfer(
    nem_transaction_ctx *ctx, uint8_t network, uint32_t timestamp,
    const ed25519_public_key signer, uint64_t fee, uint32_t deadline,
    uint32_t mode, const ed25519_public_key remote) {
  if (!signer) {
    signer = ctx->public_key;
  }

  bool ret = nem_transaction_write_common(
      ctx, NEM_TRANSACTION_TYPE_IMPORTANCE_TRANSFER,
      (uint32_t)network << 24 | 1, timestamp, signer, fee, deadline);
  if (!ret) return false;

  SERIALIZE_U32(mode);
  SERIALIZE_TAGGED(remote, sizeof(ed25519_public_key));

  return true;
}
