/* ssl_bn.c
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

#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <wolfssl/wolfcrypt/settings.h>

 #include <wolfssl/internal.h>
#ifndef WC_NO_RNG
    #include <wolfssl/wolfcrypt/random.h>
#endif

#if !defined(WOLFSSL_SSL_BN_INCLUDED)
    #ifndef WOLFSSL_IGNORE_FILE_WARN
        #warning ssl_bn.c does not need to be compiled separately from ssl.c
    #endif
#else

/* Check on validity of big number.
 *
 * Used for parameter validation.
 *
 * @param [in] bn  Big number.
 * @return 1 when bn is not NULL and internal representation is not NULL.
 * @return 0 otherwise.
 */
#define BN_IS_NULL(bn) (((bn) == NULL) || ((bn)->internal == NULL))

/*******************************************************************************
 * Constructor/Destructor/Initializer APIs
 ******************************************************************************/

#if defined(OPENSSL_EXTRA) && !defined(NO_ASN)
/* Set big number to be negative.
 *
 * @param [in, out] bn   Big number to make negative.
 * @param [in]      neg  Whether number is negative.
 * @return  1 on success.
 * @return  -1 when bn or internal representation of bn is NULL.
 */
static int wolfssl_bn_set_neg(WOLFSSL_BIGNUM* bn, int neg)
{
    int ret = 1;

    if (BN_IS_NULL(bn)) {
        WOLFSSL_MSG("bn NULL error");
        ret = -1;
    }
#if !defined(WOLFSSL_SP_MATH_ALL) || defined(WOLFSSL_SP_INT_NEGATIVE)
    else if (neg) {
        mp_setneg((mp_int*)bn->internal);
    }
    else {
        ((mp_int*)bn->internal)->sign = MP_ZPOS;
    }
#endif

    return ret;
}
#endif /* OPENSSL_EXTRA && !NO_ASN */

#if defined(OPENSSL_EXTRA) || defined(OPENSSL_EXTRA_X509_SMALL)
/* Get the internal representation value into an MP integer.
 *
 * When calling wolfssl_bn_get_value, mpi should be cleared by caller if no
 * longer used. ie mp_free(mpi). This is to free data when fastmath is
 * disabled since a copy of mpi is made by this function and placed into bn.
 *
 * @param [in]      bn   Big number to copy value from.
 * @param [in, out] mpi  MP integer to copy into.
 * @return  1 on success.
 * @return  -1 when bn or internal representation of bn is NULL.
 * @return  -1 when mpi is NULL.
 * @return  -1 when copy fails.
 */
int wolfssl_bn_get_value(WOLFSSL_BIGNUM* bn, mp_int* mpi)
{
    int ret = 1;

    WOLFSSL_MSG("Entering wolfssl_bn_get_value_mp");

    /* Validate parameters. */
    if (BN_IS_NULL(bn)) {
        WOLFSSL_MSG("bn NULL error");
        ret = -1;
    }
    else if (mpi == NULL) {
        WOLFSSL_MSG("mpi NULL error");
        ret = -1;
    }

    /* Copy the internal representation into MP integer. */
    if ((ret == 1) && mp_copy((mp_int*)bn->internal, mpi) != MP_OKAY) {
        WOLFSSL_MSG("mp_copy error");
        ret = -1;
    }

    return ret;
}

/* Set big number internal representation to value in mpi.
 *
 * Will create a new big number if bn points to NULL.
 *
 * When calling wolfssl_bn_set_value, mpi should be cleared by caller if no
 * longer used. ie mp_free(mpi). This is to free data when fastmath is
 * disabled since a copy of mpi is made by this function and placed into bn.
 *
 * @param [in, out] bn   Pointer to big number to have value.
 * @param [in]      mpi  MP integer with value to set.
 * @return  1 on success.
 * @return  -1 when mpi or bn is NULL.
 * @return  -1 when creating a new big number fails.
 * @return  -1 when copying MP integer fails.
 */
int wolfssl_bn_set_value(WOLFSSL_BIGNUM** bn, mp_int* mpi)
{
    int ret = 1;
    WOLFSSL_BIGNUM* a = NULL;

#ifdef WOLFSSL_DEBUG_OPENSSL
    WOLFSSL_ENTER("wolfssl_bn_set_value");
#endif

    /* Validate parameters. */
    if ((bn == NULL) || (mpi == NULL)) {
        WOLFSSL_MSG("mpi or bn NULL error");
        ret = -1;
    }

    /* Allocate a new big number if one not passed in. */
    if ((ret == 1) && (*bn == NULL)) {
        a = wolfSSL_BN_new();
        if (a == NULL) {
            WOLFSSL_MSG("wolfssl_bn_set_value alloc failed");
            ret = -1;
        }
        *bn = a;
    }

    /* Copy MP integer value into internal representation of big number. */
    if ((ret == 1) && (mp_copy(mpi, (mp_int*)((*bn)->internal)) != MP_OKAY)) {
        WOLFSSL_MSG("mp_copy error");
        ret = -1;
    }

    /* Dispose of any allocated big number on error. */
    if ((ret == -1) && (a != NULL)) {
        BN_free(a);
        *bn = NULL;
    }
    return ret;
}

/* Initialize a big number.
 *
 * Assumes bn is not NULL.
 *
 * @param [in, out] bn  Big number to initialize.
 */
static void wolfssl_bn_init(WOLFSSL_BIGNUM* bn)
{
    /* Clear fields of big number. */
    XMEMSET(bn, 0, sizeof(WOLFSSL_BIGNUM));
    /* Initialization only fails when passed NULL. */
    (void)mp_init(&bn->mpi);
    /* Set an internal representation. */
    bn->internal = &bn->mpi;
}

/* Create a new big number.
 *
 * @return  An allocated and initialized big number on success.
 * @return  NULL on failure.
 */
WOLFSSL_BIGNUM* wolfSSL_BN_new(void)
{
    WOLFSSL_BIGNUM* bn = NULL;

#ifdef WOLFSSL_DEBUG_OPENSSL
    WOLFSSL_ENTER("wolfSSL_BN_new");
#endif

    /* Allocate memory for big number. */
    bn = (WOLFSSL_BIGNUM*)XMALLOC(sizeof(WOLFSSL_BIGNUM), NULL,
        DYNAMIC_TYPE_BIGINT);
    if (bn == NULL) {
        WOLFSSL_MSG("wolfSSL_BN_new malloc WOLFSSL_BIGNUM failure");
    }
    else {
        /* Initialize newly allocated object. */
        wolfssl_bn_init(bn);
    }

    return bn;
}

#if !defined(USE_INTEGER_HEAP_MATH) && !defined(HAVE_WOLF_BIGINT)
/* Initialize a big number.
 *
 * Call this instead of wolfSSL_BN_new() and wolfSSL_BN_free().
 *
 * Do not call this API after wolfSSL_BN_new() or wolfSSL_BN_init().
 *
 * @param [in, out] bn  Big number to initialize.
 */
void wolfSSL_BN_init(WOLFSSL_BIGNUM* bn)
{
#ifdef WOLFSSL_DEBUG_OPENSSL
    WOLFSSL_ENTER("wolfSSL_BN_init");
#endif

    /* Validate parameter. */
    if (bn != NULL) {
        /* Initialize big number object. */
        wolfssl_bn_init(bn);
    }
}
#endif

/* Dispose of big number.
 *
 * bn is unusable after this call.
 *
 * @param [in, out] bn  Big number to free.
 */
void wolfSSL_BN_free(WOLFSSL_BIGNUM* bn)
{
#ifdef WOLFSSL_DEBUG_OPENSSL
    WOLFSSL_ENTER("wolfSSL_BN_free");
#endif

    /* Validate parameter. */
    if (bn != NULL) {
        /* Cleanup any internal representation. */
        if (bn->internal != NULL) {
            /* Free MP integer. */
            mp_free(&bn->mpi);
        }
        /* Dispose of big number object. */
        XFREE(bn, NULL, DYNAMIC_TYPE_BIGINT);
        /* bn = NULL, don't try to access or double free it */
    }
}

/* Zeroize and dispose of big number.
 *
 * bn is unusable after this call.
 *
 * @param [in, out] bn  Big number to clear and free.
 */
void wolfSSL_BN_clear_free(WOLFSSL_BIGNUM* bn)
{
#ifdef WOLFSSL_DEBUG_OPENSSL
    WOLFSSL_ENTER("wolfSSL_BN_clear_free");
#endif

    /* Validate parameter. */
    if (bn != NULL) {
        /* Check for internal representation. */
        if (bn->internal != NULL) {
            /* Zeroize MP integer. */
            mp_forcezero((mp_int*)bn->internal);
        }
        /* Dispose of big number. */
        wolfSSL_BN_free(bn);
    }
}

/* Zeroize big number.
 *
 * @param [in, out] bn  Big number to clear.
 */
void wolfSSL_BN_clear(WOLFSSL_BIGNUM* bn)
{
#ifdef WOLFSSL_DEBUG_OPENSSL
    WOLFSSL_ENTER("wolfSSL_BN_clear");
#endif

    /* Validate parameter. */
    if (!BN_IS_NULL(bn)) {
        /* Zeroize MP integer. */
        mp_forcezero((mp_int*)bn->internal);
    }
}
#endif /* OPENSSL_EXTRA || OPENSSL_EXTRA_X509_SMALL */

#ifdef OPENSSL_EXTRA

static WOLFSSL_BIGNUM* bn_one = NULL;

/* Return a big number with the value of one.
 *
 * @return  A big number with the value one on success.
 * @return  NULL on failure.
 */
