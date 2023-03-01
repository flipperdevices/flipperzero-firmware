/*
	Public domain by Andrew M. <liquidsun@gmail.com>
*/


/*
	Arithmetic modulo the group order n = 2^252 +  27742317777372353535851937790883648493 = 7237005577332262213973186563042994240857116359379907606001950938285454250989

	k = 32
	b = 1 << 8 = 256
	m = 2^252 + 27742317777372353535851937790883648493 = 0x1000000000000000000000000000000014def9dea2f79cd65812631a5cf5d3ed
	mu = floor( b^(k*2) / m ) = 0xfffffffffffffffffffffffffffffffeb2106215d086329a7ed9ce5a30a2c131b
*/

#define bignum256modm_bits_per_limb 30
#define bignum256modm_limb_size 9

typedef uint32_t bignum256modm_element_t;
typedef bignum256modm_element_t bignum256modm[9];

/* see HAC, Alg. 14.42 Step 4 */
void reduce256_modm(bignum256modm r);

/*
	Barrett reduction,  see HAC, Alg. 14.42

	Instead of passing in x, pre-process in to q1 and r1 for efficiency
*/
void barrett_reduce256_modm(bignum256modm r, const bignum256modm q1, const bignum256modm r1);

/* addition modulo m */
void add256_modm(bignum256modm r, const bignum256modm x, const bignum256modm y);

/* -x modulo m */
void neg256_modm(bignum256modm r, const bignum256modm x);

/* subtraction x-y modulo m */
void sub256_modm(bignum256modm r, const bignum256modm x, const bignum256modm y);

/* multiplication modulo m */
void mul256_modm(bignum256modm r, const bignum256modm x, const bignum256modm y);

void expand256_modm(bignum256modm out, const unsigned char *in, size_t len);

void expand_raw256_modm(bignum256modm out, const unsigned char in[32]);

int is_reduced256_modm(const bignum256modm in);

void contract256_modm(unsigned char out[32], const bignum256modm in);

void contract256_window4_modm(signed char r[64], const bignum256modm in);

void contract256_slidingwindow_modm(signed char r[256], const bignum256modm s, int windowsize);

/* 64bit uint to scalar value */
void set256_modm(bignum256modm r, uint64_t v);

/* scalar value to 64bit uint */
int get256_modm(uint64_t * v, const bignum256modm r);

/* equality test on two reduced scalar values */
int eq256_modm(const bignum256modm x, const bignum256modm y);

/* comparison of two reduced scalar values */
int cmp256_modm(const bignum256modm x, const bignum256modm y);

/* scalar null check, has to be reduced */
int iszero256_modm(const bignum256modm x);

/* simple copy, no reduction */
void copy256_modm(bignum256modm r, const bignum256modm x);

/* check if nonzero && same after reduction */
int check256_modm(const bignum256modm x);

/* (cc - aa * bb) % l */
void mulsub256_modm(bignum256modm r, const bignum256modm a, const bignum256modm b, const bignum256modm c);

/* (cc + aa * bb) % l */
void muladd256_modm(bignum256modm r, const bignum256modm a, const bignum256modm b, const bignum256modm c);
