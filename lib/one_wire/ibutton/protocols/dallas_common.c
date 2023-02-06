#include "dallas_common.h"

#include <one_wire/maxim_crc.h>

#define DALLAS_COMMON_CMD_READ_ROM (0x33U)

bool dallas_common_read_rom(OneWireHost* host, DallasCommonRomData* rom_data) {
    onewire_host_write(host, DALLAS_COMMON_CMD_READ_ROM);
    onewire_host_read_bytes(host, rom_data->bytes, sizeof(DallasCommonRomData));

    const uint8_t crc_calculated = maxim_crc8(rom_data->bytes, sizeof(DallasCommonRomData) - 1, MAXIM_CRC8_INIT);
    const uint8_t crc_received = rom_data->fields.checksum;

    return crc_calculated == crc_received;
}
