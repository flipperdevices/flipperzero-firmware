/*-
 * Copyright (c) 2003, 2004 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
#include <asn_internal.h>
#include <BIT_STRING.h>
#include <asn_internal.h>

/*
 * BIT STRING basic type description.
 */
static const ber_tlv_tag_t asn_DEF_BIT_STRING_tags[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (3 << 2))
};
asn_OCTET_STRING_specifics_t asn_SPC_BIT_STRING_specs = {
	sizeof(BIT_STRING_t),
	offsetof(BIT_STRING_t, _asn_ctx),
	ASN_OSUBV_BIT
};
asn_TYPE_operation_t asn_OP_BIT_STRING = {
	OCTET_STRING_free,         /* Implemented in terms of OCTET STRING */
	BIT_STRING_print,
	BIT_STRING_compare,
	OCTET_STRING_decode_ber,   /* Implemented in terms of OCTET STRING */
	OCTET_STRING_encode_der,   /* Implemented in terms of OCTET STRING */
	OCTET_STRING_decode_xer_binary,
	BIT_STRING_encode_xer,
#ifdef	ASN_DISABLE_OER_SUPPORT
	0,
	0,
#else
	BIT_STRING_decode_oer,
	BIT_STRING_encode_oer,
#endif  /* ASN_DISABLE_OER_SUPPORT */
#ifdef	ASN_DISABLE_PER_SUPPORT
	0,
	0,
#else
	BIT_STRING_decode_uper,	/* Unaligned PER decoder */
	BIT_STRING_encode_uper,	/* Unaligned PER encoder */
#endif  /* ASN_DISABLE_PER_SUPPORT */
	BIT_STRING_random_fill,
	0	/* Use generic outmost tag fetcher */
};
asn_TYPE_descriptor_t asn_DEF_BIT_STRING = {
	"BIT STRING",
	"BIT_STRING",
	&asn_OP_BIT_STRING,
	asn_DEF_BIT_STRING_tags,
	sizeof(asn_DEF_BIT_STRING_tags)
	  / sizeof(asn_DEF_BIT_STRING_tags[0]),
	asn_DEF_BIT_STRING_tags,	/* Same as above */
	sizeof(asn_DEF_BIT_STRING_tags)
	  / sizeof(asn_DEF_BIT_STRING_tags[0]),
	{ 0, 0, BIT_STRING_constraint },
	0, 0,	/* No members */
	&asn_SPC_BIT_STRING_specs
};

/*
 * BIT STRING generic constraint.
 */
