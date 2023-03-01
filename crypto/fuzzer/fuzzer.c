/**
 * Copyright (c) 2020-2022 Christian Reitter
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

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// includes for potential target functions
// based on test_check.c
#include "address.h"
#include "aes/aes.h"
#include "base32.h"
#include "base58.h"
#include "bignum.h"
#include "bip32.h"
#include "bip39.h"
#include "blake256.h"
#include "blake2b.h"
#include "blake2s.h"
#include "chacha_drbg.h"
#include "curves.h"
#include "ecdsa.h"
#include "ed25519-donna/ed25519-donna.h"
#include "ed25519-donna/ed25519-keccak.h"
#include "ed25519-donna/ed25519.h"
#include "hasher.h"
#include "hmac_drbg.h"
#include "memzero.h"
#include "monero/monero.h"
#include "nem.h"
#include "nist256p1.h"
#include "pbkdf2.h"
#include "rand.h"
#include "rc4.h"
#include "rfc6979.h"
#include "script.h"
#include "secp256k1.h"
#include "segwit_addr.h"
#include "sha2.h"
#include "sha3.h"
#include "shamir.h"
#include "slip39.h"
#include "slip39_wordlist.h"
#include "zkp_bip340.h"
#include "zkp_context.h"
#include "zkp_ecdsa.h"

#if defined(__has_feature)
#if __has_feature(memory_sanitizer)
#include <sanitizer/msan_interface.h>
#endif
#endif

/* code design notes
 *
 * TODO note down design tradeoffs for this fuzzer style
 */

/* code performance notes
 *
 * use #define over runtime checks for performance reasons
 * avoid VLA arrays for performance reasons
 * potential performance drawbacks of heap usage are accepted for better out of
 * bounds error detection some expensive functions are hidden with compile-time
 * switches fuzzer harnesses are meant to exit early if the preconditions are
 * not met
 */

/* fuzzer input data handling */
const uint8_t *fuzzer_ptr;
size_t fuzzer_length;

const uint8_t *fuzzer_input(size_t len) {
  if (fuzzer_length < len) {
    fuzzer_length = 0;
    return NULL;
  }
  const uint8_t *result = fuzzer_ptr;
  fuzzer_length -= len;
  fuzzer_ptr += len;
  return result;
}

/* fuzzer state handling */
void fuzzer_reset_state(void) {
  // reset the PRNGs to make individual fuzzer runs deterministic
  srand(0);
  random_reseed(0);

  // clear internal caches
  // note: this is not strictly required for all fuzzer targets
#if USE_BIP32_CACHE
  bip32_cache_clear();
#endif
#if USE_BIP39_CACHE
  bip39_cache_clear();
#endif
}

void crash(void) {
  // intentionally exit the program
  // the fuzzer framework treats this as a crash
  exit(1);
}

// IDEA are there advantages to turning this into a macro?
//
// check the memory area for memory information leaks if MSAN is available,
// crash if problems are detected
void check_msan(void *pointer, size_t length) {
#if defined(__has_feature)
#if __has_feature(memory_sanitizer)
  // check `address` for memory info leakage
  __msan_check_mem_is_initialized(pointer, length);
#else
  // ignore if MSan is not enabled
  (void)pointer;
  (void)length;
#endif
#else
  // ignore if the compiler does not know __has_feature()
  (void)pointer;
  (void)length;
#endif
}

// simplify the pointer check after a var_pointer = malloc()
// return -1 to mark fuzz input as uninteresting for the fuzz engine
// warning: use only if no manual memory cleanup is needed
#define RETURN_IF_NULL(var_pointer) \
  if (var_pointer == NULL) {        \
    return -1;                      \
  }

void zkp_initialize_context_or_crash(void) {
  // The current context usage has persistent side effects
  // TODO switch to frequent re-initialization where necessary
  if (!zkp_context_is_initialized()) {
    if (zkp_context_init() != 0) {
      crash();
    }
  }
}

/* individual fuzzer harness functions */

int fuzz_bn_format(void) {
  bignum256 target_bignum;
  // we need some amount of initial data
  if (fuzzer_length < sizeof(target_bignum) + 1 + 1) {
    return -1;
  }

#define FUZZ_BN_FORMAT_OUTPUT_BUFFER_SIZE 512
  char buf[FUZZ_BN_FORMAT_OUTPUT_BUFFER_SIZE] = {0};
  int ret = 0;

  // mutate the struct contents
  memcpy(&target_bignum, fuzzer_input(sizeof(target_bignum)),
         sizeof(target_bignum));

  uint8_t prefixlen = 0;
  uint8_t suffixlen = 0;
  uint32_t decimals = 0;
  int32_t exponent = 0;
  bool trailing = false;
  // range 1 to 128
  prefixlen = (fuzzer_input(1)[0] & 127) + 1;
  suffixlen = (fuzzer_input(1)[0] & 127) + 1;

  // check for the second half of the data
  if (fuzzer_length < (size_t)(prefixlen + suffixlen + 4 + 4 + 1 - 2)) {
    return -1;
  }
  memcpy(&decimals, fuzzer_input(4), 4);
  memcpy(&exponent, fuzzer_input(4), 4);
  trailing = (fuzzer_input(1)[0] & 1);

  // IDEA allow prefix == NULL
  char *prefix = malloc(prefixlen);
  RETURN_IF_NULL(prefix);
  // IDEA allow suffix == NULL
  char *suffix = malloc(suffixlen);
  if (suffix == NULL) {
    free(prefix);
    return -1;
  }

  memset(prefix, 0, prefixlen);
  memset(suffix, 0, suffixlen);
  // only fetch up to (length - 1) to ensure null termination together with the
  // memset
  memcpy(prefix, fuzzer_input(prefixlen - 1), prefixlen - 1);
  memcpy(suffix, fuzzer_input(suffixlen - 1), suffixlen - 1);

  ret = bn_format(&target_bignum, prefix, suffix, decimals, exponent, trailing,
                  0, buf, FUZZ_BN_FORMAT_OUTPUT_BUFFER_SIZE);

  // basic sanity checks for the return values
  if (ret > FUZZ_BN_FORMAT_OUTPUT_BUFFER_SIZE) {
    crash();
  }

  check_msan(&buf, FUZZ_BN_FORMAT_OUTPUT_BUFFER_SIZE);

  free(prefix);
  free(suffix);
  return 0;
}

// arbitrarily chosen maximum size
#define BASE32_DECODE_MAX_INPUT_LEN 512

int fuzz_base32_decode(void) {
  if (fuzzer_length < 2 || fuzzer_length > BASE32_DECODE_MAX_INPUT_LEN) {
    return -1;
  }

  char *in_buffer = malloc(fuzzer_length);
  RETURN_IF_NULL(in_buffer);
  // basic heuristic: the decoded output will always fit in less or equal space
  uint8_t *out_buffer = malloc(fuzzer_length);
  if (out_buffer == NULL) {
    free(in_buffer);
    return -1;
  }

  size_t outlen = fuzzer_length;
  size_t raw_inlen = fuzzer_length;
  memcpy(in_buffer, fuzzer_input(raw_inlen), raw_inlen);

  // null-terminate input buffer to prevent issues with strlen()
  in_buffer[raw_inlen - 1] = 0;

  uint8_t *ret = base32_decode(in_buffer, raw_inlen, out_buffer, outlen,
                               BASE32_ALPHABET_RFC4648);

  if (ret != NULL) {
    check_msan(out_buffer, outlen);
  }
  free(in_buffer);
  free(out_buffer);
  return 0;
}

