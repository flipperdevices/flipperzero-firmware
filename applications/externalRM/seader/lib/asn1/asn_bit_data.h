/*
 * Copyright (c) 2005-2017 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
#ifndef	ASN_BIT_DATA
#define	ASN_BIT_DATA

#include <asn_system.h>		/* Platform-specific types */

#ifdef __cplusplus
extern "C" {
#endif

/*
 * This structure describes a position inside an incoming PER bit stream.
 */
typedef struct asn_bit_data_s {
  const uint8_t *buffer;  /* Pointer to the octet stream */
         size_t  nboff;   /* Bit offset to the meaningful bit */
         size_t  nbits;   /* Number of bits in the stream */
         size_t  moved;   /* Number of bits moved through this bit stream */
  int (*refill)(struct asn_bit_data_s *);
  void *refill_key;
} asn_bit_data_t;

/*
 * Create a contiguous non-refillable bit data structure.
 * Can be freed by FREEMEM().
 */
asn_bit_data_t *asn_bit_data_new_contiguous(const void *data, size_t size_bits);

/*
 * Extract a small number of bits (<= 31) from the specified PER data pointer.
 * This function returns -1 if the specified number of bits could not be
 * extracted due to EOD or other conditions.
 */
int32_t asn_get_few_bits(asn_bit_data_t *, int get_nbits);

/* Undo the immediately preceeding "get_few_bits" operation */
void asn_get_undo(asn_bit_data_t *, int get_nbits);

/*
 * Extract a large number of bits from the specified PER data pointer.
 * This function returns -1 if the specified number of bits could not be
 * extracted due to EOD or other conditions.
 */
int asn_get_many_bits(asn_bit_data_t *, uint8_t *dst, int right_align,
			int get_nbits);

/* Non-thread-safe debugging function, don't use it */
char *asn_bit_data_string(asn_bit_data_t *);

/*
 * This structure supports forming bit output.
 */
typedef struct asn_bit_outp_s {
	uint8_t *buffer;	/* Pointer into the (tmpspace) */
	size_t nboff;		/* Bit offset to the meaningful bit */
	size_t nbits;		/* Number of bits left in (tmpspace) */
	uint8_t tmpspace[32];	/* Preliminary storage to hold data */
	int (*output)(const void *data, size_t size, void *op_key);
	void *op_key;		/* Key for (output) data callback */
	size_t flushed_bytes;	/* Bytes already flushed through (output) */
} asn_bit_outp_t;

/* Output a small number of bits (<= 31) */
int asn_put_few_bits(asn_bit_outp_t *, uint32_t bits, int obits);

/* Output a large number of bits */
int asn_put_many_bits(asn_bit_outp_t *, const uint8_t *src, int put_nbits);

/*
 * Flush whole bytes (0 or more) through (outper) member.
 * The least significant bits which are not used are guaranteed to be set to 0.
 * Returns -1 if callback returns -1. Otherwise, 0.
 */
int asn_put_aligned_flush(asn_bit_outp_t *);

#ifdef __cplusplus
}
#endif

#endif	/* ASN_BIT_DATA */
