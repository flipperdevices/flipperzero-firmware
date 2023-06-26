/*
 * Copyright (c) 2017 Lev Walkin <vlm@lionet.info>.
 * All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
#ifndef ASN_DISABLE_OER_SUPPORT

#include <asn_internal.h>
#include <constr_SEQUENCE.h>
#include <OPEN_TYPE.h>
#include <errno.h>

/*
 * This macro "eats" the part of the buffer which is definitely "consumed",
 * i.e. was correctly converted into local representation or rightfully skipped.
 */
#undef  ADVANCE
#define ADVANCE(num_bytes)                   \
    do {                                     \
        size_t num = num_bytes;              \
        ptr = ((const char *)ptr) + num;     \
        size -= num;                         \
        consumed_myself += num;              \
    } while(0)

/*
 * Switch to the next phase of parsing.
 */
#undef  NEXT_PHASE
#undef PHASE_OUT
#define NEXT_PHASE(ctx) \
    do {                \
        ctx->phase++;   \
        ctx->step = 0;  \
    } while(0)

/*
 * Check whether we are inside the extensions group.
 */
#define IN_EXTENSION_GROUP(specs, memb_idx) \
    ((specs)->first_extension >= 0          \
     && (unsigned)(specs)->first_extension <= (memb_idx))

#define IN_ROOT_GROUP_PRED(edx)                            \
    edx < (specs->first_extension < 0 ? td->elements_count \
                                      : (size_t)specs->first_extension)

#define FOR_IN_ROOT_GROUP(edx) for(edx = 0; IN_ROOT_GROUP_PRED(edx); edx++)

/*
 * Return a standardized complex structure.
 */
#undef  RETURN
#define RETURN(_code)   do {                    \
                rval.code = _code;              \
                rval.consumed = consumed_myself;\
                return rval;                    \
        } while(0)

/*
 * Return pointer to a member.
 */
static void **
element_ptrptr(void *struct_ptr, asn_TYPE_member_t *elm, void **tmp_save_ptr) {
    if(elm->flags & ATF_POINTER) {
        /* Member is a pointer to another structure */
        return (void **)((char *)struct_ptr + elm->memb_offset);
    } else {
        assert(tmp_save_ptr);
        *tmp_save_ptr = (void *)((char *)struct_ptr + elm->memb_offset);
        return tmp_save_ptr;
    }
}

static const void *
element_ptr(const void *struct_ptr, const asn_TYPE_member_t *elm) {
    if(elm->flags & ATF_POINTER) {
        /* Member is a pointer to another structure */
        return *(const void *const *)((const char *)struct_ptr
                                      + elm->memb_offset);
    } else {
        return (const void *)((const char *)struct_ptr + elm->memb_offset);
    }
}

