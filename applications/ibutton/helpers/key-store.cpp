#include "key-store.h"
#include <furi.h>

uint16_t KeyStore::get_key_count() {
    return store.size();
}

uint8_t KeyStore::add_key() {
    store.push_back(iButtonKey());
    return get_key_count() - 1;
}

void KeyStore::set_key_type(uint8_t index, iButtonKeyType type) {
    iButtonKey* key = get_key(index);
    key->set_type(type);
}

void KeyStore::set_key_name(uint8_t index, char* name) {
    iButtonKey* key = get_key(index);
    key->set_name(name);
}

void KeyStore::set_key_data(uint8_t index, uint8_t* data, uint8_t data_size) {
    iButtonKey* key = get_key(index);
    key->set_data(data, data_size);
}

iButtonKeyType KeyStore::get_key_type(uint8_t index) {
    iButtonKey* key = get_key(index);
    return key->get_key_type();
}

const char* KeyStore::get_key_name(uint8_t index) {
    iButtonKey* key = get_key(index);
    return key->get_name();
}

uint8_t* KeyStore::get_key_data(uint8_t index) {
    iButtonKey* key = get_key(index);
    return key->get_data();
}

void KeyStore::remove_key(uint8_t index) {
    iButtonKey* key = get_key(index);
}

KeyStore::KeyStore() {
    store.push_back(iButtonKey(
        iButtonKeyType::KeyDallas, "Home_key", 0x01, 0x41, 0xCE, 0x67, 0x0F, 0x00, 0x00, 0xB6));
    store.push_back(iButtonKey(
        iButtonKeyType::KeyDallas, "Work_key", 0x01, 0xFD, 0x0E, 0x84, 0x01, 0x00, 0x00, 0xDB));
}

KeyStore::~KeyStore() {
}

iButtonKey* KeyStore::get_key(uint8_t index) {
    furi_check(index > 0);
    furi_check(index < get_key_count());
    return &(*std::next(store.begin(), index));
}