const WOLFSSL_BIGNUM* wolfSSL_BN_value_one(void)
{
    WOLFSSL_BIGNUM* one;

    WOLFSSL_ENTER("wolfSSL_BN_value_one");

    /* Get the global object. */
    one = bn_one;
    /* Create a new big number if global not set. */
    if ((one == NULL) && ((one = wolfSSL_BN_new()) != NULL)) {
        /* Set internal representation to have a value of 1. */
        if (mp_set_int((mp_int*)one->internal, 1) != MP_OKAY) {
            /* Dispose of big number on error. */
            wolfSSL_BN_free(one);
            one = NULL;
        }
        else
    #ifndef SINGLE_THREADED
        /* Ensure global has not been set by another thread. */
        if (bn_one == NULL)
    #endif
        {
            /* Set this big number as the global. */
            bn_one = one;
        }
    #ifndef SINGLE_THREADED
        /* Check if another thread has set the global. */
        if (bn_one != one) {
            /* Dispose of this big number and return the global.  */
            wolfSSL_BN_free(one);
            one = bn_one;
        }
    #endif
    }

    return one;
}

static void wolfSSL_BN_free_one(void) {
    wolfSSL_BN_free(bn_one);
    bn_one = NULL;
}

/* Create a new big number with the same value as the one passed in.
 *
 * @param [in] bn  Big number to duplicate.
 * @return  Big number on success.
 * @return  NULL when bn or internal representation of bn is NULL.
 * @return  NULL when creating a new big number fails.
 * @return  NULL when copying the internal representation fails.
 */
WOLFSSL_BIGNUM* wolfSSL_BN_dup(const WOLFSSL_BIGNUM* bn)
{
    int err = 0;
    WOLFSSL_BIGNUM* ret = NULL;

    WOLFSSL_ENTER("wolfSSL_BN_dup");

    /* Validate parameter. */
    if (BN_IS_NULL(bn)) {
        WOLFSSL_MSG("bn NULL error");
        err = 1;
    }

    /* Create a new big number to return. */
    if ((!err) && ((ret = wolfSSL_BN_new()) == NULL)) {
        WOLFSSL_MSG("bn new error");
        err = 1;
    }

    if (!err) {
        err = (wolfSSL_BN_copy(ret, bn) == NULL);
    }

    if (err) {
         /* Dispose of dynamically allocated data. */
         wolfSSL_BN_free(ret);
         ret = NULL;
    }
    return ret;
}

/* Copy value from bn into another r.
 *
 * @param [in, out] r   Big number to copy into.
 * @param [in]      bn  Big number to copy from.
 * @return  Big number copied into on success.
 * @return  NULL when r or bn is NULL.
 * @return  NULL when copying fails.
 */
WOLFSSL_BIGNUM* wolfSSL_BN_copy(WOLFSSL_BIGNUM* r, const WOLFSSL_BIGNUM* bn)
{
    WOLFSSL_ENTER("wolfSSL_BN_copy");

    /* Validate parameters. */
    if (BN_IS_NULL(r) || BN_IS_NULL(bn)) {
        WOLFSSL_MSG("r or bn NULL error");
        r = NULL;
    }

    /* Copy the value in. */
    if ((r != NULL) && mp_copy((mp_int*)bn->internal, (mp_int*)r->internal) !=
            MP_OKAY) {
        WOLFSSL_MSG("mp_copy error");
        r = NULL;
    }

    if (r != NULL) {
        /* Copy other fields in a big number. */
        r->neg = bn->neg;
    }

    return r;
}


/*******************************************************************************
 * Encode/Decode APIs.
 ******************************************************************************/

/* Encode the number is a big-endian byte array.
 *
 * Assumes byte array is large enough to hold encoding when not NULL.
 * Use NULL for byte array to get length.
 *
 * Return compliant with OpenSSL.
 *
 * @param [in]  bn  Big number to reduced
 * @param [out] r   Byte array to encode into. May be NULL.
 * @return  Length of big number in bytes on success.
 * @return  -1 when bn is NULL or encoding fails.
 */
int wolfSSL_BN_bn2bin(const WOLFSSL_BIGNUM* bn, unsigned char* r)
{
    int ret;

    WOLFSSL_ENTER("wolfSSL_BN_bn2bin");

    /* Validate parameters. */
    if (BN_IS_NULL(bn)) {
        WOLFSSL_MSG("NULL bn error");
        ret = -1;
    }
    else {
        /* Get the length of the encoding. */
        ret = mp_unsigned_bin_size((mp_int*)bn->internal);
        /* Encode if byte array supplied. */
        if ((r != NULL) && (mp_to_unsigned_bin((mp_int*)bn->internal, r) !=
                MP_OKAY)) {
            WOLFSSL_MSG("mp_to_unsigned_bin error");
            ret = -1;
        }
    }

    return ret;
}


/* Return a big number with value of the decoding of the big-endian byte array.
 *
 * Returns ret when not NULL.
 * Allocates a big number when ret is NULL.
 * Assumes str is not NULL.
 *
 * @param [in]      data  Byte array to decode.
 * @param [in]      len   Number of bytes in byte array.
 * @param [in, out] ret   Big number to reduced. May be NULL.
 * @return  A big number on success.
 * @return  NULL on failure.
 */
WOLFSSL_BIGNUM* wolfSSL_BN_bin2bn(const unsigned char* data, int len,
    WOLFSSL_BIGNUM* ret)
{
    WOLFSSL_BIGNUM* bn = NULL;

    WOLFSSL_ENTER("wolfSSL_BN_bin2bn");

    /* Validate parameters. */
    if ((data == NULL) || (len < 0)) {
        ret = NULL;
    }
    /* Allocate a new big number when ret is NULL. */
    else if (ret == NULL) {
        ret = wolfSSL_BN_new();
        bn = ret;
    }

    /* Check ret is usable. */
    if (ret != NULL) {
        /* Check internal representation is usable. */
        if (ret->internal == NULL) {
            ret = NULL;
        }
        else {
            /* Decode into big number. */
            if (mp_read_unsigned_bin((mp_int*)ret->internal, data, (word32)len)
                    != 0) {
                WOLFSSL_MSG("mp_read_unsigned_bin failure");
                /* Don't return anything on failure. bn will be freed if set. */
                ret = NULL;
            }
            else {
                /* Don't free bn as we may be returning it. */
                bn = NULL;
            }
        }
    }

    /* Dispose of allocated BN not being returned. */
    wolfSSL_BN_free(bn);

    return ret;
}

/* Encode the big number value into a string, of the radix, that is allocated.
 *
 * @param [in] bn     Big number to encode.
 * @param [in] radix  Radix to encode to.
 * @return  String with encoding on success.
 * @return  NULL when bn or internal representation of bn is NULL.
 * @return  NULL on failure.
 */
static char* wolfssl_bn_bn2radix(const WOLFSSL_BIGNUM* bn, int radix)
{
    int err = 0;
    int len = 0;
    char* str = NULL;


    /* Validate parameter. */
    if (BN_IS_NULL(bn)) {
        WOLFSSL_MSG("bn NULL error");
        err = 1;
    }

    /* Determine length of encoding. */
    if ((!err) && (mp_radix_size((mp_int*)bn->internal, radix, &len) !=
            MP_OKAY)) {
        WOLFSSL_MSG("mp_radix_size failure");
        err = 1;
    }

    if (!err) {
        /* Allocate string. */
        str = (char*)XMALLOC((size_t)len, NULL, DYNAMIC_TYPE_OPENSSL);
        if (str == NULL) {
            WOLFSSL_MSG("BN_bn2hex malloc string failure");
            err = 1;
        }
    }

    /* Encode into string using wolfCrypt. */
    if ((!err) && (mp_toradix((mp_int*)bn->internal, str, radix) != MP_OKAY)) {
        err = 1;
    }

    if (err) {
        /* Dispose of dynamically allocated data. */
        XFREE(str, NULL, DYNAMIC_TYPE_OPENSSL);
        /* Don't return freed string. */
        str = NULL;
    }
    return str;
}

/* Encode the big number value into hex string that is allocated.
 *
 * @param [in] bn  Big number to encode.
 * @return  String with encoding on success.
 * @return  NULL when bn or internal representation of bn is NULL.
 * @return  NULL on failure.
 */
char* wolfSSL_BN_bn2hex(const WOLFSSL_BIGNUM *bn)
{
    WOLFSSL_ENTER("wolfSSL_BN_bn2hex");
    return wolfssl_bn_bn2radix(bn, MP_RADIX_HEX);
}

/* Decode string of a radix into a big number.
 *
 * If bn is a pointer to NULL, then a new big number is allocated and assigned.
 *
 * Note on use: this function expects str to be an even length. It is
 * converting pairs of bytes into 8-bit values. As an example, the RSA
 * public exponent is commonly 0x010001. To get it to convert, you need
 * to pass in the string "010001", it will fail if you use "10001". This
 * is an affect of how Base16_Decode() works.
 *
 * @param [in, out] bn     Pointer to a big number. May point to NULL.
 * @param [in]      str    Hex string to decode.
 * @param [in]      radix  Radix to decode from.
 * @return  1 on success.
 * @return  0 when bn or str is NULL or str is zero length.
 * @return  0 when creating a new big number fails.
 * @return  0 when decoding fails.
 */
static int wolfssl_bn_radix2bn(WOLFSSL_BIGNUM** bn, const char* str, int radix)
{
    int ret = 1;
    WOLFSSL_BIGNUM* a = NULL;

    /* Validate parameters. */
    if ((bn == NULL) || (str == NULL) || (str[0] == '\0')) {
        WOLFSSL_MSG("Bad function argument");
        ret = 0;
    }
    /* Check if we have a big number to decode into. */
    if ((ret == 1) && (*bn == NULL)) {
        /* Allocate a new big number. */
        a = wolfSSL_BN_new();
        if (a == NULL) {
            WOLFSSL_MSG("BN new failed");
            ret = 0;
        }
        /* Return allocated big number. */
        *bn = a;
    }
    /* Decode hex string into internal representation. */
    if ((ret == 1) && (mp_read_radix((mp_int*)(*bn)->internal, str, radix) !=
            MP_OKAY)) {
        WOLFSSL_MSG("Bad read_radix error");
        ret = 0;
    }

    if ((ret == 0) && (a != NULL)) {
        /* Dispose of big number. */
        wolfSSL_BN_free(a);
        /* Don't return freed big number. */
        *bn = NULL;
    }
    return ret;
}

