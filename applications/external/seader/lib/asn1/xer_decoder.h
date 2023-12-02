/*-
 * Copyright (c) 2004-2017 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
#ifndef	_XER_DECODER_H_
#define	_XER_DECODER_H_

#include <asn_application.h>

#ifdef __cplusplus
extern "C" {
#endif

struct asn_TYPE_descriptor_s;	/* Forward declaration */

/*
 * The XER decoder of any ASN.1 type. May be invoked by the application.
 * Decodes CANONICAL-XER and BASIC-XER.
 */
asn_dec_rval_t xer_decode(
    const struct asn_codec_ctx_s *opt_codec_ctx,
    const struct asn_TYPE_descriptor_s *type_descriptor,
    void **struct_ptr,  /* Pointer to a target structure's pointer */
    const void *buffer, /* Data to be decoded */
    size_t size         /* Size of data buffer */
);

/*
 * Type of the type-specific XER decoder function.
 */
typedef asn_dec_rval_t(xer_type_decoder_f)(
    const asn_codec_ctx_t *opt_codec_ctx,
    const struct asn_TYPE_descriptor_s *type_descriptor, void **struct_ptr,
    const char *opt_mname, /* Member name */
    const void *buf_ptr, size_t size);

/*******************************
 * INTERNALLY USEFUL FUNCTIONS *
 *******************************/

/*
 * Generalized function for decoding the primitive values.
 * Used by more specialized functions, such as OCTET_STRING_decode_xer_utf8
 * and others. This function should not be used by applications, as its API
 * is subject to changes.
 */
asn_dec_rval_t xer_decode_general(
    const asn_codec_ctx_t *opt_codec_ctx,
    asn_struct_ctx_t *ctx, /* Type decoder context */
    void *struct_key,      /* Treated as opaque pointer */
    const char *xml_tag,   /* Expected XML tag name */
    const void *buf_ptr, size_t size,
    int (*opt_unexpected_tag_decoder)(void *struct_key, const void *chunk_buf,
                                      size_t chunk_size),
    ssize_t (*body_receiver)(void *struct_key, const void *chunk_buf,
                             size_t chunk_size, int have_more));


/*
 * Fetch the next XER (XML) token from the stream.
 * The function returns the number of bytes occupied by the chunk type,
 * returned in the _ch_type. The _ch_type is only set (and valid) when
 * the return value is >= 0.
 */
  typedef enum pxer_chunk_type {
	PXER_WMORE,     /* Chunk type is not clear, more data expected. */
	PXER_TAG,	    /* Complete XER tag */
	PXER_TEXT,	    /* Plain text between XER tags */
	PXER_COMMENT	/* A comment, may be part of */
  } pxer_chunk_type_e;
ssize_t xer_next_token(int *stateContext,
	const void *buffer, size_t size, pxer_chunk_type_e *_ch_type);

/*
 * This function checks the buffer against the tag name is expected to occur.
 */
  typedef enum xer_check_tag {
	XCT_BROKEN	= 0,	/* The tag is broken */
	XCT_OPENING	= 1,	/* This is the <opening> tag */
	XCT_CLOSING	= 2,	/* This is the </closing> tag */
	XCT_BOTH	= 3,	/* This is the <modified/> tag */
	XCT__UNK__MASK	= 4,	/* Mask of everything unexpected */
	XCT_UNKNOWN_OP	= 5,	/* Unexpected <opening> tag */
	XCT_UNKNOWN_CL	= 6,	/* Unexpected </closing> tag */
	XCT_UNKNOWN_BO	= 7	/* Unexpected <modified/> tag */
  } xer_check_tag_e;
xer_check_tag_e xer_check_tag(const void *buf_ptr, int size,
		const char *need_tag);

/*
 * Get the number of bytes consisting entirely of XER whitespace characters.
 * RETURN VALUES:
 * >=0:	Number of whitespace characters in the string.
 */
size_t xer_whitespace_span(const void *chunk_buf, size_t chunk_size);

/*
 * Skip the series of anticipated extensions.
 */
int xer_skip_unknown(xer_check_tag_e tcv, ber_tlv_len_t *depth);

#ifdef __cplusplus
}
#endif

#endif	/* _XER_DECODER_H_ */
