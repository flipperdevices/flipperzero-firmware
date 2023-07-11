/*-
 * Copyright (c) 2003-2017 Lev Walkin <vlm@lionet.info>.
 * All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
#include <asn_internal.h>
#include <OCTET_STRING.h>
#include <BIT_STRING.h>	/* for .bits_unused member */
#include <errno.h>

/*
 * OCTET STRING basic type description.
 */
static const ber_tlv_tag_t asn_DEF_OCTET_STRING_tags[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (4 << 2))
};
asn_OCTET_STRING_specifics_t asn_SPC_OCTET_STRING_specs = {
	sizeof(OCTET_STRING_t),
	offsetof(OCTET_STRING_t, _asn_ctx),
	ASN_OSUBV_STR
};

asn_TYPE_operation_t asn_OP_OCTET_STRING = {
	OCTET_STRING_free,
	OCTET_STRING_print,	/* OCTET STRING generally means a non-ascii sequence */
	OCTET_STRING_compare,
	OCTET_STRING_decode_ber,
	OCTET_STRING_encode_der,
	OCTET_STRING_decode_xer_hex,
	OCTET_STRING_encode_xer,
#ifdef	ASN_DISABLE_OER_SUPPORT
	0,
	0,
#else
	OCTET_STRING_decode_oer,
	OCTET_STRING_encode_oer,
#endif  /* ASN_DISABLE_OER_SUPPORT */
#ifdef	ASN_DISABLE_PER_SUPPORT
	0,
	0,
#else
	OCTET_STRING_decode_uper,	/* Unaligned PER decoder */
	OCTET_STRING_encode_uper,	/* Unaligned PER encoder */
#endif	/* ASN_DISABLE_PER_SUPPORT */
	OCTET_STRING_random_fill,
	0	/* Use generic outmost tag fetcher */
};
asn_TYPE_descriptor_t asn_DEF_OCTET_STRING = {
	"OCTET STRING",		/* Canonical name */
	"OCTET_STRING",		/* XML tag name */
	&asn_OP_OCTET_STRING,
	asn_DEF_OCTET_STRING_tags,
	sizeof(asn_DEF_OCTET_STRING_tags)
	  / sizeof(asn_DEF_OCTET_STRING_tags[0]),
	asn_DEF_OCTET_STRING_tags,	/* Same as above */
	sizeof(asn_DEF_OCTET_STRING_tags)
	  / sizeof(asn_DEF_OCTET_STRING_tags[0]),
	{ 0, 0, asn_generic_no_constraint },
	0, 0,	/* No members */
	&asn_SPC_OCTET_STRING_specs
};

#undef	_CH_PHASE
#undef	NEXT_PHASE
#undef	PREV_PHASE
#define	_CH_PHASE(ctx, inc) do {					\
		if(ctx->phase == 0)					\
			ctx->context = 0;				\
		ctx->phase += inc;					\
	} while(0)
#define	NEXT_PHASE(ctx)	_CH_PHASE(ctx, +1)
#define	PREV_PHASE(ctx)	_CH_PHASE(ctx, -1)

#undef	ADVANCE
#define	ADVANCE(num_bytes)	do {					\
		size_t num = (num_bytes);				\
		buf_ptr = ((const char *)buf_ptr) + num;		\
		size -= num;						\
		consumed_myself += num;					\
	} while(0)

#undef	RETURN
#define	RETURN(_code)	do {						\
		asn_dec_rval_t tmprval;					\
		tmprval.code = _code;					\
		tmprval.consumed = consumed_myself;			\
		return tmprval;						\
	} while(0)

#undef	APPEND
#define	APPEND(bufptr, bufsize)	do {					\
		size_t _bs = (bufsize);		/* Append size */	\
		size_t _ns = ctx->context;	/* Allocated now */	\
		size_t _es = st->size + _bs;	/* Expected size */	\
		/* int is really a typeof(st->size): */			\
		if((int)_es < 0) RETURN(RC_FAIL);			\
		if(_ns <= _es) {					\
			void *ptr;					\
			/* Be nice and round to the memory allocator */	\
			do { _ns = _ns ? _ns << 1 : 16; }		\
			    while(_ns <= _es);				\
			/* int is really a typeof(st->size): */		\
			if((int)_ns < 0) RETURN(RC_FAIL);		\
			ptr = REALLOC(st->buf, _ns);			\
			if(ptr) {					\
				st->buf = (uint8_t *)ptr;		\
				ctx->context = _ns;			\
			} else {					\
				RETURN(RC_FAIL);			\
			}						\
			ASN_DEBUG("Reallocating into %ld", (long)_ns);	\
		}							\
		memcpy(st->buf + st->size, bufptr, _bs);		\
		/* Convenient nul-termination */			\
		st->buf[_es] = '\0';					\
		st->size = _es;						\
	} while(0)

/*
 * The main reason why ASN.1 is still alive is that too much time and effort
 * is necessary for learning it more or less adequately, thus creating a gut
 * necessity to demonstrate that aquired skill everywhere afterwards.
 * No, I am not going to explain what the following stuff is.
 */
struct _stack_el {
    ber_tlv_len_t left;     /* What's left to read (or -1) */
    ber_tlv_len_t got;      /* What was actually processed */
    unsigned cont_level;    /* Depth of subcontainment */
    int want_nulls;         /* Want null "end of content" octets? */
    int bits_chopped;       /* Flag in BIT STRING mode */
    ber_tlv_tag_t tag;      /* For debugging purposes */
    struct _stack_el *prev;
    struct _stack_el *next;
};
struct _stack {
	struct _stack_el *tail;
	struct _stack_el *cur_ptr;
};

static struct _stack_el *
OS__add_stack_el(struct _stack *st) {
	struct _stack_el *nel;

	/*
	 * Reuse the old stack frame or allocate a new one.
	 */
	if(st->cur_ptr && st->cur_ptr->next) {
		nel = st->cur_ptr->next;
		nel->bits_chopped = 0;
		nel->got = 0;
		/* Retain the nel->cont_level, it's correct. */
	} else {
		nel = (struct _stack_el *)CALLOC(1, sizeof(struct _stack_el));
		if(nel == NULL)
			return NULL;
	
		if(st->tail) {
			/* Increase a subcontainment depth */
			nel->cont_level = st->tail->cont_level + 1;
			st->tail->next = nel;
		}
		nel->prev = st->tail;
		st->tail = nel;
	}

	st->cur_ptr = nel;

	return nel;
}

static struct _stack *
_new_stack(void) {
	return (struct _stack *)CALLOC(1, sizeof(struct _stack));
}

/*
 * Decode OCTET STRING type.
 */
