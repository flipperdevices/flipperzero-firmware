/*
 * Copyright (c) 2003-2017 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
#include <asn_internal.h>
#include <constr_CHOICE.h>
#include <per_opentype.h>

/*
 * Number of bytes left for this structure.
 * (ctx->left) indicates the number of bytes _transferred_ for the structure.
 * (size) contains the number of bytes in the buffer passed.
 */
#define	LEFT	((size<(size_t)ctx->left)?size:(size_t)ctx->left)

/*
 * If the subprocessor function returns with an indication that it wants
 * more data, it may well be a fatal decoding problem, because the
 * size is constrained by the <TLV>'s L, even if the buffer size allows
 * reading more data.
 * For example, consider the buffer containing the following TLVs:
 * <T:5><L:1><V> <T:6>...
 * The TLV length clearly indicates that one byte is expected in V, but
 * if the V processor returns with "want more data" even if the buffer
 * contains way more data than the V processor have seen.
 */
#define	SIZE_VIOLATION	(ctx->left >= 0 && (size_t)ctx->left <= size)

/*
 * This macro "eats" the part of the buffer which is definitely "consumed",
 * i.e. was correctly converted into local representation or rightfully skipped.
 */
#undef	ADVANCE
#define	ADVANCE(num_bytes)	do {		\
		size_t num = num_bytes;		\
		ptr = ((const char *)ptr) + num;\
		size -= num;			\
		if(ctx->left >= 0)		\
			ctx->left -= num;	\
		consumed_myself += num;		\
	} while(0)

/*
 * Switch to the next phase of parsing.
 */
#undef	NEXT_PHASE
#define	NEXT_PHASE(ctx)	do {			\
		ctx->phase++;			\
		ctx->step = 0;			\
	} while(0)

/*
 * Return a standardized complex structure.
 */
#undef	RETURN
#define	RETURN(_code)	do {			\
		rval.code = _code;		\
		rval.consumed = consumed_myself;\
		return rval;			\
	} while(0)

/*
 * See the definitions.
 */
static unsigned _fetch_present_idx(const void *struct_ptr, unsigned off,
                                   unsigned size);
static void _set_present_idx(void *sptr, unsigned offset, unsigned size,
                             unsigned pres);
static const void *_get_member_ptr(const asn_TYPE_descriptor_t *,
                                   const void *sptr, asn_TYPE_member_t **elm,
                                   unsigned *present);

/*
 * Tags are canonically sorted in the tag to member table.
 */
static int
_search4tag(const void *ap, const void *bp) {
	const asn_TYPE_tag2member_t *a = (const asn_TYPE_tag2member_t *)ap;
	const asn_TYPE_tag2member_t *b = (const asn_TYPE_tag2member_t *)bp;

	int a_class = BER_TAG_CLASS(a->el_tag);
	int b_class = BER_TAG_CLASS(b->el_tag);

	if(a_class == b_class) {
		ber_tlv_tag_t a_value = BER_TAG_VALUE(a->el_tag);
		ber_tlv_tag_t b_value = BER_TAG_VALUE(b->el_tag);

		if(a_value == b_value)
			return 0;
		else if(a_value < b_value)
			return -1;
		else
			return 1;
	} else if(a_class < b_class) {
		return -1;
	} else {
		return 1;
	}
}

/*
 * The decoder of the CHOICE type.
 */
