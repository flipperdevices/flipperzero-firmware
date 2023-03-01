#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "bip32.h"
#include "curves.h"
#include "ecdsa.h"
#include "ed25519-donna/ed25519.h"
#include "hasher.h"
#include "nist256p1.h"
#include "secp256k1.h"

static uint8_t msg[256];

void prepare_msg(void) {
  for (size_t i = 0; i < sizeof(msg); i++) {
    msg[i] = i * 1103515245;
  }
}

void bench_sign_secp256k1(int iterations) {
  uint8_t sig[64], priv[32], pby;

  const ecdsa_curve *curve = &secp256k1;

  memcpy(priv,
         "\xc5\x5e\xce\x85\x8b\x0d\xdd\x52\x63\xf9\x68\x10\xfe\x14\x43\x7c\xd3"
         "\xb5\xe1\xfb\xd7\xc6\xa2\xec\x1e\x03\x1f\x05\xe8\x6d\x8b\xd5",
         32);

  for (int i = 0; i < iterations; i++) {
    ecdsa_sign(curve, HASHER_SHA2, priv, msg, sizeof(msg), sig, &pby, NULL);
  }
}

void bench_sign_nist256p1(int iterations) {
  uint8_t sig[64], priv[32], pby;

  const ecdsa_curve *curve = &nist256p1;

  memcpy(priv,
         "\xc5\x5e\xce\x85\x8b\x0d\xdd\x52\x63\xf9\x68\x10\xfe\x14\x43\x7c\xd3"
         "\xb5\xe1\xfb\xd7\xc6\xa2\xec\x1e\x03\x1f\x05\xe8\x6d\x8b\xd5",
         32);

  for (int i = 0; i < iterations; i++) {
    ecdsa_sign(curve, HASHER_SHA2, priv, msg, sizeof(msg), sig, &pby, NULL);
  }
}

void bench_sign_ed25519(int iterations) {
  ed25519_secret_key sk;
  ed25519_signature sig;

  memcpy(sk,
         "\xc5\x5e\xce\x85\x8b\x0d\xdd\x52\x63\xf9\x68\x10\xfe\x14\x43\x7c\xd3"
         "\xb5\xe1\xfb\xd7\xc6\xa2\xec\x1e\x03\x1f\x05\xe8\x6d\x8b\xd5",
         32);

  for (int i = 0; i < iterations; i++) {
    ed25519_sign(msg, sizeof(msg), sk, sig);
  }
}

void bench_verify_secp256k1_33(int iterations) {
  uint8_t sig[64], pub[33], priv[32], pby;

  const ecdsa_curve *curve = &secp256k1;

  memcpy(priv,
         "\xc5\x5e\xce\x85\x8b\x0d\xdd\x52\x63\xf9\x68\x10\xfe\x14\x43\x7c\xd3"
         "\xb5\xe1\xfb\xd7\xc6\xa2\xec\x1e\x03\x1f\x05\xe8\x6d\x8b\xd5",
         32);
  ecdsa_get_public_key33(curve, priv, pub);
  ecdsa_sign(curve, HASHER_SHA2, priv, msg, sizeof(msg), sig, &pby, NULL);

  for (int i = 0; i < iterations; i++) {
    ecdsa_verify(curve, HASHER_SHA2, pub, sig, msg, sizeof(msg));
  }
}

void bench_verify_secp256k1_65(int iterations) {
  uint8_t sig[64], pub[65], priv[32], pby;

  const ecdsa_curve *curve = &secp256k1;

  memcpy(priv,
         "\xc5\x5e\xce\x85\x8b\x0d\xdd\x52\x63\xf9\x68\x10\xfe\x14\x43\x7c\xd3"
         "\xb5\xe1\xfb\xd7\xc6\xa2\xec\x1e\x03\x1f\x05\xe8\x6d\x8b\xd5",
         32);
  ecdsa_get_public_key65(curve, priv, pub);
  ecdsa_sign(curve, HASHER_SHA2, priv, msg, sizeof(msg), sig, &pby, NULL);

  for (int i = 0; i < iterations; i++) {
    ecdsa_verify(curve, HASHER_SHA2, pub, sig, msg, sizeof(msg));
  }
}

void bench_verify_nist256p1_33(int iterations) {
  uint8_t sig[64], pub[33], priv[32], pby;

  const ecdsa_curve *curve = &nist256p1;

  memcpy(priv,
         "\xc5\x5e\xce\x85\x8b\x0d\xdd\x52\x63\xf9\x68\x10\xfe\x14\x43\x7c\xd3"
         "\xb5\xe1\xfb\xd7\xc6\xa2\xec\x1e\x03\x1f\x05\xe8\x6d\x8b\xd5",
         32);
  ecdsa_get_public_key33(curve, priv, pub);
  ecdsa_sign(curve, HASHER_SHA2, priv, msg, sizeof(msg), sig, &pby, NULL);

  for (int i = 0; i < iterations; i++) {
    ecdsa_verify(curve, HASHER_SHA2, pub, sig, msg, sizeof(msg));
  }
}

