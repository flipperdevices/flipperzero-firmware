/*
 * Copyright (c) 2005-2017 Lev Walkin <vlm@lionet.info>.
 * All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
#include <asn_system.h>
#include <asn_internal.h>
#include <asn_bit_data.h>

/*
 * Create a contiguous non-refillable bit data structure.
 * Can be freed by FREEMEM().
 */
asn_bit_data_t *
asn_bit_data_new_contiguous(const void *data, size_t size_bits) {
    size_t size_bytes = (size_bits + 7) / 8;
    asn_bit_data_t *pd;
    uint8_t *bytes;

    /* Get the extensions map */
    pd = CALLOC(1, sizeof(*pd) + size_bytes + 1);
    if(!pd) {
        return NULL;
    }
    bytes = (void *)(((char *)pd) + sizeof(*pd));
    memcpy(bytes, data, size_bytes);
    bytes[size_bytes] = 0;
    pd->buffer = bytes;
    pd->nboff = 0;
    pd->nbits = size_bits;

    return pd;
}


char *
asn_bit_data_string(asn_bit_data_t *pd) {
	static char buf[2][32];
	static int n;
	n = (n+1) % 2;
    snprintf(buf[n], sizeof(buf[n]),
             "{m=%" ASN_PRI_SIZE " span %" ASN_PRI_SIZE "[%" ASN_PRI_SIZE
             "..%" ASN_PRI_SIZE "] (%" ASN_PRI_SIZE ")}",
             pd->moved, ((uintptr_t)(pd->buffer) & 0xf), pd->nboff, pd->nbits,
             pd->nbits - pd->nboff);
    return buf[n];
}

void
asn_get_undo(asn_bit_data_t *pd, int nbits) {
	if((ssize_t)pd->nboff < nbits) {
		assert((ssize_t)pd->nboff < nbits);
	} else {
		pd->nboff -= nbits;
		pd->moved -= nbits;
	}
}

/*
 * Extract a small number of bits (<= 31) from the specified PER data pointer.
 */
int32_t
asn_get_few_bits(asn_bit_data_t *pd, int nbits) {
	size_t off;	/* Next after last bit offset */
	ssize_t nleft;	/* Number of bits left in this stream */
	uint32_t accum;
	const uint8_t *buf;

	if(nbits < 0)
		return -1;

	nleft = pd->nbits - pd->nboff;
	if(nbits > nleft) {
		int32_t tailv, vhead;
		if(!pd->refill || nbits > 31) return -1;
		/* Accumulate unused bytes before refill */
		ASN_DEBUG("Obtain the rest %d bits (want %d)",
			(int)nleft, (int)nbits);
		tailv = asn_get_few_bits(pd, nleft);
		if(tailv < 0) return -1;
		/* Refill (replace pd contents with new data) */
		if(pd->refill(pd))
			return -1;
		nbits -= nleft;
		vhead = asn_get_few_bits(pd, nbits);
		/* Combine the rest of previous pd with the head of new one */
		tailv = (tailv << nbits) | vhead;  /* Could == -1 */
		return tailv;
	}

	/*
	 * Normalize position indicator.
	 */
	if(pd->nboff >= 8) {
		pd->buffer += (pd->nboff >> 3);
		pd->nbits  -= (pd->nboff & ~0x07);
		pd->nboff  &= 0x07;
	}
	pd->moved += nbits;
	pd->nboff += nbits;
	off = pd->nboff;
	buf = pd->buffer;

	/*
	 * Extract specified number of bits.
	 */
	if(off <= 8)
		accum = nbits ? (buf[0]) >> (8 - off) : 0;
	else if(off <= 16)
		accum = ((buf[0] << 8) + buf[1]) >> (16 - off);
	else if(off <= 24)
		accum = ((buf[0] << 16) + (buf[1] << 8) + buf[2]) >> (24 - off);
	else if(off <= 31)
		accum = (((uint32_t)buf[0] << 24) + (buf[1] << 16)
			+ (buf[2] << 8) + (buf[3])) >> (32 - off);
	else if(nbits <= 31) {
		asn_bit_data_t tpd = *pd;
		/* Here are we with our 31-bits limit plus 1..7 bits offset. */
		asn_get_undo(&tpd, nbits);
		/* The number of available bits in the stream allow
		 * for the following operations to take place without
		 * invoking the ->refill() function */
		accum  = asn_get_few_bits(&tpd, nbits - 24) << 24;
		accum |= asn_get_few_bits(&tpd, 24);
	} else {
		asn_get_undo(pd, nbits);
		return -1;
	}

	accum &= (((uint32_t)1 << nbits) - 1);

	ASN_DEBUG("  [PER got %2d<=%2d bits => span %d %+ld[%d..%d]:%02x (%d) => 0x%x]",
		(int)nbits, (int)nleft,
		(int)pd->moved,
		(((long)pd->buffer) & 0xf),
		(int)pd->nboff, (int)pd->nbits,
		((pd->buffer != NULL)?pd->buffer[0]:0),
		(int)(pd->nbits - pd->nboff),
		(int)accum);

	return accum;
}

/*
 * Extract a large number of bits from the specified PER data pointer.
 */