asn_dec_rval_t
CHOICE_decode_ber(const asn_codec_ctx_t *opt_codec_ctx,
                  const asn_TYPE_descriptor_t *td, void **struct_ptr,
                  const void *ptr, size_t size, int tag_mode) {
    /*
	 * Bring closer parts of structure description.
	 */
	const asn_CHOICE_specifics_t *specs =
		(const asn_CHOICE_specifics_t *)td->specifics;
	asn_TYPE_member_t *elements = td->elements;

	/*
	 * Parts of the structure being constructed.
	 */
	void *st = *struct_ptr;	/* Target structure. */
	asn_struct_ctx_t *ctx;	/* Decoder context */

	ber_tlv_tag_t tlv_tag;	/* T from TLV */
	ssize_t tag_len;	/* Length of TLV's T */
	asn_dec_rval_t rval;	/* Return code from subparsers */

	ssize_t consumed_myself = 0;	/* Consumed bytes from ptr */

	ASN_DEBUG("Decoding %s as CHOICE", td->name);

	/*
	 * Create the target structure if it is not present already.
	 */
	if(st == 0) {
		st = *struct_ptr = CALLOC(1, specs->struct_size);
		if(st == 0) {
			RETURN(RC_FAIL);
		}
	}

	/*
	 * Restore parsing context.
	 */
	ctx = (asn_struct_ctx_t *)((char *)st + specs->ctx_offset);
	
	/*
	 * Start to parse where left previously
	 */
	switch(ctx->phase) {
	case 0:
		/*
		 * PHASE 0.
		 * Check that the set of tags associated with given structure
		 * perfectly fits our expectations.
		 */

		if(tag_mode || td->tags_count) {
			rval = ber_check_tags(opt_codec_ctx, td, ctx, ptr, size,
				tag_mode, -1, &ctx->left, 0);
			if(rval.code != RC_OK) {
				ASN_DEBUG("%s tagging check failed: %d",
					td->name, rval.code);
				return rval;
			}

			if(ctx->left >= 0) {
				/* ?Substracted below! */
				ctx->left += rval.consumed;
			}
			ADVANCE(rval.consumed);
		} else {
			ctx->left = -1;
		}

		NEXT_PHASE(ctx);

		ASN_DEBUG("Structure consumes %ld bytes, buffer %ld",
			(long)ctx->left, (long)size);

		/* Fall through */
	case 1:
		/*
		 * Fetch the T from TLV.
		 */
		tag_len = ber_fetch_tag(ptr, LEFT, &tlv_tag);
		ASN_DEBUG("In %s CHOICE tag length %d", td->name, (int)tag_len);
		switch(tag_len) {
		case 0: if(!SIZE_VIOLATION) RETURN(RC_WMORE);
			/* Fall through */
		case -1: RETURN(RC_FAIL);
		}

		do {
			const asn_TYPE_tag2member_t *t2m;
			asn_TYPE_tag2member_t key;

			key.el_tag = tlv_tag;
			t2m = (const asn_TYPE_tag2member_t *)bsearch(&key,
					specs->tag2el, specs->tag2el_count,
					sizeof(specs->tag2el[0]), _search4tag);
			if(t2m) {
				/*
				 * Found the element corresponding to the tag.
				 */
				NEXT_PHASE(ctx);
				ctx->step = t2m->el_no;
				break;
			} else if(specs->ext_start == -1) {
				ASN_DEBUG("Unexpected tag %s "
					"in non-extensible CHOICE %s",
					ber_tlv_tag_string(tlv_tag), td->name);
				RETURN(RC_FAIL);
			} else {
				/* Skip this tag */
				ssize_t skip;

				ASN_DEBUG("Skipping unknown tag %s",
					ber_tlv_tag_string(tlv_tag));

				skip = ber_skip_length(opt_codec_ctx,
					BER_TLV_CONSTRUCTED(ptr),
					(const char *)ptr + tag_len,
					LEFT - tag_len);

				switch(skip) {
				case 0: if(!SIZE_VIOLATION) RETURN(RC_WMORE);
					/* Fall through */
				case -1: RETURN(RC_FAIL);
				}

				ADVANCE(skip + tag_len);
				RETURN(RC_OK);
			}
		} while(0);

	case 2:
		/*
		 * PHASE 2.
		 * Read in the element.
		 */
	    do {
		asn_TYPE_member_t *elm;/* CHOICE's element */
		void *memb_ptr;		/* Pointer to the member */
		void **memb_ptr2;	/* Pointer to that pointer */

		elm = &elements[ctx->step];

		/*
		 * Compute the position of the member inside a structure,
		 * and also a type of containment (it may be contained
		 * as pointer or using inline inclusion).
		 */
		if(elm->flags & ATF_POINTER) {
			/* Member is a pointer to another structure */
			memb_ptr2 = (void **)((char *)st + elm->memb_offset);
		} else {
			/*
			 * A pointer to a pointer
			 * holding the start of the structure
			 */
			memb_ptr = (char *)st + elm->memb_offset;
			memb_ptr2 = &memb_ptr;
		}
		/* Set presence to be able to free it properly at any time */
		_set_present_idx(st, specs->pres_offset,
				specs->pres_size, ctx->step + 1);
		/*
		 * Invoke the member fetch routine according to member's type
		 */
		rval = elm->type->op->ber_decoder(opt_codec_ctx, elm->type,
				memb_ptr2, ptr, LEFT, elm->tag_mode);
		switch(rval.code) {
		case RC_OK:
			break;
		case RC_WMORE: /* More data expected */
			if(!SIZE_VIOLATION) {
				ADVANCE(rval.consumed);
				RETURN(RC_WMORE);
			}
			RETURN(RC_FAIL);
		case RC_FAIL: /* Fatal error */
			RETURN(rval.code);
		} /* switch(rval) */
		
		ADVANCE(rval.consumed);
	  } while(0);

		NEXT_PHASE(ctx);

		/* Fall through */
	case 3:
		ASN_DEBUG("CHOICE %s Leftover: %ld, size = %ld, tm=%d, tc=%d",
			td->name, (long)ctx->left, (long)size,
			tag_mode, td->tags_count);

		if(ctx->left > 0) {
			/*
			 * The type must be fully decoded
			 * by the CHOICE member-specific decoder.
			 */
			RETURN(RC_FAIL);
		}

		if(ctx->left == -1
		&& !(tag_mode || td->tags_count)) {
			/*
			 * This is an untagged CHOICE.
			 * It doesn't contain nothing
			 * except for the member itself, including all its tags.
			 * The decoding is completed.
			 */
			NEXT_PHASE(ctx);
			break;
		}

		/*
		 * Read in the "end of data chunks"'s.
		 */
		while(ctx->left < 0) {
			ssize_t tl;

			tl = ber_fetch_tag(ptr, LEFT, &tlv_tag);
			switch(tl) {
			case 0: if(!SIZE_VIOLATION) RETURN(RC_WMORE);
				/* Fall through */
			case -1: RETURN(RC_FAIL);
			}

			/*
			 * Expected <0><0>...
			 */
			if(((const uint8_t *)ptr)[0] == 0) {
				if(LEFT < 2) {
					if(SIZE_VIOLATION)
						RETURN(RC_FAIL);
					else
						RETURN(RC_WMORE);
				} else if(((const uint8_t *)ptr)[1] == 0) {
					/*
					 * Correctly finished with <0><0>.
					 */
					ADVANCE(2);
					ctx->left++;
					continue;
				}
			} else {
				ASN_DEBUG("Unexpected continuation in %s",
					td->name);
				RETURN(RC_FAIL);
			}

			/* UNREACHABLE */
		}

		NEXT_PHASE(ctx);
	case 4:
		/* No meaningful work here */
		break;
	}
	
	RETURN(RC_OK);
}