/* Decode hex string into a big number.
 *
 * If bn is a pointer to NULL, then a new big number is allocated and assigned.
 *
 * Note on use: this function expects str to be an even length. It is
 * converting pairs of bytes into 8-bit values. As an example, the RSA
 * public exponent is commonly 0x010001. To get it to convert, you need
 * to pass in the string "010001", it will fail if you use "10001". This
 * is an affect of how Base16_Decode() works.
 *
 * @param [in, out] bn   Pointer to a big number. May point to NULL.
 * @param [in]      str  Hex string to decode.
 * @return  1 on success.
 * @return  0 when bn or str is NULL or str is zero length.
 * @return  0 when creating a new big number fails.
 * @return  0 when decoding fails.
 */
int wolfSSL_BN_hex2bn(WOLFSSL_BIGNUM** bn, const char* str)
{
    WOLFSSL_ENTER("wolfSSL_BN_hex2bn");
    return wolfssl_bn_radix2bn(bn, str, MP_RADIX_HEX);
}

#if defined(WOLFSSL_KEY_GEN) || defined(HAVE_COMP_KEY)
/* Encode big number into decimal string.
 *
 * @param [in] bn  Big number to encode.
 * @return  String with encoding on success.
 * @return  NULL when bn or internal representation of bn is NULL.
 * @return  NULL on failure.
 */
char* wolfSSL_BN_bn2dec(const WOLFSSL_BIGNUM *bn)
{
    WOLFSSL_ENTER("wolfSSL_BN_bn2hex");
    return wolfssl_bn_bn2radix(bn, MP_RADIX_DEC);
}
#else
/* Encode big number into decimal string.
 *
 * @param [in] bn  Big number to encode.
 * @return  NULL as implementation not available.
 */
char* wolfSSL_BN_bn2dec(const WOLFSSL_BIGNUM* bn)
{
    (void)bn;
    WOLFSSL_ENTER("wolfSSL_BN_bn2dec");
    return NULL;
}
#endif /* defined(WOLFSSL_KEY_GEN) || defined(HAVE_COMP_KEY) */


#ifndef NO_RSA
/* Decode hex string into a big number.
 *
 * If bn is a pointer to NULL, then a new big number is allocated and assigned.
 *
 * Note on use: this function expects str to be an even length. It is
 * converting pairs of bytes into 8-bit values. As an example, the RSA
 * public exponent is commonly 0x010001. To get it to convert, you need
 * to pass in the string "010001", it will fail if you use "10001". This
 * is an affect of how Base16_Decode() works.
 *
 * @param [in, out] bn   Pointer to a big number. May point to NULL.
 * @param [in]      str  Hex string to decode.
 * @return  1 on success.
 * @return  0 when bn or str is NULL or str is zero length.
 * @return  0 when creating a new big number fails.
 * @return  0 when decoding fails.
 */
int wolfSSL_BN_dec2bn(WOLFSSL_BIGNUM** bn, const char* str)
{
    WOLFSSL_ENTER("wolfSSL_BN_bn2dec");
    return wolfssl_bn_radix2bn(bn, str, MP_RADIX_DEC);
}
#else
/* Decode hex string into a big number.
 *
 * @param [in, out] bn   Pointer to a big number. May point to NULL.
 * @param [in]      str  Hex string to decode.
 * @return  0 as implementation not available..
 */
int wolfSSL_BN_dec2bn(WOLFSSL_BIGNUM** bn, const char* str)
{
    (void)bn;
    (void)str;
    WOLFSSL_ENTER("wolfSSL_BN_bn2dec");
    return 0;
}
#endif

/*******************************************************************************
 * Get/Set APIs
 ******************************************************************************/

/* Calculate the number of bytes need to represent big number.
 *
 * Return compliant with OpenSSL.
 *
 * @param [in] bn  Big number to use.
 * @return  Size of BIGNUM in bytes on success.
 * @return  0 when bn or internal representation of bn is NULL.
 */
int wolfSSL_BN_num_bytes(const WOLFSSL_BIGNUM* bn)
{
    int ret;

    WOLFSSL_ENTER("wolfSSL_BN_num_bytes");

    /* Validate parameter. */
    if (BN_IS_NULL(bn)) {
        ret = 0;
    }
    else {
        /* Get size from wolfCrypt. */
        ret = mp_unsigned_bin_size((mp_int*)bn->internal);
    }

    return ret;
}

/* Calculate the number of bits need to represent big number.
 *
 * Return compliant with OpenSSL.
 *
 * @param [in] bn  Big number to use.
 * @return  Size of BIGNUM in bits on success.
 * @return  0 when bn or internal representation of bn is NULL.
 */
int wolfSSL_BN_num_bits(const WOLFSSL_BIGNUM* bn)
{
    int ret;

    WOLFSSL_ENTER("wolfSSL_BN_num_bits");

    /* Validate parameter. */
    if (BN_IS_NULL(bn)) {
        ret = 0;
    }
    else {
        /* Get size from wolfCrypt. */
        ret = mp_count_bits((mp_int*)bn->internal);
    }

    return ret;
}

/* Indicates whether a big number is negative.
 *
 * @param [in] bn  Big number to use.
 * @return  1 when number is negative.
 * @return  0 when number is positive.
 * @return  0 when bn is NULL.
 */
int wolfSSL_BN_is_negative(const WOLFSSL_BIGNUM* bn)
{
    int ret;

    /* Validate parameter. */
    if (BN_IS_NULL(bn)) {
        ret = 0;
    }
    else {
        /* Check sign with wolfCrypt. */
        ret = mp_isneg((mp_int*)bn->internal);
    }

    return ret;
}

/* Indicates whether a big number is odd.
 *
 * Return compliant with OpenSSL.
 *
 * @param [in] bn  Big number to use.
 * @return  1 when number is odd.
 * @return  0 when number is even.
 * @return  0 when bn or internal representation of bn is NULL.
 */
int wolfSSL_BN_is_odd(const WOLFSSL_BIGNUM* bn)
{
    int ret;

    WOLFSSL_ENTER("wolfSSL_BN_is_odd");

    /* Validate parameter. */
    if (BN_IS_NULL(bn)) {
        ret = 0;
    }
    else {
        /* wolfCrypt checks whether value is odd. */
        ret = (mp_isodd((mp_int*)bn->internal) == MP_YES);
    }

    return ret;
}

#ifndef NO_WOLFSSL_STUB
/* Mask the lowest n bits.
 *
 * TODO: mp_mod_2d()
 *
 * Return compliant with OpenSSL.
 *
 * @param [in, out] bn  Big number to operation on.
 * @param [in]      n   Number of bits.
 * @return  0 on failure.
 */
int wolfSSL_mask_bits(WOLFSSL_BIGNUM* bn, int n)
{
    (void)bn;
    (void)n;
    WOLFSSL_ENTER("wolfSSL_BN_mask_bits");
    WOLFSSL_STUB("BN_mask_bits");
    return 0;
}
#endif

/* Set a bit of the value in a big number.
 *
 * Return code compliant with OpenSSL.
 *
 * @param [in, out] bn  Big number to modify.
 * @return  1 on success.
 * @return  0 when bn or internal representation of bn is NULL.
 * @return  0 when failed to set bit.
 */
int wolfSSL_BN_set_bit(WOLFSSL_BIGNUM* bn, int n)
{
    int ret = 1;

    if (BN_IS_NULL(bn)) {
        WOLFSSL_MSG("bn NULL error");
        ret = 0;
    }
    else if (mp_set_bit((mp_int*)bn->internal, n) != MP_OKAY) {
        WOLFSSL_MSG("mp_set_bit error");
        ret = 0;
    }

    return ret;
}

/* Clear a bit of the value in a big number.
 *
 * Return code compliant with OpenSSL.
 *
 * @param [in] bn  Big number to check.
 * @param [in] n   Inidex of bit to check.
 * @return  1 on success.
 * @return  0 when bn or internal representation of bn is NULL.
 * @return  0 when failed to clear bit.
 */
int wolfSSL_BN_clear_bit(WOLFSSL_BIGNUM* bn, int n)
{
    int ret = 1;
#ifndef WOLFSSL_SMALL_STACK
    mp_int tmp[1];
#else
    mp_int* tmp = NULL;
#endif

    /* Validate parameters. */
    if (BN_IS_NULL(bn) || (n < 0)) {
        WOLFSSL_MSG("bn NULL error");
        ret = 0;
    }
    /* Check if bit is set to clear. */
    if ((ret == 1) && (mp_is_bit_set((mp_int*)bn->internal, n))) {
    #ifdef WOLFSSL_SMALL_STACK
        /* Allocate a new MP integer to hold bit to clear. */
        tmp = (mp_int*)XMALLOC(sizeof(mp_int), NULL, DYNAMIC_TYPE_BIGINT);
        if (tmp == NULL) {
            ret = 0;
        }
    #endif
        if (ret == 1) {
            /* Reset new MP integer. */
            XMEMSET(tmp, 0, sizeof(mp_int));
            if (mp_init(tmp) != MP_OKAY) {
                ret = 0;
            }
        }
        /* Set the bit to clear into temporary MP integer. */
        if ((ret == 1) && (mp_set_bit(tmp, n) != MP_OKAY)) {
            ret = 0;
        }
        /* Clear bit by sutraction. */
        if ((ret == 1) && (mp_sub((mp_int*)bn->internal, tmp,
                (mp_int*)bn->internal) != MP_OKAY)) {
            ret = 0;
        }

        /* Free any dynamic memory in MP integer. */
        mp_clear(tmp);
    #ifdef WOLFSSL_SMALL_STACK
        /* Dispose of temporary MP integer. */
        XFREE(tmp, NULL, DYNAMIC_TYPE_BIGINT);
    #endif
    }

    return ret;
}

/* Returns whether the bit is set in the value of the big number.
 *
 * When bn is NULL, returns 0.
 *
 * Return code compliant with OpenSSL.
 *
 * @param [in] bn  Big number to check.
 * @param [in] n   Inidex of bit to check.
 * @return  1 if bit set.
 * @return  0 otherwise.
 */
