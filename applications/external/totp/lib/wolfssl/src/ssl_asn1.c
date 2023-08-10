/* ssl_asn1.c
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

#if !defined(WOLFSSL_SSL_ASN1_INCLUDED)
    #ifndef WOLFSSL_IGNORE_FILE_WARN
        #warning ssl_asn1.c does not need to be compiled separately from ssl.c
    #endif
#else

/*******************************************************************************
 * ASN1_item APIs
 ******************************************************************************/

#ifndef NO_ASN

#ifdef OPENSSL_EXTRA

#ifdef OPENSSL_ALL

/* Create an ASN1 item of the specified type.
 *
 * @param [out] item  Pointer to location to place new ASN1 item.
 * @param [in]  type  Type of ASN1 item to create.
 * @return  0 on success.
 * @return  1 when item type not supported.
 * @return  1 when item type allocation fails.
 */
static int wolfssl_asn1_item_new(void** item, int type)
{
    int err = 0;

    switch (type) {
        case WOLFSSL_X509_ALGOR_ASN1:
            *(WOLFSSL_X509_ALGOR**)item = wolfSSL_X509_ALGOR_new();
            break;
        case WOLFSSL_ASN1_BIT_STRING_ASN1:
            *(WOLFSSL_ASN1_BIT_STRING**)item = wolfSSL_ASN1_BIT_STRING_new();
            break;
        case WOLFSSL_ASN1_INTEGER_ASN1:
           *(WOLFSSL_ASN1_INTEGER**)item = wolfSSL_ASN1_INTEGER_new();
           break;
        default:
            WOLFSSL_MSG("Type not supported in wolfSSL_ASN1_item_new");
            *(void**)item = NULL;
    }
    /* Check whether an item was put in. */
    if (*(void**)item == NULL) {
        err = 1;
    }

    return err;
}

/* Create a new ASN1 item based on a template.
 *
 * @param [in] tpl  Template of ASN1 items.
 * @return  A new ASN1 item on success.
 * @return  NULL when tpl is NULL, dynamic memory allocation fails or ASN1
 *          item type not supported.
 */
void* wolfSSL_ASN1_item_new(const WOLFSSL_ASN1_ITEM* tpl)
{
    int err = 0;
    void* ret = NULL;
    const WOLFSSL_ASN1_TEMPLATE *mem = NULL;
    size_t i;

    WOLFSSL_ENTER("wolfSSL_ASN1_item_new");

    if (tpl != NULL) {
        ret = (void *)XMALLOC(tpl->size, NULL, DYNAMIC_TYPE_OPENSSL);
    }

    if (ret != NULL) {
        XMEMSET(ret, 0, tpl->size);
        for (mem = tpl->members, i = 0; i < tpl->mcount; mem++, i++) {
            if ((err = wolfssl_asn1_item_new(
                    (void**)(((byte*)ret) + mem->offset), mem->type))) {
                break;
            }
        }
    }

    if (err) {
        wolfSSL_ASN1_item_free(ret, tpl);
        ret = NULL;
    }
    return ret;
}

/* Dispose of an ASN1 item of the specified type.
 *
 * @param [in, out] item  Pointer to an anonymized ASN1 item to free.
 * @param [in]      type  Type of ASN1 item to free.
 */
static void wolfssl_asn1_item_free(void** item, int type)
{
    switch (type) {
        case WOLFSSL_X509_ALGOR_ASN1:
            wolfSSL_X509_ALGOR_free(*(WOLFSSL_X509_ALGOR**)item);
            break;
        case WOLFSSL_ASN1_BIT_STRING_ASN1:
            wolfSSL_ASN1_BIT_STRING_free(*(WOLFSSL_ASN1_BIT_STRING**)item);
            break;
        case WOLFSSL_ASN1_INTEGER_ASN1:
            wolfSSL_ASN1_INTEGER_free(*(WOLFSSL_ASN1_INTEGER**)item);
            break;
        default:
            WOLFSSL_MSG("Type not supported in wolfSSL_ASN1_item_free");
    }
}

/* Dispose of ASN1 item based on a template.
 *
 * @param [in, out] val  ASN item to free.
 * @param [in,      tpl  Template of ASN1 items.
 */
void wolfSSL_ASN1_item_free(void *items, const WOLFSSL_ASN1_ITEM *tpl)
{
    const WOLFSSL_ASN1_TEMPLATE *mem = NULL;
    size_t i;

    WOLFSSL_ENTER("wolfSSL_ASN1_item_free");

    if (items != NULL) {
        for (mem = tpl->members, i = 0; i < tpl->mcount; mem++, i++) {
            wolfssl_asn1_item_free((void**)(((byte*)items) + mem->offset),
                mem->type);
        }
    }
    XFREE(items, NULL, DYNAMIC_TYPE_OPENSSL);
}

/* Offset buf if not NULL or NULL. */
#define bufLenOrNull(buf, len) (((buf) != NULL) ? ((buf) + (len)) : NULL)

/* Encode X509 algorithm as DER.
 *
 * @param [in]      algor  X509 algorithm object.
 * @param [in, out] buf    Buffer to encode into. May be NULL.
 * @return  Length of DER encoding on success.
 * @return  0 on failure.
 */
static int wolfSSL_i2d_X509_ALGOR(const WOLFSSL_X509_ALGOR* algor, byte* buf)
{
    int ret;
    word32 oid = 0;
    word32 idx = 0;

    if (algor->algorithm == 0) {
        WOLFSSL_MSG("X509_ALGOR algorithm not set");
        ret = 0;
    }
    else if (GetObjectId(algor->algorithm->obj, &idx, &oid,
            (word32)algor->algorithm->grp, algor->algorithm->objSz) < 0) {
        WOLFSSL_MSG("Issue getting OID of object");
        ret = 0;
    }
    else {
        ret = (int)SetAlgoID((int)oid, buf, algor->algorithm->grp, 0);
    }

    return ret;
}

/* Encode ASN.1 BIT_STRING as DER.
 *
 * @param [in]      bit_str  BIT_STRING object.
 * @param [in, out] buf      Buffer to encode into. May be NULL.
 * @return  Length of DER encoding on success.
 */
static int wolfSSL_i2d_ASN1_BIT_STRING(const WOLFSSL_ASN1_BIT_STRING* bit_str,
    byte* buf)
{
    int len;

    len = (int)SetBitString((word32)bit_str->length, 0, buf);
    if ((buf != NULL) && (bit_str->data != NULL)) {
        XMEMCPY(buf + len, bit_str->data, (size_t)bit_str->length);
    }

    return len + bit_str->length;
}

/* Encode ASN item as DER.
 *
 * @param [in]      item  Pointer to anonymized ASN item.
 * @param [in, out] buf   Buffer to encode into. May be NULL.
 * @return  Length of DER encoding on success.
 * @return  0 on failure.
 */
static int wolfssl_i2d_asn1_item(void** item, int type, byte* buf)
{
    int len;

    switch (type) {
        case WOLFSSL_X509_ALGOR_ASN1:
            len = wolfSSL_i2d_X509_ALGOR(*(const WOLFSSL_X509_ALGOR**)item,
                buf);
            break;
        case WOLFSSL_ASN1_BIT_STRING_ASN1:
            len = wolfSSL_i2d_ASN1_BIT_STRING(
                *(const WOLFSSL_ASN1_BIT_STRING**)item, buf);
            break;
        case WOLFSSL_ASN1_INTEGER_ASN1:
        {
            byte *tmp_buf = buf;
            len = wolfSSL_i2d_ASN1_INTEGER(
                *(const WOLFSSL_ASN1_INTEGER**)item, &tmp_buf);
            if ((buf == NULL) && (tmp_buf != NULL)) {
                XFREE(tmp_buf, NULL, DYNAMIC_TYPE_ASN1);
                tmp_buf = NULL;
            }
        }
        break;
        default:
            WOLFSSL_MSG("Type not support in processMembers");
            len = 0;
    }

    return len;
}

/* Encode members of an ASN.1 SEQUENCE as DER.
 *
 * @param [in]      src      ASN1 items to encode.
 * @param [in, out] buf      Buffer to encode into. May be NULL.
 * @param [in]      members  ASN1 template members.
 * @param [in]      mcount   Count of template members.
 * @return  Length of DER encoding on success.
 * @return  0 on failure.
 */
static int wolfssl_i2d_asn1_items(const void* src, byte*buf,
    const WOLFSSL_ASN1_TEMPLATE* members, size_t mcount)
{
    const WOLFSSL_ASN1_TEMPLATE* mem = NULL;
    int len = 0;
    int ret;
    size_t i;

    WOLFSSL_ENTER("wolfssl_i2d_asn1_items");

    for (mem = members, i = 0; i < mcount; mem++, i++) {
        ret = wolfssl_i2d_asn1_item((void**)(((byte*)src) + mem->offset),
            mem->type, bufLenOrNull(buf, len));
        if (ret == 0) {
            len = 0;
            break;
        }
        len += ret;
    }

    WOLFSSL_LEAVE("wolfssl_i2d_asn1_items", len);

    return len;
}

/* Encode sequence and items under it.
 *
 * @param [in]      src  ASN1 items to encode.
 * @param [in, out] buf  Buffer to encode into. May be NULL.
 * @param [in]      tpl  Template of ASN1 items.
 * @return  Length of DER encoding on success.
 * @return  0 on failure.
 */
static int i2d_ASN_SEQUENCE(const void* src, byte* buf,
    const WOLFSSL_ASN1_ITEM* tpl)
{
    word32 seq_len;
    word32 len = 0;

    seq_len = (word32)wolfssl_i2d_asn1_items(src, NULL, tpl->members,
        tpl->mcount);
    if (seq_len != 0) {
        len = SetSequence(seq_len, buf);
        if (buf != NULL) {
            wolfssl_i2d_asn1_items(src, buf + len, tpl->members, tpl->mcount);
        }
        len += seq_len;
    }

    return (int)len;
}

/* Encode ASN1 template item.
 *
 * @param [in]      src  ASN1 items to encode.
 * @param [in, out] buf  Buffer to encode into. May be NULL.
 * @param [in]      tpl  Template of ASN1 items.
 * @return  Length of DER encoding on success.
 * @return  0 on failure.
 */
static int wolfssl_asn1_item_encode(const void* src, byte* buf,
    const WOLFSSL_ASN1_ITEM* tpl)
{
    int len;

    switch (tpl->type) {
        case ASN_SEQUENCE:
            len = i2d_ASN_SEQUENCE(src, buf, tpl);
            break;
        default:
            WOLFSSL_MSG("Type not supported in wolfSSL_ASN1_item_i2d");
            len = 0;
    }

    return len;
}

/* Encode ASN1 template.
 *
 * @param [in]      src   ASN1 items to encode.
 * @param [in, out] dest  Pointer to buffer to encode into. May be NULL.
 * @param [in]      tpl   Template of ASN1 items.
 * @return  Length of DER encoding on success.
 * @return  0 on failure.
 */
int wolfSSL_ASN1_item_i2d(const void* src, byte** dest,
    const WOLFSSL_ASN1_ITEM* tpl)
{
    int ret = 1;
    int len = 0;
    byte* buf = NULL;

    WOLFSSL_ENTER("wolfSSL_ASN1_item_i2d");

    /* Validate parameters. */
    if ((src == NULL) || (tpl == NULL)) {
        ret = 0;
    }

    if ((ret == 1) && ((len = wolfssl_asn1_item_encode(src, NULL, tpl)) == 0)) {
        ret = 0;
    }

    if ((ret == 1) && (dest != NULL)) {
        if (*dest == NULL) {
            buf = (byte*)XMALLOC((size_t)len, NULL, DYNAMIC_TYPE_ASN1);
            if (buf == NULL)
                ret = 0;
            *dest = buf;
        }

        if (ret == 1) {
            len = wolfssl_asn1_item_encode(src, *dest, tpl);
        }
    }

    if (ret == 0) {
        XFREE(buf, NULL, DYNAMIC_TYPE_ASN1);
        len = 0;
    }
    WOLFSSL_LEAVE("wolfSSL_ASN1_item_i2d", len);
    return len;
}

#endif /* OPENSSL_ALL */

#endif /* OPENSSL_EXTRA */

/*******************************************************************************
 * ASN1_BIT_STRING APIs
 ******************************************************************************/

#if defined(OPENSSL_EXTRA) || defined(WOLFSSL_WPAS_SMALL)
/* Create a new ASN.1 BIT_STRING object.
 *
 * @return  ASN.1 BIT_STRING object on success.
 * @return  NULL when dynamic memory allocation fails.
 */
WOLFSSL_ASN1_BIT_STRING* wolfSSL_ASN1_BIT_STRING_new(void)
{
    WOLFSSL_ASN1_BIT_STRING* bitStr;

    bitStr = (WOLFSSL_ASN1_BIT_STRING*)XMALLOC(sizeof(WOLFSSL_ASN1_BIT_STRING),
        NULL, DYNAMIC_TYPE_OPENSSL);
    if (bitStr) {
        XMEMSET(bitStr, 0, sizeof(WOLFSSL_ASN1_BIT_STRING));
    }

    return bitStr;
}

/* Dispose of ASN.1 BIT_STRING object.
 *
 * Do not use bitStr after calling this function.
 *
 * @param [in, out] bitStr  ASN.1 BIT_STRING to free. May be NULL.
 */
void wolfSSL_ASN1_BIT_STRING_free(WOLFSSL_ASN1_BIT_STRING* bitStr)
{
    if (bitStr != NULL) {
        /* Dispose of any data allocated in BIT_STRING. */
        XFREE(bitStr->data, NULL, DYNAMIC_TYPE_OPENSSL);
    }
    /* Dispose of the ASN.1 BIT_STRING object. */
    XFREE(bitStr, NULL, DYNAMIC_TYPE_OPENSSL);
}

/* Get the value of the bit from the ASN.1 BIT_STRING at specified index.
 *
 * A NULL object a value of 0 for the bit at all indices.
 * A negative index has a value of 0 for the bit.
 *
 * @param [in] bitStr  ASN.1 BIT_STRING object.
 * @param [in] i       Index of bit.
 * @return  Value of bit.
 */
int wolfSSL_ASN1_BIT_STRING_get_bit(const WOLFSSL_ASN1_BIT_STRING* bitStr,
    int i)
{
    int bit = 0;

    /* Check for data and whether index is in range. */
    if ((bitStr != NULL) && (bitStr->data != NULL) && (i >= 0) &&
            (bitStr->length > (i / 8))) {
        bit = (bitStr->data[i / 8] & (1 << (7 - (i % 8)))) ? 1 : 0;
    }

    return bit;
}
#endif /* OPENSSL_EXTRA || WOLFSSL_WPAS_SMALL */

#if defined(OPENSSL_ALL) && !defined(NO_CERTS)

/* Grow data to require length.
 *
 * @param [in] bitStr  ASN.1 BIT_STRING object.
 * @param [in] len     Length, in bytes, of data required.
 * @return  1 on success.
 * @return  0 when dynamic memory allocation fails.
 */
static int wolfssl_asn1_bit_string_grow(WOLFSSL_ASN1_BIT_STRING* bitStr,
    int len)
{
    int ret = 1;
    byte* tmp;

    /* Realloc to length required. */
    tmp = (byte*)XREALLOC(bitStr->data, (size_t)len, NULL,
        DYNAMIC_TYPE_OPENSSL);
    if (tmp == NULL) {
        ret = 0;
    }
    else {
        /* Clear out new, top bytes. */
        XMEMSET(tmp + bitStr->length, 0, (size_t)(len - bitStr->length));
        bitStr->data = tmp;
        bitStr->length = len;
    }

    return ret;
}

/* Set the value of a bit in the ASN.1 BIT_STRING at specified index.
 *
 * @param [in] bitStr  ASN.1 BIT_STRING object.
 * @param [in] idx     Index of bit to set.
 * @param [in] val     Value of bit to set. Valid values: 0 or 1.
 * @return  1 on success.
 * @return  0 when bitStr is NULL, idx is negative, val is not 0 or 1, or
 *          dynamic memory allocation fails.
 */
int wolfSSL_ASN1_BIT_STRING_set_bit(WOLFSSL_ASN1_BIT_STRING* bitStr, int idx,
    int val)
{
    int ret = 1;
    int i = 0;

    /* Validate parameters. */
    if ((bitStr == NULL) || (idx < 0) || ((val != 0) && (val != 1))) {
        ret = 0;
    }

    if (ret == 1) {
        i = idx / 8;

        /* Check if we need to extend data range. */
        if ((i >= bitStr->length) && (val != 0)) {
            /* Realloc data to handle having bit set at index. */
            ret = wolfssl_asn1_bit_string_grow(bitStr, i + 1);
        }
    }
    if ((ret == 1) && (i < bitStr->length)) {
        /* Bit on at index. */
        byte bit = 1 << (7 - (idx % 8));

        /* Clear bit and set to value. */
        bitStr->data[i] &= ~bit;
        bitStr->data[i] |= bit & (byte)(0 - val);
    }

    return ret;
}

#endif /* OPENSSL_ALL && !NO_CERTS */

/*******************************************************************************
 * ASN1_INTEGER APIs
 ******************************************************************************/

