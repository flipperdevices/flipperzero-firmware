#include <furi_hal.h>
#include <lib/mlib/m-dict.h>
#include <toolbox/sha256.h>

#ifndef FURI_HAL_CRYPTO_ADVANCED_AVAIL
#include "crypto/gcm.h"
#endif /* FURI_HAL_CRYPTO_ADVANCED_AVAIL */

#include "crypto_wrapper.h"

DICT_DEF2(ESubGhzChatReplayDict, uint64_t, uint32_t)

struct ESugGhzChatCryptoCtx {
    uint8_t key[KEY_BITS / 8];
#ifndef FURI_HAL_CRYPTO_ADVANCED_AVAIL
    gcm_context gcm_ctx;
#endif /* FURI_HAL_CRYPTO_ADVANCED_AVAIL */
    ESubGhzChatReplayDict_t replay_dict;
    uint64_t run_id;
    uint32_t counter;
};

struct ESubGhzChatCryptoMsg {
    uint64_t run_id;
    uint32_t counter;
    uint8_t iv[IV_BYTES];
    uint8_t tag[TAG_BYTES];
    uint8_t data[0];
} __attribute__((packed));

void crypto_init(void) {
#ifndef FURI_HAL_CRYPTO_ADVANCED_AVAIL
    /* init the GCM and AES tables */
    gcm_initialize();
#endif /* FURI_HAL_CRYPTO_ADVANCED_AVAIL */
}

void crypto_explicit_bzero(void* s, size_t len) {
    memset(s, 0, len);
    asm volatile("" ::: "memory");
}

ESubGhzChatCryptoCtx* crypto_ctx_alloc(void) {
    ESubGhzChatCryptoCtx* ret = malloc(sizeof(ESubGhzChatCryptoCtx));

    if(ret != NULL) {
        memset(ret, 0, sizeof(ESubGhzChatCryptoCtx));
        ESubGhzChatReplayDict_init(ret->replay_dict);
        ret->run_id = 0;
        ret->counter = 1;
    }

    return ret;
}

void crypto_ctx_free(ESubGhzChatCryptoCtx* ctx) {
    crypto_ctx_clear(ctx);
    ESubGhzChatReplayDict_clear(ctx->replay_dict);
    free(ctx);
}

void crypto_ctx_clear(ESubGhzChatCryptoCtx* ctx) {
    crypto_explicit_bzero(ctx->key, sizeof(ctx->key));
#ifndef FURI_HAL_CRYPTO_ADVANCED_AVAIL
    crypto_explicit_bzero(&(ctx->gcm_ctx), sizeof(ctx->gcm_ctx));
#endif /* FURI_HAL_CRYPTO_ADVANCED_AVAIL */
    ESubGhzChatReplayDict_reset(ctx->replay_dict);
    ctx->run_id = 0;
    ctx->counter = 1;
}

static uint64_t crypto_calc_run_id(FuriString* flipper_name, uint32_t tick) {
    const char* fn = furi_string_get_cstr(flipper_name);
    size_t fn_len = strlen(fn);

    uint8_t h_in[fn_len + sizeof(uint32_t)];
    memcpy(h_in, fn, fn_len);
    memcpy(h_in + fn_len, &tick, sizeof(uint32_t));

    uint8_t h_out[256];
    sha256(h_in, fn_len + sizeof(uint32_t), h_out);

    uint64_t run_id;
    memcpy(&run_id, h_out, sizeof(uint64_t));

    return run_id;
}

bool crypto_ctx_set_key(
    ESubGhzChatCryptoCtx* ctx,
    const uint8_t* key,
    FuriString* flipper_name,
    uint32_t tick) {
    ctx->run_id = crypto_calc_run_id(flipper_name, tick);
    ctx->counter = 1;

    memcpy(ctx->key, key, KEY_BITS / 8);
#ifdef FURI_HAL_CRYPTO_ADVANCED_AVAIL
    return true;
#else /* FURI_HAL_CRYPTO_ADVANCED_AVAIL */
    return (gcm_setkey(&(ctx->gcm_ctx), key, KEY_BITS / 8) == 0);
#endif /* FURI_HAL_CRYPTO_ADVANCED_AVAIL */
}

void crypto_ctx_get_key(ESubGhzChatCryptoCtx* ctx, uint8_t* key) {
    memcpy(key, ctx->key, KEY_BITS / 8);
}

