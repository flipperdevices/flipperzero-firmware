/*-
 * Copyright (c) 2004, 2005, 2006 Lev Walkin <vlm@lionet.info>.
 * All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
/*
 * Read the NativeInteger.h for the explanation wrt. differences between
 * INTEGER and NativeInteger.
 * Basically, both are decoders and encoders of ASN.1 INTEGER type, but this
 * implementation deals with the standard (machine-specific) representation
 * of them instead of using the platform-independent buffer.
 */
#include <asn_internal.h>
#include <NativeInteger.h>

/*
 * NativeInteger basic type description.
 */
static const ber_tlv_tag_t asn_DEF_NativeInteger_tags[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (2 << 2))
};
asn_TYPE_operation_t asn_OP_NativeInteger = {
	NativeInteger_free,
	NativeInteger_print,
	NativeInteger_compare,
	NativeInteger_decode_ber,
	NativeInteger_encode_der,
	NativeInteger_decode_xer,
	NativeInteger_encode_xer,
#ifdef	ASN_DISABLE_OER_SUPPORT
	0,
	0,
#else
	NativeInteger_decode_oer,	/* OER decoder */
	NativeInteger_encode_oer,	/* Canonical OER encoder */
#endif  /* ASN_DISABLE_OER_SUPPORT */
#ifdef	ASN_DISABLE_PER_SUPPORT
	0,
	0,
#else
	NativeInteger_decode_uper,	/* Unaligned PER decoder */
	NativeInteger_encode_uper,	/* Unaligned PER encoder */
#endif	/* ASN_DISABLE_PER_SUPPORT */
	NativeInteger_random_fill,
	0	/* Use generic outmost tag fetcher */
};
asn_TYPE_descriptor_t asn_DEF_NativeInteger = {
	"INTEGER",			/* The ASN.1 type is still INTEGER */
	"INTEGER",
	&asn_OP_NativeInteger,
	asn_DEF_NativeInteger_tags,
	sizeof(asn_DEF_NativeInteger_tags) / sizeof(asn_DEF_NativeInteger_tags[0]),
	asn_DEF_NativeInteger_tags,	/* Same as above */
	sizeof(asn_DEF_NativeInteger_tags) / sizeof(asn_DEF_NativeInteger_tags[0]),
	{ 0, 0, asn_generic_no_constraint },
	0, 0,	/* No members */
	0	/* No specifics */
};

/*
 * Decode INTEGER type.
 */
asn_dec_rval_t
NativeInteger_decode_ber(const asn_codec_ctx_t *opt_codec_ctx,
                         const asn_TYPE_descriptor_t *td, void **nint_ptr,
                         const void *buf_ptr, size_t size, int tag_mode) {
    const asn_INTEGER_specifics_t *specs =
        (const asn_INTEGER_specifics_t *)td->specifics;
    long *native = (long *)*nint_ptr;
	asn_dec_rval_t rval;
	ber_tlv_len_t length;

	/*
	 * If the structure is not there, allocate it.
	 */
	if(native == NULL) {
		native = (long *)(*nint_ptr = CALLOC(1, sizeof(*native)));
		if(native == NULL) {
			rval.code = RC_FAIL;
			rval.consumed = 0;
			return rval;
		}
	}

	ASN_DEBUG("Decoding %s as INTEGER (tm=%d)",
		td->name, tag_mode);

	/*
	 * Check tags.
	 */
	rval = ber_check_tags(opt_codec_ctx, td, 0, buf_ptr, size,
			tag_mode, 0, &length, 0);
	if(rval.code != RC_OK)
		return rval;

	ASN_DEBUG("%s length is %d bytes", td->name, (int)length);

	/*
	 * Make sure we have this length.
	 */
	buf_ptr = ((const char *)buf_ptr) + rval.consumed;
	size -= rval.consumed;
	if(length > (ber_tlv_len_t)size) {
		rval.code = RC_WMORE;
		rval.consumed = 0;
		return rval;
	}

	/*
	 * ASN.1 encoded INTEGER: buf_ptr, length
	 * Fill the native, at the same time checking for overflow.
	 * If overflow occurred, return with RC_FAIL.
	 */
	{
		INTEGER_t tmp;
		union {
			const void *constbuf;
			void *nonconstbuf;
		} unconst_buf;
		long l;

		unconst_buf.constbuf = buf_ptr;
		tmp.buf = (uint8_t *)unconst_buf.nonconstbuf;
		tmp.size = length;

		if((specs&&specs->field_unsigned)
			? asn_INTEGER2ulong(&tmp, (unsigned long *)&l) /* sic */
			: asn_INTEGER2long(&tmp, &l)) {
			rval.code = RC_FAIL;
			rval.consumed = 0;
			return rval;
		}

		*native = l;
	}

	rval.code = RC_OK;
	rval.consumed += length;

	ASN_DEBUG("Took %ld/%ld bytes to encode %s (%ld)",
		(long)rval.consumed, (long)length, td->name, (long)*native);

	return rval;
}

