#pragma once
#include "key-info.h"
#include <array>

class RfidKey {
public:
    RfidKey();
    ~RfidKey();

    void set_type(LfrfidKeyType type);
    void set_data(uint8_t* data, const uint8_t data_size);

    LfrfidKeyType get_type();
    uint8_t* get_data();

    const char* get_type_text();
    const uint8_t get_type_data_count();

private:
    std::array<uint8_t, LFRFID_KEY_SIZE> data;
    LfrfidKeyType type;
};
