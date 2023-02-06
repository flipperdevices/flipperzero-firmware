#pragma once

#include <one_wire/one_wire_host.h>

typedef union {
    struct {
        uint8_t family_code;
        uint8_t serial_number[6];
        uint8_t checksum;
    } fields;
    uint8_t bytes[8];
} DallasCommonRomData;

bool dallas_common_read_rom(OneWireHost* host, DallasCommonRomData* rom_data);
