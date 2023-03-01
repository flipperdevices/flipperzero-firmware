/*
	Timing safe memory compare
*/
int ed25519_verify(const unsigned char *x, const unsigned char *y, size_t len);

/*
	conversions
*/

void ge25519_p1p1_to_partial(ge25519 *r, const ge25519_p1p1 *p);

void ge25519_p1p1_to_full(ge25519 *r, const ge25519_p1p1 *p);

void ge25519_full_to_pniels(ge25519_pniels *p, const ge25519 *r);

/*
	adding & doubling
*/

void ge25519_double_p1p1(ge25519_p1p1 *r, const ge25519 *p);

#ifndef ED25519_NO_PRECOMP
void ge25519_nielsadd2_p1p1(ge25519_p1p1 *r, const ge25519 *p, const ge25519_niels *q, unsigned char signbit);
#endif

/* computes [s1]p1 + [s2]p2 */
#if USE_MONERO
void ge25519_double_scalarmult_vartime2(ge25519 *r, const ge25519 *p1, const bignum256modm s1, const ge25519 *p2, const bignum256modm s2);
#endif

void ge25519_pnielsadd_p1p1(ge25519_p1p1 *r, const ge25519 *p, const ge25519_pniels *q, unsigned char signbit);

void ge25519_double_partial(ge25519 *r, const ge25519 *p);

void ge25519_double(ge25519 *r, const ge25519 *p);

void ge25519_nielsadd2(ge25519 *r, const ge25519_niels *q);

void ge25519_pnielsadd(ge25519_pniels *r, const ge25519 *p, const ge25519_pniels *q);


/*
	pack & unpack
*/

void ge25519_pack(unsigned char r[32], const ge25519 *p);

int ge25519_unpack_negative_vartime(ge25519 *r, const unsigned char p[32]);

/*
	scalarmults
*/

void ge25519_set_neutral(ge25519 *r);

/* computes [s1]p1 + [s2]base */
void ge25519_double_scalarmult_vartime(ge25519 *r, const ge25519 *p1, const bignum256modm s1, const bignum256modm s2);

/* computes [s1]p1, constant time */
void ge25519_scalarmult(ge25519 *r, const ge25519 *p1, const bignum256modm s1);

void ge25519_scalarmult_base_choose_niels(ge25519_niels *t, const uint8_t table[256][96], uint32_t pos, signed char b);

/* computes [s]basepoint */
void ge25519_scalarmult_base_niels(ge25519 *r, const uint8_t basepoint_table[256][96], const bignum256modm s);

/* check if r is on curve */
int ge25519_check(const ge25519 *r);

/* a == b */
int ge25519_eq(const ge25519 *a, const ge25519 *b);

/* copies one point to another */
void ge25519_copy(ge25519 *dst, const ge25519 *src);

/* sets B point to r */
void ge25519_set_base(ge25519 *r);

/* 8*P */
void ge25519_mul8(ge25519 *r, const ge25519 *t);

/* -P */
void ge25519_neg_partial(ge25519 *r);

/* -P */
void ge25519_neg_full(ge25519 *r);

/* reduce all coords */
void ge25519_reduce(ge25519 *r, const ge25519 *t);

/* normalizes coords. (x, y, 1, x*y) */
void ge25519_norm(ge25519 *r, const ge25519 * t);

/* Simple addition */
void ge25519_add(ge25519 *r, const ge25519 *a, const ge25519 *b, unsigned char signbit);

/* point from bytes, used in H_p() */
void ge25519_fromfe_frombytes_vartime(ge25519 *r, const unsigned char *s);

/* point from bytes */
int ge25519_unpack_vartime(ge25519 *r, const unsigned char *s);

/* aG, wrapper for niels base mult. */
void ge25519_scalarmult_base_wrapper(ge25519 *r, const bignum256modm s);
