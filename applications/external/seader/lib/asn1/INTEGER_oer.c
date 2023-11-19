/*
 * Copyright (c) 2017 Lev Walkin <vlm@lionet.info>.
 * All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
#ifndef ASN_DISABLE_OER_SUPPORT

#include <asn_internal.h>
#include <INTEGER.h>
#include <errno.h>

asn_dec_rval_t
INTEGER_decode_oer(const asn_codec_ctx_t *opt_codec_ctx,
                   const asn_TYPE_descriptor_t *td,
                   const asn_oer_constraints_t *constraints, void **sptr,
                   const void *ptr, size_t size) {
    const asn_INTEGER_specifics_t *specs =
        (const asn_INTEGER_specifics_t *)td->specifics;
    asn_dec_rval_t rval = {RC_OK, 0};
    INTEGER_t *st = (INTEGER_t *)*sptr;
    struct asn_oer_constraint_number_s ct = {0, 0};
    size_t req_bytes;

    (void)opt_codec_ctx;
    (void)specs;

    if(!st) {
        st = (INTEGER_t *)(*sptr = CALLOC(1, sizeof(*st)));
        if(!st) ASN__DECODE_FAILED;
    }

    FREEMEM(st->buf);
    st->buf = 0;
    st->size = 0;

    if(!constraints) constraints = td->encoding_constraints.oer_constraints;
    if(constraints) ct = constraints->value;

    if(ct.width) {
        req_bytes = ct.width;
    } else {
        /* No lower bound and no upper bound, effectively */

        ssize_t consumed = oer_fetch_length(ptr, size, &req_bytes);
        if(consumed == 0) {
            ASN__DECODE_STARVED;
        } else if(consumed == -1) {
            ASN__DECODE_FAILED;
        }
        rval.consumed += consumed;
        ptr = (const char *)ptr + consumed;
        size -= consumed;
    }

    if(req_bytes > size) {
        ASN__DECODE_STARVED;
    }

    if(ct.positive) {
        /* X.969 08/2015 10.2(a) */
        unsigned msb;   /* Most significant bit */
        size_t useful_size;

        /* Check most significant bit */
        msb = *(const uint8_t *)ptr >> 7; /* yields 0 or 1 */
        useful_size = msb + req_bytes;
        st->buf = (uint8_t *)MALLOC(useful_size + 1);
        if(!st->buf) {
            ASN__DECODE_FAILED;
        }

        /*
         * Record a large unsigned in a way not to confuse it
         * with signed value.
         */
        st->buf[0] = '\0';
        memcpy(st->buf + msb, ptr, req_bytes);
        st->buf[useful_size] = '\0';    /* Just in case, 0-terminate */
        st->size = useful_size;

        rval.consumed += req_bytes;
        return rval;
    } else {
        /* X.969 08/2015 10.2(b) */
        st->buf = (uint8_t *)MALLOC(req_bytes + 1);
        if(!st->buf) {
            ASN__DECODE_FAILED;
        }

        memcpy(st->buf, ptr, req_bytes);
        st->buf[req_bytes] = '\0'; /* Just in case, 0-terminate */
        st->size = req_bytes;

        rval.consumed += req_bytes;
        return rval;
    }
}

/*
 * Encode as Canonical OER.
 */
asn_enc_rval_t
INTEGER_encode_oer(const asn_TYPE_descriptor_t *td,
                   const asn_oer_constraints_t *constraints, const void *sptr,
                   asn_app_consume_bytes_f *cb, void *app_key) {
    const INTEGER_t *st = sptr;
    asn_enc_rval_t er;
    struct asn_oer_constraint_number_s ct = {0, 0};
    const uint8_t *buf;
    const uint8_t *end;
    size_t useful_bytes;
    size_t req_bytes = 0;
    int sign = 0;

    if(!st || st->size == 0) ASN__ENCODE_FAILED;

    if(!constraints) constraints = td->encoding_constraints.oer_constraints;
    if(constraints) ct = constraints->value;

    er.encoded = 0;

    buf = st->buf;
    end = buf + st->size;

    sign = (buf && buf < end) ? buf[0] & 0x80 : 0;

    /* Ignore 9 leading zeroes or ones */
    if(ct.positive) {
        if(sign) {
            /* The value given is a signed value. Can't proceed. */
            ASN__ENCODE_FAILED;
        }
        /* Remove leading zeros. */
        for(; buf + 1 < end; buf++) {
            if(buf[0] != 0x0) break;
        }
    } else {
        for(; buf + 1 < end; buf++) {
            if(buf[0] == 0x0 && (buf[1] & 0x80) == 0) {
                continue;
            } else if(buf[0] == 0xff && (buf[1] & 0x80) != 0) {
                continue;
            }
            break;
        }
    }

    useful_bytes = end - buf;
    if(ct.width) {
        req_bytes = ct.width;
    } else {
        ssize_t r = oer_serialize_length(useful_bytes, cb, app_key);
        if(r < 0) {
            ASN__ENCODE_FAILED;
        }
        er.encoded += r;
        req_bytes = useful_bytes;
    }

    if(req_bytes < useful_bytes) {
        ASN__ENCODE_FAILED;
    }

    er.encoded += req_bytes;

    for(; req_bytes > useful_bytes; req_bytes--) {
        if(cb(sign?"\xff":"\0", 1, app_key) < 0) {
            ASN__ENCODE_FAILED;
        }
    }

    if(cb(buf, useful_bytes, app_key) < 0) {
        ASN__ENCODE_FAILED;
    }

    ASN__ENCODED_OK(er);
}

#endif  /* ASN_DISABLE_OER_SUPPORT */
