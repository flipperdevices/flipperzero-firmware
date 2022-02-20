#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#include <lib/flipper_format/flipper_format.h>

typedef struct SubGhzBlockGeneric SubGhzBlockGeneric;

struct SubGhzBlockGeneric {
    const char* protocol_name;
    uint64_t data;
    uint32_t serial;
    uint8_t data_count_bit;
    uint8_t btn;
    uint16_t cnt;
};


bool subghz_block_generic_load_file(SubGhzBlockGeneric* instance, FlipperFormat* flipper_file);

bool subghz_block_generic_save_file(SubGhzBlockGeneric* instance, FlipperFormat* flipper_file);