int
BIT_STRING_constraint(const asn_TYPE_descriptor_t *td, const void *sptr,
                      asn_app_constraint_failed_f *ctfailcb, void *app_key) {
    const BIT_STRING_t *st = (const BIT_STRING_t *)sptr;

	if(st && st->buf) {
		if((st->size == 0 && st->bits_unused)
		|| st->bits_unused < 0 || st->bits_unused > 7) {
			ASN__CTFAIL(app_key, td, sptr,
				"%s: invalid padding byte (%s:%d)",
				td->name, __FILE__, __LINE__);
			return -1;
		}
	} else {
		ASN__CTFAIL(app_key, td, sptr,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}

	return 0;
}

static const char *_bit_pattern[16] = {
	"0000", "0001", "0010", "0011", "0100", "0101", "0110", "0111",
	"1000", "1001", "1010", "1011", "1100", "1101", "1110", "1111"
};

asn_enc_rval_t
BIT_STRING_encode_xer(const asn_TYPE_descriptor_t *td, const void *sptr,
                      int ilevel, enum xer_encoder_flags_e flags,
                      asn_app_consume_bytes_f *cb, void *app_key) {
    asn_enc_rval_t er;
	char scratch[128];
	char *p = scratch;
	char *scend = scratch + (sizeof(scratch) - 10);
	const BIT_STRING_t *st = (const BIT_STRING_t *)sptr;
	int xcan = (flags & XER_F_CANONICAL);
	uint8_t *buf;
	uint8_t *end;

	if(!st || !st->buf)
		ASN__ENCODE_FAILED;

	er.encoded = 0;

	buf = st->buf;
	end = buf + st->size - 1;	/* Last byte is special */

	/*
	 * Binary dump
	 */
	for(; buf < end; buf++) {
		int v = *buf;
		int nline = xcan?0:(((buf - st->buf) % 8) == 0);
		if(p >= scend || nline) {
			ASN__CALLBACK(scratch, p - scratch);
			p = scratch;
			if(nline) ASN__TEXT_INDENT(1, ilevel);
		}
		memcpy(p + 0, _bit_pattern[v >> 4], 4);
		memcpy(p + 4, _bit_pattern[v & 0x0f], 4);
		p += 8;
	}

	if(!xcan && ((buf - st->buf) % 8) == 0)
		ASN__TEXT_INDENT(1, ilevel);
	ASN__CALLBACK(scratch, p - scratch);
	p = scratch;

	if(buf == end) {
		int v = *buf;
		int ubits = st->bits_unused;
		int i;
		for(i = 7; i >= ubits; i--)
			*p++ = (v & (1 << i)) ? 0x31 : 0x30;
		ASN__CALLBACK(scratch, p - scratch);
	}

	if(!xcan) ASN__TEXT_INDENT(1, ilevel - 1);

	ASN__ENCODED_OK(er);
cb_failed:
	ASN__ENCODE_FAILED;
}


/*
 * BIT STRING specific contents printer.
 */
int
BIT_STRING_print(const asn_TYPE_descriptor_t *td, const void *sptr, int ilevel,
                 asn_app_consume_bytes_f *cb, void *app_key) {
    const char * const h2c = "0123456789ABCDEF";
	char scratch[64];
	const BIT_STRING_t *st = (const BIT_STRING_t *)sptr;
	uint8_t *buf;
	uint8_t *end;
	char *p = scratch;

	(void)td;	/* Unused argument */

	if(!st || !st->buf)
		return (cb("<absent>", 8, app_key) < 0) ? -1 : 0;

	ilevel++;
	buf = st->buf;
	end = buf + st->size;

	/*
	 * Hexadecimal dump.
	 */
	for(; buf < end; buf++) {
		if((buf - st->buf) % 16 == 0 && (st->size > 16)
				&& buf != st->buf) {
			_i_INDENT(1);
			/* Dump the string */
			if(cb(scratch, p - scratch, app_key) < 0) return -1;
			p = scratch;
		}
		*p++ = h2c[*buf >> 4];
		*p++ = h2c[*buf & 0x0F];
		*p++ = 0x20;
	}

	if(p > scratch) {
		p--;	/* Eat the tailing space */

		if((st->size > 16)) {
			_i_INDENT(1);
		}

		/* Dump the incomplete 16-bytes row */
		if(cb(scratch, p - scratch, app_key) < 0)
			return -1;
	}

    if(st->bits_unused) {
        int ret = snprintf(scratch, sizeof(scratch), " (%d bit%s unused)",
                           st->bits_unused, st->bits_unused == 1 ? "" : "s");
        assert(ret > 0 && ret < (ssize_t)sizeof(scratch));
        if(ret > 0 && ret < (ssize_t)sizeof(scratch)
           && cb(scratch, ret, app_key) < 0)
            return -1;
    }

	return 0;
}

/*
 * Non-destructively remove the trailing 0-bits from the given bit string.
 */
static const BIT_STRING_t *
BIT_STRING__compactify(const BIT_STRING_t *st, BIT_STRING_t *tmp) {
    const uint8_t *b;
    union {
        const uint8_t *c_buf;
        uint8_t *nc_buf;
    } unconst;

    if(st->size == 0) {
        assert(st->bits_unused == 0);
        return st;
    } else {
        for(b = &st->buf[st->size - 1]; b > st->buf && *b == 0; b--) {
            ;
        }
        /* b points to the last byte which may contain data */
        if(*b) {
            int unused = 7;
            uint8_t v = *b;
            v &= -(int8_t)v;
            if(v & 0x0F) unused -= 4;
            if(v & 0x33) unused -= 2;
            if(v & 0x55) unused -= 1;
            tmp->size = b-st->buf + 1;
            tmp->bits_unused = unused;
        } else {
            tmp->size = b-st->buf;
            tmp->bits_unused = 0;
        }

        assert(b >= st->buf);
    }

    unconst.c_buf = st->buf;
    tmp->buf = unconst.nc_buf;
    return tmp;
}

/*
 * Lexicographically compare the common prefix of both strings,
 * and if it is the same return -1 for the smallest string.
 */
int
BIT_STRING_compare(const asn_TYPE_descriptor_t *td, const void *aptr,
                   const void *bptr) {
    /*
     * Remove information about trailing bits, since
     * X.680 (08/2015) #22.7 "ensure that different semantics are not"
     * "associated with [values that differ only in] the trailing 0 bits."
     */
    BIT_STRING_t compact_a, compact_b;
    const BIT_STRING_t *a = BIT_STRING__compactify(aptr, &compact_a);
    const BIT_STRING_t *b = BIT_STRING__compactify(bptr, &compact_b);
    const asn_OCTET_STRING_specifics_t *specs = td->specifics;

    assert(specs && specs->subvariant == ASN_OSUBV_BIT);

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
                /* Figure out how many unused bits */
                if(a->bits_unused > b->bits_unused) {
                    return -1;
                } else if(a->bits_unused < b->bits_unused) {
                    return 1;
                } else {
                    return 0;
                }
            }
        } else {
            return ret;
        }
    } else if(!a && !b) {
        return 0;
    } else if(!a) {
        return -1;
    } else {
        return 1;
    }
}

