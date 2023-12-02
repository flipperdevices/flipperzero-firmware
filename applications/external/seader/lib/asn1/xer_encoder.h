/*-
 * Copyright (c) 2004-2017 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
#ifndef	_XER_ENCODER_H_
#define	_XER_ENCODER_H_

#include <asn_application.h>

#ifdef __cplusplus
extern "C" {
#endif

struct asn_TYPE_descriptor_s;	/* Forward declaration */

/* Flags used by the xer_encode() and (*xer_type_encoder_f), defined below */
enum xer_encoder_flags_e {
	/* Mode of encoding */
	XER_F_BASIC	= 0x01,	/* BASIC-XER (pretty-printing) */
	XER_F_CANONICAL	= 0x02	/* Canonical XER (strict rules) */
};

/*
 * The XER encoder of any type. May be invoked by the application.
 * Produces CANONICAL-XER and BASIC-XER depending on the (xer_flags).
 */
asn_enc_rval_t xer_encode(const struct asn_TYPE_descriptor_s *type_descriptor,
                          const void *struct_ptr, /* Structure to be encoded */
                          enum xer_encoder_flags_e xer_flags,
                          asn_app_consume_bytes_f *consume_bytes_cb,
                          void *app_key /* Arbitrary callback argument */
);

/*
 * The variant of the above function which dumps the BASIC-XER (XER_F_BASIC)
 * output into the chosen file pointer.
 * RETURN VALUES:
 * 	 0: The structure is printed.
 * 	-1: Problem printing the structure.
 * WARNING: No sensible errno value is returned.
 */
int xer_fprint(FILE *stream, const struct asn_TYPE_descriptor_s *td,
               const void *struct_ptr);

/*
 * A helper function that uses XER encoding/decoding to verify that:
 * - Both structures encode into the same BASIC XER.
 * - Both resulting XER byte streams can be decoded back.
 * - Both decoded structures encode into the same BASIC XER (round-trip).
 * All of this verifies equivalence between structures and a round-trip.
 * ARGUMENTS:
 *  (opt_debug_stream)  - If specified, prints ongoing details.
 */
enum xer_equivalence_e {
    XEQ_SUCCESS,          /* The only completely positive return value */
    XEQ_FAILURE,          /* General failure */
    XEQ_ENCODE1_FAILED,   /* First sructure XER encoding failed */
    XEQ_ENCODE2_FAILED,   /* Second structure XER encoding failed */
    XEQ_DIFFERENT,        /* Structures encoded into different XER */
    XEQ_DECODE_FAILED,    /* Decode of the XER data failed */
    XEQ_ROUND_TRIP_FAILED /* Bad round-trip */
};
enum xer_equivalence_e xer_equivalent(
    const struct asn_TYPE_descriptor_s *type_descriptor, const void *struct1,
    const void *struct2, FILE *opt_debug_stream);

/*
 * Type of the generic XER encoder.
 */
typedef asn_enc_rval_t(xer_type_encoder_f)(
    const struct asn_TYPE_descriptor_s *type_descriptor,
    const void *struct_ptr, /* Structure to be encoded */
    int ilevel,             /* Level of indentation */
    enum xer_encoder_flags_e xer_flags,
    asn_app_consume_bytes_f *consume_bytes_cb, /* Callback */
    void *app_key                              /* Arbitrary callback argument */
);

#ifdef __cplusplus
}
#endif

#endif	/* _XER_ENCODER_H_ */
