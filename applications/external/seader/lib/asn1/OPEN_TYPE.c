/*
 * Copyright (c) 2017 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
#include <asn_internal.h>
#include <OPEN_TYPE.h>
#include <constr_CHOICE.h>
#include <per_opentype.h>
#include <errno.h>

asn_TYPE_operation_t asn_OP_OPEN_TYPE = {
	OPEN_TYPE_free,
	OPEN_TYPE_print,
	OPEN_TYPE_compare,
	OPEN_TYPE_decode_ber,
	OPEN_TYPE_encode_der,
	OPEN_TYPE_decode_xer,
	OPEN_TYPE_encode_xer,
	0, 0,	/* No OER support, use "-gen-OER" to enable */
#ifdef ASN_DISABLE_PER_SUPPORT
	0, 0,
#else
	OPEN_TYPE_decode_uper,
	OPEN_TYPE_encode_uper,
#endif
	0,  /* Random fill is not supported for open type */
	0,	/* Use generic outmost tag fetcher */
};

#undef  ADVANCE
#define ADVANCE(num_bytes)               \
    do {                                 \
        size_t num = num_bytes;          \
        ptr = ((const char *)ptr) + num; \
        size -= num;                     \
        consumed_myself += num;          \
    } while(0)

asn_dec_rval_t
OPEN_TYPE_ber_get(const asn_codec_ctx_t *opt_codec_ctx,
                  const asn_TYPE_descriptor_t *td, void *sptr,
                  const asn_TYPE_member_t *elm, const void *ptr, size_t size) {
    size_t consumed_myself = 0;
    asn_type_selector_result_t selected;
    void *memb_ptr;   /* Pointer to the member */
    void **memb_ptr2; /* Pointer to that pointer */
    void *inner_value;
    asn_dec_rval_t rv;

    if(!(elm->flags & ATF_OPEN_TYPE)) {
        ASN__DECODE_FAILED;
    }

    if(!elm->type_selector) {
        ASN_DEBUG("Type selector is not defined for Open Type %s->%s->%s",
                  td->name, elm->name, elm->type->name);
        ASN__DECODE_FAILED;
    }

    selected = elm->type_selector(td, sptr);
    if(!selected.presence_index) {
        ASN__DECODE_FAILED;
    }

    /* Fetch the pointer to this member */
    if(elm->flags & ATF_POINTER) {
        memb_ptr2 = (void **)((char *)sptr + elm->memb_offset);
    } else {
        memb_ptr = (char *)sptr + elm->memb_offset;
        memb_ptr2 = &memb_ptr;
    }
    if(*memb_ptr2 != NULL) {
        /* Make sure we reset the structure first before encoding */
        if(CHOICE_variant_set_presence(elm->type, *memb_ptr2, 0) != 0) {
            ASN__DECODE_FAILED;
        }
    }

    inner_value =
        (char *)*memb_ptr2
        + elm->type->elements[selected.presence_index - 1].memb_offset;

    ASN_DEBUG("presence %d\n", selected.presence_index);

    rv = selected.type_descriptor->op->ber_decoder(
        opt_codec_ctx, selected.type_descriptor, &inner_value, ptr, size,
        elm->tag_mode);
    ADVANCE(rv.consumed);
    rv.consumed = 0;
    switch(rv.code) {
    case RC_OK:
        if(CHOICE_variant_set_presence(elm->type, *memb_ptr2,
                                       selected.presence_index)
           == 0) {
            rv.code = RC_OK;
            rv.consumed = consumed_myself;
            return rv;
        } else {
            /* Oh, now a full-blown failure failure */
        }
        /* Fall through */
    case RC_FAIL:
        rv.consumed = consumed_myself;
        /* Fall through */
    case RC_WMORE:
        break;
    }

    if(*memb_ptr2) {
        const asn_CHOICE_specifics_t *specs =
            selected.type_descriptor->specifics;
        if(elm->flags & ATF_POINTER) {
            ASN_STRUCT_FREE(*selected.type_descriptor, inner_value);
            *memb_ptr2 = NULL;
        } else {
            ASN_STRUCT_FREE_CONTENTS_ONLY(*selected.type_descriptor,
                                          inner_value);
            memset(*memb_ptr2, 0, specs->struct_size);
        }
    }
    return rv;
}

