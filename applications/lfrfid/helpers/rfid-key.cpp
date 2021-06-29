#include "rfid-key.h"
#include <furi/check.h>
#include <string.h>

RfidKey::RfidKey() {
    data.fill(0);

    for(uint8_t i = 0; i < (LFRFID_KEY_NAME_SIZE + 1); i++) {
        name[i] = 0;
    }
}

RfidKey::~RfidKey() {
}

void RfidKey::set_type(LfrfidKeyType _type) {
    type = _type;
}

void RfidKey::set_data(uint8_t* _data, const uint8_t _data_size) {
    furi_assert(_data_size <= data.size());
    for(uint8_t i = 0; i < _data_size; i++) {
        data[i] = _data[i];
    }
}

void RfidKey::set_name(const char* _name) {
    strlcpy(name, _name, get_name_length());
}

LfrfidKeyType RfidKey::get_type() {
    return type;
}

uint8_t* RfidKey::get_data() {
    return &data[0];
}

const char* RfidKey::get_type_text() {
    return lfrfid_key_get_type_string(type);
}

const uint8_t RfidKey::get_type_data_count() {
    return lfrfid_key_get_type_data_count(type);
}

char* RfidKey::get_name() {
    return name;
}

uint8_t RfidKey::get_name_length() {
    return LFRFID_KEY_NAME_SIZE;
}