void bench_verify_nist256p1_65(int iterations) {
  uint8_t sig[64], pub[65], priv[32], pby;

  const ecdsa_curve *curve = &nist256p1;

  memcpy(priv,
         "\xc5\x5e\xce\x85\x8b\x0d\xdd\x52\x63\xf9\x68\x10\xfe\x14\x43\x7c\xd3"
         "\xb5\xe1\xfb\xd7\xc6\xa2\xec\x1e\x03\x1f\x05\xe8\x6d\x8b\xd5",
         32);
  ecdsa_get_public_key65(curve, priv, pub);
  ecdsa_sign(curve, HASHER_SHA2, priv, msg, sizeof(msg), sig, &pby, NULL);

  for (int i = 0; i < iterations; i++) {
    ecdsa_verify(curve, HASHER_SHA2, pub, sig, msg, sizeof(msg));
  }
}

void bench_verify_ed25519(int iterations) {
  ed25519_public_key pk;
  ed25519_secret_key sk;
  ed25519_signature sig;

  memcpy(sk,
         "\xc5\x5e\xce\x85\x8b\x0d\xdd\x52\x63\xf9\x68\x10\xfe\x14\x43\x7c\xd3"
         "\xb5\xe1\xfb\xd7\xc6\xa2\xec\x1e\x03\x1f\x05\xe8\x6d\x8b\xd5",
         32);
  ed25519_publickey(sk, pk);
  ed25519_sign(msg, sizeof(msg), sk, sig);

  for (int i = 0; i < iterations; i++) {
    ed25519_sign_open(msg, sizeof(msg), pk, sig);
  }
}

void bench_multiply_curve25519(int iterations) {
  uint8_t result[32];
  uint8_t secret[32];
  uint8_t basepoint[32];

  memcpy(secret,
         "\xc5\x5e\xce\x85\x8b\x0d\xdd\x52\x63\xf9\x68\x10\xfe\x14\x43\x7c\xd3"
         "\xb5\xe1\xfb\xd7\xc6\xa2\xec\x1e\x03\x1f\x05\xe8\x6d\x8b\xd5",
         32);
  memcpy(basepoint,
         "\x96\x47\xda\xbe\x1e\xea\xaf\x25\x47\x1e\x68\x0b\x4d\x7c\x6f\xd1\x14"
         "\x38\x76\xbb\x77\x59\xd8\x3d\x0f\xf7\xa2\x49\x08\xfd\xda\xbc",
         32);

  for (int i = 0; i < iterations; i++) {
    curve25519_scalarmult(result, secret, basepoint);
  }
}

static HDNode root;

void prepare_node(void) {
  hdnode_from_seed((uint8_t *)"NothingToSeeHere", 16, SECP256K1_NAME, &root);
  hdnode_fill_public_key(&root);
}

void bench_ckd_normal(int iterations) {
  char addr[MAX_ADDR_SIZE];
  HDNode node;
  for (int i = 0; i < iterations; i++) {
    memcpy(&node, &root, sizeof(HDNode));
    hdnode_public_ckd(&node, i);
    hdnode_fill_public_key(&node);
    ecdsa_get_address(node.public_key, HASHER_SHA2, HASHER_SHA2D, 0, addr,
                      sizeof(addr));
  }
}

void bench_ckd_optimized(int iterations) {
  char addr[MAX_ADDR_SIZE];
  curve_point pub;
  ecdsa_read_pubkey(&secp256k1, root.public_key, &pub);
  for (int i = 0; i < iterations; i++) {
    hdnode_public_ckd_address_optimized(&pub, root.chain_code, i, 0,
                                        HASHER_SHA2, HASHER_SHA2D, addr,
                                        sizeof(addr), false);
  }
}

void bench(void (*func)(int), const char *name, int iterations) {
  clock_t t = clock();
  func(iterations);
  float speed = iterations / ((float)(clock() - t) / CLOCKS_PER_SEC);
  printf("%25s: %8.2f ops/s\n", name, speed);
}

#define BENCH(FUNC, ITER) bench(FUNC, #FUNC, ITER)

int main(void) {
  prepare_msg();

  BENCH(bench_sign_secp256k1, 500);
  BENCH(bench_verify_secp256k1_33, 500);
  BENCH(bench_verify_secp256k1_65, 500);

  BENCH(bench_sign_nist256p1, 500);
  BENCH(bench_verify_nist256p1_33, 500);
  BENCH(bench_verify_nist256p1_65, 500);

  BENCH(bench_sign_ed25519, 4000);
  BENCH(bench_verify_ed25519, 4000);

  BENCH(bench_multiply_curve25519, 4000);

  prepare_node();

  BENCH(bench_ckd_normal, 1000);
  BENCH(bench_ckd_optimized, 1000);

  return 0;
}