// arbitrarily chosen maximum size
#define BASE32_ENCODE_MAX_INPUT_LEN 512

int fuzz_base32_encode(void) {
  if (fuzzer_length > BASE32_ENCODE_MAX_INPUT_LEN) {
    return -1;
  }

  uint8_t *in_buffer = malloc(fuzzer_length);
  RETURN_IF_NULL(in_buffer);
  // TODO: find a better heuristic for output buffer size
  size_t outlen = 2 * fuzzer_length;
  char *out_buffer = malloc(outlen);
  if (out_buffer == NULL) {
    free(in_buffer);
    return -1;
  }

  // mutate in_buffer
  size_t raw_inlen = fuzzer_length;
  memcpy(in_buffer, fuzzer_ptr, raw_inlen);
  fuzzer_input(raw_inlen);

  char *ret = base32_encode(in_buffer, raw_inlen, out_buffer, outlen,
                            BASE32_ALPHABET_RFC4648);

  if (ret != NULL) {
    // the return value is a pointer to the end of the written buffer,
    // use it to calculate the used buffer area
    check_msan(out_buffer, ret - out_buffer);
  }
  free(in_buffer);
  free(out_buffer);
  return 0;
}

// internal limit is 128, try some extra bytes
#define BASE58_ENCODE_MAX_INPUT_LEN 140

int fuzz_base58_encode_check(void) {
  if (fuzzer_length > BASE58_ENCODE_MAX_INPUT_LEN) {
    return -1;
  }

  uint8_t *in_buffer = malloc(fuzzer_length);
  RETURN_IF_NULL(in_buffer);
  // TODO: find a better heuristic for output buffer size
  size_t outlen = 2 * fuzzer_length;
  char *out_buffer = malloc(outlen);
  if (out_buffer == NULL) {
    free(in_buffer);
    return -1;
  }

  // mutate in_buffer
  size_t raw_inlen = fuzzer_length;
  memcpy(in_buffer, fuzzer_input(raw_inlen), raw_inlen);

  int ret = 0;
  // run multiple hasher variants for the same input
  base58_encode_check(in_buffer, raw_inlen, HASHER_SHA2D, out_buffer, outlen);
  base58_encode_check(in_buffer, raw_inlen, HASHER_BLAKED, out_buffer, outlen);
  base58_encode_check(in_buffer, raw_inlen, HASHER_GROESTLD_TRUNC, out_buffer,
                      outlen);
  ret = base58_encode_check(in_buffer, raw_inlen, HASHER_SHA3K, out_buffer,
                            outlen);

  // check one of the encode results
  if (ret != 0) {
    // the return value describes how many characters are written
    check_msan(out_buffer, ret);
  }

  free(in_buffer);
  free(out_buffer);
  return 0;
}

// internal limit is 128, try some extra bytes
#define BASE58_DECODE_MAX_INPUT_LEN 140

int fuzz_base58_decode_check(void) {
  if (fuzzer_length > BASE58_DECODE_MAX_INPUT_LEN) {
    return -1;
  }

  uint8_t *in_buffer = malloc(fuzzer_length + 1);
  RETURN_IF_NULL(in_buffer);

  size_t raw_inlen = fuzzer_length;
  memcpy(in_buffer, fuzzer_input(raw_inlen), raw_inlen);
  uint8_t out_buffer[MAX_ADDR_RAW_SIZE] = {0};
  // force null-termination
  in_buffer[raw_inlen] = 0;
  const char *in_char = (const char *)in_buffer;

  // run multiple hasher variants for the same input
  base58_decode_check(in_char, HASHER_SHA2D, out_buffer, MAX_ADDR_RAW_SIZE);
  base58_decode_check(in_char, HASHER_BLAKED, out_buffer, MAX_ADDR_RAW_SIZE);
  base58_decode_check(in_char, HASHER_GROESTLD_TRUNC, out_buffer,
                      MAX_ADDR_RAW_SIZE);
  base58_decode_check(in_char, HASHER_SHA3K, out_buffer, MAX_ADDR_RAW_SIZE);

  check_msan(out_buffer, MAX_ADDR_RAW_SIZE);

  free(in_buffer);
  return 0;
}

// arbitrarily chosen maximum size meant to limit input complexity
// there is no input size limit for the target function
#define XMR_BASE58_ADDR_DECODE_MAX_INPUT_LEN 512

int fuzz_xmr_base58_addr_decode_check(void) {
  if (fuzzer_length > XMR_BASE58_ADDR_DECODE_MAX_INPUT_LEN) {
    return -1;
  }

  // TODO no null termination used !?
  // TODO use better size heuristic
  size_t outlen = fuzzer_length;
  char *in_buffer = malloc(fuzzer_length);
  RETURN_IF_NULL(in_buffer);
  uint8_t *out_buffer = malloc(outlen);
  if (out_buffer == NULL) {
    free(in_buffer);
    return -1;
  }

  // tag is only written to
  uint64_t tag = 0;
  size_t raw_inlen = fuzzer_length;
  // mutate in_buffer
  memcpy(in_buffer, fuzzer_input(raw_inlen), raw_inlen);

  int ret = xmr_base58_addr_decode_check(in_buffer, raw_inlen, &tag, out_buffer,
                                         outlen);

  // IDEA check tag for expected values?
  // IDEA re-encode valid decoding results to check function consistency?

  if (ret != 0) {
    check_msan(out_buffer, outlen);
  }

  free(in_buffer);
  free(out_buffer);
  return 0;
}

// arbitrarily chosen maximum size
#define XMR_BASE58_DECODE_MAX_INPUT_LEN 512
// a more focused variant of the xmr_base58_addr_decode_check() harness
int fuzz_xmr_base58_decode(void) {
  if (fuzzer_length > XMR_BASE58_DECODE_MAX_INPUT_LEN) {
    return -1;
  }

  // TODO better size heuristic
  size_t outlen = fuzzer_length;
  char *in_buffer = malloc(fuzzer_length);
  RETURN_IF_NULL(in_buffer);
  uint8_t *out_buffer = malloc(outlen);
  if (out_buffer == NULL) {
    free(in_buffer);
    return -1;
  }

  memset(out_buffer, 0, outlen);

  // mutate in_buffer
  size_t raw_inlen = fuzzer_length;
  memcpy(in_buffer, fuzzer_input(raw_inlen), raw_inlen);

  xmr_base58_decode(in_buffer, raw_inlen, out_buffer, &outlen);

  free(in_buffer);
  free(out_buffer);
  return 0;
}

