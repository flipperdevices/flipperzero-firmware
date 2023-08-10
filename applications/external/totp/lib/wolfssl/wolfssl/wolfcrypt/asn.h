/* asn.h
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

/*!
    \file wolfssl/wolfcrypt/asn.h
*/

/*

DESCRIPTION
This library provides the interface to Abstract Syntax Notation One (ASN.1) objects.
ASN.1 is a standard interface description language for defining data structures
that can be serialized and deserialized in a cross-platform way.

*/
#ifndef WOLF_CRYPT_ASN_H
#define WOLF_CRYPT_ASN_H

#include <wolfssl/wolfcrypt/types.h>

#ifndef NO_ASN


#if !defined(NO_ASN_TIME) && defined(NO_TIME_H)
    #define NO_ASN_TIME /* backwards compatibility with NO_TIME_H */
#endif

#include <wolfssl/wolfcrypt/wolfmath.h>

#ifndef NO_DH
    #include <wolfssl/wolfcrypt/dh.h>
#endif
#ifndef NO_DSA
    #include <wolfssl/wolfcrypt/dsa.h>
#endif
#ifndef NO_SHA
    #include <wolfssl/wolfcrypt/sha.h>
#endif
#ifndef NO_MD5
    #include <wolfssl/wolfcrypt/md5.h>
#endif
#include <wolfssl/wolfcrypt/sha256.h>
#ifdef WOLFSSL_SM3
    #include <wolfssl/wolfcrypt/sm3.h>
#endif
#include <wolfssl/wolfcrypt/asn_public.h>   /* public interface */

#if defined(NO_SHA) && defined(NO_SHA256)
    #define WC_SHA256_DIGEST_SIZE 32
#endif

