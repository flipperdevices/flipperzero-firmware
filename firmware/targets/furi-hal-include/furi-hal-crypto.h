#pragma once

#include <stdint.h>

/** FuriHalCryptoKey Type */
typedef enum {
    FuriHalCryptoKeyTypeMaster, /**< Master key */
    FuriHalCryptoKeyTypeSimple,  /**< Simple enencrypted key */
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

/** Initialize cryptography layer
 * This includes AES engines, PKA and RNG
 */
void furi_hal_crypto_init();

/** Store key in crypto storage
 * @param key - FuriHalCryptoKey to store. Only Master, Simple or Encrypted
 * @return - key_index in store
 */
uint8_t furi_hal_crypto_store_add_key(FuriHalCryptoKey* key);

/** Load key into AES engine
 * 
 * @param key_index - key index that was returned on store key step
 */
void furi_hal_crypto_store_load_key(uint8_t key_index);
