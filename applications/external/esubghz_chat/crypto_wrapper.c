#include <furi_hal.h>

#ifndef FURI_HAL_CRYPTO_ADVANCED_AVAIL
#include "crypto/gcm.h"
#endif /* FURI_HAL_CRYPTO_ADVANCED_AVAIL */

#include "crypto_wrapper.h"

struct ESugGhzChatCryptoCtx {
    uint8_t key[KEY_BITS / 8];
#ifndef FURI_HAL_CRYPTO_ADVANCED_AVAIL
    gcm_context gcm_ctx;
#endif /* FURI_HAL_CRYPTO_ADVANCED_AVAIL */
};

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
    }

    return ret;
}

void crypto_ctx_free(ESubGhzChatCryptoCtx* ctx) {
    crypto_ctx_clear(ctx);
    free(ctx);
}

void crypto_ctx_clear(ESubGhzChatCryptoCtx* ctx) {
    crypto_explicit_bzero(ctx, sizeof(ESubGhzChatCryptoCtx));
}

bool crypto_ctx_set_key(ESubGhzChatCryptoCtx* ctx, const uint8_t* key) {
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

#ifdef FURI_HAL_CRYPTO_ADVANCED_AVAIL
    return (
        furi_hal_crypto_gcm_decrypt_and_verify(
            ctx->key, in, in + IV_BYTES, out, in_len - MSG_OVERHEAD, in + in_len - TAG_BYTES) ==
        FuriHalCryptoGCMStateOk);
#else /* FURI_HAL_CRYPTO_ADVANCED_AVAIL */
    return (
        gcm_auth_decrypt(
            &(ctx->gcm_ctx),
            in,
            IV_BYTES,
            NULL,
            0,
            in + IV_BYTES,
            out,
            in_len - MSG_OVERHEAD,
            in + in_len - TAG_BYTES,
            TAG_BYTES) == 0);
#endif /* FURI_HAL_CRYPTO_ADVANCED_AVAIL */
}

bool crypto_ctx_encrypt(ESubGhzChatCryptoCtx* ctx, uint8_t* in, size_t in_len, uint8_t* out) {
    furi_hal_random_fill_buf(out, IV_BYTES);

#ifdef FURI_HAL_CRYPTO_ADVANCED_AVAIL
    return (
        furi_hal_crypto_gcm_encrypt_and_tag(
            ctx->key, out, in, out + IV_BYTES, in_len, out + IV_BYTES + in_len) ==
        FuriHalCryptoGCMStateOk);
#else /* FURI_HAL_CRYPTO_ADVANCED_AVAIL */
    return (
        gcm_crypt_and_tag(
            &(ctx->gcm_ctx),
            ENCRYPT,
            out,
            IV_BYTES,
            NULL,
            0,
            in,
            out + IV_BYTES,
            in_len,
            out + IV_BYTES + in_len,
            TAG_BYTES) == 0);
#endif /* FURI_HAL_CRYPTO_ADVANCED_AVAIL */
}
