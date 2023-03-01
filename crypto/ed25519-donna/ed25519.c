/*
	Public domain by Andrew M. <liquidsun@gmail.com>

	Ed25519 reference implementation using Ed25519-donna
*/


/* define ED25519_SUFFIX to have it appended to the end of each public function */
#ifdef ED25519_SUFFIX
#define ED25519_FN3(fn,suffix) fn##suffix
#define ED25519_FN2(fn,suffix) ED25519_FN3(fn,suffix)
#define ED25519_FN(fn) ED25519_FN2(fn,ED25519_SUFFIX)
#else
#define ED25519_FN(fn) fn
#endif

#include "ed25519-donna.h"
#include "ed25519.h"

#include "ed25519-hash-custom.h"
#include "rand.h"
#include "memzero.h"

/*
	Generates a (extsk[0..31]) and aExt (extsk[32..63])
*/
DONNA_INLINE static void
ed25519_extsk(hash_512bits extsk, const ed25519_secret_key sk) {
	ed25519_hash(extsk, sk, 32);
	extsk[0] &= 248;
	extsk[31] &= 127;
	extsk[31] |= 64;
}

static void
ed25519_hram(hash_512bits hram, const ed25519_public_key R, const ed25519_public_key pk, const unsigned char *m, size_t mlen) {
	ed25519_hash_context ctx;
	ed25519_hash_init(&ctx);
	ed25519_hash_update(&ctx, R, 32);
	ed25519_hash_update(&ctx, pk, 32);
	ed25519_hash_update(&ctx, m, mlen);
	ed25519_hash_final(&ctx, hram);
}

void
ED25519_FN(ed25519_publickey) (const ed25519_secret_key sk, ed25519_public_key pk) {
	hash_512bits extsk = {0};
	ed25519_extsk(extsk, sk);
	ed25519_publickey_ext(extsk, pk);
	memzero(&extsk, sizeof(extsk));
}

void
ED25519_FN(ed25519_cosi_commit) (ed25519_secret_key nonce, ed25519_public_key commitment) {
	bignum256modm r = {0};
	ge25519 ALIGN(16) R;
	unsigned char extnonce[64] = {0};

	/* r = random512 mod L */
	random_buffer(extnonce, sizeof(extnonce));
	expand256_modm(r, extnonce, sizeof(extnonce));
	memzero(&extnonce, sizeof(extnonce));
	contract256_modm(nonce, r);

	/* R = rB */
	ge25519_scalarmult_base_niels(&R, ge25519_niels_base_multiples, r);
	memzero(&r, sizeof(r));
	ge25519_pack(commitment, &R);
}

int
ED25519_FN(ed25519_cosi_sign) (const unsigned char *m, size_t mlen, const ed25519_secret_key sk, const ed25519_secret_key nonce, const ed25519_public_key R, const ed25519_public_key pk, ed25519_cosi_signature sig) {
	bignum256modm r = {0}, S = {0}, a = {0};
	hash_512bits extsk = {0}, hram = {0};

	ed25519_extsk(extsk, sk);

	/* r */
	expand_raw256_modm(r, nonce);
	if (!is_reduced256_modm(r))
		return -1;

	/* S = H(R,A,m).. */
	ed25519_hram(hram, R, pk, m, mlen);
	expand256_modm(S, hram, 64);

	/* S = H(R,A,m)a */
	expand256_modm(a, extsk, 32);
	memzero(&extsk, sizeof(extsk));
	mul256_modm(S, S, a);
	memzero(&a, sizeof(a));

	/* S = (r + H(R,A,m)a) */
	add256_modm(S, S, r);
	memzero(&r, sizeof(r));

	/* S = (r + H(R,A,m)a) mod L */
	contract256_modm(sig, S);

	return 0;
}