asn_enc_rval_t
CHOICE_encode_der(const asn_TYPE_descriptor_t *td, const void *sptr,
                  int tag_mode, ber_tlv_tag_t tag, asn_app_consume_bytes_f *cb,
                  void *app_key) {
    const asn_CHOICE_specifics_t *specs = (const asn_CHOICE_specifics_t *)td->specifics;
	asn_TYPE_member_t *elm;	/* CHOICE element */
	asn_enc_rval_t erval;
	const void *memb_ptr;
	size_t computed_size = 0;
	unsigned present;

	if(!sptr) ASN__ENCODE_FAILED;

	ASN_DEBUG("%s %s as CHOICE",
		cb?"Encoding":"Estimating", td->name);

	present = _fetch_present_idx(sptr,
		specs->pres_offset, specs->pres_size);

	/*
	 * If the structure was not initialized, it cannot be encoded:
	 * can't deduce what to encode in the choice type.
	 */
	if(present == 0 || present > td->elements_count) {
		if(present == 0 && td->elements_count == 0) {
			/* The CHOICE is empty?! */
			erval.encoded = 0;
			ASN__ENCODED_OK(erval);
		}
		ASN__ENCODE_FAILED;
	}

	/*
	 * Seek over the present member of the structure.
	 */
	elm = &td->elements[present-1];
	if(elm->flags & ATF_POINTER) {
        memb_ptr =
            *(const void *const *)((const char *)sptr + elm->memb_offset);
        if(memb_ptr == 0) {
			if(elm->optional) {
				erval.encoded = 0;
				ASN__ENCODED_OK(erval);
			}
			/* Mandatory element absent */
			ASN__ENCODE_FAILED;
		}
	} else {
        memb_ptr = (const void *)((const char *)sptr + elm->memb_offset);
    }

	/*
	 * If the CHOICE itself is tagged EXPLICIT:
	 * T ::= [2] EXPLICIT CHOICE { ... }
	 * Then emit the appropriate tags.
	 */
	if(tag_mode == 1 || td->tags_count) {
		/*
		 * For this, we need to pre-compute the member.
		 */
		ssize_t ret;

		/* Encode member with its tag */
		erval = elm->type->op->der_encoder(elm->type, memb_ptr,
			elm->tag_mode, elm->tag, 0, 0);
		if(erval.encoded == -1)
			return erval;

		/* Encode CHOICE with parent or my own tag */
		ret = der_write_tags(td, erval.encoded, tag_mode, 1, tag,
			cb, app_key);
		if(ret == -1)
			ASN__ENCODE_FAILED;
		computed_size += ret;
	}

	/*
	 * Encode the single underlying member.
	 */
	erval = elm->type->op->der_encoder(elm->type, memb_ptr,
		elm->tag_mode, elm->tag, cb, app_key);
	if(erval.encoded == -1)
		return erval;

	ASN_DEBUG("Encoded CHOICE member in %ld bytes (+%ld)",
		(long)erval.encoded, (long)computed_size);

	erval.encoded += computed_size;

	return erval;
}

ber_tlv_tag_t
CHOICE_outmost_tag(const asn_TYPE_descriptor_t *td, const void *ptr, int tag_mode, ber_tlv_tag_t tag) {
	const asn_CHOICE_specifics_t *specs = (const asn_CHOICE_specifics_t *)td->specifics;
	unsigned present;

	assert(tag_mode == 0); (void)tag_mode;
	assert(tag == 0); (void)tag;

	/*
	 * Figure out which CHOICE element is encoded.
	 */
	present = _fetch_present_idx(ptr, specs->pres_offset, specs->pres_size);

	if(present > 0 && present <= td->elements_count) {
		const asn_TYPE_member_t *elm = &td->elements[present-1];
		const void *memb_ptr;

		if(elm->flags & ATF_POINTER) {
			memb_ptr = *(const void * const *)
					((const char *)ptr + elm->memb_offset);
		} else {
			memb_ptr = (const void *)
					((const char *)ptr + elm->memb_offset);
		}

		return asn_TYPE_outmost_tag(elm->type, memb_ptr,
			elm->tag_mode, elm->tag);
	} else {
		return (ber_tlv_tag_t)-1;
	}
}