/*
 * Encode the NativeInteger using the standard INTEGER type DER encoder.
 */
asn_enc_rval_t
NativeInteger_encode_der(const asn_TYPE_descriptor_t *sd, const void *ptr,
                         int tag_mode, ber_tlv_tag_t tag,
                         asn_app_consume_bytes_f *cb, void *app_key) {
    unsigned long native = *(const unsigned long *)ptr; /* Disable sign ext. */
    asn_enc_rval_t erval;
	INTEGER_t tmp;

#ifdef	WORDS_BIGENDIAN		/* Opportunistic optimization */

	tmp.buf = (uint8_t *)&native;
	tmp.size = sizeof(native);

#else	/* Works even if WORDS_BIGENDIAN is not set where should've been */
	uint8_t buf[sizeof(native)];
	uint8_t *p;

	/* Prepare a fake INTEGER */
	for(p = buf + sizeof(buf) - 1; p >= buf; p--, native >>= 8)
		*p = (uint8_t)native;

	tmp.buf = buf;
	tmp.size = sizeof(buf);
#endif	/* WORDS_BIGENDIAN */
	
	/* Encode fake INTEGER */
	erval = INTEGER_encode_der(sd, &tmp, tag_mode, tag, cb, app_key);
    if(erval.structure_ptr == &tmp) {
        erval.structure_ptr = ptr;
    }
	return erval;
}

/*
 * Decode the chunk of XML text encoding INTEGER.
 */
asn_dec_rval_t
NativeInteger_decode_xer(const asn_codec_ctx_t *opt_codec_ctx,
                         const asn_TYPE_descriptor_t *td, void **sptr,
                         const char *opt_mname, const void *buf_ptr,
                         size_t size) {
    const asn_INTEGER_specifics_t *specs =
        (const asn_INTEGER_specifics_t *)td->specifics;
    asn_dec_rval_t rval;
	INTEGER_t st;
	void *st_ptr = (void *)&st;
	long *native = (long *)*sptr;

	if(!native) {
		native = (long *)(*sptr = CALLOC(1, sizeof(*native)));
		if(!native) ASN__DECODE_FAILED;
	}

	memset(&st, 0, sizeof(st));
	rval = INTEGER_decode_xer(opt_codec_ctx, td, &st_ptr, 
		opt_mname, buf_ptr, size);
	if(rval.code == RC_OK) {
		long l;
		if((specs&&specs->field_unsigned)
			? asn_INTEGER2ulong(&st, (unsigned long *)&l) /* sic */
			: asn_INTEGER2long(&st, &l)) {
			rval.code = RC_FAIL;
			rval.consumed = 0;
		} else {
			*native = l;
		}
	} else {
		/*
		 * Cannot restart from the middle;
		 * there is no place to save state in the native type.
		 * Request a continuation from the very beginning.
		 */
		rval.consumed = 0;
	}
	ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_INTEGER, &st);
	return rval;
}


asn_enc_rval_t
NativeInteger_encode_xer(const asn_TYPE_descriptor_t *td, const void *sptr,
                         int ilevel, enum xer_encoder_flags_e flags,
                         asn_app_consume_bytes_f *cb, void *app_key) {
    const asn_INTEGER_specifics_t *specs =
        (const asn_INTEGER_specifics_t *)td->specifics;
    char scratch[32];	/* Enough for 64-bit int */
	asn_enc_rval_t er;
	const long *native = (const long *)sptr;

	(void)ilevel;
	(void)flags;

	if(!native) ASN__ENCODE_FAILED;

	er.encoded = snprintf(scratch, sizeof(scratch),
			(specs && specs->field_unsigned)
			? "%lu" : "%ld", *native);
	if(er.encoded <= 0 || (size_t)er.encoded >= sizeof(scratch)
		|| cb(scratch, er.encoded, app_key) < 0)
		ASN__ENCODE_FAILED;

	ASN__ENCODED_OK(er);
}

