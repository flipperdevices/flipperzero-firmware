/*
 * Copyright (c) 2017 Lev Walkin <vlm@lionet.info>.
 * All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
#ifndef ASN_DISABLE_OER_SUPPORT

#include <asn_internal.h>
#include <constr_CHOICE.h>
#include <errno.h>

/*
 * Return a standardized complex structure.
 */
#undef  RETURN
#define RETURN(_code)                    \
    do {                                 \
        asn_dec_rval_t rval;             \
        rval.code = _code;               \
        rval.consumed = consumed_myself; \
        return rval;                     \
    } while(0)

#undef  ADVANCE
#define ADVANCE(num_bytes)               \
    do {                                 \
        size_t num = num_bytes;          \
        ptr = ((const char *)ptr) + num; \
        size -= num;                     \
        consumed_myself += num;          \
    } while(0)

/*
 * Switch to the next phase of parsing.
 */
#undef  NEXT_PHASE
#define NEXT_PHASE(ctx) \
    do {                \
        ctx->phase++;   \
        ctx->step = 0;  \
    } while(0)
#undef  SET_PHASE
#define SET_PHASE(ctx, value) \
    do {                      \
        ctx->phase = value;   \
        ctx->step = 0;        \
    } while(0)

/*
 * Tags are canonically sorted in the tag to member table.
 */
static int
_search4tag(const void *ap, const void *bp) {
    const asn_TYPE_tag2member_t *a = (const asn_TYPE_tag2member_t *)ap;
    const asn_TYPE_tag2member_t *b = (const asn_TYPE_tag2member_t *)bp;

    int a_class = BER_TAG_CLASS(a->el_tag);
    int b_class = BER_TAG_CLASS(b->el_tag);

    if(a_class == b_class) {
        ber_tlv_tag_t a_value = BER_TAG_VALUE(a->el_tag);
        ber_tlv_tag_t b_value = BER_TAG_VALUE(b->el_tag);

        if(a_value == b_value)
            return 0;
        else if(a_value < b_value)
            return -1;
        else
            return 1;
    } else if(a_class < b_class) {
        return -1;
    } else {
        return 1;
    }
}

/*
 * X.696 (08/2015) #8.7 Encoding of tags
 */
static ssize_t
oer_fetch_tag(const void *ptr, size_t size, ber_tlv_tag_t *tag_r) {
    ber_tlv_tag_t val;
    ber_tlv_tag_t tclass;
    size_t skipped;

    if(size == 0)
        return 0;

    val = *(const uint8_t *)ptr;
    tclass = (val >> 6);
    if((val & 0x3F) != 0x3F) {
        /* #8.7.1 */
        *tag_r = ((val & 0x3F) << 2) | tclass;
        return 1;
    }

    /*
     * Each octet contains 7 bits of useful information.
     * The MSB is 0 if it is the last octet of the tag.
     */
    for(val = 0, ptr = ((const char *)ptr) + 1, skipped = 2; skipped <= size;
        ptr = ((const char *)ptr) + 1, skipped++) {
        unsigned int oct = *(const uint8_t *)ptr;
        if(oct & 0x80) {
            val = (val << 7) | (oct & 0x7F);
            /*
             * Make sure there are at least 9 bits spare
             * at the MS side of a value.
             */
            if(val >> ((8 * sizeof(val)) - 9)) {
                /*
                 * We would not be able to accomodate
                 * any more tag bits.
                 */
                return -1;
            }
        } else {
            val = (val << 7) | oct;
            *tag_r = (val << 2) | tclass;
            return skipped;
        }
    }

    return 0; /* Want more */
}