int wolfSSL_BN_is_bit_set(const WOLFSSL_BIGNUM* bn, int n)
{
    int ret;

    /* Check for big number value. */
    if (BN_IS_NULL(bn) || (n < 0)) {
        WOLFSSL_MSG("bn NULL error");
        ret = 0;
    }
    else {
        /* Set bit with wolfCrypt. */
        ret = mp_is_bit_set((mp_int*)bn->internal, (mp_digit)n);
    }

    return ret;
}

/* Set the big number to the value 0.
 *
 * @param [in, out] bn  Big number to use.
 */
void wolfSSL_BN_zero(WOLFSSL_BIGNUM* bn)
{
    /* Validate parameter. */
    if (!BN_IS_NULL(bn)) {
        /* Set wolfCrypt representation to 0. */
        mp_zero((mp_int*)bn->internal);
    }
}

/* Set the big number to the value 0.
 *
 * @param [in, out] bn  Big number to use.
 * @return  1 on success.
 * @return  0 when bn or internal representation of bn is NULL.
 */
int wolfSSL_BN_one(WOLFSSL_BIGNUM* bn)
{
    int ret;

    /* Validate parameter. */
    if (BN_IS_NULL(bn)) {
        ret = 0;
    }
    else {
        /* Set to value one. */
        ret = wolfSSL_BN_set_word(bn, 1);
    }

    return ret;
}

/* Get the value of the MP integer as a word.
 *
 * Assumes the MP integer value will fit in a word.
 *
 * @param [in] mp  MP integer.
 * @return  Value of MP integer as an unsigned long.
 */
static WOLFSSL_BN_ULONG wolfssl_bn_get_word_1(mp_int *mp) {
#if DIGIT_BIT >= (SIZEOF_LONG * CHAR_BIT)
    return (WOLFSSL_BN_ULONG)mp->dp[0];
#else
    WOLFSSL_BN_ULONG ret = 0UL;
    unsigned int i;

    for (i = 0; i < (unsigned int)mp->used; ++i) {
        ret |= ((WOLFSSL_BN_ULONG)mp->dp[i]) << (DIGIT_BIT * i);
    }

    return ret;
#endif
}

/* Return the value of big number as an unsigned long if possible.
 *
 * @param [in] bn  Big number to get value from.
 * @return  Value or 0xFFFFFFFFL if bigger than unsigned long.
 */
WOLFSSL_BN_ULONG wolfSSL_BN_get_word(const WOLFSSL_BIGNUM* bn)
{
    WOLFSSL_BN_ULONG ret;

    WOLFSSL_ENTER("wolfSSL_BN_get_word");

    /* Validate parameter. */
    if (BN_IS_NULL(bn)) {
        WOLFSSL_MSG("Invalid argument");
        ret = 0;
    }
    /* Check whether big number is to fit in an unsigned long. */
    else if (wolfSSL_BN_num_bytes(bn) > (int)sizeof(unsigned long)) {
        WOLFSSL_MSG("bignum is larger than unsigned long");
        ret = WOLFSSL_BN_MAX_VAL;
    }
    else {
        /* Get the word from the internal representation. */
        ret = wolfssl_bn_get_word_1((mp_int*)bn->internal);
    }

    return ret;
}

/* Set the big number to the value in the word.
 *
 * Return code compliant with OpenSSL.
 *
 * @param [in, out] bn  Big number to set.
 * @param [in       w   Word to set.
 * @return  1 on success.
 * @return  0 when bn is NULL or setting value failed.
 */
int wolfSSL_BN_set_word(WOLFSSL_BIGNUM* bn, unsigned long w)
{
    int ret = 1;

    WOLFSSL_ENTER("wolfSSL_BN_set_word");

    /* Validate parameters. */
    if (BN_IS_NULL(bn)) {
        WOLFSSL_MSG("bn NULL error");
        ret = 0;
    }

    /* Set the word into the internal representation. */
    if ((ret == 1) && (mp_set_int((mp_int*)bn->internal, w) != MP_OKAY)) {
        WOLFSSL_MSG("mp_init_set_int error");
        ret = 0;
    }

    return ret;
}

/*******************************************************************************
 * Comparison APIs
 ******************************************************************************/

/* Compares two big numbers. a <=> b
 *
 * NULL equals NULL
 * NULL less than not NULL
 * not NULL greater than NULL.
 *
 * Return compliant with OpenSSL.
 *
 * @param [in] bn  First big number to compare.
 * @param [in] bn  Second big number to compare.
 * @return  -1 when a is less than b (a < b).
 * @return  0 when a is equal to b (a == b).
 * @return  1 when a is greater than b (a > b).
 * @return  0 when bn or internal representation of bn is NULL.
 */
int wolfSSL_BN_cmp(const WOLFSSL_BIGNUM* a, const WOLFSSL_BIGNUM* b)
{
    int ret;
    int bIsNull;

    WOLFSSL_ENTER("wolfSSL_BN_cmp");

    /* Must know whether b is NULL. */
    bIsNull = BN_IS_NULL(b);
    /* Check whether a is NULL. */
    if (BN_IS_NULL(a)) {
        if (bIsNull) {
            /* NULL equals NULL. */
            ret = 0;
        }
        else {
            /* NULL less than not NULL. */
            ret = -1;
        }
    }
    else if (bIsNull) {
        /* not NULL greater than NULL. */
        ret = 1;
    }
    else {
        /* Compare big numbers with wolfCrypt. */
        ret = mp_cmp((mp_int*)a->internal, (mp_int*)b->internal);
        /* Convert wolfCrypt return value. */
        if (ret == MP_EQ) {
            ret = 0;
        }
        else if (ret == MP_GT) {
            ret = 1;
        }
        else {
            ret = -1;
        }
    }

    return ret;
}

/* Indicates whether a big number is the value 0.
 *
 * Return compliant with OpenSSL.
 *
 * @param [in] bn  Big number to use.
 * @return  1 when number is zero.
 * @return  0 when number is not zero.
 * @return  0 when bn or internal representation of bn is NULL.
 */
int wolfSSL_BN_is_zero(const WOLFSSL_BIGNUM* bn)
{
    int ret;

    WOLFSSL_ENTER("wolfSSL_BN_is_zero");

    /* Validate parameter. */
    if (BN_IS_NULL(bn)) {
        ret = 0;
    }
    else {
        /* wolfCrypt checks whether value is 0. */
        ret = (mp_iszero((mp_int*)bn->internal) == MP_YES);
    }

    return ret;
}

/* Indicates whether a big number is the value 1.
 *
 * Return compliant with OpenSSL.
 *
 * @param [in] bn  Big number to use.
 * @return  1 when number is one.
 * @return  0 when number is not one.
 * @return  0 when bn or internal representation of bn is NULL.
 */
int wolfSSL_BN_is_one(const WOLFSSL_BIGNUM* bn)
{
    int ret;

    WOLFSSL_ENTER("wolfSSL_BN_is_one");

    /* Validate parameter. */
    if (BN_IS_NULL(bn)) {
        ret = 0;
    }
    else {
        /* wolfCrypt checks whether value is 1. */
        ret = (mp_cmp_d((mp_int*)bn->internal, 1) == MP_EQ);
    }

    return ret;
}

/* Indicates whether a big number is the value passed in.
 *
 * Return compliant with OpenSSL.
 *
 * @param [in] bn  Big number to use.
 * @return  1 when big number is the value.
 * @return  0 when big number is not the value.
 * @return  0 when bn or internal representation of bn is NULL.
 */
int wolfSSL_BN_is_word(const WOLFSSL_BIGNUM* bn, WOLFSSL_BN_ULONG w)
{
    int ret;

    WOLFSSL_ENTER("wolfSSL_BN_is_word");

    /* Validate parameter. */
    if (BN_IS_NULL(bn)) {
        WOLFSSL_MSG("bn NULL error");
        ret = 0;
    }
    else
#if DIGIT_BIT < (SIZEOF_LONG * CHAR_BIT)
    /* When value is greater than what can be stored in one digit - special
     * case. */
    if (w > (WOLFSSL_BN_ULONG)MP_MASK) {
        /* TODO: small stack */
        mp_int w_mp;

        /* Create a MP to hold the number. */
        if (mp_init(&w_mp) != MP_OKAY) {
            ret = 0;
        }
        /* Set the value - held in more than one digit. */
        else if (mp_set_int(&w_mp, w) != MP_OKAY) {
            ret = 0;
        }
        else {
            /* Compare MP representations. */
            ret = (mp_cmp((mp_int *)bn->internal, &w_mp) == MP_EQ);
            mp_free(&w_mp);
        }
    }
    else
#endif
    {
        /* wolfCrypt checks whether it is the value. */
        ret = (mp_isword((mp_int*)bn->internal, (mp_digit)w) == MP_YES);
    }

    return ret;
}


/*******************************************************************************
 * Word operation APIs.
 ******************************************************************************/

/* Add/subtract a word to/from a big number.
 *
 * Internal function for adding/subtracting an unsigned long from a
 * WOLFSSL_BIGNUM. To add, pass "sub" as 0. To subtract, pass it as 1.
 *
 * @param [in, out] bn   Big number to operate on.
 * @param [in]      w    Word to operate with.
 * @param [in]      sub  Indicates whether operation to perform is a subtract.
 * @return  1 on success.
 * @return  0 in failure.
 */
