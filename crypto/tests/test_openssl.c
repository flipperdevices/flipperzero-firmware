/**
 * Copyright (c) 2013-2014 Tomas Dzetkulic
 * Copyright (c) 2013-2014 Pavol Rusnak
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

/* OpenSSL's SHA256_CTX/SHA512_CTX conflicts with our own */
#define SHA256_CTX _openssl_SHA256_CTX
#define SHA512_CTX _openssl_SHA512_CTX
#include <openssl/bn.h>
#include <openssl/ecdsa.h>
#include <openssl/obj_mac.h>
#include <openssl/opensslv.h>
#include <openssl/sha.h>
#undef SHA256_CTX
#undef SHA512_CTX

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "ecdsa.h"
#include "hasher.h"
#include "rand.h"

#include "nist256p1.h"
#include "secp256k1.h"

#include "memzero.h"

void openssl_check(unsigned int iterations, int nid, const ecdsa_curve *curve) {
  uint8_t sig[64], pub_key33[33], pub_key65[65], priv_key[32], msg[256],
      hash[32];
  struct SHA256state_st sha256;
  EC_GROUP *ecgroup;

  ecgroup = EC_GROUP_new_by_curve_name(nid);

  for (unsigned int iter = 0; iter < iterations; iter++) {
    // random message len between 1 and 256
    int msg_len = (random32() & 0xFF) + 1;
    // create random message
    random_buffer(msg, msg_len);

    // new ECDSA key
    EC_KEY *eckey = EC_KEY_new();
    EC_KEY_set_group(eckey, ecgroup);

    // generate the key
    EC_KEY_generate_key(eckey);
    // copy key to buffer
    const BIGNUM *K = EC_KEY_get0_private_key(eckey);
    int bn_off = sizeof(priv_key) - BN_num_bytes(K);
    memzero(priv_key, bn_off);
    BN_bn2bin(K, priv_key + bn_off);

    // use our ECDSA signer to sign the message with the key
    if (ecdsa_sign(curve, HASHER_SHA2, priv_key, msg, msg_len, sig, NULL,
                   NULL) != 0) {
      printf("trezor-crypto signing failed\n");
      return;
    }

    // generate public key from private key
    if (ecdsa_get_public_key33(curve, priv_key, pub_key33) != 0) {
      printf("ecdsa_get_public_key33 failed\n");
      return;
    }

    if (ecdsa_get_public_key65(curve, priv_key, pub_key65) != 0) {
      printf("ecdsa_get_public_key65 failed\n");
      return;
    }

    // use our ECDSA verifier to verify the message signature
    if (ecdsa_verify(curve, HASHER_SHA2, pub_key65, sig, msg, msg_len) != 0) {
      printf("trezor-crypto verification failed (pub_key_len = 65)\n");
      return;
    }
    if (ecdsa_verify(curve, HASHER_SHA2, pub_key33, sig, msg, msg_len) != 0) {
      printf("trezor-crypto verification failed (pub_key_len = 33)\n");
      return;
    }

    // copy signature to the OpenSSL struct
    ECDSA_SIG *signature = ECDSA_SIG_new();
#if OPENSSL_VERSION_NUMBER < 0x10100000L
    BN_bin2bn(sig, 32, signature->r);
    BN_bin2bn(sig + 32, 32, signature->s);
#else
    BIGNUM *R = BN_bin2bn(sig, 32, NULL);
    BIGNUM *S = BN_bin2bn(sig + 32, 32, NULL);
    ECDSA_SIG_set0(signature, R, S);
#endif

    // compute the digest of the message
    // note: these are OpenSSL functions, not our own
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, msg, msg_len);
    SHA256_Final(hash, &sha256);

    // verify all went well, i.e. we can decrypt our signature with OpenSSL
    int v = ECDSA_do_verify(hash, 32, signature, eckey);
    if (v != 1) {
      printf("OpenSSL verification failed (%d)\n", v);
      return;
    }

    ECDSA_SIG_free(signature);
    EC_KEY_free(eckey);
    if (((iter + 1) % 100) == 0) printf("Passed ... %d\n", iter + 1);
  }
  EC_GROUP_free(ecgroup);
  printf("All OK\n");
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Usage: test_openssl iterations\n");
    return 1;
  }

  unsigned int iterations;
  sscanf(argv[1], "%u", &iterations);

  printf("Testing secp256k1:\n");
  openssl_check(iterations, NID_secp256k1, &secp256k1);

  printf("Testing nist256p1:\n");
  openssl_check(iterations, NID_X9_62_prime256v1, &nist256p1);

  return 0;
}
