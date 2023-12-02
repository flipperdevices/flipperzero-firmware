/*-
 * Copyright (c) 2003, 2004 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
#include <asn_internal.h>
#include <ber_tlv_length.h>
#include <ber_tlv_tag.h>

ssize_t
ber_fetch_length(int _is_constructed, const void *bufptr, size_t size,
		ber_tlv_len_t *len_r) {
	const uint8_t *buf = (const uint8_t *)bufptr;
	unsigned oct;

	if(size == 0)
		return 0;	/* Want more */

	oct = *(const uint8_t *)buf;
	if((oct & 0x80) == 0) {
		/*
		 * Short definite length.
		 */
		*len_r = oct;	/* & 0x7F */
		return 1;
	} else {
		ber_tlv_len_t len;
		size_t skipped;

		if(_is_constructed && oct == 0x80) {
			*len_r = -1;	/* Indefinite length */
			return 1;
		}

		if(oct == 0xff) {
			/* Reserved in standard for future use. */
			return -1;
		}

		oct &= 0x7F;	/* Leave only the 7 LS bits */
		for(len = 0, buf++, skipped = 1;
			oct && (++skipped <= size); buf++, oct--) {

			/* Verify that we won't overflow. */
			if(!(len >> ((8 * sizeof(len)) - (8+1)))) {
				len = (len << 8) | *buf;
			} else {
				/* Too large length value. */
				return -1;
			}
		}

		if(oct == 0) {
			if(len < 0 || len > RSSIZE_MAX) {
				/* Length value out of sane range. */
				return -1;
			}

			*len_r = len;
			return skipped;
		}

		return 0;	/* Want more */
	}

}

ssize_t
ber_skip_length(const asn_codec_ctx_t *opt_codec_ctx,
		int _is_constructed, const void *ptr, size_t size) {
	ber_tlv_len_t vlen;	/* Length of V in TLV */
	ssize_t tl;		/* Length of L in TLV */
	ssize_t ll;		/* Length of L in TLV */
	size_t skip;

	/*
	 * Make sure we didn't exceed the maximum stack size.
	 */
	if(ASN__STACK_OVERFLOW_CHECK(opt_codec_ctx))
		return -1;

	/*
	 * Determine the size of L in TLV.
	 */
	ll = ber_fetch_length(_is_constructed, ptr, size, &vlen);
	if(ll <= 0) return ll;

	/*
	 * Definite length.
	 */
	if(vlen >= 0) {
		skip = ll + vlen;
		if(skip > size)
			return 0;	/* Want more */
		return skip;
	}

	/*
	 * Indefinite length!
	 */
	ASN_DEBUG("Skipping indefinite length");
	for(skip = ll, ptr = ((const char *)ptr) + ll, size -= ll;;) {
		ber_tlv_tag_t tag;

		/* Fetch the tag */
		tl = ber_fetch_tag(ptr, size, &tag);
		if(tl <= 0) return tl;

		ll = ber_skip_length(opt_codec_ctx,
			BER_TLV_CONSTRUCTED(ptr),
			((const char *)ptr) + tl, size - tl);
		if(ll <= 0) return ll;

		skip += tl + ll;

		/*
		 * This may be the end of the indefinite length structure,
		 * two consecutive 0 octets.
		 * Check if it is true.
		 */
		if(((const uint8_t *)ptr)[0] == 0
		&& ((const uint8_t *)ptr)[1] == 0)
			return skip;

		ptr = ((const char *)ptr) + tl + ll;
		size -= tl + ll;
 	}

	/* UNREACHABLE */
}

size_t
der_tlv_length_serialize(ber_tlv_len_t len, void *bufp, size_t size) {
	size_t required_size;	/* Size of len encoding */
	uint8_t *buf = (uint8_t *)bufp;
	uint8_t *end;
	int i;

	if(len <= 127) {
		/* Encoded in 1 octet */
		if(size) *buf = (uint8_t)len;
		return 1;
	}

	/*
	 * Compute the size of the subsequent bytes.
	 */
	for(required_size = 1, i = 8; i < 8 * (int)sizeof(len); i += 8) {
		if(len >> i)
			required_size++;
		else
			break;
	}

	if(size <= required_size)
		return required_size + 1;

	*buf++ = (uint8_t)(0x80 | required_size);  /* Length of the encoding */

	/*
	 * Produce the len encoding, space permitting.
	 */
	end = buf + required_size;
	for(i -= 8; buf < end; i -= 8, buf++)
		*buf = (uint8_t)(len >> i);

	return required_size + 1;
}

