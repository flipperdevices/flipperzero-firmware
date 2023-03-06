#ifndef __BYTE_ORDER_H__
#define __BYTE_ORDER_H__

// FROM sha2.h:
/*
 * BYTE_ORDER NOTE:
 *
 * Please make sure that your system defines BYTE_ORDER.  If your
 * architecture is little-endian, make sure it also defines
 * LITTLE_ENDIAN and that the two (BYTE_ORDER and LITTLE_ENDIAN) are
 * equivalent.
 *
 * If your system does not define the above, then you can do so by
 * hand like this:
 *
 *   #define LITTLE_ENDIAN 1234
 *   #define BIG_ENDIAN    4321
 *
 * And for little-endian machines, add:
 *
 *   #define BYTE_ORDER LITTLE_ENDIAN
 *
 * Or for big-endian machines:
 *
 *   #define BYTE_ORDER BIG_ENDIAN
 *
 * The FreeBSD machine this was written on defines BYTE_ORDER
 * appropriately by including <sys/types.h> (which in turn includes
 * <machine/endian.h> where the appropriate definitions are actually
 * made).
 */

#ifndef LITTLE_ENDIAN
#define LITTLE_ENDIAN 1234
#define BIG_ENDIAN 4321
#endif

#ifndef BYTE_ORDER
#define BYTE_ORDER LITTLE_ENDIAN
#endif

#define REVERSE32(w, x)                                              \
  {                                                                  \
    uint32_t tmp = (w);                                              \
    tmp = (tmp >> 16) | (tmp << 16);                                 \
    (x) = ((tmp & 0xff00ff00UL) >> 8) | ((tmp & 0x00ff00ffUL) << 8); \
  }

#define REVERSE64(w, x)                           \
  {                                               \
    uint64_t tmp = (w);                           \
    tmp = (tmp >> 32) | (tmp << 32);              \
    tmp = ((tmp & 0xff00ff00ff00ff00ULL) >> 8) |  \
          ((tmp & 0x00ff00ff00ff00ffULL) << 8);   \
    (x) = ((tmp & 0xffff0000ffff0000ULL) >> 16) | \
          ((tmp & 0x0000ffff0000ffffULL) << 16);  \
  }

#endif
