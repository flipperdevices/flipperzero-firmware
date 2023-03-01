#include <stdio.h>
#include <string.h>
#include <time.h>
#include "bip32.h"
#include "bip39.h"
#include "curves.h"
#include "ecdsa.h"
#include "secp256k1.h"

char iter[256];
uint8_t seed[512 / 8];
char addr[MAX_ADDR_SIZE];
int count = 0, found = 0;
HDNode node;
clock_t start;

#define ACCOUNT_LEGACY 0

// around 280 tries per second

// testing data:
//
// mnemonic:   "all all all all all all all all all all all all"
// address:    legacy: "1JAd7XCBzGudGpJQSDSfpmJhiygtLQWaGL"
//             segwit: "3L6TyTisPBmrDAj6RoKmDzNnj4eQi54gD2"
// passphrase: ""
//
// mnemonic:   "all all all all all all all all all all all all"
// address:    legacy: "1N3uJ5AU3FTYQ1ZQgTMtYmgSvMBmQiGVBS"
//             segwit: "3NcXPfbDP4UHSbuHASALJEBtDeAcWYMMcS"
// passphrase: "testing"

int main(int argc, char **argv) {
  if (argc != 2 && argc != 3) {
    fprintf(stderr, "Usage: bip39bruteforce address [mnemonic]\n");
    return 1;
  }
  const char *address = argv[1];
  const char *mnemonic, *item;
  if (argc == 3) {
    mnemonic = argv[2];
    item = "passphrase";
  } else {
    mnemonic = NULL;
    item = "mnemonic";
  }
  if (mnemonic && !mnemonic_check(mnemonic)) {
    fprintf(stderr, "\"%s\" is not a valid mnemonic\n", mnemonic);
    return 2;
  }
  printf("Reading %ss from stdin ...\n", item);
  start = clock();
  for (;;) {
    if (fgets(iter, 256, stdin) == NULL) break;
    int len = strlen(iter);
    if (len <= 0) {
      continue;
    }
    count++;
    iter[len - 1] = 0;
    if (mnemonic) {
      mnemonic_to_seed(mnemonic, iter, seed, NULL);
    } else {
      mnemonic_to_seed(iter, "", seed, NULL);
    }
    hdnode_from_seed(seed, 512 / 8, SECP256K1_NAME, &node);
#if ACCOUNT_LEGACY
    hdnode_private_ckd_prime(&node, 44);
#else
    hdnode_private_ckd_prime(&node, 49);
#endif
    hdnode_private_ckd_prime(&node, 0);
    hdnode_private_ckd_prime(&node, 0);
    hdnode_private_ckd(&node, 0);
    hdnode_private_ckd(&node, 0);
    hdnode_fill_public_key(&node);
#if ACCOUNT_LEGACY
    // Legacy address
    ecdsa_get_address(node.public_key, 0, HASHER_SHA2_RIPEMD, HASHER_SHA2D,
                      addr, sizeof(addr));
#else
    // Segwit-in-P2SH
    ecdsa_get_address_segwit_p2sh(node.public_key, 5, HASHER_SHA2_RIPEMD,
                                  HASHER_SHA2D, addr, sizeof(addr));
#endif
    if (strcmp(address, addr) == 0) {
      found = 1;
      break;
    }
  }
  float dur = (float)(clock() - start) / CLOCKS_PER_SEC;
  printf("Tried %d %ss in %f seconds = %f tries/second\n", count, item, dur,
         (float)count / dur);
  if (found) {
    printf("Correct %s found! :-)\n\"%s\"\n", item, iter);
    return 0;
  }
  printf("Correct %s not found. :-(\n", item);
  return 4;
}