int
asn_get_many_bits(asn_bit_data_t *pd, uint8_t *dst, int alright, int nbits) {
	int32_t value;

	if(alright && (nbits & 7)) {
		/* Perform right alignment of a first few bits */
		value = asn_get_few_bits(pd, nbits & 0x07);
		if(value < 0) return -1;
		*dst++ = value;	/* value is already right-aligned */
		nbits &= ~7;
	}

	while(nbits) {
		if(nbits >= 24) {
			value = asn_get_few_bits(pd, 24);
			if(value < 0) return -1;
			*(dst++) = value >> 16;
			*(dst++) = value >> 8;
			*(dst++) = value;
			nbits -= 24;
		} else {
			value = asn_get_few_bits(pd, nbits);
			if(value < 0) return -1;
			if(nbits & 7) {	/* implies left alignment */
				value <<= 8 - (nbits & 7),
				nbits += 8 - (nbits & 7);
				if(nbits > 24)
					*dst++ = value >> 24;
			}
			if(nbits > 16)
				*dst++ = value >> 16;
			if(nbits > 8)
				*dst++ = value >> 8;
			*dst++ = value;
			break;
		}
	}

	return 0;
}

/*
 * Put a small number of bits (<= 31).
 */
int
asn_put_few_bits(asn_bit_outp_t *po, uint32_t bits, int obits) {
	size_t off;	/* Next after last bit offset */
	size_t omsk;	/* Existing last byte meaningful bits mask */
	uint8_t *buf;

	if(obits <= 0 || obits >= 32) return obits ? -1 : 0;

	ASN_DEBUG("[PER put %d bits %x to %p+%d bits]",
			obits, (int)bits, (void *)po->buffer, (int)po->nboff);

	/*
	 * Normalize position indicator.
	 */
	if(po->nboff >= 8) {
		po->buffer += (po->nboff >> 3);
		po->nbits  -= (po->nboff & ~0x07);
		po->nboff  &= 0x07;
	}

	/*
	 * Flush whole-bytes output, if necessary.
	 */
	if(po->nboff + obits > po->nbits) {
		size_t complete_bytes;
		if(!po->buffer) po->buffer = po->tmpspace;
		complete_bytes = (po->buffer - po->tmpspace);
		ASN_DEBUG("[PER output %ld complete + %ld]",
			(long)complete_bytes, (long)po->flushed_bytes);
		if(po->output(po->tmpspace, complete_bytes, po->op_key) < 0)
			return -1;
		if(po->nboff)
			po->tmpspace[0] = po->buffer[0];
		po->buffer = po->tmpspace;
		po->nbits = 8 * sizeof(po->tmpspace);
		po->flushed_bytes += complete_bytes;
	}

	/*
	 * Now, due to sizeof(tmpspace), we are guaranteed large enough space.
	 */
	buf = po->buffer;
	omsk = ~((1 << (8 - po->nboff)) - 1);
	off = (po->nboff + obits);

	/* Clear data of debris before meaningful bits */
	bits &= (((uint32_t)1 << obits) - 1);

	ASN_DEBUG("[PER out %d %u/%x (t=%d,o=%d) %x&%x=%x]", obits,
		(int)bits, (int)bits,
		(int)po->nboff, (int)off,
		buf[0], (int)(omsk&0xff),
		(int)(buf[0] & omsk));

	if(off <= 8)	/* Completely within 1 byte */
		po->nboff = off,
		bits <<= (8 - off),
		buf[0] = (buf[0] & omsk) | bits;
	else if(off <= 16)
		po->nboff = off,
		bits <<= (16 - off),
		buf[0] = (buf[0] & omsk) | (bits >> 8),
		buf[1] = bits;
	else if(off <= 24)
		po->nboff = off,
		bits <<= (24 - off),
		buf[0] = (buf[0] & omsk) | (bits >> 16),
		buf[1] = bits >> 8,
		buf[2] = bits;
	else if(off <= 31)
		po->nboff = off,
		bits <<= (32 - off),
		buf[0] = (buf[0] & omsk) | (bits >> 24),
		buf[1] = bits >> 16,
		buf[2] = bits >> 8,
		buf[3] = bits;
	else {
		if(asn_put_few_bits(po, bits >> (obits - 24), 24)) return -1;
		if(asn_put_few_bits(po, bits, obits - 24)) return -1;
	}

	ASN_DEBUG("[PER out %u/%x => %02x buf+%ld]",
		(int)bits, (int)bits, buf[0],
		(long)(po->buffer - po->tmpspace));

	return 0;
}


/*
 * Output a large number of bits.
 */
int
asn_put_many_bits(asn_bit_outp_t *po, const uint8_t *src, int nbits) {

	while(nbits) {
		uint32_t value;

		if(nbits >= 24) {
			value = (src[0] << 16) | (src[1] << 8) | src[2];
			src += 3;
			nbits -= 24;
			if(asn_put_few_bits(po, value, 24))
				return -1;
		} else {
			value = src[0];
			if(nbits > 8)
				value = (value << 8) | src[1];
			if(nbits > 16)
				value = (value << 8) | src[2];
			if(nbits & 0x07)
				value >>= (8 - (nbits & 0x07));
			if(asn_put_few_bits(po, value, nbits))
				return -1;
			break;
		}
	}

	return 0;
}


int
asn_put_aligned_flush(asn_bit_outp_t *po) {
    uint32_t unused_bits = (0x7 & (8 - (po->nboff & 0x07)));
    size_t complete_bytes =
        (po->buffer ? po->buffer - po->tmpspace : 0) + ((po->nboff + 7) >> 3);

    if(unused_bits) {
        po->buffer[po->nboff >> 3] &= ~0u << unused_bits;
    }

    if(po->output(po->tmpspace, complete_bytes, po->op_key) < 0) {
        return -1;
    } else {
        po->buffer = po->tmpspace;
        po->nboff = 0;
        po->nbits = 8 * sizeof(po->tmpspace);
        po->flushed_bytes += complete_bytes;
        return 0;
    }
}

