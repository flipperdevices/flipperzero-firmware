/*-
 * Copyright (c) 2003, 2004 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
#include <asn_internal.h>
#include <errno.h>

static ssize_t der_write_TL(ber_tlv_tag_t tag, ber_tlv_len_t len,
	asn_app_consume_bytes_f *cb, void *app_key, int constructed);

/*
 * The DER encoder of any type.
 */
asn_enc_rval_t
der_encode(const asn_TYPE_descriptor_t *type_descriptor, const void *struct_ptr,
           asn_app_consume_bytes_f *consume_bytes, void *app_key) {
    ASN_DEBUG("DER encoder invoked for %s",
		type_descriptor->name);

	/*
	 * Invoke type-specific encoder.
	 */
    return type_descriptor->op->der_encoder(
        type_descriptor, struct_ptr, /* Pointer to the destination structure */
        0, 0, consume_bytes, app_key);
}

/*
 * Argument type and callback necessary for der_encode_to_buffer().
 */
typedef struct enc_to_buf_arg {
	void *buffer;
	size_t left;
} enc_to_buf_arg;
static int encode_to_buffer_cb(const void *buffer, size_t size, void *key) {
	enc_to_buf_arg *arg = (enc_to_buf_arg *)key;

	if(arg->left < size)
		return -1;	/* Data exceeds the available buffer size */

	memcpy(arg->buffer, buffer, size);
	arg->buffer = ((char *)arg->buffer) + size;
	arg->left -= size;

	return 0;
}

/*
 * A variant of the der_encode() which encodes the data into the provided buffer
 */
asn_enc_rval_t
der_encode_to_buffer(const asn_TYPE_descriptor_t *type_descriptor,
                     const void *struct_ptr, void *buffer, size_t buffer_size) {
    enc_to_buf_arg arg;
	asn_enc_rval_t ec;

	arg.buffer = buffer;
	arg.left = buffer_size;

	ec = type_descriptor->op->der_encoder(type_descriptor,
		struct_ptr,	/* Pointer to the destination structure */
		0, 0, encode_to_buffer_cb, &arg);
	if(ec.encoded != -1) {
		assert(ec.encoded == (ssize_t)(buffer_size - arg.left));
		/* Return the encoded contents size */
	}
	return ec;
}


/*
 * Write out leading TL[v] sequence according to the type definition.
 */
ssize_t
der_write_tags(const asn_TYPE_descriptor_t *sd, size_t struct_length,
               int tag_mode, int last_tag_form,
               ber_tlv_tag_t tag, /* EXPLICIT or IMPLICIT tag */
               asn_app_consume_bytes_f *cb, void *app_key) {
#define ASN1_DER_MAX_TAGS_COUNT 4
    ber_tlv_tag_t
        tags_buf_scratch[ASN1_DER_MAX_TAGS_COUNT * sizeof(ber_tlv_tag_t)];
    ssize_t lens[ASN1_DER_MAX_TAGS_COUNT * sizeof(ssize_t)];
    const ber_tlv_tag_t *tags; /* Copy of tags stream */
    int tags_count;            /* Number of tags */
    size_t overall_length;
    int i;

    ASN_DEBUG("Writing tags (%s, tm=%d, tc=%d, tag=%s, mtc=%d)",
		sd->name, tag_mode, sd->tags_count,
		ber_tlv_tag_string(tag),
		tag_mode
			?(sd->tags_count+1
				-((tag_mode == -1) && sd->tags_count))
			:sd->tags_count
	);

    if(sd->tags_count + 1 > ASN1_DER_MAX_TAGS_COUNT) {
        ASN_DEBUG("System limit %d on tags count", ASN1_DER_MAX_TAGS_COUNT);
        return -1;
    }

	if(tag_mode) {
		/*
		 * Instead of doing shaman dance like we do in ber_check_tags(),
		 * allocate a small array on the stack
		 * and initialize it appropriately.
		 */
		int stag_offset;
		ber_tlv_tag_t *tags_buf = tags_buf_scratch;
		tags_count = sd->tags_count
			+ 1	/* EXPLICIT or IMPLICIT tag is given */
			- ((tag_mode == -1) && sd->tags_count);
		/* Copy tags over */
		tags_buf[0] = tag;
		stag_offset = -1 + ((tag_mode == -1) && sd->tags_count);
		for(i = 1; i < tags_count; i++)
			tags_buf[i] = sd->tags[i + stag_offset];
		tags = tags_buf;
	} else {
		tags = sd->tags;
		tags_count = sd->tags_count;
	}

	/* No tags to write */
	if(tags_count == 0)
		return 0;

	/*
	 * Array of tags is initialized.
	 * Now, compute the size of the TLV pairs, from right to left.
	 */
	overall_length = struct_length;
	for(i = tags_count - 1; i >= 0; --i) {
		lens[i] = der_write_TL(tags[i], overall_length, 0, 0, 0);
		if(lens[i] == -1) return -1;
		overall_length += lens[i];
		lens[i] = overall_length - lens[i];
	}

	if(!cb) return overall_length - struct_length;

	ASN_DEBUG("Encoding %s TL sequence (%d elements)", sd->name,
                  tags_count);

	/*
	 * Encode the TL sequence for real.
	 */
	for(i = 0; i < tags_count; i++) {
		ssize_t len;
		int _constr;

		/* Check if this tag happens to be constructed */
		_constr = (last_tag_form || i < (tags_count - 1));

		len = der_write_TL(tags[i], lens[i], cb, app_key, _constr);
		if(len == -1) return -1;
	}

	return overall_length - struct_length;
}

static ssize_t
der_write_TL(ber_tlv_tag_t tag, ber_tlv_len_t len,
		asn_app_consume_bytes_f *cb, void *app_key,
		int constructed) {
	uint8_t buf[32];
	size_t size = 0;
	int buf_size = cb?sizeof(buf):0;
	ssize_t tmp;

	/* Serialize tag (T from TLV) into possibly zero-length buffer */
	tmp = ber_tlv_tag_serialize(tag, buf, buf_size);
	if(tmp == -1 || tmp > (ssize_t)sizeof(buf)) return -1;
	size += tmp;

	/* Serialize length (L from TLV) into possibly zero-length buffer */
	tmp = der_tlv_length_serialize(len, buf+size, buf_size?buf_size-size:0);
	if(tmp == -1) return -1;
	size += tmp;

	if(size > sizeof(buf))
		return -1;

	/*
	 * If callback is specified, invoke it, and check its return value.
	 */
	if(cb) {
		if(constructed) *buf |= 0x20;
		if(cb(buf, size, app_key) < 0)
			return -1;
	}

	return size;
}
