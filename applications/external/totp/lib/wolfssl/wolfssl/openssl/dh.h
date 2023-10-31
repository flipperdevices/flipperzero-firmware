/* dh.h
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

/* dh.h for openSSL */


#ifndef WOLFSSL_DH_H_
#define WOLFSSL_DH_H_

#include <wolfssl/openssl/bn.h>
#include <wolfssl/openssl/opensslv.h>

#ifdef __cplusplus
    extern "C" {
#endif

#ifndef WOLFSSL_DH_TYPE_DEFINED /* guard on redeclaration */
    typedef struct WOLFSSL_DH       WOLFSSL_DH;
    #define WOLFSSL_DH_TYPE_DEFINED
#endif

struct WOLFSSL_DH {
    WOLFSSL_BIGNUM* p;
    WOLFSSL_BIGNUM* g;
    WOLFSSL_BIGNUM* q;
    WOLFSSL_BIGNUM* pub_key;     /* openssh deference g^x */
    WOLFSSL_BIGNUM* priv_key;    /* openssh deference x   */
    void*          internal;     /* our DH */
    char           inSet;        /* internal set from external ? */
    char           exSet;        /* external set from internal ? */
    /*added for lighttpd openssl compatibility, go back and add a getter in
     * lighttpd src code.
     */
    int length;
    wolfSSL_Ref ref;             /* Reference count information. */
};

WOLFSSL_API WOLFSSL_DH *wolfSSL_d2i_DHparams(WOLFSSL_DH **dh,
                                         const unsigned char **pp, long length);
WOLFSSL_API int wolfSSL_i2d_DHparams(const WOLFSSL_DH *dh, unsigned char **out);
WOLFSSL_API WOLFSSL_DH* wolfSSL_DH_new(void);
WOLFSSL_API WOLFSSL_DH* wolfSSL_DH_new_by_nid(int nid);
WOLFSSL_API void        wolfSSL_DH_free(WOLFSSL_DH* dh);
WOLFSSL_API WOLFSSL_DH* wolfSSL_DH_dup(WOLFSSL_DH* dh);
WOLFSSL_API int         wolfSSL_DH_up_ref(WOLFSSL_DH* dh);

WOLFSSL_API int wolfSSL_DH_check(const WOLFSSL_DH *dh, int *codes);
WOLFSSL_API int wolfSSL_DH_size(WOLFSSL_DH* dh);
WOLFSSL_API int wolfSSL_DH_generate_key(WOLFSSL_DH* dh);
WOLFSSL_API int wolfSSL_DH_compute_key(unsigned char* key, const WOLFSSL_BIGNUM* pub,
                                     WOLFSSL_DH* dh);
WOLFSSL_API int wolfSSL_DH_LoadDer(WOLFSSL_DH* dh, const unsigned char* derBuf,
                                   int derSz);
WOLFSSL_API int wolfSSL_DH_set_length(WOLFSSL_DH* dh, long len);
WOLFSSL_API int wolfSSL_DH_set0_pqg(WOLFSSL_DH *dh, WOLFSSL_BIGNUM *p,
    WOLFSSL_BIGNUM *q, WOLFSSL_BIGNUM *g);

WOLFSSL_API WOLFSSL_DH* wolfSSL_DH_get_2048_256(void);

#if defined(OPENSSL_EXTRA) || defined(OPENSSL_EXTRA_X509_SMALL)

typedef WOLFSSL_DH                   DH;

#define DH_new        wolfSSL_DH_new
#define DH_free       wolfSSL_DH_free
#define DH_up_ref     wolfSSL_DH_up_ref
#define DH_new_by_nid wolfSSL_DH_new_by_nid

#define d2i_DHparams    wolfSSL_d2i_DHparams
#define i2d_DHparams    wolfSSL_i2d_DHparams
#define DH_check        wolfSSL_DH_check

#define DH_size         wolfSSL_DH_size
#define DH_generate_key wolfSSL_DH_generate_key
#define DH_compute_key  wolfSSL_DH_compute_key
#define DH_set_length   wolfSSL_DH_set_length
#define DH_set0_pqg     wolfSSL_DH_set0_pqg
#define DH_get0_pqg     wolfSSL_DH_get0_pqg
#define DH_get0_key     wolfSSL_DH_get0_key
#define DH_set0_key     wolfSSL_DH_set0_key
#define DH_bits(x)      (BN_num_bits((x)->p))

#define DH_GENERATOR_2                  2
#define DH_CHECK_P_NOT_PRIME            0x01
#define DH_CHECK_P_NOT_SAFE_PRIME       0x02
#define DH_NOT_SUITABLE_GENERATOR       0x08

/* Temporary values for wolfSSL_DH_Check*/
#define DH_CHECK_INVALID_Q_VALUE        0x10
#define DH_CHECK_Q_NOT_PRIME            0x11
/* end temp */

/* for pre 1.1.0 */
#define get_rfc2409_prime_768      wolfSSL_DH_768_prime
#define get_rfc2409_prime_1024     wolfSSL_DH_1024_prime
#define get_rfc3526_prime_1536     wolfSSL_DH_1536_prime
#define get_rfc3526_prime_2048     wolfSSL_DH_2048_prime
#define get_rfc3526_prime_3072     wolfSSL_DH_3072_prime
#define get_rfc3526_prime_4096     wolfSSL_DH_4096_prime
#define get_rfc3526_prime_6144     wolfSSL_DH_6144_prime
#define get_rfc3526_prime_8192     wolfSSL_DH_8192_prime

#define DH_get_2048_256 wolfSSL_DH_get_2048_256

#if defined(OPENSSL_ALL) || defined(HAVE_STUNNEL)
#define DH_generate_parameters    wolfSSL_DH_generate_parameters
#define DH_generate_parameters_ex wolfSSL_DH_generate_parameters_ex
#endif /* OPENSSL_ALL || HAVE_STUNNEL */

#define DH_GENERATOR_2 2
#define DH_GENERATOR_5 5

#endif /* OPENSSL_EXTRA || OPENSSL_EXTRA_X509_SMALL */

#ifdef __cplusplus
    }  /* extern "C" */
#endif

#endif /* WOLFSSL_DH_H_ */