asn_dec_rval_t
SEQUENCE_decode_oer(const asn_codec_ctx_t *opt_codec_ctx,
                    const asn_TYPE_descriptor_t *td,
                    const asn_oer_constraints_t *constraints, void **struct_ptr,
                    const void *ptr, size_t size) {
    const asn_SEQUENCE_specifics_t *specs =
        (const asn_SEQUENCE_specifics_t *)td->specifics;
    asn_dec_rval_t rval = {RC_OK, 0};
    void *st = *struct_ptr; /* Target structure */
    asn_struct_ctx_t *ctx; /* Decoder context */
    size_t consumed_myself = 0; /* Consumed bytes from ptr. */

    (void)constraints;

    if(ASN__STACK_OVERFLOW_CHECK(opt_codec_ctx))
        ASN__DECODE_FAILED;

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

    /*
     * Start to parse where left previously.
     */
    switch(ctx->phase) {
    case 0: {
        /*
         * Fetch preamble.
         */
        asn_bit_data_t *preamble;
        int has_extensions_bit = (specs->first_extension >= 0);
        size_t preamble_bits = (has_extensions_bit + specs->roms_count);
        size_t preamble_bytes = ((7 + preamble_bits) >> 3);

        ASN_DEBUG("OER SEQUENCE %s Decoding PHASE 0", td->name);

        ASN_DEBUG(
            "Expecting preamble bits %" ASN_PRI_SIZE " for %s (including %d extension bits)",
            preamble_bits, td->name, has_extensions_bit);

        if(preamble_bytes > size) {
            ASN__DECODE_STARVED;
        }

        preamble = asn_bit_data_new_contiguous(ptr, preamble_bits);
        if(!preamble) {
            RETURN(RC_FAIL);
        }
        preamble->nboff = has_extensions_bit;
        ctx->ptr = preamble;
        ADVANCE(preamble_bytes);
    }
        NEXT_PHASE(ctx);
        /* FALL THROUGH */
    case 1: {
        /* Decode components of the extension root */
        asn_bit_data_t *preamble = ctx->ptr;
        size_t edx;

        ASN_DEBUG("OER SEQUENCE %s Decoding PHASE 1 (Root)", td->name);

        assert(preamble);

        for(edx = (ctx->step >> 1); IN_ROOT_GROUP_PRED(edx);
            edx++, ctx->step = (ctx->step & ~1) + 2) {
            asn_TYPE_member_t *elm = &td->elements[edx];

            ASN_DEBUG("Decoding %s->%s", td->name, elm->name);

            assert(!IN_EXTENSION_GROUP(specs, edx));

            if(ctx->step & 1) {
                goto microphase2_decode_continues;
            }


            if(elm->optional) {
                int32_t present = asn_get_few_bits(preamble, 1);
                if(present < 0) {
                    ASN_DEBUG("Presence map ended prematurely: %d", present);
                    RETURN(RC_FAIL);
                } else if(present == 0) {
                    if(elm->default_value_set) {
                        /* Fill-in DEFAULT */
                        void *tmp;
                        if(elm->default_value_set(
                               element_ptrptr(st, elm, &tmp))) {
                            RETURN(RC_FAIL);
                        }
                    }
                    /* The member is not present. */
                    continue;
                }
                /* Present OPTIONAL or DEFAULT component. */
            }

            /*
             * MICROPHASE 2: Invoke the member-specific decoder.
             */
            ctx->step |= 1; /* Confirm entering next microphase */
        microphase2_decode_continues:
            if(elm->flags & ATF_OPEN_TYPE) {
                rval = OPEN_TYPE_oer_get(opt_codec_ctx, td, st, elm, ptr, size);
            } else {
                void *save_memb_ptr; /* Temporary reference. */
                void **memb_ptr2;  /* Pointer to a pointer to a memmber */

                memb_ptr2 = element_ptrptr(st, elm, &save_memb_ptr);

                rval = elm->type->op->oer_decoder(
                    opt_codec_ctx, elm->type,
                    elm->encoding_constraints.oer_constraints, memb_ptr2, ptr,
                    size);
            }
            switch(rval.code) {
            case RC_OK:
                ADVANCE(rval.consumed);
                break;
            case RC_WMORE:
                ASN_DEBUG("More bytes needed at element %s \"%s\"", td->name,
                          elm->name);
                ADVANCE(rval.consumed);
                RETURN(RC_WMORE);
            case RC_FAIL:
                ASN_DEBUG("Decoding failed at element %s \"%s\"", td->name,
                          elm->name);
                RETURN(RC_FAIL);
            }
        } /* for(all root members) */

    }
        NEXT_PHASE(ctx);
        /* FALL THROUGH */
    case 2:
        assert(ctx->ptr);
        {
        /* Cleanup preamble. */
        asn_bit_data_t *preamble = ctx->ptr;
        asn_bit_data_t *extadds;
        int has_extensions_bit = (specs->first_extension >= 0);
        int extensions_present =
            has_extensions_bit
            && (preamble->buffer == NULL
                || (((const uint8_t *)preamble->buffer)[0] & 0x80));
        uint8_t unused_bits;
        size_t len = 0;
        ssize_t len_len;

        ASN_DEBUG("OER SEQUENCE %s Decoding PHASE 2", td->name);

        preamble->buffer = 0; /* Will do extensions_present==1 next time. */

        if(!extensions_present) {
            ctx->phase = 10;
            RETURN(RC_OK);
        }

        /*
         * X.696 (08/2015) #16.1 (c), #16.4
         * Read in the extension addition presence bitmap.
         */

        len_len = oer_fetch_length(ptr, size, &len);
        if(len_len > 0) {
            ADVANCE(len_len);
        } else if(len_len < 0) {
            RETURN(RC_FAIL);
        } else {
            RETURN(RC_WMORE);
        }

        if(len == 0) {
            /* 16.4.1-2 */
            RETURN(RC_FAIL);
        } else if(len > size) {
            RETURN(RC_WMORE);
        }

        /* Account for unused bits */
        unused_bits = 0x7 & *(const uint8_t *)ptr;
        ADVANCE(1);
        len--;
        if(unused_bits && len == 0) {
            RETURN(RC_FAIL);
        }

        /* Get the extensions map */
        extadds = asn_bit_data_new_contiguous(ptr, len * 8 - unused_bits);
        if(!extadds) {
            RETURN(RC_FAIL);
        }
        FREEMEM(preamble);
        ctx->ptr = extadds;
        ADVANCE(len);
    }
        NEXT_PHASE(ctx);
        ctx->step =
            (specs->first_extension < 0 ? td->elements_count
                                        : (size_t)specs->first_extension);
        /* Fall through */
    case 3:
        ASN_DEBUG("OER SEQUENCE %s Decoding PHASE 3 (Extensions)", td->name);
        for(; ctx->step < (signed)td->elements_count; ctx->step++) {
            asn_bit_data_t *extadds = ctx->ptr;
            size_t edx = ctx->step;
            asn_TYPE_member_t *elm = &td->elements[edx];
            void *tmp_memb_ptr;
            void **memb_ptr2 = element_ptrptr(st, elm, &tmp_memb_ptr);

            switch(asn_get_few_bits(extadds, 1)) {
            case -1:
                /*
                 * Not every one of our extensions is known to the remote side.
                 * Continue filling in their defaults though.
                 */
                /* Fall through */
            case 0:
                /* Fill-in DEFAULT */
                if(elm->default_value_set
                   && elm->default_value_set(memb_ptr2)) {
                    RETURN(RC_FAIL);
                }
                continue;
            case 1: {
                /* Read OER open type */
                ssize_t ot_size =
                    oer_open_type_get(opt_codec_ctx, elm->type,
                                      elm->encoding_constraints.oer_constraints,
                                      memb_ptr2, ptr, size);
                assert(ot_size <= (ssize_t)size);
                if(ot_size > 0) {
                    ADVANCE(ot_size);
                } else if(ot_size < 0) {
                    RETURN(RC_FAIL);
                } else {
                    /* Roll back open type parsing */
                    asn_get_undo(extadds, 1);
                    RETURN(RC_WMORE);
                }
                break;
            }
            default:
                RETURN(RC_FAIL);
            }
        }

        NEXT_PHASE(ctx);
        /* Fall through */
    case 4:
        ASN_DEBUG("OER SEQUENCE %s Decoding PHASE 4", td->name);
        /* Read in the rest of Open Types while ignoring them */
        for(;;) {
            asn_bit_data_t *extadds = ctx->ptr;
            switch(asn_get_few_bits(extadds, 1)) {
            case 0:
                continue;
            case 1: {
                ssize_t skipped = oer_open_type_skip(ptr, size);
                if(skipped > 0) {
                    ADVANCE(skipped);
                } else if(skipped < 0) {
                    RETURN(RC_FAIL);
                } else {
                    asn_get_undo(extadds, 1);
                    RETURN(RC_WMORE);
                }
                continue;
            }
            case -1:
                /* No more Open Type encoded components */
                break;
            default:
                RETURN(RC_FAIL);
            }
            break;
        }
    }

    RETURN(RC_OK);
}

