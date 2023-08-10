/* sp.h
 *
 * Copyright (C) 2006-2023 wolfSSL Inc.
 *
 * This file is part of wolfSSL.
 *
 * wolfSSL is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * wolfSSL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1335, USA
 */


#ifndef WOLF_CRYPT_SP_H
#define WOLF_CRYPT_SP_H

#include <wolfssl/wolfcrypt/types.h>

#if defined(WOLFSSL_HAVE_SP_RSA) || defined(WOLFSSL_HAVE_SP_DH) || \
                                    defined(WOLFSSL_HAVE_SP_ECC)
#ifdef _WIN32_WCE
    typedef __int8           int8_t;
    typedef __int32          int32_t;
    typedef __int64          int64_t;
    typedef unsigned __int8  uint8_t;
    typedef unsigned __int32 uint32_t;
    typedef unsigned __int64 uint64_t;
#elif !defined(WOLFSSL_LINUXKM)
    #include <stdint.h>
#endif

#include <wolfssl/wolfcrypt/wolfmath.h>
#include <wolfssl/wolfcrypt/sp_int.h>

#if defined(HAVE_ECC) && defined(WOLFSSL_HAVE_SP_ECC)
    #include <wolfssl/wolfcrypt/ecc.h>
#else
    #undef WOLFSSL_HAVE_SP_ECC
#endif

#ifdef noinline
    #define SP_NOINLINE noinline
#elif defined(_MSC_VER)
    #define SP_NOINLINE __declspec(noinline)
#elif defined(__ICCARM__) || defined(__IAR_SYSTEMS_ICC__)
    #define SP_NOINLINE _Pragma("inline = never")
#elif defined(__GNUC__) || defined(__KEIL__) || defined(__DCC__)
    #define SP_NOINLINE __attribute__((noinline))
#else
    #define SP_NOINLINE
#endif