static int wolfssl_bn_add_word_int(WOLFSSL_BIGNUM *bn, WOLFSSL_BN_ULONG w,
    int sub)
{
    int ret = 1;
#if DIGIT_BIT < (SIZEOF_LONG * CHAR_BIT)
#ifdef WOLFSSL_SMALL_STACK
    mp_int* w_mp = NULL;
#else
    mp_int w_mp[1];
#endif /* WOLFSSL_SMALL_STACK */
#endif

#if DIGIT_BIT < (SIZEOF_LONG * CHAR_BIT)
#ifdef WOLFSSL_SMALL_STACK
    /* Allocate temporary MP integer. */
    w_mp = (mp_int*)XMALLOC(sizeof(*w_mp), NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (w_mp == NULL) {
        ret = 0;
    }
    else
#endif /* WOLFSSL_SMALL_STACK */
    {
        /* Clear out MP integer so it can be freed. */
        XMEMSET(w_mp, 0, sizeof(*w_mp));
    }
#endif

    /* Validate parameters. */
    if (BN_IS_NULL(bn)) {
        WOLFSSL_MSG("bn NULL error");
        ret = 0;
    }

    if (ret == 1) {
        int rc = 0;
#if DIGIT_BIT < (SIZEOF_LONG * CHAR_BIT)
        if (w > (WOLFSSL_BN_ULONG)MP_MASK) {
            /* Initialize temporary MP integer. */
            if (mp_init(w_mp) != MP_OKAY) {
                ret = 0;
            }
            /* Set value into temporary MP integer. */
            if ((ret == 1) && (mp_set_int(w_mp, w) != MP_OKAY)) {
                ret = 0;
            }
            if (ret == 1) {
                if (sub) {
                    /* Subtract as MP integer. */
                    rc = mp_sub((mp_int *)bn->internal, w_mp,
                        (mp_int *)bn->internal);
                }
                else {
                    /* Add as MP integer. */
                    rc = mp_add((mp_int *)bn->internal, w_mp,
                        (mp_int *)bn->internal);
                }
                if (rc != MP_OKAY) {
                    WOLFSSL_MSG("mp_add/sub error");
                    ret = 0;
                }
            }
        }
        else
#endif
        {
            if (sub) {
                /* Subtract word from MP integer. */
                rc = mp_sub_d((mp_int*)bn->internal, (mp_digit)w,
                    (mp_int*)bn->internal);
            }
            else {
                /* Add word from MP integer. */
                rc = mp_add_d((mp_int*)bn->internal, (mp_digit)w,
                    (mp_int*)bn->internal);
            }
            if (rc != MP_OKAY) {
                WOLFSSL_MSG("mp_add/sub_d error");
                ret = 0;
            }
        }
    }

#if DIGIT_BIT < (SIZEOF_LONG * CHAR_BIT)
    mp_free(w_mp);
#ifdef WOLFSSL_SMALL_STACK
    XFREE(w_mp, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif /* WOLFSSL_SMALL_STACK */
#endif
    return ret;
}

/* Add a word to a big number.
 *
 * Return code compliant with OpenSSL.
 *
 * @param [in, out] bn   Big number to operate on.
 * @param [in]      w    Word to operate with.
 * @return  1 on success.
 * @return  0 in failure.
 */
int wolfSSL_BN_add_word(WOLFSSL_BIGNUM *bn, WOLFSSL_BN_ULONG w)
{
    int ret;

    WOLFSSL_ENTER("wolfSSL_BN_add_word");

    ret = wolfssl_bn_add_word_int(bn, w, 0);

    WOLFSSL_LEAVE("wolfSSL_BN_add_word", ret);

    return ret;
}

/* Subtract a word from a big number.
 *
 * Return code compliant with OpenSSL.
 *
 * @param [in, out] bn   Big number to operate on.
 * @param [in]      w    Word to operate with.
 * @return  1 on success.
 * @return  0 in failure.
 */
int wolfSSL_BN_sub_word(WOLFSSL_BIGNUM* bn, WOLFSSL_BN_ULONG w)
{
    int ret;

    WOLFSSL_ENTER("wolfSSL_BN_sub_word");

    ret = wolfssl_bn_add_word_int(bn, w, 1);

    WOLFSSL_LEAVE("wolfSSL_BN_sub_word", ret);

    return ret;
}

#if defined(WOLFSSL_KEY_GEN) && (!defined(NO_RSA) || !defined(NO_DH) || \
    !defined(NO_DSA))
/* Calculate bn modulo word w. bn % w
 *
 * Return code compliant with OpenSSL.
 *
 * @return  Word result on success
 * @return  -1 on error
 */
WOLFSSL_BN_ULONG wolfSSL_BN_mod_word(const WOLFSSL_BIGNUM *bn,
    WOLFSSL_BN_ULONG w)
{
    WOLFSSL_BN_ULONG ret = 0;

    WOLFSSL_ENTER("wolfSSL_BN_mod_word");

    /* Validate parameters. */
    if (BN_IS_NULL(bn)) {
        WOLFSSL_MSG("bn NULL error");
        ret = (WOLFSSL_BN_ULONG)-1;
    }

#if DIGIT_BIT < (SIZEOF_LONG * CHAR_BIT)
    if ((ret == 0) && (w > (WOLFSSL_BN_ULONG)MP_MASK)) {
        /* TODO: small stack */
        mp_int w_mp;
        mp_int r_mp;

        /* Memset MP integers to be safe to free. */
        XMEMSET(&w_mp, 0, sizeof(w_mp));
        XMEMSET(&r_mp, 0, sizeof(r_mp));

        /* Initialize MP integer to hold word. */
        if (mp_init(&w_mp) != MP_OKAY) {
            ret = (WOLFSSL_BN_ULONG)-1;
        }
        /* Initialize MP integer to hold result word. */
        if ((ret == 0) && (mp_init(&r_mp) != MP_OKAY)) {
            ret = (WOLFSSL_BN_ULONG)-1;
        }
        /* Set modulus word into MP integer. */
        if ((ret == 0) && (mp_set_int(&w_mp, w) != MP_OKAY)) {
            ret = (WOLFSSL_BN_ULONG)-1;
        }
        /* Calculate modulus result. */
        if ((ret == 0) && (mp_mod((mp_int *)bn->internal, &w_mp, &r_mp) !=
                MP_OKAY)) {
            WOLFSSL_MSG("mp_mod error");
            ret = (WOLFSSL_BN_ULONG)-1;
        }
        if (ret == 0) {
            /* Get modulus result into an unsigned long. */
            ret = wolfssl_bn_get_word_1(&r_mp);
        }

        /* Dispose of dynamically allocated data. */
        mp_free(&r_mp);
        mp_free(&w_mp);
    }
    else
#endif
    if (ret == 0) {
        mp_digit mp_ret;

        /* Calculate modulus result using wolfCrypt. */
        if (mp_mod_d((mp_int*)bn->internal, (mp_digit)w, &mp_ret) != MP_OKAY) {
            WOLFSSL_MSG("mp_add_d error");
            ret = (WOLFSSL_BN_ULONG)-1;
        }
        else {
            /* Return result. */
            ret = (WOLFSSL_BN_ULONG)mp_ret;
        }
    }

    return ret;
}
#endif /* WOLFSSL_KEY_GEN && (!NO_RSA || !NO_DH || !NO_DSA) */

/*******************************************************************************
 * Shift APIs
 ******************************************************************************/

#ifndef WOLFSSL_SP_MATH
/* Shift the value in bn left by n bits into r. r = bn << n
 *
 * Return code compliant with OpenSSL.
 *
 * @return  1 on success.
 * @return  0 when r or bn or internal representation is NULL.
 * @return  0 on failure.
 */
int wolfSSL_BN_lshift(WOLFSSL_BIGNUM *r, const WOLFSSL_BIGNUM *bn, int n)
{
    int ret = 1;

    WOLFSSL_ENTER("wolfSSL_BN_lshift");

    /* Validate parameters. */
    if (BN_IS_NULL(r) || BN_IS_NULL(bn)) {
        WOLFSSL_MSG("bn NULL error");
        ret = 0;
    }
    else if (n < 0) {
        WOLFSSL_MSG("n value error");
        ret = 0;
    }

    /* Use wolfCrypt perform operation. */
    if ((ret == 1) && (mp_mul_2d((mp_int*)bn->internal, n,
            (mp_int*)r->internal) != MP_OKAY)) {
        WOLFSSL_MSG("mp_mul_2d error");
        ret = 0;
    }

    return ret;
}

/* Shift the value in bn right by n bits into r. r = bn >> n
 *
 * Return code compliant with OpenSSL.
 *
 * @return  1 on success.
 * @return  0 when r or bn or internal representation is NULL.
 * @return  0 on failure.
 */
int wolfSSL_BN_rshift(WOLFSSL_BIGNUM *r, const WOLFSSL_BIGNUM *bn, int n)
{
    int ret = 1;

    WOLFSSL_ENTER("wolfSSL_BN_rshift");

    /* Validate parameters. */
    if (BN_IS_NULL(r) || BN_IS_NULL(bn)) {
        WOLFSSL_MSG("bn NULL error");
        ret = 0;
    }
    else if (n < 0) {
        WOLFSSL_MSG("n value error");
        ret = 0;
    }

    /* Use wolfCrypt perform operation. */
    if ((ret == 1) && (mp_div_2d((mp_int*)bn->internal, n, (mp_int*)r->internal,
            NULL) != MP_OKAY)) {
        WOLFSSL_MSG("mp_mul_2d error");
        ret = 0;
    }

    return ret;
}
#endif

/*******************************************************************************
 * Simple Math APIs
 ******************************************************************************/

/* Add a to b into r. r = a + b
 *
 * Return code compliant with OpenSSL.
 *
 * @param [out] r  Big number to put result into.
 * @param [in]  a  Big number to be added to.
 * @param [in]  b  Big number to add with.
 *
 * @return  1 on success.
 * @return  0 when r, a or b or internal representation is NULL.
 * @return  0 on failure.
 */
int wolfSSL_BN_add(WOLFSSL_BIGNUM *r, WOLFSSL_BIGNUM *a, WOLFSSL_BIGNUM *b)
{
    int ret = 1;

    WOLFSSL_ENTER("wolfSSL_BN_add");

    /* Validate parameters. */
    if (BN_IS_NULL(r) || BN_IS_NULL(a) || BN_IS_NULL(b)) {
        WOLFSSL_MSG("bn NULL error");
        ret = 0;
    }

    /* Add the internal representations into internal representation. */
    if ((ret == 1) && (mp_add((mp_int*)a->internal, (mp_int*)b->internal,
            (mp_int*)r->internal) != MP_OKAY)) {
        WOLFSSL_MSG("mp_add_d error");
        ret = 0;
    }

    return ret;
}

/* Subtract a from b into r. r = a - b
 *
 * @param [out] r  Big number to put result into.
 * @param [in]  a  Big number to be subtracted from.
 * @param [in]  b  Big number to subtract with.
 *
 * @return  1 on success.
 * @return  0 when r, a or b is NULL.
 * @return  0 on failure.
 */
int wolfSSL_BN_sub(WOLFSSL_BIGNUM* r, const WOLFSSL_BIGNUM* a,
    const WOLFSSL_BIGNUM* b)
{
    int ret = 1;

    WOLFSSL_ENTER("wolfSSL_BN_sub");

    /* Validate parameters. */
    if (BN_IS_NULL(r) || BN_IS_NULL(a) || BN_IS_NULL(b)) {
        ret = 0;
    }

    /* Have wolfCrypt perform operation with internal representations. */
    if ((ret == 1) && (mp_sub((mp_int*)a->internal,(mp_int*)b->internal,
            (mp_int*)r->internal) != MP_OKAY)) {
        ret = 0;
    }

    WOLFSSL_LEAVE("wolfSSL_BN_sub mp_sub", ret);
    return ret;
}

/* Multiply a with b into r. r = a * b
 *
 * @param [out] r    Big number to put result into.
 * @param [in]  a    Big number to be multiplied.
 * @param [in]  b    Big number to multiply with.
 * @param [in]  ctx  BN context object. Unused.
 *
 * @return  1 on success.
 * @return  0 when r, a or b is NULL.
 * @return  0 when internal representation of r, a or b is NULL.
 * @return  0 on failure.
 */
int wolfSSL_BN_mul(WOLFSSL_BIGNUM *r, WOLFSSL_BIGNUM *a, WOLFSSL_BIGNUM *b,
    WOLFSSL_BN_CTX *ctx)
{
    int ret = 1;

    (void)ctx;

    WOLFSSL_ENTER("wolfSSL_BN_mul");

    /* Validate parameters. */
    if (BN_IS_NULL(r) || BN_IS_NULL(a) || BN_IS_NULL(b)) {
        ret = 0;
    }

    /* Have wolfCrypt perform operation with internal representations. */
    if ((ret == 1) && (mp_mul((mp_int*)a->internal, (mp_int*)b->internal,
            (mp_int*)r->internal) != MP_OKAY)) {
        ret = 0;
    }

    WOLFSSL_LEAVE("wolfSSL_BN_mul", ret);
    return ret;
}

#ifndef WOLFSSL_SP_MATH
/* Divide a by b into dv and put remainder into rem. dv = a / b, rem = a % b
 *
 * @param [out] dv   Big number to put division result into.
 * @param [out] rem  Big number to put remainder into.
 * @param [in]  a    Big number to be divided.
 * @param [in]  b    Big number to divide with.
 * @param [in]  ctx  BN context object. Unused.
 *
 * @return  1 on success.
 * @return  0 when dv, rem, a or b is NULL.
 * @return  0 when internal representation of dv, rem, a or b is NULL.
 * @return  0 on failure.
 */
int wolfSSL_BN_div(WOLFSSL_BIGNUM* dv, WOLFSSL_BIGNUM* rem,
    const WOLFSSL_BIGNUM* a, const WOLFSSL_BIGNUM* d, WOLFSSL_BN_CTX* ctx)
{
    int ret = 1;
    WOLFSSL_BIGNUM* res = dv;

    /* BN context not needed. */
    (void)ctx;

    WOLFSSL_ENTER("wolfSSL_BN_div");

    if (BN_IS_NULL(res)) {
        res = wolfSSL_BN_new();
    }

    /* Validate parameters. */
    if (BN_IS_NULL(res) || BN_IS_NULL(rem) || BN_IS_NULL(a) || BN_IS_NULL(d)) {
        ret = 0;
    }

    /* Have wolfCrypt perform operation with internal representations. */
    if ((ret == 1) && (mp_div((mp_int*)a->internal, (mp_int*)d->internal,
            (mp_int*)res->internal, (mp_int*)rem->internal) != MP_OKAY)) {
        ret = 0;
    }

    if (res != dv)
        wolfSSL_BN_free(res);

    WOLFSSL_LEAVE("wolfSSL_BN_div", ret);
    return ret;
}
#endif

/* Calculate a mod b into r. r = a % b
 *
 * @param [out] r    Big number to put result into.
 * @param [in]  a    Big number to reduced
 * @param [in]  b    Big number to reduce with.
 * @param [in]  ctx  BN context object. Unused.
 *
 * @return  1 on success.
 * @return  0 when r, a or b is NULL.
 * @return  0 on failure.
 */
int wolfSSL_BN_mod(WOLFSSL_BIGNUM* r, const WOLFSSL_BIGNUM* a,
                  const WOLFSSL_BIGNUM* b, const WOLFSSL_BN_CTX* c)
{
    int ret = 1;

    (void)c;

    WOLFSSL_ENTER("wolfSSL_BN_mod");

    /* Validate parameters. */
    if (BN_IS_NULL(r) || BN_IS_NULL(a) || BN_IS_NULL(b)) {
        ret = 0;
    }

    /* Have wolfCrypt perform operation with internal representations. */
    if ((ret == 1) && (mp_mod((mp_int*)a->internal,(mp_int*)b->internal,
            (mp_int*)r->internal) != MP_OKAY)) {
        ret = 0;
    }

    WOLFSSL_LEAVE("wolfSSL_BN_mod mp_mod", ret);
    return ret;
}

/*******************************************************************************
 * Math and Mod APIs
 ******************************************************************************/

#ifndef WOLFSSL_SP_MATH
/* Add a to b modulo m into r. r = a + b (mod m)
 *
 * @param [in, out] r    Big number to hold result.
 * @param [in]      a    Big number to add to.
 * @param [in]      b    Big number to add with.
 * @param [in]      m    Big number that is the modulus.
 * @param [in]      ctx  BN context. Not used.
 * @return  1 on success.
 * @return  0 when r, a or b or internal representation is NULL.
 * @return  0 on calculation failure.
 */
int wolfSSL_BN_mod_add(WOLFSSL_BIGNUM *r, const WOLFSSL_BIGNUM *a,
    const WOLFSSL_BIGNUM *b, const WOLFSSL_BIGNUM *m, WOLFSSL_BN_CTX *ctx)
{
    int ret = 1;

    /* BN context not needed. */
    (void)ctx;

    WOLFSSL_ENTER("wolfSSL_BN_add");

    /* Validate parameters. */
    if (BN_IS_NULL(r) || BN_IS_NULL(a) || BN_IS_NULL(b) || BN_IS_NULL(m)) {
        WOLFSSL_MSG("bn NULL error");
        ret = 0;
    }

    /* Perform operation with wolfCrypt. */
    if ((ret == 1) && (mp_addmod((mp_int*)a->internal, (mp_int*)b->internal,
            (mp_int*)m->internal, (mp_int*)r->internal) != MP_OKAY)) {
        WOLFSSL_MSG("mp_add_d error");
        ret = 0;
    }

    return ret;
}
#endif

/* Calculate a multiplied by b, mod m into r. r = (a * b) % m
 *
 * @param [out] r    Big number to put result into.
 * @param [in]  a    Base as a big number.
 * @param [in]  b    Multiplier as a big number.
 * @param [in]  m    Modulus as a big number.
 * @param [in]  ctx  BN context object. Unused.
 *
 * @return  1 on success.
 * @return  0 when r, a, b or m is NULL.
 * @return  0 on failure.
 */
int wolfSSL_BN_mod_mul(WOLFSSL_BIGNUM *r, const WOLFSSL_BIGNUM *a,
    const WOLFSSL_BIGNUM *b, const WOLFSSL_BIGNUM *m, WOLFSSL_BN_CTX *ctx)
{
    int ret = 1;

    /* BN context not needed. */
    (void)ctx;

    WOLFSSL_ENTER("wolfSSL_BN_mod_mul");

    /* Validate parameters. */
    if (BN_IS_NULL(r) || BN_IS_NULL(a) || BN_IS_NULL(b) || BN_IS_NULL(m)) {
        WOLFSSL_MSG("Bad Argument");
        ret = 0;
    }

    /* Have wolfCrypt perform operation with internal representations. */
    if ((ret == 1) && (mp_mulmod((mp_int*)a->internal, (mp_int*)b->internal,
            (mp_int*)m->internal, (mp_int*)r->internal)) != MP_OKAY) {
        ret = 0;
    }

    WOLFSSL_LEAVE("wolfSSL_BN_mod_mul", ret);
    return ret;
}


/* Calculate a to the power of e, mod m into r. r = (a ^ e) % m
 *
 * @param [out] r    Big number to put result into.
 * @param [in]  a    Base as a big number.
 * @param [in]  e    Exponent as a big number.
 * @param [in]  m    Modulus as a big number.
 * @param [in]  ctx  BN context object. Unused.
 *
 * @return  1 on success.
 * @return  0 when r, a, p or m is NULL.
 * @return  0 on failure.
 */
int wolfSSL_BN_mod_exp(WOLFSSL_BIGNUM *r, const WOLFSSL_BIGNUM *a,
    const WOLFSSL_BIGNUM *e, const WOLFSSL_BIGNUM *m, WOLFSSL_BN_CTX *ctx)
{
    int ret = 1;

    /* BN context not needed. */
    (void)ctx;

    WOLFSSL_ENTER("wolfSSL_BN_mod_exp");

    /* Validate parameters. */
    if (BN_IS_NULL(r) || BN_IS_NULL(a) || BN_IS_NULL(e) || BN_IS_NULL(m)) {
        WOLFSSL_MSG("Bad Argument");
        ret = 0;
    }

    /* Have wolfCrypt perform operation with internal representations. */
    if ((ret == 1) && (mp_exptmod((mp_int*)a->internal, (mp_int*)e->internal,
            (mp_int*)m->internal, (mp_int*)r->internal)) != MP_OKAY) {
        ret = 0;
    }

    WOLFSSL_LEAVE("wolfSSL_BN_mod_exp", ret);
    return ret;
}

/* Calculate the modular inverse of a mod m into r. r = a^-1 mod m
 *
 * A new big number is allocated when r is NULL.
 *
 * @param [in, out] r    Big number to hold result. May be NULL.
 * @param [in]      a    Big number to invert.
 * @param [in]      m    Big number that is the modulus.
 * @param [in]      ctx  BN context. Not used.
 * @return  Big number holding result on success.
 * @return  NULL on failure.
 */
WOLFSSL_BIGNUM *wolfSSL_BN_mod_inverse(WOLFSSL_BIGNUM *r, WOLFSSL_BIGNUM *a,
    const WOLFSSL_BIGNUM *m, WOLFSSL_BN_CTX *ctx)
{
    int err = 0;
    WOLFSSL_BIGNUM* t = NULL;

    /* BN context not needed. */
    (void)ctx;

    WOLFSSL_ENTER("wolfSSL_BN_mod_inverse");

    /* Validate parameters. */
    if (BN_IS_NULL(a) || BN_IS_NULL(m) || ((r != NULL) &&
            (r->internal == NULL))) {
        WOLFSSL_MSG("a or n NULL error");
        err = 1;
    }
    /* Check whether we have a result big number. */
    if ((!err) && (r == NULL)) {
        /* Allocate a new big number to hold result and be returned. */
        t = wolfSSL_BN_new();
        if (t == NULL){
            WOLFSSL_MSG("WolfSSL_BN_new() failed");
            err = 1;
        }
        r = t;
    }

    /* Compute inverse of a modulo n and return in r */
    if ((!err) && (mp_invmod((mp_int *)a->internal, (mp_int *)m->internal,
            (mp_int*)r->internal) != MP_OKAY)) {
        WOLFSSL_MSG("mp_invmod() error");
        err = 1;
    }

    if (err) {
        wolfSSL_BN_free(t);
        r = NULL;
    }
    return r;
}

/*******************************************************************************
 * Other Math APIs
 ******************************************************************************/

#if !defined(NO_RSA) && defined(WOLFSSL_KEY_GEN)
/* Needed to get mp_gcd. */
/* Greatest Common Divisor (GCM) of a and b into r. r = GCD(a, b)
 *
 * @param [out] r    Big number to put result into.
 * @param [in]  a    First big number.
 * @param [in]  b    Second big number.
 * @param [in]  ctx  BN context object. Unused.
 *
 * @return  1 on success.
 * @return  0 when r, a or b is NULL.
 * @return  0 when internal representation of r, a or b is NULL.
 * @return  0 on failure.
 */
int wolfSSL_BN_gcd(WOLFSSL_BIGNUM* r, WOLFSSL_BIGNUM* a, WOLFSSL_BIGNUM* b,
    WOLFSSL_BN_CTX* ctx)
{
    int ret = 1;

    /* BN context not needed. */
    (void)ctx;

    WOLFSSL_ENTER("wolfSSL_BN_gcd");

    /* Validate parameters. */
    if (BN_IS_NULL(r) || BN_IS_NULL(a) || BN_IS_NULL(b)) {
        ret = 0;
    }

    /* Have wolfCrypt perform operation with internal representations. */
    if ((ret == 1) && (mp_gcd((mp_int*)a->internal, (mp_int*)b->internal,
            (mp_int*)r->internal) != MP_OKAY)) {
        ret = 0;
    }

    WOLFSSL_LEAVE("wolfSSL_BN_gcd", ret);
    return ret;
}
#endif /* !NO_RSA && WOLFSSL_KEY_GEN */

/*******************************************************************************
 * Random APIs
 ******************************************************************************/

/* Generates a random number up to bits long.
 *
 * @param [in, out] bn       Big number to generate into.
 * @param [in]      bits     Number of bits in word.
 * @param [in]      top      Whether top bits must be set.
 *                           Valid values: WOLFSSL_BN_RAND_TOP_ANY,
 *                           WOLFSSL_BN_RAND_TOP_ONE, WOLFSSL_BN_RAND_TOP_TWO.
 * @param [in]      bottom   Whether bottom bit must be set.
 *                           Valid values: WOLFSSL_BN_RAND_BOTTOM_ANY,
                             WOLFSSL_BN_RAND_BOTTOM_ODD.
 * @return  1 on success.
 * @return  0 when bn is NULL.
 * @return  0 when bits is invalid.
 * @return  0 when bits and top/bottom are invalid.
 * @return  0 when generation fails.
 */
int wolfSSL_BN_rand(WOLFSSL_BIGNUM* bn, int bits, int top, int bottom)
{
    int ret = 1;
    word32 len = (word32)((bits + 7) / 8);
    WC_RNG* rng;

    WOLFSSL_ENTER("wolfSSL_BN_rand");

    /* Validate parameters. */
    if (BN_IS_NULL(bn)) {
        WOLFSSL_MSG("Bad argument - bn");
        ret = 0;
    }
    else if (bits < 0) {
        WOLFSSL_MSG("Bad argument - bits < 0");
        ret = 0;
    }
    else if ((bits == 0) && ((bottom != WOLFSSL_BN_RAND_BOTTOM_ANY) ||
            (top != WOLFSSL_BN_RAND_TOP_ANY))) {
        WOLFSSL_MSG("Bad top/bottom - bits == 0");
        ret = 0;
    }
    else if ((bits == 1) && (top > WOLFSSL_BN_RAND_TOP_ONE)) {
        WOLFSSL_MSG("Bad top - bits == 1");
        ret = 0;
    }

    /* Handle simple case of zero bits. */
    if ((ret == 1) && (bits == 0)) {
        mp_zero((mp_int*)bn->internal);
    }
    else if (ret == 1) {
        byte* buff = NULL;

        /* Get random to global random to generate bits. */
        if ((rng = wolfssl_make_global_rng()) == NULL) {
            WOLFSSL_MSG("Failed to use global RNG.");
            ret = 0;
        }

        /* Allocate buffer to hold generated bits. */
        if ((ret == 1) && ((buff = (byte*)XMALLOC(len, NULL,
                DYNAMIC_TYPE_TMP_BUFFER)) == NULL)) {
            WOLFSSL_MSG("Failed to allocate buffer.");
            ret = 0;
        }
        /* Generate bytes to cover bits. */
        if ((ret == 1) && wc_RNG_GenerateBlock(rng, buff, len) != 0) {
            WOLFSSL_MSG("wc_RNG_GenerateBlock failed");
            ret = 0;
        }
        /* Read bytes in to big number. */
        if ((ret == 1) && mp_read_unsigned_bin((mp_int*)bn->internal, buff, len)
                != MP_OKAY) {
            WOLFSSL_MSG("mp_read_unsigned_bin failed");
            ret = 0;
        }
        /* Dispose of buffer - no longer needed. */
        XFREE(buff, NULL, DYNAMIC_TYPE_TMP_BUFFER);

        if (ret == 1) {
            /* Truncate to requested bit length. */
            mp_rshb((mp_int*)bn->internal, 8 - (bits % 8));
        }

        /* Set top bit when required. */
        if ((ret == 1) && (top >= WOLFSSL_BN_RAND_TOP_ONE) &&
                (mp_set_bit((mp_int*)bn->internal, bits - 1) != MP_OKAY)) {
            WOLFSSL_MSG("Failed to set top bit");
            ret = 0;
        }
        /* Set second top bit when required. */
        if ((ret == 1) && (top > WOLFSSL_BN_RAND_TOP_ONE) &&
                (mp_set_bit((mp_int*)bn->internal, bits - 2) != MP_OKAY)) {
            WOLFSSL_MSG("Failed to set second top bit");
            ret = 0;
        }
        /* Set bottom bit when required. */
        if ((ret == 1) && (bottom == WOLFSSL_BN_RAND_BOTTOM_ODD) &&
                (mp_set_bit((mp_int*)bn->internal, 0) != MP_OKAY)) {
            WOLFSSL_MSG("Failed to set 0th bit");
            ret = 0;
        }
    }

    WOLFSSL_LEAVE("wolfSSL_BN_rand", ret);

    return ret;
}

/* Generates a pseudo-random number up to bits long.
 *
 * Implemented using wolfSSL_BN_rand().
 *
 * @param [in, out] bn       Big number to generate into.
 * @param [in]      bits     Number of bits in word.
 * @param [in]      top      Whether top bits must be set.
 *                           Valid values: WOLFSSL_BN_RAND_TOP_ANY,
 *                           WOLFSSL_BN_RAND_TOP_ONE, WOLFSSL_BN_RAND_TOP_TWO.
 * @param [in]      bottom   Whether bottom bit must be set.
 *                           Valid values: WOLFSSL_BN_RAND_BOTTOM_ANY,
                             WOLFSSL_BN_RAND_BOTTOM_ODD.
 * @return  1 on success.
 * @return  0 when bn is NULL.
 * @return  0 when bits is invalid.
 * @return  0 when bits and top/bottom are invalid.
 * @return  0 when generation fails.
 */
int wolfSSL_BN_pseudo_rand(WOLFSSL_BIGNUM* bn, int bits, int top, int bottom)
{
    return wolfSSL_BN_rand(bn, bits, top, bottom);
}

/* Maximum number of trials to attempt at generating a number in range. */
#define RANGE_MAX_TRIALS    100

/* Generate big number to be a value between 0 and range-1.
 *
 * N = length of range input var
 *
 * Generate N-bit length numbers until generated number is less than range
 * @param [in] r      Big number to generate into.
 * @param [in] range  The upper limit of generated number.
 * @return  1 on success.
 * @return  0 on failure.
 */
int wolfSSL_BN_rand_range(WOLFSSL_BIGNUM *r, const WOLFSSL_BIGNUM *range)
{
    int ret = 1;

    WOLFSSL_ENTER("wolfSSL_BN_rand_range");

    /* Validate parameters. */
    if (BN_IS_NULL(r) || BN_IS_NULL(range)) {
        WOLFSSL_MSG("Bad parameter");
        ret = 0;
    }

    if (ret == 1) {
        /* Calculate number of bits in modulus. */
        int n = wolfSSL_BN_num_bits(range);

        if (n <= 1) {
            /* Modulus is 0 or 1. */
            wolfSSL_BN_zero(r);
        }
        else {
            int i;
            /* Try generating a number in range for a limited number of trials.
             */
            for (i = 0; (ret == 1) && (i < RANGE_MAX_TRIALS); i++) {
                /* Generate a random number in range. */
                if (wolfSSL_BN_pseudo_rand(r, n, WOLFSSL_BN_RAND_TOP_ANY,
                        WOLFSSL_BN_RAND_BOTTOM_ANY) == 0) {
                    WOLFSSL_MSG("wolfSSL_BN_rand error");
                    ret = 0;
                }
                /* Check if in range. */
                else if (wolfSSL_BN_cmp(r, range) < 0) {
                    break;
                }
            }
            /* Fail if max trial attempts made. */
            if (i >= RANGE_MAX_TRIALS) {
                WOLFSSL_MSG("wolfSSL_BN_rand_range too many iterations");
                ret = 0;
            }
        }
    }

    return ret;
}

/*******************************************************************************
 * Prime APIs
 ******************************************************************************/

#if defined(WOLFSSL_KEY_GEN) && (!defined(NO_RSA) || !defined(NO_DH) || \
    !defined(NO_DSA))