#ifndef  ASN_DISABLE_PER_SUPPORT

asn_dec_rval_t
NativeInteger_decode_uper(const asn_codec_ctx_t *opt_codec_ctx,
                          const asn_TYPE_descriptor_t *td,
                          const asn_per_constraints_t *constraints, void **sptr,
                          asn_per_data_t *pd) {
    const asn_INTEGER_specifics_t *specs =
        (const asn_INTEGER_specifics_t *)td->specifics;
    asn_dec_rval_t rval;
	long *native = (long *)*sptr;
	INTEGER_t tmpint;
	void *tmpintptr = &tmpint;

	(void)opt_codec_ctx;
	ASN_DEBUG("Decoding NativeInteger %s (UPER)", td->name);

	if(!native) {
		native = (long *)(*sptr = CALLOC(1, sizeof(*native)));
		if(!native) ASN__DECODE_FAILED;
	}

	memset(&tmpint, 0, sizeof tmpint);
	rval = INTEGER_decode_uper(opt_codec_ctx, td, constraints,
				   &tmpintptr, pd);
	if(rval.code == RC_OK) {
		if((specs&&specs->field_unsigned)
			? asn_INTEGER2ulong(&tmpint, (unsigned long *)native)
			: asn_INTEGER2long(&tmpint, native))
			rval.code = RC_FAIL;
		else
			ASN_DEBUG("NativeInteger %s got value %ld",
				td->name, *native);
	}
	ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_INTEGER, &tmpint);

	return rval;
}

asn_enc_rval_t
NativeInteger_encode_uper(const asn_TYPE_descriptor_t *td,
                          const asn_per_constraints_t *constraints,
                          const void *sptr, asn_per_outp_t *po) {
    const asn_INTEGER_specifics_t *specs =
        (const asn_INTEGER_specifics_t *)td->specifics;
    asn_enc_rval_t er;
	long native;
	INTEGER_t tmpint;

	if(!sptr) ASN__ENCODE_FAILED;

    native = *(const long *)sptr;

    ASN_DEBUG("Encoding NativeInteger %s %ld (UPER)", td->name, native);

	memset(&tmpint, 0, sizeof(tmpint));
	if((specs&&specs->field_unsigned)
		? asn_ulong2INTEGER(&tmpint, native)
		: asn_long2INTEGER(&tmpint, native))
		ASN__ENCODE_FAILED;
	er = INTEGER_encode_uper(td, constraints, &tmpint, po);
	ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_INTEGER, &tmpint);
	return er;
}

#endif  /* ASN_DISABLE_PER_SUPPORT */

/*
 * INTEGER specific human-readable output.
 */
int
NativeInteger_print(const asn_TYPE_descriptor_t *td, const void *sptr,
                    int ilevel, asn_app_consume_bytes_f *cb, void *app_key) {
    const asn_INTEGER_specifics_t *specs =
        (const asn_INTEGER_specifics_t *)td->specifics;
    const long *native = (const long *)sptr;
    char scratch[32]; /* Enough for 64-bit int */
    int ret;

    (void)td;       /* Unused argument */
    (void)ilevel;   /* Unused argument */

    if(native) {
        long value = *native;
        ret = snprintf(scratch, sizeof(scratch),
                       (specs && specs->field_unsigned) ? "%lu" : "%ld", value);
        assert(ret > 0 && (size_t)ret < sizeof(scratch));
        if(cb(scratch, ret, app_key) < 0) return -1;
        if(specs && (value >= 0 || !specs->field_unsigned)) {
            const asn_INTEGER_enum_map_t *el =
                INTEGER_map_value2enum(specs, value);
            if(el) {
                if(cb(" (", 2, app_key) < 0) return -1;
                if(cb(el->enum_name, el->enum_len, app_key) < 0) return -1;
                if(cb(")", 1, app_key) < 0) return -1;
            }
        }
        return 0;
	} else {
		return (cb("<absent>", 8, app_key) < 0) ? -1 : 0;
	}
}