#if defined(OPENSSL_EXTRA) || defined(WOLFSSL_WPAS_SMALL)
/* Create a new empty ASN.1 INTEGER object.
 *
 * @return  ASN.1 INTEGER object on success.
 * @return  NULL when dynamic memory allocation fails.
 */
WOLFSSL_ASN1_INTEGER* wolfSSL_ASN1_INTEGER_new(void)
{
    WOLFSSL_ASN1_INTEGER* a;

    /* Allocate a new ASN.1 INTEGER object. */
    a = (WOLFSSL_ASN1_INTEGER*)XMALLOC(sizeof(WOLFSSL_ASN1_INTEGER), NULL,
        DYNAMIC_TYPE_OPENSSL);
    if (a != NULL) {
        XMEMSET(a, 0, sizeof(WOLFSSL_ASN1_INTEGER));
        /* Use fixed buffer field for data. */
        a->data      = a->intData;
        a->isDynamic = 0;
        /* Maximum supported by fixed buffer. */
        a->dataMax   = WOLFSSL_ASN1_INTEGER_MAX;
        /* No value set - no data. */
        a->length    = 0;
    }

    return a;
}

/* Free the ASN.1 INTEGER object and any dynamically allocated data.
 *
 * @param [in, out] in  ASN.1 INTEGER object.
 */
void wolfSSL_ASN1_INTEGER_free(WOLFSSL_ASN1_INTEGER* in)
{
    if ((in != NULL) && (in->isDynamic)) {
        /* Dispose of any data allocated in INTEGER. */
        XFREE(in->data, NULL, DYNAMIC_TYPE_OPENSSL);
    }
    /* Dispose of the ASN.1 INTEGER object. */
    XFREE(in, NULL, DYNAMIC_TYPE_OPENSSL);
}

#if defined(OPENSSL_EXTRA)
/* Reset the data of ASN.1 INTEGER object back to empty fixed array.
 *
 * @param [in] a  ASN.1 INTEGER object.
 */
static void wolfssl_asn1_integer_reset_data(WOLFSSL_ASN1_INTEGER* a)
{
    /* Don't use dynamic buffer anymore. */
    if (a->isDynamic) {
        /* Cache pointer to allocated data. */
        unsigned char* data = a->data;
        /* No longer dynamic. */
        a->isDynamic = 0;
        /* Point data at fixed array. */
        a->data = a->intData;
        /* Set maximum length to fixed array size. */
        a->dataMax = (unsigned int)sizeof(a->intData);
        /* Dispose of dynamically allocated data. */
        XFREE(data, NULL, DYNAMIC_TYPE_OPENSSL);
    }
    /* Clear out data from fixed array. */
    XMEMSET(a->intData, 0, sizeof(a->intData));
    /* No data, no length. */
    a->length = 0;
    /* No data, not negative. */
    a->negative = 0;
    /* Set type to positive INTEGER. */
    a->type = V_ASN1_INTEGER;
}
#endif /* OPENSSL_EXTRA */

/* Setup ASN.1 INTEGER object to handle data of required length.
 *
 * @param [in, out] a    ASN.1 INTEGER object.
 * @param [in]      len  Required length in bytes.
 * @return  1 on success.
 * @return  0 on dynamic memory allocation failure.
 */
static int wolfssl_asn1_integer_require_len(WOLFSSL_ASN1_INTEGER* a, int len,
    int keepOldData)
{
    int ret = 1;
    byte* data;
    byte* oldData = a->intData;
    int oldLen = a->length;

    if (a->isDynamic && (len > (int)a->dataMax)) {
        oldData = a->data;
        a->isDynamic = 0;
        a->data = a->intData;
        a->dataMax = (unsigned int)sizeof(a->intData);
    }
    a->length = 0;
    if ((!a->isDynamic) && (len > (int)a->dataMax)) {
        /* Create a new buffer to hold large integer value. */
        data = (byte*)XMALLOC((size_t)len, NULL, DYNAMIC_TYPE_OPENSSL);
        if (data == NULL) {
            ret = 0;
        }
        else {
            /* Indicate data is dynamic and copy data over. */
            a->isDynamic = 1;
            a->data = data;
            a->dataMax = (word32)len;
        }
    }
    if (keepOldData) {
         if (oldData != a->data) {
             /* Copy old data into new buffer. */
             XMEMCPY(a->data, oldData, (size_t)oldLen);
         }
         /* Restore old length. */
         a->length = oldLen;
    }
    if (oldData != a->intData) {
         /* Dispose of the old dynamic data. */
         XFREE(oldData, NULL, DYNAMIC_TYPE_OPENSSL);
    }

    return ret;
}

/* Duplicate the ASN.1 INTEGER object into a newly allocated one.
 *
 * @param [in] src  ASN.1 INTEGER object to copy.
 * @return  ASN.1 INTEGER object on success.
 * @return  NULL when src is NULL or dynamic memory allocation fails.
 */
WOLFSSL_ASN1_INTEGER* wolfSSL_ASN1_INTEGER_dup(const WOLFSSL_ASN1_INTEGER* src)
{
    WOLFSSL_ASN1_INTEGER* dup = NULL;

    WOLFSSL_ENTER("wolfSSL_ASN1_INTEGER_dup");

    /* Check for object to duplicate. */
    if (src != NULL) {
        /* Create a new ASN.1 INTEGER object to be copied into. */
        dup = wolfSSL_ASN1_INTEGER_new();
    }
    /* Check for object to copy into. */
    if (dup != NULL) {
        /* Copy simple fields. */
        dup->length   = src->length;
        dup->negative = src->negative;
        dup->type     = src->type;

        if (!src->isDynamic) {
            /* Copy over data from/to fixed buffer. */
            XMEMCPY(dup->intData, src->intData, WOLFSSL_ASN1_INTEGER_MAX);
        }
        else if (wolfssl_asn1_integer_require_len(dup, src->length, 0) == 0) {
            wolfSSL_ASN1_INTEGER_free(dup);
            dup = NULL;
        }
        else {
            XMEMCPY(dup->data, src->data, (size_t)src->length);
        }
    }

    return dup;
}
#endif /* OPENSSL_EXTRA || WOLFSSL_WPAS_SMALL */

#if defined(OPENSSL_EXTRA)

/* Compare values in two ASN.1 INTEGER objects.
 *
 * @param [in] a  First ASN.1 INTEGER object.
 * @param [in] b  Second ASN.1 INTEGER object.
 * @return Negative value when a is less than b.
 * @return 0 when a equals b.
 * @return Positive value when a is greater than b.
 * @return -1 when a or b is NULL.
 */
int wolfSSL_ASN1_INTEGER_cmp(const WOLFSSL_ASN1_INTEGER* a,
    const WOLFSSL_ASN1_INTEGER* b)
{
    int ret = 0;

    WOLFSSL_ENTER("wolfSSL_ASN1_INTEGER_cmp");

    /* Validate parameters. */
    if ((a == NULL) || (b == NULL)) {
        WOLFSSL_MSG("Bad parameter.");
        ret = -1;
    }
    /* Negative value < Positive value */
    else if (a->negative && !b->negative) {
        ret = -1;
    }
    /* Positive value > Negative value */
    else if (!a->negative && b->negative) {
        ret = 1;
    }
    else {
        /* Check for difference in length. */
        if (a->length != b->length) {
            ret = a->length - b->length;
        }
        else {
            /* Compare data given they are the same length. */
            ret = XMEMCMP(a->data, b->data, (size_t)a->length);
        }
        /* Reverse comparison result when both negative. */
        if (a->negative) {
            ret = -ret;
        }
    }

    WOLFSSL_LEAVE("wolfSSL_ASN1_INTEGER_cmp", ret);

    return ret;
}


/* Calculate 2's complement of DER encoding.
 *
 * @param [in]  data    Array that is number.
 * @param [in]  length  Number of bytes in array.
 * @return  0 on success.
 * @return  -1 when get length from DER header failed.
 */
static void wolfssl_twos_compl(byte* data, int length)
{
    int i;

    /* Invert bits - 1's complement. */
    for (i = 0; i < length; ++i) {
        data[i] = ~data[i];
    }
    /* 2's complement - add 1. */
    for (i = length - 1; (++data[i]) == 0; --i) {
        /* Do nothing. */
    }
}

/* Calculate 2's complement of DER encoding.
 *
 * @param [in]  data    Array that is number.
 * @param [in]  length  Number of bytes in array.
 * @param [out] neg     When NULL, 2's complement data.
 *                      When not NULL, check for negative first and return.
 * @return  0 on success.
 * @return  -1 when get length from DER header failed.
 */
static int wolfssl_asn1_int_twos_compl(byte* data, int length, byte* neg)
{
    int ret = 0;
    word32 idx = 1;     /* Skip tag. */
    int len;

    /* Get length from DER header. */
    if (GetLength(data, &idx, &len, (word32)length) < 0) {
        ret = -1;
    }
    else {
        if (neg != NULL) {
            *neg = data[idx] & 0x80;
        }
        if ((neg == NULL) || (*neg != 0)) {
            wolfssl_twos_compl(data + idx, length - (int)idx);
        }
    }

    return ret;
}

/* Encode ASN.1 INTEGER as DER without tag.
 *
 * When out points to NULL, a new buffer is allocated and returned.
 *
 * @param [in]      a    ASN.1 INTEGER object.
 * @param [in, out] out  Pointer to buffer to hold encoding. May point to NULL.
 * @return  Length of encoding on success.
 * @return  -1 when a is NULL or no data, out is NULL, dynamic memory allocation
 *          fails or encoding length fails.
 */
int wolfSSL_i2d_ASN1_INTEGER(const WOLFSSL_ASN1_INTEGER* a, unsigned char** out)
{
    int ret = 0;
    byte* buf = NULL;

    WOLFSSL_ENTER("wolfSSL_i2d_ASN1_INTEGER");

    /* Validate parameters. */
    if ((a == NULL) || (a->data == NULL) || (a->length <= 0) || (out == NULL)) {
        WOLFSSL_MSG("Bad parameter.");
        ret = -1;
    }

    if ((ret == 0) && (*out == NULL)) {
        /* Allocate buffer to hold encoding. */
        buf = (unsigned char*)XMALLOC((size_t)a->length, NULL,
            DYNAMIC_TYPE_ASN1);
        if (buf == NULL) {
            WOLFSSL_MSG("Failed to allocate output buffer.");
            ret = -1;
        }
        /* Return any allocated buffer. */
        *out = buf;
    }
    if (ret == 0) {
        /* Copy the data (including tag and length) into output buffer. */
        XMEMCPY(*out, a->data, (size_t)a->length);
        /* Only magnitude of the number stored (i.e. the sign isn't encoded).
         * The "negative" field is 1 if the value must be interpreted as
         * negative and we need to output the 2's complement of the value in
         * the DER output.
         */
        if (a->negative) {
            ret = wolfssl_asn1_int_twos_compl(*out, a->length, NULL);
        }
    }
    if (ret == 0) {
        ret = a->length;
        /* Move pointer on passed encoding when buffer passed in. */
        if (buf == NULL) {
            *out += a->length;
        }
    }
    /* Dispose of any dynamically allocated data on error. */
    else if (buf != NULL) {
        /* Dispose of buffer allocated locally on error. */
        XFREE(buf, NULL, DYNAMIC_TYPE_ASN1);
        /* Don't return freed buffer. */
        *out = NULL;
    }

    WOLFSSL_LEAVE("wolfSSL_i2d_ASN1_INTEGER", ret);

    return ret;
}

/* Decode DER encoding of ASN.1 INTEGER.
 *
 * @param [out]     a     ASN.1 INTEGER object. May be NULL.
 * @param [in, out] in    Pointer to buffer containing DER encoding.
 * @param [in]      inSz  Length of data in buffer.
 * @return  ASN.1 INTEGER object on success.
 * @return  NULL when in or *in is NULL, inSz is less than or equal to 2 or
 *          parsing DER failed.
 */
WOLFSSL_ASN1_INTEGER* wolfSSL_d2i_ASN1_INTEGER(WOLFSSL_ASN1_INTEGER** a,
    const unsigned char** in, long inSz)
{
    WOLFSSL_ASN1_INTEGER* ret = NULL;
    int err = 0;
    word32 idx = 1;
    int len = 0;

    WOLFSSL_ENTER("wolfSSL_d2i_ASN1_INTEGER");

    /* Validate parameters. */
    if ((in == NULL) || (*in == NULL) || (inSz <= 2)) {
        WOLFSSL_MSG("Bad parameter");
        err = 1;
    }

    /* Check that the tag is correct. */
    if ((!err) && (*in)[0] != ASN_INTEGER) {
        WOLFSSL_MSG("Tag doesn't indicate integer type.");
        err = 1;
    }
    /* Check that length and use this instead of inSz. */
    if ((!err) && (GetLength(*in, &idx, &len, (word32)inSz) <= 0)) {
        WOLFSSL_MSG("ASN.1 length not valid.");
        err = 1;
    }
    /* Allocate a new ASN.1 INTEGER object. */
    if ((!err) && ((ret = wolfSSL_ASN1_INTEGER_new()) == NULL)) {
        err = 1;
    }
    if ((!err) && (wolfssl_asn1_integer_require_len(ret, (int)idx + len, 0) !=
            1)) {
        err = 1;
    }
    if (!err) {
        /* Set type. */
        ret->type = V_ASN1_INTEGER;

        /* Copy DER encoding and length. */
        XMEMCPY(ret->data, *in, (size_t)(idx + (word32)len));
        ret->length = (int)idx + len;
        /* Do 2's complement if number is negative. */
        if (wolfssl_asn1_int_twos_compl(ret->data, ret->length, &ret->negative)
                != 0) {
            err = 1;
        }
    }
    if ((!err) && ret->negative) {
        /* Update type if number was negative. */
        ret->type |= V_ASN1_NEG_INTEGER;
    }

    if (err) {
        /* Dispose of dynamically allocated data on error. */
        wolfSSL_ASN1_INTEGER_free(ret);
        ret = NULL;
    }
    else {
        if (a != NULL) {
            /* Return ASN.1 INTEGER through a. */
            *a = ret;
        }
        *in += ret->length;
    }

    return ret;
}

#ifndef NO_BIO

/* Get length of leading hexadecimal characters.
 *
 * Looks for continuation character before carriage returns and line feeds.
 *
 * @param [in]  str   String with input.
 * @param [in]  len   Length of string.
 * @param [out] cont  Line continuation character at end of line before
 *                    carriage returns and line feeds.
 * @return  Number of leading hexadecimal characters in string.
 */
static int wolfssl_a2i_asn1_integer_clear_to_eol(char* str, int len, int* cont)
{
    byte num;
    word32 nLen;
    int i;

    /* Strip off trailing carriage returns and line feeds. */
    while ((len > 0) && ((str[len - 1] == '\n') || (str[len - 1] == '\r'))) {
        len--;
    }
    /* Check for line continuation character. */
    if ((len > 0) && (str[len - 1] == '\\')) {
        *cont = 1;
        len--;
    }
    else {
        *cont = 0;
    }

    /* Find end of hexadecimal characters. */
    nLen = 1;
    for (i = 0; i < len; i++) {
        /* Check if character is a hexadecimal character. */
        if (Base16_Decode((const byte*)str + i, 1, &num, &nLen) == ASN_INPUT_E)
        {
            /* Found end of hexadecimal characters, return count. */
            len = i;
            break;
        }
    }

    return len;
}

/* Read number from BIO as a string.
 *
 * Line continuation character at end of line means next line must be read.
 *
 * @param [in]      bio   BIO to read from.
 * @param [in]      asn1  ASN.1 INTEGER object to put number into.
 * @param [in, out] buf   Buffer to use when reading.
 * @param [in]      size  Length of buffer in bytes.
 * @return  1 on success.
 * @return  0 on failure.
 */
int wolfSSL_a2i_ASN1_INTEGER(WOLFSSL_BIO *bio, WOLFSSL_ASN1_INTEGER *asn1,
    char *buf, int size)
{
    int ret = 1;
    int readNextLine = 1;
    int len;
    word32 outLen = 0;
    const int hdrSz = 1 + MAX_LENGTH_SZ;

    WOLFSSL_ENTER("wolfSSL_a2i_ASN1_INTEGER");

    if ((bio == NULL) || (asn1 == NULL) || (buf == NULL) || (size <= 0)) {
        WOLFSSL_MSG("Bad parameter");
        ret = 0;
    }

    while ((ret == 1) && readNextLine) {
        int lineLen;

        /* Assume we won't be reading any more. */
        readNextLine = 0;

        /* Read a line. */
        lineLen = wolfSSL_BIO_gets(bio, buf, size);
        if (lineLen <= 0) {
            WOLFSSL_MSG("wolfSSL_BIO_gets error");
            ret = 0;
        }

        if (ret == 1) {
            /* Find length of hexadecimal digits in string. */
            lineLen = wolfssl_a2i_asn1_integer_clear_to_eol(buf, lineLen,
                &readNextLine);
            /* Check we have a valid hexadecimal string to process. */
            if ((lineLen == 0) || ((lineLen % 2) != 0)) {
                WOLFSSL_MSG("Invalid line length");
                ret = 0;
            }
        }
        if (ret == 1) {
            /* Calculate length of complete number so far. */
            len = asn1->length + (lineLen / 2);
            /* Make sure enough space for number and maximum header. */
            if (wolfssl_asn1_integer_require_len(asn1, len + hdrSz, outLen != 0)
                    != 1) {
                ret = 0;
            }
        }
        if (ret == 1) {
            /* Decode string and append to data. */
            outLen = (word32)(lineLen / 2);
            (void)Base16_Decode((byte*)buf, (word32)lineLen,
                asn1->data + asn1->length, &outLen);
            /* Update length of data. */
            asn1->length += (int)outLen;
        }
    }

    if (ret == 1) {
        int idx;

        /* Get ASN.1 header length. */
        idx = SetASNInt(asn1->length, asn1->data[0], NULL);
        /* Move data to be after ASN.1 header. */
        XMEMMOVE(asn1->data + idx, asn1->data, (size_t)asn1->length);
        /* Encode ASN.1 header. */
        SetASNInt(asn1->length, asn1->data[idx], asn1->data);
        /* Update length of data. */
        asn1->length += idx;
    }

    return ret;
}