/* Generate a prime number.
 *
 * @param [in, out] prime  Big number to generate into.
 * @param [in]      bits   Number of bits in generated number.
 * @param [in]      safe   Whether number must be a safe prime.
 * @param [in]      add    Value to add when generating. Not used.
 * @param [in]      rem    Remainder of number modulo add. Not used.
 * @param [in]      cb     Generation callback. Not used.
 * @return  1 on success.
 * @return  0 when prime is NULL.
 * @return  0 when safe required or add or rem is not NULL.
 * @return  0 on generation failure.
 */
int wolfSSL_BN_generate_prime_ex(WOLFSSL_BIGNUM* prime, int bits,
    int safe, const WOLFSSL_BIGNUM* add, const WOLFSSL_BIGNUM* rem,
    WOLFSSL_BN_GENCB* cb)
{
    int ret = 1;
#ifdef WOLFSSL_SMALL_STACK
    WC_RNG* tmpRng = NULL;
#else
    WC_RNG  tmpRng[1];
#endif
    WC_RNG* rng = NULL;
    int localRng = 0;

    /* Callback not used. */
    (void)cb;

    WOLFSSL_ENTER("wolfSSL_BN_generate_prime_ex");

    /* Check unsupported parameters. */
    if ((safe == 1) || (add != NULL) || (rem != NULL)) {
        ret = 0;
    }
    /* Validate parameters. */
    else if (BN_IS_NULL(prime)) {
        ret = 0;
    }

    /* Create a new RNG or use global. */
    if ((ret == 1) && ((rng = wolfssl_make_rng(tmpRng, &localRng)) == NULL)) {
        ret = 0;
    }

    /* Use wolfCrypt to generate a prime. */
    if ((ret == 1) && (mp_rand_prime((mp_int*)prime->internal, (bits + 7) / 8,
            rng, NULL) != MP_OKAY)) {
        ret = 0;
    }

    if (localRng) {
        /* Dispose of local RNG that was created. */
        wc_FreeRng(rng);
    #ifdef WOLFSSL_SMALL_STACK
        XFREE(rng, NULL, DYNAMIC_TYPE_RNG);
    #endif
    }

    WOLFSSL_LEAVE("wolfSSL_BN_generate_prime_ex", ret);

    return ret;
}