void
NativeInteger_free(const asn_TYPE_descriptor_t *td, void *ptr,
                   enum asn_struct_free_method method) {
    if(!td || !ptr)
		return;

	ASN_DEBUG("Freeing %s as INTEGER (%d, %p, Native)",
		td->name, method, ptr);

    switch(method) {
    case ASFM_FREE_EVERYTHING:
        FREEMEM(ptr);
        break;
    case ASFM_FREE_UNDERLYING:
        break;
    case ASFM_FREE_UNDERLYING_AND_RESET:
        memset(ptr, 0, sizeof(long));
        break;
    }
}

int
NativeInteger_compare(const asn_TYPE_descriptor_t *td, const void *aptr, const void *bptr) {
    (void)td;

    if(aptr && bptr) {
        const asn_INTEGER_specifics_t *specs =
            (const asn_INTEGER_specifics_t *)td->specifics;
        if(specs && specs->field_unsigned) {
            const unsigned long *a = aptr;
            const unsigned long *b = bptr;
            if(*a < *b) {
                return -1;
            } else if(*a > *b) {
                return 1;
            } else {
                return 0;
            }
        } else {
            const long *a = aptr;
            const long *b = bptr;
            if(*a < *b) {
                return -1;
            } else if(*a > *b) {
                return 1;
            } else {
                return 0;
            }
        }
    } else if(!aptr) {
        return -1;
    } else {
        return 1;
    }
}

asn_random_fill_result_t
NativeInteger_random_fill(const asn_TYPE_descriptor_t *td, void **sptr,
                          const asn_encoding_constraints_t *constraints,
                          size_t max_length) {
    const asn_INTEGER_specifics_t *specs =
        (const asn_INTEGER_specifics_t *)td->specifics;
    asn_random_fill_result_t result_ok = {ARFILL_OK, 1};
    asn_random_fill_result_t result_failed = {ARFILL_FAILED, 0};
    asn_random_fill_result_t result_skipped = {ARFILL_SKIPPED, 0};
    long *st = *sptr;
    const asn_INTEGER_enum_map_t *emap;
    size_t emap_len;
    intmax_t value;
    int find_inside_map;

    if(max_length == 0) return result_skipped;

    if(st == NULL) {
        st = (long *)CALLOC(1, sizeof(*st));
        if(st == NULL) {
            return result_failed;
        }
    }

    if(specs) {
        emap = specs->value2enum;
        emap_len = specs->map_count;
        if(specs->strict_enumeration) {
            find_inside_map = emap_len > 0;
        } else {
            find_inside_map = emap_len ? asn_random_between(0, 1) : 0;
        }
    } else {
        emap = 0;
        emap_len = 0;
        find_inside_map = 0;
    }

    if(find_inside_map) {
        assert(emap_len > 0);
        value = emap[asn_random_between(0, emap_len - 1)].nat_value;
    } else {
        const asn_per_constraints_t *ct;

        static const long variants[] = {
            -65536, -65535, -65534, -32769, -32768, -32767, -16385, -16384,
            -16383, -257,   -256,   -255,   -254,   -129,   -128,   -127,
            -126,   -1,     0,      1,      126,    127,    128,    129,
            254,    255,    256,    257,    16383,  16384,  16385,  32767,
            32768,  32769,  65534,  65535,  65536,  65537};
        if(specs && specs->field_unsigned) {
            assert(variants[18] == 0);
            value = variants[asn_random_between(
                18, sizeof(variants) / sizeof(variants[0]) - 1)];
        } else {
            value = variants[asn_random_between(
                0, sizeof(variants) / sizeof(variants[0]) - 1)];
        }

        if(!constraints) constraints = &td->encoding_constraints;
        ct = constraints ? constraints->per_constraints : 0;
        if(ct && (ct->value.flags & APC_CONSTRAINED)) {
            if(value < ct->value.lower_bound || value > ct->value.upper_bound) {
                value = asn_random_between(ct->value.lower_bound,
                                           ct->value.upper_bound);
            }
        }
    }

    *sptr = st;
    *st = value;
    return result_ok;
}
