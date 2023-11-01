/*
 * Copyright (c) 2017 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
#include <asn_internal.h>
#include <asn_application.h>
#include <errno.h>

static asn_enc_rval_t asn_encode_internal(const asn_codec_ctx_t *opt_codec_ctx,
                                          enum asn_transfer_syntax syntax,
                                          const asn_TYPE_descriptor_t *td,
                                          const void *sptr,
                                          asn_app_consume_bytes_f *callback,
                                          void *callback_key);


struct callback_count_bytes_key {
    asn_app_consume_bytes_f *callback;
    void *callback_key;
    size_t computed_size;
};

/*
 * Encoder which just counts bytes that come through it.
 */
static int
callback_count_bytes_cb(const void *data, size_t size, void *keyp) {
    struct callback_count_bytes_key *key = keyp;
    int ret;

    ret = key->callback(data, size, key->callback_key);
    if(ret >= 0) {
        key->computed_size += size;
    }

    return ret;
}

struct overrun_encoder_key {
    void *buffer;
    size_t buffer_size;
    size_t computed_size;
};

struct dynamic_encoder_key {
    void *buffer;
    size_t buffer_size;
    size_t computed_size;
};

struct callback_failure_catch_key {
    asn_app_consume_bytes_f *callback;
    void *callback_key;
    int callback_failed;
};

/*
 * Encoder which doesn't stop counting bytes
 * even if it reaches the end of the buffer.
 */
static int
overrun_encoder_cb(const void *data, size_t size, void *keyp) {
    struct overrun_encoder_key *key = keyp;

    if(key->computed_size + size > key->buffer_size) {
        /*
         * Avoid accident on the next call:
         * stop adding bytes to the buffer.
         */
        key->buffer_size = 0;
    } else {
        memcpy((char *)key->buffer + key->computed_size, data, size);
    }
    key->computed_size += size;

    return 0;
}

/*
 * Encoder which dynamically allocates output, and continues
 * to count even if allocation failed.
 */
static int
dynamic_encoder_cb(const void *data, size_t size, void *keyp) {
    struct dynamic_encoder_key *key = keyp;

    if(key->buffer) {
        if(key->computed_size + size >= key->buffer_size) {
            void *p;
            size_t new_size = key->buffer_size;

            do {
                new_size *= 2;
            } while(new_size <= key->computed_size + size);

            p = REALLOC(key->buffer, new_size);
            if(p) {
                key->buffer = p;
                key->buffer_size = new_size;
            } else {
                FREEMEM(key->buffer);
                key->buffer = 0;
                key->buffer_size = 0;
                key->computed_size += size;
                return 0;
            }
        }
        memcpy((char *)key->buffer + key->computed_size, data, size);
    }

    key->computed_size += size;

    return 0;
}

/*
 * Encoder which help convert the application level encoder failure into EIO.
 */
static int
callback_failure_catch_cb(const void *data, size_t size, void *keyp) {
    struct callback_failure_catch_key *key = keyp;
    int ret;

    ret = key->callback(data, size, key->callback_key);
    if(ret < 0) {
        key->callback_failed = 1;
    }

    return ret;
}

asn_enc_rval_t
asn_encode(const asn_codec_ctx_t *opt_codec_ctx,
           enum asn_transfer_syntax syntax, const asn_TYPE_descriptor_t *td,
           const void *sptr, asn_app_consume_bytes_f *callback, void *callback_key) {
    struct callback_failure_catch_key cb_key;
    asn_enc_rval_t er;

    if(!callback) {
        errno = EINVAL;
        ASN__ENCODE_FAILED;
    }

    cb_key.callback = callback;
    cb_key.callback_key = callback_key;
    cb_key.callback_failed = 0;

    er = asn_encode_internal(opt_codec_ctx, syntax, td, sptr,
                             callback_failure_catch_cb, &cb_key);
    if(cb_key.callback_failed) {
        assert(er.encoded == -1);
        assert(errno == EBADF);
        errno = EIO;
    }

    return er;
}

asn_enc_rval_t
asn_encode_to_buffer(const asn_codec_ctx_t *opt_codec_ctx,
                     enum asn_transfer_syntax syntax,
                     const asn_TYPE_descriptor_t *td, const void *sptr,
                     void *buffer, size_t buffer_size) {
    struct overrun_encoder_key buf_key;
    asn_enc_rval_t er;

    if(buffer_size > 0 && !buffer) {
        errno = EINVAL;
        ASN__ENCODE_FAILED;
    }

    buf_key.buffer = buffer;
    buf_key.buffer_size = buffer_size;
    buf_key.computed_size = 0;

    er = asn_encode_internal(opt_codec_ctx, syntax, td, sptr,
                             overrun_encoder_cb, &buf_key);

    if(er.encoded >= 0 && (size_t)er.encoded != buf_key.computed_size) {
        ASN_DEBUG("asn_encode() returned %" ASN_PRI_SSIZE
                  " yet produced %" ASN_PRI_SIZE " bytes",
                  er.encoded, buf_key.computed_size);
        assert(er.encoded < 0 || (size_t)er.encoded == buf_key.computed_size);
    }

    return er;
}