asn_dec_rval_t
CHOICE_decode_oer(const asn_codec_ctx_t *opt_codec_ctx,
                  const asn_TYPE_descriptor_t *td,
                  const asn_oer_constraints_t *constraints, void **struct_ptr,
                  const void *ptr, size_t size) {
    /*
     * Bring closer parts of structure description.
     */
    const asn_CHOICE_specifics_t *specs =
        (const asn_CHOICE_specifics_t *)td->specifics;
    asn_TYPE_member_t *elements = td->elements;

    /*
     * Parts of the structure being constructed.
     */
    void *st = *struct_ptr; /* Target structure. */
    asn_struct_ctx_t *ctx;  /* Decoder context */

    ssize_t consumed_myself = 0; /* Consumed bytes from ptr */

    (void)constraints;

    ASN_DEBUG("Decoding %s as CHOICE", td->name);

    /*
     * Create the target structure if it is not present already.
     */
    if(st == 0) {
        st = *struct_ptr = CALLOC(1, specs->struct_size);
        if(st == 0) {
            RETURN(RC_FAIL);
        }
    }

    /*
     * Restore parsing context.
     */
    ctx = (asn_struct_ctx_t *)((char *)st + specs->ctx_offset);
    switch(ctx->phase) {
    case 0: {
        /*
         * Discover the tag.
         */
        ber_tlv_tag_t tlv_tag; /* T from TLV */
        ssize_t tag_len;       /* Length of TLV's T */

        tag_len = oer_fetch_tag(ptr, size, &tlv_tag);
        switch(tag_len) {
        case 0:
            ASN__DECODE_STARVED;
        case -1:
            ASN__DECODE_FAILED;
        }

        do {
            const asn_TYPE_tag2member_t *t2m;
            asn_TYPE_tag2member_t key = {0, 0, 0, 0};
            key.el_tag = tlv_tag;

            t2m = (const asn_TYPE_tag2member_t *)bsearch(
                &key, specs->tag2el, specs->tag2el_count,
                sizeof(specs->tag2el[0]), _search4tag);
            if(t2m) {
                /*
                 * Found the element corresponding to the tag.
                 */
                NEXT_PHASE(ctx);
                ctx->step = t2m->el_no;
                break;
            } else if(specs->ext_start == -1) {
                ASN_DEBUG(
                    "Unexpected tag %s "
                    "in non-extensible CHOICE %s",
                    ber_tlv_tag_string(tlv_tag), td->name);
                RETURN(RC_FAIL);
            } else {
                /* Skip open type extension */
                ASN_DEBUG(
                    "Not implemented skipping open type extension for tag %s",
                    ber_tlv_tag_string(tlv_tag));
                RETURN(RC_FAIL);
            }
        } while(0);


        ADVANCE(tag_len);
    }
        /* Fall through */
    case 1: {
        asn_TYPE_member_t *elm = &elements[ctx->step]; /* CHOICE's element */
        void *memb_ptr;         /* Pointer to the member */
        void **memb_ptr2;       /* Pointer to that pointer */
        asn_dec_rval_t rval;

        /*
         * Compute the position of the member inside a structure,
         * and also a type of containment (it may be contained
         * as pointer or using inline inclusion).
         */
        if(elm->flags & ATF_POINTER) {
            /* Member is a pointer to another structure */
            memb_ptr2 = (void **)((char *)st + elm->memb_offset);
        } else {
            /*
             * A pointer to a pointer
             * holding the start of the structure
             */
            memb_ptr = (char *)st + elm->memb_offset;
            memb_ptr2 = &memb_ptr;
        }

        /* Set presence to be able to free it properly at any time */
        (void)CHOICE_variant_set_presence(td, st, ctx->step + 1);

        if(specs->ext_start >= 0 && specs->ext_start <= ctx->step) {
            ssize_t got =
                oer_open_type_get(opt_codec_ctx, elm->type,
                                  elm->encoding_constraints.oer_constraints,
                                  memb_ptr2, ptr, size);
            if(got < 0) ASN__DECODE_FAILED;
            if(got == 0) ASN__DECODE_STARVED;
            rval.code = RC_OK;
            rval.consumed = got;
        } else {
            rval = elm->type->op->oer_decoder(
                opt_codec_ctx, elm->type,
                elm->encoding_constraints.oer_constraints, memb_ptr2, ptr,
                size);
        }
        rval.consumed += consumed_myself;
        switch(rval.code) {
        case RC_OK:
            NEXT_PHASE(ctx);
        case RC_WMORE:
            break;
        case RC_FAIL:
            SET_PHASE(ctx, 3);  /* => 3 */
        }
        return rval;
    }
    case 2:
        /* Already decoded everything */
        RETURN(RC_OK);
    case 3:
        /* Failed to decode, after all */
        RETURN(RC_FAIL);
    }

    RETURN(RC_FAIL);
}