/* Check whether a big number is prime.
 *
 * Return code compliant with OpenSSL.
 *
 * @param [in] bn      Big number to check.
 * @param [in] checks  Number of Miller-Rabin tests to perform.
 * @param [in] ctx     BN context. Not used.
 * @param [in] cb      Generation callback. Not used.
 * @return  1 when number is prime.
 * @return  0 when number is not prime.
 * @return  -1 when bn is NULL or failure when checking.
 */
int wolfSSL_BN_is_prime_ex(const WOLFSSL_BIGNUM *bn, int checks,
    WOLFSSL_BN_CTX *ctx, WOLFSSL_BN_GENCB *cb)
{
    int ret = 1;
    WC_RNG* rng    = NULL;
#ifdef WOLFSSL_SMALL_STACK
    WC_RNG* tmpRng = NULL;
#else
    WC_RNG  tmpRng[1];
#endif
    int localRng = 0;
    int res = MP_NO;

    /* BN context not needed. */
    (void)ctx;
    (void)cb;

    WOLFSSL_ENTER("wolfSSL_BN_is_prime_ex");

    if (BN_IS_NULL(bn)) {
        WOLFSSL_MSG("bn NULL error");
        ret = -1;
    }

    /* Create a new RNG or use global. */
    if ((ret == 1) && ((rng = wolfssl_make_rng(tmpRng, &localRng)) == NULL)) {
        ret = -1;
    }

    if ((ret == 1) && (mp_prime_is_prime_ex((mp_int*)bn->internal, checks, &res,
            rng) != MP_OKAY)) {
        WOLFSSL_MSG("mp_prime_is_prime_ex error");
        ret = -1;
    }

    if (localRng) {
        wc_FreeRng(rng);
    #ifdef WOLFSSL_SMALL_STACK
        XFREE(rng, NULL, DYNAMIC_TYPE_RNG);
    #endif
    }

    if ((ret != -1) && (res != MP_YES)) {
        WOLFSSL_MSG("mp_prime_is_prime_ex not prime");
        ret = 0;
    }

    return ret;
}