asn_dec_rval_t
OCTET_STRING_decode_ber(const asn_codec_ctx_t *opt_codec_ctx,
                        const asn_TYPE_descriptor_t *td, void **sptr,
                        const void *buf_ptr, size_t size, int tag_mode) {
    const asn_OCTET_STRING_specifics_t *specs = td->specifics
				? (const asn_OCTET_STRING_specifics_t *)td->specifics
				: &asn_SPC_OCTET_STRING_specs;
	BIT_STRING_t *st = (BIT_STRING_t *)*sptr;
	asn_dec_rval_t rval;
	asn_struct_ctx_t *ctx;
	ssize_t consumed_myself = 0;
	struct _stack *stck;		/* Expectations stack structure */
	struct _stack_el *sel = 0;	/* Stack element */
	int tlv_constr;
	enum asn_OS_Subvariant type_variant = specs->subvariant;

	ASN_DEBUG("Decoding %s as %s (frame %ld)",
		td->name,
		(type_variant == ASN_OSUBV_STR) ?
			"OCTET STRING" : "OS-SpecialCase",
		(long)size);

	/*
	 * Create the string if does not exist.
	 */
	if(st == NULL) {
		st = (BIT_STRING_t *)(*sptr = CALLOC(1, specs->struct_size));
		if(st == NULL) RETURN(RC_FAIL);
	}

	/* Restore parsing context */
	ctx = (asn_struct_ctx_t *)((char *)st + specs->ctx_offset);

	switch(ctx->phase) {
	case 0:
		/*
		 * Check tags.
		 */
		rval = ber_check_tags(opt_codec_ctx, td, ctx,
			buf_ptr, size, tag_mode, -1,
			&ctx->left, &tlv_constr);
		if(rval.code != RC_OK)
			return rval;

		if(tlv_constr) {
			/*
			 * Complex operation, requires stack of expectations.
			 */
			ctx->ptr = _new_stack();
			if(!ctx->ptr) {
				RETURN(RC_FAIL);
			}
		} else {
			/*
			 * Jump into stackless primitive decoding.
			 */
			_CH_PHASE(ctx, 3);
			if(type_variant == ASN_OSUBV_ANY && tag_mode != 1)
				APPEND(buf_ptr, rval.consumed);
			ADVANCE(rval.consumed);
			goto phase3;
		}

		NEXT_PHASE(ctx);
		/* Fall through */
	case 1:
	phase1:
		/*
		 * Fill the stack with expectations.
		 */
		stck = (struct _stack *)ctx->ptr;
		sel = stck->cur_ptr;
	  do {
		ber_tlv_tag_t tlv_tag;
		ber_tlv_len_t tlv_len;
		ber_tlv_tag_t expected_tag;
		ssize_t tl, ll, tlvl;
				/* This one works even if (sel->left == -1) */
		size_t Left = ((!sel||(size_t)sel->left >= size)
					?size:(size_t)sel->left);


		ASN_DEBUG("%p, s->l=%ld, s->wn=%ld, s->g=%ld\n", (void *)sel,
			(long)(sel?sel->left:0),
			(long)(sel?sel->want_nulls:0),
			(long)(sel?sel->got:0)
		);
		if(sel && sel->left <= 0 && sel->want_nulls == 0) {
			if(sel->prev) {
				struct _stack_el *prev = sel->prev;
				if(prev->left != -1) {
					if(prev->left < sel->got)
						RETURN(RC_FAIL);
					prev->left -= sel->got;
				}
				prev->got += sel->got;
				sel = stck->cur_ptr = prev;
				if(!sel) break;
				tlv_constr = 1;
				continue;
			} else {
				sel = stck->cur_ptr = 0;
				break;	/* Nothing to wait */
			}
		}

		tl = ber_fetch_tag(buf_ptr, Left, &tlv_tag);
		ASN_DEBUG("fetch tag(size=%ld,L=%ld), %sstack, left=%ld, wn=%ld, tl=%ld",
			(long)size, (long)Left, sel?"":"!",
			(long)(sel?sel->left:0),
			(long)(sel?sel->want_nulls:0),
			(long)tl);
		switch(tl) {
		case -1: RETURN(RC_FAIL);
		case 0: RETURN(RC_WMORE);
		}

		tlv_constr = BER_TLV_CONSTRUCTED(buf_ptr);

		ll = ber_fetch_length(tlv_constr,
				(const char *)buf_ptr + tl,Left - tl,&tlv_len);
		ASN_DEBUG("Got tag=%s, tc=%d, left=%ld, tl=%ld, len=%ld, ll=%ld",
			ber_tlv_tag_string(tlv_tag), tlv_constr,
				(long)Left, (long)tl, (long)tlv_len, (long)ll);
		switch(ll) {
		case -1: RETURN(RC_FAIL);
		case 0: RETURN(RC_WMORE);
		}

		if(sel && sel->want_nulls
			&& ((const uint8_t *)buf_ptr)[0] == 0
			&& ((const uint8_t *)buf_ptr)[1] == 0)
		{

			ASN_DEBUG("Eat EOC; wn=%d--", sel->want_nulls);

			if(type_variant == ASN_OSUBV_ANY
			&& (tag_mode != 1 || sel->cont_level))
				APPEND("\0\0", 2);

			ADVANCE(2);
			sel->got += 2;
			if(sel->left != -1) {
				sel->left -= 2;	/* assert(sel->left >= 2) */
			}

			sel->want_nulls--;
			if(sel->want_nulls == 0) {
				/* Move to the next expectation */
				sel->left = 0;
				tlv_constr = 1;
			}

			continue;
		}

		/*
		 * Set up expected tags,
		 * depending on ASN.1 type being decoded.
		 */
		switch(type_variant) {
		case ASN_OSUBV_BIT:
			/* X.690: 8.6.4.1, NOTE 2 */
			/* Fall through */
		case ASN_OSUBV_STR:
		default:
			if(sel) {
				unsigned level = sel->cont_level;
				if(level < td->all_tags_count) {
					expected_tag = td->all_tags[level];
					break;
				} else if(td->all_tags_count) {
					expected_tag = td->all_tags
						[td->all_tags_count - 1];
					break;
				}
				/* else, Fall through */
			}
			/* Fall through */
		case ASN_OSUBV_ANY:
			expected_tag = tlv_tag;
			break;
		}


		if(tlv_tag != expected_tag) {
			char buf[2][32];
			ber_tlv_tag_snprint(tlv_tag,
				buf[0], sizeof(buf[0]));
			ber_tlv_tag_snprint(td->tags[td->tags_count-1],
				buf[1], sizeof(buf[1]));
			ASN_DEBUG("Tag does not match expectation: %s != %s",
				buf[0], buf[1]);
			RETURN(RC_FAIL);
		}

		tlvl = tl + ll;	/* Combined length of T and L encoding */
		if((tlv_len + tlvl) < 0) {
			/* tlv_len value is too big */
			ASN_DEBUG("TLV encoding + length (%ld) is too big",
				(long)tlv_len);
			RETURN(RC_FAIL);
		}

		/*
		 * Append a new expectation.
		 */
		sel = OS__add_stack_el(stck);
		if(!sel) RETURN(RC_FAIL);

		sel->tag = tlv_tag;

		sel->want_nulls = (tlv_len==-1);
		if(sel->prev && sel->prev->left != -1) {
			/* Check that the parent frame is big enough */
			if(sel->prev->left < tlvl + (tlv_len==-1?0:tlv_len))
				RETURN(RC_FAIL);
			if(tlv_len == -1)
				sel->left = sel->prev->left - tlvl;
			else
				sel->left = tlv_len;
		} else {
			sel->left = tlv_len;
		}
		if(type_variant == ASN_OSUBV_ANY
		&& (tag_mode != 1 || sel->cont_level))
			APPEND(buf_ptr, tlvl);
		sel->got += tlvl;
		ADVANCE(tlvl);

		ASN_DEBUG("+EXPECT2 got=%ld left=%ld, wn=%d, clvl=%u",
			(long)sel->got, (long)sel->left,
			sel->want_nulls, sel->cont_level);

	  } while(tlv_constr);
		if(sel == NULL) {
			/* Finished operation, "phase out" */
			ASN_DEBUG("Phase out");
			_CH_PHASE(ctx, +3);
			break;
		}

		NEXT_PHASE(ctx);
		/* Fall through */
	case 2:
		stck = (struct _stack *)ctx->ptr;
		sel = stck->cur_ptr;
		ASN_DEBUG("Phase 2: Need %ld bytes, size=%ld, alrg=%ld, wn=%d",
			(long)sel->left, (long)size, (long)sel->got,
				sel->want_nulls);
	    {
		ber_tlv_len_t len;

		assert(sel->left >= 0);

		len = ((ber_tlv_len_t)size < sel->left)
				? (ber_tlv_len_t)size : sel->left;
		if(len > 0) {
			if(type_variant == ASN_OSUBV_BIT
			&& sel->bits_chopped == 0) {
				/* Put the unused-bits-octet away */
				st->bits_unused = *(const uint8_t *)buf_ptr;
				APPEND(((const char *)buf_ptr+1), (len - 1));
				sel->bits_chopped = 1;
			} else {
				APPEND(buf_ptr, len);
			}
			ADVANCE(len);
			sel->left -= len;
			sel->got += len;
		}

		if(sel->left) {
			ASN_DEBUG("OS left %ld, size = %ld, wn=%d\n",
				(long)sel->left, (long)size, sel->want_nulls);
			RETURN(RC_WMORE);
		}

		PREV_PHASE(ctx);
		goto phase1;
	    }
		break;
	case 3:
	phase3:
		/*
		 * Primitive form, no stack required.
		 */
		assert(ctx->left >= 0);

		if(size < (size_t)ctx->left) {
			if(!size) RETURN(RC_WMORE);
			if(type_variant == ASN_OSUBV_BIT && !ctx->context) {
				st->bits_unused = *(const uint8_t *)buf_ptr;
				ctx->left--;
				ADVANCE(1);
			}
			APPEND(buf_ptr, size);
			assert(ctx->context > 0);
			ctx->left -= size;
			ADVANCE(size);
			RETURN(RC_WMORE);
		} else {
			if(type_variant == ASN_OSUBV_BIT
			&& !ctx->context && ctx->left) {
				st->bits_unused = *(const uint8_t *)buf_ptr;
				ctx->left--;
				ADVANCE(1);
			}
			APPEND(buf_ptr, ctx->left);
			ADVANCE(ctx->left);
			ctx->left = 0;

			NEXT_PHASE(ctx);
		}
		break;
	}

	if(sel) {
		ASN_DEBUG("3sel p=%p, wn=%d, l=%ld, g=%ld, size=%ld",
			(void *)sel->prev, sel->want_nulls,
			(long)sel->left, (long)sel->got, (long)size);
		if(sel->prev || sel->want_nulls > 1 || sel->left > 0) {
			RETURN(RC_WMORE);
		}
	}

	/*
	 * BIT STRING-specific processing.
	 */
	if(type_variant == ASN_OSUBV_BIT) {
        if(st->size) {
			if(st->bits_unused < 0 || st->bits_unused > 7) {
				RETURN(RC_FAIL);
			}
			/* Finalize BIT STRING: zero out unused bits. */
			st->buf[st->size-1] &= 0xff << st->bits_unused;
		} else {
			if(st->bits_unused) {
				RETURN(RC_FAIL);
			}
		}
	}

	ASN_DEBUG("Took %ld bytes to encode %s: [%s]:%ld",
		(long)consumed_myself, td->name,
		(type_variant == ASN_OSUBV_STR) ? (char *)st->buf : "<data>",
		(long)st->size);


	RETURN(RC_OK);
}