/* Write out number in ASN.1 INTEGER object to BIO as string.
 *
 * @param [in] bp  BIO to write to.
 * @param [in] a   ASN.1 INTEGER object.
 * @return  Number of characters written on success.
 * @return  0 when bp or a is NULL.
 * @return  0 DER header in data is invalid.
 */
int wolfSSL_i2a_ASN1_INTEGER(BIO *bp, const WOLFSSL_ASN1_INTEGER *a)
{
    int err = 0;
    word32 idx = 1;     /* Skip ASN.1 INTEGER tag byte. */
    int len = 0;
    byte buf[WOLFSSL_ASN1_INTEGER_MAX * 2 + 1];
    word32 bufLen;

    WOLFSSL_ENTER("wolfSSL_i2a_ASN1_INTEGER");

    /* Validate parameters. */
    if ((bp == NULL) || (a == NULL)) {
         err = 1;
    }

    if (!err) {
        /* Read DER length - must be at least 1 byte. */
        if (GetLength(a->data, &idx, &len, (word32)a->length) <= 0) {
            err = 1;
        }
    }

    /* Keep encoding and writing while no error and bytes in data. */
    while ((!err) && (idx < (word32)a->length)) {
        /* Number of bytes left to encode. */
        int encLen = a->length - (int)idx;
        /* Reduce to maximum buffer size if necessary. */
        if (encLen > (int)sizeof(buf) / 2) {
            encLen = (int)sizeof(buf) / 2;
        }

        /* Encode bytes from data into buffer. */
        bufLen = (int)sizeof(buf);
        (void)Base16_Encode(a->data + idx, (word32)encLen, buf, &bufLen);
        /* Update index to next bytes to encoded. */
        idx += (word32)encLen;

        /* Write out characters but not NUL char. */
        if (wolfSSL_BIO_write(bp, buf, (int)bufLen - 1) != (int)(bufLen - 1)) {
            err = 1;
        }
    }

    if (err) {
        /* Return 0 on error. */
        len = 0;
    }
    /* Return total number of characters written. */
    return len * 2;
}
#endif /* !NO_BIO */

#ifndef NO_ASN
/* Determine if a pad byte is required and its value for a number.
 *
 * Assumes values pointed to by pad and padVal are both 0.
 *
 * @param [in]       data    Number encoded as big-endian bytes.
 * @param [in]       len     Length of number in bytes.
 * @param [in, out]  neg     Indicates number is negative.
 * @param [out]      pad     Number of padding bytes required.
 * @param [out]      padVal  Padding byte to prepend.
 */
static void wolfssl_asn1_integer_pad(unsigned char* data, int len,
    unsigned char* neg, char* pad, unsigned char* padVal)
{
    /* Check for empty data. */
    if (len == 0) {
        *pad = 1;
        *padVal = 0x00;
        *neg = 0;
    }
    else {
        /* Get first, most significant, byte of encoded number. */
        unsigned char firstByte = data[0];

        /* 0 can't be negative. */
        if ((len == 1) && (firstByte == 0x00)) {
            *neg = 0;
        }
        /* Positive value must not have top bit of first byte set. */
        if ((!*neg) && (firstByte >= 0x80)) {
            *pad = 1;
            *padVal = 0x00;
        }
        /* Negative numbers are two's complemented.
         * Two's complement value must have top bit set.
         */
        else if (*neg && (firstByte > 0x80)) {
            *pad = 1;
            *padVal = 0xff;
        }
        /* Checking for: 0x80[00]*
         * when negative that when two's complemented will be: 0x80[00]*
         * and therefore doesn't require pad byte.
         */
        else if (*neg && (firstByte == 0x80)) {
            int i;
            /* Check rest of bytes. */
            for (i = 1; i < len; i++) {
                if (data[i] != 0x00) {
                    /* Not 0x80[00]* */
                    *pad = 1;
                    *padVal = 0xff;
                    break;
                }
            }
        }
    }
}

/* Convert ASN.1 INTEGER object into content octets.
 *
 * TODO: compatibility with OpenSSL? OpenSSL assumes data not DER encoded.
 *
 * When pp points to a buffer, on success pp will point to after the encoded
 * data.
 *
 * @param [in]      a   ASN.1 INTEGER object.
 * @param [in, out] pp  Pointer to buffer. May be NULL. Cannot point to NULL.
 * @return  Length of encoding on success.
 * @return  0 when a is NULL, pp points to NULL or DER length encoding invalid.
 */
int wolfSSL_i2c_ASN1_INTEGER(WOLFSSL_ASN1_INTEGER *a, unsigned char **pp)
{
    int err = 0;
    int len = 0;
    char pad = 0;
    unsigned char padVal = 0;
    word32 idx = 1;

    WOLFSSL_ENTER("wolfSSL_i2c_ASN1_INTEGER");

    /* Validate parameters. */
    if ((a == NULL) || ((pp != NULL) && (*pp == NULL))) {
        err = 1;
    }

    /* Get length from DER encoding. */
    if ((!err) && (GetLength_ex(a->data, &idx, &len, a->dataMax, 0) < 0)) {
        err = 1;
    }

    if (!err) {
        /* Determine pad length and value. */
        wolfssl_asn1_integer_pad(a->data + idx, len, &a->negative, &pad,
            &padVal);
        /* Total encoded length is number length plus one when padding. */
        len += (int)pad;
    }

    /* Check buffer supplied to write into. */
    if ((!err) && (pp != NULL)) {
        /* Put in any pad byte. */
        if (pad) {
            (*pp)[0] = padVal;
        }
        /* Copy remaining bytes into output buffer. */
        XMEMCPY(*pp + pad, a->data + idx, (size_t)(len - pad));
        /* Two's complement copied bytes when negative. */
        if (a->negative) {
            wolfssl_twos_compl(*pp + pad, len - pad);
        }
        /* Move pointer past encoded data. */
        *pp += len;
    }

    return len;
}

/* Make a big number with the value in the ASN.1 INTEGER object.
 *
 * A new big number object is allocated when bn is NULL.
 *
 * @param [in] ai  ASN.1 INTEGER object.
 * @param [in] bn  Big number object. May be NULL.
 * @return  Big number object on success.
 * @return  NULL when ai is NULL or converting from binary fails.
 */
WOLFSSL_BIGNUM *wolfSSL_ASN1_INTEGER_to_BN(const WOLFSSL_ASN1_INTEGER *ai,
    WOLFSSL_BIGNUM *bn)
{
    int err = 0;
    word32 idx = 1;
    int len = 0;
    WOLFSSL_BIGNUM* ret = NULL;

    WOLFSSL_ENTER("wolfSSL_ASN1_INTEGER_to_BN");

    /* Validate parameters. */
    if (ai == NULL) {
        err = 1;
    }

    if (!err) {
        /* Get the length of ASN.1 INTEGER number. */
        if ((ai->data[0] != ASN_INTEGER) || (GetLength(ai->data, &idx, &len,
                (word32)ai->length) <= 0)) {
        #if defined(WOLFSSL_QT) || defined(WOLFSSL_HAPROXY)
            idx = 0;
            len = ai->length;
        #else
            WOLFSSL_MSG("Data in WOLFSSL_ASN1_INTEGER not DER encoded");
            err = 1;
        #endif
        }
    }
    if (!err) {
        /* Convert binary to big number. */
        ret = wolfSSL_BN_bin2bn(ai->data + idx, len, bn);
        if (ret != NULL) {
            /* Handle negative. */
            (void)wolfssl_bn_set_neg(ret, ai->negative);
        }
    }

    return ret;
}
#endif /* !NO_ASN */

/* Create an ASN.1 INTEGER object from big number.
 *
 * Allocates a new ASN.1 INTEGER object when ai is NULL.
 *
 * @param [in] bn  Big number to encode.
 * @param [in] ai  ASN.1 INTEGER object. May be NULL.
 * @return  ASN.1 INTEGER object on success.
 * @return  NULL when dynamic memory allocation fails.
 */
WOLFSSL_ASN1_INTEGER* wolfSSL_BN_to_ASN1_INTEGER(const WOLFSSL_BIGNUM *bn,
    WOLFSSL_ASN1_INTEGER *ai)
{
    int err = 0;
    WOLFSSL_ASN1_INTEGER* a = NULL;
    int len = 0;
    int numBits = 0;
    byte firstByte = 0;

    WOLFSSL_ENTER("wolfSSL_BN_to_ASN1_INTEGER");

    /* Validate parameters. */
    if (bn == NULL) {
        err = 1;
    }
    /* Use ASN.1 INTEGER object if provided. */
    else if (ai != NULL) {
        a = ai;
    }
    /* Create an ASN.1 INTEGER object to return. */
    else {
        a = wolfSSL_ASN1_INTEGER_new();
        if (a == NULL) {
            err = 1;
        }
    }

    /* Check we have an ASN.1 INTEGER object to set. */
    if (!err) {
        int length;

        /* Set type and negative. */
        a->type = V_ASN1_INTEGER;
        if (wolfSSL_BN_is_negative(bn) && !wolfSSL_BN_is_zero(bn)) {
            a->negative = 1;
            a->type |= V_ASN1_NEG_INTEGER;
        }

        /* Get length in bytes of encoded number. */
        len = wolfSSL_BN_num_bytes(bn);
        if (len == 0) {
            len = 1;
        }
        /* Get length in bits of encoded number. */
        numBits = wolfSSL_BN_num_bits(bn);
        /* Leading zero required if most-significant byte has top bit set. */
        if ((numBits > 0) && (numBits % 8) == 0) {
            firstByte = 0x80;
        }
        /* Get length of header based on length of number. */
        length = SetASNInt(len, firstByte, NULL);
        /* Add number of bytes to encode number. */
        length += len;

        /* Update data field to handle length. */
        if (wolfssl_asn1_integer_require_len(a, length, 0) != 1) {
            err = 1;
        }
    }
    if (!err) {
        /* Write ASN.1 header. */
        int idx = SetASNInt(len, firstByte, a->data);

        /* Populate data. */
        if (numBits == 0) {
            a->data[idx] = 0;
        }
        else {
            /* Add encoded number. */
            len = wolfSSL_BN_bn2bin(bn, a->data + idx);
            if (len < 0) {
                err = 1;
            }
        }

        /* Set length to encoded length. */
        a->length = idx + len;
    }

    if (err) {
        /* Can't use ASN.1 INTEGER object. */
        if (a != ai) {
            wolfSSL_ASN1_INTEGER_free(a);
        }
        a = NULL;
    }
    return a;
}

/* Get the value of the ASN.1 INTEGER as a long.
 *
 * Returning 0 on NULL and -1 on error is consistent with OpenSSL.
 *
 * @param [in] a  ASN.1 INTEGER object.
 * @return  Value as a long.
 * @return  0 when a is NULL.
 * @return  -1 when a big number operation fails.
 */
long wolfSSL_ASN1_INTEGER_get(const WOLFSSL_ASN1_INTEGER* a)
{
    long ret = 1;
    WOLFSSL_BIGNUM* bn = NULL;

    WOLFSSL_ENTER("wolfSSL_ASN1_INTEGER_get");

    /* Validate parameter. */
    if (a == NULL) {
        ret = 0;
    }

    if (ret > 0) {
        /* Create a big number from the DER encoding. */
        bn = wolfSSL_ASN1_INTEGER_to_BN(a, NULL);
        if (bn == NULL) {
            ret = -1;
        }
    }
    if (ret > 0) {
        /* Get the big number as a word. */
        ret = (long)wolfSSL_BN_get_word(bn);
        /* Negate number of ASN.1 INTEGER was negative. */
        if (a->negative == 1) {
            ret = -ret;
        }
    }

    /* Dispose of big number as no longer needed. */
    if (bn != NULL) {
        wolfSSL_BN_free(bn);
    }

    WOLFSSL_LEAVE("wolfSSL_ASN1_INTEGER_get", (int)ret);

    return ret;
}


/* Sets the value of the ASN.1 INTEGER object to the long value.
 *
 * @param [in, out] a  ASN.1 INTEGER object.
 * @param [in]      v  Value to set.
 * @return  1 on success.
 * @return  0 when a is NULL.
 */
int wolfSSL_ASN1_INTEGER_set(WOLFSSL_ASN1_INTEGER *a, long v)
{
    int ret = 1;

    /* Validate parameters. */
    if (a == NULL) {
        ret = 0;
    }
    if (ret == 1) {
        byte j;
        unsigned int i = 0;
        byte tmp[sizeof(long)];
        byte pad = 0;

        wolfssl_asn1_integer_reset_data(a);

        /* Check for negative. */
        if (v < 0) {
            /* Set negative and 2's complement the value. */
            a->negative = 1;
            a->type |= V_ASN1_NEG;
            v = -v;
        }

        /* Put value into temporary buffer - at least one byte encoded. */
        tmp[0] = (byte)(v & 0xff);
        v >>= 8;
        for (j = 1; j < (byte)sizeof(long); j++) {
            if (v == 0) {
                break;
            }
            tmp[j] = (byte)(v & 0xff);
            v >>= 8;
        }
        /* Pad with 0x00 to indicate positive number when top bit set. */
        if ((!a->negative) && (tmp[j-1] & 0x80)) {
            pad = 1;
        }

        /* Set tag. */
        a->data[i++] = ASN_INTEGER;
        /* Set length of encoded value. */
        a->data[i++] = pad + j;
        /* Set length of DER encoding. +2 for tag and length */
        a->length = 2 + pad + j;

        /* Add pad byte if required. */
        if (pad == 1) {
            a->data[i++] = 0;
        }
        /* Copy in data. */
        for (; j > 0; j--) {
            a->data[i++] = tmp[j-1];
        }
    }

    return ret;
}

#endif /* OPENSSL_EXTRA */

/*******************************************************************************
 * ASN1_OBJECT APIs
 ******************************************************************************/

#if !defined(NO_ASN)
#if defined(OPENSSL_EXTRA) || defined(OPENSSL_EXTRA_X509_SMALL)
/* Create a new ASN.1 OBJECT_ID object.
 *
 * @return  ASN.1 OBJECT_ID object on success.
 * @return  NULL when dynamic memory allocation fails.
 */
WOLFSSL_ASN1_OBJECT* wolfSSL_ASN1_OBJECT_new(void)
{
    WOLFSSL_ASN1_OBJECT* obj;

    /* Allocate memory for new ASN.1 OBJECT. */
    obj = (WOLFSSL_ASN1_OBJECT*)XMALLOC(sizeof(WOLFSSL_ASN1_OBJECT), NULL,
        DYNAMIC_TYPE_ASN1);
    if (obj != NULL) {
        XMEMSET(obj, 0, sizeof(WOLFSSL_ASN1_OBJECT));
        /* Setup pointers. */
        obj->d.ia5 = &(obj->d.ia5_internal);
    #if defined(OPENSSL_ALL)
        obj->d.iPAddress = &(obj->d.iPAddress_internal);
    #endif
        /* Object was allocated. */
        obj->dynamic |= WOLFSSL_ASN1_DYNAMIC;
    }

    return obj;
}

/* Dispose of any ASN.1 OBJECT_ID dynamically allocated data.
 *
 * Do not use obj after calling this function.
 *
 * @param [in, out] obj  ASN.1 OBJECT_ID object.
 */
void wolfSSL_ASN1_OBJECT_free(WOLFSSL_ASN1_OBJECT* obj)
{
    if (obj != NULL) {
        /* Check for dynamically allocated copy of encoded data. */
        if ((obj->dynamic & WOLFSSL_ASN1_DYNAMIC_DATA) != 0) {
        #ifdef WOLFSSL_DEBUG_OPENSSL
            WOLFSSL_MSG("Freeing ASN1 data");
        #endif
            XFREE((void*)obj->obj, obj->heap, DYNAMIC_TYPE_ASN1);
            obj->obj = NULL;
        }
    #if defined(OPENSSL_EXTRA)
        /* Check for path length ASN.1 INTEGER - X.509 extension. */
        if (obj->pathlen != NULL) {
            wolfSSL_ASN1_INTEGER_free(obj->pathlen);
            obj->pathlen = NULL;
        }
    #endif
        /* Check whether object was dynamically allocated. */
        if ((obj->dynamic & WOLFSSL_ASN1_DYNAMIC) != 0) {
    #ifdef WOLFSSL_DEBUG_OPENSSL
            WOLFSSL_MSG("Freeing ASN1 OBJECT");
    #endif
            XFREE(obj, NULL, DYNAMIC_TYPE_ASN1);
        }
    }
}