#endif /* WOLFSSL_KEY_GEN && (!NO_RSA || !NO_DH || !NO_DSA) */

/*******************************************************************************
 * Print APIs
 ******************************************************************************/

#if !defined(NO_FILESYSTEM) && defined(XFPRINTF)
/* Print big number to file pointer.
 *
 * Return code compliant with OpenSSL.
 *
 * @param [in] fp  File pointer.
 * @param [in] bn  Big number to print.
 * @return  1 on success.
 * @return  0 when fp is a bad file pointer.
 * @return  0 when bn is NULL.
 * @return  0 when creating hex string fails.
 * @return  0 when printing fails.
 */
int wolfSSL_BN_print_fp(XFILE fp, const WOLFSSL_BIGNUM *bn)
{
    int ret = 1;
    char* buf = NULL;

    WOLFSSL_ENTER("wolfSSL_BN_print_fp");

    /* Validate parameters. */
    if ((fp == XBADFILE) || BN_IS_NULL(bn)) {
        WOLFSSL_MSG("bn NULL error");
        ret = 0;
    }

    /* Create a hex string of big number. */
    if ((ret == 1) && ((buf = wolfSSL_BN_bn2hex(bn)) == NULL)) {
        WOLFSSL_MSG("wolfSSL_BN_bn2hex failure");
        ret = 0;
    }

    /* Print hex string to file pointer. */
    if ((ret == 1) && (XFPRINTF(fp, "%s", buf) < 0)) {
        ret = 0;
    }

    /* Dispose of any allocated data. */
    XFREE(buf, NULL, DYNAMIC_TYPE_OPENSSL);

    return ret;
}
#endif /* !NO_FILESYSTEM && XFPRINTF */

/*******************************************************************************
 * BN_CTX APIs
 ******************************************************************************/

/* Allocate and return a new BN context object.
 *
 * BN context not needed for operations.
 *
 * @return  Pointer to dummy object.
 */
WOLFSSL_BN_CTX* wolfSSL_BN_CTX_new(void)
{
    /* wolfcrypt doesn't need BN context. */
    static int ctx;
    WOLFSSL_ENTER("wolfSSL_BN_CTX_new");
    return (WOLFSSL_BN_CTX*)&ctx;
}

/* Initialize a BN context object.
 *
 * BN context not needed for operations.
 *
 * @param [in] ctx  Dummy BN context.
 */
void wolfSSL_BN_CTX_init(WOLFSSL_BN_CTX* ctx)
{
    (void)ctx;
    WOLFSSL_ENTER("wolfSSL_BN_CTX_init");
}


/* Free a BN context object.
 *
 * BN context not needed for operations.
 *
 * @param [in] ctx  Dummy BN context.
 */
void wolfSSL_BN_CTX_free(WOLFSSL_BN_CTX* ctx)
{
    (void)ctx;
    WOLFSSL_ENTER("wolfSSL_BN_CTX_free");
    /* Don't do anything since using dummy, static BN context. */
}

/* Get a big number based on the BN context.
 *
 * @param [in] ctx  BN context. Not used.
 * @return  Big number on success.
 * @return  NULL on failure.
 */
WOLFSSL_BIGNUM *wolfSSL_BN_CTX_get(WOLFSSL_BN_CTX *ctx)
{
    /* ctx is not used - returning a new big number. */
    (void)ctx;

    WOLFSSL_ENTER("wolfSSL_BN_CTX_get");

    /* Return a new big number. */
    return wolfSSL_BN_new();
}

#ifndef NO_WOLFSSL_STUB
/* Start stack of temporary big numbers.
 *
 * Newly allocated big numbers are returned instead of having a stack.
 *
 * @param [in] ctx  BN context. Not used.
 */
void wolfSSL_BN_CTX_start(WOLFSSL_BN_CTX *ctx)
{
    (void)ctx;

    WOLFSSL_ENTER("wolfSSL_BN_CTX_start");
    WOLFSSL_STUB("BN_CTX_start");
    WOLFSSL_MSG("wolfSSL_BN_CTX_start TBD");
}
#endif

#endif /* OPENSSL_EXTRA */

#endif /* !WOLFSSL_SSL_BN_INCLUDED */