#ifdef __cplusplus
    extern "C" {
#endif

#ifndef EXTERNAL_SERIAL_SIZE
    #define EXTERNAL_SERIAL_SIZE 32
#endif

enum {
    ISSUER  = 0,
    SUBJECT = 1,

    BEFORE  = 0,
    AFTER   = 1
};

/* ASN Tags   */
enum ASN_Tags {
    ASN_EOC               = 0x00,
    ASN_BOOLEAN           = 0x01,
    ASN_INTEGER           = 0x02,
    ASN_BIT_STRING        = 0x03,
    ASN_OCTET_STRING      = 0x04,
    ASN_TAG_NULL          = 0x05,
    ASN_OBJECT_ID         = 0x06,
    ASN_OBJECT_DESC       = 0x07,
    ASN_INSTANCE_OF       = 0x08,
    ASN_REAL              = 0x09,
    ASN_ENUMERATED        = 0x0a,
    ASN_EMBEDDED_PDV      = 0x0b,
    ASN_UTF8STRING        = 0x0c,
    ASN_RELATIVE_OID      = 0x0d,
    ASN_SEQUENCE          = 0x10,
    ASN_SET               = 0x11,
    ASN_NUMERICSTRING     = 0x12,
    ASN_PRINTABLE_STRING  = 0x13,
    ASN_T61STRING         = 0x14,
    ASN_VIDEOTEXSTRING    = 0x15,
    ASN_IA5_STRING        = 0x16,
    ASN_UTC_TIME          = 0x17,
    ASN_GENERALIZED_TIME  = 0x18,
    ASN_GRAPHICSTRING     = 0x19,
    ASN_ISO646STRING      = 0x1a,
    ASN_GENERALSTRING     = 0x1b,
    ASN_UNIVERSALSTRING   = 0x1c,
    ASN_CHARACTER_STRING  = 0x1d,
    ASN_BMPSTRING         = 0x1e,
    ASN_TYPE_MASK         = 0x1f,

    ASN_LONG_LENGTH       = 0x80,
    ASN_INDEF_LENGTH      = 0x80,

    /* ASN_Flags - Bitmask */
    ASN_CONSTRUCTED       = 0x20,
    ASN_APPLICATION       = 0x40,
    ASN_CONTEXT_SPECIFIC  = 0x80,
    ASN_PRIVATE           = 0xC0,
    ASN_CLASS_MASK        = 0xC0,

    CRL_EXTENSIONS        = 0xa0,
    ASN_EXTENSIONS        = 0xa3,

    /* GeneralName types */
    ASN_OTHER_TYPE        = 0x00,
    ASN_RFC822_TYPE       = 0x01,
    ASN_DNS_TYPE          = 0x02,
    ASN_DIR_TYPE          = 0x04,
    ASN_URI_TYPE          = 0x06, /* the value 6 is from GeneralName OID */
    ASN_IP_TYPE           = 0x07, /* the value 7 is from GeneralName OID */
    ASN_RID_TYPE          = 0x08,

    /* PKCS #7 types */
    ASN_ENC_CONTENT       = 0x00,
    ASN_OTHERNAME_VALUE   = 0x00,

    /* AuthorityKeyIdentifier fields */
    ASN_AUTHKEYID_KEYID   = 0x00,
    ASN_AUTHKEYID_ISSUER  = 0x01,
    ASN_AUTHKEYID_SERIAL  = 0x02,

    /* GeneralSubtree fields */
    ASN_SUBTREE_MIN       = 0x00,
    ASN_SUBTREE_MAX       = 0x01,

    /* x509 Cert Fields */
    ASN_X509_CERT_VERSION = 0x00,

    /* x509 Cert Extension Fields */
    ASN_AKID_KEYID        = 0x00,

    /* ECC Key Fields */
    ASN_ECC_PARAMS        = 0x00,
    ASN_ECC_PUBKEY        = 0x01,

    /* OneAsymmetricKey Fields */
    ASN_ASYMKEY_ATTRS     = 0x00,
    ASN_ASYMKEY_PUBKEY    = 0x01
};

/* NOTE: If ASN_UTC_TIME_SIZE or ASN_GENERALIZED_TIME_SIZE are ever modified
 *       one needs to update the logic in asn.c function GetAsnTimeString()
 *       which depends on the size 14 and/or 16 to determine which format to
 *       place in the "buf" (output)
 */
#define ASN_UTC_TIME_SIZE 14 /* Read note above before modifying */
#define ASN_GENERALIZED_TIME_SIZE 16 /* Read note above before modifying */
#define ASN_GENERALIZED_TIME_MAX 68

#ifdef WOLFSSL_ASN_TEMPLATE
/* Different data types that can be stored in ASNGetData/ASNSetData. */
enum ASNItem_DataType {
    /* Default for tag type. */
    ASN_DATA_TYPE_NONE           = 0,
    /* 8-bit integer value. */
    ASN_DATA_TYPE_WORD8          = 1,
    /* 16-bit integer value. */
    ASN_DATA_TYPE_WORD16         = 2,
    /* 32-bit integer value. */
    ASN_DATA_TYPE_WORD32         = 4,
    /* Buffer with data and length. */
    ASN_DATA_TYPE_BUFFER         = 5,
    /* An expected/required buffer with data and length. */
    ASN_DATA_TYPE_EXP_BUFFER     = 6,
    /* Replace the item with buffer (data and length). */
    ASN_DATA_TYPE_REPLACE_BUFFER = 7,
    /* Big number as an mp_int. */
    ASN_DATA_TYPE_MP             = 8,
    /* Big number as an mp_int that has already been initialized. */
    ASN_DATA_TYPE_MP_INITED      = 9,
    /* Big number as a positive or negative mp_int. */
    ASN_DATA_TYPE_MP_POS_NEG     = 10,
    /* ASN.1 CHOICE. A 0 terminated list of tags that are valid. */
    ASN_DATA_TYPE_CHOICE         = 11
};

/* A template entry describing an ASN.1 item. */
typedef struct ASNItem {
    /* Depth of ASN.1 item - how many constructed ASN.1 items above. */
    byte depth;
    /* BER/DER tag to expect. */
    byte tag;
    /* Whether the ASN.1 item is constructed. */
    byte constructed:1;
    /* Whether to parse the header only or skip data. If
     * ASNSetData.data.buffer.data is supplied then this option gets
     * overwritten and the child nodes get ignored. */
    byte headerOnly:1;
    /* Whether ASN.1 item is optional.
     *  - 0 means not optional
     *  - 1 means is optional
     *  - 2+ means one of these at the same level with same value must appear.
     */
    byte optional;
} ASNItem;

/* Dynamic data for setting (encoding) an ASN.1 item. */
typedef struct ASNSetData {
    /* Reverse offset into buffer of ASN.1 item - calculated in SizeASN_Items().
     * SetASN_Items() subtracts from total length to get usable value.
     */
    word32 offset;
    /* Length of data in ASN.1 item - calculated in SizeASN_Items(). */
    word32 length;
    /* Different data type representation. */
    union {
        /* 8-bit integer value. */
        byte    u8;
        /* 16-bit integer value. */
        word16  u16;
        /* 32-bit integer value. */
        word32  u32;
        /* Big number as an mp_int. */
        mp_int* mp;
        /* Buffer as data pointer and length. */
        struct {
            /* Data to write out. */
            const byte* data;
            /* Length of data to write out. */
            word32      length;
        } buffer;
    } data;
    /* Type of data stored in data field - enum ASNItem_DataType. */
    byte   dataType;
    /* Don't write this ASN.1 item out.
     * Optional items are dependent on the data being encoded.
     */
    byte   noOut;
} ASNSetData;

/* Dynamic data for getting (decoding) an ASN.1 item. */
typedef struct ASNGetData {
    /* Offset into buffer where encoding starts. */
    word32 offset;
    /* Total length of data in ASN.1 item.
     * BIT_STRING and INTEGER lengths include leading byte. */
    word32 length;
    union {
        /* Pointer to 8-bit integer. */
        byte*       u8;
        /* Pointer to 16-bit integer. */
        word16*     u16;
        /* Pointer to 32-bit integer. */
        word32*     u32;
        /* Pointer to mp_int for big number. */
        mp_int*     mp;
        /* List of possible tags. Useful for CHOICE ASN.1 items. */
        const byte* choice;
        /* Buffer to copy into. */
        struct {
            /* Buffer to hold ASN.1 data. */
            byte*   data;
            /* Maximum length of buffer. */
            word32* length;
        } buffer;
        /* Reference to ASN.1 item's data. */
        struct {
            /* Pointer reference into input buffer. */
            const byte* data;
            /* Length of data. */
            word32      length;
        } ref;
        /* Data of an OBJECT_ID. */
        struct {
            /* OID data reference into input buffer. */
            const byte* data;
            /* Length of OID data. */
            word32      length;
            /* Type of OID expected. */
            word32      type;
            /* OID sum - 32-bit id. */
            word32      sum;
        } oid;
    } data;
    /* Type of data stored in data field - enum ASNItem_DataType. */
    byte dataType;
    /* Tag found in BER/DER item. */
    byte tag;
} ASNGetData;

WOLFSSL_LOCAL int SizeASN_Items(const ASNItem* asn, ASNSetData *data,
    int count, int* encSz);
WOLFSSL_LOCAL int SetASN_Items(const ASNItem* asn, ASNSetData *data, int count,
    byte* output);
WOLFSSL_LOCAL int GetASN_Items(const ASNItem* asn, ASNGetData *data, int count,
    int complete, const byte* input, word32* inOutIdx, word32 maxIdx);

#ifdef WOLFSSL_ASN_TEMPLATE_TYPE_CHECK
WOLFSSL_LOCAL void GetASN_Int8Bit(ASNGetData *dataASN, byte* num);
WOLFSSL_LOCAL void GetASN_Int16Bit(ASNGetData *dataASN, word16* num);
WOLFSSL_LOCAL void GetASN_Int32Bit(ASNGetData *dataASN, word32* num);
WOLFSSL_LOCAL void GetASN_Buffer(ASNGetData *dataASN, byte* data,
    word32* length);
WOLFSSL_LOCAL void GetASN_ExpBuffer(ASNGetData *dataASN, const byte* data,
    word32 length);
WOLFSSL_LOCAL void GetASN_MP(ASNGetData *dataASN, mp_int* num);
WOLFSSL_LOCAL void GetASN_MP_Inited(ASNGetData *dataASN, mp_int* num);
WOLFSSL_LOCAL void GetASN_MP_PosNeg(ASNGetData *dataASN, mp_int* num);
WOLFSSL_LOCAL void GetASN_Choice(ASNGetData *dataASN, const byte* options);
WOLFSSL_LOCAL void GetASN_Boolean(ASNGetData *dataASN, byte* num);
WOLFSSL_LOCAL void GetASN_OID(ASNGetData *dataASN, int oidType);
WOLFSSL_LOCAL void GetASN_GetConstRef(ASNGetData * dataASN, const byte** data,
    word32* length);
WOLFSSL_LOCAL void GetASN_GetRef(ASNGetData * dataASN, byte** data,
    word32* length);
WOLFSSL_LOCAL void GetASN_OIDData(ASNGetData * dataASN, byte** data,
    word32* length);
WOLFSSL_LOCAL void SetASN_Boolean(ASNSetData *dataASN, byte val);
WOLFSSL_LOCAL void SetASN_Int8Bit(ASNSetData *dataASN, byte num);
WOLFSSL_LOCAL void SetASN_Int16Bit(ASNSetData *dataASN, word16 num);
WOLFSSL_LOCAL void SetASN_Buffer(ASNSetData *dataASN, const byte* data,
    word32 length);
WOLFSSL_LOCAL void SetASN_ReplaceBuffer(ASNSetData *dataASN, const byte* data,
    word32 length);
WOLFSSL_LOCAL void SetASN_MP(ASNSetData *dataASN, mp_int* num);
WOLFSSL_LOCAL void SetASN_OID(ASNSetData *dataASN, int oid, int oidType);
#else
/* Setup ASN data item to get an 8-bit number.
 *
 * @param [in] dataASN  Dynamic ASN data item.
 * @param [in] num      Pointer to an 8-bit variable.
 */
#define GetASN_Int8Bit(dataASN, num)                                   \
    do {                                                               \
        (dataASN)->dataType = ASN_DATA_TYPE_WORD8;                     \
        (dataASN)->data.u8  = num;                                     \
    } while (0)

/* Setup ASN data item to get a 16-bit number.
 *
 * @param [in] dataASN  Dynamic ASN data item.
 * @param [in] num      Pointer to a 16-bit variable.
 */
#define GetASN_Int16Bit(dataASN, num)                                  \
    do {                                                               \
        (dataASN)->dataType = ASN_DATA_TYPE_WORD16;                    \
        (dataASN)->data.u16 = num;                                     \
    } while (0)

/* Setup ASN data item to get a 32-bit number.
 *
 * @param [in] dataASN  Dynamic ASN data item.
 * @param [in] num      Pointer to a 32-bit variable.
 */
#define GetASN_Int32Bit(dataASN, num)                                  \
    do {                                                               \
        (dataASN)->dataType = ASN_DATA_TYPE_WORD32;                    \
        (dataASN)->data.u32 = num;                                     \
    } while (0)

/* Setup ASN data item to get data into a buffer of a specific length.
 *
 * @param [in] dataASN  Dynamic ASN data item.
 * @param [in] d        Buffer to hold data.
 * @param [in] l        Length of buffer in bytes.
 */
#define GetASN_Buffer(dataASN, d, l)                                   \
    do {                                                               \
        (dataASN)->dataType           = ASN_DATA_TYPE_BUFFER;          \
        (dataASN)->data.buffer.data   = d;                             \
        (dataASN)->data.buffer.length = l;                             \
    } while (0)

/* Setup ASN data item to check parsed data against expected buffer.
 *
 * @param [in] dataASN  Dynamic ASN data item.
 * @param [in] d        Buffer containing expected data.
 * @param [in] l        Length of buffer in bytes.
 */
#define GetASN_ExpBuffer(dataASN, d, l)                                \
    do {                                                               \
        (dataASN)->dataType        = ASN_DATA_TYPE_EXP_BUFFER;         \
        (dataASN)->data.ref.data   = d;                                \
        (dataASN)->data.ref.length = l;                                \
    } while (0)

/* Setup ASN data item to get a number into an mp_int.
 *
 * @param [in] dataASN  Dynamic ASN data item.
 * @param [in] num      Multi-precision number object.
 */
#define GetASN_MP(dataASN, num)                                        \
    do {                                                               \
        (dataASN)->dataType = ASN_DATA_TYPE_MP;                        \
        (dataASN)->data.mp  = num;                                     \
    } while (0)

/* Setup ASN data item to get a number into an mp_int that is initialized.
 *
 * @param [in] dataASN  Dynamic ASN data item.
 * @param [in] num      Multi-precision number object.
 */
#define GetASN_MP_Inited(dataASN, num)                                 \
    do {                                                               \
        (dataASN)->dataType = ASN_DATA_TYPE_MP_INITED;                 \
        (dataASN)->data.mp  = num;                                     \
    } while (0)

/* Setup ASN data item to get a positive or negative number into an mp_int.
 *
 * @param [in] dataASN  Dynamic ASN data item.
 * @param [in] num      Multi-precision number object.
 */
#define GetASN_MP_PosNeg(dataASN, num)                                 \
    do {                                                               \
        (dataASN)->dataType = ASN_DATA_TYPE_MP_POS_NEG;                \
        (dataASN)->data.mp  = num;                                     \
    } while (0)

/* Setup ASN data item to be a choice of tags.
 *
 * @param [in] dataASN  Dynamic ASN data item.
 * @param [in] choice   0 terminated list of tags that are valid.
 */
#define GetASN_Choice(dataASN, options)                                \
    do {                                                               \
        (dataASN)->dataType    = ASN_DATA_TYPE_CHOICE;                 \
        (dataASN)->data.choice = options;                              \
    } while (0)

/* Setup ASN data item to get a boolean value.
 *
 * @param [in] dataASN  Dynamic ASN data item.
 * @param [in] num      Pointer to an 8-bit variable.
 */
#define GetASN_Boolean(dataASN, num)                                   \
    do {                                                               \
        (dataASN)->dataType = ASN_DATA_TYPE_NONE;                      \
        (dataASN)->data.u8  = num;                                     \
    } while (0)

/* Setup ASN data item to be a an OID of a specific type.
 *
 * @param [in] dataASN  Dynamic ASN data item.
 * @param [in] oidType  Type of OID to expect.
 */
#define GetASN_OID(dataASN, oidType)                                   \
    (dataASN)->data.oid.type = oidType

/* Get the data and length from an ASN data item.
 *
 * @param [in]  dataASN  Dynamic ASN data item.
 * @param [out] d        Pointer to data of item.
 * @param [out] l        Length of buffer in bytes.
 */
#define GetASN_GetConstRef(dataASN, d, l)                              \
    do {                                                               \
        *(d) = (dataASN)->data.ref.data;                               \
        *(l) = (dataASN)->data.ref.length;                             \
    } while (0)

/* Get the data and length from an ASN data item.
 *
 * @param [in]  dataASN  Dynamic ASN data item.
 * @param [out] d        Pointer to data of item.
 * @param [out] l        Length of buffer in bytes.
 */
#define GetASN_GetRef(dataASN, d, l)                                   \
    do {                                                               \
        *(d) = (byte*)(dataASN)->data.ref.data;                        \
        *(l) =        (dataASN)->data.ref.length;                      \
    } while (0)

/* Get the data and length from an ASN data item that is an OID.
 *
 * @param [in]  dataASN  Dynamic ASN data item.
 * @param [out] d        Pointer to .
 * @param [out] l        Length of buffer in bytes.
 */
#define GetASN_OIDData(dataASN, d, l)                                  \
    do {                                                               \
        *(d) = (byte*)(dataASN)->data.oid.data;                        \
        *(l) =        (dataASN)->data.oid.length;                      \
    } while (0)

/* Setup an ASN data item to set a boolean.
 *
 * @param [in] dataASN  Dynamic ASN data item.
 * @param [in] val      Boolean value.
 */
#define SetASN_Boolean(dataASN, val)                                   \
    do {                                                               \
        (dataASN)->dataType = ASN_DATA_TYPE_NONE;                      \
        (dataASN)->data.u8  = val;                                     \
    } while (0)

/* Setup an ASN data item to set an 8-bit number.
 *
 * @param [in] dataASN  Dynamic ASN data item.
 * @param [in] num      8-bit number to set.
 */
#define SetASN_Int8Bit(dataASN, num)                                   \
    do {                                                               \
        (dataASN)->dataType = ASN_DATA_TYPE_WORD8;                     \
        (dataASN)->data.u8  = num;                                     \
    } while (0)

/* Setup an ASN data item to set a 16-bit number.
 *
 * @param [in] dataASN  Dynamic ASN data item.
 * @param [in] num      16-bit number to set.
 */
#define SetASN_Int16Bit(dataASN, num)                                  \
    do {                                                               \
        (dataASN)->dataType = ASN_DATA_TYPE_WORD16;                    \
        (dataASN)->data.u16 = num;                                     \
    } while (0)

/* Setup an ASN data item to set the data in a buffer.
 *
 * @param [in] dataASN  Dynamic ASN data item.
 * @param [in] d        Buffer containing data to set.
 * @param [in] l        Length of data in buffer in bytes.
 */
#define SetASN_Buffer(dataASN, d, l)                                   \
    do {                                                               \
        (dataASN)->data.buffer.data   = d;                             \
        (dataASN)->data.buffer.length = l;                             \
    } while (0)

/* Setup an ASN data item to set the DER encode data in a buffer.
 *
 * @param [in] dataASN  Dynamic ASN data item.
 * @param [in] d        Buffer containing BER encoded data to set.
 * @param [in] l        Length of data in buffer in bytes.
 */
#define SetASN_ReplaceBuffer(dataASN, d, l)                            \
    do {                                                               \
        (dataASN)->dataType           = ASN_DATA_TYPE_REPLACE_BUFFER;  \
        (dataASN)->data.buffer.data   = d;                             \
        (dataASN)->data.buffer.length = l;                             \
    } while (0)

/* Setup an ASN data item to set an muli-precision number.
 *
 * @param [in] dataASN  Dynamic ASN data item.
 * @param [in] num      Multi-precision number.
 */
#define SetASN_MP(dataASN, num)                                        \
    do {                                                               \
        (dataASN)->dataType = ASN_DATA_TYPE_MP;                        \
        (dataASN)->data.mp  = num;                                     \
    } while (0)

/* Setup an ASN data item to set an OID based on id and type.
 *
 * oid and oidType pair are unique.
 *
 * @param [in] dataASN  Dynamic ASN data item.
 * @param [in] oid      OID identifier.
 * @param [in] oidType  Type of OID.
 */
#define SetASN_OID(dataASN, oid, oidType)                              \
    (dataASN)->data.buffer.data = OidFromId(oid, oidType,              \
                                       &(dataASN)->data.buffer.length)
#endif /* WOLFSSL_ASN_TEMPLATE_TYPE_CHECK */


/* Get address at the start of the BER item.
 *
 * @param [in] dataASN  Dynamic ASN data item.
 * @param [in] in       Input buffer.
 * @return  Address at start of BER item.
 */
#define GetASNItem_Addr(dataASN, in)                                   \
    ((in) + (dataASN).offset)

/* Get length of a BER item - including tag and length.
 *
 * @param [in] dataASN  Dynamic ASN data item.
 * @param [in] in       Input buffer.
 * @return  Length of a BER item.
 */
#define GetASNItem_Length(dataASN, in)                                 \
    ((dataASN).length + (word32)((dataASN).data.buffer.data - (in)) -  \
                                                     (dataASN).offset)

/* Get the index of a BER item's data.
 *
 * @param [in] dataASN  Dynamic ASN data item.
 * @param [in] in       Input buffer.
 * @return  Index of a BER item's data.
 */
#define GetASNItem_DataIdx(dataASN, in)                                \
    (word32)((dataASN).data.ref.data - (in))

/* Get the end index of a BER item - index of the start of the next item.
 *
 * @param [in] dataASN  Dynamic ASN data item.
 * @param [in] in       Input buffer.
 * @return  End index of a BER item.
 */
#define GetASNItem_EndIdx(dataASN, in)                                 \
    ((word32)((dataASN).data.ref.data - (in)) +                        \
                                            (dataASN).data.ref.length)

/* For a BIT_STRING, get the unused bits byte.
 *
 * @param [in] dataASN  Dynamic ASN data item.
 * @return  Unused bits byte in BIT_STRING.
 */
#define GetASNItem_UnusedBits(dataASN)                                 \
    (*((dataASN).data.ref.data - 1))

/* Set the data items at indices start to end inclusive to not be encoded.
 *
 * @param [in] dataASN  Dynamic ASN data item.
 * @param [in] start    First item not to be encoded.
 * @param [in] end      Last item not to be encoded.
 */
#define SetASNItem_NoOut(dataASN, start, end)                          \
    do {                                                               \
        int ii;                                                        \
        for (ii = (start); ii <= (end); ii++) {                        \
            (dataASN)[ii].noOut = 1;                                   \
        }                                                              \
    }                                                                  \
    while (0)

/* Set the data items below node to not be encoded.
 *
 * @param [in] dataASN  Dynamic ASN data item.
 * @param [in] node     Node who's children should not be encoded.
 * @param [in] dataASNLen Number of items in dataASN.
 */
#define SetASNItem_NoOutBelow(dataASN, asn, node, dataASNLen)          \
    do {                                                               \
        int ii;                                                        \
        for (ii = (node) + 1; ii < (int)(dataASNLen); ii++) {          \
            if ((asn)[ii].depth <= (asn)[node].depth)                  \
                break;                                                 \
            (dataASN)[ii].noOut = 1;                                   \
        }                                                              \
    }                                                                  \
    while (0)

/* Set the node and all nodes below to not be encoded.
 *
 * @param [in] dataASN  Dynamic ASN data item.
 * @param [in] node     Node which should not be encoded. Child nodes will
 *                      also not be encoded.
 * @param [in] dataASNLen Number of items in dataASN.
 */
#define SetASNItem_NoOutNode(dataASN, asn, node, dataASNLen)           \
    do {                                                               \
        int ii;                                                        \
        (dataASN)[node].noOut = 1;                                     \
        for (ii = (node) + 1; ii < (int)(dataASNLen); ii++) {          \
            if ((asn)[ii].depth <= (asn)[node].depth)                  \
                break;                                                 \
            (dataASN)[ii].noOut = 1;                                   \
        }                                                              \
    }                                                                  \
    while (0)

#endif /* WOLFSSL_ASN_TEMPLATE */


enum DN_Tags {
    ASN_DN_NULL       = 0x00,
    ASN_COMMON_NAME   = 0x03,   /* CN */
    ASN_SUR_NAME      = 0x04,   /* SN */
    ASN_SERIAL_NUMBER = 0x05,   /* serialNumber */
    ASN_COUNTRY_NAME  = 0x06,   /* C  */
    ASN_LOCALITY_NAME = 0x07,   /* L  */
    ASN_STATE_NAME    = 0x08,   /* ST */
    ASN_STREET_ADDR   = 0x09,   /* street */
    ASN_ORG_NAME      = 0x0a,   /* O  */
    ASN_ORGUNIT_NAME  = 0x0b,   /* OU */
    ASN_BUS_CAT       = 0x0f,   /* businessCategory */
    ASN_POSTAL_CODE   = 0x11,   /* postalCode */
    ASN_USER_ID       = 0x12,   /* UserID */
#ifdef WOLFSSL_CERT_NAME_ALL
    ASN_NAME          = 0x29,   /* name */
    ASN_GIVEN_NAME    = 0x2a,   /* GN */
    ASN_INITIALS      = 0x2b,   /* initials */
    ASN_DNQUALIFIER   = 0x2e,   /* dnQualifier */
#endif /* WOLFSSL_CERT_NAME_ALL */


    ASN_CONTENT_TYPE  = 0x97, /* not actual OID (see attrPkcs9ContentTypeOid) */
    ASN_EMAIL_NAME    = 0x98, /* not actual OID (see attrEmailOid) */
    ASN_CUSTOM_NAME   = 0x99, /* not actual OID (see CertOidField) */

    /* pilot attribute types
     * OID values of 0.9.2342.19200300.100.1.* */
    ASN_FAVOURITE_DRINK  = 0x13, /* favouriteDrink */
    ASN_DOMAIN_COMPONENT = 0x19  /* DC */
};

/* This is the size of the smallest possible PEM header and footer */
extern const int pem_struct_min_sz;

#if defined(OPENSSL_EXTRA) || defined(OPENSSL_EXTRA_X509_SMALL)
typedef struct WOLFSSL_ObjectInfo {
    int nid;
    int id;
    word32 type;
    const char* sName;
    const char* lName;
} WOLFSSL_ObjectInfo;
extern const size_t wolfssl_object_info_sz;
extern const WOLFSSL_ObjectInfo wolfssl_object_info[];
#endif /* defined(OPENSSL_EXTRA) || defined(OPENSSL_EXTRA_X509_SMALL) */

/* DN Tag Strings */
#define WOLFSSL_COMMON_NAME      "/CN="
#define WOLFSSL_LN_COMMON_NAME   "/commonName="
#define WOLFSSL_SUR_NAME         "/SN="
#ifdef WOLFSSL_CERT_NAME_ALL
    #define WOLFSSL_NAME             "/N="
    #define WOLFSSL_INITIALS         "/initials="
    #define WOLFSSL_GIVEN_NAME       "/GN="
    #define WOLFSSL_DNQUALIFIER      "/dnQualifier="
#endif /* WOLFSSL_CERT_NAME_ALL */
#define WOLFSSL_SERIAL_NUMBER    "/serialNumber="
#define WOLFSSL_COUNTRY_NAME     "/C="
#define WOLFSSL_LN_COUNTRY_NAME  "/countryName="
#define WOLFSSL_LOCALITY_NAME    "/L="
#define WOLFSSL_LN_LOCALITY_NAME "/localityName="
#define WOLFSSL_STATE_NAME       "/ST="
#define WOLFSSL_LN_STATE_NAME    "/stateOrProvinceName="
#define WOLFSSL_STREET_ADDR_NAME "/street="
#define WOLFSSL_LN_STREET_ADDR_NAME "/streetAddress="
#define WOLFSSL_POSTAL_NAME      "/postalCode="
#define WOLFSSL_ORG_NAME         "/O="
#define WOLFSSL_LN_ORG_NAME      "/organizationName="
#define WOLFSSL_ORGUNIT_NAME     "/OU="
#define WOLFSSL_LN_ORGUNIT_NAME  "/organizationalUnitName="
#define WOLFSSL_DOMAIN_COMPONENT "/DC="
#define WOLFSSL_LN_DOMAIN_COMPONENT "/domainComponent="
#define WOLFSSL_BUS_CAT          "/businessCategory="
#define WOLFSSL_JOI_C            "/jurisdictionC="
#define WOLFSSL_JOI_ST           "/jurisdictionST="
#define WOLFSSL_EMAIL_ADDR       "/emailAddress="

#define WOLFSSL_USER_ID          "/UID="
#define WOLFSSL_DOMAIN_COMPONENT "/DC="
#define WOLFSSL_FAVOURITE_DRINK  "/favouriteDrink="
#define WOLFSSL_CONTENT_TYPE     "/contentType="

#if defined(WOLFSSL_APACHE_HTTPD)
    /* otherName strings */
    #define WOLFSSL_SN_MS_UPN       "msUPN"
    #define WOLFSSL_LN_MS_UPN       "Microsoft User Principal Name"
    #define WOLFSSL_MS_UPN_SUM 265
    #define WOLFSSL_SN_DNS_SRV      "id-on-dnsSRV"
    #define WOLFSSL_LN_DNS_SRV      "SRVName"
    /* TLS features extension strings */
    #define WOLFSSL_SN_TLS_FEATURE  "tlsfeature"
    #define WOLFSSL_LN_TLS_FEATURE  "TLS Feature"
    #define WOLFSSL_TLS_FEATURE_SUM 92
#endif

#if defined(OPENSSL_EXTRA) || defined(OPENSSL_EXTRA_X509_SMALL)
/* NIDs */
#define NID_undef 0
#define NID_netscape_cert_type NID_undef
#define NID_des 66
#define NID_des3 67
#define NID_sha256 672
#define NID_sha384 673
#define NID_sha512 674
#define NID_sha512_224 1094
#define NID_sha512_256 1095
#define NID_pkcs7_signed 22
#define NID_pkcs7_enveloped 23
#define NID_pkcs7_signedAndEnveloped 24
#define NID_pkcs9_unstructuredName 49
#define NID_pkcs9_contentType 50  /* 1.2.840.113549.1.9.3 */
#define NID_pkcs9_challengePassword 54
#define NID_hw_name_oid 73
#define NID_id_pkix_OCSP_basic 74
#define NID_any_policy 75
#define NID_anyExtendedKeyUsage 76
#define NID_givenName 100  /* 2.5.4.42 */
#define NID_initials 101  /* 2.5.4.43 */
#define NID_title 106
#define NID_description 107
#define NID_basic_constraints 133
#define NID_key_usage 129      /* 2.5.29.15 */
#define NID_ext_key_usage 151  /* 2.5.29.37 */
#define NID_subject_key_identifier 128
#define NID_authority_key_identifier 149
#define NID_private_key_usage_period 130  /* 2.5.29.16 */
#define NID_subject_alt_name 131
#define NID_issuer_alt_name 132
#define NID_info_access 69
#define NID_sinfo_access 79       /* id-pe 11 */
#define NID_name_constraints 144  /* 2.5.29.30 */
#define NID_crl_distribution_points 145  /* 2.5.29.31 */
#define NID_certificate_policies 146
#define NID_policy_mappings 147
#define NID_policy_constraints 150
#define NID_inhibit_any_policy 168       /* 2.5.29.54 */
#define NID_tlsfeature 1020              /* id-pe 24 */
#define NID_buildingName 1494

#define NID_dnQualifier 174              /* 2.5.4.46 */
#define NID_commonName 14                /* CN Changed to not conflict
                                    * with PBE_SHA1_DES3 */
#define NID_name 173                     /* N , OID = 2.5.4.41 */
#define NID_surname 0x04                 /* SN */
#define NID_serialNumber 0x05            /* serialNumber */
#define NID_countryName 0x06             /* C  */
#define NID_localityName 0x07            /* L  */
#define NID_stateOrProvinceName 0x08     /* ST */
#define NID_streetAddress ASN_STREET_ADDR  /* street */
#define NID_organizationName 0x0a        /* O  */
#define NID_organizationalUnitName 0x0b  /* OU */
#define NID_jurisdictionCountryName 0xc
#define NID_jurisdictionStateOrProvinceName 0xd
#define NID_businessCategory ASN_BUS_CAT
#define NID_domainComponent ASN_DOMAIN_COMPONENT
#define NID_postalCode ASN_POSTAL_CODE   /* postalCode */
#define NID_favouriteDrink 462
#define NID_userId 458
#define NID_emailAddress 0x30            /* emailAddress */
#define NID_id_on_dnsSRV 82              /* 1.3.6.1.5.5.7.8.7 */
#define NID_ms_upn 265                   /* 1.3.6.1.4.1.311.20.2.3 */

#define NID_X9_62_prime_field 406        /* 1.2.840.10045.1.1 */
#endif /* OPENSSL_EXTRA */

enum ECC_TYPES
{
    ECC_PREFIX_0 = 160,
    ECC_PREFIX_1 = 161
};

#ifdef WOLFSSL_CERT_PIV
    enum PIV_Tags {
        ASN_PIV_CERT          = 0x0A,
        ASN_PIV_NONCE         = 0x0B,
        ASN_PIV_SIGNED_NONCE  = 0x0C,

        ASN_PIV_TAG_CERT      = 0x70,
        ASN_PIV_TAG_CERT_INFO = 0x71,
        ASN_PIV_TAG_MSCUID    = 0x72,
        ASN_PIV_TAG_ERR_DET   = 0xFE,

        /* certificate info masks */
        ASN_PIV_CERT_INFO_COMPRESSED = 0x03,
        ASN_PIV_CERT_INFO_ISX509     = 0x04,
        /* GZIP is 0x01 */
        ASN_PIV_CERT_INFO_GZIP       = 0x01,
    };
#endif /* WOLFSSL_CERT_PIV */


#define ASN_JOI_PREFIX_SZ       10
#define ASN_JOI_PREFIX          "\x2b\x06\x01\x04\x01\x82\x37\x3c\x02\x01"
#define ASN_JOI_C               0x3
#define ASN_JOI_ST              0x2

#ifndef WC_ASN_NAME_MAX
    #if defined(OPENSSL_EXTRA) || defined(OPENSSL_EXTRA_X509_SMALL) || \
        defined(WOLFSSL_CERT_EXT)
        #ifdef WOLFSSL_MULTI_ATTRIB
            #define WC_ASN_NAME_MAX 360
        #else
            #define WC_ASN_NAME_MAX 330
        #endif
    #else
        #ifdef WOLFSSL_MULTI_ATTRIB
            #define WC_ASN_NAME_MAX 330
        #else
            #define WC_ASN_NAME_MAX 256
        #endif
    #endif
#endif

/* Maximum OID dotted form size. */
#define ASN1_OID_DOTTED_MAX_SZ         16

enum Misc_ASN {
    MAX_SALT_SIZE       =  64,     /* MAX PKCS Salt length */
    MAX_IV_SIZE         =  64,     /* MAX PKCS Iv length */
    ASN_BOOL_SIZE       =   2,     /* including type */
    ASN_ECC_HEADER_SZ   =   2,     /* String type + 1 byte len */
    ASN_ECC_CONTEXT_SZ  =   2,     /* Content specific type + 1 byte len */
#if defined(WOLFSSL_SM2) && defined(WOLFSSL_SM3)
    KEYID_SIZE          = WC_SM3_DIGEST_SIZE,
#elif defined(NO_SHA) || (!defined(NO_SHA256) && defined(WC_ASN_HASH_SHA256))
    KEYID_SIZE          = WC_SHA256_DIGEST_SIZE,
#else
    KEYID_SIZE          = WC_SHA_DIGEST_SIZE,
#endif
#if !defined(WOLFSSL_RSA_PUBLIC_ONLY) && (defined(WOLFSSL_KEY_GEN) || defined(OPENSSL_EXTRA) || !defined(RSA_LOW_MEM))
    RSA_INTS            =   8,     /* RSA ints in private key */
#elif !defined(WOLFSSL_RSA_PUBLIC_ONLY)
    RSA_INTS            =   5,     /* RSA ints in private key */
#else
    RSA_INTS            =   2,     /* RSA ints in private key */
#endif
    DSA_PARAM_INTS      =   3,     /* DSA parameter ints */
    RSA_PUB_INTS        =   2,     /* RSA ints in public key */
    DSA_PUB_INTS        =   4,     /* DSA ints in public key */
    DSA_INTS            =   5,     /* DSA ints in private key */
    MIN_DATE_SIZE       =  12,
    MAX_DATE_SIZE       =  32,
    ASN_GEN_TIME_SZ     =  15,     /* 7 numbers * 2 + Zulu tag */
#ifndef NO_RSA
#ifdef WOLFSSL_HAPROXY
    MAX_ENCODED_SIG_SZ  = 1024,    /* Supports 8192 bit keys */
#else
    MAX_ENCODED_SIG_SZ  = 512,     /* Supports 4096 bit keys */
#endif
#elif defined(HAVE_ECC)
    MAX_ENCODED_SIG_SZ  = 140,
#elif defined(HAVE_CURVE448)
    MAX_ENCODED_SIG_SZ  = 114,
#else
    MAX_ENCODED_SIG_SZ  =  64,
#endif
    MAX_SIG_SZ          = 256,
    MAX_ALGO_SZ         =  20,
    MAX_SHORT_SZ        =   6,     /* asn int + byte len + 4 byte length */
    MAX_LENGTH_SZ       =   4,     /* Max length size for DER encoding */
    MAX_SEQ_SZ          =   5,     /* enum(seq | con) + length(4) */
    MAX_SET_SZ          =   5,     /* enum(set | con) + length(4) */
    MAX_OCTET_STR_SZ    =   5,     /* enum(set | con) + length(4) */
    MAX_EXP_SZ          =   5,     /* enum(contextspec|con|exp) + length(4) */
    MAX_PRSTR_SZ        =   5,     /* enum(prstr) + length(4) */
    MAX_VERSION_SZ      =   5,     /* enum + id + version(byte) + (header(2))*/
    MAX_ENCODED_DIG_ASN_SZ= 9,     /* enum(bit or octet) + length(4) */
    MAX_ENCODED_DIG_SZ  =  64 + MAX_ENCODED_DIG_ASN_SZ, /* asn header + sha512 */
    MAX_RSA_INT_SZ      = 517,     /* RSA raw sz 4096 for bits + tag + len(4) */
    MAX_DSA_INT_SZ      = 389,     /* DSA raw sz 3072 for bits + tag + len(4) */
    MAX_DSA_PUBKEY_SZ   = (DSA_PUB_INTS * MAX_DSA_INT_SZ) + (2 * MAX_SEQ_SZ) +
                          2 + MAX_LENGTH_SZ, /* Maximum size of a DSA public
                                      key taken from wc_SetDsaPublicKey. */
    MAX_DSA_PRIVKEY_SZ  = (DSA_INTS * MAX_DSA_INT_SZ) + MAX_SEQ_SZ +
                          MAX_VERSION_SZ, /* Maximum size of a DSA Private
                                      key taken from DsaKeyIntsToDer. */
    MAX_RSA_E_SZ        =  16,     /* Max RSA public e size */
    MAX_CA_SZ           =  32,     /* Max encoded CA basic constraint length */
    MAX_SN_SZ           =  35,     /* Max encoded serial number (INT) length */
    MAX_DER_DIGEST_SZ     = MAX_ENCODED_DIG_SZ + MAX_ALGO_SZ + MAX_SEQ_SZ,
                            /* Maximum DER digest size */
    MAX_DER_DIGEST_ASN_SZ = MAX_ENCODED_DIG_ASN_SZ + MAX_ALGO_SZ + MAX_SEQ_SZ,
                            /* Maximum DER digest ASN header size */
                            /* Max X509 header length indicates the max length + 2 ('\n', '\0') */
    MAX_X509_HEADER_SZ  = (37 + 2), /* Maximum PEM Header/Footer Size */
#ifdef WOLFSSL_CERT_GEN
    #ifdef WOLFSSL_CERT_REQ
                          /* Max encoded cert req attributes length */
        MAX_ATTRIB_SZ   = MAX_SEQ_SZ * 4 + (11 + MAX_SEQ_SZ) * 3 +
                          MAX_PRSTR_SZ * 2 + CTC_NAME_SIZE * 2,
                          /* 11 is the OID size */
    #endif
    #if defined(WOLFSSL_ALT_NAMES) || defined(WOLFSSL_CERT_EXT)
        MAX_EXTENSIONS_SZ   = 1 + MAX_LENGTH_SZ + CTC_MAX_ALT_SIZE,
    #else
        MAX_EXTENSIONS_SZ   = 1 + MAX_LENGTH_SZ + MAX_CA_SZ,
    #endif
                                   /* Max total extensions, id + len + others */
#endif
#if defined(WOLFSSL_CERT_EXT) || defined(OPENSSL_EXTRA) || \
        defined(HAVE_PKCS7) || defined(OPENSSL_EXTRA_X509_SMALL) || \
        defined(HAVE_OID_DECODING) || defined(HAVE_OID_ENCODING)
    MAX_OID_SZ          = 32,      /* Max DER length of OID*/
    MAX_OID_STRING_SZ   = 64,      /* Max string length representation of OID*/
#endif
#ifdef WOLFSSL_CERT_EXT
    MAX_KID_SZ          = 45,      /* Max encoded KID length (SHA-256 case) */
    MAX_KEYUSAGE_SZ     = 18,      /* Max encoded Key Usage length */
    MAX_EXTKEYUSAGE_SZ  = 12 + (6 * (8 + 2)) +
                          CTC_MAX_EKU_OID_SZ, /* Max encoded ExtKeyUsage
                          (SEQ/LEN + OBJID + OCTSTR/LEN + SEQ +
                          (6 * (SEQ + OID))) */
#ifndef IGNORE_NETSCAPE_CERT_TYPE
    MAX_NSCERTTYPE_SZ   = MAX_SEQ_SZ + 17, /* SEQ + OID + OCTET STR +
                                            * NS BIT STR */
#endif
    MAX_CERTPOL_NB      = CTC_MAX_CERTPOL_NB,/* Max number of Cert Policy */
    MAX_CERTPOL_SZ      = CTC_MAX_CERTPOL_SZ,
#endif
    MAX_AIA_SZ          = 2,       /* Max Authority Info Access extension size*/
    OCSP_NONCE_EXT_SZ   = 35,      /* OCSP Nonce Extension size */
    MAX_OCSP_EXT_SZ     = 58,      /* Max OCSP Extension length */
    MAX_OCSP_NONCE_SZ   = 16,      /* OCSP Nonce size           */
    MAX_PUBLIC_KEY_SZ   = MAX_DSA_PUBKEY_SZ + MAX_ALGO_SZ + MAX_SEQ_SZ * 2,
#ifdef WOLFSSL_ENCRYPTED_KEYS
    HEADER_ENCRYPTED_KEY_SIZE = 88,/* Extra header size for encrypted key */
#else
    HEADER_ENCRYPTED_KEY_SIZE = 0,
#endif
    TRAILING_ZERO       = 1,       /* Used for size of zero pad */
    ASN_TAG_SZ          = 1,       /* single byte ASN.1 tag */
    MIN_VERSION_SZ      = 3,       /* Min bytes needed for GetMyVersion */
    MAX_X509_VERSION    = 3,       /* Max X509 version allowed */
    MIN_X509_VERSION    = 0,       /* Min X509 version allowed */
    WOLFSSL_X509_V1     = 0,
    WOLFSSL_X509_V2     = 1,
    WOLFSSL_X509_V3     = 2,
#if defined(OPENSSL_ALL)  || defined(WOLFSSL_MYSQL_COMPATIBLE) || \
    defined(WOLFSSL_NGINX) || defined(WOLFSSL_HAPROXY) || \
    defined(OPENSSL_EXTRA) || defined(HAVE_PKCS7)
    MAX_TIME_STRING_SZ  = 25,      /* Max length of formatted time string */
#endif

    PKCS5_SALT_SZ       = 8,

    PEM_LINE_SZ        = 64,               /* Length of Base64 encoded line, not including new line */
    PEM_LINE_LEN       = PEM_LINE_SZ + 12, /* PEM line max + fudge */

    COUNTRY_CODE_LEN   = 2         /* RFC 3739 */
};

#ifndef WC_MAX_NAME_ENTRIES
    /* entries added to x509 name struct */
    #ifdef OPENSSL_EXTRA
    #define WC_MAX_NAME_ENTRIES 16
    #else
    #define WC_MAX_NAME_ENTRIES 14
    #endif
#endif
#define MAX_NAME_ENTRIES WC_MAX_NAME_ENTRIES


enum Oid_Types {
    oidHashType         = 0,
    oidSigType          = 1,
    oidKeyType          = 2,
    oidCurveType        = 3,
    oidBlkType          = 4,
    oidOcspType         = 5,
    oidCertExtType      = 6,
    oidCertAuthInfoType = 7,
    oidCertPolicyType   = 8,
    oidCertAltNameType  = 9,
    oidCertKeyUseType   = 10,
    oidKdfType          = 11,
    oidKeyWrapType      = 12,
    oidCmsKeyAgreeType  = 13,
    oidPBEType          = 14,
    oidHmacType         = 15,
    oidCompressType     = 16,
    oidCertNameType     = 17,
    oidTlsExtType       = 18,
    oidCrlExtType       = 19,
    oidCsrAttrType      = 20,
#ifdef WOLFSSL_SUBJ_DIR_ATTR
    oidSubjDirAttrType  = 21,
#endif
    oidIgnoreType
};


enum Hash_Sum  {
    MD2h      = 646,
    MD5h      = 649,
    SHAh      =  88,
    SHA224h   = 417,
    SHA256h   = 414,
    SHA384h   = 415,
    SHA512h   = 416,
    SHA512_224h = 418,
    SHA512_256h = 419,
    SHA3_224h = 420,
    SHA3_256h = 421,
    SHA3_384h = 422,
    SHA3_512h = 423,
    SHAKE128h = 424,
    SHAKE256h = 425,
    SM3h      = 640
};

#if !defined(NO_DES3) || !defined(NO_AES)
enum Block_Sum {
#ifdef WOLFSSL_AES_128
    AES128CBCb = 414,
    AES128GCMb = 418,
    AES128CCMb = 419,
#endif
#ifdef WOLFSSL_AES_192
    AES192CBCb = 434,
    AES192GCMb = 438,
    AES192CCMb = 439,
#endif
#ifdef WOLFSSL_AES_256
    AES256CBCb = 454,
    AES256GCMb = 458,
    AES256CCMb = 459,
#endif
#ifndef NO_DES3
    DESb       = 69,
    DES3b      = 652
#endif
};
#endif /* !NO_DES3 || !NO_AES */


enum Key_Sum {
    DSAk              = 515,
    RSAk              = 645,
    RSAPSSk           = 654,
    RSAESOAEPk        = 651, /* 1.2.840.113549.1.1.7 */
    ECDSAk            = 518,
    SM2k              = 667,
    ED25519k          = 256, /* 1.3.101.112 */
    X25519k           = 254, /* 1.3.101.110 */
    ED448k            = 257, /* 1.3.101.113 */
    X448k             = 255, /* 1.3.101.111 */
    DHk               = 647, /* dhKeyAgreement OID: 1.2.840.113549.1.3.1 */
    FALCON_LEVEL1k    = 268, /* 1.3.9999.3.1 */
    FALCON_LEVEL5k    = 271, /* 1.3.9999.3.4 */
    DILITHIUM_LEVEL2k = 213,    /* 1.3.6.1.4.1.2.267.7.4.4 */
    DILITHIUM_LEVEL3k = 216,    /* 1.3.6.1.4.1.2.267.7.6.5 */
    DILITHIUM_LEVEL5k = 220,    /* 1.3.6.1.4.1.2.267.7.8.7 */
    SPHINCS_FAST_LEVEL1k   = 281, /* 1 3 9999 6 7 4 */
    SPHINCS_FAST_LEVEL3k   = 283, /* 1 3 9999 6 8 3 + 2 (See GetOID() in asn.c) */
    SPHINCS_FAST_LEVEL5k   = 282, /* 1 3 9999 6 9 3 */
    SPHINCS_SMALL_LEVEL1k  = 287, /* 1 3 9999 6 7 10 */
    SPHINCS_SMALL_LEVEL3k  = 285, /* 1 3 9999 6 8 7 */
    SPHINCS_SMALL_LEVEL5k  = 286  /* 1 3 9999 6 9 7 */
};

#if !defined(NO_AES) || defined(HAVE_PKCS7)
enum KeyWrap_Sum {
#ifdef WOLFSSL_AES_128
    AES128_WRAP  = 417,
#endif
#ifdef WOLFSSL_AES_192
    AES192_WRAP  = 437,
#endif
#ifdef WOLFSSL_AES_256
    AES256_WRAP  = 457,
#endif
#ifdef HAVE_PKCS7
    PWRI_KEK_WRAP = 680  /*id-alg-PWRI-KEK, 1.2.840.113549.1.9.16.3.9 */
#endif
};
#endif /* !NO_AES || PKCS7 */

enum Key_Agree {
    dhSinglePass_stdDH_sha1kdf_scheme   = 464,
    dhSinglePass_stdDH_sha224kdf_scheme = 188,
    dhSinglePass_stdDH_sha256kdf_scheme = 189,
    dhSinglePass_stdDH_sha384kdf_scheme = 190,
    dhSinglePass_stdDH_sha512kdf_scheme = 191
};



enum KDF_Sum {
    PBKDF2_OID = 660,
    MGF1_OID = 652
};


enum HMAC_Sum {
    HMAC_SHA224_OID   = 652,
    HMAC_SHA256_OID   = 653,
    HMAC_SHA384_OID   = 654,
    HMAC_SHA512_OID   = 655,
    HMAC_SHA3_224_OID = 426,
    HMAC_SHA3_256_OID = 427,
    HMAC_SHA3_384_OID = 428,
    HMAC_SHA3_512_OID = 429
};


enum Extensions_Sum {
    BASIC_CA_OID    = 133,           /* 2.5.29.19 */
    ALT_NAMES_OID   = 131,           /* 2.5.29.17 */
    CRL_DIST_OID    = 145,           /* 2.5.29.31 */
    AUTH_INFO_OID   = 69,            /* 1.3.6.1.5.5.7.1.1 */
    AUTH_KEY_OID    = 149,           /* 2.5.29.35 */
    SUBJ_KEY_OID    = 128,           /* 2.5.29.14 */
    CERT_POLICY_OID = 146,           /* 2.5.29.32 */
    CRL_NUMBER_OID  = 134,           /* 2.5.29.20 */
    KEY_USAGE_OID   = 129,           /* 2.5.29.15 */
    INHIBIT_ANY_OID = 168,           /* 2.5.29.54 */
    EXT_KEY_USAGE_OID         = 151, /* 2.5.29.37 */
    NAME_CONS_OID             = 144, /* 2.5.29.30 */
    PRIV_KEY_USAGE_PERIOD_OID = 130, /* 2.5.29.16 */
    SUBJ_INFO_ACC_OID         = 79,  /* 1.3.6.1.5.5.7.1.11 */
    POLICY_MAP_OID            = 147, /* 2.5.29.33 */
    POLICY_CONST_OID          = 150, /* 2.5.29.36 */
    ISSUE_ALT_NAMES_OID       = 132, /* 2.5.29.18 */
    TLS_FEATURE_OID           = 92,  /* 1.3.6.1.5.5.7.1.24 */
    NETSCAPE_CT_OID           = 753, /* 2.16.840.1.113730.1.1 */
    OCSP_NOCHECK_OID          = 121, /* 1.3.6.1.5.5.7.48.1.5
                                         id-pkix-ocsp-nocheck */
    SUBJ_DIR_ATTR_OID         = 123, /* 2.5.29.9 */

    AKEY_PACKAGE_OID          = 1048, /* 2.16.840.1.101.2.1.2.78.5
                                        RFC 5958  - Asymmetric Key Packages */
    FASCN_OID = 419, /* 2.16.840.1.101.3.6.6 Federal PKI Policy FASC-N */
    UPN_OID   = 265  /* 1.3.6.1.4.1.311.20.2.3 UPN */
};

enum CertificatePolicy_Sum {
    CP_ANY_OID              = 146, /* id-ce 32 0 */
#ifdef WOLFSSL_FPKI
    CP_FPKI_COMMON_AUTH_OID = 426, /* 2.16.840.1.101.3.2.1.3.13 */
    CP_FPKI_PIV_AUTH_OID    = 453, /* 2.16.840.1.101.3.2.1.3.40 */
    CP_FPKI_PIV_AUTH_HW_OID = 454, /* 2.16.840.1.101.3.2.1.3.41 */
    CP_FPKI_PIVI_AUTH_OID   = 458, /* 2.16.840.1.101.3.2.1.3.45 */
#endif /* WOLFSSL_FPKI */
    WOLF_ENUM_DUMMY_LAST_ELEMENT(CertificatePolicy_Sum)
};

enum SepHardwareName_Sum {
    HW_NAME_OID     = 79   /* 1.3.6.1.5.5.7.8.4 from RFC 4108*/
};

enum AuthInfo_Sum {
    AIA_OCSP_OID      = 116, /* 1.3.6.1.5.5.7.48.1, id-ad-ocsp */
    AIA_CA_ISSUER_OID = 117, /* 1.3.6.1.5.5.7.48.2, id-ad-caIssuers */
    #ifdef WOLFSSL_SUBJ_INFO_ACC
    AIA_CA_REPO_OID   = 120, /* 1.3.6.1.5.5.7.48.5, id-ad-caRepository */
    #endif /* WOLFSSL_SUBJ_INFO_ACC */
    WOLF_ENUM_DUMMY_LAST_ELEMENT(AuthInfo_Sum)
};

#define ID_PKIX(num) (67+(num)) /* 1.3.6.1.5.5.7.num, id-pkix num */
#define ID_KP(num) (ID_PKIX(3)+(num)) /* 1.3.6.1.5.5.7.3.num, id-kp num */
enum ExtKeyUsage_Sum { /* From RFC 5280 */
    EKU_ANY_OID         = 151, /* 2.5.29.37.0, anyExtendedKeyUsage         */
    EKU_SERVER_AUTH_OID = 71,  /* 1.3.6.1.5.5.7.3.1, id-kp-serverAuth      */
    EKU_CLIENT_AUTH_OID = 72,  /* 1.3.6.1.5.5.7.3.2, id-kp-clientAuth      */
    EKU_CODESIGNING_OID = 73,  /* 1.3.6.1.5.5.7.3.3, id-kp-codeSigning     */
    EKU_EMAILPROTECT_OID = 74, /* 1.3.6.1.5.5.7.3.4, id-kp-emailProtection */
    EKU_TIMESTAMP_OID   = 78,  /* 1.3.6.1.5.5.7.3.8, id-kp-timeStamping    */
    EKU_OCSP_SIGN_OID   = 79,  /* 1.3.6.1.5.5.7.3.9, id-kp-OCSPSigning     */

    /* From RFC 6187: X.509v3 Certificates for Secure Shell Authentication */
    EKU_SSH_CLIENT_AUTH_OID    = ID_KP(21), /* id-kp-secureShellClient */
    EKU_SSH_MSCL_OID           = 264,
        /* 1.3.6.1.4.1.311.20.2.2, MS Smart Card Logon */
    EKU_SSH_KP_CLIENT_AUTH_OID = 64
        /* 1.3.6.1.5.2.3.4, id-pkinit-KPClientAuth*/
};

#ifdef WOLFSSL_SUBJ_DIR_ATTR
#define ID_PDA(num) (ID_PKIX(9)+(num)) /* 1.3.6.1.5.5.7.9.num, id-pda num */
enum SubjDirAttr_Sum { /* From RFC 3739, section 3.3.2 */
    SDA_DOB_OID    = ID_PDA(1), /* id-pda-dateOfBirth */
    SDA_POB_OID    = ID_PDA(2), /* id-pda-placeOfBirth */
    SDA_GENDER_OID = ID_PDA(3), /* id-pda-gender */
    SDA_COC_OID    = ID_PDA(4), /* id-pda-countryOfCitizenship */
    SDA_COR_OID    = ID_PDA(5)  /* id-pda-countryOfResidence */
};
#endif /* WOLFSSL_SUBJ_DIR_ATTR */

#ifdef HAVE_LIBZ
enum CompressAlg_Sum {
    ZLIBc = 679  /* 1.2.840.113549.1.9.16.3.8, id-alg-zlibCompress */
};
#endif

enum VerifyType {
    NO_VERIFY   = 0,
    VERIFY      = 1,
    VERIFY_CRL  = 2,
    VERIFY_OCSP = 3,
    VERIFY_NAME = 4,
    VERIFY_SKIP_DATE = 5,
    VERIFY_OCSP_CERT = 6
};

#ifdef WOLFSSL_CERT_EXT
enum KeyIdType {
    SKID_TYPE = 0,
    AKID_TYPE = 1
};
#endif

#if defined(WOLFSSL_CERT_REQ) || defined(WOLFSSL_CERT_NAME_ALL)
enum CsrAttrType {
    UNSTRUCTURED_NAME_OID = 654,
    PKCS9_CONTENT_TYPE_OID = 655,
    CHALLENGE_PASSWORD_OID = 659,
    SERIAL_NUMBER_OID = 94,
    EXTENSION_REQUEST_OID = 666,
    USER_ID_OID = 865,
    DNQUALIFIER_OID = 135,
    INITIALS_OID = 132,
    SURNAME_OID = 93,
    NAME_OID = 130,
    GIVEN_NAME_OID = 131
};
#endif

/* Key usage extension bits (based on RFC 5280) */
#define KEYUSE_DIGITAL_SIG    0x0080
#define KEYUSE_CONTENT_COMMIT 0x0040
#define KEYUSE_KEY_ENCIPHER   0x0020
#define KEYUSE_DATA_ENCIPHER  0x0010
#define KEYUSE_KEY_AGREE      0x0008
#define KEYUSE_KEY_CERT_SIGN  0x0004
#define KEYUSE_CRL_SIGN       0x0002
#define KEYUSE_ENCIPHER_ONLY  0x0001
#define KEYUSE_DECIPHER_ONLY  0x8000

/* Extended Key Usage bits (internal mapping only) */
#define EXTKEYUSE_USER        0x80
#define EXTKEYUSE_OCSP_SIGN   0x40
#define EXTKEYUSE_TIMESTAMP   0x20
#define EXTKEYUSE_EMAILPROT   0x10
#define EXTKEYUSE_CODESIGN    0x08
#define EXTKEYUSE_CLIENT_AUTH 0x04
#define EXTKEYUSE_SERVER_AUTH 0x02
#define EXTKEYUSE_ANY         0x01
#ifdef WOLFSSL_WOLFSSH
    #define EXTKEYUSE_SSH_CLIENT_AUTH    0x01
    #define EXTKEYUSE_SSH_MSCL           0x02
    #define EXTKEYUSE_SSH_KP_CLIENT_AUTH 0x04
#endif /* WOLFSSL_WOLFSSH */

#define WC_NS_SSL_CLIENT      0x80
#define WC_NS_SSL_SERVER      0x40
#define WC_NS_SMIME           0x20
#define WC_NS_OBJSIGN         0x10
#define WC_NS_SSL_CA          0x04
#define WC_NS_SMIME_CA        0x02
#define WC_NS_OBJSIGN_CA      0x01


#if defined(OPENSSL_ALL) || defined(OPENSSL_EXTRA) || \
    defined(WOLFSSL_WPAS_SMALL) || defined(WOLFSSL_IP_ALT_NAME)
    #ifndef WOLFSSL_MAX_IPSTR
        #define WOLFSSL_MAX_IPSTR 46 /* max ip size IPv4 mapped IPv6 */
    #endif
    #define WOLFSSL_IP4_ADDR_LEN 4
    #define WOLFSSL_IP6_ADDR_LEN 16
#endif /* OPENSSL_ALL || WOLFSSL_IP_ALT_NAME */

typedef struct DNS_entry   DNS_entry;

struct DNS_entry {
    DNS_entry* next;   /* next on DNS list */
    int        type;   /* i.e. ASN_DNS_TYPE */
    int        len;    /* actual DNS len */
    char*      name;   /* actual DNS name */
#if defined(OPENSSL_ALL) || defined(WOLFSSL_IP_ALT_NAME)
    char*      ipString; /* human readable form of IP address */
#endif
#if defined(OPENSSL_ALL)
    char*      ridString; /* human readable form of registeredID */
#endif

#ifdef WOLFSSL_FPKI
    int        oidSum; /* provide oid sum for verification */
#endif
};

#ifdef WOLFSSL_FPKI
    /* RFC4122 i.e urn:uuid:f81d4fae-7dec-11d0-a765-00a0c91e6bf6 */
    #define DEFAULT_UUID_SZ 45
#endif

typedef struct Base_entry  Base_entry;

struct Base_entry {
    Base_entry* next;   /* next on name base list */
    char*       name;   /* actual name base */
    int         nameSz; /* name length */
    byte        type;   /* Name base type (DNS or RFC822) */
};


enum SignatureState {
    SIG_STATE_BEGIN,
    SIG_STATE_HASH,
    SIG_STATE_KEY,
    SIG_STATE_DO,
    SIG_STATE_CHECK
};


#ifdef HAVE_PK_CALLBACKS
#ifdef HAVE_ECC
    typedef int (*wc_CallbackEccVerify)(
           const unsigned char* sig, unsigned int sigSz,
           const unsigned char* hash, unsigned int hashSz,
           const unsigned char* keyDer, unsigned int keySz,
           int* result, void* ctx);
#endif
#ifndef NO_RSA
    typedef int (*wc_CallbackRsaVerify)(
           unsigned char* sig, unsigned int sigSz,
           unsigned char** out,
           const unsigned char* keyDer, unsigned int keySz,
           void* ctx);
#endif
#endif /* HAVE_PK_CALLBACKS */

#if defined(WOLFSSL_RENESAS_TSIP_TLS) || defined(WOLFSSL_RENESAS_SCEPROTECT) ||\
    defined(HAVE_PK_CALLBACKS)
typedef struct tagCertAttribute {
        byte   verifyByTSIP_SCE;
        word32 certBegin;
        word32 pubkey_n_start;
        word32 pubkey_n_len;
        word32 pubkey_e_start;
        word32 pubkey_e_len;
        int curve_id;
        const byte* cert;
        word32 certSz;
        const byte* keyIndex;
  } CertAttribute;
#endif

struct SignatureCtx {
    void* heap;
    byte* digest;
#ifndef NO_RSA
    byte* out;
#endif
#if !(defined(NO_RSA) && defined(NO_DSA))
    byte* sigCpy;
#endif
#if defined(HAVE_ECC) || defined(HAVE_ED25519) || defined(HAVE_ED448) || \
    !defined(NO_DSA)
    int verify;
#endif
    union {
    #ifndef NO_RSA
        struct RsaKey*      rsa;
    #endif
    #ifndef NO_DSA
        struct DsaKey*      dsa;
    #endif
    #ifdef HAVE_ECC
        struct ecc_key*     ecc;
    #endif
    #ifdef HAVE_ED25519
        struct ed25519_key* ed25519;
    #endif
    #ifdef HAVE_ED448
        struct ed448_key* ed448;
    #endif
    #ifdef HAVE_PQC
        struct falcon_key* falcon;
        struct dilithium_key* dilithium;
        struct sphincs_key* sphincs;
    #endif
        void* ptr;
    } key;
    int devId;
    int state;
    int typeH;
    int digestSz;
    word32 keyOID;
#ifdef WOLFSSL_ASYNC_CRYPT
    WC_ASYNC_DEV* asyncDev;
    void* asyncCtx;
#endif

#ifdef HAVE_PK_CALLBACKS
#ifdef HAVE_ECC
    wc_CallbackEccVerify pkCbEcc;
    void* pkCtxEcc;
#endif
#ifndef NO_RSA
    wc_CallbackRsaVerify pkCbRsa;
    void* pkCtxRsa;
#endif
#endif /* HAVE_PK_CALLBACKS */
#ifndef NO_RSA
#if defined(WOLFSSL_RENESAS_TSIP_TLS) || defined(WOLFSSL_RENESAS_SCEPROTECT) ||\
    defined(HAVE_PK_CALLBACKS)
    CertAttribute  CertAtt;
#endif
#ifdef WC_RSA_PSS
    enum wc_HashType hash;
    int mgf;
    int saltLen;
#endif
#endif
};

enum CertSignState {
    CERTSIGN_STATE_BEGIN,
    CERTSIGN_STATE_DIGEST,
    CERTSIGN_STATE_ENCODE,
    CERTSIGN_STATE_DO
};

struct CertSignCtx {
    byte* sig;
    byte* digest;
    #ifndef NO_RSA
        byte* encSig;
        int encSigSz;
    #endif
    int state; /* enum CertSignState */
};

#define DOMAIN_COMPONENT_MAX 10

struct DecodedName {
    char*   fullName;
    int     fullNameLen;
    int     entryCount;
    int     cnIdx;
    int     cnLen;
    int     cnNid;
    int     snIdx;
    int     snLen;
    int     snNid;
    int     cIdx;
    int     cLen;
    int     cNid;
    int     lIdx;
    int     lLen;
    int     lNid;
    int     stIdx;
    int     stLen;
    int     stNid;
    int     oIdx;
    int     oLen;
    int     oNid;
    int     ouIdx;
    int     ouLen;
#ifdef WOLFSSL_CERT_EXT
    int     bcIdx;
    int     bcLen;
    int     jcIdx;
    int     jcLen;
    int     jsIdx;
    int     jsLen;
#endif
    int     ouNid;
    int     emailIdx;
    int     emailLen;
    int     emailNid;
    int     uidIdx;
    int     uidLen;
    int     uidNid;
    int     serialIdx;
    int     serialLen;
    int     serialNid;
    int     dcIdx[DOMAIN_COMPONENT_MAX];
    int     dcLen[DOMAIN_COMPONENT_MAX];
    int     dcNum;
    int     dcMode;
};

/* ASN Encoded Name field */
typedef struct EncodedName {
    int  nameLen;                /* actual string value length */
    int  totalLen;               /* total encoded length */
    int  type;                   /* type of name */
    int  used;                   /* are we actually using this one */
    byte encoded[CTC_NAME_SIZE * 2]; /* encoding */
} EncodedName;

#ifndef WOLFSSL_MAX_PATH_LEN
    /* RFC 5280 Section 6.1.2. "Initialization" - item (k) defines
     *     (k)  max_path_length:  this integer is initialized to "n", is
     *     decremented for each non-self-issued certificate in the path,
     *     and may be reduced to the value in the path length constraint
     *     field within the basic constraints extension of a CA
     *     certificate.
     *
     * wolfSSL has arbitrarily selected the value 127 for "n" in the above
     * description. Users can modify the maximum path length by setting
     * WOLFSSL_MAX_PATH_LEN to a preferred value at build time
     */
    #define WOLFSSL_MAX_PATH_LEN 127
#endif

typedef struct DecodedName DecodedName;
typedef struct DecodedCert DecodedCert;
typedef struct Signer      Signer;
#ifdef WOLFSSL_TRUST_PEER_CERT
typedef struct TrustedPeerCert TrustedPeerCert;
#endif /* WOLFSSL_TRUST_PEER_CERT */
typedef struct SignatureCtx SignatureCtx;
typedef struct CertSignCtx  CertSignCtx;

#if defined(WOLFSSL_CUSTOM_OID) && defined(WOLFSSL_ASN_TEMPLATE) \
    && defined(HAVE_OID_DECODING)
typedef int (*wc_UnknownExtCallback)(const word16* oid, word32 oidSz, int crit,
                                     const unsigned char* der, word32 derSz);
#endif

struct DecodedCert {
    const byte* publicKey;
    word32  pubKeySize;
    int     pubKeyStored;
    word32  certBegin;               /* offset to start of cert          */
    word32  sigIndex;                /* offset to start of signature     */
    word32  sigLength;               /* length of signature              */
    word32  signatureOID;            /* sum of algorithm object id       */
    word32  keyOID;                  /* sum of key algo  object id       */
#ifdef WC_RSA_PSS
    word32  sigParamsIndex;          /* start of signature parameters    */
    word32  sigParamsLength;         /* length of signature parameters   */
#endif
    int     version;                 /* cert version, 1 or 3             */
    DNS_entry* altNames;             /* alt names list of dns entries    */
#ifndef IGNORE_NAME_CONSTRAINTS
    DNS_entry* altEmailNames;        /* alt names list of RFC822 entries */
    DNS_entry* altDirNames;          /* alt names list of DIR entries    */
    Base_entry* permittedNames;      /* Permitted name bases             */
    Base_entry* excludedNames;       /* Excluded name bases              */
#endif /* IGNORE_NAME_CONSTRAINTS */
    byte    subjectHash[KEYID_SIZE]; /* hash of all Names                */
    byte    issuerHash[KEYID_SIZE];  /* hash of all Names                */
#ifdef HAVE_OCSP
    byte    subjectKeyHash[KEYID_SIZE]; /* hash of the public Key         */
    byte    issuerKeyHash[KEYID_SIZE];  /* hash of the public Key         */
#endif /* HAVE_OCSP */
    const byte* signature;           /* not owned, points into raw cert  */
    char*   subjectCN;               /* CommonName                       */
    int     subjectCNLen;            /* CommonName Length                */
    char    subjectCNEnc;            /* CommonName Encoding              */
    char    issuer[WC_ASN_NAME_MAX]; /* full name including common name  */
    char    subject[WC_ASN_NAME_MAX];/* full name including common name  */
    int     verify;                  /* Default to yes, but could be off */
    const byte* source;              /* byte buffer holder cert, NOT owner */
    word32  srcIdx;                  /* current offset into buffer       */
    word32  maxIdx;                  /* max offset based on init size    */
    void*   heap;                    /* for user memory overrides        */
    byte    serial[EXTERNAL_SERIAL_SIZE];  /* raw serial number          */
    int     serialSz;                /* raw serial bytes stored */
    const byte* extensions;          /* not owned, points into raw cert  */
    int     extensionsSz;            /* length of cert extensions */
    word32  extensionsIdx;           /* if want to go back and parse later */
    const byte* extAuthInfo;         /* Authority Information Access URI */
    int     extAuthInfoSz;           /* length of the URI                */
#if defined(OPENSSL_ALL) || defined(WOLFSSL_QT)
    const byte* extAuthInfoCaIssuer; /* Authority Info Access caIssuer URI */
    int     extAuthInfoCaIssuerSz;   /* length of the caIssuer URI         */
#endif
    const byte* extCrlInfoRaw;       /* Entire CRL Distribution Points
                                      * Extension. This is useful when
                                      * re-generating the DER. */
    int     extCrlInfoRawSz;         /* length of the extension          */
    const byte* extCrlInfo;          /* CRL Distribution Points          */
    int     extCrlInfoSz;            /* length of the URI                */
    byte    extSubjKeyId[KEYID_SIZE]; /* Subject Key ID                  */
    byte    extAuthKeyId[KEYID_SIZE]; /* Authority Key ID                */
#ifdef WOLFSSL_AKID_NAME
    const byte* extAuthKeyIdIssuer;  /* Authority Key ID authorityCertIssuer */
    word32  extAuthKeyIdIssuerSz;    /* Authority Key ID authorityCertIssuer length */
    const byte* extAuthKeyIdIssuerSN; /* Authority Key ID authorityCertSerialNumber */
    word32  extAuthKeyIdIssuerSNSz;   /* Authority Key ID authorityCertSerialNumber length */
#endif
    byte    pathLength;              /* CA basic constraint path length  */
    byte    maxPathLen;              /* max_path_len see RFC 5280 section
                                      * 6.1.2 "Initialization" - (k) for
                                      * description of max_path_len */
    byte    policyConstSkip;         /* Policy Constraints skip certs value */
    word16  extKeyUsage;             /* Key usage bitfield               */
    byte    extExtKeyUsage;          /* Extended Key usage bitfield      */
#ifdef WOLFSSL_WOLFSSH
    byte    extExtKeyUsageSsh;       /* Extended Key Usage bitfield for SSH */
#endif /* WOLFSSL_WOLFSSH */

#if defined(OPENSSL_EXTRA) || defined(OPENSSL_EXTRA_X509_SMALL)
    const byte* extExtKeyUsageSrc;
    word32  extExtKeyUsageSz;
    word32  extExtKeyUsageCount;
#ifdef WOLFSSL_AKID_NAME
    const byte* extRawAuthKeyIdSrc;
    word32  extRawAuthKeyIdSz;
#endif
    const byte* extAuthKeyIdSrc;
    word32  extAuthKeyIdSz;
    const byte* extSubjKeyIdSrc;
    word32  extSubjKeyIdSz;
#endif
#ifdef OPENSSL_ALL
    const byte* extSubjAltNameSrc;
    word32  extSubjAltNameSz;
#endif
#ifdef WOLFSSL_SUBJ_DIR_ATTR
    char countryOfCitizenship[COUNTRY_CODE_LEN+1]; /* ISO 3166 Country Code */
    #ifdef OPENSSL_ALL
        const byte* extSubjDirAttrSrc;
        word32 extSubjDirAttrSz;
    #endif
#endif /* WOLFSSL_SUBJ_DIR_ATTR */
#ifdef WOLFSSL_SUBJ_INFO_ACC
    const byte* extSubjInfoAccCaRepo;
    word32 extSubjInfoAccCaRepoSz;
    #ifdef OPENSSL_ALL
        const byte* extSubjInfoAccSrc;
        word32 extSubjInfoAccSz;
    #endif
#endif /* WOLFSSL_SUBJ_INFO_ACC */

#if defined(HAVE_ECC) || defined(HAVE_ED25519) || defined(HAVE_ED448)
    word32  pkCurveOID;           /* Public Key's curve OID */
#endif /* HAVE_ECC */
    const byte* beforeDate;
    int     beforeDateLen;
    const byte* afterDate;
    int     afterDateLen;
#if defined(HAVE_PKCS7) || defined(WOLFSSL_CERT_EXT)
    const byte* issuerRaw;           /* pointer to issuer inside source */
    int     issuerRawLen;
#endif
#if !defined(IGNORE_NAME_CONSTRAINTS) || defined(WOLFSSL_CERT_EXT)
    const byte* subjectRaw;          /* pointer to subject inside source */
    int     subjectRawLen;
#endif
#if !defined(IGNORE_NAME_CONSTRAINTS) || \
     defined(WOLFSSL_CERT_GEN) || defined(WOLFSSL_CERT_EXT)
    char*   subjectEmail;
    int     subjectEmailLen;
#endif
#if defined(WOLFSSL_CERT_GEN) || defined(WOLFSSL_CERT_EXT)
    /* easy access to subject info for other sign */
    char*   subjectSN;
    int     subjectSNLen;
    char    subjectSNEnc;
    #ifdef WOLFSSL_CERT_NAME_ALL
    char*   subjectN;
    int     subjectNLen;
    char    subjectNEnc;
    char*   subjectI;
    int     subjectILen;
    char    subjectIEnc;
    char*   subjectGN;
    int     subjectGNLen;
    char    subjectGNEnc;
    char*   subjectDNQ;
    int     subjectDNQLen;
    char    subjectDNQEnc;
    #endif /*WOLFSSL_CERT_NAME_ALL */
    char*   subjectC;
    int     subjectCLen;
    char    subjectCEnc;
    char*   subjectL;
    int     subjectLLen;
    char    subjectLEnc;
    char*   subjectST;
    int     subjectSTLen;
    char    subjectSTEnc;
    char*   subjectO;
    int     subjectOLen;
    char    subjectOEnc;
    char*   subjectOU;
    int     subjectOULen;
    char    subjectOUEnc;
    char*   subjectSND;
    int     subjectSNDLen;
    char    subjectSNDEnc;
    char*   subjectUID;
    int     subjectUIDLen;
    char    subjectUIDEnc;
    char*   subjectStreet;
    int     subjectStreetLen;
    char    subjectStreetEnc;
    char*   subjectBC;
    int     subjectBCLen;
    char    subjectBCEnc;
    char*   subjectJC;
    int     subjectJCLen;
    char    subjectJCEnc;
    char*   subjectJS;
    int     subjectJSLen;
    char    subjectJSEnc;
    char*   subjectPC;
    int     subjectPCLen;
    char    subjectPCEnc;
#if defined(WOLFSSL_HAVE_ISSUER_NAMES)
    char*   issuerCN;
    int     issuerCNLen;
    char    issuerCNEnc;
    char*   issuerSN;
    int     issuerSNLen;
    char    issuerSNEnc;
    char*   issuerC;
    int     issuerCLen;
    char    issuerCEnc;
    char*   issuerL;
    int     issuerLLen;
    char    issuerLEnc;
    char*   issuerST;
    int     issuerSTLen;
    char    issuerSTEnc;
    char*   issuerO;
    int     issuerOLen;
    char    issuerOEnc;
    char*   issuerOU;
    int     issuerOULen;
    char    issuerOUEnc;
    char*   issuerSND;
    int     issuerSNDLen;
    char    issuerSNDEnc;
    char*   issuerEmail;
    int     issuerEmailLen;
#endif /* WOLFSSL_HAVE_ISSUER_NAMES */
#endif /* defined(WOLFSSL_CERT_GEN) || defined(WOLFSSL_CERT_EXT) */
#if defined(OPENSSL_EXTRA) || defined(OPENSSL_EXTRA_X509_SMALL)
    /* WOLFSSL_X509_NAME structures (used void* to avoid including ssl.h) */
    void* issuerName;
    void* subjectName;
#endif /* OPENSSL_EXTRA */
#ifdef WOLFSSL_SEP
    int     deviceTypeSz;
    byte*   deviceType;
    int     hwTypeSz;
    byte*   hwType;
    int     hwSerialNumSz;
    byte*   hwSerialNum;
#endif /* WOLFSSL_SEP */
#ifdef WOLFSSL_CERT_EXT
    char    extCertPolicies[MAX_CERTPOL_NB][MAX_CERTPOL_SZ];
    int     extCertPoliciesNb;
#endif /* WOLFSSL_CERT_EXT */
#ifndef IGNORE_NETSCAPE_CERT_TYPE
    byte    nsCertType;
#endif

#ifdef WOLFSSL_CERT_REQ
    /* CSR attributes */
    char*   contentType; /* Content Type */
    int     contentTypeLen;
    char*   cPwd; /* Challenge Password */
    int     cPwdLen;
    char*   sNum; /* Serial Number */
    int     sNumLen;
    char*   dnQualifier;
    int     dnQualifierLen;
    char*   initials;
    int     initialsLen;
    char*   surname;
    int     surnameLen;
    char*   givenName;
    int     givenNameLen;
    char*   unstructuredName;
    int     unstructuredNameLen;
#endif /* WOLFSSL_CERT_REQ */

    Signer* ca;
#ifndef NO_CERTS
    SignatureCtx sigCtx;
#endif
#if defined(WOLFSSL_RENESAS_TSIP) || defined(WOLFSSL_RENESAS_SCEPROTECT)
    byte*  sce_tsip_encRsaKeyIdx;
#endif
#ifdef WOLFSSL_MAXQ10XX_TLS
    word32 publicKeyIndex; /* offset to start of public key */
#endif

    int badDate;
    int criticalExt;

    /* Option Bits */
    byte subjectCNStored : 1;      /* have we saved a copy we own */
    byte extSubjKeyIdSet : 1;      /* Set when the SKID was read from cert */
    byte extAuthKeyIdSet : 1;      /* Set when the AKID was read from cert */
#ifndef IGNORE_NAME_CONSTRAINTS
    byte extNameConstraintSet : 1;
#endif
    byte isCA : 1;                 /* CA basic constraint true */
    byte pathLengthSet : 1;        /* CA basic const path length set */
    byte weOwnAltNames : 1;        /* altNames haven't been given to copy */
    byte extKeyUsageSet : 1;
    byte extExtKeyUsageSet : 1;    /* Extended Key Usage set */
#ifdef HAVE_OCSP
    byte ocspNoCheckSet : 1;       /* id-pkix-ocsp-nocheck set */
#endif
    byte extCRLdistSet : 1;
    byte extAuthInfoSet : 1;
    byte extBasicConstSet : 1;
    byte extPolicyConstSet : 1;
    byte extPolicyConstRxpSet : 1; /* requireExplicitPolicy set */
    byte extPolicyConstIpmSet : 1; /* inhibitPolicyMapping set */
    byte extSubjAltNameSet : 1;
    byte inhibitAnyOidSet : 1;
    byte selfSigned : 1;           /* Indicates subject and issuer are same */
#if defined(WOLFSSL_SEP) || defined(WOLFSSL_QT)
    byte extCertPolicySet : 1;
#endif
    byte extCRLdistCrit : 1;
    byte extAuthInfoCrit : 1;
    byte extBasicConstCrit : 1;
    byte extPolicyConstCrit : 1;
    byte extSubjAltNameCrit : 1;
    byte extAuthKeyIdCrit : 1;
#ifndef IGNORE_NAME_CONSTRAINTS
    byte extNameConstraintCrit : 1;
#endif
    byte extSubjKeyIdCrit : 1;
    byte extKeyUsageCrit : 1;
    byte extExtKeyUsageCrit : 1;
#ifdef WOLFSSL_SUBJ_DIR_ATTR
    byte extSubjDirAttrSet : 1;
#endif
#ifdef WOLFSSL_SUBJ_INFO_ACC
    byte extSubjInfoAccSet : 1;
#endif
#if defined(WOLFSSL_SEP) || defined(WOLFSSL_QT)
    byte extCertPolicyCrit : 1;
#endif
#ifdef WOLFSSL_CERT_REQ
    byte isCSR : 1;                /* Do we intend on parsing a CSR? */
#endif
#if defined(WOLFSSL_CUSTOM_OID) && defined(WOLFSSL_ASN_TEMPLATE) \
    && defined(HAVE_OID_DECODING)
    wc_UnknownExtCallback unknownExtCallback;
#endif
};

#if defined(WOLFSSL_SM2) && defined(WOLFSSL_SM3)
    #define SIGNER_DIGEST_SIZE WC_SM3_DIGEST_SIZE
#elif defined(NO_SHA)
    #define SIGNER_DIGEST_SIZE WC_SHA256_DIGEST_SIZE
#else
    #define SIGNER_DIGEST_SIZE WC_SHA_DIGEST_SIZE
#endif

/* CA Signers */
/* if change layout change PERSIST_CERT_CACHE functions too */
struct Signer {
    word32  pubKeySize;
    word32  keyOID;                  /* key type */
    word16  keyUsage;
    byte    maxPathLen;
    byte    selfSigned : 1;
    const byte* publicKey;
    int     nameLen;
    char*   name;                    /* common name */
#ifndef IGNORE_NAME_CONSTRAINTS
        Base_entry* permittedNames;
        Base_entry* excludedNames;
#endif /* IGNORE_NAME_CONSTRAINTS */
    byte    subjectNameHash[SIGNER_DIGEST_SIZE];
                                     /* sha hash of names in certificate */
    #ifdef HAVE_OCSP
        byte    issuerNameHash[SIGNER_DIGEST_SIZE];
                                     /* sha hash of issuer names in certificate.
                                      * Used in OCSP to check for authorized
                                      * responders. */
    #endif
    #ifndef NO_SKID
        byte    subjectKeyIdHash[SIGNER_DIGEST_SIZE];
                                     /* sha hash of key in certificate */
    #endif
    #ifdef HAVE_OCSP
        byte subjectKeyHash[KEYID_SIZE];
    #endif
#ifdef WOLFSSL_AKID_NAME
    byte serialHash[SIGNER_DIGEST_SIZE]; /* serial number hash */
#endif
#ifdef WOLFSSL_SIGNER_DER_CERT
    DerBuffer* derCert;
#endif
#if defined(WOLFSSL_RENESAS_TSIP_TLS) || defined(WOLFSSL_RENESAS_SCEPROTECT)
    word32 cm_idx;
#endif
    Signer* next;
};


#ifdef WOLFSSL_TRUST_PEER_CERT
/* used for having trusted peer certs rather then CA */
struct TrustedPeerCert {
    int     nameLen;
    char*   name;                    /* common name */
    #ifndef IGNORE_NAME_CONSTRAINTS
        Base_entry* permittedNames;
        Base_entry* excludedNames;
    #endif /* IGNORE_NAME_CONSTRAINTS */
    byte    subjectNameHash[SIGNER_DIGEST_SIZE];
                                     /* sha hash of names in certificate */
    #ifndef NO_SKID
        byte    subjectKeyIdHash[SIGNER_DIGEST_SIZE];
                                     /* sha hash of SKID in certificate */
    #endif
    word32 sigLen;
    byte*  sig;
    struct TrustedPeerCert* next;
};
#endif /* WOLFSSL_TRUST_PEER_CERT */


/* for testing or custom openssl wrappers */
#if defined(WOLFSSL_TEST_CERT) || defined(OPENSSL_EXTRA) || \
    defined(OPENSSL_EXTRA_X509_SMALL) || defined(WOLFSSL_PUBLIC_ASN)
    #define WOLFSSL_ASN_API WOLFSSL_API
#else
    #define WOLFSSL_ASN_API WOLFSSL_LOCAL
#endif

#ifdef HAVE_SMIME
#define MIME_HEADER_ASCII_MIN   33
#define MIME_HEADER_ASCII_MAX   126

typedef struct MimeParam MimeParam;
typedef struct MimeHdr MimeHdr;

struct MimeParam
{
    MimeParam*  next;
    char*       attribute;
    char*       value;
};

struct MimeHdr
{
    MimeHdr*    next;
    MimeParam*  params;
    char*       name;
    char*       body;
};

typedef enum MimeTypes
{
    MIME_HDR,
    MIME_PARAM
} MimeTypes;

typedef enum MimeStatus
{
    MIME_NAMEATTR,
    MIME_BODYVAL
} MimeStatus;
#endif /* HAVE_SMIME */


WOLFSSL_LOCAL int HashIdAlg(word32 oidSum);
WOLFSSL_LOCAL int CalcHashId(const byte* data, word32 len, byte* hash);
WOLFSSL_LOCAL int CalcHashId_ex(const byte* data, word32 len, byte* hash,
    int hashAlg);
WOLFSSL_LOCAL int GetName(DecodedCert* cert, int nameType, int maxIdx);

WOLFSSL_ASN_API int wc_BerToDer(const byte* ber, word32 berSz, byte* der,
                                word32* derSz);

WOLFSSL_ASN_API void FreeAltNames(DNS_entry* altNames, void* heap);
WOLFSSL_ASN_API DNS_entry* AltNameNew(void* heap);
#ifndef IGNORE_NAME_CONSTRAINTS
    WOLFSSL_ASN_API void FreeNameSubtrees(Base_entry* names, void* heap);
#endif /* IGNORE_NAME_CONSTRAINTS */
WOLFSSL_ASN_API void InitDecodedCert(DecodedCert* cert, const byte* source,
                                     word32 inSz, void* heap);
WOLFSSL_LOCAL void InitDecodedCert_ex(DecodedCert* cert, const byte* source,
                                     word32 inSz, void* heap, int devId);
WOLFSSL_ASN_API void FreeDecodedCert(DecodedCert* cert);
WOLFSSL_ASN_API int  ParseCert(DecodedCert* cert, int type, int verify,
                               void* cm);

#if defined(WOLFSSL_CUSTOM_OID) && defined(WOLFSSL_ASN_TEMPLATE) \
    && defined(HAVE_OID_DECODING)
WOLFSSL_ASN_API int wc_SetUnknownExtCallback(DecodedCert* cert,
                                             wc_UnknownExtCallback cb);
#endif

WOLFSSL_LOCAL int DecodePolicyOID(char *out, word32 outSz, const byte *in,
                                  word32 inSz);
WOLFSSL_LOCAL int EncodePolicyOID(byte *out, word32 *outSz,
                                  const char *in, void* heap);
WOLFSSL_API int CheckCertSignature(const byte*,word32,void*,void* cm);
WOLFSSL_LOCAL int CheckCertSignaturePubKey(const byte* cert, word32 certSz,
        void* heap, const byte* pubKey, word32 pubKeySz, int pubKeyOID);
#ifdef OPENSSL_EXTRA
WOLFSSL_API int wc_CheckCertSigPubKey(const byte* cert, word32 certSz,
                                      void* heap, const byte* pubKey,
                                      word32 pubKeySz, int pubKeyOID);
#endif
#if (defined(HAVE_ED25519) && defined(HAVE_ED25519_KEY_IMPORT) || \
    (defined(HAVE_ED448) && defined(HAVE_ED448_KEY_IMPORT)))
