#include <assert.h>
#include "ed25519-donna.h"
#include "memzero.h"

/* sqrt(x) is such an integer y that 0 <= y <= p - 1, y % 2 = 0, and y^2 = x (mod p). */
/* d = -121665 / 121666 */
#if !defined(NDEBUG)
static const bignum25519 ALIGN(16) fe_d = {
		0x35978a3, 0x0d37284, 0x3156ebd, 0x06a0a0e, 0x001c029, 0x179e898, 0x3a03cbb, 0x1ce7198, 0x2e2b6ff, 0x1480db3}; /* d */
#endif
static const bignum25519 ALIGN(16) fe_sqrtm1 = {
		0x20ea0b0, 0x186c9d2, 0x08f189d, 0x035697f, 0x0bd0c60, 0x1fbd7a7, 0x2804c9e, 0x1e16569, 0x004fc1d, 0x0ae0c92}; /* sqrt(-1) */
//static const bignum25519 ALIGN(16) fe_d2 = {
//		0x2b2f159, 0x1a6e509, 0x22add7a, 0x0d4141d, 0x0038052, 0x0f3d130, 0x3407977, 0x19ce331, 0x1c56dff, 0x0901b67}; /* 2 * d */

/* A = 2 * (1 - d) / (1 + d) = 486662 */
static const bignum25519 ALIGN(16) fe_ma2 = {
		0x33de3c9, 0x1fff236, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff}; /* -A^2 */
static const bignum25519 ALIGN(16) fe_ma = {
		0x3f892e7, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff, 0x3ffffff, 0x1ffffff}; /* -A */
static const bignum25519 ALIGN(16) fe_fffb1 = {
		0x1e3bdff, 0x025a2b3, 0x18e5bab, 0x0ba36ac, 0x0b9afed, 0x004e61c, 0x31d645f, 0x09d1bea, 0x102529e, 0x0063810}; /* sqrt(-2 * A * (A + 2)) */
static const bignum25519 ALIGN(16) fe_fffb2 = {
		0x383650d, 0x066df27, 0x10405a4, 0x1cfdd48, 0x2b887f2, 0x1e9a041, 0x1d7241f, 0x0612dc5, 0x35fba5d, 0x0cbe787}; /* sqrt(2 * A * (A + 2)) */
static const bignum25519 ALIGN(16) fe_fffb3 = {
		0x0cfd387, 0x1209e3a, 0x3bad4fc, 0x18ad34d, 0x2ff6c02, 0x0f25d12, 0x15cdfe0, 0x0e208ed, 0x32eb3df, 0x062d7bb}; /* sqrt(-sqrt(-1) * A * (A + 2)) */
static const bignum25519 ALIGN(16) fe_fffb4 = {
		0x2b39186, 0x14640ed, 0x14930a7, 0x04509fa, 0x3b91bf0, 0x0f7432e, 0x07a443f, 0x17f24d8, 0x031067d, 0x0690fcc}; /* sqrt(sqrt(-1) * A * (A + 2)) */


/*
	Timing safe memory compare
*/
int ed25519_verify(const unsigned char *x, const unsigned char *y, size_t len) {
	size_t differentbits = 0;
	while (len--)
		differentbits |= (*x++ ^ *y++);
	return (int) (1 & ((differentbits - 1) >> 8));
}

/*
	conversions
*/

void ge25519_p1p1_to_partial(ge25519 *r, const ge25519_p1p1 *p) {
	curve25519_mul(r->x, p->x, p->t);
	curve25519_mul(r->y, p->y, p->z);
	curve25519_mul(r->z, p->z, p->t);
}

void ge25519_p1p1_to_full(ge25519 *r, const ge25519_p1p1 *p) {
	curve25519_mul(r->x, p->x, p->t);
	curve25519_mul(r->y, p->y, p->z);
	curve25519_mul(r->z, p->z, p->t);
	curve25519_mul(r->t, p->x, p->y);
}

