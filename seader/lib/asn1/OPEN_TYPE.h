/*-
 * Copyright (c) 2017-2017 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
#ifndef ASN_OPEN_TYPE_H
#define ASN_OPEN_TYPE_H

#include <asn_application.h>

#ifdef __cplusplus
extern "C" {
#endif

#define OPEN_TYPE_free CHOICE_free
#define OPEN_TYPE_print CHOICE_print
#define OPEN_TYPE_compare CHOICE_compare
#define OPEN_TYPE_constraint CHOICE_constraint
#define OPEN_TYPE_decode_ber NULL
#define OPEN_TYPE_encode_der CHOICE_encode_der
#define OPEN_TYPE_decode_xer NULL
#define OPEN_TYPE_encode_xer CHOICE_encode_xer
#define OPEN_TYPE_decode_uper NULL

extern asn_TYPE_operation_t asn_OP_OPEN_TYPE;

/*
 * Decode an Open Type which is potentially constraiend
 * by the other members of the parent structure.
 */
asn_dec_rval_t OPEN_TYPE_ber_get(const asn_codec_ctx_t *opt_codec_ctx,
                                 const asn_TYPE_descriptor_t *parent_type,
                                 void *parent_structure,
                                 const asn_TYPE_member_t *element,
                                 const void *ptr, size_t size);

asn_dec_rval_t OPEN_TYPE_xer_get(const asn_codec_ctx_t *opt_codec_ctx,
                                 const asn_TYPE_descriptor_t *parent_type,
                                 void *parent_structure,
                                 const asn_TYPE_member_t *element,
                                 const void *ptr, size_t size);

asn_dec_rval_t OPEN_TYPE_oer_get(const asn_codec_ctx_t *opt_codec_ctx,
                                 const asn_TYPE_descriptor_t *parent_type,
                                 void *parent_structure,
                                 asn_TYPE_member_t *element, const void *ptr,
                                 size_t size);

asn_dec_rval_t OPEN_TYPE_uper_get(const asn_codec_ctx_t *opt_codec_ctx,
                                  const asn_TYPE_descriptor_t *parent_type,
                                  void *parent_structure,
                                  const asn_TYPE_member_t *element,
                                  asn_per_data_t *pd);

asn_enc_rval_t OPEN_TYPE_encode_uper(
    const asn_TYPE_descriptor_t *type_descriptor,
    const asn_per_constraints_t *constraints, const void *struct_ptr,
    asn_per_outp_t *per_output);

#ifdef __cplusplus
}
#endif

#endif	/* ASN_OPEN_TYPE_H */
