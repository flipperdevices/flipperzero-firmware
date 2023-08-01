/**
 * @file furi_hal_crypto.h
 * Cryptography HAL API
 */
#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define FURI_HAL_CRYPTO_ADVANCED_AVAIL 1

/** FuriHalCryptoKey Type */
typedef enum {
    FuriHalCryptoKeyTypeMaster, /**< Master key */
    FuriHalCryptoKeyTypeSimple, /**< Simple enencrypted key */
    FuriHalCryptoKeyTypeEncrypted, /**< Encrypted with Master key */
} FuriHalCryptoKeyType;

/** FuriHalCryptoKey Size in bits */
typedef enum {
    FuriHalCryptoKeySize128,
    FuriHalCryptoKeySize256,
} FuriHalCryptoKeySize;

/** FuriHalCryptoKey */
typedef struct {
    FuriHalCryptoKeyType type;
    FuriHalCryptoKeySize size;
    uint8_t* data;
} FuriHalCryptoKey;

/** FuriHalCryptoGCMState Result of a GCM operation */
typedef enum {
    FuriHalCryptoGCMStateOk, /**< operation successful */
    FuriHalCryptoGCMStateError, /**< error during en-/decryption */
    FuriHalCryptoGCMStateAuthFailure, /**< tags do not match, auth failed */
} FuriHalCryptoGCMState;

/** Initialize cryptography layer This includes AES engines, PKA and RNG
 */
void furi_hal_crypto_init();

bool furi_hal_crypto_verify_enclave(uint8_t* keys_nb, uint8_t* valid_keys_nb);

bool furi_hal_crypto_verify_key(uint8_t key_slot);

/** Store key in crypto storage
 *
 * @param      key   FuriHalCryptoKey to store. Only Master, Simple or
 *                   Encrypted
 * @param      slot  pinter to int where store slot number will be saved
 *
 * @return     true on success
 */
bool furi_hal_crypto_store_add_key(FuriHalCryptoKey* key, uint8_t* slot);

/** Init AES engine and load key from crypto store
 *
 * @param      slot  store slot number
 * @param[in]  iv    pointer to 16 bytes Initialization Vector data
 *
 * @return     true on success
 */
bool furi_hal_crypto_store_load_key(uint8_t slot, const uint8_t* iv);

/** Unload key engine and deinit AES engine
 *
 * @param      slot  store slot number
 *
 * @return     true on success
 */
bool furi_hal_crypto_store_unload_key(uint8_t slot);

/** Init AES engine and load supplied key
 * Use furi_hal_crypto_unload_key() to deinit the AES engine after using this.
 *
 * @param[in]  key   pointer to 32 bytes key data
 * @param[in]  iv    pointer to 16 bytes Initialization Vector data
 *
 * @return     true on success
 */
bool furi_hal_crypto_load_key(const uint8_t* key, const uint8_t* iv);

/** Deinit AES engine
 * Only use this if you previously inited the AES engine using
 * furi_hal_crypto_load_key().
 *
 * @return     true on success
 */
bool furi_hal_crypto_unload_key(void);

/** Encrypt data
 *
 * @param      input   pointer to input data
 * @param      output  pointer to output data
 * @param      size    input/output buffer size in bytes
 *
 * @return     true on success
 */
bool furi_hal_crypto_encrypt(const uint8_t* input, uint8_t* output, size_t size);

/** Decrypt data
 *
 * @param      input   pointer to input data
 * @param      output  pointer to output data
 * @param      size    input/output buffer size in bytes
 *
 * @return     true on success
 */
bool furi_hal_crypto_decrypt(const uint8_t* input, uint8_t* output, size_t size);

/** Encrypt the input using AES-CTR
 * Decryption can be performed by supplying the ciphertext as input.
 * Inits and deinits the AES engine internally.
 *
 * @param[in]  key     pointer to 32 bytes key data
 * @param[in]  iv      pointer to 12 bytes Initialization Vector data
 * @param[in]  input   pointer to input data
 * @param[out] output  pointer to output data
 * @param      length  length of the input and output in bytes
 *
 * @return     true on success
 */
bool furi_hal_crypto_ctr(
    const uint8_t* key,
    const uint8_t* iv,
    const uint8_t* input,
    uint8_t* output,
    size_t length);

/** Encrypt/decrypt the input using AES-GCM
 * When decrypting the tag generated needs to be compared to the tag attached
 * to the ciphertext in a constant-time fashion. If the tags are not equal, the
 * decryption failed and the plaintext returned needs to be discarded.
 * Inits and deinits the AES engine internally.
 *
 * @param[in]  key        pointer to 32 bytes key data
 * @param[in]  iv         pointer to 12 bytes Initialization Vector data
 * @param[in]  aad        pointer to additional authentication data
 * @param      aad_length length of the additional authentication data in bytes
 * @param[in]  input      pointer to input data
 * @param[out] output     pointer to output data
 * @param      length     length of the input and output in bytes
 * @param[out] tag        pointer to 16 bytes space for the tag
 * @param      decrypt    true for decryption, false otherwise
 *
 * @return     true on success
 */
bool furi_hal_crypto_gcm(
    const uint8_t* key,
    const uint8_t* iv,
    const uint8_t* aad,
    size_t aad_length,
    const uint8_t* input,
    uint8_t* output,
    size_t length,
    uint8_t* tag,
    bool decrypt);

/** Encrypt the input using AES-GCM and generate a tag
 * Inits and deinits the AES engine internally.
 *
 * @param[in]  key        pointer to 32 bytes key data
 * @param[in]  iv         pointer to 12 bytes Initialization Vector data
 * @param[in]  aad        pointer to additional authentication data
 * @param      aad_length length of the additional authentication data in bytes
 * @param[in]  input      pointer to input data
 * @param[out] output     pointer to output data
 * @param      length     length of the input and output in bytes
 * @param[out] tag        pointer to 16 bytes space for the tag
 *
 * @return     FuriHalCryptoGCMStateOk on success, FuriHalCryptoGCMStateError
 *             on failure
 */
FuriHalCryptoGCMState furi_hal_crypto_gcm_encrypt_and_tag(
    const uint8_t* key,
    const uint8_t* iv,
    const uint8_t* aad,
    size_t aad_length,
    const uint8_t* input,
    uint8_t* output,
    size_t length,
    uint8_t* tag);

/** Decrypt the input using AES-GCM and verify the provided tag
 * Inits and deinits the AES engine internally.
 *
 * @param[in]  key        pointer to 32 bytes key data
 * @param[in]  iv         pointer to 12 bytes Initialization Vector data
 * @param[in]  aad        pointer to additional authentication data
 * @param      aad_length length of the additional authentication data in bytes
 * @param[in]  input      pointer to input data
 * @param[out] output     pointer to output data
 * @param      length     length of the input and output in bytes
 * @param[out] tag        pointer to 16 bytes tag
 *
 * @return     FuriHalCryptoGCMStateOk on success, FuriHalCryptoGCMStateError
 *             on failure, FuriHalCryptoGCMStateAuthFailure if the tag does not
 *             match
 */
FuriHalCryptoGCMState furi_hal_crypto_gcm_decrypt_and_verify(
    const uint8_t* key,
    const uint8_t* iv,
    const uint8_t* aad,
    size_t aad_length,
    const uint8_t* input,
    uint8_t* output,
    size_t length,
    const uint8_t* tag);

#ifdef __cplusplus
}
#endif
