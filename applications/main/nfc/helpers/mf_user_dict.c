#include "mf_user_dict.h"

#include <nfc/helpers/nfc_dict.h>
#include <furi/furi.h>

struct MfUserDict {
    size_t keys_num;
    MfClassicKey* keys_arr;
};

MfUserDict* mf_user_dict_alloc(size_t max_keys_to_load) {
    MfUserDict* instance = malloc(sizeof(MfUserDict));

    NfcDict* dict = nfc_dict_alloc(NfcDictTypeUser);
    furi_assert(dict);

    size_t dict_keys_num = nfc_dict_get_total_keys(dict);
    instance->keys_num = MIN(max_keys_to_load, dict_keys_num);

    if(instance->keys_num > 0) {
        instance->keys_arr = malloc(instance->keys_num * sizeof(MfClassicKey));
        for(size_t i = 0; i < instance->keys_num; i++) {
            bool key_loaded = nfc_dict_get_next_key(dict, &instance->keys_arr[i]);
            furi_assert(key_loaded);
        }
    }
    nfc_dict_free(dict);

    return instance;
}

void mf_user_dict_free(MfUserDict* instance) {
    furi_assert(instance);

    if(instance->keys_num > 0) {
        free(instance->keys_arr);
    }
    free(instance);
}

size_t mf_user_dict_get_keys_cnt(MfUserDict* instance) {
    furi_assert(instance);

    return instance->keys_num;
}

void mf_user_dict_get_key_str(MfUserDict* instance, uint32_t index, FuriString* str) {
    furi_assert(instance);
    furi_assert(str);
    furi_assert(index < instance->keys_num);
    furi_assert(instance->keys_arr);

    furi_string_reset(str);
    for(size_t i = 0; i < sizeof(MfClassicKey); i++) {
        furi_string_cat_printf(str, "%02X", instance->keys_arr[index].data[i]);
    }
}

bool mf_user_dict_delete_key(MfUserDict* instance, uint32_t index) {
    furi_assert(instance);
    furi_assert(index < instance->keys_num);
    furi_assert(instance->keys_arr);

    NfcDict* dict = nfc_dict_alloc(NfcDictTypeUser);
    furi_assert(dict);

    bool key_delete_success = nfc_dict_delete_key(dict, &instance->keys_arr[index]);
    nfc_dict_free(dict);

    if(key_delete_success) {
        instance->keys_num--;
    }

    return key_delete_success;
}