bool crypto_ctx_decrypt(ESubGhzChatCryptoCtx* ctx, uint8_t* in, size_t in_len, uint8_t* out) {
    if(in_len < MSG_OVERHEAD + 1) {
        return false;
    }

    struct ESubGhzChatCryptoMsg* msg = (struct ESubGhzChatCryptoMsg*)in;

    // check if message is stale, if yes, discard
    uint32_t* counter = ESubGhzChatReplayDict_get(ctx->replay_dict, msg->run_id);
    if(counter != NULL) {
        if(*counter >= __ntohl(msg->counter)) {
            return false;
        }
    }

    // decrypt and auth message
#ifdef FURI_HAL_CRYPTO_ADVANCED_AVAIL
    bool ret =
        (furi_hal_crypto_gcm_decrypt_and_verify(
             ctx->key,
             msg->iv,
             (uint8_t*)msg,
             RUN_ID_BYTES + COUNTER_BYTES,
             msg->data,
             out,
             in_len - MSG_OVERHEAD,
             msg->tag) == FuriHalCryptoGCMStateOk);
#else /* FURI_HAL_CRYPTO_ADVANCED_AVAIL */
    bool ret =
        (gcm_auth_decrypt(
             &(ctx->gcm_ctx),
             msg->iv,
             IV_BYTES,
             (uint8_t*)msg,
             RUN_ID_BYTES + COUNTER_BYTES,
             msg->data,
             out,
             in_len - MSG_OVERHEAD,
             msg->tag,
             TAG_BYTES) == 0);
#endif /* FURI_HAL_CRYPTO_ADVANCED_AVAIL */

    // if auth was successful update replay dict
    if(ret) {
        ESubGhzChatReplayDict_set_at(ctx->replay_dict, msg->run_id, __ntohl(msg->counter));
    }

    return ret;
}

bool crypto_ctx_encrypt(ESubGhzChatCryptoCtx* ctx, uint8_t* in, size_t in_len, uint8_t* out) {
    struct ESubGhzChatCryptoMsg* msg = (struct ESubGhzChatCryptoMsg*)out;

    // fill message header
    msg->run_id = ctx->run_id;
    msg->counter = __htonl(ctx->counter);
    furi_hal_random_fill_buf(msg->iv, IV_BYTES);

    // encrypt message and store tag in header
#ifdef FURI_HAL_CRYPTO_ADVANCED_AVAIL
    bool ret =
        (furi_hal_crypto_gcm_encrypt_and_tag(
             ctx->key,
             msg->iv,
             (uint8_t*)msg,
             RUN_ID_BYTES + COUNTER_BYTES,
             in,
             msg->data,
             in_len,
             msg->tag) == FuriHalCryptoGCMStateOk);
#else /* FURI_HAL_CRYPTO_ADVANCED_AVAIL */
    bool ret =
        (gcm_crypt_and_tag(
             &(ctx->gcm_ctx),
             ENCRYPT,
             msg->iv,
             IV_BYTES,
             (uint8_t*)msg,
             RUN_ID_BYTES + COUNTER_BYTES,
             in,
             msg->data,
             in_len,
             msg->tag,
             TAG_BYTES) == 0);
#endif /* FURI_HAL_CRYPTO_ADVANCED_AVAIL */

    // update replay dict and increase internal counter
    if(ret) {
        ESubGhzChatReplayDict_set_at(ctx->replay_dict, ctx->run_id, ctx->counter);
        ctx->counter++;
    }

    return ret;
}

size_t crypto_ctx_dump_replay_dict(
    ESubGhzChatCryptoCtx* ctx,
    CryptoCtxReplayDictWriter writer,
    void* writer_ctx) {
    size_t ret = 0;
    ESubGhzChatReplayDict_it_t i;

    for(ESubGhzChatReplayDict_it(i, ctx->replay_dict); !ESubGhzChatReplayDict_end_p(i);
        ESubGhzChatReplayDict_next(i), ret++) {
        ESubGhzChatReplayDict_itref_t* ref = ESubGhzChatReplayDict_ref(i);
        if(!writer(ref->key, ref->value, writer_ctx)) {
            break;
        }
    }

    return ret;
}

size_t crypto_ctx_read_replay_dict(
    ESubGhzChatCryptoCtx* ctx,
    CryptoCtxReplayDictReader reader,
    void* reader_ctx) {
    size_t ret = 0;

    uint64_t run_id;
    uint32_t counter;

    while(reader(&run_id, &counter, reader_ctx)) {
        ESubGhzChatReplayDict_set_at(ctx->replay_dict, run_id, counter);
        ret++;
    }

    return ret;
}