void ge25519_full_to_pniels(ge25519_pniels *p, const ge25519 *r) {
	curve25519_sub(p->ysubx, r->y, r->x);
	curve25519_add(p->xaddy, r->y, r->x);
	curve25519_copy(p->z, r->z);
	curve25519_mul(p->t2d, r->t, ge25519_ec2d);
}

/*
	adding & doubling
*/

void ge25519_double_p1p1(ge25519_p1p1 *r, const ge25519 *p) {
	bignum25519 a = {0}, b = {0}, c = {0};

	curve25519_square(a, p->x);
	curve25519_square(b, p->y);
	curve25519_square(c, p->z);
	curve25519_add_reduce(c, c, c);
	curve25519_add(r->x, p->x, p->y);
	curve25519_square(r->x, r->x);
	curve25519_add(r->y, b, a);
	curve25519_sub(r->z, b, a);
	curve25519_sub_after_basic(r->x, r->x, r->y);
	curve25519_sub_after_basic(r->t, c, r->z);
}

#ifndef ED25519_NO_PRECOMP
void ge25519_nielsadd2_p1p1(ge25519_p1p1 *r, const ge25519 *p, const ge25519_niels *q, unsigned char signbit) {
	const bignum25519 *qb = (const bignum25519 *)q;
	bignum25519 *rb = (bignum25519 *)r;
	bignum25519 a = {0}, b = {0}, c = {0};

	curve25519_sub(a, p->y, p->x);
	curve25519_add(b, p->y, p->x);
	curve25519_mul(a, a, qb[signbit]); /* x for +, y for - */
	curve25519_mul(r->x, b, qb[signbit^1]); /* y for +, x for - */
	curve25519_add(r->y, r->x, a);
	curve25519_sub(r->x, r->x, a);
	curve25519_mul(c, p->t, q->t2d);
	curve25519_add_reduce(r->t, p->z, p->z);
	curve25519_copy(r->z, r->t);
	curve25519_add(rb[2+signbit], rb[2+signbit], c); /* z for +, t for - */
	curve25519_sub(rb[2+(signbit^1)], rb[2+(signbit^1)], c); /* t for +, z for - */
}
#endif

void ge25519_pnielsadd_p1p1(ge25519_p1p1 *r, const ge25519 *p, const ge25519_pniels *q, unsigned char signbit) {
	const bignum25519 *qb = (const bignum25519 *)q;
	bignum25519 *rb = (bignum25519 *)r;
	bignum25519 a = {0}, b = {0}, c = {0};

	curve25519_sub(a, p->y, p->x);
	curve25519_add(b, p->y, p->x);
	curve25519_mul(a, a, qb[signbit]); /* ysubx for +, xaddy for - */
	curve25519_mul(r->x, b, qb[signbit^1]); /* xaddy for +, ysubx for - */
	curve25519_add(r->y, r->x, a);
	curve25519_sub(r->x, r->x, a);
	curve25519_mul(c, p->t, q->t2d);
	curve25519_mul(r->t, p->z, q->z);
	curve25519_add_reduce(r->t, r->t, r->t);
	curve25519_copy(r->z, r->t);
	curve25519_add(rb[2+signbit], rb[2+signbit], c); /* z for +, t for - */
	curve25519_sub(rb[2+(signbit^1)], rb[2+(signbit^1)], c); /* t for +, z for - */
}

void ge25519_double_partial(ge25519 *r, const ge25519 *p) {
	ge25519_p1p1 t = {0};
	ge25519_double_p1p1(&t, p);
	ge25519_p1p1_to_partial(r, &t);
}

void ge25519_double(ge25519 *r, const ge25519 *p) {
	ge25519_p1p1 t = {0};
	ge25519_double_p1p1(&t, p);
	ge25519_p1p1_to_full(r, &t);
}

void ge25519_nielsadd2(ge25519 *r, const ge25519_niels *q) {
	bignum25519 a = {0}, b = {0}, c = {0}, e = {0}, f = {0}, g = {0}, h = {0};

	curve25519_sub(a, r->y, r->x);
	curve25519_add(b, r->y, r->x);
	curve25519_mul(a, a, q->ysubx);
	curve25519_mul(e, b, q->xaddy);
	curve25519_add(h, e, a);
	curve25519_sub(e, e, a);
	curve25519_mul(c, r->t, q->t2d);
	curve25519_add(f, r->z, r->z);
	curve25519_add_after_basic(g, f, c);
	curve25519_sub_after_basic(f, f, c);
	curve25519_mul(r->x, e, f);
	curve25519_mul(r->y, h, g);
	curve25519_mul(r->z, g, f);
	curve25519_mul(r->t, e, h);
}

