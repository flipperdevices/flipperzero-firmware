#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    NfcDictModeOpenExisting,
    NfcDictModeOpenAlways,
} NfcDictMode;

typedef struct NfcDict NfcDict;

bool nfc_dict_check_presence(const char* path);

NfcDict* nfc_dict_alloc(const char* path, NfcDictMode mode, size_t key_size);

void nfc_dict_free(NfcDict* instance);

uint32_t nfc_dict_get_total_keys(NfcDict* instance);

bool nfc_dict_rewind(NfcDict* instance);

bool nfc_dict_is_key_present(NfcDict* instance, const uint8_t* key, size_t key_size);

bool nfc_dict_get_next_key(NfcDict* instance, uint8_t* key, size_t key_size);

bool nfc_dict_add_key(NfcDict* instance, const uint8_t* key, size_t key_size);

bool nfc_dict_delete_key(NfcDict* instance, const uint8_t* key, size_t key_size);

#ifdef __cplusplus
}
#endif
