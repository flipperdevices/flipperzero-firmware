#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define KEY_BITS 256
#define IV_BYTES 12
#define TAG_BYTES 16

#define MSG_OVERHEAD (IV_BYTES + TAG_BYTES)

typedef struct ESugGhzChatCryptoCtx ESubGhzChatCryptoCtx;

void crypto_init(void);

/* Function to clear sensitive memory. */
void crypto_explicit_bzero(void* s, size_t len);

ESubGhzChatCryptoCtx* crypto_ctx_alloc(void);
void crypto_ctx_free(ESubGhzChatCryptoCtx* ctx);

void crypto_ctx_clear(ESubGhzChatCryptoCtx* ctx);

bool crypto_ctx_set_key(ESubGhzChatCryptoCtx* ctx, const uint8_t* key);
void crypto_ctx_get_key(ESubGhzChatCryptoCtx* ctx, uint8_t* key);

bool crypto_ctx_decrypt(ESubGhzChatCryptoCtx* ctx, uint8_t* in, size_t in_len, uint8_t* out);
bool crypto_ctx_encrypt(ESubGhzChatCryptoCtx* ctx, uint8_t* in, size_t in_len, uint8_t* out);

#ifdef __cplusplus
}
#endif
