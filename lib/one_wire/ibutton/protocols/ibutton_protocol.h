#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include <core/string.h>
#include <one_wire/one_wire_host.h>

typedef bool (*iButtonProtocolReadRom)(OneWireHost*, uint8_t*);
typedef bool (*iButtonProtocolReadUserData)(OneWireHost*, uint8_t*);

typedef struct {
    const uint8_t family_code;
    const size_t rom_size;
    const size_t user_data_size;

    iButtonProtocolReadRom read_rom;
    iButtonProtocolReadUserData read_user_data;
} iButtonProtocolBase;
