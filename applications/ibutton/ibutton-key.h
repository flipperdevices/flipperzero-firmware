#pragma once
#include <stdint.h>

class iButtonKey {
public:
    enum class KeyType : uint8_t {
        KeyDallas,
    };

    uint8_t get_size();

    void set_data(uint8_t* data, uint8_t data_count);
    uint8_t* get_data();

    void set_name(char* name);
    char* get_name();

    void set_type(iButtonKey::KeyType key_type);
    iButtonKey::KeyType get_key_type();

private:
    static const uint8_t size = 8;
    uint8_t data[size] = {0, 0, 0, 0, 0, 0, 0, 0};
    char* name = {0};

    iButtonKey::KeyType type = iButtonKey::KeyType::KeyDallas;
};