/*
 * Copyright (c) 2017 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
#include <asn_internal.h>
#include <asn_codecs_prim.h>

/*
 * The OER encoder of any type.
 */
asn_enc_rval_t
oer_encode(const asn_TYPE_descriptor_t *type_descriptor, const void *struct_ptr,
           asn_app_consume_bytes_f *consume_bytes, void *app_key) {
    ASN_DEBUG("OER encoder invoked for %s", type_descriptor->name);

    /*
     * Invoke type-specific encoder.
     */
    return type_descriptor->op->oer_encoder(
        type_descriptor, 0,
        struct_ptr, /* Pointer to the destination structure */
        consume_bytes, app_key);
}

/*
 * Argument type and callback necessary for oer_encode_to_buffer().
 */
typedef struct enc_to_buf_arg {
        void *buffer;
        size_t left;
} enc_to_buf_arg;
static int
encode_to_buffer_cb(const void *buffer, size_t size, void *key) {
    enc_to_buf_arg *arg = (enc_to_buf_arg *)key;

    if(arg->left < size) return -1; /* Data exceeds the available buffer size */

    memcpy(arg->buffer, buffer, size);
    arg->buffer = ((char *)arg->buffer) + size;
    arg->left -= size;

    return 0;
}

/*
 * A variant of the oer_encode() which encodes the data into the provided buffer
 */
asn_enc_rval_t
oer_encode_to_buffer(const asn_TYPE_descriptor_t *type_descriptor,
                     const asn_oer_constraints_t *constraints,
                     const void *struct_ptr, /* Structure to be encoded */
                     void *buffer,           /* Pre-allocated buffer */
                     size_t buffer_size      /* Initial buffer size (maximum) */
) {
    enc_to_buf_arg arg;
    asn_enc_rval_t ec;

    arg.buffer = buffer;
    arg.left = buffer_size;

    if(type_descriptor->op->oer_encoder == NULL) {
        ec.encoded = -1;
        ec.failed_type = type_descriptor;
        ec.structure_ptr = struct_ptr;
        ASN_DEBUG("OER encoder is not defined for %s",
                type_descriptor->name);
    } else {
        ec = type_descriptor->op->oer_encoder(
            type_descriptor, constraints,
            struct_ptr, /* Pointer to the destination structure */
            encode_to_buffer_cb, &arg);
        if(ec.encoded != -1) {
            assert(ec.encoded == (ssize_t)(buffer_size - arg.left));
            /* Return the encoded contents size */
        }
    }
    return ec;
}

asn_enc_rval_t
oer_encode_primitive(const asn_TYPE_descriptor_t *td,
                     const asn_oer_constraints_t *constraints, const void *sptr,
                     asn_app_consume_bytes_f *cb, void *app_key) {
    const ASN__PRIMITIVE_TYPE_t *st = (const ASN__PRIMITIVE_TYPE_t *)sptr;
    asn_enc_rval_t er = {0, 0, 0};
    ssize_t ret;

    (void)constraints;

    if(!st) ASN__ENCODE_FAILED;

    ASN_DEBUG("Encoding %s (%" ASN_PRI_SIZE " bytes)", td ? td->name : "", st->size);

    /*
     * X.696 (08/2015) #27.2
     */
    ret = oer_serialize_length(st->size, cb, app_key);
    if(ret < 0) {
        ASN__ENCODE_FAILED;
    }
    er.encoded += ret;

    er.encoded += st->size;
    if(cb(st->buf, st->size, app_key) < 0) {
        ASN__ENCODE_FAILED;
    } else {
        ASN__ENCODED_OK(er);
    }
}

static int
oer__count_bytes(const void *buffer, size_t size, void *bytes_ptr) {
    size_t *bytes = bytes_ptr;
    (void)buffer;
    *bytes += size;
    return 0;
}

ssize_t
oer_open_type_put(const asn_TYPE_descriptor_t *td,
                  const asn_oer_constraints_t *constraints, const void *sptr,
                  asn_app_consume_bytes_f *cb, void *app_key) {
    size_t serialized_byte_count = 0;
    asn_enc_rval_t er;
    ssize_t len_len;

    er = td->op->oer_encoder(td, constraints, sptr, oer__count_bytes,
                             &serialized_byte_count);
    if(er.encoded < 0) return -1;
    assert(serialized_byte_count == (size_t)er.encoded);

    len_len = oer_serialize_length(serialized_byte_count, cb, app_key);
    if(len_len == -1) return -1;

    er = td->op->oer_encoder(td, constraints, sptr, cb, app_key);
    if(er.encoded < 0) return -1;
    assert(serialized_byte_count == (size_t)er.encoded);

    return len_len + er.encoded;
}