/*
 * X.696 (08/2015) #8.7 Encoding of tags
 */
static ssize_t
oer_put_tag(ber_tlv_tag_t tag, asn_app_consume_bytes_f *cb, void *app_key) {
    uint8_t tclass = BER_TAG_CLASS(tag);
    ber_tlv_tag_t tval = BER_TAG_VALUE(tag);

    if(tval < 0x3F) {
        uint8_t b = (uint8_t)((tclass << 6) | tval);
        if(cb(&b, 1, app_key) < 0) {
            return -1;
        }
        return 1;
    } else {
        uint8_t buf[1 + 2 * sizeof(tval)];
        uint8_t *b = &buf[sizeof(buf)-1]; /* Last addressable */
        size_t encoded;
        for(; ; tval >>= 7) {
            if(tval >> 7) {
                *b-- = 0x80 | (tval & 0x7f);
            } else {
                *b-- = tval & 0x7f;
                break;
            }
        }
        *b = (uint8_t)((tclass << 6) | 0x3F);
        encoded = sizeof(buf) - (b - buf);
        if(cb(b, encoded, app_key) < 0) {
            return -1;
        }
        return encoded;
    }

}

/*
 * Encode as Canonical OER.
 */
asn_enc_rval_t
CHOICE_encode_oer(const asn_TYPE_descriptor_t *td,
                  const asn_oer_constraints_t *constraints, const void *sptr,
                  asn_app_consume_bytes_f *cb, void *app_key) {
    const asn_CHOICE_specifics_t *specs =
        (const asn_CHOICE_specifics_t *)td->specifics;
    asn_TYPE_member_t *elm; /* CHOICE element */
    unsigned present;
    const void *memb_ptr;
    ber_tlv_tag_t tag;
    ssize_t tag_len;
    asn_enc_rval_t er = {0, 0, 0};

    (void)constraints;

    if(!sptr) ASN__ENCODE_FAILED;

    ASN_DEBUG("OER %s encoding as CHOICE", td->name);

    present = CHOICE_variant_get_presence(td, sptr);
    if(present == 0 || present > td->elements_count) {
        ASN_DEBUG("CHOICE %s member is not selected", td->name);
        ASN__ENCODE_FAILED;
    }

    elm = &td->elements[present-1];
    if(elm->flags & ATF_POINTER) {
        memb_ptr =
            *(const void *const *)((const char *)sptr + elm->memb_offset);
        if(memb_ptr == 0) {
            /* Mandatory element absent */
            ASN__ENCODE_FAILED;
        }
    } else {
        memb_ptr = (const void *)((const char *)sptr + elm->memb_offset);
    }

    tag = asn_TYPE_outmost_tag(elm->type, memb_ptr, elm->tag_mode, elm->tag);
    if(tag == 0) {
        ASN__ENCODE_FAILED;
    }

    tag_len = oer_put_tag(tag, cb, app_key);
    if(tag_len < 0) {
        ASN__ENCODE_FAILED;
    }

    if(specs->ext_start >= 0 && (unsigned)specs->ext_start <= (present-1)) {
        ssize_t encoded = oer_open_type_put(elm->type,
                               elm->encoding_constraints.oer_constraints,
                               memb_ptr, cb, app_key);
        if(encoded < 0) ASN__ENCODE_FAILED;
        er.encoded = tag_len + encoded;
    } else {
        er = elm->type->op->oer_encoder(
            elm->type, elm->encoding_constraints.oer_constraints, memb_ptr, cb,
            app_key);
        if(er.encoded >= 0) er.encoded += tag_len;
    }

    return er;
}

#endif  /* ASN_DISABLE_OER_SUPPORT */
