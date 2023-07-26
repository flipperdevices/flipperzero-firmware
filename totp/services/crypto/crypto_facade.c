#include "crypto_facade.h"
#include <furi_hal_crypto.h>
#include "crypto_v1.h"
#include "crypto_v2.h"
#include "constants.h"

bool totp_crypto_check_key_slot(uint8_t key_slot) {
    uint8_t empty_iv[CRYPTO_IV_LENGTH] = {0};
    if(key_slot < ACCEPTABLE_CRYPTO_KEY_SLOT_START || key_slot > ACCEPTABLE_CRYPTO_KEY_SLOT_END) {
        return false;
    }

    return furi_hal_crypto_verify_key(key_slot) &&
           furi_hal_crypto_store_load_key(key_slot, empty_iv) &&
           furi_hal_crypto_store_unload_key(key_slot);
}

uint8_t* totp_crypto_encrypt(
    const uint8_t* plain_data,
    const size_t plain_data_length,
    const uint8_t* iv,
    uint8_t crypto_version,
    uint8_t key_slot,
    size_t* encrypted_data_length) {
    if(crypto_version == 1) {
        return totp_crypto_encrypt_v1(plain_data, plain_data_length, iv, encrypted_data_length);
    }

    if(crypto_version == 2) {
        return totp_crypto_encrypt_v2(
            plain_data, plain_data_length, iv, key_slot, encrypted_data_length);
    }

    furi_crash("Unsupported crypto version");
}

uint8_t* totp_crypto_decrypt(
    const uint8_t* encrypted_data,
    const size_t encrypted_data_length,
    const uint8_t* iv,
    uint8_t crypto_version,
    uint8_t key_slot,
    size_t* decrypted_data_length) {
    if(crypto_version == 1) {
        return totp_crypto_decrypt_v1(
            encrypted_data, encrypted_data_length, iv, decrypted_data_length);
    }

    if(crypto_version == 2) {
        return totp_crypto_decrypt_v2(
            encrypted_data, encrypted_data_length, iv, key_slot, decrypted_data_length);
    }

    furi_crash("Unsupported crypto version");
}

CryptoSeedIVResult
    totp_crypto_seed_iv(PluginState* plugin_state, const uint8_t* pin, uint8_t pin_length) {
    if(plugin_state->crypto_version == 1) {
        return totp_crypto_seed_iv_v1(plugin_state, pin, pin_length);
    }

    if(plugin_state->crypto_version == 2) {
        return totp_crypto_seed_iv_v2(plugin_state, pin, pin_length);
    }

    furi_crash("Unsupported crypto version");
}

bool totp_crypto_verify_key(const PluginState* plugin_state) {
    if(plugin_state->crypto_version == 1) {
        return totp_crypto_verify_key_v1(plugin_state);
    }

    if(plugin_state->crypto_version == 2) {
        return totp_crypto_verify_key_v2(plugin_state);
    }

    furi_crash("Unsupported crypto version");
}