void ge25519_pnielsadd(ge25519_pniels *r, const ge25519 *p, const ge25519_pniels *q) {
	bignum25519 a = {0}, b = {0}, c = {0}, x = {0}, y = {0}, z = {0}, t = {0};

	curve25519_sub(a, p->y, p->x);
	curve25519_add(b, p->y, p->x);
	curve25519_mul(a, a, q->ysubx);
	curve25519_mul(x, b, q->xaddy);
	curve25519_add(y, x, a);
	curve25519_sub(x, x, a);
	curve25519_mul(c, p->t, q->t2d);
	curve25519_mul(t, p->z, q->z);
	curve25519_add(t, t, t);
	curve25519_add_after_basic(z, t, c);
	curve25519_sub_after_basic(t, t, c);
	curve25519_mul(r->xaddy, x, t);
	curve25519_mul(r->ysubx, y, z);
	curve25519_mul(r->z, z, t);
	curve25519_mul(r->t2d, x, y);
	curve25519_copy(y, r->ysubx);
	curve25519_sub(r->ysubx, r->ysubx, r->xaddy);
	curve25519_add(r->xaddy, r->xaddy, y);
	curve25519_mul(r->t2d, r->t2d, ge25519_ec2d);
}


/*
	pack & unpack
*/

void ge25519_pack(unsigned char r[32], const ge25519 *p) {
	bignum25519 tx = {0}, ty = {0}, zi = {0};
	unsigned char parity[32] = {0};
	curve25519_recip(zi, p->z);
	curve25519_mul(tx, p->x, zi);
	curve25519_mul(ty, p->y, zi);
	curve25519_contract(r, ty);
	curve25519_contract(parity, tx);
	r[31] ^= ((parity[0] & 1) << 7);
}

int ge25519_unpack_negative_vartime(ge25519 *r, const unsigned char p[32]) {
	const unsigned char zero[32] = {0};
	const bignum25519 one = {1};
	unsigned char parity = p[31] >> 7;
	unsigned char check[32] = {0};
	bignum25519 t = {0}, root = {0}, num = {0}, den = {0}, d3 = {0};

	curve25519_expand(r->y, p);
	curve25519_copy(r->z, one);
	curve25519_square(num, r->y); /* x = y^2 */
	curve25519_mul(den, num, ge25519_ecd); /* den = dy^2 */
	curve25519_sub_reduce(num, num, r->z); /* x = y^1 - 1 */
	curve25519_add(den, den, r->z); /* den = dy^2 + 1 */

	/* Computation of sqrt(num/den) */
	/* 1.: computation of num^((p-5)/8)*den^((7p-35)/8) = (num*den^7)^((p-5)/8) */
	curve25519_square(t, den);
	curve25519_mul(d3, t, den);
	curve25519_square(r->x, d3);
	curve25519_mul(r->x, r->x, den);
	curve25519_mul(r->x, r->x, num);
	curve25519_pow_two252m3(r->x, r->x);

	/* 2. computation of r->x = num * den^3 * (num*den^7)^((p-5)/8) */
	curve25519_mul(r->x, r->x, d3);
	curve25519_mul(r->x, r->x, num);

	/* 3. Check if either of the roots works: */
	curve25519_square(t, r->x);
	curve25519_mul(t, t, den);
	curve25519_sub_reduce(root, t, num);
	curve25519_contract(check, root);
	if (!ed25519_verify(check, zero, 32)) {
		curve25519_add_reduce(t, t, num);
		curve25519_contract(check, t);
		if (!ed25519_verify(check, zero, 32))
			return 0;
		curve25519_mul(r->x, r->x, ge25519_sqrtneg1);
	}

	curve25519_contract(check, r->x);
	if ((check[0] & 1) == parity) {
		curve25519_copy(t, r->x);
		curve25519_neg(r->x, t);
	}
	curve25519_mul(r->t, r->x, r->y);
	return 1;
}

