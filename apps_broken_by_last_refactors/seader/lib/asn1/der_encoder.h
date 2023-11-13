/*-
 * Copyright (c) 2003-2017 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
#ifndef	_DER_ENCODER_H_
#define	_DER_ENCODER_H_

#include <asn_application.h>

#ifdef __cplusplus
extern "C" {
#endif

struct asn_TYPE_descriptor_s;	/* Forward declaration */

/*
 * The DER encoder of any type. May be invoked by the application.
 * Produces DER- and BER-compliant encoding. (DER is a subset of BER).
 *
 * NOTE: Use the ber_decode() function (ber_decoder.h) to decode data
 * produced by der_encode().
 */
asn_enc_rval_t der_encode(const struct asn_TYPE_descriptor_s *type_descriptor,
                          const void *struct_ptr, /* Structure to be encoded */
                          asn_app_consume_bytes_f *consume_bytes_cb,
                          void *app_key /* Arbitrary callback argument */
);

/* A variant of der_encode() which encodes data into the pre-allocated buffer */
asn_enc_rval_t der_encode_to_buffer(
    const struct asn_TYPE_descriptor_s *type_descriptor,
    const void *struct_ptr, /* Structure to be encoded */
    void *buffer,           /* Pre-allocated buffer */
    size_t buffer_size      /* Initial buffer size (maximum) */
);

/*
 * Type of the generic DER encoder.
 */
typedef asn_enc_rval_t(der_type_encoder_f)(
    const struct asn_TYPE_descriptor_s *type_descriptor,
    const void *struct_ptr, /* Structure to be encoded */
    int tag_mode,           /* {-1,0,1}: IMPLICIT, no, EXPLICIT */
    ber_tlv_tag_t tag, asn_app_consume_bytes_f *consume_bytes_cb, /* Callback */
    void *app_key /* Arbitrary callback argument */
);


/*******************************
 * INTERNALLY USEFUL FUNCTIONS *
 *******************************/

/*
 * Write out leading TL[v] sequence according to the type definition.
 */
ssize_t der_write_tags(const struct asn_TYPE_descriptor_s *type_descriptor,
                       size_t struct_length,
                       int tag_mode,      /* {-1,0,1}: IMPLICIT, no, EXPLICIT */
                       int last_tag_form, /* {0,!0}: prim, constructed */
                       ber_tlv_tag_t tag,
                       asn_app_consume_bytes_f *consume_bytes_cb,
                       void *app_key);

#ifdef __cplusplus
}
#endif

#endif	/* _DER_ENCODER_H_ */
