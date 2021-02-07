#include "ibutton-key.h"
#include <furi.h>

uint8_t iButtonKey::get_size() {
    return size;
}

void iButtonKey::set_data(uint8_t* _data, uint8_t _data_count) {
    furi_check(_data_count > 0);
    furi_check(_data_count <= get_size());

    memset(data, 0, get_size());
    memcpy(data, _data, _data_count);
}

uint8_t* iButtonKey::get_data() {
    return data;
}

void iButtonKey::set_name(char* _name) {
    name = _name;
}

char* iButtonKey::get_name() {
    return name;
}

void iButtonKey::set_type(iButtonKey::KeyType _key_type) {
    type = _key_type;
}

iButtonKey::KeyType iButtonKey::get_key_type() {
    return type;
}
