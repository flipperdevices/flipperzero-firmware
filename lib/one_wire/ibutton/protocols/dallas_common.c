#include "dallas_common.h"

#include <one_wire/maxim_crc.h>

bool dallas_common_read_rom(OneWireHost* host, uint8_t* data) {
    onewire_host_read_bytes(host, data, DALLAS_COMMON_ROM_SIZE);

    const uint8_t crc_calculated = maxim_crc8(data, DALLAS_COMMON_ROM_SIZE - 1, MAXIM_CRC8_INIT);
    const uint8_t crc_received = data[DALLAS_COMMON_ROM_SIZE - 1];

    return crc_calculated == crc_received;
}