// arbitrarily chosen maximum size
#define XMR_BASE58_ADDR_ENCODE_MAX_INPUT_LEN 140

int fuzz_xmr_base58_addr_encode_check(void) {
  // tag_in is internally limited
  uint8_t tag_in;
  int ret1 = 0;
  size_t tag_size = sizeof(tag_in);
  if (fuzzer_length < tag_size + 1 ||
      fuzzer_length > XMR_BASE58_ADDR_ENCODE_MAX_INPUT_LEN) {
    return -1;
  }

  // mutate tag_in
  memcpy(&tag_in, fuzzer_input(tag_size), tag_size);

  // TODO better size heuristic
  size_t outlen = fuzzer_length * 2;
  uint8_t *in_buffer = malloc(fuzzer_length);
  RETURN_IF_NULL(in_buffer);
  char *out_buffer = malloc(outlen);
  if (out_buffer == NULL) {
    free(in_buffer);
    return -1;
  }

  memset(out_buffer, 0, outlen);

  // mutate in_buffer
  size_t raw_inlen = fuzzer_length;
  memcpy(in_buffer, fuzzer_input(raw_inlen), raw_inlen);

  ret1 = xmr_base58_addr_encode_check(tag_in, in_buffer, raw_inlen, out_buffer,
                                      outlen);

  if (ret1 != 0) {
    // encoding successful
    uint64_t second_tag = 0;
    // TODO improve length
    uint8_t dummy_buffer[XMR_BASE58_ADDR_ENCODE_MAX_INPUT_LEN] = {0};
    int ret2 = 0;
    // ret1 represents the actual length of the encoded string
    // this is important for the decode function to succeed
    ret2 = xmr_base58_addr_decode_check(out_buffer, ret1, &second_tag,
                                        dummy_buffer, sizeof(dummy_buffer));
    // the tag comparison is between unequal types, but this is acceptable here
    if (ret2 == 0 || tag_in != second_tag) {
      crash();
    }
  }

  free(in_buffer);
  free(out_buffer);
  return 0;
}

// arbitrarily chosen maximum size
#define XMR_BASE58_ENCODE_MAX_INPUT_LEN 512
// a more focused variant of the xmr_base58_addr_encode_check() harness
int fuzz_xmr_base58_encode(void) {
  if (fuzzer_length > XMR_BASE58_ENCODE_MAX_INPUT_LEN) {
    return -1;
  }

  // TODO better size heuristic
  size_t outlen = fuzzer_length * 2;
  uint8_t *in_buffer = malloc(fuzzer_length);
  RETURN_IF_NULL(in_buffer);
  char *out_buffer = malloc(outlen);
  if (out_buffer == NULL) {
    free(in_buffer);
    return -1;
  }

  memset(out_buffer, 0, outlen);

  // mutate in_buffer
  size_t raw_inlen = fuzzer_length;
  memcpy(in_buffer, fuzzer_input(raw_inlen), raw_inlen);

  xmr_base58_encode(out_buffer, &outlen, in_buffer, raw_inlen);

  free(in_buffer);
  free(out_buffer);
  return 0;
}

int fuzz_xmr_serialize_varint(void) {
// arbitrarily chosen maximum size
#define XMR_SERIALIZE_VARINT_MAX_INPUT_LEN 128

  uint64_t varint_in;
  size_t varint_in_size = sizeof(varint_in);
  if (fuzzer_length <= varint_in_size ||
      fuzzer_length > XMR_SERIALIZE_VARINT_MAX_INPUT_LEN) {
    return -1;
  }

  uint8_t out_buffer[XMR_SERIALIZE_VARINT_MAX_INPUT_LEN] = {0};
  size_t outlen = sizeof(out_buffer);
  uint64_t varint_out = 0;

  // mutate varint_in
  memcpy(&varint_in, fuzzer_input(varint_in_size), varint_in_size);

  // mutate in_buffer
  size_t raw_inlen = fuzzer_length;
  uint8_t *in_buffer = malloc(raw_inlen);
  RETURN_IF_NULL(in_buffer);
  memcpy(in_buffer, fuzzer_input(raw_inlen), raw_inlen);

  // use the varint
  xmr_size_varint(varint_in);
  xmr_write_varint(out_buffer, outlen, varint_in);

  // use the input buffer
  xmr_read_varint(in_buffer, raw_inlen, &varint_out);

  // IDEA cross-check write/read results

  free(in_buffer);
  return 0;
}

// arbitrarily chosen maximum size
#define NEM_VALIDATE_ADDRESS_MAX_INPUT_LEN 128

int fuzz_nem_validate_address(void) {
  if (fuzzer_length < 1 || fuzzer_length > NEM_VALIDATE_ADDRESS_MAX_INPUT_LEN) {
    return -1;
  }

  uint8_t network = fuzzer_input(1)[0];
  size_t raw_inlen = fuzzer_length + 1;
  char *in_buffer = malloc(raw_inlen);
  RETURN_IF_NULL(in_buffer);

  // mutate the buffer
  memcpy(in_buffer, fuzzer_input(raw_inlen - 1), raw_inlen - 1);
  // force null-termination
  in_buffer[raw_inlen - 1] = 0;

  nem_validate_address(in_buffer, network);

  free(in_buffer);
  return 0;
}

int fuzz_nem_get_address(void) {
  unsigned char ed25519_public_key_fuzz[32] = {0};
  uint8_t version = 0;

  // TODO switch to < comparison?
  if (fuzzer_length != (sizeof(ed25519_public_key_fuzz) + sizeof(version))) {
    return -1;
  }

  char address[NEM_ADDRESS_SIZE + 1] = {0};

  memcpy(ed25519_public_key_fuzz, fuzzer_input(sizeof(ed25519_public_key_fuzz)),
         sizeof(ed25519_public_key_fuzz));
  memcpy(&version, fuzzer_input(sizeof(version)), sizeof(version));

  nem_get_address(ed25519_public_key_fuzz, version, address);

  check_msan(&address, sizeof(address));
  return 0;
}

int fuzz_xmr_get_subaddress_secret_key(void) {
  bignum256modm m = {0};
  uint32_t major = 0;
  uint32_t minor = 0;
  if (fuzzer_length != (sizeof(bignum256modm) + 2 * sizeof(uint32_t))) {
    return -1;
  }

  bignum256modm output = {0};

  memcpy(m, fuzzer_input(sizeof(bignum256modm)), sizeof(bignum256modm));
  memcpy(&major, fuzzer_input(sizeof(uint32_t)), sizeof(uint32_t));
  memcpy(&minor, fuzzer_input(sizeof(uint32_t)), sizeof(uint32_t));

  xmr_get_subaddress_secret_key(output, major, minor, m);

  check_msan(&output, sizeof(output));
  return 0;
}

