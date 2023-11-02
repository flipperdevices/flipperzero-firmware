#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define RUN_ID_BYTES (sizeof(uint64_t))
#define COUNTER_BYTES (sizeof(uint32_t))
#define KEY_BITS 256
#define IV_BYTES 12
#define TAG_BYTES 16

#define MSG_OVERHEAD (RUN_ID_BYTES + COUNTER_BYTES + IV_BYTES + TAG_BYTES)

typedef struct ESugGhzChatCryptoCtx ESubGhzChatCryptoCtx;

void crypto_init(void);

/* Function to clear sensitive memory. */
void crypto_explicit_bzero(void* s, size_t len);

ESubGhzChatCryptoCtx* crypto_ctx_alloc(void);
void crypto_ctx_free(ESubGhzChatCryptoCtx* ctx);

void crypto_ctx_clear(ESubGhzChatCryptoCtx* ctx);

bool crypto_ctx_set_key(
    ESubGhzChatCryptoCtx* ctx,
    const uint8_t* key,
    FuriString* flipper_name,
    uint32_t tick);
void crypto_ctx_get_key(ESubGhzChatCryptoCtx* ctx, uint8_t* key);

bool crypto_ctx_decrypt(ESubGhzChatCryptoCtx* ctx, uint8_t* in, size_t in_len, uint8_t* out);
bool crypto_ctx_encrypt(ESubGhzChatCryptoCtx* ctx, uint8_t* in, size_t in_len, uint8_t* out);

typedef bool (*CryptoCtxReplayDictWriter)(uint64_t run_id, uint32_t counter, void* context);
typedef bool (*CryptoCtxReplayDictReader)(uint64_t* run_id, uint32_t* counter, void* context);

size_t crypto_ctx_dump_replay_dict(
    ESubGhzChatCryptoCtx* ctx,
    CryptoCtxReplayDictWriter writer,
    void* writer_ctx);
size_t crypto_ctx_read_replay_dict(
    ESubGhzChatCryptoCtx* ctx,
    CryptoCtxReplayDictReader reader,
    void* reader_ctx);

#ifdef __cplusplus
}
#endif