asn_encode_to_new_buffer_result_t
asn_encode_to_new_buffer(const asn_codec_ctx_t *opt_codec_ctx,
                         enum asn_transfer_syntax syntax,
                         const asn_TYPE_descriptor_t *td, const void *sptr) {
    struct dynamic_encoder_key buf_key;
    asn_encode_to_new_buffer_result_t res;

    buf_key.buffer_size = 16;
    buf_key.buffer = MALLOC(buf_key.buffer_size);
    buf_key.computed_size = 0;

    res.result = asn_encode_internal(opt_codec_ctx, syntax, td, sptr,
                                     dynamic_encoder_cb, &buf_key);

    if(res.result.encoded >= 0
       && (size_t)res.result.encoded != buf_key.computed_size) {
        ASN_DEBUG("asn_encode() returned %" ASN_PRI_SSIZE
                  " yet produced %" ASN_PRI_SIZE " bytes",
                  res.result.encoded, buf_key.computed_size);
        assert(res.result.encoded < 0
               || (size_t)res.result.encoded == buf_key.computed_size);
    }

    res.buffer = buf_key.buffer;

    /* 0-terminate just in case. */
    if(res.buffer) {
        assert(buf_key.computed_size < buf_key.buffer_size);
        ((char *)res.buffer)[buf_key.computed_size] = '\0';
    }

    return res;
}

static asn_enc_rval_t
asn_encode_internal(const asn_codec_ctx_t *opt_codec_ctx,
                    enum asn_transfer_syntax syntax,
                    const asn_TYPE_descriptor_t *td, const void *sptr,
                    asn_app_consume_bytes_f *callback, void *callback_key) {
    asn_enc_rval_t er;
    enum xer_encoder_flags_e xer_flags = XER_F_CANONICAL;

    (void)opt_codec_ctx; /* Parameters are not checked on encode yet. */

    if(!td || !sptr) {
        errno = EINVAL;
        ASN__ENCODE_FAILED;
    }

    switch(syntax) {
    case ATS_NONSTANDARD_PLAINTEXT:
        if(td->op->print_struct) {
            struct callback_count_bytes_key cb_key;
            cb_key.callback = callback;
            cb_key.callback_key = callback_key;
            cb_key.computed_size = 0;
            if(td->op->print_struct(td, sptr, 1, callback_count_bytes_cb,
                                    &cb_key)
                   < 0
               || callback_count_bytes_cb("\n", 1, &cb_key) < 0) {
                errno = EBADF; /* Structure has incorrect form. */
                er.encoded = -1;
                er.failed_type = td;
                er.structure_ptr = sptr;
            } else {
                er.encoded = cb_key.computed_size;
                er.failed_type = 0;
                er.structure_ptr = 0;
            }
        } else {
            errno = ENOENT; /* Transfer syntax is not defined for this type. */
            ASN__ENCODE_FAILED;
        }
        break;

    case ATS_RANDOM:
        errno = ENOENT; /* Randomization doesn't make sense on output. */
        ASN__ENCODE_FAILED;

    case ATS_BER:
        /* BER is a superset of DER. */
        /* Fall through. */
    case ATS_DER:
        if(td->op->der_encoder) {
            er = der_encode(td, sptr, callback, callback_key);
            if(er.encoded == -1) {
                if(er.failed_type && er.failed_type->op->der_encoder) {
                    errno = EBADF;  /* Structure has incorrect form. */
                } else {
                    errno = ENOENT; /* DER is not defined for this type. */
                }
            }
        } else {
            errno = ENOENT; /* Transfer syntax is not defined for this type. */
            ASN__ENCODE_FAILED;
        }
        break;
    case ATS_CER:
        errno = ENOENT; /* Transfer syntax is not defined for any type. */
        ASN__ENCODE_FAILED;

#ifdef  ASN_DISABLE_OER_SUPPORT
    case ATS_BASIC_OER:
    case ATS_CANONICAL_OER:
        errno = ENOENT; /* PER is not defined. */
        ASN__ENCODE_FAILED;
        break;
#else /* ASN_DISABLE_OER_SUPPORT */
    case ATS_BASIC_OER:
        /* CANONICAL-OER is a superset of BASIC-OER. */
        /* Fall through. */
    case ATS_CANONICAL_OER:
        if(td->op->oer_encoder) {
            er = oer_encode(td, sptr, callback, callback_key);
            if(er.encoded == -1) {
                if(er.failed_type && er.failed_type->op->oer_encoder) {
                    errno = EBADF;  /* Structure has incorrect form. */
                } else {
                    errno = ENOENT; /* OER is not defined for this type. */
                }
            }
        } else {
            errno = ENOENT; /* Transfer syntax is not defined for this type. */
            ASN__ENCODE_FAILED;
        }
        break;
#endif /* ASN_DISABLE_OER_SUPPORT */

#ifdef  ASN_DISABLE_PER_SUPPORT
    case ATS_UNALIGNED_BASIC_PER:
    case ATS_UNALIGNED_CANONICAL_PER:
        errno = ENOENT; /* PER is not defined. */
        ASN__ENCODE_FAILED;
        break;
#else /* ASN_DISABLE_PER_SUPPORT */
    case ATS_UNALIGNED_BASIC_PER:
        /* CANONICAL-UPER is a superset of BASIC-UPER. */
        /* Fall through. */
    case ATS_UNALIGNED_CANONICAL_PER:
        if(td->op->uper_encoder) {
            er = uper_encode(td, 0, sptr, callback, callback_key);
            if(er.encoded == -1) {
                if(er.failed_type && er.failed_type->op->uper_encoder) {
                    errno = EBADF;  /* Structure has incorrect form. */
                } else {
                    errno = ENOENT; /* UPER is not defined for this type. */
                }
            } else {
                ASN_DEBUG("Complete encoded in %ld bits", (long)er.encoded);
                if(er.encoded == 0) {
                    /* Enforce "Complete Encoding" of X.691 #11.1 */
                    if(callback("\0", 1, callback_key) < 0) {
                        errno = EBADF;
                        ASN__ENCODE_FAILED;
                    }
                    er.encoded = 8; /* Exactly 8 zero bits is added. */
                }
                /* Convert bits into bytes */
                er.encoded = (er.encoded + 7) >> 3;
            }
        } else {
            errno = ENOENT; /* Transfer syntax is not defined for this type. */
            ASN__ENCODE_FAILED;
        }
        break;
#endif  /* ASN_DISABLE_PER_SUPPORT */

    case ATS_BASIC_XER:
        /* CANONICAL-XER is a superset of BASIC-XER. */
        xer_flags &= ~XER_F_CANONICAL;
        xer_flags |= XER_F_BASIC;
        /* Fall through. */
    case ATS_CANONICAL_XER:
        if(td->op->xer_encoder) {
            er = xer_encode(td, sptr, xer_flags, callback, callback_key);
            if(er.encoded == -1) {
                if(er.failed_type && er.failed_type->op->xer_encoder) {
                    errno = EBADF;  /* Structure has incorrect form. */
                } else {
                    errno = ENOENT; /* XER is not defined for this type. */
                }
            }
        } else {
            errno = ENOENT; /* Transfer syntax is not defined for this type. */
            ASN__ENCODE_FAILED;
        }
        break;

    default:
        errno = ENOENT;
        ASN__ENCODE_FAILED;
    }

    return er;
}