/*
 * Encode OCTET STRING type using DER.
 */
asn_enc_rval_t
OCTET_STRING_encode_der(const asn_TYPE_descriptor_t *td, const void *sptr,
                        int tag_mode, ber_tlv_tag_t tag,
                        asn_app_consume_bytes_f *cb, void *app_key) {
    asn_enc_rval_t er;
	const asn_OCTET_STRING_specifics_t *specs = td->specifics
				? (const asn_OCTET_STRING_specifics_t *)td->specifics
				: &asn_SPC_OCTET_STRING_specs;
	const BIT_STRING_t *st = (const BIT_STRING_t *)sptr;
	enum asn_OS_Subvariant type_variant = specs->subvariant;
	int fix_last_byte = 0;

	ASN_DEBUG("%s %s as OCTET STRING",
		cb?"Estimating":"Encoding", td->name);

	/*
	 * Write tags.
	 */
	if(type_variant != ASN_OSUBV_ANY || tag_mode == 1) {
		er.encoded = der_write_tags(td,
				(type_variant == ASN_OSUBV_BIT) + st->size,
			tag_mode, type_variant == ASN_OSUBV_ANY, tag,
			cb, app_key);
		if(er.encoded == -1) {
			er.failed_type = td;
			er.structure_ptr = sptr;
			return er;
		}
	} else {
		/* Disallow: [<tag>] IMPLICIT ANY */
		assert(type_variant != ASN_OSUBV_ANY || tag_mode != -1);
		er.encoded = 0;
	}

	if(!cb) {
		er.encoded += (type_variant == ASN_OSUBV_BIT) + st->size;
		ASN__ENCODED_OK(er);
	}

	/*
	 * Prepare to deal with the last octet of BIT STRING.
	 */
	if(type_variant == ASN_OSUBV_BIT) {
		uint8_t b = st->bits_unused & 0x07;
		if(b && st->size) fix_last_byte = 1;
		ASN__CALLBACK(&b, 1);
	}

	/* Invoke callback for the main part of the buffer */
	ASN__CALLBACK(st->buf, st->size - fix_last_byte);

	/* The last octet should be stripped off the unused bits */
	if(fix_last_byte) {
		uint8_t b = st->buf[st->size-1] & (0xff << st->bits_unused);
		ASN__CALLBACK(&b, 1);
	}

	ASN__ENCODED_OK(er);
cb_failed:
	ASN__ENCODE_FAILED;
}

asn_enc_rval_t
OCTET_STRING_encode_xer(const asn_TYPE_descriptor_t *td, const void *sptr,
                        int ilevel, enum xer_encoder_flags_e flags,
                        asn_app_consume_bytes_f *cb, void *app_key) {
    const char * const h2c = "0123456789ABCDEF";
	const OCTET_STRING_t *st = (const OCTET_STRING_t *)sptr;
	asn_enc_rval_t er;
	char scratch[16 * 3 + 4];
	char *p = scratch;
	uint8_t *buf;
	uint8_t *end;
	size_t i;

	if(!st || (!st->buf && st->size))
		ASN__ENCODE_FAILED;

	er.encoded = 0;

	/*
	 * Dump the contents of the buffer in hexadecimal.
	 */
	buf = st->buf;
	end = buf + st->size;
	if(flags & XER_F_CANONICAL) {
		char *scend = scratch + (sizeof(scratch) - 2);
		for(; buf < end; buf++) {
			if(p >= scend) {
				ASN__CALLBACK(scratch, p - scratch);
				p = scratch;
			}
			*p++ = h2c[(*buf >> 4) & 0x0F];
			*p++ = h2c[*buf & 0x0F];
		}

		ASN__CALLBACK(scratch, p-scratch);	/* Dump the rest */
	} else {
		for(i = 0; buf < end; buf++, i++) {
			if(!(i % 16) && (i || st->size > 16)) {
				ASN__CALLBACK(scratch, p-scratch);
				p = scratch;
				ASN__TEXT_INDENT(1, ilevel);
			}
			*p++ = h2c[(*buf >> 4) & 0x0F];
			*p++ = h2c[*buf & 0x0F];
			*p++ = 0x20;
		}
		if(p - scratch) {
			p--;	/* Remove the tail space */
			ASN__CALLBACK(scratch, p-scratch); /* Dump the rest */
			if(st->size > 16)
				ASN__TEXT_INDENT(1, ilevel-1);
		}
	}

	ASN__ENCODED_OK(er);
cb_failed:
	ASN__ENCODE_FAILED;
}

static const struct OCTET_STRING__xer_escape_table_s {
	const char *string;
	int size;
} OCTET_STRING__xer_escape_table[] = {
#define	OSXET(s)	{ s, sizeof(s) - 1 }
	OSXET("\074\156\165\154\057\076"),	/* <nul/> */
	OSXET("\074\163\157\150\057\076"),	/* <soh/> */
	OSXET("\074\163\164\170\057\076"),	/* <stx/> */
	OSXET("\074\145\164\170\057\076"),	/* <etx/> */
	OSXET("\074\145\157\164\057\076"),	/* <eot/> */
	OSXET("\074\145\156\161\057\076"),	/* <enq/> */
	OSXET("\074\141\143\153\057\076"),	/* <ack/> */
	OSXET("\074\142\145\154\057\076"),	/* <bel/> */
	OSXET("\074\142\163\057\076"),		/* <bs/> */
	OSXET("\011"),				/* \t */
	OSXET("\012"),				/* \n */
	OSXET("\074\166\164\057\076"),		/* <vt/> */
	OSXET("\074\146\146\057\076"),		/* <ff/> */
	OSXET("\015"),				/* \r */
	OSXET("\074\163\157\057\076"),		/* <so/> */
	OSXET("\074\163\151\057\076"),		/* <si/> */
	OSXET("\074\144\154\145\057\076"),	/* <dle/> */
	OSXET("\074\144\143\061\057\076"),	/* <de1/> */
	OSXET("\074\144\143\062\057\076"),	/* <de2/> */
	OSXET("\074\144\143\063\057\076"),	/* <de3/> */
	OSXET("\074\144\143\064\057\076"),	/* <de4/> */
	OSXET("\074\156\141\153\057\076"),	/* <nak/> */
	OSXET("\074\163\171\156\057\076"),	/* <syn/> */
	OSXET("\074\145\164\142\057\076"),	/* <etb/> */
	OSXET("\074\143\141\156\057\076"),	/* <can/> */
	OSXET("\074\145\155\057\076"),		/* <em/> */
	OSXET("\074\163\165\142\057\076"),	/* <sub/> */
	OSXET("\074\145\163\143\057\076"),	/* <esc/> */
	OSXET("\074\151\163\064\057\076"),	/* <is4/> */
	OSXET("\074\151\163\063\057\076"),	/* <is3/> */
	OSXET("\074\151\163\062\057\076"),	/* <is2/> */
	OSXET("\074\151\163\061\057\076"),	/* <is1/> */
	{ 0, 0 },	/* " " */
	{ 0, 0 },	/* ! */
	{ 0, 0 },	/* \" */
	{ 0, 0 },	/* # */
	{ 0, 0 },	/* $ */
	{ 0, 0 },	/* % */
	OSXET("\046\141\155\160\073"),	/* &amp; */
	{ 0, 0 },	/* ' */
	{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}, /* ()*+,-./ */
	{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}, /* 01234567 */
	{0,0},{0,0},{0,0},{0,0},			 /* 89:; */
	OSXET("\046\154\164\073"),	/* &lt; */
	{ 0, 0 },	/* = */
	OSXET("\046\147\164\073"),	/* &gt; */
};

static int
OS__check_escaped_control_char(const void *buf, int size) {
	size_t i;
	/*
	 * Inefficient algorithm which translates the escape sequences
	 * defined above into characters. Returns -1 if not found.
	 * TODO: replace by a faster algorithm (bsearch(), hash or
	 * nested table lookups).
	 */
	for(i = 0; i < 32 /* Don't spend time on the bottom half */; i++) {
		const struct OCTET_STRING__xer_escape_table_s *el;
		el = &OCTET_STRING__xer_escape_table[i];
		if(el->size == size && memcmp(buf, el->string, size) == 0)
			return i;
	}
	return -1;
}

static int
OCTET_STRING__handle_control_chars(void *struct_ptr, const void *chunk_buf, size_t chunk_size) {
	/*
	 * This might be one of the escape sequences
	 * for control characters. Check it out.
	 * #11.15.5
	 */
	int control_char = OS__check_escaped_control_char(chunk_buf,chunk_size);
	if(control_char >= 0) {
		OCTET_STRING_t *st = (OCTET_STRING_t *)struct_ptr;
		void *p = REALLOC(st->buf, st->size + 2);
		if(p) {
			st->buf = (uint8_t *)p;
			st->buf[st->size++] = control_char;
			st->buf[st->size] = '\0';	/* nul-termination */
			return 0;
		}
	}
	
	return -1;	/* No, it's not */
}

