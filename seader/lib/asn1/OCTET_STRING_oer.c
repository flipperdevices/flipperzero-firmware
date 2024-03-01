/*
 * Copyright (c) 2017 Lev Walkin <vlm@lionet.info>.
 * All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
#ifndef ASN_DISABLE_OER_SUPPORT

#include <asn_internal.h>
#include <OCTET_STRING.h>
#include <errno.h>

asn_dec_rval_t
OCTET_STRING_decode_oer(const asn_codec_ctx_t *opt_codec_ctx,
                        const asn_TYPE_descriptor_t *td,
                        const asn_oer_constraints_t *constraints, void **sptr,
                        const void *ptr, size_t size) {
    const asn_OCTET_STRING_specifics_t *specs =
        td->specifics ? (const asn_OCTET_STRING_specifics_t *)td->specifics
                      : &asn_SPC_OCTET_STRING_specs;
    OCTET_STRING_t *st = (OCTET_STRING_t *)*sptr;
    const asn_oer_constraints_t *cts =
        constraints ? constraints : td->encoding_constraints.oer_constraints;
    ssize_t ct_size = cts ? cts->size : -1;
    asn_dec_rval_t rval = {RC_OK, 0};
    size_t expected_length = 0;

    size_t unit_bytes;
    switch(specs->subvariant) {
    default:
    case ASN_OSUBV_BIT:
        ASN_DEBUG("Invalid use of OCTET STRING to decode BIT STRING");
        ASN__DECODE_FAILED;
    case ASN_OSUBV_ANY:
        /* Fall through */
    case ASN_OSUBV_STR:
        unit_bytes = 1;
        break;
    case ASN_OSUBV_U16:
        unit_bytes = 2;
        break;
    case ASN_OSUBV_U32:
        unit_bytes = 4;
        break;
    }

    (void)opt_codec_ctx;

    if(!st) {
        st = (OCTET_STRING_t *)(*sptr = CALLOC(1, specs->struct_size));
        if(!st) ASN__DECODE_FAILED;
    }

    if(ct_size >= 0) {
        expected_length = unit_bytes * ct_size;
    } else {
        /*
         * X.696 (08/2015) #27.2
         * Encode length determinant as _number of octets_, but only
         * if upper bound is not equal to lower bound.
         */
        ssize_t len_len = oer_fetch_length(ptr, size, &expected_length);
        if(len_len > 0) {
            rval.consumed = len_len;
            ptr = (const char *)ptr + len_len;
            size -= len_len;
        } else if(len_len == 0) {
            ASN__DECODE_STARVED;
        } else if(len_len < 0) {
            ASN__DECODE_FAILED;
        }

        if(expected_length % unit_bytes != 0) {
            ASN_DEBUG(
                "Data size %" ASN_PRI_SIZE " bytes is not consistent with multiplier %" ASN_PRI_SIZE "",
                expected_length, unit_bytes);
            ASN__DECODE_FAILED;
        }
    }

    if(size < expected_length) {
        ASN__DECODE_STARVED;
    } else {
        uint8_t *buf = MALLOC(expected_length + 1);
        if(buf == NULL) {
            ASN__DECODE_FAILED;
        } else {
            memcpy(buf, ptr, expected_length);
            buf[expected_length] = '\0';
        }
        FREEMEM(st->buf);
        st->buf = buf;
        st->size = expected_length;

        rval.consumed += expected_length;
        return rval;
    }
}

/*
 * Encode as Canonical OER.
 */
asn_enc_rval_t
OCTET_STRING_encode_oer(const asn_TYPE_descriptor_t *td,
                        const asn_oer_constraints_t *constraints,
                        const void *sptr, asn_app_consume_bytes_f *cb,
                        void *app_key) {
    const asn_OCTET_STRING_specifics_t *specs =
        td->specifics ? (const asn_OCTET_STRING_specifics_t *)td->specifics
                      : &asn_SPC_OCTET_STRING_specs;
    const OCTET_STRING_t *st = (const OCTET_STRING_t *)sptr;
    const asn_oer_constraints_t *cts =
        constraints ? constraints : td->encoding_constraints.oer_constraints;
    ssize_t ct_size = cts ? cts->size : -1;
    asn_enc_rval_t er = {0, 0, 0};

    if(!st) ASN__ENCODE_FAILED;

    ASN_DEBUG("Encoding %s %" ASN_PRI_SIZE " as OCTET STRING", td ? td->name : "", st->size);

    if(ct_size >= 0) {
        /*
         * Check that available data matches the constraint
         */
        size_t unit_bytes;
        switch(specs->subvariant) {
        default:
        case ASN_OSUBV_BIT:
            ASN_DEBUG("Invalid use of OCTET STRING to encode BIT STRING");
            ASN__ENCODE_FAILED;
        case ASN_OSUBV_ANY:
            /* Fall through */
        case ASN_OSUBV_STR:
            unit_bytes = 1;
            break;
        case ASN_OSUBV_U16:
            unit_bytes = 2;
            break;
        case ASN_OSUBV_U32:
            unit_bytes = 4;
            break;
        }

        if(st->size != unit_bytes * (size_t)ct_size) {
            ASN_DEBUG(
                "Trying to encode %s (%" ASN_PRI_SIZE " bytes) which doesn't fit SIZE "
                "constraint (%" ASN_PRI_SIZE ")",
                td->name, st->size, ct_size);
            ASN__ENCODE_FAILED;
        }
    } else {
        /*
         * X.696 (08/2015) #27.2
         * Encode length determinant as _number of octets_, but only
         * if upper bound is not equal to lower bound.
         */
        ssize_t ret = oer_serialize_length(st->size, cb, app_key);
        if(ret < 0) {
            ASN__ENCODE_FAILED;
        }
        er.encoded += ret;
    }

    er.encoded += st->size;
    if(cb(st->buf, st->size, app_key) < 0) {
        ASN__ENCODE_FAILED;
    } else {
        ASN__ENCODED_OK(er);
    }
}

#endif  /* ASN_DISABLE_OER_SUPPORT */
