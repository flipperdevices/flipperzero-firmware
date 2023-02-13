#pragma once

#include <flipper_format.h>

#include <one_wire/one_wire_host.h>
#include <one_wire/one_wire_slave.h>

#define DALLAS_COMMON_MANUFACTURER_NAME "Dallas"

#define DALLAS_COMMON_CMD_READ_ROM 0x33U
#define DALLAS_COMMON_CMD_MATCH_ROM 0x55U
#define DALLAS_COMMON_CMD_SKIP_ROM 0xCCU
#define DALLAS_COMMON_CMD_COND_SEARCH 0xECU
#define DALLAS_COMMON_CMD_SEARCH_ROM 0xF0U

#define DALLAS_COMMON_CMD_READ_MEM 0xF0U

typedef union {
    struct {
        uint8_t family_code;
        uint8_t serial_number[6];
        uint8_t checksum;
    } fields;
    uint8_t bytes[8];
} DallasCommonRomData;

bool dallas_common_is_valid_crc(const DallasCommonRomData* rom_data);

bool dallas_common_read_rom(OneWireHost* host, DallasCommonRomData* rom_data);

bool dallas_common_read_mem(OneWireHost* host, uint16_t address, uint8_t* data, size_t data_size);

bool dallas_common_emulate_search_rom(OneWireSlave* slave, const DallasCommonRomData* rom_data);

bool dallas_common_emulate_read_rom(OneWireSlave* slave, const DallasCommonRomData* rom_data);

bool dallas_common_save_rom_data(FlipperFormat* ff, const DallasCommonRomData* rom_data);

bool dallas_common_load_rom_data(
    FlipperFormat* ff,
    uint32_t format_version,
    DallasCommonRomData* rom_data);