int fuzz_xmr_derive_private_key(void) {
  bignum256modm base = {0};
  ge25519 deriv = {0};
  uint32_t idx = 0;

  if (fuzzer_length !=
      (sizeof(bignum256modm) + sizeof(ge25519) + sizeof(uint32_t))) {
    return -1;
  }

  memcpy(base, fuzzer_input(sizeof(bignum256modm)), sizeof(bignum256modm));
  memcpy(&deriv, fuzzer_input(sizeof(ge25519)), sizeof(ge25519));
  memcpy(&idx, fuzzer_input(sizeof(uint32_t)), sizeof(uint32_t));

  bignum256modm output = {0};

  xmr_derive_private_key(output, &deriv, idx, base);
  check_msan(&output, sizeof(output));
  return 0;
}

int fuzz_xmr_derive_public_key(void) {
  ge25519 base = {0};
  ge25519 deriv = {0};
  uint32_t idx = 0;

  if (fuzzer_length != (2 * sizeof(ge25519) + sizeof(uint32_t))) {
    return -1;
  }

  memcpy(&base, fuzzer_input(sizeof(ge25519)), sizeof(ge25519));
  memcpy(&deriv, fuzzer_input(sizeof(ge25519)), sizeof(ge25519));
  memcpy(&idx, fuzzer_input(sizeof(uint32_t)), sizeof(uint32_t));

  ge25519 output = {0};

  xmr_derive_public_key(&output, &deriv, idx, &base);
  check_msan(&output, sizeof(output));
  return 0;
}

#define SHAMIR_MAX_SHARE_COUNT 16
#define SHAMIR_MAX_DATA_LEN (SHAMIR_MAX_SHARE_COUNT * SHAMIR_MAX_LEN)
int fuzz_shamir_interpolate(void) {
  if (fuzzer_length != (2 * sizeof(uint8_t) + SHAMIR_MAX_SHARE_COUNT +
                        SHAMIR_MAX_DATA_LEN + sizeof(size_t))) {
    return -1;
  }

  uint8_t result[SHAMIR_MAX_LEN] = {0};
  uint8_t result_index = 0;
  uint8_t share_indices[SHAMIR_MAX_SHARE_COUNT] = {0};
  uint8_t share_values_content[SHAMIR_MAX_SHARE_COUNT][SHAMIR_MAX_LEN] = {0};
  const uint8_t *share_values[SHAMIR_MAX_SHARE_COUNT] = {0};
  uint8_t share_count = 0;
  size_t len = 0;

  for (size_t i = 0; i < SHAMIR_MAX_SHARE_COUNT; i++) {
    share_values[i] = share_values_content[i];
  }

  memcpy(&result_index, fuzzer_input(sizeof(uint8_t)), sizeof(uint8_t));
  memcpy(&share_indices, fuzzer_input(SHAMIR_MAX_SHARE_COUNT),
         SHAMIR_MAX_SHARE_COUNT);
  memcpy(&share_values_content, fuzzer_input(SHAMIR_MAX_DATA_LEN),
         SHAMIR_MAX_DATA_LEN);
  memcpy(&share_count, fuzzer_input(sizeof(uint8_t)), sizeof(uint8_t));
  // note: this is platform specific via byte length of size_t
  memcpy(&len, fuzzer_input(sizeof(size_t)), sizeof(size_t));

  // mirror a check that the real code does
  if (share_count < 1 || share_count > SHAMIR_MAX_SHARE_COUNT) {
    return 0;
  }
  // (len > SHAMIR_MAX_LEN) is handled in the target function

  shamir_interpolate(result, result_index, share_indices, share_values,
                     share_count, len);
  check_msan(&result, sizeof(result));
  return 0;
}

int fuzz_ecdsa_sign_digest_functions(void) {
  // bug result reference: https://github.com/trezor/trezor-firmware/pull/1697

  uint8_t curve_decider = 0;
  uint8_t priv_key[32] = {0};
  uint8_t digest[32] = {0};

  uint8_t sig1[64] = {0};
  uint8_t sig2[64] = {0};
  uint8_t pby1, pby2 = 0;
  if (fuzzer_length < 1 + sizeof(priv_key) + sizeof(digest)) {
    return -1;
  }
  const ecdsa_curve *curve;

  memcpy(&curve_decider, fuzzer_input(1), 1);
  memcpy(&priv_key, fuzzer_input(sizeof(priv_key)), sizeof(priv_key));
  memcpy(&digest, fuzzer_input(sizeof(digest)), sizeof(digest));

  // pick one of the standard curves
  if ((curve_decider & 0x1) == 1) {
    curve = &secp256k1;
  } else {
    curve = &nist256p1;
  }

  int res = 0;

  // IDEA optionally set a function for is_canonical() callback
  int res1 = ecdsa_sign_digest(curve, priv_key, digest, sig1, &pby1, NULL);

  // the zkp function variant is only defined for a specific curve
  if (curve == &secp256k1) {
    int res2 =
        zkp_ecdsa_sign_digest(curve, priv_key, digest, sig2, &pby2, NULL);
    if ((res1 == 0 && res2 != 0) || (res1 != 0 && res2 == 0)) {
      // one variant succeeded where the other did not
      crash();
    }
    if (res1 == 0 && res2 == 0) {
      if ((pby1 != pby2) || memcmp(&sig1, &sig2, sizeof(sig1)) != 0) {
        // result values are different
        crash();
      }
    }
  }

  // successful signing
  if (res1 == 0) {
    uint8_t pub_key[33] = {0};
    res = ecdsa_get_public_key33(curve, priv_key, pub_key);
    if (res != 0) {
      // pubkey derivation did not succeed
      crash();
    }

    res = ecdsa_verify_digest(curve, pub_key, sig1, digest);
    if (res != 0) {
      // verification did not succeed
      crash();
    }
  }
  return 0;
}

int fuzz_ecdsa_verify_digest_functions(void) {
  uint8_t curve_decider = 0;
  uint8_t hash[32] = {0};
  uint8_t sig[64] = {0};
  uint8_t pub_key[65] = {0};

  if (fuzzer_length < 1 + sizeof(hash) + sizeof(sig) + sizeof(pub_key)) {
    return -1;
  }

  memcpy(&curve_decider, fuzzer_input(1), 1);
  memcpy(&hash, fuzzer_input(sizeof(hash)), sizeof(hash));
  memcpy(&sig, fuzzer_input(sizeof(sig)), sizeof(sig));
  memcpy(&pub_key, fuzzer_input(sizeof(pub_key)), sizeof(pub_key));

  const ecdsa_curve *curve;
  // pick one of the standard curves
  if ((curve_decider & 0x1) == 1) {
    curve = &secp256k1;
  } else {
    curve = &nist256p1;
  }

  int res1 = ecdsa_verify_digest(curve, (const uint8_t *)&pub_key,
                                 (const uint8_t *)&sig, (const uint8_t *)&hash);
  if (res1 == 0) {
    // See if the fuzzer ever manages to get find a correct verification
    // intentionally trigger a crash to make this case observable
    // TODO this is not an actual problem, remove in the future
    crash();
  }

  // the zkp_ecdsa* function only accepts the secp256k1 curve
  if (curve == &secp256k1) {
    int res2 =
        zkp_ecdsa_verify_digest(curve, (const uint8_t *)&pub_key,
                                (const uint8_t *)&sig, (const uint8_t *)&hash);

    // the error code behavior is different between both functions, compare only
    // verification state
    if ((res1 == 0 && res2 != 0) || (res1 != 0 && res2 == 0)) {
      // results differ, this is a problem
      crash();
    }
  }

  return 0;
}