/*
 * Encode as Canonical OER.
 */
asn_enc_rval_t
SEQUENCE_encode_oer(const asn_TYPE_descriptor_t *td,
                    const asn_oer_constraints_t *constraints, const void *sptr,
                    asn_app_consume_bytes_f *cb, void *app_key) {
    const asn_SEQUENCE_specifics_t *specs = (const asn_SEQUENCE_specifics_t *)td->specifics;
    size_t computed_size = 0;
    int has_extensions_bit = (specs->first_extension >= 0);
    size_t preamble_bits = (has_extensions_bit + specs->roms_count);
    uint32_t has_extensions = 0;
    size_t edx;
    int ret;

    (void)constraints;

    if(preamble_bits) {
        asn_bit_outp_t preamble;

        memset(&preamble, 0, sizeof(preamble));
        preamble.output = cb;
        preamble.op_key = app_key;

        if(has_extensions_bit) {
            for(edx = specs->first_extension; edx < td->elements_count; edx++) {
                asn_TYPE_member_t *elm = &td->elements[edx];
                const void *memb_ptr = element_ptr(sptr, elm);
                if(memb_ptr) {
                    if(elm->default_value_cmp
                       && elm->default_value_cmp(memb_ptr) == 0) {
                        /* Do not encode default values in extensions */
                    } else {
                        has_extensions = 1;
                        break;
                    }
                }
            }
            ret = asn_put_few_bits(&preamble, has_extensions, 1);
            assert(ret == 0);
            if(ret < 0) {
                ASN__ENCODE_FAILED;
            }
        }

        /*
         * Encode optional components bitmap.
         */
        if(specs->roms_count) {
            FOR_IN_ROOT_GROUP(edx) {
                asn_TYPE_member_t *elm = &td->elements[edx];

                if(IN_EXTENSION_GROUP(specs, edx)) break;

                if(elm->optional) {
                    const void *memb_ptr = element_ptr(sptr, elm);
                    uint32_t has_component = memb_ptr != NULL;
                    if(has_component && elm->default_value_cmp
                       && elm->default_value_cmp(memb_ptr) == 0) {
                        has_component = 0;
                    }
                    ret = asn_put_few_bits(&preamble, has_component, 1);
                    if(ret < 0) {
                        ASN__ENCODE_FAILED;
                    }
                }
            }
        }

        asn_put_aligned_flush(&preamble);
        computed_size += preamble.flushed_bytes;
    }   /* if(preamble_bits) */

    /*
     * Put root components and extensions root.
     */
    for(edx = 0; edx < td->elements_count; edx++) {
        asn_TYPE_member_t *elm = &td->elements[edx];
        asn_enc_rval_t er;
        const void *memb_ptr;

        if(IN_EXTENSION_GROUP(specs, edx)) break;

        memb_ptr = element_ptr(sptr, elm);
        if(memb_ptr) {
            if(elm->default_value_cmp
               && elm->default_value_cmp(memb_ptr) == 0) {
                /* Skip default values in encoding */
                continue;
            }
        } else {
            if(elm->optional) continue;
            /* Mandatory element is missing */
            ASN__ENCODE_FAILED;
        }
        if(!elm->type->op->oer_encoder) {
            ASN_DEBUG("OER encoder is not defined for type %s", elm->type->name);
            ASN__ENCODE_FAILED;
        }
        er = elm->type->op->oer_encoder(
            elm->type, elm->encoding_constraints.oer_constraints, memb_ptr, cb,
            app_key);
        if(er.encoded == -1) {
            ASN_DEBUG("... while encoding %s member \"%s\"\n", td->name,
                      elm->name);
            return er;
        }
        computed_size += er.encoded;
    }

    /*
     * Before encode extensions, encode extensions additions presense bitmap
     # X.696 (08/2015) #16.4.
     */
    if(has_extensions) {
        asn_bit_outp_t extadds;

        /* Special case allowing us to use exactly one byte for #8.6 */
        size_t aoms_length_bits = specs->aoms_count;
        size_t aoms_length_bytes = (7 + aoms_length_bits) >> 3;
        uint8_t unused_bits = 0x07 & (8 - (aoms_length_bits & 0x07));

        assert(1 + aoms_length_bytes <= 127);

        memset(&extadds, 0, sizeof(extadds));
        extadds.output = cb;
        extadds.op_key = app_key;

        /* #8.6 length determinant */
        ret = asn_put_few_bits(&extadds, (1 + aoms_length_bytes), 8);
        if(ret < 0) ASN__ENCODE_FAILED;

        /* Number of unused bytes, #16.4.2 */
        ret = asn_put_few_bits(&extadds, unused_bits, 8);
        if(ret < 0) ASN__ENCODE_FAILED;

        /* Encode presence bitmap #16.4.3 */
        for(edx = specs->first_extension; edx < td->elements_count; edx++) {
            asn_TYPE_member_t *elm = &td->elements[edx];
            const void *memb_ptr = element_ptr(sptr, elm);
            if(memb_ptr && elm->default_value_cmp
               && elm->default_value_cmp(memb_ptr) == 0) {
                memb_ptr = 0;   /* Do not encode default value. */
            }
            ret |= asn_put_few_bits(&extadds, memb_ptr ? 1 : 0, 1);
        }
        if(ret < 0) ASN__ENCODE_FAILED;

        asn_put_aligned_flush(&extadds);
        computed_size += extadds.flushed_bytes;

        /* Now, encode extensions */
        for(edx = specs->first_extension; edx < td->elements_count; edx++) {
            asn_TYPE_member_t *elm = &td->elements[edx];
            const void *memb_ptr = element_ptr(sptr, elm);

            if(memb_ptr) {
                if(elm->default_value_cmp
                   && elm->default_value_cmp(memb_ptr) == 0) {
                    /* Do not encode default value. */
                } else {
                    ssize_t wrote = oer_open_type_put(
                        elm->type, elm->encoding_constraints.oer_constraints,
                        memb_ptr, cb, app_key);
                    if(wrote == -1) {
                        ASN__ENCODE_FAILED;
                    }
                    computed_size += wrote;
                }
            } else if(!elm->optional) {
                ASN__ENCODE_FAILED;
            }
        }
    }   /* if(has_extensions) */


    {
        asn_enc_rval_t er = {0, 0, 0};
        er.encoded = computed_size;
        ASN__ENCODED_OK(er);
    }
}

#endif  /* ASN_DISABLE_OER_SUPPORT */