int
CHOICE_constraint(const asn_TYPE_descriptor_t *td, const void *sptr,
                  asn_app_constraint_failed_f *ctfailcb, void *app_key) {
    const asn_CHOICE_specifics_t *specs =
        (const asn_CHOICE_specifics_t *)td->specifics;
    unsigned present;

	if(!sptr) {
		ASN__CTFAIL(app_key, td, sptr,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}

	/*
	 * Figure out which CHOICE element is encoded.
	 */
	present = _fetch_present_idx(sptr, specs->pres_offset,specs->pres_size);
	if(present > 0 && present <= td->elements_count) {
		asn_TYPE_member_t *elm = &td->elements[present-1];
		const void *memb_ptr;

		if(elm->flags & ATF_POINTER) {
			memb_ptr = *(const void * const *)((const char *)sptr + elm->memb_offset);
			if(!memb_ptr) {
				if(elm->optional)
					return 0;
				ASN__CTFAIL(app_key, td, sptr,
					"%s: mandatory CHOICE element %s absent (%s:%d)",
					td->name, elm->name, __FILE__, __LINE__);
				return -1;
			}
		} else {
			memb_ptr = (const void *)((const char *)sptr + elm->memb_offset);
		}

		if(elm->encoding_constraints.general_constraints) {
			return elm->encoding_constraints.general_constraints(elm->type, memb_ptr,
				ctfailcb, app_key);
		} else {
			return elm->type->encoding_constraints.general_constraints(elm->type,
					memb_ptr, ctfailcb, app_key);
		}
	} else {
		ASN__CTFAIL(app_key, td, sptr,
			"%s: no CHOICE element given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
}

#undef	XER_ADVANCE
#define	XER_ADVANCE(num_bytes)	do {			\
		size_t num = num_bytes;			\
		buf_ptr = (const void *)(((const char *)buf_ptr) + num); \
		size -= num;				\
		consumed_myself += num;			\
	} while(0)

/*
 * Decode the XER (XML) data.
 */
asn_dec_rval_t
CHOICE_decode_xer(const asn_codec_ctx_t *opt_codec_ctx,
                  const asn_TYPE_descriptor_t *td, void **struct_ptr,
                  const char *opt_mname, const void *buf_ptr, size_t size) {
    /*
	 * Bring closer parts of structure description.
	 */
	const asn_CHOICE_specifics_t *specs = (const asn_CHOICE_specifics_t *)td->specifics;
	const char *xml_tag = opt_mname ? opt_mname : td->xml_tag;

	/*
	 * Parts of the structure being constructed.
	 */
	void *st = *struct_ptr;	/* Target structure. */
	asn_struct_ctx_t *ctx;	/* Decoder context */

	asn_dec_rval_t rval;		/* Return value of a decoder */
	ssize_t consumed_myself = 0;	/* Consumed bytes from ptr */
	size_t edx;			/* Element index */

	/*
	 * Create the target structure if it is not present already.
	 */
	if(st == 0) {
		st = *struct_ptr = CALLOC(1, specs->struct_size);
		if(st == 0) RETURN(RC_FAIL);
	}

	/*
	 * Restore parsing context.
	 */
	ctx = (asn_struct_ctx_t *)((char *)st + specs->ctx_offset);
	if(ctx->phase == 0 && !*xml_tag)
		ctx->phase = 1;	/* Skip the outer tag checking phase */

	/*
	 * Phases of XER/XML processing:
	 * Phase 0: Check that the opening tag matches our expectations.
	 * Phase 1: Processing body and reacting on closing tag.
	 * Phase 2: Processing inner type.
	 * Phase 3: Only waiting for closing tag.
	 * Phase 4: Skipping unknown extensions.
	 * Phase 5: PHASED OUT
	 */
	for(edx = ctx->step; ctx->phase <= 4;) {
		pxer_chunk_type_e ch_type;	/* XER chunk type */
		ssize_t ch_size;		/* Chunk size */
		xer_check_tag_e tcv;		/* Tag check value */
		asn_TYPE_member_t *elm;

		/*
		 * Go inside the member.
		 */
		if(ctx->phase == 2) {
			asn_dec_rval_t tmprval;
			void *memb_ptr;		/* Pointer to the member */
			void **memb_ptr2;	/* Pointer to that pointer */
			unsigned old_present;

			elm = &td->elements[edx];

			if(elm->flags & ATF_POINTER) {
				/* Member is a pointer to another structure */
				memb_ptr2 = (void **)((char *)st
					+ elm->memb_offset);
			} else {
				memb_ptr = (char *)st + elm->memb_offset;
				memb_ptr2 = &memb_ptr;
			}

			/* Start/Continue decoding the inner member */
			tmprval = elm->type->op->xer_decoder(opt_codec_ctx,
					elm->type, memb_ptr2, elm->name,
					buf_ptr, size);
			XER_ADVANCE(tmprval.consumed);
			ASN_DEBUG("XER/CHOICE: itdf: [%s] code=%d",
				elm->type->name, tmprval.code);
			old_present = _fetch_present_idx(st,
				specs->pres_offset, specs->pres_size);
			assert(old_present == 0 || old_present == edx + 1);
			/* Record what we've got */
			_set_present_idx(st,
				specs->pres_offset, specs->pres_size, edx + 1);
			if(tmprval.code != RC_OK)
				RETURN(tmprval.code);
			ctx->phase = 3;
			/* Fall through */
		}

		/* No need to wait for closing tag; special mode. */
		if(ctx->phase == 3 && !*xml_tag) {
			ctx->phase = 5;	/* Phase out */
			RETURN(RC_OK);
		}

		/*
		 * Get the next part of the XML stream.
		 */
		ch_size = xer_next_token(&ctx->context, buf_ptr, size, &ch_type);
		if(ch_size == -1) {
            RETURN(RC_FAIL);
        } else {
			switch(ch_type) {
			case PXER_WMORE:
                RETURN(RC_WMORE);
			case PXER_COMMENT:	/* Got XML comment */
			case PXER_TEXT:		/* Ignore free-standing text */
				XER_ADVANCE(ch_size);	/* Skip silently */
				continue;
			case PXER_TAG:
				break;	/* Check the rest down there */
			}
		}

		tcv = xer_check_tag(buf_ptr, ch_size, xml_tag);
		ASN_DEBUG("XER/CHOICE checked [%c%c%c%c] vs [%s], tcv=%d",
			ch_size>0?((const uint8_t *)buf_ptr)[0]:'?',
			ch_size>1?((const uint8_t *)buf_ptr)[1]:'?',
			ch_size>2?((const uint8_t *)buf_ptr)[2]:'?',
			ch_size>3?((const uint8_t *)buf_ptr)[3]:'?',
		xml_tag, tcv);

		/* Skip the extensions section */
		if(ctx->phase == 4) {
			ASN_DEBUG("skip_unknown(%d, %ld)",
				tcv, (long)ctx->left);
			switch(xer_skip_unknown(tcv, &ctx->left)) {
			case -1:
				ctx->phase = 5;
				RETURN(RC_FAIL);
			case 1:
				ctx->phase = 3;
				/* Fall through */
			case 0:
				XER_ADVANCE(ch_size);
				continue;
			case 2:
				ctx->phase = 3;
				break;
			}
		}

		switch(tcv) {
		case XCT_BOTH:
			break;	/* No CHOICE? */
		case XCT_CLOSING:
			if(ctx->phase != 3)
				break;
			XER_ADVANCE(ch_size);
			ctx->phase = 5;	/* Phase out */
			RETURN(RC_OK);
		case XCT_OPENING:
			if(ctx->phase == 0) {
				XER_ADVANCE(ch_size);
				ctx->phase = 1;	/* Processing body phase */
				continue;
			}
			/* Fall through */
		case XCT_UNKNOWN_OP:
		case XCT_UNKNOWN_BO:

			if(ctx->phase != 1)
				break;	/* Really unexpected */

			/*
			 * Search which inner member corresponds to this tag.
			 */
			for(edx = 0; edx < td->elements_count; edx++) {
				elm = &td->elements[edx];
				tcv = xer_check_tag(buf_ptr,ch_size,elm->name);
				switch(tcv) {
				case XCT_BOTH:
				case XCT_OPENING:
					/*
					 * Process this member.
					 */
					ctx->step = edx;
					ctx->phase = 2;
					break;
				case XCT_UNKNOWN_OP:
				case XCT_UNKNOWN_BO:
					continue;
				default:
					edx = td->elements_count;
					break;	/* Phase out */
				}
				break;
			}
			if(edx != td->elements_count)
				continue;

			/* It is expected extension */
			if(specs->ext_start != -1) {
				ASN_DEBUG("Got anticipated extension");
				/*
				 * Check for (XCT_BOTH or XCT_UNKNOWN_BO)
				 * By using a mask. Only record a pure
				 * <opening> tags.
				 */
				if(tcv & XCT_CLOSING) {
					/* Found </extension> without body */
					ctx->phase = 3; /* Terminating */
				} else {
					ctx->left = 1;
					ctx->phase = 4; /* Skip ...'s */
				}
				XER_ADVANCE(ch_size);
				continue;
			}

			/* Fall through */
		default:
			break;
		}

		ASN_DEBUG("Unexpected XML tag [%c%c%c%c] in CHOICE [%s]"
			" (ph=%d, tag=%s)",
			ch_size>0?((const uint8_t *)buf_ptr)[0]:'?',
			ch_size>1?((const uint8_t *)buf_ptr)[1]:'?',
			ch_size>2?((const uint8_t *)buf_ptr)[2]:'?',
			ch_size>3?((const uint8_t *)buf_ptr)[3]:'?',
			td->name, ctx->phase, xml_tag);
		break;
	}

	ctx->phase = 5;	/* Phase out, just in case */
	RETURN(RC_FAIL);
}


asn_enc_rval_t
CHOICE_encode_xer(const asn_TYPE_descriptor_t *td, const void *sptr, int ilevel,
                  enum xer_encoder_flags_e flags, asn_app_consume_bytes_f *cb,
                  void *app_key) {
    const asn_CHOICE_specifics_t *specs =
        (const asn_CHOICE_specifics_t *)td->specifics;
    asn_enc_rval_t er;
	unsigned present;

	if(!sptr)
		ASN__ENCODE_FAILED;

	/*
	 * Figure out which CHOICE element is encoded.
	 */
	present = _fetch_present_idx(sptr, specs->pres_offset,specs->pres_size);

	if(present == 0 || present > td->elements_count) {
		ASN__ENCODE_FAILED;
	}  else {
		asn_enc_rval_t tmper;
		asn_TYPE_member_t *elm = &td->elements[present-1];
		const void *memb_ptr;
		const char *mname = elm->name;
		unsigned int mlen = strlen(mname);

		if(elm->flags & ATF_POINTER) {
            memb_ptr =
                *(const void *const *)((const char *)sptr + elm->memb_offset);
            if(!memb_ptr) ASN__ENCODE_FAILED;
		} else {
            memb_ptr = (const void *)((const char *)sptr + elm->memb_offset);
        }

        er.encoded = 0;

		if(!(flags & XER_F_CANONICAL)) ASN__TEXT_INDENT(1, ilevel);
		ASN__CALLBACK3("<", 1, mname, mlen, ">", 1);

		tmper = elm->type->op->xer_encoder(elm->type, memb_ptr,
				ilevel + 1, flags, cb, app_key);
		if(tmper.encoded == -1) return tmper;
		er.encoded += tmper.encoded;

		ASN__CALLBACK3("</", 2, mname, mlen, ">", 1);
	}

	if(!(flags & XER_F_CANONICAL)) ASN__TEXT_INDENT(1, ilevel - 1);

	ASN__ENCODED_OK(er);
cb_failed:
	ASN__ENCODE_FAILED;
}

asn_dec_rval_t
CHOICE_decode_uper(const asn_codec_ctx_t *opt_codec_ctx,
                   const asn_TYPE_descriptor_t *td,
                   const asn_per_constraints_t *constraints, void **sptr,
                   asn_per_data_t *pd) {
    const asn_CHOICE_specifics_t *specs =
        (const asn_CHOICE_specifics_t *)td->specifics;
    asn_dec_rval_t rv;
	const asn_per_constraint_t *ct;
	asn_TYPE_member_t *elm;	/* CHOICE's element */
	void *memb_ptr;
	void **memb_ptr2;
	void *st = *sptr;
	int value;

	if(ASN__STACK_OVERFLOW_CHECK(opt_codec_ctx))
		ASN__DECODE_FAILED;

	/*
	 * Create the target structure if it is not present already.
	 */
	if(!st) {
		st = *sptr = CALLOC(1, specs->struct_size);
		if(!st) ASN__DECODE_FAILED;
	}

	if(constraints) ct = &constraints->value;
	else if(td->encoding_constraints.per_constraints) ct = &td->encoding_constraints.per_constraints->value;
	else ct = 0;

	if(ct && ct->flags & APC_EXTENSIBLE) {
		value = per_get_few_bits(pd, 1);
		if(value < 0) ASN__DECODE_STARVED;
		if(value) ct = 0;	/* Not restricted */
	}

	if(ct && ct->range_bits >= 0) {
		value = per_get_few_bits(pd, ct->range_bits);
		if(value < 0) ASN__DECODE_STARVED;
		ASN_DEBUG("CHOICE %s got index %d in range %d",
			td->name, value, ct->range_bits);
		if(value > ct->upper_bound)
			ASN__DECODE_FAILED;
	} else {
		if(specs->ext_start == -1)
			ASN__DECODE_FAILED;
		value = uper_get_nsnnwn(pd);
		if(value < 0) ASN__DECODE_STARVED;
		value += specs->ext_start;
		if((unsigned)value >= td->elements_count)
			ASN__DECODE_FAILED;
	}

	/* Adjust if canonical order is different from natural order */
	if(specs->from_canonical_order) {
        ASN_DEBUG("CHOICE presence from wire %d", value);
		value = specs->from_canonical_order[value];
        ASN_DEBUG("CHOICE presence index effective %d", value);
    }

	/* Set presence to be able to free it later */
	_set_present_idx(st, specs->pres_offset, specs->pres_size, value + 1);

	elm = &td->elements[value];
	if(elm->flags & ATF_POINTER) {
		/* Member is a pointer to another structure */
		memb_ptr2 = (void **)((char *)st + elm->memb_offset);
	} else {
		memb_ptr = (char *)st + elm->memb_offset;
		memb_ptr2 = &memb_ptr;
	}
	ASN_DEBUG("Discovered CHOICE %s encodes %s", td->name, elm->name);

	if(ct && ct->range_bits >= 0) {
		rv = elm->type->op->uper_decoder(opt_codec_ctx, elm->type,
			elm->encoding_constraints.per_constraints, memb_ptr2, pd);
	} else {
		rv = uper_open_type_get(opt_codec_ctx, elm->type,
			elm->encoding_constraints.per_constraints, memb_ptr2, pd);
	}

	if(rv.code != RC_OK)
		ASN_DEBUG("Failed to decode %s in %s (CHOICE) %d",
			elm->name, td->name, rv.code);
	return rv;
}

asn_enc_rval_t
CHOICE_encode_uper(const asn_TYPE_descriptor_t *td,
                   const asn_per_constraints_t *constraints, const void *sptr,
                   asn_per_outp_t *po) {
    const asn_CHOICE_specifics_t *specs = (const asn_CHOICE_specifics_t *)td->specifics;
	asn_TYPE_member_t *elm;	/* CHOICE's element */
	const asn_per_constraint_t *ct;
	const void *memb_ptr;
	unsigned present;
	int present_enc;

	if(!sptr) ASN__ENCODE_FAILED;

	ASN_DEBUG("Encoding %s as CHOICE", td->name);

	if(constraints) ct = &constraints->value;
	else if(td->encoding_constraints.per_constraints)
		ct = &td->encoding_constraints.per_constraints->value;
	else ct = 0;

	present = _fetch_present_idx(sptr, specs->pres_offset, specs->pres_size);

	/*
	 * If the structure was not initialized properly, it cannot be encoded:
	 * can't deduce what to encode in the choice type.
	 */
	if(present == 0 || present > td->elements_count)
		ASN__ENCODE_FAILED;
	else
		present--;

	ASN_DEBUG("Encoding %s CHOICE element %d", td->name, present);

    /* Adjust if canonical order is different from natural order */
    if(specs->to_canonical_order)
        present_enc = specs->to_canonical_order[present];
    else
        present_enc = present;

    if(ct && ct->range_bits >= 0) {
		if(present_enc < ct->lower_bound
		|| present_enc > ct->upper_bound) {
			if(ct->flags & APC_EXTENSIBLE) {
                ASN_DEBUG(
                    "CHOICE member %d (enc %d) is an extension (%ld..%ld)",
                    present, present_enc, ct->lower_bound, ct->upper_bound);
                if(per_put_few_bits(po, 1, 1))
					ASN__ENCODE_FAILED;
			} else {
				ASN__ENCODE_FAILED;
			}
			ct = 0;
		}
	}
	if(ct && ct->flags & APC_EXTENSIBLE) {
        ASN_DEBUG("CHOICE member %d (enc %d) is not an extension (%ld..%ld)",
                  present, present_enc, ct->lower_bound, ct->upper_bound);
        if(per_put_few_bits(po, 0, 1))
			ASN__ENCODE_FAILED;
    }


	elm = &td->elements[present];
    ASN_DEBUG("CHOICE member \"%s\" %d (as %d)", elm->name, present,
              present_enc);
    if(elm->flags & ATF_POINTER) {
		/* Member is a pointer to another structure */
        memb_ptr =
            *(const void *const *)((const char *)sptr + elm->memb_offset);
        if(!memb_ptr) ASN__ENCODE_FAILED;
	} else {
        memb_ptr = (const char *)sptr + elm->memb_offset;
    }

    if(ct && ct->range_bits >= 0) {
        if(per_put_few_bits(po, present_enc, ct->range_bits))
            ASN__ENCODE_FAILED;

        return elm->type->op->uper_encoder(
            elm->type, elm->encoding_constraints.per_constraints, memb_ptr, po);
    } else {
        asn_enc_rval_t rval;
        if(specs->ext_start == -1) ASN__ENCODE_FAILED;
        if(uper_put_nsnnwn(po, present_enc - specs->ext_start))
            ASN__ENCODE_FAILED;
        if(uper_open_type_put(elm->type,
                              elm->encoding_constraints.per_constraints,
                              memb_ptr, po))
            ASN__ENCODE_FAILED;
        rval.encoded = 0;
        ASN__ENCODED_OK(rval);
    }
}


int
CHOICE_print(const asn_TYPE_descriptor_t *td, const void *sptr, int ilevel,
             asn_app_consume_bytes_f *cb, void *app_key) {
    const asn_CHOICE_specifics_t *specs = (const asn_CHOICE_specifics_t *)td->specifics;
	unsigned present;

	if(!sptr) return (cb("<absent>", 8, app_key) < 0) ? -1 : 0;

	/*
	 * Figure out which CHOICE element is encoded.
	 */
	present = _fetch_present_idx(sptr, specs->pres_offset,specs->pres_size);

	/*
	 * Print that element.
	 */
	if(present > 0 && present <= td->elements_count) {
		asn_TYPE_member_t *elm = &td->elements[present-1];
		const void *memb_ptr;

		if(elm->flags & ATF_POINTER) {
			memb_ptr = *(const void * const *)((const char *)sptr + elm->memb_offset);
			if(!memb_ptr) return (cb("<absent>", 8, app_key) < 0) ? -1 : 0;
		} else {
			memb_ptr = (const void *)((const char *)sptr + elm->memb_offset);
		}

		/* Print member's name and stuff */
		if(0) {
			if(cb(elm->name, strlen(elm->name), app_key) < 0
			|| cb(": ", 2, app_key) < 0)
				return -1;
		}

		return elm->type->op->print_struct(elm->type, memb_ptr, ilevel,
			cb, app_key);
	} else {
		return (cb("<absent>", 8, app_key) < 0) ? -1 : 0;
	}
}

void
CHOICE_free(const asn_TYPE_descriptor_t *td, void *ptr,
            enum asn_struct_free_method method) {
    const asn_CHOICE_specifics_t *specs =
        (const asn_CHOICE_specifics_t *)td->specifics;
    unsigned present;

	if(!td || !ptr)
		return;

	ASN_DEBUG("Freeing %s as CHOICE", td->name);

	/*
	 * Figure out which CHOICE element is encoded.
	 */
	present = _fetch_present_idx(ptr, specs->pres_offset, specs->pres_size);

	/*
	 * Free that element.
	 */
	if(present > 0 && present <= td->elements_count) {
		asn_TYPE_member_t *elm = &td->elements[present-1];
		void *memb_ptr;

		if(elm->flags & ATF_POINTER) {
			memb_ptr = *(void **)((char *)ptr + elm->memb_offset);
			if(memb_ptr)
				ASN_STRUCT_FREE(*elm->type, memb_ptr);
		} else {
			memb_ptr = (void *)((char *)ptr + elm->memb_offset);
			ASN_STRUCT_FREE_CONTENTS_ONLY(*elm->type, memb_ptr);
		}
	}

    switch(method) {
    case ASFM_FREE_EVERYTHING:
        FREEMEM(ptr);
        break;
    case ASFM_FREE_UNDERLYING:
        break;
    case ASFM_FREE_UNDERLYING_AND_RESET:
        memset(ptr, 0, specs->struct_size);
        break;
    }
}


/*
 * The following functions functions offer protection against -fshort-enums,
 * compatible with little- and big-endian machines.
 * If assertion is triggered, either disable -fshort-enums, or add an entry
 * here with the ->pres_size of your target stracture.
 * Unless the target structure is packed, the ".present" member
 * is guaranteed to be aligned properly. ASN.1 compiler itself does not
 * produce packed code.
 */
static unsigned
_fetch_present_idx(const void *struct_ptr, unsigned pres_offset,
                   unsigned pres_size) {
    const void *present_ptr;
	unsigned present;

	present_ptr = ((const char *)struct_ptr) + pres_offset;

	switch(pres_size) {
	case sizeof(int):	present = *(const unsigned int *)present_ptr; break;
	case sizeof(short):	present = *(const unsigned short *)present_ptr; break;
	case sizeof(char):	present = *(const unsigned char *)present_ptr; break;
	default:
		/* ANSI C mandates enum to be equivalent to integer */
		assert(pres_size != sizeof(int));
		return 0;	/* If not aborted, pass back safe value */
	}

	return present;
}

static void
_set_present_idx(void *struct_ptr, unsigned pres_offset, unsigned pres_size,
                 unsigned present) {
    void *present_ptr;
	present_ptr = ((char *)struct_ptr) + pres_offset;

	switch(pres_size) {
	case sizeof(int):	*(unsigned int *)present_ptr   = present; break;
	case sizeof(short):	*(unsigned short *)present_ptr = present; break;
	case sizeof(char):	*(unsigned char *)present_ptr  = present; break;
	default:
		/* ANSI C mandates enum to be equivalent to integer */
		assert(pres_size != sizeof(int));
	}
}

static const void *
_get_member_ptr(const asn_TYPE_descriptor_t *td, const void *sptr,
                asn_TYPE_member_t **elm_ptr, unsigned *present_out) {
    const asn_CHOICE_specifics_t *specs =
        (const asn_CHOICE_specifics_t *)td->specifics;
    unsigned present;

    if(!sptr) {
        *elm_ptr = NULL;
        *present_out = 0;
        return NULL;
    }

    /*
	 * Figure out which CHOICE element is encoded.
	 */
	present = _fetch_present_idx(sptr, specs->pres_offset, specs->pres_size);
    *present_out = present;

    /*
     * The presence index is intentionally 1-based to avoid
     * treating zeroed structure as a valid one.
     */
	if(present > 0 && present <= td->elements_count) {
        asn_TYPE_member_t *const elm = &td->elements[present - 1];
        const void *memb_ptr;

		if(elm->flags & ATF_POINTER) {
            memb_ptr =
                *(const void *const *)((const char *)sptr + elm->memb_offset);
        } else {
            memb_ptr = (const void *)((const char *)sptr + elm->memb_offset);
        }
        *elm_ptr = elm;
        return memb_ptr;
    } else {
        *elm_ptr = NULL;
        return NULL;
    }

}

int
CHOICE_compare(const asn_TYPE_descriptor_t *td, const void *aptr, const void *bptr) {
    asn_TYPE_member_t *aelm;
    asn_TYPE_member_t *belm;
    unsigned apresent = 0;
    unsigned bpresent = 0;
    const void *amember = _get_member_ptr(td, aptr, &aelm, &apresent);
    const void *bmember = _get_member_ptr(td, bptr, &belm, &bpresent);

    if(amember && bmember) {
        if(apresent == bpresent) {
            assert(aelm == belm);
            return aelm->type->op->compare_struct(aelm->type, amember, bmember);
        } else if(apresent < bpresent) {
            return -1;
        } else {
            return 1;
        }
    } else if(!amember) {
        return -1;
    } else {
        return 1;
    }
}

/*
 * Return the 1-based choice variant presence index.
 * Returns 0 in case of error.
 */
unsigned
CHOICE_variant_get_presence(const asn_TYPE_descriptor_t *td, const void *sptr) {
    const asn_CHOICE_specifics_t *specs =
        (const asn_CHOICE_specifics_t *)td->specifics;
    return _fetch_present_idx(sptr, specs->pres_offset, specs->pres_size);
}

/*
 * Sets or resets the 1-based choice variant presence index.
 * In case a previous index is not zero, the currently selected structure
 * member is freed and zeroed-out first.
 * Returns 0 on success and -1 on error.
 */
int
CHOICE_variant_set_presence(const asn_TYPE_descriptor_t *td, void *sptr,
                            unsigned present) {
    const asn_CHOICE_specifics_t *specs =
        (const asn_CHOICE_specifics_t *)td->specifics;
    unsigned old_present;

    if(!sptr) {
        return -1;
    }

    if(present > td->elements_count)
        return -1;

    old_present =
        _fetch_present_idx(sptr, specs->pres_offset, specs->pres_size);
    if(present == old_present)
        return 0;

    if(old_present != 0) {
        assert(old_present <= td->elements_count);
        ASN_STRUCT_RESET(*td, sptr);
    }

    _set_present_idx(sptr, specs->pres_offset, specs->pres_size, present);

    return 0;
}


asn_random_fill_result_t
CHOICE_random_fill(const asn_TYPE_descriptor_t *td, void **sptr,
                   const asn_encoding_constraints_t *constr,
                   size_t max_length) {
    const asn_CHOICE_specifics_t *specs =
        (const asn_CHOICE_specifics_t *)td->specifics;
    asn_random_fill_result_t res;
    asn_random_fill_result_t result_failed = {ARFILL_FAILED, 0};
    asn_random_fill_result_t result_skipped = {ARFILL_SKIPPED, 0};
    const asn_TYPE_member_t *elm;
    unsigned present;
    void *memb_ptr;   /* Pointer to the member */
    void **memb_ptr2; /* Pointer to that pointer */
    void *st = *sptr;

    if(max_length == 0) return result_skipped;

    (void)constr;

    if(st == NULL) {
        st = CALLOC(1, specs->struct_size);
        if(st == NULL) {
            return result_failed;
        }
    }

    present = asn_random_between(1, td->elements_count);
    elm = &td->elements[present - 1];

	if(elm->flags & ATF_POINTER) {
		/* Member is a pointer to another structure */
		memb_ptr2 = (void **)((char *)st + elm->memb_offset);
	} else {
		memb_ptr = (char *)st + elm->memb_offset;
		memb_ptr2 = &memb_ptr;
	}

    res = elm->type->op->random_fill(elm->type, memb_ptr2,
                                    &elm->encoding_constraints, max_length);
    _set_present_idx(st, specs->pres_offset, specs->pres_size, present);
    if(res.code == ARFILL_OK) {
        *sptr = st;
    } else {
        if(st == *sptr) {
            ASN_STRUCT_RESET(*td, st);
        } else {
            ASN_STRUCT_FREE(*td, st);
        }
    }

    return res;
}


asn_TYPE_operation_t asn_OP_CHOICE = {
	CHOICE_free,
	CHOICE_print,
	CHOICE_compare,
	CHOICE_decode_ber,
	CHOICE_encode_der,
	CHOICE_decode_xer,
	CHOICE_encode_xer,
#ifdef	ASN_DISABLE_OER_SUPPORT
	0,
	0,
#else
	CHOICE_decode_oer,
	CHOICE_encode_oer,
#endif  /* ASN_DISABLE_OER_SUPPORT */
#ifdef ASN_DISABLE_PER_SUPPORT
	0,
	0,
#else
	CHOICE_decode_uper,
	CHOICE_encode_uper,
#endif	/* ASN_DISABLE_PER_SUPPORT */
	CHOICE_random_fill,
	CHOICE_outmost_tag
};
