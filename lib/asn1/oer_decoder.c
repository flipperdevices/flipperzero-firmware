/*
 * Copyright (c) 2017 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
#include <asn_internal.h>
#include <asn_codecs_prim.h>

/*
 * The OER decoder of any type.
 */
asn_dec_rval_t
oer_decode(const asn_codec_ctx_t *opt_codec_ctx,
           const asn_TYPE_descriptor_t *type_descriptor, void **struct_ptr,
           const void *ptr, size_t size) {
    asn_codec_ctx_t s_codec_ctx;

	/*
	 * Stack checker requires that the codec context
	 * must be allocated on the stack.
	 */
	if(opt_codec_ctx) {
		if(opt_codec_ctx->max_stack_size) {
			s_codec_ctx = *opt_codec_ctx;
			opt_codec_ctx = &s_codec_ctx;
		}
	} else {
		/* If context is not given, be security-conscious anyway */
		memset(&s_codec_ctx, 0, sizeof(s_codec_ctx));
		s_codec_ctx.max_stack_size = ASN__DEFAULT_STACK_MAX;
		opt_codec_ctx = &s_codec_ctx;
	}

	/*
	 * Invoke type-specific decoder.
	 */
	return type_descriptor->op->oer_decoder(opt_codec_ctx, type_descriptor, 0,
		struct_ptr,	/* Pointer to the destination structure */
		ptr, size	/* Buffer and its size */
		);
}

/*
 * Open Type is encoded as a length (#8.6) followed by that number of bytes.
 * Since we're just skipping, reading the length would be enough.
 */
ssize_t
oer_open_type_skip(const void *bufptr, size_t size) {
    size_t len = 0;
    return oer_fetch_length(bufptr, size, &len);
}

/*
 * Read the Open Type (X.696 (08/2015), #30).
 * RETURN VALUES:
 *       0:     More data expected than bufptr contains.
 *      -1:     Fatal error deciphering length.
 *      >0:     Number of bytes used from bufptr.
 */
ssize_t
oer_open_type_get(const asn_codec_ctx_t *opt_codec_ctx,
                  const struct asn_TYPE_descriptor_s *td,
                  const asn_oer_constraints_t *constraints, void **struct_ptr,
                  const void *bufptr, size_t size) {
    asn_dec_rval_t dr;
    size_t container_len = 0;
    ssize_t len_len;
    enum asn_struct_free_method dispose_method =
        (*struct_ptr) ? ASFM_FREE_UNDERLYING_AND_RESET : ASFM_FREE_EVERYTHING;

    /* Get the size of a length determinant */
    len_len = oer_fetch_length(bufptr, size, &container_len);
    if(len_len <= 0) {
        return len_len; /* Error or more data expected */
    }

    /*
     * len_len can't be bigger than size, but size without len_len
     * should be bigger or equal to container length
     */
    if(size - len_len < container_len) {
        /* More data is expected */
        return 0;
    }

    dr = td->op->oer_decoder(opt_codec_ctx, td, constraints, struct_ptr,
                         (const uint8_t *)bufptr + len_len, container_len);
    if(dr.code == RC_OK) {
        return len_len + container_len;
    } else {
        /* Even if RC_WMORE, we can't get more data into a closed container. */
        td->op->free_struct(td, *struct_ptr, dispose_method);
        *struct_ptr = NULL;
        return -1;
    }
}


asn_dec_rval_t
oer_decode_primitive(const asn_codec_ctx_t *opt_codec_ctx,
                     const asn_TYPE_descriptor_t *td,
                     const asn_oer_constraints_t *constraints, void **sptr,
                     const void *ptr, size_t size) {
    ASN__PRIMITIVE_TYPE_t *st = (ASN__PRIMITIVE_TYPE_t *)*sptr;
    asn_dec_rval_t rval = {RC_OK, 0};
    size_t expected_length = 0;
    ssize_t len_len;

    (void)td;
    (void)opt_codec_ctx;
    (void)constraints;

    if(!st) {
        st = (ASN__PRIMITIVE_TYPE_t *)(*sptr = CALLOC(
                                           1, sizeof(ASN__PRIMITIVE_TYPE_t)));
        if(!st) ASN__DECODE_FAILED;
    }


    /*
     * X.696 (08/2015) #27.2
     * Encode length determinant as _number of octets_, but only
     * if upper bound is not equal to lower bound.
     */
    len_len = oer_fetch_length(ptr, size, &expected_length);
    if(len_len > 0) {
        rval.consumed = len_len;
        ptr = (const char *)ptr + len_len;
        size -= len_len;
    } else if(len_len == 0) {
        ASN__DECODE_STARVED;
    } else if(len_len < 0) {
        ASN__DECODE_FAILED;
    }

    if(size < expected_length) {
        ASN__DECODE_STARVED;
    } else {
        uint8_t *buf = MALLOC(expected_length + 1);
        if(buf == NULL) {
            ASN__DECODE_FAILED;
        } else {
            memcpy(buf, ptr, expected_length);
            buf[expected_length] = '\0';
        }
        FREEMEM(st->buf);
        st->buf = buf;
        st->size = expected_length;

        rval.consumed += expected_length;
        return rval;
    }
}