asn_enc_rval_t
OCTET_STRING_encode_xer_utf8(const asn_TYPE_descriptor_t *td, const void *sptr,
                             int ilevel, enum xer_encoder_flags_e flags,
                             asn_app_consume_bytes_f *cb, void *app_key) {
    const OCTET_STRING_t *st = (const OCTET_STRING_t *)sptr;
	asn_enc_rval_t er;
	uint8_t *buf, *end;
	uint8_t *ss;	/* Sequence start */
	ssize_t encoded_len = 0;

	(void)ilevel;	/* Unused argument */
	(void)flags;	/* Unused argument */

	if(!st || (!st->buf && st->size))
		ASN__ENCODE_FAILED;

	buf = st->buf;
	end = buf + st->size;
	for(ss = buf; buf < end; buf++) {
		unsigned int ch = *buf;
		int s_len;	/* Special encoding sequence length */

		/*
		 * Escape certain characters: X.680/11.15
		 */
		if(ch < sizeof(OCTET_STRING__xer_escape_table)
			/sizeof(OCTET_STRING__xer_escape_table[0])
		&& (s_len = OCTET_STRING__xer_escape_table[ch].size)) {
			if(((buf - ss) && cb(ss, buf - ss, app_key) < 0)
			|| cb(OCTET_STRING__xer_escape_table[ch].string, s_len,
					app_key) < 0)
				ASN__ENCODE_FAILED;
			encoded_len += (buf - ss) + s_len;
			ss = buf + 1;
		}
	}

	encoded_len += (buf - ss);
	if((buf - ss) && cb(ss, buf - ss, app_key) < 0)
		ASN__ENCODE_FAILED;

	er.encoded = encoded_len;
	ASN__ENCODED_OK(er);
}

/*
 * Convert from hexadecimal format (cstring): "AB CD EF"
 */
static ssize_t OCTET_STRING__convert_hexadecimal(void *sptr, const void *chunk_buf, size_t chunk_size, int have_more) {
	OCTET_STRING_t *st = (OCTET_STRING_t *)sptr;
	const char *chunk_stop = (const char *)chunk_buf;
	const char *p = chunk_stop;
	const char *pend = p + chunk_size;
	unsigned int clv = 0;
	int half = 0;	/* Half bit */
	uint8_t *buf;

	/* Reallocate buffer according to high cap estimation */
	size_t new_size = st->size + (chunk_size + 1) / 2;
	void *nptr = REALLOC(st->buf, new_size + 1);
	if(!nptr) return -1;
	st->buf = (uint8_t *)nptr;
	buf = st->buf + st->size;

	/*
	 * If something like " a b c " appears here, the " a b":3 will be
	 * converted, and the rest skipped. That is, unless buf_size is greater
	 * than chunk_size, then it'll be equivalent to "ABC0".
	 */
	for(; p < pend; p++) {
		int ch = *(const unsigned char *)p;
		switch(ch) {
		case 0x09: case 0x0a: case 0x0c: case 0x0d:
		case 0x20:
			/* Ignore whitespace */
			continue;
		case 0x30: case 0x31: case 0x32: case 0x33: case 0x34: /*01234*/
		case 0x35: case 0x36: case 0x37: case 0x38: case 0x39: /*56789*/
			clv = (clv << 4) + (ch - 0x30);
			break;
		case 0x41: case 0x42: case 0x43:	/* ABC */
		case 0x44: case 0x45: case 0x46:	/* DEF */
			clv = (clv << 4) + (ch - 0x41 + 10);
			break;
		case 0x61: case 0x62: case 0x63:	/* abc */
		case 0x64: case 0x65: case 0x66:	/* def */
			clv = (clv << 4) + (ch - 0x61 + 10);
			break;
		default:
			*buf = 0;	/* JIC */
			return -1;
		}
		if(half++) {
			half = 0;
			*buf++ = clv;
			chunk_stop = p + 1;
		}
	}

	/*
	 * Check partial decoding.
	 */
	if(half) {
		if(have_more) {
			/*
			 * Partial specification is fine,
			 * because no more more PXER_TEXT data is available.
			 */
			*buf++ = clv << 4;
			chunk_stop = p;
		}
	} else {
		chunk_stop = p;
	}

	st->size = buf - st->buf;	/* Adjust the buffer size */
	assert(st->size <= new_size);
	st->buf[st->size] = 0;		/* Courtesy termination */

	return (chunk_stop - (const char *)chunk_buf);	/* Converted size */
}

/*
 * Convert from binary format: "00101011101"
 */
static ssize_t OCTET_STRING__convert_binary(void *sptr, const void *chunk_buf, size_t chunk_size, int have_more) {
	BIT_STRING_t *st = (BIT_STRING_t *)sptr;
	const char *p = (const char *)chunk_buf;
	const char *pend = p + chunk_size;
	int bits_unused = st->bits_unused & 0x7;
	uint8_t *buf;

	/* Reallocate buffer according to high cap estimation */
	size_t new_size = st->size + (chunk_size + 7) / 8;
	void *nptr = REALLOC(st->buf, new_size + 1);
	if(!nptr) return -1;
	st->buf = (uint8_t *)nptr;
	buf = st->buf + st->size;

	(void)have_more;

	if(bits_unused == 0)
		bits_unused = 8;
	else if(st->size)
		buf--;

	/*
	 * Convert series of 0 and 1 into the octet string.
	 */
	for(; p < pend; p++) {
		int ch = *(const unsigned char *)p;
		switch(ch) {
		case 0x09: case 0x0a: case 0x0c: case 0x0d:
		case 0x20:
			/* Ignore whitespace */
			break;
		case 0x30:
		case 0x31:
			if(bits_unused-- <= 0) {
				*++buf = 0;	/* Clean the cell */
				bits_unused = 7;
			}
			*buf |= (ch&1) << bits_unused;
			break;
		default:
			st->bits_unused = bits_unused;
			return -1;
		}
	}

	if(bits_unused == 8) {
		st->size = buf - st->buf;
		st->bits_unused = 0;
	} else {
		st->size = buf - st->buf + 1;
		st->bits_unused = bits_unused;
	}

	assert(st->size <= new_size);
	st->buf[st->size] = 0;		/* Courtesy termination */

	return chunk_size;	/* Converted in full */
}

/*
 * Something like strtod(), but with stricter rules.
 */
static int
OS__strtoent(int base, const char *buf, const char *end, int32_t *ret_value) {
	const int32_t last_unicode_codepoint = 0x10ffff;
	int32_t val = 0;
	const char *p;

	for(p = buf; p < end; p++) {
		int ch = *p;

		switch(ch) {
		case 0x30: case 0x31: case 0x32: case 0x33: case 0x34: /*01234*/
		case 0x35: case 0x36: case 0x37: case 0x38: case 0x39: /*56789*/
			val = val * base + (ch - 0x30);
			break;
		case 0x41: case 0x42: case 0x43:	/* ABC */
		case 0x44: case 0x45: case 0x46:	/* DEF */
			val = val * base + (ch - 0x41 + 10);
			break;
		case 0x61: case 0x62: case 0x63:	/* abc */
		case 0x64: case 0x65: case 0x66:	/* def */
			val = val * base + (ch - 0x61 + 10);
			break;
		case 0x3b:	/* ';' */
			*ret_value = val;
			return (p - buf) + 1;
		default:
			return -1;	/* Character set error */
		}

		/* Value exceeds the Unicode range. */
		if(val > last_unicode_codepoint) {
			return -1;
		}
	}

	*ret_value = -1;
	return (p - buf);
}

/*
 * Convert from the plain UTF-8 format, expanding entity references: "2 &lt; 3"
 */
