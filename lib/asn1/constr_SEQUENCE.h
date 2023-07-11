/*-
 * Copyright (c) 2003-2017 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
#ifndef	_CONSTR_SEQUENCE_H_
#define	_CONSTR_SEQUENCE_H_

#include <asn_application.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct asn_SEQUENCE_specifics_s {
	/*
	 * Target structure description.
	 */
	unsigned struct_size;	/* Size of the target structure. */
	unsigned ctx_offset;	/* Offset of the asn_struct_ctx_t member */

	/*
	 * Tags to members mapping table (sorted).
	 */
	const asn_TYPE_tag2member_t *tag2el;
	unsigned tag2el_count;

	/*
	 * Optional members of the extensions root (roms) or additions (aoms).
	 * Meaningful for PER.
	 */
	const int *oms;         /* Optional MemberS */
	unsigned roms_count;    /* Root optional members count */
	unsigned aoms_count;    /* Additions optional members count */

	/*
	 * Description of an extensions group.
	 * Root components are clustered at the beginning of the structure,
	 * whereas extensions are clustered at the end. -1 means not extensible.
	 */
	signed first_extension;       /* First extension addition */
} asn_SEQUENCE_specifics_t;


/*
 * A set specialized functions dealing with the SEQUENCE type.
 */
asn_struct_free_f SEQUENCE_free;
asn_struct_print_f SEQUENCE_print;
asn_struct_compare_f SEQUENCE_compare;
asn_constr_check_f SEQUENCE_constraint;
ber_type_decoder_f SEQUENCE_decode_ber;
der_type_encoder_f SEQUENCE_encode_der;
xer_type_decoder_f SEQUENCE_decode_xer;
xer_type_encoder_f SEQUENCE_encode_xer;
oer_type_decoder_f SEQUENCE_decode_oer;
oer_type_encoder_f SEQUENCE_encode_oer;
per_type_decoder_f SEQUENCE_decode_uper;
per_type_encoder_f SEQUENCE_encode_uper;
asn_random_fill_f  SEQUENCE_random_fill;
extern asn_TYPE_operation_t asn_OP_SEQUENCE;

#ifdef __cplusplus
}
#endif

#endif	/* _CONSTR_SEQUENCE_H_ */