WOLFSSL_LOCAL int wc_CertGetPubKey(const byte* cert, word32 certSz,
    const unsigned char** pubKey, word32* pubKeySz);
#endif

#ifdef WOLFSSL_CERT_REQ
WOLFSSL_LOCAL int CheckCSRSignaturePubKey(const byte* cert, word32 certSz,
        void* heap, const byte* pubKey, word32 pubKeySz, int pubKeyOID);
#endif /* WOLFSSL_CERT_REQ */
WOLFSSL_ASN_API int AddSignature(byte* buf, int bodySz, const byte* sig, int sigSz,
                        int sigAlgoType);
WOLFSSL_LOCAL int ParseCertRelative(DecodedCert* cert, int type, int verify,
                                    void* cm);
WOLFSSL_LOCAL int DecodeToKey(DecodedCert* cert, int verify);
#ifdef WOLFSSL_ASN_TEMPLATE
WOLFSSL_LOCAL int DecodeCert(DecodedCert* cert, int verify, int* criticalExt);
#endif
WOLFSSL_LOCAL int wc_GetPubX509(DecodedCert* cert, int verify, int* badDate);

WOLFSSL_LOCAL const byte* OidFromId(word32 id, word32 type, word32* oidSz);
WOLFSSL_LOCAL Signer* MakeSigner(void* heap);
WOLFSSL_LOCAL void    FreeSigner(Signer* signer, void* heap);
WOLFSSL_LOCAL void    FreeSignerTable(Signer** table, int rows, void* heap);
#ifdef WOLFSSL_TRUST_PEER_CERT
WOLFSSL_LOCAL void    FreeTrustedPeer(TrustedPeerCert* tp, void* heap);
WOLFSSL_LOCAL void    FreeTrustedPeerTable(TrustedPeerCert** table, int rows,
                                           void* heap);
