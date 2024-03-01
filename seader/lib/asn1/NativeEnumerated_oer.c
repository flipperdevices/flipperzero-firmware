/*
 * Copyright (c) 2017 Lev Walkin <vlm@lionet.info>.
 * All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
#ifndef ASN_DISABLE_OER_SUPPORT

#include <asn_internal.h>
#include <NativeEnumerated.h>
#include <errno.h>

static long
asn__nativeenumerated_convert(const uint8_t *b, const uint8_t *end) {
    unsigned long value;

    /* Perform the sign initialization */
    /* Actually value = -(*b >> 7); gains nothing, yet unreadable! */
    if((*b >> 7)) {
        value = (unsigned long)(-1);
    } else {
        value = 0;
    }

    /* Conversion engine */
    for(; b < end; b++) {
        value = (value << 8) | *b;
    }

    return value;
}

asn_dec_rval_t
NativeEnumerated_decode_oer(const asn_codec_ctx_t *opt_codec_ctx,
                            const asn_TYPE_descriptor_t *td,
                            const asn_oer_constraints_t *constraints,
                            void **nint_ptr, const void *ptr, size_t size) {
    asn_dec_rval_t rval = {RC_OK, 0};
    long *native = (long *)*nint_ptr;
    const uint8_t *b = ptr;

    (void)opt_codec_ctx;
    (void)constraints;

    if(size < 1) {
        ASN__DECODE_STARVED;
    }

    if((*b & 0x80) == 0) {
        /*
         * X.696 (08/2015) #11.2 Short form for Enumerated.
         */
        if(!native) {
            native = (long *)(*nint_ptr = CALLOC(1, sizeof(*native)));
            if(!native) ASN__DECODE_FAILED;
        }

        *native = *b;
        rval.consumed = 1;
    } else {
        /*
         * X.696 (08/2015) #11.4 Long form for Enumerated.
         */
        size_t length = *b & 0x7f;
        const uint8_t *bend;
        long value;

        if(length < 1 || length > sizeof(*native)) {
            ASN__DECODE_FAILED;
        }
        if((1 + length) > size) {
            ASN__DECODE_STARVED;
        }
        b++;
        bend = b + length;

        value = asn__nativeenumerated_convert(b, bend);
        if(value < 0) {
            const asn_INTEGER_specifics_t *specs =
                (const asn_INTEGER_specifics_t *)td->specifics;
            if(specs && specs->field_unsigned) {
                ASN__DECODE_FAILED;
            }
        }

        if(!native) {
            native = (long *)(*nint_ptr = CALLOC(1, sizeof(*native)));
            if(!native) ASN__DECODE_FAILED;
        }

        *native = value;

        rval.consumed = (1 + length);
    }

    return rval;
}

/*
 * Encode as Canonical OER.
 */
asn_enc_rval_t
NativeEnumerated_encode_oer(const asn_TYPE_descriptor_t *td,
                            const asn_oer_constraints_t *constraints,
                            const void *sptr, asn_app_consume_bytes_f *cb,
                            void *app_key) {
    asn_enc_rval_t er;
    long native;

    (void)constraints;

    if(!sptr) ASN__ENCODE_FAILED;

    native = *(const long *)sptr;

    if(native >= 0 && native <= 127) {
        /* #11.2 Short form */
        uint8_t b = native;
        er.encoded = 1;
        if(cb(&b, er.encoded, app_key) < 0) {
            ASN__ENCODE_FAILED;
        }
        ASN__ENCODED_OK(er);
    } else {
        /* #11.2 Long form */
        uint8_t buf[1 + sizeof(native)];
        uint8_t *b = &buf[sizeof(native)];  /* Last addressable */
        long final_pattern = -1 * (native < 0);

        for(;;) {
            *b-- = native;
            native >>= 8;
            if(native == final_pattern) {
                if(final_pattern) {
                    if((b[1] & 0x80)) break;
                } else {
                    if(!(b[1] & 0x80)) break;
                }
            }
        }
        *b = 0x80 | (&buf[sizeof(native)] - b);
        er.encoded = 1 + (&buf[sizeof(native)] - b);
        if(cb(b, er.encoded, app_key) < 0) {
            ASN__ENCODE_FAILED;
        }
        ASN__ENCODED_OK(er);
    }
}

#endif  /* ASN_DISABLE_OER_SUPPORT */
