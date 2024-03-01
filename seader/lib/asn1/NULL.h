/*-
 * Copyright (c) 2003-2017 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
#ifndef	ASN_TYPE_NULL_H
#define	ASN_TYPE_NULL_H

#include <asn_application.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * The value of the NULL type is meaningless.
 * Use the BOOLEAN type if you need to carry true/false semantics.
 */
typedef int NULL_t;

extern asn_TYPE_descriptor_t asn_DEF_NULL;
extern asn_TYPE_operation_t asn_OP_NULL;

asn_struct_free_f NULL_free;
asn_struct_print_f NULL_print;
asn_struct_compare_f NULL_compare;
ber_type_decoder_f NULL_decode_ber;
der_type_encoder_f NULL_encode_der;
xer_type_decoder_f NULL_decode_xer;
xer_type_encoder_f NULL_encode_xer;
oer_type_decoder_f NULL_decode_oer;
oer_type_encoder_f NULL_encode_oer;
per_type_decoder_f NULL_decode_uper;
per_type_encoder_f NULL_encode_uper;
asn_random_fill_f  NULL_random_fill;

#define NULL_constraint	asn_generic_no_constraint

#ifdef __cplusplus
}
#endif

#endif	/* NULL_H */
