#include "ibutton_key.h"

#include <stdlib.h>
#include <string.h>

#include <core/check.h>

typedef union {
    struct {
        uint8_t family_code;
        uint8_t serial_number[6];
        uint8_t checksum;
    } fields;
    uint8_t bytes[8];
} iButtonKeyRomData;

typedef struct {
    uint8_t* data;
    size_t size;
} iButtonKeyUserData;

struct iButtonKey {
    size_t protocol_id;
    iButtonKeyRomData rom_data;
    void* user_data;
};

iButtonKey* ibutton_key_alloc() {
    iButtonKey* key = malloc(sizeof(iButtonKey));
    return key;
}

void ibutton_key_free(iButtonKey* key) {
    if(key->user_data) {
        free(key->user_data);
    }

    free(key);
}

size_t ibutton_key_get_protocol_id(const iButtonKey* key) {
    return key->protocol_id;
}

void ibutton_key_set_protocol_id(iButtonKey* key, size_t protocol_id) {
    key->protocol_id = protocol_id;
}

void ibutton_key_set_rom_data(iButtonKey* key, const uint8_t* rom_data, size_t data_size) {
    furi_check(data_size <= sizeof(key->rom_data.bytes));
    memcpy(key->rom_data.bytes, rom_data, data_size);
}

void ibutton_key_set_user_data(iButtonKey* key, const void* user_data, size_t data_size) {
    if(key->user_data) {
        free(key->user_data);
    }

    key->user_data = malloc(data_size);
    memcpy(key->user_data, user_data, data_size);
}
