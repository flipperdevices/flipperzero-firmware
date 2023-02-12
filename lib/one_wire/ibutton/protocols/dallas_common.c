#include "dallas_common.h"

#include <one_wire/maxim_crc.h>

#define DALLAS_COMMON_CMD_READ_ROM (0x33U)
#define DALLAS_COMMON_CMD_READ_MEM (0xF0U)

#define DALLAS_COMMON_ROM_DATA_KEY_V1 "Data"
#define DALLAS_COMMON_ROM_DATA_KEY_V2 "Rom Data"

bool dallas_common_is_valid_crc(const DallasCommonRomData* rom_data) {
    const uint8_t crc_calculated =
        maxim_crc8(rom_data->bytes, sizeof(DallasCommonRomData) - 1, MAXIM_CRC8_INIT);
    const uint8_t crc_received = rom_data->fields.checksum;

    return crc_calculated == crc_received;
}

bool dallas_common_read_rom(OneWireHost* host, DallasCommonRomData* rom_data) {
    onewire_host_write(host, DALLAS_COMMON_CMD_READ_ROM);
    onewire_host_read_bytes(host, rom_data->bytes, sizeof(DallasCommonRomData));

    return dallas_common_is_valid_crc(rom_data);
}

bool dallas_common_read_mem(OneWireHost* host, uint16_t address, uint8_t* data, size_t data_size) {
    onewire_host_write(host, DALLAS_COMMON_CMD_READ_MEM);

    onewire_host_write(host, (uint8_t)address);
    onewire_host_write(host, (uint8_t)(address > 8));

    onewire_host_read_bytes(host, data, (uint16_t)data_size);
    return true;
}

bool dallas_common_save_rom_data(FlipperFormat* ff, const DallasCommonRomData* rom_data) {
    return flipper_format_write_hex(
        ff, DALLAS_COMMON_ROM_DATA_KEY_V2, rom_data->bytes, sizeof(DallasCommonRomData));
}

bool dallas_common_load_rom_data(
    FlipperFormat* ff,
    uint32_t format_version,
    DallasCommonRomData* rom_data) {
    switch(format_version) {
    case 1:
        return flipper_format_read_hex(
            ff, DALLAS_COMMON_ROM_DATA_KEY_V1, rom_data->bytes, sizeof(DallasCommonRomData));
    case 2:
        return flipper_format_read_hex(
            ff, DALLAS_COMMON_ROM_DATA_KEY_V2, rom_data->bytes, sizeof(DallasCommonRomData));
    default:
        return false;
    }
}
