#include "dallas_common.h"

#include <core/common_defines.h>
#include <one_wire/maxim_crc.h>

#define BITS_IN_BYTE 8U

#define DALLAS_COMMON_ROM_DATA_KEY_V1 "Data"
#define DALLAS_COMMON_ROM_DATA_KEY_V2 "Rom Data"

#define DALLAS_COMMON_COPY_SCRATCH_TIMEOUT_US 100U
#define DALLAS_COMMON_COPY_SCRATCH_POLL_US 5U

bool dallas_common_skip_rom(OneWireHost* host) {
    onewire_host_write(host, DALLAS_COMMON_CMD_SKIP_ROM);
    return true;
}

bool dallas_common_read_rom(OneWireHost* host, DallasCommonRomData* rom_data) {
    onewire_host_write(host, DALLAS_COMMON_CMD_READ_ROM);
    onewire_host_read_bytes(host, rom_data->bytes, sizeof(DallasCommonRomData));

    return dallas_common_is_valid_crc(rom_data);
}

bool dallas_common_write_scratchpad(
    OneWireHost* host,
    uint16_t address,
    const uint8_t* data,
    size_t data_size) {
    onewire_host_write(host, DALLAS_COMMON_CMD_WRITE_SCRATCH);
    onewire_host_write(host, (uint8_t)address);
    onewire_host_write(host, (uint8_t)(address >> BITS_IN_BYTE));

    onewire_host_write_bytes(host, data, data_size);

    return true;
}

bool dallas_common_read_scratchpad(
    OneWireHost* host,
    DallasCommonAddressRegs* regs,
    uint8_t* data,
    size_t data_size) {
    onewire_host_write(host, DALLAS_COMMON_CMD_READ_SCRATCH);
    onewire_host_read_bytes(host, regs->bytes, sizeof(DallasCommonAddressRegs));
    onewire_host_read_bytes(host, data, data_size);

    return true;
}

bool dallas_common_copy_scratchpad(OneWireHost* host, const DallasCommonAddressRegs* regs) {
    onewire_host_write(host, DALLAS_COMMON_CMD_COPY_SCRATCH);
    onewire_host_write_bytes(host, regs->bytes, sizeof(DallasCommonAddressRegs));

    size_t time_elapsed;
    for(time_elapsed = 0; time_elapsed < DALLAS_COMMON_COPY_SCRATCH_TIMEOUT_US;
        time_elapsed += DALLAS_COMMON_COPY_SCRATCH_POLL_US) {
        if(!onewire_host_read_bit(host)) break;
        furi_delay_us(DALLAS_COMMON_COPY_SCRATCH_POLL_US);
    }

    return time_elapsed < DALLAS_COMMON_COPY_SCRATCH_TIMEOUT_US;
}

bool dallas_common_read_mem(OneWireHost* host, uint16_t address, uint8_t* data, size_t data_size) {
    onewire_host_write(host, DALLAS_COMMON_CMD_READ_MEM);

    onewire_host_write(host, (uint8_t)address);
    onewire_host_write(host, (uint8_t)(address > BITS_IN_BYTE));

    onewire_host_read_bytes(host, data, (uint16_t)data_size);

    return true;
}

bool dallas_common_emulate_search_rom(OneWireSlave* bus, const DallasCommonRomData* rom_data) {
    for(size_t i = 0; i < sizeof(DallasCommonRomData); i++) {
        for(size_t j = 0; j < BITS_IN_BYTE; j++) {
            bool bit = (rom_data->bytes[i] >> j) & 0x01;

            if(!onewire_slave_send_bit(bus, bit)) return false;
            if(!onewire_slave_send_bit(bus, !bit)) return false;

            onewire_slave_receive_bit(bus);
            // TODO: check for errors and return if any
        }
    }

    return true;
}

bool dallas_common_emulate_read_rom(OneWireSlave* bus, const DallasCommonRomData* rom_data) {
    return onewire_slave_send(bus, rom_data->bytes, sizeof(DallasCommonRomData));
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

bool dallas_common_is_valid_crc(const DallasCommonRomData* rom_data) {
    const uint8_t crc_calculated =
        maxim_crc8(rom_data->bytes, sizeof(DallasCommonRomData) - 1, MAXIM_CRC8_INIT);
    const uint8_t crc_received = rom_data->fields.checksum;

    return crc_calculated == crc_received;
}