int fuzz_word_index(void) {
#define MAX_WORD_LENGTH 12

  if (fuzzer_length < MAX_WORD_LENGTH) {
    return -1;
  }

  char word[MAX_WORD_LENGTH + 1] = {0};
  memcpy(&word, fuzzer_ptr, MAX_WORD_LENGTH);
  size_t word_length = strlen(word);
  uint16_t index = 0;

  word_index(&index, (const char *)&word, word_length);

  return 0;
}

int fuzz_slip39_word_completion_mask(void) {
  if (fuzzer_length != 2) {
    return -1;
  }
  uint16_t sequence = (fuzzer_ptr[0] << 8) + fuzzer_ptr[1];
  fuzzer_input(2);

  slip39_word_completion_mask(sequence);

  return 0;
}

// regular MAX_MNEMONIC_LEN is 240, try some extra bytes
#define MAX_MNEMONIC_FUZZ_LENGTH 256
int fuzz_mnemonic_check(void) {
  if (fuzzer_length < MAX_MNEMONIC_FUZZ_LENGTH) {
    return -1;
  }

  char mnemonic[MAX_MNEMONIC_FUZZ_LENGTH + 1] = {0};
  memcpy(&mnemonic, fuzzer_ptr, MAX_MNEMONIC_FUZZ_LENGTH);

  // at the time of creation of this fuzzer harness, mnemonic_check()
  // internally calls mnemonic_to_bits() while checking the result
  int ret = mnemonic_check(mnemonic);

  (void)ret;
  /*
  if(ret == 1) {
    // correct result
  }
  */

  return 0;
}

int fuzz_mnemonic_from_data(void) {
  if (fuzzer_length < 16 || fuzzer_length > 32) {
    return -1;
  }

  const char *mnemo_result = mnemonic_from_data(fuzzer_ptr, fuzzer_length);
  if (mnemo_result != NULL) {
    int res = mnemonic_check(mnemo_result);
    if (res == 0) {
      // TODO the mnemonic_check() function is currently incorrectly rejecting
      // valid 15 and 21 word seeds
      // remove this workaround limitation later
      if (fuzzer_length != 20 && fuzzer_length != 28) {
        // the generated mnemonic has an invalid format
        crash();
      }
    }
  }
  // scrub the internal buffer to rule out persistent side effects
  mnemonic_clear();
  return 0;
}

// passphrase normally has a 64 or 256 byte length maximum
#define MAX_PASSPHRASE_FUZZ_LENGTH 257
int fuzz_mnemonic_to_seed(void) {
  if (fuzzer_length < MAX_MNEMONIC_FUZZ_LENGTH + MAX_PASSPHRASE_FUZZ_LENGTH) {
    return -1;
  }

  char mnemonic[MAX_PASSPHRASE_FUZZ_LENGTH + 1] = {0};
  char passphrase[MAX_MNEMONIC_FUZZ_LENGTH + 1] = {0};
  uint8_t seed[512 / 8] = {0};

  memcpy(&mnemonic, fuzzer_input(MAX_MNEMONIC_FUZZ_LENGTH),
         MAX_MNEMONIC_FUZZ_LENGTH);
  memcpy(&passphrase, fuzzer_input(MAX_PASSPHRASE_FUZZ_LENGTH),
         MAX_PASSPHRASE_FUZZ_LENGTH);

  mnemonic_to_seed(mnemonic, passphrase, seed, NULL);

  return 0;
}

int fuzz_ethereum_address_checksum(void) {
  uint8_t addr[20] = {0};
  char address[43] = {0};
  uint64_t chain_id = 0;
  bool rskip60 = false;

  if (fuzzer_length < sizeof(addr) + sizeof(address) + sizeof(chain_id) + 1) {
    return -1;
  }

  memcpy(addr, fuzzer_input(sizeof(addr)), sizeof(addr));
  memcpy(address, fuzzer_input(sizeof(address)), sizeof(address));
  memcpy(&chain_id, fuzzer_input(sizeof(chain_id)), sizeof(chain_id));
  // usually dependent on chain_id, but determined separately here
  rskip60 = (*fuzzer_input(1)) & 0x1;

  ethereum_address_checksum(addr, address, rskip60, chain_id);

  return 0;
}

int fuzz_aes(void) {
  if (fuzzer_length < 1 + 16 + 16 + 32) {
    return -1;
  }

  aes_encrypt_ctx ctxe;
  aes_decrypt_ctx ctxd;
  uint8_t ibuf[16] = {0};
  uint8_t obuf[16] = {0};
  uint8_t iv[16] = {0};
  uint8_t cbuf[16] = {0};

  const uint8_t *keylength_decider = fuzzer_input(1);

  // note: the unit test uses the fixed 32 byte key
  // 603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4
  uint8_t keybuf[32] = {0};
  memcpy(&keybuf, fuzzer_input(32), 32);

#ifdef AES_VAR
  // try 128, 192, 256 bit key lengths

  size_t keylength = 32;
  switch (keylength_decider[0] & 0x3) {
    case 0:
      // invalid length
      keylength = 1;
      break;
    case 1:
      keylength = 16;
      break;
    case 2:
      keylength = 24;
      break;
    case 3:
      keylength = 32;
      break;
  }

  if (aes_encrypt_key((const unsigned char *)&keybuf, keylength, &ctxe) ||
      aes_decrypt_key((const unsigned char *)&keybuf, keylength, &ctxd)) {
    // initialization problems, stop processing
    // we expect this to happen with the invalid key length
    return 0;
  }
#else
  // use a 256 bit key length
  (void)keylength_decider;
  aes_encrypt_key256((const unsigned char *)&keybuf, &ctxe);
  aes_decrypt_key256((const unsigned char *)&keybuf, &ctxd);
#endif

  memcpy(ibuf, fuzzer_input(16), 16);
  memcpy(iv, fuzzer_input(16), 16);

  aes_ecb_encrypt(ibuf, obuf, 16, &ctxe);
  aes_ecb_decrypt(ibuf, obuf, 16, &ctxd);

  aes_cbc_encrypt(ibuf, obuf, 16, iv, &ctxe);
  aes_cbc_decrypt(ibuf, obuf, 16, iv, &ctxd);

  aes_cfb_encrypt(ibuf, obuf, 16, iv, &ctxe);
  aes_cfb_decrypt(ibuf, obuf, 16, iv, &ctxe);

  aes_ofb_encrypt(ibuf, obuf, 16, iv, &ctxe);
  aes_ofb_decrypt(ibuf, obuf, 16, iv, &ctxe);

  aes_ctr_encrypt(ibuf, obuf, 16, cbuf, aes_ctr_cbuf_inc, &ctxe);
  aes_ctr_decrypt(ibuf, obuf, 16, cbuf, aes_ctr_cbuf_inc, &ctxe);
  return 0;
}

