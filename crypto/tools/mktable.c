#include <assert.h>
#include <stdio.h>
#include "bignum.h"
#include "bip32.h"
#include "ecdsa.h"
#include "rand.h"

/*
 * This program prints the contents of the ecdsa_curve.cp array.
 * The entry cp[i][j] contains the number (2*j+1)*16^i*G,
 * where G is the generator of the specified elliptic curve.
 */
int main(int argc, char **argv) {
  int i, j, k;
  if (argc != 2) {
    printf("Usage: %s CURVE_NAME\n", argv[0]);
    return 1;
  }
  const char *name = argv[1];
  const curve_info *info = get_curve_by_name(name);
  if (info == 0) {
    printf("Unknown curve '%s'\n", name);
    return 1;
  }
  const ecdsa_curve *curve = info->params;
  if (curve == 0) {
    printf("Unknown curve params");
    return 1;
  }

  curve_point ng = curve->G;
  curve_point pow2ig = curve->G;
  for (i = 0; i < 64; i++) {
    // invariants:
    //   pow2ig = 16^i * G
    //   ng     = pow2ig
    printf("\t{\n");
    for (j = 0; j < 8; j++) {
      // invariants:
      //   pow2ig = 16^i * G
      //   ng     = (2*j+1) * 16^i * G
#ifndef NDEBUG
      curve_point checkresult;
      bignum256 a;
      bn_zero(&a);
      a.val[(4 * i) / BN_BITS_PER_LIMB] = ((uint32_t)2 * j + 1)
                                          << ((4 * i) % BN_BITS_PER_LIMB);
      bn_normalize(&a);
      point_multiply(curve, &a, &curve->G, &checkresult);
      assert(point_is_equal(&checkresult, &ng));
#endif
      printf("\t\t/* %2d*16^%d*G: */\n\t\t{{{", 2 * j + 1, i);
      // print x coordinate
      for (k = 0; k < 9; k++) {
        printf((k < 8 ? "0x%08x, " : "0x%04x"), ng.x.val[k]);
      }
      printf("}},\n\t\t {{");
      // print y coordinate
      for (k = 0; k < 9; k++) {
        printf((k < 8 ? "0x%08x, " : "0x%04x"), ng.y.val[k]);
      }
      if (j == 7) {
        printf("}}}\n\t},\n");
      } else {
        printf("}}},\n");
        point_add(curve, &pow2ig, &ng);
      }
      point_add(curve, &pow2ig, &ng);
    }
    pow2ig = ng;
  }
  return 0;
}
