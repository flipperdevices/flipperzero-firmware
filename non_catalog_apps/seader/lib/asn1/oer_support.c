/*
 * Copyright (c) 2017 Lev Walkin <vlm@lionet.info>.
 * All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
#include <asn_system.h>
#include <asn_internal.h>

#include <oer_support.h>

/*
 * Fetch the length determinant (X.696 08/2015, #8.6) into *len_r.
 * RETURN VALUES:
 *       0:     More data expected than bufptr contains.
 *      -1:     Fatal error deciphering length.
 *      >0:     Number of bytes used from bufptr.
 */
ssize_t
oer_fetch_length(const void *bufptr, size_t size, size_t *len_r) {
    uint8_t first_byte;
    size_t len_len;    /* Length of the length determinant */
    const uint8_t *b;
    const uint8_t *bend;
    size_t len;

    if(size == 0) {
        *len_r = 0;
        return 0;
    }

    first_byte = *(const uint8_t *)bufptr;
    if((first_byte & 0x80) == 0) {   /* Short form */
        *len_r = first_byte; /* 0..127 */
        return 1;
    }

    len_len = (first_byte & 0x7f);
    if((1 + len_len) > size) {
        *len_r = 0;
        return 0;
    }

    b = (const uint8_t *)bufptr + 1;
    bend = b + len_len;

    for(; b < bend && *b == 0; b++) {
        /* Skip the leading 0-bytes */
    }

    if((bend - b) > (ssize_t)sizeof(size_t)) {
        /* Length is not representable by the native size_t type */
        *len_r = 0;
        return -1;
    }

    for(len = 0; b < bend; b++) {
        len = (len << 8) + *b;
    }

    if(len > RSIZE_MAX) { /* A bit of C11 validation */
        *len_r = 0;
        return -1;
    }

    *len_r = len;
    assert(len_len + 1 == (size_t)(bend - (const uint8_t *)bufptr));
    return len_len + 1;
}


/*
 * Serialize OER length. Returns the number of bytes serialized
 * or -1 if a given callback returned with negative result.
 */
ssize_t
oer_serialize_length(size_t length, asn_app_consume_bytes_f *cb,
                     void *app_key) {
    uint8_t scratch[1 + sizeof(length)];
    uint8_t *sp = scratch;
    int littleEndian = 1;   /* Run-time detection */
    const uint8_t *pstart;
    const uint8_t *pend;
    const uint8_t *p;
    int add;

    if(length <= 127) {
        uint8_t b = length;
        if(cb(&b, 1, app_key) < 0) {
            return -1;
        }
        return 1;
    }

    if(*(char *)&littleEndian) {
        pstart = (const uint8_t *)&length + sizeof(length) - 1;
        pend = (const uint8_t *)&length;
        add = -1;
    } else {
        pstart = (const uint8_t *)&length;
        pend = pstart + sizeof(length);
        add = 1;
    }

    for(p = pstart; p != pend; p += add) {
        /* Skip leading zeros. */
        if(*p) break;
    }

    for(sp = scratch + 1; ; p += add) {
        *sp++ = *p;
        if(p == pend) break;
    }
    assert((sp - scratch) - 1 <= 0x7f);
    scratch[0] = 0x80 + ((sp - scratch) - 1);

    if(cb(scratch, sp - scratch, app_key) < 0) {
        return -1;
    }

    return sp - scratch;
}

