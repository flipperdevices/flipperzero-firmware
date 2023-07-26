#include <furi_hal.h>

#include "crypto/gcm.h"
#include "crypto_wrapper.h"

struct ESugGhzChatCryptoCtx {
	gcm_context gcm_ctx;
};

void crypto_init(void)
{
	/* init the GCM and AES tables */
	gcm_initialize();
}

void crypto_explicit_bzero(void *s, size_t len)
{
	memset(s, 0, len);
	asm volatile("" ::: "memory");
}

ESubGhzChatCryptoCtx *crypto_ctx_alloc(void)
{
	ESubGhzChatCryptoCtx *ret = malloc(sizeof(ESubGhzChatCryptoCtx));

	if (ret != NULL) {
		memset(ret, 0, sizeof(ESubGhzChatCryptoCtx));
	}

	return ret;
}

void crypto_ctx_free(ESubGhzChatCryptoCtx *ctx)
{
	crypto_ctx_clear(ctx);
	free(ctx);
}

void crypto_ctx_clear(ESubGhzChatCryptoCtx *ctx)
{
	crypto_explicit_bzero(ctx, sizeof(ESubGhzChatCryptoCtx));
}

bool crypto_ctx_set_key(ESubGhzChatCryptoCtx *ctx, const uint8_t *key)
{
	return (gcm_setkey(&(ctx->gcm_ctx), key, KEY_BITS / 8) == 0);
}

bool crypto_ctx_decrypt(ESubGhzChatCryptoCtx *ctx, uint8_t *in, size_t in_len,
		uint8_t *out)
{
	if (in_len < MSG_OVERHEAD + 1) {
		return false;
	}

	return (gcm_auth_decrypt(&(ctx->gcm_ctx),
			in, IV_BYTES,
			NULL, 0,
			in + IV_BYTES, out, in_len - MSG_OVERHEAD,
			in + in_len - TAG_BYTES, TAG_BYTES) == 0);

}

bool crypto_ctx_encrypt(ESubGhzChatCryptoCtx *ctx, uint8_t *in, size_t in_len,
		uint8_t *out)
{
	furi_hal_random_fill_buf(out, IV_BYTES);
	return (gcm_crypt_and_tag(&(ctx->gcm_ctx), ENCRYPT,
			out, IV_BYTES,
			NULL, 0,
			in, out + IV_BYTES, in_len,
			out + IV_BYTES + in_len, TAG_BYTES) == 0);
}