#endif /* WOLFSSL_TRUST_PEER_CERT */

WOLFSSL_ASN_API int ToTraditional(byte* buffer, word32 length);
WOLFSSL_ASN_API int ToTraditional_ex(byte* buffer, word32 length,
                                     word32* algId);
WOLFSSL_LOCAL int ToTraditionalInline(const byte* input, word32* inOutIdx,
                                      word32 length);
WOLFSSL_LOCAL int ToTraditionalInline_ex(const byte* input, word32* inOutIdx,
                                         word32 length, word32* algId);
WOLFSSL_LOCAL int ToTraditionalEnc(byte* input, word32 sz, const char* password,
                     int passwordSz, word32* algId);
WOLFSSL_ASN_API int UnTraditionalEnc(byte* key, word32 keySz, byte* out,
        word32* outSz, const char* password, int passwordSz, int vPKCS,
        int vAlgo, byte* salt, word32 saltSz, int itt, WC_RNG* rng, void* heap);
WOLFSSL_ASN_API int TraditionalEnc(byte* key, word32 keySz, byte* out,
        word32* outSz, const char* password, int passwordSz, int vPKCS,
        int vAlgo, int encAlgId, byte* salt, word32 saltSz, int itt,
        WC_RNG* rng, void* heap);
WOLFSSL_LOCAL int DecryptContent(byte* input, word32 sz,const char* psw,int pswSz);
WOLFSSL_LOCAL int EncryptContent(byte* input, word32 sz, byte* out, word32* outSz,
        const char* password,int passwordSz, int vPKCS, int vAlgo,
        byte* salt, word32 saltSz, int itt, WC_RNG* rng, void* heap);
