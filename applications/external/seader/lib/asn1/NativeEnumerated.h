/*
 * Copyright (c) 2004-2017 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
/*
 * This type differs from the standard ENUMERATED in that it is modelled using
 * the fixed machine type (long, int, short), so it can hold only values of
 * limited length. There is no type (i.e., NativeEnumerated_t, any integer type
 * will do).
 * This type may be used when integer range is limited by subtype constraints.
 */
#ifndef	_NativeEnumerated_H_
#define	_NativeEnumerated_H_

#include <NativeInteger.h>

#ifdef __cplusplus
extern "C" {
#endif

extern asn_TYPE_descriptor_t asn_DEF_NativeEnumerated;
extern asn_TYPE_operation_t asn_OP_NativeEnumerated;

xer_type_encoder_f NativeEnumerated_encode_xer;
oer_type_decoder_f NativeEnumerated_decode_oer;
oer_type_encoder_f NativeEnumerated_encode_oer;
per_type_decoder_f NativeEnumerated_decode_uper;
per_type_encoder_f NativeEnumerated_encode_uper;

#define NativeEnumerated_free       NativeInteger_free
#define NativeEnumerated_print      NativeInteger_print
#define NativeEnumerated_compare    NativeInteger_compare
#define NativeEnumerated_random_fill NativeInteger_random_fill
#define NativeEnumerated_constraint asn_generic_no_constraint
#define NativeEnumerated_decode_ber NativeInteger_decode_ber
#define NativeEnumerated_encode_der NativeInteger_encode_der
#define NativeEnumerated_decode_xer NativeInteger_decode_xer

#ifdef __cplusplus
}
#endif

#endif	/* _NativeEnumerated_H_ */