#ifndef  ASN_DISABLE_PER_SUPPORT

#undef  RETURN
#define RETURN(_code)                       \
    do {                                    \
        asn_dec_rval_t tmprval;             \
        tmprval.code = _code;               \
        tmprval.consumed = consumed_myself; \
        return tmprval;                     \
    } while(0)

static asn_per_constraint_t asn_DEF_BIT_STRING_constraint_size = {
    APC_SEMI_CONSTRAINED, -1, -1, 0, 0};

asn_dec_rval_t
BIT_STRING_decode_uper(const asn_codec_ctx_t *opt_codec_ctx,
                       const asn_TYPE_descriptor_t *td,
                       const asn_per_constraints_t *constraints, void **sptr,
                       asn_per_data_t *pd) {
    const asn_OCTET_STRING_specifics_t *specs = td->specifics
		? (const asn_OCTET_STRING_specifics_t *)td->specifics
		: &asn_SPC_BIT_STRING_specs;
    const asn_per_constraints_t *pc =
        constraints ? constraints : td->encoding_constraints.per_constraints;
	const asn_per_constraint_t *csiz;
	asn_dec_rval_t rval = { RC_OK, 0 };
	BIT_STRING_t *st = (BIT_STRING_t *)*sptr;
	ssize_t consumed_myself = 0;
	int repeat;

	(void)opt_codec_ctx;

	if(pc) {
		csiz = &pc->size;
	} else {
		csiz = &asn_DEF_BIT_STRING_constraint_size;
	}

	if(specs->subvariant != ASN_OSUBV_BIT) {
		ASN_DEBUG("Subvariant %d is not BIT OSUBV_BIT", specs->subvariant);
		RETURN(RC_FAIL);
    }

	/*
	 * Allocate the string.
	 */
	if(!st) {
		st = (BIT_STRING_t *)(*sptr = CALLOC(1, specs->struct_size));
		if(!st) RETURN(RC_FAIL);
	}

	ASN_DEBUG("PER Decoding %s size %ld .. %ld bits %d",
		csiz->flags & APC_EXTENSIBLE ? "extensible" : "non-extensible",
		csiz->lower_bound, csiz->upper_bound, csiz->effective_bits);

	if(csiz->flags & APC_EXTENSIBLE) {
		int inext = per_get_few_bits(pd, 1);
		if(inext < 0) RETURN(RC_WMORE);
		if(inext) {
			csiz = &asn_DEF_BIT_STRING_constraint_size;
		}
	}

	if(csiz->effective_bits >= 0) {
		FREEMEM(st->buf);
        st->size = (csiz->upper_bound + 7) >> 3;
        st->buf = (uint8_t *)MALLOC(st->size + 1);
		if(!st->buf) { st->size = 0; RETURN(RC_FAIL); }
	}

	/* X.691, #16.5: zero-length encoding */
	/* X.691, #16.6: short fixed length encoding (up to 2 octets) */
	/* X.691, #16.7: long fixed length encoding (up to 64K octets) */
	if(csiz->effective_bits == 0) {
		int ret;
        ASN_DEBUG("Encoding BIT STRING size %ld", csiz->upper_bound);
        ret = per_get_many_bits(pd, st->buf, 0, csiz->upper_bound);
		if(ret < 0) RETURN(RC_WMORE);
		consumed_myself += csiz->upper_bound;
		st->buf[st->size] = 0;
        st->bits_unused = (8 - (csiz->upper_bound & 0x7)) & 0x7;
        RETURN(RC_OK);
	}

	st->size = 0;
	do {
		ssize_t raw_len;
		ssize_t len_bytes;
		ssize_t len_bits;
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
        len_bits = raw_len;
        len_bytes = (len_bits + 7) >> 3;
        if(len_bits & 0x7) st->bits_unused = 8 - (len_bits & 0x7);
        /* len_bits be multiple of 16K if repeat is set */
        p = REALLOC(st->buf, st->size + len_bytes + 1);
		if(!p) RETURN(RC_FAIL);
		st->buf = (uint8_t *)p;

        ret = per_get_many_bits(pd, &st->buf[st->size], 0, len_bits);
        if(ret < 0) RETURN(RC_WMORE);
		st->size += len_bytes;
	} while(repeat);
	st->buf[st->size] = 0;	/* nul-terminate */

	return rval;
}