/*
	scalarmults
*/

void ge25519_set_neutral(ge25519 *r)
{
	memzero(r, sizeof(ge25519));
	r->y[0] = 1;
	r->z[0] = 1;
}

#define S1_SWINDOWSIZE 5
#define S1_TABLE_SIZE (1<<(S1_SWINDOWSIZE-2))
#ifdef ED25519_NO_PRECOMP
#define S2_SWINDOWSIZE 5
#else
#define S2_SWINDOWSIZE 7
#endif
#define S2_TABLE_SIZE (1<<(S2_SWINDOWSIZE-2))

/* computes [s1]p1 + [s2]base */
void ge25519_double_scalarmult_vartime(ge25519 *r, const ge25519 *p1, const bignum256modm s1, const bignum256modm s2) {
	signed char slide1[256] = {0}, slide2[256] = {0};
	ge25519_pniels pre1[S1_TABLE_SIZE] = {0};
#ifdef ED25519_NO_PRECOMP
	ge25519_pniels pre2[S2_TABLE_SIZE] = {0};
#endif
	ge25519 dp = {0};
	ge25519_p1p1 t = {0};
	int32_t i = 0;

	memzero(&t, sizeof(ge25519_p1p1));
	contract256_slidingwindow_modm(slide1, s1, S1_SWINDOWSIZE);
	contract256_slidingwindow_modm(slide2, s2, S2_SWINDOWSIZE);

	ge25519_double(&dp, p1);
	ge25519_full_to_pniels(pre1, p1);
	for (i = 0; i < S1_TABLE_SIZE - 1; i++)
		ge25519_pnielsadd(&pre1[i+1], &dp, &pre1[i]);

#ifdef ED25519_NO_PRECOMP
	ge25519_double(&dp, &ge25519_basepoint);
	ge25519_full_to_pniels(pre2, &ge25519_basepoint);
	for (i = 0; i < S2_TABLE_SIZE - 1; i++)
		ge25519_pnielsadd(&pre2[i+1], &dp, &pre2[i]);
#endif

	ge25519_set_neutral(r);

	i = 255;
	while ((i >= 0) && !(slide1[i] | slide2[i]))
		i--;

	for (; i >= 0; i--) {
		ge25519_double_p1p1(&t, r);

		if (slide1[i]) {
			ge25519_p1p1_to_full(r, &t);
			ge25519_pnielsadd_p1p1(&t, r, &pre1[abs(slide1[i]) / 2], (unsigned char)slide1[i] >> 7);
		}

		if (slide2[i]) {
			ge25519_p1p1_to_full(r, &t);
#ifdef ED25519_NO_PRECOMP
			ge25519_pnielsadd_p1p1(&t, r, &pre2[abs(slide2[i]) / 2], (unsigned char)slide2[i] >> 7);
#else
			ge25519_nielsadd2_p1p1(&t, r, &ge25519_niels_sliding_multiples[abs(slide2[i]) / 2], (unsigned char)slide2[i] >> 7);
#endif
		}

		ge25519_p1p1_to_partial(r, &t);
	}
	curve25519_mul(r->t, t.x, t.y);
	memzero(slide1, sizeof(slide1));
	memzero(slide2, sizeof(slide2));
}