int fuzz_chacha_drbg(void) {
#define CHACHA_DRBG_ENTROPY_LENGTH 32
#define CHACHA_DRBG_RESEED_LENGTH 32
#define CHACHA_DRBG_NONCE_LENGTH 16
#define CHACHA_DRBG_RESULT_LENGTH 16

  if (fuzzer_length < CHACHA_DRBG_ENTROPY_LENGTH + CHACHA_DRBG_RESEED_LENGTH +
                          CHACHA_DRBG_NONCE_LENGTH) {
    return -1;
  }

  uint8_t entropy[CHACHA_DRBG_ENTROPY_LENGTH] = {0};
  uint8_t reseed[CHACHA_DRBG_RESEED_LENGTH] = {0};
  uint8_t nonce_bytes[CHACHA_DRBG_NONCE_LENGTH] = {0};
  uint8_t result[CHACHA_DRBG_RESULT_LENGTH] = {0};
  CHACHA_DRBG_CTX ctx;

  // IDEA switch to variable input sizes
  memcpy(&entropy, fuzzer_input(CHACHA_DRBG_ENTROPY_LENGTH),
         CHACHA_DRBG_ENTROPY_LENGTH);
  memcpy(&reseed, fuzzer_input(CHACHA_DRBG_RESEED_LENGTH),
         CHACHA_DRBG_RESEED_LENGTH);
  memcpy(&nonce_bytes, fuzzer_input(CHACHA_DRBG_NONCE_LENGTH),
         CHACHA_DRBG_NONCE_LENGTH);

  chacha_drbg_init(&ctx, entropy, sizeof(entropy), nonce_bytes,
                   sizeof(nonce_bytes));
  chacha_drbg_reseed(&ctx, reseed, sizeof(reseed), NULL, 0);
  chacha_drbg_generate(&ctx, result, sizeof(result));

  return 0;
}

int fuzz_ed25519_sign_verify(void) {
  ed25519_secret_key secret_key;
  ed25519_signature signature;
  ed25519_public_key public_key;
  // length chosen arbitrarily
  uint8_t message[32] = {0};
  int ret = 0;

  if (fuzzer_length <
      sizeof(secret_key) + sizeof(signature) + sizeof(message)) {
    return -1;
  }

  memcpy(&secret_key, fuzzer_input(sizeof(secret_key)), sizeof(secret_key));
  memcpy(&signature, fuzzer_input(sizeof(signature)), sizeof(signature));
  memcpy(&message, fuzzer_input(sizeof(message)), sizeof(message));

  ed25519_publickey(secret_key, public_key);
  // sign message, this should always succeed
  ed25519_sign(message, sizeof(message), secret_key, signature);

  // verify message, we expect this to work
  ret = ed25519_sign_open(message, sizeof(message), public_key, signature);

  if (ret != 0) {
    // verification did not succeed
    crash();
  }

  return 0;
}

int fuzz_zkp_bip340_sign_digest(void) {
  uint8_t priv_key[32] = {0};
  uint8_t aux_input[32] = {0};
  uint8_t digest[32] = {0};
  uint8_t pub_key[32] = {0};
  uint8_t sig[64] = {0};

  if (fuzzer_length <
      sizeof(priv_key) + sizeof(aux_input) + sizeof(digest) + sizeof(sig)) {
    return -1;
  }
  memcpy(priv_key, fuzzer_input(sizeof(priv_key)), sizeof(priv_key));
  memcpy(digest, fuzzer_input(sizeof(digest)), sizeof(digest));
  // TODO leave initialized to 0x0?
  memcpy(aux_input, fuzzer_input(sizeof(aux_input)), sizeof(aux_input));
  // TODO leave initialized to 0x0?
  memcpy(sig, fuzzer_input(sizeof(sig)), sizeof(sig));

  zkp_bip340_get_public_key(priv_key, pub_key);
  check_msan(&pub_key, sizeof(pub_key));
  zkp_bip340_sign_digest(priv_key, digest, sig, aux_input);
  check_msan(&sig, sizeof(sig));
  check_msan(&aux_input, sizeof(aux_input));

  // IDEA test sign result?

  return 0;
}

int fuzz_zkp_bip340_verify_digest(void) {
  int res = 0;
  uint8_t pub_key[32] = {0};
  uint8_t digest[32] = {0};
  uint8_t sig[64] = {0};

  if (fuzzer_length < sizeof(digest) + sizeof(pub_key) + sizeof(sig)) {
    return -1;
  }
  memcpy(pub_key, fuzzer_input(sizeof(pub_key)), sizeof(pub_key));
  memcpy(digest, fuzzer_input(sizeof(digest)), sizeof(digest));
  memcpy(sig, fuzzer_input(sizeof(sig)), sizeof(sig));

  res = zkp_bip340_verify_digest(pub_key, sig, digest);

  // res == 0 is valid, but crash to make successful passes visible
  // TODO remove this later
  if (res == 0) {
    crash();
  }

  return 0;
}

int fuzz_zkp_bip340_tweak_keys(void) {
  uint8_t internal_priv[32] = {0};
  uint8_t root_hash[32] = {0};
  uint8_t internal_pub[32] = {0};
  uint8_t result[32] = {0};

  if (fuzzer_length <
      sizeof(internal_priv) + sizeof(root_hash) + sizeof(internal_pub)) {
    return -1;
  }
  memcpy(internal_priv, fuzzer_input(sizeof(internal_priv)),
         sizeof(internal_priv));
  memcpy(root_hash, fuzzer_input(sizeof(root_hash)), sizeof(root_hash));
  memcpy(internal_pub, fuzzer_input(sizeof(internal_pub)),
         sizeof(internal_pub));

  // IDEA act on return values
  zkp_bip340_tweak_private_key(internal_priv, root_hash, result);
  zkp_bip340_tweak_public_key(internal_pub, root_hash, result);

  return 0;
}