static ssize_t
OCTET_STRING__convert_entrefs(void *sptr, const void *chunk_buf,
                              size_t chunk_size, int have_more) {
    OCTET_STRING_t *st = (OCTET_STRING_t *)sptr;
	const char *p = (const char *)chunk_buf;
	const char *pend = p + chunk_size;
	uint8_t *buf;

	/* Reallocate buffer */
	size_t new_size = st->size + chunk_size;
	void *nptr = REALLOC(st->buf, new_size + 1);
	if(!nptr) return -1;
	st->buf = (uint8_t *)nptr;
	buf = st->buf + st->size;

	/*
	 * Convert series of 0 and 1 into the octet string.
	 */
	for(; p < pend; p++) {
		int ch = *(const unsigned char *)p;
		int len;	/* Length of the rest of the chunk */

		if(ch != 0x26 /* '&' */) {
			*buf++ = ch;
			continue;	/* That was easy... */
		}

		/*
		 * Process entity reference.
		 */
		len = chunk_size - (p - (const char *)chunk_buf);
		if(len == 1 /* "&" */) goto want_more;
		if(p[1] == 0x23 /* '#' */) {
			const char *pval;	/* Pointer to start of digits */
			int32_t val = 0;	/* Entity reference value */
			int base;

			if(len == 2 /* "&#" */) goto want_more;
			if(p[2] == 0x78 /* 'x' */)
				pval = p + 3, base = 16;
			else
				pval = p + 2, base = 10;
			len = OS__strtoent(base, pval, p + len, &val);
			if(len == -1) {
				/* Invalid charset. Just copy verbatim. */
				*buf++ = ch;
				continue;
			}
			if(!len || pval[len-1] != 0x3b) goto want_more;
			assert(val > 0);
			p += (pval - p) + len - 1; /* Advance past entref */

			if(val < 0x80) {
				*buf++ = (char)val;
			} else if(val < 0x800) {
				*buf++ = 0xc0 | ((val >> 6));
				*buf++ = 0x80 | ((val & 0x3f));
			} else if(val < 0x10000) {
				*buf++ = 0xe0 | ((val >> 12));
				*buf++ = 0x80 | ((val >> 6) & 0x3f);
				*buf++ = 0x80 | ((val & 0x3f));
			} else if(val < 0x200000) {
				*buf++ = 0xf0 | ((val >> 18));
				*buf++ = 0x80 | ((val >> 12) & 0x3f);
				*buf++ = 0x80 | ((val >> 6) & 0x3f);
				*buf++ = 0x80 | ((val & 0x3f));
			} else if(val < 0x4000000) {
				*buf++ = 0xf8 | ((val >> 24));
				*buf++ = 0x80 | ((val >> 18) & 0x3f);
				*buf++ = 0x80 | ((val >> 12) & 0x3f);
				*buf++ = 0x80 | ((val >> 6) & 0x3f);
				*buf++ = 0x80 | ((val & 0x3f));
			} else {
				*buf++ = 0xfc | ((val >> 30) & 0x1);
				*buf++ = 0x80 | ((val >> 24) & 0x3f);
				*buf++ = 0x80 | ((val >> 18) & 0x3f);
				*buf++ = 0x80 | ((val >> 12) & 0x3f);
				*buf++ = 0x80 | ((val >> 6) & 0x3f);
				*buf++ = 0x80 | ((val & 0x3f));
			}
		} else {
			/*
			 * Ugly, limited parsing of &amp; &gt; &lt;
			 */
			char *sc = (char *)memchr(p, 0x3b, len > 5 ? 5 : len);
			if(!sc) goto want_more;
			if((sc - p) == 4
				&& p[1] == 0x61	/* 'a' */
				&& p[2] == 0x6d	/* 'm' */
				&& p[3] == 0x70	/* 'p' */) {
				*buf++ = 0x26;
				p = sc;
				continue;
			}
			if((sc - p) == 3) {
				if(p[1] == 0x6c) {
					*buf = 0x3c;	/* '<' */
				} else if(p[1] == 0x67) {
					*buf = 0x3e;	/* '>' */
				} else {
					/* Unsupported entity reference */
					*buf++ = ch;
					continue;
				}
				if(p[2] != 0x74) {
					/* Unsupported entity reference */
					*buf++ = ch;
					continue;
				}
				buf++;
				p = sc;
				continue;
			}
			/* Unsupported entity reference */
			*buf++ = ch;
		}

		continue;
	want_more:
		if(have_more) {
			/*
			 * We know that no more data (of the same type)
			 * is coming. Copy the rest verbatim.
			 */
			*buf++ = ch;
			continue;
		}
		chunk_size = (p - (const char *)chunk_buf);
		/* Processing stalled: need more data */
		break;
	}

	st->size = buf - st->buf;
	assert(st->size <= new_size);
	st->buf[st->size] = 0;		/* Courtesy termination */

	return chunk_size;	/* Converted in full */
}

/*
 * Decode OCTET STRING from the XML element's body.
 */
static asn_dec_rval_t
OCTET_STRING__decode_xer(
    const asn_codec_ctx_t *opt_codec_ctx, const asn_TYPE_descriptor_t *td,
    void **sptr, const char *opt_mname, const void *buf_ptr, size_t size,
    int (*opt_unexpected_tag_decoder)(void *struct_ptr, const void *chunk_buf,
                                      size_t chunk_size),
    ssize_t (*body_receiver)(void *struct_ptr, const void *chunk_buf,
                             size_t chunk_size, int have_more)) {
    OCTET_STRING_t *st = (OCTET_STRING_t *)*sptr;
	const asn_OCTET_STRING_specifics_t *specs = td->specifics
				? (const asn_OCTET_STRING_specifics_t *)td->specifics
				: &asn_SPC_OCTET_STRING_specs;
	const char *xml_tag = opt_mname ? opt_mname : td->xml_tag;
	asn_struct_ctx_t *ctx;		/* Per-structure parser context */
	asn_dec_rval_t rval;		/* Return value from the decoder */
	int st_allocated;

	/*
	 * Create the string if does not exist.
	 */
	if(!st) {
		st = (OCTET_STRING_t *)CALLOC(1, specs->struct_size);
		*sptr = (void *)st;
		if(!st) goto sta_failed;
		st_allocated = 1;
	} else {
		st_allocated = 0;
	}
	if(!st->buf) {
		/* This is separate from above section */
		st->buf = (uint8_t *)CALLOC(1, 1);
		if(!st->buf) {
			if(st_allocated) {
				*sptr = 0;
				goto stb_failed;
			} else {
				goto sta_failed;
			}
		}
	}

	/* Restore parsing context */
	ctx = (asn_struct_ctx_t *)(((char *)*sptr) + specs->ctx_offset);

	return xer_decode_general(opt_codec_ctx, ctx, *sptr, xml_tag,
		buf_ptr, size, opt_unexpected_tag_decoder, body_receiver);

stb_failed:
	FREEMEM(st);
sta_failed:
	rval.code = RC_FAIL;
	rval.consumed = 0;
	return rval;
}

/*
 * Decode OCTET STRING from the hexadecimal data.
 */
asn_dec_rval_t
OCTET_STRING_decode_xer_hex(const asn_codec_ctx_t *opt_codec_ctx,
                            const asn_TYPE_descriptor_t *td, void **sptr,
                            const char *opt_mname, const void *buf_ptr,
                            size_t size) {
    return OCTET_STRING__decode_xer(opt_codec_ctx, td, sptr, opt_mname,
		buf_ptr, size, 0, OCTET_STRING__convert_hexadecimal);
}

/*
 * Decode OCTET STRING from the binary (0/1) data.
 */
asn_dec_rval_t
OCTET_STRING_decode_xer_binary(const asn_codec_ctx_t *opt_codec_ctx,
                               const asn_TYPE_descriptor_t *td, void **sptr,
                               const char *opt_mname, const void *buf_ptr,
                               size_t size) {
    return OCTET_STRING__decode_xer(opt_codec_ctx, td, sptr, opt_mname,
		buf_ptr, size, 0, OCTET_STRING__convert_binary);
}

/*
 * Decode OCTET STRING from the string (ASCII/UTF-8) data.
 */
asn_dec_rval_t
OCTET_STRING_decode_xer_utf8(const asn_codec_ctx_t *opt_codec_ctx,
                             const asn_TYPE_descriptor_t *td, void **sptr,
                             const char *opt_mname, const void *buf_ptr,
                             size_t size) {
    return OCTET_STRING__decode_xer(opt_codec_ctx, td, sptr, opt_mname,
		buf_ptr, size,
		OCTET_STRING__handle_control_chars,
		OCTET_STRING__convert_entrefs);
}

#ifndef  ASN_DISABLE_PER_SUPPORT

static int
OCTET_STRING_per_get_characters(asn_per_data_t *po, uint8_t *buf,
		size_t units, unsigned int bpc, unsigned int unit_bits,
		long lb, long ub, const asn_per_constraints_t *pc) {
	uint8_t *end = buf + units * bpc;

	ASN_DEBUG("Expanding %d characters into (%ld..%ld):%d",
		(int)units, lb, ub, unit_bits);

	/* X.691: 27.5.4 */
	if((unsigned long)ub <= ((unsigned long)2 << (unit_bits - 1))) {
		/* Decode without translation */
		lb = 0;
	} else if(pc && pc->code2value) {
		if(unit_bits > 16)
			return 1;	/* FATAL: can't have constrained
					 * UniversalString with more than
					 * 16 million code points */
		for(; buf < end; buf += bpc) {
			int value;
			int code = per_get_few_bits(po, unit_bits);
			if(code < 0) return -1;	/* WMORE */
			value = pc->code2value(code);
			if(value < 0) {
				ASN_DEBUG("Code %d (0x%02x) is"
					" not in map (%ld..%ld)",
					code, code, lb, ub);
				return 1;	/* FATAL */
			}
			switch(bpc) {
			case 1: *buf = value; break;
			case 2: buf[0] = value >> 8; buf[1] = value; break;
			case 4: buf[0] = value >> 24; buf[1] = value >> 16;
				buf[2] = value >> 8; buf[3] = value; break;
			}
		}
		return 0;
	}

	/* Shortcut the no-op copying to the aligned structure */
	if(lb == 0 && (unit_bits == 8 * bpc)) {
		return per_get_many_bits(po, buf, 0, unit_bits * units);
	}

	for(; buf < end; buf += bpc) {
		int32_t code = per_get_few_bits(po, unit_bits);
		int32_t ch = code + lb;
		if(code < 0) return -1;	/* WMORE */
		if(ch > ub) {
			ASN_DEBUG("Code %d is out of range (%ld..%ld)",
				ch, lb, ub);
			return 1;	/* FATAL */
		}
		switch(bpc) {
		case 1: *buf = ch; break;
		case 2: buf[0] = ch >> 8; buf[1] = ch; break;
		case 4: buf[0] = ch >> 24; buf[1] = ch >> 16;
			buf[2] = ch >> 8; buf[3] = ch; break;
		}
	}

	return 0;
}