/* computes [s1]p1 + [s2]p2 */
#if USE_MONERO
void ge25519_double_scalarmult_vartime2(ge25519 *r, const ge25519 *p1, const bignum256modm s1, const ge25519 *p2, const bignum256modm s2) {
	signed char slide1[256] = {0}, slide2[256] = {0};
	ge25519_pniels pre1[S1_TABLE_SIZE] = {0};
	ge25519_pniels pre2[S1_TABLE_SIZE] = {0};
	ge25519 dp = {0};
	ge25519_p1p1 t = {0};
	int32_t i = 0;

	memzero(&t, sizeof(ge25519_p1p1));
	contract256_slidingwindow_modm(slide1, s1, S1_SWINDOWSIZE);
	contract256_slidingwindow_modm(slide2, s2, S1_SWINDOWSIZE);

	ge25519_double(&dp, p1);
	ge25519_full_to_pniels(pre1, p1);
	for (i = 0; i < S1_TABLE_SIZE - 1; i++)
		ge25519_pnielsadd(&pre1[i+1], &dp, &pre1[i]);

	ge25519_double(&dp, p2);
	ge25519_full_to_pniels(pre2, p2);
	for (i = 0; i < S1_TABLE_SIZE - 1; i++)
		ge25519_pnielsadd(&pre2[i+1], &dp, &pre2[i]);

	ge25519_set_neutral(r);

	i = 255;
	while ((i >= 0) && !(slide1[i] | slide2[i]))
		i--;

	for (; i >= 0; i--) {
		ge25519_double_p1p1(&t, r);

		if (slide1[i]) {
			ge25519_p1p1_to_full(r, &t);
			ge25519_pnielsadd_p1p1(&t, r, &pre1[abs(slide1[i]) / 2], (unsigned char)slide1[i] >> 7);
		}

		if (slide2[i]) {
			ge25519_p1p1_to_full(r, &t);
			ge25519_pnielsadd_p1p1(&t, r, &pre2[abs(slide2[i]) / 2], (unsigned char)slide2[i] >> 7);
		}

		ge25519_p1p1_to_partial(r, &t);
	}
	curve25519_mul(r->t, t.x, t.y);
	memzero(slide1, sizeof(slide1));
	memzero(slide2, sizeof(slide2));
}
#endif

/*
 * The following conditional move stuff uses conditional moves.
 * I will check on which compilers this works, and provide suitable
 * workarounds for those where it doesn't.
 *
 * This works on gcc 4.x and above with -O3.  Don't use -O2, this will
 * cause the code to not generate conditional moves.  Don't use any -march=
 * with less than i686 on x86
 */
static void ge25519_cmove_stride4(long * r, long * p, long * pos, long * n, int stride) {
  long x0=r[0], x1=r[1], x2=r[2], x3=r[3], y0 = 0, y1 = 0, y2 = 0, y3 = 0;
  for(; p<n; p+=stride) {
    volatile int flag=(p==pos);
    y0 = p[0];
    y1 = p[1];
    y2 = p[2];
    y3 = p[3];
    x0 = flag ? y0 : x0;
    x1 = flag ? y1 : x1;
    x2 = flag ? y2 : x2;
    x3 = flag ? y3 : x3;
  }
  r[0] = x0;
  r[1] = x1;
  r[2] = x2;
  r[3] = x3;
}
#define HAS_CMOVE_STRIDE4

static void ge25519_cmove_stride4b(long * r, long * p, long * pos, long * n, int stride) {
  long x0=p[0], x1=p[1], x2=p[2], x3=p[3], y0 = 0, y1 = 0, y2 = 0, y3 = 0;
  for(p+=stride; p<n; p+=stride) {
    volatile int flag=(p==pos);
    y0 = p[0];
    y1 = p[1];
    y2 = p[2];
    y3 = p[3];
    x0 = flag ? y0 : x0;
    x1 = flag ? y1 : x1;
    x2 = flag ? y2 : x2;
    x3 = flag ? y3 : x3;
  }
  r[0] = x0;
  r[1] = x1;
  r[2] = x2;
  r[3] = x3;
}
#define HAS_CMOVE_STRIDE4B

void ge25519_move_conditional_pniels_array(ge25519_pniels * r, const ge25519_pniels * p, int pos, int n) {
#ifdef HAS_CMOVE_STRIDE4B
  size_t i = 0;
  for(i=0; i<sizeof(ge25519_pniels)/sizeof(long); i+=4) {
    ge25519_cmove_stride4b(((long*)r)+i,
			   ((long*)p)+i,
			   ((long*)(p+pos))+i,
			   ((long*)(p+n))+i,
			   sizeof(ge25519_pniels)/sizeof(long));
  }
#else
  size_t i = 0;
  for(i=0; i<n; i++) {
    ge25519_move_conditional_pniels(r, p+i, pos==i);
  }
#endif
}

