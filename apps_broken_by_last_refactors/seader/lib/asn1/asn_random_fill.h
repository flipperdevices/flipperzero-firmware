/*
 * Copyright (c) 2017 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
#ifndef	ASN_RANDOM_FILL
#define	ASN_RANDOM_FILL

/* Forward declarations */
struct asn_TYPE_descriptor_s;
struct asn_encoding_constraints_s;

/*
 * Initialize a structure with random data according to the type specification
 * and optional member constraints.
 * ARGUMENTS:
 *  (max_length)        - See (approx_max_length_limit).
 *  (memb_constraints)  - Member constraints, if exist.
 *                        The type can be constrained differently according
 *                        to PER and OER specifications, so we find a value
 *                        at the intersection of these constraints.
 * In case the return differs from ARFILL_OK, the (struct_ptr) contents
 * and (current_length) value remain in their original state.
 */
typedef struct asn_random_fill_result_s {
    enum {
        ARFILL_FAILED = -1, /* System error (memory?) */
        ARFILL_OK = 0,      /* Initialization succeeded */
        ARFILL_SKIPPED = 1  /* Not done due to (length?) constraint */
    } code;
    size_t length; /* Approximate number of bytes created. */
} asn_random_fill_result_t;
typedef asn_random_fill_result_t(asn_random_fill_f)(
    const struct asn_TYPE_descriptor_s *td, void **struct_ptr,
    const struct asn_encoding_constraints_s *memb_constraints,
    size_t max_length);

/*
 * Returns 0 if the structure was properly initialized, -1 otherwise.
 * The (approx_max_length_limit) specifies the approximate limit of the
 * resulting structure in units closely resembling bytes. The actual result
 * might be several times larger or smaller than the length limit.
 */
int asn_random_fill(const struct asn_TYPE_descriptor_s *td, void **struct_ptr,
                    size_t approx_max_length_limit);

/*
 * Returns a random number between min and max.
 */
intmax_t asn_random_between(intmax_t min, intmax_t max);

#endif	/* ASN_RANDOM_FILL */