static int
OCTET_STRING_per_put_characters(asn_per_outp_t *po, const uint8_t *buf,
		size_t units, unsigned int bpc, unsigned int unit_bits,
		long lb, long ub, const asn_per_constraints_t *pc) {
	const uint8_t *end = buf + units * bpc;

	ASN_DEBUG("Squeezing %d characters into (%ld..%ld):%d (%d bpc)",
		(int)units, lb, ub, unit_bits, bpc);

	/* X.691: 27.5.4 */
	if((unsigned long)ub <= ((unsigned long)2 << (unit_bits - 1))) {
		/* Encode as is */
		lb = 0;
	} else if(pc && pc->value2code) {
		for(; buf < end; buf += bpc) {
			int code;
			uint32_t value;
			switch(bpc) {
			case 1: value = *(const uint8_t *)buf; break;
			case 2: value = (buf[0] << 8) | buf[1]; break;
			case 4: value = (buf[0] << 24) | (buf[1] << 16)
					| (buf[2] << 8) | buf[3]; break;
			default: return -1;
			}
			code = pc->value2code(value);
			if(code < 0) {
				ASN_DEBUG("Character %d (0x%02x) is"
					" not in map (%ld..%ld)",
					*buf, *buf, lb, ub);
				return -1;
			}
			if(per_put_few_bits(po, code, unit_bits))
				return -1;
		}
	}

	/* Shortcut the no-op copying to the aligned structure */
	if(lb == 0 && (unit_bits == 8 * bpc)) {
		return per_put_many_bits(po, buf, unit_bits * units);
	}

    for(ub -= lb; buf < end; buf += bpc) {
        int ch;
        uint32_t value;
        switch(bpc) {
        case 1:
            value = *(const uint8_t *)buf;
            break;
        case 2:
            value = (buf[0] << 8) | buf[1];
            break;
        case 4:
            value = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
            break;
        default:
            return -1;
        }
        ch = value - lb;
        if(ch < 0 || ch > ub) {
            ASN_DEBUG("Character %d (0x%02x) is out of range (%ld..%ld)", *buf,
                      value, lb, ub + lb);
            return -1;
        }
        if(per_put_few_bits(po, ch, unit_bits)) return -1;
    }

    return 0;
}

static asn_per_constraints_t asn_DEF_OCTET_STRING_constraints = {
	{ APC_CONSTRAINED, 8, 8, 0, 255 },
	{ APC_SEMI_CONSTRAINED, -1, -1, 0, 0 },
	0, 0
};

asn_dec_rval_t
OCTET_STRING_decode_uper(const asn_codec_ctx_t *opt_codec_ctx,
                         const asn_TYPE_descriptor_t *td,
                         const asn_per_constraints_t *constraints, void **sptr,
                         asn_per_data_t *pd) {
    const asn_OCTET_STRING_specifics_t *specs = td->specifics
		? (const asn_OCTET_STRING_specifics_t *)td->specifics
		: &asn_SPC_OCTET_STRING_specs;
    const asn_per_constraints_t *pc =
        constraints ? constraints : td->encoding_constraints.per_constraints;
    const asn_per_constraint_t *cval;
	const asn_per_constraint_t *csiz;
	asn_dec_rval_t rval = { RC_OK, 0 };
	OCTET_STRING_t *st = (OCTET_STRING_t *)*sptr;
	ssize_t consumed_myself = 0;
	int repeat;
	enum {
		OS__BPC_CHAR	= 1,
		OS__BPC_U16	= 2,
		OS__BPC_U32	= 4
	} bpc;	/* Bytes per character */
	unsigned int unit_bits;
	unsigned int canonical_unit_bits;

	(void)opt_codec_ctx;

	if(pc) {
		cval = &pc->value;
		csiz = &pc->size;
	} else {
		cval = &asn_DEF_OCTET_STRING_constraints.value;
		csiz = &asn_DEF_OCTET_STRING_constraints.size;
	}

	switch(specs->subvariant) {
	default:
	case ASN_OSUBV_ANY:
	case ASN_OSUBV_BIT:
		ASN_DEBUG("Unrecognized subvariant %d", specs->subvariant);
		RETURN(RC_FAIL);
		break;
	case ASN_OSUBV_STR:
		canonical_unit_bits = unit_bits = 8;
		if(cval->flags & APC_CONSTRAINED)
			unit_bits = cval->range_bits;
		bpc = OS__BPC_CHAR;
		break;
	case ASN_OSUBV_U16:
		canonical_unit_bits = unit_bits = 16;
		if(cval->flags & APC_CONSTRAINED)
			unit_bits = cval->range_bits;
		bpc = OS__BPC_U16;
		break;
	case ASN_OSUBV_U32:
		canonical_unit_bits = unit_bits = 32;
		if(cval->flags & APC_CONSTRAINED)
			unit_bits = cval->range_bits;
		bpc = OS__BPC_U32;
		break;
	}

	/*
	 * Allocate the string.
	 */
	if(!st) {
		st = (OCTET_STRING_t *)(*sptr = CALLOC(1, specs->struct_size));
		if(!st) RETURN(RC_FAIL);
	}

	ASN_DEBUG("PER Decoding %s size %ld .. %ld bits %d",
		csiz->flags & APC_EXTENSIBLE ? "extensible" : "non-extensible",
		csiz->lower_bound, csiz->upper_bound, csiz->effective_bits);

	if(csiz->flags & APC_EXTENSIBLE) {
		int inext = per_get_few_bits(pd, 1);
		if(inext < 0) RETURN(RC_WMORE);
		if(inext) {
			csiz = &asn_DEF_OCTET_STRING_constraints.size;
			unit_bits = canonical_unit_bits;
		}
	}

	if(csiz->effective_bits >= 0) {
		FREEMEM(st->buf);
		if(bpc) {
			st->size = csiz->upper_bound * bpc;
		} else {
			st->size = (csiz->upper_bound + 7) >> 3;
		}
		st->buf = (uint8_t *)MALLOC(st->size + 1);
		if(!st->buf) { st->size = 0; RETURN(RC_FAIL); }
	}

	/* X.691, #16.5: zero-length encoding */
	/* X.691, #16.6: short fixed length encoding (up to 2 octets) */
	/* X.691, #16.7: long fixed length encoding (up to 64K octets) */
	if(csiz->effective_bits == 0) {
		int ret;
		if(bpc) {
			ASN_DEBUG("Encoding OCTET STRING size %ld",
				csiz->upper_bound);
			ret = OCTET_STRING_per_get_characters(pd, st->buf,
				csiz->upper_bound, bpc, unit_bits,
				cval->lower_bound, cval->upper_bound, pc);
			if(ret > 0) RETURN(RC_FAIL);
		} else {
			ASN_DEBUG("Encoding BIT STRING size %ld",
				csiz->upper_bound);
			ret = per_get_many_bits(pd, st->buf, 0,
					    unit_bits * csiz->upper_bound);
		}
		if(ret < 0) RETURN(RC_WMORE);
		consumed_myself += unit_bits * csiz->upper_bound;
		st->buf[st->size] = 0;
		RETURN(RC_OK);
	}

	st->size = 0;
	do {
		ssize_t raw_len;
		ssize_t len_bytes;
		void *p;
		int ret;

		/* Get the PER length */
		raw_len = uper_get_length(pd, csiz->effective_bits, csiz->lower_bound,
		                          &repeat);
		if(raw_len < 0) RETURN(RC_WMORE);
		if(raw_len == 0 && st->buf) break;

		ASN_DEBUG("Got PER length eb %ld, len %ld, %s (%s)",
			(long)csiz->effective_bits, (long)raw_len,
			repeat ? "repeat" : "once", td->name);
        len_bytes = raw_len * bpc;
		p = REALLOC(st->buf, st->size + len_bytes + 1);
		if(!p) RETURN(RC_FAIL);
		st->buf = (uint8_t *)p;

        ret = OCTET_STRING_per_get_characters(pd, &st->buf[st->size], raw_len,
                                              bpc, unit_bits, cval->lower_bound,
                                              cval->upper_bound, pc);
        if(ret > 0) RETURN(RC_FAIL);
		if(ret < 0) RETURN(RC_WMORE);
		st->size += len_bytes;
	} while(repeat);
	st->buf[st->size] = 0;	/* nul-terminate */

	return rval;
}