asn_dec_rval_t
asn_decode(const asn_codec_ctx_t *opt_codec_ctx,
           enum asn_transfer_syntax syntax, const asn_TYPE_descriptor_t *td,
           void **sptr, const void *buffer, size_t size) {
    if(!td || !td->op || !sptr || (size && !buffer)) {
        ASN__DECODE_FAILED;
    }

    switch(syntax) {
    case ATS_CER:
    case ATS_NONSTANDARD_PLAINTEXT:
    default:
        errno = ENOENT;
        ASN__DECODE_FAILED;

    case ATS_RANDOM:
        if(!td->op->random_fill) {
            ASN__DECODE_FAILED;
        } else {
            if(asn_random_fill(td, sptr, 16000) == 0) {
                asn_dec_rval_t ret = {RC_OK, 0};
                return ret;
            } else {
                ASN__DECODE_FAILED;
            }
        }
        break;

    case ATS_DER:
    case ATS_BER:
        return ber_decode(opt_codec_ctx, td, sptr, buffer, size);

    case ATS_BASIC_OER:
    case ATS_CANONICAL_OER:
#ifdef  ASN_DISABLE_OER_SUPPORT
        errno = ENOENT;
        ASN__DECODE_FAILED;
#else
        return oer_decode(opt_codec_ctx, td, sptr, buffer, size);
#endif

    case ATS_UNALIGNED_BASIC_PER:
    case ATS_UNALIGNED_CANONICAL_PER:
#ifdef  ASN_DISABLE_PER_SUPPORT
        errno = ENOENT;
        ASN__DECODE_FAILED;
#else
        return uper_decode_complete(opt_codec_ctx, td, sptr, buffer, size);
#endif

    case ATS_BASIC_XER:
    case ATS_CANONICAL_XER:
        return xer_decode(opt_codec_ctx, td, sptr, buffer, size);
    }
}