void
ED25519_FN(ed25519_sign_ext) (const unsigned char *m, size_t mlen, const ed25519_secret_key sk, const ed25519_secret_key skext, ed25519_signature RS) {
	ed25519_hash_context ctx;
	bignum256modm r = {0}, S = {0}, a = {0};
	ge25519 ALIGN(16) R = {0};
	ge25519 ALIGN(16) A = {0};
	ed25519_public_key pk = {0};
	hash_512bits extsk = {0}, hashr = {0}, hram = {0};

	/* we don't stretch the key through hashing first since its already 64 bytes */

	memcpy(extsk, sk, 32);
	memcpy(extsk+32, skext, 32);


	/* r = H(aExt[32..64], m) */
	ed25519_hash_init(&ctx);
	ed25519_hash_update(&ctx, extsk + 32, 32);
	ed25519_hash_update(&ctx, m, mlen);
	ed25519_hash_final(&ctx, hashr);
	expand256_modm(r, hashr, 64);
	memzero(&hashr, sizeof(hashr));

	/* R = rB */
	ge25519_scalarmult_base_niels(&R, ge25519_niels_base_multiples, r);
	ge25519_pack(RS, &R);

	/* a = aExt[0..31] */
	expand256_modm(a, extsk, 32);
	memzero(&extsk, sizeof(extsk));

	/* A = aB */
	ge25519_scalarmult_base_niels(&A, ge25519_niels_base_multiples, a);
	ge25519_pack(pk, &A);

	/* S = H(R,A,m).. */
	ed25519_hram(hram, RS, pk, m, mlen);
	expand256_modm(S, hram, 64);

	/* S = H(R,A,m)a */
	mul256_modm(S, S, a);
	memzero(&a, sizeof(a));

	/* S = (r + H(R,A,m)a) */
	add256_modm(S, S, r);
	memzero(&r, sizeof(r));

	/* S = (r + H(R,A,m)a) mod L */
	contract256_modm(RS + 32, S);
}

void
ED25519_FN(ed25519_sign) (const unsigned char *m, size_t mlen, const ed25519_secret_key sk, ed25519_signature RS) {
	hash_512bits extsk = {0};
	ed25519_extsk(extsk, sk);
	ED25519_FN(ed25519_sign_ext)(m, mlen, extsk, extsk + 32, RS);
	memzero(&extsk, sizeof(extsk));
}

int
ED25519_FN(ed25519_sign_open) (const unsigned char *m, size_t mlen, const ed25519_public_key pk, const ed25519_signature RS) {
	ge25519 ALIGN(16) R = {0}, A = {0};
	hash_512bits hash = {0};
	bignum256modm hram = {0}, S = {0};
	unsigned char checkR[32] = {0};

	if ((RS[63] & 224) || !ge25519_unpack_negative_vartime(&A, pk))
		return -1;

	/* hram = H(R,A,m) */
	ed25519_hram(hash, RS, pk, m, mlen);
	expand256_modm(hram, hash, 64);

	/* S */
	expand_raw256_modm(S, RS + 32);
	if (!is_reduced256_modm(S))
		return -1;

	/* SB - H(R,A,m)A */
	ge25519_double_scalarmult_vartime(&R, &A, hram, S);
	ge25519_pack(checkR, &R);

	/* check that R = SB - H(R,A,m)A */
	return ed25519_verify(RS, checkR, 32) ? 0 : -1;
}

int
ED25519_FN(ed25519_scalarmult) (ed25519_public_key res, const ed25519_secret_key sk, const ed25519_public_key pk) {
	bignum256modm a = {0};
	ge25519 ALIGN(16) A = {0}, P = {0};
	hash_512bits extsk = {0};

	ed25519_extsk(extsk, sk);
	expand256_modm(a, extsk, 32);
	memzero(&extsk, sizeof(extsk));

	if (!ge25519_unpack_negative_vartime(&P, pk)) {
		return -1;
	}

	ge25519_scalarmult(&A, &P, a);
	memzero(&a, sizeof(a));
	curve25519_neg(A.x, A.x);
	ge25519_pack(res, &A);
	return 0;
}


