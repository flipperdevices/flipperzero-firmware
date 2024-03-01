/*
 * Copyright (c) 2005-2017 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
#include <asn_system.h>
#include <asn_internal.h>
#include <per_support.h>

/*
 * X.691-201508 #10.9 General rules for encoding a length determinant.
 * Get the optionally constrained length "n" from the stream.
 */
ssize_t
uper_get_length(asn_per_data_t *pd, int ebits, size_t lower_bound,
                int *repeat) {
    ssize_t value;

    *repeat = 0;

    /* #11.9.4.1 Encoding if constrained (according to effective bits) */
    if(ebits >= 0 && ebits <= 16) {
        value = per_get_few_bits(pd, ebits);
        if(value >= 0) value += lower_bound;
        return value;
    }

	value = per_get_few_bits(pd, 8);
    if((value & 0x80) == 0) { /* #11.9.3.6 */
        return (value & 0x7F);
    } else if((value & 0x40) == 0) { /* #11.9.3.7 */
        /* bit 8 ... set to 1 and bit 7 ... set to zero */
        value = ((value & 0x3f) << 8) | per_get_few_bits(pd, 8);
        return value; /* potential -1 from per_get_few_bits passes through. */
    } else if(value < 0) {
        ASN_DEBUG("END of stream reached for PER");
        return -1;
    }
    value &= 0x3f; /* this is "m" from X.691, #11.9.3.8 */
    if(value < 1 || value > 4) {
        return -1; /* Prohibited by #11.9.3.8 */
    }
    *repeat = 1;
    return (16384 * value);
}

/*
 * Get the normally small length "n".
 * This procedure used to decode length of extensions bit-maps
 * for SET and SEQUENCE types.
 */
ssize_t
uper_get_nslength(asn_per_data_t *pd) {
	ssize_t length;

	ASN_DEBUG("Getting normally small length");

	if(per_get_few_bits(pd, 1) == 0) {
		length = per_get_few_bits(pd, 6) + 1;
		if(length <= 0) return -1;
		ASN_DEBUG("l=%d", (int)length);
		return length;
	} else {
		int repeat;
		length = uper_get_length(pd, -1, 0, &repeat);
		if(length >= 0 && !repeat) return length;
		return -1; /* Error, or do not support >16K extensions */
	}
}

/*
 * Get the normally small non-negative whole number.
 * X.691, #10.6
 */
ssize_t
uper_get_nsnnwn(asn_per_data_t *pd) {
	ssize_t value;

	value = per_get_few_bits(pd, 7);
	if(value & 64) {	/* implicit (value < 0) */
		value &= 63;
		value <<= 2;
		value |= per_get_few_bits(pd, 2);
		if(value & 128)	/* implicit (value < 0) */
			return -1;
		if(value == 0)
			return 0;
		if(value >= 3)
			return -1;
		value = per_get_few_bits(pd, 8 * value);
		return value;
	}

	return value;
}

/*
 * X.691-11/2008, #11.6
 * Encoding of a normally small non-negative whole number
 */
int
uper_put_nsnnwn(asn_per_outp_t *po, int n) {
	int bytes;

	if(n <= 63) {
		if(n < 0) return -1;
		return per_put_few_bits(po, n, 7);
	}
	if(n < 256)
		bytes = 1;
	else if(n < 65536)
		bytes = 2;
	else if(n < 256 * 65536)
		bytes = 3;
	else
		return -1;	/* This is not a "normally small" value */
	if(per_put_few_bits(po, bytes, 8))
		return -1;

	return per_put_few_bits(po, n, 8 * bytes);
}


/* X.691-2008/11, #11.5.6 -> #11.3 */
int uper_get_constrained_whole_number(asn_per_data_t *pd, unsigned long *out_value, int nbits) {
	unsigned long lhalf;    /* Lower half of the number*/
	long half;

	if(nbits <= 31) {
		half = per_get_few_bits(pd, nbits);
		if(half < 0) return -1;
		*out_value = half;
		return 0;
	}

	if((size_t)nbits > 8 * sizeof(*out_value))
		return -1;  /* RANGE */

	half = per_get_few_bits(pd, 31);
	if(half < 0) return -1;

	if(uper_get_constrained_whole_number(pd, &lhalf, nbits - 31))
		return -1;

	*out_value = ((unsigned long)half << (nbits - 31)) | lhalf;
	return 0;
}