/* Duplicate the ASN.1 OBJECT_ID object.
 *
 * @param [in] obj  ASN.1 OBJECT_ID object to copy.
 * @return  New ASN.1 OBJECT_ID object on success.
 * @return  NULL when obj is NULL or dynamic memory allocation fails.
 */
WOLFSSL_ASN1_OBJECT* wolfSSL_ASN1_OBJECT_dup(WOLFSSL_ASN1_OBJECT* obj)
{
    WOLFSSL_ASN1_OBJECT* dupl = NULL;

    WOLFSSL_ENTER("wolfSSL_ASN1_OBJECT_dup");

    /* Validate parameter. */
    if (obj == NULL) {
        WOLFSSL_MSG("Bad parameter");
    }
    /* Create a new ASN.1 OBJECT_ID object to return. */
    else if ((dupl = wolfSSL_ASN1_OBJECT_new()) == NULL) {
        WOLFSSL_MSG("wolfSSL_ASN1_OBJECT_new error");
    }
    if (dupl != NULL) {
        /* Copy short name. */
        XMEMCPY(dupl->sName, obj->sName, WOLFSSL_MAX_SNAME);
        /* Copy simple fields. */
        dupl->type  = obj->type;
        dupl->grp   = obj->grp;
        dupl->nid   = obj->nid;
        dupl->objSz = obj->objSz;
    #ifdef OPENSSL_EXTRA
        dupl->ca    = obj->ca;
    #endif
        /* Check for encoding. */
        if (obj->obj) {
            /* Allocate memory for ASN.1 OBJECT_ID DER encoding. */
            dupl->obj = (const unsigned char*)XMALLOC(obj->objSz, NULL,
                DYNAMIC_TYPE_ASN1);
            if (dupl->obj == NULL) {
                WOLFSSL_MSG("ASN1 obj malloc error");
                wolfSSL_ASN1_OBJECT_free(dupl);
                dupl = NULL;
            }
            else {
                /* Encoding buffer was dynamically allocated. */
                dupl->dynamic |= WOLFSSL_ASN1_DYNAMIC_DATA;
                /* Copy DER encoding. */
                XMEMCPY((byte*)dupl->obj, obj->obj, obj->objSz);
            }
        }
    }

    return dupl;
}
#endif /* OPENSSL_EXTRA || OPENSSL_EXTRA_X509_SMALL */
#endif /* !NO_ASN */

#if defined(OPENSSL_EXTRA) || defined(WOLFSSL_WPAS_SMALL)

/**
 * Parse DER encoding and return header information.
 *
 * *in is moved to the value of the ASN1 object
 *
 * @param [in, out] in     Pointer to BER encoded data.
 * @param [out]     len    Length of parsed ASN1 object
 * @param [out]     tag    Tag value of parsed ASN1 object
 * @param [out]     cls    Class of parsed ASN1 object
 * @param [in]      inLen  Length of *in buffer
 * @return int  Depends on which bits are set in the returned int:
 *              0x80 an error occurred during parsing.
 *              0x20 parsed object is constructed.
 *              0x01 the parsed object length is indefinite.
 */
int wolfSSL_ASN1_get_object(const unsigned char **in, long *len, int *tag,
    int *cls, long inLen)
{
    int err = 0;
    word32 inOutIdx = 0;
    int l = 0;
    byte t = 0;
    int ret = 0x80;

    WOLFSSL_ENTER("wolfSSL_ASN1_get_object");

    if ((in == NULL) || (*in == NULL) || (len == NULL) || (tag == NULL) ||
            (cls == NULL) || (inLen <= 0)) {
        WOLFSSL_MSG("Bad parameter");
        err = 1;
    }
    if (!err) {
        /* Length at least 1, parameters valid - cannot fail to get tag. */
        GetASNTag(*in, &inOutIdx, &t, (word32)inLen);
        /* Get length in DER encoding. */
        if (GetLength_ex(*in, &inOutIdx, &l, (word32)inLen, 0) < 0) {
            WOLFSSL_MSG("GetLength error");
            err = 1;
        }
    }
    if (!err) {
        /* Return header information. */
        *tag = t & ASN_TYPE_MASK;  /* Tag number is 5 lsb */
        *cls = t & ASN_CLASS_MASK; /* Class is 2 msb */
        *len = l;
        ret = t & ASN_CONSTRUCTED;

        if (l > (int)(inLen - inOutIdx)) {
            /* Still return other values but indicate error in msb */
            ret |= 0x80;
        }

        /* Move pointer to after DER header. */
        *in += inOutIdx;
    }

    return ret;
}

/* Creates and ASN.1 OBJECT_ID object from DER encoding.
 *
 * @param [out]     a       Pointer to return new ASN.1 OBJECT_ID through.
 * @param [in, out] der     Pointer to buffer holding DER encoding.
 * @param [in]      length  Length of DER encoding in bytes.
 * @return  New ASN.1 OBJECT_ID object on success.
 * @return  NULL when der or *der is NULL or length is less than or equal zero.
 * @return  NULL when not an OBJECT_ID or decoding fails.
 * @return  NULL when dynamic memory allocation fails.
 */
WOLFSSL_ASN1_OBJECT *wolfSSL_d2i_ASN1_OBJECT(WOLFSSL_ASN1_OBJECT **a,
    const unsigned char **der, long length)
{
    WOLFSSL_ASN1_OBJECT* ret = NULL;
    int err = 0;
    const unsigned char *d;
    long len = 0;
    int tag = 0;
    int cls;

    WOLFSSL_ENTER("wolfSSL_d2i_ASN1_OBJECT");

    /* Validate parameters. */
    if ((der == NULL) || (*der == NULL) || (length <= 0)) {
        WOLFSSL_MSG("Bad parameter");
        err = 1;
    }
    if (!err) {
        /* Get pointer to be modified along the way. */
        d = *der;

        /* Move d to value and get length and tag. */
        if (wolfSSL_ASN1_get_object(&d, &len, &tag, &cls, length) & 0x80) {
            WOLFSSL_MSG("wolfSSL_ASN1_get_object error");
            err = 1;
        }
    }
    /* Check it DER encoding is of an OBJECT_ID. */
    if ((!err) && (tag != ASN_OBJECT_ID)) {
        WOLFSSL_MSG("Not an ASN object");
        err = 1;
    }
    /* Create an ASN.1 OBJECT_ID_object from value. TODO: not DER encoding? */
    if ((!err) && ((ret = wolfSSL_c2i_ASN1_OBJECT(a, &d, len)) != NULL)) {
        /* Update pointer to after decoded bytes. */
        *der = d;
    }

    return ret;
}

/* Write out DER encoding of ASN.1 OBJECT_ID.
 *
 * When pp is NULL, length is returned.
 * When pp points to NULL, a new buffer is allocated and returned through pp.
 * When pp points to a buffer, it is moved on past encoded data on success.
 *
 * @param [in]      a   ASN.1 OBJECT_ID object.
 * @param [in, out] pp  Pointer to buffer to write to. May be NULL.
 * @return  Length of encoding on success.
 * @return  0 when a or encoding buffer is NULL.
 * @return  0 when dynamic memory allocation fails.
 */
int wolfSSL_i2d_ASN1_OBJECT(WOLFSSL_ASN1_OBJECT *a, unsigned char **pp)
{
    int len = 0;

    WOLFSSL_ENTER("wolfSSL_i2d_ASN1_OBJECT");

    /* Validate parameters */
    if ((a == NULL) || (a->obj == NULL)) {
        WOLFSSL_MSG("Bad parameters");
    }
    /* Only return length when no pointer supplied. */
    else if (pp == NULL) {
        len = (int)a->objSz;
    }
    else {
        byte *p = NULL;

        /* Check if we have a buffer to encode into. */
        if (*pp == NULL) {
            /* Allocate a new buffer to return. */
            p = (byte*)XMALLOC(a->objSz, NULL, DYNAMIC_TYPE_OPENSSL);
            if (p == NULL) {
                WOLFSSL_MSG("Bad malloc");
            }
            else {
                /* Return allocated buffer. */
                *pp = p;
            }
        }

        /* Check we have a buffer to encode into. */
        if (*pp != NULL) {
            /* Copy in DER encoding. */
            XMEMCPY(*pp, a->obj, a->objSz);
            /* Move on pointer if user supplied. */
            if (p == NULL) {
                *pp += a->objSz;
            }
            /* Return length of DER encoding. */
            len = (int)a->objSz;
        }
    }

    return len;
}

/* Create an ASN.1 OBJECT_ID object from the content octets.
 *
 * @param [out]     a    Pointer to return ASN.1 OBJECT_ID object.
 * @param [in, out] pp   Pointer to buffer holding content octets.
 * @param [in]      len  Length of content octets in bytes.
 * @return  New ASN.1 OBJECT_ID object on success.
 * @return  NULL when pp or *pp is NULL or length is less than or equal zero.
 * @return  NULL when dynamic memory allocation fails.
 */
WOLFSSL_ASN1_OBJECT *wolfSSL_c2i_ASN1_OBJECT(WOLFSSL_ASN1_OBJECT **a,
        const unsigned char **pp, long len)
{
    int err = 0;
    WOLFSSL_ASN1_OBJECT* ret = NULL;

    WOLFSSL_ENTER("wolfSSL_c2i_ASN1_OBJECT");

    /* Validate parameters. */
    if ((pp == NULL) || (*pp == NULL) || (len <= 0)) {
        WOLFSSL_MSG("Bad parameter");
        err = 1;
    }

    /* Create a new ASN.1 OBJECT_ID object. */
    if ((!err) && ((ret = wolfSSL_ASN1_OBJECT_new()) == NULL)) {
        WOLFSSL_MSG("wolfSSL_ASN1_OBJECT_new error");
        err = 1;
    }

    if (!err) {
        /* Allocate memory for content octets. */
        ret->obj = (const unsigned char*)XMALLOC((size_t)len, NULL,
            DYNAMIC_TYPE_ASN1);
        if (ret->obj == NULL) {
            WOLFSSL_MSG("error allocating asn data memory");
            wolfSSL_ASN1_OBJECT_free(ret);
            ret = NULL;
            err = 1;
        }
    }

    if (!err) {
        /* Content octets buffer was dynamically allocated. */
        ret->dynamic |= WOLFSSL_ASN1_DYNAMIC_DATA;
        /* Copy in content octets and set size. */
        XMEMCPY((byte*)ret->obj, *pp, (size_t)len);
        ret->objSz = (unsigned int)len;

        /* Move pointer to after data copied out. */
        *pp += len;
        /* Return ASN.1 OBJECT_ID object through a if required. */
        if (a != NULL) {
            *a = ret;
        }
    }

    return ret;
}

#endif /* OPENSSL_EXTRA || WOLFSSL_WPAS_SMALL */

#ifdef OPENSSL_EXTRA

/* Write at most buf_len bytes of textual representation of ASN.1 OBJECT_ID.
 *
 * @param [in, out] buf      Buffer to write to.
 * @param [in]      buf_len  Length of buffer in bytes.
 * @param [in]      a        ASN.1 OBJECT_ID object.
 * @return  Number of bytes written on success.
 * @return  0 on failure.
 */
int wolfSSL_i2t_ASN1_OBJECT(char *buf, int buf_len, WOLFSSL_ASN1_OBJECT *a)
{
    WOLFSSL_ENTER("wolfSSL_i2t_ASN1_OBJECT");

    return wolfSSL_OBJ_obj2txt(buf, buf_len, a, 0);
}

#ifndef NO_BIO
/* Write out the text encoding of the ASN.1 OBJECT_ID.
 *
 * @param [in] bp  BIO to write to.
 * @param [in] a   ASN.1 OBJECT_ID object.
 * @return  Number of bytes written on success.
 * @return  0 on failure.
 */
int wolfSSL_i2a_ASN1_OBJECT(WOLFSSL_BIO *bp, WOLFSSL_ASN1_OBJECT *a)
{
    int length = 0;
    int cLen = 0;
    word32 idx = 0;
    const char null_str[] = "NULL";
    const char invalid_str[] = "<INVALID>";
    char buf[80];

    WOLFSSL_ENTER("wolfSSL_i2a_ASN1_OBJECT");

    /* Validate parameters. */
    if (bp == NULL) {
        /* Do nothing. */
    }
    /* NULL object is written as "NULL". */
    else if (a == NULL) {
        /* Write "NULL" - as done in OpenSSL. */
        length = wolfSSL_BIO_write(bp, null_str, (int)XSTRLEN(null_str));
    }
    /* Try getting text version and write it out. */
    else if ((length = i2t_ASN1_OBJECT(buf, sizeof(buf), a)) > 0) {
        length = wolfSSL_BIO_write(bp, buf, length);
    }
    /* Look for DER header. */
    else if ((a->obj == NULL) || (a->obj[idx++] != ASN_OBJECT_ID)) {
        WOLFSSL_MSG("Bad ASN1 Object");
    }
    /* Get length from DER header. */
    else if (GetLength((const byte*)a->obj, &idx, &cLen, a->objSz) < 0) {
        length = 0;
    }
    else {
        /* Write out "<INVALID>" and dump content. */
        length = wolfSSL_BIO_write(bp, invalid_str, (int)XSTRLEN(invalid_str));
        length += wolfSSL_BIO_dump(bp, (const char*)(a->obj + idx), cLen);
    }

    return length;
}
#endif /* !NO_BIO */

#endif /* OPENSSL_EXTRA */

/*******************************************************************************
 * ASN1_SK_OBJECT APIs
 ******************************************************************************/

#if (defined(OPENSSL_EXTRA) || defined(WOLFSSL_WPAS_SMALL)) && !defined(NO_ASN)
/* Create a new WOLFSSL_ASN1_OBJECT stack.
 *
 * @return  New WOLFSSL_ASN1_OBJECT stack on success.
 * @return  NULL when dynamic memory allocation fails.
 */
WOLFSSL_STACK* wolfSSL_sk_new_asn1_obj(void)
{
    WOLFSSL_ENTER("wolfSSL_sk_new_asn1_obj");

    return wolfssl_sk_new_type(STACK_TYPE_OBJ);
}

/* Dispose of WOLFSL_ASN1_OBJECT stack.
 *
 * @param [in, out] sk  Stack to free nodes in.
 */
void wolfSSL_sk_ASN1_OBJECT_free(WOLF_STACK_OF(WOLFSSL_ASN1_OBJECT)* sk)
{
    /* Dispose of stack. */
    wolfSSL_sk_free(sk);
}

/* Dispose of all ASN.1 OBJECT_ID objects in ASN1_OBJECT stack.
 *
 * This is different then wolfSSL_ASN1_OBJECT_free in that it allows for
 * choosing the function to use when freeing an ASN1_OBJECT stack.
 *
 * @param [in, out] sk  ASN.1 OBJECT_ID stack to free.
 * @param [in]      f   Free function to apply to each ASN.1 OBJECT_ID object.
 */
void wolfSSL_sk_ASN1_OBJECT_pop_free(WOLF_STACK_OF(WOLFSSL_ASN1_OBJECT)* sk,
    void (*f) (WOLFSSL_ASN1_OBJECT*))
{
    WOLFSSL_ENTER("wolfSSL_sk_ASN1_OBJECT_pop_free");
    wolfSSL_sk_pop_free(sk, (wolfSSL_sk_freefunc)f);
}

/* Push a WOLFSSL_ASN1_OBJECT onto stack.
 *
 * @param [in, out] sk   ASN.1 OBJECT_ID stack.
 * @param [in]      obj  ASN.1 OBJECT_ID object to push on. Cannot be NULL.
 * @return  1 on success.
 * @return  0 when sk or obj is NULL.
 * @return  0 when dynamic memory allocation fails.
 */
int wolfSSL_sk_ASN1_OBJECT_push(WOLF_STACK_OF(WOLFSSL_ASN1_OBJECT)* sk,
    WOLFSSL_ASN1_OBJECT* obj)
{
    int ret = 0;

    WOLFSSL_ENTER("wolfSSL_sk_ASN1_OBJECT_push");

    /* Push on when we have a stack and object to work with. */
    if ((sk != NULL) && (obj != NULL)) {
        ret = wolfSSL_sk_push(sk, obj);
    }

    return ret;
}

/* Pop off a WOLFSSL_ASN1_OBJECT from the stack.
 *
 * @param [in, out] sk  ASN.1 OBJECT_ID stack.
 * @return  ASN.1 OBJECT_ID object on success.
 * @return  NULL when stack is NULL or no nodes left in stack.
 */
WOLFSSL_ASN1_OBJECT* wolfSSL_sk_ASN1_OBJECT_pop(
    WOLF_STACK_OF(WOLFSSL_ASN1_OBJECT)* sk)
{
    return (WOLFSSL_ASN1_OBJECT*)wolfssl_sk_pop_type(sk, STACK_TYPE_OBJ);
}

#endif /* (OPENSSL_EXTRA || WOLFSSL_WPAS_SMALL) && !NO_ASN */

/*******************************************************************************
 * ASN1_STRING APIs
 ******************************************************************************/

#if defined(OPENSSL_EXTRA) || defined(OPENSSL_EXTRA_X509_SMALL)

/* Create a new ASN.1 STRING object.
 *
 * @return  New ASN.1 STRING object on success.
 * @return  NULL when dynamic memory allocation fails.
 */