asn_enc_rval_t
BIT_STRING_encode_uper(const asn_TYPE_descriptor_t *td,
                       const asn_per_constraints_t *constraints,
                       const void *sptr, asn_per_outp_t *po) {
    const asn_OCTET_STRING_specifics_t *specs =
        td->specifics ? (const asn_OCTET_STRING_specifics_t *)td->specifics
                      : &asn_SPC_BIT_STRING_specs;
    const asn_per_constraints_t *pc =
        constraints ? constraints : td->encoding_constraints.per_constraints;
	const asn_per_constraint_t *csiz;
	const BIT_STRING_t *st = (const BIT_STRING_t *)sptr;
	BIT_STRING_t compact_bstr;  /* Do not modify this directly! */
	asn_enc_rval_t er = { 0, 0, 0 };
	int inext = 0;		/* Lies not within extension root */
	size_t size_in_bits;
	const uint8_t *buf;
	int ret;
	int ct_extensible;

	if(!st || (!st->buf && st->size))
		ASN__ENCODE_FAILED;

	if(specs->subvariant == ASN_OSUBV_BIT) {
        if((st->size == 0 && st->bits_unused) || (st->bits_unused & ~7))
            ASN__ENCODE_FAILED;
    } else {
		ASN__ENCODE_FAILED;
    }

	if(pc) {
        csiz = &pc->size;
    } else {
		csiz = &asn_DEF_BIT_STRING_constraint_size;
	}
	ct_extensible = csiz->flags & APC_EXTENSIBLE;

    /* Figure out the size without the trailing bits */
    st = BIT_STRING__compactify(st, &compact_bstr);
    size_in_bits = 8 * st->size - st->bits_unused;

    ASN_DEBUG(
        "Encoding %s into %" ASN_PRI_SIZE " bits"
        " (%ld..%ld, effective %d)%s",
        td->name, size_in_bits, csiz->lower_bound, csiz->upper_bound,
        csiz->effective_bits, ct_extensible ? " EXT" : "");

    /* Figure out whether size lies within PER visible constraint */

    if(csiz->effective_bits >= 0) {
        if((ssize_t)size_in_bits > csiz->upper_bound) {
            if(ct_extensible) {
                csiz = &asn_DEF_BIT_STRING_constraint_size;
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
        int add_trailer = (ssize_t)size_in_bits < csiz->lower_bound;
        ASN_DEBUG(
            "Encoding %" ASN_PRI_SIZE " bytes (%ld), length (in %d bits) trailer %d; actual "
            "value %" ASN_PRI_SSIZE "",
            st->size, size_in_bits - csiz->lower_bound, csiz->effective_bits,
            add_trailer,
            add_trailer ? 0 : (ssize_t)size_in_bits - csiz->lower_bound);
        ret = per_put_few_bits(
            po, add_trailer ? 0 : (ssize_t)size_in_bits - csiz->lower_bound,
            csiz->effective_bits);
        if(ret) ASN__ENCODE_FAILED;
        ret = per_put_many_bits(po, st->buf, size_in_bits);
        if(ret) ASN__ENCODE_FAILED;
        if(add_trailer) {
            static const uint8_t zeros[16];
            size_t trailing_zero_bits = csiz->lower_bound - size_in_bits;
            while(trailing_zero_bits > 0) {
                if(trailing_zero_bits > 8 * sizeof(zeros)) {
                    ret = per_put_many_bits(po, zeros, 8 * sizeof(zeros));
                    trailing_zero_bits -= 8 * sizeof(zeros);
                } else {
                    ret = per_put_many_bits(po, zeros, trailing_zero_bits);
                    trailing_zero_bits = 0;
                }
                if(ret) ASN__ENCODE_FAILED;
            }
        }
        ASN__ENCODED_OK(er);
    }

    ASN_DEBUG("Encoding %" ASN_PRI_SIZE " bytes", st->size);

    buf = st->buf;
    do {
        int need_eom = 0;
        ssize_t maySave = uper_put_length(po, size_in_bits, &need_eom);
        if(maySave < 0) ASN__ENCODE_FAILED;

        ASN_DEBUG("Encoding %" ASN_PRI_SSIZE " of %" ASN_PRI_SIZE "", maySave, size_in_bits);

        ret = per_put_many_bits(po, buf, maySave);
        if(ret) ASN__ENCODE_FAILED;

        buf += maySave >> 3;
        size_in_bits -= maySave;
        assert(!(maySave & 0x07) || !size_in_bits);
        if(need_eom && uper_put_length(po, 0, 0))
            ASN__ENCODE_FAILED; /* End of Message length */
    } while(size_in_bits);

    ASN__ENCODED_OK(er);
}

#endif  /* ASN_DISABLE_PER_SUPPORT */

asn_random_fill_result_t
BIT_STRING_random_fill(const asn_TYPE_descriptor_t *td, void **sptr,
                       const asn_encoding_constraints_t *constraints,
                       size_t max_length) {
    const asn_OCTET_STRING_specifics_t *specs =
        td->specifics ? (const asn_OCTET_STRING_specifics_t *)td->specifics
                      : &asn_SPC_BIT_STRING_specs;
    asn_random_fill_result_t result_ok = {ARFILL_OK, 1};
    asn_random_fill_result_t result_failed = {ARFILL_FAILED, 0};
    asn_random_fill_result_t result_skipped = {ARFILL_SKIPPED, 0};
    static unsigned lengths[] = {0,     1,     2,     3,     4,     8,
                                 126,   127,   128,   16383, 16384, 16385,
                                 65534, 65535, 65536, 65537};
    uint8_t *buf;
    uint8_t *bend;
    uint8_t *b;
    size_t rnd_bits, rnd_len;
    BIT_STRING_t *st;

    if(max_length == 0) return result_skipped;

    switch(specs->subvariant) {
    case ASN_OSUBV_ANY:
        return result_failed;
    case ASN_OSUBV_BIT:
        break;
    default:
        break;
    }

    /* Figure out how far we should go */
    rnd_bits = lengths[asn_random_between(
        0, sizeof(lengths) / sizeof(lengths[0]) - 1)];
    if(!constraints || !constraints->per_constraints)
        constraints = &td->encoding_constraints;
    if(constraints->per_constraints) {
        const asn_per_constraint_t *pc = &constraints->per_constraints->size;
        if(pc->flags & APC_CONSTRAINED) {
            long suggested_upper_bound = pc->upper_bound < (ssize_t)max_length
                                             ? pc->upper_bound
                                             : (ssize_t)max_length;
            if(max_length < (size_t)pc->lower_bound) {
                return result_skipped;
            }
            if(pc->flags & APC_EXTENSIBLE) {
                switch(asn_random_between(0, 5)) {
                case 0:
                    if(pc->lower_bound > 0) {
                        rnd_bits = pc->lower_bound - 1;
                        break;
                    }
                    /* Fall through */
                case 1:
                    rnd_bits = pc->upper_bound + 1;
                    break;
                case 2:
                    /* Keep rnd_bits from the table */
                    if(rnd_bits < max_length) {
                        break;
                    }
                    /* Fall through */
                default:
                    rnd_bits = asn_random_between(pc->lower_bound,
                                                  suggested_upper_bound);
                }
            } else {
                rnd_bits =
                    asn_random_between(pc->lower_bound, suggested_upper_bound);
            }
        } else {
            rnd_bits = asn_random_between(0, max_length - 1);
        }
    } else if(rnd_bits >= max_length) {
        rnd_bits = asn_random_between(0, max_length - 1);
    }

    rnd_len = (rnd_bits + 7) / 8;
    buf = CALLOC(1, rnd_len + 1);
    if(!buf) return result_failed;

    bend = &buf[rnd_len];

    for(b = buf; b < bend; b++) {
        *(uint8_t *)b = asn_random_between(0, 255);
    }
    *b = 0; /* Zero-terminate just in case. */

    if(*sptr) {
        st = *sptr;
        FREEMEM(st->buf);
    } else {
        st = (BIT_STRING_t *)(*sptr = CALLOC(1, specs->struct_size));
        if(!st) {
            FREEMEM(buf);
            return result_failed;
        }
    }

    st->buf = buf;
    st->size = rnd_len;
    st->bits_unused = (8 - (rnd_bits & 0x7)) & 0x7;
    if(st->bits_unused) {
        assert(st->size > 0);
        st->buf[st->size-1] &= 0xff << st->bits_unused;
    }

    result_ok.length = st->size;
    return result_ok;
}