WOLFSSL_LOCAL int wc_GetKeyOID(byte* key, word32 keySz, const byte** curveOID,
        word32* oidSz, int* algoID, void* heap);

typedef struct tm wolfssl_tm;
#if defined(OPENSSL_ALL) || defined(WOLFSSL_MYSQL_COMPATIBLE) || defined(OPENSSL_EXTRA) || \
    defined(WOLFSSL_NGINX) || defined(WOLFSSL_HAPROXY)
WOLFSSL_LOCAL int GetTimeString(byte* date, int format, char* buf, int len);
#endif
#if !defined(NO_ASN_TIME) && !defined(USER_TIME) && \
    !defined(TIME_OVERRIDES) && (defined(OPENSSL_EXTRA) || defined(HAVE_PKCS7))
WOLFSSL_LOCAL int GetFormattedTime(void* currTime, byte* buf, word32 len);
WOLFSSL_LOCAL int GetAsnTimeString(void* currTime, byte* buf, word32 len);
#endif
WOLFSSL_LOCAL int ExtractDate(const unsigned char* date, unsigned char format,
                                                 wolfssl_tm* certTime, int* idx);
WOLFSSL_LOCAL int DateGreaterThan(const struct tm* a, const struct tm* b);
WOLFSSL_LOCAL int wc_ValidateDate(const byte* date, byte format, int dateType);

