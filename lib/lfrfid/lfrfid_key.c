#include <furi.h>
#include <one_wire/maxim_crc.h>
#include "lfrfid_key.h"

struct LFRFIDKey {
    uint8_t data[LFRIFID_KEY_DATA_SIZE];
    char name[LFRIFID_KEY_NAME_SIZE];
    LFRFIDKeyType type;
};

LFRFIDKey* lfrfid_key_alloc() {
    LFRFIDKey* key = malloc(sizeof(LFRFIDKey));
    memset(key, 0, sizeof(LFRFIDKey));
    return key;
}

void lfrfid_key_free(LFRFIDKey* key) {
    free(key);
}

void lfrfid_key_set(LFRFIDKey* to, const LFRFIDKey* from) {
    memcpy(to, from, sizeof(LFRFIDKey));
}

void lfrfid_key_set_data(LFRFIDKey* key, uint8_t* data, uint8_t data_count) {
    furi_check(data_count > 0);
    furi_check(data_count <= LFRIFID_KEY_DATA_SIZE);

    memset(key->data, 0, LFRIFID_KEY_DATA_SIZE);
    memcpy(key->data, data, data_count);
}

void lfrfid_key_clear_data(LFRFIDKey* key) {
    memset(key->data, 0, LFRIFID_KEY_DATA_SIZE);
}

const uint8_t* lfrfid_key_get_data_p(LFRFIDKey* key) {
    return key->data;
}

uint8_t lfrfid_key_get_data_size(LFRFIDKey* key) {
    return lfrfid_key_get_size_by_type(key->type);
}

void lfrfid_key_set_name(LFRFIDKey* key, const char* name) {
    strlcpy(key->name, name, LFRIFID_KEY_NAME_SIZE);
}

const char* lfrfid_key_get_name_p(LFRFIDKey* key) {
    return key->name;
}

void lfrfid_key_set_type(LFRFIDKey* key, LFRFIDKeyType key_type) {
    key->type = key_type;
}

LFRFIDKeyType lfrfid_key_get_type(LFRFIDKey* key) {
    return key->type;
}

const char* lfrfid_key_get_string_by_type(LFRFIDKeyType key_type) {
    switch(key_type) {
    case LFRFIDKeyEM4100:
        return "EM4100";
        break;
    case LFRFIDKeyH10301:
        return "H10301";
        break;
    case LFRFIDKeyI40134:
        return "I40134";
        break;
    case LFRFIDKeyKeyIOProxXSF:
        return "IOProxXSF";
        break;
    default:
        furi_crash("Invalid LF RFID type");
        return "";
        break;
    }
}

const char* lfrfid_key_get_manufacturer_string(LFRFIDKeyType type) {
    switch(type) {
    case LFRFIDKeyEM4100:
        return "EM-Marin";
        break;
    case LFRFIDKeyH10301:
        return "HID";
        break;
    case LFRFIDKeyI40134:
        return "Indala";
        break;
    case LFRFIDKeyKeyIOProxXSF:
        return "Kantech";
        break;
    }

    return "Unknown";
}

bool lfrfid_key_get_type_by_string(const char* type_string, LFRFIDKeyType* key_type) {
    uint32_t max_key_type = LFRFIDKeyKeyIOProxXSF;

    for(uint32_t i = 0; i <= max_key_type; i++) {
        if(strcmp(type_string, lfrfid_key_get_string_by_type(i)) == 0) {
            *key_type = i;
            return true;
        }
    }

    return false;
}

uint8_t lfrfid_key_get_size_by_type(LFRFIDKeyType key_type) {
    uint8_t size = 0;

    switch(key_type) {
    case LFRFIDKeyEM4100:
        size = 5;
        break;
    case LFRFIDKeyH10301:
        size = 3;
        break;
    case LFRFIDKeyI40134:
        size = 3;
        break;
    case LFRFIDKeyKeyIOProxXSF:
        size = 4;
        break;
    }

    return size;
}

uint8_t lfrfid_key_get_max_size() {
    return LFRIFID_KEY_DATA_SIZE;
}