asn_dec_rval_t
OPEN_TYPE_xer_get(const asn_codec_ctx_t *opt_codec_ctx,
                  const asn_TYPE_descriptor_t *td, void *sptr,
                  const asn_TYPE_member_t *elm, const void *ptr, size_t size) {
    size_t consumed_myself = 0;
    asn_type_selector_result_t selected;
    void *memb_ptr;   /* Pointer to the member */
    void **memb_ptr2; /* Pointer to that pointer */
    void *inner_value;
    asn_dec_rval_t rv;

    int xer_context = 0;
    ssize_t ch_size;
    pxer_chunk_type_e ch_type;

    if(!(elm->flags & ATF_OPEN_TYPE)) {
        ASN__DECODE_FAILED;
    }

    if(!elm->type_selector) {
        ASN_DEBUG("Type selector is not defined for Open Type %s->%s->%s",
                  td->name, elm->name, elm->type->name);
        ASN__DECODE_FAILED;
    }

    selected = elm->type_selector(td, sptr);
    if(!selected.presence_index) {
        ASN__DECODE_FAILED;
    }

    /* Fetch the pointer to this member */
    assert(elm->flags == ATF_OPEN_TYPE);
    if(elm->flags & ATF_POINTER) {
        memb_ptr2 = (void **)((char *)sptr + elm->memb_offset);
    } else {
        memb_ptr = (char *)sptr + elm->memb_offset;
        memb_ptr2 = &memb_ptr;
    }
    if(*memb_ptr2 != NULL) {
        /* Make sure we reset the structure first before encoding */
        if(CHOICE_variant_set_presence(elm->type, *memb_ptr2, 0)
           != 0) {
            ASN__DECODE_FAILED;
        }
    }

    /*
     * Confirm wrapper.
     */
    for(;;) {
        ch_size = xer_next_token(&xer_context, ptr, size, &ch_type);
        if(ch_size < 0) {
            ASN__DECODE_FAILED;
        } else {
            switch(ch_type) {
            case PXER_WMORE:
                ASN__DECODE_STARVED;
            case PXER_COMMENT:
            case PXER_TEXT:
                ADVANCE(ch_size);
                continue;
            case PXER_TAG:
                break;
            }
            break;
        }
    }

    /*
     * Wrapper value confirmed.
     */
    switch(xer_check_tag(ptr, ch_size, elm->name)) {
    case XCT_OPENING:
        ADVANCE(ch_size);
        break;
    case XCT_BROKEN:
    default:
        ASN__DECODE_FAILED;
    }

    inner_value =
        (char *)*memb_ptr2
        + elm->type->elements[selected.presence_index - 1].memb_offset;

    rv = selected.type_descriptor->op->xer_decoder(
        opt_codec_ctx, selected.type_descriptor, &inner_value, NULL, ptr, size);
    ADVANCE(rv.consumed);
    rv.consumed = 0;
    switch(rv.code) {
    case RC_OK:
        if(CHOICE_variant_set_presence(elm->type, *memb_ptr2,
                                       selected.presence_index)
           == 0) {
            break;
        } else {
            rv.code = RC_FAIL;
        }
        /* Fall through */
    case RC_FAIL:
        /* Point to a best position where failure occurred */
        rv.consumed = consumed_myself;
        /* Fall through */
    case RC_WMORE:
        /* Wrt. rv.consumed==0:
         * In case a genuine RC_WMORE, the whole Open Type decoding
         * will have to be restarted.
         */
        if(*memb_ptr2) {
            const asn_CHOICE_specifics_t *specs =
                selected.type_descriptor->specifics;
            if(elm->flags & ATF_POINTER) {
                ASN_STRUCT_FREE(*selected.type_descriptor, inner_value);
                *memb_ptr2 = NULL;
            } else {
                ASN_STRUCT_FREE_CONTENTS_ONLY(*selected.type_descriptor,
                                              inner_value);
                memset(*memb_ptr2, 0, specs->struct_size);
            }
        }
        return rv;
    }

    /*
     * Finalize wrapper.
     */
    for(;;) {
        ch_size = xer_next_token(&xer_context, ptr, size, &ch_type);
        if(ch_size < 0) {
            ASN__DECODE_FAILED;
        } else {
            switch(ch_type) {
            case PXER_WMORE:
                ASN__DECODE_STARVED;
            case PXER_COMMENT:
            case PXER_TEXT:
                ADVANCE(ch_size);
                continue;
            case PXER_TAG:
                break;
            }
            break;
        }
    }

    /*
     * Wrapper value confirmed.
     */
    switch(xer_check_tag(ptr, ch_size, elm->name)) {
    case XCT_CLOSING:
        ADVANCE(ch_size);
        break;
    case XCT_BROKEN:
    default:
        ASN__DECODE_FAILED;
    }

    rv.consumed += consumed_myself;

    return rv;
}