/* ASN.1 helper functions */
#ifdef WOLFSSL_CERT_GEN
WOLFSSL_LOCAL   int SetNameEx(byte* output, word32 outputSz, CertName* name, void* heap);
WOLFSSL_ASN_API int SetName(byte* output, word32 outputSz, CertName* name);
WOLFSSL_LOCAL const char* GetOneCertName(CertName* name, int idx);
WOLFSSL_LOCAL byte GetCertNameId(int idx);
#endif
WOLFSSL_LOCAL int GetShortInt(const byte* input, word32* inOutIdx, int* number,
                              word32 maxIdx);
WOLFSSL_LOCAL int SetShortInt(byte* input, word32* inOutIdx, word32 number,
                              word32 maxIdx);

WOLFSSL_LOCAL const char* GetSigName(int oid);
WOLFSSL_LOCAL int GetLength(const byte* input, word32* inOutIdx, int* len,
                           word32 maxIdx);
WOLFSSL_LOCAL int GetLength_ex(const byte* input, word32* inOutIdx, int* len,
                           word32 maxIdx, int check);
WOLFSSL_LOCAL int GetSequence(const byte* input, word32* inOutIdx, int* len,
                             word32 maxIdx);
WOLFSSL_LOCAL int GetSequence_ex(const byte* input, word32* inOutIdx, int* len,
                           word32 maxIdx, int check);