int fuzz_ecdsa_get_public_key_functions(void) {
  uint8_t privkey[32] = {0};
  uint8_t pubkey33_1[33] = {0};
  uint8_t pubkey33_2[33] = {0};
  uint8_t pubkey65_1[65] = {0};
  uint8_t pubkey65_2[65] = {0};

  // note: the zkp_ecdsa_* variants require this specific curve
  const ecdsa_curve *curve = &secp256k1;

  if (fuzzer_length < sizeof(privkey)) {
    return -1;
  }
  memcpy(privkey, fuzzer_input(sizeof(privkey)), sizeof(privkey));

  int res_33_1 = ecdsa_get_public_key33(curve, privkey, pubkey33_1);
  int res_33_2 = zkp_ecdsa_get_public_key33(curve, privkey, pubkey33_2);
  int res_65_1 = ecdsa_get_public_key65(curve, privkey, pubkey65_1);
  int res_65_2 = zkp_ecdsa_get_public_key65(curve, privkey, pubkey65_2);

  // the function pairs have different return error codes for the same input
  // so only fail if the one succeeds where the other does not
  if ((res_33_1 == 0 && res_33_2 != 0) || (res_33_1 != 0 && res_33_2 == 0)) {
    // function result mismatch
    crash();
  }
  if ((res_65_1 == 0 && res_65_2 != 0) || (res_65_1 != 0 && res_65_2 == 0)) {
    // function result mismatch
    crash();
  }

  if (res_33_1 == 0 && res_33_2 == 0 &&
      memcmp(&pubkey33_1, &pubkey33_2, sizeof(pubkey33_1)) != 0) {
    // function result data mismatch
    crash();
  }

  if (res_65_1 == 0 && res_65_2 == 0 &&
      memcmp(&pubkey65_1, &pubkey65_2, sizeof(pubkey65_1)) != 0) {
    // function result data mismatch
    crash();
  }

  return 0;
}

int fuzz_ecdsa_recover_pub_from_sig_functions(void) {
  uint8_t digest[32] = {0};
  uint8_t sig[64] = {0};
  const ecdsa_curve *curve = &secp256k1;
  uint8_t recid = 0;
  uint8_t pubkey1[65] = {0};
  uint8_t pubkey2[65] = {0};

  if (fuzzer_length < sizeof(digest) + sizeof(sig) + sizeof(recid)) {
    return -1;
  }
  memcpy(digest, fuzzer_input(sizeof(digest)), sizeof(digest));
  memcpy(sig, fuzzer_input(sizeof(sig)), sizeof(sig));
  memcpy(&recid, fuzzer_input(sizeof(recid)), sizeof(recid));
  // conform to parameter requirements
  recid = recid & 0x03;

  int res1 = zkp_ecdsa_recover_pub_from_sig(curve, pubkey1, sig, digest, recid);
  int res2 = ecdsa_recover_pub_from_sig(curve, pubkey2, sig, digest, recid);

  if ((res1 == 0 && res2 != 0) || (res1 != 0 && res2 == 0)) {
    // result mismatch
    // bug result reference: https://github.com/trezor/trezor-firmware/pull/2050
    crash();
  }

  if (res1 == 0 && res2 == 0 &&
      memcmp(&pubkey1, &pubkey2, sizeof(pubkey1)) != 0) {
    // pubkey result mismatch
    crash();
  }

  return 0;
}

int fuzz_ecdsa_sig_from_der(void) {
  // bug result reference: https://github.com/trezor/trezor-firmware/pull/2058
  uint8_t der[72] = {0};
  uint8_t out[72] = {0};

  if (fuzzer_length < sizeof(der)) {
    return -1;
  }
  memcpy(der, fuzzer_input(sizeof(der)), sizeof(der));
  // null-terminate
  der[sizeof(der) - 1] = 0;
  size_t der_len = strlen((const char *)der);

  // IDEA use different fuzzer-controlled der_len such as 1 to 73
  int ret = ecdsa_sig_from_der(der, der_len, out);
  (void)ret;
  // IDEA check if back conversion works

  return 0;
}

int fuzz_ecdsa_sig_to_der(void) {
  uint8_t sig[64] = {0};
  uint8_t der[72] = {0};

  if (fuzzer_length < sizeof(sig)) {
    return -1;
  }
  memcpy(sig, fuzzer_input(sizeof(sig)), sizeof(sig));

  int ret = ecdsa_sig_to_der((const uint8_t *)&sig, der);
  (void)ret;
  // IDEA check if back conversion works

  return 0;
}

int fuzz_button_sequence_to_word(void) {
  uint16_t input = 0;
  if (fuzzer_length < sizeof(input)) {
    return -1;
  }
  memcpy(&input, fuzzer_input(sizeof(input)), sizeof(input));

  button_sequence_to_word(input);
  return 0;
}

int fuzz_xmr_add_keys(void) {
  bignum256modm a, b;
  ge25519 A, B;

  if (fuzzer_length < sizeof(bignum256modm) * 2 + sizeof(ge25519) * 2) {
    return -1;
  }
  memcpy(&a, fuzzer_input(sizeof(bignum256modm)), sizeof(bignum256modm));
  memcpy(&b, fuzzer_input(sizeof(bignum256modm)), sizeof(bignum256modm));
  memcpy(&A, fuzzer_input(sizeof(ge25519)), sizeof(ge25519));
  memcpy(&B, fuzzer_input(sizeof(ge25519)), sizeof(ge25519));

  ge25519 r;

  xmr_add_keys2(&r, a, b, &B);
  check_msan(&r, sizeof(r));

  xmr_add_keys2_vartime(&r, a, b, &B);
  check_msan(&r, sizeof(r));

  xmr_add_keys3(&r, a, &A, b, &B);
  check_msan(&r, sizeof(r));

  xmr_add_keys3_vartime(&r, a, &A, b, &B);
  check_msan(&r, sizeof(r));

  return 0;
}

int fuzz_ecdh_multiply(void) {
  uint8_t priv_key[32];
  // 33 or 65 bytes content
  uint8_t pub_key[65];
  uint8_t decider;
  if (fuzzer_length < sizeof(priv_key) + sizeof(pub_key) + sizeof(decider)) {
    return -1;
  }
  memcpy(&priv_key, fuzzer_input(sizeof(priv_key)), sizeof(priv_key));
  memcpy(&pub_key, fuzzer_input(sizeof(pub_key)), sizeof(pub_key));
  memcpy(&decider, fuzzer_input(sizeof(decider)), sizeof(decider));

  uint8_t session_key[65] = {0};
  int res1 = 0;

  // TODO evaluate crash with &curve == NULL, documentation / convention issue?

  const ecdsa_curve *curve2;
  // ecdh_multiply() is only called with secp256k1 and nist256p1 curve from
  // modtrezorcrypto code theoretically other curve parameters are also possible
  if ((decider & 1) == 0) {
    curve2 = &nist256p1;
  } else {
    curve2 = &secp256k1;
  }

  res1 = ecdh_multiply(curve2, (uint8_t *)&priv_key, (uint8_t *)&pub_key,
                       (uint8_t *)&session_key);
  check_msan(&session_key, sizeof(session_key));

  if (res1 != 0) {
    // failure case
  }

  return 0;
}