#ifndef  ASN_DISABLE_PER_SUPPORT

asn_dec_rval_t
OPEN_TYPE_uper_get(const asn_codec_ctx_t *opt_codec_ctx,
                   const asn_TYPE_descriptor_t *td, void *sptr,
                   const asn_TYPE_member_t *elm, asn_per_data_t *pd) {
    asn_type_selector_result_t selected;
    void *memb_ptr;   /* Pointer to the member */
    void **memb_ptr2; /* Pointer to that pointer */
    void *inner_value;
    asn_dec_rval_t rv;

    if(!(elm->flags & ATF_OPEN_TYPE)) {
        ASN__DECODE_FAILED;
    }

    if(!elm->type_selector) {
        ASN_DEBUG("Type selector is not defined for Open Type %s->%s->%s",
                  td->name, elm->name, elm->type->name);
        ASN__DECODE_FAILED;
    }

    selected = elm->type_selector(td, sptr);
    if(!selected.presence_index) {
        ASN__DECODE_FAILED;
    }

    /* Fetch the pointer to this member */
    assert(elm->flags == ATF_OPEN_TYPE);
    if(elm->flags & ATF_POINTER) {
        memb_ptr2 = (void **)((char *)sptr + elm->memb_offset);
    } else {
        memb_ptr = (char *)sptr + elm->memb_offset;
        memb_ptr2 = &memb_ptr;
    }
    if(*memb_ptr2 != NULL) {
        /* Make sure we reset the structure first before encoding */
        if(CHOICE_variant_set_presence(elm->type, *memb_ptr2, 0)
           != 0) {
            ASN__DECODE_FAILED;
        }
    }

    inner_value =
        (char *)*memb_ptr2
        + elm->type->elements[selected.presence_index - 1].memb_offset;

    rv = uper_open_type_get(opt_codec_ctx, selected.type_descriptor, NULL,
                            &inner_value, pd);
    switch(rv.code) {
    case RC_OK:
        if(CHOICE_variant_set_presence(elm->type, *memb_ptr2,
                                       selected.presence_index)
           == 0) {
            break;
        } else {
            rv.code = RC_FAIL;
        }
        /* Fall through */
    case RC_WMORE:
    case RC_FAIL:
        if(*memb_ptr2) {
            const asn_CHOICE_specifics_t *specs =
                selected.type_descriptor->specifics;
            if(elm->flags & ATF_POINTER) {
                ASN_STRUCT_FREE(*selected.type_descriptor, inner_value);
                *memb_ptr2 = NULL;
            } else {
                ASN_STRUCT_FREE_CONTENTS_ONLY(*selected.type_descriptor,
                                              inner_value);
                memset(*memb_ptr2, 0, specs->struct_size);
            }
        }
    }
    return rv;
}

asn_enc_rval_t
OPEN_TYPE_encode_uper(const asn_TYPE_descriptor_t *td,
                      const asn_per_constraints_t *constraints,
                      const void *sptr, asn_per_outp_t *po) {
    const void *memb_ptr;   /* Pointer to the member */
    asn_TYPE_member_t *elm; /* CHOICE's element */
    asn_enc_rval_t er;
    unsigned present;

    (void)constraints;

    present = CHOICE_variant_get_presence(td, sptr);
    if(present == 0 || present > td->elements_count) {
        ASN__ENCODE_FAILED;
    } else {
        present--;
    }

    ASN_DEBUG("Encoding %s OPEN TYPE element %d", td->name, present);

    elm = &td->elements[present];
    if(elm->flags & ATF_POINTER) {
        /* Member is a pointer to another structure */
        memb_ptr =
            *(const void *const *)((const char *)sptr + elm->memb_offset);
        if(!memb_ptr) ASN__ENCODE_FAILED;
    } else {
        memb_ptr = (const char *)sptr + elm->memb_offset;
    }

    if(uper_open_type_put(elm->type, NULL, memb_ptr, po) < 0) {
        ASN__ENCODE_FAILED;
    }

    er.encoded = 0;
    ASN__ENCODED_OK(er);
}


#endif  /* ASN_DISABLE_PER_SUPPORT */