WOLFSSL_ASN1_STRING* wolfSSL_ASN1_STRING_new(void)
{
    WOLFSSL_ASN1_STRING* asn1;

#ifdef WOLFSSL_DEBUG_OPENSSL
    WOLFSSL_ENTER("wolfSSL_ASN1_STRING_new");
#endif

    asn1 = (WOLFSSL_ASN1_STRING*)XMALLOC(sizeof(WOLFSSL_ASN1_STRING), NULL,
        DYNAMIC_TYPE_OPENSSL);
    if (asn1 != NULL) {
        XMEMSET(asn1, 0, sizeof(WOLFSSL_ASN1_STRING));
    }

    return asn1;
}

/* Create a new ASN.1 STRING object.
 *
 * @param [in] type  Encoding type.
 * @return  New ASN.1 STRING object on success.
 * @return  NULL when dynamic memory allocation fails.
 */
WOLFSSL_ASN1_STRING* wolfSSL_ASN1_STRING_type_new(int type)
{
    WOLFSSL_ASN1_STRING* asn1;

#ifdef WOLFSSL_DEBUG_OPENSSL
    WOLFSSL_ENTER("wolfSSL_ASN1_STRING_type_new");
#endif

    asn1 = wolfSSL_ASN1_STRING_new();
    if (asn1 != NULL) {
        asn1->type = type;
    }

    return asn1;
}

/* Dispose of ASN.1 STRING object.
 *
 * @param [in, out] asn1  ASN.1 STRING object.
 */
void wolfSSL_ASN1_STRING_free(WOLFSSL_ASN1_STRING* asn1)
{
#ifdef WOLFSSL_DEBUG_OPENSSL
    WOLFSSL_ENTER("wolfSSL_ASN1_STRING_free");
#endif

    /* Check we have an object to free. */
    if (asn1 != NULL) {
        /* Dispose of dynamic data. */
        if ((asn1->length > 0) && asn1->isDynamic) {
            XFREE(asn1->data, NULL, DYNAMIC_TYPE_OPENSSL);
        }
    }
    /* Dispose of ASN.1 STRING object. */
    XFREE(asn1, NULL, DYNAMIC_TYPE_OPENSSL);
}

/* Copy an ASN.1 STRING object from src into dest.
 *
 * @param [in, out] dest  ASN.1 STRING object to copy into.
 * @param [in]      src   ASN.1 STRING object to copy from.
 */
int wolfSSL_ASN1_STRING_copy(WOLFSSL_ASN1_STRING* dest,
    const WOLFSSL_ASN1_STRING* src)
{
    int ret = 1;

    /* Validate parameters. */
    if ((src == NULL) || (dest == NULL)) {
        ret = 0;
    }
    /* Set the DER encoding. */
    if ((ret == 1) && (wolfSSL_ASN1_STRING_set(dest, src->data, src->length) !=
            1)) {
        ret = 0;
    }
    if (ret == 1) {
        /* Copy simple fields. */
        dest->type  = src->type;
        dest->flags = src->flags;
    }

    return ret;
}

/* Duplicate an ASN.1 STRING object.
 *
 * @param [in] asn1  ASN.1 STRING object to duplicate.
 * @return  New ASN.1 STRING object on success.
 * @return  NULL when asn1 is NULL or dynamic memory allocation fails.
 */
WOLFSSL_ASN1_STRING* wolfSSL_ASN1_STRING_dup(WOLFSSL_ASN1_STRING* asn1)
{
    WOLFSSL_ASN1_STRING* dupl = NULL;

    WOLFSSL_ENTER("wolfSSL_ASN1_STRING_dup");

    /* Check we have an object to duplicate. */
    if (asn1 == NULL) {
        WOLFSSL_MSG("Bad parameter");
    }
    else {
        /* Create a new ASN.1 STRING object. */
        dupl = wolfSSL_ASN1_STRING_new();
        if (dupl == NULL) {
            WOLFSSL_MSG("wolfSSL_ASN1_STRING_new error");
        }
    }

    if (dupl != NULL) {
        /* Copy the contents. */
        if (wolfSSL_ASN1_STRING_copy(dupl, asn1) != 1) {
            WOLFSSL_MSG("wolfSSL_ASN1_STRING_copy error");
            /* Dispose of duplicate and return NULL. */
            wolfSSL_ASN1_STRING_free(dupl);
            dupl = NULL;
        }
    }

    return dupl;
}

/* Compare two ASN.1 STRING objects.
 *
 * Compares type when data the same.
 *
 * @param [in] a  First ASN.1 STRING object.
 * @param [in] b  Second ASN.1 STRING object.
 * @return Negative value when a is less than b.
 * @return 0 when a equals b.
 * @return Positive value when a is greater than b.
 * @return -1 when a or b is NULL.
 */
int wolfSSL_ASN1_STRING_cmp(const WOLFSSL_ASN1_STRING *a,
    const WOLFSSL_ASN1_STRING *b)
{
    int ret;
    WOLFSSL_ENTER("wolfSSL_ASN1_STRING_cmp");

    /* Validate parameters. */
    if ((a == NULL) || (b == NULL)) {
        ret = -1;
    }
    /* Compare length of data. */
    else if (a->length != b->length) {
        ret = a->length - b->length;
    }
    /* Compare data. */
    else if ((ret = XMEMCMP(a->data, b->data, (size_t)a->length)) == 0) {
        /* Compare ASN.1 types - wolfSSL_ASN1_STRING_type_new(). */
        ret = a->type - b->type;
    }

    return ret;
}

#endif /* OPENSSL_EXTRA || OPENSSL_EXTRA_X509_SMALL */

#if defined(OPENSSL_EXTRA)
#if !defined(NO_CERTS)
#ifndef NO_WOLFSSL_STUB
WOLFSSL_ASN1_STRING* wolfSSL_d2i_DISPLAYTEXT(WOLFSSL_ASN1_STRING **asn,
    const unsigned char **in, long len)
{
    WOLFSSL_STUB("d2i_DISPLAYTEXT");
    (void)asn;
    (void)in;
    (void)len;
    return NULL;
}
#endif
#endif /* !NO_CERTS */
#endif /* OPENSSL_EXTRA */

#ifndef NO_ASN
#if defined(OPENSSL_EXTRA)
/* Convert ASN.1 STRING that is UniversalString type to PrintableString type.
 *
 * @param [in, out] s  ASN.1 STRING object to convert.
 * @return  1 on success.
 * @return  0 when s is NULL.
 * @return  0 when type is not UniversalString or string is not of that format.
 */
int wolfSSL_ASN1_UNIVERSALSTRING_to_string(WOLFSSL_ASN1_STRING *s)
{
    int ret = 1;
    char* p;

    WOLFSSL_ENTER("wolfSSL_ASN1_UNIVERSALSTRING_to_string");

    /* Validate parameter. */
    if (s == NULL) {
        WOLFSSL_MSG("Bad parameter");
        ret = 0;
    }

    /* Check type of ASN.1 STRING. */
    if ((ret == 1) && (s->type != V_ASN1_UNIVERSALSTRING)) {
        WOLFSSL_MSG("Input is not a universal string");
        ret = 0;
    }

    /* Check length is indicative of UNIVERSAL_STRING. */
    if ((ret == 1) && ((s->length % 4) != 0)) {
        WOLFSSL_MSG("Input string must be divisible by 4");
        ret = 0;
    }

    if (ret == 1) {
        /* Ensure each UniversalString character looks right. */
        for (p = s->data; p < s->data + s->length; p += 4)
            if ((p[0] != '\0') || (p[1] != '\0') || (p[2] != '\0'))
                break;
        /* Check whether we failed loop early. */
        if (p != s->data + s->length) {
            WOLFSSL_MSG("Wrong string format");
            ret = 0;
        }
    }

    if (ret == 1) {
        char* copy;

        /* Strip first three bytes of each four byte character. */
        for (copy = p = s->data; p < s->data + s->length; p += 4) {
            *copy++ = p[3];
        }
        /* Place NUL on end. */
        *copy = '\0';
        /* Update length and type. */
        s->length /= 4;
        s->type = V_ASN1_PRINTABLESTRING;
    }

    return ret;
}
#endif /* OPENSSL_EXTRA */

#if defined(OPENSSL_EXTRA) || defined(OPENSSL_EXTRA_X509_SMALL)
/* Convert ASN.1 STRING to UTF8 encoding.
 *
 * Assumes stored encoding is UTF8.
 * Returned buffer should be freed using OPENSSL_free().
 *
 * @param [out] out   Pointer to return allocated string.
 * @param [in]  asn1  ASN.1 STRING object.
 * @return  Length of string, excluding NUL, on success.
 * @return  -1 when out or asn1 is NULL.
 * @return  -1 when no data to return.
 * @return  -1 dynamic memory allocation fails.
 */
int wolfSSL_ASN1_STRING_to_UTF8(unsigned char **out, WOLFSSL_ASN1_STRING *asn1)
{
    unsigned char* buf = NULL;
    unsigned char* data = NULL;
    int len = -1;

    /* Validate parameters. */
    if ((out != NULL) && (asn1 != NULL)) {
        /* Get data and length. */
        data = wolfSSL_ASN1_STRING_data(asn1);
        len = wolfSSL_ASN1_STRING_length(asn1);
        /* Check data and length are usable. */
        if ((data == NULL) || (len < 0)) {
            len = -1;
        }
    }
    if (len != -1) {
        /* Allocate buffer to hold string and NUL. */
        buf = (unsigned char*)XMALLOC((size_t)(len + 1), NULL,
            DYNAMIC_TYPE_OPENSSL);
        if (buf == NULL) {
            len = -1;
        }
    }
    if (len != -1) {
        /* Copy in string - NUL always put on end of stored string. */
        XMEMCPY(buf, data, (size_t)(len + 1));
        /* Return buffer. */
        *out = buf;
    }

    return len;
}
#endif /* OPENSSL_EXTRA || OPENSSL_EXTRA_X509_SMALL */

#if defined(OPENSSL_EXTRA)

/* Encode ASN.1 STRING data as hex digits separated by colon.
 *
 * Assumes length is greater than 0.
 *
 * @param [in] s  ASN.1 STRING object.
 * @return  Buffer containing string representation on success.
 * @return  NULL when dynamic memory allocation fails.
 * @return  NULL when encoding a character as hex fails.
 */