void ge25519_move_conditional_niels_array(ge25519_niels * r, const uint8_t p[8][96], int pos, int n) {
  size_t i = 0;
  for(i=0; i<96/sizeof(long); i+=4) {
    ge25519_cmove_stride4(((long*)r)+i,
			  ((long*)p)+i,
			  ((long*)(p+pos))+i,
			  ((long*)(p+n))+i,
			  96/sizeof(long));
  }
}

/* computes [s1]p1, constant time */
void ge25519_scalarmult(ge25519 *r, const ge25519 *p1, const bignum256modm s1) {
	signed char slide1[64] = {0};
	ge25519_pniels pre1[9] = {0};
	ge25519_pniels pre = {0};
	ge25519 d1 = {0};
	ge25519_p1p1 t = {0};
	int32_t i = 0;

	contract256_window4_modm(slide1, s1);

	ge25519_full_to_pniels(pre1+1, p1);
	ge25519_double(&d1, p1);

	ge25519_set_neutral(r);
	ge25519_full_to_pniels(pre1, r);

	ge25519_full_to_pniels(pre1+2, &d1);
	for (i = 1; i < 7; i++) {
		ge25519_pnielsadd(&pre1[i+2], &d1, &pre1[i]);
	}

	for (i = 63; i >= 0; i--) {
		int k=abs(slide1[i]);
		ge25519_double_partial(r, r);
		ge25519_double_partial(r, r);
		ge25519_double_partial(r, r);
		ge25519_double_p1p1(&t, r);
		ge25519_move_conditional_pniels_array(&pre, pre1, k, 9);
		ge25519_p1p1_to_full(r, &t);
		ge25519_pnielsadd_p1p1(&t, r, &pre, (unsigned char)slide1[i] >> 7);
		ge25519_p1p1_to_partial(r, &t);
	}
	curve25519_mul(r->t, t.x, t.y);
	memzero(slide1, sizeof(slide1));
}

void ge25519_scalarmult_base_choose_niels(ge25519_niels *t, const uint8_t table[256][96], uint32_t pos, signed char b) {
	bignum25519 neg = {0};
	uint32_t sign = (uint32_t)((unsigned char)b >> 7);
	uint32_t mask = ~(sign - 1);
	uint32_t u = (b + mask) ^ mask;

	/* ysubx, xaddy, t2d in packed form. initialize to ysubx = 1, xaddy = 1, t2d = 0 */
	uint8_t packed[96] = {0};
	packed[0] = 1;
	packed[32] = 1;

	ge25519_move_conditional_niels_array((ge25519_niels *)packed, &table[pos*8], u-1, 8);

	/* expand in to t */
	curve25519_expand(t->ysubx, packed +  0);
	curve25519_expand(t->xaddy, packed + 32);
	curve25519_expand(t->t2d  , packed + 64);

	/* adjust for sign */
	curve25519_swap_conditional(t->ysubx, t->xaddy, sign);
	curve25519_neg(neg, t->t2d);
	curve25519_swap_conditional(t->t2d, neg, sign);
}

/* computes [s]basepoint */
void ge25519_scalarmult_base_niels(ge25519 *r, const uint8_t basepoint_table[256][96], const bignum256modm s) {
	signed char b[64] = {0};
	uint32_t i = 0;
	ge25519_niels t = {0};

	contract256_window4_modm(b, s);

	ge25519_scalarmult_base_choose_niels(&t, basepoint_table, 0, b[1]);
	curve25519_sub_reduce(r->x, t.xaddy, t.ysubx);
	curve25519_add_reduce(r->y, t.xaddy, t.ysubx);
	memzero(r->z, sizeof(bignum25519));
	curve25519_copy(r->t, t.t2d);
	r->z[0] = 2;
	for (i = 3; i < 64; i += 2) {
		ge25519_scalarmult_base_choose_niels(&t, basepoint_table, i / 2, b[i]);
		ge25519_nielsadd2(r, &t);
	}
	ge25519_double_partial(r, r);
	ge25519_double_partial(r, r);
	ge25519_double_partial(r, r);
	ge25519_double(r, r);
	ge25519_scalarmult_base_choose_niels(&t, basepoint_table, 0, b[0]);
	curve25519_mul(t.t2d, t.t2d, ge25519_ecd);
	ge25519_nielsadd2(r, &t);
	for(i = 2; i < 64; i += 2) {
		ge25519_scalarmult_base_choose_niels(&t, basepoint_table, i / 2, b[i]);
		ge25519_nielsadd2(r, &t);
	}
}