asn_enc_rval_t
OCTET_STRING_encode_uper(const asn_TYPE_descriptor_t *td,
                         const asn_per_constraints_t *constraints,
                         const void *sptr, asn_per_outp_t *po) {
    const asn_OCTET_STRING_specifics_t *specs = td->specifics
		? (const asn_OCTET_STRING_specifics_t *)td->specifics
		: &asn_SPC_OCTET_STRING_specs;
	const asn_per_constraints_t *pc = constraints ? constraints
				: td->encoding_constraints.per_constraints;
	const asn_per_constraint_t *cval;
	const asn_per_constraint_t *csiz;
	const OCTET_STRING_t *st = (const OCTET_STRING_t *)sptr;
	asn_enc_rval_t er = { 0, 0, 0 };
	int inext = 0;		/* Lies not within extension root */
	unsigned int unit_bits;
	unsigned int canonical_unit_bits;
	size_t size_in_units;
	const uint8_t *buf;
	int ret;
	enum {
		OS__BPC_CHAR	= 1,
		OS__BPC_U16	= 2,
		OS__BPC_U32	= 4
	} bpc;	/* Bytes per character */
	int ct_extensible;

	if(!st || (!st->buf && st->size))
		ASN__ENCODE_FAILED;

	if(pc) {
		cval = &pc->value;
		csiz = &pc->size;
	} else {
		cval = &asn_DEF_OCTET_STRING_constraints.value;
		csiz = &asn_DEF_OCTET_STRING_constraints.size;
	}
	ct_extensible = csiz->flags & APC_EXTENSIBLE;

	switch(specs->subvariant) {
	default:
	case ASN_OSUBV_ANY:
	case ASN_OSUBV_BIT:
		ASN__ENCODE_FAILED;
	case ASN_OSUBV_STR:
		canonical_unit_bits = unit_bits = 8;
		if(cval->flags & APC_CONSTRAINED)
			unit_bits = cval->range_bits;
		bpc = OS__BPC_CHAR;
		size_in_units = st->size;
		break;
	case ASN_OSUBV_U16:
		canonical_unit_bits = unit_bits = 16;
		if(cval->flags & APC_CONSTRAINED)
			unit_bits = cval->range_bits;
		bpc = OS__BPC_U16;
		size_in_units = st->size >> 1;
		if(st->size & 1) {
			ASN_DEBUG("%s string size is not modulo 2", td->name);
			ASN__ENCODE_FAILED;
		}
		break;
	case ASN_OSUBV_U32:
		canonical_unit_bits = unit_bits = 32;
		if(cval->flags & APC_CONSTRAINED)
			unit_bits = cval->range_bits;
		bpc = OS__BPC_U32;
		size_in_units = st->size >> 2;
		if(st->size & 3) {
			ASN_DEBUG("%s string size is not modulo 4", td->name);
			ASN__ENCODE_FAILED;
		}
		break;
	}

	ASN_DEBUG("Encoding %s into %" ASN_PRI_SIZE " units of %d bits"
		" (%ld..%ld, effective %d)%s",
		td->name, size_in_units, unit_bits,
		csiz->lower_bound, csiz->upper_bound,
		csiz->effective_bits, ct_extensible ? " EXT" : "");

	/* Figure out whether size lies within PER visible constraint */

    if(csiz->effective_bits >= 0) {
        if((ssize_t)size_in_units < csiz->lower_bound
           || (ssize_t)size_in_units > csiz->upper_bound) {
            if(ct_extensible) {
                csiz = &asn_DEF_OCTET_STRING_constraints.size;
                unit_bits = canonical_unit_bits;
                inext = 1;
            } else {
                ASN__ENCODE_FAILED;
            }
        }
    } else {
        inext = 0;
    }

    if(ct_extensible) {
		/* Declare whether length is [not] within extension root */
		if(per_put_few_bits(po, inext, 1))
			ASN__ENCODE_FAILED;
	}

    if(csiz->effective_bits >= 0 && !inext) {
        ASN_DEBUG("Encoding %" ASN_PRI_SIZE " bytes (%ld), length in %d bits", st->size,
                  size_in_units - csiz->lower_bound, csiz->effective_bits);
        ret = per_put_few_bits(po, size_in_units - csiz->lower_bound,
                               csiz->effective_bits);
        if(ret) ASN__ENCODE_FAILED;
        ret = OCTET_STRING_per_put_characters(po, st->buf, size_in_units, bpc,
                                              unit_bits, cval->lower_bound,
                                              cval->upper_bound, pc);
        if(ret) ASN__ENCODE_FAILED;
        ASN__ENCODED_OK(er);
    }

    ASN_DEBUG("Encoding %" ASN_PRI_SIZE " bytes", st->size);

    buf = st->buf;
    ASN_DEBUG("Encoding %" ASN_PRI_SIZE " in units", size_in_units);
    do {
        int need_eom = 0;
        ssize_t may_save = uper_put_length(po, size_in_units, &need_eom);
        if(may_save < 0) ASN__ENCODE_FAILED;

        ASN_DEBUG("Encoding %" ASN_PRI_SSIZE " of %" ASN_PRI_SIZE "%s", may_save, size_in_units,
                  need_eom ? ",+EOM" : "");

        ret = OCTET_STRING_per_put_characters(po, buf, may_save, bpc, unit_bits,
                                              cval->lower_bound,
                                              cval->upper_bound, pc);
        if(ret) ASN__ENCODE_FAILED;

        buf += may_save * bpc;
        size_in_units -= may_save;
        assert(!(may_save & 0x07) || !size_in_units);
        if(need_eom && uper_put_length(po, 0, 0))
            ASN__ENCODE_FAILED; /* End of Message length */
    } while(size_in_units);

    ASN__ENCODED_OK(er);
}

#endif  /* ASN_DISABLE_PER_SUPPORT */

int
OCTET_STRING_print(const asn_TYPE_descriptor_t *td, const void *sptr,
                   int ilevel, asn_app_consume_bytes_f *cb, void *app_key) {
    const char * const h2c = "0123456789ABCDEF";
	const OCTET_STRING_t *st = (const OCTET_STRING_t *)sptr;
	char scratch[16 * 3 + 4];
	char *p = scratch;
	uint8_t *buf;
	uint8_t *end;
	size_t i;

	(void)td;	/* Unused argument */

	if(!st || (!st->buf && st->size))
		return (cb("<absent>", 8, app_key) < 0) ? -1 : 0;

	/*
	 * Dump the contents of the buffer in hexadecimal.
	 */
	buf = st->buf;
	end = buf + st->size;
	for(i = 0; buf < end; buf++, i++) {
		if(!(i % 16) && (i || st->size > 16)) {
			if(cb(scratch, p - scratch, app_key) < 0)
				return -1;
			_i_INDENT(1);
			p = scratch;
		}
		*p++ = h2c[(*buf >> 4) & 0x0F];
		*p++ = h2c[*buf & 0x0F];
		*p++ = 0x20;
	}

	if(p > scratch) {
		p--;	/* Remove the tail space */
		if(cb(scratch, p - scratch, app_key) < 0)
			return -1;
	}

	return 0;
}

int
OCTET_STRING_print_utf8(const asn_TYPE_descriptor_t *td, const void *sptr,
                        int ilevel, asn_app_consume_bytes_f *cb,
                        void *app_key) {
    const OCTET_STRING_t *st = (const OCTET_STRING_t *)sptr;

	(void)td;	/* Unused argument */
	(void)ilevel;	/* Unused argument */

	if(st && (st->buf || !st->size)) {
		return (cb(st->buf, st->size, app_key) < 0) ? -1 : 0;
	} else {
		return (cb("<absent>", 8, app_key) < 0) ? -1 : 0;
	}
}

void
OCTET_STRING_free(const asn_TYPE_descriptor_t *td, void *sptr,
                  enum asn_struct_free_method method) {
	OCTET_STRING_t *st = (OCTET_STRING_t *)sptr;
	const asn_OCTET_STRING_specifics_t *specs;
	asn_struct_ctx_t *ctx;
	struct _stack *stck;

	if(!td || !st)
		return;

	specs = td->specifics
		    ? (const asn_OCTET_STRING_specifics_t *)td->specifics
		    : &asn_SPC_OCTET_STRING_specs;
	ctx = (asn_struct_ctx_t *)((char *)st + specs->ctx_offset);

	ASN_DEBUG("Freeing %s as OCTET STRING", td->name);

	if(st->buf) {
		FREEMEM(st->buf);
		st->buf = 0;
	}

	/*
	 * Remove decode-time stack.
	 */
	stck = (struct _stack *)ctx->ptr;
	if(stck) {
		while(stck->tail) {
			struct _stack_el *sel = stck->tail;
			stck->tail = sel->prev;
			FREEMEM(sel);
		}
		FREEMEM(stck);
	}

    switch(method) {
    case ASFM_FREE_EVERYTHING:
        FREEMEM(sptr);
        break;
    case ASFM_FREE_UNDERLYING:
        break;
    case ASFM_FREE_UNDERLYING_AND_RESET:
        memset(sptr, 0,
               td->specifics
                   ? ((const asn_OCTET_STRING_specifics_t *)(td->specifics))
                         ->struct_size
                   : sizeof(OCTET_STRING_t));
        break;
    }
}