static char* wolfssl_asn1_string_to_hex_chars(const WOLFSSL_ASN1_STRING *s)
{
    char* tmp;
    int tmpSz = s->length * 3;

    tmp = (char*)XMALLOC((size_t)tmpSz, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (tmp == NULL) {
        WOLFSSL_MSG("Memory Error");
    }
    else {
        int i;
        unsigned char* str = (unsigned char*)s->data;

        /* Put out all but last character as a hex digit with ':'. */
        for (i = 0; i < s->length; i++) {
            /* Put in hex digit string at end of tmp. */
            ByteToHexStr(str[i], tmp + i * 3);
            /* Check not last character. */
            if (i < s->length - 1) {
                /* Put in separator: ':'. */
                tmp[i * 3 + 2] = ':';
            }
            /* Last character. */
            else {
                /* Put in NUL to terminate string. */
                tmp[i * 3 + 2] = '\0';
            }
        }
    }

    return tmp;
}

/* Create a string encoding of ASN.1 STRING object.
 *
 * @param [in] method  Method table. Unused.
 * @param [in] s       ASN.1 STRING object.
 * @return  Buffer containing string representation on success.
 * @return  NULL when s or data is NULL.
 * @return  NULL when dynamic memory allocation fails.
 * @return  NULL when encoding a character as hex fails.
 */
char* wolfSSL_i2s_ASN1_STRING(WOLFSSL_v3_ext_method *method,
    const WOLFSSL_ASN1_STRING *s)
{
    char* ret = NULL;

    WOLFSSL_ENTER("wolfSSL_i2s_ASN1_STRING");
    (void)method;

    /* Validate parameters. */
    if ((s == NULL) || (s->data == NULL)) {
        WOLFSSL_MSG("Bad Function Argument");
    }
    /* Handle 0 length data separately. */
    else if (s->length == 0) {
        ret = (char *)XMALLOC(1, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        if (ret != NULL) {
            ret[0] = '\0';
        }
    }
    else {
        /* Convert unreadable strings to hexadecimal. */
        ret = wolfssl_asn1_string_to_hex_chars(s);
    }

    return ret;
}
#endif /* OPENSSL_EXTRA */
#endif /* NO_ASN */

#if defined(OPENSSL_EXTRA) || defined(OPENSSL_EXTRA_X509_SMALL)
/* Get the type of encoding.
 *
 * @param [in] asn1  ASN.1 STRING object.
 * @return  Encoding type on success.
 * @return  0 when asn1 is NULL or no encoding set.
 */
int wolfSSL_ASN1_STRING_type(const WOLFSSL_ASN1_STRING* asn1)
{
    int type = 0;

#ifdef WOLFSSL_DEBUG_OPENSSL
    WOLFSSL_ENTER("wolfSSL_ASN1_STRING_type");
#endif

    if (asn1 != NULL) {
        type = asn1->type;
    }

    return type;
}

#ifndef NO_CERTS
/* Get the pointer that is the data.
 *
 * @param [in] asn  ASN.1 STRING object.
 * @return  Buffer with string on success.
 * @return  NULL when asn is NULL or no data set.
 */
const unsigned char* wolfSSL_ASN1_STRING_get0_data(
    const WOLFSSL_ASN1_STRING* asn)
{
    char* data = NULL;

    WOLFSSL_ENTER("wolfSSL_ASN1_STRING_get0_data");

    if (asn != NULL) {
        data = asn->data;
    }

    return (const unsigned char*)data;
}

/* Get the pointer that is the data.
 *
 * @param [in] asn  ASN.1 STRING object.
 * @return  Buffer with string on success.
 * @return  NULL when asn is NULL or no data set.
 */
unsigned char* wolfSSL_ASN1_STRING_data(WOLFSSL_ASN1_STRING* asn)
{
    char* data = NULL;

#ifdef WOLFSSL_DEBUG_OPENSSL
    WOLFSSL_ENTER("wolfSSL_ASN1_STRING_data");
#endif

    if (asn != NULL) {
        data = asn->data;
    }

    return (unsigned char*)data;
}

/* Get the length of the data.
 *
 * @param [in] asn  ASN.1 STRING object.
 * @return  String length on success.
 * @return  0 when asn is NULL or no data set.
 */
int wolfSSL_ASN1_STRING_length(WOLFSSL_ASN1_STRING* asn)
{
    int len = 0;

#ifdef WOLFSSL_DEBUG_OPENSSL
    WOLFSSL_ENTER("wolfSSL_ASN1_STRING_length");
#endif

    if (asn) {
        len = asn->length;
    }

    return len;
}
#endif /* !NO_CERTS */

/* Set the string data.
 *
 * When sz is less than 0, the string length will be calculated using XSTRLEN.
 *
 * @param [in, out] asn1    ASN.1 STRING object.
 * @param [in]      data    String data to set.
 * @param [in]      sz      Length of data to set in bytes.
 * @return  1 on success.
 * @return  0 when asn1 is NULL or data is NULL and sz is not zero.
 * @return  0 when dynamic memory allocation fails.
 */
int wolfSSL_ASN1_STRING_set(WOLFSSL_ASN1_STRING* asn1, const void* data, int sz)
{
    int ret = 1;

#ifdef WOLFSSL_DEBUG_OPENSSL
    WOLFSSL_ENTER("wolfSSL_ASN1_STRING_set");
#endif

    /* Validate parameters. */
    if ((asn1 == NULL) || ((data == NULL) && (sz != 0))) {
        ret = 0;
    }

    /* Calculate size from data if not passed in. */
    if ((ret == 1) && (sz < 0)) {
        sz = (int)XSTRLEN((const char*)data);
        if (sz < 0) {
            ret = 0;
        }
    }

    if (ret == 1) {
        /* Dispose of any existing dynamic data. */
        if (asn1->isDynamic) {
            XFREE(asn1->data, NULL, DYNAMIC_TYPE_OPENSSL);
            asn1->data = NULL;
        }

        /* Check string will fit - including NUL. */
        if (sz + 1 > CTC_NAME_SIZE) {
            /* Allocate new buffer. */
            asn1->data = (char*)XMALLOC((size_t)(sz + 1), NULL,
                DYNAMIC_TYPE_OPENSSL);
            if (asn1->data == NULL) {
                ret = 0;
            }
            else {
                /* Ensure buffer will be freed. */
                asn1->isDynamic = 1;
            }
        }
        else {
            /* Clear out fixed array and use it for data. */
            XMEMSET(asn1->strData, 0, CTC_NAME_SIZE);
            asn1->data = asn1->strData;
            asn1->isDynamic = 0;
        }
    }
    if (ret == 1) {
        /* Check if there is a string to copy. */
        if (data != NULL) {
            /* Copy string and append NUL. */
            XMEMCPY(asn1->data, data, (size_t)sz);
            asn1->data[sz] = '\0';
        }
        /* Set size of string. */
        asn1->length = sz;
    }

    return ret;
}
#endif /* OPENSSL_EXTRA || OPENSSL_EXTRA_X509_SMALL */

#if (defined(OPENSSL_EXTRA) || defined(OPENSSL_EXTRA_X509_SMALL)) && \
    !defined(WOLFCRYPT_ONLY)
#ifndef NO_CERTS

/* Make a UTF8 canonical version of ASN.1 STRING object's data.
 *
 * @param [in, out] asn  ASN.1 STRING to set.
 */
static void wolfssl_asn1_string_canonicalize(WOLFSSL_ASN1_STRING* asn)
{
    char* src = asn->data;
    char* p = asn->data + asn->length - 1;
    int len = asn->length;
    int i;

    /* Trim whitespace from the tail. */
    for (; (len > 0) && (XISSPACE((unsigned char)*p)); len--) {
        p--;
    }
    if (len > 0) {
        /* Trim whitespace from the head. */
        for (; XISSPACE((unsigned char)*src); len--) {
            src++;
        }
    }

    /* Output at the start. */
    p = asn->data;
    /* Process each character in string after trim. */
    for (i = 0; i < len; p++, i++) {
        /* Check for non-ascii character. */
        if (!XISASCII(*src)) {
            /* Keep non-ascii character as-is. */
            *p = *src++;
        }
        /* Check for whitespace. */
        else if (XISSPACE((unsigned char)*src)) {
            /* Only use space character for whitespace. */
            *p = 0x20;
            /* Skip any succeeding whitespace characters. */
            while (XISSPACE((unsigned char)*++src)) {
                i++;
            }
        }
        else {
            /* Convert to lower case. */
            *p = (char)XTOLOWER((unsigned char)*src++);
        }
    }
    /* Set actual length after canonicalization. */
    asn->length = (int)(p - asn->data);
}

/* Make a canonical version of ASN.1 STRING object in ASN.1 STRING object.
 *
 * @param [in, out] asn_out  ASN.1 STRING object to set.
 * @param [in]      asn_in   ASN.1 STRING object to get data from.
 * @return  1 on success.
 * @return  BAD_FUNC_ARG when asn_out or asn_in is NULL.
 * @return  0 when no data.
 * @return  0 when dynamic memory allocation fails.
 */
int wolfSSL_ASN1_STRING_canon(WOLFSSL_ASN1_STRING* asn_out,
    const WOLFSSL_ASN1_STRING* asn_in)
{
    int ret = 1;

    WOLFSSL_ENTER("wolfSSL_ASN1_STRING_canon");

    /* Validate parameters. */
    if ((asn_out == NULL) || (asn_in == NULL)) {
        WOLFSSL_MSG("invalid function arguments");
        ret = BAD_FUNC_ARG;
    }

    if (ret == 1) {
        switch (asn_in->type) {
            case MBSTRING_UTF8:
            case V_ASN1_PRINTABLESTRING:
                /* Set type to UTF8. */
                asn_out->type = MBSTRING_UTF8;
                /* Dispose of any dynamic data already in asn_out. */
                if (asn_out->isDynamic) {
                    XFREE(asn_out->data, NULL, DYNAMIC_TYPE_OPENSSL);
                    asn_out->data = NULL;
                }
                /* Make ASN.1 STRING into UTF8 buffer. */
                asn_out->length = wolfSSL_ASN1_STRING_to_UTF8(
                    (unsigned char**)&asn_out->data,
                    (WOLFSSL_ASN1_STRING*)asn_in);
                /* Check for error from creating UTF8 string. */
                if (asn_out->length < 0) {
                    ret = 0;
                }
                else {
                    /* Data now dynamic after converting to UTF8. */
                    asn_out->isDynamic = 1;
                    /* Canonicalize the data. */
                    wolfssl_asn1_string_canonicalize(asn_out);
                    if (asn_out->length == 0) {
                        /* Dispose of data if canonicalization removes all
                         * characters. */
                        XFREE(asn_out->data, NULL, DYNAMIC_TYPE_OPENSSL);
                        asn_out->data = NULL;
                        asn_out->isDynamic = 0;
                    }
                }
                break;
            default:
                /* Unrecognized format - just copy. */
                WOLFSSL_MSG("just copy string");
                ret = wolfSSL_ASN1_STRING_copy(asn_out, asn_in);
        }
    }

    return ret;
}

#endif /* !NO_CERTS */
#endif /* (OPENSSL_EXTRA || OPENSSL_EXTRA_X509_SMALL) && !WOLFCRYPT_ONLY */

#if defined(OPENSSL_EXTRA)

#if !defined(NO_ASN)
#ifndef NO_BIO
/* Returns boolean indicating character is unprintable.
 *
 * @param [in] c  ASCII character.
 * @return  1 when character is unprintable.
 * @return  0 when character is printable.
 */
static int wolfssl_unprintable_char(char c)
{
    const unsigned char last_unprintable = 31;
    const unsigned char LF = 10;               /* Line Feed */
    const unsigned char CR = 13;               /* Carriage Return */

    return (c <= last_unprintable) && (c != LF) && (c != CR);
}

/* Print ASN.1 STRING to BIO.
 *
 * TODO: Unprintable characters conversion is destructive.
 *
 * @param [in] bio  BIO to print to.
 * @param [in] str  ASN.1 STRING to print.
 * @return  Length of string written on success.
 * @return  0 when bio or str is NULL.
 * @return  0 when writing to BIO fails.
 */
int wolfSSL_ASN1_STRING_print(WOLFSSL_BIO *bio, WOLFSSL_ASN1_STRING *str)
{
    int len = 0;

    WOLFSSL_ENTER("wolfSSL_ASN1_STRING_print");

    /* Validate parameters. */
    if ((bio != NULL) && (str != NULL)) {
        int i;

        len = str->length;
        /* Convert all unprintable characters to '.'. */
        for (i = 0; i < len; i++) {
            if (wolfssl_unprintable_char(str->data[i])) {
                str->data[i] = '.';
            }
        }
        /* Write string to BIO. */
        if (wolfSSL_BIO_write(bio, str->data, len) != len) {
            len = 0;
        }
    }

    return len;
}
#endif /* !NO_BIO */
#endif /* !NO_ASN */

/* Get a string for the ASN.1 tag.
 *
 * @param [in] tag  ASN.1 tag.
 * @return  A string.
 */
const char* wolfSSL_ASN1_tag2str(int tag)
{
    static const char *const tag_label[31] = {
        "EOC", "BOOLEAN", "INTEGER", "BIT STRING", "OCTET STRING", "NULL",
        "OBJECT", "OBJECT DESCRIPTOR", "EXTERNAL", "REAL", "ENUMERATED",
        "<ASN1 11>", "UTF8STRING", "<ASN1 13>", "<ASN1 14>", "<ASN1 15>",
        "SEQUENCE", "SET", "NUMERICSTRING", "PRINTABLESTRING", "T61STRING",
        "VIDEOTEXTSTRING", "IA5STRING", "UTCTIME", "GENERALIZEDTIME",
        "GRAPHICSTRING", "VISIBLESTRING", "GENERALSTRING", "UNIVERSALSTRING",
        "<ASN1 29>", "BMPSTRING"
    };
    const char* str = "(unknown)";

    /* Clear negative flag. */
    if ((tag == V_ASN1_NEG_INTEGER) || (tag == V_ASN1_NEG_ENUMERATED)) {
        tag &= ~V_ASN1_NEG;
    }
    /* Check for known basic types. */
    if ((tag >= 0) && (tag <= 30)) {
        str = tag_label[tag];
    }

    return str;
}

#ifndef NO_BIO

/* Print out ASN.1 tag for the ASN.1 STRING to the BIO.
 *
 * @param [in] bio  BIO to write to.
 * @param [in] str  ASN.1 STRING object.
 * @return  Number of characters written on success.
 * @return  0 when BIO write fails.
 */
static int wolfssl_string_print_type(WOLFSSL_BIO *bio, WOLFSSL_ASN1_STRING *str)
{
    int type_len;
    const char *tag;

    /* Get tag and string length. */
    tag = wolfSSL_ASN1_tag2str(str->type);
    type_len = (int)XSTRLEN(tag);
    /* Write tag to BIO. */
    if (wolfSSL_BIO_write(bio, tag, type_len) != type_len){
        type_len = 0;
    }
    /* Write colon after tag string. */
    else if (wolfSSL_BIO_write(bio, ":", 1) != 1) {
        type_len = 0;
    }
    else {
        /* Written colon - update count. */
        type_len++;
    }

    return type_len;
}

/* Dump hex digit representation of each string character to BIO.
 *
 * TODO: Assumes length is only one byte ie less than 128 characters long.
 *
 * @param [in] bio    BIO to write to.
 * @param [in] str    ASN.1 STRING object.
 * @param [in] asDer  Whether to write out as a DER encoding.
 * @return  Number of characters written to BIO on success.
 * @return  -1 when writing to BIO fails.
 */
static int wolfssl_asn1_string_dump_hex(WOLFSSL_BIO *bio,
    WOLFSSL_ASN1_STRING *str, int asDer)
{
    const char* hash="#";
    char hex_tmp[4];
    int str_len = 1;

    /* Write out hash character to indicate hex string. */
    if (wolfSSL_BIO_write(bio, hash, 1) != 1) {
        str_len = -1;
    }
    else {
        /* Check if we are to write out DER header. */
        if (asDer) {
            /* Encode tag and length as hex into temporary. */
            ByteToHexStr((byte)str->type, &hex_tmp[0]);
            ByteToHexStr((byte)str->length, &hex_tmp[2]);
            /* Update count of written characters: tag and length. */
            str_len += 4;
            /* Write out tag and length as hex digits. */
            if (wolfSSL_BIO_write(bio, hex_tmp, 4) != 4) {
                str_len = -1;
            }
        }
    }

    if (str_len != -1) {
        char* p;
        char* end;

        /* Calculate end of string. */
        end = str->data + str->length - 1;
        for (p = str->data; p <= end; p++) {
            /* Encode string character as hex into temporary. */
            ByteToHexStr((byte)*p, hex_tmp);
            /* Update count of written characters. */
            str_len += 2;
            /* Write out character as hex digites. */
            if (wolfSSL_BIO_write(bio, hex_tmp, 2) != 2) {
                str_len = -1;
                break;
            }
        }
    }

    return str_len;
}

/* Check whether character needs to be escaped.
 *
 * @param [in] c    Character to check for.
 * @param [in] str  String to check.
 * @return  1 when character found.
 * @return  0 when character not found.
 */
static int wolfssl_check_esc_char(char c)
{
    int ret = 0;
    const char esc_ch[] = "+;<>\\";
    const char* p = esc_ch;

    /* Check if character matches any of those needing escaping. */
    for (; (*p) != '\0'; p++) {
        /* Check if character matches escape character. */
        if (c == (*p)) {
            ret = 1;
            break;
        }
    }

    return ret;
}

/* Print out string, with escaping for special characters, to BIO.
 *
 * @param [in] bio  BIO to write to.
 * @param [in] str  ASN.1 STRING object.
 * @return  Number of characters written to BIO on success.
 * @return  -1 when writing to BIO fails.
 */
static int wolfssl_asn1_string_print_esc_2253(WOLFSSL_BIO *bio,
    WOLFSSL_ASN1_STRING *str)
{
    char* p;
    int str_len = 0;

    /* Write all of string character by character. */
    for (p = str->data; (*p) != '\0'; p++) {
        /* Check if character needs escaping. */
        if (wolfssl_check_esc_char(*p)){
            /* Update count of written characters. */
            str_len++;
            /* Write out escaping character. */
            if (wolfSSL_BIO_write(bio,"\\", 1) != 1) {
                str_len = -1;
                break;
            }
        }
        /* Update count of written characters. */
        str_len++;
        /* Write out character. */
        if (wolfSSL_BIO_write(bio, p, 1) != 1) {
            str_len = -1;
            break;
        }
    }

    return str_len;
}

/* Extended print ASN.1 STRING to BIO.
 *
 * @param [in] bio    BIO to print to.
 * @param [in] str    ASN.1 STRING to print.
 * @param [in] flags  Flags describing output format.
 * @return  Length of string written on success.
 * @return  0 when bio or str is NULL.
 * @return  0 when writing to BIO fails.
 */
int wolfSSL_ASN1_STRING_print_ex(WOLFSSL_BIO *bio, WOLFSSL_ASN1_STRING *str,
    unsigned long flags)
{
    int err = 0;
    int str_len = -1;
    int type_len = 0;

    WOLFSSL_ENTER("wolfSSL_ASN1_STRING_PRINT_ex");

    /* Validate parameters. */
    if ((bio == NULL) || (str == NULL)) {
        err = 1;
    }
    /* Check if ASN.1 type is to be printed. */
    if ((!err) && (flags & ASN1_STRFLGS_SHOW_TYPE)) {
        /* Print type and colon to BIO. */
        type_len = wolfssl_string_print_type(bio, str);
        if (type_len == 0) {
            err = 1;
        }
    }

    if (!err) {
        if (flags & ASN1_STRFLGS_DUMP_ALL) {
            /* Dump hex. */
            str_len = wolfssl_asn1_string_dump_hex(bio, str,
                flags & ASN1_STRFLGS_DUMP_DER);
        }
        else if (flags & ASN1_STRFLGS_ESC_2253) {
            /* Print out string with escaping. */
            str_len = wolfssl_asn1_string_print_esc_2253(bio, str);
        }
        else {
            /* Get number of characters to write. */
            str_len = str->length;
            /* Print out string as is. */
            if (wolfSSL_BIO_write(bio, str->data, str_len) != str_len) {
                err = 1;
            }
        }
    }

    if ((!err) && (str_len != -1)) {
        /* Include any characters written for type. */
        str_len += type_len;
    }
    else {
        str_len = 0;
    }

    return str_len;
}

#endif /* !NO_BIO */

#endif /* OPENSSL_EXTRA */

/*******************************************************************************
 * ASN1_GENERALIZEDTIME APIs
 ******************************************************************************/

#ifdef OPENSSL_EXTRA

/* Free the static ASN.1 GENERALIZED TIME object.
 *
 * Not an OpenSSL compatibility API.
 *
 * @param [in] asn1Time  ASN.1 GENERALIZED TIME object.
 */
void wolfSSL_ASN1_GENERALIZEDTIME_free(WOLFSSL_ASN1_TIME* asn1Time)
{
    WOLFSSL_ENTER("wolfSSL_ASN1_GENERALIZEDTIME_free");
    if (asn1Time != NULL) {
        XMEMSET(asn1Time->data, 0, sizeof(asn1Time->data));
    }
}

#ifndef NO_BIO
/* Return the month as a string.
 *
 * Assumes n is '01'-'12'.
 *
 * @param [in] n  The number of the month as a two characters (1 based).
 * @return  Month as a string.
 */
static WC_INLINE const char* MonthStr(const char* n)
{
    static const char monthStr[12][4] = {
            "Jan", "Feb", "Mar", "Apr", "May", "Jun",
            "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
    const char* month = "BAD";
    int i;

    i = (n[0] - '0') * 10 + (n[1] - '0') - 1;
    /* Convert string to number and index table. */
    if ((i >= 0) && (i < 12)) {
        month = monthStr[i];
    }

    return month;
}

/* Print an ASN.1 GENERALIZED TIME to a BIO.
 *
 * @param [in] bio      BIO to write to.
 * @param [in] asnTime  ASN.1 GENERALIZED TIME object.
 * @return  1 on success.
 * @return  0 when ASN.1 GENERALIZED TIME type is invalid.
 * @return  0 when writing to BIO fails.
 * @return  BAD_FUNC_ARG when bio or asnTime is NULL.
 */
int wolfSSL_ASN1_GENERALIZEDTIME_print(WOLFSSL_BIO* bio,
    const WOLFSSL_ASN1_GENERALIZEDTIME* asnTime)
{
    int ret = 1;
    const char* p = NULL;
    WOLFSSL_ENTER("wolfSSL_ASN1_GENERALIZEDTIME_print");

    /* Validate parameters. */
    if ((bio == NULL) || (asnTime == NULL)) {
        ret = BAD_FUNC_ARG;
    }
    /* Check type is GENERALIZED TIME. */
    if ((ret == 1) && (asnTime->type != V_ASN1_GENERALIZEDTIME)) {
        WOLFSSL_MSG("Error, not GENERALIZED_TIME");
        ret = 0;
    }
    if (ret == 1) {
        /* Get the string. */
        p = (const char *)(asnTime->data);

        /* Print month as a 3 letter string. */
        if (wolfSSL_BIO_write(bio, MonthStr(p + 4), 3) != 3) {
            ret = 0;
        }
    }
    /* Print space separator. */
    if ((ret == 1) && (wolfSSL_BIO_write(bio, " ", 1) != 1)) {
        ret = 0;
    }

    /* Print day. */
    if ((ret == 1) && (wolfSSL_BIO_write(bio, p + 6, 2) != 2)) {
        ret = 0;
    }
    /* Print space separator. */
    if ((ret == 1) && (wolfSSL_BIO_write(bio, " ", 1) != 1)) {
        ret = 0;
    }

    /* Print hour. */
    if ((ret == 1) && (wolfSSL_BIO_write(bio, p + 8, 2) != 2)) {
        ret = 0;
    }
    /* Print time separator - colon. */
    if ((ret == 1) && (wolfSSL_BIO_write(bio, ":", 1) != 1)) {
        ret = 0;
    }

    /* Print minutes. */
    if ((ret == 1) && (wolfSSL_BIO_write(bio, p + 10, 2) != 2)) {
        ret = 0;
    }
    /* Print time separator - colon. */
    if ((ret == 1) && (wolfSSL_BIO_write(bio, ":", 1) != 1)) {
        ret = 0;
    }

    /* Print seconds. */
    if ((ret == 1) && (wolfSSL_BIO_write(bio, p + 12, 2) != 2)) {
        ret = 0;
    }
    /* Print space separator. */
    if ((ret == 1) && (wolfSSL_BIO_write(bio, " ", 1) != 1)) {
        ret = 0;
    }
    /* Print year. */
    if ((ret == 1) && (wolfSSL_BIO_write(bio, p, 4) != 4)) {
        ret = 0;
    }

    return ret;
}
#endif /* !NO_BIO */

#endif /* OPENSSL_EXTRA */

/*******************************************************************************
 * ASN1_TIME APIs
 ******************************************************************************/

#ifndef NO_ASN_TIME

#ifdef OPENSSL_EXTRA
/* Allocate a new ASN.1 TIME object.
 *
 * @return  New empty ASN.1 TIME object on success.
 * @return  NULL when dynamic memory allocation fails.
 */
WOLFSSL_ASN1_TIME* wolfSSL_ASN1_TIME_new(void)
{
    WOLFSSL_ASN1_TIME* ret;

    /* Allocate a new ASN.1 TYPE object. */
    ret = (WOLFSSL_ASN1_TIME*)XMALLOC(sizeof(WOLFSSL_ASN1_TIME), NULL,
        DYNAMIC_TYPE_OPENSSL);
    if (ret != NULL) {
        /* Clear out fields. */
        XMEMSET(ret, 0, sizeof(WOLFSSL_ASN1_TIME));
    }

    return ret;
}

/* Dispose of ASN.1 TIME object.
 *
 * @param [in, out] t  ASN.1 TIME object.
 */
void wolfSSL_ASN1_TIME_free(WOLFSSL_ASN1_TIME* t)
{
    /* Dispose of ASN.1 TIME object. */
    XFREE(t, NULL, DYNAMIC_TYPE_OPENSSL);
}

#ifndef NO_WOLFSSL_STUB
/* Set the Unix time GMT into ASN.1 TIME object.
 *
 * Not implemented.
 *
 * @param [in, out] a   ASN.1 TIME object.
 * @param [in]      t   Unix time GMT.
 * @return  An ASN.1 TIME object.
 */
WOLFSSL_ASN1_TIME *wolfSSL_ASN1_TIME_set(WOLFSSL_ASN1_TIME *a, time_t t)
{
    WOLFSSL_STUB("wolfSSL_ASN1_TIME_set");
    (void)a;
    (void)t;
    return a;
}
#endif /* !NO_WOLFSSL_STUB */

/* Convert time to Unix time (GMT).
 *
 * @param [in] sec     Second in minute. 0-59.
 * @param [in] minute  Minute in hour. 0-59.
 * @param [in] hour    Hour in day. 0-23.
 * @param [in] mday    Day of month. 1-31.
 * @param [in] mon     Month of year. 0-11
 * @param [in] year    Year including century. ie: 1991, 2023, 2048.
 * @return  Seconds since 00:00:00 01/01/1970 for the time passed in.
 */
static long long wolfssl_time_to_unix_time(int sec, int minute, int hour,
    int mday, int mon, int year)
{
    /* Number of cumulative days from the previous months, starting from
     * beginning of January. */
    static const int monthDaysCumulative [12] = {
        0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334
    };
    int leapDays = year;

    /* Leap day at end of February. */
    if (mon <= 1) {
        --leapDays;
    }
    /* Calculate leap days. */
    leapDays = leapDays / 4 - leapDays / 100 + leapDays / 400 - 1969 / 4 +
               1969 / 100 - 1969 / 400;

    /* Calculate number of seconds. */
    return ((((long long) (year - 1970) * 365 + leapDays +
           monthDaysCumulative[mon] + mday - 1) * 24 + hour) * 60 + minute) *
           60 + sec;
}

/* Convert ASN.1 TIME object to Unix time (GMT).
 *
 * @param [in]  t     ASN.1 TIME object.
 * @param [out] secs  Number of seconds since 00:00:00 01/01/1970.
 * @return  1 on success.
 * @return  0 when conversion of time fails.
 */
static int wolfssl_asn1_time_to_secs(const WOLFSSL_ASN1_TIME* t,
    long long* secs)
{
    int ret = 1;
    struct tm tm_s;
    struct tm *tmGmt = &tm_s;

    /* Convert ASN.1 TIME to broken-down time. NULL treated as current time. */
    ret = wolfSSL_ASN1_TIME_to_tm(t, tmGmt);
    if (ret != 1) {
        WOLFSSL_MSG("Failed to convert from time to struct tm.");
    }
    else {
        /* We use wolfssl_time_to_unix_time here instead of XMKTIME to avoid the
         * Year 2038 problem on platforms where time_t is 32 bits. struct tm
         * stores the year as years since 1900, so we add 1900 to the year. */
        *secs = wolfssl_time_to_unix_time(tmGmt->tm_sec, tmGmt->tm_min,
            tmGmt->tm_hour, tmGmt->tm_mday, tmGmt->tm_mon,
            tmGmt->tm_year + 1900);
    }

    return ret;
}

/* Calculate difference in time of two ASN.1 TIME objects.
 *
 * @param [out] days  Number of whole days between from and to.
 * @param [out] secs  Number of seconds less than a day between from and to.
 * @param [in]  from  ASN.1 TIME object as start time.
 * @param [in]  to    ASN.1 TIME object as end time.
 * @return  1 on success.
 * @return  0 when days or secs is NULL.
 * @return  0 when conversion of time fails.
 */
int wolfSSL_ASN1_TIME_diff(int *days, int *secs, const WOLFSSL_ASN1_TIME *from,
    const WOLFSSL_ASN1_TIME *to)
{
    int ret = 1;

    WOLFSSL_ENTER("wolfSSL_ASN1_TIME_diff");

    /* Validate parameters. */
    if (days == NULL) {
        WOLFSSL_MSG("days is NULL");
        ret = 0;
    }
    if ((ret == 1) && (secs == NULL)) {
        WOLFSSL_MSG("secs is NULL");
        ret = 0;
    }

    if ((ret == 1) && ((from == NULL) && (to == NULL))) {
        *days = 0;
        *secs = 0;
    }
    else if (ret == 1) {
        const long long SECS_PER_DAY = 24 * 60 * 60;
        long long fromSecs;
        long long toSecs = 0;

        ret = wolfssl_asn1_time_to_secs(from, &fromSecs);
        if (ret == 1) {
            ret = wolfssl_asn1_time_to_secs(to, &toSecs);
        }
        if (ret == 1) {
            long long diffSecs = toSecs - fromSecs;
            *days = (int) (diffSecs / SECS_PER_DAY);
            *secs = (int) (diffSecs - ((long long)*days * SECS_PER_DAY));
        }
    }

    return ret;
}

/* Compare two ASN.1 TIME objects by comparing time value.
 *
 * @param [in] a  First ASN.1 TIME object.
 * @param [in] b  Second ASN.1 TIME object.
 * @return Negative value when a is less than b.
 * @return 0 when a equals b.
 * @return Positive value when a is greater than b.
 * @return -2 when a or b is invalid.
 */
int wolfSSL_ASN1_TIME_compare(const WOLFSSL_ASN1_TIME *a,
    const WOLFSSL_ASN1_TIME *b)
{
    int ret;
    int days;
    int secs;

    WOLFSSL_ENTER("wolfSSL_ASN1_TIME_compare");

    /* Calculate difference in time between a and b. */
    if (wolfSSL_ASN1_TIME_diff(&days, &secs, a, b) != 1) {
        WOLFSSL_MSG("Failed to get time difference.");
        ret = -2;
    }
    else if (days == 0 && secs == 0) {
        /* a and b are the same time. */
        ret = 0;
    }
    else if (days >= 0 && secs >= 0) {
        /* a is before b. */
        ret = -1;
    }
    /* Assume wolfSSL_ASN1_TIME_diff creates coherent values. */
    else {
        ret = 1;
    }

    WOLFSSL_LEAVE("wolfSSL_ASN1_TIME_compare", ret);

    return ret;
}

#if !defined(USER_TIME) && !defined(TIME_OVERRIDES)
/* Adjust the time into an ASN.1 TIME object.
 *
 * @param [in] a           ASN.1 TIME object. May be NULL.
 * @param [in] t           Time to offset.
 * @param [in] offset_day  Number of days to offset. May be negative.
 * @param [in] offset_sec  Number of seconds to offset. May be negative.
 * @return  ASN.1 TIME object on success.
 * @return  NULL when formatting time fails.
 * @return  NULL when dynamic memory allocation fails.
 */
WOLFSSL_ASN1_TIME* wolfSSL_ASN1_TIME_adj(WOLFSSL_ASN1_TIME* a, time_t t,
    int offset_day, long offset_sec)
{
    WOLFSSL_ASN1_TIME* ret = NULL;
    const time_t sec_per_day = 24*60*60;
    int time_get;
    char time_str[MAX_TIME_STRING_SZ];
    time_t offset_day_sec = offset_day * sec_per_day;
    time_t t_adj          = t + offset_day_sec + offset_sec;

    WOLFSSL_ENTER("wolfSSL_ASN1_TIME_adj");

    /* Get time string as either UTC or GeneralizedTime. */
    time_get = GetFormattedTime(&t_adj, (byte*)time_str, MAX_TIME_STRING_SZ);
    if (time_get > 0) {
        ret = a;
        if (ret == NULL) {
            ret = wolfSSL_ASN1_TIME_new();
        }
        /* Set the string into the ASN.1 TIME object. */
        if ((wolfSSL_ASN1_TIME_set_string(ret, time_str) != 1) && (ret != a)) {
            wolfSSL_ASN1_TIME_free(ret);
            ret = NULL;
        }
    }

    return ret;
}
#endif /* !USER_TIME && !TIME_OVERRIDES */

/* Get the length of the ASN.1 TIME data.
 *
 * Not an OpenSSL function - ASN1_TIME is not opaque.
 *
 * @param [in] t  ASN.1 TIME object.
 * @return  Length of data on success.
 * @return  0 when t is NULL or no time set.
 */
int wolfSSL_ASN1_TIME_get_length(const WOLFSSL_ASN1_TIME *t)
{
    int len = 0;

    WOLFSSL_ENTER("wolfSSL_ASN1_TIME_get_length");

    if (t != NULL) {
        len = t->length;
    }

    return len;
}

/* Get the data from the ASN.1 TIME object.
 *
 * Not an OpenSSL function - ASN1_TIME is not opaque.
 *
 * @param [in] t  ASN.1 TIME object.
 * @return  Data buffer on success.
 * @return  NULL when t is NULL.
 */
unsigned char* wolfSSL_ASN1_TIME_get_data(const WOLFSSL_ASN1_TIME *t)
{
    unsigned char* data = NULL;

    WOLFSSL_ENTER("wolfSSL_ASN1_TIME_get_data");

    if (t != NULL) {
        data = (unsigned char*)t->data;
    }

    return data;
}

/* Check format of string in ASN.1 TIME object.
 *
 * @param [in] a  ASN.1 TIME object.
 * @return  1 on success.
 * @return  0 when format invalid.
 */
int wolfSSL_ASN1_TIME_check(const WOLFSSL_ASN1_TIME* a)
{
    int ret = 1;
    char buf[MAX_TIME_STRING_SZ];

    WOLFSSL_ENTER("wolfSSL_ASN1_TIME_check");

    /* If can convert to human readable then format good. */
    if (wolfSSL_ASN1_TIME_to_string((WOLFSSL_ASN1_TIME*)a, buf,
            MAX_TIME_STRING_SZ) == NULL) {
        ret = 0;
    }

    return ret;
}

/* Set the time as a string into ASN.1 TIME object.
 *
 * When t is NULL, str is checked only.
 *
 * @param [in, out] t    ASN.1 TIME object.
 * @param [in]      str  Time as a string.
 * @return  1 on success.
 * @return  0 when str is NULL.
 * @return  0 when str is not formatted correctly.
 */
int wolfSSL_ASN1_TIME_set_string(WOLFSSL_ASN1_TIME *t, const char *str)
{
    int ret = 1;
    int slen = 0;

    WOLFSSL_ENTER("wolfSSL_ASN1_TIME_set_string");

    if (str == NULL) {
        WOLFSSL_MSG("Bad parameter");
        ret = 0;
    }
    if (ret == 1) {
        /* Get length of string including NUL terminator. */
        slen = (int)XSTRLEN(str) + 1;
        if (slen > CTC_DATE_SIZE) {
            WOLFSSL_MSG("Date string too long");
            ret = 0;
        }
    }
    if ((ret == 1) && (t != NULL)) {
        /* Copy in string including NUL terminator. */
        XMEMCPY(t->data, str, (size_t)slen);
        /* Do not include NUL terminator in length. */
        t->length = slen - 1;
        /* Set ASN.1 type based on string length. */
        t->type = ((slen == ASN_UTC_TIME_SIZE) ? V_ASN1_UTCTIME :
            V_ASN1_GENERALIZEDTIME);
    }

    return ret;
}

/* Convert ASN.1 TIME object to ASN.1 GENERALIZED TIME object.
 *
 * @param [in]      t    ASN.1 TIME object.
 * @param [in, out] out  ASN.1 GENERALIZED TIME object.
 * @return  ASN.1 GENERALIZED TIME object on success.
 * @return  NULL when t is NULL or t has wrong ASN.1 type.
 * @return  NULL when dynamic memory allocation fails.
 */
WOLFSSL_ASN1_TIME* wolfSSL_ASN1_TIME_to_generalizedtime(WOLFSSL_ASN1_TIME *t,
    WOLFSSL_ASN1_TIME **out)
{
    WOLFSSL_ASN1_TIME *ret = NULL;

    WOLFSSL_ENTER("wolfSSL_ASN1_TIME_to_generalizedtime");

    /* Validate parameters. */
    if (t == NULL) {
        WOLFSSL_MSG("Invalid ASN_TIME value");
    }
    /* Ensure ASN.1 type is one that is supported. */
    else if ((t->type != V_ASN1_UTCTIME) &&
             (t->type != V_ASN1_GENERALIZEDTIME)) {
        WOLFSSL_MSG("Invalid ASN_TIME type.");
    }
    /* Check for ASN.1 GENERALIZED TIME object being passed in. */
    else if ((out != NULL) && (*out != NULL)) {
        /* Copy into the passed in object. */
        ret = *out;
    }
    else {
        /* Create a new ASN.1 GENERALIZED TIME object. */
        ret = wolfSSL_ASN1_TIME_new();
        if (ret == NULL) {
            WOLFSSL_MSG("memory alloc failed.");
        }
    }

    if (ret != NULL) {
        /* Set the ASN.1 type and length of string. */
        ret->type = V_ASN1_GENERALIZEDTIME;
        ret->length = ASN_GENERALIZED_TIME_SIZE;

        if (t->type == V_ASN1_GENERALIZEDTIME) {
            /* Just copy as data already appropriately formatted. */
            XMEMCPY(ret->data, t->data, ASN_GENERALIZED_TIME_SIZE);
        }
        else {
            /* Convert UTC TIME to GENERALIZED TIME. */
            if (t->data[0] >= '5') {
                /* >= 50 is 1900s.  */
                ret->data[0] = '1'; ret->data[1] = '9';
            }
            else {
                /* < 50 is 2000s.  */
                ret->data[0] = '2'; ret->data[1] = '0';
            }
            /* Append rest of the data as it is the same. */
            XMEMCPY(&ret->data[2], t->data, ASN_UTC_TIME_SIZE);
        }

        /* Check for pointer to return result through. */
        if (out != NULL) {
            *out = ret;
        }
    }

    return ret;
}

#endif /* OPENSSL_EXTRA */

#if defined(WOLFSSL_MYSQL_COMPATIBLE) || defined(OPENSSL_EXTRA)
/* Get string from ASN.1 TIME object.
 *
 * Not an OpenSSL compatibility API.
 *
 * @param [in]      t    ASN.1 TIME object.
 * @param [in, out] buf  Buffer to put string in.
 * @param [in]      len  Length of buffer in bytes.
 * @return  buf on success.
 * @return  NULL when t or buf is NULL, or len is less than 5.
 * @return  NULL when ASN.1 TIME length is larger than len.
 * @return  NULL when internal time format not valid.
 */
char* wolfSSL_ASN1_TIME_to_string(WOLFSSL_ASN1_TIME* t, char* buf, int len)
{
    WOLFSSL_ENTER("wolfSSL_ASN1_TIME_to_string");

    /* Validate parameters. */
    if ((t == NULL) || (buf == NULL) || (len < 5)) {
        WOLFSSL_MSG("Bad argument");
        buf = NULL;
    }

    /* Check internal length against passed in length. */
    if ((buf != NULL) && (t->length > len)) {
        WOLFSSL_MSG("Length of date is longer then buffer");
        buf = NULL;
    }

    /* Get time as human readable string. */
    if ((buf != NULL) && !GetTimeString(t->data, t->type, buf, len)) {
        buf = NULL;
    }

    return buf;
}

/* Number of characters in a UTC TIME string. */
#define UTCTIME_LEN     13

/* Get year from UTC TIME string.
 *
 * @param [in]  str   UTC TIME string.
 * @param [in]  len   Length of string in bytes.
 * @param [out] year  Year as extracted from string.
 * @return  1 on success.
 * @return  0 when length is too short for a UTC TIME.
 * @return  0 when not ZULU time.
 */
static int wolfssl_utctime_year(const unsigned char* str, int len, int* year)
{
    int ret = 1;

    /* Check minimal length for UTC TIME. */
    if (len < UTCTIME_LEN) {
        WOLFSSL_MSG("WOLFSSL_ASN1_TIME buffer length is invalid.");
        ret = 0;
    }
    /* Only support ZULU time. */
    if ((ret == 1) && (str[UTCTIME_LEN - 1] != 'Z')) {
        WOLFSSL_MSG("Expecting UTC time.");
        ret = 0;
    }

    if (ret == 1) {
        int tm_year;
        /* 2-digit year. */
        tm_year =  (str[0] - '0') * 10;
        tm_year +=  str[1] - '0';
        /* Check for year being in the 2000s. */
        if (tm_year < 50) {
            tm_year += 100;
        }
        *year = tm_year;
    }

    return ret;
}

/* Number of characters in a GENERALIZED TIME string. */
#define GENTIME_LEN     15

/* Get year from GENERALIZED TIME string.
 *
 * @param [in]  str   GENERALIZED TIME string.
 * @param [in]  len   Length of string in bytes.
 * @param [out] year  Year as extracted from string.
 * @return  1 on success.
 * @return  0 when length is too short for a GENERALIZED TIME.
 * @return  0 when not ZULU time.
 */
static int wolfssl_gentime_year(const unsigned char* str, int len, int* year)
{
    int ret = 1;

    /* Check minimal length for GENERALIZED TIME. */
    if (len < GENTIME_LEN) {
        WOLFSSL_MSG("WOLFSSL_ASN1_TIME buffer length is invalid.");
        ret = 0;
    }
    if ((ret == 1) && (str[GENTIME_LEN - 1] != 'Z')) {
        WOLFSSL_MSG("Expecting Generalized time.");
        ret = 0;
    }

    if (ret == 1) {
        int tm_year;
        /* 4-digit year. */
        tm_year =  (str[0] - '0') * 1000;
        tm_year += (str[1] - '0') * 100;
        tm_year += (str[2] - '0') * 10;
        tm_year +=  str[3] - '0';
        /* Only need value to be years since 1900. */
        tm_year -= 1900;
        *year = tm_year;
    }

    return ret;
}

/* Convert an ASN.1 TIME to a struct tm.
 *
 * @param [in] asnTime  ASN.1 TIME object.
 * @param [in] tm       Broken-down time. Must be non-NULL.
 * @return  1 on success.
 * @return  0 when string format is invalid.
 */
static int wolfssl_asn1_time_to_tm(const WOLFSSL_ASN1_TIME* asnTime,
    struct tm* tm)
{
    int ret = 1;
    const unsigned char* asn1TimeBuf;
    int asn1TimeBufLen;
    int i = 0;
#ifdef XMKTIME
    struct tm localTm;

    XMEMSET(&localTm, 0, sizeof localTm);
#endif

    /* Get the string buffer - fixed array, can't fail. */
    asn1TimeBuf = wolfSSL_ASN1_TIME_get_data(asnTime);
    /* Get the length of the string. */
    asn1TimeBufLen = wolfSSL_ASN1_TIME_get_length(asnTime);
    if (asn1TimeBufLen <= 0) {
        WOLFSSL_MSG("Failed to get WOLFSSL_ASN1_TIME buffer length.");
        ret = 0;
    }
    if (ret == 1) {
        /* Zero out values in broken-down time. */
        XMEMSET(tm, 0, sizeof(struct tm));

        if (asnTime->type == V_ASN1_UTCTIME) {
            /* Get year from UTC TIME string. */
            int tm_year;
            if ((ret = wolfssl_utctime_year(asn1TimeBuf, asn1TimeBufLen,
                    &tm_year)) == 1) {
                tm->tm_year = tm_year;
                /* Month starts after year - 2 characters. */
                i = 2;
            }
        }
        else if (asnTime->type == V_ASN1_GENERALIZEDTIME) {
            /* Get year from GENERALIZED TIME string. */
            int tm_year;
            if ((ret = wolfssl_gentime_year(asn1TimeBuf, asn1TimeBufLen,
                    &tm_year)) == 1) {
                tm->tm_year = tm_year;
                /* Month starts after year - 4 characters. */
                i = 4;
            }
        }
        else {
            /* No other time formats known. */
            WOLFSSL_MSG("asnTime->type is invalid.");
            ret = 0;
        }
     }
     if (ret == 1) {
        /* Fill in rest of broken-down time from string. */
        /* January is 0 not 1 */
        tm->tm_mon   = (asn1TimeBuf[i] - '0') * 10; i++;
        tm->tm_mon  += (asn1TimeBuf[i] - '0') - 1;  i++;
        tm->tm_mday  = (asn1TimeBuf[i] - '0') * 10; i++;
        tm->tm_mday += (asn1TimeBuf[i] - '0');      i++;
        tm->tm_hour  = (asn1TimeBuf[i] - '0') * 10; i++;
        tm->tm_hour += (asn1TimeBuf[i] - '0');      i++;
        tm->tm_min   = (asn1TimeBuf[i] - '0') * 10; i++;
        tm->tm_min  += (asn1TimeBuf[i] - '0');      i++;
        tm->tm_sec   = (asn1TimeBuf[i] - '0') * 10; i++;
        tm->tm_sec  += (asn1TimeBuf[i] - '0');

    #ifdef XMKTIME
        XMEMCPY(&localTm, tm, sizeof(struct tm));
        /* Call XMKTIME on tm to get tm_wday and tm_yday fields populated.
           Note that localTm is used here to avoid modifying other fields,
           such as tm_isdst/tm_gmtoff. */
        XMKTIME(&localTm);
        tm->tm_wday = localTm.tm_wday;
        tm->tm_yday = localTm.tm_yday;
    #endif
    }

    return ret;
}

/* Get the current time into a broken-down time.
 *
 * @param [out] tm  Broken-time time.
 * @return  1 on success.
 * @return  0 when tm is NULL.
 * @return  0 when get current time fails.
 * @return  0 when converting Unix time to broken-down time fails.
 */
static int wolfssl_get_current_time_tm(struct tm* tm)
{
    int ret = 1;
    time_t currentTime;
    struct tm *tmpTs;
#if defined(NEED_TMP_TIME)
    /* for use with gmtime_r */
    struct tm tmpTimeStorage;
    tmpTs = &tmpTimeStorage;
#else
    tmpTs = NULL;
#endif
    (void)tmpTs;

    /* Must have a pointer to return result into. */
    if (tm == NULL) {
        WOLFSSL_MSG("asnTime and tm are both NULL");
        ret = 0;
    }
    if (ret == 1) {
        /* Get current Unix Time GMT. */
        currentTime = wc_Time(0);
        if (currentTime <= 0) {
            WOLFSSL_MSG("Failed to get current time.");
            ret = 0;
        }
    }
    if (ret == 1) {
        /* Convert Unix Time GMT into broken-down time. */
        tmpTs = XGMTIME(&currentTime, tmpTs);
        if (tmpTs == NULL) {
            WOLFSSL_MSG("Failed to convert current time to UTC.");
            ret = 0;
        }
    }
    if (ret == 1) {
        /* Copy from the structure returned into parameter. */
        XMEMCPY(tm, tmpTs, sizeof(*tm));
    }

    return ret;
}

/* Convert ASN.1 TIME object's time into broken-down representation.
 *
 * Internal time string is checked when tm is NULL.
 *
 * @param [in]  asnTime  ASN.1 TIME object.
 * @param [out] tm       Broken-down time.
 * @return  1 on success.
 * @return  0 when asnTime is NULL and tm is NULL.
 * @return  0 getting current time fails.
 * @return  0 when internal time string is invalid.
 */
int wolfSSL_ASN1_TIME_to_tm(const WOLFSSL_ASN1_TIME* asnTime, struct tm* tm)
{
    int ret = 1;

    WOLFSSL_ENTER("wolfSSL_ASN1_TIME_to_tm");

    /* If asnTime is NULL, then the current time is converted. */
    if (asnTime == NULL) {
        ret = wolfssl_get_current_time_tm(tm);
    }
    /* If tm is NULL this function performs a format check on asnTime only. */
    else if (tm == NULL) {
        ret = wolfSSL_ASN1_TIME_check(asnTime);
    }
    else {
        /* Convert ASN.1 TIME to broken-down time. */
        ret = wolfssl_asn1_time_to_tm(asnTime, tm);
    }

    return ret;
}

#ifndef NO_BIO
/* Print the ASN.1 TIME object as a string to BIO.
 *
 * @param [in] bio      BIO to write to.
 * @param [in] asnTime  ASN.1 TIME object.
 * @return  1 on success.
 * @return  0 when bio or asnTime is NULL.
 * @return  0 when creating human readable string fails.
 * @return  0 when writing to BIO fails.
 */
int wolfSSL_ASN1_TIME_print(WOLFSSL_BIO* bio, const WOLFSSL_ASN1_TIME* asnTime)
{
    int  ret = 1;

    WOLFSSL_ENTER("wolfSSL_ASN1_TIME_print");

    /* Validate parameters. */
    if ((bio == NULL) || (asnTime == NULL)) {
        WOLFSSL_MSG("NULL function argument");
        ret = 0;
    }

    if (ret == 1) {
        char buf[MAX_TIME_STRING_SZ];
        int len;

        /* Create human readable string. */
        if (wolfSSL_ASN1_TIME_to_string((WOLFSSL_ASN1_TIME*)asnTime, buf,
                sizeof(buf)) == NULL) {
            /* Write out something anyway but return error. */
            XMEMSET(buf, 0, MAX_TIME_STRING_SZ);
            XSTRNCPY(buf, "Bad time value", sizeof(buf)-1);
            ret = 0;
        }

        /* Write out string. */
        len = (int)XSTRLEN(buf);
        if (wolfSSL_BIO_write(bio, buf, len) != len) {
            WOLFSSL_MSG("Unable to write to bio");
            ret = 0;
        }
    }

    return ret;
}
#endif /* !NO_BIO */

#endif /* WOLFSSL_MYSQL_COMPATIBLE || OPENSSL_EXTRA */

#ifdef OPENSSL_EXTRA

#ifndef NO_BIO
/* Print the ASN.1 UTC TIME object as a string to BIO.
 *
 * @param [in] bio      BIO to write to.
 * @param [in] asnTime  ASN.1 UTC TIME object.
 * @return  1 on success.
 * @return  0 when bio or asnTime is NULL.
 * @return  0 when ASN.1 type is not UTC TIME.
 * @return  0 when creating human readable string fails.
 * @return  0 when writing to BIO fails.
 */
int wolfSSL_ASN1_UTCTIME_print(WOLFSSL_BIO* bio, const WOLFSSL_ASN1_UTCTIME* a)
{
    int ret = 1;

    WOLFSSL_ENTER("wolfSSL_ASN1_UTCTIME_print");

    /* Validate parameters. */
    if ((bio == NULL) || (a == NULL)) {
        ret = 0;
    }
    /* Validate ASN.1 UTC TIME object is of type UTC_TIME. */
    if ((ret == 1) && (a->type != V_ASN1_UTCTIME)) {
        WOLFSSL_MSG("Error, not UTC_TIME");
        ret = 0;
    }

    if (ret == 1) {
        /* Use generic time printing function to do work. */
        ret = wolfSSL_ASN1_TIME_print(bio, a);
    }

    return ret;
}
#endif /* !NO_BIO */

#endif /* OPENSSL_EXTRA */

#endif /* !NO_ASN_TIME */

/*******************************************************************************
 * ASN1_TYPE APIs
 ******************************************************************************/

#if defined(OPENSSL_EXTRA) || defined(WOLFSSL_WPAS_SMALL)

/**
 * Allocate a new ASN.1 TYPE object.
 *
 * @return  New empty ASN.1 TYPE object on success.
 * @return  NULL when dynamic memory allocation fails.
 */
WOLFSSL_ASN1_TYPE* wolfSSL_ASN1_TYPE_new(void)
{
    WOLFSSL_ASN1_TYPE* ret;

    /* Allocate a new ASN.1 TYPE object. */
    ret = (WOLFSSL_ASN1_TYPE*)XMALLOC(sizeof(WOLFSSL_ASN1_TYPE), NULL,
        DYNAMIC_TYPE_OPENSSL);
    if (ret != NULL) {
        /* Clear out fields. */
        XMEMSET(ret, 0, sizeof(WOLFSSL_ASN1_TYPE));
    }

    return ret;
}

/* Free the ASN.1 TYPE object's value field.
 *
 * @param [in, out] at  ASN.1 TYPE object.
 */
static void wolfssl_asn1_type_free_value(WOLFSSL_ASN1_TYPE* at)
{
    switch (at->type) {
        case V_ASN1_NULL:
            break;
        case V_ASN1_OBJECT:
            wolfSSL_ASN1_OBJECT_free(at->value.object);
            break;
        case V_ASN1_UTCTIME:
        #if !defined(NO_ASN_TIME) && defined(OPENSSL_EXTRA)
            wolfSSL_ASN1_TIME_free(at->value.utctime);
        #endif
            break;
        case V_ASN1_GENERALIZEDTIME:
        #if !defined(NO_ASN_TIME) && defined(OPENSSL_EXTRA)
            wolfSSL_ASN1_TIME_free(at->value.generalizedtime);
        #endif
            break;
        case V_ASN1_UTF8STRING:
        case V_ASN1_PRINTABLESTRING:
        case V_ASN1_T61STRING:
        case V_ASN1_IA5STRING:
        case V_ASN1_UNIVERSALSTRING:
        case V_ASN1_SEQUENCE:
            wolfSSL_ASN1_STRING_free(at->value.asn1_string);
            break;
        default:
            break;
    }
}

/**
 * Free ASN.1 TYPE object and its value.
 *
 * @param [in, out] at  ASN.1 TYPE object.
 */
void wolfSSL_ASN1_TYPE_free(WOLFSSL_ASN1_TYPE* at)
{
    if (at != NULL) {
        /* Dispose of value in ASN.1 TYPE object. */
        wolfssl_asn1_type_free_value(at);
    }
    /* Dispose of ASN.1 TYPE object. */
    XFREE(at, NULL, DYNAMIC_TYPE_OPENSSL);
}

#endif /* OPENSSL_EXTRA || WOLFSSL_WPAS_SMALL */

#if defined(OPENSSL_ALL) || defined(OPENSSL_EXTRA) || defined(WOLFSSL_WPAS) || \
    defined(WOLFSSL_WPAS_SMALL)
/**
 * Set ASN.1 TYPE object with a type and value.
 *
 * Type of value for different types:
 *   V_ASN1_NULL            : Value should be NULL.
 *   V_ASN1_OBJECT          : WOLFSSL_ASN1_OBJECT.
 *   V_ASN1_UTCTIME         : WOLFSSL_ASN1_TIME.
 *   V_ASN1_GENERALIZEDTIME : WOLFSSL_ASN1_TIME.
 *   V_ASN1_UTF8STRING      : WOLFSSL_ASN1_STRING.
 *   V_ASN1_PRINTABLESTRING : WOLFSSL_ASN1_STRING.
 *   V_ASN1_T61STRING       : WOLFSSL_ASN1_STRING.
 *   V_ASN1_IA5STRING       : WOLFSSL_ASN1_STRING.
 *   V_ASN1_UNINVERSALSTRING: WOLFSSL_ASN1_STRING.
 *   V_ASN1_SEQUENCE        : WOLFSSL_ASN1_STRING.
 *
 * @param [in, out] a      ASN.1 TYPE object to set.
 * @param [in]      type   ASN.1 type of value.
 * @param [in]      value  Value to store.
 */
void wolfSSL_ASN1_TYPE_set(WOLFSSL_ASN1_TYPE *a, int type, void *value)
{
    if (a != NULL) {
        switch (type) {
            case V_ASN1_NULL:
                if (value != NULL) {
                    WOLFSSL_MSG("NULL tag meant to be always empty!");
                    /* No way to return error - value will not be used. */
                }
                FALL_THROUGH;
            case V_ASN1_OBJECT:
            case V_ASN1_UTCTIME:
            case V_ASN1_GENERALIZEDTIME:
            case V_ASN1_UTF8STRING:
            case V_ASN1_PRINTABLESTRING:
            case V_ASN1_T61STRING:
            case V_ASN1_IA5STRING:
            case V_ASN1_UNIVERSALSTRING:
            case V_ASN1_SEQUENCE:
                /* Dispose of any value currently set. */
                wolfssl_asn1_type_free_value(a);
                /* Assign anonymously typed input to anonymously typed field. */
                a->value.ptr = (char *)value;
                /* Set the type required. */
                a->type = type;
                break;
            default:
                WOLFSSL_MSG("Unknown or unsupported ASN1_TYPE");
                /* No way to return error. */
        }
    }
}

#endif /* OPENSSL_ALL || OPENSSL_EXTRA || WOLFSSL_WPAS */

#endif /* !NO_ASN */

#endif /* !WOLFSSL_SSL_ASN1_INCLUDED */

