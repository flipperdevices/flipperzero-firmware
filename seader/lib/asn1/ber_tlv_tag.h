/*-
 * Copyright (c) 2003-2017 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
#ifndef	_BER_TLV_TAG_H_
#define	_BER_TLV_TAG_H_

#ifdef __cplusplus
extern "C" {
#endif

enum asn_tag_class {
	ASN_TAG_CLASS_UNIVERSAL		= 0,	/* 0b00 */
	ASN_TAG_CLASS_APPLICATION	= 1,	/* 0b01 */
	ASN_TAG_CLASS_CONTEXT		= 2,	/* 0b10 */
	ASN_TAG_CLASS_PRIVATE		= 3	/* 0b11 */
};
typedef unsigned ber_tlv_tag_t;	/* BER TAG from Tag-Length-Value */

/*
 * Tag class is encoded together with tag value for optimization purposes.
 */
#define	BER_TAG_CLASS(tag)	((tag) & 0x3)
#define	BER_TAG_VALUE(tag)	((tag) >> 2)
#define	BER_TLV_CONSTRUCTED(tagptr)	(((*(const uint8_t *)tagptr)&0x20)?1:0)

#define	BER_TAGS_EQUAL(tag1, tag2)	((tag1) == (tag2))

/*
 * Several functions for printing the TAG in the canonical form
 * (i.e. "[PRIVATE 0]").
 * Return values correspond to their libc counterparts (if any).
 */
ssize_t ber_tlv_tag_snprint(ber_tlv_tag_t tag, char *buf, size_t buflen);
ssize_t ber_tlv_tag_fwrite(ber_tlv_tag_t tag, FILE *);
char *ber_tlv_tag_string(ber_tlv_tag_t tag);


/*
 * This function tries to fetch the tag from the input stream.
 * RETURN VALUES:
 * 	 0:	More data expected than bufptr contains.
 * 	-1:	Fatal error deciphering tag.
 *	>0:	Number of bytes used from bufptr. tag_r will contain the tag.
 */
ssize_t ber_fetch_tag(const void *bufptr, size_t size, ber_tlv_tag_t *tag_r);

/*
 * This function serializes the tag (T from TLV) in BER format.
 * It always returns number of bytes necessary to represent the tag,
 * it is a caller's responsibility to check the return value
 * against the supplied buffer's size.
 */
size_t ber_tlv_tag_serialize(ber_tlv_tag_t tag, void *bufptr, size_t size);

#ifdef __cplusplus
}
#endif

#endif	/* _BER_TLV_TAG_H_ */