#ifndef ED25519_SUFFIX

#include "curve25519-donna-scalarmult-base.h"

void
ed25519_publickey_ext(const ed25519_secret_key extsk, ed25519_public_key pk) {
	bignum256modm a = {0};
	ge25519 ALIGN(16) A = {0};

	expand256_modm(a, extsk, 32);

	/* A = aB */
	ge25519_scalarmult_base_niels(&A, ge25519_niels_base_multiples, a);
	memzero(&a, sizeof(a));
	ge25519_pack(pk, &A);
}

int
ed25519_cosi_combine_publickeys(ed25519_public_key res, CONST ed25519_public_key *pks, size_t n) {
	size_t i = 0;
	ge25519 P = {0};
	ge25519_pniels sump = {0};
	ge25519_p1p1 sump1 = {0};

	if (n == 1) {
		memcpy(res, pks, sizeof(ed25519_public_key));
		return 0;
	}
	if (!ge25519_unpack_negative_vartime(&P, pks[i++])) {
		return -1;
	}
	ge25519_full_to_pniels(&sump, &P);
	while (i < n - 1) {
		if (!ge25519_unpack_negative_vartime(&P, pks[i++])) {
			return -1;
		}
		ge25519_pnielsadd(&sump, &P, &sump);
	}
	if (!ge25519_unpack_negative_vartime(&P, pks[i++])) {
		return -1;
	}
	ge25519_pnielsadd_p1p1(&sump1, &P, &sump, 0);
	ge25519_p1p1_to_partial(&P, &sump1);
	curve25519_neg(P.x, P.x);
	ge25519_pack(res, &P);
	return 0;
}

void
ed25519_cosi_combine_signatures(ed25519_signature res, const ed25519_public_key R, CONST ed25519_cosi_signature *sigs, size_t n) {
	bignum256modm s = {0}, t = {0};
	size_t i = 0;

	expand256_modm(s, sigs[i++], 32);
	while (i < n) {
		expand256_modm(t, sigs[i++], 32);
		add256_modm(s, s, t);
	}
	memcpy(res, R, 32);
	contract256_modm(res + 32, s);
}

/*
	Fast Curve25519 basepoint scalar multiplication
*/
void
curve25519_scalarmult_basepoint(curve25519_key pk, const curve25519_key e) {
	curve25519_key ec = {0};
	bignum256modm s = {0};
	bignum25519 ALIGN(16) yplusz = {0}, zminusy = {0};
	ge25519 ALIGN(16) p = {0};
	size_t i = 0;

	/* clamp */
	for (i = 0; i < 32; i++) ec[i] = e[i];
	ec[0] &= 248;
	ec[31] &= 127;
	ec[31] |= 64;

	expand_raw256_modm(s, ec);
	memzero(&ec, sizeof(ec));

	/* scalar * basepoint */
	ge25519_scalarmult_base_niels(&p, ge25519_niels_base_multiples, s);
	memzero(&s, sizeof(s));

	/* u = (y + z) / (z - y) */
	curve25519_add(yplusz, p.y, p.z);
	curve25519_sub(zminusy, p.z, p.y);
	curve25519_recip(zminusy, zminusy);
	curve25519_mul(yplusz, yplusz, zminusy);
	curve25519_contract(pk, yplusz);
}

void
curve25519_scalarmult(curve25519_key mypublic, const curve25519_key secret, const curve25519_key basepoint) {
	curve25519_key e = {0};
	size_t i = 0;

	for (i = 0;i < 32;++i) e[i] = secret[i];
	e[0] &= 0xf8;
	e[31] &= 0x7f;
	e[31] |= 0x40;
	curve25519_scalarmult_donna(mypublic, e, basepoint);
	memzero(&e, sizeof(e));
}

#endif // ED25519_SUFFIX