int ge25519_check(const ge25519 *r){
	/* return (z % q != 0 and
						 x * y % q == z * t % q and
						(y * y - x * x - z * z - ed25519.d * t * t) % q == 0)
	 */

	bignum25519 z={0}, lhs={0}, rhs={0}, tmp={0}, res={0};
	curve25519_reduce(z, r->z);

	curve25519_mul(lhs, r->x, r->y);
	curve25519_mul(rhs, r->z, r->t);
	curve25519_sub_reduce(lhs, lhs, rhs);

	curve25519_square(res, r->y);
	curve25519_square(tmp, r->x);
	curve25519_sub_reduce(res, res, tmp);
	curve25519_square(tmp, r->z);
	curve25519_sub_reduce(res, res, tmp);
	curve25519_square(tmp, r->t);
	curve25519_mul(tmp, tmp, ge25519_ecd);
	curve25519_sub_reduce(res, res, tmp);

	const int c1 = curve25519_isnonzero(z);
	const int c2 = curve25519_isnonzero(lhs);
	const int c3 = curve25519_isnonzero(res);
	return c1 & (c2^0x1) & (c3^0x1);
}

int ge25519_eq(const ge25519 *a, const ge25519 *b){
	int eq = 1;
	bignum25519 t1={0}, t2={0};

	eq &= ge25519_check(a);
	eq &= ge25519_check(b);

	curve25519_mul(t1, a->x, b->z);
	curve25519_mul(t2, b->x, a->z);
	curve25519_sub_reduce(t1, t1, t2);
	eq &= curve25519_isnonzero(t1) ^ 1;

	curve25519_mul(t1, a->y, b->z);
	curve25519_mul(t2, b->y, a->z);
	curve25519_sub_reduce(t1, t1, t2);
	eq &= curve25519_isnonzero(t1) ^ 1;

	return eq;
}

void ge25519_copy(ge25519 *dst, const ge25519 *src){
	curve25519_copy(dst->x, src->x);
	curve25519_copy(dst->y, src->y);
	curve25519_copy(dst->z, src->z);
	curve25519_copy(dst->t, src->t);
}

void ge25519_set_base(ge25519 *r){
	ge25519_copy(r, &ge25519_basepoint);
}

void ge25519_mul8(ge25519 *r, const ge25519 *t) {
	ge25519_double_partial(r, t);
	ge25519_double_partial(r, r);
	ge25519_double(r, r);
}

void ge25519_neg_partial(ge25519 *r){
	curve25519_neg(r->x, r->x);
}

void ge25519_neg_full(ge25519 *r){
	curve25519_neg(r->x, r->x);
	curve25519_neg(r->t, r->t);
}

void ge25519_reduce(ge25519 *r, const ge25519 *t){
	curve25519_reduce(r->x, t->x);
	curve25519_reduce(r->y, t->y);
	curve25519_reduce(r->z, t->z);
	curve25519_reduce(r->t, t->t);
}

void ge25519_norm(ge25519 *r, const ge25519 * t){
	bignum25519 zinv = {0};
	curve25519_recip(zinv, t->z);
	curve25519_mul(r->x, t->x, zinv);
	curve25519_mul(r->y, t->y, zinv);
	curve25519_mul(r->t, r->x, r->y);
	curve25519_set(r->z, 1);
}

void ge25519_add(ge25519 *r, const ge25519 *p, const ge25519 *q, unsigned char signbit) {
	ge25519_pniels P_ni = {0};
	ge25519_p1p1 P_11 = {0};

	ge25519_full_to_pniels(&P_ni, q);
	ge25519_pnielsadd_p1p1(&P_11, p, &P_ni, signbit);
	ge25519_p1p1_to_full(r, &P_11);
}