WOLFSSL_LOCAL int GetOctetString(const byte* input, word32* inOutIdx, int* len,
                         word32 maxIdx);
WOLFSSL_LOCAL int CheckBitString(const byte* input, word32* inOutIdx, int* len,
                          word32 maxIdx, int zeroBits, byte* unusedBits);
WOLFSSL_LOCAL int GetSet(const byte* input, word32* inOutIdx, int* len,
                        word32 maxIdx);
WOLFSSL_LOCAL int GetSet_ex(const byte* input, word32* inOutIdx, int* len,
                        word32 maxIdx, int check);
WOLFSSL_LOCAL int GetMyVersion(const byte* input, word32* inOutIdx,
                              int* version, word32 maxIdx);
WOLFSSL_LOCAL int GetInt(mp_int* mpi, const byte* input, word32* inOutIdx,
                         word32 maxIdx);

#ifdef HAVE_OID_ENCODING
    WOLFSSL_API int wc_EncodeObjectId(const word16* in, word32 inSz,
        byte* out, word32* outSz);
    WOLFSSL_LOCAL int EncodeObjectId(const word16* in, word32 inSz,
        byte* out, word32* outSz);
#endif
#if defined(HAVE_OID_DECODING) || defined(WOLFSSL_ASN_PRINT) || \
    defined(OPENSSL_ALL)
    WOLFSSL_LOCAL int DecodeObjectId(const byte* in, word32 inSz,
        word16* out, word32* outSz);
#endif
WOLFSSL_LOCAL int GetASNObjectId(const byte* input, word32* inOutIdx, int* len,
                                 word32 maxIdx);
WOLFSSL_LOCAL int SetObjectId(int len, byte* output);
WOLFSSL_LOCAL int GetObjectId(const byte* input, word32* inOutIdx, word32* oid,
                              word32 oidType, word32 maxIdx);
WOLFSSL_LOCAL int GetAlgoId(const byte* input, word32* inOutIdx, word32* oid,
                           word32 oidType, word32 maxIdx);
WOLFSSL_LOCAL int GetASNTag(const byte* input, word32* idx, byte* tag,
                            word32 inputSz);

WOLFSSL_LOCAL word32 SetASNLength(word32 length, byte* output);
WOLFSSL_LOCAL word32 SetASNSequence(word32 len, byte* output);
WOLFSSL_LOCAL word32 SetASNOctetString(word32 len, byte* output);
WOLFSSL_LOCAL word32 SetASNImplicit(byte tag,byte number, word32 len,
                                    byte* output);
WOLFSSL_LOCAL word32 SetASNExplicit(byte number, word32 len, byte* output);
WOLFSSL_LOCAL word32 SetASNSet(word32 len, byte* output);

WOLFSSL_LOCAL word32 SetLength(word32 length, byte* output);
WOLFSSL_LOCAL word32 SetSequence(word32 len, byte* output);
WOLFSSL_LOCAL word32 SetOctetString(word32 len, byte* output);
WOLFSSL_LOCAL int SetASNInt(int len, byte firstByte, byte* output);
WOLFSSL_LOCAL word32 SetBitString(word32 len, byte unusedBits, byte* output);
WOLFSSL_LOCAL word32 SetImplicit(byte tag,byte number,word32 len,byte* output);
WOLFSSL_LOCAL word32 SetExplicit(byte number, word32 len, byte* output);
WOLFSSL_LOCAL word32 SetSet(word32 len, byte* output);
WOLFSSL_LOCAL word32 SetAlgoID(int algoOID,byte* output,int type,int curveSz);
WOLFSSL_LOCAL int SetMyVersion(word32 version, byte* output, int header);
WOLFSSL_LOCAL int SetSerialNumber(const byte* sn, word32 snSz, byte* output,
    word32 outputSz, int maxSnSz);
/* name is of type WOLFSSL_ASN1_OTHERNAME; use void* to avoid including ssl.h */
WOLFSSL_LOCAL word32 SetOthername(void *name, byte *output);

#ifndef WOLFSSL_ASN_TEMPLATE
WOLFSSL_LOCAL int wc_GetSerialNumber(const byte* input, word32* inOutIdx,
    byte* serial, int* serialSz, word32 maxIdx);
#endif
WOLFSSL_LOCAL int GetNameHash(const byte* source, word32* idx, byte* hash,
                              int maxIdx);
WOLFSSL_LOCAL int GetNameHash_ex(const byte* source, word32* idx, byte* hash,
                                 int maxIdx, word32 sigOID);
WOLFSSL_LOCAL int wc_CheckPrivateKeyCert(const byte* key, word32 keySz, DecodedCert* der);
WOLFSSL_LOCAL int wc_CheckPrivateKey(const byte* privKey, word32 privKeySz,
                                     const byte* pubKey, word32 pubKeySz, enum Key_Sum ks);
WOLFSSL_LOCAL int StoreDHparams(byte* out, word32* outLen, mp_int* p, mp_int* g);
#ifdef WOLFSSL_DH_EXTRA
WOLFSSL_API int wc_DhPublicKeyDecode(const byte* input, word32* inOutIdx,
                        DhKey* key, word32 inSz);
#endif
WOLFSSL_LOCAL int FlattenAltNames(byte* output, word32 outputSz,
                                  const DNS_entry* names);

WOLFSSL_LOCAL int wc_EncodeName(EncodedName* name, const char* nameStr,
        char nameType, byte type);
WOLFSSL_LOCAL int wc_EncodeNameCanonical(EncodedName* name, const char* nameStr,
                                char nameType, byte type);

#if defined(HAVE_ECC) || !defined(NO_DSA)
    /* ASN sig helpers */
    WOLFSSL_LOCAL int StoreECC_DSA_Sig(byte* out, word32* outLen, mp_int* r,
                                      mp_int* s);
    WOLFSSL_LOCAL int StoreECC_DSA_Sig_Bin(byte* out, word32* outLen,
        const byte* r, word32 rLen, const byte* s, word32 sLen);
    WOLFSSL_LOCAL int DecodeECC_DSA_Sig_Bin(const byte* sig, word32 sigLen,
        byte* r, word32* rLen, byte* s, word32* sLen);
    WOLFSSL_LOCAL int DecodeECC_DSA_Sig(const byte* sig, word32 sigLen,
                                       mp_int* r, mp_int* s);
    WOLFSSL_LOCAL int DecodeECC_DSA_Sig_Ex(const byte* sig, word32 sigLen,
                                       mp_int* r, mp_int* s, int init);
#endif
#ifndef NO_DSA
WOLFSSL_LOCAL int StoreDSAParams(byte*, word32*, const mp_int*, const mp_int*,
    const mp_int*);
#endif

WOLFSSL_LOCAL void InitSignatureCtx(SignatureCtx* sigCtx, void* heap, int devId);
WOLFSSL_LOCAL void FreeSignatureCtx(SignatureCtx* sigCtx);

WOLFSSL_LOCAL int SetAsymKeyDerPublic(const byte* pubKey, word32 pubKeyLen,
    byte* output, word32 outLen, int keyType, int withHeader);
WOLFSSL_LOCAL int DecodeAsymKeyPublic(const byte* input, word32* inOutIdx, word32 inSz,
    byte* pubKey, word32* pubKeyLen, int keyType);

#ifndef NO_CERTS

WOLFSSL_LOCAL int wc_EncryptedInfoParse(EncryptedInfo* info,
                                        const char** pBuffer, size_t bufSz);