/* X.691-2008/11, #11.5.6 -> #11.3 */
int
uper_put_constrained_whole_number_u(asn_per_outp_t *po, unsigned long v,
                                    int nbits) {
    if(nbits <= 31) {
        return per_put_few_bits(po, v, nbits);
    } else {
        /* Put higher portion first, followed by lower 31-bit */
        if(uper_put_constrained_whole_number_u(po, v >> 31, nbits - 31))
            return -1;
        return per_put_few_bits(po, v, 31);
    }
}

/*
 * X.691 (08/2015) #11.9 "General rules for encoding a length determinant"
 * Put the length "n" (or part of it) into the stream.
 */
ssize_t
uper_put_length(asn_per_outp_t *po, size_t length, int *need_eom) {
    int dummy = 0;
    if(!need_eom) need_eom = &dummy;

    if(length <= 127) {	/* #11.9.3.6 */
        *need_eom = 0;
        return per_put_few_bits(po, length, 8)
            ? -1 : (ssize_t)length;
    } else if(length < 16384) { /* #10.9.3.7 */
        *need_eom = 0;
        return per_put_few_bits(po, length|0x8000, 16)
            ? -1 : (ssize_t)length;
    }

    *need_eom = 0 == (length & 16383);
    length >>= 14;
    if(length > 4) {
        *need_eom = 0;
        length = 4;
    }

    return per_put_few_bits(po, 0xC0 | length, 8)
            ? -1 : (ssize_t)(length << 14);

}


/*
 * Put the normally small length "n" into the stream.
 * This procedure used to encode length of extensions bit-maps
 * for SET and SEQUENCE types.
 */
int
uper_put_nslength(asn_per_outp_t *po, size_t length) {
    if(length <= 64) {
        /* #11.9.3.4 */
        if(length == 0) return -1;
        return per_put_few_bits(po, length - 1, 7) ? -1 : 0;
    } else {
        int need_eom = 0;
        if(uper_put_length(po, length, &need_eom) != (ssize_t)length
           || need_eom) {
            /* This might happen in case of >16K extensions */
            return -1;
        }
    }

    return 0;
}

static int
per__long_range(long lb, long ub, unsigned long *range_r) {
    unsigned long bounds_range;
    if((ub < 0) == (lb < 0)) {
        bounds_range = ub - lb;
    } else if(lb < 0) {
        assert(ub >= 0);
        bounds_range = 1 + ((unsigned long)ub + (unsigned long)-(lb + 1));
    } else {
        assert(!"Unreachable");
        return -1;
    }
    *range_r = bounds_range;
    return 0;
}

int
per_long_range_rebase(long v, long lb, long ub, unsigned long *output) {
    unsigned long range;

    assert(lb <= ub);

    if(v < lb || v > ub || per__long_range(lb, ub, &range) < 0) {
        /* Range error. */
        return -1;
    }

    /*
     * Fundamentally what we're doing is returning (v-lb).
     * However, this triggers undefined behavior when the word width
     * of signed (v) is the same as the size of unsigned (*output).
     * In practice, it triggers the UndefinedSanitizer. Therefore we shall
     * compute the ranges accurately to avoid C's undefined behavior.
     */
    if((v < 0) == (lb < 0)) {
        *output = v-lb;
        return 0;
    } else if(v < 0) {
        unsigned long rebased = 1 + (unsigned long)-(v+1) + (unsigned long)lb;
        assert(rebased <= range);   /* By construction */
        *output = rebased;
        return 0;
    } else if(lb < 0) {
        unsigned long rebased = 1 + (unsigned long)-(lb+1) + (unsigned long)v;
        assert(rebased <= range);   /* By construction */
        *output = rebased;
        return 0;
    } else {
        assert(!"Unreachable");
        return -1;
    }
}

int
per_long_range_unrebase(unsigned long inp, long lb, long ub, long *outp) {
    unsigned long range;

    if(per__long_range(lb, ub, &range) != 0) {
        return -1;
    }

    if(inp > range) {
        /*
         * We can encode something in the given number of bits that technically
         * exceeds the range. This is an avenue for security errors,
         * so we don't allow that.
         */
        return -1;
    }

    if(inp <= LONG_MAX) {
        *outp = (long)inp + lb;
    } else {
        *outp = (lb + LONG_MAX + 1) + (long)((inp - LONG_MAX) - 1);
    }

    return 0;
}
