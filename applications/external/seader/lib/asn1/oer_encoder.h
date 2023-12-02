/*
 * Copyright (c) 2017 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
#ifndef	OER_ENCODER_H
#define	OER_ENCODER_H

#include <asn_application.h>

#ifdef __cplusplus
extern "C" {
#endif

struct asn_TYPE_descriptor_s;	/* Forward declaration */

/*
 * The Octet Encoding Rules (OER, X.696 08/2015) encoder for any type.
 * This function may be invoked directly by the application.
 * Produces CANONICAL-OER output compatible with CANONICAL-OER
 * and BASIC-OER decoders.
 */
asn_enc_rval_t oer_encode(const struct asn_TYPE_descriptor_s *type_descriptor,
                          const void *struct_ptr, /* Structure to be encoded */
                          asn_app_consume_bytes_f *consume_bytes_cb,
                          void *app_key /* Arbitrary callback argument */
);

/* A variant of oer_encode() which encodes data into the pre-allocated buffer */
asn_enc_rval_t oer_encode_to_buffer(
    const struct asn_TYPE_descriptor_s *type_descriptor,
    const asn_oer_constraints_t *constraints,
    const void *struct_ptr, /* Structure to be encoded */
    void *buffer,           /* Pre-allocated buffer */
    size_t buffer_size      /* Initial buffer size (maximum) */
);

/*
 * Type of the generic OER encoder.
 */
typedef asn_enc_rval_t(oer_type_encoder_f)(
    const struct asn_TYPE_descriptor_s *type_descriptor,
    const asn_oer_constraints_t *constraints,
    const void *struct_ptr,                    /* Structure to be encoded */
    asn_app_consume_bytes_f *consume_bytes_cb, /* Callback */
    void *app_key                              /* Arbitrary callback argument */
);

/*
 * Write out the Open Type (X.696 (08/2015), #30).
 * RETURN VALUES:
 *  -1: Fatal error encoding the type.
 *  >0: Number of bytes serialized.
 */
ssize_t oer_open_type_put(const struct asn_TYPE_descriptor_s *td,
                          const asn_oer_constraints_t *constraints,
                          const void *struct_ptr,
                          asn_app_consume_bytes_f *consume_bytes_cb,
                          void *app_key);


/*
 * Length-prefixed buffer encoding for primitive types.
 */
oer_type_encoder_f oer_encode_primitive;

#ifdef __cplusplus
}
#endif

#endif	/* OER_ENCODER_H */