/*
 * Conversion routines.
 */
int
OCTET_STRING_fromBuf(OCTET_STRING_t *st, const char *str, int len) {
	void *buf;

	if(st == 0 || (str == 0 && len)) {
		errno = EINVAL;
		return -1;
	}

	/*
	 * Clear the OCTET STRING.
	 */
	if(str == NULL) {
		FREEMEM(st->buf);
		st->buf = 0;
		st->size = 0;
		return 0;
	}

	/* Determine the original string size, if not explicitly given */
	if(len < 0)
		len = strlen(str);

	/* Allocate and fill the memory */
	buf = MALLOC(len + 1);
	if(buf == NULL)
		return -1;

	memcpy(buf, str, len);
	((uint8_t *)buf)[len] = '\0';	/* Couldn't use memcpy(len+1)! */
	FREEMEM(st->buf);
	st->buf = (uint8_t *)buf;
	st->size = len;

	return 0;
}

OCTET_STRING_t *
OCTET_STRING_new_fromBuf(const asn_TYPE_descriptor_t *td, const char *str,
                         int len) {
    const asn_OCTET_STRING_specifics_t *specs =
        td->specifics ? (const asn_OCTET_STRING_specifics_t *)td->specifics
                      : &asn_SPC_OCTET_STRING_specs;
    OCTET_STRING_t *st;

	st = (OCTET_STRING_t *)CALLOC(1, specs->struct_size);
	if(st && str && OCTET_STRING_fromBuf(st, str, len)) {
		FREEMEM(st);
		st = NULL;
	}

	return st;
}

/*
 * Lexicographically compare the common prefix of both strings,
 * and if it is the same return -1 for the smallest string.
 */
int
OCTET_STRING_compare(const asn_TYPE_descriptor_t *td, const void *aptr,
                     const void *bptr) {
    const asn_OCTET_STRING_specifics_t *specs = td->specifics;
    const OCTET_STRING_t *a = aptr;
    const OCTET_STRING_t *b = bptr;

    assert(!specs || specs->subvariant != ASN_OSUBV_BIT);

    if(a && b) {
        size_t common_prefix_size = a->size <= b->size ? a->size : b->size;
        int ret = memcmp(a->buf, b->buf, common_prefix_size);
        if(ret == 0) {
            /* Figure out which string with equal prefixes is longer. */
            if(a->size < b->size) {
                return -1;
            } else if(a->size > b->size) {
                return 1;
            } else {
                return 0;
            }
        } else {
            return ret < 0 ? -1 : 1;
        }
    } else if(!a && !b) {
        return 0;
    } else if(!a) {
        return -1;
    } else {
        return 1;
    }

}

/*
 * Biased function for randomizing character values around their limits.
 */
static uint32_t
OCTET_STRING__random_char(unsigned long lb, unsigned long ub) {
    assert(lb <= ub);
    switch(asn_random_between(0, 16)) {
    case 0:
        if(lb < ub) return lb + 1;
        /* Fall through */
    case 1:
        return lb;
    case 2:
        if(lb < ub) return ub - 1;
        /* Fall through */
    case 3:
        return ub;
    default:
        return asn_random_between(lb, ub);
    }
}


size_t
OCTET_STRING_random_length_constrained(
    const asn_TYPE_descriptor_t *td,
    const asn_encoding_constraints_t *constraints, size_t max_length) {
    const unsigned lengths[] = {0,     1,     2,     3,     4,     8,
                                126,   127,   128,   16383, 16384, 16385,
                                65534, 65535, 65536, 65537};
    size_t rnd_len;

    /* Figure out how far we should go */
    rnd_len = lengths[asn_random_between(
        0, sizeof(lengths) / sizeof(lengths[0]) - 1)];

    if(!constraints || !constraints->per_constraints)
        constraints = &td->encoding_constraints;
    if(constraints->per_constraints) {
        const asn_per_constraint_t *pc = &constraints->per_constraints->size;
        if(pc->flags & APC_CONSTRAINED) {
            long suggested_upper_bound = pc->upper_bound < (ssize_t)max_length
                                             ? pc->upper_bound
                                             : (ssize_t)max_length;
            if(max_length <= (size_t)pc->lower_bound) {
                return pc->lower_bound;
            }
            if(pc->flags & APC_EXTENSIBLE) {
                switch(asn_random_between(0, 5)) {
                case 0:
                    if(pc->lower_bound > 0) {
                        rnd_len = pc->lower_bound - 1;
                        break;
                    }
                    /* Fall through */
                case 1:
                    rnd_len = pc->upper_bound + 1;
                    break;
                case 2:
                    /* Keep rnd_len from the table */
                    if(rnd_len <= max_length) {
                        break;
                    }
                    /* Fall through */
                default:
                    rnd_len = asn_random_between(pc->lower_bound,
                                                 suggested_upper_bound);
                }
            } else {
                rnd_len =
                    asn_random_between(pc->lower_bound, suggested_upper_bound);
            }
        } else {
            rnd_len = asn_random_between(0, max_length);
        }
    } else if(rnd_len > max_length) {
        rnd_len = asn_random_between(0, max_length);
    }

    return rnd_len;
}

asn_random_fill_result_t
OCTET_STRING_random_fill(const asn_TYPE_descriptor_t *td, void **sptr,
                         const asn_encoding_constraints_t *constraints,
                         size_t max_length) {
	const asn_OCTET_STRING_specifics_t *specs = td->specifics
				? (const asn_OCTET_STRING_specifics_t *)td->specifics
				: &asn_SPC_OCTET_STRING_specs;
    asn_random_fill_result_t result_ok = {ARFILL_OK, 1};
    asn_random_fill_result_t result_failed = {ARFILL_FAILED, 0};
    asn_random_fill_result_t result_skipped = {ARFILL_SKIPPED, 0};
    unsigned int unit_bytes = 1;
    unsigned long clb = 0;  /* Lower bound on char */
    unsigned long cub = 255;  /* Higher bound on char value */
    uint8_t *buf;
    uint8_t *bend;
    uint8_t *b;
    size_t rnd_len;
    OCTET_STRING_t *st;

    if(max_length == 0 && !*sptr) return result_skipped;

    switch(specs->subvariant) {
    default:
    case ASN_OSUBV_ANY:
        return result_failed;
    case ASN_OSUBV_BIT:
        /* Handled by BIT_STRING itself. */
        return result_failed;
    case ASN_OSUBV_STR:
        unit_bytes = 1;
        clb = 0;
        cub = 255;
        break;
    case ASN_OSUBV_U16:
        unit_bytes = 2;
        clb = 0;
        cub = 65535;
        break;
    case ASN_OSUBV_U32:
        unit_bytes = 4;
        clb = 0;
        cub = 0x10FFFF;
        break;
    }

    if(!constraints || !constraints->per_constraints)
        constraints = &td->encoding_constraints;
    if(constraints->per_constraints) {
        const asn_per_constraint_t *pc = &constraints->per_constraints->value;
        if(pc->flags & APC_SEMI_CONSTRAINED) {
            clb = pc->lower_bound;
        } else if(pc->flags & APC_CONSTRAINED) {
            clb = pc->lower_bound;
            cub = pc->upper_bound;
        }
    }

    rnd_len =
        OCTET_STRING_random_length_constrained(td, constraints, max_length);

    buf = CALLOC(unit_bytes, rnd_len + 1);
    if(!buf) return result_failed;

    bend = &buf[unit_bytes * rnd_len];

    switch(unit_bytes) {
    case 1:
        for(b = buf; b < bend; b += unit_bytes) {
            *(uint8_t *)b = OCTET_STRING__random_char(clb, cub);
        }
        *(uint8_t *)b = 0;
        break;
    case 2:
        for(b = buf; b < bend; b += unit_bytes) {
            uint32_t code = OCTET_STRING__random_char(clb, cub);
            b[0] = code >> 8;
            b[1] = code;
        }
        *(uint16_t *)b = 0;
        break;
    case 4:
        for(b = buf; b < bend; b += unit_bytes) {
            uint32_t code = OCTET_STRING__random_char(clb, cub);
            b[0] = code >> 24;
            b[1] = code >> 16;
            b[2] = code >> 8;
            b[3] = code;
        }
        *(uint32_t *)b = 0;
        break;
    }

    if(*sptr) {
        st = *sptr;
        FREEMEM(st->buf);
    } else {
        st = (OCTET_STRING_t *)(*sptr = CALLOC(1, specs->struct_size));
        if(!st) {
            FREEMEM(buf);
            return result_failed;
        }
    }

    st->buf = buf;
    st->size = unit_bytes * rnd_len;

    result_ok.length = st->size;
    return result_ok;
}
