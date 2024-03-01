#include <asn_application.h>
#include <asn_internal.h>
#include <per_decoder.h>

/*
 * Decode a "Production of a complete encoding", X.691#10.1.
 * The complete encoding contains at least one byte, and is an integral
 * multiple of 8 bytes.
 */
asn_dec_rval_t
uper_decode_complete(const asn_codec_ctx_t *opt_codec_ctx,
                     const asn_TYPE_descriptor_t *td, void **sptr,
                     const void *buffer, size_t size) {
    asn_dec_rval_t rval;

	rval = uper_decode(opt_codec_ctx, td, sptr, buffer, size, 0, 0);
	if(rval.consumed) {
		/*
		 * We've always given 8-aligned data,
		 * so convert bits to integral bytes.
		 */
		rval.consumed += 7;
		rval.consumed >>= 3;
	} else if(rval.code == RC_OK) {
		if(size) {
			if(((const uint8_t *)buffer)[0] == 0) {
				rval.consumed = 1;	/* 1 byte */
			} else {
				ASN_DEBUG("Expecting single zeroed byte");
				rval.code = RC_FAIL;
			}
		} else {
			/* Must contain at least 8 bits. */
			rval.code = RC_WMORE;
		}
	}

	return rval;
}

asn_dec_rval_t
uper_decode(const asn_codec_ctx_t *opt_codec_ctx,
            const asn_TYPE_descriptor_t *td, void **sptr, const void *buffer,
            size_t size, int skip_bits, int unused_bits) {
    asn_codec_ctx_t s_codec_ctx;
	asn_dec_rval_t rval;
	asn_per_data_t pd;

	if(skip_bits < 0 || skip_bits > 7
	|| unused_bits < 0 || unused_bits > 7
	|| (unused_bits > 0 && !size))
		ASN__DECODE_FAILED;

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

	/* Fill in the position indicator */
	memset(&pd, 0, sizeof(pd));
	pd.buffer = (const uint8_t *)buffer;
	pd.nboff = skip_bits;
	pd.nbits = 8 * size - unused_bits; /* 8 is CHAR_BIT from <limits.h> */
	if(pd.nboff > pd.nbits)
		ASN__DECODE_FAILED;

	/*
	 * Invoke type-specific decoder.
	 */
	if(!td->op->uper_decoder)
		ASN__DECODE_FAILED;	/* PER is not compiled in */
	rval = td->op->uper_decoder(opt_codec_ctx, td, 0, sptr, &pd);
	if(rval.code == RC_OK) {
		/* Return the number of consumed bits */
		rval.consumed = ((pd.buffer - (const uint8_t *)buffer) << 3)
					+ pd.nboff - skip_bits;
		ASN_DEBUG("PER decoding consumed %ld, counted %ld",
			(long)rval.consumed, (long)pd.moved);
		assert(rval.consumed == pd.moved);
	} else {
		/* PER codec is not a restartable */
		rval.consumed = 0;
	}
	return rval;
}