void ge25519_fromfe_frombytes_vartime(ge25519 *r, const unsigned char *s){
	bignum25519 u={0}, v={0}, w={0}, x={0}, y={0}, z={0};
	unsigned char sign = 0;

	curve25519_expand_reduce(u, s);

	curve25519_square(v, u);
	curve25519_add_reduce(v, v, v); /* 2 * u^2 */
	curve25519_set(w, 1);
	curve25519_add_reduce(w, v, w); /* w = 2 * u^2 + 1 */

	curve25519_square(x, w); /* w^2 */
	curve25519_mul(y, fe_ma2, v); /* -2 * A^2 * u^2 */
	curve25519_add_reduce(x, x, y); /* x = w^2 - 2 * A^2 * u^2 */

	curve25519_divpowm1(r->x, w, x); /* (w / x)^(m + 1) */
	curve25519_square(y, r->x);
	curve25519_mul(x, y, x);
	curve25519_sub_reduce(y, w, x);
	curve25519_copy(z, fe_ma);

	if (curve25519_isnonzero(y)) {
		curve25519_add_reduce(y, w, x);
		if (curve25519_isnonzero(y)) {
			goto negative;
		} else {
			curve25519_mul(r->x, r->x, fe_fffb1);
		}
	} else {
		curve25519_mul(r->x, r->x, fe_fffb2);
	}
	curve25519_mul(r->x, r->x, u); /* u * sqrt(2 * A * (A + 2) * w / x) */
	curve25519_mul(z, z, v); /* -2 * A * u^2 */
	sign = 0;
	goto setsign;
negative:
	curve25519_mul(x, x, fe_sqrtm1);
	curve25519_sub_reduce(y, w, x);
	if (curve25519_isnonzero(y)) {
		assert((curve25519_add_reduce(y, w, x), !curve25519_isnonzero(y)));
		curve25519_mul(r->x, r->x, fe_fffb3);
	} else {
		curve25519_mul(r->x, r->x, fe_fffb4);
	}
	/* r->x = sqrt(A * (A + 2) * w / x) */
	/* z = -A */
	sign = 1;
setsign:
	if (curve25519_isnegative(r->x) != sign) {
		assert(curve25519_isnonzero(r->x));
		curve25519_neg(r->x, r->x);
	}
	curve25519_add_reduce(r->z, z, w);
	curve25519_sub_reduce(r->y, z, w);
	curve25519_mul(r->x, r->x, r->z);

	// Partial form, saving from T coord computation .
	// Later is mul8 discarding T anyway.
	// rt = ((rx * ry % q) * inv(rz)) % q
	// curve25519_mul(x, r->x, r->y);
	// curve25519_recip(z, r->z);
	// curve25519_mul(r->t, x, z);

#if !defined(NDEBUG)
	{
		bignum25519 check_x={0}, check_y={0}, check_iz={0}, check_v={0};
		curve25519_recip(check_iz, r->z);
		curve25519_mul(check_x, r->x, check_iz);
		curve25519_mul(check_y, r->y, check_iz);
		curve25519_square(check_x, check_x);
		curve25519_square(check_y, check_y);
		curve25519_mul(check_v, check_x, check_y);
		curve25519_mul(check_v, fe_d, check_v);
		curve25519_add_reduce(check_v, check_v, check_x);
		curve25519_sub_reduce(check_v, check_v, check_y);
		curve25519_set(check_x, 1);
		curve25519_add_reduce(check_v, check_v, check_x);
		assert(!curve25519_isnonzero(check_v));
	}
#endif
}

int ge25519_unpack_vartime(ge25519 *r, const unsigned char *s){
	int res = ge25519_unpack_negative_vartime(r, s);
	ge25519_neg_full(r);
	return res;
}

void ge25519_scalarmult_base_wrapper(ge25519 *r, const bignum256modm s){
	ge25519_scalarmult_base_niels(r, ge25519_niels_base_multiples, s);
}