int fuzz_segwit_addr_encode(void) {
  // the current firmware code only uses witver = 0 and witver = 1
  // we give more flexibility, but do not allow the full int range
  uint8_t chosen_witver = 0;
  // restrict fuzzer variations to lengths of 0 to 255
  uint8_t chosen_witprog_len = 0;

  // in typical use, hrp is a bech32 prefix of 2 to 4 chars
  // TODO make this dynamic, investigate lowercase requirements
  // see also https://github.com/sipa/bech32/issues/38
  char *hrp = "bc";

  if (fuzzer_length < sizeof(chosen_witver) + sizeof(chosen_witprog_len)) {
    return -1;
  }
  memcpy(&chosen_witver, fuzzer_input(sizeof(chosen_witver)),
         sizeof(chosen_witver));
  memcpy(&chosen_witprog_len, fuzzer_input(sizeof(chosen_witprog_len)),
         sizeof(chosen_witprog_len));

  if (chosen_witprog_len > fuzzer_length) {
    return -1;
  }

  char output_address[MAX_ADDR_SIZE] = {0};
  uint8_t *witprog = malloc(chosen_witprog_len);
  RETURN_IF_NULL(witprog);
  memcpy(witprog, fuzzer_input(chosen_witprog_len), chosen_witprog_len);

  int ret = segwit_addr_encode(output_address, hrp, chosen_witver, witprog,
                               chosen_witprog_len);

  // IDEA act depending on ret
  (void)ret;

  free(witprog);
  return 0;
}

// int segwit_addr_decode(int* witver, uint8_t* witdata, size_t* witdata_len,
// const char* hrp, const char* addr) {
int fuzz_segwit_addr_decode(void) {
  int decoded_witver = 0;
  size_t decoded_witprog_len = 0;
  // TODO
  uint8_t addr_raw[MAX_ADDR_RAW_SIZE] = {0};
  uint8_t chosen_addr_len = 0;

  if (fuzzer_length < sizeof(chosen_addr_len)) {
    return -1;
  }

  memcpy(&chosen_addr_len, fuzzer_input(sizeof(chosen_addr_len)),
         sizeof(chosen_addr_len));

  if (chosen_addr_len > fuzzer_length) {
    return -1;
  }

  char *addr = malloc(chosen_addr_len + 1);
  RETURN_IF_NULL(addr);
  memcpy(addr, fuzzer_input(chosen_addr_len), chosen_addr_len);
  // null termination
  addr[chosen_addr_len] = 0;

  // TODO see comments in fuzz_segwit_addr_encode()
  char *hrp = "bc";

  int ret = segwit_addr_decode(&decoded_witver, addr_raw, &decoded_witprog_len,
                               hrp, addr);
  // IDEA act depending on ret
  (void)ret;

  free(addr);
  return 0;
}

/* fuzzer main function */

#define META_HEADER_SIZE 3

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
  // reject input that is too short
  if (size < META_HEADER_SIZE) {
    return -1;
  }

  fuzzer_reset_state();

  // this controls up to 256 different test cases
  uint8_t target_decision = data[0];

  // data[1] is reserved for explicit sub decisions
  // uint8_t target_sub_decision = data[1];

  // data[2] is reserved for future use

  // assign the fuzzer payload data for the target functions
  fuzzer_ptr = data + META_HEADER_SIZE;
  fuzzer_length = size - META_HEADER_SIZE;

  // if active: reject all other inputs that are not the selected target
  // this is helpful for directing the fuzzing focus on a specific case
#ifdef FUZZER_EXCLUSIVE_TARGET
  if (target_decision != FUZZER_EXCLUSIVE_TARGET) {
    return -1;
  }
#endif

  // recent libFuzzer implementations support marking inputs as non-interesting
  // via return -1; instead of the regular return 0;
  // see
  // https://github.com/llvm/llvm-project/commit/92fb310151d2b1e349695fc0f1c5d5d50afb3b52
  int target_result = 0;

  // TODO reorder and regroup target functions
  switch (target_decision) {
    case 0:
      target_result = fuzz_bn_format();
      break;
    case 1:
      target_result = fuzz_base32_decode();
      break;
    case 2:
      target_result = fuzz_base32_encode();
      break;
    case 3:
      target_result = fuzz_base58_encode_check();
      break;
    case 4:
      target_result = fuzz_base58_decode_check();
      break;
    case 5:
      target_result = fuzz_xmr_base58_addr_decode_check();
      break;
    case 6:
      target_result = fuzz_xmr_base58_addr_encode_check();
      break;
    case 7:
      target_result = fuzz_xmr_serialize_varint();
      break;
    case 8:
      target_result = fuzz_nem_validate_address();
      break;
    case 9:
      target_result = fuzz_nem_get_address();
      break;
    case 10:
      target_result = fuzz_xmr_get_subaddress_secret_key();
      break;
    case 11:
      target_result = fuzz_xmr_derive_private_key();
      break;
    case 12:
      target_result = fuzz_xmr_derive_public_key();
      break;
    case 13:
      target_result = fuzz_shamir_interpolate();
      break;
    case 14:
#ifdef FUZZ_ALLOW_SLOW
      zkp_initialize_context_or_crash();
      // slow through expensive bignum operations
      target_result = fuzz_ecdsa_verify_digest_functions();
#endif
      break;
    case 15:
      target_result = fuzz_word_index();
      break;
    case 16:
      target_result = fuzz_slip39_word_completion_mask();
      break;
    case 17:
      target_result = fuzz_mnemonic_check();
      break;
    case 18:
#ifdef FUZZ_ALLOW_SLOW
      target_result = fuzz_aes();
#endif
      break;
    case 22:
      target_result = fuzz_chacha_drbg();
      break;
    case 23:
#ifdef FUZZ_ALLOW_SLOW
      zkp_initialize_context_or_crash();
      // slow through expensive bignum operations
      target_result = fuzz_ecdsa_sign_digest_functions();
#endif
      break;
    case 24:
      target_result = fuzz_ed25519_sign_verify();
      break;
    case 25:
      target_result = fuzz_mnemonic_from_data();
      break;
    case 26:
      target_result = fuzz_mnemonic_to_seed();
      break;
    case 27:
      target_result = fuzz_button_sequence_to_word();
      break;
    case 28:
      target_result = fuzz_segwit_addr_encode();
      break;
    case 29:
      target_result = fuzz_segwit_addr_decode();
      break;
    case 30:
      target_result = fuzz_ethereum_address_checksum();
      break;

    case 41:
      zkp_initialize_context_or_crash();
      target_result = fuzz_zkp_bip340_sign_digest();
      break;
    case 42:
      zkp_initialize_context_or_crash();
      target_result = fuzz_zkp_bip340_verify_digest();
      break;
    case 43:
      zkp_initialize_context_or_crash();
      target_result = fuzz_zkp_bip340_tweak_keys();
      break;
    case 50:
      zkp_initialize_context_or_crash();
      target_result = fuzz_ecdsa_get_public_key_functions();
      break;
    case 51:
      zkp_initialize_context_or_crash();
      target_result = fuzz_ecdsa_recover_pub_from_sig_functions();
      break;
    case 52:
      target_result = fuzz_ecdsa_sig_from_der();
      break;
    case 53:
      target_result = fuzz_ecdsa_sig_to_der();
      break;
    case 60:
      target_result = fuzz_xmr_base58_encode();
      break;
    case 61:
      target_result = fuzz_xmr_base58_decode();
      break;
    case 63:
      target_result = fuzz_xmr_add_keys();
      break;
    case 64:
      target_result = fuzz_ecdh_multiply();
      break;

    default:
      // mark as uninteresting input
      return -1;
      break;
  }
  return target_result;
}
