/*
 * Copyright (c) 2003-2017 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
#ifndef	_CONSTR_CHOICE_H_
#define	_CONSTR_CHOICE_H_

#include <asn_application.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct asn_CHOICE_specifics_s {
	/*
	 * Target structure description.
	 */
	unsigned struct_size;       /* Size of the target structure. */
	unsigned ctx_offset;        /* Offset of the asn_codec_ctx_t member */
	unsigned pres_offset;       /* Identifier of the present member */
	unsigned pres_size;         /* Size of the identifier (enum) */

	/*
	 * Tags to members mapping table.
	 */
	const asn_TYPE_tag2member_t *tag2el;
	unsigned tag2el_count;

	/* Canonical ordering of CHOICE elements, for PER */
	const unsigned *to_canonical_order;
	const unsigned *from_canonical_order;

	/*
	 * Extensions-related stuff.
	 */
	signed ext_start; /* First member of extensions, or -1 */
} asn_CHOICE_specifics_t;

/*
 * A set specialized functions dealing with the CHOICE type.
 */
asn_struct_free_f CHOICE_free;
asn_struct_print_f CHOICE_print;
asn_struct_compare_f CHOICE_compare;
asn_constr_check_f CHOICE_constraint;
ber_type_decoder_f CHOICE_decode_ber;
der_type_encoder_f CHOICE_encode_der;
xer_type_decoder_f CHOICE_decode_xer;
xer_type_encoder_f CHOICE_encode_xer;
oer_type_decoder_f CHOICE_decode_oer;
oer_type_encoder_f CHOICE_encode_oer;
per_type_decoder_f CHOICE_decode_uper;
per_type_encoder_f CHOICE_encode_uper;
asn_outmost_tag_f CHOICE_outmost_tag;
asn_random_fill_f CHOICE_random_fill;
extern asn_TYPE_operation_t asn_OP_CHOICE;

/*
 * Return the 1-based choice variant presence index.
 * Returns 0 in case of error.
 */
unsigned CHOICE_variant_get_presence(const asn_TYPE_descriptor_t *td,
                                     const void *structure_ptr);

/*
 * Sets or resets the 1-based choice variant presence index.
 * In case a previous index is not zero, the currently selected structure
 * member is freed and zeroed-out first.
 * Returns 0 on success and -1 on error.
 */
int CHOICE_variant_set_presence(const asn_TYPE_descriptor_t *td,
                                void *structure_ptr, unsigned present);

#ifdef __cplusplus
}
#endif

#endif	/* _CONSTR_CHOICE_H_ */
