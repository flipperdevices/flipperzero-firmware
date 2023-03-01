#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "bip32.h"
#include "curves.h"
#include "ecdsa.h"

#define VERSION_PUBLIC 0x0488b21e

void process_job(uint32_t jobid, const char *xpub, uint32_t change,
                 uint32_t from, uint32_t to) {
  HDNode node, child;
  if (change > 1 || to <= from ||
      hdnode_deserialize_public(xpub, VERSION_PUBLIC, SECP256K1_NAME, &node,
                                NULL) != 0) {
    printf("%d error\n", jobid);
    return;
  }
  hdnode_public_ckd(&node, change);
  uint32_t i;
  char address[36];
  for (i = from; i < to; i++) {
    memcpy(&child, &node, sizeof(HDNode));
    hdnode_public_ckd(&child, i);
    ecdsa_get_address(child.public_key, 0, HASHER_SHA2, HASHER_SHA2D, address,
                      sizeof(address));
    printf("%d %d %s\n", jobid, i, address);
  }
}

int main(void) {
  char line[1024], xpub[1024];
  uint32_t jobid, change, from, to;
  int r;
  for (;;) {
    if (!fgets(line, sizeof(line), stdin)) break;
    r = sscanf(line, "%u %s %u %u %u\n", &jobid, xpub, &change, &from, &to);
    if (r < 1) {
      printf("error\n");
    } else if (r != 5) {
      printf("%d error\n", jobid);
    } else {
      process_job(jobid, xpub, change, from, to);
    }
  }
  return 0;
}
