/*
 * Copyright (c) 2003-2017 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
#ifndef	ASN_CODECS_H
#define	ASN_CODECS_H

#ifdef __cplusplus
extern "C" {
#endif

struct asn_TYPE_descriptor_s;	/* Forward declaration */

/*
 * This structure defines a set of parameters that may be passed
 * to every ASN.1 encoder or decoder function.
 * WARNING: if max_stack_size member is set, and you are calling the
 *   function pointers of the asn_TYPE_descriptor_t directly,
 *   this structure must be ALLOCATED ON THE STACK!
 *   If you can't always satisfy this requirement, use ber_decode(),
 *   xer_decode() and uper_decode() functions instead.
 */
typedef struct asn_codec_ctx_s {
	/*
	 * Limit the decoder routines to use no (much) more stack than a given
	 * number of bytes. Most of decoders are stack-based, and this
	 * would protect against stack overflows if the number of nested
	 * encodings is high.
	 * The OCTET STRING, BIT STRING and ANY BER decoders are heap-based,
	 * and are safe from this kind of overflow.
	 * A value from getrlimit(RLIMIT_STACK) may be used to initialize
	 * this variable. Be careful in multithreaded environments, as the
	 * stack size is rather limited.
	 */
	size_t  max_stack_size; /* 0 disables stack bounds checking */
} asn_codec_ctx_t;

/*
 * Type of the return value of the encoding functions (der_encode, xer_encode).
 */
typedef struct asn_enc_rval_s {
	/*
	 * Number of bytes encoded.
	 * -1 indicates failure to encode the structure.
	 * In this case, the members below this one are meaningful.
	 */
	ssize_t encoded;

	/*
	 * Members meaningful when (encoded == -1), for post mortem analysis.
	 */

	/* Type which cannot be encoded */
	const struct asn_TYPE_descriptor_s *failed_type;

	/* Pointer to the structure of that type */
	const void *structure_ptr;
} asn_enc_rval_t;
#define	ASN__ENCODE_FAILED do {					\
	asn_enc_rval_t tmp_error;				\
	tmp_error.encoded = -1;					\
	tmp_error.failed_type = td;				\
	tmp_error.structure_ptr = sptr;				\
	ASN_DEBUG("Failed to encode element %s", td ? td->name : "");	\
	return tmp_error;					\
} while(0)
#define	ASN__ENCODED_OK(rval) do {				\
	rval.structure_ptr = 0;					\
	rval.failed_type = 0;					\
	return rval;						\
} while(0)

/*
 * Type of the return value of the decoding functions (ber_decode, xer_decode)
 * 
 * Please note that the number of consumed bytes is ALWAYS meaningful,
 * even if code==RC_FAIL. This is to indicate the number of successfully
 * decoded bytes, hence providing a possibility to fail with more diagnostics
 * (i.e., print the offending remainder of the buffer).
 */
enum asn_dec_rval_code_e {
	RC_OK,		/* Decoded successfully */
	RC_WMORE,	/* More data expected, call again */
	RC_FAIL		/* Failure to decode data */
};
typedef struct asn_dec_rval_s {
	enum asn_dec_rval_code_e code;	/* Result code */
	size_t consumed;		/* Number of bytes consumed */
} asn_dec_rval_t;
#define	ASN__DECODE_FAILED do {					\
	asn_dec_rval_t tmp_error;				\
	tmp_error.code = RC_FAIL;				\
	tmp_error.consumed = 0;					\
	ASN_DEBUG("Failed to decode element %s", td ? td->name : "");	\
	return tmp_error;					\
} while(0)
#define	ASN__DECODE_STARVED do {				\
	asn_dec_rval_t tmp_error;				\
	tmp_error.code = RC_WMORE;				\
	tmp_error.consumed = 0;					\
	return tmp_error;					\
} while(0)

#ifdef __cplusplus
}
#endif

#endif	/* ASN_CODECS_H */