#ifdef __cplusplus
    extern "C" {
#endif

#ifdef WOLFSSL_HAVE_SP_RSA

/* non-const versions only needed for inlined ARM assembly */
#if defined(HAVE_FIPS_VERSION) && HAVE_FIPS_VERSION == 2 \
    && ( defined(WOLFSSL_SP_ARM32_ASM) || defined(WOLFSSL_SP_ARM64_ASM) )

WOLFSSL_LOCAL int sp_RsaPublic_2048(const byte* in, word32 inLen,
    mp_int* em, mp_int* mm, byte* out, word32* outLen);
WOLFSSL_LOCAL int sp_RsaPrivate_2048(const byte* in, word32 inLen,
    mp_int* dm, mp_int* pm, mp_int* qm, mp_int* dpm, mp_int* dqm, mp_int* qim,
    mp_int* mm, byte* out, word32* outLen);

WOLFSSL_LOCAL int sp_RsaPublic_3072(const byte* in, word32 inLen,
    mp_int* em, mp_int* mm, byte* out, word32* outLen);
WOLFSSL_LOCAL int sp_RsaPrivate_3072(const byte* in, word32 inLen,
    mp_int* dm, mp_int* pm, mp_int* qm, mp_int* dpm, mp_int* dqm, mp_int* qim,
    mp_int* mm, byte* out, word32* outLen);

WOLFSSL_LOCAL int sp_RsaPublic_4096(const byte* in, word32 inLen,
    mp_int* em, mp_int* mm, byte* out, word32* outLen);
WOLFSSL_LOCAL int sp_RsaPrivate_4096(const byte* in, word32 inLen,
    mp_int* dm, mp_int* pm, mp_int* qm, mp_int* dpm, mp_int* dqm, mp_int* qim,
    mp_int* mm, byte* out, word32* outLen);

#else

WOLFSSL_LOCAL int sp_RsaPublic_2048(const byte* in, word32 inLen,
    const mp_int* em, const mp_int* mm, byte* out, word32* outLen);
WOLFSSL_LOCAL int sp_RsaPrivate_2048(const byte* in, word32 inLen,
    const mp_int* dm, const mp_int* pm, const mp_int* qm, const mp_int* dpm,
    const mp_int* dqm, const mp_int* qim, const mp_int* mm, byte* out,
    word32* outLen);

WOLFSSL_LOCAL int sp_RsaPublic_3072(const byte* in, word32 inLen,
    const mp_int* em, const mp_int* mm, byte* out, word32* outLen);
WOLFSSL_LOCAL int sp_RsaPrivate_3072(const byte* in, word32 inLen,
    const mp_int* dm, const mp_int* pm, const mp_int* qm, const mp_int* dpm,
    const mp_int* dqm, const mp_int* qim, const mp_int* mm, byte* out,
    word32* outLen);

WOLFSSL_LOCAL int sp_RsaPublic_4096(const byte* in, word32 inLen,
    const mp_int* em, const mp_int* mm, byte* out, word32* outLen);
WOLFSSL_LOCAL int sp_RsaPrivate_4096(const byte* in, word32 inLen,
    const mp_int* dm, const mp_int* pm, const mp_int* qm, const mp_int* dpm,
    const mp_int* dqm, const mp_int* qim, const mp_int* mm, byte* out,
    word32* outLen);

#endif /* HAVE_FIPS_VERSION && HAVE_FIPS_VERSION == 2 && !WOLFSSL_SP_ARM[32|64]_ASM */

#endif /* WOLFSSL_HAVE_SP_RSA */

#if defined(WOLFSSL_HAVE_SP_DH) || defined(WOLFSSL_HAVE_SP_RSA)

/* non-const versions only needed for inlined ARM assembly */
#if defined(HAVE_FIPS_VERSION) && HAVE_FIPS_VERSION == 2 \
    && ( defined(WOLFSSL_SP_ARM32_ASM) || defined(WOLFSSL_SP_ARM64_ASM) )

WOLFSSL_LOCAL int sp_ModExp_1024(mp_int* base, mp_int* exp, mp_int* mod,
    mp_int* res);
WOLFSSL_LOCAL int sp_ModExp_1536(mp_int* base, mp_int* exp, mp_int* mod,
    mp_int* res);
WOLFSSL_LOCAL int sp_ModExp_2048(mp_int* base, mp_int* exp, mp_int* mod,
    mp_int* res);
WOLFSSL_LOCAL int sp_ModExp_3072(mp_int* base, mp_int* exp, mp_int* mod,
    mp_int* res);
WOLFSSL_LOCAL int sp_ModExp_4096(mp_int* base, mp_int* exp, mp_int* mod,
    mp_int* res);

#else

WOLFSSL_LOCAL int sp_ModExp_1024(const mp_int* base, const mp_int* exp,
    const mp_int* mod, mp_int* res);
WOLFSSL_LOCAL int sp_ModExp_1536(const mp_int* base, const mp_int* exp,
    const mp_int* mod, mp_int* res);
WOLFSSL_LOCAL int sp_ModExp_2048(const mp_int* base, const mp_int* exp,
    const mp_int* mod, mp_int* res);
WOLFSSL_LOCAL int sp_ModExp_3072(const mp_int* base, const mp_int* exp,
    const mp_int* mod, mp_int* res);
WOLFSSL_LOCAL int sp_ModExp_4096(const mp_int* base, const mp_int* exp,
    const mp_int* mod, mp_int* res);

#endif /* HAVE_FIPS_VERSION && HAVE_FIPS_VERSION == 2 && !WOLFSSL_SP_ARM[32|64]_ASM */

#endif

#ifdef WOLFSSL_HAVE_SP_DH

/* non-const versions only needed for inlined ARM assembly */
#if defined(HAVE_FIPS_VERSION) && HAVE_FIPS_VERSION == 2 \
    && ( defined(WOLFSSL_SP_ARM32_ASM) || defined(WOLFSSL_SP_ARM64_ASM) )

WOLFSSL_LOCAL int sp_DhExp_2048(mp_int* base, const byte* exp, word32 expLen,
    mp_int* mod, byte* out, word32* outLen);
WOLFSSL_LOCAL int sp_DhExp_3072(mp_int* base, const byte* exp, word32 expLen,
    mp_int* mod, byte* out, word32* outLen);
WOLFSSL_LOCAL int sp_DhExp_4096(mp_int* base, const byte* exp, word32 expLen,
    mp_int* mod, byte* out, word32* outLen);

#else

WOLFSSL_LOCAL int sp_DhExp_2048(const mp_int* base, const byte* exp,
    word32 expLen, const mp_int* mod, byte* out, word32* outLen);
WOLFSSL_LOCAL int sp_DhExp_3072(const mp_int* base, const byte* exp,
    word32 expLen, const mp_int* mod, byte* out, word32* outLen);
WOLFSSL_LOCAL int sp_DhExp_4096(const mp_int* base, const byte* exp,
    word32 expLen, const mp_int* mod, byte* out, word32* outLen);

#endif /* HAVE_FIPS_VERSION && HAVE_FIPS_VERSION == 2 && !WOLFSSL_SP_ARM[32|64]_ASM */

#endif /* WOLFSSL_HAVE_SP_DH */

#ifdef WOLFSSL_HAVE_SP_ECC

/* non-const versions only needed for inlined ARM assembly */
#if defined(HAVE_FIPS_VERSION) && HAVE_FIPS_VERSION == 2 \
    && ( defined(WOLFSSL_SP_ARM32_ASM) || defined(WOLFSSL_SP_ARM64_ASM) )

int sp_ecc_mulmod_256(mp_int* km, ecc_point* gm, ecc_point* rm, int map,
                      void* heap);
int sp_ecc_mulmod_base_256(mp_int* km, ecc_point* rm, int map, void* heap);

int sp_ecc_make_key_256(WC_RNG* rng, mp_int* priv, ecc_point* pub, void* heap);
int sp_ecc_secret_gen_256(mp_int* priv, ecc_point* pub, byte* out,
                          word32* outlen, void* heap);
int sp_ecc_sign_256(const byte* hash, word32 hashLen, WC_RNG* rng, mp_int* priv,
                    mp_int* rm, mp_int* sm, mp_int* km, void* heap);
int sp_ecc_verify_256(const byte* hash, word32 hashLen, mp_int* pX, mp_int* pY,
                      mp_int* pZ, mp_int* r, mp_int* sm, int* res, void* heap);
int sp_ecc_is_point_256(mp_int* pX, mp_int* pY);
int sp_ecc_check_key_256(mp_int* pX, mp_int* pY, mp_int* privm, void* heap);
int sp_ecc_proj_add_point_256(mp_int* pX, mp_int* pY, mp_int* pZ,
                              mp_int* qX, mp_int* qY, mp_int* qZ,
                              mp_int* rX, mp_int* rY, mp_int* rZ);
int sp_ecc_proj_dbl_point_256(mp_int* pX, mp_int* pY, mp_int* pZ,
                              mp_int* rX, mp_int* rY, mp_int* rZ);
int sp_ecc_map_256(mp_int* pX, mp_int* pY, mp_int* pZ);
int sp_ecc_uncompress_256(mp_int* xm, int odd, mp_int* ym);


int sp_ecc_mulmod_384(mp_int* km, ecc_point* gm, ecc_point* rm, int map,
                      void* heap);
int sp_ecc_mulmod_base_384(mp_int* km, ecc_point* rm, int map, void* heap);

int sp_ecc_make_key_384(WC_RNG* rng, mp_int* priv, ecc_point* pub, void* heap);
int sp_ecc_secret_gen_384(mp_int* priv, ecc_point* pub, byte* out,
                          word32* outlen, void* heap);
int sp_ecc_sign_384(const byte* hash, word32 hashLen, WC_RNG* rng, mp_int* priv,
                    mp_int* rm, mp_int* sm, mp_int* km, void* heap);
int sp_ecc_verify_384(const byte* hash, word32 hashLen, mp_int* pX, mp_int* pY,
                      mp_int* pZ, mp_int* r, mp_int* sm, int* res, void* heap);
int sp_ecc_is_point_384(mp_int* pX, mp_int* pY);
int sp_ecc_check_key_384(mp_int* pX, mp_int* pY, mp_int* privm, void* heap);
int sp_ecc_proj_add_point_384(mp_int* pX, mp_int* pY, mp_int* pZ,
                              mp_int* qX, mp_int* qY, mp_int* qZ,
                              mp_int* rX, mp_int* rY, mp_int* rZ);
int sp_ecc_proj_dbl_point_384(mp_int* pX, mp_int* pY, mp_int* pZ,
                              mp_int* rX, mp_int* rY, mp_int* rZ);
int sp_ecc_map_384(mp_int* pX, mp_int* pY, mp_int* pZ);
int sp_ecc_uncompress_384(mp_int* xm, int odd, mp_int* ym);

#else

WOLFSSL_LOCAL int sp_ecc_mulmod_256(const mp_int* km, const ecc_point* gm,
    ecc_point* rm, int map, void* heap);
WOLFSSL_LOCAL int sp_ecc_mulmod_add_256(const mp_int* km, const ecc_point* gm,
    const ecc_point* am, int inMont, ecc_point* rm, int map, void* heap);
WOLFSSL_LOCAL int sp_ecc_mulmod_base_256(const mp_int* km, ecc_point* rm,
    int map, void* heap);
WOLFSSL_LOCAL int sp_ecc_mulmod_base_add_256(const mp_int* km,
    const ecc_point* am, int inMont, ecc_point* rm, int map, void* heap);

WOLFSSL_LOCAL int sp_ecc_make_key_256(WC_RNG* rng, mp_int* priv, ecc_point* pub,
    void* heap);
WOLFSSL_LOCAL int sp_ecc_secret_gen_256(const mp_int* priv,
    const ecc_point* pub, byte* out, word32* outlen, void* heap);
WOLFSSL_LOCAL int sp_ecc_sign_256(const byte* hash, word32 hashLen, WC_RNG* rng,
    const mp_int* priv, mp_int* rm, mp_int* sm, mp_int* km, void* heap);
WOLFSSL_LOCAL int sp_ecc_verify_256(const byte* hash, word32 hashLen,
    const mp_int* pX, const mp_int* pY, const mp_int* pZ, const mp_int* r,
    const mp_int* sm, int* res, void* heap);
WOLFSSL_LOCAL int sp_ecc_is_point_256(const mp_int* pX, const mp_int* pY);
WOLFSSL_LOCAL int sp_ecc_check_key_256(const mp_int* pX, const mp_int* pY,
    const mp_int* privm, void* heap);
WOLFSSL_LOCAL int sp_ecc_proj_add_point_256(mp_int* pX, mp_int* pY, mp_int* pZ,
    mp_int* qX, mp_int* qY, mp_int* qZ, mp_int* rX, mp_int* rY, mp_int* rZ);
WOLFSSL_LOCAL int sp_ecc_proj_dbl_point_256(mp_int* pX, mp_int* pY, mp_int* pZ,
    mp_int* rX, mp_int* rY, mp_int* rZ);
WOLFSSL_LOCAL int sp_ecc_map_256(mp_int* pX, mp_int* pY, mp_int* pZ);
WOLFSSL_LOCAL int sp_ecc_uncompress_256(mp_int* xm, int odd, mp_int* ym);

WOLFSSL_LOCAL int sp_ecc_mulmod_384(const mp_int* km, const ecc_point* gm,
    ecc_point* rm, int map, void* heap);
WOLFSSL_LOCAL int sp_ecc_mulmod_add_384(const mp_int* km, const ecc_point* gm,
    const ecc_point* am, int inMont, ecc_point* rm, int map, void* heap);
WOLFSSL_LOCAL int sp_ecc_mulmod_base_384(const mp_int* km, ecc_point* rm,
    int map, void* heap);
WOLFSSL_LOCAL int sp_ecc_mulmod_base_add_384(const mp_int* km,
    const ecc_point* am, int inMont, ecc_point* rm, int map, void* heap);

WOLFSSL_LOCAL int sp_ecc_make_key_384(WC_RNG* rng, mp_int* priv, ecc_point* pub, void* heap);
WOLFSSL_LOCAL int sp_ecc_secret_gen_384(const mp_int* priv,
    const ecc_point* pub, byte* out, word32* outlen, void* heap);
WOLFSSL_LOCAL int sp_ecc_sign_384(const byte* hash, word32 hashLen, WC_RNG* rng,
    const mp_int* priv, mp_int* rm, mp_int* sm, mp_int* km, void* heap);
WOLFSSL_LOCAL int sp_ecc_verify_384(const byte* hash, word32 hashLen,
    const mp_int* pX, const mp_int* pY, const mp_int* pZ, const mp_int* r,
    const mp_int* sm, int* res, void* heap);
WOLFSSL_LOCAL int sp_ecc_is_point_384(const mp_int* pX, const mp_int* pY);
WOLFSSL_LOCAL int sp_ecc_check_key_384(const mp_int* pX, const mp_int* pY,
    const mp_int* privm, void* heap);
WOLFSSL_LOCAL int sp_ecc_proj_add_point_384(mp_int* pX, mp_int* pY, mp_int* pZ,
    mp_int* qX, mp_int* qY, mp_int* qZ, mp_int* rX, mp_int* rY, mp_int* rZ);
WOLFSSL_LOCAL int sp_ecc_proj_dbl_point_384(mp_int* pX, mp_int* pY, mp_int* pZ,
    mp_int* rX, mp_int* rY, mp_int* rZ);
WOLFSSL_LOCAL int sp_ecc_map_384(mp_int* pX, mp_int* pY, mp_int* pZ);
WOLFSSL_LOCAL int sp_ecc_uncompress_384(mp_int* xm, int odd, mp_int* ym);

WOLFSSL_LOCAL int sp_ecc_mulmod_521(const mp_int* km, const ecc_point* gm,
    ecc_point* rm, int map, void* heap);
WOLFSSL_LOCAL int sp_ecc_mulmod_add_521(const mp_int* km, const ecc_point* gm,
    const ecc_point* am, int inMont, ecc_point* rm, int map, void* heap);
WOLFSSL_LOCAL int sp_ecc_mulmod_base_521(const mp_int* km, ecc_point* rm,
    int map, void* heap);
WOLFSSL_LOCAL int sp_ecc_mulmod_base_add_521(const mp_int* km,
    const ecc_point* am, int inMont, ecc_point* rm, int map, void* heap);

WOLFSSL_LOCAL int sp_ecc_make_key_521(WC_RNG* rng, mp_int* priv, ecc_point* pub, void* heap);
WOLFSSL_LOCAL int sp_ecc_secret_gen_521(const mp_int* priv,
    const ecc_point* pub, byte* out, word32* outlen, void* heap);
WOLFSSL_LOCAL int sp_ecc_sign_521(const byte* hash, word32 hashLen, WC_RNG* rng,
    const mp_int* priv, mp_int* rm, mp_int* sm, mp_int* km, void* heap);
WOLFSSL_LOCAL int sp_ecc_verify_521(const byte* hash, word32 hashLen,
    const mp_int* pX, const mp_int* pY, const mp_int* pZ, const mp_int* r,
    const mp_int* sm, int* res, void* heap);
WOLFSSL_LOCAL int sp_ecc_is_point_521(const mp_int* pX, const mp_int* pY);
WOLFSSL_LOCAL int sp_ecc_check_key_521(const mp_int* pX, const mp_int* pY,
    const mp_int* privm, void* heap);
WOLFSSL_LOCAL int sp_ecc_proj_add_point_521(mp_int* pX, mp_int* pY, mp_int* pZ,
    mp_int* qX, mp_int* qY, mp_int* qZ, mp_int* rX, mp_int* rY, mp_int* rZ);
WOLFSSL_LOCAL int sp_ecc_proj_dbl_point_521(mp_int* pX, mp_int* pY, mp_int* pZ,
    mp_int* rX, mp_int* rY, mp_int* rZ);
WOLFSSL_LOCAL int sp_ecc_map_521(mp_int* pX, mp_int* pY, mp_int* pZ);
WOLFSSL_LOCAL int sp_ecc_uncompress_521(mp_int* xm, int odd, mp_int* ym);

WOLFSSL_LOCAL int sp_ecc_mulmod_1024(const mp_int* km, const ecc_point* gm,
    ecc_point* rm, int map, void* heap);
WOLFSSL_LOCAL int sp_ecc_mulmod_base_1024(const mp_int* km, ecc_point* rm,
    int map, void* heap);
WOLFSSL_LOCAL int sp_ecc_mulmod_base_add_1024(const mp_int* km,
    const ecc_point* am, int inMont, ecc_point* rm, int map, void* heap);
WOLFSSL_LOCAL int sp_ecc_gen_table_1024(const ecc_point* gm, byte* table,
    word32* len, void* heap);
WOLFSSL_LOCAL int sp_ecc_mulmod_table_1024(const mp_int* km,
    const ecc_point* gm, byte* table, ecc_point* r, int map, void* heap);
WOLFSSL_LOCAL int sp_ModExp_Fp_star_1024(const mp_int* base, mp_int* exp,
    mp_int* res);
WOLFSSL_LOCAL int sp_Pairing_1024(const ecc_point* p, const ecc_point* q,
    mp_int* res);
WOLFSSL_LOCAL int sp_Pairing_gen_precomp_1024(const ecc_point* p, byte* table,
    word32* len);
WOLFSSL_LOCAL int sp_Pairing_precomp_1024(const ecc_point* p,
    const ecc_point* q, mp_int* res, const byte* table, word32 len);
WOLFSSL_LOCAL int sp_ecc_is_point_1024(const mp_int* pX, const mp_int* pY);
WOLFSSL_LOCAL int sp_ecc_check_key_1024(const mp_int* pX, const mp_int* pY,
    const mp_int* privm, void* heap);

#endif /* HAVE_FIPS_VERSION && HAVE_FIPS_VERSION == 2  && !WOLFSSL_SP_ARM[32|64]_ASM */

#ifdef WOLFSSL_SP_NONBLOCK
WOLFSSL_LOCAL int sp_ecc_make_key_256_nb(sp_ecc_ctx_t* sp_ctx, WC_RNG* rng,
    mp_int* priv, ecc_point* pub, void* heap);
WOLFSSL_LOCAL int sp_ecc_secret_gen_256_nb(sp_ecc_ctx_t* sp_ctx,
    const mp_int* priv, const ecc_point* pub, byte* out, word32* outLen,
    void* heap);
WOLFSSL_LOCAL int sp_ecc_sign_256_nb(sp_ecc_ctx_t* ctx, const byte* hash,
    word32 hashLen, WC_RNG* rng, mp_int* priv, mp_int* rm, mp_int* sm,
    mp_int* km, void* heap);
WOLFSSL_LOCAL int sp_ecc_verify_256_nb(sp_ecc_ctx_t* ctx, const byte* hash,
    word32 hashLen, const mp_int* pX, const mp_int* pY, const mp_int* pZ,
    const mp_int* r, const mp_int* sm, int* res, void* heap);

WOLFSSL_LOCAL int sp_ecc_make_key_384_nb(sp_ecc_ctx_t* sp_ctx, WC_RNG* rng,
    mp_int* priv, ecc_point* pub, void* heap);
WOLFSSL_LOCAL int sp_ecc_secret_gen_384_nb(sp_ecc_ctx_t* sp_ctx,
    const mp_int* priv, const ecc_point* pub, byte* out, word32* outLen,
    void* heap);
WOLFSSL_LOCAL int sp_ecc_sign_384_nb(sp_ecc_ctx_t* ctx, const byte* hash,
    word32 hashLen, WC_RNG* rng, mp_int* priv, mp_int* rm, mp_int* sm,
    mp_int* km, void* heap);
WOLFSSL_LOCAL int sp_ecc_verify_384_nb(sp_ecc_ctx_t* ctx, const byte* hash,
    word32 hashLen, const mp_int* pX, const mp_int* pY, const mp_int* pZ,
    const mp_int* r, const mp_int* sm, int* res, void* heap);

WOLFSSL_LOCAL int sp_ecc_make_key_521_nb(sp_ecc_ctx_t* sp_ctx, WC_RNG* rng,
    mp_int* priv, ecc_point* pub, void* heap);
WOLFSSL_LOCAL int sp_ecc_secret_gen_521_nb(sp_ecc_ctx_t* sp_ctx,
    const mp_int* priv, const ecc_point* pub, byte* out, word32* outLen,
    void* heap);
WOLFSSL_LOCAL int sp_ecc_sign_521_nb(sp_ecc_ctx_t* ctx, const byte* hash,
    word32 hashLen, WC_RNG* rng, mp_int* priv, mp_int* rm, mp_int* sm,
    mp_int* km, void* heap);
WOLFSSL_LOCAL int sp_ecc_verify_521_nb(sp_ecc_ctx_t* ctx, const byte* hash,
    word32 hashLen, const mp_int* pX, const mp_int* pY, const mp_int* pZ,
    const mp_int* r, const mp_int* sm, int* res, void* heap);
#endif /* WOLFSSL_SP_NONBLOCK */

#ifdef HAVE_ECC_SM2

WOLFSSL_LOCAL int sp_ecc_mulmod_sm2_256(mp_int* km, ecc_point* gm,
    ecc_point* rm, int map, void* heap);
WOLFSSL_LOCAL int sp_ecc_mulmod_base_sm2_256(mp_int* km, ecc_point* rm, int map,
    void* heap);

WOLFSSL_LOCAL int sp_ecc_make_key_sm2_256(WC_RNG* rng, mp_int* priv,
    ecc_point* pub, void* heap);
WOLFSSL_LOCAL int sp_ecc_secret_gen_sm2_256(mp_int* priv, ecc_point* pub,
    byte* out, word32* outlen, void* heap);

WOLFSSL_LOCAL int sp_ecc_sign_sm2_256(const byte* hash, word32 hashLen,
    WC_RNG* rng, mp_int* priv, mp_int* rm, mp_int* sm, mp_int* km, void* heap);
WOLFSSL_LOCAL int sp_ecc_verify_sm2_256(const byte* hash, word32 hashLen,
    mp_int* pX, mp_int* pY, mp_int* pZ, mp_int* r, mp_int* sm, int* res,
    void* heap);

WOLFSSL_LOCAL int sp_ecc_is_point_sm2_256(mp_int* pX, mp_int* pY);
WOLFSSL_LOCAL int sp_ecc_check_key_sm2_256(mp_int* pX, mp_int* pY,
    mp_int* privm, void* heap);

WOLFSSL_LOCAL int sp_ecc_proj_add_point_sm2_256(mp_int* pX, mp_int* pY,
    mp_int* pZ, mp_int* qX, mp_int* qY, mp_int* qZ, mp_int* rX, mp_int* rY,
    mp_int* rZ);
WOLFSSL_LOCAL int sp_ecc_proj_dbl_point_sm2_256(mp_int* pX, mp_int* pY,
    mp_int* pZ, mp_int* rX, mp_int* rY, mp_int* rZ);
WOLFSSL_LOCAL int sp_ecc_map_sm2_256(mp_int* pX, mp_int* pY, mp_int* pZ);
WOLFSSL_LOCAL int sp_ecc_uncompress_sm2_256(mp_int* xm, int odd, mp_int* ym);

#endif


#endif /* WOLFSSL_HAVE_SP_ECC */


#ifdef __cplusplus
    } /* extern "C" */
#endif

#endif /* WOLFSSL_HAVE_SP_RSA || WOLFSSL_HAVE_SP_DH || WOLFSSL_HAVE_SP_ECC */

#endif /* WOLF_CRYPT_SP_H */