WOLFSSL_LOCAL int PemToDer(const unsigned char* buff, long sz, int type,
                          DerBuffer** pDer, void* heap, EncryptedInfo* info,
                          int* eccKey);
WOLFSSL_LOCAL int AllocDer(DerBuffer** der, word32 length, int type, void* heap);
WOLFSSL_LOCAL void FreeDer(DerBuffer** der);

#if (defined(WOLFSSL_CERT_GEN) && defined(WOLFSSL_CERT_EXT)) || \
    (defined(OPENSSL_ALL) || defined(OPENSSL_EXTRA))
WOLFSSL_LOCAL int ParseKeyUsageStr(const char* value, word16* keyUsage,
        void* heap);
WOLFSSL_LOCAL int ParseExtKeyUsageStr(const char* value, byte* extKeyUsage,
        void* heap);
#endif /* (CERT_GEN && CERT_EXT) || (OPENSSL_ALL || OPENSSL_EXTRA) */

#endif /* !NO_CERTS */

#ifdef HAVE_SMIME
WOLFSSL_LOCAL int wc_MIME_parse_headers(char* in, int inLen, MimeHdr** hdrs);
WOLFSSL_LOCAL int wc_MIME_header_strip(char* in, char** out, size_t start, size_t end);
WOLFSSL_LOCAL MimeHdr* wc_MIME_find_header_name(const char* name, MimeHdr* hdr);
WOLFSSL_LOCAL MimeParam* wc_MIME_find_param_attr(const char* attribute, MimeParam* param);
WOLFSSL_LOCAL char* wc_MIME_single_canonicalize(const char* line, word32* len);
WOLFSSL_LOCAL int wc_MIME_free_hdrs(MimeHdr* head);
#endif /* HAVE_SMIME */

#ifdef WOLFSSL_CERT_GEN

enum cert_enums {
    RSA_KEY                  = 10,
    ECC_KEY                  = 12,
    ED25519_KEY              = 13,
    ED448_KEY                = 14,
    DSA_KEY                  = 15,
    FALCON_LEVEL1_KEY        = 16,
    FALCON_LEVEL5_KEY        = 17,
    DILITHIUM_LEVEL2_KEY     = 18,
    DILITHIUM_LEVEL3_KEY     = 19,
    DILITHIUM_LEVEL5_KEY     = 20,
    SPHINCS_FAST_LEVEL1_KEY  = 24,
    SPHINCS_FAST_LEVEL3_KEY  = 25,
    SPHINCS_FAST_LEVEL5_KEY  = 26,
    SPHINCS_SMALL_LEVEL1_KEY = 27,
    SPHINCS_SMALL_LEVEL3_KEY = 28,
    SPHINCS_SMALL_LEVEL5_KEY = 29
};

#endif /* WOLFSSL_CERT_GEN */



/* for pointer use */
typedef struct CertStatus CertStatus;

#ifdef HAVE_OCSP

enum Ocsp_Response_Status {
    OCSP_SUCCESSFUL        = 0, /* Response has valid confirmations */
    OCSP_MALFORMED_REQUEST = 1, /* Illegal confirmation request */
    OCSP_INTERNAL_ERROR    = 2, /* Internal error in issuer */
    OCSP_TRY_LATER         = 3, /* Try again later */
    OCSP_SIG_REQUIRED      = 5, /* Must sign the request (4 is skipped) */
    OCSP_UNAUTHORIZED      = 6  /* Request unauthorized */
};


enum Ocsp_Cert_Status {
    CERT_GOOD    = 0,
    CERT_REVOKED = 1,
    CERT_UNKNOWN = 2
};


enum Ocsp_Sums {
    OCSP_BASIC_OID = 117,
    OCSP_NONCE_OID = 118
};

#ifdef OPENSSL_EXTRA
enum Ocsp_Verify_Error {
    OCSP_VERIFY_ERROR_NONE = 0,
    OCSP_BAD_ISSUER = 1
};
#endif


typedef struct OcspRequest  OcspRequest;
typedef struct OcspResponse OcspResponse;


struct CertStatus {
    CertStatus* next;

    byte serial[EXTERNAL_SERIAL_SIZE];
    int serialSz;
#ifdef OPENSSL_EXTRA
    WOLFSSL_ASN1_INTEGER* serialInt;
#endif

    int status;

    byte thisDate[MAX_DATE_SIZE];
    byte nextDate[MAX_DATE_SIZE];
    byte thisDateFormat;
    byte nextDateFormat;
#if defined(OPENSSL_ALL) || defined(WOLFSSL_NGINX) || \
    defined(WOLFSSL_HAPROXY) || defined(HAVE_LIGHTY)
    WOLFSSL_ASN1_TIME thisDateParsed;
    WOLFSSL_ASN1_TIME nextDateParsed;
    byte* thisDateAsn;
    byte* nextDateAsn;
#endif

    byte*  rawOcspResponse;
    word32 rawOcspResponseSz;
};

typedef struct OcspEntry OcspEntry;

#if defined(WOLFSSL_SM2) && defined(WOLFSSL_SM3)
#define OCSP_DIGEST_SIZE WC_SM3_DIGEST_SIZE
#elif defined(NO_SHA)
#define OCSP_DIGEST_SIZE WC_SHA256_DIGEST_SIZE
#else
#define OCSP_DIGEST_SIZE WC_SHA_DIGEST_SIZE
#endif

struct OcspEntry
{
    OcspEntry *next;                      /* next entry                */
    word32 hashAlgoOID;                   /* hash algo ID              */
    byte issuerHash[OCSP_DIGEST_SIZE];    /* issuer hash               */
    byte issuerKeyHash[OCSP_DIGEST_SIZE]; /* issuer public key hash    */
    CertStatus *status;                   /* OCSP response list        */
    int totalStatus;                      /* number on list            */
    byte* rawCertId;                      /* raw bytes of the CertID   */
    int rawCertIdSize;                    /* num bytes in raw CertID   */
    /* option bits - using 32-bit for alignment */
    word32 ownStatus:1;                   /* do we need to free the status
                                           * response list */
    word32 isDynamic:1;                   /* was dynamically allocated */
    word32 used:1;                        /* entry used                */
};

/* TODO: Long-term, it would be helpful if we made this struct and other OCSP
         structs conform to the ASN spec as described in RFC 6960. It will help
         with readability and with implementing OpenSSL compatibility API
         functions, because OpenSSL's OCSP data structures conform to the
         RFC. */
struct OcspResponse {
    int     responseStatus;  /* return code from Responder */

    byte*   response;        /* Pointer to beginning of OCSP Response */
    word32  responseSz;      /* length of the OCSP Response */

    byte    producedDate[MAX_DATE_SIZE];
                             /* Date at which this response was signed */
    byte    producedDateFormat; /* format of the producedDate */

    byte*   cert;
    word32  certSz;

    byte*   sig;             /* Pointer to sig in source */
    word32  sigSz;           /* Length in octets for the sig */
    word32  sigOID;          /* OID for hash used for sig */

    OcspEntry* single;       /* chain of OCSP single responses */

    byte*   nonce;           /* pointer to nonce inside ASN.1 response */
    int     nonceSz;         /* length of the nonce string */

    byte*   source;          /* pointer to source buffer, not owned */
    word32  maxIdx;          /* max offset based on init size */

#ifdef OPENSSL_EXTRA
    int     verifyError;
#endif
    void*  heap;
};


struct OcspRequest {
    byte   issuerHash[KEYID_SIZE];
    byte   issuerKeyHash[KEYID_SIZE];
#if defined(WOLFSSL_SM2) && defined(WOLFSSL_SM3)
    int    hashSz;
#endif
    byte*  serial;   /* copy of the serial number in source cert */
    int    serialSz;
#ifdef OPENSSL_EXTRA
    WOLFSSL_ASN1_INTEGER* serialInt;
#endif
#if defined(OPENSSL_ALL) || defined(WOLFSSL_NGINX) || \
    defined(WOLFSSL_HAPROXY) || defined(WOLFSSL_APACHE_HTTPD) || \
    defined(HAVE_LIGHTY)
    void* cid; /* WOLFSSL_OCSP_CERTID kept to free */
#endif
    byte*  url;      /* copy of the extAuthInfo in source cert */
    int    urlSz;

    byte   nonce[MAX_OCSP_NONCE_SZ];
    int    nonceSz;
    void*  heap;
    void*  ssl;
};

WOLFSSL_LOCAL void InitOcspResponse(OcspResponse* resp, OcspEntry* single,
                     CertStatus* status, byte* source, word32 inSz, void* heap);
WOLFSSL_LOCAL void FreeOcspResponse(OcspResponse* resp);
WOLFSSL_LOCAL int OcspResponseDecode(OcspResponse* resp, void* cm, void* heap,
                                     int noVerify);

WOLFSSL_LOCAL int    InitOcspRequest(OcspRequest* req, DecodedCert* cert,
                                     byte useNonce, void* heap);
WOLFSSL_LOCAL void   FreeOcspRequest(OcspRequest* req);
WOLFSSL_LOCAL int    EncodeOcspRequest(OcspRequest* req, byte* output,
                                       word32 size);
WOLFSSL_LOCAL word32 EncodeOcspRequestExtensions(OcspRequest* req, byte* output,
                                                 word32 size);


WOLFSSL_LOCAL int  CompareOcspReqResp(OcspRequest* req, OcspResponse* resp);


#endif /* HAVE_OCSP */


/* for pointer use */
typedef struct RevokedCert RevokedCert;

#ifdef HAVE_CRL

struct RevokedCert {
    byte         serialNumber[EXTERNAL_SERIAL_SIZE];
    int          serialSz;
    RevokedCert* next;
    byte         revDate[MAX_DATE_SIZE];
    byte         revDateFormat;
};

typedef struct DecodedCRL DecodedCRL;

struct DecodedCRL {
    word32  certBegin;               /* offset to start of cert          */
    word32  sigIndex;                /* offset to start of signature     */
    word32  sigLength;               /* length of signature              */
    word32  signatureOID;            /* sum of algorithm object id       */
    byte*   signature;               /* pointer into raw source, not owned */
    byte    issuerHash[SIGNER_DIGEST_SIZE]; /* issuer name hash          */
    byte    crlHash[SIGNER_DIGEST_SIZE]; /* raw crl data hash            */
    byte    lastDate[MAX_DATE_SIZE]; /* last date updated  */
    byte    nextDate[MAX_DATE_SIZE]; /* next update date   */
    byte    lastDateFormat;          /* format of last date */
    byte    nextDateFormat;          /* format of next date */
    RevokedCert* certs;              /* revoked cert list  */
#if defined(OPENSSL_EXTRA)
    byte*   issuer;                  /* full name including common name  */
    word32  issuerSz;                /* length of the issuer             */
#endif
    int          totalCerts;         /* number on list     */
    int          version;            /* version of cert    */
    void*   heap;
#ifndef NO_SKID
    byte    extAuthKeyIdSet;
    byte    extAuthKeyId[SIGNER_DIGEST_SIZE]; /* Authority Key ID        */
#endif
    int          crlNumber;          /* CRL number extension  */
};

WOLFSSL_LOCAL void InitDecodedCRL(DecodedCRL* dcrl, void* heap);
WOLFSSL_LOCAL int VerifyCRL_Signature(SignatureCtx* sigCtx,
                                      const byte* toBeSigned, word32 tbsSz,
                                      const byte* signature, word32 sigSz,
                                      word32 signatureOID, Signer *ca,
                                      void* heap);
WOLFSSL_LOCAL int ParseCRL(RevokedCert* rcert, DecodedCRL* dcrl,
                           const byte* buff, word32 sz, int verify, void* cm);
WOLFSSL_LOCAL void FreeDecodedCRL(DecodedCRL* dcrl);


#endif /* HAVE_CRL */


#ifdef __cplusplus
    } /* extern "C" */
#endif

#endif /* !NO_ASN */


#if ((defined(HAVE_ED25519) && defined(HAVE_ED25519_KEY_IMPORT)) \
    || (defined(HAVE_CURVE25519) && defined(HAVE_CURVE25519_KEY_IMPORT)) \
    || (defined(HAVE_ED448) && defined(HAVE_ED448_KEY_IMPORT)) \
    || (defined(HAVE_CURVE448) && defined(HAVE_CURVE448_KEY_IMPORT)) \
    || (defined(HAVE_PQC) && defined(HAVE_FALCON)) \
    || (defined(HAVE_PQC) && defined(HAVE_DILITHIUM)) \
    || (defined(HAVE_PQC) && defined(HAVE_SPHINCS)))
WOLFSSL_LOCAL int DecodeAsymKey(const byte* input, word32* inOutIdx,
    word32 inSz, byte* privKey, word32* privKeyLen, byte* pubKey,
    word32* pubKeyLen, int keyType);
#endif

#ifdef WC_ENABLE_ASYM_KEY_EXPORT
WOLFSSL_LOCAL int SetAsymKeyDer(const byte* privKey, word32 privKeyLen,
    const byte* pubKey, word32 pubKeyLen, byte* output, word32 outLen,
    int keyType);
#endif


#if !defined(NO_ASN) || !defined(NO_PWDBASED)

#ifndef PKCS_MAX_KEY_SIZE
    #define PKCS_MAX_KEY_SIZE    64  /* MAX PKCS Key length */
#endif
#if !defined(WOLFSSL_GAME_BUILD) && !defined(MAX_KEY_SIZE)
    /* for backwards compatibility */
    #define MAX_KEY_SIZE PKCS_MAX_KEY_SIZE
#endif
#ifndef MAX_UNICODE_SZ
    #define MAX_UNICODE_SZ  256
#endif

enum PBESTypes {
    PBE_MD5_DES        = 0,
    PBE_SHA1_RC4_128   = 1,
    PBE_SHA1_DES       = 2,
    PBE_SHA1_DES3      = 3,
    PBE_AES256_CBC     = 4,
    PBE_AES128_CBC     = 5,
    PBE_SHA1_40RC2_CBC = 6,

    PBE_SHA1_RC4_128_SUM   = 657,
    PBE_SHA1_DES3_SUM      = 659,
    PBE_SHA1_40RC2_CBC_SUM = 662,
    PBE_MD5_DES_SUM        = 651,
    PBE_SHA1_DES_SUM       = 658,
    PBES2_SUM              = 661,

    PBES2              = 13,       /* algo ID */
    PBES1_MD5_DES      = 3,
    PBES1_SHA1_DES     = 10
};

enum PKCSTypes {
    PKCS5v2             =   6,     /* PKCS #5 v2.0 */
    PKCS12v1            =  12,     /* PKCS #12 */
    PKCS5               =   5,     /* PKCS oid tag */
    PKCS8v0             =   0,     /* default PKCS#8 version */
    PKCS8v1             =   1,     /* PKCS#8 version including public key */
    PKCS1v0             =   0,     /* default PKCS#1 version */
    PKCS1v1             =   1     /* Multi-prime version */
};

#endif /* !NO_ASN || !NO_PWDBASED */

#endif /* WOLF_CRYPT_ASN_H */
